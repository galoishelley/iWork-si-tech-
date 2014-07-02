/******************************************
 *     Prg:  publicEmer.cp
 *     Edit:  yinyx
 *     Date:  2010-5-7 11:09:51
 ******************************************/

#include "boss_srv.h"
#include "publicsrv.h"
#include "publicEmer.h"
/***************************
	发送CRM应急工单
	yinyx
	2010-5-11 13:38:57
***************************/
int fSendOrderEmer(int input_par_num,int output_par_num, FBFR32 *transIN, char *psDBUse,char *service_name,char *id_type, char *idNo, long iLoginAccept, char *opCode, char *loginNo, char *opNote, sPublicDBCfg *dbCfg, char *retCode, char *retMsg)
{
	int i,j,k,s;
	char vParamList[MAX_PARMS_NUM*20][2000];
	char sDbConName[10 + 1];
	int	sCountList[MAX_PARMS_NUM];
	
	memset(vParamList, 0, sizeof(vParamList));
	memset(sDbConName, 0, sizeof(sDbConName));
	strcpy(sDbConName, psDBUse);
	for(s = 0;s < MAX_PARMS_NUM;s++)
	{
		sCountList[s]=0;
	}
	
	/*如果结构体为空则直接取共享内存中的的数据库连接*/
	if(1 != dbCfg->bExists)
	{
		/*goto end;*/
		return 0;
	}
	/*判断发送工单是否生效*/
	if(dbCfg->orderCreateFlag[0] != 'Y')
	{
		return 0;
	}
	/*获取数据库标签*/
	if(dbCfg->emFlag[0] == 'Y')
	{
		memset(sDbConName, 0, sizeof(sDbConName));
		strcpy(sDbConName, dbCfg->dbLabel);
	}
	k = 0,s=0;
	for (i = 0; i < input_par_num; i++)
	{
		sCountList[i]=(int)Foccur32(transIN,GPARM32_0+i);
		for( j = 0;j < sCountList[i]; j++)
		{
			sprintf(vParamList[k], "parms_%d", i);

			if(Fget32(transIN,GPARM32_0+i, j, vParamList[k+1], NULL) == -1)
			{
				strcpy(retCode,"999999");
				sprintf(retMsg,"Fget32 input parms error: get param %ld value error in get_input_parms.",(long)GPARM32_0+i);
				return 1;
			}
			k = k+2;
			if(k >= MAX_PARMS_NUM*20)
			{
				strcpy(retCode,"222222");
				sprintf(retMsg, "输入参数过多，生成工单错误!");
				pubLog_DBErr(_FFL_, service_name, "retCode=[%s]", "retMsg=[%s]", retCode, retMsg);
				return 1;
			}
		}
	}
	
	if (SetOrderBusiSendEM(sDbConName,service_name, input_par_num, output_par_num, vParamList, k ,id_type, idNo, 0, ltoa(iLoginAccept), opCode, loginNo, opNote) != 0)
	{
		strcpy(retCode,"111111");
    	sprintf(retMsg, "生成应急业务工单错误!");
		pubLog_DBErr(_FFL_, service_name, "retCode=[%s]", "retMsg=[%s]", retCode, retMsg);
		return 1;
	}
	
	return 0;
}
