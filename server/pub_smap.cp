#include "boss_srv.h"
#include "publicsrv.h"

/*����ORACLE��SQLͨ����*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/************************************************
  * Auth: PeiJH
  * Name: pubSmapSend
  * Func: ��������������Ϣ
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


	/*--��֯���������ȡorg_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWork_No,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_,"", "%06d", "��ȡ���Ź���ʧ��[%s]",ret,pWork_No);
		return ret;
	}
	Trim(vOrgId);
	pubLog_Debug(_FFL_,"","vOrgId","[%s,%d]", vOrgId,strlen(pLogin_Accept));
	/*---��֯���������ȡorg_id edit by magang at 18/04/2009---end*/
	/*--��֯���������ȡgroup_id edit by magang at 16/04/2009--begin*/
	EXEC SQL SELECT a.group_id
			INTO :vGroupId
 			FROM dchngroupmsg a,dchngroupinfo b
 			WHERE b.group_id  = :vOrgId and b.parent_group_id = a.group_id  and a.class_code='12';

	if(SQLCODE != 0&&SQLCODE !=1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%d", "��ȡ���Ÿ��ڵ�ʧ��",SQLCODE);
		return -1003;
	}
	if(SQLCODE == 1403)
	{
		pubLog_Debug(_FFL_,"","vOrgId[%s]","���������м����ڵ�", vOrgId);
		strcpy(vGroupId,vOrgId);
	}
	Trim(vGroupId);
	pubLog_Debug(_FFL_,"","vGroupId","[%s,%d]", vGroupId,strlen(pLogin_Accept));
	/*---��֯���������ȡgroup_id edit by magang at 16/04/2009---end*/
    /* ��SMAP�������� */
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
		sprintf(Return_Message,"��SMAP��������ʧ��! errcode:%d", sqlca.sqlcode);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320002", Return_Message);
		return 320002;
	}

    /* ��ȡ OP_CODE ��Ӧ������ʱ�� */
	EXEC SQL SELECT nvl(Delay_Time, 9)   INTO  :vDelay_Time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(Return_Message,"��ȡҵ������Ӧ������ʱ�����!code:%d", SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320003", Return_Message);
		return 320003;
	}

	return 0;
}


/******************************************************
  * Auth: PeiJH
  * Name: pubSmapBack
  * Func: ����������������Ϣ
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
	
	EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'δ֪����')
			INTO :vBack_Code, :vBack_Desc
			FROM  Smap_Back
			WHERE  Login_Accept = to_number(:pMax_Accept)
				AND  Back_Code is not null;
	if (SQLCODE == 1403)
	{
		/* �޷�����Ϣ */
		vRet_Code[0] = '9';
		strcpy(vBack_Code,"ZZZZ");
		strcpy(vBack_Desc,"SMAP�޷�����Ϣ!");
	}
	
	strcpy(OutBack_Code,vBack_Code);
	strcpy(OutBack_Desc,vBack_Desc);
	iRet_Code = atoi(vRet_Code);
	return iRet_Code;
}

/***************************************************
  * Auth: PeiJH
  * Name: pubSmapBatchSend
  * Func: ����������������Ϣ
  * Input:������ˮ �����һ����¼ �������� ���Ŵ��� ��������
  * Output:������� ������Ϣ
  * Date: 2003/10/14
*****************************************************/
void pubSmapBatchSend(char *InMax_Accept,char *InNumList,char *InOp_Code,
                      char *InAll_No,    char *InWork_No ,
                      char *Return_Code, char *Return_Message)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char    pMax_Accept[14+1];          /* ��ˮ��  */
		char    pNumList[255+1];            /* �����һ����¼ */
		char    pOp_Code[4+1];              /* �������� */
		char    pAll_No[10+1];              /*  */
		char    pWork_No[6+1];              /* �������� */
	EXEC SQL END DECLARE SECTION;

	/* ���Ի����� */
	init(pNumList);     init(pOp_Code);
	init(pAll_No);      init(pWork_No);     init(Return_Message);
	
	/* ȡ��������� */
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
		sprintf(Return_Message,"����Smap_Batch_Record��ʧ��!code:%d",SQLCODE);
		strcpy(Return_Code,"320090");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320090", Return_Message);
	}
}

