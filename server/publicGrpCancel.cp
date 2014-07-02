/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
/***************************************
 *function  �����û������������ú���
 *author    Huaxm
 *created   2006-11-08 15:43
****************************************/


#include "boss_srv.h"
#include "publicsrv.h"
#include "pubLog.h"
/*����ҵ�����ȡ����sSmProduct���sm_code*/
#if PROVINCE_RUN == PROVINCE_JILIN
    #define VPMN_SM_CODE           "vp" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
    #define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_IP_SM_CODE      "04" /*IPר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_DIANLU_SM_CODE  "13" /*��·����ҵ���Ʒ�ƴ���*/
#else
    #define VPMN_SM_CODE           "vp" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
    #define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_IP_SM_CODE      "ip" /*IPר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
    #define ZHUANX_DIANLU_SM_CODE  "dl" /*��·����ҵ���Ʒ�ƴ���*/
#endif

EXEC SQL INCLUDE SQLCA;
/**
 *@function information
 *@name fPubCancelAllBill
 *@description  ��������
   ɾ��һ���û��������ʷ�
 *@author     huaxm
 *@created    2006-11-08 09:53
 */
int fPubCancelAllBill(const char *idNo,  const char *userNo,
   const char *effectTime,  const char *opCode,
   const char *loginNo,  const char *loginAccept, const char *srvMsgTableName,
   const char *opTime, char *retMsg)
{
    EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[2048];      /* sql�ַ���   */
    char productCode[8+1];    /* ��Ʒ����    */
    char serviceCode[4+1];    /* �������    */
    /*char productLevel[1+1];*/       /* ����Ʒ��־ */
    int  ret = 0 ;      /* ��������ֵ  */
    char oldAccept[22+1];     /* Ҫȡ���ײ͵�������ˮ */
    char beginTime[17 + 1];   /*��¼������Ʒ�Ŀ�ʼʱ��*/
    char endTime[17 + 1];     /*��Ʒ�����ֹͣ��Ʒ*/
    
    const char* p_prodcod =DEFAULT_PRODUCT_CODE;
    EXEC SQL END DECLARE SECTION;

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","idNo          =[%s]",    idNo);
    pubLog_Debug(_FFL_,"","","userNo        =[%s]",    userNo);
    pubLog_Debug(_FFL_,"","","effectTime    =[%s]",    effectTime);
    pubLog_Debug(_FFL_,"","","opCode        =[%s]",    opCode);
    pubLog_Debug(_FFL_,"","","loginNo       =[%s]",    loginNo);
    pubLog_Debug(_FFL_,"","","loginAccept   =[%s]",    loginAccept);
    pubLog_Debug(_FFL_,"","","srvMsgTableName=[%s]",   srvMsgTableName);
    pubLog_Debug(_FFL_,"","","opTime        =[%s]",    opTime);
    pubLog_Debug(_FFL_,"","","call function fPubCancelAllBill()");
#endif

  /* ȡ������ */
    init(dynStmt);
    /*liuhao edit �󶨱��� 20090429 begin*/
    /*
    sprintf(dynStmt,  
        "select distinct service_code,login_accept,begin_time"
        "  from %s%c"
        " where id_no=to_number(:idNo)"
        "   and product_code = '%s'"
        "   and end_time > to_char(sysdate, 'YYYYMMDD HH24:MI:SS')",
        srvMsgTableName,idNo[strlen(idNo)-1], DEFAULT_PRODUCT_CODE);  
        */
        
    sprintf(dynStmt,  
        "select distinct service_code,login_accept,begin_time"
        "  from %s%c"
        " where id_no=to_number(:idNo)"
        "   and product_code = :v1"
        "   and end_time > to_char(sysdate, 'YYYYMMDD HH24:MI:SS')",
        srvMsgTableName,idNo[strlen(idNo)-1]);  
        
    EXEC SQL PREPARE sel_dSrvMsg2 from :dynStmt;
    EXEC SQL declare cur_dSrvMsg2 cursor for sel_dSrvMsg2;
    EXEC SQL OPEN cur_dSrvMsg2 using :idNo,:p_prodcod;
    /* liuhao edit end*/	
    for(;;)
    {
        init(serviceCode);
        init(oldAccept);
        init(beginTime);
        EXEC SQL  FETCH cur_dSrvMsg2 INTO :serviceCode, :oldAccept, :beginTime;
        if (SQLCODE != 0) break;
        Trim(oldAccept);  
          
        init(endTime);
        if(strlen(effectTime) < 4)/*û��������Чʱ��*/
        {
            strncpy(endTime,beginTime,17);
        }
        else
        {
            strncpy(endTime,effectTime,17);
        }
        pubLog_Debug(_FFL_,"","","fPubCancelAllBill.endService endTime = [%s]  ",endTime);

        ret = endService(userNo,serviceCode,oldAccept,endTime,
            opCode,loginNo,loginAccept,opTime,retMsg);
        if (ret !=0)
        {
            pubLog_Debug(_FFL_,"","","call  endService ret = [%d]",ret);
            PUBLOG_DBDEBUG("");
            EXEC SQL CLOSE cur_dSrvMsg2;
            return ret;
        }
    }
    EXEC SQL CLOSE cur_dSrvMsg2;
    
    /* ȡ����Ʒ */
    init(dynStmt);
    /*liuhao 20090429 edit �󶨱��� begin*/
    /*sprintf(dynStmt,  
        " select distinct product_code,login_accept,product_level,begin_time"
        " select distinct product_code,login_accept,begin_time"
        "  from %s%c"
        " where id_no=to_number(:idNo)"
        "   and product_code != '%s'"
        "   and end_time > sysdate ",
       " order by product_level desc",
        srvMsgTableName,idNo[strlen(idNo)-1], DEFAULT_PRODUCT_CODE);
        */
     sprintf(dynStmt,   
       " select distinct product_code,login_accept,begin_time"
        "  from %s%c"
        " where id_no=to_number(:idNo)"
        "   and product_code != :v1"
        "   and end_time > sysdate ",  
        srvMsgTableName,idNo[strlen(idNo)-1]);      
    pubLog_Debug(_FFL_,"","","dynStmt=[%s]",dynStmt);
    EXEC SQL PREPARE sel_dSrvMsg1 from :dynStmt;
    EXEC SQL declare cur_dSrvMsg1 cursor for sel_dSrvMsg1;
    EXEC SQL OPEN cur_dSrvMsg1 using :idNo,:p_prodcod;
    /*liuhao end edit*/
    for(;;)
    {
        init(productCode);
        init(oldAccept);
        /*init(productLevel);*/
        init(beginTime);
        /*EXEC SQL  FETCH cur_dSrvMsg1 INTO :productCode, :oldAccept, :productLevel, :beginTime;*/
        EXEC SQL  FETCH cur_dSrvMsg1 INTO :productCode, :oldAccept, :beginTime;
        if (SQLCODE != 0) break;
        Trim(oldAccept);
        
        
        init(endTime);
        if(strlen(effectTime) < 4)/*û��������Чʱ��*/
        {
            strncpy(endTime,beginTime,17);
        }
        else
        {
            strncpy(endTime,effectTime,17);
        }
        pubLog_Debug(_FFL_,"","","fPubCancelAllBill.endProduct endTime = [%s]  ",endTime);
            
        ret = endProduct(userNo,productCode,oldAccept,endTime,
            opCode,loginNo,loginAccept,opTime,retMsg);
        if (ret !=0)
        {
            pubLog_Debug(_FFL_,"","","call  endProduct ret = [%d]",ret);
            PUBLOG_DBDEBUG("");
            EXEC SQL CLOSE cur_dSrvMsg1;
            return ret;
        }
    }
    EXEC SQL CLOSE cur_dSrvMsg1;
    
    return 0 ;

}


