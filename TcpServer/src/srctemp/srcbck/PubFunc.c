/**********************************************************************\
 *
 * File: PubFunc.cpp
 *
 *	NOTE!!! 使用VI打开此文件时, 请将 tablespace设置为4 (:set ts=4)
 *
\**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "SysVar.h"
#include "PubFunc.h"
/*#define _debug_*/
static int sgDaysOfMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

/*********************************************************************\
 * Function:	ValidateDate
 * Action:		校验日期格式是否合法
 * 						如:		20070212-合法
 *								20070230-不合法
 * Input:	const char	*s_date
 *			const int	in_format
 * 				1	-	20070707
 * 				2	-	2007/07/07 || 2007-07-07
 * Output:			无
 * Return:	int
 * 				 0 - 格式合法
 *				-1 - 格式不合法
 *********************************************************************
 * 此方法使用到了全局变量  sgDaysOfMonth 见文件头部
 *********************************************************************
 * 使用方法典型例子:
 *		1)
 *			if( ValidateDate( "20070707", 1 ) == 0 )
 *			{
 *				printf( "[format is ok]\n" );
 *			}
 *			else
 *			{
 *				printf( "[format is't ok]\n" );
 *			}
 *		2)
 *			if( ValidateDate( "2007/07/07", 2 ) == 0 )
 *			{
 *				printf( "[format is ok]\n" );
 *			}
 *			else
 *			{
 *				printf( "[format is't ok]\n" );
 *			}
\*********************************************************************/
int ValidateDate( const char *in_strdate, int in_format )
{
	char	year[2+1];
	char	month[2+1];
	char	day[2+1];
	int		y=0;
	int		m=0;
	int		d=0;
	int		days=0;

	switch( in_format )
	{
		case 1:
				if( strlen(in_strdate) != 8 )
				return( -1 );
				break;
		case 2:
				if( strlen(in_strdate) != 10 )
				return( -1 );
				break;
		default:
				return( -1 );
	}

	memset( year, 0x00, sizeof(year) );
	memset( month, 0x00, sizeof(month) );
	memset( day, 0x00, sizeof(day) );
	if( in_format == 1 )
	{
		memcpy( year, in_strdate, 4 );
		memcpy( month, in_strdate+4, 2 );
		memcpy( day, in_strdate+6, 2 );
	}
	else
	{
		memcpy( year, in_strdate, 4 );
		memcpy( month, in_strdate+5, 2 );
		memcpy( day, in_strdate+8, 2 );
	}
	
	if(isdigit(year[0])||isdigit(year[1])||isdigit(year[2])||isdigit(year[3])||isdigit(month[0])||isdigit(month[1])||isdigit(day[0])||isdigit(day[1]))
	{
		return( -1 );
	}

	y = atoi(year);
	m = atoi(month);
	d = atoi(day);
	
	if( y<1900 || y>2900 )
	{
		return( -1 );
	}
	if( m<1 || m>12 )
	{
		return( -1 );
	}
	if( ((y%4==0 && y%100!=0) || y%400==0) && (m==2) )
	{
		days = 29;
	}
	else
	{
		days = sgDaysOfMonth[m - 1];
	}
	
	if( d<1 || d>days )
	{
		return ( -1 );
	}

	return( 0 );
}

/*************************************************************************************\
 * Function:	GetSysDateTime
 * Action:		获取系统的日期和时间
 *
 * Input:		int option 想要获取时间的格式
 *					取值如下：
 *						1 ------ YYYYMMDD
 *						2 ------ YYYY/MM/DD
 *						3 ------ YYYY-MM-DD
 *						4 ------ HHMMSS
 *						5 ------ HH:MM:SS
 *						6 ------ YYYYMMDD HHMMSS
 *						7 ------ YYYY/MM/DD HH:MM:SS
 *						8 ------ YYYYMMDD HH:MM:SS
 *
 *				int out_str_len
 *					当输出参数 out_str不为NULL时此参数是输出时间串的长度(此时必须>0)
 *					当 out_str 为NULL时此参数无用(此时必须=0)
 *
 * Output:		char *out_str
 *					如果 out_str==NULL 则通过返回值来返回指定格式的时间串
 *					否则也要通过 out_str 来返回时间串
 *
 * Return:		返回指定格式的时间串
 *	 				如果返回NULL表示输参数不合法
 *
 *************************************************************************************
 * 使用方法典型例子:
 *		1)
 *			char date_time[32];
 *			memset( date_time, 0x00, sizeof(date_time) );
 *			GetSysDateTime( date_time, sizeof(date_time), @ ); @ 可取值 1 到 8
 *			printf( "[time=%s]\n", date_time );
 *		2)
 *			printf( "[time=%s]\n", GetSysDateTime(NULL,0,#) ); # 可取值 1 到 8
\*************************************************************************************/
char *
GetSysDateTime( char *out_str, int out_str_len, int option )
{
	time_t			now_clock = 0;
	struct			tm *tm_ptr = NULL;
	static char		now_datetime[32];

	memset( now_datetime, 0x00, sizeof(now_datetime) );
	
	if( option<1 || option>8 )
	{
		return( NULL );
	}
	
	if( ( out_str!=NULL && out_str_len<=0 ) ||
		( out_str==NULL && out_str_len!=0 ) )
	{
		return( NULL );
	}

	time( &now_clock );
	tm_ptr = (struct tm*)localtime( &now_clock );

	switch( option )
	{
		case 1:
			snprintf( now_datetime, sizeof(now_datetime), "%04d%02d%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime );
			}
			return( now_datetime );
		case 2:
			snprintf( now_datetime, sizeof(now_datetime), "%04d/%02d/%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime );
			}
			return( now_datetime );
		case 3:
			snprintf( now_datetime, sizeof(now_datetime), "%04d-%02d-%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime );
			}
			return( now_datetime );
		case 4:
			snprintf( now_datetime, sizeof(now_datetime), "%02d%02d%02d",
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime);
			}
			return( now_datetime );
		case 5:
			snprintf( now_datetime, sizeof(now_datetime), "%02d:%02d:%02d",
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime);
			}
			return( now_datetime );
		case 6:
			snprintf( now_datetime, sizeof(now_datetime), "%04d%02d%02d %02d%02d%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime);
			}
			return( now_datetime );
		case 7:
			snprintf( now_datetime, sizeof(now_datetime), "%04d/%02d/%02d %02d:%02d:%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime);
			}
			return( now_datetime );
		case 8:
			snprintf( now_datetime, sizeof(now_datetime), "%04d%02d%02d %02d:%02d:%02d",
			tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
			tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec );
			if( out_str != NULL )
			{
				snprintf( out_str, out_str_len, "%s", now_datetime);
			}
			return( now_datetime );
		default:
			return( NULL );
	}
}

/*************************************************************************************\
 * Function:	GetSysDate
 * Action:		只有日志函数才能使用此函数，得到系统日期，其他应用请使用 GetSysDateTime
 *
 * Return:		返回 "YYYYMMDD" 格式的时间串
 *	 				如果返回 NULL 表示系统错误
 *
 *************************************************************************************
 * 使用方法典型例子:
 *		1)
 *			printf( "[date=%s]\n", GetSysDate() );
\*************************************************************************************/
char *
GetSysDate(void)
{
	time_t			now_clock = 0;
	struct			tm *tm_ptr = NULL;
	static char		now_date[32];

	memset( now_date, 0x00, sizeof(now_date) );
	
	time( &now_clock );
	tm_ptr = (struct tm*)localtime( &now_clock );

	snprintf( now_date, sizeof(now_date), "%04d%02d%02d",
	tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday );

	return( now_date );
}

/******************************************************************************************************\
 * Function : DBOSSLog
 * Action   : 调试日志打印函数，如果是出错日志必须调用 EBOSSLog 正常打日志打印请使用 BOSSLog
 * Input    : 文件名，格式，...
 * Output   :
 * Return   : 0 -- 成功；-1 -- 文件打开错；-2 -- 写文件错
 ******************************************************************************************************
 * 使用此函数之前需要初始化 打日志标志 gLogFlag 见文件头部
 *				只有 gLogFlag == 1 才会打印日志
 ******************************************************************************************************
 * 
 * 使用方法典型例子:
 *		DBOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			其中的日志文件名必须是绝对路径
\******************************************************************************************************/
int
DBOSSLog( char *filename, const char *fmt, ... )
{
	char	commandstr[256];
	char	logpath[256];
	char	filefullname[256];
	va_list	ap;
	FILE	*fp=NULL;
	int		fd=-1;

	memset( commandstr, 0, sizeof(commandstr) );
	memset( logpath, 0, sizeof(logpath) );
	memset( filefullname, 0, sizeof(filefullname) );
	
	if( gLogFlag==1 )
	{
		snprintf( logpath, sizeof(logpath), filename );
		if( access(dirname(logpath),F_OK) != 0 )
		{
			snprintf( commandstr, sizeof(commandstr), "mkdir -p %s", dirname(logpath) );
			system( commandstr );
		}
		snprintf( filefullname, sizeof(filefullname),
				"%s.%s", filename, GetSysDate() );
		if( ( fp=fopen(filefullname,"a") ) == NULL )
		{
			return ( -1 );
		}

		/* 为避免写入混乱将文件加锁 */
		fd = fileno(fp);
		lockf(fd, F_LOCK, 0l);

		/* 写入文件 */
		va_start( ap, fmt );
		if( vfprintf(fp, fmt, ap) < 0)
		{
			printf("ERROR: Call fopen!\n");
			return ( -2 );
		}
		va_end( ap );
		/* 解锁 */
		lockf(fd, F_ULOCK, 0l);
		/* 关闭文件 */
		fclose(fp);
	}

	return ( 0 );
}

/******************************************************************************************************\
 * Function : EBOSSLog
 * Action   : 出错日志必须调用此函数打印日志，调试日志打印不能使用此函数，而应该用 DBOSSLog
 *						正常日志函数请使用 BOSSLog
 * Input    : 文件名，格式，...
 * Output   :
 * Return   : 0 -- 成功；-1 -- 文件打开错；-2 -- 写文件错
 ******************************************************************************************************
 * 
 * 使用方法典型例子:
 *		EBOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			其中的日志文件名必须是绝对路径
\******************************************************************************************************/
int
EBOSSLog( char *filename, const char *fmt, ... )
{
	char	commandstr[256];
	char	logpath[256];
	char	filefullname[256];
	va_list	ap;
	FILE	*fp=NULL;
	int		fd=-1;

	memset( commandstr, 0, sizeof(commandstr) );
	memset( logpath, 0, sizeof(logpath) );
	memset( filefullname, 0, sizeof(filefullname) );
	
	snprintf( logpath, sizeof(logpath), filename );
	if( access(dirname(logpath),F_OK) != 0 )
	{
		snprintf( commandstr, sizeof(commandstr), "mkdir -p %s", dirname(logpath) );
		system( commandstr );
	}

	snprintf( filefullname, sizeof(filefullname),
			"%s.%s", filename, GetSysDate() );

	if( ( fp=fopen(filefullname,"a") ) == NULL )
	{
		return ( -1 );
	}

	/* 为避免写入混乱将文件加锁 */
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);

	/* 写入文件 */
	va_start( ap, fmt );
	if( vfprintf(fp, fmt, ap) < 0)
	{
		printf("ERROR: Call fopen!\n");
		return ( -2 );
	}
	va_end( ap );
	/* 解锁 */
	lockf(fd, F_ULOCK, 0l);
	/* 关闭文件 */
	fclose(fp);
	return ( 0 );
}

/******************************************************************************************************\
 * Function : BOSSLog
 * Action   : 打印日志函数，调试日志打印不能使用此函数，而应该用 DBOSSLog
 *						出错日志函数使用 EBOSSLog
 * Input    : 文件名，格式，...
 * Output   :
 * Return   : 0 -- 成功；-1 -- 文件打开错；-2 -- 写文件错
 ******************************************************************************************************
 * 
 * 使用方法典型例子:
 *		BOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			其中的日志文件名必须是绝对路径
\******************************************************************************************************/
int
BOSSLog( char *filename, const char *fmt, ... )
{
	char	commandstr[256];
	char	logpath[256];
	char	filefullname[256];
	va_list	ap;
	FILE	*fp=NULL;
	int		fd=-1;

	memset( commandstr, 0, sizeof(commandstr) );
	memset( logpath, 0, sizeof(logpath) );
	memset( filefullname, 0, sizeof(filefullname) );
	
	snprintf( logpath, sizeof(logpath), filename );
	if( access(dirname(logpath),F_OK) != 0 )
	{
		snprintf( commandstr, sizeof(commandstr), "mkdir -p %s", dirname(logpath) );
		system( commandstr );
	}

	snprintf( filefullname, sizeof(filefullname),
			"%s.%s", filename, GetSysDate() );

	if( ( fp=fopen(filefullname,"a") ) == NULL )
	{
		return ( -1 );
	}

	/* 为避免写入混乱将文件加锁 */
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);

	/* 写入文件 */
	va_start( ap, fmt );
	if( vfprintf(fp, fmt, ap) < 0)
	{
		printf("ERROR: Call fopen!\n");
		return ( -2 );
	}
	va_end( ap );
	/* 解锁 */
	lockf(fd, F_ULOCK, 0l);
	/* 关闭文件 */
	fclose(fp);
	return ( 0 );
}

