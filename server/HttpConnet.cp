/*
 *	�ļ�����boss_http.c
 *
 *	��Ȩ���� (C) 2002	��������(�й�)���޹�˾
 *
 *	������ʹ��libcurl����http���ӣ���һ��bossͨѶģ��
 *        ʹ�ñ�ģ��ǰ�����Ȱ�װlibcurl��
 *
 *	�汾��1.0
 *
 *	�޶���¼��
 *	�޶�����		�޶���		�޶�����
 *  20080108        ����ѫ      �ṩhttp �ͻ���
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
{ /*������Ϣ*/
/*����0 */  "Call successfully!",
/*����1 */  "Error to malloc memory!",
/*����2 */  "NULL parameter!",
/*����3 */  "Unknown error!"
};

/**=============================================================================
    ������: PutDataCallback_BossHttp()
      ����: �ϴ����ݵ�http server�Ļص�����
      ����:
            void *ptr:���Ҫ�ϴ�������ͷָ��
            size_t size:Ҫ�ϴ�ÿ�����ݿ�Ĵ�С
            size_t nmemb:Ҫ�ϴ����ݿ���
            void *data:���ݵ�BOSS_HTTP_HANDLE
      ����: ��
      ���: ��
    ����ֵ:
            ʵ��Ҫ�ϴ����ֽ���
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
    ������: GetDataCallback_BossHttp()
      ����: �´����ݵ�client�Ļص�����
      ����:
            void *ptr:���Ҫ�´�������ͷָ��
            size_t size:Ҫ�´�ÿ�����ݿ�Ĵ�С
            size_t nmemb:Ҫ�´����ݿ���
            void *stream:���ݵ�BOSS_HTTP_HANDLE
      ����: ��
      ���: ��
    ����ֵ:
            ʵ��Ҫ�´����ֽ���
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
    ������: InitBossHttpConnection()
      ����: ��ʼ��http���ӣ���þ��
      ����: ��
      ����: ��
      ���: ��
    ����ֵ:
            BOSS_HTTP_HANDLE���ָ��:�ɹ�  NULL:ʧ��
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
    ������: SetBossHttpTimeout()
      ����: �������ӳ�ʱ����
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫ���ó�ʱ�ľ��ָ��
            long lSecond:���ӳ�ʱ��������Ϊ0ʱʹ��ϵͳĬ��ֵ
      ����: ��
      ���: ��
    ����ֵ: ��
==============================================================================*/
void SetBossHttpTimeout(BOSS_HTTP_HANDLE* httpHandle,long lSecond)
{
  if(httpHandle == NULL)
    return;
  httpHandle->lTimeOut = lSecond;
  return;
}

/**=============================================================================
    ������: SetBossHttpUserPwd()
      ����: ���������û�������
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫ���ó�ʱ�ľ��ָ��
            long lSecond:���ӳ�ʱ��������Ϊ0ʱʹ��ϵͳĬ��ֵ
      ����: ��
      ���: ��
    ����ֵ: ��
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
    ������: OpenBossHttpConnectionPUT()
      ����: ��http���ӣ�PUT���ͺͽ�������
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫʹ��http���ӵľ��ָ��
            char *psUrl:Ҫ���ӵ�http server��URL
            char* psInputStr:Ҫ���͵������ַ���ָ��
      ����: ��
      ���: ��
    ����ֵ:
            not NULL:���ؽ��յ�������  NULL:ʧ�ܣ���ʹ��GetBossHttpErrMsg()��ȡ����
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
    ������: OpenBossHttpConnectionPOST()
      ����: ��http���ӣ�POST��ʽ���ͺͽ�������
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫʹ��http���ӵľ��ָ��
            char *psUrl:Ҫ���ӵ�http server��URL
            char* psInputStr:Ҫ���͵������ַ���ָ��
      ����: ��
      ���: ��
    ����ֵ:
            not NULL:���ؽ��յ�������  NULL:ʧ�ܣ���ʹ��GetBossHttpErrMsg()��ȡ����
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
    ������: CloseBossHttpConnection()
      ����: �ر�http����
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫ��ȡ������Ϣ�ľ��
      ����: ��
      ���: ��
    ����ֵ: ��
            ��һ�κ������õĴ�����Ϣ
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
    ������: GetBossHttpErrMsg()
      ����: ��ȡ������Ϣ
      ����:
            BOSS_HTTP_HANDLE* httpHandle:Ҫ��ȡ������Ϣ�ľ��
      ����: ��
      ���: ��
    ����ֵ:
            ��һ�κ������õĴ�����Ϣ
==============================================================================*/
char* GetBossHttpErrMsg(BOSS_HTTP_HANDLE* httpHandle)
{
  if(httpHandle == NULL)
    return NULL;
  return httpHandle->sErrMsg;
}

/**=============================================================================
    ������: connectAndSend()
      ����: ��http���ӣ����ͺͽ�������
      ����:
      ����: psUrl:http server��URL
            logflag:��¼��־��1��¼���������û�������userName/passwd
            sInputBuf:�������

      ���: ���ش�
    ����ֵ:
            not NULL:���ؽ��յ�������  NULL:ʧ�ܣ���ʹ��GetBossHttpErrMsg()��ȡ����
==============================================================================*/
int connectAndSend(char* psUrl,int logflag,char* userName, char* passwd,char* sInputBuf,char *soutBuf)
{
	BOSS_HTTP_HANDLE *pstHttpHandle;
	char sRetCode[5]="0000";    /*������*/
	char sOutputBuf[BUFFER_LEN+1024]="";
	char sLogBuf[BUFFER_LEN*2];
	char *psOutput;

	printf("��ʼ����http\n");
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
