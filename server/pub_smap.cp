#include "boss_srv.h"
#include "publicsrv.h"

/*包含ORACLE的SQL通信区*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/************************************************
  * Auth: PeiJH
  * Name: pubSmapSend
  * Func: 向智能网发送信息
  * Date: 2003/10/14
*************************************************/
int pubSmapSend(char *InWork_No,  char *InOp_Code, char *InOrg_Code,
                char *InCmdText,  char *InAll_No,   int  vDelay_Time,
                char *InMax_Accept,char *Return_Message, char *InBatchType)
{
	char	TmpSqlStr[1024];

	int	ret=0;
	EXEC SQL BEGIN DECLARE SECTION;
		char pWork_No[6+1];
		char pOp_Code[4+1];
		char pOrg_Code[9+1];
		char pCmdText[1024];
		char pAll_No[20+1];
		char pLogin_Accept[14+1];
		char pBatchType[1+1];
		
		char vGroupId[10+1];
		char vOrgId[10+1];
	EXEC SQL END   DECLARE SECTION;

	init(pWork_No);
	init(pOp_Code);
	init(pOrg_Code);
	init(pLogin_Accept);
	init(pCmdText);
	init(pAll_No);
	init(Return_Message);
	init(pBatchType);
	memset(vGroupId,0,sizeof(vGroupId));
	memset(vOrgId,0,sizeof(vOrgId));
	
	strcpy(pWork_No,InWork_No);
	strcpy(pOp_Code,InOp_Code);
	strcpy(pOrg_Code,InOrg_Code);
	strcpy(pCmdText,InCmdText);
	strcpy(pAll_No,InAll_No);
	strcpy(pLogin_Accept,InMax_Accept);
	strcpy(pBatchType, InBatchType);


	pubLog_Debug(_FFL_,"","pLogin_Accept","[%s,%d]", pLogin_Accept,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pOp_Code","[%s,%d]", pOp_Code,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pCmdText","[%s,%d]", pCmdText,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pWork_No","[%s,%d]", pWork_No,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pAll_No","[%s,%d]", pAll_No,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pOrg_Code","[%s,%d]", pOrg_Code,strlen(pLogin_Accept));
	pubLog_Debug(_FFL_,"","pBatchType","[%s,%d]", pBatchType,strlen(pLogin_Accept));


	/*--组织机构改造获取org_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWork_No,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_,"", "%06d", "获取工号归属失败[%s]",ret,pWork_No);
		return ret;
	}
	Trim(vOrgId);
	pubLog_Debug(_FFL_,"","vOrgId","[%s,%d]", vOrgId,strlen(pLogin_Accept));
	/*---组织机构改造获取org_id edit by magang at 18/04/2009---end*/
	/*--组织机构改造获取group_id edit by magang at 16/04/2009--begin*/
	EXEC SQL SELECT a.group_id
			INTO :vGroupId
 			FROM dchngroupmsg a,dchngroupinfo b
 			WHERE b.group_id  = :vOrgId and b.parent_group_id = a.group_id  and a.class_code='12';

	if(SQLCODE != 0&&SQLCODE !=1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%d", "获取工号父节点失败",SQLCODE);
		return -1003;
	}
	if(SQLCODE == 1403)
	{
		pubLog_Debug(_FFL_,"","vOrgId[%s]","工号无县市级父节点", vOrgId);
		strcpy(vGroupId,vOrgId);
	}
	Trim(vGroupId);
	pubLog_Debug(_FFL_,"","vGroupId","[%s,%d]", vGroupId,strlen(pLogin_Accept));
	/*---组织机构改造获取group_id edit by magang at 16/04/2009---end*/
    /* 向SMAP发送命令 */
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"INSERT INTO SMAP_SEND(Login_Accept,Op_Code,Send_Time, "
			"Send_Message, send_type, batch_flag, login_no, all_no, region_code, "
			"district_code,group_id) values(to_number(:v1), :v2, sysdate, :v3, '0', :v4, :v5, "
			":v6, substr(:v7,1,2), substr(:v8,3,2),:v9)");

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","sqlstr","[%s]", TmpSqlStr);
#endif
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :pLogin_Accept, :pOp_Code, :pCmdText, :pBatchType,
									:pWork_No, :pAll_No, :pOrg_Code,:pOrg_Code,:vGroupId;

	if(SQLCODE != 0)
	{
		sprintf(Return_Message,"向SMAP发送命令失败! errcode:%d", sqlca.sqlcode);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320002", Return_Message);
		return 320002;
	}

    /* 读取 OP_CODE 对应的滞留时间 */
	EXEC SQL SELECT nvl(Delay_Time, 9)   INTO  :vDelay_Time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(Return_Message,"读取业务代码对应的滞留时间错误!code:%d", SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320003", Return_Message);
		return 320003;
	}

	return 0;
}


