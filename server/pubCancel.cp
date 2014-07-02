#include "pubLog.h"
#include "boss_srv.h"
#include "publicsrv.h"
#include "pubOrder.h"

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL BEGIN DECLARE SECTION;
    char input_parms[MAX_PARMS_NUM][1000];
    char output_parms[MAX_PARMS_NUM][1000];
EXEC SQL END DECLARE SECTION;


 /**
  ** Auth: YuanBY
  ** Name: pubCancelAcctInfo
  ** Func: 消户操作冲正帐户信息 
  ** 输入参数  
  **     操作工号 
  **     客户ID   
  **     用户ID   
  **     帐户ID   
  **     操作流水 
  **     帐务日期 
  **     系统日期 
  **     操作代码  
  **       
  ** Date: 2003/10/14
  ** Stat: Test Pass
  **/
  
int pubCancelAcctInfo(char sIn_Phone_NoA[16] , char sIn_Login_noA[7] , char sIn_Cust_idA[23] , 
     char sIn_User_idA[23] , char sIn_Acct_idA[23] ,
     char sIn_Login_acceptA[39] , char sIn_Total_dateA[21] , 
     char sIn_Sys_dateA[21] , char sIn_Op_codeA[5] , 
     char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_Phone_No[15+1];           /* 手机号码      */
        char    sIn_Login_no[6+1];            /* 操作工号      */
        char    sIn_Cust_id[22+1];            /* 客户ID        */
        char    sIn_User_id[22+1];            /* 用户ID        */
        char    sIn_Acct_id[22+1];            /* 帐户ID        */
        char    sIn_Login_accept[38+1];       /* 操作流水      */
        char    sIn_Total_date[20+1];         /* 帐务日期      */
        char    sIn_Sys_date[20+1];           /* 系统日期      */
        char    sIn_Op_code[4+1];             /* 操作代码      */
        
        char    sV_Assure_no[22+1];           /* 担保号码      */
        int     sV_Rent_flag=0;               /* 租机          */
        long     vOp_No=0; /*   帐务序列  */ 
        char    vSm_Code[2+1];
        char    vAttr_Code[8+1];
        char    vBelong_Code[7+1];
        
        char 	vGroupId[10+1];
        int		Vret=0;
        int		iAssureNum = 0;
	
		char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];				/*ng解耦 by mengfy at 20090810 */		
		int		v_ret = 0;						
		char	dynStmt[2024];
		char	sSerail_No_o[10+1];
		char	sMain_Code_o[2+1];
		int		vCount = 0;
 		
 		TdConUserMsgIndex	tConUserMsgoldIndex;			
 		TdCustTypeIndex 	tCustTypeoldIndex;
 		TdCustConMsgIndex	tdCustConMsgIndex;
    EXEC SQL END DECLARE SECTION;
    
    pubLog_Debug(_FFL_, "pubCancelAcctInfo", "", "begin function ");
    
    /* 初试化参数 */
    init(sIn_Phone_No);
    init(sIn_Login_no);
    init(sIn_Cust_id);
    init(sIn_User_id);
    init(sIn_Acct_id);
    init(sIn_Login_accept);
    init(sIn_Total_date);
    init(sIn_Sys_date);
    init(sIn_Op_code);
    
    init(sV_Assure_no);
    init(vSm_Code);
    init(vAttr_Code);
    init(vBelong_Code);
    
    init(vGroupId);
    
    init(v_parameter_array);		/*ng解耦 by mengfy at 20090810 */		
    init(sSerail_No_o);
    init(sMain_Code_o);
    memset(&tConUserMsgoldIndex, 0, sizeof(tConUserMsgoldIndex));
    memset(&tCustTypeoldIndex, 0, sizeof(tCustTypeoldIndex));
    memset(&tdCustConMsgIndex, 0, sizeof(tdCustConMsgIndex));
     
    
    /* 取得输入参数 */
    strcpy(sIn_Phone_No , sIn_Phone_NoA );
    strcpy(sIn_Login_no , sIn_Login_noA );
    strcpy(sIn_Cust_id , sIn_Cust_idA );
    strcpy(sIn_User_id , sIn_User_idA );
    strcpy(sIn_Acct_id , sIn_Acct_idA );
    strcpy(sIn_Login_accept , sIn_Login_acceptA );
    strcpy(sIn_Total_date , sIn_Total_dateA );
    strcpy(sIn_Sys_date , sIn_Sys_dateA );
    strcpy(sIn_Op_code , sIn_Op_codeA );
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Phone_No", "[%s]",		sIn_Phone_No);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Login_no", "[%s]",		sIn_Login_no);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Cust_id", "[%s]",			sIn_Cust_id);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_User_id", "[%s]",			sIn_User_id);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Acct_id", "[%s]",			sIn_Acct_id);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Login_accept", "[%s]",	sIn_Login_accept);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Total_date", "[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Sys_date", "[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sIn_Op_code", "[%s]",      sIn_Op_code);
    #endif
    
    if(strcmp(sIn_Op_code,"1122")==0)
    {
        /* 插入 dCustDocHis 表 */
        /*---组织机构改造插入字段 edit by zhaoqm at 2009-04-07---*/
        EXEC SQL insert into dCustDocHis(CUST_ID , REGION_CODE , DISTRICT_CODE ,
                      TOWN_CODE , CUST_NAME , CUST_PASSWD , CUST_STATUS ,
                      RUN_TIME , OWNER_GRADE , OWNER_TYPE , CUST_ADDRESS ,
                      ID_TYPE , ID_ICCID , ID_ADDRESS , ID_VALIDDATE ,
                      CONTACT_PERSON , CONTACT_PHONE , CONTACT_ADDRESS ,
                      CONTACT_POST , CONTACT_MAILADDRESS , CONTACT_FAX ,
                      CONTACT_EMAILL , ORG_CODE , CREATE_TIME ,
                      CLOSE_TIME , PARENT_ID ,
                      UPDATE_ACCEPT , UPDATE_TIME , UPDATE_LOGIN ,
                      UPDATE_TYPE , UPDATE_CODE,UPDATE_DATE,
                      GROUP_ID,ORG_ID)
                 select CUST_ID , REGION_CODE , DISTRICT_CODE ,
                      TOWN_CODE , CUST_NAME , CUST_PASSWD , CUST_STATUS ,
                      RUN_TIME , OWNER_GRADE , OWNER_TYPE , CUST_ADDRESS ,
                      ID_TYPE , ID_ICCID , ID_ADDRESS , ID_VALIDDATE ,
                      CONTACT_PERSON , CONTACT_PHONE , CONTACT_ADDRESS ,
                      CONTACT_POST , CONTACT_MAILADDRESS , CONTACT_FAX ,
                      CONTACT_EMAILL , ORG_CODE , CREATE_TIME ,
                      CLOSE_TIME , PARENT_ID ,
                      to_number(:sIn_Login_accept),
                      TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                      :sIn_Login_no , 'd' , :sIn_Op_code,to_number(:sIn_Total_date),
                      GROUP_ID,ORG_ID
                 from dCustDoc
                 where CUST_ID = to_number(:sIn_Cust_id) ;
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入dCustDocHis表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -1;
        }
        
        /* 删除 dCustDoc 表 */ /****20070528 modify****/
        /*EXEC SQL delete from dCustDoc 
                 where CUST_ID = to_number(:sIn_Cust_id) ;
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","删除dCustDoc表失败!");
            pubLog_Debug(_FFL_, "pubCancelAcctInfo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -2;
        }*/
    
    }
    /*--组织机构改造获取group_id edit by zhaoqm at 08/04/2009--begin*/
	Vret = sGetUserGroupid(sIn_Phone_No,vGroupId);
	if(Vret <0)
	{
		strcpy(sErrMessA,"查询用户group_id失败!");
		pubLog_Debug(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
		return -10;
	}
	Trim(vGroupId);

	pubLog_Debug(_FFL_, "", "", "sIn_User_id=[%s]", sIn_User_id);
	/*---组织机构改造获取group_id edit by zhaoqm at 08/04/2009---end*/
    EXEC SQL select sm_code,attr_code,belong_code into :vSm_Code,:vAttr_Code,:vBelong_Code
             from dCustMsg where id_no=to_number(:sIn_User_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","查询dCustMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -3;
    }
    /****20070528 add begine****/
    if(strcmp(sIn_Op_code,"1122")==0 || strcmp(sIn_Op_code,"1125")==0)
    {
    	if(strcmp(vSm_Code,"cb")!=0)
    	{
    		sprintf(sErrMessA,"%s","非长白行开户不能在此进行冲正");
			pubLog_Debug(_FFL_,"pubCancelAcctInfo","sErrMessA,sIn_Op_code,vSm_Code","[%s][%s][%s]",sErrMessA,sIn_Op_code,vSm_Code);
        	return -3;
    	}
    }
    if(strcmp(sIn_Op_code,"1123")==0 || strcmp(sIn_Op_code,"1126")==0)
    {
    	if(strcmp(vSm_Code,"z0")!=0)
    	{
    		sprintf(sErrMessA,"%s","非神州行开户不能在此进行冲正");
        	pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sErrMessA", "[%s]sIn_Op_code = [%s],vSm_Code = [%s]",sErrMessA,sIn_Op_code,vSm_Code);
        	return -3;
    	}
    }
    if(strcmp(sIn_Op_code,"1121")==0 || strcmp(sIn_Op_code,"1124")==0)
    {
    	if(strcmp(vSm_Code,"cb")==0)
    	{
    		sprintf(sErrMessA,"%s","非全球通开户不能在此进行冲正");
        	pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sErrMessA", "[%s]sIn_Op_code = [%s],vSm_Code = [%s]",sErrMessA,sIn_Op_code,vSm_Code);
        	return -3;
    	}
    }
    /****20070528 add end****/
    
    /* 插入 dCustMsgHis 表 */
    /*---组织机构改造插入字段 edit by zhaoqm at 2009-04-07---*/
    EXEC SQL insert into dCustMsgHis(ID_NO , CUST_ID , CONTRACT_NO , IDS ,
                  PHONE_NO , SM_CODE , SERVICE_TYPE , ATTR_CODE , 
                  USER_PASSWD , BELONG_CODE , LIMIT_OWE , CREDIT_CODE ,
                  CREDIT_VALUE , RUN_CODE , RUN_TIME , BILL_DATE ,
                  BILL_TYPE , ENCRYPT_PREPAY , CMD_RIGHT , 
                  LAST_BILL_TYPE , BAK_FIELD ,
                  UPDATE_ACCEPT , UPDATE_TIME , UPDATE_DATE , UPDATE_LOGIN ,
                  UPDATE_TYPE , UPDATE_CODE,OPEN_TIME,
                  GROUP_NO )
             select ID_NO , CUST_ID , CONTRACT_NO , IDS ,
                  PHONE_NO , SM_CODE , SERVICE_TYPE , ATTR_CODE , 
                  USER_PASSWD , BELONG_CODE , LIMIT_OWE , CREDIT_CODE ,
                  CREDIT_VALUE , RUN_CODE , RUN_TIME , BILL_DATE ,
                  BILL_TYPE , ENCRYPT_PREPAY , CMD_RIGHT , 
                  LAST_BILL_TYPE , BAK_FIELD ,
                  to_number(:sIn_Login_accept),
                  TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS') , to_number(substr(:sIn_Sys_date,1,8)) ,
                  :sIn_Login_no , 'd' , :sIn_Op_code,OPEN_TIME,
                  GROUP_NO
             from dCustMsg
             where ID_NO = to_number(:sIn_User_id)
                 and CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","插入dCustMsgHis表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -3;
    }
    /* 删除 dCustMsg 表 */
	/*ng解耦 by mengfy at 20090810 */		
/*    EXEC SQL delete from dCustMsg 
             where ID_NO = to_number(:sIn_User_id)
                 and CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","删除dCustMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -4;
    }
*/
    init(v_parameter_array);		

	strcpy(v_parameter_array[0], sIn_User_id);
	strcpy(v_parameter_array[1], sIn_Cust_id);
	
	v_ret = 0;
	v_ret = OrderDeleteCustMsg(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept), sIn_Login_no,
			"pubCancelAcctInfo", sIn_User_id, "and CUST_ID = to_number(:v1)", v_parameter_array);	
    if(0 != v_ret)
    {
        sprintf(sErrMessA,"%s","删除dCustMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -4;
    }
	/*ng解耦 by mengfy at 20090810 */		
    /* 插入 dConMsgHis 表 */
    /*---组织机构改造插入字段 edit by zhaoqm at 2009-04-07---*/
    EXEC SQL insert into dConMsgHis(CONTRACT_NO , CON_CUST_ID , BANK_CODE ,
                  POST_BANK_CODE , ACCOUNT_NO , BANK_CUST , ODDMENT ,
                  BELONG_CODE , PREPAY_FEE , PREPAY_TIME , ACCOUNT_TYPE ,
                  STATUS , STATUS_TIME , POST_FLAG , DEPOSIT ,
                  MIN_YM , OWE_FEE , ACCOUNT_MARK , ACCOUNT_LIMIT ,
                  PAY_CODE , CONTRACT_PASSWD ,
                  UPDATE_CODE , UPDATE_DATE , UPDATE_TIME ,
                  UPDATE_LOGIN , UPDATE_ACCEPT , UPDATE_TYPE,
                  GROUP_ID )
             select CONTRACT_NO , CON_CUST_ID , BANK_CODE ,
                  POST_BANK_CODE , ACCOUNT_NO , BANK_CUST , ODDMENT ,
                  BELONG_CODE , PREPAY_FEE , PREPAY_TIME , ACCOUNT_TYPE ,
                  STATUS , STATUS_TIME , POST_FLAG , DEPOSIT ,
                  MIN_YM , OWE_FEE , ACCOUNT_MARK , ACCOUNT_LIMIT ,
                  PAY_CODE , CONTRACT_PASSWD ,
                  :sIn_Op_code , to_number(:sIn_Total_date),
                  TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                  :sIn_Login_no , to_number(:sIn_Login_accept),'d',
                  GROUP_ID
             from dConMsg
             where CONTRACT_NO = to_number(:sIn_Acct_id)
                  and CON_CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","插入dConMsgHis表失败!");
       	PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -5;
    }
    
    /*NG 解偶  业务工单改造 Update by miaoyl 20091002 begin*/
    /* 删除 dConMsg 表 */
	/*ng解耦 by mengfy at 20090810 */		
    EXEC SQL delete from dConMsg
             where CONTRACT_NO = to_number(:sIn_Acct_id)
                  and CON_CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","删除dConMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -6;
    }

    /*
    init(v_parameter_array);		

    strcpy(v_parameter_array[0],sIn_Acct_id);
    strcpy(v_parameter_array[1],sIn_Cust_id);
	
	v_ret = 0;
	v_ret = OrderDeleteConMsg(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept), sIn_Login_no, "pubCancelAcctInfo",
			sIn_Acct_id, "and CON_CUST_ID = to_number(:v1)", v_parameter_array);
	if (v_ret != 0)
	{
        sprintf(sErrMessA,"%s","删除dConMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s],v_ret=[%d]",sErrMessA,v_ret);
        return -6;
	}
	ng解耦 by mengfy at 20090810 */		
    /* 删除 dConMsgPre 表 
    EXEC SQL delete from dConMsgPre
             where CONTRACT_NO = to_number(:sIn_Acct_id);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        sprintf(sErrMessA,"%s","删除dConMsgPre表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -6;
    }*/
    /*NG 解偶  业务工单改造 Update by miaoyl 20091002 end*/
    
    /* 查询担保号码 */
	pubLog_Debug(_FFL_, "", "", "sIn_User_id=[%s]", sIn_User_id);
	pubLog_Debug(_FFL_, "", "", "sIn_Cust_id=[%s]", sIn_Cust_id);
    
    EXEC SQL select ASSURE_NO into :sV_Assure_no 
             from  dCustInnet 
             where ID_NO = to_number(:sIn_User_id) 
                  and CUST_ID = to_number(:sIn_Cust_id); 
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","查询担保号码失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -20;
    }
    
    Trim(sV_Assure_no);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCancelAcctInfo", "sV_Assure_no", "[%s]",sV_Assure_no);
    #endif
    
    /* 修改担保数量 */
	/*ng解耦 by mengfy at 20090810 */		
