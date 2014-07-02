/*
 *	文件名：boss_http.c
 *
 *	版权所有 (C) 2002	神州数码(中国)有限公司
 *
 *	描述：使用libcurl进行http连接，与一级boss通讯模块
 *        使用本模块前必须先安装libcurl库
 *
 *	版本：1.0
 *
 *	修订记录：
 *	修订日期		修订人		修订内容
 *  20080108        王建勋      提供http 客户端
 */

#include "boss_http.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "boss_http.h"

#define BUFFER_LEN 10240
#define HTTP_TIMEOUT 10

static char* sBossHttpErrMsg_BossHttp[]=
{ /*错误消息*/
/*错误0 */  "Call successfully!",
/*错误1 */  "Error to malloc memory!",
/*错误2 */  "NULL parameter!",
/*错误3 */  "Unknown error!"
};

/**=============================================================================
    函数名: PutDataCallback_BossHttp()
      描述: 上传数据到http server的回调函数
      参数:
            void *ptr:存放要上传的数据头指针
            size_t size:要上传每个数据块的大小
            size_t nmemb:要上传数据块数
            void *data:传递的BOSS_HTTP_HANDLE
      输入: 无
      输出: 无
    返回值:
            实际要上传的字节数
==============================================================================*/
static size_t PutDataCallback_BossHttp(void *ptr,size_t size,size_t nmemb,void *data)
{
  size_t lPutByte;
  BOSS_HTTP_HANDLE* pstHttpHandle;

  lPutByte = size * nmemb;
  pstHttpHandle = (BOSS_HTTP_HANDLE*)data;
  if((pstHttpHandle->lInputSize - pstHttpHandle->lPutPos) < lPutByte)
    lPutByte = pstHttpHandle->lInputSize - pstHttpHandle->lPutPos;

  /*no bytes need to by upload*/
  if(lPutByte == 0)
    return 0;
  memcpy(ptr, &(pstHttpHandle->psInput[pstHttpHandle->lPutPos]), lPutByte);
  pstHttpHandle->lPutPos += lPutByte;
  return lPutByte;
}

/**=============================================================================
    函数名: GetDataCallback_BossHttp()
      描述: 下传数据到client的回调函数
      参数:
            void *ptr:存放要下传的数据头指针
            size_t size:要下传每个数据块的大小
            size_t nmemb:要下传数据块数
            void *stream:传递的BOSS_HTTP_HANDLE
      输入: 无
      输出: 无
    返回值:
            实际要下传的字节数
==============================================================================*/
static size_t GetDataCallback_BossHttp(void *ptr,size_t size,size_t nmemb,void *data)
{
  int iReadSize;
  BOSS_HTTP_HANDLE* pstHttpHandle;

  iReadSize = size * nmemb;
  /*no bytes need to be read*/
  if(iReadSize == 0)
    return 0;

  pstHttpHandle = (BOSS_HTTP_HANDLE*)data;
  /*realloc memory to store the read string*/
  pstHttpHandle->psOutput = (char*)realloc(pstHttpHandle->psOutput,
    pstHttpHandle->lOutputSize+iReadSize+1);

  /*read into memory*/
  if(pstHttpHandle->psOutput!=NULL)
  {
    memcpy(&(pstHttpHandle->psOutput[pstHttpHandle->lOutputSize]),ptr,iReadSize);
    pstHttpHandle->lOutputSize += iReadSize;
    pstHttpHandle->psOutput[pstHttpHandle->lOutputSize] = '\0';
  }

  return iReadSize;
}

/**=============================================================================
    函数名: InitBossHttpConnection()
      描述: 初始化http连接，获得句柄
      参数: 无
      输入: 无
      输出: 无
    返回值:
            BOSS_HTTP_HANDLE句柄指针:成功  NULL:失败
==============================================================================*/
BOSS_HTTP_HANDLE* InitBossHttpConnection()
{
  BOSS_HTTP_HANDLE* pstHttpHandle;

  /*malloc memory*/
  pstHttpHandle = (BOSS_HTTP_HANDLE*)malloc(sizeof(BOSS_HTTP_HANDLE));
  if(pstHttpHandle==NULL)
    return NULL;

  /*init the curl session*/
  pstHttpHandle->curlHandle = curl_easy_init();
  if(pstHttpHandle->curlHandle==NULL)
  {
    free(pstHttpHandle);
    return NULL;
  }

  /*init fields of handle*/
  pstHttpHandle->lTimeOut = 0;
  memset(pstHttpHandle->sUserName,0,sizeof(pstHttpHandle->sUserName));
  memset(pstHttpHandle->sPassword,0,sizeof(pstHttpHandle->sPassword));
  pstHttpHandle->psInput = NULL;
  pstHttpHandle->psOutput = NULL;
  pstHttpHandle->lPutPos = 0;
  pstHttpHandle->lInputSize = 0;
  pstHttpHandle->lOutputSize = 0;
  pstHttpHandle->sErrMsg[0] = '\0';

  return pstHttpHandle;
}

