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

typedef struct tagBossApp /*add by miaoyl at 20110311*/
{
	int  	count;
	int  	case_code[100];
	char	server_name_english[100][20];
	int     input_num[100];
	int     output_num[100];
	char    param_trans[100][100];
}BOSSAPP;

BOSSAPP g_bossapp;

typedef struct tagWorkToCase /*add by miaoyl at 20110311*/
{
	int  	count;
	char	work[100][50];
	int		case_code[100];
	int		call_flag[100];
	int     para[100];
	char    para_value[60][128];
}WORKTOCASE;

WORKTOCASE g_worktocase;

typedef struct tagBossOrCrm /*add by miaoyl at 20110311*/
{
	int  	count;
	char	belong_code[100][5];
	char	wsn_addr[100][50];
}BOSSORCRM;

BOSSORCRM g_BossorCrm;

typedef struct tagBossErrCode /*add by miaoyl at 20110311*/
{
	int  	count;
	char  	boss_errcode[1000][100];
	char	trans_errcode[1000][100];
}BOSSERRCODE;

BOSSERRCODE g_Bosserrcode;

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
	char chOutputValue[60][60][2048];/*��������*/
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

/*add by miaoyl at 20110322*/
extern char *ALLTrim(char*);
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

/*extern void GetProfile(char* pchSection, char* pchSegName, char* pchValue);*//*modify by miaoyl at 20110322*/
extern int GetProfile(char* pchSection, char* pchSegName, char* pchValue,int iflag);
void freadln(char *szBuf, FILE *fp);

extern void sendPackage(int,char *, int, int iPackageNum);

/*int getPhoneWsnAddr(char *pchPhoneNo, char* pchWsnAddr);*//*modify by miaoyl at 20110322*/
extern int getPhoneWsnAddr(char *pchPhoneNo, int belongcode,char* pchWsnAddr);

int getPhoneAddr(char *pchPhoneNo, char* pchWsnAddr);
#endif

/*add by miaoyl at  20110322*/

extern char workaddresscfg[100+1];
extern char worktocasecfg[100+1];
extern char bossappcfg[100+1];
extern char errorcodecfg[100+1];
extern char phoneheadcfg[100+1];
extern char bossorcrmcfg[100+1];
extern char chWsnAddr[100+1];
