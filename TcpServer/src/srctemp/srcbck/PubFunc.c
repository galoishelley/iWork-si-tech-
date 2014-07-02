/**********************************************************************\
 *
 * File: PubFunc.cpp
 *
 *	NOTE!!! ʹ��VI�򿪴��ļ�ʱ, �뽫 tablespace����Ϊ4 (:set ts=4)
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
 * Action:		У�����ڸ�ʽ�Ƿ�Ϸ�
 * 						��:		20070212-�Ϸ�
 *								20070230-���Ϸ�
 * Input:	const char	*s_date
 *			const int	in_format
 * 				1	-	20070707
 * 				2	-	2007/07/07 || 2007-07-07
 * Output:			��
 * Return:	int
 * 				 0 - ��ʽ�Ϸ�
 *				-1 - ��ʽ���Ϸ�
 *********************************************************************
 * �˷���ʹ�õ���ȫ�ֱ���  sgDaysOfMonth ���ļ�ͷ��
 *********************************************************************
 * ʹ�÷�����������:
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
 * Action:		��ȡϵͳ�����ں�ʱ��
 *
 * Input:		int option ��Ҫ��ȡʱ��ĸ�ʽ
 *					ȡֵ���£�
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
 *					��������� out_str��ΪNULLʱ�˲��������ʱ�䴮�ĳ���(��ʱ����>0)
 *					�� out_str ΪNULLʱ�˲�������(��ʱ����=0)
 *
 * Output:		char *out_str
 *					��� out_str==NULL ��ͨ������ֵ������ָ����ʽ��ʱ�䴮
 *					����ҲҪͨ�� out_str ������ʱ�䴮
 *
 * Return:		����ָ����ʽ��ʱ�䴮
 *	 				�������NULL��ʾ��������Ϸ�
 *
 *************************************************************************************
 * ʹ�÷�����������:
 *		1)
 *			char date_time[32];
 *			memset( date_time, 0x00, sizeof(date_time) );
 *			GetSysDateTime( date_time, sizeof(date_time), @ ); @ ��ȡֵ 1 �� 8
 *			printf( "[time=%s]\n", date_time );
 *		2)
 *			printf( "[time=%s]\n", GetSysDateTime(NULL,0,#) ); # ��ȡֵ 1 �� 8
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
 * Action:		ֻ����־��������ʹ�ô˺������õ�ϵͳ���ڣ�����Ӧ����ʹ�� GetSysDateTime
 *
 * Return:		���� "YYYYMMDD" ��ʽ��ʱ�䴮
 *	 				������� NULL ��ʾϵͳ����
 *
 *************************************************************************************
 * ʹ�÷�����������:
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
 * Action   : ������־��ӡ����������ǳ�����־������� EBOSSLog ��������־��ӡ��ʹ�� BOSSLog
 * Input    : �ļ�������ʽ��...
 * Output   :
 * Return   : 0 -- �ɹ���-1 -- �ļ��򿪴�-2 -- д�ļ���
 ******************************************************************************************************
 * ʹ�ô˺���֮ǰ��Ҫ��ʼ�� ����־��־ gLogFlag ���ļ�ͷ��
 *				ֻ�� gLogFlag == 1 �Ż��ӡ��־
 ******************************************************************************************************
 * 
 * ʹ�÷�����������:
 *		DBOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			���е���־�ļ��������Ǿ���·��
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

		/* Ϊ����д����ҽ��ļ����� */
		fd = fileno(fp);
		lockf(fd, F_LOCK, 0l);

		/* д���ļ� */
		va_start( ap, fmt );
		if( vfprintf(fp, fmt, ap) < 0)
		{
			printf("ERROR: Call fopen!\n");
			return ( -2 );
		}
		va_end( ap );
		/* ���� */
		lockf(fd, F_ULOCK, 0l);
		/* �ر��ļ� */
		fclose(fp);
	}

	return ( 0 );
}