/**=============================================================================
    函数名: SetBossHttpTimeout()
      描述: 设置连接超时秒数
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要设置超时的句柄指针
            long lSecond:连接超时的秒数，为0时使用系统默认值
      输入: 无
      输出: 无
    返回值: 无
==============================================================================*/
void SetBossHttpTimeout(BOSS_HTTP_HANDLE* httpHandle,long lSecond)
{
  if(httpHandle == NULL)
    return;
  httpHandle->lTimeOut = lSecond;
  return;
}

/**=============================================================================
    函数名: SetBossHttpUserPwd()
      描述: 设置连接用户名密码
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要设置超时的句柄指针
            long lSecond:连接超时的秒数，为0时使用系统默认值
      输入: 无
      输出: 无
    返回值: 无
==============================================================================*/
void SetBossHttpUserPwd(BOSS_HTTP_HANDLE* httpHandle,char *psUser,char *psPwd)
{
  if(httpHandle == NULL)
    return;
  strcpy(httpHandle->sUserName,psUser);
  strcpy(httpHandle->sPassword,psPwd);
  return;
}

/**=============================================================================
    函数名: OpenBossHttpConnectionPUT()
      描述: 打开http连接，PUT发送和接收数据
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要使用http连接的句柄指针
            char *psUrl:要连接的http server的URL
            char* psInputStr:要发送的数据字符串指针
      输入: 无
      输出: 无
    返回值:
            not NULL:返回接收到的数据  NULL:失败，可使用GetBossHttpErrMsg()获取错误
==============================================================================*/
char* OpenBossHttpConnectionPUT(BOSS_HTTP_HANDLE* httpHandle,char *psUrl,char* psInputStr)
{
  int iRet;
  char sUserPwd[256];

  if(httpHandle == NULL)
    return NULL;

  /*get parameters*/
  if(psUrl==NULL||psInputStr==NULL)
  {
    strcpy(httpHandle->sErrMsg,sBossHttpErrMsg_BossHttp[2]);
    return NULL;
  }
  httpHandle->psInput = psInputStr;
  if(httpHandle->psOutput!=NULL)
    free(httpHandle->psOutput);
  httpHandle->psOutput = NULL;
  httpHandle->lPutPos = 0;
  httpHandle->lInputSize = strlen(psInputStr);
  httpHandle->lOutputSize = 0;

  /*specify URL to get*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_URL, psUrl);

  /*set the send data callback function*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_READFUNCTION,PutDataCallback_BossHttp);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_READDATA,(void *)httpHandle);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_UPLOAD,1) ;
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_PUT,1);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_INFILESIZE,httpHandle->lInputSize);

  /*set the receive data callback function*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_WRITEFUNCTION,GetDataCallback_BossHttp);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_WRITEDATA,(void *)httpHandle);

  /*set connection timeout*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_CONNECTTIMEOUT,httpHandle->lTimeOut);

  /*set connection username and password*/
  sprintf(sUserPwd,"%s:%s",httpHandle->sUserName,httpHandle->sPassword);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_USERPWD,sUserPwd);

  /*get error message buffer*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_ERRORBUFFER,httpHandle->sErrMsg);

  /*perform real http connection*/
  iRet = curl_easy_perform(httpHandle->curlHandle);

  if(iRet!=0)
  {
    return NULL;
  }
  else
  {
    strcpy(httpHandle->sErrMsg,sBossHttpErrMsg_BossHttp[0]);
    return httpHandle->psOutput;
  }
}

/**=============================================================================
    函数名: OpenBossHttpConnectionPOST()
      描述: 打开http连接，POST方式发送和接收数据
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要使用http连接的句柄指针
            char *psUrl:要连接的http server的URL
            char* psInputStr:要发送的数据字符串指针
      输入: 无
      输出: 无
    返回值:
            not NULL:返回接收到的数据  NULL:失败，可使用GetBossHttpErrMsg()获取错误
==============================================================================*/
char* OpenBossHttpConnectionPOST(BOSS_HTTP_HANDLE* httpHandle,char *psUrl,char* psInputStr)
{
  int iRet;
  char sUserPwd[256];

  if(httpHandle == NULL)
    return NULL;

  /*get parameters*/
  if(psUrl==NULL||psInputStr==NULL)
  {
    strcpy(httpHandle->sErrMsg,sBossHttpErrMsg_BossHttp[2]);
    return NULL;
  }
  httpHandle->psInput = psInputStr;
  if(httpHandle->psOutput!=NULL)
    free(httpHandle->psOutput);
  httpHandle->psOutput = NULL;
  httpHandle->lPutPos = 0;
  httpHandle->lInputSize = strlen(psInputStr);
  httpHandle->lOutputSize = 0;

  /*specify URL to get*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_URL, psUrl);

  /*set the send data callback function*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_READFUNCTION,PutDataCallback_BossHttp);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_READDATA,(void *)httpHandle);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_INFILESIZE,httpHandle->lInputSize);

  /*set the receive data callback function*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_WRITEFUNCTION,GetDataCallback_BossHttp);
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_WRITEDATA,(void *)httpHandle);

  /*set connection timeout*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_CONNECTTIMEOUT,httpHandle->lTimeOut);

  /*set connection username and password*/
  if(strlen(httpHandle->sUserName) > 1 && strlen(httpHandle->sPassword) > 1)
  {
    sprintf(sUserPwd,"%s:%s",httpHandle->sUserName,httpHandle->sPassword);
    curl_easy_setopt(httpHandle->curlHandle,CURLOPT_USERPWD,sUserPwd);
  }

  /*get error message buffer*/
  curl_easy_setopt(httpHandle->curlHandle,CURLOPT_ERRORBUFFER,httpHandle->sErrMsg);

  /* Now specify the POST data */
  curl_easy_setopt(httpHandle->curlHandle, CURLOPT_POSTFIELDS, psInputStr);

  /*perform real http connection*/
  iRet = curl_easy_perform(httpHandle->curlHandle);

  if(iRet!=0)
  {
    return NULL;
  }
  else
  {
    strcpy(httpHandle->sErrMsg,sBossHttpErrMsg_BossHttp[0]);
    return httpHandle->psOutput;
  }
}

