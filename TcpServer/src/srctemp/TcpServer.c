/][
]**********************************************************************\
 *
 * File: SynServ.cpp
 *
 *  NOTE!!! 使用VI打开此文件时, 请将 tablespace设置为4 (:set ts=4)
 *
\**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <libgen.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include "SysVar.h"
#include "PubFunc.h"
#define _debug_
extern void doProcess(struct str_PARA *pStrucPara, int iQueryType, char* pchBuff);
extern int	doDetailQuery(struct str_PARA *pStrucPara, int iSocket,int pchQueryType,char *outbuff);
extern int 	doSPQuery(struct str_PARA *pStrucPara,char *outbuff);
extern int 	GetPacketHead(char *,char *);
extern int 	GenarateSendbuffer(char *,char * ,char * );
extern void GetCurrentTime(int, char *);
extern int	InitServerSocket(int *,	char *, char *);
extern int 	Encrypt(const char*, char *);
extern int 	unEncrypt(const char*, char *);
extern int 	LogIt(char *,char *) ;
extern int 	LockFile(char* );
extern int 	convertpath(char *);
extern int 	getConfpath(char *);
extern int 	readcfg(char *,char *,char *,char *);
extern char* ALLTrim(char *);  /*modify miaoyl at 20110322*/
extern int	Initialize(char *);
extern int  getWorkOfCase(char *, int *,int *);
extern int 	CheckProcNumber(int , int,int );
extern void dototalfee(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff);
extern int  callTuxedoTotalfee(struct tagTuxedoCall *pTuxedoCall,char* pTuxedoReturn);


void sig_int(int);
void sig_int1(int);
void TimeOut(int );
extern void TimeOutAlarm(int );
extern void InitSignal();
extern int 	GetStringNumber(char*, char**);
extern void sig_chld(int);
extern void sig_chld1(int);
extern int 	RecvResultOnHP(int, char *,int,int);
extern int 	getPara(const char*,char*,STRPARA*);
extern int 	Create_share(key_t,int );
extern int 	CreateSEM(key_t);
extern int 	creat_semaphore(key_t,int);
extern int 	init_semaphore(int,int,int);
extern int 	unlocksem(int, int);
extern int 	locksem(int,int);
extern int 	IncreaseChildPid(int,int,pid_t);
extern int 	decreaseChildPid(int,int,pid_t);
extern void Killsonpid(int,int);
extern int 	Checkheartbeat(char* );

/*********************全局变量**********************************/
int  MainQuitFlag = 0;
char logpath[255]; 
char IPAddress[30];
char IPPort[10];
char LockFilePath[255];
int	 acceptsocket=0;
int  ipflag = 0;
char acconnip[MAX_LEGALIP_COUNT][20];
int  ipcount = 0;
int  sonquit=0;
int  MaxProcNumber=0;
int  ShmId  = 0;
int  SemId  = 0;
unsigned int  TimeOut_ForCon = 0;
STRPARA  ParaValue;

char workaddresscfg[100+1];  
char worktocasecfg[100+1];   
char bossappcfg[100+1];      
char errorcodecfg[100+1];    
char phoneheadcfg[100+1];    
char bossorcrmcfg[100+1];    
char chWsnAddr[100+1];       

#define init(a)   memset(a,0,sizeof(a));/*modify miaoyl at 20110322*/