/******************************************************************************************************\
 * Function : EBOSSLog
 * Action   : ������־������ô˺�����ӡ��־��������־��ӡ����ʹ�ô˺�������Ӧ���� DBOSSLog
 *						������־������ʹ�� BOSSLog
 * Input    : �ļ�������ʽ��...
 * Output   :
 * Return   : 0 -- �ɹ���-1 -- �ļ��򿪴�-2 -- д�ļ���
 ******************************************************************************************************
 * 
 * ʹ�÷�����������:
 *		EBOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			���е���־�ļ��������Ǿ���·��
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

	/* Ϊ����д����ҽ��ļ����� */
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);

	/* д���ļ� */
	va_start( ap, fmt );
	if( vfprintf(fp, fmt, ap) < 0)
	{
		printf("ERROR: Call fopen!\n");
		return ( -2 );
	}
	va_end( ap );
	/* ���� */
	lockf(fd, F_ULOCK, 0l);
	/* �ر��ļ� */
	fclose(fp);
	return ( 0 );
}

/******************************************************************************************************\
 * Function : BOSSLog
 * Action   : ��ӡ��־������������־��ӡ����ʹ�ô˺�������Ӧ���� DBOSSLog
 *						������־����ʹ�� EBOSSLog
 * Input    : �ļ�������ʽ��...
 * Output   :
 * Return   : 0 -- �ɹ���-1 -- �ļ��򿪴�-2 -- д�ļ���
 ******************************************************************************************************
 * 
 * ʹ�÷�����������:
 *		BOSSLog( "/inter/adapter/log/boss.log", "%d%s\n", 100, "Hello BOSS!" );
 *			���е���־�ļ��������Ǿ���·��
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

	/* Ϊ����д����ҽ��ļ����� */
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);

	/* д���ļ� */
	va_start( ap, fmt );
	if( vfprintf(fp, fmt, ap) < 0)
	{
		printf("ERROR: Call fopen!\n");
		return ( -2 );
	}
	va_end( ap );
	/* ���� */
	lockf(fd, F_ULOCK, 0l);
	/* �ر��ļ� */
	fclose(fp);
	return ( 0 );
}

/*********************************************************************\
 * Function: RTrim
 * Action:	ȥ���ַ�������Ŀո�TAB���س�
 *
 * Input:	char	*in_str
 * Output:			��
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
 * Action:	ȥ���ַ�������Ŀո�TAB���س�
 *
 * Input:	char	*in_str
 * Output:			��
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
 * Action:	ȥ���ַ����������ߵĿո�TAB���س�
 *
 * Input:	char	*in_str
 * Output:			��
 * Return:	char	*in_str
\*********************************************************************/
char *
Trim( char *in_str )
{
	return( LTrim(RTrim(in_str)) );
}

