#ifndef __PUBLICEMER_H
#define __PUBLICEMER_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <string.h>
#include <ctype.h>
#include "des.h"
#include "order_xml.h"
#include "fml32.h"

/*共享内存配置个数*/
#define MAXDBCFG 5000
#define EMDIR "EMDIR"
#define EMCFGFILE "/emconfig.cfg"
#define EMKEY 1
#define QUEKEY 1101
#define MSGTYPE 1

#ifndef SQLCODE
	#define SQLCODE             sqlca.sqlcode
#endif

#ifndef COMMENT
	#define COMMENT        '#'
#endif

#ifndef init
	#define init(a) memset(a, 0, sizeof(a))
#endif

/*存放配置表sEmChangeCfg数据的结构体*/
typedef struct{
	int bExists;
	unsigned long lHashA;
	unsigned long lHashB;
	char serviceName      [20 + 1];
	char opCode           [4  + 1];
	char dbLabel          [10 + 1];
	char emFlag           [1  + 1];
	char orderCreateFlag  [1  + 1];
	char emsupportFlag    [1  + 1];
	char opTime           [19 + 1];
	char relaOpCode       [4  + 1];
	char machineCode      [4  + 1];
	char opNote           [60 + 1];
}sPublicDBCfg;
/*存放数据库表中对应主机的全部配置数据，该数据放在动态内存中*/
struct emshm{
	unsigned long sPubDBCfgCnt;
	char lastRefreshTime[17 + 1];
	sPublicDBCfg sPublicDBCfgs[MAXDBCFG];
};
/*消息队列结构体*/
struct sPubQueueMsg{
	long msgType;
	int  request;
	char quitFlag[1 + 1];
};


unsigned long HashKey(const char *vStr);
unsigned long ELFhash(const char *vStr);
long GetHashTablePos(const char *vStr, struct emshm *emTable);
int SetHashTable(const char *vStr, struct emshm *emTable, sPublicDBCfg *sDbCfg);
int sPubGetCfgFile(const char *file, char *rtn_path);
key_t sPubGetShmKey();
struct emshm *sPubCreateShm();
struct emshm *sPubGetShm();
int sPubDisconShm(struct emshm *shmt);
int sPubDelShm();
void doPrint(sPublicDBCfg *sPubDbCfg);
int getConfigKey(const char *cfgLine, char *machineCode, char *machineIP, char *Db, char *userName, char *passWd, char *refreshSpan);
int getDbCfg(const char *serviceName, sPublicDBCfg *dbCfg);
int fEmerCheck(const char *serviceName, sPublicDBCfg *oDbCfg, char *oRetCode, char *oRetMsg);
int getEmerDbLabel(const char *serviceName, char *dbLabel);
key_t sPubGetSemKey();
int sPubGetSemQueId();
int sPubCreateSem();
int sPubSndMsg(const int iQueId, const char *sMsg);
int sPubRcvMsg(const int iQueId, char *sMsg);
int sPubQuitQue(const int iQueId);

int spublicDBLoginEmer(FBFR32 *transIN, FBFR32 *transOUT, char *srvName, 
		char *connect_name, sPublicDBCfg *dbCfg);
int sChkDBLoginEmer(FBFR32 *transIN, FBFR32 *transOUT, char *srvName, 
		char *connect_name, sPublicDBCfg *dbCfg);
int spublicDBCloseEmer(char *connect_name,sPublicDBCfg *dbCfg);

#endif