/************************************************************************
��������:fPubCancelGrpUser
����ʱ��:2006/11/01
������Ա:Huaxm
��������:�����û��������ú���
������
         typedef struct tPubCancelGrpUserTag
         {
           char sGrpId_No[14+1];      / * ����-�����û�ID_NO* /
           char sLogin_No[6+1];       / * ����-����Ա������* /
           char sOp_Code[4+1];        / * ����-��������* /
           char sOp_Time[20+1];       / * ����-ϵͳʱ��* /
           char sSysAccept[14+1];     / * ����-��ˮ��*  /
           char sOp_Note[60+1];       / * ����-��������*  /
           char sReturnMsg[60+1];     / * ���-���ش�����Ϣ*  /
         }tPubCancelGrpUser;
�� �� ֵ:0������ȷ����,��������
***********************************************************************
int fPubCancelGrpUser(tPubCancelGrpUser *funcIn)
{
    int  retCode=0;
  EXEC SQL BEGIN DECLARE SECTION;
    char sGrpIdNo[14+1];               �����û�ID_NO
    char sLoginNo[6+1];                ����Ա������
    char sOldSysAccept[14+1];          ��ˮ��
    char sOpCode[4+1];                 ��������
    char sOpTime[20+1];                ϵͳʱ��
    char sSysAccept[14+1];             ��ˮ��
    char sOpNote[60+1];                ��������

    char *return_message = &funcIn->sReturnMsg;             

	 ������Ϣ
    char *return_message = funcIn->sReturnMsg;              ������Ϣ
    char sTotalDate[8+1];             
        
    char sCustId[22+1];               
    char sGrpUserNo[22+1];            
    char sSmCode[2+1];                
    char sProductCode[8+1];           
    char sBelongCode[9+1];            
    char sContractNo[14+1];           
    char vGroupId[10+1];              
    char sGrpProductCode[2+1];        
    char sOrgId[10+1];                
    char sTmpApnNo[4+1];              
    char sMemberId[14+1];             
    int  iAccountIdNbr = 0;
    int  iBillFlag = 0;
    
  EXEC SQL END DECLARE SECTION;
    char sBuffer[2048];
    int  iCount = 0;

    init(sGrpIdNo);
    init(sLoginNo);
    init(sOldSysAccept);
    init(sOpCode);
    init(sOpTime);
    init(sSysAccept);
    init(sOpNote);
    init(sTotalDate);
    
    init(sCustId);
    init(sGrpUserNo);
    init(sSmCode);
    init(sProductCode);
    init(sBelongCode);
    init(sContractNo);
    init(vGroupId);
    init(sGrpProductCode);
    init(sOrgId);
    init(sTmpApnNo);
    init(sMemberId);
    init(sBuffer);

    ������ֵ
    strcpy(sGrpIdNo,      funcIn->sGrpId_no);
    strcpy(sLoginNo,      funcIn->sLogin_no);
    strcpy(sOldSysAccept, funcIn->sOldSys_accept);
    strcpy(sOpCode,       funcIn->sOp_code);
    strcpy(sOpTime,       funcIn->sOp_time);
    strcpy(sSysAccept,    funcIn->sSys_accept);
    strcpy(sOpNote,       funcIn->sOp_note);
    strncpy(sTotalDate,   sOpTime, 8);
    
    Trim(sGrpIdNo);
    Trim(sLoginNo);
    Trim(sOldSysAccept);
    Trim(sOpCode);
    Trim(sOpTime);
    Trim(sSysAccept);
    Trim(sOpNote);

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","sGrpIdNo   =[%s]",    sGrpIdNo);
    pubLog_Debug(_FFL_,"","","sLoginNo   =[%s]",    sLoginNo);
    pubLog_Debug(_FFL_,"","","sOldSysAccept =[%s]", sOldSysAccept);
    pubLog_Debug(_FFL_,"","","sOpCode    =[%s]",    sOpCode);
    pubLog_Debug(_FFL_,"","","sOpTime    =[%s]",    sOpTime);
    pubLog_Debug(_FFL_,"","","sSysAccept =[%s]",    sSysAccept);
    pubLog_Debug(_FFL_,"","","sOpNote    =[%s]",    sOpNote);
    pubLog_Debug(_FFL_,"","","sTotalDate =[%s]",    sTotalDate);
#endif

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","��ѯdGrpUserMsg����Ϣ");
#endif
    EXEC SQL SELECT cust_id, user_no, sm_code, product_code, account_id,
                    region_code||district_code||town_code, group_id
               INTO :sCustId, :sGrpUserNo, :sSmCode, :sProductCode, :sContractNo,
                    :sBelongCode, :vGroupId
               FROM dGrpUserMsg
              WHERE id_no = to_number(:sGrpIdNo)
                AND bill_date > sysdate;
    if(SQLCODE == 1403){
        sprintf(return_message,"�����û�[%s]������",sGrpIdNo);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190501",return_message); 
        return 190501;
    }
    else if(SQLCODE != 0){
        sprintf(return_message,"��ѯdGrpUserMsg����Ϣ[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190502",return_message); 
        return 190502;
    }
    Trim(sCustId  );
    Trim(sGrpUserNo);
    Trim(sSmCode);
    Trim(sProductCode);
    Trim(sContractNo);
    Trim(sBelongCode);
    Trim(vGroupId);
    
     �����л��г�Ա�Ļ������������
    iCount = 0;
    EXEC SQL select count(*) into :iCount
               from dGrpUserMebMsg
              where id_no = to_number(:sGrpIdNo);
    if (SQLCODE != 0)
    {
        sprintf(return_message, "��ѯ��dGrpUserMebMsg����SQLCODE=[%d]", SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190503",return_message); 
        return 190503;
    }
    else if (iCount != 0)
    {
        sprintf(return_message, "����[�����û�ID��%s]���г�Ա��������ɾ��", sGrpIdNo);
        pubLog_Debug(_FFL_,"","190504",return_message);
        return 190504;
    }

    ��ѯ����Ա��org_id
    
    EXEC SQL select org_id into :sOrgId from dLoginMsg where login_no = :sLoginNo;
    if(SQLCODE != 0)
    {
        sprintf(return_message,"��ѯdLoginMsg��org_idʧ��[%d]",SQLCODE);
        printf("%s", return_message);
        return 190505;
    }
              
    
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","��ѯ���ſͻ�ϵͳ�Ĳ�Ʒ����");
#endif
    EXEC SQL SELECT product_code INTO :sGrpProductCode
               FROM sGrpSmCode
              WHERE sm_code = :sSmCode;
    if(SQLCODE != 0){
        sprintf(return_message,"��ѯsGrpSmCode���ſͻ�ϵͳ��Ʒ�������[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190506",return_message);
        return 190506;
    }
    Trim(sGrpProductCode);

    ȡ������������¼��
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","ȡ��wGrpOrderOpr����Ϣ");
    #endif
    EXEC SQL delete from wGrpOrderOpr
              where CUST_ID = :sCustId  and LOGIN_ACCEPT = :sOldSysAccept;
    if(SQLCODE != 0){
          sprintf(return_message, "ɾ��wGrpOrderOpr�����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190507",return_message);
          return 190507;
    }
    
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","ȡ������");
#endif
    EXEC SQL INSERT INTO dGrpOrderGoodsHis
                  ( ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
                    UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
                    DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
                    CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
                    CURR_TACHE,
                    UPDATE_LOGIN,   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
                    UPDATE_CODE,    UPDATE_TYPE
                  )
            select 
                    ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
                    UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
                    DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
                    CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
                    CURR_TACHE,  
                    :sLoginNo, to_number(:sSysAccept), to_number(:sTotalDate), to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
                    :sOpCode, 'D'
               from dGrpOrderGoods 
              where product_id in
                    (SELECT product_id FROM dGrpProduct 
                      WHERE cust_id = to_number(:sCustId) 
                        AND product_code = :sGrpProductCode
                    );
    if(SQLCODE != 0){
          pubLog_Debug(_FFL_,"","","CustId=[%s],GrpProductCode=[%s]",sCustId,sGrpProductCode);
          sprintf(return_message, "����dGrpOrderGoodsHis�����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190508",return_message);
          return 190508;
    }
    
    EXEC SQL DELETE FROM dGrpOrderGoods WHERE product_id in 
             (SELECT product_id FROM dGrpProduct WHERE cust_id = to_number(:sCustId) AND product_code = :sGrpProductCode);
    if (SQLCODE != 0 && SQLCODE != 1403)
    {
        sprintf(return_message, "ȡ��������dGrpOrderGoods����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190509",return_message);
        return 190509;
    }

#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","������Ϣ��dGrpProductHis��");
        pubLog_Debug(_FFL_,"","","INSERT INTO dGrpProductHis ");
        pubLog_Debug(_FFL_,"",""," (CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,  ");
        pubLog_Debug(_FFL_,"","","  PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,  ");
        pubLog_Debug(_FFL_,"","","  BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,  ");
        pubLog_Debug(_FFL_,"","","  PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,  ");
        pubLog_Debug(_FFL_,"","","  BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,  ");
        pubLog_Debug(_FFL_,"","","  NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,  ");
        pubLog_Debug(_FFL_,"","","  CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,  ");
        pubLog_Debug(_FFL_,"","","  APN1,           APN2,              APN3,          INPUT_DATE,  ");
        pubLog_Debug(_FFL_,"","","  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME, ");
        pubLog_Debug(_FFL_,"","","  UPDATE_CODE,  UPDATE_TYPE) ");
        pubLog_Debug(_FFL_,"",""," SELECT ");
        pubLog_Debug(_FFL_,"","","  CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,  ");
        pubLog_Debug(_FFL_,"","","  PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,  ");
        pubLog_Debug(_FFL_,"","","  BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,  ");
        pubLog_Debug(_FFL_,"","","  PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,  ");
        pubLog_Debug(_FFL_,"","","  BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,  ");
        pubLog_Debug(_FFL_,"","","  NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,  ");
        pubLog_Debug(_FFL_,"","","  CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,  ");
        pubLog_Debug(_FFL_,"","","  APN1,           APN2,              APN3,          INPUT_DATE,  ");
        pubLog_Debug(_FFL_,"","","  '%s', %s, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), ", sLoginNo, sSysAccept, sTotalDate, sOpTime);
        pubLog_Debug(_FFL_,"","","  '%s',     'D' ", sOpCode);
        pubLog_Debug(_FFL_,"",""," FROM dGrpProduct ");
        pubLog_Debug(_FFL_,"","","WHERE cust_id = to_number('%s') ", sCustId);
        pubLog_Debug(_FFL_,"","","  AND product_code = '%s'; ", sGrpProductCode);
#endif
	 ��ͻ���ṹ���� edit by magang at 20090626---begin
    EXEC SQL INSERT INTO dGrpProductHis
              (CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID, 
               PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME, 
               BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT, 
               PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE, 
               BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC, 
               NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE, 
               CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE, 
               APN1,           APN2,              APN3,          INPUT_DATE, 
               UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
               UPDATE_CODE,  UPDATE_TYPE)
              SELECT
               CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID, 
               PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME, 
               BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT, 
               PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE, 
               BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC, 
               NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE, 
               CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE, 
               APN1,           APN2,              APN3,          INPUT_DATE, 
               :sLoginNo, to_number(:sSysAccept), to_number(:sTotalDate), to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
               :sOpCode,     'D'
              FROM dGrpProduct
             WHERE cust_id = to_number(:sCustId)
               AND product_code = :sGrpProductCode;
	 ��ͻ���ṹ���� edit by magang at 20090626---end               	
   EXEC SQL INSERT INTO dGrpProductHis
              (CUST_ID,        UNIT_ID,           PRODUCT_ID, 
               PRODUCT_CODE,   CONTRACT_ID,       ORDER_TIME, 
               BEGIN_TIME,     END_TIME,          PRODUCT_STATUS,  
               OPERATION_CODE,     PRODUCT_DESC, 
               NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,      
               TREATY_FILE_NAME,  FILE_FORMAT,  
               INPUT_DATE, 
               UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
               UPDATE_CODE,  UPDATE_TYPE, SAVE_FILE,CREATE_LOGIN,CREATE_DATE)
              SELECT
               CUST_ID,        UNIT_ID,           PRODUCT_ID, 
               PRODUCT_CODE,   CONTRACT_ID,       ORDER_TIME, 
               BEGIN_TIME,     END_TIME,          PRODUCT_STATUS,  
               OPERATION_CODE,     PRODUCT_DESC, 
               NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,      
               TREATY_FILE_NAME,  FILE_FORMAT,   
               INPUT_DATE, 
               :sLoginNo, to_number(:sSysAccept), to_number(:sTotalDate), to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
               :sOpCode,     'D',	SAVE_FILE, CREATE_LOGIN, CREATE_DATE
              FROM dGrpProduct
             WHERE cust_id = to_number(:sCustId)
               AND product_code = :sGrpProductCode;            	
               	
    if ((SQLCODE != 0) || (SQLROWS == 0))
    {
        sprintf(return_message, "����dGrpProductHis����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190510",return_message);
        return 190510;
    }
    EXEC SQL DELETE FROM dGrpProduct
              WHERE cust_id = to_number(:sCustId)
                AND product_code = :sGrpProductCode;
    if (SQLCODE != 0)
    {
        sprintf(return_message, "ɾ��dGrpProduct���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190511",return_message);
        return 190511;
    }
    
    
    
    if(strncmp(sSmCode,"ap",2) == 0)
    {
        printf("sm_code = ap, delete dGrpApnMsg ");
        init(sTmpApnNo);
        EXEC SQL select trim(apn_no) into :sTmpApnNo from dGrpApnMsg where id_no = to_number(:sGrpIdNo);
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
            sprintf(return_message, "��ѯdGrpApnMsg���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
            printf("%s", return_message);
            return 190512;
        }
        printf("huaxm:apn_code=[%s]",sTmpApnNo);
        Trim(sTmpApnNo);
        EXEC SQL INSERT INTO dGrpApnMsgHis
                      (
                        ID_NO, BELONG_CODE, APN_ID, APN_NO, TERMINAL_NUM, BILL_TYPE, 
                        PRODUCT_CODE, PRODUCT_ACCEPT, BAK_FIELD1, GROUP_ID, 
                        UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, 
                        UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE
                      )
                 select ID_NO, BELONG_CODE, APN_ID, APN_NO, TERMINAL_NUM, BILL_TYPE, 
                        PRODUCT_CODE, PRODUCT_ACCEPT, BAK_FIELD1, GROUP_ID,
                        :sLoginNo, to_number(:sSysAccept), to_number(:sTotalDate),
                        to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'), :sOpCode, 'D'
                   from dGrpApnMsg
                  where id_no = to_number(:sGrpIdNo);
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
            sprintf(return_message, "����dGrpApnMsgHis���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
            printf("%s", return_message);
            return 190513;
        }
        
        EXEC SQL DELETE from dGrpApnMsg where id_no = to_number(:sGrpIdNo);
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
            sprintf(return_message, "ɾ��dGrpApnMsg���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
            printf("%s", return_message);
            return 190514;
        }
        
        EXEC SQL INSERT INTO sApnCodeHis
                      (
                        REGION_CODE, APN_CODE, APN_NAME, COMMAND_CODE, APN_ID, VALID_FLAG, 
                        UPDATE_LOGIN, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE
                      )
                 select REGION_CODE, APN_CODE, APN_NAME, COMMAND_CODE, APN_ID, VALID_FLAG,
                        :sLoginNo, to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'), :sOpCode, 'D'
                   from sApnCode
                  where apn_code = :sTmpApnNo;
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
            sprintf(return_message, "����sApnCodeHis���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
            printf("%s", return_message);
            return 190515;
        }
        
        EXEC SQL UPDATE sApnCode SET valid_flag = '0' where apn_code = :sTmpApnNo;
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
            sprintf(return_message, "�޸�sApnCode���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
            printf("%s", return_message);
            return 190516;
        }
    }
    
    EXEC SQL update dGrpUserNoRes set resource_code ='0'
              where region_code = substr(:sBelongCode,1,2)
                and user_no = :sGrpUserNo;
    if ((SQLCODE != 0) && (SQLCODE != 1403))
    {
        sprintf(return_message, "�޸�dGrpUserNoRes���¼ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
        printf("%s", return_message);
        return 190517;
    }
    
    
    ɾ������
    if ( (retCode=fPubCancelAllBill(sGrpIdNo, sGrpUserNo, "", sOpCode, sLoginNo, sSysAccept, "dGrpSrvMsg", sOpTime, return_message)) != 0)
    {
        pubLog_Debug(_FFL_,"","","����fPubCancelAllBillʧ�ܣ�retCode[%d]", retCode);
        return retCode;
    }

    
    ɾ��dAccountIdInfo��¼
    EXEC SQL INSERT INTO dAccountIdInfoHis
                  (
                    MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE,
                    UPDATE_LOGIN,UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT,UPDATE_CODE,UPDATE_TYPE
                  )
             SELECT
                    MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE,
                    :sLoginNo, to_number(:sTotalDate), to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:sSysAccept),:sOpCode,'D'
               FROM dAccountIdInfo
              WHERE msisdn = :sGrpUserNo;
    if (SQLCODE != 0)
    {
        sprintf(return_message, "�����dAccountIdInfoHis����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190518",return_message);
        return 190518;
    }

    EXEC SQL  INSERT INTO wAccountIdInfoChg
                   (
                     MSISDN, SERVICE_TYPE, SERVICE_NO, UPDATE_TYPE, RUN_CODE, OP_TIME
                   )
              SELECT MSISDN, SERVICE_TYPE, SERVICE_NO, 'D', RUN_CODE, to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
                FROM dAccountIdInfo
               WHERE msisdn = :sGrpUserNo;
    if (SQLCODE != 0)
    {
        sprintf(return_message, "�����wAccountIdInfoChg����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190519",return_message);
        return 190519;
    }

    EXEC SQL  DELETE FROM dAccountIdInfo
         WHERE msisdn = :sGrpUserNo;
    if (SQLCODE != 0)
    {
        sprintf(return_message, "ɾ��dAccountIdInfo�����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190520",return_message);
        return 190520;
    }

    ����Ƿ��м��ſͻ��������û����ø��ʻ�
    iAccountIdNbr = 0;
    EXEC SQL SELECT count(*) INTO :iAccountIdNbr
               FROM dGrpUserMsg
              WHERE cust_id = :sCustId
                AND account_id = :sContractNo;
    if (SQLCODE != 0)
    {
        sprintf(return_message, "��ѯdGrpUserMsg���ʻ�[%s]ʧ��![%d]", sContractNo, SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190521",return_message);
        return 190521;
    }

    if (iAccountIdNbr != 1) 
    {
#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","�����û����ø��ʻ��������û�ʹ�ã���ɾ���˻���Ϣ��");
#endif
    }
    else
    {
        û�����������û����ø��ʻ�������ɾ�����ʻ�
        ��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009
        EXEC SQL INSERT INTO dConMsgHis
                      (
                        contract_no,    con_cust_id,  contract_passwd,
                        bank_cust,      oddment,      belong_code,
                        prepay_fee,     prepay_time,  Status,
                        status_time,    post_flag ,   deposit ,
                        min_ym,         owe_fee,      account_mark,
                        account_limit,  pay_code,     bank_code,
                        post_bank_code, Account_No,   account_type,group_id,
                        update_code,    update_date,  update_time,
                        update_login,   update_accept,update_type
                      )
                 SELECT
                        contract_no,    con_cust_id,  contract_passwd,
                        bank_cust,      oddment,      belong_code,
                        prepay_fee,     prepay_time,  Status,
                        status_time,    post_flag ,   deposit ,
                        min_ym,         owe_fee,      account_mark,
                        account_limit,  pay_code,     bank_code,
                        post_bank_code, Account_No,   account_type,group_id,
                        :sOpCode,      to_number(:sTotalDate),
                        TO_DATE(:sOpTime,'YYYYMMDD HH24:MI:SS'),
                        :sLoginNo,    to_number(:sSysAccept),'D'
                   FROM dConMsg
                  WHERE contract_no = to_number(:sContractNo);
        if (SQLCODE != 0)
        {
            sprintf(return_message, "����dConMsgHis ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190522",return_message);
            return 190522;
        }
        
        EXEC SQL DELETE FROM dConMsg WHERE contract_no = to_number(:sContractNo);
        if (SQLCODE != 0)
        {
            sprintf(return_message, "ɾ��dConMsg ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190523",return_message);
            return 190523;
        }
        
        EXEC SQL DELETE FROM dConMsgPre WHERE contract_no = to_number(:sContractNo);
        if (SQLCODE != 0) {
            sprintf(return_message, "ɾ�������û�dConMsgPre����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190524",return_message);
            return 190524;
        }
    
        EXEC SQL INSERT INTO dCustConMsgHis
                      (
                        cust_id, contract_no, begin_time, end_time,
                        update_login, update_accept, update_date,
                        update_time, update_code, update_type
                      )
                 SELECT
                        cust_id, contract_no, begin_time, end_time,
                        :sLoginNo, to_number(:sSysAccept), to_number(:sTotalDate), 
                        to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'), :sOpCode, 'D'
                   FROM dCustConMsg
                  WHERE cust_id = to_number(:sCustId)
                    AND contract_no = to_number(:sContractNo);
        if (SQLCODE != 0)
        {
            sprintf(return_message, "����dCustConMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190525",return_message);
            return 190525;
        }
    
        EXEC SQL delete FROM dCustConMsg WHERE cust_id = to_number(:sCustId) AND contract_no = to_number(:sContractNo);
        if (SQLCODE != 0)
        {
            sprintf(return_message, "ɾ��dCustConMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190526",return_message);
            return 190526;
        }
    } �ʻ��������


    EXEC SQL INSERT INTO dCustUserHis
                  (
                    CUST_ID, ID_NO, REP_FLAG, CUST_FLAG,
                    UPDATE_LOGIN, UPDATE_TIME, UPDATE_ACCEPT, OP_CODE, UPDATE_TYPE
                  )
             SELECT
                    CUST_ID, ID_NO, REP_FLAG, CUST_FLAG,
                    :sLoginNo, to_date(:sOpTime,'YYYYMMDD HH24:MI:SS'), 
                    to_number(:sSysAccept), :sOpCode, 'D'
               FROM dCustUserMsg
              WHERE id_no = to_number(:sGrpIdNo);
    if(SQLCODE != 0)
    {
        sprintf(return_message, "����dCustUserHis����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190527",return_message);
        return 190527;
    }

    EXEC SQL DELETE FROM dCustUserMsg WHERE id_no=to_number(:sGrpIdNo);
    if(SQLCODE != 0 && SQLCODE != 1403)
    {
        sprintf(return_message, "ɾ��dCustUserMsg����![%d][%s]", SQLCODE, SQLERRMSG);
        pubLog_Debug(_FFL_,"","","%s��sGrpIdNo��[%s]", return_message,sGrpIdNo);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190528",return_message);        
        return 190528;
    }

#if 0 Disabled by huaxm at 20061117 for deleting dCustMsg.
    iBillFlag = 0;
    EXEC SQL  SElECT count(*) INTO :iBillFlag
                FROM sProductAttr
               WHERE product_attr = (select product_attr from sProductCode where product_code = :sProductCode)
                 AND isbill = 'Y';
                 
    ������ڼƷ��¼�����ɾ��dCustMsg�ȱ�
    if (iBillFlag <= 0)
    {
        pubLog_Debug(_FFL_,"","","�����ڼƷ��¼�!");
    }
    else
    {
#endifEND --Disabled by huaxm at 20061117 for deleting dCustMsg.

#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","��¼��ʷdCustMsgHis��!");
#endif
     ��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009
        EXEC SQL INSERT INTO dCustMsgHis
                      (
                        ID_NO, CUST_ID, CONTRACT_NO, IDS,PHONE_NO, SM_CODE, SERVICE_TYPE,
                        ATTR_CODE, USER_PASSWD, BELONG_CODE, LIMIT_OWE, CREDIT_CODE, CREDIT_VALUE,
                        RUN_CODE, OPEN_TIME, RUN_TIME, GROUP_ID,BILL_DATE, BILL_TYPE, 
                        ENCRYPT_PREPAY, CMD_RIGHT, LAST_BILL_TYPE, BAK_FIELD,
                        UPDATE_ACCEPT, UPDATE_TIME, UPDATE_DATE, 
                        UPDATE_LOGIN, UPDATE_TYPE, UPDATE_CODE,GROUP_NO
                      )
                 SELECT
                        ID_NO, CUST_ID, CONTRACT_NO, IDS, PHONE_NO, SM_CODE, SERVICE_TYPE,
                        ATTR_CODE, USER_PASSWD, BELONG_CODE, LIMIT_OWE, CREDIT_CODE, CREDIT_VALUE,
                        RUN_CODE, OPEN_TIME, RUN_TIME, GROUP_ID, BILL_DATE, BILL_TYPE, 
                        ENCRYPT_PREPAY, CMD_RIGHT, LAST_BILL_TYPE, BAK_FIELD,
                        to_number(:sSysAccept), to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
                        to_number(:sTotalDate), :sLoginNo, 'D', :sOpCode,GROUP_NO
                   FROM dCustMsg
                  WHERE id_no = to_number(:sGrpIdNo);
        if((SQLCODE != 0)&&(SQLCODE != 1403))
        {
            sprintf(return_message, "����dCustMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190529",return_message); 
            return 190529;
        }

#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","��¼��ʷwUserMsgChg��!");
#endif
   ��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009
        EXEC SQL INSERT INTO wUserMsgChg
                      (
                        OP_NO, OP_TYPE, ID_NO, CUST_ID, PHONE_NO, SM_CODE,
                        ATTR_CODE, BELONG_CODE, RUN_CODE, RUN_TIME, OP_TIME, GROUP_ID
                      )
                 SELECT
                        sMaxBillChg.NEXTVAL, '3', ID_NO, CUST_ID, PHONE_NO, SM_CODE,
                        ATTR_CODE, BELONG_CODE, RUN_CODE, to_char(RUN_TIME,'YYYYMMDDHHMMSS'), 
                        to_char(to_date(:sOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHHMMSS'), GROUP_NO
                   FROM dCustMsg 
                  WHERE id_no = to_number(:sGrpIdNo);
        if((SQLCODE != 0)&&(SQLCODE != 1403))
        {
            sprintf(return_message, "����wUserMsgChg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190530",return_message); 

            return 190530;
        }
        
#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","ɾ��dCustMsg��!");
#endif
        EXEC SQL DELETE FROM dCustMsg WHERE id_no = :sGrpIdNo;
        if((SQLCODE != 0)&&(SQLCODE != 1403))
        {
            sprintf(return_message, "ɾ��dCustMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","190531",return_message); 

            return 190531;
        }
#if 0Disabled by huaxm at 20061117 for deleting dCustMsg.
    }
#endifEND-- Disabled by huaxm at 20061117 for deleting dCustMsg.

#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","ɾ��dGrpUserMsg����Ϣ");
    pubLog_Debug(_FFL_,"","","INSERT INTO dGrpUserMsgHis ");
    pubLog_Debug(_FFL_,"","","   (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
    pubLog_Debug(_FFL_,"","","    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
    pubLog_Debug(_FFL_,"","","    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
    pubLog_Debug(_FFL_,"","","    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
    pubLog_Debug(_FFL_,"","","    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
    pubLog_Debug(_FFL_,"","","    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
    pubLog_Debug(_FFL_,"","","    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN, ");
    pubLog_Debug(_FFL_,"","","    UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE, ");
    pubLog_Debug(_FFL_,"","","    UPDATE_TYPE) ");
    pubLog_Debug(_FFL_,"",""," SELECT ");
    pubLog_Debug(_FFL_,"","","    CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
    pubLog_Debug(_FFL_,"","","    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
    pubLog_Debug(_FFL_,"","","    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
    pubLog_Debug(_FFL_,"","","    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
    pubLog_Debug(_FFL_,"","","    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
    pubLog_Debug(_FFL_,"","","    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
    pubLog_Debug(_FFL_,"","","    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     '%s', ", sLoginNo);
    pubLog_Debug(_FFL_,"","","    %s, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), '%s', ", sSysAccept, sTotalDate, sOpTime, sOpCode);
    pubLog_Debug(_FFL_,"","","    'D' ");
    pubLog_Debug(_FFL_,"","","   FROM dGrpUserMsg ");
    pubLog_Debug(_FFL_,"","","  WHERE ID_NO = to_number('%s') ", sGrpIdNo);
    pubLog_Debug(_FFL_,"","","    AND bill_date > last_day(sysdate) + 1; ");
#endif
    EXEC SQL INSERT INTO dGrpUserMsgHis
          (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
           IDS,         USER_NAME,     SM_CODE,       group_id,
           PRODUCT_TYPE,  PRODUCT_CODE,
           USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
           REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
           LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
           OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
           LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
           UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
           UPDATE_TYPE)
        SELECT
           CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
           IDS,         USER_NAME,     SM_CODE,       group_id,
           PRODUCT_TYPE,  PRODUCT_CODE,
           USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
           REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
           LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
           OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
           LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     :sLoginNo,
           to_number(:sSysAccept), to_number(:sTotalDate), 
           to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'), :sOpCode, 'D'
          FROM dGrpUserMsg
         WHERE id_no = to_number(:sGrpIdNo);
    if(SQLCODE != 0){
        sprintf(return_message,"дdGrpUserMsgHis��ʧ��[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190532",return_message); 
        return 190532;
    }
    
    EXEC SQL DELETE FROM dGrpUserMsg WHERE id_no = to_number(:sGrpIdNo);
    if(SQLCODE != 0){
        sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190533",return_message); 
        return 190533;
    }

    EXEC SQL DELETE FROM dGrpUserMsgAdd
              WHERE id_no = to_number(:sGrpIdNo);
    if(SQLCODE != 0 && SQLCODE != 1403){
        sprintf(return_message,"ɾ��dGrpUserMsgAdd���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190534",return_message); 
        return 190534;
    }

#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","insert into dConUserMsgHis");
#endif
    EXEC SQL INSERT INTO dConUserMsgHis
                (   Id_No,         Contract_No,  Serial_No,
                    Bill_Order,    Pay_Order,    Begin_YMD,
                    Begin_TM,      End_YMD,      End_TM,
                    Limit_Pay,     Rate_Flag,    Stop_Flag,
                    Update_Accept, Update_Login, Update_Date,
                    Update_Time,   Update_Code,  Update_Type  )
             SELECT
                    Id_No,         Contract_No,  Serial_No,
                    Bill_Order,    Pay_Order,    Begin_YMD,
                    Begin_TM,      End_YMD,      End_TM,
                    Limit_Pay,     Rate_Flag,    Stop_Flag,
                    to_number(:sSysAccept), :sLoginNo, to_number(:sTotalDate),
                    to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'), :sOpCode, 'D'
               FROM dConUserMsg
              WHERE Id_No = to_number(:sGrpIdNo)
                AND Contract_No = to_number(:sContractNo);
    if ((SQLCODE != 0)&&(SQLCODE != 1403)) {
        sprintf(return_message, "����dConUserMsgHis����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190535",return_message); 
        return 190535;
    }
     #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","������Ϣ��dGrpProductHis��");
        pubLog_Debug(_FFL_,"","","INSERT INTO dConUserMsgHis ");
        pubLog_Debug(_FFL_,"",""," (Id_No,         Contract_No,  Serial_No,  ");
        pubLog_Debug(_FFL_,"","","  Bill_Order,    Pay_Order,    Begin_YMD,  ");
        pubLog_Debug(_FFL_,"","","  Begin_TM,      End_YMD,      End_TM, ");
        pubLog_Debug(_FFL_,"","","  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
        pubLog_Debug(_FFL_,"","","  Update_Time,   Update_Code,  Update_Type  ) ");
        pubLog_Debug(_FFL_,"","","  SELECT  ");
        pubLog_Debug(_FFL_,"","","  CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,  ");
        pubLog_Debug(_FFL_,"","","  APN1,           APN2,              APN3,          INPUT_DATE,  ");
        pubLog_Debug(_FFL_,"","","  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME, ");
        pubLog_Debug(_FFL_,"","","  UPDATE_CODE,  UPDATE_TYPE) ");
        pubLog_Debug(_FFL_,"",""," SELECT ");
        pubLog_Debug(_FFL_,"",""," Id_No,         Contract_No,  Serial_No, ");
        pubLog_Debug(_FFL_,"","","  Bill_Order,    Pay_Order,    Begin_YMD, ");
        pubLog_Debug(_FFL_,"",""," Begin_TM,      End_YMD,      End_TM, ");
        pubLog_Debug(_FFL_,"","","  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
        pubLog_Debug(_FFL_,"","","  to_number('%s'), '%s', to_number('%s'), ", sSysAccept, sLoginNo, sTotalDate);
        pubLog_Debug(_FFL_,"","","  to_date(),'%s','D' ", sOpCode);
        pubLog_Debug(_FFL_,"",""," FROM dConUserMsg ");
        pubLog_Debug(_FFL_,"","","WHERE id_no = to_number('%s') ", sGrpIdNo);
        pubLog_Debug(_FFL_,"","","  AND contract_no = to_number('%s'); ", sContractNo);
    #endif

    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","delete dConUserMsg");
    #endif
    EXEC SQL delete from dConUserMsg WHERE Id_No = to_number(:sGrpIdNo) AND Contract_No = to_number(:sContractNo);
    if (SQLCODE != 0 && SQLCODE != 1403) {
        sprintf(return_message, "ɾ��dConUserMsg���¼����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190536",return_message); 
        return 190536;
    }

    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","delete dBillCustDetail");
    #endif
    init(sBuffer);
    liuhao edit 20090429 �󶨱��� begin
    sprintf(sBuffer,
            " INSERT INTO dBillCustDetHis%c "
            "      ( "
            "        ID_NO, DETAIL_TYPE, DETAIL_CODE, BEGIN_TIME, END_TIME,"
            "        FAV_ORDER, REGION_CODE, MODE_CODE, MODE_FLAG, MODE_TIME, "
            "        MODE_STATUS, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, "
            "        LOGIN_ACCEPT, UPDATE_ACCEPT, UPDATE_LOGIN, UPDATE_DATE,"
            "        UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE"
            "      ) "
            " SELECT ID_NO, DETAIL_TYPE, DETAIL_CODE, BEGIN_TIME, END_TIME,  "
            "        FAV_ORDER, REGION_CODE, MODE_CODE, MODE_FLAG, MODE_TIME, "
            "        MODE_STATUS, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, "
            "        LOGIN_ACCEPT, to_number(:v1), :v2, to_number(:v3),"
            "        to_date(:v4, 'YYYYMMDD HH24:MI:SS'), :v5, 'D'"
            "   FROM dBillCustDetail%c where id_no = to_number('%s')",
            sGrpIdNo[strlen(sGrpIdNo)-1], sGrpIdNo[strlen(sGrpIdNo)-1], sGrpIdNo);
    sprintf(sBuffer,
            " INSERT INTO dBillCustDetHis%c "
            "      ( "
            "        ID_NO, DETAIL_TYPE, DETAIL_CODE, BEGIN_TIME, END_TIME,"
            "        FAV_ORDER, REGION_CODE, MODE_CODE, MODE_FLAG, MODE_TIME, "
            "        MODE_STATUS, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, "
            "        LOGIN_ACCEPT, UPDATE_ACCEPT, UPDATE_LOGIN, UPDATE_DATE,"
            "        UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE"
            "      ) "
            " SELECT ID_NO, DETAIL_TYPE, DETAIL_CODE, BEGIN_TIME, END_TIME,  "
            "        FAV_ORDER, REGION_CODE, MODE_CODE, MODE_FLAG, MODE_TIME, "
            "        MODE_STATUS, OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, "
            "        LOGIN_ACCEPT, to_number(:v1), :v2, to_number(:v3),"
            "        to_date(:v4, 'YYYYMMDD HH24:MI:SS'), :v5, 'D'"
            "   FROM dBillCustDetail%c where id_no = to_number(:v1)",
            sGrpIdNo[strlen(sGrpIdNo)-1], sGrpIdNo[strlen(sGrpIdNo)-1]);
            
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","insert: dBillCust%cDetHis = [%s]",sGrpIdNo[strlen(sGrpIdNo)-1]);
#endif 
    EXEC SQL PREPARE dBillCustDetHisXPre From :sBuffer; 
    EXEC SQL EXECUTE dBillCustDetHisXPre USING :sSysAccept,:sLoginNo,:sTotalDate,:sOpTime,:sOpCode,:sGrpIdNo;
    liuhao end edit
    if (SQLCODE != 0 && SQLCODE != 1403) 
    {
        sprintf(return_message, "����dBillCustDetHisX����![%d]",sGrpIdNo[strlen(sGrpIdNo)-1],SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190537",return_message); 
        return 190537;
    }

    init(sBuffer);
     liuhao edit 20090429 �󶨱��� begin
     
    sprintf(sBuffer," Delete from dBillCustDetail%c where id_no = to_number('%s')",
            sGrpIdNo[strlen(sGrpIdNo)-1], sGrpIdNo);
                
    sprintf(sBuffer," Delete from dBillCustDetail%c where id_no = to_number(:v1)",
            sGrpIdNo[strlen(sGrpIdNo)-1]);
    
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","delete: dBillCustDetail%c = [%s]",sGrpIdNo[strlen(sGrpIdNo)-1]);
#endif 
    EXEC SQL PREPARE dBillCustDetailXPre From :sBuffer; 
    EXEC SQL EXECUTE dBillCustDetailXPre using :sGrpIdNo;
    liuhao edit end
    if (SQLCODE != 0 && SQLCODE != 1403) 
    {
        sprintf(return_message, "����dBillCustDetail%c����![%d]",sGrpIdNo[strlen(sGrpIdNo)-1],SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","190538",return_message); 
        return 190538;
    }
    
    
            
    return 0;
}

*/