/******************************************************
  * Auth: PeiJH
  * Name: pubSmapBack
  * Func: 接受智能网返回信息
  * Date: 2003/10/14
*******************************************************/
int pubSmapBack(char *InWork_No, char *InOp_Code, char *InMax_Accept,
                char *OutBack_Code,char *OutBack_Desc )
{
	EXEC SQL BEGIN DECLARE SECTION;
		char pWork_No[6+1];
		char pOp_Code[4+1];
		char pAll_No[20+1];
		char pMax_Accept[14+1];
		
		char vRet_Code[1+1];
		int  iRet_Code=0;
		char vBack_Code[4+1];
		char vBack_Desc[256];
	EXEC SQL END DECLARE SECTION;

	init(pWork_No);
	init(pOp_Code);
	init(pAll_No);
	init(pMax_Accept);
	init(vRet_Code);
	init(vBack_Code);
	init(vBack_Desc);
	
	strcpy(pWork_No,InWork_No);
	strcpy(pOp_Code,InOp_Code);
	strcpy(pMax_Accept,InMax_Accept);
	
	EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'未知错误')
			INTO :vBack_Code, :vBack_Desc
			FROM  Smap_Back
			WHERE  Login_Accept = to_number(:pMax_Accept)
				AND  Back_Code is not null;
	if (SQLCODE == 1403)
	{
		/* 无返回信息 */
		vRet_Code[0] = '9';
		strcpy(vBack_Code,"ZZZZ");
		strcpy(vBack_Desc,"SMAP无返回信息!");
	}
	
	strcpy(OutBack_Code,vBack_Code);
	strcpy(OutBack_Desc,vBack_Desc);
	iRet_Code = atoi(vRet_Code);
	return iRet_Code;
}

/***************************************************
  * Auth: PeiJH
  * Name: pubSmapBatchSend
  * Func: 接受智能网返回信息
  * Input:批量流水 插入的一条记录 操作代码 集团代码 操作工号
  * Output:错误代码 错误消息
  * Date: 2003/10/14
*****************************************************/
void pubSmapBatchSend(char *InMax_Accept,char *InNumList,char *InOp_Code,
                      char *InAll_No,    char *InWork_No ,
                      char *Return_Code, char *Return_Message)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char    pMax_Accept[14+1];          /* 流水号  */
		char    pNumList[255+1];            /* 插入的一条记录 */
		char    pOp_Code[4+1];              /* 操作代码 */
		char    pAll_No[10+1];              /*  */
		char    pWork_No[6+1];              /* 操作工号 */
	EXEC SQL END DECLARE SECTION;

	/* 初试化参数 */
	init(pNumList);     init(pOp_Code);
	init(pAll_No);      init(pWork_No);     init(Return_Message);
	
	/* 取得输入参数 */
	strcpy(pMax_Accept, InMax_Accept);
	strcpy(pNumList,    InNumList);
	strcpy(pOp_Code,    InOp_Code);
	strcpy(pAll_No,     InAll_No);
	strcpy(pWork_No,    InWork_No);


	#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","pMax_Accept","[%s]", pMax_Accept);
		pubLog_Debug(_FFL_,"","pNumList","[%s]", pNumList);
		pubLog_Debug(_FFL_,"","pOp_Code","[%s]", pOp_Code);
		pubLog_Debug(_FFL_,"","pAll_No","[%s]", pAll_No);
		pubLog_Debug(_FFL_,"","pWork_No","[%s]", pWork_No);
	#endif

	EXEC SQL INSERT INTO Smap_Batch_Record
				(Login_Accept, Op_Code , Crea_Time,
				Resp_Time,    NumList,  Ret_Code,
				Login_No,     All_No,   Op_Type)
			VALUES(to_number(:pMax_Accept),:pOp_Code,sysdate,
				sysdate,     :pNumList,  'ZZZZ' ,
				:pWork_No,   :pAll_No,   '0');
	if ( SQLCODE != 0 )
	{
		sprintf(Return_Message,"插入Smap_Batch_Record表失败!code:%d",SQLCODE);
		strcpy(Return_Code,"320090");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320090", Return_Message);
	}
}

