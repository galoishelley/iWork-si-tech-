/**********�������֯�ṹ����(�޸Ķ�)*********/
/*******�������֯�ṹ���ڸ��� edit by  zhaoyan 07/05/2009 <sXXXXCfm(opCode1,opCode3....)> ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubProcOrder.h"

EXEC SQL INCLUDE SQLCA;
char* strSplit(char* str, char* dealim, int i);

/**********************************************
 @FUNC   fDsmpPlatformOrder    ҵ��ƽ̨����DSMP��Ʒ��������
 @input  params: pOrderData -> ��Ʒ������Ϣ
 @input  params: return_mess -> ������Ϣ
 @input  params: vEfftFlag -> ������ϵ��Ч��־:
 			(����ҵ��ƽ̨���÷���ʱ����Ҫ���صĲ���)
			0��������ϵ��Ч
			1��������ϵ����Ч������ȷ�ϻ�ȫҵ���˶�Ӧ��ʱ����1
			9��������DSMPӦ��BOSS�ĵ�����ȷ������ʱ����9
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
  	char 		vRslt[2+1];     /**�������  ������������д**/
  	char 		modeCode[8+1];

	/** ���岿�ֱ��� **/
	char 		temp_runcode[1 +1];
	char 		temp_third_runcode[1 +1];
	char 		temp_serv_attr[1+1];         /*ҵ������	G:ȫ��ҵ��,L:����ҵ��,Ĭ��Ϊȫ��ҵ��**/
	char 		temp_reconfirm[1+1];         /*�Ƿ���Ҫ����ȷ��  0:������ȷ��, 1:����ȷ��,Ĭ��Ϊ����ȷ��**/
	char 		temp_thirdvalidate[1+1];     /*�Ƿ���ҵ��ƽ̨��������ȷ�� 0:����Ҫ,1:��Ҫ,Ĭ��Ϊ����Ҫ **/
	char 		temp_order_flag[1+1];        /**������Ϣ 1:������Ʒ   0:�˶��������Ʒ **/
	char 		temp_orderadd_flag[1+1];     /**����������Ϣ 1:������������Ϣ   0:������Ϣ���� **/
	char 		temp_spId[20+1];
	char 		temp_spBizCode[21+1];
	char 		temp_source_type[6 +1];      /** BOSS:BOSS��������������Ӫҵ����WWWӪҵ����10086 �� **/
	char 		temp_portalcode2[4+1];
	char 		temp_reconfirm_flag[1+1];    /** ����ȷ�ϱ�־ **/
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
	char 		temp_other_info[200+1];     /** ����������Blackberryҵ�� ��Ҫ���ӿ�ISMI�����**/
	char 		temp_default_oper[2+1];	   /** �������ȱʡ�Ĳ�������**/

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

    /***��ȡϵͳ����ʱ�� */
	Sinitn(totalDate); Sinitn(opTime); Sinitn(yearMonth); Sinitn(curTime);
	EXEC SQL	SELECT 	to_char(sysdate, 'yyyymmdd'),
						to_char(sysdate,'yyyymmdd hh24:mi:ss'),
						to_char(sysdate, 'yyyymm'),
						to_char(sysdate,'hh24miss')
				  INTO :totalDate,:opTime, :yearMonth, :curTime  FROM DUAL;
    if (SQLCODE != 0) {
    	strcpy(retCode,"791760");
    	sprintf(retMsg, "��ȡϵͳ����ʱ�����[%d]!" ,SQLCODE);
    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }

    /***��ȡ��������Ա��Ϣ */
    EXEC SQL	SELECT 	org_code, nvl(trim(ip_address),'172.0.0.1')
			  	INTO 	:orgCode, :ipAddr
			  	FROM 	dLoginMsg
			 	WHERE 	login_no = :tBizData->login_no ;

    if (SQLCODE == 1403){
        strcpy(orgCode,"0101790in");
        strcpy(ipAddr, "172.0.0.1");
    }else if (SQLCODE != 0) {
    	strcpy(retCode,"791761");
    	sprintf(retMsg, "��ȡ��dLoginMsg����[%s][%d]!", tBizData->login_no,SQLCODE);
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
        	sprintf(retMsg, "��ȡDDSMPSPINFO���е�ҵ�����ͳ���[%d]!" ,SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        if(SQLCODE == 1403)
        {
        	strcpy(retCode,"123456");
        	strcpy(retMsg, "��ҵ���벻����!");
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
    	sprintf(retMsg, "��ȡ��dCustMsg����[%s][%d]!", phoneNo,SQLCODE);
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
    		sprintf(retMsg, "�û��ѵ���ͣ��[%s]!", temp_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if ((strcmp(temp_runcode,"F") == 0) ||  (strcmp(temp_runcode,"G") == 0)||(strcmp(temp_runcode,"H") == 0)||(strcmp(temp_runcode,"L") == 0)||(strcmp(temp_runcode,"M") == 0) ){
    		strcpy(retCode,"911321");
    		sprintf(retMsg, "�û���ͣ��[%s]!", temp_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if  ((strcmp(temp_runcode,"I") == 0)||(strcmp(temp_runcode,"c") == 0)||(strcmp(temp_runcode,"b") == 0)||(strcmp(temp_runcode,"a") == 0)||(strcmp(temp_runcode,"j") == 0)||(strcmp(temp_runcode,"l") == 0)) {
    		strcpy(retCode,"911322");
    		sprintf(retMsg, "�û���Ԥ����[%s]!", temp_runcode);
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
    

    /*�жϺ���״̬�Ƿ���������    */
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
	  			  FROM cChgRun
	  			 WHERE op_code=:opCode and run_code=:temp_runcode
	  			   AND region_code= substr(:belongCode ,1,2);
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    strcpy(retCode,"791734");
	    sprintf(retMsg, "�û���ǰ״̬[%s]��������!", temp_runcode);
	    PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		goto end_valid ;
	}

    /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
    memset(sTmpTime, 0, sizeof(sTmpTime));
    if((i = fPubFormatTime(sTmpTime[0],opTime)) != 0)
    {
        sprintf(retCode, "791732");
        sprintf(retMsg,"op_time����fPubFormatTimeʧ��! iRet=[%d]", i);
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
    }

    /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
    if((i = fPubFormatTime(sTmpTime[1],custRunTime)) != 0)
    {
        sprintf(retCode, "791733");
        sprintf(retMsg,"op_time����fPubFormatTimeʧ��! iRet=[%d]", i);
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

    /*���ϵͳ������ˮ **/
    lLoginAccept =  atol(loginAccept);
    if ( lLoginAccept <= 0 )
    	{
	    EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :lLoginAccept from dual;
	    if(lLoginAccept <= 0)
	    {
	    	sprintf(retCode, "%06d", ERR_GET_MAX_SYS_ACCEPT);
	    	sprintf(retMsg, "ȡ������ˮ�Ų��ɹ���������ȷ��!");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    sprintf(loginAccept, "%ld", lLoginAccept);
    }

	/* ҵ��������Ϣ��¼(�����ҵ��ע��������޸������������) */
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
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�����������newPasswd = [%s]", sCustWebPwd);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "idNo = [%s]", idNo);
#endif

    /** ��ȡ�������û���Ϣ **/
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
    		sprintf(retMsg, "��ȡ�������û��ı�dCustMsg����[%s][%d]!", thirdNo,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    if(strcmp(oprCode,"02")!=0 && strcmp(oprCode,"07")!=0 && strcmp(oprCode,"89")!=0 && strcmp(oprCode,"99")!=0)
    {
    	if ((strcmp(temp_third_runcode,"D") == 0)||(strcmp(temp_third_runcode,"E") == 0)) {
    		strcpy(retCode,"911320");
    		sprintf(retMsg, "�������û��ѵ���ͣ��[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if ((strcmp(temp_third_runcode,"F") == 0) ||  (strcmp(temp_third_runcode,"G") == 0)||(strcmp(temp_third_runcode,"H") == 0)||(strcmp(temp_third_runcode,"L") == 0)||(strcmp(temp_third_runcode,"M") == 0) ){
    		strcpy(retCode,"911321");
    		sprintf(retMsg, "�������û���ͣ��[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    	if  ((strcmp(temp_third_runcode,"I") == 0)||(strcmp(temp_third_runcode,"c") == 0)||(strcmp(temp_third_runcode,"b") == 0)||(strcmp(temp_third_runcode,"a") == 0)||(strcmp(temp_third_runcode,"j") == 0)||(strcmp(temp_third_runcode,"l") == 0)) {
    		strcpy(retCode,"911322");
    		sprintf(retMsg, "�������û�Ԥ����[%s]!", temp_third_runcode);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
    }

    	/*�жϵ������û�״̬�Ƿ��������� **/
	    strcpy(vallow_flag,"0");
	    EXEC SQL	SELECT allow_flag
	    			  INTO :vallow_flag
	      			  FROM cChgRun
	      			 WHERE op_code=:opCode and run_code=:temp_third_runcode
	      			   AND region_code= substr(:belongCode ,1,2);
	    if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	    {
	        strcpy(retCode,"791734");
	        sprintf(retMsg, "�������û���ǰ״̬[%s]��������!", temp_third_runcode);
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

    /** �Խṹ�������������Ϣ **/
    strcpy(tBizData->id_no            ,idNo  );
    strcpy(tBizData->third_id_no      ,thirdIdNo  );
    strcpy(tBizData->opr_time         ,opTime  );
    strcpy(tBizData->belong_code      ,belongCode  );
    strcpy(tBizData->total_date       ,totalDate  );
    strcpy(tBizData->login_accept     ,loginAccept);

    /**71|�㶫ͨ��������Ҫ�޸�ҵ������,����ӿڶ�����д��� **/
    if ((strcmp(tBizData->serv_code,"17" ) == 0) && (strncmp(tBizData->spid,"719",3 ) == 0))
    	{
        strcpy(temp_biz_type,"71");
    }
    else
    	{
        strcpy(temp_biz_type,tBizData->serv_code);
    }

    /** Blackberryҵ�� ��Ҫ��ƽ̨����IMSI����**/
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
	        sprintf(retMsg, "�����õ�IMSI�������!" );
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    Trim(temp_other_info);
    }

    	/** BOSS������ѯ  **/
		EXEC SQL	SELECT  nvl(SOURCE_TYPE,' ')  into :temp_source_type
		          FROM SOBOPRSOURCE
				 WHERE type_code  = :oprSource  ;
    	if  ((SQLCODE != 0) &&(SQLCODE != 1403))
    	{
    		strcpy(retCode,"791790");
    		sprintf(retMsg, "��ȡ���������SOBOPRSOURCE����[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		    Trim(temp_source_type);

    	/** BOSSƽ̨��ѯ  **/
		EXEC SQL	SELECT	portalcode
					into 	:temp_portalcode2
					from  	SOBBIZTYPE_dsmp
				 	WHERE 	oprcode  = :bizType  ;
    	if  ((SQLCODE != 0) &&(SQLCODE != 1403))
    	{
    		strcpy(retCode,"791791");
    		sprintf(retMsg, "��ȡ���������SOBBIZTYPE_dsmp����[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		    Trim(temp_portalcode2);

    /** ��������Ʒ�������Ч�� **/
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
	        sprintf(retMsg, "��ȡsBillMOdeCode����ͨ��Ʒ��Ϣʧ��[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    if  (iRet < 1)
	    {
	        strcpy(retCode,"791765");
	        sprintf(retMsg, "�������ͨ��Ʒ����[%s]����Ч�Ĳ�Ʒ����!", tBizData->mode_code);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }

	    /** ��ȡ�����DSMP��Ʒ��Ϣ**/
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
	        sprintf(retMsg, "��ȡDSMP�ڲ�ת����ϵ��SDSMPPRODTRANS��Ϣʧ��[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	    if  (iRet < 1)
	    {
	        strcpy(retCode,"791767");
	        sprintf(retMsg, "�������ͨ��Ʒ����[%s]û����������ҵ�����Ʒ!", tBizData->mode_code);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
	    	goto end_valid ;
	    }
	}
	else
		{  /**    b.acid_flag = '0' ��ԭ�Ӳ�Ʒ**/
        /** ������ҵ���ҵ���������OPRCODE ����Ч�Խ����ж�**/
	    EXEC SQL	SELECT 	count(*)
	    		  	INTO 	:iRet
	    		  	FROM 	SOBBIZOPERCODE
	    		 	WHERE 	oprcode = :oprCode AND instr(bizlist,:bizType) >0 ;
        if (SQLCODE != 0)
        {
        	strcpy(retCode,"791762");
        	sprintf(retMsg, "��ȡ��SOBBIZOPERCODE����[%d]!",SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        if (iRet < 1 )
        {
        	strcpy(retCode,"791716");
        	sprintf(retMsg, "��������OPERCODE[%s]�������� !",bizType );
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
	        sprintf(retMsg, "��ȡSPҵ���Ӧ�������Ʒ��Ϣʧ��[%d]!", SQLCODE);
	        PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
        }
        else if (SQLCODE == 0)
        	{
            /** ֧����Ե���SPҵ��Ĳ�Ʒ�������ж����Żݴ��� **/
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
        	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", bizType,SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        Trim(temp_portalcode);
	}

    /** �����û������Ĳ�Ʒ��Ϣ 1.���������Ʒ���ڶ��SPID�����ȴ���
         2����δ���OPRCODEҵ�����������DSMP���ͻ�Ĳ�������(������������ ������ͬ�ģ����������岻ͬ�Ĳ���)
         3�������DSMP��صĲ������� **/
    if (strcmp(tBizData->comb_flag,"1") == 0 )
    	{  /** �����Ʒ�Ĵ��� **/
        if  ( (strncmp(tBizData->opr_code,BIZ_OPR_REGIST,2) == 0 )
              || (strncmp(tBizData->opr_code,BIZ_OPR_DEST,2) == 0)|| (strncmp(tBizData->opr_code,BIZ_OPR_13,2) == 0 )
              ||(strncmp(tBizData->opr_code, BIZ_OPR_11,2) == 0&&strncmp(tBizData->serv_code, "19",2) == 0)){
                strcpy(temp_order_flag,"1"  );
        }else{
                strcpy(temp_order_flag,"0"  );
        }

        /** ��Ʒ������Ϣ���ж�**/
        if (strlen(infoCode) > 0 ) {
            strcpy(temp_orderadd_flag         ,"0"  );
        }

    }
  else if ((strncmp(oprCode,BIZ_OPR_14,2) == 0) || (strncmp(oprCode,BIZ_OPR_15,2) == 0)
               || (strncmp(oprCode,BIZ_OPR_SUSPEND,2) == 0) || (strncmp(oprCode,BIZ_OPR_RESUME,2) == 0)
               || ((strcmp(oprCode,"09") == 0) && (strcmp(bizType,"16") == 0))  )
      {
    	/*14-�û�������ͣ,15-�û������ָ�, 09-�û�������ͣ���ֻ�����ר�á�***/

		/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** DSMP�������ͼ�� **/
		sprintf(retCode, "%06d", fDsmpOprCodeCheck(bizType ,tBizData->opr_code,oprCode, retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "DSMP�������ͼ��retCode=[%s]" ,retCode);
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
 	else if ((strncmp(oprCode,BIZ_OPR_21,2) == 0) && (strncmp(bizType,"28",2) == 0)){    /**28|��Ϸҵ�� 21 - ��ֵ**/
		/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
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
	else if ((strncmp(oprCode,"03",2) == 0) && (strncmp(bizType,"88",2) == 0)){    /**88|BlackBerry  03-�û���Ϣ��� **/
		/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
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
		{/**08-��ҵ��ƽ̨���ϱ�� **/
		/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"01", retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
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
  	{/**06-���� 11-���� 21-����ȷ�� 01-ע��**/
        /** ��Բ���û��SPID����SPBizCode��ҵ����д���,��ʼ��Ϊϵͳ�����ȱʡֵ **/
        EXEC SQL  	SELECT 	nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ')
	        		into  	:temp_spId,:temp_spBizCode
	      			FROM  	SOBBIZTYPE_dsmp
	     			WHERE  	oprcode = :bizType;
        if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
        {
        	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", bizType,SQLCODE);
        	strcpy(retCode,"791783");
        	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
        	goto end_valid ;
        }
        Trim(temp_spId);
        Trim(temp_spBizCode);
	    if (strlen(temp_spId) > 0 )
	    	{
	        strcpy(tBizData->spid,temp_spId);   /**��ʼ��Ϊϵͳ�����ȱʡֵ **/
	        strcpy(tBizData->send_spid," ");
	        strcpy(spId,temp_spId);
	    } else {
	        strcpy(tBizData->send_spid,tBizData->spid);
	    }

	    if (strlen(temp_spBizCode) > 0 )  {
	        strcpy(tBizData->bizcode,temp_spBizCode);   /**��ʼ��Ϊϵͳ�����ȱʡֵ **/
	        strcpy(tBizData->send_bizcode," ");
	        strcpy(spBizCode,temp_spBizCode);
	    } else {
	        strcpy(tBizData->send_bizcode,tBizData->bizcode);
	    }

		/** ���SP���롢SPҵ������Ƿ���á��Ƿ�Ϸ�   **/
		sprintf(retCode, "%06d", fDsmpSPBizCheck(spId, spBizCode, bizType, "A",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���SP���롢SPҵ����뷵��retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpSPBizCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** �û���Ϣ���ع��ܼ�� **/
		/** ����������� by zhangxina @ 20110328 **/
		sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(idNo, phoneNo, bizType, spId, spBizCode, belongCode,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�û���Ϣ���ع��ܼ��retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (strncmp(oprCode,BIZ_OPR_11,2) == 0)
		{
			/** �������û���Ϣ���ع��ܼ�� **/
			/** ����������� by zhangxina @ 20110328 **/
			sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(thirdIdNo, thirdNo, bizType, spId, spBizCode, belongCode,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�������û���Ϣ���ع��ܼ��retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
		}

		/** ȷ���û��Ƿ񶨹���ͬһҵ�� **/
		sprintf(retCode, "%06d", fDsmpProdOrderCheck(idNo, thirdIdNo, spId ,spBizCode , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "ȷ���û��Ƿ񶨹���ͬһҵ��retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpProdOrderCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** ȷ�ϸ�SPҵ����Ϣ **/
		EXEC SQL	SELECT 	SERVATTR,nvl(THIRDVALIDATE,' '),nvl(RECONFIRM,' ') ,nvl(BILLINGTYPE,' ')
		            INTO 	:temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		          	FROM 	DDSMPSPBIZINFO
				 	WHERE 	SPID = :spId   AND BIZCODE = :spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	if (SQLCODE != 0)
    	{
    		strcpy(retCode,"791735");
    		sprintf(retMsg, "��ȡSPҵ����Ϣ��DDSMPSPBIZINFO����[%s][%d]!", spId,SQLCODE);
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

		/** ���Ͷ���ȷ����Ϣ��  ���Žӿڱ�   **/
		/** BOSS֧�ֶ���ȷ�Ϸ������������ã� ����ȷ�Ϸ���������Ϊ�ջ������ʱ���û��������ⷵ�������Ϊ�û�ͬ�ⶩ��
		    ������ȷ�Ϸ���������1λ���λ����ʱ������û���������о�ȷƥ�䣬ƥ��ɹ�����ɶ�������  ***/
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
    		sprintf(retMsg, "��ȡ����ȷ�ϴ����SDSMPRECONFIRM����[%s][%d]!", spId,SQLCODE);
    		PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		goto end_valid ;
    	}
		Trim(temp_reconfirm_flag);

		/** ���ֽṹ�������ֵ **/

		strcpy(tBizData->chg_flag         ,"0"  );/*SPҵ��������Ķ���/�˶���Ҫ���˱����λΪ1��������0**/
		strcpy(tBizData->chrg_type        ,vchrg_type  );	/**�Ʒ�����**/

        if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0))
        	{
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00��ͨ��google�������� 99�����������ֻ�����ƽ̨�޹�**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

		strcpy(tBizData->first_order_date ,opTime  );
		strcpy(tBizData->first_login_no   ,loginNo  );
		strcpy(tBizData->last_order_date  ,opTime  );
		strcpy(tBizData->local_code       ,temp_serv_attr );	  /*ȫ����������־**/
		strcpy(tBizData->valid_flag       ,"1" );	          /*��Ч��־ **/
		strcpy(tBizData->reserved         ,"  "  );
		tBizData->order_id = lLoginAccept ;
		strcpy(tBizData->sm_code         ,smCode  );
		strcpy(tBizData->password        ,sCustWebPwd  );
	  	strcpy(tBizData->send_eff_time    ,opTime  );

	  	strcpy(temp_order_flag         ,"1"  );

		/** ������Ϣ���ӿڣ��ȴ�������ȷ�ϵĽӿڷ���  temp_reconfirm_flag **/
		if ( (strcmp(temp_thirdvalidate, "1") == 0) && (strcmp(temp_reconfirm_flag, "1") != 0))
			{
    		memset(dynStmt, 0, sizeof(dynStmt));
    		iRet =0;
			/**sParamStr ��ɣ�ҵ������/����/���Ź���/OPCODE/TOTALDATE
								/IDTYPE��Ĭ��01��/�ֻ�����/��������tBizData->opr_code,
								/SP��ҵ����/SPҵ�����/��Ϣ����/��Ϣֵ
								/ҵ������/������Դ/����ʱ��/�û�״̬
								/״̬���ʱ��/������ע
			 **/
			sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~21~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
       		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,
                				tBizData->spid,tBizData->bizcode, infoCode,infoValue,sCustWebPwd,tBizData->opr_source ,
                				sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);

			iRet = fCallInterface(dynStmt,retCode ,retMsg ,second_retCode);
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "������Ϣ���ӿڣ�������ȷ�ϵĽӿڷ���retCode=[%s]" ,retCode);
			if (iRet != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:���ýӿں�������second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		    strcpy(retCode,"791738");
    		    sprintf(retMsg, "fCallInterface:���ýӿں�������iRet=[%d]", iRet );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}

			if 	(strncmp(second_retCode, "05",2) == 0)
				{	/**05:ҵ��ƽ̨�Ѵ��ڶ�����ϵ **/
					if (strncmp(tBizData->opr_code, BIZ_OPR_DEST,2) == 0||strncmp(tBizData->opr_code, BIZ_OPR_13,2) == 0||(strncmp(tBizData->opr_code, BIZ_OPR_11,2) == 0&&strncmp(tBizData->serv_code, "19",2) == 0))
						{
						/** ���� BOSSϵͳ �� ҵ��ƽ̨ ���ݲ�һ������ ,�����������**/
					}
				  else {
					pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:ƽ̨���ش���second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		        strcpy(retCode,"791739");
    		        sprintf(retMsg, "fCallInterface:ƽ̨���ش���second_retCode=[%s] ", second_retCode );
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		        goto end_valid ;
					}
			}
			else if (strncmp(second_retCode, "00",2) != 0)
				{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:ƽ̨���ش���second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
    		    strcpy(retCode,"791740");
    		    sprintf(retMsg, "fCallInterface:ƽ̨���ش���second_retCode=[%s] ", second_retCode );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}

			else if (strncmp(retCode, "000000",6) != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fCallInterface:����ƽִ̨�к������� retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		    strcpy(retCode,"791741");
    		    sprintf(retMsg, "fCallInterface:����ƽִ̨�к������� retCode=[%s]", retCode );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    		    goto end_valid ;
			}
		}
		/**������ȷ�ϵĽӿڴ������ **/
		strcpy(vEfftFlag, "1");

        /** ��Ʒ������Ϣ���ж�**/
        if (strlen(infoCode) > 0)
        	{
            strcpy(temp_orderadd_flag         ,"0"  );
        }
    }
  	else if (strncmp(oprCode,BIZ_OPR_89,2) == 0)
  	{/**89-SPȫҵ���˶� **/

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
			sprintf(retMsg, "���Ҹ��û��Ĳ�Ʒ������Ϣʧ��[%s][%d]!", spId,SQLCODE);
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (iRet < 1) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "û���ҵ����û��Ĳ�Ʒ������Ϣ[%s][%d]!", spId,SQLCODE);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		/** ���SP���롢SPҵ������Ƿ���á��Ƿ�Ϸ�  **/
		sprintf(retCode, "%06d", fDsmpSPBizCheck(spId, spBizCode, bizType, "C",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���SP���롢SPҵ����뷵��retCode=[%s]" ,retCode);
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
  	{/**99-DSMPȫҵ���˶�,02-�û�ע��**/
    	Sinitn(temp_sql);
    	iRet = 0;
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ѯҵ��ƽ̨����temp_portalcode=[%s]",temp_portalcode);
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
			sprintf(retMsg, "���Ҹ��û��Ĳ�Ʒ������Ϣʧ��[%s][%d]!", bizType,SQLCODE);
			PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

		if (iRet < 1) {
			strcpy(retCode,"791717");
			sprintf(retMsg, "û���ҵ����û��Ĳ�Ʒ������Ϣ[%s][%d]!",bizType,SQLCODE);
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

    	strcpy(temp_order_flag         ,"0"  );

	}
	else if  (strncmp(oprCode,BIZ_OPR_UNDEST,2) == 0||strncmp(oprCode,BIZ_OPR_12,2) == 0)
		{/**07-�˶� **/
		/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid = [%s]", tBizData->spid);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode = [%s]", tBizData->bizcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag = [%s]", tBizData->valid_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time = [%s]", tBizData->end_time);
#endif

		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "07-�˶�   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "07-�˶�   tBizData->bizcode = [%s]", tBizData->bizcode);
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
	         {/**03- �����޸� , 09- �������� **/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->spid = [%s]", tBizData->spid);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->bizcode = [%s]", tBizData->bizcode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->valid_flag = [%s]", tBizData->valid_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "tBizData->end_time = [%s]", tBizData->end_time);
#endif

		sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "ȷ���û��Ƿ񶨹���ͬһҵ��retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"001305");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�����޸�   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�����޸�   oprCode = [%s]", oprCode);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�����޸�   old_password = [%s]", old_password);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�����޸�   tBizData->password = [%s]", tBizData->password);
#endif

        /** ����У��
        Trim(old_password);
        if ((strcmp(old_password,tBizData->password) != 0)  && (strncmp(oprCode,BIZ_OPR_CHGPWD,2) == 0) ){
		    sprintf(retMsg,"����У�����,����ľ��������û�ԭ�����벻һ��!" );
    	    strcpy(retCode,"791781");
    	    goto end_valid ;
        }**/

		strcpy(temp_order_flag         ,"0"  );
	    strcpy(tBizData->opr_code       ,oprCode  );
	    strcpy(tBizData->opr_source     ,oprSource  );
	    strcpy(tBizData->password       ,sCustWebPwd  );
	    strcpy(tBizData->send_eff_time    ,opTime  );

  }
  else {/** ��������Ƿ�  **/
		sprintf(retMsg,"��������opr_code�������[%s]!",oprCode);
    	strcpy(retCode,"791716");
    	pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	goto end_valid ;
	}

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "...��ʼ���ɲ�Ʒ������ϵ...        "        );
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

    /** �����Ʒ������ϵ  **/
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
			sprintf(retMsg, "û���ҵ����û��Ĳ�Ʒ������Ϣ[%d]!",  SQLCODE);
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

            /** ��Բ���û��SPID����SPBizCode��ҵ����д���,��ʼ��Ϊϵͳ�����ȱʡֵ **/
            EXEC SQL  SELECT nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ') ,nvl(DEFAULT_ORDEROPR,'06')
	            into  :temp_spId,:temp_spBizCode ,:temp_default_oper
	          FROM  SOBBIZTYPE_dsmp
	         WHERE  oprcode = :bizType;
            if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
            {
            	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", bizType,SQLCODE);
            	strcpy(retCode,"791783");
            	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
            	goto end_valid ;
            }

            Trim(temp_default_oper);
            strcpy(tBizData->opr_code        ,temp_default_oper  );

	    /** �û���Ϣ���ع��ܼ�� **/
		/** ����������� by zhangxina @ 20110328 **/
	    sprintf(retCode, "%06d", fDsmpInfoOnOffCheck(idNo, phoneNo, bizType, spId, spBizCode, belongCode,retMsg));
	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�û���Ϣ���ع��ܼ��retCode=[%s]" ,retCode);
	    if (strcmp(retCode, "000000") != 0)
		    {
		    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpInfoOnOffCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
		    	EXEC SQL CLOSE vCurCancel3;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		    	goto end_valid ;
		    }

		    /** ���SP���롢SPҵ������Ƿ���á��Ƿ�Ϸ�   **/
		    sprintf(retCode, "%06d", fDsmpSPBizCheck(temp_spId, temp_spBizCode, bizType, "A",retMsg));
		    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���SP���롢SPҵ����뷵��retCode=[%s]" ,retCode);
		    if (strcmp(retCode, "000000") != 0)
		    {
		    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpSPBizCheck:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
		    	EXEC SQL CLOSE vCurCancel3;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
		    	goto end_valid ;
		    }
		    /** ȷ�ϸ�SPҵ����Ϣ **/
		    EXEC SQL	SELECT SERVATTR,THIRDVALIDATE,RECONFIRM ,BILLINGTYPE
		                  INTO :temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		              FROM DDSMPSPBIZINFO
		    		 WHERE SPID = :temp_spId AND BIZCODE = :temp_spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	    if (SQLCODE != 0)
    	    {
    	    	strcpy(retCode,"791771");
    	    	sprintf(retMsg, "��ȡSPҵ����Ϣ��DDSMPSPBIZINFO����[%s][%d]!", spId,SQLCODE);
    	    	EXEC SQL CLOSE vCurCancel3;
    	    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }
					Trim(vchrg_type);
            /** �ж��û��Ƿ��������ҵ�� **/
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
				sprintf(retMsg,"�ж��û��Ƿ������ҵ��ʧ��[%d]!",SQLCODE);
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
			    	sprintf(retMsg,"�����[%s][%s]������������Ʒ��,�����ظ�����!",temp_spId ,temp_spBizCode );
			    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "  [%s]", retMsg);
			    	strcpy(retCode,"791769");
			    	EXEC SQL CLOSE vCurCancel3;
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			    	goto end_valid ;
			    }
			    else if (strncmp(tBizData->op_code,"9113",4) != 0 )
			    	{
			        /** ���¶�����Ϣ�������Ʒ **/
			        sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, "0",retMsg));
			        pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���¶�����Ϣ�������Ʒ retCode=[%s]" ,retCode);
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
		    strcpy(tBizData->chg_flag         ,"0"  );	        /*SPҵ��������Ķ���/�˶���Ҫ���˱����λΪ1��������0**/
		    strcpy(tBizData->chrg_type        ,vchrg_type  );	/**�Ʒ�����**/
            if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) )
            	{  /** 00��ͨ��google�������� 99�����������ֻ�����ƽ̨�޹�**/
		       		 strcpy(tBizData->channel_id       ,"00"  );
             }
            else {
                strcpy(tBizData->channel_id       ,"99"  );
            }

		    strcpy(tBizData->first_order_date ,opTime  );
		    strcpy(tBizData->first_login_no   ,loginNo  );
		    strcpy(tBizData->last_order_date  ,opTime  );
		    strcpy(tBizData->local_code       ,temp_serv_attr );	  /*ȫ����������־**/
		    strcpy(tBizData->valid_flag       ,"1" );	          /*��Ч��־ **/
		    strcpy(tBizData->reserved         ,"  "  );
		    tBizData->order_id = lLoginAccept ;
		    strcpy(tBizData->sm_code         ,smCode  );
		    strcpy(tBizData->password        ,oldPwd  );

	        if (strncmp(tBizData->op_code,"9113",4) == 0 )
	        	{
			    /** ��¼�û��Ĳ�Ʒ������ϸ��Ϣ ***/
			    sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��¼�û��Ĳ�Ʒ������Ϣ retCode=[%s]" ,retCode);
			    if (strcmp(retCode, "000000") != 0)
			    {
			    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			    	EXEC SQL CLOSE vCurCancel3;
    				pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			    	goto end_valid ;
			    }
       	        /**������̶����õ��Żݴ��� **/
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
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......���ͽӿ����ݵ�DSMPƽ̨");
            #endif
            memset(dynStmt, 0, sizeof(dynStmt));
    	    /**sParamStr ��ɣ�ҵ������/����/���Ź���/OPCODE/TOTALDATE
    	    					/IDTYPE��Ĭ��01��/�ֻ�����/��������
    	    					/SP��ҵ����/SPҵ�����/��Ϣ����/��Ϣֵ    bizType    tBizData->serv_code
    	    					/ҵ������/������Դ/����ʱ��/�û�״̬
    	    					/״̬���ʱ��/������ע   **/
            sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
               		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                        				tBizData->send_spid,tBizData->send_bizcode, infoCode,infoValue,
                        				sCustWebPwd,tBizData->opr_source ,sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	    /** ���ýӿڷ��ͳ���Խӿ���Ϣ���д��� **/
    	    sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ýӿڷ��ͳ���Խӿ���Ϣ����retCode=[%s]" ,retCode);
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

		    /** ȷ�ϸ�SPҵ����Ϣ **/
		    EXEC SQL	SELECT 	SERVATTR,THIRDVALIDATE,RECONFIRM ,BILLINGTYPE
		                INTO 	:temp_serv_attr , :temp_thirdvalidate,:temp_reconfirm,:vchrg_type
		              	FROM 	DDSMPSPBIZINFO
		    		 	WHERE 	SPID = :temp_spId AND BIZCODE = :temp_spBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    	    if (SQLCODE != 0)
    	    {
    	    	strcpy(retCode,"791771");
    	    	sprintf(retMsg, "��ȡSPҵ����Ϣ��DDSMPSPBIZINFO����[%s][%d]!", spId,SQLCODE);
    	    	EXEC SQL CLOSE vCurCancel4;
    	    	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    }
					Trim(vchrg_type);
		    sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "01",retMsg));
		    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
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

		    strcpy(tBizData->chg_flag         ,"0"  );	        /*SPҵ��������Ķ���/�˶���Ҫ���˱����λΪ1��������0**/
		    strcpy(tBizData->chrg_type        ,vchrg_type  );	/**�Ʒ�����**/
            if ((strcmp(oprSource,"01") == 0) || (strcmp(oprSource,"03") == 0) ) {
		        strcpy(tBizData->channel_id       ,"00"  );	    /** 00��ͨ��google�������� 99�����������ֻ�����ƽ̨�޹�**/
            } else {
                strcpy(tBizData->channel_id       ,"99"  );
            }

            /**  ��ʼ��Ϊϵͳ�����ȱʡֵ  **/
            EXEC SQL  SELECT  nvl(DEFAULT_CANCLEOPR,'07')
	            into   :temp_default_oper
	          FROM  SOBBIZTYPE_dsmp
	          WHERE  oprcode = :bizType;
            if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
            {
            	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", bizType,SQLCODE);
            	strcpy(retCode,"791783");
            	PUBLOG_DBDEBUG("fDsmpPlatformOrder");
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
            	goto end_valid ;
            }
            Trim(temp_default_oper);
            strcpy(tBizData->opr_code        ,temp_default_oper  );

		/** ��¼�û��Ĳ�Ʒ������ϸ��Ϣ ***/
		sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��¼�û��Ĳ�Ʒ������Ϣ retCode=[%s]" ,retCode);
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
		    	sprintf(retMsg, "��ȡdBillCustDetail��ʷ����ʧ��[%d]", SQLCODE);
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
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......���ͽӿ����ݵ�DSMPƽ̨");
            #endif
            memset(dynStmt, 0, sizeof(dynStmt));
    	    /**sParamStr ��ɣ�ҵ������/����/���Ź���/OPCODE/TOTALDATE
    	    					/IDTYPE��Ĭ��01��/�ֻ�����/��������
    	    					/SP��ҵ����/SPҵ�����/��Ϣ����/��Ϣֵ    bizType    tBizData->serv_code
    	    					/ҵ������/������Դ/����ʱ��/�û�״̬
    	    					/״̬���ʱ��/������ע   **/
            sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
               		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                        				tBizData->send_spid,tBizData->send_bizcode, infoCode,infoValue, sCustWebPwd,
                        				tBizData->opr_source ,sTmpTime[0],vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	    /** ���ýӿڷ��ͳ���Խӿ���Ϣ���д��� **/
    	    sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ýӿڷ��ͳ���Խӿ���Ϣ����retCode=[%s]" ,retCode);
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
        /* �������� ������Ϣ�޸ĵ�  �����漰������Ϣ�����ҵ��������� */
    	sprintf(retCode, "%06d", fPubDsmpOrderMsgAdd(tBizData,temp_order_flag,temp_orderadd_flag,retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "�������� ������Ϣ�޸ĵ�  �����漰������Ϣ���fPubDsmpOrderMsgAdd:retCode=[%s]" ,retCode);
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
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ɲ�Ʒ������ϵ   tBizData->order_id = [%ld]", tBizData->order_id);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ɲ�Ʒ������ϵ   lLoginAccept = [%ld]", lLoginAccept);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ɲ�Ʒ������ϵ   temp_reconfirm_flag = [%s]", temp_reconfirm_flag);
	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ɲ�Ʒ������ϵ   oprCode = [%s]", oprCode);
#endif
        /** ������Ϣ�Ĵ��� ***/
        if (strncmp(temp_orderadd_flag,"0",1) == 0)
        	{
    	    sprintf(retCode, "%06d", fPubDsmpOrderMsgAdd(tBizData,temp_order_flag,temp_orderadd_flag,retMsg));
    	    pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "������Ϣ�Ĵ���fPubDsmpOrderMsgAdd:retCode=[%s]" ,retCode);
    	    if (strcmp(retCode, "000000") != 0)
    	    	{
    	    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsgAdd:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
    	    	goto end_valid ;
    	    	}
        }

		/** ��¼�û��Ĳ�Ʒ������Ϣ ***/
		sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��¼�û��Ĳ�Ʒ������Ϣ����retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}
   }

	/** 89-SPȫҵ���˶�    **/
	if (strncmp(oprCode,BIZ_OPR_89,2) == 0)
		{
	    strcpy(tBizData->multi_order_flag ,"1"  ); /*���������Ĵ����־  1������;���ڿ�����ͬ���ӿڱ�ͬ��������ֻ��һ�� */
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
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00��ͨ��google�������� 99�����������ֻ�����ƽ̨�޹�**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

        /** ��¼���͵�ͬ���ӿڵ���Ϣ�� ֻ����1�� **/
		sprintf(retCode, "%06d", fPubDsmpSubInfo(tBizData , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���͵�ͬ���ӿڵ���Ϣ�� fPubDsmpSubInfo: retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpSubInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"911330");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

    	Sinitn(temp_sql);
    	/** ���˶����Ͷ�����ϵ��ֻ�˶��Զ����ͱ������͵�DSMP **/
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
			sprintf(retMsg, "û���ҵ���SP�Ķ�����Ϣ[%s][%d]!", spId, SQLCODE);
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

			/** ��ȡ�û��Ĳ�Ʒ������Ϣ    **/
			sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData , "00", retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢfDsmpGetOrderInfo: retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fDsmpGetOrderInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				strcpy(retCode,"001305");
				EXEC SQL CLOSE vCurCancel;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "22222222222222222222��ȡ�û��Ĳ�Ʒ������ϢfDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			strcpy(tBizData->opr_code       ,oprCode  );
		    strcpy(temp_order_flag         ,"0"  );
 		    strcpy(tBizData->valid_flag     ,"0"       );
 		    strcpy(tBizData->end_time       ,opTime );
	        strcpy(tBizData->opr_source     ,oprSource  );
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "44444444444444444444444443��ȡ�û��Ĳ�Ʒ������ϢfDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			/** ��¼�û��Ĳ�Ʒ������ϸ��Ϣ ***/
			sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��¼�û��Ĳ�Ʒ������ϸ��Ϣ����retCode=[%s]" ,retCode);
			if (strcmp(retCode, "000000") != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpOrderMsg:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
				EXEC SQL CLOSE vCurCancel;
    			pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
				goto end_valid ;
			}
 pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "222333333333333333333333��ȡ�û��Ĳ�Ʒ������ϢfDsmpGetOrderInfo: retCode=[%s]" ,retCode);

			Sinitn(temp_order_id);
       	    EXEC SQL FETCH vCurCancel into :temp_order_id;
       	}
       	EXEC SQL CLOSE vCurCancel;

	}

	/** 99-DSMPȫҵ���˶� **/
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
		    strcpy(tBizData->channel_id       ,"00"  );	    /** 00��ͨ��google�������� 99�����������ֻ�����ƽ̨�޹�**/
        } else {
            strcpy(tBizData->channel_id       ,"99"  );
        }

	    /** ��¼���͵�ͬ���ӿڵ���Ϣ�� ֻ����1�� **/
		sprintf(retCode, "%06d", fPubDsmpSubInfo(tBizData , retMsg));
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���͵�ͬ���ӿڵ���Ϣ�� fPubDsmpSubInfo: retCode=[%s]" ,retCode);
		if (strcmp(retCode, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "fPubDsmpSubInfo:retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
			strcpy(retCode,"911330");
    		pubLog_DBErr(_FFL_, "fDsmpPlatformOrder", retCode, retMsg);
			goto end_valid ;
		}

        /*DSMPȫҵ���˶�������:���е�ȫ���˶�(�����Զ����ġ����͵ı��ˡ��Լ����������Լ��Ķ�����ϵ)*/
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
			sprintf(retMsg, "û���ҵ����û��Ĳ�Ʒ������Ϣ[%d]!", SQLCODE);
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
			/** ��ȡ�û��Ĳ�Ʒ������Ϣ **/
			sprintf(retCode, "%06d", fDsmpGetOrderInfo(tBizData ,"00",retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��ȡ�û��Ĳ�Ʒ������ϢretCode=[%s]" ,retCode);
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

			/** ��¼�û��Ĳ�Ʒ������ϸ��Ϣ ***/
			sprintf(retCode, "%06d", fPubDsmpOrderMsg(tBizData, temp_order_flag,retMsg));
			pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "��¼�û��Ĳ�Ʒ������Ϣ retCode=[%s]" ,retCode);
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
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "����ȷ���ж�oprCode=[%s]", oprCode);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "����ȷ���ж�temp_reconfirm_flag=[%s]", temp_reconfirm_flag);
		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "������ȷ���ж�temp_thirdvalidate=[%s]", temp_thirdvalidate);
#endif

    /** ��Ҫ����ȷ�ϵĲ�Ʒ�����Ĵ���   ������ȷ���ж� (strcmp(temp_thirdvalidate, "1") == 0)  **/
    /** ����ȷ����ȫҵ���˶����ز���Ч������������������Ч
    0��������ϵ��Ч
    1��������ϵ����Ч,����ȷ�ϻ�ȫҵ���˶�Ӧ��ʱ,��1
    9��������DSMPӦ��BOSS�ĵ�����ȷ������ʱ����9
    ***/
    if (strcmp(temp_reconfirm_flag, "1") == 0) {
    	strcpy(vEfftFlag, "1");

    	/** ���ö���ȷ�Ϻ������ж���ȷ�� **/
    	sprintf(retCode, "%06d", fPubDsmpReConfirm(tBizData,retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "����ȷ�Ϻ�����Ϣ����retCode=[%s]" ,retCode);
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

    /** ֻ��BOSS�������ŷ�����Ϣ��ƽ̨ **/
    /** DSMPҵ���Ѿ�������ͬ�����˶�����ϵ**/
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
        	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", bizType,SQLCODE);
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
    		pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "......���ͽӿ����ݵ�DSMPƽ̨");
        #endif
        memset(dynStmt, 0, sizeof(dynStmt));
    	/**sParamStr ��ɣ�ҵ������/����/���Ź���/OPCODE/TOTALDATE
    						/IDTYPE��Ĭ��01��/�ֻ�����/��������
    						/SP��ҵ����/SPҵ�����/��Ϣ����/��Ϣֵ    bizType    tBizData->serv_code
    						/ҵ������/������Դ/����ʱ��/�û�״̬
    						/״̬���ʱ��/������ע   **/
        sprintf ( dynStmt,"%s~%s~%s~%s~%s~01~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s%s",
           		                    temp_biz_type,loginNo,belongCode,opCode,totalDate,tBizData->phone_no,tBizData->opr_code,
                    				spId,spBizCode, infoCode,infoValue, sCustWebPwd,tBizData->opr_source ,sTmpTime[0],
                    				vrun_code,sTmpTime[1],opNote,tBizData->channel_id,temp_other_info);
    	/** ���ýӿڷ��ͳ���Խӿ���Ϣ���д��� **/
    	sprintf(retCode, "%06d", fDsmpInterfaceSend(tBizData,dynStmt,"0",retMsg));
    	pubLog_Debug(_FFL_, "fDsmpPlatformOrder", "", "���ýӿڷ���fDsmpInterfaceSend()�Խӿ���Ϣ���д�����retCode=[%s][%s]" ,retCode,retMsg);
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