/************************************************************************
��������:fCreateGrpUserMebBaseFav
����ʱ��:2006/11/21
������Ա:Huaxm
��������:ɾ�����ų�Ա�û��Ʒѱ�dBaseFav��¼
������
     typedef struct tCreateGrpUserMebBaseFavTag
     {
         char sGrpId_no[14+1];      / * �����û�ID_NO* /
         char sMember_no[14+1];     / * ��Ա�û�PHONE_NO, ALL��ʾȫ����Ա* /
         char sLogin_no[6+1];       / * ����Ա������* /
         char sOp_code[4+1];        / * ��������* /
         char sOp_time[20+1];       / * ϵͳʱ��* /
         char sSys_accept[14+1];    / * ������ˮ��new* /
         char sOp_note[60+1];       / * ������־* /
         char sReturnMsg[60+1];     / * ���ش�����Ϣ* /
     }tCreateGrpUserMebBaseFav;
     
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fCreateGrpUserMebBaseFav(tCreateGrpUserMebBaseFav *funcIn)
{
    int  retCode=0,ret = 0;
    EXEC SQL BEGIN DECLARE SECTION;
    char sGrpIdNo[14+1];              /* �����û�ID_NO*/
    char sMemberNo[14+1];             /* ��Ա�û�PHONE_NO, ALL��ʾȫ����Ա*/
    char sLoginNo[6+1];               /* ����Ա������*/
    char sOpCode[4+1];                /* ��������*/
    char sOpTime[20+1];               /* ϵͳʱ��*/
    char sSysAccept[14+1];            /* ��ˮ��*/
    char sOpNote[60+1];               /* ��������*/