/***************************************************************************************************************\
 *	������2007-04-17 14:17������(Heap Sort)
 *
 *
 *	1.	����˼�룺
 *			��������һ����ѡ����������������У���R[1..N]������һ����ȫ��������˳��洢�ṹ
 *			������ȫ��������˫�׽��ͺ��ӽ��֮������ڹ�ϵ��ѡ����С��Ԫ�ء�
 *
 *
 *	2.	�ѵĶ���: N��Ԫ�ص�����K1,K2,K3,...,Kn.��Ϊ�ѣ����ҽ����������������ԣ�
 *			Ki��K2i
 *
 *			Ki ��K2i+1(1�� I�� [N/2])
 *
 *		��ʵ�����������������ʵ���ȫ��������
 *			������һ��Ҷ�ӽ��Ĺؼ��־����ڵ����亢�ӽ��Ĺؼ��֡�
 *			��������10,15,56,25,30,70����һ���ѡ����ֶ��и���㣨��Ϊ�Ѷ����Ĺؼ�����С�����ǰ�����ΪС���ѡ�
 *			��֮������ȫ����������һ��Ҷ�ӽ��Ĺؼ��־����ڵ����亢�ӵĹؼ��֣����֮Ϊ����ѡ�
 *
 *
 *	3.	������̣�
 *			��������������С���ѣ������ѣ���ѡȡ��ǰ�������йؼ���С������󣩵ļ�¼ʵ������ġ�
 *			���ǲ������ô����������
 *			ÿһ������Ļ��������ǣ�
 *				����ǰ����������Ϊһ������ѣ�ѡȡ�ؼ������ĶѶ���¼���������������е����һ����¼������
 *				���������ú�ֱ��ѡ�������෴������������ԭ��¼����β���γɲ�����ǰ����������¼����
 *
 *
 *	�ô��������Ļ���˼��
 *		�� �Ƚ���ʼ�ļ�R[1..n]����һ������ѣ��˶�Ϊ��ʼ��������
 *		�� �ٽ��ؼ������ļ�¼R[1](���Ѷ�)�������������һ����¼R[n]������
 *			�ɴ˵õ��µ�������R[1..n-1]��������R[n]��������R[1..n-1].keys��R[n].key
 *		�� ���ڽ������µĸ�R[1]����Υ�������ʣ���Ӧ����ǰ������R[1..n-1]����Ϊ�ѡ�
 *			Ȼ���ٴν�R[1..n-1]�йؼ������ļ�¼R[1]�͸���������һ����¼R[n-1]������
 *			�ɴ˵õ��µ�������R[1..n-2]��������R[n-1..n]��
 *			���������ϵR[1..n-2].keys��R[n-1..n].keys��ͬ��Ҫ��R[1..n-2]����Ϊ�ѡ�
 *			......
 *			ֱ��������ֻ��һ��Ԫ��Ϊֹ��
 *
 *		�� Heapify����˼�뷽��
 *			ÿ������ʼǰR[l..i]����R[1]Ϊ���Ķѣ���R[1]��R[i]������
 *			�µ�������R[1..i-1]��ֻ��R[1]��ֵ�����˱仯���ʳ�R[1]����Υ���������⣬
 *			�����κν��Ϊ�����������Ƕѡ���ˣ���������������R[low..high]ʱ��ֻ�������R[low]Ϊ���������ɡ�
 *		"ɸѡ��"������
 *			R[low]����������(������)�����Ƕѣ������������ĸ�R[2low]��R[2low+1]�ֱ��Ǹ��������йؼ������Ľ�㡣
 *			��R[low].key��С�����������ӽ��Ĺؼ��֣���R[low]δΥ�������ʣ���R[low]Ϊ���������Ƕѣ����������
 *			������뽫R[low]�������������ӽ���йؼ��ֽϴ��߽��н�����
 *			��R[low]��R[large](R[large].key=max(R[2low].key��R[2low+1].key))������
 *			�������ֿ���ʹ���R[large]Υ�������ʣ�ͬ�����ڸý[large]Ϊ���������е�����
 *			�˹���ֱ����ǰ�������Ľ������������ʣ����߸ý������Ҷ��Ϊֹ��
 *			�������̾����ɸ��һ�����ѽ�С�Ĺؼ������ɸ��ȥ�������ϴ�Ĺؼ������ѡ������
 *			��ˣ����˽��˷�����Ϊ"ɸѡ��"��
\***************************************************************************************************************/

/*************************************************************************************\
 * Function:	HeapAdjust
 * Action:		������������жѵ���
 *
 * Input:		int	*ele_array	-- Ϊ������Ľṹ�������ʼ��ַ
 *				int	s
 *				int	m
 * Output:		�����������ͬһ��
 * Return:		��
\*************************************************************************************/
static void
HeapAdjust( int *ele_array, int s, int m )
{
	/*********************************************************************************
		��֪ele_array[s..m]�г�ele_array[s]֮�������ѵĶ���,����������ele_array[s]
		ʹele_array[s..m]��Ϊһ���󶥶�
	**********************************************************************************/

	int j,rc;
	rc=ele_array[s];

	for(j=2*(s+1)-1;j<=m;j=(j+1)*2-1)		/* �عؼ��ֽϴ�Ľ������ɸѡ */
	{
		if(j<m&&ele_array[j]<ele_array[j+1])
			++j; 				/* jΪ�ؼ��ֽϴ�ļ�¼���±� */
		if(rc>=ele_array[j])
			break; 				/* rcӦ������λ��s�� */
		ele_array[s]=ele_array[j];
		s=j;
	}

	ele_array[s]=rc;			/* ���� */
}