/*******************************************************************
  * Auth: huhx
  * Name: pubSmapBatchSend1
  * Func: 接受智能网返回信息
  * Input:批量流水 插入的批量纪录记录 操作代码 集团代码 操作工号
  * Output:错误代码 错误消息
  * Date: 2003/10/14
********************************************************************/
int pubSmapBatchSend1(char *InMax_Accept,char *InNumList,char *InOp_Code,
                      char *InAll_No,char *InWork_No , char *Return_Message,char *tok)
{
	char *tmpstr[20];
	int retCode=0;
	int i=0;
	
	EXEC SQL BEGIN DECLARE SECTION;
	char    pMax_Accept[14+1];          /* 流水号  */
	char    pNumList[2048+1];            /* 插入的一条记录 */
	char    pOp_Code[4+1];              /* 操作代码 */
	char    pAll_No[10+1];              /*  */
	char    pWork_No[6+1];              /* 操作工号 */
	char	vNumList[255+1];
	char	vPhoneNo[15+1];
	int		vNum = 0;

	EXEC SQL END DECLARE SECTION;
	
	/* 初试化参数 */
	init(pNumList);     init(pOp_Code);
	init(pAll_No);      init(pWork_No);     init(Return_Message);
	init(vNumList);
	
	/* 取得输入参数 */
	strcpy(pMax_Accept, InMax_Accept);
	strcpy(pNumList,    InNumList);
	strcpy(pOp_Code,    InOp_Code);
	strcpy(pAll_No,     InAll_No);
	strcpy(pWork_No,    InWork_No);

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","pMax_Accept","[%s]", pMax_Accept);
		pubLog_Debug(_FFL_,"","pNumList","[%s]", pNumList);
		pubLog_Debug(_FFL_,"","pOp_Code","[%s]", pOp_Code);
		pubLog_Debug(_FFL_,"","pAll_No","[%s]", pAll_No);
		pubLog_Debug(_FFL_,"","pWork_No","[%s]", pWork_No);
	#endif

	retCode = gettok(pNumList, tok, tmpstr);
	for(i=0;i<20;i++)
	{
		if ( atol(tmpstr[i]) < 10 )
			break;
		init(vNumList);		init(vPhoneNo);
		strcpy(vNumList, tmpstr[i]);
		memcpy(vPhoneNo, vNumList, 11);
		if ( atoi(pOp_Code) == 3330 && atoi(pAll_No) != 6)
		{
			vNum = 0;
			EXEC SQL SELECT COUNT(*) INTO :vNum FROM dVpmnUsrMsg
			WHERE real_no = :vPhoneNo;
			if ( SQLCODE != 0 )
			{
				sprintf(Return_Message,"查询dVPmnUsrMsg表失败!code:%d",SQLCODE);
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"", "320080", Return_Message);
				return 320080 ;
			}
			if ( vNum > 0 )
			{
				sprintf(Return_Message,"用户%s是VPMN集团成员,不能申请Iuser业务",vPhoneNo);
				pubLog_DBErr(_FFL_,"", "320081", Return_Message);
				return 320081 ;
			}
		}
		if ( atoi(pOp_Code) == 3332 )
		{
			vNum = 0;
			EXEC SQL SELECT COUNT(*) INTO :vNum FROM sIuserUsrGroupMsg
					 WHERE  phone_no = :vPhoneNo and group_no = 6;
			if ( SQLCODE != 0 )
			{
				sprintf(Return_Message,"查询sIuserUsrGroupMsg表失败!code:%d",SQLCODE);
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"", "320082", Return_Message);
				return 320082 ;
			}
			if ( vNum > 0 )
			{
				sprintf(Return_Message,"用户%s是彩铃用户，请首先关闭彩铃业务",vPhoneNo);
				pubLog_DBErr(_FFL_,"", "320084", Return_Message);
				return 320084 ;
			}
		}

		pubLog_Debug(_FFL_,"","pMax_Accept","[%s]", pMax_Accept);
		pubLog_Debug(_FFL_,"","pNumList","[%s]", pNumList);
		pubLog_Debug(_FFL_,"","pOp_Code","[%s]", pOp_Code);
		pubLog_Debug(_FFL_,"","pAll_No","[%s]", pAll_No);
		pubLog_Debug(_FFL_,"","pWork_No","[%s]", pWork_No);

		EXEC SQL INSERT INTO Smap_Batch_Record
							(Login_Accept, Op_Code , Crea_Time,
							 Resp_Time,    NumList,  Ret_Code,
							 Login_No,     All_No,   Op_Type)
					  VALUES(to_number(:pMax_Accept),:pOp_Code,sysdate,
							 sysdate,     :vNumList,  'ZZZZ' ,
							 :pWork_No,   :pAll_No,   '0');
		if ( SQLCODE != 0 ){
			sprintf(Return_Message,"插入Smap_Batch_Record表失败!code:%d",SQLCODE);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "320091", Return_Message);
			return 320091 ;
		}
	}
	EXEC SQL COMMIT WORK;
	return 0;
}

