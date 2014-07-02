/**********************************************************************\
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


/*#define _debug_*/

extern void doProcess(struct str_PARA *pStrucPara, int iQueryType, char* pchBuff);
extern int	doDetailQuery(struct str_PARA *pStrucPara, int iSocket,char* pchQueryType,char *outbuff);
extern int 	doSPQuery(struct str_PARA *pStrucPara,char *outbuff);
extern int 	GetPacketHead(char *,char *);
extern int 	GenarateSendbuffer(char *,char * ,char * );
extern void GetCurrentTime(int, char *);
extern int	InitServerSocket(int *,	char *, char *);
extern int 	Encrypt(const char*, char *);
extern int 	unEncrypt(const char*, char *);
extern int 	BubbleSort(PHONEHEAD *);
extern int 	LogIt(char *,char *) ;
extern int 	LockFile(char* );
extern int 	convertpath(char *);
extern int 	getConfpath(char *);
extern int 	readcfg(char *,char *,char *,char *);
extern void ALLTrim(char *);  
extern int	Initialize(char *);
extern int 	CheckProcNumber(int , int,int );
extern void dototalfee(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff);
extern int callTuxedoTotalfee(struct tagTuxedoCall *pTuxedoCall,char* pTuxedoReturn);

void sig_int(int);
void sig_int1(int);
void TimeOut(int );
extern void TimeOutAlarm(int );
extern void InitSignal();
extern int 	GetStringNumber(char*, char**);
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
int  			MainQuitFlag = 0;
char 			logpath[255]; 
char 			IPAddress[30];
char 			IPPort[10];

char 			LockFilePath[255];
int	 			acceptsocket=0;
int  			ipflag = 0;
char 			acconnip[MAX_LEGALIP_COUNT][20];
int  			ipcount = 0;
int  			sonquit=0;
int  			MaxProcNumber=0;
int  			ShmId  = 0;
int  			SemId  = 0;
unsigned int  	TimeOut_ForCon = 0;


