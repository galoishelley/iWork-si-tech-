#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <varargs.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <setjmp.h>
#include <termio.h>
#include <pthread.h>

#define 	_SHOWTIME_

#define		SUCCE					0

#define		LENGTH				512
#define		FILEPATH_LEN	512

#define		FAVDETCODE		5
#define		MODECODE			9
#define		PHONELEN			16
#define		CONLEN				32
#define		DATETIME			15
#define		PHHEADLEN			8
#define		MAXPERSFOBJ		8
#define		MAXPERSFCOND	8

#define		MAXDYNASFNODE	8000
#define		MAXSTATICNODE	10000

#define		PROCNUM				"PROCNUM"
#define  	DBLINK				"DBLINK"

#define 	UNZONEFLAG		'0'
#define 	STATICZONE		'1'		/***静态区标志***/
#define 	DYNAZONE			'2'		/***动态区标志***/

#define 	FREEABLE   		'9'	
#define 	UNFREEABLE 		'6'

#define 	MSG_FLAG			"SUMFAV"
#define 	HEAD_LEN			18
#define		DBILL_OWE			'0'
#define		DBILL_DET			'2'


typedef struct
{
	int  forknum;
	char  dblink[64];
	char  bcpend[7];
	char  cfgFile[FILEPATH_LEN];
	char  errLog[FILEPATH_LEN];
	char  debugLog[FILEPATH_LEN];
	char  bcpfile[FILEPATH_LEN];
	char  owefile[FILEPATH_LEN];
	char  inxfile[FILEPATH_LEN];
	char  msgInfoFile[FILEPATH_LEN];
	char  paybcp[FILEPATH_LEN];
	char  errinfo[FILEPATH_LEN];
}IniPathType;
IniPathType iniInfo;

typedef struct
{
	short  pos;
	pid_t  pid;
	pid_t  dbpid;
	int    msgid;
}TaskProcType;
#define TASKPROCSIZE sizeof(TaskProcType)
TaskProcType *taskProc;

typedef struct
{
	char phoneNo[PHONELEN];
	char begintime[DATETIME];
	char endtime[DATETIME];
}ReadSockType;
#define READSOCKTYPESIZE sizeof(ReadSockType)
ReadSockType readSockNode[1];

typedef struct
{
	char  file[FILEPATH_LEN];
	int   fd;
	long  fsize;
	void  *pmmap;
}MmapHandleType;
#define MMAPHANDLE_SIZE sizeof(MmapHandleType)
MmapHandleType	*gMmapHandle;
int	gMmapHandleNum;

typedef struct
{
	int			condOrder;
	double	hasCondedFee;
	double	condedFee;
}SumFavCondNode;
#define SUMFAVCONDSIZE sizeof(SumFavCondNode)

typedef struct
{
	char		favourType;
	char		modeCodeDet[MODECODE];
	char		favourCode[FAVDETCODE];
	int			orderCode;
	double  hasObjFee;
	double	objFee;
	double  hasObjCond;
	double  objCond;
	double  hasFavedFee;
	double	favedFee;
	double	modeFavour;
	char	cycleBegin[DATETIME];
	char	cycleEnd[DATETIME];
	SumFavCondNode sumFavCondDet[MAXPERSFCOND];
}SumFavObjNode;
#define SUMFAVOBJSIZE sizeof(SumFavObjNode)

typedef struct
{
	long	idNo;
	char	phoneNo[PHONELEN];
	SumFavObjNode  sumFavObjDet[MAXPERSFOBJ];
	long  nextRecAddr;
}SumFavType;
#define SUMFAVSIZE sizeof(SumFavType)

typedef struct
{
	char				bitmap[MAXDYNASFNODE];
	SumFavType  stillSF[MAXSTATICNODE];
	SumFavType  dynaSF[MAXDYNASFNODE];
}SumFavMmap;

typedef struct sumFavBuff
{
	char	zoneFlag;
	long	zonePos;
	long	nextPos;
	char	freeFlag;
	int		maxObjPos;
	SumFavObjNode  objList[MAXPERSFOBJ];
}SumFavBuffType;
#define SUMFAVBUFFSIZE sizeof(SumFavBuffType)

typedef struct
{
	long	idNo;
	char	phoneNo[PHONELEN];
	SumFavBuffType *sumfavList;
}SumFavListType;
#define SUMFAVLISTSIZE sizeof(SumFavListType)
SumFavListType *sumFav;

