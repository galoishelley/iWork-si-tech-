/******************************************
 *      Prg:  publicErr.cp
 *     Edit:  guo.yj
 * Modi/Add:  zhaohao  2001.10.31
 *     Date:  2001.02.02
 ******************************************/
#include "boss_srv.h"
#include "publicsrv.h"
#include "sqlcpr.h"

#define WORKDIR "WORKDIR"

extern int errno;

EXEC SQL INCLUDE SQLCA;

EXEC SQL BEGIN DECLARE SECTION;
	char input_parms[MAX_PARMS_NUM][MAX_PARMS_LENGTH];
	char output_parms[MAX_PARMS_NUM][MAX_PARMS_LENGTH];
	char input_parms2[MAX_PARMS_NUM][MAX_PARMS_LENGTH*2];
EXEC SQL END DECLARE SECTION;

FBFR32 *MemoryAllocate32(int FIRSTOCCS32,int OutputParNum,char *SerName,
                         FBFR32 *transIN,FBFR32 **transOUT,char *loginlable)
{ 
	FLDLEN32 	len;
	char   	temp_buf[100];

	memset(temp_buf,0,sizeof(temp_buf));
	len=0;

	/*** 为输出分配FML缓冲区 ****/
	len=(FLDLEN32)(FIRSTOCCS32*OutputParNum*50);
	*transOUT=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(*transOUT==(FBFR32 *)NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf,"\n内存分配失败 服务名=[%-s],size[%-ld]",SerName,(long)len);
		/* 错误处理函数自动释放 transIN 和 transOUT */
		error_handler32(transIN,*transOUT,"200",temp_buf,loginlable,"0");
	}
	return *transOUT;
}

FBFR32 *spublicAlloMem32(char *connName, char *connType, FBFR32 *transIN32, FBFR32 *transOUT32, int TotalAllocate,char *SrvName)
{
	FLDLEN32   len;
	char       temp_buf[100];

	memset(temp_buf,0,sizeof(temp_buf));
	len=0;

	len=(FLDLEN32)(TotalAllocate);
	transOUT32=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(transOUT32==(FBFR32 *)NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "\n内存分配失败 服务名=[%-s],size[%-ld]",SrvName,(long)len);
		/* 错误处理函数自动释放 transIN 和 transOUT */
		error_handler32(transIN32,transOUT32,"200",temp_buf,connName,connType);
	}
	return transOUT32;
}

