#include <stdarg.h>
#include "boss_srv.h"
#include "publicsrv.h"


#define SPLITCHR ","
#define MAX_ARRSZ      9
#define MAX_VAR_LEN    128


/***
 拆文本串
*/

void split(char input[MAX_ARRSZ][MAX_VAR_LEN +1 ],char* buffer)
{
	int i=0;
	char tmp[512];
	char *buf;
	
	strcpy(tmp,buffer);
	buf= strstr( buffer, SPLITCHR);
	
	while(strlen(buf))
  {
  	memset(input[i],0x00,sizeof(input[i]));
  	strncpy(input[i],tmp,strlen(tmp)-strlen(buf));
		sprintf(tmp,"%s",buf+1);
		buf = strstr( tmp, SPLITCHR);
		i++;
	}
	
	strncpy(input[i],tmp,strlen(tmp)-strlen(buf)-1);
	
}

/***
 获取当前操作时间
*/

void gettime(char* buf)
{
	time_t tmp_time;
	struct tm *tmp_tm;
	char realdate[20];
	
	memset(realdate,0x00,sizeof(realdate));

	tmp_time=time(NULL);
	tmp_tm=localtime(&tmp_time);
	sprintf(realdate,"%04d-%02d-%02d %02d:%02d:%02d",
	                 tmp_tm->tm_year+1900, tmp_tm->tm_mon+1,tmp_tm->tm_mday,
	                 tmp_tm->tm_hour,tmp_tm->tm_min,tmp_tm->tm_sec);
	                 
   memcpy(buf,realdate,20);
}

int callServer(char* iServiceName, char* vRetCode, char* vRetMsg,char* iInputNum, char* iOutNum,...)
{
	int i,vInputNum,vOutNum;
	va_list vAp;
	
	vInputNum = atoi(iInputNum);
	vOutNum = atoi(iOutNum);
	
	char *tmp;
	char vArgv[50][256];
	char vOut[50][256];
	
	/**
	*tuxedo 参数
	*/
	FBFR32 *sendbuf, *rcvbuf;
	FLDLEN32 sendlen, rcvlen;
	long reqlen;
  char buffer[256];
  char vtime[20];
  int ret,occs;
  
  memset(vArgv,0x00,sizeof(vArgv[0])*vInputNum);
  memset(vOut,0x00,sizeof(vOut[0])*vOutNum);
  memset(vtime,0x00,sizeof(vtime));
  
  gettime(vtime);
  
  if (tpinit((TPINIT *) NULL) == -1) {
  	strcpy(vRetCode, "100000");
		sprintf(vRetMsg,"%s Tpinit failed\n",vtime);
		return 1;
  }
  
  sendlen = (FLDLEN32)(vInputNum*100);
  
  if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
	        strcpy(vRetCode, "100001");
					sprintf(vRetMsg, "Error allocating send buffer");
					tpterm();
	        return 1;
	}
	
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,iInputNum,(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,iOutNum,(FLDLEN32)0);
	
	va_start(vAp, iOutNum);
	
	for(i=0;i<atoi(iInputNum);i++)
	{
		tmp=va_arg(vAp, char*);
		strcpy(vArgv[i],tmp);
		Fchg32(sendbuf,GPARM32_0+i,0,vArgv[i],(FLDLEN32)0);
		
		printf("vArgv[%d] %s\n",i,vArgv[i]);
	}
	
	va_end(vAp);
	
	rcvlen = (FLDLEN32)(vOutNum*100);
	
  if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
	        strcpy(vRetCode, "100002");
					sprintf(vRetMsg, "Error allocating receive buffer");
	        tpfree((char *)sendbuf);
	        tpterm();
	        return 1;
	}
	
	reqlen=Fsizeof32(rcvbuf);
	
	ret = tpcall(iServiceName, (char *)sendbuf, 0, (char **)&rcvbuf, &reqlen, (long)0);
	
	if(ret == -1)
	{
	        strcpy(vRetCode, "98");
					sprintf(vRetMsg, "Can't send request to service TOUPPER");
	        printf( "Tperrno = %d\n", tperrno);
	        tpfree((char *)sendbuf);
	        tpfree((char *)rcvbuf);
	        tpterm();
	        return 1;
	}
	
	Fget32(rcvbuf,SVC_ERR_NO32,0,vRetCode,NULL);
	printf("++ SVC_ERR_NO32 = [%s] \n", vRetCode);
	Fget32(rcvbuf,SVC_ERR_MSG32,0,vRetMsg,NULL);
	printf("++ SVC_ERR_MSG32 = [%s] \n", vRetMsg);
	
	for(i=0;i<atoi(iOutNum);i++)
	{
		occs = 0;
    occs = Foccur32(rcvbuf, GPARM32_0 + i);
    if(occs == 0)
    {
    	Fget32(rcvbuf,GPARM32_0+i,0,vOut[i],NULL);
    	printf("RECV GPARM32_[%d]=[%s]\n", i, vOut[i]);
    }
	}
		
	tpterm();
	
	if(strncmp(vRetCode,"00",2) != 0)
		return atoi(vRetCode);
	
	return 0;
}

