/**********************************************
 @lixg PRG  : pubAdcInterFace.cp
 @lixg EDIT : li_xingang@si-tech.com.cn
 @lixg DATE : 2003-10-15
 @lixg FUNC : ADC平台接口
 *********************************************/

/*******已完成组织结构二期改造 edit by  mengfy 07/05/2009  ****/

#include "boss_srv.h"
#include "publicsrv.h"

/*----组织机构二期改造 增加头文件 和 宏定义 by mengfy @2009/05/07 begin ----*/
#include "pubLog.h"

#define _DEBUG_
/*----组织机构二期改造 增加头文件 和 宏定义 by mengfy @2009/05/07 end ----*/

EXEC SQL INCLUDE SQLCA;

/********************************************** 
	字符串转换成字符数组 
 **********************************************/
int StrToArray(char Array[50][256], char *Str, char *delim) 
{
	int i = 0;
	char *token;

	token = strtok(Str, delim);
	while (token != NULL) {
		strcpy(Array[i++], token);
		token = strtok(NULL, delim);
	}
	
	return 0;
}

/**********************************************
 @lixg FUNC : sendECToAdcInfo
 @input  params: iLoginAccept -> 操作流水
 @input  params: iGrpIdNo -> 集团用户ID
 @input  params: iBizCode -> 业务代码
 @input  params: iOprCode -> 操作类型(01_增加, 02_取消, 05_修改)
 @output params: 0 success else 0 fail
fee_object:
1--对集团号码收费
2--对目的号码收费
3--特定手机号收费
 *********************************************/