/***************************************************************/
int
main( int argc, char **argv )
{
	int    	i = 0;
	char   	cfgFilePath[255];
	int    	nRet = 0;
	int	   	socketfd=-1;
	struct 	sockaddr_in Connect_Address;
	int    	length=0;
	char   	connIp[32];
	pid_t  	childpid = 0;
	char   	RecvBuff[RECV_SIZE];
	char   	TransCode[13];
	STRPARA ParaValue;
	char*  	PacketBody = NULL;
	char   	SeparateFlag[2];
	key_t  	IPCkey = 0;

	SHAREBUFFER* pShareBuffer = NULL;
	
	char 	chBuff[4096];
	char 	Packethead[PACKETHEAD+2];
	char 	SendBuffer[4496];
	char 	src[8+1];
	char 	dest[8+1];
	int 	flag=0;
	
	memset(SendBuffer,0,sizeof(SendBuffer));
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
	
	SeparateFlag[0] = '\t';
	SeparateFlag[1] = '\0';
		
	nRet = getConfpath(cfgFilePath);
	if(nRet < 0)
	{
		printf("请检查配置文件$ADAPTERDIR/TcpServerCfg/TcpServer.cfg是否存在！\n");
	  return -1; 
	}	
	nRet = Initialize(cfgFilePath);
	if(nRet<0)
	{
		printf("Initialize error! Error	number=%d\n",nRet);
		return -1;
	}		
	strcat(logpath,"/");
	sprintf(HEAD,"%s",logpath);
	
	sprintf(loginfo,	"Program<%d> started!.",getpid());
	LogIt(loginfo,HEAD);
	
	memset(&g_phonehead, 0, sizeof(g_phonehead));
	getPhoneHead();
	getWorkAddressHead();
	#ifdef _debug_
	printf("g_phoneheadsum=[%d]\n",g_phonehead.count);
	printf("g_WorkAddresssum=[%d]\n",g_WorkAddress.count);
	#endif
	BubbleSort(&g_phonehead);
	
	strcat(LockFilePath,"/TcpServer.lk");
	if((nRet=LockFile(LockFilePath)) != 1)
	{
		if (nRet == -1)   
		{
			sprintf(loginfo,"operate lock file %s failed!",LockFilePath);
			LogIt(loginfo,HEAD);
			printf("operate lock file %s failed!\n",LockFilePath);
			exit(-1);
		} 
		else if (nRet == 0)      
		{
			sprintf(loginfo,"OTATcpServer has already been exist!");
			LogIt(loginfo,HEAD);
			printf("TcpServer has already been exist!\n");
			exit(0);
		}
	}		
	IPCkey = ftok(cfgFilePath,466);
	#ifdef _debug_
	printf("IPCkey=[%d]\n",IPCkey);
	#endif
	ShmId = Create_share(IPCkey,sizeof(SHAREBUFFER));
	if(ShmId == -1)
	{
	    sprintf(loginfo,"%s","创建共享内存失败" );
    	LogIt(loginfo,HEAD);
    	return -1;
	}
	#ifdef _debug_
	printf("创建成功ShmId=[%d]\n",ShmId);
	#endif
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
  #ifdef _debug_
  	printf("创建成功SemId=[%d]\n",SemId);
  #endif
	nRet = InitServerSocket(&socketfd,IPAddress,IPPort);
	if (nRet < 0)
	{
	  printf(	"Init ServerSocket failed!->[%s]!\n",strerror(socketfd));
		sprintf(loginfo,"Init ServerSocket failed!error:<%d>,errmsg:<%s>.Program exit!",	socketfd, strerror(socketfd));
		LogIt(loginfo,HEAD);
		return -1;
	}
	#ifdef _debug_
	printf("process running ip=[%s],port=[%s]\n",IPAddress,IPPort);
	#endif	
	int m=0;
	while (!MainQuitFlag)
	{	
		#ifdef _debug_
			printf("进入主循环，等待连接\n");
		#endif
		length =sizeof(Connect_Address);
 		acceptsocket = accept(socketfd, (struct sockaddr *)(&Connect_Address),&length);
		#ifdef _debug_
 			printf("connect acceptsocket=[%d]\n",acceptsocket);
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
		{
			ipflag = 1;
		}
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
			#ifdef _debug_
				printf("%s\n",loginfo);
			#endif			
			close(acceptsocket);
			sleep(1);
			continue;
		}	
		else
		{
			#ifdef _debug_
				printf("connect legal ip=[%s]\n",connIp);
			#endif
		}
		if(CheckProcNumber(SemId,ShmId,MaxProcNumber)!=0)
		{
			sprintf(loginfo, "Connect has arrived MaxProcNumber=[%d]",MaxProcNumber);
			LogIt(loginfo,HEAD);
			sleep(1);
			close(acceptsocket);
			continue;
		}
		childpid = fork();
		if (childpid < 0)
		{
			#ifdef _debug_
				printf("Fork failed!\n");
			#endif
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
  		
		  memset(RecvBuff, 0, sizeof(RecvBuff));	  		
		  nRet = RecvResultOnHP(acceptsocket,RecvBuff,RECV_SIZE,30);	  			  		
		  if(nRet <= 0)
		  {
		  	#ifdef _debug_
		  		printf("[%s]断开连接\n",connIp);
		  	#endif
		  	/******add by 赵东伟*****/
		  	close(acceptsocket);
		  	nRet = decreaseChildPid(ShmId,SemId,getpid());
		  	if(nRet < 0)
				{
					sprintf(loginfo,"decreaseChildPid Error");
					LogIt(loginfo,HEAD);
				}
				exit(0);
				/************************/
		  }
		  sprintf(loginfo,"RecvBuff=[%s][%d]\n",RecvBuff,nRet);
    	LogIt(loginfo,HEAD);
			#ifdef _debug_
		  printf("RecvBuff = [%s]\n",RecvBuff);
			#endif  	
		  PacketBody = &RecvBuff[PACKETHEAD];
		  memset(&ParaValue,0,sizeof(STRPARA));
		  getPara(PacketBody,SeparateFlag,&ParaValue);
		  #ifdef _debug_
		  for(i = 0; i < ParaValue.nParaCount; i ++ )
		  {
		  	sprintf(loginfo,"para:[%s]\n",ParaValue.acParaName[i]);
    		LogIt(loginfo,HEAD);
		  }
		  #endif
			memset(Packethead,0,sizeof(Packethead));
		  GetPacketHead(RecvBuff,Packethead);
		  if(!strcmp(ParaValue.acParaName[3],"QUERY_FOR_OTA"))
		  {
		  	memset(chBuff, 0, sizeof(chBuff));
		  	doProcess(&ParaValue, 1046,chBuff);
		  	#ifdef _debug_
		  		printf("doProcess chBuff=[%s]\n",chBuff,strlen(chBuff));
				#endif
		  	memset(SendBuffer,0,sizeof(SendBuffer));
				GenarateSendbuffer(Packethead,chBuff,SendBuffer);
				sprintf(loginfo,"SendBuffer = [%s]\n",SendBuffer);
				LogIt(loginfo,HEAD);
				#ifdef _debug_
					printf("SendBuffer = [%s]\n",SendBuffer);
				#endif
				int i=0;
		 		i=send(acceptsocket,SendBuffer,strlen(SendBuffer),0);		
		 	}
		 	else
		 	{
		 	  	send(acceptsocket,RecvBuff,RECV_SIZE,0);	
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
			printf("running a process,pid=[%d]\n",childpid);
		#endif
		nRet = IncreaseChildPid(ShmId,SemId,childpid);
		if(nRet < 0)
		{
			sprintf(loginfo,"IncreaseChildPid Error");
			LogIt(loginfo,HEAD);
			
		}
	}
		                                            
	close(socketfd);
	Killsonpid(ShmId,SemId);
	sprintf(loginfo,"Main proc realease all ,exit success!");
	shmctl(ShmId,IPC_RMID,0);
	semctl(SemId,IPC_RMID,0);
	LogIt(loginfo,HEAD);
	#ifdef _debug_
		printf("%s\n",loginfo);
	#endif
	printf("success end\n");
  return( 0 );
}
  
void sig_int(int signal_type)
{
	MainQuitFlag=1;
	#ifdef _debug_
		printf("SIGNAL quit=1\n");
	#endif
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

  act.sa_handler = SIG_IGN;
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
  result=convertpath(logpath);                   
  if(result==-1)                                 
  {                                              
  	printf("Environment does not exist!\n");   
  	return -1;                                 
  }                                              
  if(result == -2)                               
  {                                              
  	printf("Environment not set correctly!\n");
  	return -1;                                 
  }     
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
  #ifdef _debug_
  printf("MaxProcNumber = [%d]\n",MaxProcNumber);
  #endif
  
  memset(key,0,sizeof(key));
  sprintf(key,"TimeOut");     
  if (readcfg( filepath, section, key, timeout) < 0)	
  {    
  	printf("Get	 TimeOut failed!\n");                       
      return - 3;                                          
  }                                                            
  ALLTrim(timeout);
  TimeOut_ForCon = atoi(timeout);
  #ifdef _debug_
  printf("TimeOut_ForCon = [%d]\n",TimeOut_ForCon);
  #endif
  
  memset(key,0,sizeof(key));
  sprintf(key,"LockFilePath");     
  if (readcfg( filepath, section, key, LockFilePath) < 0)	
  {    
  	printf("Get	 LockFilePath failed!\n");                       
      return - 3;                                          
  }   
 
  ALLTrim(LockFilePath); 
  result=convertpath(LockFilePath);                   
  if(result==-1)                                 
  {                                              
  	printf("Environment does not exist!\n");   
  	return -1;                                 
  }                                              
  if(result == -2)                               
  {                                              
  	printf("Environment not set correctly!\n");
  	return -1;                                 
  }  
  
     
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
  for(int i=0; i<ipcount; i++)
  {
  	#ifdef _debug_
  	printf("acconnip[%d]=[%s]\n",i,acconnip[i]);
  	#endif
  } 
  
  #ifdef _debug_  
  printf("logpath = [%s]\n",logpath);
  printf("IPAddress = [%s]\n",IPAddress);
	printf("IPPort = [%s]\n",IPPort);  
	printf("LockFilePath = [%s]\n",LockFilePath);
	#endif  
  return 0;                                                
}  

void sig_int1(int signal_type)
{
		int nRet = 0;
	close(acceptsocket);
	nRet = decreaseChildPid(ShmId,SemId,getpid());
	printf("sig_int1");
	if(nRet < 0)
	{
		sprintf(loginfo,"decreaseChildPid Error");
		LogIt(loginfo,HEAD);
	}
			
	exit(0);
	#ifdef _debug_
		printf("SIGNAL sonquit=1\n");
	#endif
}

void TimeOut(int sig)
{
	int nRet = 0;
	close(acceptsocket);
	nRet = decreaseChildPid(ShmId,SemId,getpid());
	printf("无心跳,超时,结束处理子进程\n");
	if(nRet < 0)
	{
		sprintf(loginfo,"decreaseChildPid Error");
		LogIt(loginfo,HEAD);
	}
			
	exit(0);
}    

                                                                                                                    

