#ifndef _QUERYACCT_H_
#define _QUERYACCT_H_

#include <time.h>
#include <sys/types.h>
#include <sys/times.h>
#include <netinet/in.h>


#define NET_MODE_SERVER			1
#define NET_MODE_CLIENT			2
#define NET_MAX_LISTEN			30
#define NET_SELECT_TIMEOUT		1
#define NET_READ_TIMEOUT		3
#define NET_CONNECT_TIMEOUT		3


#define USERINFO_VERSION			"SI-BIMS v1.0.0"
#define USERINFO_DATATYPE_BASE			"BASE"
#define USERINFO_DATATYPE_ACCT			"ACCOUNT"
#define USERINFO_DATATYPE_RELATION		"RELATION"
#define USERINFO_DATATYPE_FAV			"FAVOUR"
#define USERINFO_DATATYPE_VPMN			"BVGROUP"
#define USERINFO_DATATYPE_IVGROUP		"IVGROUP"
#define USERINFO_DATATYPE_IVMEMBER		"IVMEMBER"
#define USERINFO_DATATYPE_CONNECT		"HeartBeat"
#define USERINFO_DATATYPE_SERVICE		"INGWUSER"

#define USERINFO_KEYTYPE_DEFAULT		3

#define USERINFO_BASE				1
#define USERINFO_ACCT				2
#define USERINFO_RELATION			3
#define USERINFO_FAV				4
#define USERINFO_VPMN				5
#define USERINFO_IVGROUP			6
#define USERINFO_IVMEMBER			7
#define USERINFO_SERVICE			8

#define CONNET_HEAD_LEN				96	/*�������Ӱ��ĳ���*/
#define CONNET_CMD_STOP				"STOP"	/*�ر���������*/
#define CONNET_CMD_START			"ALL"	/*������������*/
#define CONNET_CMD_LEN				192	/*�������������󳤶�*/

#define USERINFO_MAX_RECORD			64	/*��󷵻ؼ�¼��*/
#define USERINFO_FIELD_MAXLEN			32	/*����󳤶�*/
#define USERINFO_MIN_LEN			32	/*��С���ش�����*/
#define USERINFO_MAX_LEN			128	/*��󷵻ؼ�¼����*/
#define USERINFO_ANSPKGHEAD_LEN			50	/*Ӧ�����ͷ����*/

#define USERINFO_SPLITCHAR_FIELD		','	/*��ָ����*/
#define USERINFO_SPLITCHAR_RECORD		';'	/*��¼�ָ����*/

#define USERINFO_STATUS_NORMAL			"00"	/*��������״̬*/

#define USERINFO_OPTYPE_SELECT			'0'	/*��ѯ*/

#define USERINFO_FIELDNUM_BASE			10	/*base��¼������*/
#define USERINFO_FIELDNUM_ACCT			12	/*acct��¼������*/
#define USERINFO_FIELDNUM_RELATION		4	/*relation��¼������*/
#define USERINFO_FIELDNUM_FAV			4	/*fav��¼������*/
#define USERINFO_FIELDNUM_VPMN			5	/*vpmn��¼������*/
#define USERINFO_FIELDNUM_IVGROUP		8	/*������vpmn���ż�¼������*/
#define USERINFO_FIELDNUM_IVMEMBER		12	/*������vpmn���ų�Ա��¼������*/
#define USERINFO_FIELDNUM_SERVICE		6	/*service��¼������*/

#define MAX_POOL				20	/*���������*/
#define	MSISDN_LEN				11	/*�ֻ����볤��*/

#if defined(__cplusplus)
extern "C" {
#endif

/*����ͨѶ���ýṹ��*/
typedef struct
{
	int iMode;
	int iSocketFd;
	int iAcceptFd;
	int iCommPort;
	char chCommIp[32+1];
	struct sockaddr_in stSocketAddress;
	
	fd_set  stFdSet;
	struct  timeval stTimeVal;
	
	fd_set  stFdSet1;
	struct  timeval stTimeVal1;
}stNet_NetInfo;


/*46���ֽ�*/
typedef struct
{
	int  chIngwFee;			/*INGW���*/
	char chIngwFlag[2+1];		/*INGW���ı�ʶ*/     
	int  chBossFee;			/*BOSS���*/
	char chBossFlag[2+1];		/*BOSS���ı�ʶ*/
	int  chVoiceFee;		/*vc��ʱ���*/
	char chVoiceFlag[2+1];		/*vc���ı�ʶ*/
	int  chGprsFee;			/*gprs��ʱ���*/
	char chGprsFlag[2+1];		/*gprs���ı�ʶ*/
	int  chSsFee;			/*ss��ʱ���*/
	char chSsFlag[2+1];		/*ss���ı�ʶ*/
	int  chMlFee;			/*ml��ʱ���*/
	char chMlFlag[2+1];		/*ml���ı�ʶ*/
}UserInfo_Acct;

typedef struct
{
	int  iDataType;			/*��������*/
	char chVersion[16+1];		/*ͨѶ��ʶ*/
	char chDataType[10+1];		/*��������*/
	char chOptCode[1+1];		/*��������*/
	char chMsisdn[15+1];		/*�ֻ���*/
	char chMonth[6+1];		/*�ļ��·�*/
	char chSysTime[15];		/*ϵͳʱ��*/
	char chKeyType;			/*��ʶ�Ƿ��Ǽ���*/
}UserInfo_ReqHead;

/*38���ֽ�*/
typedef struct
{
	char chVersion[16+1];		/*ͨѶ��ʶ*/
	char chDataType[10+1];		/*��������*/
	char chReturnStatus[2+1];	/*Ӧ��״̬��*/
	int  iReturnNum;		/*�������¼��*/
	int  iSize;			/*�������ݵĳ���*/
	char field_sep;			/*��ָ��*/
	char record_sep;		/*��¼�ָ��*/
}UserInfo_AnsHead;

/*�������ӵ�*/
typedef struct 
{
	int  iConnFd;			/*socket fd*/
	int  iPort;			/*�˿ں�*/
	char strInstance[17];		/*ʵ����*/
	char strAddress[16];		/*ip��ַ*/
}ConnetPoint;

typedef struct 
{
	int iConnetCount;		/*���ӳصĴ�С*/
	ConnetPoint szConnPoint[MAX_POOL];	/*���ӳ�*/
}ConnetPool;

ConnetPool g_ConnPool;

int initConnetPool(ConnetPool *pConnetPool);

int closeConnetPool(ConnetPool *pConnetPool);

int getAcctInfo(const ConnetPoint *pConnetPoint, const char *strMsisdn, double *dFee);

#if defined(__cplusplus)
}
#endif

#endif
