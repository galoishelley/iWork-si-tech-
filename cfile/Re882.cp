/**********************
 * Դ�ļ�����Re882.cp
 * �汾�ţ�1.0
 * �������ߣ�miaoyl
 * ����ʱ�䣺2012-06-20
 * ����������
 * ����������ʡ����ֵҵ��ƽ̨���˲�ѯ
 * Tuxedo��������
 * �޸���ʷ:
 * [�޸�������][�޸�ʱ��][�޸�����]
 *********************/
#include "boss_srv.h"
#include "publicsrv.h"
#include "publicEmer.h"
/*������ݿ����ӷ�ʽ���ó����ӣ���ſ�DB_LONG_CONNע��
#define DB_LONG_CONN*/
/*���ʹ���������ݿ��ǩ�����޸Ĵ˱�������*/
/*#define DB_LABEL LABELDBCHANGE*/
#include "tux_server.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/*
 +Server��ʼ��������+
*/
TUX_SERVER_BEGIN();

/************************************************************
 *@ �������ƣ�se882Qry
 *@ �������ڣ�2012/06/20
 *@ ����汾: Ver1.0
 *@ ������Ա��miaoyl
 *@ ����������ʡ����ֵҵ��ƽ̨���˲�ѯ
 *@
 *@ ���������
 *@			iOpCode			���ܱ��
 *@			iLoginNo		��������
 *@			iLoginPwd		��������
 *@			iOrgCode		���Ź���
 *@			iCommandCodeƽ̨����
 *@			iBeginTime	��ʼʱ��
 *@			iEndTime		����ʱ��
 *@ ���ز�����
 *@		
 *@
 *@ �޸ļ�¼:
 *@			�޸���Ա��
 *@			�޸����ڣ�
 *@			�޸�ǰ�汾�ţ�
 *@			�޸ĺ�汾��:
 *@			�޸�ԭ��
 *@			����������
 cl32 se882Qry 7 11 e882 aa021A COJKPFLAJGAPLGEB 0101031AA 00 20120525 20120701
 cl32 se882Qry 7 11 e882 aa021A COJKPFLAJGAPLGEB 0101031AA 46 20120525 20120701
 ************************************************************/