/***************************************************************/
int main( int argc, char **argv )
{
	int    i = 0;
	char   cfgFilePath[255];
	int    nRet = 0;
	int	   socketfd=-1;
	struct sockaddr_in Connect_Address;
	int    length=0;
	char   connIp[32];
	pid_t  childpid = 0;
	char   RecvBuff[RECV_SIZE];
	char   TransCode[13];
	char*  PacketBody = NULL;
	char   SeparateFlag[2];
	key_t  IPCkey = 0;
	char Packethead[PACKETHEAD+2];
	char ForDetailSendBuffer[1024*10000+146];
	char Detailbuff[1024*10000];
	char src[8+1];
	char dest[8+1];
	int flag=0,code=0,call_flag=0;
	SHAREBUFFER* pShareBuffer = NULL;
			
	memset(Packethead,0,sizeof(Packethead));
	memset(cfgFilePath,0,sizeof(cfgFilePath));
	memset(logpath,0,sizeof(logpath));	
	memset(IPAddress,0,sizeof(IPAddress));
	memset(IPPort,0,sizeof(IPPort));
	memset(HEAD,0,sizeof(HEAD));
	memset(LockFilePath,0,sizeof(LockFilePath));
	memset(&Connect_Address,0,sizeof(struct sockaddr_in));
	memset(connIp,0,sizeof(connIp));	
	memset(TransCode,0,sizeof(TransCode));
	memset(&ParaValue,0,sizeof(STRPARA));
	memset(&g_WorkAddress,0,sizeof(WORKADDRESS));
	memset(&g_BossorCrm,0,sizeof(BOSSORCRM));
	memset(&g_phonehead,0,sizeof(PHONEHEAD));
	memset(&g_worktocase,0,sizeof(WORKTOCASE));
	memset(&g_bossapp,0,sizeof(BOSSAPP));
	memset(&g_Bosserrcode,0,sizeof(BOSSERRCODE));
	memset(phoneheadcfg,0,sizeof(phoneheadcfg));
	memset(workaddresscfg,0,sizeof(workaddresscfg));
	memset(worktocasecfg,0,sizeof(worktocasecfg));
	memset(bossappcfg,0,sizeof(bossappcfg));
	memset(errorcodecfg,0,sizeof(errorcodecfg));
	
	SeparateFlag[0] = '\t';
	SeparateFlag[1] = '\0';
	if(argc<2)
	{	
		printf("start error! example:%s cfgfile\n",argv[0]);
		exit(1);
	}	
	strcpy(cfgFilePath,argv[1]);
	nRet = Initialize(cfgFilePath);
	if(nRet<0)
	{
		printf("Initialize error! Error	number=%d\n",nRet);
		return -1;
	}
	strcat(logpath,"/");
	sprintf(HEAD,"%s",logpath);
	
	sprintf(loginfo,	"Program<%d> started!\n",getpid());
	LogIt(loginfo,HEAD);
	
	#ifdef _debug_ 
		sprintf(loginfo,"cfgFilePath:[%s]\n",cfgFilePath);
		LogIt(loginfo,HEAD);
		sprintf(loginfo,"MaxProcNumber=[%d]\n",MaxProcNumber);
		LogIt(loginfo,HEAD);
		sprintf(loginfo,"TimeOut_ForCon=[%d]\n",TimeOut_ForCon);
		LogIt(loginfo,HEAD);
		sprintf(loginfo,"logpath=[%s]\n",logpath);
		LogIt(loginfo,HEAD);
		sprintf(loginfo,"IPAddress=[%s],IPPort=[%s]\n",IPAddress,IPPort);
		LogIt(loginfo,HEAD);
		sprintf(loginfo,"LockFilePath=[%s]\n",LockFilePath);
		LogIt(loginfo,HEAD);
		for(int i=0; i<ipcount; i++)
	  {	
	  	sprintf(loginfo,"acconnip[%d]=[%s]\n",i,acconnip[i]);
	  	LogIt(loginfo,HEAD);
	  }
	#endif 
	
	nRet=getBossapp();
	if(nRet<0)
	{
		sprintf(loginfo,"getBossapp error! Error	number=%d\n",nRet);
	  LogIt(loginfo,HEAD);
		return -1;
	}
	
	nRet=getPhoneHead();
	if(nRet<0)
	{
		sprintf(loginfo,"getPhoneHead error! Error	number=%d\n",nRet);
	  LogIt(loginfo,HEAD);
		return -1;
	}
	
	nRet=getWorkAddressHead();
	if(nRet<0)
	{
		sprintf(loginfo,"getWorkAddressHead error! Error	number=%d\n",nRet);
	  LogIt(loginfo,HEAD);
		return -1;
	}
	
	nRet=getBossorCrm();
	if(nRet<0)
	{
		sprintf(loginfo,"getBossorCrm error! Error	number=%d\n",nRet);
	  LogIt(loginfo,HEAD);
		return -1;
	}
	
	nRet=getWorkToCase();
	if(nRet<0)
	{
		sprintf(loginfo,"getWorkToCase error! Error	number=%d\n",nRet);
	    LogIt(loginfo,HEAD);
		return -1;
	}
	nRet=getBosserrcode();
	if(nRet<0)
	{
		sprintf(loginfo,"getWorkToCase error! Error	number=%d\n",nRet);
	  LogIt(loginfo,HEAD);
		return -1;
	}
	if((nRet=LockFile(LockFilePath)) != 1)
	{
		if (nRet == -1)   
		{
			sprintf(loginfo,"operate lock file %s failed!",LockFilePath);
			LogIt(loginfo,HEAD);
			exit(-1);
		} 
		else if (nRet == 0)      
		{
			sprintf(loginfo,"TcpServer has already been exist!");
			LogIt(loginfo,HEAD);
			exit(0);
		}
	}
	IPCkey = ftok(cfgFilePath,525);
	ShmId = Create_share(IPCkey,sizeof(SHAREBUFFER));
	if(ShmId == -1)
	{
	  sprintf(loginfo,"%s","创建共享内存失败" );
    LogIt(loginfo,HEAD);
    return -1;
	}
	pShareBuffer = (SHAREBUFFER*)shmat(ShmId,0,0);
	if(pShareBuffer == (SHAREBUFFER*)SHM_FAILED)
	{
		sprintf(loginfo,"%s","连接共享内存失败" );
		LogIt(loginfo,HEAD);
		return -1;
	}
	for(i = 0; i < MAXPROCNUMBER; i++)
	{
		pShareBuffer->ProcPid[i] = -1;
	}
	pShareBuffer->nProcCount = 0;
	shmdt(pShareBuffer);
	
	SemId = CreateSEM(IPCkey);
	if(SemId == -1)
	{
	  sprintf(loginfo,"%s","创建信号量失败" );
	  LogIt(loginfo,HEAD);
    return -1;
  }
	nRet = InitServerSocket(&socketfd,IPAddress,IPPort);
	if (nRet < 0)
	{
		sprintf(loginfo,"Init ServerSocket failed!error:<%d>,errmsg:<%s>.Program exit!",	socketfd, strerror(socketfd));
		LogIt(loginfo,HEAD);
		return -1;
	}
	while (!MainQuitFlag)
	{		
		#ifdef _debug_
			sprintf(loginfo,"\n进入主循环，等待连接\n\n");
	  	LogIt(loginfo,HEAD);
		#endif
		length =sizeof(Connect_Address);
 		acceptsocket = accept(socketfd, (struct sockaddr *)(&Connect_Address),&length);
		#ifdef _debug_
			sprintf(loginfo,"connect acceptsocket=[%d]\n",acceptsocket);
	  	LogIt(loginfo,HEAD);
		#endif
 		if (acceptsocket< 0)
 		{
 			if(MainQuitFlag)
	 			break;
	  	continue;
 		}
		/*判断连接IP是否合法，非法则记录日志，取消该连接*/
		memset(connIp,0,sizeof(connIp));
		ipflag = 0;
		strcpy(connIp,inet_ntoa(Connect_Address.sin_addr));
		if(strcmp(acconnip[0],"0.0.0.0") == 0)
			ipflag = 1;
		for (i=0;i<ipcount;i++)
		{			
			if(strcmp(acconnip[i],connIp) == 0)
			{
				ipflag = 1;				
				break;
			}
		}
		if(!ipflag)
		{			
			sprintf(loginfo, "Connect ip isn't legal [%s]", connIp );
			LogIt(loginfo,HEAD);
			close(acceptsocket);
			sleep(1);
			continue;
		}	
		else
		{
			#ifdef _debug_
				sprintf(loginfo,"connect legal ip=[%s]\n",connIp);
				LogIt(loginfo,HEAD);
			#endif
		}
		if(CheckProcNumber(SemId,ShmId,MaxProcNumber)!=0)
		{
			sprintf(loginfo, "Connect has arrived MaxProcNumber=[%d]",MaxProcNumber);
			LogIt(loginfo,HEAD);
			close(acceptsocket);
			sleep(1);			
			continue;
		}
		childpid = fork();
		if (childpid < 0)
		{
			sprintf(loginfo,"Fork Child Error->[%s]",strerror(errno));
			LogIt(loginfo,HEAD);
			break;
		}
		if (childpid ==	0)
		{
			close(socketfd);			
			struct sigaction act1, oact1;
			sigemptyset(&act1.sa_mask);
			act1.sa_handler = sig_int1;
			act1.sa_flags = 0;
			sigaction(SIGINT, &act1,	&oact1);
			sigaction(SIGTERM, &act1,&oact1);
			sigaction(SIGQUIT, &act1,&oact1);
			
		  act1.sa_handler = TimeOut;
  		sigemptyset(&act1.sa_mask);
  		act1.sa_flags = 0;
  		sigaction(SIGALRM, &act1,&oact1);
		  	
		  while(!sonquit)
		  {
		  	memset(RecvBuff, 0, sizeof(RecvBuff));		  		
		  	alarm(TimeOut_ForCon);		  		
		  	nRet = RecvResultOnHP(acceptsocket,RecvBuff,RECV_SIZE,30);		  		
		  	alarm(TimeOut_ForCon);		  		
		  	if(nRet <= 0)
		  	{
		  		#ifdef _debug_
		  			sprintf(loginfo,"[%s]断开连接\n",connIp);
						LogIt(loginfo,HEAD);
		  		#endif
		  		break;
		  	}
		  	#ifdef _debug_
		  		sprintf(loginfo,"++++++++sonpid begin dealing+++++++++++\n");
    			LogIt(loginfo,HEAD);
					sprintf(loginfo,"packlen=[%d]\n",nRet);
					LogIt(loginfo,HEAD);
				#endif
		  	sprintf(loginfo,"RecvBuff=[%s]\n",RecvBuff );
    		LogIt(loginfo,HEAD);
				if(Checkheartbeat(RecvBuff) == 0)
				{
				  sprintf(loginfo,"Recv is heartbeat");
    			LogIt(loginfo,HEAD);
    			memset(Packethead,0,sizeof(Packethead));
		  		GetPacketHead(RecvBuff,Packethead);
		  		Packethead[42]='1';
		  		Packethead[PACKETHEAD]='\n';
		  		Packethead[PACKETHEAD+1]='\0';
		  		sprintf(loginfo,"SendBuffer = [%s]\n",Packethead);
    			LogIt(loginfo,HEAD);
		  		send(acceptsocket,Packethead,strlen(Packethead),0);
		  		continue;
				}		
		  	PacketBody = &RecvBuff[PACKETHEAD];
		  	memset(&ParaValue,0,sizeof(STRPARA));
		  	getPara(PacketBody,SeparateFlag,&ParaValue);
				memset(Packethead,0,sizeof(Packethead));
				memset(Detailbuff, 0, sizeof(Detailbuff));
		  	memset(ForDetailSendBuffer,0,sizeof(ForDetailSendBuffer));
		  	GetPacketHead(RecvBuff,Packethead);
		  	code=0;
		  	call_flag=0;
		  	if(getWorkOfCase(ParaValue.acParaName[3],&code,&call_flag)!=0)
		  	{
		  		send(acceptsocket,RecvBuff,RECV_SIZE,0);
		  		sprintf(loginfo,"this work not exists\n");
    			LogIt(loginfo,HEAD);
    			continue;
		  	}	  			
		  	if(code==1001||code==1057||code==1069||code==1031)
		  	{
		  		memset(src,0,sizeof(src));
					memset(dest,0,sizeof(dest));
					strncpy(src,ParaValue.acParaName[5],8);
		  		flag=Encrypt(src,dest);
		  		strncpy(ParaValue.acParaName[5],dest,8);
		  		if(code==1031)
		  		{
		  			memset(src,0,sizeof(src));
						memset(dest,0,sizeof(dest));
		  			strncpy(src,ParaValue.acParaName[6],8);
		  			flag=Encrypt(src,dest);
		  			strncpy(ParaValue.acParaName[6],dest,8);
		  		}
		  	}
		  	else if(code==1049||code==1050)
		  	{
		  		if(strcmp(ParaValue.acParaName[6],"0")==0)
					strcpy(ParaValue.acParaName[6],"未缴");
					else if(strcmp(ParaValue.acParaName[6],"1")==0)
					strcpy(ParaValue.acParaName[6],"已缴");
		  	}
		  	init(Detailbuff);
		  	init(ForDetailSendBuffer);
		  	if(call_flag>=1&&call_flag<=5)
		  	{
		  		nRet=getWorkWsnAddr(ParaValue.acParaName[3],chWsnAddr);
		  		if(nRet<0)
 					{
 						sprintf(Detailbuff,"%s","0\t0\t-1\n");
 						call_flag=6;
 					}
 					else if(nRet>0)
 					{ 			
 						memset(chWsnAddr, 0, sizeof(chWsnAddr));
 						flag = getPhoneWsnAddr(ParaValue.acParaName[4],nRet,chWsnAddr);
 						if(flag<0)
 						{
 							sprintf(Detailbuff,"%s","0\t0\t-100\n");
 							call_flag=6;
 						}
 					}
 				
 				sprintf(loginfo,"chWsnAddr[%s]\n",chWsnAddr);
    			LogIt(loginfo,HEAD);
    			if(call_flag==1)
		  			doProcess(&ParaValue,code,Detailbuff);
		  		else if(call_flag==2)
		  			dototalfee(&ParaValue, code,Detailbuff);		
		  		else if(call_flag==3)
		  		  doDetailQuery(&ParaValue,acceptsocket,code,Detailbuff);		  	  	
		  		else if(call_flag==4)
		  		  doSPQuery(&ParaValue,Detailbuff);
		  		else if(call_flag==5)
		  			doInvoicePrint(&ParaValue,code,Detailbuff);
		  		GenarateSendbuffer(Packethead,Detailbuff,ForDetailSendBuffer);
		  		sprintf(loginfo,"SendBuffer = [%s]\n",ForDetailSendBuffer);
					LogIt(loginfo,HEAD);
		  		send(acceptsocket,ForDetailSendBuffer,strlen(ForDetailSendBuffer),0);
		  	}
		  	else if(call_flag!=6)
		  	{
		  	  send(acceptsocket,RecvBuff,RECV_SIZE,0);	
		  	}
		  	init(Detailbuff);
		  	init(ForDetailSendBuffer);
		  	sprintf(loginfo,"++++++++sonpid end deailing+++++++++++\n\n");
    		LogIt(loginfo,HEAD);
		  }
		  close(acceptsocket);
		  nRet = decreaseChildPid(ShmId,SemId,getpid());
			if(nRet < 0)
			{
				sprintf(loginfo,"decreaseChildPid Error");
				LogIt(loginfo,HEAD);
			}			
			exit(0);
		}
		close(acceptsocket);
		#ifdef _debug_
			sprintf(loginfo,"running a process,pid=[%d]\n",childpid);
			LogIt(loginfo,HEAD);
		#endif
		nRet = IncreaseChildPid(ShmId,SemId,childpid);
		if(nRet < 0)
		{
			sprintf(loginfo,"IncreaseChildPid Error");
			LogIt(loginfo,HEAD);
			break;
		}
	}
	close(socketfd);
	Killsonpid(ShmId,SemId);
	sprintf(loginfo,"Main proc realease all ,exit success!");
	LogIt(loginfo,HEAD);
	shmctl(ShmId,IPC_RMID,0);
	semctl(SemId,IPC_RMID,0);
  return( 0 );
}
  
