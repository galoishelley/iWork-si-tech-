/*******�������֯�ṹ���ڸ��� edit by  zhangzhuo 26/05/2009  ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"

#define _DEBUG_


EXEC SQL INCLUDE SQLCA;


/**
* ���ŷ��ͺ���
*
* @author	lab
*
* @version   %I%, %G%
*
* @since	 1.00
*
* @inparam   ���Žṹ�� tShortMsg *msg
*
* @inparam	 ������ˮ	 vmsg->vLogin_Accept		 long
* @inparam	 ��Ϣ�ȼ�	 vmsg->vMsg_Level			long
* @inparam	 �ƶ�����	 vmsg->vPhone_No			 char(15)
* @inparam	 ������Ϣ	 vmsg->vSend_Msg			 char(255)
* @inparam	 ����˳��	 vmsg->vOrder_Code		   int
* @inparam	 ���ش���	 vmsg->vBack_Code			int
* @inparam	 ��������	 vmsg->vOp_Code			  char(4)
* @inparam	 ��������	 vmsg->vLogin_No			 char(6)
*
* @outparam  ���ش�����Ϣ   retMsg   char(60)
*
* @return	0:  �ɹ�
*			1:  ����wCommonShortMsg���ŷ�����־��ʧ��;
*/
int sendShortMsg(tShortMsg *vmsg,char *retMsg)
{
	char   vSQL_Text[4096+1];		 /* SQL���	   */
	char   vTmp_String[4096+1];	   /* ��ʱ����	  */
	char   vReturn_Msg[255+1];		/* ������Ϣ	  */

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendShortMsg","", "sendShortMsg: Starting Execute");
#endif

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:������ˮ	  =	", "[%ld]", vmsg->vLogin_Accept);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:��Ϣ�ȼ�	  =	 ", "[%d]", vmsg->vMsg_Level);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:�ƶ�����	  =	 ", "[%s]", vmsg->vPhone_No);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:������Ϣ	  =	 ", "[%s]", vmsg->vSend_Msg);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:����˳��	  =	 ", "[%d]", vmsg->vOrder_Code);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:���ش���	  =	 ", "[%d]", vmsg->vBack_Code);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:��������	  =	 ", "[%s]", vmsg->vOp_Code);
	pubLog_Debug(_FFL_, "sendShortMsg", "debug msg:��������	  =	 ", "[%s]", vmsg->vLogin_No);
#endif

	memset(vSQL_Text,   0,   sizeof(vSQL_Text));
	memset(vTmp_String, 0,   sizeof(vTmp_String));
	memset(vReturn_Msg, 0,   sizeof(vReturn_Msg));

	/*
	* ������ŷ��ͱ�
	*
	* commented by lab 2005.1.31
	*/

	memset(vSQL_Text,   0,  sizeof(vSQL_Text));
	strcpy(vSQL_Text,"INSERT INTO wCommonShortMsg");
	strcat(vSQL_Text,"(Command_Id,   Login_Accept,	Msg_Level,");
	strcat(vSQL_Text,"Phone_No,	  Msg,			 Order_Code,");
	strcat(vSQL_Text,"Back_Code,	 Dx_Op_Code,	  Login_No,");
	strcat(vSQL_Text,"Op_Time,	   Sent_Time)");
	strcat(vSQL_Text," VALUES( ");

	memset(vTmp_String, 0,  sizeof(vTmp_String));	strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s sOffon.nextval,  %ld, %ld,",vTmp_String, vmsg->vLogin_Accept,vmsg->vMsg_Level);

	memset(vTmp_String, 0,  sizeof(vTmp_String));	strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', '%s',%d,",vTmp_String,vmsg->vPhone_No,vmsg->vSend_Msg,vmsg->vOrder_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));	strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %d, '%s','%s',",vTmp_String,vmsg->vBack_Code,vmsg->vOp_Code,vmsg->vLogin_No);

	memset(vTmp_String, 0,  sizeof(vTmp_String));	strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s SysDate,SysDate)",vTmp_String);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendShortMsg", "", "sendShortMsg: vSQL_Text=[%s]",vSQL_Text);
#endif

	memset(vSQL_Text,   0,  sizeof(vSQL_Text));
	strcpy(vSQL_Text,"INSERT INTO wCommonShortMsg");
	strcat(vSQL_Text,"(Command_Id,   Login_Accept,	Msg_Level,");
	strcat(vSQL_Text,"Phone_No,	  Msg,			 Order_Code,");
	strcat(vSQL_Text,"Back_Code,	 Dx_Op_Code,	  Login_No,");
	strcat(vSQL_Text,"Op_Time,	   Sent_Time)");
	strcat(vSQL_Text," VALUES( ");
	strcat(vSQL_Text," sOffon.nextval, :v1, :v2,");
	strcat(vSQL_Text," :v3, :v4, :v5,");
	strcat(vSQL_Text," :v6, :v7, :v8,");
	strcat(vSQL_Text," SysDate,SysDate)");

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "sendShortMsg", "sendShortMsg: vSQL_Text=", "[%s]",vSQL_Text);
#endif

	EXEC SQL PREPARE Exec_wCommonShortMsg FROM :vSQL_Text;
	EXEC SQL EXECUTE Exec_wCommonShortMsg using
		:vmsg->vLogin_Accept,	:vmsg->vMsg_Level,
		:vmsg->vPhone_No,		:vmsg->vSend_Msg,
		:vmsg->vOrder_Code,	  :vmsg->vBack_Code,
		:vmsg->vOp_Code,		 :vmsg->vLogin_No;

	if (SQLCODE != OK)
	{
		sprintf(vReturn_Msg,"������ŷ�����־��ʧ��[wCommonShortMsg],SQLCODE=[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "sendShortMsg", "error msg:������ŷ�����־��ʧ��[wCommonShortMsg],SQLCODE=","[%d][%s]",SQLCODE,SQLERRMSG);

		if (retMsg != NULL)
		{
			strncpy(retMsg,  vReturn_Msg, strlen(rtrim(vReturn_Msg)));
			retMsg[strlen(rtrim(vReturn_Msg))] = '\0';
		}
		return 1;
	}

	return 0;
}

/**
* ��ȡ��������ˮ
*
* @author	lab
*
* @version   %I%, %G%
*
* @since	 1.00
*
* @outparam  ������ˮ CHAR(15)
*
* @return	0:�ɹ���1:ORACLE����2:����������Ч
*/
int getMaxSysAccept(char *accept)
{
	char vAccept[14+1];			   /* ϵͳʱ��		 */
	char vSQL_Text[1024+1];		   /* SQL���		  */

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "getMaxSysAccept", "", "getMaxSysAccept: Starting Execute");
#endif

	memset(vAccept,		 0, sizeof(vAccept));
	memset(vSQL_Text,	   0, sizeof(vSQL_Text));

	strcpy(vSQL_Text,"SELECT to_char(ltrim(rtrim(sMaxSysAccept.nextval))) FROM Dual");

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "getMaxSysAccept", "GetFormatDate: vSQL_Text=", "[%s]",vSQL_Text);
#endif

	EXEC SQL PREPARE Prepare_sMaxSysAccept FROM :vSQL_Text;
	EXEC SQL DECLARE Cursor_sMaxSysAccept CURSOR FOR Prepare_sMaxSysAccept;
	EXEC SQL OPEN Cursor_sMaxSysAccept;
	for(;;)
	{
		EXEC SQL FETCH Cursor_sMaxSysAccept INTO :vAccept;
		Trim(vAccept);
		if ((SQLCODE != OK) && (SQLCODE != NOTFOUND))
		{
			EXEC SQL CLOSE Cursor_sMaxSysAccept;
			return 1;
		}

		if (SQLCODE == NOTFOUND) break;
	}
	EXEC SQL CLOSE Cursor_sMaxSysAccept;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "getMaxSysAccept", "", " SQLCODE=[%d],SQLERRMSG=[%s]",SQLCODE,SQLERRMSG);
#endif

	if (strlen(vAccept) <= 0) return 2;
	{

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "getMaxSysAccept", "vAccept=", "[%s]",vAccept);
#endif

		if (accept != NULL)
		{
			strncpy(accept,	vAccept,  strlen(vAccept));
			accept[strlen(vAccept)]   = '\0';
		}
		return 0;
	}
}



/**
* ��ȡ���ݿ�ϵͳʱ��
*
* @author	lab
*
* @version   %I%, %G%
*
* @since	 1.00
*
* @inparam   ʱ���ʽ CHAR(21)  ORACLEʱ���ʽ;
*
* @outparam  ϵͳʱ�� CHAR(21)
*
* @return	0:�ɹ���1:ORACLE����2:����������Ч
*/
int GetFormatDate(char *format,char *sysdate)
{
	char vFormat[21+1];			   /* ʱ���ʽ		 */
	char vSysDate[21+1];			  /* ϵͳʱ��		 */
	char vSQL_Text[1024+1];		   /* SQL���		  */

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "GetFormatDate", "GetFormatDate:", " Starting Execute");
#endif

	memset(vFormat,		 0, sizeof(vFormat));
	memset(vSysDate,		0, sizeof(vSysDate));
	memset(vSQL_Text,	   0, sizeof(vSQL_Text));

	if (format != NULL)
	{
		strncpy(vFormat,	format,   strlen(rtrim(format)));
		vFormat[21]	   = '\0';
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "GetFormatDate", "input parameter format value=", "[%s]",vFormat);
#endif

	memset(vSQL_Text,	   0, sizeof(vSQL_Text));
	sprintf(vSQL_Text,"SELECT To_Char(SysDate,'%s') FROM Dual",vFormat);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "GetFormatDate", "vSQL_Text=", "[%s]",vSQL_Text);
#endif

	memset(vSQL_Text,	   0, sizeof(vSQL_Text));
	strcpy(vSQL_Text,"SELECT To_Char(SysDate,:v1) FROM Dual");

	EXEC SQL PREPARE Prepare_SysDate FROM :vSQL_Text;
	EXEC SQL DECLARE Cursor_SysDate CURSOR FOR Prepare_SysDate;
	EXEC SQL OPEN Cursor_SysDate using :vFormat;
	for(;;)
	{
		EXEC SQL FETCH Cursor_SysDate INTO :vSysDate;

		if ((SQLCODE != OK) && (SQLCODE != NOTFOUND))
		{
			EXEC SQL CLOSE Cursor_SysDate;
			return 1;
		}

		if (SQLCODE == NOTFOUND) break;
	}
	EXEC SQL CLOSE Cursor_SysDate;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "GetFormatDate", "", "SQLCODE=[%d],SQLERRMSG=[%s]",SQLCODE,SQLERRMSG);
#endif

	if (strlen(vSysDate) <= 0) return 2;
	{

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "GetFormatDate", "vSysDate=", "[%s]",rtrim(vSysDate));
#endif

		if (sysdate != NULL)
		{
			strncpy(sysdate,	vSysDate,  strlen(rtrim(vSysDate)));
			sysdate[strlen(rtrim(vSysDate))]   = '\0';
		}
		return 0;
	}
}


/**
* ����֧Ʊʣ�����
*
* @author	peijh
*
* @version   %I%, %G%
*
* @since	 1.00
*
* @inparam ֧Ʊ����	  InCheck_No				 char(20)
* @inparam ֧Ʊ����	  InCheck_Pay				float
* @inparam ���д���	  InBank_Code				char(5)
* @inparam ������ע	  InOp_Note				  char(60)
* @inparam ��������	  InLogin_No				 char(6)
* @inparam ������ˮ	  InLogin_Accept			 char(23)
* @inparam ��������	  InOp_Code				  char(4)
* @inparam ��������	  InTotal_Date			   char(8)
* @inparam ����ʱ��	  InSysDate				  char(17)
* @inparam ϵͳ��ע	  InSystem_Note			  char(60)
* @inparam ��������	  InOrg_Code				 char(9)
* @inparam ����ʶ��	  InOrg_Id				   char(10)
*
* @return	0:  �ɹ�
*/
int pubOpenAcctCheck(char *InCheck_No,   float InCheck_Pay,  char *InBank_Code,
					 char *InOp_Note,	char  *InLogin_No,  char InLogin_Accept[23],
					 char *InOp_Code,	char  *InTotal_Date,char *InSysDate,
					 char *InSystem_Note,char  *InOrg_Code,  char *InOrg_Id)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char  pCheck_No[20+1];
	float pCheck_Pay=0.00;
	char  pBank_Code[5+1];
	char  pOp_Note[60+1];
	char  pLogin_No[6+1];
	char  pLogin_Accept[23];
	char  pOp_Code[4+1];
	char  pTotal_Date[8+1];
	char  pSysDate[20+1];
	char  pOrg_Code[9+1];
	char  pOrg_Id[10+1];
	float vCheck_Prepay=0;
	EXEC SQL END   DECLARE SECTION;

	init(pCheck_No);   init(pBank_Code); init(pOp_Note);
	init(pOp_Code); init(pLogin_No);  init(pTotal_Date);
	init(pSysDate); init(pOrg_Code);  init(pLogin_Accept);
	init(pOrg_Id);

	pCheck_Pay=InCheck_Pay;
	memcpy(pLogin_Accept,   InLogin_Accept,  22);
	memcpy(pCheck_No,	   InCheck_No,	  20);
	memcpy(pBank_Code,	  InBank_Code,	  5);
	memcpy(pOp_Note,		InOp_Note,	   60);
	memcpy(pLogin_No,	   InLogin_No,	   6);
	memcpy(pOp_Code,		InOp_Code,		4);
	memcpy(pTotal_Date,	 InTotal_Date,	 8);
	memcpy(pSysDate,		InSysDate,	   20);
	memcpy(pOrg_Code,	   InOrg_Code,	   9);
	memcpy(pOrg_Id,		 InOrg_Id,		10);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "", " Starting Execute");
#endif


#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "֧Ʊ����   = ", "[%s]",	 InCheck_No);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "֧Ʊ����   = ", "[%12.2f]", InCheck_Pay);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "���д���   = ", "[%s]",	 InBank_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "������ע   = ", "[%s]",	 InOp_Note);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "��������   = ", "[%s]",	 InLogin_No);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "������ˮ   = ", "[%s]",	 InLogin_Accept);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "��������   = ", "[%s]",	 InOp_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "��������   = ", "[%s]",	 InTotal_Date);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "ϵͳʱ��   = ", "[%s]",	 InSysDate);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "ϵͳ��ע   = ", "[%s]",	 InSystem_Note);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "��������   = ", "[%s]",	 InOrg_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctCheck", "����ʶ��   = ", "[%s]",	 InOrg_Id);
#endif


