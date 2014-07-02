/**********************************************
 @wt PRG  : publicNG.cp
 @wt EDIT : dx
 @wt DATE : 2009-9-19
 @wt FUNC : 解耦使用的一些公共函数，在原函数基础上添加send_type和send_id字段，
 			以保持同一个业务中工单发送一致性。
 *********************************************/


#include "boss_srv.h"
#include "publicsrv.h"
#include "contact.h"
#include "pubLog.h"


/*以下业务代码取决于sSmProduct表的sm_code*/
#define VPMN_SM_CODE           "01" /*VPMN产品业务的品牌代码*/
#define BOSSFAV_SM_CODE        "23" /*BOSS优惠专线业务的品牌代码*/
#define ZHUANX_IP_SM_CODE      "04" /*IP专线业务的品牌代码*/
#define ZHUANX_NET_SM_CODE     "11" /*互联网专线业务的品牌代码*/
#define ZHUANX_DIANLU_SM_CODE  "13" /*电路出租业务的品牌代码*/

#define _DEBUG_
EXEC SQL INCLUDE SQLCA;

/**
* 创建帐户档案信息函数
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   工单send_type char *send_type
* @inparam   工单send_id	   char *send_id
* @inparam   帐户档案结构体   tAccDoc *doc

*
* @inparam   操作流水         doc->vLogin_Accept             char(14)
* @inparam   帐户ID           doc->vContract_No              char(14)
* @inparam   客户ID           doc->vCon_Cust_Id              char(14)
* @inparam   帐户密码         doc->vContract_Passwd          char(8)
* @inparam   帐户名称         doc->vBank_Cust                char(60)
* @inparam   帐户零头         doc->vOddment;                 float
* @inparam   归属代码         doc->vBelong_Code              char(7)
* @inparam   预付费           doc->vPrepay_Fee;              float               
* @inparam   帐户信誉度       doc->vAccount_Limit            char(1)
* @inparam   帐户状态         doc->vStatus                   char(1)
* @inparam   邮寄标志         doc->vPost_Flag                char(1)
* @inparam   押金             doc->vDeposit;                 float
* @inparam   帐户欠费         doc->vOwe_Fee;                 float
* @inparam   帐户积分         doc->vAccount_Mark;            int
* @inparam   付款方式         doc->vPay_Code                 char(1)
* @inparam   银行代码         doc->vBank_Code                char(5)
* @inparam   局方银行代码     doc->vPost_Bank_Code           char(5)
* @inparam   帐号             doc->vAccount_No               char(30)
* @inparam   帐户类型         doc->vAccount_Type             char(1)
* @inparam   开始日期         doc->vBegin_Time               char(17)
* @inparam   结束日期         doc->vEnd_Time                 char(17)
* @inparam   操作代码         doc->vOp_Code                  char(4)
* @inparam   操作工号         doc->vLogin_No                 char(6)
* @inparam   机构编码         doc->vOrg_Code                 char(9)
* @inparam   操作备注         doc->vOp_Note                  char(60)
* @inparam   系统备注         doc->vSystem_Note              char(60)
* @inparam   IP_ADDR          doc->vIp_Addr                  char(15)
* @inparam   节点标识         doc->vGroup_Id                 char(10)
* @inparam   更新类型         doc->vUpdate_Type              char(1)
*
* @outparam  返回错误信息   retMsg   char(60)
*
* @return        0:  成功
* @return        3:  插入帐户信息历史表失败[dConMsgHis];
* @return        4:  插入帐户信息表失败[dConMsg];
*/
int createConMsg_ng(char *send_type,char *send_id,tAccDoc *Acc_Cust,char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sql_str[2048];                        /* 存储动态SQL语句串 */
	char totalDate[8+1];                   /* 帐务日期                  */
	char tmp_string[17+1];                 /* 存储日期格式          */
	char yearMonth[6+1];
	int  ret=0;
	
	/*ng解耦 begin*/
	TdConMsg vTdConMsg;
	char vOddment[5+1];
	char vPrepay_Fee[17+1];
	char vDeposit[17+1];
	char vOwe_Fee[17+1];
	char vAccount_Mark[10+1];
	/*ng解耦 end*/
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
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg操作流水         =[%s]",     Acc_Cust->vLogin_Accept);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户ID           =[%s]",     Acc_Cust->vContract_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg客户ID           =[%s]",     Acc_Cust->vCon_Cust_Id);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户密码         =[%s]",     Acc_Cust->vContract_Passwd);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户名称         =[%s]",     Acc_Cust->vBank_Cust);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户零头         =[%12.2f]", Acc_Cust->vOddment);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg归属代码         =[%s]",     Acc_Cust->vBelong_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg预付费           =[%12.2f]", Acc_Cust->vPrepay_Fee);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户信誉度       =[%s]",     Acc_Cust->vAccount_Limit);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户状态         =[%s]",     Acc_Cust->vStatus);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg邮寄标志         =[%s]",     Acc_Cust->vPost_Flag);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg押金             =[%12.2f]", Acc_Cust->vDeposit);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户欠费         =[%12.2f]", Acc_Cust->vOwe_Fee);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户积分         =[%d]",     Acc_Cust->vAccount_Mark);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg付款方式         =[%s]",     Acc_Cust->vPay_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg银行代码         =[%s]",     Acc_Cust->vBank_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg局方银行代码     =[%s]",     Acc_Cust->vPost_Bank_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐号             =[%s]",     Acc_Cust->vAccount_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg帐户类型         =[%s]",     Acc_Cust->vAccount_Type);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg开始日期         =[%s]",     Acc_Cust->vBegin_Time);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg结束日期         =[%s]",     Acc_Cust->vEnd_Time);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg操作代码         =[%s]",     Acc_Cust->vOp_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg操作工号         =[%s]",     Acc_Cust->vLogin_No);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg机构编码         =[%s]",     Acc_Cust->vOrg_Code);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg操作备注         =[%s]",     Acc_Cust->vOp_Note);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg系统备注         =[%s]",     Acc_Cust->vSystem_Note);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msgIP_ADDR          =[%s]",     Acc_Cust->vIp_Addr);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg节点标识         =[%s]",     Acc_Cust->vGroup_Id);
	pubLog_Debug(_FFL_, "createConMsg", "", "debug msg更新类型         =[%s]",     Acc_Cust->vUpdate_Type);
#endif


strncpy(yearMonth,Acc_Cust->vBegin_Time,6);
strncpy(totalDate,Acc_Cust->vBegin_Time,8);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "", " createConMsg 插入dConMsgHis表");
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
	        pubLog_Debug(_FFL_, "createConMsg", "插入dConMsgHis表出错!", "错误代码SQLCODE=[%d],错误信息＝[%s]",SQLCODE,SQLERRMSG);
	        strcpy(retMsg,"插入dConMsgHis表出错!");
	        return 3;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createConMsg", "", " 插入  dConMsg表");
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

	/*ng解耦 数据工单改造 屏蔽原有程序 by wxcrm at 20090903 begin
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
		pubLog_Debug(_FFL_, "createConMsg", "插入dConMsg表出错!", "错误代码SQLCODE=[%d],错误信息＝[%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"插入dConMsg表出错!");
		return 4;
	}
	ng解耦 数据工单改造 屏蔽原有程序 by wxcrm at 20090903 end*/
	/*ng解耦 数据工单改造 by wxcrm at 20090903 begin*/
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
		pubLog_Debug(_FFL_, "createConMsg", "插入dConMsg表出错!", "错误代码SQLCODE=[%d],错误信息＝[%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"插入dConMsg表出错!");
		return 4;
	}
	/*ng解耦 数据工单改造 by wxcrm at 20090903 end*/
	return 0;
}

/**
 *     积分冲正函数，按照帐户进行对奖
* @inparam
* @inparam   工单send_type char *send_type
* @inparam   工单send_id	   char *send_id
 	sBackLoginAccept	冲正流水
 	iBackYearMonth		冲正年月
 	sLoginAccept		操作流水
 	sLoginNo			操作工号
 	sOpCode				操作代码
 	sOpTime				操作时间
 	iSysYearMonth		系统年月

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
	int 	iFindFlag=0;        /*表示是否找到积分兑换记录*/
	char tmp_buf[2048];
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	/*ng解耦 by yaoxc end*/

	init(sContractNo);
	init(sDetailCode);
	init(sBakeContractNo);

	/*******进行透支的冲正,可能没有变更明细只有透支********/
	/***********一个流水只有一个透支记录，所以不用判断输入年月*************/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","***********检查透支有没有被冲销，如果被冲销过则不能做冲正！************");
#endif

	/*根据输入流水号查找积分透支表*/
	EXEC SQL	select owe_mark   ,owe_ym  ,to_char(contract_no),  payed_owe_mark
	              into :fOweMark, :iOweYm,:sContractNo, :fPayedOweMark
	              from dCustOweMark
	             where owe_accept=to_number(:sBackLoginAccept);

	if(SQLCODE==1403)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","需要到积分透支历史表中查询[%d]",SQLCODE);
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
			pubLog_Debug(_FFL_,"","","积分透支历史表中无透支信息[%d]", SQLCODE);
