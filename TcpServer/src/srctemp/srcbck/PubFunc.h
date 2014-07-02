/*************************************************************************\
 *	NOTE!!! ʹ��VI�򿪴��ļ�ʱ, �뽫 tablespace����Ϊ4 (:set ts=4)
\*************************************************************************/
/*����tuxedo������������*/
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
 *����:����tuxedo����������ṹ��
 *����:����
 *����:2008��3��18��
 */
struct tagTuxedoCall{
	char chServName[20];		/*������������*/
	int	 iInputNum;					/*�����������*/
	int	 iOutputNum;        /*�����������*/
	char chInputValue[60][128];/*��������*/
};
/*
 *����:����tuxedo���񴫳������ṹ��
 *����:����
 *����:2008��3��18��
 */
struct tagTuxedoReturn{
	char return_code[6+1];		/*������������*/
	char return_msg[128+1];		/*������������*/
	int	 iOutputNum;        	/*�������������*/
	int	 iArraySum;           /*�����¼������Ŀ*/
	char chOutputValue[60][60][1024];/*��������*/
};

/*
 *����:��ӿڴ�����ṹ����
 *����:����
 *����:2008��3��21��
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



/*---------------------------ϵͳ��������--------------------------------*/
/* ����ʱ���ʽ�Ƿ�Ϸ� */
extern int ValidateDate(const char*,int);

/* ������־���� */
extern int DBOSSLog(char*,const char*, ... );

/* ������־���� */
extern int EBOSSLog(char*,const char*, ... );

/* ��־���� */
extern int BOSSLog(char*,const char*, ... );

/* ֻ����־�������Ե��ô˺������������������ GetSysDateTime  */
extern char *GetSysDate(void);

/* ��ȡϵͳ���ڣ�ʱ�� */
extern char *GetSysDateTime(char*,int,int);

/* ȥ���ַ�������Ŀո�TAB���س� */
extern char *RTrim(char*);

/* ȥ���ַ�������Ŀո�TAB���س� */
extern char *LTrim(char*);

/* ȥ���ַ����������ߵĿո�TAB���س� */
extern char *Trim(char*);

/* ���ַ���ת���ɴ�д */
extern char *ToUpper(char*);

/* ���ַ���ת����Сд */
extern char *ToLower(char*);
/*---------------------------ϵͳ��������--------------------------------*/

/*-----------------------------�����㷨----------------------------------*/
/* ������������ж����� */
extern int HeapSort(int*,int);

/* ������������п������� */
extern int QuickSort(int*,int);
/*-----------------------------�����㷨----------------------------------*/

/*----------------------------���ݿ����---------------------------------*/
/*��ORACLE�ĵ�¼�����н���*/
extern int DBParseUPS(char*,int,char*,int,char*,int,const char*);
/*----------------------------���ݿ����---------------------------------*/

/*-----------------------------Ӧ�����----------------------------------*/
/* ��ȡ�����ļ��еĲ��� */
extern int ReadConfig(const char*,const char*,const char*,char*,int);

/* �ж��ļ��Ƿ���ڣ�������ڣ�֤�����н��̴��ڣ������ظ��������� */
extern int LockFile(char*);

/* �滻���еĻ�������Ϊʵ�ʵ����� */
extern int ReplaceEnvString(char*,int);

/* ��ȡ�ַ��������еĸ��������� */
extern int ReadItems(const char*,const char*, char*,int,int);
/*-----------------------------Ӧ�����----------------------------------*/

extern void GetProfile(char* pchSection, char* pchSegName, char* pchValue);
void freadln(char *szBuf, FILE *fp);

extern void sendPackage(int,char *, int, int iPackageNum);
int getPhoneWsnAddr(char *pchPhoneNo, char* pchWsnAddr);
int getPhoneAddr(char *pchPhoneNo, char* pchWsnAddr);
#endif
