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
   TlsAtom			   *mAtomServer[CATOM_COUNT];		/*ԭ�ӷ���*/
   TlsTableSet		   *mInOutCfg;						/*������������������ñ�*/
   TlsTableSet		   *mSerInfo;
   int				   mAtomCount;							/*ԭ�ӷ�������*/
   int				   mRouteCode;							/*·�ɴ���*/
   char				   mstablename[30];							/*·�ɱ���*/
   char				   mroutevalue[30];							/*·�ɱ�ֵ*/
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
