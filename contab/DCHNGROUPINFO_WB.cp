/*@name					DCHNGROUPINFO_WB.cp	
 *@description			自办自营厅组织结构信息同步(给网站用)
 *@author				majha
 *@created				2013.01.17
 */
#include <stdio.h>
#include "global.h"
#include "debug.h"
#include "pubdb.h"
#define init(a)	memset(a,0,sizeof(a))
#define LOGIN_NO "system"
#define PARMANAME "DCHNGROUPINFO_WB"
#define REGIONCODEMAX 3
#define	MAXLEN	1024
EXEC SQL INCLUDE SQLCA;
#define SQLROWS  sqlca.sqlerrd[2]
EXEC SQL BEGIN DECLARE SECTION;
	char beginTime[MAX_DATETIME_LEN+1];
	char endTime[MAX_DATETIME_LEN+1];
	char region_code[REGIONCODEMAX];	
EXEC SQL END DECLARE SECTION;


int DCHNGROUPINFO_WB(TProgram* program,char** argv)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char sqlstring[1024];
		int  denorm_level=1;
		char temp_date[8+1];
		char file_name[512];
		char i_sFileDir[MAXLEN+1];		/* 数据文件目录 */
		char i_sFileDirBak[MAXLEN+1];	/* 数据文件备份目录 */
		char l_sFTPPath[MAXLEN+1];		/* FTP传送脚本文件绝对路径 */
		char l_sSendPath[MAXLEN+1];		/* 远程FTP文件路径 */
		
		char l_sSendIP[20+1];			/* 远程登陆主机IP地址 */
		char l_sUser[20+1];				/* 远程登录主机用户名 */
		char l_sPassWD[20+1];			/* 远程登陆主机用户密码 */	
		char order_str[512];
		FILE *fp;
		char vGroupId[10+1];
		char vParentGroupId[10+1];
		char vGroupName[100+1];
		char vDenormLevel[2+1];
		char vBelongCode[4+1];
	EXEC SQL END DECLARE SECTION;
	
	init(sqlstring);
	init(temp_date);
	init(file_name);
	init(i_sFileDir);
	init(i_sFileDirBak);
	init(l_sSendIP);
	init(l_sUser);
	init(l_sPassWD);
	init(l_sFTPPath);
	init(order_str);
	/* 获取配置数据参数 */
	strcpy(i_sFileDir, program->parameters.parameter[0].paramValue);
	strcpy(i_sFileDirBak, program->parameters.parameter[1].paramValue);
	strcpy(l_sSendIP, program->parameters.parameter[2].paramValue);
	strcpy(l_sUser, program->parameters.parameter[3].paramValue);
	strcpy(l_sPassWD, program->parameters.parameter[4].paramValue);
	strcpy(l_sSendPath, program->parameters.parameter[5].paramValue);
	strcpy(l_sFTPPath, program->parameters.parameter[6].paramValue);
		
	EXEC SQL WHENEVER SQLERROR CONTINUE;
	EXEC SQL WHENEVER SQLWARNING CONTINUE;
	EXEC SQL WHENEVER NOT FOUND CONTINUE;
	EXEC SQL select to_char(sysdate,'YYYYMMDD') into temp_date from dual;
	if (sqlca.sqlcode!=0) {
		printf("取系统时间错误\n");
		return -1;
	}	
	
	
	sprintf(file_name,"%s%s.%s",i_sFileDir,"DChngroupinfo",temp_date);
	Trim(file_name);
	sprintf(sqlstring,"%s","select distinct  c.group_id,c.parent_group_id,d.group_name ,to_char(denorm_level),substr(d.boss_org_code,1,4) "
				" from dchngroupmsg b,dchngroupinfo c,dchngroupmsg d "
				" where c.group_id in (select group_id   from dchngroupmsg where class_code='1068')"
				" and c.group_id=b.group_id"
				" and c.parent_group_id=d.group_id ");
	EXEC SQL PREPARE Sql_info FROM :sqlstring;	
	EXEC SQL DECLARE group_cur CURSOR FOR Sql_info;	
	EXEC SQL OPEN group_cur ;
	printf("sqlstring is [%s]\n",sqlstring); 
	if (sqlca.sqlcode!=0) {
		printf("open cursor is failure\n");
		return -1;	
	}	
	
	if ((fp=fopen(file_name,"w+"))==NULL){
	
		printf("打开文件失败!\n");
		EXEC SQL CLOSE group_cur;
		return -1;
	}	
	for (;;){
		
		memset(&vGroupId,0,sizeof(vGroupId));
		memset(&vParentGroupId,0,sizeof(vParentGroupId));
		memset(&vGroupName,0,sizeof(vGroupName));
		memset(&vDenormLevel,0,sizeof(vDenormLevel));
		memset(&vBelongCode,0,sizeof(vBelongCode));
		
		EXEC SQL FETCH group_cur INTO :vGroupId,:vParentGroupId,:vGroupName,:vDenormLevel,:vBelongCode;	
		if ((SQLCODE!=0)||(SQLCODE==1403))	
		{
			printf("[%d]\n",SQLCODE);	
			break;	
		}	
		fprintf(fp,"%s|%s|%s|%s|%s|\n",Trim(vGroupId),Trim(vParentGroupId),Trim(vGroupName),Trim(vDenormLevel),Trim(vBelongCode));		
	}	
	EXEC SQL CLOSE group_cur;
	if (fclose(fp)) printf("关闭文件失败!\n");
	init(file_name);
	sprintf(file_name,"%s.%s","DChngroupinfo",temp_date);
	sprintf(order_str,"sh %sDCHNGROUPINFO_WB.sh %s %s %s %s %s %s %s",
			l_sFTPPath,i_sFileDir,i_sFileDirBak,l_sSendIP,l_sUser,l_sPassWD,l_sSendPath,file_name);
	printf("order_str is [%s]",order_str);
	
	if (system(order_str)<0){
		fprintf(stderr, "<5>. FTP file is error!\n\tFile = [%s]\n", order_str);
		printf("11. FTP file is error!\n\tFile = [%s]\n", order_str);
		return -1;
	}

	return 0;
	
}
int main(int argc, char **argv)
{
	TProgram program;
	int i=0;
	
	init(programName);
	init(region_code);
	init(beginTime);
	init(endTime);
	if (argc < 1)
	{
		DBUG_PRINT(("DCHNGROUPINFO_WB 参数有错误!\n"));
		exit(0);
	}
	strcpy(programName, PARMANAME);	
	initXml();	
	/* 获取待解析文件参数 */
	if (getProgram(&program, PARMANAME)!= 0)
	{
		fprintf(stderr, "getProgram "PARMANAME" error\n");
		destroyXml();
		exit(0);
	}
	strcpy(beginTime, getTime());
	DBUG_PRINT(("程序开始运行时间：[%s]\n", beginTime));
	/* 是否有可用数据库 */
	if ( 0 == program.dbConnNames.useDbConnNum )
	{
		destroyXml();
		printf("4. getProgram(): [useDBConnNum] is zero!\n");
		return -1;
	}
	
	/* 连接数据库 */
	if ( dbLoginnew(&program.dbConnNames.dbConn[0]) != 0 )
	{
		destroyXml();
		fprintf(stderr, "<2>. dbLoginnew(): Connect to DataBase is error!\n");
		printf("5. dbLoginnew(): Connect to DataBase is error!\n");
		return -1;
	}
	strcpy(beginTime, getTime());
	printf("\n********** Process [%s] is running: [%s] **********\n", PARMANAME, beginTime);
			
	if (DCHNGROUPINFO_WB(&program,argv) != 0)
	{
		DBUG_PRINT((PARMANAME" 运行失败!\n"));
		exit(0);
	}
	strcpy(endTime, getTime());
	printf("********** Process [%s] is SUCCESS: [%s] **********\n", PARMANAME, endTime);
		 
	
	/* 断开已连接的数据库、释放资源 */
	dbClose();
	/*必须在数据库事物完成之后做此操作，否则会coredump*/
	destroyXml();		
	DBUG_LEAVE("main");
	return 0;
}