int sendCmd(char *InCmdCode,int type,char *InPhoneNo,char *return_msg,
				char *InSysDate, char *pWorkNo, char *pOpCode, char *pOrgCode,
									char *InLoginAccept,char *InTotalDate)
{

	int		ret=0;
	EXEC SQL BEGIN DECLARE SECTION;
		char    vCust_Id[10+1];              /* 客户ID     */
		char    vId_No[14+1];                /* 用户ID     */
		char    vBelong_Code[7+1];            /* 归属代码   */
		char    vSm_Code[2+1];                /* 业务类型   */
		char    vSwitch_No[20+1];
		char    vSim_No[20+1];
		char    vOffonAccept[14+1];
		char    vHlr_Code[1+1];
		char    vSysDate[20+1];
		char    vRun_Code[1+1];               /* 运行状态 */
		char    vBusiness_status[1+1];        /* 商业标志   */
		char	vOpCode[4+1];
		char	vWorkNo[6+1];
		char 	vOrgCode[9+1];
		char	vPhoneNo[15+1];
		char	vLoginAccept[14+1];
		char 	vTotal_Date[8+1];
		char	vCmdCode[2+1];
		char	vScp_Code[10+1];

		char vOrgId[10+1];
		char vGroupId[10+1];

	EXEC SQL END DECLARE SECTION;

	init(vCust_Id);		init(vId_No);		init(vBelong_Code);
	init(vSm_Code);		init(vSwitch_No);	init(vSim_No);
	init(vOffonAccept);	init(vHlr_Code);	init(vSysDate);
	init(vRun_Code);	init(vBusiness_status);
	init(vWorkNo);		init(vOpCode);
	init(vOrgCode);		init(vPhoneNo);
	init(vLoginAccept); init(vTotal_Date);
	init(vCmdCode);
	memset(vOrgId,0,sizeof(vOrgId));
	memset(vGroupId,0,sizeof(vGroupId));

	strcpy(vSysDate, InSysDate);
	strcpy(vOpCode,  pOpCode);
	strcpy(vWorkNo,  pWorkNo);
	strcpy(vOrgCode, pOrgCode);
	strcpy(vPhoneNo, InPhoneNo);
	strcpy(vLoginAccept, InLoginAccept);
	strcpy(vTotal_Date, InTotalDate);
	sprintf(vBusiness_status,"%d",type);
	strcpy(vCmdCode, InCmdCode);

	/*--组织机构改造获取group_id edit by magang at 16/04/2009--begin*/
	ret=0;
	ret = sGetUserGroupid(vPhoneNo,vGroupId);
	if(ret <0)
	{
		strcpy(return_msg,"查询用户group_id失败!");
		pubLog_DBErr(_FFL_,"","%06d", return_msg,ret);
		return ret ;
	}
	Trim(vGroupId);
	/*---组织机构改造获取group_id edit by magang at 16/04/2009---end*/
	/*--组织机构改造获取org_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(vWorkNo,vOrgId);
	if(ret !=0)
	{
		strcpy(return_msg,"查询工号org_id失败!");
		pubLog_DBErr(_FFL_,"","%06d", return_msg,ret);
		return ret;
	}
	Trim(vOrgId);
	/*---组织机构改造获取org_id edit by magang at 16/04/2009---end*/

	/*取用户信息*/
	EXEC SQL SELECT id_no ,CUST_ID,belong_code,sm_code ,substr(run_code,2,1)
			   INTO :vId_No,:vCust_Id,:vBelong_Code,:vSm_Code,:vRun_Code
			   FROM dCustMsg
			  WHERE phone_no = :vPhoneNo
				AND substr(run_code,2,1)<'a';
	if (SQLCODE != 0){
		sprintf(return_msg,"查询dCustMsg表发生异常[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1001 ;
	}

	/* 发送开关机命令 */
	EXEC SQL SELECT SWITCH_NO,SIM_NO INTO :vSwitch_No,:vSim_No
			   FROM dCustSim
			   WHERE Id_No=to_number(:vId_No);
	if (SQLCODE != 0){
		strcpy(return_msg,"查询dCustSim表发生异常!");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1002 ;
	}

	init(vScp_Code);
	EXEC SQL SELECT scp_code INTO :vScp_Code
		FROM   sScpCode
		WHERE  command_code = :vCmdCode and
			Phone_Head=substr(:vPhoneNo,1,7);
	if (SQLCODE != 0)
	{
		if ( SQLCODE == 1403 )
		{
			strcpy(vScp_Code,"1111111111");
		} 
		else
		{
			strcpy(return_msg,"查询sScpCode表发生异常!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1003 ;
		}
	}

	EXEC SQL SELECT Hlr_Code INTO :vHlr_Code
			 FROM   sHlrCode
			 WHERE  Phoneno_Head=substr(:vPhoneNo,1,7);
	if (SQLCODE != 0){
		strcpy(return_msg,"查询sHlrCode表发生异常!");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1004 ;
	}

	EXEC SQL SELECT sOffOn.nextval INTO :vOffonAccept FROM DUAL;
	/*---组织机构改造插入字段 edit by magang at 16/04/2009---*/
	EXEC SQL INSERT INTO wSndCmdDay
						(COMMAND_ID,  HLR_CODE,    COMMAND_ORDER,ID_NO,
						BELONG_CODE, SM_CODE,     PHONE_NO,     NEW_PHONE,
						IMSI_NO,     NEW_IMSI,    OTHER_CHAR,   OP_CODE,
						TOTAL_DATE,  OP_TIME,     LOGIN_NO,     ORG_CODE,
						LOGIN_ACCEPT,REQUEST_TIME,BUSINESS_STATUS,
						SEND_STATUS, SEND_TIME,   COMMAND_CODE,GROUP_ID,ORG_ID)
			  VALUES(to_number(:vOffonAccept),:vHlr_Code, 0, to_number(:vId_No),
					 :vBelong_Code,:vSm_Code,rtrim(:vPhoneNo), rtrim(:vPhoneNo),
					 :vSwitch_No,  rtrim(:vScp_Code),:vSim_No,:vOpCode,
					 to_number(:vTotal_Date),sysdate,
					 :vWorkNo,   :vOrgCode,
					 to_number(:vLoginAccept),
					 TO_DATE(:vSysDate,'YYYYMMDD HH24:MI:SS'),:vBusiness_status,
					 '0',TO_DATE(:vSysDate,'YYYYMMDD HH24:MI:SS'),:vCmdCode,:vGroupId,:vOrgId);
	if (SQLCODE != 0){
		sprintf(return_msg,"插入wSndCmdDay表发生异常![%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1004 ;
	}
	return 0;
}
/***********************************
  * Auth: huhx
  * Name: publicSmapBack
  * Func: 接受智能网返回信息
  * Date: 2004/01/04
************************************/
int publicSmapBack(char *InWork_No, char *InOp_Code, char *InMax_Accept, int vDelayTime, char *return_msg )
{
	int  i = 0;
	int  flag=0;
	char TmpSqlStr[1024];

	EXEC SQL BEGIN DECLARE SECTION;
		char pWork_No[6+1];
		char pOp_Code[4+1];
		char pAll_No[20+1];
		char pMax_Accept[14+1];
		char vBack_Code[4+1];
		char vBack_Desc[256];
		char delay_time[10+1];
	EXEC SQL END   DECLARE SECTION;

	init(pWork_No); 	init(pOp_Code);		init(pAll_No);
	init(pMax_Accept);  init(vBack_Code);	init(vBack_Desc);
	
	strcpy(pWork_No,InWork_No);
	strcpy(pOp_Code,InOp_Code);
	strcpy(pMax_Accept,InMax_Accept);

	/* 读取 OP_CODE 对应的滞留时间 */
	EXEC SQL SELECT nvl(Delay_Time, 9) INTO  :delay_time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(return_msg,"读取业务代码(OP_CODE)对应的滞留时间错误!code:%d",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320003", return_msg);
		return 320003;
	}

	while ( i < atoi(delay_time) )
	{
		EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'未知错误')
				   INTO :vBack_Code, :vBack_Desc
				   FROM  Smap_Back
				  WHERE  Login_Accept = to_number(:pMax_Accept);
		if (SQLCODE == 1403){
			i++;
			sleep(1);
			continue;
		}
		if ( SQLCODE == 0 )
		{
			flag = 1;
			break;
		}else{
			flag = 2;
		}
		i++;
		sleep(1);
	}

	if ( flag != 1 )
	{
		/* 无返回信息  删除smap_send指令(要考虑交易超时冲正)*/
		init(TmpSqlStr);
	/*---组织机构改造插入字段 edit by magang at 16/04/2009---*/
		sprintf(TmpSqlStr, "INSERT INTO Smap_Send_Error(Login_Accept,Op_Code,Send_Time,"
						   "Send_Message,Send_Type,  Batch_Flag, Login_No,"
						   "All_No,Region_Code,District_Code,oper_note,group_id) "
						   "SELECT login_accept,op_code,send_time, "
						   "send_message, send_type, batch_flag, login_no, "
						   "all_no,region_code,district_code,'操作超时',group_id FROM smap_send "
						   "WHERE login_accept = to_number(:v1)");

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","TmpSqlStr","[%s]", TmpSqlStr);
#endif

		EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
		EXEC SQL EXECUTE sql_stmt using :pMax_Accept;

		if(SQLCODE != 0)
		{
			sprintf(return_msg,"SMAP发送失败命令入库错误! errcode:%d", sqlca.sqlcode);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "320003", return_msg);
			return 32;
		}

		EXEC SQL COMMIT WORK ;
		strcpy(return_msg,"SMAP无返回信息!");
		 pubLog_DBErr(_FFL_,"", "3", return_msg);
		return 3;
	}

	if ( atoi(vBack_Code) != 0 )
	{
		strcpy(return_msg, vBack_Desc);
		pubLog_DBErr(_FFL_,"", vBack_Code, return_msg);
		return atoi(vBack_Code);
	}
	strcpy(return_msg,"处理成功");
	return 0;
}

/************************************************
  * Auth:
  * Name: pubPttSend
  * Func: PTT发送信息
  * Date: 2006/02/13
*************************************************/
int pubPttSend(char *InWork_No,  char *InOp_Code, char *InOrg_Code,
               char *InCmdText,  char *InAll_No,  char *InPttType,
               char *InMax_Accept,char *InCustType,  char *Return_Message)
{
	char	TmpSqlStr[1024];
	int			ret=0;
	EXEC SQL BEGIN DECLARE SECTION;
		char pWork_No[6+1];
		char pOp_Code[4+1];
		char pOrg_Code[9+1];
		char pCmdText[256];
		char pAll_No[20+1];
		char pLogin_Accept[14+1];
		char pPttType[1+1];
		char pCustType[1+1];
		
		char vGroupId[10+1];
		char vOrgId[10+1];
	EXEC SQL END   DECLARE SECTION;

	init(pWork_No);
	init(pOp_Code);
	init(pOrg_Code);
	init(pLogin_Accept);
	init(pCmdText);
	init(pAll_No);
	init(Return_Message);
	init(pPttType);
	init(pCustType);
	memset(vGroupId,0,sizeof(vGroupId));
	memset(vOrgId,0,sizeof(vOrgId));

	strcpy(pWork_No,InWork_No);
	strcpy(pOp_Code,InOp_Code);
	strcpy(pOrg_Code,InOrg_Code);
	strcpy(pCmdText,InCmdText);
	strcpy(pAll_No,InAll_No);
	strcpy(pLogin_Accept,InMax_Accept);
	strcpy(pPttType,InPttType);
	strcpy(pCustType,InCustType);


#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","pLogin_Accept","[%s]", pLogin_Accept);
	pubLog_Debug(_FFL_,"","pOp_Code","[%s]", pOp_Code);
	pubLog_Debug(_FFL_,"","pCmdText","[%s]", pCmdText);
	pubLog_Debug(_FFL_,"","pWork_No","[%s]", pWork_No);
	pubLog_Debug(_FFL_,"","pAll_No","[%s]", pAll_No);
	pubLog_Debug(_FFL_,"","pOrg_Code","[%s]", pOrg_Code);
	pubLog_Debug(_FFL_,"","pPttType","[%s]", pPttType);
	pubLog_Debug(_FFL_,"","pCustType","[%s]", pCustType);
#endif

	/*--组织机构改造获取org_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWork_No,vOrgId);
	if(ret <0)
	{
		printf("获得工号归属出错");
		pubLog_DBErr(_FFL_,"", "%06d", "获取工号归属失败",ret);
		return ret;
	}
	Trim(vOrgId);
	/*---组织机构改造获取org_id edit by magang at 18/04/2009---end*/

	/* 向PTT发送命令 */
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"INSERT INTO PTT_SEND(LOGIN_ACCEPT,OP_CODE,SEND_TIME,SEND_MESSAGE, "
			"SEND_TYPE,PTT_TYPE,ALL_NO,LOGIN_NO,REGION_CODE,DIST_CODE,TOWN_CODE,CUST_TYPE,GROUP_ID) "
			"values(to_number(:v1), :v2, sysdate, :v3, '0', :v4, :v5, "
			":v6, substr(:v7,1,2), substr(:v8,3,2),substr(:v9,5,3),:v10,:v11)");

	#ifdef _DEBUG_
		pubLog_DBErr(_FFL_,"", "sqlstr", "[%s]",TmpSqlStr);
	#endif

	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :pLogin_Accept,:pOp_Code,:pCmdText,:pPttType,
					:pAll_No,:pWork_No,:pOrg_Code,:pOrg_Code,:pOrg_Code,:pCustType,:vOrgId;

	if(SQLCODE != 0)
	{
		sprintf(Return_Message,"向PTT发送命令失败! errcode:%d", sqlca.sqlcode);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320101", Return_Message);
		return 320101;
	}
	return 0;
}