/*******************************************************************************************\
 * Function:	HeapSort
 * Action:		������������ж�����
 *
 * Input:		int	*ele_array	-- Ϊ������Ľṹ�������ʼ��ַ
 *				int	ele_num		-- Ϊ������Ľṹ�����Ԫ�ظ���
 * Output:		�����������ͬһ��
 * Return:
 *			 0	-- �ɹ�
 *			-1	-- ����������Ϸ�
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

	for( i=ele_num/2-1; i>=0; --i )			/* ��ele_array[0..ele_num-1]���ɴ󶥶� */
		HeapAdjust( ele_array, i, ele_num-1 );

	for( i=ele_num-1; i>0; --i )
	{
		t = ele_array[0];					/* ���Ѷ���¼�͵�ǰδ������������ele_array[0..i] */
		ele_array[0] = ele_array[i];		/* �е����һ����¼�໥���� */
		ele_array[i] = t;
		HeapAdjust( ele_array, 0, i-1 );	/* ��ele_array[0..i-1]���µ���Ϊ�󶥶� */
	}

	return( 0 );
}

/*******************************************************************************************\
 * Function:	partition
 * Action:		������������п�������
 *
 * Input:		int	*ele_array	-- Ϊ������������������ʼ��ַ
 *				int	lower		-- Ϊ���������������Ŀ�ʼԪ���±�
 *				int	higher		-- Ϊ���������������Ľ���Ԫ�ظ���
 * Output:		�����������ͬһ��
 * Return:
 *			�ֽ�Ԫ���±�
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
 * Action:		������������п�������
 *
 * Input:		int	*ele_array	-- Ϊ������������������ʼ��ַ
 *				int	lower		-- Ϊ���������������Ŀ�ʼԪ���±�
 *				int	higher		-- Ϊ���������������Ľ���Ԫ�ظ���
 * Output:		�����������ͬһ��
 * Return:
 *			 0	-- �ɹ�
 *			-1	-- ����������Ϸ�
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
 * Action:		������������п�������
 *
 * Input:		int	*ele_array	-- Ϊ������������������ʼ��ַ
 *				int	ele_num		-- Ϊ����������������Ԫ�ظ���
 * Output:		�����������ͬһ��
 * Return:
 *			 0	-- �ɹ�
 *			-1	-- ����������Ϸ�
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
 * Action:		���ַ���ת���ɴ�д
 *
 * Input:		char *in_str
 * Output:		ͬ Input
 * Return:		ͬ Output
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
 * Action:		���ַ���ת����Сд
 *
 * Input:		char *in_str
 * Output:		ͬ Input
 * Return:		ͬ Output
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
 * Action:		�� ORACLE �����Ӵ����н��� ����ʽ��������� i_source_str
 *
 * Input:		char *i_source_str
 *					��ʽ 1: uname/passwd
 *					��ʽ 2: uname/passwd@localservice
 * Output:
 *				char *o_uname -- �û�����������ַ
 *				char *o_uname_len -- �û���������峤��
 *				char *o_passwd -- ������������ַ
 *				char *o_passwd_len -- ����������峤��
 *				char *o_sname -- ORACLE���ط������� ��������ַ
 *				char *o_sname_len -- ORACLE���ط������� ������峤��
 * Return:
 *			0	- �ɹ�
 *			-1	- �������Ϸ�
 *			-2	- ����ʽ���Ϸ�
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
 * Action:		��ȡ�����ļ��еĸ������ò���
 *
 * Input:		const char *i_cfgfile	-	���������ļ���
 *							���ļ�������������ھ���·���ļ���
 *							���ļ�����˵����
 *								���ļ���"�������"�����ظ�
 *								ͬһ��"�������"�²�������ͬ��"������"
 *				const char *i_section	-	�������
 *				const char *i_key		-	������
 * Output:
 *				char *o_value			-	����ֵ��������ַ
 *				char *o_value_len		-	����ֵ������峤��
 * Return:
 *			 0	- �ɹ�
 *			-1	- �������Ϸ�
 *			-2	- ���ļ�����
 *			-3	- �Ҳ���������� section
 *			-4	- �Ҳ������� key
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

	/* -- ���� section --*/
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
	
	/* -- ���� key --*/
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
 * Action:		��ȡ�ַ��������еĸ����ֶ�
 *
 * Input:		const char *i_str	-	�����ַ�������
 *				char *i_separator	-	���봮�и����ֶεķָ���
 * Output:
 *				char	**o_pstr	-	�����ά�����ַ
 *				int		o_num		-	Ӧ����Ҫ���ֶεĸ���
 *				int		o_size		-	ÿ���ֶ���󳤶�
 * Return:
 *			> 0 - ��ȡ��������
 *			  0 - û�л�ȡ���κ��ֶ�
 *			 -1	- �������Ϸ�
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
 *	Action:		�滻�ַ����еĻ�������Ϊʵ�ʵ�����
 *					ʹ�ô˺�����ǰ���ǣ�
 *						������ຬ��һ����������
 *			��:
 *				$ADAPTERDIR/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *				$(ADAPTERDIR)/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *				${ADAPTERDIR}/tmp/list.txt -> /inter/adapter/tmp/list.txt
 *
 *	Input:		char *filename  -- �����Ǵ��л����������ַ���
 *				int 			-- ���봮��Ϊ������ǣ��������󳤶�
 *	Output:		char *filename  -- ���������Ѿ����滻���ַ���
 *	Return:
 *			 0 - ת���ɹ�
 *			 1 - ���������������������ϵĻ�������
 *			-1 - ��������������
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
 *����:���������Ĳ�ѯ������ת����BOSS����tuxedo��������ʽ
 *����:���� 
 *����:2008��4��18��
 *����:
 *���:
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
	
	/*��ȡ����������ò���*/
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"INPUT_NUM",chValue);
	pTuxedoCall->iInputNum = atoi(chValue);

	#ifdef _debug_
		printf("INPUT_NUM=[%s]\n",chValue);
	#endif

	/*��ȡ����������ò���*/
	memset(chValue, 0, sizeof(chValue));
	GetProfile(chQueryType,"OUTPUT_NUM",chValue);
	pTuxedoCall->iOutputNum = atoi(chValue);
	
	#ifdef _debug_
		printf("OUTPUT_NUM=[%s]\n",chValue);
	#endif
	
	/*��ȡ�����������ò���*/
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
 *����:����tuxedo����
 *����:���� 
 *����:2008��4��18��
 *����:
 *���:
 */