_TUX_SVC_ se882Qry( TPSVCINFO *transb )
{
	int i=0;
	EXEC SQL BEGIN DECLARE SECTION;
		/*�����������,������ʽ��i-��ͷ,����ÿ����������ĸ��д*/
		long	iLoginAccept=0;			/*������ˮ*/
		char	iOpCode[4+1];				/*���ܱ��*/
		char	iLoginNo[6+1];			/*��������*/
		char	iLoginPwd[16+1];		/*��������*/
		char	iOrgCode[9+1];			/*���Ź���*/
		char	iOpNote[60+1];
		char	vIdNo[14+1];
		char	service_name[19+1];

		char	iCommandCode[2+1];
		char	iBeginTime[8+1];
		char	iEndTime[8+1];
		char	vBeginTime[15+1];
		char	vEndTime[15+1];
		char	vUnit_ID[64+1];
		char	vUnit_Name[30+1];
		char	vKpi_ID[32+1];
		char	vKpi_Value[256+1];
		char	vUnitIDTmp[64+1];
		char	vDataTime[15+1];
		char	vResultDataTmp[128+1];
		char	ch=',';
		char  vResultData[2048][100];
		char	vStrTmp[512+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(iLoginNo);
	Sinitn(iLoginPwd);
	Sinitn(iOrgCode);
	Sinitn(iOpCode);
	Sinitn(iOpCode);
	Sinitn(vIdNo);
	Sinitn(service_name);
	
	Sinitn(iCommandCode);
	Sinitn(iBeginTime);
	Sinitn(iEndTime);
	Sinitn(vBeginTime);
	Sinitn(vEndTime);
	Sinitn(vUnit_ID);
	Sinitn(vUnit_Name);
	Sinitn(vKpi_ID);
	Sinitn(vKpi_Value);
	Sinitn(vUnitIDTmp);
	Sinitn(vDataTime);
	Sinitn(vResultDataTmp);
	Sinitn(vStrTmp);
	
	memset(vResultData,0,sizeof(vResultData));
	
	/*ע���޸ķ�������*/
	strcpy(service_name,"se882Qry");
	
	/*+Service��ʼ��������+*/
	TUX_SERVICE_BEGIN(iLoginAccept,iOpCode,iLoginNo,iOpNote,ORDERIDTYPE_USER,vIdNo,"ALL",NULL);
	
	/*��ȡ�������*/
	strcpy(iOpCode,					input_parms[0]);
	strcpy(iLoginNo,				input_parms[1]);
	strcpy(iLoginPwd,				input_parms[2]);
	strcpy(iOrgCode,				input_parms[3]);
	strcpy(iCommandCode,		input_parms[4]);
	strcpy(iBeginTime,			input_parms[5]);
	strcpy(iEndTime,				input_parms[6]);
	
	Trim(iLoginNo);
	Trim(iLoginPwd);
	Trim(iOrgCode);
	Trim(iCommandCode);
	Trim(iBeginTime);
	Trim(iEndTime);
	
	/*ҵ���߼���ʼ*/
	sprintf(vBeginTime,"%s000000",iBeginTime);
	sprintf(vEndTime,"%s235959",iEndTime);
	
	printf("vBeginTime=[%s],vEndTime=[%s]\n",vBeginTime,vEndTime);   
	
	if(strcmp(iCommandCode,"00")!=0)
	{	
		/*��ȡ�û��Ļ�����Ϣ */			
		EXEC SQL select unit_id,Trim(unit_name)||'('||Trim(unit_code)||')'
							 into :vUnit_ID,:vUnit_Name
							 from SUNITDESCRIPTION
							where unit_code=:iCommandCode and true_flag='1';
		if(SQLCODE != 0)
		{
			strcpy(oRetCode,"e88201");
			sprintf(oRetMsg,"select SUNITDESCRIPTION err[%d]!",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		Trim(vUnit_ID);
		Trim(vUnit_Name);
		
		printf("vUnit_ID=[%s],vUnit_Name=[%s]\n",vUnit_ID,vUnit_Name);   
		
		sprintf(vStrTmp,"select unit_id,datatime,max(r)||',' as resultdata "
									 "from "
									 "(select unit_id,datatime,kpi_id,wmsys.WM_CONCAT(kpi_values)over(partition BY unit_id,datatime order by kpi_id) as r "
									 "from dSamdat "
									 "where unit_id like '%s'||'%%' "
									 "and datatime >= '%s' "
									 "and datatime <= '%s' ) "
									 "group by unit_id,datatime ",vUnit_ID,vBeginTime,vEndTime);
		Trim(vStrTmp);
		printf("vStrTmp=%s \n",vStrTmp);
		EXEC SQL PREPARE vSql_Prepare from :vStrTmp;
		EXEC SQL DECLARE strCur01 CURSOR for vSql_Prepare;
		EXEC SQL OPEN strCur01;
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"e88202");
			sprintf(oRetMsg,"open strCur01 err[%d]!",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		i=0;
		for(;;)
		{
			Sinitn(vUnitIDTmp);
			Sinitn(vDataTime);
			Sinitn(vResultDataTmp);
			memset(vResultData,0,sizeof(vResultData));
			
			EXEC SQL FETCH strCur01 INTO :vUnitIDTmp,:vDataTime,:vResultDataTmp;
			
			if(SQLCODE==1403) break;
			if(SQLCODE!=0)
			{
				strcpy(oRetCode,"e88203");
				sprintf(oRetMsg,"fetch strCur01 err[%d]!",SQLCODE);
				PUBLOG_DBDEBUG(service_name);
				PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
				EXEC SQL CLOSE strCur01; 
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vUnitIDTmp);
			Trim(vDataTime);
			Trim(vResultDataTmp);
			
			printf("\nvUnitIDTmp=%s vDataTime=%s vResultDataTmp=%s \n",vUnitIDTmp,vDataTime,vResultDataTmp);
			/*��*/
			getValueByIndex(vResultDataTmp,0,ch,vResultData[0]);	
			getValueByIndex(vResultDataTmp,1,ch,vResultData[1]);	
			getValueByIndex(vResultDataTmp,2,ch,vResultData[2]);	
			getValueByIndex(vResultDataTmp,3,ch,vResultData[3]);	
			getValueByIndex(vResultDataTmp,4,ch,vResultData[4]);	
			getValueByIndex(vResultDataTmp,5,ch,vResultData[5]);	
	
			transOUT = Add_Ret_Value(GPARM32_2,iCommandCode);
			transOUT = Add_Ret_Value(GPARM32_3,vUnit_Name);
			transOUT = Add_Ret_Value(GPARM32_4,vUnitIDTmp);
			transOUT = Add_Ret_Value(GPARM32_5,vDataTime);
			transOUT = Add_Ret_Value(GPARM32_6,vResultData[0]);
			transOUT = Add_Ret_Value(GPARM32_7,vResultData[1]);
			transOUT = Add_Ret_Value(GPARM32_8,vResultData[2]);
			transOUT = Add_Ret_Value(GPARM32_9,vResultData[3]);
			transOUT = Add_Ret_Value(GPARM32_10,vResultData[4]);
			transOUT = Add_Ret_Value(GPARM32_11,vResultData[5]);
			/*BOSS������,��ֵҵ��ƽ̨������,�ܲ���������,BOSS����ƽ̨��ƽ̨����BOSS*/
		}
		EXEC SQL CLOSE strCur01;
	}
	else if(strcmp(iCommandCode,"00")==0)
	{
		sprintf(vStrTmp,"select unit_id,datatime,max(r)||',' as resultdata "
									 "from "
									 "(select unit_id,datatime,kpi_id,wmsys.WM_CONCAT(kpi_values)over(partition BY unit_id,datatime order by kpi_id) as r "
									 "from dSamdat "
									 "where "
									 "datatime >= '%s' "
									 "and datatime <= '%s' ) "
									 "group by unit_id,datatime ",vBeginTime,vEndTime);
		Trim(vStrTmp);
		printf("vStrTmp=%s \n",vStrTmp);
		EXEC SQL PREPARE vSql_Prepare from :vStrTmp;
		EXEC SQL DECLARE strCur02 CURSOR for vSql_Prepare;
		EXEC SQL OPEN strCur02;
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"e88206");
			sprintf(oRetMsg,"open strCur01 err[%d]!",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		i=0;
		for(;;)
		{
			Sinitn(vUnitIDTmp);
			Sinitn(vDataTime);
			Sinitn(vResultDataTmp);
			memset(vResultData,0,sizeof(vResultData));
			
			EXEC SQL FETCH strCur01 INTO :vUnitIDTmp,:vDataTime,:vResultDataTmp;
			
			if(SQLCODE==1403) break;
			if(SQLCODE!=0)
			{
				strcpy(oRetCode,"e88207");
				sprintf(oRetMsg,"fetch strCur01 err[%d]!",SQLCODE);
				PUBLOG_DBDEBUG(service_name);
				PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
				EXEC SQL CLOSE strCur02; 
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vUnitIDTmp);
			Trim(vDataTime);
			Trim(vResultDataTmp);
			
			printf("\nvUnitIDTmp=%s vDataTime=%s vResultDataTmp=%s \n",vUnitIDTmp,vDataTime,vResultDataTmp);
			/*��*/
			getValueByIndex(vResultDataTmp,0,ch,vResultData[0]);	
			getValueByIndex(vResultDataTmp,1,ch,vResultData[1]);	
			getValueByIndex(vResultDataTmp,2,ch,vResultData[2]);	
			getValueByIndex(vResultDataTmp,3,ch,vResultData[3]);	
			getValueByIndex(vResultDataTmp,4,ch,vResultData[4]);	
			getValueByIndex(vResultDataTmp,5,ch,vResultData[5]);	
			
			/*��ȡ�û��Ļ�����Ϣ */			
			EXEC SQL select Trim(unit_name)||'('||Trim(unit_code)||')'
								 into :vUnit_Name
								 from SUNITDESCRIPTION
								where unit_id=:vUnitIDTmp and true_flag='1';
			if(SQLCODE != 0)
			{
				EXEC SQL CLOSE strCur02; 
				strcpy(oRetCode,"e88208");
				sprintf(oRetMsg,"select SUNITDESCRIPTION err[%d]!",SQLCODE);
				PUBLOG_DBDEBUG(service_name);
				PUBLOG_DBERR(__FUNCTION__,oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vUnit_Name);
			
			printf("vUnit_ID=[%s],vUnit_Name=[%s]\n",vUnitIDTmp,vUnit_Name);   
		
	
			transOUT = Add_Ret_Value(GPARM32_2,iCommandCode);
			transOUT = Add_Ret_Value(GPARM32_3,vUnit_Name);
			transOUT = Add_Ret_Value(GPARM32_4,vUnitIDTmp);
			transOUT = Add_Ret_Value(GPARM32_5,vDataTime);
			transOUT = Add_Ret_Value(GPARM32_6,vResultData[0]);
			transOUT = Add_Ret_Value(GPARM32_7,vResultData[1]);
			transOUT = Add_Ret_Value(GPARM32_8,vResultData[2]);
			transOUT = Add_Ret_Value(GPARM32_9,vResultData[3]);
			transOUT = Add_Ret_Value(GPARM32_10,vResultData[4]);
			transOUT = Add_Ret_Value(GPARM32_11,vResultData[5]);
			/*BOSS������,��ֵҵ��ƽ̨������,�ܲ���������,BOSS����ƽ̨��ƽ̨����BOSS*/
		}
		EXEC SQL CLOSE strCur02;
	}		
	/*ҵ���߼�����*/
	
SERVICE_FINALLY_DECLARE();
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetCode=[%s],oRetMsg=[%s]\n",oRetCode,oRetMsg);

	if(atol(oRetCode) == 0)
	{
		EXEC SQL COMMIT;
	}
	else
	{
		EXEC SQL ROLLBACK;
	}
	
	/*TODO:����������������������Ľ��*/
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	
	/*
	+Service������������+
	*/
TUX_SERVICE_END();
}