int sendECToAdcInfo(char *iLoginAccept, char *iGrpIdNo, char *iBizCode, char *iOprCode, char *oRetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long lLoginAccept = atol(iLoginAccept);
		long lCustId=0;
		int i=0, ret=0, iCount = 0;

		char vOpTime[17+1];
		char vEcID[10+1];
		char vFeeObject[256+1];
		char vFeeTerminalId[256+1];
		char vServiceNo[256+1];
		char vServCode[256+1];
		int  vLisenceNum = 0;
		char vAccessNumber[128+1];
		char vUserName[60+1];
		char vLoginNo[6+1];
		char vOrgCode[9+1];
		
		char vIsPreCharge[1+1];
		int  vMaxItemPerDay = 0;
		int  vMaxItemPerMon = 0;
		char vIsTextSign[1+1];
		char vDefaultSignLang[1+1];
		char vTextSignEn[20+1];
		char vTextSignZh[20+1];
		
		char vStartTime[255+1];
		char vEndTime[255+1];
		char vInfoCode[255+1];
		char vInfoValue[255+1];
		char vParamArray1[50][255+1];
		char vParamArray2[50][255+1];
		char vOpCode[4+1];
		char vUserNo[15+1];
		char vUnitId[18+1];
		char Flag[1+1];
		char vBizScope[1+1];
		char accessnumProp[2+1];
		char ecbizAccessNum[21+1];
		char authMode[1+1];
		char limitCount[8+1];
		char bizMode[1+1];  /*1-EC业务 2-SI面向EC业务 3-SI面向个人业务*/
		char moStr1[256+1];
		char moStr2[256+1];
		char moStr3[256+1];
		char moStr4[256+1];
		char moStr5[256+1];
		
	EXEC SQL END DECLARE SECTION;
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
	TDIAGWSERVOPERIndex tDIAGWSERVOPERIndex;
	TDIAGWSERVOPER tDIAGWSERVOPER;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sTempSqlStr[1024];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	memset(&tDIAGWSERVOPERIndex,0,sizeof(tDIAGWSERVOPERIndex));
	memset(&tDIAGWSERVOPER,0,sizeof(tDIAGWSERVOPER));
	init(v_parameter_array);
	init(sTempSqlStr);
	/*ng解耦 by yaoxc end*/

	Sinitn(vOpTime);
	Sinitn(vServiceNo);
	Sinitn(vServCode);
	Sinitn(vEcID);
	Sinitn(vFeeObject);
	Sinitn(vFeeTerminalId);
	Sinitn(vAccessNumber);
	Sinitn(vIsTextSign);
	Sinitn(vIsPreCharge);
	Sinitn(vDefaultSignLang);
	Sinitn(vTextSignEn);
	Sinitn(vTextSignZh);
	Sinitn(vStartTime);
	Sinitn(vEndTime);
	Sinitn(vInfoCode);
	Sinitn(vInfoValue);
	Sinitn(vUserName);
	Sinitn(vLoginNo);
	Sinitn(vOrgCode);
	Sinitn(vOpCode);
	Sinitn(vUserNo);
	Sinitn(vUnitId);
	Sinitn(vBizScope);	
	Sinitn(Flag);
	Sinitn(accessnumProp);
	Sinitn(ecbizAccessNum);
	Sinitn(bizMode);
	Sinitn(moStr1);
	Sinitn(moStr2);
	Sinitn(moStr3);
	Sinitn(moStr4);
	Sinitn(moStr5);
	
	/*组织结构二期改造 打印输出 by mengfy @2009/05/07 begin ----*/
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "开始调用sendECToAdcInfo函数！");
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "input iLoginAccept = [%s]", iLoginAccept);
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "input iGrpIdNo = [%s]", iGrpIdNo);
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "input iBizCode = [%s]", iBizCode);
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "input iOprCode = [%s]", iOprCode);
	/*组织结构二期改造 打印输出 by mengfy @2009/05/07 end ----*/

	/* 查询集团信息是否存在 */
	EXEC SQL select cust_id, user_no into :lCustId, :vUserNo 
		from dGrpUserMsg where id_no = to_number(:iGrpIdNo);
	if (SQLCODE != 0)
	{
		sprintf(oRetMsg, "ADC:查询集团客户ID失败[%d]！\n", SQLCODE);
		PUBLOG_DBDEBUG("sendECToAdcInfo");
		pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Query dGrpUserMsg error, SQLCODE = [%d]", SQLCODE);
		return -1;
	}
	/*--- 大客户表结构调整 edit by magang at 20090626---begin
	EXEC SQL select '431'||unit_code into :vUnitCode from dGrpMsg where cust_id = :lCustId;
	 --- 大客户表结构调整 edit by magang at 20090626---end*/
	EXEC SQL select '431'||unit_id into :vUnitId from dGrpMsg where cust_id = :lCustId; 
	if (SQLCODE != 0)
	{
		sprintf(oRetMsg, "ADC:查询集团UNIT_ID失败[%d]！\n", SQLCODE);
		PUBLOG_DBDEBUG("sendECToAdcInfo");
		pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Query dGrpMsg error, SQLCODE = [%d]", SQLCODE);
		return -1;
	}
	
	/* 几个特殊数据的处理 */
	EXEC SQL select field_value into :vIsPreCharge from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X4';
	if (SQLCODE != 0)
	{
		strcpy(vIsPreCharge, "0");
	}
	Trim(vIsPreCharge);
	
	EXEC SQL select field_value into :vIsTextSign from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X5';
	if (SQLCODE != 0)
	{
		strcpy(vIsTextSign, "1");
	}
	Trim(vIsTextSign);
	
	EXEC SQL select field_value into :vDefaultSignLang from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X6';
	if (SQLCODE != 0)
	{
		strcpy(vDefaultSignLang, "1");
	}
	Trim(vDefaultSignLang);
	
	EXEC SQL select field_value into :vTextSignEn from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X7';
	if (SQLCODE != 0)
	{
		strcpy(vTextSignEn, "[english]");
	}
	Trim(vTextSignEn);
	
	EXEC SQL select field_value into :vTextSignZh from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X8';
	if (SQLCODE != 0)
	{
		strcpy(vTextSignZh, "【中文】");
	}
	Trim(vTextSignZh);
	
	EXEC SQL select to_number(field_value) into :vMaxItemPerDay from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '000X9';
	if (SQLCODE != 0)
	{
		vMaxItemPerDay = 0;
	}
	
	EXEC SQL select to_number(field_value) into :vMaxItemPerMon from dGrpUserMsgAdd
		where id_no = :iGrpIdNo and field_code = '00X10';
	if (SQLCODE != 0)
	{
		vMaxItemPerMon = 0;
	}
	/* 特殊数据处理完毕 */
	
	/* 查询集团产品定购时间和业务接入号 */
	EXEC SQL select c.access_number, to_char(a.op_time, 'yyyymmddhh24miss'), 
			d.user_name, a.login_no, a.op_code, c.biz_scope,c.access_model,c.biz_mode
		into :vAccessNumber, :vOpTime, :vUserName, :vLoginNo, :vOpCode, :vBizScope,:accessnumProp,:bizMode
		from dGrpSrvMsg a, sProductBizRela b, sBizModeCode c, dGrpUserMsg d
		where a.product_code = b.product_code and b.biz_code = c.biz_code and a.id_no = to_number(:iGrpIdNo) 
			and b.biz_code = :iBizCode and a.main_flag = 'Y' and a.end_time > last_day(sysdate) + 1 
			and a.id_no = d.id_no and rownum<2;
	if (SQLCODE != 0)
	{
		sprintf(oRetMsg, "ADC:查询集团产品定购时间和业务接入号失败[%d]！\n", SQLCODE);
		PUBLOG_DBDEBUG("sendECToAdcInfo");
		pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Query dGrpSrvMsg error, SQLCODE = [%d]", SQLCODE);
		return -1;
	}
	Trim(vAccessNumber);
	
	/* 插入ADC平台相关接口表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'PADC' and biz_code = :iBizCode;
	if (iCount == 1)
	{
		EXEC SQL select field_value into :vLisenceNum
			from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000XI';
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "ADC:查询集团成员数量限制失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "select dGrpUserMsgAdd Error sqlcode[%d]", SQLCODE);
			return -1;
		}

		EXEC SQL insert into dPadcEcBusi
				(loginaccept, ecid, oprnumb, oprcode, bizcode, biztype, bizname, accessmodel,
				accessnumber, price, billingtype, bizpri, bizstatus, rblist, oprefftime, provurl,
				usagedesc, introurl, lisencenum, recordtime)
			select
				:lLoginAccept, to_char(:lCustId), '0', :iOprCode, :iBizCode, biz_type, biz_name, '01',
				access_number, price, billing_type, biz_pri, biz_status, '1', sysdate, prov_url,
				usage_desc, intro_url, :vLisenceNum, sysdate
			from sBizModeCode
			where biz_code = :iBizCode;
		if (SQLCODE != 0)
		{
			sprintf(oRetMsg, "ADC:插入EC订购接口表失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Insert dPadcEcBusi Error sqlcode[%d]", SQLCODE);
			return -1;
		}

		EXEC SQL insert into dPadcEcBusiOperParam	(loginaccept, paramname, paramvalue, recordtime)
			select :lLoginAccept, c.send_name, a.field_value, to_date(:vOpTime, 'yyyymmddhh24miss')
			from dGrpUserMsgAdd a, sBizParamTarget b, sBizParamCode c
			where a.busi_type = b.param_set and a.field_code = b.param_code and b.target_code = 'PADC' 
				and a.field_code = c.param_code and a.id_no = to_number(:iGrpIdNo) and a.other_value = :iBizCode /* order by a.field_order */;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "ADC:插入EC订购附加接口表失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Insert dPadcEcBusiOperParam Error sqlcode[%d], iGrpIdNo[%s]", SQLCODE, iGrpIdNo);
			return -1;
		}

		/* 补充页面没有传回的默认参数 */
		EXEC SQL insert into dPadcEcBusiOperParam	(loginaccept, paramname, paramvalue, recordtime)
			select :lLoginAccept, c.send_name, a.default_value, to_date(:vOpTime, 'yyyymmddhh24miss')
			from sBizParamDetail a, sBizParamTarget b, sBizParamCode c, sBizModeCode d
			where a.param_code not in 
				(select field_code from dGrpUserMsgAdd where id_no = to_number(:iGrpIdNo) and other_value = :iBizCode)
				and a.param_set = b.param_set and a.param_code = b.param_code
				and a.param_set = d.param_set and a.param_code = c.param_code
				and b.target_code = 'PADC' and d.biz_code = :iBizCode;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "ADC:补充EC订购附加参数表失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "Insert dPadcEcBusiOperParam Error sqlcode[%d], iGrpIdNo[%s]", SQLCODE, iGrpIdNo);
			return -1;
		}
	}

	/* 插入行业网关相关接口表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'IAGW' and biz_code = :iBizCode;
	if (iCount == 1)
	{
		/* 000X0: 业务接入号; 000X1:计费对象; 000X2: 计费号码 */
		EXEC SQL select field_value into :vServiceNo from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X0';
		if (SQLCODE != 0)
		{
			sprintf(oRetMsg, "ADC:查询企业特服号码失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 企业特服号 Error [%d]", SQLCODE);
			return -9;
		}

		Trim(vServiceNo);
		/*
		strncpy(vEcID, vServiceNo+strlen(vAccessNumber), strlen(vServiceNo)-strlen(vAccessNumber));
		*/
		
		if(strncmp(vAccessNumber,vServiceNo,strlen(vAccessNumber)) != 0)
		{
			sprintf(oRetMsg, "IAGW:使用基本接入号错误！[%s][%s]",vAccessNumber,vServiceNo);
			printf("IAGW:使用基本接入号错误！\n");
			return -9;
		}
		
		if(strcmp(accessnumProp,"02") == 0)
		{
			if((strcmp(vServiceNo,vAccessNumber) != 0) && (strcmp(bizMode,"2") != 0))
			{
				sprintf(oRetMsg, "IAGW:彩信业务接入号必须与基本接入号相同！");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "IAGW:彩信业务业务接入号必须与基本接入号相同！", SQLCODE);
				return -9;
			}
		}

		EXEC SQL select field_value into :vFeeObject from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X1';
		if (SQLCODE != 0)
		{
			sprintf(oRetMsg, "ADC:查询计费对象失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 计费对象 Error [%d]", SQLCODE);
			return -9;
		}
	
		Trim(vFeeObject);
		strcpy(Flag,"1");
		/* 如果计费对象是集团付费，送集团虚拟号码到行业网关 */
		if (vFeeObject[0] == '1') 
		{  
			 strcpy(vFeeTerminalId,vUserNo);
		}
		else
		{
			EXEC SQL select field_value into :vFeeTerminalId from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000X2';
			if (SQLCODE != 0)
			{
				sprintf(oRetMsg, "ADC:查询计费号码失败[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 计费号码 Error [%d]", SQLCODE);
				return -9;
			}
		}

		Trim(vFeeTerminalId);

		/* 如果计费对象是特定手机号，验证该特定手机号码状态是否正常 */
		if ((vFeeObject[0] == '3') && (strcmp(iOprCode, "02") != 0))
		{
			EXEC SQL select count(*) into :iCount from dCustMsg 
				where phone_no = :vFeeTerminalId and substr(run_code,2,1) = 'A';
			if (SQLCODE != 0 || iCount == 0)
			{
				sprintf(oRetMsg, "ADC:查询特定手机号码失败[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT dCustMsg Error [%d]", SQLCODE);
				return -9;
			}
		}
		
		/*EC基本接入号*/
		strcpy(ecbizAccessNum,vServiceNo);
		
		/*MO指令*/
		EXEC SQL SELECT trim(field_value) into :moStr1 from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000Y4' AND trim(field_value)<>'*';
		if (SQLCODE != 0) 
		{
			strcpy(moStr1,"");
		}
		
		EXEC SQL SELECT trim(field_value) into :moStr2 from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000Y5' AND trim(field_value)<>'*';
		if (SQLCODE != 0) 
		{
			strcpy(moStr2,"");
		}
		
		EXEC SQL SELECT trim(field_value) into :moStr3 from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000Y6' AND trim(field_value)<>'*';
		if (SQLCODE != 0) 
		{
			strcpy(moStr3,"");
		}
		
		EXEC SQL SELECT trim(field_value) into :moStr4 from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000Y7' AND trim(field_value)<>'*';
		if (SQLCODE != 0) 
		{
			strcpy(moStr4,"");
		}
		
		EXEC SQL SELECT trim(field_value) into :moStr5 from dGrpUserMsgAdd
				where id_no = :iGrpIdNo and field_code = '000Y8' AND trim(field_value)<>'*';
		if (SQLCODE != 0) 
		{
			strcpy(moStr5,"");
		}
		
		/* 处理短信发送时间限制参数 */
		EXEC SQL select a.field_value into :vStartTime
			from dGrpUserMsgAdd a, sBizParamTarget b
			where a.busi_type = b.param_set and a.field_code = b.param_code 
				and b.target_code = 'IAGW' and id_no = to_number(:iGrpIdNo) 
				and a.other_value = :iBizCode and a.user_type = 'time' and b.param_code = '00007';
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "IAGW:查询短信发送时间失败[%d]！", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 查询短信发送时间失败 Error [%d]", SQLCODE);
			return -9;
		}
		
		EXEC SQL select a.field_value into vEndTime
			from dGrpUserMsgAdd a, sBizParamTarget b
			where a.busi_type = b.param_set and a.field_code = b.param_code 
				and b.target_code = 'IAGW' and id_no = to_number(:iGrpIdNo) 
				and a.other_value = :iBizCode and a.user_type = 'time' and b.param_code = '00008';
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "IAGW:查询短信发送时间失败[%d]！", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 查询短信发送时间失败 Error [%d]", SQLCODE);
			return -10;
		}		
		
		/* 处理行业网关其他附加参数 */
		EXEC SQL select a.field_value into :vInfoCode
			from dGrpUserMsgAdd a, sBizParamTarget b
			where a.busi_type = b.param_set and a.field_code = b.param_code 
				and b.target_code = 'IAGW' and id_no = to_number(:iGrpIdNo) 
				and a.other_value = :iBizCode and a.user_type = 'othe' and b.param_code = '00009';
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "IAGW:查询行业网关其他附加参数失败[%d]！", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 查询行业网关其他附加参数失败 Error [%d]", SQLCODE);
			return -11;
		}
		
		EXEC SQL select a.field_value into :vInfoValue
			from dGrpUserMsgAdd a, sBizParamTarget b
			where a.busi_type = b.param_set and a.field_code = b.param_code 
				and b.target_code = 'IAGW' and id_no = to_number(:iGrpIdNo) 
				and a.other_value = :iBizCode and a.user_type = 'othe' and b.param_code = '00010';
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(oRetMsg, "IAGW:查询行业网关其他附加参数失败[%d]！", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT 查询行业网关其他附加参数失败 Error [%d]", SQLCODE);
			return -12;
		}
		
		sprintf(vServCode,"%s",vServiceNo);
		Trim(vServCode);
		
		/* 插入行业网关计费接口表和流水表 01_增加, 02_取消, 05_修改 */
		if (strcmp(iOprCode, "02") == 0 || strcmp(iOprCode, "05") == 0)
		{	/*ng解耦
			EXEC SQL update dIagwServOper set end_time = sysdate
			where serv_code = :vServCode and oper_code = :iBizCode and msisdn = :vFeeTerminalId;
			if (SQLCODE != 0 && SQLCODE != 1403){
				sprintf(oRetMsg, "ADC:更新行业网关计费接口表和流水表[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "upd dIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
				return -39;
			}
			ng解耦*/
			/*ng解耦 by yaoxc begin*/
			init(sTempSqlStr);
			sprintf(sTempSqlStr,"select serv_code,oper_code,msisdn,to_char(FEE),to_char(START_TIME,'yyyymmdd hh24:mi:ss') "
								"from dIagwServOper "
			 					"where serv_code = :vServCode and oper_code = :iBizCode "
								" and msisdn = :vFeeTerminalId");
			EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
			EXEC SQL DECLARE ngcur01 cursor for sql_str;
			EXEC SQL OPEN ngcur01 using :vServCode,:iBizCode,:vFeeTerminalId;
			for(;;)
			{
				memset(&tDIAGWSERVOPERIndex,0,sizeof(tDIAGWSERVOPERIndex));
				EXEC SQL FETCH ngcur01 INTO :tDIAGWSERVOPERIndex;
				if((0!=SQLCODE) && (1403!=SQLCODE))
				{
					sprintf(oRetMsg, "查询dIagwServOper错误\n");
					EXEC SQL CLOSE ngcur01;
					return -1;
				}
				if(1403==SQLCODE) break;
					
				init(v_parameter_array);
		
				strcpy(v_parameter_array[0],vServCode);
				strcpy(v_parameter_array[1],iBizCode);
				strcpy(v_parameter_array[2],vFeeTerminalId);
				strcpy(v_parameter_array[3],tDIAGWSERVOPERIndex.sFee);
				strcpy(v_parameter_array[4],tDIAGWSERVOPERIndex.sStartTime);
    		
				v_ret=OrderUpdateDIAGWSERVOPER(ORDERIDTYPE_USER,iGrpIdNo,100,
							   			vOpCode,atol(iLoginAccept),vLoginNo,"sendECToAdcInfo",
										tDIAGWSERVOPERIndex,tDIAGWSERVOPERIndex,
										" end_time = sysdate ","",v_parameter_array);
				printf("OrderUpdateDIAGWSERVOPER ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					sprintf(oRetMsg, "ADC:更新行业网关计费接口表和流水表[%d]！\n", SQLCODE);
					PUBLOG_DBDEBUG("sendECToAdcInfo");
					pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "upd dIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
					EXEC SQL CLOSE ngcur01;
					return -39;
				}
			}
			EXEC SQL CLOSE ngcur01; 
			/*ng解耦 by yaoxc end*/
#ifdef _CHGTABLE_
		EXEC SQL insert into wIagwServOper
				(serv_code, oper_code, msisdn, fee, start_time, end_time, oper_flag, bill_type, fee_object)
			select serv_code, oper_code, msisdn, fee, start_time, sysdate,'02', bill_type, fee_object
			from dIagwServOper
			where serv_code = :vServCode and oper_code = :iBizCode and msisdn = :vFeeTerminalId;
			if (SQLCODE != 0)
			{
				sprintf(oRetMsg, "ADC:插入行业网关计费接口表和流水表[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "ins wIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
				return -59;
			}
#endif
		}

		if (strcmp(iOprCode, "01") == 0 || strcmp(iOprCode, "05") == 0)
		{	/*ng解耦
			EXEC SQL insert into dIagwServOper
				(serv_code, oper_code, msisdn, fee, start_time, end_time, bill_type, fee_object)
			select :vServCode, biz_code, :vFeeTerminalId, price,
				sysdate, to_date('2050-01-01', 'yyyy-mm-dd'), billing_type, :vFeeObject
			from sBizModeCode
			where biz_code = :iBizCode;
			if (SQLCODE != 0)
			{
				sprintf(oRetMsg, "ADC:插入行业网关计费接口表和流水表[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "ins dIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
				return -49;
			}
			ng解耦*/
			/*ng解耦 by yaoxc begin*/
			init(sTempSqlStr);
			sprintf(sTempSqlStr,"select :vServCode, biz_code, :vFeeTerminalId, to_char(price), "
								"to_char(sysdate,'yyyymmdd hh24:mi:ss'), '20500101 00:00:00', billing_type, :vFeeObject "
								"from sBizModeCode "
			 					"where biz_code = :iBizCode ");
			EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
			EXEC SQL DECLARE ngcur02 cursor for sql_str;
			EXEC SQL OPEN ngcur02 using :vServCode,:vFeeTerminalId,:vFeeObject,:iBizCode;
			for(;;)
			{
				memset(&tDIAGWSERVOPER,0,sizeof(tDIAGWSERVOPER));
				EXEC SQL FETCH ngcur02 INTO :tDIAGWSERVOPER;
				if((0!=SQLCODE) && (1403!=SQLCODE))
				{
					sprintf(oRetMsg, "查询sBizModeCode错误\n");
					EXEC SQL CLOSE ngcur02;
					return -1;
				}
				if(1403==SQLCODE) break;
    		
				v_ret=OrderInsertDIAGWSERVOPER(ORDERIDTYPE_USER,iGrpIdNo,100,
							   			vOpCode,atol(iLoginAccept),vLoginNo,"sendECToAdcInfo",
										tDIAGWSERVOPER );
				printf("OrderInsertDIAGWSERVOPER ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					sprintf(oRetMsg, "ADC:更新行业网关计费接口表和流水表[%d]！\n", SQLCODE);
					PUBLOG_DBDEBUG("sendECToAdcInfo");
					pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "upd dIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
					EXEC SQL CLOSE ngcur02;
					return -39;
				}
			}
			EXEC SQL CLOSE ngcur02; 
			/*ng解耦 by yaoxc end*/
#ifdef _CHGTABLE_		
			EXEC SQL insert into wIagwServOper
				(serv_code, oper_code, msisdn, fee, start_time, end_time, oper_flag, bill_type, fee_object)
			select serv_code, oper_code, msisdn, fee, start_time, end_time,'01', bill_type, fee_object
			from dIagwServOper
			where serv_code = :vServCode and oper_code = :iBizCode and msisdn = :vFeeTerminalId;
			if (SQLCODE != 0)
			{
				sprintf(oRetMsg, "ADC:插入行业网关计费接口表和流水表[%d]！\n", SQLCODE);
				PUBLOG_DBDEBUG("sendECToAdcInfo");
				pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "ins wIagwServOper Error [%d] [%s]", SQLCODE, SQLERRMSG);
				return -59;
			}
#endif
		}

		Trim(vStartTime);
		Trim(vEndTime);
		Trim(vInfoCode);
		Trim(vInfoValue);
		pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "vStartTime=[%s]", vStartTime);
		pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "vEndTime=[%s]", vEndTime);
		pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "vInfoCode=[%s]", vInfoCode);
		pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "vInfoValue=[%s]", vInfoValue);
				
		ret=0;
		ret=sendIagwBusiMsg(lLoginAccept,iOprCode,vIsPreCharge,vMaxItemPerDay,
					vMaxItemPerMon,vIsTextSign,vDefaultSignLang,vTextSignEn,	
					vTextSignZh,vServiceNo,vAccessNumber,ecbizAccessNum,
					vStartTime,vEndTime,vInfoCode,vInfoValue,iBizCode,
					lCustId,accessnumProp,iGrpIdNo,moStr1,moStr2,
					moStr3,moStr4,moStr5,oRetMsg);
		if(ret != 0)
		{
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "sendIagwBusiMsg error = [%d],[%s]", ret,oRetMsg);
			return -1;
		}
	}
	
	/* 调用全网ADC企业信息接口 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'GADC' and biz_code = :iBizCode;
	if (iCount == 1){
		/* 000X0: 企业特服号; 000X1:计费对象; 000X2: 计费号码 */
		EXEC SQL select field_value into :vServiceNo
			from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X0';
		if (SQLCODE != 0 || iCount == 0) {
			sprintf(oRetMsg, "ADC:查询全网ADC企业特服号失败[%d]！\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECToAdcInfo");
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "SELECT vServiceNo Error [%d]", SQLCODE);
			return -69;
		}

		EXEC SQL select org_code
				 into :vOrgCode
				 from dLoginMsg where login_no = :vLoginNo;
		if (SQLCODE != 0) {
			sprintf(oRetMsg, "ADC:查询登录工号OrgCode失败[%d]！\n", SQLCODE);
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "select vOrgCode Error [%d]", SQLCODE);
			return -79;
		}
		
		ret=0;
		ret = sendGadcECInfo(vLoginNo, vOrgCode, vUnitId, vUserName, vServiceNo, iBizCode,
			vOpTime, vIsTextSign, vDefaultSignLang, vTextSignEn, vTextSignZh, iOprCode, vOpCode, oRetMsg);
		if (ret != 0) {
			pubLog_DBErr(_FFL_, "sendECToAdcInfo", "", "sendGadcECInfo Error [%d]", ret);
			return -89;			
		}
	}
		
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendECToAdcInfo", "", "sendECToAdcInfo函数调用结束！");
#endif

	return 0;
}

