#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <varargs.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/***宏定义***/
#define  VAL_LEN		64
#define  PAYED			'0'	
#define  OWED			'1'


char LOGINACCEPT[20+1];
char OP_TIME[15];


/***字段长度定义***/	
#define PHONENO			16
#define FSMCODE			3
#define FATTRCODE		9
#define FBELONG			8
#define FGROUP			11
#define FMODECODE		9
#define FDATETIME		15
#define FOPTIME			15
#define FTOTALDATE  	9
#define FENCRYPTOWE		17

#define	 BILLTYPE		2
#define  FEECODE_LEN	4
#define  DETAILCODE		6
#define  TIME_LEN		15


struct DCUSTOWE_TYPE
{
	long  idNo;
	long  conNo;				/*帐户id	*/
	long  custId;				/*客户id	*/
	long  conCustId;			/*帐户归属id*/
	char  phoneNo[PHONENO];	
	char  smCode[FSMCODE];	
	char  attrCode[FATTRCODE];	
	char  belongCode[FBELONG];	
	char  groupId[FGROUP];		/*归属代码		*/
	char  modeCode[FMODECODE]; 	/*资费套餐模板代码	*/
	
	char  billBegin[FOPTIME];	/*开始时间*/
	char  billEnd[FOPTIME];		/*结束时间*/
	char  billTime[FOPTIME];	/*帐务时间*/
	char  totalDate[FTOTALDATE];	/*帐务日期*/
	int   yearMonth;			/*出帐年月*/
	int   billDay;				/*出帐批次*/
	char  billType[BILLTYPE];	/*帐单类型*/
	
	char  billFlag;
	char  adjFlag;
	char  encryptOwe[FENCRYPTOWE];	

	double  limitingPay;		/**新增，累计限额    **/
	double  sumShould;			/**新增，累计应收总额**/
	double  sumFavour;			/**新增，累计优惠总额**/

	double  incrShould;			/**新增，增量应收总额**/
	double  incrFavour;			/**新增，增量优惠总额**/
	int	incrTimes;
	int	incrDuras;
		
	struct DCUSTOWEDET_TYPE *detnext; /**明细的结构指针，综合查询时为空**/
	struct DCUSTOWE_TYPE *owenext;	
};
typedef struct DCUSTOWE_TYPE	DCUSTOWE_TYPE;


struct DCUSTOWEDET_TYPE
{
	char  feeCode[FEECODE_LEN];
	char  detCode[DETAILCODE];

	double  incrshld;	/*新增，增量应收明细*/
	double  incrfav;	/*新增，增量优惠明细*/
	int     incrtimes;
	int     incrduras;
	struct DCUSTOWEDET_TYPE *next;
};
typedef struct DCUSTOWEDET_TYPE	DCUSTOWEDET_TYPE;


#define QRYALL '0'
#define QRYDET '1'
#define QRYBILL  '2'

struct QRY_TYPE{
	char phoneno[PHONENO];
	long conno;				/**按手机查询时conno=0**/
	char begintime[TIME_LEN];		/**YYYYMMDDHHMISI如：20031103213045**/
	char endtime[TIME_LEN];
	char qrytype;				/**0   查询综合帐单，1查询综合明细帐单 2 billqry**/
};
typedef struct QRY_TYPE	QRY_TYPE;

#define  SUFFIXLEN 7
typedef struct 
{
	char serviceId[PHONENO];
	char suffix[SUFFIXLEN+1];
	long pos;
}INDEX_TYPE;