#if PROVINCE_RUN != PROVINCE_JILIN
	EXEC SQL INSERT INTO wCheckOpr
		(Total_Date,			 Login_Accept,			   Org_Code,
		Login_No,			   Op_Code,					Bank_Code,
		Check_No,			   Check_Pay,				  Op_Time,
		Op_Note,				Org_Id)
		VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),  :pOrg_Code,
		:pLogin_No,			 :pOp_Code,				  :pBank_Code,
		:pCheck_No,			 :pCheck_Pay,				TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
		:pOp_Note,			  :pOrg_Id);
#else
    /*��֯������������org_id�ֶ� edit by zhangzhuo 26/05/2009 begin*/
	EXEC SQL INSERT INTO wCheckOpr
		(Total_Date,			 Login_Accept,			   Org_Code,
		Login_No,			   Op_Code,					Bank_Code,
		Check_No,			   Check_Pay,				  Op_Time,
		Op_Note,                 Org_Id)
		VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),  :pOrg_Code,
		:pLogin_No,			 :pOp_Code,				  :pBank_Code,
		:pCheck_No,			 :pCheck_Pay,				TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
		:pOp_Note,              :pOrg_Id);
	/*��֯������������org_id�ֶ� edit by zhangzhuo 26/05/2009 end*/
#endif
	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "pubOpenAcctCheck", "����֧Ʊ������ˮ��(wCheckOpr),SQLCODE=", "[%d][%s]",SQLCODE,SQLERRMSG);
		return 1;
	}


	EXEC SQL SELECT Nvl(Check_Prepay,0.00) INTO :vCheck_Prepay
		FROM dCheckPrepay
		WHERE Bank_Code  = :pBank_Code
		AND Check_No   = :pCheck_No;
	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "pubOpenAcctCheck", "��ѯ֧Ʊ����(dCheckPrepay),SQLCODE=", "[%d][%s]",SQLCODE,SQLERRMSG);
		return 2;
	}


	if(vCheck_Prepay >= InCheck_Pay)
	{
		/* ng ���� ҵ�񹤵����� by liuzhou 20091001*/
		/*
		EXEC SQL UPDATE dCheckPrepay
			SET Check_Prepay = Check_Prepay - :pCheck_Pay,
			Op_Time	  = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS')
			WHERE Bank_Code  = :pBank_Code
			AND Check_No   = :pCheck_No;

		if (SQLCODE != 0 || SQLROWS != 1)
		{
			pubLog_Debug(_FFL_, "pubOpenAcctCheck", "����֧Ʊ����(dCheckPrepay) �����֧Ʊ������,SQLCODE=", "[%d][%s]",SQLCODE,SQLERRMSG);
			return 3;
		}
		*/
		/* ng ���� ҵ�񹤵����� by liuzhou 20091001*/		
	}
	else
	{
		pubLog_Debug(_FFL_, "pubOpenAcctCheck", "", "֧Ʊ����");
		return 4;
	}

	return 0;
}

/**
* �����û���Ϣ����
*
* @author        zhangzhe@20090220
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   �ͻ������ṹ��   tUserDoc *doc
*
* @inparam   �ͻ�Id                   doc->vCust_Id                   	long
* @inparam   �û�ID                   doc->vId_No                       long
* @inparam   Ĭ���ʻ�                 doc->vContract_No                 long
* @inparam   �ʻ�����                 doc->vIds                         long
* @inparam   �������                 doc->vPhone_No                  	char(15)
* @inparam   ҵ�����                 doc->vSm_Code                   	char(2)
* @inparam   ��������                 doc->vService_Type          		char(2)
* @inparam   �ͻ����Դ���             doc->vAttr_Code                 	char(8)
* @inparam   �û�����                 doc->vUser_Passwd           		char(8)
* @inparam   ����ʱ��                 doc->vOpen_Time                 	char(17)
* @inparam   ��������                 doc->vBelong_Code           		char(7)
* @inparam   ������                   doc->vLimit_Owe                 	float
* @inparam   �����ȼ�                 doc->vCredit_Code           		char(1)
* @inparam   �����ȼ���ֵ             doc->vCredit_Value          		int
* @inparam   ����״̬                 doc->vRun_Code                  	char(2)
* @inparam   ״̬ʱ��                 doc->vRun_Time                  	char(17)
* @inparam   �Ʒ�����                 doc->vBill_Date                 	char(17)
* @inparam   ��������                 doc->vBill_Type                 	int
* @inparam   �����ֶ�                 doc->vEncrypt_Prepay        		char(16)
* @inparam   ���ػ�Ȩ��ֵ             doc->vCmd_Right                 	int
* @inparam   �ϴγ�������             doc->vLast_Bill_Type        		char(1)
* @inparam   �����ֶ�                 doc->vBak_Field                 	char(12)
* @inparam   ������ˮ                 doc->vLogin_Accept          		long
* @inparam   ����ʱ��                 doc->vOp_Time                   	char(17)
* @inparam   ��������                 doc->vTotal_Date                	int
* @inparam   ��������                 doc->vLogin_No                  	char(6)
* @inparam   ��������                 doc->vOp_Code                   	char(4)
* @inparam   �����ڵ�                 doc->vGroup_Id                  	char(10)
* @inparam   �������                 doc->varea_code                 	char(10)
*
* @outparam  ���ش�����Ϣ   retMsg   char(60)
*
* @return        0:  �ɹ�
* @return        1:  �����û�������Ϣ��ʷ��[dCustMsgHis]ʧ��
* @return        2:  �����û�������Ϣ��[dCustMsg]ʧ��
*
*/
int createCustMsg(tUserDoc *doc,char *retMsg)
{
	char vSQL_Text[4096+1];                 /* SQL���              */
	char vTmp_String[4096+1];               /* ��ʱ����             */
	char vOp_Type[1+1];                             /* ��������             */
	char vOp_No[14+1];                      /* ����ӿ���ˮ                 */
	int  ret_code=0;                        /* ���ش���             */
	char vReturn_Msg[255+1];                /* ������Ϣ             */
	long pcust_id=0;
	int iCount = 0;

	/*ng ����*/
	int ret = 0;
	TdCustMsg vTdCustMsg;
	char vAreaStr[10+1];
	char vIdNoStr[14+1];
	char vCustIdStr[14+1];
	char vContractNoStr[14+1];
	char vIdsStr[10+1];
	char vLimitOweStr[14+1];
	char vCreditValueStr[8+1];
	char vBillTypeStr[4+1];
	char vCmdRightStr[4+1];
	
	memset(vSQL_Text,   0,   sizeof(vSQL_Text));
   	memset(vTmp_String, 0,   sizeof(vTmp_String));
   	memset(vOp_Type,    0,   sizeof(vOp_Type));
   	memset(vOp_No,      0,   sizeof(vOp_No));
   	memset(vReturn_Msg, 0,   sizeof(vReturn_Msg));
   	memset(vAreaStr, 0,   sizeof(vAreaStr));
   	
   	
   	memset(&vTdCustMsg, 0, sizeof(vTdCustMsg));
   	memset(vIdNoStr, 0,   sizeof(vIdNoStr));
   	memset(vCustIdStr, 0,   sizeof(vCustIdStr));
   	memset(vContractNoStr, 0,   sizeof(vContractNoStr));
   	memset(vIdsStr, 0,   sizeof(vIdsStr));
	memset(vLimitOweStr, 0,   sizeof(vLimitOweStr));
	memset(vCreditValueStr, 0,   sizeof(vCreditValueStr));
	memset(vBillTypeStr, 0,   sizeof(vBillTypeStr));
	memset(vCmdRightStr, 0,   sizeof(vCmdRightStr));

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createCustMsg", "", "createCustMsg: Starting Execute");
#endif

   	Trim(doc->vGroup_Id);
   	Trim(doc->varea_code);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �ͻ�Id                 = [%ld]",             doc->vCust_Id        );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �û�ID                 = [%ld]",             doc->vId_No           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: Ĭ���ʻ�               = [%ld]",             doc->vContract_No       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �ʻ�����               = [%ld]",             doc->vIds                );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �������               = [%s]",              doc->vPhone_No          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ҵ�����               = [%s]",              doc->vSm_Code           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%s]",              doc->vService_Type      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �ͻ����Դ���           = [%s]",              doc->vAttr_Code         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �û�����               = [%s]",              doc->vUser_Passwd       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ����ʱ��               = [%s]",              doc->vOpen_Time         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%s]",              doc->vBelong_Code       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ������                 = [%12.2f]",          doc->vLimit_Owe         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����ȼ�               = [%s]",              doc->vCredit_Code       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����ȼ���ֵ           = [%d]",              doc->vCredit_Value      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ����״̬               = [%s]",              doc->vRun_Code          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ״̬ʱ��               = [%s]",              doc->vRun_Time          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �Ʒ�����               = [%s]",              doc->vBill_Date         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%d]",              doc->vBill_Type         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����ֶ�               = [%s]",              doc->vEncrypt_Prepay    );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ���ػ�Ȩ��ֵ           = [%d]",              doc->vCmd_Right         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �ϴγ�������           = [%s]",              doc->vLast_Bill_Type    );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����ֶ�               = [%s]",              doc->vBak_Field         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ������ˮ               = [%ld]",             doc->vLogin_Accept      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ����ʱ��               = [%s]",              doc->vOp_Time           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%d]",              doc->vTotal_Date        );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%s]",              doc->vLogin_No          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: ��������               = [%s]",              doc->vOp_Code           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����ڵ�               = [%s]",              doc->vGroup_Id          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �������               = [%s]",              doc->varea_code          );
#endif
	
	/* �������ػ��汾��dcustmsg��groupmsg�ֶΣ���doc->varea_code��group_id���ͽڵ㣬���Զ���ֵ*/
	EXEC SQL SELECT COUNT(*)
			 INTO :iCount
			 FROM dChnGroupMsg
			 WHERE group_id = :doc->varea_code
			 AND class_code in('13','1159','14');
	if (SQLCODE != 0)
	{
		sprintf(vReturn_Msg,"ȡgroup_idʧ��,SQLCODE=[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "createCustMsg", "ȡ���ػ���Ϣʧ��,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return 2;
	}
	if(iCount == 0)
	{
		EXEC SQL SELECT PARENT_GROUP_ID
		INTO :vAreaStr
		FROM dChnGroupInfo
		WHERE group_id =:doc->vGroup_Id
		AND denorm_level = 1;
		if (SQLCODE != 0)
		{
			sprintf(vReturn_Msg,"ȡgroup_idʧ��,SQLCODE=[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "createCustMsg", "ȡ���ػ���Ϣʧ��,", "SQLCODE=[%d]",SQLCODE);
			return 3;
		}
	}
	else
	{
		strcpy(vAreaStr,doc->varea_code);
	}
	Trim(vAreaStr);
	
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: �����������Ϊ       = [%s]",             vAreaStr        );
   
   	strcpy(vOp_Type,        "a");


	/* �����û�������Ϣ��ʷ��dCustMsgHis*/

	strcpy(vSQL_Text,"INSERT INTO dCustMsgHis ");
	strcat(vSQL_Text,"(Cust_Id,           Id_No,                  Contract_No,");
	strcat(vSQL_Text,"Ids,                        Phone_No,           Sm_Code,");
	strcat(vSQL_Text,"Service_Type,   Attr_Code,          User_Passwd,");
	strcat(vSQL_Text,"Belong_Code,        Limit_Owe,          Credit_Code,");
	strcat(vSQL_Text,"Credit_Value,   Run_Code,           ");
	strcat(vSQL_Text,"Open_Time,          ");
	strcat(vSQL_Text,"Run_Time,");
	strcat(vSQL_Text,"Bill_Date,");
	strcat(vSQL_Text,"Bill_Type,          Encrypt_Prepay, Cmd_Right,");
	strcat(vSQL_Text,"Last_Bill_Type, Bak_Field,      ");
	strcat(vSQL_Text,"group_id,	group_no,");
	strcat(vSQL_Text,"Update_Accept,  Update_Time,        Update_Date,");
	strcat(vSQL_Text,"Update_Login,   Update_Type,        Update_Code)");
	strcat(vSQL_Text," VALUES( ");

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %ld,  %ld, %ld,",vTmp_String,doc->vCust_Id,doc->vId_No,doc->vContract_No);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %ld, '%s', '%s',",vTmp_String,doc->vIds,doc->vPhone_No,doc->vSm_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vService_Type,doc->vAttr_Code,doc->vUser_Passwd);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', %12.2f, '%s',",vTmp_String,doc->vBelong_Code,doc->vLimit_Owe,doc->vCredit_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %d, '%s',",vTmp_String,doc->vCredit_Value,doc->vRun_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vOpen_Time);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vRun_Time);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vBill_Date);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %d, '%s', %d,",vTmp_String,doc->vBill_Type,doc->vEncrypt_Prepay,doc->vCmd_Right);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', '%s', ",vTmp_String,doc->vLast_Bill_Type,doc->vBak_Field);
	
	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s','%s'",vTmp_String,vAreaStr,doc->vGroup_Id);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %ld, To_Date('%s','YYYYMMDD HH24:MI:SS'), %d,",vTmp_String,doc->vLogin_Accept,doc->vOp_Time,doc->vTotal_Date);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s','%s', '%s')",vTmp_String,doc->vLogin_No,vOp_Type,doc->vOp_Code);