/*    EXEC SQL update dAssuMsg set ASSURE_NUM = ASSURE_NUM -1
             where ASSURE_NO = to_number(:sV_Assure_no);
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
        sprintf(sErrMessA,"%s","修改担保数量失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -21;
    }
*/	
	/*ADD BY LIUWEIB 如果sV_Assure_no为则不再0执行减一操作*/
	if(strcmp(sV_Assure_no,"0")!= 0)
	{
		init(v_parameter_array);		
		strcpy(v_parameter_array[0],sV_Assure_no);
		v_ret =0;
		v_ret = OrderUpdateAssuMsg(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept), sIn_Login_no, 
			"pubCancelAcctInfo", sV_Assure_no, "ASSURE_NUM = ASSURE_NUM -1", "", v_parameter_array);
		if (v_ret < 0)
		{
		    sprintf(sErrMessA,"修改担保数量失败!");
		    PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","sErrMessA","[%s],v_ret=[%d]",sErrMessA,v_ret);
		    return -21;
		}     
	}
	/*ng解耦 by mengfy at 20090810 */		
    /* 插入 dCustInnetHis 表 */
    /*---组织机构改造插入字段 edit by zhaoqm at 2009-04-07---*/
    EXEC SQL insert into dCustInnetHis(ID_NO , CUST_ID , BUY_NAME , BUY_ID , BELONG_CODE ,
                  TOWN_CODE , INNET_TYPE , OPEN_TIME , LOGIN_ACCEPT , 
                  LOGIN_NO , MACHINE_CODE , CASH_PAY , CHECK_PAY ,
                  SIM_FEE , MACHINE_FEE , INNET_FEE , CHOICE_FEE , 
                  OTHER_FEE , HAND_FEE , DEPOSIT , BACK_FLAG , 
                  ENCRYPT_FEE , SYSTEM_NOTE , OP_NOTE , ASSURE_NO ,
                  UPDATE_LOGIN , UPDATE_ACCEPT , UPDATE_DATE ,
                  UPDATE_TIME , UPDATE_CODE , UPDATE_TYPE ,
                  GROUP_ID)
             select ID_NO , CUST_ID , ' ' , CUST_ID , BELONG_CODE ,
                  TOWN_CODE , INNET_TYPE , OPEN_TIME , LOGIN_ACCEPT , 
                  LOGIN_NO , MACHINE_CODE , CASH_PAY , CHECK_PAY ,
                  SIM_FEE , MACHINE_FEE , INNET_FEE , CHOICE_FEE , 
                  OTHER_FEE , HAND_FEE , DEPOSIT , BACK_FLAG , 
                  ENCRYPT_FEE , SYSTEM_NOTE , OP_NOTE , ASSURE_NO ,
                  :sIn_Login_no , to_number(:sIn_Login_accept), :sIn_Total_date,
                  TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                  :sIn_Op_code , 'd',
                  GROUP_ID
             from dCustInnet
             where ID_NO = to_number(:sIn_User_id) 
                  and CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0){
        sprintf(sErrMessA,"%s","插入dCustInnetHis表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -7;
    }

    /* 删除 dCustInnet 表 */
	/*ng解耦 by mengfy at 20090810 */		
/*    EXEC SQL delete from dCustInnet 
             where ID_NO = to_number(:sIn_User_id) 
                  and CUST_ID = to_number(:sIn_Cust_id);
    if(SQLCODE!=0){
        sprintf(sErrMessA,"%s","删除dCustInnet表失败!");
        return -8;
    }
*/
	init(v_parameter_array);
	
	strcpy(v_parameter_array[0], sIn_User_id);
	strcpy(v_parameter_array[1], sIn_Cust_id);

	v_ret = 0;
	v_ret = OrderDeleteCustInnet(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept), sIn_Login_no, 
			"pubCancelAcctInfo", sIn_User_id, "and CUST_ID = :v1", v_parameter_array);
	if (0 != v_ret)
	{
        sprintf(sErrMessA,"%s", "删除dCustInnet表失败!");
        return -8;
	}			
	/*ng解耦 by mengfy at 20090810 */		
    /* 插入 dCustUserHis 表 */
    EXEC SQL insert into dCustUserHis(CUST_ID , ID_NO , REP_FLAG , CUST_FLAG , UPDATE_TIME , 
                                      UPDATE_LOGIN , UPDATE_TYPE , UPDATE_ACCEPT , OP_CODE)
             select CUST_ID , ID_NO , REP_FLAG , CUST_FLAG , sysdate ,
                    :sIn_Login_no , 'D' , to_number(:sIn_Login_accept) , :sIn_Op_code
             from dCustUserMsg
             where CUST_ID = to_number(:sIn_Cust_id)
                and  ID_NO = to_number(:sIn_User_id);
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
        sprintf(sErrMessA,"%s","删除dCustUserMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -9;
    }

    /* 删除 dCustUserMsg 表 */
    EXEC SQL delete from dCustUserMsg 
             where CUST_ID = to_number(:sIn_Cust_id)
                and  ID_NO = to_number(:sIn_User_id);
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
        sprintf(sErrMessA,"%s","删除dCustUserMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -9;
    }

    /* 插入 dCustConMsgHis 表 */
    EXEC SQL insert into dCustConMsgHis( CUST_ID , CONTRACT_NO , 
                   BEGIN_TIME , END_TIME , 
                   UPDATE_ACCEPT , UPDATE_LOGIN , UPDATE_DATE ,
                   UPDATE_TIME , UPDATE_CODE , UPDATE_TYPE )
             select  CUST_ID , CONTRACT_NO , BEGIN_TIME , END_TIME , 
                   to_number(:sIn_Login_accept) , :sIn_Login_no , 
                   to_number(:sIn_Total_date) , 
                   TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                   :sIn_Op_code , 'd'
             from dCustConMsg 
             where CUST_ID = to_number(:sIn_Cust_id)
                  and CONTRACT_NO = to_number(:sIn_Acct_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","插入dCustConMsgHis表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -10;
    }
    
    /* 删除 dCustConMsg 表 */
    /*ng_解耦 edit by mengfy@09/18/2009
    EXEC SQL delete from dCustConMsg 
             where CUST_ID = to_number(:sIn_Cust_id)
                  and CONTRACT_NO = to_number(:sIn_Acct_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","删除dCustConMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -11;
    }
*/
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	
	strncpy(v_parameter_array[0], sIn_Cust_id, 10);				   
	strncpy(v_parameter_array[1], sIn_Acct_id, 14);
					   
	memset(&tdCustConMsgIndex, 0, sizeof(tdCustConMsgIndex));
					   
	strncpy(tdCustConMsgIndex.sCustId, sIn_Cust_id, 10);
	strncpy(tdCustConMsgIndex.sContractNo, sIn_Acct_id, 14);

	v_ret = 0;
    v_ret = OrderDeleteCustConMsg(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept),
    		sIn_Login_no, "pubCancelAcctInfo", tdCustConMsgIndex, "", v_parameter_array);
    if (v_ret != 0)
    {
        sprintf(sErrMessA,"%s","删除dCustConMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -11;
    }
    /*ng_解耦 edit by mengfy@09/18/2009*/
    /* 插入 dConUserMsgHis 表 */
    EXEC SQL insert into dConUserMsgHis( ID_NO , CONTRACT_NO , BILL_ORDER  , 
                  PAY_ORDER , BEGIN_YMD , BEGIN_TM , END_YMD , END_TM ,
                  LIMIT_PAY , RATE_FLAG , STOP_FLAG ,
                  UPDATE_ACCEPT , UPDATE_LOGIN , UPDATE_DATE ,
                  UPDATE_TIME , UPDATE_CODE , UPDATE_TYPE ,SERIAL_NO)
             select ID_NO , CONTRACT_NO , BILL_ORDER  ,
                  PAY_ORDER , BEGIN_YMD , BEGIN_TM , END_YMD , END_TM ,
                  LIMIT_PAY , RATE_FLAG , STOP_FLAG ,
                  to_number(:sIn_Login_accept) , :sIn_Login_no , to_number(:sIn_Total_date) , 
                  TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                  :sIn_Op_code , 'd' ,SERIAL_NO
             from dConUserMsg 
             where ID_NO = to_number(:sIn_User_id)
                  and CONTRACT_NO = to_number(:sIn_Acct_id);
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","插入dConUserMsgHis表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -12;
    }
    
    /* 删除 dConUserMsg 表 */
	/*ng解耦 by mengfy at 20090810 */		