#endif
		}
		else if(SQLCODE == 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","积分透支历史表中有透支信息[%d]!",SQLCODE);
#endif
		}
		else
		{
			strcpy(retCode,"376926");
			sprintf(retMsg,"查询积分透支历史表时发生错误[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE preMarkAwardBackCur;
			return -1;
		}
		EXEC SQL CLOSE preMarkAwardBackCur;
	}
	else if(SQLCODE!=0)
	{
		strcpy(retCode,"376919");
		strcpy(retMsg,"查询积分透支表时发生错误\n");
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","在积分透支表中找到记录\n");
#endif
	}

    Trim(sContractNo);

	if (fPayedOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","用户透支的积分已经被做过冲销，不容许再冲正!\n");
#endif
		strcpy(retCode,"376919");
		strcpy(retMsg,"用户透支的积分已经被做过冲销，不容许再冲正!\n");
		return -1;
	}
	else if (fOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","修改积分透支表!\n");
#endif
		/*冲销完毕，将记录移到积分透支历史表*/
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
			sprintf(retMsg, "冲销完毕，将记录移到积分透支历史表失败！[SQLCODE:%d]", SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

		EXEC SQL delete from dCustOweMark where Owe_Accept=to_number(:sBackLoginAccept);
		if (SQLCODE != 0)
		{
			strcpy(retCode,"3769395");
			sprintf(retMsg,"删除积分透支表记录出错[%d][%s]",SQLCODE,SQLERRMSG);
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

	/*积分变换类型为1代表此类型是积分兑换*/
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
			sprintf(retMsg,"取帐户积分信息出错[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		iFindFlag = 1;

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","获得的该细则产生的积分变化为%lf",fChangeValue);
		pubLog_Debug(_FFL_,"","","当前帐户为%s,备份帐户为[%s]",sContractNo,sBakeContractNo);
#endif
		if (strcmp(sBakeContractNo, sContractNo) != 0)
		{
			if(sBakeContractNo[0] != '\0')
			{
				pubLog_Debug(_FFL_,"","","多帐户兑奖冲正出错\n");
				strcpy(retCode,"376915");
				sprintf(retMsg,"多帐户兑奖冲正出错");
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
				sprintf(retMsg,"从帐户积分表中查询帐户总积分出错[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkAwardBack2;
				return -1;
			}
			else
			{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","查询到新帐户总积分为%lf,最早积分变更日期%d",fCurrentPoint,iMinYm);
#endif
			}

			iCurrMinYm=iMinYm;
			init(sBakeContractNo);
			strcpy(sBakeContractNo,sContractNo);
			fAccCount=0;               /*临时帐户清空*/
			fCurrentTotalValue=fCurrentPoint;
		}
		else
		{
			/*表示contract_no和sBakeContractNo相同，已经处理过了。*/
		}

		/*查询当前年月帐户积分变更明细表中，同操作流水，帐户id,二级积分科目，积分帐户年月下最大的帐户积分变更序号*/
		pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积分变更明细表中最大的积分变更序号",iSysYearMonth);
		Trim(sContractNo);
		Trim(sDetailCode);

		/*判断最小年月*/
		if (iMinYm > iBillYm)
		{
			iCurrMinYm = iBillYm;
		}
		/***lula增加*******/
		iMinYm=iCurrMinYm;
		/******lula增加****/
		init(table_name);
		sprintf(table_name,"dMarkChangeDetail%6d",iSysYearMonth);
		pubLog_Debug(_FFL_,"","","数据为[%s],[%s],[%s],[%d]\n",sLoginAccept,sContractNo,sDetailCode,iBillYm);
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
			pubLog_Debug(_FFL_,"","","没有找到该条件下变更序号,初始数值设置成[1]");
			EXEC SQL CLOSE curMarkAwardBack3;
		}
		else if(SQLCODE==0)
		{
			iChangeSeq=iChangeSeq+1;
			EXEC SQL CLOSE curMarkAwardBack3;
			pubLog_Debug(_FFL_,"","","当前设置的变更序号为%d",iChangeSeq);
		}
		else
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","查找变更序号时，发生错误[%d]",SQLCODE);
#endif
			sprintf(retMsg,"查询最大积分变更序号[%d]\n",SQLCODE);
			EXEC SQL CLOSE curMarkAwardBack3;
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","向当前系统时间下的积分变更明细表中插入当前操作的结果");
#endif
		/*******lula修改********/
		temp1=fAfterDetailValue+fChangeValue;
		temp2=fCurrentTotalValue+fChangeValue;
		/*******lula修改********/
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
			sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}
		fCurrentTotalValue=temp2;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","向积分变更明细表中插入数据成功\n");
#endif
		fAccCount+=fChangeValue;

		pubLog_Debug(_FFL_,"","","contract_no=%s;sDetailCode=%s\n",sContractNo,sDetailCode);
		pubLog_Debug(_FFL_,"","","更新%d年月的帐户积分科目明细表\n",iBillYm);
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
			pubLog_Debug(_FFL_,"","","成功对帐户科目明细表更新数值\n");
		}
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"更新帐户科目明细表可用积分失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		pubLog_Debug(_FFL_,"","","选择积分科目表下该二级科目下的最小年月\n");
		EXEC SQL	select min_ym
		              into :iDetailMinYm
		              from dConMarkDetail
		             where contract_no = to_number(:sContractNo)
		               and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"选择积分科目表下该二级科目下的最小年月失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		if( iDetailMinYm>iBillYm )
		{
			iDetailMinYm = iBillYm;
		}

		pubLog_Debug(_FFL_,"","","更新积分科目表\n");
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
			sprintf(retMsg,"更新积分科目表失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}
	}
	EXEC SQL CLOSE curMarkAwardBack2;
	/*游标关闭，将最后的帐户下的数据写到帐户积分表中*/

	if (sBakeContractNo[0] == '\0')
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","在帐户积分变更明细表中没有找到当前流水\n");
#endif
		/*
		strcpy(retCode,"376918");
		strcpy(retMsg,"帐户积分变更明细表中没有找到当前流水");
		return -1;
		*/
	}
	else
	{
/*****
fAccCount是从dMarkChangeDetail表取的，不包含透支积分，fOweMark为透支积分
*****/	/*ng解耦
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
			sprintf(retMsg,"更新帐户积分表出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
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
			sprintf(retMsg,"更新帐户积分表出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","将累积计算的积分%lf加到该帐户%s的积分表中，成功\n",fAccCount,sBakeContractNo);
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
			sprintf(retMsg,"更新帐户科目明细表可用积分失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}
	return 0;
}


/**积分按帐户或帐户下积分科目扣减函数
 * @inparam iDeductType 调整类型： '4'积分调整、'5'转赠或'7'积分扣除

 	lContractNo			积分帐户
 	sDetailCodeIn		积分二级科目  按科目扣减时不能为NULL
 	sChangeType			变更类型
 	fTotalCount			扣减积分
 	lLoginAccept		操作流水
 	sLoginNo			  操作工号
 	sOpCode				  操作代码
 	sOpTime				  操作时间
 	iSysYearMonth		系统时间

 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

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
	double  fPreMark=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];

	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	init(sContractNo);
	/*ng解耦 by yaoxc end*/

	init(sContractStatus);
	init(sDetailCode);
	init(sTableName);

	/*根据帐户查找当前积分， 透支积分额度，已透支积分，已经消费积分，帐户状态,最早消费年月*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_point, owe_limit, owe_point, total_used, to_char(contract_status), min_ym
	              into :fCurrentPoint,:fOweLimit,:fOwePoint,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConMark
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125058");
		sprintf(retMsg,"查询帐户积分信息错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fCurrentPoint=%lf\n",fCurrentPoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOweLimit=%lf\n",fOweLimit);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOwePoint=%lf\n",fOwePoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息iMinYearMonth=%d\n",iMinYearMonth);
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
			sprintf(retMsg,"查询帐户积分科目积分错误[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		if (fCurrentPoint < fTotalCount)
		{
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","帐户积分科目的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
	#endif
			strcpy(retCode,"1000");
			sprintf(retMsg,"帐户积分科目的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
			return -1;
		}
	}
	else if (fCurrentPoint < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户积分的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
#endif
		strcpy(retCode,"1000");
		sprintf(retMsg,"帐户积分的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
		return -1;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"125060");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
#endif
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}

	if(fOwePoint>0.00)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","该账户已经发生透支，请确认");
#endif
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************积分兑换开始**********************\n");
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
			pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]\n",
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
			pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]\n",
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
		while(fTotalCountRemain > 0)/*游标选取正确*/
		{
			EXEC SQL fetch curMarkDeduct1 into :fRemainMark,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","取动态表格%s错误,由于当前表被取空，错误代码是%d\n",sTableName,SQLCODE);
				break;
			}
			Trim(sDetailCode);
			if(fRemainMark==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","从%s中取得的fRemainMark=%lf sDetailCode=%s\n",sTableName,fRemainMark,sDetailCode);
#endif
			fPreMark=fRemainMark;                /*记录此次游标位置的积分*/
			if(fRemainMark>=fTotalCountRemain)         /*游标积分大于当前交易需要的积分*/
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
				sprintf(retMsg,"更新帐户积分科目明细表时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标积分更新成功\n");
			pubLog_Debug(_FFL_,"","","当前游标位置剩余的积分数值为%lf\n",fRemainMark);
			pubLog_Debug(_FFL_,"","","该游标消费的积分数量为%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积分变更明细表中最大的积分变更序号\n",iSysYearMonth);
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
				pubLog_Debug(_FFL_,"","","没有找到积分变更明细表变更序号,初始数值设置成[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","设置积分变更明细表变更之后序号为%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"125062");
				sprintf(retMsg,"查询最大积分变更序号错误[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct2;
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			EXEC SQL CLOSE curMarkDeduct2;
			pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积分变更明细表中dMarkChangeDetail\n");
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
				sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","向积分变更明细表中插入数据成功");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","更新帐户积分科目表");
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
				sprintf(retMsg,"更新帐户积分科目表失败[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标大,交易结束更新帐户积分科目标成功");
			pubLog_Debug(_FFL_,"","","交易结束,直接跳转到关闭游标");
#endif
		}
		EXEC SQL CLOSE curMarkDeduct1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","单表循环结束，取下一张表格的年月为%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while操作正常结束１.没有透支积分2.发生透支积分");
	pubLog_Debug(_FFL_,"","","当前年月为%d",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","当前年月为%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","更新帐户表前的检查,total_point=%f\n",fTotalCount);
#endif
	if(sChangeType[0]=='9')/*如果“积分转赠”积分扣减时，不扣减月积分和年积分和累记积分*/
	{
		if(strcmp(sOpCode,"2418")==0)
		{	/*ng解耦
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
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
				return -1;
			}
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
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
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	
	 	}
	 	else
	 	{
			if(strcmp(sOpCode,"2419")==0)
			{	/*ng解耦
				EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
								 add_point=add_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
				ng解耦*/
    			/*ng解耦 by yaoxc begin*/
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
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng解耦 by yaoxc end*/
		 	}	 
		 	else{
		 		/*ng解耦		
		 		EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
				ng解耦*/
    			/*ng解耦 by yaoxc begin*/
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
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]\n", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng解耦 by yaoxc end*/
			}
	 	}
	}
	else if(sChangeType[0]=='8')/*单独的做积分调整时，扣减积分变更时扣减月积分和年积分和累记积分*/
	{	/*ng解耦
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
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
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
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/

	}
	else/*其他服务扣积分时，不扣减月积分和年积分和累记积分。记录使用积分*/
	{	/*ng解耦
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
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
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
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/
	}
	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","正常结束后更新帐户积分数据成功fTotalCountRemain=%f",fTotalCountRemain);
#endif

	if(fTotalCountRemain > 0.005)/*发生积分透支*/
	{
		strcpy(retCode,"125066");
		sprintf(retMsg,"发生积分透支错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	return 0;
}

/**
* 创建用户信息函数
*
* @author        zhangzhe@20090220
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   客户档案结构体   tUserDoc *doc
*
* @inparam   客户Id                   doc->vCust_Id                   	long
* @inparam   用户ID                   doc->vId_No                       long
* @inparam   默认帐户                 doc->vContract_No                 long
* @inparam   帐户数量                 doc->vIds                         long
* @inparam   服务号码                 doc->vPhone_No                  	char(15)
* @inparam   业务代码                 doc->vSm_Code                   	char(2)
* @inparam   服务类型                 doc->vService_Type          		char(2)
* @inparam   客户属性代码             doc->vAttr_Code                 	char(8)
* @inparam   用户密码                 doc->vUser_Passwd           		char(8)
* @inparam   入网时间                 doc->vOpen_Time                 	char(17)
* @inparam   归属代码                 doc->vBelong_Code           		char(7)
* @inparam   信誉度                   doc->vLimit_Owe                 	float
* @inparam   信誉等级                 doc->vCredit_Code           		char(1)
* @inparam   信誉等级阀值             doc->vCredit_Value          		int
* @inparam   运行状态                 doc->vRun_Code                  	char(2)
* @inparam   状态时间                 doc->vRun_Time                  	char(17)
* @inparam   计费日期                 doc->vBill_Date                 	char(17)
* @inparam   出帐周期                 doc->vBill_Type                 	int
* @inparam   加密字段                 doc->vEncrypt_Prepay        		char(16)
* @inparam   开关机权限值             doc->vCmd_Right                 	int
* @inparam   上次出帐周期             doc->vLast_Bill_Type        		char(1)
* @inparam   备用字段                 doc->vBak_Field                 	char(12)
* @inparam   操作流水                 doc->vLogin_Accept          		long
* @inparam   操作时间                 doc->vOp_Time                   	char(17)
* @inparam   帐务日期                 doc->vTotal_Date                	int
* @inparam   操作工号                 doc->vLogin_No                  	char(6)
* @inparam   操作代码                 doc->vOp_Code                   	char(4)
* @inparam   归属节点                 doc->vGroup_Id                  	char(10)
* @inparam   区域代码                 doc->varea_code                 	char(10)
*
* @outparam  返回错误信息   retMsg   char(60)
*
* @return        0:  成功
* @return        1:  插入用户主档信息历史表[dCustMsgHis]失败
* @return        2:  插入用户主档信息表[dCustMsg]失败
*
*/
int createCustMsg_ng(char *send_type,char *send_id,tUserDoc *doc,char *retMsg)
{
	char vSQL_Text[4096+1];                 /* SQL语句              */
	char vTmp_String[4096+1];               /* 临时变量             */
	char vOp_Type[1+1];                             /* 操作类型             */
	char vOp_No[14+1];                      /* 帐务接口流水                 */
	int  ret_code=0;                        /* 返回代码             */
	char vReturn_Msg[255+1];                /* 返回信息             */
	long pcust_id=0;
	int iCount = 0;

	/*ng 解耦*/
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
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 客户Id                 = [%ld]",             doc->vCust_Id        );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 用户ID                 = [%ld]",             doc->vId_No           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 默认帐户               = [%ld]",             doc->vContract_No       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 帐户数量               = [%ld]",             doc->vIds                );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 服务号码               = [%s]",              doc->vPhone_No          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 业务代码               = [%s]",              doc->vSm_Code           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 服务类型               = [%s]",              doc->vService_Type      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 客户属性代码           = [%s]",              doc->vAttr_Code         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 用户密码               = [%s]",              doc->vUser_Passwd       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 入网时间               = [%s]",              doc->vOpen_Time         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 归属代码               = [%s]",              doc->vBelong_Code       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 信誉度                 = [%12.2f]",          doc->vLimit_Owe         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 信誉等级               = [%s]",              doc->vCredit_Code       );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 信誉等级阀值           = [%d]",              doc->vCredit_Value      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 运行状态               = [%s]",              doc->vRun_Code          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 状态时间               = [%s]",              doc->vRun_Time          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 计费日期               = [%s]",              doc->vBill_Date         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 出帐周期               = [%d]",              doc->vBill_Type         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 加密字段               = [%s]",              doc->vEncrypt_Prepay    );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 开关机权限值           = [%d]",              doc->vCmd_Right         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 上次出帐周期           = [%s]",              doc->vLast_Bill_Type    );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 备用字段               = [%s]",              doc->vBak_Field         );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 操作流水               = [%ld]",             doc->vLogin_Accept      );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 操作时间               = [%s]",              doc->vOp_Time           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 帐务日期               = [%d]",              doc->vTotal_Date        );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 操作工号               = [%s]",              doc->vLogin_No          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 操作代码               = [%s]",              doc->vOp_Code           );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 归属节点               = [%s]",              doc->vGroup_Id          );
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 区域代码               = [%s]",              doc->varea_code          );
#endif

	/* 吉林属地化版本，dcustmsg的groupmsg字段，若doc->varea_code非group_id类型节点，则自动赋值*/
	EXEC SQL SELECT COUNT(*)
			 INTO :iCount
			 FROM dChnGroupMsg
			 WHERE group_id = :doc->varea_code
			 AND class_code in('13','1159','14');
	if (SQLCODE != 0)
	{
		sprintf(vReturn_Msg,"取group_id失败,SQLCODE=[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "createCustMsg", "取属地化信息失败,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
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
			sprintf(vReturn_Msg,"取group_id失败,SQLCODE=[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "createCustMsg", "取属地化信息失败,", "SQLCODE=[%d]",SQLCODE);
			return 3;
		}
	}
	else
	{
		strcpy(vAreaStr,doc->varea_code);
	}
	Trim(vAreaStr);
	
	pubLog_Debug(_FFL_, "createCustMsg", "", "debug msg: 区域代码修正为       = [%s]",             vAreaStr        );
	
   	strcpy(vOp_Type,        "a");


	/* 插入用户主档信息历史表dCustMsgHis*/

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
		sprintf(vReturn_Msg,"插入用户主档信息历史表[dCustMsgHis]表失败,SQLCODE=[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "createCustMsg", "插入用户主档信息历史表[dCustMsgHis]表失败,", " SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);

		if (retMsg != NULL)
		{
			strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
			retMsg[strlen(vReturn_Msg)] = '\0';
		}
		return 1;
	}


	/*插入用户主档信息表dCustMsg*/

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
	/*ng解耦 by wxcrm at 20090812 begin
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
	    sprintf(vReturn_Msg,"插入用户主档信息表[dCustMsg]表失败,SQLCODE=[%d]",SQLCODE);
	    pubLog_Debug(_FFL_, "createCustMsg", "插入用户主档信息表[dCustMsg]表失败,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
	    if (retMsg != NULL)
	    {
	        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
	        retMsg[strlen(vReturn_Msg)] = '\0';
	    }
			return 2;
	}
	ng解耦 by wxcrm at 20090812 end*/
	
	
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
		sprintf(vReturn_Msg,"插入用户主档信息表[dCustMsg]表失败,SQLCODE=[%d]",SQLCODE);
	    pubLog_Debug(_FFL_, "createCustMsg", "插入用户主档信息表[dCustMsg]表失败,", "SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
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
* 创建客户档案信息
*
* @author        zhangzhe
*
* @version   %I%, %G%
*
* @since         1.00
*
* @inparam   客户档案结构体   tCustDoc *doc
*
* @inparam   客户id               doc->vCust_Id               long;
* @inparam   客户地区代码         doc->vRegion_Code           char(2);
* @inparam   客户市县代码         doc->vDistrict_Code         char(2);
* @inparam   客户网点代码         doc->vTown_Code             char(3;
* @inparam   机构编码             doc->vOrg_Code              char(9);
* @inparam   归属节点             doc->vGroup_Id              char(10);
* @inparam   工号识别             doc->vOrg_Id                char(10);
* @inparam   客户名称             doc->vCust_Name             char(60);
* @inparam   客户密码             doc->vCust_Passwd           char(8);
* @inparam   客户状态             doc->vCust_Status           char(2);
* @inparam   客户状态时间         doc->vRun_Time              char(17) YYYYMMDD HH24:MI:SS;
* @inparam   档案建立时间         doc->vCreate_Time           char(17) YYYYMMDD HH24:MI:SS;
* @inparam   档案注销时间         doc->vClose_Time            char(17) YYYYMMDD HH24:MI:SS;
* @inparam   客户等级代码         doc->vOwner_Grade           char(2);
* @inparam   客户类型             doc->vOwner_Type            char(2);
* @inparam   客户住址             doc->vCust_Addr             char(128);
* @inparam   证件类型             doc->vId_Type               char(1);
* @inparam   证件号码             doc->vId_Iccid              char(20);
* @inparam   证件地址             doc->vId_Addr               char(60);
* @inparam   证件有效期           doc->vId_ValidDate          char(17) YYYYMMDD;
* @inparam   联系人               doc->vContact_Person        char(20);
* @inparam   联系电话             doc->vContact_Phone         char(20);
* @inparam   联系地址             doc->vContact_Addr          char(128);
* @inparam   联系人邮政编码       doc->vContact_Post          char(6);
* @inparam   联系人通信地址       doc->vContact_MailAddr      char(60);
* @inparam   联系人传真           doc->vContact_Fax           char(30);
* @inparam   联系人E-mail         doc->vContact_Email         char(30);
* @inparam   创建备注             doc->vCreate_Note           char(60);
* @inparam   操作流水             doc->vLogin_Accept          long;
* @inparam   操作时间             doc->vOp_Time               char(17) YYYYMMDD HH24:MI:SS;
* @inparam   帐务日期             doc->vTotal_Date            int;
* @inparam   操作工号             doc->vLogin_No              char(6);
* @inparam   操作代码             doc->vOp_Code               char(4);
*
* @outparam  返回错误信息   retMsg   char(60)
*
* @return        0:  成功
* @return        1:  error of insert dCustDocHis;
* @return        2:  error of insert dCustDoc;
*/
int createCustDoc_ng(char *send_type,char *send_id,tCustDoc *doc,char *retMsg)
{
        char vSQL_Text[5096+1];                 /* SQL语句           */
        char vTmp_String[5096+1];               /* 临时变量          */
        char vClose_Time[8+1];                  /* 档案失效时间      */
        char vOp_Type[1+1];                     /* 操作类型          */
        long vUnit_Id;                          /* UNIT id           */
        char vStatus[1+1];						/* 客户状态			 */
        char regionGrpId[10+1];
        char districtGrpId[10+1];
        char vReturn_Msg[255+1];                /* 返回信息          */
		char vVpmn_flag[1+1];					/* ivpn集团标志		 */
		
		/*ng解耦*/
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
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户id                =[%ld]", doc->vCust_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户地区代码  = [%s]", doc->vRegion_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户市县代码  = [%s]", doc->vDistrict_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户网点代码  = [%s]", doc->vTown_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:机构编码          = [%s]", doc->vOrg_Code);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:归属节点          = [%s]", doc->vGroup_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:工号识别          = [%s]", doc->vOrg_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户名称          = [%s]", doc->vCust_Name);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户密码          = [%s]", doc->vCust_Passwd);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户状态          = [%s]", doc->vCust_Status);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户状态时间      = [%s]", doc->vRun_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:档案建立时间      = [%s]", doc->vCreate_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:档案注销时间      = [%s]", doc->vClose_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户等级代码      = [%s]", doc->vOwner_Grade);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户类型          = [%s]", doc->vOwner_Type);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:客户住址          = [%s]", doc->vCust_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:证件类型          = [%s]", doc->vId_Type);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:证件号码          = [%s]", doc->vId_Iccid);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:证件地址          = [%s]", doc->vId_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:证件有效期        = [%s]", doc->vId_ValidDate);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系人            = [%s]", doc->vContact_Person);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系电话          = [%s]", doc->vContact_Phone);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系地址          = [%s]", doc->vContact_Addr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系人邮政编码    = [%s]", doc->vContact_Post);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系人通信地址    = [%s]", doc->vContact_MailAddr);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系人传真        = [%s]", doc->vContact_Fax);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:联系人E-mail      = [%s]", doc->vContact_Email);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:上级客户ID        = [%s]", doc->vParent_Id);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:创建备注          = [%s]", doc->vCreate_Note);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:操作流水          = [%ld]", doc->vLogin_Accept);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:操作时间          = [%s]", doc->vOp_Time);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:帐务日期          = [%d]", doc->vTotal_Date);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:操作工号          = [%s]", doc->vLogin_No);
		pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:真实标识          = [%s]", doc->vTrue_Flag);
        pubLog_Debug(_FFL_, "createCustDoc", "", "debug msg:操作代码          = [%s]", doc->vOp_Code);