/**********************************************
 @lixg FUNC : sendECMebBizInfo
 @input  params: iLoginAccept -> 操作流水
 @input  params: iGrpIdNo -> 集团用户ID
 @input  params: iBizCode -> 业务代码
 @input  params: iPhoneNo -> 手机号码
 @input  params: iUserId  -> 成员用户ID
 @input  params: iOprCode -> 操作类型(01_加入名单, 02_退出名单, 03_订购, 04_取消订购)
 @output params: 0 success else 0 fail
 *********************************************/
int sendECMebBizInfo(char *iLoginAccept, char *iGrpIdNo, char *iBizCode, char *iPhoneNo, 
	char *iUserId, char *iOprCode, char *oRetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long lLoginAccept = atol(iLoginAccept);
		long lCustId = 0.0;
		int iCount = 0;
		
		char vEcID[10+1];		
		char vBizCode[10+1];
		char vServiceNo[256+1];
		char vAccessNumber[128+1];
		char vBizName[50+1];
		char vRetCode[6+1];
		char vRetMsg[60+1];
		char vOpCode[4+1];
		char vOpTime[17+1];
		char vLoginNo[6+1];
		char vTotalDate[8+1];
		char vOrgCode[9+1];
		int	 retValue=0;
		char accessModel[2+1];
		char rbList[1+1];
		char vadcOprCode[2+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(vEcID);
	Sinitn(vBizCode);
	Sinitn(vServiceNo);
	Sinitn(vAccessNumber);
	memset(vBizName,0,sizeof(vBizName));
	memset(vRetCode,0,sizeof(vRetCode));
	memset(vRetMsg,0,sizeof(vRetMsg));
	Sinitn(vOpCode);
	Sinitn(vOpTime);
	Sinitn(vLoginNo);
	Sinitn(vTotalDate);
	Sinitn(vOrgCode);
	Sinitn(accessModel);
	Sinitn(vadcOprCode);
	Sinitn(rbList);

	/*组织结构二期改造 打印输出 by mengfy @2009/05/07 begin ----*/
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "开始调用sendECToAdcInfo函数！");
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iLoginAccept = [%s]", iLoginAccept);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iGrpIdNo = [%s]", iGrpIdNo);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iBizCode = [%s]", iBizCode);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iOprCode = [%s]", iOprCode);
	/*组织结构二期改造 打印输出 by mengfy @2009/05/07 end ----*/

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "开始调用sendECMebBizInfo函数!");
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iLoginAccept = [%s]", iLoginAccept);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iGrpIdNo = [%s]", iGrpIdNo);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iBizCode = [%s]", iBizCode);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iPhoneNo = [%s]", iPhoneNo);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iPhoneNo = [%s]", iUserId);
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "input iOprCode = [%s]", iOprCode);
#endif

	/* 查询集团信息是否存在 */
	EXEC SQL select cust_id into :lCustId from dGrpUserMsg where id_no = to_number(:iGrpIdNo);
	if (SQLCODE != 0){
		sprintf(oRetMsg, "Query dGrpUserMsg error, SQLCODE = [%d]\n", SQLCODE);
		PUBLOG_DBDEBUG("sendECMebBizInfo");
		pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Query dGrpUserMsg error, SQLCODE = [%d]", SQLCODE);
		return -1;
	}
	
	/* 查询集团产品定购时间和业务接入号 */
	EXEC SQL select c.access_number, to_char(a.op_time, 'yyyymmddhh24miss'), a.op_code, 
			a.login_no, to_char(sysdate, 'yyyymmdd'),c.rb_list
		into :vAccessNumber, :vOpTime, :vOpCode, :vLoginNo, :vTotalDate,:rbList
		from dGrpSrvMsg a, sProductBizRela b, sBizModeCode c, dGrpUsermsg d
		where a.product_code = b.product_code and b.biz_code = c.biz_code and a.id_no = to_number(:iGrpIdNo)
			and b.biz_code = :iBizCode and a.main_flag = 'Y' and a.end_time > last_day(sysdate) + 1
			and a.id_no = d.id_no and rownum<2;
	if (SQLCODE != 0){
		sprintf(oRetMsg, "Query dGrpSrvMsg error, SQLCODE = [%d]\n", SQLCODE);
		PUBLOG_DBDEBUG("sendECMebBizInfo");
		pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Query dGrpSrvMsg error, SQLCODE = [%d]", SQLCODE);
		return -1;
	}
	Trim(vAccessNumber);

	/* 插入ADC签约关系表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'PADC' and biz_code = :iBizCode;
	if (iCount == 1){
		if(strcmp(rbList,"0") == 0)
		{
			if(strcmp(iOprCode,"01") == 0)
			{
				strcpy(vadcOprCode,"03");
			}
			else
			{
				strcpy(vadcOprCode,"04");
			}
		}
		else
		{
			strcpy(vadcOprCode,iOprCode);
		}
		
		
		EXEC SQL insert into dPadcSign
			(loginaccept, pkgseq, mobnum, oprnumb, oprcode, efft, bizcode, ecid,
			 recordtime, dealflag, dealtime, trytimes)
		values
			(:lLoginAccept, 0, :iPhoneNo, '0', :vadcOprCode, sysdate, :iBizCode, to_char(:lCustId),
			sysdate, '0', sysdate, '0');
		if (SQLCODE != 0){
			sprintf(oRetMsg, "Insert dPadcSign error =[%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Insert dPadcSign error =[%d]", SQLCODE);
			return -1;
		}
		
		/* 添加成员附加信息 */
		EXEC SQL insert into dPadcSignParam
				(loginaccept, mobnum, paramname, paramvalue, recordtime)
			select :lLoginAccept, :iPhoneNo, b.send_name, a.param_value, sysdate
			from dgrpusermebmsgadd a, sBizParamCode b, sBizParamTarget c, sBizModeCode d
			where a.param_code = b.param_code and b.param_code = c.param_code and c.target_code = 'PADC'
	  		and c.param_set = d.mparam_set and a.other_value = d.biz_code
	  		and a.id_no = :iGrpIdNo and a.user_id = :iUserId and d.biz_code = :iBizCode;
			/* 现在要求附加资费传给ADC平台 */
		if (SQLCODE != 0){
			sprintf(oRetMsg, "Insert dPadcSign error =[%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Insert dPadcSign error =[%d]", SQLCODE);
			return -1;
		}
	}

	/* 插入行业网关签约关系表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'IAGW' and biz_code = :iBizCode;
	if (iCount == 1){
		EXEC SQL select nvl(field_value, ' ') into :vServiceNo
			from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X0';
		if (SQLCODE != 0 && SQLCODE != 1403){
			sprintf(oRetMsg, "Query dGrpUserMsgAdd error, SQLCODE = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Query dGrpUserMsgAdd error, SQLCODE = [%d]", SQLCODE);
			return -1;
		}
		Trim(vServiceNo);
		
		/* 根据服务号（长号码）和接入号（短号码）计算ECID（送行业网关） */
		strncpy(vEcID, vServiceNo+strlen(vAccessNumber), strlen(vServiceNo)-strlen(vAccessNumber));

		/*retValue=0;
		retValue=sendIagwSignMsg(lLoginAccept,iPhoneNo,iOprCode,iBizCode,vEcID,vAccessNumber,oRetMsg);
		if(retValue != 0)
		{
			pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "sendIagwSignMsg error = [%d],[%s]", retValue,oRetMsg);
			return -1;
		}*/
		
		EXEC SQL SELECT nvl(ACCESS_MODEL,'01') INTO :accessModel FROM sbizmodecode WHERE biz_code=:iBizCode;
		
		EXEC SQL insert into tiagwsignsyn
				(
				 maxaccept,MobNum,ServCode,ServCodeProp,BizCode,
				 OprCode,EfftT,DOMAIN,recordtime,dealflag,dealtime
				)
			values
				(
				 :lLoginAccept,:iPhoneNo,:vServiceNo,:accessModel,:iBizCode,
				 :iOprCode,to_char(sysdate,'yyyymmddhh24miss'),'0', sysdate, '0',sysdate
				);
		if (SQLCODE != 0){
			sprintf(oRetMsg, "Insert dIagwSign error = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Insert dIagwSign error = [%d]", SQLCODE);
			return -1;
		}
	}
	
	/* 调用PUSHMAIL接口服务 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'MAIL' and biz_code = :iBizCode;
	if (iCount == 1){
		EXEC SQL select org_code
				 into :vOrgCode
				 from dLoginMsg 
				 where login_no = :vLoginNo;
		if (SQLCODE != 0) {
			sprintf(oRetMsg, "select vOrgCode Error [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "select vOrgCode Error [%d]", SQLCODE);
			return -1;
		}
		
		sprintf(vRetCode, "%06d", sendPushmailInfo(iLoginAccept, vLoginNo, vOrgCode, 
			vTotalDate, iPhoneNo, vOpCode, vOpTime, iOprCode, vRetMsg));
	  if (strcmp(vRetCode, "000000") != 0) {
		  	sprintf(oRetMsg, vRetMsg);
		  	pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "sendPushmailInfo Error [%s]", vRetCode);
		  	return -1;
		}
	}
	
	/* 插入全网MAS签约关系表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'GMAS' and biz_code = :iBizCode;
	if (iCount == 1){
		EXEC SQL select nvl(field_value, ' ') into :vServiceNo
			from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X0';
		if (SQLCODE != 0 && SQLCODE != 1403){
			sprintf(oRetMsg, "Query dGrpUserMsgAdd error, SQLCODE = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Query dGrpUserMsgAdd error, SQLCODE = [%d]", SQLCODE);
			return -1;
		}
		Trim(vServiceNo);
		
		/* 根据服务号（长号码）和接入号（短号码）计算ECID（送行业网关） */
		strncpy(vEcID, vServiceNo+strlen(vAccessNumber), strlen(vServiceNo)-strlen(vAccessNumber));
		
		EXEC SQL insert into dMasSign 
   		(loginaccept, bizcode, prodname, servcode, mobnum, oprcode,
     	efft, recordtime, dealflag, dealtime, domain)
		select
			:lLoginAccept, :iBizCode, biz_name, :vServiceNo, :iPhoneNo, :iOprCode,
			to_char(sysdate, 'YYYYMMDD'), sysdate, '0', sysdate, '0'
			from sBizModeCode where biz_code = :iBizCode;
		if (SQLCODE != 0) {
			sprintf(oRetMsg, "insert into dMasSign error = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
		    pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "insert into dMasSign error = [%d]", SQLCODE);
      		return -1;
		}
	}

	/* 插入全网ADC签约关系表 */
	EXEC SQL select count(*) into :iCount from sBizModeTarget
		where target_code = 'GADC' and biz_code = :iBizCode;
	if (iCount == 1){
		EXEC SQL select nvl(field_value, ' ') into :vServiceNo
			from dGrpUserMsgAdd
			where id_no = :iGrpIdNo and field_code = '000X0';
		if (SQLCODE != 0 && SQLCODE != 1403){
			sprintf(oRetMsg, "Query dGrpUserMsgAdd error, SQLCODE = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "Query dGrpUserMsgAdd error, SQLCODE = [%d]", SQLCODE);
			return -1;
		}
		Trim(vServiceNo);
		
		EXEC SQL insert into tAdcSignMsg
				(maxaccept, loginaccept, pkgseq, mobnum, oprnumb, oprcode, efftt, bizcode, 
				servcode, domain, recordtime, dealflag, dealreslut, dealtime, trytimes)
			values 
				(:lLoginAccept, :lLoginAccept, '0', :iPhoneNo, ' ', :iOprCode, :vTotalDate, :iBizCode,
				:vServiceNo, '0', sysdate, '0', ' ', sysdate, 0);
		if (SQLCODE != 0) {
			sprintf(oRetMsg, "insert into tAdcSignMsg error = [%d]\n", SQLCODE);
			PUBLOG_DBDEBUG("sendECMebBizInfo");
			pubLog_DBErr(_FFL_, "sendECMebBizInfo", "", "insert into tAdcSignMsg error = [%d]", SQLCODE);
			return -1;
		}
	}
	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendECMebBizInfo", "", "sendECMebBizInfo函数调用结束！");
#endif

	return 0;
}

