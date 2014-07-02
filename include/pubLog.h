#ifndef _PUBLOG_H_
#define _PUBLOG_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
 
/*--- 日志类别编码 ---*/
#define DEBUG_LOG  1000	/* 调试日志 */
#define DBDBG_LOG  1001 /* 数据库调试日志 */
#define APPRUN_LOG 2000	/* 运行日志 */
#define APPERR_LOG 3000 /* 应用程序错误 */
#define OSERR_LOG  3001	/* 操作系统错误 */
#define DBERR_LOG  3002	/* 数据库错误 */
#define ASERR_LOG  3003	/* 应用服务器错误 */


/*使用Oracle SQLCA*/
#define _USE_SQLCA_

/*日志类别名称长度*/
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
/*FILE,FUNCTION,LINE的缩写*/
#define _FFL_ __FILE__,__FUNCTION__,__LINE__

#ifdef _USE_SQLCA_
#define PUBLOG_DBDEBUG(svcName)             pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DBDBG_LOG,"",SQLERRMSG)


#endif
/***************
变长参数的宏定义在PROC编译器中不识别，预编译过程不好用，改为使用pubLogUtil注入C代码实现
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
	char    vBegin_Time[17+1]; 	/*开始时间,格式YYYYMMDDHH24MISS */
	double  vUsed_Time;  	 	/*受理耗时,要求精确到小数点后两位,单位秒*/
	long    vLogion_Accept; 	/*操作流水*/
	char    vLogin_No[6+1]; 	/*操作工号*/
	char    vOp_Code[5+1]; 		/*操作代码*/
	char    vServ_Name[20]; 	/*服务名称*/
	char    vPhone_No[15];		/*用户号码*/
	long    vId_No; 		/*用户id_no*/
	long    vOther_No; 		/*其它号码*/
	char    vRetCode[6+1]; 		/*受理结果*/
	char    vRetMsg[6+1]; 		/*受理结果*/
	int     vFlag;			/*写入日志标志*/
}BOMC_LOG;

#define WRITE_BOMC_LOG 0x0100

/*记录网管日志函数*/
int WriteBomcLog(BOMC_LOG *logInfo);
/*设置网管日志写入标志*/
int SetBomcLogFlag(BOMC_LOG *logInfo);
/*清除网管日志写入标志*/
int ClearBomcLogFlag(BOMC_LOG *logInfo);

#endif