void sig_int(int signal_type)
{
	MainQuitFlag=1;
	#ifdef _debug_
		sprintf(loginfo,"SIGNAL quit=1\n");
		LogIt(loginfo,HEAD);
	#endif
}
int getWorkOfCase(char *pchWork, int *case_code,int *call_flag)
{
	char pwork[50];
	sprintf(pwork,"%s",pchWork);
	int i = 0;
	int ret=0;
	
	for(i = 0; i < g_worktocase.count ; i++)
	{
		if(strcmp(g_worktocase.work[i],pwork)==0&&strcmp(ParaValue.acParaName[(g_worktocase.para[i])],g_worktocase.para_value[i])==0)
		{
			*case_code=g_worktocase.case_code[i];
			*call_flag=g_worktocase.call_flag[i];
			ret=1;
			break;
		}
	}
	if(ret!=1)
		return -1;
	return 0;
}

void InitSignal()
{
	struct sigaction act, oact;

	sigemptyset(&act.sa_mask);
	act.sa_handler = sig_int;
	act.sa_flags = 0;
	sigaction(SIGINT, &act,	&oact);
	sigaction(SIGTERM, &act,&oact);
	sigaction(SIGQUIT, &act,&oact);

	act.sa_handler = SIG_IGN;
  	sigemptyset(&act.sa_mask);
  	act.sa_flags = 0;
  	sigaction(SIGHUP, &act,&oact);
  	sigaction(SIGPIPE, &act,&oact);

  	act.sa_handler = TimeOutAlarm;
  	sigemptyset(&act.sa_mask);
  	act.sa_flags = 0;
  	sigaction(SIGALRM, &act,&oact);

  	act.sa_handler = sig_chld;
  	sigemptyset(&act.sa_mask);
  	act.sa_flags = 0;
  	sigaction(SIGCHLD, &act,&oact);
}

