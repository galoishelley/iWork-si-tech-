/*************************************************************************\
 *	NOTE!!! 使用VI打开此文件时, 请将 tablespace设置为4 (:set ts=4)
\*************************************************************************/
/*调用tuxedo公共函数定义*/
#include <atmi.h>
#include <ctype.h>
#include <fml.h>
#include <fml32.h>
#include <Usysflds.h>
#include "general32.flds.h"

#define BUFF_SIZE  1024*2000
#define PACKETHEAD 145
#define MAX_LEGALIP_COUNT 60
#define RECV_SIZE  900
#define MAXPROCNUMBER 500

char loginfo[BUFF_SIZE];
char HEAD[2550];

typedef struct tagPhoneHead
{
	int  count;
	char	phone_head[4000][7+1];
	char	wsn_addr[4000][20];
}PHONEHEAD;

PHONEHEAD g_phonehead;
typedef struct tagWorkAddress
{
	int  count;
	char	work[100][50];
	char	wsn_addr[100][20];
}WORKADDRESS;

WORKADDRESS g_WorkAddress;

/*#define _debug_*/

typedef struct str_PARA
{
	char acParaName[30][512];		
	int  nParaCount;
}STRPARA;

typedef struct share_Buffer
{
	pid_t ProcPid[MAXPROCNUMBER];		
	int  nProcCount;
}SHAREBUFFER;
/*
 *功能:调用tuxedo服务传入参数结构体
 *作者:王良
 *日期:2008年3月18日
 */
struct tagTuxedoCall{
	char chServName[20];		/*调用输入名称*/
	int	 iInputNum;					/*输入参数个数*/
	int	 iOutputNum;        /*输出参数个数*/
	char chInputValue[60][128];/*输入结果集*/
};
/*
 *功能:调用tuxedo服务传出参数结构体
 *作者:王良
 *日期:2008年3月18日
 */
struct tagTuxedoReturn{
	char return_code[6+1];		/*调用输入名称*/
	char return_msg[128+1];		/*调用输入名称*/
	int	 iOutputNum;        	/*输出参数例个数*/
	int	 iArraySum;           /*输入记录数组数目*/
	char chOutputValue[60][60][1024];/*输出结果集*/
};

/*
 *功能:与接口传输包结构定义
 *作者:王良
 *日期:2008年3月21日
 */
 
struct tagPacketStruct
{
	char   Verson[4];
	char   PacketLen[5];
	char   TransValidateCode[8];
	char   ClientFlag[4];
	char   ServerFlag[4];
	char   TransCode[12];
	char   TransDirection[1];
	char   TransSequence[32];
	char   TransDate[14];
	char   StateCode[4];
	char   StateInfo[32];
	char   MultiPacketFlag[1];
	char   PacketNumber[3];
	char   Reserve[16];
	char   PacketBody[1];                     
};


#ifndef __PUB_H__
#define __PUB_H__



/*---------------------------系统基础函数--------------------------------*/
/* 测试时间格式是否合法 */
extern int ValidateDate(const char*,int);

/* 调试日志函数 */
extern int DBOSSLog(char*,const char*, ... );

/* 错误日志函数 */
extern int EBOSSLog(char*,const char*, ... );

/* 日志函数 */
extern int BOSSLog(char*,const char*, ... );

/* 只有日志函数可以调用此函数，其他函数请调用 GetSysDateTime  */
extern char *GetSysDate(void);

/* 获取系统日期，时间 */
extern char *GetSysDateTime(char*,int,int);

/* 去掉字符串右面的空格、TAB、回车 */
extern char *RTrim(char*);

/* 去掉字符串左面的空格、TAB、回车 */
extern char *LTrim(char*);

/* 去掉字符串左右两边的空格、TAB、回车 */
extern char *Trim(char*);

/* 将字符串转化成大写 */
extern char *ToUpper(char*);

/* 将字符串转化成小写 */
extern char *ToLower(char*);
/*---------------------------系统基础函数--------------------------------*/

/*-----------------------------排序算法----------------------------------*/
/* 对整形数组进行堆排序 */
extern int HeapSort(int*,int);

/* 对整形数组进行快速排序 */
extern int QuickSort(int*,int);
/*-----------------------------排序算法----------------------------------*/

/*----------------------------数据库相关---------------------------------*/
/*对ORACLE的登录串进行解析*/
extern int DBParseUPS(char*,int,char*,int,char*,int,const char*);
/*----------------------------数据库相关---------------------------------*/

/*-----------------------------应用相关----------------------------------*/
/* 读取配置文件中的参数 */
extern int ReadConfig(const char*,const char*,const char*,char*,int);

/* 判断文件是否存在，如果存在，证明还有进程存在，不能重复启动进程 */
extern int LockFile(char*);

/* 替换串中的环境变量为实际的内容 */
extern int ReplaceEnvString(char*,int);

/* 读取字符串缓冲中的各个配置项 */
extern int ReadItems(const char*,const char*, char*,int,int);
/*-----------------------------应用相关----------------------------------*/

extern void GetProfile(char* pchSection, char* pchSegName, char* pchValue);
void freadln(char *szBuf, FILE *fp);

extern void sendPackage(int,char *, int, int iPackageNum);
int getPhoneWsnAddr(char *pchPhoneNo, char* pchWsnAddr);
int getPhoneAddr(char *pchPhoneNo, char* pchWsnAddr);
#endif
