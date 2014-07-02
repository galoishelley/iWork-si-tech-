#include <stdio.h>
#include <stdlib.h>
#include <userlog.h>
#include <string.h>

#define LOG_INF(xx) { userlog("Info: %s\n", xx); fflush(stdout); }


int AddFileNameNo(char *strFileName,char *strDealTime)
{
  char strBuffer[128];
  char strBufferFile[512];
  int  nFileNo = 0;
   
  if(strlen(strDealTime) != 14)
  	return -1;
  	
  memset(strBuffer,0,sizeof(strBuffer));
  memcpy(strBuffer,strDealTime+10,2);
  nFileNo = (atoi(strBuffer)/15)*15;
  
  memset(strBuffer,0,sizeof(strBuffer));
  memcpy(strBuffer,strDealTime,10);
  
  memset(strBufferFile,0,sizeof(strBufferFile));
  sprintf(strBufferFile,"%s%s%02d",strFileName,strBuffer,nFileNo);
  strcpy(strFileName,strBufferFile);
  
  return 0;
}

int NMSWriteStartLog(char *RunFlag,long Sequence,char *BelongCode,char *LoginNo,char *OrgCode,char *OpCode,char *UsrID, char *UsrNo, char *ServiceName,char *BeginTime, int ParaNum,char *Para1, char *Para2,char *Para3,char *Para4)
{

        char FileName[256];
        char *IsmpEnv;
				char tempstr[256];

        FILE *Fp;


        IsmpEnv = (char *)getenv("NMSLOG");
        if (IsmpEnv == NULL) {
                userlog("Error: set NMSLOG env variable ,pls.\n");
                return -1;
        }

        memset(FileName, 0, sizeof(FileName));
        strcpy(FileName , IsmpEnv);
        
        
        if(AddFileNameNo(FileName,BeginTime)< 0){
        				userlog("Error: AddFileNameNo NMSLOG file err.Check it,pls!\n");
                return -2;
				}
				
        if ((Fp = fopen(FileName, "a+")) == NULL){
                userlog("Error: open NMSLOG file err.Check it,pls!\n");
                return -2;
        }

        switch (ParaNum) {
                case 1:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, ServiceName, BeginTime);
                        break;
                case 2:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, ServiceName, BeginTime);
                        break;
                case 3:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, Para3, ServiceName, BeginTime);
                        break;
                case 4:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, Para3, Para4, ServiceName, BeginTime);
                        break;
                default:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, ServiceName, BeginTime);
                        break;
        }

        fclose(Fp);

	memset(tempstr,0,sizeof(tempstr));
	sprintf(tempstr,"chmod 777 %s",FileName);
	system(tempstr);

        return 0;
}

int NMSWriteEndLog  (char *RunFlag,long Sequence,char *BelongCode,char *LoginNo,char *OrgCode,char *OpCode,char *UsrID, char *UsrNo, char *ServiceName,char *BeginTime,char *EndTime, char *ServiceState,int ParaNum,char *Para1, char *Para2,char *Para3,char *Para4)
{


        char FileName[256];
        char *IsmpEnv;
				char tempstr[256];

        FILE *Fp;


        IsmpEnv = (char *)getenv("NMSLOG");
        if (IsmpEnv == NULL) {
                userlog("Error: set NMSLOG env variable ,pls.\n");
                return -1;
        }

        memset(FileName, 0, sizeof(FileName));
        strcpy(FileName , IsmpEnv);
        
        
        if(AddFileNameNo(FileName,EndTime)< 0){
        				userlog("Error:AddFileNameNo NMSLOG file err.Check it,pls!\n");
                return -2;
				}

        if ((Fp = fopen(FileName, "a+")) == NULL){
                userlog("Error: open NMSLOG file err.Check it,pls!\n");
                return -2;
        }

        switch (ParaNum) {
                case 1:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, ServiceName, BeginTime, EndTime, ServiceState);
                        break;
                case 2:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, ServiceName, BeginTime,EndTime, ServiceState);
                        break;
                case 3:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, Para3, ServiceName, BeginTime,EndTime, ServiceState);
                        break;
                case 4:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, Para1, Para2, Para3, Para4, ServiceName, BeginTime,EndTime, ServiceState);
                        break;
                default:
                        fprintf(Fp,"%s,%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", RunFlag, Sequence, BelongCode,LoginNo,OrgCode, OpCode, UsrID, UsrNo, ServiceName, BeginTime,EndTime, ServiceState);
                        break;
        }

        fclose(Fp);

	memset(tempstr,0,sizeof(tempstr));
	sprintf(tempstr,"chmod 777 %s",FileName);
	system(tempstr);

        return 0;
}

/*int main(int argc, char *argv[])
{	 
	char strTime[32];
	char strTime2[32];
	struct tm	tm_now;
	time_t t_now;
	
	while(1)
	{
		memset(strTime,0,sizeof(strTime));
		t_now = time(NULL);
		memset(&tm_now,0,sizeof(struct tm));
		tm_now = *(localtime(&t_now));
		sprintf(strTime,"%d%02d%02d%02d%02d%02d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, 
						tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
		
		memset(strTime2,0,sizeof(strTime2));
		t_now = time(NULL) + 10;
		memset(&tm_now,0,sizeof(struct tm));
		tm_now = *(localtime(&t_now));
		sprintf(strTime2,"%d%02d%02d%02d%02d%02d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, 
						tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
		NMSWriteStartLog((char*)"BEGIN",12123130,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID", (char*)"UsrNo", (char*)"ServiceName",strTime, 0,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
		
		NMSWriteStartLog((char*)"BEGIN",121231321,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID", (char*)"UsrNo", (char*)"ServiceName",strTime, 1,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
		
		NMSWriteStartLog((char*)"BEGIN",121231322,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID", (char*)"UsrNo", (char*)"ServiceName",strTime, 2,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
		
		NMSWriteStartLog((char*)"BEGIN",121231323,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID", (char*)"UsrNo", (char*)"ServiceName",strTime, 3,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
		
		NMSWriteStartLog((char*)"BEGIN",121231324,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID", (char*)"UsrNo", (char*)"ServiceName",strTime, 4,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
		
		
		
		NMSWriteEndLog((char*)"END",200707290,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID",(char*)"UsrNo",(char*)"ServiceName",strTime,strTime2,(char*)"ServiceState",0,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
	
		NMSWriteEndLog((char*)"END",200707291,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID",(char*)"UsrNo",(char*)"ServiceName",strTime,strTime2,(char*)"ServiceState",1,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
	
		NMSWriteEndLog((char*)"END",200707292,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID",(char*)"UsrNo",(char*)"ServiceName",strTime,strTime2,(char*)"ServiceState",2,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
	
		NMSWriteEndLog((char*)"END",200707293,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID",(char*)"UsrNo",(char*)"ServiceName",strTime,strTime2,(char*)"ServiceState",3,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
	
		NMSWriteEndLog((char*)"END",200707294,(char*)"BelongCode",(char*)"LoginNo",(char*)"OrgCode",(char*)"OpCode",(char*)"UsrID",(char*)"UsrNo",(char*)"ServiceName",strTime,strTime2,(char*)"ServiceState",4,(char*)"Para1",(char*)"Para2",(char*)"Para3",(char*)"Para4");
	
		system("sleep 5");
	}
	
	exit(0);
}*/