int	Initialize(char *filepath) 
{
	char section[20];
	int  result = 0;
	char key[50];
	char clientip[MAX_LEGALIP_COUNT*20];
	char *pconnIp[MAX_LEGALIP_COUNT];	
	int i = 0;
	char maxprocnumber[10];
	char timeout[20];
	
	memset(timeout,0,sizeof(timeout));
	memset(maxprocnumber,0,sizeof(maxprocnumber));
	memset(section,0,sizeof(section));
	memset(key,0,sizeof(key));
	memset(clientip,0,sizeof(clientip));
		
	InitSignal();	
	
	sprintf(section,"TCPSERVERCFG_1");
	sprintf(key,"LogPath");
	if (readcfg( filepath, section, key, logpath) < 0)
  {                                                   
  	printf("Get	LogPath	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(logpath);
  
  memset(key,0,sizeof(key));
  sprintf(key,"workaddresscfg");
	if (readcfg( filepath, section, key, workaddresscfg) < 0)
  {                                                   
  	printf("Get	workaddresscfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(workaddresscfg);
  
  memset(key,0,sizeof(key));
  sprintf(key,"worktocasecfg");
	if (readcfg( filepath, section, key, worktocasecfg) < 0)
  {                                                   
  	printf("Get	worktocasecfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(worktocasecfg);
  
  memset(key,0,sizeof(key));
  sprintf(key,"bossappcfg");
	if (readcfg( filepath, section, key, bossappcfg) < 0)
  {                                                   
  	printf("Get	bossappcfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(bossappcfg);
  
  memset(key,0,sizeof(key));
  sprintf(key,"errorcodecfg");
	if (readcfg( filepath, section, key, errorcodecfg) < 0)
  {                                                   
  	printf("Get	errorcodecfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(errorcodecfg);
  
  memset(key,0,sizeof(key));
  sprintf(key,"phoneheadcfg");
	if (readcfg( filepath, section, key, phoneheadcfg) < 0)
  {                                                   
  	printf("Get	phoneheadcfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(phoneheadcfg);
  
  memset(key,0,sizeof(key));
  sprintf(key,"bossorcrmcfg");
	if (readcfg( filepath, section, key, bossorcrmcfg) < 0)
  {                                                   
  	printf("Get	bossorcrmcfg	 failed!\n");                                                                                                                   
  	return	- 1;                                    
  } 
  ALLTrim(bossorcrmcfg);
       
  memset(key,0,sizeof(key));
  sprintf(key,"IP");
  if ((result=readcfg( filepath, section, key, IPAddress)) < 0)	
  {
  	if(result==-4)                                           
  		sprintf(IPAddress,"%s","INADDR_ANY");                         
  	else                                                     
  	{                                                        
  		printf("Get	IP	failed!\n");                                                         
  		return - 2;                                          
  	}                                                        
  }                                                            
  ALLTrim(IPAddress); 
                             
  memset(key,0,sizeof(key));
  sprintf(key,"Port");     
  if (readcfg( filepath, section, key, IPPort) < 0)	
  {    
  	printf("Get	 Port failed!\n");                       
    return - 3;                                          
  }                                                            
  ALLTrim(IPPort);
  
  memset(key,0,sizeof(key));
  sprintf(key,"MaxProc");     
  if (readcfg( filepath, section, key, maxprocnumber) < 0)	
  {    
  	printf("Get	 MaxProc failed!\n");                       
    return - 3;                                          
  }                                                            
  ALLTrim(maxprocnumber);
  MaxProcNumber = atoi(maxprocnumber);
  
  memset(key,0,sizeof(key));
  sprintf(key,"TimeOut");     
  if (readcfg( filepath, section, key, timeout) < 0)	
  {    
  	printf("Get	 TimeOut failed!\n");                       
    return - 3;                                          
  }                                                            
  ALLTrim(timeout);
  TimeOut_ForCon = atoi(timeout);
  
  memset(key,0,sizeof(key));
  sprintf(key,"LockFilePath");     
  if (readcfg( filepath, section, key, LockFilePath) < 0)	
  {
  	printf("Get	 LockFilePath failed!\n");                       
    return - 3;                                          
  }
  ALLTrim(LockFilePath); 
  
  memset(key,0,sizeof(key));
  sprintf(key,"LegalIP");
  if((result=readcfg(filepath,section,key,clientip))<=0)
	{
		sprintf(loginfo,"config--clientip flag is error,please check config!error:%d\n",result);
		printf("loginfo=%s\n",loginfo);
		exit(1);
  }
  ALLTrim(clientip);
  for(int i=0;i<MAX_LEGALIP_COUNT;i++)
  {
		pconnIp[i] = acconnip[i];
		memset(pconnIp[i],0,sizeof(pconnIp[i]));
	}  
  memset(acconnip,0,sizeof(acconnip));
  ipcount = GetStringNumber(clientip,pconnIp); 
  return 0;                                                
}

void sig_int1(int signal_type)
{
	sonquit = 1;
	#ifdef _debug_
		sprintf(loginfo,"SIGNAL sonquit=1\n");
		LogIt(loginfo,HEAD);
	#endif
}

void TimeOut(int sig)
{
	int nRet = 0;
	close(acceptsocket);
	nRet = decreaseChildPid(ShmId,SemId,getpid());
	sprintf(loginfo,"无心跳,超时,结束处理子进程\n");
	LogIt(loginfo,HEAD);
	if(nRet < 0)
	{
		sprintf(loginfo,"decreaseChildPid Error");
		LogIt(loginfo,HEAD);
	}			
	exit(0);
}    
                                                                                                                   