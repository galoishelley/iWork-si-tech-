#include "pubLog.h"



#undef _DEBUG_

#ifdef _USE_SQLCA_
EXEC SQL INCLUDE SQLCA;
#define SQLCODE  sqlca.sqlcode
#define SQLERRMSG sqlca.sqlerrm.sqlerrmc
#endif

int pubLog_Debug(const char * file,const char * function,int line,const char * svcName,char *errNo,const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = DEBUG_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog_AppRun(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = APPRUN_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog_AppErr(const char * file,const char * function,int line,const char * svcName,char * errNo, const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = APPERR_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog_OSErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = OSERR_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog_DBErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = DBERR_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog_ASErr(const char * file,const char * function,int line,const char * svcName,char * errNo,const char *logMsgFmt,...)
{
	va_list ap;
	int ret = -1;
	int logCode = ASERR_LOG;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}

int pubLog(const char * svcName,const char * file,const char * function,int line,int logCode,char *errNo,const char *logMsgFmt,...)
/**
** svcName 服务名
** codePos 代码位置
** logCode 日志类别编码
** errNo 错误代码，调试日志和运行日志此值无意义
** logMsgFmt 日志信息
** ...
** 兼容1.0版
**/
{
	va_list ap;
	int ret = -1;

	va_start(ap,logMsgFmt);

	ret = vpubLog(svcName,file,function,line,logCode,errNo,logMsgFmt,ap);

	va_end(ap);

	return ret;
}


int vpubLog(const char * svcName,const char * file,const char * function,int line,int logCode,char *errNo,const char *logMsgFmt,va_list ap)
/**
** svcName 服务名
** codePos 代码位置
** logCode 日志类别编码
** errNo 错误代码，调试日志和运行日志此值无意义
** logMsgFmt 日志信息
** ...
**/
{
	EXEC SQL BEGIN DECLARE SECTION;
		char pubLogFlag[1+1];	/*统一日志开关 N 不打印 Y 打印*/
		unsigned int debugBit = 255;  /*打印的屏蔽位*/
		char svc_Name[50+1];	/*函数或者service名*/
	EXEC SQL END DECLARE SECTION;
	struct timeval tv;
	struct tm *l_now;
	char strNow[40];
	char chLogLevel[LOG_LEVEL_LEN+1] = "";
	char outputFormat[200] = "";
	char *buffer;
	long buf_len = 0;
	int ret=0;
	char tmp_errNo[500];
	int tmpSqlCode;
	char tmpSqlErrMsg[70 + 1];
	const char * pLogMsgFmtStr;
	char *ENV_pt = NULL;
	int doOutPut = 0;

	memset(svc_Name,0,sizeof(svc_Name));
	memset(pubLogFlag,0,sizeof(pubLogFlag));
	memset(tmpSqlErrMsg,0,sizeof(tmpSqlErrMsg));
	strcpy(svc_Name,function);
	strcpy(pubLogFlag,"Y");
	
	/*输出格式指针*/
	pLogMsgFmtStr = logMsgFmt;

	/*时间处理*/
	gettimeofday(&tv,NULL);
	l_now=localtime(&tv.tv_sec);
	
#ifdef _USE_SQLCA_
	/*保存SQLCODE SQLERRMSG现场*/
	tmpSqlCode = SQLCODE;
	strncpy(tmpSqlErrMsg,SQLERRMSG,70);
#endif
	
#ifdef _USE_SQLCA_
	/*DBDEBUG类型的日志对SQLMSG特殊处理*/
	if(1001 == logCode)
	{
		sprintf(tmp_errNo,"%d",SQLCODE);
		if(SQLCODE == 0) /*解决SQLCODE==0时，不处理SQLERRMSG问题*/
		{
			strcpy(tmpSqlErrMsg,"SQL执行成功");
		}
		pLogMsgFmtStr = tmpSqlErrMsg;
	}
#endif
	if(1001 != logCode) /*输出格式准备*/
	{
		strcpy(tmp_errNo,errNo);
	}
	strcpy(outputFormat,"%s|%d|%s|%s|%s@%s@%d|%d|%s|%s\n");
	
#ifdef _USE_SQLCA_
	/*取程序打印日志开关*/
	EXEC SQL SELECT PUB_LOG,OTHER_FLAG
				INTO :pubLogFlag,:debugBit
				FROM cSetLogFlag
				WHERE SVC_NAME=:svc_Name;
	if(SQLCODE == 0)
	{
		if(pubLogFlag[0] != 'Y' && pubLogFlag[0] != 'y')
		{
			debugBit = 0; /*总开关关闭所有均不输出*/
		}
	}
	else
	{
#endif
		/*环境变量处理*/
		if ( (ENV_pt = getenv("PUBLOG_BIT")) != NULL)
		{
			debugBit = atoi(ENV_pt);
		}
#ifdef _USE_SQLCA_
	}
#endif
	
#ifdef _DEBUG_
	printf("DEBUG BIT = %d\n",debugBit);
#endif

	switch(logCode)
	{
		case 1000:
			if ( (debugBit & 1) == 1)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"DEBUG_LOG");
			break;
		case 1001:
			if( (debugBit & 2) == 2)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"DBDBG_LOG");
			break;
		case 2000:
			if ( (debugBit & 4) == 4)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"APPRUN_LOG");
			break;
		case 3000:
			if ( (debugBit & 8) == 8)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"APPERR_LOG");
			break;
		case 3001:
			if ( (debugBit & 16) == 16)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"OSERR_LOG");
			break;
		case 3002:
			if ( (debugBit & 32) == 32)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"DBERR_LOG");
			break;
		case 3003:
			if ( (debugBit & 64) == 64)
			{
				doOutPut = 1;
			}
			strcpy(chLogLevel,"ASERR_LOG");
			break;
		default:
			strcpy(chLogLevel,"UNKNOWN");
			break;
	}
	