/*
    char *return_message = &funcIn->sReturnMsg;             
*/
    char *return_message = funcIn->sReturnMsg;             /* ������Ϣ*/
    char sTotalDate[8+1];             /* */
    
    char sGrpSendCode[10+1];
    char sBeginMemberNo[14+1];
    char sEndMemberNo[14+1];
    char sServiceCode[4+1];
    char sPriceCode[4+1];
    char sFavOrder[1+1];
    char sBeginTime[17+1];
    char sEndTime[17+1];
    char sProductCode[8+1];
    char sSmCode[2+1];
    char sRegionCode[2+1];
    char sFavBrand[2+1];
    char sPhoneNo[22+1];
    char sIdNo[22+1];
    char vGroupId[10+1];

    EXEC SQL END DECLARE SECTION;
    char sBuffer[2048];
    int  iCount = 0;


    init(sGrpIdNo);
    init(sMemberNo);
    init(sLoginNo);
    init(sOpCode);
    init(sOpTime);
    init(sSysAccept);
    init(sOpNote);
    init(sTotalDate);
    init(sGrpSendCode);
    init(sBeginMemberNo);
    init(sEndMemberNo);
    init(sBuffer);
    init(sServiceCode);
    init(sPriceCode);
    init(sFavOrder);
    init(sBeginTime);
    init(sEndTime);
    init(sProductCode);
    init(sSmCode);
    init(sRegionCode);
    init(sFavBrand);
    init(sPhoneNo);
    init(sIdNo);
    init(vGroupId);
    
    strcpy(sGrpIdNo,      funcIn->sGrpId_no);
    strcpy(sMemberNo,     funcIn->sMember_no);
    strcpy(sLoginNo,      funcIn->sLogin_no);
    strcpy(sOpCode,       funcIn->sOp_code);
    strcpy(sOpTime,       funcIn->sOp_time);
    strcpy(sSysAccept,    funcIn->sSys_accept);
    strcpy(sOpNote,       funcIn->sOp_note);
    strncpy(sTotalDate,   sOpTime, 8);
    
    Trim(sGrpIdNo);
    Trim(sMemberNo);
    Trim(sLoginNo);
    Trim(sOpCode);
    Trim(sOpTime);
    Trim(sSysAccept);
    Trim(sOpNote);
  
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","begin function fCreateGrpUserMebBaseFav();-----------------");
    pubLog_Debug(_FFL_,"","","sGrpIdNo   =[%s]",    sGrpIdNo);
    pubLog_Debug(_FFL_,"","","sMemberNo  =[%s]",    sMemberNo);
    pubLog_Debug(_FFL_,"","","sLoginNo   =[%s]",    sLoginNo);
    pubLog_Debug(_FFL_,"","","sOpCode    =[%s]",    sOpCode);
    pubLog_Debug(_FFL_,"","","sOpTime    =[%s]",    sOpTime);
    pubLog_Debug(_FFL_,"","","sSysAccept =[%s]",    sSysAccept);
    pubLog_Debug(_FFL_,"","","sOpNote    =[%s]",    sOpNote);
    pubLog_Debug(_FFL_,"","","sTotalDate =[%s]",    sTotalDate);
