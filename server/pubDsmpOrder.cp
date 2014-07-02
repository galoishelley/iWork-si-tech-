/**********已完成组织结构改造(无改动)*********/
/*******已完成组织结构二期改造 edit by  zhaoyan 07/05/2009 <sXXXXCfm(opCode1,opCode3....)> ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubProcOrder.h"

EXEC SQL INCLUDE SQLCA;
char* strSplit(char* str, char* dealim, int i);

/**********************************************
 @FUNC   fDsmpPlatformOrder    业务平台定购DSMP产品函数调用
 @input  params: pOrderData -> 产品定购信息
 @input  params: return_mess -> 错误信息
 @input  params: vEfftFlag -> 订购关系生效标志:
 			(定义业务平台调用服务时，需要返回的参数)
			0－订购关系生效
			1－订购关系不生效，二次确认或全业务退订应答时，填1
			9－其它，DSMP应答BOSS的第三方确认请求时，填9
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpPlatformOrder(DSMPORDEROpr *tBizData, char *vEfftFlag,char *retMsg)
{
	int 		i  = 0;
	char 		retCode[6+1];
	char 		sCustWebPwd[8];
	char 		temp_sql[2000];
	int 		iRet =0;

	EXEC SQL BEGIN DECLARE SECTION;
	long 		lLoginAccept;
	char 		totalDate[8 + 1];
	char 		curTime[6+1];
	char 		yearMonth[6 + 1];
	char 		opTime[17 + 1];
	char 		dynStmt[1024];
 	char 		idNo[22 + 1];
	char 		smCode[2+1];
	char 		belongCode[7+1];

 	/*input parameter information*/
 	char 		loginAccept[22 + 1];
 	char 		opCode[4 + 1];
 	char 		loginNo[6 + 1];
 	char 		orgCode[9 + 1];
	char 		idType[2+1];
	char 		oprCode[2+1];
	char 		bizType[2+1];
	char 		oldPwd[8+1];
	char 		newPwd[8+1];
	char 		phoneNo[15+1];
	char 		spId[1001];
	char 		spBizCode[1001];
	char 		infoCode[1001];
	char 		infoValue[1001];
 	char 		inLimit[1 + 1];
 	char 		handFee[9 + 1];
 	char 		opNote[60 + 1];
 	char 		ipAddr[15 + 1];
 	char 		thirdNo[11 + 1];
 	char 		oprSource[2 + 1];
	char 		custRunTime[14+1];
	char 		vTmpOprCode[2+1];
	char 		thirdIdNo[14 + 1 ];
	char 		vchrg_type[2+1];
  	char 		vRslt[2+1];     /**操作结果  按错误代码表填写**/
  	char 		modeCode[8+1];

	/** 定义部分变量 **/
	char 		temp_runcode[1 +1];
	char 		temp_third_runcode[1 +1];
	char 		temp_serv_attr[1+1];         /*业务属性	G:全网业务,L:本地业务,默认为全网业务**/
	char 		temp_reconfirm[1+1];         /*是否需要二次确认  0:不二次确认, 1:二次确认,默认为二次确认**/
	char 		temp_thirdvalidate[1+1];     /*是否向业务平台做第三方确认 0:不需要,1:需要,默认为不需要 **/
	char 		temp_order_flag[1+1];        /**定购信息 1:定购产品   0:退订、变更产品 **/
	char 		temp_orderadd_flag[1+1];     /**定购附加信息 1:单独处理附加信息   0:附加信息处理 **/
	char 		temp_spId[20+1];
	char 		temp_spBizCode[21+1];
	char 		temp_source_type[6 +1];      /** BOSS:BOSS渠道还包含短信营业厅、WWW营业厅、10086 等 **/
	char 		temp_portalcode2[4+1];
	char 		temp_reconfirm_flag[1+1];    /** 二次确认标志 **/
	char 		second_retCode[4+1];
	char 		vpassword[30 +1];
	char 		vrun_code[2+1];
	char 		vrun_time[17 +1];
	char 		sTmpTime[5][14 + 1];
	char 		vallow_flag[1+1];
	char 		temp_order_id[14+1];
	char 		temp_biz_type[2+1];
	char 		temp_portalcode[8+1];
	char 		temp_comb_flag[1+1];
	char 		temp_mode_code[8+1];
	char 		old_password[32+1];
	char 		old_login_accept[14+1];
	char 		temp_prod_code[21+1];
	char 		temp_other_info[200+1];     /** 用来处理如Blackberry业务 需要传接口ISMI的情况**/
	char 		temp_default_oper[2+1];	   /** 捆绑操作缺省的操作命令**/

	EXEC SQL END DECLARE SECTION;

	Sinitn(loginAccept);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(orgCode);
	Sinitn(phoneNo);
	Sinitn(idNo);
	Sinitn(spId);
	Sinitn(spBizCode);
	Sinitn(infoCode);
	Sinitn(infoValue);
	Sinitn(inLimit);
	Sinitn(opNote);
	Sinitn(ipAddr);
	Sinitn(idType);
	Sinitn(oprCode);
	Sinitn(bizType);
	Sinitn(oldPwd);
	Sinitn(newPwd);
	Sinitn(handFee);
	Sinitn(thirdNo);
	Sinitn(thirdIdNo);
	Sinitn(oprSource);
	Sinitn(modeCode);

	Sinitn(vchrg_type   );
	Sinitn(vRslt);
	Sinitn(vallow_flag);
	Sinitn(temp_biz_type);
	Sinitn(temp_portalcode);
	Sinitn(temp_orderadd_flag);

	Sinitn(temp_runcode);
	Sinitn(temp_third_runcode);
	Sinitn(temp_serv_attr);
	Sinitn(temp_reconfirm);
	Sinitn(temp_thirdvalidate);
	Sinitn(temp_order_flag);
	Sinitn(temp_spId  );
	Sinitn(temp_spBizCode  );
	Sinitn(temp_source_type);
	Sinitn(temp_reconfirm_flag);
	Sinitn(vpassword );
	Sinitn(vrun_code );
	Sinitn(vrun_time );
	Sinitn(sTmpTime );
	Sinitn(temp_comb_flag);
	Sinitn(temp_mode_code);
	Sinitn(old_password);
	Sinitn(old_login_accept);
	Sinitn(temp_prod_code);
	Sinitn(temp_other_info);
	Sinitn(temp_default_oper);
	Sinitn(temp_portalcode2);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "-- begin -- fDsmpPlatformOrder ");
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->order_id        =[%ld]", tBizData->order_id       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->id_no           =[%s]", tBizData->id_no           );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->phone_no        =[%s]", tBizData->phone_no        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->serv_code       =[%s]", tBizData->serv_code       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_code        =[%s]", tBizData->opr_code        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->third_id_no,    =[%s]", tBizData->third_id_no    );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->third_phoneno   =[%s]", tBizData->third_phoneno   );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid            =[%s]", tBizData->spid            );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode         =[%s]", tBizData->bizcode         );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->prod_code       =[%s]", tBizData->prod_code       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->comb_flag       =[%s]", tBizData->comb_flag       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->mode_code       =[%s]", tBizData->mode_code       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->chg_flag        =[%s]", tBizData->chg_flag        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->chrg_type       =[%s]", tBizData->chrg_type       );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->channel_id      =[%s]", tBizData->channel_id      );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_source      =[%s]", tBizData->opr_source      );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->first_order_date=[%s]", tBizData->first_order_date);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->first_login_no  =[%s]", tBizData->first_login_no  );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->last_order_date =[%s]", tBizData->last_order_date );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_time        =[%s]", tBizData->opr_time        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->eff_time        =[%s]", tBizData->eff_time        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time        =[%s]", tBizData->end_time        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->local_code      =[%s]", tBizData->local_code      );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag      =[%s]", tBizData->valid_flag      );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->belong_code     =[%s]", tBizData->belong_code     );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->total_date      =[%s]", tBizData->total_date      );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->login_accept    =[%s]", tBizData->login_accept    );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->reserved        =[%s]", tBizData->reserved        );
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->sync_flag     =[%s]"  , tBizData->sync_flag     );
#endif

	strcpy(loginAccept,	tBizData->login_accept);
	strcpy(loginNo,		tBizData->login_no);
	strcpy(idType,		"01");
	strcpy(phoneNo,		tBizData->phone_no);
	strcpy(opCode,		tBizData->op_code);
	strcpy(oprCode,		tBizData->opr_code);
	strcpy(bizType,		tBizData->serv_code);
	strcpy(oldPwd,		" ");
	strcpy(newPwd,		tBizData->password);
	strcpy(spId,		tBizData->spid);
	strcpy(spBizCode,	tBizData->bizcode);
	strcpy(infoCode,	tBizData->infocode);
	strcpy(infoValue,	tBizData->infovalue);
	strcpy(inLimit,		" ");
	strcpy(handFee,		"0");
	strcpy(opNote,		tBizData->op_note);
	strcpy(thirdNo,		tBizData->third_phoneno);
	strcpy(oprSource,   tBizData->opr_source);
	strcpy(modeCode,    tBizData->mode_code);
	strcpy(old_password,tBizData->old_password);

  	Trim(infoCode);
  	Trim(infoValue);
	Sinitn(vTmpOprCode);
	strncpy(vTmpOprCode,oprCode,2);
	if(strcmp(oprCode, BIZ_OPR_PWDRESET) == 0)
	{
		Sinitn(newPwd); Sinitn(oprCode);
		strcpy(oprCode, "03");
	}

	strcpy(retCode,	"000000");
	strcpy(retMsg,	"Successfully");
	strcpy(vEfftFlag, "0");
	strcpy(vRslt,	  "00");

    /***读取系统操作时间 */
	Sinitn(totalDate); Sinitn(opTime); Sinitn(yearMonth); Sinitn(curTime);
	EXEC SQL	SELECT 	to_char(sysdate, 'yyyymmdd'),
						to_char(sysdate,'yyyymmdd hh24:mi:ss'),
						to_char(sysdate, 'yyyymm'),
						to_char(sysdate,'hh24miss')
				  INTO :totalDate,:opTime, :yearMonth, :curTime  FROM DUAL;
    if (SQLCODE != 0) {
    	strcpy(retCode,"791760");
    	sprintf(retMsg, "读取系统操作时间出错[%d]!" ,SQLCODE);
    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }

    /***读取操作操作员信息 */
    EXEC SQL	SELECT 	org_code, nvl(trim(ip_address),'172.0.0.1')
			  	INTO 	:orgCode, :ipAddr
			  	FROM 	dLoginMsg
			 	WHERE 	login_no = :tBizData->login_no ;

    if (SQLCODE == 1403){
        strcpy(orgCode,"0101790in");
        strcpy(ipAddr, "172.0.0.1");
    }else if (SQLCODE != 0) {
    	strcpy(retCode,"791761");
    	sprintf(retMsg, "读取表dLoginMsg出错[%s][%d]!", tBizData->login_no,SQLCODE);
    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }
    Trim(orgCode);
    Trim(ipAddr);

    Trim(bizType);
    if (strlen(bizType) < 1) {
	    EXEC SQL	SELECT 	biztype
	    			  INTO :bizType  FROM DDSMPSPINFO
	    			 WHERE SPID = :tBizData->spid and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
        if  (SQLCODE != 0 && SQLCODE != 1403) {
        	strcpy(retCode,"791785");
        	sprintf(retMsg, "读取DDSMPSPINFO表中的业务类型出错[%d]!" ,SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        if(SQLCODE == 1403)
        {
        	strcpy(retCode,"123456");
        	strcpy(retMsg, "企业代码不存在!");
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        Trim(bizType);
        strcpy(tBizData->serv_code,bizType );
    }

	Sinitn(smCode);
	Sinitn(belongCode);
	Sinitn(custRunTime);

	EXEC SQL	SELECT 	a.id_no, a.sm_code, a.belong_code,
				   			to_char(a.run_time,'yyyymmddhh24miss'),
				   			substr(a.run_code,2,1),b.run_codenew
			  	INTO 	:idNo, :smCode, :belongCode,
			  				:custRunTime,
			  				:temp_runcode,:vrun_code
			  	FROM 	dCustMsg a, SOBRUNCODECONVERT b
			 	WHERE 	a.phone_no = :phoneNo
			   				AND substr(a.run_code,2,1) < 'a'
			   				AND substr(a.run_code,2,1) = b.run_code ;

    if (SQLCODE != 0)
    {
    	strcpy(retCode,"791701");
    	sprintf(retMsg, "读取表dCustMsg出错[%s][%d]!", phoneNo,SQLCODE);
    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }
    Trim(idNo);
    Trim(vrun_code);
		if(strcmp(oprCode,"02")!=0 && strcmp(oprCode,"07")!=0 && strcmp(oprCode,"89")!=0 && strcmp(oprCode,"99")!=0)
		{
    	if ((strcmp(temp_runcode,"D") == 0)||(strcmp(temp_runcode,"E") == 0)) {
    		strcpy(retCode,"911320");
    		sprintf(retMsg, "用户已单向停机[%s]!", temp_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if ((strcmp(temp_runcode,"F") == 0) ||  (strcmp(temp_runcode,"G") == 0)||(strcmp(temp_runcode,"H") == 0)||(strcmp(temp_runcode,"L") == 0)||(strcmp(temp_runcode,"M") == 0) ){
    		strcpy(retCode,"911321");
    		sprintf(retMsg, "用户已停机[%s]!", temp_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if  ((strcmp(temp_runcode,"I") == 0)||(strcmp(temp_runcode,"c") == 0)||(strcmp(temp_runcode,"b") == 0)||(strcmp(temp_runcode,"a") == 0)||(strcmp(temp_runcode,"j") == 0)||(strcmp(temp_runcode,"l") == 0)) {
    		strcpy(retCode,"911322");
    		sprintf(retMsg, "用户已预销户[%s]!", temp_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }
    
		sprintf(retCode, "%06d", fDsmpOprLimitCtl(phoneNo, idNo,	opCode	,	retMsg));		
		if (strcmp(retCode,	"000000")	!= 0)
		{
				printf("fDsmpOprLimitCtl:retCode=[%s]	retMsg=[%s]	\n", retCode,retMsg	);
				goto end_valid ;
		}    
    

    /*判断号码状态是否允许受理    */
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
	  			  FROM cChgRun
	  			 WHERE op_code=:opCode and run_code=:temp_runcode
	  			   AND region_code= substr(:belongCode ,1,2);
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    strcpy(retCode,"791734");
	    sprintf(retMsg, "用户当前状态[%s]不允许变更!", temp_runcode);
	    PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		goto end_valid ;
	}

    /*转换时间格式 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
    memset(sTmpTime, 0, sizeof(sTmpTime));
    if((i = fPubFormatTime(sTmpTime[0],opTime)) != 0)
    {
        sprintf(retCode, "791732");
        sprintf(retMsg,"op_time调用fPubFormatTime失败! iRet=[%d]", i);
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }

    /*转换时间格式 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
    if((i = fPubFormatTime(sTmpTime[1],custRunTime)) != 0)
    {
        sprintf(retCode, "791733");
        sprintf(retMsg,"op_time调用fPubFormatTime失败! iRet=[%d]", i);
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "phoneNo = [%s]", phoneNo);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "idNo = [%s]", idNo);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "smCode = [%s]", smCode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "custRunTime = [%s]", custRunTime);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_runcode = [%s]", temp_runcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "belongCode = [%s]", belongCode);
#endif

    /*获得系统操作流水 **/
    lLoginAccept =  atol(loginAccept);
    if ( lLoginAccept <= 0 )
    	{
	    EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :lLoginAccept from dual;
	    if(lLoginAccept <= 0)
	    {
	    	sprintf(retCode, "%06d", ERR_GET_MAX_SYS_ACCEPT);
	    	sprintf(retMsg, "取操作流水号不成功，请重新确认!");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    sprintf(loginAccept, "%ld", lLoginAccept);
    }

	/* 业务受理信息记录(如果是业务注册或密码修改随机生成密码) */
	Sinitn(sCustWebPwd);
	strcpy(sCustWebPwd, oldPwd);

	if(strlen(newPwd) == 0)
	{
		if(strcmp(oprCode, "01") == 0 || strcmp(oprCode, "03") == 0)
		{
			Sinitn(sCustWebPwd);
			sprintf(sCustWebPwd,"%02d%02d%02d",rand() % 100,rand() % 100,rand() % 100);
		}
	}
	else
	{
		strcpy(sCustWebPwd, newPwd);
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "随机生成密码newPasswd = [%s]", sCustWebPwd);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "idNo = [%s]", idNo);
#endif

    /** 读取第三方用户信息 **/
	Trim(thirdNo);
	Trim(spId);
  	Trim(spBizCode);
	if (strlen(thirdNo) == 11 )  {
		EXEC SQL	SELECT id_no,substr(run_code,2,1)
		              INTO :thirdIdNo , :temp_third_runcode
		          FROM dCustMsg
				 WHERE phone_no = :thirdNo
				   AND substr(run_code,2,1) < 'a';
    	if (SQLCODE != 0)
    	{
    		strcpy(retCode,"791705");
    		sprintf(retMsg, "读取第三方用户的表dCustMsg出错[%s][%d]!", thirdNo,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    if(strcmp(oprCode,"02")!=0 && strcmp(oprCode,"07")!=0 && strcmp(oprCode,"89")!=0 && strcmp(oprCode,"99")!=0)
    {
    	if ((strcmp(temp_third_runcode,"D") == 0)||(strcmp(temp_third_runcode,"E") == 0)) {
    		strcpy(retCode,"911320");
    		sprintf(retMsg, "第三方用户已单向停机[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if ((strcmp(temp_third_runcode,"F") == 0) ||  (strcmp(temp_third_runcode,"G") == 0)||(strcmp(temp_third_runcode,"H") == 0)||(strcmp(temp_third_runcode,"L") == 0)||(strcmp(temp_third_runcode,"M") == 0) ){
    		strcpy(retCode,"911321");
    		sprintf(retMsg, "第三方用户已停机[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if  ((strcmp(temp_third_runcode,"I") == 0)||(strcmp(temp_third_runcode,"c") == 0)||(strcmp(temp_third_runcode,"b") == 0)||(strcmp(temp_third_runcode,"a") == 0)||(strcmp(temp_third_runcode,"j") == 0)||(strcmp(temp_third_runcode,"l") == 0)) {
    		strcpy(retCode,"911322");
    		sprintf(retMsg, "第三方用户预销户[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }

    	/*判断第三方用户状态是否允许受理 **/
	    strcpy(vallow_flag,"0");
	    EXEC SQL	SELECT allow_flag
	    			  INTO :vallow_flag
	      			  FROM cChgRun
	      			 WHERE op_code=:opCode and run_code=:temp_third_runcode
	      			   AND region_code= substr(:belongCode ,1,2);
	    if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	    {
	        strcpy(retCode,"791734");
	        sprintf(retMsg, "第三方用户当前状态[%s]不允许变更!", temp_third_runcode);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }

	}
	else {
		strcpy(thirdNo,"  ");
		strcpy(thirdIdNo,"0");
	}
	Trim(thirdIdNo);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "thirdIdNo = [%s]", thirdIdNo);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "thirdNo = [%s]", thirdNo);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_third_runcode = [%s]", temp_third_runcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "spId = [%s]", spId);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "spBizCode = [%s]", spBizCode);
#endif

    /** 对结构体变量赋操作信息 **/
    strcpy(tBizData->id_no            ,idNo  );
    strcpy(tBizData->third_id_no      ,thirdIdNo  );
    strcpy(tBizData->opr_time         ,opTime  );
    strcpy(tBizData->belong_code      ,belongCode  );
    strcpy(tBizData->total_date       ,totalDate  );
    strcpy(tBizData->login_accept     ,loginAccept);

    /**71|广东通用下载需要修改业务类型,方便接口对其进行处理 **/
    if ((strcmp(tBizData->serv_code,"17" ) == 0) && (strncmp(tBizData->spid,"719",3 ) == 0))
    	{
        strcpy(temp_biz_type,"71");
    }
    else
    	{
        strcpy(temp_biz_type,tBizData->serv_code);
    }

    /** Blackberry业务 需要给平台发送IMSI号码**/
    if (strcmp(tBizData->serv_code,"88" ) == 0)
    	{
	    EXEC SQL	SELECT '~'||b.imsi_no
	    			  INTO :temp_other_info
	      			  FROM dCustSim  a, dSimRes  b
	      			 WHERE  a.sim_no =  b.sim_no
	      			   AND a.id_no = to_number(:idNo);
	    if (SQLCODE != 0 )
	    {
	        strcpy(retCode,"791784");
	        sprintf(retMsg, "查找用的IMSI号码错误!" );
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    Trim(temp_other_info);
    }

    	/** BOSS渠道查询  **/
		EXEC SQL	SELECT  nvl(SOURCE_TYPE,' ')  into :temp_source_type
		          FROM SOBOPRSOURCE
				 WHERE type_code  = :oprSource  ;
    	if  ((SQLCODE != 0) &&(SQLCODE != 1403))
    	{
    		strcpy(retCode,"791790");
    		sprintf(retMsg, "读取渠道代码表SOBOPRSOURCE出错[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		    Trim(temp_source_type);

    	/** BOSS平台查询  **/
		EXEC SQL	SELECT	portalcode
					into 	:temp_portalcode2
					from  	SOBBIZTYPE_dsmp
				 	WHERE 	oprcode  = :bizType  ;
    	if  ((SQLCODE != 0) &&(SQLCODE != 1403))
    	{
    		strcpy(retCode,"791791");
    		sprintf(retMsg, "读取渠道代码表SOBBIZTYPE_dsmp出错[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		    Trim(temp_portalcode2);

    /** 检查捆绑产品代码的有效性 **/
	if (strcmp(tBizData->comb_flag,"1") == 0 )
		{
	    iRet = 0 ;
	    EXEC SQL	SELECT 	count(*)
	    			  INTO 	:iRet
	      			  FROM 	sBillMOdeCode
	      			 WHERE  region_code= substr(:belongCode ,1,2)
	      			   AND  mode_code = :tBizData->mode_code
	      			   AND  sysdate between start_time and stop_time ;
	    if  (SQLCODE != 0 )
	    {
	        strcpy(retCode,"791764");
	        sprintf(retMsg, "读取sBillMOdeCode中普通产品信息失败[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    if  (iRet < 1)
	    {
	        strcpy(retCode,"791765");
	        sprintf(retMsg, "申请的普通产品代码[%s]是无效的产品代码!", tBizData->mode_code);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }

	    /** 读取捆绑的DSMP产品信息**/
	    iRet = 0 ;
	    EXEC SQL	SELECT count(*)
	    			  INTO :iRet
	      			  FROM SDSMPPRODTRANS
	      			 WHERE  region_code= substr(:belongCode ,1,2)
	      			   AND  mode_code = :tBizData->mode_code
	      			   AND  sysdate between BEGIN_TIME  and END_TIME ;
	    if  (SQLCODE != 0 )
	    {
	        strcpy(retCode,"791766");
	        sprintf(retMsg, "读取DSMP内部转换关系表SDSMPPRODTRANS信息失败[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    if  (iRet < 1)
	    {
	        strcpy(retCode,"791767");
	        sprintf(retMsg, "申请的普通产品代码[%s]没有捆绑数据业务类产品!", tBizData->mode_code);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	}
	else
		{  /**    b.acid_flag = '0' ：原子产品**/
        /** 对申请业务的业务操作类型OPRCODE 的有效性进行判断**/
	    EXEC SQL	SELECT 	count(*)
	    		  	INTO 	:iRet
	    		  	FROM 	SOBBIZOPERCODE
	    		 	WHERE 	oprcode = :oprCode AND instr(bizlist,:bizType) >0 ;
        if (SQLCODE != 0)
        {
        	strcpy(retCode,"791762");
        	sprintf(retMsg, "读取表SOBBIZOPERCODE出错[%d]!",SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        if (iRet < 1 )
        {
        	strcpy(retCode,"791716");
        	sprintf(retMsg, "操作代码OPERCODE[%s]不可以用 !",bizType );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }

		EXEC SQL	SELECT 	a.mode_code ,a.prod_code
		            INTO 	:modeCode ,:temp_prod_code
		          	FROM 	SDSMPPRODTRANS a,SDSMPPRODBIZ b
					WHERE 	a.PROD_CODE = b.PROD_CODE
				   				AND b.SPID = :spId
				   				AND b.BIZCODE = :spBizCode  AND b.acid_flag = '0'
				   				AND a.mode_comb_flag  = '0'
				   				AND sysdate between b.BEGIN_TIME  and b.END_TIME
				   				AND sysdate between a.BEGIN_TIME  and a.END_TIME
				   				AND a.region_code = substr(:belongCode,1,2);
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
	        strcpy(retCode,"791780");
	        sprintf(retMsg, "读取SP业务对应的捆绑产品信息失败[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
        }
        else if (SQLCODE == 0)
        	{
            /** 支持针对单个SP业务的产品定购进行二批优惠处理 **/
            Trim(modeCode);
            strcpy(tBizData->mode_code,modeCode);
            strcpy(tBizData->comb_flag,"1");
            strcpy(tBizData->prod_code,temp_prod_code);
        } else {
            strcpy(tBizData->mode_code,"  ");
            strcpy(tBizData->comb_flag,"0");
        }

        EXEC SQL  	SELECT 	nvl(portalcode,' ')
	              	into   	:temp_portalcode
	              	FROM  	SOBBIZTYPE_dsmp
	    		 	WHERE  	oprcode = :bizType ;
        if  (SQLCODE != 0)
        {
					strcpy(retCode,"791725");
        	sprintf(retMsg, "读取SOBBIZTYPE_dsmp出错[%s][%d]!", bizType,SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        Trim(temp_portalcode);
	}

    /** 处理用户定购的产品信息 1.由于捆绑产品存在多个SPID，首先处理；
         2、其次处理，OPRCODE业务操作代码与DSMP相冲突的操作代码(即：操作代码 代码相同的，而操作含义不同的操作)
         3、最后处理，DSMP相关的操作代码 **/
    if (strcmp(tBizData->comb_flag,"1") == 0 )
    	{  /** 捆绑产品的处理 **/
        if  ( (strncmp(tBizData->opr_code,BIZ_OPR_REGIST,2) == 0 )
              || (strncmp(tBizData->opr_code,BIZ_OPR_DEST,2) == 0)|| (strncmp(tBizData->opr_code,BIZ_OPR_13,2) == 0 )
              ||(strncmp(tBizData->opr_code, BIZ_OPR_11,2) == 0&&strncmp(tBizData->serv_code, "19",2) == 0)){
                strcpy(temp_order_flag,"1"  );
        }else{
                strcpy(temp_order_flag,"0"  );
        }

        /** 产品附加信息的判断**/
        if (strlen(infoCode) > 0 ) {
            strcpy(temp_orderadd_flag         ,"0"  );
        }

    }
  else if ((strncmp(oprCode,BIZ_OPR_14,2) == 0) || (strncmp(oprCode,BIZ_OPR_15,2) == 0)
               || (strncmp(oprCode,BIZ_OPR_SUSPEND,2) == 0) || (strncmp(oprCode,BIZ_OPR_RESUME,2) == 0)
               || ((strcmp(oprCode,"09") == 0) && (strcmp(bizType,"16") == 0))  )
      {
    	/*14-用户主动暂停,15-用户主动恢复, 09-用户主动暂停，手机邮箱专用。***/

		/** 读取用户的产品定购信息 **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** DSMP操作类型检查 **/
		sprintf(retCode, "%06d", fDsmpOprCodeCheck(bizType ,tBizData->opr_code,oprCode, retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "DSMP操作类型检查retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpOprCodeCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		strcpy(temp_order_flag         ,"0"  );
	    strcpy(tBizData->opr_code         ,oprCode  );
	    strcpy(tBizData->opr_source       ,oprSource  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

    }
 	else if ((strncmp(oprCode,BIZ_OPR_21,2) == 0) && (strncmp(bizType,"28",2) == 0)){    /**28|游戏业务 21 - 充值**/
		/** 读取用户的产品定购信息 **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
		strcpy(temp_orderadd_flag         ,"1"  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

	}
	else if ((strncmp(oprCode,"03",2) == 0) && (strncmp(bizType,"88",2) == 0)){    /**88|BlackBerry  03-用户信息变更 **/
		/** 读取用户的产品定购信息 **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
		strcpy(temp_orderadd_flag         ,"1"  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

	}
	else if (strncmp(oprCode,BIZ_OPR_CHGDAT,2) == 0)
		{/**08-与业务平台资料变更 **/
		/** 读取用户的产品定购信息 **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
		strcpy(temp_orderadd_flag         ,"1"  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

    }
  	else if ((strncmp(oprCode,BIZ_OPR_DEST,2) == 0) || (strncmp(oprCode,BIZ_OPR_11,2) == 0)
                || (strncmp(oprCode,BIZ_OPR_21,2) == 0)
                || (strncmp(oprCode,BIZ_OPR_REGIST,2) == 0)|| (strncmp(oprCode,BIZ_OPR_13,2) == 0))
  	{/**06-订购 11-赠送 21-订购确认 01-注册**/
        /** 针对部分没有SPID或者SPBizCode的业务进行处理,初始化为系统定义的缺省值 **/
        EXEC SQL  	SELECT 	nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ')
	        		into  	:temp_spId,:temp_spBizCode
	      			FROM  	SOBBIZTYPE_dsmp
	     			WHERE  	oprcode = :bizType;
        if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
        {
        	sprintf(retMsg, "读取SOBBIZTYPE_dsmp出错[%s][%d]!", bizType,SQLCODE);
        	strcpy(retCode,"791783");
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        Trim(temp_spId);
        Trim(temp_spBizCode);
	    if (strlen(temp_spId) > 0 )
	    	{
	        strcpy(tBizData->spid,temp_spId);   /**初始化为系统定义的缺省值 **/
	        strcpy(tBizData->send_spid," ");
	        strcpy(spId,temp_spId);
	    } else {
	        strcpy(tBizData->send_spid,tBizData->spid);
	    }

	    if (strlen(temp_spBizCode) > 0 )  {
	        strcpy(tBizData->bizcode,temp_spBizCode);   /**初始化为系统定义的缺省值 **/
	        strcpy(tBizData->send_bizcode," ");
	        strcpy(spBizCode,temp_spBizCode);
	    } else {
	        strcpy(tBizData->send_bizcode,tBizData->bizcode);
	    }

		/** 检查SP代码、SP业务代码是否可用、是否合法   **/
		sprintf(retCode, "%06d", fDsmpSPBizCheck(spId, spBizCode, bizType, "A",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "检查SP代码、SP业务代码返回retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpSPBizCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** 用户信息开关功能检查 **/
		/** 增加输入参数 by zhangxina @ 20110328 **/
		sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(idNo, phoneNo, bizType, spId, spBizCode, belongCode,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "用户信息开关功能检查retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (strncmp(oprCode,BIZ_OPR_11,2) == 0)
		{
			/** 第三方用户信息开关功能检查 **/
			/** 增加输入参数 by zhangxina @ 20110328 **/
			sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(thirdIdNo, thirdNo, bizType, spId, spBizCode, belongCode,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "第三方用户信息开关功能检查retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
		}

		/** 确认用户是否定购过同一业务 **/
		sprintf(retCode, "%06d", fDsmpProdOrderCheck(idNo, thirdIdNo, spId ,spBizCode , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "确认用户是否定购过同一业务retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpProdOrderCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** 确认该SP业务信息 **/
		EXEC SQL	SELECT 	SERVATTR,nvl(THIRDVALIDATE,' '),nvl(RECONFIRM,' ') ,nvl(BILLINGTYPE,' ')
		            INTO 	:temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		          	FROM 	DDSMPSPBIZINFO
				 	WHERE 	SPID = :spId   AND BIZCODE = :spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	if (SQLCODE != 0)
    	{
    		strcpy(retCode,"791735");
    		sprintf(retMsg, "读取SP业务信息表DDSMPSPBIZINFO出错[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	Trim(vchrg_type);

		Trim(temp_thirdvalidate);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_source_type=[%s]temp_thirdvalidate=[%s]",temp_source_type,temp_thirdvalidate);
		if ((strncmp(oprCode,BIZ_OPR_DEST,2) != 0) && (strncmp(oprCode,BIZ_OPR_11,2) != 0)&& (strncmp(oprCode,BIZ_OPR_13,2) != 0) ) {
		    strcpy(temp_thirdvalidate,"0");
		}else  if (strncmp(temp_source_type,"BOSS",4) != 0)
			{
		    strcpy(temp_thirdvalidate,"0");
		}

		/** 发送二次确认信息到  短信接口表   **/
		/** BOSS支持二次确认返回码的灵活配置， 二次确认返回码配置为空或非数字时，用户返回任意返回码均视为用户同意订购
		    当二次确认返回码配置1位或多位数字时，则对用户返回码进行精确匹配，匹配成功才完成订购操作  ***/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", " spid = [%s]",spId);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "spBizCode = [%s]", spBizCode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "oprSource = [%s]", oprSource);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "opCode = [%s]", opCode);
#endif
		EXEC SQL 	SELECT	cfm_flag  into :temp_reconfirm_flag
		          	FROM 	SDSMPRECONFIRM
				 	WHERE 	SPID  = :spId  AND BIZCODE   =:spBizCode
                   				AND OPR_SOURCE  =:oprSource  AND OP_CODE = :opCode ;
    	if ((SQLCODE != 0) &&(SQLCODE != 1403))
    	{
    		strcpy(retCode,"791737");
    		sprintf(retMsg, "读取二次确认代码表SDSMPRECONFIRM出错[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		Trim(temp_reconfirm_flag);

		/** 部分结构体变量赋值 **/

		strcpy(tBizData->chg_flag         ,"0"  );/*SP业务变更引起的订购/退订需要将此标记置位为1，否则填0**/
		strcpy(tBizData->chrg_type        ,vchrg_type  );	/**计费类型**/

        if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0))
        	{
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00－通过google搜索订购 99－本操作与手机搜索平台无关**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

		strcpy(tBizData->first_order_date ,opTime  );
		strcpy(tBizData->first_login_no   ,loginNo  );
		strcpy(tBizData->last_order_date  ,opTime  );
		strcpy(tBizData->local_code       ,temp_serv_attr );	  /*全网、本网标志**/
		strcpy(tBizData->valid_flag       ,"1" );	          /*有效标志 **/
		strcpy(tBizData->reserved         ,"  "  );
		tBizData->order_id = lLoginAccept ;
		strcpy(tBizData->sm_code         ,smCode  );
		strcpy(tBizData->password        ,sCustWebPwd  );
	  	strcpy(tBizData->send_eff_time    ,opTime  );

	  	strcpy(temp_order_flag         ,"1"  );

		/** 发送信息到接口，等待第三方确认的接口返回  temp_reconfirm_flag **/
		if ( (strcmp(temp_thirdvalidate, "1") == 0) && (strcmp(temp_reconfirm_flag, "1") != 0))
			{
    		memset(dynStmt, 0, sizeof(dynStmt));
    		iRet =0;
			/**sParamStr 组成：业务类型/工号/工号归属/OPCODE/TOTALDATE
								/IDTYPE（默认01）/手机号码/操作代码tBizData->opr_code,
								/SP企业代码/SP业务代码/信息代码/信息值
								/业务密码/操作来源/操作时间/用户状态
								/状态变更时间/操作备注
			 **/
			sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~21~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
       		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,
                				tBizData->spid,tBizData->bizcode, infoCode,infoValue,sCustWebPwd,tBizData->opr_source ,
                				sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);

			iRet = fCallInterface(dynStmt,retCode ,retMsg ,second_retCode);
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "发送信息到接口，第三方确认的接口返回retCode=[%s]" ,retCode);
			if (iRet != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:调用接口函数错误second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		    strcpy(retCode,"791738");
    		    sprintf(retMsg, "fCallInterface:调用接口函数错误iRet=[%d]", iRet );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}

			if 	(strncmp(second_retCode, "05",2) == 0)
				{	/**05:业务平台已存在订购关系 **/
					if (strncmp(tBizData->opr_code, BIZ_OPR_DEST,2) == 0||strncmp(tBizData->opr_code, BIZ_OPR_13,2) == 0||(strncmp(tBizData->opr_code, BIZ_OPR_11,2) == 0&&strncmp(tBizData->serv_code, "19",2) == 0))
						{
						/** 属于 BOSS系统 与 业务平台 数据不一致问题 ,当作正常情况**/
					}
				  else {
					pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:平台返回错误second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		        strcpy(retCode,"791739");
    		        sprintf(retMsg, "fCallInterface:平台返回错误second_retCode=[%s] ", second_retCode );
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		        goto end_valid ;
					}
			}
			else if (strncmp(second_retCode, "00",2) != 0)
				{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:平台返回错误second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		    strcpy(retCode,"791740");
    		    sprintf(retMsg, "fCallInterface:平台返回错误second_retCode=[%s] ", second_retCode );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}

			else if (strncmp(retCode, "000000",6) != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:调用平台执行函数错误 retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		    strcpy(retCode,"791741");
    		    sprintf(retMsg, "fCallInterface:调用平台执行函数错误 retCode=[%s]", retCode );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}
		}
		/**第三方确认的接口处理结束 **/
		strcpy(vEfftFlag, "1");

        /** 产品附加信息的判断**/
        if (strlen(infoCode) > 0)
        	{
            strcpy(temp_orderadd_flag         ,"0"  );
        }
    }
  	else if (strncmp(oprCode,BIZ_OPR_89,2) == 0)
  	{/**89-SP全业务退订 **/

    	Sinitn(temp_sql);
    	iRet = 0;
		sprintf(temp_sql, "select count(*) from DDSMPORDERMSG a where ( (a.id_no=to_number(:v1) and  a.THIRD_ID_NO = 0 ) OR a.THIRD_ID_NO = to_number(:v2) ) and a.spid =:v3  and sysdate between EFF_TIME  and end_time  " );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql = [%s]", temp_sql);

	    EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :temp_sql INTO :iRet using :idNo ,:idNo ,:spId ;
			END;
		END-EXEC;

		if (SQLCODE != 0) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "查找该用户的产品定购信息失败[%s][%d]!", spId,SQLCODE);
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (iRet < 1) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "没有找到该用户的产品定购信息[%s][%d]!", spId,SQLCODE);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** 检查SP代码、SP业务代码是否可用、是否合法  **/
		sprintf(retCode, "%06d", fDsmpSPBizCheck(spId, spBizCode, bizType, "C",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "检查SP代码、SP业务代码返回retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpSPBizCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		strcpy(temp_order_flag         ,"0"  );
	    strcpy(tBizData->opr_code         ,oprCode  );
	    strcpy(tBizData->opr_source       ,oprSource  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

    }
  else if ((strncmp(oprCode,BIZ_OPR_99,2) == 0)  || (strncmp(oprCode,BIZ_OPR_EXIT,2) == 0))
  	{/**99-DSMP全业务退订,02-用户注销**/
    	Sinitn(temp_sql);
    	iRet = 0;
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "查询业务平台代码temp_portalcode=[%s]",temp_portalcode);
		if  (strncmp(temp_portalcode,"DSMP",4) == 0 ) {
				memset(temp_sql,0,sizeof(temp_sql));
		    sprintf(temp_sql, "select count(*) from DDSMPORDERMSG  a ,SOBBIZTYPE_dsmp b  "
		  	   " where ( a.id_no= to_number(:v1) OR THIRD_ID_NO = to_number(:v2) )	"
		  	   "   and ( sysdate between a.EFF_TIME  and a.end_time) "
		  	   "   and a.serv_code = b.oprcode "
		  	   "   and  b.PORTALCODE = 'DSMP'  "  );
	    EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :temp_sql INTO :iRet using :idNo ,:idNo;
			END;
		END-EXEC;

		} else {
				memset(temp_sql,0,sizeof(temp_sql));
		    sprintf(temp_sql, "select count(*) from DDSMPORDERMSG  a ,SOBBIZTYPE_dsmp b  "
		  	   " where ( a.id_no= to_number(:v1) OR THIRD_ID_NO = to_number(:v2) ) "
		  	   "  and ( sysdate between a.EFF_TIME  and a.end_time) "
		  	   "   and a.serv_code = b.oprcode "
		  	   "   and  b.oprcode = :v3  "  );
	    EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :temp_sql INTO :iRet using :idNo ,:idNo ,:bizType;
			END;
		END-EXEC;
		}
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql=[%s]", temp_sql);
#endif

		if (SQLCODE != 0) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "查找该用户的产品定购信息失败[%s][%d]!", bizType,SQLCODE);
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (iRet < 1) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "没有找到该用户的产品定购信息[%s][%d]!",bizType,SQLCODE);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

    	strcpy(temp_order_flag         ,"0"  );

	}
	else if  (strncmp(oprCode,BIZ_OPR_UNDEST,2) == 0||strncmp(oprCode,BIZ_OPR_12,2) == 0)
		{/**07-退订 **/
		/** 读取用户的产品定购信息 **/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid = [%s]", tBizData->spid);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode = [%s]", tBizData->bizcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag = [%s]", tBizData->valid_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time = [%s]", tBizData->end_time);
#endif

		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "07-退订   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "07-退订   tBizData->bizcode = [%s]", tBizData->bizcode);
#endif
		strcpy(temp_order_flag         ,"0"  );
 		strcpy(tBizData->valid_flag     ,"0"       );
 		strcpy(tBizData->end_time       ,opTime );
	    strcpy(tBizData->opr_code         ,oprCode  );
	    strcpy(tBizData->opr_source       ,oprSource  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

	}
	else if ((strncmp(oprCode,BIZ_OPR_CHGPWD,2) == 0)
	         || (strncmp(oprCode,BIZ_OPR_PWDRESET,2) == 0))
	         {/**03- 密码修改 , 09- 密码重置 **/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid = [%s]", tBizData->spid);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode = [%s]", tBizData->bizcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag = [%s]", tBizData->valid_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time = [%s]", tBizData->end_time);
#endif

		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "确认用户是否定购过同一业务retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "密码修改   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "密码修改   oprCode = [%s]", oprCode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "密码修改   old_password = [%s]", old_password);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "密码修改   tBizData->password = [%s]", tBizData->password);
#endif

        /** 密码校验
        Trim(old_password);
        if ((strcmp(old_password,tBizData->password) != 0)  && (strncmp(oprCode,BIZ_OPR_CHGPWD,2) == 0) ){
		    sprintf(retMsg,"密码校验错误,输入的旧密码与用户原有密码不一致!" );
    	    strcpy(retCode,"791781");
    	    goto end_valid ;
        }**/

		strcpy(temp_order_flag         ,"0"  );
	    strcpy(tBizData->opr_code       ,oprCode  );
	    strcpy(tBizData->opr_source     ,oprSource  );
	    strcpy(tBizData->password       ,sCustWebPwd  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

  }
  else {/** 操作代码非法  **/
		sprintf(retMsg,"操作代码opr_code输入错误[%s]!",oprCode);
    	strcpy(retCode,"791716");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
	}

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "...开始生成产品定购关系...        "        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->order_id        =[%ld]", tBizData->order_id       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->id_no           =[%s]", tBizData->id_no           );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->phone_no        =[%s]", tBizData->phone_no        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->serv_code       =[%s]", tBizData->serv_code       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_code        =[%s]", tBizData->opr_code        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->third_id_no,    =[%s]", tBizData->third_id_no    );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->third_phoneno   =[%s]", tBizData->third_phoneno   );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid            =[%s]", tBizData->spid            );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode         =[%s]", tBizData->bizcode         );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->prod_code       =[%s]", tBizData->prod_code       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->comb_flag       =[%s]", tBizData->comb_flag       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->mode_code       =[%s]", tBizData->mode_code       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->chg_flag        =[%s]", tBizData->chg_flag        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->chrg_type       =[%s]", tBizData->chrg_type       );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->channel_id      =[%s]", tBizData->channel_id      );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_source      =[%s]", tBizData->opr_source      );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->first_order_date=[%s]", tBizData->first_order_date);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->first_login_no  =[%s]", tBizData->first_login_no  );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->last_order_date =[%s]", tBizData->last_order_date );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->opr_time        =[%s]", tBizData->opr_time        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->eff_time        =[%s]", tBizData->eff_time        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time        =[%s]", tBizData->end_time        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->local_code      =[%s]", tBizData->local_code      );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag      =[%s]", tBizData->valid_flag      );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->belong_code     =[%s]", tBizData->belong_code     );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->total_date      =[%s]", tBizData->total_date      );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->login_accept    =[%s]", tBizData->login_accept    );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->reserved        =[%s]", tBizData->reserved        );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->sm_code        =[%s]",  tBizData->sm_code         );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->password        =[%s]", tBizData->password        );
#endif

    /** 处理产品定购关系  **/
    if ( (strcmp(tBizData->comb_flag,"1") == 0 ) && (strcmp(temp_order_flag,"1") == 0 ))
    	{
    	Sinitn(temp_sql);
		sprintf(temp_sql, "select a.bizcode,a.spid,c.biztype from SDSMPPRODBIZ  a ,SDSMPPRODTRANS b ,DDsmpSPINFO c "
		  	   " where a.prod_code = b.prod_code and a.ACID_FLAG = '0'  "
		  	   "   and ( sysdate between a.begin_TIME  and a.end_time) and b.region_code =substr(:v1,1,2) "
		  	   "   and b.mode_code = :v2   and a.spid = c.spid  "
		  	   "   and ( sysdate between b.begin_TIME  and b.end_time) and sysdate between to_date(c.validdate,'yyyymmdd') and to_date(c.expiredate,'yyyymmdd')"  );
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql=[%s]", temp_sql);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "modeCode=[%s]", modeCode);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "belongCode=[%s]", belongCode);

#endif

    EXEC SQL PREPARE preStmt FROM :temp_sql;
	  EXEC SQL DECLARE vCurCancel3 CURSOR for preStmt;
		EXEC SQL OPEN vCurCancel3 using :belongCode,:modeCode ;
		EXEC SQL FETCH vCurCancel3 into :temp_spBizCode ,:temp_spId, :bizType;

		if (SQLCODE == 1403)
			{
			strcpy(retCode,"791770");
			sprintf(retMsg, "没有找到该用户的产品定购信息[%d]!",  SQLCODE);
			EXEC SQL CLOSE vCurCancel3;
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

        while(SQLCODE == 0)
        {
			Trim(temp_spBizCode);
			Trim(temp_spId);
			strcpy(tBizData->bizcode          ,temp_spBizCode  );
			strcpy(tBizData->spid             ,temp_spId  );
		  	strcpy(tBizData->serv_code        ,bizType  );
	    	strcpy(tBizData->send_eff_time    ,opTime  );

            /** 针对部分没有SPID或者SPBizCode的业务进行处理,初始化为系统定义的缺省值 **/
            EXEC SQL  SELECT nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ') ,nvl(DEFAULT_ORDEROPR,'06')
	            into  :temp_spId,:temp_spBizCode ,:temp_default_oper
	          FROM  SOBBIZTYPE_dsmp
	         WHERE  oprcode = :bizType;
            if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
            {
            	sprintf(retMsg, "读取SOBBIZTYPE_dsmp出错[%s][%d]!", bizType,SQLCODE);
            	strcpy(retCode,"791783");
            	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
            	goto end_valid ;
            }

            Trim(temp_default_oper);
            strcpy(tBizData->opr_code        ,temp_default_oper  );

	    /** 用户信息开关功能检查 **/
		/** 增加输入参数 by zhangxina @ 20110328 **/
	    sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(idNo, phoneNo, bizType, spId, spBizCode, belongCode,retMsg));
	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "用户信息开关功能检查retCode=[%s]" ,retCode);
	    if (strcmp(retCode, "000000") != 0)
		    {
		    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
		    	EXEC SQL CLOSE vCurCancel3;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		    	goto end_valid ;
		    }

		    /** 检查SP代码、SP业务代码是否可用、是否合法   **/
		    sprintf(retCode, "%06d", fDsmpSPBizCheck(temp_spId, temp_spBizCode, bizType, "A",retMsg));
		    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "检查SP代码、SP业务代码返回retCode=[%s]" ,retCode);
		    if (strcmp(retCode, "000000") != 0)
		    {
		    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpSPBizCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
		    	EXEC SQL CLOSE vCurCancel3;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		    	goto end_valid ;
		    }
		    /** 确认该SP业务信息 **/
		    EXEC SQL	SELECT SERVATTR,THIRDVALIDATE,RECONFIRM ,BILLINGTYPE
		                  INTO :temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		              FROM DDSMPSPBIZINFO
		    		 WHERE SPID = :temp_spId AND BIZCODE = :temp_spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	    if (SQLCODE != 0)
    	    {
    	    	strcpy(retCode,"791771");
    	    	sprintf(retMsg, "读取SP业务信息表DDSMPSPBIZINFO出错[%s][%d]!", spId,SQLCODE);
    	    	EXEC SQL CLOSE vCurCancel3;
    	    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }
					Trim(vchrg_type);
            /** 判断用户是否申请过该业务 **/
            Sinitn(dynStmt);
            Sinitn(temp_comb_flag);
			sprintf(dynStmt, " select comb_flag from DDSMPORDERMSG%ld     "
			                  " where id_no=to_number(:v1) and  spid = :v2 and  bizcode= :v3  "
				              " and sysdate between EFF_TIME and end_time and third_id_no = 0 ", atol(idNo)%10L  );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql = [%s]", dynStmt);
#endif
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :temp_comb_flag using :idNo,:temp_spId ,:temp_spBizCode;
			END;
			END-EXEC;
			if ((SQLCODE != OK  ) && (SQLCODE != 1403  ))
				{
				sprintf(retMsg,"判断用户是否申请该业务失败[%d]!",SQLCODE);
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "  [%s]", retMsg);
				strcpy(retCode,"791768");
				EXEC SQL CLOSE vCurCancel3;
				PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
			else if (SQLCODE == 0  )
			{
			    if (strcmp(temp_comb_flag,"1") == 0 )
			    	{
			    	sprintf(retMsg,"申请的[%s][%s]捆绑在其它产品中,不能重复捆绑!",temp_spId ,temp_spBizCode );
			    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "  [%s]", retMsg);
			    	strcpy(retCode,"791769");
			    	EXEC SQL CLOSE vCurCancel3;
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			    	goto end_valid ;
			    }
			    else if (strncmp(tBizData->op_code,"9113",4) != 0 )
			    	{
			        /** 更新定购信息成捆绑产品 **/
			        sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, "0",retMsg));
			        pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "更新定购信息成捆绑产品 retCode=[%s]" ,retCode);
			        if (strcmp(retCode, "000000") != 0)
			        {
			        	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			        	EXEC SQL CLOSE vCurCancel3;
    					pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			        	goto end_valid ;
			        }
			        break;
			    }
			}
				Trim(vchrg_type);
		    strcpy(tBizData->chg_flag         ,"0"  );	        /*SP业务变更引起的订购/退订需要将此标记置位为1，否则填0**/
		    strcpy(tBizData->chrg_type        ,vchrg_type  );	/**计费类型**/
            if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) )
            	{  /** 00－通过google搜索订购 99－本操作与手机搜索平台无关**/
		       		 strcpy(tBizData->channel_id       ,"00"  );
             }
            else {
                strcpy(tBizData->channel_id       ,"99"  );
            }

		    strcpy(tBizData->first_order_date ,opTime  );
		    strcpy(tBizData->first_login_no   ,loginNo  );
		    strcpy(tBizData->last_order_date  ,opTime  );
		    strcpy(tBizData->local_code       ,temp_serv_attr );	  /*全网、本网标志**/
		    strcpy(tBizData->valid_flag       ,"1" );	          /*有效标志 **/
		    strcpy(tBizData->reserved         ,"  "  );
		    tBizData->order_id = lLoginAccept ;
		    strcpy(tBizData->sm_code         ,smCode  );
		    strcpy(tBizData->password        ,oldPwd  );

	        if (strncmp(tBizData->op_code,"9113",4) == 0 )
	        	{
			    /** 记录用户的产品定购明细信息 ***/
			    sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "记录用户的产品定购信息 retCode=[%s]" ,retCode);
			    if (strcmp(retCode, "000000") != 0)
			    {
			    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			    	EXEC SQL CLOSE vCurCancel3;
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			    	goto end_valid ;
			    }
       	        /**二次与固定费用的优惠处理 **/
		        sprintf(retCode,"%06d",pubBillBeginMode(phoneNo,modeCode,opTime,opCode,loginNo,loginAccept,opTime,"0"," ", retMsg) );
		        if (strcmp(retCode, "000000") != 0)
		        {
    	            	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "pubBillBeginMode:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    					pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	            	goto end_valid ;
		        }
		    }

		    if ((strncmp(oprCode,"99",2) == 0 ) && (strncmp(temp_portalcode,"DSMP",4) == 0 ))
		    	{
		        strcpy(temp_biz_type, "00");
		    }

            #ifdef _DEBUG_
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......发送接口数据到DSMP平台");
            #endif
            memset(dynStmt, 0, sizeof(dynStmt));
    	    /**sParamStr 组成：业务类型/工号/工号归属/OPCODE/TOTALDATE
    	    					/IDTYPE（默认01）/手机号码/操作代码
    	    					/SP企业代码/SP业务代码/信息代码/信息值    bizType    tBizData->serv_code
    	    					/业务密码/操作来源/操作时间/用户状态
    	    					/状态变更时间/操作备注   **/
            sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
               		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                        				tBizData->send_spid,tBizData->send_bizcode, infoCode,infoValue,
                        				sCustWebPwd,tBizData->opr_source ,sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	    /** 调用接口发送程序对接口信息进行处理 **/
    	    sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "调用接口发送程序对接口信息返回retCode=[%s]" ,retCode);
    	    if (strcmp(retCode, "000000") != 0)
    	    {
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInterfaceSend:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    	    	EXEC SQL CLOSE vCurCancel3;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }
       	    EXEC SQL FETCH vCurCancel3 into :temp_spBizCode ,:temp_spId, :bizType;
       	}
       	EXEC SQL CLOSE vCurCancel3;

    }
    else if ( (strcmp(tBizData->comb_flag,"1") == 0 ) && (strcmp(temp_order_flag,"1") != 0 ))
    	{
    	Sinitn(temp_sql);
		sprintf(temp_sql, "select bizcode,spid,SERV_CODE  from DDSMPORDERMSG%ld a ,SDSMPPRODTRANS b  "
		  	   " where a.id_no= to_number(:v1) 	and ( sysdate between a.EFF_TIME  and a.end_time) "
		  	   "   and a.mode_code = :v2  and a.mode_code = b.mode_code  "
		  	   "   and ( sysdate between b.begin_TIME  and b.end_time) "
		  	   "   and b.region_code =substr(:v3,1,2) ", 	atol(idNo)%10L );
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql=[%s]", temp_sql);
#endif

        EXEC SQL PREPARE preStmt FROM :temp_sql;
	    EXEC SQL DECLARE vCurCancel4 CURSOR for preStmt;
		EXEC SQL OPEN vCurCancel4 using :idNo ,:modeCode ,:belongCode  ;
		EXEC SQL FETCH vCurCancel4 into :temp_spBizCode ,:temp_spId, :bizType;

        while(SQLCODE == 0)
        {
			Trim(temp_spBizCode);
			Trim(temp_spId);
			strcpy(tBizData->bizcode          ,temp_spBizCode  );
			strcpy(tBizData->spid             ,temp_spId  );
		    strcpy(tBizData->serv_code        ,bizType  );
	        strcpy(tBizData->send_eff_time    ,opTime  );

		    /** 确认该SP业务信息 **/
		    EXEC SQL	SELECT 	SERVATTR,THIRDVALIDATE,RECONFIRM ,BILLINGTYPE
		                INTO 	:temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		              	FROM 	DDSMPSPBIZINFO
		    		 	WHERE 	SPID = :temp_spId AND BIZCODE = :temp_spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	    if (SQLCODE != 0)
    	    {
    	    	strcpy(retCode,"791771");
    	    	sprintf(retMsg, "读取SP业务信息表DDSMPSPBIZINFO出错[%s][%d]!", spId,SQLCODE);
    	    	EXEC SQL CLOSE vCurCancel4;
    	    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }
					Trim(vchrg_type);
		    sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
		    if (strcmp(retCode, "000000") != 0)
		    {
		    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
		    	strcpy(retCode,"001305");
		    	EXEC SQL CLOSE vCurCancel4;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		    	goto end_valid ;
		    }

 		    strcpy(tBizData->valid_flag     ,"1"       );
 		    /** strcpy(tBizData->end_time       ,opTime ); 	 **/
	        strcpy(tBizData->opr_code         ,oprCode  );
	        strcpy(tBizData->opr_source       ,oprSource  );

		    strcpy(tBizData->chg_flag         ,"0"  );	        /*SP业务变更引起的订购/退订需要将此标记置位为1，否则填0**/
		    strcpy(tBizData->chrg_type        ,vchrg_type  );	/**计费类型**/
            if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) ) {
		        strcpy(tBizData->channel_id       ,"00"  );	    /** 00－通过google搜索订购 99－本操作与手机搜索平台无关**/
            } else {
                strcpy(tBizData->channel_id       ,"99"  );
            }

            /**  初始化为系统定义的缺省值  **/
            EXEC SQL  SELECT  nvl(DEFAULT_CANCLEOPR,'07')
	            into   :temp_default_oper
	          FROM  SOBBIZTYPE_dsmp
	          WHERE  oprcode = :bizType;
            if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
            {
            	sprintf(retMsg, "读取SOBBIZTYPE_dsmp出错[%s][%d]!", bizType,SQLCODE);
            	strcpy(retCode,"791783");
            	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
            	goto end_valid ;
            }
            Trim(temp_default_oper);
            strcpy(tBizData->opr_code        ,temp_default_oper  );

		/** 记录用户的产品定购明细信息 ***/
		sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "记录用户的产品定购信息 retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				EXEC SQL CLOSE vCurCancel4;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}

	    if ((strncmp(oprCode,"99",2) == 0 ) && (strncmp(temp_portalcode,"DSMP",4) == 0 )) {
		        strcpy(temp_biz_type, "00");
		    }

		Sinitn(old_login_accept);
			EXEC SQL 	SELECT 	distinct to_char(login_accept)
		                into 	:old_login_accept
		    	        From  	dBillCustDetail
		    	        where 	id_no= :idNo and mode_code =:modeCode  and mode_status ='Y'
		    	        			AND  END_TIME >sysdate;
		    if (SQLCODE != 0)
		    {
		    	sprintf(retMsg, "读取dBillCustDetail历史数据失败[%d]", SQLCODE);
		    	strcpy(retCode,"791782");
    	    	EXEC SQL CLOSE vCurCancel4;
    	    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
		    }
		    /*** ****/
		    sprintf(retCode, "%06d",pubBillEndMode(phoneNo,modeCode,old_login_accept,opTime,opCode,loginNo,loginAccept,opTime,retMsg) );
		    if (strcmp(retCode, "000000") != 0)
		    {
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "pubBillEndMode:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    	    	EXEC SQL CLOSE vCurCancel4;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
		    }

            #ifdef _DEBUG_
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......发送接口数据到DSMP平台");
            #endif
            memset(dynStmt, 0, sizeof(dynStmt));
    	    /**sParamStr 组成：业务类型/工号/工号归属/OPCODE/TOTALDATE
    	    					/IDTYPE（默认01）/手机号码/操作代码
    	    					/SP企业代码/SP业务代码/信息代码/信息值    bizType    tBizData->serv_code
    	    					/业务密码/操作来源/操作时间/用户状态
    	    					/状态变更时间/操作备注   **/
            sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
               		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                        				tBizData->send_spid,tBizData->send_bizcode, infoCode,infoValue, sCustWebPwd,
                        				tBizData->opr_source ,sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	    /** 调用接口发送程序对接口信息进行处理 **/
    	    sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "调用接口发送程序对接口信息返回retCode=[%s]" ,retCode);
    	    if (strcmp(retCode, "000000") != 0)
    	    {
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInterfaceSend:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    	    	EXEC SQL CLOSE vCurCancel4;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }

       	    EXEC SQL FETCH vCurCancel4 into :temp_spBizCode ,:temp_spId, :bizType;
       	}
       	EXEC SQL CLOSE vCurCancel4;

    }
    else if(strncmp(temp_orderadd_flag,"1",1) == 0)
    	{
        /* 单独处理 资料信息修改等  仅仅涉及附加信息变更的业务操作类型 */
    	sprintf(retCode, "%06d", fPubDsmpOrderMsgAdd(tBizData,temp_order_flag,temp_orderadd_flag,retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "单独处理 资料信息修改等  仅仅涉及附加信息变更fPubDsmpOrderMsgAdd:retCode=[%s]" ,retCode);
    	if (strcmp(retCode, "000000") != 0)
    	{
    		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsgAdd:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }
    else if (( (strncmp(oprCode,BIZ_OPR_DEST,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_11,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_21,2) == 0) || (strncmp(oprCode,BIZ_OPR_UNDEST,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_14,2) == 0) || (strncmp(oprCode,BIZ_OPR_15,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_SUSPEND,2) == 0) || (strncmp(oprCode,BIZ_OPR_RESUME,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_REGIST,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_CHGPWD,2) == 0) || (strncmp(oprCode,BIZ_OPR_PWDRESET,2) == 0)
         || (strncmp(oprCode,BIZ_OPR_25,2) == 0)|| (strncmp(oprCode,BIZ_OPR_12,2) == 0)||(strncmp(oprCode,BIZ_OPR_13,2) == 0) )
      && (strcmp(temp_reconfirm_flag, "1") != 0) )
      {

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "生成产品定购关系   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "生成产品定购关系   lLoginAccept = [%ld]", lLoginAccept);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "生成产品定购关系   temp_reconfirm_flag = [%s]", temp_reconfirm_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "生成产品定购关系   oprCode = [%s]", oprCode);
#endif
        /** 附加信息的处理 ***/
        if (strncmp(temp_orderadd_flag,"0",1) == 0)
        	{
    	    sprintf(retCode, "%06d", fPubDsmpOrderMsgAdd(tBizData,temp_order_flag,temp_orderadd_flag,retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "附加信息的处理fPubDsmpOrderMsgAdd:retCode=[%s]" ,retCode);
    	    if (strcmp(retCode, "000000") != 0)
    	    	{
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsgAdd:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    	}
        }

		/** 记录用户的产品定购信息 ***/
		sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "记录用户的产品定购信息返回retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
   }

	/** 89-SP全业务退订    **/
	if (strncmp(oprCode,BIZ_OPR_89,2) == 0)
		{
	    strcpy(tBizData->multi_order_flag ,"1"  ); /*多条定购的处理标志  1：多条;用于控制向同步接口表同步的数据只有一条 */
		strcpy(tBizData->spid             ,spId  );
		strcpy(tBizData->send_spid        ,spId  );
		strcpy(tBizData->id_no            ,idNo  );
		strcpy(tBizData->phone_no         ,phoneNo );
		strcpy(vEfftFlag, "1");
		strcpy(tBizData->opr_code       ,oprCode  );
 		strcpy(tBizData->valid_flag     ,"0"       );
 		strcpy(tBizData->end_time       ,opTime );
	  	strcpy(tBizData->opr_source     ,oprSource  );
	  	strcpy(tBizData->bizcode        ," "  );
	  	strcpy(tBizData->eff_time       ,opTime );
	  	strcpy(tBizData->chg_flag         ,"0"  );
	  	strcpy(tBizData->send_eff_time    ,opTime  );

        if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) ) {
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00－通过google搜索订购 99－本操作与手机搜索平台无关**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

        /** 记录发送到同步接口的信息表 只能是1条 **/
		sprintf(retCode, "%06d", fPubDsmpSubInfo(tBizData , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "发送到同步接口的信息表 fPubDsmpSubInfo: retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpSubInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"911330");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

    	Sinitn(temp_sql);
    	/** 不退订赠送订购关系，只退订自订购和别人赠送的DSMP **/
		sprintf(temp_sql, " select to_char(order_id) from DDSMPORDERMSG     "
		  				  "	where ( (id_no=to_number(:v1) and THIRD_ID_NO = 0 ) OR THIRD_ID_NO = to_number(:v2) ) "
						  "	and  spid =:v3 and sysdate between EFF_TIME  and end_time  "  );
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql = [%s]", temp_sql);

        EXEC SQL PREPARE preStmt FROM :temp_sql;
	    EXEC SQL DECLARE vCurCancel CURSOR for preStmt;
		EXEC SQL OPEN vCurCancel using :idNo,:idNo ,:spId ;
		EXEC SQL FETCH vCurCancel into :temp_order_id;

		if (SQLCODE == 1403) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "没有找到该SP的定购信息[%s][%d]!", spId, SQLCODE);
			EXEC SQL CLOSE vCurCancel;
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

        while(SQLCODE == 0)
        {
			Trim(temp_spBizCode);
			strcpy(tBizData->bizcode   ,temp_spBizCode  );
			tBizData->order_id = atol(temp_order_id);

			/** 读取用户的产品定购信息    **/
			sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "00", retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息fDsmpGetOrderInfo: retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				strcpy(retCode,"001305");
				EXEC SQL CLOSE vCurCancel;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "22222222222222222222读取用户的产品定购信息fDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			strcpy(tBizData->opr_code       ,oprCode  );
		    strcpy(temp_order_flag         ,"0"  );
 		    strcpy(tBizData->valid_flag     ,"0"       );
 		    strcpy(tBizData->end_time       ,opTime );
	        strcpy(tBizData->opr_source     ,oprSource  );
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "44444444444444444444444443读取用户的产品定购信息fDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			/** 记录用户的产品定购明细信息 ***/
			sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "记录用户的产品定购明细信息返回retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				EXEC SQL CLOSE vCurCancel;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "222333333333333333333333读取用户的产品定购信息fDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			Sinitn(temp_order_id);
       	    EXEC SQL FETCH vCurCancel into :temp_order_id;
       	}
       	EXEC SQL CLOSE vCurCancel;

	}

	/** 99-DSMP全业务退订 **/
	if ((strncmp(oprCode,BIZ_OPR_99,2) == 0)  || (strncmp(oprCode,BIZ_OPR_EXIT,2) == 0) )
		{
		strcpy(tBizData->id_no            ,idNo  );
		strcpy(tBizData->phone_no         ,phoneNo );
 		strcpy(tBizData->end_time         ,opTime );
	    strcpy(tBizData->opr_code         ,oprCode  );
	    strcpy(tBizData->opr_source       ,oprSource  );
	    strcpy(tBizData->multi_order_flag ,"1"  );
	    strcpy(tBizData->bizcode          ," "  );
	    strcpy(tBizData->spid             ," "  );
	    strcpy(tBizData->eff_time         ,opTime );
	    strcpy(tBizData->chg_flag         ,"0"  );
	    strcpy(tBizData->send_eff_time    ,opTime  );
        if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) ) {
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00－通过google搜索订购 99－本操作与手机搜索平台无关**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

	    /** 记录发送到同步接口的信息表 只能是1条 **/
		sprintf(retCode, "%06d", fPubDsmpSubInfo(tBizData , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "发送到同步接口的信息表 fPubDsmpSubInfo: retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpSubInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"911330");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

        /*DSMP全业务退订、销户:所有的全部退订(包含自定购的、赠送的别人、以及别人赠送自己的订购关系)*/
    	Sinitn(temp_sql);
    	if  (strncmp(temp_portalcode,"DSMP",4) == 0 )
    		{
		    sprintf(temp_sql, "select to_char(a.order_id) from DDSMPORDERMSG  a   "
		  	   " where ( a.id_no= to_number(:v1)  OR THIRD_ID_NO = to_number(:v2) )	"
		  	   "   and ( sysdate between a.EFF_TIME  and a.end_time) "
		  	   "   and a.serv_code in (select b.oprcode from SOBBIZTYPE_dsmp b Where b.PORTALCODE = 'DSMP'  "
		  	   "   OR b.oprcode = :v3 )  "   );
			}
		else
			{
		    sprintf(temp_sql, "select to_char(a.order_id) from DDSMPORDERMSG a    "
		  	   " where ( a.id_no= to_number(:v1)  OR THIRD_ID_NO = to_number(:v2) )	"
		  	   "   and ( sysdate between a.EFF_TIME  and a.end_time) "
		  	   "   and  a.serv_code = :v3  "  );
		}
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_sql=[%s]", temp_sql);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "bizType=[%s]",  bizType);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "idNo=[%s]", idNo);
#endif

        EXEC SQL PREPARE preStmt FROM :temp_sql;
	    EXEC SQL DECLARE vCurCancel2 CURSOR for preStmt;
		EXEC SQL OPEN vCurCancel2 using :idNo , :idNo ,:bizType ;
		EXEC SQL FETCH vCurCancel2 into :temp_order_id;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "SQLCODE=[%d]", SQLCODE);
#endif
		if (SQLCODE == 1403) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "没有找到该用户的产品定购信息[%d]!", SQLCODE);
			EXEC SQL CLOSE vCurCancel2;
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

        while(SQLCODE == 0)
        {
            Trim(temp_order_id);
			tBizData->order_id = atol(temp_order_id);
		    strcpy(tBizData->id_no            ,idNo  );
		    strcpy(tBizData->phone_no         ,phoneNo );

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_spBizCode=[%s]", temp_spBizCode);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_spId=[%s]",  temp_spId);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "temp_order_id=[%s]",  temp_order_id);
#endif
			/** 读取用户的产品定购信息 **/
			sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"00",retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "读取用户的产品定购信息retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				strcpy(retCode,"001305");
				EXEC SQL CLOSE vCurCancel2;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}

		    strcpy(temp_order_flag         ,"0"  );
 		    strcpy(tBizData->valid_flag     ,"0"       );
 		    strcpy(tBizData->end_time       ,opTime );
	        strcpy(tBizData->opr_code         ,oprCode  );
	        strcpy(tBizData->opr_source       ,oprSource  );

			/** 记录用户的产品定购明细信息 ***/
			sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "记录用户的产品定购信息 retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				EXEC SQL CLOSE vCurCancel2;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
       	    EXEC SQL FETCH vCurCancel2 into :temp_order_id;
       	}
       	EXEC SQL CLOSE vCurCancel2;
	}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "二次确认判断oprCode=[%s]", oprCode);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "二次确认判断temp_reconfirm_flag=[%s]", temp_reconfirm_flag);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "第三方确认判断temp_thirdvalidate=[%s]", temp_thirdvalidate);
#endif

    /** 需要二次确认的产品定购的处理   第三方确认判断 (strcmp(temp_thirdvalidate, "1") == 0)  **/
    /** 二次确认与全业务退订返回不生效，其他场景均返回生效
    0－订购关系生效
    1－订购关系不生效,二次确认或全业务退订应答时,填1
    9－其它，DSMP应答BOSS的第三方确认请求时，填9
    ***/
    if (strcmp(temp_reconfirm_flag, "1") == 0) {
    	strcpy(vEfftFlag, "1");

    	/** 调用二次确认函数进行二次确认 **/
    	sprintf(retCode, "%06d", fPubDsmpReConfirm(tBizData,retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "二次确认函数信息返回retCode=[%s]" ,retCode);
    	if (strcmp(retCode, "000000") != 0)
    	{
    		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpReConfirm:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }
    else if (strncmp(oprCode,BIZ_OPR_99,2) == 0) {
        strcpy(vEfftFlag, "1");
    }
    else if (strncmp(oprCode,BIZ_OPR_89,2) == 0) {
        strcpy(vEfftFlag, "1");
    }
    else {
        strcpy(vEfftFlag, "0");
    }

    /** 只有BOSS渠道，才发送信息到平台 **/
    /** DSMP业务已经在上面同步过了订购关系**/
    if ((strcmp(tBizData->comb_flag,"1") != 0 ) && (strncmp(temp_portalcode2,"DSMP",4) != 0 )
          &&  (strcmp(temp_thirdvalidate, "1") != 0) )
      {
        EXEC SQL  SELECT nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' '),nvl(portalcode,' ')
	                into  :temp_spId,:temp_spBizCode,:temp_portalcode
	              FROM  SOBBIZTYPE_dsmp
	    		 WHERE  oprcode = :bizType ;
        if  (SQLCODE != 0)
        {
	    	strcpy(retCode,"791725");
        	sprintf(retMsg, "读取SOBBIZTYPE_dsmp出错[%s][%d]!", bizType,SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
		Trim(temp_spBizCode);
		Trim(temp_spId);
		Trim(temp_portalcode);
		if (strlen(temp_spId) > 0 )
			{
		    strcpy(spId, " ");
		}
		if (strlen(temp_spBizCode) > 0 )
			{
		    strcpy(spBizCode, " ");
		}

		if ((strncmp(oprCode,"99",2) == 0 ) && (strncmp(temp_portalcode,"DSMP",4) == 0 ))
			{
		    strcpy(temp_biz_type, "00");
		}

        #ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......发送接口数据到DSMP平台");
        #endif
        memset(dynStmt, 0, sizeof(dynStmt));
    	/**sParamStr 组成：业务类型/工号/工号归属/OPCODE/TOTALDATE
    						/IDTYPE（默认01）/手机号码/操作代码
    						/SP企业代码/SP业务代码/信息代码/信息值    bizType    tBizData->serv_code
    						/业务密码/操作来源/操作时间/用户状态
    						/状态变更时间/操作备注   **/
        sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
           		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                    				spId,spBizCode, infoCode,infoValue, sCustWebPwd,tBizData->opr_source ,sTmpTime[0],
                    				vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	/** 调用接口发送程序对接口信息进行处理 **/
    	sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "调用接口发送fDsmpInterfaceSend()对接口信息进行处理返回retCode=[%s][%s]" ,retCode,retMsg);
    	if (strcmp(retCode, "000000") != 0)
    	{
    		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInterfaceSend:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }

end_valid:

    /** sprintf ( vEfftFlag,"%s%s",vEfftFlag,sTmpTime[0]);   ***/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "++ retCode = [%s] [%s] ++", retCode, retMsg);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "++ vEfftFlag = [%s]   ++", vEfftFlag);
#endif

	if (strcmp(retCode, "000000") == 0)
	{
		return  0;
	}
	else
	{
		return  atoi(retCode);
	}
}


