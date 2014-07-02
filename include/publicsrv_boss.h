#ifndef _PUBLICSRV_H
#define _PUBLICSRV_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <varargs.h>
#include <unistd.h>
#include <signal.h>

#include <string.h>
#include <ctype.h>
#include <atmi.h>
#include <userlog.h>
#include <fml.h>
#include <fml32.h>
#include "des.h"
#include "Uunix.h"
#include <include_boss.h>
#include <acctquery.h>

#if USE_SCCSID
static char Sccsid[] = {"%Z% %M% %I% %G%"};
#endif /* USE_SCCSID */

#define Sinitn(src) memset(src, 0, sizeof(src))
#define Sinit0(src) memset(src, '0', sizeof(src))
#define Sinits(src) memset(src, ' ', sizeof(src))

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
/* added 20020423 */

#define LCONNTYPE	"0"
#define SCONNTYPE	"1"

#define ACCSHMKEYFILE   "acceptfile.shm"
#define ACCSEMKEYFILE   "acceptfile.sem"

#define REDOSHMKEYFILE   "redofile.shm"
#define REDOSEMKEYFILE   "redofile.sem"

#define SOPCODEFILE     "sCancelOpCode.cfg"
#define CFMACCEPTFILE   "cfmAcceptFile."
#define CANCELACCEPTFILE  "cancelAcceptFile."

#define SHMKDIR 	"sDBLogin.cfg"
#define SHMKEY		1
#define PERMS		0666
#define SHMKLOGDIR 	"sLogLevel.cfg"

#define MAX_BIND 	5
#define MAXCODE 	50
#define LINE 		80
#define COMMENT 	'#'

#define TRANSSDBLOGIN 	"sDBLogin.cfg"
#define TRANSSPRNFORMAT	"sPrnFormat.cfg"
#define TRANSSMAINTITLE	"sMainTitle.cfg"
#define TRANSSLOGLEVEL	"sLogLevel.cfg"

#define DEFTIME 	50

#define BIGCOUNT        200
#define MAXLABELLEN	12
#define MAXLOGIN	100
#define MAXLOGNUM	10000

#define MAXBUFLEN       1024
#define MAXENCRYPTLEN   16
#define MAXDECRYPTLEN   16
#define WORKDIR         "WORKDIR"
#define REDODIR         "REDODIR"
#define ORGPASSWDLEN	6

#define REDOPROCESSNUM	40
#define REDOCOUNT	4


/* 共享内存存储结构(数据库) */
struct spublicdblogin {
	char          label[MAXLABELLEN+1];
	char          server[MAXLABELLEN+1];
	char          database[MAXLABELLEN+1];
	char          username[MAXLABELLEN+1];
	char          password[MAXENDELEN+1];
};  

struct comshm {
	unsigned int 		sdbloginno;
	struct spublicdblogin 	sdblogin[MAXLOGIN];
};

/* 内存流水使用的共享内存结构 */
struct acceptshm
{
	unsigned long count;
	char  sysdate[10];
};

/* 重做日志REDO使用的共享内存结构 */
struct redoShm
{
	char	memAccept[REDOPROCESSNUM*REDOCOUNT][100];
	unsigned long	redoPosition[REDOPROCESSNUM*REDOCOUNT];
	char	redoFile[REDOPROCESSNUM*REDOCOUNT][100];
	char	svcName[REDOPROCESSNUM*REDOCOUNT][20];
	char	phoneNo[REDOPROCESSNUM*REDOCOUNT][20];
	unsigned int	flag[REDOPROCESSNUM*REDOCOUNT];
	unsigned int	endFlag;
};

/* 共享内存存储结构(日志) */
struct spublicloglevel {
	char		codelevel[5+1];
	unsigned int	flag;
};
struct logshm {
	unsigned int		sloglevelno;
	struct spublicloglevel	sloglevel[MAXLOGNUM];
};	


/* MAC校验结构 */
typedef struct MacStruc {
	int     use_int;
	int     srcint[10];
	int     use_float;
	float   srcfloat[10];
        int     use_char;
        char    srcchar[10][20];
} MACSTRUC;