#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createCustMsg", "createCustMsg: vSQL_Text=", "[%s]",vSQL_Text);
#endif

	strcpy(vSQL_Text,"INSERT INTO dCustMsgHis ");
	strcat(vSQL_Text,"(Cust_Id,           Id_No,                  Contract_No,");
	strcat(vSQL_Text,"Ids,                        Phone_No,           Sm_Code,");
	strcat(vSQL_Text,"Service_Type,   Attr_Code,          User_Passwd,");
	strcat(vSQL_Text,"Belong_Code,        Limit_Owe,          Credit_Code,");
	strcat(vSQL_Text,"Credit_Value,   Run_Code,           ");
	strcat(vSQL_Text,"Open_Time,          ");
	strcat(vSQL_Text,"Run_Time,");
	strcat(vSQL_Text,"Bill_Date,");
	strcat(vSQL_Text,"Bill_Type,          Encrypt_Prepay, Cmd_Right,");
	strcat(vSQL_Text,"Last_Bill_Type, Bak_Field,          ");
	strcat(vSQL_Text,"group_id,		group_no,");
	strcat(vSQL_Text,"Update_Accept,  Update_Time,        Update_Date,");
	strcat(vSQL_Text,"Update_Login,   Update_Type,        Update_Code)");
	strcat(vSQL_Text," VALUES( ");
	strcat(vSQL_Text,":v1,  :v2, :v3,");
	strcat(vSQL_Text,":v4,  :v5, :v6,");
	strcat(vSQL_Text,":v7,  :v8, :v9,");
	strcat(vSQL_Text,":v10, :v11,:v12,");
	strcat(vSQL_Text,":v13, :v14,");
	strcat(vSQL_Text,"To_Date(:v15,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,"To_Date(:v16,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,"To_Date(:v17,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,":v18, :v19, :v20,");
	strcat(vSQL_Text,":v21, :v22, ");
	strcat(vSQL_Text,":v34,:v35,");
	strcat(vSQL_Text,":v25, To_Date(:v26,'YYYYMMDD HH24:MI:SS'), :v27,");
	strcat(vSQL_Text,":v28, :v29, :v30)");

	EXEC SQL PREPARE Exec_dCustMsgHis FROM :vSQL_Text;
	EXEC SQL EXECUTE Exec_dCustMsgHis using
	        :doc->vCust_Id,           :doc->vId_No,                  :doc->vContract_No,
	        :doc->vIds,                   :doc->vPhone_No,           :doc->vSm_Code,
	        :doc->vService_Type,  :doc->vAttr_Code,          :doc->vUser_Passwd,
	        :doc->vBelong_Code,   :doc->vLimit_Owe,          :doc->vCredit_Code,
	        :doc->vCredit_Value,  :doc->vRun_Code,
	        :doc->vOpen_Time,
	        :doc->vRun_Time,
	        :doc->vBill_Date,
	        :doc->vBill_Type,         :doc->vEncrypt_Prepay, :doc->vCmd_Right,
	        :doc->vLast_Bill_Type,:doc->vBak_Field,          
	        :vAreaStr,:doc->vGroup_Id,
	        :doc->vLogin_Accept,  :doc->vOp_Time,                :doc->vTotal_Date,
	        :doc->vLogin_No,          :vOp_Type,                         :doc->vOp_Code;

	if (SQLCODE != OK)
	{
		sprintf(vReturn_Msg,"�����û�������Ϣ��ʷ��[dCustMsgHis]��ʧ��,SQLCODE=[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "createCustMsg", "�����û�������Ϣ��ʷ��[dCustMsgHis]��ʧ��,", " SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		if (retMsg != NULL)
		{
			strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
			printf("^^^^^^^^^^^^^^^^^^^^\n");
			retMsg[strlen(vReturn_Msg)] = '\0';
		}
		return 1;
	}

	/*�����û�������Ϣ��dCustMsg*/

	strcpy(vSQL_Text,"INSERT INTO dCustMsg ");
	strcat(vSQL_Text,"(Cust_Id,           Id_No,                  Contract_No,");
	strcat(vSQL_Text,"Ids,                        Phone_No,           Sm_Code,");
	strcat(vSQL_Text,"Service_Type,   Attr_Code,          User_Passwd,");
	strcat(vSQL_Text,"Belong_Code,        Limit_Owe,          Credit_Code,");
	strcat(vSQL_Text,"Credit_Value,   Run_Code,           ");
	strcat(vSQL_Text,"Open_Time,          ");
	strcat(vSQL_Text,"Run_Time,");
	strcat(vSQL_Text,"Bill_Date,");
	strcat(vSQL_Text,"Bill_Type,          Encrypt_Prepay, Cmd_Right,");
	strcat(vSQL_Text,"Last_Bill_Type, Bak_Field,         ");
	strcat(vSQL_Text,"group_id,		group_no,group_msg)");
	strcat(vSQL_Text," VALUES( ");

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %ld,  %ld, %ld,",vTmp_String,doc->vCust_Id,doc->vId_No,doc->vContract_No);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %ld, '%s', '%s',",vTmp_String,doc->vIds,doc->vPhone_No,doc->vSm_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vService_Type,doc->vAttr_Code,doc->vUser_Passwd);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', %12.2f, '%s',",vTmp_String,doc->vBelong_Code,doc->vLimit_Owe,doc->vCredit_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %d, '%s',",vTmp_String,doc->vCredit_Value,doc->vRun_Code);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vOpen_Time);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vRun_Time);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'),",vTmp_String,doc->vBill_Date);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s %d, '%s', %d,",vTmp_String,doc->vBill_Type,doc->vEncrypt_Prepay,doc->vCmd_Right);

	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s', '%s',",vTmp_String,doc->vLast_Bill_Type,doc->vBak_Field);
	
	memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
	sprintf(vSQL_Text,"%s '%s','%s','%s')",vTmp_String,vAreaStr,doc->vGroup_Id,vAreaStr);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createCustMsg", "vSQL_Text=", "[%s]",vSQL_Text);
#endif
	/*ng���� by wxcrm at 20090812 begin
	strcpy(vSQL_Text,"INSERT INTO dCustMsg ");
	strcat(vSQL_Text,"(Cust_Id,           Id_No,                  Contract_No,");
	strcat(vSQL_Text,"Ids,                        Phone_No,           Sm_Code,");
	strcat(vSQL_Text,"Service_Type,   Attr_Code,          User_Passwd,");
	strcat(vSQL_Text,"Belong_Code,        Limit_Owe,          Credit_Code,");
	strcat(vSQL_Text,"Credit_Value,   Run_Code,           ");
	strcat(vSQL_Text,"Open_Time,          ");
	strcat(vSQL_Text,"Run_Time,");
	strcat(vSQL_Text,"Bill_Date,");
	strcat(vSQL_Text,"Bill_Type,          Encrypt_Prepay, Cmd_Right,");
	strcat(vSQL_Text,"Last_Bill_Type, Bak_Field,");
	strcat(vSQL_Text,"group_id,		group_no,group_msg)");
	strcat(vSQL_Text," VALUES( ");
	strcat(vSQL_Text,":v1,  :v2, :v3,");
	strcat(vSQL_Text,":v4,  :v5, :v6,");
	strcat(vSQL_Text,":v7,  :v8, :v9,");
	strcat(vSQL_Text,":v10, :v11,:v12,");
	strcat(vSQL_Text,":v13, :v14,");
	strcat(vSQL_Text,"To_Date(:v15,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,"To_Date(:v16,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,"To_Date(:v17,'YYYYMMDD HH24:MI:SS'),");
	strcat(vSQL_Text,":v18, :v19, :v20,");
	strcat(vSQL_Text,":v21, :v22,");
	strcat(vSQL_Text,"nvl(:v25,'00999'),:v26,nvl(:v25,'00999'))");

	EXEC SQL PREPARE Exec_dCustMsg FROM :vSQL_Text;
	EXEC SQL EXECUTE Exec_dCustMsg using
                        :doc->vCust_Id,         :doc->vId_No,                   :doc->vContract_No,
                        :doc->vIds,             :doc->vPhone_No,                :doc->vSm_Code,
                        :doc->vService_Type,    :doc->vAttr_Code,               :doc->vUser_Passwd,
                        :doc->vBelong_Code,     :doc->vLimit_Owe,               :doc->vCredit_Code,
                        :doc->vCredit_Value,    :doc->vRun_Code,
                        :doc->vOpen_Time,
                        :doc->vRun_Time,
                        :doc->vBill_Date,
                        :doc->vBill_Type,       :doc->vEncrypt_Prepay,  :doc->vCmd_Right,
                        :doc->vLast_Bill_Type,  :doc->vBak_Field,       
                        :doc->varea_code,:doc->vGroup_Id,:doc->varea_code;
	if (SQLCODE != OK)
	{
	    sprintf(vReturn_Msg,"�����û�������Ϣ��[dCustMsg]��ʧ��,SQLCODE=[%d]",SQLCODE);
	    pubLog_Debug(_FFL_, "createCustMsg", "�����û�������Ϣ��[dCustMsg]��ʧ��,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
	    if (retMsg != NULL)
	    {
	        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
	        retMsg[strlen(vReturn_Msg)] = '\0';
	    }
			return 2;
	}
	ng���� by wxcrm at 20090812 end*/
	

	sprintf(vIdNoStr,"%ld",doc->vId_No);
	sprintf(vCustIdStr,"%ld",doc->vCust_Id);
	sprintf(vContractNoStr,"%ld",doc->vContract_No);
	sprintf(vIdsStr,"%ld",doc->vIds);
	sprintf(vLimitOweStr,"%12.2f",doc->vLimit_Owe);
	sprintf(vCreditValueStr,"%d",doc->vCredit_Value);
	sprintf(vBillTypeStr,"%d",doc->vBill_Type);
	sprintf(vCmdRightStr,"%d",doc->vCmd_Right);
	
		
	Trim(vIdNoStr);
	Trim(vCustIdStr);
	Trim(vContractNoStr);
	Trim(vIdsStr);
	Trim(vLimitOweStr);
	Trim(vCreditValueStr);
	Trim(vBillTypeStr);
	Trim(vCmdRightStr);

	strcpy(vTdCustMsg.sIdNo				,	vIdNoStr	);
	strcpy(vTdCustMsg.sCustId			,	vCustIdStr	);
	strcpy(vTdCustMsg.sContractNo		,	vContractNoStr	);
	strcpy(vTdCustMsg.sIds				,	vIdsStr	);
	strcpy(vTdCustMsg.sPhoneNo			,	doc->vPhone_No	);
	strcpy(vTdCustMsg.sSmCode			,	doc->vSm_Code	);
	strcpy(vTdCustMsg.sServiceType		,	doc->vService_Type	);
	strcpy(vTdCustMsg.sAttrCode			,	doc->vAttr_Code	);
	strcpy(vTdCustMsg.sUserPasswd		,	doc->vUser_Passwd	);
	strcpy(vTdCustMsg.sOpenTime			,	doc->vOpen_Time	);
	strcpy(vTdCustMsg.sBelongCode		,	doc->vBelong_Code	);
	strcpy(vTdCustMsg.sLimitOwe			,	vLimitOweStr	);
	strcpy(vTdCustMsg.sCreditCode		,	doc->vCredit_Code	);
	strcpy(vTdCustMsg.sCreditValue		,	vCreditValueStr	);
	strcpy(vTdCustMsg.sRunCode			,	doc->vRun_Code	);
	strcpy(vTdCustMsg.sRunTime			,	doc->vRun_Time	);
	strcpy(vTdCustMsg.sBillDate			,	doc->vBill_Date	);
	strcpy(vTdCustMsg.sBillType			,	vBillTypeStr	);
	strcpy(vTdCustMsg.sEncryptPrepay	,	doc->vEncrypt_Prepay	);
	strcpy(vTdCustMsg.sCmdRight			,	vCmdRightStr	);
	strcpy(vTdCustMsg.sLastBillType		,	doc->vLast_Bill_Type	);
	strcpy(vTdCustMsg.sBakField			,	doc->vBak_Field	);
	strcpy(vTdCustMsg.sGroupId			,	vAreaStr	);
	strcpy(vTdCustMsg.sGroupNo			,	doc->vGroup_Id	);
	strcpy(vTdCustMsg.sgroupmsg			,	vAreaStr	);

	ret = OrderInsertCustMsg("2",vIdNoStr,100,doc->vOp_Code,doc->vLogin_Accept,doc->vLogin_No,"createCustMsg",vTdCustMsg);
	if (ret != 0)
	{
		sprintf(vReturn_Msg,"�����û�������Ϣ��[dCustMsg]��ʧ��,SQLCODE=[%d]",SQLCODE);
	    pubLog_Debug(_FFL_, "createCustMsg", "�����û�������Ϣ��[dCustMsg]��ʧ��,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
	    if (retMsg != NULL)
	    {
	        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
	        retMsg[strlen(vReturn_Msg)] = '\0';
	    }
		return 2;
	}
	return 0;
}