/**********************************************
 @lixg FUNC : sendPushmailInfo
 @input  params: iLoginAccept -> 操作流水
 @input  params: iGrpIdNo -> 集团用户ID
 @input  params: iBizCode -> 业务代码
 @input  params: iPhoneNo -> 手机号码
 @input  params: iUserId  -> 成员用户ID
 @input  params: iOprCode -> 操作类型(01_加入名单, 02_退出名单, 03_订购, 04_取消订购)
 @output params: 0 success else 0 fail
 *********************************************/
int sendPushmailInfo(char *iLoginAccept, char *iLoginNo, char *iOrgCode, char *iTotalDate, 
	char *iPhoneNo, char *iOpCode, char *iOpTime, char *iOprCode, char *oRetMsg)
{
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	int i = 0, ilen = 0, olen = 0, ret = 0;
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

	FBFR32 *transIN_tp = NULL;
	FBFR32 *transOUT_tp = NULL;
	FLDLEN32 len;                /*输出缓存区分配的空间*/
	char temp_buf[61];       /*临时使用的字符串数组*/

	EXEC SQL BEGIN DECLARE SECTION;
		char vRuleName[40+1];
		char vRuleParam[255+1];
		char *tokStr[10];
		char retCode[6+1];
		char retMsg[60+1];
	EXEC SQL END DECLARE SECTION;
	
	init(retCode);
	init(retMsg);
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	memset(temp_buf,0,sizeof(temp_buf));
	memset(vRuleName,0,sizeof(vRuleName));
	memset(vRuleParam,0,sizeof(vRuleParam));
	memset(&tokStr,0,sizeof(tokStr));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

	/* 处理pushmail业务规则 */
	EXEC SQL DECLARE cur CURSOR for
			select b.rule_name, nvl(a.rule_param, ' ') from sBusiSmRuleRela a, sRule b
			where a.busi_type = '1000' and a.sm_code = 'pm' and a.rule_code = b.rule_code;
	EXEC SQL OPEN cur;
	for (;;) {
		init(vRuleName);
		init(vRuleParam);
		
		EXEC SQL FETCH cur INTO :vRuleName, :vRuleParam;
		if (SQLCODE != 0) break;

		Trim(vRuleName);
		Trim(vRuleParam);
		
		ret = gettok(vRuleParam, "|", tokStr);
		if (ret != 0) {
			sprintf(oRetMsg, "解析串vRuleParam失败[%d]!\n", ret);
			return -1001;
		}
	
		for (i=0; i<10; i++) {
			if (tokStr[i] == NULL || strlen(tokStr[i]) == 0) break;
			Trim(tokStr[i]);
			
			ret = sendCmd(tokStr[i], 1, iPhoneNo, oRetMsg, iOpTime, iLoginNo, iOpCode, iOrgCode, iLoginAccept, iTotalDate);
			if (ret != 0) {
				sprintf(oRetMsg, "调用sendCmd失败[%d][%s]!\n", ret, oRetMsg);
				return -1002;
			}
		}
	}
	EXEC SQL CLOSE cur;

	Trim(iLoginNo);
	Trim(iOrgCode);		
	Trim(iTotalDate);
	Trim(iPhoneNo);
	Trim(iOpTime);
	
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	transIN_tp = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transIN_tp == (FBFR32 *)NULL) {
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s4230Snd, size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
	transOUT_tp = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(transOUT_tp == (FBFR32 *)NULL) {
		tpfree((char *)transIN_tp);
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s4230Snd, size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}

	/* 准备输入参数 */
	Fchg32(transIN_tp, SEND_PARMS_NUM32, 0, "22", (FLDLEN32)0);/**调用sOBProxySrv参数个数**/
	Fchg32(transIN_tp, RECP_PARMS_NUM32, 0, "3",  (FLDLEN32)0);/**sOBProxySrv返回参数个数**/
	Fchg32(transIN_tp, GPARM32_0,  0, "s4230Snd", (FLDLEN32)0);/**服务sOBProxySrv调用一级BOSS接口服务名称**/
	Fchg32(transIN_tp, GPARM32_1,  0, "19",       (FLDLEN32)0);/**一级BOSS接口服务s4230Snd请求参数个数**/
	Fchg32(transIN_tp, GPARM32_2,  0, "3",        (FLDLEN32)0);/**一级BOSS接口服务s4230Snd返回参数个数**/
	Fchg32(transIN_tp, GPARM32_3,  0, iLoginNo,   (FLDLEN32)0);/**营业员工号**/
	Fchg32(transIN_tp, GPARM32_4,  0, iOrgCode,   (FLDLEN32)0);/**营业员归属**/
	Fchg32(transIN_tp, GPARM32_5,  0, "4230",     (FLDLEN32)0);/**一级BOSS功能代码,此功能点默认为4230**/
	Fchg32(transIN_tp, GPARM32_6,  0, iTotalDate, (FLDLEN32)0);/**帐务日期 格式:yyyymmdd**/
	Fchg32(transIN_tp, GPARM32_7,  0, "01",       (FLDLEN32)0);/**标识类型 01：手机号码**/
	Fchg32(transIN_tp, GPARM32_8,  0, iPhoneNo,   (FLDLEN32)0);/**标识值(手机号码)**/
	Fchg32(transIN_tp, GPARM32_9,  0, iOprCode,   (FLDLEN32)0);/**操作类型：01:注册 02：注销 03：密码修改 04：暂停 05：恢复 06：订购 07：取消**/
	Fchg32(transIN_tp, GPARM32_10, 0, "15",       (FLDLEN32)0);/**业务类型：15：pushmail**/
	Fchg32(transIN_tp, GPARM32_11, 0, "",         (FLDLEN32)0);/****密码**/
	Fchg32(transIN_tp, GPARM32_12, 0, iOpTime,    (FLDLEN32)0);/**操作时间**/
	Fchg32(transIN_tp, GPARM32_13, 0, "",         (FLDLEN32)0);/**SP企业代码 增值业务订购(06)和取消(07)时使用**/
	Fchg32(transIN_tp, GPARM32_14, 0, "",         (FLDLEN32)0);/**SP业务代码**/
	Fchg32(transIN_tp, GPARM32_15, 0, "300",      (FLDLEN32)0);/**信息代码 300 pushmail信息代码**/
	Fchg32(transIN_tp, GPARM32_16, 0, "01",       (FLDLEN32)0);/**信息值：01:集团pushmail 02:个人pushmail**/
	Fchg32(transIN_tp, GPARM32_17, 0, "0",        (FLDLEN32)0); /**用户状态 可选**/
	Fchg32(transIN_tp, GPARM32_18, 0, iOpTime,    (FLDLEN32)0);/**变更时间 **/
	Fchg32(transIN_tp, GPARM32_19, 0, "0",        (FLDLEN32)0);/**其它状态 可选**/
	Fchg32(transIN_tp, GPARM32_20, 0, "431",      (FLDLEN32)0);/**省代码 吉林431**/
	Fchg32(transIN_tp, GPARM32_21, 0, "开通",     (FLDLEN32)0);/**操作日志**/
	
	ilen = Fsizeof32(transIN_tp);
	olen = Fsizeof32(transOUT_tp);
		
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendPushmailInfo", "", "调用服务s4230Snd开始!");
#endif

	/* 通过sOBProxySrv调用pushmail接口 */
	/*
	ret = tpcall("sOBProxySrv",(char *)transIN_tp,(long)&ilen,(char **)&transOUT_tp,(long *)&olen,0);
	*/
	ret = tpcall("sOBProxySrv",(char *)transIN_tp, 0,(char **)&transOUT_tp,(long *)&olen,0);
	if (ret != 0) {
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		sprintf(oRetMsg, "调用s4230Snd出错[%d]!", ret);
		return -1003;
	}
	  
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendPushmailInfo", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
#endif

	Fget32(transOUT_tp, SVC_ERR_NO32, 0, retCode, (FLDLEN32)0);
	Fget32(transOUT_tp, SVC_ERR_MSG32, 0, retMsg, (FLDLEN32)0);

	if (atol(retCode) != 0) {
		sprintf(oRetMsg, "调用s4230Snd错误[%s][%s]!", retCode, retMsg);
		pubLog_DBErr(_FFL_, "sendPushmailInfo", "", "调用s4230Snd error retCode=[%s]retMsg=[%s]",retCode,retMsg);
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return -1004;
	}
	
	tpfree((char *)transIN_tp);
	tpfree((char *)transOUT_tp);
	return 0;
}

