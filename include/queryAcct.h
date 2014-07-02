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

#define CONNET_HEAD_LEN				96	/*网络连接包的长度*/
#define CONNET_CMD_STOP				"STOP"	/*关闭连接命令*/
#define CONNET_CMD_START			"ALL"	/*建立连接命令*/
#define CONNET_CMD_LEN				192	/*后续请求包的最大长度*/

#define USERINFO_MAX_RECORD			64	/*最大返回记录数*/
#define USERINFO_FIELD_MAXLEN			32	/*域最大长度*/
#define USERINFO_MIN_LEN			32	/*最小返回串长度*/
#define USERINFO_MAX_LEN			128	/*最大返回记录长度*/
#define USERINFO_ANSPKGHEAD_LEN			50	/*应答包包头长度*/

#define USERINFO_SPLITCHAR_FIELD		','	/*域分割符号*/
#define USERINFO_SPLITCHAR_RECORD		';'	/*记录分割符号*/

#define USERINFO_STATUS_NORMAL			"00"	/*正常返回状态*/

#define USERINFO_OPTYPE_SELECT			'0'	/*查询*/

#define USERINFO_FIELDNUM_BASE			10	/*base记录域数量*/
#define USERINFO_FIELDNUM_ACCT			12	/*acct记录域数量*/
#define USERINFO_FIELDNUM_RELATION		4	/*relation记录域数量*/
#define USERINFO_FIELDNUM_FAV			4	/*fav记录域数量*/
#define USERINFO_FIELDNUM_VPMN			5	/*vpmn记录域数量*/
#define USERINFO_FIELDNUM_IVGROUP		8	/*智能网vpmn集团记录域数量*/
#define USERINFO_FIELDNUM_IVMEMBER		12	/*智能网vpmn集团成员记录域数量*/
#define USERINFO_FIELDNUM_SERVICE		6	/*service记录域数量*/

#define MAX_POOL				20	/*最大连接数*/
#define	MSISDN_LEN				11	/*手机号码长度*/

#if defined(__cplusplus)
extern "C" {
#endif

/*网络通讯共用结构体*/
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


/*46个字节*/
typedef struct
{
	int  chIngwFee;			/*INGW余额*/
	char chIngwFlag[2+1];		/*INGW更改标识*/     
	int  chBossFee;			/*BOSS余额*/
	char chBossFlag[2+1];		/*BOSS更改标识*/
	int  chVoiceFee;		/*vc临时余额*/
	char chVoiceFlag[2+1];		/*vc更改标识*/
	int  chGprsFee;			/*gprs临时余额*/
	char chGprsFlag[2+1];		/*gprs更改标识*/
	int  chSsFee;			/*ss临时余额*/
	char chSsFlag[2+1];		/*ss更改标识*/
	int  chMlFee;			/*ml临时余额*/
	char chMlFlag[2+1];		/*ml更改标识*/
}UserInfo_Acct;

typedef struct
{
	int  iDataType;			/*数据类型*/
	char chVersion[16+1];		/*通讯标识*/
	char chDataType[10+1];		/*数据类型*/
	char chOptCode[1+1];		/*操作类型*/
	char chMsisdn[15+1];		/*手机号*/
	char chMonth[6+1];		/*文件月份*/
	char chSysTime[15];		/*系统时间*/
	char chKeyType;			/*标识是否是集团*/
}UserInfo_ReqHead;

/*38个字节*/
typedef struct
{
	char chVersion[16+1];		/*通讯标识*/
	char chDataType[10+1];		/*数据类型*/
	char chReturnStatus[2+1];	/*应答状态码*/
	int  iReturnNum;		/*数据体记录数*/
	int  iSize;			/*后续数据的长度*/
	char field_sep;			/*域分割符*/
	char record_sep;		/*记录分割符*/
}UserInfo_AnsHead;

/*网络连接点*/
typedef struct 
{
	int  iConnFd;			/*socket fd*/
	int  iPort;			/*端口号*/
	char strInstance[17];		/*实例名*/
	char strAddress[16];		/*ip地址*/
}ConnetPoint;

typedef struct 
{
	int iConnetCount;		/*连接池的大小*/
	ConnetPoint szConnPoint[MAX_POOL];	/*连接池*/
}ConnetPool;

ConnetPool g_ConnPool;

int initConnetPool(ConnetPool *pConnetPool);

int closeConnetPool(ConnetPool *pConnetPool);

int getAcctInfo(const ConnetPoint *pConnetPoint, const char *strMsisdn, double *dFee);

#if defined(__cplusplus)
}
#endif

#endif