/**
* �����ʻ�������Ϣ����
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   �ʻ������ṹ��   tAccDoc *doc
*
* @inparam   ������ˮ         doc->vLogin_Accept             char(14)
* @inparam   �ʻ�ID           doc->vContract_No              char(14)
* @inparam   �ͻ�ID           doc->vCon_Cust_Id              char(14)
* @inparam   �ʻ�����         doc->vContract_Passwd          char(8)
* @inparam   �ʻ�����         doc->vBank_Cust                char(60)
* @inparam   �ʻ���ͷ         doc->vOddment;                 float
* @inparam   ��������         doc->vBelong_Code              char(7)
* @inparam   Ԥ����           doc->vPrepay_Fee;              float               
* @inparam   �ʻ�������       doc->vAccount_Limit            char(1)
* @inparam   �ʻ�״̬         doc->vStatus                   char(1)
* @inparam   �ʼı�־         doc->vPost_Flag                char(1)
* @inparam   Ѻ��             doc->vDeposit;                 float
* @inparam   �ʻ�Ƿ��         doc->vOwe_Fee;                 float
* @inparam   �ʻ�����         doc->vAccount_Mark;            int
* @inparam   ���ʽ         doc->vPay_Code                 char(1)
* @inparam   ���д���         doc->vBank_Code                char(5)
* @inparam   �ַ����д���     doc->vPost_Bank_Code           char(5)
* @inparam   �ʺ�             doc->vAccount_No               char(30)
* @inparam   �ʻ�����         doc->vAccount_Type             char(1)
* @inparam   ��ʼ����         doc->vBegin_Time               char(17)
* @inparam   ��������         doc->vEnd_Time                 char(17)
* @inparam   ��������         doc->vOp_Code                  char(4)
* @inparam   ��������         doc->vLogin_No                 char(6)
* @inparam   ��������         doc->vOrg_Code                 char(9)
* @inparam   ������ע         doc->vOp_Note                  char(60)
* @inparam   ϵͳ��ע         doc->vSystem_Note              char(60)
* @inparam   IP_ADDR          doc->vIp_Addr                  char(15)
* @inparam   �ڵ��ʶ         doc->vGroup_Id                 char(10)
* @inparam   ��������         doc->vUpdate_Type              char(1)
*
* @outparam  ���ش�����Ϣ   retMsg   char(60)
*
* @return        0:  �ɹ�
* @return        3:  �����ʻ���Ϣ��ʷ��ʧ��[dConMsgHis];
* @return        4:  �����ʻ���Ϣ��ʧ��[dConMsg];
*/
int createConMsg(tAccDoc *Acc_Cust,char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sql_str[2048];                        /* �洢��̬SQL��䴮 */
	char totalDate[8+1];                   /* ��������                  */
	char tmp_string[17+1];                 /* �洢���ڸ�ʽ          */
	char yearMonth[6+1];
	int  ret=0;
	
	/*ng���� begin*/
	TdConMsg vTdConMsg;
	char vOddment[5+1];
	char vPrepay_Fee[17+1];
	char vDeposit[17+1];
	char vOwe_Fee[17+1];
	char vAccount_Mark[10+1];
	/*ng���� end*/
	EXEC SQL END DECLARE SECTION;

	memset(sql_str    , 0, sizeof(sql_str        ));
	memset(totalDate  , 0, sizeof(totalDate  ));
	memset(tmp_string , 0, sizeof(tmp_string ));
	memset(yearMonth  , 0, sizeof(yearMonth  ));
	
	memset(vOddment  , 0, sizeof(vOddment  ));
	memset(vPrepay_Fee  , 0, sizeof(vPrepay_Fee  ));
	memset(vDeposit  , 0, sizeof(vDeposit  ));
	memset(vOwe_Fee  , 0, sizeof(vOwe_Fee  ));
	memset(vAccount_Mark  , 0, sizeof(vAccount_Mark  ));
	memset(&vTdConMsg  , 0, sizeof(vTdConMsg  ));
	Trim(Acc_Cust->vGroup_Id);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg������ˮ         =[%s]",     Acc_Cust->vLogin_Accept);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�ID           =[%s]",     Acc_Cust->vContract_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ͻ�ID           =[%s]",     Acc_Cust->vCon_Cust_Id);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�����         =[%s]",     Acc_Cust->vContract_Passwd);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�����         =[%s]",     Acc_Cust->vBank_Cust);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ���ͷ         =[%12.2f]", Acc_Cust->vOddment);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vBelong_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msgԤ����           =[%12.2f]", Acc_Cust->vPrepay_Fee);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�������       =[%s]",     Acc_Cust->vAccount_Limit);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�״̬         =[%s]",     Acc_Cust->vStatus);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʼı�־         =[%s]",     Acc_Cust->vPost_Flag);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msgѺ��             =[%12.2f]", Acc_Cust->vDeposit);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�Ƿ��         =[%12.2f]", Acc_Cust->vOwe_Fee);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�����         =[%d]",     Acc_Cust->vAccount_Mark);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg���ʽ         =[%s]",     Acc_Cust->vPay_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg���д���         =[%s]",     Acc_Cust->vBank_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ַ����д���     =[%s]",     Acc_Cust->vPost_Bank_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʺ�             =[%s]",     Acc_Cust->vAccount_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ʻ�����         =[%s]",     Acc_Cust->vAccount_Type);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��ʼ����         =[%s]",     Acc_Cust->vBegin_Time);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vEnd_Time);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vOp_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vLogin_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vOrg_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg������ע         =[%s]",     Acc_Cust->vOp_Note);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msgϵͳ��ע         =[%s]",     Acc_Cust->vSystem_Note);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msgIP_ADDR          =[%s]",     Acc_Cust->vIp_Addr);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg�ڵ��ʶ         =[%s]",     Acc_Cust->vGroup_Id);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg��������         =[%s]",     Acc_Cust->vUpdate_Type);
#endif


strncpy(yearMonth,Acc_Cust->vBegin_Time,6);
strncpy(totalDate,Acc_Cust->vBegin_Time,8);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "", " createConMsg ����dConMsgHis��");
#endif
	Trim(yearMonth);
	Trim(totalDate);
	memset(sql_str,  0, sizeof(sql_str));

	sprintf(sql_str,"insert into dConMsgHis"
	        "("
	        " CONTRACT_NO,CON_CUST_ID,CONTRACT_PASSWD,BANK_CUST,ODDMENT,"
	        " BELONG_CODE,PREPAY_FEE,PREPAY_TIME,STATUS,STATUS_TIME,"
	        " POST_FLAG,DEPOSIT,MIN_YM,OWE_FEE,ACCOUNT_MARK,ACCOUNT_LIMIT,"
	        " PAY_CODE,BANK_CODE,POST_BANK_CODE,ACCOUNT_NO,ACCOUNT_TYPE,"
	        " UPDATE_CODE,UPDATE_DATE,UPDATE_TIME,UPDATE_LOGIN,"
	        " UPDATE_ACCEPT,UPDATE_TYPE,GROUP_ID"
	        ")"
	        " values(to_number('%s'),to_number('%s'),'%s','%s',%10.2f,'%s',"
	        " %10.2f,to_date('%s','YYYYMMDD HH24:MI:SS'),'%s',to_date('%s','YYYYMMDD HH24:MI:SS'),'%s',%10.2f,to_number('%s'),%10.2f,"
	        " %d,'%s','%s','%s','%s','%s','%s','%s',to_number('%s'),to_date('%s','YYYYMMDD HH24:MI:SS'),"
	        " '%s',to_number('%s'),'%s','%s'"
	        ")",
	        Acc_Cust->vContract_No,                 Acc_Cust->vCon_Cust_Id,
	        Acc_Cust->vContract_Passwd,         Acc_Cust->vBank_Cust,
	        Acc_Cust->vOddment,                         Acc_Cust->vBelong_Code,
	        Acc_Cust->vPrepay_Fee,Acc_Cust->vBegin_Time,
	        Acc_Cust->vStatus, Acc_Cust->vBegin_Time , Acc_Cust->vPost_Flag, Acc_Cust->vDeposit,
	        yearMonth,           Acc_Cust->vOwe_Fee,
	        Acc_Cust->vAccount_Mark,                Acc_Cust->vAccount_Limit,
	        Acc_Cust->vPay_Code,                        Acc_Cust->vBank_Code,
	        Acc_Cust->vPost_Bank_Code,          Acc_Cust->vAccount_No,
	        Acc_Cust->vAccount_Type,                Acc_Cust->vOp_Code,
	        totalDate,Acc_Cust->vBegin_Time,
	        Acc_Cust->vLogin_No,                        Acc_Cust->vLogin_Accept,
	        Acc_Cust->vUpdate_Type,                 Acc_Cust->vGroup_Id);

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createConMsg", "insert dConMsgHis sql_str = ", "[%s]",sql_str);
#endif

	memset(sql_str,  0, sizeof(sql_str));
	sprintf(sql_str,"insert into dConMsgHis"
	        "("
	        " CONTRACT_NO,CON_CUST_ID,CONTRACT_PASSWD,BANK_CUST,ODDMENT,"
	        " BELONG_CODE,PREPAY_FEE,PREPAY_TIME,STATUS,STATUS_TIME,"
	        " POST_FLAG,DEPOSIT,MIN_YM,OWE_FEE,ACCOUNT_MARK,ACCOUNT_LIMIT,"
	        " PAY_CODE,BANK_CODE,POST_BANK_CODE,ACCOUNT_NO,ACCOUNT_TYPE,"
	        " UPDATE_CODE,UPDATE_DATE,UPDATE_TIME,UPDATE_LOGIN,"
	        " UPDATE_ACCEPT,UPDATE_TYPE,GROUP_ID"
	        ")"
	        " values(to_number(:v1),to_number(:v2),nvl(:v3,' '),:v4,:v5,"
	        " :v6,"
	        " :v7,to_date(:v28,'YYYYMMDD HH24:MI:SS'),:v8,to_date(:v29,'YYYYMMDD HH24:MI:SS'),:v9,:v10,to_number(:v11),:v12,"
	        " :v13,:v14,:v15,:v16,:v17,:v18,:v19,:v20,to_number(:v21),to_date(:v30,'YYYYMMDD HH24:MI:SS'),"
	        " :v22,to_number(:v23),:v24,:v25"
	        ")");
#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createConMsg", ":Acc_Cust->vCon_Cust_Id=", "[%s]",Acc_Cust->vCon_Cust_Id);
#endif
	EXEC SQL PREPARE ins_stmt From :sql_str;
	EXEC SQL EXECUTE ins_stmt using
	        :Acc_Cust->vContract_No,                 :Acc_Cust->vCon_Cust_Id,
	        :Acc_Cust->vContract_Passwd,         :Acc_Cust->vBank_Cust,
	        :Acc_Cust->vOddment,                         :Acc_Cust->vBelong_Code,
	        :Acc_Cust->vPrepay_Fee,                  :Acc_Cust->vBegin_Time,
	        :Acc_Cust->vStatus,						:Acc_Cust->vBegin_Time,
	        :Acc_Cust->vPost_Flag,                   :Acc_Cust->vDeposit,
	        :yearMonth,                                          :Acc_Cust->vOwe_Fee,
	        :Acc_Cust->vAccount_Mark,                :Acc_Cust->vAccount_Limit,
	        :Acc_Cust->vPay_Code,                        :Acc_Cust->vBank_Code,
	        :Acc_Cust->vPost_Bank_Code,          :Acc_Cust->vAccount_No,
	        :Acc_Cust->vAccount_Type,                :Acc_Cust->vOp_Code,
	        :totalDate,                               :Acc_Cust->vBegin_Time,
	        :Acc_Cust->vLogin_No,
	        :Acc_Cust->vLogin_Accept,                :Acc_Cust->vUpdate_Type,
	        :Acc_Cust->vGroup_Id;

	if (SQLCODE !=0)
	{
	        pubLog_Debug(_FFL_, "createConMsg", "����dConMsgHis�����!", "�������SQLCODE=[%d],������Ϣ��[%s]",SQLCODE,SQLERRMSG);
	        strcpy(retMsg,"����dConMsgHis�����!");
	        return 3;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "", " ����  dConMsg��");
#endif

	strcpy(sql_str," ");
	sprintf(sql_str,"insert into dConMsg"
	        "("
	        " CONTRACT_NO, CON_CUST_ID,  CONTRACT_PASSWD, BANK_CUST,"
	        " ODDMENT,         BELONG_CODE,  PREPAY_FEE,          PREPAY_TIME,"
	        " STATUS,          STATUS_TIME,  POST_FLAG,           DEPOSIT,"
	        " MIN_YM,          OWE_FEE,          ACCOUNT_MARK,        ACCOUNT_LIMIT,"
	        " PAY_CODE,        BANK_CODE,        POST_BANK_CODE,  ACCOUNT_NO,"
	        " ACCOUNT_TYPE, GROUP_ID"
	        ")"
	        " values( "
	        "to_number('%s'),   to_number('%s'),        '%s',  '%s',"
	        "%10.2f,                        '%s',                           %10.2f,to_date('%s','YYYYMMDD HH24:MI:SS'),"
	        "'%s',                          to_date('%s','YYYYMMDD HH24:MI:SS'),                        '%s',  %10.2f,"
	        "to_number('%s'),   %10.2f,                         %d,        '%s',"
	        "'%s',                          '%s',                           '%s',  '%s',"
	        "'%s',                          '%s'"
	        ")",
	        Acc_Cust->vContract_No,                 Acc_Cust->vCon_Cust_Id,
	        Acc_Cust->vContract_Passwd,         Acc_Cust->vBank_Cust,
	        Acc_Cust->vOddment,                         Acc_Cust->vBelong_Code,
	        Acc_Cust->vPrepay_Fee,                  Acc_Cust->vBegin_Time,
	        Acc_Cust->vStatus,						Acc_Cust->vBegin_Time,
	        Acc_Cust->vPost_Flag,                   Acc_Cust->vDeposit,
	        yearMonth,                                          Acc_Cust->vOwe_Fee,
	        Acc_Cust->vAccount_Mark,                Acc_Cust->vAccount_Limit,
	        Acc_Cust->vPay_Code,                        Acc_Cust->vBank_Code,
	        Acc_Cust->vPost_Bank_Code,          Acc_Cust->vAccount_No,
	        Acc_Cust->vAccount_Type,                Acc_Cust->vGroup_Id);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "insert dConMsg sql_str = ", "[%s]",sql_str);
