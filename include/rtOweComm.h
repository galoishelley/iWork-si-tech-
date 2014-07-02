struct bill_info
{
	char	beg[4];
	char	ver[3];
	char	msgtype;
	char	msglen[4];
	char	nextlen[4];
	char	msgbody[1024];
};

#define HEAD_LEN	16
#define	MSGFLAG	"BILL"
#define	DBILL_NUM	'0'
#define	DBILL_OWE	'1'
#define DBILL_DET	'2'
#define	TRANS_BEG	'0'
#define	TRANS_CON	'1'
#define	TRANS_END	'2'
#define INFO_LEN	sizeof(struct bill_info)
#define  OPTIME_LEN                      15

struct QRY_COMM{
	char phoneno[PHONENO+1];
	char conno[32];						/**���ֻ���ѯʱconno=0**/
	char begintime[OPTIME_LEN];		/**YYYYMMDDHHMISI�磺20031103213045**/
	char endtime[OPTIME_LEN];
	char qrytype;					/**0   ��ѯ�ۺ��ʵ���1��ѯ�ۺ���ϸ�ʵ�**/
};

struct DCUSTOWE_COMM{
	char  idNo[32];
	char  conNo[32];			/*�ʻ�id*/
	char  custId[32];		/*�ͻ�id*/
	char  conCustId[32];		/*�ʻ�����id*/
	char  phoneNo[PHONENO];	
	char  smCode[FSMCODE];	
	char  attrCode[FATTRCODE];	
	char  belongCode[FBELONG];	
	char  billCode[FMODECODE]; 		/*�ʷ��ײ�ģ�����*/
/*
	char  idconBegin[FDATETIME];		
	char  idconEnd[FDATETIME];		�û��ʻ���ϵ����ʱ��*/
	
	char  billBegin[FOPTIME];		/*��ʼʱ��*/	
	char  billEnd[FOPTIME];			/*����ʱ��*/	
	char  billTime[FOPTIME];			/*����ʱ��*/	
	char  totalDate[FTOTALDATE];		/*��������*/	
	char  yearMonth[12];					/*��������*/
	char  billDay[12];					/*��������*/
	char  billType[BILLTYPE+1];		/**�ʵ�����**/
	
	char  billFlag;
	char  adjFlag;
	char  encryptOwe[FENCRYPTOWE];	
		
	char  shouldPay[64];
	char  favourFee[64];
	char  callTimes[12];
	char  durations[12];
	struct DCUSTOWEDET_TYPE *detnext; /**��ϸ�Ľṹָ�룬�ۺϲ�ѯʱΪ��**/
	struct DCUSTOWE_TYPE *owenext;	
};

struct DCUSTOWEDET_COMM{
    char  feeCode[FEECODE_LEN+1];
    char  detCode[DETAILCODE];
    char  should[64];
    char  favour[64];
    char  times[12];
    char  durations[12];
    struct DCUSTOWEDET_TYPE *next;
};

#define DCUSTOWE_LEN	sizeof(struct DCUSTOWE_COMM)
#define DCUSTOWEDET_LEN	sizeof(struct DCUSTOWEDET_COMM)
#define QRY_LEN			sizeof(struct QRY_COMM)

