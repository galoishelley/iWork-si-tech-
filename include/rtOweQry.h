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


/***�궨��***/
#define  VAL_LEN		64
#define  PAYED			'0'	
#define  OWED			'1'


char LOGINACCEPT[20+1];
char OP_TIME[15];


/***�ֶγ��ȶ���***/	
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
	long  conNo;				/*�ʻ�id	*/
	long  custId;				/*�ͻ�id	*/
	long  conCustId;			/*�ʻ�����id*/
	char  phoneNo[PHONENO];	
	char  smCode[FSMCODE];	
	char  attrCode[FATTRCODE];	
	char  belongCode[FBELONG];	
	char  groupId[FGROUP];		/*��������		*/
	char  modeCode[FMODECODE]; 	/*�ʷ��ײ�ģ�����	*/
	
	char  billBegin[FOPTIME];	/*��ʼʱ��*/
	char  billEnd[FOPTIME];		/*����ʱ��*/
	char  billTime[FOPTIME];	/*����ʱ��*/
	char  totalDate[FTOTALDATE];	/*��������*/
	int   yearMonth;			/*��������*/
	int   billDay;				/*��������*/
	char  billType[BILLTYPE];	/*�ʵ�����*/
	
	char  billFlag;
	char  adjFlag;
	char  encryptOwe[FENCRYPTOWE];	

	double  limitingPay;		/**�������ۼ��޶�    **/
	double  sumShould;			/**�������ۼ�Ӧ���ܶ�**/
	double  sumFavour;			/**�������ۼ��Ż��ܶ�**/

	double  incrShould;			/**����������Ӧ���ܶ�**/
	double  incrFavour;			/**�����������Ż��ܶ�**/
	int	incrTimes;
	int	incrDuras;
		
	struct DCUSTOWEDET_TYPE *detnext; /**��ϸ�Ľṹָ�룬�ۺϲ�ѯʱΪ��**/
	struct DCUSTOWE_TYPE *owenext;	
};
typedef struct DCUSTOWE_TYPE	DCUSTOWE_TYPE;


struct DCUSTOWEDET_TYPE
{
	char  feeCode[FEECODE_LEN];
	char  detCode[DETAILCODE];

	double  incrshld;	/*����������Ӧ����ϸ*/
	double  incrfav;	/*�����������Ż���ϸ*/
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
	long conno;				/**���ֻ���ѯʱconno=0**/
	char begintime[TIME_LEN];		/**YYYYMMDDHHMISI�磺20031103213045**/
	char endtime[TIME_LEN];
	char qrytype;				/**0   ��ѯ�ۺ��ʵ���1��ѯ�ۺ���ϸ�ʵ� 2 billqry**/
};
typedef struct QRY_TYPE	QRY_TYPE;

#define  SUFFIXLEN 7
typedef struct 
{
	char serviceId[PHONENO];
	char suffix[SUFFIXLEN+1];
	long pos;
}INDEX_TYPE;


