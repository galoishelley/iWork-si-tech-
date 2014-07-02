/*
 *	�ļ�����boss_http.h
 *
 *	��Ȩ���� (C) 2002	��������(�й�)���޹�˾
 *
 *	������ʹ��libcurl����http���ӣ���һ��bossͨѶģ��ͷ�ļ�
 *        ʹ�ñ�ģ��ǰ�����Ȱ�װlibcurl��
 *
 *	�汾��1.0
 *
 *	�޶���¼��
 *	�޶�����		�޶���		�޶�����
 *  20080108        ����ѫ      �ṩhttp �ͻ���
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#ifndef ORA_PROC
#include <curl.h>
#include <types.h>
#include <easy.h>
#endif

#ifndef _BOSS_HTTP_H_
#define _BOSS_HTTP_H_

/*==============================ϵͳ����������================================*/

/*================================�ṹ������==================================*/
typedef struct defBOSS_HTTP_HANDLE
{
#ifndef ORA_PROC
  CURL* curlHandle;   /*curl�����Ӿ��*/
#endif 
  long lTimeOut;      /*���ӳ�ʱֵ*/
  char sUserName[128];/*���ӵ��û���*/
  char sPassword[128];/*���ӵ�����*/
  char* psInput;      /*Ҫ�ϴ����ַ���*/
  char* psOutput;     /*��ȡ�Ľ���ַ���*/
  size_t lPutPos;     /*�ϴ��ص�ʱ�ĵ�ǰλ��*/
  size_t lInputSize;  /*Ҫ�ϴ����ַ�������*/
  size_t lOutputSize; /*�´����ַ�������*/
  char sErrMsg[CURL_ERROR_SIZE+1];  /*������Ϣ*/
} BOSS_HTTP_HANDLE;

/*================================����������==================================*/
BOSS_HTTP_HANDLE* InitBossHttpConnection();   /*��ʼ��http���ӣ���þ��*/
void SetBossHttpTimeout(BOSS_HTTP_HANDLE* httpHandle,long lSecond); /*�������ӳ�ʱ����*/
void SetBossHttpUserPwd(BOSS_HTTP_HANDLE* httpHandle,char *psUser,char *psPwd); /*���������û�������*/
char* OpenBossHttpConnection(BOSS_HTTP_HANDLE* httpHandle,char *psUrl,char* psInputStr);    /*��http���ӣ����ͺͽ�������*/
void CloseBossHttpConnection(BOSS_HTTP_HANDLE* httpHandle);  /*�ر�http����*/
char* GetBossHttpErrMsg(BOSS_HTTP_HANDLE* httpHandle);  /*��ȡ������Ϣ*/
#endif