#endif
    
    /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/

       ret = sGetLoginOrgid(sLoginNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_Debug(_FFL_,"","","��ȡ�û�org_idʧ��!");
         	  return 200919; 
         	}
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
    
    if((strncmp(sMemberNo, "ALL", 3) == 0) || strlen(sMemberNo) == 0)
    {
        strcpy(sMemberNo, "%%");
        strcpy(sBeginMemberNo, "00000000000000");
        strcpy(sEndMemberNo,   "99999999999999");
#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","Modify: sBeginMemberNo  =[%s]",    sBeginMemberNo);
        pubLog_Debug(_FFL_,"","","Modify: sEndMemberNo    =[%s]",    sEndMemberNo);
#endif
    } 
        
    init(sGrpSendCode);
    createGrpSendCode(sGrpIdNo, sGrpSendCode);
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","sGrpSendCode = [%s]", sGrpSendCode);
#endif
    
    
    EXEC SQL select sm_code, region_code into :sSmCode, :sRegionCode
               from dGrpUserMsg where id_no = :sGrpIdNo
               	and run_code < 'a';
    if(SQLCODE != 0)
    {
        sprintf(return_message,"��ѯdGrpUserMsg��ʧ��! [%d]", SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","351971",return_message); 
        return 351971;
    }
    
    init(sFavBrand);
    EXEC SQL select fav_brand into :sFavBrand 
               from sSmCode where region_code = substr(:sRegionCode,1,2)
                and sm_code = :sSmCode;
    if(SQLCODE != 0)
    {
        sprintf(return_message,"��ѯsSmCode��ʧ��! [%d]", SQLCODE);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","351972",return_message); 
        return 351972;
    }
    
    init(sBuffer);
    /*liuhao begin �󶨱��� 20090429*/
    /*sprintf(sBuffer, 
        " select service_code, price_code, fav_order, begin_time, end_time, product_code "
        "   from dGrpSrvMsg%c "
        "  where id_no = %s and service_type = '0' "
        "    and to_date(begin_time,'yyyymmdd hh24:mi:ss') <= sysdate "
        "    and to_date(end_time,'yyyymmdd hh24:mi:ss')  >= sysdate ", sGrpIdNo[strlen(sGrpIdNo)-1], sGrpIdNo);
	*/
 	sprintf(sBuffer, 
        " select service_code, price_code, fav_order, begin_time, end_time, product_code "
        "   from dGrpSrvMsg%c "
        "  where id_no = :v1 and service_type = '0' "
        "    and to_date(begin_time,'yyyymmdd hh24:mi:ss') <= sysdate "
        "    and to_date(end_time,'yyyymmdd hh24:mi:ss')  >= sysdate ", sGrpIdNo[strlen(sGrpIdNo)-1]);        
    EXEC SQL prepare dGrpSrvMsgPre from :sBuffer;
    EXEC SQL declare dGrpSrvMsgCur CURSOR for dGrpSrvMsgPre;
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","sBuffer =[%s]", sBuffer);
#endif
    EXEC SQL OPEN dGrpSrvMsgCur using  :sGrpIdNo;
    /*liuhao edit end*/
    for(;;)
    {
        EXEC SQL fetch dGrpSrvMsgCur into :sServiceCode, :sPriceCode, :sFavOrder, :sBeginTime, :sEndTime, :sProductCode;
        PUBLOG_DBDEBUG("");
        if(SQLCODE == 1403) break;
        else if(SQLCODE != 0)
        {
            sprintf(return_message,"��ѯdGrpSrvMsg%c��ʧ��! [%d]",sGrpIdNo[strlen(sGrpIdNo)-1], SQLCODE);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","351973",return_message); 
            EXEC SQL CLOSE dGrpSrvMsgCur ;
            return 351973;
        }
        Trim(sEndTime);
        Trim(sProductCode);
        
        /* ����û��Ƿ���ڼƷѼ��Ż� */
        EXEC SQL DECLARE dGrpUserMebMsgCur CURSOR for
                  select member_no, member_id from dGrpUserMebMsg 
                   where id_no = :sGrpIdNo
                     and member_no >= :sBeginMemberNo
                     and member_no <= :sEndMemberNo
                     and to_date(:sOpTime,'yyyymmdd hh24:mi:ss') 
                         between begin_time and end_time;
        EXEC SQL OPEN dGrpUserMebMsgCur;
        for(;;)
        {
            EXEC SQL FETCH dGrpUserMebMsgCur INTO :sPhoneNo, sIdNo;
            if(SQLCODE == 1403) break;
            else if(SQLCODE != 0)
            {
                sprintf(return_message,"��ѯdGrpUserMebMsg��ʧ��! [%d]", SQLCODE);
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","351974",return_message); 
                EXEC SQL CLOSE dGrpUserMebMsgCur ;
                return 351974;
            }
            Trim(sPhoneNo);
            Trim(sIdNo);
            
#ifdef _DEBUG_
            pubLog_Debug(_FFL_,"","","phone_no = [%s] id_no = [%s] ", sPhoneNo, sIdNo);
#endif
            iCount = 0;
            EXEC SQL select count(*) into :iCount from dBaseFav
                      where msisdn = :sPhoneNo
                        and flag_code = :sGrpSendCode
                        and fav_type = :sServiceCode
                        and product_code = :sProductCode
                        and (sysdate between start_time and end_time);
            if(SQLCODE != 0)
            {
                pubLog_Debug(_FFL_,"","","--phone_no=[%s] sqlcode = [%d] [%s] [%s] [%s] ", sPhoneNo, SQLCODE, sServiceCode, sGrpSendCode, sProductCode);
                sprintf(return_message,"��ѯdBaseFav��ʧ��!");
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","351975",return_message); 
                return 351975;
            }
        
            if(iCount == 0)
            {
#ifdef _DEBUG_
                pubLog_Debug(_FFL_,"","","-I-insert dBaseFav:  iCount = [%d] "
                    "sPhoneNo[%s]  sFavBrand[%s]  sServiceCode[%s]  sGrpSendCode[%s]  sFavOrder[%s]  sEndTime[%s]  sIdNo[%s]  sProductCode[%s]",
                    iCount,sPhoneNo,sFavBrand,sServiceCode,sGrpSendCode,sFavOrder,sEndTime,sIdNo,sProductCode);
#endif
 /*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
                EXEC SQL INSERT INTO dBaseFav
                              ( 
                                msisdn, fav_brand, fav_type, flag_code, fav_order,
                                fav_period, free_value, start_time, end_time, 
                                id_no, group_id, product_code
                              )
                         values 
                              ( :sPhoneNo, :sFavBrand, :sServiceCode, :sGrpSendCode, :sFavOrder,
                                '0', 0, sysdate, to_date(:sEndTime,'YYYYMMDD HH24:MI:SS'),
                                :sIdNo, :vGroupId, :sProductCode
                              );
                if(SQLCODE != 0)
                {   
                    sprintf(return_message,"Error: Ins dBaseFav sqlcode =[%d]!",SQLCODE);
                    pubLog_Debug(_FFL_,"","","%s",return_message);
                    return 351976;
                }
                EXEC SQL INSERT INTO wBaseFavChg
                              (
                                msisdn, fav_brand, fav_type, flag_code, fav_order,
                                fav_period, free_value, start_time, end_time, 
                                id_no, group_id, product_code, flag, deal_time
                              )
                         values 
                              ( :sPhoneNo, :sFavBrand, :sServiceCode, :sGrpSendCode, :sFavOrder,
                                '0', 0, sysdate, to_date(:sEndTime,'YYYYMMDD HH24:MI:SS'),
                                :sIdNo, :vGroupId, :sProductCode, '1', to_date(:sOpTime,'yyyymmdd hh24:mi:ss')
                              );
                if(SQLCODE!=0)
                {
                    sprintf(return_message,"Error ins wBaseFavChg sqlcode =[%d]!", SQLCODE);
                    pubLog_Debug(_FFL_,"","","%s",return_message);
                    return 351977;
                }  
            }
            else
            {
#ifdef _DEBUG_
                pubLog_Debug(_FFL_,"","","-U-update dBaseFav:  iCount = [%d]  "
                    "sPhoneNo[%s]  sFavBrand[%s]  sServiceCode[%s]  sGrpSendCode[%s]  sFavOrder[%s]  sEndTime[%s]  sIdNo[%s]  sProductCode[%s]",
                    iCount,sPhoneNo,sFavBrand,sServiceCode,sGrpSendCode,sFavOrder,sEndTime,sIdNo,sProductCode);
#endif
                EXEC SQL update dBaseFav set end_time = to_date(:sEndTime,'YYYYMMDD HH24:MI:SS')
                          where msisdn = :sPhoneNo
                            and fav_type = :sServiceCode
                            and flag_code = :sGrpSendCode
                            and PRODUCT_code = :sProductCode
                            and (sysdate between start_time and end_time);
                if(SQLCODE != 0)
                {
                    sprintf(return_message,"Error Update dBaseFav sqlcode =[%d]!", SQLCODE);
                    pubLog_Debug(_FFL_,"","","%s",return_message);
                    return 351978;
                }
                
                EXEC SQL INSERT INTO wBaseFavChg
                              (
                                msisdn, fav_brand, fav_type, flag_code, fav_order,
                                fav_period, free_value, start_time, end_time, 
                                id_no, group_id, product_code, flag, deal_time
                              )
                         select 
                                MSISDN, FAV_BRAND, FAV_TYPE, FLAG_CODE, FAV_ORDER, 
                                FAV_PERIOD, FREE_VALUE, start_time, END_time, 
                                ID_NO, GROUP_ID, product_code, '2', to_date(:sOpTime,'yyyymmdd hh24:mi:ss')
                           from dBaseFav
                          where msisdn = :sPhoneNo
                            and fav_type = :sServiceCode
                            and flag_code = :sGrpSendCode
                            and PRODUCT_code = :sProductCode
                            and end_time = to_date(:sEndTime,'YYYYMMDD HH24:MI:SS');
                if(SQLCODE != 0)
                {
                    sprintf(return_message,"Error ins wBaseFavChg sqlcode =[%d]!", SQLCODE);
                    pubLog_Debug(_FFL_,"","","%s",return_message);
                    return 351979;
                }
            }
        }
        EXEC SQL CLOSE dGrpUserMebMsgCur;
    }
    EXEC SQL CLOSE dGrpSrvMsgCur;  
    
    return 0;
}


/************************************************************************
��������:fDeleteGrpUserMebBaseFav
����ʱ��:2006/11/21
������Ա:Huaxm
��������:ɾ�����ų�Ա�û��Ʒѱ�dBaseFav��¼
������
         typedef struct tPubCancelGrpUserTag
         {
           char sGrpId_No[14+1];      / * ����-�����û�ID_NO* /
           char sLogin_No[6+1];       / * ����-����Ա������* /
           char sOp_Code[4+1];        / * ����-��������* /
           char sOp_Time[20+1];       / * ����-ϵͳʱ��* /
           char sSysAccept[14+1];     / * ����-��ˮ��*  /
           char sOp_Note[60+1];       / * ����-��������*  /
           char sReturnMsg[60+1];     / * ���-���ش�����Ϣ*  /
         }tPubCancelGrpUser;
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fDeleteGrpUserMebBaseFav(tDeleteGrpUserMebBaseFav *funcIn)
{
    int  retCode=0;
    EXEC SQL BEGIN DECLARE SECTION;
    char sGrpIdNo[14+1];              /* �����û�ID_NO*/
    char sMemberNo[14+1];             /* ��Ա�û�PHONE_NO, ALL��ʾȫ����Ա*/
    char sLoginNo[6+1];               /* ����Ա������*/
    char sOpCode[4+1];                /* ��������*/
    char sOpTime[20+1];               /* ϵͳʱ��*/
    char sSysAccept[14+1];            /* ��ˮ��*/
    char sOpNote[60+1];               /* ��������*/
    char *return_message = funcIn->sReturnMsg;             /* ������Ϣ*/
    char sTotalDate[8+1];             /* */
    
    char sGrpSendCode[10+1];
    char sBeginMemberNo[14+1];
    char sEndMemberNo[14+1];
    char sPhoneNo[22+1];
    char sIdNo[22+1];

    EXEC SQL END DECLARE SECTION;
    char sBuffer[2048];
    int  iCount = 0;

    init(sGrpIdNo);
    init(sMemberNo);
    init(sLoginNo);
    init(sOpCode);
    init(sOpTime);
    init(sSysAccept);
    init(sOpNote);
    init(sTotalDate);
    
    init(sGrpSendCode);
    init(sBeginMemberNo);
    init(sEndMemberNo);
    init(sPhoneNo);
    init(sIdNo);
    init(sBuffer);

    
    strcpy(sGrpIdNo,      funcIn->sGrpId_no);
    strcpy(sMemberNo,     funcIn->sMember_no);
    strcpy(sLoginNo,      funcIn->sLogin_no);
    strcpy(sOpCode,       funcIn->sOp_code);
    strcpy(sOpTime,       funcIn->sOp_time);
    strcpy(sSysAccept,    funcIn->sSys_accept);
    strcpy(sOpNote,       funcIn->sOp_note);
    strncpy(sTotalDate,   sOpTime, 8);
    
    Trim(sGrpIdNo);
    Trim(sMemberNo);
    Trim(sLoginNo);
    Trim(sOpCode);
    Trim(sOpTime);
    Trim(sSysAccept);
    Trim(sOpNote);
  
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","","begin function fDeleteGrpUserMebBaseFav();-------------");
    pubLog_Debug(_FFL_,"","","sGrpIdNo   =[%s]",    sGrpIdNo);
    pubLog_Debug(_FFL_,"","","sMemberNo  =[%s]",    sMemberNo);
    pubLog_Debug(_FFL_,"","","sLoginNo   =[%s]",    sLoginNo);
    pubLog_Debug(_FFL_,"","","sOpCode    =[%s]",    sOpCode);
    pubLog_Debug(_FFL_,"","","sOpTime    =[%s]",    sOpTime);
    pubLog_Debug(_FFL_,"","","sSysAccept =[%s]",    sSysAccept);
    pubLog_Debug(_FFL_,"","","sOpNote    =[%s]",    sOpNote);
    pubLog_Debug(_FFL_,"","","sTotalDate =[%s]",    sTotalDate);
#endif
    
    if((strncmp(sMemberNo, "ALL", 3) == 0) || strlen(sMemberNo) == 0)
    {
        strcpy(sMemberNo, "%%");
        strcpy(sBeginMemberNo, "00000000000000");
        strcpy(sEndMemberNo,   "99999999999999");
#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","Modify: sBeginMemberNo  =[%s]",    sBeginMemberNo);
        pubLog_Debug(_FFL_,"","","Modify: sEndMemberNo    =[%s]",    sEndMemberNo);
#endif
    } 
    
    
    init(sGrpSendCode);
    createGrpSendCode(sGrpIdNo, sGrpSendCode);
    
    EXEC SQL DECLARE dGrpUserMebMsgCurA CURSOR for
              select member_no, member_id from dGrpUserMebMsg 
               where id_no = :sGrpIdNo
                 and member_no >= :sBeginMemberNo
                 and member_no <= :sEndMemberNo
                 and sysdate between begin_time and end_time;
    EXEC SQL OPEN dGrpUserMebMsgCurA;
    for(;;)
    {
        EXEC SQL FETCH dGrpUserMebMsgCurA INTO :sPhoneNo, sIdNo;
        if(SQLCODE == 1403) break;
        else if(SQLCODE != 0)
        {
            sprintf(return_message,"��ѯdGrpUserMebMsg��ʧ��! [%d]", SQLCODE);
            pubLog_Debug(_FFL_,"","","%s",return_message);
            EXEC SQL CLOSE dGrpUserMebMsgCurA ;
            return 351951;
        }
        Trim(sPhoneNo);
        Trim(sIdNo);

#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","","phone_no=[%s] id_no=[%s] sGrpSendCode=[%s]", sPhoneNo, sIdNo, sGrpSendCode);
#endif        
        
        EXEC SQL update dBaseFav set end_time = to_date(:sOpTime,'yyyymmdd hh24:mi:ss')
                  where flag_code = :sGrpSendCode
                    and msisdn = :sPhoneNo
                    and sysdate between start_time and end_time;
        if(SQLCODE != 0 && SQLCODE != 1403)
        {
	        PUBLOG_DBDEBUG("");
            return 351952;
        }    
        
        
        EXEC SQL INSERT INTO wBaseFavChg
                      (
                        msisdn,fav_brand,fav_type,flag_code,fav_order,fav_period,free_value,
                        start_time,end_time,id_no,group_id,product_code,flag,deal_time
                      )
                 select 
                        MSISDN, FAV_BRAND, FAV_TYPE, FLAG_CODE, FAV_ORDER, FAV_PERIOD, FREE_VALUE, 
                        start_time, end_time, ID_NO, GROUP_ID, PRODUCT_code, '2', sysdate
                   from dBaseFav
                  where flag_code = :sGrpSendCode
                    and msisdn = :sPhoneNo
                    and end_time = to_date(:sOpTime,'YYYYMMDD HH24:MI:SS');
        if(SQLCODE != 0 && SQLCODE != 1403)
        {
            pubLog_Debug(_FFL_,"","","Ins wBaseFavChg Error sqlcode = [%d]", SQLCODE);
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","351953","Ins wBaseFavChg Error sqlcode = [%d]", SQLCODE);             
            return 351953;
        }
    }
    EXEC SQL CLOSE dGrpUserMebMsgCurA ;
    
    return 0;
}