#endif

	/*ng���� ���ݹ������� ����ԭ�г��� by wxcrm at 20090903 begin
	strcpy(sql_str," ");
	sprintf(sql_str,"insert into dConMsg"
	        "("
	        " CONTRACT_NO,         CON_CUST_ID,"
	        " CONTRACT_PASSWD, BANK_CUST,"
	        " ODDMENT,                 BELONG_CODE,"
	        " PREPAY_FEE,          PREPAY_TIME,"
	        " STATUS,                  STATUS_TIME,"
	        " POST_FLAG,           DEPOSIT,"
	        " MIN_YM,                  OWE_FEE,"
	        " ACCOUNT_MARK,        ACCOUNT_LIMIT,"
	        " PAY_CODE,                BANK_CODE,"
	        " POST_BANK_CODE,  ACCOUNT_NO,"
	        " ACCOUNT_TYPE,        GROUP_ID"
	        ")"
	        " values( "
	        " to_number(:v1), to_number(:v2),"
	        " nvl(:v3,' '),                        :v4,"
	        " :v5,                        :v6,"
	        " :v7,                        to_date(:v23,'YYYYMMDD HH24:MI:SS'),"
	        " :v8,                        to_date(:v24,'YYYYMMDD HH24:MI:SS'),"
	        " :v9,                        :v10,"
	        " to_number(:v11),:v12,"
	        " :v13,                   :v14,"
	        " :v15,                   :v16,"
	        " :v17,                   :v18,"
	        " :v19,                   :v20"
	        ")");

	EXEC SQL PREPARE ins_stmt1 From :sql_str;
	EXEC SQL EXECUTE ins_stmt1 using
	        :Acc_Cust->vContract_No,        :Acc_Cust->vCon_Cust_Id,
	        :Acc_Cust->vContract_Passwd,:Acc_Cust->vBank_Cust,
	        :Acc_Cust->vOddment,                :Acc_Cust->vBelong_Code,
	        :Acc_Cust->vPrepay_Fee,         :Acc_Cust->vBegin_Time,
	        :Acc_Cust->vStatus,				:Acc_Cust->vBegin_Time,
	        :Acc_Cust->vPost_Flag,          :Acc_Cust->vDeposit,
	        :yearMonth,                                 :Acc_Cust->vOwe_Fee,
	        :Acc_Cust->vAccount_Mark,   :Acc_Cust->vAccount_Limit,
	        :Acc_Cust->vPay_Code,           :Acc_Cust->vBank_Code,
	        :Acc_Cust->vPost_Bank_Code, :Acc_Cust->vAccount_No,
	        :Acc_Cust->vAccount_Type,   :Acc_Cust->vGroup_Id;

	if (SQLCODE !=0)
	{
		pubLog_Debug(_FFL_, "createConMsg", "����dConMsg�����!", "�������SQLCODE=[%d],������Ϣ��[%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"����dConMsg�����!");
		return 4;
	}
	ng���� ���ݹ������� ����ԭ�г��� by wxcrm at 20090903 end*/
	/*ng���� ���ݹ������� by wxcrm at 20090903 begin*/
	sprintf(vOddment		,"%2.2f",	Acc_Cust->vOddment		);	
	sprintf(vPrepay_Fee		,"%12.2f",	Acc_Cust->vPrepay_Fee	);
	sprintf(vDeposit		,"%12.2f",	Acc_Cust->vDeposit		);
	sprintf(vOwe_Fee		,"%12.2f",	Acc_Cust->vOwe_Fee		);
	sprintf(vAccount_Mark	,"%d",	Acc_Cust->vAccount_Mark	);
	
	Trim(vPrepay_Fee);
	Trim(vOddment);
	Trim(vDeposit);
	Trim(vOwe_Fee);
	Trim(vAccount_Mark);
	
	if (strlen(Acc_Cust->vContract_Passwd)==0)
	{
		strcpy(Acc_Cust->vContract_Passwd," ");
	}
	
	strcpy(vTdConMsg.sContract_no		,Acc_Cust->vContract_No		);
	strcpy(vTdConMsg.sConCustId     	,Acc_Cust->vCon_Cust_Id		);
	strcpy(vTdConMsg.sContractPasswd	,Acc_Cust->vContract_Passwd	);
	strcpy(vTdConMsg.sBankCust   		,Acc_Cust->vBank_Cust		);
	strcpy(vTdConMsg.sOddment    		,vOddment			 	 	);
	strcpy(vTdConMsg.sBelongCode    	,Acc_Cust->vBelong_Code		);
	strcpy(vTdConMsg.sPrepayFee      	,vPrepay_Fee				);
	strcpy(vTdConMsg.sPrepayTime  		,Acc_Cust->vBegin_Time		);
	strcpy(vTdConMsg.sStatus      		,Acc_Cust->vStatus			);
	strcpy(vTdConMsg.sStatus_time  		,Acc_Cust->vBegin_Time		);
	strcpy(vTdConMsg.sPostFlag     		,Acc_Cust->vPost_Flag		);
	strcpy(vTdConMsg.sDeposit      		,vDeposit					);
	strcpy(vTdConMsg.sMinYm    			,yearMonth					);
	strcpy(vTdConMsg.sOweFee 			,vOwe_Fee					);
	strcpy(vTdConMsg.sAccountMark		,vAccount_Mark				);
	strcpy(vTdConMsg.sAccountLimit 		,Acc_Cust->vAccount_Limit	);
	strcpy(vTdConMsg.sPayCode        	,Acc_Cust->vPay_Code		);
	strcpy(vTdConMsg.sBankCode      	,Acc_Cust->vBank_Code		);
	strcpy(vTdConMsg.sPostBankCode 		,Acc_Cust->vPost_Bank_Code	);
	strcpy(vTdConMsg.sAccountNo 		,Acc_Cust->vAccount_No		);
	strcpy(vTdConMsg.sAccountType 		,Acc_Cust->vAccount_Type	);
	strcpy(vTdConMsg.sGroupId   		,Acc_Cust->vGroup_Id		);
	
	ret = 0;
	ret = OrderInsertConMsg("3",Acc_Cust->vContract_No,100,Acc_Cust->vOp_Code,atol(Acc_Cust->vLogin_Accept),Acc_Cust->vLogin_No,"createConMsg",vTdConMsg);
	if (ret != 0)
	{
		pubLog_Debug(_FFL_, "createConMsg", "����dConMsg�����!", "�������SQLCODE=[%d],������Ϣ��[%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"����dConMsg�����!");
		return 4;
	}
	/*ng���� ���ݹ������� by wxcrm at 20090903 end*/
	return 0;
}


/**
* �����ͻ�������Ϣ
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   �ͻ������ṹ��   tCustDoc *doc
*
* @inparam   �ͻ�id               doc->vCust_Id               long;
* @inparam   �ͻ���������         doc->vRegion_Code           char(2);
* @inparam   �ͻ����ش���         doc->vDistrict_Code         char(2);
* @inparam   �ͻ��������         doc->vTown_Code             char(3;
* @inparam   ��������             doc->vOrg_Code              char(9);
* @inparam   �����ڵ�             doc->vGroup_Id              char(10);
* @inparam   ����ʶ��             doc->vOrg_Id                char(10);
* @inparam   �ͻ�����             doc->vCust_Name             char(60);
* @inparam   �ͻ�����             doc->vCust_Passwd           char(8);
* @inparam   �ͻ�״̬             doc->vCust_Status           char(2);
* @inparam   �ͻ�״̬ʱ��         doc->vRun_Time              char(17) YYYYMMDD HH24:MI:SS;
* @inparam   ��������ʱ��         doc->vCreate_Time           char(17) YYYYMMDD HH24:MI:SS;
* @inparam   ����ע��ʱ��         doc->vClose_Time            char(17) YYYYMMDD HH24:MI:SS;
* @inparam   �ͻ��ȼ�����         doc->vOwner_Grade           char(2);
* @inparam   �ͻ�����             doc->vOwner_Type            char(2);
* @inparam   �ͻ�סַ             doc->vCust_Addr             char(128);
* @inparam   ֤������             doc->vId_Type               char(1);
* @inparam   ֤������             doc->vId_Iccid              char(20);
* @inparam   ֤����ַ             doc->vId_Addr               char(60);
* @inparam   ֤����Ч��           doc->vId_ValidDate          char(17) YYYYMMDD;
* @inparam   ��ϵ��               doc->vContact_Person        char(20);
* @inparam   ��ϵ�绰             doc->vContact_Phone         char(20);
* @inparam   ��ϵ��ַ             doc->vContact_Addr          char(128);
* @inparam   ��ϵ����������       doc->vContact_Post          char(6);
* @inparam   ��ϵ��ͨ�ŵ�ַ       doc->vContact_MailAddr      char(60);
* @inparam   ��ϵ�˴���           doc->vContact_Fax           char(30);
* @inparam   ��ϵ��E-mail         doc->vContact_Email         char(30);
* @inparam   ������ע             doc->vCreate_Note           char(60);
* @inparam   ������ˮ             doc->vLogin_Accept          long;
* @inparam   ����ʱ��             doc->vOp_Time               char(17) YYYYMMDD HH24:MI:SS;
* @inparam   ��������             doc->vTotal_Date            int;
* @inparam   ��������             doc->vLogin_No              char(6);
* @inparam   ��������             doc->vOp_Code               char(4);
*
* @outparam  ���ش�����Ϣ   retMsg   char(60)
*
* @return        0:  �ɹ�
* @return        1:  error of insert dCustDocHis;
* @return        2:  error of insert dCustDoc;
*/
int createCustDoc(tCustDoc *doc,char *retMsg)
{
        char vSQL_Text[5096+1];                 /* SQL���           */
        char vTmp_String[5096+1];               /* ��ʱ����          */
        char vClose_Time[8+1];                  /* ����ʧЧʱ��      */
        char vOp_Type[1+1];                     /* ��������          */
        long vUnit_Id;                          /* UNIT id           */
        char vStatus[1+1];						/* �ͻ�״̬			 */
        char regionGrpId[10+1];
        char districtGrpId[10+1];
        char vReturn_Msg[255+1];                /* ������Ϣ          */
		char vVpmn_flag[1+1];					/* ivpn���ű�־		 */
		
		/*ng����*/
		int	ret = 0;
		TdCustDoc vTdCustDoc;
		char vCustIdStr[14+1];
		
		memset(&vTdCustDoc, 0, sizeof(vTdCustDoc));
		
#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createCustDoc", "", "createCustDoc1: Starting Execute");
#endif

		Trim(doc->vGroup_Id);
		Trim(doc->vOrg_Id);

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�id                =[%ld]", doc->vCust_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ���������  = [%s]", doc->vRegion_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ����ش���  = [%s]", doc->vDistrict_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ��������  = [%s]", doc->vTown_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��������          = [%s]", doc->vOrg_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�����ڵ�          = [%s]", doc->vGroup_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:����ʶ��          = [%s]", doc->vOrg_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�����          = [%s]", doc->vCust_Name);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�����          = [%s]", doc->vCust_Passwd);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�״̬          = [%s]", doc->vCust_Status);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�״̬ʱ��      = [%s]", doc->vRun_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��������ʱ��      = [%s]", doc->vCreate_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:����ע��ʱ��      = [%s]", doc->vClose_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ��ȼ�����      = [%s]", doc->vOwner_Grade);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�����          = [%s]", doc->vOwner_Type);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ͻ�סַ          = [%s]", doc->vCust_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:֤������          = [%s]", doc->vId_Type);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:֤������          = [%s]", doc->vId_Iccid);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:֤����ַ          = [%s]", doc->vId_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:֤����Ч��        = [%s]", doc->vId_ValidDate);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ��            = [%s]", doc->vContact_Person);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ�绰          = [%s]", doc->vContact_Phone);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ��ַ          = [%s]", doc->vContact_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ����������    = [%s]", doc->vContact_Post);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ��ͨ�ŵ�ַ    = [%s]", doc->vContact_MailAddr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ�˴���        = [%s]", doc->vContact_Fax);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ϵ��E-mail      = [%s]", doc->vContact_Email);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:�ϼ��ͻ�ID        = [%s]", doc->vParent_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:������ע          = [%s]", doc->vCreate_Note);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:������ˮ          = [%ld]", doc->vLogin_Accept);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:����ʱ��          = [%s]", doc->vOp_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��������          = [%d]", doc->vTotal_Date);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��������          = [%s]", doc->vLogin_No);
		pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��ʵ��ʶ          = [%s]", doc->vTrue_Flag);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:��������          = [%s]", doc->vOp_Code);