/*    EXEC SQL delete from dConUserMsg 
             where ID_NO = to_number(:sIn_User_id)
                  and CONTRACT_NO = to_number(:sIn_Acct_id);
    if(SQLCODE!=0)
    {
    	pubLog_Debug(_FFL_, "", "", "sIn_User_id=[%s]", sIn_User_id);
    	pubLog_Debug(_FFL_, "", "", "sIn_Acct_id=[%s]", sIn_Acct_id);
        sprintf(sErrMessA,"%s","删除dConUserMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -13;
    }
*/
	init(dynStmt);
	sprintf(dynStmt,"select to_char(SERIAL_NO) from dConUserMsg "
             "where ID_NO = to_number(:v1)"
                  "and CONTRACT_NO = to_number(:v2)");
	EXEC SQL PREPARE sql_str FROM :dynStmt;
	EXEC SQL DECLARE ngcur01 cursor for sql_str;
	EXEC SQL OPEN ngcur01 using :sIn_User_id, :sIn_Acct_id;
	
	vCount = 0;
	for (;;)
	{	
		init(sSerail_No_o);

		EXEC SQL FETCH ngcur01 INTO :sSerail_No_o;
		if((0!=SQLCODE) && (1403!=SQLCODE))
		{
	        sprintf(sErrMessA,"删除dConUserMsg表失败!v_ret=[%d]", v_ret);
	        PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
			EXEC SQL CLOSE ngcur01;
			return -23;
		}
		if(1403==SQLCODE) break;
			
		Trim(sSerail_No_o);
		
		memset(&tConUserMsgoldIndex,0,sizeof(tConUserMsgoldIndex));
		
		strcpy(tConUserMsgoldIndex.sIdNo         , sIn_User_id);
		strcpy(tConUserMsgoldIndex.sContractNo   , sIn_Acct_id);
		strcpy(tConUserMsgoldIndex.sSerialNo     , sSerail_No_o);

		init(v_parameter_array);
		
	    strcpy(v_parameter_array[0],sIn_User_id);
	    strcpy(v_parameter_array[1],sIn_Acct_id);
	    strcpy(v_parameter_array[2],sSerail_No_o);
		
		v_ret = 0;
		v_ret=OrderDeleteConUserMsg(ORDERIDTYPE_USER, sIn_User_id,100,
				sIn_Op_code, atol(sIn_Login_accept), sIn_Login_no,"pubCancelAcctInfo",
				tConUserMsgoldIndex, "", v_parameter_array);
		if(0!=v_ret)
		{
	        sprintf(sErrMessA,"删除dConUserMsg表失败!v_ret=[%d]", v_ret);
	        PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
			EXEC SQL CLOSE ngcur01;
	        return -13;
		}
		vCount ++;			
	}		
	EXEC SQL CLOSE ngcur01;
	if (vCount == 0)
	{
    	pubLog_Debug(_FFL_, "", "", "sIn_User_id=[%s]", sIn_User_id);
    	pubLog_Debug(_FFL_, "", "", "sIn_Acct_id=[%s]", sIn_Acct_id);
        sprintf(sErrMessA,"%s","删除dConUserMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -13;
	}

	/*ng解耦 by mengfy at 20090810 */		
    
/*******
 帐务系统提供序列生成 op_no
 Op_Type: 1 Ins , 2 Upd , 3 Del
********/
    EXEC SQL SELECT sMaxBillChg.NEXTVAL INTO :vOp_No
             FROM DUAL;
        if(SQLCODE != 0){
	PUBLOG_DBDEBUG("pubCancelAcctInfo");
            pubLog_Debug(_FFL_, "pubCancelAcctInfo", "", "取帐务接口流水sMaxBillChg,错误 [%d]",SQLCODE);
            return -9;
        }
	/*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
    /*ng解耦 add by mengfy@08/21/2009---*/
#ifdef _CHGTABLE_        
    EXEC SQL INSERT INTO wUserMsgChg
                        (Op_No,        Op_Type,       Id_No,
                         Cust_Id,      Phone_No,      Sm_Code,
                         Attr_Code,    Belong_Code,   Run_Code,
                         Run_Time,     Op_Time,		  GROUP_ID)
                  VALUES(:vOp_No,     '3',          TO_NUMBER(:sIn_User_id),
                          TO_NUMBER(:sIn_Cust_id),:sIn_Phone_No,:vSm_Code, 
                         :vAttr_Code, :vBelong_Code, 'AA',
                          SUBSTR(:sIn_Sys_date,1,8)||SUBSTR(:sIn_Sys_date,10,2)
                       ||SUBSTR(:sIn_Sys_date,13,2)||SUBSTR(:sIn_Sys_date,16,2),
                          SUBSTR(:sIn_Sys_date,1,8)||SUBSTR(:sIn_Sys_date,10,2)
                       ||SUBSTR(:sIn_Sys_date,13,2)||SUBSTR(:sIn_Sys_date,16,2),
                       	:vGroupId);
        if (SQLCODE != 0) 
        {
        	sprintf(sErrMessA,"%s","插入wUserMsgChg 错误!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            EXEC SQL ROLLBACK;
            return -10;
        }
#endif	/*_CHGTABLE_*/
	/*ng解耦 add by mengfy@08/21/2009---*/
    EXEC SQL SELECT sMaxBillChg.NEXTVAL INTO :vOp_No
             FROM DUAL;
        if(SQLCODE!=0)
        {
        	sprintf(sErrMessA,"%s","取帐务接口流水sMaxBillChg错误!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -18;
        }
	/*ng解耦 by mengfy at 20090821*/
#ifdef _CHGTABLE_	
    EXEC SQL INSERT INTO wConUserChg
                        (Op_No,       Op_Type,    Id_No,
                         Contract_No, Bill_Order, Conuser_Update_Time,
                         Limit_Pay,   Rate_Flag,  Fee_Code,    
                         Fee_Rate,   Op_Time,DETAIL_CODE)
                 VALUES(:vOp_No,     '30',         TO_NUMBER(:sIn_User_id),
                         TO_NUMBER(:sIn_Acct_id),      99999999,      
                         to_char(sysdate,'YYYYMMDDHH24MISS'),
                         0,          'N',      ' ',         
                         0,         
                         to_char(sysdate,'YYYYMMDDHH24MISS'),'*');
        if (SQLCODE != 0) 
        {
            sprintf(sErrMessA,"%s","插入wConUserChg 错误!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            EXEC SQL ROLLBACK;
            return -19;
        }
#endif	/*_CHGTABLE_*/
    /*ng解耦 by mengfy at 20090821 */
    
    sV_Rent_flag=0;
    EXEC SQL select count(*) into :sV_Rent_flag from dCustRMsg where id_no = to_number(:sIn_User_id);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        sprintf(sErrMessA,"%s","查询dCustRMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -14;
    }
    if(sV_Rent_flag>0) /* 是租机用户 */
    {
    	EXEC SQL insert into dCustRMsgHis(ID_NO , RENT_MONTH , RENT_CODE , MACHINE_CODE , MACHINE_FEE , SIM_FEE , 
    	                                  UPDATE_ACCEPT , UPDATE_TIME , UPDATE_DATE , 
    	                                  UPDATE_LOGIN , UPDATE_TYPE , UPDATE_CODE)
    	         select ID_NO , RENT_MONTH , RENT_CODE , MACHINE_CODE , MACHINE_FEE , SIM_FEE , 
    	                :sIn_Login_accept , TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS') , to_number(:sIn_Total_date) , 
    	                :sIn_Login_no , 'd' , :sIn_Op_code
    	         from dCustRMsg where id_no = to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入dCustRMsgHis表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -15;
        }
        
        EXEC SQL delete dCustRMsg where id_no = to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","删除dCustRMsg表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -16;
        }
		/*ng解耦 by mengfy at 20090810 */		
/*        EXEC SQL delete dCustType where id_no=to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","删除dCustType表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -17;
        }
*/
			init(dynStmt);
			sprintf(dynStmt, "SELECT MAIN_CODE FROM dCustType WHERE id_no = to_number(:v1)");
			EXEC SQL PREPARE sql_str FROM :dynStmt;
			EXEC SQL DECLARE ngcur02 cursor for sql_str;
			EXEC SQL OPEN ngcur02 using :sIn_User_id;	
			vCount = 0;
			for (;;)
			{
				init(sMain_Code_o);
				
				EXEC SQL FETCH ngcur02 INTO :sMain_Code_o;
				if ((0 != SQLCODE) && (1403 != SQLCODE))
				{
		            sprintf(sErrMessA,"%s","删除dCustType表失败!");
		            PUBLOG_DBDEBUG("pubCancelAcctInfo");
					pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
					EXEC SQL CLOSE ngcur02;
					return -25;
				}	
				if (1403 == SQLCODE) break;
					
				memset(v_parameter_array, 0, sizeof(v_parameter_array));
				strcpy(v_parameter_array[0], sIn_User_id);
				strcpy(v_parameter_array[1], sMain_Code_o);
				
				memset(&tCustTypeoldIndex, 0, sizeof(tCustTypeoldIndex));
				strcpy(tCustTypeoldIndex.sIdNo, sIn_User_id);
				strcpy(tCustTypeoldIndex.sMainCode, sMain_Code_o);
							
				v_ret = 0;
				v_ret = OrderDeleteCustType(ORDERIDTYPE_USER, sIn_User_id, 100, sIn_Op_code, atol(sIn_Login_accept),
						sIn_Login_no, "pubCancelAcctInfo", tCustTypeoldIndex, "", v_parameter_array);
				if (0 != v_ret)
				{
		            sprintf(sErrMessA,"%s","删除dCustType表失败!");
		            PUBLOG_DBDEBUG("pubCancelAcctInfo");
					pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
					EXEC SQL CLOSE ngcur02;
		            return -17;
				}					
				vCount ++; 
			}	
			EXEC SQL CLOSE ngcur02;		
			if (vCount == 0)
			{
		            sprintf(sErrMessA,"%s","删除dCustType表失败!");
		            PUBLOG_DBDEBUG("pubCancelAcctInfo");
					pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
		            return -17;
			}
		/*ng解耦 by mengfy at 20090810 */		
    }
    
    sV_Rent_flag=0;
    EXEC SQL select count(*) into :sV_Rent_flag from dPackMsg where id_no=to_number(:sIn_User_id);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        sprintf(sErrMessA,"%s","查询dPackMsg表失败!");
        PUBLOG_DBDEBUG("pubCancelAcctInfo");
		pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
        return -17;
    }
    pubLog_Debug(_FFL_, "pubCancelAcctInfo", "", "是否营销包=[%d][%s]",sV_Rent_flag,sIn_User_id);
    /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
    if(sV_Rent_flag>0) /* 是营销包用户 */
    {
        EXEC SQL insert into dPackMsgHis(id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , Update_accept , Update_time , Update_date , Update_login , 
                    Update_type , Update_code ,
                    GROUP_ID,ORG_ID)
                 select id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , to_number(:sIn_Login_accept) , 
    	            TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS') , to_number(:sIn_Total_date),
                    :sIn_Login_no , 'd' , :sIn_Op_code,
                    GROUP_ID,ORG_ID
                 from dPackMsg
                 where id_no=to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入dPackMsgHis表失败!");
           	PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -18;
        }
        
        EXEC SQL delete dPackMsg where id_no = to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","删除dPackMsg表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -19;
        }
        /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
        EXEC SQL insert into dBillPackMsgHis(id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                                         Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                                         Total_date , Op_time , Update_accept , Update_time , 
                                         Update_date , Update_login , Update_type , Update_code ,
                                         GROUP_ID,ORG_ID)
                 select id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                    Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                    Total_date , Op_time , to_number(:sIn_Login_accept) , 
    	            TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS') , to_number(:sIn_Total_date),
                    :sIn_Login_no , 'd' , :sIn_Op_code,
                    GROUP_ID,ORG_ID
                 from dBillPackMsg
                 where id_no=to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入dBillPackMsgHis表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -20;
        }
        
        EXEC SQL delete dBillPackMsg where id_no = to_number(:sIn_User_id);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","删除dBillPackMsg表失败!");
            PUBLOG_DBDEBUG("pubCancelAcctInfo");
			pubLog_DBErr(_FFL_,"pubCancelAcctInfo","sErrMessA","[%s]",sErrMessA);
            return -21;
        }
    }
   
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubCancelAcctInfo", "", " end function");
    
    return 0 ;
	
}




  /**
  * Auth: YuanBY
  * Name: pubCancelFeeInfo
  * Func: 消户操作冲正费用信息 
  * 输入参数  
  *     操作工号 
  *     用户ID    
  *     帐户ID    
  *     操作流水  
  *     开户流水  
  *     帐务日期  
  *     系统日期  
  *     操作代码  
  *     机构编码  
  *     系统备注  
  *     操作备注  
  *     IP地址    
  *     支票交款  
  *       
  *       
  * Date: 2003/10/14
  * Stat: Test Pass
  */
  