typedef struct PrnFormat {
	char	line_no[4+1];
	char	col_width[4+1];
	char	order_type[2+1];
	char	format_type[2+1];
	char	all_var[36+1];
} PRNFORMAT;

#define EXTERN

#ifndef EXTERN
	char   *spublicGetEnvDir( char *env);
	int    spublicGetCfgDir(const char *file, char *rtn_path);
	int    spublicGetBinDir(const char *file, char *rtn_path);
	int    spublicGetIncludeDir(const char *file, char *rtn_path);
	int    spublicGetLibDir(const char *file, char *rtn_path);
	int    spublicGetPublicDir(const char *file, char *rtn_path);
	int    spublicGetTmpDir(const char *file, char *rtn_path);

	int    spublicGetRedoDir(char *file, char *rtn_path);

	key_t  spublicGetShmKey();
	struct comshm * spublicCreatShm();
	struct comshm * spublicGetShm();
	int    spublicPutKeyShm();
	int    spublicDelShm();
	int    spublicDtcShm(struct comshm *shmp);
	struct spublicdblogin spublicGetDBP(char *connect_name);

	key_t  spublicLogGetShmKey();
	struct logshm * spublicLogCreatShm();
	struct logshm * spublicLogGetShm();
	int    spublicLogPutKeyShm();
	int    spublicLogDelShm();
	int    spublicLogDtcShm(struct logshm *shmp);
	int    spublicLogGetFlag(char *codelevel);
	struct spublicloglevel spublicLogGetDBP(char *codelevel);

	int sGetDenPasswd(char *connect_name, char *login_no, char *login_passwd);
	struct spublicdblogin spublicGetDBP(char *connect_name);
	int spublicGenMAC(char *MAINKEY, struct MacStruc *macstruc, char *out_MAC);
	int spublicRight(char *connect_name,char *login_no, char *login_passwd, char *function_code);
	int sPayGetAccept(char *connect_name, int flag);
	int spublicGetAccept(char *connect_name, int flag);
	int spublicGetnAccept(char *connect_name,int flag, int n);
	int spublicPrnFormat();

        char *Srtrim(char *src);
        char *Sltrim(char *src);
        char *Strim(char *src);
        char *Ssubstr(char *src, int begin, int length, char *dest);
        char *Slower(char *src);
        char *Supper(char *src);

        char *Tchgformat(char *src, char *dest, char *type);
        char *Tgetsysdatetime(int type);
        char *Tgetssysdatetime(char s);
        char *Tgetyear();
        char *Tgetmonth();
        char *Tgetday();
        char *Tgethour();
        char *Tgetminute();
        char *Tgetsecond();

	double roundN(double source, int num);
	void bosslog();

	int BcdToAsc_(unsigned char *dest, unsigned char *src, int length);
	int NCreatOneSem(const key_t semkey);
	int NSemDel(int semid);
	int NOpenOneSem(const key_t semkey);
	int Nsm_sem_lock(const int semid);
	int Nsm_sem_unlock(const int semid);
	int Nsem_lock_nowait(const int semid);
	int Nsem_unlock_nowait(const int semid);
	struct acceptshm * NCreatShm(key_t shmkey, int length,int *shmid);
	struct acceptshm * NOpenShm(key_t shmkey);
	int NCloseShm(const struct acceptshm *shmp);
	int NDtcShm(const struct acceptshm *shmp);
	int NDelShm(key_t shmkey);
	int NOpenShmErr(key_t shmkey);
	char *GetSysDatetime();
	void Quit(int signo);
	int CreatMemAccept();
	char *GetMemAccept();
	int DelMemAccept();
	char *GetMemDate();
	int redoLog(char *strService, FBFR *transIN);
	int redoLog32(char *strService, FBFR32 *transIN);
	char *getRedoAccept(char inPayAccept[12],char inTotalDate[9], char inOpCode[5],char inNowTotalDate[9],FBFR *transIN);
	char *getRedoAccept32(char inPayAccept[12],char inTotalDate[9], char inOpCode[5],char inNowTotalDate[9],FBFR32 *transIN);