#endif

        /*
        * ���� dCustDocHis ��
        */

        memset(vSQL_Text,   0,   sizeof(vSQL_Text));
        memset(vTmp_String, 0,   sizeof(vTmp_String));
        memset(vClose_Time, 0,   sizeof(vClose_Time));
        memset(vOp_Type,    0,   sizeof(vOp_Type));
        memset(&vUnit_Id,   0,   sizeof(vUnit_Id));
        memset(vReturn_Msg, 0,   sizeof(vReturn_Msg));
		memset(vStatus,		0,   sizeof(vStatus));
		memset(vVpmn_flag,	0,   sizeof(vVpmn_flag));
		memset(vCustIdStr,	0,   sizeof(vCustIdStr));
		

        strcpy(vClose_Time, "20500101");
        strcpy(vOp_Type,        "a");
		vStatus[0]=doc->vCust_Status[1];
		vStatus[1]='\0';
        /*
        * ����ͻ�������Ϣ��ʷ��dCustDocHis
        */

        strcpy(vSQL_Text,"INSERT INTO dCustDocHis ");
        strcat(vSQL_Text,"(Cust_Id,            Region_Code,          District_Code,");
        strcat(vSQL_Text,"Town_Code,           Cust_Name,            Cust_Passwd,");
        strcat(vSQL_Text,"Cust_Status,         Run_Time,             Owner_Grade,");
        strcat(vSQL_Text,"Owner_Type,          Cust_Address,         Id_Type,");
        strcat(vSQL_Text,"Id_Iccid,            Id_Address,           Id_ValidDate,");
        strcat(vSQL_Text,"Contact_Person,      Contact_Phone,        Contact_Address,");
        strcat(vSQL_Text,"Contact_Post,        Contact_MailAddress,");
        strcat(vSQL_Text,"Contact_Fax,         Contact_Emaill,       Org_Code,");
        strcat(vSQL_Text,"Create_Time,         Close_Time,           Parent_Id,");
        strcat(vSQL_Text,"Update_Accept,       Update_Time,          Update_Date,");
        strcat(vSQL_Text,"Update_Login,        Update_Type,          Update_Code,");
        strcat(vSQL_Text,"Group_Id,            Org_Id)");
        strcat(vSQL_Text," VALUES( ");

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s %ld,  '%s', '%s',",vTmp_String,doc->vCust_Id,doc->vRegion_Code,doc->vDistrict_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vTown_Code,doc->vCust_Name,doc->vCust_Passwd);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', To_Date('%s','YYYYMMDD HH24:MI:SS'), '%s',",vTmp_String,doc->vCust_Status,doc->vRun_Time,doc->vOwner_Grade);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vOwner_Type,doc->vCust_Addr,doc->vId_Type);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', To_Date('%s','YYYYMMDD'),",vTmp_String,doc->vId_Iccid,doc->vId_Addr,doc->vId_ValidDate);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vContact_Person,doc->vContact_Phone,doc->vContact_Addr);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s',",vTmp_String,doc->vContact_Post,doc->vContact_MailAddr);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vContact_Fax,doc->vContact_Email,doc->vOrg_Code);


        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'), To_Date('%s','YYYYMMDD'),'%s',",
                vTmp_String,doc->vCreate_Time,vClose_Time,doc->vParent_Id);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s %ld, To_Date('%s','YYYYMMDD HH24:MI:SS'), %d,",vTmp_String,doc->vLogin_Accept,doc->vOp_Time,doc->vTotal_Date);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s','%s', '%s',",vTmp_String,doc->vLogin_No,vOp_Type,doc->vOp_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s')",vTmp_String,doc->vGroup_Id,doc->vOrg_Id);

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createCustDoc", "", "createCustDoc: vSQL_Text=[%s]",vSQL_Text);
#endif
        strcpy(vSQL_Text,"INSERT INTO dCustDocHis ");
        strcat(vSQL_Text,"(Cust_Id,            Region_Code,          District_Code,");
        strcat(vSQL_Text,"Town_Code,           Cust_Name,            Cust_Passwd,");
        strcat(vSQL_Text,"Cust_Status,         Run_Time,             Owner_Grade,");
        strcat(vSQL_Text,"Owner_Type,          Cust_Address,         Id_Type,");
        strcat(vSQL_Text,"Id_Iccid,            Id_Address,           Id_ValidDate,");
        strcat(vSQL_Text,"Contact_Person,      Contact_Phone,        Contact_Address,");
        strcat(vSQL_Text,"Contact_Post,        Contact_MailAddress,");
        strcat(vSQL_Text,"Contact_Fax,         Contact_Emaill,       Org_Code,");
        strcat(vSQL_Text,"Create_Time,         Close_Time,           Parent_Id,");
        strcat(vSQL_Text,"Update_Accept,       Update_Time,          Update_Date,");
        strcat(vSQL_Text,"Update_Login,        Update_Type,          Update_Code,");
        strcat(vSQL_Text,"Group_Id,            Org_Id)");
        strcat(vSQL_Text," VALUES( ");
        strcat(vSQL_Text,":v1,  :v2, :v3,");
        strcat(vSQL_Text,":v4,  :v5 ,nvl(:v6,' '),");
        strcat(vSQL_Text,":v7, To_Date(:v8,'YYYYMMDD HH24:MI:SS'), :v9,");
        strcat(vSQL_Text,":v10, :v11, :v12,");
        strcat(vSQL_Text,":v13, :v14, To_Date(:v15,'YYYYMMDD'),");
        strcat(vSQL_Text,":v16, :v17, :v18,");
        strcat(vSQL_Text,":v19, :v20,");
        strcat(vSQL_Text,":v21, :v22, :v23,");
        strcat(vSQL_Text,"To_Date(:v24,'YYYYMMDD HH24:MI:SS'), To_Date(:v25,'YYYYMMDD'),:v26,");
        strcat(vSQL_Text,":v27, To_Date(:v28,'YYYYMMDD HH24:MI:SS'), :v29,");
        strcat(vSQL_Text,":v30, :v31, :v32,");
        strcat(vSQL_Text,":v33, :v34)");
        EXEC SQL PREPARE Exec_dCustDocHis FROM :vSQL_Text;
        EXEC SQL EXECUTE Exec_dCustDocHis using
                :doc->vCust_Id,           :doc->vRegion_Code,           :doc->vDistrict_Code,
                :doc->vTown_Code,         :doc->vCust_Name,              :doc->vCust_Passwd,
                :doc->vCust_Status,       :doc->vRun_Time,              :doc->vOwner_Grade,
                :doc->vOwner_Type,        :doc->vCust_Addr,             :doc->vId_Type,
                :doc->vId_Iccid,          :doc->vId_Addr,               :doc->vId_ValidDate,
                :doc->vContact_Person,    :doc->vContact_Phone,         :doc->vContact_Addr,
                :doc->vContact_Post,      :doc->vContact_MailAddr,
                :doc->vContact_Fax,       :doc->vContact_Email,         :doc->vOrg_Code,
                :doc->vCreate_Time,       :vClose_Time,                 :doc->vParent_Id,
                :doc->vLogin_Accept,      :doc->vOp_Time,               :doc->vTotal_Date,
                :doc->vLogin_No,          :vOp_Type,                    :doc->vOp_Code,
                :doc->vGroup_Id,          :doc->vOrg_Id;

        if (SQLCODE != OK)
        {
                sprintf(vReturn_Msg,"����ͻ�������ʷ���ϱ�[dCustDocHis]��ʧ��,SQLCODE=[%d]",SQLCODE);
                pubLog_Debug(_FFL_, "createCustDoc", "����ͻ�������ʷ��ʷ���ϱ�[dCustDocHis]��ʧ��,SQLCODE=", "[%d][%s]",SQLCODE,SQLERRMSG);

                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
                        retMsg[strlen(vReturn_Msg)] = '\0';
                }
                return 1;
        }

        /*
        * ����ͻ�������Ϣ��dCustDoc
        *
        * commented by lab 2005.1.24
        */

        strcpy(vSQL_Text,"INSERT INTO dCustDoc ");
        strcat(vSQL_Text,"(Cust_Id,            Region_Code,          District_Code,");
        strcat(vSQL_Text,"Town_Code,           Cust_Name,            Cust_Passwd,");
        strcat(vSQL_Text,"Cust_Status,         Run_Time,             Owner_Grade,");
        strcat(vSQL_Text,"Owner_Type,          Cust_Address,         Id_Type,");
        strcat(vSQL_Text,"Id_Iccid,            Id_Address,           Id_ValidDate,");
        strcat(vSQL_Text,"Contact_Person,      Contact_Phone,        Contact_Address,");
        strcat(vSQL_Text,"Contact_Post,        Contact_MailAddress,");
        strcat(vSQL_Text,"Contact_Fax,         Contact_Emaill,       Org_Code,");
        strcat(vSQL_Text,"Create_Time,         Close_Time,           Parent_Id,");
        strcat(vSQL_Text,"Group_Id,            Org_Id,");
        strcat(vSQL_Text,"TRUE_FLAG,           INFO_TYPE,            AGREEMENT_TYPE)");
        strcat(vSQL_Text," VALUES( ");

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s %ld,  '%s', '%s',",vTmp_String,doc->vCust_Id,doc->vRegion_Code,doc->vDistrict_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vTown_Code,doc->vCust_Name,doc->vCust_Passwd);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', To_Date('%s','YYYYMMDD HH24:MI:SS'), '%s',",vTmp_String,doc->vCust_Status,doc->vRun_Time,doc->vOwner_Grade);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vOwner_Type,doc->vCust_Addr,doc->vId_Type);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', To_Date('%s','YYYYMMDD'),",vTmp_String,doc->vId_Iccid,doc->vId_Addr,doc->vId_ValidDate);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vContact_Person,doc->vContact_Phone,doc->vContact_Addr);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s',",vTmp_String,doc->vContact_Post,doc->vContact_MailAddr);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,doc->vContact_Fax,doc->vContact_Email,doc->vOrg_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'), To_Date('%s','YYYYMMDD'),'%s',",
                vTmp_String,doc->vCreate_Time,vClose_Time,doc->vParent_Id);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s',",vTmp_String,doc->vGroup_Id,doc->vOrg_Id);

	    memset(vTmp_String, 0,	sizeof(vTmp_String));				 strcpy(vTmp_String,vSQL_Text);
        sprintf(vSQL_Text,"%s '%s','%s','%s')",vTmp_String,doc->vTrue_Flag,doc->vInfo_Type,doc->vAgreement_Type);



#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "createCustDoc", "vSQL_Text=", " [%s]",vSQL_Text);
#endif
		/*ng���� by wxcrm at 20090812 begin
        strcpy(vSQL_Text,"INSERT INTO dCustDoc ");
        strcat(vSQL_Text,"(Cust_Id,            Region_Code,          District_Code,");
        strcat(vSQL_Text,"Town_Code,           Cust_Name,            Cust_Passwd,");
        strcat(vSQL_Text,"Cust_Status,         Run_Time,             Owner_Grade,");
        strcat(vSQL_Text,"Owner_Type,          Cust_Address,         Id_Type,");
        strcat(vSQL_Text,"Id_Iccid,            Id_Address,           Id_ValidDate,");
        strcat(vSQL_Text,"Contact_Person,      Contact_Phone,        Contact_Address,");
        strcat(vSQL_Text,"Contact_Post,        Contact_MailAddress,");
        strcat(vSQL_Text,"Contact_Fax,         Contact_Emaill,       Org_Code,");
        strcat(vSQL_Text,"Create_Time,         Close_Time,           Parent_Id,");
        strcat(vSQL_Text,"Group_Id,            Org_Id,");
        strcat(vSQL_Text,"TRUE_FLAG,		   INFO_TYPE,					 AGREEMENT_TYPE)");
        strcat(vSQL_Text," VALUES( ");
        strcat(vSQL_Text," :v1,  :v2, :v3,");
        strcat(vSQL_Text," :v4,  :v5, nvl(:v6, ' '),");
        strcat(vSQL_Text," :v7,  To_Date(:v8,'YYYYMMDD HH24:MI:SS'), :v9,");
        strcat(vSQL_Text," :v10, :v11, :v12,");
        strcat(vSQL_Text," :v13, :v14, To_Date(:v15,'YYYYMMDD'),");
        strcat(vSQL_Text," :v16, :v17, :v18,");
        strcat(vSQL_Text," :v19, :v20,");
        strcat(vSQL_Text," :v21, :v22, :v23,");
        strcat(vSQL_Text," To_Date(:v24,'YYYYMMDD HH24:MI:SS'), To_Date(:v25,'YYYYMMDD'),:v26,");
        strcat(vSQL_Text," :v27, :v28,");
        strcat(vSQL_Text," :v34, :v35,:v36)");

        EXEC SQL PREPARE Exec_dCustDoc FROM :vSQL_Text;
        EXEC SQL EXECUTE Exec_dCustDoc using
                :doc->vCust_Id,        :doc->vRegion_Code,          :doc->vDistrict_Code,
                :doc->vTown_Code,     :doc->vCust_Name,             :doc->vCust_Passwd,
                :doc->vCust_Status,   :doc->vRun_Time,              :doc->vOwner_Grade,
                :doc->vOwner_Type,    :doc->vCust_Addr,             :doc->vId_Type,
                :doc->vId_Iccid,      :doc->vId_Addr,               :doc->vId_ValidDate,
                :doc->vContact_Person,:doc->vContact_Phone,         :doc->vContact_Addr,
                :doc->vContact_Post,  :doc->vContact_MailAddr,
                :doc->vContact_Fax,   :doc->vContact_Email,         :doc->vOrg_Code,
                :doc->vCreate_Time,   :vClose_Time,                 :doc->vParent_Id,
                :doc->vGroup_Id,      :doc->vOrg_Id,
                :doc->vTrue_Flag,     :doc->vInfo_Type,  :doc->vAgreement_Type;

        if (SQLCODE != OK)
        {
                sprintf(vReturn_Msg,"����ͻ�������[dCustDoc]��ʧ��,SQLCODE=[%d]",SQLCODE);
                pubLog_Debug(_FFL_, "createCustDoc", "����ͻ��������ϱ�[dCustDoc]��ʧ��", ",SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
                        retMsg[strlen(vReturn_Msg)] = '\0';
                }
                return 2;
        }
		ng���� by wxcrm at 20090812 end*/
		sprintf(vCustIdStr,"%ld",doc->vCust_Id);
		Trim(vCustIdStr);
		if(vTdCustDoc.sCustPasswd[0]=='\0')
		{
			strcpy(vTdCustDoc.sCustPasswd," ");
		}
		strncpy(vTdCustDoc.sCustId				,	vCustIdStr				,10	);
		strncpy(vTdCustDoc.sRegionCode			,	doc->vRegion_Code		,2	);
		strncpy(vTdCustDoc.sDistrictCode		,	doc->vDistrict_Code		,2);
		strncpy(vTdCustDoc.sTownCode			,	doc->vTown_Code			,3);
		strncpy(vTdCustDoc.sCustName			,	doc->vCust_Name			,60);
		strncpy(vTdCustDoc.sCustPasswd			,	doc->vCust_Passwd		,8);
		strncpy(vTdCustDoc.sCustStatus			,	doc->vCust_Status		,2);
		strncpy(vTdCustDoc.sRunTime				,	doc->vRun_Time			,17);
		strncpy(vTdCustDoc.sOwnerGrade			,	doc->vOwner_Grade		,2);
		strncpy(vTdCustDoc.sOwnerType			,	doc->vOwner_Type		,2);
		strncpy(vTdCustDoc.sCustAddress			,	doc->vCust_Addr			,60);
		strncpy(vTdCustDoc.sIdType				,	doc->vId_Type			,2);
		strncpy(vTdCustDoc.sIdIccid				,	doc->vId_Iccid			,20);
		strncpy(vTdCustDoc.sIdAddress			,	doc->vId_Addr			,60);
		strncpy(vTdCustDoc.sIdValiddate			,	doc->vId_ValidDate		,17);
		strncpy(vTdCustDoc.sContactPerson		,	doc->vContact_Person	,20);
		strncpy(vTdCustDoc.sContactPhone		,	doc->vContact_Phone		,20);
		strncpy(vTdCustDoc.sContactAddress		,	doc->vContact_Addr		,100);
		strncpy(vTdCustDoc.sContactPost			,	doc->vContact_Post		,6);
		strncpy(vTdCustDoc.sContactMailaddress	,	doc->vContact_MailAddr	,60);
		strncpy(vTdCustDoc.sContactFax			,	doc->vContact_Fax		,30);
		strncpy(vTdCustDoc.sContactEmaill		,	doc->vContact_Email		,30);
		strncpy(vTdCustDoc.sOrgCode				,	doc->vOrg_Code			,9);
		strncpy(vTdCustDoc.sCreateTime			,	doc->vCreate_Time		,17);
		strncpy(vTdCustDoc.sCloseTime			,	vClose_Time				,17);
		strncpy(vTdCustDoc.sParentId			,	doc->vParent_Id			,14);
		strncpy(vTdCustDoc.sCreateNote			,	""						,60);
		strncpy(vTdCustDoc.sTrueFlag			,	doc->vTrue_Flag			,1);
		strncpy(vTdCustDoc.sInfoType			,	doc->vInfo_Type			,2);
		strncpy(vTdCustDoc.sAgreementType		,	doc->vAgreement_Type	,1);
		strncpy(vTdCustDoc.sGroupId				,	doc->vGroup_Id			,10);
		strncpy(vTdCustDoc.sOrgId				,	doc->vOrg_Id			,10);
		Trim(vTdCustDoc.sParentId);
		ret = OrderInsertCustDoc("1",vCustIdStr,100,doc->vOp_Code,doc->vLogin_Accept,doc->vLogin_No,"createCustDoc",vTdCustDoc);
		if (ret != 0)
		{
			sprintf(vReturn_Msg,"����ͻ�������[dCustDoc]��ʧ��,SQLCODE=[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "createCustDoc", "����ͻ��������ϱ�[dCustDoc]��ʧ��", ",SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
            if (retMsg != NULL)
            {
            	strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
            	retMsg[strlen(vReturn_Msg)] = '\0';
            }
            return 2;
		}
		return 0;
}