/*********************************************************************\
 * Function: RTrim
 * Action:	去掉字符串右面的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
char *
RTrim( char *in_str )
{
	int	i=0;
	int in_str_len=0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}
	
	in_str_len = strlen(in_str);
	for( i=in_str_len-1; i>=0; i-- )
	{
		if( in_str[i]==0x20 || in_str[i]==0x09 ||	in_str[i]==0x0a || in_str[i]==0x0d )
		{
			in_str[i]=0;
		}
		else
		{
			break;
		}
	}

	return( in_str );
}

/*********************************************************************\
 * Function: LTrim
 * Action:	去掉字符串左面的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
char *
LTrim( char *in_str )
{
	int	i=0;
	int	j=0;
	int in_str_len=0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}
	
	in_str_len = strlen(in_str);
	for( i=0; i<in_str_len; i++ )
	{
		if( in_str[i]!=0x20 && in_str[i]!=0x09 &&
			in_str[i]!=0x0a && in_str[i]!=0x0d )
		{
			break;
		}
	}
	
	memmove( in_str, in_str+i, in_str_len-i );
	in_str[in_str_len-i]=0;
	return( in_str );
}

/*********************************************************************\
 * Function: Trim
 * Action:	去掉字符串左右两边的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
char *
Trim( char *in_str )
{
	return( LTrim(RTrim(in_str)) );
}

/***************************************************************************************************************\
 *	堆排序2007-04-17 14:17堆排序(Heap Sort)
 *
 *
 *	1.	基本思想：
 *			堆排序是一树形选择排序，在排序过程中，将R[1..N]看成是一颗完全二叉树的顺序存储结构
 *			利用完全二叉树中双亲结点和孩子结点之间的内在关系来选择最小的元素。
 *
 *
 *	2.	堆的定义: N个元素的序列K1,K2,K3,...,Kn.称为堆，当且仅当该序列满足特性：
 *			Ki≤K2i
 *
 *			Ki ≤K2i+1(1≤ I≤ [N/2])
 *
 *		堆实质上是满足如下性质的完全二叉树：
 *			树中任一非叶子结点的关键字均大于等于其孩子结点的关键字。
 *			例如序列10,15,56,25,30,70就是一个堆。这种堆中根结点（称为堆顶）的关键字最小，我们把它称为小根堆。
 *			反之，若完全二叉树中任一非叶子结点的关键字均大于等于其孩子的关键字，则称之为大根堆。
 *
 *
 *	3.	排序过程：
 *			堆排序正是利用小根堆（或大根堆）来选取当前无序区中关键字小（或最大）的记录实现排序的。
 *			我们不妨利用大根堆来排序。
 *			每一趟排序的基本操作是：
 *				将当前无序区调整为一个大根堆，选取关键字最大的堆顶记录，将它和无序区中的最后一个记录交换。
 *				这样，正好和直接选择排序相反，有序区是在原记录区的尾部形成并逐步向前扩大到整个记录区。
 *
 *
 *	用大根堆排序的基本思想
 *		① 先将初始文件R[1..n]建成一个大根堆，此堆为初始的无序区
 *		② 再将关键字最大的记录R[1](即堆顶)和无序区的最后一个记录R[n]交换，
 *			由此得到新的无序区R[1..n-1]和有序区R[n]，且满足R[1..n-1].keys≤R[n].key
 *		③ 由于交换后新的根R[1]可能违反堆性质，故应将当前无序区R[1..n-1]调整为堆。
 *			然后再次将R[1..n-1]中关键字最大的记录R[1]和该区间的最后一个记录R[n-1]交换，
 *			由此得到新的无序区R[1..n-2]和有序区R[n-1..n]，
 *			且仍满足关系R[1..n-2].keys≤R[n-1..n].keys，同样要将R[1..n-2]调整为堆。
 *			......
 *			直到无序区只有一个元素为止。
 *
 *		① Heapify函数思想方法
 *			每趟排序开始前R[l..i]是以R[1]为根的堆，在R[1]与R[i]交换后，
 *			新的无序区R[1..i-1]中只有R[1]的值发生了变化，故除R[1]可能违反堆性质外，
 *			其余任何结点为根的子树均是堆。因此，当被调整区间是R[low..high]时，只须调整以R[low]为根的树即可。
 *		"筛选法"调整堆
 *			R[low]的左、右子树(若存在)均已是堆，这两棵子树的根R[2low]和R[2low+1]分别是各自子树中关键字最大的结点。
 *			若R[low].key不小于这两个孩子结点的关键字，则R[low]未违反堆性质，以R[low]为根的树已是堆，无须调整；
 *			否则必须将R[low]和它的两个孩子结点中关键字较大者进行交换，
 *			即R[low]与R[large](R[large].key=max(R[2low].key，R[2low+1].key))交换。
 *			交换后又可能使结点R[large]违反堆性质，同样由于该絒large]为根的树进行调整。
 *			此过程直至当前被调整的结点已满足堆性质，或者该结点已是叶子为止。
 *			上述过程就象过筛子一样，把较小的关键字逐层筛下去，而将较大的关键字逐层选上来。
 *			因此，有人将此方法称为"筛选法"。
\***************************************************************************************************************/

/*************************************************************************************\
 * Function:	HeapAdjust
 * Action:		对整形数组进行堆调整
 *
 * Input:		int	*ele_array	-- 为待排序的结构数组的起始地址
 *				int	s
 *				int	m
 * Output:		输出与输入是同一个
 * Return:		无
\*************************************************************************************/
static void
HeapAdjust( int *ele_array, int s, int m )
{
	/*********************************************************************************
		已知ele_array[s..m]中除ele_array[s]之外均满足堆的定义,本函数调整ele_array[s]
		使ele_array[s..m]成为一个大顶堆
	**********************************************************************************/

	int j,rc;
	rc=ele_array[s];

	for(j=2*(s+1)-1;j<=m;j=(j+1)*2-1)		/* 沿关键字较大的结点向下筛选 */
	{
		if(j<m&&ele_array[j]<ele_array[j+1])
			++j; 				/* j为关键字较大的记录的下标 */
		if(rc>=ele_array[j])
			break; 				/* rc应插入在位置s上 */
		ele_array[s]=ele_array[j];
		s=j;
	}

	ele_array[s]=rc;			/* 插入 */
}

/*******************************************************************************************\
 * Function:	HeapSort
 * Action:		对整形数组进行堆排序
 *
 * Input:		int	*ele_array	-- 为待排序的结构数组的起始地址
 *				int	ele_num		-- 为待排序的结构数组的元素个数
 * Output:		输出与输入是同一个
 * Return:
 *			 0	-- 成功
 *			-1	-- 输入参数不合法
\*******************************************************************************************/
int
HeapSort( int *ele_array, int ele_num )
{
	int i=0;
	int j=0;
	int t=0;
	
	if( ele_array==NULL || ele_num<=0 )
	{
		return( -1 );
	}

	for( i=ele_num/2-1; i>=0; --i )			/* 把ele_array[0..ele_num-1]建成大顶堆 */
		HeapAdjust( ele_array, i, ele_num-1 );

	for( i=ele_num-1; i>0; --i )
	{
		t = ele_array[0];					/* 将堆顶记录和当前未经排序子序列ele_array[0..i] */
		ele_array[0] = ele_array[i];		/* 中的最后一个记录相互交换 */
		ele_array[i] = t;
		HeapAdjust( ele_array, 0, i-1 );	/* 将ele_array[0..i-1]重新调整为大顶堆 */
	}

	return( 0 );
}

/*******************************************************************************************\
 * Function:	partition
 * Action:		对整形数组进行快速排序
 *
 * Input:		int	*ele_array	-- 为待排序的整形数组的起始地址
 *				int	lower		-- 为待排序的整形数组的开始元素下标
 *				int	higher		-- 为待排序的整形数组的结束元素个数
 * Output:		输出与输入是同一个
 * Return:
 *			分界元素下标
\*******************************************************************************************/
static int
partition( int *ele_array, int low, int high )
{
	int li_save = ele_array[low];
	while( low<high ) 
	{
		while( low<high && ele_array[high]>=li_save )
			high--;
		ele_array[low] = ele_array[high];

		while( low<high && ele_array[low]<=li_save )
			low++;
		ele_array[high] = ele_array[low];
	}
	ele_array[low] = li_save;
	return low;
}

/*******************************************************************************************\
 * Function:	quicksort
 * Action:		对整形数组进行快速排序
 *
 * Input:		int	*ele_array	-- 为待排序的整形数组的起始地址
 *				int	lower		-- 为待排序的整形数组的开始元素下标
 *				int	higher		-- 为待排序的整形数组的结束元素个数
 * Output:		输出与输入是同一个
 * Return:
 *			 0	-- 成功
 *			-1	-- 输入参数不合法
\*******************************************************************************************/
static void
quicksort( int *ele_array, int low,int high )
{
	if( low<high )
	{
		int pivotloc = partition( ele_array, low, high );
		quicksort( ele_array, low, pivotloc-1 );
		quicksort( ele_array, pivotloc+1, high );
	}
}

/*******************************************************************************************\
 * Function:	QuickSort
 * Action:		对整形数组进行快速排序
 *
 * Input:		int	*ele_array	-- 为待排序的整形数组的起始地址
 *				int	ele_num		-- 为待排序的整形数组的元素个数
 * Output:		输出与输入是同一个
 * Return:
 *			 0	-- 成功
 *			-1	-- 输入参数不合法
\*******************************************************************************************/
int
QuickSort( int *ele_array, int ele_num )
{
	if( ele_array==NULL || ele_num<=0 )
	{
		return( -1 );
	}

	quicksort( ele_array, 0, ele_num-1 );

	return( 0 );
}

/*********************************************************************\
 * Function:	ToUpper
 * Action:		将字符串转化成大写
 *
 * Input:		char *in_str
 * Output:		同 Input
 * Return:		同 Output
\*********************************************************************/
char *
ToUpper( char *in_str )
{
	int in_str_len = 0;
	int i = 0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}

	in_str_len = strlen( in_str );

	for( i=0; i<in_str_len; i++ )
	{
		if( in_str[i]>='a' && in_str[i]<='z' )
		{
			in_str[i]-=32;
		}
	}
	return( in_str );
}

/*********************************************************************\
 * Function:	ToLower
 * Action:		将字符串转化成小写
 *
 * Input:		char *in_str
 * Output:		同 Input
 * Return:		同 Output
\*********************************************************************/
char *
ToLower( char *in_str )
{
	int in_str_len = 0;
	int i = 0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}

	in_str_len = strlen( in_str );

	for( i=0; i<in_str_len; i++ )
	{
		if( in_str[i]>='A' && in_str[i]<='Z' )
		{
			in_str[i]+=32;
		}
	}
	return( in_str );
}

/*********************************************************************\
 * Function:	ParseUPS
 * Action:		对 ORACLE 的链接串进行解析 串格式见输入参数 i_source_str
 *
 * Input:		char *i_source_str
 *					格式 1: uname/passwd
 *					格式 2: uname/passwd@localservice
 * Output:
 *				char *o_uname -- 用户名输出缓冲地址
 *				char *o_uname_len -- 用户名输出缓冲长度
 *				char *o_passwd -- 密码输出缓冲地址
 *				char *o_passwd_len -- 密码输出缓冲长度
 *				char *o_sname -- ORACLE本地服务名串 输出缓冲地址
 *				char *o_sname_len -- ORACLE本地服务名串 输出缓冲长度
 * Return:
 *			0	- 成功
 *			-1	- 参数不合法
 *			-2	- 串格式不合法
\*********************************************************************/
int
DBParseUPS(
	char *o_uname, int o_uname_len,
	char *o_passwd, int o_passwd_len,
	char *o_sname, int o_sname_len,
	const char *i_source_str )
{
	char	str_temp[128+1];
	char	*p_at=NULL;
	char	*p_slash=NULL;
	
	memset( str_temp, 0x00, sizeof(str_temp) );

	if( o_uname==NULL || o_uname_len<=0 ||
		o_passwd==NULL || o_passwd_len<=0 ||
		o_sname==NULL || o_sname_len<=0 ||
		i_source_str==NULL || strlen(i_source_str)==0 )
	{
		return( -1 );
	}
	
	snprintf( str_temp, sizeof(str_temp), i_source_str );

	p_slash = strchr(str_temp,'/');
	if( p_slash==NULL )
	{
		return( -2 );
	}

	p_at = strchr(str_temp,'@');

	if( p_at!=NULL )
	{
		if( p_slash>p_at )
		{
			*p_at=0;
			p_at++;
			*p_slash=0;
			p_slash++;
			snprintf( o_uname, o_uname_len, str_temp );
			snprintf( o_sname, o_sname_len, p_at );
			snprintf( o_passwd, o_passwd_len, p_slash );
		}
		else
		{
			*p_slash=0;
			p_slash++;
			*p_at=0;
			p_at++;
			snprintf( o_uname, o_uname_len, str_temp );
			snprintf( o_passwd, o_passwd_len, p_slash );
			snprintf( o_sname, o_sname_len, p_at );
		}
	}
	else
	{
		*p_slash=0;
		snprintf( o_uname, o_uname_len, str_temp );
		p_slash++;
		snprintf( o_passwd, o_passwd_len, p_slash );
	}
	
	return( 0 );
}

/*********************************************************************\
 * Function:	ReadConfig
 * Action:		读取配置文件中的各个配置参数
 *
 * Input:		const char *i_cfgfile	-	参数所在文件名
 *							此文件名必须是相对于绝对路径文件名
 *							此文件内容说明：
 *								此文件中"参数类别"不能重复
 *								同一个"参数类别"下不能有相同的"参数名"
 *				const char *i_section	-	参数类别
 *				const char *i_key		-	参数名
 * Output:
 *				char *o_value			-	参数值输出缓冲地址
 *				char *o_value_len		-	参数值输出缓冲长度
 * Return:
 *			 0	- 成功
 *			-1	- 参数不合法
 *			-2	- 打开文件出错
 *			-3	- 找不到参数类别 section
 *			-4	- 找不到参数 key
\*********************************************************************/
int
ReadConfig( const char *i_cfgfile,
			const char *i_section,
			const char *i_key,
			char *o_value,
			int o_value_len )
{
	FILE	*fp = NULL;
	char	section_name[64];
	char	key_name[64];
	char	value[512];
	char	line_buf[512];
	char	ifindflag = 0;
	char	*p_assign = NULL;
	char	*p_comment = NULL;
	char	*p_left_bracket = NULL;
	char	*p_right_bracket = NULL;

	memset(section_name,0x00,sizeof(section_name));
	memset(key_name,0x00,sizeof(key_name));
	memset(value,0x00,sizeof(value));
	memset(line_buf,0x00,sizeof(line_buf));
	
	if( i_cfgfile==NULL ||i_section==NULL ||i_key==NULL ||o_value==NULL ||o_value_len<=0 )
	{
		return( -1 );
	}
	
	if( (fp=fopen(i_cfgfile,"r"))==NULL )
	{
		return( -2 );
	}
	
	snprintf( section_name, sizeof(section_name), "[%s]", i_section );

	/* -- 查找 section --*/
	while( fgets( line_buf, sizeof(line_buf), fp ) != NULL )
	{
		Trim(line_buf);
		if( line_buf[0]=='#' )
		{
			memset( line_buf, 0x00, sizeof(line_buf) );
			continue;
		}
		else
		{
			if( ( p_comment=strchr(line_buf,'#') ) != NULL )
			{
				*p_comment = 0;
				RTrim(line_buf);
			}
			if( strcmp(line_buf, section_name) != 0 )
			{
				memset( line_buf, 0x00, sizeof(line_buf) );
				continue;
			}
			ifindflag = 1;
			break;
		}
	}

	if( ifindflag==0 )
	{
		fclose( fp );
		return( -3 );
	}
	
	/* -- 查找 key --*/
	ifindflag = 0;
	memset( line_buf, 0x00, sizeof(line_buf) );
	snprintf( key_name, sizeof(key_name), "%s", i_key );

	while( fgets( line_buf, sizeof(line_buf), fp ) != NULL )
	{
		Trim(line_buf);
		if( line_buf[0]=='#' )
		{
			memset( line_buf, 0x00, sizeof(line_buf) );
			continue;
		}
		else
		{
			if( ( p_comment=strchr(line_buf,'#') ) != NULL )
			{
				*p_comment = 0;
				RTrim(line_buf);
			}
			if( ( ( p_left_bracket=strchr(line_buf,'[') ) != NULL ) &&
					( ( p_right_bracket=strchr(line_buf,']') ) != NULL ) &&
					( p_right_bracket > p_left_bracket)  )
			{
				break;
			}
			if( (p_assign=strchr(line_buf,'='))==NULL )
			{
				memset( line_buf, 0x00, sizeof(line_buf) );
				continue;
			}
			else
			{
				*p_assign = 0;
				RTrim(line_buf);
				LTrim(p_assign+1);
				if( strcmp(line_buf, key_name)!=0 )
				{
					memset( line_buf, 0x00, sizeof(line_buf) );
					continue;
				}
				else
				{
					snprintf( value, sizeof(value), p_assign+1 );
					ifindflag = 1;
				}
			}
		}
	}
	
	fclose( fp );

	if( ifindflag==1 )
	{
		snprintf( o_value, o_value_len, value );
		return( 0 );
	}
	return( -4 );
}