/***********************************
  * Auth:
  * Name: pubPttBack
  * Func: 接受PTT回信息
  * Date: 2006/02/13
************************************/
int pubPttBack(char *InWork_No, char *InOp_Code, char *InMax_Accept, char *return_msg )
{
	int  i = 0;
	int  flag=0;
	char TmpSqlStr[1024];
	
	EXEC SQL BEGIN DECLARE SECTION;
		char pWork_No[6+1];
		char pOp_Code[4+1];
		char pAll_No[20+1];
		char pMax_Accept[14+1];
		char vBack_Code[5+1];
		char vBack_Desc[256];
		char delay_time[10+1];
	EXEC SQL END   DECLARE SECTION;
	
	init(pWork_No);
	init(pOp_Code);
	init(pAll_No);
	init(pMax_Accept);
	init(vBack_Code);
	init(vBack_Desc);

	strcpy(pWork_No,InWork_No);
	strcpy(pOp_Code,InOp_Code);
	strcpy(pMax_Accept,InMax_Accept);
	
	/* 读取 OP_CODE 对应的滞留时间 */
	EXEC SQL SELECT nvl(Delay_Time, 9)
			INTO  :delay_time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(return_msg,"读取业务代码%s对应的滞留时间错误!%d",pOp_Code,SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320102", return_msg);
		return 320102;
	}

	while ( i < atoi(delay_time) )
	{
		EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'未知错误')
				   INTO :vBack_Code, :vBack_Desc
				   FROM  PTT_Back
				  WHERE  Login_Accept = to_number(:pMax_Accept);
		if (SQLCODE == 1403)
		{
			i++;
			sleep(1);
			continue;
		}
		if ( SQLCODE == 0 )
		{
			flag = 1;
			break;
		}
		else
		{
			flag = 2;
		}
		i++;
		sleep(1);
	}

	if ( flag != 1 )
	{
		strcpy(return_msg,"PPT无返回信息!");
		pubLog_DBErr(_FFL_,"", "320103", return_msg);
		return 320103;
	}

	if ( atoi(vBack_Code) != 0 )
	{
		strcpy(return_msg, vBack_Desc);
		pubLog_DBErr(_FFL_,"", vBack_Code, return_msg);
		return atoi(vBack_Code);
	}
	strcpy(return_msg,"处理成功");
	return 0;
}

