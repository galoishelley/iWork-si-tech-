/*
 *	文件名：boss_http.h
 *
 *	版权所有 (C) 2002	神州数码(中国)有限公司
 *
 *	描述：使用libcurl进行http连接，与一级boss通讯模块头文件
 *        使用本模块前必须先安装libcurl库
 *
 *	版本：1.0
 *
 *	修订记录：
 *	修订日期		修订人		修订内容
 *  20080108        王建勋      提供http 客户端
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

/*==============================系统参数定义区================================*/

/*================================结构定义区==================================*/
typedef struct defBOSS_HTTP_HANDLE
{
#ifndef ORA_PROC
  CURL* curlHandle;   /*curl库连接句柄*/
#endif 
  long lTimeOut;      /*连接超时值*/
  char sUserName[128];/*连接的用户名*/
  char sPassword[128];/*连接的密码*/
  char* psInput;      /*要上传的字符串*/
  char* psOutput;     /*获取的结果字符串*/
  size_t lPutPos;     /*上传回调时的当前位置*/
  size_t lInputSize;  /*要上传的字符串长度*/
  size_t lOutputSize; /*下传的字符串长度*/
  char sErrMsg[CURL_ERROR_SIZE+1];  /*错误消息*/
} BOSS_HTTP_HANDLE;

/*================================函数定义区==================================*/
BOSS_HTTP_HANDLE* InitBossHttpConnection();   /*初始化http连接，获得句柄*/
void SetBossHttpTimeout(BOSS_HTTP_HANDLE* httpHandle,long lSecond); /*设置连接超时秒数*/
void SetBossHttpUserPwd(BOSS_HTTP_HANDLE* httpHandle,char *psUser,char *psPwd); /*设置连接用户名密码*/
char* OpenBossHttpConnection(BOSS_HTTP_HANDLE* httpHandle,char *psUrl,char* psInputStr);    /*打开http连接，发送和接收数据*/
void CloseBossHttpConnection(BOSS_HTTP_HANDLE* httpHandle);  /*关闭http连接*/
char* GetBossHttpErrMsg(BOSS_HTTP_HANDLE* httpHandle);  /*获取错误信息*/
#endif