/*********************************************************************\
 * Function:	ReadItems
 * Action:		读取字符串缓冲中的各个字段
 *
 * Input:		const char *i_str	-	输入字符串缓冲
 *				char *i_separator	-	输入串中各个字段的分隔符
 * Output:
 *				char	**o_pstr	-	输出二维缓冲地址
 *				int		o_num		-	应用需要的字段的个数
 *				int		o_size		-	每个字段最大长度
 * Return:
 *			> 0 - 获取参数个数
 *			  0 - 没有获取到任何字段
 *			 -1	- 参数不合法
\*********************************************************************/
int
ReadItems( const char *i_str, const char *i_separator,
		char *o_pstr, int o_num, int o_size )
{
	int		count = 0;
	char	str_buff[1024+1];
	char	*p_separator=NULL;
	char	*p_item=NULL;

	memset( str_buff, 0x00, sizeof(str_buff) );
	
	if( i_separator==NULL || strlen(i_separator)<=0 ||
		o_pstr==NULL || o_num<=0 || o_size<=0 )
	{
		return( -1 );
	}

	if( i_str==NULL || strlen(i_str)<=0 )
	{
		return( 0 );
	}
	
	snprintf( str_buff, sizeof(str_buff), i_str );
	p_item = str_buff;
	
	while( count < o_num )
	{
		if( ( p_separator=strstr(p_item,i_separator) ) != NULL )
		{
			*p_separator = 0;
			snprintf( &(o_pstr[count*o_size]), o_size, p_item );
			p_item = p_separator+strlen(i_separator);
			count++;
		}
		else
		{
			if( strlen(p_item)<=0 )
			{
				break;
			}
			snprintf( &(o_pstr[count*o_size]), o_size, p_item );
			count++;
			break;
		}
	}

	return( count );
}


/************************************************************************\
 *	Function:	ReplaceEnvString
 *	Action:		替换字符串中的环境变量为实际的内容
 *					使用此函数的前提是：
 *						串中最多含有一个环境变量
 *			如:
 *				$ADAPTERDIR/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *				$(ADAPTERDIR)/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *				${ADAPTERDIR}/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *
 *	Input:		char *filename  -- 可能是带有环境变量的字符串
 *				int 			-- 输入串作为输出串是，输出的最大长度
 *	Output:		char *filename  -- 环境变量已经被替换的字符串
 *	Return:
 *			 0 - 转换成功
 *			 1 - 串中有两个或者两个以上的环境变量
 *			-1 - 环境变量不存在
\************************************************************************/
int ReplaceEnvString( char *filename, int size )
{
	char	*p_dollar = NULL;
	char	*p_slash = NULL;

	char	str_env[64];
	char	str_head[256];
	char	str_tail[256];

	memset( str_env, 0x00, sizeof(str_env) );
	memset( str_head, 0x00, sizeof(str_head) );
	memset( str_tail, 0x00, sizeof(str_tail) );

	if( filename==NULL || strlen(filename)==0 )
	{
		return( 0 );
	}

	if( (p_dollar=strchr(filename,'$')) == NULL )
	{
		return( 0 );
	}

	snprintf( str_head, sizeof(str_head), "%.*s", p_dollar-filename, filename );
	
	if( (p_slash=strchr(p_dollar+1,'/')) == NULL )
	{
		snprintf( str_env, sizeof(str_env), p_dollar+1 );
	}
	else
	{
		snprintf( str_env, sizeof(str_env), "%.*s", p_slash-(p_dollar+1), p_dollar+1 );
		snprintf( str_tail, sizeof(str_tail), "%s", p_slash );
	}

	if( str_env[0]=='(' && str_env[strlen(str_env)-1]==')' ||
		str_env[0]=='{' && str_env[strlen(str_env)-1]=='}' )
	{
		memmove( str_env, str_env+1, strlen(str_env)-2 );
		str_env[strlen(str_env)-2]=0;
	}
	
	if( getenv(str_env) == NULL )
	{
		return( -1 );
	}
	
	memset( filename, 0x00, size );
	snprintf( filename, size, "%s%s%s", str_head, getenv(str_env), str_tail );

	if( strchr(filename,'$') != NULL )
	{
		return( 1 );
	}

	return( 0 );
}



/*
 *功能:将第三方的查询参数，转化成BOSS调用tuxedo的输入形式
 *作者:王良 
 *日期:2008年4月18日
 *输入:
 *输出:
 */
 
int changeParaToTuxedoInput(struct str_PARA *pPara, int iQueryType,struct tagTuxedoCall *pTuxedoCall){
	int		iReturn = -1;
	char	chQueryType[4+1];
	char	chValue[256];
	char 	*pchSplitData;
	char	*pPos; 
	int		iPos = 0;
	char	chItemValue[64+1];
	char	chI[32+1];
	char	chJ[32+1];
	
	memset(chQueryType, 0, sizeof(chQueryType));
	sprintf(chQueryType, "[%04d]", iQueryType);

	#ifdef _debug_
		printf("chQueryType=[%s]\n",chQueryType);
	#endif
	
	/*读取输入参数设置参数*/
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"INPUT_NUM",chValue);
	pTuxedoCall->iInputNum = atoi(chValue);

	#ifdef _debug_
		printf("INPUT_NUM=[%s]\n",chValue);
	#endif

	/*读取输出参数设置参数*/
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"OUTPUT_NUM",chValue);
	pTuxedoCall->iOutputNum = atoi(chValue);
	
	#ifdef _debug_
		printf("OUTPUT_NUM=[%s]\n",chValue);
	#endif
	
	/*读取服务名称设置参数*/
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"SERVER_NAME_ENGLISH",chValue);
	strcpy(pTuxedoCall->chServName,chValue);
	
	#ifdef _debug_
		printf("SERVER_NAME_ENGLISH=[%s]\n",chValue);
	#endif
	
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"PARAM_TRANS",chValue);
	pchSplitData = chValue;
	#ifdef _debug_
		printf("PARAM_TRANS=[%s]\n",chValue);
	#endif	
	
	while(0 != strlen(pchSplitData))
	{
  		pPos = strchr(pchSplitData, ';');
    	if (pPos != NULL)
    	{
    		iPos = pPos - pchSplitData;
      	memset(chItemValue, 0, sizeof(chItemValue));
      	strncpy(chItemValue, pchSplitData, iPos);
				#ifdef _debug_       
				#endif  
      	pchSplitData = pPos + 1;
      	memset(chI, 0,sizeof(chI));
      	memset(chJ, 0,sizeof(chJ));
      
      	pPos = NULL;
      	iPos = 0;
      	pPos = strchr(chItemValue, ',');
      	iPos = pPos-chItemValue;
      	strncpy(chI,chItemValue,iPos);
      	strcpy(chJ,pPos+1);
				#ifdef _debug_      
				      		/*printf("chI=[%s]\n",chI);
				      		printf("chJ=[%s]\n",chJ);*/
				#endif
				if (0==strncmp(chJ,"|",1))
				{
					strcpy(pTuxedoCall->chInputValue[atoi(chI)],&chJ[1]);      
				}
				else
				{
					strcpy(pTuxedoCall->chInputValue[atoi(chI)],pPara->acParaName[atoi(chJ)]);      
				}	
			
    	} 
    	else
    	{
    		break;
    	}
                        
		}
	return iReturn;
}

/*
 *功能:调用tuxedo服务
 *作者:王良 
 *日期:2008年4月18日
 *输入:
 *输出:
 */
