/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/**********************************************
 @wt PRG  : publicBill.cp
 @wt EDIT : Wallas Wang
 @wt DATE : 2003-10-15
 @wt FUNC : 资费套餐业务实现的核心函数
 *********************************************/


#include "boss_srv.h"
#include "publicsrv.h"
#include "pubShortMsg.h"

#ifndef MAXENDTIME
#define MAXENDTIME "20200101 00:00:00"
#endif
#define DT "YYYYMMDD HH24:MI:SS"


#define _DEBUG_
#define _WTPRN_

extern int PrepareNewShortMsg(int iModelId);
extern int SetShortMsgParameter(int iModelId,const char *iKey, const char* iValue);
extern int GenerateShortMsg(int iModelId, const char *iRecvPhoneNo, const char *iLoginNo,const char *iOpCode, long iLoginAccept, const char *iOpTime);

EXEC SQL INCLUDE SQLCA;


/*********************************************
** 函数名：	checkOpcodeLimit
** 说明:	通过表slimitopcodesql来限制哪个业务能开通哪个套餐
** 作者:	lixiaoxin
** 时间:	20130220
**********************************************/
int checkOpcodeLimit(char * vREGION_CODE,char * vOP_CODE,char * vMODE_CODE,char * err_msg,
char * vPHONE_NO,char * vID_NO,char * vLOGIN_NO)
{
	EXEC SQL BEGIN DECLARE SECTION;
        char sqlstr[5000];
        char vSELECT_SQL[200];
        char vMODE_FLAG[1+1];
        char vloginREGION_CODE[8+1];
        char vORG_ID[10+1];
    EXEC SQL END   DECLARE SECTION;
    int ncount=0;
    
    /*初始化*/
    Sinitn(sqlstr);
    Sinitn(vSELECT_SQL);
    Sinitn(vMODE_FLAG);
    Sinitn(vloginREGION_CODE);
    Sinitn(vORG_ID);
    
    Trim(vREGION_CODE);
    Trim(vOP_CODE);
    Trim(vMODE_CODE);
    Trim(err_msg);
    Trim(vPHONE_NO);
    Trim(vID_NO);
    Trim(vLOGIN_NO);
    
    /*取条件SQL*/
    exec sql select nvl(select_sql,' ') into :vSELECT_SQL
    	       from slimitopcodesql
    	      where op_code = :vOP_CODE;
   	if(SQLCODE != 0 && SQLCODE != 1403)
   	{
   		sprintf(err_msg,"根据op_code取条件SQL出错[%d]",SQLCODE);
   		pubLog_DBErr(_FFL_, "","","根据op_code取条件SQL出错[%d]",SQLCODE);
   		return -1;
   	}
   	else if(SQLCODE ==1403)
   	{
   		sprintf(vSELECT_SQL,"stop_time>sysdate and show_flag='N'");
   	}
   	Trim(vSELECT_SQL);
   	
   	/*根据限制条件取套餐代码，查看是否限制*/
   	ncount = 0;
   	/*支持 vSELECT_SQL 为空*/
   	if(strlen(vSELECT_SQL) == 0)
   	{
   		sprintf(sqlstr,"select count(*)  from sbillmodecode"
   						" where region_code = '%s' and mode_code = '%s' ",
   						vREGION_CODE,vMODE_CODE);
   	}
   	else
   	{
	   	sprintf(sqlstr,"select count(*)  from sbillmodecode"
	   						" where region_code = '%s' and mode_code = '%s' "
	   						"and %s",vREGION_CODE,vMODE_CODE,vSELECT_SQL);
   	}
   	Trim(sqlstr);
   	printf("\nsqlstr=[%s]\n",sqlstr);
	EXEC SQL EXECUTE
	begin
		EXECUTE IMMEDIATE :sqlstr into :ncount;
	end;
	END-EXEC;
	if(SQLCODE != 0 )
	{
		sprintf(err_msg,"查看是否进行业务限制出错[%d]",SQLCODE);
   		pubLog_DBErr(_FFL_, "","","查看是否进行业务限制出错[%d]",SQLCODE);
   		return -2;
	}
	if(0 == ncount)
	{
		sprintf(err_msg,"此业务办理此套餐受限制，不允许办理");
   		pubLog_DBErr(_FFL_, "","","此业务办理此套餐受限制，不允许办理");
   		return -3;
	}
	
	/*记录套餐办理监控表*/
	ncount = 0;
	exec sql select count(*) into :ncount
		   from sbillmodecode
		  where region_code = :vREGION_CODE 
		  	and mode_code = :vMODE_CODE
		  	and (show_flag='T' or show_flag='Y');
	if(SQLCODE != 0)
	{
		sprintf(err_msg,"查看套餐是不是隐性套餐出错[%d]",SQLCODE);
   		pubLog_DBErr(_FFL_, "","","查看套餐是不是隐性套餐出错[%d]",SQLCODE);
   		return -4;	
	}
	else if(ncount >0)
	{
		sprintf(vMODE_FLAG,"1");
		goto insertdb;
	}
	ncount = 0;
	exec sql select count(*) into :ncount
		   from sbillmodecode
		  where region_code = :vREGION_CODE 
		  	and mode_code = :vMODE_CODE
		  	and stop_time <= sysdate;
	if(SQLCODE != 0)
	{
		sprintf(err_msg,"查看套餐是不是下线套餐出错[%d]",SQLCODE);
   		pubLog_DBErr(_FFL_, "","","查看套餐是不是下线套餐出错[%d]",SQLCODE);
   		return -5;	
	}
	else if(ncount >0)
	{
		sprintf(vMODE_FLAG,"2");
		goto insertdb;
	}
	ncount = 0;
	exec sql select count(*) into :ncount
		   from smaxwatchcfg
		  where region_code = :vREGION_CODE 
		  	and mode_code = :vMODE_CODE
		  	and srvno = '0001';
	if(SQLCODE != 0)
	{
		sprintf(err_msg,"查看套餐是不是受监控套餐出错[%d]",SQLCODE);
   		pubLog_DBErr(_FFL_, "","","查看套餐是不是受监控套餐出错[%d]",SQLCODE);
   		return -5;	
	}
	else if(ncount >0)
	{
		sprintf(vMODE_FLAG,"3");
		goto insertdb;
	}
	
	
insertdb:
	/*记录套餐办理监控表 region_code 用login_no的region_code*/
	if(strlen(vMODE_FLAG) > 0)
	{
		/*取login_no的region_code等信息*/
		exec sql select org_id,substr(org_code,1,2) into :vORG_ID,:vloginREGION_CODE
			  from dloginmsg where login_no = :vLOGIN_NO;
		if(SQLCODE != 0)
		{
			sprintf(err_msg,"取工号信息出错[%d]",SQLCODE);
	   		pubLog_DBErr(_FFL_, "","","取工号信息出错[%d]",SQLCODE);
	   		return -5;	
		}
		Trim(vORG_ID);
		Trim(vloginREGION_CODE);	

		exec sql insert into wmodecodemorelist(region_code,mode_code,phone_no,id_no,
					login_no,org_id,op_time,total_date,mode_flag)
			 values(:vloginREGION_CODE,:vMODE_CODE,:vPHONE_NO,:vID_NO,:vLOGIN_NO,
			 	:vORG_ID,sysdate,to_char(sysdate,'yyyymmdd'),:vMODE_FLAG);
		if(SQLCODE !=0)
		{
			sprintf(err_msg,"记录套餐办理监控表[%d]",SQLCODE);
	   		pubLog_DBErr(_FFL_, "","","记录套餐办理监控表[%d]",SQLCODE);
	   		return -6;	
		}
	}
	
    return 0;
}
/**********************************************
 @wt PRG  : pubGetEffectTime
 @wt FUNC : 根据iSendFlag，
 @wt      : 来计算生效时间oEffectTime。
 @wt      : 需要的输入参数还有iOpTime,iIdNo。
 @wt*******************************************
 @wt 0 iOpTime    : 输入 系统的当前时间
 @wt 1 iSendFlag  : 输入 生效方式
 @wt                  0 -- 立即生效
 @wt                  1 -- 一般预约
 @wt                  2 -- 个性预约
 @wt 2 iIdNo      : 输入 用户的ID
 @wt 3 oEffectTime: 输出 资费的生效时间
 *********************************************/
int pubGetEffectTime(char *iOpTime,char *iSendFlag,char *iIdNo,char *oEffectTime){
    EXEC SQL BEGIN DECLARE SECTION;
        char    vSqlStr[256],vEffTime[18];
		char	vErrmsg[60+1];
		int		ret=0;
		char	vEndTime[17+1];
		char	vAcctCyCleType[1+1];
		char	vCycleBeginDay[2+1];
		char	vCycleType[1+1];
    EXEC SQL END   DECLARE SECTION;

    Sinitn(vEffTime);Trim(iOpTime);Trim(iIdNo);
	Sinitn(vErrmsg);Sinitn(vAcctCyCleType);Sinitn(vCycleBeginDay);
	Sinitn(vCycleType);Sinitn(vEndTime);
    switch(iSendFlag[0]){
        case '0':
            strcpy(vEffTime,iOpTime);
            break;
        case '1':
			/*exec sql select to_char(add_months(sysdate,1),'yyyymm')||'01 00:00:00' into :vEffTime from dual;*/
			ret = pubGetCustAcctCyCle(iIdNo,iOpTime,1,0,vEffTime,vEndTime,vErrmsg);
			if(ret != 0)
			{
				PUBLOG_DBDEBUG("pubGetEffectTime");
				pubLog_DBErr(_FFL_, "","","获取用户的套餐生效时间出错[%d]",ret);
				return 3;
			}
            break;
        case '2':
            Sinitn(vSqlStr);
            sprintf(vSqlStr,"select to_char(add_months(sysdate,1),'yyyymm')||to_char(begin_time,'dd')||' 00:00:00' "
            " from dBillCustDetail%c where id_no=to_number(:v1) and begin_time<=sysdate and end_time>sysdate "
            " and mode_flag='0' and mode_time='Y'",iIdNo[strlen(iIdNo)-1]);
            exec sql prepare preGetEffectTime from :vSqlStr;
            exec sql declare curGetEffectTime cursor for preGetEffectTime;
            exec sql open curGetEffectTime using :iIdNo;
            if(SQLCODE!=OK)
         	{
         		PUBLOG_DBDEBUG("pubGetEffectTime");
         		pubLog_DBErr(_FFL_, "","-1","");
            	return 1;
            }
            for(;;){
                exec sql fetch curGetEffectTime into :vEffTime;
                if(SQLCODE==NOTFOUND) break;
                if(SQLCODE!=OK&&SQLCODE!=NOTFOUND)
                {
                	PUBLOG_DBDEBUG("pubGetEffectTime");
                	pubLog_DBErr(_FFL_, "","-2","");
           			return 2;
            	}
            }
            break;
        default:
            ;
    }
    Sinitn(oEffectTime);
    strcpy(oEffectTime,vEffTime);
#ifdef _PRINT_
    pubLog_Debug(_FFL_, "", "", "pubGetEffectTime [%s][%s]",vEffTime,oEffectTime);
#endif
    return 0;
}
/**********************************************
 @wt PRG  : pubCompYMD
 @wt FUNC : 根据基准时间Time，Cycle，Unit
 @wt      : 来计算实际时间YMD。
 @wt*********************************************
 @wt 0 Time : 输入 基准时间
 @wt 1 Cycle: 输入 时间单位的数量
 @wt 2 Unit : 输入 时间单位
 @wt                  0 -- 天
 @wt                  1 -- 月
 @wt                  2 -- 年
 @wt                  3 -- 自然月
 @wt 3 YMD  : 输出 实际时间
 *********************************************/
void pubCompYMD(char *Time,int Cycle,int Unit,char *YMD,char *IdNo)
{
    char    sY[5],sMDHMS[14],sYMD[18];
	int	iY=0,iRet=0;
	char	oErrMsg[60+1];/*add by cencm at 2012-2-22 16:05:52*/
	char	vEndTime[17+1];/*add by cencm at 2012-2-22 16:05:52*/

    Sinitn(YMD);Sinitn(sYMD);
    Sinitn(sY);Sinitn(sMDHMS);
	Sinitn(oErrMsg);
	Sinitn(vEndTime);
    switch(Unit){
        case 0:
            exec sql select to_char(to_date(:Time,'YYYYMMDD HH24:MI:SS')+:Cycle,'YYYYMMDD HH24:MI:SS')
                into :sYMD from dual;
            break;
        case 1:
            exec sql select to_char(add_months(to_date(:Time,'YYYYMMDD HH24:MI:SS'),:Cycle),'YYYYMMDD HH24:MI:SS')
                into :sYMD from dual;
            break;
        case 2:
            Ssubstr(Time,1,4,sY);
            Ssubstr(Time,5,13,sMDHMS);
            sprintf(sYMD,"%4d%13s",atoi(sY)+Cycle,sMDHMS);
            break;
        case 3:
			iRet=pubGetCustAcctCyCle(IdNo,Time,Cycle,0,sYMD,vEndTime,oErrMsg );
			if(iRet != 0)
			{
				pubLog_Debug(_FFL_, "", "", "pubCompYMD iRet=[%d] oErrMsg=[%s]\n",iRet,oErrMsg);
				/*用户账期获取错误后按原逻辑进行获取*/
            exec sql select to_char(add_months(to_date(:Time,'YYYYMMDD HH24:MI:SS'),:Cycle),'YYYYMM')||'01'
                into :sYMD from dual;
			}
            break;
        default:;
    }
    strcpy(YMD,sYMD);
}
/**********************************************
 @wt  PRG  : pubBillEndMode
 @wt  FUNC : 取消iPhoneNo的iOldMode资费
 @wt       : 其中生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt *********************************************
 @wt 0 iPhoneNo      移动号码
 @wt 1 iOldMode      要取消的套餐代码
 @wt 2 iOldAccept    要取消套餐的申请流水
 @wt 3 iEffectTime   生效时间
 @wt 4 iOpCode       业务代码
 @wt 5 iLoginNo      工号
 @wt 6 iLoginAccept  业务流水
 @wt 7 iOpTime       业务时间
 @wt 8 oErrMsg       函数内部错误
 @wt *********************************************
 @wt  数据结果
 @wt      dBillCustDetailX (Deteted/updated)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(Updated) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 *********************************************/
int pubBillEndMode(
	char *iPhoneNo,
	char *iOldMode,
	char *iOldAccept,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *oErrMsg)
{
	/*boss2.0 lixg add: 20070118 begin */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	/*boss2.0 lixg add: 20070118 end */

	EXEC SQL BEGIN DECLARE SECTION;
		char vECtrlCode[5+1];
		char vIdNo[23];
		int  i=0,t=0,vTotalDate=0,vFavOrder=0,vBeginFlag=0,vEndFlag=0,vAccountOpType=0,vFuncBindFlag=-1,vDeltaDays=-1,vnum=0;
		char vBelongCode[8],vDetailType[2],vDetailCode[5],vBeginTime[18],vEndTime[18];
		char vEndSql[1024],vSelSql[1024],vModeTime[2],vSmCode[3],vOrgCode[10];
		char vBindOffonFlag[2],vFuncBindCode[3],vVpmnGroup[11];
		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];

		/* lixg add for Product provide begin */
		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		int  retValue=0;
		/* lixg add for Product provide end */

		char vNewCardCode[2+1];
		char vOldCardCode[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		
		char vDealFunc[2+1];/*功能代码 add by chendx@20111228 */
		int  funcflag = 0;  /*单套餐单功能标志，1表示单套餐单功能*/
		TPubBillInfo tPubBillInfo;
		int vMaxNum, vCurNum, vBuyNum;

		/*ng 解耦*/
		int ret = 0;
		int vCount = 0;
		char    dynStmt[1024];
		char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char 	v_update_sql[500+1];
		char	vTotalDateStr[8+1];
		TdBaseFavIndex			vTdBaseFavIndex;
		TdBaseVpmnIndex			vTdBaseVpmnIndex;
		TdBillCustDetailIndex	vTdBillCustDetailIndex;
	EXEC SQL END   DECLARE SECTION;

	memset(v_update_sql, 0, sizeof(v_update_sql));
	memset(dynStmt, 0, sizeof(dynStmt));
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
	memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
	memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
	memset(vTotalDateStr, 0, sizeof(vTotalDateStr));

	memset(vNewCardCode, 0, sizeof(vNewCardCode));
	memset(vOldCardCode, 0, sizeof(vOldCardCode));
	
	memset(vDealFunc, 0, sizeof(vDealFunc));

	Trim(iPhoneNo);
	Trim(iOldMode);
	Trim(iOldAccept);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);


	oErrMsg[0] = '\0';
	/*init(oErrMsg);*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode begin -----");
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iPhoneNo=[%s] -----",iPhoneNo);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iOldMode=[%s] -----",iOldMode);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iOldAccept=[%s] -----",iOldAccept);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iEffectTime=[%s] -----",iEffectTime);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iOpCode=[%s] -----",iOpCode);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iLoginNo=[%s] -----",iLoginNo);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iLoginAccept=[%s] -----",iLoginAccept);
	pubLog_Debug(_FFL_, "", "", "pubBillEndMode iOpTime=[%s] -----",iOpTime);
#endif

	Sinitn(vIdNo);Sinitn(vBelongCode);

	EXEC SQL select to_char(id_no),belong_code,sm_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vSmCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if (SQLCODE != OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubBillEndMode");
		pubLog_DBErr(_FFL_, "", "1024", "pubBillEndMode :select dCustMsg error[%s]",oErrMsg);
		return 1024;
	}

/*
    EXEC SQL insert into wsndcailmsghis(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
											LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
											DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
			 select LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
								LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,'d',
								sysdate,SND_TIMES,prom_name,deal_func,begin_time,mode_accept
			from wsndcailmsg
			where mode_accept=to_number(:iOldAccept) and id_no=to_number(:vIdNo);
	if(SQLCODE!=0 && SQLCODE!=1403)
	{
		pubLog_Debug(_FFL_, "pubBillEndMode", "", " .pubBillEndMode :insert into wsndcailmsghis error[%d][%s]", oErrMsg);
        strcpy(oErrMsg,"处理wsndcailmsg1错误!");
        return 1024;
	}

    EXEC SQL delete from wsndcailmsg
    			where mode_accept=to_number(:iOldAccept) and id_no=to_number(:vIdNo);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
    	pubLog_Debug(_FFL_, "pubBillEndMode", "", " .pubBillEndMode :delete from wsndcailmsg error[%d][%s]", oErrMsg);
        strcpy(oErrMsg,"处理wsndcailmsg错误!");
        return 1024;
    }
*/

	/*if(strcmp(iOpCode,"1270")==0 || strcmp(iOpCode,"1272")==0 || strcmp(iOpCode,"1266")==0 || strcmp(iOpCode,"1268")==0 || strcmp(iOpCode,"3506")==0)
    {
		init(vAwakeModeName);
		init(vawakemsg);
		init(useFlag);
		strcpy(useFlag,"d");
		EXEC SQL select mode_name,awake_msg
					into :vAwakeModeName,:vawakemsg
					from sawakemodecode
					where region_code=substr(:vBelongCode,1,2)
					and awakemode_code=:iOldMode
					and use_flag=:useFlag;
		if(SQLCODE==0)
		{
			Trim(vAwakeModeName);
			Trim(vawakemsg);
			sprintf(vawakemsg,"中国移动温馨提示：您已经开通了%s业务套餐。",vAwakeModeName);
			EXEC SQL insert into wCommonShortMsg(COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,
    	   					PHONE_NO,MSG,ORDER_CODE,BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME,sent_time)
    	   			select SMSG_SEQUENCE.nextval,to_number(:iLoginAccept),1,:iPhoneNo,:vawakemsg,
    	   					0,0,:iOpCode,:iLoginNo,sysdate,''
    	   			from dual;
    	   	if(SQLCODE!=0 && SQLCODE!=1403)
    	   	{
    	   		pubLog_Debug(_FFL_, "pubBillEndMode", "", " .pubBillBeginMode :insert wCommonShortMsg error[%d][%s]", oErrMsg);
    	    	strcpy(oErrMsg,"发送提醒短信错误!");
    	    	return 1024;
    	   	}
		}
	}*/

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if (SQLCODE != OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubBillEndMode");
		pubLog_DBErr(_FFL_, "", "127078", "select dLoginMsg error[%s]",oErrMsg);
		return 127078;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	init(vECtrlCode);
	/* boss2.0 lixg add: 20070408 增加产品后项控制 */

	EXEC SQL select nvl(end_ctrl_code,' ') into :vECtrlCode
		from sBillModeCode
		where region_code = substr(:vBelongCode,1,2)
		and mode_code = :iOldMode;
	if(SQLCODE != 0){
		PUBLOG_DBDEBUG("pubBillEndMode");
		sprintf(oErrMsg,"用户[%s]没有定购产品[%s]!", iPhoneNo, iOldMode);
#ifdef _DEBUG_
	pubLog_DBErr(_FFL_, "", "1023", "ECtrlCheck Error [%s] [%s] [%s]", iOldMode, iPhoneNo,oErrMsg);
#endif
		return 1023;
	}

	pubLog_Debug(_FFL_, "pubBillEndMode", "", "begin to fProdCheckOpr --ctrlCode=[%s]---", vECtrlCode);

	Trim(vECtrlCode);

	if (strlen(vECtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vECtrlCode, iOpCode, iLoginNo, &fe) != 0){
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, "", "atoi(tmpRetCode)","");
			return atoi(tmpRetCode);
		}
	}


	pubLog_Debug(_FFL_, "pubBillEndMode", "", "end to fProdCheckOpr -----");
#endif


	EXEC SQL select days into :vDeltaDays from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iOldMode and open_flag='0';
	if (SQLCODE==OK||SQLROWS==1){
		EXEC SQL insert into dCustExpireHis(
			ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
			UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
			select
			ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
			to_number(:iLoginAccept),to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'d',:iOpCode,:vTotalDate
			from dCustExpire where id_no=to_number(:vIdNo);
		if (SQLCODE!=0&&SQLCODE!=1403){
			strcpy(oErrMsg,"删除用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "", "127088", "pubBillEndMode ",SQLCODE);
			pubLog_DBErr(_FFL_, "", "127088", "pubBillEndMode [%s]",oErrMsg);
			return 127088;
		}

		/*ng解耦 by wxcrm at 20090813 begin
		EXEC SQL delete dCustExpire where id_no=to_number(:vIdNo);
		if (SQLCODE!=0&&SQLCODE!=1403){

			strcpy(oErrMsg,"删除用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "", "127087", oErrMsg);
			return 127087;
		}
		ng解耦 by wxcrm at 20090813 end*/
		memset(v_parameter_array, 0, sizeof(v_parameter_array));
		strcpy(v_parameter_array[0],vIdNo);
		ret = 0;
		ret = OrderDeleteCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vIdNo,"",v_parameter_array);
		if (ret < 0)
		{
			strcpy(oErrMsg,"删除用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "", "127087", oErrMsg);
			return 127087;
		}

	}
	Trim(vIdNo);

	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
    EXEC SQL select a.deal_func into :vDealFunc
                from ssndcailcfg a,soutsidefeemode b
               where a.region_code = b.region_code
               	 and a.mode_code = b.mode_code
               	 and a.region_code = substr(:vBelongCode,1,2)
               	 and a.mode_code = :iOldMode;
    if(SQLCODE==OK&&SQLROWS==1)
	{
		printf("表示单套餐单功能!\n");
		funcflag = 1;
	}       	 	
	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
	

	Sinitn(vSelSql);
	sprintf(vSelSql,"select detail_type,detail_code,to_char(begin_time,'yyyymmdd hh24:mi:ss'),fav_order,"
		" to_char(end_time,'yyyymmdd hh24:mi:ss'),mode_time from dBillCustDetail%c where id_no=to_number(:v1) "
		" and mode_code=:v2 and login_accept=to_number(:v3) order by mode_time desc",vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubBillEndMode", "", "pubBillEndMode %s",vSelSql);
#endif
	EXEC SQL PREPARE preModeDet FROM :vSelSql;
	EXEC SQL DECLARE curModeDet CURSOR FOR preModeDet;
	EXEC SQL OPEN curModeDet USING :vIdNo,:iOldMode,:iOldAccept;
	if(SQLCODE!=OK)
	{
		strcpy(oErrMsg,"查询dBillCustDetail初始化错误");
		PUBLOG_DBDEBUG("pubBillEndMode");
		pubLog_DBErr(_FFL_, "", "127040", "pubBillEndMode :open (select dBillModeDetail) error[%s]",oErrMsg);
		return 127040;
	}
	printf(":vIdNo = %s,:iOldMode = %s,:iOldAccept =%s\n",vIdNo,iOldMode,iOldAccept);
	for(i=0;;)
	{
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vBeginTime);Sinitn(vEndTime);
		EXEC SQL FETCH curModeDet INTO :vDetailType,:vDetailCode,:vBeginTime,:vFavOrder,:vEndTime,:vModeTime;
		printf(":vDetailType = %s,:vDetailCode = %s,:vBeginTime =%s,:vFavOrder = %d,:vEndTime = %s,:vModeTime =%s\n",vDetailType,vDetailCode,vBeginTime,vFavOrder,vEndTime,vModeTime);

		if((SQLCODE!=OK&&SQLCODE!=NOTFOUND)||(SQLCODE==NOTFOUND&&i==0))
		{
			EXEC SQL CLOSE curModeDet;
			strcpy(oErrMsg,"查询dBillCustDetail明细错误");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "", "127041", "pubBillEndMode :fetch (select dBillModeDetail) error[%s] SQLCODE= %d",oErrMsg,SQLCODE);

			return 127041;
		}
		if(SQLCODE==NOTFOUND&&i>0) break;

		if (strcmp(vModeTime,"Y")==0){
			/*** vBeginFlag 记录生效时间与要取消套餐的开始时间比较后的关系 ***/
			vBeginFlag = strcmp(iEffectTime,vBeginTime);
		}
		if(strcmp(iOpCode,"g601")==0)
		{
			vBeginFlag=0;
			vBeginFlag = strcmp(iEffectTime,vBeginTime);
		}
		/*** vEndFlag 记录生效时间与要取消套餐的结束时间比较后的关系 ***/
		vEndFlag=strcmp(vEndTime,iEffectTime);
		
		if (i==0)
		{
			printf("funcflag[%d],iOpCode[%s],vDealFunc[%s]\n",funcflag,iOpCode,vDealFunc);
			/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
			/*chendx 测试使用1272，没用2976*/
			if(funcflag == 1 && strcmp(iOpCode,"2976") == 0 && strcmp(vDealFunc,"9d") == 0)
			{
				printf("网站进行套餐修改，且该套餐是单功能(手机报)，不需要发送短信提醒！\n");
			}/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
			else
			{
				retValue=DcustModeChgAwake(iOpCode,iOldMode,vBelongCode,"d",atol(iOldAccept),iPhoneNo,iLoginNo,iEffectTime,vEndTime);
				if (retValue < 0){
					EXEC SQL CLOSE curModeDet;
					strcpy(oErrMsg,"发送提醒短信错误!");
					PUBLOG_DBDEBUG("pubBillEndMode");
					pubLog_DBErr(_FFL_, "", "127099", "pubBillBeginMode :insert dBillCustDetail error[%s]",oErrMsg);
					return 127099;
				}
			}
		}

		if(vBeginFlag <= 0)
		{
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'d',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10 ",
				vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubBillEndMode", "", "pubBillEndMode :%s",vEndSql);
#endif
			EXEC SQL PREPARE preWWW_1 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_1 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,
				:iLoginAccept,:vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				EXEC SQL CLOSE curModeDet;
				strcpy(oErrMsg,"删除dBillCustDetail时记录历史明细错误!");
				PUBLOG_DBDEBUG("pubBillEndMode");
				pubLog_DBErr(_FFL_, "", "127042", "[%s][%s]",SQLROWS,oErrMsg);
				return 127042;
			}

			Sinitn(vEndSql);
			/*ng解耦 by wxcrm at 20090813 begin
			sprintf(vEndSql," delete dBillCustDetail%c where id_no=to_number(:v1) and mode_code=:v2 and login_accept=to_number(:v3) "
				" and detail_type=:v4 and detail_code=:v5",vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubBillEndMode", "", " pubBillEndMode :%s",vEndSql);
#endif
			exec sql prepare preWWW_2 from :vEndSql;
			exec sql execute preWWW_2 using :vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if(SQLCODE!=OK||SQLROWS!=1)
			{
			    exec sql close curModeDet;
			     strcpy(oErrMsg,"删除dBillCustDetail无效记录时错误!");
			    PUBLOG_DBDEBUG("pubBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127043", "[%s]",oErrMsg);
			    return 127043;
			}
			ng解耦 by wxcrm at 20090813 end*/
			memset(dynStmt, 0, sizeof(dynStmt));
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
			memset(v_parameter_array, 0, sizeof(v_parameter_array));
			/*
			sprintf(dynStmt, "SELECT id_no,mode_code,login_accept,detail_type,detail_code "
                             "FROM dBillCustDetail%c "
                             "WHERE id_no=to_number(:v1) and mode_code=:v2 and login_accept=to_number(:v3) "
                             "and detail_type=:v4 and detail_code=:v5",vIdNo[strlen(vIdNo)-1]);
			EXEC SQL EXECUTE
			BEGIN
			        EXECUTE IMMEDIATE :dynStmt
			                into :vTdBillCustDetailIndex
			                using :vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			END;
			END-EXEC;
			if(SQLCODE!=OK||SQLROWS!=1)
			{
			   	exec sql close curModeDet;
			    strcpy(oErrMsg,"查询dBillCustDetail无效记录时错误!");
			    PUBLOG_DBDEBUG("pubBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127043", "[%s]",oErrMsg);
			    return 127043;
			}*/
			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_parameter_array[0],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[1],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[2],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[3],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[4],vTdBillCustDetailIndex.sDetailCode);

			ret = 0;
			ret = OrderDeleteBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBillCustDetailIndex,"",v_parameter_array);
			if (ret != 0)
			{
				exec sql close curModeDet;
			    strcpy(oErrMsg,"删除dBillCustDetail无效记录时错误!");
			    PUBLOG_DBDEBUG("pubBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127043", "[%s]",oErrMsg);
			    return 127043;
			}
		}
		else 
		{
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'u',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10",vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
			#ifdef _WTPRN_
				pubLog_Debug(_FFL_, "pubBillEndMode", "", "pubBillEndMode :%s",vEndSql);
			#endif
			EXEC SQL PREPARE preWWW_3 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_3 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vIdNo,
				:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				strcpy(oErrMsg,"更改dBillCustDetail时记录历史明细错误!");
			    pubLog_DBErr(_FFL_, "SQLCODE = [%d]", "127044", "[%s]",SQLCODE, oErrMsg);
				EXEC SQL CLOSE curModeDet;
				PUBLOG_DBDEBUG("pubBillEndMode");
				return 127044;
			}
			Sinitn(vEndSql);
			/*
			sprintf(vEndSql,"update dBillCustDetail%c set mode_status='N',end_time=\
				decode(sign(end_time-to_date(:v1,'YYYYMMDD HH24:MI:SS')),1,to_date(:v2,'YYYYMMDD HH24:MI:SS'),end_time),\
				login_accept=to_number(:v3),op_code=:v4,total_date=:v5,\
				op_time=to_date(:v6,'yyyymmdd hh24:mi:ss'),login_no=:v7\
				where id_no=to_number(:v8) and mode_code=:v9 and login_accept=to_number(:v10) and detail_type=:v11 and detail_code=:v12",\
				vIdNo[strlen(vIdNo)-1]);
			*/


			/*ng解耦 by wxcrm at 20090813 begin
			sprintf(vEndSql,"update dBillCustDetail%c set mode_status='N',end_time= "
				" decode(sign(end_time-to_date(:v1,'YYYYMMDD HH24:MI:SS')),1,to_date(:v2,'YYYYMMDD HH24:MI:SS'),end_time),"
				" op_code=:v4,total_date=:v5,"
				" op_time=to_date(:v6,'yyyymmdd hh24:mi:ss'),login_no=:v7 "
				"where id_no=to_number(:v8) and mode_code=:v9 and login_accept=to_number(:v10) and detail_type=:v11 and detail_code=:v12",
				vIdNo[strlen(vIdNo)-1]);
			EXEC SQL PREPARE preWWW_4 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_4 USING :iEffectTime,:iEffectTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				exec sql close curModeDet;
			 	PUBLOG_DBDEBUG("pubBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127045", "[%s]",oErrMsg);
				strcpy(oErrMsg,"更改dBillCustDetail错误!");
				return 127045;
			}
			ng解耦 by wxcrm at 20090813 end*/
			memset(dynStmt, 0, sizeof(dynStmt));
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));

			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			memset(v_parameter_array, 0, sizeof(v_parameter_array));
			sprintf(vTotalDateStr,"%d",vTotalDate);

			strcpy(v_parameter_array[0],iEffectTime);
			strcpy(v_parameter_array[1],iEffectTime);
			strcpy(v_parameter_array[2],iOpCode);
			strcpy(v_parameter_array[3],vTotalDateStr);
			strcpy(v_parameter_array[4],iOpTime);
			strcpy(v_parameter_array[5],iLoginNo);
			strcpy(v_parameter_array[6],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[7],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[8],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[9],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[10],vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_update_sql,"mode_status='N',end_time=decode(sign(end_time-to_date(:v20,'YYYYMMDD HH24:MI:SS')),1,to_date(:v21,'YYYYMMDD HH24:MI:SS'),end_time), op_code=:v22,total_date=:v23,op_time=to_date(:v24,'yyyymmdd hh24:mi:ss'),login_no=:v25 ");
			Trim(v_update_sql);

			ret = 0;
			ret = OrderUpdateBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBillCustDetailIndex,vTdBillCustDetailIndex,v_update_sql,"",v_parameter_array);

			if (ret != 0)
			{
				exec sql close curModeDet;
			 	PUBLOG_DBDEBUG("pubBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127045", "[%s]",oErrMsg);
				strcpy(oErrMsg,"更改dBillCustDetail错误!");
				return 127045;
			}

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubBillEndMode", "", "pubBillEndMode :update(for/u)dBillCustDetails%c%s",vIdNo[strlen(vIdNo)-1],vEndSql);
#endif
		}
		
		switch(vDetailType[0])
		{
			case '0':
				/*** process bill system data begin ***/
				if (vEndFlag > 0){
					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL update dBaseFav set end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')
						where msisdn=:iPhoneNo and fav_type=:vDetailCode
						and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					if(SQLCODE!=OK||SQLROWS!=1)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubBillEndMode");
			   			pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						strcpy(oErrMsg,"更改dBaseFav错误!");
						return 127046;
					}
					ng解耦 by wxcrm at 20090812 end*/
					/*
					memset(dynStmt, 0, sizeof(dynStmt));
					sprintf(dynStmt, "SELECT msisdn, fav_type, flag_code, start_time "
									 "FROM dBaseFav "
									 "WHERE msisdn=:v1 and fav_type=:v2 "
									 "and start_time=to_date(:v3,'YYYYMMDD HH24:MI:SS')");

					EXEC SQL EXECUTE
					BEGIN
						EXECUTE IMMEDIATE :dynStmt
								into :vTdBaseFavIndex
								using :iPhoneNo, :vDetailCode, :vBeginTime;
					END;
					END-EXEC;
					*/

					EXEC SQL SELECT msisdn, fav_type, flag_code, to_char(start_time,'YYYYMMDD HH24:MI:SS')
							 INTO :vTdBaseFavIndex.sMsisdn,:vTdBaseFavIndex.sFavType,:vTdBaseFavIndex.sFlagCode,:vTdBaseFavIndex.sStartTime
							 FROM dBaseFav
							 WHERE msisdn=:iPhoneNo and fav_type=:vDetailCode
							 AND start_time=TO_DATE(:vBeginTime,'YYYYMMDD HH24:MI:SS');

					if(SQLCODE!=OK||SQLROWS!=1)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubBillEndMode");
						sprintf(oErrMsg,"查询dBaseFav错误![%s][%s]",vDetailCode,vBeginTime);
			   			pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}
					memset(v_parameter_array, 0, sizeof(v_parameter_array));
					strcpy(v_parameter_array[0],iEffectTime);
					strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
					strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
					strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
					strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);
					ret = 0;
					ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:v11,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
					if (ret != 0)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubBillEndMode");
						strcpy(oErrMsg,"更改dBaseFav错误!");
			   			pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
					    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					    start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
						select
					    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					    start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
					        region_code,id_no,group_id,product_code
						from dBaseFav where msisdn=:iPhoneNo and fav_type=:vDetailCode
					    and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					if(SQLCODE!=OK||SQLROWS!=1)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubBillEndMode");
			   			pubLog_DBErr(_FFL_, "", "127047", "[%s]",oErrMsg);
						strcpy(oErrMsg,"记录wBaseFavChg错误!");
						return 127047;
					}
					#endif
					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL delete dBaseFav where msisdn=:iPhoneNo and fav_type=:vDetailCode
					    and start_time>=end_time;
					if(SQLCODE!=OK&&SQLCODE!=NOTFOUND)
					{
						exec sql close curModeDet;
						strcpy(oErrMsg,"清除dBaseFav无效记录时错误!");
						PUBLOG_DBDEBUG("pubBillEndMode");
			   			pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);

						return 127049;
					}
					ng解耦 by wxcrm at 20090812 end*/
					memset(dynStmt, 0, sizeof(dynStmt));
					sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
									 "FROM dBaseFav "
									 "WHERE msisdn=:v1 and fav_type=:v2 and start_time>=end_time");
					EXEC SQL PREPARE ng_SqlStr3 FROM :dynStmt;
					EXEC SQL DECLARE ng_Cur3 CURSOR FOR ng_SqlStr3;
					EXEC SQL OPEN ng_Cur3 using :iPhoneNo,:vDetailCode;
					for(i=0;;)
					{
						memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
						EXEC SQL FETCH ng_Cur3 INTO :vTdBaseFavIndex;
						if (SQLCODE == 1403) break;
						if (SQLCODE != 1403 && SQLCODE != 0)
						{
							strcpy(oErrMsg,"查询dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur3;
							EXEC SQL CLOSE curModeDet;
							return 127049;
						}
						memset(v_parameter_array, 0, sizeof(v_parameter_array));
						strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
						ret = 0;
						ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
						if (ret != 0)
						{
							strcpy(oErrMsg,"清除dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur3;
							EXEC SQL CLOSE curModeDet;
							return 127049;
						}
					}
					EXEC SQL CLOSE ng_Cur3;


					/*** 处理聊天套餐默认退出vmpn集团 ***/
					init(vVpmnGroup);
					EXEC SQL select group_index into :vVpmnGroup from sBillVpmnCond
						where region_code=substr(:vBelongCode,1,2) and mode_code=:iOldMode;
					if(SQLROWS==1)
					{
					    Trim(vVpmnGroup);
					    #ifdef _CHGTABLE_
					    exec sql insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					    select msisdn,groupid,mocrate,mtcrate,valid,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'2',to_date(:iOpTime,'yyyymmdd hh24:mi:ss')
					    from dBaseVpmn where msisdn=:iPhoneNo and groupid=:vVpmnGroup
					    and valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127081", " [%s]", oErrMsg);

					        return 127081;
					    }
					    #endif
					    /*ng解耦 by wxcrm at 20090813 begin
					    exec sql update dBaseVpmn set invalid=to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')
					    where msisdn=:iPhoneNo and groupid=:vVpmnGroup
					    and valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);

					        return 127080;
					    }
					    ng解耦 by wxcrm at 20090813 end*/
					    EXEC SQL SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS')
					    		 INTO :vTdBaseVpmnIndex
					    		 FROM dBaseVpmn
					    		 WHERE msisdn=:iPhoneNo
					    		 AND groupid=:vVpmnGroup
					    		 AND valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理询dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }

					    Trim(vTdBaseVpmnIndex.sMsisdn);
					    Trim(vTdBaseVpmnIndex.sGroupId);
					    Trim(vTdBaseVpmnIndex.sValid);

					    memset(v_parameter_array, 0, sizeof(v_parameter_array));
					    strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sGroupId);
						strcpy(v_parameter_array[3],vTdBaseVpmnIndex.sValid);

						ret = 0;
					    ret = OrderUpdateBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,vTdBaseVpmnIndex,"invalid=to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')","",v_parameter_array);
					    if (ret != 0)
					    {
					    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }


					    /*ng解耦 by wxcrm at 20090813 begin
					    exec sql delete dBaseVpmn where msisdn=:iPhoneNo and groupid=:vVpmnGroup and valid>=invalid;
						ng解耦 by wxcrm at 20090813 end*/
						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseVpmn "
										 "WHERE msisdn=:iPhoneNo and groupid=:vVpmnGroup and valid>=invalid");
						EXEC SQL PREPARE ng_SqlStr5 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur5 CURSOR FOR ng_SqlStr5;
						EXEC SQL OPEN ng_Cur5 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
						{
							memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
							EXEC SQL FETCH ng_Cur5 INTO :vTdBaseVpmnIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}

						    Trim(vTdBaseVpmnIndex.sMsisdn);
						    Trim(vTdBaseVpmnIndex.sGroupId);
						    Trim(vTdBaseVpmnIndex.sValid);

							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseVpmnIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sGroupId);
							strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sValid);

							ret = 0;

							/*******************ng解耦 liuzhou 只删除dbasevpmn表中记录，不插wBaseVpmnChg  *****************/
							/*ret = OrderDeleteBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);*/
							ret = OrderDeleteOtherBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur5;


						/*ng解耦 by wxcrm at 20090812 begin
					    exec sql update dBaseFav set end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')
					    where msisdn=:iPhoneNo and fav_type='aa00' and flag_code=:vVpmnGroup
					    and  start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK)
					    {
					    	sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }
					    ng解耦 by wxcrm at 20090812 end*/
					    memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
					    strncpy(vTdBaseFavIndex.sMsisdn		,iPhoneNo	,	15	);
					    strncpy(vTdBaseFavIndex.sFavType	,"aa00"		,	4	);
					    strncpy(vTdBaseFavIndex.sFlagCode	,vVpmnGroup	,	10	);
					    strncpy(vTdBaseFavIndex.sStartTime	,vBeginTime	,	17	);

					    Trim(vTdBaseFavIndex.sMsisdn);
					    Trim(vTdBaseFavIndex.sFavType);
					    Trim(vTdBaseFavIndex.sFlagCode);
					    Trim(vTdBaseFavIndex.sStartTime);

					    memset(v_parameter_array, 0, sizeof(v_parameter_array));
					    strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);

						ret = 0;
						ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
						if (ret != 0)
						{
							sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
						}
					    #ifdef _CHGTABLE_
					    exec sql insert into wBaseFavChg(
					        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					        start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					    select
					        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					        start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,
					            id_no,group_id,product_code
					    from dBaseFav where msisdn=:iPhoneNo and fav_type='aa00' and flag_code=:vVpmnGroup
					        and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK)
					    {
					    	sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubBillEndMode");
			   				pubLog_DBErr(_FFL_, "", "127081", "[%s]", oErrMsg);
					        return 127081;
					    }
					    #endif
					    /*ng解耦 by wxcrm at 20090812 begin
					    exec sql delete dBaseFav where msisdn=:iPhoneNo and fav_type='aa00'
					        and flag_code=:vVpmnGroup and start_time>=end_time;
						ng解耦 by wxcrm at 20090812 end*/
						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseFav "
										 "WHERE msisdn=:v1 and fav_type='aa00' and flag_code=:v2 and start_time>=end_time");

						EXEC SQL PREPARE ng_SqlStr4 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur4 CURSOR FOR ng_SqlStr4;
						EXEC SQL OPEN ng_Cur4 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
					    {
					    	memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
							EXEC SQL FETCH ng_Cur4 INTO :vTdBaseFavIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}
							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
							strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
							strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
							ret = 0;
							ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur4;
					}
				}
				/*** process bill system data end ***/
			case '1':case '2': case '3':case '4':case 'a':
				/*** process account system data ***/
				if(vBeginFlag<=0) vAccountOpType=3; else vAccountOpType=2;

				if(vEndFlag >= 0){
					#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
						op_no,op_type,op_time,
						id_no,detail_type,detail_code,begin_time,
						end_time,fav_order,mode_code)
					values(
						sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE != OK){
						sprintf(oErrMsg,"记录wUserFavChg时错误![%d]",SQLCODE);
						PUBLOG_DBDEBUG("pubBillEndMode");
			   			pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
			   			exec sql close curModeDet;
						return 127048;
					}
					#endif
				}
				else {
					#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
					    op_no,op_type,op_time,
					    id_no,detail_type,detail_code,begin_time,
					    end_time,fav_order,mode_code)
					values(
					    sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
					    to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
					    to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE!=OK){
					  strcpy(oErrMsg,"记录wUserFavChg时错误!");
					  PUBLOG_DBDEBUG("pubBillEndMode");
					  pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
					  exec sql close curModeDet;
					  return 127048;
					}
					#endif
				}

				if(vDetailType[0]!='a') break;
				/** 对于特服绑定的取消,判断该用户是否还有别的套餐有相同的绑定,
				    如果没有,则取消该特服绑定的预约记录  **/
				EXEC SQL select count(*) into :vFuncBindFlag
					from dBillCustDetail where id_no=to_number(:vIdNo) and detail_type='a'
					and detail_code=:vDetailCode and end_time>to_date(:iEffectTime,'yyyymmdd hh24:mi:ss');
				if(vFuncBindFlag==0)
				{
					init(vBindOffonFlag);init(vFuncBindCode);
					EXEC SQL select function_code,off_on into :vFuncBindCode,:vBindOffonFlag from sBillFuncBind
					where region_code=substr(:vBelongCode,1,2) and function_bind=:vDetailCode;

				/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
					EXEC SQL insert into wCustExpireHis(
						command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type, ORG_ID)
						SELECT command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,:iLoginNo,to_number(:iLoginAccept),:vTotalDate,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
						:iOpCode,'d', ORG_ID
						from wCustExpire where id_no=to_number(:vIdNo) and function_code=:vFuncBindCode
						and business_status=:vBindOffonFlag and login_accept=:iOldAccept;

					EXEC SQL delete wCustExpire where id_no=to_number(:vIdNo)
						and function_code=:vFuncBindCode and function_code=:vFuncBindCode and login_accept=to_number(:iOldAccept);
				}
				break;

			/**
			* modify Time: 2005-11-20
			* modify author: lixg
			* modify Resion: 吉林产品改造升级 begin
			*/
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				pubLog_Debug(_FFL_, "pubBillEndMode", "", "回退缴费回馈、买手机送话费产品信息。。。");
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d", vTotalDate);
				strcpy(vGrantMsg.modeCode, iOldMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.oldLoginAccept, iOldAccept);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);

				if ( (iProductRetCode = sProductCancelFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "", "iProductRetCode", "");
					return iProductRetCode;
				}

				break;
				/* end */
			default:;
		}
		i++;
	}
	EXEC SQL CLOSE curModeDet;

/* 集团上线暂时注释下面代码 */
#if 0
	/* 记录服务历史表 chenxuan boss3 */
	EXEC SQL insert into dBillCustSrvHis (id_no, srv_type, srv_code, begin_time, end_time,
			mode_code, login_accept, update_code, update_date, update_time, update_login,
			update_accept, update_type)
		select id_no, srv_type, srv_code, begin_time, end_time, mode_code, login_accept,
			:iOpCode, :vTotalDate, to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :iLoginNo, :iLoginAccept, 'd'
		from dBillCustSrv where id_no = :vIdNo and login_accept = :iOldAccept;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		PUBLOG_DBDEBUG("pubBillEndMode");
		pubLog_DBErr(_FFL_, "", "127070", " .pubBillBeginMode :insert dBillCustSrv error");
		return 127070;
	}

	/* 删除用户服务 */
	EXEC SQL delete from dBillCustSrv where id_no = :vIdNo and login_accept = :iOldAccept;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		PUBLOG_DBDEBUG("pubBillEndMode");
		pubLog_DBErr(_FFL_, "", "127071", "delete dBillCustSrv error");
		return 127071;
	}

	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.off_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre0 FROM :vSelSql;
	EXEC SQL DECLARE cur0 CURSOR FOR pre0;
	EXEC SQL OPEN cur0 USING :vBelongCode, :iOldMode;
	for (;;){
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur0 INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "", "127071", "insert wSndSrvDay error");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur0;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	/* 调用指令分发函数 chenxuan boss3 */
	strcpy(tPubBillInfo.IdNo,        vIdNo);
	strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
	strcpy(tPubBillInfo.BelongCode,  vBelongCode);
	strcpy(tPubBillInfo.SmCode,      vSmCode);
	strcpy(tPubBillInfo.ModeCode,    iOldMode);
	strcpy(tPubBillInfo.OpCode,      iOpCode);
	strcpy(tPubBillInfo.OpTime,      iOpTime);
	strcpy(tPubBillInfo.EffectTime,  iEffectTime);
	tPubBillInfo.TotalDate = vTotalDate;
	strcpy(tPubBillInfo.LoginNo,     iLoginNo);
	strcpy(tPubBillInfo.LoginAccept, iOldAccept);
	pubLog_Debug(_FFL_, "", "", "begin fDispatchCmd+++++++++++++++ ");
	iProductRetCode = fDispatchCmd("0", &tPubBillInfo, oErrMsg);
	if (iProductRetCode != 0) {
		pubLog_DBErr(_FFL_, "", "127072", "");
		return 127072;
	}
	/* 指令分发函数结束 chenxuan boss3 */

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	EXEC SQL select max_num, cur_num, buy_num INTO :vMaxNum, :vCurNum, :vBuyNum
		from sBillModeNum where region_code = substr(:vBelongCode,1,2) and mode_code = :iOldMode
		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'2',sysdate
				  from	sbillmodenum
			     where	region_code = substr(:vBelongCode,1,2)  and mode_code = :iOldMode and cur_num > 0
				   and 	sysdate > begin_time and sysdate < end_time;

		/*EXEC SQL update sBillModeNum set cur_num = cur_num-1
			where region_code = substr(:vBelongCode,1,2) and mode_code = :iOldMode and cur_num > 0
				and sysdate > begin_time and sysdate < end_time;*/

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(oErrMsg, "更新表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "","127073", "[%s]", oErrMsg);
			return 127073;
		}
	}

	EXEC SQL select max_num, cur_num, buy_num INTO :vMaxNum, :vCurNum, :vBuyNum
		from sBillModeNum where region_code = '00' and mode_code = :iOldMode
		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'2',sysdate
				  from	sbillmodenum
			     where	region_code = '00'  and mode_code = :iOldMode and cur_num > 0
				   and 	sysdate > begin_time and sysdate < end_time;

		/*EXEC SQL update sBillModeNum set cur_num = cur_num-1
			where region_code = substr(:vBelongCode,1,2) and mode_code = :iOldMode and cur_num > 0
				and sysdate > begin_time and sysdate < end_time;*/

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(oErrMsg, "更新表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillEndMode");
			pubLog_DBErr(_FFL_, "","127073", "[%s]", oErrMsg);
			return 127073;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

    /*
    if(strstr(iOldMode,"gn22"))
    {
            pubLog_Debug(_FFL_, "pubBillEndMode", "", "[%s,%s]GRPS绑定取消",vIdNo,iOldMode);
            EXEC SQL select count(*) into :vnum from dbillcustdetail where id_no=to_number(:vIdNo) and mode_code like 'gn22%' and mode_time='Y'
                and to_char(end_time,'yyyymmdd')>to_char(last_day(sysdate)+1,'yyyymmdd');
            if(vnum==0)
            {
                exec sql insert into wCustExpire(
                     command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                     total_date,op_code,op_time,function_code,command_code,business_status,
                     expire_time,op_note)
                     values(sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
                           :vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),'55','55','0',
                           to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'YYYYMMDD HH24:MI:SS'),:iOldMode||'绑定取消特服55');
                if(SQLCODE!=OK)
                {
                    pubLog_Debug(_FFL_, "pubBillEndMode", "", " .pubBillBeginMode :insert wCustExpire error[%d][%s]", oErrMsg);
                    return 127068;
                }
            }
    }
    */

	/**majha 20110630日增加VIP营销 begin
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/
	EXEC Sql select card_code
			into :vOldCardCode
			from sgnsalemode
			where region_code=substr(:vBelongCode,1,2)
			and	mode_code=:iOldMode;
	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		strcpy(oErrMsg, "查询sgnsalemode错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100057"," [%s] ", oErrMsg);
		return 100060;
	}
	if(SQLCODE==0)
	{
		EXEC SQL SELECT init_card_code
					into :vNewCardCode
					from dcusthigh_sale
				where id_no=to_number(:vIdNo);
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(oErrMsg, "查询dcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100061"," [%s] ", oErrMsg);
			return 100060;
		}
		if(SQLCODE==1403)
		{
			strcpy(vNewCardCode,"00");
		}
		EXEC SQL insert into tcusthigh_sale
				(login_accept,id_no,phone_no,login_no,op_code,
				mode_code,op_type,old_card_code,new_card_code,
				region_code,op_time,deal_flag)
			values(sMaxSysAccept.nextval,to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,
				  :iOldMode,'d',:vOldCardCode,:vNewCardCode,
				  substr(:vBelongCode,1,2),sysdate,'0');
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录tcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100059"," [%s] ", oErrMsg);
			return 100062;
		}
	}
	/**majha 20110630日增加VIP营销 end
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubBillEndMode", "", " pubBillEndMode end ");
#endif

	return 0;
}
/**********************************************
 @wt  PRG  : pubBillBeginMode
 @wt  FUNC : 取消iPhoneNo的iNewMode资费
 @wt       : 其中资费总生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt       : 操作时间iOpTime
 @wt       : 资费总的生效方式iSendFlag
 @wt       : 资费中所有的个性批价信息组合串iFlagStr
 @wt       : iFlagStr的格式为
 @wt       : aaaa^A1^A2^:bbbb^B1^B2^:cccc^C1^C2^C3^:
 @wt       : aaaa,bbbb,cccc为个性批价代码
 @wt       : A1、A2为aaaa的信息组合;B1、B2为bbbb的信息组合……
 @wt -----------------------------------------------------
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iFlagStr     计费的附加信息串
 @wt 10 vBunchNo    手机串号资源（产品改造新增参数）
 @wt 9 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/

int pubBillBeginMode(
	char *iPhoneNo,
	char *iNewMode,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *iSendFlag,
	char *iFlagStr,
	char *iBunchNo,
	char *oErrMsg)
{
	/* BOSS2.0 lixg add begin: 20070410 */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	char vBCtrlCode[5+1];
	char dynStmt[1024];

	/* BOSS2.0 lixg add end: 20070410 */
	EXEC SQL BEGIN DECLARE SECTION;
		char    vIdNo[23];
		int     i=0,t=0,vTotalDate=0,vFavOrder=0;
		char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
		int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
		char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
		char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
		char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
		int     vSendFlag,vUserFunc=-1;
		char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
		double  vMocRate=0,vMtcRate=0;
		char    vTimeCode[3];
		tUserBase   userBase;
		float       vPrepayFee=0;

		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		char vLastBIllType[1+1];
		char vBeginFlag[1+1];

		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];
		int	 retValue=0;
		int	 vCount=0;
		char vPromName[20+1];
		char vOrgId[10+1];
		char vNewCardCode[2+1];
		char vOldCardCode[2+1];

		char vPhoneNo1[15+1];
		char vFuncCode1[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		char vGroupId[10+1];
		TPubBillInfo tPubBillInfo;

		char vRegionCode[2+1];
		int  vMaxNum, vCurNum, vBuyNum;
		char vNewType[1+1];
		
		char vDealFunc[2+1];/*功能代码 add by chendx@20111228 */
		int  funcflag = 0;/*单套餐单功能标志，1表示单套餐单功能*/

		/*ng 解耦*/
		char vEffectTime[17+1];
		double  vMocRateNum=0,vMtcRateNum=0;
		char vMocRateStr[8+1];
		char vMtcRateStr[8+1];
		char vFavOrderStr[4+1];
		char vTotalDateStr[8+1];
		int  iNum = 0;
		int	 iNum1 = 0;
		TdBaseFav 			vTdBaseFav;
		TdBaseVpmn 			vTdBaseVpmn;
		TdBillCustDetail 	vTdBillCustDetail;
		TdCustExpire 		vTdCustExpire;
		TdCustFunc			vTdCustFunc;
		TdCustFuncIndex		vTdCustFuncIndex;
		TdCustFuncHis		vTdCustFuncHis;

		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.07.26 end*/

	EXEC SQL END   DECLARE SECTION;

	Sinitn(dynStmt);
	Sinitn(vPhoneNo1);
	Sinitn(vFuncCode1);
	Sinitn(vRegionCode);
	Sinitn(vOrgId);
	Sinitn(vEffectTime);
	Sinitn(v_parameter_array);
	Sinitn(vMocRateStr);
	Sinitn(vMtcRateStr);
	Sinitn(vFavOrderStr);
	Sinitn(vTotalDateStr);
	Sinitn(vNewCardCode);
	Sinitn(vOldCardCode);

	Sinitn(vDealFunc);

	Trim(iPhoneNo);
	Trim(iNewMode);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);
	/*Trim(iSendFlag);
	Trim(iFlagStr);*/

	memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
	/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.07.26 end*/


	oErrMsg[0] = '\0';
	/*init(oErrMsg);*/

	/*if(strcmp(iOpCode,"2933")==0)
	{
		strcpy(iSendFlag,"0");
		strcpy(iFlagStr,"");
	}
	Trim(iFlagStr);
	Trim(iSendFlag);*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "",""," begin ----- ");
	pubLog_Debug(_FFL_, "",""," iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "",""," iNewMode=[%s] ",iNewMode);
	pubLog_Debug(_FFL_, "",""," iEffectTime=[%s] ",iEffectTime);
	pubLog_Debug(_FFL_, "",""," iOpCode=[%s]  ",iOpCode);
	pubLog_Debug(_FFL_, "",""," iLoginNo=[%s] ",iLoginNo);
	pubLog_Debug(_FFL_, "",""," iLoginAccept=[%s] ",iLoginAccept);
	pubLog_Debug(_FFL_, "",""," iOpTime=[%s]  ",iOpTime);
	pubLog_Debug(_FFL_, "",""," iSendFlag=[%s] ",iSendFlag);

	pubLog_Debug(_FFL_, "",""," iFlagStr=[%s] ",iFlagStr);
#endif
	
	Sinitn(vIdNo);
	Sinitn(vLastBIllType);
	Sinitn(vBeginFlag);
	Trim(iOpTime);
	EXEC SQL select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
		return 1024;
	}

	strncpy(vRegionCode, vBelongCode, 2);

	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
	int ret =0;
	init(vGroupId);
	ret = sGetUserGroupid(iPhoneNo,vGroupId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败!");
		return 200919;
	}
	Trim(vGroupId);
	/*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}
	
	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127075","[%s] ", oErrMsg);
		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}
	/* 检查资费关系结束 chenxuan boss3 */

/*
	EXEC SQL declare CurSndC01 cursor for
		select prom_name,deal_func
			from ssndcailcfg
			where region_code=substr(:vBelongCode,1,2)
    		and mode_code =:iNewMode;
	EXEC SQL open CurSndC01;
	for(i=0;;)
	{
		init(vPromName);
		init(vDealFunc);
		EXEC SQL fetch CurSndC01 into :vPromName,:vDealFunc;
		if(SQLCODE!=0)
		{
			printf(" .pubBillBeginMode :select ssndcailcfg error [%d][%s] ", oErrMsg);
			break;
		}
		EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
										LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
										DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
					values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
							to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
							:vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
							to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_number(:iLoginAccept));
		if(SQLCODE!=0)
		{
				printf(" .pubBillBeginMode :insert wsndcailmsg error [%d][%s] ", oErrMsg);
        		strcpy(oErrMsg,"插入wsndcailmsg表错误!");
        		EXEC SQL close CurSndC01;
        		return 1024;
		}
	}
	EXEC SQL close CurSndC01;
*/


	Trim(vIdNo);
	Sinitn(vHlrCode);
	Sinitn(vImsiNo);Sinitn(vSimNo);

	if(strncmp(iNewMode,"kd",2)!=0 && strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0
		&& strncmp(iNewMode,"id",2)!=0 && strncmp(iNewMode,"af",2)!=0 && strncmp(iNewMode,"ad",2)!=0 && strncmp(iNewMode,"ma",2)!=0)
	{/*** 专线用户 ***/
		/*增加IMS用户判断 add by zhaohx at 20110526 */
		if(strncmp(iNewMode,"im",2)!=0 && strncmp(iNewMode,"iv",2)!=0 && strncmp(iNewMode,"dm",2)!=0 && strncmp(iNewMode,"mt",2)!=0 && strncmp(iNewMode,"tp",2)!=0)
		{
			EXEC SQL select hlr_code into :vHlrCode from sHlrCode
				where phoneno_head=substr(:iPhoneNo,1,7);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询sHrlCode错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
				return 127076;
			}

			EXEC SQL select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
				where id_no=to_number(:vIdNo);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询dCustSim错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
				return 127077;
			}
		}
	}

	 /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(iLoginNo,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败!");
		return 200919;
	}
	Trim(vOrgId);
	/*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127078"," [%s] ", oErrMsg);
		return 127078;
	}


	Trim(vOrgId);
	Sinitn(vTimeCode);
	EXEC SQL select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
	if(SQLCODE==OK&&SQLROWS==1){
		if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
		{
			Sinitn((void*)(&userBase));
			/*调用公用函数*/
			init(oErrMsg);
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			/*sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));*/
			sprintf(oErrMsg, "%06d", fGetUserBaseInfo_ng(iPhoneNo,iOpCode,&userBase));
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			Trim(oErrMsg);
			if( strcmp(oErrMsg, "000000") != 0 )
			{
				strcpy(oErrMsg,"查询用户信息错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

			vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
			pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
			vDeltaDays=0;


			EXEC SQL select days into :vDeltaDays from sPayTime
			where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='1'
				and begin_money<=:vPrepayFee and end_money>=:vPrepayFee;
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

		}

		Sinitn(vLastBIllType);
		Sinitn(vBeginFlag);
		EXEC SQL select last_bill_type into :vLastBIllType
			from dcustmsg
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK)
		{
			strcpy(oErrMsg,"取用户last_bill_type错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
			return 127078;
		}
		if(strncmp(vLastBIllType,"1",1)==0)
		{
			strcpy(vBeginFlag,"Y");
		}
		else
		{
			strcpy(vBeginFlag,"N");
		}

		/*ng解耦 by wxcrm at 20090812 begin
		exec sql insert into dCustExpire(ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME)
			values(to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}
		ng解耦 by wxcrm at 20090812 end*/
		EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	vBeginFlag		,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}

		exec sql insert into dCustExpireHis(
		    ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
		    UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
		values(
			to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
			return 127085;
		}

		/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
		exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
			values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期wExpireList错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
			return 127086;
		}
	}


	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
    EXEC SQL select a.deal_func into :vDealFunc
                from ssndcailcfg a,soutsidefeemode b
               where a.region_code = b.region_code
               	 and a.mode_code = b.mode_code
               	 and a.region_code = :vRegionCode
               	 and a.mode_code = :iNewMode;
    if(SQLCODE==OK&&SQLROWS==1)
	{
		printf("表示单套餐单功能!\n");
		funcflag = 1;
	}  
	printf("chendx 测试 [%d]\n",SQLCODE);        	 	
	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
	
	Sinitn(vModeFlag);
	Sinitn(vSmCode);
	init(vFavBrand);
	init(vBCtrlCode);
	
	EXEC SQL select a.mode_flag, a.sm_code, b.fav_brand, nvl(a.before_ctrl_code,' ')
		into :vModeFlag,:vSmCode,:vFavBrand, :vBCtrlCode
		from sBillModeCode a, sSmCode b
		where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
			and b.region_code=a.region_code and b.sm_code=a.sm_code;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127060","[%s] ", oErrMsg);
		return 127060;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	/* lixg add: 20070118 增加产品前项控制 */
	Trim(vBCtrlCode);
	if(strlen(vBCtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vBCtrlCode, iOpCode, iLoginNo, &fe) != 0)
		{
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, ""," atoi(tmpRetCode)",oErrMsg);
			return atoi(tmpRetCode);
		}
	}
#endif

	/* 处理产品包含的资费代码 */
	Sinitn(vSelSql);
	strcpy(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
		" from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2");
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubBillBeginMode %s ",vSelSql);
#endif
	EXEC SQL PREPARE preBeginDet FROM :vSelSql;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127079;
	}
	EXEC SQL DECLARE curBeginDet1 CURSOR FOR preBeginDet;
	EXEC SQL OPEN curBeginDet1 USING :vBelongCode, :iNewMode;
	for(i=0;;){
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
		EXEC SQL FETCH curBeginDet1
			into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
		if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
			exec sql close curBeginDet1;

			return 127061;
		}
		if(i!=0&&SQLCODE==NOTFOUND) break;

		/*****************************************************************************
		 ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
		 ** sBillModeDetail.time_unit 0天/1月/2年                                   **
		 ****************************************************************************/
		Sinitn(vBeginTime);
		Sinitn(vEndTime);
		switch (vTimeFlag[0]){
			case '0':
				strcpy(vBeginTime,iEffectTime);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '1':
				strcpy(vBeginTime,iEffectTime);
				pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime,vIdNo);
				break;
			case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
				strcpy(vEndTime,MAXENDTIME);
				break;
			default:;
		}

		Trim(vBeginTime);
		Trim(vEndTime);
		if (strcmp(vModeTime,"Y") == 0)
		{
			printf("funcflag[%d],iOpCode[%s],vDealFunc[%s]\n",funcflag,iOpCode,vDealFunc);
			/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
			/*chendx 测试使用1272，没用2976*/
			if(funcflag == 1 && strcmp(iOpCode,"2976") == 0 && strcmp(vDealFunc,"9d") == 0)
			{
				printf("网站进行套餐修改，且该套餐是单功能(手机报)，不需要发送短信提醒！\n");
			}/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
			else
			{
				retValue=DcustModeChgAwake(iOpCode,iNewMode,vBelongCode,"a",atol(iLoginAccept),iPhoneNo,iLoginNo,vBeginTime,vEndTime);
				if(retValue<0){
					EXEC SQL CLOSE curBeginDet1;
					strcpy(oErrMsg,"发送提醒短信错误!");
					PUBLOG_DBDEBUG("pubBillBeginMode");
					pubLog_DBErr(_FFL_, "","127099","[%s] ", oErrMsg);
					return 127099;
				}
			}
		}


		pubLog_Debug(_FFL_,"","","pubBillBeginMode  [%s~%s][%s:%s-%s]*%s* ",\
		vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);


		Sinitn(vBeginSql);
		sprintf(vBeginSql,
			" insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
			" fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
			" login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
			" :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
			":v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
			vIdNo[strlen(vIdNo)-1]);
		EXEC SQL PREPARE preWWW_5 FROM :vBeginSql;
		EXEC SQL EXECUTE preWWW_5 USING :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
			:vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
			:iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if (SQLCODE!=OK){
			sprintf(oErrMsg,"记录dBillCustDetail的历史错误[%d]!",SQLCODE);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_,"","","pubBillBeginMode %s ",vBeginSql);
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
			exec sql close curBeginDet1;
			return 127062;
		}

		Sinitn(vBeginSql);
		/*ng解耦 by wxcrm at 20090812 begin
		sprintf(vBeginSql,
			"insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,"
			" mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),:v6,:v7,"
			" :v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),:v13,to_number(:v14),substr(:v15,1,2))",
			vIdNo[strlen(vIdNo)-1]);
		#ifdef _WTPRN_
			pubLog_Debug(_FFL_,"","","pubBillBeginMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);

		exec sql prepare preWWW_6 from :vBeginSql;
		exec sql execute preWWW_6 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,:vFavOrder,:iNewMode,
			:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if(SQLCODE!=OK){
			exec sql close curBeginDet;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		ng解耦 by wxcrm at 20090812 end*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet1;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		switch (vDetailType[0]){
			/* lixg Add: 2005-11-20 begin */
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
				strcpy(vGrantMsg.modeCode, iNewMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.machineId, iBunchNo);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);
				pubLog_Debug(_FFL_,"","","product Begin ..... ");
				if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
					return iProductRetCode;
				}
				break;
			/* lixg Add: 2005-11-20 end */

			case '0':
				/** process bill system data begin **/
				init(vVpmnGroup);
				EXEC SQL select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate
					from sBillVpmnCond
					where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
				if (SQLROWS==1)
				{
					Trim(vVpmnGroup);


				  	/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseFav(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/

					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}


					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
						to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
						'1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083"," [%s] ", oErrMsg);
						return 127083;
					}
					#endif

					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
						values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
						to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/
					vMocRateNum = vMocRate*1000;
					vMtcRateNum = vMtcRate*1000;
					sprintf(vMocRateStr,"%f",vMocRateNum);
					sprintf(vMtcRateStr,"%f",vMtcRateNum);
					strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
					    '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083","[%s] ", oErrMsg);

					    return 127083;
					}
					#endif
				}

				if (iFlagStr[0]=='\0'){
					Sinitn(vFather);
					sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
				} else {
					strcpy(vFather,iFlagStr);
				}

				for (;;){
					Sinitn(vUncle);
					Sinitn(vSon);
					pubApartStr(vFather,':',vUncle,vSon);
					if (vSon[0]==vDetailCode[0]&&
							vSon[1]==vDetailCode[1]&&
							vSon[2]==vDetailCode[2]&&
							vSon[3]==vDetailCode[3])
					{
						Sinitn(vFather);
						Sinitn(vUncle);
						pubApartStr(vSon,'^',vFather,vUncle);
						for(;;){
							Sinitn(vUncle);
							Sinitn(vSon);
							pubApartStr(vFather,'^',vUncle,vSon);

							if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");


							pubLog_Debug(_FFL_,"","","iPhoneNo =[%s] ", iPhoneNo);
							pubLog_Debug(_FFL_,"","","favType =[%s] ", vDetailCode);
							pubLog_Debug(_FFL_,"","","flagCode =[%s] ", vSon);
							pubLog_Debug(_FFL_,"","","vBeginTime =[%s] ", vBeginTime);

							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dBaseFav(
								msisdn,fav_brand,fav_type,flag_code,
								fav_order,fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
								substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);
							/*EXEC SQL SELECT
								nvl(:iPhoneNo,'null'),nvl(:vSmCode,'null'),nvl(:vDetailCode,'null'),nvl(:vSon,'null'),nvl(to_char(:vFavOrder),'null'),
								nvl(:vFavDay,'null'),nvl(:vBeginTime,'null'),nvl(:vEndTime,'null'),
								nvl(substr(:vBelongCode,1,2),'null'),nvl(:vIdNo,'null'),nvl(:vGroupId,'null'),nvl(:iNewMode,'null')
								INTO :vTdBaseFav.sMsisdn,		:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,		:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,		:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,	:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,	:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,		:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}*/
							EXEC SQL SELECT count(*)
									INTO :iNum1
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum1!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"取sBillRateCode记录数错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								
								printf("\n---vDetailCode=%s\n",vDetailCode);
								return 127064;
							}

							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	vSon		);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] %d", oErrMsg,ret);
								
								return 127064;
							}
							#ifdef _CHGTABLE_
							EXEC SQL insert into wBaseFavChg(
								msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
								start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
								to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
								to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if(SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录wBaseFavChg错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127065","[%s] ", oErrMsg);

								return 127065;
							}
							#endif
							strcpy(vFather,vUncle);
							if(vFather[0]==0) break;
						}
						break;
					}
					strcpy(vFather,vUncle);
					if(vFather[0]=='\0') break;
				}
				/** process bill system data end **/
			case '1':case '2': case '3':case '4':case 'a':
				#ifdef _CHGTABLE_
				EXEC SQL insert into wUserFavChg(
					op_no,op_type,op_time,id_no,detail_type,detail_code,
					begin_time,end_time,fav_order,mode_code)
					values(
					sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
					to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
				if(SQLCODE!=OK){
					exec sql close curBeginDet1;
					strcpy(oErrMsg,"记录wUseFavChg错误!");
					PUBLOG_DBDEBUG("pubBillBeginMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);
					return 127066;
				}
				#endif

				if(vDetailType[0] != 'a') break;

				/** 处理特服绑定的情况 ***/
				vSendFlag=strcmp(vBeginTime,iOpTime);
				Sinitn(vFunctionType);Sinitn(vFunctionCode);
				Sinitn(vOffOn);Sinitn(vCommandCode);



				EXEC SQL select a.function_code,a.off_on,b.command_code,b.function_type
					into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
					from sBillFuncBind a,sFuncList b
					where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
					and b.region_code=a.region_code and b.sm_code=a.sm_code
					and b.function_code=a.function_code;
				if (SQLCODE!=OK){
					PUBLOG_DBDEBUG("pubBillBeginMode");
					pubLog_DBErr(_FFL_, "","127067","");
					return 127067;
				}

				if (vSendFlag > 0) /***预约 开/关***/
				{
					EXEC SQL select count(*) into :vUserFunc from wCustExpire
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
						and business_status=:vOffOn;
					if (vUserFunc == 0){
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						EXEC SQL insert into wCustExpire(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,ORG_ID)
							values(
							sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
							:vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
							to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
						if (SQLCODE != OK){
							PUBLOG_DBDEBUG("pubBillBeginMode");
							pubLog_DBErr(_FFL_, "","127068"," ");
							return 127068;
						}

						EXEC SQL insert into wCustExpireHis(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,ORG_ID)
							select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
							from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
						if (SQLCODE!=OK||SQLROWS!=1){
							PUBLOG_DBDEBUG("pubBillBeginMode");
							pubLog_DBErr(_FFL_, "","127069","");
							return 127069;
						}
					}
				}
				else { /***立即 开/关***/
					EXEC SQL select count(*) into :vUserFunc from dCustFunc
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

					if (vOffOn[0]=='1'){ /** 立即开 ***/
						if(vUserFunc==0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								values(
								to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(vTotalDateStr, 0, sizeof(vTotalDateStr));
							sprintf(vTotalDateStr,"%d",vTotalDate);
							strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}


							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFunc(id_no,function_type,function_code,op_time)
								values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
							ng解耦 by wxcrm at 20090812 end*/
							strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
						}
					}
					else {  /** 立即关 **/
						if (vUserFunc != 0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								select
								id_no,function_type,function_code,op_time,
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
								from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127072","");
								return 127072;
							}
							ng解耦 by wxcrm at 20090812 end*/

							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDateStr,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr50 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur501 CURSOR FOR ng_SqlStr50;
							EXEC SQL OPEN ng_Cur501 using :iLoginNo,:iLoginAccept,:vTotalDateStr,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur501 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode取dCustFuncHis主键");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127098;
								}

								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur501;



							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL delete dCustFunc where id_no = to_number(:vIdNo) and function_code = :vFunctionCode;
							if(SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode取dCustFunc主键");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
						}
					}

					EXEC SQL insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
						op_code,login_no,login_accept)
						values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
						to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
					if (SQLCODE != OK){
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127074","");
						return 127074;
					}

					if (iOpCode[0] != '8') /*** 非专线用户在立即生效时发送开关机命令 ***/
					{
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						
						/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
						if(strcmp(vFunctionCode,"02")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE a.id_no = :vIdNo
														AND a.function_code = '87';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS3");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						if(strcmp(vFunctionCode,"87")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE id_no = :vIdNo
														AND function_code = '02';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS1");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
						else
						{
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}

						if(strcmp(vFunctionCode,"95")==0)
						{
							Sinitn(dynStmt);
							sprintf(dynStmt,"insert into wSndCmdDay "
								"("
								"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,"
								"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
								"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT,"
								"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE,org_id, group_id"
								")"
								" select "
								" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, "
								" :v4, :v5, :v6, :v7, :v8, :v9,"
								" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14, to_number(:v15), "
								" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code, :v22, :v23 "
								" from sFuncList "
								" where region_code = substr(:v19, 1,2)"
								"   and sm_code = :v20"
								"   and function_code = :v21");

							if (vOffOn[0]=='1')
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"05");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :iPhoneNo, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId, :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;

								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
							else
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
						}
					}
				}

			default:;
		}
		i++;
	}
	EXEC SQL CLOSE curBeginDet1;

#if 0
	/* 开始处理服务代码 chenxuan boss3 */
	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre FROM :vSelSql;
	EXEC SQL DECLARE cur CURSOR FOR pre;
	EXEC SQL OPEN cur USING :vBelongCode, :iNewMode;
	for (;;) {
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入用户使用服务表 */
		EXEC SQL insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
				mode_code, login_accept)
			values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
				to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127070","");
			return 127070;
		}

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127071","");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	/* 调用指令分发函数 chenxuan boss3 */
	strcpy(tPubBillInfo.IdNo,        vIdNo);
	strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
	strcpy(tPubBillInfo.BelongCode,  vBelongCode);
	strcpy(tPubBillInfo.SmCode,      vSmCode);
	strcpy(tPubBillInfo.ModeCode,    iNewMode);
	strcpy(tPubBillInfo.OpCode,      iOpCode);
	strcpy(tPubBillInfo.OpTime,      iOpTime);
	strcpy(tPubBillInfo.EffectTime,  iEffectTime);
	tPubBillInfo.TotalDate = vTotalDate;
	strcpy(tPubBillInfo.LoginNo,     iLoginNo);
	strcpy(tPubBillInfo.LoginAccept, iLoginAccept);

	iProductRetCode = fDispatchCmd("1", &tPubBillInfo, oErrMsg);
	if (iProductRetCode != 0) {
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100050","");
		return 100050;
	}
	/* 指令分发函数结束 chenxuan boss3 */

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
		INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
		from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
			and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100051"," [%s] ", oErrMsg);
			return 100051;
		}
	    Trim(flag);
	    pubLog_Debug(_FFL_,"","","11111111111111111111=[%s] ",flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100052","[%s]",oErrMsg);

			return 100052;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0) && (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100053","[%s]",oErrMsg);
			return 100053;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

	/*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			  INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			  from sBillModeNum
			 where region_code = '00' and mode_code = :iNewMode
		       and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = '00' and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100054"," [%s] ", oErrMsg);
			return 100054;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0)&& (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100055","[%s]", oErrMsg);

			return 100055;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100056","[%s]", oErrMsg);
			return 100056;
		}
	}
	/*增加全省统一数量控制功能 20081119  end*/

	/**majha 20110630日增加VIP营销 begin
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/
	EXEC Sql select card_code
			into :vNewCardCode
			from sgnsalemode
			where region_code=substr(:vBelongCode,1,2)
			and	mode_code=:iNewMode;
	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		strcpy(oErrMsg, "查询sgnsalemode错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100057"," [%s] ", oErrMsg);
		return 100057;
	}
	if(SQLCODE==0)
	{
		EXEC SQL select substr(attr_code,3,2)
				into :vOldCardCode
				from dcustmsg
				where phone_no=:iPhoneNo;
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "查询dcustmsg错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100058"," [%s] ", oErrMsg);
			return 100058;
		}
		EXEC SQL insert into tcusthigh_sale
				(login_accept,id_no,phone_no,login_no,op_code,
				mode_code,op_type,old_card_code,new_card_code,
				region_code,op_time,deal_flag)
			values(sMaxSysAccept.nextval,to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,
				  :iNewMode,'a',:vOldCardCode,:vNewCardCode,
				  substr(:vBelongCode,1,2),sysdate,'0');
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录tcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100059"," [%s][%s] ",iLoginAccept, oErrMsg);
			return 100059;
		}
		vCount=0;
		EXEC SQL select count(*)
					into :vCount
					from dcusthigh_sale
					where id_no=to_number(:vIdNo);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录tcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100060"," [%s] ", oErrMsg);
			return 100060;
		}
		if(vCount==0)
		{
			EXEC SQL insert into dcusthigh_sale
						(id_no,phone_no,init_card_code,op_time)
				values(to_number(:vIdNo),:iPhoneNo,:vOldCardCode,sysdate);
			if (SQLCODE != 0 )
			{
				strcpy(oErrMsg, "记录dcusthigh_sale错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","100061"," [%s] ", oErrMsg);
				return 100061;
			}
		}

	}
	/**majha 20110630日增加VIP营销 end
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/
	/*套餐开通时，对于立即生效，并且存在套餐折算表的套餐，记录一张新表（折算短信发送记录表）*/
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}
	
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubBillBeginMode end ----- ");
#endif
    return 0;
}
/**********************************************
 @wt  PRG  : pubBillBeginModeZs
 @wt  业务赠送新增，解决结束时间问题
 @wt  FUNC : 取消iPhoneNo的iNewMode资费
 @wt       : 其中资费总生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt       : 操作时间iOpTime
 @wt       : 资费总的生效方式iSendFlag
 @wt       : 资费中所有的个性批价信息组合串iFlagStr
 @wt       : iFlagStr的格式为
 @wt       : aaaa^A1^A2^:bbbb^B1^B2^:cccc^C1^C2^C3^:
 @wt       : aaaa,bbbb,cccc为个性批价代码
 @wt       : A1、A2为aaaa的信息组合;B1、B2为bbbb的信息组合……
 @wt -----------------------------------------------------
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iFlagStr     计费的附加信息串
 @wt 10 vBunchNo    手机串号资源（产品改造新增参数）
 @wt 9 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/

int pubBillBeginModeZs(
	char *iPhoneNo,
	char *iNewMode,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *iSendFlag,
	char *iFlagStr,
	char *iBunchNo,
	char *oErrMsg,
	char *iEndTime)
{
	/* BOSS2.0 lixg add begin: 20070410 */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	char vBCtrlCode[5+1];
	char    dynStmt[1024];

	/* BOSS2.0 lixg add end: 20070410 */
	EXEC SQL BEGIN DECLARE SECTION;
		char    vIdNo[23];
		int     i=0,t=0,vTotalDate=0,vFavOrder=0;
		char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
		int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
		char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
		char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
		char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
		int     vSendFlag,vUserFunc=-1;
		char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
		double  vMocRate=0,vMtcRate=0;
		char    vTimeCode[3];
		tUserBase   userBase;
		float       vPrepayFee=0;

		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		char vLastBIllType[1+1];
		char vBeginFlag[1+1];

		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];
		int		retValue=0;
		int	 vCount=0;
		char vPromName[20+1];
		char vDealFunc[2+1];
		char vOrgId[10+1];

		char vPhoneNo1[15+1];
		char vFuncCode1[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		  char vGroupId[10+1];
		TPubBillInfo tPubBillInfo;

		char vRegionCode[2+1];
		int vMaxNum, vCurNum, vBuyNum;
		char vNewType[1+1];

		/*ng 解耦*/
		char vEffectTime[17+1];
		double  vMocRateNum=0,vMtcRateNum=0;
		char vMocRateStr[8+1];
		char vMtcRateStr[8+1];
		char vFavOrderStr[4+1];
		char vTotalDateStr[8+1];
		int  iNum = 0;
		int	 iNum1 = 0;
		TdBaseFav 			vTdBaseFav;
		TdBaseVpmn 			vTdBaseVpmn;
		TdBillCustDetail 	vTdBillCustDetail;
		TdCustExpire 		vTdCustExpire;
		TdCustFunc			vTdCustFunc;
		TdCustFuncIndex		vTdCustFuncIndex;
		TdCustFuncHis		vTdCustFuncHis;

		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.07.26 end*/
	EXEC SQL END   DECLARE SECTION;

	Sinitn(dynStmt);
	Sinitn(vPhoneNo1);
	Sinitn(vFuncCode1);
	Sinitn(vRegionCode);
	Sinitn(vOrgId);
	Sinitn(vEffectTime);
	Sinitn(v_parameter_array);
	Sinitn(vMocRateStr);
	Sinitn(vMtcRateStr);
	Sinitn(vFavOrderStr);
	Sinitn(vTotalDateStr);

	Trim(iPhoneNo);
	Trim(iNewMode);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);
	/*Trim(iSendFlag);
	Trim(iFlagStr);*/

	memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));

	/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.07.26 end*/

	oErrMsg[0] = '\0';
	/*init(oErrMsg);*/

	/*if(strcmp(iOpCode,"2933")==0)
	{
		strcpy(iSendFlag,"0");
		strcpy(iFlagStr,"");
	}
	Trim(iFlagStr);
	Trim(iSendFlag);*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "",""," begin ----- ");
	pubLog_Debug(_FFL_, "",""," iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "",""," iNewMode=[%s] ",iNewMode);
	pubLog_Debug(_FFL_, "",""," iEffectTime=[%s] ",iEffectTime);
	pubLog_Debug(_FFL_, "",""," iOpCode=[%s]  ",iOpCode);
	pubLog_Debug(_FFL_, "",""," iLoginNo=[%s] ",iLoginNo);
	pubLog_Debug(_FFL_, "",""," iLoginAccept=[%s] ",iLoginAccept);
	pubLog_Debug(_FFL_, "",""," iOpTime=[%s]  ",iOpTime);
	pubLog_Debug(_FFL_, "",""," iSendFlag=[%s] ",iSendFlag);

	pubLog_Debug(_FFL_, "",""," iFlagStr=[%s] ",iFlagStr);
#endif

	Sinitn(vIdNo);
	Sinitn(vLastBIllType);
	Sinitn(vBeginFlag);
	Trim(iOpTime);
	EXEC SQL select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
		return 1024;
	}
	/*if(strcmp(iOpCode,"1270")==0 || strcmp(iOpCode,"1272")==0 || strcmp(iOpCode,"1266")==0 || strcmp(iOpCode,"1268")==0 || strcmp(iOpCode,"3505")==0)
    {
		init(vAwakeModeName);
		init(vawakemsg);
		init(useFlag);
		strcpy(useFlag,"a");
		EXEC SQL select mode_name,awake_msg
					into :vAwakeModeName,:vawakemsg
					from sawakemodecode
					where region_code=substr(:vBelongCode,1,2)
					and awakemode_code=:iNewMode
					and use_flag=:useFlag;
		if(SQLCODE==0)
		{
			Trim(vAwakeModeName);
			Trim(vawakemsg);
			sprintf(vawakemsg,"中国移动温馨提示：您已经开通了%s业务套餐。",vAwakeModeName);
			EXEC SQL insert into wCommonShortMsg(COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,
    	   					PHONE_NO,MSG,ORDER_CODE,BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME,sent_time)
    	   			select SMSG_SEQUENCE.nextval,to_number(:iLoginAccept),1,:iPhoneNo,:vawakemsg,
    	   					0,0,:iOpCode,:iLoginNo,sysdate,''
    	   			from dual;
    	   	if(SQLCODE!=0 && SQLCODE!=1403)
    	   	{
    	   		printf(" .pubBillBeginMode :insert wCommonShortMsg error [%d][%s] ", oErrMsg);
    	    	strcpy(oErrMsg,"发送提醒短信错误!");
    	    	return 1024;
    	   	}
		}
	}*/

	strncpy(vRegionCode, vBelongCode, 2);

	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
           int ret =0;
           init(vGroupId);
           ret = sGetUserGroupid(iPhoneNo,vGroupId);
           if(ret <0)
             	{
					pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败!");
        			return 200919;
             	}
           Trim(vGroupId);
          /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}

	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127075","[%s] ", oErrMsg);
		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}
	/* 检查资费关系结束 chenxuan boss3 */

/*
	EXEC SQL declare CurSndC01 cursor for
		select prom_name,deal_func
			from ssndcailcfg
			where region_code=substr(:vBelongCode,1,2)
    		and mode_code =:iNewMode;
	EXEC SQL open CurSndC01;
	for(i=0;;)
	{
		init(vPromName);
		init(vDealFunc);
		EXEC SQL fetch CurSndC01 into :vPromName,:vDealFunc;
		if(SQLCODE!=0)
		{
			printf(" .pubBillBeginMode :select ssndcailcfg error [%d][%s] ", oErrMsg);
			break;
		}
		EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
										LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
										DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
					values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
							to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
							:vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
							to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_number(:iLoginAccept));
		if(SQLCODE!=0)
		{
				printf(" .pubBillBeginMode :insert wsndcailmsg error [%d][%s] ", oErrMsg);
        		strcpy(oErrMsg,"插入wsndcailmsg表错误!");
        		EXEC SQL close CurSndC01;
        		return 1024;
		}
	}
	EXEC SQL close CurSndC01;
*/


	Trim(vIdNo);
	Sinitn(vHlrCode);
	Sinitn(vImsiNo);Sinitn(vSimNo);
	/*
	if(iOpCode[0]!='8'){*** 专线用户 ***
	*/
	if(strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0
		&& strncmp(iNewMode,"id",2)!=0 && strncmp(iNewMode,"af",2)!=0 && strncmp(iNewMode,"ad",2)!=0 && strncmp(iNewMode,"ma",2)!=0){/*** 专线用户 ***/
		EXEC SQL select hlr_code into :vHlrCode from sHlrCode
			where phoneno_head=substr(:iPhoneNo,1,7);
		if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询sHrlCode错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
			return 127076;
		}

		EXEC SQL select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询dCustSim错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
			return 127077;
		}
	}

	 /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetLoginOrgid(iLoginNo,vOrgId);
       if(ret <0)
         	{
				pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败!");
				return 200919;
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127078"," [%s] ", oErrMsg);
		return 127078;
	}


	Trim(vOrgId);
	Sinitn(vTimeCode);
	EXEC SQL select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
	if(SQLCODE==OK&&SQLROWS==1){
		if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
		{
			Sinitn((void*)(&userBase));
			/*调用公用函数*/
			init(oErrMsg);
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			/*sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));*/
			sprintf(oErrMsg, "%06d", fGetUserBaseInfo_ng(iPhoneNo,iOpCode,&userBase));
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			Trim(oErrMsg);
			if( strcmp(oErrMsg, "000000") != 0 )
			{
				strcpy(oErrMsg,"查询用户信息错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

			vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
			pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
			vDeltaDays=0;


			EXEC SQL select days into :vDeltaDays from sPayTime
			where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='1'
				and begin_money<=:vPrepayFee and end_money>=:vPrepayFee;
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

		}

		Sinitn(vLastBIllType);
		Sinitn(vBeginFlag);
		EXEC SQL select last_bill_type into :vLastBIllType
			from dcustmsg
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK)
		{
			strcpy(oErrMsg,"取用户last_bill_type错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
			return 127078;
		}
		if(strncmp(vLastBIllType,"1",1)==0)
		{
			strcpy(vBeginFlag,"Y");
		}
		else
		{
			strcpy(vBeginFlag,"N");
		}

		/*ng解耦 by wxcrm at 20090812 begin
		exec sql insert into dCustExpire(ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME)
			values(to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}
		ng解耦 by wxcrm at 20090812 end*/
		EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	vBeginFlag		,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}

		exec sql insert into dCustExpireHis(
		    ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
		    UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
		values(
			to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
			return 127085;
		}

		/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
		exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
			values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期wExpireList错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
			return 127086;
		}
	}


	Sinitn(vModeFlag);Sinitn(vSmCode);init(vFavBrand);init(vBCtrlCode);
	EXEC SQL select a.mode_flag, a.sm_code, b.fav_brand, nvl(a.before_ctrl_code,' ')
		into :vModeFlag,:vSmCode,:vFavBrand, :vBCtrlCode
		from sBillModeCode a, sSmCode b
		where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
			and b.region_code=a.region_code and b.sm_code=a.sm_code;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127060","[%s] ", oErrMsg);
		return 127060;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	/* lixg add: 20070118 增加产品前项控制 */
	Trim(vBCtrlCode);
	if(strlen(vBCtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vBCtrlCode, iOpCode, iLoginNo, &fe) != 0)
		{
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, ""," atoi(tmpRetCode)",oErrMsg);
			return atoi(tmpRetCode);
		}
	}
#endif

	/* 处理产品包含的资费代码 */
	Sinitn(vSelSql);
	strcpy(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
		" from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2");
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubBillBeginMode %s ",vSelSql);
#endif
	EXEC SQL PREPARE preBeginDet FROM :vSelSql;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127079;
	}
	EXEC SQL DECLARE curBeginDetcor CURSOR FOR preBeginDet;
	EXEC SQL OPEN curBeginDetcor USING :vBelongCode, :iNewMode;
	for(i=0;;){
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
		EXEC SQL FETCH curBeginDetcor
			into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
		if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
			exec sql close curBeginDetcor;

			return 127061;
		}
		if(i!=0&&SQLCODE==NOTFOUND) break;

		/*****************************************************************************
		 ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
		 ** sBillModeDetail.time_unit 0天/1月/2年                                   **
		 ****************************************************************************/
		Sinitn(vBeginTime);
		Sinitn(vEndTime);
		switch (vTimeFlag[0]){
			case '0':
				strcpy(vBeginTime,iEffectTime);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '1':
				strcpy(vBeginTime,iEffectTime);
				pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime,vIdNo);
				break;
			case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
				strcpy(vEndTime,MAXENDTIME);
				break;
			default:;
		}
		/**业务产品赠送结束时间特俗指定**/
		Trim(iEndTime);
	  if(strlen(iEndTime)>0)
	  {
	  	Sinitn(vEndTime);
	  	strcpy(vEndTime,iEndTime);
	  }


		Trim(vBeginTime);
		Trim(vEndTime);
		if (strcmp(vModeTime,"Y") == 0){
			/*printf("******vBeginTime=[%s]vEndTime=[%s]********** ",vBeginTime,vEndTime);*/
			retValue=DcustModeChgAwake(iOpCode,iNewMode,vBelongCode,"a",atol(iLoginAccept),iPhoneNo,iLoginNo,vBeginTime,vEndTime);
			if(retValue<0){
				EXEC SQL CLOSE curBeginDetcor;
				strcpy(oErrMsg,"发送提醒短信错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127099","[%s] ", oErrMsg);
				return 127099;
			}
		}

#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubBillBeginMode  [%s~%s][%s:%s-%s]*%s* ",\
		vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);
#endif

		Sinitn(vBeginSql);
		sprintf(vBeginSql,
			" insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
			" fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
			" login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
			" :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
			":v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
			vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubBillBeginMode %s ",vBeginSql);
#endif
		EXEC SQL PREPARE preWWW_5 FROM :vBeginSql;
		EXEC SQL EXECUTE preWWW_5 USING :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
			:vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
			:iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if (SQLCODE!=OK){
			sprintf(oErrMsg,"记录dBillCustDetail的历史错误[%d]!",SQLCODE);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
			exec sql close curBeginDetcor;
			return 127062;
		}

		Sinitn(vBeginSql);
		/*ng解耦 by wxcrm at 20090812 begin
		sprintf(vBeginSql,
			"insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,"
			" mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),:v6,:v7,"
			" :v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),:v13,to_number(:v14),substr(:v15,1,2))",
			vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubBillBeginMode %s ",vBeginSql);
#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);

		exec sql prepare preWWW_6 from :vBeginSql;
		exec sql execute preWWW_6 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,:vFavOrder,:iNewMode,
			:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if(SQLCODE!=OK){
			exec sql close curBeginDet;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		ng解耦 by wxcrm at 20090812 end*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDetcor;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		switch (vDetailType[0]){
			/* lixg Add: 2005-11-20 begin */
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
				strcpy(vGrantMsg.modeCode, iNewMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.machineId, iBunchNo);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);
				pubLog_Debug(_FFL_,"","","product Begin ..... ");
				if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
					return iProductRetCode;
				}
				break;
			/* lixg Add: 2005-11-20 end */

			case '0':
				/** process bill system data begin **/
				init(vVpmnGroup);
				EXEC SQL select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate
					from sBillVpmnCond
					where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
				if (SQLROWS==1)
				{
					Trim(vVpmnGroup);


				  	/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseFav(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/

					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}


					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
						to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
						'1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083"," [%s] ", oErrMsg);
						return 127083;
					}
					#endif

					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
						values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
						to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/
					vMocRateNum = vMocRate*1000;
					vMtcRateNum = vMtcRate*1000;
					sprintf(vMocRateStr,"%f",vMocRateNum);
					sprintf(vMtcRateStr,"%f",vMtcRateNum);
					strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
					    '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083","[%s] ", oErrMsg);

					    return 127083;
					}
					#endif
				}

				if (iFlagStr[0]=='\0'){
					Sinitn(vFather);
					sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
				} else {
					strcpy(vFather,iFlagStr);
				}

				for (;;){
					Sinitn(vUncle);
					Sinitn(vSon);
					pubApartStr(vFather,':',vUncle,vSon);
					if (vSon[0]==vDetailCode[0]&&
							vSon[1]==vDetailCode[1]&&
							vSon[2]==vDetailCode[2]&&
							vSon[3]==vDetailCode[3])
					{
						Sinitn(vUncle);
						Sinitn(vFather);
						pubApartStr(vSon,'^',vFather,vUncle);
						for(;;){
							Sinitn(vUncle);
							Sinitn(vSon);
							pubApartStr(vFather,'^',vUncle,vSon);

							if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");


							pubLog_Debug(_FFL_,"","","iPhoneNo =[%s] ", iPhoneNo);
							pubLog_Debug(_FFL_,"","","favType =[%s] ", vDetailCode);
							pubLog_Debug(_FFL_,"","","flagCode =[%s] ", vSon);
							pubLog_Debug(_FFL_,"","","vBeginTime =[%s] ", vBeginTime);

							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dBaseFav(
								msisdn,fav_brand,fav_type,flag_code,
								fav_order,fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
								substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);
							/*EXEC SQL SELECT
								nvl(:iPhoneNo,'null'),nvl(:vSmCode,'null'),nvl(:vDetailCode,'null'),nvl(:vSon,'null'),nvl(to_char(:vFavOrder),'null'),
								nvl(:vFavDay,'null'),nvl(:vBeginTime,'null'),nvl(:vEndTime,'null'),
								nvl(substr(:vBelongCode,1,2),'null'),nvl(:vIdNo,'null'),nvl(:vGroupId,'null'),nvl(:iNewMode,'null')
								INTO :vTdBaseFav.sMsisdn,		:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,		:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,		:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,	:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,	:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,		:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}*/
							EXEC SQL SELECT count(*)
									INTO :iNum1
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum1!=1){
								exec sql close curBeginDetcor;
								strcpy(oErrMsg,"取sBillRateCode记录数错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}

							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	vSon		);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDetcor;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}
							#ifdef _CHGTABLE_
							EXEC SQL insert into wBaseFavChg(
								msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
								start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
								to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
								to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if(SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDetcor;
								strcpy(oErrMsg,"记录wBaseFavChg错误!");
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127065","[%s] ", oErrMsg);

								return 127065;
							}
							#endif
							strcpy(vFather,vUncle);
							if(vFather[0]==0) break;
						}
						break;
					}
					strcpy(vFather,vUncle);
					if(vFather[0]=='\0') break;
				}
				/** process bill system data end **/
			case '1':case '2': case '3':case '4':case 'a':
				#ifdef _CHGTABLE_
				EXEC SQL insert into wUserFavChg(
					op_no,op_type,op_time,id_no,detail_type,detail_code,
					begin_time,end_time,fav_order,mode_code)
					values(
					sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
					to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
				if(SQLCODE!=OK){
					exec sql close curBeginDetcor;
					strcpy(oErrMsg,"记录wUseFavChg错误!");
					PUBLOG_DBDEBUG("pubBillBeginMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);
					return 127066;
				}
				#endif

				if(vDetailType[0] != 'a') break;

				/** 处理特服绑定的情况 ***/
				vSendFlag=strcmp(vBeginTime,iOpTime);
				Sinitn(vFunctionType);Sinitn(vFunctionCode);
				Sinitn(vOffOn);Sinitn(vCommandCode);



				EXEC SQL select a.function_code,a.off_on,b.command_code,b.function_type
					into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
					from sBillFuncBind a,sFuncList b
					where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
					and b.region_code=a.region_code and b.sm_code=a.sm_code
					and b.function_code=a.function_code;
				if (SQLCODE!=OK){
					PUBLOG_DBDEBUG("pubBillBeginMode");
					pubLog_DBErr(_FFL_, "","127067","");
					return 127067;
				}

				if (vSendFlag > 0) /***预约 开/关***/
				{
					EXEC SQL select count(*) into :vUserFunc from wCustExpire
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
						and business_status=:vOffOn;
					if (vUserFunc == 0){
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						EXEC SQL insert into wCustExpire(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,ORG_ID)
							values(
							sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
							:vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
							to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
						if (SQLCODE != OK){
							PUBLOG_DBDEBUG("pubBillBeginMode");
							pubLog_DBErr(_FFL_, "","127068"," ");
							return 127068;
						}

						EXEC SQL insert into wCustExpireHis(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,ORG_ID)
							select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
							from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
						if (SQLCODE!=OK||SQLROWS!=1){
							PUBLOG_DBDEBUG("pubBillBeginMode");
							pubLog_DBErr(_FFL_, "","127069","");
							return 127069;
						}
					}
				}
				else { /***立即 开/关***/
					EXEC SQL select count(*) into :vUserFunc from dCustFunc
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

					if (vOffOn[0]=='1'){ /** 立即开 ***/
						if(vUserFunc==0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								values(
								to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(vTotalDateStr, 0, sizeof(vTotalDateStr));
							sprintf(vTotalDateStr,"%d",vTotalDate);
							strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}


							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFunc(id_no,function_type,function_code,op_time)
								values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
							ng解耦 by wxcrm at 20090812 end*/
							strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
						}
					}
					else {  /** 立即关 **/
						if (vUserFunc != 0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								select
								id_no,function_type,function_code,op_time,
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
								from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127072","");
								return 127072;
							}
							ng解耦 by wxcrm at 20090812 end*/

							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDateStr,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr50 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur502 CURSOR FOR ng_SqlStr50;
							EXEC SQL OPEN ng_Cur502 using :iLoginNo,:iLoginAccept,:vTotalDateStr,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur502 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode取dCustFuncHis主键");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur502;
									return 127098;
								}

								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur502;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur502;



							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL delete dCustFunc where id_no = to_number(:vIdNo) and function_code = :vFunctionCode;
							if(SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Curor CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Curor using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Curor INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode取dCustFunc主键");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Curor;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginMode");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Curor;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Curor;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
						}
					}

					EXEC SQL insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
						op_code,login_no,login_accept)
						values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
						to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
					if (SQLCODE != OK){
						PUBLOG_DBDEBUG("pubBillBeginMode");
						pubLog_DBErr(_FFL_, "","127074","");
						return 127074;
					}

					if (iOpCode[0] != '8') /*** 非专线用户在立即生效时发送开关机命令 ***/
					{
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						
						/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
						if(strcmp(vFunctionCode,"02")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE a.id_no = :vIdNo
														AND a.function_code = '87';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS3");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						if(strcmp(vFunctionCode,"87")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE id_no = :vIdNo
														AND function_code = '02';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS1");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
						else
						{
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}

						if(strcmp(vFunctionCode,"95")==0)
						{
							Sinitn(dynStmt);
							sprintf(dynStmt,"insert into wSndCmdDay "
								"("
								"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,"
								"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
								"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT,"
								"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE,org_id, group_id"
								")"
								" select "
								" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, "
								" :v4, :v5, :v6, :v7, :v8, :v9,"
								" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14, to_number(:v15), "
								" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code, :v22, :v23 "
								" from sFuncList "
								" where region_code = substr(:v19, 1,2)"
								"   and sm_code = :v20"
								"   and function_code = :v21");

							if (vOffOn[0]=='1')
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"05");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :iPhoneNo, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId, :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;

								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
							else
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
						}
					}
				}

			default:;
		}
		i++;
	}
	EXEC SQL CLOSE curBeginDetcor;

#if 0
	/* 开始处理服务代码 chenxuan boss3 */
	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre FROM :vSelSql;
	EXEC SQL DECLARE cur CURSOR FOR pre;
	EXEC SQL OPEN cur USING :vBelongCode, :iNewMode;
	for (;;) {
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入用户使用服务表 */
		EXEC SQL insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
				mode_code, login_accept)
			values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
				to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127070","");
			return 127070;
		}

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127071","");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	/* 调用指令分发函数 chenxuan boss3 */
	strcpy(tPubBillInfo.IdNo,        vIdNo);
	strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
	strcpy(tPubBillInfo.BelongCode,  vBelongCode);
	strcpy(tPubBillInfo.SmCode,      vSmCode);
	strcpy(tPubBillInfo.ModeCode,    iNewMode);
	strcpy(tPubBillInfo.OpCode,      iOpCode);
	strcpy(tPubBillInfo.OpTime,      iOpTime);
	strcpy(tPubBillInfo.EffectTime,  iEffectTime);
	tPubBillInfo.TotalDate = vTotalDate;
	strcpy(tPubBillInfo.LoginNo,     iLoginNo);
	strcpy(tPubBillInfo.LoginAccept, iLoginAccept);

	iProductRetCode = fDispatchCmd("1", &tPubBillInfo, oErrMsg);
	if (iProductRetCode != 0) {
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100050","");
		return 100050;
	}
	/* 指令分发函数结束 chenxuan boss3 */

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
		INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
		from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
			and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100051"," [%s] ", oErrMsg);
			return 100051;
		}
	    Trim(flag);
	    pubLog_Debug(_FFL_,"","","11111111111111111111=[%s] ",flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100052","[%s]",oErrMsg);

			return 100052;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0) && (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100053","[%s]",oErrMsg);
			return 100053;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

	/*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			  INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			  from sBillModeNum
			 where region_code = '00' and mode_code = :iNewMode
		       and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = '00' and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100054"," [%s] ", oErrMsg);
			return 100054;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0)&& (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100055","[%s]", oErrMsg);

			return 100055;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100056","[%s]", oErrMsg);
			return 100056;
		}
	}
	/*增加全省统一数量控制功能 20081119  end*/
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubBillBeginMode end ----- ");
#endif
    return 0;
}

/********************************************************
 @wt PRG  : pubBillCheck
 @wt DATE : 2003.12.22
 @wt EDIT : wangtao
 @wt FUNC : 检验用户的主套餐优惠时间是否存在重叠或空白，
 @wt        非主套餐优惠时间是否存在重叠
 @wt PARM :
 @wt 0 iIdNo         用户ID
 @wt 1 oCurBaseMode  当前主资费代码
 @wt 1 oCurAccept    当前主资费的开通流水
 @wt 2 oNextBaseMode 下月主资费代码
 @wt 2 oNextAccept   下月主资费的开通流水
 @wt 1 oErrMsg       函数内部错误信息
 *******************************************************/
int pubBillCheck(
    char    *iIdNo,
    char    *oCurBaseMode,
    char    *oCurAccept,
    char    *oNextBaseMode,
    char    *oNextAccept,
    char    *oErrMsg)
{
    exec sql begin declare section;
        char vSqlStr[1204],vModeFlag[2],vModeCode[9],vTmBegin[18],vTmEnd[18],vLoginAccept[23];
        char vModeFlagTmp[2],vModeCodeTmp[9],vTmBeginTmp[18],vTmEndTmp[18];
        char vTmNow[18],vTmNext[18];
        int  i;
    exec sql end   declare section;

    Trim(iIdNo);
    init(oCurBaseMode);
    init(oCurAccept);
    init(oNextBaseMode);
    init(oNextAccept);
    init(oErrMsg);

    init(vTmNow);
    init(vTmNext);
    exec sql select to_char(sysdate,'yyyymmdd hh24:mi:ss'),to_char(add_months(sysdate,1),'yyyymm')||'01 00:00:00'
    into :vTmNow,:vTmNext from dual;

    /** 检验主资费是否有优惠重叠或优惠空白 **/
    init(vSqlStr);
    sprintf(vSqlStr,"select mode_code,to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time,'yyyymmdd hh24:mi:ss'),"
    " to_char(login_accept) from dBillCustDetail%c where id_no=to_number(:v1) and mode_time='Y' and mode_flag='0' and begin_time<end_time and sysdate <=end_time"
    " order by begin_time asc",iIdNo[strlen(iIdNo)-1]);
    exec sql prepare preCheckBasic from :vSqlStr;
    if(SQLCODE!=OK)
    {
		 strcpy(oErrMsg,"初始化检验用户主套餐优惠时间时发生错误!");
    	PUBLOG_DBDEBUG("pubBillCheck");
		pubLog_DBErr(_FFL_, "","127090","%s ",oErrMsg);
        return 127090;
    }
    exec sql declare curCheckBasic cursor for preCheckBasic;
    exec sql open curCheckBasic using :iIdNo;
    for(i=0,init(vModeFlagTmp),init(vTmBeginTmp),init(vTmEndTmp);;i++)
    {
        init(vModeCode);
        init(vTmBegin);
        init(vTmEnd);
        init(vLoginAccept);
        exec sql fetch curCheckBasic into :vModeCode,:vTmBegin,:vTmEnd,:vLoginAccept;
        if(SQLCODE==NOTFOUND) break;
        if(SQLCODE!=OK)
        {
            exec sql close curCheckBasic;
			strcpy(oErrMsg,"检验用户主套餐优惠时间时发生错误!");
            PUBLOG_DBDEBUG("pubBillCheck");
			pubLog_DBErr(_FFL_, "","127091","%s ",oErrMsg);

            return 127091;
        }
        if(strcmp(vTmNow,vTmBegin)>=0&&strcmp(vTmNow,vTmEnd)<0)
        {
            strcpy(oCurBaseMode,vModeCode);
            strcpy(oCurAccept,vLoginAccept);
        }
        if(strcmp(vTmNext,vTmBegin)>=0&&strcmp(vTmNext,vTmEnd)<0)
        {
            strcpy(oNextBaseMode,vModeCode);
            strcpy(oNextAccept,vLoginAccept);
        }

/**        printf("---%s|%s|%s---- ",vModeCode,vTmBegin,vTmEnd);  **/
        if(i>0)
        {
            if(strcmp(vTmBegin,vTmEndTmp)<0)
            {
                exec sql close curCheckBasic;
                strcpy(oErrMsg,"该用户主套餐优惠时间存在重叠!");
                pubLog_DBErr(_FFL_, "","127092","%s",oErrMsg);
                return 127092;
            }
            /**
            if(strcmp(vTmBegin,vTmEndTmp)>0)
            {
                printf("error on %s,%s,%s. ",vModeCode,vTmBegin,vTmEndTmp);
                exec sql close curCheckBasic;
                strcpy(oErrMsg,"该用户主套餐优惠时间存在空白!");
                return 127093;
            }
            ***/
        }
        strcpy(vTmBeginTmp,vTmBegin);
        strcpy(vTmEndTmp,vTmEnd);
    }
    exec sql close curCheckBasic;

    /** 检验非主资费是否有优惠重叠 **/
    init(vSqlStr);
    sprintf(vSqlStr,"select mode_code,to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time,'yyyymmdd hh24:mi:ss') "
    " from dBillCustDetail%c where id_no=to_number(:v1) and mode_time='Y' and mode_flag!='0' and begin_time<end_time and sysdate <=end_time"
    " order by mode_code asc,begin_time asc",iIdNo[strlen(iIdNo)-1]);
    exec sql prepare preCheckAdd from :vSqlStr;
    if(SQLCODE!=OK)
    {
    	strcpy(oErrMsg,"初始化检验用户非主套餐优惠时间时发生错误!");
    	PUBLOG_DBDEBUG("pubBillCheck");
		pubLog_DBErr(_FFL_, "","127094","%s ",oErrMsg);
        return 127094;
    }
    exec sql declare curCheckAdd cursor for preCheckAdd;
    exec sql open curCheckAdd using :iIdNo;
    for(init(vModeCodeTmp),init(vTmBeginTmp),init(vTmEndTmp);;)
    {
        init(vModeCode);
        init(vTmBegin);
        init(vTmEnd);
        exec sql fetch curCheckAdd into :vModeCode,:vTmBegin,:vTmEnd;
        if(SQLCODE==NOTFOUND) break;
        if(SQLCODE!=OK)
        {
            exec sql close curCheckAdd;
			 strcpy(oErrMsg,"检验用户非主套餐优惠时间时发生错误!");
            PUBLOG_DBDEBUG("pubBillCheck");
			pubLog_DBErr(_FFL_, "","127095","%s ",oErrMsg);
            return 127095;
        }
        /** printf("---%s|%s|%s*%s|%s|%s---- ",vModeCode,vTmBegin,vTmEnd,vModeCodeTmp,vTmBeginTmp,vTmEndTmp);  **/
        if(strcmp(vModeCode,vModeCodeTmp)!=0)
        {
            strcpy(vModeCodeTmp,vModeCode);
        }
        else
        {
            if(strcmp(vTmBegin,vTmEndTmp)<0)
            {
                exec sql close curCheckAdd;
                init(oErrMsg);
                sprintf(oErrMsg,"该用户套餐%s优惠时间存在重叠!",vModeCode);
                pubLog_DBErr(_FFL_, "","127092","%s",oErrMsg);
                return 127092;
            }
        }
        strcpy(vTmBeginTmp,vTmBegin);
        strcpy(vTmEndTmp,vTmEnd);
    }
    exec sql close curCheckAdd;

    return 0;
}
/**********************************************
 * PRG  : pubBillOffOn  未启用
 * FUNC : 当用户iPhoneNo将主资费由iOldMode
 *      : 变成iNewMode的时候，生效时间为iEffectTime
 *      : 系统当前时间为iOpTime
 *      : 根据sBillOffOn进行开关机命令
 *      : 其中iOpCode为功能代码
 *      : iLoginNo为操作工号
 *      : iLoginAccept为操作流水
 *      : iOpTime为系统当前时间
 * 返回结果
 *     int pubBillOffOn      !=0 error
 *                           ==0 success
 *********************************************/
int pubBillOffOn(
    char *iPhoneNo,
    char *iOldMode,
    char *iNewMode,
    char *iEffectTime,
    char *iOpCode,
    char *iLoginNo,
    char *iLoginAccept,
    char *iOpTime)
{
    EXEC SQL BEGIN DECLARE SECTION;
        char    vIdNo[23],vOrgCode[10],vBelongCode[8],vImsiNo[21],vSimNo[21];
        char    vHlrCode[2],vSmCode[3];
        int     vTotalDate=0,vSendFlag=0,ret = 0;
        char  vOrgId[10+1];
        char  vGroupId[10+1];
    EXEC SQL END   DECLARE SECTION;

    Sinitn(vIdNo);Sinitn(vOrgCode);Sinitn(vBelongCode);Sinitn(vSmCode);Sinitn(vOrgId); Sinitn(vGroupId);

    /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/

       ret = sGetUserGroupid(iPhoneNo,vGroupId);
       if(ret <0)
         	{
         		PUBLOG_DBDEBUG("pubBillOffOn");
				pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败! ");
         	  	return 200919;
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    exec sql select to_char(id_no),belong_code,sm_code into :vIdNo,:vBelongCode,:vSmCode
    from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
    if(SQLCODE!=OK){
    	PUBLOG_DBDEBUG("pubBillOffOn");
		pubLog_DBErr(_FFL_, "","1024"," .pubBillOffOn :select dCustMsg error ");
        return 1024;
    }

    Sinitn(vHlrCode);
    exec sql select hlr_code into :vHlrCode from sHlrCode
    where phoneno_head=substr(:iPhoneNo,1,7);
    if(SQLCODE!=OK){
    	PUBLOG_DBDEBUG("pubBillOffOn");
		pubLog_DBErr(_FFL_, "","127076","pubBillOffOn :select sHlrCode error");
        return 127076;
    }

    Sinitn(vImsiNo);Sinitn(vSimNo);
    exec sql select switch_no,sim_no into :vImsiNo,:vSimNo
    from dCustSim where id_no=to_number(:vIdNo);
    if(SQLCODE!=OK){
    	PUBLOG_DBDEBUG("pubBillOffOn");
		pubLog_DBErr(_FFL_, "","127077"," .pubBillOffOn :select dCustSim error");
        return 127077;
    }

     /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetLoginOrgid(iLoginNo,vOrgId);
       if(ret <0)
         	{
				pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败! ");
				return 200919;
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    Sinitn(vOrgCode);
    exec sql select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
    if(SQLCODE!=OK){
    	PUBLOG_DBDEBUG("pubBillOffOn");
		pubLog_DBErr(_FFL_, "","127078"," .pubBillOffOn :select dLoginMsg error ");
        return 127078;
    }
    Trim(vOrgId);
    Trim(vGroupId);
    exec sql select sign(to_date(:iEffectTime)-to_date(:iOpTime)) into :vSendFlag from dual;
    if(vSendFlag>0){
    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
        exec sql insert into wCustExpire(
            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
            total_date,op_code,op_time,function_code,command_code,business_status,
            expire_time,op_note,org_id)
        select
            sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
            :vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),'ZZ',command_code,off_on,
            to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS'),'资费'||:iOldMode||'变更'||:iNewMode,:vOrgId
        from sBillOffOn
        where region_code=substr(:vBelongCode,1,2) and old_mode=:iOldMode and new_mode=:iNewMode;
        if(SQLCODE!=OK){
        	PUBLOG_DBDEBUG("pubBillOffOn");
			pubLog_DBErr(_FFL_, "","127095","pubBillOffOn insert wCustExpire error");
            return 127095;
        }

        exec sql insert into wCustExpireHis(
            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
            total_date,op_code,op_time,function_code,command_code,business_status,
            expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,org_id)
        select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
            total_date,op_code,op_time,function_code,command_code,business_status,
            expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a',org_id
        from wCustExpire where login_accept=:iLoginAccept and function_code='ZZ';
        if(SQLCODE!=OK||SQLROWS==0){
        	PUBLOG_DBDEBUG("pubBillOffOn");
			pubLog_DBErr(_FFL_, "","127096","pubBillOffOn :insert wCustExpireHis error");
            return 127096;
        }
    }
    else{
    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
        exec sql insert into wSndCmdDay(
            command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
            new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
            org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
        select
            sOffOn.nextval,:vHlrCode,order_code,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,command_code,
            :iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
            :vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),off_on,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOrgId,:vGroupId
        from sBillOffOn
        where region_code=substr(:vBelongCode,1,2) and old_mode=:iOldMode and new_mode=:iNewMode;
        if(SQLCODE!=OK){
        	PUBLOG_DBDEBUG("pubBillOffOn");
			pubLog_DBErr(_FFL_, "","127097"," pubBillOffOn :insert wSndSmdDay error");
            return 127097;
        }
    }

    return 0;
}
/************************************************************************
函数名称:fGetIdDeadOwe
编码时间:2004.02.19
编码人员:wallas wang
功能描述:根据用户ID取所有帐户的 dead 信息
输入参数:用户ID
输出参数:用户欠费信息,其中包括以下内容
用户总欠费
用户总预存
第一个帐号
第一个帐号欠费
返 回 值:0代表正确返回,其他错误
************************************************************************/

int fGetIdDeadOwe(const char *id_no, tUserOwe *user_owe)
{
    EXEC SQL BEGIN DECLARE SECTION;
    char    vid_no[22+1];
    char    vcontract_no[22+1];
    char    vst[512];
    double  vowe_fee=0;
    double  vprepay_fee=0;
    int     vtmp_num=0;
    int     ids=0;
    int     vret=0;
    char    phone_no[15+1];
    double  v_unbill=0;
    long    contract_no=0;
    EXEC SQL END DECLARE SECTION;

    memset(vid_no,0,sizeof(vid_no));
    memset(vcontract_no,0,sizeof(vcontract_no));
    memset(vst,0,sizeof(vst));
    init(phone_no);
    strcpy(vid_no,id_no);
    Trim(vid_no);
    sprintf(vst,
    "select nvl(sum(should_pay-favour_fee-prepay_fee-payed_prepay-payed_later),0) from dCustOweDeadTotal%c where id_no=to_number(:v1)",
        vid_no[strlen(vid_no)-1]);
    exec sql execute
        begin
            execute immediate :vst into :vowe_fee using :vid_no;
        end;
    end-exec;

    user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
    return 0;
}
/************************************************************************
函数名称:fGetIdBaseInfo
编码时间:2004/02/15
编码人员:wallas
功能描述:根据移动号码取用户的基本信息
输入参数:移动号码
输出参数:用户基本信息
用户id
客户id
belong_code
业务类型代码
业务类型名称
客户名称
用户密码
状态代码
状态名称
等级代码
等级名称
用户类型代码
用户类型名称
客户住址
证件类型
证件名称
证件号码
客户卡类型
客户卡名称
集团客户标志
集团客户名称
bak_field
用户欠费(tUserOwe)
返 回 值:0代表正确返回,其他错误
************************************************************************/
int fGetIdBaseInfo(const char *phone_no,tUserBase *user_base,char *vRealPhone)
/** phone_no为id_no(str) edited by wallas ***/
{
    EXEC SQL BEGIN DECLARE SECTION;
    /***下面是用到的主变量****/
    char        vphone_no[15+1];
    char        vid_no[23];
    char        vcust_id[23];
    char        vsm_code[2+1];
    char        vsm_name[20+1];
    char        vcust_name[60+1];
    char        vuser_passwd[8+1];
    char        vrun_code[1+1];
    char        vrun_name[20+1];
    char        vowner_grade[2+1];
    char        vgrade_name[20+1];
    char        vowner_type[2+1];
    char        vtype_name[20+1];
    char        vcust_address[60+1];
    char        vid_type[1+1];
    char        vid_name[12+1];
    char        vid_iccid[20+1];
    char        vcard_type[2+1];
    char        vcard_name[20+1];
    char        vgrpbig_flag[1+1];
    char        vgrpbig_name[20+1];
    char        vcontract_no[23];
    double      vowe_fee=0;
    double      vprepay_fee=0;
    char        vregion_code[2+1];
    char        vBelongCode[8];
    char        vst[512];
    int         vtmp_num=0;
    int         vret=0;
    char        vPhTmp[16];
    int         vDeadFlag;
    EXEC SQL END DECLARE SECTION;

    memset(vphone_no,0,sizeof(vphone_no));

    memset(user_base->id_no,0,sizeof(user_base->id_no));
    memset(user_base->cust_id,0,sizeof(user_base->cust_id));
    memset(user_base->belong_code,0,sizeof(user_base->belong_code));
    memset(user_base->sm_code,0,sizeof(user_base->sm_code));
    memset(user_base->sm_name,0,sizeof(user_base->sm_name));
    memset(user_base->cust_name,0,sizeof(user_base->cust_name));
    memset(user_base->user_passwd,0,sizeof(user_base->user_passwd));
    memset(user_base->run_code,0,sizeof(user_base->run_code));
    memset(user_base->run_name,0,sizeof(user_base->run_name));
    memset(user_base->owner_grade,0,sizeof(user_base->owner_grade));
    memset(user_base->grade_name,0,sizeof(user_base->grade_name));
    memset(user_base->owner_type,0,sizeof(user_base->owner_type));
    memset(user_base->type_name,0,sizeof(user_base->type_name));
    memset(user_base->cust_address,0,sizeof(user_base->cust_address));
    memset(user_base->id_type,0,sizeof(user_base->id_type));
    memset(user_base->id_name,0,sizeof(user_base->id_name));
    memset(user_base->id_iccid,0,sizeof(user_base->id_iccid));
    memset(user_base->card_type,0,sizeof(user_base->card_type));
    memset(user_base->card_name,0,sizeof(user_base->card_name));
    memset(user_base->grpbig_flag,0,sizeof(user_base->grpbig_flag));
    memset(user_base->grpbig_name,0,sizeof(user_base->grpbig_name));
    memset(user_base->bak_field,0,sizeof(user_base->bak_field));
    memset(user_base->user_owe.accountNo,0,sizeof(user_base->user_owe.accountNo));
    user_base->user_owe.totalOwe=0;
    user_base->user_owe.totalPrepay=0;
    user_base->user_owe.accountOwe=0;
    user_base->user_owe.unbillFee=0;

    memset(vid_no,0,sizeof(vid_no));
    memset(vcust_id,0,sizeof(vcust_id));
    memset(vcontract_no,0,sizeof(vcontract_no));
    memset(vBelongCode,0,sizeof(vBelongCode));
    memset(vsm_code,0,sizeof(vsm_code));
    memset(vsm_name,0,sizeof(vsm_name));
    memset(vcust_name,0,sizeof(vcust_name));
    memset(vuser_passwd,0,sizeof(vuser_passwd));
    memset(vrun_code,0,sizeof(vrun_code));
    memset(vrun_name,0,sizeof(vrun_name));
    memset(vowner_grade,0,sizeof(vowner_grade));
    memset(vgrade_name,0,sizeof(vgrade_name));
    memset(vowner_type,0,sizeof(vowner_type));
    memset(vtype_name,0,sizeof(vtype_name));
    memset(vcust_address,0,sizeof(vcust_address));
    memset(vid_type,0,sizeof(vid_type));
    memset(vid_name,0,sizeof(vid_name));
    memset(vid_iccid,0,sizeof(vid_iccid));

    memset(vcard_type,0,sizeof(vcard_type));
    memset(vcard_name,0,sizeof(vcard_name));
    memset(vgrpbig_flag,0,sizeof(vgrpbig_flag));
    memset(vgrpbig_name,0,sizeof(vgrpbig_name));

    memset(vregion_code,0,sizeof(vregion_code));
    memset(vst,0,sizeof(vst));

    memcpy(vphone_no,phone_no,sizeof(vphone_no)-1);


    /*查询dCustMsg表*/
    Sinitn(vid_no);
    Sinitn(vcust_id);
    Sinitn(vuser_passwd);
    Sinitn(vrun_code);
    Sinitn(vBelongCode);
    Sinitn(vPhTmp);
    EXEC SQL SELECT to_char(id_no),to_char(cust_id),sm_code,
            user_passwd,substr(run_code,2,1),
            substr(attr_code,3,2),substr(attr_code,5,1),
            belong_code,substr(belong_code,1,2),phone_no
    INTO :vid_no,:vcust_id,:vsm_code,
            :vuser_passwd,:vrun_code,
            :vcard_type, :vgrpbig_flag,:vBelongCode,:vregion_code,:vPhTmp
    FROM dCustMsg WHERE id_no=to_number(:vphone_no);
    if(SQLCODE==NOTFOUND)
    {
        vDeadFlag=1;
        exec sql select to_char(id_no),to_char(cust_id),sm_code,
                user_passwd,substr(run_code,2,1),
                substr(attr_code,3,2),substr(attr_code,5,1),
                belong_code,substr(belong_code,1,2),phone_no
        INTO :vid_no,:vcust_id,:vsm_code,
                :vuser_passwd,:vrun_code,
                :vcard_type, :vgrpbig_flag,:vBelongCode,:vregion_code,:vPhTmp
        from dCustMsgDead where id_no=to_number(:vphone_no);
    }
    else
    {
        vDeadFlag=0;
    }
    if(SQLCODE!=0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1207","");
		return 1207;
	}
    strcpy(vRealPhone,vPhTmp);
    /*查询dCustDoc表*/
    EXEC SQL SELECT
        cust_name,owner_grade,owner_type,
        cust_address,id_type,id_iccid
    INTO
        :vcust_name,:vowner_grade,:vowner_type,
        :vcust_address,:vid_type,:vid_iccid
    FROM dCustDoc
    WHERE cust_id=to_number(:vcust_id);
    if(SQLCODE!=0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1208","");
		return 1208;
	}
    /*查询sSmCode表*/
    EXEC SQL SELECT sm_name INTO :vsm_name
    FROM sSmCode WHERE region_code=:vregion_code AND sm_code=:vsm_code;
    if(SQLCODE!=0)
	{
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1209","");
		return 1209;
	}
    /*查询sRunCode表*/
    EXEC SQL SELECT run_name INTO :vrun_name
    FROM sRunCode WHERE region_code=:vregion_code AND run_code=:vrun_code;

    /*查询sCustGradeCode表*/
    EXEC SQL SELECT type_name INTO :vgrade_name
    FROM sCustGradeCode WHERE region_code=:vregion_code and owner_code=:vowner_grade;
    if(SQLCODE!=0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1210","");
		return 1210;
	}
    /*查询sCustTypeCode表*/
    EXEC SQL SELECT type_name INTO :vtype_name
    FROM sCustTypeCode WHERE type_code=:vowner_type;
    if(SQLCODE!=0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1211","");
		return 1211;
	}

    /*查询sIdTtype表*/
    EXEC SQL SELECT id_name INTO :vid_name
    FROM sIdType WHERE id_type=:vid_type;
    if(SQLCODE!=0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1212","");
		return 1212;
	}

    /* 查询 sBigCardCode */
    EXEC SQL select card_name into :vcard_name
    from  sBigCardCode where card_type = :vcard_type;
    if(SQLCODE != 0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1213","-vcard_type = [%s] ",vcard_type);
        pubLog_DBErr(_FFL_, "","1213","++ Find sBigCardCode error [%d] ",SQLCODE);
        return 1213;
    }

    /* 查询 sGrpBigFlag */
    EXEC SQL select grp_name into :vgrpbig_name
    from  sGrpBigFlag where grp_flag = :vgrpbig_flag;
    if(SQLCODE != 0)
    {
    	PUBLOG_DBDEBUG("fGetIdBaseInfo");
		pubLog_DBErr(_FFL_, "","1214","-vgrp_flag = [%s] ",vgrpbig_flag);
		pubLog_DBErr(_FFL_, "","1214","++ Find sGrpBigFlag error [%d] ",SQLCODE);
        return 1214;
    }
    if( vDeadFlag==0 )
    {
       vret=fGetUserOwe(vid_no,&user_base->user_owe);
    }
    else
    {
        vret=fGetIdDeadOwe(vid_no,&user_base->user_owe);
    }

    /*赋值*/
    Trim(vid_no); strcpy(user_base->id_no,vid_no);
    Trim(vcust_id); strcpy(user_base->cust_id,vcust_id);
    Trim(vBelongCode); strcpy(user_base->belong_code,vBelongCode);
    Trim(vsm_code); strcpy(user_base->sm_code,vsm_code);
    Trim(vsm_name); strcpy(user_base->sm_name,vsm_name);
    Trim(vuser_passwd); strcpy(user_base->cust_name,vcust_name);
    Trim(vuser_passwd); strcpy(user_base->user_passwd,vuser_passwd);
    Trim(vrun_code); strcpy(user_base->run_code,vrun_code);
    Trim(vrun_name); strcpy(user_base->run_name,vrun_name);
    Trim(vowner_grade); strcpy(user_base->owner_grade,vowner_grade);
    Trim(vgrade_name); strcpy(user_base->grade_name,vgrade_name);

    Trim(vowner_type); strcpy(user_base->owner_type,vowner_type);
    Trim(vtype_name); strcpy(user_base->type_name,vtype_name);
    /*
    strcpy(user_base->owner_type,vgrpbig_flag);
    strcpy(user_base->type_name,vgrpbig_name);
    */

    Trim(vcust_address); strcpy(user_base->cust_address,vcust_address);
    Trim(vid_type); strcpy(user_base->id_type,vid_type);
    Trim(vid_name); strcpy(user_base->id_name,vid_name);
    Trim(vid_iccid); strcpy(user_base->id_iccid,vid_iccid);

    Trim(vcard_type); strcpy(user_base->card_type,vcard_type);
    Trim(vcard_name); strcpy(user_base->card_name,vcard_name);
    Trim(vgrpbig_flag); strcpy(user_base->grpbig_flag,vgrpbig_flag);
    Trim(vgrpbig_name); strcpy(user_base->grpbig_name,vgrpbig_name);

    return 0;
}
/**********************************************
 @wt  PRG  : pubBillBeginTimeMode
 @wt  FUNC : 取消iPhoneNo的iNewMode资费
 @wt       : 其中资费总生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt       : 操作时间iOpTime
 @wt       : 资费总的生效方式iSendFlag
 @wt       : 资费中所有的个性批价信息组合串iFlagStr
 @wt       : iFlagStr的格式为
 @wt       : aaaa^A1^A2^:bbbb^B1^B2^:cccc^C1^C2^C3^:
 @wt       : aaaa,bbbb,cccc为个性批价代码
 @wt       : A1、A2为aaaa的信息组合;B1、B2为bbbb的信息组合……
 @wt -----------------------------------------------------
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 11 iEndTime    结束时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iFlagStr     计费的附加信息串
 @wt 10 vBunchNo    手机串号资源（产品改造新增参数）
 @wt 9 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/
int pubBillBeginTimeMode(
    char *iPhoneNo,
    char *iNewMode,
    char *iEffectTime,
    char *iEndTime,
    char *iOpCode,
    char *iLoginNo,
    char *iLoginAccept,
    char *iOpTime,
    char *iSendFlag,
    char *iFlagStr,
    char *iBunchNo,
    char *oErrMsg)
{
    EXEC SQL BEGIN DECLARE SECTION;
      char    vIdNo[23];
      int     i=0,t=0,vTotalDate=0,vFavOrder=0,ret =0;
      char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
      int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
      char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
      char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
      char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
      int     vSendFlag,vUserFunc=-1;
      char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
      double  vMocRate=0,vMtcRate=0;
      char    vTimeCode[3];
      tUserBase   userBase;
      float       vPrepayFee=0;
      int		  retValue=0;
	  int		vCount=0;
      tGrantData vGrantMsg;
      int     iProductRetCode = 0;
      char vPromName[20+1];
	  char vDealFunc[2+1];
	  char vSrvCode[4+1];
	  char vSrvNetType[1+1];
	  char vCmdCode[256];
	  char vOrgId[10+1];
	  char vGroupId[10+1];
	  TPubBillInfo tPubBillInfo;

	  char vRegionCode[2+1];
	  int vMaxNum, vCurNum, vBuyNum;
	  char vNewType[1+1];

	  /*ng解耦*/
	  char 			vEffectTime[17+1];
	  char    		dynStmt[1024];
	  char 			vMocRateStr[8+1];
	  char 			vMtcRateStr[8+1];
	  char 			vFavOrderStr[4+1];
	  char			vTotalDateStr[8+1];
	  int 			iNum = 0;
	  int 			iNum2 = 0;
	  char 			v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	  TdBaseFav 			vTdBaseFav;
	  TdBaseVpmn 			vTdBaseVpmn;
	  TdBillCustDetail 		vTdBillCustDetail;
	  TdCustExpire 			vTdCustExpire;
	  TdCustFunc			vTdCustFunc;
	  TdCustFuncIndex		vTdCustFuncIndex;
	  TdCustFuncHis			vTdCustFuncHis;
	  
		/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.03.19 end*/
      EXEC SQL END   DECLARE SECTION;

      Trim(iPhoneNo);
      Trim(iNewMode);
      Trim(iEffectTime);
      Trim(iEndTime);
      Trim(iOpCode);
      Trim(iLoginNo);
      Trim(iLoginAccept);
      Trim(iOpTime);
      /*Trim(iSendFlag);
      Trim(iFlagStr);*/

      oErrMsg[0] = '\0';
    /*init(oErrMsg);*/
#ifdef _WTPRN_

    pubLog_Debug(_FFL_,"","","--pubBillBeginTimeMode  begin ----- ");
   	pubLog_Debug(_FFL_,"","","iPhoneNo=[%s]",iPhoneNo);
   	pubLog_Debug(_FFL_,"","","iNewMode=[%s] ",iNewMode);
    pubLog_Debug(_FFL_,"","","iEffectTime=[%s]",iEffectTime);
    pubLog_Debug(_FFL_,"","","iOpCode=[%s] ",iOpCode);
    pubLog_Debug(_FFL_,"","","iLoginNo=[%s]",iLoginNo);
    pubLog_Debug(_FFL_,"","","iLoginAccept=[%s] ",iLoginAccept);
    pubLog_Debug(_FFL_,"","","iOpTime=[%s]",iOpTime);
    pubLog_Debug(_FFL_,"","","iSendFlag=[%s] ",iSendFlag);
    pubLog_Debug(_FFL_,"","","iFlagStr=[%s]",iFlagStr);
#endif
    Sinitn(vIdNo);
    Sinitn(vOrgId);
    Sinitn(vGroupId);
    Sinitn(vRegionCode);
    Sinitn(vEffectTime);
    Sinitn(v_parameter_array);
    Sinitn(vFavOrderStr);
    Sinitn(vTotalDateStr);

    memset(dynStmt, 0, sizeof(dynStmt));
    memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));

	memset(&vMocRateStr, 0, sizeof(vMocRateStr));
	memset(&vMtcRateStr, 0, sizeof(vMtcRateStr));
	/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.04.17 end*/

    Trim(iOpTime);


    /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret = sGetUserGroupid(iPhoneNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败! ");
         	  return 200919;
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    exec sql select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
    into :vIdNo,:vBelongCode,:vTotalDate
    from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
    if(SQLCODE!=OK){
    	strcpy(oErrMsg,"查询dCustMsg错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
        return 1024;
    }

  Trim(vGroupId);
	strncpy(vRegionCode, vBelongCode, 2);

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}

	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127075"," [%s] ", oErrMsg);

		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}
	/* 检查资费关系结束 chenxuan boss3 */

/*
	EXEC SQL declare CurSndC011 cursor for
		select prom_name,deal_func
			from ssndcailcfg
			where region_code=substr(:vBelongCode,1,2)
    		and mode_code =:iNewMode;
	EXEC SQL open CurSndC011;
	for(i=0;;)
	{
		init(vPromName);
		init(vDealFunc);
		EXEC SQL fetch CurSndC011 into :vPromName,:vDealFunc;
		if(SQLCODE!=0)
		{
			printf(" .pubBillBeginTimeMode :select ssndcailcfg error [%d][%s] ", oErrMsg);
			break;
		}
		EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
										LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
										DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
					values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
							to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
							:vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
							to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_number(:iLoginAccept));
		if(SQLCODE!=0)
		{
				printf(" .pubBillBeginTimeMode :insert wsndcailmsg error [%d][%s] ", oErrMsg);
        		strcpy(oErrMsg,"插入wsndcailmsg表错误!");
        		EXEC SQL close CurSndC011;
        		return 1024;
		}
	}
	EXEC SQL close CurSndC011;
*/
    Trim(vIdNo);
    Sinitn(vHlrCode);
    Sinitn(vImsiNo);Sinitn(vSimNo);
    /*
    if(iOpCode[0]!='8'){*** 专线用户 ***
    */
    if(strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0 && strncmp(iNewMode,"id",2)!=0){/*** 专线用户 ***/
        exec sql select hlr_code into :vHlrCode from sHlrCode
        where phoneno_head=substr(:iPhoneNo,1,7);
        if(SQLCODE!=OK){
        	 strcpy(oErrMsg,"查询sHrlCode错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
            return 127076;
        }

        exec sql select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
        where id_no=to_number(:vIdNo);
        if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询dCustSim错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
            return 127077;
        }
    }

     /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetLoginOrgid(iLoginNo,vOrgId);
       if(ret <0)
         	{
				pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败! ");
         	 	return 200919;
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    Sinitn(vOrgCode);
    exec sql select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127078","[%s] ", oErrMsg);
        return 127078;
    }
    Sinitn(vTimeCode);
    Trim(vOrgId);

    exec sql select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
    where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
    if(SQLCODE==OK&&SQLROWS==1)
    {
            if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
            {
                Sinitn((void*)(&userBase));
                /*调用公用函数*/
                init(oErrMsg);
                sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));
                Trim(oErrMsg);
                if( strcmp(oErrMsg, "000000") != 0 )
                {
                    strcpy(oErrMsg,"查询用户信息错误!");
                    pubLog_DBErr(_FFL_, "","127078","%s",oErrMsg);
                    return 127078;
                }
                vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
                pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
                vDeltaDays=0;
                exec sql select days into :vDeltaDays from sPayTime
                where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='1'
                    and begin_money<=:vPrepayFee and end_money>=:vPrepayFee;
                if(SQLCODE!=OK)
                {
                    strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
       				pubLog_DBErr(_FFL_, "","127078","%s",oErrMsg);
                    return 127078;
                }
            }
		/*ng解耦 by wxcrm at 20090812 begin
        exec sql insert into dCustExpire(ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME)
        values(to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'N',to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
            to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays);
        if(SQLCODE!=0)
        {
        	strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
            return 127084;
        }
        ng解耦 by wxcrm at 20090812 end*/
        EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	"N"				,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
            return 127084;
		}


        exec sql insert into dCustExpireHis(
            ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
            UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
        values(
            to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'N',to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
            to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
            to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
        if(SQLCODE!=0)
        {
            strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
            return 127085;
        }
        /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
        exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
        values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
            to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
        if(SQLCODE!=0)
        {
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
            return 127086;
        }
    }

    Sinitn(vModeFlag);Sinitn(vSmCode);init(vFavBrand);
    exec sql select a.mode_flag,a.sm_code,b.fav_brand
    into :vModeFlag,:vSmCode,:vFavBrand from sBillModeCode a,sSmCode b
    where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
        and b.region_code=a.region_code and b.sm_code=a.sm_code;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127060"," [%s] ", oErrMsg);
        return 127060;
    }

    Sinitn(vSelSql);
    strcpy(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
        " from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2");
#ifdef _WTPRN_
    pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vSelSql);
#endif
    exec sql prepare preBeginDet1 from :vSelSql;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127079","[%s]", oErrMsg);

        return 127079;
    }
    exec sql declare curBeginDet12 cursor for preBeginDet1;
    exec sql open curBeginDet12 using :vBelongCode,:iNewMode;
    for(i=0;;){
        Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
        exec sql fetch curBeginDet12
        into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
        if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
            exec sql close curBeginDet12;
            return 127061;
        }
        if(i!=0&&SQLCODE==NOTFOUND) break;

        /*****************************************************************************
         ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
         ** sBillModeDetail.time_unit 0天/1月/2年                                   **
         ****************************************************************************/
        Sinitn(vBeginTime);
        Sinitn(vEndTime);
        switch(vTimeFlag[0]){
            case '0':
                strcpy(vBeginTime,iEffectTime);
                /*strcpy(vEndTime,MAXENDTIME);*/
                break;
            case '1':
                strcpy(vBeginTime,iEffectTime);
                /*pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime);*/
                break;
            case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
                /*strcpy(vEndTime,MAXENDTIME);*/
                break;
            default:;
        }
        /*strcpy(vBeginTime,iEffectTime);*/
        strcpy(vEndTime,iEndTime);
        Trim(vBeginTime);
        Trim(vEndTime);
#ifdef _WTPRN_
       pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode [%s~%s][%s:%s-%s]*%s*"
            ,vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);
#endif


		if (strcmp(vModeTime,"Y") == 0){
			/*printf("******vBeginTime=[%s]vEndTime=[%s]********** ",vBeginTime,vEndTime);*/
			retValue=DcustModeChgAwake(iOpCode,iNewMode,vBelongCode,"a",atol(iLoginAccept),iPhoneNo,iLoginNo,vBeginTime,vEndTime);
			if(retValue<0){
				EXEC SQL CLOSE curBeginDet12;
				strcpy(oErrMsg,"发送提醒短信错误!");
				PUBLOG_DBDEBUG("pubBillBeginMode");
				pubLog_DBErr(_FFL_, "","127099","[%s] ", oErrMsg);
				return 127099;
			}
		}

        Sinitn(vBeginSql);
        sprintf(vBeginSql,
            "insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
            " fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
            " login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
            " values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
            " :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
            " :v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
            vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_

        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode%s",vBeginSql);
#endif
        exec sql prepare preWWW_5 from :vBeginSql;
        exec sql execute preWWW_5 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
            :vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
            :iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
        if(SQLCODE!=OK){
            sprintf(oErrMsg,"记录dBillCustDetail的历史错误![%d]",SQLCODE);
            exec sql close curBeginDet12;
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
            return 127062;
        }

        Sinitn(vBeginSql);
        #ifdef _WTPRN_
        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);
        /*ng解耦 by wxcrm at 20090812 begin
        sprintf(vBeginSql,
            "insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,"
            " mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code) "
            " values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),:v6,:v7,"
            " :v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),:v13,to_number(:v14),substr(:v15,1,2))",
            vIdNo[strlen(vIdNo)-1]);
		#ifdef _WTPRN_
        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);

        exec sql prepare preWWW_6 from :vBeginSql;
        exec sql execute preWWW_6 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,:vFavOrder,:iNewMode,
            :vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
        if(SQLCODE!=OK){
            exec sql close curBeginDet1;
            strcpy(oErrMsg,"记录dBillCustDetail错误!");
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127063",oErrMsg);
            return 127063;
        }
		ng解耦 by wxcrm at 20090812 end*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet12;
            strcpy(oErrMsg,"记录dBillCustDetail错误!");
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127063",oErrMsg);
            return 127063;
		}

		 switch(vDetailType[0]){
            /* lixg Add: 2005-11-20 begin */
            case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
            case PROD_MACHFEE_CODE: /* 买手机送话费产品 */

                memset(&vGrantMsg, 0, sizeof(vGrantMsg));
                strcpy(vGrantMsg.applyType, vDetailType);
                strcpy(vGrantMsg.phoneNo, iPhoneNo);
                strcpy(vGrantMsg.loginNo, iLoginNo);
                strcpy(vGrantMsg.opCode, iOpCode);
                strcpy(vGrantMsg.opNote, " ");
                strcpy(vGrantMsg.opTime, iOpTime);
                sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
                strcpy(vGrantMsg.modeCode, iNewMode);
                strcpy(vGrantMsg.detModeCode, vDetailCode);
                strcpy(vGrantMsg.machineId, iBunchNo);
                strcpy(vGrantMsg.loginAccept, iLoginAccept);
                pubLog_Debug(_FFL_,"","","product Begin ..... ");
                if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 )
                {
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
                    return iProductRetCode;
                }
                break;
            /* lixg Add: 2005-11-20 end */

            case '0': /** process bill system data              **/
                init(vVpmnGroup);
                exec sql select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate from sBillVpmnCond
                where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
                if(SQLROWS==1)
                {
                    Trim(vVpmnGroup);
                    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                    /*ng解耦 by wxcrm at 20090812 begin
                    exec sql insert into dBaseFav(
                        msisdn,fav_brand,fav_type,flag_code,fav_order,
                        fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
                    values(
                        :iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
                        to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),substr(:vBelongCode,1,2),:vIdNo, :vGroupId ,:iNewMode);
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
                    }
                    ng解耦 by wxcrm at 20090812 end*/

					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
					}


                    #ifdef _CHGTABLE_
                    exec sql insert into wBaseFavChg(
                        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                        start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
                    values(
                        :iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
                        to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
                        '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId ,:iNewMode);
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127083", oErrMsg);
                        return 127083;
                    }
                    #endif
					/*ng解耦 by wxcrm at 20090812 begin
                    exec sql insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
                    values(:iPhoneNo,:vVpmnGroup,:vMocRate,:vMtcRate,
                        to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
                    }
                    ng解耦 by wxcrm at 20090812 end*/
                    sprintf(vMocRateStr,"%f",vMocRate);
					sprintf(vMtcRateStr,"%f",vMtcRate);
                    memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
                    strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
					}
                    #ifdef _CHGTABLE_
                    exec sql insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
                    values(:iPhoneNo,:vVpmnGroup,:vMocRate,:vMtcRate,
                        to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
                        '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
                   		 PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127083", oErrMsg);
                        return 127083;
                    }
                    #endif
                }

                if(iFlagStr[0]=='\0'){
                    Sinitn(vFather);
                    sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
                }
                else
                    strcpy(vFather,iFlagStr);

                for(;;){
                    pubApartStr(vFather,':',vUncle,vSon);
                    if(vSon[0]==vDetailCode[0]&&
                       vSon[1]==vDetailCode[1]&&
                       vSon[2]==vDetailCode[2]&&
                       vSon[3]==vDetailCode[3]){
                        pubApartStr(vSon,'^',vFather,vUncle);
                        for(;;){
                            pubApartStr(vFather,'^',vUncle,vSon);

                            if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");
                         /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                            /*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dBaseFav(
                                msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                                start_time,end_time,region_code,id_no,group_id,product_code)
                            select
                                :iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
                                :vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
                                substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
                            from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
                            if(SQLCODE!=OK||SQLROWS!=1){
                                exec sql close curBeginDet1;
                                strcpy(oErrMsg,"记录dBaseFav错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);

                                return 127064;
                            }
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);

							/*
							EXEC SQL SELECT
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
                                :vFavDay,:vBeginTime,:vEndTime,
                                substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
                            	INTO :vTdBaseFav.sMsisdn,:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet1;
                                strcpy(oErrMsg,"初始化结构体错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                return 127064;
							}
							*/
							EXEC SQL SELECT count(*)
									INTO :iNum2
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum2!=1){
								strcpy(oErrMsg,"取sBillRateCode记录错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                exec sql close curBeginDet12;
                                return 127064;
							}
							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	vSon		);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDet12;
                                strcpy(oErrMsg,"记录dBaseFav错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                return 127064;
							}
							#ifdef _CHGTABLE_
                            exec sql insert into wBaseFavChg(
                                msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                                start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
                            select
                                :iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
                                :vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
                                to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
                                    to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
                            from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
                            if(SQLCODE!=OK||SQLROWS!=1){
                                exec sql close curBeginDet12;
                                strcpy(oErrMsg,"记录wBaseFavChg错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127065"," [%s] ", oErrMsg);

                                return 127065;
                            }
							#endif
                            strcpy(vFather,vUncle);
                            if(vFather[0]==0) break;
                        }
                        break;
                    }
                    strcpy(vFather,vUncle);
                    if(vFather[0]=='\0') break;
                }
            case '1':case '2': case '3':case '4':case 'a':
            	#ifdef _CHGTABLE_
                exec sql insert into wUserFavChg(
                    op_no,op_type,op_time,id_no,detail_type,detail_code,
                    begin_time,end_time,fav_order,mode_code)
                values(
                    sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
                    to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
                if(SQLCODE!=OK){
                    exec sql close curBeginDet12;
                    strcpy(oErrMsg,"记录wUseFavChg错误!");
                    PUBLOG_DBDEBUG("pubBillBeginTimeMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);

                    return 127066;
                }
                #endif

                if(vDetailType[0]!='a') break;
                /** 处理特服绑定的情况 ***/


                vSendFlag=strcmp(vBeginTime,iOpTime);
                Sinitn(vFunctionType);Sinitn(vFunctionCode);
                Sinitn(vOffOn);Sinitn(vCommandCode);
                exec sql select a.function_code,a.off_on,b.command_code,b.function_type
                    into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
                from sBillFuncBind a,sFuncList b
                where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
                    and b.region_code=a.region_code and b.sm_code=a.sm_code
                    and b.function_code=a.function_code;

				if(SQLCODE!=OK){
                	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
					pubLog_DBErr(_FFL_, "","127067"," .pubBillBeginTimeMode :select sFuncList error ");
                    return 127067;
                }

                if(vSendFlag>0)/***预约 开/关***/
                {
                    exec sql select count(*) into :vUserFunc from wCustExpire
                    where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
                    and business_status=:vOffOn;
                    if(vUserFunc==0)
                    {
                    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                        exec sql insert into wCustExpire(
                            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,org_id)
                        values(
                            sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
                            :vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
                            to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
                        if(SQLCODE!=OK){
							PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127068"," .pubBillBeginTimeMode :insert wCustExpire error");
                            return 127068;
                        }

                        exec sql insert into wCustExpireHis(
                            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,org_id)
                        select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
                        from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
                        if(SQLCODE!=OK||SQLROWS!=1){
                        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127069"," .pubBillBeginTimeMode :insert wCustExpireHis error");
                            return 127069;
                        }
                    }
                }
                else{
                    exec sql select count(*) into :vUserFunc from dCustFunc
                    where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

                    if(vOffOn[0]=='1'){ /** 立即开 ***/
                        if(vUserFunc==0){
                        	/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFuncHis(
                                id_no,function_type,function_code,op_time,
                                update_login,update_accept,update_date,update_time,update_code,update_type)
                            values(
                                to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
                                :iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127070"," .pubBillBeginTimeMode :insert dCustFuncHis(I) error");
                                return 127070;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            Sinitn(vTotalDateStr);
                            sprintf(vTotalDateStr,"%d",vTotalDate);
                            strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127070"," .pubBillBeginTimeMode :insert dCustFuncHis(I) error");
                                return 127070;
							}

							/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFunc(id_no,function_type,function_code,op_time)
                            values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127071"," .pubBillBeginTimeMode :insert dCustFunc error");
                                return 127071;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127071"," .pubBillBeginTimeMode :insert dCustFunc error");
                                return 127071;
							}
                        }
                    }
                    else{  /** 立即关 **/
                        if(vUserFunc!=0){
                        	/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFuncHis(
                                id_no,function_type,function_code,op_time,
                                update_login,update_accept,update_date,update_time,update_code,update_type)
                            select
                                id_no,function_type,function_code,op_time,
                                :iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
                            from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :insert dCustFuncHis(D) error ");
                                return 127072;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDate,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr6 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur6 CURSOR FOR ng_SqlStr6;
							EXEC SQL OPEN ng_Cur6 using :iLoginNo,:iLoginAccept,:vTotalDate,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur6 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :select dCustFunc error ");
                                	EXEC SQL Close ng_Cur6;
									return 127072;
								}
								Trim(vTdCustFuncHis.sIdNo);
								Trim(vTdCustFuncHis.sFunctionType);
								Trim(vTdCustFuncHis.sFunctionCode);
								Trim(vTdCustFuncHis.sOpTime);
								Trim(vTdCustFuncHis.sUpdateLogin);
								Trim(vTdCustFuncHis.sUpdateAccept);
								Trim(vTdCustFuncHis.sUpdateDate);
								Trim(vTdCustFuncHis.sUpdateTime);
								Trim(vTdCustFuncHis.sUpdateCode);
								Trim(vTdCustFuncHis.sUpdateType);
								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :insert dCustFuncHis(D) error ");
                                	EXEC SQL Close ng_Cur6;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur6;

							/*ng解耦 by wxcrm at 20090812 begin
                            exec sql delete dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
                                return 127073;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur10 CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur10 using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur10 INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127073","select dCustFunc error");
									EXEC SQL Close ng_Cur10;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
									EXEC SQL Close ng_Cur10;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur10;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
                                return 127073;
							}
                        }
                    }

                    exec sql insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
                        op_code,login_no,login_accept)
                    values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
                        to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
                    if(SQLCODE!=OK){
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127074"," .pubBillBeginTimeMode :insert wCustFuncDay error");
                        return 127074;
                    }

                    if(iOpCode[0]!='8')/*** 非专线用户在立即生效时发送开关机命令 ***/
                    {
                    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                    	/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
											if(strcmp(vFunctionCode,"02")==0)
											{
												EXEC SQL SELECT COUNT(1)
																	into : iCount
																	FROM dcustfunc a
																		WHERE a.id_no = :vIdNo
																			AND a.function_code = '87';
												if(SQLCODE != 0)
												{
													PUBLOG_DBDEBUG("");
													pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
													return 121993;
												}	
												if(iCount == 0)
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1");
													}
													else
													{
														strcpy(vNewCmdStr,"N");
													}
												}
												else
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1&BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"BOS3");
													}
												}
												EXEC SQL insert into wSndCmdDay(
													command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
													new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
													org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
													values(
													sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
													:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
													:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
												if (SQLCODE != OK){
													PUBLOG_DBDEBUG("pubBillBeginMode");
													pubLog_DBErr(_FFL_, "","127075","");
													return 127075;
												}
											}
											if(strcmp(vFunctionCode,"87")==0)
											{
												EXEC SQL SELECT COUNT(1)
																	into : iCount
																	FROM dcustfunc a
																		WHERE id_no = :vIdNo
																			AND function_code = '02';
												if(SQLCODE != 0)
												{
													PUBLOG_DBDEBUG("");
													pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
													return 121993;
												}	
												if(iCount == 0)
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"N");
													}
												}
												else
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1&BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"BOS1");
													}
												}
												EXEC SQL insert into wSndCmdDay(
													command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
													new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
													org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
													values(
													sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
													:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
													:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
												if (SQLCODE != OK){
													PUBLOG_DBDEBUG("pubBillBeginMode");
													pubLog_DBErr(_FFL_, "","127075","");
													return 127075;
												}
											}
											/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
											else
											{
                       exec sql insert into wSndCmdDay(
                            command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
                            new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
                            org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
                        values(
                            sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
                            :iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
                            :vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOrgId, :vGroupId);

                        if(SQLCODE!=OK){
                        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127075"," .pubBillBeginTimeMode :insert wSndSmdDay error");
                            return 127075;
                        }
                      }
                    }

            }
            default:;
        }
    i++;
    }
    exec sql close curBeginDet12;

		strncpy(vRegionCode, vBelongCode, 2);

/* 集团上线暂时注释下面代码 */
#if 0
		/* 开始处理服务代码 chenxuan boss3 */
    Sinitn(vSelSql);
    strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
    	"from sBillModeSrv a, sSrvCmdRelat b "
    	"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
    	"order by b.cmd_order");
    exec sql prepare pre1 from :vSelSql;
    exec sql declare cur1 cursor for pre1;
    exec sql open cur1 using :vBelongCode, :iNewMode;
    for (;;) {
    	init(vSrvCode); init(vSrvNetType); init(vCmdCode);
      exec sql fetch cur1 into :vSrvCode, :vSrvNetType, :vCmdCode;
      if (SQLCODE == 1403) break;
      Trim(vCmdCode);

      /* 插入用户使用服务表 */
      exec sql insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
      		mode_code, login_accept)
      	values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
      		to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
      if (SQLCODE != 0) {
      	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127070","insert dBillCustSrv error");
      	return 127070;
      }

			/* 插入接口相关表 */
      exec sql insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
      		cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
      	values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
      		:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
      if (SQLCODE != 0) {
      	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127071","insert wSndSrvDay error");
      	return 127071;
      }
    }
    exec sql close cur1;
		/* 处理服务代码结束 chenxuan boss3 */
#endif

		/* 调用指令分发函数 chenxuan boss3 */
		strcpy(tPubBillInfo.IdNo,        vIdNo);
		strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
		strcpy(tPubBillInfo.BelongCode,  vBelongCode);
		strcpy(tPubBillInfo.SmCode,      vSmCode);
		strcpy(tPubBillInfo.ModeCode,    iNewMode);
		strcpy(tPubBillInfo.OpCode,      iOpCode);
		strcpy(tPubBillInfo.OpTime,      iOpTime);
		strcpy(tPubBillInfo.EffectTime,  iEffectTime);
		tPubBillInfo.TotalDate = vTotalDate;
		strcpy(tPubBillInfo.LoginNo,     iLoginNo);
		strcpy(tPubBillInfo.LoginAccept, iLoginAccept);

		iProductRetCode = fDispatchCmd("1", &tPubBillInfo, oErrMsg);
		if (iProductRetCode != 0) {
			pubLog_DBErr(_FFL_, "","127071","");
			return 127072;
		}
		/* 指令分发函数结束 chenxuan boss3 */

		/* 开始产品订购数量限制 chenxuan boss3 */
		init(vBeginTime);
		init(vEndTime);
		init(flag);
    EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
    	INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
    	from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
    		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
    	EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127073"," [%s]", oErrMsg);
			return 127073;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127074"," .pubBillBeginTimeMode :select max_num error[%s]", oErrMsg);
			return 127074;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127075","[%s]", oErrMsg);
			return 127075;
	    }
    }
    /* 产品订购数量限制结束 chenxuan boss3 */

    /*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			  INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			  from sBillModeNum
			 where region_code = '00' and mode_code = :iNewMode
		       and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = '00' and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, " 插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127076","[%s]", oErrMsg);

			return 127076;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127077","[%s]", oErrMsg);
			return 127077;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]", oErrMsg);
			return 127078;
	    }
	}
	/*增加全省统一数量控制功能 20081119  end*/
	
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode end -----");
#endif
    return 0;
}

/********************************************************
 * PRG  : CancelAllMode
 * DATE : 2003.12.22
 * EDIT : wangtao
 * FUNC : 删除套餐记录,销户时使用
 * PARM :
 * 0 iIdNo         用户ID
 * 1 oCurBaseMode  当前主资费代码
 * 1 oCurAccept    当前主资费的开通流水
 * 2 oNextBaseMode 下月主资费代码
 * 2 oNextAccept   下月主资费的开通流水
 * 1 oErrMsg       函数内部错误信息
 *******************************************************/
int CancelAllMode(char *pIdNo,char *oErrMsg)
{
    FBFR32 *transIN = NULL, *transOUT = NULL;
    char * temp_buf;
    int errNo;

    EXEC SQL BEGIN DECLARE SECTION;
    char    dynStmt[1024];
    char    defaultContractNo[22+1];
    char    idNo[22+1];
    char    phone_no[15+1];
    int     total_date=0;
    char    op_code[4+1];
    char    login_no[6+1];
    char    org_code[9+1];
   long    login_accept=0;
    long    id_no=0;
    EXEC SQL END DECLARE SECTION;

    Sinitn(idNo);
    strcpy(idNo, pIdNo);

    EXEC SQL    INSERT INTO dCustFuncDead
                (
                        ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  OP_TIME

                )
                SELECT
                        ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  OP_TIME

                  FROM dCustFunc
                 WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"插入表dCustFuncDead错误!");
        PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121349","");
        return 121349;
    }

    EXEC SQL DELETE FROM dCustFunc WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"删除表dCustFunc错误!");
       	PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121350","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121350;
    }

    EXEC SQL    INSERT INTO dCustFuncHisDead
                (
                    ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  OP_TIME,    UPDATE_LOGIN,
                    UPDATE_ACCEPT,  UPDATE_DATE,    UPDATE_TIME,    UPDATE_CODE,    UPDATE_TYPE

                )
                SELECT
                    ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  OP_TIME,    UPDATE_LOGIN,
                    UPDATE_ACCEPT,  UPDATE_DATE,    UPDATE_TIME,    UPDATE_CODE,    UPDATE_TYPE
                  FROM dCustFuncHis
                 WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"插入表dCustFuncHisDead错误!");
        PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121371","[%s] SQLCODE=[%d]",oErrMsg,SQLCODE);
        return 121371;
    }

    EXEC SQL DELETE FROM dCustFuncHis WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"删除表dCustFuncHis错误!");
        PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121372","[%s] SQLCODE=[%d]",oErrMsg,SQLCODE);
        return 121372;
    }

    EXEC SQL    INSERT INTO dCustFuncAddDead
                (
                    ID_NO,  FUNCTION_CODE,  FUNCTION_TYPE,  BEGIN_TIME,
                    BILL_TIME,  FAVOUR_MONTH,   ADD_NO, OTHER_ATTR

                )
                SELECT
                    ID_NO,  FUNCTION_CODE,  FUNCTION_TYPE,  BEGIN_TIME,
                    BILL_TIME,  FAVOUR_MONTH,   ADD_NO, OTHER_ATTR
                  FROM dCustFuncAdd
                 WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE != 1403)
    {
        strcpy(oErrMsg,"插入表dCustFuncAddDead错误!");
        PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121373","[%s] SQLCODE=[%d]",oErrMsg,SQLCODE);
        return 121373;
    }

    EXEC SQL DELETE FROM dCustFuncAdd WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"删除表dCustFuncAdd错误!");
        PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121374","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121374;
    }

    EXEC SQL    INSERT INTO dCustFuncAddHisDead
                (
                    ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  BEGIN_TIME, BILL_TIME,
                    FAVOUR_MONTH,   ADD_NO, OTHER_ATTR, UPDATE_LOGIN,   UPDATE_ACCEPT,
                    UPDATE_DATE,    UPDATE_TIME,    UPDATE_CODE,    UPDATE_TYPE
                )
                SELECT
                    ID_NO,  FUNCTION_TYPE,  FUNCTION_CODE,  BEGIN_TIME, BILL_TIME,
                    FAVOUR_MONTH,   ADD_NO, OTHER_ATTR, UPDATE_LOGIN,   UPDATE_ACCEPT,
                    UPDATE_DATE,    UPDATE_TIME,    UPDATE_CODE,    UPDATE_TYPE
                  FROM dCustFuncAddHis
                 WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE != 1403)
    {
        strcpy(oErrMsg,"插入表dCustFuncAddHisDead错误!");
       	PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121375","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121375;
    }

    EXEC SQL DELETE FROM dCustFuncAddHis WHERE id_no=to_number(:idNo);
    if (SQLCODE != 0 && SQLCODE !=1403)
    {
        strcpy(oErrMsg,"删除表dCustFuncAddHis错误!");
       	PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121376","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121376;
    }

    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt,    "INSERT INTO dBillCustDetailDead"
                        "("
                        "   ID_NO,  DETAIL_TYPE,    DETAIL_CODE,    BEGIN_TIME,"
                        "   END_TIME,   FAV_ORDER,  MODE_CODE,  MODE_FLAG,"
                        "   MODE_TIME,  MODE_STATUS,    LOGIN_ACCEPT,   OP_CODE,"
                        "   TOTAL_DATE, OP_TIME,    LOGIN_NO,   REGION_CODE"
                        ")"
                        "SELECT"
                        "   ID_NO,  DETAIL_TYPE,    DETAIL_CODE,    BEGIN_TIME,"
                        "   END_TIME,   FAV_ORDER,  MODE_CODE,  MODE_FLAG,"
                        "   MODE_TIME,  MODE_STATUS,    LOGIN_ACCEPT,   OP_CODE,"
                        "   TOTAL_DATE, OP_TIME,    LOGIN_NO,   REGION_CODE"
                        "  FROM dBillCustDetail%c"
                        " WHERE id_no = to_number(:v1)",
                        idNo[strlen(idNo)-1]);

    EXEC SQL PREPARE prepare6 From :dynStmt;
    EXEC SQL EXECUTE prepare6 USING :idNo;
    if (SQLCODE != 0)
    {
        sprintf(oErrMsg,"插入表dCustFuncAddHisDead错误[%d]!",SQLCODE);
         PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121377","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121377;
    }

    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt,    "DELETE FROM dBillCustDetail%c"
                        " WHERE id_no = to_number(:v1)",
                        idNo[strlen(idNo)-1]);
    EXEC SQL PREPARE prepare7 From :dynStmt;
    EXEC SQL EXECUTE prepare7 USING :idNo;
    if (SQLCODE != 0)
    {
        sprintf(oErrMsg,"删除表dBillCustDetail%c错误[%d]!", idNo[strlen(idNo)-1],SQLCODE);
         PUBLOG_DBDEBUG("CancelAllMode");
		pubLog_DBErr(_FFL_, "","121378","[%s] SQLCODE=[%d] ",oErrMsg,SQLCODE);
        return 121378;
    }

    return 0;
}


/***
套餐变更短信提醒
***/
int DcustModeChgAwake(char *op_code,char *mode_code,char *belong_code,char *use_flag,long login_accept,char *phone_no,char *login_no,char *begin_time,char *end_time)
{
	EXEC SQL BEGIN DECLARE SECTION;

	long	loginAccept=0;
	char	opCode[4+1];
	char	modeCode[8+1];
	char	belongCode[7+1];
	char	useFlag[1+1];
	char	phoneNo[15+1];
	char	loginNo[6+1];
	char	regionCode[2+1];
	char	sendMsg[512];
	char	sendMsgOld[512];
	char	tempBuf[128];
	char	opMonth[2+1];
	char	opDate[2+1];
	char	opHour[2+1];
	char	opMi[2+1];
	char	beginTime[17+1];
	char	endTime[17+1];
	char	modeName[30+1];
	char	modeNote[255+1];
	int		flag=0;
	int		oldLoginAccept=0;
	int		count=0,countNow=0;
	char	vIdNo[14+1];
	/**统一短信**/
	int		smModelId=0;
	int		ret=0;
	char	vOpTime[19+1];
	EXEC SQL END DECLARE SECTION;
	init(vIdNo);
	init(opCode);
	init(modeCode);
	init(belongCode);
	init(useFlag);
	init(phoneNo);
	init(loginNo);
	init(regionCode);
	init(sendMsg);
	init(sendMsgOld);
	init(tempBuf);
	init(opMonth);
	init(opDate);
	init(opHour);
	init(opMi);
	init(beginTime);
	init(endTime);
	init(modeNote);
	init(vOpTime);

	strcpy(opCode,op_code);
	strcpy(modeCode,mode_code);
	strncpy(regionCode,belong_code,2);
	strcpy(useFlag,use_flag);
	strcpy(phoneNo,phone_no);
	strcpy(loginNo,login_no);
	strcpy(beginTime,begin_time);
	strcpy(endTime,end_time);
	loginAccept=login_accept;
	/*printf("******beginTime=[%s]endTime=[%s]********** ",beginTime,endTime);*/
	if(strcmp(opCode,"1270")!=0 && strcmp(opCode,"1272")!=0 && strcmp(opCode,"1266")!=0 && strcmp(opCode,"1268")!=0 && strcmp(opCode,"3505")!=0 && strcmp(opCode,"3506")!=0&& strcmp(opCode,"2976")!=0&& strcmp(opCode,"7811")!=0)
	{
		return 0;
	}

	/*EXEC SQL SELECT mode_name,awake_msg,to_char(sysdate,'MM'),to_char(sysdate,'DD'),to_char(sysdate,'HH24'),to_char(sysdate,'MI')
					INTO :modeName,:modeNote,:opMonth,:opDate,:opHour,:opMi FROM sawakemodecode
					WHERE awakemode_code=:modeCode AND region_code=:regionCode AND use_flag=:useFlag AND useing='Y';
	if(SQLCODE==1403)
	{
		return 0;
	}
	else if(SQLCODE!=0 && SQLCODE!=1403)
	{
		PUBLOG_DBDEBUG("DcustModeChgAwake");
		pubLog_DBErr(_FFL_, "","-1","select sawakemodecode error[%d] ",SQLCODE);
		return -1;
	}*/

	EXEC SQL SELECT note,mode_name,to_char(sysdate,'MM'),to_char(sysdate,'DD'),to_char(sysdate,'HH24'),to_char(sysdate,'MI')
			INTO :modeNote,:modeName,:opMonth,:opDate,:opHour,:opMi
			FROM SBILLMODECODE
			WHERE  region_code=:regionCode and mode_code=:modeCode;
	if(SQLCODE==1403)
	{
		return 0;
	}
	else if(SQLCODE!=0 && SQLCODE!=1403)
	{
		printf("select SBILLMODECODE error[%d] ",SQLCODE);
		return -4;
	}
	Trim(modeNote);

	sprintf(sendMsgOld,"您于%s月%s日%s时%s分在",opMonth,opDate,opHour,opMi);
	sprintf(sendMsg,"尊敬的客户，您好！");

	if(strcmp(loginNo,"www000")==0)
	{
		strcpy(tempBuf,"网上营业厅");
	}
	else if(strcmp(loginNo,"dx0000")==0)
	{
		strcpy(tempBuf,"短信营业厅");
	}
	else if(strcmp(loginNo,"wap000")==0)
	{
		strcpy(tempBuf,"WAP掌上营业厅");
	}
	else
	{
		/*---组织机构改造替换字段 edit by magang at 08/05/2009---
		EXEC SQL SELECT a.town_name
				into :tempBuf
				from stowncode a,dloginmsg b
				where b.login_no=:loginNo
				and a.region_code=substr(b.org_code,1,2)
				and district_code=substr(b.org_code,3,2)
				and town_code=substr(b.org_code,5,3);
				---组织机构改造替换字段 edit by magang at 08/05/2009--- */
				EXEC SQL SELECT a.group_name
				into :tempBuf
				FROM dChnGroupmsg a,dLoginMsg b
				WHERE b.login_no=:loginNo
				and a.group_id=b.group_id ;

		if(SQLCODE!=0)
		{
			PUBLOG_DBDEBUG("DcustModeChgAwake");
			pubLog_DBErr(_FFL_, "","-2","select stowncode error[%d] ",SQLCODE);
			return -2;
		}
	}
	Trim(tempBuf);
	Trim(modeName);
	Trim(beginTime);
	Trim(endTime);

	if(strcmp(useFlag,"a")==0)
	{
		/*sprintf(sendMsg,"%s%s办理了%s套餐,该套餐生效时间%s,失效时间%s,具体资费描述:%s.",sendMsg,tempBuf,modeName,beginTime,endTime,modeNote);*/
		/*sprintf(sendMsg,"%s%s办理了%s套餐,该套餐生效时间%s,具体资费描述:%s.",sendMsg,tempBuf,modeName,beginTime,modeNote);*/
		sprintf(sendMsg,"%s您已成功订购%s套餐,该套餐%s生效,%s,如有疑问，请咨询10086。中国移动",sendMsg,modeName,beginTime,modeNote);
		smModelId = 2713;
		PrepareNewShortMsg(smModelId);
		SetShortMsgParameter(smModelId, "MODE_NAME", modeName);
		SetShortMsgParameter(smModelId, "BEGIN_TIME", beginTime);
		SetShortMsgParameter(smModelId, "MODE_NOTE", modeNote);
	}
	else if(strcmp(useFlag,"d")==0)
	{
		flag=strcmp(beginTime,endTime);
		if(flag<0)
		{
			sprintf(sendMsg,"%s%s取消了%s套餐,该套餐失效时间%s.",sendMsgOld,tempBuf,modeName,beginTime);
			smModelId = 2723;
			PrepareNewShortMsg(smModelId);
			SetShortMsgParameter(smModelId, "MONTH", opMonth);
			SetShortMsgParameter(smModelId, "DAY", opDate);
			SetShortMsgParameter(smModelId, "HOUR", opHour);
			SetShortMsgParameter(smModelId, "MIN", opMi);
			SetShortMsgParameter(smModelId, "BUSINESS_SITE", tempBuf);
			SetShortMsgParameter(smModelId, "MODE_NAME", modeName);
			SetShortMsgParameter(smModelId, "END_TIME", beginTime);
		}
		else
		{
			sprintf(sendMsg,"%s%s取消了%s套餐,该套餐失效时间%s.",sendMsgOld,tempBuf,modeName,endTime);
			smModelId = 2723;
			PrepareNewShortMsg(smModelId);
			SetShortMsgParameter(smModelId, "MONTH", opMonth);
			SetShortMsgParameter(smModelId, "DAY", opDate);
			SetShortMsgParameter(smModelId, "HOUR", opHour);
			SetShortMsgParameter(smModelId, "MIN", opMi);
			SetShortMsgParameter(smModelId, "BUSINESS_SITE", tempBuf);
			SetShortMsgParameter(smModelId, "MODE_NAME", modeName);
			SetShortMsgParameter(smModelId, "END_TIME", endTime);
		}
		/* R_JLMob_liuyinga_CRM_CMI_2011_0551 关于完善手机上网功能变更操作和优化上网套餐变更提醒短信的需求 20110909 begin*/
		EXEC SQL select id_No
			into :vIdNo
			FROM dcustmsg 
			where phone_no =:phoneNo;
		if(SQLCODE!=0 && SQLCODE!=1403)
		{
			PUBLOG_DBDEBUG("DcustModeChgAwake");
			pubLog_DBErr(_FFL_, "","-3","查询用户信息错误[%d] ",SQLCODE);
			return -3;
		}
		Trim(vIdNo);
		if(strcmp(vIdNo,"")!= 0)
		{
			printf("检查订购绑定特服关系\n");
			count=0;
			EXEC SQL SELECT count(*) 
				into :count
				FROM dbillcustdetail a, sbillfuncbind b  
				WHERE a.region_code = b.region_code
				AND a.detail_code = b.function_bind
				AND b.function_code = '55'
				AND a.mode_code = :modeCode
				AND a.end_time > SYSDATE
				AND a.id_no=:vIdNo;
			if(SQLCODE!=0 && SQLCODE!=1403)
			{
				PUBLOG_DBDEBUG("DcustModeChgAwake");
				pubLog_DBErr(_FFL_, "","-3","查询套餐绑定订购关系错误[%d] ",SQLCODE);
				return -3;
			}
			if(count != 0)
			{
				countNow =0;
				// 1	立即开通情况  特服表
				EXEC SQL  SELECT count(*)
					into :countNow
					FROM dcustfunc
					WHERE id_no =:vIdNo 
					AND function_code = '55';
				if(SQLCODE!=0)
				{
					PUBLOG_DBDEBUG("DcustModeChgAwake");
					pubLog_DBErr(_FFL_, "","-3","查询特服订购关系错误[%d] ",SQLCODE);
					return -3;
				}
				printf(" 1	立即开通情况  特服表 countNow [%d] \n",countNow);
				if(countNow != 0 )
				{
					if(flag<0)
					{
						smModelId = 2323;
						PrepareNewShortMsg(smModelId);
						SetShortMsgParameter(smModelId, "MONTH", opMonth);
						SetShortMsgParameter(smModelId, "DAY", opDate);
						SetShortMsgParameter(smModelId, "HOUR", opHour);
						SetShortMsgParameter(smModelId, "MIN", opMi);
						SetShortMsgParameter(smModelId, "BUSINESS_SITE", tempBuf);
						SetShortMsgParameter(smModelId, "MODE_NAME", modeName);
						SetShortMsgParameter(smModelId, "END_TIME", beginTime);
					}
					else
					{
						smModelId = 2323;
						PrepareNewShortMsg(smModelId);
						SetShortMsgParameter(smModelId, "MONTH", opMonth);
						SetShortMsgParameter(smModelId, "DAY", opDate);
						SetShortMsgParameter(smModelId, "HOUR", opHour);
						SetShortMsgParameter(smModelId, "MIN", opMi);
						SetShortMsgParameter(smModelId, "BUSINESS_SITE", tempBuf);
						SetShortMsgParameter(smModelId, "MODE_NAME", modeName);
						SetShortMsgParameter(smModelId, "END_TIME", endTime);
					}
					sprintf(sendMsg,"%s仍为您保留手机上网功能，详细资费信息询10086。",sendMsg);
				}
			}
		}
		/* R_JLMob_liuyinga_CRM_CMI_2011_0551 关于完善手机上网功能变更操作和优化上网套餐变更提醒短信的需求 20110909 end  */
	}
	printf("sendMsg : %s\n",sendMsg);
	Trim(sendMsg);
	/**统一短信改造 yaoxc 20111110
	**EXEC SQL insert into WCOMMONSHORTMSGTX(COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,
	**	   	PHONE_NO,MSG,ORDER_CODE,BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME,sent_time)
	**	   select SMSG_SEQUENCE.nextval,to_number(:loginAccept),1,:phoneNo,:sendMsg,
	**	   0,0,:opCode,:loginNo,sysdate,'' from dual;
	**if(SQLCODE!=0 && SQLCODE!=1403)
	**{
	**	PUBLOG_DBDEBUG("DcustModeChgAwake");
	**	pubLog_DBErr(_FFL_, "","-3","发送提醒短信错误[%d] ",SQLCODE);
	**	return -3;
	**}
	**/
	EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss')
				INTO :vOpTime
				FROM DUAL;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("DcustModeChgAwake");
		pubLog_DBErr(_FFL_, "","-4","查询系统时间失败 SQLCODE[%d] ",SQLCODE);
		return -4;
	}
	ret=0;
	ret = GenerateShortMsg(smModelId,phoneNo,loginNo,opCode,loginAccept,vOpTime);
	if ( ret != 0 )
	{
		PUBLOG_DBDEBUG("DcustModeChgAwake");
		pubLog_DBErr(_FFL_, "","-3","发送提醒短信错误[%d] ",ret);
		return -3;
	}
	return 0;
}


/***
业务变更短信提醒
***/
int pubChgAwake(char *op_code,char *login_no,char *phone_no)
{
	EXEC SQL BEGIN DECLARE SECTION;

	long	loginAccept=0;
	char	opCode[4+1];
	char	loginNo[6+1];
	char	phoneNo[15+1];
	char	sendMsg[512];
	char	tempBuf[128];
	char	opMonth[2+1];
	char	opDate[2+1];
	char	opHour[2+1];
	char	opMi[2+1];
	char	opName[60+1];
	/**统一短信**/
	int		smModelId=0;
	int		ret=0;
	char	vOpTime[19+1];

	EXEC SQL END DECLARE SECTION;

	init(opCode);
	init(loginNo);
	init(phoneNo);
	init(sendMsg);
	init(tempBuf);
	init(opMonth);
	init(opDate);
	init(opHour);
	init(opMi);
	init(opName);
	init(vOpTime);
	strcpy(opCode,op_code);
	strcpy(loginNo,login_no);
	strcpy(phoneNo,phone_no);

	if(strlen(phoneNo) == 0)
	{
		return 0;
	}

	EXEC SQL SELECT op_name,to_char(sysdate,'MM'),to_char(sysdate,'DD'),to_char(sysdate,'HH24'),to_char(sysdate,'MI'),sMaxSysAccept.nextval
					INTO :opName,:opMonth,:opDate,:opHour,:opMi,:loginAccept FROM sawakeopcode
					WHERE op_code=:opCode;
	if(SQLCODE==1403)
	{
		return 0;
	}
	else if(SQLCODE!=0 && SQLCODE!=1403)
	{
		PUBLOG_DBDEBUG("pubChgAwake");
		pubLog_DBErr(_FFL_, "","-1","select sawakeopcode error[%d] ",SQLCODE);
		return -1;
	}

	sprintf(sendMsg,"您于%s月%s日%s时%s分在",opMonth,opDate,opHour,opMi);

	if(strcmp(loginNo,"www000")==0)
	{
		strcpy(tempBuf,"网上营业厅");
	}
	else if(strcmp(loginNo,"dx0000")==0)
	{
		strcpy(tempBuf,"短信营业厅");
	}
	else if(strcmp(loginNo,"wap000")==0)
	{
		strcpy(tempBuf,"WAP掌上营业厅");
	}
	else if(strcmp(loginNo,"IVR000")==0)
	{
		strcpy(tempBuf,"10086客服热线");
	}
	else if(strncmp(loginNo,"k",1)==0)
	{
		strcpy(tempBuf,"10086客服热线");
	}
	else
	{
		/*---组织机构改造替换字段 edit by magang at 08/05/2009---
		EXEC SQL SELECT a.town_name
				into :tempBuf
				from stowncode a,dloginmsg b
				where b.login_no=:loginNo
				and a.region_code=substr(b.org_code,1,2)
				and district_code=substr(b.org_code,3,2)
				and town_code=substr(b.org_code,5,3);
				---组织机构改造替换字段 edit by magang at 08/05/2009---*/
				EXEC SQL SELECT a.group_name
				into :tempBuf
				FROM dChnGroupmsg a,dLoginMsg b
				WHERE b.login_no=:loginNo
				and a.group_id=b.group_id ;
		if(SQLCODE!=0)
		{
			PUBLOG_DBDEBUG("pubChgAwake");
			pubLog_DBErr(_FFL_, "","-2","select stowncode error[%d] ",SQLCODE);
			return -2;
		}
	}
	Trim(tempBuf);
	Trim(opName);

	sprintf(sendMsg,"%s%s办理了%s业务.",sendMsg,tempBuf,opName);
	/**统一短信改造 yaoxc 20111110
	**EXEC SQL insert into WCOMMONSHORTMSGTX(COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,
	**				PHONE_NO,MSG,ORDER_CODE,BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME,sent_time)
	**			select SMSG_SEQUENCE.nextval,:loginAccept,1,:phoneNo,:sendMsg,
	**				0,0,:opCode,:loginNo,sysdate,'' from dual;
	**if(SQLCODE!=0 && SQLCODE!=1403)
	**{
	**	PUBLOG_DBDEBUG("pubChgAwake");
	**	pubLog_DBErr(_FFL_, "","-3","发送提醒短信错误[%d] ",SQLCODE);
	**	return -3;
	**}
	**/
	EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss')
				INTO :vOpTime
				FROM DUAL;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("pubChgAwake");
		pubLog_DBErr(_FFL_, "","-4","查询系统时间失败 SQLCODE[%d] ",SQLCODE);
		return -4;
	}
	smModelId = 2313;
	PrepareNewShortMsg(smModelId);
	SetShortMsgParameter(smModelId, "MONTH", opMonth);
	SetShortMsgParameter(smModelId, "DAY", opDate);
	SetShortMsgParameter(smModelId, "HOUR", opHour);
	SetShortMsgParameter(smModelId, "MIN", opMi);
	SetShortMsgParameter(smModelId, "BUSINESS_SITE", tempBuf);
	SetShortMsgParameter(smModelId, "MODE_NAME", opName);
	ret=0;
	ret = GenerateShortMsg(smModelId,phoneNo,loginNo,opCode,loginAccept,vOpTime);
	if ( ret != 0 )
	{
		PUBLOG_DBDEBUG("pubChgAwake");
		pubLog_DBErr(_FFL_, "","-3","发送提醒短信错误[%d] ",ret);
		return -3;
	}
	return 0;
}


/***
查询指定字符串中的第i个子串
author:yinyx
date:2006.2.28
S  :原始字符串，必须以分隔符结束
I  :准备获取字符串的位置，从0开始
CH :分隔符
RET:返回字符串，如果没有取到值则返回为空字符串
Result:正常为字符串的位置，>=0，错误为负数
Sample:
	getValueByIndex("a|b|c|", 0, '|', b); //result=0; b="a";
	getValueByIndex("a|b|c|", 1, '|', b); //result=1; b="b";
	getValueByIndex("a|b|c|", 3, '|', b); //result=-1; b="";
Test:
	void main()
	{
		char a[20];
		char b[10];
		int  i = 0;
		memset(a, 0, sizeof(a));
		memset(b, 0, sizeof(b));
		strcpy(a, "01|23|45|67|");
		while(getValueByIndex(a, i, '|', b)>=0)
		{
			printf("b=[%s] ", b);
			i++;
		}
	}
***/
int getValueByIndex(char *S, int I, char CH, char *RET)
{
	int i = 0;
	int pos = 0, bakpos = 0;
	int iLen = 0;

	if (S[0] == 0)
	{
		pubLog_DBErr(_FFL_, "","-1","");
		 return -1;
	}
	if (I < 0)
	{
		pubLog_DBErr(_FFL_, "","I","");
		return I;
	}

	iLen = strlen(S);
	if (S[iLen - 1] != CH)
	{
		S[iLen] = CH;
		S[iLen + 1] = 0;
	}

	while((pos = (char *)strchr(S + pos, CH) - S + 1) > 0 )
	{
		if (i == I)
		{
			strncpy(RET, S + bakpos, pos - bakpos - 1);
			pubLog_DBErr(_FFL_, "","i","");
			return i;
		}
		else
		{
			memset(RET, 0, sizeof(RET));
		}
		if (pos>=strlen(S)) break;
		i++;
		bakpos = pos;
	}
	pubLog_Debug(_FFL_,"","-1","");
	return -1;
}

/* 检查用户服务级的互斥 */
int chkModeSrvRela(char *iIdNo, char *iModeCode, char *iRegionCode, char *oRetMsg){
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0;
		char vModeCode[8+1];
	EXEC SQL END   DECLARE SECTION;

	EXEC SQL select a.MODE_CODE INTO :vModeCode
		from dBillCustDetail a, sBillModeSrv b, sBillModeSrv c, cBillSrvRela d
		where a.REGION_CODE = b.REGION_CODE and a.MODE_CODE = b.MODE_CODE
			and a.MODE_TIME = 'Y' and a.END_TIME > sysdate
			and a.REGION_CODE = c.REGION_CODE and b.SRV_CODE = d.OLD_SRV and c.SRV_CODE = d.NEW_SRV
			and d.RELA_MODE = 'M' and d.FORCE_FLAG = 'Y'
			and c.MODE_CODE = :iModeCode and a.REGION_CODE = :iRegionCode and a.ID_NO = :iIdNo;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(oRetMsg, "查询互斥关系表cBillSrvRela失败[%d]", SQLCODE);
		pubLog_DBErr(_FFL_, "","-1","%s",oRetMsg);
		return -1;
	}

	if (SQLCODE == 0) {
		sprintf(oRetMsg, "新订购产品[%s]和原有产品[%s]服务互斥，不能订购", iModeCode, vModeCode);
		pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
		return -2;
	}

	return 0;
}

/* 检查用户产品级的互斥 */
/*---组织机构改造替换字段 edit by magang at 06/05/2009---*/
int chkBillModeLimit(char *iIdNo, char *iModeCode, char *iRegionCode, char *oRetMsg){
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0;
		char vModeCode[8+1];
	EXEC SQL END   DECLARE SECTION;

	init(vModeCode);

	EXEC SQL select a.mode_code into :vModeCode
		from dBillCustDetail a, cBillModeLimit b
		where a.id_no = :iIdNo and a.mode_time = 'Y' and a.end_time > sysdate
			and b.region_code = :iRegionCode
			and a.mode_code = b.limit_mode
			and b.request_mode = :iModeCode;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(oRetMsg, "查询互斥关系表cBillModeLimit失败[%d]", SQLCODE);
		pubLog_DBErr(_FFL_, "","-1","%s",oRetMsg);
		return -1;
	}

	if (SQLCODE == 0) {
		sprintf(oRetMsg, "新订购产品[%s]和原有产品[%s]互斥，不能订购", iModeCode, vModeCode);
		pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
		return -2;
	}

	return 0;
}

/* 检查用户类别级的互斥 */
int chkModeTypeRela(char *iIdNo, char *iModeType, char *iRegionCode, char *oRetMsg){
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0;
		char vModeCode[8+1];
	EXEC SQL END   DECLARE SECTION;

	EXEC SQL select a.mode_code into :vModeCode
		from dBillCustDetail a, sBillModeCode b, cBillTypeRela c
		where a.id_no = :iIdNo and a.mode_time = 'Y' and sysdate < a.end_time
		and a.region_code = b.region_code and a.mode_code = b.mode_code
		and a.region_code = c.region_code and b.mode_type = c.old_type
		and c.new_type = :iModeType and c.rela_mode = 'M' and c.force_flag = 'Y' and c.region_code = :iRegionCode;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(oRetMsg, "查询互斥关系表cBillKindRela失败[%d]", SQLCODE);
		pubLog_DBErr(_FFL_, "","-1","%s",oRetMsg);
		return -1;
	}

	if (SQLCODE == 0) {
		sprintf(oRetMsg, "新订购产品类别[%s]和原有产品[%s]类别互斥，不能订购", iModeType, vModeCode);
		pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
		return -2;
	}

	return 0;
}

/* 分发绑定业务指令 */
int fDispatchCmd(char *iCmdCode, TPubBillInfo *tPubBillInfo, char *oRetMsg){
	int iRetCode = 0;

	iRetCode = _dealColorRing(iCmdCode, tPubBillInfo, oRetMsg);
	if (iRetCode != 0) {
		pubLog_DBErr(_FFL_, "","-1","");
		return -1;
	}

	/* 等DSMP上线后取消注释 */
	iRetCode = _dealDsmpProd(iCmdCode, tPubBillInfo, oRetMsg);
	if (iRetCode != 0) {
		pubLog_DBErr(_FFL_, "","-1","");
		return -1;
	}

	return iRetCode;
}

/* 处理彩铃业务绑定 */
int _dealColorRing(char *iCmdCode, TPubBillInfo *tPubBillInfo, char *oRetMsg){
	int i;
	EXEC SQL BEGIN DECLARE SECTION;
		char vPromName[20+1];
		char vDealFunc[2+1];
		char vFuncCommand[256];
		char vOpPlace[3+1];
		TPubBillInfo *pBi;
		char vDealFuncadd[2+1];
		char vFuncCommandadd[256];
		int  count=0;
	EXEC SQL END   DECLARE SECTION;

	pBi = tPubBillInfo;
	pubLog_Debug(_FFL_, "","","begin  select  ssndcailcfgadd [%s][%s]",pBi->BelongCode,pBi->ModeCode);
	EXEC SQL select count(*)
		into :count
		from ssndcailcfgadd
		where region_code=substr(:pBi->BelongCode,1,2) and mode_code=:pBi->ModeCode;
	if (SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("_dealColorRing");
		pubLog_DBErr(_FFL_, "","-1","_dealColorRing:select ssndcailcfgadd error[%d],[%s]\n",SQLCODE,SQLERRMSG);
    strcpy(oRetMsg,"查询ssndcailcfgadd表错误!");
    return -3;
  }
	if (iCmdCode[0] == '1') {					/* 开通彩铃业务 */
		if(count>0)
		{
			EXEC SQL DECLARE CurSndC02 CURSOR for
    	  select deal_func,nvl(func_command,' ')
    	  from ssndcailcfgadd
    	  where region_code=substr(:pBi->BelongCode,1,2) and mode_code=:pBi->ModeCode;
			EXEC SQL open CurSndC02;
    	for(i=0;;)
    	{
    	  init(vDealFuncadd);
    	  init(vFuncCommandadd);
    	  EXEC SQL fetch CurSndC02 into :vDealFuncadd,:vFuncCommandadd;
    	  if (SQLCODE!=0)
    	  {
    	  	PUBLOG_DBDEBUG("_dealColorRing");
					pubLog_DBErr(_FFL_, "","-1","_dealColorRing:no ssndcailcfgadd[%d],[%s]\n",SQLCODE,SQLERRMSG);
    	    break;
    	  }
    	  Trim(vDealFuncadd);
    	  Trim(vFuncCommandadd);
    	  if(strlen(vFuncCommandadd)==0)
    	  {
    	  	EXEC SQL insert into wsndcailmsghis(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	  							LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
	  							DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
	  			select LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,'d',
	  							sysdate,SND_TIMES,prom_name,deal_func,begin_time,mode_accept
	  			from wsndcailmsg
	  			where id_no=to_number(:pBi->IdNo) and op_type='d' and	deal_func=:vDealFuncadd;
	  			if (SQLCODE!=0 && SQLCODE!=1403)
	  			{
	  				PUBLOG_DBDEBUG("_dealColorRing");
						pubLog_DBErr(_FFL_, "","-1","insert into wsndcailmsghis error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      			strcpy(oRetMsg,"处理wsndcailmsg1错误!");
      			return -1;
	  			}
    	  	EXEC SQL delete from wsndcailmsg
    	  					where id_no=to_number(:pBi->IdNo)
    	  					and op_type='d'
    	  					and	deal_func=:vDealFuncadd;
    	  	if (SQLCODE!=0 && SQLCODE!=1403)
    			{
    				PUBLOG_DBDEBUG("_dealColorRing");
						pubLog_DBErr(_FFL_, "","-1","delete from wsndcailmsg error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      			strcpy(oRetMsg,"处理wsndcailmsg错误!");
      			EXEC SQL close CurSndC02;
      			return -4;
    			}
    	  }
    	  else
    	  {
    	  	EXEC SQL insert into wsndcailmsghis(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	  							LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
	  							DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
	  			select LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,'d',
	  							sysdate,SND_TIMES,prom_name,deal_func,begin_time,mode_accept
	  			from wsndcailmsg
	  			where id_no=to_number(:pBi->IdNo) and op_type='d' and	deal_func=:vDealFuncadd and func_command=:vFuncCommandadd;
	  			if (SQLCODE!=0 && SQLCODE!=1403)
	  			{
	  				PUBLOG_DBDEBUG("_dealColorRing");
						pubLog_DBErr(_FFL_, "","-1","insert into wsndcailmsghis error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      			strcpy(oRetMsg,"处理wsndcailmsg1错误!");
      			return -1;
	  			}
    	  	EXEC SQL delete from wsndcailmsg
    	  					where id_no=to_number(:pBi->IdNo)
    	  					and op_type='d'
    	  					and	deal_func=:vDealFuncadd
    	  					and func_command=:vFuncCommandadd;
    	  	if (SQLCODE!=0 && SQLCODE!=1403)
    			{
    				PUBLOG_DBDEBUG("_dealColorRing");
						pubLog_DBErr(_FFL_, "","-1","delete from wsndcailmsg error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      			strcpy(oRetMsg,"处理wsndcailmsg错误!");
      			EXEC SQL close CurSndC02;
      			return -4;
    			}
    	  }
    	}
    	EXEC SQL close CurSndC02;
    }
    EXEC SQL DECLARE CurSndC01 CURSOR for
      select prom_name,deal_func,nvl(func_command,' '),nvl(op_place,' ')
      from ssndcailcfg
      where region_code = substr(:pBi->BelongCode,1,2) and mode_code = :pBi->ModeCode;
    EXEC SQL open CurSndC01;
    for(i=0;;)
    {
      init(vPromName);
      init(vDealFunc);
      init(vFuncCommand);
      init(vOpPlace);

      EXEC SQL fetch CurSndC01 into :vPromName,:vDealFunc,:vFuncCommand,:vOpPlace;
      if (SQLCODE!=0){
      	PUBLOG_DBDEBUG("_dealColorRing");
				pubLog_DBErr(_FFL_, "","-1","no ssndcailcfg[%d],[%s]\n",SQLCODE,SQLERRMSG);
        break;
      }

      EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
        	LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
        	DEAL_TIME,SND_TIMES,prom_name,deal_func,
        	begin_time,mode_accept,func_command,op_place)
        values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
          to_number(:pBi->IdNo),:pBi->PhoneNo,:pBi->LoginNo,:pBi->OpCode,sysdate,
          :pBi->TotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
          to_date(:pBi->EffectTime,'yyyymmdd hh24:mi:ss'),
          to_number(:pBi->LoginAccept),:vFuncCommand,:vOpPlace);
      if (SQLCODE!=0) {
      	PUBLOG_DBDEBUG("_dealColorRing");
				pubLog_DBErr(_FFL_, "","-1","insert wsndcailmsg error[%d],[%s]\n",SQLCODE,SQLERRMSG);
        strcpy(oRetMsg,"插入wsndcailmsg表错误!");
        EXEC SQL close CurSndC01;
        return -1;
      }
    }
    EXEC SQL close CurSndC01;
	} else if (iCmdCode[0] == '0') {	/* 取消彩铃业务 */
		pubLog_Debug(_FFL_, "","",":begin  insert into   wsndcailmsghis [%s][%s]\n",pBi->LoginAccept,pBi->IdNo);
	  EXEC SQL insert into wsndcailmsghis(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	  		LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
	  		DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
	  	select LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	  		LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,'d',
	  		sysdate,SND_TIMES,prom_name,deal_func,begin_time,mode_accept
	  	from wsndcailmsg
	  	where mode_accept=to_number(:pBi->LoginAccept) and id_no=to_number(:pBi->IdNo);
	  if (SQLCODE!=0 && SQLCODE!=1403) {
	  	PUBLOG_DBDEBUG("_dealColorRing");
			pubLog_DBErr(_FFL_, "","-1","insert into wsndcailmsghis error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      strcpy(oRetMsg,"处理wsndcailmsg1错误!");
      return -1;
	  }

    EXEC SQL delete from wsndcailmsg where mode_accept=to_number(:pBi->LoginAccept) and id_no=to_number(:pBi->IdNo);
    if (SQLCODE!=0 && SQLCODE!=1403) {
    	PUBLOG_DBDEBUG("_dealColorRing");
			pubLog_DBErr(_FFL_, "","-1","delete from wsndcailmsg error[%d],[%s]\n",SQLCODE,SQLERRMSG);
      strcpy(oRetMsg,"处理wsndcailmsg错误!");
      return -2;
    }
    pubLog_Debug(_FFL_, "","","count=[%d]\n",count);
    if(count>0)
    {
    	EXEC SQL DECLARE CurSndC03 CURSOR for
      select prom_name,deal_func,nvl(func_command,' '),nvl(op_place,' ')
      from ssndcailcfg
      where region_code = substr(:pBi->BelongCode,1,2) and mode_code = :pBi->ModeCode;
    	EXEC SQL open CurSndC03;
    	for(i=0;;)
    	{
    	  init(vPromName);
    	  init(vDealFunc);
    	  init(vFuncCommand);
    	  init(vOpPlace);

    	  EXEC SQL fetch CurSndC03 into :vPromName,:vDealFunc,:vFuncCommand,:vOpPlace;
    	  if (SQLCODE!=0){
    	  	PUBLOG_DBDEBUG("_dealColorRing");
					pubLog_DBErr(_FFL_, "","-1","no ssndcailcfg[%d],[%s]\n",SQLCODE,SQLERRMSG);
    	    break;
    	  }
    	  pubLog_Debug(_FFL_, "","","fetch CurSndC03 ok [%s],[%s],[%s],[%s]\n",vPromName,vDealFunc,vFuncCommand,vOpPlace);

    	  EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
    	    	LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
    	    	DEAL_TIME,SND_TIMES,prom_name,deal_func,
    	    	begin_time,mode_accept,func_command,op_place)
    	    values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
    	      to_number(:pBi->IdNo),:pBi->PhoneNo,:pBi->LoginNo,:pBi->OpCode,sysdate,
    	      :pBi->TotalDate,'d',sysdate,0,:vPromName,:vDealFunc,
    	      to_date(:pBi->EffectTime,'yyyymmdd hh24:mi:ss'),
    	      to_number(:pBi->LoginAccept),:vFuncCommand,:vOpPlace);
    	  if (SQLCODE!=0) {
    	  	PUBLOG_DBDEBUG("_dealColorRing");
					pubLog_DBErr(_FFL_, "","-1","insert wsndcailmsg error[%d],[%s]\n",SQLCODE,SQLERRMSG);
    	    strcpy(oRetMsg,"插入wsndcailmsg表错误!");
    	    EXEC SQL close CurSndC03;
    	    return -1;
    	  }
    	}
    	EXEC SQL close CurSndC03;
    }
	}

  return 0;
}

/* 处理DSMP业务绑定 */
int _dealDsmpProd(char *iCmdCode, TPubBillInfo *tPubBillInfo, char *oErrMsg){
	int i, iCount = 0;
	int ret =0;
	EXEC SQL BEGIN DECLARE SECTION;
		char vBeginTime[17+1];
		char vEndTime[17+1];
		char vSqlStr[2048];
		char vGroupId[10+1];
		TPubBillInfo *pBi;
	EXEC SQL END   DECLARE SECTION;

	pBi = tPubBillInfo;

	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       init(vGroupId);
       ret = sGetUserGroupid(pBi->PhoneNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败! ");
         	  return 200919;
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	if (iCmdCode[0] == '1') {		      /* 开通DSMP业务 */
		/*lijian add 20080309*/
		/*查询资费是否为DSMP数据业务包,如果是DSMP数据业务包资费,需记录 PDSMPORDERPENDING */
		EXEC SQL select count(*) into :iCount
			from sDsmpProdTrans
	  	where region_code = substr(:pBi->BelongCode,1,2) and mode_code = :pBi->ModeCode
	  		and mode_comb_flag = '1' and (sysdate between begin_time and end_time);
	  if (SQLCODE != 0) {
	  		strcpy(oErrMsg,"查询DSMP数据业务包资费错误!");
	  		PUBLOG_DBDEBUG("_dealDsmpProd");
			pubLog_DBErr(_FFL_, "","-1","[%s] error [%d] ",oErrMsg);
			return -1;
	  }

		if (iCount > 0) {
			pubLog_Debug(_FFL_,"","","----_dealDsmpProd :数据DSMP业务套餐,PhoneNo =[%s] modeCode=[%s] ", pBi->PhoneNo, pBi->ModeCode);
			Sinitn(vSqlStr);
			Sinitn(vBeginTime);
			Sinitn(vEndTime);

			sprintf(vSqlStr, "select to_char(begin_time,'yyyymmdd hh24:mi:ss'), to_char(end_time ,'yyyymmdd hh24:mi:ss') "
		    "from dBillCustDetail%c "
		    "where id_no = :v1 and mode_code = :v2 "
		    "and end_time > sysdate and login_accept = :v3", pBi->IdNo[strlen(pBi->IdNo)-1]);
#ifdef _BILLDEBUG_
	pubLog_Debug(_FFL_,"","","----_dealDsmpProd %s ",vSqlStr);
#endif

			EXEC SQL prepare preBillBegin_2 from :vSqlStr;
			EXEC SQL declare curBillBegin_2 cursor for preBillBegin_2;
			EXEC SQL open curBillBegin_2 using :pBi->IdNo, :pBi->ModeCode, :pBi->LoginAccept;
			if (SQLCODE != 0) {
				strcpy(oErrMsg,"取dBillCustDetail的开始结束时间错误!");
				PUBLOG_DBDEBUG("_dealDsmpProd");
				pubLog_DBErr(_FFL_, "","-1","_dealColorRing :dBillCustDetail的开始结束时间错误 PhoneNo=[%s] [%s] ",pBi->PhoneNo,oErrMsg);
				EXEC SQL close curBillBegin_2;
				return -1;
			}

	 		EXEC SQL fetch curBillBegin_2 into :vBeginTime, :vEndTime;
			if (SQLCODE != 0) {
				strcpy(oErrMsg,"取dBillCustDetail的开始结束时间错误!");
				PUBLOG_DBDEBUG("_dealDsmpProd");
				pubLog_DBErr(_FFL_, "","-1","_dealColorRing:PhoneNo =[%s],[%s] ",pBi->PhoneNo,oErrMsg);

				EXEC SQL CLOSE curBillBegin_2;
				return -1;
			}
			EXEC SQL close curBillBegin_2;

			EXEC SQL insert into pDsmpOrderPending
					(order_id, id_no, phone_no, serv_code,
					opr_code, third_id_no, third_phoneno, spid, bizcode, prod_code, comb_flag,
					mode_code, chg_flag, chrg_type, channel_id, opr_source, first_order_date, first_login_no,
					last_order_date, opr_time,
					eff_time, end_time,
					local_code, valid_flag, belong_code, total_date,
					login_accept, sm_code, password, reserved, order_deal_flag, dsmp_send_flag, is_order_flag,
					op_code, pending_time,group_id)
				select
					sMaxSysAccept.nextval, to_number(:pBi->IdNo), :pBi->PhoneNo, c.biztype,
					nvl(d.default_orderopr, '06'), 0, ' ', a.spid, a.bizcode, b.prod_code, b.mode_comb_flag,
					nvl(:pBi->ModeCode, ' '), '0', '0', ' ', '08', sysdate, :pBi->LoginNo,
					to_date(:pBi->OpTime, 'yyyymmdd hh24:mi:ss'), to_date(:pBi->OpTime, 'yyyymmdd hh24:mi:ss'),
					to_date(:vBeginTime, 'yyyymmdd hh24:mi:ss'), to_date(:vEndTime, 'yyyymmdd hh24:mi:ss'),
					nvl(e.servattr, 'L'), '1', :pBi->BelongCode, to_number(:pBi->TotalDate),
					to_number(:pBi->LoginAccept), nvl(:pBi->SmCode, ' '), ' ', ' ', '1', '1', '1',
					:pBi->OpCode, to_date(:vBeginTime, 'yyyymmdd hh24:mi:ss'),:vGroupId
				from sDsmpProdBiz a, sDsmpProdTrans b, dDsmpSpInfo c, sObBizType_DSMP d, dDsmpSpBizInfo e
				where a.prod_code = b.prod_code and (sysdate between a.begin_time and a.end_time)
					and b.region_code = substr(:pBi->BelongCode, 1, 2) and b.mode_code = :pBi->ModeCode
					and a.spid = c.spid and (sysdate between b.begin_time and b.end_time)
					and c.biztype = d.oprcode and a.spid = e.spid and a.bizcode = e.bizcode
					and sysdate between to_date(c.validdate,'yyyymmdd') and to_date(c.expiredate,'yyyymmdd')
					and sysdate between to_date(e.validdate,'yyyymmdd') and to_date(e.expiredate,'yyyymmdd');
			if (SQLCODE != 0) {
				strcpy(oErrMsg,"插入工单表pDsmpOrderPending错误!");
				PUBLOG_DBDEBUG("_dealDsmpProd");
				pubLog_DBErr(_FFL_, "","-1","_dealColorRing : insert into pDsmpOrderPending错误 PhoneNo=[%s]  [%s] ",pBi->PhoneNo,oErrMsg);

				return -1;
			}
		}

	}
	 else if (iCmdCode[0] == '0')
	 {  /* 取消DSMP业务 */
		EXEC SQL select count(*) into :iCount
			from sDsmpProdTrans
			where region_code = substr(:pBi->BelongCode,1,2) and mode_code = :pBi->ModeCode
				and mode_comb_flag = '1' and (sysdate between begin_time and end_time);
		if (SQLCODE != 0) {
			strcpy(oErrMsg,"查询DSMP数据业务包资费错误!");
			PUBLOG_DBDEBUG("_dealDsmpProd");
			pubLog_DBErr(_FFL_, "","-1","_dealColorRing :select SDSMPPRODTRANS,PhoneNo =[%s]  [%s] ",pBi->PhoneNo,oErrMsg);

			return -1;
		}

		if (iCount > 0) {
			/**读取定购关系，插入pDsmpOrderPending表**/
			memset(vSqlStr, 0, sizeof(vSqlStr));
			sprintf(vSqlStr, " INSERT INTO pDsmpOrderPending "
					"("
					" order_id,id_no,phone_no,serv_code                 "
					" ,opr_code,third_id_no,third_phoneno,spid          "
					" ,bizcode,prod_code,comb_flag,mode_code            "
					" ,chg_flag,chrg_type,channel_id,opr_source         "
					" ,first_order_date,first_login_no,last_order_date  "
					" ,opr_time,eff_time,end_time                       "
					" ,local_code,valid_flag,belong_code                "
					" ,total_date,login_accept,reserved ,sm_code ,password ,"
					" ORDER_DEAL_FLAG,DSMP_SEND_FLAG ,IS_ORDER_FLAG, op_code, PENDING_TIME ,GROUP_ID "
					")"
					"  SELECT order_id,id_no,phone_no,serv_code                 "
					" ,nvl(b.DEFAULT_CANCLEOPR,'07'),third_id_no,third_phoneno,spid          "
					" ,bizcode,prod_code,comb_flag,mode_code            "
					" ,chg_flag,chrg_type,channel_id,opr_source         "
					" ,first_order_date,first_login_no,last_order_date  "
					" ,to_date(:v1,'yyyymmdd hh24:mi:ss'),eff_time,to_date(:v2,'yyyymmdd hh24:mi:ss')  "
					" ,local_code,'0',:v3        "
					" ,to_number(:v4),to_number(:v5),reserved ,sm_code ,password ,'1','1','0', "
					" :v6, to_date(:v7,'yyyymmdd hh24:mi:ss')   ,:v10"
					" FROM dDsmpOrderMsg%c a, sObBizType_DSMP b"
					" where a.serv_code = b.oprcode  "
					"   and ( sysdate between a.eff_time  and a.end_time)  "
					"   and a.id_no = :v8 AND a.mode_code = :v9 " , pBi->IdNo[strlen(pBi->IdNo)-1]);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","vSqlStr=[%s] ", vSqlStr);
#endif
			EXEC SQL PREPARE prepare1 FROM :vSqlStr;
			EXEC SQL EXECUTE prepare1 USING :pBi->OpTime, :pBi->EffectTime, :pBi->BelongCode,
				:pBi->TotalDate, :pBi->LoginAccept, :pBi->OpCode, :pBi->OpTime,:vGroupId, :pBi->IdNo, :pBi->ModeCode;
			if (SQLCODE != 0 && SQLCODE != 1403) {
				sprintf(oErrMsg,"插入工单表pDsmpOrderPending错误[%d]",SQLCODE);
				PUBLOG_DBDEBUG("_dealDsmpProd");
				pubLog_DBErr(_FFL_, "","-1","----: insert into pDsmpOrderPending错误 PhoneNo=[%s]  [%s] ",pBi->PhoneNo,oErrMsg);

				return -1;
			} else if (SQLROWS == 0 || SQLCODE == 1403) {
				EXEC SQL delete from pDsmpOrderPending
					where id_no = to_number(:pBi->IdNo) and mode_code = :pBi->ModeCode
						and login_accept = to_number(:pBi->LoginAccept);
				if (SQLCODE != 0 && SQLCODE != 1403) {
					sprintf(oErrMsg,"删除产品定购预处理表pDsmpOrderPending错误[%d]",SQLCODE);
					PUBLOG_DBDEBUG("_dealDsmpProd");
					pubLog_DBErr(_FFL_, "","-1","[%s] ",_dealDsmpProd);
					return -1;
				}
			}
  	}
	}

	return 0;
}

int pubTCBillBeginTimeMode(
    char *iPhoneNo,
    char *iNewMode,
    char *iEffectTime,
    char *iEndTime,
    char *iOpCode,
    char *iLoginNo,
    char *iLoginAccept,
    char *iOpTime,
    char *iSendFlag,
    char *iFlagStr,
    char *iBunchNo,
    char *iFlagCode,
    char *oErrMsg)
{
    EXEC SQL BEGIN DECLARE SECTION;
      char    vIdNo[23];
      int     i=0,t=0,vTotalDate=0,vFavOrder=0,ret =0;
      char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
      int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
      char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
      char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
      char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
      int     vSendFlag,vUserFunc=-1;
      char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
      double  vMocRate=0,vMtcRate=0;
      char    vTimeCode[3];
      tUserBase   userBase;
      float       vPrepayFee=0;
	  int         vCount=0;
      tGrantData vGrantMsg;
      int     iProductRetCode = 0;
      char vPromName[20+1];
	  char vDealFunc[2+1];
	  char vSrvCode[4+1];
	  char vSrvNetType[1+1];
	  char vCmdCode[256];
	  char vOrgId[10+1];
	  char vGroupId[10+1];
	  TPubBillInfo tPubBillInfo;

	  char vRegionCode[2+1];
	  int vMaxNum, vCurNum, vBuyNum;
	  char vNewType[1+1];

	  /*ng解耦*/
	  char 			vEffectTime[17+1];
	  char    		dynStmt[1024];
	  char 			vMocRateStr[8+1];
	  char 			vMtcRateStr[8+1];
	  char 			vFavOrderStr[4+1];
	  char			vTotalDateStr[8+1];
	  int 			iNum = 0;
	  int 			iNum2 = 0;
	  char 			v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	  TdBaseFav 			vTdBaseFav;
	  TdBaseVpmn 			vTdBaseVpmn;
	  TdBillCustDetail 		vTdBillCustDetail;
	  TdCustExpire 			vTdCustExpire;
	  TdCustFunc			vTdCustFunc;
	  TdCustFuncIndex		vTdCustFuncIndex;
	  TdCustFuncHis			vTdCustFuncHis;
	  
	  
		/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.03.19 end*/
      EXEC SQL END   DECLARE SECTION;

      Trim(iPhoneNo);
      Trim(iNewMode);
      Trim(iEffectTime);
      Trim(iEndTime);
      Trim(iOpCode);
      Trim(iLoginNo);
      Trim(iLoginAccept);
      Trim(iOpTime);


      oErrMsg[0] = '\0';
    /*init(oErrMsg);*/
#ifdef _WTPRN_

    pubLog_Debug(_FFL_,"","","--pubBillBeginTimeMode  begin ----- ");
   	pubLog_Debug(_FFL_,"","","iPhoneNo=[%s]",iPhoneNo);
   	pubLog_Debug(_FFL_,"","","iNewMode=[%s] ",iNewMode);
    pubLog_Debug(_FFL_,"","","iEffectTime=[%s]",iEffectTime);
    pubLog_Debug(_FFL_,"","","iOpCode=[%s] ",iOpCode);
    pubLog_Debug(_FFL_,"","","iLoginNo=[%s]",iLoginNo);
    pubLog_Debug(_FFL_,"","","iLoginAccept=[%s] ",iLoginAccept);
    pubLog_Debug(_FFL_,"","","iOpTime=[%s]",iOpTime);
    pubLog_Debug(_FFL_,"","","iSendFlag=[%s] ",iSendFlag);
    pubLog_Debug(_FFL_,"","","iFlagStr=[%s]",iFlagStr);
#endif
    Sinitn(vIdNo);
    Sinitn(vOrgId);
    Sinitn(vGroupId);
    Sinitn(vRegionCode);
    Sinitn(vEffectTime);
    Sinitn(v_parameter_array);
    Sinitn(vFavOrderStr);
    Sinitn(vTotalDateStr);

    memset(dynStmt, 0, sizeof(dynStmt));
    memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));

	memset(&vMocRateStr, 0, sizeof(vMocRateStr));
	memset(&vMtcRateStr, 0, sizeof(vMtcRateStr));
	/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.04.17 end*/
    Trim(iOpTime);


    /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret = sGetUserGroupid(iPhoneNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败! ");
         	  return 200919;
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    exec sql select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
    into :vIdNo,:vBelongCode,:vTotalDate
    from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
    if(SQLCODE!=OK){
    	strcpy(oErrMsg,"查询dCustMsg错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
        return 1024;
    }

  Trim(vGroupId);
	strncpy(vRegionCode, vBelongCode, 2);

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}

	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127075"," [%s] ", oErrMsg);

		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}
	/* 检查资费关系结束 chenxuan boss3 */

/*
	EXEC SQL declare CurSndC011 cursor for
		select prom_name,deal_func
			from ssndcailcfg
			where region_code=substr(:vBelongCode,1,2)
    		and mode_code =:iNewMode;
	EXEC SQL open CurSndC011;
	for(i=0;;)
	{
		init(vPromName);
		init(vDealFunc);
		EXEC SQL fetch CurSndC011 into :vPromName,:vDealFunc;
		if(SQLCODE!=0)
		{
			printf(" .pubBillBeginTimeMode :select ssndcailcfg error [%d][%s] ", oErrMsg);
			break;
		}
		EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
										LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
										DEAL_TIME,SND_TIMES,prom_name,deal_func,begin_time,mode_accept)
					values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
							to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
							:vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
							to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_number(:iLoginAccept));
		if(SQLCODE!=0)
		{
				printf(" .pubBillBeginTimeMode :insert wsndcailmsg error [%d][%s] ", oErrMsg);
        		strcpy(oErrMsg,"插入wsndcailmsg表错误!");
        		EXEC SQL close CurSndC011;
        		return 1024;
		}
	}
	EXEC SQL close CurSndC011;
*/
    Trim(vIdNo);
    Sinitn(vHlrCode);
    Sinitn(vImsiNo);Sinitn(vSimNo);
    /*
    if(iOpCode[0]!='8'){*** 专线用户 ***
    */
    if(strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0 && strncmp(iNewMode,"id",2)!=0){/*** 专线用户 ***/
        exec sql select hlr_code into :vHlrCode from sHlrCode
        where phoneno_head=substr(:iPhoneNo,1,7);
        if(SQLCODE!=OK){
        	 strcpy(oErrMsg,"查询sHrlCode错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
            return 127076;
        }

        exec sql select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
        where id_no=to_number(:vIdNo);
        if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询dCustSim错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
            return 127077;
        }
    }

     /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetLoginOrgid(iLoginNo,vOrgId);
       if(ret <0)
         	{
				pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败! ");
         	 	return 200919;
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    Sinitn(vOrgCode);
    exec sql select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127078","[%s] ", oErrMsg);
        return 127078;
    }
    Sinitn(vTimeCode);
    Trim(vOrgId);

    exec sql select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
    where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
    if(SQLCODE==OK&&SQLROWS==1)
    {
            if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
            {
                Sinitn((void*)(&userBase));
                /*调用公用函数*/
                init(oErrMsg);
                sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));
                Trim(oErrMsg);
                if( strcmp(oErrMsg, "000000") != 0 )
                {
                    strcpy(oErrMsg,"查询用户信息错误!");
                    pubLog_DBErr(_FFL_, "","127078","%s",oErrMsg);
                    return 127078;
                }
                vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
                pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
                vDeltaDays=0;
                exec sql select days into :vDeltaDays from sPayTime
                where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='1'
                    and begin_money<=:vPrepayFee and end_money>=:vPrepayFee;
                if(SQLCODE!=OK)
                {
                    strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
       				pubLog_DBErr(_FFL_, "","127078","%s",oErrMsg);
                    return 127078;
                }
            }
		/*ng解耦 by wxcrm at 20090812 begin
        exec sql insert into dCustExpire(ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME)
        values(to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'N',to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
            to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays);
        if(SQLCODE!=0)
        {
        	strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
            return 127084;
        }
        ng解耦 by wxcrm at 20090812 end*/
        EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	"N"				,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
            return 127084;
		}


        exec sql insert into dCustExpireHis(
            ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
            UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
        values(
            to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'N',to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
            to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
            to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
        if(SQLCODE!=0)
        {
            strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
            return 127085;
        }
        /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
        exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
        values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
            to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
        if(SQLCODE!=0)
        {
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
            return 127086;
        }
    }

    Sinitn(vModeFlag);Sinitn(vSmCode);init(vFavBrand);
    exec sql select a.mode_flag,a.sm_code,b.fav_brand
    into :vModeFlag,:vSmCode,:vFavBrand from sBillModeCode a,sSmCode b
    where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
        and b.region_code=a.region_code and b.sm_code=a.sm_code;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127060"," [%s] ", oErrMsg);
        return 127060;
    }

    Sinitn(vSelSql);
    strcpy(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
        " from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2");
#ifdef _WTPRN_
    pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vSelSql);
#endif
    exec sql prepare preBeginDet1 from :vSelSql;
    if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127079","[%s]", oErrMsg);

        return 127079;
    }
    exec sql declare curBeginDet11 cursor for preBeginDet1;
    exec sql open curBeginDet11 using :vBelongCode,:iNewMode;
    for(i=0;;){
        Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
        exec sql fetch curBeginDet11
        into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
        if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
            pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
            exec sql close curBeginDet11;
            return 127061;
        }
        if(i!=0&&SQLCODE==NOTFOUND) break;

        /*****************************************************************************
         ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
         ** sBillModeDetail.time_unit 0天/1月/2年                                   **
         ****************************************************************************/
        Sinitn(vBeginTime);
        Sinitn(vEndTime);
        /*switch(vTimeFlag[0]){
            case '0':
                strcpy(vBeginTime,iEffectTime);
                strcpy(vEndTime,MAXENDTIME);
                break;
            case '1':
                strcpy(vBeginTime,iEffectTime);
                pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime);
                break;
            case '2':
                pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime);
                strcpy(vEndTime,MAXENDTIME);
                break;
            default:;
        }*/
        strcpy(vBeginTime,iEffectTime);
        strcpy(vEndTime,iEndTime);
        Trim(vBeginTime);
        Trim(vEndTime);
#ifdef _WTPRN_
       pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode [%s~%s][%s:%s-%s]*%s*"
            ,vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);
#endif

        Sinitn(vBeginSql);
        sprintf(vBeginSql,
            "insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
            " fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
            " login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
            " values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
            " :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
            " :v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
            vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_

        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode%s",vBeginSql);
#endif
        exec sql prepare preWWW_5 from :vBeginSql;
        exec sql execute preWWW_5 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
            :vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
            :iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
        if(SQLCODE!=OK){
            exec sql close curBeginDet11;
            strcpy(oErrMsg,"记录dBillCustDetail的历史错误!");
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
            return 127062;
        }

        Sinitn(vBeginSql);
        #ifdef _WTPRN_
        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);
        /*ng解耦 by wxcrm at 20090812 begin
        sprintf(vBeginSql,
            "insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,"
            " mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code) "
            " values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),:v6,:v7,"
            " :v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),:v13,to_number(:v14),substr(:v15,1,2))",
            vIdNo[strlen(vIdNo)-1]);
		#ifdef _WTPRN_
        pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);

        exec sql prepare preWWW_6 from :vBeginSql;
        exec sql execute preWWW_6 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,:vFavOrder,:iNewMode,
            :vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
        if(SQLCODE!=OK){
            exec sql close curBeginDet1;
            strcpy(oErrMsg,"记录dBillCustDetail错误!");
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127063",oErrMsg);
            return 127063;
        }
		ng解耦 by wxcrm at 20090812 end*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet11;
            strcpy(oErrMsg,"记录dBillCustDetail错误!");
            PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127063",oErrMsg);
            return 127063;
		}

		 switch(vDetailType[0]){
            /* lixg Add: 2005-11-20 begin */
            case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
            case PROD_MACHFEE_CODE: /* 买手机送话费产品 */

                memset(&vGrantMsg, 0, sizeof(vGrantMsg));
                strcpy(vGrantMsg.applyType, vDetailType);
                strcpy(vGrantMsg.phoneNo, iPhoneNo);
                strcpy(vGrantMsg.loginNo, iLoginNo);
                strcpy(vGrantMsg.opCode, iOpCode);
                strcpy(vGrantMsg.opNote, " ");
                strcpy(vGrantMsg.opTime, iOpTime);
                sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
                strcpy(vGrantMsg.modeCode, iNewMode);
                strcpy(vGrantMsg.detModeCode, vDetailCode);
                strcpy(vGrantMsg.machineId, iBunchNo);
                strcpy(vGrantMsg.loginAccept, iLoginAccept);
                pubLog_Debug(_FFL_,"","","product Begin ..... ");
                if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 )
                {
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
                    return iProductRetCode;
                }
                break;
            /* lixg Add: 2005-11-20 end */

            case '0': /** process bill system data              **/
                init(vVpmnGroup);
                exec sql select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate from sBillVpmnCond
                where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
                if(SQLROWS==1)
                {
                    Trim(vVpmnGroup);
                    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                    /*ng解耦 by wxcrm at 20090812 begin
                    exec sql insert into dBaseFav(
                        msisdn,fav_brand,fav_type,flag_code,fav_order,
                        fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
                    values(
                        :iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
                        to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),substr(:vBelongCode,1,2),:vIdNo, :vGroupId ,:iNewMode);
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
                    }
                    ng解耦 by wxcrm at 20090812 end*/

					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
					}


                    #ifdef _CHGTABLE_
                    exec sql insert into wBaseFavChg(
                        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                        start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
                    values(
                        :iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
                        to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
                        '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId ,:iNewMode);
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127083", oErrMsg);
                        return 127083;
                    }
                    #endif
					/*ng解耦 by wxcrm at 20090812 begin
                    exec sql insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
                    values(:iPhoneNo,:vVpmnGroup,:vMocRate,:vMtcRate,
                        to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
                    }
                    ng解耦 by wxcrm at 20090812 end*/
                    sprintf(vMocRateStr,"%f",vMocRate);
					sprintf(vMtcRateStr,"%f",vMtcRate);
                    memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
                    strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127082", oErrMsg);
                        return 127082;
					}
                    #ifdef _CHGTABLE_
                    exec sql insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
                    values(:iPhoneNo,:vVpmnGroup,:vMocRate,:vMtcRate,
                        to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
                        '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
                    if(SQLCODE!=OK||SQLROWS!=1)
                    {
                    	sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
                   		 PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127083", oErrMsg);
                        return 127083;
                    }
                    #endif
                }

                if(iFlagStr[0]=='\0'){
                    Sinitn(vFather);
                    sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
                }
                else
				{
                    strcpy(vFather,iFlagStr);
				}
                for(;;){
                    pubApartStr(vFather,':',vUncle,vSon);
                    if(vSon[0]==vDetailCode[0]&&
                       vSon[1]==vDetailCode[1]&&
                       vSon[2]==vDetailCode[2]&&
                       vSon[3]==vDetailCode[3]){
                        pubApartStr(vSon,'^',vFather,vUncle);
                        for(;;){
                            pubApartStr(vFather,'^',vUncle,vSon);

                            if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");
                         /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                            /*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dBaseFav(
                                msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                                start_time,end_time,region_code,id_no,group_id,product_code)
                            select
                                :iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
                                :vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
                                substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
                            from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
                            if(SQLCODE!=OK||SQLROWS!=1){
                                exec sql close curBeginDet1;
                                strcpy(oErrMsg,"记录dBaseFav错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);

                                return 127064;
                            }
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);

							/*
							EXEC SQL SELECT
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
                                :vFavDay,:vBeginTime,:vEndTime,
                                substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
                            	INTO :vTdBaseFav.sMsisdn,:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet1;
                                strcpy(oErrMsg,"初始化结构体错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                return 127064;
							}
							*/
							EXEC SQL SELECT count(*)
									INTO :iNum2
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum2!=1){
								strcpy(oErrMsg,"取sBillRateCode记录错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                exec sql close curBeginDet11;
                                return 127064;
							}
							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	iFlagCode	);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDet11;
                                strcpy(oErrMsg,"记录dBaseFav错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
                                return 127064;
							}
							#ifdef _CHGTABLE_
                            exec sql insert into wBaseFavChg(
                                msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
                                start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
                            select
                                :iPhoneNo,:vSmCode,:vDetailCode,:iFlagCode,to_char(:vFavOrder),
                                :vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
                                to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
                                    to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
                            from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
                            if(SQLCODE!=OK||SQLROWS!=1){
                                exec sql close curBeginDet11;
                                strcpy(oErrMsg,"记录wBaseFavChg错误!");
                                PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127065"," [%s] ", oErrMsg);

                                return 127065;
                            }
							#endif
                            strcpy(vFather,vUncle);
                            if(vFather[0]==0) break;
                        }
                        break;
                    }
                    strcpy(vFather,vUncle);
                    if(vFather[0]=='\0') break;
                }
            case '1':case '2': case '3':case '4':case 'a':
            	#ifdef _CHGTABLE_
                exec sql insert into wUserFavChg(
                    op_no,op_type,op_time,id_no,detail_type,detail_code,
                    begin_time,end_time,fav_order,mode_code)
                values(
                    sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
                    to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
                if(SQLCODE!=OK){
                    exec sql close curBeginDet11;
                    strcpy(oErrMsg,"记录wUseFavChg错误!");
                    PUBLOG_DBDEBUG("pubBillBeginTimeMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);

                    return 127066;
                }
                #endif

                if(vDetailType[0]!='a') break;
                /** 处理特服绑定的情况 ***/


                vSendFlag=strcmp(vBeginTime,iOpTime);
                Sinitn(vFunctionType);Sinitn(vFunctionCode);
                Sinitn(vOffOn);Sinitn(vCommandCode);
                exec sql select a.function_code,a.off_on,b.command_code,b.function_type
                    into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
                from sBillFuncBind a,sFuncList b
                where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
                    and b.region_code=a.region_code and b.sm_code=a.sm_code
                    and b.function_code=a.function_code;

				if(SQLCODE!=OK){
                	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
					pubLog_DBErr(_FFL_, "","127067"," .pubBillBeginTimeMode :select sFuncList error ");
                    return 127067;
                }

                if(vSendFlag>0)/***预约 开/关***/
                {
                    exec sql select count(*) into :vUserFunc from wCustExpire
                    where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
                    and business_status=:vOffOn;
                    if(vUserFunc==0)
                    {
                    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                        exec sql insert into wCustExpire(
                            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,org_id)
                        values(
                            sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
                            :vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
                            to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
                        if(SQLCODE!=OK){
							PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127068"," .pubBillBeginTimeMode :insert wCustExpire error");
                            return 127068;
                        }

                        exec sql insert into wCustExpireHis(
                            command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,org_id)
                        select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
                            total_date,op_code,op_time,function_code,command_code,business_status,
                            expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
                        from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
                        if(SQLCODE!=OK||SQLROWS!=1){
                        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127069"," .pubBillBeginTimeMode :insert wCustExpireHis error");
                            return 127069;
                        }
                    }
                }
                else{
                    exec sql select count(*) into :vUserFunc from dCustFunc
                    where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

                    if(vOffOn[0]=='1'){ /** 立即开 ***/
                        if(vUserFunc==0){
                        	/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFuncHis(
                                id_no,function_type,function_code,op_time,
                                update_login,update_accept,update_date,update_time,update_code,update_type)
                            values(
                                to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
                                :iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127070"," .pubBillBeginTimeMode :insert dCustFuncHis(I) error");
                                return 127070;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            Sinitn(vTotalDateStr);
                            sprintf(vTotalDateStr,"%d",vTotalDate);
                            strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127070"," .pubBillBeginTimeMode :insert dCustFuncHis(I) error");
                                return 127070;
							}

							/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFunc(id_no,function_type,function_code,op_time)
                            values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127071"," .pubBillBeginTimeMode :insert dCustFunc error");
                                return 127071;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127071"," .pubBillBeginTimeMode :insert dCustFunc error");
                                return 127071;
							}
                        }
                    }
                    else{  /** 立即关 **/
                        if(vUserFunc!=0){
                        	/*ng解耦 by wxcrm at 20090812 begin
                            exec sql insert into dCustFuncHis(
                                id_no,function_type,function_code,op_time,
                                update_login,update_accept,update_date,update_time,update_code,update_type)
                            select
                                id_no,function_type,function_code,op_time,
                                :iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
                            from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :insert dCustFuncHis(D) error ");
                                return 127072;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDate,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr6 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur61 CURSOR FOR ng_SqlStr6;
							EXEC SQL OPEN ng_Cur61 using :iLoginNo,:iLoginAccept,:vTotalDate,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur61 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :select dCustFunc error ");
                                	EXEC SQL Close ng_Cur61;
									return 127072;
								}
								Trim(vTdCustFuncHis.sIdNo);
								Trim(vTdCustFuncHis.sFunctionType);
								Trim(vTdCustFuncHis.sFunctionCode);
								Trim(vTdCustFuncHis.sOpTime);
								Trim(vTdCustFuncHis.sUpdateLogin);
								Trim(vTdCustFuncHis.sUpdateAccept);
								Trim(vTdCustFuncHis.sUpdateDate);
								Trim(vTdCustFuncHis.sUpdateTime);
								Trim(vTdCustFuncHis.sUpdateCode);
								Trim(vTdCustFuncHis.sUpdateType);
								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127072"," .pubBillBeginTimeMode :insert dCustFuncHis(D) error ");
                                	EXEC SQL Close ng_Cur61;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur61;

							/*ng解耦 by wxcrm at 20090812 begin
                            exec sql delete dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
                            if(SQLCODE!=OK){
                            	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
                                return 127073;
                            }
                            ng解耦 by wxcrm at 20090812 end*/
                            memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur101 CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur101 using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur101 INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127073","select dCustFunc error");
									EXEC SQL Close ng_Cur101;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillBeginTimeMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubBillBeginTimeMode");
									pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
									EXEC SQL Close ng_Cur101;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur101;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubBillBeginTimeMode");
								pubLog_DBErr(_FFL_, "","127073","delete dCustFunc error");
                                return 127073;
							}
                        }
                    }

                    exec sql insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
                        op_code,login_no,login_accept)
                    values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
                        to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
                    if(SQLCODE!=OK){
                    	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
						pubLog_DBErr(_FFL_, "","127074"," .pubBillBeginTimeMode :insert wCustFuncDay error");
                        return 127074;
                    }

                    if(iOpCode[0]!='8')/*** 非专线用户在立即生效时发送开关机命令 ***/
                    {
                    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
                    	/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
											if(strcmp(vFunctionCode,"02")==0)
											{
												EXEC SQL SELECT COUNT(1)
																	into : iCount
																	FROM dcustfunc a
																		WHERE a.id_no = :vIdNo
																			AND a.function_code = '87';
												if(SQLCODE != 0)
												{
													PUBLOG_DBDEBUG("");
													pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
													return 121993;
												}	
												if(iCount == 0)
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1");
													}
													else
													{
														strcpy(vNewCmdStr,"N");
													}
												}
												else
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1&BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"BOS3");
													}
												}
												EXEC SQL insert into wSndCmdDay(
													command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
													new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
													org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
													values(
													sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
													:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
													:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
												if (SQLCODE != OK){
													PUBLOG_DBDEBUG("pubBillBeginMode");
													pubLog_DBErr(_FFL_, "","127075","");
													return 127075;
												}
											}
											if(strcmp(vFunctionCode,"87")==0)
											{
												EXEC SQL SELECT COUNT(1)
																	into : iCount
																	FROM dcustfunc a
																		WHERE id_no = :vIdNo
																			AND function_code = '02';
												if(SQLCODE != 0)
												{
													PUBLOG_DBDEBUG("");
													pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
													return 121993;
												}	
												if(iCount == 0)
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"N");
													}
												}
												else
												{
													if(strcmp(vOffOn,"1")==0)
													{
														strcpy(vNewCmdStr,"BOS1&BOS3");
													}
													else
													{
														strcpy(vNewCmdStr,"BOS1");
													}
												}
												EXEC SQL insert into wSndCmdDay(
													command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
													new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
													org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
													values(
													sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
													:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
													:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
												if (SQLCODE != OK){
													PUBLOG_DBDEBUG("pubBillBeginMode");
													pubLog_DBErr(_FFL_, "","127075","");
													return 127075;
												}
											}
											/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
                    	else
                    	{
                       exec sql insert into wSndCmdDay(
                            command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
                            new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
                            org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
                        values(
                            sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
                            :iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
                            :vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOrgId, :vGroupId);

                        if(SQLCODE!=OK){
                        	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
							pubLog_DBErr(_FFL_, "","127075"," .pubBillBeginTimeMode :insert wSndSmdDay error");
                            return 127075;
                        }
                      }
                    }

            }
            default:;
        }
    i++;
    }
    exec sql close curBeginDet11;

		strncpy(vRegionCode, vBelongCode, 2);

/* 集团上线暂时注释下面代码 */
#if 0
		/* 开始处理服务代码 chenxuan boss3 */
    Sinitn(vSelSql);
    strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
    	"from sBillModeSrv a, sSrvCmdRelat b "
    	"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
    	"order by b.cmd_order");
    exec sql prepare pre1 from :vSelSql;
    exec sql declare cur1 cursor for pre1;
    exec sql open cur1 using :vBelongCode, :iNewMode;
    for (;;) {
    	init(vSrvCode); init(vSrvNetType); init(vCmdCode);
      exec sql fetch cur1 into :vSrvCode, :vSrvNetType, :vCmdCode;
      if (SQLCODE == 1403) break;
      Trim(vCmdCode);

      /* 插入用户使用服务表 */
      exec sql insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
      		mode_code, login_accept)
      	values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
      		to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
      if (SQLCODE != 0) {
      	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127070","insert dBillCustSrv error");
      	return 127070;
      }

			/* 插入接口相关表 */
      exec sql insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
      		cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
      	values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
      		:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
      if (SQLCODE != 0) {
      	PUBLOG_DBDEBUG("pubBillBeginTimeMode");
		pubLog_DBErr(_FFL_, "","127071","insert wSndSrvDay error");
      	return 127071;
      }
    }
    exec sql close cur1;
		/* 处理服务代码结束 chenxuan boss3 */
#endif

		/* 调用指令分发函数 chenxuan boss3 */
		strcpy(tPubBillInfo.IdNo,        vIdNo);
		strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
		strcpy(tPubBillInfo.BelongCode,  vBelongCode);
		strcpy(tPubBillInfo.SmCode,      vSmCode);
		strcpy(tPubBillInfo.ModeCode,    iNewMode);
		strcpy(tPubBillInfo.OpCode,      iOpCode);
		strcpy(tPubBillInfo.OpTime,      iOpTime);
		strcpy(tPubBillInfo.EffectTime,  iEffectTime);
		tPubBillInfo.TotalDate = vTotalDate;
		strcpy(tPubBillInfo.LoginNo,     iLoginNo);
		strcpy(tPubBillInfo.LoginAccept, iLoginAccept);

		iProductRetCode = fDispatchCmd("1", &tPubBillInfo, oErrMsg);
		if (iProductRetCode != 0) {
			pubLog_DBErr(_FFL_, "","127071","");
			return 127072;
		}
		/* 指令分发函数结束 chenxuan boss3 */

		/* 开始产品订购数量限制 chenxuan boss3 */
		init(vBeginTime);
		init(vEndTime);
		init(flag);
    EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
    	INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
    	from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
    		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
    	EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127073"," [%s]", oErrMsg);
			return 127073;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127074"," .pubBillBeginTimeMode :select max_num error[%s]", oErrMsg);
			return 127074;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127075","[%s]", oErrMsg);
			return 127075;
	    }
    }
    /* 产品订购数量限制结束 chenxuan boss3 */

    /*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			  INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			  from sBillModeNum
			 where region_code = '00' and mode_code = :iNewMode
		       and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = '00' and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, " 插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127076","[%s]", oErrMsg);

			return 127076;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127077","[%s]", oErrMsg);
			return 127077;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubBillBeginTimeMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]", oErrMsg);
			return 127078;
	    }
	}
	/*增加全省统一数量控制功能 20081119  end*/
	
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}
	
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubBillBeginTimeMode end -----");
#endif
    return 0;
}


/**********************************************
 @wt  PRG  : pubSladderBillEndMode
 @wt  FUNC : 取消iPhoneNo的iOldMode资费 阶梯套餐绑定
 @wt       : 其中生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt *********************************************
 @wt 0 iPhoneNo      移动号码
 @wt 1 iOldMode      要取消的套餐代码
 @wt 2 iOldAccept    要取消套餐的申请流水
 @wt 3 iEffectTime   生效时间
 @wt 4 iOpCode       业务代码
 @wt 5 iLoginNo      工号
 @wt 6 iLoginAccept  业务流水
 @wt 7 iOpTime       业务时间
 @wt 8 oErrMsg       函数内部错误
 @wt *********************************************
 @wt  数据结果
 @wt      dBillCustDetailX (Deteted/updated)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(Updated) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 *********************************************/
int pubSladderBillEndMode(
	char *iPhoneNo,
	char *iSell_code,
	char *iOldMode,
	char *iOldAccept,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *oErrMsg)
{
	/*boss2.0 lixg add: 20070118 begin */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	/*boss2.0 lixg add: 20070118 end */

	EXEC SQL BEGIN DECLARE SECTION;
		char vECtrlCode[5+1];
		char vIdNo[23];
		int  i=0,t=0,vTotalDate=0,vFavOrder=0,vBeginFlag=0,vEndFlag=0,vAccountOpType=0,vFuncBindFlag=-1,vDeltaDays=-1,vnum=0;
		char vBelongCode[8],vDetailType[2],vDetailCode[5],vBeginTime[18],vEndTime[18];
		char vEndSql[1024],vSelSql[1024],vModeTime[2],vSmCode[3],vOrgCode[10];
		char vBindOffonFlag[2],vFuncBindCode[3],vVpmnGroup[11];
		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];

		/* lixg add for Product provide begin */
		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		int  retValue=0;
		/* lixg add for Product provide end */

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		TPubBillInfo tPubBillInfo;
		int vMaxNum, vCurNum, vBuyNum;

		/*ng 解耦*/
		int ret = 0;
		int vCount = 0;
		char    dynStmt[1024];
		char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char 	v_update_sql[500+1];
		char	vTotalDateStr[8+1];
		TdBaseFavIndex			vTdBaseFavIndex;
		TdBaseVpmnIndex			vTdBaseVpmnIndex;
		TdBillCustDetailIndex	vTdBillCustDetailIndex;
	EXEC SQL END   DECLARE SECTION;

	memset(v_update_sql, 0, sizeof(v_update_sql));
	memset(dynStmt, 0, sizeof(dynStmt));
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
	memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
	memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
	memset(vTotalDateStr, 0, sizeof(vTotalDateStr));

	Trim(iPhoneNo);
	Trim(iOldMode);
	Trim(iOldAccept);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);


	oErrMsg[0] = '\0';
	/*init(oErrMsg);*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode begin -----");
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iPhoneNo=[%s] -----",iPhoneNo);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iOldMode=[%s] -----",iOldMode);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iOldAccept=[%s] -----",iOldAccept);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iEffectTime=[%s] -----",iEffectTime);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iOpCode=[%s] -----",iOpCode);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iLoginNo=[%s] -----",iLoginNo);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iLoginAccept=[%s] -----",iLoginAccept);
	pubLog_Debug(_FFL_, "", "", "pubSladderBillEndMode iOpTime=[%s] -----",iOpTime);
#endif

	Sinitn(vIdNo);Sinitn(vBelongCode);

	EXEC SQL select to_char(id_no),belong_code,sm_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vSmCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if (SQLCODE != OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		pubLog_DBErr(_FFL_, "", "1024", "pubSladderBillEndMode :select dCustMsg error[%s]",oErrMsg);
		return 1024;
	}

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if (SQLCODE != OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		pubLog_DBErr(_FFL_, "", "127078", "select dLoginMsg error[%s]",oErrMsg);
		return 127078;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	init(vECtrlCode);
	/* boss2.0 lixg add: 20070408 增加产品后项控制 */

	EXEC SQL select nvl(end_ctrl_code,' ') into :vECtrlCode
		from sBillModeCode
		where region_code = substr(:vBelongCode,1,2)
		and mode_code = :iOldMode;
	if(SQLCODE != 0){
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		sprintf(oErrMsg,"用户[%s]没有定购产品[%s]!", iPhoneNo, iOldMode);
#ifdef _DEBUG_
	pubLog_DBErr(_FFL_, "", "1023", "ECtrlCheck Error [%s] [%s] [%s]", iOldMode, iPhoneNo,oErrMsg);
#endif
		return 1023;
	}

	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "begin to fProdCheckOpr --ctrlCode=[%s]---", vECtrlCode);

	Trim(vECtrlCode);

	if (strlen(vECtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vECtrlCode, iOpCode, iLoginNo, &fe) != 0){
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, "", "atoi(tmpRetCode)","");
			return atoi(tmpRetCode);
		}
	}


	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "end to fProdCheckOpr -----");
#endif


	EXEC SQL select days into :vDeltaDays from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iOldMode and open_flag='0';
	if (SQLCODE==OK||SQLROWS==1){
		EXEC SQL insert into dCustExpireHis(
			ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
			UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
			select
			ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
			to_number(:iLoginAccept),to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'d',:iOpCode,:vTotalDate
			from dCustExpire where id_no=to_number(:vIdNo);
		if (SQLCODE!=0&&SQLCODE!=1403){
			strcpy(oErrMsg,"删除用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "", "127088", "pubSladderBillEndMode ",SQLCODE);
			pubLog_DBErr(_FFL_, "", "127088", "pubSladderBillEndMode [%s]",oErrMsg);
			return 127088;
		}

		memset(v_parameter_array, 0, sizeof(v_parameter_array));
		strcpy(v_parameter_array[0],vIdNo);
		ret = 0;
		ret = OrderDeleteCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillEndMode",vIdNo,"",v_parameter_array);
		if (ret < 0)
		{
			strcpy(oErrMsg,"删除用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "", "127087", oErrMsg);
			return 127087;
		}

}
	Trim(vIdNo);



	Sinitn(vSelSql);
	sprintf(vSelSql,"select detail_type,detail_code,to_char(begin_time,'yyyymmdd hh24:mi:ss'),fav_order,"
		" to_char(end_time,'yyyymmdd hh24:mi:ss'),mode_time from dBillCustDetail%c where id_no=to_number(:v1) "
		" and mode_code=:v2 and login_accept=to_number(:v3) order by mode_time desc",vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "pubSladderBillEndMode %s",vSelSql);
	printf("vIdNo  %s  iOldMode  %s  iOldAccept  %s " ,vIdNo,iOldMode,iOldAccept);
#endif
	EXEC SQL PREPARE preModeDet FROM :vSelSql;
	EXEC SQL DECLARE curModeDet1 CURSOR FOR preModeDet;
	EXEC SQL OPEN curModeDet1 USING :vIdNo,:iOldMode,:iOldAccept;
	if(SQLCODE!=OK)
	{
		strcpy(oErrMsg,"查询dBillCustDetail初始化错误");
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		pubLog_DBErr(_FFL_, "", "127040", "pubSladderBillEndMode :open (select dBillModeDetail) error[%s]",oErrMsg);
		return 127040;
	}
	for(i=0;;)
	{
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vBeginTime);Sinitn(vEndTime);
		EXEC SQL FETCH curModeDet1 INTO :vDetailType,:vDetailCode,:vBeginTime,:vFavOrder,:vEndTime,:vModeTime;

		printf(" 查询 dBillCustDetail  :vDetailType  [%s],:vDetailCode[%s],:vBeginTime[%s],:vFavOrder[%d],:vEndTime[%s],:vModeTime [%s] ",vDetailType,vDetailCode,vBeginTime,vFavOrder,vEndTime,vModeTime);


	/*	if((SQLCODE!=OK&&SQLCODE!=NOTFOUND)||(SQLCODE==NOTFOUND&&i==0))*/
		if((SQLCODE!=OK&&SQLCODE!=NOTFOUND))
		{
			EXEC SQL CLOSE curModeDet1;
			strcpy(oErrMsg,"查询dBillCustDetail明细错误");
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "", "127041", "pubSladderBillEndMode :fetch (select dBillModeDetail) error[%s]  SQLCODE [%d]"   ,oErrMsg,SQLCODE);
			return 127041;
		}else if(SQLCODE ==1403)
			{
				break;
			}
		if(SQLCODE==NOTFOUND&&i>0) break;

		if (strcmp(vModeTime,"Y")==0){
			/*** vBeginFlag 记录生效时间与要取消套餐的开始时间比较后的关系 ***/
			vBeginFlag = strcmp(iEffectTime,vBeginTime);
		}
		/*** vEndFlag 记录生效时间与要取消套餐的结束时间比较后的关系 ***/
		vEndFlag=strcmp(vEndTime,iEffectTime);

		if (i==0){
			retValue=DcustModeChgAwake(iOpCode,iOldMode,vBelongCode,"d",atol(iOldAccept),iPhoneNo,iLoginNo,iEffectTime,vEndTime);
			if (retValue < 0){
				EXEC SQL CLOSE curModeDet1;
				strcpy(oErrMsg,"发送提醒短信错误!");
				PUBLOG_DBDEBUG("pubSladderBillEndMode");
				pubLog_DBErr(_FFL_, "", "127099", "pubSladderBillBeginMode :insert dBillCustDetail error[%s]",oErrMsg);
				return 127099;
			}
		}

		if(vBeginFlag <= 0)
		{
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'d',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10 ",
				vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "pubSladderBillEndMode :%s",vEndSql);
#endif
			EXEC SQL PREPARE preWWW_1 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_1 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,
				:iLoginAccept,:vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				EXEC SQL CLOSE curModeDet1;
				strcpy(oErrMsg,"删除dBillCustDetail时记录历史明细错误!");
				PUBLOG_DBDEBUG("pubSladderBillEndMode");
				pubLog_DBErr(_FFL_, "", "127042", "[%s][%s]",SQLROWS,oErrMsg);
				return 127042;
			}

			Sinitn(vEndSql);

			memset(dynStmt, 0, sizeof(dynStmt));
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
			memset(v_parameter_array, 0, sizeof(v_parameter_array));

			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_parameter_array[0],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[1],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[2],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[3],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[4],vTdBillCustDetailIndex.sDetailCode);

			ret = 0;
			ret = OrderDeleteBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillEndMode",vTdBillCustDetailIndex,"",v_parameter_array);
			if (ret != 0)
			{
				exec sql close curModeDet1;
			    strcpy(oErrMsg,"删除dBillCustDetail无效记录时错误!");
			    PUBLOG_DBDEBUG("pubSladderBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127043", "[%s]",oErrMsg);
			    return 127043;
			}
		}
		else {
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'u',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10",vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "pubSladderBillEndMode :%s",vEndSql);
printf("  查询dBillCustDetail 插入dBillCustDetailHIS :iOpCode %s \n,:vTotalDate %d \n,:iOpTime %s\n,:iLoginNo %s\n,:iLoginAccept %s\n,:vIdNo %s\n,:iOldMode %s \n,:iOldAccept %s\n,:vDetailType %s\n,:vDetailCode %s\n", iOpCode,vTotalDate,iOpTime,iLoginNo,iLoginAccept,vIdNo,iOldMode,iOldAccept,vDetailType,vDetailCode);

#endif
			EXEC SQL PREPARE preWWW_3 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_3 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vIdNo,
				:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				EXEC SQL CLOSE curModeDet1;
				strcpy(oErrMsg,"更改dBillCustDetail时记录历史明细错误!");
				 PUBLOG_DBDEBUG("pubSladderBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127044", "  oErrMsg[%s]   SQLCODE  [%d] ",oErrMsg,SQLCODE);
				return 127044;
			}
			Sinitn(vEndSql);
			memset(dynStmt, 0, sizeof(dynStmt));
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));

			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			memset(v_parameter_array, 0, sizeof(v_parameter_array));
			sprintf(vTotalDateStr,"%d",vTotalDate);

			strcpy(v_parameter_array[0],iEffectTime);
			strcpy(v_parameter_array[1],iEffectTime);
			strcpy(v_parameter_array[2],iOpCode);
			strcpy(v_parameter_array[3],vTotalDateStr);
			strcpy(v_parameter_array[4],iOpTime);
			strcpy(v_parameter_array[5],iLoginNo);
			strcpy(v_parameter_array[6],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[7],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[8],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[9],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[10],vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_update_sql,"mode_status='N',end_time=decode(sign(end_time-to_date(:v20,'YYYYMMDD HH24:MI:SS')),1,to_date(:v21,'YYYYMMDD HH24:MI:SS'),end_time), op_code=:v22,total_date=:v23,op_time=to_date(:v24,'yyyymmdd hh24:mi:ss'),login_no=:v25 ");
			Trim(v_update_sql);

			ret = 0;
			ret = OrderUpdateBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillEndMode",vTdBillCustDetailIndex,vTdBillCustDetailIndex,v_update_sql,"",v_parameter_array);

			if (ret != 0)
			{
				exec sql close curModeDet1;
			 	PUBLOG_DBDEBUG("pubSladderBillEndMode");
			    pubLog_DBErr(_FFL_, "", "127045", "[%s]",oErrMsg);
				strcpy(oErrMsg,"更改dBillCustDetail错误!");
				return 127045;
			}

#ifdef _WTPRN_
			pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "pubSladderBillEndMode :update(for/u)dBillCustDetails%c%s",vIdNo[strlen(vIdNo)-1],vEndSql);
#endif
		}

		switch(vDetailType[0])
		{
			/*  zhengbin */
			case '0':
				/*** process bill system data begin ***/
				if (vEndFlag > 0)
				{
					printf("\n 开始取消阶梯套餐 优惠表数据\n" );
					vCount=0;
					EXEC SQL select  count(*)
					into vCount
					from sladderfav
					where region_code =substr(:vBelongCode,0,2)
					and  sell_code  = :iSell_code
					and  mode_code  = :iOldMode
					and  detail_code =:vDetailCode;
					if(SQLCODE !=0)
					{
						sprintf(oErrMsg,"取阶梯套餐信息失败! SQLCODE = [%d]",SQLCODE);
						PUBLOG_DBDEBUG("pubSladderBillBeginMode");
						pubLog_DBErr(_FFL_, "","998616","[%s] ", oErrMsg);
						return 998516;
					}
					if(vCount == 0)
					{
						sprintf(oErrMsg,"取阶梯套餐信息失败! SQLCODE = [%d]",SQLCODE);
						PUBLOG_DBDEBUG("pubSladderBillBeginMode");
						pubLog_DBErr(_FFL_, "","998616","[%s] ", oErrMsg);
						return 998316;
					}
					Trim(vDetailCode);
					printf("\n vDetailCode   [%s]  vBeginTime [%s]\n",vDetailCode,vBeginTime);

					EXEC SQL SELECT msisdn, fav_type, flag_code, to_char(start_time,'YYYYMMDD HH24:MI:SS')
							 INTO :vTdBaseFavIndex.sMsisdn,:vTdBaseFavIndex.sFavType,:vTdBaseFavIndex.sFlagCode,:vTdBaseFavIndex.sStartTime
							 FROM dBaseFav
							 WHERE msisdn=:iPhoneNo and fav_type=:vDetailCode
							 AND start_time >=TO_DATE(:vBeginTime,'YYYYMMDD HH24:MI:SS');

					if(SQLCODE!=OK||SQLROWS!=1)
					{

						PUBLOG_DBDEBUG("pubSladderBillEndMode");
						sprintf(oErrMsg,"查询dBaseFav错误![%s][%s]",vDetailCode,vBeginTime);
						pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}
					memset(v_parameter_array, 0, sizeof(v_parameter_array));
					strcpy(v_parameter_array[0],iEffectTime);
					strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
					strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
					strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
					strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);
					ret = 0;
					ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:v11,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
					if (ret != 0)
					{

						PUBLOG_DBDEBUG("pubSladderBillEndMode");
						strcpy(oErrMsg,"更改dBaseFav错误!");
						pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}

	#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
						select
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
						region_code,id_no,group_id,product_code
						from dBaseFav where msisdn=:iPhoneNo and fav_type=:vDetailCode
						and start_time >=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					if(SQLCODE!=OK||SQLROWS!=1)
					{
						PUBLOG_DBDEBUG("pubSladderBillEndMode");
						pubLog_DBErr(_FFL_, "", "127047", "[%s]",oErrMsg);
						strcpy(oErrMsg,"记录wBaseFavChg错误!");
						return 127047;
					}
	#endif
					memset(dynStmt, 0, sizeof(dynStmt));
					sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
									"FROM dBaseFav "
									"WHERE msisdn=:v1 and fav_type=:v2 and start_time>=end_time");
					EXEC SQL PREPARE ng_SqlStr12 FROM :dynStmt;
					EXEC SQL DECLARE ng_Cur12 CURSOR FOR ng_SqlStr12;
					EXEC SQL OPEN ng_Cur12 using :iPhoneNo,:vDetailCode;
					for(i=0;;)
					{
						memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
						EXEC SQL FETCH ng_Cur12 INTO :vTdBaseFavIndex;
						if (SQLCODE == 1403) break;
						if (SQLCODE != 1403 && SQLCODE != 0)
						{
							strcpy(oErrMsg,"查询dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur12;
							return 127049;
						}
						memset(v_parameter_array, 0, sizeof(v_parameter_array));
						strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
						ret = 0;
						ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
						if (ret != 0)
						{
							strcpy(oErrMsg,"清除dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur12;
							return 127049;
						}
					}
					EXEC SQL CLOSE ng_Cur12;

					/*** 处理聊天套餐默认退出vmpn集团 ***/
					init(vVpmnGroup);
					EXEC SQL select group_index into :vVpmnGroup from sBillVpmnCond
							where region_code=substr(:vBelongCode,1,2) and mode_code=:iOldMode;
					if(SQLROWS==1)
					{
						Trim(vVpmnGroup);
#ifdef _CHGTABLE_
						exec sql insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
						select msisdn,groupid,mocrate,mtcrate,valid,
						to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'2',to_date(:iOpTime,'yyyymmdd hh24:mi:ss')
						from dBaseVpmn where msisdn=:iPhoneNo and groupid=:vVpmnGroup
						and valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
						if(SQLCODE!=OK||SQLROWS!=1)
						{
							sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127081", " [%s]", oErrMsg);
							return 127081;
						}
#endif
						EXEC SQL SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS')
								 INTO :vTdBaseVpmnIndex
								 FROM dBaseVpmn
								 WHERE msisdn=:iPhoneNo
								 AND groupid=:vVpmnGroup
								 AND valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
						if(SQLCODE!=OK||SQLROWS!=1)
						{
							sprintf(oErrMsg,"处理询dBaseVpmn默认集团%s时错误!",vVpmnGroup);
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
							return 127080;
						}

						Trim(vTdBaseVpmnIndex.sMsisdn);
						Trim(vTdBaseVpmnIndex.sGroupId);
						Trim(vTdBaseVpmnIndex.sValid);

						memset(v_parameter_array, 0, sizeof(v_parameter_array));
						strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sGroupId);
						strcpy(v_parameter_array[3],vTdBaseVpmnIndex.sValid);

						ret = 0;
						ret = OrderUpdateBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,vTdBaseVpmnIndex,"invalid=to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')","",v_parameter_array);
						if (ret != 0)
						{
							sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
							return 127080;
						}

						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseVpmn "
										 "WHERE msisdn=:iPhoneNo and groupid=:vVpmnGroup and valid>=invalid");
						EXEC SQL PREPARE ng_SqlStr7 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur7 CURSOR FOR ng_SqlStr7;
						EXEC SQL OPEN ng_Cur7 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
						{
							memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
							EXEC SQL FETCH ng_Cur7 INTO :vTdBaseVpmnIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}

							Trim(vTdBaseVpmnIndex.sMsisdn);
							Trim(vTdBaseVpmnIndex.sGroupId);
							Trim(vTdBaseVpmnIndex.sValid);

							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseVpmnIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sGroupId);
							strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sValid);
							ret = 0;
							/*******************ng解耦 liuzhou 只删除dbasevpmn表中记录，不插wBaseVpmnChg  *****************/
							/*ret = OrderDeleteBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);*/
							ret = OrderDeleteOtherBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur7;

						memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
						strncpy(vTdBaseFavIndex.sMsisdn		,iPhoneNo	,	15	);
						strncpy(vTdBaseFavIndex.sFavType	,"aa00"		,	4	);
						strncpy(vTdBaseFavIndex.sFlagCode	,vVpmnGroup	,	10	);
						strncpy(vTdBaseFavIndex.sStartTime	,vBeginTime	,	17	);

						Trim(vTdBaseFavIndex.sMsisdn);
						Trim(vTdBaseFavIndex.sFavType);
						Trim(vTdBaseFavIndex.sFlagCode);
						Trim(vTdBaseFavIndex.sStartTime);

						memset(v_parameter_array, 0, sizeof(v_parameter_array));
						strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);

						ret = 0;
						ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
						if (ret != 0)
						{
							sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
							return 127080;
						}
#ifdef _CHGTABLE_
						exec sql insert into wBaseFavChg(
						    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						    start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
						select
						    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						    start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,
						    id_no,group_id,product_code
						from dBaseFav where msisdn=:iPhoneNo and fav_type='aa00' and flag_code=:vVpmnGroup
						    and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
						if(SQLCODE!=OK)
						{
							sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
							PUBLOG_DBDEBUG("pubSladderBillEndMode");
							pubLog_DBErr(_FFL_, "", "127081", "[%s]", oErrMsg);
							return 127081;
						}
#endif
						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseFav "
										 "WHERE msisdn=:v1 and fav_type='aa00' and flag_code=:v2 and start_time>=end_time");
						EXEC SQL PREPARE ng_SqlStr14 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur14 CURSOR FOR ng_SqlStr14;
						EXEC SQL OPEN ng_Cur14 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
						{
							memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
							EXEC SQL FETCH ng_Cur14 INTO :vTdBaseFavIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}
							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
							strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
							strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
							ret = 0;
							ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubBillEndMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur14;
					}
				}
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case 'a':
				/*** process account system data ***/
				if(vBeginFlag<=0)
					vAccountOpType=3;
				else
					vAccountOpType=2;

				if(vEndFlag >= 0)
				{
#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
						op_no,op_type,op_time,
						id_no,detail_type,detail_code,begin_time,
						end_time,fav_order,mode_code)
					values(
						sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE != OK){
						exec sql close curModeDet1;
						strcpy(oErrMsg,"记录wUserFavChg时错误!");
						PUBLOG_DBDEBUG("pubSladderBillEndMode");
						pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
						return 127048;
					}
#endif
				}
				else
				{
#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
					    op_no,op_type,op_time,
					    id_no,detail_type,detail_code,begin_time,
					    end_time,fav_order,mode_code)
					values(
					    sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
					    to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
					    to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE!=OK)
					{
						exec sql close curModeDet1;
						strcpy(oErrMsg,"记录wUserFavChg时错误!");
						PUBLOG_DBDEBUG("pubSladderBillEndMode");
						pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
						return 127048;
					}
#endif
				}

				if(vDetailType[0]!='a')
					break;
				/** 对于特服绑定的取消,判断该用户是否还有别的套餐有相同的绑定,
				    如果没有,则取消该特服绑定的预约记录  **/
				EXEC SQL select count(*) into :vFuncBindFlag
					from dBillCustDetail where id_no=to_number(:vIdNo) and detail_type='a'
					and detail_code=:vDetailCode and end_time>to_date(:iEffectTime,'yyyymmdd hh24:mi:ss');
				if(vFuncBindFlag==0)
				{
					init(vBindOffonFlag);init(vFuncBindCode);
					EXEC SQL select function_code,off_on into :vFuncBindCode,:vBindOffonFlag from sBillFuncBind
					where region_code=substr(:vBelongCode,1,2) and function_bind=:vDetailCode;

				/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
					EXEC SQL insert into wCustExpireHis(
						command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type, ORG_ID)
						SELECT command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,:iLoginNo,to_number(:iLoginAccept),:vTotalDate,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
						:iOpCode,'d', ORG_ID
						from wCustExpire where id_no=to_number(:vIdNo) and function_code=:vFuncBindCode
						and business_status=:vBindOffonFlag and login_accept=:iOldAccept;

					EXEC SQL delete wCustExpire where id_no=to_number(:vIdNo)
						and function_code=:vFuncBindCode and function_code=:vFuncBindCode and login_accept=to_number(:iOldAccept);
				}
				break;

			/**
			* modify Time: 2005-11-20
			* modify author: lixg
			* modify Resion: 吉林产品改造升级 begin
			*/
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", "回退缴费回馈、买手机送话费产品信息。。。");
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d", vTotalDate);
				strcpy(vGrantMsg.modeCode, iOldMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.oldLoginAccept, iOldAccept);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);

				if ( (iProductRetCode = sProductCancelFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "", "iProductRetCode", "");
					return iProductRetCode;
				}

				break;
				/* end */
			default:;
		}
		i++;
	}
	EXEC SQL CLOSE curModeDet1;

/* 集团上线暂时注释下面代码 */
#if 0
	/* 记录服务历史表 chenxuan boss3 */
	EXEC SQL insert into dBillCustSrvHis (id_no, srv_type, srv_code, begin_time, end_time,
			mode_code, login_accept, update_code, update_date, update_time, update_login,
			update_accept, update_type)
		select id_no, srv_type, srv_code, begin_time, end_time, mode_code, login_accept,
			:iOpCode, :vTotalDate, to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :iLoginNo, :iLoginAccept, 'd'
		from dBillCustSrv where id_no = :vIdNo and login_accept = :iOldAccept;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		pubLog_DBErr(_FFL_, "", "127070", " .pubSladderBillBeginMode :insert dBillCustSrv error");
		return 127070;
	}

	/* 删除用户服务 */
	EXEC SQL delete from dBillCustSrv where id_no = :vIdNo and login_accept = :iOldAccept;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		PUBLOG_DBDEBUG("pubSladderBillEndMode");
		pubLog_DBErr(_FFL_, "", "127071", "delete dBillCustSrv error");
		return 127071;
	}

	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.off_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre0 FROM :vSelSql;
	EXEC SQL DECLARE cur0 CURSOR FOR pre0;
	EXEC SQL OPEN cur0 USING :vBelongCode, :iOldMode;
	for (;;){
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur0 INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "", "127071", "insert wSndSrvDay error");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur0;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	/* 调用指令分发函数 chenxuan boss3 */
	strcpy(tPubBillInfo.IdNo,        vIdNo);
	strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
	strcpy(tPubBillInfo.BelongCode,  vBelongCode);
	strcpy(tPubBillInfo.SmCode,      vSmCode);
	strcpy(tPubBillInfo.ModeCode,    iOldMode);
	strcpy(tPubBillInfo.OpCode,      iOpCode);
	strcpy(tPubBillInfo.OpTime,      iOpTime);
	strcpy(tPubBillInfo.EffectTime,  iEffectTime);
	tPubBillInfo.TotalDate = vTotalDate;
	strcpy(tPubBillInfo.LoginNo,     iLoginNo);
	strcpy(tPubBillInfo.LoginAccept, iOldAccept);
	pubLog_Debug(_FFL_, "", "", "begin fDispatchCmd+++++++++++++++ ");
	iProductRetCode = fDispatchCmd("0", &tPubBillInfo, oErrMsg);
	if (iProductRetCode != 0) {
		pubLog_DBErr(_FFL_, "", "127072", "");
		return 127072;
	}
	/* 指令分发函数结束 chenxuan boss3 */

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	EXEC SQL select max_num, cur_num, buy_num INTO :vMaxNum, :vCurNum, :vBuyNum
		from sBillModeNum where region_code = substr(:vBelongCode,1,2) and mode_code = :iOldMode
		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'2',sysdate
				  from	sbillmodenum
			     where	region_code = substr(:vBelongCode,1,2)  and mode_code = :iOldMode and cur_num > 0
				   and 	sysdate > begin_time and sysdate < end_time;

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(oErrMsg, "更新表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "","127073", "[%s]", oErrMsg);
			return 127073;
		}
	}

	EXEC SQL select max_num, cur_num, buy_num INTO :vMaxNum, :vCurNum, :vBuyNum
		from sBillModeNum where region_code = '00' and mode_code = :iOldMode
		and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'2',sysdate
				  from	sbillmodenum
			     where	region_code = '00'  and mode_code = :iOldMode and cur_num > 0
				   and 	sysdate > begin_time and sysdate < end_time;

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(oErrMsg, "更新表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubSladderBillEndMode");
			pubLog_DBErr(_FFL_, "","127073", "[%s]", oErrMsg);
			return 127073;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubSladderBillEndMode", "", " pubSladderBillEndMode end ");
#endif

	return 0;
}

/**********************************************
 @wt  PRG  : pubSladderBillBeginMode
 @wt  FUNC : 取消iPhoneNo的iNewMode资费  阶梯套餐绑定
 @wt       : 其中资费总生效时间为iEffectTime,
 @wt       : 业务功能iOpCode,
 @wt       : 操作工号iLoginNo,
 @wt       : 操作流水iLoginAccept,
 @wt       : 操作时间iOpTime
 @wt       : 操作时间iOpTime
 @wt       : 资费总的生效方式iSendFlag
 @wt       : 资费中所有的个性批价信息组合串iFlagStr
 @wt       : iFlagStr的格式为
 @wt       : aaaa^A1^A2^:bbbb^B1^B2^:cccc^C1^C2^C3^:
 @wt       : aaaa,bbbb,cccc为个性批价代码
 @wt       : A1、A2为aaaa的信息组合;B1、B2为bbbb的信息组合……
 @wt -----------------------------------------------------
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iFlagStr     计费的附加信息串
 @wt 10 vBunchNo    手机串号资源（产品改造新增参数）
 @wt 9 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/

int pubSladderBillBeginMode(
	char *iPhoneNo,
	char *iSell_code,
	char *iNewMode,
	char *iEffectTime,
	char *iEndTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *iSendFlag,
	char *iFlagStr,
	char *iBunchNo,
	char *oErrMsg)
{
	/* BOSS2.0 lixg add begin: 20070410 */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	char vBCtrlCode[5+1];
	char    dynStmt[1024];

	/* BOSS2.0 lixg add end: 20070410 */
	EXEC SQL BEGIN DECLARE SECTION;
		char    vIdNo[23];
		int     i=0,t=0,vTotalDate=0,vFavOrder=0;
		char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
		int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
		char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
		char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
		char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
		int     vSendFlag,vUserFunc=-1;
		char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
		double  vMocRate=0,vMtcRate=0;
		char    vTimeCode[3];
		tUserBase   userBase;
		float       vPrepayFee=0;

		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		char vLastBIllType[1+1];
		char vBeginFlag[1+1];

		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];
		int		retValue=0;
		int	 vCount=0;
		char vPromName[20+1];
		char vDealFunc[2+1];
		char vOrgId[10+1];

		char vPhoneNo1[15+1];
		char vFuncCode1[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		char vGroupId[10+1];
		TPubBillInfo tPubBillInfo;

		char vRegionCode[2+1];
		int vMaxNum, vCurNum, vBuyNum;
		char vNewType[1+1];

		char vFavValue[10+1];
		/*ng 解耦*/
		char vEffectTime[17+1];
		double  vMocRateNum=0,vMtcRateNum=0;
		char vMocRateStr[8+1];
		char vMtcRateStr[8+1];
		char vFavOrderStr[4+1];
		char vTotalDateStr[8+1];
		int  iNum = 0;
		int	 iNum1 = 0;
		TdBaseFav 			vTdBaseFav;
		TdBaseVpmn 			vTdBaseVpmn;
		TdBillCustDetail 	vTdBillCustDetail;
		TdCustExpire 		vTdCustExpire;
		TdCustFunc			vTdCustFunc;
		TdCustFuncIndex		vTdCustFuncIndex;
		TdCustFuncHis		vTdCustFuncHis;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		
		
		/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.03.19 end*/
	EXEC SQL END   DECLARE SECTION;

	Sinitn(dynStmt);
	Sinitn(vPhoneNo1);
	Sinitn(vFuncCode1);
	Sinitn(vRegionCode);
	Sinitn(vOrgId);
	Sinitn(vEffectTime);
	Sinitn(v_parameter_array);
	Sinitn(vMocRateStr);
	Sinitn(vMtcRateStr);
	Sinitn(vFavOrderStr);
	Sinitn(vTotalDateStr);

	Trim(iPhoneNo);
	Trim(iNewMode);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);
	/*Trim(iSendFlag);
	Trim(iFlagStr);*/

	memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
	/*Modify for 87,02指令参数调整 at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.04.17 end*/
	oErrMsg[0] = '\0';

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "",""," begin ----- ");
	pubLog_Debug(_FFL_, "",""," iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "",""," iNewMode=[%s] ",iNewMode);
	pubLog_Debug(_FFL_, "",""," iEffectTime=[%s] ",iEffectTime);
	pubLog_Debug(_FFL_, "",""," iEndTime=[%s] ",iEndTime);

	pubLog_Debug(_FFL_, "",""," iOpCode=[%s]  ",iOpCode);
	pubLog_Debug(_FFL_, "",""," iLoginNo=[%s] ",iLoginNo);
	pubLog_Debug(_FFL_, "",""," iLoginAccept=[%s] ",iLoginAccept);
	pubLog_Debug(_FFL_, "",""," iOpTime=[%s]  ",iOpTime);
	pubLog_Debug(_FFL_, "",""," iSendFlag=[%s] ",iSendFlag);

	pubLog_Debug(_FFL_, "",""," iFlagStr=[%s] ",iFlagStr);
#endif

	Sinitn(vIdNo);
	Sinitn(vLastBIllType);
	Sinitn(vBeginFlag);
	Trim(iOpTime);
	EXEC SQL select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
		return 1024;
	}


	strncpy(vRegionCode, vBelongCode, 2);

	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
	int ret =0;
	init(vGroupId);
	ret = sGetUserGroupid(iPhoneNo,vGroupId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败!");
		return 200919;
	}
	Trim(vGroupId);
	/*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}

	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","127075","[%s] ", oErrMsg);
		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}
	/* 检查资费关系结束 chenxuan boss3 */

	Trim(vIdNo);
	Sinitn(vHlrCode);
	Sinitn(vImsiNo);Sinitn(vSimNo);
	/*
	if(iOpCode[0]!='8'){*** 专线用户 ***
	*/
	if(strncmp(iNewMode,"kd",2)!=0 && strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0
		&& strncmp(iNewMode,"id",2)!=0 && strncmp(iNewMode,"af",2)!=0 && strncmp(iNewMode,"ad",2)!=0 && strncmp(iNewMode,"ma",2)!=0)
	{/*** 专线用户 ***/
		EXEC SQL select hlr_code into :vHlrCode from sHlrCode
			where phoneno_head=substr(:iPhoneNo,1,7);
		if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询sHrlCode错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
			return 127076;
		}

		EXEC SQL select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK){
			strcpy(oErrMsg,"查询dCustSim错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
			return 127077;
		}
	}

	 /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(iLoginNo,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败!");
		return 200919;
	}
	Trim(vOrgId);
	/*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","127078"," [%s] ", oErrMsg);
		return 127078;
	}


	Trim(vOrgId);
	Sinitn(vTimeCode);
	EXEC SQL select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
	if(SQLCODE==OK&&SQLROWS==1)
	{
		if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
		{
			Sinitn((void*)(&userBase));
			/*调用公用函数*/
			init(oErrMsg);
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			/*sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));*/
			sprintf(oErrMsg, "%06d", fGetUserBaseInfo_ng(iPhoneNo,iOpCode,&userBase));
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			Trim(oErrMsg);
			if( strcmp(oErrMsg, "000000") != 0 )
			{
				strcpy(oErrMsg,"查询用户信息错误!");
				PUBLOG_DBDEBUG("pubSladderBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

			vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
			pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
			vDeltaDays=0;

			EXEC SQL select days into :vDeltaDays
					from sPayTime
					where region_code=substr(:vBelongCode,1,2)
					and bill_code=:iNewMode
					and open_flag='1'
					and begin_money<=:vPrepayFee
					and end_money>=:vPrepayFee;
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
				PUBLOG_DBDEBUG("pubSladderBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

		}

		Sinitn(vLastBIllType);
		Sinitn(vBeginFlag);
		EXEC SQL select last_bill_type into :vLastBIllType
			from dcustmsg
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK)
		{
			strcpy(oErrMsg,"取用户last_bill_type错误");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
			return 127078;
		}
		if(strncmp(vLastBIllType,"1",1)==0)
		{
			strcpy(vBeginFlag,"Y");
		}
		else
		{
			strcpy(vBeginFlag,"N");
		}

		EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	vBeginFlag		,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}

		exec sql insert into dCustExpireHis(
		    ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
		    UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
		values(
			to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
			return 127085;
		}

		/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
		exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
			values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期wExpireList错误");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
			return 127086;
		}
	}


	Sinitn(vModeFlag);Sinitn(vSmCode);init(vFavBrand);init(vBCtrlCode);
	EXEC SQL select a.mode_flag, a.sm_code, b.fav_brand, nvl(a.before_ctrl_code,' ')
		into :vModeFlag,:vSmCode,:vFavBrand, :vBCtrlCode
		from sBillModeCode a, sSmCode b
		where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
			and b.region_code=a.region_code and b.sm_code=a.sm_code;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","127060","[%s] ", oErrMsg);
		return 127060;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	/* lixg add: 20070118 增加产品前项控制 */
	Trim(vBCtrlCode);
	if(strlen(vBCtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vBCtrlCode, iOpCode, iLoginNo, &fe) != 0)
		{
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, ""," atoi(tmpRetCode)",oErrMsg);
			return atoi(tmpRetCode);
		}
	}
#endif

	/* 处理产品包含的资费代码 */
	Sinitn(vSelSql);
	strcpy(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
		" from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2");
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubSladderBillBeginMode %s ",vSelSql);
#endif
	EXEC SQL PREPARE preBeginDet FROM :vSelSql;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127079;
	}
	EXEC SQL DECLARE curBeginDet CURSOR FOR preBeginDet;
	EXEC SQL OPEN curBeginDet USING :vBelongCode, :iNewMode;
	for(i=0;;){
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
		EXEC SQL FETCH curBeginDet
			into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
		if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
			exec sql close curBeginDet;
			return 127061;
		}
		if(i!=0&&SQLCODE==NOTFOUND) break;

		/*****************************************************************************
		 ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
		 ** sBillModeDetail.time_unit 0天/1月/2年                                   **
		 ****************************************************************************/
		Sinitn(vBeginTime);
		Sinitn(vEndTime);
		switch (vTimeFlag[0])
		{
			case '0':
				strcpy(vBeginTime,iEffectTime);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '1':
				strcpy(vBeginTime,iEffectTime);
				pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime,vIdNo);
				break;
			case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '3':
			default:;
		}
	if(strcmp (vDetailType ,"0")== 0)
		{
			init(vBeginTime);
			init(vEndTime);
			printf(  "iEffectTime  %s  \n" ,iEffectTime );
			printf(  "vBelongCode  %s  \n" ,vBelongCode );
			printf(  "iSell_code   %s  \n" ,iSell_code );
			printf(  "iNewMode     %s  \n" ,iNewMode );
			printf(  "vDetailCode     %s  \n" ,vDetailCode );
			init(vFavValue);
			/* R_CMI_JL_liuyinga_2013_462640@关于升值套餐生效方式调整的需求 by miaoyl at 20130327 begin 
			**EXEC SQL select
			**	add_months(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),fav_begin),
			**	add_months(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),fav_end),
			**	fav_value
			**	into :vBeginTime, :vEndTime,:vFavValue
			**	from sladderfav
			**	where region_code =substr(:vBelongCode,0,2)
			**	and  sell_code  = :iSell_code
			**	and  mode_code  = :iNewMode
			**	and  detail_Code =:vDetailCode;
			**	修改begin_time,end_time的计算方式.iEffectTime立即生效sysdate,预约生效下个月01号
			*/
			EXEC SQL select
				to_char(add_months(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),fav_begin),'YYYYMM')||'01',
				to_char(add_months(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),fav_end),'YYYYMM')||'01',
				fav_value
				into :vBeginTime, :vEndTime,:vFavValue
				from sladderfav
				where region_code =substr(:vBelongCode,0,2)
				and  sell_code  = :iSell_code
				and  mode_code  = :iNewMode
				and  detail_Code =:vDetailCode;
			if(SQLCODE !=0 )
			{
				sprintf(oErrMsg,"取阶梯套餐信息失败! SQLCODE = [%d]",SQLCODE);
				PUBLOG_DBDEBUG("pubSladderBillBeginMode");
				pubLog_DBErr(_FFL_, "","998616","[%s] ", oErrMsg);
				return 998516;
			}
			else if(SQLCODE == 1403)
			{
				sprintf(oErrMsg,"取阶梯套餐信息失败! SQLCODE = [%d]",SQLCODE);
				PUBLOG_DBDEBUG("pubSladderBillBeginMode");
				pubLog_DBErr(_FFL_, "","998626","[%s] ", oErrMsg);
				return 998526;
			}
		}


		/*strcpy(vEndTime,iEndTime);*/
		Trim(vBeginTime);
		Trim(vEndTime);
		if (strcmp(vModeTime,"Y") == 0){
			/*printf("******vBeginTime=[%s]vEndTime=[%s]********** ",vBeginTime,vEndTime);*/
			retValue=DcustModeChgAwake(iOpCode,iNewMode,vBelongCode,"a",atol(iLoginAccept),iPhoneNo,iLoginNo,vBeginTime,vEndTime);
			if(retValue<0){
				EXEC SQL CLOSE curBeginDet;
				strcpy(oErrMsg,"发送提醒短信错误!");
				PUBLOG_DBDEBUG("pubSladderBillBeginMode");
				pubLog_DBErr(_FFL_, "","127099","[%s] ", oErrMsg);
				return 127099;
			}
		}

#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubSladderBillBeginMode  [%s~%s][%s:%s-%s]*%s* ",\
		vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);
#endif

	printf(":vBeginTime  %s  , :vEndTime  %s ,:vFavValue  %s \n" ,vBeginTime, vEndTime,vFavValue);

		Sinitn(vBeginSql);
		sprintf(vBeginSql,
			" insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
			" fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
			" login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
			" :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
			":v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
			vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_,"","","pubSladderBillBeginMode %s ",vBeginSql);
#endif
		EXEC SQL PREPARE preWWW_5 FROM :vBeginSql;
		EXEC SQL EXECUTE preWWW_5 USING :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
			:vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
			:iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if (SQLCODE!=OK){
			exec sql close curBeginDet;
			sprintf(oErrMsg,"记录dBillCustDetail的历史错误[%d]!",SQLCODE);
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
			return 127062;
		}


		Sinitn(vBeginSql);

		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		switch (vDetailType[0]){
			/* lixg Add: 2005-11-20 begin */
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
				strcpy(vGrantMsg.modeCode, iNewMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.machineId, iBunchNo);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);
				pubLog_Debug(_FFL_,"","","product Begin ..... ");
				if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
					return iProductRetCode;
				}
				break;
				/* lixg Add: 2005-11-20 end */
			case '0' :
			/* 阶梯套餐优惠开始办理 */

				Trim(vBelongCode);
				Trim(iSell_code);
				Trim(iNewMode);
				if(iSendFlag[0]=='2')
					strcpy(vFavDay,"1");
				else
					strcpy(vFavDay,"0");

				strcpy(vSon,vDetailCode);
				strcpy(	vTdBaseFav.sMsisdn		  ,	iPhoneNo	  );
				strcpy(vTdBaseFav.sFavBrand		  ,	vSmCode		  );
				strcpy(vTdBaseFav.sFavType		  ,	vDetailCode );
				strcpy(vTdBaseFav.sFlagCode		  ,	vSon        );
				strcpy(vTdBaseFav.sFavOrder		  ,	vFavOrderStr);
				strcpy(vTdBaseFav.sFavDay		    ,	vFavDay	   	);
				strcpy(vTdBaseFav.sStartTime	  ,	vBeginTime	);
				strcpy(vTdBaseFav.sEndTime		  ,	vEndTime	  );
				strcpy(vTdBaseFav.sRegionCode	  ,	vRegionCode	);
				strcpy(vTdBaseFav.sIdNo			    ,	vIdNo		    );
				strcpy(vTdBaseFav.sGroupId		  ,	vGroupId	  );
				strcpy(vTdBaseFav.sProductCode	, iNewMode	  );
				strcpy(vTdBaseFav.sFreeValue	  , vFavValue	  );

				printf(" vTdBaseFav.sMsisdn     %s \n ",vTdBaseFav.sMsisdn     );
				printf(" vTdBaseFav.sRegionCode %s \n ",vTdBaseFav.sRegionCode );
				printf(" vTdBaseFav.sFavBrand   %s \n ",vTdBaseFav.sFavBrand   );
				printf(" vTdBaseFav.sFavType    %s \n ",vTdBaseFav.sFavType    );
				printf(" vTdBaseFav.sFlagCode   %s \n ",vTdBaseFav.sFlagCode   );
				printf(" vTdBaseFav.sFavOrder   %s \n ",vTdBaseFav.sFavOrder   );
				printf(" vTdBaseFav.sFavDay     %s \n ",vTdBaseFav.sFavDay     );
				printf(" vTdBaseFav.sStartTime  %s \n ",vTdBaseFav.sStartTime  );
				printf(" vTdBaseFav.sEndTime    %s \n ",vTdBaseFav.sEndTime    );
				printf(" vTdBaseFav.sServiceId  %s \n ",vTdBaseFav.sServiceId  );
				printf(" vTdBaseFav.sFavPeriod  %s \n ",vTdBaseFav.sFavPeriod  );
				printf(" vTdBaseFav.sFreeValue  %s \n ",vTdBaseFav.sFreeValue  );
				printf(" vTdBaseFav.sIdNo       %s \n ",vTdBaseFav.sIdNo       );
				printf(" vTdBaseFav.sGroupId    %s \n ",vTdBaseFav.sGroupId    );
				printf(" vTdBaseFav.sProductCode %s \n ",vTdBaseFav.sProductCode);

				ret = 0;
				ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdBaseFav);
				if (ret != 0)
				{
					sprintf(oErrMsg,"记录dBaseFav错误!   ret [%d] " ,ret);
					PUBLOG_DBDEBUG("pubSladderBillBeginMode");
					pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
					return 127064;
				}

				printf("iPhoneNo     %s \n",iPhoneNo);
				printf("vSmCode      %s \n",vSmCode);
				printf("vDetailCode  %s \n",vDetailCode);
				printf("vSon         %s \n",vSon);
				printf("vFavOrder    %d \n",vFavOrder);
				printf("vFavDay      %s \n",vFavDay);
				printf("vBeginTime   %s \n",vBeginTime);
				printf("vEndTime     %s \n",vEndTime);
				printf("iOpTime      %s \n",iOpTime);
				printf("vIdNo        %s \n",vIdNo);
				printf("vGroupId     %s \n",vGroupId);
				printf("iNewMode     %s \n",iNewMode);

#ifdef _CHGTABLE_
				EXEC SQL insert into wBaseFavChg(
					msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					select
					:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
					:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
					to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
					to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
					from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
				if(SQLCODE!=OK||SQLROWS!=1)
				{
					sprintf(oErrMsg,"记录wBaseFavChg错误!  SQLCODE %d ",SQLCODE);
					PUBLOG_DBDEBUG("pubSladderBillBeginMode");
					pubLog_DBErr(_FFL_, "","127065","[%s] ", oErrMsg);
					return 127065;
				}
#endif
		 break ;
			case '1':
			case '2':
			case '3':
			case '4':
			case 'a':
#ifdef _CHGTABLE_
				EXEC SQL insert into wUserFavChg(
					op_no,op_type,op_time,id_no,detail_type,detail_code,
					begin_time,end_time,fav_order,mode_code)
					values(
					sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
					to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
				if(SQLCODE!=OK){
					exec sql close curBeginDet;
					strcpy(oErrMsg,"记录wUseFavChg错误!");
					PUBLOG_DBDEBUG("pubSladderBillBeginMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);
					return 127066;
				}
#endif
				if(vDetailType[0] != 'a') break;
				/** 处理特服绑定的情况 ***/
				vSendFlag=strcmp(vBeginTime,iOpTime);
				Sinitn(vFunctionType);Sinitn(vFunctionCode);
				Sinitn(vOffOn);Sinitn(vCommandCode);
				EXEC SQL select a.function_code,a.off_on,b.command_code,b.function_type
					into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
					from sBillFuncBind a,sFuncList b
					where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
					and b.region_code=a.region_code and b.sm_code=a.sm_code
					and b.function_code=a.function_code;
				if (SQLCODE!=OK){
					PUBLOG_DBDEBUG("pubSladderBillBeginMode");
					pubLog_DBErr(_FFL_, "","127067","");
					return 127067;
				}

				if (vSendFlag > 0) /***预约 开/关***/
				{
					EXEC SQL select count(*) into :vUserFunc from wCustExpire
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
						and business_status=:vOffOn;
					if (vUserFunc == 0){
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						EXEC SQL insert into wCustExpire(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,ORG_ID)
							values(
							sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
							:vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
							to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
						if (SQLCODE != OK){
							PUBLOG_DBDEBUG("pubSladderBillBeginMode");
							pubLog_DBErr(_FFL_, "","127068"," ");
							return 127068;
						}

						EXEC SQL insert into wCustExpireHis(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,ORG_ID)
							select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
							from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
						if (SQLCODE!=OK||SQLROWS!=1){
							PUBLOG_DBDEBUG("pubSladderBillBeginMode");
							pubLog_DBErr(_FFL_, "","127069","");
							return 127069;
						}
					}
				}
				else { /***立即 开/关***/
					EXEC SQL select count(*) into :vUserFunc from dCustFunc
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

					if (vOffOn[0]=='1'){ /** 立即开 ***/
						if(vUserFunc==0){

							memset(vTotalDateStr, 0, sizeof(vTotalDateStr));
							sprintf(vTotalDateStr,"%d",vTotalDate);
							strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubSladderBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}

							strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubSladderBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
						}
					}
					else {  /** 立即关 **/
						if (vUserFunc != 0){
							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDateStr,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr50 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur50 CURSOR FOR ng_SqlStr50;
							EXEC SQL OPEN ng_Cur50 using :iLoginNo,:iLoginAccept,:vTotalDateStr,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur50 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubSladderBillBeginMode取dCustFuncHis主键");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur50;
									return 127098;
								}

								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubSladderBillBeginMode");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur50;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur50;



							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur1 CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur1 using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur1 INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubSladderBillBeginMode取dCustFunc主键");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur1;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubSladderBillBeginMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubSladderBillBeginMode");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur1;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur1;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubSladderBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
						}
					}

					EXEC SQL insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
						op_code,login_no,login_accept)
						values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
						to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
					if (SQLCODE != OK){
						PUBLOG_DBDEBUG("pubSladderBillBeginMode");
						pubLog_DBErr(_FFL_, "","127074","");
						return 127074;
					}

					if (iOpCode[0] != '8') /*** 非专线用户在立即生效时发送开关机命令 ***/
					{
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
												/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
						if(strcmp(vFunctionCode,"02")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE a.id_no = :vIdNo
														AND a.function_code = '87';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS3");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						if(strcmp(vFunctionCode,"87")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE id_no = :vIdNo
														AND function_code = '02';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS1");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
						else
						{
						
						
						
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubSladderBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}

						if(strcmp(vFunctionCode,"95")==0)
						{
							Sinitn(dynStmt);
							sprintf(dynStmt,"insert into wSndCmdDay "
								"("
								"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,"
								"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
								"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT,"
								"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE,org_id, group_id"
								")"
								" select "
								" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, "
								" :v4, :v5, :v6, :v7, :v8, :v9,"
								" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14, to_number(:v15), "
								" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code, :v22, :v23 "
								" from sFuncList "
								" where region_code = substr(:v19, 1,2)"
								"   and sm_code = :v20"
								"   and function_code = :v21");

							if (vOffOn[0]=='1')
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"05");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :iPhoneNo, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId, :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;

								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
							else
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
						}
					}
				}

			default:;
		}
		i++;
	}
	EXEC SQL CLOSE curBeginDet;

#if 0
	/* 开始处理服务代码 chenxuan boss3 */
	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre FROM :vSelSql;
	EXEC SQL DECLARE cur CURSOR FOR pre;
	EXEC SQL OPEN cur USING :vBelongCode, :iNewMode;
	for (;;) {
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入用户使用服务表 */
		EXEC SQL insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
				mode_code, login_accept)
			values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
				to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127070","");
			return 127070;
		}

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","127071","");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	/* 调用指令分发函数 chenxuan boss3 */
	strcpy(tPubBillInfo.IdNo,        vIdNo);
	strcpy(tPubBillInfo.PhoneNo,     iPhoneNo);
	strcpy(tPubBillInfo.BelongCode,  vBelongCode);
	strcpy(tPubBillInfo.SmCode,      vSmCode);
	strcpy(tPubBillInfo.ModeCode,    iNewMode);
	strcpy(tPubBillInfo.OpCode,      iOpCode);
	strcpy(tPubBillInfo.OpTime,      iOpTime);
	strcpy(tPubBillInfo.EffectTime,  iEffectTime);
	tPubBillInfo.TotalDate = vTotalDate;
	strcpy(tPubBillInfo.LoginNo,     iLoginNo);
	strcpy(tPubBillInfo.LoginAccept, iLoginAccept);

	iProductRetCode = fDispatchCmd("1", &tPubBillInfo, oErrMsg);
	if (iProductRetCode != 0) {
		PUBLOG_DBDEBUG("pubSladderBillBeginMode");
		pubLog_DBErr(_FFL_, "","100050","");
		return 100050;
	}
	/* 指令分发函数结束 chenxuan boss3 */

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
		INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
		from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
			and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100051"," [%s] ", oErrMsg);
			return 100051;
		}
	    Trim(flag);
	    pubLog_Debug(_FFL_,"","","11111111111111111111=[%s] ",flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100052","[%s]",oErrMsg);

			return 100052;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0) && (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100053","[%s]",oErrMsg);
			return 100053;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

	/*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			from sBillModeNum
			where region_code = '00' and mode_code = :iNewMode
			and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
			select	region_code,mode_code,flag,'1',sysdate
			from	sbillmodenum
			where	region_code = '00' and mode_code = :iNewMode
			and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100054"," [%s] ", oErrMsg);
			return 100054;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0)&& (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100055","[%s]", oErrMsg);

			return 100055;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubSladderBillBeginMode");
			pubLog_DBErr(_FFL_, "","100056","[%s]", oErrMsg);
			return 100056;
		}
	}
	/*增加全省统一数量控制功能 20081119  end*/
	
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","pubSladderBillBeginMode end ----- ");
#endif
	return 0;
}


/**********************************************
 @wt  PRG  : chkTDAllow
 @wt  FUNC : 铁通固网号码无线座机套餐用户限制办理业务的需求
 @wt  create : majha 
 @wt -----------------------------------------------------
 @wt 0 iIdNo     	用户ID
 @wt 1 iPhoneNo     移动号码
 @wt 2 iModeCode  	套餐代码(套餐类业务时不允许为空)
 @wt 3 iBizFlag		业务标识，0-特服，1-套餐类，2-梦网类，3-集团V网，4-家庭V网，5-智能网个人彩铃，6-智能网关
 @wt 4 oErrMsg      函数内部错误信息
 备注：限制办理业务：包括开通特服、申请短信套餐、GPRS套餐、梦网类业务等
 *************************************************/
int chkTDAllow(char *iIdNo,char *iPhoneNo, char *iModeCode,char *iBizFlag, char *oRetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0,tCount=0;
	EXEC SQL END   DECLARE SECTION;
#ifdef _DEBUG_
	printf("iIdNo=[%s]\n",iIdNo);
	printf("iPhoneNo=[%s]\n",iPhoneNo);
	printf("iModeCode=[%s]\n",iModeCode);
	printf("iBizFlag=[%s]\n",iBizFlag);
#endif	
	/*判断用户是否是铁通固网号码无线座机套餐用户*/
	vCount=0;
	EXEC SQL  select count(*)
		into :vCount 
		from dbillcustdetail b,stdwirelesscfg c
 		where b.mode_code=c.mode_code
 		and b.mode_time='Y'
 		and b.mode_status='Y'
 		and b.end_time>sysdate
		and b.id_no=to_number(:iIdNo)
		and b.region_code=c.region_code;
	if (SQLCODE != 0 ) 
	{
		sprintf(oRetMsg, "查询用户信息失败[%d]", SQLCODE);
		pubLog_DBErr(_FFL_, "","-1","%s",oRetMsg);
		return -1;
	}
	if(vCount>0)
	{
		if(strcmp(iBizFlag,"0")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能开通特服业务");
			pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
			return -2;
		}
		else if(strcmp(iBizFlag,"2")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能申请梦网业务");
			pubLog_DBErr(_FFL_, "","-3","%s",oRetMsg);
			return -3;
		}
		else if(strcmp(iBizFlag,"1")==0)
		{
			/**R_JLMob_liuyinga_CRM_CMI_2012_0467：关于限制铁通固话号码在移动侧缴费及业务办理的需求 yaoxc 20120604
			Exec sql select count(*)
				into :tCount
				from dual 
				where (:iModeCode like 'gn20%' or :iModeCode like 'gn22%');
			if (SQLCODE != 0 ) 
			{
				sprintf(oRetMsg, "查询用户套餐信息表失败[%d]", SQLCODE);
				pubLog_DBErr(_FFL_, "","-4","%s",oRetMsg);
				return -3;
			}
			if(tCount>0)
			{
				sprintf(oRetMsg, "无线座机套餐用户不能申请短信套餐、GPRS套餐");
				pubLog_DBErr(_FFL_, "","-5","%s",oRetMsg);
				return -5;
			}**/
			sprintf(oRetMsg, "无线座机套餐用户不能申请办理套餐业务");
			pubLog_DBErr(_FFL_, "","-5","%s",oRetMsg);
			return -5;
		}
		else if(strcmp(iBizFlag,"3")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能办理集团V网业务");
			pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
			return -2;
		}
		else if(strcmp(iBizFlag,"4")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能办理家庭V网业务");
			pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
			return -2;
		}
		else if(strcmp(iBizFlag,"5")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能办理彩铃业务");
			pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
			return -2;
		}
		else if(strcmp(iBizFlag,"6")==0)
		{
			sprintf(oRetMsg, "无线座机套餐用户不能办理智能网关业务");
			pubLog_DBErr(_FFL_, "","-2","%s",oRetMsg);
			return -2;
		}
		else
		{
			sprintf(oRetMsg, "输入的业务类型不正确");
			pubLog_DBErr(_FFL_, "","-6","%s",oRetMsg);
			return -6;
		}					
	}
	return 0;
}


/**
**查询包多月资费当前资费结束时间
**/
int fGetBillCurEndTime(char * vphoneNo,char *vModeAccept, char *modeCode, char *oendTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vopTime[17+1];
		long vidNo=0;
		char vmodeCode[8+1];
		char vendTime[17+1];
		char vBeginTime[17+1];
		char vTime[8+1];
		char regionCode[2+1];
		int  months=0,vmonths=0;
		char phoneNo[15+1];
		long modeAccept=0; 
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vopTime);
	Sinitn(vendTime);
	Sinitn(vBeginTime);
	Sinitn(regionCode);
	Sinitn(phoneNo);
	
	strcpy(vmodeCode,modeCode);
	strcpy(phoneNo,vphoneNo);
	modeAccept=atol(vModeAccept);
	
	EXEC SQL SELECT id_no,substr(belong_code,1,2),to_char(sysdate,'yyyymmdd') INTO :vidNo,:regionCode,:vopTime FROM DCUSTMSG WHERE phone_no=:phoneNo;
	if(SQLCODE !=0 )
	{
		return -9;
	}
	
	EXEC SQL SELECT month_num INTO :months FROM sbillmodecyclecfg WHERE region_code=:regionCode AND mode_code=:vmodeCode;
	if(SQLCODE !=0 && SQLCODE !=1403)
	{
		return -8;
	}
	else if (SQLCODE == 1403)
	{
		strcpy(oendTime,"29990101");
		return 0;
	}
	else
	{
		/*去掉AND mode_status='Y'条件，防止有预约资费时当前资费变为N，获取不到*/
		EXEC SQL SELECT to_char(begin_time,'yyyymmdd') INTO :vBeginTime FROM dbillcustdetail 
						 WHERE id_no=:vidNo AND mode_code=:vmodeCode AND login_accept=:modeAccept
						 AND mode_time='Y'  AND end_time>sysdate;
		if(SQLCODE != 0)
		{
			return -7;
		}
		
		Trim(vBeginTime);
		vmonths=months;
		for(;;)
		{
			EXEC SQL SELECT to_char(add_months(to_date(:vBeginTime,'YYYYMMDD'),:months),'YYYYMMDD') into :vTime from dual;
			printf("vTime=[%s] vopTime=[%s] months=[%d]\n",vTime,vopTime,months);
			if( (atol(vTime)-atol(vopTime))>0 )
			{
				break;
			}
			months=months+vmonths;
		}
		strcpy(oendTime,vTime);
	}
	
	return 0;
	
}


/***
**查询取消包多月套餐的结束时间
ieffTime   用户套餐取消生效时间
modeCode   套餐代码
sendFlag   生效方式
oendTime   返回参数,用户最终生效时间
***/
int fGetBillEndTime(char *vphoneNo,char *vModeAccept,char *ieffTime,char *modeCode, char *sendFlag, char *oendTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vopTime[17+1];
		char vmodeCode[8+1];
		char vsendFlag[1+1];
		char vendTime[17+1];
		char cycleTime[17+1];
		char vcycleEndTime[17+1];
		char vRetMsg[60+1];
		char phoneNo[15+1];
		int  ret = -1;
		char modeAccept[23+1];
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vopTime);
	Sinitn(vmodeCode);
	Sinitn(vsendFlag);
	Sinitn(vendTime);
	Sinitn(cycleTime);
	Sinitn(vcycleEndTime);
	Sinitn(vRetMsg);
	Sinitn(phoneNo);
	Sinitn(modeAccept);
	
	strncpy(vopTime,ieffTime,8);
	strcpy(vmodeCode,modeCode);
	strcpy(vsendFlag,sendFlag);
	strcpy(phoneNo,vphoneNo);
	strcpy(modeAccept,vModeAccept);
	
	printf("+++++phone_no=[%s],vsendFlag=[%s],vopTime=[%s],ieffTime=[%s],modeAccept=[%s],vmodeCode=[%s]+++++\n",phoneNo,vsendFlag,vopTime,ieffTime,modeAccept,vmodeCode);
	
	if(strcmp(vsendFlag,"0")==0)
	{
		strcpy(oendTime,ieffTime);
	}
	else if( strcmp(vsendFlag,"1")==0 )
	{
		ret = fGetBillCurEndTime(phoneNo,modeAccept, vmodeCode, vendTime );
		if( 0 == ret )
		{
			if( atol(vopTime) > atol(vendTime) )
			{
				strcpy(oendTime,vendTime);
			}
			else
			{
				strcpy(oendTime,vopTime);
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		strcpy(oendTime,ieffTime);
	}
	
	printf("+++++oendTime=[%s]+++++\n",oendTime);
	
	return 0;
}


/***
**查询家庭包多月套餐的结束时间
vfamilyId   家庭ID
vphoneNo    责任人号码
vfbizCode   家庭类型
vLastTime   用户原生效时间
newLastTime 用户最新生效时间
***/

int fGetFamilyEndTime(char *vfamilyId,char *vphoneNo,char *vfbizCode,char *vLastTime,char *newLastTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	int		icount=0;
	long	familyId=0;
	char	phoneNo[15+1];
	char	fbizCode[10+1];
	char	lastTime[17+1];
	char	opTime[17+1];
	char	effTime[17+1];
	char	vTime[17+1];
	char	inewTime1[17+1];
	int		orderMonth=0,months=0,i=0;
	
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(phoneNo);
	Sinitn(fbizCode);
	Sinitn(lastTime);
	Sinitn(opTime);
	Sinitn(effTime);
	Sinitn(vTime);
	Sinitn(inewTime1);
	
	familyId=atol(vfamilyId);
	strcpy(phoneNo,vphoneNo);
	strcpy(fbizCode,vfbizCode);
	strncpy(lastTime,vLastTime,8);
	
	EXEC SQL SELECT to_char(sysdate,'yyyymmdd') INTO :opTime FROM dual;
	
	if(strncmp(fbizCode,"--",2) == 0 )
	{
		EXEC SQL SELECT count(*) INTO :icount
							FROM dFamilyProInfo a,sFamilybizInfo b,dcustmsg c
							where b.send_flag='Y'
							and b.fbiz_code not in('02','03','04')
							and a.fbiz_code=b.fbiz_code 
							and a.mode_code=b.mode_code 
							and a.phone_no=c.phone_no and a.detail_type='1' 
							and substr(c.belong_code,1,2)=b.region_code and a.exp_time> sysdate+60 and a.eff_time < sysdate 
							and b.order_month is not null
							and a.family_id=:familyId;
		if(SQLCODE != 0 && SQLCODE !=1403)
		{
			printf("查询家庭包多月套餐的结束时间ERROR1\n");
			strcpy(newLastTime,lastTime);
			return -1;
		}
		
		if(icount > 0)
		{
			EXEC SQL DECLARE cur_family cursor for 
							SELECT  a.fbiz_code,a.phone_no
							FROM dFamilyProInfo a,sFamilybizInfo b,dcustmsg c
							where b.send_flag='Y'
							and b.fbiz_code not in('02','03','04')
							and a.fbiz_code=b.fbiz_code 
							and a.mode_code=b.mode_code 
							and a.phone_no=c.phone_no and a.detail_type='1' 
							and substr(c.belong_code,1,2)=b.region_code and a.exp_time> sysdate+60 and a.eff_time < sysdate 
							and b.order_month is not null
							and a.family_id=:familyId;
			EXEC SQL OPEN cur_family ;
			for(i=0;;)
			{
					init(fbizCode);
					init(phoneNo);
					init(inewTime1);
					EXEC SQL FETCH cur_family INTO :fbizCode,:phoneNo;
					if(SQLCODE == 1403) break;
					else if(SQLCODE != 0 && SQLCODE != 1403)
					{
						printf("查询家庭包多月套餐的结束时间ERROR12\n");
						strcpy(newLastTime,lastTime);
						return -12;
					}
			
					EXEC SQL  SELECT distinct to_char(eff_time,'yyyymmdd'),to_number(b.order_month) INTO :effTime,:orderMonth
									FROM dFamilyProInfo a,sFamilybizInfo b,dcustmsg c
									where b.send_flag='Y'
									and b.fbiz_code not in('02','03','04')
									and a.fbiz_code=b.fbiz_code 
									and a.mode_code=b.mode_code 
									and a.phone_no=c.phone_no and a.detail_type='1' 
									and substr(c.belong_code,1,2)=b.region_code and a.exp_time> sysdate+60 and a.eff_time < sysdate 
									and b.order_month is not null
									and a.family_id=:familyId 
									and a.fbiz_code=:fbizCode
									and a.phone_no=:phoneNo
									and rownum < 2;
					if(SQLCODE != 0 && SQLCODE !=1403)
					{
						printf("查询家庭包多月套餐的结束时间ERROR2\n");
						strcpy(newLastTime,lastTime);
						return -2;
					}
					
					Trim(effTime);
					months=orderMonth;
					for(;;)
					{
						EXEC SQL SELECT to_char(add_months(to_date(:effTime,'YYYYMMDD'),:months),'YYYYMMDD') into :vTime from dual;
						printf("vTime=[%s] vopTime=[%s] months=[%d],i=[%d],newLastTime=[%s]\n",vTime,opTime,months,i,newLastTime);
						if( (atol(vTime)-atol(opTime))>0 )
						{
							break;
						}
						months=months+orderMonth;
					}
					
					if(atol(vTime)-atol(lastTime) > 0)
					{
						strcpy(inewTime1,lastTime);
					}
					else
					{
						strcpy(inewTime1,vTime);
					}
					
					if(i == 0)
					{
						strcpy(newLastTime,inewTime1);
					}
					else
					{
						if(atol(newLastTime)-atol(inewTime1) > 0)
						{
							strcpy(newLastTime,inewTime1);
						}
					}
					
					i++;
			}
			EXEC SQL CLOSE cur_family;
		}
		else
		{
			strcpy(newLastTime,lastTime);
		}
	}
	else
	{
		EXEC SQL SELECT count(*) INTO :icount
							FROM dFamilyProInfo a,sFamilybizInfo b,dcustmsg c
							where b.send_flag='Y'
							and b.fbiz_code not in('02','03','04')
							and a.fbiz_code=b.fbiz_code 
							and a.mode_code=b.mode_code 
							and a.phone_no=c.phone_no and a.detail_type='1' 
							and substr(c.belong_code,1,2)=b.region_code and a.exp_time> sysdate+60 and a.eff_time < sysdate 
							and b.order_month is not null
							and a.family_id=:familyId 
							and a.fbiz_code=:fbizCode
							and a.phone_no=:phoneNo;
		if(SQLCODE != 0 && SQLCODE !=1403)
		{
			printf("查询家庭包多月套餐的结束时间ERROR1\n");
			strcpy(newLastTime,lastTime);
			return -1;
		}
		
		if(icount > 0)
		{
			EXEC SQL  SELECT distinct to_char(eff_time,'yyyymmdd'),to_number(b.order_month) INTO :effTime,:orderMonth
							FROM dFamilyProInfo a,sFamilybizInfo b,dcustmsg c
							where b.send_flag='Y'
							and b.fbiz_code not in('02','03','04')
							and a.fbiz_code=b.fbiz_code 
							and a.mode_code=b.mode_code 
							and a.phone_no=c.phone_no and a.detail_type='1' 
							and substr(c.belong_code,1,2)=b.region_code and a.exp_time> sysdate+60 and a.eff_time < sysdate 
							and b.order_month is not null
							and a.family_id=:familyId 
							and a.fbiz_code=:fbizCode
							and a.phone_no=:phoneNo
							and rownum < 2;
			if(SQLCODE != 0 && SQLCODE !=1403)
			{
				printf("查询家庭包多月套餐的结束时间ERROR2\n");
				strcpy(newLastTime,lastTime);
				return -2;
			}
			
			Trim(effTime);
			months=orderMonth;
			for(;;)
			{
				EXEC SQL SELECT to_char(add_months(to_date(:effTime,'YYYYMMDD'),:months),'YYYYMMDD') into :vTime from dual;
				printf("vTime=[%s] vopTime=[%s] months=[%d]\n",vTime,opTime,months);
				if( (atol(vTime)-atol(opTime))>0 )
				{
					break;
				}
				months=months+orderMonth;
			}
			
			if(atol(vTime)-atol(lastTime) > 0)
			{
				strcpy(newLastTime,lastTime);
			}
			else
			{
				strcpy(newLastTime,vTime);
			}
			
		}
		else
		{
			strcpy(newLastTime,lastTime);
		}
	}
	
	return 0;
	
}
/**********************************************
 @wt  PRG  : pubRandomBillBeginMode
 @wt  FUNC : 开通iPhoneNo的iNewMode资费
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iFlagStr     计费的附加信息串
 @wt 10 vBunchNo    手机串号资源（产品改造新增参数）
 @wt 9 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/

int pubRandomBillBeginMode(
	char *iPhoneNo,
	char *iNewMode,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iLoginAccept,
	char *iOpTime,
	char *iSendFlag,
	char *iFlagStr,
	char *iBunchNo,
	char *iDetailCodeList,
	char *iFbizCode,
	char *oErrMsg)
{
	/* BOSS2.0 lixg add begin: 20070410 */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	char vBCtrlCode[5+1];
	char dynStmt[1024];
	

	/* BOSS2.0 lixg add end: 20070410 */
	EXEC SQL BEGIN DECLARE SECTION;
		char    vIdNo[23];
		int     i=0,t=0,vTotalDate=0,vFavOrder=0;
		char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
		int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
		char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
		char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
		char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
		int     vSendFlag,vUserFunc=-1;
		char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
		double  vMocRate=0,vMtcRate=0;
		char    vTimeCode[3];
		tUserBase   userBase;
		float       vPrepayFee=0;

		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		char vLastBIllType[1+1];
		char vBeginFlag[1+1];

		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];
		int	 retValue=0;
		int	 vCount=0;
		
		char vOrgId[10+1];
		char vNewCardCode[2+1];
		char vOldCardCode[2+1];

		char vPhoneNo1[15+1];
		char vFuncCode1[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		char vGroupId[10+1];
		TPubBillInfo tPubBillInfo;

		char vRegionCode[2+1];
		int  vMaxNum, vCurNum, vBuyNum;
		char vNewType[1+1];
		
		char vDealFunc[2+1];/*功能代码 add by chendx@20111228 */
		int  funcflag = 0;/*单套餐单功能标志，1表示单套餐单功能*/

		/*ng 解耦*/
		char vEffectTime[17+1];
		double  vMocRateNum=0,vMtcRateNum=0;
		char vMocRateStr[8+1];
		char vMtcRateStr[8+1];
		char vFavOrderStr[4+1];
		char vTotalDateStr[8+1];
		int  iNum = 0;
		int	 iNum1 = 0;
		TdBaseFav 			vTdBaseFav;
		TdBaseVpmn 			vTdBaseVpmn;
		TdBillCustDetail 	vTdBillCustDetail;
		TdCustExpire 		vTdCustExpire;
		TdCustFunc			vTdCustFunc;
		TdCustFuncIndex		vTdCustFuncIndex;
		TdCustFuncHis		vTdCustFuncHis;

		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.07.26 end*/
		
		char	vDetailCodeTmp[1024+1];
		char	tDetailCode[4+1];
		char	tDetailCodeList[1024+1];
		char	*retData[80];
		char	vPromName[20+1];
		char	vFuncCommand[256+1];
		char	vOpPlace[3+1];
	EXEC SQL END   DECLARE SECTION;
	
	
	Sinitn(dynStmt);
	Sinitn(vPhoneNo1);
	Sinitn(vFuncCode1);
	Sinitn(vRegionCode);
	Sinitn(vOrgId);
	Sinitn(vEffectTime);
	Sinitn(v_parameter_array);
	Sinitn(vMocRateStr);
	Sinitn(vMtcRateStr);
	Sinitn(vFavOrderStr);
	Sinitn(vTotalDateStr);
	Sinitn(vNewCardCode);
	Sinitn(vOldCardCode);

	Sinitn(vDealFunc);

	Trim(iPhoneNo);
	Trim(iNewMode);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginAccept);
	Trim(iOpTime);
	/*Trim(iSendFlag);
	Trim(iFlagStr);*/
	Sinitn(vDetailCodeTmp);
	Sinitn(tDetailCode);
	Sinitn(tDetailCodeList);

	memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
	/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.07.26 end*/


	oErrMsg[0] = '\0';
	/*init(oErrMsg);*/

	/*if(strcmp(iOpCode,"2933")==0)
	{
		strcpy(iSendFlag,"0");
		strcpy(iFlagStr,"");
	}
	Trim(iFlagStr);
	Trim(iSendFlag);*/

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "",""," begin ----- ");
	pubLog_Debug(_FFL_, "",""," iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "",""," iNewMode=[%s] ",iNewMode);
	pubLog_Debug(_FFL_, "",""," iEffectTime=[%s] ",iEffectTime);
	pubLog_Debug(_FFL_, "",""," iOpCode=[%s]  ",iOpCode);
	pubLog_Debug(_FFL_, "",""," iLoginNo=[%s] ",iLoginNo);
	pubLog_Debug(_FFL_, "",""," iLoginAccept=[%s] ",iLoginAccept);
	pubLog_Debug(_FFL_, "",""," iOpTime=[%s]  ",iOpTime);
	pubLog_Debug(_FFL_, "",""," iSendFlag=[%s] ",iSendFlag);

	pubLog_Debug(_FFL_, "",""," iFlagStr=[%s] ",iFlagStr);
#endif
	
	Sinitn(vIdNo);
	Sinitn(vLastBIllType);
	Sinitn(vBeginFlag);
	Trim(iOpTime);
	EXEC SQL select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
		return 1024;
	}

	strncpy(vRegionCode, vBelongCode, 2);

	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
	int ret =0;
	init(vGroupId);
	ret = sGetUserGroupid(iPhoneNo,vGroupId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败!");
		return 200919;
	}
	Trim(vGroupId);
	/*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

	/* 开始检查资费关系 chenxuan boss3 */
	if (chkModeSrvRela(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127073","");
		return 127073;
	}

	if (chkBillModeLimit(vIdNo, iNewMode, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127074","");
		return 127074;
	}

	/*查看此套餐开通是否受限制  add by lixiaoxin at 20120225*/
	if(checkOpcodeLimit(vRegionCode,iOpCode,iNewMode,oErrMsg,iPhoneNo,vIdNo,iLoginNo) 
		!= 0)
	{
		pubLog_DBErr(_FFL_, "","127022","");
		return 127022;
	}
	
	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","127075","[%s] ", oErrMsg);
		return 127075;
	}

	if (chkModeTypeRela(vIdNo, vNewType, vRegionCode, oErrMsg) != 0){
		pubLog_DBErr(_FFL_, "","127076","");
		return 127076;
	}

	Trim(vIdNo);
	Sinitn(vHlrCode);
	Sinitn(vImsiNo);Sinitn(vSimNo);

	if(strncmp(iNewMode,"kd",2)!=0 && strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0
		&& strncmp(iNewMode,"id",2)!=0 && strncmp(iNewMode,"af",2)!=0 && strncmp(iNewMode,"ad",2)!=0 && strncmp(iNewMode,"ma",2)!=0)
	{/*** 专线用户 ***/
		/*增加IMS用户判断 add by zhaohx at 20110526 */
		if(strncmp(iNewMode,"im",2)!=0 && strncmp(iNewMode,"iv",2)!=0 && strncmp(iNewMode,"dm",2)!=0 && strncmp(iNewMode,"mt",2)!=0 && strncmp(iNewMode,"tp",2)!=0)
		{
			EXEC SQL select hlr_code into :vHlrCode from sHlrCode
				where phoneno_head=substr(:iPhoneNo,1,7);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询sHrlCode错误!");
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
				return 127076;
			}

			EXEC SQL select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
				where id_no=to_number(:vIdNo);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询dCustSim错误!");
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
				return 127077;
			}
		}
	}


	ret =0;
	ret = sGetLoginOrgid(iLoginNo,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败!");
		return 200919;
	}
	Trim(vOrgId);

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","127078"," [%s] ", oErrMsg);
		return 127078;
	}


	Trim(vOrgId);
	Sinitn(vTimeCode);
	EXEC SQL select days,time_code into :vDeltaDays,:vTimeCode from sPayTime
		where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='0';
	if(SQLCODE==OK&&SQLROWS==1){
		if((strncmp(iOpCode,"11",2)!=0) && strncmp(iOpCode,"8",1)!=0)
		{
			Sinitn((void*)(&userBase));
			/*调用公用函数*/
			init(oErrMsg);
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			/*sprintf(oErrMsg, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase));*/
			sprintf(oErrMsg, "%06d", fGetUserBaseInfo_ng(iPhoneNo,iOpCode,&userBase));
			/*************************ng 解耦 替换fGetUserBaseInfo_ng*************************/
			Trim(oErrMsg);
			if( strcmp(oErrMsg, "000000") != 0 )
			{
				strcpy(oErrMsg,"查询用户信息错误!");
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

			vPrepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
			pubLog_Debug(_FFL_,"","","vPrepayFee=[%f][%s] ",vPrepayFee,iNewMode);
			vDeltaDays=0;


			EXEC SQL select days into :vDeltaDays from sPayTime
			where region_code=substr(:vBelongCode,1,2) and bill_code=:iNewMode and open_flag='1'
				and begin_money<=:vPrepayFee and end_money>=:vPrepayFee;
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"用户预存款不足,不能申请预付费");
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
				return 127078;
			}

		}

		Sinitn(vLastBIllType);
		Sinitn(vBeginFlag);
		EXEC SQL select last_bill_type into :vLastBIllType
			from dcustmsg
			where id_no=to_number(:vIdNo);
		if(SQLCODE!=OK)
		{
			strcpy(oErrMsg,"取用户last_bill_type错误");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127078","[%s]",oErrMsg);
			return 127078;
		}
		if(strncmp(vLastBIllType,"1",1)==0)
		{
			strcpy(vBeginFlag,"Y");
		}
		else
		{
			strcpy(vBeginFlag,"N");
		}

		EXEC SQL SELECT to_char(to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,'yyyymmdd hh24:mi:ss')
				 INTO :vEffectTime
				 FROM dual;
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"取EXPIRE_TIME错误");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127184;
		}
		strncpy(vTdCustExpire.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdCustExpire.sOpenTime		,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sBeginFlag	,	vBeginFlag		,	1	);
		strncpy(vTdCustExpire.sBeginTime	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sOldExpire	,	iEffectTime		,	17	);
		strncpy(vTdCustExpire.sExpireTime	,	vEffectTime		,	17	);
		strncpy(vTdCustExpire.sBakField		,	""				,	20	);
		ret = 0;
		ret = OrderInsertCustExpire("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdCustExpire);
		if (ret != 0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpire错误");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127084","[%s] ",oErrMsg);
			return 127084;
		}

		exec sql insert into dCustExpireHis(
		    ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
		    UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
		values(
			to_number(:vIdNo),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),:vBeginFlag,to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
			to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')+:vDeltaDays,to_number(:iLoginAccept),
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),:iLoginNo,'a',:iOpCode,:vTotalDate);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期dCustExpireHis错误");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127085","[%s] ",oErrMsg);
			return 127085;
		}

		/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
		exec sql insert into wExpireList(ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,ORG_CODE,OP_TIME,OP_NOTE,ORG_ID)
			values(to_number(:vIdNo),:vTotalDate,to_number(:iLoginAccept),:iOpCode,:vTimeCode,:vDeltaDays,:iLoginNo,:vOrgCode,
			to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),'申请套餐'||:iNewMode||'初始化有效期信息',:vOrgId);
		if(SQLCODE!=0)
		{
			strcpy(oErrMsg,"记录用户有效期wExpireList错误");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127086","[%s] ",oErrMsg);
			return 127086;
		}
	}


	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
    EXEC SQL select a.deal_func into :vDealFunc
                from ssndcailcfg a,soutsidefeemode b
               where a.region_code = b.region_code
               	 and a.mode_code = b.mode_code
               	 and a.region_code = :vRegionCode
               	 and a.mode_code = :iNewMode;
    if(SQLCODE==OK&&SQLROWS==1)
	{
		printf("表示单套餐单功能!\n");
		funcflag = 1;
	}       	 	
	/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
	
	Sinitn(vModeFlag);
	Sinitn(vSmCode);
	init(vFavBrand);
	init(vBCtrlCode);
	
	EXEC SQL select a.mode_flag, a.sm_code, b.fav_brand, nvl(a.before_ctrl_code,' ')
		into :vModeFlag,:vSmCode,:vFavBrand, :vBCtrlCode
		from sBillModeCode a, sSmCode b
		where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
			and b.region_code=a.region_code and b.sm_code=a.sm_code;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","127060","[%s] ", oErrMsg);
		return 127060;
	}

#if PROVINCE_RUN == PROVINCE_JILIN
	/* lixg add: 20070118 增加产品前项控制 */
	Trim(vBCtrlCode);
	if(strlen(vBCtrlCode) != 0){
		if(fProdCheckOpr(iPhoneNo, vBCtrlCode, iOpCode, iLoginNo, &fe) != 0)
		{
			init(tmpRetCode); init(oErrMsg);
			getValueByParamCode(&fe, 1001, oErrMsg);
			getValueByParamCode(&fe, 1000, tmpRetCode);
			pubLog_DBErr(_FFL_, ""," atoi(tmpRetCode)",oErrMsg);
			return atoi(tmpRetCode);
		}
	}
#endif
	
	
	pubLog_Debug(_FFL_, "",""," 解析iDetailCodeList=[%s]",iDetailCodeList);	
	ret=0;
	ret = gettok(iDetailCodeList,"#", retData);
	if ( ret != 0 )
	{
		strcpy(oErrMsg,"拆分字符串错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127011;
	}
	for(i=0;i<10;i++)
	{
		if(strcmp(retData[i],"") == 0)
		{
			break;
		}
		if(i==0)
		{	
			sprintf(tDetailCodeList,"'%s'",retData[i]);
		}
		else
		{
			sprintf(tDetailCodeList,"%s,'%s'",tDetailCodeList,retData[i]);
		}
		pubLog_Debug(_FFL_, "","","[%d]=[%s]tDetailCodeList[%s]",i,tDetailCode,tDetailCodeList);	
	}
	pubLog_Debug(_FFL_, "",""," 解析完毕tDetailCodeList=[%s]",tDetailCodeList);
	
	/* 处理产品包含的资费代码 */
	Sinitn(vSelSql);
	sprintf(vSelSql,"select detail_type,detail_code,fav_order,mode_time,time_flag,time_cycle,time_unit "
		" from sBillModeDetail where region_code=substr(:v1,1,2) and mode_code=:v2 and mode_time='Y'"
		" union " 
		" select detail_type,detail_code,1,'N',time_flag,time_cycle,time_unit "
		" from sRandomPresentcfg where region_code=substr(:v3,1,2) and mode_code=:v4 and detail_type<>'1'"
		" and detail_code in (%s) ",tDetailCodeList
		);

	pubLog_Debug(_FFL_,"","","pubRandomBillBeginMode %s ",vSelSql);

	EXEC SQL PREPARE preBeginDet FROM :vSelSql;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127079;
	}
	EXEC SQL DECLARE curBeginDet1 CURSOR FOR preBeginDet;
	EXEC SQL OPEN curBeginDet1 USING :vBelongCode, :iNewMode,:vBelongCode, :iNewMode;
	for(i=0;;){
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
		EXEC SQL FETCH curBeginDet1
			into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
		if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
			exec sql close curBeginDet1;

			return 127061;
		}
		if(i!=0&&SQLCODE==NOTFOUND) break;
		/*****************************************************************************
		 ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
		 ** sBillModeDetail.time_unit 0天/1月/2年                                   **
		 ****************************************************************************/
		Sinitn(vBeginTime);
		Sinitn(vEndTime);
		switch (vTimeFlag[0]){
			case '0':
				strcpy(vBeginTime,iEffectTime);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '1':
				strcpy(vBeginTime,iEffectTime);
				pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime,vIdNo);
				break;
			case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
				strcpy(vEndTime,MAXENDTIME);
				break;
			default:;
		}

		Trim(vBeginTime);
		Trim(vEndTime);
		if (strcmp(vModeTime,"Y") == 0)
		{
			printf("funcflag[%d],iOpCode[%s],vDealFunc[%s]\n",funcflag,iOpCode,vDealFunc);
			/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx start*/
			/*chendx 测试使用1272，没用2976*/
			if(funcflag == 1 && strcmp(iOpCode,"2976") == 0 && strcmp(vDealFunc,"9d") == 0)
			{
				printf("网站进行套餐修改，且该套餐是单功能(手机报)，不需要发送短信提醒！\n");
			}/*R_JLMob_liuyinga_CRM_CMI_2011_0860@关于成晚后向副刊手机报业务流程优化的需求 20111228 chendx end*/
			else
			{
				retValue=DcustModeChgAwake(iOpCode,iNewMode,vBelongCode,"a",atol(iLoginAccept),iPhoneNo,iLoginNo,vBeginTime,vEndTime);
				if(retValue<0){
					EXEC SQL CLOSE curBeginDet1;
					strcpy(oErrMsg,"发送提醒短信错误!");
					PUBLOG_DBDEBUG("pubRandomBillBeginMode");
					pubLog_DBErr(_FFL_, "","127099","[%s] ", oErrMsg);
					return 127099;
				}
			}
		}


		pubLog_Debug(_FFL_,"","","pubRandomBillBeginMode  [%s~%s][%s:%s-%s]*%s* ",\
		vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);


		Sinitn(vBeginSql);
		sprintf(vBeginSql,
			" insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
			" fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
			" login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
			" :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
			":v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
			vIdNo[strlen(vIdNo)-1]);
		EXEC SQL PREPARE preWWW_5 FROM :vBeginSql;
		EXEC SQL EXECUTE preWWW_5 USING :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
			:vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
			:iLoginNo,:iLoginAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if (SQLCODE!=OK){
			sprintf(oErrMsg,"记录dBillCustDetail的历史错误[%d]!",SQLCODE);
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_,"","","pubRandomBillBeginMode %s ",vBeginSql);
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
			exec sql close curBeginDet1;
			return 127062;
		}

		Sinitn(vBeginSql);
		/*ng解耦 by wxcrm at 20090812 begin
		sprintf(vBeginSql,
			"insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,"
			" mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),:v6,:v7,"
			" :v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),:v13,to_number(:v14),substr(:v15,1,2))",
			vIdNo[strlen(vIdNo)-1]);
		#ifdef _WTPRN_
			pubLog_Debug(_FFL_,"","","pubRandomBillBeginMode %s ",vBeginSql);
		#endif
		pubLog_Debug(_FFL_,"","","vIdNo=[%s]  ",vIdNo);
		pubLog_Debug(_FFL_,"","","vDetailType=[%s]  ",vDetailType);
		pubLog_Debug(_FFL_,"","","vDetailCode=[%s]  ",vDetailCode);
		pubLog_Debug(_FFL_,"","","vBeginTime=[%s]  ",vBeginTime);
		pubLog_Debug(_FFL_,"","","vEndTime=[%s]  ",vEndTime);
		pubLog_Debug(_FFL_,"","","iNewMode=[%s]  ",iNewMode);
		pubLog_Debug(_FFL_,"","","iLoginAccept=[%s]  ",iLoginAccept);

		exec sql prepare preWWW_6 from :vBeginSql;
		exec sql execute preWWW_6 using :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,:vFavOrder,:iNewMode,
			:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if(SQLCODE!=OK){
			exec sql close curBeginDet;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		ng解耦 by wxcrm at 20090812 end*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iLoginAccept	,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet1;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		switch (vDetailType[0]){
			/* lixg Add: 2005-11-20 begin */
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
				strcpy(vGrantMsg.modeCode, iNewMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.machineId, iBunchNo);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);
				pubLog_Debug(_FFL_,"","","product Begin ..... ");
				if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
					return iProductRetCode;
				}
				break;
			/* lixg Add: 2005-11-20 end */

			case '0':
				/** process bill system data begin **/
				init(vVpmnGroup);
				EXEC SQL select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate
					from sBillVpmnCond
					where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
				if (SQLROWS==1)
				{
					Trim(vVpmnGroup);


				  	/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseFav(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/

					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}


					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
						to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
						'1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083"," [%s] ", oErrMsg);
						return 127083;
					}
					#endif

					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
						values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
						to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/
					vMocRateNum = vMocRate*1000;
					vMtcRateNum = vMtcRate*1000;
					sprintf(vMocRateStr,"%f",vMocRateNum);
					sprintf(vMtcRateStr,"%f",vMtcRateNum);
					strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
					    '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127083","[%s] ", oErrMsg);

					    return 127083;
					}
					#endif
				}

				if (iFlagStr[0]=='\0'){
					Sinitn(vFather);
					sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
				} else {
					strcpy(vFather,iFlagStr);
				}

				for (;;){
					Sinitn(vUncle);
					Sinitn(vSon);
					pubApartStr(vFather,':',vUncle,vSon);
					if (vSon[0]==vDetailCode[0]&&
							vSon[1]==vDetailCode[1]&&
							vSon[2]==vDetailCode[2]&&
							vSon[3]==vDetailCode[3])
					{
						Sinitn(vFather);
						Sinitn(vUncle);
						pubApartStr(vSon,'^',vFather,vUncle);
						for(;;){
							Sinitn(vUncle);
							Sinitn(vSon);
							pubApartStr(vFather,'^',vUncle,vSon);

							if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");


							pubLog_Debug(_FFL_,"","","iPhoneNo =[%s] ", iPhoneNo);
							pubLog_Debug(_FFL_,"","","favType =[%s] ", vDetailCode);
							pubLog_Debug(_FFL_,"","","flagCode =[%s] ", vSon);
							pubLog_Debug(_FFL_,"","","vBeginTime =[%s] ", vBeginTime);

							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dBaseFav(
								msisdn,fav_brand,fav_type,flag_code,
								fav_order,fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
								substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);
							/*EXEC SQL SELECT
								nvl(:iPhoneNo,'null'),nvl(:vSmCode,'null'),nvl(:vDetailCode,'null'),nvl(:vSon,'null'),nvl(to_char(:vFavOrder),'null'),
								nvl(:vFavDay,'null'),nvl(:vBeginTime,'null'),nvl(:vEndTime,'null'),
								nvl(substr(:vBelongCode,1,2),'null'),nvl(:vIdNo,'null'),nvl(:vGroupId,'null'),nvl(:iNewMode,'null')
								INTO :vTdBaseFav.sMsisdn,		:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,		:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,		:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,	:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,	:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,		:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}*/
							EXEC SQL SELECT count(*)
									INTO :iNum1
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum1!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"取sBillRateCode记录数错误!");
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								
								printf("\n---vDetailCode=%s\n",vDetailCode);
								return 127064;
							}

							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	vSon		);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] %d", oErrMsg,ret);
								
								return 127064;
							}
							#ifdef _CHGTABLE_
							EXEC SQL insert into wBaseFavChg(
								msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
								start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
								to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
								to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if(SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录wBaseFavChg错误!");
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127065","[%s] ", oErrMsg);

								return 127065;
							}
							#endif
							strcpy(vFather,vUncle);
							if(vFather[0]==0) break;
						}
						break;
					}
					strcpy(vFather,vUncle);
					if(vFather[0]=='\0') break;
				}
				/** process bill system data end **/
			case '1':case '2': case '3':case '4':case 'a':
				#ifdef _CHGTABLE_
				EXEC SQL insert into wUserFavChg(
					op_no,op_type,op_time,id_no,detail_type,detail_code,
					begin_time,end_time,fav_order,mode_code)
					values(
					sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
					to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
				if(SQLCODE!=OK){
					exec sql close curBeginDet1;
					strcpy(oErrMsg,"记录wUseFavChg错误!");
					PUBLOG_DBDEBUG("pubRandomBillBeginMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);
					return 127066;
				}
				#endif

				if(vDetailType[0] != 'a') break;

				/** 处理特服绑定的情况 ***/
				vSendFlag=strcmp(vBeginTime,iOpTime);
				Sinitn(vFunctionType);Sinitn(vFunctionCode);
				Sinitn(vOffOn);Sinitn(vCommandCode);


				
				EXEC SQL select a.function_code,a.off_on,b.command_code,b.function_type
					into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
					from sBillFuncBind a,sFuncList b
					where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
					and b.region_code=a.region_code and b.sm_code=a.sm_code
					and b.function_code=a.function_code;
				if (SQLCODE!=OK){
					PUBLOG_DBDEBUG("pubRandomBillBeginMode");
					pubLog_DBErr(_FFL_, "","127067","");
					return 127067;
				}
				pubLog_Debug(_FFL_,"","","vSendFlag=[%d]vFunctionCode[%s] ", vSendFlag,vFunctionCode);
				if (vSendFlag > 0) /***预约 开/关***/
				{
					EXEC SQL select count(*) into :vUserFunc from wCustExpire
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
						and business_status=:vOffOn;
					if (vUserFunc == 0){
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						EXEC SQL insert into wCustExpire(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,ORG_ID)
							values(
							sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iLoginAccept,
							:vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
							to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
						if (SQLCODE != OK){
							PUBLOG_DBDEBUG("pubRandomBillBeginMode");
							pubLog_DBErr(_FFL_, "","127068"," ");
							return 127068;
						}

						EXEC SQL insert into wCustExpireHis(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,ORG_ID)
							select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
							from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iLoginAccept and function_code=:vFunctionCode;
						if (SQLCODE!=OK||SQLROWS!=1){
							PUBLOG_DBDEBUG("pubRandomBillBeginMode");
							pubLog_DBErr(_FFL_, "","127069","");
							return 127069;
						}
					}
				}
				else { /***立即 开/关***/
					EXEC SQL select count(*) into :vUserFunc from dCustFunc
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

					if (vOffOn[0]=='1'){ /** 立即开 ***/
						if(vUserFunc==0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								values(
								to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(vTotalDateStr, 0, sizeof(vTotalDateStr));
							sprintf(vTotalDateStr,"%d",vTotalDate);
							strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}


							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFunc(id_no,function_type,function_code,op_time)
								values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
							ng解耦 by wxcrm at 20090812 end*/
							strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
						}
					}
					else {  /** 立即关 **/
						if (vUserFunc != 0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								select
								id_no,function_type,function_code,op_time,
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
								from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127072","");
								return 127072;
							}
							ng解耦 by wxcrm at 20090812 end*/

							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDateStr,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr50 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur501 CURSOR FOR ng_SqlStr50;
							EXEC SQL OPEN ng_Cur501 using :iLoginNo,:iLoginAccept,:vTotalDateStr,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur501 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubRandomBillBeginMode取dCustFuncHis主键");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127098;
								}

								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubRandomBillBeginMode");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur501;



							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL delete dCustFunc where id_no = to_number(:vIdNo) and function_code = :vFunctionCode;
							if(SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubRandomBillBeginMode取dCustFunc主键");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomBillBeginMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubRandomBillBeginMode");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
						}
					}

					EXEC SQL insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
						op_code,login_no,login_accept)
						values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
						to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
					if (SQLCODE != OK){
						PUBLOG_DBDEBUG("pubRandomBillBeginMode");
						pubLog_DBErr(_FFL_, "","127074","");
						return 127074;
					}

					if (iOpCode[0] != '8') /*** 非专线用户在立即生效时发送开关机命令 ***/
					{
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						
						/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
						if(strcmp(vFunctionCode,"02")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE a.id_no = :vIdNo
														AND a.function_code = '87';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS3");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						if(strcmp(vFunctionCode,"87")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE id_no = :vIdNo
														AND function_code = '02';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS1");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
						else
						{
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomBillBeginMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}

						if(strcmp(vFunctionCode,"95")==0)
						{
							Sinitn(dynStmt);
							sprintf(dynStmt,"insert into wSndCmdDay "
								"("
								"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,"
								"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
								"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT,"
								"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE,org_id, group_id"
								")"
								" select "
								" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, "
								" :v4, :v5, :v6, :v7, :v8, :v9,"
								" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14, to_number(:v15), "
								" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code, :v22, :v23 "
								" from sFuncList "
								" where region_code = substr(:v19, 1,2)"
								"   and sm_code = :v20"
								"   and function_code = :v21");

							if (vOffOn[0]=='1')
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"05");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :iPhoneNo, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId, :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;

								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
							else
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
						}
					}
				}

			default:;
		}
		EXEC SQL DECLARE CurSndcfgC01 CURSOR for
			select a.prom_name,a.deal_func,nvl(a.func_command,' '),nvl(a.op_place,' ')
			from ssndcailcfg a,sRandomPresentcfg b
			where a.region_code = substr(:vBelongCode,1,2) 
			and a.mode_code = :iNewMode
			and a.region_code=b.region_code
			and a.mode_code=b.mode_code
			and a.deal_func=b.deal_func
			and b.detail_code=:vDetailCode;
	    EXEC SQL open CurSndcfgC01;
	    for(i=0;;)
	    {
	      init(vPromName);
	      init(vDealFunc);
	      init(vFuncCommand);
	      init(vOpPlace);
	
	      EXEC SQL fetch CurSndcfgC01 into :vPromName,:vDealFunc,:vFuncCommand,:vOpPlace;
	      if (SQLCODE!=0)
	      {
	      	PUBLOG_DBDEBUG("_dealColorRing");
			pubLog_DBErr(_FFL_, "","-1","no ssndcailcfg[%d],[%s]\n",SQLCODE,SQLERRMSG);
	        break;
	      }
	
	      EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	        	LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
	        	DEAL_TIME,SND_TIMES,prom_name,deal_func,
	        	begin_time,mode_accept,func_command,op_place)
	        values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
	          to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
	          :vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
	          to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
	          to_number(:iLoginAccept),:vFuncCommand,:vOpPlace);
	      if (SQLCODE!=0) 
	      {
	      	PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","","insert wsndcailmsg error");
			EXEC SQL close CurSndcfgC01;
			return 127071;
	      }
	    }
	    EXEC SQL close CurSndcfgC01;
	    
		i++;
	}
	EXEC SQL CLOSE curBeginDet1;

#if 0
	/* 开始处理服务代码 chenxuan boss3 */
	Sinitn(vSelSql);
	strcpy(vSelSql, "select b.service_code, b.srv_net_type, b.on_cmd "
		"from sBillModeSrv a, sSrvCmdRelat b "
		"where a.region_code = substr(:v1,1,2) and a.mode_code = :v2 and a.srv_code = b.service_code "
		"order by b.cmd_order");
	EXEC SQL PREPARE pre FROM :vSelSql;
	EXEC SQL DECLARE cur CURSOR FOR pre;
	EXEC SQL OPEN cur USING :vBelongCode, :iNewMode;
	for (;;) {
		init(vSrvCode); init(vSrvNetType); init(vCmdCode);
		EXEC SQL FETCH cur INTO :vSrvCode, :vSrvNetType, :vCmdCode;
		if (SQLCODE == 1403) break;
		Trim(vCmdCode);

		/* 插入用户使用服务表 */
		EXEC SQL insert into dBillCustSrv (id_no, srv_type, srv_code, begin_time, end_time,
				mode_code, login_accept)
			values(:vIdNo, :vSrvNetType, :vSrvCode, to_date(:vBeginTime, 'YYYYMMDD HH24:MI:SS'),
				to_date(:vEndTime,  'YYYYMMDD HH24:MI:SS'), :iNewMode, :iLoginAccept);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127070","");
			return 127070;
		}

		/* 插入接口相关表 */
		EXEC SQL insert into wSndSrvDay (login_accept, id_no, phone_no, srv_net_type,
				cmd_code, login_no, op_code, op_time, total_date, send_status, send_times, deal_time)
			values (:iLoginAccept, to_number(:vIdNo), :iPhoneNo, :vSrvNetType,
				:vCmdCode, :iLoginNo, :iOpCode, :iOpTime, :vTotalDate, '0', 0, :iOpTime);
		if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","127071","");
			return 127071;
		}
	}
	EXEC SQL CLOSE cur;
	/* 处理服务代码结束 chenxuan boss3 */
#endif

	
	

	/* 开始产品订购数量限制 chenxuan boss3 */
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
		INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
		from sBillModeNum where region_code = :vRegionCode and mode_code = :iNewMode
			and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = :vRegionCode and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100051"," [%s] ", oErrMsg);
			return 100051;
		}
	    Trim(flag);
	    pubLog_Debug(_FFL_,"","","11111111111111111111=[%s] ",flag);
		if ((strcmp(flag,"C")==0) && (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100052","[%s]",oErrMsg);

			return 100052;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0) && (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "，[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100053","[%s]",oErrMsg);
			return 100053;
		}
	}
	/* 产品订购数量限制结束 chenxuan boss3 */

	/*增加全省统一数量控制功能 20081119  begin*/
	init(vBeginTime);
	init(vEndTime);
	init(flag);
	EXEC SQL select max_num, cur_num, buy_num, to_char(begin_time, 'yyyymmdd'), to_char(end_time, 'yyyymmdd'),flag
			  INTO :vMaxNum, :vCurNum, :vBuyNum, :vBeginTime, :vEndTime,:flag
			  from sBillModeNum
			 where region_code = '00' and mode_code = :iNewMode
		       and sysdate > begin_time and sysdate < end_time;
	if (SQLCODE == 0)
	{
		EXEC SQL insert into wbillmodenumopr(REGION_CODE,MODE_CODE,FLAG,OPR_TYPE,OP_TIME)
				 select	region_code,mode_code,flag,'1',sysdate
				  from	sbillmodenum
			     where	region_code = '00' and mode_code = :iNewMode
				   and 	sysdate > begin_time and sysdate < end_time;
		if (SQLCODE != 0)
		{
			strcpy(oErrMsg, "插入表wbillmodenumopr错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100054"," [%s] ", oErrMsg);
			return 100054;
		}
		Trim(flag);
		if ((strcmp(flag,"C")==0)&& (vCurNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]当前套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100055","[%s]", oErrMsg);

			return 100055;
		}
		Trim(flag);
		if ((strcmp(flag,"B")==0)&& (vBuyNum >= vMaxNum))
		{
			sprintf(oErrMsg, "[%s]到[%s]期间，[%s]已订购过的套餐办理数量已经超过了[%d]，不允许继续办理",
			vBeginTime, vEndTime, iNewMode, vMaxNum);
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100056","[%s]", oErrMsg);
			return 100056;
		}
	}
	/*增加全省统一数量控制功能 20081119  end*/

	/**majha 20110630日增加VIP营销 begin
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/
	EXEC Sql select card_code
			into :vNewCardCode
			from sgnsalemode
			where region_code=substr(:vBelongCode,1,2)
			and	mode_code=:iNewMode;
	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		strcpy(oErrMsg, "查询sgnsalemode错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","100057"," [%s] ", oErrMsg);
		return 100057;
	}
	if(SQLCODE==0)
	{
		EXEC SQL select substr(attr_code,3,2)
				into :vOldCardCode
				from dcustmsg
				where phone_no=:iPhoneNo;
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "查询dcustmsg错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100058"," [%s] ", oErrMsg);
			return 100058;
		}
		EXEC SQL insert into tcusthigh_sale
				(login_accept,id_no,phone_no,login_no,op_code,
				mode_code,op_type,old_card_code,new_card_code,
				region_code,op_time,deal_flag)
			values(sMaxSysAccept.nextval,to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,
				  :iNewMode,'a',:vOldCardCode,:vNewCardCode,
				  substr(:vBelongCode,1,2),sysdate,'0');
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录tcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100059"," [%s][%s] ",iLoginAccept, oErrMsg);
			return 100059;
		}
		vCount=0;
		EXEC SQL select count(*)
					into :vCount
					from dcusthigh_sale
					where id_no=to_number(:vIdNo);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录tcusthigh_sale错误!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100060"," [%s] ", oErrMsg);
			return 100060;
		}
		if(vCount==0)
		{
			EXEC SQL insert into dcusthigh_sale
						(id_no,phone_no,init_card_code,op_time)
				values(to_number(:vIdNo),:iPhoneNo,:vOldCardCode,sysdate);
			if (SQLCODE != 0 )
			{
				strcpy(oErrMsg, "记录dcusthigh_sale错误!");
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","100061"," [%s] ", oErrMsg);
				return 100061;
			}
		}

	}
	/**majha 20110630日增加VIP营销 end
	**R_JLMob_liuyinga_CRM_CMI_2011_0267：全网全球通统一资费需求*/
	/*套餐开通时，对于立即生效，并且存在套餐折算表的套餐，记录一张新表（折算短信发送记录表）*/
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from sconvertmode 
				where region_code=substr(:vBelongCode,1,2)
				and mode_code =:iNewMode;
	if (SQLCODE != 0 )
	{
		strcpy(oErrMsg, "查询是否属于折算套餐错误!");
		PUBLOG_DBDEBUG("pubRandomBillBeginMode");
		pubLog_DBErr(_FFL_, "","100064"," [%s] ", oErrMsg);
		return 100064;
	}
	if(vCount>0)
	{
		EXEC SQL INSERT INTO dConverModeMsg
				(login_accept,id_no,phone_no,mode_code,op_time,
				send_flag,login_No,op_code)
			values(to_number(:iLoginAccept),to_number(:vIdNo),:iPhoneNo,:iNewMode,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
				:iSendFlag,:iLoginNo,:iOpCode);
		if (SQLCODE != 0 )
		{
			strcpy(oErrMsg, "记录折算用户信息失败!");
			PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","100065"," [%s] ", oErrMsg);
			return 100065;
		}
	}
	
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubRandomBillBeginMode end ----- ");
#endif
    return 0;
}
/**********************************************
 @wt  PRG  : pubRandomUpdateMode （灵活组包业务受理变更资费信息）
 @wt  FUNC : 变更iPhoneNo的iNewMode资费的赠送业务信息
 @wt 0 iPhoneNo     移动号码
 @wt 1 iNewMode     申请的资费代码
 @wt 2 iEffectTime  生效时间
 @wt 3 iOpCode      业务代码
 @wt 4 iLoginNo     工号
 @wt 5 iLoginAccept 业务流水
 @wt 6 iOpTime      业务时间
 @wt 7 iSendFlag    生效标志
 @wt 8 iOpType      操作类型
 @wt 9 iDetailCode 操作小代码
 @wt 10 oErrMsg      函数内部错误信息
 @wt -----------------------------------------------------
 @wt  数据结果
 @wt      dBillCustDetailX (Inserted)
 @wt      dBillCustDetHisX (Inserted)
 @wt      dBaseFav(inserted) wBaseFavChg(Inserted)
 @wt      wUserFavChg(Inserted)
 @wt      dCustFunc(Inserted/deleted)
 @wt      dCustFuncHis(Inserted)
 @wt      wCustFuncDay(Inserted)
 @wt      wCustExpire(Inserted)
 @wt      wSndCmdDay(Inserted)
 *************************************************/
int pubRandomUpdateMode(
	char *iPhoneNo,
	char *iNewMode,
	char *iEffectTime,
	char *iOpCode,
	char *iLoginNo,
	char *iOldAccept,
	char *iOpTime,
	char *iSendFlag,
	char *iFlagStr,
    char *iBunchNo,
	char *iOpType,
	char *iDetailCode,
	char *iLoginAccept,
	char *oErrMsg)
{
	/* BOSS2.0 lixg add begin: 20070410 */
	CFUNCCONDENTRY fe;
	char tmpRetCode[6+1];
	char vBCtrlCode[5+1];
	char dynStmt[1024];
	

	/* BOSS2.0 lixg add end: 20070410 */
	EXEC SQL BEGIN DECLARE SECTION;
		char    vIdNo[23];
		int     i=0,t=0,vTotalDate=0,vFavOrder=0;
		char    vBelongCode[8],vDetailType[2],vDetailCode[5],vModeTime[2],vTimeFlag[2];
		int     vTimeCycle=0,vTimeUnit=0,vDeltaDays=-1;
		char    vModeFlag[2],vBeginTime[18],vEndTime[18],flag[2];
		char    vBeginSql[1024],vSelSql[1024],vFather[1024],vUncle[1024],vSon[1024];
		char    vFavDay[2],vSmCode[3],vFunctionType[2],vFunctionCode[3],vCommandCode[3],vOffOn[2];
		int     vSendFlag,vUserFunc=-1;
		char    vHlrCode[2],vSimNo[21],vImsiNo[21],vOrgCode[10],vFavBrand[2],vVpmnGroup[11];
		double  vMocRate=0,vMtcRate=0;
		char    vTimeCode[3];
		tUserBase   userBase;
		float       vPrepayFee=0;

		tGrantData vGrantMsg;
		int  iProductRetCode = 0;
		char vLastBIllType[1+1];

		char vAwakeModeName[30+1];
		char vawakemsg[255];
		char useFlag[1+1];
		int	 retValue=0;
		int	 vCount=0;
		char vOrgId[10+1];
		char vNewCardCode[2+1];
		char vOldCardCode[2+1];

		char vPhoneNo1[15+1];
		char vFuncCode1[2+1];

		char vSrvCode[4+1];
		char vSrvNetType[1+1];
		char vCmdCode[256];
		char vGroupId[10+1];
		TPubBillInfo tPubBillInfo;

		char vRegionCode[2+1];
		int  vMaxNum, vCurNum, vBuyNum;
		char vNewType[1+1];
		
		char vDealFunc[2+1];/*功能代码 add by chendx@20111228 */
		int  funcflag = 0;/*单套餐单功能标志，1表示单套餐单功能*/

		/*ng 解耦*/
		char vEffectTime[17+1];
		double  vMocRateNum=0,vMtcRateNum=0;
		char vMocRateStr[8+1];
		char vMtcRateStr[8+1];
		char vFavOrderStr[4+1];
		char vTotalDateStr[8+1];
		int  iNum = 0;
		int	 iNum1 = 0;
		TdBaseFav 			vTdBaseFav;
		TdBaseVpmn 			vTdBaseVpmn;
		TdBillCustDetail 	vTdBillCustDetail;
		TdCustExpire 		vTdCustExpire;
		TdCustFunc			vTdCustFunc;
		TdCustFuncIndex		vTdCustFuncIndex;
		TdCustFuncHis		vTdCustFuncHis;

		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
		int iCount = 0;
		char vNewCmdStr[15 + 1];
		/*Modify at 2012.07.26 end*/
		
		char	vDetailCodeTmp[1024+1];
		char	tDetailCode[4+1];
		char	tDetailCodeList[1024+1];
		char	*retData[80];
		
		char 	v_update_sql[500+1];
		TdBaseFavIndex			vTdBaseFavIndex;
		TdBaseVpmnIndex			vTdBaseVpmnIndex;
		TdBillCustDetailIndex	vTdBillCustDetailIndex;
		char	vEndSql[1024];
		char	iOldMode[8+1];
		int		vAccountOpType=0,vFuncBindFlag=-1,vEndFlag=0,vBeginFlag=0;
		char	vBindOffonFlag[2],vFuncBindCode[3];
		char	vPromName[20+1];
		char	vFuncCommand[256+1];
		char	vOpPlace[3+1];
	EXEC SQL END   DECLARE SECTION;
	
	
	Sinitn(dynStmt);
	Sinitn(vPhoneNo1);
	Sinitn(vFuncCode1);
	Sinitn(vRegionCode);
	Sinitn(vOrgId);
	Sinitn(vEffectTime);
	Sinitn(v_parameter_array);
	Sinitn(vMocRateStr);
	Sinitn(vMtcRateStr);
	Sinitn(vFavOrderStr);
	Sinitn(vNewCardCode);
	Sinitn(vOldCardCode);
	Sinitn(vDealFunc);
	Sinitn(iOldMode);

	Trim(iPhoneNo);
	Trim(iNewMode);
	Trim(iEffectTime);
	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iOldAccept);
	Trim(iLoginAccept);
	Trim(iOpTime);
	Trim(iOpType);
	Trim(iDetailCode);
	
	
	/*Trim(iSendFlag);
	Trim(iFlagStr);*/
	Sinitn(vDetailCodeTmp);
	Sinitn(tDetailCode);
	Sinitn(tDetailCodeList);

	memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
	memset(&vTdBaseVpmn, 0, sizeof(vTdBaseVpmn));
	memset(&vTdBillCustDetail, 0, sizeof(vTdBillCustDetail));
	memset(&vTdCustExpire, 0, sizeof(vTdCustExpire));
	memset(&vTdCustFunc, 0, sizeof(vTdCustFunc));
	memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
	memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
	Sinitn(vNewCmdStr);
	memset(v_update_sql, 0, sizeof(v_update_sql));
	memset(dynStmt, 0, sizeof(dynStmt));
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
	memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
	memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
	memset(vTotalDateStr, 0, sizeof(vTotalDateStr));


	oErrMsg[0] = '\0';

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "",""," begin ----- ");
	pubLog_Debug(_FFL_, "",""," iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "",""," iNewMode=[%s] ",iNewMode);
	pubLog_Debug(_FFL_, "",""," iEffectTime=[%s] ",iEffectTime);
	pubLog_Debug(_FFL_, "",""," iOpCode=[%s]  ",iOpCode);
	pubLog_Debug(_FFL_, "",""," iLoginNo=[%s] ",iLoginNo);
	pubLog_Debug(_FFL_, "",""," iLoginAccept=[%s] ",iLoginAccept);
	pubLog_Debug(_FFL_, "",""," iOpTime=[%s]  ",iOpTime);
	pubLog_Debug(_FFL_, "",""," iSendFlag=[%s] ",iSendFlag);
	pubLog_Debug(_FFL_, "",""," iOpType=[%s] ",iOpType);
	pubLog_Debug(_FFL_, "",""," iDetailCode=[%s] ",iDetailCode);
	pubLog_Debug(_FFL_, "",""," iOldAccept=[%s] ",iOldAccept);
#endif
	
	Sinitn(vIdNo);
	Sinitn(vLastBIllType);
	Trim(iOpTime);
	EXEC SQL select to_char(id_no),belong_code,to_number(substr(:iOpTime,1,8))
		into :vIdNo,:vBelongCode,:vTotalDate
		from dCustMsg where phone_no=:iPhoneNo and substr(run_code,2,1)<'a';
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dCustMsg错误!");
		PUBLOG_DBDEBUG("pubRandomAddMode");
		pubLog_DBErr(_FFL_, "","1024","[%s] ", oErrMsg);
		return 1024;
	}

	strncpy(vRegionCode, vBelongCode, 2);


	int ret =0;
	init(vGroupId);
	ret = sGetUserGroupid(iPhoneNo,vGroupId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取集团用户group_id失败!");
		return 200919;
	}
	Trim(vGroupId);
	
	EXEC SQL select mode_type into :vNewType
		from sBillModeCode where region_code = :vRegionCode and mode_code = :iNewMode;
	if (SQLCODE != 0){
		strcpy(oErrMsg,"查询 mode_type错误!");
		PUBLOG_DBDEBUG("pubRandomAddMode");
		pubLog_DBErr(_FFL_, "","127075","[%s] ", oErrMsg);
		return 127075;
	}


	Trim(vIdNo);
	Sinitn(vHlrCode);
	Sinitn(vImsiNo);Sinitn(vSimNo);

	if(strncmp(iNewMode,"kd",2)!=0 && strncmp(iNewMode,"dl",2)!=0 && strncmp(iNewMode,"ip",2)!=0 && strncmp(iNewMode,"ww",2)!=0
		&& strncmp(iNewMode,"id",2)!=0 && strncmp(iNewMode,"af",2)!=0 && strncmp(iNewMode,"ad",2)!=0 && strncmp(iNewMode,"ma",2)!=0)
	{/*** 专线用户 ***/
		/*增加IMS用户判断 add by zhaohx at 20110526 */
		if(strncmp(iNewMode,"im",2)!=0 && strncmp(iNewMode,"iv",2)!=0 && strncmp(iNewMode,"dm",2)!=0 && strncmp(iNewMode,"mt",2)!=0 && strncmp(iNewMode,"tp",2)!=0)
		{
			EXEC SQL select hlr_code into :vHlrCode from sHlrCode
				where phoneno_head=substr(:iPhoneNo,1,7);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询sHrlCode错误!");
				PUBLOG_DBDEBUG("pubRandomAddMode");
				pubLog_DBErr(_FFL_, "","127076"," [%s] ", oErrMsg);
				return 127076;
			}

			EXEC SQL select switch_no,sim_no into :vImsiNo,:vSimNo from dCustSim
				where id_no=to_number(:vIdNo);
			if(SQLCODE!=OK)
			{
				strcpy(oErrMsg,"查询dCustSim错误!");
				PUBLOG_DBDEBUG("pubRandomAddMode");
				pubLog_DBErr(_FFL_, "","127077","[%s] ", oErrMsg);
				return 127077;
			}
		}
	}
	
	ret =0;
	ret = sGetLoginOrgid(iLoginNo,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_, "","200919","获取用户org_id失败!");
		return 200919;
	}
	Trim(vOrgId);

	Sinitn(vOrgCode);
	EXEC SQL select org_code into :vOrgCode from dLoginMsg where login_no=:iLoginNo;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询dLoginMsg错误!");
		PUBLOG_DBDEBUG("pubRandomAddMode");
		pubLog_DBErr(_FFL_, "","127078"," [%s] ", oErrMsg);
		return 127078;
	}

	Sinitn(vModeFlag);
	Sinitn(vSmCode);
	init(vFavBrand);
	init(vBCtrlCode);
	EXEC SQL select a.mode_flag, a.sm_code, b.fav_brand, nvl(a.before_ctrl_code,' ')
		into :vModeFlag,:vSmCode,:vFavBrand, :vBCtrlCode
		from sBillModeCode a, sSmCode b
		where a.region_code=substr(:vBelongCode,1,2) and a.mode_code=:iNewMode
			and b.region_code=a.region_code and b.sm_code=a.sm_code;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"查询sSmCode.fav_brand错误!");
		PUBLOG_DBDEBUG("pubRandomAddMode");
		pubLog_DBErr(_FFL_, "","127060","[%s] ", oErrMsg);
		return 127060;
	}
	
	
  if(strcmp(iOpType,"A")==0)
  {
	/* 新增--处理产品包含的资费代码 */
	Sinitn(vSelSql);
	sprintf(vSelSql," select detail_type,detail_code,1,'N',time_flag,time_cycle,time_unit "
		" from sRandomPresentcfg where region_code=substr(:v1,1,2) and mode_code=:v2 and detail_type<>'1'"
		" and detail_code =:v3 ");

	pubLog_Debug(_FFL_,"","","pubRandomAddMode %s ",vSelSql);

	EXEC SQL PREPARE preBeginDet FROM :vSelSql;
	if(SQLCODE!=OK){
		strcpy(oErrMsg,"初始化查询sBillModeDetail错误!");
		PUBLOG_DBDEBUG("pubBillBeginMode");
		pubLog_DBErr(_FFL_, "","127079"," [%s] ", oErrMsg);
		return 127079;
	}
	EXEC SQL DECLARE curBeginDet1 CURSOR FOR preBeginDet;
	EXEC SQL OPEN curBeginDet1 USING :vBelongCode,:iNewMode,:iDetailCode;
	for(i=0;;){
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vModeTime);Sinitn(vTimeFlag);
		EXEC SQL FETCH curBeginDet1
			into :vDetailType,:vDetailCode,:vFavOrder,:vModeTime,:vTimeFlag,:vTimeCycle,:vTimeUnit;
		if(i==0&&SQLCODE==NOTFOUND){
			strcpy(oErrMsg,"查询sBillModeDetail明细错误!");
			PUBLOG_DBDEBUG("pubBillBeginMode");
			pubLog_DBErr(_FFL_, "","127061","[%s] ",oErrMsg);
			exec sql close curBeginDet1;

			return 127061;
		}
		if(i!=0&&SQLCODE==NOTFOUND) break;
		/*****************************************************************************
		 ** sBillModeDetail.time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
		 ** sBillModeDetail.time_unit 0天/1月/2年                                   **
		 ****************************************************************************/
		Sinitn(vBeginTime);
		Sinitn(vEndTime);
		switch (vTimeFlag[0]){
			case '0':
				strcpy(vBeginTime,iEffectTime);
				strcpy(vEndTime,MAXENDTIME);
				break;
			case '1':
				strcpy(vBeginTime,iEffectTime);
				pubCompYMD(vBeginTime,vTimeCycle,vTimeUnit,vEndTime,vIdNo);
				break;
			case '2':
				pubCompYMD(iEffectTime,vTimeCycle,vTimeUnit,vBeginTime,vIdNo);
				strcpy(vEndTime,MAXENDTIME);
				break;
			default:;
		}

		Trim(vBeginTime);
		Trim(vEndTime);

		pubLog_Debug(_FFL_,"","","pubRandomAddMode  [%s~%s][%s:%s-%s]*%s* ",\
		vDetailType,vDetailCode,vTimeFlag,vBeginTime,vEndTime,vModeFlag);


		Sinitn(vBeginSql);
		sprintf(vBeginSql,
			" insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,"
			" fav_order,mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,"
			" login_no,login_accept,update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
			" values(to_number(:v1),:v2,:v3,to_date(:v4,'YYYYMMDD HH24:MI:SS'),to_date(:v5,'YYYYMMDD HH24:MI:SS'),"
			" :v6,:v7,:v8,:v9,'Y',:v10,:v11,to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
			":v13,to_number(:v14),:v15,:v16,to_date(:v17,'YYYYMMDD HH24:MI:SS'),:v18,to_number(:v19),'a',substr(:v20,1,2))",
			vIdNo[strlen(vIdNo)-1]);
		EXEC SQL PREPARE preWWW_5 FROM :vBeginSql;
		EXEC SQL EXECUTE preWWW_5 USING :vIdNo,:vDetailType,:vDetailCode,:vBeginTime,:vEndTime,
			:vFavOrder,:iNewMode,:vModeFlag,:vModeTime,:iOpCode,:vTotalDate,:iOpTime,
			:iLoginNo,:iOldAccept,:iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vBelongCode;
		if (SQLCODE!=OK){
			sprintf(oErrMsg,"记录dBillCustDetail的历史错误[%d]!",SQLCODE);
			PUBLOG_DBDEBUG("pubRandomAddMode");
			pubLog_DBErr(_FFL_,"","","pubRandomAddMode %s ",vBeginSql);
			pubLog_DBErr(_FFL_, "","127062", oErrMsg);
			exec sql close curBeginDet1;
			return 127062;
		}
		/*记录dbillcustdetail*/
		sprintf(vFavOrderStr,"%d",vFavOrder);
		sprintf(vTotalDateStr,"%d",vTotalDate);
		strncpy(vTdBillCustDetail.sIdNo			,	vIdNo			,	14	);
		strncpy(vTdBillCustDetail.sDetailType	,	vDetailType		,	1	);
		strncpy(vTdBillCustDetail.sDetailCode	,	vDetailCode		,	4	);
		strncpy(vTdBillCustDetail.sBeginTime	,	vBeginTime		,	17	);
		strncpy(vTdBillCustDetail.sEndTime		,	vEndTime		,	17	);
		strncpy(vTdBillCustDetail.sFavOrder		,	vFavOrderStr	,	4	);
		strncpy(vTdBillCustDetail.sModeCode		,	iNewMode		,	8	);
		strncpy(vTdBillCustDetail.sModeFlag		,	vModeFlag		,	1	);
		strncpy(vTdBillCustDetail.sModeTime		,	vModeTime		,	1	);
		strncpy(vTdBillCustDetail.sModeStatus	,	"Y"				,	1	);
		strncpy(vTdBillCustDetail.sLoginAccept	,	iOldAccept		,	14	);
		strncpy(vTdBillCustDetail.sOpCode		,	iOpCode			,	4	);
		strncpy(vTdBillCustDetail.sTotalDate	,	vTotalDateStr	,	8	);
		strncpy(vTdBillCustDetail.sOpTime		,	iOpTime			,	17	);
		strncpy(vTdBillCustDetail.sLoginNo		,	iLoginNo		,	6	);
		strncpy(vTdBillCustDetail.sRegionCode	,	vRegionCode		,	2	);
		ret = 0;
		ret = OrderInsertBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdBillCustDetail);
		if (ret != 0)
		{
			exec sql close curBeginDet1;
			strcpy(oErrMsg,"记录dBillCustDetail错误!");
			PUBLOG_DBDEBUG("pubRandomAddMode");
			pubLog_DBErr(_FFL_, "","127063","[%s] ", oErrMsg);
			return 127063;
		}
		switch (vDetailType[0]){
			/* lixg Add: 2005-11-20 begin */
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d",  vTotalDate);
				strcpy(vGrantMsg.modeCode, iNewMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.machineId, iBunchNo);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);
				pubLog_Debug(_FFL_,"","","product Begin ..... ");
				if ( (iProductRetCode = sProductApplyFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "","iProductRetCode","");
					return iProductRetCode;
				}
				break;
			/* lixg Add: 2005-11-20 end */

			case '0':
				/** process bill system data begin **/
				init(vVpmnGroup);
				EXEC SQL select group_index,mocrate,mtcrate into :vVpmnGroup,:vMocRate,:vMtcRate
					from sBillVpmnCond
					where region_code=substr(:vBelongCode,1,2) and mode_code=:iNewMode;
				if (SQLROWS==1)
				{
					Trim(vVpmnGroup);


				  	/*记录dBaseFav*/
					strncpy(vTdBaseFav.sMsisdn			,	iPhoneNo		,	15	);
					strncpy(vTdBaseFav.sRegionCode		,	vRegionCode		,	2	);
					strncpy(vTdBaseFav.sFavBrand		,	vSmCode			,	2	);
					strncpy(vTdBaseFav.sFavType			,	"aa00"			,	4	);
					strncpy(vTdBaseFav.sFlagCode		,	vVpmnGroup		,	10	);
					strncpy(vTdBaseFav.sFavOrder		,	"0"				,	1	);
					strncpy(vTdBaseFav.sFavDay			,	"1"				,	1	);
					strncpy(vTdBaseFav.sStartTime		,	vBeginTime		,	17	);
					strncpy(vTdBaseFav.sEndTime			,	vEndTime		,	17	);
					strncpy(vTdBaseFav.sServiceId		,	""				,	11	);
					strncpy(vTdBaseFav.sFavPeriod		,	""				,	1	);
					strncpy(vTdBaseFav.sFreeValue		,	""				,	10	);
					strncpy(vTdBaseFav.sIdNo			,	vIdNo			,	14	);
					strncpy(vTdBaseFav.sGroupId			,	vGroupId		,	10	);
					strncpy(vTdBaseFav.sProductCode		,	iNewMode		,	8	);

					ret = 0;
					ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdBaseFav);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}


					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
						msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
						start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					values(
						:iPhoneNo,:vSmCode,'aa00',:vVpmnGroup,0,'1',
						to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
						'1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode);
					if (SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127083"," [%s] ", oErrMsg);
						return 127083;
					}
					#endif

					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL insert into dBaseVpmn(msisdn,groupid,mocrate,mtcrate,valid,invalid)
						values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
						to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}
					ng解耦 by wxcrm at 20090812 end*/
					vMocRateNum = vMocRate*1000;
					vMtcRateNum = vMtcRate*1000;
					sprintf(vMocRateStr,"%f",vMocRateNum);
					sprintf(vMtcRateStr,"%f",vMtcRateNum);
					strncpy(vTdBaseVpmn.sMsisdn		,	iPhoneNo		,	11	);
					strncpy(vTdBaseVpmn.sGroupId	,	vVpmnGroup		,	10	);
					strncpy(vTdBaseVpmn.sValid		,	iEffectTime		,	17	);
					strncpy(vTdBaseVpmn.sInvalid	,	"20200101"		,	17	);
					strncpy(vTdBaseVpmn.sMocrate 	,	vMocRateStr		,	8	);
					strncpy(vTdBaseVpmn.sMtcrate	,	vMtcRateStr		,	8	);
					ret = 0;
					ret = OrderInsertBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdBaseVpmn);
					if (ret != 0)
					{
						sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127082","[%s] ", oErrMsg);
						return 127082;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					values(:iPhoneNo,:vVpmnGroup,:vMocRate*1000,:vMtcRate*1000,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),to_date('20200101','yyyymmdd'),
					    '1',to_date(:iOpTime,'yyyymmdd hh24:mi:ss'));
					if(SQLCODE!=OK||SQLROWS!=1){
						sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127083","[%s] ", oErrMsg);

					    return 127083;
					}
					#endif
				}

				if (iFlagStr[0]=='\0'){
					Sinitn(vFather);
					sprintf(vFather,"%s^%s^:",vDetailCode,vDetailCode);
				} else {
					strcpy(vFather,iFlagStr);
				}

				for (;;){
					Sinitn(vUncle);
					Sinitn(vSon);
					pubApartStr(vFather,':',vUncle,vSon);
					if (vSon[0]==vDetailCode[0]&&
							vSon[1]==vDetailCode[1]&&
							vSon[2]==vDetailCode[2]&&
							vSon[3]==vDetailCode[3])
					{
						Sinitn(vFather);
						Sinitn(vUncle);
						pubApartStr(vSon,'^',vFather,vUncle);
						for(;;){
							Sinitn(vUncle);
							Sinitn(vSon);
							pubApartStr(vFather,'^',vUncle,vSon);

							if(iSendFlag[0]=='2') strcpy(vFavDay,"1"); else strcpy(vFavDay,"0");


							pubLog_Debug(_FFL_,"","","iPhoneNo =[%s] ", iPhoneNo);
							pubLog_Debug(_FFL_,"","","favType =[%s] ", vDetailCode);
							pubLog_Debug(_FFL_,"","","flagCode =[%s] ", vSon);
							pubLog_Debug(_FFL_,"","","vBeginTime =[%s] ", vBeginTime);

							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dBaseFav(
								msisdn,fav_brand,fav_type,flag_code,
								fav_order,fav_day,start_time,end_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),
								substr(:vBelongCode,1,2),:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(&vTdBaseFav, 0, sizeof(vTdBaseFav));
							Sinitn(vFavOrderStr);
							sprintf(vFavOrderStr,"%d",vFavOrder);
							/*EXEC SQL SELECT
								nvl(:iPhoneNo,'null'),nvl(:vSmCode,'null'),nvl(:vDetailCode,'null'),nvl(:vSon,'null'),nvl(to_char(:vFavOrder),'null'),
								nvl(:vFavDay,'null'),nvl(:vBeginTime,'null'),nvl(:vEndTime,'null'),
								nvl(substr(:vBelongCode,1,2),'null'),nvl(:vIdNo,'null'),nvl(:vGroupId,'null'),nvl(:iNewMode,'null')
								INTO :vTdBaseFav.sMsisdn,		:vTdBaseFav.sFavBrand,
									 :vTdBaseFav.sFavType,		:vTdBaseFav.sFlagCode,
									 :vTdBaseFav.sFavOrder,		:vTdBaseFav.sFavDay,
									 :vTdBaseFav.sStartTime,	:vTdBaseFav.sEndTime,
									 :vTdBaseFav.sRegionCode,	:vTdBaseFav.sIdNo,
									 :vTdBaseFav.sGroupId,		:vTdBaseFav.sProductCode
								FROM sBillRateCode
								WHERE region_code=substr(:vBelongCode,1,2)
								AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								return 127064;
							}*/
							EXEC SQL SELECT count(*)
									INTO :iNum1
									FROM sBillRateCode
									WHERE region_code=substr(:vBelongCode,1,2)
									AND rate_code=:vDetailCode;
							if (SQLCODE!=OK||iNum1!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"取sBillRateCode记录数错误!");
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] ", oErrMsg);
								
								printf("\n---vDetailCode=%s\n",vDetailCode);
								return 127064;
							}

							strcpy(vTdBaseFav.sMsisdn		,	iPhoneNo	);
							strcpy(vTdBaseFav.sFavBrand		,	vSmCode		);
							strcpy(vTdBaseFav.sFavType		,	vDetailCode	);
							strcpy(vTdBaseFav.sFlagCode		,	vSon		);
							strcpy(vTdBaseFav.sFavOrder		,	vFavOrderStr);
							strcpy(vTdBaseFav.sFavDay		,	vFavDay		);
							strcpy(vTdBaseFav.sStartTime	,	vBeginTime	);
							strcpy(vTdBaseFav.sEndTime		,	vEndTime	);
							strcpy(vTdBaseFav.sRegionCode	,	vRegionCode	);
							strcpy(vTdBaseFav.sIdNo			,	vIdNo		);
							strcpy(vTdBaseFav.sGroupId		,	vGroupId	);
							strcpy(vTdBaseFav.sProductCode	,	iNewMode	);

							ret = 0;
							ret = OrderInsertBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdBaseFav);
							if (ret != 0)
							{
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录dBaseFav错误!");
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127064","[%s] %d", oErrMsg,ret);
								
								return 127064;
							}
							#ifdef _CHGTABLE_
							EXEC SQL insert into wBaseFavChg(
								msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
								start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
								select
								:iPhoneNo,:vSmCode,:vDetailCode,:vSon,to_char(:vFavOrder),
								:vFavDay,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
								to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),'1',
								to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,:vIdNo,:vGroupId,:iNewMode
								from sBillRateCode where region_code=substr(:vBelongCode,1,2) and rate_code=:vDetailCode;
							if(SQLCODE!=OK||SQLROWS!=1){
								exec sql close curBeginDet1;
								strcpy(oErrMsg,"记录wBaseFavChg错误!");
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127065","[%s] ", oErrMsg);

								return 127065;
							}
							#endif
							strcpy(vFather,vUncle);
							if(vFather[0]==0) break;
						}
						break;
					}
					strcpy(vFather,vUncle);
					if(vFather[0]=='\0') break;
				}
				/** process bill system data end **/
			case '1':case '2': case '3':case '4':case 'a':
				#ifdef _CHGTABLE_
				EXEC SQL insert into wUserFavChg(
					op_no,op_type,op_time,id_no,detail_type,detail_code,
					begin_time,end_time,fav_order,mode_code)
					values(
					sMaxBillChg.nextval,'1',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),to_number(:vIdNo),:vDetailType,:vDetailCode,
					to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iNewMode);
				if(SQLCODE!=OK){
					exec sql close curBeginDet1;
					strcpy(oErrMsg,"记录wUseFavChg错误!");
					PUBLOG_DBDEBUG("pubRandomAddMode");
					pubLog_DBErr(_FFL_, "","127066","[%s] ", oErrMsg);
					return 127066;
				}
				#endif

				if(vDetailType[0] != 'a') break;

				/** 处理特服绑定的情况 ***/
				vSendFlag=strcmp(vBeginTime,iOpTime);
				Sinitn(vFunctionType);Sinitn(vFunctionCode);
				Sinitn(vOffOn);Sinitn(vCommandCode);

				EXEC SQL select a.function_code,a.off_on,b.command_code,b.function_type
					into :vFunctionCode,:vOffOn,:vCommandCode,:vFunctionType
					from sBillFuncBind a,sFuncList b
					where a.region_code=substr(:vBelongCode,1,2) and a.function_bind=:vDetailCode
					and b.region_code=a.region_code and b.sm_code=a.sm_code
					and b.function_code=a.function_code;
				if (SQLCODE!=OK){
					PUBLOG_DBDEBUG("pubRandomAddMode");
					pubLog_DBErr(_FFL_, "","127067","");
					return 127067;
				}
				pubLog_Debug(_FFL_,"","","pubRandomAddMode:vSendFlag=[%d]vFunctionCode[%s] ", vSendFlag,vFunctionCode);
				
				if (vSendFlag > 0) /***预约 开/关***/
				{
					EXEC SQL select count(*) into :vUserFunc from wCustExpire
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode
						and business_status=:vOffOn;
					if (vUserFunc == 0){
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						EXEC SQL insert into wCustExpire(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,ORG_ID)
							values(
							sOffOn.nextval,:vSmCode,to_number(:vIdNo),:iPhoneNo,:vOrgCode,:iLoginNo,:iOldAccept,
							:vTotalDate,:iOpCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vFunctionCode,:vCommandCode,:vOffOn,
							to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),:iNewMode||'绑定', :vOrgId);
						if (SQLCODE != OK){
							PUBLOG_DBDEBUG("pubRandomAddMode");
							pubLog_DBErr(_FFL_, "","127068"," ");
							return 127068;
						}

						EXEC SQL insert into wCustExpireHis(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type,ORG_ID)
							select command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,command_code,business_status,
							expire_time,op_note,login_no,login_accept,total_date,op_time,op_code,'a', :vOrgId
							from wCustExpire where id_no=to_number(:vIdNo) and login_accept=:iOldAccept and function_code=:vFunctionCode;
						if (SQLCODE!=OK||SQLROWS!=1){
							PUBLOG_DBDEBUG("pubRandomAddMode");
							pubLog_DBErr(_FFL_, "","127069","");
							return 127069;
						}
					}
				}
				else { /***立即 开/关***/
					EXEC SQL select count(*) into :vUserFunc from dCustFunc
						where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;

					if (vOffOn[0]=='1'){ /** 立即开 ***/
						if(vUserFunc==0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								values(
								to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'a');
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(vTotalDateStr, 0, sizeof(vTotalDateStr));
							sprintf(vTotalDateStr,"%d",vTotalDate);
							strncpy(vTdCustFuncHis.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFuncHis.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFuncHis.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFuncHis.sOpTime			,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateLogin		,	iLoginNo		,	6	);
							strncpy(vTdCustFuncHis.sUpdateAccept	,	iLoginAccept	,	14	);
							strncpy(vTdCustFuncHis.sUpdateDate		,	vTotalDateStr	,	8	);
							strncpy(vTdCustFuncHis.sUpdateTime		,	iOpTime			,	17	);
							strncpy(vTdCustFuncHis.sUpdateCode		,	iOpCode			,	4	);
							strncpy(vTdCustFuncHis.sUpdateType		,	"a"				,	1	);
							ret = 0;
							ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdCustFuncHis);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127070","");
								return 127070;
							}


							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFunc(id_no,function_type,function_code,op_time)
								values(to_number(:vIdNo),:vFunctionType,:vFunctionCode,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
							if (SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
							ng解耦 by wxcrm at 20090812 end*/
							strncpy(vTdCustFunc.sIdNo			,	vIdNo			,	14	);
							strncpy(vTdCustFunc.sFunctionType	,	vFunctionType	,	1	);
							strncpy(vTdCustFunc.sFunctionCode	,	vFunctionCode	,	2	);
							strncpy(vTdCustFunc.sOpTime			,	iOpTime			,	17	);
							Trim(vTdCustFunc.sIdNo);
							Trim(vTdCustFunc.sFunctionType);
							Trim(vTdCustFunc.sFunctionCode);
							Trim(vTdCustFunc.sOpTime);
							ret = 0;
							ret = OrderInsertCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdCustFunc);
							if (ret != 0)
							{
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127071","");
								return 127071;
							}
						}
					}
					else {  /** 立即关 **/
						if (vUserFunc != 0){
							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL insert into dCustFuncHis(
								id_no,function_type,function_code,op_time,
								update_login,update_accept,update_date,update_time,update_code,update_type)
								select
								id_no,function_type,function_code,op_time,
								:iLoginNo,:iLoginAccept,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,'d'
								from dCustFunc where id_no=to_number(:vIdNo) and function_code=:vFunctionCode;
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127072","");
								return 127072;
							}
							ng解耦 by wxcrm at 20090812 end*/

							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code,to_char(op_time,'YYYYMMDD HH24:MI:SS'),"
											 "nvl(:iLoginNo,chr(0)),nvl(:iLoginAccept,chr(0)),nvl(:vTotalDateStr,chr(0)),nvl(:iOpTime,chr(0)),nvl(:iOpCode,chr(0)),'d' "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr50 FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur501 CURSOR FOR ng_SqlStr50;
							EXEC SQL OPEN ng_Cur501 using :iLoginNo,:iLoginAccept,:vTotalDateStr,:iOpTime,:iOpCode,:vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncHis, 0, sizeof(vTdCustFuncHis));
								EXEC SQL FETCH ng_Cur501 INTO :vTdCustFuncHis.sIdNo,  			:vTdCustFuncHis.sFunctionType,
															:vTdCustFuncHis.sFunctionCode,  	:vTdCustFuncHis.sOpTime,
															:vTdCustFuncHis.sUpdateLogin,		:vTdCustFuncHis.sUpdateAccept,
															:vTdCustFuncHis.sUpdateDate,		:vTdCustFuncHis.sUpdateTime,
															:vTdCustFuncHis.sUpdateCode,		:vTdCustFuncHis.sUpdateType;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubRandomAddMode取dCustFuncHis主键");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127098;
								}

								ret = 0;
								ret = OrderInsertCustFuncHis("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdCustFuncHis);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubRandomAddMode");
									pubLog_DBErr(_FFL_, "","127072","");
									EXEC SQL Close ng_Cur501;
									return 127072;
								}
							}
							EXEC SQL Close ng_Cur501;



							/*ng解耦 by wxcrm at 20090812 begin
							EXEC SQL delete dCustFunc where id_no = to_number(:vIdNo) and function_code = :vFunctionCode;
							if(SQLCODE!=OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
							ng解耦 by wxcrm at 20090812 end*/
							memset(dynStmt, 0, sizeof(dynStmt));
							sprintf(dynStmt, "SELECT to_char(id_no),function_type,function_code "
											 "FROM dCustFunc "
											 "WHERE id_no = to_number(:v1) and function_code = :v2");
							EXEC SQL PREPARE ng_SqlStr FROM :dynStmt;
							EXEC SQL DECLARE ng_Cur CURSOR FOR ng_SqlStr;
							EXEC SQL OPEN ng_Cur using :vIdNo,:vFunctionCode;
							for(i=0;;)
							{
								memset(&vTdCustFuncIndex, 0, sizeof(vTdCustFuncIndex));
								EXEC SQL FETCH ng_Cur INTO :vTdCustFuncIndex.sIdNo,:vTdCustFuncIndex.sFunctionType,
															:vTdCustFuncIndex.sFunctionCode;
								if (SQLCODE == 1403) break;
								if (SQLCODE != 1403 && SQLCODE != 0)
								{
									PUBLOG_DBDEBUG("pubRandomAddMode取dCustFunc主键");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								strcpy(v_parameter_array[0],vTdCustFuncIndex.sIdNo);
								strcpy(v_parameter_array[1],vTdCustFuncIndex.sFunctionType);
								strcpy(v_parameter_array[2],vTdCustFuncIndex.sFunctionCode);

								ret = 0;
								ret = OrderDeleteCustFunc("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomAddMode",vTdCustFuncIndex,"",v_parameter_array);
								if (ret != 0)
								{
									PUBLOG_DBDEBUG("pubRandomAddMode");
									pubLog_DBErr(_FFL_, "","127073","");
									EXEC SQL Close ng_Cur;
									return 127073;
								}
								iNum++;
							}
							EXEC SQL Close ng_Cur;
							if (iNum == 0)
							{
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127073","");
								return 127073;
							}
						}
					}

					EXEC SQL insert into wCustFuncDay(id_no,function_code,add_flag,total_date,op_time,
						op_code,login_no,login_accept)
						values(to_number(:vIdNo),:vFunctionCode,:vOffOn,:vTotalDate,
						to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iOpCode,:iLoginNo,:iLoginAccept);
					if (SQLCODE != OK){
						PUBLOG_DBDEBUG("pubRandomAddMode");
						pubLog_DBErr(_FFL_, "","127074","");
						return 127074;
					}

					if (iOpCode[0] != '8') /*** 非专线用户在立即生效时发送开关机命令 ***/
					{
						/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
						
						/*Modify for 87,02指令参数调整 at 2012.07.26 begin*/
						if(strcmp(vFunctionCode,"02")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE a.id_no = :vIdNo
														AND a.function_code = '87';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS3");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						if(strcmp(vFunctionCode,"87")==0)
						{
							EXEC SQL SELECT COUNT(1)
												into : iCount
												FROM dcustfunc a
													WHERE id_no = :vIdNo
														AND function_code = '02';
							if(SQLCODE != 0)
							{
								PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","121993","错误提示未定义idNo[%s]",vIdNo);
								return 121993;
							}	
							if(iCount == 0)
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"N");
								}
							}
							else
							{
								if(strcmp(vOffOn,"1")==0)
								{
									strcpy(vNewCmdStr,"BOS1&BOS3");
								}
								else
								{
									strcpy(vNewCmdStr,"BOS1");
								}
							}
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vNewCmdStr,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}
						/*Modify for 87,02指令参数调整 at 2012.07.26 end*/
						else
						{
							EXEC SQL insert into wSndCmdDay(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,command_code,
								new_phone,imsi_no,new_imsi,other_char,op_code,total_date,op_time,login_no,
								org_code,login_accept,request_time,business_status,send_status,send_time,org_id,group_id)
								values(
								sOffOn.nextval,:vHlrCode,0,to_number(:vIdNo),:vBelongCode,:vSmCode,:iPhoneNo,:vCommandCode,
								:iPhoneNo,:vImsiNo,:vImsiNo,:vSimNo,:iOpCode,:vTotalDate,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:iLoginNo,
								:vOrgCode,:iLoginAccept,to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:vOffOn,'0',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'), :vOrgId, :vGroupId);
							if (SQLCODE != OK){
								PUBLOG_DBDEBUG("pubRandomAddMode");
								pubLog_DBErr(_FFL_, "","127075","");
								return 127075;
							}
						}

						if(strcmp(vFunctionCode,"95")==0)
						{
							Sinitn(dynStmt);
							sprintf(dynStmt,"insert into wSndCmdDay "
								"("
								"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,"
								"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
								"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE, LOGIN_ACCEPT,"
								"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE,org_id, group_id"
								")"
								" select "
								" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, "
								" :v4, :v5, :v6, :v7, :v8, :v9,"
								" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14, to_number(:v15), "
								" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code, :v22, :v23 "
								" from sFuncList "
								" where region_code = substr(:v19, 1,2)"
								"   and sm_code = :v20"
								"   and function_code = :v21");

							if (vOffOn[0]=='1')
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"05");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :iPhoneNo, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId, :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;

								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
							else
							{
								Sinitn(vPhoneNo1);
								strcpy(vPhoneNo1,"13800431309");
								Sinitn(vFuncCode1);
								strcpy(vFuncCode1,"61");
								exec sql execute
								begin
									EXECUTE IMMEDIATE :dynStmt USING :vHlrCode, :vIdNo, :vBelongCode,
										:vSmCode, :iPhoneNo, :vPhoneNo1, :vImsiNo,:vImsiNo, :vSimNo,
										:iOpCode, :vTotalDate, :iOpTime,:iLoginNo,:vOrgCode,:iLoginAccept,
										:iOpTime, :vOffOn,:iOpTime,:vOrgId,:vGroupId,  :vBelongCode, :vSmCode, :vFuncCode1;
								end;
								end-exec;
							}
						}
					}
				}

			default:;
		}
		
		EXEC SQL DECLARE CurSndcfgC01 CURSOR for
			select a.prom_name,a.deal_func,nvl(a.func_command,' '),nvl(a.op_place,' ')
			from ssndcailcfg a,sRandomPresentcfg b
			where a.region_code = substr(:vBelongCode,1,2) 
			and a.mode_code = :iNewMode
			and a.region_code=b.region_code
			and a.mode_code=b.mode_code
			and a.deal_func=b.deal_func
			and b.detail_code=:vDetailCode;
	    EXEC SQL open CurSndcfgC01;
	    for(i=0;;)
	    {
	      init(vPromName);
	      init(vDealFunc);
	      init(vFuncCommand);
	      init(vOpPlace);
	
	      EXEC SQL fetch CurSndcfgC01 into :vPromName,:vDealFunc,:vFuncCommand,:vOpPlace;
	      if (SQLCODE!=0)
	      {
	      	PUBLOG_DBDEBUG("_dealColorRing");
			pubLog_DBErr(_FFL_, "","-1","no ssndcailcfg[%d],[%s]\n",SQLCODE,SQLERRMSG);
	        break;
	      }

		  vCount=0;
		  EXEC SQL SELECT count(*) 
				into :vCount
				from wsndcailmsg
				where mode_accept=to_number(:iOldAccept)
				and id_no=to_number(:vIdNo)
    			and deal_func=:vDealFunc
    			and func_command=rpad(:vFuncCommand,256)
    			and op_type='d';
	      if(vCount>0)
	      {
	      	EXEC SQL delete wsndcailmsg
	      		where mode_accept=to_number(:iOldAccept)
	      		and id_no=to_number(:vIdNo)
    			and deal_func=:vDealFunc
    			and func_command=rpad(:vFuncCommand,256)
    			and op_type='d';
			if (SQLCODE!=0) 
			{
				PUBLOG_DBDEBUG("pubRandomBillBeginMode");
				pubLog_DBErr(_FFL_, "","","delete wsndcailmsg error");
				EXEC SQL close CurSndcfgC01;
				return 127071;
			}
	      }
	      EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
	        	LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
	        	DEAL_TIME,SND_TIMES,prom_name,deal_func,
	        	begin_time,mode_accept,func_command,op_place)
	        values(to_number(:iLoginAccept),mod(:iLoginAccept,10),
	          to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
	          :vTotalDate,'a',sysdate,0,:vPromName,:vDealFunc,
	          to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),
	          to_number(:iOldAccept),:vFuncCommand,:vOpPlace);
	      if (SQLCODE!=0) 
	      {
	      	PUBLOG_DBDEBUG("pubRandomBillBeginMode");
			pubLog_DBErr(_FFL_, "","","insert wsndcailmsg error");
			EXEC SQL close CurSndcfgC01;
			return 127071;
	      }
	    }
	    EXEC SQL close CurSndcfgC01;
	    
		i++;
	}
	EXEC SQL CLOSE curBeginDet1;
  }
  else if(strcmp(iOpType,"D")==0)
  {
	strcpy(iOldMode,iNewMode);

	oErrMsg[0] = '\0';

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode begin -----");
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iPhoneNo=[%s] -----",iPhoneNo);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iOldMode=[%s] -----",iOldMode);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iOldAccept=[%s] -----",iOldAccept);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iEffectTime=[%s] -----",iEffectTime);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iOpCode=[%s] -----",iOpCode);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iLoginNo=[%s] -----",iLoginNo);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iLoginAccept=[%s] -----",iLoginAccept);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iOpTime=[%s] -----",iOpTime);
	pubLog_Debug(_FFL_, "", "", "pubRandomDeleteMode iDetailCode=[%s] -----",iDetailCode);
#endif


	Sinitn(vSelSql);
	sprintf(vSelSql,"select detail_type,detail_code,to_char(begin_time,'yyyymmdd hh24:mi:ss'),fav_order,"
		" to_char(end_time,'yyyymmdd hh24:mi:ss'),mode_time from dBillCustDetail%c where id_no=to_number(:v1) "
		" and mode_code=:v2 and login_accept=to_number(:v3) and detail_code=:v4 "
		" order by mode_time desc",vIdNo[strlen(vIdNo)-1]);
#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode %s",vSelSql);
#endif
	EXEC SQL PREPARE preModeDet FROM :vSelSql;
	EXEC SQL DECLARE curModeDet CURSOR FOR preModeDet;
	EXEC SQL OPEN curModeDet USING :vIdNo,:iOldMode,:iOldAccept,:iDetailCode;
	if(SQLCODE!=OK)
	{
		strcpy(oErrMsg,"查询dBillCustDetail初始化错误");
		PUBLOG_DBDEBUG("pubRandomDeleteMode");
		pubLog_DBErr(_FFL_, "", "127040", "pubRandomDeleteMode :open (select dBillModeDetail) error[%s]",oErrMsg);
		return 127040;
	}
	printf(":vIdNo = %s,:iOldMode = %s,:iOldAccept =%s iDetailCode=%s\n",vIdNo,iOldMode,iOldAccept,iDetailCode);
	for(i=0;;)
	{
		Sinitn(vDetailType);Sinitn(vDetailCode);Sinitn(vBeginTime);Sinitn(vEndTime);
		EXEC SQL FETCH curModeDet INTO :vDetailType,:vDetailCode,:vBeginTime,:vFavOrder,:vEndTime,:vModeTime;
		printf(":vDetailType = %s,:vDetailCode = %s,:vBeginTime =%s,:vFavOrder = %d,:vEndTime = %s,:vModeTime =%s\n",vDetailType,vDetailCode,vBeginTime,vFavOrder,vEndTime,vModeTime);

		if((SQLCODE!=OK&&SQLCODE!=NOTFOUND)||(SQLCODE==NOTFOUND&&i==0))
		{
			EXEC SQL CLOSE curModeDet;
			strcpy(oErrMsg,"查询dBillCustDetail明细错误");
			PUBLOG_DBDEBUG("pubRandomDeleteMode");
			pubLog_DBErr(_FFL_, "", "127041", "pubRandomDeleteMode :fetch (select dBillModeDetail) error[%s] SQLCODE= %d",oErrMsg,SQLCODE);

			return 127041;
		}
		if(SQLCODE==NOTFOUND&&i>0) break;
		
		/*** vBeginFlag 记录生效时间与要取消套餐的开始时间比较后的关系 ***/
		/*if (strcmp(vModeTime,"Y")==0){			
			vBeginFlag = strcmp(iEffectTime,vBeginTime);
		}*/
		vBeginFlag=0;
		vBeginFlag = strcmp(iEffectTime,vBeginTime);
		/*** vEndFlag 记录生效时间与要取消套餐的结束时间比较后的关系 ***/
		vEndFlag=strcmp(vEndTime,iEffectTime);
		
		pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "iEffectTime %s vEndTime %s",iEffectTime,vEndTime);
		pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "vBeginTime %s",vBeginTime);
		pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "vBeginFlag %d",vBeginFlag);
		pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "vEndFlag %d",vEndFlag);
		
		if(vBeginFlag <= 0)
		{
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'d',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10 ",
				vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
			EXEC SQL PREPARE preWWW_1 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_1 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,
				:iLoginAccept,:vIdNo,:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				sprintf(oErrMsg,"记录dBillCustDetHis记录历史明细错误![%d]",SQLCODE);
				PUBLOG_DBDEBUG("pubRandomDeleteMode");
				pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :%s",vEndSql);
				pubLog_DBErr(_FFL_, "", "127042", "[%s][%s]",SQLROWS,oErrMsg);
				EXEC SQL CLOSE curModeDet;
				return 127042;
			}
			
			/*删除dbillcustdetail记录*/
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));
			memset(v_parameter_array, 0, sizeof(v_parameter_array));
			
			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_parameter_array[0],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[1],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[2],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[3],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[4],vTdBillCustDetailIndex.sDetailCode);

			ret = 0;
			ret = OrderDeleteBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBillCustDetailIndex,"",v_parameter_array);
			if (ret != 0)
			{
			    strcpy(oErrMsg,"删除dBillCustDetail无效记录时错误!");
			    PUBLOG_DBDEBUG("pubRandomDeleteMode");
			    pubLog_DBErr(_FFL_, "", "127043", "[%s]",oErrMsg);
			    exec sql close curModeDet;
			    return 127043;
			}
		}
		else 
		{
			Sinitn(vEndSql);
			sprintf(vEndSql,"insert into dBillCustDetHis%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,"
				" mode_code,mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" update_code,update_date,update_time,update_login,update_accept,update_type,region_code) "
				" select id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,mode_flag,mode_time,"
				" mode_status,op_code,total_date,op_time,login_no,login_accept,"
				" :v1,:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),:v4,to_number(:v5),'u',region_code from dBillCustDetail%c "
				" where id_no=to_number(:v6) and mode_code=:v7 and login_accept=to_number(:v8) and detail_type=:v9 and detail_code=:v10",vIdNo[strlen(vIdNo)-1],vIdNo[strlen(vIdNo)-1]);
			#ifdef _WTPRN_
				pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :%s",vEndSql);
			#endif
			EXEC SQL PREPARE preWWW_3 FROM :vEndSql;
			EXEC SQL EXECUTE preWWW_3 USING :iOpCode,:vTotalDate,:iOpTime,:iLoginNo,:iLoginAccept,:vIdNo,
				:iOldMode,:iOldAccept,:vDetailType,:vDetailCode;
			if (SQLCODE!=OK||SQLROWS!=1){
				strcpy(oErrMsg,"更改dBillCustDetail时记录历史明细错误!");
			    pubLog_DBErr(_FFL_, "SQLCODE = [%d]", "127044", "[%s]",SQLCODE, oErrMsg);
				EXEC SQL CLOSE curModeDet;
				PUBLOG_DBDEBUG("pubRandomDeleteMode");
				return 127044;
			}
			Sinitn(vEndSql);
			


			/*更新dbillcustdetail记录*/
			memset(&vTdBillCustDetailIndex, 0, sizeof(vTdBillCustDetailIndex));

			strncpy(vTdBillCustDetailIndex.sIdNo,			vIdNo,			14	);
			strncpy(vTdBillCustDetailIndex.sModeCode,		iOldMode,		8	);
			strncpy(vTdBillCustDetailIndex.sLoginAccept,	iOldAccept,		14	);
			strncpy(vTdBillCustDetailIndex.sDetailType,		vDetailType,	1	);
			strncpy(vTdBillCustDetailIndex.sDetailCode,		vDetailCode,	4	);

			Trim(vTdBillCustDetailIndex.sIdNo);
			Trim(vTdBillCustDetailIndex.sModeCode);
			Trim(vTdBillCustDetailIndex.sLoginAccept);
			Trim(vTdBillCustDetailIndex.sDetailType);
			Trim(vTdBillCustDetailIndex.sDetailCode);

			memset(v_parameter_array, 0, sizeof(v_parameter_array));
			sprintf(vTotalDateStr,"%d",vTotalDate);

			strcpy(v_parameter_array[0],iEffectTime);
			strcpy(v_parameter_array[1],iEffectTime);
			strcpy(v_parameter_array[2],iOpCode);
			strcpy(v_parameter_array[3],vTotalDateStr);
			strcpy(v_parameter_array[4],iOpTime);
			strcpy(v_parameter_array[5],iLoginNo);
			strcpy(v_parameter_array[6],vTdBillCustDetailIndex.sIdNo);
			strcpy(v_parameter_array[7],vTdBillCustDetailIndex.sModeCode);
			strcpy(v_parameter_array[8],vTdBillCustDetailIndex.sLoginAccept);
			strcpy(v_parameter_array[9],vTdBillCustDetailIndex.sDetailType);
			strcpy(v_parameter_array[10],vTdBillCustDetailIndex.sDetailCode);

			strcpy(v_update_sql,"mode_status='N',end_time=decode(sign(end_time-to_date(:v20,'YYYYMMDD HH24:MI:SS')),1,to_date(:v21,'YYYYMMDD HH24:MI:SS'),end_time), op_code=:v22,total_date=:v23,op_time=to_date(:v24,'yyyymmdd hh24:mi:ss'),login_no=:v25 ");
			Trim(v_update_sql);

			ret = 0;
			ret = OrderUpdateBillCustDetail("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBillCustDetailIndex,vTdBillCustDetailIndex,v_update_sql,"",v_parameter_array);

			if (ret != 0)
			{
				exec sql close curModeDet;
			 	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			    pubLog_DBErr(_FFL_, "", "127045", "[%s]",oErrMsg);
				strcpy(oErrMsg,"更改dBillCustDetail错误!");
				return 127045;
			}

#ifdef _WTPRN_
	pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :update(for/u)dBillCustDetails%c%s",vIdNo[strlen(vIdNo)-1],vEndSql);
#endif
		}
		
		pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :vDetailType %s",vDetailType);
		switch(vDetailType[0])
		{
			case '0':
				/*** process bill system data begin ***/
				if (vEndFlag > 0){
					/*更新dbasefav*/
					EXEC SQL SELECT msisdn, fav_type, flag_code, to_char(start_time,'YYYYMMDD HH24:MI:SS')
							 INTO :vTdBaseFavIndex.sMsisdn,:vTdBaseFavIndex.sFavType,:vTdBaseFavIndex.sFlagCode,:vTdBaseFavIndex.sStartTime
							 FROM dBaseFav
							 WHERE msisdn=:iPhoneNo and fav_type=:vDetailCode
							 AND start_time=TO_DATE(:vBeginTime,'YYYYMMDD HH24:MI:SS');

					if(SQLCODE!=OK||SQLROWS!=1)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubRandomDeleteMode");
						sprintf(oErrMsg,"查询dBaseFav错误![%s][%s]",vDetailCode,vBeginTime);
			   			pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}
					memset(v_parameter_array, 0, sizeof(v_parameter_array));
					strcpy(v_parameter_array[0],iEffectTime);
					strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
					strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
					strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
					strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);
					ret = 0;
					ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:v11,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
					if (ret != 0)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubRandomDeleteMode");
						strcpy(oErrMsg,"更改dBaseFav错误!");
			   			pubLog_DBErr(_FFL_, "", "127046", "[%s]",oErrMsg);
						return 127046;
					}

					#ifdef _CHGTABLE_
					EXEC SQL insert into wBaseFavChg(
					    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					    start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
						select
					    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					    start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
					        region_code,id_no,group_id,product_code
						from dBaseFav where msisdn=:iPhoneNo and fav_type=:vDetailCode
					    and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					if(SQLCODE!=OK||SQLROWS!=1)
					{
						exec sql close curModeDet;
						PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   			pubLog_DBErr(_FFL_, "", "127047", "[%s]",oErrMsg);
						strcpy(oErrMsg,"记录wBaseFavChg错误!");
						return 127047;
					}
					#endif
					/*ng解耦 by wxcrm at 20090812 begin
					EXEC SQL delete dBaseFav where msisdn=:iPhoneNo and fav_type=:vDetailCode
					    and start_time>=end_time;
					if(SQLCODE!=OK&&SQLCODE!=NOTFOUND)
					{
						exec sql close curModeDet;
						strcpy(oErrMsg,"清除dBaseFav无效记录时错误!");
						PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   			pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);

						return 127049;
					}
					ng解耦 by wxcrm at 20090812 end*/
					memset(dynStmt, 0, sizeof(dynStmt));
					sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
									 "FROM dBaseFav "
									 "WHERE msisdn=:v1 and fav_type=:v2 and start_time>=end_time");
					EXEC SQL PREPARE ng_SqlStr3 FROM :dynStmt;
					EXEC SQL DECLARE ng_Cur3 CURSOR FOR ng_SqlStr3;
					EXEC SQL OPEN ng_Cur3 using :iPhoneNo,:vDetailCode;
					for(i=0;;)
					{
						memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
						EXEC SQL FETCH ng_Cur3 INTO :vTdBaseFavIndex;
						if (SQLCODE == 1403) break;
						if (SQLCODE != 1403 && SQLCODE != 0)
						{
							strcpy(oErrMsg,"查询dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur3;
							EXEC SQL CLOSE curModeDet;
							return 127049;
						}
						memset(v_parameter_array, 0, sizeof(v_parameter_array));
						strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
						ret = 0;
						ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
						if (ret != 0)
						{
							strcpy(oErrMsg,"清除dBaseFav无效记录时错误!");
							PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127049", "[%s]", oErrMsg);
							EXEC SQL CLOSE ng_Cur3;
							EXEC SQL CLOSE curModeDet;
							return 127049;
						}
					}
					EXEC SQL CLOSE ng_Cur3;


					/*** 处理聊天套餐默认退出vmpn集团 ***/
					init(vVpmnGroup);
					EXEC SQL select group_index into :vVpmnGroup from sBillVpmnCond
						where region_code=substr(:vBelongCode,1,2) and mode_code=:iOldMode;
					if(SQLROWS==1)
					{
					    Trim(vVpmnGroup);
					    #ifdef _CHGTABLE_
					    exec sql insert into wBaseVpmnChg(msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time)
					    select msisdn,groupid,mocrate,mtcrate,valid,
					    to_date(:iEffectTime,'yyyymmdd hh24:mi:ss'),'2',to_date(:iOpTime,'yyyymmdd hh24:mi:ss')
					    from dBaseVpmn where msisdn=:iPhoneNo and groupid=:vVpmnGroup
					    and valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理wBaseVpmnChg默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127081", " [%s]", oErrMsg);

					        return 127081;
					    }
					    #endif
					    /*ng解耦 by wxcrm at 20090813 begin
					    exec sql update dBaseVpmn set invalid=to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')
					    where msisdn=:iPhoneNo and groupid=:vVpmnGroup
					    and valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);

					        return 127080;
					    }
					    ng解耦 by wxcrm at 20090813 end*/
					    EXEC SQL SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS')
					    		 INTO :vTdBaseVpmnIndex
					    		 FROM dBaseVpmn
					    		 WHERE msisdn=:iPhoneNo
					    		 AND groupid=:vVpmnGroup
					    		 AND valid=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK||SQLROWS!=1)
					    {
					    	sprintf(oErrMsg,"处理询dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }

					    Trim(vTdBaseVpmnIndex.sMsisdn);
					    Trim(vTdBaseVpmnIndex.sGroupId);
					    Trim(vTdBaseVpmnIndex.sValid);

					    memset(v_parameter_array, 0, sizeof(v_parameter_array));
					    strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sGroupId);
						strcpy(v_parameter_array[3],vTdBaseVpmnIndex.sValid);

						ret = 0;
					    ret = OrderUpdateBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseVpmnIndex,vTdBaseVpmnIndex,"invalid=to_date(:iEffectTime,'yyyymmdd hh24:mi:ss')","",v_parameter_array);
					    if (ret != 0)
					    {
					    	sprintf(oErrMsg,"处理dBaseVpmn默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }


					    /*ng解耦 by wxcrm at 20090813 begin
					    exec sql delete dBaseVpmn where msisdn=:iPhoneNo and groupid=:vVpmnGroup and valid>=invalid;
						ng解耦 by wxcrm at 20090813 end*/
						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,groupid,to_char(valid,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseVpmn "
										 "WHERE msisdn=:iPhoneNo and groupid=:vVpmnGroup and valid>=invalid");
						EXEC SQL PREPARE ng_SqlStr5 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur5 CURSOR FOR ng_SqlStr5;
						EXEC SQL OPEN ng_Cur5 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
						{
							memset(&vTdBaseVpmnIndex, 0, sizeof(vTdBaseVpmnIndex));
							EXEC SQL FETCH ng_Cur5 INTO :vTdBaseVpmnIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}

						    Trim(vTdBaseVpmnIndex.sMsisdn);
						    Trim(vTdBaseVpmnIndex.sGroupId);
						    Trim(vTdBaseVpmnIndex.sValid);

							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseVpmnIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseVpmnIndex.sGroupId);
							strcpy(v_parameter_array[2],vTdBaseVpmnIndex.sValid);

							ret = 0;

							/*******************ng解耦 liuzhou 只删除dbasevpmn表中记录，不插wBaseVpmnChg  *****************/
							/*ret = OrderDeleteBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);*/
							ret = OrderDeleteOtherBaseVpmn("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseVpmnIndex,"and valid>=invalid",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur5;


						/*ng解耦 by wxcrm at 20090812 begin
					    exec sql update dBaseFav set end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')
					    where msisdn=:iPhoneNo and fav_type='aa00' and flag_code=:vVpmnGroup
					    and  start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK)
					    {
					    	sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
					    }
					    ng解耦 by wxcrm at 20090812 end*/
					    memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
					    strncpy(vTdBaseFavIndex.sMsisdn		,iPhoneNo	,	15	);
					    strncpy(vTdBaseFavIndex.sFavType	,"aa00"		,	4	);
					    strncpy(vTdBaseFavIndex.sFlagCode	,vVpmnGroup	,	10	);
					    strncpy(vTdBaseFavIndex.sStartTime	,vBeginTime	,	17	);

					    Trim(vTdBaseFavIndex.sMsisdn);
					    Trim(vTdBaseFavIndex.sFavType);
					    Trim(vTdBaseFavIndex.sFlagCode);
					    Trim(vTdBaseFavIndex.sStartTime);

					    memset(v_parameter_array, 0, sizeof(v_parameter_array));
					    strcpy(v_parameter_array[0],iEffectTime);
						strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
						strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
						strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
						strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);

						ret = 0;
						ret = OrderUpdateBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
						if (ret != 0)
						{
							sprintf(oErrMsg,"处理 dBaseFav 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127080", " [%s]", oErrMsg);
					        return 127080;
						}
					    #ifdef _CHGTABLE_
					    exec sql insert into wBaseFavChg(
					        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					        start_time,end_time,flag,deal_time,region_code,id_no,group_id,product_code)
					    select
					        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
					        start_time,end_time,'2',to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),region_code,
					            id_no,group_id,product_code
					    from dBaseFav where msisdn=:iPhoneNo and fav_type='aa00' and flag_code=:vVpmnGroup
					        and start_time=to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS');
					    if(SQLCODE!=OK)
					    {
					    	sprintf(oErrMsg,"处理 wBaseFavChg 默认集团%s时错误!",vVpmnGroup);
					    	PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   				pubLog_DBErr(_FFL_, "", "127081", "[%s]", oErrMsg);
					        return 127081;
					    }
					    #endif
					    /*ng解耦 by wxcrm at 20090812 begin
					    exec sql delete dBaseFav where msisdn=:iPhoneNo and fav_type='aa00'
					        and flag_code=:vVpmnGroup and start_time>=end_time;
						ng解耦 by wxcrm at 20090812 end*/
						memset(dynStmt, 0, sizeof(dynStmt));
						sprintf(dynStmt, "SELECT msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
										 "FROM dBaseFav "
										 "WHERE msisdn=:v1 and fav_type='aa00' and flag_code=:v2 and start_time>=end_time");

						EXEC SQL PREPARE ng_SqlStr4 FROM :dynStmt;
						EXEC SQL DECLARE ng_Cur4 CURSOR FOR ng_SqlStr4;
						EXEC SQL OPEN ng_Cur4 using :iPhoneNo,:vVpmnGroup;
						for(i=0;;)
					    {
					    	memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
							EXEC SQL FETCH ng_Cur4 INTO :vTdBaseFavIndex;
							if (SQLCODE == 1403) break;
							if (SQLCODE != 1403 && SQLCODE != 0)
							{
								break;
							}
							memset(v_parameter_array, 0, sizeof(v_parameter_array));
							strcpy(v_parameter_array[0],vTdBaseFavIndex.sMsisdn);
							strcpy(v_parameter_array[1],vTdBaseFavIndex.sFavType);
							strcpy(v_parameter_array[2],vTdBaseFavIndex.sFlagCode);
							strcpy(v_parameter_array[3],vTdBaseFavIndex.sStartTime);
							ret = 0;
							ret = OrderDeleteOtherBaseFav("2",vIdNo,100,iOpCode,atol(iLoginAccept),iLoginNo,"pubRandomDeleteMode",vTdBaseFavIndex,"and start_time>=end_time",v_parameter_array);
							if (ret != 0)
							{
								break;
							}
						}
						EXEC SQL Close ng_Cur4;
					}
				}
				/*** process bill system data end ***/
			case '1':case '2': case '3':case '4':case 'a':
				/*** process account system data ***/
				if(vBeginFlag<=0) vAccountOpType=3; else vAccountOpType=2;

				if(vEndFlag >= 0){
					#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
						op_no,op_type,op_time,
						id_no,detail_type,detail_code,begin_time,
						end_time,fav_order,mode_code)
					values(
						sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
						to_date(:iEffectTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE != OK){
						sprintf(oErrMsg,"记录wUserFavChg时错误![%d]",SQLCODE);
						PUBLOG_DBDEBUG("pubRandomDeleteMode");
			   			pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
			   			exec sql close curModeDet;
						return 127048;
					}
					#endif
				}
				else {
					#ifdef _CHGTABLE_
					exec sql insert into wUserFavChg(
					    op_no,op_type,op_time,
					    id_no,detail_type,detail_code,begin_time,
					    end_time,fav_order,mode_code)
					values(
					    sMaxBillChg.nextval,to_char(:vAccountOpType),to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),
					    to_number(:vIdNo),:vDetailType,:vDetailCode,to_date(:vBeginTime,'YYYYMMDD HH24:MI:SS'),
					    to_date(:vEndTime,'YYYYMMDD HH24:MI:SS'),:vFavOrder,:iOldMode);
					if (SQLCODE!=OK){
					  strcpy(oErrMsg,"记录wUserFavChg时错误!");
					  PUBLOG_DBDEBUG("pubRandomDeleteMode");
					  pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
					  exec sql close curModeDet;
					  return 127048;
					}
					#endif
				}

				if(vDetailType[0]!='a') break;
				/** 对于特服绑定的取消,判断该用户是否还有别的套餐有相同的绑定,
				    如果没有,则取消该特服绑定的预约记录  **/
				EXEC SQL select count(*) into :vFuncBindFlag
					from dBillCustDetail where id_no=to_number(:vIdNo) and detail_type='a'
					and detail_code=:vDetailCode and end_time>to_date(:iEffectTime,'yyyymmdd hh24:mi:ss');
				pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :vIdNo %s vDetailCode %s vFuncBindFlag %d iEffectTime %s ",vIdNo,vDetailCode,vFuncBindFlag,iEffectTime);
				if(vFuncBindFlag==0)
				{
					init(vBindOffonFlag);init(vFuncBindCode);
					EXEC SQL select function_code,off_on into :vFuncBindCode,:vBindOffonFlag from sBillFuncBind
					where region_code=substr(:vBelongCode,1,2) and function_bind=:vDetailCode;
				
				pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "pubRandomDeleteMode :vBindOffonFlag %s  vFuncBindCode %s iOldAccept %s",vBindOffonFlag,vFuncBindCode,iOldAccept);
				/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
					EXEC SQL insert into wCustExpireHis(
						command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,update_login,update_accept,update_date,update_time,update_code,update_type, ORG_ID)
						SELECT command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
						total_date,op_code,op_time,function_code,command_code,business_status,
						expire_time,op_note,:iLoginNo,to_number(:iLoginAccept),:vTotalDate,to_date(:iOpTime,'yyyymmdd hh24:mi:ss'),
						:iOpCode,'d', ORG_ID
						from wCustExpire where id_no=to_number(:vIdNo) and function_code=:vFuncBindCode
						and business_status=:vBindOffonFlag and login_accept=:iOldAccept;

					EXEC SQL delete wCustExpire where id_no=to_number(:vIdNo)
						and function_code=:vFuncBindCode and business_status=:vBindOffonFlag and login_accept=to_number(:iOldAccept);
				}
				break;

			/**
			* modify Time: 2005-11-20
			* modify author: lixg
			* modify Resion: 吉林产品改造升级 begin
			*/
			case PROD_SENDFEE_CODE: /* 缴费回馈产品 */
			case PROD_MACHFEE_CODE: /* 买手机送话费产品 */
				pubLog_Debug(_FFL_, "pubRandomDeleteMode", "", "回退缴费回馈、买手机送话费产品信息。。。");
				memset(&vGrantMsg, 0, sizeof(vGrantMsg));
				strcpy(vGrantMsg.applyType, vDetailType);
				strcpy(vGrantMsg.phoneNo, iPhoneNo);
				strcpy(vGrantMsg.loginNo, iLoginNo);
				strcpy(vGrantMsg.opCode, iOpCode);
				strcpy(vGrantMsg.opNote, " ");
				strcpy(vGrantMsg.opTime, iOpTime);
				sprintf(vGrantMsg.totalDate, "%d", vTotalDate);
				strcpy(vGrantMsg.modeCode, iOldMode);
				strcpy(vGrantMsg.detModeCode, vDetailCode);
				strcpy(vGrantMsg.oldLoginAccept, iOldAccept);
				strcpy(vGrantMsg.loginAccept, iLoginAccept);

				if ( (iProductRetCode = sProductCancelFunc(vGrantMsg, oErrMsg)) != 0 ){
					pubLog_DBErr(_FFL_, "", "iProductRetCode", "");
					return iProductRetCode;
				}

				break;
				/* end */
			default:;
		}
		i++;
		init(vDealFunc);
		init(vFuncCommand);
		init(vPromName);
		init(vOpPlace);
		EXEC SQL select a.prom_name,a.deal_func,nvl(a.func_command,' '),nvl(a.op_place,' ')
				into :vPromName,:vDealFunc,:vFuncCommand,:vOpPlace
				from ssndcailcfg a,sRandomPresentcfg b
				where a.region_code = substr(:vBelongCode,1,2) 
				and a.mode_code = :iNewMode
				and a.region_code=b.region_code
				and a.mode_code=b.mode_code
				and a.deal_func=b.deal_func
				and b.detail_code=:vDetailCode
				and rownum<2;
		pubLog_Debug(_FFL_,"","","pubRandomDeleteMode vBelongCode[%s]iNewMode[%s]vDetailCode[%s] ",vBelongCode,iNewMode,vDetailCode);
		pubLog_Debug(_FFL_,"","","pubRandomDeleteMode SQLCODE[%d]vDealFunc[%s]vFuncCommand[%s] ",SQLCODE,vDealFunc,vFuncCommand);
		if(SQLCODE==0)
		{
			vCount=0;
			EXEC SQL SELECT count(*) 
					into :vCount
					from wsndcailmsg
					where mode_accept=to_number(:iOldAccept) 
	    			and id_no=to_number(:vIdNo)
	    			and deal_func=:vDealFunc
	    			and func_command=rpad(:vFuncCommand,256)
	    			and op_type='a';
	    	pubLog_Debug(_FFL_,"","","pubRandomDeleteMode vCount[%d]vDealFunc[%s]vFuncCommand[%s] ",vCount,vDealFunc,vFuncCommand);
	    	if(vCount>0)
	    	{
	    		EXEC SQL insert into wsndcailmsghis
						(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
						LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
						DEAL_TIME,SND_TIMES,prom_name,deal_func,
						begin_time,mode_accept)
				select LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
						LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,'d',
						sysdate,SND_TIMES,prom_name,deal_func,
						begin_time,mode_accept
					from wsndcailmsg
					where mode_accept=to_number(:iOldAccept) 
					and id_no=to_number(:vIdNo)
					and deal_func=:vDealFunc
	    			and func_command=rpad(:vFuncCommand,256)
	    			and op_type='a';
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(oErrMsg,"记录wsndcailmsghis错误![%d]",SQLCODE);
					PUBLOG_DBDEBUG("pubRandomDeleteMode");
					pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
					return 127050;
				}
		    	EXEC SQL delete from wsndcailmsg
		    			where mode_accept=to_number(:iOldAccept) 
		    			and id_no=to_number(:vIdNo)
		    			and deal_func=:vDealFunc
		    			and func_command=rpad(:vFuncCommand,256)
		    			and op_type='a';
			    if(SQLCODE!=0 && SQLCODE!=1403)
			    {
			    	sprintf(oErrMsg,"处理wsndcailmsg错误![%d]",SQLCODE);
					PUBLOG_DBDEBUG("pubRandomDeleteMode");
					pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
					return 127050;
			    }
			}
			else
			{
				EXEC SQL insert into wsndcailmsg(LOGIN_ACCEPT,DEAL_NO,ID_NO,PHONE_NO,
				        	LOGIN_NO,OP_CODE,OP_TIME,TOTAL_DATE,OP_TYPE,
				        	DEAL_TIME,SND_TIMES,prom_name,deal_func,
				        	begin_time,mode_accept,func_command,op_place)
				        values(sMaxSysAccept.nextVal,mod(sMaxSysAccept.nextVal,10),
				          to_number(:vIdNo),:iPhoneNo,:iLoginNo,:iOpCode,sysdate,
				          :vTotalDate,'d',sysdate,0,:vPromName,:vDealFunc,
				          to_date(to_char(last_day(sysdate),'yyyy-mm-dd')||' 11:00:00','yyyy-mm-dd hh24:mi:ss') ,
				          to_number(:iOldAccept),:vFuncCommand,:vOpPlace);
				if(SQLCODE!=0 )
			    {
			    	sprintf(oErrMsg,"记录wsndcailmsg错误![%d]",SQLCODE);
					PUBLOG_DBDEBUG("pubRandomDeleteMode");
					pubLog_DBErr(_FFL_, "", "127048", " [%s]", oErrMsg);
					return 127050;
			    }
			}
		}
	}
	EXEC SQL CLOSE curModeDet;
	
	
	          	
  }

	
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","pubRandomUpdateMode end ----- ");
#endif
    return 0;
}
