/**********************
 * 源文件名：Re882.cp
 * 版本号：1.0
 * 创建作者：miaoyl
 * 创建时间：2012-06-20
 * 代码依赖：
 * 功能描述：省内增值业务平台对账查询
 * Tuxedo样例代码
 * 修改历史:
 * [修改人姓名][修改时间][修改内容]
 *********************/
#include "boss_srv.h"
#include "publicsrv.h"
#include "publicEmer.h"
/*如果数据库连接方式采用长连接，则放开DB_LONG_CONN注释
#define DB_LONG_CONN*/
/*如果使用其他数据库标签，请修改此变量定义*/
/*#define DB_LABEL LABELDBCHANGE*/
#include "tux_server.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/*
 +Server开始代码声明+
*/
TUX_SERVER_BEGIN();

/************************************************************
 *@ 服务名称：se882Qry
 *@ 编码日期：2012/06/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：miaoyl
 *@ 功能描述：省内增值业务平台对账查询
 *@
 *@ 输入参数：
 *@			iOpCode			功能编号
 *@			iLoginNo		操作工号
 *@			iLoginPwd		工号密码
 *@			iOrgCode		工号归属
 *@			iCommandCode平台类型
 *@			iBeginTime	开始时间
 *@			iEndTime		结束时间
 *@ 返回参数：
 *@		
 *@
 *@ 修改记录:
 *@			修改人员：
 *@			修改日期：
 *@			修改前版本号：
 *@			修改后版本号:
 *@			修改原因：
 *@			测试用例：
 cl32 se882Qry 7 11 e882 aa021A COJKPFLAJGAPLGEB 0101031AA 00 20120525 20120701
 cl32 se882Qry 7 11 e882 aa021A COJKPFLAJGAPLGEB 0101031AA 46 20120525 20120701
 ************************************************************/
_TUX_SVC_ se882Qry( TPSVCINFO *transb )
{
	int i=0;
	EXEC SQL BEGIN DECLARE SECTION;
		/*输入参数定义,命名方式以i-开头,并且每个单词首字母大写*/
		long	iLoginAccept=0;			/*操作流水*/
		char	iOpCode[4+1];				/*功能编号*/
		char	iLoginNo[6+1];			/*操作工号*/
		char	iLoginPwd[16+1];		/*工号密码*/
		char	iOrgCode[9+1];			/*工号归属*/
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
	
	/*注意修改服务名字*/
	strcpy(service_name,"se882Qry");
	
	/*+Service开始代码声明+*/
	TUX_SERVICE_BEGIN(iLoginAccept,iOpCode,iLoginNo,iOpNote,ORDERIDTYPE_USER,vIdNo,"ALL",NULL);
	
	/*获取输入参数*/
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
	
	/*业务逻辑开始*/
	sprintf(vBeginTime,"%s000000",iBeginTime);
	sprintf(vEndTime,"%s235959",iEndTime);
	
	printf("vBeginTime=[%s],vEndTime=[%s]\n",vBeginTime,vEndTime);   
	
	if(strcmp(iCommandCode,"00")!=0)
	{	
		/*获取用户的基本信息 */			
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
			/*拆串*/
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
			/*BOSS数据量,增值业务平台数据量,总差异数据量,BOSS多于平台，平台多于BOSS*/
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
			/*拆串*/
			getValueByIndex(vResultDataTmp,0,ch,vResultData[0]);	
			getValueByIndex(vResultDataTmp,1,ch,vResultData[1]);	
			getValueByIndex(vResultDataTmp,2,ch,vResultData[2]);	
			getValueByIndex(vResultDataTmp,3,ch,vResultData[3]);	
			getValueByIndex(vResultDataTmp,4,ch,vResultData[4]);	
			getValueByIndex(vResultDataTmp,5,ch,vResultData[5]);	
			
			/*获取用户的基本信息 */			
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
			/*BOSS数据量,增值业务平台数据量,总差异数据量,BOSS多于平台，平台多于BOSS*/
		}
		EXEC SQL CLOSE strCur02;
	}		
	/*业务逻辑结束*/
	
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
	
	/*TODO:向输出缓冲区中增加其他的结果*/
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	
	/*
	+Service结束代码声明+
	*/
TUX_SERVICE_END();
}