/**=============================================================================
    函数名: CloseBossHttpConnection()
      描述: 关闭http连接
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要获取错误信息的句柄
      输入: 无
      输出: 无
    返回值: 无
            上一次函数调用的错误信息
==============================================================================*/
void CloseBossHttpConnection(BOSS_HTTP_HANDLE* httpHandle)
{
  if(httpHandle == NULL)
    return;

  /*cleanup curl session*/
  curl_easy_cleanup(httpHandle->curlHandle);

  /*free memory*/
  if(httpHandle->psOutput != NULL)
    free(httpHandle->psOutput);
  free(httpHandle);
}

/**=============================================================================
    函数名: GetBossHttpErrMsg()
      描述: 获取错误信息
      参数:
            BOSS_HTTP_HANDLE* httpHandle:要获取错误信息的句柄
      输入: 无
      输出: 无
    返回值:
            上一次函数调用的错误信息
==============================================================================*/
char* GetBossHttpErrMsg(BOSS_HTTP_HANDLE* httpHandle)
{
  if(httpHandle == NULL)
    return NULL;
  return httpHandle->sErrMsg;
}

/**=============================================================================
    函数名: connectAndSend()
      描述: 打开http连接，发送和接收数据
      参数:
      输入: psUrl:http server的URL
            logflag:登录标志，1登录，则输入用户和密码userName/passwd
            sInputBuf:输入参数

      输出: 返回串
    返回值:
            not NULL:返回接收到的数据  NULL:失败，可使用GetBossHttpErrMsg()获取错误
==============================================================================*/
int connectAndSend(char* psUrl,int logflag,char* userName, char* passwd,char* sInputBuf,char *soutBuf)
{
	BOSS_HTTP_HANDLE *pstHttpHandle;
	char sRetCode[5]="0000";    /*返回码*/
	char sOutputBuf[BUFFER_LEN+1024]="";
	char sLogBuf[BUFFER_LEN*2];
	char *psOutput;

	printf("开始调用http\n");
	psOutput = &(sOutputBuf[0]);

	/*get input param*/
	if(sInputBuf[0]=='\0')
	{
		sprintf(sLogBuf,"error to get paramter <Input>!");
		printf(sLogBuf);
		return -1;
	}

	/*init boss http connection*/
	if((pstHttpHandle=InitBossHttpConnection())==NULL)
	{
		sprintf(sLogBuf,"error to init boss http connection!");
		printf(sLogBuf);
		return -2;
	}
	printf("connection success.pstHttpHandle=[%ld]\n",pstHttpHandle);

	/*set connection timeout*/
	SetBossHttpTimeout(pstHttpHandle,HTTP_TIMEOUT);
	
	printf("psUrl=[%s]\n",psUrl);
	if(logflag==1)
    	SetBossHttpUserPwd(pstHttpHandle,userName,passwd);

	/*perform http connection*/
	psOutput = OpenBossHttpConnectionPOST(pstHttpHandle,psUrl,sInputBuf);
	if(psOutput == NULL)
	{
		psOutput = GetBossHttpErrMsg(pstHttpHandle);
		sprintf(sLogBuf,"error to open boss http connection, message:%s.\n", psOutput);
		printf(sLogBuf);
		CloseBossHttpConnection(pstHttpHandle);
		strcpy(soutBuf,psOutput);
		return -3;
	}

	/*operate get string*/;
	strcpy(sOutputBuf,psOutput);
	printf(sOutputBuf);
	strcpy(soutBuf,psOutput);

	/*close boss http connection*/
	CloseBossHttpConnection(pstHttpHandle);
	return 0;
}