#else
	extern char *spublicGetEnvDir( char *env);
	extern int  spublicGetCfgDir(const char *file, char rtn_path[LINE]);
	extern int  spublicGetBinDir(const char *file, char rtn_path[LINE]);
	extern int  spublicGetIncludeDir(const char *file, char rtn_path[LINE]);
	extern int  spublicGetLibDir(const char *file, char rtn_path[LINE]);
	extern int  spublicGetPublicDir(const char *file, char rtn_path[LINE]);
	extern int  spublicGetTmpDir(const char *file, char rtn_path[LINE]);
	extern int    spublicGetRedoDir(char file[18], char rtn_path[100]);
	extern key_t  spublicGetShmKey();
	extern struct comshm * spublicCreatShm();
	extern struct comshm * spublicGetShm();
	extern int    spublicDelShm();
	extern int    spublicDtcShm(struct comshm *shmp);
	extern struct spublicdblogin spublicGetDBP(char *connect_name);
	extern int spublicPutKeyShm();

	extern key_t  spublicLogGetShmKey();
	extern struct logshm * spublicLogCreatShm();
	extern struct logshm * spublicLogGetShm();
	extern int    spublicLogPutKeyShm();
	extern int    spublicLogDelShm();
	extern int    spublicLogDtcShm(struct logshm *shmp);
	extern int    spublicLogGetFlag(char *codelevel);
	extern struct spublicloglevel spublicLogGetDBP(char *codelevel);

	extern int sGetDenPasswd(char *connect_name, char *login_no, char *login_passwd);
	extern int spublicGenMAC(char *MAINKEY, struct MacStruc *macstruc, char *out_MAC);
	extern int spublicRight(char *connect_name,char *login_no, char *login_passwd, char *function_code);
	extern int sPayGetAccept(char *connect_name, int flag);
	extern int spublicGetAccept(char *connect_name, int flag);
	extern int spublicGetnAccept(char *connect_name,int flag, int n);
	extern int spublicPrnFormat();

        extern char *Srtrim(char *src);
        extern char *Sltrim(char *src);
        extern char *Strim(char *src);
        extern char *Ssubstr(char *src, int begin, int length, char *dest);
        extern char *Slower(char *src);
        extern char *Supper(char *src);
        extern char *Slower(char *src);
        extern char *Supper(char *src);

        extern char *Tchgformat(char *src, char *dest, char *type);
        extern char *Tgetsysdatetime(int type);
        extern char *Tgetssysdatetime(char s);
        extern char *Tgetyear();
        extern char *Tgetmonth();
        extern char *Tgetday();
        extern char *Tgethour();
        extern char *Tgetminute();
        extern char *Tgetsecond();

	extern double roundN(double source, int num);

	extern void bosslog();

	extern int BcdToAsc_(unsigned char *dest, unsigned char *src, int length);
	extern int NCreatOneSem(const key_t semkey);
	extern int NSemDel(int semid);
	extern int NOpenOneSem(const key_t semkey);
	extern int Nsm_sem_lock(const int semid);
	extern int Nsm_sem_unlock(const int semid);
	extern int Nsem_lock_nowait(const int semid);
	extern int Nsem_unlock_nowait(const int semid);
	extern struct acceptshm * NCreatShm(key_t shmkey, int length,int *shmid);
	extern struct acceptshm * NOpenShm(key_t shmkey);
	extern int NCloseShm(const struct acceptshm *shmp);
	extern int NDtcShm(const struct acceptshm *shmp);
	extern int NDelShm(key_t shmkey);
	extern int NOpenShmErr(key_t shmkey);
	extern char *GetSysDatetime();
	extern void Quit(int signo);
	extern int CreatMemAccept();
	extern char *GetMemAccept();
	extern int DelMemAccept();
	extern char *GetMemDate();
	extern int redoLog(char *strService, FBFR *transIN );
	extern int redoLog32(char *strService, FBFR32 *transIN);
	extern char *getRedoAccept(char inPayAccept[12],char inTotalDate[9], char inOpCode[5],char inNowTotalDate[9],FBFR *transIN);
	extern char *getRedoAccept32(char inPayAccept[12],char inTotalDate[9], char inOpCode[5],char inNowTotalDate[9],FBFR32 *transIN);


#endif
#endif /* _PUBLICSRV_H */
