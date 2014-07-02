#ifndef _PUBLOG_H_
#define _PUBLOG_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
 
/*--- ��־������ ---*/
#define DEBUG_LOG  1000	/* ������־ */
#define DBDBG_LOG  1001 /* ���ݿ������־ */
#define APPRUN_LOG 2000	/* ������־ */
#define APPERR_LOG 3000 /* Ӧ�ó������ */
#define OSERR_LOG  3001	/* ����ϵͳ���� */
#define DBERR_LOG  3002	/* ���ݿ���� */
#define ASERR_LOG  3003	/* Ӧ�÷��������� */


/*ʹ��Oracle SQLCA*/
#define _USE_SQLCA_

/*��־������Ƴ���*/
#define LOG_LEVEL_LEN 9
/*
#ifdef _USE_SQLCA_

	#include "sqlca.h"
	
	#ifndef SQLCODE
		#define SQLCODE sqlca.sqlcode
	#endif

	#ifndef SQLERRMSG
		#define SQLERRMSG sqlca.sqlerrm.sqlerrmc
	#endif
	#else
	#ifndef SQLCODE
		#define SQLCODE 0
	#endif
#endif
*/
/*FILE,FUNCTION,LINE����д*/
#define _FFL_ __FILE__,__FUNCTION__,__LINE__

#ifdef _USE_SQLCA_
#define PUBLOG_DBDEBUG(svcName)             pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DBDBG_LOG,"",SQLERRMSG)


#endif
/***************
�䳤�����ĺ궨����PROC�������в�ʶ��Ԥ������̲����ã���Ϊʹ��pubLogUtilע��C����ʵ��
#define PUBLOG(svcName,logCode,errNo,logMsg) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,logCode,errNo,logMsg)
#define PUBLOG_DEBUG(svcName,errNo,logMsg)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DEBUG_LOG,errNo,logMsg)
#define PUBLOG_APPRUN(svcName,errNo,logMsg) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,APPRUN_LOG,errNo,logMsg)
#define PUBLOG_APPERR(svcName,errNo,logMsg) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,APPERR_LOG,errNo,logMsg)
#define PUBLOG_OSERR(svcName,errNo,logMsg) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,OSERR_LOG,errNo,logMsg)
#define PUBLOG_DBERR(svcName,errNo,logMsg)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DBERR_LOG,errNo,logMsg)
#define PUBLOG_ASERR(svcName,errNo,logMsg)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,ASERR_LOG,errNo,logMsg)
*****************/

int pubLog(const char * svcName,const char * file,const char * function,int line,int logCode,char * errNo,const char * logMsgFmt,...);
int pubLog_Debug(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);
int pubLog_AppRun(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);
int pubLog_AppErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);
int pubLog_OSErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);
int pubLog_DBErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);
int pubLog_ASErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...);


/*BOMC LOG*/
typedef struct t_BOMC_LOG
{
	char    vBegin_Time[17+1]; 	/*��ʼʱ��,��ʽYYYYMMDDHH24MISS */
	double  vUsed_Time;  	 	/*�����ʱ,Ҫ��ȷ��С�������λ,��λ��*/
	long    vLogion_Accept; 	/*������ˮ*/
	char    vLogin_No[6+1]; 	/*��������*/
	char    vOp_Code[5+1]; 		/*��������*/
	char    vServ_Name[20]; 	/*��������*/
	char    vPhone_No[15];		/*�û�����*/
	long    vId_No; 		/*�û�id_no*/
	long    vOther_No; 		/*��������*/
	char    vRetCode[6+1]; 		/*������*/
	char    vRetMsg[6+1]; 		/*������*/
	int     vFlag;			/*д����־��־*/
}BOMC_LOG;

#define WRITE_BOMC_LOG 0x0100

/*��¼������־����*/
int WriteBomcLog(BOMC_LOG *logInfo);
/*����������־д���־*/
int SetBomcLogFlag(BOMC_LOG *logInfo);
/*���������־д���־*/
int ClearBomcLogFlag(BOMC_LOG *logInfo);

#endif