/*******************************************************************
  * Auth: huhx
  * Name: pubSmapBatchSend1
  * Func: ����������������Ϣ
  * Input:������ˮ �����������¼��¼ �������� ���Ŵ��� ��������
  * Output:������� ������Ϣ
  * Date: 2003/10/14
********************************************************************/
int pubSmapBatchSend1(char *InMax_Accept,char *InNumList,char *InOp_Code,
                      char *InAll_No,char *InWork_No , char *Return_Message,char *tok)
{
	char *tmpstr[20];
	int retCode=0;
	int i=0;
	
	EXEC SQL BEGIN DECLARE SECTION;
	char    pMax_Accept[14+1];          /* ��ˮ��  */
	char    pNumList[2048+1];            /* �����һ����¼ */
	char    pOp_Code[4+1];              /* �������� */
	char    pAll_No[10+1];              /*  */
	char    pWork_No[6+1];              /* �������� */
	char	vNumList[255+1];
	char	vPhoneNo[15+1];
	int		vNum = 0;

	EXEC SQL END DECLARE SECTION;
	
	/* ���Ի����� */
	init(pNumList);     init(pOp_Code);
	init(pAll_No);      init(pWork_No);     init(Return_Message);
	init(vNumList);
	
	/* ȡ��������� */
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
				sprintf(Return_Message,"��ѯdVPmnUsrMsg��ʧ��!code:%d",SQLCODE);
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"", "320080", Return_Message);
				return 320080 ;
			}
			if ( vNum > 0 )
			{
				sprintf(Return_Message,"�û�%s��VPMN���ų�Ա,��������Iuserҵ��",vPhoneNo);
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
				sprintf(Return_Message,"��ѯsIuserUsrGroupMsg��ʧ��!code:%d",SQLCODE);
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"", "320082", Return_Message);
				return 320082 ;
			}
			if ( vNum > 0 )
			{
				sprintf(Return_Message,"�û�%s�ǲ����û��������ȹرղ���ҵ��",vPhoneNo);
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
			sprintf(Return_Message,"����Smap_Batch_Record��ʧ��!code:%d",SQLCODE);
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
		char    vCust_Id[10+1];              /* �ͻ�ID     */
		char    vId_No[14+1];                /* �û�ID     */
		char    vBelong_Code[7+1];            /* ��������   */
		char    vSm_Code[2+1];                /* ҵ������   */
		char    vSwitch_No[20+1];
		char    vSim_No[20+1];
		char    vOffonAccept[14+1];
		char    vHlr_Code[1+1];
		char    vSysDate[20+1];
		char    vRun_Code[1+1];               /* ����״̬ */
		char    vBusiness_status[1+1];        /* ��ҵ��־   */
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

	/*--��֯���������ȡgroup_id edit by magang at 16/04/2009--begin*/
	ret=0;
	ret = sGetUserGroupid(vPhoneNo,vGroupId);
	if(ret <0)
	{
		strcpy(return_msg,"��ѯ�û�group_idʧ��!");
		pubLog_DBErr(_FFL_,"","%06d", return_msg,ret);
		return ret ;
	}
	Trim(vGroupId);
	/*---��֯���������ȡgroup_id edit by magang at 16/04/2009---end*/
	/*--��֯���������ȡorg_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(vWorkNo,vOrgId);
	if(ret !=0)
	{
		strcpy(return_msg,"��ѯ����org_idʧ��!");
		pubLog_DBErr(_FFL_,"","%06d", return_msg,ret);
		return ret;
	}
	Trim(vOrgId);
	/*---��֯���������ȡorg_id edit by magang at 16/04/2009---end*/

	/*ȡ�û���Ϣ*/
	EXEC SQL SELECT id_no ,CUST_ID,belong_code,sm_code ,substr(run_code,2,1)
			   INTO :vId_No,:vCust_Id,:vBelong_Code,:vSm_Code,:vRun_Code
			   FROM dCustMsg
			  WHERE phone_no = :vPhoneNo
				AND substr(run_code,2,1)<'a';
	if (SQLCODE != 0){
		sprintf(return_msg,"��ѯdCustMsg�����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1001 ;
	}

	/* ���Ϳ��ػ����� */
	EXEC SQL SELECT SWITCH_NO,SIM_NO INTO :vSwitch_No,:vSim_No
			   FROM dCustSim
			   WHERE Id_No=to_number(:vId_No);
	if (SQLCODE != 0){
		strcpy(return_msg,"��ѯdCustSim�����쳣!");
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
			strcpy(return_msg,"��ѯsScpCode�����쳣!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1003 ;
		}
	}

	EXEC SQL SELECT Hlr_Code INTO :vHlr_Code
			 FROM   sHlrCode
			 WHERE  Phoneno_Head=substr(:vPhoneNo,1,7);
	if (SQLCODE != 0){
		strcpy(return_msg,"��ѯsHlrCode�����쳣!");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1004 ;
	}

	EXEC SQL SELECT sOffOn.nextval INTO :vOffonAccept FROM DUAL;
	/*---��֯������������ֶ� edit by magang at 16/04/2009---*/
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
		sprintf(return_msg,"����wSndCmdDay�����쳣![%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
		return -1004 ;
	}
	return 0;
}
/***********************************
  * Auth: huhx
  * Name: publicSmapBack
  * Func: ����������������Ϣ
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

	/* ��ȡ OP_CODE ��Ӧ������ʱ�� */
	EXEC SQL SELECT nvl(Delay_Time, 9) INTO  :delay_time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(return_msg,"��ȡҵ�����(OP_CODE)��Ӧ������ʱ�����!code:%d",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320003", return_msg);
		return 320003;
	}

	while ( i < atoi(delay_time) )
	{
		EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'δ֪����')
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
		/* �޷�����Ϣ  ɾ��smap_sendָ��(Ҫ���ǽ��׳�ʱ����)*/
		init(TmpSqlStr);
	/*---��֯������������ֶ� edit by magang at 16/04/2009---*/
		sprintf(TmpSqlStr, "INSERT INTO Smap_Send_Error(Login_Accept,Op_Code,Send_Time,"
						   "Send_Message,Send_Type,  Batch_Flag, Login_No,"
						   "All_No,Region_Code,District_Code,oper_note,group_id) "
						   "SELECT login_accept,op_code,send_time, "
						   "send_message, send_type, batch_flag, login_no, "
						   "all_no,region_code,district_code,'������ʱ',group_id FROM smap_send "
						   "WHERE login_accept = to_number(:v1)");

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","TmpSqlStr","[%s]", TmpSqlStr);
#endif

		EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
		EXEC SQL EXECUTE sql_stmt using :pMax_Accept;

		if(SQLCODE != 0)
		{
			sprintf(return_msg,"SMAP����ʧ������������! errcode:%d", sqlca.sqlcode);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "320003", return_msg);
			return 32;
		}

		EXEC SQL COMMIT WORK ;
		strcpy(return_msg,"SMAP�޷�����Ϣ!");
		 pubLog_DBErr(_FFL_,"", "3", return_msg);
		return 3;
	}

	if ( atoi(vBack_Code) != 0 )
	{
		strcpy(return_msg, vBack_Desc);
		pubLog_DBErr(_FFL_,"", vBack_Code, return_msg);
		return atoi(vBack_Code);
	}
	strcpy(return_msg,"����ɹ�");
	return 0;
}