#endif

        /*
        * 插入 dCustDocHis 表
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
        * 插入客户档案信息历史表dCustDocHis
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
                sprintf(vReturn_Msg,"插入客户档案历史资料表[dCustDocHis]表失败,SQLCODE=[%d]",SQLCODE);
                pubLog_Debug(_FFL_, "createCustDoc", "插入客户档案历史历史资料表[dCustDocHis]表失败,SQLCODE=", "[%d][%s]",SQLCODE,SQLERRMSG);

                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
                        retMsg[strlen(vReturn_Msg)] = '\0';
                }
                return 1;
        }

        /*
        * 插入客户档案信息表dCustDoc
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
		/*ng解耦 by wxcrm at 20090812 begin
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
                sprintf(vReturn_Msg,"插入客户档案表[dCustDoc]表失败,SQLCODE=[%d]",SQLCODE);
                pubLog_Debug(_FFL_, "createCustDoc", "插入客户档案资料表[dCustDoc]表失败", ",SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
                if (retMsg != NULL)
                {
                        strncpy(retMsg,  vReturn_Msg, strlen(vReturn_Msg));
                        retMsg[strlen(vReturn_Msg)] = '\0';
                }
                return 2;
        }
		ng解耦 by wxcrm at 20090812 end*/
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
			sprintf(vReturn_Msg,"插入客户档案表[dCustDoc]表失败,SQLCODE=[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "createCustDoc", "插入客户档案资料表[dCustDoc]表失败", ",SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
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
函数名称:fpubAddGrpMember_ng
编码时间:2004/11/21
编码人员:yangzh
功能描述:集团用户增加成员公用函数
输入参数:集团用户id
         成员手机号码
         操作员代码
         操作代码
         帐务日期
         操作流水号
         操作备注
         错误返回信息(作为返回参数使用)
输出参数:错误代码
返 回 值:0代表正确返回,其他错误
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
         char vRun_Time[17+1];        /*add by miaoyl at 20090629 开户时间*/
         char vOrg_Id[10+1];          /*add by miaoyl at 20090708  */
         
         /*NG解藕*/
         int	v_ret=0;
         char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
         TdGrpUserMebMsg	tdGrpUserMebMsg;
         char	BeginTime1[20+1];
         char	IdNo2[22+1];
         char	sTempSqlStr[1024+1];
         int    icount=0;
    EXEC SQL END DECLARE SECTION;

    /*变量初始化*/
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
    init(vRun_Time        );    /*add by miaoyl at 20090629 开户时间*/ 
    init(vOrg_Id          );    /*add by miaoyl at 20090708  */
          

    /*变量赋值*/
    lLoginAccept = lSysAccept;
    strcpy(vGrpId_No,   inGrpId_No);
    strcpy(vPhone_No,   phone_no  );
    strcpy(vLogin_No,   login_no  );
    strcpy(vOp_Code,    op_code   );
    strcpy(vTotal_Date, total_date);
    strcpy(vOp_Note,    op_note   );

    /*--- 大客户表结构调整 edit by miaoyl at 20090708---begin*/
    /*取工号ORG_ID*/
    ret = 0;
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
	    sprintf(return_message,"查询org_id失败![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190100;
	}
	Trim(vOrg_Id);
   /*--- 大客户表结构调整 edit by miaoyl at 20090708---end*/
       
    /*查询集团用户资料*/
    EXEC SQL SELECT Cust_Id, User_No, Product_Type, Product_Code, To_Char(Bill_Date, 'YYYYMMDD')
      INTO :vGrpCust_Id, :vGrp_Userno, :vProduct_Type, :vProduct_Code, :vEnd_Date
      FROM dGrpUserMsg
      WHERE ID_NO = To_Number(:vGrpId_No) AND Bill_Date > Last_Day(sysdate) + 1;
    if (SQLCODE !=0) {
        sprintf(return_message,"查询dGrpUserMsg出错[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190103;
    }
    Trim(vGrpCust_Id);
    Trim(vGrp_Userno);
    Trim(vProduct_Type);
    Trim(vProduct_Code);

    /*查询手机客户资料*/
    Trim(vPhone_No);
    EXEC SQL SELECT id_no, cust_id, sm_code, to_char(bill_type)
       INTO :vId_No, :vCust_Id, :vSm_Code, :vBill_Code
       FROM dCustMsg WHERE phone_no = :vPhone_No;
    if (SQLCODE != 0) {
        sprintf(return_message,"查询dCustMsg表发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190101;
    }
    Trim(vBill_Code);

    /* 限制成员多次加入同一集团 */
    mCount = 0;
    EXEC SQL SELECT COUNT(*) INTO :mCount
      FROM dGrpUserMebMsg
      WHERE id_no = to_number(:vGrpId_No) and member_id = to_number(:vId_No) 
      	and end_time > last_day(sysdate) + 1;
    if (mCount > 0) {
        sprintf(return_message, "手机号码[%s]已经加入集团[%s]!", vPhone_No, vGrpId_No);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190102;
    }

    EXEC SQL select limit_flag into :vLimitFlag 
    	from sProductMebLimit where product_code = :vProduct_Code;
   	if (SQLCODE != 0 && SQLCODE != 1403) {
   		sprintf(return_message, "查询sProductMebLimit表错误[%d]", SQLCODE);
   		return 190131;
   	}
    
    if (SQLCODE == 1403) {
    	/* yangzh add 2005/04/14 固定号码不能加入同一品牌的集团成员 begin */    	
      EXEC SQL SELECT COUNT(*) INTO :mCount
        FROM dGrpUserMebMsg a, dGrpUserMsg b
        WHERE a.id_no = b.id_no and a.member_id = to_number(:vId_No)
          and b.product_type = :vProduct_Type and b.bill_date > last_day(sysdate) + 1
          and a.end_time > last_day(sysdate) + 1;
      if (SQLCODE != 0) {
          sprintf(return_message,"判断固定号码是否已经申请同一品牌产品出错[%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190131;
      }
      if (mCount > 0) {
          sprintf(return_message, "已申请同类产品");
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190132;
      }    	
    	/* yangzh add 2005/04/14 固定号码不能加入同一品牌的集团用户 end */
    } else {
    	ret = fCheckProdMebLimit_ng(vGrpId_No, vId_No, vProduct_Code, vLimitFlag, return_message);
    	if (ret != 0) {
    		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
    		return 190131;
    	}
    }
    /*查询集团客户资料*/
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
        sprintf(return_message,"查询dCustDocOrgAdd出错[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190104;
    }
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/

    /*判断是否VPMN集团客户*/
    if (0 == strcmp(vProduct_Type, VPMN_SM_CODE)) {
      strcpy(vGroup_Type_Code, "1");
    } else {
      strcpy(vGroup_Type_Code, "2");
    }

    /*查询dGrpMemberMsg表当前成员是否已经存在*/
    EXEC SQL SELECT COUNT(*) INTO :mCount
               FROM dGrpMemberMsg
              WHERE Cust_Id = To_Number(:vGrpCust_Id)
                AND Id_No = To_Number(:vId_No);
    if (SQLCODE !=0){
        sprintf(return_message,"查询dGrpMemberMsg表[%s]成员数量出错[%d]", vId_No, SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190105;
    }
    if (mCount == 0) { /*集团客户成员还不存在*/
        /*将用户记录保存到dGrpMemberMsgHis表
          big_flag --大客户标识，先更新成不是('0'非，'1'是)
unit_id,                                           vrecord.unit_id,                                   
unit_code,                                         TO_CHAR(vrecord.unit_id),                          
id_no,                                             v_id_no,                                           
big_flag,                                          '0',--大客户标识，先更新成不是('0'非，'1'是)       
phone_no,                                          TRIM(vrecord.real_no),                             
NAME,                                              NVL(SUBSTRB(TRIM(vrecord.username),1,20),'未知'),  
user_name,                                         NVL(SUBSTRB(TRIM(vrecord.username),1,20),'未知'),  
male_flag,                                         '2',                                               
belong_code,                                       '*',                                               
service_no,                                        'work99',                                          
card_code,                                         '99',                                              
member_code,                                       'C1',                                              
id_iccid,                                          SUBSTRB(vrecord.iccid,1,20),                       
dept,                                              SUBSTRB(vrecord.department,1,20),                  
ingrp_date,                                        TO_CHAR(vrecord.op_time,'yyyy-mm-dd'),--入网时间   
service_access,                                    '*',                                               
sm_code,                                           '*',                                               
pay_code,                                          '99',--付费方式，'99'未知                          
bill_code,                                         '*',                                               
pay_fee_flag,                                      '*',                                               
payed_fee,                                         vrecord.lmtfee,                                    
short_phoneno,                                     TO_CHAR(vrecord.short_no),                         
note,                                              TRIM(vrecord.note),                                
cust_id,                                           TO_NUMBER(vrecord.real_no),--客户编号，先写入手机号
load_date,                                         TO_CHAR(SYSDATE,'yyyy-mm-dd'),--入库日期           
group_type_code,                                   '1',                                               
gmember_status_code,                               '1',--所属集团类型为vpmn,成员状态为有效            
pay_fee_flag, payed_fee,
dept, substr(department,1,20),
        */
    /* LINA VIP2.0 2006_11_05 begin here*/
    /*取vip组织树dgrpgroups的org_code*/ 
    
    /*--- 大客户表结构调整 edit by miaoyl at 20090708---begin*/     
    /*EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    	char vOrgId[10+1];
    EXEC SQL END DECLARE SECTION;
    init(vOrgId);*/
    /*--- 大客户表结构调整 edit by miaoyl at 20090708---end*/
    
    /*查询集团客户信息*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "查询集团客户信息");
    #endif
    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vGrpCust_Id)
                AND Unit_Id = To_Number(:vUnit_Id);
        if (SQLCODE != 0){
            sprintf(return_message,"查Org_Code失败",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190106;
        }
    */
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/   
      
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "LINA");
 
    	/*组织机构改造插入表字段group_id和org_id  edit by zhengxg at 07/05/2009*/
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
    
        /*取开户时间*/
        Trim(vId_No);
        EXEC SQL select to_char(open_time,'yyyymmdd hh24:mi:ss') INTO :vRun_Time 
                   from dCustMsg
                  where id_no =To_Number(:vId_No);
       if (SQLCODE != 0)
        {
            sprintf(return_message,"查询dCustMsg表开户时间open_time发生异常[%d]",SQLCODE);
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
                  :vService_No, '未知', :vOp_Note,
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
                  :vService_No, '未知', :vOp_Note,
                  :vLogin_No, :lLoginAccept, :vTotal_Date,
                  sysdate, :vOp_Code, 'A', GROUP_ID,
                  '','','','',
                  '','','','',    
                  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',    
                  '',:vOrg_Id            
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);

        if (SQLCODE != 0){
            sprintf(return_message,"查询dCustDoc表,生成dGrpMemberMsgHis成员资料发生异常[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190106;
        }
        /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
        
    	/*组织机构改造插入表字段group_id和org_id  edit by zhengxg at 07/05/2009*/
    	
        /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
                  :vService_No, '未知', :vOp_Note, :sIn_Grp_OrgCode, GROUP_ID
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
                  :vService_No, '未知', :vOp_Note, GROUP_ID,
                  '','','','',
                  '','','','',    
                  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',    
                  '',:vOrg_Id           
             FROM dCustDoc
            WHERE Cust_Id = To_Number(:vCust_Id);

        if (SQLCODE != 0){
            sprintf(return_message,"查询dCustDoc表,生成dGrpMemberMsg成员资料发生异常[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190107;
        }
        /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
        
    /* LINA VIP2.0 end here*/
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "判断是否VPMN集团成员，更新集团客户成员表VPMN特有信息");
    #endif
    
    EXEC SQL SELECT Count(*) INTO :vCnt
               FROM dVPMNUsrMsg
              WHERE Real_No like :vPhone_No||'%';
    if (SQLCODE != 0){
        sprintf(return_message,"查询dVPMNUsrMsg表发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190108;
    }
    if (1 == vCnt) { /*这样集团成员就是VPMN集团成员,可以得到VPMN集团信息*/

        EXEC SQL SELECT substrb(nvl(department,'未知'), 1, 20), To_Char(lmtfee), short_no
                   INTO :vDepartment, :vLmtFee, :vShort_PhoneNo
                   FROM dVPMNUsrMsg
                  WHERE Real_No like :vPhone_No||'%';
        if (SQLCODE != 0){
            sprintf(return_message,"查询dVPMNUsrMsg表用户信息发生异常[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190109;
        }
        Trim(vDepartment);
        Trim(vLmtFee);
        Trim(vShort_PhoneNo);

        /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
        /*
        EXEC SQL UPDATE dGrpMemberMsg
                    SET dept = substrb(nvl(:vDepartment, '未知' ), 1, 20),
                        payed_fee = To_Number(:vLmtFee),
                        short_phoneno = :vShort_PhoneNo
                  WHERE Cust_Id = :vGrpCust_Id
                    AND Phone_No = :vPhone_No;
        */
      EXEC SQL UPDATE dGrpMemberMsg
        SET dept = substrb(nvl(:vDepartment, '未知' ), 1, 20)
      WHERE Cust_Id = :vGrpCust_Id
        AND Phone_No = :vPhone_No;             
        /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/   
                     
        if (SQLCODE != 0){
            sprintf(return_message,"更新dGrpMemberMsg表用户信息发生异常[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 190109;
        }
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "插入集团用户成员表");
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
        sprintf(return_message,"插入dGrpUserMebMsghis表发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190108;
    }
    /*NG解藕
    EXEC SQL INSERT INTO dGrpUserMebMsg
         ( ID_NO,        MEMBER_ID,   MEMBER_NO,   BAK_FIELD,
           BEGIN_TIME,   END_TIME )
        VALUES
         ( :vGrpId_No,    :vId_No,     :vPhone_No, :vPhone_No,
           sysdate, To_Date(:vEnd_Date, 'YYYYMMDD') );
    if (SQLCODE != 0){
        sprintf(return_message,"插入dGrpUserMebMsg表发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190109;
    }
	NG解藕*/
    /*ng解耦  by magang at 20090812 begin*/
    pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMebMsg!\n");  
			init(BeginTime1);
    		  EXEC SQL select 	to_char(sysdate,'YYYYMMDD HH24:MI:SS') 
    		  			into :BeginTime1
    		  		    from  dual;
    		  	if (SQLCODE != 0)
    		  	{
    		  		sprintf(return_message,"获取时间失败[%d]",SQLCODE);
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
				sprintf(return_message,"插入dGrpUserMebMsg表发生异常[%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        		return 190109;
			}

    /*ng解耦  by magang at 20090812 end*/
    /*yangzh add 2005-01-24 个人用户加入集团时，置attr_code为集团客户标志*/
    /*NG解藕
    EXEC SQL UPDATE dcustmsg
       SET attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)
     WHERE phone_no = :vPhone_No;
    if (SQLCODE != 0){
        sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        return 190109;
    }
	NG解藕*/
    /*ng解耦  by magang at 20090812 begin*/
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
        		pubLog_DBErr(_FFL_,"","","获取id_no失败 [%s]--\n",vPhone_No);
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
				sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
        		EXEC SQL CLOSE ngcursor3;
        		return 190109;
			}
			icount++;
		}
		EXEC SQL CLOSE ngcursor3;
		if(icount==0)
		{
			sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
        	pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
            return 290109;
		}	
 	/*ng解耦  by magang at 20090804 END*/
/*根据产品查询服务信息，一个产品可以对应多个服务 chenxuan boss3
    EXEC SQL DECLARE prod_cur CURSOR for
             select a.product_level, b.service_code
               from sProductCode a, sProductSrv b
              where a.product_code = b.product_code
                and a.product_code = :vProduct_Code
                and a.product_status = 'Y'
                and a.begin_time < sysdate
                and a.end_time > sysdate;
    if(SQLCODE!=OK){
          sprintf(return_message, "查询产品与服务对应关系错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190021;
    }

    EXEC SQL OPEN prod_cur;
    if(SQLCODE!=OK){
          EXEC SQL CLOSE prod_cur;
          sprintf(return_message, "打开产品与服务对应关系游标错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
          return 190022;
    }

    EXEC SQL FETCH prod_cur INTO :vProduct_Level, :vService_Code;
    while(SQLCODE == 0)
    {
        插入集团用户成员产品定购表,GRP侧表
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
            sprintf(return_message,"插入dGrpUserMebProdMsgHis表发生异常[%d]",SQLCODE);
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
            sprintf(return_message,"插入dGrpUserMebProdMsg表发生异常[%d]",SQLCODE);
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
函数名称:IPfpubCreateGrpUser_ng
编码时间:2004/11/21
编码人员:yangzh
功能描述:集团用户开户公用函数(8100专线使用)
输入参数:集团用户信息
         成员手机号码
         操作员代码
         操作代码
         帐务日期
         操作流水号
         操作备注
输出参数:错误代码
返 回 值:0代表正确返回,其他错误
**************************************************************************/
int IPfpubCreateGrpUser_ng(char *send_type,char *send_id,tGrpUserMsg *grpUserMsg, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note)
{   
	
	   int ret =0;
     tAccDoc  accDoc ;
    EXEC SQL BEGIN DECLARE SECTION;
         char    vCust_Id[14+1];        /* 集团客户ID       */
         char    vGrp_Id[14+1];         /* 集团用户ID       */
         char    vGrp_No[15+1];         /* 用户编号         */
         char    vIds[4+1];             /* 帐户数           */
         char    vGrp_Name[60+1];       /* 用户名称         */
         char    vProduct_Type[8+1];    /* 产品类型         */
         char    vProduct_Code[8+1];    /* 产品代码         */
         char    vOperation_Code[2+1];  /* GRP部门业务类型  */
         char    vGrpProduct_Code[2+1]; /* GRP部门的产品代码*/
         char    vUser_Passwd[8+1];     /* 业务受理密码     */
         char    vLogin_Name[20+1];     /* 自服务用户名     */
         char    vLogin_Passwd[20+1];   /* 自服务密码       */
         char    vProvince[2+1];        /* 归属省           */
         char    vRegion_Code[2+1];     /* 归属地区         */
         char    vDistrict_Code[2+1];   /* 归属县           */
         char    vTown_Code[3+1];       /* 归属营业厅       */
         char    vTerritory_Code[20+1]; /* 属地代码         */
         char    vLimit_Owe[18+1];      /* 透支额度         */
         char    vCredit_Code[2+1];     /* 信用等级         */
         char    vCredit_Value[18+1];   /* 信用度           */
         char    vRun_Code[1+1];        /* 用户状态         */
         char    vOld_Run[1+1];         /* 前一状态         */
         char    vRun_Time[20+1];       /* 状态变化时间     */
         char    vBill_Date[20+1];      /* 出帐时间         */
         char    vBill_Type[4+1];       /* 出帐周期代码     */
         char    vLast_Bill_Type[1+1];  /* 上次出帐类型     */
         char    vOp_Time[20+1];        /* 开户时间         */
         char    vSrv_Start[10+1];      /* 业务起始日期     */
         char    vSrv_Stop[10+1];       /* 业务结束日期     */
         char    vBak_Field[16+1];      /* 预留字段         */
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
         char  vTotal_Date[8+1];       /*帐务日期     */
         char  vDateChar[6+1];         /*帐务年月     */
         char  vUnit_Id[22+1];         /*集团ID       */
         char  vUnit_Code[22+1];       /*集团代码     */
         char  vAccount_Id[14+1];      /*帐户ID       */
         char  vAccount_Passwd[8+1];   /*帐户密码     */
         char  vContract_Id[22+1];     /*集团合同ID   */
         char  vContract_No[22+1];     /*集团合同代码 */
         char  vProduct_Id[22+1];      /*产品ID       */
         char  vOrder_GoodsId[22+1];   /*订单ID       */
         char  vProduct_List[1536];    /*产品代码列表 */
         char  vProduct_All[1536];     /*附加产品列表 */
         char  sqlStr[3072];
         char  vService_Type[1+1];
         char  vService_Code[4+1];
         char  vMain_Flag[1+1];
         char  vPrice_Code[4+1];
         char  vVPMN_Flag[1+1];
         char  vDisCode1[7+1];
         char  vBelong_Code[7+1];
         char  vDisCode2[7+1];
         char    vPay_Code[1+1];          /*集团帐户付费方式 */
         
          /*NG解藕*/
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
    /*在sSmCode表中,GRP部门的产品代码配置为了产品品牌*/
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
        sprintf(grpUserMsg->return_message, "帐户付款方式不能为空!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
        return 190001;
    }

    /*读取操作员工号权限*/
    EXEC SQL SELECT power_right INTO :vPower_right FROM dLoginMsg
              WHERE Login_No = :vLogin_No;
    if (SQLCODE != OK) {
        sprintf(grpUserMsg->return_message, "读取操作员权限出错![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190001;
    }
	
    /*根据操作员权限,决定地区通配字符串*/
    /*操作员权限：0、1：省工号权限;2、3：地区权限;4、5、6：市县权限;7：区域权限;
                  8：营业班长权限; 9：营业员权限 */
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
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询产品是否已经发布");
    #endif
    /*查询产品是否已经发布*/
    EXEC SQL SELECT Count(*) INTO :vCount FROM sProductRelease
              WHERE (GROUP_ID like :vDisCode1||'%' or GROUP_ID like :vDisCode2||'%' )
                AND RELEASE_STATUS = 'Y'
                AND Product_Code = :vProduct_Code;
    if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "查询sProductRelease表错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190001;
    }
    if (vCount <= 0) {
        sprintf(grpUserMsg->return_message, "产品[%s]不存在或尚未发布!",vProduct_Code);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190002;
    }

    /*查询集团档案信息*/
    EXEC SQL SELECT VPMN_FLAG INTO :vVPMN_Flag
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vCust_Id);
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "集团[%s]档案信息不存在!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190003;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "查询dCustDocOrgAdd表错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190004;
    }
    /*非VPMN集团客户，不能申请VPMN产品*/
    if (('0' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
    {
        sprintf(grpUserMsg->return_message, "非VPMN集团客户,不能申请VPMN产品");
        return 190005;
    }
    
    /*查询集团合同信息*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询集团合同信息");
    #endif
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
      
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/       
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "集团[%s]合同信息尚未录入!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190006;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "查询dGrpContract表错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190007;
    }

    /*VPMN用户开户时，返回集团客户系统的BOSS_VPMN_CODE字段*/
    Trim(vGrp_No);
    
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
   /*
    if (('1' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
    {
        EXEC SQL UPDATE dCustDocOrgAdd
                    SET BOSS_VPMN_CODE = :vGrp_No
                  WHERE Cust_Id = To_Number(:vCust_Id);
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "更新dCustDocOrgAdd表错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190007;
        }
    }
    */
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/

    Trim(vUnit_Id);
    Trim(vUnit_Code);
    Trim(vContract_Id);
    Trim(vContract_No);
    /*除专线业务外，对于同一个产品，不允许客户重复申请*/
    if ( (strcmp(vGrpProduct_Code, ZHUANX_IP_SM_CODE) != 0) &&
         (strcmp(vGrpProduct_Code, ZHUANX_DIANLU_SM_CODE) != 0) &&
         (strcmp(vGrpProduct_Code, ZHUANX_NET_SM_CODE) != 0) ) {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询dGrpUserMsg信息");
        #endif
        EXEC SQL SELECT Count(*) INTO :vCount
                   FROM dGrpUserMsg
                  WHERE Cust_Id = To_Number(:vCust_Id)
                    AND Product_Code = :vProduct_Code
                    AND Bill_Date > Last_Day(sysdate) + 1;
        if(SQLCODE!=OK){
            sprintf(grpUserMsg->return_message, "查询dGrpUserMsg表错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }
        if (vCount > 0) {
            sprintf(grpUserMsg->return_message, "集团用户已经申请[%s]产品!",vProduct_Code);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190011;
        }
    }

    /*查询集团帐户信息*/
    /*yangzh modified 2004/12/13
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询集团帐户信息");
    #endif
    EXEC SQL SELECT Trim(Contract_No)
               INTO :vAccount_Id
               FROM dConMsg
              WHERE CON_CUST_ID = To_Number(:vCust_Id);
    if(SQLCODE == 1403) {
        sprintf(grpUserMsg->return_message, "集团[%s]帐户信息不存在!", vCust_Id);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190008;
    }
    else if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "查询dConMsg表错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190009;
    }
    yangzh modified 2004/12/13*/

    /*查询dConMsg帐务表是否存在记录*/
    EXEC SQL SELECT COUNT(*) INTO :vCount
               FROM dConMsg
              WHERE CONTRACT_NO = To_Number(:vAccount_Id);
    if(SQLCODE!=OK){
        sprintf(grpUserMsg->return_message, "查询集团用户dConMsg表错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190009;
    }

    /*记录dConMsg帐务表记录*/
    if (vCount == 0) 
    { /*帐户如果不存在，就进行帐户开户处理,集团用户开户时可以选择原集团帐户*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "生成集团用户统一付费帐户");
        #endif
        
        

       Trim(vOrgId);

        
      /*组织机构改造开户函数整合  edit by liuweib at 23/02/2009
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
            sprintf(grpUserMsg->return_message, "插入集团用户dConMsgHis表错误![%d]",SQLCODE);
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
            sprintf(grpUserMsg->return_message, "插入集团用户dConMsg表错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190009;
        }
        *******组织机构改造开户函数整合  edit by liuweib at 23/02/2009*/
        
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
            sprintf(grpUserMsg->return_message, "插入集团用户dConMsgPre错误![%d]",SQLCODE);
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
            sprintf(grpUserMsg->return_message, "插入集团用户dCustConMsgHis错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dCustConMsg");
        #endif
        /*NG解藕
        EXEC SQL INSERT INTO dCustConMsg
                    (  Cust_Id,  Contract_No,  Begin_Time,    End_Time )
                    VALUES
                    (   To_Number(:vCust_Id), To_Number(:vAccount_Id),
                        sysdate, To_Date(:vSrv_Stop,'YYYYMMDD')
                    );
        if (SQLCODE != 0) {
            sprintf(grpUserMsg->return_message, "插入集团用户dCustConMsg错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;
        }
         NG解藕*/
        /*ng解耦  by magang at 20090812 begin*/
        init(BeginTime1);
        EXEC SQL select to_char(sysdate,'YYYYMMDD HH24:MI:SS') 
        		 into :BeginTime1
        		 from dual;
        if (SQLCODE != 0)
        {
        	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "获取时间失败");
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
			sprintf(grpUserMsg->return_message, "插入集团用户dCustConMsg错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
            return 190010;	
		}	     		 	
        /*ng解耦  by magang at 20090812 END*/
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
        sprintf(grpUserMsg->return_message, "插入集团用户dConUserMsgHis错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "insert into dConUserMsg");
    #endif
    /*NG解藕
    
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
        sprintf(grpUserMsg->return_message, "插入集团用户dConUserMsg错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }
	NG解藕*/
	/*ng解耦  by magang at 20090812 begin
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
				sprintf(grpUserMsg->return_message, "插入集团用户dConUserMsg错误![%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        		return 190010;
			}	
		ng解耦  by magang at 20090811 end*/

 /*yangzh disabled 20050124 不在dCustMsg里面的,就不插入wConUserChg
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
        sprintf(grpUserMsg->return_message, "插入集团用户wConUserChg错误![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190010;
    }
*/

    /*4.记录集团用户表dGrpUserMsg记录,BOSS侧帐务表*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpUserMsgHis表信息");
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
   
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
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
          sprintf(grpUserMsg->return_message, "插入dGrpUserMsgHis表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190012;
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpUserMsg表信息");
    #endif
    /*ng解耦 
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
          sprintf(grpUserMsg->return_message, "插入dGrpUserMsg表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190013;
    }
    ng解耦*/
	/*ng解耦  by magang at 20090812 begin*/
	    pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMsg!\n");
	    memset(&sTdGrpUserMsg,0,sizeof(TdGrpUserMsg));
	    init(TotalDate1);				
		EXEC SQL SELECT  to_char(sysdate,'YYYYMMDD HH24:MI:SS')
                 INTO :TotalDate1				
				 FROM	DUAL;
		if (SQLCODE !=0 )
 		{
 			sprintf(grpUserMsg->return_message, "获取时间错误![%d]",SQLCODE);
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
		  sprintf(grpUserMsg->return_message, "插入dGrpUserMsg表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190013;	
		}
		/*ng解耦  by magang at 20090812 end*/
 /*不再处理vip的表，chenxuan boss3*/ 
#if 0
    /*查询sGrpOperationProductCode得到GRP侧的业务类别*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询sGrpOperationProductCode表信息");
    #endif
    EXEC SQL SELECT OPERATION_CODE
               INTO :vOperation_Code
               FROM sGrpOperationProductCode
              WHERE Product_Code = :vGrpProduct_Code;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"查询sGrpOperationProductCode出错!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }

    /*获得产品ID*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "获得产品ID");
    #endif
    EXEC SQL SELECT To_Char(s_Product_Id.NextVal) INTO :vProduct_Id FROM DUAL;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"查询s_Product_Id出错!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }

    /*LINA VIP2.0 2006_11_05 begin here*/
    /*取vip组织树dgrpgroups的org_code*/    
    EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    EXEC SQL END DECLARE SECTION;
    /*查询集团客户信息*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询集团客户信息");
    #endif
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vCust_Id)
                AND Unit_Id = To_Number(:vUnit_Id);
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"查询Org_Code出错!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190014;
    }
    */
    strcpy(sIn_Grp_OrgCode,"");
   /* --- 大客户表结构调整 edit by miaoyl at 20090629---end*/
       
    /*插入集团用户扩展表, 集团侧表*/
    /*product_status取自dbgrpadm.sGrpProductStatusCode字典表*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpProductHis表信息");
    #endif
    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
    
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
            
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
            
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "插入dGrpProductHis表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190015;
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpProduct表信息");
    #endif
    
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
            
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
           
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "插入dGrpProduct表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190016;
    }
   
    /*获得订单ID*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "获得订单ID");
    #endif
    EXEC SQL SELECT To_Char(s_order_goods_id.NextVal) INTO :vOrder_GoodsId FROM DUAL;
    if(SQLCODE!=OK)
    {
        strcpy(grpUserMsg->return_message,"查询s_order_goods_id出错!");
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
        return 190017;
    }

    /*插入订单表记录,集团侧表*/
    /*订单有效状态CURR_STATUS见sGrpOrderGoodsStatusCode */
    /*订单施工状态URR_TACHE见sGrpOrderGoodsTacheCode*/
    Trim(vProduct_Id);
    Trim(vOrder_GoodsId);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpOrderGoodsHis表信息");
    #endif
    
   /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
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
          sprintf(grpUserMsg->return_message, "插入dGrpOrderGoodsHis表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190018;
    }

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpOrderGoods表信息");
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
          sprintf(grpUserMsg->return_message, "插入dGrpOrderGoods表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190019;
    }
     /*LINA VIP2.0 end here*/

    /*插入订单操作记录表*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录wGrpOrderOpr表信息");
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
          sprintf(grpUserMsg->return_message, "插入wGrpOrderOpr表错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190020;
    }
#endif

     /*查询产品归属关系 */
    vCount = 1;
    strToFilter(vProduct_Code, vProduct_List, strlen(vProduct_Code), sizeof(vProduct_List));
    strcpy(vProduct_All, vProduct_List);
    while (vCount > 0) {
        init(sqlStr);
        sprintf(sqlStr, "SELECT distinct a.product_code FROM sproductcode a, sproductrelease b WHERE a.product_code IN ( SELECT object_product FROM sproductcode a, sproductattach b WHERE a.product_code = b.product_code AND a.product_code IN (%s) MINUS SELECT object_product FROM sproductcode a, sProductExclude b WHERE a.product_code = b.product_code AND a.product_code IN (%s)) AND a.product_code = b.product_code AND b.release_status = 'Y' AND (b.GROUP_ID LIKE '%s%%' OR b.GROUP_ID LIKE '%s%%') AND SYSDATE BETWEEN b.begin_time AND b.end_time", vProduct_List, vProduct_List, vDisCode1, vDisCode2);
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询产品归属关系, vDisCode1[%s], vDisCode[%s], vProduct_List[%s], vProduct_All[%s]", vDisCode1, vDisCode2, vProduct_List, vProduct_All);
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
        #endif
 
        EXEC SQL PREPARE stmt001 FROM :sqlStr;
        if (SQLCODE != OK) {
             sprintf(grpUserMsg->return_message, "准备查询归属产品游标错误![%d]",SQLCODE);
             pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
             return 190020;
        }
        EXEC SQL DECLARE attachprod_cur0 CURSOR for stmt001;
        if (SQLCODE != OK) {
             sprintf(grpUserMsg->return_message, "定义归属产品游标错误![%d]",SQLCODE);
             pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
             return 190020;
        }

        vCount = 0;
        init(vProduct_List);
        EXEC SQL OPEN attachprod_cur0;
        if(SQLCODE!=OK){
             
             sprintf(grpUserMsg->return_message, "打开归属产品游标错误![%d]",SQLCODE);
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

     /*查询产品服务关系表 */
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询产品服务关系表");
    #endif
    init(sqlStr);
    sprintf(sqlStr, "select b.product_code, a.service_type, b.service_code, b.main_flag, b.fav_order, c.price_code from sSrvCode a, sProductSrv b, sSrvPrice c where a.service_code = b.service_code and a.service_code = c.service_code and b.product_code in (%s)", vProduct_All);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "查询产品服务关系表, vProduct_All[%s]", vProduct_All);
        pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
    #endif

    EXEC SQL PREPARE stmt002 FROM :sqlStr;
    if (SQLCODE != OK) {
         sprintf(grpUserMsg->return_message, "解析产品服务关系游标错误![%d]",SQLCODE);
         pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
         return 190020;
    }

    EXEC SQL DECLARE srv_cur0 CURSOR for stmt002;
    if(SQLCODE!=OK){
          sprintf(grpUserMsg->return_message, "查询产品与服务对应关系错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190021;
    }

    EXEC SQL OPEN srv_cur0;
    if(SQLCODE!=OK){
          EXEC SQL CLOSE srv_cur0;
          sprintf(grpUserMsg->return_message, "打开产品与服务对应关系游标错误![%d]",SQLCODE);
          pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
          return 190022;
    }

    init(vProduct_Code);
    EXEC SQL FETCH srv_cur0 INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
    while(SQLCODE == 0)
    {

        /*记录集团用户定购产品表,BOSS侧帐务表*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpSrvMsgHis表信息");
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
              sprintf(grpUserMsg->return_message, "记录dGrpSrvMsgHis表错误![%d]",SQLCODE);
              pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
              return 190023;
        }

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "记录dGrpSrvMsg表信息");
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
              sprintf(grpUserMsg->return_message, "记录dGrpSrvMsg表错误![%d]",SQLCODE);
              pubLog_Debug(_FFL_, "IPfpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
              return 190024;
        }
        EXEC SQL FETCH srv_cur0 INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
    }

    EXEC SQL CLOSE srv_cur0;

    return 0;
}

/************************************************************************
函数名称:fpubDelGrpMember_ng
编码时间:2004/11/21
编码人员:yangzh
功能描述:集团用户删除成员公用函数
输入参数:集团用户id
         成员手机号码
         操作员代码
         操作代码
         帐务日期
         操作流水号
         操作备注 
         错误信息
输出参数:错误代码
返 回 值:0代表正确返回,其他错误
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
         
          /*NG解藕*/
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

    /*变量初始化*/
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
    /*变量赋值*/
    lLoginAccept = lSysAccept;
    strcpy(vGrpId_No,   inGrpId_No);
    strcpy(vPhone_No,   phone_no  );
    strcpy(vLogin_No,   login_no  );
    strcpy(vOp_Code,    op_code   );
    strcpy(vTotal_Date, total_date);
    strcpy(vOp_Note,    op_note   );
    Trim(vPhone_No);
	
    /*--- 大客户表结构调整 edit by miaoyl at 20090708---begin*/
    /*取工号ORG_ID*/
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
		sprintf(return_message,"查询org_id失败![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190200;
	}
	Trim(vOrg_Id);
   /*--- 大客户表结构调整 edit by miaoyl at 20090708---end*/
       
    /*查询集团用户资料*/
    EXEC SQL SELECT Cust_Id, User_No, Account_Id, Product_Type, Product_Code
               INTO :vGrpCust_Id, :vGrp_Userno, :vAccount_Id, :vProduct_Type, :vProduct_Code
               FROM dGrpUserMsg
              WHERE ID_NO = To_Number(:vGrpId_No)
                AND Bill_Date > Last_Day(sysdate) + 1;
    if (SQLCODE !=0){
        sprintf(return_message,"查询dGrpUserMsg出错[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190201;
    }

    /*查询手机客户资料*/
    EXEC SQL SELECT id_no, CUST_ID, sm_code
               INTO :vId_No, :vCust_Id, :vSm_Code
               FROM dCustMsg
              WHERE phone_no = :vPhone_No ;
    if (SQLCODE != 0){
        sprintf(return_message,"查询dCustMsg表发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190202;
    }
		
		/* 验证是否已加入 */
    EXEC SQL select count(*) into :vCnt from dGrpUserMebMsg 
    	where id_no = :vGrpId_No and member_id = :vId_No and end_time > last_day(sysdate)+1;
    if (vCnt == 0) {
      sprintf(return_message, "手机号码[%s]未加入集团[%s]!", vPhone_No, vGrpId_No);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190203;;
    }

    /*处理dGrpMemberMsg*/
    /*#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "查询dGrpMemberMsg表");
    #endif
    EXEC SQL SELECT To_Char(UNIT_ID) INTO :vUnit_Id
               FROM dGrpMemberMsg
        WHERE Cust_Id = To_Number(:vGrpCust_Id)
          AND ID_NO = To_Number(:vId_No);
    if (SQLCODE != 0 && ){
        sprintf(return_message,"查询dGrpMemberMsg表资料发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190203;
    }*/

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "查询dGrpUserMebMsg表");
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
        sprintf(return_message,"查询dGrpUserMebMsg中[%s]的数量[%d]", vId_No, SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
          return 190204;
      }

   /* LINA VIP2.0 2006_11_05 begin here*/
    /*取vip组织树dgrpgroups的org_code*/   
    
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
    /*EXEC SQL BEGIN DECLARE SECTION;
    	char sIn_Grp_OrgCode[10+1];
    	char vOrg_id[10+1];
    EXEC SQL END DECLARE SECTION;
    init(vOrg_id);*/
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
    
    /*查询集团客户信息*/
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "查询集团客户信息");
    #endif
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
    /*
    EXEC SQL SELECT Org_Code
               INTO :sIn_Grp_OrgCode
               FROM dCustDocOrgAdd
              WHERE Cust_Id = To_Number(:vGrpCust_Id);
        if (SQLCODE != 0){
            sprintf(return_message,"查Org_Code失败",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190105;
        }
    */   
    /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/
    
    if (vCnt < 2) { /*用户只是一个集团用户的成员时，删除*/
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "删除dGrpMemberMsg表[%s]资料", vId_No);
        #endif
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "LINA++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        /*-------组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009-------*/
       
        /*--- 大客户表结构调整 edit by miaoyl at 20090629---begin*/
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
                     
       /*--- 大客户表结构调整 edit by miaoyl at 20090629---end*/    

        if (SQLCODE != 0 && SQLCODE != 1403){
            sprintf(return_message,"插入dGrpMemberMsgHis表资料发生异常[%d]",SQLCODE);
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
            sprintf(return_message,"删除dGrpMemberMsg表资料发生异常[%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190207;
        }
        /* LINA VIP2.0 end here*/
        /*yangzh add 2005-01-24 个人用户离开集团时，置attr_code为个人客户标志*/
        /*NG解藕
        EXEC SQL UPDATE dcustmsg
           SET attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)
         WHERE phone_no = :vPhone_No;
        if (SQLCODE != 0){
            sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190208;
        }
        NG解藕*/
         /*ng解耦  by magang at 20090812 begin*/
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
        		pubLog_DBErr(_FFL_,"","","获取id_no失败 [%s]--\n",vPhone_No);
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
				sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
           	 	pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
           	 	EXEC SQL CLOSE ngcursor4;
            	return 190208;
			}
			ncount++;
		}
		EXEC SQL CLOSE ngcursor4;
		if(ncount==0)
		{
			sprintf(return_message,"更新dCustMsg表attr_code错误![%d]",SQLCODE);
            pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
            return 190208;
		}	
 	/*ng解耦  by magang at 20090804 END*/
    }

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "调用函数，对集团用户成员的付费计划进行变更");
#endif

    /* 成员删除前判断有付费计划，如有不允许删除，必须先做付费计划变更 
    EXEC SQL select count(*) into :vCnt from dConUserMsg
      where id_no = :vId_No and contract_no = :vAccount_Id and serial_no = 0;
    if (SQLCODE != 0) {
      sprintf(return_message, "查询成员付费计划错误[%d]", SQLCODE);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190109;
    }

    if (vCnt > 0) {
      sprintf(return_message,"成员[%s]有付费计划，不能删除，请先执行付费计划变更！", vPhone_No);
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
      return 190110;
    }
    boss3.0 */

    /* 由于有了上面的限制，不会执行下面代码，但继续保留，以备以后修改方便 
    retCode = fpubDisConUserMsg(vId_No, vAccount_Id, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
    if (retCode != 0) {
      pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "调用fpubDisConUserMsg发生错误,Id_No[%s],Account_Id[%s],retCode[%d]", vId_No, vAccount_Id, retCode);
      return 190111;
    }
    boss3.0 */
    
    /*处理dGrpUserMebMsg*/
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
        sprintf(return_message,"插入dGrpUserMebMsgHis表资料发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190212;
    }
    /*NG解藕
    EXEC SQL UPDATE dGrpUserMebMsg
                SET End_Time = Last_Day(To_Date(:vTotal_Date,'YYYYMMDD')) + 1
               WHERE Id_No = To_Number(:vGrpId_No)
                 AND Member_no = :vPhone_No
                 AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0 || SQLROWS != 1){
        sprintf(return_message,"更新dGrpUserMebMsg表资料发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190213;
    }
	NG解藕*/
	/*ng解耦  by magang at 20090812 begin*/
    		pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateGrpUserMebMsg!\n");
    		
   EXEC SQL select to_char(member_id),to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time ,'yyyymmdd hh24:mi:ss'),to_char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD'))+1,'yyyymmdd hh24:mi:ss') 
			into :MemberId1,:BeginTime1,:EndTime1,:vTotal_Date1
			from dGrpUserMebMsg 
			where Id_No = To_Number(:vGrpId_No)
                 AND Member_no = :vPhone_No
                 AND End_Time > Last_Day(sysdate) + 1;

     			if (SQLCODE != 0 || SQLROWS != 1)
        		{
        			sprintf(return_message,"更新dGrpUserMebMsg表资料发生异常[%d]",SQLCODE);
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
				sprintf(return_message,"更新dGrpUserMebMsg表资料发生异常[%d]",SQLCODE);
        		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        		return 190213;
			}

    /*ng解耦  by magang at 20090811 end*/
	
	
/* 不再处理已废除表dGrpUserMebProdMsg chenxuan boss3
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
        sprintf(return_message,"插入dGrpUserMebProdMsgHis表资料发生异常[%d]",SQLCODE);
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
        return 190209;
    }
    EXEC SQL UPDATE dGrpUserMebProdMsg
                SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
              WHERE ID_No = To_Number(:vGrpId_No)
                AND MEMBER_ID = To_Number(:vId_No)
                AND End_Time > Last_Day(sysdate) + 1;
    if (SQLCODE != 0){
        sprintf(return_message,"删除dGrpUserMebProdMsg表资料发生异常[%d]",SQLCODE);
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
    	sprintf(retMsg, "查询集团产品表错误[%d]", SQLCODE);
    	return -1;
    }    
    if (vCount > 0) {
    	sprintf(retMsg, "成员[%s]已加入同类集团", iMebId);
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
/*ng解耦添加参数dx 2009年9月30日9:54:17*/
		char sIdNo[14+1];
		char sLoginAccept[14+1];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char vCallServiceName[30+1];
		int vSendParaNum=0;
		int vRecpParaNum=0;
		char vParamList[DLMAXITEMS][2][DLINTERFACEDATA];
		int iOrderRight=100;
/*ng解耦添加参数dx 2009年9月30日9:54:17*/		
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
		/*组织机构改造插入表字段group_id和org_id  edit by zhangxha at 02/03/2009*/
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
               printf("插入wUserMsgChg Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
               return  -1;
       }
/*ng解耦 dx 2009年9月30日9:51:04
       EXEC SQL UPDATE dCustMsg
                SET    run_code=:run_code,
                       run_time=to_date(:op_time,'YYYYMMDD HH24:MI:SS')
                WHERE  id_no=:id_no;
       if(SQLCODE!=0)
       {
               printf("更新用户资料SQLCODE[%d]SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
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
    	printf("更新用户资料ret[%d]\n",ret);
        printf("run_code[%s]op_time[%s]\n",run_code,op_time);
        return -1;
    }
    printf("-----[%s]\n",old_time);

	EXEC SQL select sOffOn.nextval into :offon_accept from dual;
	if (SQLCODE != SQL_OK) 
	{
		printf("取流水出错。[%d]\n",SQLCODE);
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
		printf("调用函数recordChgList出错[%s]。",return_message); 
		return -1;
	}	  
 	/*modify --end*/
	if(run_code[0]!='K')
	{
		printf("\ntime .....[%s][%s][%d]",op_time,old_time,month_flag);
/*ng解耦dx 2009年9月30日10:00:54 begin
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
		vSendParaNum=3; /* 业务工单服务输入参数个数 */
		vRecpParaNum=2;/* 业务工单服务输出参数个数 */
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
/*ng解耦dx2009年9月30日10:26:32 end*/	
	return 0;
}

/*******************************************************
功能:
	按手机号码交费开关机
输入参数：
	InContractNo       long    默认帐户ID
	InIdNo             long    用户ID
	*InSmCode          char    品牌代码
	*InRegionCode      char    地区代码
	*InOpCode          char    操作代码
	InPayedPrepay      double  预存划拨
	InTotalDate        int     帐务日期
	InPayAccept        long     交费流水
	*InPhoneNo         char    手机号码
	*InOrgCode         char    机构代码
	*InWorkNo          char    操作工号
	InPayMoney         double  本次交款
	InStillOwe         double  欠费金额
	InStillRemonth     double  补收月租
	InLimitOwe         double  开机限额
	InNoAccountFee     double  未出帐话费
输出参数：
返回码：
	130010 查询用户信息表出错
	130011 查询交费开机表出错
	132002 插入开关机表出错
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
	
	/***取用户的状态及开关机优先级****/
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
		printf("<PublicSendOffon>取用户状态错误[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
		return  130010;
	}
	
	/***默认帐户的预存款及付款方式******/
	/*出账不停营业改造  edit by houxue  20090326*/
	/*RetCode = GetAccountMsgOwe(InContractNo,&PrepayFee,PayCode);
	if (RetCode != SQL_OK) 
	{
		return RetCode;
	}
	
	RetCode=PublicBillFlag(vRunFlag,vBillFlag);
	if(RetCode!=0)
	{
		printf("\n检查出帐标志 err");
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
	/*出账不停营业改造  end by houxue  20090326*/      
	init(NewRun);
	init(AllFeeFlag);

	/*-- '0' 交清欠费 --*/
	/*-- '1' 交清欠费+未出帐话费 --*/
	/*-- '2' 交清欠费+未出帐话费+补收月租费 --*/
	/***LimitOwe尚未确定*****/
	/*PrepayFee = round_F(PrepayFee, 2);*/
	printf("\nCurRun[%s]\n",CurRun);
	printf("\nRunCode[%s]\n",RunCode);
	printf("\nOldCode[%s]\n",OldRun);
	/*printf("\n[%12.2f][%12.2f][%12.2f][%12.2f]",PrepayFee,LimitOwe,StillOwe,NoAccountFee); */
	/*if (round_F(PrepayFee+LimitOwe,2)>=round_F(StillOwe+NoAccountFee,2))*/
	/*余额+缴费+优惠+信誉额>0*/
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
	
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret =0;
	ret = sGetUserGroupid(PhoneNo,GroupId);
	if(ret <0)
	{
		printf("<PublicSendOffon>获取用户group_id失败![%s]\n",PhoneNo);
		return  130021;
	}
	Trim(GroupId);
	ret =0;
	ret = sGetLoginOrgid(WorkNo,OrgId);
	if(ret <0)
	{
		printf("<PublicSendOffon>获取工号org_id失败![%s]\n",WorkNo);
		return  130022;
	}
	Trim(OrgId);
	/****************************
	记录wusermsgchg
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
	                    '交费'||to_char(:v14)||'元[余存'||to_char(:v15)||'仍欠'||to_char(:v16)||'未出帐'||to_char(:v17)||']')");
                   
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
		printf("<PublicSendOffon>:取流水出错.\n");
		printf("\n[%d],[%s]",SQLCODE,SQLERRMSG);
		return 132005;
	}
	Trim(vPayMoney);
	Trim(vPrepayFee);
	Trim(vStillOwe);
	Trim(vNoAccountFee);
	
  	sprintf(ChgReson, "交费%s元[余存%s仍欠%s未出帐%s]", vPayMoney,vPrepayFee,vStillOwe,vNoAccountFee);
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
		printf("<PublicSendOffon>:调用函数recordChgList出错[%s].\n",return_message);       
		return 132006;
	}
	*/	  
 	/*---组织机构改造插入表字段edit by zhangxha at 02/03/2009---end*/

	/***更新dCustStop****/
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
 	/*输入输出参数个数*/
	sendNum = 1;
	recvNum = 3;
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng,初始化输入缓冲区失败!");
		return 100110;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"6",(FLDLEN32)0);	
						
	sprintf(vContractNo,"%ld",IContractNo);		
	Trim(vContractNo);
	Fchg32(sendbuf,GPARM32_0,0,vContractNo,(FLDLEN32)0);
	/*初始化输出缓冲区*/
	rcvlen = (FLDLEN32)((recvNum)*120);			
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		pubLog_Debug(_FFL_, "Query_Con_Owe_ng", "", "Query_Con_Owe_ng,初始化输出缓冲区失败!");
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
 	/*输入参数个数*/
	sendNum = 2;
	/*输出参数个数*/
	recvNum = 3;
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng,初始化输入缓冲区失败!");
		return 100120;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"2",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);
	
	Fchg32(sendbuf,GPARM32_0,0,vContractNo,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_1,0,vPayType,(FLDLEN32)0);
	
	/*初始化输出缓冲区*/
	rcvlen = (FLDLEN32)((recvNum)*120);
	
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "Query_Con_Pre_ng,初始化输出缓冲区失败!");
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

	/*释放分配的内存*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	*OConPreFee = vPrepayFee;
	pubLog_Debug(_FFL_, "Query_Con_Pre_ng", "", "-------------- Query_Con_Pre_ng end --------------");
 	return 0;
}
/************************************************************************
函数名称:adcAddPlusMode
编码时间:2007/03/20
编码人员:chenxuan
功能描述:ADC成员加入申请附加资费函数
输入参数:
输出参数:错误代码
返 回 值:0代表正确返回,其他错误
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
  /* 查询手机号码的基本信息 */
  EXEC SQL select to_char(id_no), belong_code, substr(belong_code,1,2), substr(belong_code,3,2), sm_code
        into :vIdNo, :vBelongCode, :vRegionCode, :vDistrictCode, :vSmCode
    from dCustMsg where phone_no = :iPhoneNo and substr(run_code,2,1) < 'a';
	if (SQLCODE != 0) {
		sprintf(oRetMsg, "当前用户[%s]的不允许申请附加套餐!", iPhoneNo);
		return 194020;
	}

  /* 查询新资费是否存在 */
  EXEC SQL select sm_code INTO :vNewSmCode from sBillModeCode
  	where region_code = :iRegionCode and mode_code = :iNewMode;
  if (SQLCODE != 0) {
  	sprintf(oRetMsg, "附加套餐在BOSS系统中未定义[%s]", iNewMode);
  	return 194021;
  }

  EXEC SQL SELECT count(1) INTO :icount FROM dbillcustdetail WHERE id_no=to_number(:vIdNo)
  				AND	mode_code=:iNewMode and mode_time='Y' and mode_status='Y'
  				AND	end_time > sysdate;
  if (SQLCODE != 0)
  {
  	sprintf(oRetMsg, "查询用户套餐 ERROR[%s]", iNewMode);
  	return 194091;
  }

  if(icount > 0)
  {
  	sprintf(oRetMsg, "用户[%s]已开通此套餐",iPhoneNo);
  	return 194099;
  }

  /* 申请附加资费  */
	sprintf(retCode, "%06d", pubBillBeginMode(iPhoneNo, iNewMode, iOpTime, iOpCode, iLoginNo,
		iLoginAccept, iOpTime, "0", "", "", temp_buf));
	printf("retCode is %s\n,temp_buf is %s\n",retCode,temp_buf)	;
  if (strcmp(retCode, "000000") != 0) {
		sprintf(oRetMsg,"申请附加套餐[%s]%s", iNewMode, temp_buf);
    return 194022;

  }

	return 0;
}

/************************************************************************
函数名称:adcDelPlusMode
编码时间:2007/03/20
编码人员:chenxuan
功能描述:ADC成员取消申请附加资费函数
输入参数:
输出参数:错误代码
返 回 值:0代表正确返回,其他错误
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
	printf("!!!取消ADC附加套餐:iOldMode is [%s]\n",iOldMode);
	/* 取原附加套餐的流水 */
	/*chendx 20121218 update 解决了包年转包月套餐无法获取流水问题*/
	sprintf(dynStmt, "select distinct login_accept from dBillCustDetail%c where "
		" mode_code = :v1 and id_no = :v2 and end_time > last_day(sysdate) + 1", iIdNo[strlen(iIdNo)-1]);
	EXEC SQL prepare pre from :dynStmt;
	EXEC SQL declare cur cursor for pre;
	EXEC SQL open cur using :iOldMode, :iIdNo;
	EXEC SQL fetch cur into :vOldAccept;
	EXEC SQL close cur;
	if (SQLCODE != 0) {
		sprintf(oRetMsg, "查询ADC附加套餐失败!");
		return 194030;
	}

  /* 取消ADC附加套餐  */
  printf("取消ADC附加套餐:iOldMode is [%s]vOldAccept[%s]\n",iOldMode,vOldAccept);
	sprintf(retCode, "%06d", pubBillEndMode(iPhoneNo, iOldMode, vOldAccept, iOpTime,
		iOpCode, iLoginNo, iLoginAccept, iOpTime, temp_buf));
	if (strcmp(retCode, "000000") != 0) {
		sprintf(oRetMsg, "取消当前附加套餐[%s]时%s", iOldMode, temp_buf);
		return 194031;
	}

	return 0;
}