void get_input_parms32(FBFR32 *transIN, int *input_par_num, int *output_par_num,
                       char *dbconnName,char *connType)
{
	int i,j=0;
	int  occs=0,count=0;
	char temp[20];
	char temp_buf[100];
	char errno[10],errmsg[200];

	*input_par_num=0;
	*output_par_num=0;

	if(transIN == NULL)
	{
		strcpy(errno,"205");
		PUBLOG_APPERR(__func__,"ERR","输入缓冲区为空");
		sprintf(errmsg, "transIN buffer is NULL,you must allocate memory for transIN!");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}

	if(Fget32(transIN, SEND_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[SEND_PARMS_NUM32]不存在!");
		strcpy(errno,"101");
		strcpy(errmsg,"Fget32 input parms error:SEND_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*input_par_num=atoi(temp);
	}

	if(Fget32(transIN, RECP_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[RECP_PARMS_NUM32]不存在!");
		strcpy(errno,"102");
		strcpy(errmsg,"Fget32 input parms error:RECP_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*output_par_num=atoi(temp);
	}
	for(i=0;i<*input_par_num;i++)
	{
		occs=(int)Foccur32(transIN,GPARM32_0+i);
		if(occs>1)
		{
			for(j=0;j<occs;j++)
			{
				if(Fget32(transIN,GPARM32_0+i, j, input_parms[count], NULL) == -1)
				{
					PUBLOG_APPERR(__func__,"ERR","获取输入参数[%ld][%d]失败!",GPARM32_0+i,j);
					strcpy(errno,"103");
					sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
					error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
				}
				count++;
			}
		}
		else
		{
			if(Fget32(transIN,GPARM32_0+i, 0, input_parms[count], NULL) == -1)
			{
				PUBLOG_APPERR(__func__,"ERR","获取输入参数[%ld]失败!",GPARM32_0+i);
				strcpy(errno,"103");
				sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
				error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
			}
			count++;
		}
	}
}


void get_input_parms32_2(FBFR32 *transIN, int *input_par_num, int *output_par_num,
                         char *dbconnName,char *connType)
{
	int i,j=0;
	int  occs=0,count=0;
	char temp[20];
	char temp_buf[100];
	char errno[10],errmsg[200];
	int ret;
	
	*input_par_num=0;
	*output_par_num=0;

	if(transIN == NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","输入缓冲区为空");
		strcpy(errno,"205");
		sprintf(errmsg, "transIN buffer is NULL,you must allocate memory for transIN!");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	if(Fget32(transIN, SEND_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[SEND_PARMS_NUM32]不存在!");
		strcpy(errno,"101");
		strcpy(errmsg,"Fget32 input parms error:SEND_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*input_par_num=atoi(temp);
	}

	if(Fget32(transIN, RECP_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[RECP_PARMS_NUM32]不存在!");
		strcpy(errno,"102");
		strcpy(errmsg,"Fget32 input parms error:RECP_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,
		dbconnName,connType);
	}
	else
	{
		*output_par_num=atoi(temp);
	}
	for(i=0;i<*input_par_num;i++)
	{
/*
		occs=(int)Foccur32(transIN,GPARM32_0+i);
		if(occs>1)
		{
			for(j=0;j<occs;j++)
			{
				if(Fget32(transIN,GPARM32_0+i, j, input_parms[count], NULL) == -1)
				{
					strcpy(errno,"103");
					sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",GPARM32_0+i);
					error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
				}
			count++;
			}
		}
		else
		{
*/
		if(Fget32(transIN,GPARM32_0+i, 0, input_parms[count], NULL) == -1)
		{
			PUBLOG_APPERR(__func__,"WARN","获取输入参数[%ld]失败!",GPARM32_0+i);
			/***
			**strcpy(errno,"103");
			**sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
			**error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
			****/
		}
		count++;
/*
		}
*/
	}
}

int get_input_parms32_2k(FBFR32 *transIN, int *input_par_num, int *output_par_num,
                        char *dbconnName,char *connType)
{
	int i,j=0;
	int  occs=0,count=0;
	char temp[20];
	char temp_buf[100];
	char errno[10],errmsg[200];
	
	*input_par_num=0;
	*output_par_num=0;

	if(transIN == NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","输入缓冲区为空");
		strcpy(errno,"205");
		sprintf(errmsg, "transIN buffer is NULL,you must allocate memory for transIN!");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}

	if(Fget32(transIN, SEND_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[SEND_PARMS_NUM32]不存在!");
		strcpy(errno,"101");
		strcpy(errmsg,"Fget32 input parms error:SEND_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*input_par_num=atoi(temp);
	}

	if(Fget32(transIN, RECP_PARMS_NUM32, 0, temp, NULL) == -1)
	{
		PUBLOG_APPERR(__func__,"ERR","输入参数[RECP_PARMS_NUM32]不存在!");
		strcpy(errno,"102");
		strcpy(errmsg,"Fget32 input parms error:RECP_PARAMS_NUM32");
		error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*output_par_num=atoi(temp);
	}
	for(i=0;i<*input_par_num;i++)
	{
		occs=(int)Foccur32(transIN,GPARM32_0+i);
		if(occs>1)
		{
			for(j=0;j<occs;j++)
			{
				if(Fget32(transIN,GPARM32_0+i, j, input_parms2[count], NULL) == -1)
				{
					PUBLOG_APPERR(__func__,"ERR","获取输入参数[%ld][%d]失败!",GPARM32_0+i,j);
					strcpy(errno,"103");
					sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
					error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
				}
				printf("input_parms2[%d]=[%s]\n",count,input_parms2[count]);
				count++;
			}
		}
		else
		{
			if(Fget32(transIN,GPARM32_0+i, 0, input_parms2[count], NULL) == -1)
			{
				PUBLOG_APPERR(__func__,"ERR","获取输入参数[%ld]失败!",GPARM32_0+i);
				strcpy(errno,"103");
				sprintf(errmsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
				error_handler32(transIN,NULL,errno,errmsg,dbconnName,connType);
			}
			count++;
		}
	}
	return 0;
}

FBFR32 *add_value32(FBFR32  *transIN,     FBFR32 *transOUT,
                    long    realloc_size, char *service_name,
                    FLDID32 param_name,   char *param_value,
                    char    *dbconnName,char *connType)
{
	FBFR32* tmp_fbfr;
	char    temp_buf[100];
	char    errno[10],errmsg[200];
	long    len=0;
	int     err_flag=0;

	if(transOUT == NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","输出缓冲区为空");
		strcpy(errno,"205");
		sprintf(errmsg, "SERVICE %s: transOUT buffer is NULL,you must allocate memory for transOUT!",service_name);
		error_handler32(transIN,transOUT,errno,errmsg,dbconnName,connType);
	}

	if(Fadd32(transOUT,param_name,param_value,NULL) == -1)
	{
		/* Fadd时内存不够 */
		if( Ferror32 == FNOSPACE )
		{
			len=Fsizeof32(transOUT)+realloc_size;
			tmp_fbfr=(FBFR32 *)tprealloc((char *)transOUT,len);
			if(tmp_fbfr==(FBFR32 *)NULL)
			{
				PUBLOG_APPERR(__func__,"ERR","重新分配缓冲区失败，分配大小(%ld)",(long)len);
				strcpy(errno,"201");
				sprintf(errmsg, "SERVICE %s: alloc error in add_value(),size(%ld)", service_name,len);
				error_handler32(transIN,transOUT,errno,errmsg,dbconnName,connType);
			}
			transOUT=tmp_fbfr;
			/* 如果仍然发生错误则放弃 */
			if(Fadd32(transOUT,param_name,param_value,NULL) == -1)
			{
				PUBLOG_APPERR(__func__,"ERR","压入输出参数[%ld]=[%s]失败!",param_name,param_value);
				strcpy(errno,"202");
				sprintf(errmsg,"SERVICE %s: add_value() error!",service_name);
				error_handler32(transIN,transOUT,errno,errmsg,dbconnName,connType);
			}
		}
		else
		/* Fadd时发生的其他错误 */
		{
			PUBLOG_APPERR(__func__,"ERR","压入输出参数时出错");
			strcpy(errno,"203");
			sprintf(errmsg,"SERVICE %s: add_value() error!",service_name);
			error_handler32(transIN,transOUT,errno,errmsg,dbconnName,connType);
		}
	}
	/* Fadd执行成功 */
	else
	{
		Ferror32 = 0;
	}
	return transOUT;
}

void error_handler32(FBFR32 *transIN,FBFR32 *transOUT,
                char *errno,char *errmsg,char *dbconnName,char *connType)
{
	char msg[200];
	int  errcode;

	EXEC SQL BEGIN DECLARE SECTION;
		char *name;
	EXEC SQL END DECLARE SECTION;

	errcode=Ferror32;
	if (sqlca.sqlerrm.sqlerrml)
	{
		printf("ERROR HANDLER32::SQLCODE=[%d][%s]\n", SQLCODE, SQLERRMSG);
	}

	if (dbconnName[0] != 0 && connType[0] !=0)
	{
		name = strtok(dbconnName,",");
		EXEC SQL ROLLBACK WORK;
		if(strcmp(connType,"0") == 0)
		{
			PUBLOG_APPRUN(__func__,"CLOSE DB CONN",name);
			spublicDBClose(name);
		}
		do
		{
			name = strtok('\0',",");
			if(name)
			{
				EXEC SQL ROLLBACK WORK;
				if(strcmp(connType,"0") == 0)
				{
					PUBLOG_APPRUN(__func__,"CLOSE DB CONN",name);
					spublicDBClose(name);
				}
			}
		}while(name);
	}

	Fchg32(transOUT,SVC_ERR_NO32,0,errno,(FLDLEN32)0);
	Fchg32(transOUT,SVC_ERR_MSG32,0,errmsg,(FLDLEN32)0);
	tpreturn( TPSUCCESS, 0, (char *)transOUT, 0L, 0 );
}

void warning_handler32()
{
	if (sqlca.sqlwarn[1] == 'W')
	{
		userlog("** Data truncated.");
	}

	if (sqlca.sqlwarn[3] == 'W')
	{
		userlog("** Insufficient host variables to store results.");
	}
	return;
}

void LoginFailure32(FBFR32 *transIN,FBFR32 *transOUT,char *loginlable)
{
	PUBLOG_APPERR(__func__,"ERR","试图登录数据库失败");
	error_handler32(transIN,transOUT,"0","试图登录数据库失败",loginlable,"0");
}

void LoginInDB32(FBFR32 *transIN,FBFR32 *transOUT,char *loginlable)
{ 
	int ret=-1;
	ret=spublicDBLogin(transIN, transOUT, "", loginlable);
	switch(ret)
	{
		case 0:

			break;
		case -1:
#ifdef _DEBUG_
			userlog("\n连接数据库失败,没有初始化共享内存\n");
#endif
			LoginFailure32(transIN,transOUT,loginlable);
			break;
		case -2:
#ifdef _DEBUG_
			userlog("\n连接数据库失败\n");
#endif
			LoginFailure32(transIN,transOUT,loginlable);
			break;
		}
}

void ExitDB32(FBFR32 *transIN,FBFR32 *transOUT,char *loginlable)
{
	int ret=-1;
	PUBLOG_APPRUN(__func__,"CLOSE DB CONN",loginlable);
	ret=spublicDBClose(loginlable);
	if (ret==0)
	{
	}
	else
	{
		LoginFailure32(transIN,transOUT,loginlable);
#ifdef _DEBUG_
		userlog("\n断开短连接失败\n");
#endif
	}
}

void ExitDB32dyn(FBFR32 *transIN,FBFR32 *transOUT,char *loginlable)
{
	int ret=-1;
	PUBLOG_APPRUN(__func__,"CLOSE DB CONN",loginlable);
	ret=spublicDBClose(loginlable);
	if (ret==0)
	{
	}
	else
	{
#ifdef _DEBUG_
		userlog("\n断开短连接失败\n");
#endif
	}
}

void ExitTuxedo32(char* sErrNo, char * sErrMsg, FBFR32 *transOUT, FBFR32 *transIN,
 char* cDbLabel, char *cServiceName)
{
	if (cDbLabel != NULL) 
	{
		PUBLOG_APPRUN(__func__,"CLOSE DB CONN",cDbLabel);
		if (!spublicDBClose(cDbLabel))
		{

		}
		else
		{
#ifdef _DEBUG_
			userlog("\n断开短连接失败\n");
#endif
		}
	}
	Fchg32(transOUT,SVC_ERR_NO32,0,sErrNo,(FLDLEN32)0);
	Fchg32(transOUT,SVC_ERR_MSG32,0,sErrMsg,(FLDLEN32)0);
	/*** Ffree32(transIN); ***/
	tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0);
}


/**FML16相关API定义*/
#ifdef _FML16_
FBFR *MemoryAllocate(int FirstOccs,int OutputParNum,const char *SerName,FBFR *transIN,FBFR **transOUT,char *loginlable)
{
	FLDLEN  len;
	char    temp_buf[100];
	
	memset(temp_buf,0,sizeof(temp_buf));
	len=0;

	/*** 为输出分配FML缓冲区 ****/
	len=(FLDLEN)(FirstOccs*OutputParNum*50);
	*transOUT=(FBFR *)tpalloc(FMLTYPE,NULL,len);
	if(*transOUT==(FBFR *)NULL)
	{
		#ifdef _DEBUG_
		sprintf(temp_buf,"\n内存分配失败 服务名=[%-s],size[%-ld]",SerName,len);
		userlog(temp_buf);
		#endif     /* 错误处理函数自动释放 transIN 和 transOUT */
		error_handler(transIN,*transOUT,"200",temp_buf,loginlable,"0");
	}
	return *transOUT;
}
#endif
#ifdef _FML16_
void get_input_parms(FBFR *transIN, int *input_par_num, int *output_par_num,
                     char *dbconnName,char *connType)
{
	int i,j=0;
	int  occs=0,count=0;
	char temp[20];
	char temp_buf[100];
	char errno[10],errmsg[200];

	*input_par_num=0;
	*output_par_num=0;

	if(transIN == NULL)
	{
		strcpy(errno,"205");
		sprintf(errmsg, "transOUT buffer is NULL,you must allocate memory for transIN!");
		error_handler(transIN,NULL,	errno,errmsg,dbconnName,connType);
	}

	if(Fget(transIN, SEND_PARMS_NUM, (FLDOCC)0, temp, NULL) == -1)
	{
		strcpy(errno,"101");
		strcpy(errmsg,"Fget input parms error:SEND_PARAMS_NUM");
		error_handler(transIN,NULL,errno,errmsg, dbconnName,connType);
	}
	else
	{
		*input_par_num=atoi(temp);
	}

	if(Fget(transIN, RECP_PARMS_NUM, 0, temp, NULL) == -1)
	{
		strcpy(errno,"102");
		strcpy(errmsg,"Fget input parms error:RECP_PARAMS_NUM");
		error_handler(transIN,NULL,errno,errmsg,dbconnName,connType);
	}
	else
	{
		*output_par_num=atoi(temp);
	}

	for(i=0;i<*input_par_num;i++)
	{
		occs=(int)Foccur(transIN,GPARM_0+i);
		if(occs>1)
		{
			for(j=0;j<occs;j++)
			{
				if(Fget(transIN,GPARM_0+i, j, input_parms[count], NULL) == -1)
				{
					strcpy(errno,"103");
					sprintf(errmsg,"Fget input parms error: get param %ld value error in get_input_parms.",(long)GPARM_0+i);
					error_handler(transIN,NULL,errno,errmsg, dbconnName,connType);
				}
				count++;
			}
		}
		else
		{
			if(Fget(transIN,GPARM_0+i, 0, input_parms[count], NULL) == -1)
			{
				strcpy(errno,"103");
				sprintf(errmsg,"Fget input parms error: get param %ld value error in get_input_parms.",(long)GPARM_0+i);
				error_handler(transIN,NULL,errno,errmsg,dbconnName,connType);
			}
			count++;
		}
	}
}
#endif

#ifdef _FML16_
FBFR *add_value(FBFR  *transIN,     FBFR *transOUT,
                long  realloc_size, char *service_name,
                FLDID param_name,   char *param_value,
                char  *dbconnName,char *connType)
{
	FBFR  *tmp_fbfr;
	char  temp_buf[100];
	char  errno[10],errmsg[200];
	long  len=0;
	int   err_flag=0;

	if(transOUT == NULL)
	{
		strcpy(errno,"205");
		sprintf(errmsg, "SERVICE %s: transOUT buffer is NULL,you must allocate memory for transOUT!",service_name);
		error_handler(transIN,transOUT,
			errno,errmsg,dbconnName,connType);
	}
	if(Fadd(transOUT,param_name,param_value,NULL) == -1)
	{
		/* Fadd时内存不够 */
		if( Ferror == FNOSPACE )
		{
			len=Fsizeof(transOUT)+realloc_size;

			tmp_fbfr=(FBFR *)tprealloc((char *)transOUT,len);
			if(tmp_fbfr==(FBFR *)NULL)
			{
				strcpy(errno,"201");
				sprintf(errmsg, "SERVICE %s: alloc error in add_value(),size(%ld)", service_name,len);
				error_handler(transIN,transOUT,errno,errmsg,dbconnName,connType);
			}
			transOUT=tmp_fbfr;
			/* 如果仍然发生错误则放弃 */
			if(Fadd(transOUT,param_name,param_value,NULL) == -1)
			{
				strcpy(errno,"202");
				sprintf(errmsg,"SERVICE %s: add_value() error!",service_name);
				error_handler(transIN,transOUT,errno,errmsg,dbconnName,connType);
			}
		}
		else /* Fadd时发生的其他错误 */
		{
			strcpy(errno,"203");
			sprintf(errmsg,"SERVICE %s: add_value() error!",service_name);
			error_handler(transIN,transOUT,errno,errmsg,dbconnName,connType);
		}
	}
	else 	/* Fadd执行成功 */
	{
		Ferror = 0;
	}
	return transOUT;
}
#endif

#ifdef _FML16_
void FML_errchk(char *msg)
{
	int errno;
	errno=Ferror;
}
#endif

#ifdef _FML16_
void warning_handler()
{
	if (sqlca.sqlwarn[1] == 'W')
	{
		userlog("** Data truncated.");
	}

	if (sqlca.sqlwarn[3] == 'W')
	{
		userlog("** Insufficient host variables to store results.");
	}
	return;
}
#endif

#ifdef _FML16_
void error_handler_long_conn(FBFR *transIN,FBFR *transOUT,
                char *errno,char *errmsg,char *dbconnName,char *connType)
{
	char msg[200];
	int  errcode;
	long TpreturnCode = TPFAIL;

	EXEC SQL BEGIN DECLARE SECTION;
		char *name;
	EXEC SQL END DECLARE SECTION;


	TpreturnCode = TPFAIL;
	if (SQLCODE==-16843058){
		TpreturnCode = TPEXIT;
	}

	errcode=Ferror;

	if (sqlca.sqlerrm.sqlerrml)
	{
		userlog("** SQLCODE=(%d)", sqlca.sqlcode);
		snprintf(errmsg,99,"%s ** SQL Server Error ** %s",
		        errmsg,sqlca.sqlerrm.sqlerrmc);
		userlog(msg);
		userlog("\n");
	}

	if (dbconnName[0] != 0 && connType[0] !=0)
	{
		name = strtok(dbconnName,",");
		EXEC SQL ROLLBACK WORK;
		if(strcmp(connType,"0") == 0)
		{
			PUBLOG_APPRUN(__func__,"CLOSE DB CONN",name);
			spublicDBClose(name);
		}
		do
		{
			name = strtok('\0',",");
			if(name)
			{
				EXEC SQL ROLLBACK WORK;
				if(strcmp(connType,"0") == 0)
				{
					PUBLOG_APPRUN(__func__,"CLOSE DB CONN",name);
					spublicDBClose(name);
				}
			}
		}while(name);
	}

	Fchg(transOUT,SVC_ERR_NO,0,errno,(FLDLEN)0);
	Fchg(transOUT,SVC_ERR_MSG,0,errmsg,(FLDLEN)0);
	tpreturn( TPSUCCESS, 0, (char *)transOUT, 0L, 0 );
}
#endif

#ifdef _FML16_
void error_handler(FBFR *transIN,FBFR *transOUT,
                   char *errno,char *errmsg,char *dbconnName,char *connType)
{
	char msg[200];
	int  errcode;

	EXEC SQL BEGIN DECLARE SECTION;
		char *name;
	EXEC SQL END DECLARE SECTION;


	errcode=Ferror;
	if (sqlca.sqlerrm.sqlerrml)
	{
		userlog("** SQLCODE=(%d)", sqlca.sqlcode);
		sprintf(msg, "** SQL Server Error ** %s",sqlca.sqlerrm.sqlerrmc);
		strcat(errmsg,msg);
		userlog(msg);
		userlog("\n");
	}

	if (dbconnName[0] != 0 && connType[0] !=0)
	{
		name = strtok(dbconnName,",");
		EXEC SQL  ROLLBACK WORK;
		if(strcmp(connType,"0") == 0)
			spublicDBClose(name);
		do
		{
			name = strtok('\0',",");
			if(name)
			{
				EXEC SQL  ROLLBACK WORK;
				if(strcmp(connType,"0") == 0)
					spublicDBClose(name);
			}
		}while(name);
	}


	Fchg(transOUT,SVC_ERR_NO,0,errno,(FLDLEN)0);
	Fchg(transOUT,SVC_ERR_MSG,0,errmsg,(FLDLEN)0);
	tpreturn( TPSUCCESS, 0, (char *)transOUT, 0L, 0 );
}
#endif

#ifdef _FML16_
void ExitTuxedo(char* sErrNo, char * sErrMsg, FBFR *transOUT, FBFR *transIN, 
                char* cDbLabel, char *cServiceName)
{
	if (cDbLabel != NULL) 
	{
		if (!spublicDBClose(cDbLabel))
		{
		}
		else
		{
#ifdef _DEBUG_
			userlog("\n断开短连接失败\n");
#endif
		}
	}
	Fchg(transOUT,SVC_ERR_NO,0,sErrNo,(FLDLEN)0);
	Fchg(transOUT,SVC_ERR_MSG,0,sErrMsg,(FLDLEN)0);
	/*** Ffree(transIN); ***/
#ifdef _DEBUG_
			userlog("\nsErrNo=%s,sErrMsg=%s\n", sErrNo, sErrMsg);
#endif
	tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0);
}
#endif


void funErrMsg(const char *id_no,
               const char *phone_no,
               const char *op_code,
               const char *login_no,
               const char *login_accept,
               const char *program_name,
               const char *error_no,
               const char *bak_field)
{
	char        vId_No[14+1];         /* 用户ID         */
	char        vPhone_No[15+1];      /* 移动号码       */
	char        vOp_Code[4+1];        /* 操作代码       */
	char        vLogin_No[6+1];       /* 操作工号       */
	char        vLogin_Accept[14+1];  /* 操作流水       */
	char        vProgram_Name[60+1];  /* 程序名称       */
	char        vError_No[6+1];       /* 错误代码       */
	char        vBak_Field[1024+1];   /* 保留字段       */
	char        vSql_Text1[1024+1];   /* SQL语句1       */
	static char vSql_Text[1024+1];    /* SQL语句        */
	int         vSql_Code;            /* SQLCODE        */
	char        vSql_ErrMsg[1024+1];  /* SQLERRMSG      */
	char        vTmp_String[1024+1];  /* 临时变量       */

	long        vId_No_Tmp;           /* 用户ID(long)   */
	long        vLogin_Accept_Tmp;    /* 操作流水(long) */

	size_t stmlen,sqlfc;       /* 捕获SQL的变量  */

	printf("Starting funErrMsg\n");
	memset(vId_No,        0, sizeof(vId_No));
	memset(vPhone_No,     0, sizeof(vPhone_No));
	memset(vOp_Code,      0, sizeof(vOp_Code));
	memset(vLogin_No,     0, sizeof(vLogin_No));
	memset(vLogin_Accept, 0, sizeof(vLogin_Accept));
	memset(vProgram_Name, 0, sizeof(vProgram_Name));
	memset(vError_No,     0, sizeof(vError_No));
	memset(vBak_Field,    0, sizeof(vBak_Field));
	memset(vSql_Text1,    0, sizeof(vSql_Text1));
	memset(vSql_Text,     0, sizeof(vSql_Text));
	memset(vSql_ErrMsg,   0, sizeof(vSql_ErrMsg));
	memset(vTmp_String,   0, sizeof(vTmp_String));
	vSql_Code           = 0;

	vId_No_Tmp          = 0;
	vLogin_Accept_Tmp   = 0;

	if (id_no != NULL)
		strcpy(vId_No,      id_no);
	else
		strcpy(vId_No,      "0");

	if (phone_no != NULL)
		strcpy(vPhone_No,   phone_no);
	else
		strcpy(vPhone_No,   "UnKnown");

	if (op_code != NULL)
		strcpy(vOp_Code,    op_code);
	else
		strcpy(vOp_Code,   "0000");

	if (login_no != NULL)
		strcpy(vLogin_No,  login_no);
	else
		strcpy(vLogin_No,  "UnKnow");

	if (login_accept != NULL)
		strcpy(vLogin_Accept, login_accept);
	else
		strcpy(vLogin_Accept, "0");

	if (program_name != NULL)
		strcpy(vProgram_Name, program_name);
	else
		strcpy(vProgram_Name, "UnKnown");

	if (error_no != NULL)
		strcpy(vError_No, error_no);
	else
		strcpy(vError_No, "999999");

	if (bak_field != NULL)
		strcpy(vBak_Field,     bak_field);
	else
		strcpy(vBak_Field, "UnKnown");

	vId_No_Tmp        = atol(Trim(vId_No));
	vLogin_Accept_Tmp = atol(Trim(vLogin_Accept));

#ifdef _DEBUG_
	printf("debug msg:\tId_No=[%s]\t用户ID=[%s]\n",vId_No,vId_No);
	printf("debug msg:\tId_No=[%s]\t移动号码=[%s]\n",vId_No,vPhone_No);
	printf("debug msg:\tId_No=[%s]\t操作代码=[%s]\n",vId_No,vOp_Code);
	printf("debug msg:\tId_No=[%s]\t操作流水=[%s]\n",vId_No,vLogin_Accept);
	printf("debug msg:\tId_No=[%s]\t程序名称=[%s]\n",vId_No,vProgram_Name);
	printf("debug msg:\tId_No=[%s]\t错误代码=[%s]\n",vId_No,vError_No);
	printf("debug msg:\tId_No=[%s]\t保留字段=[%s]\n",vId_No,vBak_Field);
	printf("debug msg:\tId_No=[%s]\t用户ID(long)=[%ld]\n",vId_No,vId_No_Tmp);
	printf("debug msg:\tId_No=[%s]\t操作流水=[%ld]\n",vId_No,vLogin_Accept_Tmp);
#endif

	stmlen = sizeof(vSql_Text);

	if (SQLCODE != 0)
	{
		vSql_Code = SQLCODE;
		strcpy(vSql_ErrMsg,SQLERRMSG);
		Trim(vSql_ErrMsg);

		SQLStmtGetText(0, vSql_Text, &stmlen, &sqlfc);
		Trim(vSql_Text);
	}
	else
	{
		strcpy(vSql_Text,"UnKnown");
	}

	if (strlen(vSql_ErrMsg) == 0)    strcpy(vSql_ErrMsg,"UnKnown");
	if (strlen(vSql_Text)   == 0)    strcpy(vSql_Text,"UnKnown");

#ifdef _DEBUG_
	printf("funErrMsg:SQLCODE=[%d]\n",vSql_Code);
	printf("funErrMsg:SQLERRMSG=[%s]\n",vSql_ErrMsg);
	printf("funErrMsg:SQLTEXT=[%s]\n",vSql_Text);
#endif

	EXEC SQL ROLLBACK;

	strcpy(vSql_Text1,"\0");
	strcpy(vSql_Text1,"INSERT INTO wErrorLog ");
	strcat(vSql_Text1," (Id_No,        Phone_No,       Op_Code,");
	strcat(vSql_Text1," Op_Time, ");
	strcat(vSql_Text1," Login_No,      Login_Accept,   Sql_Text,");
	strcat(vSql_Text1," Program_Name,  Error_No,");
	strcat(vSql_Text1," Oracle_Err_No, Oracle_Err_Msg, Bak_Field)");

	strcpy(vTmp_String,"\0");
	strcpy(vTmp_String,vSql_Text1);
	sprintf(vSql_Text1,"%s VALUES(%ld,'%s','%s',",
						vTmp_String,vId_No_Tmp,vPhone_No,vOp_Code);

	strcpy(vTmp_String,"\0");
	strcpy(vTmp_String,vSql_Text1);
	sprintf(vSql_Text1,"%s SysDate,",vTmp_String);

	strcpy(vTmp_String,"\0");
	strcpy(vTmp_String,vSql_Text1);
	sprintf(vSql_Text1,"%s '%s',%ld,'%s',",
						vTmp_String,vLogin_No,vLogin_Accept_Tmp,vSql_Text);

	strcpy(vTmp_String,"\0");
	strcpy(vTmp_String,vSql_Text1);
	sprintf(vSql_Text1,"%s '%s','%s',",
						vTmp_String,vProgram_Name,vError_No);

	strcpy(vTmp_String,"\0");
	strcpy(vTmp_String,vSql_Text1);
	sprintf(vSql_Text1,"%s %d,'%s','%s')",
						vTmp_String,vSql_Code,vSql_ErrMsg,vBak_Field);

#ifdef _DEBUG_
	printf("debug msg:\tId_No=[%s]\tSql_Statement=[%s]\n",vId_No,vSql_Text1);
#endif

	strcpy(vSql_Text1,"\0");
	strcpy(vSql_Text1,"INSERT INTO wErrorLog ");
	strcat(vSql_Text1," (Id_No,        Phone_No,       Op_Code,");
	strcat(vSql_Text1," Op_Time, ");
	strcat(vSql_Text1," Login_No,      Login_Accept,   Sql_Text,");
	strcat(vSql_Text1," Program_Name,  Error_No,");
	strcat(vSql_Text1," Oracle_Err_No, Oracle_Err_Msg, Bak_Field)");
	strcat(vSql_Text1," VALUES(:v1,:v2,:v3,");
	strcat(vSql_Text1," SysDate,");
	strcat(vSql_Text1," :v5,:v6,:v7,");
	strcat(vSql_Text1," :v8,:v9,");
	strcat(vSql_Text1," :v10,:v11,:v12)");

	EXEC SQL PREPARE Prepare_wErrorLog from :vSql_Text1;
	EXEC SQL EXECUTE Prepare_wErrorLog using
					:vId_No_Tmp,    :vPhone_No,        :vOp_Code,
					:vLogin_No,     :vLogin_Accept_Tmp,:vSql_Text,
					:vProgram_Name, :vError_No,
					:vSql_Code,     :vSql_ErrMsg,      :vBak_Field;

#ifdef _DEBUG_
	printf("debug msg:\tId_No=[%s]\tINSERT ERRMSG SQLCODE=[%d]\n",vId_No,SQLCODE);
#endif

	EXEC SQL COMMIT;
}

/*
 获取完整错误信息
 Author:yinyx
 Date:2006-4-27 15:12
 Note:
  SQLERRMSG只能显示70个字符的ORACLE错误信息
  该函数可以显示256个字符的ORACLE错误信息
*/
void SQLMSG()
{
	char errmsg[1024];
	size_t buf_len, msg_len;
	memset(errmsg, 0, sizeof(errmsg));
	buf_len = sizeof(errmsg);
	sqlglm(errmsg, &buf_len, &msg_len);
	errmsg[msg_len-1] = 0;
	Trim(errmsg);
#ifdef _DEBUG_
	printf("SQLMSG:[%ld][%s]\n", msg_len, errmsg);
#endif
}

/*
 获取当前SQL语句
 Author:yinyx
 Date:2006-4-27 15:12
 Note:
  显示最后执行的最后一条SQL语句
*/
void SQLSQL()
{
	char   errmsg[1024];
	size_t buf_len, msg_len;
	memset(errmsg, 0, sizeof(errmsg));
	buf_len = sizeof(errmsg);
	sqlgls(errmsg, &buf_len, &msg_len);
	Trim(errmsg);
#ifdef _DEBUG_
	printf("SQLSQL:[%ld][%s]\n", msg_len, errmsg);
#endif
}

/*
 显示ORACLE错误提示信息
 Author:yinyx
 Date:2006-5-18 18:14
*/
void DEBUGSHOW()
{
#ifdef _DEBUG_
	if (SQLCODE != 0)
	{
		printf("SQLCODE:[%d]\n", SQLCODE);
		SQLMSG();
		SQLSQL();
	}
#endif
}