/**
* ���뿪�ػ������־��
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   �ͻ�״̬�ṹ��   tChgList *log
*
* @inparam   ��ʶ��                   log->vCommand_Id        long
* @inparam   �û�ID                   log->vId_No             long
* @inparam   ��������                 log->vTotal_Date        long
* @inparam   ������ˮ                 log->vLogin_Accept      long
* @inparam   ������                   log->vBelong_Code       char(7)
* @inparam   �û�����                 log->vSm_Code           char(2)
* @inparam   �û�����                 log->vAttr_Code         char(8)
* @inparam   �������                 log->vPhone_No          char(15)
* @inparam   ����״̬                 log->vRun_Code          char(2)
* @inparam   ��������                 log->vOrg_Code          char(9)
* @inparam   ��������                 log->vLogin_No          char(6)
* @inparam   ��������                 log->vOp_Code           char(4)
* @inparam   ������ע                 log->vChange_Reason     char(256)
* @inparam   �ڵ��ʶ��               log->vGroup_Id          char(10)
* @inparam   ӪҵԱ��ʶ               log->vOrg_Id            char(10)
*
* @outparam  ���ش�����Ϣ   retMsg   char(60)
*
* @return        0:  �ɹ�
* @return        1:  ����wChgList��ʧ��;
*/



int recordChgList(tChgList* vwChgList,char* retMsg)
{
        EXEC SQL BEGIN DECLARE SECTION;
        char vSQL_Text[2048];                         /* �洢��̬SQL��䴮  */
        char vTmp_String[2048];                       /* �洢��̬SQL��䴮  */
        int  vCmd_Right=0;                            /* �������ȼ�         */
        char vOldTime[14+1];                          /*���ʱ��-add by liuweib*/
        EXEC SQL END DECLARE SECTION;
 
       	Trim(vwChgList->vGroup_Id);
       	Trim(vwChgList->vOrg_Id);
#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg���ػ�ID   = [%ld]",vwChgList->vCommand_Id);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg�û�ID     = [%ld]",vwChgList->vId_No);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg��������   = [%ld]",vwChgList->vTotal_Date);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg������ˮ   = [%ld]",vwChgList->vLogin_Accept);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg��������   = [%s]", vwChgList->vBelong_Code);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg���Դ���   = [%s]", vwChgList->vAttr_Code);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msgҵ�����   = [%s]", vwChgList->vSm_Code);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg�������   = [%s]", vwChgList->vPhone_No);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg��������   = [%s]", vwChgList->vOrg_Code);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg���ʱ��   = [%s]", vwChgList->vOld_Time);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg��������   = [%s]", vwChgList->vLogin_No);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg��������   = [%s]", vwChgList->vOp_Code);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg����ԭ��   = [%s]", vwChgList->vChange_Reason);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg�����ڵ�   = [%s]", vwChgList->vGroup_Id);
        pubLog_Debug(_FFL_, "recordChgList", "", "debug msg���ű�ʶ   = [%s]", vwChgList->vOrg_Id);
#endif
        memset(vSQL_Text,   0, sizeof(vSQL_Text));
        memset(vTmp_String, 0, sizeof(vTmp_String));
        memset(vOldTime,    0, sizeof(vOldTime));
        
        /*������ʹ��old_time�ֶ�,add by liueweib 20090304*/
        EXEC SQL select nvl(:vwChgList->vOld_Time,to_char(sysdate,'yyyymmddhh24miss'))
                   into :vOldTime
                   from dual;
        if(SQLCODE != OK)
        	{
        		pubLog_Debug(_FFL_, "recordChgList", "recordChgList ��ѯʱ����Ϣʧ��", ",SQLCODE=[%d],������Ϣ��[%s]",SQLCODE,SQLERRMSG);
            strcpy(retMsg,"����wChgList��ʧ��!");
            return 2;
        	}
       	Trim(vOldTime);
        /*������ʹ��old_time�ֶ�,add by liueweib 20090304*/

        strcat(vSQL_Text, " INSERT INTO wChgList ");
        strcat(vSQL_Text, " (Command_Id,    Id_No,         Total_Date,");
        strcat(vSQL_Text, " Login_Accept,   Belong_Code,   Sm_Code,");
        strcat(vSQL_Text, " Attr_Code,      Phone_No,      Run_Code,");
        strcat(vSQL_Text, " Old_Time,       Op_Time,       Org_Code,");
        strcat(vSQL_Text, " Login_No,       Op_Code,       Cmd_Right,");
        strcat(vSQL_Text, " Change_Reason,  Group_Id,      Org_Id) ");
        strcat(vSQL_Text, " VALUES( ");

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s %ld,         %ld,          %ld,", vTmp_String, vwChgList->vCommand_Id, vwChgList->vId_No, vwChgList->vTotal_Date);

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s %ld,         '%s',          '%s',", vTmp_String, vwChgList->vLogin_Accept, vwChgList->vBelong_Code, vwChgList->vSm_Code);

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s '%s',        '%s',          '%s',", vTmp_String, vwChgList->vAttr_Code, vwChgList->vPhone_No, vwChgList->vRun_Code);

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s SysDate,        SysDate,    '%s',", vTmp_String, vwChgList->vOrg_Code);

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s '%s',        '%s',          %d,", vTmp_String, vwChgList->vLogin_No, vwChgList->vOp_Code, vCmd_Right);

        memset(vTmp_String,0, sizeof(vTmp_String)); strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text, "%s '%s',        '%s',          '%s') ", vTmp_String, vwChgList->vChange_Reason, vwChgList->vGroup_Id, vwChgList->vOrg_Id);

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "recordChgList", "debug msgvSQL_Text=", "[%s]",vSQL_Text);
#endif


        EXEC SQL INSERT INTO wChgList(Command_Id,                    Id_No,                    Total_Date,
                                      Login_Accept,                  Belong_Code,              Sm_Code,
                                      Attr_Code,                     Phone_No,                 Run_Code,
                                      Old_Time,                      Op_Time,                  Org_Code,
                                      Login_No,                      Op_Code,                  Cmd_Right,
                                      Change_Reason,                 Group_Id,                 Org_Id)
                               VALUES(:vwChgList->vCommand_Id,      :vwChgList->vId_No,       :vwChgList->vTotal_Date,
                                      :vwChgList->vLogin_Accept,    :vwChgList->vBelong_Code, :vwChgList->vSm_Code,
                                      :vwChgList->vAttr_Code,       :vwChgList->vPhone_No,    :vwChgList->vRun_Code,
                                      to_date(:vOldTime,'yyyymmdd hh24:mi:ss'),     SysDate,   :vwChgList->vOrg_Code,
                                      :vwChgList->vLogin_No,        :vwChgList->vOp_Code,     :vCmd_Right,
                                      :vwChgList->vChange_Reason,   :vwChgList->vGroup_Id,    :vwChgList->vOrg_Id);

        if (SQLCODE != OK)
        {
                pubLog_Debug(_FFL_, "recordChgList", "recordChgList ����wChgList��ʧ��,", "SQLCODE=[%d],������Ϣ��[%s]",SQLCODE,SQLERRMSG);
                strcpy(retMsg,"����wChgList��ʧ��!");
                return 1;
        }
        return 0;

}


/*
 *
 *	ChgFlag   :  Y��ʾ��wChgYYYYMM���в���,N��ʾ������
 *	CustFlag  :	 Y��ʾ��dCustAssure���в���,N��ʾ������
 *
 *
 * @outparam   ���ش�����Ϣ   retMsg                              char(60)
 *
 * @return        0:  �ɹ�
 * @return        1:  �����ܽ��С��������� + ����Ԥ��
 * @return        3:  ����ȫ��ͨ�����־��ʧ��[wChgYYYMM]
 * @return        4:  ����ӪҵԱ������־��ʧ��[wLoginOprYYYYMM]
 * @return        7:  log->vOther_Fee����ֱ�Ӹ�ֵ,���ɺ����ڲ���ɸ�ֵ
 * @return       10:  �ֽ�+֧Ʊ != �����ڸ�������ϸ��֮��!
 * @return       11:  ��ѯ�ͻ�������Ϣ��dCustAssure����!
 * @return       12:  ����ͻ�������Ϣ��dCustAssure����!
 * @return       13:  �޸Ŀͻ�������Ϣ��dCustAssure����!
 */
int recordOprLog(tLoginOpr *log,char *retMsg,char *ChgFlag,char *CustFlag)
{
        int    i=0,j=0;                           /* �����±�          */
        int    max=0;                             /* ��¼FeeDetail     *
                                                   * ��ǰ��Ч��¼��    */
        int    vErr_Count;                        /* ������          */
        int    vErr_Count2;                       /* ������          */
        char   vSQL_Text[4096+1];                 /* SQL���           */
        char   vTmp_String[4096+1];               /* ��ʱ����          */
        char   dynStmt[4096+1];   								/* ��ʱ����          */
        char   vBill_Year[4+1];                   /* ��ǰ��            */
        char   vBill_Month[2+1];                  /* ��ǰ��            */
        char   vReturn_Msg[255+1];                /* ������Ϣ          */
        char   vOrg_Group[11];
				char   vassureRowId[21+1];
				
        while(i<MAXBUFRECORDS)   
        {
        	log->logFee[i] = &(log->vLogFee[i]);
        	i++;
        }


#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "recordOprLog", "", "recordOprLog: Starting Execute");
#endif

		Trim(log->vGroup_Id);
		Trim(log->vOrg_Id);

#ifdef _DEBUG_

        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:�û�ID        =        [%ld]", log->vId_No);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%d]", log->vTotal_Date);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������ˮ          =        [%ld]", log->vLogin_Accept);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:ҵ�����          =         [%s]", log->vSm_Code);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vBelong_Code);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:�ƶ�����          =         [%s]", log->vPhone_No);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vOrg_Code);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vLogin_No);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vOp_Code);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:ϵͳʱ��          =         [%s]", log->vOp_Time);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vMachine_Code);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:�ܽ��            =     [%10.2f]", log->vPay_Money);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:�ֽ�              =     [%10.2f]", log->vCash_Pay);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:֧Ʊ              =     [%10.2f]", log->vCheck_Pay);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:SIM����           =     [%10.2f]", log->vSim_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������            =     [%10.2f]", log->vMachine_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������            =     [%10.2f]", log->vInnet_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:ѡ�ŷ�            =     [%10.2f]", log->vChoice_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:SIM��Ԥ��         =     [%10.2f]", log->vSim_Prepay);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������            =     [%10.2f]", log->vOther_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������            =     [%10.2f]", log->vHand_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:Ѻ��              =     [%10.2f]", log->vDeposit);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:���˱�־          =         [%s]", log->vBack_Flag);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:�����ֶ�          =         [%s]", log->vEncrypt_Fee);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:ϵͳ��ע          =         [%s]", log->vSystem_Note);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������ע          =         [%s]", log->vOp_Note);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:����ID            =         [%s]", log->vGroup_Id);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:���ű�ʶ          =         [%s]", log->vOrg_Id);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:��������          =         [%s]", log->vPay_Type);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:IP��ַ            =         [%s]", log->vIp_Addr);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:SIM����(Old)      =     [%10.2f]", log->vSim_Fee_Old);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������(Old)       =     [%10.2f]", log->vMachine_Fee_Old);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������(Old)       =     [%10.2f]", log->vInnet_Fee_Old);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:ѡ�ŷ�(Old)       =     [%10.2f]", log->vChoice_Fee_Old);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:������(Old)       =     [%10.2f]", log->vHand_Fee_Old);
        pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:chgFlag           =         [%s]", ChgFlag);
		pubLog_Debug(_FFL_, "recordOprLog", "", "debug msg:custFlag           =        [%s]", CustFlag);		
        
                
        for(i=0; (i<MAXBUFRECORDS && strlen(log->logFee[i]->vFee_Code)>0); i++)
        {
              pubLog_Debug(_FFL_, "recordOprLog", "recordOprLog debug msg:�������", "[%d]     = [%s][%12.2f][%s]",
                   i, log->logFee[i]->vFee_Code, log->logFee[i]->vFee, log->logFee[i]->vFavour_Code);
        }

#endif


        memset(vSQL_Text,   0,   sizeof(vSQL_Text));
        memset(vTmp_String, 0,   sizeof(vTmp_String));
        memset(vBill_Year,  0,   sizeof(vBill_Year));
        memset(vBill_Month, 0,   sizeof(vBill_Month));
        memset(vReturn_Msg, 0,   sizeof(vReturn_Msg));
        memset(&max,        0,   sizeof(max));
        memset(&vErr_Count, 0,   sizeof(vErr_Count));
        memset(&vErr_Count2,0,   sizeof(vErr_Count2));
        memset(vOrg_Group,  0,   sizeof(vOrg_Group));

				
	
       
				 /*
         * ���ѽ��  =  �ֽ� + ֧Ʊ
         *
         *
         * Commented by lab 2006.09.21
         */

        /***************ҵ���߼����� modify by liuweib**********
        if (log->vPay_Money < log->vCash_Pay + log->vCheck_Pay)
        {
                strcpy(vReturn_Msg,"�����ܽ��С���������+����Ԥ��!");
                pubLog_Debug(_FFL_, "recordOprLog", "", "recordOprLog: �����ܽ��С���������+����Ԥ��!");

                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(rtrim(vReturn_Msg)));
                        retMsg[strlen(rtrim(vReturn_Msg))] = '\0';
                }
                return 1;
        }
        ****************ҵ���߼����� modify by liuweib***********/
        /*
         * ���ѽ��  =  ������õ���ϸ
         *
         *
         * Commented by lab 2006.09.21
         */

        

        strncpy(vBill_Year,  &(log->vOp_Time[0]), 4);
        vBill_Year[4]          = '\0';

        strncpy(vBill_Month, &(log->vOp_Time[4]), 2);
        vBill_Month[2]         = '\0';


#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "recordOprLog", "��ǰ��         =", "[%s]",vBill_Year);
        pubLog_Debug(_FFL_, "recordOprLog", "��ǰ��         =", "[%s]",vBill_Month);
#endif