int pubCancelFeeInfo(char sIn_Login_noA[7] , char sIn_User_idA[23],
         char sIn_Acct_idA[23] , char sIn_Login_acceptA[39],
         char sIn_Open_acceptA[39] , char sIn_Total_dateA[21],
         char sIn_Sys_dateA[21] , char sIn_Op_codeA[5],
         char sIn_Org_codeA[11] , char sIn_Sys_noteA[61],
         char sIn_Op_noteA[61] , char sIn_Ip_addrA[16],
         char fIn_Check_payA[16],
         char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_Login_no[6+1];            /* 操作工号      */
        char    sIn_User_id[22+1];            /* 用户ID        */
        char    sIn_Acct_id[22+1];            /* 帐户ID        */
        char    sIn_Login_accept[38+1];       /* 操作流水      */
        char    sIn_Open_accept[38+1];        /* 开户流水      */
        char    sIn_Total_date[20+1];         /* 帐务日期      */
        char    sIn_Sys_date[20+1];           /* 系统日期      */
        char    sIn_Op_code[4+1];             /* 操作代码      */
        char    sIn_Org_code[10+1];           /* 机构编码      */
        char    sIn_Sys_note[60+1];           /* 系统备注      */
        char    sIn_Op_note[60+1];            /* 操作备注      */
        char    sIn_Ip_addr[15+1];            /* IP地址        */
        float   fIn_Check_pay;                /* 支票交款      */
        
        char    sV_YYYYMM[6+1];               /* 当前年月      */
        char    sV_Bank_code[5+1];            /* 银行代码      */
        char    sV_Check_no[20+1];            /* 支票代码      */
      
	    char    sql_str[3024]; 
	     
	    /*组织结构改造*/
	    char    vOrgId[10+1];
	    int     ret=0;    
	          
    EXEC SQL END DECLARE SECTION;
                
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "begin function");
                
    /* 初试化参数 */
    init(sIn_Login_no);
    init(sIn_User_id);
    init(sIn_Acct_id);
    init(sIn_Login_accept);
    init(sIn_Open_accept);
    init(sIn_Total_date);
    init(sIn_Sys_date);
    init(sIn_Op_code);
    init(sIn_Org_code);
    init(sIn_Sys_note);
    init(sIn_Op_note);
    init(sIn_Ip_addr);
    init(vOrgId);
                
    /* 取得输入参数 */
    strcpy(sIn_Login_no , sIn_Login_noA );
    strcpy(sIn_User_id , sIn_User_idA );
    strcpy(sIn_Acct_id , sIn_Acct_idA );
    strcpy(sIn_Login_accept , sIn_Login_acceptA );
    strcpy(sIn_Open_accept , sIn_Open_acceptA );
    strcpy(sIn_Total_date , sIn_Total_dateA );
    strcpy(sIn_Sys_date , sIn_Sys_dateA );
    strcpy(sIn_Op_code , sIn_Op_codeA );
    strcpy(sIn_Org_code , sIn_Org_codeA );
    strcpy(sIn_Sys_note , sIn_Sys_noteA );
    strcpy(sIn_Op_note , sIn_Op_noteA );
    strcpy(sIn_Ip_addr , sIn_Ip_addrA );
    fIn_Check_pay = atof(fIn_Check_payA);
                
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Login_no", "[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_User_id", "[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Acct_id", "[%s]",      sIn_Acct_id);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Login_accept", "[%s]", sIn_Login_accept);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Open_accept", "[%s]",      sIn_Open_accept);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Total_date", "[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Sys_date", "[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Op_code", "[%s]",      sIn_Op_code);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Org_code", "[%s]",      sIn_Org_code);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Sys_note", "[%s]",      sIn_Sys_note);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Op_note", "[%s]",      sIn_Op_note);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Ip_addr", "[%s]",      sIn_Ip_addr);
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "fIn_Check_pay", "[%f]",      fIn_Check_pay);
    #endif
    
    /*---组织机构改造获取org_id edit by zhangzhuo at 26/06/2009---begin*/
    ret=0;
	ret=sGetLoginOrgid(sIn_Login_no,vOrgId);
	if(ret!=0)
	{
		sprintf(sErrMessA,"%s","取工号归属错误");
        PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
		return -11;			
	}
	Trim(vOrgId);
	/*---组织机构改造获取org_id edit by zhaoqm at 26/06/2009---end*/
    
    /* 查询当前年月 */
    EXEC SQL select to_char(sysdate , 'yyyymm') into :sV_YYYYMM from dual;
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","查询当前年月失败!");
       	PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
        return -1;
    }
    
    /* 插入 wChgDay 表 , 金额为负 */
    
    init(sql_str);
    /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
    sprintf(sql_str,"insert into wChg%s(ID_NO , TOTAL_DATE , LOGIN_ACCEPT , SM_CODE , BELONG_CODE , PHONE_NO , \
            ORG_CODE , LOGIN_NO , OP_CODE , OP_TIME , MACHINE_CODE , \
            CASH_PAY , CHECK_PAY , SIM_FEE , MACHINE_FEE , INNET_FEE , CHOICE_FEE , \
            OTHER_FEE , HAND_FEE , DEPOSIT , BACK_FLAG , ENCRYPT_FEE , SYSTEM_NOTE , OP_NOTE , \
            ORG_ID,GROUP_ID )\
        select ID_NO , to_number(:sIn_Total_date) , to_number(:sIn_Login_accept) ,SM_CODE , BELONG_CODE , PHONE_NO ,\
            :sIn_Org_code , :sIn_Login_no , :sIn_Op_code , TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS') , MACHINE_CODE ,\
            (-1)*CASH_PAY , (-1)*CHECK_PAY , (-1)*SIM_FEE , (-1)*MACHINE_FEE , (-1)*INNET_FEE , (-1)*CHOICE_FEE , \
            (-1)*OTHER_FEE , (-1)*HAND_FEE , (-1)*DEPOSIT , '1' , ENCRYPT_FEE , :sIn_Sys_note, :sIn_Op_note ,\
            :vOrgId,GROUP_ID \
        from wChg%s where ID_NO = to_number(:sIn_User_id) and LOGIN_ACCEPT = to_number(:sIn_Open_accept)" , \
        sV_YYYYMM , sV_YYYYMM);
        
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "insert wChg%s' sql_str = [%s]",sV_YYYYMM,sql_str);
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "sIn_Open_accept = [%s]",sIn_Open_accept);
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "sIn_User_id = [%s]",sIn_User_id);
    
    EXEC SQL PREPARE ins_stmt From :sql_str; 
    EXEC SQL EXECUTE ins_stmt using :sIn_Total_date, :sIn_Login_accept,:sIn_Org_code,:sIn_Login_no,:sIn_Op_code,
                                    :sIn_Sys_date,:sIn_Sys_note,:sIn_Op_note , :vOrgId, :sIn_User_id,:sIn_Open_accept; 
    
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","插入wChgYYYYMM表失败!");
        PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
        return -2;
    }    
           
    /* 修改wChgYYYYMM表 , 旧开户流水的  Back_Flag='1' */
    sprintf(sql_str,"update wChg%s set Back_Flag='1' where ID_NO = to_number(:sIn_User_id)  and LOGIN_ACCEPT = to_number(:sIn_Login_accept) ",sV_YYYYMM); 
    
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "update wChg%s' sql_str = [%s]",sV_YYYYMM,sql_str); 

    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "sIn_User_id = [%s]",sIn_User_id); 
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "sIn_Login_accept = [%s]",sIn_Login_accept); 
    
    EXEC SQL PREPARE ins_stmt From :sql_str; 
    EXEC SQL EXECUTE ins_stmt using :sIn_User_id , :sIn_Login_accept; 
    
    if(SQLCODE!=0)
    {
        sprintf(sErrMessA,"%s","修改wChgYYYYMM表失败!");
        PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
        return -3;
    }           
             
    /* 插入 wSimSalesDay 表 , 金额为负 */
    /* 修改 wSimSalesDay 表 , 旧开户流水的  Back_Flag='1' */  
    
    /* 修改 wMsisdnOpen 表 , 旧开户流水的  Back_Flag='1' */     
