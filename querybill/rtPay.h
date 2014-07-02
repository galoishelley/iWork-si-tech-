#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#include <sys/ipc.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#include <oraca.h>
#include <sqlca.h>  


/***宏定义***/
#define  CHECK			8  
#define  FORKCOND		1000
#define  VAL_LEN		64
#define  TRUE			0 
#define  FALSE			-1
#define  ERROR			-1
#define	 INFO			2
#define	 DEBUG			3
#define  MAXLINE        6024
#define  SQLLEN			1024
#define  STOP			1
#define  DBLEN			100 
#define  USERLEN        10
#define  PASSLEN        10
#define  AREALEN        20
#define  MAXFEENUM		500  /**一级明细的最大个数**/
#define  MAXPAYNUM      50   /**付费方式最大个数 **/
#define  TIMEOUT                10

#define  START_TIMES	3   /***子进程的重启次数***/
#define	 D_MAX_PROCNUM	50
#define  D_MAX_MMAPNUM	10

#define	 BLOCKSIZE		50
#define  BILLDAY		9999
#define  PAYED			'0'	
#define  OWED			'1'

#define  LOGINNO		"acct"
#define  OPCODE			"9009"
#define  PREBILLFILE		"billowe"
#define  DISCERR		1002
#define	 CNTNOT			8003
#define	 CNTFAL			8006



/***错误代码定义***/
#define  EFORK		-101

#define  EMAPADR	-601		/**查找MAP文件失败		**/
#define  EMAPUN		-602		/**断开映射失败			**/

#define  EOCNT		-3001	/**数据库连接有错		**/
#define  EONOT		-3002	/**表中无数据			**/
#define  EOOTH		-3005	/**数据库其它错误		**/
#define  EOBCP		-3003	/**入库失败				**/
#define  EOUPD		-3004	/**更新失败				**/

#define  EFOPN		-901	/**文件打开有错			**/
#define  EFRD		-940	/**文件读有错			**/
#define  EFWR		-935	/**文件写有错			**/
#define  EFNOT	 	-920	/***文件不存在			**/

#define  EINDEX     -40001	/**索引文件有错			**/
#define  EFLEN	 	-40002	/***帐单文件长度有错		**/
#define	 EPRCNUM	-40003	/**配置文件中参数有错	**/
#define  EMAPNUM	-40004	/**配置文件中参数有错	**/
#define  EMAPRD		-40005		/**读MAP文件失败		**/
#define  EMAPCL		-40006		/**清MAP文件失败		**/
#define  EBCPYM		-40007	/**参数文件中上次销帐年月有错**/


/***字段长度定义***/	
#define  DETAILCODE		3	
#define  PHONENO		16
#define  TOTALDATE		9
#define	 BILLTYPE		1
#define	 SMCODE			3
#define	 ATTRCODE		9
#define  BILLCODE		9
#define  BELONGCODE		8
#define  OPTIME_LEN			15
#define  ENCRYPTOWELEN  17
#define  PHONENO_LEN	15
#define  CONNO_LEN		22
#define  IDNO_LEN		22
#define  FEECODE_LEN	2
#define  PAYTYPE_LEN	1
#define	 MAXDOBNODE		10000
#define	 MAXSTATICNODE	10000
#define  MAXOWEBILLDET	20

#define  DBLEN          100
#define  USERLEN        10
#define  PASSLEN        10
#define  AREALEN        20


char LOGPATH[VAL_LEN];
char LOGFILE[VAL_LEN];
char ERRPATH[VAL_LEN];
char ERRFILE[VAL_LEN];
char ROLLPATH[VAL_LEN];
char ROLLFILE[VAL_LEN];
char BILLPATH[VAL_LEN];
char BILLFILE[VAL_LEN];
char PAYPATH[VAL_LEN];
char PAYFILE[VAL_LEN];
char CFGFILE[VAL_LEN];
char BILLDAY_PATH[VAL_LEN];
char BILLDAYFILE[VAL_LEN];
char BCPPATH[VAL_LEN];
char BCPFILE[VAL_LEN];
char INDEXPATH[VAL_LEN];
char INDEXFILE[VAL_LEN];

char USERDB[DBLEN];
char USERNAME[USERLEN+1];
char PASSWORD[PASSLEN+1];
char DBNAME[AREALEN+1];

char LOGINACCEPT[20+1];
char OP_TIME[15];

#define FSMCODE		3
#define FATTRCODE	9
#define FBELONG		8
#define FMODECODE	9
#define FDATETIME	15
#define FOPTIME		15
#define FTOTALDATE  9
#define FENCRYPTOWE	17

typedef struct custOweDet{
	char  billType[BILLTYPE+1];
	char  feeCode[FEECODE_LEN+1];
	char  detCode[DETAILCODE];
	double  should;
	double  favour;
	int   times;
	int   durations;
}OweConDetType;