/*
 * ��������־��wChgYYYYMM
 *
 * commented by lab 2005.1.24
 */
	if(strcmp(ChgFlag,"P")==0||strcmp(ChgFlag,"Y")==0)
	{	 


	/*	if (log->vCash_Pay + log->vCheck_Pay   != log->vSim_Fee    + log->vMachine_Fee + log->vInnet_Fee
                                                + log->vChoice_Fee + log->vHand_Fee    + log->vDeposit
                                                + log->vSim_Prepay + log->vOther_Fee)*/
    if(((log->vCash_Pay + log->vCheck_Pay) - (log->vSim_Fee + log->vMachine_Fee + log->vInnet_Fee + log->vChoice_Fee + log->vHand_Fee
																					 + log->vDeposit + log->vSim_Prepay + log->vOther_Fee) >= -0.0001) 
	&&((log->vCash_Pay + log->vCheck_Pay) - (log->vSim_Fee + log->vMachine_Fee + log->vInnet_Fee + log->vChoice_Fee + log->vHand_Fee
																					 + log->vDeposit + log->vSim_Prepay + log->vOther_Fee) <= 0.0001))
			{
				;
			}
			else
        {
                strcpy(vReturn_Msg,"�ֽ�+֧Ʊ != ��������ϸ��֮��!");
                pubLog_Debug(_FFL_, "recordOprLog", "", "recordOprLog: �ֽ�+֧Ʊ != �����ڸ�������ϸ��֮��!");

                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(rtrim(vReturn_Msg)));
                        retMsg[strlen(rtrim(vReturn_Msg))] = '\0';
                }
                return 10;
        }
		
     memset(vSQL_Text,   0,  sizeof(vSQL_Text));
     sprintf(vSQL_Text,"INSERT INTO wChg%s%s(",vBill_Year, vBill_Month);

     strcat(vSQL_Text,"Id_No,              Total_Date,         Login_Accept,");
     strcat(vSQL_Text,"Sm_Code,            Belong_Code,        Phone_No,");
     strcat(vSQL_Text,"Org_Code,           Login_No,           Op_Code,");
     strcat(vSQL_Text,"Op_Time,            Machine_Code,");
     strcat(vSQL_Text,"Cash_Pay,           Check_Pay,          Sim_Fee,");
     strcat(vSQL_Text,"Machine_Fee,        Innet_Fee,          Choice_Fee,");
     strcat(vSQL_Text,"Other_Fee,          Hand_Fee,           Deposit,");
     strcat(vSQL_Text,"Back_Flag,          Encrypt_Fee,        System_Note,");
     strcat(vSQL_Text,"Op_Note,            Group_Id,           Org_Id,Pay_Type)");
     strcat(vSQL_Text," VALUES( ");

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s %ld,  %d, %ld,",vTmp_String,log->vId_No,log->vTotal_Date,log->vLogin_Accept);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,log->vSm_Code,log->vBelong_Code,log->vPhone_No);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,log->vOrg_Code,log->vLogin_No,log->vOp_Code);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s To_Date('%s','YYYYMMDD HH24:MI:SS'), '%s',",vTmp_String,log->vOp_Time,log->vMachine_Code);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s %10.2f, %10.2f, %10.2f,",vTmp_String,log->vCash_Pay,log->vCheck_Pay,log->vSim_Fee);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s %10.2f, %10.2f, %10.2f,",vTmp_String,log->vMachine_Fee,log->vInnet_Fee,log->vChoice_Fee);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s %10.2f, %10.2f, %10.2f,",vTmp_String,log->vOther_Fee,log->vHand_Fee,log->vDeposit);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s '%s', 'UnKnown', '%s ',",vTmp_String,log->vBack_Flag,log->vSystem_Note);

     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s '%s', '%s', '%s',",vTmp_String,log->vOp_Note,log->vGroup_Id,log->vOrg_Id);

      /* 2010-11-25 POS������ֶ� */
     memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
     sprintf(vSQL_Text,"%s '%s')",vTmp_String,log->vPay_Type);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "recordOprLog", "recordOprLog: vSQL_Text=", "[%s]",vSQL_Text);
#endif

     memset(vSQL_Text,   0,  sizeof(vSQL_Text));
     sprintf(vSQL_Text,"INSERT INTO wChg%s%s(",vBill_Year, vBill_Month);

     strcat(vSQL_Text,"Id_No,              Total_Date,         Login_Accept,");
     strcat(vSQL_Text,"Sm_Code,            Belong_Code,        Phone_No,");
     strcat(vSQL_Text,"Org_Code,           Login_No,           Op_Code,");
     strcat(vSQL_Text,"Op_Time,            Machine_Code,");
     strcat(vSQL_Text,"Cash_Pay,           Check_Pay,          Sim_Fee,");
     strcat(vSQL_Text,"Machine_Fee,        Innet_Fee,          Choice_Fee,");
     strcat(vSQL_Text,"Other_Fee,          Hand_Fee,           Deposit,");
     strcat(vSQL_Text,"Back_Flag,          Encrypt_Fee,        System_Note,");
     strcat(vSQL_Text,"Op_Note,            Group_Id,           Org_Id ,pay_type)");
     strcat(vSQL_Text," VALUES( ");
     strcat(vSQL_Text,":v1,  :v2, :v3,");
     strcat(vSQL_Text,":v4,  :v5, :v6,");
     strcat(vSQL_Text,":v7,  :v8, :v9,");
     strcat(vSQL_Text,"To_Date(:v10,'YYYYMMDD HH24:MI:SS'), :v11,");
     strcat(vSQL_Text,":v12, :v13, :v14,");
     strcat(vSQL_Text,":v15, :v16, :v17,");
     strcat(vSQL_Text,":v18, :v19, :v20,");
     strcat(vSQL_Text,":v21, 'UnKnown', :v22,");
     strcat(vSQL_Text,":v23, :v24, :v25,:26)");

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "recordOprLog", "vSQL_Text=", "[%s]",vSQL_Text);
#endif
      EXEC SQL PREPARE Exec_wChgYYYYMM FROM :vSQL_Text; 
      EXEC SQL EXECUTE Exec_wChgYYYYMM using
              :log->vId_No,           :log->vTotal_Date,        :log->vLogin_Accept,
              :log->vSm_Code,         :log->vBelong_Code,       :log->vPhone_No,
              :log->vOrg_Code,        :log->vLogin_No,          :log->vOp_Code,
              :log->vOp_Time,         :log->vMachine_Code,
              :log->vCash_Pay,        :log->vCheck_Pay,         :log->vSim_Fee,
              :log->vMachine_Fee,     :log->vInnet_Fee,         :log->vChoice_Fee,
              :log->vOther_Fee,       :log->vHand_Fee,          :log->vDeposit,
              :log->vBack_Flag,       :log->vSystem_Note,
              :log->vOp_Note,         :log->vGroup_Id,          :log->vOrg_Id ,:log->vPay_Type;

			if (SQLCODE != OK)
      {
              sprintf(vReturn_Msg,"��������־��ʧ��[wChg%s%s],SQLCODE=[%d]",vBill_Year,vBill_Month,SQLCODE);
              pubLog_Debug(_FFL_, "recordOprLog", "", "recordOprLog: ��������־��ʧ��[wChg%s%s],SQLCODE=[%d][%s]!",vBill_Year,vBill_Month,SQLCODE,SQLERRMSG);
				printf("SQLCODE=[%d][%s]!\n",SQLCODE,SQLERRMSG);
              if (retMsg != NULL)
              {
                      strncpy(retMsg,  vReturn_Msg, strlen(rtrim(vReturn_Msg)));
                      retMsg[strlen(rtrim(vReturn_Msg))] = '\0';
              }
              return 3;
      }
	}

/*
 * ���������־��wLoginOprYYYYMM
 *
 * commented by lab 2005.1.24
*/
	if(strcmp(ChgFlag,"P")!=0)
	{
        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"INSERT INTO wLoginOpr%s%s(", vBill_Year, vBill_Month);
       
        strcat(vSQL_Text,"Total_Date,         Login_Accept,       Op_Code,");
        strcat(vSQL_Text,"Pay_Type,           Pay_Money,          Sm_Code,");
        strcat(vSQL_Text,"Id_No,              Phone_No,           Org_Code,");
        strcat(vSQL_Text,"Login_No,           Op_Time,            Op_Note,");
        strcat(vSQL_Text,"Ip_Addr,            Org_Id)");
        strcat(vSQL_Text," VALUES (");

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s %d,  %ld, '%s',",vTmp_String,log->vTotal_Date,log->vLogin_Accept,log->vOp_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', %10.2f, '%s',",vTmp_String,log->vPay_Type,log->vPay_Money,log->vSm_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s %ld, '%s', '%s',",vTmp_String,log->vId_No,log->vPhone_No,log->vOrg_Code);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', To_Date('%s','YYYYMMDD HH24:MI:SS'), '%s',",vTmp_String,log->vLogin_No,log->vOp_Time,log->vOp_Note);

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"%s '%s', '%s')",vTmp_String,log->vIp_Addr,log->vOrg_Id );

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "recordOprLog", "vSQL_Text=", "[%s]",vSQL_Text);
#endif

        memset(vTmp_String, 0,  sizeof(vTmp_String));        strcpy(vTmp_String, vSQL_Text);
        sprintf(vSQL_Text,"INSERT INTO wLoginOpr%s%s(", vBill_Year, vBill_Month);

        strcat(vSQL_Text,"Total_Date,         Login_Accept,   Op_Code,");
        strcat(vSQL_Text,"Pay_Type,           Pay_Money,      Sm_Code,");
        strcat(vSQL_Text,"Id_No,              Phone_No,       Org_Code,");
        strcat(vSQL_Text,"Login_No,           Op_Time,        Op_Note,");
        strcat(vSQL_Text,"Ip_Addr,            Org_Id)");
        strcat(vSQL_Text," VALUES (");
        strcat(vSQL_Text,":v1,  :v2, :v3,");
        strcat(vSQL_Text,":v4,  :v5, :v6,");
        strcat(vSQL_Text,":v7,  :v8, :v9,");
        strcat(vSQL_Text,":v10, To_Date(:v11,'YYYYMMDD HH24:MI:SS'), :v12,");
        strcat(vSQL_Text,":v13, :v14)");

        EXEC SQL PREPARE Exec_wLoginOprYYYYMM FROM :vSQL_Text;
        EXEC SQL EXECUTE Exec_wLoginOprYYYYMM using
                :log->vTotal_Date,        :log->vLogin_Accept,           :log->vOp_Code,
                :log->vPay_Type,          :log->vPay_Money,              :log->vSm_Code,
                :log->vId_No,             :log->vPhone_No,               :log->vOrg_Code,
                :log->vLogin_No,          :log->vOp_Time,                :log->vOp_Note,
                :log->vIp_Addr,           :log->vOrg_Id;

			  if (SQLCODE != OK)
        {
                sprintf(vReturn_Msg,"���������־��ʧ��[wLoginOpr%s%s],SQLCODE=[%d]",vBill_Year,vBill_Month,SQLCODE);
                pubLog_Debug(_FFL_, "recordOprLog", "", " ���������־��ʧ��[wLoginOpr%s%s],SQLCODE=[%d][%s]!",vBill_Year,vBill_Month,SQLCODE,SQLERRMSG);
                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
                        retMsg[strlen(vReturn_Msg)] = '\0';
                }
                return 4;
        }
	}  
/*
 *
 *  ����dCustAssure
 *	˵��������ID_NO��Op_Code���в�ѯ������Ѿ��м�¼����updateԭ��¼������insertһ���¼�¼
 *
 */
	if (strcmp(CustFlag,"Y")==0)
	{
		EXEC SQL SELECT rowid 
           	INTO    :vassureRowId 
          	FROM    dCustAssure 
           	WHERE   ID_NO   = to_number(:log->vId_No) 
           	AND   OP_CODE = :log->vOp_Code
           	AND    rownum < 2;
		/*��ѯ������Ϣ������ؼ�¼,����¼�¼*/
		if(SQLCODE == 0)
    	{   
	    	memset(dynStmt, 0, sizeof(dynStmt));
	    
  			sprintf(dynStmt, "UPDATE dCustAssure SET "
					 		"LOGIN_NO		= :v1,"
					 		"OP_TIME= to_date(:v2,'YYYYMMDD HH24:MI:SS'),"
                   	 		"NOTES			= :v3 "
                   	 	 "WHERE  rowid		= rtrim(:v4)" );
#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "recordOprLog", "", "dynStmt=[%s]", dynStmt);
#endif
				EXEC SQL PREPARE prepare2 From :dynStmt;
				EXEC SQL EXECUTE prepare2 using
										:log->vLogin_No,
										:log->vOp_Time,
										:log->vOp_Note,
										:vassureRowId;
    		if (SQLCODE !=0)
    		{
     	   		sprintf(retMsg,"�޸Ŀͻ�������Ϣ��dCustAssure����!SQLCODE:[%d]",SQLCODE);
				return 13 ;
    		}
    	}    	
    	/*��ѯ������Ϣ������ؼ�¼,������¼�¼*/
    	else if ( SQLCODE == 1403 || SQLCODE == 100)
    	{		
				memset(dynStmt, 0, sizeof(dynStmt));
				sprintf(dynStmt, " INSERT INTO dCustAssure "
							 " ( id_no,op_code, "
							 " login_no,op_time,notes)"
							 " Values "
			         " (:v1,:v2,:v3,to_date(:v4,'yyyymmdd hh24:mi:ss'),:v5)");
#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "recordOprLog", "", "dynStmt=[%s]", dynStmt);
#endif

				EXEC SQL PREPARE prepare1 From :dynStmt;
				EXEC SQL EXECUTE prepare1 using
									 	:log->vId_No,
									 	:log->vOp_Code,
										:log->vLogin_No,
										:log->vOp_Time,
										:log->vOp_Note;
    		if (SQLCODE !=0)
    		{
    	    	sprintf(retMsg,"����ͻ�������Ϣ��dCustAssure����!SQLCODE:[%d]",SQLCODE);
    	      	return 12 ;
    		}
    	}
    	/*��ѯ������Ϣ�����*/
		if(SQLCODE != 0 && SQLCODE != 1403 && SQLCODE != 100)
    	{	
        	sprintf(retMsg,"��ѯ�ͻ�������Ϣ��dCustAssureʧ��! SQLCODE:[%d]",SQLCODE);
        	return 11;
    	}
	}     
    
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "recordOprLog", "", "end of recordOprLog function!");
#endif
    	 return 0;
}
