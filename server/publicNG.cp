/**********************************************
 @wt PRG  : publicNG.cp
 @wt EDIT : dx
 @wt DATE : 2009-9-19
 @wt FUNC : ����ʹ�õ�һЩ������������ԭ�������������send_type��send_id�ֶΣ�
 			�Ա���ͬһ��ҵ���й�������һ���ԡ�
 *********************************************/


#include "boss_srv.h"
#include "publicsrv.h"
#include "contact.h"
#include "pubLog.h"


/*����ҵ�����ȡ����sSmProduct���sm_code*/
#define VPMN_SM_CODE           "01" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
#define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_IP_SM_CODE      "04" /*IPר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_DIANLU_SM_CODE  "13" /*��·����ҵ���Ʒ�ƴ���*/

#define _DEBUG_
EXEC SQL INCLUDE SQLCA;

/**
* �����ʻ�������Ϣ����
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   ����send_type char *send_type
* @inparam   ����send_id	   char *send_id
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
int createConMsg_ng(char *send_type,char *send_id,tAccDoc *Acc_Cust,char *retMsg)
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
	ret = OrderInsertConMsg(send_type,send_id,100,Acc_Cust->vOp_Code,atol(Acc_Cust->vLogin_Accept),Acc_Cust->vLogin_No,"createConMsg",vTdConMsg);
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
 *     ���ֳ��������������ʻ����жԽ�
* @inparam
* @inparam   ����send_type char *send_type
* @inparam   ����send_id	   char *send_id
 	sBackLoginAccept	������ˮ
 	iBackYearMonth		��������
 	sLoginAccept		������ˮ
 	sLoginNo			��������
 	sOpCode				��������
 	sOpTime				����ʱ��
 	iSysYearMonth		ϵͳ����

* @outparam
 	retCode
 	retMsg
 */
int MarkAwardBack_ng(char *send_type,char *send_id,const char *sBackLoginAccept, int iBackYearMonth, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	char    sSqlStr[2000+1];
	char 	table_name[40+1];

	double 	fAccCount=0.00;
	double  fChangeValue=0.00;
	double  available_mark=0.00;
	double  fOweMark=0.00;
	double  fPayedOweMark=0.00;
	double  fCurrentPoint=0.00;
	double  fPreDetailValue=0.00;
	double  fPreTotalValue=0.00;
	double  fAfterDetailValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fCurrentTotalValue=0.00;
	double 	temp1=0.00;
	double 	temp2=0.00;
	int    	iBillYm=0;
	int    	iOweYm=0;
	int    	iMinYm=0;
	char    sContractNo[14+1];
	char    sDetailCode[10+1];
	int    	iChangeSeq=0;
	long    iCurrMinYm=0;
	int    	iDetailMinYm=0;
	char    sBakeContractNo[14+1];
	int 	iFindFlag=0;        /*��ʾ�Ƿ��ҵ����ֶһ���¼*/
	char tmp_buf[2048];
	
	/*ng���� by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	/*ng���� by yaoxc end*/
	
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);
	/*ng���� by yaoxc end*/

	init(sContractNo);
	init(sDetailCode);
	init(sBakeContractNo);

	/*******����͸֧�ĳ���,����û�б����ϸֻ��͸֧********/
	/***********һ����ˮֻ��һ��͸֧��¼�����Բ����ж���������*************/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","***********���͸֧��û�б������������������������������************");
#endif

	/*����������ˮ�Ų��һ���͸֧��*/
	EXEC SQL	select owe_mark   ,owe_ym  ,to_char(contract_no),  payed_owe_mark
	              into :fOweMark, :iOweYm,:sContractNo, :fPayedOweMark
	              from dCustOweMark
	             where owe_accept=to_number(:sBackLoginAccept);

	if(SQLCODE==1403)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��Ҫ������͸֧��ʷ���в�ѯ[%d]",SQLCODE);
#endif

		sprintf(sSqlStr, "select owe_mark   ,  owe_ym  , to_char(contract_no),  payed_owe_mark"
		                 "  FROM dCustOweMarkHis%06d"
		                 " WHERE owe_accept=to_number(:v1) and back_flag='0' and rownum<2", iBackYearMonth);
		EXEC SQL	PREPARE preMarkAwardBack1 from :sSqlStr;
		EXEC SQL	DECLARE preMarkAwardBackCur cursor for preMarkAwardBack1;
		EXEC SQL	OPEN	preMarkAwardBackCur USING :sBackLoginAccept;
			
		printf("11111111[%d][%s]\n",SQLCODE, sSqlStr);
		EXEC SQL    fetch 	preMarkAwardBackCur INTO :fOweMark, :iOweYm,:sContractNo, :fPayedOweMark;
		
		if (SQLCODE == 1403)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","����͸֧��ʷ������͸֧��Ϣ[%d]", SQLCODE);