/************************************************
  * Auth:
  * Name: pubPttSend
  * Func: PTT������Ϣ
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

	/*--��֯���������ȡorg_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWork_No,vOrgId);
	if(ret <0)
	{
		printf("��ù��Ź�������");
		pubLog_DBErr(_FFL_,"", "%06d", "��ȡ���Ź���ʧ��",ret);
		return ret;
	}
	Trim(vOrgId);
	/*---��֯���������ȡorg_id edit by magang at 18/04/2009---end*/

	/* ��PTT�������� */
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
		sprintf(Return_Message,"��PTT��������ʧ��! errcode:%d", sqlca.sqlcode);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320101", Return_Message);
		return 320101;
	}
	return 0;
}

/***********************************
  * Auth:
  * Name: pubPttBack
  * Func: ����PTT����Ϣ
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
	
	/* ��ȡ OP_CODE ��Ӧ������ʱ�� */
	EXEC SQL SELECT nvl(Delay_Time, 9)
			INTO  :delay_time
			FROM Smap_Delay
			WHERE Op_Code = :pOp_Code;
	if(SQLCODE != 0)
	{
		sprintf(return_msg,"��ȡҵ�����%s��Ӧ������ʱ�����!%d",pOp_Code,SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320102", return_msg);
		return 320102;
	}

	while ( i < atoi(delay_time) )
	{
		EXEC SQL SELECT ltrim(Back_Code),nvl(ltrim(Back_Desc),'δ֪����')
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
		strcpy(return_msg,"PPT�޷�����Ϣ!");
		pubLog_DBErr(_FFL_,"", "320103", return_msg);
		return 320103;
	}

	if ( atoi(vBack_Code) != 0 )
	{
		strcpy(return_msg, vBack_Desc);
		pubLog_DBErr(_FFL_,"", vBack_Code, return_msg);
		return atoi(vBack_Code);
	}
	strcpy(return_msg,"����ɹ�");
	return 0;
}