/**帐单文件映射**/
typedef struct{
	long  idNo;
	long  conNo;			/*帐户id*/
	long  custId;			/*客户id*/
	long  conCustId;		/*帐户归属id*/
	char  phoneNo[PHONENO];
	char  smCode[FSMCODE];
	char  attrCode[FATTRCODE];
	char  belongCode[FBELONG];	
	char  modeCode[FMODECODE]; 		/*资费套餐模板代码*/

	/*char  idconBegin[DATETIME];
	char  idconEnd[DATETIME];		用户帐户关系结束时间*/

	char  billBegin[FOPTIME];		/*开始时间*/
	char  billEnd[FOPTIME];			/*结束时间*/
	char  billTime[FOPTIME];			/*帐务时间*/
	char  totalDate[FTOTALDATE];		/*帐务日期*/	
	int	 yearMonth;					/*出帐年月*/
	int	 billDay;					/*出帐批次*/

	char  billType[BILLTYPE+1];
	char  billFlag;
	char  adjFlag;
	char  encryptOwe[FENCRYPTOWE];
		
	double  shouldPay;
	double  favourFee;
	int		callTimes;
	int		durations;
	OweConDetType  oweBillDet[MAXOWEBILLDET];
	long	owenext;
	long	owedet;
}OweBillType;

typedef struct{
	char	bitmap[MAXDOBNODE];
	OweBillType  stillOB[MAXSTATICNODE];
	OweBillType  dynaOB[MAXDOBNODE];
}OweBillMmap;

#define  blocksize  sizeof(OweBillType)

#define  MAPSIZE	sizeof(OweBillMmap)


/****明细帐单结构***/
typedef struct{
	char   feeCode[FEECODE_LEN+1];		/*帐目代码*/
	char   detailCode[DETAILCODE];
	char   payed_status[2];
	double should;			/*应收*/
	double favour;			/*优惠*/
	double payed_prepay;
	double payed_later;
	int    times;			/*通话次数*/
	int    duration;		/*通话时长*/
	struct DCUSTOWEDET_TYPE *next;
}DCUSTOWEDET_TYPE;

/***综合帐单结构***/
typedef struct{
	long  idNo;		/*用户id*/
	long  custId;		/*客户id*/
	long  conNo;		/*帐户id*/	
	long  conCustId;		/*帐户归属客户id*/
	char	phoneNo[PHONENO];
	int	yearMonth;	/*帐户年月*/ 
	int	billDay;	/*出帐批次*/
	char	totalDate[TOTALDATE];	/*帐务日期*/
	char    billType[BILLTYPE+1];          /*帐单类型*/
	char	payed_status[2];
	char	smCode[SMCODE];
	char	attrCode[ATTRCODE];
	char	billCode[BILLCODE];	 
	char	belongCode[BELONGCODE];
	double shouldPay;                   /*总应收*/
	double favourFee;                   /*总优惠*/
	double prepay_fee;
	double payed_prepay;
	double payed_later;
	int    times;                  /*通话次数*/
	int    duration;                   /*通话时长*/
	char   billFlag;
	char   billBegin[OPTIME_LEN];	/*开始时间*/
	char   billEnd[OPTIME_LEN];		/*结束时间*/
	char	billTime[OPTIME_LEN];	/*帐务时间*/
	char   adjFlag;
	char   encrypt_owe[ENCRYPTOWELEN];  /*加密字段*/
	struct DCUSTOWEDET_TYPE *detnext;   /*明细帐单指针**/
	struct DCUSTOWE_TYPE  *owenext;		         /**综合帐单指针***/
}DCUSTOWE_TYPE;

typedef struct{
	char  zoneFlag; /*** 0静态区 1 动态区*/
	long  recordPos;   /**blockflag=0 静态区偏移量 blockflag=1 动态区偏移量**/
	char  opTime[OPTIME_LEN];
	OweBillType oweblock;
}ROLLBACK_TYPE;

#define  rollsize  sizeof(ROLLBACK_TYPE)

typedef struct {
	long idNo;
	long conNo;
	char phoneno[PHONENO];
	char billday[5];
}USERINFO_TYPE;

typedef struct {
	long idNo;
        long conNo;
        char phoneNo[PHONENO];
        double should;
        char dealTime[OPTIME_LEN];
        int  flag;
}AddFileType;

typedef struct 
{
		OweBillType  owe;
		long	pos;
		struct CLEANMAP_TYPE *next;
}CLEANMAP_TYPE;

#define  SUFFIXLEN 7
typedef struct 
{
	char serviceId[PHONENO];
	char suffix[SUFFIXLEN+1];
	long pos;
}INDEX_TYPE;