/**
用于服务端发送服务
**/

int callServerS(char* iServiceName, char* vRetCode, char* vRetMsg, char* iInputNum, char* iOutNum,...)
{
	int i,vInputNum,vOutNum;
	va_list vAp;
	
	vInputNum = atoi(iInputNum);
	vOutNum = atoi(iOutNum);
	
	char *tmp;
	char vArgv[50][256];
	char vOut[50][256];
	
	/**
	*tuxedo 参数
	*/
	FBFR32 *sendbuf, *rcvbuf;
	FLDLEN32 sendlen, rcvlen;
	long reqlen;
  char buffer[256];
  int ret,occs;
  
  memset(vArgv,0x00,sizeof(vArgv[0])*vInputNum);
  memset(vOut,0x00,sizeof(vOut[0])*vOutNum);
  
  sendlen = (FLDLEN32)(vInputNum*100);
  
  if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
	        strcpy(vRetCode, "100001");
					sprintf(vRetMsg, "Error allocating send buffer");
	        return 1;
	}
	
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,iInputNum,(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,iOutNum,(FLDLEN32)0);
	
	va_start(vAp, iOutNum);
	
	for(i=0;i<atoi(iInputNum);i++)
	{
		tmp=va_arg(vAp, char*);
		strcpy(vArgv[i],tmp);
		Fchg32(sendbuf,GPARM32_0+i,0,vArgv[i],(FLDLEN32)0);
		
		printf("vArgv[%d] %s\n",i,vArgv[i]);
	}
	
	va_end(vAp);
	
	rcvlen = (FLDLEN32)(vOutNum*100);
	
  if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
	        strcpy(vRetCode, "100002");
					sprintf(vRetMsg, "Error allocating receive buffer");
	        tpfree((char *)sendbuf);
	        return 1;
	}
	
	reqlen=Fsizeof32(rcvbuf);
	
	ret = tpcall(iServiceName, (char *)sendbuf, 0, (char **)&rcvbuf, &reqlen, (long)0);
	
	if(ret == -1)
	{
	        strcpy(vRetCode, "98");
					sprintf(vRetMsg, "Can't send request to service TOUPPER");
	        printf( "Tperrno = %d\n", tperrno);
	        tpfree((char *)sendbuf);
	        tpfree((char *)rcvbuf);
	        return 1;
	}
	
	Fget32(rcvbuf,SVC_ERR_NO32,0,vRetCode,NULL);
	printf("++ SVC_ERR_NO32 = [%s] \n", vRetCode);
	Fget32(rcvbuf,SVC_ERR_MSG32,0,vRetMsg,NULL);
	printf("++ SVC_ERR_MSG32 = [%s] \n", vRetMsg);
	
	for(i=0;i<atoi(iOutNum);i++)
	{
		occs = 0;
    occs = Foccur32(rcvbuf, GPARM32_0 + i);
    if(occs == 0)
    {
    	Fget32(rcvbuf,GPARM32_0+i,0,vOut[i],NULL);
    	printf("RECV GPARM32_[%d]=[%s]\n", i, vOut[i]);
    }
	}
	
	if(strncmp(vRetCode,"00",2) != 0)
		return atoi(vRetCode);
	
	return 0;
}