/***********************************
* Auth:cencm
* Name: PubSendCmd
* Func: ���͡�ȡ��ǩԼ��Ϣ
* Date: 2011-11-1 9:23:16
*�������:
	ָ�����--56 ǩԼ��Ϣ
	��������1������ 0: ȡ��
	��������
	���󷵻���Ϣ
	����ʱ��
	��������
	�������ܴ���
	�������Ź���
	������ˮ��
	��������
	��������--0�������� 1����ͥV�� 2������˫V��
˵��:����ǩԼ��Ϣ ����ڶ������ú��� ���Զ���ǩԼ��Ϣ�����·��� ��������ͻ�Ͷ��
************************************/

int PubSendCmd(char *InCmdCode,int type,char *InPhoneNo,char *return_msg,
				char *InSysDate, char *pWorkNo, char *pOpCode, char *pOrgCode,
									char *InLoginAccept,char *InTotalDate,char *Op_Type)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vSysDate[20+1];
		char	vRun_Code[1+1];			/* ����״̬ */
		char	vBusiness_status[1+1];	/* ��ҵ��־   */
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
	
	if(strcmp(vBusiness_status,"0") == 0)/*ɾ��ǩԼ��Ϣ*/
	{
		/*�ж��Ƿ����������û�*/
		vCount1=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount1
				FROM dVpmnUsrMsg
				WHERE real_no = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"��ѯdVpmnUsrMsg�����쳣!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1005 ;
		}
		/*�ж��Ƿ�Ϊ��ͥV���û�*/
		vCount2=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount2
				FROM dFamVpmnUser
				WHERE subno = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"��ѯdFamVpmnUser�����쳣!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1006 ;
		}
		/*�ж��Ƿ�Ϊ����˫V���û�*/
		vCount3=0;
		EXEC SQL SELECT COUNT(*) INTO :vCount3
				FROM DMINIVPMNUSRMSG
				WHERE REAL_NO = :vPhoneNo;
		if (SQLCODE != 0)
		{
			strcpy(return_msg,"��ѯdVpmnUsrMsg�����쳣!");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"", "%06d", return_msg,SQLCODE);
			return -1007 ;
		}
		if(strcmp(vOpType,"0") == 0)/*������ɾ��ǩԼ��Ϣ*/
		{
			if(vCount2>0 || vCount3>0)
			{
				return 0;
			}
		}
		else if (strcmp(vOpType,"1") == 0)/*��ͥV��ɾ��ǩԼ��Ϣ*/
		{
			if(vCount1>0 || vCount3>0)
			{
				return 0;
			}
		}
		else if(strcmp(vOpType,"2") == 0)/*����˫V��ɾ��ǩԼ��Ϣ*/
		{
			if(vCount1>0 || vCount2>0)
			{
				return 0;
			}
		}
	}
	return sendCmd(InCmdCode,type,InPhoneNo,return_msg,InSysDate,pWorkNo,pOpCode,pOrgCode,InLoginAccept,InTotalDate);
}