#ifdef _USE_SQLCA_
	/*恢复SQLCODE SQLERRMSG现场*/
	SQLCODE = tmpSqlCode;
	strncpy(SQLERRMSG,tmpSqlErrMsg,70);
#endif

	if ( !doOutPut )
	{
		return 0;
	}

	sprintf(strNow,"%d%02d%02d %02d:%02d:%02d.%03ld",
		l_now->tm_year+1900, /*年份*/
		l_now->tm_mon+1,     /*月份*/
		l_now->tm_mday,      /*日期*/
		l_now->tm_hour,      /*小时*/
		l_now->tm_min,       /*分钟*/
		l_now->tm_sec,       /*秒*/
		tv.tv_usec/1000      /*毫秒*/
	);
#ifdef _DEBUG_
	printf("strNow = %s\n",strNow);
	printf("logFmtStr = %s\n",pLogMsgFmtStr);
#endif

	buf_len = strlen(svcName) + strlen(file) + strlen(function) + strlen(pLogMsgFmtStr);
	buffer = (char*)malloc(buf_len+500);

	sprintf(buffer,outputFormat,chLogLevel,getpid(),svcName,strNow,file,function,line,logCode,tmp_errNo,pLogMsgFmtStr);

#ifdef _DEBUG_
	printf("code=%s,FMT=",errNo);
	puts(buffer);
	printf("\n");
#endif
	ret = vprintf(buffer,ap);
	free(buffer);

	return ret;
}