/*    
    fIn_Check_pay = 0; 
*/
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "fIn_Check_pay", "[%f]",      fIn_Check_pay);
    if (fIn_Check_pay > 0)   
    {
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "退支票[%f]",      fIn_Check_pay);
    	/* 查询银行代码 , 支票代码 */
        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "sIn_Open_accept=[%s]",      sIn_Open_accept);
    	EXEC SQL select BANK_CODE , CHECK_NO into :sV_Bank_code, :sV_Check_no
    	         from wCheckOpr where LOGIN_ACCEPT = to_number(:sIn_Open_accept);
    	if(SQLCODE!=0)
    	{
            sprintf(sErrMessA,"%s","查询wCheckOpr表失败!");
            PUBLOG_DBDEBUG("pubCancelFeeInfo");
			pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
            return -4;
        }  
    	
    	Trim(sV_Bank_code);
    	Trim(sV_Check_no);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sV_Bank_code", "[%s]",      sV_Bank_code);
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sV_Check_no", "[%s]",      sV_Check_no);
        #endif
    	
        /* 插入 wCheckOpr 表 , 金额为负 */
        init(sql_str);
        /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
        /*pubCancelCheckFee insert wCheckOpr begin at 20091010*/
        /*
        sprintf(sql_str , "insert into wCheckOpr(TOTAL_DATE , LOGIN_ACCEPT , ORG_CODE , LOGIN_NO ,\
             OP_CODE , BANK_CODE , CHECK_NO , CHECK_PAY , OP_TIME , OP_NOTE ,ORG_ID)\
             select to_number(:sIn_Total_date) , to_number(:sIn_Login_accept) , :sIn_Org_code ,:sIn_Login_no,\
                 :sIn_Op_code, BANK_CODE , CHECK_NO , (-1)*CHECK_PAY ,\
                 TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),:sIn_Op_note,:vOrgId from wCheckOpr \
                 where LOGIN_ACCEPT = to_number(:sIn_Open_accept) ");
        
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sql_str", "[%s]",      sql_str);
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Login_accept", "[%s]",      sIn_Login_accept);
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sIn_Open_accept", "[%s]",      sIn_Open_accept);


        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubCancelFeeInfo", "sql_str", "[%s]",      sql_str);
        #endif
        
 		EXEC SQL PREPARE ins_stmt FROM :sql_str;
        EXEC SQL EXECUTE ins_stmt using :sIn_Total_date,:sIn_Login_accept,:sIn_Org_code,:sIn_Login_no,:sIn_Op_code,
                                        :sIn_Sys_date,:sIn_Op_note,:vOrgId,:sIn_Open_accept; 
 			

        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入wCheckOpr表失败!");
            PUBLOG_DBDEBUG("pubCancelFeeInfo");
			pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
            return -5;
        }
        */    
        /*pubCancelCheckFee insert wCheckOpr end at 20091010*/
        /*NG 解偶  业务工单改造 Update by miaoyl 20091002 begin*/
        /* 修改 dCheckPrepay 
        EXEC SQL update dCheckPrepay set Check_Prepay = Check_Prepay + :fIn_Check_pay ,
                       OP_TIME = TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS')
                 where BANK_CODE = :sV_Bank_code
                     and CHECK_NO = :sV_Check_no;
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","修改dCheckPrepay表失败!");
            PUBLOG_DBDEBUG("pubCancelFeeInfo");
			pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
            return -6;
        }  */
        /*NG 解偶  业务工单改造 Update by miaoyl 20091002 end*/ 
    	
    }  /*  end  --   if (fIn_Check_pay > 0)    */
    
    /* 插入 wPayYYYYMM 表 , 金额为负 */
    /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
    
    /*NG 解偶  业务工单改造 Update by miaoyl 20091002 begin*/
    /*
    sprintf(sql_str,"insert into wPay%s(CONTRACT_NO , ID_NO , TOTAL_DATE , LOGIN_ACCEPT , CON_CUST_ID , PHONE_NO , \
              BELONG_CODE , FETCH_NO , SM_CODE , LOGIN_NO , ORG_CODE , \
              OP_TIME , OP_CODE , PAY_TYPE , ADD_MONEY , PAY_MONEY , \
              OUT_PREPAY , PREPAY_FEE , CURRENT_PREPAY , PAYED_OWE ,DELAY_FEE , OTHER_FEE ,\
              DEAD_FEE , BAD_FEE , BACK_FLAG ,ENCRYPT_FEE , RETURN_CODE , PAY_NOTE, \
              ORG_ID,GROUP_ID)\
          select CONTRACT_NO , ID_NO , to_number(:sIn_Total_date) ,to_number(:sIn_Login_accept) ,CON_CUST_ID, PHONE_NO,\
              BELONG_CODE , FETCH_NO , SM_CODE ,:sIn_Login_no , :sIn_Org_code , \
              TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'), :sIn_Op_code , PAY_TYPE, (-1)*ADD_MONEY , (-1)*PAY_MONEY ,\
              (-1)*OUT_PREPAY , (-1)*PREPAY_FEE , (-1)*CURRENT_PREPAY , (-1)*PAYED_OWE , (-1)*DELAY_FEE, (-1)*OTHER_FEE,\
              (-1)*DEAD_FEE , (-1)*BAD_FEE ,'1',ENCRYPT_FEE,RETURN_CODE , nvl(:sIn_Op_note,' '), \
              :vOrgId,GROUP_ID \
          from wPay%s where LOGIN_ACCEPT = to_number(:sIn_Open_accept) and ID_NO = to_number(:sIn_User_id) \
              and Back_Flag ='0' ",sV_YYYYMM , sV_YYYYMM );
   
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "insert wPay%s' sql_str = [%s]",sV_YYYYMM,sql_str); 
    
    EXEC SQL PREPARE ins_stmt From :sql_str; 
    EXEC SQL EXECUTE ins_stmt using :sIn_Total_date ,:sIn_Login_accept,:sIn_Login_no,:sIn_Org_code,:sIn_Sys_date,
                                    :sIn_Op_code ,:sIn_Op_note,:vOrgId,:sIn_Open_accept,:sIn_User_id; 
    
    if ((SQLCODE!=0) && (SQLCODE!=1403))
    {
        sprintf(sErrMessA,"%s","插入wPayYYYYMM表失败!");
        PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
        return -7;
    }        
    */
    /* 修改 wPayYYYYMM 表 , 旧开户流水的  Back_Flag='1' */  
    /*
    sprintf(sql_str,"update wPay%s set Back_Flag='1' where LOGIN_ACCEPT = to_number(:sIn_Open_accept) and ID_NO = to_number(:sIn_User_id) ", sV_YYYYMM );               
    
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "update wPay%s' sql_str = [%s]",sV_YYYYMM,sql_str); 
    
    EXEC SQL PREPARE ins_stmt From :sql_str; 
    EXEC SQL EXECUTE ins_stmt using :sIn_Open_accept,:sIn_User_id; 
    
    if ((SQLCODE!=0) && (SQLCODE!=1403))
    {
        sprintf(sErrMessA,"%s","修改wPayYYYYMM表失败!");
         PUBLOG_DBDEBUG("pubCancelFeeInfo");
		pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
        return -8;
    }               
    */ 
    /*NG 解偶  业务工单改造 Update by miaoyl 20091002 end*/
    /* 插入 wLoginOprYYYYMM 表 , 金额为负 */
    /* 注意：当是批量退户时，由于批量开户插入wLoginOprYYYYMM表是一条总记录，是没有ID的，所以下面的语句是0条记录，所以在批量退户的主程序中增加插入该表的语句，为了防止开户时插入了单个ID，加上IF判断 */    
    if(strncmp(sIn_Op_code,"1124",4)!=0 && strncmp(sIn_Op_code,"1125",4)!=0 && strncmp(sIn_Op_code,"1126",4)!=0&& strncmp(sIn_Op_code,"g081",4)!=0)
    {
        sprintf(sql_str,"insert into wLoginOpr%s(TOTAL_DATE , LOGIN_ACCEPT ,OP_CODE , PAY_TYPE , PAY_MONEY , \
                SM_CODE , ID_NO , PHONE_NO , ORG_CODE , LOGIN_NO , OP_TIME , \
                OP_NOTE , IP_ADDR  ,ORG_ID)\
            select to_number(:sIn_Total_date) , to_number(:sIn_Login_accept) , :sIn_Op_code , PAY_TYPE , (-1)*PAY_MONEY , \
                SM_CODE , ID_NO , PHONE_NO ,:sIn_Org_code , :sIn_Login_no, TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),\
                :sIn_Op_note,:sIn_Ip_addr ,:vOrgId\
            from wLoginOpr%s where  LOGIN_ACCEPT = to_number(:sIn_Open_accept) and ID_NO = to_number(:sIn_User_id)", \
            sV_YYYYMM , sV_YYYYMM );

        pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "insert wLoginOpr%s' sql_str = [%s]",sV_YYYYMM,sql_str); 
    
        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt using :sIn_Total_date , :sIn_Login_accept ,:sIn_Op_code ,:sIn_Org_code,:sIn_Login_no,
                                        :sIn_Sys_date , :sIn_Op_note,:sIn_Ip_addr,:vOrgId,:sIn_Open_accept ,:sIn_User_id; 
    
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入wLoginOprYYYYMM表失败!");
            PUBLOG_DBDEBUG("pubCancelFeeInfo");
			pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
            return -9;
        }
    }        
    
    /* 删除 wConInterest 表 */  
    if ((strcmp(sIn_Op_code,"1123")!=0) && (strcmp(sIn_Op_code,"1126")!=0)&& (strcmp(sIn_Op_code,"g081")!=0))
    {
        EXEC SQL delete from wConInterest
                 where CONTRACT_NO = to_number(:sIn_Acct_id)
                     and LOGIN_ACCEPT = to_number(:sIn_Open_accept);
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","删除wConInterest表失败!");
            PUBLOG_DBDEBUG("pubCancelFeeInfo");
			pubLog_DBErr(_FFL_,"pubCancelFeeInfo","sErrMessA","[%s]",sErrMessA);
            return -10;
        }    
    }
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubCancelFeeInfo", "", "end function pubCancelFeeInfo ----------");
    
    return 0 ;
	
}



  /**
  * Auth: YuanBY
  * Name: pubCancelBill
  * Func: 冲正资费信息 
  * 输入参数    
  *     用户ID     
  *
  * Date: 2003/11/26
  * Stat: Test Pass
  */
  