/**********************************************
 @EC订购信息同步
 @lixg FUNC : sendGadcECInfo
 @input  params: iLoginAccept -> 操作流水
 @input  params: iGrpIdNo -> 集团用户ID
 @input  params: iBizCode -> 业务代码
 @input  params: iPhoneNo -> 手机号码
 @input  params: iUserId  -> 成员用户ID
 @input  params: iOprCode -> 操作类型(01_加入名单, 02_退出名单, 03_订购, 04_取消订购)
 @output params: 0 success else 0 fail
 *********************************************/
int sendGadcECInfo(char *iLoginNo, char *iOrgCode, char *iCustId,	char *iCustName, 
	char *iServiceNo, char *iBizCode, char *iOpTime, char *iIsTextSign, char *iDefaultSign, 
	char *iTextSignEn, char *iTextSignZh, char *iOprCode, char *iOpCode, char *oRetMsg)
{
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	int i = 0, ilen = 0, olen = 0, ret = 0;
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

	FBFR32 *transIN_tp = NULL;
	FBFR32 *transOUT_tp = NULL;
	FLDLEN32 len;					/*输出缓存区分配的空间*/
	char temp_buf[61];       		/*临时使用的字符串数组*/
  
	EXEC SQL BEGIN DECLARE SECTION;
		char retCode[6+1];			/* 返回代码 */
		char retMsg[61];			/* 错误信息 */
		char vTmpMsg[30+1];
	EXEC SQL END DECLARE SECTION;
	
	init(retCode);
	init(retMsg);
	init(vTmpMsg);
	
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	memset(temp_buf,0,sizeof(temp_buf));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

	/*EC订购信息同步*/
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	transIN_tp=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transIN_tp == (FBFR32 *)NULL) {
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s424iSnd,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
	
	transOUT_tp=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transOUT_tp == (FBFR32 *)NULL) {
		tpfree((char *)transIN_tp);
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s424iiSnd,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
	
	Trim(iServiceNo);
	Trim(iOpTime);
	Trim(iCustId);
	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "----iServiceNo iOpTime = [%s][%s]", iServiceNo, iOpTime);
	Fchg32(transIN_tp, SEND_PARMS_NUM32, 0, "17",   (FLDLEN32)0);
	Fchg32(transIN_tp, RECP_PARMS_NUM32, 0, "4",    (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_0,  0, "s424iSnd",   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_1,  0, "14",         (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_2,  0, "4",          (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_3,  0, iLoginNo,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_4,  0, iOrgCode,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_5,  0, "424i",       (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_6,  0, iCustId,      (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_7,  0, iOprCode,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_8,  0, iBizCode,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_9,  0, iServiceNo,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_10, 0, "5",          (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_11, 0, iOpTime,      (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_12, 0, "",          	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_13, 0, iIsTextSign,  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_14, 0, iDefaultSign, (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_15, 0, iTextSignEn,  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_16, 0, iTextSignZh,  (FLDLEN32)0);
	
	ilen = Fsizeof32(transIN_tp);
	olen = Fsizeof32(transOUT_tp);
	
	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "调用服务s424iSnd开始!");

	/*
	ret = tpcall("sOBProxySrv",(char *)transIN_tp,(long)&ilen,(char **)&transOUT_tp,(long *)&olen,0);
	*/
	ret = tpcall("sOBProxySrv",(char *)transIN_tp, 0,(char **)&transOUT_tp,(long *)&olen,0);
	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "ret=[%d]",ret);
	sprintf(retCode,"%06d", ret);
	if (ret == -1) {
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		strcpy(oRetMsg, "ADC:sOBProxySrv服务调用失败！");
		return 1004;
	}	else if(ret != 0) {
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return 1005;
	}
	  
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
#endif

	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "调用服务s424iSnd结束!");
	
	Fget32(transOUT_tp,SVC_ERR_NO32,0,retCode,(FLDLEN32)0);
	Fget32(transOUT_tp,SVC_ERR_MSG32,0,retMsg,(FLDLEN32)0);
	
	pubLog_Debug(_FFL_, "sendGadcECInfo", "", "retCode[%s]",retCode);
	if (atol(retCode) != 0) {
		EXEC SQL select detail_name into :vTmpMsg from sBizListCode
			where list_code = 's424iSnd' and detail_code = :retCode;
		strcpy(oRetMsg, vTmpMsg);
		pubLog_DBErr(_FFL_, "sendGadcECInfo", "", "--s424iSnd-- retCode=[%s][%s][%s][%d]", retCode, retMsg, vTmpMsg, SQLCODE);
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return 1006;
	}
	
	tpfree((char *)transIN_tp);
	tpfree((char *)transOUT_tp);
	
	return 0;
}


/**********************************************
 @EC信息同步
 @lixg FUNC : sendGadcECMsg
 @input  params: iLoginAccept -> 操作流水
 @input  params: iGrpIdNo -> 集团用户ID
 @input  params: iBizCode -> 业务代码
 @input  params: iPhoneNo -> 手机号码
 @input  params: iUserId  -> 成员用户ID
 @input  params: iOprCode -> 操作类型(01_加入名单, 02_退出名单, 03_订购, 04_取消订购)
 @output params: 0 success else 0 fail
 *********************************************/
int sendGadcECMsg(char *iLoginNo, char *iOrgCode, char *iCustId,char *iCustName, 
	char *iServiceNo,char *ECTel,char *ECManName,char *ECFax,char *iOprCode,char *oRetMsg)
{
	int i, ilen, olen, ret;

	FBFR32 *transIN_tp = NULL;
	FBFR32 *transOUT_tp = NULL;
	FLDLEN32 len;                /*输出缓存区分配的空间*/
	char temp_buf[61];       /*临时使用的字符串数组*/
  
	EXEC SQL BEGIN DECLARE SECTION;
		char retCode[6+1]; /* 返回代码 */
		char retMsg[61];  /* 错误信息 */
		char vTmpMsg[30+1];
	EXEC SQL END DECLARE SECTION;
	
	init(retCode);
	init(retMsg);
	init(vTmpMsg);

	/*EC企业信息同步*/
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	transIN_tp = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transIN_tp == (FBFR32 *)NULL) {
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s424hSnd,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}

	transOUT_tp=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transOUT_tp == (FBFR32 *)NULL) {
		tpfree((char *)transIN_tp);
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in s424hSnd,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
		
	Trim(iLoginNo);
	Trim(iOrgCode);
	Trim(iCustId);
	Trim(iCustName);
	Trim(iServiceNo);
	init(temp_buf);
	Trim(ECTel);
	Trim(ECManName);
	Trim(ECFax);
	
	/*strncpy(temp_buf, iServiceNo, strlen(iServiceNo)-3);*/
	pubLog_Debug(_FFL_, "sendGadcECMsg", "", "--temp_buf-- = [%s][%s]", temp_buf, iCustId);
	
	Fchg32(transIN_tp, SEND_PARMS_NUM32, 0, "14", (FLDLEN32)0);
	Fchg32(transIN_tp, RECP_PARMS_NUM32, 0, "4",  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_0,  0, "s424hSnd", (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_1,  0, "11",       (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_2,  0, "4",        (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_3,  0, iLoginNo,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_4,  0, iOrgCode,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_5,  0, "424h",     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_6,  0, "431",      (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_7,  0, iCustId,    (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_8,  0, iCustName,  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_9,  0, iServiceNo, (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_10, 0, iOprCode,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_11, 0, ECTel,      (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_12, 0, ECManName,  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_13, 0, ECFax,      (FLDLEN32)0);
	
	ilen = Fsizeof32(transIN_tp);
	olen = Fsizeof32(transOUT_tp);
	
	pubLog_Debug(_FFL_, "sendGadcECMsg", "", "调用服务s424hSnd开始!");

	/*
	ret = tpcall("sOBProxySrv",(char *)transIN_tp,(long)&ilen,(char **)&transOUT_tp,(long *)&olen,0);
	*/
	ret = tpcall("sOBProxySrv",(char *)transIN_tp, 0, (char **)&transOUT_tp,(long *)&olen,0);
	pubLog_Debug(_FFL_, "sendGadcECMsg", "", "ret=[%d]",ret);
	sprintf(retCode,"%06d", ret);
	if (ret == -1) {
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		strcpy(oRetMsg, "sOBProxySrv服务调用失败！");
		return 1001;
	} else if(ret != 0) {
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return 1002;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendGadcECMsg", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
#endif

	pubLog_Debug(_FFL_, "sendGadcECMsg", "", "调用服务s424hSnd结束!");
	
	Fget32(transOUT_tp,SVC_ERR_NO32,0,retCode,(FLDLEN32)0);
	Fget32(transOUT_tp,SVC_ERR_MSG32,0,retMsg,(FLDLEN32)0);
	if (atol(retCode) != 0) {
		EXEC SQL select detail_name into :vTmpMsg from sBizListCode
			where list_code = 's424hSnd' and detail_code = :retCode;
		pubLog_DBErr(_FFL_, "sendGadcECMsg", "", "--s424hSnd-- retCode=[%s][%s][%s][%d]", retCode, retMsg, vTmpMsg, SQLCODE);
		strcpy(oRetMsg, vTmpMsg);
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return 1003;
	}
	
	tpfree((char *)transIN_tp);
	tpfree((char *)transOUT_tp);
	
	return 0;
}

/**********************************************
 @ADC，MAS成员订购信息同步
 @FUNC : sendIagwSignMsg
 @input  params: loginAccept -> 操作流水
 @input  params: phoneNo -> 手机号码
 @input  params: oprCode -> 操作类型(01_加入名单, 02_退出名单, 03_订购, 04_取消订购)
 @input  params: bizCode -> 业务代码
 @input  params: EcId  -> 企业扩展码
 @input  params: accessNumber -> 基本接入号
 @output params: 0 success else 0 fail
 *********************************************/
int sendIagwSignMsg(long vloginAccept,char *vphoneNo,char *voprCode,char *vbizCode,char *vEcId,char *vaccessNumber,char *voRetMsg)
{
	int		i=0,ilen=0,olen=0,ret=0;

	FBFR32 *transIN_tp = NULL;
	FBFR32 *transOUT_tp = NULL;
	FLDLEN32 len;                /*输出缓存区分配的空间*/
	char temp_buf[61];       /*临时使用的字符串数组*/
  
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	loginAccept[14+1];
	char	phoneNo[15+1];
	char	oprCode[2+1];
	char	bizCode[10+1];
	char	EcId[10+1];
	char	accessNumber[128+1];
	char	retCode[6+1]; /* 返回代码 */
	char	retMsg[128+1];  /* 错误信息 */
	
	EXEC SQL END DECLARE SECTION;
	
	init(loginAccept);
	init(phoneNo);
	init(oprCode);
	init(bizCode);
	init(EcId);
	init(accessNumber);
	init(retCode);
	init(retMsg);
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	memset(temp_buf,0,sizeof(temp_buf));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/
	
	sprintf(loginAccept,"%ld",vloginAccept);
	strcpy(phoneNo,vphoneNo);
	strcpy(oprCode,voprCode);
	strcpy(bizCode,vbizCode);
	strcpy(EcId,vEcId);
	strcpy(accessNumber,vaccessNumber);
	
	Trim(loginAccept);
	Trim(phoneNo);
	Trim(oprCode);
	Trim(bizCode);
	Trim(EcId);
	Trim(accessNumber);
	
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	transIN_tp = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transIN_tp == (FBFR32 *)NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in sIagwMebInfoCfm,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}

	transOUT_tp=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transOUT_tp == (FBFR32 *)NULL)
	{
		tpfree((char *)transIN_tp);
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in sIagwMebInfoCfm,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
	
	Fchg32(transIN_tp, SEND_PARMS_NUM32, 0, "6", (FLDLEN32)0);
	Fchg32(transIN_tp, RECP_PARMS_NUM32, 0, "2", (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_0,  0, loginAccept,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_1,  0, phoneNo,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_2,  0, oprCode,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_3,  0, bizCode,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_4,  0, EcId,		 (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_5,  0, accessNumber,(FLDLEN32)0);
	
	ilen = Fsizeof32(transIN_tp);
	olen = Fsizeof32(transOUT_tp);
	
	pubLog_Debug(_FFL_, "sendIagwSignMsg", "", "调用服务sIagwMebInfoCfm开始!");

	/*
	ret = tpcall("sIagwMebInfoCfm",(char *)transIN_tp,(long)&ilen,(char **)&transOUT_tp,(long *)&olen,0);
	*/
	ret = tpcall("sIagwMebInfoCfm",(char *)transIN_tp, 0,(char **)&transOUT_tp,(long *)&olen,0);
	pubLog_Debug(_FFL_, "sendIagwSignMsg", "", "ret=[%d]",ret);
	sprintf(retCode,"%06d", ret);
	if (ret == -1)
	{
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		strcpy(voRetMsg, "sIagwMebInfoCfm服务调用失败！");
		return -1;
	}
	else if(ret != 0 && ret != -1)
	{
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return -2;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendIagwSignMsg", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
#endif

	pubLog_Debug(_FFL_, "sendIagwSignMsg", "", "调用服务sIagwMebInfoCfm结束!");
	
	Fget32(transOUT_tp,SVC_ERR_NO32,0,retCode,(FLDLEN32)0);
	Fget32(transOUT_tp,SVC_ERR_MSG32,0,retMsg,(FLDLEN32)0);
	if (atol(retCode) != 0) 
	{
		pubLog_DBErr(_FFL_, "sendIagwSignMsg", "", "--sIagwMebInfoCfm-- retCode=[%s][%s]", retCode, retMsg);
		strcpy(voRetMsg,retMsg);
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return -3;
	}
	
	tpfree((char *)transIN_tp);
	tpfree((char *)transOUT_tp);
	
	return 0;
}

/**********************************************
 @ADC，MAS EC订购信息同步
 @FUNC : sendIagwBusiMsg
 @input  params: 	loginAccept		流水                  
 @input  params: 	oprCode			操作代码              
 @input  params: 	preCharge		--                    
 @input  params: 	maxItemperDay	--                    
 @input  params: 	maxItemperMon	--                    
 @input  params: 	testSign		签名类型              
 @input  params: 	defaultSignlang	--                
 @input  params: 	testSignEn		英文签名              
 @input  params: 	testSignZh		中文签名              
 @input  params: 	EcId			集团扩展位            
 @input  params: 	FeeObject		计费类型              
 @input  params: 	Flag			计费号码标志          
 @input  params: 	FeeTermalId		计费号码              
 @input  params: 	stratrTime		信发送时间限制参数串  
 @input  params: 	endTime			信发送时间限制参数值串
 @input  params: 	infoCode		附加参数代码串        
 @input  params: 	infoValue		附加参数值串          
 @input  params: 	bizCode			业务代码              
 @output params: 0 success else 0 fail
 *********************************************/
int sendIagwBusiMsg(long vloginAccept,char *voprCode,char *vpreCharge,int vmaxItemperDay,
					int vmaxItemperMon,char *vtestSign,char *vdefaultSignlang,char *vtestSignEn,	
					char *vtestSignZh,char *vserviceNo,char *vAccessNumber,char *ecbizAccessNum,
					char *vstratrTime,char *vendTime,
					char *vinfoCode,char *vinfoValue,char *vbizCode,long lCustId,
					char *accessnumProp,char *vGrpIdNo,char *vmoStr1,
					char *vmoStr2,char *vmoStr3,char *vmoStr4,char *vmoStr5,char *voRetMsg)
{                
	int		i=0,ilen=0,olen=0,ret=0;
                 
	FBFR32 *transIN_tp = NULL;
	FBFR32 *transOUT_tp = NULL;
	FLDLEN32 len;				/*输出缓存区分配的空间*/
	char temp_buf[61];       	/*临时使用的字符串数组*/
  
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	loginAccept[14+1];
	char	oprCode[2+1];
	char	preCharge[1+1];
	char	maxItemperDay[8+1];
	char	maxItemperMon[8+1];
	char	testSign[1+1];
	char	defaultSignlang[1+1];
	char	testSignEn[20+1];
	char	testSignZh[20+1];
	char	stratrTime[255+1];
	char	endTime[255+1];
	char	infoCode[255+1];
	char	infoValue[255+1];
	char	bizCode[10+1];
	char	iserviceNo[128+1];   /*业务接入号*/
	char	iAccessNumber[21+1]; /*ＳＩ基本接入号*/
	char	iecbizAccessNum[21+1];/*ec基本接入号*/
	char	ilCustId[14+1];
	char	iaccessnumProp[2+1];
	char	iGrpIdNo[14+1];
	char	retCode[6+1]; /* 返回代码 */
	char	retMsg[128+1];  /* 错误信息 */
	char	imoStr1[256+1];
	char	imoStr2[256+1];
	char	imoStr3[256+1];
	char	imoStr4[256+1];
	char	imoStr5[256+1];
	
	EXEC SQL END DECLARE SECTION;
	
	init(loginAccept);
	init(oprCode);
	init(preCharge);
	init(maxItemperDay);
	init(maxItemperMon);
	init(testSign);
	init(defaultSignlang);
	init(testSignEn);
	init(testSignZh);
	init(stratrTime);
	init(endTime);
	init(infoCode);
	init(infoValue);
	init(bizCode);
	init(iserviceNo);
	init(iAccessNumber);
	init(iecbizAccessNum);
	init(ilCustId);
	init(iaccessnumProp);
	init(iGrpIdNo);
	init(retCode);
	init(retMsg);
	init(imoStr1);
	init(imoStr2);
	init(imoStr3);
	init(imoStr4);
	init(imoStr5);
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
	memset(temp_buf,0,sizeof(temp_buf));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/
	
	sprintf(loginAccept,"%ld",vloginAccept);
	strcpy(oprCode,voprCode);
	strcpy(preCharge,vpreCharge);
	sprintf(maxItemperDay,"%d",vmaxItemperDay);
	sprintf(maxItemperMon,"%d",vmaxItemperMon);
	strcpy(testSign,vtestSign);
	strcpy(defaultSignlang,vdefaultSignlang);
	strcpy(testSignEn,vtestSignEn);
	strcpy(testSignZh,vtestSignZh);
	strcpy(stratrTime,vstratrTime);
	strcpy(endTime,vendTime);
	strcpy(infoCode,vinfoCode);
	strcpy(infoValue,vinfoValue);
	strcpy(bizCode,vbizCode);
	strcpy(iserviceNo,vserviceNo);
	strcpy(iAccessNumber,vAccessNumber);
	strcpy(iecbizAccessNum,ecbizAccessNum);
	sprintf(ilCustId,"%ld",lCustId);
	strcpy(iaccessnumProp,accessnumProp);
	strcpy(iGrpIdNo,vGrpIdNo);
	strcpy(imoStr1,vmoStr1);
	strcpy(imoStr2,vmoStr2);
	strcpy(imoStr3,vmoStr3);
	strcpy(imoStr4,vmoStr4);
	strcpy(imoStr5,vmoStr5);
	
	Trim(loginAccept);
	Trim(oprCode);
	Trim(preCharge);
	Trim(maxItemperDay);
	Trim(maxItemperMon);
	Trim(testSign);
	Trim(defaultSignlang);
	Trim(testSignEn);
	Trim(testSignZh);
	Trim(stratrTime);
	Trim(endTime);
	Trim(infoCode);
	Trim(infoValue);
	Trim(bizCode);
	Trim(iserviceNo);
	Trim(iAccessNumber);
	Trim(iecbizAccessNum);
	Trim(ilCustId);
	Trim(iaccessnumProp);
	Trim(iGrpIdNo);
	Trim(imoStr1);
	Trim(imoStr2);
	Trim(imoStr3);
	Trim(imoStr4);
	Trim(imoStr5);
	
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	transIN_tp = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transIN_tp == (FBFR32 *)NULL)
	{
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in sIagwEcInfoCfm,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}

	transOUT_tp=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if (transOUT_tp == (FBFR32 *)NULL)
	{
		tpfree((char *)transIN_tp);
		PUBLOG_APPERR(__func__,"ERR","分配缓冲区失败，分配大小(%ld)",(long)len);
		sprintf(temp_buf, "E200: alloc error in sIagwEcInfoCfm,size(%ld)", (long)len);
		error_handler32(transIN_tp,transOUT_tp,"200",temp_buf,LABELDBCHANGE,CONNECT0);
	}
	
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "iserviceNo=[%s]",iserviceNo);
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "iAccessNumber=[%s]",iAccessNumber);
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "iecbizAccessNum=[%s]",iecbizAccessNum);
	
	Fchg32(transIN_tp, SEND_PARMS_NUM32, 0, "25", 	(FLDLEN32)0);
	Fchg32(transIN_tp, RECP_PARMS_NUM32, 0, "2", 	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_0,  0, loginAccept,	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_1,  0, oprCode,   	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_2,  0, preCharge, 	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_3,  0, maxItemperDay,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_4,  0, maxItemperMon,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_5,  0, testSign,		(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_6,  0, defaultSignlang,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_7,  0, testSignEn,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_8,  0, testSignZh,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_9,  0, stratrTime,	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_10,  0, endTime,		(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_11,  0, infoCode,	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_12,  0, infoValue,	(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_13,  0, bizCode,		(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_14,  0, iserviceNo,  (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_15,  0, iAccessNumber,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_16,  0, iecbizAccessNum,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_17,  0, ilCustId,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_18,  0, iaccessnumProp,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_19,  0, iGrpIdNo,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_20,  0, imoStr1,   (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_21,  0, imoStr2,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_22,  0, imoStr3,(FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_23,  0, imoStr4,     (FLDLEN32)0);
	Fchg32(transIN_tp, GPARM32_24,  0, imoStr5,     (FLDLEN32)0);
	                                   
	ilen = Fsizeof32(transIN_tp);
	olen = Fsizeof32(transOUT_tp);
	
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "调用服务sIagwEcInfoCfm开始!");

	/*
	ret = tpcall("sIagwEcInfoCfm",(char *)transIN_tp,(long)&ilen,(char **)&transOUT_tp,(long *)&olen,0);
	*/
	ret = tpcall("sIagwEcInfoCfm",(char *)transIN_tp, 0,(char **)&transOUT_tp,(long *)&olen,0);
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "ret=[%d]",ret);
	sprintf(retCode,"%06d", ret);
	if (ret == -1)
	{
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		strcpy(voRetMsg, "sIagwEcInfoCfm服务调用失败！");
		return -1;
	}
	else if(ret != 0 && ret != -1)
	{
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return -2;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
#endif

	pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "调用服务sIagwEcInfoCfm结束!");
	
	Fget32(transOUT_tp,SVC_ERR_NO32,0,retCode,(FLDLEN32)0);
	Fget32(transOUT_tp,SVC_ERR_MSG32,0,retMsg,(FLDLEN32)0);
	if (atol(retCode) != 0) 
	{
		pubLog_Debug(_FFL_, "sendIagwBusiMsg", "", "--sIagwEcInfoCfm-- retCode=[%s][%s]", retCode, retMsg);
		strcpy(voRetMsg,retMsg);
		tpfree((char *)transIN_tp);
		tpfree((char *)transOUT_tp);
		return -3;
	}
	
	tpfree((char *)transIN_tp);
	tpfree((char *)transOUT_tp);
	
	return 0;
}