#endif
		}
		else if(SQLCODE == 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","����͸֧��ʷ������͸֧��Ϣ[%d]!",SQLCODE);
#endif
		}
		else
		{
			strcpy(retCode,"376926");
			sprintf(retMsg,"��ѯ����͸֧��ʷ��ʱ��������[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE preMarkAwardBackCur;
			return -1;
		}
		EXEC SQL CLOSE preMarkAwardBackCur;
	}
	else if(SQLCODE!=0)
	{
		strcpy(retCode,"376919");
		strcpy(retMsg,"��ѯ����͸֧��ʱ��������\n");
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�ڻ���͸֧�����ҵ���¼\n");
#endif
	}

    Trim(sContractNo);

	if (fPayedOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�û�͸֧�Ļ����Ѿ��������������������ٳ���!\n");
#endif
		strcpy(retCode,"376919");
		strcpy(retMsg,"�û�͸֧�Ļ����Ѿ��������������������ٳ���!\n");
		return -1;
	}
	else if (fOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�޸Ļ���͸֧��!\n");
#endif
		/*������ϣ�����¼�Ƶ�����͸֧��ʷ��*/
		init(tmp_buf);
		strcpy(tmp_buf, "insert into dCustOweMarkHis");
		sprintf(tmp_buf,"%s%d",tmp_buf,iOweYm);
		strcat(tmp_buf, "(Owe_Accept, contract_no, Owe_ym, owe_mark, payed_owe_mark,");
		strcat(tmp_buf, "Finish_Flag, login_no, op_time, op_code, back_flag) ");
		strcat(tmp_buf, "select Owe_Accept, contract_no, Owe_ym, owe_mark, owe_mark, ");
		strcat(tmp_buf, "'0', login_no, op_time, op_code, '1' ");
		strcat(tmp_buf, "from dCustOweMark where Owe_Accept=to_number(:iOweAccept)");

		EXEC SQL PREPARE pre_dCustOweMarkHisYM from :tmp_buf;
		EXEC SQL EXECUTE pre_dCustOweMarkHisYM using :sBackLoginAccept;
		if (SQLCODE != 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","tmp_buf:%s\n sBackLoginAccept:%s\n", tmp_buf, sBackLoginAccept);
#endif
			sprintf(retCode, "000011");
			sprintf(retMsg, "������ϣ�����¼�Ƶ�����͸֧��ʷ��ʧ�ܣ�[SQLCODE:%d]", SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

		EXEC SQL delete from dCustOweMark where Owe_Accept=to_number(:sBackLoginAccept);
		if (SQLCODE != 0)
		{
			strcpy(retCode,"3769395");
			sprintf(retMsg,"ɾ������͸֧���¼����[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}

	init(sSqlStr);
	sprintf(sSqlStr, "select to_char(contract_no), to_char(detail_code), bill_ym,"
					 "       change_seq, pre_detail_value, pre_total_value,"
					 "       abs(change_value), after_detail_value, after_total_value"
					 "  from dMarkChangeDetail%d"
					 " where op_accept = to_number(:sBackLoginAccept)"
					 "   and changetype = '1'"
					 "   and back_flag = '0'"
					 " order by bill_ym desc",
					 iBackYearMonth);

	/*���ֱ任����Ϊ1����������ǻ��ֶһ�*/
	EXEC SQL PREPARE preMarkAwardBack2 from :sSqlStr;
	EXEC SQL DECLARE curMarkAwardBack2 CURSOR FOR preMarkAwardBack2;
	EXEC SQL OPEN    curMarkAwardBack2 using :sBackLoginAccept;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","OPEN SQLCODE :%d\n", SQLCODE);
#endif
	for(;;)
	{
		EXEC SQL FETCH   curMarkAwardBack2 INTO :sContractNo, :sDetailCode, :iBillYm,
												:iChangeSeq, :fPreDetailValue, :fPreTotalValue,
												:fChangeValue, :fAfterDetailValue, :fAfterTotalValue;
		if (SQLCODE == 1403)
		{
			break;
		}
		else if (SQLCODE != 0)
		{
			strcpy(retCode,"376935");
			sprintf(retMsg,"ȡ�ʻ�������Ϣ����[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		iFindFlag = 1;

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��õĸ�ϸ������Ļ��ֱ仯Ϊ%lf",fChangeValue);
		pubLog_Debug(_FFL_,"","","��ǰ�ʻ�Ϊ%s,�����ʻ�Ϊ[%s]",sContractNo,sBakeContractNo);
#endif
		if (strcmp(sBakeContractNo, sContractNo) != 0)
		{
			if(sBakeContractNo[0] != '\0')
			{
				pubLog_Debug(_FFL_,"","","���ʻ��ҽ���������\n");
				strcpy(retCode,"376915");
				sprintf(retMsg,"���ʻ��ҽ���������");
				EXEC SQL CLOSE curMarkAwardBack2;
				return -1;
			}

			EXEC SQL	select current_point,min_ym
			              into :fCurrentPoint,:iMinYm
			              from dconmark
			             where contract_no=to_number(:sContractNo);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"376916");
				sprintf(retMsg,"���ʻ����ֱ��в�ѯ�ʻ��ܻ��ֳ���[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkAwardBack2;
				return -1;
			}
			else
			{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","��ѯ�����ʻ��ܻ���Ϊ%lf,������ֱ������%d",fCurrentPoint,iMinYm);
#endif
			}

			iCurrMinYm=iMinYm;
			init(sBakeContractNo);
			strcpy(sBakeContractNo,sContractNo);
			fAccCount=0;               /*��ʱ�ʻ����*/
			fCurrentTotalValue=fCurrentPoint;
		}
		else
		{
			/*��ʾcontract_no��sBakeContractNo��ͬ���Ѿ�������ˡ�*/
		}

		/*��ѯ��ǰ�����ʻ����ֱ����ϸ���У�ͬ������ˮ���ʻ�id,�������ֿ�Ŀ�������ʻ������������ʻ����ֱ�����*/
		pubLog_Debug(_FFL_,"","","��ѯ��ǰ����%d�ʻ����ֱ����ϸ�������Ļ��ֱ�����",iSysYearMonth);
		Trim(sContractNo);
		Trim(sDetailCode);

		/*�ж���С����*/
		if (iMinYm > iBillYm)
		{
			iCurrMinYm = iBillYm;
		}
		/***lula����*******/
		iMinYm=iCurrMinYm;
		/******lula����****/
		init(table_name);
		sprintf(table_name,"dMarkChangeDetail%6d",iSysYearMonth);
		pubLog_Debug(_FFL_,"","","����Ϊ[%s],[%s],[%s],[%d]\n",sLoginAccept,sContractNo,sDetailCode,iBillYm);
		sprintf(sSqlStr,
			" select nvl(max(change_seq),0) "
			" from %s  where op_accept=to_number(:v1) "
			" and   contract_no =to_number(:v2) "
			" and   Detail_Code =to_number(:v3) "
			" and   bill_ym     = :v4 ",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack3 from :sSqlStr;
		EXEC SQL DECLARE curMarkAwardBack3 CURSOR for preMarkAwardBack3;
		EXEC SQL OPEN    curMarkAwardBack3 using :sLoginAccept,:sContractNo,:sDetailCode,:iBillYm;;
		EXEC SQL fetch curMarkAwardBack3 into :iChangeSeq;

		if(SQLCODE==1403)
		{
			iChangeSeq=1;
			pubLog_Debug(_FFL_,"","","û���ҵ��������±�����,��ʼ��ֵ���ó�[1]");
			EXEC SQL CLOSE curMarkAwardBack3;
		}
		else if(SQLCODE==0)
		{
			iChangeSeq=iChangeSeq+1;
			EXEC SQL CLOSE curMarkAwardBack3;
			pubLog_Debug(_FFL_,"","","��ǰ���õı�����Ϊ%d",iChangeSeq);
		}
		else
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","���ұ�����ʱ����������[%d]",SQLCODE);
#endif
			sprintf(retMsg,"��ѯ�����ֱ�����[%d]\n",SQLCODE);
			EXEC SQL CLOSE curMarkAwardBack3;
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��ǰϵͳʱ���µĻ��ֱ����ϸ���в��뵱ǰ�����Ľ��");
#endif
		/*******lula�޸�********/
		temp1=fAfterDetailValue+fChangeValue;
		temp2=fCurrentTotalValue+fChangeValue;
		/*******lula�޸�********/
		/*fCurrentTotalValue=temp2;*/
		init(table_name);
		sprintf(table_name,"dMarkChangeDetail%6d",iSysYearMonth);
		sprintf(sSqlStr,
			"insert into %s  "
			"(op_accept,contract_no,Detail_Code,Bill_Ym,change_seq,"
			"ChangeType,pre_detail_value,pre_total_value,change_value,"
			"after_detail_value,after_total_value,"
			"login_no,op_time,back_flag,op_code)"
			"values(to_number(:v1),to_number(:v2),to_number(:v3),:v4,:v5,"
			"'1',:v6,:v7,:v8,"
			":v9,:v10,"
			":v11,to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),'1',:sOpCode)",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack4 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack4 using
			:sLoginAccept,:sContractNo,:sDetailCode,:iBillYm,:iChangeSeq,
			:fAfterDetailValue,:fCurrentTotalValue,:fChangeValue,
			:temp1,:temp2,:sLoginNo, :sOpTime, :sOpCode;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"����ֱ����ϸ���в������ʱ��������[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}
		fCurrentTotalValue=temp2;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","����ֱ����ϸ���в������ݳɹ�\n");
#endif
		fAccCount+=fChangeValue;

		pubLog_Debug(_FFL_,"","","contract_no=%s;sDetailCode=%s\n",sContractNo,sDetailCode);
		pubLog_Debug(_FFL_,"","","����%d���µ��ʻ����ֿ�Ŀ��ϸ��\n",iBillYm);
		init(table_name);
		sprintf(table_name,"dConMarkPre%6d",iBillYm);
		sprintf(sSqlStr,
			" update %s "
			" set available_mark=available_mark+:v1, "
			" op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS') "
			" where  contract_no=to_number(:v2)   "
			" and    detail_code=to_number(:v3) ",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack5 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack5 using :fChangeValue,:sOpTime,:sContractNo,:sDetailCode;
		if(SQLCODE==0)
		{
			pubLog_Debug(_FFL_,"","","�ɹ����ʻ���Ŀ��ϸ�������ֵ\n");
		}
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"�����ʻ���Ŀ��ϸ����û���ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		pubLog_Debug(_FFL_,"","","ѡ����ֿ�Ŀ���¸ö�����Ŀ�µ���С����\n");
		EXEC SQL	select min_ym
		              into :iDetailMinYm
		              from dConMarkDetail
		             where contract_no = to_number(:sContractNo)
		               and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"ѡ����ֿ�Ŀ���¸ö�����Ŀ�µ���С����ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		if( iDetailMinYm>iBillYm )
		{
			iDetailMinYm = iBillYm;
		}

		pubLog_Debug(_FFL_,"","","���»��ֿ�Ŀ��\n");
		EXEC SQL	update dConMarkDetail
		               set min_ym=:iDetailMinYm,
		                   max_ym=:iSysYearMonth,
		                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
		                   available_point=available_point+:fChangeValue
		             where contract_no = to_number(:sContractNo)
			           and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"���»��ֿ�Ŀ��ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}
	}
	EXEC SQL CLOSE curMarkAwardBack2;
	/*�α�رգ��������ʻ��µ�����д���ʻ����ֱ���*/

	if (sBakeContractNo[0] == '\0')
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","���ʻ����ֱ����ϸ����û���ҵ���ǰ��ˮ\n");
#endif
		/*
		strcpy(retCode,"376918");
		strcpy(retMsg,"�ʻ����ֱ����ϸ����û���ҵ���ǰ��ˮ");
		return -1;
		*/
	}
	else
	{
/*****
fAccCount�Ǵ�dMarkChangeDetail��ȡ�ģ�������͸֧���֣�fOweMarkΪ͸֧����
*****/	/*ng����
		EXEC SQL	update dConMark
		               set current_point = current_point+:fAccCount,
		                   min_ym        = :iCurrMinYm,
		                   month_used    = month_used-:fAccCount,
		                   year_used     = year_used-:fAccCount,
		                   total_used    = total_used-:fAccCount,
		                   owe_point     = owe_point - :fOweMark,
		                   current_time  = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		             where contract_no   = :sBakeContractNo;

		if(SQLCODE!=0)
		{
			strcpy(retCode,"376917");
			sprintf(retMsg,"�����ʻ����ֱ����[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng����*/
    	/*ng���� by yaoxc begin*/
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fAccCount);
		sprintf(v_parameter_array[1],"%ld",iCurrMinYm);
		sprintf(v_parameter_array[2],"%f",fAccCount);
		sprintf(v_parameter_array[3],"%f",fAccCount);
		sprintf(v_parameter_array[4],"%f",fAccCount);
		sprintf(v_parameter_array[5],"%f",fOweMark);
		strcpy(v_parameter_array[6],sOpTime);
		strcpy(v_parameter_array[7],sBakeContractNo);
    	
		v_ret=OrderUpdateConMark(send_type,send_id,100,
							  	sOpCode,atol(sLoginAccept),sLoginNo,"MarkAwardBack",
								sBakeContractNo,
								" current_point = current_point+:fAccCount,min_ym = :iCurrMinYm,month_used = month_used-:fAccCount,year_used = year_used-:fAccCount,total_used = total_used-:fAccCount,owe_point = owe_point - :fOweMark,current_time = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"376917");
			sprintf(retMsg,"�����ʻ����ֱ����[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng���� by yaoxc end*/
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","���ۻ�����Ļ���%lf�ӵ����ʻ�%s�Ļ��ֱ��У��ɹ�\n",fAccCount,sBakeContractNo);
#endif

		sprintf(sSqlStr,"update dMarkChangeDetail%6d "
						"   set back_flag='1'"
						" where op_accept=to_number(:op_accept)   ",
						iBackYearMonth);
		EXEC SQL PREPARE preMarkAwardBack6 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack6 using :sBackLoginAccept;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376918");
			sprintf(retMsg,"�����ʻ���Ŀ��ϸ����û���ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}
	return 0;
}


/**���ְ��ʻ����ʻ��»��ֿ�Ŀ�ۼ�����
 * @inparam iDeductType �������ͣ� '4'���ֵ�����'5'ת����'7'���ֿ۳�

 	lContractNo			�����ʻ�
 	sDetailCodeIn		���ֶ�����Ŀ  ����Ŀ�ۼ�ʱ����ΪNULL
 	sChangeType			�������
 	fTotalCount			�ۼ�����
 	lLoginAccept		������ˮ
 	sLoginNo			  ��������
 	sOpCode				  ��������
 	sOpTime				  ����ʱ��
 	iSysYearMonth		ϵͳʱ��

 * @outparam
 	retCode				���ش���
 	retMsg				������Ϣ

 */
int MarkDeduct_ng(char *send_type,char *send_id,long lContractNo, const char* sDetailCodeIn, const char* sChangeType,
		double fTotalCount, long lLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	double  fCurrentPoint=0.00;
	double  fOweLimit=0.00;
	double  fOwePoint=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainMark=0.00;
	char 	sDetailCode[10+1];       /*number10*/
	long    change_seq=0;
	int    	iCurrentYearMonth=0;
	long    iMinYearMonth=0;
	double  fPreMark=0.00;                 /*ÿ�δ��α�ʱ�����ֵ*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];

	/*ng���� by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];
	/*ng���� by yaoxc end*/
	
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);
	init(sContractNo);
	/*ng���� by yaoxc end*/

	init(sContractStatus);
	init(sDetailCode);
	init(sTableName);

	/*�����ʻ����ҵ�ǰ���֣� ͸֧���ֶ�ȣ���͸֧���֣��Ѿ����ѻ��֣��ʻ�״̬,������������*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_point, owe_limit, owe_point, total_used, to_char(contract_status), min_ym
	              into :fCurrentPoint,:fOweLimit,:fOwePoint,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConMark
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125058");
		sprintf(retMsg,"��ѯ�ʻ�������Ϣ����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfCurrentPoint=%lf\n",fCurrentPoint);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfOweLimit=%lf\n",fOweLimit);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfOwePoint=%lf\n",fOwePoint);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢiMinYearMonth=%d\n",iMinYearMonth);
#endif
		fPreTotalValue=fCurrentPoint;
	}

	if (sDetailCodeIn != NULL)
	{
		EXEC SQL	select Available_Point, min_ym
		              into :fCurrentPoint,:iMinYearMonth
		              from dConMarkDetail
		             where contract_no = :lContractNo
		               and detail_code=to_number(:sDetailCodeIn);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125059");
			sprintf(retMsg,"��ѯ�ʻ����ֿ�Ŀ���ִ���[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		if (fCurrentPoint < fTotalCount)
		{
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�ʻ����ֿ�Ŀ�ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentPoint,fTotalCount);
	#endif
			strcpy(retCode,"1000");
			sprintf(retMsg,"�ʻ����ֿ�Ŀ�ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentPoint,fTotalCount);
			return -1;
		}
	}
	else if (fCurrentPoint < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�ʻ����ֵĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentPoint,fTotalCount);
#endif
		strcpy(retCode,"1000");
		sprintf(retMsg,"�ʻ����ֵĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentPoint,fTotalCount);
		return -1;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"125060");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�ʻ�����,����ִ�в���");
#endif
		sprintf(retMsg,"�ʻ�����,����ִ�в���");
		return -1;
	}

	if(fOwePoint>0.00)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","���˻��Ѿ�����͸֧����ȷ��");
#endif
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************���ֶһ���ʼ**********************\n");
#endif
	while((iCurrentYearMonth<=iSysYearMonth) && (fTotalCountRemain > 0))
	{
		init(sTableName);
		sprintf(sTableName,"dConMarkPre%6d\n",iCurrentYearMonth);
		if (sDetailCodeIn == NULL)
		{
			sprintf(sSqlStr,
				"select     a.available_mark,to_char(b.detail_code) "
				" from      %s a,sMarkCodeDetail b "
				" where     a.contract_no=:v1 "
				" and       a.detail_code=b.detail_code "
				" order by  b.DEDUCT_ORDER ",
				sTableName);
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��ǰ������%s,��ʱ��%d,ϵͳʱ��Ϊ[%d] sSqlStr[%s]\n",
				sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
	#endif
		}
		else
		{
			sprintf(sSqlStr,
				"select available_mark,detail_code "
				"  from %s  "
				" where contract_no=:v1 "
				"   and detail_code=:sDetailCodeIn " ,
				sTableName);
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��ǰ������%s,��ʱ��%d,ϵͳʱ��Ϊ[%d] sSqlStr[%s]\n",
				sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
	#endif
		}
		EXEC SQL PREPARE preMarkDeduct1 from :sSqlStr;
		EXEC SQL DECLARE curMarkDeduct1 CURSOR for preMarkDeduct1;
		if (sDetailCodeIn == NULL)
		{
			pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
			EXEC SQL OPEN curMarkDeduct1 using :lContractNo;
		}
		else
		{
			EXEC SQL OPEN curMarkDeduct1 using :lContractNo,:sDetailCodeIn;
		}
		while(fTotalCountRemain > 0)/*�α�ѡȡ��ȷ*/
		{
			EXEC SQL fetch curMarkDeduct1 into :fRemainMark,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","ȡ��̬���%s����,���ڵ�ǰ��ȡ�գ����������%d\n",sTableName,SQLCODE);
				break;
			}
			Trim(sDetailCode);
			if(fRemainMark==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��%s��ȡ�õ�fRemainMark=%lf sDetailCode=%s\n",sTableName,fRemainMark,sDetailCode);
#endif
			fPreMark=fRemainMark;                /*��¼�˴��α�λ�õĻ���*/
			if(fRemainMark>=fTotalCountRemain)         /*�α���ִ��ڵ�ǰ������Ҫ�Ļ���*/
			{
				fRemainMark=fRemainMark-fTotalCountRemain;
				fTotalCountRemain = 0;
			}
			else
			{
				fTotalCountRemain = fTotalCountRemain - fRemainMark;
				fRemainMark=0;
			}
			fChangeValue=fPreMark-fRemainMark;
			fAfterTotalValue=fPreTotalValue-fChangeValue;

			init(sTableName);
			sprintf(sTableName,"dConMarkPre%6d",iCurrentYearMonth);
			sprintf(sSqlStr, "update %s"
			                 "   set available_mark=:v1,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=:v2"
			                 "   and detail_code=to_number(:v3)",sTableName);
			EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
			EXEC SQL EXECUTE s3760_first02 using  :fRemainMark,:sOpTime, :lContractNo,:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125061");
				sprintf(retMsg,"�����ʻ����ֿ�Ŀ��ϸ��ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�α���ָ��³ɹ�\n");
			pubLog_Debug(_FFL_,"","","��ǰ�α�λ��ʣ��Ļ�����ֵΪ%lf\n",fRemainMark);
			pubLog_Debug(_FFL_,"","","���α����ѵĻ�������Ϊ%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","��ѯ��ǰ����%d�ʻ����ֱ����ϸ�������Ļ��ֱ�����\n",iSysYearMonth);
#endif

			init(sTableName);
			sprintf(sTableName,"dMarkChangeDetail%6d",iSysYearMonth);
			sprintf(sSqlStr,
				"select nvl(max(change_seq),0) "
				"  from %s"
				" where op_accept=:v1"
				"   and   contract_no =:v2 "
				"   and   detail_code =to_number(:v3) "
				"   and   bill_ym     = :v4 ",
				sTableName);
		/*	EXEC SQL PREPARE s3760_first03 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first03 into :change_seq
				using :lLoginAccept,:lContractNo,:sDetailCode,:iCurrentYearMonth;*/
			EXEC SQL PREPARE s3799_first04 from :sSqlStr;
			EXEC SQL DECLARE curMarkDeduct2 CURSOR for s3799_first04;
			EXEC SQL OPEN curMarkDeduct2 using :lLoginAccept, :lContractNo, :sDetailCode, :iCurrentYearMonth;
			EXEC SQL FETCH curMarkDeduct2 INTO :change_seq;
	
			if(SQLCODE==1403)
			{
				change_seq=1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","û���ҵ����ֱ����ϸ�������,��ʼ��ֵ���ó�[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","���û��ֱ����ϸ����֮�����Ϊ%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"125062");
				sprintf(retMsg,"��ѯ�����ֱ����Ŵ���[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct2;
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			EXEC SQL CLOSE curMarkDeduct2;
			pubLog_Debug(_FFL_,"","","��������Ϣ���뵽�ʻ����ֱ����ϸ����dMarkChangeDetail\n");
			sprintf(sSqlStr,
				"insert into %s  "
				"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
				"ChangeType,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
				"After_Detail_Value,After_Total_Value,"
				"login_no        ,op_time,back_flag,op_code)"
				"values(:v1,:v2,to_number(:v3),:v4,:v5,"
				":ChangeType,:v6,:v7,-1*:v8,"
				":v9,:v10,"
				"nvl(:v11,'test'),sysdate,'0',:op_code)",
				sTableName);

			EXEC SQL PREPARE s3760_first04 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first04 using
				:lLoginAccept,    :lContractNo,:sDetailCode,:iCurrentYearMonth,:change_seq,
				:sChangeType,     :fPreMark        ,:fPreTotalValue         ,:fChangeValue,
				:fRemainMark,     :fAfterTotalValue         ,:sLoginNo, :sOpCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125063");
				sprintf(retMsg,"����ֱ����ϸ���в������ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","����ֱ����ϸ���в������ݳɹ�");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�����ʻ����ֿ�Ŀ��");
#endif
			EXEC SQL	update dConMarkDetail
			               set available_point=available_point-:fChangeValue,
			                   min_ym=:iCurrentYearMonth,
			                   max_ym=:iSysYearMonth,
			                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
			             where contract_no=:lContractNo
			               and detail_code=to_number(:sDetailCode);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125064");
				sprintf(retMsg,"�����ʻ����ֿ�Ŀ��ʧ��[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�α��,���׽��������ʻ����ֿ�Ŀ��ɹ�");
			pubLog_Debug(_FFL_,"","","���׽���,ֱ����ת���ر��α�");
#endif
		}
		EXEC SQL CLOSE curMarkDeduct1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","����ѭ��������ȡ��һ�ű�������Ϊ%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while��������������.û��͸֧����2.����͸֧����");
	pubLog_Debug(_FFL_,"","","��ǰ����Ϊ%d",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","��ǰ����Ϊ%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","�����ʻ���ǰ�ļ��,total_point=%f\n",fTotalCount);
#endif
	if(sChangeType[0]=='9')/*���������ת�������ֿۼ�ʱ�����ۼ��»��ֺ�����ֺ��ۼǻ���*/
	{
		if(strcmp(sOpCode,"2418")==0)
		{	/*ng����
			EXEC SQL  update dConMark
	    	             set current_point   = current_point-:fTotalCount,
	    	             	 Month_point=Month_point-:fTotalCount,
							 year_point=year_point-:fTotalCount, 
							 add_point=add_point-:fTotalCount,*/
	    	                 /*min_ym          = :iCurrentYearMonth,*/
	    	                /* current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	    	           where contract_no=:lContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
				return -1;
			}
			ng����*/
    		/*ng���� by yaoxc begin*/
    		sprintf(sContractNo,"%ld",lContractNo);

    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",fTotalCount);
			sprintf(v_parameter_array[1],"%f",fTotalCount);
			sprintf(v_parameter_array[2],"%f",fTotalCount);
			sprintf(v_parameter_array[3],"%f",fTotalCount);
			strcpy(v_parameter_array[4],sOpTime);
			strcpy(v_parameter_array[5],sContractNo);
    		
			v_ret=OrderUpdateConMark(send_type,send_id,100,
								  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
									sContractNo,
									"current_point   = current_point-:v0,Month_point=Month_point-:v1,year_point=year_point-:v2,add_point=add_point-:v3,current_time    = to_date(:v4, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark1 ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
				return -1;
			}
    		/*ng���� by yaoxc end*/	
	 	}
	 	else
	 	{
			if(strcmp(sOpCode,"2419")==0)
			{	/*ng����
				EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
								 add_point=add_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
				ng����*/
    			/*ng���� by yaoxc begin*/
    			sprintf(sContractNo,"%ld",lContractNo);
    			init(v_parameter_array);
					
				sprintf(v_parameter_array[0],"%f",fTotalCount);
				sprintf(v_parameter_array[1],"%f",fTotalCount);
				sprintf(v_parameter_array[2],"%d",iCurrentYearMonth);
				strcpy(v_parameter_array[3],sOpTime);
				strcpy(v_parameter_array[4],sContractNo);
    			
				v_ret=OrderUpdateConMark(send_type,send_id,100,
									  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
										sContractNo,
										" current_point = current_point-:v0,add_point=add_point-:v1,min_ym = :v2,current_time = to_date(:v3, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
				printf("OrderUpdateConMark2 ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng���� by yaoxc end*/
		 	}	 
		 	else{
		 		/*ng����		
		 		EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
				ng����*/
    			/*ng���� by yaoxc begin*/
    			sprintf(sContractNo,"%ld",lContractNo);
    			init(v_parameter_array);
					
				sprintf(v_parameter_array[0],"%f",fTotalCount);
				sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
				strcpy(v_parameter_array[2],sOpTime);
				strcpy(v_parameter_array[3],sContractNo);
    			
				v_ret=OrderUpdateConMark(send_type,send_id,100,
									  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
										sContractNo,
										" current_point = current_point-:v0,min_ym = :v1,current_time = to_date(:v2, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
				printf("OrderUpdateConMark3 ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "�����ʻ����ֱ�(dConMark)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng���� by yaoxc end*/
			}
	 	}
	}
	else if(sChangeType[0]=='8')/*�����������ֵ���ʱ���ۼ����ֱ��ʱ�ۼ��»��ֺ�����ֺ��ۼǻ���*/
	{	/*ng����
		EXEC SQL  update dConMark
	                 set current_point   = current_point-:fTotalCount,
	                     min_ym          = :iCurrentYearMonth,
	                     Month_point     = Month_point-:fTotalCount,
	                     year_point      = year_point-:fTotalCount,
	                     add_point       = add_point-:fTotalCount,
	                     current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	               where contract_no=:lContractNo;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"��������������ʻ����ֱ����ݳ���[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng����*/
    	/*ng���� by yaoxc begin*/
    	sprintf(sContractNo,"%ld",lContractNo);
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
		sprintf(v_parameter_array[2],"%f",fTotalCount);
		sprintf(v_parameter_array[3],"%f",fTotalCount);
		sprintf(v_parameter_array[4],"%f",fTotalCount);
		strcpy(v_parameter_array[5],sOpTime);
		strcpy(v_parameter_array[6],sContractNo);
    	
		v_ret=OrderUpdateConMark(send_type,send_id,100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
								sContractNo,
								" current_point = current_point-:v0,min_ym = :v1,Month_point = Month_point-:v2,year_point = year_point-:v3,add_point = add_point-:v4,current_time = to_date(:v5, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark4 ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"��������������ʻ����ֱ����ݳ���[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng���� by yaoxc end*/

	}
	else/*��������ۻ���ʱ�����ۼ��»��ֺ�����ֺ��ۼǻ��֡���¼ʹ�û���*/
	{	/*ng����
		EXEC SQL	update dConMark
	               set current_point   =   round(current_point-:fTotalCount,2),
	                   min_ym          =   :iCurrentYearMonth,*/
	                  /* month_used      =   month_used+:fTotalCount,
	                   total_used      =   total_used+:fTotalCount,
	                   year_used       = year_used+:fTotalCount,*/
	                  /* current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	             where contract_no=:lContractNo;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"��������������ʻ����ֱ����ݳ���[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng����*/
    	/*ng���� by yaoxc begin*/
    	sprintf(sContractNo,"%ld",lContractNo);
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
		strcpy(v_parameter_array[2],sOpTime);
		strcpy(v_parameter_array[3],sContractNo);
    	
		v_ret=OrderUpdateConMark(send_type,send_id,100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
								sContractNo,
								" current_point = round(current_point-:fTotalCount,2),min_ym = :iCurrentYearMonth,current_time = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark5 ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"��������������ʻ����ֱ����ݳ���[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng���� by yaoxc end*/
	}
	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","��������������ʻ��������ݳɹ�fTotalCountRemain=%f",fTotalCountRemain);
#endif

	if(fTotalCountRemain > 0.005)/*��������͸֧*/
	{
		strcpy(retCode,"125066");
		sprintf(retMsg,"��������͸֧����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
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
int createCustMsg_ng(char *send_type,char *send_id,tUserDoc *doc,char *retMsg)
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

	ret = OrderInsertCustMsg(send_type,send_id,100,doc->vOp_Code,doc->vLogin_Accept,doc->vLogin_No,"createCustMsg",vTdCustMsg);
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
int createCustDoc_ng(char *send_type,char *send_id,tCustDoc *doc,char *retMsg)
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
		ret = OrderInsertCustDoc(send_type,send_id,100,doc->vOp_Code,doc->vLogin_Accept,doc->vLogin_No,"createCustDoc",vTdCustDoc);
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

/************************************************************************
��������:fpubAddGrpMember_ng
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û����ӳ�Ա���ú���
�������:�����û�id
         ��Ա�ֻ�����
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע
         ���󷵻���Ϣ(��Ϊ���ز���ʹ��)
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubAddGrpMember_ng(char *send_type,char *send_id,char *inGrpId_No, char *phone_no, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{
    EXEC SQL BEGIN DECLARE SECTION;
         int  vCnt = 0;
         int  mCount = 0;
         int  vProduct_Level = 0;
         char vGrpCust_Id[22+1];
         char vGrpId_No[22+1];
         char vPhone_No[22+1];
         char vCust_Id[10+1];
         char vId_No[14+1];
         char vSm_Code[2+1];
         char vUnit_Id[10+1];
         char vUnit_Code[10+1];
         char vGrp_Userno[15+1];
         char vProduct_Type[8+1];
         char vProduct_Code[8+1];
         char vService_Code[4+1];
         char vService_No[6+1];
         char vBill_Code[8+1];
         char vGroup_Type_Code[1+1];
         char vDepartment[36+1];
         char vLmtFee[14+1];
         char vShort_PhoneNo[11+1];

         long lLoginAccept = 0;
         char vLogin_No[6+1];
         char vOp_Code[4+1];
         char vTotal_Date[8+1];
         char vEnd_Date[8+1];
         char vOp_Note[60+1];
         int  ret;
         char vLimitFlag[1+1];
         char vRun_Time[17+1];        /*add by miaoyl at 20090629 ����ʱ��*/
         char vOrg_Id[10+1];          /*add by miaoyl at 20090708  */
         
         /*NG��ź*/
         int	v_ret=0;
         char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
         TdGrpUserMebMsg	tdGrpUserMebMsg;
         char	BeginTime1[20+1];
         char	IdNo2[22+1];
         char	sTempSqlStr[1024+1];
         int    icount=0;
    EXEC SQL END DECLARE SECTION;

    /*������ʼ��*/
    init(vGrpCust_Id      );
    init(vGrpId_No        );
    init(vPhone_No        );
    init(vId_No           );
    init(vCust_Id         );
    init(vSm_Code         );
    init(vUnit_Id         );
    init(vUnit_Code       );
    init(vGrp_Userno      );
    init(vProduct_Type    );
    init(vProduct_Code    );
    init(vService_Code    );
    init(vService_No      );
    init(vBill_Code       );
    init(vGroup_Type_Code );
    init(vDepartment      );
    init(vLmtFee          );
    init(vShort_PhoneNo   );
    init(vLogin_No        );
    init(vOp_Code         );
    init(vTotal_Date      );
    init(vEnd_Date        );
    init(vOp_Note         );
    init(vLimitFlag       );
    init(vRun_Time        );    /*add by miaoyl at 20090629 ����ʱ��*/ 
    init(vOrg_Id          );    /*add by miaoyl at 20090708  */
          

    /*������ֵ*/
    lLoginAccept = lSysAccept;
    strcpy(vGrpId_No,   inGrpId_No);
    strcpy(vPhone_No,   phone_no  );
    strcpy(vLogin_No,   login_no  );
    strcpy(vOp_Code,    op_code   );
    strcpy(vTotal_Date, total_date);
    strcpy(vOp_Note,    op_note   );

    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
    /*ȡ����ORG_ID*/
    ret = 0;
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
	    sprintf(return_message,"��ѯorg_idʧ��![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190100;
	}
	Trim(vOrg_Id);
   /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/
       
    /*��ѯ�����û�����*/
    EXEC SQL SELECT Cust_Id, User_No, Product_Type, Product_Code, To_Char(Bill_Date, 'YYYYMMDD')
      INTO :vGrpCust_Id, :vGrp_Userno, :vProduct_Type, :vProduct_Code, :vEnd_Date
      FROM dGrpUserMsg
      WHERE ID_NO = To_Number(:vGrpId_No) AND Bill_Date > Last_Day(sysdate) + 1;
    if (SQLCODE !=0) {
        sprintf(return_message,"��ѯdGrpUserMsg����[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190103;
    }
    Trim(vGrpCust_Id);
    Trim(vGrp_Userno);
    Trim(vProduct_Type);
    Trim(vProduct_Code);

    /*��ѯ�ֻ��ͻ�����*/
    Trim(vPhone_No);
    EXEC SQL SELECT id_no, cust_id, sm_code, to_char(bill_type)
       INTO :vId_No, :vCust_Id, :vSm_Code, :vBill_Code
       FROM dCustMsg WHERE phone_no = :vPhone_No;
    if (SQLCODE != 0) {
        sprintf(return_message,"��ѯdCustMsg�����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190101;
    }
    Trim(vBill_Code);

    /* ���Ƴ�Ա��μ���ͬһ���� */
    mCount = 0;
    EXEC SQL SELECT COUNT(*) INTO :mCount
      FROM dGrpUserMebMsg
      WHERE id_no = to_number(:vGrpId_No) and member_id = to_number(:vId_No) 
      	and end_time > last_day(sysdate) + 1;
    if (mCount > 0) {
        sprintf(return_message, "�ֻ�����[%s]�Ѿ����뼯��[%s]!", vPhone_No, vGrpId_No);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190102;
    }

    EXEC SQL select limit_flag into :vLimitFlag 
    	from sProductMebLimit where product_code = :vProduct_Code;
   	if (SQLCODE != 0 && SQLCODE != 1403) {
   		sprintf(return_message, "��ѯsProductMebLimit�����[%d]", SQLCODE);
   		return 190131;
   	}
    
    if (SQLCODE == 1403) {
    	/* yangzh add 2005/04/14 �̶����벻�ܼ���ͬһƷ�Ƶļ��ų�Ա begin */    	
      EXEC SQL SELECT COUNT(*) INTO :mCount
        FROM dGrpUserMebMsg a, dGrpUserMsg b
        WHERE a.id_no = b.id_no and a.member_id = to_number(:vId_No)
          and b.product_type = :vProduct_Type and b.bill_date > last_day(sysdate) + 1
          and a.end_time > last_day(sysdate) + 1;
      if (SQLCODE != 0) {
          sprintf(return_message,"�жϹ̶������Ƿ��Ѿ�����ͬһƷ�Ʋ�Ʒ����[%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190131;
      }
      if (mCount > 0) {
          sprintf(return_message, "������ͬ���Ʒ");
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190132;
      }    	
    	/* yangzh add 2005/04/14 �̶����벻�ܼ���ͬһƷ�Ƶļ����û� end */
    } else {
    	ret = fCheckProdMebLimit_ng(vGrpId_No, vId_No, vProduct_Code, vLimitFlag, return_message);
    	if (ret != 0) {
    		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
    		return 190131;
    	}
    }
    /*��ѯ���ſͻ�����*/
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*  
    EXEC SQL SELECT to_char(unit_id), trim(unit_code), service_no
      INTO :vUnit_Id, :vUnit_Code, :vService_No
      FROM dCustDocOrgAdd
      WHERE cust_id = to_number(:vGrpCust_Id);
    */ 
    EXEC SQL SELECT to_char(unit_id), trim(unit_id), service_no
      INTO :vUnit_Id, :vUnit_Code, :vService_No
      FROM dCustDocOrgAdd
      WHERE cust_id = to_number(:vGrpCust_Id);  
    if (SQLCODE != 0) {
        sprintf(return_message,"��ѯdCustDocOrgAdd����[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190104;
    }
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

    /*�ж��Ƿ�VPMN���ſͻ�*/
    if (0 == strcmp(vProduct_Type, VPMN_SM_CODE)) {
      strcpy(vGroup_Type_Code, "1");
    } else {
      strcpy(vGroup_Type_Code, "2");
    }

    /*��ѯdGrpMemberMsg��ǰ��Ա�Ƿ��Ѿ�����*/
    EXEC SQL SELECT COUNT(*) INTO :mCount
               FROM dGrpMemberMsg
              WHERE Cust_Id = To_Number(:vGrpCust_Id)
                AND Id_No = To_Number(:vId_No);
    if (SQLCODE !=0){
        sprintf(return_message,"��ѯdGrpMemberMsg��[%s]��Ա��������[%d]", vId_No, SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190105;
    }
    if (mCount == 0) { /*���ſͻ���Ա��������*/
        /*���û���¼���浽dGrpMemberMsgHis��
          big_flag --��ͻ���ʶ���ȸ��³ɲ���('0'�ǣ�'1'��)
unit_id,                                           vrecord.unit_id,                                   
unit_code,                                         TO_CHAR(vrecord.unit_id),                          
id_no,                                             v_id_no,                                           
big_flag,                                          '0',--��ͻ���ʶ���ȸ��³ɲ���('0'�ǣ�'1'��)       
phone_no,                                          TRIM(vrecord.real_no),                             
NAME,                                              NVL(SUBSTRB(TRIM(vrecord.username),1,20),'δ֪'),  
user_name,                                         NVL(SUBSTRB(TRIM(vrecord.username),1,20),'δ֪'),  
male_flag,                                         '2',                                               
belong_code,                                       '*',                                               
service_no,                                        'work99',                                          
card_code,                                         '99',                                              
member_code,                                       'C1',                                              
id_iccid,                                          SUBSTRB(vrecord.iccid,1,20),                       
dept,                                              SUBSTRB(vrecord.department,1,20),                  
ingrp_date,                                        TO_CHAR(vrecord.op_time,'yyyy-mm-dd'),--����ʱ��   
service_access,                                    '*',                                               
sm_code,                                           '*',                                               
pay_code,                                          '99',--���ѷ�ʽ��'99'δ֪                          
bill_code,                                         '*',                                               
pay_fee_flag,                                      '*',                                               
payed_fee,                                         vrecord.lmtfee,                                    
short_phoneno,                                     TO_CHAR(vrecord.short_no),                         
note,                                              TRIM(vrecord.note),                                
cust_id,                                           TO_NUMBER(vrecord.real_no),--�ͻ���ţ���д���ֻ���
load_date,                                         TO_CHAR(SYSDATE,'yyyy-mm-dd'),--�������           
group_type_code,                                   '1',                                               
gmember_status_code,                               '1',--������������Ϊvpmn,��Ա״̬Ϊ��Ч            
pay_fee_flag, payed_fee,
dept, substr(department,1,20),
        */
    /* LINA VIP2.0 2006_11_05 begin here*/
    /*ȡvip��֯��dgrpgroups��org_code*/ 
    
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/     
    /*EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    	char vOrgId[10+1];
    EXEC SQL END DECLARE SECTION;
    init(vOrgId);*/
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/
    
    /*��ѯ���ſͻ���Ϣ*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "��ѯ���ſͻ���Ϣ");
    #endif
    /*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vGrpCust_Id)
                AND Unit_Id = To_Number(:vUnit_Id);
        if (SQLCODE != 0){
            sprintf(return_message,"��Org_Codeʧ��",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190106;
        }
    */
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/   
      
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "LINA");
 
    	/*��֯�������������ֶ�group_id��org_id  edit by zhengxg at 07/05/2009*/
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    
        /*ȡ����ʱ��*/
        Trim(vId_No);
        EXEC SQL select to_char(open_time,'yyyymmdd hh24:mi:ss') INTO :vRun_Time 
                   from dCustMsg
                  where id_no =To_Number(:vId_No);
       if (SQLCODE != 0)
        {
            sprintf(return_message,"��ѯdCustMsg����ʱ��open_time�����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190124;
        }
            
    	/*
        EXEC SQL INSERT INTO dGrpMemberMsgHis
                 (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,                                    
                  PHONE_NO, NAME, USER_NAME, BELONG_CODE,                                    
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,                                    
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,                                    
                  big_flag, male_flag,                                    
                  card_code, member_code,                                    
                  ingrp_date, service_access, sm_code,                                    
                  pay_code, bill_code, load_date,                                    
                  group_type_code, gmember_status_code,                                    
                  pay_fee_flag, payed_fee, short_phoneno,                                    
                  service_no, dept, note,                                    
                  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,                                    
                  UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, Org_Code, GROUP_ID)                                   
           SELECT :vGrpCust_Id, :vUnit_Id, :vUnit_Code,:vId_No, :vId_No,
                  :vPhone_No, cust_name, cust_name, region_code||district_code||town_code,
                  Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
                  CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
                  '0', '2',
                  '99', 'C1',
                  To_Char(sysdate, 'yyyy-mm-dd'), '*', :vSm_Code,
                  '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
                  :vGroup_Type_Code, '1',
                  '*', 0, '0',
                  :vService_No, 'δ֪', :vOp_Note,
                  :vLogin_No, :lLoginAccept, :vTotal_Date,
                  sysdate, :vOp_Code, 'A', :sIn_Grp_OrgCode, GROUP_ID
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);
            */
            
            PUBLOG_DEBUG( "1", "","vGrpCust_Id       =[%s]", vGrpCust_Id);
            PUBLOG_DEBUG( "1", "","vUnit_Id       =[%s]", vUnit_Id);
            PUBLOG_DEBUG( "1", "","vId_No       =[%s]", vId_No);
            PUBLOG_DEBUG( "1", "","vPhone_No       =[%s]", vPhone_No);
            PUBLOG_DEBUG( "1", "","vCust_Id       =[%s]", vCust_Id);
            PUBLOG_DEBUG( "1", "","vSm_Code       =[%s]", vSm_Code);
            PUBLOG_DEBUG( "1", "","vService_No       =[%s]", vService_No);
            PUBLOG_DEBUG( "1", "","vOp_Note       =[%s]", vOp_Note);
            PUBLOG_DEBUG( "1", "","vRun_Time       =[%s]", vRun_Time);
            PUBLOG_DEBUG( "1", "","vOrg_Id       =[%s]", vOrg_Id);
            PUBLOG_DEBUG( "1", "","vCust_Id       =[%s]", vCust_Id);
      		PUBLOG_DEBUG( "1", "","vOp_Code       =[%s]", vOp_Code);
      		PUBLOG_DEBUG( "1", "","vTotal_Date       =[%s]", vTotal_Date);
      		PUBLOG_DEBUG( "1", "","lLoginAccept       =[%ld]", lLoginAccept);
      		PUBLOG_DEBUG( "1", "","vLogin_No       =[%s]", vLogin_No);
            
            EXEC SQL INSERT INTO dGrpMemberMsgHis
                 (CUST_ID, UNIT_ID, ID_NO,                                    
                  PHONE_NO, USER_NAME, BELONG_CODE,                                    
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,                                    
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,                                    
                  big_flag,                                    
                  card_code, member_code,                                    
                  ingrp_date, service_access, sm_code,                                    
                  service_no, dept, note,                                    
                  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,                                    
                  UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, GROUP_ID,
                  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
                  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
                  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
                  correct_note,ORG_ID)
            SELECT :vGrpCust_Id, :vUnit_Id, :vId_No,
                  :vPhone_No, cust_name, region_code||district_code||town_code,
                  Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
                  CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
                  '0',
                  '99', 'C1',
                  sysdate, '*', :vSm_Code,
                  :vService_No, 'δ֪', :vOp_Note,
                  :vLogin_No, :lLoginAccept, :vTotal_Date,
                  sysdate, :vOp_Code, 'A', GROUP_ID,
                  '','','','',
                  '','','','',    
                  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',    
                  '',:vOrg_Id            
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);

        if (SQLCODE != 0){
            sprintf(return_message,"��ѯdCustDoc��,����dGrpMemberMsgHis��Ա���Ϸ����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190106;
        }
        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
        
    	/*��֯�������������ֶ�group_id��org_id  edit by zhengxg at 07/05/2009*/
    	
        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
       /*    	
        EXEC SQL INSERT INTO dGrpMemberMsg
                 (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,
                  PHONE_NO, NAME, USER_NAME, BELONG_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  big_flag, male_flag,
                  card_code, member_code,
                  ingrp_date, service_access, sm_code,
                  pay_code, bill_code, load_date,
                  group_type_code, gmember_status_code,
                  pay_fee_flag, payed_fee, short_phoneno,
                  service_no, dept, note, Org_Code, GROUP_ID)
           SELECT :vGrpCust_Id, :vUnit_Id, :vUnit_Code, :vId_No, :vId_No,
                  :vPhone_No, cust_name, cust_name, region_code||district_code||town_code,
                  Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
                  CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
                  '0', '2',
                  '99', 'C1',
                  To_Char(sysdate, 'yyyy-mm-dd'), '*', :vSm_Code,
                  '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
                  :vGroup_Type_Code, '1',
                  '*', 0, '0',
                  :vService_No, 'δ֪', :vOp_Note, :sIn_Grp_OrgCode, GROUP_ID
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);
*/
        
        EXEC SQL INSERT INTO dGrpMemberMsg
                 (CUST_ID, UNIT_ID, ID_NO,
                  PHONE_NO, USER_NAME, BELONG_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  big_flag,
                  card_code, member_code,
                  ingrp_date, service_access, sm_code,
                  service_no, dept, note, GROUP_ID,
                  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
                  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
                  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
                  correct_note,ORG_ID)
           SELECT :vGrpCust_Id, :vUnit_Id, :vId_No,
                  :vPhone_No, cust_name, region_code||district_code||town_code,
                  Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
                  CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
                  '0',
                  '99', 'C1',
                  sysdate, '*', :vSm_Code,
                  :vService_No, 'δ֪', :vOp_Note, GROUP_ID,
                  '','','','',
                  '','','','',    
                  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',    
                  '',:vOrg_Id           
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);

        if (SQLCODE != 0){
            sprintf(return_message,"��ѯdCustDoc��,����dGrpMemberMsg��Ա���Ϸ����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190107;
        }
        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
        
    /* LINA VIP2.0 end here*/
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "�ж��Ƿ�VPMN���ų�Ա�����¼��ſͻ���Ա��VPMN������Ϣ");
    #endif
    
    EXEC SQL SELECT Count(*) INTO :vCnt
               FROM dVPMNUsrMsg
              WHERE Real_No like :vPhone_No||'%';
    if (SQLCODE != 0){
        sprintf(return_message,"��ѯdVPMNUsrMsg�����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190108;
    }
    if (1 == vCnt) { /*�������ų�Ա����VPMN���ų�Ա,���Եõ�VPMN������Ϣ*/

        EXEC SQL SELECT substrb(nvl(department,'δ֪'), 1, 20), To_Char(lmtfee), short_no
                   INTO :vDepartment, :vLmtFee, :vShort_PhoneNo
                   FROM dVPMNUsrMsg
                  WHERE Real_No like :vPhone_No||'%';
        if (SQLCODE != 0){
            sprintf(return_message,"��ѯdVPMNUsrMsg���û���Ϣ�����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190109;
        }
        Trim(vDepartment);
        Trim(vLmtFee);
        Trim(vShort_PhoneNo);

        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
        /*
        EXEC SQL UPDATE dGrpMemberMsg
                    SET dept = substrb(nvl(:vDepartment, 'δ֪' ), 1, 20),
                        payed_fee = To_Number(:vLmtFee),
                        short_phoneno = :vShort_PhoneNo
                  WHERE Cust_Id = :vGrpCust_Id
                    AND Phone_No = :vPhone_No;
        */
      EXEC SQL UPDATE dGrpMemberMsg
        SET dept = substrb(nvl(:vDepartment, 'δ֪' ), 1, 20)
      WHERE Cust_Id = :vGrpCust_Id
        AND Phone_No = :vPhone_No;             
        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/   
                     
        if (SQLCODE != 0){
            sprintf(return_message,"����dGrpMemberMsg���û���Ϣ�����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190109;
        }
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "���뼯���û���Ա��");
    #endif
    EXEC SQL INSERT INTO dGrpUserMebMsghis
         ( ID_NO,        MEMBER_ID,   MEMBER_NO,   BAK_FIELD,
           BEGIN_TIME,   END_TIME,
           UPDATE_LOGIN, UPDATE_DATE, UPDATE_TIME, UPDATE_ACCEPT,
           UPDATE_CODE,  UPDATE_TYPE)
        VALUES
         ( :vGrpId_No,   :vId_No,     :vPhone_No,  :vPhone_No,
           sysdate,      To_Date(:vEnd_Date, 'YYYYMMDD'),
           :vLogin_No,   :vTotal_Date,sysdate,     :lLoginAccept,
           :vOp_Code, 'A');
    if (SQLCODE != 0){
        sprintf(return_message,"����dGrpUserMebMsghis�����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190108;
    }
    /*NG��ź
    EXEC SQL INSERT INTO dGrpUserMebMsg
         ( ID_NO,        MEMBER_ID,   MEMBER_NO,   BAK_FIELD,
           BEGIN_TIME,   END_TIME )
        VALUES
         ( :vGrpId_No,    :vId_No,     :vPhone_No, :vPhone_No,
           sysdate, To_Date(:vEnd_Date, 'YYYYMMDD') );
    if (SQLCODE != 0){
        sprintf(return_message,"����dGrpUserMebMsg�����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190109;
    }
	NG��ź*/
    /*ng����  by magang at 20090812 begin*/
    pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMebMsg!\n");  
			init(BeginTime1);
    		  EXEC SQL select 	to_char(sysdate,'YYYYMMDD HH24:MI:SS') 
    		  			into :BeginTime1
    		  		    from  dual;
    		  	if (SQLCODE != 0)
    		  	{
    		  		sprintf(return_message,"��ȡʱ��ʧ��[%d]",SQLCODE);
    		  		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
       			 	return 192109;
    		  	}		
        		Trim(BeginTime1);
        		Trim(vGrpId_No);
        		Trim(vId_No);
        		Trim(vPhone_No);
        		Trim(vEnd_Date);
   				memset(&tdGrpUserMebMsg,0,sizeof(TdGrpUserMebMsg));
				strcpy(tdGrpUserMebMsg.sIdNo,vGrpId_No);    		
    			strcpy(tdGrpUserMebMsg.sMemberId,vId_No); 
    			strcpy(tdGrpUserMebMsg.sMemberNo,vPhone_No); 
    			strcpy(tdGrpUserMebMsg.sBeginTime,BeginTime1); 
    			strcpy(tdGrpUserMebMsg.sEndTime,vEnd_Date); 
    			strcpy(tdGrpUserMebMsg.sBakField,vPhone_No); 
    		v_ret=0;
			v_ret=OrderInsertGrpUserMebMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,tdGrpUserMebMsg);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dGrpUserMebMsg�����쳣[%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        		return 190109;
			}

    /*ng����  by magang at 20090812 end*/
    /*yangzh add 2005-01-24 �����û����뼯��ʱ����attr_codeΪ���ſͻ���־*/
    /*NG��ź
    EXEC SQL UPDATE dcustmsg
       SET attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)
     WHERE phone_no = :vPhone_No;
    if (SQLCODE != 0){
        sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190109;
    }
	NG��ź*/
    /*ng����  by magang at 20090812 begin*/
    	memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
    	icount=0;
		sprintf(sTempSqlStr,"select to_char(id_no)  from dcustmsg where phone_no = :vPhone_No");		 
	 	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
        EXEC SQL declare ngcursor3 cursor for sql_str;
        EXEC SQL open ngcursor3 using :vPhone_No;
    	for(;;)	
     	{   
     		init(IdNo2);
     		EXEC SQL FETCH ngcursor3 into :IdNo2;    			
     		if((0!=SQLCODE)&&(1403!=SQLCODE))
        	{
        		PUBLOG_DBDEBUG("fpubDelGrpUser");
        		pubLog_DBErr(_FFL_,"","","��ȡid_noʧ�� [%s]--\n",vPhone_No);
        		EXEC SQL CLOSE ngcursor3;
				return 191207;
        	}
        	if(1403==SQLCODE) break;
        	Trim(IdNo2);				
    	 	pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateCustMsg!\n");    	 
			init(i_parameter_array);
			strcpy(i_parameter_array[0],IdNo2);
			v_ret=0;
			v_ret=OrderUpdateCustMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,IdNo2,"attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)","",i_parameter_array);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        		EXEC SQL CLOSE ngcursor3;
        		return 190109;
			}
			icount++;
		}
		EXEC SQL CLOSE ngcursor3;
		if(icount==0)
		{
			sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
        	pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 290109;
		}	
 	/*ng����  by magang at 20090804 END*/
/*���ݲ�Ʒ��ѯ������Ϣ��һ����Ʒ���Զ�Ӧ������� chenxuan boss3
    EXEC SQL DECLARE prod_cur CURSOR for
             select a.product_level, b.service_code
               from sProductCode a, sProductSrv b
              where a.product_code = b.product_code
                and a.product_code = :vProduct_Code
                and a.product_status = 'Y'
                and a.begin_time < sysdate
                and a.end_time > sysdate;
    if(SQLCODE!=OK){
          sprintf(return_message, "��ѯ��Ʒ������Ӧ��ϵ����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190021;
    }

    EXEC SQL OPEN prod_cur;
    if(SQLCODE!=OK){
          EXEC SQL CLOSE prod_cur;
          sprintf(return_message, "�򿪲�Ʒ������Ӧ��ϵ�α����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190022;
    }

    EXEC SQL FETCH prod_cur INTO :vProduct_Level, :vService_Code;
    while(SQLCODE == 0)
    {
        ���뼯���û���Ա��Ʒ������,GRP���
        EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
             ( ID_NO,         USER_NO,      MEMBER_ID,     MEMBER_NO,
               PRODUCT_TYPE,  PRODUCT_CODE, PRODUCT_LEVEL, SERVICE_CODE,
               BEGIN_TIME,    END_TIME,
               BAK_FIELD,     UPDATE_LOGIN, UPDATE_DATE,   UPDATE_TIME,
               UPDATE_ACCEPT, UPDATE_CODE,  UPDATE_TYPE)
            VALUES
             ( :vGrpId_No,      :vGrp_Userno,   :vId_No,       :vPhone_No,
               :vProduct_Type,  :vProduct_Code, :vProduct_Level, :vService_Code,
               sysdate,         To_Date(:vEnd_Date, 'YYYYMMDD'),
               '0',             :vLogin_No,     :vTotal_Date,    sysdate,
               :lLoginAccept, :vOp_Code, 'A');
        if (SQLCODE != 0){
            sprintf(return_message,"����dGrpUserMebProdMsgHis�����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            EXEC SQL CLOSE prod_cur;
            return 190111;
        }
        EXEC SQL INSERT INTO dGrpUserMebProdMsg
             ( ID_NO,         USER_NO,      MEMBER_ID,     MEMBER_NO,
               PRODUCT_TYPE,  PRODUCT_CODE, PRODUCT_LEVEL, SERVICE_CODE,
               BEGIN_TIME,    END_TIME,
               BAK_FIELD)
            VALUES
             ( :vGrpId_No,      :vGrp_Userno,  :vId_No,    :vPhone_No,
               :vProduct_Type,  :vProduct_Code, :vProduct_Level, :vService_Code,
               sysdate,         To_Date(:vEnd_Date, 'YYYYMMDD'),
               '0');
        if (SQLCODE != 0){
            sprintf(return_message,"����dGrpUserMebProdMsg�����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            EXEC SQL CLOSE prod_cur;
            return 190112;
        }

        EXEC SQL FETCH prod_cur INTO :vProduct_Level, :vService_Code;
    }
    EXEC SQL CLOSE prod_cur;
*/
    return 0;
}

/************************************************************************
��������:IPfpubCreateGrpUser_ng
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û��������ú���(8100ר��ʹ��)
�������:�����û���Ϣ
         ��Ա�ֻ�����
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע
�������:�������
�� �� ֵ:0������ȷ����,��������
**************************************************************************/
int IPfpubCreateGrpUser_ng(char *send_type,char *send_id,tGrpUserMsg *grpUserMsg, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note)
{   
	
	   int ret =0;
     tAccDoc  accDoc ;
    EXEC SQL BEGIN DECLARE SECTION;
         char    vCust_Id[14+1];        /* ���ſͻ�ID       */
         char    vGrp_Id[14+1];         /* �����û�ID       */
         char    vGrp_No[15+1];         /* �û����         */
         char    vIds[4+1];             /* �ʻ���           */
         char    vGrp_Name[60+1];       /* �û�����         */
         char    vProduct_Type[8+1];    /* ��Ʒ����         */
         char    vProduct_Code[8+1];    /* ��Ʒ����         */
         char    vOperation_Code[2+1];  /* GRP����ҵ������  */
         char    vGrpProduct_Code[2+1]; /* GRP���ŵĲ�Ʒ����*/
         char    vUser_Passwd[8+1];     /* ҵ����������     */
         char    vLogin_Name[20+1];     /* �Է����û���     */
         char    vLogin_Passwd[20+1];   /* �Է�������       */
         char    vProvince[2+1];        /* ����ʡ           */
         char    vRegion_Code[2+1];     /* ��������         */
         char    vDistrict_Code[2+1];   /* ������           */
         char    vTown_Code[3+1];       /* ����Ӫҵ��       */
         char    vTerritory_Code[20+1]; /* ���ش���         */
         char    vLimit_Owe[18+1];      /* ͸֧���         */
         char    vCredit_Code[2+1];     /* ���õȼ�         */
         char    vCredit_Value[18+1];   /* ���ö�           */
         char    vRun_Code[1+1];        /* �û�״̬         */
         char    vOld_Run[1+1];         /* ǰһ״̬         */
         char    vRun_Time[20+1];       /* ״̬�仯ʱ��     */
         char    vBill_Date[20+1];      /* ����ʱ��         */
         char    vBill_Type[4+1];       /* �������ڴ���     */
         char    vLast_Bill_Type[1+1];  /* �ϴγ�������     */
         char    vOp_Time[20+1];        /* ����ʱ��         */
         char    vSrv_Start[10+1];      /* ҵ����ʼ����     */
         char    vSrv_Stop[10+1];       /* ҵ���������     */
         char    vBak_Field[16+1];      /* Ԥ���ֶ�         */
				 char    vSm_Code[2+1]; 
				 char     vOrgId[10+1];

         int   i, inx, vCount = 0;
         int   vPower_right = 0;
         int   vFav_Order = 0;
         long  lLoginAccept;
         char  vOrg_Code[7+1];
         char  vLogin_No[6+1];
         char  vOp_Code[4+1];
         char  vOp_Note[60+1];
         char  vTotal_Date[8+1];       /*��������     */
         char  vDateChar[6+1];         /*��������     */
         char  vUnit_Id[22+1];         /*����ID       */
         char  vUnit_Code[22+1];       /*���Ŵ���     */
         char  vAccount_Id[14+1];      /*�ʻ�ID       */
         char  vAccount_Passwd[8+1];   /*�ʻ�����     */
         char  vContract_Id[22+1];     /*���ź�ͬID   */
         char  vContract_No[22+1];     /*���ź�ͬ���� */
         char  vProduct_Id[22+1];      /*��ƷID       */
         char  vOrder_GoodsId[22+1];   /*����ID       */
         char  vProduct_List[1536];    /*��Ʒ�����б� */
         char  vProduct_All[1536];     /*���Ӳ�Ʒ�б� */
         char  sqlStr[3072];
         char  vService_Type[1+1];
         char  vService_Code[4+1];
         char  vMain_Flag[1+1];
         char  vPrice_Code[4+1];
         char  vVPMN_Flag[1+1];
         char  vDisCode1[7+1];
         char  vBelong_Code[7+1];
         char  vDisCode2[7+1];
         char    vPay_Code[1+1];          /*�����ʻ����ѷ�ʽ */
         
          /*NG��ź*/
         TdConUserMsg	tdConUserMsg;
         char	TotalDate1[20+1];
         char	SrvStop1[20+1];
          char	BeginTime1[20+1];
         TdCustConMsg	sTdCustConMsg;
         int	v_ret=0;        
         TdGrpUserMsg	sTdGrpUserMsg;
    EXEC SQL END DECLARE SECTION;
    
    memset(&accDoc, 0, sizeof(accDoc));
    init(vOrg_Code        );
    init(vLogin_No        );
    init(vOrgId);
    init(vOp_Code         );
    init(vOp_Note         );
    init(vTotal_Date      );
    init(vDateChar        );
    init(vUnit_Id         );
    init(vUnit_Code       );
    init(vContract_Id     );
    init(vContract_No     );
    init(vProduct_Id      );
    init(vOrder_GoodsId   );
    init(vService_Type    );
    init(vSm_Code);
    init(vService_Code    );
    init(vMain_Flag       );
    init(vPrice_Code      );

    init(vCust_Id         );
    init(vGrp_Id          );
    init(vAccount_Id      );
    init(vAccount_Passwd  );
    init(vGrp_No          );
    init(vIds             );
    init(vGrp_Name        );
    init(vProduct_Type    );
    init(vProduct_Code    );
    init(vUser_Passwd     );
    init(vLogin_Name      );
    init(vLogin_Passwd    );
    init(vProvince        );
    init(vRegion_Code     );
    init(vDistrict_Code   );
    init(vTown_Code       );
    init(vTerritory_Code  );
    init(vLimit_Owe       );
    init(vCredit_Code     );
    init(vCredit_Value    );
    init(vRun_Code        );
    init(vOld_Run         );
    init(vRun_Time        );
    init(vBill_Date       );
    init(vBill_Type       );
    init(vLast_Bill_Type  );
    init(vOp_Time         );
    init(vSrv_Start       );
    init(vSrv_Stop        );
    init(vBak_Field       );
    init(vVPMN_Flag       );
    init(vOperation_Code  );
    init(vGrpProduct_Code );
    init(vProduct_List    );
    init(vProduct_All     );
    init(sqlStr           );
    init(vDisCode1        );
    init(vDisCode2        );
    init(vPay_Code        );
    init(vBelong_Code);

    lLoginAccept = lSysAccept;
    strcpy(vLogin_No, login_no);
    strcpy(vOp_Code,  op_code);
    strcpy(vOp_Note,  op_note);
    strcpy(vTotal_Date, total_date);
    strncpy(vDateChar,  total_date, 6);
    strcpy(vOrg_Code,       grpUserMsg->org_code);
    strcpy(vCust_Id,        grpUserMsg->cust_id);
    strcpy(vGrp_Id,         grpUserMsg->id_no);
    strcpy(vGrp_No,         grpUserMsg->user_no);
    strcpy(vIds,            grpUserMsg->ids);
    strcpy(vGrp_Name,       grpUserMsg->user_name);
    strcpy(vAccount_Id,     grpUserMsg->account_no);
    strcpy(vAccount_Passwd, grpUserMsg->account_passwd);
    /*��sSmCode����,GRP���ŵĲ�Ʒ��������Ϊ�˲�ƷƷ��*/
    strcpy(vGrpProduct_Code,grpUserMsg->product_type);
    strcpy(vProduct_Type,   grpUserMsg->product_type);
    strcpy(vProduct_Code,   grpUserMsg->product_code);
    strcpy(vUser_Passwd,    grpUserMsg->user_passwd);
    strcpy(vLogin_Name,     grpUserMsg->login_name);
    strcpy(vLogin_Passwd,   grpUserMsg->login_passwd);
    strcpy(vProvince,       grpUserMsg->prov_code);
    strcpy(vRegion_Code,    grpUserMsg->region_code);
    strcpy(vDistrict_Code,  grpUserMsg->district_code);
    strcpy(vTown_Code,      grpUserMsg->town_code);
    strcpy(vTerritory_Code, grpUserMsg->territory_code);
    strcpy(vLimit_Owe,      grpUserMsg->limit_owe);
    strcpy(vCredit_Code,    grpUserMsg->credit_code);
    strcpy(vCredit_Value,   grpUserMsg->credit_value);
    strcpy(vRun_Code,       grpUserMsg->run_code);
    strcpy(vOld_Run,        grpUserMsg->old_run);
    strcpy(vRun_Time,       grpUserMsg->run_time);
    strcpy(vBill_Date,      grpUserMsg->bill_date);
    strcpy(vBill_Type,      grpUserMsg->bill_type);
    strcpy(vLast_Bill_Type, grpUserMsg->last_bill_type);
    strcpy(vOp_Time,        grpUserMsg->op_time);
    strcpy(vSrv_Start,      grpUserMsg->srv_start);
    strcpy(vSrv_Stop,       grpUserMsg->srv_stop);
    strcpy(vSm_Code,        grpUserMsg->sm_code);
    strcpy(vBak_Field,      grpUserMsg->bak_field);
    strcpy(vPay_Code,       grpUserMsg->pay_code);
    strcpy(vOrgId,          grpUserMsg->org_id);
    strcpy(vBelong_Code,    grpUserMsg->belong_code);
    Trim(vBelong_Code);
    Trim(vOrg_Code);
    Trim(vProduct_Type);
    Trim(vProduct_Code);

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "lLoginAccept[%ld]",    lLoginAccept   );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vLogin_No[%s]",       vLogin_No      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vOp_Code[%s]",        vOp_Code       );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vOp_Note[%s]",        vOp_Note       );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vTotal_Date[%s]",     vTotal_Date    );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vDateChar[%s]",       vDateChar      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vOrg_Code[%s]",       vOrg_Code      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vCust_Id[%s]",        vCust_Id       );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vGrp_Id[%s]",         vGrp_Id        );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vGrp_No[%s]",         vGrp_No        );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vIds[%s]",            vIds           );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vGrp_Name[%s]",       vGrp_Name      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vProduct_Type[%s]",   vProduct_Type  );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vProduct_Code[%s]",   vProduct_Code  );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vUser_Passwd[%s]",    vUser_Passwd   );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vLogin_Name[%s]",     vLogin_Name    );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vLogin_Passwd[%s]",   vLogin_Passwd  );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vProvince[%s]",       vProvince      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vRegion_Code[%s]",    vRegion_Code   );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vDistrict_Code[%s]",  vDistrict_Code );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vTown_Code[%s]",      vTown_Code     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vTerritory_Code[%s]", vTerritory_Code);
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vLimit_Owe[%s]",      vLimit_Owe     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vCredit_Code[%s]",    vCredit_Code   );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vCredit_Value[%s]",   vCredit_Value  );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vRun_Code[%s]",       vRun_Code      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vOld_Run[%s]",        vOld_Run       );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vRun_Time[%s]",       vRun_Time      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vBill_Date[%s]",      vBill_Date     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vBill_Type[%s]",      vBill_Type     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vLast_Bill_Type[%s]", vLast_Bill_Type);
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vOp_Time[%s]",        vOp_Time       );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vSrv_Start[%s]",      vSrv_Start     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vSrv_Stop[%s]",       vSrv_Stop      );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "vBak_Field[%s]",      vBak_Field     );
    pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "pay_code[%s]",        vPay_Code      );
#endif

    if (strlen(vPay_Code) == 0) {
        sprintf(grpUserMsg->return_message, "�ʻ����ʽ����Ϊ��!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
        return 190001;
    }

    /*��ȡ����Ա����Ȩ��*/
    EXEC SQL SELECT power_right INTO :vPower_right FROM dLoginMsg
              WHERE Login_No = :vLogin_No;
    if (SQLCODE != OK) {
        sprintf(grpUserMsg->return_message, "��ȡ����ԱȨ�޳���![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190001;
    }
	
    /*���ݲ���ԱȨ��,��������ͨ���ַ���*/
    /*����ԱȨ�ޣ�0��1��ʡ����Ȩ��;2��3������Ȩ��;4��5��6������Ȩ��;7������Ȩ��;
                  8��Ӫҵ�೤Ȩ��; 9��ӪҵԱȨ�� */
    if(vPower_right < 2) {
    	strcpy(vDisCode1, "99");
    	strcpy(vDisCode2, "");
    } else if ((vPower_right >= 2) && (vPower_right < 4)) {
    	strcpy(vDisCode1, "99");
    	strncpy(vDisCode2, vOrg_Code, 2);
    } else if (( vPower_right >= 4) && (vPower_right <= 7)) {
    	strcpy(vDisCode1, "9999");
    	strncpy(vDisCode2, vOrg_Code, 4);
    } else {
    	strcpy(vDisCode1, "9999999");
    	strncpy(vDisCode2, vOrg_Code, 7);
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ��Ʒ�Ƿ��Ѿ�����");
    #endif
    /*��ѯ��Ʒ�Ƿ��Ѿ�����*/
    EXEC SQL SELECT Count(*) INTO :vCount FROM sProductRelease
              WHERE (GROUP_ID like :vDisCode1||'%' or GROUP_ID like :vDisCode2||'%' )
                AND RELEASE_STATUS = 'Y'
                AND Product_Code = :vProduct_Code;
    if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "��ѯsProductRelease�����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190001;
    }
    if (vCount <= 0) {
        sprintf(grpUserMsg->return_message, "��Ʒ[%s]�����ڻ���δ����!",vProduct_Code);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190002;
    }

    /*��ѯ���ŵ�����Ϣ*/
    EXEC SQL SELECT VPMN_FLAG INTO :vVPMN_Flag
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vCust_Id);
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "����[%s]������Ϣ������!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190003;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "��ѯdCustDocOrgAdd�����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190004;
    }
    /*��VPMN���ſͻ�����������VPMN��Ʒ*/
    if (('0' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
    {
        sprintf(grpUserMsg->return_message, "��VPMN���ſͻ�,��������VPMN��Ʒ");
        return 190005;
    }
    
    /*��ѯ���ź�ͬ��Ϣ*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ���ź�ͬ��Ϣ");
    #endif
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Unit_Id, Trim(Unit_Code), Contract_Id, Trim(Contract_No)
               INTO :vUnit_Id, :vUnit_Code, :vContract_Id, :vContract_No
               FROM dGrpContract
              WHERE Cust_Id = To_Number(:vCust_Id);
    */
    EXEC SQL SELECT Unit_Id, Trim(Unit_Id), Contract_Id,Contract_Id
           INTO :vUnit_Id, :vUnit_Code, :vContract_Id, :vContract_No
           FROM dGrpContract
          WHERE Cust_Id = To_Number(:vCust_Id);
      
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/       
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "����[%s]��ͬ��Ϣ��δ¼��!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190006;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "��ѯdGrpContract�����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190007;
    }

    /*VPMN�û�����ʱ�����ؼ��ſͻ�ϵͳ��BOSS_VPMN_CODE�ֶ�*/
    Trim(vGrp_No);
    
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
   /*
    if (('1' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
    {
        EXEC SQL UPDATE dCustDocOrgAdd
                    SET BOSS_VPMN_CODE = :vGrp_No
                  WHERE Cust_Id = To_Number(:vCust_Id);
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "����dCustDocOrgAdd�����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190007;
        }
    }
    */
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

    Trim(vUnit_Id);
    Trim(vUnit_Code);
    Trim(vContract_Id);
    Trim(vContract_No);
    /*��ר��ҵ���⣬����ͬһ����Ʒ��������ͻ��ظ�����*/
    if ( (strcmp(vGrpProduct_Code, ZHUANX_IP_SM_CODE) != 0) &&
         (strcmp(vGrpProduct_Code, ZHUANX_DIANLU_SM_CODE) != 0) &&
         (strcmp(vGrpProduct_Code, ZHUANX_NET_SM_CODE) != 0) ) {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯdGrpUserMsg��Ϣ");
        #endif
        EXEC SQL SELECT Count(*) INTO :vCount
                   FROM dGrpUserMsg
                  WHERE Cust_Id = To_Number(:vCust_Id)
                    AND Product_Code = :vProduct_Code
                    AND Bill_Date > Last_Day(sysdate) + 1;
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "��ѯdGrpUserMsg�����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }
        if (vCount > 0) {
            sprintf(grpUserMsg->return_message, "�����û��Ѿ�����[%s]��Ʒ!",vProduct_Code);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190011;
        }
    }

    /*��ѯ�����ʻ���Ϣ*/
    /*yangzh modified 2004/12/13
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ�����ʻ���Ϣ");
    #endif
    EXEC SQL SELECT Trim(Contract_No)
               INTO :vAccount_Id
               FROM dConMsg
              WHERE CON_CUST_ID = To_Number(:vCust_Id);
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "����[%s]�ʻ���Ϣ������!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190008;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "��ѯdConMsg�����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190009;
    }
    yangzh modified 2004/12/13*/

    /*��ѯdConMsg������Ƿ���ڼ�¼*/
    EXEC SQL SELECT COUNT(*) INTO :vCount
               FROM dConMsg
              WHERE CONTRACT_NO = To_Number(:vAccount_Id);
    if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "��ѯ�����û�dConMsg�����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190009;
    }

    /*��¼dConMsg������¼*/
    if (vCount == 0) 
    { /*�ʻ���������ڣ��ͽ����ʻ���������,�����û�����ʱ����ѡ��ԭ�����ʻ�*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "���ɼ����û�ͳһ�����ʻ�");
        #endif
        
        

       Trim(vOrgId);

        
      /*��֯�������쿪����������  edit by liuweib at 23/02/2009
        EXEC SQL INSERT INTO dConMsgHis
               (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
                ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
                STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
                MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
                PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
                ACCOUNT_TYPE, UPDATE_LOGIN,  UPDATE_ACCEPT,   UPDATE_DATE,
                UPDATE_TIME,  UPDATE_CODE,   UPDATE_TYPE, GROUP_ID)
              VALUES
               (:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
                0,            :vOrg_Code,    0,               sysdate,
                'Y',          sysdate,       '0',             0,
                :vDateChar,   0,             0,               'A',
                :vPay_Code,   ' ',           ' ',             ' ',
                '1',          :vLogin_No,    :lLoginAccept,   :vTotal_Date,
                sysdate,      :vOp_Code,     'A', :vOrgId);
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgHis�����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190008;
        }
        EXEC SQL INSERT INTO dConMsg
               (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
                ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
                STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
                MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
                PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
                ACCOUNT_TYPE, GROUP_ID)
              VALUES
               (:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
                0,            :vOrg_Code,    0,               sysdate,
                'Y',          sysdate,       '0',             0,
                :vDateChar,   0,             0,               'A',
                :vPay_Code,   'NULL',        'NULL',          'NULL',
                '1', :vOrgId);
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "���뼯���û�dConMsg�����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190009;
        }
        *******��֯�������쿪����������  edit by liuweib at 23/02/2009*/
        
        sprintf(accDoc.vLogin_Accept,      	"%ld", lLoginAccept);
	      strcpy(accDoc.vContract_No,       	 vAccount_Id);
	      strcpy(accDoc.vCon_Cust_Id,       	    vCust_Id);
	      strcpy(accDoc.vContract_Passwd,	 vAccount_Passwd);
	      strcpy(accDoc.vBank_Cust,      	       vGrp_Name);
	      strcpy(accDoc.vBelong_Code,         vBelong_Code);
	      strcpy(accDoc.vAccount_Limit,     	         "A");
	      strcpy(accDoc.vStatus,            	         "Y");
	      strcpy(accDoc.vPost_Flag,         	         "0");
	      strcpy(accDoc.vPay_Code,          	   vPay_Code);
	      strcpy(accDoc.vBank_Code,      	          "NULL");
	      strcpy(accDoc.vPost_Bank_Code,    	      "NULL");
	      strcpy(accDoc.vAccount_No,      	        "NULL");
	      strcpy(accDoc.vAccount_Type,    	           "1");
	      strcpy(accDoc.vBegin_Time,      	   vTotal_Date);
	      strcpy(accDoc.vEnd_Time,         	            "");
	      strcpy(accDoc.vOp_Code,           	    vOp_Code);
	      strcpy(accDoc.vLogin_No,          	   vLogin_No);
	      strcpy(accDoc.vOrg_Code,          	   vOrg_Code);
	      strcpy(accDoc.vOp_Note,           	    vOp_Note);
	      strcpy(accDoc.vSystem_Note,       	    vOp_Note);
	      strcpy(accDoc.vIp_Addr,           	   "0.0.0.0");
	      strcpy(accDoc.vGroup_Id,          	      vOrgId);
	      strcpy(accDoc.vUpdate_Type,       	         "A");
	      accDoc.vOddment  = 0.00;
	      accDoc.vPrepay_Fee  = 0.00;
	      accDoc.vDeposit = 0.00;
	      accDoc.vOwe_Fee = 0.00;
	      accDoc.vAccount_Mark = 0.00;
        
        ret = 0;
        ret = createConMsg_ng(send_type,send_id,&accDoc,grpUserMsg->return_message);
        if(ret != 0)
        	{
        	   pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "-----%d:%s---",ret,grpUserMsg->return_message);
             return 190009;
        	}
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dConMsgPre");
        #endif
        /*
        EXEC SQL INSERT INTO dConMsgPre
                      ( CONTRACT_NO, PAY_TYPE,  PREPAY_FEE, LAST_PREPAY,
                        ADD_PREPAY,  LIVE_FLAG, ALLOW_PAY,  BEGIN_DT,
                        END_DT)
                 VALUES
                     ( To_Number(:vAccount_Id), '0', 0, 0,
                       0, '0', 0, :total_date,
                       :vSrv_Stop);
        if (SQLCODE != 0) {
            sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgPre����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }
		*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dCustConMsgHis");
        #endif
        EXEC SQL INSERT INTO dCustConMsgHis
                    (   Cust_Id,         Contract_No,     Begin_Time,
                        End_Time,        Update_Accept,   Update_Login,
                        Update_Date,     Update_Time,     Update_Code,
                        Update_Type
                    )
                    VALUES
                    (   To_Number(:vCust_Id),To_Number(:vAccount_Id), sysdate,
                        To_Date(:vSrv_Stop,'YYYYMMDD'),
                        To_Number(:lSysAccept),  :vLogin_No,
                        To_Number(:total_date),  sysdate, :op_code,
                        'a'
                    );
        if (SQLCODE != 0) {
            sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsgHis����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dCustConMsg");
        #endif
        /*NG��ź
        EXEC SQL INSERT INTO dCustConMsg
                    (  Cust_Id,  Contract_No,  Begin_Time,    End_Time )
                    VALUES
                    (   To_Number(:vCust_Id), To_Number(:vAccount_Id),
                        sysdate, To_Date(:vSrv_Stop,'YYYYMMDD')
                    );
        if (SQLCODE != 0) {
            sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }
         NG��ź*/
        /*ng����  by magang at 20090812 begin*/
        init(BeginTime1);
        EXEC SQL select to_char(sysdate,'YYYYMMDD HH24:MI:SS') 
        		 into :BeginTime1
        		 from dual;
        if (SQLCODE != 0)
        {
        	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ȡʱ��ʧ��");
        	return 192010;
        } 		 
        Trim(BeginTime1);
        memset(&sTdCustConMsg,0,sizeof(TdCustConMsg));
        strcpy(sTdCustConMsg.sCustId,vCust_Id);	
        strcpy(sTdCustConMsg.sContractNo,vAccount_Id);
        strcpy(sTdCustConMsg.sBeginTime,BeginTime1);
        strcpy(sTdCustConMsg.sEndTime,vSrv_Stop);	 	
        v_ret=0;
		v_ret=OrderInsertCustConMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdCustConMsg);

		if(0!=v_ret)
		{
			sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;	
		}	     		 	
        /*ng����  by magang at 20090812 END*/
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dConUserMsgHis");
    #endif
    EXEC SQL INSERT INTO dConUserMsgHis
            (
                Id_No,        Contract_No,  Serial_No,
                Bill_Order,   Pay_Order,    Begin_YMD,
                Begin_TM,     End_YMD,      End_TM,
                Limit_Pay,    Rate_Flag,    Stop_Flag,
                Update_Accept,Update_Login, Update_Date,
                Update_Time,  Update_Code,  Update_Type
            )
            VALUES
            (
                To_Number(:vGrp_Id),To_Number(:vAccount_Id),0,
                99999999,     1,           :total_date,
                SubStr(:total_date,1,6),   :vSrv_Stop,   Substr(:vSrv_Stop, 1,6),
                9999999999,  'N',          'Y',
                To_Number(:lLoginAccept), :vLogin_No, To_Number(:total_date),
                sysdate,     :op_code,    'a'
            );
    if (SQLCODE != 0) {
        sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsgHis����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dConUserMsg");
    #endif
    /*NG��ź
    
    EXEC SQL    INSERT INTO dConUserMsg
                (
                    Id_No,      Contract_No,  Serial_No,
                    Bill_Order, Pay_Order,    Begin_YMD,
                    Begin_TM,   End_YMD,      End_TM,
                    Limit_Pay,  Rate_Flag,    Stop_Flag
                )
                VALUES
                (
                    To_Number(:vGrp_Id),To_Number(:vAccount_Id),0,
                    99999999,   1,           :total_date,
                    SubStr(:total_date,1,6),
                    :vSrv_Stop, SubStr(:vSrv_Stop,1,6),
                    9999999999, 'N', 'Y'
                );
    if (SQLCODE != 0) {
        sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }
	NG��ź*/
	/*ng����  by magang at 20090812 begin
					init(TotalDate1);				
					EXEC SQL SELECT  SubStr(:total_date,1,6),SubStr(:vSrv_Stop,1,6)
                             INTO :TotalDate1,:SrvStop1					
							 FROM	DUAL;
					if (SQLCODE !=0 )
    				{
    		    		return 193010;
    				}
    				Trim(TotalDate1);
    				Trim(SrvStop1);
					memset(&tdConUserMsg,0,sizeof(TdConUserMsg));
					strcpy(tdConUserMsg.sIdNo,vGrp_Id);
					strcpy(tdConUserMsg.sContractNo,vAccount_Id);
					strcpy(tdConUserMsg.sBillOrder,"99999999");
					strcpy(tdConUserMsg.sSerialNo,"0");
					strcpy(tdConUserMsg.sPayOrder,"1");
					strcpy(tdConUserMsg.sBeginYmd,total_date);
					strcpy(tdConUserMsg.sBeginTm,TotalDate1);
					strcpy(tdConUserMsg.sEndYmd,vSrv_Stop);
					strcpy(tdConUserMsg.sEndTm,SrvStop1);
					strcpy(tdConUserMsg.sLimitPay,"9999999999");
					strcpy(tdConUserMsg.sRateFlag,"N");
					strcpy(tdConUserMsg.sStopFlag,"Y");
			v_ret=0;
			v_ret=OrderInsertConUserMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,tdConUserMsg);

			if(0!=v_ret)
			{
				sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        		return 190010;
			}	
		ng����  by magang at 20090811 end*/

 /*yangzh disabled 20050124 ����dCustMsg�����,�Ͳ�����wConUserChg
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into wConUserChg");
    #endif
    EXEC SQL INSERT INTO wConUserChg
                (
                    Op_No,       Op_Type,    Id_No,
                    Contract_No, Bill_Order, Conuser_Update_Time,
                    Limit_Pay,   Rate_Flag,  Fee_Code,
                    Fee_Rate,   Op_Time
                )
              VALUES
                (
                    sMaxBillChg.NextVal,     '10',     To_Number(:vGrp_Id),
                    To_Number(:vAccount_Id), 99999999, To_Char(sysdate,'YYYYMMDDHH24MISS'),
                    0,          '0',      ' ',
                    0,    To_Char(sysdate,'YYYYMMDDHH24MISS')
                 );
    if (SQLCODE != 0) {
        sprintf(grpUserMsg->return_message, "���뼯���û�wConUserChg����![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }
*/

    /*4.��¼�����û���dGrpUserMsg��¼,BOSS�������*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpUserMsgHis����Ϣ");
    #endif
    
    printf("-------------------------------vCust_Id=%s-------------------",vCust_Id); 		
    printf("-------------------------------vGrp_Id=%s-------------------",vGrp_Id);
    printf("-------------------------------vAccount_Id=%s-------------------",vAccount_Id);
    printf("-------------------------------vGrp_No=%s-------------------",vGrp_No);
    printf("-------------------------------vGrp_Name=%s-------------------",vGrp_Name);
    printf("-------------------------------vProduct_Type=%s-------------------",vProduct_Type);
    printf("-------------------------------vProduct_Code=%s-------------------",vProduct_Code);
    printf("-------------------------------vUser_Passwd=%s-------------------",vUser_Passwd);
    printf("-------------------------------vProvince=%s-------------------",vProvince);
    printf("-------------------------------vRegion_Code=%s-------------------",vRegion_Code);
    printf("-------------------------------vDistrict_Code=%s-------------------",vDistrict_Code);
    printf("-------------------------------vTown_Code=%s-------------------",vTown_Code);
    printf("-------------------------------vCredit_Code=%s-------------------",vCredit_Code);
    printf("-------------------------------vCredit_Value=%s-------------------",vCredit_Value);
    printf("-------------------------------TotalDate1=%s-------------------",TotalDate1);
    printf("-------------------------------vSrv_Stop=%s-------------------",vSrv_Stop);
    printf("-------------------------------vOp_Code=%s-------------------",vOp_Code);
    printf("-------------------------------vSm_Code=%s-------------------",vSm_Code);
    printf("-------------------------------vOrgId=%s-------------------",vOrgId);
    printf("-------------------------------vTotal_Date=%s-------------------",vTotal_Date);
    printf("-------------------------------lLoginAccept=[%ld]-------------------",lLoginAccept);
   
/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
    EXEC SQL INSERT INTO dGrpUserMsgHis
          (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
           IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
           USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
           REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
           LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
           OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
           LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
           UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
           UPDATE_TYPE, SM_CODE, GROUP_ID)
        VALUES
         (:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_No,
          1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
          :vUser_Passwd, ' ',           ' ',            :vProvince,
          :vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
          1000000,     :vCredit_Code, :vCredit_Value,   'A',
          'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
          '1',           sysdate,       :vOp_Code,      :vLogin_No,
          :lLoginAccept, :vTotal_Date,  sysdate,        :vOp_Code,
          'A', :vSm_Code, :vOrgId);
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpUserMsgHis�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190012;
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpUserMsg����Ϣ");
    #endif
    /*ng���� 
    EXEC SQL INSERT INTO dGrpUserMsg
          (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
           IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
           USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
           REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
           LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
           OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
           LAST_BILL_TYPE, OP_TIME,    BAK_FIELD, SM_CODE, GROUP_ID)
        VALUES
         (:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_No,
          1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
          :vUser_Passwd, ' ',           ' ',            :vProvince,
          :vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
          1000000,       :vCredit_Code, :vCredit_Value, 'A',
          'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
          '1',           sysdate,       :vOp_Code, :vSm_Code, :vOrgId);
    if (SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190013;
    }
    ng����*/
	/*ng����  by magang at 20090812 begin*/
	    pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMsg!\n");
	    memset(&sTdGrpUserMsg,0,sizeof(TdGrpUserMsg));
	    init(TotalDate1);				
		EXEC SQL SELECT  to_char(sysdate,'YYYYMMDD HH24:MI:SS')
                 INTO :TotalDate1				
				 FROM	DUAL;
		if (SQLCODE !=0 )
 		{
 			sprintf(grpUserMsg->return_message, "��ȡʱ�����![%d]",SQLCODE);
 			pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
      		return 193010;
      	}  				
      	Trim(TotalDate1);
	    strcpy(sTdGrpUserMsg.sCustId,vCust_Id);
	    strcpy(sTdGrpUserMsg.sIdNo,vGrp_Id);
	    strcpy(sTdGrpUserMsg.sAccountId,vAccount_Id);
	    strcpy(sTdGrpUserMsg.sUserNo,vGrp_No);
	    strcpy(sTdGrpUserMsg.sIds,"1");
	    strcpy(sTdGrpUserMsg.sUserName,vGrp_Name);
	    strcpy(sTdGrpUserMsg.sProductType,vProduct_Type);
	    strcpy(sTdGrpUserMsg.sProductCode,vProduct_Code);
	    strcpy(sTdGrpUserMsg.sUserPasswd,vUser_Passwd);
	    strcpy(sTdGrpUserMsg.sLoginName," ");
	    strcpy(sTdGrpUserMsg.sLoginPasswd," ");
	    strcpy(sTdGrpUserMsg.sProvCode,vProvince);
	    strcpy(sTdGrpUserMsg.sRegionCode,vRegion_Code);
	    strcpy(sTdGrpUserMsg.sDistrictCode,vDistrict_Code);
	    strcpy(sTdGrpUserMsg.sTownCode,vTown_Code);
	    strcpy(sTdGrpUserMsg.sTerritoryCode,"0");
	    strcpy(sTdGrpUserMsg.sLimitOwe,"1000000");
	    strcpy(sTdGrpUserMsg.sCreditCode,vCredit_Code);
	    strcpy(sTdGrpUserMsg.sCreditValue,vCredit_Value);
	    strcpy(sTdGrpUserMsg.sRunCode,"A");
	    strcpy(sTdGrpUserMsg.sOldRun,"A"); 
	    strcpy(sTdGrpUserMsg.sRunTime,TotalDate1);
	    strcpy(sTdGrpUserMsg.sBillDate,vSrv_Stop);
	    strcpy(sTdGrpUserMsg.sBillType,"1"); 
	    strcpy(sTdGrpUserMsg.sLastBillType,"1"); 
	    strcpy(sTdGrpUserMsg.sOpTime,TotalDate1);
	    strcpy(sTdGrpUserMsg.sBakField,vOp_Code);
	    strcpy(sTdGrpUserMsg.sSmCode,vSm_Code);
	    strcpy(sTdGrpUserMsg.sGroupId,vOrgId);
	    v_ret=0;
		v_ret=OrderInsertGrpUserMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdGrpUserMsg);

		if(0!=v_ret)
		{
		  sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190013;	
		}
		/*ng����  by magang at 20090812 end*/
 /*���ٴ���vip�ı�chenxuan boss3*/ 
#if 0
    /*��ѯsGrpOperationProductCode�õ�GRP���ҵ�����*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯsGrpOperationProductCode����Ϣ");
    #endif
    EXEC SQL SELECT OPERATION_CODE
               INTO :vOperation_Code
               FROM sGrpOperationProductCode
              WHERE Product_Code = :vGrpProduct_Code;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"��ѯsGrpOperationProductCode����!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }

    /*��ò�ƷID*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ò�ƷID");
    #endif
    EXEC SQL SELECT To_Char(s_Product_Id.NextVal) INTO :vProduct_Id FROM DUAL;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"��ѯs_Product_Id����!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }

    /*LINA VIP2.0 2006_11_05 begin here*/
    /*ȡvip��֯��dgrpgroups��org_code*/    
    EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    EXEC SQL END DECLARE SECTION;
    /*��ѯ���ſͻ���Ϣ*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ���ſͻ���Ϣ");
    #endif
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vCust_Id)
                AND Unit_Id = To_Number(:vUnit_Id);
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"��ѯOrg_Code����!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }
    */
    strcpy(sIn_Grp_OrgCode,"");
   /* --- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
       
    /*���뼯���û���չ��, ���Ų��*/
    /*product_statusȡ��dbgrpadm.sGrpProductStatusCode�ֵ��*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpProductHis����Ϣ");
    #endif
    /*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
    
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL INSERT INTO dGrpProductHis
         (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
          PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
          PRODUCT_STATUS, OPERATION_CODE,
          UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
          UPDATE_CODE,  UPDATE_TYPE, ORG_CODE)
         VALUES
          (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
           :vGrpProduct_Code, :vContract_Id, :vContract_No,
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           :vOp_Note,
           '1',            :vOperation_Code,
           :vLogin_No,     :lLoginAccept, :vTotal_Date,  sysdate,
           :vOp_Code,      'A',           :sIn_Grp_OrgCode);
     */
     EXEC SQL INSERT INTO dGrpProductHis
         (CUST_ID,      UNIT_ID,     PRODUCT_ID,
          PRODUCT_CODE, CONTRACT_ID, ORDER_TIME,
          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
          PRODUCT_STATUS, OPERATION_CODE,
          UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
          UPDATE_CODE,  UPDATE_TYPE, SAVE_FILE,Create_login,Create_date)
         VALUES
          (:vCust_Id,      :vUnit_Id,    :vProduct_Id,
           :vGrpProduct_Code, :vContract_Id, 
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           :vOp_Note,
           '1',            :vOperation_Code,
           :vLogin_No,     :lLoginAccept, :vTotal_Date,  sysdate,
           :vOp_Code,      'A',NULL,:vLogin_No,sysdate);       
            
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
            
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpProductHis�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190015;
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpProduct����Ϣ");
    #endif
    
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL INSERT INTO dGrpProduct
         (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
          PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
          PRODUCT_STATUS, OPERATION_CODE, ORG_CODE)
         VALUES
          (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
           :vGrpProduct_Code, :vContract_Id, :vContract_No,
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           :vOp_Note,
           '1',            :vOperation_Code     ,:sIn_Grp_OrgCode);
    */
    EXEC SQL INSERT INTO dGrpProduct
         (CUST_ID,      UNIT_ID,     PRODUCT_ID,
          PRODUCT_CODE, CONTRACT_ID, ORDER_TIME,
          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
          PRODUCT_STATUS, OPERATION_CODE, SAVE_FILE,Create_date,Create_login)
    VALUES
          (:vCust_Id,      :vUnit_Id,    :vProduct_Id,
           :vGrpProduct_Code, :vContract_Id,
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
           To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
           :vOp_Note,
           '1',            :vOperation_Code     ,NULL,sysdate,:vLogin_No);
            
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
           
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpProduct�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190016;
    }
   
    /*��ö���ID*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ö���ID");
    #endif
    EXEC SQL SELECT To_Char(s_order_goods_id.NextVal) INTO :vOrder_GoodsId FROM DUAL;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"��ѯs_order_goods_id����!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190017;
    }

    /*���붩�����¼,���Ų��*/
    /*������Ч״̬CURR_STATUS��sGrpOrderGoodsStatusCode */
    /*����ʩ��״̬URR_TACHE��sGrpOrderGoodsTacheCode*/
    Trim(vProduct_Id);
    Trim(vOrder_GoodsId);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpOrderGoodsHis����Ϣ");
    #endif
    
   /*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
    EXEC SQL INSERT INTO dGrpOrderGoodsHis
           (ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
            UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
            DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
            CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
            CURR_TACHE,
            UPDATE_LOGIN,   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
            UPDATE_CODE,    UPDATE_TYPE  , ORG_CODE)
           VALUES
           (:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
            :vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
            To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
            To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
            :vGrpProduct_Code, :vOp_Note,
            '1', '01', '01', '01',
            '01',
            :vLogin_No,      :lLoginAccept, :vTotal_Date,  sysdate,
            :vOp_Code,       'A'          , :sIn_Grp_OrgCode);
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpOrderGoodsHis�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190018;
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpOrderGoods����Ϣ");
    #endif
    EXEC SQL INSERT INTO dGrpOrderGoods
           (ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
            UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
            DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
            CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
            CURR_TACHE ,    ORG_CODE)
           VALUES
           (:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
            :vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
            To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
            To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
            :vGrpProduct_Code, :vOp_Note,
            '1', '01', '01', '01',
            '01', :sIn_Grp_OrgCode);
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����dGrpOrderGoods�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190019;
    }
     /*LINA VIP2.0 end here*/

    /*���붩��������¼��*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼wGrpOrderOpr����Ϣ");
    #endif
    EXEC SQL insert into wGrpOrderOpr
            (LOGIN_ACCEPT, CUST_ID,       UNIT_ID,    DIS_ORDER_GOODS_ID,
             OPERATOR,     OPERATOR_TIME, CURR_TACHE, CURR_STATUS,
             OPERATOR_CODE)
          values
            (:lLoginAccept, :vCust_Id,    :vUnit_Id,  :vOrder_GoodsId,
             :vLogin_No,    sysdate,      '01',       '01',
             '01');
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "����wGrpOrderOpr�����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190020;
    }
#endif

     /*��ѯ��Ʒ������ϵ */
    vCount = 1;
    strToFilter(vProduct_Code, vProduct_List, strlen(vProduct_Code), sizeof(vProduct_List));
    strcpy(vProduct_All, vProduct_List);
    while (vCount > 0) {
        init(sqlStr);
        sprintf(sqlStr, "SELECT distinct a.product_code FROM sproductcode a, sproductrelease b WHERE a.product_code IN ( SELECT object_product FROM sproductcode a, sproductattach b WHERE a.product_code = b.product_code AND a.product_code IN (%s) MINUS SELECT object_product FROM sproductcode a, sProductExclude b WHERE a.product_code = b.product_code AND a.product_code IN (%s)) AND a.product_code = b.product_code AND b.release_status = 'Y' AND (b.GROUP_ID LIKE '%s%%' OR b.GROUP_ID LIKE '%s%%') AND SYSDATE BETWEEN b.begin_time AND b.end_time", vProduct_List, vProduct_List, vDisCode1, vDisCode2);
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ��Ʒ������ϵ, vDisCode1[%s], vDisCode[%s], vProduct_List[%s], vProduct_All[%s]", vDisCode1, vDisCode2, vProduct_List, vProduct_All);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
        #endif
 
        EXEC SQL PREPARE stmt001 FROM :sqlStr;
        if (SQLCODE != OK) {
             sprintf(grpUserMsg->return_message, "׼����ѯ������Ʒ�α����![%d]",SQLCODE);
             pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
             return 190020;
        }
        EXEC SQL DECLARE attachprod_cur0 CURSOR for stmt001;
        if (SQLCODE != OK) {
             sprintf(grpUserMsg->return_message, "���������Ʒ�α����![%d]",SQLCODE);
             pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
             return 190020;
        }

        vCount = 0;
        init(vProduct_List);
        EXEC SQL OPEN attachprod_cur0;
        if(SQLCODE!=OK){
             
             sprintf(grpUserMsg->return_message, "�򿪹�����Ʒ�α����![%d]",SQLCODE);
             pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
             EXEC SQL CLOSE attachprod_cur0;
             return 190020;
        }

        EXEC SQL FETCH attachprod_cur0 INTO :vProduct_Code;
        while(SQLCODE == 0)
        {
            vCount++;
            if (vCount > 1)
                sprintf(vProduct_List, "%s, '%s'", vProduct_List, vProduct_Code);
            else
                sprintf(vProduct_List, "'%s'", vProduct_Code);
            EXEC SQL FETCH attachprod_cur0 INTO :vProduct_Code;
        }
        EXEC SQL CLOSE attachprod_cur0;

        if (strlen(vProduct_List) > 0) {
            sprintf(vProduct_All, "%s, %s", vProduct_All, vProduct_List);
        }
    }

     /*��ѯ��Ʒ�����ϵ�� */
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ��Ʒ�����ϵ��");
    #endif
    init(sqlStr);
    sprintf(sqlStr, "select b.product_code, a.service_type, b.service_code, b.main_flag, b.fav_order, c.price_code from sSrvCode a, sProductSrv b, sSrvPrice c where a.service_code = b.service_code and a.service_code = c.service_code and b.product_code in (%s)", vProduct_All);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��ѯ��Ʒ�����ϵ��, vProduct_All[%s]", vProduct_All);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
    #endif

    EXEC SQL PREPARE stmt002 FROM :sqlStr;
    if (SQLCODE != OK) {
         sprintf(grpUserMsg->return_message, "������Ʒ�����ϵ�α����![%d]",SQLCODE);
         pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
         return 190020;
    }

    EXEC SQL DECLARE srv_cur0 CURSOR for stmt002;
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "��ѯ��Ʒ������Ӧ��ϵ����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190021;
    }

    EXEC SQL OPEN srv_cur0;
    if(SQLCODE!=OK){
          EXEC SQL CLOSE srv_cur0;
          sprintf(grpUserMsg->return_message, "�򿪲�Ʒ������Ӧ��ϵ�α����![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190022;
    }

    init(vProduct_Code);
    EXEC SQL FETCH srv_cur0 INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
    while(SQLCODE == 0)
    {

        /*��¼�����û�������Ʒ��,BOSS�������*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpSrvMsgHis����Ϣ");
        #endif
       EXEC SQL INSERT INTO dGrpSrvMsgHis
            (ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
             SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
             MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
             OP_TIME,   UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
             UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,USE_FLAG,PERSON_FLAG)
            VALUES
            (:vGrp_Id,    :vService_Type, :vService_Code, :vPrice_Code,
             :vFav_Order, :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
             :vMain_Flag, :lLoginAccept,  :vLogin_No,     :vOp_Code,
             sysdate,     :vLogin_No,     :lLoginAccept,  :vTotal_Date,
             sysdate,     :vOp_Code,       'A','Y','N');
        if(SQLCODE!=OK){
              sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsgHis�����![%d]",SQLCODE);
              pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
              return 190023;
        }

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "��¼dGrpSrvMsg����Ϣ");
        #endif
        EXEC SQL INSERT INTO dGrpSrvMsg
            (ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
             SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
             MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
             OP_TIME,USE_FLAG,PERSON_FLAG)
            VALUES
            (:vGrp_Id,    :vService_Type, :vService_Code, :vPrice_Code,
             :vFav_Order, :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
             :vMain_Flag, :lLoginAccept, :vLogin_No,      :vOp_Code,
             sysdate,'Y','N');
        if(SQLCODE!=OK){
              sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsg�����![%d]",SQLCODE);
              pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
              return 190024;
        }
        EXEC SQL FETCH srv_cur0 INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
    }

    EXEC SQL CLOSE srv_cur0;

    return 0;
}

/************************************************************************
��������:fpubDelGrpMember_ng
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û�ɾ����Ա���ú���
�������:�����û�id
         ��Ա�ֻ�����
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע 
         ������Ϣ
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubDelGrpMember_ng(char *send_type,char *send_id,char *inGrpId_No, char *phone_no, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{
    int  retCode = 0;
 
    EXEC SQL BEGIN DECLARE SECTION;
         int  vCnt = 0;
         int  vProduct_Level = 0;
         char vGrpCust_Id[22+1];
         char vGrpId_No[22+1];
         char vPhone_No[22+1];
         char vCust_Id[10+1];
         char vId_No[14+1];
         char vSm_Code[2+1];
         char vUnit_Id[10+1];
         char vUnit_Code[10+1];
         char vGrp_Userno[15+1];
         char vProduct_Type[8+1];
         char vProduct_Code[8+1];
         char vService_Code[4+1];
         char vAccount_Id[14+1];

         long lLoginAccept = 0;
         char vLogin_No[6+1];
         char vOp_Code[4+1];
         char vTotal_Date[8+1];
         char vOp_Note[60+1];
         int   ret=0;                   /*add by miaoyl at 20090708  */
         char  vOrg_Id[10+1];           /*add by miaoyl at 20090708  */
         
          /*NG��ź*/
         int	v_ret=0;
         char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
         char	MemberId1[22+1];
		 char	BeginTime1[20+1];
		 char	EndTime1[20+1];
		 TdGrpUserMebMsgIndex oldIndex1;
		 TdGrpUserMebMsgIndex newIndex1;
		 char	sTempSqlStr[1024+1];
		 char	IdNo3[22+1];
		 char	vTotal_Date1[22+1];
		 int 	ncount=0;
    EXEC SQL END DECLARE SECTION;

    /*������ʼ��*/
    init(vGrpCust_Id      );
    init(vGrpId_No        );
    init(vPhone_No        );
    init(vId_No           );
    init(vCust_Id         );
    init(vSm_Code         );
    init(vUnit_Id         );
    init(vUnit_Code       );
    init(vGrp_Userno      );
    init(vProduct_Type    );
    init(vProduct_Code    );
    init(vService_Code    );
    init(vAccount_Id      );
    init(vLogin_No        );
    init(vOp_Code         );
    init(vTotal_Date      );
    init(vOp_Note         );
    init(vAccount_Id      );
    init(vOrg_Id          );  /*add by miaoyl at 20090708  */
    init(vTotal_Date1	  );
    /*������ֵ*/
    lLoginAccept = lSysAccept;
    strcpy(vGrpId_No,   inGrpId_No);
    strcpy(vPhone_No,   phone_no  );
    strcpy(vLogin_No,   login_no  );
    strcpy(vOp_Code,    op_code   );
    strcpy(vTotal_Date, total_date);
    strcpy(vOp_Note,    op_note   );
    Trim(vPhone_No);
	
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
    /*ȡ����ORG_ID*/
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
		sprintf(return_message,"��ѯorg_idʧ��![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190200;
	}
	Trim(vOrg_Id);
   /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/
       
    /*��ѯ�����û�����*/
    EXEC SQL SELECT Cust_Id, User_No, Account_Id, Product_Type, Product_Code
               INTO :vGrpCust_Id, :vGrp_Userno, :vAccount_Id, :vProduct_Type, :vProduct_Code
               FROM dGrpUserMsg
              WHERE ID_NO = To_Number(:vGrpId_No)
                AND Bill_Date > Last_Day(sysdate) + 1;
    if (SQLCODE !=0){
        sprintf(return_message,"��ѯdGrpUserMsg����[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190201;
    }

    /*��ѯ�ֻ��ͻ�����*/
    EXEC SQL SELECT id_no, CUST_ID, sm_code
               INTO :vId_No, :vCust_Id, :vSm_Code
               FROM dCustMsg
              WHERE phone_no = :vPhone_No ;
    if (SQLCODE != 0){
        sprintf(return_message,"��ѯdCustMsg�����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190202;
    }
		
		/* ��֤�Ƿ��Ѽ��� */
    EXEC SQL select count(*) into :vCnt from dGrpUserMebMsg 
    	where id_no = :vGrpId_No and member_id = :vId_No and end_time > last_day(sysdate)+1;
    if (vCnt == 0) {
      sprintf(return_message, "�ֻ�����[%s]δ���뼯��[%s]!", vPhone_No, vGrpId_No);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190203;;
    }

    /*����dGrpMemberMsg*/
    /*#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "��ѯdGrpMemberMsg��");
    #endif
    EXEC SQL SELECT To_Char(UNIT_ID) INTO :vUnit_Id
               FROM dGrpMemberMsg
        WHERE Cust_Id = To_Number(:vGrpCust_Id)
          AND ID_NO = To_Number(:vId_No);
    if (SQLCODE != 0 && ){
        sprintf(return_message,"��ѯdGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190203;
    }*/

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "��ѯdGrpUserMebMsg��");
    #endif
    EXEC SQL SELECT COUNT(*) INTO :vCnt
              FROM dGrpUserMebMsg
            WHERE Member_Id = To_Number(:vId_No)
              AND Id_No In
                  (SELECT Id_No FROM dGrpUserMsg
                    WHERE Cust_Id = To_Number(:vGrpCust_Id)
                      AND Bill_Date > Last_Day(sysdate) + 1
                   )
              AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0){
        sprintf(return_message,"��ѯdGrpUserMebMsg��[%s]������[%d]", vId_No, SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
          return 190204;
      }

   /* LINA VIP2.0 2006_11_05 begin here*/
    /*ȡvip��֯��dgrpgroups��org_code*/   
    
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    	char vOrg_id[10+1];
    EXEC SQL END DECLARE SECTION;
    init(vOrg_id);*/
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
    
    /*��ѯ���ſͻ���Ϣ*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "��ѯ���ſͻ���Ϣ");
    #endif
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vGrpCust_Id);
        if (SQLCODE != 0){
            sprintf(return_message,"��Org_Codeʧ��",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190105;
        }
    */   
    /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
    
    if (vCnt < 2) { /*�û�ֻ��һ�������û��ĳ�Աʱ��ɾ��*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "ɾ��dGrpMemberMsg��[%s]����", vId_No);
        #endif
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "LINA++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        /*-------��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009-------*/
       
        /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
        /*
        EXEC SQL INSERT INTO dGrpMemberMsgHis
                 (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO,ID_NO,
                  PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  GMEMBER_STATUS_CODE, UPDATE_LOGIN, UPDATE_ACCEPT,
                  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, ORG_CODE, GROUP_ID)
           SELECT CUST_ID, UNIT_ID, UNIT_CODE, ID_NO, ID_NO,
                  PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  GMEMBER_STATUS_CODE, :vLogin_No, :lLoginAccept,
                  :vTotal_Date,  sysdate, :vOp_Code, 'd', :sIn_Grp_OrgCode, GROUP_ID
             FROM dGrpMemberMsg
            WHERE Cust_Id = To_Number(:vGrpCust_Id)
              AND ID_NO = To_Number(:vId_No);
       */
        
        EXEC SQL INSERT INTO dGrpMemberMsgHis
                 (CUST_ID, UNIT_ID,ID_NO,
                  PHONE_NO, USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  UPDATE_LOGIN, UPDATE_ACCEPT,
                  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, GROUP_ID,
                  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
                  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
                  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
                  correct_note,ORG_ID)
         SELECT CUST_ID, UNIT_ID, ID_NO,
                  PHONE_NO,USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
                  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
                  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
                  :vLogin_No, :lLoginAccept,
                  :vTotal_Date,  sysdate, :vOp_Code, 'd', GROUP_ID,
                  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
                  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
                  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
                  correct_note,:vOrg_Id              
             FROM dGrpMemberMsg
            WHERE Cust_Id = To_Number(:vGrpCust_Id)
              AND ID_NO = To_Number(:vId_No);  
                     
       /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/    

        if (SQLCODE != 0 && SQLCODE != 1403){
            sprintf(return_message,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190206;
        }
        
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vLogin_No = [%s]",vLogin_No);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vTotal_Date = [%s]",vTotal_Date);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":lLoginAccept= [%ld]",lLoginAccept);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vOp_Code= [%s]",vOp_Code);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vPhone_No= [%s]",vPhone_No);
        
        EXEC SQL delete FROM dGrpMemberMsg
            WHERE Cust_Id = To_Number(:vGrpCust_Id)
              AND ID_NO = To_Number(:vId_No);
        if (SQLCODE != 0 && SQLCODE != 1403){
            sprintf(return_message,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190207;
        }
        /* LINA VIP2.0 end here*/
        /*yangzh add 2005-01-24 �����û��뿪����ʱ����attr_codeΪ���˿ͻ���־*/
        /*NG��ź
        EXEC SQL UPDATE dcustmsg
           SET attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)
         WHERE phone_no = :vPhone_No;
        if (SQLCODE != 0){
            sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190208;
        }
        NG��ź*/
         /*ng����  by magang at 20090812 begin*/
         ncount=0;
    	memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select to_char(id_no)  from dcustmsg where phone_no = :vPhone_No");		 
	 	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
        EXEC SQL declare ngcursor4 cursor for sql_str;
        EXEC SQL open ngcursor4 using :vPhone_No;
    	for(;;)	
     	{   
     		init(IdNo3);
     		EXEC SQL FETCH ngcursor4 into :IdNo3;    			
     		if((0!=SQLCODE)&&(1403!=SQLCODE))
        	{
        		PUBLOG_DBDEBUG("fpubDelGrpUser");
        		pubLog_DBErr(_FFL_,"","","��ȡid_noʧ�� [%s]--\n",vPhone_No);
        		EXEC SQL CLOSE ngcursor4;
				return 191207;
        	}
        	if(1403==SQLCODE) break;
        	Trim(IdNo3);				
    	 	pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateCustMsg!\n");    	 
			init(i_parameter_array);
			strcpy(i_parameter_array[0],IdNo3);
			v_ret=0;
			v_ret=OrderUpdateCustMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,IdNo3,"attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)","",i_parameter_array);
	
			if(0!=v_ret)
			{
				sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
           	 	pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
           	 	EXEC SQL CLOSE ngcursor4;
            	return 190208;
			}
			ncount++;
		}
		EXEC SQL CLOSE ngcursor4;
		if(ncount==0)
		{
			sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190208;
		}	
 	/*ng����  by magang at 20090804 END*/
    }

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "���ú������Լ����û���Ա�ĸ��Ѽƻ����б��");
#endif

    /* ��Աɾ��ǰ�ж��и��Ѽƻ������в�����ɾ���������������Ѽƻ���� 
    EXEC SQL select count(*) into :vCnt from dConUserMsg
      where id_no = :vId_No and contract_no = :vAccount_Id and serial_no = 0;
    if (SQLCODE != 0) {
      sprintf(return_message, "��ѯ��Ա���Ѽƻ�����[%d]", SQLCODE);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190109;
    }

    if (vCnt > 0) {
      sprintf(return_message,"��Ա[%s]�и��Ѽƻ�������ɾ��������ִ�и��Ѽƻ������", vPhone_No);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190110;
    }
    boss3.0 */

    /* ����������������ƣ�����ִ��������룬�������������Ա��Ժ��޸ķ��� 
    retCode = fpubDisConUserMsg(vId_No, vAccount_Id, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
    if (retCode != 0) {
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "����fpubDisConUserMsg��������,Id_No[%s],Account_Id[%s],retCode[%d]", vId_No, vAccount_Id, retCode);
      return 190111;
    }
    boss3.0 */
    
    /*����dGrpUserMebMsg*/
    EXEC SQL INSERT INTO dGrpUserMebMsgHis
                 (ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
                  BEGIN_TIME, END_TIME,
                  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
                  UPDATE_CODE, UPDATE_TYPE)
              SELECT
                  ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
                  BEGIN_TIME, END_TIME,
                  :vLogin_No, :lLoginAccept, :vTotal_Date, sysdate,
                  :vOp_Code, 'd'
                FROM dGrpUserMebMsg
               WHERE Id_No = To_Number(:vGrpId_No)
                 AND Member_no = :vPhone_No
                 AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0){
        sprintf(return_message,"����dGrpUserMebMsgHis�����Ϸ����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190212;
    }
    /*NG��ź
    EXEC SQL UPDATE dGrpUserMebMsg
                SET End_Time = Last_Day(To_Date(:vTotal_Date,'YYYYMMDD')) + 1
               WHERE Id_No = To_Number(:vGrpId_No)
                 AND Member_no = :vPhone_No
                 AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0 || SQLROWS != 1){
        sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190213;
    }
	NG��ź*/
	/*ng����  by magang at 20090812 begin*/
    		pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateGrpUserMebMsg!\n");
    		
   EXEC SQL select to_char(member_id),to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time ,'yyyymmdd hh24:mi:ss'),to_char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD'))+1,'yyyymmdd hh24:mi:ss') 
			into :MemberId1,:BeginTime1,:EndTime1,:vTotal_Date1
			from dGrpUserMebMsg 
			where Id_No = To_Number(:vGrpId_No)
                 AND Member_no = :vPhone_No
                 AND End_Time > Last_Day(sysdate) + 1;

     			if (SQLCODE != 0 || SQLROWS != 1)
        		{
        			sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
        			pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
       			 	return 190213;
        		}
			
			Trim(vGrpId_No);
        	Trim(MemberId1);	
        	Trim(BeginTime1);
        	Trim(EndTime1);
        	Trim(vTotal_Date1);
       		Trim(vPhone_No);
       		
        	memset(&oldIndex1,0,sizeof(TdGrpUserMebMsgIndex));
        	memset(&newIndex1,0,sizeof(TdGrpUserMebMsgIndex));
    		strcpy(oldIndex1.sIdNo,vGrpId_No);
    		strcpy(oldIndex1.sMemberId,MemberId1);
    		strcpy(oldIndex1.sBeginTime,BeginTime1);
    		strcpy(oldIndex1.sEndTime,EndTime1);
    		
    		strcpy(newIndex1.sIdNo,vGrpId_No);
    		strcpy(newIndex1.sMemberId,MemberId1);
    		strcpy(newIndex1.sBeginTime,BeginTime1);
    		strcpy(newIndex1.sEndTime,vTotal_Date1);
    		
    		init(i_parameter_array);
    		strcpy(i_parameter_array[0],vTotal_Date);
			strcpy(i_parameter_array[1],vGrpId_No);
			strcpy(i_parameter_array[2],MemberId1);
			strcpy(i_parameter_array[3],BeginTime1);
			strcpy(i_parameter_array[4],EndTime1);
			strcpy(i_parameter_array[5],vPhone_No);
			
			v_ret=0;
			v_ret=OrderUpdateGrpUserMebMsg(send_type,send_id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex1,newIndex1,
												"End_Time = Last_Day(To_Date(:vTotal_Date,'YYYYMMDD')) + 1",
												"AND Member_no = :vPhone_No ",i_parameter_array);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        		return 190213;
			}

    /*ng����  by magang at 20090811 end*/
	
	
/* ���ٴ����ѷϳ���dGrpUserMebProdMsg chenxuan boss3
    EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
               (ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
                PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
                BEGIN_TIME, END_TIME,
                BAK_FIELD, UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
                UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
             SELECT
                ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
                PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
                BEGIN_TIME, END_TIME,
                BAK_FIELD, :vLogin_No, :lLoginAccept, :vTotal_Date,
                sysdate, :vOp_Code, 'd'
               FROM dGrpUserMebProdMsg
              WHERE ID_No = To_Number(:vGrpId_No)
                AND MEMBER_ID = To_Number(:vId_No);
    if (SQLCODE != 0){
        sprintf(return_message,"����dGrpUserMebProdMsgHis�����Ϸ����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190209;
    }
    EXEC SQL UPDATE dGrpUserMebProdMsg
                SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
              WHERE ID_No = To_Number(:vGrpId_No)
                AND MEMBER_ID = To_Number(:vId_No)
                AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0){
        sprintf(return_message,"ɾ��dGrpUserMebProdMsg�����Ϸ����쳣[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190210;
    }
*/
    return 0;
}

int fCheckProdMebLimit_ng(char *iGrpId, char *iMebId, char *iProductCode, char *iLimitFlag, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0;
	EXEC SQL END DECLARE SECTION;
	
	/*  */
	if (iLimitFlag[0] == '1') {
    EXEC SQL select count(*) into :vCount
    	from dGrpUserMebMsg a, dGrpUserMsg b
      where a.id_no = b.id_no and a.member_id = to_number(:iMebId)
      	and b.product_code = :iProductCode and b.bill_date > last_day(sysdate) + 1
        and a.end_time > last_day(sysdate) + 1;
    if (SQLCODE != 0) {
    	sprintf(retMsg, "��ѯ���Ų�Ʒ�����[%d]", SQLCODE);
    	return -1;
    }    
    if (vCount > 0) {
    	sprintf(retMsg, "��Ա[%s]�Ѽ���ͬ�༯��", iMebId);
    	return -1;
    }
	}
	
	return 0;
}

int PublicGoStop_ng(DCUSTMSG_TYPE *dCustMsg,DLOGINMSG_TYPE *dLoginMsg,char *InRunCode,char *InOpNote)
{
	 EXEC SQL BEGIN DECLARE SECTION;
		long id_no = 0;
		char phone_no[15+1];
		double  should_pay=0;
		int stop_flag=0;
		char TmpSqlStr[2000+1];
		char run_code[2+1];
		char sm_code[2+1];
		char attr_code[8+1];
		char belong_code[7+1];
		long login_accept = 0;
		char op_code[4+1];
		char login_no[6+1];
		char org_code[9+1];
		int  err_code=0;
		int  ids=0;
		char  op_time[17+1];
		long  cust_id=0;
		char year_month[6+1];
		char total_date[8+1];
		char old_time[17+1];
		int  month_flag=0;
		char op_note[60+1];
		char group_id[10+1];
		char org_id[10+1];
		int  ret=0;      
		tChgList  chgList; 
		long offon_accept=0;
		char ChgReson[256+1];
		char GroupId[10+1];
		char return_message[256];
/*ng������Ӳ���dx 2009��9��30��9:54:17*/
		char sIdNo[14+1];
		char sLoginAccept[14+1];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char vCallServiceName[30+1];
		int vSendParaNum=0;
		int vRecpParaNum=0;
		char vParamList[DLMAXITEMS][2][DLINTERFACEDATA];
		int iOrderRight=100;
/*ng������Ӳ���dx 2009��9��30��9:54:17*/		
       EXEC SQL END DECLARE SECTION;
       
        init(sIdNo);
        init(sLoginAccept);
        init(v_parameter_array);
        init(vCallServiceName);
        init(vParamList);

		init(return_message);
		init(ChgReson);	
		init(run_code);
		strcpy(run_code,InRunCode);
		memset(&chgList,0,sizeof(chgList));
		
		id_no=dCustMsg->id_no;
		cust_id=dCustMsg->cust_id;
		init(phone_no);
		strcpy(phone_no,dCustMsg->phone_no);
		init(sm_code);
		strcpy(sm_code,dCustMsg->sm_code);
		init(attr_code);
		strcpy(attr_code,dCustMsg->attr_code);
		init(belong_code);
		strcpy(belong_code,dCustMsg->belong_code);
		init(old_time);
		strcpy(old_time,dCustMsg->run_time);
		
		login_accept=dLoginMsg->login_accept;
		init(op_code);
		strcpy(op_code,dLoginMsg->op_code);
		init(login_no);
		strcpy(login_no,dLoginMsg->login_no);
		init(org_code);
		strcpy(org_code,dLoginMsg->org_code);
		
		init(org_id);
		init(group_id);
		strcpy(org_id,dLoginMsg->org_id);
		strcpy(group_id,dLoginMsg->group_id);
		
		init(GroupId);
		strcpy(GroupId,dCustMsg->group_id);
		Trim(GroupId);
		
		init(op_note);
		strcpy(op_note,InOpNote);
		
		init(year_month);
		init(op_time);
		init(total_date);
		
		strcpy(op_time,dLoginMsg->op_time);
		
		strncpy(year_month,op_time,6);
		strncpy(total_date,op_time,8);
		
		if(strncmp(old_time,total_date,8)==0)
		{
			month_flag=0;
		}
		else
		{
			month_flag=1;
		}
		/*��֯�������������ֶ�group_id��org_id  edit by zhangxha at 02/03/2009*/
       init(TmpSqlStr);
       sprintf(TmpSqlStr,"insert into wUserMsgChg(op_no,op_type,id_no,cust_id,\
                          phone_no,sm_code,attr_code,belong_code,\
                          run_code,run_time,op_time ,group_id) values( \
                          sMaxBillChg.NEXTVAL,'2',:v1,:v2,:v3,:v4,:v5,:v6,\
                          :v7,to_char(sysdate,'yyyymmddhh24miss'),\
                          to_char(sysdate,'yyyymmddhh24miss'),:v8)");

       EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
       EXEC SQL EXECUTE sql_stmt using :id_no,:cust_id,:phone_no,:sm_code,
                                       :attr_code,:belong_code,:run_code,:GroupId;
       if(SQLCODE!=0)
       {
               printf("\n  TmpSql=[%s]",TmpSqlStr);
               printf("����wUserMsgChg Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
               return  -1;
       }
/*ng���� dx 2009��9��30��9:51:04
       EXEC SQL UPDATE dCustMsg
                SET    run_code=:run_code,
                       run_time=to_date(:op_time,'YYYYMMDD HH24:MI:SS')
                WHERE  id_no=:id_no;
       if(SQLCODE!=0)
       {
               printf("�����û�����SQLCODE[%d]SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
               printf("run_code[%s]op_time[%s]\n",run_code,op_time);
               return -1;
       }
*/
	sprintf(sIdNo,"%ld",id_no);
	Trim(sIdNo);
	strcpy(v_parameter_array[0],run_code);
    strcpy(v_parameter_array[1],op_time);
    strcpy(v_parameter_array[2],sIdNo);
    ret=0;
	ret=OrderUpdateCustMsg(ORDERIDTYPE_USER,sIdNo,100,op_code,login_accept,login_no,"PublicGoStop_ng",sIdNo,
       " run_code = :runCode, run_time = to_date(:opTime, 'yyyymmdd hh24:mi:ss') ","",v_parameter_array);
	printf("------------------------- [%d] ----------------------\n",ret);
	if (ret != 0)
    {    				  
    	printf("�����û�����ret[%d]\n",ret);
        printf("run_code[%s]op_time[%s]\n",run_code,op_time);
        return -1;
    }
    printf("-----[%s]\n",old_time);

	EXEC SQL select sOffOn.nextval into :offon_accept from dual;
	if (SQLCODE != SQL_OK) 
	{
		printf("ȡ��ˮ����[%d]\n",SQLCODE);
		return -1;
	}
	strcpy(ChgReson,op_code);
	Trim(ChgReson);
	
	Trim(belong_code);
	Trim(sm_code);
	Trim(attr_code);
	Trim(phone_no);
	Trim(run_code);
	Trim(org_code);
	Trim(login_no);
	Trim(op_code);
	Trim(old_time);
	
	chgList.vCommand_Id          =  offon_accept;
	chgList.vId_No               =  id_no;
	chgList.vTotal_Date          =  atoi(total_date);
	chgList.vLogin_Accept        =  login_accept;
	
	strncpy(chgList.vBelong_Code,  belong_code, 7);
	strncpy(chgList.vSm_Code,      sm_code,     2);
	strncpy(chgList.vAttr_Code,    attr_code,   8);
	strncpy(chgList.vPhone_No,     phone_no,   15);
	strncpy(chgList.vRun_Code,     run_code,    2);
	strncpy(chgList.vOrg_Code,     org_code,    9);
	strncpy(chgList.vLogin_No,     login_no,    6);
	strncpy(chgList.vOp_Code,      op_code,     4);
	strcpy(chgList.vChange_Reason, ChgReson      );
	strncpy(chgList.vGroup_Id,     GroupId,    10);
	strncpy(chgList.vOrg_Id,       org_id,     10);
	strcpy(chgList.vOld_Time,      old_time      );
	

	
	ret=0;
	ret = recordChgList(&chgList,return_message);
	if(ret != 0)
	{
		printf("���ú���recordChgList����[%s]��",return_message); 
		return -1;
	}	  
 	/*modify --end*/
	if(run_code[0]!='K')
	{
		printf("\ntime .....[%s][%s][%d]",op_time,old_time,month_flag);
/*ng����dx 2009��9��30��10:00:54 begin
		EXEC SQL update dCustStop
			set   stop_times=stop_times+1,
			stop_durations=stop_durations+round((to_date(:op_time,'YYYYMMDD HH24:MI:SS')-to_date(:old_time,'YYYYMMDD HH24:MI:SS'))*3600*24) 
			where  id_no=:id_no;
			printf("\n dCustStop[%ld]",id_no);
                       if (SQLCODE <0)
                       {
                               printf("\nupdate error [%d],[%s]",SQLCODE,SQLERRMSG);
                       }
                       if (SQLCODE==NOTFOUND)
                       {

                       }
*/

		memset(vParamList,0,DLMAXITEMS*2*DLINTERFACEDATA);
		
		strcpy(vCallServiceName,"bodb_GoStop");
		vSendParaNum=3; /* ҵ�񹤵���������������� */
		vRecpParaNum=2;/* ҵ�񹤵���������������� */
		sprintf(sLoginAccept,"%ld",login_accept);
    	
		strcpy(vParamList[0][0],"id_no");
		strcpy(vParamList[0][1],sIdNo);
		
		strcpy(vParamList[1][0],"op_time");
		strcpy(vParamList[1][1],op_time);
		
		strcpy(vParamList[2][0],"old_time");
		strcpy(vParamList[2][1],old_time);
		
		ret=0;
		ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
				                          ORDERIDTYPE_USER,sIdNo,iOrderRight,sLoginAccept,op_code,login_no,"PublicGoStop_ng");			
		printf("----------SetOrderBusiSendCrm ret=[%d]----------\n",ret);
		if (ret != 0) 
		{
			printf("\nupdate error SetOrder bodb_GoStop[%d]\n",ret);
			return -6;
		}
	}
/*ng����dx2009��9��30��10:26:32 end*/	
	return 0;
}

/*******************************************************
����:
	���ֻ����뽻�ѿ��ػ�
���������
	InContractNo       long    Ĭ���ʻ�ID
	InIdNo             long    �û�ID
	*InSmCode          char    Ʒ�ƴ���
	*InRegionCode      char    ��������
	*InOpCode          char    ��������
	InPayedPrepay      double  Ԥ�滮��
	InTotalDate        int     ��������
	InPayAccept        long     ������ˮ
	*InPhoneNo         char    �ֻ�����
	*InOrgCode         char    ��������
	*InWorkNo          char    ��������
	InPayMoney         double  ���ν���
	InStillOwe         double  Ƿ�ѽ��
	InStillRemonth     double  ��������
	InLimitOwe         double  �����޶�
	InNoAccountFee     double  δ���ʻ���
���������
�����룺
	130010 ��ѯ�û���Ϣ�����
	130011 ��ѯ���ѿ��������
	132002 ���뿪�ػ������
*********************************************************/
int PublicSendOffon_ng(long InContractNo, long InIdNo, char *InSmCode, 
					char *InRegionCode, char *InOpCode, double InPayedPrepay,
					int InTotalDate, long InPayAccept, char *InPhoneNo, 
					char *InOrgCode, char *InWorkNo, double InPayMoney, 
					double InStillOwe, double InStillRemonth, double InLimitOwe,
        			double InNoAccountFee,char *InOpen,double InPeFee,char *UpdateRunCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int    CmdRight = 0;
		long    PayAccept = 0;
		int    TotalDate = 0;
		int    RetCode = 0;
		char   SmCode[SMCODELEN+1];
		char   OldRun[RUNCODELEN+1];
		char   CurRun[RUNCODELEN+1];
		char   RunCode[RUNCODELEN+RUNCODELEN+1];
		char   OldTime[14+1];
		char   PayCode[PAYCODELEN+1];
		char   AttrCode[ATTRCODELEN+1];
		char   BelongCode[7+1];
		char   PhoneNo[PHONENOLEN+1];
		char   OrgCode[ORGCODELEN+1];
		char   WorkNo[6+1];
		char   RegionCode[REGIONCODELEN+1];
		char   OpCode[OPCODELEN+1];
		char   UpdateRunTime[14+1];
		char   UpdateRunTime2[17+1];
        char   sysdate[14+1];
		char   TmpSqlStr[TMPLEN+1];
		char   NewRun[RUNCODELEN+1];
		char   AllFeeFlag[1+1];   
		char   ChangeFlag[CHANGEFLAGLEN+1];
		long   offon_accept = 0;
		double PayedPrepay = 0.00;
		double PrepayFee = 0.00;
		double PayMoney = 0.00;
		double StillOwe = 0.00;
		double LimitOwe = 0.00;
		double NoAccountFee = 0.00;
		double StillRemonth = 0.00;
		long   IdNo=0;
		long   OP_NO=0;
		long   CustID=0;
		int    ret=0;      
		tChgList  chgList;
		char   GroupId[10+1]; 
		char   OrgId[10+1];   
		char   ChgReson[256+1];
		char   return_message[256];
		char   vPayMoney[12+1];
		char   vPrepayFee[12+1];
		char   vStillOwe[12+1];
		char   vNoAccountFee[12+1];
		char   vRunFlag[1+1];
		char   vBillFlag[1+1];
		int    MinYm = 0;		
		char   BelongCode2[7+1];
		double OweFee = 0.00;
		long   ConCustid=0;   
		double vPeFee=0.00;  
		char   i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];  
		int    v_ret=0;
		    
	EXEC SQL END DECLARE SECTION;
	
	int RetValue = 0;
	init(SmCode);
	init(OldRun);
	init(CurRun);
	init(RunCode);
	init(OldTime);
	init(PayCode);
	init(AttrCode);
	init(BelongCode);
	init(PhoneNo);
	init(OrgCode);
	init(WorkNo);
	init(AllFeeFlag);
	init(RegionCode);
	init(OpCode);
	init(UpdateRunTime);
	init(UpdateRunTime2);
	init(TmpSqlStr);
	init(sysdate);
	init(GroupId);   
	init(OrgId);     
	init(ChgReson);
	init(return_message);
	init(vPayMoney);
	init(vPrepayFee);
	init(vStillOwe);
	init(vNoAccountFee);
	init(vRunFlag);
	init(vBillFlag);
	init(BelongCode2);
	
	memset(&chgList,0,sizeof(chgList));
	memset(&i_parameter_array,0,DLMAXITEMS*DLINTERFACEDATA);
        
	PayedPrepay = InPayedPrepay;
	PayAccept = InPayAccept;
	PayMoney = InPayMoney;
	
	StillOwe = InStillOwe;
	StillRemonth = InStillRemonth;
	NoAccountFee = InNoAccountFee;
	LimitOwe = InLimitOwe;
	vPeFee=InPeFee;
	
	strcpy(SmCode, InSmCode);
	strcpy(RegionCode, InRegionCode);
	strcpy(OpCode, InOpCode);
	TotalDate = InTotalDate;
	strcpy(PhoneNo, InPhoneNo);
	strcpy(OrgCode, InOrgCode);
	strcpy(WorkNo, InWorkNo);
	IdNo=InIdNo;
	strcpy(InOpen,"N");
	
	/***ȡ�û���״̬�����ػ����ȼ�****/
	init(TmpSqlStr);
	sprintf(TmpSqlStr, "SELECT substr(run_code,1,1),substr(run_code,2,1),\
						run_code,to_char(run_time,'YYYYMMDDHH24MISS'),\
						attr_code,belong_code,cmd_right,cust_id \
					    FROM dCustMsg WHERE id_no=:v1");
	
	EXEC SQL EXECUTE
	BEGIN
    	    EXECUTE IMMEDIATE :TmpSqlStr INTO :OldRun, :CurRun,:RunCode, :OldTime,
	                                          :AttrCode, :BelongCode, :CmdRight,:CustID
	                                          using :IdNo;
    	
	END;
	END-EXEC;

	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicSendOffon>[%s]\n", TmpSqlStr);
		printf("<PublicSendOffon>ȡ�û�״̬����[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
		return  130010;
	}
	
	/***Ĭ���ʻ���Ԥ�����ʽ******/
	/*���˲�ͣӪҵ����  edit by houxue  20090326*/
	/*RetCode = GetAccountMsgOwe(InContractNo,&PrepayFee,PayCode);
	if (RetCode != SQL_OK) 
	{
		return RetCode;
	}
	
	RetCode=PublicBillFlag(vRunFlag,vBillFlag);
	if(RetCode!=0)
	{
		printf("\n�����ʱ�־ err");
		return RetCode;
	}
	Trim(vRunFlag);
	Trim(vBillFlag);
	*/
	/*
	RetCode = GetAccountMsgOwe2(InContractNo,vRunFlag,vBillFlag,&PrepayFee,PayCode,BelongCode2,&OweFee,&MinYm,&ConCustid);
	if (RetCode != SQL_OK) 
	{
		return RetCode;
	}    
	printf("PrepayFee=%12.2f\n",PrepayFee);
	*/
	/*���˲�ͣӪҵ����  end by houxue  20090326*/      
	init(NewRun);
	init(AllFeeFlag);

	/*-- '0' ����Ƿ�� --*/
	/*-- '1' ����Ƿ��+δ���ʻ��� --*/
	/*-- '2' ����Ƿ��+δ���ʻ���+��������� --*/
	/***LimitOwe��δȷ��*****/
	/*PrepayFee = round_F(PrepayFee, 2);*/
	printf("\nCurRun[%s]\n",CurRun);
	printf("\nRunCode[%s]\n",RunCode);
	printf("\nOldCode[%s]\n",OldRun);
	/*printf("\n[%12.2f][%12.2f][%12.2f][%12.2f]",PrepayFee,LimitOwe,StillOwe,NoAccountFee); */
	/*if (round_F(PrepayFee+LimitOwe,2)>=round_F(StillOwe+NoAccountFee,2))*/
	/*���+�ɷ�+�Ż�+������>0*/
	if((vPeFee+InLimitOwe)>0)
	{
		printf("1\n");
		strcpy(ChangeFlag, "1");
		strcpy(NewRun,"A");
		strcpy(InOpen,"Y");
	}
	if ((NewRun[0] == '\0')||(0 == strcmp(ChangeFlag, "0"))) 
	{
		printf("2\n");
		strcpy(UpdateRunCode, RunCode);
		strcpy(UpdateRunTime, OldTime);
		strcpy(InOpen,"N");
	}        
	else 
	{
		printf("3\n");
		sprintf(UpdateRunCode,"%cA",CurRun[0]);

		EXEC SQL SELECT to_char(sysdate,'YYYYMMDDHH24MISS'),to_char(sysdate,'YYYYMMDDHH24MISS'),to_char(sysdate,'YYYYMMDD HH24:MI:SS') 
		           INTO :UpdateRunTime,:sysdate,:UpdateRunTime2 from dual;
		strcpy(InOpen,"Y");
        
	}
	if (NewRun[0] == '\0' || !strcmp(ChangeFlag, "0")) 
	{
		return RET_OK;
	}
	
	/*
	RetValue = PublicUpdateCustMsgRunCode(InIdNo, UpdateRunCode, UpdateRunTime);
	if (RetValue != RET_OK) 
	{
		return RetValue;
	}
	if (NewRun[0] == '\0' || !strcmp(ChangeFlag, "0")) 
	{
		return RET_OK;
	}
	*/
	init(i_parameter_array);
	strcpy(i_parameter_array[0],UpdateRunCode);
	strcpy(i_parameter_array[1],UpdateRunTime2);
	sprintf(i_parameter_array[2],"%ld",IdNo);
	v_ret=0;
	v_ret=OrderUpdateCustMsg("2",i_parameter_array[2],100,OpCode,PayAccept,WorkNo,"bodb_sendOffon_ng",i_parameter_array[2],
	                 "run_code=:RunCode,run_time=to_date(:RunTime,'YYYYMMDD HH24:MI:SS')",
	                 "",i_parameter_array);
	if(0!=v_ret)
	{
		printf("update dCustMsg Error![%d] \n",v_ret);
	    return v_ret;
	}
	
	/*--��֯�������������ֶ�edit by zhangxha at 04/03/2009--begin*/
	ret =0;
	ret = sGetUserGroupid(PhoneNo,GroupId);
	if(ret <0)
	{
		printf("<PublicSendOffon>��ȡ�û�group_idʧ��![%s]\n",PhoneNo);
		return  130021;
	}
	Trim(GroupId);
	ret =0;
	ret = sGetLoginOrgid(WorkNo,OrgId);
	if(ret <0)
	{
		printf("<PublicSendOffon>��ȡ����org_idʧ��![%s]\n",WorkNo);
		return  130022;
	}
	Trim(OrgId);
	/****************************
	��¼wusermsgchg
	****************************/
	/*
	sprintf(TmpSqlStr,"insert into wUserMsgChg (OP_NO,OP_TYPE,ID_NO,\
	                   CUST_ID,PHONE_NO,SM_CODE,ATTR_CODE,BELONG_CODE,\
	                   RUN_CODE,RUN_TIME,OP_TIME) \
	                   values (smaxbillchg.nextval,2,:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,to_char(sysdate,'YYYYMMDDHH24MISS'))");
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :IdNo,:CustID,:PhoneNo,:SmCode,:AttrCode,:BelongCode,:UpdateRunCode,:UpdateRunTime;*/
	 
    EXEC SQL INSERT into wUserMsgChg (OP_NO,OP_TYPE,ID_NO, 
                         CUST_ID,PHONE_NO,SM_CODE,ATTR_CODE,BELONG_CODE,
                         RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID)
                         values (smaxbillchg.nextval,2,:IdNo,:CustID,:PhoneNo,:SmCode,:AttrCode,:BelongCode,:UpdateRunCode,:UpdateRunTime,to_char(sysdate,'YYYYMMDDHH24MISS'),:GroupId);       
	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicSendOffon>: TmpSqlStr=[%s]\n", TmpSqlStr);
		printf("\n[%d],[%s]",SQLCODE,SQLERRMSG);
		return 132002;
	}	
  	/*init(TmpSqlStr);
	sprintf(TmpSqlStr, "INSERT INTO wChgList(command_id,id_no,total_date,\
						login_accept,belong_code,sm_code,attr_code,phone_no,\
						run_code,old_time,op_time,org_code,login_no,op_code,\
						cmd_right,change_reason) \
						VALUES (soffon.nextval,:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,\
						to_date(:v9,'YYYYMMDDHH24MISS'),\
					    sysdate,:v10,:v11,:v12,:v13,\
	                    '����'||to_char(:v14)||'Ԫ[���'||to_char(:v15)||'��Ƿ'||to_char(:v16)||'δ����'||to_char(:v17)||']')");
                   
    EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
    EXEC SQL EXECUTE sql_stmt using :InIdNo, :TotalDate, :PayAccept,:BelongCode, :SmCode, 
                                    :AttrCode, :PhoneNo, :UpdateRunCode, :OldTime, :OrgCode, 
                                    :WorkNo, :OpCode, :CmdRight,:PayMoney,:PrepayFee,
                                    :StillOwe,:NoAccountFee;
       
	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicSendOffon>: TmpSqlStr=[%s][%ld][%s][%s][%s]\n", TmpSqlStr,InIdNo,PhoneNo,UpdateRunCode,SmCode);
		printf("\n[%d],[%s]",SQLCODE,SQLERRMSG);
		return 132002;
	}
	*/   
	/* 
	EXEC SQL select sOffOn.nextval,to_char(:PayMoney),to_char(:PrepayFee),to_char(:StillOwe),
					to_char(:NoAccountFee)
 	           into :offon_accept,:vPayMoney,:vPrepayFee,:vStillOwe,:vNoAccountFee
 	           from dual;
 	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicSendOffon>:ȡ��ˮ����.\n");
		printf("\n[%d],[%s]",SQLCODE,SQLERRMSG);
		return 132005;
	}
	Trim(vPayMoney);
	Trim(vPrepayFee);
	Trim(vStillOwe);
	Trim(vNoAccountFee);
	
  	sprintf(ChgReson, "����%sԪ[���%s��Ƿ%sδ����%s]", vPayMoney,vPrepayFee,vStillOwe,vNoAccountFee);
  	Trim(ChgReson);
  	
  	Trim(BelongCode);
    Trim(SmCode);
    Trim(AttrCode);
    Trim(PhoneNo);
    Trim(UpdateRunCode);
    Trim(OrgCode);
    Trim(WorkNo);
    Trim(OpCode);
    Trim(OldTime);
  	
	chgList.vCommand_Id          =  offon_accept;
	chgList.vId_No               =  InIdNo;
	chgList.vTotal_Date          =  TotalDate;
	chgList.vLogin_Accept        =  PayAccept;
	
	strncpy(chgList.vBelong_Code,   BelongCode,    7);
	strncpy(chgList.vSm_Code,       SmCode,        2);
	strncpy(chgList.vAttr_Code,     AttrCode,      8);
	strncpy(chgList.vPhone_No,      PhoneNo,      15);
	strncpy(chgList.vRun_Code,      UpdateRunCode, 2);
	strncpy(chgList.vOrg_Code,      OrgCode,       9);
	strncpy(chgList.vLogin_No,      WorkNo,        6);
	strncpy(chgList.vOp_Code,       OpCode,        4);
	strcpy(chgList.vChange_Reason,  ChgReson		);
	strncpy(chgList.vGroup_Id,      GroupId,      10);
	strncpy(chgList.vOrg_Id,        OrgId,        10);
    strcpy(chgList.vOld_Time,       OldTime         );
    
	ret=0;
	ret = recordChgList(&chgList,return_message);
	if(ret != 0)
	{
		printf("<PublicSendOffon>:���ú���recordChgList����[%s].\n",return_message);       
		return 132006;
	}
	*/	  
 	/*---��֯�������������ֶ�edit by zhangxha at 02/03/2009---end*/

	/***����dCustStop****/
	/*
	if(CurRun[0]!='K')
	{
			EXEC SQL update  dCustStop
					 set   stop_times=stop_times+1,
						   stop_durations=stop_durations+round(to_date(:sysdate,'YYYYMMDDHH24MISS')-to_date(:OldTime,'YYYYMMDDHH24MISS'))
					 where  id_no=:IdNo;
			printf("\n dCustStop[%ld]",IdNo);
			if (SQLCODE <0) 
			{
				printf("\nupdate error [%d],[%s]",SQLCODE,SQLERRMSG);
				return 132002;
			}
			if (SQLCODE==NOTFOUND)
			{
				
			}	               
	}
	*/
	strcpy(InOpen,"Y");
	return RET_OK;
}

int Query_Con_Owe_ng(long IContractNo,double *OConOweFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int ret=0;
	int	unBillYm=0;
	int sendNum,recvNum=0;
	char buffer1[256],buffer2[256],buffer3[256]; 
	char vContractNo[14+1];
	double vConOweFee = 0.00;
	FBFR32	*sendbuf, *rcvbuf;
    FLDLEN32  sendlen,rcvlen;
 	EXEC SQL END DECLARE SECTION;
 	
 	init(buffer1);init(buffer2);init(buffer3);
 	init(vContractNo);	
 	pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "-------------- Query_Con_Owe_ng begin --------------");
 	/*���������������*/
	sendNum = 1;
	recvNum = 3;
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng,��ʼ�����뻺����ʧ��!");
		return 100110;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"6",(FLDLEN32)0);	
						
	sprintf(vContractNo,"%ld",IContractNo);		
	Trim(vContractNo);
	Fchg32(sendbuf,GPARM32_0,0,vContractNo,(FLDLEN32)0);
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);			
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng,��ʼ�����������ʧ��!");
		return 100111;
	}
	ret = fPubCallSrv(CALLSERV_TP,"bs_qryConMsg",sendbuf,&rcvbuf);

	if(ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng call bs_qryConMsg failed ret=[%d]!!!",ret);
		return 100112;
	}
	
    Fget32(rcvbuf,GPARM32_0,0,buffer1,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_1,0,buffer2,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_5,0,buffer3,(FLDLEN32)0);
    if(strncmp(buffer1,"000000",6) != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng call bs_qryConMsg failed buffer1=[%s]!!!",buffer1);
		return 100113;
	}
	Trim(buffer3);
	vConOweFee = atof(buffer3);

	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	*OConOweFee = vConOweFee;
	pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "-------------- Query_Con_Owe_ng end --------------");
 	return 0;
}

int Query_Con_Pre_ng(long IContractNo, char *IPayType, double *OConPreFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int ret=0;
	int	unBillYm=0;
	int sendNum,recvNum=0;
	char buffer1[256],buffer2[256],buffer3[256]; 
	char vContractNo[14+1];
	char vPayType[1+1];
	double vPrepayFee = 0.00;
	FBFR32	*sendbuf, *rcvbuf;
    FLDLEN32  sendlen,rcvlen;
 	EXEC SQL END DECLARE SECTION;
 	
 	init(buffer1);init(buffer2);init(buffer3);
 	init(vContractNo);init(vPayType);
 	sprintf(vContractNo,"%ld",IContractNo);
 	Trim(vContractNo);
 	strncpy(vPayType,IPayType,1);
 	
 	pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "-------------- Query_Con_Pre_ng begin --------------");
 	/*�����������*/
	sendNum = 2;
	/*�����������*/
	recvNum = 3;
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng,��ʼ�����뻺����ʧ��!");
		return 100120;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"2",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);
	
	Fchg32(sendbuf,GPARM32_0,0,vContractNo,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_1,0,vPayType,(FLDLEN32)0);
	
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);
	
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng,��ʼ�����������ʧ��!");
		return 100121;
	}
	
	ret = fPubCallSrv(CALLSERV_TP,"bs_queryConPre",sendbuf,&rcvbuf);
	if(ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng call bs_queryConPre failed ret=[%d]\n !!!",ret);
		return 100123;
	}
	
	Fget32(rcvbuf,SVC_ERR_NO32,0,buffer1,(FLDLEN32)0);
	Fget32(rcvbuf,SVC_ERR_MSG32,0,buffer2,(FLDLEN32)0);
	
	Trim(buffer1);
	Trim(buffer2);					
	if ((strcmp(buffer1,"000000") != 0) && (strcmp(buffer1,"140300") != 0))
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng call bs_queryConPre failed buffer1=[%s]!!!",buffer1);
		return 100124;
	}
		
	Fget32(rcvbuf,GPARM32_2,0,buffer3,(FLDLEN32)0);
	
	Trim(buffer3);
	vPrepayFee = atof(buffer3);
	pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng call bs_queryConPre,vPrepayFee =[%.2f]!!!",vPrepayFee);

	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	*OConPreFee = vPrepayFee;
	pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "-------------- Query_Con_Pre_ng end --------------");
 	return 0;
}
/************************************************************************
��������:adcAddPlusMode
����ʱ��:2007/03/20
������Ա:chenxuan
��������:ADC��Ա�������븽���ʷѺ���
�������:
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int adcAddPlusMode(char *iPhoneNo, char *iNewMode, char *iLoginNo, char *iOpCode,	char *iOpTime,
	char *iLoginAccept, char *iRegionCode, char *oRetMsg)
{
	char retCode[6+1];
	char retMsg[256];

	EXEC SQL BEGIN DECLARE SECTION;
		int  icount;
    char vIdNo[22+1];
    char vBelongCode[7+1];
    char vRegionCode[2+1];
    char vDistrictCode[2+1];
    char vSmCode[2+1];
    char vNewSmCode[2+1];
		char temp_buf[100];
	EXEC SQL END DECLARE SECTION;
  /* ��ѯ�ֻ�����Ļ�����Ϣ */
  EXEC SQL select to_char(id_no), belong_code, substr(belong_code,1,2), substr(belong_code,3,2), sm_code
        into :vIdNo, :vBelongCode, :vRegionCode, :vDistrictCode, :vSmCode
    from dCustMsg where phone_no = :iPhoneNo and substr(run_code,2,1) < 'a';
	if (SQLCODE != 0) {
		sprintf(oRetMsg, "��ǰ�û�[%s]�Ĳ��������븽���ײ�!", iPhoneNo);
		return 194020;
	}

  /* ��ѯ���ʷ��Ƿ���� */
  EXEC SQL select sm_code INTO :vNewSmCode from sBillModeCode
  	where region_code = :iRegionCode and mode_code = :iNewMode;
  if (SQLCODE != 0) {
  	sprintf(oRetMsg, "�����ײ���BOSSϵͳ��δ����[%s]", iNewMode);
  	return 194021;
  }

  EXEC SQL SELECT count(1) INTO :icount FROM dbillcustdetail WHERE id_no=to_number(:vIdNo)
  				AND	mode_code=:iNewMode and mode_time='Y' and mode_status='Y'
  				AND	end_time > sysdate;
  if (SQLCODE != 0)
  {
  	sprintf(oRetMsg, "��ѯ�û��ײ� ERROR[%s]", iNewMode);
  	return 194091;
  }

  if(icount > 0)
  {
  	sprintf(oRetMsg, "�û�[%s]�ѿ�ͨ���ײ�",iPhoneNo);
  	return 194099;
  }

  /* ���븽���ʷ�  */
	sprintf(retCode, "%06d", pubBillBeginMode(iPhoneNo, iNewMode, iOpTime, iOpCode, iLoginNo,
		iLoginAccept, iOpTime, "0", "", "", temp_buf));
	printf("retCode is %s\n,temp_buf is %s\n",retCode,temp_buf)	;
  if (strcmp(retCode, "000000") != 0) {
		sprintf(oRetMsg,"���븽���ײ�[%s]%s", iNewMode, temp_buf);
    return 194022;

  }

	return 0;
}