void doProcess(struct str_PARA *pStrucPara, int iQueryType, char* pchBuff)
{
	char	chWsnAddr[128];		/*tuxedo wsl ��ַ���˿�������Ϣ*/
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
		case 1046:/*OTA�ײͲ�ѯ*/
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
		case 1001:/*������֤*/
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
		case 1003:/*�˵���ѯ(�����˵�����ʷ�˵�)*/
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

		case 1005:/*��ǰ����ѯ*/
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

		case 1012:/*�ɷ���ʷ��ѯ*/
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

		case 1014:/*�ͻ����й��ܲ�ѯ*/
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


		case 1018:/*��������ز�ѯ*/
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

		case 1020:/*GPRS����ҵ������*/
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

		case 1021:/*GPRS����ҵ��ȡ��*/
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
		case 1022:/*����ʾҵ������*/
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

		case 1023:/*����ʾҵ��ȡ��*/
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

		case 1025:/*�ƶ�����ҵ������*/
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

		case 1026:/*�ƶ�����ҵ��ȡ��*/
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

		case 1029:/*�ֻ���ͣ*/
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
			
		case 1030:/*�ֻ�����*/
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

		case 1031:/*�û������޸�*/
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
		case 1032:/*����ҵ������*/
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
		case 1033:/*����ҵ��ȡ��*/
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
		case 1037:/*�ɷѳ���*/
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
						
		case 1035:/*�ɷѲ�ѯ*/
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
		
		case 1024:/*����ҵ������*/
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

		case 1027:/*�ֻ���ҵ������*/
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

		case 1028:/*������ϵ�����˶�*/
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

		case 1017:/*PUK���ѯ*/
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

		case 1013:/*GPRS������ѯ*/
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

		case 1019:/*�ײ�������ȡ��*/
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
		case 1015:/**���ֲ�ѯ**/
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
		case 1038:/**��ȡ�����֤��**/
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
		case 1039:/**��֤�����֤��**/
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
		case 1040:/**�ʷѼ��Ż���Ϣ��ѯ**/
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
		case 1043:/**�ֻ�������ͨ**/
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
		case 1044:/**�ֻ�����ȡ��**/
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
		case 1045:/**������ϵ�����˶�**/
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
	char		chTmp[32];		/*��ʱ����*/
	int			i = 0;
	long 		len = 0;
	int  		ret = 0;
	char		chServerName[64];
	char 		buffer[400];
	int 		ret_line = 0; 
	char 		temp_buf[100];
	int 		j=0;	
	char		chReturnCode[6+1];		/*���ش���*/
	char		chReturnMsg[128+1];		/*���ش���������Ϣ*/
	char		chSegment[1024];				/*����ֵƬ��*/
	int			iHasRecord = 1;				/*�����ֶα�ʾ*/
	
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

	/*������������������� */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* ��������ľ���ֵ */
	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* Ϊ�����������FML������ */
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

	/*����TUXEDO����*/
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
 *����:��ȡ�����ļ���,����
 *����:����
 *����:2008��3��19��
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
			printf("�������ļ�����!");
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
 *����:�굥��ѯ
 *����:����
 *����:2008��3��20��
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
	char	chWsnAddr[128];		/*tuxedo wsl ��ַ���˿�������Ϣ*/
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
	
	/*Ϊ�����������FML������*/ 
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
          
	/*Ϊ�����������FML������*/
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
	char	chWsnAddr[128];		/*tuxedo wsl ��ַ���˿�������Ϣ*/
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
	
	/*Ϊ�����������FML������*/ 
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
	/*Ϊ�����������FML������*/
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
 *����:��socket����������ݰ�
 *����:����
 *����:2008��3��21��
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
		printf("�������ļ�����!");
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
	char	chWsnAddr[128];		/*tuxedo wsl ��ַ���˿�������Ϣ*/
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
	char	chWsnAddr[128];		/*tuxedo wsl ��ַ���˿�������Ϣ*/
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
	char	chTmp[32];		/*��ʱ����*/
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
	
	char	chReturnCode[6+1];		/*���ش���*/
	char	chReturnMsg[128+1];		/*���ش���������Ϣ*/
	char	chSegment[1024];				/*����ֵƬ��*/
	int		iHasRecord = 1;				/*�����ֶα�ʾ*/
	
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

	/*������������������� */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* ��������ľ���ֵ */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* Ϊ�����������FML������ */
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

	/*����TUXEDO����*/
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
	char	chTmp[32];		/*��ʱ����*/
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
	
	char	chReturnCode[6+1];		/*���ش���*/
	char	chReturnMsg[128+1];		/*���ش���������Ϣ*/
	char	chSegment[1024];				/*����ֵƬ��*/
	int		iHasRecord = 1;				/*�����ֶα�ʾ*/
	
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

	/*������������������� */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* ��������ľ���ֵ */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			printf("pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* Ϊ�����������FML������ */
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

	/*����TUXEDO����*/
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
		printf("�������ļ�����!");
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
		printf("��������ADAPTERDIRδ����\n");               
		return -1;                                          
	}                                                       
	if(ret==-2)                                             
	{                                                       
		printf("��������ADAPTERDIR���ò���ȷ\n");           
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
	/*����section*/                                           
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
                                                              
	/*����key=value*/                                         
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

    fd = fopen(filename,"r");		/*��һ���Ѿ����ڵ��ļ�*/
    if(fd != NULL)      			/*��־�ļ�����*/
    {
        fscanf(fd,"%s",pidstr);		/*��FDָ����ļ��е�PID����PIDSTR��*/
        fclose(fd);
        pid = atoi(pidstr);
        if (pid < 2)        		/*������Ч���û�����ID*/
            mayLock = 1;
        else
        {
            if (kill(pid,0) < 0)    /*��������Ч��*/
            {
                if (errno == ESRCH)	/*����ʧЧ*/
                    mayLock = 1;
                else            	/*ִ�д���*/
                    mayLock = -1;
            }
            else                    /*���̴��ڻ��*/
                mayLock = 0;
        }
    }
    else
    {
        if (errno == ENOENT || errno == EBADF)      /*��־�ļ������ڣ�û�л����*/
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

	/*//��������*/
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
	
	/*���� ��|�� ����ѭ��ȡ��string*/
	while((string_flag=strchr(string_buf,'|')) != NULL)
	{
		ilen = string_flag-string_buf;
		//printf("result_buf=[%s][%d]\n",result_buf,ilen);
		result_buf[ilen]='\0';
		//printf("result_buf=[%s][%d]\n",result_buf,ilen);
		
		strcpy(arrString[i],result_buf);		
		/*ָ������һλ��ȥ��'|'*/
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
	sprintf(loginfo,"--ϵͳ����ʱ--");
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
