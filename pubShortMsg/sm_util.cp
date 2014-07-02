#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pubShortMsg.h"
int tpsvrinit(int argc,char **argv);
void tpsvrdone();

EXEC SQL INCLUDE SQLCA;

int main ( int argc, char *argv[] )
{
	EXEC SQL BEGIN DECLARE SECTION;
		char connectStr[] = "dbchange/abc123@thcustdb";
	EXEC SQL END DECLARE SECTION;

	int i;
	int ret;
	int smModelId = 1063;
	int model_num = 1;
	int test_time = 1;
	char vOpCode[] = "1100";

	tpsvrinit(0,0);

	EXEC SQL Connect :connectStr;

	if(SQLCODE != 0)
	{
		printf("连接数据库出错\n");
	}

	for(i = 0; i < model_num * test_time; i++)
	{
		/*准备组装的新的短信*/
		PrepareNewShortMsg(smModelId);
		
		char key[100];
		char value[100];
		
		strcpy(key,"USER_NAME");
		strcpy(value,"孙振兴");
		
		SetShortMsgParameter(smModelId, key, value);

		SetShortMsgParameter(smModelId, "PASSWORD", "123456");

		ret = GenerateShortMsg(smModelId,"15945995971",vOpCode,12319842);
		printf("ret = %d\n",ret);
	}

	tpsvrdone();
	
	EXEC SQL ROLLBACK WORK RELEASE;

	return 0;
}

int tpsvrinit(int argc,char **argv)
{
	/*初始化短信组装环境*/
	InitShortMsgLib();
	return 0;
}

void tpsvrdone()
{
	/*销毁短信组装环境*/
	DestroyShortMsgLib();

}

