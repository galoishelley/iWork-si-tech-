/*@name					DCHNGROUPINFO_WB.cp	
 *@description			�԰���Ӫ����֯�ṹ��Ϣͬ��(����վ��)
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
		char i_sFileDir[MAXLEN+1];		/* �����ļ�Ŀ¼ */
		char i_sFileDirBak[MAXLEN+1];	/* �����ļ�����Ŀ¼ */
		char l_sFTPPath[MAXLEN+1];		/* FTP���ͽű��ļ�����·�� */
		char l_sSendPath[MAXLEN+1];		/* Զ��FTP�ļ�·�� */
		
		char l_sSendIP[20+1];			/* Զ�̵�½����IP��ַ */
		char l_sUser[20+1];				/* Զ�̵�¼�����û��� */
		char l_sPassWD[20+1];			/* Զ�̵�½�����û����� */	
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
	/* ��ȡ�������ݲ��� */
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
		printf("ȡϵͳʱ�����\n");
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
	
		printf("���ļ�ʧ��!\n");
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
	if (fclose(fp)) printf("�ر��ļ�ʧ��!\n");
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
		DBUG_PRINT(("DCHNGROUPINFO_WB �����д���!\n"));
		exit(0);
	}
	strcpy(programName, PARMANAME);	
	initXml();	
	/* ��ȡ�������ļ����� */
	if (getProgram(&program, PARMANAME)!= 0)
	{
		fprintf(stderr, "getProgram "PARMANAME" error\n");
		destroyXml();
		exit(0);
	}
	strcpy(beginTime, getTime());
	DBUG_PRINT(("����ʼ����ʱ�䣺[%s]\n", beginTime));
	/* �Ƿ��п������ݿ� */
	if ( 0 == program.dbConnNames.useDbConnNum )
	{
		destroyXml();
		printf("4. getProgram(): [useDBConnNum] is zero!\n");
		return -1;
	}
	
	/* �������ݿ� */
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
		DBUG_PRINT((PARMANAME" ����ʧ��!\n"));
		exit(0);
	}
	strcpy(endTime, getTime());
	printf("********** Process [%s] is SUCCESS: [%s] **********\n", PARMANAME, endTime);
		 
	
	/* �Ͽ������ӵ����ݿ⡢�ͷ���Դ */
	dbClose();
	/*���������ݿ��������֮�����˲����������coredump*/
	destroyXml();		
	DBUG_LEAVE("main");
	return 0;
}