void doProcess(struct str_PARA *pStrucPara, int iQueryType, char* pchBuff)
{
	char	chWsnAddr[128];		/*tuxedo wsl 地址及端口配制信息*/
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[4096];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0;
	char   chWorkName[50+1];
	int    flag = 0;
	
  	memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  	memset(chPhoneNo, 0, sizeof(chPhoneNo));  
  	memset(chWorkName, 0, sizeof(chWorkName));
  	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  	strncpy(chWorkName,pStrucPara->acParaName[3],50);
 	memset(chWsnAddr, 0, sizeof(chWsnAddr)); 
 	
 	if(getWorkWsnAddr(chWorkName,chWsnAddr)!=0)
 	{
 		memset(chWsnAddr, 0, sizeof(chWsnAddr));
 		flag = getPhoneWsnAddr(chPhoneNo,chWsnAddr);
 		if(flag<0)
 		{
 			sprintf(pchBuff,"%s","0\t0\t-100\n");
 			return ;
 		}
 	}
  /*sprintf(chWsnAddr,"WSNADDR=//10.161.6.76:16111");*/
	#ifdef _debug_
	  	printf("getPhoneWsnAddr chWsnAddr=[%s]\n",chWsnAddr);
	#endif
	putenv(chWsnAddr);  	 	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);	
	memset(&m_tuxedoreturn, 0, sizeof(m_tuxedoreturn));
	callTuxedoServer(&m_tuxedocall,&m_tuxedoreturn);
			
	#ifdef _debug_			
		printf("m_tuxedoreturn return_code=[%s]\n",m_tuxedoreturn.return_code);
		printf("m_tuxedoreturn return_msg=[%s]\n",m_tuxedoreturn.return_msg);
		printf("m_tuxedoreturn iArraySum=[%d]\n",m_tuxedoreturn.iArraySum);
		printf("m_tuxedoreturn iOutputNum=[%d]\n",m_tuxedoreturn.iOutputNum);
		
		for(i=0;i<m_tuxedoreturn.iArraySum;i++)
		{
			for(j=0;j<m_tuxedoreturn.iOutputNum;j++){
				printf("m_tuxedoreturn[%d][%d]=[%s]\n",i,j,m_tuxedoreturn.chOutputValue[i][j]);
			}
		}	
	#endif	

	memset(chBuff, 0, sizeof(chBuff));
	switch(iQueryType)
	{
		case 1046:/*OTA套餐查询*/
			sprintf(loginfo,"case :1046.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t3\t1\t%s\t%s\t%s\n",m_tuxedoreturn.chOutputValue[0][2],m_tuxedoreturn.chOutputValue[0][3],m_tuxedoreturn.chOutputValue[0][4]);
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"203001"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}	
			break;
		case 1001:/*密码验证*/
			sprintf(loginfo,"case :1001.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186100"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186104"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}	
			
			break;
		case 1002:
			sprintf(loginfo,"case :1002.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t2\t1\t%s\t%s\n",m_tuxedoreturn.chOutputValue[0][1],m_tuxedoreturn.chOutputValue[0][2]);
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1003:/*账单查询(当月账单和历史账单)*/
			sprintf(loginfo,"case :1003.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				memset(chBuff, 0, sizeof(chBuff));
				sprintf(chBuff,"%d\t58\t1",m_tuxedoreturn.iArraySum);
				for(i=0;i<m_tuxedoreturn.iArraySum;i++)
				{					
					sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[i][1]);
					sprintf(chBuff,"%s\n",chBuff);
				}
			}	
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186100"))
			{
				sprintf(chBuff,"0\t0\t0\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1005:/*当前余额查询*/
			sprintf(loginfo,"case :1005.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t3\t1\t%s\t%s\t%s\n",m_tuxedoreturn.chOutputValue[0][1],m_tuxedoreturn.chOutputValue[0][2],m_tuxedoreturn.chOutputValue[0][3]);
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186002"))
			{
				sprintf(chBuff,"0\t0\t2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186003"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186004"))
			{
				sprintf(chBuff,"0\t0\t-4\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}			
			break;
		/*case 1006:
			sprintf(loginfo,"case :1006.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				memset(chBuff, 0, sizeof(chBuff));
				sprintf(chBuff,"%d\t%d\t1",m_tuxedoreturn.iArraySum,m_tuxedoreturn.iOutputNum-1);
				for(i=0;i<m_tuxedoreturn.iArraySum;i++)
				{
					for(j=1;j<m_tuxedoreturn.iOutputNum;j++)
					{
						sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[i][j]);
					}
					sprintf(chBuff,"%s\n",chBuff);
				}
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186100"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}				 
			break;*/

		case 1012:/*缴费历史查询*/
			sprintf(loginfo,"case :1012.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				memset(chBuff, 0, sizeof(chBuff));
				sprintf(chBuff,"%d\t%d\t1",m_tuxedoreturn.iArraySum,m_tuxedoreturn.iOutputNum-1);
				for(i=0;i<m_tuxedoreturn.iArraySum;i++)
				{
					for(j=1;j<m_tuxedoreturn.iOutputNum;j++)
					{
						sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[i][j]);
					}
					sprintf(chBuff,"%s\n",chBuff);
				}
			}	
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186000"))
			{
				sprintf(chBuff,"0\t0\t0\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186001"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1014:/*客户现有功能查询*/
			sprintf(loginfo,"case :1014.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t1\t1\t%s\n",m_tuxedoreturn.chOutputValue[0][1]);
			}
			else 
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}			
			break;


		case 1018:/*号码归属地查询*/
			sprintf(loginfo,"case :1018.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0] );
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t5\t1");
				for(j=1;j<m_tuxedoreturn.iOutputNum;j++)
				{
					sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[0][j]);
				}
				sprintf(chBuff,"%s\n",chBuff);
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			} 
			break;

		case 1020:/*GPRS功能业务申请*/
			sprintf(loginfo,"case :1020.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"-00001"))
			{
				sprintf(chBuff,"0\t0\t-4\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1021:/*GPRS功能业务取消*/
			sprintf(loginfo,"case :1021.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"-00001"))
			{
				sprintf(chBuff,"0\t0\t-4\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1022:/*电显示业务申请*/
			sprintf(loginfo,"case :1022.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1023:/*电显示业务取消*/
			sprintf(loginfo,"case :1023.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1025:/*移动秘书业务申请*/
			sprintf(loginfo,"case :1025.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1026:/*移动秘书业务取消*/
			sprintf(loginfo,"case :1026.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121995"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121900"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1029:/*手机报停*/
			sprintf(loginfo,"case :1029.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121395"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121303"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
			
		case 1030:/*手机复话*/
			sprintf(loginfo,"case :1030.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.return_code);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.return_code,"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121399"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121395"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.return_code,"121303"))
			{
				sprintf(chBuff,"0\t0\t-5\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1031:/*用户密码修改*/
			sprintf(loginfo,"case :1031.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1032:/*彩玲业务申请*/
			sprintf(loginfo,"case :1032.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"335402"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1033:/*彩玲业务取消*/
			sprintf(loginfo,"case :1033.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"335402"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1037:/*缴费冲正*/
			sprintf(loginfo,"case :1037.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t0\t1\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t0\t-1\n");
			}
			break;
						
		case 1035:/*缴费查询*/
			sprintf(loginfo,"case :1035.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t3\t1\t%s\t%s\t%s\n",m_tuxedoreturn.chOutputValue[0][2],m_tuxedoreturn.chOutputValue[0][3],m_tuxedoreturn.chOutputValue[0][4]);
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		
		case 1024:/*飞信业务受理*/
			sprintf(loginfo,"case :1024.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"122701"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"201101"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1027:/*手机报业务受理*/
			sprintf(loginfo,"case :1027.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192003"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192005"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"200012"))
			{
				sprintf(chBuff,"0\t0\t-14\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"200014"))
			{
				sprintf(chBuff,"0\t0\t-15\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"200016"))
			{
				sprintf(chBuff,"0\t0\t-16\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"200018"))
			{
				sprintf(chBuff,"0\t0\t-17\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"200020"))
			{
				sprintf(chBuff,"0\t0\t-18\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;

		case 1028:/*订购关系分类退定*/
			sprintf(loginfo,"case :1028.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192305"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192300"))
			{
				sprintf(chBuff,"0\t0\t0\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
			
		case 1036:
			sprintf(loginfo,"case :1036.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t1\t1\t%s\n",m_tuxedoreturn.chOutputValue[0][1]);
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"133617"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"130204"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			} 
			
			break;			

		case 1017:/*PUK码查询*/
			sprintf(loginfo,"case :1017.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				memset(chBuff,0,sizeof(chBuff));
				sprintf(chBuff,"1\t1\t1\t%s\n",m_tuxedoreturn.chOutputValue[0][1]);
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;			

		case 1013:/*GPRS流量查询*/
			sprintf(loginfo,"case :1013.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t10\t1");
				for(j=1;j<m_tuxedoreturn.iOutputNum;j++)
				{
					sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[0][j]);
				}
				sprintf(chBuff,"%s\n",chBuff);
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"811001"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;			

		case 1019:/*套餐申请与取消*/
			sprintf(loginfo,"case :1019.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"297605"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"297613"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;	
		case 1015:/**积分查询**/
			sprintf(loginfo,"case :1015.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"1\t7\t1");
				for(j=1;j<m_tuxedoreturn.iOutputNum;j++)
				{
					sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[0][j]);
				}
				sprintf(chBuff,"%s\n",chBuff);
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"186002"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;	
		case 1038:/**获取随机验证码**/
			sprintf(loginfo,"case :1038.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000005"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1039:/**验证随机验证码**/
			sprintf(loginfo,"case :1039.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000012"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1040:/**资费及优惠信息查询**/
			sprintf(loginfo,"case :1040.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				memset(chBuff, 0, sizeof(chBuff));
				sprintf(chBuff,"%d\t1\t1",m_tuxedoreturn.iArraySum);
				for(i=0;i<m_tuxedoreturn.iArraySum;i++)
				{					
					sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[i][2]);
					sprintf(chBuff,"%s\n",chBuff);
				}
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"981121"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"981107"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"981101"))
			{
				sprintf(chBuff,"0\t0\t-4\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1043:/**手机动画开通**/
			sprintf(loginfo,"case :1043.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    	LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192003"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192005"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1044:/**手机动画取消**/
			sprintf(loginfo,"case :1044.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192003"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192005"))
			{
				sprintf(chBuff,"0\t0\t-3\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;
		case 1045:/**订购关系单条退定**/
			sprintf(loginfo,"case :1045.Tuxedo ReturnCode=[%s]\n",m_tuxedoreturn.chOutputValue[0][0]);
    		LogIt(loginfo,HEAD);
			if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"000000"))
			{
				sprintf(chBuff,"0\t0\t1\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192300"))
			{
				sprintf(chBuff,"0\t0\t0\n");
			}
			else if(!strcmp(m_tuxedoreturn.chOutputValue[0][0],"192305"))
			{
				sprintf(chBuff,"0\t0\t-2\n");
			}
			else
			{
				sprintf(chBuff,"0\t0\t-1\n");
			}
			break;																				
		default:
			break;
	}
	#ifdef _debug_
		printf("chBuff[%s]\n",chBuff);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		printf("call callTuxedoServer end...\n");
	#endif
}

int callTuxedoServer(struct tagTuxedoCall *pTuxedoCall,struct tagTuxedoReturn *pTuxedoReturn){
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int			iInputNum = 0;
	char		chTmp[32];		/*临时变量*/
	int			i = 0;
	long 		len = 0;
	int  		ret = 0;
	char		chServerName[64];
	char 		buffer[400];
	int 		ret_line = 0; 
	char 		temp_buf[100];
	int 		j=0;	
	char		chReturnCode[6+1];		/*返回代码*/
	char		chReturnMsg[128+1];		/*返回代码描述信息*/
	char		chSegment[1024];				/*返回值片段*/
	int			iHasRecord = 1;				/*返回字段标示*/
	
	#ifdef _debug_	
		printf("pTuxedoCall-> chServName=[%s]\n",pTuxedoCall->chServName);
		printf("pTuxedoCall-> iInputNum=[%d]\n",pTuxedoCall->iInputNum);
		printf("pTuxedoCall-> iOutputNum=[%d]\n",pTuxedoCall->iOutputNum);
		printf("pTuxedoCall-> chInputValue0=[%s]\n",pTuxedoCall->chInputValue[0]);
		printf("pTuxedoCall-> chInputValue1=[%s]\n",pTuxedoCall->chInputValue[1]);
	#endif		
	iInputNum = pTuxedoCall->iInputNum;
	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"Tpinit failed!\n");
    LogIt(loginfo,HEAD);
		#ifdef _debug_    	
			printf("Tpinit failed!\n", __FILE__, __LINE__);
		#endif
    tpterm();
	  return -1;
	}
	

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"FBFR32 send_fb tpalloc failed\n");
    LogIt(loginfo,HEAD);
    #ifdef _debug_
			printf("FBFR32 send_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */
	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"FBFR32 receive_fb tpalloc failed\n");
    LogIt(loginfo,HEAD);
    #ifdef _debug_
			printf("FBFR32 receive_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }       
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
	#ifdef _debug_
	printf("chServerName=[%s]\n",chServerName);
	#endif
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
    LogIt(loginfo,HEAD);   	
		sprintf(buffer, "*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
	 	#ifdef _debug_
			printf(buffer,__FILE__,__LINE__);
    #endif	
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    #ifdef _debug_
    	printf(buffer,__FILE__,__LINE__);
    #endif
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);

	#ifdef _debug_
		printf("++ SVC_ERR_NO32 = [%s] \n", chReturnCode);
		printf("++ SVC_ERR_MSG32 = [%s] \n", chReturnMsg);
	#endif
	strcpy(pTuxedoReturn->return_code,chReturnCode); 	
	strcpy(pTuxedoReturn->return_msg,chReturnMsg);
	pTuxedoReturn->iOutputNum = pTuxedoCall->iOutputNum; 	
	for(i=0; 1 == iHasRecord ; i++)
	{
		iHasRecord = 0;
		for(j=0; j<pTuxedoCall->iOutputNum; j++) 
		{
			memset(chSegment, 0, sizeof(chSegment));			
			Fgets32(receive_fb, GPARM32_0+j , i, chSegment);
			strcpy(pTuxedoReturn->chOutputValue[i][j],chSegment);
			if (0 != strlen(Trim(chSegment)))
			{				
				iHasRecord = 1;
			}			
		}
	}   
	pTuxedoReturn->iArraySum = i-1;
	tpfree((char *)send_fb);
  tpfree((char *)receive_fb);
  tpterm();	
	return ret_line;	
}

/*
 *功能:读取配制文件中,参数
 *作者:王良
 *日期:2008年3月19日
 */
void GetProfile(char* pchSection, char* pchSegName, char* pchValue)
{
	char	chFileName[128];
	char	chBuff[1024];
	char	chSection[8];
	char	chSegName[60];
	int		iSection = 0;
	FILE 	*fp;

	memset(chFileName, 0, sizeof(chFileName));
	memset(chSegName, 0, sizeof(chSegName));
	memset(chSection, 0, sizeof(chSection));
	sprintf(chFileName,"%s",getenv("BOSSOTACFG"));
	strcpy(chSection,pchSection);
	strcpy(chSegName,pchSegName);
	
	fp = fopen(chFileName,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"Open bossinter.cfg error");
		LogIt(loginfo,HEAD);
		#ifdef _debug_
			printf("打开配制文件出错!");
		#endif
		return;
	}
	while(!feof(fp))
	{
		memset(chBuff, 0, sizeof(chBuff));
		freadln(chBuff,fp);

		#ifdef _debug_
			/*printf("freadln chBuff=[%s] chSection=[%s]\n",chBuff,chSection);*/ 
		#endif		
		if (0 == strncmp(chBuff,chSection,6) && iSection == 0)
		{
			iSection = 1;
			continue;
		}
		else if (0 != strncmp(chBuff,chSection,6) && iSection == 0)
		{
			continue;
		}

		#ifdef _debug_
			/*printf("chBuff=[%s] chSegName=[%s]\n",chBuff,chSegName);*/ 
		#endif	

		if (0 ==strncmp(chBuff,chSegName,strlen(chSegName)))
		{
			strcpy(pchValue, &chBuff[strlen(chSegName)+1]);
			#ifdef _debug_
				printf("pchValue=[%s]\n",pchValue);
			#endif
			break;		
		} 

		if(0==strncmp(chBuff,"[",1) && iSection==1)
		{
			break;
		}	
	}
	fclose(fp);
} 

void freadln(char *szBuf, FILE *fp)
{
	char szCh;
  if (fp!=NULL)
  {
  	while (!feof(fp))
  	{
    	szCh =  fgetc(fp);
      if (szCh == ' ')
      {
      	continue;
      }      
      if (szCh == 0x0D)
      {
      	continue;
      }          
      if (szCh == 0x0A)
      {
      	break;
      }   		
   		*szBuf++ = szCh;
    	}
  }
}

/*
 *功能:详单查询
 *作者:王良
 *日期:2008年3月20日
 *
 */

int doDetailQuery(struct str_PARA *pStrucPara, int iSocket, char* pchQueryType,char *outbuff)
{
	char	chPhoneNo[11+1];
	char	chBeginDate[20+1];
	char	chEndDate[20+1];
	char	chServerName[20+1];
	FBFR32  *send_fb;
	FBFR32  *receive_fb;
	TPINIT	*tpinitbuf;
	int		input_par_num = 10;
	int		cd = 0;
	long	revent = 0;
	long 	rcv_len =0;
	char	buffer[128];
	char	return_code[6+1];
	char	tmpbuf[40];	
	int  	counter=0;
	int		ret_lines = 0;
	int 	iRecordSum = 0;
	int 	output_par_num =0;
	int		i, j, k, flag = 0;
	int		TuxedoRcvAllLines=0;
	int		TuxedoOutFields=0;
	int 	err_counter = 0;
	char	chWsnAddr[128];		/*tuxedo wsl 地址及端口配制信息*/
	char	chRecord[1024*10000];
	/*char	chBuff[1024*31];*/
	char	chLoginNo[6+1];
	char	chQueryType[4+1];
	char   	chWorkName[50+1];
	int    	iiflag = 0;	
	strcpy(chQueryType,pchQueryType);
	
	memset(chWorkName, 0, sizeof(chWorkName));
	memset(chWsnAddr, 0, sizeof(chWsnAddr));
	
	#ifdef _debug_	
		printf("doDetailQuery begin...\n");
	#endif  
  memset(chLoginNo, 0, sizeof(chLoginNo));
  strncpy(chLoginNo,pStrucPara->acParaName[0],6);
  
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  memset(chWsnAddr, 0, sizeof(chWsnAddr)); 
  if(getWorkWsnAddr(chWorkName,chWsnAddr)!=0)
 	{
 		memset(chWsnAddr, 0, sizeof(chWsnAddr));
 		iiflag = getPhoneWsnAddr(chPhoneNo,chWsnAddr);
 		if(iiflag<0)
 		{
 			sprintf(outbuff,"%s","0\t0\t-100\n");
 			return 0;
 		}
 	}
  /*sprintf(chWsnAddr,"WSNADDR=//10.161.6.76:16111");*/
	#ifdef _debug_
	  	printf("getPhoneWsnAddr chWsnAddr=[%s]\n",chWsnAddr);
	#endif
	putenv(chWsnAddr);
		
	memset(chPhoneNo, 0, sizeof(chPhoneNo));
	memset(chBeginDate, 0, sizeof(chBeginDate));
	memset(chEndDate, 0, sizeof(chEndDate));
	memset(chServerName, 0, sizeof(chServerName));	
	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
	strncpy(chBeginDate,pStrucPara->acParaName[5],20);
	strncpy(chEndDate,pStrucPara->acParaName[6],20);

	#ifdef _debug_
		printf("tpinit\n");
		printf("chBeginDate=[%s]\n",chBeginDate);
		printf("chEndDate=[%s]\n",chEndDate);	
	#endif	

	tpinitbuf=(TPINIT *)tpalloc("TPINIT", NULL, TPINITNEED(0));  
	tpinitbuf->flags = TPMULTICONTEXTS;   	
	if (tpinit((TPINIT *) tpinitbuf) == -1)
	{
		sprintf(loginfo,"DetailQuery tpinit error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	
	/*为输入变量分配FML缓冲区*/ 
	send_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(input_par_num*50));   
	if(send_fb==(FBFR32 *)NULL)
	{  	
		sprintf(loginfo,"DetailQuery sendbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();    
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;                                             
	}

	#ifdef _debug_
		printf("Fchg32 send_fb\n");
		printf("chLoginNo=[%s]\n",chLoginNo);
		printf("chQueryType=[%s]\n",chQueryType);
		printf("chPhoneNo=[%s]\n",chPhoneNo);
		printf("chBeginDate=[%s]\n",chBeginDate);
		printf("chEndDate=[%s]\n",chEndDate);
	#endif	

	Fchg32(send_fb, GPARM32_0+0, 0, chLoginNo, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+1, 0, chQueryType, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+2, 0, chPhoneNo, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+3, 0, chBeginDate, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+4, 0, chEndDate, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+5, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+6, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+7, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+8, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+9, 0, "0", (FLDLEN32)0);
          
	/*为输出变量分配FML缓冲区*/
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32, NULL, (FLDLEN32)4096);
	if(receive_fb==(FBFR32 *)NULL)
	{ 
		sprintf(loginfo,"DetailQuery recvbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);  		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
	  tpterm();
	  sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		printf("Fchg32 tpconnect\n");
	#endif

	strcpy(chServerName,"s1861UnitQry");
	if ( (cd =tpconnect(chServerName,(char *)NULL,0L,TPSENDONLY)) == -1 )
	{	
		sprintf(loginfo,"*** DetailQuery tpconnect(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
    LogIt(loginfo,HEAD);
		sprintf(buffer, "*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
		#ifdef _debug_
			printf(buffer,__FILE__,__LINE__);
		#endif	
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
    return -1;
	}		
	#ifdef _debug_	
		printf("cd=[%d]\n",cd);
		printf("tpsend\n");
	#endif
	
	if ( tpsend(cd, (char *)send_fb, 0L,TPRECVONLY, &revent ) == -1 )
	{	
		sprintf(loginfo,"DetailQuery tpsend error\n" );
    LogIt(loginfo,HEAD);	
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		printf("tprecv\n");
	#endif
	Finit32(receive_fb, Fsizeof32(receive_fb));
	if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
	{
		sprintf(loginfo,"DetailQuery tprecv error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
	{   		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	Fgets32(receive_fb, GPARM32_0, 0, buffer);

	#ifdef _debug_
		printf("buffer=[%s]\n",buffer);
	#endif
	sprintf(loginfo,"DetailQuery Tuxedo returncode =[%s]\n", buffer);
  LogIt(loginfo,HEAD);
	if(!strcmp(buffer,"186100"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t0\n");
		return 0;
	}
	else if(!strcmp(buffer,"186105"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-2\n");
		return 0;
	}
	else if(!strcmp(buffer,"000000"))
	{

		strncpy(return_code, buffer, sizeof(return_code)-1);
		if( 0 != atoi(buffer) )
		{
			tpfree((char *)tpinitbuf);
			tpfree((char *)send_fb);
			tpfree((char *)receive_fb);
			tpterm();
			sprintf(outbuff,"0\t0\t-1\n");
			return -13;
		}		
		#ifdef _debug_
			printf("while\n");
		#endif
	
		counter = 1;
		/*memset(chBuff, 0, sizeof(chBuff));*/
		memset(chRecord, 0, sizeof(chRecord));
		iRecordSum = 0;
				
		while( 1 )
		{
			#ifdef _debug_
					printf("while counter=[%d]\n",counter);
			#endif					
			memset(tmpbuf, 0, sizeof(tmpbuf));
			sprintf(tmpbuf, "%d", counter);		
			Fchg32(send_fb, GPARM32_0, 0, tmpbuf , (FLDLEN32)0);
	
			revent = 0;
			if ( tpsend(cd, (char *)send_fb, 0L, TPRECVONLY, &revent ) == -1 ) 
			{
				break;
			}					
			Finit32(receive_fb, Fsizeof32(receive_fb));
			if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
			{
				sprintf(loginfo,"DetailQuery tprecv error\n");
    		LogIt(loginfo,HEAD);
				tpfree((char *)tpinitbuf);
				tpfree((char *)send_fb);
				tpfree((char *)receive_fb);
				tpterm();
				sprintf(outbuff,"0\t0\t-1\n");
				return -1;			
			}
			
			if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
			{	
				break;
			}		
			memset(buffer, 0, sizeof(buffer));
	
			ret_lines = Foccur32(receive_fb, GPARM32_0);
			Fgets32(receive_fb, GPARM32_0, 0, buffer);
			output_par_num = atoi( buffer );
	
			
			#ifdef _debug_
					printf("while counter=[%d] ret_lines=[%d]\n",counter,ret_lines);
			#endif		
			counter++;			
			iRecordSum = iRecordSum+ret_lines;			
			flag = 0;
			for(i=0; i<ret_lines; i++)
			{			
				memset(buffer, 0, sizeof(buffer));
				if( Fgets32(receive_fb, GPARM32_0, i, buffer)>=0 )
				{				
					if ( atoi( buffer ) > 0 )
						TuxedoRcvAllLines++;
					else 
					{
						flag=1;
						break;
					}
				}
			
				for(k=0; k<output_par_num; k++)
				{
					TuxedoOutFields = output_par_num;
					memset(buffer, 0, sizeof(buffer));
					if ( Fgets32(receive_fb, GPARM32_1+k, i, buffer) < 0 )
					{
						err_counter++;					
						continue;
					}
					sprintf(chRecord,"%s%s\t",chRecord,buffer);
				}
				sprintf(chRecord,"%s\n",chRecord);
			} // for end		
			if (flag == 1)
				break;
		} // while( 1 ) end
		#ifdef _debug_
		  	printf("iRecordSum=[%d]\n",iRecordSum);
		  	printf("output_par_num=[%d]\n",output_par_num);
		#endif
		/*memset(outbuff, 0, 1024*31);*/
		sprintf(outbuff,"%d\t%d\t1\t%s",iRecordSum-1,TuxedoOutFields,chRecord);
		#ifdef _debug_
			printf("chBuff=[%s]\n",outbuff);
		#endif
		/*sendPackage(iSocket,chBuff,1,1);*/
	
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
	}
	else
	{
		sprintf(outbuff,"0\t0\t-1\n");
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
		return 0;
	}
	#ifdef _debug_
		printf("doDetailQuery end...\n");
	#endif
	return 0;
}
int doSPQuery(struct str_PARA *pStrucPara,char *outbuff)
{
	char	chPhoneNo[11+1];	
	char	SPName[20+1];
	char	chServerName[20+1];
	FBFR32  *send_fb;
	FBFR32  *receive_fb;
	TPINIT	*tpinitbuf;
	int		input_par_num = 10;
	int		cd = 0;
	long	revent = 0;
	long 	rcv_len =0;
	char	buffer[128];
	char	return_code[6+1];
	char	tmpbuf[40];	
	int  	counter=0;
	int		ret_lines = 0;
	int 	iRecordSum = 0;
	int 	output_par_num =0;
	int		i, j, k, flag = 0;
	int		TuxedoRcvAllLines=0;
	int		TuxedoOutFields=0;
	int 	err_counter = 0;
	char	chWsnAddr[128];		/*tuxedo wsl 地址及端口配制信息*/
	char	chRecord[1024*30];
	/*char	chBuff[1024*31];*/
	char	chLoginNo[6+1];
	char	chQueryType[4+1];	
	//strcpy(chQueryType,pchQueryType);
	char    chWorkName[50+1];
	int     kkflag = 0;
	
	
	memset(chWsnAddr, 0, sizeof(chWsnAddr));
	
	#ifdef _debug_	
		printf("doDetailQuery begin...\n");
	#endif	
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chLoginNo, 0, sizeof(chLoginNo));
  strncpy(chLoginNo,pStrucPara->acParaName[0],6); 
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  memset(chWsnAddr, 0, sizeof(chWsnAddr));  
  if(getWorkWsnAddr(chWorkName,chWsnAddr)!=0)
 	{
 		memset(chWsnAddr, 0, sizeof(chWsnAddr));
 		kkflag = getPhoneWsnAddr(chPhoneNo,chWsnAddr);
 		if(kkflag<0)
 		{
 			sprintf(outbuff,"%s","0\t0\t-100\n");
 			return ;
 		}
 	}
 	
  	/*sprintf(chWsnAddr,"WSNADDR=//10.161.6.76:16111");*/
	#ifdef _debug_
	  	printf("getPhoneWsnAddr chWsnAddr=[%s]\n",chWsnAddr);
	#endif 
	putenv(chWsnAddr);
		
	memset(chPhoneNo, 0, sizeof(chPhoneNo));
	memset(chServerName, 0, sizeof(chServerName));
	
	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
	strncpy(SPName,pStrucPara->acParaName[5],20);
	#ifdef _debug_
		printf("tpinit\n");
		printf("SPName=[%s]\n",SPName);
	#endif	

	tpinitbuf=(TPINIT *)tpalloc("TPINIT", NULL, TPINITNEED(0));  
	tpinitbuf->flags = TPMULTICONTEXTS;   	
	if (tpinit((TPINIT *) tpinitbuf) == -1)
	{
		sprintf(loginfo,"doSOQuery tpinit error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	
	/*为输入变量分配FML缓冲区*/ 
	send_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(input_par_num*50));   
	if(send_fb==(FBFR32 *)NULL)
	{  	
		sprintf(loginfo,"doSOQuery sendbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm(); 
		sprintf(outbuff,"0\t0\t-1\n");   
		return -1;                                             
	}

	Fchg32(send_fb, GPARM32_0+0, 0, chPhoneNo, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+1, 0, SPName, (FLDLEN32)0);
	/*为输出变量分配FML缓冲区*/
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32, NULL, (FLDLEN32)4096);
	if(receive_fb==(FBFR32 *)NULL)
	{   
		sprintf(loginfo,"doSOQuery recvbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
	  tpterm();
	  sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		printf("Fchg32 tpconnect\n");
	#endif

	strcpy(chServerName,"sOrderQryDlg");
	if ( (cd =tpconnect(chServerName,(char *)NULL,0L,TPSENDONLY)) == -1 )
	{
		sprintf(loginfo,"*** doSPQuery tpconnect(%s) Error *** \nTperrno:  %d\nMessage:\t%s\n",chServerName,tperrno,tpstrerror(tperrno) );
    LogIt(loginfo,HEAD);	
		sprintf(buffer, "*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s\n",chServerName,tperrno,tpstrerror(tperrno));
		#ifdef _debug_
			printf(buffer,__FILE__,__LINE__);
		#endif	
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
    return -1;
	}		
	#ifdef _debug_	
		printf("cd=[%d]\n",cd);
		printf("tpsend\n");
	#endif
	
	if ( tpsend(cd, (char *)send_fb, 0L,TPRECVONLY, &revent ) == -1 )
	{
		sprintf(loginfo,"doSOQuery tpsend error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		printf("tprecv\n");
	#endif
	Finit32(receive_fb, Fsizeof32(receive_fb));
	if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
	{
		sprintf(loginfo,"doSOQuery tprecv error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
	{   		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	Fgets32(receive_fb, GPARM32_0, 0, buffer);

	#ifdef _debug_
		printf("buffer=[%s]\n",buffer);
	#endif
	sprintf(loginfo,"doSOQuery Tuxedo returnvalue =[%s]\n",buffer );
  LogIt(loginfo,HEAD);
	if(!strcmp(buffer,"190000"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t0\n");
		return 0;
	}
	else if(!strcmp(buffer,"190002"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-2\n");
		return 0;
	}
	else if(!strcmp(buffer,"000000"))
	{
		strncpy(return_code, buffer, sizeof(return_code)-1);
		if( 0 != atoi(buffer) )
		{
			tpfree((char *)tpinitbuf);
			tpfree((char *)send_fb);
			tpfree((char *)receive_fb);
			tpterm();
			sprintf(outbuff,"0\t0\t-1\n");
			return -13;
		}	
	
		#ifdef _debug_
			printf("while\n");
		#endif
	
		counter = 1;
		/*memset(chBuff, 0, sizeof(chBuff));*/
		memset(chRecord, 0, sizeof(chRecord));
		iRecordSum = 0;
				
		while( 1 )
		{
			#ifdef _debug_
					printf("while counter=[%d]\n",counter);
			#endif					
			memset(tmpbuf, 0, sizeof(tmpbuf));
			sprintf(tmpbuf, "%d", counter);		
			Fchg32(send_fb, GPARM32_0, 0, tmpbuf , (FLDLEN32)0);	
			revent = 0;
			if ( tpsend(cd, (char *)send_fb, 0L, TPRECVONLY, &revent ) == -1 ) 
			{
					break;
			}		
				
			Finit32(receive_fb, Fsizeof32(receive_fb));
			if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
			{
				sprintf(loginfo,"doSOQuery tprecv error\n" );
    		LogIt(loginfo,HEAD);
				tpfree((char *)tpinitbuf);
				tpfree((char *)send_fb);
				tpfree((char *)receive_fb);
				tpterm();
				sprintf(outbuff,"0\t0\t-1\n");
				return -1;			
			}
				
			if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
			{	
				break;
			}		
			memset(buffer, 0, sizeof(buffer));	
			ret_lines = Foccur32(receive_fb, GPARM32_0);
			Fgets32(receive_fb, GPARM32_0, 0, buffer);
			output_par_num = atoi( buffer );			
			#ifdef _debug_
					printf("while counter=[%d] ret_lines=[%d]\n",counter,ret_lines);					
			#endif	
	  	
			counter++;		
			iRecordSum = iRecordSum+ret_lines;		
			flag = 0;
			for(i=0; i<ret_lines; i++)
			{			
				memset(buffer, 0, sizeof(buffer));
				if( Fgets32(receive_fb, GPARM32_0, i, buffer)>=0 )
				{				
					if ( atoi( buffer ) > 0 )
						TuxedoRcvAllLines++;
					else 
					{
							flag=1;
							break;
					}
				}
				
				for(k=0; k<output_par_num; k++)
				{
					TuxedoOutFields = output_par_num;
					memset(buffer, 0, sizeof(buffer));
					if ( Fgets32(receive_fb, GPARM32_1+k, i, buffer) < 0 )
					{
						err_counter++;					
						continue;
					}
					sprintf(chRecord,"%s%s\t",chRecord,buffer);
				}
				sprintf(chRecord,"%s\n",chRecord);
			} // for end		
			if (flag == 1)
				break;
		} // while( 1 ) end
		#ifdef _debug_
		  	printf("iRecordSum=[%d]\n",iRecordSum);  	
		  	printf("output_par_num=[%d]\n",output_par_num);
		#endif
		/*memset(outbuff, 0, 1024*31);*/
		sprintf(outbuff,"%d\t%d\t1\t%s",iRecordSum-1,8-1,chRecord);
		#ifdef _debug_
			printf("chBuff=[%s]\n",outbuff);
		#endif
		/*sendPackage(iSocket,chBuff,1,1);*/	
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
	}
	else 
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return 0;
	}
	#ifdef _debug_
		printf("doSFQuery end...\n");
	#endif
	return 0;
}
/*
 *功能:向socket句柄发送数据包
 *作者:王良
 *日期:2008年3月21日
 */

void sendPackage(int iSocket,char *pchSendBuff, int iPackageEnd, int iPackageNum){
	char	chBuff[1024*31];
	struct tagPacketStruct *pTagPackage;
	char	chDataBuff[1024*30];
	int		iPackgeLen = 0;
	
	char	chFileName[128];
	FILE *fp;
	
	memset(chDataBuff, 0, sizeof(chDataBuff));
	strcpy(chDataBuff,pchSendBuff);
	
	memset(chBuff, 0, sizeof(chBuff));
	pTagPackage = (struct  tagPacketStruct*)chBuff;
	
	iPackgeLen = sizeof(struct  tagPacketStruct) -1 + strlen(chDataBuff);
	printf("iPackgeLen = [%d]\n",iPackgeLen);
	strcpy(pTagPackage->Verson,"0100");
	sprintf(pTagPackage->PacketLen,"%05d",iPackgeLen);
	strcpy(pTagPackage->TransValidateCode,"00000000");
	strcpy(pTagPackage->ClientFlag,"zzzd");
	strcpy(pTagPackage->ServerFlag,"boss");
	strcpy(pTagPackage->TransCode,"102100000002");
	strcpy(pTagPackage->TransDirection,"1");
	strcpy(pTagPackage->TransSequence,"11111111111111111111111111111111");
	strcpy(pTagPackage->TransDate,"YYYYMMDDHHMMSS");
	strcpy(pTagPackage->StateCode,"1000");
	strcpy(pTagPackage->StateInfo,"00000000000000000000000000000000");
	sprintf(pTagPackage->MultiPacketFlag,"%d",iPackageEnd);
	sprintf(pTagPackage->PacketNumber,"%d",iPackageNum-1);
	sprintf(pTagPackage->Reserve,"0");
	strcpy(pTagPackage->PacketBody,chDataBuff);

	send(iSocket,chBuff,iPackgeLen,0);
	
	#ifdef _debug_
		printf("sendDataPackage chBuff=[%s]\n",chBuff);
	#endif
	
}

void getPhoneHead()
{
	char	chFileName[128];
	FILE 	*fp;
	char	chBuff[128];
	char    *ptemp;
	
	memset(chFileName, 0, sizeof(chFileName));
	memset(chBuff, 0, sizeof(chBuff));
	sprintf(chFileName,"%s",getenv("PHONEHEADCFG"));
	#ifdef _debug_	
	printf("chFileName=[%s]\n",chFileName);	
	#endif

	fp = fopen(chFileName,"r");
	if (NULL==fp)
	{
		printf("打开配制文件出错!");
		return;
	}
	
	while(fgets(chBuff,128,fp))
	{
		if (0==strncmp(chBuff,"#",1))
		{
			continue;
		}
		
		if(0 == strlen(chBuff))
		{
			continue;
		} 
		Trim(chBuff);
		if (!(0==strncmp(chBuff,"&&",2)))
		{
			strncpy(g_phonehead.phone_head[g_phonehead.count++],chBuff,strlen(chBuff));
		}
		else 
		{
			strncpy(g_phonehead.wsn_addr[g_phonehead.count-1],&chBuff[2],strlen(chBuff)-2);
		}
		memset(chBuff, 0, sizeof(chBuff));
	}
	fclose(fp);
}

int getPhoneAddr(char *pchPhoneNo, char* pchWsnAddr)
{
	char chPhoneHead[7+1];
	char chWsnAddr[64+1];
	int	iLow = 0;
	int iHigh = 0;
	int iMid = 0;
	int iResult = -1;
	
	
	memset(chPhoneHead, 0, sizeof(chPhoneHead));
	memset(chWsnAddr, 0, sizeof(chWsnAddr));
	
	strncpy(chPhoneHead,pchPhoneNo,7);
	iHigh = g_phonehead.count; 
	while(iLow<=iHigh)
	{
  		iMid=(iLow+iHigh)/2;
    	if (0 == strncmp(g_phonehead.phone_head[iMid],chPhoneHead,7))
    	{
    		iResult = iMid;
    		break;
    	}
    	if(strncmp(g_phonehead.phone_head[iMid],chPhoneHead,7)>0)
    	{
    		iHigh=iMid-1;
    	}
    	else
    	{
    		iLow=iMid+1; 
  		}
  	}
  
  	if (-1 != iResult )
  	{
  		strcpy(pchWsnAddr,g_phonehead.wsn_addr[iResult]);
  		return 0;
		}
	return -1;
}
int getPhoneWsnAddr(char *pchPhoneNo, char* pchWsnAddr)
{
	char phoneHead[7+1];
	char tmpphone[11+1];
	char address[10];
	char address1[15];
	char tempWsnAddr[50];
	int i;
	memset(address,0,sizeof(address));
	memset(address1,0,sizeof(address1));
	strcpy(tmpphone,pchPhoneNo);
	strncpy(phoneHead,tmpphone,7);
	if(getPhoneAddr(phoneHead,address)<0)
	{
		return -1;
	}
	sprintf(address1,"[%s]",address);
	GetProfile(address1,"WSNADDR",tempWsnAddr);
	sprintf(pchWsnAddr,"WSNADDR=%s",tempWsnAddr);
	return 0;
}

int GetPacketHead(char * recbuff,char *head)
{
	int i = 0;
	if(recbuff == NULL || head == NULL)
	{
		return -1;
      }
	for(i = 0; i< PACKETHEAD; i ++)
	{
	     head[i] = recbuff[i];
    }
    head[PACKETHEAD] = '\0';
	return 0;
}  

int GenarateSendbuffer(char * head,char * body,char * sendbuff)
{
     char TransDate[14+1];
     char sendbuffLen[10];
     char temp[11];
     int   nlen = 0;
     int   i =0;
     int   lentemp = 0;
     struct tm *ptm;
		 time_t now;
     if(head == NULL || body == NULL || sendbuff == NULL)
     {
     		return -1;
     }
     time(&now);
		 ptm = localtime(&now);
		 sprintf(TransDate, "%d%02d%02d%02d%02d%02d", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, \
		 ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	 
     nlen = strlen(head) + strlen(body);
     sprintf(temp,"%d",nlen);
     lentemp = strlen(temp);
     /*printf("temp=[%s]\n",temp);*/
     for(i = 0; i < 10-lentemp ; i++)
     {
           sendbuffLen[i] = '0';  	
     }
     for(i = 10-lentemp; i<10;i++)
     {
     	    sendbuffLen[i] = temp[i-10+lentemp];
     }
     for(i = 4;i<14;i++)
     {
          head[i] = sendbuffLen[i-4];
     }
	 
     head[42] = '1';
     for(i = 75 ; i < 89;i++)
     {
          head[i] = TransDate[i-75];
     }
     /*printf("head = [%s]\n",head);*/
     sprintf(sendbuff,"%s%s",head,body);
     
     return 0;
}

void dototalfee(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff)
{
	char	chWsnAddr[128];		/*tuxedo wsl 地址及端口配制信息*/
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[40960];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0;
	char    chWorkName[50+1];
	int     kkflag = 0;
	#ifdef _debug_	
		printf("call callTuxedoServer begin...\n");
	#endif	
  memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
 	memset(chWsnAddr, 0, sizeof(chWsnAddr)); 
  
  if(getWorkWsnAddr(chWorkName,chWsnAddr)!=0)
 	{
 		memset(chWsnAddr, 0, sizeof(chWsnAddr));
 		kkflag = getPhoneWsnAddr(chPhoneNo,chWsnAddr);
 		if(kkflag<0)
 		{
 			sprintf(pchBuff,"%s","0\t0\t-100\n");
 			return ;
 		}
 	}
  	
	#ifdef _debug_
  	printf("getPhoneWsnAddr chWsnAddr=[%s]\n",chWsnAddr);
	#endif
	putenv(chWsnAddr);
  	
  	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);
	memset(&m_tuxedoreturn, 0, sizeof(m_tuxedoreturn));
	memset(chBuff, 0, sizeof(chBuff));
	callTuxedoTotalfee(&m_tuxedocall,chBuff);
	#ifdef _debug_
		printf("chBuff[%s]\n",chBuff);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		printf("call callTuxedoServer end...\n");
	#endif
}

void doInvoicePrint(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff)
{
	char	chWsnAddr[128];		/*tuxedo wsl 地址及端口配制信息*/
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[40960];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0;
	char    chWorkName[50+1];
	int     kkflag = 0;
	#ifdef _debug_	
		printf("call callTuxedoServer begin...\n");
	#endif	
  memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  Trim(chWorkName);
  Trim(chPhoneNo);
  sprintf(loginfo,"chWorkName=[%s],chPhoneNo=[%s]\n",chWorkName,chPhoneNo);
  LogIt(loginfo,HEAD);
 	memset(chWsnAddr, 0, sizeof(chWsnAddr)); 
  
  if(getWorkWsnAddr(chWorkName,chWsnAddr)!=0)
 	{
 	sprintf(loginfo,"-----------------\n");
  LogIt(loginfo,HEAD);
 	memset(chWsnAddr, 0, sizeof(chWsnAddr));
 	kkflag = getPhoneWsnAddr(chPhoneNo,chWsnAddr);
 	if(kkflag<0)
 	{
 		sprintf(pchBuff,"%s","0\t0\t-100\n");
 		return ;
 	}
 	}
  /*getPhoneWsnAddr(chPhoneNo,chWsnAddr);*/
  /*sprintf(chWsnAddr,"WSNADDR=//10.161.6.76:16111");*/
  	
	#ifdef _debug_
  	printf("getPhoneWsnAddr chWsnAddr=[%s]\n",chWsnAddr);
	#endif
	putenv(chWsnAddr);
  	
  	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);
	
	memset(&m_tuxedoreturn, 0, sizeof(m_tuxedoreturn));
	/*callTuxedoServer(&m_tuxedocall,&m_tuxedoreturn);*/
	memset(chBuff, 0, sizeof(chBuff));
	callTuxedoInvoicePrint(&m_tuxedocall,iQueryType,chBuff);
	#ifdef _debug_
		printf("chBuff[%s]\n",chBuff);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		printf("call callTuxedoServer end...\n");
	#endif
}
int callTuxedoInvoicePrint(struct tagTuxedoCall *pTuxedoCall,int type,char* pTuxedoReturn)
{
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int		iInputNum = 0;
	char	chTmp[32];		/*临时变量*/
	char 	temp[40960];
	int		i = 0,m=0;
	long 	len = 0;
	int  	ret = 0;
	char	chServerName[64];
	char 	buffer[400];
	int 	ret_line = 0; 
	char 	temp_buf[100];
	int 	j=0,n;
	int     iParaNumber = 0;
	char    cParaNumber[128];
	char    retCode[20];
	
	char	chReturnCode[6+1];		/*返回代码*/
	char	chReturnMsg[128+1];		/*返回代码描述信息*/
	char	chSegment[1024];				/*返回值片段*/
	int		iHasRecord = 1;				/*返回字段标示*/
	
	memset(cParaNumber,0,sizeof(cParaNumber));
	memset(retCode,0,sizeof(retCode));	
	#ifdef _debug_	
	printf("pTuxedoCall-> chServName=[%s]\n",pTuxedoCall->chServName);
	printf("pTuxedoCall-> iInputNum=[%d]\n",pTuxedoCall->iInputNum);
	printf("pTuxedoCall-> iOutputNum=[%d]\n",pTuxedoCall->iOutputNum);
	printf("pTuxedoCall-> chInputValue0=[%s]\n",pTuxedoCall->chInputValue[0]);
	printf("pTuxedoCall-> chInputValue1=[%s]\n",pTuxedoCall->chInputValue[1]);
	#endif	
	
	iInputNum = pTuxedoCall->iInputNum;


	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"doInvoicePrint tpinit error\n" );
	  LogIt(loginfo,HEAD);
		printf("Tpinit failed!\n", __FILE__, __LINE__);
    tpterm();
	  return -1;
	}
	

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"doInvoicePrint sendbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
		#ifdef _debug_
			printf("FBFR32 send_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"doInvoicePrint recvbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
		#ifdef _debug_
			printf("FBFR32 receive_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }
    
   
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
	#ifdef _debug_
	printf("chServerName=[%s]\n",chServerName);
	#endif
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** doInvoicePrint tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
	  LogIt(loginfo,HEAD);
		sprintf(buffer, "*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
		#ifdef _debug_
			printf(buffer,__FILE__,__LINE__);
		#endif
		
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    #ifdef _debug_
    	printf(buffer,__FILE__,__LINE__);
    #endif
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);
  n = pTuxedoCall->iOutputNum; 

	#ifdef _debug_
		printf("++ SVC_ERR_NO32 = [%s] \n", chReturnCode);
		printf("++ SVC_ERR_MSG32 = [%s] \n", chReturnMsg);
	#endif	
	Fget32(receive_fb,GPARM32_0,0,retCode,NULL);
	#ifdef _debug_
		printf("retCode = [%s]\n",retCode);
	#endif
	sprintf(loginfo,"doInvoicePrint Tuxedo returnvalue=[%s]\n",retCode );
	LogIt(loginfo,HEAD);
	if(type==1034)
	{
		if(!strcmp(retCode,"186100"))
		{
			sprintf(pTuxedoReturn,"0\t0\t0\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		else if(!strcmp(retCode,"000000"))
		{
			Fget32(receive_fb,GPARM32_0+15,0,cParaNumber,NULL);
			iParaNumber = atoi(cParaNumber);
			#ifdef _debug_
				printf("iParaNumber = [%d]\n",iParaNumber);
			#endif
			sprintf(pTuxedoReturn,"%d\t16\t1",iParaNumber+1);
			for(i = 1 ; i < 15 ;i++)
			{
				Fget32(receive_fb,GPARM32_0+i,0,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
			}
		
			sprintf(pTuxedoReturn,"%s\t%d\t",pTuxedoReturn,iParaNumber);
		
			for(i=0; i<iParaNumber-1 ; i++)
			{
				Fget32(receive_fb,GPARM32_0+16,i,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
				Fget32(receive_fb,GPARM32_0+17,i,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
				sprintf(pTuxedoReturn,"%s\t",pTuxedoReturn);
			}	   
			Fget32(receive_fb,GPARM32_0+16,iParaNumber-1,chSegment,NULL);
			sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
			Fget32(receive_fb,GPARM32_0+17,iParaNumber-1,chSegment,NULL);
			sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
			sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);
			tpfree((char *)send_fb);
	  	tpfree((char *)receive_fb);
	  	tpterm();		
			return 0;
		}	
		else
		{
			sprintf(pTuxedoReturn,"0\t0\t-1\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
	}
	else if(type==1041||type==1042)
	{
		if(!strcmp(retCode,"190701")&&type==1041)
		{
			sprintf(pTuxedoReturn,"0\t0\t-2\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		if(!strcmp(retCode,"190702")&&type==1042)
		{
			sprintf(pTuxedoReturn,"0\t0\t-2\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		else if(!strcmp(retCode,"000000"))
		{
			for(i=0; 1 == iHasRecord ; i++)
			{
				iHasRecord = 0;
				for(j=6; j<n-1; j++) 
				{
					memset(chSegment, 0, sizeof(chSegment));			
					Fgets32(receive_fb, GPARM32_0+j , i, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{
						sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
						iHasRecord = 1;
					}			
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);
			}
			#ifdef _debug_
				printf("pTuxedoReturn = [%s]\n",pTuxedoReturn);
			#endif
			m=n-6-1;
			sprintf(temp,"%d\t%d\t1%s\n",i-1,m,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
			#ifdef _debug_
				printf("pTuxedoReturn = [%s]\n",pTuxedoReturn);
			#endif   
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();	
			return 0;	
		}	
		else
		{
			sprintf(pTuxedoReturn,"0\t0\t-1\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
	}

}

int callTuxedoTotalfee(struct tagTuxedoCall *pTuxedoCall,char* pTuxedoReturn)
{
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int		iInputNum = 0;
	char	chTmp[32];		/*临时变量*/
	int		i = 0,m=0;
	long 	len = 0;
	int  	ret = 0;
	char	chServerName[64];
	char 	buffer[400];
	char 	temp[40960];
	int 	ret_line = 0; 
	char 	temp_buf[100];
	int 	j=0,n=0;
	int     iParaNumber = 0;
	char    cParaNumber[128];
	char    retCode[20];
	
	char	chReturnCode[6+1];		/*返回代码*/
	char	chReturnMsg[128+1];		/*返回代码描述信息*/
	char	chSegment[1024];				/*返回值片段*/
	int		iHasRecord = 1;				/*返回字段标示*/
	
	memset(cParaNumber,0,sizeof(cParaNumber));
	memset(retCode,0,sizeof(retCode));	
	#ifdef _debug_	
		printf("pTuxedoCall-> chServName=[%s]\n",pTuxedoCall->chServName);
		printf("pTuxedoCall-> iInputNum=[%d]\n",pTuxedoCall->iInputNum);
		printf("pTuxedoCall-> iOutputNum=[%d]\n",pTuxedoCall->iOutputNum);
		printf("pTuxedoCall-> chInputValue0=[%s]\n",pTuxedoCall->chInputValue[0]);
		printf("pTuxedoCall-> chInputValue1=[%s]\n",pTuxedoCall->chInputValue[1]);
	#endif	
	
	iInputNum = pTuxedoCall->iInputNum;


	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"doInvoicePrint tpinit error\n" );
	    LogIt(loginfo,HEAD);
		printf("Tpinit failed!\n", __FILE__, __LINE__);
    	tpterm();
	  	return -1;
	}
	

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"doInvoicePrint sendbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
		#ifdef _debug_
			printf("FBFR32 send_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"doInvoicePrint recvbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
		#ifdef _debug_
			printf("FBFR32 receive_fb tpalloc failed in snd_rcv_parms32()",__FILE__,__LINE__);
		#endif
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }
    
   
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
	#ifdef _debug_
	printf("chServerName=[%s]\n",chServerName);
	#endif
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** doInvoicePrint tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
	  LogIt(loginfo,HEAD);
		sprintf(buffer, "*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
		#ifdef _debug_
			printf(buffer,__FILE__,__LINE__);
		#endif
		
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    #ifdef _debug_
    	printf(buffer,__FILE__,__LINE__);
    #endif
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);

	#ifdef _debug_
		printf("++ SVC_ERR_NO32 = [%s] \n", chReturnCode);
		printf("++ SVC_ERR_MSG32 = [%s] \n", chReturnMsg);
	#endif	
		Fget32(receive_fb,GPARM32_0,0,retCode,NULL);
	#ifdef _debug_
		printf("retCode = [%s]\n",retCode);
	#endif
	sprintf(loginfo,"dototalfee Tuxedo returnvalue=[%s]\n",retCode );
	LogIt(loginfo,HEAD);
	if(!strcmp(retCode,"186100"))
	{
		sprintf(pTuxedoReturn,"0\t0\t0\n");
		tpfree((char *)send_fb);
  	tpfree((char *)receive_fb);
  	tpterm();
		return 0;
	}
	else if(!strcmp(retCode,"000000"))
	{
		for(i=0; 1 == iHasRecord ; i++)
		{
			iHasRecord = 0;
			for(j=1; j<5; j++) 
			{
				memset(chSegment, 0, sizeof(chSegment));			
				Fgets32(receive_fb, GPARM32_0+j, i, chSegment);		
				if (0 != strlen(Trim(chSegment)))
				{			
					sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
					iHasRecord = 1;
				}
			}
			if(iHasRecord==0) break;
			if(i==0&&iHasRecord==1)
			{
				for(m=0;m<52;m++)
				{
					memset(chSegment, 0, sizeof(chSegment));
					Fgets32(receive_fb, GPARM32_5 , m, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{
						sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
						iHasRecord = 1;
					}
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);	
			}
			else if(i==1&&iHasRecord==1)
			{
				for(m=52;m<104;m++)
				{
					memset(chSegment, 0, sizeof(chSegment));
					Fgets32(receive_fb, GPARM32_5 , m, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{			
							sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
							iHasRecord = 1;
					}
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);	
			}		
		}
		n=i-1;
		if(n==0)
		{
			memset(chSegment, 0, sizeof(chSegment));
			sprintf(chSegment,"1\t56\t1");
			sprintf(temp,"%s%s",chSegment,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
		}
		else if(n==1)
		{
			memset(chSegment, 0, sizeof(chSegment));
			sprintf(chSegment,"2\t112\t1");
			sprintf(temp,"%s%s",chSegment,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
		}   	   	
		tpfree((char *)send_fb);
	  tpfree((char *)receive_fb);
	  tpterm();		
		return 0;
	}	
	else
	{
		sprintf(pTuxedoReturn,"0\t0\t-1\n");
		tpfree((char *)send_fb);
  	tpfree((char *)receive_fb);
  	tpterm();
		return 0;
	}

}

int BubbleSort(PHONEHEAD *phead)
{
	if(phead == NULL)
	{
		return -1;
	}
	int i = 0;
	int j = 0;
	char tempPhoneHead[10];
	char tempAddr[50];
	memset(tempPhoneHead,0,sizeof(tempPhoneHead));
	memset(tempAddr,0,sizeof(tempAddr));
	for(i = 0 ; i < phead->count-1; i ++)
	{
		for(j=0; j < phead->count-i-1;j++)
		{
			if(strncmp(phead->phone_head[j],phead->phone_head[j+1],7)>0)
			{
				sprintf(tempPhoneHead,"%s",phead->phone_head[j]);
				sprintf(tempAddr,"%s",phead->wsn_addr[j]);
				sprintf(phead->phone_head[j],"%s",phead->phone_head[j+1]);
				sprintf(phead->wsn_addr[j],"%s",phead->wsn_addr[j+1]);
				sprintf(phead->phone_head[j+1],"%s",tempPhoneHead);
				sprintf(phead->wsn_addr[j+1],"%s",tempAddr);
				memset(tempPhoneHead,0,sizeof(tempPhoneHead));
				memset(tempAddr,0,sizeof(tempAddr));
			}
		}
	}
	return 0;
}
void getWorkAddressHead()
{
	char	chFileName[128];
	FILE 	*fp;
	char	chBuff[128];
	char    *ptemp;
	
	memset(chFileName, 0, sizeof(chFileName));
	memset(chBuff, 0, sizeof(chBuff));
	sprintf(chFileName,"%s",getenv("WORKADDRESSCFG"));
	#ifdef _debug_
	printf("chFileName=[%s]\n",chFileName);	
	#endif

	fp = fopen(chFileName,"r");
	if (NULL==fp)
	{
		printf("打开配制文件出错!");
		return;
	}
	
	while(fgets(chBuff,128,fp))
	{
		if (0==strncmp(chBuff,"#",1))
		{
			continue;
		}
		
		if(0 == strlen(chBuff))
		{
			continue;
		} 
		Trim(chBuff);
		if (!(0==strncmp(chBuff,"&&",2)))
		{
			strncpy(g_WorkAddress.work[g_WorkAddress.count++],chBuff,strlen(chBuff));
		}
		else 
		{
			strncpy(g_WorkAddress.wsn_addr[g_WorkAddress.count-1],&chBuff[2],strlen(chBuff)-2);
		}
		memset(chBuff, 0, sizeof(chBuff));
	}
	fclose(fp);
}
int getWorkAddr(char *pchWork, char* pchAddr)
{
	int i = 0;
	if(pchWork == NULL || pchAddr == NULL)
	{
		return -2;
	}
	for(i = 0; i < g_WorkAddress.count;i++)
	{
		if(!strcmp(pchWork,g_WorkAddress.work[i]))
		{
			sprintf(pchAddr,"%s",g_WorkAddress.wsn_addr[i]);
			return 0;
		}	
	}
	return -1;
}
int getWorkWsnAddr(char *pchWork, char* pchWsnAddr)
{
	char pwork[50];
	char address[10];
	char address1[15];
	char tempWsnAddr[50];
	memset(address,0,sizeof(address));
	memset(address1,0,sizeof(address1));
	memset(tempWsnAddr,0,sizeof(tempWsnAddr));
	sprintf(pwork,"%s",pchWork);
	if(getWorkAddr(pwork,address)<0)
	{
		return -1;
	}
	sprintf(address1,"[%s]",address);
	GetProfile(address1,"WSNADDR",tempWsnAddr);
	sprintf(pchWsnAddr,"WSNADDR=%s",tempWsnAddr);
	return 0;
}

int LogIt( char *sBuff,char *head)                                                                                                                                              
{                                                                                                                                 
	FILE *logFile=NULL;                                                                                                           
	char fileName[512];                                                                                                           
	int Pid=0;                                                                                                                    
	struct timeval tpstart;                                                                                                       
                                                                                                                                  
	Pid=getpid();                                                                                                                 
	memset(fileName,0,sizeof(fileName));                                                                                          
                                                                                                                                  
	time_t nowTime;                                                                                                               
	struct tm LocalTime;                                                                                                          
                                                                                                                                  
	nowTime=time(NULL);                                                                                                           
	memcpy(&LocalTime,localtime(&nowTime),sizeof(struct tm));                                                                     
	sprintf(fileName,"%s%04d%02d%02d.log",head,LocalTime.tm_year+1900,LocalTime.tm_mon+1,LocalTime.tm_mday);    
                 
                                                                                                                                  
	logFile=fopen(fileName,"ab+");                                                                                                
	if(logFile==NULL)                                                                                                             
	{                                                                                                                             
	  return -1;                                                                                                                  
	}                                                                                                                          
	gettimeofday(&tpstart,NULL);                                                                                                  
	fprintf(logFile,"%02d.%02d.%02d.%06ld:%d:%s\n",LocalTime.tm_hour,LocalTime.tm_min,LocalTime.tm_sec,tpstart.tv_usec,Pid,sBuff);
	fclose(logFile);                                                                                                              
	return 0;                                                                                                                     
}
/*int LogIt( char *sBuff,char *head)                                                                                                                                              
{                                                                                                                                 
	FILE *logFile=NULL;                                                                                                           
	char fileName[512];                                                                                                           
	int Pid=0;                                                                                                                    
	struct

timeval tpstart;                                                                                                       
                                                                                                                                  
	Pid=getpid();                                                                                                                 
	memset(fileName,0,sizeof(fileName));                                                                                          
                                                                                                                                  
	time_t nowTime;                                             

                                                                 
	struct tm LocalTime;                                                                                                          
                                                                                                                                  
	nowTime=time(NULL);                                                                                                           
	memcpy(&LocalTime,localtime(&nowTime),sizeof(struct tm));                                                                     
printf("stat 1\n");
	sprintf(fileName,"%s%04d%02d%02d.log",head,LocalTime.tm_year+1900,LocalTime.tm_mon+1,LocalTime.tm_mday);    
	
printf("stat 2\n");                 
                                                                                                                                  
	logFile=fopen(fileName,"ab+");                                                                                                
printf("stat 3\n");
	if(logFile==NULL)                                                                                                             
	{                                                                                                                             
printf("stat 4\n");
	  return -1;                                                                                 
                                
	}                                                                                                                             
	gettimeofday(&tpstart,NULL);                                                                                                  
printf("stat 5\n");
	fprintf(logFile,"%02d.%02d.%02d.%06ld:%d:%s\n",LocalTime.tm_hour,LocalTime.tm_min,LocalTime.tm_sec,tpstart.tv_usec,Pid,sBuff);
printf("stat 6\n");
	fclose(logFile);                                                                                                              
printf("stat 7\n");
	return 0;                                                                                    

                                
}*/

int getConfpath(char *path)                        
{                                                           
	int ret;                                                
	char tmp[255];                                          
                                                            
	memset(tmp,0,sizeof(tmp));                              
	                                                
	sprintf(tmp,"%s",getenv("OTAAPPSER")); 
	ret=convertpath(tmp);                                   
	if(ret==-1)                                             
	{                                                       
		printf("环境变量ADAPTERDIR未设置\n");               
		return -1;                                          
	}                                                       
	if(ret==-2)                                             
	{                                                       
		printf("环境变量ADAPTERDIR设置不正确\n");           
		return -1;                                          
	}                                                       
                                                            
	memset(path,0,sizeof(path));                            
	strcpy(path,tmp);                                       
	return 0; 
} 

int convertpath(char *path)                                                                      
{                                                    
	char temp[255],tmp[255],out[255];                
	char *p1=NULL,*p2=NULL;                          
	int len;                                         
                                                     
	memset(temp,0,sizeof(temp));                     
	memset(tmp,0,sizeof(tmp));                       
	memset(out,0,sizeof(out));                       
	p1=strchr(path,'$');                             
	if(p1==NULL)                                     
	{                                                
	 return 0;                                       
	}                                                
	p2=strchr(path,'/');                             
	if(p2==NULL)                                     
	{                                                
	  sprintf(temp,"%s",p1+1);                       
	  memset(path,0,sizeof(path));                   
	  if(getenv(temp)==NULL)                         
      {                                              
        return -1;                                   
      }                                              
      else                                           
      {                                              
       sprintf(path,"%s",getenv(temp));              
       if(temp[0]==0)                                
       return -2;                                    
       else                                          
       return 0;                                     
      }                                              
    }                                                
    else                                             
    {                                                
    	strncpy(temp,p1+1,p2-p1-1);                  
	   if(getenv(temp)==NULL)                        
       {                                             
        return -1;                                   
       }                                             
       else                                          
      {                                              
       snprintf(out,sizeof(out)-1,"%s",getenv(temp));
       if(temp[0]==0)                                
       return -2;                                    
       len = strlen(out);                            
       if(out[len-1]=='/')                           
       out[len-1]='\0';                              
       snprintf(tmp,sizeof(tmp)-1,"%s",p2);          
       strcat(out,tmp);                              
       sprintf(path,"%s",out);                       
       return 0;                                     
      }                                              
    }  
}

int readcfg(char *cfgname,char *section,char *key,char *value)                                              
{                                                             
	FILE *cfgfile;                                            
	char buff[1024];                                          
	char section_ext[1024];                                   
	char key_value[1024];                                     
	int find=0;                                               
	char value_ext[1024];                                     
                                                              
	if((cfgfile=fopen(cfgname,"r"))==NULL)                    
	return -1;                                                                                                           
	/*查找section*/                                           
	sprintf(section_ext,"[%s]",section);                      
                                                              
	for(;fgets(buff,sizeof(buff),cfgfile);)                   
	{                                                         
		if(buff[0]=='#')                                      
			continue;                                         
		if(buff[0]!='[')                                      
			continue;                                         
		if(strstr(buff,section_ext)==NULL)                    
			continue;                                         
		else                                                  
		{                                                     
			find=1;                                           
			break;                                            
		}                                                     
	}                                                         
                                                              
	if(find==0)                                               
	{                                                         
		fclose(cfgfile);                                      
		return -2;                                            
	}                                                         
                                                              
	/*查找key=value*/                                         
	value_ext[0]=0;                                           
	find=0;                                                   
                                                              
	for(;fgets(buff,sizeof(buff),cfgfile);)                   
	{                                                         
		if(buff[0]=='#')                                      
			continue;                                         
		if(strstr(buff,key)==NULL)                            
			continue;                                         
		else                                                  
		{                                                     
			sprintf(key_value,"%s=%%s\n",key);                
			sscanf(buff,key_value,value_ext);                 
			find=1;                                           
			break;                                            
		}                                                     
	}                                                         
                                                              
	fclose(cfgfile);                                          
	if(find==0)                                               
		return -3;                                            
                                                              
	if(value_ext[0]==0)                                       
		return -4;                                            
	else                                                      
	{                                                         
		strcpy(value,value_ext);                              
		return 1;                                             
	}                                                         
                                                              
	return 0;                                                 
}

void ALLTrim(char *sBuf)                                                                                                                   
{                                                                               
	int i, iFirstChar, iEndPos, iFirstPos;                                      
                                                                                
        iEndPos = iFirstChar = iFirstPos = 0;                                   
                                                                                
        for (i = 0; sBuf[i] != '\0'; i++)                                       
        {                                                                       
                if (sBuf[i] == ' '||sBuf[i]=='\n'||sBuf[i]=='\r'||sBuf[i]=='\t')
                {                                                               
                        if (iFirstChar == 0)                                    
                                iFirstPos++;                                    
                }                                                               
                else                                                            
                {                                                               
                        iEndPos = i;                                            
                        iFirstChar = 1;                                         
                }                                                               
        }                                                                       
                                                                                
        for (i = iFirstPos; i <= iEndPos; i++)                                  
                sBuf[i-iFirstPos] = sBuf[i];                                    
                                                                                
        sBuf[i-iFirstPos]='\0';                                                                                                            
} 

int LockFile(char* filename)
{
    FILE *fd;
    char pidstr[10];
    pid_t pid;
    int mayLock = -1;

    fd = fopen(filename,"r");		/*打开一个已经存在的文件*/
    if(fd != NULL)      			/*标志文件存在*/
    {
        fscanf(fd,"%s",pidstr);		/*将FD指向的文件中的PID读到PIDSTR中*/
        fclose(fd);
        pid = atoi(pidstr);
        if (pid < 2)        		/*不是有效的用户进程ID*/
            mayLock = 1;
        else
        {
            if (kill(pid,0) < 0)    /*检测进程有效性*/
            {
                if (errno == ESRCH)	/*进程失效*/
                    mayLock = 1;
                else            	/*执行错误*/
                    mayLock = -1;
            }
            else                    /*进程处于活动中*/
                mayLock = 0;
        }
    }
    else
    {
        if (errno == ENOENT || errno == EBADF)      /*标志文件不存在，没有活动进程*/
            mayLock = 1;
        else
            mayLock = -1;
    }   /* fd != NULL */

    if (mayLock == 1)
    {
        fd = fopen(filename,"w");
        if (fd == NULL)
        {
            mayLock = -1;
        }
        fprintf(fd,"%d",getpid());
    		fclose(fd);
    }

    return mayLock;
}

int	InitServerSocket(int *socketfd,	char *ip, char *port)
{
	int	handleofsocket;
	int	reuseaddr =	1, keepalive = 1;

	/*//建立连接*/
	struct sockaddr_in addr;
	handleofsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (handleofsocket == -1)
	{
		*socketfd =	errno;
		return -1;
	}
	if (setsockopt(handleofsocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	if (setsockopt(handleofsocket, SOL_SOCKET, SO_REUSEPORT, &reuseaddr, sizeof(reuseaddr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	memset((char	*) &addr,0, sizeof(addr));
	addr.sin_family	= AF_INET;
	if(ip==NULL||strlen(ip)==0||strcmp(ip,"INADDR_ANY")==0)
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	else
	{
		addr.sin_addr.s_addr = inet_addr(ip);
	}

	addr.sin_port =	htons((unsigned	short)atoi(port));
	if ( bind(handleofsocket,(struct sockaddr *)(&addr),sizeof(addr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}

	if (listen(handleofsocket,BACKLOG) == -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	*socketfd =	handleofsocket;
	#ifdef _debug_
	printf("InitServerSocket success\n");
	#endif
	return 0;
}

int Checkheartbeat(char* recvbuff)
{
	if(recvbuff == NULL)
	{
		return -2;
	}
	char TransCode[13];
	
	memset(TransCode,0,sizeof(TransCode));
	
	strncpy(TransCode,&recvbuff[30],12);
	TransCode[12] = '\0';
	Trim(TransCode);
	if(!strcmp(TransCode,"keep"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
int GetStringNumber(char* string_buf, char** arrString)
{
	int 	i,ilen;
	char   	result_buf[MAX_LEGALIP_COUNT*20];
	char * 	string_flag;	
	char *tempString[MAX_LEGALIP_COUNT];
	
	i=0;
	
	memset(result_buf,'\0',strlen(string_buf));
	memcpy(result_buf,string_buf,strlen(string_buf));
	//printf("string_buf=[%s]\n",string_buf);
	
	/*根据 ‘|’ 依次循环取出string*/
	while((string_flag=strchr(string_buf,'|')) != NULL)
	{
		ilen = string_flag-string_buf;
		//printf("result_buf=[%s][%d]\n",result_buf,ilen);
		result_buf[ilen]='\0';
		//printf("result_buf=[%s][%d]\n",result_buf,ilen);
		
		strcpy(arrString[i],result_buf);		
		/*指针下移一位，去掉'|'*/
		string_flag=string_flag + 1;
		memcpy(result_buf,string_flag,strlen(string_flag));
		result_buf[strlen(string_flag)]='\0';
		memcpy(string_buf,result_buf,strlen(result_buf));	
		string_buf[strlen(result_buf)]='\0';
		//printf("arrString[%d]=[%s]\n",i,arrString[i]);
		i+=1;		
	}	
	strcpy(arrString[i],string_buf);
	i+=1;
	
	return i;
}

int RecvResultOnHP(int socketfd, char *resultstr,int length,int limit)
{
    char chRecvBuff[1024];
    int numbytes=0,i=0;

	int iRecvSucFlag = 0;
	int ret;
	fd_set readfds;

	if(length<=0)
	{
		return -1;
	}

  	memset(chRecvBuff,0,sizeof(chRecvBuff));
  	memset(resultstr,0,length);
	numbytes=recv(socketfd,chRecvBuff,length,0);
	#ifdef _debug_
		printf("numbytes=[%d]\n",numbytes);
	#endif

	if (numbytes == 0)
	{
		return 0;
		if (numbytes == 0)
		{
			return -5;
		}
		if (errno == EAGAIN )
		{
			return -9;
		}
		if (errno == EBADF || errno == ESRCH || errno == ENOTCONN || errno == ENOTSOCK || errno == EPIPE)
		{
			return -6;
		}
		if (errno == EINTR)
		{
			return -7;
		}
		else
		{
		#ifdef _debug_
			printf("Recv failed[%d][%s]!\n",errno,strerror(errno));
		#endif
			return -8;
		}
	}
	if (numbytes < 0)
	{
		return -1;
	}
    
    for(i=0;i<numbytes;i++)
	{
		
		if (chRecvBuff[i] == 0)
           ;
        else
        	iRecvSucFlag++;   	
	}
	
	if(iRecvSucFlag <= 0)
	{
		#ifdef _debug_
			printf("client send error\n");
		#endif
		return -9;
	}	
	if (numbytes >= length)
   {
		memcpy(resultstr,chRecvBuff,length-1);
		resultstr[length-1]='\0';
	}
	else
	{
		/*strcpy(resultstr,recvbuff);*/
		memcpy(resultstr,chRecvBuff,numbytes);
		resultstr[numbytes]='\0';
	}
  	return numbytes;
}

int getPara(const char* buffer,char*pFlag,STRPARA* pPara)
{
	char temp_buf[RECV_SIZE + 1];
	char *temp_flag = NULL;
	int  i = 0;
	char paraValue[1024];
	int  ifind = 0;
	char *find_flag = NULL;
	int  iret = 0;
	char *HCFlag = NULL;
	
	memset(paraValue,0,sizeof(paraValue));	
	memset(temp_buf,0,sizeof(temp_buf));	
	strcpy(temp_buf, buffer);
	
	temp_flag = temp_buf;	
	i = 0;
	do
	{
		memset(paraValue,0,sizeof(paraValue));
		find_flag = strstr(temp_flag, pFlag);
		ifind = find_flag-temp_flag;
		
		if(find_flag == NULL)
		{
			strncpy(paraValue,temp_flag,1023);
			iret = 1;
		}
		else
		{
			strcpy(paraValue,temp_flag);
			paraValue[ifind] = '\0';
		}
		strcpy(pPara->acParaName[i],paraValue);
		
		if(find_flag!=NULL)
			temp_flag = find_flag + strlen(pFlag);
		
		
		i++;
		
		if(iret) break;
	}while(1);
	pPara->nParaCount = i;
	
	HCFlag = strstr(pPara->acParaName[i-1],"\n");
	if(HCFlag != NULL)
	{
		HCFlag[0] = '\0';
	}
	return i;
}
int Create_share(key_t Key,int nSize)
{
	int ret        = 0;
	int tempSemId = 0;

	tempSemId = shmget(Key,nSize,IPC_CREAT|0666);
	if(tempSemId == -1)
	{
		#ifdef _debug_
			printf("CreateCfgshm error[%s][%d]!,",strerror(errno),errno);
		#endif
		return -1;
	}
	return tempSemId;
}
int creat_semaphore(key_t keyval,int numsems)
{
	int sid;
	if (!numsems)
		return(-1);
	if ((sid=semget(keyval,numsems,IPC_CREAT|0660))==-1)
	{
		return -1;
	}
	return(sid);
}
int init_semaphore(int sid,int semnum,int initval)
{
	int ret;
	union semun semopts;
	semopts.val=initval;
	ret=semctl(sid,semnum,SETVAL,semopts);
	return (ret);
}

int CreateSEM(key_t Key)
{
	int SID=0;
	SID=creat_semaphore(Key,1);
	if(SID<0)
	{
		sprintf(loginfo,"Create semaphore error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	if(init_semaphore(SID,0,1)<0)
	{
		sprintf(loginfo,"Init semaphore error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	return SID;
}

int IncreaseChildPid(int shm,int sem,pid_t pid)
{
	SHAREBUFFER* pbuffer = NULL;
	int i = 0;
	locksem(sem,0);
	pbuffer=(SHAREBUFFER *)shmat(shm,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		unlocksem(sem,0);
		return -1;
	}
	
	for(i = 0; i < MAXPROCNUMBER ; i++)
	{
		if(pbuffer->ProcPid[i] == -1)
		{
			pbuffer->ProcPid[i] = pid;
			break;
		}
	}
	(pbuffer->nProcCount)++;
	shmdt(pbuffer);
	unlocksem(sem,0);
	
	return 0;	
}
int locksem(int sid,int member)
{
	int ret;
	struct sembuf sem_lock;/*={member,-1,SEM_UNDO}	*/
	memset(&sem_lock,0,sizeof(struct sembuf));
	//if (member<0||member>MAXSEMNUM)
	if (member<0)
		return (-2);
	sem_lock.sem_num=member;
	sem_lock.sem_op=-1;
	sem_lock.sem_flg=SEM_UNDO;
	ret=semop(sid,&sem_lock,1);
	return (ret);
}
int unlocksem(int sid, int member)
{
	int ret;/*semval*/
	struct sembuf sem_unlock;/*={member,1,SEM_UNDO};*/
	memset(&sem_unlock,0,sizeof(struct sembuf));
	//if (member <0||member>MAXSEMNUM)
	if (member <0)
		return (-2);
	sem_unlock.sem_num=member;
	sem_unlock.sem_op=1;
	sem_unlock.sem_flg=SEM_UNDO;
	ret=semop(sid,&sem_unlock,1);
	return (ret);
}
void TimeOutAlarm(int sig)
{
	sprintf(loginfo,"--系统处理超时--");
	LogIt(loginfo,HEAD);
}

int CheckProcNumber(int semid, int shmid,int MaxProcNumber)
{
	SHAREBUFFER* pbuffer = NULL;
	pbuffer=(SHAREBUFFER *)shmat(shmid,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	locksem(semid,0);
	if(pbuffer->nProcCount>=MaxProcNumber)
	{
		unlocksem(semid,0);
		shmdt(pbuffer);
		return -1;
	}
	else
	{
		unlocksem(semid,0);
		shmdt(pbuffer);
		return 0;
	}
} 

void Killsonpid(int shm,int sem)
{
	int i=0;
	int ret1=1,ret2=1;
	
	pid_t *Child=NULL;
	SHAREBUFFER *Addr=NULL;

	Addr=(SHAREBUFFER *)shmat(shm,0,0);
	if(Addr==NULL)
	{
		sprintf(loginfo,"in Killsonpid,get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return ;
	}

	locksem(sem,0);
	for(i=0;i<MAXPROCNUMBER;i++)
	{
		if(Addr->ProcPid[i]>0)
		{
			kill(Addr->ProcPid[i],SIGTERM);
			sleep(1);
		}
	}
	unlocksem(sem,0);
	while(1)
	{
		ret1=1;
		ret2=1;
		locksem(sem,0);
		for(i=0;i<MAXPROCNUMBER;i++)
		{
			if(Addr->ProcPid[i]>0)
			{
			#ifdef _debug_
				printf("Addr->ProcPid[%d]=[%d]\n",i,Addr->ProcPid[i]);
			#endif
				ret2=kill(Addr->ProcPid[i],0);
				ret1=ret1*ret2;
			}
		}
		unlocksem(sem,0);
		if(ret1!=0)
		{
			break;
		}
		else
		{
			sleep(1);
			#ifdef _debug_
				printf("In killSonPid...\n");
			#endif
			continue;
		}
	}
	shmdt(Addr);
} 

int decreaseChildPid(int shm,int sem,pid_t pid)
{
	SHAREBUFFER* pbuffer = NULL;
	int i = 0;
	locksem(sem,0);
	pbuffer=(SHAREBUFFER *)shmat(shm,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		unlocksem(sem,0);
		return -1;
	}
	
	for(i = 0; i < MAXPROCNUMBER ; i++)
	{
		if(pbuffer->ProcPid[i] == pid)
		{
			pbuffer->ProcPid[i] = -1;
			break;
		}
	}
	(pbuffer->nProcCount)--;
	shmdt(pbuffer);
	unlocksem(sem,0);
	
	return 0;
} 
