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


/***�궨��***/
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
#define  MAXFEENUM		500  /**һ����ϸ��������**/
#define  MAXPAYNUM      50   /**���ѷ�ʽ������ **/
#define  TIMEOUT                10

#define  START_TIMES	3   /***�ӽ��̵���������***/
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



/***������붨��***/
#define  EFORK		-101

#define  EMAPADR	-601		/**����MAP�ļ�ʧ��		**/
#define  EMAPUN		-602		/**�Ͽ�ӳ��ʧ��			**/

#define  EOCNT		-3001	/**���ݿ������д�		**/
#define  EONOT		-3002	/**����������			**/
#define  EOOTH		-3005	/**���ݿ���������		**/
#define  EOBCP		-3003	/**���ʧ��				**/
#define  EOUPD		-3004	/**����ʧ��				**/

#define  EFOPN		-901	/**�ļ����д�			**/
#define  EFRD		-940	/**�ļ����д�			**/
#define  EFWR		-935	/**�ļ�д�д�			**/
#define  EFNOT	 	-920	/***�ļ�������			**/

#define  EINDEX     -40001	/**�����ļ��д�			**/
#define  EFLEN	 	-40002	/***�ʵ��ļ������д�		**/
#define	 EPRCNUM	-40003	/**�����ļ��в����д�	**/
#define  EMAPNUM	-40004	/**�����ļ��в����д�	**/
#define  EMAPRD		-40005		/**��MAP�ļ�ʧ��		**/
#define  EMAPCL		-40006		/**��MAP�ļ�ʧ��		**/
#define  EBCPYM		-40007	/**�����ļ����ϴ����������д�**/


/***�ֶγ��ȶ���***/	
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

/**�ʵ��ļ�ӳ��**/
typedef struct{
	long  idNo;
	long  conNo;			/*�ʻ�id*/
	long  custId;			/*�ͻ�id*/
	long  conCustId;		/*�ʻ�����id*/
	char  phoneNo[PHONENO];
	char  smCode[FSMCODE];
	char  attrCode[FATTRCODE];
	char  belongCode[FBELONG];	
	char  modeCode[FMODECODE]; 		/*�ʷ��ײ�ģ�����*/

	/*char  idconBegin[DATETIME];
	char  idconEnd[DATETIME];		�û��ʻ���ϵ����ʱ��*/

	char  billBegin[FOPTIME];		/*��ʼʱ��*/
	char  billEnd[FOPTIME];			/*����ʱ��*/
	char  billTime[FOPTIME];			/*����ʱ��*/
	char  totalDate[FTOTALDATE];		/*��������*/	
	int	 yearMonth;					/*��������*/
	int	 billDay;					/*��������*/

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


/****��ϸ�ʵ��ṹ***/
typedef struct{
	char   feeCode[FEECODE_LEN+1];		/*��Ŀ����*/
	char   detailCode[DETAILCODE];
	char   payed_status[2];
	double should;			/*Ӧ��*/
	double favour;			/*�Ż�*/
	double payed_prepay;
	double payed_later;
	int    times;			/*ͨ������*/
	int    duration;		/*ͨ��ʱ��*/
	struct DCUSTOWEDET_TYPE *next;
}DCUSTOWEDET_TYPE;

/***�ۺ��ʵ��ṹ***/
typedef struct{
	long  idNo;		/*�û�id*/
	long  custId;		/*�ͻ�id*/
	long  conNo;		/*�ʻ�id*/	
	long  conCustId;		/*�ʻ������ͻ�id*/
	char	phoneNo[PHONENO];
	int	yearMonth;	/*�ʻ�����*/ 
	int	billDay;	/*��������*/
	char	totalDate[TOTALDATE];	/*��������*/
	char    billType[BILLTYPE+1];          /*�ʵ�����*/
	char	payed_status[2];
	char	smCode[SMCODE];
	char	attrCode[ATTRCODE];
	char	billCode[BILLCODE];	 
	char	belongCode[BELONGCODE];
	double shouldPay;                   /*��Ӧ��*/
	double favourFee;                   /*���Ż�*/
	double prepay_fee;
	double payed_prepay;
	double payed_later;
	int    times;                  /*ͨ������*/
	int    duration;                   /*ͨ��ʱ��*/
	char   billFlag;
	char   billBegin[OPTIME_LEN];	/*��ʼʱ��*/
	char   billEnd[OPTIME_LEN];		/*����ʱ��*/
	char	billTime[OPTIME_LEN];	/*����ʱ��*/
	char   adjFlag;
	char   encrypt_owe[ENCRYPTOWELEN];  /*�����ֶ�*/
	struct DCUSTOWEDET_TYPE *detnext;   /*��ϸ�ʵ�ָ��**/
	struct DCUSTOWE_TYPE  *owenext;		         /**�ۺ��ʵ�ָ��***/
}DCUSTOWE_TYPE;

typedef struct{
	char  zoneFlag; /*** 0��̬�� 1 ��̬��*/
	long  recordPos;   /**blockflag=0 ��̬��ƫ���� blockflag=1 ��̬��ƫ����**/
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