/*记录网管日志*/
int WriteBomcLog(BOMC_LOG *logInfo)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char 	BomcFlag[1+1];		/*网管日志开关 N 不打印 Y 打印*/
		char	svc_Name[50+1];
	EXEC SQL END DECLARE SECTION;
	char *BOMC_LOG_PATH_pt = NULL;
	char DefaultLogPath[] = ".";
	char bomc_log_file[200];
	int tMins=0;
	char minsNum[2+1];
	time_t today_timer;
	struct tm *tblock;
	FILE *fpOut;

	memset(BomcFlag,0,sizeof(BomcFlag));
	memset(svc_Name,0,sizeof(svc_Name));
	memset(minsNum,0,sizeof(minsNum));
	strcpy(svc_Name,logInfo->vServ_Name);
	strcpy(BomcFlag,"Y");

	Trim(svc_Name);

	/*取程序打印日志开关*/
	EXEC SQL 	SELECT BOMC_LOG
				INTO :BomcFlag
				FROM cSetLogFlag
				WHERE SVC_NAME=:svc_Name;
	if(SQLCODE!=0 && SQLCODE!=1403)
	{
		printf("读取日志标志失败[%d][%s]\n",SQLCODE,svc_Name);
		return -1;
	}

	today_timer = time(NULL);
	tblock = localtime(&today_timer);
	tMins=tblock->tm_min;
	if(tMins < 30)
	{
		strcpy(minsNum,"00");
	}
	else
	{
		strcpy(minsNum,"30");
	}

	memset(bomc_log_file,0,sizeof(bomc_log_file));

	if ( (BOMC_LOG_PATH_pt = getenv("BOMCLOGPATH")) != NULL)
	{
		sprintf(bomc_log_file,"%s/nmlog.%04d%02d%02d%02d%2s",BOMC_LOG_PATH_pt,tblock->tm_year+1900,tblock->tm_mon+1,tblock->tm_mday,tblock->tm_hour,minsNum);
	}
	else
	{
		sprintf(bomc_log_file,"%s/nmlog.%04d%02d%02d%02d%2s",DefaultLogPath,tblock->tm_year+1900,tblock->tm_mon+1,tblock->tm_mday,tblock->tm_hour,minsNum);
	}

	if(logInfo->vFlag == WRITE_BOMC_LOG && BomcFlag[0]=='Y')
	{
#ifdef _DEBUG_
		printf("WRITE_BOMC_LOG=[%d]|网管日志路径[%s]\n",logInfo->vFlag,bomc_log_file);
#endif
		if( (fpOut = fopen(bomc_log_file,"a+"))== NULL)
		{
			printf("打开网管日志文件失败\n");
			return -1;
		}
		fprintf(fpOut,"%s|%0.3f|%ld|%s|%s|%s|%s|%ld|%ld|%s\n",
			logInfo->vBegin_Time,logInfo->vUsed_Time,logInfo->vLogion_Accept,
			logInfo->vLogin_No,logInfo->vOp_Code,logInfo->vServ_Name,
			logInfo->vPhone_No,logInfo->vId_No,logInfo->vOther_No,
			logInfo->vRetCode);
		fclose(fpOut);
	}
	return 0;
}

int SetBomcLogFlag(BOMC_LOG *logInfo)
{
	logInfo->vFlag = WRITE_BOMC_LOG;
	return 0;
}

int ClearBomcLogFlag(BOMC_LOG *logInfo)
{
	logInfo->vFlag = 0;
	return 0;
}

/* ******************
 * 获取当前时间
 * @author 	 zhangzhe
 * created   2009-04-20
 * begin_time	格式: YYYYMMDDHHMISS
 * nowSec		格式：1970年1月1日0时0分0 秒算起至今的UTC时间所经过的毫秒数
 * *****************/
void getBeginTime(char *begin_time,double *nowMsec)
{
	struct 		timeval run_start;
	struct 		tm *start_time;

	memset(begin_time,0,sizeof(begin_time));
	*nowMsec=0;

	gettimeofday(&run_start,NULL);
	start_time = localtime(&run_start.tv_sec);

	*nowMsec=run_start.tv_sec+run_start.tv_usec/1000000.0;

	sprintf(begin_time,	"%4d%02d%02d%02d%02d%02d",
					start_time->tm_year+1900, /*年份*/
					start_time->tm_mon+1,     /*月份*/
					start_time->tm_mday,      /*日期*/
					start_time->tm_hour,      /*小时*/
					start_time->tm_min,       /*分钟*/
					start_time->tm_sec       /*秒*/
			);
}


/* ******************
 * 获取输入时间与当前时间的差
 * @author 	 zhangzhe
 * created   2009-04-20
 * 输入参数		nowSec	单位：秒
 * 输出参数		getTimeDif() 当前时间与nowSec相差的秒数
 * *****************/
void getTimeDif(double *timedif,double oldUsec)
{
	double		nowUsec=0;
	struct 		timeval now;
	struct 		tm *start_time;
	/*获取当前时间*/
	gettimeofday(&now,NULL);

	nowUsec=now.tv_sec+now.tv_usec/1000000.0;
	/*获取时间差*/
	*timedif=nowUsec-oldUsec;

}