/************************************************************************
��������:adcDelPlusMode
����ʱ��:2007/03/20
������Ա:chenxuan
��������:ADC��Աȡ�����븽���ʷѺ���
�������:
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int adcDelPlusMode(char *iPhoneNo, char *iIdNo, char *iOldMode, char *iLoginNo, char *iOpCode,
	char *iOpTime, char *iLoginAccept, char *oRetMsg)
{
	char retCode[6+1];
	char retMsg[256];

	EXEC SQL BEGIN DECLARE SECTION;
    char vOldAccept[14+1];
		char temp_buf[100];
		char dynStmt[2048];
	EXEC SQL END DECLARE SECTION;

	init(vOldAccept);
	init(temp_buf);
	init(dynStmt);
	printf("!!!ȡ��ADC�����ײ�:iOldMode is [%s]\n",iOldMode);
	/* ȡԭ�����ײ͵���ˮ */
	/*chendx 20121218 update ����˰���ת�����ײ��޷���ȡ��ˮ����*/
	sprintf(dynStmt, "select distinct login_accept from dBillCustDetail%c where "
		" mode_code = :v1 and id_no = :v2 and end_time > last_day(sysdate) + 1", iIdNo[strlen(iIdNo)-1]);
	EXEC SQL prepare pre from :dynStmt;
	EXEC SQL declare cur cursor for pre;
	EXEC SQL open cur using :iOldMode, :iIdNo;
	EXEC SQL fetch cur into :vOldAccept;
	EXEC SQL close cur;
	if (SQLCODE != 0) {
		sprintf(oRetMsg, "��ѯADC�����ײ�ʧ��!");
		return 194030;
	}

  /* ȡ��ADC�����ײ�  */
  printf("ȡ��ADC�����ײ�:iOldMode is [%s]vOldAccept[%s]\n",iOldMode,vOldAccept);
	sprintf(retCode, "%06d", pubBillEndMode(iPhoneNo, iOldMode, vOldAccept, iOpTime,
		iOpCode, iLoginNo, iLoginAccept, iOpTime, temp_buf));
	if (strcmp(retCode, "000000") != 0) {
		sprintf(oRetMsg, "ȡ����ǰ�����ײ�[%s]ʱ%s", iOldMode, temp_buf);
		return 194031;
	}

	return 0;
}