typedef struct
{
char	beg[6];
char	ver[3];
char	msgtype;
char	msglen[4];
char	nextlen[4];
char	msgbody[1024];
}billInfoType;
#define	INFOLEN		sizeof(billInfoType)
billInfoType	*billinfo;

typedef struct
{
	long  idNo;
	char  phoneNo[PHONELEN];
	int   indexPos;
	char  head[PHHEADLEN];	
	char  beginTime[DATETIME];
	char  endTime[DATETIME];

	char  sysTime[DATETIME];
	char  sysYm[7];
	char  sysDay[3];
	
	char  owefile[FILEPATH_LEN];

	MmapHandleType  *mmaphandle;
	SumFavBuffType 	*ob_buf;
	int	ob_buf_num;
}OnceDataType;
#define ONCEDATA_SIZE sizeof(OnceDataType)
OnceDataType onceData;

struct SumFavDetType
{
	char		modeCodeDet[MODECODE];	
	char		favourCode[FAVDETCODE];
	double 	hasObjFee;
	double	objFee;
	double  	hasObjCond;
	double 	objCond;
	double  	hasFavedFee;
	double	favedFee;
	double	modeFavour;
	double	minusFee;
	char		cycleBegin[DATETIME];
	char		cycleEnd[DATETIME];
	struct SumFavDetType	*next;
};
typedef struct SumFavDetType SumFavDetType;

struct dCustSumFavType
{
	long		idNo;
	char		phoneNo[PHONELEN];
	SumFavDetType	*detNext; 
	struct dCustSumFavType		*next;
};
typedef struct dCustSumFavType dCustSumFavType;

typedef struct
{
	char		modeCodeDet[9];
	char		favourCode[5];
	char  	hasObjFee[64];
	char		objFee[64];
	char  	hasObjCond[64];
	char 	 	objCond[64];
	char  	hasFavedFee[64];
	char		favedFee[64];
	char		modeFavour[64];
	char		minusFee[64];
	char		cycleBegin[15];
	char		cycleEnd[15];
	SumFavDetType	*detnext;
}sumFavDet_com;
#define	SUMFAVDETCOMSIZE		sizeof(sumFavDet_com)
sumFavDet_com *detcomm;

typedef struct
{
	char  	idNo[32];
	char  	phoneNo[16];
	SumFavDetType	*detNext; 
	dCustSumFavType		*next;
}sumFav_com;

#define	SUMFAVCOMSIZE		sizeof(sumFav_com)
sumFav_com	*sumfavcom;

typedef struct
{
	char	phoneNo[PHONELEN];
	char	begintime[DATETIME];
	char	endtime[DATETIME];
}QRY_TYPE;
#define	QRY_LEN	sizeof(QRY_TYPE)

SumFavType   		sfnode[1];
SumFavObjNode 	sfojbnode[1];

#define	ISSPACE(x) ((x)==' '||(x)=='\r'||(x)=='\n'||(x)=='\f'||(x)=='\b'||(x)=='\t')
#define SFOFFSET(a,b,c)	(a*sizeof(char) + b*SUMFAVSIZE   + c*SUMFAVSIZE)

int		chkProgNum(char *progName);
int		startDaemon();
int		readIniFile();
int		GetBcpend(char *p_fname, char *p_yymm);
int		getPara(char *fname,char *name,char *para);
int		readCfgFile();
int		toNum(char *source);
int		getSumFavFile(char *owefile);
int		checkFileStat(char *fname,char *errinfo);
int 	CmpMmapHandle(const void *a, const void *b);
int 	SortMmapHandle(const void *a, const void *b);
int		GetMmapHandle(char *p_file, MmapHandleType *p_handle, int p_num, MmapHandleType **ret_handle);
int		CreateMmapHandle(char *p_file, MmapHandleType **p_handle, int *p_num, char *errinfo);
int		getSumFavBlock(SumFavMmap *p_mmap, int p_pos, char *p_beTime,char *p_endTime,char *p_phone, SumFavType *p_empty,SumFavBuffType **p_OB_Buf,int *p_ob_num,char *p_errstr);
int		sendSockMsg(int sockid);

char	*trim(char *string);
char 	*setspace(char *s,int len);

long	*meter(int nchildren);
long 	getFileSize(char *fname,char *errinfo);

void	debugLog();
void	errLog();
void 	sig_chld(int singo);
void	child_main(int i, int listenfd,int msgid);
void	get_sockfd_value(int sockfd,int msgid);

