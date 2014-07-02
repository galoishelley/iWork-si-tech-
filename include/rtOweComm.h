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
	char conno[32];						/**按手机查询时conno=0**/
	char begintime[OPTIME_LEN];		/**YYYYMMDDHHMISI如：20031103213045**/
	char endtime[OPTIME_LEN];
	char qrytype;					/**0   查询综合帐单，1查询综合明细帐单**/
};

struct DCUSTOWE_COMM{
	char  idNo[32];
	char  conNo[32];			/*帐户id*/
	char  custId[32];		/*客户id*/
	char  conCustId[32];		/*帐户归属id*/
	char  phoneNo[PHONENO];	
	char  smCode[FSMCODE];	
	char  attrCode[FATTRCODE];	
	char  belongCode[FBELONG];	
	char  billCode[FMODECODE]; 		/*资费套餐模板代码*/
/*
	char  idconBegin[FDATETIME];		
	char  idconEnd[FDATETIME];		用户帐户关系结束时间*/
	
	char  billBegin[FOPTIME];		/*开始时间*/	
	char  billEnd[FOPTIME];			/*结束时间*/	
	char  billTime[FOPTIME];			/*帐务时间*/	
	char  totalDate[FTOTALDATE];		/*帐务日期*/	
	char  yearMonth[12];					/*出帐年月*/
	char  billDay[12];					/*出帐批次*/
	char  billType[BILLTYPE+1];		/**帐单类型**/
	
	char  billFlag;
	char  adjFlag;
	char  encryptOwe[FENCRYPTOWE];	
		
	char  shouldPay[64];
	char  favourFee[64];
	char  callTimes[12];
	char  durations[12];
	struct DCUSTOWEDET_TYPE *detnext; /**明细的结构指针，综合查询时为空**/
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