/***********************************
* Auth:cencm
* Name: PubSendCmd
* Func: 发送、取消签约信息
* Date: 2011-11-1 9:23:16
*输入参数:
	指令代码--56 签约信息
	操作类型1：增加 0: 取消
	操作号码
	错误返回信息
	操作时间
	操作工号
	操作功能代码
	操作工号归属
	操作流水号
	操作日期
	操作类型--0：智能网 1：家庭V网 2：集团双V网
说明:发送签约信息 因存在二次利用号码 所以对于签约信息都重新发送 以免引起客户投诉
************************************/

int PubSendCmd(char *InCmdCode,int type,char *InPhoneNo,char *return_msg,
				char *InSysDate, char *pWorkNo, char *pOpCode, char *pOrgCode,
									char *InLoginAccept,char *InTotalDate,char *Op_Type)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vSysDate[20+1];
		char	vRun_Code[1+1];			/* 运行状态 */
		char	vBusiness_status[1+1];	/* 商业标志   */
		char	vOpCode[4+1];
		char	vWorkNo[6+1];
		char	vOrgCode[9+1];
		char	vPhoneNo[15+1];
		char	vLoginAccept[14+1];
		char 	vTotal_Date[8+1];
		char	vCmdCode[2+1];
		char	vScp_Code[10+1];
		char	vOpType[1+1];
		
		int		vCount1 = 0,vCount2=0,vCount3=0;
	EXEC SQL END DECLARE SECTION;

	init(vSysDate);			init(vOpCode);
	init(vBusiness_status);	init(vWorkNo);
	init(vOrgCode);			init(vPhoneNo);
	init(vLoginAccept); 	init(vTotal_Date);
	init(vCmdCode);

	strcpy(vSysDate, InSysDate);
	strcpy(vOpCode,  pOpCode);
	strcpy(vWorkNo,  pWorkNo);
	strcpy(vOrgCode, pOrgCode);
	strcpy(vPhoneNo, InPhoneNo);
	strcpy(vLoginAccept, InLoginAccept);
	strcpy(vTotal_Date, InTotalDate);
	sprintf(vBusiness_status,"%d",type);
	strcpy(vCmdCode, InCmdCode);
	strcpy(vOpType,Op_Type);
	
	if(strcmp(vBusiness_status,"0") == 0)/*删除签约信息*/
	{
		/*判定是否是智能网用户*/
		vCount1=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount1
				FROM dVpmnUsrMsg
				WHERE real_no = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"查询dVpmnUsrMsg表发生异常!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1005 ;
		}
		/*判定是否为家庭V网用户*/
		vCount2=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount2
				FROM dFamVpmnUser
				WHERE subno = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"查询dFamVpmnUser表发生异常!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1006 ;
		}
		/*判断是否为集团双V网用户*/
		vCount3=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount3
				FROM DMINIVPMNUSRMSG
				WHERE REAL_NO = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"查询dVpmnUsrMsg表发生异常!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1007 ;
		}
		if(strcmp(vOpType,"0") == 0)/*智能网删除签约信息*/
		{
			if(vCount2>0 || vCount3>0)
			{
				return 0;
			}
		}
		else if (strcmp(vOpType,"1") == 0)/*家庭V网删除签约信息*/
		{
			if(vCount1>0 || vCount3>0)
			{
				return 0;
			}
		}
		else if(strcmp(vOpType,"2") == 0)/*集团双V网删除签约信息*/
		{
			if(vCount1>0 || vCount2>0)
			{
				return 0;
			}
		}
	}
	return sendCmd(InCmdCode,type,InPhoneNo,return_msg,InSysDate,pWorkNo,pOpCode,pOrgCode,InLoginAccept,InTotalDate);
}
