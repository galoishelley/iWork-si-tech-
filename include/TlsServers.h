/*
 *	make by xmx 2006.04.14
 */
#ifndef _TLSSERVERS
#define _TLSSERVERS


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TlsAtom.h"

 
#define  CATOM_COUNT  100

typedef struct TlsServers
{
   char                mCode[7];
   int				   log_level;
   char				   cp_name[30];
   TlsAtom			   *mAtomServer[CATOM_COUNT];		/*原子服务*/
   TlsTableSet		   *mInOutCfg;						/*服务输入输出参数配置表*/
   TlsTableSet		   *mSerInfo;
   int				   mAtomCount;							/*原子服务数量*/
   int				   mRouteCode;							/*路由代码*/
   char				   mstablename[30];							/*路由表单名*/
   char				   mroutevalue[30];							/*路由表值*/
}TlsServers;


void TlsServers_Init(TlsServers *vTlsServers);
TlsServers *TlsServers_New(char *vServersCode);
int TlsServers_GetConfig(char *vSerCode,TlsServers *vTlsServers);
void TlsServers_Free(TlsServers *vTlsServers);
int TlsServers_DoServer(TlsServers *vTlsServers,TlsTableSet *vDate,TRetMsg *vRetMsg);
int TlsServers_AddAtom(TlsServers *vTlsServers,char *vAtomCodes,char *vOrderCode);

int TlsServers_GenInParam(TlsServers *vTlsServers,TlsAtom *vlsAtom,TlsTableSet *vInData,TlsTableSet *vOutData,TRetMsg *vRetMsg);
int TlsServers_GenOutParam(TlsServers *vTlsServers,TlsAtom *vlsAtom,TlsTableSet *vInData,TlsTableSet *vOutData,TRetMsg *vRetMsg);

void TlsServers_CAFree(TlsServerBase *vTlsServerBase);
int TlsServers_CAGetConfig(char *vSerCode,TlsAtom *vTlsAtom);

int TlsServers_GetAtomByOrder(TlsServers *vTlsServer,char *vRunOrder);

void TlsServers_ToShow(TlsServers *vTlsServer);

#endif