int pubCancelBill(
         char  sIn_User_idA[23] ,  char  sIn_Phone_noA[16] ,
         char  sIn_Sys_dateA[21] ,  char  sIn_Op_codeA[5] ,
         char  sIn_Login_noA[7] ,  char  sIn_Login_acceptA[39] ,
         char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_User_id[22+1];            /* 用户ID        */
        char    sIn_Phone_no[15+1];           /* 手机号码      */
        char    sIn_Sys_date[20+1];           /* 系统日期      */
        char    sIn_Op_code[4+1];             /* 操作代码      */
        char    sIn_Login_no[6+1];            /* 操作工号      */
        char    sIn_Login_accept[38+1];       /* 操作流水      */
		
        char    sV_Sqlstr[2048];              /* sql字符串     */
        char    sV_Mode_code[8+1];            /* 资费代码      */
        int     iV_FuncRet = 0 ;              /* 函数返回值    */
        char    sV_Errmsg[200+1];             /* 错误信息      */
        char    sV_Oldaccept[22+1];           /* 要取消套餐的申请流水 */
        
    EXEC SQL END DECLARE SECTION;
    
    pubLog_Debug(_FFL_, "pubCancelBill", "", "begin function");
    
    /* 初试化参数 */
    init(sIn_User_id);
    init(sIn_Phone_no);
    init(sIn_Sys_date);
    init(sIn_Op_code);
    init(sIn_Login_no);
    init(sIn_Login_accept);
    
    init(sV_Sqlstr);
    init(sV_Mode_code);
    init(sV_Errmsg);
    init(sV_Oldaccept);
    
    /* 取得输入参数 */
    strcpy(sIn_User_id,sIn_User_idA);
    strcpy(sIn_Phone_no,sIn_Phone_noA);
    strcpy(sIn_Sys_date,sIn_Sys_dateA);
    strcpy(sIn_Op_code,sIn_Op_codeA);
    strcpy(sIn_Login_no,sIn_Login_noA);
    strcpy(sIn_Login_accept,sIn_Login_acceptA);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_User_id", "[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_Phone_no", "[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_Sys_date", "[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_Op_code", "[%s]",      sIn_Op_code);
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_Login_no", "[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubCancelBill", "sIn_Login_accept", "[%s]",      sIn_Login_accept);
    #endif
    
    /* 查询 dBillCustDetail 表 */
    sprintf(sV_Sqlstr,"select distinct MODE_CODE,login_accept from dBillCustDetail%c where id_no=to_number(:sIn_User_id)  and mode_status='Y'",sIn_User_id[strlen(sIn_User_id)-1]); 
    
    EXEC SQL PREPARE sele_sql from :sV_Sqlstr ;
    EXEC SQL DECLARE cur010 CURSOR for sele_sql;
    EXEC SQL OPEN cur010 using :sIn_User_id; 
    EXEC SQL FETCH cur010 INTO :sV_Mode_code,:sV_Oldaccept;
    if (SQLCODE != 0)
    {
        if (SQLCODE == 1403)
        {
            sprintf(sErrMessA,"%s","SUCCESSED!");  
           PUBLOG_DBDEBUG("pubCancelFunc");
			pubLog_DBErr(_FFL_,"pubCancelFunc","sErrMessA","[%s]",sErrMessA); 
            return 0;
        }
        else
        {
            sprintf(sErrMessA,"%s","查询dBillCustDetail表出错!");
            PUBLOG_DBDEBUG("pubCancelFunc");
			pubLog_DBErr(_FFL_,"pubCancelFunc","sErrMessA","[%s]",sErrMessA);  
            return -1;	
        }	
    }
    
    for(;SQLCODE==0;)
    {
        Trim(sV_Mode_code);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubCancelBill", "", "sV_Mode_code=[%s]",sV_Mode_code); 
        #endif	
        
        iV_FuncRet = pubBillEndMode(sIn_Phone_no,sV_Mode_code,sV_Oldaccept,sIn_Sys_date,
              sIn_Op_code,sIn_Login_no,sIn_Login_accept,sIn_Sys_date,sV_Errmsg);
        
        if (iV_FuncRet !=0)
        {
            pubLog_Debug(_FFL_, "pubCancelBill", "", "call  pubBillEndMode ret = [%d]",iV_FuncRet);
            sprintf(sErrMessA,"%s[%s]","调用pubBillEndMode出错!",sV_Errmsg);
			pubLog_Debug(_FFL_,"pubCancelFunc","sErrMessA","[%s]",sErrMessA);  
            return -2;	
        }
        
        EXEC SQL FETCH cur010 INTO :sV_Mode_code;
    }
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubCancelBill", "", "end function");
    
    return 0 ;
	
}

  /**
  * Auth: YuanBY
  * Name: pubCancelCheckFee
  * Func: 消户操作冲正支票信息 
  * 输入参数  
  *     操作工号 
  *     开户流水
  *     帐务日期
  *     操作流水
  *     机构编码
  *     操作工号
  *     操作代码
  *     系统日期
  *     操作备注
  *     支票交款
  *       
  *       
  * Date: 2003/10/14
  * Stat: Test Pass
  */
  
int pubCancelCheckFee(
         char sIn_Open_acceptA[39] , char sIn_Total_dateA[21],
         char sIn_Login_acceptA[39] , char sIn_Org_codeA[11],
         char sIn_Login_noA[7] , char sIn_Op_codeA[5],
         char sIn_Sys_dateA[21] , char sIn_Op_noteA[61],
         char fIn_Check_payA[16],
         char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_Open_accept[38+1];        /* 开户流水      */
        char    sIn_Total_date[20+1];         /* 帐务日期      */
        char    sIn_Login_accept[38+1];       /* 操作流水      */
        char    sIn_Org_code[10+1];           /* 机构编码      */
        char    sIn_Login_no[6+1];            /* 操作工号      */
        char    sIn_Op_code[4+1];             /* 操作代码      */
        char    sIn_Sys_date[20+1];           /* 系统日期      */
        char    sIn_Op_note[60+1];            /* 操作备注      */
        float   fIn_Check_pay;                /* 支票交款      */
        
        char    sV_Bank_code[5+1];            /* 银行代码      */
        char    sV_Check_no[20+1];            /* 支票代码      */
      
	    char    sql_str[3024];   
	     
	     /*组织结构改造*/
	    char    vOrgId[10+1];
	    int     ret=0;   
	         
    EXEC SQL END DECLARE SECTION;
                
    pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "begin function");
                
    /* 初试化参数 */
    init(sIn_Open_accept);
    init(sIn_Total_date);
    init(sIn_Login_accept);
    init(sIn_Org_code);
    init(sIn_Login_no);
    init(sIn_Op_code);
    init(sIn_Sys_date);
    init(sIn_Op_note);
    init(sV_Bank_code);
    init(sV_Check_no);
    init(sql_str);
                
    /* 取得输入参数 */
    strcpy(sIn_Open_accept , sIn_Open_acceptA );
    strcpy(sIn_Total_date , sIn_Total_dateA );
    strcpy(sIn_Login_accept , sIn_Login_acceptA );
    strcpy(sIn_Org_code , sIn_Org_codeA );
    strcpy(sIn_Login_no , sIn_Login_noA );
    strcpy(sIn_Op_code , sIn_Op_codeA );
    strcpy(sIn_Sys_date , sIn_Sys_dateA );
    strcpy(sIn_Op_note , sIn_Op_noteA );
    fIn_Check_pay = atof(fIn_Check_payA);
                
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Open_accept", "[%s]",      sIn_Open_accept);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Total_date", "[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Login_accept", "[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Org_code", "[%s]",      sIn_Org_code);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Login_no", "[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Op_code", "[%s]",      sIn_Op_code);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Sys_date", "[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "sIn_Op_note", "[%s]",      sIn_Op_note);
        pubLog_Debug(_FFL_, "pubCancelCheckFee", "fIn_Check_pay", "[%f]",      fIn_Check_pay);
    #endif
   
    /*---组织机构改造获取org_id edit by zhangzhuo at 26/06/2009---begin*/
    ret=0;
	ret=sGetLoginOrgid(sIn_Login_no,vOrgId);
	if(ret!=0)
	{
		sprintf(sErrMessA,"%s","取工号归属错误");
        PUBLOG_DBDEBUG("pubCancelCheckFee");
		pubLog_DBErr(_FFL_,"pubCancelCheckFee","sErrMessA","[%s]",sErrMessA);
		return -3;			
	}
	Trim(vOrgId);
	/*---组织机构改造获取org_id edit by zhaoqm at 26/06/2009---end*/
    
    if (fIn_Check_pay > 0)   
    {
    	/* 查询银行代码 , 支票代码 */
    	EXEC SQL select BANK_CODE , CHECK_NO into :sV_Bank_code, :sV_Check_no
    	         from wCheckOpr where LOGIN_ACCEPT = to_number(:sIn_Open_accept);
    	if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","查询wCheckOpr表失败!");
            pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -4;
        }  
    	
    	Trim(sV_Bank_code);
    	Trim(sV_Check_no);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "sV_Bank_code      =[%s]",      sV_Bank_code);
            pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "sV_Check_no      =[%s]",      sV_Check_no);
        #endif
    	
        /* 插入 wCheckOpr 表 , 金额为负 */
        init(sql_str);
        /*---组织机构改造插入字段 edit by ZHAOQM at 2009-04-07---*/
        sprintf(sql_str , "insert into wCheckOpr(TOTAL_DATE , LOGIN_ACCEPT , ORG_CODE , LOGIN_NO ,\
             OP_CODE , BANK_CODE , CHECK_NO , CHECK_PAY , OP_TIME , OP_NOTE ,ORG_ID)\
             select to_number(:sIn_Total_date) , to_number(:sIn_Login_accept) , :sIn_Org_code ,:sIn_Login_no,\
                 :sIn_Op_code, BANK_CODE , CHECK_NO , (-1)*CHECK_PAY ,\
                 TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),:sIn_Op_note,:vOrgId from wCheckOpr \
                 where LOGIN_ACCEPT = to_number(:sIn_Open_accept) ");
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "sql_str      =[%s]",      sql_str);
         #endif
        
        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt using :sIn_Total_date,:sIn_Login_accept,:sIn_Org_code,:sIn_Login_no,:sIn_Op_code,
                                        :sIn_Sys_date,:sIn_Op_note, :vOrgId, :sIn_Open_accept; 

        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","插入wCheckOpr表失败!");
            PUBLOG_DBDEBUG("pubCancelCheckFee");
			pubLog_DBErr(_FFL_,"pubCancelCheckFee","sErrMessA","[%s]",sErrMessA);
            return -5;
        }    
        
        /*NG 解偶  业务工单改造 Update by miaoyl 20091002 begin*/
        /* 修改 dCheckPrepay */
        /*
        EXEC SQL update dCheckPrepay set Check_Prepay = Check_Prepay + :fIn_Check_pay ,
                       OP_TIME = TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS')
                 where BANK_CODE = :sV_Bank_code
                     and CHECK_NO = :sV_Check_no;
        if(SQLCODE!=0)
        {
            sprintf(sErrMessA,"%s","修改dCheckPrepay表失败!");
            PUBLOG_DBDEBUG("pubCancelCheckFee");
			pubLog_DBErr(_FFL_,"pubCancelCheckFee","sErrMessA","[%s]",sErrMessA);
            return -6;
        } */
        /*NG 解偶  业务工单改造 Update by miaoyl 20091002 begin*/  
    	
    }  /*  end  --   if (fIn_Check_pay > 0)    */
    
   
   
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubCancelCheckFee", "", "end function");
    
    return 0 ;
	
}

