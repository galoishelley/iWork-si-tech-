/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
/*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 <sXXXXCfm(opCode1,opCode3....)> ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "publicsrv.h"
#include "order_xml.h"
#include "pubOrder.h"


#include "publicEmer.h"
#define DB_LONG_CONN
#include "tux_server.h"
/**  �����������б�
  *  CheckPhoneSimNo     : �������  �ֻ����롢SIM��״̬          
  *  pubOpenAcctRes      : �û������޸���Դ��Ϣ                 
  *  pubOpenAcctFunc     : �û����������ط���Ϣ(��ʱ��Ҫ)                 
  *  pubOpenAcctCheck_1    : �û�����֧Ʊ���ݴ���                 
  *  pubCBXExpire        : �����û���Ч��
  *  pubPack             : Ӫ��������                 
  *  pubOpenCustConfirm  : �û��еĿͻ�����
  *  pubOpenAcctConfirm  : ��¼�û�������ؿͻ����û����ʻ����� 
  *  pubOpenAcctFav      : �û������Żݴ���                     
  *  pubOpenAcctSendCmd  : �û��������ػ�����                   
  *  pubSZXConfirm       : �����п���ȷ�ϲ���    
  *  pubSMAPConfirm      : ��������������
  *  pubDefaultFunc      : ���Ĭ���ط��������ط��� 
  *  pubOpenFunc         : �û����������ط���Ϣ       
  *  pubOpenDataCard     : ��e�п���       
  *  pubDataCred         : ���ݿ�����       
  *  s1104Cfm            : ��ͨ����(1104)�������п���(1106)�����ݿ�����(1120) 
  *  s1108Cfm            : �����п���(1108)
  *  s1114Cfm            : ȫ��ͨ������������
  *  s1118Cfm            : ������������������
  */
  
#define _DEBUG_
#define ROWLEN 2048  /* �����ַ���Ĭ�ϳ��� */
#define ROWNUM11 200    /* �����ַ���Ĭ����������� */
#define ORDER_RIGHT 100
/*����ORACLE��SQLͨ����*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

TUX_SERVER_BEGIN();

/*
char *substr(char *source,int begin,int num)
{
    int i;
    char dest[2048+1]="\0";
    if(begin<1 || num<1) return NULL;
    for(i=begin-1;i<begin+num-1;i++){
        dest[i-begin+1]=source[i];
    }
    dest[i-begin+1]='\0';
    return dest;
}
*/
/*
void NMSWriteStartLog(char *RunFlag,int Sequence,char *LoginNo,char *OpCode,char *UsrID, char *UsrNo, char *ServiceName,char *BeginTime, int ParaNum,char *Para1,char *Para2,char *Para3,char *Para4);
void NMSWriteEndLog(char *RunFlag,int Sequence, char *ServiceState,char *EndTime,char *OpCode);
*/
extern char *substr(char *source,int begin,int num);

/**
  * Auth: liujj
  * Name: pubIsNetAmount
  * Func: У���Ƿ��Ǿ���
  * Date: 2012.10.29
  * Stat: edit
  * return : 0 - �Ǿ��� 1 - ���� else - ��������
  */

int pubIsNetAmount(char *phone_no,char *bossorgcode,char *sm_code , float prepay_fee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vPhoneNo[20 + 1];
		char vBossOrgCode[9 + 1];
		char vRegionCode[2 + 1];
		char vDisCode[2 + 1]; 
		char vSmCode[2 + 1];
		float vPrepayFee = 0.00 ;
		char vIsNetAmount[2 + 1];
		
		char vChnType[1 + 1]; 
		int  iPreFeeFlag = 0; /*������ϱ��*/
		int  iIsReport = 0;
		float vDeposit = 0.00;
		
		char vErrorMsg[60 +1];
	
	EXEC SQL END DECLARE SECTION;
	
	init(vPhoneNo);
	init(vBossOrgCode);
	init(vRegionCode);
	init(vDisCode);
	init(vChnType);
	init(vErrorMsg);
	init(vSmCode);
	init(vIsNetAmount);
	
	strcpy(vPhoneNo,phone_no);
	strcpy(vBossOrgCode,bossorgcode);
	strcpy(vSmCode,sm_code);
	vPrepayFee = prepay_fee;
	
	Trim(vPhoneNo);
	Trim(vBossOrgCode);
	Trim(vSmCode);
	
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vPhoneNo     = %s",vPhoneNo);
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vBossOrgCode = %s",vBossOrgCode);
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vSmCode      = %s",vSmCode);
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vPrepayFee   = %10.2f",vPrepayFee);
	
	/**
	** ��ȡ ��������,���д���
	**/
	EXEC SQL select substr(:vBossOrgCode,1,2), substr(:vBossOrgCode,3,2)
							into :vRegionCode,:vDisCode
							from dual;
	if(SQLCODE != 0)
	{
		sprintf(vErrorMsg,"��ȡ����������д���ʧ��![%s]",vBossOrgCode);
		PUBLOG_DBDEBUG("pubIsNetAmount");
		pubLog_DBErr(_FFL_,"pubIsNetAmount","800006",vErrorMsg);        
		return 800006;
	}
	Trim(vRegionCode);
	Trim(vDisCode);
	
	
	/***
	** ��ѯ�Ƿ��Ǵ�����������
	***/
	
	EXEC SQL SELECT DISTINCT c.chntype
						into :vChnType
						FROM dcustres a, dchngroupmsg b, schnclass c
						WHERE phone_no = :vPhoneNo
							AND a.GROUP_ID = b.GROUP_ID
							AND b.class_code = c.class_code;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		sprintf(vErrorMsg,"��ѯ����[%s]�Ƿ��Ǵ�����������ʧ��!",vPhoneNo);
		PUBLOG_DBDEBUG("pubIsNetAmount");
		pubLog_DBErr(_FFL_,"pubIsNetAmount","800001",vErrorMsg);        
		return 800001;
	}
	else if(SQLCODE == 1403)
	{
		/**
		** �Ǿ���
		**/
		return 0;
	}
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vChnType     = %s",vChnType);
	Trim(vChnType);
	
	/***
	** ��ѯ�Ƿ�����ϵ���
	***/
	iPreFeeFlag = 0;
	EXEC SQL select count(1)
							into :iPreFeeFlag
						from srptmdcode 
						where prefee_flag='1' 
							and region_code=:vRegionCode 
							and district_code=:vDisCode;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		sprintf(vErrorMsg,"��ѯ����[%s][%s]�Ƿ����ʧ��!",vRegionCode,vDisCode);
		PUBLOG_DBDEBUG("pubIsNetAmount");
		pubLog_DBErr(_FFL_,"pubIsNetAmount","800002",vErrorMsg);        
		return 800002;
	}
	else if(SQLCODE == 1403)
	{
		/**
		** �Ǿ���
		**/
		return 0;
	}
	pubLog_Debug(_FFL_, "pubIsNetAmount", "", "iPreFeeFlag     = %d",iPreFeeFlag);
	/*˵��:(����ϵ���������������)  ����  (����������)*/
	if ((strcmp(vChnType,"2") == 0 && iPreFeeFlag == 0) || strcmp(vChnType,"3") == 0)
	{
		/***
		** ��һ���ж�����Ԥ����Ƿ��������
		***/
		EXEC SQL select deposit
							into :vDeposit
							from dbqdadm.schnnetamountdeveloprule 
							where region_code=:vRegionCode 
								and dis_code=:vDisCode 
								and sm_type=decode(:vSmCode,'gn','gn','dn','dn','hn') 
								AND year_month = to_char(sysdate,'yyyymm')
								and busi_code='04';
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(vErrorMsg,"��ѯ����[%s][%s][%s]���Ԥ��ʧ��!",vRegionCode,vDisCode,vSmCode);
			PUBLOG_DBDEBUG("pubIsNetAmount");
			pubLog_DBErr(_FFL_,"pubIsNetAmount","800003",vErrorMsg);        
			return 800003;
		}
		else if(SQLCODE == 1403)
		{
			/**
			** �Ǿ���
			**/
			return 0;
		}
		pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vDeposit     = %d",vDeposit);
		if(vDeposit <= vPrepayFee)
		{
			/***
			** ��ѯ������
			**/
			EXEC SQL SELECT a.is_net_amount
							into :vIsNetAmount
						FROM dbqdadm.schnnetamountrule a
						WHERE a.busi_code = '04'
							AND a.region_code = :vRegionCode
							AND a.dis_code = :vDisCode
							AND year_month = to_char(sysdate,'yyyymm')
							AND ROWNUM = 1;
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				sprintf(vErrorMsg,"��ѯ����[%s][%s]�Ƿ񾻶�ʧ��!",vRegionCode,vDisCode);
				PUBLOG_DBDEBUG("pubIsNetAmount");
				pubLog_DBErr(_FFL_,"pubIsNetAmount","800004",vErrorMsg);        
				return 800004;
			}
			else if(SQLCODE == 1403)
			{
				/**
				** �Ǿ���
				**/
				return 0;
			}
			Trim(vIsNetAmount);
			pubLog_Debug(_FFL_, "pubIsNetAmount", "", "vIsNetAmount     = %s",vIsNetAmount);
			if(strcmp(vIsNetAmount,"0") == 0)
			{
				/**
				** ��ѯ�Ƿ���Ҫ����
				**/
				iIsReport = 0;
				EXEC SQL SELECT count(1)
										into :iIsReport
									FROM dchngroupmsg a, dbqdadm.schnclassreport b
									WHERE a.class_code = b.class_code 
										AND a.boss_org_code = :vBossOrgCode;
				if(SQLCODE != 0 && SQLCODE != 1403)
				{
					sprintf(vErrorMsg,"��ѯORGCODE[%s]�Ƿ����ʧ��!",vBossOrgCode);
					PUBLOG_DBDEBUG("pubIsNetAmount");
					pubLog_DBErr(_FFL_,"pubIsNetAmount","800005",vErrorMsg);        
					return 800005;
				}
				else if(SQLCODE == 1403)
				{
					/**
					** �Ǿ���
					**/
					return 0;
				}
				else
				{
					pubLog_Debug(_FFL_, "pubIsNetAmount", "", "iIsReport     = %d",iIsReport);
					if(iIsReport > 0)
					{
						/**
						** ����
						**/
						return 1;
					}
					else
					{
						/**
						** �Ǿ���
						**/
						return 0;
					}
				}
			}
			else
			{
				/**
				** �Ǿ���
				**/
				return 0;
			}
			
		}
		else
		{
			/**
			** �Ǿ���
			**/
			return 0;
		}
	}
	else
	{
		/**
		** �Ǿ���
		**/
		return 0;
	}

	return 0;

}


/**
  * Auth: YuanBY
  * Name: CheckPhoneSimNo
  * Func: ��������ֻ����롢SIM��״̬
  * Date: 2003/10/14
  * Stat: Test Pass
  */
int CheckPhoneSimNo(char sIn_Phone_noA[12], char sIn_OrgCodeA[11], 
                    char sIn_Sm_codeA[3],   char sIn_Sim_noA[21],   
                    char sIn_Sim_typeA[6],  char sIn_Phone_IccA[20],
                    char sIn_Phone_logNoA[6],char * sErrorMessA,char *vOrgId)
{
    EXEC SQL BEGIN DECLARE SECTION;                               
        char    sIn_Phone_no[15+1];          /* �ֻ�����      */
        char    sIn_OrgCode[9+1];            /* ��������      */
        char    sIn_Sm_code[2+1];            /* ҵ������      */
        char    sIn_Sim_no[20+1];            /* SIM������     */
        char    sIn_Sim_type[5+1];           /* SIM������     */
        char    sIn_Phone_Icc[20+1];	     /* ֤������      */
        char	  sIn_Phone_logNo[6+1];	     /* ����          */
                                                              
        char    sV_Region_code[2+1];         /* ��������      */
        char    sV_District_code[2+1];       /* ���ش���      */
        char    sV_Town_code[3+1];           /* �������      */
        char    sV_Phone_hlrcode[3+1];       /* �ֻ�  HlrCode */
        char    sV_Sim_hlrcode[3+1];         /* Sim�� HlrCode */
        char    sV_No_type[1+1];             /* ��������      */
        char    sV_Phone_head[7+1];          /* �Ŷ�ͷ        */
        char    sV_Bel_login[6+1];                /* sim���Ĺ������� */
        
        char    sTmp_No_type[1+1];           /* ��ʱ��������  */
        char    sTmp_Region_code[2+1];       /* ��ʱ��������  */
        char    sTmp_District_code[2+1];     /* ��ʱ���ش���  */
        char    sTmp_Town_code[3+1];         /* ��ʱ�������  */  
        char    sTmp_State[2+1];             /* ��ʱ״̬      */
        
        char    sV_Sm_code[2+1];             /* ҵ������      */
        char    sV_Sim_type[5+1];            /* SIM������     */
                                             
        int     iV_Count=0;                    /* ��¼����      */
        int     vCount=0;
        int 	  vvCount=0;
        char    vLchntype[1+1];			/*���Ź�����������*/
        char    vPchntype[1+1];			/*���������������*/
        char    vLgroupid[10+1];		/*����group_id*/
        char    vPgroupid[10+1];		/*����group_id*/
        char	vGroupChnType[1+1];
		    char	vSimfeeFlag[1+1];

        char 	vGroupid_res[10+1];
		int		ret=0;
		char 	flag[2];
		char	vClassCode[4+1];
		char	vIsTown[1+1];

    EXEC SQL END DECLARE SECTION;
    
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "--------------- begin function CheckPhoneSimNo ----------");
    
    /* ���Ի����� */
    init(vGroupChnType);
    init(vSimfeeFlag);
    init(sIn_Phone_no);
    init(sIn_OrgCode);
    init(sIn_Sm_code);
    init(sIn_Sim_no);
    init(sIn_Sim_type);
    init(sIn_Phone_Icc);
    init(sIn_Phone_logNo);
    
    init(sV_Region_code);
    init(sV_District_code);
    init(sV_Town_code);
    init(sV_Phone_hlrcode);
    init(sV_Sim_hlrcode);
    init(sV_Phone_head);
    init(sV_Bel_login);
    init(vGroupid_res);
    init(flag);
    init(vClassCode);
    init(vIsTown);
 
    /* ȡ��������� */
    strcpy(sIn_Phone_no,sIn_Phone_noA);
    strcpy(sIn_OrgCode,sIn_OrgCodeA);
    strcpy(sIn_Sm_code,sIn_Sm_codeA);
    strcpy(sIn_Sim_no,sIn_Sim_noA);
    strcpy(sIn_Sim_type,sIn_Sim_typeA);
    strcpy(sIn_Phone_Icc,sIn_Phone_IccA);
    strcpy(sIn_Phone_logNo,sIn_Phone_logNoA);
    
    Trim(sIn_Phone_no);
    Trim(sIn_OrgCode);
    Trim(sIn_Sm_code);
    Trim(sIn_Sim_no);
    Trim(sIn_Sim_type);
    Trim(sIn_Phone_Icc);
    Trim(sIn_Phone_logNo);
    Trim(vOrgId);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_OrgCode       =[%s]",      sIn_OrgCode); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Sm_code       =[%s]",      sIn_Sm_code);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Sim_no        =[%s]",      sIn_Sim_no); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Sim_type      =[%s]",      sIn_Sim_type); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Phone_Icc     =[%s]",      sIn_Phone_Icc);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Phone_logNo   =[%s]",      sIn_Phone_logNo);
    #endif    
    /* ��ֻ������� */
   
    EXEC SQL SELECT SUBSTR(:sIn_OrgCode,1,2),SUBSTR(:sIn_OrgCode,3,2),
                    SUBSTR(:sIn_OrgCode,5,3)
               INTO :sV_Region_code,        :sV_District_code,
                    :sV_Town_code
               FROM DUAL;
    if (SQLCODE !=0){
        strcpy(sErrorMessA,"��ֹ����������!");
		PUBLOG_DBDEBUG("CheckPhoneSimNo");
		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999901",sErrorMessA);        
        return 999901;
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Region_code   =[%s] ", sV_Region_code);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_District_code =[%s] ", sV_District_code);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Town_code     =[%s] ", sV_Town_code); 
    #endif
    
    /* ��ѯ�������� */
    EXEC SQL SELECT  No_Type  
               INTO :sV_No_type
               FROM  sNoTypecode 
              WHERE  Region_Code = :sV_Region_code
                AND  Sm_Code     = :sIn_Sm_code;
    if (SQLCODE !=0)
    {
        strcpy(sErrorMessA,"��ѯsNoTypecode�����!");        
		PUBLOG_DBDEBUG("CheckPhoneSimNo");
		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999902",sErrorMessA);        
        return 999902 ;	
    }
     
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_No_type = [%s]",sV_No_type); 
    #endif
    
    /* ��ѯ����� �������� , ���ش��� , ������� , ����״̬ */
    init(sTmp_Region_code);
    init(sTmp_District_code);
    init(sTmp_Town_code);
    init(sTmp_State);
    init(sTmp_No_type);

 
    EXEC SQL SELECT  
                     Group_id, Resource_Code,       
                     substr(No_Type,5,1)
               INTO :vGroupid_res,
                    :sTmp_State ,
                    :sTmp_No_type
               FROM dCustRes
              WHERE Phone_No = :sIn_Phone_no;
    if (SQLCODE !=0)
    {
    	if (SQLCODE == 1403)
    	{
    	    strcpy(sErrorMessA,"�ֻ����벻������dCustRes��!");    	    
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999904",sErrorMessA);       	    
            return 999904 ;
    	}
    	else
    	{
            strcpy(sErrorMessA,"��ѯdCustRes�����!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999905",sErrorMessA);
            return 999905 ;     
        }	
    }
    Trim(vGroupid_res);
    Trim(sTmp_Region_code);
    Trim(sTmp_District_code);
    Trim(sTmp_Town_code);
    Trim(sTmp_State);
    Trim(sTmp_No_type);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_No_type       = [%s] ",  sTmp_No_type);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_Region_code   = [%s] ",  sTmp_Region_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_District_code = [%s] ",  sTmp_District_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_Town_code     = [%s] ",  sTmp_Town_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_State         = [%s] ",  sTmp_State); 
    #endif
    
    if ((strcmp(sTmp_State,"0")!=0) && (strcmp(sTmp_State,"h")!=0)&&(strcmp(sTmp_State,"w")!=0)&&(strcmp(sTmp_State,"9")!=0)&&(strcmp(sTmp_State,"j")!=0)&&(strcmp(sTmp_State,"p")!=0))
    {
        strcpy(sErrorMessA,"�ֻ������Ѿ���ʹ��!");
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return 999911 ; 
    }
    
    Trim(sIn_Phone_Icc);
    Trim(sIn_Phone_no);
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++sIn_Phone_Icc[%s]",sIn_Phone_Icc);
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++sIn_Phone_no[%s]",sIn_Phone_no);
    
    vCount=0;
    EXEC SQL select count(*) into :vCount 
    	from dCustOccupied 
    	where phone_no=:sIn_Phone_no and op_kind='0' and begin_time >sysdate-1/24;
    if(SQLCODE!=0)
    {
        	strcpy(sErrorMessA,"��ѯdCustOccupied��ʧ��!");	        	
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999801",sErrorMessA);        	
        	return 999801;
    }
    if(vCount>0)
    {
    	vvCount=0;
    	EXEC SQL select count(*) into :vvCount 
    	from dCustOccupied 
    	where phone_no=:sIn_Phone_no and id_no=trim(:sIn_Phone_Icc) and op_kind='0' and begin_time >sysdate-1/24;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"��ѯdCustOccupied��ʧ��!");	
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999819",sErrorMessA);         	
        	return 999819;
    	}
    	if(vvCount==0)
    	{
    		strcpy(sErrorMessA,"�û�������֤��������ԤԼ֤�����벻һ��!");
    		return 999802;
    	}
    	EXEC SQL insert into dCustOccupiedhis(ID_NO,PHONE_NO,BEGIN_TIME,LOGIN_ACCEPT,BAK_FIELD,
    					UPDATE_ACCEPT,UPDATE_LOGIN,UPDATE_TIME,UPDATE_CODE)
    				select ID_NO,PHONE_NO,BEGIN_TIME,sMaxSysAccept.nextVal,'',
    					UPDATE_ACCEPT,:sIn_Phone_logNo,sysdate,'2592'
    			from dCustOccupied 
    			where phone_no=:sIn_Phone_no 
    			and id_no=trim(:sIn_Phone_Icc) 
    			and op_kind='0' 
    			and begin_time >sysdate-1/24;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"����dCustOccupiedhis��ʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999820",sErrorMessA); 
        	return 999820;
    	}
    	EXEC SQL delete dCustOccupied 
    			where phone_no=:sIn_Phone_no 
    			and id_no=trim(:sIn_Phone_Icc) 
    			and op_kind='0' 
    			and begin_time >sysdate-1/24;		
	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"ɾ��dCustOccupied��ʧ��!");	
       		PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999821",sErrorMessA); 
        	return 999821;
    	}
    }
    
    vCount=0;
    EXEC SQL select count(*) into :vCount 
    	from dCustOccupied 
    	where phone_no=:sIn_Phone_no and op_kind='1' and begin_time >sysdate-1;
    if(SQLCODE!=0)
    {
        	strcpy(sErrorMessA,"��ѯdCustOccupied��ʧ��!");	
      		PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999801",sErrorMessA); 
        	return 999801;
    }
    if(vCount>0)
    {
    	vvCount=0;
    	EXEC SQL select count(*) into :vvCount 
    	from dCustOccupied 
    	where phone_no=:sIn_Phone_no and id_no=trim(:sIn_Phone_Icc) and op_kind='1' and begin_time >sysdate-1;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"��ѯdCustOccupied��ʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999819",sErrorMessA); 
        	return 999819;
    	}
    	if(vvCount==0)
    	{
    		strcpy(sErrorMessA,"�û�������֤��������ԤԼ֤�����벻һ��!");
    		return 999802;
    	}
    	EXEC SQL update webPhoneres set resource_code='5' where phone_no=:sIn_Phone_no;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"���±�webPhoneresʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999853",sErrorMessA);
        	return 999853;
    	}
    	EXEC SQL insert into webPhoneres_tmp (LOGIN_ACCEPT,PHONE_NO,NO_TYPE,RESOURCE_CODE,CHOICE_FEE,
    				LIMIT_FEE,REGION_CODE,GROUP_ID,GROUP_NAME,DISTRICT_CODE,UPDATE_TIME,TRANS_TIME)
    		select 0,PHONE_NO,NO_TYPE,RESOURCE_CODE,CHOICE_FEE,
    			LIMIT_FEE,REGION_CODE,GROUP_ID,GROUP_NAME,DISTRICT_CODE,sysdate,sysdate
    		from webPhoneres where phone_no=:sIn_Phone_no;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"insert webPhoneres_tmpʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999854",sErrorMessA);
        	return 999854;
    	}
    }

 	EXEC SQL select class_code into :vClassCode from dchngroupmsg where group_id=:vGroupid_res;
    if (SQLCODE !=0 )
    {
 			strcpy(sErrorMessA,"�ȽϹ�������!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999920",sErrorMessA);
			return 999920 ;    		
    }    
    Trim(vClassCode);
    ret=sCheckGroupId(vGroupid_res,vOrgId,"11",flag,sErrorMessA);
 	if(ret != 0)
 	{
 			
			strcpy(sErrorMessA,"�ȽϹ�������!");
			pubLog_Debug(_FFL_,"CheckPhoneSimNo","999906",sErrorMessA);
			return 999906 ; 
 	}    
  	if  (strcmp(flag,"N") == 0)
 	{
 			strcpy(sErrorMessA,"���ֻ����벻���ڱ�����!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999908",sErrorMessA);
			return 999908 ;
 	}    
    
     /*
    if ((strcmp(sTmp_District_code,sV_District_code)!=0))
    {
        strcpy(sErrorMessA,"�ֻ����벻���ڱ�����!");
    	return 999803 ; 
    }*/
    if (strcmp(vClassCode,"11")!=0){
	    ret=sCheckGroupId(vGroupid_res,vOrgId,"12",flag,sErrorMessA);
	 	if(ret != 0)
	 	{
	 			
				strcpy(sErrorMessA,"�ȽϹ�������!");
				pubLog_Debug(_FFL_,"CheckPhoneSimNo","999919",sErrorMessA);
				return 999919 ; 
	 	}
		
	  	if  (strcmp(flag,"N") == 0)
	 	{
	 			strcpy(sErrorMessA,"���ֻ����벻���ڱ�����!");
				pubLog_Debug(_FFL_,"CheckPhoneSimNo","999910",sErrorMessA);
				return 999910 ;
	 	}    
	 }	
     /*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 end*/ 
    /*���Ź����������ͣ�*/
	EXEC SQL select distinct c.chntype 
	into :vLchntype 
	from dloginmsg a,dchngroupmsg b,schnclass c
	where a.login_no=:sIn_Phone_logNo and a.org_id = b.group_id /* and a.group_id=b.group_id edit by liuweib20091023*/
	and b.class_code=c.class_code;
	if(SQLCODE!=0)
   	{
   	    strcpy(sErrorMessA,"��ѯ���Ź�����������ʧ��!");	
 	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999804",sErrorMessA);
   	    return 999804;
   	}
   	if((strcmp(vLchntype,"0")!=0)&(strcmp(vLchntype,"2")!=0))
   	{
   			strcpy(sErrorMessA,"�ù��Ź����������ʹ���!");	
   	    	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "�ù��Ź����������ʹ���[%s][%s]",sIn_Phone_logNo,vLchntype);
   	    	return 999899;
   	}

   /*��������������ͣ�*/
	EXEC SQL select distinct c.chntype 
	into :vPchntype 
	from dcustres a,dchngroupmsg b,schnclass c
	where phone_no=:sIn_Phone_no and a.group_id=b.group_id
	and b.class_code=  c.class_code;
	if(SQLCODE!=0)
       	{
       	    strcpy(sErrorMessA,"��ѯ���������������ʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999805",sErrorMessA);
       	    return 999805;
       	}
       	if((strcmp(vPchntype,"0")!=0)&(strcmp(vPchntype,"1")!=0)&(strcmp(vPchntype,"2")!=0)&(strcmp(vPchntype,"3")!=0))
       	{
	       		strcpy(sErrorMessA,"�ú�������������ʹ���!");	
				pubLog_Debug(_FFL_,"CheckPhoneSimNo","999898",sErrorMessA);
       	    	return 999898;
       	}
   /*����group_id��*/
	EXEC SQL select org_id /*group_id->org_id edit by liuweib20091023*/
		into :vLgroupid 
		from dloginmsg 
		where login_no=:sIn_Phone_logNo;
	if(SQLCODE!=0)
       	{
       	    strcpy(sErrorMessA,"��ѯ����group_idʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999806",sErrorMessA);
       	    return 999806;
       	}
   /*����group_id��*/
	EXEC SQL select group_id into :vPgroupid from dcustres where phone_no=:sIn_Phone_no;
	if(SQLCODE!=0)
   	{
   	    strcpy(sErrorMessA,"��ѯ����group_idʧ��!");	
 	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999807",sErrorMessA);
   	    return 999807;
   	} 
       	
       	/********20060825 add ��� begine**********/
       	EXEC SQL select simfee_flag
       				into :vSimfeeFlag
       				from srptmdcode
       				where region_code=substr(:sIn_OrgCode,1,2)
       				and district_code=substr(:sIn_OrgCode,3,2);
        if(SQLCODE!=0)
       	{
       	    strcpy(sErrorMessA,"��ѯsrptmdcodeʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999887",sErrorMessA);
       	    return 999887;
       	} 
       	if(vSimfeeFlag[0]=='1')
       	{
       		if(vPchntype[0]=='3')
       		{
       			EXEC SQL select distinct(b.chntype) 
       							into :vGroupChnType
       					from dchngroupmsg a,schnclass b 
						where trim(a.group_id)=trim(:vLgroupid)
						and a.class_code= b.class_code  
						and a.class_code=b.parent_class_code;
				if(SQLCODE!=0)
       			{
       	    		strcpy(sErrorMessA,"��ѯvGroupChnTypeʧ��!");	
		     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
					pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999888",sErrorMessA);
       	    		return 999888;
       			}
       			
       			if(vGroupChnType[0]=='0')
       			{
       				strcpy(sErrorMessA,"��ϵ��д�������Դ�԰�����������");	
       	    		pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", " vGroupChnType= [%s]",vGroupChnType);
       	    		return 999889;
       			}
       		
       		}
       	}
       	/********20060825 add ��� end**********/
        
        /*�Ƿ�����������ж���Դ�Ĺ���group_id�Ƿ�Ϊ��½���ŵĸ����*/
       	EXEC SQL select count(*) into :vCount 
    			from dChnGroupInfo 
    			where parent_group_id=:vPgroupid and group_id=:vLgroupid;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"��ѯdChnGroupInfo��ʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999829",sErrorMessA);
        	return 999829;
    	}
    	/*�Ƿ���ں�����:��wTownTown��group_id=��Դ������group_id,ͬʱallow_groupid�ǵ�½���Ź���������һ�������*/
    	Trim(vPgroupid);
    	Trim(vLgroupid);
       	EXEC SQL select count(*) into :vvCount 
    			from wTownTown 
    			where group_id=:vPgroupid and allow_groupid in
    			(select parent_group_id from dChnGroupInfo where group_id=:vLgroupid);
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"��ѯwTownTown��ʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999830",sErrorMessA);
        	return 999830;
    	}  	
       	
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vLchntype[%s]+++++++++++++",vLchntype);
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vPchntype[%s]+++++++++++++",vPchntype);
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vLgroupid[%s]+++++++++++++",vLgroupid);
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vPgroupid[%s]+++++++++++++",vPgroupid);
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vCount[%d]+++++++++++++",vCount);
       	pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "+++++++++++vvCount[%d]+++++++++++++",vvCount);
       	
       	
       	if(strcmp(vLchntype,"0")==0)
       	{
       		if(strcmp(vPchntype,"2")==0)
       		{
       			strcpy(sErrorMessA,"�ú�����Դ������������Ϊ����Ӫҵ�����������!");	
       	    		return 999808;
       		}
       		if(strcmp(vPchntype,"0")==0)
       		{
       			if(strcmp(vLgroupid,vPgroupid)!=0 && vCount==0 && vvCount==0)
       			{
       				strcpy(sErrorMessA,"�ú�����Դ�����빤����Դ������ͬ�����Ҹ���Դ���ܱ��ù��Ź������������!");	
       	    			return 999809;
       			}	
       		}
       	}
       	if(strcmp(vLchntype,"2")==0)
       	{
       		if((strcmp(vPchntype,"0")==0)||(strcmp(vPchntype,"1")==0))
       		{
       			strcpy(sErrorMessA,"�ú�����Դ������������Ϊ�԰�Ӫҵ���԰칲�����������!");	
       	    		return 999810;
       		}
       		if(strcmp(vPchntype,"2")==0)
       		{
       			if(strcmp(vLgroupid,vPgroupid)!=0 && vCount==0 && vvCount==0)
       			{
       				strcpy(sErrorMessA,"�ú�����Դ�����빤����Դ������ͬ�����Ҹ���Դ���ܱ��ù��Ź������������!");	
       	    			return 999811;
       			}
       		}
       	}
       	
    
    /* ��֤SIM�� */
    
    /* ��ѯ SIM��ҵ������ SIM������ �������� ���ش��� ������� ����״̬ */
    init(sV_Sm_code);
    init(sV_Sim_type);
    init(sTmp_Region_code);
    init(sTmp_District_code);
    init(sTmp_Town_code);
    init(sTmp_State);
    init(sV_Phone_head);
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sIn_Sim_no=[%s]",sIn_Sim_no);

    EXEC SQL SELECT  Sm_Code,    Sim_Type,
                     Region_Code,          NVL(District_Code,'ZZ'),NVL(Town_Code,'ZZZ'),group_id,
                     Sim_Status , PHONENO_HEAD, NVL(BEL_LOGIN,' ')
               INTO :sV_Sm_code , :sV_Sim_type , :sTmp_Region_code,:sTmp_District_code,:sTmp_Town_code,
                    :vGroupid_res,
                    :sTmp_State , :sV_Phone_head, :sV_Bel_login
               FROM  dSimRes
              WHERE Sim_No = :sIn_Sim_no;
    if (SQLCODE !=0)
    {
    	if (SQLCODE ==1403)
    	{
    	    strcpy(sErrorMessA,"SIM�����벻����dSimRes��!");
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999912",sErrorMessA);
            return 999912 ;  	
    	}
    	else
    	{
            strcpy(sErrorMessA,"��ѯdSimRes�����!");
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999913",sErrorMessA);
            return 999913 ;        	
        }
    }
    
    Trim(sV_Sm_code);
    Trim(sV_Sim_type);
    Trim(sTmp_Region_code);
    Trim(sTmp_District_code);
    Trim(sTmp_Town_code);
    Trim(sTmp_State);
    Trim(sV_Phone_head);
    Trim(vGroupid_res);
    
    /*#ifdef _DEBUG_*/
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Sm_code         = [%s]",    sV_Sm_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Sim_type        = [%s]",    sV_Sim_type); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_Region_code   = [%s]",    sTmp_Region_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_District_code = [%s]",    sTmp_District_code); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_Town_code     = [%s]",    sTmp_Town_code);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sTmp_State         = [%s]",    sTmp_State); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Phone_head      = [%s]",    sV_Phone_head); 
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Bel_login       = [%s]",    sV_Bel_login);
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "vGroupid_res       = [%s]",    vGroupid_res);
    /*#endif*/
    
    if (strcmp(sTmp_State,"0")!=0){
        strcpy(sErrorMessA,"SIM�������Ѿ���ʹ��!");
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "999914",sErrorMessA);
        return 999914 ; 
    }  

    if (strcmp(sV_Sim_type,sIn_Sim_type)!=0){
        strcpy(sErrorMessA,"SIM�����벻���ڱ�SIM������!");
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "999915",sErrorMessA);
        return 999915 ; 
    }  
    /*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 begin*/ 
 	EXEC SQL select class_code into :vClassCode from dchngroupmsg where group_id=:vGroupid_res;
	if (SQLCODE !=0 ){
 			strcpy(sErrorMessA,"��ù�������!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999926",sErrorMessA);
			return 999926; 	
		
	} 
	Trim(vClassCode);     
    /*              
    if (strcmp(sTmp_Region_code,sV_Region_code)!=0){
        strcpy(sErrorMessA,"SIM�����벻���ڱ�����!");
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return 999916 ; 
    }  */
    ret=sCheckGroupId(vGroupid_res,vOrgId,"11",flag,sErrorMessA);
 	if(ret != 0)
 	{
 			
			strcpy(sErrorMessA,"�ȽϹ�������!");
			
			pubLog_Debug(_FFL_,"CheckPhoneSimNo","999989",sErrorMessA);
			return 999989 ; 
 	}    
  	if  (strcmp(flag,"N") == 0)
 	{
 			strcpy(sErrorMessA,"SIM�����벻���ڱ����ػ���!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999916",sErrorMessA);
			return 999916 ;
 	}

 	if (strcmp(vClassCode,"11")!=0){
	    ret=sCheckGroupId(vGroupid_res,vOrgId,"12",flag,sErrorMessA);
	 	if(ret != 0)
	 	{
	 			
				strcpy(sErrorMessA,"�ȽϹ�������!");
				
				pubLog_Debug(_FFL_,"CheckPhoneSimNo","999989",sErrorMessA);
				return 999989 ; 
	 	}    
	 	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","vGroupid_res is %s",vGroupid_res);
			
	  	if  (strcmp(flag,"N") == 0)
	 	{
	 			strcpy(sErrorMessA,"SIM�����벻���ڱ����ػ���!");
				PUBLOG_DBDEBUG("CheckPhoneSimNo");
				pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999916",sErrorMessA);
				return 999916 ;
	 	}     
	} 	
     /*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 end*/ 
/*    
    if ((strcmp(sTmp_Town_code,sV_Town_code)!=0) 
       &&(strcmp(sTmp_Town_code,"999")!=0))
    {
       	iV_Count=0;
       	EXEC SQL select count(*) into :iV_Count from wtowntown where region_code=:sV_Region_code
       	    and district_code=:sV_District_code and town_code=:sV_Town_code and allow_town=:sTmp_Town_code;
       	if(SQLCODE!=0 && SQLCODE!=1403)
       	{
       	    strcpy(sErrorMessA,"��ѯwtowntown��ʧ��!");	
       	    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
       	    return 999918;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"SIM�����벻���ڱ��������������!");	
            pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	    return 999918 ; 
    	}
    }   
 */
	/*���ڶ���sim����ԴҪ����䵽ӪҵԱ��ʹ��*/
	/*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 begin*/ 
	/*
    if ((strcmp(sTmp_Town_code,sV_Town_code)!=0) 
       &&(strcmp(sTmp_Town_code,"999")!=0))
    {
		strcpy(sErrorMessA,"SIM�����벻���ڱ��������!");	
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	return 999918 ;
    } 
    */
    if  ((strcmp(vGroupid_res,vOrgId) != 0)&&(strcmp(vClassCode,"12")!=0)&&(strcmp(vClassCode,"11")!=0)) 
	{
		strcpy(sErrorMessA,"SIM�����벻���ڱ��������!");	
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "999918", "%s",sErrorMessA);
    	return 999918;
	}    	
  
    /*if ((strcmp(sIn_Phone_logNo,sV_Bel_login)!=0) && (strcmp(sTmp_Town_code,"999")!=0) && (strcmp(sTmp_Town_code,sV_Town_code)!=0))
    {
       	iV_Count=0;
       	EXEC SQL select count(*) into :iV_Count from wtowntown where region_code=:sV_Region_code
       	    and district_code=:sV_District_code and town_code=:sV_Town_code and allow_town=:sTmp_Town_code;
       	if(SQLCODE!=0 && SQLCODE!=1403)
       	{
       	    strcpy(sErrorMessA,"��ѯwtowntown��ʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999919",sErrorMessA);
       	    return 999919;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"SIM�����벻���ڸõ�½���Ż������!");	
            pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	    return 999920 ; 
    	}
    }*/
	     

	if(strcmp(sIn_Phone_logNo,sV_Bel_login)!=0 &&strcmp(vGroupid_res,vOrgId)!=0 &&strcmp(vClassCode,"12")!=0 && strcmp(vClassCode,"11")!=0)
	{
		iV_Count=0;		
       	EXEC SQL select count(*) into :iV_Count from wtowntown where  group_id =:vOrgId and allow_groupid = :vGroupid_res;
        if(SQLCODE!=0 && SQLCODE!=1403)
       	{
       	    strcpy(sErrorMessA,"��ѯwtowntown��ʧ��!");	
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999919",sErrorMessA);
       	    return 999919;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"SIM�����벻���ڸõ�½���Ż������!");	
            pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	    return 999920 ; 
    	}     	
	}
	
    /*******�������֯�ṹ���ڸ��� edit by  liuhao 19/05/2009 end*/ 
    
  /*****/
    strncpy(sV_Phone_head,sIn_Phone_no,7);
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Phone_head = [%s] ",sV_Phone_head);
    
    /* ��ѯ�ֻ�hlr_code */
    EXEC SQL SELECT  Hlr_Code 
               INTO :sV_Phone_hlrcode
               FROM  sHlrCode
              WHERE  Region_Code        = :sV_Region_code
                AND  Phoneno_Head = :sV_Phone_head;
    if (SQLCODE !=0){
        if (SQLCODE !=1403){
            strcpy(sErrorMessA,"��ѯsHlrCode�����!");
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999921",sErrorMessA);
            return 999921 ;  
        }
        else{
            strcpy(sErrorMessA,"���ֻ�����ĺŶ�û��ע�ᵽsHlrCode��!");
            pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return 999922 ;  	
        }	
    }
    
    Trim(sV_Phone_hlrcode);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Phone_hlrcode = [%s]",sV_Phone_hlrcode); 
    #endif
    
    /* ���SIM����Ӧ�ĺŶ�ͷ��7��9���Ͳ���Ҫ�˶�HLR */
    if(strcmp(sV_Phone_head,"9999999")==0)
        return 0;

    /* ��ѯSIM��hlr_code */
    EXEC SQL SELECT HLR_CODE into :sV_Sim_hlrcode
             from sHlrCode
             WHERE REGION_CODE = :sV_Region_code
                 and PHONENO_HEAD = :sV_Phone_head ;
    if (SQLCODE !=0){
        if (SQLCODE !=1403){
            strcpy(sErrorMessA,"��ѯsHlrCode�����!");
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999923",sErrorMessA);
            return 999923 ;
        }
        else{
            strcpy(sErrorMessA,"SIM�������Ӧ�ĺŶ�û����sHlrcode����ע��!");
     	    PUBLOG_DBDEBUG("CheckPhoneSimNo");
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999924",sErrorMessA);
            return 999924 ;	
            
        }	
    }
    
    Trim(sV_Sim_hlrcode);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "sV_Sim_hlrcode = [%s]",sV_Sim_hlrcode); 
    #endif
    
    /* �Ƚ�����HlrCode */
    if (strcmp(sV_Phone_hlrcode,sV_Sim_hlrcode)!=0){
        strcpy(sErrorMessA,"�ֻ�������SIM����hlr����Ӧ!");
        pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return 999925 ;
    }
    
    strcpy(sErrorMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "CheckPhoneSimNo", "", "--------------- end function CheckPhoneSimNo ----------");
    
    return 0 ;
	
}


/***************************************
 ��������  pubOpenCustConfirm
 ��������  2004/03/04
 ������Ա  FuJian copy Yuanby
 ���ù��̣�
 �����������û��еĿͻ�����
 ��������� 
	�ͻ���ʶ
	��������
	�ͻ�����
	�ͻ�����
	�ͻ�״̬
	�ͻ�����
	�ͻ����
	�ͻ���ַ
	�ͻ�֤������
	֤������
	֤����ַ
	֤����Ч��   yyyy-mm-dd
	��ϵ������
	��ϵ�˵绰
	��ϵ�˵�ַ
	��ϵ����������
	��ϵ��ͨѶ��ַ
	��ϵ�˴���
	��ϵ�˵�������
	��������
	�����û���־(1 ���û� , 2 ���û�)
	�ϼ��ͻ���ʶ
	------
	�Ա�
	��������    YYYY-MM-DD
	ְҵ����
	ѧ��
	�ͻ�����
	�ͻ�ϰ��
	------
	��λ����
	Ӫҵִ������
	Ӫҵִ�պ���
	Ӫҵִ����Ч��     YYYY-MM-DD
	���˴���
	------
	��������
        ��������
        ������ע
        IP_ADDR
	------
	�ɼ��ź�
	
 ��������� 
      �������
      ������Ϣ
      ������ˮ
      
      
�޸ļ�¼:
		�޸���Ա��
        �޸����ڣ�
		�޸�ǰ�汾�ţ�
		�޸ĺ�汾��:
		�޸�ԭ��
                                  
****************************************/
int pubOpenCustConfirm(char *send_type,char *send_id,char *sIn_CustMsg,char *sIn_CustId,char *sIn_CustContact,char *sIn_CustAttr,char *sIn_CustAdd,
                       char *sV_Total_date,char *sV_LoginAccept,char *sV_Op_time,char *sV_YYYYMM,char *sErrorMess,
                       char *sTrue_flag,char *sVCustinfoType,char *sV_Agreement_Type,char* vOrgId)
{
    int i=0,j=0,k=0,p=0,q=0,begin=0,end=0,tmplen=0,num=0;
    char vTmp_List[1000];
	int Vret = 0;
	tCustDoc  custDoc;    
    EXEC SQL BEGIN DECLARE SECTION;
        char    sIn_Cust_id[10+1];                   /* �ͻ���ʶ         */
        char    sIn_BelongCode[7+1];                 /* ��������         */
        char    sIn_Cust_name[60+1];                 /* �ͻ�����         */
        char    sIn_Cust_passwd[8+1];                /* �ͻ�����         */
        char    sIn_Cust_status[2+1];                /* �ͻ�״̬         */
        char    sIn_Owner_grade[2+1];                /* �ͻ�����         */
        char    sIn_Owner_type[2+1];                 /* �ͻ����         */
        char    sIn_Cust_address[60+1];              /* �ͻ���ַ         */
        
        char    sIn_Id_type[2+1];                    /* �ͻ�֤������     */
        char    sIn_Id_iccid[20+1];                  /* ֤������         */
        char    sIn_Id_address[60+1];                /* ֤����ַ         */
        char    sIn_Id_validdate[17+1];              /* ֤����Ч�� yyyymmdd */
        
        char    sIn_Contact_person[20+1];            /* ��ϵ������       */
        char    sIn_Contact_phone[20+1];             /* ��ϵ�˵绰       */
        char    sIn_Contact_address[100+1];           /* ��ϵ�˵�ַ       */
        char    sIn_Contact_post[6+1];               /* ��ϵ����������   */
        char    sIn_Contact_mailaddress[60+1];       /* ��ϵ��ͨѶ��ַ   */
        char    sIn_Contact_fax[30+1];               /* ��ϵ�˴���       */ 
        char    sIn_Contact_emaill[30+1];            /* ��ϵ�˵�������   */ 
        
        char    sIn_Org_code[9+1];                   /* ��������         */ 
        char    sIn_Parent_id[10+1];                 /* �ϼ��ͻ���ʶ     */ 
        char    sIn_Cust_sex[2+1];                   /* �Ա�             */ 
        char    sIn_Birthday[17+1];                  /* ��������   YYYYMMDD  */ 
        char    sIn_Profession_id[4+1];              /* ְҵ����         */ 
        char    sIn_Edu_level[2+1];                  /* ѧ��             */ 
        char    sIn_Cust_love[20+1];                 /* �ͻ�����         */ 
        char    sIn_Cust_habit[20+1];                /* �ͻ�ϰ��         */ 
        char    sIn_Orgcharacter_id[2+1];            /* ��λ����         */ 
        
        char    sIn_Business_licencetype[2+1];       /* Ӫҵִ������     */
        char    sIn_Business_licenceno[60+1];        /* Ӫҵִ�պ���     */
        char    sIn_Licence_validdate[20+1];         /* Ӫҵִ����Ч�� YYYYMMDD  */
        char    sIn_Org_legalrep[20+1];              /* ���˴���         */
        char    sIn_Vpmngrp_info[60+1];              /* Ⱥ����Ϣ         */
        
        char    sIn_Op_code[4+1];                   /* ��������          */
        char    sIn_Work_no[6+1];                   /* ��������          */
        char    sIn_Sys_note[60+1];                 /* ������ע          */
        char    sIn_Op_note[60+1];                  /* ������ע          */
        char    sIn_Ip_Addr[15+1];                  /* IP_ADDR           */
        char    sIn_Group_index[10+1];              /* ԭ���ź�          */
            
        char    sV_Region_code[2+1];                /* ��������          */
        char    sV_District_code[2+1];              /* ���ش���          */
        char    sV_Town_code[3+1];                  /* �������          */
        char    sql_str[1024];
        
        
        char    sIn_true_flag[1+1];                 /*�Ƿ�Ϊʵ��  Y or N*/
        int     sV_row=0;                           /* ��¼��            */
    	
    	char  sIn_Custinfo_Type[2+1];
    	char  sIn_Agreement_Type[1+1];
	      char vGroupId[10+1];    
	      char vChgFlag[1+1];
	      char vCustFlag[1+1];    	
    EXEC SQL END DECLARE SECTION;

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "==========begin pubOpenCustConfirm =============="); 
    #endif
	
    /* ���Ի����� */
    init(sIn_Agreement_Type);
    init(sIn_Custinfo_Type);
    init(vTmp_List);
    
    init(sIn_Cust_id);
    init(sIn_BelongCode);
    init(sIn_Cust_name);
    init(sIn_Cust_passwd);
    init(sIn_Cust_status);
    init(sIn_Owner_grade);
    init(sIn_Owner_type);
    init(sIn_Cust_address);
    
    init(sIn_Id_type);
    init(sIn_Id_iccid);
    init(sIn_Id_address);
    init(sIn_Id_validdate);
    
    init(sIn_Contact_person);
    init(sIn_Contact_phone);
    init(sIn_Contact_address);
    init(sIn_Contact_post);
    init(sIn_Contact_mailaddress);
    init(sIn_Contact_fax);
    init(sIn_Contact_emaill);
    
    init(sIn_Org_code);
    init(sIn_Parent_id);
    init(sIn_Cust_sex);
    init(sIn_Birthday);
    init(sIn_Profession_id);
    init(sIn_Edu_level);
    init(sIn_Cust_love);
    init(sIn_Cust_habit);
    init(sIn_Orgcharacter_id);
    
    init(sIn_Business_licencetype);
    init(sIn_Business_licenceno);
    init(sIn_Licence_validdate);
    init(sIn_Org_legalrep);
    init(sIn_Vpmngrp_info);
    
    init(sIn_Op_code);
    init(sIn_Work_no);
    init(sIn_Sys_note);
    init(sIn_Op_note);
    init(sIn_Ip_Addr);
    init(sIn_Group_index);

    init(sV_Region_code);
    init(sV_District_code);
    init(sV_Town_code);
    init(sql_str);
    
    init(sIn_true_flag);
    init(vGroupId);
    init(vChgFlag);
    init(vCustFlag);
	memset(&custDoc,0,sizeof(custDoc));    
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "==========  ��ʼȡ��������� =============="); 
    #endif
	
    /* ȡ��������� */
    memset(vTmp_List,0x00,sizeof(vTmp_List));    
    memcpy(vTmp_List,sIn_CustMsg,sizeof(vTmp_List)-1);
    Trim(vTmp_List);
    if(strlen(vTmp_List)!=0)
    {        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
 
                if(k==1) strcpy(sIn_Cust_id,substr(vTmp_List,begin,num));
                if(k==2) strcpy(vGroupId,substr(vTmp_List,begin,num));    
                if(k==3) strcpy(sIn_Cust_name,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sIn_Cust_passwd,substr(vTmp_List,begin,num));
                if(k==5) strcpy(sIn_Cust_status,substr(vTmp_List,begin,num));
                if(k==6) strcpy(sIn_Owner_grade,substr(vTmp_List,begin,num)); /* �ͻ�����û������,Ŀǰ����"00" */
                if(k==7) strcpy(sIn_Owner_type,substr(vTmp_List,begin,num));
                if(k==8) strcpy(sIn_Cust_address,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        Trim(sIn_Cust_passwd);
        if(strlen(sIn_Cust_passwd)==0)
            strcpy(sIn_Cust_passwd," ");
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_id  =[%s]",sIn_Cust_id);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_BelongCode  =[%s]",sIn_BelongCode);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_name=[%s]",sIn_Cust_name);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_passwd=[%s]",sIn_Cust_passwd);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_status=[%s]",sIn_Cust_status);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Owner_grade=[%s]",sIn_Owner_grade);            
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Owner_type=[%s]",sIn_Owner_type);            
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_address=[%s]",sIn_Cust_address);            
        #endif
    }
    /*add by liupenga ��֯��������20090306*/
    EXEC SQL  select boss_org_code into :sIn_BelongCode 
    	from dchngroupmsg where group_id=:vGroupId;
    if(SQLCODE!=OK)
    {
        strcpy(sErrorMess,"��ѯdchngroupmsg�����!");
 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
		pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110090",sErrorMess);
        return 110090 ;    	
    }

    memset(vTmp_List,0x00,sizeof(vTmp_List));    
    memcpy(vTmp_List,sIn_CustId,sizeof(vTmp_List)-1);
    Trim(vTmp_List);
    if(strlen(vTmp_List)!=0)
    {        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
 
                if(k==1) strcpy(sIn_Id_type,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sIn_Id_iccid,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sIn_Id_address,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sIn_Id_validdate,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        Trim(sIn_Id_validdate);
        if(strlen(sIn_Id_validdate)==0)
            strcpy(sIn_Id_validdate,"19491001");
            
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_type  =[%s]",sIn_Id_type);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_iccid  =[%s]",sIn_Id_iccid);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_address=[%s]",sIn_Id_address);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_validdate=[%s]",sIn_Id_validdate);
        #endif
    }

    memset(vTmp_List,0x00,sizeof(vTmp_List));    
    memcpy(vTmp_List,sIn_CustContact,sizeof(vTmp_List)-1);
    Trim(vTmp_List);
    if(strlen(vTmp_List)!=0)
    {        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
 
                if(k==1) strcpy(sIn_Contact_person,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sIn_Contact_phone,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sIn_Contact_address,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sIn_Contact_post,substr(vTmp_List,begin,num));
                if(k==5) strcpy(sIn_Contact_mailaddress,substr(vTmp_List,begin,num));
                if(k==6) strcpy(sIn_Contact_fax,substr(vTmp_List,begin,num));
                if(k==7) strcpy(sIn_Contact_emaill,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_person  =[%s]",sIn_Contact_person);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_phone  =[%s]",sIn_Contact_phone);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_address=[%s]",sIn_Contact_address);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_post=[%s]",sIn_Contact_post);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_mailaddress=[%s]",sIn_Contact_mailaddress);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_fax=[%s]",sIn_Contact_fax);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_emaill=[%s]",sIn_Contact_emaill);
        #endif
        Trim(sIn_Contact_post);
        Trim(sIn_Contact_fax);
        Trim(sIn_Contact_emaill);
        if(strlen(sIn_Contact_post)==0)
            strcpy(sIn_Contact_post," ");
        if(strlen(sIn_Contact_fax)==0)
            strcpy(sIn_Contact_fax," ");
        if(strlen(sIn_Contact_emaill)==0)
            strcpy(sIn_Contact_emaill," ");
    }
    
    memset(vTmp_List,0x00,sizeof(vTmp_List));    
    memcpy(vTmp_List,sIn_CustAttr,sizeof(vTmp_List)-1);
    Trim(vTmp_List);
    if(strlen(vTmp_List)!=0)
    {        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
 
                if(k==1) strcpy(sIn_Org_code,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sIn_Parent_id,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sIn_Cust_sex,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sIn_Birthday,substr(vTmp_List,begin,num));
                if(k==5) strcpy(sIn_Profession_id,substr(vTmp_List,begin,num));
                if(k==6) strcpy(sIn_Edu_level,substr(vTmp_List,begin,num));
                if(k==7) strcpy(sIn_Cust_love,substr(vTmp_List,begin,num));
                if(k==8) strcpy(sIn_Cust_habit,substr(vTmp_List,begin,num));
                if(k==9) strcpy(sIn_Orgcharacter_id,substr(vTmp_List,begin,num));
                if(k==10) strcpy(sIn_Business_licencetype,substr(vTmp_List,begin,num));
                if(k==11) strcpy(sIn_Business_licenceno,substr(vTmp_List,begin,num)); 
                if(k==12) strcpy(sIn_Licence_validdate,substr(vTmp_List,begin,num));
                if(k==13) strcpy(sIn_Org_legalrep,substr(vTmp_List,begin,num));
                if(k==14) strcpy(sIn_Vpmngrp_info,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        Trim(sIn_Birthday);
        Trim(sIn_Cust_love);
        Trim(sIn_Cust_habit);
        Trim(sIn_Edu_level);
        Trim(sIn_Cust_sex);
        Trim(sIn_Profession_id);
        
        if(strlen(sIn_Birthday)==0)
            strcpy(sIn_Birthday,"19491001");
        if(strlen(sIn_Cust_love)==0)
            strcpy(sIn_Cust_love," ");
        if(strlen(sIn_Cust_habit)==0)
            strcpy(sIn_Cust_habit," ");
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Org_code  =[%s]",sIn_Org_code);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Parent_id  =[%s]",sIn_Parent_id);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_sex=[%s]",sIn_Cust_sex);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Birthday=[%s]",sIn_Birthday);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Profession_id=[%s]",sIn_Profession_id);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Edu_level=[%s]",sIn_Edu_level);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_love=[%s]",sIn_Cust_love);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_habit=[%s]",sIn_Cust_habit);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Orgcharacter_id=[%s]",sIn_Orgcharacter_id);
        #endif
    }
    
    memset(vTmp_List,0x00,sizeof(vTmp_List));    
    memcpy(vTmp_List,sIn_CustAdd,sizeof(vTmp_List)-1);
    Trim(vTmp_List);
    if(strlen(vTmp_List)!=0)
    {        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
 
                if(k==1) strcpy(sIn_Op_code,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sIn_Work_no,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sIn_Sys_note,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sIn_Op_note,substr(vTmp_List,begin,num));
                if(k==5) strcpy(sIn_Ip_Addr,substr(vTmp_List,begin,num));
                if(k==6) strcpy(sIn_Group_index,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Op_code  =[%s]",sIn_Op_code);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Work_no  =[%s]",sIn_Work_no);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Sys_note=[%s]",sIn_Sys_note);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Op_note=[%s]",sIn_Op_note);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Ip_Addr=[%s]",sIn_Ip_Addr);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Group_index=[%s]",sIn_Group_index);
        #endif
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_id= [%s]  sIn_BelongCode=[%s]",sIn_Cust_id,sIn_BelongCode);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_name= [%s]  sIn_Cust_passwd=[%s]",sIn_Cust_name,sIn_Cust_passwd);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_status= [%s]  sIn_Owner_grade=[%s]",sIn_Cust_status,sIn_Owner_grade);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Owner_type= [%s]  sIn_Cust_address=[%s]",sIn_Owner_type,sIn_Cust_address);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Owner_type= [%s]  sIn_Cust_address=[%s]",sIn_Owner_type,sIn_Cust_address);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_type= [%s]  sIn_Id_iccid=[%s]",sIn_Id_type,sIn_Id_iccid);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Id_address= [%s]  sIn_Id_validdate=[%s]",sIn_Id_address,sIn_Id_validdate);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_person= [%s]  sIn_Contact_phone=[%s]",sIn_Contact_person,sIn_Contact_phone);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_address= [%s]  sIn_Contact_post=[%s]",sIn_Contact_address,sIn_Contact_post);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_mailaddress= [%s]  sIn_Contact_fax=[%s]",sIn_Contact_mailaddress,sIn_Contact_fax);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Contact_emaill= [%s]  sIn_Org_code=[%s]",sIn_Contact_emaill,sIn_Org_code);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Parent_id= [%s]  sIn_Cust_sex=[%s]",sIn_Parent_id,sIn_Cust_sex);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Birthday= [%s]  sIn_Profession_id=[%s]",sIn_Birthday,sIn_Profession_id);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Edu_level= [%s]  sIn_Cust_love=[%s]",sIn_Edu_level,sIn_Cust_love);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Cust_habit= [%s]  sIn_Orgcharacter_id=[%s]",sIn_Cust_habit,sIn_Orgcharacter_id);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Business_licencetype= [%s]  sIn_Business_licenceno=[%s]",sIn_Business_licencetype,sIn_Business_licenceno);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Licence_validdate= [%s]  sIn_Org_legalrep=[%s]",sIn_Licence_validdate,sIn_Org_legalrep);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Vpmngrp_info= [%s]  sIn_Op_code=[%s]",sIn_Vpmngrp_info,sIn_Op_code);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Work_no= [%s]  sIn_Sys_note=[%s]",sIn_Work_no,sIn_Sys_note);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Op_note= [%s]  sIn_Ip_Addr=[%s]  ",sIn_Op_note,sIn_Ip_Addr);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Group_index= [%s]",sIn_Group_index);
    #endif

    /* ���  �������� */
    EXEC SQL SELECT substr(:sIn_BelongCode,1,2),substr(:sIn_BelongCode,3,2),substr(:sIn_BelongCode,5,3)
             INTO :sV_Region_code,:sV_District_code,:sV_Town_code FROM dual;
    if(SQLCODE!=OK)
    {
        strcpy(sErrorMess,"��ֹ����������!");
 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
		pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110002",sErrorMess);
        
        return 110002;	
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]",\
        sV_Region_code,sV_District_code,sV_Town_code); 
    #endif
    
    Trim(sIn_Id_type);
pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "[%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s]",
                   sIn_Cust_id,sV_Region_code,sV_District_code,sV_Town_code,
                   sIn_Cust_name,sIn_Cust_passwd,sIn_Cust_status,sV_Op_time,
                   sIn_Owner_grade,sIn_Owner_type,sIn_Cust_address,sIn_Id_type,
                   sIn_Id_iccid,sIn_Id_address,sIn_Id_validdate,sIn_Contact_person,
                   sIn_Contact_phone,sIn_Contact_address,sIn_Contact_post,sIn_Contact_mailaddress,
                   sIn_Contact_fax,sIn_Contact_emaill,sIn_Org_code,sV_Op_time,
                   sIn_Parent_id,sV_LoginAccept,sV_Op_time,sIn_Work_no,
                   sIn_Op_code,sV_Total_date);
/*    Trim(sIn_Cust_id);Trim(sV_Region_code);Trim(sV_District_code);Trim(sV_Town_code);Trim(sIn_Cust_name);
    Trim(sIn_Cust_passwd);Trim(sIn_Cust_status);Trim(sV_Op_time);Trim(sIn_Owner_grade);Trim(sIn_Owner_type);
    Trim(sIn_Cust_address);Trim(sIn_Id_type);Trim(sIn_Id_iccid);Trim(sIn_Id_address);Trim(sIn_Id_validdate);
    Trim(sIn_Contact_person);Trim(sIn_Contact_phone);Trim(sIn_Contact_address);Trim(sIn_Contact_post);
    Trim(sIn_Contact_mailaddress);Trim(sIn_Contact_fax);Trim(sIn_Contact_emaill);Trim(sIn_Org_code);
    Trim(sV_Op_time);Trim(sIn_Parent_id);Trim(sV_LoginAccept);Trim(sV_Op_time);Trim(sIn_Work_no);
    Trim(sIn_Op_code);Trim(sV_Total_date);
    if(strlen(sIn_Contact_post)==0)
        strcpy(sIn_Contact_post," ");
    if(strlen(sIn_Contact_fax)==0)
        strcpy(sIn_Contact_fax," ");
    if(strlen(sIn_Contact_emaill)==0)
        strcpy(sIn_Contact_emaill," ");
*/
    /* ���� dCustDocHis �� */
    Trim(sIn_Cust_name);
    Trim(sIn_Cust_address);
    Trim(sIn_Id_iccid);
    if(strlen(sIn_Cust_name)==0)
        strcpy(sIn_Cust_name," ");
    if(strlen(sIn_Cust_address)==0)
        strcpy(sIn_Cust_address," ");
    if(strlen(sIn_Id_iccid)==0)
        strcpy(sIn_Id_iccid," ");
   

    strncpy(sIn_true_flag,sTrue_flag,1);
    strncpy(sIn_Custinfo_Type,sVCustinfoType,2);
    strncpy(sIn_Agreement_Type,sV_Agreement_Type,1);
    
	Trim(sIn_Agreement_Type);
    Trim(sIn_Custinfo_Type); 
    Trim(sIn_true_flag); 
    /* ���� dCustDoc �� */

	 
	    custDoc.vCust_Id=    atol(sIn_Cust_id);
        strncpy(custDoc.vRegion_Code		,sV_Region_code,2);
        strncpy(custDoc.vDistrict_Code	,sV_District_code,2);
        strncpy(custDoc.vTown_Code		,sV_Town_code,3);
        strncpy(custDoc.vOrg_Code		,sIn_Org_code,9);
        strncpy(custDoc.vGroup_Id		,vGroupId,10);
        strncpy(custDoc.vOrg_Id			,vOrgId,10);
        strncpy(custDoc.vCust_Name		,sIn_Cust_name,60);
        strncpy(custDoc.vCust_Passwd		,sIn_Cust_passwd,8);
        strncpy(custDoc.vCust_Status		,sIn_Cust_status,2);
        strncpy(custDoc.vRun_Time		,sV_Op_time,17);
        strncpy(custDoc.vCreate_Time		,sV_Op_time,17);
        strncpy(custDoc.vClose_Time		,"20501230 00:00:00",17);
        strncpy(custDoc.vOwner_Grade		,sIn_Owner_grade,2);
        strncpy(custDoc.vOwner_Type		,sIn_Owner_type,2);
        strncpy(custDoc.vCust_Addr		,sIn_Cust_address,60);
        strncpy(custDoc.vId_Addr			,sIn_Id_address,60);
        strncpy(custDoc.vContact_Addr			,sIn_Contact_address,100);
        strncpy(custDoc.vContact_MailAddr	,sIn_Contact_mailaddress,60);       
        strncpy(custDoc.vId_Type			,sIn_Id_type,1);
        strncpy(custDoc.vId_Iccid		,sIn_Id_iccid,20);
        strncpy(custDoc.vId_ValidDate	,sIn_Id_validdate,17);
        strncpy(custDoc.vContact_Person,sIn_Contact_person,20);
        strncpy(custDoc.vContact_Phone,sIn_Contact_phone,20);
        strncpy(custDoc.vContact_Post,sIn_Contact_post,6);
        strncpy(custDoc.vContact_Fax,sIn_Contact_fax,30);
        strncpy(custDoc.vContact_Email,sIn_Contact_emaill,30);
        strncpy(custDoc.vParent_Id		,sIn_Parent_id,14);
        strncpy(custDoc.vCreate_Note		,sIn_Op_note,60);
        strncpy(custDoc.vOp_Time			,sV_Op_time,17);
        strncpy(custDoc.vLogin_No		,sIn_Work_no,6);
        strncpy(custDoc.vOp_Code			,sIn_Op_code,4);
        custDoc.vLogin_Accept=  atol(sV_LoginAccept);
        custDoc.vTotal_Date=    atoi(sV_Total_date);    
        strncpy(custDoc.vTrue_Flag,sIn_true_flag,1);
        strncpy(custDoc.vInfo_Type,sIn_Custinfo_Type,2);
        strncpy(custDoc.vAgreement_Type,sIn_Agreement_Type,1);
            
        Vret = 0; 
        Vret = createCustDoc_ng(send_type,send_id,&custDoc,sErrorMess);
        if(Vret != 0)
        {
			strcpy(sErrorMess,"��������!");
        	pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "%s:%s-- [%d][%s]",__FILE__,sErrorMess,SQLCODE,SQLERRMSG);
        	return 110092 ;
        }   
        /*������ edit by cencm at 20100414--------begin-------------*/
       /* if(strncmp(sIn_true_flag,"Y",1)==0)
        {
        	EXEC SQL insert into dchncusttrue(ID_NO,CUST_ID,GROUP_ID,OPEN_TIME,
									TOTAL_DATE,OP_TIME,BELONG_CODE,OP_CODE)
			values('',to_number(:sIn_Cust_id),'','',
			to_number(:sV_Total_date),to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),'',:sIn_Op_code);
			if(SQLCODE!=0)
			{
				strcpy(sErrorMess,"�����dchncusttrue����!");
		 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
				pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110005",sErrorMess);
           		return 110005 ;
			}
        }*/
        /*������ edit by cencm at 20100414--------end-------------*/
    
    
    	 
         if (strcmp(sIn_Owner_type,"01")==0){
        /* ���� dCustDocInAdd �� */
        init(sql_str);
        sprintf(sql_str,"insert into dCustDocInAdd(cust_id,cust_sex,birthday,profession_id,edu_level,cust_love,cust_habit) "
                " values(to_number(:v1),:v2,to_date(:v3,'YYYYMMDD'),:v4,:v5,:v6,:v7)");
        Trim(sIn_Profession_id);
        Trim(sIn_Cust_id);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "[%s][%s][%s][%s][%s][%s][%s]",sIn_Cust_id,sIn_Cust_sex,sIn_Birthday,sIn_Profession_id,sIn_Edu_level,sIn_Cust_love,sIn_Cust_habit);
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "insert dCustDocInAdd' sql_str = [%s]",sql_str);

        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt using :sIn_Cust_id,:sIn_Cust_sex,:sIn_Birthday,:sIn_Profession_id,:sIn_Edu_level,
        				:sIn_Cust_love,:sIn_Cust_habit;
        if (SQLCODE !=0)
        {
            strcpy(sErrorMess,"����dCustDocInAdd�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110006",sErrorMess);
            return 110006 ;
        }
pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "[%s][%s][%s][%s][%s][%s][%s]",sIn_Cust_id,sIn_Cust_sex,sIn_Birthday,sIn_Profession_id,sIn_Edu_level,sIn_Cust_love,sIn_Cust_habit);
        
        /* ���� dCustDocInAddHis �� */
       /*������ edit by cencm at 20100414--------begin-------------*/  
       /* 
    	EXEC SQL insert into dCustDocInAddHis(CUST_ID,CUST_SEX,BIRTHDAY,PROFESSION_ID,
    	            EDU_LEVEL,CUST_LOVE,CUST_HABIT,
    	            UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE)
    	         select CUST_ID,CUST_SEX,BIRTHDAY,PROFESSION_ID,
    	            EDU_LEVEL,CUST_LOVE,CUST_HABIT,
    	            to_number(:sV_LoginAccept),to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
                    :sIn_Work_no,'a',:sIn_Op_code
                 from dCustDocInAdd 
                 where CUST_ID = to_number(:sIn_Cust_id);
    	if (SQLCODE !=0)
        {
            strcpy(sErrorMess,"����dCustDocInAddHis�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110007",sErrorMess);
            return 110007 ;
        }*/
        /*������ edit by cencm at 20100414--------end-------------*/  
        
        
    }
    else  if (strcmp(sIn_Owner_type,"03")==0){
        /* ���� dGrpMid �� */
        sV_row=0;
        EXEC SQL select count(*) into :sV_row from dGrpMid where group_index=:sIn_Group_index;
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"��ѯdGrpMid�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110017",sErrorMess);
            return 110017 ;
        }
        if(sV_row>0 && strcmp(sIn_Group_index,"0")!=0)
        {
            sprintf(sErrorMess,"ԭ���ź��Ѿ����ڣ�����������[%s]!",sIn_Group_index);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "%s:%s-- [%d][%s]",__FILE__,sErrorMess,SQLCODE,SQLERRMSG);
            return 110018 ;
        }
        sV_row=0;
        EXEC SQL select count(*) into :sV_row from dGrpDoc where group_index=:sIn_Group_index;
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"��ѯdGrpDoc�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110017",sErrorMess);
            return 110017 ;
        }
        if(sV_row>0 && strcmp(sIn_Group_index,"0")!=0)
        {
            sprintf(sErrorMess,"ԭ���ź��Ѿ����ڣ�����������[%s]!",sIn_Group_index);
            pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "%s:%s-- [%d][%s]",__FILE__,sErrorMess,SQLCODE,SQLERRMSG);
            return 110018 ;
        }
        
        if(strcmp(sIn_Group_index,"0")==0)
            strcpy(sIn_Group_index,"");
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "sIn_Group_index=[%s]",sIn_Group_index);
        
        EXEC SQL insert into dGrpMid(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate);
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpMid�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110008",sErrorMess);
            return 110008 ;
        }

        /* ���� dGrpMidHis �� */
        EXEC SQL insert into dGrpMidHis(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time,
                     UPDATE_LOGIN,UPDATE_CODE,UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate,
                     :sIn_Work_no,:sIn_Op_code,to_number(to_char(sysdate,'yyyymmdd')),sysdate,to_number(:sV_LoginAccept));
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpMidHis�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110009",sErrorMess);
            return 110009 ;
        }
        EXEC SQL insert into dGrpDoc(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate);
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpDoc�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110008",sErrorMess);
            return 110008 ;
        }

        /* ���� dGrpDocHis �� */
        EXEC SQL insert into dGrpDocHis(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time,
                     UPDATE_LOGIN,UPDATE_CODE,UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate,
                     :sIn_Work_no,:sIn_Op_code,to_number(to_char(sysdate,'yyyymmdd')),sysdate,to_number(:sV_LoginAccept));
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpDocHis�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110009",sErrorMess);
            return 110009 ;
        }
    }
    else if (strcmp(sIn_Owner_type,"02")==0){
        /* ���� dGrpMid �� */
        EXEC SQL insert into dGrpMid(CUST_ID,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time)
                 values(to_number(:sIn_Cust_id),'00',:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate);
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpMid�����!");
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110015",sErrorMess);
            return 110015 ;
        }

        /* ���� dGrpMidHis �� */
        EXEC SQL insert into dGrpMidHis(CUST_ID,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time,
                     UPDATE_LOGIN,UPDATE_CODE,UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT)
                 values(to_number(:sIn_Cust_id),'00',:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate,
                     :sIn_Work_no,:sIn_Op_code,to_number(to_char(sysdate,'yyyymmdd')),sysdate,to_number(:sV_LoginAccept));
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpMidHis�����!");            
	 	    PUBLOG_DBDEBUG("pubOpenCustConfirm");
			pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110016",sErrorMess);            
            return 110016 ;
        }
    }
    else{
        strcpy(sErrorMess,"����Ŀͻ����ͳ���!");
        return 110012 ;	
    }
        
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenCustConfirm", "", "end pubOpenCustConfirm"); 
    #endif


        return 0;
}

/**
  * Auth: PeiJH
  * Name: pubOpenAcctRes
  * Func: �û������޸���Դ��Ϣ
  * Date: 2003/10/14
  * Stat: Test Pass
  */
int pubOpenAcctRes(char *InId_No,       char *InPhone_No,  char *InSim_No,
                   char *InBelong_Code, char *InLogin_No,  char InLogin_Accept[23],
                   char *InOp_Code,     char *InTotal_Date,char *InSysDate,char *InGroupNo)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char TmpSqlStr[1024];
       char pId_No[22+1];
       char pPhone_No[15+1];
       char pSim_No[20+1];
       char vImsi_No[20+1];
       char pBelong_Code[9+1];
       char pLogin_No[6+1];
       char pLogin_Accept[23]; 
       char pOp_Code[4+1];
       char pTotal_Date[8+1];
       char vOrg_Code[9+1];
       char pSysDate[20+1];
       char pGroupNo[10+1];
       char vGroupId[10+1];
       char vYearMonth[6+1];
       char oldSimGroupId[10+1];
       char oldSimLoginNo[6+1];
       
       char sMax_IdCode[22+1];  /* wPhoneImsi ��ˮ */
       int  flag=0;
       char vbillCode[8+1];
       char vgoodType[4+1];
       double ilimitFee=0;
       double tlimitFee=0;
       int     sV_count=0;                   /*�Ƿ������������ж�*/   
       /*NG*/
       int ret =0;
       TdCustSim	sTdCustSim;
       char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
       char mysql_ng[1024];
       TwPhoneImsi	sTwPhoneImsi;
       char vImsi_No_Ng[20+1];
    EXEC SQL END   DECLARE SECTION;

    /* �ֲ�������ʼ�� */
    init(TmpSqlStr);   init(pPhone_No);    init(pSim_No);
    init(vImsi_No);    init(pBelong_Code); init(pLogin_No);
    init(pTotal_Date); init(pSysDate);     init(vOrg_Code);  init(pGroupNo);  
    init(pOp_Code);    init(pId_No);	    init(vGroupId);
    init(vYearMonth);  init(oldSimGroupId); init(oldSimLoginNo);
	init(vbillCode);
	init(vgoodType); init(mysql_ng); init(vImsi_No_Ng);
	memset(&sTdCustSim,0,sizeof(sTdCustSim));
	memset(v_parameter_array,0,sizeof(v_parameter_array));
	memset(&sTwPhoneImsi,0,sizeof(sTwPhoneImsi));
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "-------- �û������޸���Դ��Ϣ pubOpenAcctRes() BEGIN --------");
    
    memcpy(pLogin_Accept,         InLogin_Accept   ,    22);    
    memcpy(pId_No,          InId_No,         22);
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pSim_No,         InSim_No,        20);
    memcpy(pBelong_Code,    InBelong_Code,   9);
    memcpy(pLogin_No,       InLogin_No,      6); 
    memcpy(pOp_Code,        InOp_Code,       4);
    memcpy(pTotal_Date,     InTotal_Date,    8);
    memcpy(pSysDate,        InSysDate,       20);
    memcpy(pGroupNo,        InGroupNo,       10);

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Phone_No       = [%s]",   pPhone_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Sim_No         = [%s]",   pSim_No         );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Belong_code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "pOp_Code       = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "pTotal_Date    = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "pSysDate       = [%s]",   pSysDate        );
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "pGroupNo       = [%s]",   pGroupNo        );
#endif

    EXEC SQL SELECT Org_Code ,org_id,to_char(sysdate, 'yyyymm') 
    INTO :vOrg_Code, :vGroupId, :vYearMonth /*group_id ->org_id edit by liuweib20091023*/
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
        if(SQLCODE!=0){
	 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ȡ������Ϣ����!");
            EXEC SQL ROLLBACK;
            return -1;
        }
    init(vGroupId);
    strcpy(vGroupId,pGroupNo);
     pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "vGroupId       = [%s]",   vGroupId );
    EXEC SQL SELECT Imsi_No ,group_Id, nvl(login_no, ' ') INTO :vImsi_No,:oldSimGroupId, :oldSimLoginNo 
             FROM   dSimRes
             WHERE  Sim_No=:pSim_No;
        if(SQLCODE!=0){
	 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","��ѯdSimResȡImsi_No����!");
            
            EXEC SQL ROLLBACK;
            return -1;
        }                                
        /* lixg add begin :204-12-17*/
        memset(TmpSqlStr, 0, sizeof(TmpSqlStr)); Trim(vGroupId); Trim(pSysDate);
        sprintf(TmpSqlStr,"INSERT INTO wCustResOpr%s "
                                "( LOGIN_ACCEPT, TOTAL_DATE, NO_TYPE, BEGIN_NO, "
                                " END_NO, PASSWORD, OTHER_ATTR, CHOICE_FEE, LIMIT_PREPAY, "
                                " BEGIN_TIME, OLD_GROUP, OLD_LOGIN, NEW_GROUP, NEW_LOGIN, "
                                " RESOURCE_CODE, OP_TIME, OP_CODE, LOGIN_NO, ORG_GROUP)"
                                " select to_number(:v1), to_number(:v2), no_type, phone_no,"
                                " phone_no, password, other_attr, choice_fee, LIMIT_PREPAY,"
                                " BEGIN_TIME, group_id, login_no, :v3, :v4,"
                                " '1', to_char(to_date(:v5, 'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'), :v6, :v7, :v8 "
                                " from dCustRes where phone_no = :v9", vYearMonth);
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr;
        EXEC SQL EXECUTE ins_stmt using :pLogin_Accept,:pTotal_Date,
                            :vGroupId,:pLogin_No,:pSysDate,:pOp_Code,:pLogin_No,:vGroupId,:pPhone_No;
        if (SQLCODE != 0) {
#ifdef _DEBUG_
          pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "TmpSqlStr =[%s][%d]", TmpSqlStr, SQLCODE);
          pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "v1 = [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]", pLogin_Accept,pTotal_Date,vGroupId,pLogin_No,pSysDate,pOp_Code,pLogin_No,vGroupId,pPhone_No);
#endif
	 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_,"pubOpenAcctRes","-33","�����wCustResOpr����!"); 
            EXEC SQL ROLLBACK;
            return -33;
        }
        
        /* ���� dCustResHis �� */
    	/*EXEC SQL insert into dCustResHis(PHONE_NO , NO_TYPE , PASSWORD , OTHER_ATTR ,
                  RESOURCE_CODE , CHOICE_FEE , REGION_CODE , DISTRICT_CODE , 
                  TOWN_CODE  , BEGIN_TIME , OP_TIME , LOGIN_NO , ORG_CODE , 
                  LOGIN_ACCEPT , TOTAL_DATE ,ASSIGN_ACCEPT , LIMIT_PREPAY ,
                  UPDATE_ACCEPT , UPDATE_TIME , UPDATE_LOGIN , UPDATE_TYPE ,
                  UPDATE_CODE , UPDATE_DATE,group_id)
              select PHONE_NO , NO_TYPE , PASSWORD , OTHER_ATTR ,
                  RESOURCE_CODE , CHOICE_FEE , REGION_CODE , DISTRICT_CODE , 
                  TOWN_CODE  , BEGIN_TIME , OP_TIME , LOGIN_NO , ORG_CODE , 
                  LOGIN_ACCEPT , TOTAL_DATE ,ASSIGN_ACCEPT , LIMIT_PREPAY ,
                  :pLogin_Accept , sysdate , :pLogin_No , 'U' ,
                  :pOp_Code , :pTotal_Date,group_id
              from dCustRes
              WHERE     Phone_No          = :pPhone_No
              AND       (Resource_Code= '0' or Resource_Code= 'h');
    	if(SQLCODE!=0){
        	pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "����dCustResHis��ʧ��!");
        	pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        	return -11;
    	}*/
    
    EXEC SQL UPDATE dCustRes
             SET Resource_Code= '1',          
                 Login_No     = :pLogin_No,
                 Login_Accept = to_number(:pLogin_Accept), 
                 Op_Time      =  TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                 Total_Date   =  TO_NUMBER(:pTotal_Date),     
                 Org_Code     = :vOrg_Code,
                 Region_Code  = DECODE(SUBSTR(:pBelong_Code,1,2),
                               '99', SUBSTR(:vOrg_Code,1,2),
                                SUBSTR(:pBelong_Code,1,2)),
                 District_Code= DECODE(SUBSTR(:pBelong_Code,3,2),
                               '99', SUBSTR(:vOrg_Code,3,2),
                                SUBSTR(:pBelong_Code,3,2)),
                 Town_Code    = DECODE(SUBSTR(:pBelong_Code,5,3),
                               '999', SUBSTR(:vOrg_Code,5,3),
                                SUBSTR(:pBelong_Code,5,3)),
                 group_id = :vGroupId
             WHERE Phone_No   =:pPhone_No;
/** ������������ **
               AND Resource_Code='9'; 
**/
        if(SQLROWS!=1){
	 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","���º�����Դ�����!");            
            EXEC SQL ROLLBACK;
            return -1;
        }

   /*  by zd 20090224 ��������ͬ������״̬ */
    
      EXEC SQL select  count(1)  into  :sV_count 
                 from dgoodphoneres
                 where phone_no = :pPhone_No;
          	if(SQLCODE!=0)
					{
					     
				 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
						 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","��ѯdgoodphonereshis��ʧ��!");            
					     
					        return -1;
					}
				  if(sV_count>0){
        	
        	   EXEC SQL insert into dgoodphonereshis(PHONE_NO, GOODSM_MODE, 
        	   BILL_CODE,GOOD_TYPE,REGION_CODE,PRE_DO_FLAG, BEGIN_TIME   ,
 						  END_TIME, BAK_FIELD, DISTRICT_CODE, RESOURCE_CODE 
 						  ,UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,
 						  UPDATE_CODE,UPDATE_DATE)
              select PHONE_NO, GOODSM_MODE, 
              BILL_CODE,GOOD_TYPE,REGION_CODE,PRE_DO_FLAG, BEGIN_TIME   ,
 						  END_TIME, BAK_FIELD, DISTRICT_CODE, RESOURCE_CODE ,
                  to_number(:pLogin_Accept) , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') , :pLogin_No , 'U' ,
                  :pOp_Code , :pTotal_Date
              from dgoodphoneres
              WHERE     Phone_No= :pPhone_No;
					    if(SQLCODE!=0){					        
					 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
							 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����dgoodphonereshis��ʧ��!");            

					        return -1;
					      }
					        	
        	
            EXEC  SQL UPDATE dgoodphoneres  
            SET   RESOURCE_CODE = '1'
            WHERE Phone_No = :pPhone_No;
         
					    if(SQLCODE!=0){					        
					 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
							 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�޸�dgoodphoneres��ʧ��!");            
					        
					        return -1;
					    }
        }     
    /* by zd end 20090124*/
    
	    
  /* by zd 20090720 ��¼dsimreshis���Ա�����ҵ�ԭ������Ϣ*/
    
        	   EXEC SQL insert into dsimreshis(SIM_NO,PHONENO_HEAD,IMSI_NO,PIN1,PUK1,PIN2,PUK2,ASK_NO,GET_MODE,
							SIM_STATUS,SIM_TYPE,REGION_CODE,DISTRICT_CODE,TOWN_CODE,OP_CODE,OP_TIME,LOGIN_NO,LOGIN_ACCEPT,
							TOTAL_DATE,PHONE_NO,UPDATE_LOGIN,UPDATE_ACCEPT,UPDATE_DATE,UPDATE_TIME,
							UPDATE_CODE,UPDATE_TYPE,GROUP_ID)
							select SIM_NO,PHONENO_HEAD,IMSI_NO,'','','','',0,0,
							SIM_STATUS,SIM_TYPE,REGION_CODE,DISTRICT_CODE,TOWN_CODE,OP_CODE,OP_TIME,LOGIN_NO,LOGIN_ACCEPT,
							TOTAL_DATE,PHONE_NO,:pLogin_No,to_number(:pLogin_Accept),TO_NUMBER(:pTotal_Date),TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
								:pOp_Code,'u',group_id
              from dSimRes
              WHERE Sim_No =:pSim_No;
					    if(SQLCODE!=0){					        
					 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
							 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����dsimreshis��ʧ��!");            

					        return -1;
					      }
					        	
    
    /*by zd end 20090720*/    

    
    EXEC SQL UPDATE dSimRes
                SET Sim_Status   ='1',
                    /* Phoneno_Head =substr(:pPhone_No,1,7), */
                    Login_No     =:pLogin_No,
                    Login_Accept =to_number(:pLogin_Accept),
                    Op_Time      = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    Total_Date   = TO_NUMBER(:pTotal_Date),
                    Region_Code  =DECODE(SUBSTR(:pBelong_Code,1,2),
                                 '99', SUBSTR(:vOrg_Code,1,2),
                                  SUBSTR(:pBelong_Code,1,2)),
                    District_Code=DECODE(SUBSTR(:pBelong_Code,3,2),
                                 '99', SUBSTR(:vOrg_Code,3,2),
                                  SUBSTR(:pBelong_Code,3,2)),
                    Town_Code    =DECODE(SUBSTR(:pBelong_Code,5,3),
                                 '999',SUBSTR(:vOrg_Code,5,3),
                                  SUBSTR(:pBelong_Code,5,3)),
                    group_id     =:vGroupId,
                    op_code		=:pOp_Code
              WHERE Sim_No =:pSim_No; 
/** ������������ **
                AND Sim_Status='9'; 
**/
        if(SQLROWS!=1){
	 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
			 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Դ�����!");            
            EXEC SQL ROLLBACK;
            return -1;
        }
        
        memset(TmpSqlStr, 0, sizeof(TmpSqlStr));
        sprintf(TmpSqlStr, "    INSERT INTO wSimResOpr%s "
                        "(LOGIN_ACCEPT, TOTAL_DATE, SIM_TYPE, BEGIN_NO, END_NO, OLD_GROUP, "
                        " NEW_GROUP, OLD_LOGIN,NEW_LOGIN,SIM_STATUS, OP_TIME, "
                        " OP_CODE, LOGIN_NO, ORG_GROUP)"
                        " select login_accept, total_date, sim_type, sim_no, sim_no, :v1,"
                        " group_id, :v2, login_no, sim_status, to_char(op_time,'yyyymmddhh24miss'), "
                        " op_code, login_no, group_id from dSimRes "
                        " where sim_no = :v3", vYearMonth);
#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "dynStmt=[%s] +++ SqlCode = [%d]+++", TmpSqlStr,SQLCODE);
#endif                        
        EXEC SQL PREPARE prepare1 From :TmpSqlStr;
        EXEC SQL EXECUTE prepare1 USING :oldSimGroupId,:oldSimLoginNo,:pSim_No;

    if (SQLCODE != 0)
    {             
	 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
			 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����wSimResOpr%s����!",vYearMonth);            
                
            EXEC SQL ROLLBACK;
            return -1;
    }

    EXEC SQL INSERT INTO dCustSimHis 
                        (Id_No,         Switch_No,      Sim_No,
                         Update_Login,  Update_Accept,  Update_Date,
                         Update_Time,   Update_Code,    Update_Type)
                 VALUES(TO_NUMBER(:pId_No),:vImsi_No, :pSim_No,
                       :pLogin_No,     to_number(:pLogin_Accept),  TO_NUMBER(:pTotal_Date),
                        TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                       :pOp_Code,      'a'); 
        if(SQLCODE!=0){
	 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
			 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ��ʷ�����!");
            
            EXEC SQL ROLLBACK;
            return -1;
        }
        
/*NG ���� start*/
/*
    EXEC SQL INSERT INTO dCustSim 
                        (Id_No,   Switch_No, Sim_No)
                  VALUES(TO_NUMBER(:pId_No), :vImsi_No, :pSim_No);
        if(SQLCODE!=0){
	 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
			 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ�����!");
            EXEC SQL ROLLBACK;
            return -1;
        }
        
*/
			strcpy(sTdCustSim.sIdNo,pId_No);
			strcpy(sTdCustSim.sSwitchNo,vImsi_No);
			strcpy(sTdCustSim.sSimNo,pSim_No);
		    ret =OrderInsertCustSim(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
								pOp_Code, atol(pLogin_Accept), pLogin_No, "pubOpenAcctRes",
								sTdCustSim);
			if (ret!=0){
		 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
				pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ�����!");
	            EXEC SQL ROLLBACK;
	            return -1;				
				
			}			   
         
/*NG ���� end*/    
    /* 
    EXEC SQL INSERT INTO wPhoneImsi1
                   (Phone_No,    Imsi_No,     Op_Time,
                    Enable_Flag, Update_Code, Id_Code) 
            VALUES(:pPhone_No,  :vImsi_No,    
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                   '0',          0,          :pLogin_Accept);
       if (SQLCODE != 0) {
           pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi1) ����![%d]",SQLCODE);
           EXEC SQL ROLLBACK;
           return -1;
       }
    */
    /* ��ѯ��ˮ */
    EXEC SQL select sMaxIdCode.nextval into :sMax_IdCode from dual;
    if (SQLCODE != 0) {       
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","��ѯ��ˮʧ��!");
        EXEC SQL ROLLBACK;
        return -1;
    }
    
    Trim(pSysDate);
    Trim(pLogin_Accept);
    Trim(pTotal_Date);
    Trim(pLogin_No);
    Trim(pOp_Code);
    flag=0;
    EXEC SQL select count(*) into :flag from wPhoneImsi where imsi_no=:vImsi_No and end_time>sysdate;
    if(flag>0)
    {
    	pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "IMSI�ڹ�ϵ��Ӧ��(wPhoneImsi)����ʹ�õļ�¼");
    	EXEC SQL ROLLBACK;
    	return -1;
    }
    EXEC SQL insert into wPhoneImsiHis(ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, UPDATE_ACCEPT, 
                                       UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN, UPDATE_TYPE, UPDATE_CODE)
             select ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, to_number(:pLogin_Accept), 
                     TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),TO_NUMBER(:pTotal_Date),:pLogin_No,'d',:pOp_Code
             from wPhoneImsi where imsi_no=:vImsi_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
        
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��ʷ��(wPhoneImsiHis) imsiʧ��!");

        EXEC SQL ROLLBACK;
        return -1;
    }
    
    /*NG���� start*/
    /*
    EXEC SQL delete wPhoneImsi where imsi_no=:vImsi_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) imsiʧ��!");
        
        EXEC SQL ROLLBACK;
        return -1;
    }
    */
    Trim(vImsi_No);
    strcpy(v_parameter_array[0],vImsi_No);
    ret=OrderDeleteOtherWPhoneImsi(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						pOp_Code, atol(pLogin_Accept), pLogin_No, "pubOpenAcctRes",
						vImsi_No,"",&v_parameter_array);
    if (ret < 0){
 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
		pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) imsiʧ��!");        
        EXEC SQL ROLLBACK;
        return -1;   
    
    }
					   
    /*NG���� end*/
/*
    flag=0;
    EXEC SQL select count(*) into :flag from wPhoneImsi where phone_no=:pPhone_No and end_time>sysdate;
    if(flag>0)
    {
    	pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "�ֻ������ڹ�ϵ��Ӧ��(wPhoneImsi)����ʹ�õļ�¼");
    	EXEC SQL ROLLBACK;
    	return -1;
    }
*/
    EXEC SQL insert into wPhoneImsiHis(ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, UPDATE_ACCEPT, 
                                       UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN, UPDATE_TYPE, UPDATE_CODE)
             select ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, to_number(:pLogin_Accept), 
                     TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),TO_NUMBER(:pTotal_Date),:pLogin_No,'d',:pOp_Code
             from wPhoneImsi where phone_no=:pPhone_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��ʷ��(wPhoneImsiHis) phoneʧ��!");
        
        EXEC SQL ROLLBACK;
        return -1;
    }
    /*NG���� start*/
    /*
    EXEC SQL delete wPhoneImsi where phone_no=:pPhone_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) phoneʧ��!");
        
        EXEC SQL ROLLBACK;
        return -1;
    }*/
    EXEC SQL DECLARE curng_01 CURSOR for select IMSI_NO from wPhoneImsi where phone_no=:pPhone_No;
    EXEC SQL OPEN curng_01;        	
    for (;;){
    	EXEC SQL FETCH curng_01 INTO :vImsi_No_Ng ;
	    if ((SQLCODE != 0) && (SQLCODE != 1403) )
	    {
		     PUBLOG_DBDEBUG("pubOpenAcctRes");
			 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","���α�(wPhoneImsiHis) phoneʧ��!");
	    	EXEC SQL CLOSE curng_01;
	        EXEC SQL ROLLBACK;
			return -1;
	    }    	
	    if (SQLCODE == 1403) break;
	    strcpy(v_parameter_array[0],vImsi_No_Ng);
	    /*strcpy(v_parameter_array[1],pPhone_No);
	    ret=OrderDeleteWPhoneImsi(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							pOp_Code, atol(pLogin_Accept), pLogin_No, "pubOpenAcctRes",
						vImsi_No," and phone_no=:pPhone_No ",&v_parameter_array);*/
		ret=OrderDeleteOtherWPhoneImsi(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							pOp_Code, atol(pLogin_Accept), pLogin_No, "pubOpenAcctRes",
						vImsi_No_Ng,"",&v_parameter_array);
	    if (ret!=0){
	 	    PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) imsiʧ��!");        
			EXEC SQL CLOSE curng_01;
	        EXEC SQL ROLLBACK;
	        return -1;   
	    }    
	}    
	EXEC SQL CLOSE curng_01;
    /*NG���� end*/
    
    /*NG���� start*/
    /*
    EXEC SQL insert into wPhoneImsi(
                   ID_CODE   ,   PHONE_NO ,  IMSI_NO ,
                   BEGIN_TIME ,  END_TIME ) 
             values(to_number(:sMax_IdCode), :pPhone_No, :vImsi_No , 
                   TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                   to_date('20501230 01:01:01','YYYYMMDD HH24:MI:SS'));
    if (SQLCODE != 0) {

	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) ����!");
        EXEC SQL ROLLBACK;
        return -1;
    }
    
    */
    strcpy(sTwPhoneImsi.sIdCode,sMax_IdCode);
	strcpy(sTwPhoneImsi.sPhoneNo,pPhone_No);
	strcpy(sTwPhoneImsi.sImsiNo,vImsi_No);
	strcpy(sTwPhoneImsi.sBeginTime,pSysDate);
	strcpy(sTwPhoneImsi.sEndTime,"20501230 01:01:01");
    
	ret=OrderInsertWPhoneImsi(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						    pOp_Code, atol(pLogin_Accept), pLogin_No, "pubOpenAcctRes",
						    sTwPhoneImsi);
					            
    if (ret !=0){
    	
 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) ����!");
        EXEC SQL ROLLBACK;
        return -1;   
    	
    }	
    /*NG���� end*/
    /* 
    flag=0;
    EXEC SQL select count(*) into :flag from wPhoneImsiChg where IMSI_NO=:vImsi_No and flag='2';
    if (SQLCODE != 0 && SQLCODE!=1403) 
    {
        
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","��ѯwPhoneImsiChg��ʧ��!");
        EXEC SQL ROLLBACK;
        return -1;
    }
    if(flag>0)
    {
        pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "�ÿ�Ŀǰ����ʹ�ã�����30����֮��ʹ�ã�������������![%d]",SQLCODE);
        pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "SQLERRMSG = [%s]",SQLERRMSG);
        EXEC SQL ROLLBACK;
        return -2;
    }*/ /*modify by wangdp at 20091020*/

    EXEC SQL insert into wPhoneImsiChg(
                 ID_CODE  , PHONE_NO ,  IMSI_NO ,
                 BEGIN_TIME , END_TIME , FLAG )
             values(to_number(:sMax_IdCode), :pPhone_No, :vImsi_No , 
                 TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                 to_date('20501230 01:01:01','YYYYMMDD HH24:MI:SS') ,
                 '1');
    if (SQLCODE != 0) {
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����wPhoneImsiChg����!");
        EXEC SQL ROLLBACK;
        return -1;
    }
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "bbbbbbbbb");
    
    /*���������޶�*/
    EXEC SQL SELECT bill_code,good_type INTO :vbillCode,:vgoodType FROM dgoodphoneres
    		 WHERE phone_no=:pPhone_No;
    if(SQLCODE != 0 && SQLCODE != 1403)
    {    
	     PUBLOG_DBDEBUG("pubOpenAcctRes");
		 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SELECT dgoodphoneres����!");

        EXEC SQL ROLLBACK;
        return -1;
    }
    else if(SQLCODE == 0)
    {
    	if(strcmp(vbillCode,"00000000") == 0)
    	{
    		EXEC SQL INSERT INTO dgoodphonenolimit
    						(
    						 phone_no,region_code,district_code,good_type,GROUP_ID
    						)
    					VALUES
    						(
    						 :pPhone_No,substr(:pBelong_Code,1,2),substr(:pBelong_Code,3,2),:vgoodType,:vGroupId
    						);
    		if(SQLCODE != 0)
		    {
			     PUBLOG_DBDEBUG("pubOpenAcctRes");
				 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","INSERT INTO dgoodphonenolimit����!");
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
    	}
    	else
    	{
    		EXEC SQL SELECT limit_fee INTO :ilimitFee FROM dreggoodbillcfg
    				 WHERE region_code=substr(:pBelong_Code,1,2) 
    				 AND bill_code=:vbillCode
    				 AND good_type=:vgoodType;
    		if(SQLCODE != 0)
		    {
  		        PUBLOG_DBDEBUG("pubOpenAcctRes");
				pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SELECT dreggoodbillcfg����!");
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
		    
		    EXEC SQL SELECT limit_fee INTO :tlimitFee FROM dreggoodbillcfg
    				 WHERE region_code='00' 
    				 AND use_flag='Y'
    				 AND good_type=:vgoodType;
    		if(SQLCODE != 0)
		    {
  		        PUBLOG_DBDEBUG("pubOpenAcctRes");
				pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SELECT t dreggoodbillcfg����!");
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
		    
		    if(ilimitFee < tlimitFee)
		    {
		    	EXEC SQL INSERT INTO dgoodphonenolimit
    						(
    						 phone_no,region_code,district_code,good_type,GROUP_ID
    						)
    					VALUES
    						(
    						 :pPhone_No,substr(:pBelong_Code,1,2),substr(:pBelong_Code,3,2),:vgoodType,:vGroupId
    						);
	    		if(SQLCODE != 0)
			    {
	  		        PUBLOG_DBDEBUG("pubOpenAcctRes");
					pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","INSERT INTO dgoodphonenolimit����!");
			        EXEC SQL ROLLBACK;
			        return -1;
			    }
		    }
    	}
    }
       
    pubLog_Debug(_FFL_, "pubOpenAcctRes", "", "------------ �û������޸���Դ��Ϣ pubOpenAcctRes() END --------");
    return 0;
}


/**
  * Auth: PeiJH
  * Name: pubOpenAcctFunc
  * Func: �û����������ط���Ϣ
  *       (���һ���ط�������д���)
  * Date: 2003/10/14
  * Stat: Test Pass
  */
int pubOpenAcctFunc(char *InId_No,       char *InPhone_No,    
                  char *InFunction_Type, char *InFunction_Code, 
                  char *InFavour_Month,  char *InAdd_No,
                  char *InOther_Attr,    char *InBelong_Code, 
                  char *InLogin_No,      char  InLogin_Accept[23],  
                  char *InOp_Code,       char *InTotal_Date,
                  char *InSysDate )
{
    EXEC SQL BEGIN DECLARE SECTION;
       char TmpSqlStr[1024];
       int  TmpLen=0;
       char pId_No[22+1];
       char pPhone_No[15+1];
       char pFunction_Type[1+1];
       char pFunction_Code[2+1];
       int  pFavour_Month=0;
       char pAdd_No[15+1];
       char pOther_Attr[40+1];
       char pBelong_Code[7+1];
       char pLogin_No[6+1];
       char pLogin_Accept[23];
       char pOp_Code[4+1];
       char pTotal_Date[8+1];
       char vOrg_Code[9+1];
       char pSysDate[20+1];
    EXEC SQL END   DECLARE SECTION;


    init(pPhone_No);      init(pFunction_Type);
    init(pFunction_Code); init(pAdd_No);     init(pOther_Attr);
    init(pBelong_Code);   init(pLogin_No);   init(pOp_Code);
    init(pTotal_Date);    init(vOrg_Code);   init(pSysDate);
    init(pId_No);

    memcpy(pLogin_Accept,   InLogin_Accept ,  22);    
    pFavour_Month =         atoi(InFavour_Month);       
    memcpy(pId_No,          InId_No,         22);
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pFunction_Type,  InFunction_Type, 1 );
    memcpy(pFunction_Code,  InFunction_Code, 2 );      
    memcpy(pAdd_No,         InAdd_No,        15);
    memcpy(pOther_Attr,     InOther_Attr,    40);
    memcpy(pBelong_Code,    InBelong_Code,   7 );
    memcpy(pLogin_No,       InLogin_No,      6 );
    memcpy(pOp_Code,        InOp_Code,       4 ); 
    memcpy(pTotal_Date,     InTotal_Date,    8 );
    memcpy(pSysDate,        InSysDate,       20);

    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "---------- �û����������ط���Ϣ pubOpenAcctFunc() BEGIN --------");
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Phone_No       = [%s]",   pPhone_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Function_Code  = [%s]",   pFunction_Code  );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Function_Type  = [%s]",   pFunction_Type  );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Favour_Month   = [%d]",   pFavour_Month   );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Add_No         = [%s]",   pAdd_No         );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Other_Attr     = [%s]",   pOther_Attr     );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Belong_code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Op_Code        = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "pTotal_Date    = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "pSysDate       = [%s]",   pSysDate        );
#endif

    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
 	         PUBLOG_DBDEBUG("pubOpenAcctFunc");
			 pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","ȡ������Ϣ����!");             
             EXEC SQL ROLLBACK;
             return -1;
         }

    /* ��ʷ����¹��� 'a'���ӡ�'u'�޸ġ�'d'ɾ�� */
    EXEC SQL INSERT INTO dCustFuncHis
                        (Id_No,       Function_Type,     Function_Code,
                         Op_Time,
                         Update_Login,Update_Accept,     Update_Date,
                         Update_Time, Update_Code,Update_Type)
                  VALUES(TO_NUMBER(:pId_No),:pFunction_Type,:pFunction_Code,
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pLogin_No, to_number(:pLogin_Accept),     TO_NUMBER(:pTotal_Date),
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pOp_Code,  'a');
        if(SQLCODE!=0){
 	         PUBLOG_DBDEBUG("pubOpenAcctFunc");
			 pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط���ʷ�����!");             
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL INSERT INTO dCustFunc 
                        (Id_No,Function_Type,Function_Code,Op_Time)
                  VALUES(TO_NUMBER(:pId_No),:pFunction_Type,:pFunction_Code,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'));
        if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubOpenAcctFunc");
			pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط���Ϣ��dCustFunc����!");                         
            EXEC SQL ROLLBACK;
            return -1;
        }

    if(pFavour_Month > 0 || strlen(ltrim(rtrim(pAdd_No))) >0){
    
        EXEC SQL INSERT INTO dCustFuncAddHis 
                            (Id_No,          Function_Type,      Function_Code,
                             Begin_Time,     Bill_Time, 
                             Favour_Month,   Add_No,             Other_Attr,
                             Update_Login,   Update_Accept,      Update_Date,
                             Update_Time,    Update_Code,        Update_Type)
                    VALUES(TO_NUMBER(:pId_No),:pFunction_Type,  :pFunction_Code,
                           TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),        
                           Add_Months(TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                          :pFavour_Month),
                          :pFavour_Month, :pAdd_No,           :pOther_Attr,
                          :pLogin_No ,     to_number(:pLogin_Accept),     TO_NUMBER(:pTotal_Date),
                           TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),       
                          :pOp_Code,       'a');

        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctFunc");
			pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����û������ط���ʷ��(dCustFuncAddHis)����!");                         
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL INSERT INTO dCustFuncAdd 
                        (Id_No,       Function_Type,      Function_Code,
                         Begin_Time,  Bill_Time,
                         Favour_Month,
                         Add_No,      Other_Attr)
                  VALUES(TO_NUMBER(:pId_No),:pFunction_Type,:pFunction_Code,
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),    
                          Add_Months(TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pFavour_Month),
                         :pFavour_Month,
                         :pAdd_No,    :pOther_Attr);

        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctFunc");
			pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","���븽���ط���Ϣ��dCustFuncAdd ����!");                         
            EXEC SQL ROLLBACK;
            return -1;
        }

    }

    if(strlen(ltrim(rtrim(pAdd_No))) >0){

    EXEC SQL UPDATE dCustRes
                SET Resource_Code ='1',
                    Login_No      =:pLogin_No,
                    Login_accept  =to_number(:pLogin_Accept),
                    Op_Time       = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    Total_Date    = TO_NUMBER(:pTotal_Date),
                    Org_Code      =:vOrg_Code,
                    Region_Code   = DECODE(SUBSTR(:pBelong_Code,1,2),
                                   '99',SUBSTR(:vOrg_Code,1,2),
                                    SUBSTR(:pBelong_Code,1,2)),
                    District_Code = DECODE(SUBSTR(:pBelong_Code,3,2),
                                   '99',SUBSTR(:vOrg_Code,3,2),
                                    SUBSTR(:pBelong_Code,3,2)),
                    Town_Code     = DECODE(SUBSTR(:pBelong_Code,5,3),
                                   '999',SUBSTR(:vOrg_Code,5,3),
                                    SUBSTR(:pBelong_Code,5,3))
              WHERE Phone_No      = :pAdd_No
                AND No_Type       = 'g'
                AND Resource_Code = '0';

        if (SQLROWS != 1) {
            #ifdef _DEBUG_
		        PUBLOG_DBDEBUG("pubOpenAcctFunc");
				pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","���º�����Դ��(dCustRes) ����!");                         
            #endif
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL INSERT INTO wPhId
                        (Old_No,        New_No,   
                         Begin_Time,
                         Imsi_No,       Id_No)
                 VALUES(:pAdd_No,      :pPhone_No,
                         To_Date('20500101','YYYYMMDD'),
                        'UnKnown',      TO_NUMBER(:pId_No));

        if (SQLCODE != 0) {
            #ifdef _DEBUG_
		        PUBLOG_DBDEBUG("pubOpenAcctFunc");
				pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","����ĺ�ת����(wPhId) ����!");                         
            #endif
            EXEC SQL ROLLBACK;
            return -1;
        }
    }
    
    EXEC SQL INSERT INTO wCustFuncDay
                        (Id_No,      Function_Code, Add_Flag,        
                         Total_Date, Op_Time,       Op_Code,         
                         Login_No,   Login_Accept)
                  VALUES(TO_NUMBER(:pId_No),    
                        :pFunction_Code,'1',TO_NUMBER(:pTotal_Date),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pOp_Code,  :pLogin_No,    to_number(:pLogin_Accept));    
        if (SQLCODE != 0) {
            #ifdef _DEBUG_
		        PUBLOG_DBDEBUG("pubOpenAcctFunc");
				pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط�������ˮ��(wCustFuncDay) ����!");                         
            #endif
            EXEC SQL ROLLBACK;
            return -1;
        }                        
    
    
    pubLog_Debug(_FFL_, "pubOpenAcctFunc", "", "------------ �û����������ط���Ϣ pubOpenAcctFunc() END --------");
    return 0;

}

/**
  * Auth: PeiJH
  * Name: pubOpenAcctCheck_1
  * Func: �û�����֧Ʊ���ݴ���
  * Date: 2003/10/14
  * Stat: Test Pass
  */
int pubOpenAcctCheck_1(char *InCheck_No,  float InCheck_Pay,  char *InBank_Code,                       
                   char *InOp_Note,   char  *InLogin_No,  char InLogin_Accept[23],
                   char *InOp_Code,   char *InTotal_Date, char *InSysDate,
                   char *InSystem_Note ,char *vOrgId)
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
       char  vOrg_Code[9+1];
       float vCheck_Prepay=0;
       
       int vret=0;
       /*char vOrgId[10+1];*/
    EXEC SQL END   DECLARE SECTION;

    init(pCheck_No);   init(pBank_Code); init(pOp_Note);
    init(pOp_Code);    init(pLogin_No);  init(pTotal_Date); 
    init(pSysDate);    init(vOrg_Code);  init(pLogin_Accept);
    /*init(vOrgId);*/
    pCheck_Pay=InCheck_Pay;
    memcpy(pLogin_Accept,   InLogin_Accept,  22);    
    memcpy(pCheck_No,       InCheck_No,      20);
    memcpy(pBank_Code,      InBank_Code,     5);
    memcpy(pOp_Note,        InOp_Note,       60);
    memcpy(pLogin_No,       InLogin_No,      6);
    memcpy(pOp_Code,        InOp_Code,       4);
    memcpy(pTotal_Date,     InTotal_Date,    8);
    memcpy(pSysDate,        InSysDate,       20);

    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "------------ �û�����֧Ʊ���� pubOpenAcctCheck_1() BEGIN --------");
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Check_No       = [%s]",   InCheck_No      );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Check_Pay      = [%.2f]", InCheck_Pay     );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Bank_Code      = [%s]",   InBank_Code     );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Op_Note        = [%s]",   InOp_Note       );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Login_Accept   = [%d]",   InLogin_Accept  );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Login_No       = [%s]",   InLogin_No      );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Op_Code        = [%s]",   InOp_Code       );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "Total_Date     = [%s]",   InTotal_Date    );
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "SysDate        = [%s]",   InSysDate       );
	
    
    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
	if(SQLCODE!=0)
	{
        PUBLOG_DBDEBUG("pubOpenAcctCheck_1");
		pubLog_DBErr(_FFL_,"pubOpenAcctCheck_1","-1","ȡ������Ϣ����!");                         
		return -1;
    }     
	
/** ��֯�������� liuhao    2000900401 begin*/ 	
/*
	Trim(pLogin_No);
	vret=sGetLoginOrgid(pLogin_No,vOrgId);
	if(vret <0)
	{		
		pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "��ѯ����org_idʧ��!ret=%d",vret);
		return -2;
	}
	Trim(vOrgId);	
*/	
/** ��֯�������� liuhao    2000900401 end*/ 		
    EXEC SQL INSERT INTO wCheckOpr
                        (Total_Date,      Login_Accept,     Org_Code,
                         Login_No,        Op_Code,          Bank_Code,
                         Check_No,        Check_Pay,        Op_Time,
                         Op_Note,ORG_ID)
                  VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),   :vOrg_Code,
                         :pLogin_No,     :pOp_Code,        :pBank_Code,
                         :pCheck_No,     :pCheck_Pay,       
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pOp_Note,:vOrgId);
    if (SQLCODE != 0) {
        PUBLOG_DBDEBUG("pubOpenAcctCheck_1");
		pubLog_DBErr(_FFL_,"pubOpenAcctCheck_1","-1","����֧Ʊ������ˮ��(wCheckOpr) ����!");                         
        EXEC SQL ROLLBACK;
        return -1;
    }

    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 begin*/
    /*
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "---- bank_code=[%s]",pBank_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "---- check_no =[%s]",pCheck_No);
    
    EXEC SQL SELECT Check_Prepay INTO :vCheck_Prepay
               FROM dCheckPrepay
              WHERE Bank_Code  = :pBank_Code
                AND Check_No   = :pCheck_No;             
    if (SQLCODE != 0) {
        PUBLOG_DBDEBUG("pubOpenAcctCheck_1");
		pubLog_DBErr(_FFL_,"pubOpenAcctCheck_1","-1","��ѯ֧Ʊ����(dCheckPrepay) ����!");                         
        EXEC SQL ROLLBACK;
        return -3;
    }                

    if(vCheck_Prepay >= InCheck_Pay){
        EXEC SQL UPDATE dCheckPrepay
                    SET Check_Prepay = Check_Prepay   - :pCheck_Pay,
                             Op_Time = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') 
                  WHERE Bank_Code  = :pBank_Code
                    AND Check_No   = :pCheck_No;
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctCheck_1");
			pubLog_DBErr(_FFL_,"pubOpenAcctCheck_1","-1","����֧Ʊ����(dCheckPrepay) �����֧Ʊ����!");                         
            EXEC SQL ROLLBACK;
            return -1;
        }                    
    }
    */
    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 end*/
    pubLog_Debug(_FFL_, "pubOpenAcctCheck_1", "", "------------ �û�����֧Ʊ���� pubOpenAcctCheck_1() End --------");
    return 0;
}

/**
  * Auth: PeiJH
  * Name: pubCBXExpire
  * Func: �����û���Ч�� 
  * Date: 2003/10/14
  */
int pubCBXExpire(char *InId_No,   char *InBelong_Code, float InPrepay_Fee,
                 char *InOp_Note, char *InLogin_No,    char InLogin_Accept[23],
                 char *InOp_Code, char *InTotal_Date,  char *InSysDate,
                 char *InSm_Code, char *InBill_Code)
{
	
    EXEC SQL BEGIN DECLARE SECTION;
       char  TmpSqlStr[1024];
       char  pId_No[22+1];
       char  pBelong_Code[7+1];
       float pPrepay_Fee=0.00;  
       char  pOp_Note[60+1];
       char  pLogin_No[6+1];
       char  pLogin_Accept[23];
       char  vOrg_Code[9+1];
       char  pOp_Code[4+1];
       int   vDays=0;
       char  vTime_Code[2+1];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1];
       
       char  sV_Region_code[2+1];     /* ��������    */
       char  sV_Sqlstr[1048];         /* ��ʱ�ַ���  */
       char  pExpire_Flag[1+1];       /* ��Ч�ڱ�־  */
    EXEC SQL END   DECLARE SECTION;
   
    init(vTime_Code); init(pOp_Note);    init(pLogin_No);
    init(vOrg_Code);  init(vTime_Code);  init(pBelong_Code); 
    init(pOp_Code);   init(pTotal_Date); init(pSysDate);
    init(pId_No);     init(pLogin_Accept); init(sV_Sqlstr);
    init(pExpire_Flag);

    pubLog_Debug(_FFL_, "pubCBXExpire", "", "------------ ��Ч�ڴ��� pubCBXExpire() Begin --------");
    pPrepay_Fee   =         InPrepay_Fee;    
    memcpy(pLogin_Accept,   InLogin_Accept,  22);        
    memcpy(pId_No,          InId_No,         22);
    memcpy(pBelong_Code,    InBelong_Code,   7 );
    memcpy(pOp_Note,        pOp_Note,        60);
    memcpy(pLogin_No,       InLogin_No,      6 );
    memcpy(pOp_Code,        InOp_Code,       4 );
    memcpy(pTotal_Date,     InTotal_Date,    8 );
    memcpy(pSysDate,        InSysDate,       20);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Prepay_Fee     = [%.2f]", pPrepay_Fee     );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Op_Note        = [%s]",   pOp_Note        );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "Total_Date     = [%s]",   pTotal_Date     ); 
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "SysDate        = [%s]",   pSysDate        );     
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "InSm_Code      = [%s]",   InSm_Code        );     
    #endif
              
    
    EXEC SQL SELECT Org_Code , substr(Org_Code,1,2) INTO :vOrg_Code ,:sV_Region_code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubCBXExpire");
			pubLog_DBErr(_FFL_,"pubCBXExpire","-1","ȡ������Ϣ����!");                         
             
             return -1;
         }    
        
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "vOrg_Code          = [%s]",   vOrg_Code          );
        pubLog_Debug(_FFL_, "pubCBXExpire", "", "sV_Region_code    = [%s]",   sV_Region_code    );
    #endif
  /***********  
    EXEC SQL SELECT expire_flag into :pExpire_Flag
             FROM   sSmCode
             WHERE  region_code=:sV_Region_code and sm_code=:InSm_Code;
         if(SQLCODE==1403)
         {
             pubLog_Debug(_FFL_, "pubCBXExpire", "", "sSmCode��û�з�������������");
             return 0;
         } 
         else if(SQLCODE!=0){
             pubLog_Debug(_FFL_, "pubCBXExpire", "", "ȡ��Ч�ڱ�־����!");
             return -1;
         }
pubLog_Debug(_FFL_, "pubCBXExpire", "", "pExpire_Flag=[%s]",pExpire_Flag);
    if(strncmp(pExpire_Flag,"N",1)==0)
    {
    	pubLog_Debug(_FFL_, "pubCBXExpire", "", "��Ч�ڱ�־ΪN������Ҫ������Ч�ڱ�");
    	return 0;
    }
  */  
    EXEC SQL SELECT  Days,  Time_Code  
               INTO :vDays, :vTime_Code 
               FROM sPayTime  
              WHERE Region_Code=SUBSTR(:vOrg_Code,1,2)
                AND Time_Code=:InSm_Code
                AND Bill_Code=:InBill_Code
                AND Begin_Money<=:pPrepay_Fee
                AND End_Money>:pPrepay_Fee
                AND open_flag='0';
                
    if(SQLCODE !=0 && SQLCODE!=1403)
    {
        PUBLOG_DBDEBUG("pubCBXExpire");
		pubLog_DBErr(_FFL_,"pubCBXExpire","-1","��ѯsPayTime����!");                         
        return -1;
    }
    if(SQLCODE==1403)
    {

        PUBLOG_DBDEBUG("pubCBXExpire");
		pubLog_DBErr(_FFL_,"pubCBXExpire","0","���ʷ���sPayTime�����ڣ�����Ҫ������Ч�ڱ�!");                         
    	
    	return 0;
    }
            
    EXEC SQL INSERT INTO dCustExpire 
                        (Id_No,      Open_Time, Begin_Flag,
                         Begin_Time, Old_Expire,EXPIRE_TIME,
                         Bak_Field)
                  VALUES(TO_NUMBER(:pId_No),    
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pExpire_Flag,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS')+:vDays,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS')+:vDays,
                        'NULL');
    if(SQLCODE !=0){
        #ifdef _DEBUG_
	        PUBLOG_DBDEBUG("pubCBXExpire");
			pubLog_DBErr(_FFL_,"pubCBXExpire","-1","����dCustExpire ����!");                         
        #endif
        EXEC SQL ROLLBACK;
        return -1;
    }

    EXEC SQL INSERT INTO wExpireList 
                        (Id_No,      Total_Date,    Login_Accept,
                         Op_Code,    Time_Code,     Days,
                         Login_No,   Org_Code,      Op_Time,
                         Op_Note) 
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pTotal_Date),
                        to_number(:pLogin_Accept),
                        :pOp_Code,  :vTime_Code,   :vDays,
                        :pLogin_No, :vOrg_Code,  
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pOp_Note);
    if (SQLCODE !=0 ){ 
        #ifdef _DEBUG_
	        PUBLOG_DBDEBUG("pubCBXExpire");
			pubLog_DBErr(_FFL_,"pubCBXExpire","-1","����wExpireList ���� !");                         
        #endif
        EXEC SQL ROLLBACK;
        return -1;
    }
    pubLog_Debug(_FFL_, "pubCBXExpire", "", "------------ ��Ч�ڴ��� pubCBXExpire() End --------");
    
    return 0;
}

/**
  * Auth: FuJian
  * Name: pubPack
  * Func: ����Ӫ�������� 
  * Date: 2004/01/30
  */
int pubPack(char *InId_No,        char *InPhone_No,    char *InBelong_Code, 
            char *InMachine_Code, float InMachine_Fee,
            char *InPack_Id,      char *InLogin_No,    char *InOrg_Code,
            char *InOp_Code,      char *InLoginAccept, char *InTotal_Date,
            char *InSysDate,      char *InMsgErr,char *vGroup_id,char* vOrg_id)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  pId_No[22+1];
       char  pPhone_No[15+1];
       char  pBelong_Code[7+1];
       char  pMachine_Code[3+1];
       char  pPack_Id[20+1];
       char  pLogin_No[6+1];
       char  pOrg_Code[9+1];
       char  pOp_Code[4+1];
       char  pLogin_Accept[23];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1];       
       
       int   vMonth_Num=0;
       
       
    EXEC SQL END   DECLARE SECTION;
   
    init(pId_No);        init(pPhone_No);    init(pBelong_Code);
    init(pMachine_Code); init(pPack_Id); 
    init(pLogin_No);     init(pOrg_Code);    init(pOp_Code);
    init(pLogin_Accept); init(pTotal_Date);  init(pSysDate);

    pubLog_Debug(_FFL_, "pubPack", "", "------------ Ӫ�������� pubPack() Begin --------");
    memcpy(pId_No,          InId_No,         22);        
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pBelong_Code,    InBelong_Code,   7 );
    memcpy(pMachine_Code,   InMachine_Code,  3 );
    memcpy(pPack_Id,        InPack_Id,       20);
    memcpy(pLogin_No,       InLogin_No,      6 );
    memcpy(pOrg_Code,       InOrg_Code,      9 );
    memcpy(pOp_Code,        InOp_Code,       4 );
    memcpy(pLogin_Accept,   InLoginAccept,   22);
    memcpy(pTotal_Date,     InTotal_Date,    8 );
    memcpy(pSysDate,        InSysDate,       20);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubPack", "", "Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_, "pubPack", "", "pPhone_No      = [%s]",   pPhone_No       );
        pubLog_Debug(_FFL_, "pubPack", "", "Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_, "pubPack", "", "pMachine_Code  = [%s]",   pMachine_Code   );
        pubLog_Debug(_FFL_, "pubPack", "", "pPack_Id       = [%s]",   pPack_Id        );
        pubLog_Debug(_FFL_, "pubPack", "", "Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_, "pubPack", "", "pOrg_Code      = [%s]",   pOrg_Code       );
        pubLog_Debug(_FFL_, "pubPack", "", "Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_, "pubPack", "", "Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_, "pubPack", "", "Total_Date     = [%s]",   pTotal_Date     ); 
        pubLog_Debug(_FFL_, "pubPack", "", "SysDate        = [%s]",   pSysDate        );     
    #endif

    EXEC SQL select month_num into :vMonth_Num from sPackCode where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubPack", "", "��ѯsPackCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsPackCode��ʧ��!");
        PUBLOG_DBDEBUG("pubPack");
		pubLog_DBErr(_FFL_,"pubPack","-1",InMsgErr);                         
        return -1;
    }
    /*edit organizational liuhao 20090331 begin
	Trim(pLogin_No);
	vret=sGetLoginOrgid(pLogin_No,vOrgId);
	if(vret <0)
	{		
		strcpy(InMsgErr,"��ѯ����org_idʧ��!");
		retrun -1;
	}
	Trim(vOrgId);
	Trim(pPhone_No);
	
	if (sGetUserGroupid(pPhone_No,vGroup_id) < 0 )
	{
		strcpy(InMsgErr,"��ѯ�û�group_idʧ��!");
		return -1;
		
	}	
	Trim(vGroup_id);
	
	edit organizational liuhao 20090331 begin*/	
    EXEC SQL insert into dPackMsg(id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time,group_id,org_id )
             select to_number(:pId_No) , :pPhone_No , :pBelong_Code , :pMachine_Code , :pPack_Id , 
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),add_months(to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),month_num) ,
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , :InMachine_Fee , Month_num , :pLogin_No , :pOrg_Code , 
                    to_number(:pLogin_Accept) , :pOp_Code , :pTotal_Date , to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),:vGroup_id, :vOrg_id 
             from sPackCode
             where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubPack", "", "����dPackMsg��ʧ��!");
        strcpy(InMsgErr,"����dPackMsg��ʧ��!");
        PUBLOG_DBDEBUG("pubPack");
		pubLog_DBErr(_FFL_,"pubPack","-1",InMsgErr);                         
        return -1;
    }
                    
    EXEC SQL insert into dPackMsgHis(id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , Update_accept , Update_time , Update_date , Update_login , 
                    Update_type , Update_code,group_id,org_id )
             select id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , to_number(:pLogin_Accept) , 
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss') , to_number(:pTotal_Date) , :pLogin_No , 'a' , :pOp_Code,
                    :vGroup_id,:vOrg_id 	
             from dPackMsg
             where id_no=:pId_No;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubPack", "", "����dPackMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dPackMsgHis��ʧ��!");
        PUBLOG_DBDEBUG("pubPack");
		pubLog_DBErr(_FFL_,"pubPack","-1",InMsgErr);                         

        return -1;
    }

    EXEC SQL insert into dBillPackMsg(id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                                      Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , Total_date , Op_time,
                                      group_id,org_id  )
             select to_number(:pId_No) , :pBelong_Code , :pMachine_Code , mode_code,mode_flag,mode_type,
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),add_months(to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),:vMonth_Num) ,
                    :pLogin_No , :pOrg_Code , to_number(:pLogin_Accept) , :pOp_Code , :pTotal_Date , to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),
                    :vGroup_id,:vOrg_id 		
             from sBillPackCode
             where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubPack", "", "����dBillPackMsg��ʧ��!");
        strcpy(InMsgErr,"����dBillPackMsg��ʧ��!");
        PUBLOG_DBDEBUG("pubPack");
		pubLog_DBErr(_FFL_,"pubPack","-1",InMsgErr);                         
        return -1;
    }
    
    EXEC SQL insert into dBillPackMsgHis(id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                                         Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                                         Total_date , Op_time , Update_accept , Update_time , 
                                         Update_date , Update_login , Update_type , Update_code,group_id,org_id )
             select id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                    Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                    Total_date , Op_time , to_number(:pLogin_Accept) , to_date(:pSysDate,'yyyymmdd hh24:mi:ss') , 
                    to_number(:pTotal_Date) , :pLogin_No , 'a' , :pOp_Code,:vGroup_id,:vOrg_id 
             from dBillPackMsg
             where id_no=:pId_No;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubPack", "", "����dBillPackMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dBillPackMsgHis��ʧ��!");
        PUBLOG_DBDEBUG("pubPack");
		pubLog_DBErr(_FFL_,"pubPack","-1",InMsgErr);                         
        return -1;
    }
	/*edit organizational liuhao 20090331 end*/	
    pubLog_Debug(_FFL_, "pubPack", "", "------------ Ӫ�������� pubPack() End --------");
    
    return 0;
}

/**
  * Auth: PeiJH
  * Name: pubOpenAcctConfirm
  * Func: ��¼�û�������ؿͻ����û����ʻ����� 
  * Date: 2003/10/14
  */ 
   
int pubOpenAcctConfirm(char *InCust_Id,char *InCon_Id,     char *InId_No, 
                 char *InCust_Passwd, char *InUser_Passwd, char *InCon_Passwd,
                 float InLimit_Owe,   char *InPhone_No,    char *InSm_Code , 
                 char *InService_Type,char *InAttr_Code,   char *InCredit_Code,
                 int   InCredit_Value,char *InPay_Code,    char *InPay_Type,   
                 int   InBill_Type,   int   InCmd_Right,   char *InSim_No,   
                 char InBelong_Code[7+1], char *InOp_Note,     char *InSystem_Note,
                 char *InLogin_No,    char *InIp_Address,  char  InLogin_Accept[23],
                 char *InOp_Code,     char *InInnet_Type,  char *InMachine_Code,
                 char *InAssure_Name, char *InId_Type,     char *InAssure_Id,   
                 char *InAssure_Phone,char *InAssure_Address,char *InAssure_ZIP,
                 char *InPost_Flag,   char *InPost_Type,   char *InPost_Address,
                 char *InPost_ZIP,    char *InPost_Fax,    char *InPost_Mail, char *InPost_Content,
                 float InCash_Pay,    float InCheck_Pay,   float InPrepay_Fee,
                 float InSim_Fee,     float InMachine_Fee, float InInnet_Fee,
                 float InChoice_Fee,  float InOther_Fee,   float InHand_Fee,
                 char *InTotal_Date,  char *InSysDate,
                 char In_Cust_Name[61], char In_Cust_Address[61],
                 char In_Id_Type[3] ,   char In_Id_Iccid[21], char In_NewOldFlag[3],
                 char In_CustDocFlag[2+1] ,char *InAreaCode ,char *InGroupNo,
                 char *InTrue_flag,char *sVCustinfoType,char *sV_Agreement_Type,char* vOrgId)
{
	int Vret = 0;
	tCustDoc  custDoc;
	tLoginOpr vLog; 
	tAccDoc   accDoc;
	tUserDoc custStruc;
		
    EXEC SQL BEGIN DECLARE SECTION;
        char  TmpSqlStr[2048];
				int		juagecount=0;
        char  pCust_Id[22+1];
        char  pCon_Id[22+1];
        char  pId_No[22+1];
        char  pCust_Passwd[8+1];
        char  pUser_Passwd[8+1];
        char  pCon_Passwd[8+1];                
        float pLimit_Owe=0;
        char  pPhone_No[15+1];
        char  pSm_Code[2+1];
        char  pService_Type[2+1];
        char  pAttr_Code[8+1];
        char  pCredit_Code[1+1];
        int   pCredit_Value=0;
        char  pPay_Code[1+1];
        char  pPay_Type[4+1]; /** ������� **/
        int   pBill_Type=0;  /** ?? **/
        int   pCmd_Right=0;
        char  pSim_No[20+1];
        char  pBelong_Code[7+1];
        char  pAreaCode[20+1];
        char  pGroupNo[10];
        char  pOp_Note[60+1];
        char  pSystem_Note[60+1];
        char  pLogin_No[6+1];
        char  pIp_Address[15+1];
        char  pLogin_Accept[23];
        char  pOp_Code[4+1];

        char  pInnet_Type[2+1];
        char  pMachine_Code[3+1];
        
        long  vAssure_No=0;        /* �������� number(12)*/
        char  pAssure_Name[60+1];
        char  pId_Type[1+1];
        char  pAssure_Id[20+1];
        char  pAssure_Phone[20+1];
        char  pAssure_Address[60+1];
        char  pAssure_ZIP[6+1];

        char  pPost_Flag[1+1];
        char  pPost_Type[1+1];
        char  pPost_Address[200+1];
        char  pPost_ZIP[6+1];
        char  pPost_Fax[30+1];
        char  pPost_Mail[30+1];
        char  pPost_Content[1+1];
       
        float pCash_Pay=0;  
        float pCheck_Pay=0;  
        float pCash_Pay_2=0;  
        float pCheck_Pay_2=0;  
        float pShould_pay=0;
        float pPrepay_Fee=0;
        float pSim_Fee=0;  
        float pMachine_Fee=0;  
        float pInnet_Fee=0;  
        float pChoice_Fee=0;  
        float pOther_Fee=0;  
        float pHand_Fee=0;  
        
        long  vOp_No=0; /*   ��������  */ 
        char  vOrg_Code[9+1];
        char  vRegion_Code[2+1];
        char  vDistrict_Code[2+1];
        char  vTown_Code[3+1];
        char  vParent_Id[22+1];
        float vInterest =0;
        char  vDateChar[6+1];
        int   vAssure_Num=0;
        float vPay_Money=0;
        int   vIds=0;
        char  vCon_Cust_Id[22+1];
        char  pTotal_Date[8+1];
        char  pSysDate[20+1];
        int   vCount=0;
        char  vPayFlag[1+1];
        
        char p_Cust_Name[61] ;        /* �����пͻ����� */
        char p_Cust_Address[61] ;     /* �����пͻ���ַ */
        char p_Id_Type[3] ;           /* ������֤������ */
        char p_Id_Iccid[21] ;         /* ������֤������ */
        char p_In_CustDocFlag[3];     /* �ͻ����ϱ�־   0 ��һ��Ϣ   1  ������Ϣ */
        char p_NewOldFlag[2+1];       /* �ͻ�����   0 �½��ͻ�  1 ���пͻ� */
        char sV_SqlStr[3048];         /* ��ʱ�ַ���     */
        int  sV_Cust_num=0;           /* �ͻ�����       */
        char sV_Cb_passwd[8+1];       /* ����������     */

        char sV_true_flag[1+1];
        char sIn_Custinfo_Type[2+1];
        char sIn_Agreement_Type[1+1];
        int vGoodNoCount=0;

      char vGroupId[10+1];    
      char vChgFlag[1+1];
      char vCustFlag[1+1]; 
      char sErrorMess[60];  
      char return_mess[60]; 
      /*NG*/    
      int ret=0;
      char vAssure_No_NG[12+1];
      char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
      TdAssuMsg	sTdAssuMsg;
      TdConMark	sTdConMark;
      TdConUserMsg tdConUserMsg;
      TdCustInnet  sTdCustInnet;
      TdCustPost   sTdCustPost;
      TdCustConMsg sTdCustConMsg;
      char sBeginYmd[8+1];
      char sBeginTm[6+1];
      int  assure_count=0;
    EXEC SQL END   DECLARE SECTION;

    init(pCust_Passwd);    init(pCon_Passwd);    init(pUser_Passwd);
    init(pPhone_No);       init(pSm_Code);       init(pService_Type);
    init(pAttr_Code);      init(pCredit_Code);   init(pPay_Code);
    init(pPay_Type);       init(pSim_No);        init(pBelong_Code); init(pAreaCode);init(pGroupNo);
    init(pOp_Note);        init(pLogin_No);      init(pOp_Code);
    init(pInnet_Type);     init(pMachine_Code);  init(pId_Type);
    init(pAssure_Name);    init(pAssure_Id);     init(pAssure_Phone);
    init(pAssure_Address); init(pPost_Flag);     init(pPost_Type);
    init(pPost_Address);   init(pPost_ZIP);      init(pPost_Fax);
    init(pPost_Mail);      init(pPost_Content);init(vOrg_Code);      init(vRegion_Code);
    init(vDistrict_Code);  init(vTown_Code);     init(vDateChar);
    init(pTotal_Date);     init(pSysDate);       init(TmpSqlStr);
    init(pIp_Address);     init(vParent_Id);     init(vCon_Cust_Id);
    init(pCust_Id);        init(pCon_Id);        init(pId_No);
    init(pSystem_Note);    init(pAssure_ZIP);    init(vPayFlag);
    
    init(p_Cust_Name);    init(p_Cust_Address);
    init(p_Id_Type);      init(p_Id_Iccid); 
    init(p_NewOldFlag);   init(p_In_CustDocFlag);
    init(sV_Cb_passwd);
    init(sIn_Custinfo_Type);
    init(sIn_Agreement_Type);
    init(sV_true_flag);

    init(vGroupId);
    init(vChgFlag);
    init(vCustFlag);
	memset(&custDoc,0,sizeof(custDoc));
	memset(&vLog,0,sizeof(vLog));
	memset(&accDoc,0,sizeof(accDoc));	
	memset(&custStruc,0,sizeof(custStruc));
	init(sErrorMess);
	init(return_mess);
	init(vAssure_No_NG);
	init(sBeginYmd);
	init(sBeginTm);
	memset(v_parameter_array,0,sizeof(v_parameter_array));
    memset(&sTdAssuMsg,0,sizeof(sTdAssuMsg));
    memset(&sTdConMark,0,sizeof(sTdConMark));
    memset(&tdConUserMsg,0,sizeof(tdConUserMsg));
    memset(&sTdCustInnet,0,sizeof(sTdCustInnet));
    memset(&sTdCustPost,0,sizeof(sTdCustPost));
    memset(&sTdCustConMsg, 0, sizeof(sTdCustConMsg));
    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "-------- �û�����ȷ�� pubOpenAcctConfirm() BEGIN --------");

    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cust_Id        = [%s]",   InCust_Id        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Con_Id         = [%s]",   InCon_Id         );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Id_No          = [%s]",   InId_No          );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cust_Passwd    = [%s]",   InCust_Passwd    );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Con_Passwd     = [%s]",   InCon_Passwd     );   
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "User_Passwd    = [%s]",   InUser_Passwd    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Limit_Owe      = [%.2f]", InLimit_Owe      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Credit_Value   = [%d]",   InCredit_Value   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Bill_Type      = [%d]",   InBill_Type      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cmd_Right      = [%d]",   InCmd_Right      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Login_Accept   = [%s]",   InLogin_Accept   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cash_Pay       = [%.2f]", InCash_Pay       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Check_Pay      = [%.2f]", InCheck_Pay      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Prepay_Fee     = [%.2f]", InPrepay_Fee     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sim_Fee        = [%.2f]", InSim_Fee        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Machine_Fee    = [%.2f]", InMachine_Fee    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Innet_Fee      = [%.2f]", InInnet_Fee      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Choice_Fee     = [%.2f]", InChoice_Fee     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Other_Fee      = [%.2f]", InOther_Fee      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Hand_Fee       = [%.2f]", InHand_Fee       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Phone_No       = [%s]",   InPhone_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sm_Code        = [%s]",   InSm_Code        ); 
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Service_Type   = [%s]",   InService_Type   );         
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Attr_Code      = [%s]",   InAttr_Code      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Credit_Code    = [%s]",   InCredit_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Pay_Code       = [%s]",   InPay_Code       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Pay_Type       = [%s]",   InPay_Type       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sim_No         = [%s]",   InSim_No         );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Belong_Code    = [%s]",   InBelong_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Op_Note        = [%s]",   InOp_Note        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "System_Note    = [%s]",   InSystem_Note    );    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Login_No       = [%s]",   InLogin_No       );
    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Ip_Address     = [%s]",   InIp_Address     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Op_Code        = [%s]",   InOp_Code        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Innet_Type     = [%s]",   InInnet_Type     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Machine_Code   = [%s]",   InMachine_Code   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Name    = [%s]",   InAssure_Name    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Id_Type        = [%s]",   InId_Type        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Id      = [%s]",   InAssure_Id      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Phone   = [%s]",   InAssure_Phone   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Address = [%s]",   InAssure_Address );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_ZIP     = [%s]",   InAssure_ZIP     );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Flag      = [%s]",   InPost_Flag      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Type      = [%s]",   InPost_Type      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Address   = [%s]",   InPost_Address   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_ZIP       = [%s]",   InPost_ZIP       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Fax       = [%s]",   InPost_Fax       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Mail      = [%s]",   InPost_Mail      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Content      = [%s]",   InPost_Content);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Total_Date     = [%s]",   InTotal_Date     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SysDate        = [%s]",   InSysDate        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "In_NewOldFlag        = [%s]",   In_NewOldFlag        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "In_CustDocFlag        = [%s]",   In_CustDocFlag        );   

    pLimit_Owe    = InLimit_Owe;
    pCredit_Value = InCredit_Value;
    pBill_Type    = InBill_Type;     
    pCmd_Right    = InCmd_Right;
    memcpy(pLogin_Accept,    InLogin_Accept,   22);
    pCash_Pay     = InCash_Pay;
    pCheck_Pay    = InCheck_Pay;
    pCash_Pay_2   = InCash_Pay;
    pCheck_Pay_2  = InCheck_Pay;
    pPrepay_Fee   = InPrepay_Fee;
    pSim_Fee      = InSim_Fee;
    pMachine_Fee  = InMachine_Fee;
    pInnet_Fee    = InInnet_Fee;
    pChoice_Fee   = InChoice_Fee;
    pOther_Fee    = InOther_Fee;
    pHand_Fee     = InHand_Fee;
    
    
    memcpy(pCust_Id,        InCust_Id,       22);
    memcpy(pCon_Id,         InCon_Id,        22);
    memcpy(pId_No,          InId_No,         22);
    memcpy(pCust_Passwd,    InUser_Passwd,   8);    
    memcpy(pCon_Passwd,     InUser_Passwd,   8);    
    memcpy(pUser_Passwd,    InUser_Passwd,   8);    
    memcpy(pPhone_No,       InPhone_No,     15);
    memcpy(pSm_Code ,       InSm_Code,       2);  
    memcpy(pService_Type ,  InService_Type,  2);      
    memcpy(pAttr_Code,      InAttr_Code,     8);
    memcpy(pCredit_Code,    InCredit_Code,   1);
    memcpy(pPay_Code,       InPay_Code,      1);
    memcpy(pPay_Type,       InPay_Type,      1);
    memcpy(pSim_No,         InSim_No,        20);
    memcpy(pBelong_Code,    InBelong_Code,    7);
    memcpy(pAreaCode,    InAreaCode, 10);
    memcpy(pGroupNo,     InGroupNo, 10);
    memcpy(pSystem_Note,    InSystem_Note,   60);    
    memcpy(pOp_Note,        InOp_Note,       60);    
    memcpy(pLogin_No,       InLogin_No,       6);
    memcpy(pIp_Address,     InIp_Address,    15);
    memcpy(pOp_Code,        InOp_Code,        4);
    memcpy(pInnet_Type,     InInnet_Type,     2);
    memcpy(pMachine_Code,   InMachine_Code,   3);
    memcpy(pAssure_Name,    InAssure_Name,   60);
    memcpy(pId_Type,        InId_Type,       20);    
    memcpy(pAssure_Id,      InAssure_Id,     20);
    memcpy(pAssure_Phone,   InAssure_Phone,  20);
    memcpy(pAssure_Address, InAssure_Address,60);
    memcpy(pAssure_ZIP,     InAssure_ZIP,     6);    
    memcpy(pPost_Flag,      InPost_Flag,      1);
    memcpy(pPost_Type,      InPost_Type,      1);
    memcpy(pPost_Address,   InPost_Address,  200);
    memcpy(pPost_ZIP,       InPost_ZIP,       6);
    memcpy(pPost_Fax,       InPost_Fax,      30);
    memcpy(pPost_Mail,      InPost_Mail,     30);
    memcpy(pPost_Content,      InPost_Content,     1);
    memcpy(pTotal_Date,     InTotal_Date,     8);
    memcpy(pSysDate,        InSysDate,       20);
    
    strcpy(p_Cust_Name , In_Cust_Name );
    strcpy(p_Cust_Address , In_Cust_Address );
    strcpy(p_Id_Type , In_Id_Type );
    strcpy(p_Id_Iccid , In_Id_Iccid );
    strcpy(p_NewOldFlag ,  In_NewOldFlag );
    strcpy(p_In_CustDocFlag ,  In_CustDocFlag );
     
    strncpy(sV_true_flag,InTrue_flag,1);
    strncpy(sIn_Custinfo_Type,sVCustinfoType,2);
    strncpy(sIn_Agreement_Type,sV_Agreement_Type,1);
    
    
            
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cust_Id        = [%s]",   pCust_Id        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Con_Id         = [%s]",   pCon_Id         );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cust_Passwd    = [%s]",   pCust_Passwd    );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Con_Passwd     = [%s]",   pCon_Passwd     );   
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "User_Passwd    = [%s]",   pUser_Passwd    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Limit_Owe      = [%.2f]", pLimit_Owe      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Credit_Value   = [%d]",   pCredit_Value   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Bill_Type      = [%d]",   pBill_Type      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cmd_Right      = [%d]",   pCmd_Right      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Cash_Pay       = [%.2f]", pCash_Pay       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Check_Pay      = [%.2f]", pCheck_Pay      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Prepay_Fee     = [%.2f]", pPrepay_Fee     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sim_Fee        = [%.2f]", pSim_Fee        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Machine_Fee    = [%.2f]", pMachine_Fee    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Innet_Fee      = [%.2f]", pInnet_Fee      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Choice_Fee     = [%.2f]", pChoice_Fee     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Other_Fee      = [%.2f]", pOther_Fee      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Hand_Fee       = [%.2f]", pHand_Fee       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Phone_No       = [%s]",   pPhone_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sm_Code        = [%s]",   pSm_Code        ); 
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pService_Type  = [%s]",   pService_Type   );     
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Attr_Code      = [%s]",   pAttr_Code      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Credit_Code    = [%s]",   pCredit_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Pay_Code       = [%s]",   pPay_Code       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Pay_Type       = [%s]",   pPay_Type       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Sim_No         = [%s]",   pSim_No         );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Belong_Code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "AreaCode    = [%s]",   pAreaCode    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Op_Note        = [%s]",   pOp_Note        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "System_Note    = [%s]",   pSystem_Note    );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "sV_true_flag   = [%s]",   sV_true_flag    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Ip_Address     = [%s]",   pIp_Address     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Op_Code        = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Innet_Type     = [%s]",   pInnet_Type     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Machine_Code   = [%s]",   pMachine_Code   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Name    = [%s]",   pAssure_Name    );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Id_Type        = [%s]",   pId_Type        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_No      = [%s]",   pAssure_Id      );        
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Phone   = [%s]",   pAssure_Phone   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_Address = [%s]",   pAssure_Address );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Assure_ZIP     = [%s]",   pAssure_ZIP     );    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Flag      = [%s]",   pPost_Flag      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Type      = [%s]",   pPost_Type      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Address   = [%s]",   pPost_Address   );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_ZIP       = [%s]",   pPost_ZIP       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Fax       = [%s]",   pPost_Fax       );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Mail      = [%s]",   pPost_Mail      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Post_Content      = [%s]",   pPost_Content      );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Total_Date     = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SysDate        = [%s]",   pSysDate        );
    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_Cust_Name        = [%s]",   p_Cust_Name        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_Cust_Address        = [%s]",   p_Cust_Address        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_Id_Type        = [%s]",   p_Id_Type        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_Id_Iccid        = [%s]",   p_Id_Iccid        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_NewOldFlag        = [%s]",   p_NewOldFlag        );
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "p_In_CustDocFlag        = [%s]",   p_In_CustDocFlag        );
  
#endif

    /* ��ʼ�� */
/*****
 Credit_Code  : �����ȴ���         sCreditCode                 E
 Credit_Value : ������ȡֵ                                     100
 Limit_Owe    : ͸֧���(��ʹ��)   sCreditCode -> OverFee      0
 Attr_Code    : �ͻ�����           Pay_Code||Owner_Code        000
 Cmd_Right    : ���ػ�����Ȩֵ                                 1
 Service_Type : ��������           sServiceType               
 Bill_Type    : ������������       sBillType                  
 Pay_Type     : ĳ�νɷѸ��ʽ   sPayType
 Pay_Code     : �û����ʽ       sPayCode
 *****/

    vPay_Money = pCash_Pay + pCheck_Pay;

    pCredit_Value=0;
    pLimit_Owe=0;
    pCmd_Right=0;
    /*
    strcpy(pCredit_Code,"E");
    */
    /*
    strcpy(pAttr_Code,"000");
    */
    strcpy(pPay_Code,"0");
    strcpy(pPay_Type,"0");

    if(pCheck_Pay>0)
        strcpy(pPay_Type,"1");
        
    if(strcmp(pOp_Code,"1106")==0 || strcmp(pOp_Code,"1108")==0){ /* ������ҵ�� */
        vAssure_No = 999999999999;
    }

    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","ȡ������Ϣ����");                         
             return -1;
         }
	
	strcpy(vGroupId,pGroupNo);
	
    Trim(vOrg_Code);

    EXEC SQL SELECT SUBSTR(:pTotal_Date,1,6) INTO :vDateChar
             FROM   DUAL;
     
    EXEC SQL SELECT DECODE(SUBSTR(:pBelong_Code,1,2), '99',SUBSTR(:vOrg_Code,1,2),SUBSTR(:pBelong_Code,1,2)),
                    DECODE(SUBSTR(:pBelong_Code,3,2), '99',SUBSTR(:vOrg_Code,3,2),SUBSTR(:pBelong_Code,3,2)),
                    DECODE(SUBSTR(:pBelong_Code,5,3), '999',SUBSTR(:vOrg_Code,5,3),SUBSTR(:pBelong_Code,5,3))
               INTO :vRegion_Code,:vDistrict_Code,:vTown_Code
               From dual;
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-2","��ѯdLoginMsg ����");                         
            EXEC SQL ROLLBACK;
            return -2;
        }
/*
    if(strcmp(pSm_Code,"cb")==0)
    {
        EXEC SQL select password into :sV_Cb_passwd from dCustRes where phone_no=:pPhone_No;
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "��ȡ�û�����ʧ��[%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -2;
        }

        Trim(sV_Cb_passwd);
        if(strlen(sV_Cb_passwd)==0)
        {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "�û����벻��Ϊ��");
            EXEC SQL ROLLBACK;
            return -2;
        }

        strcpy(pCust_Passwd,sV_Cb_passwd);
        strcpy(pUser_Passwd,sV_Cb_passwd);
        strcpy(pCon_Passwd,sV_Cb_passwd);
    }
*/
    /*--------------- �ͻ����û���Ϣ ------------------ */

    EXEC SQL INSERT INTO dCustUserMsg
                        (Cust_Id,    
                         Id_No,    Rep_Flag,   Cust_Flag)
                  VALUES(TO_NUMBER(:pCust_Id), 
                         TO_NUMBER(:pId_No), '0',    '0');
            if (SQLCODE != 0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-3","����ͻ��û���ϵ�����");                         
                EXEC SQL ROLLBACK;
                return -3;
            }
    EXEC SQL INSERT INTO dCustUserHis
                        (Cust_Id,  Id_No,    Rep_Flag,   Cust_Flag,
                         update_time,   update_login,   update_type,
                         update_accept, op_code)
                  VALUES(TO_NUMBER(:pCust_Id), TO_NUMBER(:pId_No), '0',    '0',
                         sysdate,       :pLogin_No,     'A',
                         :pLogin_Accept,:pOp_Code);
            if (SQLCODE != 0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-3","����ͻ��û���ϵ��ʷ�����");                         
                EXEC SQL ROLLBACK;
                return -4;
            }
    
    if(strncmp(p_NewOldFlag,"1",1)!=0) /* ���ǲ��Ͽͻ��Ͳ�dCustDoc�� */
    {
        pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "���ǲ��Ͽͻ��Ͳ�dCustDoc��  ");
        
        strcpy(vParent_Id,pId_No);
        EXEC SQL SELECT COUNT(*) INTO :sV_Cust_num FROM dCustDoc where cust_id=:pCust_Id;
        if(sV_Cust_num==0) /* sV_Cust_num>0 ��ʾ������������һ��Ϣ��ֻ����һ�� */
        {
            Trim(p_Cust_Name);
            Trim(p_Cust_Address);
            Trim(p_Id_Iccid);
            if(strlen(p_Cust_Name)==0)
                strcpy(p_Cust_Name," ");
            if(strlen(p_Cust_Address)==0)
                strcpy(p_Cust_Address," ");
            if(strlen(p_Id_Iccid)==0)
                strcpy(p_Id_Iccid," ");
/*
            sprintf(sV_SqlStr,"insert into dCustDoc(Cust_Id,Region_Code,District_Code,Town_Code,Cust_Name,Cust_Passwd,"
                    " Cust_Status,Run_Time,Owner_grade,Owner_Type,Cust_Address,Id_Type,Id_Iccid,"
                    " Id_Address, Id_Validdate,Contact_person,Contact_Phone,Contact_Address,Contact_Post,"
                    " Contact_Mailaddress,Contact_Fax,Contact_Emaill,Org_Code,Create_Time,"
                    " Close_Time,Parent_Id,true_flag,info_type,agreement_type) "
                " VALUES(to_number(:pCust_Id) , :vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,"
                    " '01',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'00','01',:p_Cust_Address,:p_Id_Type,:p_Id_Iccid,"
                    " 'NULL',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'NuLL','Null','Null','Null',"
                    " 'Null','Null','Null',:vOrg_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),"
                    " TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),to_number(:pCust_Id),:sV_true_flag,:sVCustinfoType,:sIn_Agreement_Type) ");
                
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "���� dCustDoc  :sV_SqlStr=[%s]", sV_SqlStr);
            #endif   
       
            EXEC SQL PREPARE ins_stmt From :sV_SqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCust_Id,:vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,
                                            :pSysDate,:p_Cust_Address ,:p_Id_Type,:p_Id_Iccid,:pSysDate,:vOrg_Code,
                                            :pSysDate,:pCust_Id,:sV_true_flag,:sVCustinfoType,:sIn_Agreement_Type; 
        
            if (SQLCODE != 0)
            {
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����ͻ�������ʷ��(dCustDoc)����[%d]",SQLCODE);
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
                EXEC SQL ROLLBACK;
                return -5;
            }
        
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "begin  insert into dCustDocHis");
            init(sV_SqlStr);
      
            sprintf(sV_SqlStr,"insert into dCustDocHis(Cust_Id,Region_Code,District_Code,Town_Code,Cust_Name,Cust_Passwd,"
                    " Cust_Status,Run_Time,Owner_grade,Owner_Type,Cust_Address,Id_Type,Id_Iccid,"
                    " Id_Address, Id_Validdate,Contact_person,Contact_Phone,Contact_Address,Contact_Post,"
                    " Contact_Mailaddress,Contact_Fax,Contact_Emaill,Org_Code,Create_Time,"
                    " Close_Time,Parent_Id,UPDATE_ACCEPT,"
                    " UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE) "
                 " VALUES(to_number(:pCust_Id) , :vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,"
                    " '01',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'00','01',:p_Cust_Address,:p_Id_Type,:p_Id_Iccid,"
                    " 'NULL',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'NuLL','Null','Null','Null',"
                    " 'Null','Null','Null',:vOrg_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),"
                    " TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),to_number(:pCust_Id),to_number(:pLogin_Accept),"
                    " TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),to_number(:pTotal_Date),:pLogin_No,'a',:pOp_Code) ");
            
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "���� dCustDocHis  :sV_SqlStr=[%s]", sV_SqlStr);
            #endif   
        
            EXEC SQL PREPARE ins_stmt From :sV_SqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCust_Id,:vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,
                                            :pCust_Passwd,:pSysDate,:p_Cust_Address ,:p_Id_Type,:p_Id_Iccid,:pSysDate,
                                            :vOrg_Code,:pSysDate,:pCust_Id  ,:pLogin_Accept,:pSysDate,:pTotal_Date,
                                            :pLogin_No,:pOp_Code;  
           
            if (SQLCODE != 0) 
            {
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����ͻ����ϱ�(dCustDocHis) ���� [%d]",SQLCODE);
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
                EXEC SQL ROLLBACK;
                return -6;
            } 
            
 */
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/
	 
	    custDoc.vCust_Id=    atol(pCust_Id);
        strncpy(custDoc.vRegion_Code		,vRegion_Code,2);
        strncpy(custDoc.vDistrict_Code	,vDistrict_Code,2);
        strncpy(custDoc.vTown_Code		,vTown_Code,3);
        strncpy(custDoc.vOrg_Code		,vOrg_Code,9);
        strncpy(custDoc.vGroup_Id		,vGroupId,10);
        strncpy(custDoc.vOrg_Id			,vOrgId,10);
        strncpy(custDoc.vCust_Name		,p_Cust_Name,60);
        strncpy(custDoc.vCust_Passwd		,pCust_Passwd,8);
        strncpy(custDoc.vCust_Status		,"01",2);
        strncpy(custDoc.vRun_Time		,pSysDate,17);
        strncpy(custDoc.vCreate_Time		,pSysDate,17);
        strncpy(custDoc.vClose_Time		,"20500101 00:00:00",17);
        strncpy(custDoc.vOwner_Grade		,"00",2);
        strncpy(custDoc.vOwner_Type		,"01",2);
        strncpy(custDoc.vCust_Addr		,p_Cust_Address,60);
        strncpy(custDoc.vId_Addr			,"NULL",60);
        strncpy(custDoc.vContact_Addr			,"NULL",100);
        strncpy(custDoc.vContact_MailAddr			,"NULL",60);       
        strncpy(custDoc.vId_Type			,p_Id_Type,1);
        strncpy(custDoc.vId_Iccid		,p_Id_Iccid,20);
        strncpy(custDoc.vId_ValidDate	,pSysDate,8);
        strncpy(custDoc.vContact_Person,"NULL",20);
        strncpy(custDoc.vContact_Phone,"NULL",20);
        strncpy(custDoc.vContact_Post,"NULL",6);
        strncpy(custDoc.vContact_Fax,"NUll",30);
        strncpy(custDoc.vContact_Email,"NULL",30);           
        strncpy(custDoc.vParent_Id		,pCust_Id,14);
        strncpy(custDoc.vCreate_Note		,pOp_Note,60);           
        strncpy(custDoc.vOp_Time			,pSysDate,17);           
        strncpy(custDoc.vLogin_No		,pLogin_No,6);
        strncpy(custDoc.vOp_Code			,pOp_Code,4);
        custDoc.vLogin_Accept=  atol(pLogin_Accept);
        custDoc.vTotal_Date=    atoi(pTotal_Date);  
         
        strncpy(custDoc.vTrue_Flag,sV_true_flag,1);
        strncpy(custDoc.vInfo_Type,sVCustinfoType,2);
        strncpy(custDoc.vAgreement_Type,sIn_Agreement_Type,1);
        /*����custDoc.vTrue_Flag,custDoc.vInfo_Type,custDoc.vAgreement_Type�������ģ��δ����ֵ����������뿪��ʧ�ܵ�����*/     
                      
        Vret = 0;
        Vret = createCustDoc_ng(ORDERIDTYPE_USER,pId_No,&custDoc,sErrorMess);
        if(Vret != 0)
        {
	         return -1;
	         
        }   /*������ edit by cencm at 20100414--------begin-------------*/         
           /* if(strncmp(sV_true_flag,"Y",1)==0)
            {
            	EXEC SQL insert into dchncusttrue(ID_NO,CUST_ID,GROUP_ID,OPEN_TIME,
									TOTAL_DATE,OP_TIME,BELONG_CODE,OP_CODE)
					values(TO_NUMBER(:pId_No),to_number(:pCust_Id),:pGroupNo,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
						to_number(:pTotal_Date),to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),:pBelong_Code,:pOp_Code);
				if(SQLCODE!=0)
				{					
			        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
					pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-36","����dchncusttrue����!");                         
                	EXEC SQL ROLLBACK;
                	return -36;
				}
            }*/
            /*������ edit by cencm at 20100414--------end-------------*/
        }    
    }
    
    /* �����ʼ�û�������Ϣ */
/**********2006-10-24 tongqp����**********/
/*NG���� start*/

/*
	EXEC SQL insert into dconmark
				   (CONTRACT_NO,INIT_POINT,CURRENT_POINT,CURRENT_TIME,
				    MONTH_POINT,YEAR_POINT,LASTYEAR_POINT,ADD_POINT,OWE_POINT,LAST_POINT,
				    MONTH_USED,YEAR_USED,TOTAL_USED,MIN_YM,CONTRACT_STATUS,OWE_LIMIT,STATUS_TIME)
             values(TO_NUMBER(:pCon_Id), 0 , 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                	0 , 0 , 0 , 0 , 0 , 0 ,
                	0 , 0 , 0 , TO_char(sysdate,'YYYYMM') , 0 , 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'));
             if (SQLCODE != 0) 
             {
  		          PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-7","���� dconmark �����!");                         
                  EXEC SQL ROLLBACK;
                  return -7;
             }
*/             
			strcpy(sTdConMark.sContractNo,pCon_Id);
			strcpy(sTdConMark.sInitPoint,"0");
			strcpy(sTdConMark.sCurrentPoint,"0");
			strncpy(sTdConMark.sCurrentTime,pSysDate,17);
			strcpy(sTdConMark.sMonthPoint,"0");
			strcpy(sTdConMark.sYearPoint,"0");
			strcpy(sTdConMark.sLastyearPoint,"0");
			strcpy(sTdConMark.sAddPoint,"0");
			strcpy(sTdConMark.sOwePoint,"0");
			strcpy(sTdConMark.sLastPoint,"0");
			strcpy(sTdConMark.sMonthUsed,"0");
			strcpy(sTdConMark.sYearUsed,"0");
			strcpy(sTdConMark.sTotalUsed,"0");
			strncpy(sTdConMark.sMinYm,pSysDate,6);
			strcpy(sTdConMark.sContractStatus,"0");
			strcpy(sTdConMark.sOweLimit,"0");
			strncpy(sTdConMark.sStatusTime,pSysDate,17);
			strcpy(sTdConMark.sChgFlag,"1");	/*09����ϵͳ�������� add by majha at 20091030*/

			ret=OrderInsertConMark(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						    		 pOp_Code, atol(pLogin_Accept), pLogin_No, pOp_Note,
						    		 sTdConMark);
			if (ret !=0){
			   
	          PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-7","���� dconmark �����!");                         
              EXEC SQL ROLLBACK;			   
			  return -7;
			   	
			}
/*NG���� end*/            
/**********2006-10-24 tongqp����**********/

/*************2006-10-24 tongqp����**********
    EXEC SQL insert into dMarkMsg(ID_NO , OPEN_TIME , INIT_POINT , CURRENT_POINT,
                 YEAR_POINT , OP_TIME , BEGIN_TIME , ADD_POINT , TOTAL_USED ,
                 TOTAL_PRIZE , TOTAL_PUNISH , LASTYEAR_POINT , BASE_POINT,SM_CODE)
             values(TO_NUMBER(:pId_No),TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 0 , 0 ,
                 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') , 0 , 0 ,
                 0 , 0 , 0 , 0,:pSm_Code);
             if (SQLCODE != 0) {
                  pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "���� dMarkMsg ����� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
                  EXEC SQL ROLLBACK;
                  return -7;
             }
*************2006-10-24 tongqp����**********/    
    vIds=1;
pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pService_Type=[%s]",pService_Type);
/*
    EXEC SQL INSERT INTO dCustMsgHis
                        (Id_No,          Cust_Id,        Contract_No ,
                         Ids,            Phone_No,       Sm_Code,
                         Attr_Code,      User_Passwd,    Belong_Code, 
                         Limit_Owe,      Credit_Code,    Credit_Value, 
                         Run_Code ,      Run_Time,       Bill_Date,
                         Bill_Type,      Encrypt_Prepay, Cmd_right,   
                         Last_Bill_Type, Bak_Field,      Service_Type,
                         Open_Time,      Update_Accept,  Update_Time,    
                         Update_Login,   Update_Type,    Update_Code,
                         Update_Date, group_id,group_no)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         TO_NUMBER(:pCon_Id),
                        :vIds,         :pPhone_No,     :pSm_Code,
                        :pAttr_Code,   :pUser_Passwd,  :pBelong_Code,
                        :pLimit_Owe,   :pCredit_Code,  :pCredit_Value,
                        'AA',           
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pBill_Type,   'UnKnown',      :pCmd_Right,
                         TO_CHAR(:pBill_Type),   '001',:pService_Type,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         to_number(:pLogin_Accept),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pLogin_No,    'a',            :pOp_Code,
                         TO_NUMBER(:pTotal_Date), nvl(:pAreaCode,'00999'),:pGroupNo);
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����ͻ�������ʷ��dCustMsgHis ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -7;
        }

    EXEC SQL INSERT INTO dCustMsg
                        (Id_No,          Cust_Id,        Contract_No ,
                         Ids,            Phone_No,       Sm_Code,
                         Attr_Code,      User_Passwd,    Belong_Code, 
                         Limit_Owe,      Credit_Code,    Credit_Value, 
                         Run_Code ,      Run_Time,       Bill_Date,
                         Bill_Type,      Encrypt_Prepay, Cmd_right,   
                         Last_Bill_Type, Bak_Field ,     Service_Type,
                         Open_Time, group_id,group_no,group_msg)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         TO_NUMBER(:pCon_Id),
                        :vIds,         :pPhone_No,     :pSm_Code,
                        :pAttr_Code,   :pUser_Passwd,  :pBelong_Code,
                        :pLimit_Owe,   :pCredit_Code,  :pCredit_Value,
                        'AA',           
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pBill_Type,   'UnKnown',      :pCmd_Right,
                         TO_CHAR(:pBill_Type),   '001',:pService_Type,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), nvl(:pAreaCode,'00999'),:pGroupNo,nvl(:pAreaCode,'00999'));
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����dCustMsg ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -8;
        }
*/
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/
pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "liupenga =====���Կ�ʼ");
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pId_No=[%s]",pId_No);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pTotal_Date=[%s]",pTotal_Date);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pLogin_Accept=[%s]",pLogin_Accept);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pSm_Code=[%s]",pSm_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vOrg_Code=[%s]",vOrg_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pPhone_No=[%s]",pPhone_No);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vOrg_Code=[%s]",vOrg_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pLogin_No=[%s]",pLogin_No);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pOp_Code=[%s]",pOp_Code);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pSysDate=[%s]",pSysDate);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pMachine_Code=[%s]",pMachine_Code);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pSystem_Note=[%s]",pSystem_Note);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pOp_Note=[%s]",pOp_Note);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vGroupId=[%s]",vGroupId);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vOrgId=[%s]",vOrgId);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pPay_Type=[%s]",pPay_Type);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pIp_Address=[%s]",pIp_Address);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vPay_Money=[%f]",vPay_Money);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pCash_Pay_2=[%f]",pCash_Pay_2);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pCheck_Pay_2=[%f]",pCheck_Pay_2);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pSim_Fee=[%f]",pSim_Fee);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pMachine_Fee=[%f]",pMachine_Fee);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pInnet_Fee=[%f]",pInnet_Fee);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pChoice_Fee=[%f]",pChoice_Fee);	 
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pOther_Fee=[%f]",pOther_Fee);
	pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pHand_Fee=[%f]",pHand_Fee);
	
pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "liupenga =====���Խ���");
		EXEC SQL select count(*)
						into :juagecount
						from dcustdoc
						where cust_id=to_number(:pCust_Id);
		if (SQLCODE != 0) 
    {
      PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-8","��ѯdcustdoc����!");                                     
      EXEC SQL ROLLBACK;
      return -8;
    }
    if(juagecount==0)
    {
    	EXEC SQL select count(*)
						into :juagecount
						from dcustdochis
						where cust_id=to_number(:pCust_Id);
			if (SQLCODE != 0) 
    	{
    	  PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	  	  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-9","��ѯdcustdoc����!");                                     
    	  EXEC SQL ROLLBACK;
    	  return -9;
    	}
    	if(juagecount==0)
    	{
	  	  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-10","dcustdoc��dcustdochis���޼�¼");                                     
    	  EXEC SQL ROLLBACK;
    	  return -10;
    	}
    }	 
		custStruc.vCust_Id             =atol(pCust_Id); 
		custStruc.vId_No               = atol(pId_No);
		custStruc.vContract_No         =atol(pCon_Id);
		custStruc.vIds                 =vIds;       
		                 
		strncpy(custStruc.vPhone_No     ,  pPhone_No,15);       
		strncpy(custStruc.vSm_Code      ,  pSm_Code,2);       
		strncpy(custStruc.vService_Type ,  pService_Type,2);       
		strncpy(custStruc.vAttr_Code    ,   pAttr_Code ,8);       
		strncpy(custStruc.vUser_Passwd  ,  pUser_Passwd,8);       
		strncpy(custStruc.vOpen_Time    ,  pSysDate,17);       
		strncpy(custStruc.vBelong_Code  ,   pBelong_Code,7);          
		
		custStruc.vLimit_Owe            =pLimit_Owe;
		         
		strncpy(custStruc.vCredit_Code  ,   pCredit_Code,1);         
		
		custStruc.vCredit_Value         =pCredit_Value; 
		
		strncpy(custStruc.vRun_Code    ,  "AA" ,2);        
		strncpy(custStruc.vRun_Time    ,    pSysDate,17);        
		strncpy(custStruc.vBill_Date   ,   pSysDate, 17);        
		
		custStruc.vBill_Type             =pBill_Type;
		         
		strncpy(custStruc.vEncrypt_Prepay , "UnKnown",16);     
		
		custStruc.vCmd_Right              =pCmd_Right; 
		     
		/*strcpy(custStruc.vLast_Bill_Type   ,             );   */
		sprintf(custStruc.vLast_Bill_Type,"%d",pBill_Type);
		strncpy(custStruc.vBak_Field        ,    "001",12);   
		
		custStruc.vLogin_Accept          =atol(pLogin_Accept);  
		
		strncpy(custStruc.vOp_Time          ,  pSysDate,17           );  
		
		custStruc.vTotal_Date                   =atoi(pTotal_Date);  
		 
		strncpy(custStruc.vLogin_No    ,    pLogin_No,6              );        
		strncpy(custStruc.vOp_Code     ,    pOp_Code ,4             );        
		strncpy(custStruc.vGroup_Id    ,    vGroupId,10              );    
		strncpy(custStruc.varea_code   ,    pAreaCode,10              );    
		Vret=0;
	   Vret = createCustMsg(&custStruc,return_mess);
	   if(Vret != 0)
	   	{
		    return -1;
	   	}           
        
      EXEC SQL INSERT INTO dinnetmsg
                        (PHONE_NO,ID_NO,CUST_ID,CONTRACT_NO,SM_CODE,
                        MODE_CODE,OPEN_TIME,GROUP_NO,FLAG)
                  VALUES(:pPhone_No,TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         TO_NUMBER(:pCon_Id),:pSm_Code,'00000000',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pGroupNo,'0');
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-37","����dCustMsg ����!");                                     
            EXEC SQL ROLLBACK;
            return -37;
        }
    
    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 begin*/  
    /*  
    sprintf(sV_SqlStr,"INSERT INTO dCustOweStop%d(ID_NO, CONTRACT_NO, RUN_CODE, RUN_TIME, CALLING_TIMES, "
                      " LOGIN_ACCEPT, AWAKE_TIMES, UNBILL_FEE) "
                      " VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCon_Id),'AA',sysdate,0,0,0,0)",atol(pId_No)%100);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "sV_SqlStr=[%s]",sV_SqlStr);
    EXEC SQL PREPARE ins_stmt From :sV_SqlStr;
    EXEC SQL EXECUTE ins_stmt using :pId_No,:pCon_Id;
    if (SQLCODE != 0) 
    {
        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-8","����dCustOweStop ����!");                                     
        EXEC SQL ROLLBACK;
        return -8;
    }*/
    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 end*/
    
/*******
 ����ϵͳ�ṩ�������� op_no
 Op_Type: 1 Ins , 2 Upd , 3 Del
********/
    EXEC SQL SELECT sMaxBillChg.NEXTVAL INTO :vOp_No
             FROM DUAL;
        if(SQLCODE != 0){
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "ȡ����ӿ���ˮsMaxBillChg,���� [%d]",SQLCODE);
            return -9;
        }

    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "op_no=[%ld]",vOp_No);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "id_no=[%s]",pId_No);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "Parent_Id=[%s]",vParent_Id);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "pCust_Id=[%s]",pCust_Id);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "phone_no=[%s]",pPhone_No);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "sm_code=[%s]",pSm_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "attr_code=[%s]",pAttr_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "belong_code=[%s]",pBelong_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "sysdate=[%s]",pSysDate);

    EXEC SQL INSERT INTO wUserMsgChg
                        (Op_No,        Op_Type,       Id_No,
                         Cust_Id,      Phone_No,      Sm_Code,
                         Attr_Code,    Belong_Code,   Run_Code,
                         Run_Time,     Op_Time,GROUP_ID)
                  VALUES(:vOp_No,     '1',          TO_NUMBER(:pId_No),
                          TO_NUMBER(:pCust_Id),:pPhone_No,:pSm_Code, 
                         :pAttr_Code, :pBelong_Code, 'AA',
                          SUBSTR(:pSysDate,1,8)||SUBSTR(:pSysDate,10,2)
                       ||SUBSTR(:pSysDate,13,2)||SUBSTR(:pSysDate,16,2),
                          SUBSTR(:pSysDate,1,8)||SUBSTR(:pSysDate,10,2)
                       ||SUBSTR(:pSysDate,13,2)||SUBSTR(:pSysDate,16,2),:vGroupId);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-10","����wUserMsgChg ����!");                                     
            EXEC SQL ROLLBACK;
            return -10;
        }

    /*------------ ����Ĭ���ʻ� -----------------*/
    /*
      1) Ĭ���ʻ� Contract_No = Id_No
      2) �ʻ����� Account_Type 
         Ĭ���ʻ��������ʻ���˽���ʻ�������ͣ����������ͣ�� 
                                 S
      3) 
    */

    memcpy(vCon_Cust_Id, pCust_Id, 22);
    if(strlen(p_Cust_Name)==0)
        strcpy(p_Cust_Name,"NULL");
    if(strcmp(pOp_Code,"1104")==0)
    {
        EXEC SQL select cust_name into :p_Cust_Name from dCustDoc where cust_id=TO_NUMBER(:pCust_Id);
    }
    /*
    EXEC SQL INSERT INTO dConMsgHis
                        (Contract_No,    Con_Cust_Id,  Contract_Passwd, 
                         Bank_Cust,      Oddment,      Belong_Code,
                         PrePay_FEE,     Prepay_Time,  Status,  
                         Status_Time,    Post_Flag ,   Deposit ,
                         Min_Ym,         Owe_Fee,      Account_Mark,
                         Account_Limit,  Pay_Code,     Bank_Code, 
                         Post_Bank_Code, Account_No,   Account_Type,
                         Update_Code,    Update_Date,  Update_Time,
                         Update_Login,   Update_Accept,Update_Type)
                  VALUES(TO_NUMBER(:pCon_Id),TO_NUMBER(:vCon_Cust_Id),
                        :pCon_Passwd,
                        :p_Cust_Name,         0,           :pBelong_Code,
                        :pPrepay_Fee,   
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'Y',
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                        :pPost_Flag,   0,
                         SUBSTR(:pSysDate,1,6), 0,   0,
                        'A',           :pPay_Code,   'Null',
                        'Null',        'Null',       'S',
                        :pOp_Code,      TO_NUMBER(:pTotal_Date), 
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pLogin_No,    to_number(:pLogin_Accept),'a');
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����dConMsgHis ���� [%d]",SQLCODE);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -11;
        }

    EXEC SQL INSERT INTO dConMsg  
                        (Contract_No,    Con_Cust_Id,  Contract_Passwd, 
                         Bank_Cust,      Oddment,      Belong_Code,
                         PrePay_FEE,     Prepay_Time,  Status,  
                         Status_Time,    Post_Flag ,   Deposit ,
                         Min_Ym,         Owe_Fee,      Account_Mark,
                         Account_Limit,  Pay_Code,     Bank_Code, 
                         Post_Bank_Code, Account_No,   Account_Type)
                  VALUES(TO_NUMBER(:pCon_Id),TO_NUMBER(:vCon_Cust_Id),
                        :pCon_Passwd,
                        :p_Cust_Name,         0,           :pBelong_Code,
                        :pPrepay_Fee,   
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'Y',
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                        :pPost_Flag,   0,
                         SUBSTR(:pSysDate,1,6), 0,   0,
                        'A',           :pPay_Code,   'Null',
                        'Null',        'Null',       '0');
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����dConMsg ���� [%d]",SQLCODE);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -12;
        }
        */
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/
    
          strncpy(accDoc.vLogin_Accept,      	pLogin_Accept,14);
	      strncpy(accDoc.vContract_No,       	 pCon_Id,14);
	      strncpy(accDoc.vCon_Cust_Id,       	    vCon_Cust_Id,10);
	      strncpy(accDoc.vContract_Passwd,	      pCon_Passwd,8);
	      strncpy(accDoc.vBank_Cust,      	       p_Cust_Name,60);
	      strncpy(accDoc.vBelong_Code,            pBelong_Code,7);
	      strncpy(accDoc.vAccount_Limit,     	         "A",1);
	      strncpy(accDoc.vStatus,            	         "Y",1);
	      strncpy(accDoc.vPost_Flag,         	         pPost_Flag,1);
	      strncpy(accDoc.vPay_Code,          	         pPay_Code,1);
	      strncpy(accDoc.vBank_Code,      	          "Null",5);
	      strncpy(accDoc.vPost_Bank_Code,    	      "Null",5);
	      strncpy(accDoc.vAccount_No,      	        "Null",30);
	      strncpy(accDoc.vAccount_Type,    	           "0",1);
	      strncpy(accDoc.vBegin_Time,      	        pSysDate,17);
	      strncpy(accDoc.vEnd_Time,         	        pSysDate,17);
	      strncpy(accDoc.vOp_Code,           	    pOp_Code,4);
	      strncpy(accDoc.vLogin_No,          	   pLogin_No,6);
	      strncpy(accDoc.vOrg_Code,          	    vOrg_Code,9);
	      strncpy(accDoc.vOp_Note,           	    pOp_Note,60);
	      strncpy(accDoc.vSystem_Note,       	    pSystem_Note,60);
	      strncpy(accDoc.vIp_Addr,           	   "0.0.0.0",15);
	      strncpy(accDoc.vGroup_Id,          	    vGroupId,10);
	      strncpy(accDoc.vUpdate_Type,       	         "a",1);
	      accDoc.vOddment  = 0.00;
	      accDoc.vPrepay_Fee  = pPrepay_Fee;
	      accDoc.vDeposit = 0.00;
	      accDoc.vOwe_Fee = 0.00;
	      accDoc.vAccount_Mark = 0.00;
        
        Vret = 0;
        Vret = createConMsg_ng(ORDERIDTYPE_USER,pId_No,&accDoc,sErrorMess);
        if(Vret != 0)
        {/*
			sprintf(oRetCode, "%06d",Vret);
			RAISE_ERROR(oRetCode,oRetMsg);
			*/
			return -1;
			
        }        

    EXEC SQL select to_char(count(*)) into :vPayFlag from sPackCode where region_code=:vRegion_Code and mach_code=:pMachine_Code;
    if(atoi(vPayFlag)>0) /* ��Ӫ����Ԥ�� */
        strcpy(vPayFlag,"j");
    else if(strcmp(pSm_Code,"d0")==0) /* ���ݿ� */
        strcpy(vPayFlag,"k");
    else
        strcpy(vPayFlag,"0");
    
    vGoodNoCount=0;
	EXEC SQL select count(*)
			into :vGoodNoCount
			from dGoodPhoneRes
			where phone_no=:pPhone_No
			and good_type in (select good_type from sgoodnotype);
	if(SQLCODE != 0) 
	{
        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-39","����wUserMsgChg ����!");                                     
		EXEC SQL ROLLBACK;
		return -39;
    }
    if(vGoodNoCount>0)
    {
    	init(vPayFlag);
    	strcpy(vPayFlag,"J");
    }
    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 begin*/  
    /*     
    EXEC SQL INSERT INTO dConMsgPre(CONTRACT_NO,PAY_TYPE,PREPAY_FEE,LAST_PREPAY,ADD_PREPAY,LIVE_FLAG,ALLOW_PAY,
                    BEGIN_DT,END_DT)
                VALUES(TO_NUMBER(:pCon_Id),:vPayFlag,:pPrepay_Fee,0,:pPrepay_Fee,'0',0,
                    substr(:pSysDate,1,8),'20200101');
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-34","����dConMsgPre ����!");                                     
            EXEC SQL ROLLBACK;
            return -34;
        }
    */        
    /*NG ��ż  ҵ�񹤵����� Update by miaoyl 20091001 end*/
    EXEC SQL INSERT INTO dCustConMsgHis
                        (Cust_Id,         Contract_No,     Begin_Time,
                         End_Time,        Update_Accept,   Update_Login,
                         Update_Date,     Update_Time,     Update_Code,
                         Update_Type)
                  VALUES(TO_NUMBER(:pCust_Id),TO_NUMBER(:pCon_Id),    
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE('20500101','YYYYMMDD'),
                         to_number(:pLogin_Accept),  :pLogin_No,       TO_NUMBER(:pTotal_Date),   
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pOp_Code,      'a');
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����dCustConMsgHis ���� [%d]",SQLCODE);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -13;
        }
    /*NG ���� start*/
    /*
    EXEC SQL INSERT INTO dCustConMsg
                        (Cust_Id,  Contract_No,  Begin_Time,
                         End_Time)
                  VALUES(TO_NUMBER(:pCust_Id),TO_NUMBER(:pCon_Id),    
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE('20500101','YYYYMMDD'));
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-14","����dCustConMsg ����!");                                     
            EXEC SQL ROLLBACK;
            return -14;
        }
    */
    memset(&sTdCustConMsg, 0, sizeof(sTdCustConMsg));
	strncpy(sTdCustConMsg.sCustId,pCust_Id, 10);
	strncpy(sTdCustConMsg.sContractNo,pCon_Id, 14);
	strncpy(sTdCustConMsg.sBeginTime,pSysDate, 17);
	strncpy(sTdCustConMsg.sEndTime,"20500101", 8);
    ret=OrderInsertCustConMsg(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						    pOp_Code,atol(pLogin_Accept),pLogin_No,pOp_Note,		 				   
						    sTdCustConMsg);
	if (ret!=0){
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-14","����dCustConMsg ����!,ret=[%d]",ret);                                     
            EXEC SQL ROLLBACK;
            return -14;		  	
	}	
   /*NG ���� end*/
   EXEC SQL INSERT INTO dConUserMsgHis
                        (Id_No,        Contract_No,  Serial_No,
                         Bill_Order,   Pay_Order,    Begin_YMD,    
                         Begin_TM,     End_YMD,      End_TM,       
                         Limit_Pay,    Rate_Flag,    Stop_Flag,
                         Update_Accept,Update_Login, Update_Date,  
                         Update_Time,  Update_Code,  Update_Type)
                 VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCon_Id),0, 
                        99999999,     1,           to_char(sysdate,'yyyymmdd'),  
                        to_char(sysdate,'hh24miss'),
                       '20500101',   '000000',      
                        0,  'N',          'Y',          
                       to_number(:pLogin_Accept),:pLogin_No,   TO_NUMBER(:pTotal_Date),
                        TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                       :pOp_Code,    'a');
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����dConUserMsgHis ���� [%d]",SQLCODE);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -15;
        }
	/*NG���� start*/
	/*
    EXEC SQL INSERT INTO dConUserMsg
                        (Id_No,      Contract_No,  Serial_No,
                         Bill_Order, Pay_Order,    Begin_YMD,    
                         Begin_TM,   End_YMD,      End_TM,       
                         Limit_Pay,  Rate_Flag,    Stop_Flag)
                 VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCon_Id),0, 
                        99999999,   1,           to_char(sysdate,'yyyymmdd'),  
                        to_char(sysdate,'hh24miss'),
                       '20500101', '000000',      
                        0,'N',      'Y');
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-16","����dConUserMsg ����!");                                     
            EXEC SQL ROLLBACK;
            return -16;
        }
     */
            EXEC SQL select to_char(sysdate,'yyyymmdd'),to_char(sysdate,'HH24MISS')
                       into :sBeginYmd,:sBeginTm
                       from dual;
            if (SQLCODE != 0) {
            	PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            	return -17;
            }	
			strcpy(tdConUserMsg.sIdNo,pId_No);
			strcpy(tdConUserMsg.sContractNo,pCon_Id);
			strcpy(tdConUserMsg.sBillOrder,"99999999");
			strcpy(tdConUserMsg.sSerialNo,"0");
			strcpy(tdConUserMsg.sPayOrder,"1");
			strncpy(tdConUserMsg.sBeginYmd,sBeginYmd,8);
			strncpy(tdConUserMsg.sBeginTm,sBeginTm,6);
			strcpy(tdConUserMsg.sEndYmd,"20500101");
			strcpy(tdConUserMsg.sEndTm,"000000");
			strcpy(tdConUserMsg.sLimitPay,"0");
			strcpy(tdConUserMsg.sRateFlag,"N");
			strcpy(tdConUserMsg.sStopFlag,"Y");
			ret =OrderInsertConUserMsg(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,				
										pOp_Code, atol(pLogin_Accept), pLogin_No, pOp_Note,
										tdConUserMsg);
            if (ret !=0) {
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-16","����dConUserMsg ����!");                                     
	            EXEC SQL ROLLBACK;
	            return -16; 	
            }
     
     /*NG���� end*/
    /** 
     Interest  Ԥ�����Ϣ

     **/

    EXEC SQL INSERT INTO wConInterest
                        (Contract_No,        Total_Date,   
                         Login_Accept,       Interest)
                  VALUES(TO_NUMBER(:pCon_Id),TO_NUMBER(:pTotal_Date), 
                        to_number(:pLogin_Accept),     :vInterest);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-17","����wConInterest ����!");                                     
            EXEC SQL ROLLBACK;
            return -17;
        }


/*******
 ����ϵͳ�ṩ�������� op_no
 Op_Type: 1 Ins , 2 Upd , 3 Del
********/

    EXEC SQL SELECT sMaxBillChg.NEXTVAL INTO :vOp_No
             FROM DUAL;
        if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-18","ȡ����ӿ���ˮsMaxBillChg,����!");                                     
            return -18;
        }
		/**chendh CRM3.5�������ڵ���,sundm���,2012.3.17**/
/**
**    EXEC SQL INSERT INTO wConUserChg
**                        (Op_No,       Op_Type,    Id_No,
**                         Contract_No, Bill_Order, Conuser_Update_Time,
**                         Limit_Pay,   Rate_Flag,  Fee_Code,    
**                         Fee_Rate,   Op_Time,DETAIL_CODE)
**                 VALUES(:vOp_No,     '10',         TO_NUMBER(:pId_No),
**                         TO_NUMBER(:pCon_Id),      99999999,      
**                         to_char(sysdate,'YYYYMMDDHH24MISS'),
**                         0,          'N',      ' ',         
**                         0,         
**                         to_char(sysdate,'YYYYMMDDHH24MISS'),'*');
**        if (SQLCODE != 0) {            
**	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
**		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-19","����wConUserChg ����!");                                     
**            EXEC SQL ROLLBACK;
**            return -19;
**        }
**/
    if(strcmp(pPost_Flag,"1")==0){
        EXEC SQL INSERT INTO dCustPostHis
                            (Constract_No,   Post_Flag,     Post_Type,
                             Post_Address,   Post_ZIP,      Fax_No, post_cont_code,
                             Mail_Address,   Update_Accept, Update_Login, 
                             Update_Date,    Update_Time,   Update_Code,
                             Update_Type)
                      VALUES(TO_NUMBER(:pCon_Id),:pPost_Flag,:pPost_Type,
                             :pPost_Address,:pPost_ZIP,    :pPost_Fax, :pPost_Content,
                             :pPost_Mail,   to_number(:pLogin_Accept),:pLogin_No,
                              TO_NUMBER(:pTotal_Date),  
                              TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                             :pOp_Code,'a');
            if (SQLCODE != 0) {
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-19","����dCustPostHis ����!");                                     
                EXEC SQL ROLLBACK;
                return -20;
            }
    /*NG���� start*/
    /*
        EXEC SQL INSERT INTO dCustPost
                            (Contract_No,    Post_Flag,   Post_Type,
                             Post_Address,   Post_ZIP,    Fax_No,
                             Mail_Address, post_cont_code,op_time)
                      VALUES(TO_NUMBER(:pCon_Id),:pPost_Flag,:pPost_Type,
                             :pPost_Address, :pPost_ZIP,  :pPost_Fax,
                             :pPost_Mail, :pPost_Content,sysdate);
            if (SQLCODE != 0) {
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-21","����dCustPost ����!");                                     
                EXEC SQL ROLLBACK;
                return -21;
            }
     */       
	    strcpy(sTdCustPost.sContractNo,pCon_Id);
		strcpy(sTdCustPost.sPostFlag,pPost_Flag);
		strcpy(sTdCustPost.sPostType,pPost_Type);
		strcpy(sTdCustPost.sPostAddress,pPost_Address);
		strcpy(sTdCustPost.sPostZip,pPost_ZIP);
		strcpy(sTdCustPost.sFaxNo,pPost_Fax);
		strcpy(sTdCustPost.sMailAddress,pPost_Mail);
		strcpy(sTdCustPost.sPostContCode,pPost_Content);
		strcpy(sTdCustPost.sOpTime,pSysDate);
        ret=OrderInsertCustPost(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						    pOp_Code,atol(pLogin_Accept),pLogin_No,pOp_Note,
		 				    sTdCustPost);
	  	if (ret!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-21","����dCustPost ����!");                                     
                EXEC SQL ROLLBACK;
                return -21;	  			
	  	}	
     
     
/*NG���� end*/            
    }

    /* ���µ�����Ϣ���еĵ������� */
    
    if(strlen(ltrim(rtrim(pAssure_Id)))!=0)
    {
    	/*NG���� start*/
    	/*
        EXEC SQL update dAssuMsg set Assure_Num = Assure_Num +1
                 where Assure_Id = :pAssure_Id 
                     and  Id_Type   = :pId_Type;  
                     
        if ((SQLCODE != 0)&&(SQLCODE != 1403))
        {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-22","�޸�dAssuMsg����������!");                                     
            return -22;		
        } 
        */
      EXEC SQL select count(*)
      				into :assure_count
      				from dAssuMsg 
      				where Assure_Id = :pAssure_Id 
      				and Id_Type   = :pId_Type;
     if(SQLCODE != 0)
     {
	      PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-22","��ѯdAssuMsg����������!");
		    printf("��ѯdAssuMsg����������[%d]",SQLCODE);                                     
        return -22;		
     } 
        
  		EXEC SQL DECLARE cur_Msg_01 CURSOR FOR
  		  select to_char(ASSURE_NO) from dAssuMsg where Assure_Id = :pAssure_Id and Id_Type   = :pId_Type;
        EXEC SQL OPEN cur_Msg_01;
	    while (SQLCODE==0){	
	    		init(vAssure_No_NG);
	        	EXEC SQL FETCH cur_Msg_01 INTO :vAssure_No_NG;
		        if ((SQLCODE != 0)&&(SQLCODE != 1403))
		        {
			        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-22","�޸�dAssuMsg����������!");  
				    EXEC SQL CLOSE cur_Msg_01;                                   
		            return -22;		
		        } 
				if (SQLCODE == 1403)  break;
		        strncpy(v_parameter_array[0],vAssure_No_NG,12);
		        strncpy(v_parameter_array[1],pAssure_Id,20);
		        strncpy(v_parameter_array[2],pId_Type,1);
				ret=OrderUpdateAssuMsg(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							   pOp_Code,atol(pLogin_Accept),pLogin_No,pOp_Note,
							   v_parameter_array[0],
							   " Assure_Num = Assure_Num +1 "," and Assure_Id = :pAssure_Id and  Id_Type   = :pId_Type",
							   &v_parameter_array);					   
				if (ret !=0)
				{
			        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-22","�޸�dAssuMsg����������!");    
				    EXEC SQL CLOSE cur_Msg_01;                                 
		            return -22;	
				}
			}
			EXEC SQL CLOSE cur_Msg_01;
		
			
        /*NG���� end*/ 
        if(assure_count==0)
        {
        	 EXEC SQL CLOSE cur_Msg_01;
             /* ���뵣����Ϣ */ 
             EXEC SQL SELECT sMaxAssuNo.NEXTVAL INTO :vAssure_No FROM DUAL;
             if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","ȡ������ vAssure_No,����!");                                     
                 return -1;
             }	
             pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "vAssure_No =[%d] ",vAssure_No); 
             
             EXEC SQL INSERT INTO dAssuMsgHis(
                           Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address,   
                           Assure_ZIP,   Assure_Num,     Update_Accept,  
                           Update_Time,  Update_Login,   Update_Type,    
                           Update_Code,  Update_Date)
                      VALUES(:vAssure_No,  :pAssure_Name,  :pId_Type,
                           :pAssure_Id,  :pAssure_Phone, :pAssure_Address,  
                           :pAssure_ZIP,  1,             to_number(:pLogin_Accept),
                            TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                           :pLogin_No,    'a',           :pOp_Code,
                            TO_NUMBER(:pTotal_Date));
             if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-23","����dAssuMsgHis �����!");                                     
                 return -23;
             }	
             /*NG���� start*/
             /*
             EXEC SQL INSERT INTO dAssuMsg(
                           Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address,   
                           Assure_ZIP,   Assure_Num)
                      VALUES(:vAssure_No,  :pAssure_Name,  :pId_Type,
                           :pAssure_Id,  :pAssure_Phone, :pAssure_Address,  
                           :pAssure_ZIP, 1);   
             if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dAssuMsg �����!");                                     
                 return -24;
             }	*/
			sprintf(sTdAssuMsg.sAssureNo, "%ld", vAssure_No);
			strcpy(sTdAssuMsg.sAssureName, pAssure_Name);		       
			strcpy(sTdAssuMsg.sIdType, pId_Type);		       
			strcpy(sTdAssuMsg.sAssureId, pAssure_Id);		       
			strcpy(sTdAssuMsg.sAssurePhone, pAssure_Phone);		       
			strcpy(sTdAssuMsg.sAssureZip, pAssure_ZIP);		       
			strcpy(sTdAssuMsg.sAssureAddress, pAssure_Address);		       
			strcpy(sTdAssuMsg.sAssureNum, "1");	                
	        ret= OrderInsertAssuMsg(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							    pOp_Code,atol(pLogin_Accept),pLogin_No,pOp_Note,
							    sTdAssuMsg);
	
			if ( ret !=0){
			        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-23","����dAssuMsgHis �����!");                                     
	                 return -23;
			}
        /*NG���� end*/
        }
        else
        {
             EXEC SQL select assure_no into :vAssure_No from dAssuMsg 
                 where Assure_Id = :pAssure_Id 
                     and  Id_Type   = :pId_Type; 
             if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","��ѯdAssuMsg �����!");                                     
                 return -24;
             }
             /* ������ʷ�� */
             EXEC SQL INSERT INTO dAssuMsgHis(
                           Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address,   
                           Assure_ZIP,   Assure_Num,     Update_Accept,  
                           Update_Time,  Update_Login,   Update_Type,    
                           Update_Code,  Update_Date)
                      select Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address,
                           Assure_ZIP , Assure_Num -1 , to_number(:pLogin_Accept),
                           TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                           :pLogin_No,    'a',           :pOp_Code,
                            TO_NUMBER(:pTotal_Date)
                      from  dAssuMsg
                      where Assure_Id = :pAssure_Id 
                         and  Id_Type   = :pId_Type;  
              if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-25","insert dAssuMsgHis �����!");                                     
                 return -25;
              }	  
             
             
        }   
    }  /* if(strlen(ltrim(rtrim(pAssure_Id)))!=0) */

    pShould_pay=pCash_Pay_2+pCheck_Pay_2-pPrepay_Fee; /* ���Ѻϼ�-Ԥ��� */
    if(pCash_Pay_2>=pShould_pay) /* dCustInnet��wChg����֧ƱӦ��Ϊ�㣬�ֽ�=pShould_pay */
    {
        pCash_Pay_2=pShould_pay;
        pCheck_Pay_2=0;
    }
    else
    {
        pCheck_Pay_2=pShould_pay-pCash_Pay_2;
    }

    EXEC SQL INSERT INTO dCustInnetHis
                        (Id_No,          Cust_Id,      Belong_Code,
                         Town_Code,      Innet_Type,   Open_Time,
                         Login_Accept,   Login_No,     Machine_Code,
                         Cash_Pay,       Check_Pay,     Sim_Fee,
                         Machine_Fee,    Innet_Fee,    Choice_Fee,
                         Other_Fee,      Hand_Fee,     Deposit,
                         Back_Flag,      Encrypt_Fee,  System_Note,
                         Op_Note,        Assure_No,    Update_Accept,  
                         Update_Time,    Update_Login, Update_Type,    
                         Update_Code,    Update_Date,GROUP_ID)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         :pBelong_Code,
                        :vTown_Code,    :pInnet_Type, 
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                        to_number(:pLogin_Accept), :pLogin_No,   :pMachine_Code, 
                        :pCash_Pay_2,     :pCheck_Pay_2,  :pSim_Fee,
                        :pMachine_Fee,  :pInnet_Fee,   :pChoice_Fee,
                        :pOther_Fee,    :pHand_Fee,    0,
                        '0',            'UnKnown',    :pSystem_Note,
                        :pOp_Note,      :vAssure_No,  to_number(:pLogin_Accept),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pLogin_No,    'a',            :pOp_Code,
                         TO_NUMBER(:pTotal_Date),:vGroupId);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-26","����dCustInnetHis ����!");                                     
            EXEC SQL ROLLBACK;
            return -26;
        }
    /*NG ���� start*/
    /*
    EXEC SQL INSERT INTO dCustInnet
                        (Id_No,          Cust_Id,      Belong_Code,
                         Town_Code,      Innet_Type,   Open_Time,
                         Login_Accept,   Login_No,     Machine_Code,
                         Cash_Pay,       Check_Pay,     Sim_Fee,
                         Machine_Fee,    Innet_Fee,    Choice_Fee,
                         Other_Fee,      Hand_Fee,     Deposit,
                         Back_Flag,      Encrypt_Fee,  System_Note,
                         Op_Note,        Assure_No,GROUP_ID)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         :pBelong_Code,
                        :vTown_Code,    :pInnet_Type, 
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                        to_number(:pLogin_Accept), :pLogin_No,   :pMachine_Code, 
                        :pCash_Pay_2,     :pCheck_Pay_2,  :pSim_Fee,
                        :pMachine_Fee,  :pInnet_Fee,   :pChoice_Fee,
                        :pOther_Fee,    :pHand_Fee,    0,
                        '0',            'UnKnown',    :pSystem_Note,
                        :pOp_Note,      :vAssure_No,:vGroupId);
        if (SQLCODE != 0) {
            
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-27","����dCustInnet ����!");                                     
            EXEC SQL ROLLBACK;
            return -27;
        }
    */    
    
		strncpy(sTdCustInnet.sIdNo,pId_No,14);
		strncpy(sTdCustInnet.sCustId,pCust_Id,10);
		strncpy(sTdCustInnet.sBelongCode,pBelong_Code,7);
		strncpy(sTdCustInnet.sTownCode,vTown_Code,3);
		strncpy(sTdCustInnet.sInnetType,pInnet_Type,2);
		strncpy(sTdCustInnet.sOpenTime,pSysDate,17);
		strncpy(sTdCustInnet.sLoginAccept,pLogin_Accept,14);
		strncpy(sTdCustInnet.sLoginNo,pLogin_No,6);
		strncpy(sTdCustInnet.sMachineCode,pMachine_Code,3);
		sprintf(sTdCustInnet.sCashPay,"%f",pCash_Pay_2);
		sprintf(sTdCustInnet.sCheckPay,"%f",pCheck_Pay_2);
		sprintf(sTdCustInnet.sSimFee, "%f", pSim_Fee);
		sprintf(sTdCustInnet.sMachineFee, "%f", pMachine_Fee);
		sprintf(sTdCustInnet.sInnetFee, "%f", pInnet_Fee);
		sprintf(sTdCustInnet.sChoiceFee, "%f", pChoice_Fee);
		sprintf(sTdCustInnet.sOtherFee, "%f", pOther_Fee);
		sprintf(sTdCustInnet.sHandFee, "%f", pHand_Fee);
		strcpy(sTdCustInnet.sDeposit, "0");
		strcpy(sTdCustInnet.sBackFlag, "0");
		strcpy(sTdCustInnet.sEncryptFee, "UnKnown");
		strncpy(sTdCustInnet.sSystemNote, pSystem_Note, 60);
		strncpy(sTdCustInnet.sOpNote, pOp_Note, 60);
		sprintf(sTdCustInnet.sAssureNo, "%ld", vAssure_No);
		strncpy(sTdCustInnet.sGroupId, vGroupId, 10);   
        ret=OrderInsertCustInnet(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
						pOp_Code, atol(pLogin_Accept), pLogin_No, pOp_Note,
						sTdCustInnet);
		if (ret!=0){
	      PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-27","����dCustInnet ����!");		  
		  EXEC SQL ROLLBACK;
		  return -27;
		}
	
     /*NG ���� end*/
     
    /*NG ��ż  ҵ�񹤵�����Update by miaoyl 20091001 begin*/ 
    /*
    EXEC SQL INSERT INTO dCustStop
                   (Id_No,     Stop_Times, Stop_Durations,
                    Stop_Time, Open_Time,  Should_Pay)
            VALUES( TO_NUMBER(:pId_No), 0, 0,
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),    
                    0);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-28","����dCustStop ����!");                                     
            EXEC SQL ROLLBACK;
            return -28;
        }*/
    /*NG ��ż  ҵ�񹤵�����Update by miaoyl 20091001 end*/     

    /* ���´�����Դ�� */
    EXEC SQL UPDATE dAgRes
                SET Open_Flag     = 'Y',
                    Open_Time     =  TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    Open_Login    = :pLogin_No,
                    Open_Org      = :vOrg_Code,
                    Open_Date     = :pTotal_Date,
                    Open_Accept   = to_number(:pLogin_Accept)
              WHERE All_No        = :pPhone_No
                 OR All_No        = :pSim_No;
        if (SQLCODE != 0 && SQLCODE != 1403) {
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-29","���� dAgRes ����!");                                     
            EXEC SQL ROLLBACK;
            return -29;
        }


    /* ����wPhoneSim�� */
    EXEC SQL UPDATE wPhoneSim
                SET Open_Flag     = '1'
              WHERE Phone_No      = :pPhone_No
                AND Sim_No        = :pSim_No
                AND Open_Flag     = '0';
        if (SQLCODE != 0) {
            if(SQLCODE == 1403){
                EXEC SQL INSERT INTO wPhoneSim
                               (Phone_No,         Sim_No,         Open_Flag,
                                Login_Accept)
                        VALUES(:pPhone_No,       :pSim_No,       '1',
                               to_number(:pLogin_Accept));
                    if (SQLCODE != 0){
				        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
					    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wPhoneSim ����!");                                     
                        EXEC SQL ROLLBACK;
                        return -1;
                    }
            }
            else{
                
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-30","����wPhoneSim ����!");                                     
                
                EXEC SQL ROLLBACK;
                return -30;
            }
        }

    /*----------------- ��¼��־ -----------------*/
/*****
    ע����õ�ȡֵ
    vAdd_Money
    vPay_Money
    vOut_Prepay
    vPrepay_Fee
    vCurrent_Prepay
    vPayed_Owe
*******/
        /*NG ��ż  ҵ�񹤵�����Update by miaoyl 20091001 begin*/
        /*
        if(pPrepay_Fee > 0)
        {
        	if(vGoodNoCount>0)
		    {
		    	init(pPay_Type);
		    	strcpy(pPay_Type,"J");
		    }
		    
		    
            init(TmpSqlStr); 
         
            sprintf(TmpSqlStr , "insert into wPay%s(CONTRACT_NO, ID_NO, TOTAL_DATE,LOGIN_ACCEPT,"
                    " CON_CUST_ID,PHONE_NO,BELONG_CODE , FETCH_NO , SM_CODE , LOGIN_NO , ORG_CODE ,"
                    " OP_TIME , OP_CODE , PAY_TYPE , ADD_MONEY , PAY_MONEY , OUT_PREPAY , PREPAY_FEE , CURRENT_PREPAY , "
                    " PAYED_OWE , DELAY_FEE , OTHER_FEE , DEAD_FEE , BAD_FEE , BACK_FLAG , ENCRYPT_FEE , oRetCode , PAY_NOTE,ORG_ID,GROUP_ID )  "
                    " values(to_number(:pCon_Id) , to_number(:pId_No), to_number(:pTotal_Date), to_number(:pLogin_Accept) , "
                    " to_number(:vCon_Cust_Id) ,:pPhone_No, :pBelong_Code, '000000', :pSm_Code, :pLogin_No, :vOrg_Code,"
                    " TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Code,:pPay_Type, to_number(:pPrepay_Fee) , "
                    " to_number(:pPrepay_Fee), 0 , to_number(:pPrepay_Fee) , 0, "
                    " 0 , 0 , 0 , 0 , 0 , '0' , 'NULL' , '0' ,:pOp_Note,:vOrgId,:vGroupId)",vDateChar);
        
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����wPay%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
        
            EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCon_Id ,:pId_No ,:pTotal_Date  ,:pLogin_Accept ,:vCon_Cust_Id, :pPhone_No,
                                            :pBelong_Code,:pSm_Code,:pLogin_No ,:vOrg_Code,:pSysDate,:pOp_Code,
                                            :pPay_Type , :pPrepay_Fee , :pPrepay_Fee, :pPrepay_Fee , :pOp_Note,:vOrgId,:vGroupId;  
        
            if (SQLCODE != 0) {
		        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
			    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-31","����wPay%s ����!",vDateChar);                                     
                EXEC SQL ROLLBACK;
                return -31;
            }
        } */ 
        /*NG ��ż  ҵ�񹤵�����Update by miaoyl 20091001 end*/  
    /********** 2 �� Machine_Fee **********/
/*
    init(TmpSqlStr);

    sprintf(TmpSqlStr, "INSERT INTO wChg%s (ID_NO,TOTAL_DATE,LOGIN_ACCEPT,SM_CODE,BELONG_CODE,PHONE_NO,"
            " ORG_CODE,LOGIN_NO,OP_CODE,OP_TIME,MACHINE_CODE,CASH_PAY,CHECK_PAY,SIM_FEE,MACHINE_FEE,"
            " INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE) "
            " VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pSm_Code,:pBelong_Code,:pPhone_No,"
            " :vOrg_Code,:pLogin_No,:pOp_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pMachine_Code,"
            " to_number(:pCash_Pay_2),to_number(:pCheck_Pay_2),to_number(:pSim_Fee),to_number(:pMachine_Fee),"
            " to_number(:pInnet_Fee),to_number(:pChoice_Fee),to_number(:pOther_Fee),to_number(:pHand_Fee),0,'0','UnKnown',:pSystem_Note,:pOp_Note)",vDateChar);

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����wChg%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
        #endif

    EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
    EXEC SQL EXECUTE ins_stmt using :pId_No,:pTotal_Date,:pLogin_Accept,:pSm_Code,:pBelong_Code,:pPhone_No,:vOrg_Code,
                                    :pLogin_No,:pOp_Code,:pSysDate,:pMachine_Code,:pCash_Pay_2,:pCheck_Pay_2,:pSim_Fee,
                                    :pMachine_Fee,:pInnet_Fee,:pChoice_Fee,:pOther_Fee,:pHand_Fee,:pSystem_Note,:pOp_Note;
    if (SQLCODE != 0) {
        pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����wChg%s ���� [%d]" ,vDateChar,SQLCODE);
        pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
        EXEC SQL ROLLBACK;
        return -32;
    }
    */
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/

    

    vLog.vId_No                            =  atol(pId_No);
    vLog.vTotal_Date                       =   atoi(pTotal_Date);
    vLog.vLogin_Accept                     =  atol(pLogin_Accept);

	 strncpy(vLog.vSm_Code,       pSm_Code,2);
	 strncpy(vLog.vBelong_Code,   pBelong_Code,7);
	 strncpy(vLog.vPhone_No,      pPhone_No,15);
	 strncpy(vLog.vOrg_Code,      vOrg_Code,9);
	 strncpy(vLog.vLogin_No,      pLogin_No,6);
	 strncpy(vLog.vOp_Code,       pOp_Code,4);
	 strncpy(vLog.vOp_Time,       pSysDate,17);
	 strncpy(vLog.vMachine_Code,  pMachine_Code,3);
	 strncpy(vLog.vBack_Flag,     "0",1);
	 strncpy(vLog.vEncrypt_Fee,   "0",16);
	 strncpy(vLog.vSystem_Note,   pSystem_Note,60 );
	 strncpy(vLog.vOp_Note,       pOp_Note,60 );
	 strncpy(vLog.vGroup_Id,      vGroupId,10);
	 strncpy(vLog.vOrg_Id,        vOrgId,10);
	 Trim(pPay_Type);
	 strcpy(vLog.vPay_Type,       pPay_Type);  /*�������*/
	 strncpy(vLog.vIp_Addr,       pIp_Address,15);
	 vLog.vPay_Money              = vPay_Money;
	 vLog.vCash_Pay               = pCash_Pay_2;
	 vLog.vCheck_Pay          											   = pCheck_Pay_2;
	 vLog.vSim_Fee            											   = pSim_Fee;
	 vLog.vMachine_Fee        											   = pMachine_Fee;
	 vLog.vInnet_Fee          											   = pInnet_Fee;
	 vLog.vChoice_Fee         											   = pChoice_Fee;
	 vLog.vOther_Fee           											 = pOther_Fee;
	 vLog.vHand_Fee           											   = pHand_Fee;
	 vLog.vDeposit            											   = 0;
   

   
   
   Vret = 0;
   strcpy(vChgFlag,"P");
   strcpy(vCustFlag,"N");
  
     pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "========%ld====",vLog.vId_No); 
  
   Vret = recordOprLog(&vLog,return_mess,vChgFlag,vCustFlag);
   if(Vret != 0)
   	{
	    return -1;
	    
   	}       
    
    if ((strcmp(pOp_Code,"1114")!=0)&&(strcmp(pOp_Code,"1116")!=0) && (strcmp(pOp_Code,"1118")!=0))
    {
    	/* ��������������� �� ����˱� */
    	/*
        init(TmpSqlStr);
        sprintf(TmpSqlStr,"INSERT INTO wLoginOpr%s (TOTAL_DATE , LOGIN_ACCEPT , OP_CODE , PAY_TYPE , PAY_MONEY,SM_CODE ,"
                " ID_NO , PHONE_NO , ORG_CODE , LOGIN_NO , OP_TIME , OP_NOTE , IP_ADDR ) "
            	" VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pOp_Code,:pPay_Type,to_number(:vPay_Money),:pSm_Code,"
                " TO_NUMBER(:pId_No),:pPhone_No,:vOrg_Code,:pLogin_No,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Note,:pIp_Address)", vDateChar);
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����wLoginOpr%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
        
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pTotal_Date,:pLogin_Accept,:pOp_Code,:pPay_Type,:vPay_Money,:pSm_Code,:pId_No,
                                        :pPhone_No,:vOrg_Code,:pLogin_No,:pSysDate,:pOp_Note,:pIp_Address;
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "����wLoginOpr%s ���� [%d]",vDateChar,SQLCODE);
            pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "SQLERRMSG = [%s]",SQLERRMSG);
            EXEC SQL ROLLBACK;
            return -33;
        }
        */
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/

    vLog.vId_No                            =  atol(pId_No);
    vLog.vTotal_Date                       =   atol(pTotal_Date);
    vLog.vLogin_Accept                     =  atol(pLogin_Accept);

	 strncpy(vLog.vSm_Code,       pSm_Code,2);
	 strncpy(vLog.vBelong_Code,   vOrg_Code,7);
	 strncpy(vLog.vPhone_No,      pPhone_No,15);
	 strncpy(vLog.vOrg_Code,      vOrg_Code,9);
	 strncpy(vLog.vLogin_No,      pLogin_No,6);
	 strncpy(vLog.vOp_Code,       pOp_Code,4);
	 strncpy(vLog.vOp_Time,       pSysDate,17);
	 strncpy(vLog.vMachine_Code,  pMachine_Code,3);
	 strncpy(vLog.vBack_Flag,     "0",1);
	 strncpy(vLog.vEncrypt_Fee,   "0",16);
	 strncpy(vLog.vSystem_Note,   pSystem_Note,60 );
	 strncpy(vLog.vOp_Note,       pOp_Note,60 );
	 strncpy(vLog.vGroup_Id,      vGroupId,10);
	 strncpy(vLog.vOrg_Id,        vOrgId,10);
	 Trim(pPay_Type);
	 strcpy(vLog.vPay_Type,       pPay_Type); /*�������*/
	 strncpy(vLog.vIp_Addr,       pIp_Address,15);
	 vLog.vPay_Money              = vPay_Money;
	 vLog.vCash_Pay               = pCash_Pay_2;
	 vLog.vCheck_Pay          	  = pCheck_Pay_2;
	 vLog.vSim_Fee            	  = pSim_Fee;
	 vLog.vMachine_Fee        	  = pMachine_Fee;
	 vLog.vInnet_Fee          	  = pInnet_Fee;
	 vLog.vChoice_Fee         	  = pChoice_Fee;
	 vLog.vOther_Fee              = pOther_Fee;
	 vLog.vHand_Fee           	  = pHand_Fee;
	 vLog.vDeposit            	  = 0;
   
     Vret = 0;
     strcpy(vChgFlag,"N");
     strcpy(vCustFlag,"N");
  
     pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "========%ld====",vLog.vId_No); 
  
     Vret = recordOprLog(&vLog,return_mess,vChgFlag,vCustFlag);
	   if(Vret != 0)
	   	{
		    return -1;
		    
	   	}           
    } /* end --  if (strcmp(pOp_Code,"1114")!=0)  */

       
/******* û�д����˱�
    EXEC SQL INSERT wSimSalesDay

    EXEC SQL INSERT wMsisdnOpen
********/
    
    pubLog_Debug(_FFL_, "pubOpenAcctConfirm", "", "-------- �û�����ȷ�� pubOpenAcctConfirm() END --------");

    return 0;
}
/**
  * Auth: PeiJH
  * Name: pubOpenAcctFav
  * Func: �û������Żݴ���
  * Date: 2003/10/14
  */
int pubOpenAcctFav(char *InId_No,       char *InPhone_No,    char *InSm_Code,     
                   char *InBelong_Code, char *InFavour_Code, char *InShould_Data,
                   char *InActual_Data, char *InLogin_No,    char InLogin_Accept[23],
                   char *InOp_Note,     char *InSystem_Note, char *InOp_Code,     
                   char *InTotal_Date,  char *InSysDate,char *vOrgId)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  pId_No[22+1];
       char  pPhone_No[15+1];
       char  pSm_Code[2+1];
       char  pFavour_Code[4+1];
       char  pShould_Data[17+1];      /*  �Ż�ǰ����   */
       char  pActual_Data[17+1];      /*  �Żݺ�����   */
       char  pBelong_Code[7+1];                     
       char  pLogin_No[6+1];
       char  pLogin_Accept[23];
       char  pSystem_Note[60+1];
       char  pOp_Note[60+1];       
       char  pOp_Code[4+1];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1]; 
       char  vOrg_Code[9+1];
       float vShould_Pay=0.00;
       float vActual_Pay=0.00;
       char  vShould_Bill[17+1];      
       char  vActual_Bill[17+1];      
       char  vFavour_Type[1+1];       /*  �Ż����ͣ�0 �����Ż� ��1 ʱ���Ż� */
       
       int ret=0;
       /*char  vOrgId[10+1];*/
    EXEC SQL END   DECLARE SECTION;

    init(pPhone_No);   init(pSm_Code);
    init(pBelong_Code);init(pLogin_No);  init(pOp_Note);   
    init(pOp_Code);    init(pTotal_Date);init(pSysDate);    
    init(vOrg_Code);   init(pId_No);     init(pFavour_Code);
    init(pShould_Data);init(pActual_Data);
    init(vShould_Bill);init(vActual_Bill);
    init(vFavour_Type);  init(pLogin_Accept);
	/*init(vOrgId);*/
    memcpy(pLogin_Accept,   InLogin_Accept,  22);
    memcpy(pId_No,          InId_No,         22);
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pSm_Code,        InSm_Code,        2);
    memcpy(pBelong_Code,    InBelong_Code,    7);
    memcpy(pFavour_Code,    InFavour_Code,    4);
    memcpy(pShould_Data,    InShould_Data,   17);
    memcpy(pActual_Data,    InActual_Data,   17);            
    memcpy(pLogin_No,       InLogin_No,       6);
    memcpy(pSystem_Note,    InSystem_Note,   60);    
    memcpy(pOp_Note,        InOp_Note,       60);
    memcpy(pOp_Code,        InOp_Code,        4);
    memcpy(pTotal_Date,     InTotal_Date,     8);
    memcpy(pSysDate,        InSysDate,       20);
    
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "---------- �û������Żݴ��� pubOpenAcctFav() BEGIN --------");
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Id_No        = [%s]",pId_No);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Phone_No     = [%s]",pPhone_No);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Sm_Code      = [%s]",pSm_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Belong_Code  = [%s]",pBelong_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "pFavour_Code = [%s]",pFavour_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "pShould_Data = [%s]",pShould_Data);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "pActual_Data = [%s]",pActual_Data);            
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Login_No     = [%s]",pLogin_No);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Login_Accept = [%s]",pLogin_Accept);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "System_Note  = [%s]",pSystem_Note);    
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Op_Note      = [%s]",pOp_Note);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Op_Code      = [%s]",pOp_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "Total_Date   = [%s]",pTotal_Date);
    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "SysDate      = [%s]",pSysDate);

    /* Ӧ��Ϊ��϶��������Ż� */
    if(strcmp(pShould_Data,"0")==0)
        return 0;
        
    strcpy(vFavour_Type,"0"); 

    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){

	        PUBLOG_DBDEBUG("pubOpenAcctFav");
		    pubLog_DBErr(_FFL_,"pubOpenAcctFav","-31","ȡ������Ϣ����!");                                     
             EXEC SQL ROLLBACK;
             return -1;
         }

    if(strlen(pShould_Data) != 17){
    	strcpy(vFavour_Type,"0");
    	vShould_Pay = atof(pShould_Data);
    	vActual_Pay = atof(pActual_Data);
    }
    else if(strlen(pShould_Data) == 17){
    	strcpy(vFavour_Type,"1");
    	memcpy(vShould_Bill,pShould_Data,17);
    	memcpy(vActual_Bill,pActual_Data,17);
    }
    /** ��֯�������� liuhao    200090331 begin*/     
	/*ret = sGetLoginOrgid(pLogin_No,vOrgId);
	if(ret <0)
	{
		return -1;

	}
	Trim(vOrgId);
	*/
	/** ��֯�������� liuhao    200090331 end*/
    if(strcmp(vFavour_Type,"0")==0){
        EXEC SQL INSERT INTO wLoginFav
                            (Total_Date,      Login_Accept,       Favour_Code,
                             Org_Code,        Login_No,           Op_Code,
                             Op_Time,         Sm_Code,            Id_No,
                             Phone_No,        Should_Pay,         Actual_Pay,
                             Should_Bill,     Actual_Bill,        Op_Note, ORG_ID)
                     VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pFavour_Code,
                            :vOrg_Code,      :pLogin_No,         :pOp_Code,
                             TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                            :pSm_Code,       :pId_No,
                            :pPhone_No,      :vShould_Pay,       :vActual_Pay,
                             NULL,            NULL,               :pOp_Note, :vOrgId);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctFav");
		    pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","����wLoginFav ����!");                                     
            
            EXEC SQL ROLLBACK;
            return -1;
        }
              
    }

    if(strcmp(vFavour_Type,"1")==0){
        EXEC SQL INSERT INTO wLoginFav
                            (Total_Date,      Login_Accept,       Favour_Code,
                             Org_Code,        Login_No,           Op_Code,
                             Op_Time,         Sm_Code,            Id_No,
                             Phone_No,        Should_Pay,         Actual_Pay,
                             Should_Bill,     Actual_Bill,        Op_Note,ORG_ID)
                     VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pFavour_Code,
                            :vOrg_Code,      :pLogin_No,         :pOp_Code,
                             TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                            :pSm_Code,       :pId_No,
                            :pPhone_No,       0,                  0,
                             TO_DATE(:vShould_Bill,'YYYYMMDD HH24:MI:SS'),   
                             TO_DATE(:vActual_Bill,'YYYYMMDD HH24:MI:SS'),
                            :pOp_Note,:vOrgId);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctFav");
		    pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","����wLoginFav ����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }
        
    }    


    pubLog_Debug(_FFL_, "pubOpenAcctFav", "", "---------- �û������Żݴ��� pubOpenAcctFav() END --------");    
    
    return 0;
}

   
/**
  * Auth: PeiJH
  * Name: pubOpenAcctSendCmd
  * Func: �û��������ػ�����
  * Date: 2003/10/14
  */
int pubOpenAcctSendCmd(char *InId_No,       char *InPhone_No,    char *InAttr_Code,
                       char *InSm_Code,     char *InBelong_Code, char *InLogin_No,
                       char InLogin_Accept[23],char *InOp_Note,     char *InSystem_Note,
                       char *InOp_Code,     char *InTotal_Date,  char *InSysDate,
                       char sIn_Offon_acceptA[22+1])
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  pId_No[22+1];
       char  pPhone_No[15+1];
       char  pAttr_Code[8+1];
       char  pSm_Code[2+1];
       char  pBelong_Code[7+1];
       char  pLogin_No[6+1];
       char  pLogin_Accept[23];
       char  pSystem_Note[60+1];       
       char  pOp_Note[60+1];
       char  pOp_Code[4+1];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1]; 
       char  vOrg_Code[9+1];
       int   vCommand_Id=0;
       int   vCmd_Right=0;
       char sIn_Offon_accept[22+1];
       
       char vOrgId[10+1];
       char vGroupId[10+1];
    EXEC SQL END   DECLARE SECTION;

    init(pPhone_No);   init(pAttr_Code); init(pSm_Code);
    init(pBelong_Code);init(pLogin_No);  init(pOp_Note);   
    init(pOp_Code);    init(pTotal_Date);init(pSysDate);    
    init(vOrg_Code);   init(pId_No);     init(pSystem_Note);
    init(sIn_Offon_accept);              init(pLogin_Accept);
	init(vOrgId);      init(vGroupId);

    memcpy(pLogin_Accept,   InLogin_Accept,  22);
    memcpy(pId_No,          InId_No,         22);
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pAttr_Code,      InAttr_Code,      8);
    memcpy(pSm_Code,        InSm_Code,        2);
    memcpy(pBelong_Code,    InBelong_Code,    7);
    memcpy(pLogin_No,       InLogin_No,       6);
    memcpy(pSystem_Note,    InSystem_Note,   60);    
    memcpy(pOp_Note,        InOp_Note,       60);
    memcpy(pOp_Code,        InOp_Code,        4);
    memcpy(pTotal_Date,     InTotal_Date,     8);
    memcpy(pSysDate,        InSysDate,       20);
    strcpy(sIn_Offon_accept,sIn_Offon_acceptA);
    
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "---------- �û��������ػ����� pubOpenAcctSendCmd() BEGIN --------");
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Id_No       = [%s]",pId_No);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Phone_No    = [%s]",pPhone_No);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Attr_Code   = [%s]",pAttr_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Sm_Code     = [%s]",pSm_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Belong_Code = [%s]",pBelong_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Login_No    = [%s]",pLogin_No);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Login_Accept= [%s]",pLogin_Accept);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "System_Note = [%s]",pSystem_Note);    
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Op_Note     = [%s]",pOp_Note);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Op_Code     = [%s]",pOp_Code);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "Total_Date  = [%s]",pTotal_Date);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "SysDate     = [%s]",pSysDate);
    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "sIn_Offon_accept     = [%s]",sIn_Offon_accept);


    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubOpenAcctSendCmd");
		    pubLog_DBErr(_FFL_,"pubOpenAcctSendCmd","-1","ȡ������Ϣ����!");                                     
             EXEC SQL ROLLBACK;
             return -1;
         }
    
    /*
    EXEC SQL SELECT sOffon.nextval  INTO :vCommand_Id
               FROM DUAL;
    */
     
  

    EXEC SQL INSERT INTO wChgList
                        (Command_Id,
                         Id_No,         Total_Date,     Login_Accept,
                         Belong_Code,   Sm_Code,        Attr_Code,
                         Phone_No,      Run_Code,       Old_Time,
                         Op_Time,       Org_Code,       Login_No,
                         Op_Code,       Cmd_Right,      Change_Reason)
                 VALUES(to_number(:sIn_Offon_accept),
                         TO_NUMBER(:pId_No),TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),
                        :pBelong_Code, decode(:pSm_Code,'z0','cb',:pSm_Code),      :pAttr_Code,
                        :pPhone_No,    'AA',           
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),     
                        :vOrg_Code,     :pLogin_No,
                        decode(:pOp_Code, '1108','1106', '1118', '1116',:pOp_Code),     :vCmd_Right,    :pSystem_Note);
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubOpenAcctSendCmd");
		    pubLog_DBErr(_FFL_,"pubOpenAcctSendCmd","-1","����wChgList ����!");                                     

            EXEC SQL ROLLBACK;
            return -1;
        }

    pubLog_Debug(_FFL_, "pubOpenAcctSendCmd", "", "---------- �û��������ػ����� pubOpenAcctSendCmd() END --------");
    return 0;

}

/**
  * Auth: FuJian
  * Name: pubSMAPConfirm
  * Func: ��������������
  * Date: 2003/12/25
  */
int pubSMAPConfirm(char *InPhone_No,     char *InOp_Code,     char *InSysDate,
                   char *InBelong_Code,  char *InLogin_No)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  pPhone_No[15+1];
       char  pOp_Code[4+1];
       char  pSysDate[20+1]; 
       char  pBelong_Code[7+1];
       char  pLogin_No[6+1];
       
       char  vLogin_Accept[22+1];
       char  cmdtext[255];
       int   vdelay_time=0;        /* �ӳ�ʱ�� */
       char  oRetMsg[61];       /*������Ϣ*/ 
       char  vBack_Code[4+1];      /* ���������ش��� */
       char  vBack_Desc[255+1];    /* �������������� */
       int   vret_code=0;
       int   i=0;

    EXEC SQL END   DECLARE SECTION;
    
    init(pPhone_No);    init(pOp_Code);     init(pSysDate);
    init(pBelong_Code); init(vLogin_Accept);init(cmdtext);
    init(oRetMsg);   init(vBack_Code);   init(vBack_Desc);

    memcpy(pPhone_No,     InPhone_No,      15);
    memcpy(pOp_Code,      InOp_Code,        4);
    memcpy(pSysDate,      InSysDate,       20);
    memcpy(pBelong_Code,  InBelong_Code,    7);
    memcpy(pLogin_No,     InLogin_No,       6);
    
    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "---------- ����������� pubSMAPConfirm() BEGIN --------");

    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "pPhone_No     = [%s]",   pPhone_No      );
    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "pOp_Code      = [%s]",   pOp_Code       );    
    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "pSysDate      = [%s]",   pSysDate       );
    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "pBelong_Code  = [%s]",   InBelong_Code  );    

    /* ��SMAP�������� */
    EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :vLogin_Accept FROM DUAL;
             if(SQLCODE!=0){
		        PUBLOG_DBDEBUG("pubSMAPConfirm");
			    pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","ȡϵͳ��ˮ����!");                                     
                 EXEC SQL ROLLBACK;
                 return -1;
             }
    if(strncmp(pOp_Code,"1104",4)==0) /* ���� */
        sprintf(cmdtext,"aaaaa");
    else if(strncmp(pOp_Code,"1105",4)==0) /* �������� */
        sprintf(cmdtext,"aaaaa");
    else
    {
        pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "�����ڵ�OP_CODE!");
        return -1;
    }
       
    vret_code = pubSmapSend(pLogin_No, pOp_Code, pBelong_Code,cmdtext,"vall_no",vdelay_time,vLogin_Accept,oRetMsg);

    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "loginaccept:%s",vLogin_Accept);
    if ( vret_code > 0 )
    {
        pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "����smap_send��ʧ��![%d]",vret_code);
        pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "SQLERRMSG = [%s]",oRetMsg);
        EXEC SQL ROLLBACK;
        return -1;
    }
    else{
        EXEC SQL COMMIT WORK;
    }

    /* �ع�,���ٵȴ����� */
    if(strncmp(pOp_Code,"1105",4)==0)
        return 0;
        
    /*  ��SMAP_BACKȡ�÷�����Ϣ */
    i=1;
    while(i<=vdelay_time){
        pubSmapBack(pLogin_No, pOp_Code, vLogin_Accept,
                    vBack_Code,vBack_Desc );
        if(strcmp(vBack_Code,"ZZZZ")==0){
            i++;sleep(1);
        }
        else break;
    }

    if(strcmp(vBack_Code,"ZZZZ")==0){
        EXEC SQL ROLLBACK;
        EXEC SQL delete Smap_send where login_accept=to_number(:vLogin_Accept);
        EXEC SQL COMMIT WORK;
        return -1;
    }
    else if(strcmp(vBack_Code,"0000")!=0){
        EXEC SQL ROLLBACK;
        return -1;
    }
    pubLog_Debug(_FFL_, "pubSMAPConfirm", "", "---------- ����������� pubSMAPConfirm() END --------");
    return 0;
}

/**
  * Auth: PeiJH
  * Name: pubSZXConfirm
  * Func: �����п���ȷ�ϲ���
  * Date: 2003/10/14
  */
int pubSZXConfirm(char *InCust_Id,      char *InId_No,      char *InCust_Passwd,
                  char *InUser_Passwd,  char *InCust_Name,  char *InCust_Address,
                  char *InId_Type,      char *InId_Iccid,   char *InPhone_No,   
                  char *InSim_No,       char *InBelong_Code,char *InIp_Address, 
                  float InCash_Pay,     float InCheck_Pay,  float InSim_Fee,    
                  float InMachine_Fee,  float InChoice_Fee, float InHand_Fee,   
                  char *InLogin_No,     char InLogin_Accept[23],char *InOp_Note,    
                  char *InSystem_Note,  char *InOp_Code,    char *InTotal_Date, 
                  char *InSysDate,char *InTrue_flag)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  TmpSqlStr[1024];
       char  pCust_Id[22+1];       
       char  pId_No[22+1];
       char  pPhone_No[15+1];
       char  pCust_Passwd[6+1];
       char  pUser_Passwd[6+1];       
       char  pCust_Name[60+1];
       char  pCust_Address[60+1];
       char  pId_Type[2+1];
       char  pId_Iccid[20+1];
       char  pSim_No[20+1];
       char  pBelong_Code[7+1];
       char  pIp_Address[15+1];
       float pCash_Pay=0;
       float pCheck_Pay=0;
       float pSim_Fee=0;
       float pMachine_Fee=0;
       float pChoice_Fee=0;
       float pHand_Fee=0;
       char  pLogin_No[6+1];
       char  pLogin_Accept[23];
       char  pSystem_Note[60+1];       
       char  pOp_Note[60+1];
       char  pOp_Code[4+1];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1]; 
       char  vOrg_Code[9+1];
       char  vImsi_No[20+1];
       char  vBill_Code[8+1];
       char  vDateChar[6+1];
       float vPay_Money=0;
       char  vRegion_Code[2+1];
       char  vDistrict_Code[2+1];
       char  vTown_Code[3+1];
       char  vSm_Code[2+1];
       char  vPay_Type[4+1]; /*�������*/
       char  cmdtext[255];
       char  vLogin_Accept[22+1];
       int   vret_code=0;
       int   vdelay_time=0;        /* �ӳ�ʱ�� */
       char  oRetMsg[61];       /*������Ϣ*/ 
       int   i=0;
       char  vBack_Code[4+1];      /* ���������ش��� */
       char  vBack_Desc[255+1];    /* �������������� */
           char  vGroupId[10+1];
           char  vYearMonth[6+1];
       
       char sV_true_flag[1+1];   /*ʵ����ʶ Y or N*/
       
    EXEC SQL END   DECLARE SECTION;

    init(TmpSqlStr);    init(pCust_Id);     init(pId_No);       
    init(pCust_Passwd); init(pUser_Passwd); init(pPhone_No);
    init(pSim_No);      init(vSm_Code);     init(pBelong_Code);
    init(vPay_Type);    init(pIp_Address);  init(pLogin_No);
    init(pOp_Note);     init(pOp_Code);     init(pTotal_Date);
    init(pSysDate);     init(vOrg_Code);    init(vImsi_No);
    init(vBill_Code);   init(vDateChar);    init(pSystem_Note);
    init(vRegion_Code); init(vDistrict_Code);init(vTown_Code);
    init(vPay_Type);    init(vSm_Code);     init(pCust_Name);
    init(pCust_Address);init(pId_Type);     init(pId_Iccid);
    init(pLogin_Accept);init(cmdtext);      init(oRetMsg);
    init(vLogin_Accept);init(vBack_Code);   init(vBack_Desc);
	init(vGroupId); init(vYearMonth);init(sV_true_flag);
    pCash_Pay      =      InCash_Pay;
    pCheck_Pay     =      InCheck_Pay;
    pSim_Fee       =      InSim_Fee;
    pMachine_Fee   =      InMachine_Fee;
    pChoice_Fee    =      InChoice_Fee;
    pHand_Fee      =      InHand_Fee;
    memcpy(pLogin_Accept, InLogin_Accept,   22);
    memcpy(pCust_Id,      InCust_Id,       22);
    memcpy(pId_No,        InId_No,         22);    
    memcpy(pCust_Passwd,  InCust_Passwd,    6);    
    memcpy(pUser_Passwd,  InUser_Passwd,    6);    
    memcpy(pCust_Name,    InCust_Name,     60);
    memcpy(pCust_Address, InCust_Address,  60);
    memcpy(pId_Type,      InId_Type,        2);
    memcpy(pId_Iccid,     InId_Iccid,      20);                
    memcpy(pPhone_No,     InPhone_No,      15);
    memcpy(pSim_No,       InSim_No,        20);
    memcpy(pBelong_Code,  InBelong_Code,    7);
    memcpy(pIp_Address,   InIp_Address,    15);
    memcpy(pLogin_No,     InLogin_No,       6);
    memcpy(pSystem_Note,  InSystem_Note,   60);    
    memcpy(pOp_Note,      InOp_Note,       60);
    memcpy(pOp_Code,      InOp_Code,        4);
    memcpy(pTotal_Date,   InTotal_Date,     8);
    memcpy(pSysDate,      InSysDate,       20);
    strncpy(sV_true_flag,InTrue_flag,1);
    
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "---------- �����п���ȷ�ϴ��� pubSZXConfirm() BEGIN --------");

    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Cust_Id       = [%s]",   pCust_Id       );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Id_No         = [%s]",   pId_No         );    
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Cust_Passwd   = [%s]",   pCust_Passwd   );    
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "User_Passwd   = [%s]",   pUser_Passwd   );    
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Cust_Name     = [%s]",   pCust_Name   );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Cust_Address  = [%s]",   pCust_Address   );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Id_Type       = [%s]",   pId_Type   );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Id_Iccid      = [%s]",   pId_Iccid   );                
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Phone_No      = [%s]",   pPhone_No      );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Sim_No        = [%s]",   pSim_No        );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Belong_Code   = [%s]",   pBelong_Code   );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Ip_Address    = [%s]",   pIp_Address    );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Cash_Pay      = [%.2f]", pCash_Pay      );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Check_Pay     = [%.2f]", pCheck_Pay     );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Sim_Fee       = [%.2f]", pSim_Fee       );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Machine_Fee   = [%.2f]", pMachine_Fee   );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Choice_Fee    = [%.2f]", pChoice_Fee    );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Hand_Fee      = [%.2f]", pHand_Fee      );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Login_No      = [%s]",   pLogin_No      );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Login_Accept  = [%s]",   pLogin_Accept  );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "System_Note   = [%s]",   pSystem_Note   );    
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Op_Note       = [%s]",   pOp_Note       );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Op_Code       = [%s]",   pOp_Code       );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "Total_Date    = [%s]",   pTotal_Date    );
    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "SysDate       = [%s]",   pSysDate       );

    strcpy(vSm_Code,"z0");
    strcpy(vPay_Type,"0");
    strcpy(vBill_Code,"UnKnown");
    vPay_Money=pCheck_Pay + pCash_Pay;

    EXEC SQL SELECT Org_Code, to_char(sysdate,'yyyymm')  INTO :vOrg_Code, :vYearMonth
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
        if(SQLCODE!=0){
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","ȡ������Ϣ����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL SELECT SUBSTR(:pTotal_Date,1,6) INTO :vDateChar
             FROM   DUAL;
    
    EXEC SQL SELECT DECODE(SUBSTR(:pBelong_Code,1,2),
                    '99',SUBSTR(Org_Code,1,2),
                    SUBSTR(:pBelong_Code,1,2)),
                    DECODE(SUBSTR(:pBelong_Code,3,2),
                    '99',SUBSTR(Org_Code,3,2),
                    SUBSTR(:pBelong_Code,3,2)),
                    DECODE(SUBSTR(:pBelong_Code,5,3),
                    '999',SUBSTR(Org_Code,5,3),
                    SUBSTR(:pBelong_Code,5,3)),
                    Org_Code,org_id /*group_id -> org_id edit by liuweib 20091023*/
               INTO :vRegion_Code,:vDistrict_Code,:vTown_Code,:vOrg_Code,:vGroupId
               FROM dLoginMsg
              WHERE Login_No = :pLogin_No;
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","��ѯdLoginMsg ���� !");                                     
            
            EXEC SQL ROLLBACK;
            return -1;
        }

    /* ������Դ */   
    EXEC SQL SELECT Imsi_No INTO :vImsi_No
             FROM   dSimRes
             WHERE  Sim_No=:pSim_No;
        if(SQLCODE!=0){            
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","��ѯdSimResȡImsi_No����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }                                
        /* lixg add begin */
        memset(TmpSqlStr, 0, sizeof(TmpSqlStr));
        sprintf(TmpSqlStr,"INSERT INTO wCustResOpr%s "
                                "( LOGIN_ACCEPT, TOTAL_DATE, NO_TYPE, BEGIN_NO, "
                                " END_NO, PASSWORD, OTHER_ATTR, CHOICE_FEE, LIMIT_PREPAY, "
                                " BEGIN_TIME, OLD_GROUP, OLD_LOGIN, NEW_GROUP, NEW_LOGIN, "
                                " RESOURCE_CODE, OP_TIME, OP_CODE, LOGIN_NO, ORG_GROUP)"
                                " select to_number(:v1), to_number(:v2), no_type, phone_no,"
                                " phone_no, password, other_attr, choice_fee, LIMIT_PREPAY,"
                                " BEGIN_TIME, group_id, login_no, :v3, :v4,"
                                " '1', to_char(to_date(:v5, 'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'), :v6, :v7, :v8 "
                                " from dCustRes where phone_no = :v9", vYearMonth);
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr;
        EXEC SQL EXECUTE ins_stmt using :pLogin_Accept,:pTotal_Date,
                            :vGroupId,:pLogin_No,
                                                        :pSysDate,:pOp_Code,:pLogin_No,:vGroupId,:pPhone_No;
        if (SQLCODE != 0) {
#ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubSZXConfirm", "", "lixg2 TmpSqlStr =[%s][%d]", TmpSqlStr, SQLCODE);
#endif
            EXEC SQL ROLLBACK;
            return -34;
        }

    EXEC SQL UPDATE dCustRes
             SET Resource_Code= '1',          
                 Login_No     = :pLogin_No,
                 Login_Accept = to_number(:pLogin_Accept), 
                 Op_Time      =  TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                 Total_Date   =  TO_NUMBER(:pTotal_Date),     
                 Org_Code     = :vOrg_Code,
                 Region_Code  = DECODE(SUBSTR(:pBelong_Code,1,2),
                               '99', SUBSTR(:vOrg_Code,1,2),
                                SUBSTR(:pBelong_Code,1,2)),
                 District_Code= DECODE(SUBSTR(:pBelong_Code,3,2),
                               '99', SUBSTR(:vOrg_Code,3,2),
                                SUBSTR(:pBelong_Code,3,2)),
                 Town_Code    = DECODE(SUBSTR(:pBelong_Code,5,3),
                               '999', SUBSTR(:vOrg_Code,5,3),
                                SUBSTR(:pBelong_Code,5,3)),
                 group_id = :vGroupId
             WHERE Phone_No   =:pPhone_No;
        if(SQLROWS!=1){
            pubLog_Debug(_FFL_, "pubSZXConfirm", "", "���º�����Դ�����![%d]",SQLCODE);
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","���º�����Դ�����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }
    /* by zd 20090720 ��¼dsimreshis���Ա�����ҵ�ԭ������Ϣ*/
    
        	   EXEC SQL insert into dsimreshis(SIM_NO,PHONENO_HEAD,IMSI_NO,PIN1,PUK1,PIN2,PUK2,ASK_NO,GET_MODE,
							SIM_STATUS,SIM_TYPE,REGION_CODE,DISTRICT_CODE,TOWN_CODE,OP_CODE,OP_TIME,LOGIN_NO,LOGIN_ACCEPT,
							TOTAL_DATE,PHONE_NO,UPDATE_LOGIN,UPDATE_ACCEPT,UPDATE_DATE,UPDATE_TIME,
							UPDATE_CODE,UPDATE_TYPE,GROUP_ID)
							select SIM_NO,PHONENO_HEAD,IMSI_NO,'','','','',0,0,
							SIM_STATUS,SIM_TYPE,REGION_CODE,DISTRICT_CODE,TOWN_CODE,OP_CODE,OP_TIME,LOGIN_NO,LOGIN_ACCEPT,
							TOTAL_DATE,PHONE_NO,:pLogin_No,to_number(:pLogin_Accept),TO_NUMBER(:pTotal_Date),TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
								:pOp_Code,'u',group_id
              from dSimRes
              WHERE Sim_No =:pSim_No;
					    if(SQLCODE!=0){					        
					 	     PUBLOG_DBDEBUG("pubOpenAcctRes");
							 pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����dsimreshis��ʧ��!");            

					        return -1;
					      }
					        	
    
    /*by zd end 20090720*/
    
    EXEC SQL UPDATE dSimRes
                SET Sim_Status   ='1',
                    /* Phoneno_Head =substr(:pPhone_No,1,7), */
                    Login_No     =:pLogin_No,
                    Login_Accept =to_number(:pLogin_Accept),
                    Op_Time      = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    Total_Date   = TO_NUMBER(:pTotal_Date),
                    Region_Code  =DECODE(SUBSTR(:pBelong_Code,1,2),
                                 '99', SUBSTR(:vOrg_Code,1,2),
                                  SUBSTR(:pBelong_Code,1,2)),
                    District_Code=DECODE(SUBSTR(:pBelong_Code,3,2),
                                 '99', SUBSTR(:vOrg_Code,3,2),
                                  SUBSTR(:pBelong_Code,3,2)),
                    Town_Code    =DECODE(SUBSTR(:pBelong_Code,5,3),
                                 '999',SUBSTR(:vOrg_Code,5,3),
                                  SUBSTR(:pBelong_Code,5,3)),
                    op_code		=:pOp_Code
                    /*              ,
                    Phone_No     =:pPhone_No
                    */
              WHERE Sim_No =:pSim_No; 
        if(SQLROWS!=1){

	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����SIM����Դ�����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }
        
    Trim(pCust_Name);
    Trim(pCust_Address);
    Trim(pId_Iccid);
    if(strlen(pCust_Name)==0)
        strcpy(pCust_Name," ");
    if(strlen(pCust_Address)==0)
        strcpy(pCust_Address," ");
    if(strlen(pId_Iccid)==0)
        strcpy(pId_Iccid," ");

    EXEC SQL INSERT INTO dCustDocHis
                        (Cust_Id,        Region_Code,    District_Code,
                         Town_Code,      Cust_Name,      Cust_Passwd,
                         Cust_Status,    Run_Time,       Owner_grade,
                         Owner_Type,     Cust_Address,   Id_Type,
                         Id_Iccid,       Id_Address,     Id_Validdate,
                         Contact_person, Contact_Phone,  Contact_Address,
                         Contact_Post,   Contact_Mailaddress,
                         Contact_Fax,    Contact_Emaill, Org_Code,
                         Create_Time,    CLOSE_TIME,     Parent_Id,      
                         Update_Accept,  Update_Time,    Update_Login,   
                         Update_Type,    Update_Code,    Update_Date)
                  VALUES(TO_NUMBER(:pCust_Id),:vRegion_Code,:vDistrict_Code,
                         :vTown_Code,   :pCust_Name,    :pCust_Passwd, 
                         'Z',            
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'Z',
                         'Z',           :pCust_Address, :pId_Type,
                         :pId_Iccid,    'Null',          
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         'NuLL',        'Null',         'Null',
                         'Null',        'Null',
                         'Null',        'Null',         :vOrg_Code,
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),        
                          TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),        
                          TO_NUMBER(:pCust_Id),          to_number(:pLogin_Accept), 
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pLogin_No,     'a',            :pOp_Code,
                          TO_NUMBER(:pTotal_Date));
            if (SQLCODE != 0)
            {
            	if (strcmp(pOp_Code,"1118")!=0)
            	{
            	    /* ��������������������ظ���¼������ */
                    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "����ͻ�������ʷ��(dCustDocHis)����[%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);                
                    EXEC SQL ROLLBACK;
                    return -1;
                }
            }

    EXEC SQL INSERT INTO dCustDoc
                        (Cust_Id,        Region_Code,    District_Code,
                         Town_Code,      Cust_Name,      Cust_Passwd,
                         Cust_Status,    Run_Time,       Owner_grade,
                         Owner_Type,     Cust_Address,   Id_Type,
                         Id_Iccid,       Id_Address,     Id_Validdate,
                         Contact_person, Contact_Phone,  Contact_Address,
                         Contact_Post,   Contact_Mailaddress,
                         Contact_Fax,    Contact_Emaill, Org_Code,
                         Create_Time,    Close_Time,     Parent_Id,true_flag)
                  VALUES(TO_NUMBER(:pCust_Id),:vRegion_Code,:vDistrict_Code,
                         :vTown_Code,   :pCust_Name,    :pCust_Passwd, 
                         'Z',            
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'Z',
                         'Z',           :pCust_Address, :pId_Type,
                         :pId_Iccid,    'Null',          
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         'NuLL',         'Null',         'Null',
                         'Null',         'Null',
                         'Null',         'Null',         :vOrg_Code,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),        
                         TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),        
                         TO_NUMBER(:pCust_Id),TRIM(:sV_true_flag));
            
            if (SQLCODE != 0)
            {
            	if (strcmp(pOp_Code,"1118")!=0)
            	{
            	    /* ��������������������ظ���¼������ */
			        PUBLOG_DBDEBUG("pubSZXConfirm");
				    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����ͻ�������ʷ��(dCustDoc)����!");                                     
                    
                    EXEC SQL ROLLBACK;
                    return -1;
                }
            }
            
    EXEC SQL INSERT INTO dSimMsgHis
                        (Id_No,       Cust_Id,      Phone_No,    
                         Sim_No,      Imsi_No,     Sm_Code,     
                         Open_Time,   Expire_Time, Prepay_Fee,  
                         Prepay_Rate, Owe_Fee,     Password,    
                         Run_Code,    Run_Time,    Belong_Code, 
                         Bill_Code,   Begin_Time,  Bak_1,       
                         Bak_2,       Bak_3,       Update_Login,
                         Update_Accept,Update_Date,Update_Time, 
                         Update_Code, Update_Type)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                        :pPhone_No,   
                        :pSim_No,    :vImsi_No,    :vSm_Code,  
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),
                         0,            0,
                         0,           :pUser_Passwd,'AA',
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pBelong_Code,:vBill_Code,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        'Null',        0,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pLogin_No,  to_number(:pLogin_Accept), TO_NUMBER(:pTotal_Date),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pOp_Code,   'a');
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����dSimMsgHis����!");                                     
            
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL INSERT INTO dSimMsg
                        (Id_No,       Cust_Id,      Phone_No,    
                         Sim_No,      Imsi_No,     Sm_Code,     
                         Open_Time,   Expire_Time, Prepay_Fee,  
                         Prepay_Rate, Owe_Fee,     Password,    
                         Run_Code,    Run_Time,    Belong_Code, 
                         Bill_Code,   Begin_Time,  Bak_1,       
                         Bak_2,       Bak_3)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                        :pPhone_No,   
                        :pSim_No,    :vImsi_No,    :vSm_Code,  
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),
                         0,            0,
                         0,           :pUser_Passwd,'AA',
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        :pBelong_Code,:vBill_Code,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        'Null',        0,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'));
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����dSimMsgHis����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }

    /* ���´�����Դ�� */
    EXEC SQL UPDATE dAgRes
                SET Open_Flag     = 'Y',
                    Open_Time     =  TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                    Open_Login    = :pLogin_No,
                    Open_Org      = :vOrg_Code,
                    Open_Date     = :pTotal_Date,
                    Open_Accept   = to_number(:pLogin_Accept)
              WHERE All_No        = :pPhone_No
                 OR All_No        = :pSim_No;
        if (SQLCODE != 0 && SQLCODE != 1403) {
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","���� dAgRes ���� !");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }

    /* ����wPhoneSim�� */
    EXEC SQL UPDATE wPhoneSim
                SET Open_Flag     = '1'
              WHERE Phone_No      = :pPhone_No
                AND Sim_No        = :pSim_No
                AND Open_Flag     = '0';
        if (SQLCODE != 0) {        	
            if(SQLCODE == 1403){
                EXEC SQL INSERT INTO wPhoneSim
                               (Phone_No,         Sim_No,         Open_Flag,
                                Login_Accept)
                        VALUES(:pPhone_No,       :pSim_No,       '1',
                               to_number(:pLogin_Accept));
                    if (SQLCODE != 0){
				        PUBLOG_DBDEBUG("pubSZXConfirm");
					    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wPhoneSim ����!");                                     
                        EXEC SQL ROLLBACK;
                        return -1;
                    }
            }
            else{
		        PUBLOG_DBDEBUG("pubSZXConfirm");
			    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wPhoneSim ����!");                                     
                EXEC SQL ROLLBACK;
                return -1;
            }
        }

    /* ����wChgYYYYMM */
    init(TmpSqlStr);

    sprintf(TmpSqlStr,"INSERT INTO wChg%s (ID_NO,TOTAL_DATE,LOGIN_ACCEPT,SM_CODE,BELONG_CODE,PHONE_NO,"
            " ORG_CODE,LOGIN_NO,OP_CODE,OP_TIME,MACHINE_CODE,CASH_PAY,CHECK_PAY,SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,"
            " OTHER_FEE,HAND_FEE,DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,"
            " OP_NOTE) "
        	" VALUES(to_number(:pId_No),to_number(:pTotal_Date),to_number(:pLogin_Accept),:vSm_Code,:pBelong_Code,:pPhone_No,"
            " :vOrg_Code,:pLogin_No,:pOp_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),' ',to_number(:pCash_Pay), "
            " to_number(:pCheck_Pay),to_number(:pSim_Fee),to_number(:pMachine_Fee),0,to_number(:pChoice_Fee),"
            " 0,to_number(:pHand_Fee),0,'0','UnKnown',:pSystem_Note,:pOp_Note)",vDateChar);

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubSZXConfirm", "", "����wChg%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
        #endif

        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pId_No,:pTotal_Date,:pLogin_Accept,:vSm_Code,:pBelong_Code,:pPhone_No,
                                        :vOrg_Code,:pLogin_No,:pOp_Code,:pSysDate,:pCash_Pay,:pCheck_Pay,:pSim_Fee,
                                        :pMachine_Fee,:pChoice_Fee,:pHand_Fee,:pSystem_Note,:pOp_Note;

    if (SQLCODE != 0) {
        PUBLOG_DBDEBUG("pubSZXConfirm");
	    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wChg%s ����!",vDateChar);                                     
        EXEC SQL ROLLBACK;
        return -1;
    }

    /* ����wLoginOprYYYYMM */
    if (strcmp(pOp_Code,"1118")!=0)
    {
        init(TmpSqlStr);
        sprintf(TmpSqlStr,"INSERT INTO wLoginOpr%s  (TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,PAY_TYPE,PAY_MONEY,SM_CODE,"
                " ID_NO,PHONE_NO,ORG_CODE,LOGIN_NO,OP_TIME,OP_NOTE,IP_ADDR) "
            	" VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pOp_Code,:vPay_Type,to_number(:vPay_Money),:vSm_Code,"
                " TO_NUMBER(:pId_No),:pPhone_No,:vOrg_Code,:pLogin_No,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Note,:pIp_Address)", vDateChar);
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, "pubSZXConfirm", "", "����wLoginOpr%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
        
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pTotal_Date,:pLogin_Accept,:pOp_Code,:vPay_Type,:vPay_Money,:vSm_Code,:pId_No,
                                         :pPhone_No,:vOrg_Code,:pLogin_No,:pSysDate,:pOp_Note,:pIp_Address;
        
        if (SQLCODE != 0) {
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wLoginOpr%s ����!",vDateChar);                                     
            EXEC SQL ROLLBACK;
            return -1;
        }
    }

    /* ����wSimChgDay */
    EXEC SQL INSERT INTO wSimChgDay
                        (Id_No,         Total_Date,       Login_Accept,
                         Sm_Code,       Belong_Code,      Phone_No,
                         Num,           
                         Org_Code,      Login_No,
                         Op_Code,       Op_Time,          Machine_Code,
                         Pay_Type,      Cash_Pay,         Check_Pay,
                         Sim_Fee,       Machine_Fee,      Innet_Fee,
                         Choice_Fee,    Other_Fee,        Hand_Fee,
                         Deposit,       Prepay_Fee,       Back_Flag,
                         System_Note,   Op_Note)
                  VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),
                         :vSm_Code,    :pBelong_Code,    :pPhone_No,
                          1,           :vOrg_Code,       :pLogin_No,
                         :pOp_Code,    
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),        
                         '999',
                         '0',          :vPay_Money,       0,
                         :pSim_Fee,     0.00,             0.00,
                         :pChoice_Fee,  0.00,             :pHand_Fee,
                          0.00,         0.00,             '0',
                         :pSystem_Note,:pOp_Note);                        
        if (SQLCODE != 0) {
            pubLog_Debug(_FFL_, "pubSZXConfirm", "", "����wSimChgDay ���� [%d]",SQLCODE);
	        PUBLOG_DBDEBUG("pubSZXConfirm");
		    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wSimChgDay ����!");                                     
            EXEC SQL ROLLBACK;
            return -1;
        }
           
/*********
wSimSalesDay

wMsisdnOpen
*********/

    pubLog_Debug(_FFL_, "pubSZXConfirm", "", "---------- �����п���ȷ�ϴ��� pubSZXConfirm() END --------");

    return 0;
}

  /**
  * Auth: YuanBY
  * Name: pubDefaultFunc
  * Func: ���Ĭ���ط�
  * �������    
  *     ���뻺����  ( Tuxedo �������� )
  *     ���������  ( Tuxedo �������� )
  *     ��ʱ�����ַ���   ( Tuxedo �������� )
  *     ���ش�����Ϣ
  *
  * Date: 2003/10/14
  * Stat: Test Pass
  */
  
int pubDefaultFunc(
      char sIn_User_idA[23] , char sIn_Login_noA[7] ,
      char sIn_Belong_CodeA[8] , char sIn_Sm_CodeA[3] ,
      char sIn_Total_dateA[21] , char sIn_Sys_dateA[21] ,
      char sIn_Login_acceptA[39] , char sIn_Op_codeA[5] ,
      char * sErrMessA , char sIn_Mode_codeA[9] ,
      tFuncEnv *funcEnv)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_User_id[22+1];            /* �û�ID       */
        char    sIn_Login_no[6+1];            /* ��������     */
        char    sIn_Belong_Code[7+1];         /* ��������     */
        char    sIn_Sm_Code[2+1];             /* ҵ������     */
        char    sIn_Total_date[20+1];         /* ��������     */
        char    sIn_Sys_date[20+1];           /* ϵͳ����     */
        char    sIn_Login_accept[38+1];       /* ������ˮ     */
        char    sIn_Op_code[4+1];             /* ��������     */
        char    sIn_Mode_code[9];             /* ���ʷѴ���   */
        
        char    sV_Region_code[2+1];          /* ��������     */
        char    sV_District_code[2+1];        /* ���ش���     */
        char    sV_Town_code[3+1];            /* �������     */
        char    sV_Function_type[1+1];        /* �ط�����     */
        char    sV_Function_code[2+1];        /* �ط�����     */
        char    sV_Add_flag[1+1];             /* ���ӱ�־     */
        int     sV_flag=0;
		
    EXEC SQL END DECLARE SECTION;
    
    int ret=0;
    
    pubLog_Debug(_FFL_, "pubDefaultFunc", "", "--------------- begin function pubDefaultFunc ----------");
    
    /* ���Ի����� */
    init(sIn_User_id);
    init(sIn_Login_no);
    init(sIn_Belong_Code);
    init(sIn_Sm_Code);
    init(sIn_Total_date);
    init(sIn_Sys_date);
    init(sIn_Login_accept);
    init(sIn_Op_code);
    init(sIn_Mode_code);
    
    init(sV_Region_code);
    init(sV_District_code);
    init(sV_Town_code);
    init(sV_Function_type);
    init(sV_Function_code);
    init(sV_Add_flag);
    
    /* ȡ��������� */
    strcpy(sIn_User_id,sIn_User_idA);
    strcpy(sIn_Login_no,sIn_Login_noA);
    strcpy(sIn_Belong_Code,sIn_Belong_CodeA);
    strcpy(sIn_Sm_Code,sIn_Sm_CodeA);
    strcpy(sIn_Total_date,sIn_Total_dateA);
    strcpy(sIn_Sys_date,sIn_Sys_dateA);
    strcpy(sIn_Login_accept,sIn_Login_acceptA);
    strcpy(sIn_Op_code,sIn_Op_codeA);
    strcpy(sIn_Mode_code,sIn_Mode_codeA);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Belong_Code      =[%s]",      sIn_Belong_Code);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Sm_Code      =[%s]",      sIn_Sm_Code);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Total_date      =[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Login_accept      =[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sIn_Op_code      =[%s]",      sIn_Op_code);
    #endif
    
    /* ���  �������� */
    EXEC SQL SELECT substr(:sIn_Belong_Code,1,2),substr(:sIn_Belong_Code,3,2),substr(:sIn_Belong_Code,5,3)
             INTO :sV_Region_code,:sV_District_code,:sV_Town_code FROM dual;
    if(SQLCODE!=0){
        sprintf(sErrMessA,"%s","��ֹ�������ʧ��!");
        PUBLOG_DBDEBUG("pubDefaultFunc");
	    pubLog_DBErr(_FFL_,"pubDefaultFunc","-1",sErrMessA);                                     
        
        return -1;
    } 
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]",
        sV_Region_code,sV_District_code,sV_Town_code); 
    #endif
    
    /* ��ѯĬ�� */
    EXEC SQL DECLARE c102 CURSOR for 
        select FUNCTION_TYPE,FUNCTION_CODE , ADD_FLAG 
        from sFuncList 
        where REGION_CODE = :sV_Region_code
            and SM_CODE = :sIn_Sm_Code
            and DEFAULT_ADD_FLAG = '1'
            and show_flag='Y'
            and function_code not in(
        select b.function_code from sBillModeDetail a,sBillFuncBind b
        where a.region_code = :sV_Region_code and a.mode_code = :sIn_Mode_code
            and a.detail_code=b.FUNCTION_BIND
            and a.detail_type='a' and a.region_code=b.region_code);

    EXEC SQL OPEN c102 ;
    EXEC SQL FETCH c102 INTO :sV_Function_type , :sV_Function_code , :sV_Add_flag;
    if (SQLCODE==1403)
    {
    	strcpy(sErrMessA,"SUCCESSED!");
        PUBLOG_DBDEBUG("pubDefaultFunc");
	    pubLog_DBErr(_FFL_,"pubDefaultFunc","0",sErrMessA);                                     
    	
    	return 0;
    }
        
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
    	sprintf(sErrMessA,"%s","��ѯsFuncList��ʧ��!");

        PUBLOG_DBDEBUG("pubDefaultFunc");
	    pubLog_DBErr(_FFL_,"pubDefaultFunc","-2",sErrMessA);                                     
    	
        return -2;
    }
    
    while(SQLCODE == 0)
    {
        Trim(sV_Function_type);
        Trim(sV_Function_code);
        Trim(sV_Add_flag);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sV_Function_type      =[%s]",      sV_Function_type);
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sV_Function_code      =[%s]",      sV_Function_code);
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "sV_Add_flag      =[%s]",      sV_Add_flag);
        #endif
        
        EXEC SQL select count(*) into :sV_flag from dCustFunc where id_no=to_number(:sIn_User_id) and function_code=:sV_Function_code;
        if(sV_flag>0)
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "aaaaaaaaaaaasV_Function_code=[%s]",sV_Function_code);
        else
        {
        /* ���� dCustFunc �� */
        EXEC SQL insert into dCustFunc(ID_NO , FUNCTION_TYPE , FUNCTION_CODE , OP_TIME )
                 values(to_number(:sIn_User_id),:sV_Function_type,:sV_Function_code,
                        TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'));
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFunc��ʧ��!");
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
	        PUBLOG_DBDEBUG("pubDefaultFunc");
		    pubLog_DBErr(_FFL_,"pubDefaultFunc","-3",sErrMessA);                                     
            
            return -3;
        } 
        
        /* ���� dCustFuncHis �� */
        EXEC SQL insert into dCustFuncHis(ID_NO , FUNCTION_TYPE , FUNCTION_CODE , OP_TIME,
                     UPDATE_LOGIN ,UPDATE_ACCEPT ,UPDATE_DATE , UPDATE_TIME ,
                     UPDATE_CODE , UPDATE_TYPE )
                 values(to_number(:sIn_User_id),:sV_Function_type,:sV_Function_code,
                        TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                        :sIn_Login_no, to_number(:sIn_Login_accept),
                        to_number(:sIn_Total_date), 
                        TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                        :sIn_Op_code , 'a');
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFuncHis��ʧ��!");
	        PUBLOG_DBDEBUG("pubDefaultFunc");
		    pubLog_DBErr(_FFL_,"pubDefaultFunc","-4",sErrMessA);                                     

            return -4;
        } 
        }
        /* ���� dCustFuncAdd �� 
        EXEC SQL insert into dCustFuncAdd(ID_NO , FUNCTION_CODE , FUNCTION_TYPE )
                 values(to_number(:sIn_User_id),:sV_Function_code ,:sV_Function_type);
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFuncAdd��ʧ��!");
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -5;
        } 
        
         ���� dCustFuncAddHis �� 
        EXEC SQL insert into dCustFuncAddHis(ID_NO , FUNCTION_CODE , FUNCTION_TYPE ,
                     UPDATE_LOGIN , UPDATE_ACCEPT , UPDATE_DATE , UPDATE_TIME ,
                     UPDATE_CODE , UPDATE_TYPE ) 
                 values(to_number(:sIn_User_id),:sV_Function_code ,:sV_Function_type ,
                        :sIn_Login_no, to_number(:sIn_Login_accept),
                        to_number(:sIn_Total_date), 
                        TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                        :sIn_Op_code , 'a');
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFuncAddHis��ʧ��!");
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -6;
        } 
        */        
        /* ���� wCustFuncDay �� */
        EXEC SQL insert into wCustFuncDay(ID_NO , FUNCTION_CODE , ADD_FLAG , 
                      TOTAL_DATE , OP_TIME , OP_CODE , LOGIN_NO , LOGIN_ACCEPT )
                 values(to_number(:sIn_User_id), :sV_Function_code , :sV_Add_flag,
                     :sIn_Total_date , TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'),
                     :sIn_Op_code , :sIn_Login_no , to_number(:sIn_Login_accept));
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����wCustFuncDay��ʧ��!");

	        PUBLOG_DBDEBUG("pubDefaultFunc");
		    pubLog_DBErr(_FFL_,"pubDefaultFunc","-7",sErrMessA);                                     
            
            return -7;
        } 
        Trim(sV_Function_code);
	ret = fSendCmd(funcEnv, '1', "1219", sV_Function_code);
	if(ret != 0)
	{
            sprintf(sErrMessA,"%s","����fSendCmd����wSndCmdDayʧ��!");
            pubLog_Debug(_FFL_, "pubDefaultFunc", "", "ret = [%d]",ret);
            return -8;
	}

        EXEC SQL FETCH c102 INTO :sV_Function_type , :sV_Function_code , :sV_Add_flag;
    }
    EXEC SQL CLOSE c102 ;
    
   
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubDefaultFunc", "", "--------------- end function pubDefaultFunc ----------");
    
    return 0 ;
	
}



  /**
  * Auth: YuanBY
  * Name: pubOpenFunc
  * Func: ������ӿͻ����ط���Ϣ
  * �������    
  *     ���뻺����  ( Tuxedo �������� )
  *     ���������  ( Tuxedo �������� )
  *     ��ʱ�����ַ���   ( Tuxedo �������� )
  *     ���ش�����Ϣ
  *
  * Date: 2003/10/14
  * Stat: Test Pass
  */
  
int pubOpenFunc(FBFR32 *transINa , FBFR32 *transOUTa ,
      char *temp_bufA ,char sIn_Total_dateA[21] ,
      char sIn_Sys_dateA[21] , char sIn_Op_codeA[5] ,
      char sIn_Login_noA[7] , char sIn_Org_codeA[10+1],
      char sIn_Login_acceptA[22+1] , char sIn_User_idA[22+1],
      char sIn_Phone_noA[15+1] , char sIn_Sm_codeA[2+1],
      char sIn_Belong_codeA[7+1] , char sIn_Offon_acceptA[22+1],
      char sIn_UsedFunc_ListA[ROWLEN] , char sIn_UnUsedFunc_ListA[ROWLEN],
      char sIn_AddAddFunc_ListA[ROWLEN], char  sIn_CmdRightA[5] ,
      char sIn_SimNoA[21], double sIn_PreFeeA,char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_Total_date[20+1];         /* ��������      */
        char    sIn_Sys_date[20+1];           /* ϵͳ����      */
        char    sIn_Op_code[4+1];             /* ��������      */
        char    sIn_Login_no[6+1];            /* ��������      */
        char    sIn_Org_code[10+1];           /* ��������      */
        char    sIn_Login_accept[22+1];       /* ������ˮ      */
        char    sIn_User_id[22+1];            /* �û�ID        */
        char    sIn_Phone_no[15+1];           /* �ֻ�����      */
        char    sIn_Sm_code[2+1];             /* ҵ������      */
        char    sIn_Belong_code[7+1];         /* ��������      */
        char    sIn_Offon_accept[22+1];       /* ���ػ���ˮ    */
        char    sIn_UsedFunc_List[ROWLEN];    /* ������Ч�ط��� */
        char    sIn_UnUsedFunc_List[ROWLEN];  /* ԤԼ��Ч�ط��� */
        char    sIn_AddAddFunc_List[ROWLEN];  /* ���̺��ط���   */
        char 	sIn_CmdRight[4+1];            /* Ȩ�޴���       */
        char 	sIn_SimNo[20+1];              /* SIM������      */ 
        double  sIn_PreFee=0;
        
        char	sV_Func_Code[2+1];            /* �ط�����      */
        char	sV_BillBindFunc[MAXFUNCTIONNUM][2+1];   /* ���ط����� */
        int     iV_billBindFuncNum;                     /* ���ط����� */
        char	sV_allFuncCode[1000+1];                 /* ���˵������ط���Ϣ */
        char	sV_RetCode[6+1];                        /* ������       */
        char    sV_sqlstr[2000+1];         
        
        char 	sV_ImsiNo[15+1];              /* IMSI ���� */
	char 	sV_hlrCode[1+1];              /* HLR  ���� */
        
        int     iFuncRet = 0;
 	double sV_LimitFee=0;
 	char   sV_FunCode[25+1];
         
		
    EXEC SQL END DECLARE SECTION;
    
    tFuncEnv funcEnv;                    /* �ͻ���Ϣ�ṹ   */
    
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "--------------- begin function pubOpenFunc ----------");
    
    /* ���Ի����� */
    init(sV_FunCode);
    init(sIn_Total_date);
    init(sIn_Sys_date);
    init(sIn_Op_code);
    init(sIn_Login_no);
    init(sIn_Org_code);
    init(sIn_Login_accept);
    init(sIn_User_id);
    init(sIn_Phone_no);
    init(sIn_Sm_code);
    init(sIn_Belong_code);
    init(sIn_Offon_accept);
    init(sIn_UsedFunc_List);
    init(sIn_UnUsedFunc_List);
    init(sIn_AddAddFunc_List);
    init(sIn_CmdRight);
    init(sIn_SimNo);
    
    init(sV_Func_Code);
    init(sV_allFuncCode);
    init(sV_RetCode);
    init(sV_sqlstr);
    
    memset(sV_BillBindFunc, 0, sizeof(sV_BillBindFunc));
    
    /* ȡ��������� */
    strcpy(sIn_Total_date,sIn_Total_dateA);
    strcpy(sIn_Sys_date,sIn_Sys_dateA);
    strcpy(sIn_Op_code,sIn_Op_codeA);
    strcpy(sIn_Login_no,sIn_Login_noA);
    strcpy(sIn_Org_code,sIn_Org_codeA);
    strcpy(sIn_Login_accept,sIn_Login_acceptA);
    strcpy(sIn_User_id,sIn_User_idA);
    strcpy(sIn_Phone_no,sIn_Phone_noA);
    strcpy(sIn_Sm_code,sIn_Sm_codeA);
    strcpy(sIn_Belong_code,sIn_Belong_codeA);
    strcpy(sIn_Offon_accept,sIn_Offon_acceptA);
    strcpy(sIn_UsedFunc_List,sIn_UsedFunc_ListA);
    strcpy(sIn_UnUsedFunc_List,sIn_UnUsedFunc_ListA);
    strcpy(sIn_AddAddFunc_List,sIn_AddAddFunc_ListA);
    strcpy(sIn_CmdRight,sIn_CmdRightA);
    strcpy(sIn_SimNo,sIn_SimNoA);
    sIn_PreFee=sIn_PreFeeA;
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Total_date      =[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Op_code      =[%s]",      sIn_Op_code);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Org_code      =[%s]",      sIn_Org_code);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Login_accept      =[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Sm_code      =[%s]",      sIn_Sm_code);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Belong_code      =[%s]",      sIn_Belong_code);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_Offon_accept      =[%s]",      sIn_Offon_accept);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_UsedFunc_List      =[%s]",      sIn_UsedFunc_List);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_UnUsedFunc_List      =[%s]",      sIn_UnUsedFunc_List);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_AddAddFunc_List      =[%s]",      sIn_AddAddFunc_List);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_CmdRight      =[%s]",      sIn_CmdRight);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_SimNo      =[%s]",      sIn_SimNo);
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sIn_PreFee      =[%f]",      sIn_PreFee);
    #endif
    
    if ((strlen(ltrim(rtrim(sIn_UsedFunc_List)))==0) && 
        (strlen(ltrim(rtrim(sIn_UnUsedFunc_List)))==0) &&
        (strlen(ltrim(rtrim(sIn_UnUsedFunc_List)))==0) &&
        (strlen(ltrim(rtrim(sIn_AddAddFunc_List)))==0))
    {
        strcpy(sErrMessA,"SUCCESSED!");
        
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "������û��ѡ���ط�");
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "--------------- end function pubOpenFunc ----------");
    
        return 0 ;	
    	
    }
    
    /* ��ѯ  IMSI���� */
    EXEC SQL select substr(switch_no,1,15) into :sV_ImsiNo
             from   dCustSim
             where  id_no = to_number(:sIn_User_id);
    if (SQLCODE != 0)
    {
        sprintf(sErrMessA, "��ѯdCustSim����!"); 
        PUBLOG_DBDEBUG("pubOpenFunc");
	    pubLog_DBErr(_FFL_,"pubOpenFunc","-10",sErrMessA);                                     
        
        return -10;         
    }
    
    Trim(sV_ImsiNo);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sV_ImsiNo      =[%s]",      sV_ImsiNo);
    #endif
    
    /* ��ѯ  HLR���� */
    EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
             where  phoneno_head like substr(:sIn_Phone_no,1,7)||'%';
    if (SQLCODE != 0)
    {
        sprintf(sErrMessA, "��ѯsHlrCode����!"); 
        PUBLOG_DBDEBUG("pubOpenFunc");
	    pubLog_DBErr(_FFL_,"pubOpenFunc","-11",sErrMessA);                                     
        
        return -11;         
    }
    
    Trim(sV_hlrCode);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenFunc", "", "sV_hlrCode      =[%s]",      sV_hlrCode);
    #endif
    
    /* ���ͻ���Ϣ������ֵ */
    Trim(sIn_Total_date);
    Trim(sIn_Sys_date);
    Trim(sIn_Op_code);
    Trim(sIn_Login_no);
    Trim(sIn_Org_code);
    Trim(sIn_Login_accept);
    Trim(sIn_User_id);
    Trim(sIn_Phone_no);
    Trim(sIn_Sm_code);
    funcEnv.transIN=transINa;
    funcEnv.transOUT=transOUTa; 
    funcEnv.temp_buf=temp_bufA;
    funcEnv.totalDate=sIn_Total_date;
    funcEnv.opTime=sIn_Sys_date;
    funcEnv.opCode=sIn_Op_code;
    funcEnv.loginNo=sIn_Login_no;
    funcEnv.orgCode=sIn_Org_code;
    funcEnv.loginAccept=sIn_Login_accept;
    funcEnv.idNo=sIn_User_id;
    funcEnv.phoneNo=sIn_Phone_no;
    funcEnv.smCode=sIn_Sm_code;
    funcEnv.cmdRight=sIn_CmdRight;
    funcEnv.simNo=sIn_SimNo;
    funcEnv.belongCode=sIn_Belong_code;
    funcEnv.offonAccept = ""; /* not use  */
    funcEnv.imsiNo=sV_ImsiNo;
    funcEnv.hlrCode=sV_hlrCode;
    
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.totalDate=[%s]",funcEnv.totalDate);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.opTime=[%s]",funcEnv.opTime);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.opCode=[%s]",funcEnv.opCode);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.loginNo=[%s]",funcEnv.loginNo);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.orgCode=[%s]",funcEnv.orgCode);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.loginAccept=[%s]",funcEnv.loginAccept);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.idNo=[%s]",funcEnv.idNo);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.phoneNo=[%s]",funcEnv.phoneNo);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.smCode=[%s]",funcEnv.smCode);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.cmdRight=[%s]",funcEnv.cmdRight);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.simNo=[%s]",funcEnv.simNo);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.belongCode=[%s]",funcEnv.belongCode);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.imsiNo=[%s]",funcEnv.imsiNo);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "funcEnv.hlrCode=[%s]",funcEnv.hlrCode);

    pubLog_Debug(_FFL_, "pubOpenFunc", "", " --- begin process ---- ");
    
    /* ��ѯ���ط� */
    /*
    sprintf(sV_sqlstr,"SELECT distinct b.function_code FROM dBillCustDetail%c a, sBillFuncBind b\
         WHERE a.DETAIL_CODE = b.FUNCTION_BIND  AND a.DETAIL_TYPE = 'a' \
           AND b.region_code = substr('%s', 1,2)  AND begin_time <= sysdate \
           AND end_time > sysdate  AND a.id_no = %s ",sIn_User_id[strlen(sIn_User_id)-1], sIn_Org_code, sIn_User_id);
    
    EXEC SQL PREPARE sele_stmt from :sV_sqlstr;
    EXEC SQL DECLARE c101 CURSOR for sele_stmt;
    EXEC SQL OPEN c101 ;
    EXEC SQL FETCH c101 INTO :sV_Func_Code;
    
    iV_billBindFuncNum = 0;
    while(SQLCODE == 0)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, "pubOpenFunc", "", "sV_Func_Code      =[%s]",      sV_Func_Code);
        #endif
        
        strcpy(sV_BillBindFunc[iV_billBindFuncNum], sV_Func_Code);
        iV_billBindFuncNum ++;
        EXEC SQL FETCH c101 INTO :sV_Func_Code; 
    }
    EXEC SQL CLOSE c101 ;
    
    strcat(sV_allFuncCode, sIn_UsedFunc_List);
    strcat(sV_allFuncCode, sIn_UnUsedFunc_List);
    strcat(sV_allFuncCode, sIn_AddAddFunc_List);
    
    sprintf(sV_RetCode, "%06d", checkBillBindFunc(sV_allFuncCode, 
       sV_BillBindFunc,iV_billBindFuncNum, sV_Func_Code));
    
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "call checkBillBindFunc retcode = [%d]",sV_RetCode);
    if ( strcmp(sV_RetCode, "000000") != 0)
    {
        sprintf(sErrMessA, "���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�[%s]���б��!", sV_RetCode);
        return -2;
    }
    */
    
    /*if(strncmp(sIn_Op_code,"1104",4)==0)
    {*/
    	/*����������Ч���ط�*/
	iFuncRet=fAddUsedLimitFuncs(&funcEnv, sIn_UsedFunc_List,sIn_PreFee,&sV_LimitFee,sV_FunCode);
	if ( iFuncRet != 0)
	{
		if(iFuncRet==121958)
		{
			pubLog_Debug(_FFL_, "pubOpenFunc", "", "1vLimitFee[%f]",sV_LimitFee);
			pubLog_Debug(_FFL_, "pubOpenFunc", "", "1vFunCode[%s]",sV_FunCode);
			sprintf(sErrMessA, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sV_FunCode,sV_LimitFee);
		}
		else
		{
			strcpy(sErrMessA, "����������Ч���ط�����!");
		}
		return -6; 
	}
	/*����ԤԼ��Ч���ط�*/
	iFuncRet=fAddUnUsedLimitFuncs(&funcEnv, sIn_UnUsedFunc_List,sIn_PreFee,&sV_LimitFee,sV_FunCode);
	if ( iFuncRet != 0)
	{
		if(iFuncRet==121956)
		{
			pubLog_Debug(_FFL_, "pubOpenFunc", "", "2vLimitFee[%f]",sV_LimitFee);
			pubLog_Debug(_FFL_, "pubOpenFunc", "", "2vFunCode[%s]",sV_FunCode);
			sprintf(sErrMessA, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sV_FunCode,sV_LimitFee);
		}
		else
		{
			strcpy(sErrMessA, "����ԤԼ��Ч���ط�����!");
		}
		
		return -7; 
	}
    /*}
    else
    {*/
    	/*����������Ч���ط�*/
    	/*iFuncRet =  fAddUsedFuncs(&funcEnv, sIn_UsedFunc_List) ;
    	pubLog_Debug(_FFL_, "pubOpenFunc", "", "call fAddUsedFuncs retcode = [%d]",iFuncRet);
    	if (iFuncRet != 0)
    	{            
    	    sprintf(sErrMessA, "����������Ч���ط�����!");  
    	    return -3; 
    	}*/
    	
    	/*����ԤԼ��Ч���ط�*/
    	/*iFuncRet = fAddUnUsedFuncs(&funcEnv, sIn_UnUsedFunc_List);
    	pubLog_Debug(_FFL_, "pubOpenFunc", "", "call fAddUnUsedFuncs retcode = [%d]",iFuncRet);
    	if (iFuncRet != 0)
    	{
    	    sprintf(sErrMessA, "����ԤԼ��Ч���ط�����!");  
    	    return -4;
    	}
    }*/
    
    /*���Ӵ��̺ŵ��ط���*/
    iFuncRet =  fAddAddFuncs(&funcEnv, sIn_AddAddFunc_List);
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "call fAddAddFuncs retcode = [%d]",iFuncRet);
    if (iFuncRet != 0)
    {
        sprintf(sErrMessA, "���Ӵ��̺ŵ��ط�����!"); 
        return -5;         
    }
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubOpenFunc", "", "--------------- end function pubOpenFunc ----------");
    
    return 0 ;
	
}

  /**
  * Auth: YuanBY
  * Name: pubOpenDataCard
  * Func: ��e�п��� 
  * �������    
  *     ���ش�����Ϣ
  *
  * Date: 2003/12/2
  * Stat: Test Pass
  */
  
int pubOpenDataCard(
      char  sIn_Cred_idA[22] ,  char  sIn_Phone_noA[16] ,
      char  sIn_Sys_dateA[21] ,  char  sIn_Use_typeA[2] ,
      char  sIn_User_idA[23] ,  char  sIn_Login_noA[7] ,
      char  sIn_Gprs_rateA[5] ,  char  sIn_Wlan_rateA[5] ,
      char * sErrMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_Cred_id[21+1];            /* ƾ֤���к�    */
        char    sIn_Phone_no[15+1];           /* �ֻ�����      */
        char    sIn_Sys_date[20+1];           /* ϵͳ����      */
        char    sIn_Use_type[1+1];            /* ƾ֤ʹ�÷�ʽ    "1"��Ӫҵ��  "2":  1860   */
        char    sIn_User_id[22+1];            /* �û�ID        */
        char    sIn_Login_no[6+1];            /* ��������      */
        char    sIn_Gprs_rate[8+1];           /* GPRS����      */
        char    sIn_Wlan_rate[8+1];           /* WLAN����      */
        
        char 	sV_Cred_batch[3+1];           /* ƾ֤���κ�     */ 
        char 	sV_Cred_flag[1+1];            /* ״̬      0�ɹ�/1ʧ��/2��ͻ/3����/4����ɹ�/5����ʧ��/6�����ͻ     */ 
     
		
    EXEC SQL END DECLARE SECTION;
    
    pubLog_Debug(_FFL_, "pubOpenDataCard", "", "--------------- begin function pubOpenDataCard ----------");
    
    /* ���Ի����� */
    init(sIn_Cred_id);
    init(sIn_Phone_no);
    init(sIn_Sys_date);
    init(sIn_Use_type);
    init(sIn_User_id);
    init(sIn_Login_no);
    init(sIn_Gprs_rate);
    init(sIn_Wlan_rate);
    
    init(sV_Cred_batch);
    init(sV_Cred_flag);
   
    /* ��ʼ����ʼ���� */
    strcpy(sV_Cred_batch,"000");
    strcpy(sV_Cred_flag , "3");
    
    
    /* ȡ��������� */
    strcpy(sIn_Cred_id,sIn_Cred_idA);
    strcpy(sIn_Phone_no,sIn_Phone_noA);
    strcpy(sIn_Sys_date,sIn_Sys_dateA);
    strcpy(sIn_Use_type,sIn_Use_typeA);
    strcpy(sIn_User_id,sIn_User_idA);
    strcpy(sIn_Login_no,sIn_Login_noA);
    strcpy(sIn_Gprs_rate,sIn_Gprs_rateA);
    strcpy(sIn_Wlan_rate,sIn_Wlan_rateA);
    
    Trim(sIn_Cred_id);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Cred_id      =[%s]",      sIn_Cred_id);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Use_type      =[%s]",      sIn_Use_type);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Gprs_rate      =[%s]",      sIn_Gprs_rate);
        pubLog_Debug(_FFL_, "pubOpenDataCard", "", "sIn_Wlan_rate      =[%s]",      sIn_Wlan_rate);
    #endif
    
    /* ���� dCredCust �� */
    EXEC SQL insert into dCredCust(rec_type , cred_id , phone_no , cred_price , Op_time , 
                  Use_type , cred_date , Coagent_code , Oper_type , Oper_code , cred_fav,
                  cred_batch , Id_no , Login_no , cred_flag , 
                  sg_check , user_flag , gprs_rate , wlan_rate )
             select rec_type , cred_id , :sIn_Phone_no , cred_price , :sIn_Sys_date ,
                  :sIn_Use_type , cred_date , coagent_code , Oper_type , Oper_code , cred_fav ,
                  :sV_Cred_batch , to_number(:sIn_User_id) , :sIn_Login_no , :sV_Cred_flag ,
                  '0000' , 'N' , :sIn_Gprs_rate , :sIn_Wlan_rate 
             from  dCredRes where cred_id = :sIn_Cred_id ; 
             
    if(SQLCODE!=0)
    {
    	if(SQLCODE==1403)
    	{
            strcpy(sErrMessA,"���ݿ���û�и�ƾ֤����!");
	        PUBLOG_DBDEBUG("pubOpenDataCard");
		    pubLog_DBErr(_FFL_,"pubOpenDataCard","-1",sErrMessA);
            return -1;
        }
        else
        {
            strcpy(sErrMessA,"����dCredCust�����!");
	        PUBLOG_DBDEBUG("pubOpenDataCard");
		    pubLog_DBErr(_FFL_,"pubOpenDataCard","-2",sErrMessA);
            return -2;
        }	
    }
    
    /* �޸� dCredRes ��״̬ */
    EXEC SQL update dCredRes set cred_status = '1'
             where cred_id = :sIn_Cred_id ; 
    if(SQLCODE!=0)
    {
        strcpy(sErrMessA,"�޸�dCredRes�����!");
        PUBLOG_DBDEBUG("pubOpenDataCard");
	    pubLog_DBErr(_FFL_,"pubOpenDataCard","-3",sErrMessA);
        return -3;
    }
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_, "pubOpenDataCard", "", "--------------- end function pubOpenDataCard ----------");
    
    return 0 ;
	
}

/**
  * Auth: FuJian
  * Name: pubDataCred
  * Func: �������ݿ����� 
  * Date: 2004/02/10
  */
int pubDataCred(float InPrepay_Fee,   char *InId_No,       char *InPhone_No,
                char *InMode_Code,    char *InBelong_Code, char *InLogin_No,
                char *InOrg_Code,     char *InLoginAccept, char *InOp_Code,
                char *InTotal_Date,   char *InSysDate,     char *InMsgErr)
{
    EXEC SQL BEGIN DECLARE SECTION;
       char  pId_No[22+1];
       char  pPhone_No[15+1];
       char  pMode_code[8+1];
       char  pBelong_Code[7+1];
       char  pLogin_No[6+1];
       char  pOrg_Code[9+1];
       char  pLogin_Accept[23];
       char  pOp_Code[4+1];
       char  pTotal_Date[8+1];
       char  pSysDate[20+1];
       
       char  vDataCode[2+1];
       int   vOrderCode=0;
       float vMoneyFee=0;  /* �ȼ���С���� */
       float vModeFee=0;   /* �ײͰ��·��� */
       int   vMonthNum=0;  /* �Ż����� */
       float vFavFee=0;    /* �Żݷ���=vModeFee*vMonthNum-vMoneyFee ���С��0������wTotalFav */
       char  vMonthCode[4+1];
       int   vNum=0;
       char  vSqlStr[1500+1];
       char  vEndTime[8+1];
    EXEC SQL END   DECLARE SECTION;
   
    init(pId_No);       init(pPhone_No);     init(pMode_code);
    init(pBelong_Code);
    init(pLogin_No);     init(pOrg_Code);    init(pOp_Code);
    init(pLogin_Accept); init(pTotal_Date);  init(pSysDate);
    init(vDataCode);     init(vMonthCode);   init(vSqlStr);
    init(vEndTime);

    pubLog_Debug(_FFL_, "pubDataCred", "", "------------ ���ݿ����� pubDataCred() Begin --------");
    memcpy(pId_No,          InId_No,         22);        
    memcpy(pPhone_No,       InPhone_No,      15);
    memcpy(pMode_code,      InMode_Code,     8 );
    memcpy(pBelong_Code,    InBelong_Code,   7 );
    memcpy(pLogin_No,       InLogin_No,      6 );
    memcpy(pOrg_Code,       InOrg_Code,      9 );
    memcpy(pOp_Code,        InOp_Code,       4 );
    memcpy(pLogin_Accept,   InLoginAccept,   22);
    memcpy(pTotal_Date,     InTotal_Date,    8 );
    memcpy(pSysDate,        InSysDate,       20);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "pubDataCred", "", "InPrepay_Fee   = [%.2f]", InPrepay_Fee    );
        pubLog_Debug(_FFL_, "pubDataCred", "", "Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_, "pubDataCred", "", "pPhone_No      = [%s]",   pPhone_No       );
        pubLog_Debug(_FFL_, "pubDataCred", "", "pMode_code     = [%s]",   pMode_code      );
        pubLog_Debug(_FFL_, "pubDataCred", "", "Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_, "pubDataCred", "", "Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_, "pubDataCred", "", "pOrg_Code      = [%s]",   pOrg_Code       );
        pubLog_Debug(_FFL_, "pubDataCred", "", "Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_, "pubDataCred", "", "Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_, "pubDataCred", "", "Total_Date     = [%s]",   pTotal_Date     ); 
        pubLog_Debug(_FFL_, "pubDataCred", "", "SysDate        = [%s]",   pSysDate        );     
    #endif
/*
    EXEC SQL insert into dDataCredMsg(id_no , phone_no , Belong_code , Data_code , Begin_time , 
                                      End_time , Month_num , Login_no , Org_code , Login_accept , 
                                      Op_code , Total_date , Op_time)
             select to_number(:pId_No) , :pPhone_No , :pBelong_Code , data_code , to_char(sysdate,'yyyymmdd') , 
                    to_char(add_months(sysdate,month_num),'yyyymmdd') , month_num , :pLogin_No , :pOrg_Code , to_number(:pLogin_Accept) , 
                    :pOp_Code , to_number(:pTotal_Date) , to_date(:pSysDate,'yyyymmdd hh24:mi:ss')
             from sDataCredCode
             where :InPrepay_Fee>=begin_money and :InPrepay_Fee<end_money and rownum<2;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "����dDataCredMsg��ʧ��!");
        strcpy(InMsgErr,"����dDataCredMsg��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
*/
    EXEC SQL insert into dDataCredMsg(id_no , phone_no , Belong_code , Data_code , Begin_time , 
                                      End_time , Month_num , Login_no , Org_code , Login_accept , 
                                      Op_code , Total_date , Op_time)
             select to_number(:pId_No) , :pPhone_No , :pBelong_Code , data_code , to_char(sysdate,'yyyymmdd') , 
                    '20200101' , month_num , :pLogin_No , :pOrg_Code , to_number(:pLogin_Accept) , 
                    :pOp_Code , to_number(:pTotal_Date) , to_date(:pSysDate,'yyyymmdd hh24:mi:ss')
             from sDataCredCode
             where :InPrepay_Fee>=begin_money and :InPrepay_Fee<end_money and rownum<2;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "����dDataCredMsg��ʧ��!");
        strcpy(InMsgErr,"����dDataCredMsg��ʧ��!");
        PUBLOG_DBDEBUG("pubDataCred");
	    pubLog_DBErr(_FFL_,"pubDataCred","-1",InMsgErr);
        return -1;
    }

    EXEC SQL insert into dDataCredMsgHis(id_no , phone_no , Belong_code , Data_code , Begin_time , End_time , Month_num ,
                                         Login_no , Org_code , Login_accept , Op_code , Total_date , Op_time , 
                                         Update_accept , Update_time , Update_date , 
                                         Update_login , Update_type , Update_code)
             select id_no , phone_no , Belong_code , Data_code , Begin_time , End_time , Month_num , 
                    Login_no , Org_code , Login_accept , Op_code , Total_date , Op_time , 
                    to_number(:pLogin_Accept) , to_date(:pSysDate,'yyyymmdd hh24:mi:ss') , to_number(:pTotal_Date) ,
                    :pLogin_No , 'a' , :pOp_Code
             from dDataCredMsg
             where id_no=:pId_No;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "����dDataCredMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dDataCredMsgHis��ʧ��!");
        PUBLOG_DBDEBUG("pubDataCred");
	    pubLog_DBErr(_FFL_,"pubDataCred","-1",InMsgErr);
        return -1;
    }
/*
    EXEC SQL insert into wTotalFavChg(OP_NO,OP_TYPE,PHONE_NO, ORDER_CODE, ID_NO, FAVOUR_OJBECTS,FAVOUR_FEE, OP_TIME)
            select sMaxBillChg.NEXTVAL,'3',phone_no,order_code,id_no,fee_codes,favour_fee,sysdate
            from wTotalFav where phone_no=:pPhone_No;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "����wTotalFavChg��ʧ��!");
        strcpy(InMsgErr,"����wTotalFavChg��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    EXEC SQL delete wTotalFav where phone_no=:pPhone_No;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "ɾ��wTotalFav��ʧ��!");
        strcpy(InMsgErr,"ɾ��wTotalFav��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    EXEC SQL select order_code into :vOrderCode from wTotalFav where id_no=to_number(:pId_No);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "��ѯwTotalFav��ʧ��!");
        strcpy(InMsgErr,"��ѯwTotalFav��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    vOrderCode++;
    
    EXEC SQL select begin_money,month_num,month_code into :vMoneyFee,vMonthNum,:vMonthCode
             from sDataCredCode
             where :InPrepay_Fee>=begin_money and :InPrepay_Fee<end_money and rownum<2;
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "��ѯsDataCredCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsDataCredCode��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    
    EXEC SQL select sum(a.month_fee) into :vModeFee from sBillMonthCode a,sBillModeDetail b
            where a.region_code=b.region_code and a.region_code=substr(:pBelong_Code,1,2) 
                and a.month_code=b.detail_code and b.mode_code=:pMode_code and b.detail_type='1';
    if(SQLCODE!=0)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "��ѯsBillMonthCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsBillMonthCode��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    
    vNum=0;
    EXEC SQL select count(*),to_char(add_months(sysdate,1),'yyyymm')||'01' into :vNum,vEndTime from sDataCredCfg where mode_code=:pMode_code;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        pubLog_Debug(_FFL_, "pubDataCred", "", "��ѯsDataCredCfg��ʧ��!");
        strcpy(InMsgErr,"��ѯsDataCredCfg��ʧ��!");
        pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    if(vNum>0)
    {
        vFavFee=vModeFee*vMonthNum;
        init(vSqlStr);
        sprintf(vSqlStr,\
            "insert into dBillCustDetail%c(id_no,detail_type,detail_code,begin_time,end_time,fav_order,mode_code,\
            mode_flag,mode_time,mode_status,op_code,total_date,op_time,login_no,login_accept,region_code)\
            values(to_number(:v1),'1',:v2,to_date(:v3,'YYYYMMDD HH24:MI:SS'),to_date(:v4,'YYYYMMDD'),0,'gn23zzzz',\
            '2','Y','N',:v5,to_number(:v6),to_date(:v7,'YYYYMMDD HH24:MI:SS'),:v8,to_number(:v9),substr(:v10,1,2))",\
            pId_No[strlen(pId_No)-1]);

        exec sql prepare sqltmp from :vSqlStr;
        exec sql execute sqltmp using :pId_No,:vMonthCode,:pSysDate,:vEndTime,
            :pOp_Code,:pTotal_Date,:pSysDate,:pLogin_No,:pLogin_Accept,:pBelong_Code;
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, "pubDataCred", "", "��¼dBillCustDetail����!");
            strcpy(InMsgErr,"��¼dBillCustDetail����!");
            pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
        exec sql insert into wUserFavChg(
            op_no,op_type,op_time,id_no,detail_type,detail_code,
            begin_time,end_time,fav_order,mode_code)
        values(
            sMaxBillChg.nextval,'1',to_date(:pSysDate,'YYYYMMDD HH24:MI:SS'),to_number(:pId_No),'1',:vMonthCode,
            to_date(:pSysDate,'YYYYMMDD HH24:MI:SS'),to_date(:vEndTime,'YYYYMMDD'),0,'gn23zzzz');
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, "pubDataCred", "", "��¼dBillCustDetail����!");
            strcpy(InMsgErr,"��¼dBillCustDetail����!");
            pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
    }
    else
        vFavFee=vModeFee*vMonthNum-vMoneyFee;

    if(vFavFee>0)
    {
        EXEC SQL insert into wTotalFav(PHONE_NO, ORDER_CODE, ID_NO, FEE_CODES, SM_FLAG, FAVOUR_FEE, BEGIN_TIME, END_TIME)
                values(:pPhone_No,:vOrderCode,to_number(:pId_No),'r0','g',:vFavFee,sysdate,add_months(sysdate,:vMonthNum));
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, "pubDataCred", "", "����wTotalFav��ʧ��!");
            strcpy(InMsgErr,"����wTotalFav��ʧ��!");
            pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
        EXEC SQL insert into wTotalFavChg(OP_NO,OP_TYPE,PHONE_NO, ORDER_CODE, ID_NO, FAVOUR_OJBECTS,FAVOUR_FEE, OP_TIME)
                values(sMaxBillChg.NEXTVAL,'1',:pPhone_No,:vOrderCode,to_number(:pId_No),'r0',:vFavFee,to_char(sysdate,'yyyymmddhh24miss'));
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, "pubDataCred", "", "����wTotalFavChg��ʧ��!");
            strcpy(InMsgErr,"����wTotalFavChg��ʧ��!");
            pubLog_Debug(_FFL_, "pubDataCred", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
    }
	*/
    pubLog_Debug(_FFL_, "pubDataCred", "", "------------ ���ݿ����� pubDataCred() End --------");
    
    return 0;
}


/******************************************************
 �������ƣ�s1104Cfm
 �������ڣ�2003/10/16
 ����汾: V1.0
 ������Ա��PeiJH
 ������������ͨ�û������������п���
 ��������� 
        pLogin_No                  ����
        pIp_Address                ӪҵԱ��½IP 
        pOp_Code                   ��������
        pSystem_Note               ϵͳ��ע
        pOp_Note                   ������ע
        pPhone_No                  �ֻ�����
        pSim_Type                  SIM������
        pSim_No                    SIM����
        pBelong_Code               �����������
        pCust_Id                   �ͻ�Id
        pId_No                     �û�Id 
        pCon_Id                    �ʻ�Id
        pCust_Passwd               �ͻ�����
        pUser_Passwd               �û�����
        pCon_Passwd                �ʻ�����
        pCust_List                 �ͻ���Ϣ��
        pAssure_List               ������Ϣ��
        pPost_List                 �ʼ��ʵ���Ϣ��
        pFee_List                  ������Ϣ��
        pUsedFunc_List             ������Ч���ط���
        pPackage_List              �ײ���Ϣ��
        pFavour_List               �Ż���Ϣ��
        pUnUsedFunc_List           ԤԼ��Ч���ط���
        pAddAddFunc_List           ���̺ŵ��ط���
        pdefaultFunc               Ĭ���ط���   
        pCBXCustInfo               �����пͻ���Ϣ��
        sIn_Login_Accept           ������ˮ
        pRent_Flag                 ��һλ�Ƿ������s��b ���������r ���  �ڶ�λ�Ƿ����ط�ѡ��ť��y �����n û�е��
        pPack_Id                   Ӫ����ƾ֤��
        
        
        
pCust_List = ҵ��Ʒ�ƴ��� ~ �������ʹ��� ~ ������ ~ �û����� ~ �����ȴ��� ~
     ������ ~ �������� ~ ���Ѵ��� ~ �������� ~  Ȩ��ֵ ~  �������� ~
     �������� ~ �������� ~ ֧Ʊ���� ~ ���д��� 
     
pFee_List = �ֽ𽻿� ~ ֧Ʊ���� ~ Ԥ��� ~ SIM���� ~ ������ ~ ������ ~ ѡ�ŷ� ~ ������ ~ ������

pFavour_List = �Żݴ��� ~ �Ż�ǰ���� ~ �Żݺ�����        
        
 ���ز�����
        ���ش��� 
        �������� 
 �޸ļ�¼:
        �޸���Ա��
        �޸����ڣ�
        �޸�ǰ�汾�ţ�
        �޸ĺ�汾��:
        �޸�ԭ��
*****************************************************/

_TUX_SVC_ s1104Cfm(TPSVCINFO *transb){
    char        temp_buf[61];       /*��ʱʹ�õ��ַ�������*/
	time_t t_beg,t_end;
    /*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
    char		  service_name[19 + 1];
    char		  vStartTime[20+1];			/* ����ʼʱ�� YYYYMMDDHHMISS*/
    double		vUsedTime=0.00;				/* ��������ʱ�� ��λ����*/
    double		vStartSec=0.00;				/* ����ʼʱ�� ��λ����*/
    BOMC_LOG 	logShip;					/*������־��¼�ṹ���������*/
		FBFR32    *transIN_TP = NULL;
		FBFR32    *transOUT_TP = NULL;
		FLDLEN32  len;
    EXEC SQL BEGIN DECLARE SECTION;
    /* ��������������� */
	char  vModeCode[8+1];
	char  vModeCode1[8+1];
	int	  vFlag = 0;
	int   iCount = 0;
	char  sTemp[10+1];                /*���չ�����ʵ����ʶ*/
    char  pLogin_No[6+1];            /*   ����          */
    char  pIp_Address[15+1];         /*   ӪҵԱ��½IP  */
    char  pOp_Code[4+1];             /*   ��������      */
    char  pSystem_Note[60+1];        /*   ϵͳ��ע      */        
    char  pOp_Note[60+1];            /*   ������ע      */
    char  pPhone_No[15+1];           /*   �ֻ�����      */
    char  pSim_Type[5+1];            /*   SIM������     */
    char  pSim_No[20+1];             /*   SIM����       */
    char  pImsi_No[20+1];            /*   pImsi_No       */
    char  pBelong_Code[7+1];         /*   �����������   */    
    char  pCust_Id[22+1];            /*   �ͻ�Id        */
    char  pId_No[22+1];              /*   �û�Id        */
    char  pCon_Id[22+1];             /*   �ʻ�Id        */
    char  pCust_Passwd[8+1];         /*   �ͻ����� ��Ӧ���Ǽ��ܺ�ĳ��ȣ�*/ 
    char  pUser_Passwd[8+1];         /*   �û����� ��Ӧ���Ǽ��ܺ�ĳ��ȣ�*/
    char  pCon_Passwd[8+1];          /*   �ʻ����� ��Ӧ���Ǽ��ܺ�ĳ��ȣ�*/
    char  pGrade_code[8+1];          /*   �û�����      */
    char  pAreaCode[20+1];            /*   �û�����      */
    char  pGroupNo[10+1];
    char  pPost_Flag[1+1];           /*   �ʼ��ʵ���־  */
    char  pCust_List[ROWLEN];        /*   �ͻ���Ϣ��    */
    char  pAssure_List[ROWLEN];      /*   ������Ϣ��    */
    char  pPost_List[ROWLEN];        /*   �ʼ��ʵ���Ϣ��*/
    char  pFee_List[ROWLEN];         /*   ������Ϣ��    */
    char  pUsedFunc_List[ROWLEN];        /*   ������Ч���ط���    */
                           /* ��ʽ��'AAYYYYMMDD|'  */
    char  pPackage_List[ROWLEN];     /*   �ײ���Ϣ��    */    
    char  pFavour_List[ROWLEN];      /*   �Ż���Ϣ��    */
    char  pUnUsedFunc_List[ROWLEN];        /*   ԤԼ��Ч���ط���    */
              /* 'AAYYYYMMDDYYYYMMDD|'  */
    char  pAddAddFunc_List[ROWLEN];        /*   ���̺ŵ��ط���    */
                      /* ''AAXXXXXXXX '  */
    char  pdefaultFunc[ROWLEN];        /*   Ĭ���ط���    */
                      /* ''AA|AA '  */
    char  pCBXCustInfo[200];         /*   �����пͻ���Ϣ��    */
    char  sIn_Login_Accept[22+1];    /* ������ˮ     */
    char  sIn_E_DataList[50];        /* ��e����Ϣ��  ƾ֤ϵ�к�~ Gprs�ײ� ~ WLAN�ײ� */ 
    
    char  sV_Cust_Name[60+1];          /*   �ͻ�����      */
    char  sV_Cust_Address[60+1];       /*   �ͻ���ַ      */
    char  sV_Id_Type[2+1];             /*   �ͻ�֤������  */
    char  sV_Id_Iccid[20+1];           /*   �ͻ�֤������  */ 
    char  sV_Iccid[20+1];
    char  vIdIccid[20+1];
    char	comm_iccid[20+1];
    char  vOrderId[18+1];
    char	rechntype[1+1];    
    
    char  sV_Gprs_rate[8+1];           /* GPRS����      */
    char  sV_Wlan_rate[8+1];           /* WLAN����      */    
    char  sV_Cred_id[21+1];            /* ƾ֤���к�    */
    char  pRent_Flag[2+1];             /* ��һλ�Ƿ������s��b ���������r ���  �ڶ�λ�Ƿ����ط�ѡ��ť��y �����n û�е��  */
    char  pPack_Id[20+1];              /* Ӫ����ƾ֤��  */

    char sTrue_flag[1+1];
    char  vErrMsg[60+1];               /*   �����ʷѴ�����Ϣ */
    
  
    /* ����ʹ�õ����������� */
    /*---- ������Ϣ ----*/
    char  vOrg_Code[9+1];            /*   ��������       */
    char  vRegion_Code[2+1];         /*   ��������       */
    char  vDistrict_Code[2+1];       /*   ���ش���       */
    char  vTown_Code[3+1];           /*   Ӫҵ������     */
    char  vTotal_Date[8+1];          /*   ��������       */

    /*---- dCustMsg ----*/
    char  vContract_Passwd[8+1];     /*   Ĭ���ʻ�����   */
    
    char  vSm_Code[2+1];             /*   Ʒ�ƴ���       */
    char  vService_Type[2+1];        /*   �������ʹ���   */
    char  vAttr_Code[8+1];
    float vLimit_Owe=0.00;           /*   ����״̬       */
    char  vCredit_Code[2+1];         /*                  */
    int   vCredit_Value=0;           /*                  */
    int   vBill_Type=0;              /*   ��������       */
    char  vPay_Code[1+1];
    char  vPay_Type[4+1];            /*   �������       */
    int   vCmd_Right=0;
    char  sV_Cmd_right[3];      
    char  vInnet_Type[2+1];          /*   ��������       */
    char  vMachine_Code[3+1];        /*   ��������       */
    char  vOther_Attr[40+1];         /*   ��������       */
    
    int   vFavNum=0;                       /*   ӪҵԱ�Ż����� */
    char  vFavour_Code[ROWNUM11][4+1];     /*   �Żݴ���       */
    char  vShould_Data[ROWNUM11][17+1];    /*   �Ż�ǰ����     */
    char  vActual_Data[ROWNUM11][17+1];    /*   �Żݺ�����     */
    
    int   vFuncNum=0;                      /*   �û������ط�����*/
    char  vFunction_Type[ROWNUM11][1+1];   /*   �ط�����        */
    char  vFunction_Code[ROWNUM11][2+1];   /*   �ط�����        */
    char  vFavour_Month[ROWNUM11][100];    /*   �Ż�����        */
    char  vAdd_No[ROWNUM11][15+1];         /*   ���Ӻ���        */
    char  vBegin_Time[ROWNUM11][20+1];     /*   �Żݿ�ʼʱ��    */
    char  vEnd_Time[ROWNUM11][20+1];       /*   �Żݽ���ʱ��    */

    char  vCheck_No[20+1];           /*   ֧Ʊ����       */
    char  vBank_Code[5+1];           /*   ���д���       */

    char  vAssure_No[12+1];          /*   �����˺���     */
    char  vAssure_Name[60+1];        /*   ����������     */
    char  vId_Type[1+1];             /*   ������֤������ */
    char  vAssure_Id[20+1];          /*   ������֤������ */
    char  vAssure_Phone[20+1];       /*   �����˵绰     */
    char  vAssure_Address[60+1];     /*   �����˵�ַ     */
    char  vAssure_ZIP[6+1];          /*   �������ʱ�     */    

    char  vPost_Flag[1+1];           /*   �ʼ��ʵ���־   */
    char  vPost_Type[1+1];           /*   �ʼ��ʵ�����   */
    char  vPost_Address[200+1];       /*   �ʼ��ʵ���ַ   */
    char  vPost_ZIP[6+1];            /*   �ʼ��ʵ��ʱ�   */
    char  vPost_Fax[30+1];           /*   �ʼ��ʵ�����   */
    char  vPost_Mail[30+1];          /*   �ʼ��ʵ�Mail   */
    char  vPost_Content[1+1];          /*   �ʼ��ʵ�Mail   */
    
    char  vLogin_Accept_Char[22+1];  /*   ������ˮ(char) */ 
    char  vs_Offon_accept[22+1];     /*   ���ػ���ˮ     */
    int   vBillModeNum=0;            /*   �û�������ʷѴ������������ʷѣ���ѡ����*/    
    char  vBillMode[ROWNUM11][8+1];  /*   �ʷѴ���       */

    float vCash_Pay=0;               /*   �ֽ𽻿�       */
    float vCheck_Pay=0;              /*   ֧Ʊ����       */
    float vPrepay_Fee=0;             /*   Ԥ���         */
    float vSim_Fee=0;                /*   SIM����        */
    float vMachine_Fee=0;            /*   ������         */
    float vInnet_Fee=0;              /*   ������         */
    float vChoice_Fee=0;             /*   ѡ�ŷ�         */
    float vOther_Fee=0;              /*   ������         */
    float vHand_Fee=0;               /*   ������         */
    
    /***************add for ��𾻶���� 2012.6.18*********************/
    int   vIsNetAmount=0;            /*   �Ƿ񾻶����(0 ���ʽ, 1 �Ǿ��ʽ)  */
    char  vIsOutright[1+1];          /*   �Ƿ���ϴ�����(Y:��ʾ���ڷ���ϴ�����)  */
    float vReward_Fee=0;             /*   �����̳��     */
    float vTax_Fee=0;                /*   ����˰��       */
    /***************add for ��𾻶���� *********************/
    
    float vInterest =0;              /*   Ԥ�����Ϣ     */
    float vPay_Money=0;              /*   ??             */
       
    int   tmplen=0,begin=1,end=1,num=0,i=0,k=1;
    int   tmplen1=0,begin1=1,end1=1,num1=0,i1=0,k1=1;
    int   ret=0;
    char  vTmp_List[ROWLEN],vTmp_List1[ROWLEN];
    char  sV_YYYYMM[6+1];            /*   ����           */

    char  vDateChar[6+1];            /*   �ֱ���ǰ��     */
    char  vSysDate[20+1];            /*   ϵͳʱ��       */

    char  iEffectTime[17+1];
	char	vBunchNo[20+1];
	int pLimitFeeMax=0;  /*159��������������������*/
	char vOrgId[10+1];
	char vPassWord[16+1];
	/* ADDED BY ZHCY : 2009-06-01 */
	char l_sPhoneHead[7+1];
	char l_sModeCode[8+1];
	char l_sRegionCode[2+1];
	char l_sModeName[30+1];
	char l_sFlag[1+1];	
	
	/*NG*/
	char sMainCode[2+1];
	char sMainNote[20+1];
	TdCustType	sTdCustType;
	
	char	inResv1[5+1];
	long	iLoginAccept=0;
	/*Add by liujj at 2011��10��25�� Begin*/
	int iIsTd = 0;
	/*Add by liujj at 2011��10��25�� End*/
	
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	
	char vaccflag[1+1]; /*�Ƿ�����ڱ�־*/
	long  vcontractNo=0; /*�˻�*/
	char vcycleDay[2+1];/*����  28������  �磺1�š�2�� ......28��    */
	int iaccnum=0;
	char vnowdate[2+1];/*ϵͳ��ǰʱ�� DD*/
	char	vCustAcctCycleBeginTime[17+1];
	char	vCustAcctCycleEndTime[17+1];
	int		vRetout=0;
	
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
	char	vOrderStatus[3+1];
	/*���ӵ��Ӵ��Ż�  xubp  2013-05-20  begin*/
	char  vElecSeries[15+1];/*�ֻ����Ӵ���*/
	char  vPrepayFee[10+1];/*Ԥ�����*/
	char  vMonthFav[10+1];/*ÿ���Żݽ��*/
	char  vFavCode[20+1];/*�ֻ����Żݴ���*/
	char  vInnetPrepay[19+1];/*�������Ԥ��*/
	char  vFavMoney[10+1];/*�Żݽ��*/
	char  vMonthNum[5+1];/*�Ż�����*/
	char  vBeginTime[8+1];/*�Żݿ�ʼʱ��*/
	char  vEndTime[8+1];/*�Żݽ���ʱ��*/
	char  vLeastFee[5+1];/*�������*/
	char  vLoginAccept[14+1];/*������ˮ */
	char  return_code_tp[6+1]; 
	char  return_msg_tp[61+1];
	char  vOpNote[60+1];
	int   vRetCodeTp=0;
	long  ilen=0;
	long  olen=0;
	int   vCount2=0;
	/*���ӵ��Ӵ��Ż�  xubp  2013-05-20  end*/
    EXEC SQL END DECLARE SECTION;
    /***************add for ��𾻶���� 2012.6.18*********************/
    init(vIsOutright);
    /***************add for ��𾻶���� *********************/
    int iRet = 0;
        
    tFuncEnv funcEnv;                /* �ͻ���Ϣ�ṹ  */
    char  sV_ImsiNo[15+1];
    char  sV_hlrCode[1+1];
    char  sV_NewOldFlag[1+1];
    char  sV_AddMode[8+1];           /* Ӫ���������ʷ� */
    char  iAddLac[1000+1];	/* �����С�����봮 */

    /* �ͻ����� */
    char    sIn_CustMsg[1000];
    char    sIn_CustId[1000];
    char    sIn_CustContact[1000];
    char    sIn_CustAttr[1000];
    char    sIn_CustAdd[1000];
    char    sIn_CustCreate[1+1];

    char    sV_SendFlag[1+1];
	char	dynStmt[1024];
	char	TableName[30+1];
	char	sCustinfoType[2+1];
	char    vOtaOpCode[4+1];
	int     vOtaCount=0;
	char	sagreement_type[1+1];
	
	char vLimitCause[256+1];
    char vDealType[1+1];
    char vSimPassWd[8+1];
    int vCount=0;
    int vCount1=0;
    char vGCustName[60+1];
    char vGIdIccid[20+1];
	char vGtrueFlag[1+1];
	char offenFunc[32+1];
	char vrunCode[2+1];
	char	newfuncList[128+1];
  	char	oldfuncList[128+1];
  	char	newfuncStr[128+1];
  	double V_LimitFee=0;
  	char	sv_funccode[25+1];
  	char	defaultfuncList[64+1];
  	
  	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
  	int   v_ret=0;
	char  vCallServiceName[30+1];
	int   vSendParaNum=0;
	int   vRecpParaNum=0;
	char  vParamList[DLMAXITEMS][2][DLINTERFACEDATA];
	int   iOrderRight=100;
  	char  mRegion_Code[2+1];         /*   ��������       */
  	char  pPayFlag[1+1];
  	int   iGoodNoCount=0;
  	char  cPrepay_Fee[20+1];
  	char  cCheck_Pay[20+1];
  	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/
  	
  	/*ng���� by yaoxc begin*/
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	
	
	init(v_parameter_array);
	init(sTempSqlStr);
	/*ng���� by yaoxc end*/
  	
    /*�ַ��ͱ�����ʼ��,����ʹ������ķ�ʽ,Ҳ����ʹ��
     init(������),initʽ��ͷ�ļ��ж���ĺ�,init�Ķ�
     ��: define init(a)  memset(a,0,sizeof(a))  ***/
	/*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
	init(service_name);
	strcpy(service_name, "s1104Cfm");
	init(vStartTime);
	memset(&logShip, 0, sizeof(logShip));
	/*��ȡ����ʼʱ��*/
	getBeginTime(&vStartTime,&vStartSec);
    
    /** ------------------ ������ʼ����ʼ ------------------ **/

    init(pSystem_Note);
    init(temp_buf);        init(vTmp_List);        init(pIp_Address); 
    init(pPhone_No);       init(pOp_Code);         init(pOp_Note);  
    init(pSim_No);         init(pBelong_Code);     init(pPost_Flag);
    init(pCust_Passwd);    init(pUser_Passwd);     init(pCon_Passwd);
    init(pCust_List);      init(pAssure_List);     init(pPost_List);
    init(pFee_List);       init(pPackage_List);    init(pGrade_code);
    init(pImsi_No);        init(pIp_Address);      init(pCust_Id);
    init(pCon_Id);         init(pId_No);           init(pFavour_List);
    init(pSim_Type);       init(sV_YYYYMM);        init(pAreaCode); 
    init(pGroupNo); 	   init(sV_Iccid);		   init(vIdIccid);
    init(pUsedFunc_List);  init(pUnUsedFunc_List);   init(pAddAddFunc_List);
    init(pdefaultFunc);	   init(defaultfuncList);

    init(vSm_Code);        init(vAttr_Code);       init(vService_Type);
    init(vCredit_Code);    init(vPay_Code);        init(vPay_Type);       
    init(vInnet_Type);     init(vMachine_Code);    init(vAssure_ZIP);
    init(vCheck_No);       init(vBank_Code);       init(vAssure_No);
    init(vAssure_Name);    init(vAssure_Id);       init(vAssure_Phone);
    init(vAssure_Address); init(vPost_Flag);       init(vPost_Type);
    init(vPost_Address);   init(vPost_ZIP);        init(vPost_Fax);
    init(vPost_Mail);  init(vPost_Content);    init(vOrg_Code);        init(vRegion_Code);
    init(vDistrict_Code);  init(vTown_Code);       init(vDateChar);
    init(vTotal_Date);     init(vContract_Passwd); init(vSysDate);
    init(vTmp_List);       init(vTmp_List1);       init(vId_Type);  
    init(vLogin_Accept_Char);   init(sIn_Login_Accept);
    init(sIn_E_DataList);       init(sV_Gprs_rate);
    init(sV_Wlan_rate);         init(sV_Cred_id);
    init(pRent_Flag);      init(vErrMsg);			init(sV_ImsiNo);
    init(sV_hlrCode);      init(sV_NewOldFlag);    init(pPack_Id);
    init(sV_AddMode);      init(sV_Cust_Name);     init(sV_Cust_Address);
    
    init(sIn_CustMsg);     init(sIn_CustId);       init(sIn_CustContact);
    init(sIn_CustAttr);    init(sIn_CustAdd);      init(sIn_CustCreate);
    
    init(iEffectTime);  	 init(comm_iccid);	init(rechntype);
	 init(vBunchNo); init(dynStmt); init(TableName);
	 init(vOrderId);init(vElecSeries);init(vPrepayFee);init(vMonthFav);init(vFavCode);init(vInnetPrepay);
	 init(vFavMoney);init(vMonthNum);init(vBeginTime);init(vEndTime);init(vLeastFee);
	 init(return_code_tp);init(return_msg_tp);init(vLoginAccept);init(vOpNote);
	 
	 init(sTrue_flag);init(sTemp);init(pLogin_No);init(sCustinfoType);
	 init(vOtaOpCode);
	 init(sagreement_type);
	 init(vLimitCause);
	init(vDealType);
	init(vSimPassWd);
	init(vGCustName);
    init(vGIdIccid);
    init(vGtrueFlag);
    init(offenFunc);
    init(vrunCode);
    init(newfuncList);
  	init(oldfuncList);
  	init(newfuncStr);
  	init(sv_funccode);

	init(vOrgId);
	init(vPassWord);
  	init(l_sPhoneHead);
  	init(l_sModeCode);
  	init(l_sRegionCode);
  	init(l_sModeName);
  	init(l_sFlag);
  	init(iAddLac);
  	init(vOrderStatus);
  	
  	init(sMainCode); init(sMainNote);
  	
  	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	init(vaccflag);
	init(vcycleDay);
	init(vnowdate);
	init(vCustAcctCycleBeginTime);
	init(vCustAcctCycleEndTime);
	
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
  	
  	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
  	init(vCallServiceName);
    init(mRegion_Code);
    init(pPayFlag);
    init(cPrepay_Fee);
    init(cCheck_Pay);
  	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/	
  	
    for(i=0;i<ROWNUM11;i++)
    {
        init(vFunction_Code[i]);
        init(vFunction_Type[i]);
        init(vFavour_Month[i]);
        init(vAdd_No[i]);
        init(vBegin_Time[i]);
        init(vEnd_Time[i]);
    }
    
    for(i=0;i<ROWNUM11;i++)
    {
        init(vFavour_Code[i]);
        init(vShould_Data[i]);
        init(vActual_Data[i]);
        
        init(vBillMode[i]);
    }   
    memset(&sTdCustType,0,sizeof(sTdCustType));
	 time(&t_beg);
	memset(inResv1,0,sizeof(inResv1));
	strcpy(inResv1,"ALL");
	Trim(inResv1);
	 
	TUX_SERVICE_BEGIN(iLoginAccept,pOp_Code,pLogin_No,pOp_Note,ORDERIDTYPE_USER,pId_No,inResv1,NULL);
	
	strcpy(oRetCode,"000000");
	strcpy(oRetMsg,"ҵ����ɹ�");

    /** ------------------ ������ʼ������ ------------------ **/
    
    strcpy(sTemp , input_parms[0]);
    strcpy(pIp_Address , input_parms[1]);
    strcpy(pOp_Code , input_parms[2]);
    strcpy(pSystem_Note , input_parms[3]);
    strcpy(pOp_Note, input_parms[4]);
    strcpy(pPhone_No , input_parms[5]);
    strcpy(pSim_Type , input_parms[6]);
    strcpy(pSim_No , input_parms[7]);
    strcpy(pBelong_Code, input_parms[8]);
    strcpy(pCust_Id , input_parms[9]);
    strcpy(pId_No , input_parms[10]);
    strcpy(pCon_Id , input_parms[11]);
    strcpy(pCust_Passwd, input_parms[12]);
    strcpy(pUser_Passwd , input_parms[13]);
    strcpy(pCon_Passwd , input_parms[14]);
    strcpy(pCust_List , input_parms[15]);
    strcpy(pAssure_List, input_parms[16]);
    strcpy(pPost_List , input_parms[17]);
    strcpy(pFee_List , input_parms[18]);
    strcpy(pUsedFunc_List , input_parms[19]);
    strcpy(pPackage_List, input_parms[20]);
    strcpy(pFavour_List , input_parms[21]);
    strcpy(pUnUsedFunc_List , input_parms[22]);
    strcpy(pAddAddFunc_List , input_parms[23]);
    strcpy(pCBXCustInfo , input_parms[24]);
    strcpy(sIn_Login_Accept , input_parms[25]);
    strcpy(sIn_E_DataList , input_parms[26]);
    strcpy(pRent_Flag , input_parms[27]);
    strcpy(pPack_Id , input_parms[28]);
    strcpy(sIn_CustMsg , input_parms[29]);
    strcpy(sIn_CustId , input_parms[30]);
    strcpy(sIn_CustContact , input_parms[31]);
    strcpy(sIn_CustAttr , input_parms[32]);
    strcpy(sIn_CustAdd , input_parms[33]);
    strcpy(sIn_CustCreate , input_parms[34]);

    strncpy(pLogin_No,sTemp,6);
    strncpy(sTrue_flag,sTemp+6,1);
    strncpy(sCustinfoType,sTemp+6+1,2);
    strncpy(sagreement_type,sTemp+6+1+2,1);
    strncpy(vRegion_Code,pBelong_Code,2);
    strcpy(vrunCode,"AA");
    strncpy(l_sRegionCode, pBelong_Code, 2);
    strncpy(l_sPhoneHead, pPhone_No, 7);   
    strcpy(vPassWord,input_parms[35]); /*�����������*/
    strcpy(iAddLac,input_parms[36]);  /*������ʱ�����ײ�*/
    
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	strcpy(vaccflag,input_parms[37]);  /*�����Ƿ�����ڱ�־*/
	strcpy(vcycleDay,input_parms[38]);  /*����Ƕ������������ڣ�����ʹ�����Ȼ���� 1*/
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
	
		/*���ӵ��Ӵ��Ż� add by  xubp   2013-05-21   begin*/
		strcpy(vElecSeries,input_parms[39]);
		strcpy(vFavCode,input_parms[40]);
		strcpy(vPrepayFee,input_parms[41]);
		strcpy(vMonthFav,input_parms[42]);
		/*���ӵ��Ӵ��Ż� add by  xubp   2013-05-21   end*/

    
    #ifdef _DEBUG_

        pubLog_Debug(_FFL_, service_name, "", "ӪҵԱ����    =[%s]",    pLogin_No    );
        pubLog_Debug(_FFL_, service_name, "", "ӪҵԱ��½IP  =[%s]",    pIp_Address  );
        pubLog_Debug(_FFL_, service_name, "", "��������      =[%s]",    pOp_Code     );
        pubLog_Debug(_FFL_, service_name, "", "ϵͳ��ע      =[%s]",    pSystem_Note );        
        pubLog_Debug(_FFL_, service_name, "", "������ע      =[%s]",    pOp_Note     );        
        pubLog_Debug(_FFL_, service_name, "", "�ֻ�����      =[%s]",    pPhone_No    );
        pubLog_Debug(_FFL_, service_name, "", "SIM������     =[%s]",    pSim_Type    );
        pubLog_Debug(_FFL_, service_name, "", "SIM����       =[%s]",    pSim_No      );
        pubLog_Debug(_FFL_, service_name, "", "�����������  =[%s]",    pBelong_Code );        
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�ID        =[%s]",    pCust_Id     );
        pubLog_Debug(_FFL_, service_name, "", "�û�ID        =[%s]",    pId_No       );
        pubLog_Debug(_FFL_, service_name, "", "�ʻ�ID        =[%s]",    pCon_Id      );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�����      =[%s]",    pCust_Passwd );
        pubLog_Debug(_FFL_, service_name, "", "�û�����      =[%s]",    pUser_Passwd );
        pubLog_Debug(_FFL_, service_name, "", "�ʻ�����      =[%s]",    pCon_Passwd  );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ���Ϣ��    =[%s]",    pCust_List   );
        pubLog_Debug(_FFL_, service_name, "", "������Ϣ��    =[%s]",    pAssure_List );
        pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ���Ϣ��=[%s]",    pPost_List   );
        pubLog_Debug(_FFL_, service_name, "", "������Ϣ��    =[%s]",    pFee_List    );
        pubLog_Debug(_FFL_, service_name, "", "������Ч���ط���=[%s]",    pUsedFunc_List   );
        pubLog_Debug(_FFL_, service_name, "", "�ײ���Ϣ��    =[%s]",    pPackage_List);
        pubLog_Debug(_FFL_, service_name, "", "�Ż���Ϣ��    =[%s]",    pFavour_List);    
        pubLog_Debug(_FFL_, service_name, "", "ԤԼ��Ч���ط���=[%s]",    pUnUsedFunc_List   );
        pubLog_Debug(_FFL_, service_name, "", "���̺ŵ��ط���=[%s]",    pAddAddFunc_List   );  
        pubLog_Debug(_FFL_, service_name, "", "�����пͻ���Ϣ��=[%s]",    pCBXCustInfo   );   
        pubLog_Debug(_FFL_, service_name, "", "sIn_Login_Accept = [%s] ", sIn_Login_Accept );
        pubLog_Debug(_FFL_, service_name, "", "��e����Ϣ�� = [%s] ",   sIn_E_DataList );
        pubLog_Debug(_FFL_, service_name, "", "��һλ�Ƿ������s��b ���������r ���  �ڶ�λ�Ƿ����ط�ѡ��ť��y �����n û�е�� = [%s] ",   pRent_Flag );
        pubLog_Debug(_FFL_, service_name, "", "Ӫ����ƾ֤��  =[%s]",    pPack_Id     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustMsg  =[%s]",    sIn_CustMsg     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustId  =[%s]",    sIn_CustId     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustContact  =[%s]",    sIn_CustContact     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustAttr  =[%s]",    sIn_CustAttr     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustAdd  =[%s]",    sIn_CustAdd     );
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustCreate  =[%s]",    sIn_CustCreate     );
        pubLog_Debug(_FFL_, service_name, "", "�Ƿ�Ϊʵ����=[%s]",sTrue_flag);
        
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
		pubLog_Debug(_FFL_, service_name, "", "vaccflag  =[%s]",    vaccflag     );
		pubLog_Debug(_FFL_, service_name, "", "vcycleDay=[%s]",vcycleDay);
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
        
    #endif
    
        if(strcmp(pOp_Code,"118a")==0)
        {
        	strcpy(vOtaOpCode,pOp_Code);
        	init(pOp_Code);
        	strcpy(pOp_Code,"1108");
        }
        else
        {
        	strcpy(vOtaOpCode,"1111");
        }
        
        pubLog_Debug(_FFL_, service_name, "", "vOtaOpCode=[%s]",vOtaOpCode);
        pubLog_Debug(_FFL_, service_name, "", "pOp_Code=[%s]",pOp_Code);
    
    
    EXEC SQL WHENEVER SQLERROR DO error_handler32(transIN,transOUT,
       "200",temp_buf,LABELDBCHANGE,CONNECT0);
    EXEC SQL WHENEVER SQLWARNING DO    warning_handler32();
    EXEC SQL WHENEVER NOT FOUND CONTINUE;
    

    /** ----------------------ҵ����ʼ---------------------- **/
    /* ���������עΪ�գ�������ע=ϵͳ��ע */
    if(strlen(ltrim(rtrim(pOp_Note)))==0){
    	strcpy(pOp_Note,pSystem_Note);
    }
    

    
    /* ȡ�ù�����Ϣ */
    EXEC SQL SELECT TO_CHAR(SysDate,'YYYYMMDD'),
                    TO_CHAR(SysDate,'YYYYMMDD HH24:MI:SS'),
                    TO_CHAR(SysDate,'yyyymm'),
                    TO_CHAR(add_months(SysDate,1),'yyyymm')||'01 00:00:00',TO_CHAR(SysDate,'DD')
               INTO :vTotal_Date,:vSysDate,:sV_YYYYMM,:iEffectTime,:vnowdate
               FROM DUAL;


    
    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
             pubLog_Debug(_FFL_, service_name, "", "ȡ������Ϣ����!");
             strcpy(oRetCode,"110401");
             strcpy(oRetMsg, "ȡ������Ϣ����!");    
             PUBLOG_DBDEBUG(service_name);
             pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
             RAISE_ERROR(oRetCode,oRetMsg);
         }
    /*--��֯���������ȡorg_id edit by liuweib at 19/02/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pLogin_No,vOrgId);
	if(ret <0)
	{
		sprintf(oRetCode,"%06d",110491);
		strcpy(oRetMsg,"��ѯ����org_idʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	Trim(vOrgId);
	/*---��֯���������ȡorg_id edit by liuweib at 19/02/2009---end*/
 /*--������֯�������� liuhao    200090520 begin*/	
 	
	sprintf (oRetCode, "%06d", spublicRight( LABELDBCHANGE, pLogin_No, vPassWord, pOp_Code ));
	if (strcmp(oRetCode, "000000") != 0)
	{
		strcpy(oRetMsg,"����Ȩ����֤ʧ��!");
		pubLog_Debug(_FFL_, service_name, "000001", oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}                                      
/*--������֯�������� liuhao    200090520 end*/	
    
    
    vOtaCount=0;
    /*	**EXEC SQL select count(1)
    	**			into :vOtaCount
    	**	from dsimrespasswd
    	**	where sim_no=:pSim_No;*/
	/*	**EXEC SQL select count(1)
    	**			into :vOtaCount    			
    	**	from sotasimtype
    	**	where res_code in 
    	**	(select sim_type from dSimRes where sim_no=:pSim_No);
    if(SQLCODE!=0)
    {
    	 strcpy(oRetCode,"110442");
         strcpy(oRetMsg, "ȡOTA��Ϣʧ��");
         PUBLOG_DBDEBUG(service_name);
         pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
         RAISE_ERROR(oRetCode,oRetMsg);
    }
    if(strcmp(vOtaOpCode,"118a")==0)
    {
    	if(vOtaCount==0)
    	{
    		strcpy(oRetCode,"110442");
         	strcpy(oRetMsg, "�˿�����OTA��,�뵽��ͨ����ҳ����в���");
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
         	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    }
    else
    {
    	if(vOtaCount>0)
    	{
    		strcpy(oRetCode,"110442");
         	strcpy(oRetMsg, "�˿���OTA��,�뵽OTA����ҳ����в���");
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
         	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    }
    if(vOtaCount==0)
    {*//*modify by wangdp 20080728 ���ڶ�ȫ��ͨƷ�ƿ���OTA��׼��������Ʒ��OTA����������������ϵ�����*/
    	init(vSimPassWd);
    	EXEC SQL select pass_word
    			into :vSimPassWd
    			from dsimrespassword
    			where sim_no=:pSim_No
    			and (region_code=substr(:pBelong_Code,1,2) or region_code='99');
    	if(SQLCODE!=0 && SQLCODE!=1403)
    	{
    	 	strcpy(oRetCode,"110452");
         	sprintf(oRetMsg, "dsimrespassword��Ϣ��ȫ[%d][%s]",SQLCODE,pSim_No);
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name,"110452", oRetMsg);
         	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	else if(SQLCODE==0)
    	{
    		init(pCust_Passwd);
    		init(pUser_Passwd);
    		init(pCon_Passwd);
    		strcpy(pCust_Passwd,vSimPassWd);
    		strcpy(pUser_Passwd,vSimPassWd);
    		strcpy(pCon_Passwd,vSimPassWd);
    		
    		EXEC SQL update dsimrespassword set total_date=to_number(:vTotal_Date)
    					where sim_no=:pSim_No
    					and (region_code=substr(:pBelong_Code,1,2) or region_code='99');
    		if(SQLCODE!=0 && SQLCODE!=1403)
    		{
    		 	strcpy(oRetCode,"110453");
        	 	sprintf(oRetMsg, "dsimrespassword2��Ϣ��ȫ[%d][%s]",SQLCODE,pSim_No);
        	 	PUBLOG_DBDEBUG(service_name);
        	 	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        	 	RAISE_ERROR(oRetCode,oRetMsg);
    		}
    	}
    	
   	/*}
   	else if(vOtaCount>0)
    {
    	**	init(vSimPassWd);
    	**	EXEC SQL select pass_word
    	**			into :vSimPassWd
    	**			from dsimrespassword
    	**			where sim_no=:pSim_No
    	**			and (region_code=substr(:pBelong_Code,1,2) or region_code='99');
    	**	if(SQLCODE!=0)
    	**	{
    	**	 	strcpy(oRetCode,"110452");
        **	 	sprintf(oRetMsg, "OTA��Ϣ��ȫ[%d][%s]",SQLCODE,pSim_No);
        **	 	PUBLOG_DBDEBUG(service_name);
        **	 	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        **	 	RAISE_ERROR(oRetCode,oRetMsg);
    	**	}
    	**	init(pCust_Passwd);
    	**	init(pUser_Passwd);
    	**	init(pCon_Passwd);
    	**	strcpy(pCust_Passwd,vSimPassWd);
    	**	strcpy(pUser_Passwd,vSimPassWd);
    	**	strcpy(pCon_Passwd,vSimPassWd);
    	**	EXEC SQL update dsimrespassword set total_date=to_number(:vTotal_Date)
    	**				where sim_no=:pSim_No
    	**				and (region_code=substr(:pBelong_Code,1,2) or region_code='99');
    	**	if(SQLCODE!=0 )
    	**	{
    	**	 	strcpy(oRetCode,"110453");
        **	 	sprintf(oRetMsg, "OTA��Ϣ��ȫ2[%d][%s]",SQLCODE,pSim_No);
        **	 	PUBLOG_DBDEBUG(service_name);
        **	 	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        **	 	RAISE_ERROR(oRetCode,oRetMsg);
    	**	}
    }*//*modify by wangdp 20080728 ���ڶ�ȫ��ͨƷ�ƿ���OTA��׼��������Ʒ��OTA����������������ϵ�����*/
    				
    
    /* ȡ�ò�����ˮ */
    Trim(sIn_Login_Accept);
    if  ((strlen(sIn_Login_Accept)==0)||(strcmp(sIn_Login_Accept,"0")==0))
    {
    EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :vLogin_Accept_Char
             FROM DUAL;
             if(SQLCODE!=0){
                 pubLog_Debug(_FFL_, service_name, "", "ȡ����������ˮ����!");
                 strcpy(oRetCode,"110402");
                 strcpy(oRetMsg, "ȡ����������ˮ����!");
                 PUBLOG_DBDEBUG(service_name);
                 pubLog_DBErr(_FFL_, service_name, "%s", "%s",oRetCode,oRetMsg);
                 RAISE_ERROR(oRetCode,oRetMsg);
             }
    
    }
    else
    {
        strcpy(vLogin_Accept_Char ,sIn_Login_Accept);
    }
    
    Trim(vLogin_Accept_Char);
    iLoginAccept=atol(vLogin_Accept_Char);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "vLogin_Accept_Char  �� [%s]",vLogin_Accept_Char);
    #endif     
    
        
	
	
	sprintf(oRetCode, "%06d", spublicLimit(pLogin_No,pOp_Code,vLimitCause,vDealType));
	
	Trim(vLimitCause);
	pubLog_Debug(_FFL_, service_name, "", "oRetCode[%s]vLimitCause[%s]vDealType[%s]",oRetCode,vLimitCause,vDealType);
	if(strcmp(oRetCode,"000000")!=0)
	{
		if(strcmp(vDealType,"0")!=0)
		{
    		pubLog_Debug(_FFL_, service_name, "", "vLimitCause[%s]oRetCode[%s]", vLimitCause, oRetCode);
    		strcpy(oRetMsg,vLimitCause);
    		PUBLOG_DBDEBUG(service_name);
    		pubLog_DBErr(_FFL_, service_name, oRetCode,oRetMsg);
    		RAISE_ERROR(oRetCode,oRetMsg);
		}
	}

    /*----------------------- �����û�������Ϣ����ʼ ----------------------*/     
    
	if(strcmp(pOp_Code, "1108") == 0)
	{   vFlag = 0;
		if((strstr(pUsedFunc_List, "19") && strstr(pUsedFunc_List, "20"))
		  ||(strstr(pUsedFunc_List, "19") && strstr(pUsedFunc_List, "20")))
		{
            strcpy(oRetCode,"110814");
            strcpy(oRetMsg,"����ͬʱ����[19][20]�ط�!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
		}
		if(strstr(pUsedFunc_List, "19")) vFlag = 1;
		if(strstr(pUsedFunc_List, "20")) vFlag = 2;
	}
    pubLog_Debug(_FFL_, service_name, "", "------------------  4  ----------");   
    
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,pCust_List,sizeof(pCust_List)-1);
pubLog_Debug(_FFL_, service_name, "", "11111111111111  Tmp_List=[%s]",vTmp_List);    
    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;
  	/*delete by liujj for ���ٷǿ�����־��� at2011-04-15
  	printf("vTmp_List=[%s]\n",vTmp_List);*/
    for(i=0;i<=tmplen;i++){
    if(vTmp_List[i]=='~'){
            k=k+1;
            end = i;
            num = end - begin + 1;
		/**
            vTmp_List[i]='\0';
       printf("vTmp_List[%d]=[%s]\n",k,substr(vTmp_List,begin,num));
       **/	
            if(k==1)  strcpy(vSm_Code,substr(vTmp_List,begin,num));
            if(k==2)  strcpy(vService_Type,substr(vTmp_List,begin,num));
            if(k==3)  vLimit_Owe=atof(substr(vTmp_List,begin,num));
            if(k==4)  strcpy(vAttr_Code,substr(vTmp_List,begin,num));
           /* if(k==5)  strcpy(vCredit_Code,substr(vTmp_List,begin,num));*/
            if(k==6)  vCredit_Value=atoi(substr(vTmp_List,begin,num));
            if(k==7)  vBill_Type=atoi(substr(vTmp_List,begin,num));
            if(k==8)  strcpy(vPay_Code,substr(vTmp_List,begin,num));
            if(k==9)  strcpy(vPay_Type,substr(vTmp_List,begin,num));
            if(k==10) vCmd_Right=atoi(substr(vTmp_List,begin,num));
            if(k==11) strcpy(vInnet_Type,substr(vTmp_List,begin,num));
            if(k==12) strcpy(vMachine_Code,substr(vTmp_List,begin,num));
            if(k==13) strcpy(vOther_Attr,substr(vTmp_List,begin,num));
            if(k==14) strcpy(vCheck_No,substr(vTmp_List,begin,num));
            if(k==15) strcpy(vBank_Code,substr(vTmp_List,begin,num));  
            if(k==16) strcpy(pGrade_code,substr(vTmp_List,begin,num));
            if(k==17) strcpy(pAreaCode,substr(vTmp_List,begin,num));
            if(k==18) strcpy(pGroupNo,substr(vTmp_List,begin,num));
            if(k==19) strcpy(sV_Iccid,substr(vTmp_List,begin,num));

            begin = end +2;
        }
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "ҵ��Ʒ�ƴ��� =[%s]",     vSm_Code        );
        pubLog_Debug(_FFL_, service_name, "", "�������ʹ��� =[%s]",     vService_Type   );        
        pubLog_Debug(_FFL_, service_name, "", "������       =[%10.2f]", vLimit_Owe      );
        pubLog_Debug(_FFL_, service_name, "", "�û�����     =[%s]",     vAttr_Code      );
        pubLog_Debug(_FFL_, service_name, "", "�����ȴ���   =[%s]",     vCredit_Code    );
        pubLog_Debug(_FFL_, service_name, "", "������       =[%d]",     vCredit_Value   );
        pubLog_Debug(_FFL_, service_name, "", "��������     =[%d]",     vBill_Type      );
        pubLog_Debug(_FFL_, service_name, "", "���Ѵ���     =[%s]",     vPay_Code       );
        pubLog_Debug(_FFL_, service_name, "", "��������     =[%s]",     vPay_Type       );
        pubLog_Debug(_FFL_, service_name, "", "Ȩ��ֵ       =[%d]",     vCmd_Right      );
        pubLog_Debug(_FFL_, service_name, "", "��������     =[%s]",     vInnet_Type     );
        pubLog_Debug(_FFL_, service_name, "", "��������     =[%s]",     vMachine_Code   );
        pubLog_Debug(_FFL_, service_name, "", "��������     =[%s]",     vOther_Attr     );
        pubLog_Debug(_FFL_, service_name, "", "֧Ʊ����     =[%s]",     vCheck_No       );
        pubLog_Debug(_FFL_, service_name, "", "���д���     =[%s]",     vBank_Code      );                
        pubLog_Debug(_FFL_, service_name, "", "�û�����     =[%s]",     pGrade_code      );                
        pubLog_Debug(_FFL_, service_name, "", "�û�����     =[%s]",     pAreaCode      );  
        pubLog_Debug(_FFL_, service_name, "", "pGroupNo     =[%s]",     pGroupNo      );  
        pubLog_Debug(_FFL_, service_name, "", "sV_Iccid     =[%s]",     sV_Iccid      );            
        
    #endif   
    
    init(vAttr_Code);

    /* �û��еĿͻ�������pubOpenCustConfirm */
    ret=-1;
    if(strcmp(sIn_CustCreate,"Y")==0)
    {
        ret=pubOpenCustConfirm(ORDERIDTYPE_USER,pId_No,sIn_CustMsg,sIn_CustId,sIn_CustContact,sIn_CustAttr,sIn_CustAdd,vTotal_Date,
                               vLogin_Accept_Char,vSysDate,sV_YYYYMM,oRetMsg,sTrue_flag,sCustinfoType,sagreement_type,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�ͻ���������! ret = [%d]",ret);
            strcpy(oRetCode,"110404");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        /*ng����	
        **EXEC SQL update dcustdoc set cust_passwd =:pUser_Passwd
        **			where cust_id=to_number(:pCust_Id);
        **if(SQLCODE!=0)
        **{
        **	pubLog_Debug(_FFL_, service_name, "", "���¿ͻ��������! SQLCODE = [%d]",SQLCODE);
        **    strcpy(oRetCode,"110404");
        **    strcpy(oRetMsg,"���¿ͻ��������!");
        **    PUBLOG_DBDEBUG(service_name);
        **    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        **    RAISE_ERROR(oRetCode,oRetMsg);
        **}
         ng����*/   
    	/*ng���� by yaoxc begin*/
    	init(v_parameter_array);
    	
    	strcpy(v_parameter_array[0],pUser_Passwd);
    	strcpy(v_parameter_array[1],pCust_Id);
    	
		v_ret=OrderUpdateCustDoc(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
								pOp_Code,atol(vLogin_Accept_Char),pLogin_No,pOp_Note,
								pCust_Id,
								" cust_passwd =:pUser_Passwd","",v_parameter_array);
		/*printf("OrderUpdateCustDoc ,ret=[%d]\n",v_ret);*/
		if(0 > v_ret)
		{
			pubLog_Debug(_FFL_, service_name, "", "���¿ͻ��������! SQLCODE = [%d]",SQLCODE);
            strcpy(oRetCode,"110404");
            strcpy(oRetMsg,"���¿ͻ��������!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
		}
    	/*ng���� by yaoxc end*/
        	
    }
    else if(strcmp(pOp_Code,"1104")==0)
    {
    	pubLog_Debug(_FFL_, service_name, "", "sIn_CustId=[%s]",sIn_CustId);
    	vFavNum=0;
        EXEC SQL select count(*) into :vFavNum from dCustDoc where cust_id=to_number(:pCust_Id);
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "��ѯ�ͻ�����ʧ�ܻ�ͻ�������! SQLCODE = [%d]",SQLCODE);
            strcpy(oRetCode,"110404");
            strcpy(oRetMsg,"��ѯ�ͻ�����ʧ�ܻ�ͻ�������");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s",oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        if(vFavNum!=1)
        {
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�������! SQLCODE = [%d]",SQLCODE);
            strcpy(oRetCode,"110404");
            strcpy(oRetMsg,"�ͻ�������");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }

    if (strcmp(pOp_Code ,"1106")!=0 && strcmp(pOp_Code ,"1108")!=0) 
    {
        sprintf(vAttr_Code,"%.2s000Y",pGrade_code); 
        
        /* �������� vAttr_Code = dCustDoc.OWNER_GRADE + '000' 
        **EXEC SQL select OWNER_GRADE||'000Y' into :vAttr_Code
        **         from dCustDoc 
        **         where CUST_ID = :pCust_Id;
        **if (SQLCODE != 0 )
        **{
        **    strcpy(oRetCode,"110488");
        **    strcpy(oRetMsg, "��ѯ dCustDoc �����!");
        **    PUBLOG_DBDEBUG(service_name);
        **    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        **    RAISE_ERROR(oRetCode,oRetMsg);	
        **}    
        */
        strncpy(sV_NewOldFlag,"1",1);
    }
    else
    {
    	strcpy(vAttr_Code , "00000Y");
    	strncpy(sV_NewOldFlag,"0",1);
    }
    
    Trim(vAttr_Code);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "vAttr_Code =[%s]",     vAttr_Code        );
    #endif   
             
    
    
                                                    
    /*----------------------- �����û�������Ϣ������ ----------------------*/
             
    /* ��ֳ����пͻ���Ϣ�� */     
    if(strlen(ltrim(rtrim(pCBXCustInfo)))!=0)
    {    
        memset(vTmp_List,0x00,sizeof(vTmp_List));    
        memcpy(vTmp_List,pCBXCustInfo,sizeof(pCBXCustInfo)-1);
        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;     
        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
                /*
                vTmp_List[i]='\0';
                */
                if(k==1) strcpy(sV_Cust_Name,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sV_Cust_Address,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sV_Id_Type,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sV_Id_Iccid,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�����  =[%s]",sV_Cust_Name);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ���ַ  =[%s]",sV_Cust_Address);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������ =[%s]",sV_Id_Type);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������ =[%s]",sV_Id_Iccid);  
        #endif      
     
             
    }         
                                                    
    /*--------------------------- ����������Ϣ�� --------------------------*/
    if(strlen(ltrim(rtrim(pAssure_List)))!=0)
    {
        memset(vTmp_List,0x00,sizeof(vTmp_List));    
        memcpy(vTmp_List,pAssure_List,sizeof(pAssure_List)-1);
        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
                /*
                vTmp_List[i]='\0';
                */
                if(k==1) strcpy(vAssure_Name,substr(vTmp_List,begin,num));
                if(k==2) strcpy(vId_Type,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(vAssure_Id,substr(vTmp_List,begin,num));
                if(k==4) strcpy(vAssure_Phone,substr(vTmp_List,begin,num));
                if(k==5) strcpy(vAssure_Address,substr(vTmp_List,begin,num));
                if(k==6) strcpy(vAssure_ZIP,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�� �� ��  =[%s]",vAssure_Name);
            pubLog_Debug(_FFL_, service_name, "", "֤������  =[%s]",vId_Type);
            pubLog_Debug(_FFL_, service_name, "", "������֤��=[%s]",vAssure_Id);
            pubLog_Debug(_FFL_, service_name, "", "�����˵绰=[%s]",vAssure_Phone);
            pubLog_Debug(_FFL_, service_name, "", "�����˵�ַ=[%s]",vAssure_Address);
            pubLog_Debug(_FFL_, service_name, "", "�������ʱ�=[%s]",vAssure_ZIP);            
        #endif
    }
    /*------------------------- ����������Ϣ�� --------------------------*/

    /*--------------------------- ����������Ϣ�� --------------------------*/
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,pFee_List,sizeof(pFee_List)-1);
    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;


	pubLog_Debug(_FFL_, "mfy test", "", "11111111111111111111111111111111111111vTmp_List=[%s]", vTmp_List);

    for(i=0;i<=tmplen;i++){
    if(vTmp_List[i]=='~'){
            k=k+1;
            end = i;
            num = end - begin + 1;
            /*
            vTmp_List[i]='\0';
            */
      if(k==1) vCash_Pay=atof(substr(vTmp_List,begin,num));
      if(k==2) vCheck_Pay=atof(substr(vTmp_List,begin,num));
      if(k==3) vPrepay_Fee=atof(substr(vTmp_List,begin,num));
      if(k==4) vSim_Fee=atof(substr(vTmp_List,begin,num));
      if(k==5) vMachine_Fee=atof(substr(vTmp_List,begin,num));
      if(k==6) vInnet_Fee=atof(substr(vTmp_List,begin,num));
      if(k==7) vChoice_Fee=atof(substr(vTmp_List,begin,num));
      if(k==8) vOther_Fee=atof(substr(vTmp_List,begin,num));
      if(k==9) vHand_Fee=atof(substr(vTmp_List,begin,num));
	  if(k==10) strcpy(vBunchNo, substr(vTmp_List,begin,num));
	    if(k==11) vIsNetAmount=atoi(substr(vTmp_List,begin,num));
      if(k==12) strcpy(vIsOutright,substr(vTmp_List,begin,num));
      if(k==13) vReward_Fee=atof(substr(vTmp_List,begin,num));
      if(k==14) vTax_Fee=atof(substr(vTmp_List,begin,num));

            begin = end +2;
        }
    }
    vOther_Fee = vTax_Fee - vReward_Fee;
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "�ֽ𽻿�=[%10.2f]",vCash_Pay);
        pubLog_Debug(_FFL_, service_name, "", "֧Ʊ����=[%10.2f]",vCheck_Pay);
        pubLog_Debug(_FFL_, service_name, "", "Ԥ���=[%10.2f]",vPrepay_Fee);
        pubLog_Debug(_FFL_, service_name, "", "SIM����=[%10.2f]",vSim_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vMachine_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vInnet_Fee);
        pubLog_Debug(_FFL_, service_name, "", "ѡ�ŷ�=[%10.2f]",vChoice_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vOther_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vHand_Fee);
		pubLog_Debug(_FFL_, service_name, "", "�ֻ�����=[%s]",vBunchNo);
		
		    pubLog_Debug(_FFL_, service_name, "", "�Ƿ񾻶����=[%d]",vIsNetAmount);
        pubLog_Debug(_FFL_, service_name, "", "�Ƿ���ϴ�����=[%s]",vIsOutright);
        pubLog_Debug(_FFL_, service_name, "", "�����̳��=[%10.2f]",vReward_Fee);
        pubLog_Debug(_FFL_, service_name, "", "����˰��=[%10.2f]",vTax_Fee);
    #endif

    /*------------------------- ����������Ϣ�� --------------------------*/

    /*----------------------- �����ʼ��ʵ���Ϣ�� --------------------------*/
    if(strlen(ltrim(rtrim(pPost_List)))!=0){    
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        memcpy(vTmp_List,pPost_List,sizeof(pPost_List)-1);
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
    
        for(i=0;i<=tmplen;i++){
        if(vTmp_List[i]=='~'){
                k=k+1;        	
                end = i;
                num = end - begin + 1;
                /*
                vTmp_List[i]='\0';
                */
                if(k==1) strcpy(vPost_Flag,substr(vTmp_List,begin,num));
                if(k==2) strcpy(vPost_Type,substr(vTmp_List,begin,num));
                if(k==3) strcpy(vPost_Address,substr(vTmp_List,begin,num));
                if(k==4) strcpy(vPost_ZIP,substr(vTmp_List,begin,num));
                if(k==5) strcpy(vPost_Fax,substr(vTmp_List,begin,num));
                if(k==6) strcpy(vPost_Mail,substr(vTmp_List,begin,num));
                if(k==7) strcpy(vPost_Content,substr(vTmp_List,begin,num));
        /**
        pubLog_Debug(_FFL_, service_name, "", "---result=[%s],i=[%d],k=[%d],begin=[%d],end=[%d],num=[%d]",
                   vTmp_List,i,k, begin,end,num);
        **/
                begin = end +2;
            }
        }
       
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ���־=[%s]",vPost_Flag);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ�����=[%s]",vPost_Type);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ���ַ=[%s]",vPost_Address);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ��ʱ�=[%s]",vPost_ZIP);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ�����=[%s]",vPost_Fax);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ�Mail=[%s]",vPost_Mail);
            pubLog_Debug(_FFL_, service_name, "", "�ʼ��ʵ�Content=[%s]",vPost_Content);
        #endif
    }
    else
    {
        strcpy(vPost_Flag,"0");	
    	
    }

    /*----------------------- �����ʼ��ʵ���Ϣ�� --------------------------*/


    /*----------------------- �����ײ���Ϣ����ʼ --------------------------*/
    if(strlen(ltrim(rtrim(pPackage_List)))!=0){
        memset(vTmp_List,0x00,sizeof(vTmp_List));       
        memcpy(vTmp_List,pPackage_List,sizeof(pPackage_List)-1);
		pubLog_Debug(_FFL_, service_name, "", "----Tmp_List=[%s]",vTmp_List);        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                end = i;
                num = end - begin + 1;
                /*
                vTmp_List[i]='\0';
                */
                strcpy(vBillMode[k],substr(vTmp_List,begin,num));
         /* pubLog_Debug(_FFL_, service_name, "", "---result=[%s],i=[%d],k=[%d],begin=[%d],end=[%d],num=[%d]", 
                   vTmp_List,i,k, begin,end,num); */
                begin = end +2;
                k++;                
            }
        }
        vBillModeNum = k;
        
        #ifdef _DEBUG_
            for(i=0; i<=vBillModeNum; i++){
                pubLog_Debug(_FFL_, service_name, "", "�ʷѴ���=[%s]",vBillMode[i]);
            }           
        #endif
        
        
        
        
        
        
        /* ADDED BY ZHCY : 2009-06-01 */
        /*
        *	R_JLMob_zhangyh_CRM_PD3_2009_0157�����ڶ�188���������ʷ��ײͽ���ϵͳ���Ƶ�����
        *	188�����������ʷ�������"ȫ��ͨ88ϵ���ײ�"
        */
        iCount = 0;
        EXEC SQL select count(*) into :iCount
        		from dphoneheadattr 
        		where phone_head = :l_sPhoneHead;
        if ( SQLCODE != 0 )
        {
      		 pubLog_Debug(_FFL_, service_name, "", "�жϺŶ�[%s]�Ƿ��������Ŷγ���! SQLCODE = [%d]",l_sPhoneHead,SQLCODE);
        	strcpy(oRetCode, "110895");
        	sprintf(oRetMsg, "�жϺŶ�[%s]�Ƿ��������Ŷγ���! SQLCODE = [%d]", l_sPhoneHead, SQLCODE);
        	PUBLOG_DBDEBUG(service_name);
        	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
 
        	RAISE_ERROR(oRetCode,oRetMsg);
        }
		 if ( iCount > 0 )
		        {
		        	
		        	ret = 0;
		        	for(i=0; i<=vBillModeNum; i++)
		        	{
		                BindPhoneHeadMode(l_sRegionCode, vSm_Code, l_sPhoneHead, vBillMode[i], l_sFlag, oRetCode, oRetMsg);
			        	if ( l_sFlag[0] == 'Y' )
			        	{
			        		strcpy(oRetCode, "000000");
			        		strcpy(oRetMsg, "�����ɹ�");
			        		ret = 1;
			        		break;
			        	}                
    }        
	if ( ret != 1 )
	{
        	 pubLog_Debug(_FFL_, service_name, "", "��ѡ�ײͲ���������Ŷ�[%s]",l_sPhoneHead);
         	strcpy(oRetCode, "110890");
        	sprintf(oRetMsg, "��ѡ�ײͲ���������Ŷ�[%s]!", l_sPhoneHead);
        		PUBLOG_DBDEBUG(service_name);
        	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        	RAISE_ERROR(oRetCode,oRetMsg);
        }
	  }
	}        

	if(strcmp(pOp_Code ,"1108") == 0 && vFlag != 0)
	{
		memset(vModeCode, '\0', sizeof(vModeCode));
		strcpy(vModeCode,vBillMode[0]);
		pubLog_Debug(_FFL_, service_name, "", "vMode = [%s] [%s]", vModeCode, pBelong_Code);
		EXEC SQL select count(*) into :iCount from sBillModeDetail 
		 	 where region_code = substr(:pBelong_Code,1,2) 
			 and mode_code = :vModeCode
			 and detail_code in (select function_bill from sBillFuncFav 
						where region_code = substr(:pBelong_Code,1,2)
					    and	  function_code = decode(:vFlag, 1,'19',2,'20')); 
		if(iCount == 0)
		{
			strcpy(oRetCode, "110873");
			sprintf(oRetMsg, "�������ط����û���ǰ���ʷѳ�ͻ[%d]!",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
	}
    /*----------------------- �����ײ���Ϣ������ --------------------------*/

    /*Add by liujj for  a��ͨTD���������Կ�ͨʡ�ڡ����ڡ��������� 
    					b����ͨTD����������ͨ85ǩԼ��Ϣ at 2011��10��25�� Begin*/
    /*�жϿ��������Ƿ�Ϊ��ͨTD����*/
    iIsTd = 0;
    EXEC SQL SELECT count(1) into :iIsTd 
    				from sbillmodedetail 
    				where detail_code = 'g085' 
    					and region_code = :l_sRegionCode
    					and mode_code = Trim(:vBillMode[0]);
    if(SQLCODE != 0 )
    {
    	strcpy(oRetCode,"110481");
        strcpy(oRetMsg,"��ѯ��sbillmodedetailʧ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    if(iIsTd > 0)
    {
    	/*��������Ϊ��ͨTD������������ͨʡ�ڡ����ڡ���������*/
    	if(strstr(pUsedFunc_List, "15") || strstr(pUsedFunc_List, "16")||(strstr(pUsedFunc_List, "17")))
		{
            strcpy(oRetCode,"110482");
            strcpy(oRetMsg,"��ͨTD������������ͨʡ�ڡ����ڡ���������!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
		}
    }
    else
    {
    	/*����ͨTD����������ͨ85ǩԼ��Ϣ*/
    	if((strstr(pUsedFunc_List, "85")))
		{
            strcpy(oRetCode,"110483");
            strcpy(oRetMsg,"����ͨTD����������ͨ85ǩԼ��Ϣ!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
		}
    }
    /*Add by liujj at 2011��10��25�� End*/
 


    /* �����롢SIM����Ч��*/
    ret=-1;
    ret=CheckPhoneSimNo(pPhone_No,     vOrg_Code,      vSm_Code,      
                        pSim_No,       pSim_Type,      sV_Iccid,pLogin_No,oRetMsg,vOrgId);
    if(ret != 0){
        pubLog_Debug(_FFL_, service_name, "", "�����롢SIM����Ч�Դ���[%s]",oRetMsg);
        sprintf(oRetCode,"%d",ret);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }

    /* ���º��롢SIM����Ϣ */
    ret=-1;
    ret=pubOpenAcctRes(pId_No,        pPhone_No,      pSim_No,
                       pBelong_Code,  pLogin_No,      vLogin_Accept_Char, 
                       pOp_Code,      vTotal_Date,    vSysDate,pGroupNo);
    if(ret != 0)
    {
    	if(ret==-2)
    	{
            pubLog_Debug(_FFL_, service_name, "", "���º���SIM����Դ����,�ÿ�Ŀǰ����ʹ�ã�����30����֮��ʹ�ã�������������! ret = [%d]",ret);
            strcpy(oRetCode,"110404");
            strcpy(oRetMsg, "�ÿ�Ŀǰ����ʹ�ã�����30����֮��ʹ�ã�������������!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
           
        pubLog_Debug(_FFL_, service_name, "", "���º���SIM����Դ����! ret = [%d]",ret);
        strcpy(oRetCode,"110404");
        strcpy(oRetMsg, "���º���SIM����Դ����!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }

    /* ��¼�ͻ����û����ʻ���Ϣ */
    ret=-1;    
    if(strncmp(vSm_Code,"z0",2)==0 || strncmp(vSm_Code,"cb",2)==0)
    {
    	if(strlen(rtrim(pCust_Passwd))==0)
    	{
    		strcpy(pCust_Passwd,pUser_Passwd);
    	}
    	
    }
    strcpy(vCredit_Code,"S");
    ret=pubOpenAcctConfirm(pCust_Id,    pCon_Id,       pId_No, 
                         pCust_Passwd,  pUser_Passwd,  pCon_Passwd,
                         vLimit_Owe,    pPhone_No,     vSm_Code ,     
                         vService_Type, vAttr_Code,    vCredit_Code,  
                         vCredit_Value, vPay_Code,     vPay_Type,     
                         vBill_Type,    vCmd_Right,    pSim_No,       
                         pBelong_Code,  pOp_Note,      pSystem_Note,
                         pLogin_No,     pIp_Address,   vLogin_Accept_Char, 
                         pOp_Code,      vInnet_Type,   vMachine_Code,
                         vAssure_Name,  vId_Type,      vAssure_Id,
                         vAssure_Phone, vAssure_Address,vAssure_ZIP,
                         vPost_Flag,    vPost_Type,   vPost_Address,
                         vPost_ZIP,     vPost_Fax,    vPost_Mail, vPost_Content,
                         vCash_Pay,     vCheck_Pay,   vPrepay_Fee,
                         vSim_Fee,      vMachine_Fee, vInnet_Fee,
                         vChoice_Fee,   vOther_Fee,   vHand_Fee,
                         vTotal_Date,   vSysDate,
                         sV_Cust_Name , sV_Cust_Address ,
                         sV_Id_Type , sV_Id_Iccid ,sV_NewOldFlag,"0",pAreaCode,pGroupNo,sTrue_flag,sCustinfoType,sagreement_type,vOrgId);
    if(ret != 0){
        pubLog_Debug(_FFL_, service_name, "", "����ͻ� �û� �ʻ���Ϣ����! ret = [%d]",ret);
        strcpy(oRetCode,"110405");
        strcpy(oRetMsg, "����ͻ� �û� �ʻ���Ϣ����!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    
	/*���ֶ����� �����û��˺���Ϣ�� 2012/5/22 13:47:45 S.H begin */
	if(strcmp(vaccflag,"Y")==0)
	{
		/*ȡ�˺���Ϣ*/
		/*EXEC SQL SELECT CONTRACT_NO INTO vcontractNo FROM DCUSTMSG WHERE PHONE_NO=:pLogin_No;
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, service_name, "", "ȡ�˺���Ϣ����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
			strcpy(oRetCode,"110480");
			strcpy(oRetMsg,"ȡ�˺���Ϣ����!");
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}*/
		
		pubLog_Debug(_FFL_, service_name, "", "pCon_Id:[%s]",pCon_Id);
		pubLog_Debug(_FFL_, service_name, "", "pId_No:[%s]",pId_No);
		
		pubLog_Debug(_FFL_, service_name, "", "pPhone_No:[%s]",pPhone_No);
		pubLog_Debug(_FFL_, service_name, "", "vcycleDay:[%s]",vcycleDay);
		
		pubLog_Debug(_FFL_, service_name, "", "iLoginAccept:[%ld]",iLoginAccept);
		pubLog_Debug(_FFL_, service_name, "", "pLogin_No:[%s]",pLogin_No);
		
		pubLog_Debug(_FFL_, service_name, "", "pOp_Note:[%s]",pOp_Note);
		pubLog_Debug(_FFL_, service_name, "", "l_sRegionCode:[%s]",l_sRegionCode);
		pubLog_Debug(_FFL_, service_name, "", "pOp_Code:[%s]",pOp_Code);
		
		
		iaccnum=pubCustAcctCycle(atol(pCon_Id),atol(pId_No),pPhone_No,atol(vcycleDay),iLoginAccept,pLogin_No,pOp_Note,l_sRegionCode,pOp_Code);
		if(iaccnum != 0)
		{
			pubLog_Debug(_FFL_, service_name, "", "���ú���pubCustAcctCycle����! ret = [%d]",iaccnum);
			strcpy(oRetCode,"110405");
			strcpy(oRetMsg, "�����û���������Ϣ�����!");
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
	}
	
	
	/*���ֶ����� �����û��˺���Ϣ�� 2012/5/22 13:47:45 S.H end */

    /***************add for ��𾻶���� 2012.6.18*********************/
    Trim(vIsOutright);
    if( vIsNetAmount == 0 && strcmp(vIsOutright,"Y") == 0)
    {
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "��ˮ=[%s]",sIn_Login_Accept);
        pubLog_Debug(_FFL_, service_name, "", "group_id=[%s]",pGroupNo);
        
        pubLog_Debug(_FFL_, service_name, "", "������=[%s]",pSim_Type);
        pubLog_Debug(_FFL_, service_name, "", "��������=[%s]",pOp_Code);
        pubLog_Debug(_FFL_, service_name, "", "��������=[%s]",pLogin_No);
        
        pubLog_Debug(_FFL_, service_name, "", "����ʱ��=[%s]",vSysDate);
        pubLog_Debug(_FFL_, service_name, "", "�мۿ�-��ʼ����,����-����=[%s]",pPhone_No);
        pubLog_Debug(_FFL_, service_name, "", "�мۿ�-��������,����-����=[%s]",pPhone_No);
        pubLog_Debug(_FFL_, service_name, "", "Ʒ��,�мۿ�-��¼xx=[%s]",vSm_Code);
		    pubLog_Debug(_FFL_, service_name, "", "��������=[1]");
		    
		    pubLog_Debug(_FFL_, service_name, "", "�����,Ӧ��(��)=[%10.2f]",vPrepay_Fee);
		    pubLog_Debug(_FFL_, service_name, "", "ʵ��(��)[%10.2f]",vCash_Pay);
		    
        pubLog_Debug(_FFL_, service_name, "", "�����̳��=[%10.2f]",vReward_Fee);
        pubLog_Debug(_FFL_, service_name, "", "����˰��=[%10.2f]",vTax_Fee);
        pubLog_Debug(_FFL_, service_name, "", "ҵ�����=[04]");
    #endif

        EXEC SQL insert into wnetamountdetail 
                             (LOGIN_ACCEPT, GROUP_ID, RES_CODE, 
                             OP_CODE, LOGIN_NO, OP_TIME, 
                             BEGIN_NO, END_NO, SM_CODE, 
                             SUM_NUM, SHOULD_FEE, REAL_FEE, 
                             REWARD_FEE, TAX_FEE, BUSI_CODE ) 
                       values(:sIn_Login_Accept, :pGroupNo, :pSim_Type,  
                              :pOp_Code, :pLogin_No,to_date(:vSysDate,'YYYYMMDD HH24:MI:SS'),  
                              :pPhone_No, :pPhone_No, :vSm_Code,  
                              '1', :vPrepay_Fee, :vCash_Pay,  
                              :vReward_Fee, :vTax_Fee, '04');
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetail�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(oRetCode,"110407");
            strcpy(oRetMsg,"����wnetamountdetail�����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        
        EXEC SQL insert into wnetamountdetailhis
                             (LOGIN_ACCEPT, GROUP_ID, RES_CODE, 
                             OP_CODE, LOGIN_NO, OP_TIME, 
                             BEGIN_NO, END_NO, SM_CODE, 
                             SUM_NUM, SHOULD_FEE, REAL_FEE, 
                             REWARD_FEE, TAX_FEE, BUSI_CODE ,
                             UPDATE_OP_CODE, UPDATE_LOGIN, UPDATE_NOTE, 
                             UPDATE_TIME, UPDATE_ACCEPT, UPDATE_TYPE) 
                       values(:sIn_Login_Accept, :pGroupNo, :pSim_Type,  
                              :pOp_Code, :pLogin_No,to_date(:vSysDate,'YYYYMMDD HH24:MI:SS'),  
                              :pPhone_No, :pPhone_No, :vSm_Code,  
                              '1', :vPrepay_Fee, :vCash_Pay,  
                              :vReward_Fee, :vTax_Fee, '04',
                              :pOp_Code,:pLogin_No,'ȫ��ͨ��ͨ����',
                              sysdate,:sIn_Login_Accept,'a');
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetailhis�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(oRetCode,"110408");
            strcpy(oRetMsg,"����wnetamountdetailhis�����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }
    /***************add for ��𾻶���� 2012.6.18*********************/
    /***************add for ��������֤ǰ̨����ķǾ������Ƿ���ȷ 2012.10.29 liujj begin*******/
    else
    {
    	iRet = 0;
    	iRet = pubIsNetAmount(pPhone_No,pBelong_Code,vSm_Code,vPrepay_Fee);
    	if(iRet == 0)
    	{
    		pubLog_Debug(_FFL_, service_name, "", "ҳ��������жϾ�Ϊ�Ǿ���[%s]!",pPhone_No);
    	}
    	else if(iRet == 1)
    	{
				pubLog_Debug(_FFL_, service_name, "", "�����жϺ���[%s]������Ҫ����!",pPhone_No);
				pubLog_Debug(_FFL_, service_name, "", "ҳ�洫�����:[%d][%s]!",vIsNetAmount,vIsOutright);
				strcpy(oRetCode,"110412");
				strcpy(oRetMsg,"�����ж�Ϊ����,��ҳ�洫���������,�����²���!");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	else
    	{
				strcpy(oRetCode,"110413");
				strcpy(oRetMsg,"�ж��Ƿ񾻶�ʧ��!");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
    	}
    }
    /***************add for ��������֤ǰ̨����ķǾ������Ƿ���ȷ 2012.10.29 liujj end*******/
		
    /* ��¼�û������Ϣ */
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "��һλ�Ƿ������s��b ���������r ���  �ڶ�λ�Ƿ����ط�ѡ��ť��y �����n û�е�� = [%s] ",   pRent_Flag );
    #endif
    if(strncmp(&pRent_Flag[0],"r",1)==0)
    {
        EXEC SQL insert into dCustRMsgHis(Id_no,rent_month,rent_code,machine_code,Machine_fee,Sim_fee,UPDATE_ACCEPT,
                                          UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE)
                     values(:pId_No,12,'0',:vMachine_Code,:vMachine_Fee,:vSim_Fee,to_number(:vLogin_Accept_Char),
                            :vSysDate,:vTotal_Date,:pLogin_No,'a',:pOp_Code);
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "�����û������ʷ��Ϣ����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(oRetCode,"110405");
            strcpy(oRetMsg,"�����û������ʷ��Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        EXEC SQL insert into dCustRMsg(Id_no,rent_month,rent_code,machine_code,Machine_fee,Sim_fee)
                     values(:pId_No,12,'0',:vMachine_Code,:vMachine_Fee,:vSim_Fee);
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "�����û������Ϣ����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(oRetCode,"110405");
            strcpy(oRetMsg,"�����û������Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        
        /*NG���� start*/
        /*
        **EXEC SQL insert into dCustType(id_no,main_code,main_note)
        **         select to_number(:pId_No),main_code,main_name
        **         from sMainCode where main_code='16';
        **if(SQLCODE!=0)
        **{
        **    pubLog_Debug(_FFL_, service_name, "", "����dCustType�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
        **    strcpy(oRetCode,"110406");
        **    strcpy(oRetMsg,"����dCustType�����!");
        **    PUBLOG_DBDEBUG(service_name);
        **    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        **    RAISE_ERROR(oRetCode,oRetMsg);
        }
        */
       EXEC SQL declare curFeeCode CURSOR FOR
			 select main_code,main_name
			 from sMainCode where main_code='16';
		EXEC SQL OPEN curFeeCode;
		while (SQLCODE==0){
			init(sMainCode);
			init(sMainNote);
			EXEC SQL FETCH curFeeCode into :sMainCode, :sMainNote;		
	        if((SQLCODE!=0) && (SQLCODE != 1403))
	        {
	            pubLog_Debug(_FFL_, service_name, "", "����dCustType�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
	            strcpy(oRetCode,"110406");
	            strcpy(oRetMsg,"����dCustType�����!");
	            PUBLOG_DBDEBUG(service_name);
	            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	            EXEC SQL CLOSE curFeeCode;
	            RAISE_ERROR(oRetCode,oRetMsg);
	        }
	        if (SQLCODE == 1403) break;
	        
			strcpy(sTdCustType.sIdNo,pId_No);
			strcpy(sTdCustType.sMainCode,sMainCode);
			strcpy(sTdCustType.sMainNote,sMainNote);     
	        ret=OrderInsertCustType(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							pOp_Code,atol(vLogin_Accept_Char),pLogin_No,pOp_Note,
							sTdCustType);
		    if (ret !=0){
	            pubLog_Debug(_FFL_, service_name, "", "����dCustType�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
	            strcpy(oRetCode,"110406");
	            strcpy(oRetMsg,"����dCustType�����!");
	            PUBLOG_DBDEBUG(service_name);
	            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		    	EXEC SQL CLOSE curFeeCode;
	            RAISE_ERROR(oRetCode,oRetMsg);		    	
		    }	
		}					   
        EXEC SQL CLOSE curFeeCode;
        /*NG���� end*/
    }

    /* */
    
    /* ��� ��e����Ϣ�� */
    if(strlen(ltrim(rtrim(sIn_E_DataList)))!=0)
    {
        memset(vTmp_List,0x00,sizeof(vTmp_List));    
        memcpy(vTmp_List,sIn_E_DataList,sizeof(sIn_E_DataList)-1);
        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;     
        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
               
                if(k==1) strcpy(sV_Cred_id,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sV_Gprs_rate,substr(vTmp_List,begin,num));
                if(k==3) strcpy(sV_Wlan_rate,substr(vTmp_List,begin,num)); 
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "ƾ֤���к�  =[%s]",sV_Cred_id);
            pubLog_Debug(_FFL_, service_name, "", "GPRS����  =[%s]",sV_Gprs_rate);
            pubLog_Debug(_FFL_, service_name, "", "WLAN����  =[%s]",sV_Wlan_rate);
        #endif    
        
        /* ������e�п������� */  
        ret = -1;
        ret = pubOpenDataCard(sV_Cred_id , pPhone_No,
              vSysDate , "1" , pId_No , pLogin_No ,
              sV_Gprs_rate , sV_Wlan_rate , oRetMsg);
              
        if (ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "������e�п�����������! ret = [%d]",ret); 
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg );
            strcpy(oRetCode,"110477");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);	
        }        
    	
    }
    
    /* Ӫ�������� */
    if(strlen(ltrim(rtrim(pPack_Id)))>=10)
    {
    	ret = -1;
    	ret=pubPack(pId_No , pPhone_No , pBelong_Code , vMachine_Code , vMachine_Fee , pPack_Id , 
    	            pLogin_No , vOrg_Code , pOp_Code , vLogin_Accept_Char , vTotal_Date , vSysDate , oRetMsg,pGroupNo,vOrgId);
        if (ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "����Ӫ������������! ret = [%d]",ret); 
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg );
            strcpy(oRetCode,"110478");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);	
        }        
    }

    /* ���ݿ����� �����ݿ� && Ԥ���>0 && ����E�� */
    Trim(vSm_Code);
    Trim(sIn_E_DataList);
    if(strcmp(vSm_Code,"d0")==0 && vPrepay_Fee>0 && strlen(sIn_E_DataList)==0)
    {
    	ret = -1;
    	ret=pubDataCred(vPrepay_Fee , pId_No , pPhone_No , vBillMode[1] , pBelong_Code , pLogin_No , vOrg_Code ,vLogin_Accept_Char , pOp_Code , vTotal_Date , vSysDate , oRetMsg);
        if (ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "�������ݿ���������! ret = [%d]",ret); 
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg );
            strcpy(oRetCode,"110479");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);	
        }        
    }
        
    /*----------------------- �����Ż���Ϣ�� --------------------------*/
    if(strlen(ltrim(rtrim(pFavour_List)))!=0){        
        memset(vTmp_List,0x00,sizeof(vTmp_List));
    pubLog_Debug(_FFL_, service_name, "", "pFavour_List=[%s]",pFavour_List);
        memcpy(vTmp_List,pFavour_List,sizeof(pFavour_List)-1);
    pubLog_Debug(_FFL_, service_name, "", "vTmp_List=[%s]",vTmp_List);
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
    
        for(i=0;i<=tmplen;i++){
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
               
    
            memset(vTmp_List1,0x00,sizeof(vTmp_List1));
            strcpy(vTmp_List1,substr(vTmp_List,begin,num));
    
    pubLog_Debug(_FFL_, service_name, "", "---------vTmp_List1=[%s]-----------",vTmp_List1);
            tmplen1=strlen(vTmp_List1);
            begin1 = 1; end1 = 1; k1=0;
    
            for(i1=0;i1<=tmplen1;i1++){
            if(vTmp_List1[i1]=='&'){
                k1=k1+1;
                end1 = i1;
                num1 = end1 - begin1 + 1;
                
               if(k1==1) strcpy(vFavour_Code[k-1],substr(vTmp_List1,begin1,num1));
               if(k1==2) strcpy(vShould_Data[k-1],substr(vTmp_List1,begin1,num1));
               if(k1==3) strcpy(vActual_Data[k-1],substr(vTmp_List1,begin1,num1));
    
                begin1 = end1 +2;
                      }
            }
   
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�Żݴ���[%s]       = [%s]",k,vFavour_Code[k-1]);
            pubLog_Debug(_FFL_, service_name, "", "�Ż�ǰ����[%s]     = [%s]",k,vShould_Data[k-1]);
            pubLog_Debug(_FFL_, service_name, "", "�Żݺ�����[%s]     = [%s]",k,vActual_Data[k-1]);
        #endif
                ret=-1;
                ret=pubOpenAcctFav(pId_No,           pPhone_No,           vSm_Code,         
                                   pBelong_Code,     vFavour_Code[k-1],   vShould_Data[k-1],
                                   vActual_Data[k-1],pLogin_No,           vLogin_Accept_Char,    
                                   pOp_Note,         pSystem_Note,        pOp_Code,            
                                   vTotal_Date,      vSysDate,vOrgId);
                if(ret != 0){
                    pubLog_Debug(_FFL_, service_name, "", "�����Ż���Ϣ����! ret = [%d]",ret);
                    strcpy(oRetCode,"110406");
                    strcpy(oRetMsg, "�����Ż���Ϣ����!");
                    PUBLOG_DBDEBUG(service_name);
                    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                    RAISE_ERROR(oRetCode,oRetMsg);
                }
                begin = end +2;
            }
        }
    }
    
    /* ��¼֧Ʊ��Ϣ */     
    if(vCheck_Pay>0){
        ret=-1;
        ret=pubOpenAcctCheck_1(vCheck_No,     vCheck_Pay,      vBank_Code,
                           pOp_Note,      pLogin_No,       vLogin_Accept_Char, 
                           pOp_Code,      vTotal_Date,     vSysDate,
                           pSystem_Note,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�޸�֧Ʊ��Ϣ����,ret=[%d]",ret);
            strcpy(oRetCode,"110409");
            strcpy(oRetMsg, "�޸�֧Ʊ��Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }

    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
    
    EXEC SQL SELECT DECODE(SUBSTR(:pBelong_Code,1,2), '99',SUBSTR(:vOrg_Code,1,2),SUBSTR(:pBelong_Code,1,2))
           INTO :mRegion_Code
           From dual;
    if (SQLCODE != 0) {
        strcpy(oRetCode,"113409");
        sprintf(oRetMsg,"��ѯRegion_code����[%d]",SQLCODE);
        PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_,service_name,"%s","%s",oRetCode,oRetMsg);                         
        RAISE_ERROR(oRetCode,oRetMsg);
    }
        
    EXEC SQL select to_char(count(*)) into :pPayFlag from sPackCode where region_code=:mRegion_Code and mach_code=:vMachine_Code;
    if(atoi(pPayFlag)>0) /* ��Ӫ����Ԥ�� */
        strcpy(pPayFlag,"j");
    else if(strcmp(vSm_Code,"d0")==0) /* ���ݿ� */
        strcpy(pPayFlag,"k");
    else
        strcpy(pPayFlag,"0");
    
    iGoodNoCount=0;
	EXEC SQL select count(*)
			into :iGoodNoCount
			from dGoodPhoneRes
			where phone_no=:pPhone_No
			and good_type in (select good_type from sgoodnotype);
	if(SQLCODE != 0) 
	{
	    strcpy(oRetCode,"113410");
        sprintf(oRetMsg,"��ѯdGoodPhoneRes����[%d]",SQLCODE);
        PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_,service_name,"%s","%s",oRetCode,oRetMsg);                         
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    if(iGoodNoCount>0)
    {
    	init(pPayFlag);
    	strcpy(pPayFlag,"J");
    }
    
    /*֧Ʊ����pay_Type��ֵ*/
    if(vCheck_Pay>0)
    {
        init(vPay_Type);
        strcpy(vPay_Type,"1");  
    }        
          
    /*payType��ֵ*/
    if(vPrepay_Fee>0)
    {
       if (iGoodNoCount>0)
        {
        	init(vPay_Type);
	    	strcpy(vPay_Type,"J");
        }
    }
    pubLog_DBErr(_FFL_,service_name,"","begin to call bodb_sDataCfm!!!");
    memset(vParamList,0,DLMAXITEMS*2*DLINTERFACEDATA);
    init(vCallServiceName);
    strcpy(vCallServiceName,"bodb_sDataCfm");
	vSendParaNum=21; /* ҵ�񹤵���������������� */
	vRecpParaNum=2;/* ҵ�񹤵���������������� */
	
	sprintf(cPrepay_Fee,"%10.2f",vPrepay_Fee);
	sprintf(cCheck_Pay,"%10.2f",vCheck_Pay);
	
	strcpy(vParamList[0][0],"pId_No");
	strcpy(vParamList[0][1],pId_No);
    
	strcpy(vParamList[1][0],"pCon_Id");
	strcpy(vParamList[1][1],pCon_Id);
	
	strcpy(vParamList[2][0],"vPrepay_Fee");
	strcpy(vParamList[2][1],cPrepay_Fee);
	
	strcpy(vParamList[3][0],"pSysDate");
	strcpy(vParamList[3][1],vSysDate);	
	
	strcpy(vParamList[4][0],"pPayFlag");
	strcpy(vParamList[4][1],pPayFlag);
	
	strcpy(vParamList[5][0],"pPay_Type");
	strcpy(vParamList[5][1],vPay_Type);    /*ȷ��ֵ������ҵ�񹤵�*/ 
	
	strcpy(vParamList[6][0],"pTotal_Date"); 
	strcpy(vParamList[6][1],vTotal_Date);
	
	strcpy(vParamList[7][0],"pLogin_Accept"); 
	strcpy(vParamList[7][1],vLogin_Accept_Char); 
	
	strcpy(vParamList[8][0],"pCust_Id"); 
	strcpy(vParamList[8][1],pCust_Id);
	
	strcpy(vParamList[9][0],"pPhone_No");
	strcpy(vParamList[9][1],pPhone_No);
	
	strcpy(vParamList[10][0],"pBelong_Code");
	strcpy(vParamList[10][1],pBelong_Code);
	
	strcpy(vParamList[11][0],"pSm_Code");
	strcpy(vParamList[11][1],vSm_Code); 
	 
	strcpy(vParamList[12][0],"pLogin_No");
	strcpy(vParamList[12][1],pLogin_No);
	
	strcpy(vParamList[13][0],"pOrg_Code");
	strcpy(vParamList[13][1],vOrg_Code);    /*�����ڹ���������select*/
	
	strcpy(vParamList[14][0],"pOp_Code"); 
	strcpy(vParamList[14][1],pOp_Code); 
	
	strcpy(vParamList[15][0],"pOp_Note"); 
	strcpy(vParamList[15][1],pOp_Note); 
	
	strcpy(vParamList[16][0],"vOrgId"); 
	strcpy(vParamList[16][1],vOrgId);
	
	strcpy(vParamList[17][0],"pGroupNo");
	strcpy(vParamList[17][1],pGroupNo);
	
	strcpy(vParamList[18][0],"pBank_Code"); 
	strcpy(vParamList[18][1],vBank_Code); 
	
	strcpy(vParamList[19][0],"pCheck_No"); 
	strcpy(vParamList[19][1],vCheck_No); 
	
	strcpy(vParamList[20][0],"vCheck_Pay"); 
	strcpy(vParamList[20][1],cCheck_Pay);
	
	v_ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
			                          ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,vLogin_Accept_Char,pOp_Code,pLogin_No,pOp_Note);			
	if (v_ret != 0) 
	{
	    strcpy(oRetCode, "295338");
	    sprintf(oRetMsg,"��ʼ���˱�,�ɷ���ˮ��ʧ�� ret=[%d]",v_ret);
		PUBLOG_DBDEBUG("s1104Cfm");
		pubLog_DBErr(_FFL_, "s1104Cfm", "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
    
    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/
    
    /* �������û���Ч�� */
    /*  �ڿ�ͨ�ײ�ʱ�ᴦ��
   ** ret=-1;
   ** ret=pubCBXExpire(pId_No,     pBelong_Code, vPrepay_Fee,
   **                  pOp_Note,   pLogin_No,    vLogin_Accept_Char, 
   **                  pOp_Code,   vTotal_Date,  vSysDate,
   **                  vSm_Code,   vBillMode[0]);
   ** if(ret != 0){
   **     pubLog_Debug(_FFL_, service_name, "", "�����û���Ч�ڴ���!");
   **     strcpy(oRetCode,"110410");
   **     strcpy(oRetMsg, "�����û���Ч�ڴ���!");
   **     PUBLOG_DBDEBUG(service_name);
   **     pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
   **     RAISE_ERROR(oRetCode,oRetMsg);
   ** }
     */
    /* ��¼�ʷ��ײ���Ϣ */

    /* ��e�в���Ҫ����������, �������뼯�Ź�˾����ͬ���󷢿������ע�⣺û���ط� */
	if(strlen(ltrim(rtrim(sIn_E_DataList)))!=0)
	{
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	
    /* ���ػ��ӿ� */   
        /* ȡ���ػ���ˮ */
    EXEC SQL SELECT sOffon.nextval  INTO :vs_Offon_accept  FROM DUAL;
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "vs_Offon_accept     = [%s]",vs_Offon_accept);
    #endif
              
    /* ��ѯ  HLR���� */
    EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
             where  phoneno_head like substr(:pPhone_No,1,7)||'%';
    if (SQLCODE != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "��ȡHLR�������!");
        strcpy(oRetCode,"110411");
        strcpy(oRetMsg, "��ȡHLR�������!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    
    Trim(sV_hlrCode);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_hlrCode      =[%s]",      sV_hlrCode);
    #endif
    
    /* update 2008-9-24*/
    if(strncmp(&pRent_Flag[1],"n",1)==0)
    {
    	if(strlen(ltrim(rtrim(pPackage_List)))!=0)
        {
    	    /* ���ʷ��ײͣ����ʷ��ײͣ�vBillMode[0] */
    	    pubLog_Debug(_FFL_, service_name, "", "vBillMode[0]=",vBillMode[0]);
            ret = newpubDefaultFunc(pId_No , pLogin_No ,
                 pBelong_Code , vSm_Code ,
                vTotal_Date , vSysDate ,
                 vLogin_Accept_Char , pOp_Code ,
                 oRetMsg , vBillMode[0],
                 defaultfuncList);
        }
        else
        {
    	    /* ���ʷ��ײ� */
            ret = newpubDefaultFunc(pId_No , pLogin_No ,
                 pBelong_Code , vSm_Code ,
                 vTotal_Date , vSysDate ,
                 vLogin_Accept_Char , pOp_Code ,
                 oRetMsg , "0",
                 defaultfuncList);   	
        }
        pubLog_Debug(_FFL_, service_name, "", "call newpubDefaultFunc ,ret=[%d][%s]!" , ret,defaultfuncList);
        if(ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "���Ĭ���ط�����,ret=[%d]!" , ret);
            strcpy(oRetCode,"110410");
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }
    
    Trim(defaultfuncList);
    Trim(pUsedFunc_List);
    pubLog_Debug(_FFL_, service_name, "", "++++++pUsedFunc_List=[%s],defaultfuncList=[%s]++++++",pUsedFunc_List,defaultfuncList);
    sprintf(pUsedFunc_List,"%s%s",pUsedFunc_List,defaultfuncList);
    
    pubLog_Debug(_FFL_, service_name, "", "liuweib -%s-%s-%s-%s-",vRegion_Code,pOp_Code,vSm_Code,vrunCode);
    /*add in 2008-9-9*/
    ret=-1;
    /* 20090401 ��֧TD����,�޸Ĺ�������+idNO add by liuweib*/
    ret=soffencommand(pId_No,vRegion_Code,pOp_Code,vSm_Code,vrunCode,offenFunc);
    if(ret != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "soffencommandȡ���ػ��������!");
        strcpy(oRetCode,"110491");
        strcpy(oRetMsg, "ȡ���ػ��������!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    
    ret=-1;
    ret=sNewfuncList(pUsedFunc_List,offenFunc,newfuncList,oldfuncList,newfuncStr,sV_hlrCode);
    if(ret != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "sNewfuncList�ϲ����ػ�ָ�����!");
        strcpy(oRetCode,"110492");
        strcpy(oRetMsg, "�ϲ����ػ�ָ�����!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    
    init(pUsedFunc_List);
    strcpy(pUsedFunc_List,oldfuncList);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "pUsedFunc_List      =[%s]",      pUsedFunc_List);
        pubLog_Debug(_FFL_, service_name, "", "newfuncList      =[%s]",      newfuncList);
        pubLog_Debug(_FFL_, service_name, "", "newfuncStr      =[%s]",      newfuncStr);
    #endif
    
    
    
    /*ret=-1;
   ** ret=pubOpenAcctSendCmd(pId_No,           pPhone_No,       vAttr_Code,
   **                        vSm_Code,         pBelong_Code,    pLogin_No,
   **                        vLogin_Accept_Char,    pOp_Note,        pSystem_Note,
   **                        pOp_Code,         vTotal_Date,     vSysDate,
   **                        vs_Offon_accept);
   ** if(ret != 0){
   **     pubLog_Debug(_FFL_, service_name, "", "���Ϳ��ػ��������!");
   **     strcpy(oRetCode,"110411");
   **     strcpy(oRetMsg, "���Ϳ��ػ��������!");
   **     PUBLOG_DBDEBUG(service_name);
   **     pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
   **     RAISE_ERROR(oRetCode,oRetMsg);
   ** }*/
    
    /* ��ѯ  IMSI���� */
    EXEC SQL select substr(switch_no,1,15) into :sV_ImsiNo
             from   dCustSim
             where  id_no = to_number(:pId_No);
    if (SQLCODE != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "��ȡIMSI_No����!");
        strcpy(oRetCode,"110410");
        strcpy(oRetMsg, "��ȡIMSI_NO����!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    
    Trim(sV_ImsiNo);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_ImsiNo      =[%s]",      sV_ImsiNo);
    #endif
    
    /* ���ͻ���Ϣ������ֵ */
    Trim(vTotal_Date);
    Trim(vSysDate);
    Trim(pOp_Code);
    Trim(pOp_Code);
    Trim(pLogin_No);
    Trim(vOrg_Code);
    Trim(vLogin_Accept_Char);
    Trim(pId_No);
    Trim(pPhone_No);
    Trim(vSm_Code);
    Trim(pSim_No);
    Trim(pBelong_Code);
    Trim(sV_ImsiNo);
    Trim(sV_hlrCode);
    funcEnv.transIN=transIN;
    funcEnv.transOUT=transOUT; 
    funcEnv.temp_buf=temp_buf;
    funcEnv.totalDate=vTotal_Date;
    funcEnv.opTime=vSysDate;
    funcEnv.opCode=pOp_Code;
    funcEnv.loginNo=pLogin_No;
    funcEnv.orgCode=vOrg_Code;
    funcEnv.loginAccept=vLogin_Accept_Char;
    funcEnv.idNo=pId_No;
    funcEnv.phoneNo=pPhone_No;
    funcEnv.smCode=vSm_Code;
    funcEnv.cmdRight="1";
    funcEnv.simNo=pSim_No;
    funcEnv.belongCode=pBelong_Code;
    funcEnv.offonAccept = ""; /* not use  */
    funcEnv.imsiNo=sV_ImsiNo;
    funcEnv.hlrCode=sV_hlrCode;
/*
    if (strcmp(pOp_Code,"1104")==0) 
    {         
*/
	ret =-1;
	ret =NewpubOpenAcctSendCmd(&funcEnv,'1',pOp_Code,newfuncList);
	if(ret != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "�û��������ػ��������!");
        strcpy(oRetCode,"110411");
        pubLog_Debug(_FFL_, service_name, "", " ret = [%d] ",ret);
        strcpy(oRetMsg, "�û��������ػ��������!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
	
	ret =-1;
	ret =NewfAddUsedFuncs(&funcEnv,newfuncStr,vPrepay_Fee,&V_LimitFee,sv_funccode);
	if(ret != 0)
    {
        if(ret==121958)
		{
			sprintf(oRetMsg, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sv_funccode,V_LimitFee);
		}
		else
		{
			strcpy(oRetMsg, "����������Ч���ط�����!");
		}
        
        pubLog_Debug(_FFL_, service_name, "", "����������Ч���ط�����!");
        strcpy(oRetCode,"110411");
        pubLog_Debug(_FFL_, service_name, "", " ret = [%d] ",ret);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
	
    	sprintf(sV_Cmd_right, "%d",vCmd_Right);
    	
        ret = pubOpenFunc(transIN , transOUT , temp_buf ,vTotal_Date ,
            vSysDate , pOp_Code , pLogin_No , vOrg_Code ,
            vLogin_Accept_Char , pId_No , pPhone_No , vSm_Code ,
            pBelong_Code , vs_Offon_accept ,
            pUsedFunc_List , pUnUsedFunc_List , pAddAddFunc_List ,
            sV_Cmd_right , pSim_No ,vPrepay_Fee,
            oRetMsg);
        if(ret != 0)
        {
             pubLog_Debug(_FFL_, service_name, "", "�����û������ط���Ϣ����!");
            strcpy(oRetCode,"110410");
            pubLog_Debug(_FFL_, service_name, "", " ret = [%d] ",ret);
            
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }  
/*
    }
*/

    
    /*159�������� begine*/
    /*pLimitFeeMax=0;
    **EXEC SQL  select abs(b.favoure1)
    **			into :pLimitFeeMax 
	**				from sBillModeDetail a,sBillTotCode b,dGoodPhoneRes c
	**				where a.region_code=substr(:pBelong_Code,1,2)
	**				and a.mode_code=c.BILL_CODE
	**				and c.phone_no=:pPhone_No
	**				and substr(c.phone_no,1,3)='159'
	**				and a.region_code=b.region_code and a.detail_type='2'
	**				and a.detail_code=b.total_code 
	**				and c.good_type in ('0008','0009','0010','0011','0012','0013','0014','0015');
	**
    **if(SQLCODE!=0 && SQLCODE!=1403)
    **{
    **	pubLog_Debug(_FFL_, service_name, "", "��ѯ159��������ʧ��,SQLCODE=[%d]!" , SQLCODE);
    **    strcpy(oRetCode,"110487");
    **    strcpy(oRetMsg, "��ѯ159��������ʧ��");
    **    PUBLOG_DBDEBUG(service_name);
    **    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    **    RAISE_ERROR(oRetCode,oRetMsg);
    **}
    **if(SQLCODE==0)
    **{
    **	pubLog_Debug(_FFL_, service_name, "", "pLimitFeeMax=[%d] vPrepay_Fee[%f]" , pLimitFeeMax,vPrepay_Fee);
    **	if(pLimitFeeMax>0)
    **	{
    **		if((pLimitFeeMax*6)>vPrepay_Fee)
    **		{
    **			pubLog_Debug(_FFL_, service_name, "", "�û���Ԥ��������������Ѱ����ܶ�,pLimitFeeMax=[%d] vPrepay_Fee[%.2f]" , pLimitFeeMax,vPrepay_Fee);
    **	    	strcpy(oRetCode,"110486");
    **	    	sprintf(oRetMsg, "�û�[%s]��Ԥ�������ڰ�����������ܶ�,�����²���!",pPhone_No);
    **	    	PUBLOG_DBDEBUG(service_name);
    **	    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    **	    	RAISE_ERROR(oRetCode,oRetMsg);
    **		}
    **	}
    }*/
    /*159�������� end */
    /*    */
    pubLog_Debug(_FFL_, service_name, "", "vBillModeNum=[%d]",vBillModeNum);
    for(i=0;i<vBillModeNum;i++){
    	ret = -1;
    	if(strlen(vBillMode[i])==0)
    	    continue;
    	pubLog_Debug(_FFL_, service_name, "", "pubBillBeginMode�����ʷ�=[%s][%d]",vBillMode[i],i);
		if (i==0)
		{
			memset(vModeCode1, '\0', sizeof(vModeCode1));
			strcpy(vModeCode1,vBillMode[i]);

			sprintf(TableName,"dinnetmodecode%ld",atol(pId_No)%10);
			sprintf(dynStmt,"insert into %s (ID_NO,MODE_CODE,TOTAL_DATE) values(TO_NUMBER(:vi),:v2,TO_NUMBER(:v3))",TableName);
			EXEC SQL PREPARE sql_stmt FROM :dynStmt;
			EXEC SQL EXECUTE sql_stmt using :pId_No,:vModeCode1,:vTotal_Date;
			if (SQLCODE != 0)
			{
				strcpy(oRetCode,"110411");
				sprintf(oRetMsg,"�����[%s] ʧ��",TableName);
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);				
				EXEC SQL ROLLBACK;
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			      /**insert dAllUser*/  
			      /*������ edit by cencm at 20100414--------begin-------------*/
			/*	**exec sql insert into dAllUser(
			    **   msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
			    **   start_time,end_time,region_code,id_no,group_id,product_code)
			   	**values(
			    **   :pPhone_No,:vSm_Code,'user','user',0,'1',sysdate,
			    **   to_date('20500101 00:00:00','YYYYMMDD HH24:MI:SS'),substr(:pBelong_Code,1,2),
			    **   	:pId_No,'null',:vModeCode1);
			  	**			if(SQLCODE!=OK)
				**			{
				**				pubLog_Debug(_FFL_, service_name, "", "����dAllUser���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
				**				EXEC SQL ROLLBACK;
				**				PUBLOG_DBDEBUG(service_name);
				**				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
				**				RAISE_ERROR(oRetCode,oRetMsg);
				**			}  */
						/*������ edit by cencm at 20100414--------end-------------*/	

		}
    	if(strncmp(vBillMode[i],"gn24",4)==0  || strncmp(vBillMode[i],"gn1m",4)==0)
    	{
    		/*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H begin*/
    		if(strcmp(vaccflag,"Y")==0)
			{
				pubLog_Debug(_FFL_, service_name, "", "��ǰʱ��DD[%s]�ɹ���",vnowdate);
				
				if(strcmp(vcycleDay,vnowdate)==0)
				{
					ret = pubBillBeginMode(pPhone_No,    vBillMode[i],    vSysDate,
							pOp_Code,     pLogin_No,       vLogin_Accept_Char,
							vSysDate,     "0",     "", vBunchNo,
							vErrMsg);
				}
				else if(strcmp(vcycleDay,vnowdate)<0) /*����������29---31�ż�� ������� �¸�������Ч*/
				{
					/*ȡ�û��¸����ڿ�ʼʱ��*/
					/*�����ڸ���   ��ȡ��һ���� ��ʼ������ʱ�� S.H  2012/6/11 15:54:52 begin */
					vRetout=0;
					vRetout=pubGetCustAcctCyCle(pId_No,vSysDate,1,0,vCustAcctCycleBeginTime,vCustAcctCycleEndTime,vErrMsg );
					if(vRetout != 0)
					{
						strcpy(oRetCode,"110473");
						sprintf(oRetMsg, "��ȡ����ʱ��ʧ��,vRetout=[%d] vErrMsg=[%s]\n",vRetout,vErrMsg);
						PUBLOG_DBDEBUG(service_name);
						pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
						RAISE_ERROR(oRetCode,oRetMsg);
						
					}
					pubLog_Debug(_FFL_, service_name, "", "��һ�����ڿ�ʼʱ��[%s]�ɹ���",vCustAcctCycleBeginTime);
					pubLog_Debug(_FFL_, service_name, "", "��һ�����ڽ���ʱ��[%s]�ɹ���",vCustAcctCycleEndTime);
					
					ret = pubBillBeginMode(pPhone_No,    vBillMode[i],    vCustAcctCycleBeginTime,
						pOp_Code,     pLogin_No,       vLogin_Accept_Char,
						vSysDate,     "1",     "", vBunchNo,
						vErrMsg);
				}
    		}
    		else    /*������ ������*/
    		{
	    	    ret = pubBillBeginMode(pPhone_No,    vBillMode[i],    iEffectTime,
	    	                           pOp_Code,     pLogin_No,       vLogin_Accept_Char,
	    	                           vSysDate,     "1",     "", vBunchNo,
	    	                           vErrMsg);
    	     }
    	     /*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H end*/
    	}
    	else
    	{
    	    ret = pubBillBeginMode(pPhone_No,    vBillMode[i],    vSysDate,
    	                           pOp_Code,     pLogin_No,       vLogin_Accept_Char,
    	                           vSysDate,     "0",     "", vBunchNo,
    	                           vErrMsg);
    	}
        if(ret!=0){
            EXEC SQL ROLLBACK;
            sprintf(oRetCode,"%d",ret);            
            sprintf(oRetMsg,"�����ʷ�[%s]����%s",vBillMode[i],vErrMsg);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }

	/* ����û��ʷ���Ϣ�Ƿ����� */	
	ret = pubCheckBillMsg(pId_No);
    if(ret != 0)
    {
    	pubLog_Debug(_FFL_, service_name, "", "�û��ʷ���Ϣû������,ret=[%d]!" , ret);
        strcpy(oRetCode,"110410");
        sprintf(oRetMsg, "�û�[%s]�ʷ���Ϣû������,�����²���!",pPhone_No);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
      /*by zd ����188�źŵĲ������� 20110722 188��ͨ���������������������ʵ�����ַ�ʽ��1��ʵ����������2����ʵ������ begin*/
      vCount1=0;
       vCount=0;
        /*by zd �ж��Ƿ�Ϊ�Խ���Ӫ*/
    EXEC SQL select count(1)
    			into :vCount1
    			from dchngroupmsg 
    			where group_id = :vOrgId
    			and class_code ='1068';
  if(SQLCODE!=0)
    {
    	strcpy(oRetCode,"110486");
        sprintf(oRetMsg, "��ѯ[%s]�Ƿ��԰�Ӫҵ��ʧ��[%d]",pPhone_No,SQLCODE);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    pubLog_Debug(_FFL_, service_name, "", "GGGGGGGGGGGGGG[%s][%d]",pPhone_No,vCount1);    			
	if ( vCount1 >0)  			
	{
	   EXEC SQL select count(1)
    			into :vCount
    			from dGoodPhoneRes
    			where phone_no = :pPhone_No;
	}
	else 
	{
	    EXEC SQL select count(1)
    			into :vCount
    			from dGoodPhoneRes
    			where phone_no = :pPhone_No
    			and good_type !='0020';
	}
	 /*by zd ����188�źŵĲ������� 20110722 188��ͨ���������������������ʵ�����ַ�ʽ��1��ʵ����������2����ʵ������ end*/
    if(SQLCODE!=0)
    {
    	strcpy(oRetCode,"110487");
        sprintf(oRetMsg, "��ѯ[%s]��������ʧ��[%d]",pPhone_No,SQLCODE);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
    pubLog_Debug(_FFL_, service_name, "", "GGGGGGGGGGGGGG[%s][%d]",pPhone_No,vCount);
    if(vCount>0)
    {
    	EXEC SQL select cust_name,Id_Iccid,nvl(true_flag,'N')
    			into :vGCustName,:vGIdIccid,:vGtrueFlag
    			from dcustdoc 
    			where cust_id=to_number(:pCust_Id);
    	if(SQLCODE!=0)
    	{
    		strcpy(oRetCode,"110487");
        	sprintf(oRetMsg, "��ѯ[%s]dcustdocʧ��[%d]",pCust_Id,SQLCODE);
        	PUBLOG_DBDEBUG(service_name);
        	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	Trim(vGCustName);
    	Trim(vGIdIccid);
    	pubLog_Debug(_FFL_, service_name, "", "GGGGGGGGGGGGGG[%s][%s]",vGCustName,vGIdIccid);
    	if(strcmp(vGCustName,"Ԥ����")==0 || strcmp(vGIdIccid,"111111111111111111")==0 || strcmp(vGtrueFlag,"Y")!=0)
    	{
    		strcpy(oRetCode,"110487");
        	sprintf(oRetMsg, "����������Ҫ��ʵ��д�ͻ�����[%s]��֤������[%s],�û����ϱ������Ѻ�ʵ",vGCustName,vGIdIccid);
        	PUBLOG_DBDEBUG(service_name);
        	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    }
   
    /*�鿴�Ƿ���ԤԼ����������  20090222add*/
  	vCount=0;
  	EXEC SQL SELECT count(*)
  			   into :vCount
	           from dgoodphonereservations
	          where phone_no=:pPhone_No
	            and reservation_flag='0';
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"110489");
    	sprintf(oRetMsg, "��ѯdgoodphonereservationsʧ��[%d]",SQLCODE);
    	PUBLOG_DBDEBUG(service_name);
    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    	RAISE_ERROR(oRetCode,oRetMsg);
	}
	if(vCount>0)
	{
	/* by zd 20090708 ���Ӷ�ԤԼ��Ϣ�ж� begin */
				EXEC SQL select id_iccid
						into :comm_iccid
						from dgoodphonereservations
						where phone_no=:pPhone_No
	          and reservation_flag='0';
	  if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110493");
  	  sprintf(oRetMsg, "��ѯdgoodphonereservationsʧ��[%d]",SQLCODE);
  	  RAISE_ERROR(oRetCode,oRetMsg);
		}
		
		Trim(comm_iccid);
		Trim(sV_Iccid);
		if(strlen(sV_Iccid)==0)strcpy(sV_Iccid,sV_Id_Iccid);
		if(strcmp(comm_iccid,sV_Iccid)!=0)
		{
			pubLog_Debug(_FFL_, service_name, "", "---comm_iccid%s sV_Iccid%s",comm_iccid,sV_Iccid);
			strcpy(oRetCode,"110494");
  	  		sprintf(oRetMsg, "�˺���ΪԤԼ���룬�����ͻ�������ԤԼ��Ϣ��һ��");
  	  		RAISE_ERROR(oRetCode,oRetMsg);
		}
		if(strcmp(sTrue_flag,"Y")!=0)
		{
			
			strcpy(oRetCode,"110494");
		  	  sprintf(oRetMsg, "�˺���ΪԤԼ����,����ʵ������");
		  	  RAISE_ERROR(oRetCode,oRetMsg);
		}
/* by zd 20090708 ���Ӷ�ԤԼ��Ϣ�ж� end */		
		
		EXEC SQL insert into dgoodphonereservationshis
					 (PHONE_NO,ID_TYPE,ID_ICCID,CUST_NAME,SUCCESS_FLAG,
					  RESERVATION_TIME,OP_TIME,RESERVATION_FLAG,UPDATE_ACCEPT,  UPDATE_TIME,  UPDATE_DATE,  UPDATE_LOGIN, 
					  UPDATE_TYPE,  UPDATE_CODE )
			  select PHONE_NO,ID_TYPE,ID_ICCID,CUST_NAME,SUCCESS_FLAG,RESERVATION_TIME,OP_TIME,RESERVATION_FLAG,
              		 to_number(:vLogin_Accept_Char),:vSysDate,:vTotal_Date,:pLogin_No,'U',:pOp_Code
              		 from dgoodphonereservations
              WHERE  phone_no=:pPhone_No
              	and  reservation_flag='0';
	    if(SQLCODE!=0)
	    {
	      	strcpy(oRetCode,"110489");
		    sprintf(oRetMsg, "����dgoodphonereservationshisʧ��[%d]",SQLCODE);
		   	PUBLOG_DBDEBUG(service_name);
		   	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		   	RAISE_ERROR(oRetCode,oRetMsg);
		}
	
		EXEC SQL UPDATE dgoodphonereservations
		           set  success_flag ='1', reservation_flag ='1',op_time=to_date(:vSysDate,'yyyymmdd hh24:mi:ss')
		          where phone_no=:pPhone_No
		           and  reservation_flag='0';
	    if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110490");
	    	sprintf(oRetMsg, "����dgoodphonereservationsʧ��[%d]",SQLCODE);
	    	PUBLOG_DBDEBUG(service_name);
	    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	    	RAISE_ERROR(oRetCode,oRetMsg);
		}  
				
	}
	/*�鿴�Ƿ���ԤԼ����������  20090222edd*/
	/*�鿴�Ƿ���ԤԼ����ͨ����  20090222add*/
  	vCount=0;
  	EXEC SQL SELECT count(*)
  			   into :vCount
	           from dphonereservemsg
	          where phone_no=:pPhone_No
	            and reservation_flag='0';
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"110490");
    sprintf(oRetMsg, "��ѯdphonereservemsgʧ��[%d]",SQLCODE);
    PUBLOG_DBDEBUG(service_name);
    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    RAISE_ERROR(oRetCode,oRetMsg);
	}
	if(vCount>0)
	{
		EXEC SQL select distinct c.chntype 
		into :rechntype 
		from dloginmsg a,dchngroupmsg b,schnclass c
		where a.login_no=:pLogin_No and a.org_id = b.group_id /*a.group_id=b.group_id edit by liuweib20091023*/
		and b.class_code=c.class_code;
		if(SQLCODE!=0)
    {
    	strcpy(oRetCode,"110499");
      strcpy(oRetMsg,"��ѯ���Ź�����������ʧ��!");	
      pubLog_Debug(_FFL_, service_name, "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
      PUBLOG_DBDEBUG(service_name);
      pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
      RAISE_ERROR(oRetCode,oRetMsg);
    }
    if(strcmp(rechntype,"0")!=0)
    {
    	strcpy(oRetCode,"110498");
    	strcpy(oRetMsg,"ԤԼ����ֻ������Ӫ������");	
      pubLog_Debug(_FFL_, service_name, "", "ԤԼ����ֻ������Ӫ������");
      PUBLOG_DBDEBUG(service_name);
      pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
      RAISE_ERROR(oRetCode,oRetMsg);
    }
		EXEC SQL select id_iccid
						into :comm_iccid
						from dphonereservemsg
						where phone_no=:pPhone_No
	          and reservation_flag='0';
	  if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110493");
  	  sprintf(oRetMsg, "��ѯdphonereservemsgʧ��[%d]",SQLCODE);
  	  PUBLOG_DBDEBUG(service_name);
  	  pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
  	  RAISE_ERROR(oRetCode,oRetMsg);
		}
		
		Trim(comm_iccid);
		Trim(sV_Iccid);
		if (strlen(sV_Iccid)==0) strcpy(sV_Iccid,sV_Id_Iccid);
		if(strcmp(comm_iccid,sV_Iccid)!=0)
		{
			pubLog_Debug(_FFL_, service_name, "", "---comm_iccid%s sV_Iccid%s",comm_iccid,sV_Iccid);
			strcpy(oRetCode,"110494");
  	  sprintf(oRetMsg, "�˺���ΪԤԼ���룬�����ͻ�������ԤԼ��Ϣ��һ��");
  	  PUBLOG_DBDEBUG(service_name);
  	  pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
  	  RAISE_ERROR(oRetCode,oRetMsg);
		}
		if(strcmp(sTrue_flag,"Y")!=0)
		{
			pubLog_Debug(_FFL_, service_name, "", "---comm_iccid%s sV_Iccid%s",comm_iccid,sV_Iccid);
			strcpy(oRetCode,"110494");
  	  sprintf(oRetMsg, "�˺���ΪԤԼ����,����ʵ������");
  	  PUBLOG_DBDEBUG(service_name);
  	  pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
  	  RAISE_ERROR(oRetCode,oRetMsg);
		}
		EXEC SQL insert into dphonereservemsghis
						(phone_no,id_type,id_iccid,cust_name,reservation_time,op_time,reservation_flag,update_accept,
						update_time,update_date,update_login,update_type,update_code,login_no,contract_phone,group_id,query_chn)
				 		select  phone_no,id_type,id_iccid,cust_name,reservation_time,op_time,reservation_flag,
				 		to_number(:vLogin_Accept_Char),:vSysDate,:vTotal_Date,:pLogin_No,'u',:pOp_Code,login_no,
				 		contract_phone,group_id,query_chn from   dphonereservemsg
            WHERE  phone_no=:pPhone_No 	and  reservation_flag='0';
	  if(SQLCODE!=0)
	  {
	    strcpy(oRetCode,"110491");
		  sprintf(oRetMsg, "����dphonereservemsghisʧ��[%d]",SQLCODE);
		 	PUBLOG_DBDEBUG(service_name);
		 	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		 	RAISE_ERROR(oRetCode,oRetMsg);
		}
	
		EXEC SQL UPDATE dphonereservemsg
		           set  reservation_flag ='1',op_time=to_date(:vSysDate,'yyyymmdd hh24:mi:ss')
		          where phone_no=:pPhone_No
		           and  reservation_flag='0';
	  if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110492");
	    	sprintf(oRetMsg, "����dphonereservemsgʧ��[%d]",SQLCODE);
	    	PUBLOG_DBDEBUG(service_name);
	    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	    	RAISE_ERROR(oRetCode,oRetMsg);
		}				
	}
	/*�鿴�Ƿ�����վԤԼ���ͻ����ŵĺ���  20130121add*/
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from DWEBDELIVERY
				where order_phoneno=:pPhone_No
				and delivery_type='0'
				and order_status in ('010','020');
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"110489");
		sprintf(oRetMsg, "��ѯDWEBDELIVERYʧ��[%d]",SQLCODE);
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	if(vCount>0)
	{
		EXEC SQL select nvl(identity_card,' '),nvl(order_id,' '),nvl(order_status,' ')
					into :comm_iccid,:vOrderId,:vOrderStatus
					from DWEBDELIVERY
					where order_phoneno=:pPhone_No
					and delivery_type='0'
					and order_status in ('010','020');
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110489");
			sprintf(oRetMsg, "��ѯDWEBDELIVERYʧ��[%d]",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		Trim(comm_iccid);
		Trim(vOrderId);
		Trim(vOrderStatus);
		Trim(sV_Iccid);
		
		if(strlen(sV_Iccid)==0)strcpy(sV_Iccid,sV_Id_Iccid);
		if(strcmp(comm_iccid,sV_Iccid)!=0)
		{
			pubLog_Debug(_FFL_, service_name, "", "---comm_iccid%s sV_Iccid%s",comm_iccid,sV_Iccid);
			strcpy(oRetCode,"110494");
			sprintf(oRetMsg,"�˺���ΪԤԼ���͵ĺ��룬�����ͻ�������ԤԼ��Ϣ��һ��");
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		if(strcmp(sTrue_flag,"Y")!=0)
		{
			strcpy(oRetCode,"110494");
			sprintf(oRetMsg,"�˺���ΪԤԼ����,����ʵ������");
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		if(strcmp(vOrderStatus,"010")==0)
		{
			strcpy(oRetCode,"110489");
			strcpy(oRetMsg,"�˺���ΪԤԼ���룬����δ����״̬����������");
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		EXEC SQL insert into DWEBDELIVERYHIS
			(delivery_type, order_time, order_channel, cust_name, 
			identity_card,adress, delivery_time, invoice, 
			order_phoneno, mode_code, reserve, buy_phone, model, 
			price, bind_cycle, lowest_consume, order_id, org_id, 
			total_date, op_time, phoneno, choosemodecode, 
			grant_accept, order_status,contact_orgid,phone_type,
			Update_Accept,  Update_Time,Update_Login,Update_Type,
			Update_Code,Update_Date,update_note)
		select delivery_type, order_time, order_channel, cust_name, 
			identity_card,adress, delivery_time, invoice, 
			order_phoneno, mode_code, reserve, buy_phone, model, 
			price, bind_cycle, lowest_consume, order_id, org_id, 
			total_date, op_time, phoneno, choosemodecode, 
			grant_accept, order_status,contact_orgid,phone_type,
			to_number(:vLogin_Accept_Char),to_date(:vSysDate,'yyyymmdd hh24:mi:ss'),:pLogin_No,'U',
			:pOp_Code,:vTotal_Date,'��������'
		from DWEBDELIVERY
		where order_phoneno=:pPhone_No
		and delivery_type='0'
		and order_status in ('020');
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110491");
			sprintf(oRetMsg, "��¼DWEBDELIVERYHISʧ��[%d]",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		EXEC SQL UPDATE DWEBDELIVERY
				set order_status='030',
					op_time=to_date(:vSysDate,'yyyymmdd hh24:mi:ss')
				where order_phoneno=:pPhone_No
				and delivery_type='0'
				and order_status in ('020');
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"110492");
			sprintf(oRetMsg, "����DWEBDELIVERYʧ��[%d]",SQLCODE);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		
		/* ����ҵ�񹤵�bodb_0196Cfm���۳������˺Ž��*/
		memset(vParamList, 0, MAX_PARMS_NUM*2*DLINTERFACEDATA);
		memset(vCallServiceName, '\0', sizeof(vCallServiceName));
		strcpy(vCallServiceName, "bodb_0196Cfm");
		vSendParaNum=3;
		vRecpParaNum=2;
	
		strcpy(vParamList[0][0], "order_id");
		strcpy(vParamList[0][1], vOrderId);
		
		strcpy(vParamList[1][0], "login_no");
		strcpy(vParamList[1][1], pLogin_No);
		
		strcpy(vParamList[2][0], "op_code");
		strcpy(vParamList[2][1], pOp_Code);
		
		v_ret = 0;
		v_ret = SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
								ORDERIDTYPE_USER, pId_No, ORDER_RIGHT, vLogin_Accept_Char,pOp_Code,pLogin_No,pOp_Note);			
		if(0 != v_ret) 
		{
			strcpy(oRetCode,"110493");
			sprintf(oRetMsg,"����ҵ�񹤵��۳������˻�Ǯ����v_ret=[%d]!",v_ret);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
	}
	/*�鿴�Ƿ�����վԤԼ���ͻ����ŵĺ���  20130121end*/
	
	/*���ӿ�ͨ��ʱ�����ײ�  20110518  begin*/
	Trim(iAddLac);
	if(strlen(iAddLac)>1)
	{
		PUBLOG_DEBUG(service_name, "","����С���Ż�[%s]",iAddLac);
		init(vModeCode);
		for(i=0;i<strlen(iAddLac);i++)
		{
			if(strncmp(&iAddLac[i],"~",1)!=0)
			{
				strncat(vModeCode,&iAddLac[i],1);
			}
			else
			{
				PUBLOG_DEBUG(service_name, "","����vModeCode=[%s]",vModeCode);
				ret=pubBillBeginMode(pPhone_No,vModeCode,vSysDate,pOp_Code, \
							pLogin_No,vLogin_Accept_Char,vSysDate,"1","","",oRetMsg);
				if(ret>0)
				{
					strcpy(oRetCode,"110497");
					strcpy(oRetMsg,"��ͨ��ʱ�����Ż��ײ�ʧ��!");
					RAISE_ERROR(oRetCode,oRetMsg);
				}
				init(vModeCode);
			}
		}
		
		EXEC SQL declare curAddLace CURSOR FOR
			 select main_code,main_name
			 from sMainCode where main_code='24';
		EXEC SQL OPEN curAddLace;
		while (SQLCODE==0)
		{
			init(sMainCode);
			init(sMainNote);
			EXEC SQL FETCH curAddLace into :sMainCode, :sMainNote;		
	        if((SQLCODE!=0) && (SQLCODE != 1403))
	        {
	            strcpy(oRetCode,"110496");
	            strcpy(oRetMsg,"��ѯҵ�����Դ�������!");
	            PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
	            EXEC SQL CLOSE curAddLace;
	            RAISE_ERROR(oRetCode,oRetMsg);
	        }
	        if (SQLCODE == 1403) break;
			strcpy(sTdCustType.sIdNo,pId_No);
			strcpy(sTdCustType.sMainCode,sMainCode);
			strcpy(sTdCustType.sMainNote,sMainNote);     
	        ret=OrderInsertCustType(ORDERIDTYPE_USER,pId_No,ORDER_RIGHT,
							pOp_Code,atol(vLogin_Accept_Char),pLogin_No,pOp_Note,
							sTdCustType);
		    if (ret !=0)
		    {
		    	PUBLOG_DEBUG(service_name,"","ret=[%d]",ret);
	            strcpy(oRetCode,"110495");
	            strcpy(oRetMsg,"�����û����Ա����!");
	            PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
		    	EXEC SQL CLOSE curAddLace;
	            RAISE_ERROR(oRetCode,oRetMsg);
		    }
		}
		EXEC SQL CLOSE curAddLace;
	}
	/*���ӿ�ͨ��ʱ�����ײ�  20110518  end*/
	/*���ӵ��Ӵ��Ż�   add by  xubp   2013-05-20   begin*/
	Trim(vElecSeries);
	Trim(vFavCode);
	Trim(vPrepayFee);
	Trim(vMonthFav);
	Trim(pPhone_No);
	Trim(pCon_Id);
	if(strlen(vElecSeries)>1)
	{
		/*�жϵ��Ӵ��Ƿ�ռ��*/
		EXEC SQL SELECT count(*) into :vCount2
							from dCustPresent
							where bunch_no=:vElecSeries;
		if(SQLCODE!=0 && SQLCODE!=1403)
		{
			strcpy(oRetCode,"111455");
			sprintf(oRetMsg, "��ѯ���Ӵ������ϴ���[%d]",SQLCODE);
			PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		if( vCount2 != 0)
		{
			strcpy(oRetCode,"111456");
			sprintf(oRetMsg,"���ֻ������Ѿ���ʹ��[%s]",vElecSeries);
			PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		/*��ȡ����Ԥ��*/
		Trim(vRegion_Code);
		Trim(vSm_Code);
		Trim(vInnet_Type);
		EXEC SQL SELECT to_char(nvl(INNET_PREPAY,0)) 
								into :vInnetPrepay
							from sinnetcode
							where region_code=:vRegion_Code
								and sm_code=:vSm_Code
								and inland_toll=:vInnet_Type;
		if(SQLCODE!=0)
		{
			strcpy(oRetCode,"111457");
			strcpy(oRetMsg,"��ѯ����Ԥ������!");
			PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		Trim(vInnetPrepay);
		if((atof(vPrepayFee)+atof(vMonthFav)+vPrepay_Fee-atof(vInnetPrepay)) >= 0)
		{
			EXEC SQL COMMIT;
			/*����s1308Cfm����������ֻ������Ѳ���*/
			EXEC SQL SELECT to_char(fav_money),to_char(month_num),to_char(least_fee),
											to_char(SYSDATE,'yyyymmdd'),to_char(ADD_MONTHS(SYSDATE,month_num),'yyyymmdd')
								into :vFavMoney,:vMonthNum,:vLeastFee,:vBeginTime,:vEndTime
								from spresentcfg
								where region_code=:vRegion_Code
									and fav_code=:vFavCode
									and (begin_money <= :vPrepayFee and end_money > :vPrepayFee);
			if(SQLCODE!=0)
			{
				strcpy(oRetCode,"111458");
				sprintf(oRetMsg, "�����ѳɹ����Ż����ñ�spresentcfg��Ϣ������[%d]���뵽1308ҳ�����°���ҵ��",SQLCODE);
				PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vFavMoney);
			Trim(vMonthNum);
			Trim(vLeastFee);
			Trim(vBeginTime);
			Trim(vEndTime);
			/*ȡ��ˮ*/
			EXEC SQL SELECT to_char(sMaxSysAccept.nextVal)
				INTO :vLoginAccept
				from dual;
			if(SQLCODE != 0)
			{
				strcpy(oRetCode,"111459");
				sprintf(oRetMsg, "ȡϵͳ��ˮ����[%d]",SQLCODE);
				PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vLoginAccept);
			sprintf(vOpNote, "����Ա%s���û��ֻ�%s���ͻ���",pLogin_No,pPhone_No);
			Trim(vOpNote);
			printf("-------��ʼ����s1308Cfm���ֻ������ѷ������ҵ��-------\n");
			len = (FLDLEN32)(FIRST_OCCS32 * output_par_num * 100);
	
			transIN_TP=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
			if(transIN_TP == (FBFR32 *)NULL)
			{
				sprintf(temp_buf,"E200: alloc error in s1308Cfm, size(%ld)", (long)len);
				error_handler32(transIN,transOUT,"200",temp_buf,LABELDBCHANGE,CONNECT0);
			}

			transOUT_TP=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
			if(transOUT_TP == (FBFR32 *)NULL)
			{
				sprintf(temp_buf,"E200: alloc error in s1308Cfm, size(%ld)", (long)len);
				error_handler32(transIN,transOUT,"200",temp_buf,LABELDBCHANGE,CONNECT0);
			}
			Fchg32(transIN_TP,SEND_PARMS_NUM32,0,"17",(FLDLEN32)0);
			Fchg32(transIN_TP,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_0, 0,vElecSeries,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_1, 0,vFavCode,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_2, 0,pPhone_No,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_3, 0,vPrepayFee,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_4, 0,vFavMoney,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_5, 0,vMonthFav,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_6, 0,vMonthNum,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_7, 0,vBeginTime,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_8, 0,vEndTime,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_9, 0,vLeastFee,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_10, 0,pCon_Id,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_11, 0,vOpNote,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_12, 0,pLogin_No,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_13, 0,vOrg_Code,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_14, 0,"1308",(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_15, 0,pIp_Address,(FLDLEN32)0);
			Fchg32(transIN_TP,GPARM32_16, 0,vLoginAccept,(FLDLEN32)0);

			ilen = Fsizeof32(transIN_TP);
			olen = Fsizeof32(transOUT_TP);
			vRetCodeTp=0;
			vRetCodeTp = tpcall("s1308Cfm",(char *)transIN_TP,(long) &ilen,(char **)&transOUT_TP,(long *)&olen,0);
			sprintf(oRetCode,"%06d", vRetCodeTp);
			if (vRetCodeTp==-1)
			{
				strcpy(oRetCode,"111460");
				strcpy(oRetMsg, "���÷���s1308Cfmʧ�ܣ��뵽1308ҳ�����°���!");
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg,oRetCode);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			else if (vRetCodeTp!= 0)
			{
				strcpy(oRetCode,"111461");
				strcpy(oRetMsg, "���÷���s1308Cfm�����뵽1308ҳ�����°���!!");
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg,oRetCode);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Fget32(transOUT_TP,SVC_ERR_NO32,0,return_code_tp,(FLDLEN32)0);
			Fget32(transOUT_TP,SVC_ERR_MSG32,0,return_msg_tp,(FLDLEN32)0);
			if (atol(return_code_tp) != 0)
			{
				strcpy(oRetCode,return_code_tp);
				strcpy(oRetMsg,"����ҵ���Ѱ���ɹ����뵽1308���°������ֻ�������ҵ��");
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg,oRetCode);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			/*���óɹ����¼��DOPENUSERFARE*/
			EXEC SQL insert into DOPENUSERFARE(LOGIN_ACCEPT,BUNCH_NO,FAV_CODE,PHONE_NO,BEGIN_TIME,
																						END_TIME,LOGIN_NO,OP_CODE,OP_TIME)
								values(:vLoginAccept,:vElecSeries,:vFavCode,:pPhone_No,:vBeginTime,
														:vEndTime,:pLogin_No,:pOp_Code,:vSysDate);
			if(SQLCODE != 0)
			{
				strcpy(oRetCode,"111462");
				sprintf(oRetMsg, "���뿪�������ֻ������ѷ��������¼��ʧ��[%d]",SQLCODE);
				PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
		}
		else
		{
				strcpy(oRetCode,"111463");
				strcpy(oRetMsg, "�û������ܽ��С������Ԥ�棬���ܰ�����ҵ��");
				PUBLOG_DBERR(service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
		}
	}

	/*���ӵ��Ӵ��Ż�   add by  xubp   2013-05-20   end*/
	/*�鿴�Ƿ���ԤԼ����ͨ����  20090222edd*/
	
	 /* by majha add at 20100824 ������������У��*/
/*majha 20101206�շ�����֤У�飬��Ϊǰ̨�����У��
	**vCount=0;
	**EXEC SQL select  count(1)  
	**		   into  :vCount 
	**           from  dGoodPhoneJPmsg
	**          where  sale_phone_no = :pPhone_No
	**          	and  reserve_flag = 'Y';
	**if(SQLCODE!=0)
	**{
	**	strcpy(oRetCode,"110495");
	**	sprintf(oRetMsg, "��ѯdGoodPhoneJPmsg��ʧ��[%d]",SQLCODE);
	**	PUBLOG_DBDEBUG(service_name);
	**	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	**	RAISE_ERROR(oRetCode,oRetMsg);
	**}	
	**if(vCount>0)
	**{
	**	EXEC SQL SELECT id_iccid
	**			   into :vIdIccid
	**			   from dGoodPhoneJPmsg
	**			  where sale_phone_no = :pPhone_No
	**			  	and reserve_flag = 'Y';
	**	if(SQLCODE!=0)
	**	{
	**		strcpy(oRetCode,"110496");
	**		sprintf(oRetMsg, "��ѯdGoodPhoneJPmsg��ʧ��[%d]",SQLCODE);
	**		PUBLOG_DBDEBUG(service_name);
	**		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	**		RAISE_ERROR(oRetCode,oRetMsg);
	**	}
	**	Trim(vIdIccid);
	**	Trim(sV_Iccid);
	**	if (strlen(sV_Iccid)==0) strcpy(sV_Iccid,sV_Id_Iccid);
	**	if(strcmp(vIdIccid,sV_Iccid)!=0)
	**	{
	**		strcpy(oRetCode,"110496");
	**		sprintf(oRetMsg, "�˺���Ϊ����Ԥռ�������룬����֤�����뾺�����֤�Ų�һ��[%d]",SQLCODE);
	**		PUBLOG_DBDEBUG(service_name);
	**		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	**		RAISE_ERROR(oRetCode,oRetMsg);
	**	}	  	
	} */
	/* by majha add at 20100824 ������������У��*/	
	
	
    /** ----------------------ҵ�������---------------------- **/
SERVICE_FINALLY_DECLARE();
	PUBLOG_DBDEBUG(service_name);
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg, oRetCode);

    /** �����񷵻���Ϣѹ�����뻺������,ѹ��ı����Ǻ����ĵ�6������,����
     transIN��transOUT��Ϊ�˸ú���ִ�г���ʱ�ͷŻ�����ʱʹ��,��������ֶ�
     ���ӱ�ǩ���������Ͳ�Ҫд��,�������ʱ�п��ܲ�����ȷ��������ݿ������
    */
    pubLog_Debug(_FFL_, service_name, "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    
    
    if(!strcmp(oRetCode,"000000"))
        EXEC SQL commit;
    else
        exec sql rollback;
        
    
    
    pubLog_Debug(_FFL_, service_name, "", "s1104Cfm: oRetCode=[%s],oRetMsg=[%s]",
                      oRetCode,oRetMsg);
	
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	transOUT = Add_Ret_Value(GPARM32_2,vLogin_Accept_Char);
    

    time(&t_end);
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, service_name, "", "==end==s1104Cfm[%s %s]ReturnCode[%s],ReturnMsg[%s],totaltime[%ld]",getSysTime(106),getSysTime(109),oRetCode,oRetMsg,t_end-t_beg);
	#endif    

    /* ������־ */
    if (strncmp(oRetCode,"000000",4)==0)
    {
        if(redoLog32("s1104Cfm", transIN)!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "redoLog Error!");
        }
    }

	/*---��֯�����������ͳһ��־���� edit by liuhao at 19/05/2009--begin-*/
	/*MSWriteEndLog("END",vLogin_Accept_Char, oRetCode, getSysTime(112),pOp_Code);*/
	/*---��֯�����������ͳһ��־���� edit by liuhao at 19/05/2009--end-*/
	/*���񷵻أ������������ݷ��ظ��ͻ��� */
	/*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
	/*��ȡ��������ʱ��*/
	getTimeDif(&vUsedTime, vStartSec);
	
	/*Ϊ������־�ṹ�帳ֵ*/
	strcpy(logShip.vBegin_Time, vStartTime);
	logShip.vUsed_Time = vUsedTime;
	logShip.vLogion_Accept = atol(vLogin_Accept_Char);
	strcpy(logShip.vLogin_No, pLogin_No);
	strcpy(logShip.vOp_Code, pOp_Code);
	strcpy(logShip.vServ_Name, service_name);
	strcpy(logShip.vPhone_No, pPhone_No);
	logShip.vId_No = atol(pId_No);
	logShip.vOther_No = atol(pCust_Id);
	strcpy(logShip.vRetCode, oRetCode);
	strcpy(logShip.vRetMsg, oRetMsg); 
	/*����������־��־λ*/
	SetBomcLogFlag(&logShip);
	/*��¼������־*/
	WriteBomcLog(&logShip);
TUX_SERVICE_END();
}


/******************************************************
 �������ƣ�s1108Cfm
 �������ڣ�2003/10/16
 ����汾: V1.0
 ������Ա��PeiJH
 ���������������п���
 ��������� 
        ��    ��       
        ��������
 ���ز�����
        ���ش��� 
        �������� 
 �޸ļ�¼:
        �޸���Ա��
        �޸����ڣ�
        �޸�ǰ�汾�ţ�
        �޸ĺ�汾��:
        �޸�ԭ��
*****************************************************/
_TUX_SVC_ s1108Cfm(TPSVCINFO *transb){
    char        temp_buf[61];       /*��ʱʹ�õ��ַ�������*/


    char		  service_name[19 + 1];
    char		  vStartTime[20+1];			/* ����ʼʱ�� YYYYMMDDHHMISS*/
    double		vUsedTime=0.00;				/* ��������ʱ�� ��λ����*/
    double		vStartSec=0.00;				/* ����ʼʱ�� ��λ����*/
    BOMC_LOG 	logShip;					/*������־��¼�ṹ���������*/
    EXEC SQL BEGIN DECLARE SECTION;
    /* ��������������� */
    char  pLogin_No[6+1];            /*   ����          */
    char  pIp_Address[15+1];         /*   ӪҵԱ��½IP  */
    char  pOp_Code[4+1];             /*   ��������      */
    char  pSystem_Note[60+1];        /*   ϵͳ��ע      */    
    char  pOp_Note[60+1];            /*   ������ע      */
    char  pPhone_No[15+1];           /*   �ֻ�����      */
    char  pSim_Type[5+1];            /*   SIM������     */    
    char  pSim_No[20+1];             /*   SIM����       */
    char  pBelong_Code[7+1];         /*   �����������   */    
    char  pCust_Id[22+1];            /*   �ͻ�Id        */
    char  pId_No[22+1];              /*   �û�Id        */
    char  pCust_Passwd[8+1];         /*   �ͻ����� ��Ӧ���Ǽ��ܺ�ĳ��ȣ�*/ 
    char  pUser_Passwd[8+1];         /*   �û����� ��Ӧ���Ǽ��ܺ�ĳ��ȣ�*/     
    char  pCust_Name[60+1];          /*   �ͻ�����      */
    char  pCust_Address[60+1];       /*   �ͻ���ַ      */
    char  pId_Type[60+1];            /*   �ͻ�֤������  */
    char  pId_Iccid[60+1];           /*   �ͻ�֤������  */         
    char  pCheck_No[20+1];           /*   ֧Ʊ����      */
    char  pBank_Code[5+1];           /*   ���д���      */    
    char  pFee_List[ROWLEN];         /*   ������Ϣ��    */
    char  pFavour_List[ROWLEN];      /*   �Ż���Ϣ��    */   
    char  sIn_Login_Accept[22+1];    /* ������ˮ        */     
  
    /* ����ʹ�õ����������� */
    /*---- ������Ϣ ----*/
    char  vOrg_Code[9+1];            /*   ��������       */
    char  vRegion_Code[2+1];         /*   ��������       */
    char  vDistrict_Code[2+1];       /*   ���ش���       */
    char  vTown_Code[3+1];           /*   Ӫҵ������     */
    char  vTotal_Date[8+1];          /*   ��������       */
    
    /*---- dCustMsg ----*/
    char  vContract_Passwd[8+1];     /*   Ĭ���ʻ�����   */
    
    char  vSm_Code[2+1];             /*   Ʒ�ƴ���       */
    char  vService_Type[2+1];        /*   �������ʹ���   */
    char  vAttr_Code[8+1];
    float vLimit_Owe=0.00;           /*   ����״̬       */
    char  vCredit_Code[2+1];         /*                  */
    int   vCredit_Value=0;           /*                  */
    int   vBill_Type=0;              /*   ��������       */
    char  vPay_Code[1+1];
    char  vPay_Type[4+1];            /*   �������       */
    int   vCmd_Right=0;
    char  vInnet_Type[2+1];          /*   ��������       */
    char  vMachine_Code[3+1];        /*   ��������       */
    char  vOther_Attr[40+1];         /*   ��������       */

    char  vFavour_Code[ROWNUM11][4+1];     /*   �Żݴ���       */
    char  vShould_Data[ROWNUM11][17+1];    /*   �Ż�ǰ����     */
    char  vActual_Data[ROWNUM11][17+1];    /*   �Żݺ�����     */
    char  vLogin_Accept_Char[23];          /*  ������ˮ        */
    
    char  vFunction_Type[ROWNUM11][1+1];   /*   �ط�����       */
    char  vFunction_Code[ROWNUM11][2+1];   /*   �ط�����       */
    char  vFavour_Month[ROWNUM11][100];    /*   �Ż�����       */
    char  vAdd_No[ROWNUM11][15+1];         /*   ���Ӻ���       */
    char  vBegin_Time[ROWNUM11][20+1];     /*   �Żݿ�ʼʱ��   */
    char  vEnd_Time[ROWNUM11][20+1];       /*   �Żݽ���ʱ��   */

    float vCash_Pay=0;               /*   �ֽ𽻿�       */
    float vCheck_Pay=0;              /*   ֧Ʊ����       */
    float vPrepay_Fee=0;             /*   Ԥ���         */
    float vSim_Fee=0;                /*   SIM����        */
    float vMachine_Fee=0;            /*   ������         */
    float vInnet_Fee=0;              /*   ������         */
    float vChoice_Fee=0;             /*   ѡ�ŷ�         */
    float vOther_Fee=0;              /*   ������         */
    float vHand_Fee=0;               /*   ������         */
    
    float vInterest =0;              /*   Ԥ�����Ϣ     */
    float vPay_Money=0;              /*   ??             */
    char  vs_Offon_accept[22+1];     /*   ���ػ���ˮ     */

    int   tmplen=0,begin=1,end=1,num=0,i=0,k=1;
    int   tmplen1=0,begin1=1,end1=1,num1=0,i1=0,k1=1;
    int   ret=0;
    int   smap_ret=0;
    char  vTmp_List[ROWLEN],vTmp_List1[ROWLEN];
    char  vDateChar[6+1];            /*   �ֱ���ǰ��     */
    char  vSysDate[20+1];            /*   ϵͳʱ��       */
    
    char  sV_Sqlstr[2048];           /* ��ʱ�ַ���       */
    char  sV_ImsiNo[15+1];           /* imsi_no */
    char  sV_hlrCode[1+1];           /* hlr���� */
    
    char sTrue_flag[1+1];
    char sTemp[8+1];
    int  vCount=0;

	char vOrgId[10+1];
	char vPassWord[16+1];
	long	iLoginAccept=0;
	char	inResv1[5+1];
    EXEC SQL END DECLARE SECTION;
    
    tFuncEnv funcEnv;

    /*�ַ��ͱ�����ʼ��,����ʹ������ķ�ʽ,Ҳ����ʹ��
     init(������),initʽ��ͷ�ļ��ж���ĺ�,init�Ķ�
     ��: define init(a)  memset(a,0,sizeof(a))  ***/

	init(service_name);
	strcpy(service_name, "s1108Cfm");
	init(vStartTime);
	memset(&logShip, 0, sizeof(logShip));
	/*��ȡ����ʼʱ��*/
	getBeginTime(&vStartTime,&vStartSec);
    
    /** ------------------ ������ʼ����ʼ ------------------ **/

    init(pSystem_Note);
    init(temp_buf);        init(vTmp_List);        init(pIp_Address); 
    init(pPhone_No);       init(pOp_Code);         init(pOp_Note); 
    init(pSim_No);         init(pBelong_Code);     init(pCust_Passwd);    
    init(pUser_Passwd);    init(pCust_Name);       init(pCust_Address);
    init(pId_Type);        init(pId_Iccid);        init(pFee_List);        
    init(pIp_Address);     init(pCust_Id);         init(pId_No);           
    init(pFavour_List);    init(pCheck_No);        init(pBank_Code);       
    init(pSim_Type);

    init(vSm_Code);        init(vAttr_Code);       init(vService_Type);
    init(vCredit_Code);    init(vPay_Code);        init(vPay_Type);       
    init(vInnet_Type);     init(vMachine_Code);    init(vOrg_Code);       
    init(vRegion_Code);    init(vDistrict_Code);   init(vTown_Code); 
    init(vDateChar);       init(vTotal_Date);      init(vSysDate);
    init(vTmp_List);       init(vTmp_List1);       init(vs_Offon_accept);
    init(sIn_Login_Accept);init(sV_Sqlstr);        init(sV_ImsiNo);
    init(sV_hlrCode); init(sTemp); init(sTrue_flag); init(vOrgId); init(vPassWord);
/*    
    memset(funcEnv,0,size(funcEnv));
*/ 
    for(i=0;i<20;i++)
    {
        init(vFunction_Code[i]);
        init(vFunction_Type[i]);
        init(vFavour_Month[i]);
        init(vAdd_No[i]);
        init(vBegin_Time[i]);
        init(vEnd_Time[i]);
    }

    for(i=0;i<20;i++)
    {
        init(vFavour_Code[i]);
        init(vShould_Data[i]);
        init(vActual_Data[i]);
    }
	init(inResv1);
	strcpy(inResv1,"ALL");
	Trim(inResv1);
    /** ------------------ ������ʼ������ ------------------ **/
    
    TUX_SERVICE_BEGIN(iLoginAccept,pOp_Code,pLogin_No,pOp_Note,ORDERIDTYPE_USER,pId_No,inResv1,NULL);

    strcpy(sTemp,				input_parms[0]);
    memcpy(pIp_Address,          input_parms[1], 15);
    memcpy(pOp_Code,             input_parms[2], 4); 
    memcpy(pSystem_Note,         input_parms[3], 60);    
    memcpy(pOp_Note,             input_parms[4], 60);
    memcpy(pPhone_No,            input_parms[5], 15);
    memcpy(pSim_Type,            input_parms[6], 3);    
    memcpy(pSim_No,              input_parms[7], 20);
    memcpy(pBelong_Code,         input_parms[8], 7 );    
    memcpy(pCust_Id,             input_parms[9], 22);    
    memcpy(pId_No,               input_parms[10], 22);    
    memcpy(pCust_Passwd,         input_parms[11], 8);
    memcpy(pUser_Passwd,         input_parms[12], 8);    
    memcpy(pCust_Name,           input_parms[13], 60);
    memcpy(pCust_Address,        input_parms[14], 60);
    memcpy(pId_Type,             input_parms[15], 2);
    memcpy(pId_Iccid,            input_parms[16], 20);
    memcpy(pCheck_No,            input_parms[17], 20);
    memcpy(pBank_Code,           input_parms[18], 5);        
    memcpy(pFee_List,            input_parms[19], 2048);
    memcpy(pFavour_List,         input_parms[20], 2048);
    strcpy(sIn_Login_Accept,         input_parms[21]);
    
    
    strncpy(pLogin_No,sTemp,6);
    strncpy(sTrue_flag,sTemp+6,1);    
    memcpy(vPassWord,input_parms[22],16); /*�����������*/
    /*strcpy(vPassWord,"COJKPFLAJGAPLGEB")*/

    #ifdef _DEBUG_
pubLog_Debug(_FFL_, service_name, "", "---------------------- ��ӡ���ղ�����ʼ -------------------- ");
        pubLog_Debug(_FFL_, service_name, "", "ӪҵԱ����    =[%s]",    pLogin_No    );
        pubLog_Debug(_FFL_, service_name, "", "ӪҵԱ��½IP  =[%s]",    pIp_Address  );
        pubLog_Debug(_FFL_, service_name, "", "��������      =[%s]",    pOp_Code     );
        pubLog_Debug(_FFL_, service_name, "", "ϵͳ��ע      =[%s]",    pSystem_Note );        
        pubLog_Debug(_FFL_, service_name, "", "������ע      =[%s]",    pOp_Note     );
        pubLog_Debug(_FFL_, service_name, "", "�ֻ�����      =[%s]",    pPhone_No    );
        pubLog_Debug(_FFL_, service_name, "", "SIM������     =[%s]",    pSim_Type    );        
        pubLog_Debug(_FFL_, service_name, "", "SIM����       =[%s]",    pSim_No      );
        pubLog_Debug(_FFL_, service_name, "", "�����������  =[%s]",    pBelong_Code );        
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�ID        =[%s]",    pCust_Id     );
        pubLog_Debug(_FFL_, service_name, "", "�û�ID        =[%s]",    pId_No       );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�����      =[%s]",    pCust_Passwd );
        pubLog_Debug(_FFL_, service_name, "", "�û�����      =[%s]",    pUser_Passwd );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�����      =[%s]",    pCust_Name   );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ���ַ      =[%s]",    pCust_Address);
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������  =[%s]",    pId_Type     );
        pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������  =[%s]",    pId_Iccid    );        
        pubLog_Debug(_FFL_, service_name, "", "֧Ʊ����      =[%s]",    pCheck_No    );
        pubLog_Debug(_FFL_, service_name, "", "���д���      =[%s]",    pBank_Code   );                
        pubLog_Debug(_FFL_, service_name, "", "������Ϣ��    =[%s]",    pFee_List    );
        pubLog_Debug(_FFL_, service_name, "", "�Ż���Ϣ��    =[%s]",    pFavour_List );
        pubLog_Debug(_FFL_, service_name, "", "sIn_Login_Accept = [%s]",   sIn_Login_Accept );
        

pubLog_Debug(_FFL_, service_name, "", "---------------------- ��ӡ���ղ������� -------------------- ");
    #endif
    
    /*���������,����������ݿ����,���������ĺ������д���.�ô�����
      ������ʾ����,��������ǰ���������,��ע�����һ������,Ϊ0ʱ��ʾ�Ƕ�
      ���ӣ���������ǰ��Ͽ����ӣ���1ʱ��ʾ�ǳ����ӣ�ֻ�����������У�����
      �����ӡ�ͬʱ�ô����������Զ�����transIN��transOUT�Ļ��������ڴ���
      �������е�5���ֶ������ݿ�����ӱ�ǩ��*/
    
    EXEC SQL WHENEVER SQLERROR DO error_handler32(transIN,transOUT,
       "200",temp_buf,LABELDBCHANGE,CONNECT0);
 
    EXEC SQL WHENEVER SQLWARNING DO    warning_handler32();
    
    EXEC SQL WHENEVER NOT FOUND CONTINUE;
    
    

    /** ----------------------ҵ����ʼ---------------------- **/
    pubLog_Debug(_FFL_, service_name, "", "----- ������ҵ�� ----- ");
    /* ���������עΪ�գ�������ע=ϵͳ��ע */
    
    Trim(pOp_Note);
    if( strlen(pOp_Note)==0)
    {
    	strcpy(pOp_Note,pSystem_Note);
    }    
    strcpy(vSm_Code,"z0");
    strcpy(vAttr_Code,"00000Y");
    
    /* ȡ�ù�����Ϣ */
    EXEC SQL SELECT TO_CHAR(SysDate,'YYYYMMDD'),
                    TO_CHAR(SysDate,'YYYYMMDD HH24:MI:SS')
               INTO :vTotal_Date,:vSysDate
               FROM DUAL;
               
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "��ʼȡ��������");
    #endif      
	ret =0;
	ret = sGetLoginOrgid(pLogin_No,vOrgId);
	if(ret <0)
	{
		sprintf(oRetCode,"%06d",110491);
		strcpy(oRetMsg,"��ѯ����org_idʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	Trim(vOrgId);
 /*--������֯�������� liuhao    200090520 begin*/	
	sprintf (oRetCode, "%06d", spublicRight( LABELDBCHANGE, pLogin_No, vPassWord, pOp_Code ));
	if (strcmp(oRetMsg, "000000") != 0)
	{
		strcpy(oRetMsg,"����Ȩ����֤ʧ��!");
		pubLog_Debug(_FFL_, service_name, "000001", oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}                                      
/*--������֯�������� liuhao    200090520 end*/	

    sprintf(sV_Sqlstr,"select Org_code from dLoginMsg where Login_No = :pLogin_No");
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " sV_Sqlstr = [%s]",sV_Sqlstr);
    #endif

    EXEC SQL PREPARE select_stmt FROM :sV_Sqlstr; 
    EXEC SQL DECLARE cur001 CURSOR for select_stmt;	
    EXEC SQL OPEN cur001 using :pLogin_No;
    EXEC SQL FETCH cur001 INTO :vOrg_Code; 
    EXEC SQL CLOSE cur001 ; 
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vOrg_Code = [%s]",vOrg_Code);
    #endif
    
    /* ȡ�ò�����ˮ */
    Trim(sIn_Login_Accept );
    if  ((strlen(sIn_Login_Accept)==0)||(strcmp(sIn_Login_Accept,"0")==0))
    {
    EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :vLogin_Accept_Char
             FROM DUAL;
             if(SQLCODE!=0){
                 pubLog_Debug(_FFL_, service_name, "", "ȡ����������ˮ����!");
                 strcpy(oRetCode,"110802");
                 strcpy(oRetMsg, "ȡ����������ˮ����!");
                 PUBLOG_DBDEBUG(service_name);
                 pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                 RAISE_ERROR(oRetCode,oRetMsg);
             }
    
    }
    else
    {
    	strcpy(vLogin_Accept_Char ,sIn_Login_Accept);
    }
    
    Trim(vLogin_Accept_Char);
    
    iLoginAccept=atol(vLogin_Accept_Char);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "vLogin_Accept_Char  �� [%s]",vLogin_Accept_Char);
    #endif             


    /*--------------------------- ����������Ϣ�� --------------------------*/
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,pFee_List,sizeof(pFee_List)-1);
    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++){
    if(vTmp_List[i]=='~'){
            k=k+1;
            end = i;
            num = end - begin + 1;
            vTmp_List[i]='\0';
/*      if(k==1) {vCash_Pay=atof(substr(vTmp_List,begin,num));        pubLog_Debug(_FFL_, service_name, "", "a�ֽ𽻿�=[%10.2f]",vCash_Pay);}*/
      if(k==1) vCash_Pay=atof(substr(vTmp_List,begin,num));
      if(k==2) vCheck_Pay=atof(substr(vTmp_List,begin,num));
      if(k==3) vPrepay_Fee=atof(substr(vTmp_List,begin,num));
      if(k==4) vSim_Fee=atof(substr(vTmp_List,begin,num));
      if(k==5) vMachine_Fee=atof(substr(vTmp_List,begin,num));
      if(k==6) vInnet_Fee=atof(substr(vTmp_List,begin,num));
      if(k==7) vChoice_Fee=atof(substr(vTmp_List,begin,num));
      if(k==8) vOther_Fee=atof(substr(vTmp_List,begin,num));
      if(k==9) vHand_Fee=atof(substr(vTmp_List,begin,num));
            begin = end +2;
        }
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "�ֽ𽻿�=[%10.2f]",vCash_Pay);
        pubLog_Debug(_FFL_, service_name, "", "֧Ʊ����=[%10.2f]",vCheck_Pay);
        pubLog_Debug(_FFL_, service_name, "", "Ԥ���=[%10.2f]",vPrepay_Fee);
        pubLog_Debug(_FFL_, service_name, "", "SIM����=[%10.2f]",vSim_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vMachine_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vInnet_Fee);
        pubLog_Debug(_FFL_, service_name, "", "ѡ�ŷ�=[%10.2f]",vChoice_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vOther_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",vHand_Fee);
    #endif

    /*------------------------- ����������Ϣ�� --------------------------*/

    /* �����롢SIM����Ч��*/

    ret=-1;
    ret=CheckPhoneSimNo(pPhone_No,     vOrg_Code,      vSm_Code,      
                        pSim_No,       pSim_Type,     pId_Iccid, pLogin_No,oRetMsg,vOrgId);
    if(ret != 0){
        pubLog_Debug(_FFL_, service_name, "", "�����롢SIM����Ч�Դ���[%s]",oRetMsg);
        sprintf(oRetCode,"%d",ret);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }

    /* ������������������ */
/*
    smap_ret=0;
    ret=-1;
    ret=pubSMAPConfirm(pPhone_No,      pOp_Code,      vSysDate,
                      pBelong_Code,   pLogin_No);
    if(ret != 0){
        smap_ret=ret;
        pubLog_Debug(_FFL_, service_name, "", "������������ʧ��!");
        strcpy(oRetCode,"110803");
        strcpy(oRetMsg, "������ҵ��ȷ�ϴ���!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }                         
*/                       
    /* ������ҵ��ȷ�� */
    ret=-1;    
    ret=pubSZXConfirm(pCust_Id,        pId_No,        pCust_Passwd, 
                      pUser_Passwd,    pCust_Name,    pCust_Address,
                      pId_Type,        pId_Iccid,     pPhone_No,     
                      pSim_No,         pBelong_Code,  pIp_Address,   
                      vCash_Pay,       vCheck_Pay,    vSim_Fee,      
                      vMachine_Fee,    vChoice_Fee,   vHand_Fee,     
                      pLogin_No,       vLogin_Accept_Char, pOp_Note,      
                      pSystem_Note,    pOp_Code,      vTotal_Date,   
                      vSysDate,sTrue_flag);
    if(ret != 0){
        pubLog_Debug(_FFL_, service_name, "", "������ҵ��ȷ�ϴ���!");
        strcpy(oRetCode,"110804");
        strcpy(oRetMsg, "������ҵ��ȷ�ϴ���!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }                         

    /*----------------------- �����Ż���Ϣ�� --------------------------*/
pubLog_Debug(_FFL_, service_name, "", "pFavour_List=[%s]",pFavour_List);
    if(strlen(ltrim(rtrim(pFavour_List)))!=0){
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        memcpy(vTmp_List,pFavour_List,sizeof(pFavour_List)-1);
pubLog_Debug(_FFL_, service_name, "", "vTmp_List=[%s]",vTmp_List);
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
    
        for(i=0;i<=tmplen;i++){
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
                vTmp_List[i]='\0';
    
            memset(vTmp_List1,0x00,sizeof(vTmp_List1));
pubLog_Debug(_FFL_, service_name, "", "vTmp_List=[%s][%d][%d][%s]",vTmp_List,begin,num,substr(vTmp_List,begin,num));
            strcpy(vTmp_List1,rtrim(substr(vTmp_List,begin,num)));
pubLog_Debug(_FFL_, service_name, "", "vTmp_List1=[%s]",vTmp_List1);
            tmplen1=strlen(vTmp_List1);
            begin1 = 1; end1 = 1; k1=0;
    
            for(i1=0;i1<=tmplen1;i1++){
            if(vTmp_List1[i1]=='&'){
                k1=k1+1;
                end1 = i1;
                num1 = end1 - begin1 + 1;
                vTmp_List1[i1]='\0';
               if(k1==1) strcpy(vFavour_Code[k-1],substr(vTmp_List1,begin1,num1));
               if(k1==2) strcpy(vShould_Data[k-1],substr(vTmp_List1,begin1,num1));
               if(k1==3) strcpy(vActual_Data[k-1],substr(vTmp_List1,begin1,num1));
                begin1 = end1 +2;
                      }
            }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�Żݴ���[%d]       = [%s]",k,vFavour_Code[k-1]);
            pubLog_Debug(_FFL_, service_name, "", "�Ż�ǰ����[%d]     = [%s]",k,vShould_Data[k-1]);
            pubLog_Debug(_FFL_, service_name, "", "�Żݺ�����[%d]     = [%s]",k,vActual_Data[k-1]);
        #endif
                ret=-1;
                ret=pubOpenAcctFav(pId_No,           pPhone_No,           vSm_Code,         
                                   pBelong_Code,     vFavour_Code[k-1],   vShould_Data[k-1],
                                   vActual_Data[k-1],pLogin_No,           vLogin_Accept_Char,    
                                   pOp_Note,         pSystem_Note,        pOp_Code,            
                                   vTotal_Date,      vSysDate,vOrgId);
                if(ret != 0){
                    pubLog_Debug(_FFL_, service_name, "", "�����Ż���Ϣ����!");
                    strcpy(oRetCode,"110805");
                    strcpy(oRetMsg, "�����Ż���Ϣ����!");
                    PUBLOG_DBDEBUG(service_name);
                    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                    RAISE_ERROR(oRetCode,oRetMsg);
                }
                begin = end +2;
            }
        }
    }

    /* ��¼֧Ʊ��Ϣ */     
    if(vCheck_Pay>0)
    {
        ret=-1;
        ret=pubOpenAcctCheck_1(pCheck_No,   vCheck_Pay,      pBank_Code,
                           pOp_Note,      pLogin_No,       vLogin_Accept_Char, 
                           pOp_Code,      vTotal_Date,     vSysDate,
                           pSystem_Note,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�޸�֧Ʊ��Ϣ����ret = [%d]!",ret);
            strcpy(oRetCode,"110807");
            strcpy(oRetMsg, "�޸�֧Ʊ��Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }

    /* ���ػ��ӿ� */   
    ret=-1;
    
    /* ȡ���ػ���ˮ */
    EXEC SQL SELECT sOffon.nextval  INTO :vs_Offon_accept  FROM DUAL;
               
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "vs_Offon_accept     = [%s]",vs_Offon_accept);
    #endif
    
    ret=pubOpenAcctSendCmd(pId_No,           pPhone_No,       vAttr_Code,
                           vSm_Code,         pBelong_Code,    pLogin_No,
                           vLogin_Accept_Char,    pOp_Note,        pSystem_Note,
                           pOp_Code,         vTotal_Date,     vSysDate ,
                           vs_Offon_accept);
    if(ret != 0){
         pubLog_Debug(_FFL_, service_name, "", "���Ϳ��ػ��������!");
        strcpy(oRetCode,"110808");
        strcpy(oRetMsg, "���Ϳ��ػ��������!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }

pubLog_Debug(_FFL_, service_name, "", "���Ϳ��ػ��������");
    /**** Ĭ���ط���Ϣ ****/
      /* ��ѯ  IMSI���� */
    EXEC SQL select substr(trim(imsi_no),1,15) into :sV_ImsiNo
             from   dSimRes
             where  sim_no = :pSim_No;
    if (SQLCODE != 0)
    {
        strcpy(oRetCode,"110809");
        strcpy(oRetMsg, "��ѯIMSI��ʧ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
pubLog_Debug(_FFL_, service_name, "", "imsi���");
    
    Trim(sV_ImsiNo);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_ImsiNo      =[%s]",      sV_ImsiNo);
    #endif
    
    /* ��ѯ  HLR���� */
    EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
             where  phoneno_head = substr(:pPhone_No,1,7);
    if (SQLCODE != 0)
    {
        strcpy(oRetCode,"110810");
        strcpy(oRetMsg, "��ѯhlr����ʧ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
pubLog_Debug(_FFL_, service_name, "", "hlrcode���");
    
    Trim(sV_hlrCode);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_hlrCode      =[%s]",      sV_hlrCode);
    #endif
    
    /* ���ͻ���Ϣ������ֵ */
    Trim(temp_buf);
    Trim(vTotal_Date);
    Trim(vSysDate);
    Trim(pOp_Code);
    Trim(pLogin_No);
    Trim(vOrg_Code);
    Trim(vLogin_Accept_Char);
    Trim(pId_No);
    Trim(pPhone_No);
    Trim(vSm_Code);
    Trim(pSim_No);
    Trim(pBelong_Code);
    Trim(sV_ImsiNo);
    Trim(sV_hlrCode);
    funcEnv.transIN=transIN;
    funcEnv.transOUT=transOUT; 
    funcEnv.temp_buf=temp_buf;
    funcEnv.totalDate=vTotal_Date;
    funcEnv.opTime=vSysDate;
    funcEnv.opCode=pOp_Code;
    funcEnv.loginNo=pLogin_No;
    funcEnv.orgCode=vOrg_Code;
    funcEnv.loginAccept=vLogin_Accept_Char;
    funcEnv.idNo=pId_No;
    funcEnv.phoneNo=pPhone_No;
    funcEnv.smCode=vSm_Code;
    funcEnv.cmdRight="1";
    funcEnv.simNo=pSim_No;
    funcEnv.belongCode=pBelong_Code;
    funcEnv.offonAccept = ""; /* not use  */
    funcEnv.imsiNo=sV_ImsiNo;
    funcEnv.hlrCode=sV_hlrCode;

    ret=-1;
    
    ret = pubDefaultFunc(pId_No , pLogin_No ,
         pBelong_Code , vSm_Code ,
         vTotal_Date , vSysDate ,
         vLogin_Accept_Char , pOp_Code ,
         oRetMsg ,"0",
         &funcEnv);
    
    pubLog_Debug(_FFL_, service_name, "", "call pubDefaultFunc ,ret=[%d]!" , ret);
    if(ret != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "���Ĭ���ط�����,ret=[%d]!" , ret);
        strcpy(oRetCode,"110410");
        pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg);
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
     /*�鿴�Ƿ���ԤԼ���������� 20090222add*/
 /* **	vCount=0;
  	**EXEC SQL SELECT count(*)
  	**		   into :vCount
	**           from dgoodphonereservations
	**          where phone_no=:pPhone_No
	**            and reservation_flag='0';
	**if(SQLCODE!=0)
	**{
	**	strcpy(oRetCode,"110489");
    **	sprintf(oRetMsg, "��ѯdgoodphonereservationsʧ��[%d]",SQLCODE);
    **	PUBLOG_DBDEBUG(service_name);
    **	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    **	RAISE_ERROR(oRetCode,oRetMsg);
	**}
	**if(vCount>0)
	**{
	**	EXEC SQL UPDATE dgoodphonereservations
	**	           set  success_flag ='1', reservation_flag ='1'
	**	          where phone_no=:pPhone_No;
	**    if(SQLCODE!=0)
	**	{
	**		strcpy(oRetCode,"110490");
	**    	sprintf(oRetMsg, "����dgoodphonereservationsʧ��[%d]",SQLCODE);
	**    	PUBLOG_DBDEBUG(service_name);
	**    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	**    	RAISE_ERROR(oRetCode,oRetMsg);
	**	}          	
	**	EXEC SQL UPDATE dgoodphoneres
	**	           set  resource_code='1'
	**	         where  phone_no=:pPhone_No;
	**	if(SQLCODE!=0)
	**	{
	**		strcpy(oRetCode,"110490");
	**    	sprintf(oRetMsg, "����dgoodphoneresʧ��[%d]",SQLCODE);
	**    	PUBLOG_DBDEBUG(service_name);
	**    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	**    	RAISE_ERROR(oRetCode,oRetMsg);
	**	}        
	**}
   */ 
    

    /** ----------------------ҵ�������---------------------- **/


SERVICE_FINALLY_DECLARE();
	PUBLOG_DBDEBUG(service_name);
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg, oRetCode);
    if(strncmp(oRetCode,"000000",6)!=0) /* ȷ��ʧ�� */
    {
    	if(smap_ret!=0) /* �������������ӿڴ���Ҫ�ع��ӿڱ� */
    	{
            /* ���������������������� */
            ret=-1;
/*
            ret=pubSMAPConfirm(pPhone_No,      pOp_Code,      vSysDate,
                                pBelong_Code,   pLogin_No);
            if(ret != 0){
                smap_ret=ret;
                pubLog_Debug(_FFL_, service_name, "", "������������ʧ��!");
                strcpy(oRetCode,"110803");
                strcpy(oRetMsg, "������ҵ��ȷ�ϴ���!");
            }                         
*/
    	}
    	EXEC SQL ROLLBACK;
    }
    else
        /* ����ȷ�� */
        EXEC SQL COMMIT;
    
    /** �����񷵻���Ϣѹ�����뻺������,ѹ��ı����Ǻ����ĵ�6������,����
     transIN��transOUT��Ϊ�˸ú���ִ�г���ʱ�ͷŻ�����ʱʹ��,��������ֶ�
     ���ӱ�ǩ���������Ͳ�Ҫд��,�������ʱ�п��ܲ�����ȷ��������ݿ������
    */
    pubLog_Debug(_FFL_, service_name, "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    
    pubLog_Debug(_FFL_, service_name, "", "s1108Cfm: oRetCode=[%s],oRetMsg=[%s]",
                      oRetCode,oRetMsg);
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	transOUT = Add_Ret_Value(GPARM32_2,vLogin_Accept_Char);

    /* ������־ */
    if (strncmp(oRetCode,"000000",4)==0)
    {
        if(redoLog32("s1108Cfm", transIN)!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "redoLog Error!");
        }
    }


	/*��ȡ��������ʱ��*/
	getTimeDif(&vUsedTime, vStartSec);
	
	/*Ϊ������־�ṹ�帳ֵ*/
	strcpy(logShip.vBegin_Time, vStartTime);
	logShip.vUsed_Time = vUsedTime;
	logShip.vLogion_Accept = atol(vLogin_Accept_Char);
	strcpy(logShip.vLogin_No, pLogin_No);
	strcpy(logShip.vOp_Code, pOp_Code);
	strcpy(logShip.vServ_Name, service_name);
	strcpy(logShip.vPhone_No, pPhone_No);
	logShip.vId_No = atol(pId_No);
	logShip.vOther_No = atol(pId_No);
	strcpy(logShip.vRetCode, oRetCode);
	strcpy(logShip.vRetMsg, oRetMsg); 
	/*����������־��־λ*/
	SetBomcLogFlag(&logShip);
	/*��¼������־*/
	WriteBomcLog(&logShip);
TUX_SERVICE_END();
}


/***************************************
 ��������  s1114Cfm
 ��������  2003/10/09
 ������Ա  Yuanby
 ���ù��̣�
 ����������ȫ��ͨ������������
 ���������
	��������        
        ӪҵԱ��½IP    
        ��������        
        ϵͳ��ע        
        ������ע        
        SIM������       
        �����������    
        �ͻ�Id          
        �û�Id          
        �ʻ�Id          
        �ͻ�����        
        �û�����        
        �ʻ�����        
        ҵ������        
        �������ʹ���    
        ��������        
        ��������        
        ��������        
        ���ѷ�ʽ   0  �ֽ�  1  ֧Ʊ
        SIM����         
        �ܷ�����Ϣ��    
        �ֻ����봮      
        SIM�����봮     
        ѡ�ŷѴ�        
        ѡ��Ԥ�洮      
        �ײ���Ϣ��      
        �����пͻ���Ϣ��
        �ʷѴ���        
        �Ż���Ϣ��
        �ͻ�����   0 �½��ͻ�  1 ���пͻ�   
        ������ˮ    

	        
 ���������     
      �������  
      ������Ϣ  
      ������ˮ
                
                
�޸ļ�¼:       
		�޸���Ա��
                �޸����ڣ�
		�޸�ǰ�汾�ţ�
		�޸ĺ�汾��:
		�޸�ԭ��
                                  
****************************************/
_TUX_SVC_ s1114Cfm( TPSVCINFO *transb )
{       
    char	          temp_buf[61];        /*��ʱʹ�õ��ַ�������*/
    char            service_name[20];
	int Vret = 0;
	tLoginOpr vLog; 
	
	time_t t_beg,t_end;        
    /*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
    char		  vStartTime[20+1];			/* ����ʼʱ�� YYYYMMDDHHMISS*/
    double		vUsedTime=0.00;				/* ��������ʱ�� ��λ����*/
    double		vStartSec=0.00;				/* ����ʼʱ�� ��λ����*/
    BOMC_LOG 	logShip;					/*������־��¼�ṹ���������*/
    EXEC SQL BEGIN DECLARE SECTION;
        char    sTemp[10+1];
        char    sIn_Login_no[6+1];                    /* ��������        */
        char    sIn_Ip_Address[15+1];                 /* ӪҵԱ��½IP    */
        char    sIn_Op_Code[4+1];                     /* ��������        */
        char    sIn_System_Note[60+1];                /* ϵͳ��ע        */        
        char    sIn_Op_Note[60+1];                    /* ������ע        */
        char    sIn_Sim_Type[5+1];                    /* SIM������       */     
        char    sIn_Belong_Code[7+1];                 /* �����������    */           
		    char    vAreaCode[10+1];
		    char    pGroupNo[10+1];
        char    sIn_CustDocFlag[2+1];                 /* �ͻ����ϱ�־   0 ��һ��Ϣ   1  ������Ϣ  */
	
        char    sIn_Cust_Id[22+1];                    /* �ͻ�Id          */
        char    sIn_Cust_Passwd[6+1];                 /* �ͻ�����        */ 
        char    sIn_Sm_code[2+1];                     /* ҵ������        */
        char    sIn_Service_type[2+1];                /* �������ʹ���    */
        char    sIn_Innet_code[2+1];                  /* ��������        */
        char    sIn_Org_code[9+1];                    /* ��������        */
        int     iIn_Bill_Type=0;                      /* ��������        */
        char    sIn_PayFlag[2+1];                     /* ���ѷ�ʽ   0  �ֽ�  1  ֧Ʊ  */
        float   fIn_Sim_fee = 0.00;                   /* SIM����         */
        char    sIn_Fee_List[500];                    /* �ܷ�����Ϣ��    */
        char    sIn_Phone_no_List[2048];              /* �ֻ����봮      */
        char    sIn_Sim_no_List[2048];                /* SIM�����봮     */
        char    sIn_Choice_fee_List[2048];            /* ѡ�ŷѴ�        */  
        char    sIn_Prepay_fee_List[2048];            /* ѡ��Ԥ�洮      */ 
        char    sIn_Package_List[2048];               /* �ײ���Ϣ��      */    
        char    sIn_CustInfo[200];                    /* �ͻ���Ϣ��      */
        char    sIn_Favour_List[2048];                /* �Ż���Ϣ��      */
        char    sIn_Password[2048];                   /* �û��ʻ����봮  */
        char    sIn_NewOldFlag[2+1];                  /* �ͻ�����   0 �½��ͻ�  1 ���пͻ�    */
        char    sIn_Login_Accept[22+1];               /* ������ˮ        */ 
        char    sIn_Check_No[20+1];                   /* ֧Ʊ����        */
        char    sIn_Bank_Code[5+1];                   /* ���д���        */
        char    pUsedFunc_List[ROWLEN];               /* ������Ч���ط�������ʽ��'AAYYYYMMDD|'  */
        char    pUnUsedFunc_List[ROWLEN];             /* ԤԼ��Ч���ط�������ʽ��'AAYYYYMMDDYYYYMMDD|'  */
        char    pAddAddFunc_List[ROWLEN];             /* ���̺ŵ��ط�������ʽ��'AAXXXXXXXX '  */
        char    sV_Flag[1+1];
        char    tpUsedFunc_List[ROWLEN];
        
          
        
/* 
sIn_Fee_List = �ֽ𽻿� ~ ֧Ʊ���� ~ Ԥ��� ~ SIM����  ~ ѡ�ŷ� ~ ������
sIn_CustInfo = �ͻ����� ~ �ͻ���ַ ~ �ͻ�֤������ ~ �ͻ�֤������

*/
        
        char    sV_Login_accept[22+1] ;               /* ������ˮ        */
        char    sV_Offon_accept[22+1];                /* ���ػ���ˮ      */    
        char    sV_User_Id[22+1];                     /* �û�ID          */
        char    sV_Acct_Id[22+1];                     /* �ʻ�ID          */
        char    sV_BillMode[100][8+1];                /* �ʷѴ���        */
                    
        char    sV_Total_date[20+1];                  /* ��������        */
        char    sV_Sys_date[20+1];                    /* ϵͳ����        */
        char    sV_Region_code[2+1];                  /* ��������        */
        int     iV_BillModeNum=0;            /*   �û�������ʷѴ������������ʷѣ���ѡ����*/ 
        
        int     iV_FavNum=0;                          /* ӪҵԱ�Ż�����  */
        char    sV_Favour_Code[100][4+1];             /* �Żݴ���        */
        char    sV_Should_Data[100][17+1];            /* �Ż�ǰ����      */
        char    sV_Actual_Data[100][17+1];            /* �Żݺ�����      */
        
        char    sV_Phone_no_List[100][16];            /* �ֻ����봮      */
        char    sV_Password_List[100][9];             /* �û����봮      */
        char    sV_Sim_no_List[100][21];              /* SIM�����봮     */
        float   fV_Choice_fee_List[100];              /* ѡ�ŷѴ�        */  
        float   fV_Prepay_fee_List[100];              /* ѡ��Ԥ�洮      */ 
        
        float   fV_Cash_Pay = 0.00 ;                  /* �ֽ𽻿�        */ 
        float   fV_Check_Pay = 0.00 ;                 /* �ֽ𽻿�        */ 
        float   fV_Sum_Cash_Pay = 0.00 ;              /* ���ֽ𽻿�      */   
        float   fV_Sum_Check_Pay = 0.00 ;             /* ��֧Ʊ����      */
        float   fV_Sum_Prepay_Fee = 0.00 ;            /* ��Ԥ���        */
        float   fV_Sum_Sim_Fee = 0.00 ;               /* ��SIM����       */
        float   fV_Sum_Choice_Fee = 0.00 ;            /* ��ѡ�ŷ�        */
        float   fV_SumHand_Fee = 0.00 ;               /* ������          */
        
        char    sV_Cust_Name[60+1];                   /* �ͻ�����        */
        char    sV_Cust_Address[60+1];                /* �ͻ���ַ        */
        char    sV_CustId_Type[2+1];                  /* �ͻ�֤������    */
        char    sV_Id_Iccid[20+1];                    /* �ͻ�֤������    */
        char    sV_Iccid[20+1];     
        
        
        int     iV_PhoneCount = 0 ;                   /* �ֻ���������    */
        int     iV_PassCount = 0 ;                    /* �û�����    */
        int     iV_Count = 0;                         /* ����������      */
        char    sV_YYYYMM[6+1];                       /* ����            */
        
        char    vTmp_List[2048];
        char    vTmp_List1[2048];
        int     tmplen=0,begin=1,end=1,num=0,i=0,k=1;
        int     tmplen1=0,begin1=1,end1=1,num1=0,i1=0,k1=1;
        
        char    sTmp_Str[30];                         /* ��ʱ�ַ���      */ 
        int     ret =0;                               /* ��������ֵ      */  
        int     iPhone=0;                             /* ������          */
        char    iEffectTime[17+1];
		char	vModeCode1[8+1];
		char	TableName[30+1];
		char	dynStmt[1024];
		char	pId_No[22+1];
		char	sV_Phone_no_1[15+1];
         /* ����ϵͳĬ��ֵ */
         float  fV_Limit_Owe  = 0.00;                /* ������           */
         char   sV_Attr_Code[8+1];                   /* �û�����         */
         char   sV_Credit_Code[2+1];                 /* �����ȴ���       */
         int    iV_Credit_Value = 0;                 /* ������ֵ         */
         char   sV_Pay_Code[1+1];                    /* ���Ѵ���         */
         char   sV_Pay_Type[4+1];                    /* ��������         */ /*�������*/
         int    iV_Cmd_Right = 0;                    /* Ȩ��ֵ           */
         char   sV_Machine_Code[3+1];                /* ��������         */
         char   sV_Assure_Name[60+1];                /* ����������       */
         char   sV_Id_Type[2+1];                     /* ������֤������   */
         char   sV_Assure_Id[20+1];                  /* ������֤������   */
         char   sV_Assure_Phone[20+1];               /* �����˵绰       */
         char   sV_Assure_Address[60+1];             /* �����˵�ַ       */
         char   sV_Assure_ZIP[6+1];                  /* �������ʱ�       */ 
         char   sV_Post_Flag[1+1];                   /* �ʼ��ʵ���־     */
         char   sV_Post_Type[1+1];                   /* �ʼ��ʵ�����     */
         char   sV_Post_Address[200+1];               /* �ʼ��ʵ���ַ     */
         char   sV_Post_ZIP[6+1];                    /* �ʼ��ʵ��ʱ�     */
         char   sV_Post_Fax[30+1];                   /* �ʼ��ʵ�����     */
         char   sV_Post_Mail[30+1];                  /* �ʼ��ʵ�Mail     */
         char   sV_Post_Content[1+1];                  /* �ʼ��ʵ�Mail     */
         float  fV_Machine_Fee=0.00;                 /* ������           */
         float  fV_Innet_Fee=0.00;                   /* ������           */
         float  fV_Other_Fee=0.00;                   /* ������           */
         float  fV_Hand_Fee= 0.00;                   /* ������           */
         char   sV_service_type1[2+1];
         char   vErrMsg[60+1];                       /* �����ʷѴ�����Ϣ */
         char   sV_ImsiNo[15+1];                     /* IMSI��           */
         char   sV_hlrCode[1+1];                     /* HLR����          */
		
         char  sTrue_flag[1+1];
         char  sCustinfoType[2+1];
         char  sagreement_type[1+1];
         char vLimitCause[256+1];
    	 char vDealType[1+1];
    	 char strTime[14+1];
		 char strTime2[14+1];
         char vSimPassWd[8+1];
         
         int vCount=0;
         char vGCustName[60+1];
         char vGIdIccid[20+1];
         char vGtrueFlag[1+1];
         int	vOtaCount=0;
         char offenFunc[32+1];
		 char vrunCode[2+1];
		 double V_LimitFee=0;
		 char	sv_funccode[25+1];
		 char	newfuncList[128+1];
  		 char	oldfuncList[128+1];
  		 char	newfuncStr[128+1];
         char	defaultfuncList[64+1];
/*add by liupenga ��֯��������20090304 begin*/    
	      char vOrgId[10+1];
	      char vGroupId[10+1];    
	      char vChgFlag[1+1];
	      char vCustFlag[1+1];
	      char vsql_t[1024]; 
	      char vGroupID_dis[10+1];
	      char tmp_org_code[30+1];
	      char tmp_areacode[5+1];
	      int  Root_Distance=-1;
	      char vPassWord[16+1];
/*add by liupenga ��֯��������20090304 begin*/   

      	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
        int   v_ret=0;
    	char  vCallServiceName[30+1];
    	int   vSendParaNum=0;
    	int   vRecpParaNum=0;
    	char  vParamList[DLMAXITEMS][2][DLINTERFACEDATA];
    	int   iOrderRight=100;
      	char  mRegion_Code[2+1];         /*   ��������       */
      	char  pPayFlag[1+1];
      	int   iGoodNoCount=0;
      	char  vOrg_Code[9+1];            /*   ��������       */
      	char  cV_Prepay_fee_List[20+1];
        char  cV_Sum_Check_Pay[20+1];   	
      	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/
      	
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
		
		char vaccflag[1+1]; /*�Ƿ�����ڱ�־*/
		long  vcontractNo=0; /*�˻�*/
		char vcycleDay[2+1];/*����  28������  �磺1�š�2�� ......28��    */
		int iaccnum=0;
		char vnowdate[2+1];/*ϵͳ��ǰʱ�� DD*/
		char	vCustAcctCycleBeginTime[17+1];
		char	vCustAcctCycleEndTime[17+1];
		int		vRetout=0;
		
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
      /***************add for ��𾻶���� 2012.6.18*********************/
    int   vIsNetAmount=0;            /*   �Ƿ񾻶����(0 ���ʽ, 1 �Ǿ��ʽ)  */
    char  vIsOutright[1+1];          /*   �Ƿ���ϴ�����(Y:��ʾ���ڷ���ϴ�����)  */
    float vReward_Fee=0;             /*   �����̳��     */
    float vTax_Fee=0;                /*   ����˰��       */
    /***************add for ��𾻶���� *********************/	 	
    int iRet = 0;
  	 	long	iLoginAccept=0;
  	 	char	inResv1[5+1];
    EXEC SQL END DECLARE SECTION;
    
    tFuncEnv funcEnv;
	/*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
	init(vStartTime);
	memset(&logShip, 0, sizeof(logShip));
	/*��ȡ����ʼʱ��*/
	getBeginTime(&vStartTime,&vStartSec);
    /*
    memset(funcEnv, 0, sizeof(funcEnv));
    */
    /* ���Ի����� */
    init(vGCustName);
    init(vGIdIccid);
    init(vSimPassWd);
    init(sagreement_type);
    init(sIn_Login_no);
    init(sIn_Ip_Address);
    init(sIn_Op_Code);
    init(sIn_System_Note);
    init(sIn_Op_Note);
    init(sIn_Sim_Type);
    init(sIn_Belong_Code);
    init(sIn_CustDocFlag);
    init(sIn_Cust_Id);
    init(sIn_Cust_Passwd);
    init(sIn_Sm_code);
    init(sIn_Service_type);
    init(sIn_Innet_code);
    init(sIn_Org_code);
    init(sIn_PayFlag);
    init(sIn_Fee_List);
    init(sIn_Phone_no_List);
    init(sIn_Sim_no_List);
    init(sIn_Choice_fee_List);
    init(sIn_Prepay_fee_List);
    init(sIn_Package_List);
    init(sIn_CustInfo);
    init(sIn_Favour_List);
    init(sIn_Password);
    init(sIn_NewOldFlag);
    init(sIn_Check_No);
    init(sIn_Bank_Code);
    init(pUsedFunc_List);
    init(tpUsedFunc_List);
    init(pUnUsedFunc_List);
    init(pAddAddFunc_List);
    init(sV_Flag);
    init(vAreaCode);
    init(pGroupNo);
    
    init(sV_Login_accept);
    init(sV_Offon_accept);
    init(sV_User_Id);
    init(sV_Acct_Id);
    init(sV_Total_date);
    init(sV_Sys_date);
    init(sV_Region_code);
    init(sIn_Login_Accept);
    init(sV_service_type1);
    init(vErrMsg);
    init(sV_ImsiNo);
    init(sV_hlrCode);
    init(sV_Iccid);
    
    init(iEffectTime);
    init(TableName);
	init(dynStmt);
	init(pId_No);
	init(vLimitCause);
	init(vDealType);
	init(strTime);
	init(strTime2);
	init(sv_funccode);
	/*add by liupenga ��֯��������20090304*/
    init(vOrgId);
    init(vGroupId);
    init(vChgFlag);
    init(vCustFlag);
    init(vsql_t);
    init(vGroupID_dis);
    init(tmp_org_code);
    init(tmp_areacode); 
    
    /***************add for ��𾻶���� 2012.6.18*********************/
    init(vIsOutright);
    /***************add for ��𾻶���� *********************/
    
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	init(vaccflag);
	init(vcycleDay);
	init(vnowdate);
	init(vCustAcctCycleBeginTime);
	init(vCustAcctCycleEndTime);
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/

	memset(&vLog,0,sizeof(vLog));	
	
	  init(sTemp);
	  init(sTrue_flag);
	  init(sCustinfoType);
    for (i=0;i<100;i++)
    {
         init(sV_Favour_Code[i]);
         init(sV_Should_Data[i]);
         init(sV_Actual_Data[i]);
         init(sV_Phone_no_List[i]);
         init(sV_Password_List[i]);
         init(sV_Sim_no_List[i]);
         
         fV_Choice_fee_List[i] = 0.00;
         fV_Prepay_fee_List[i] = 0.00;
    }
    
    init(sV_Cust_Name);
    init(sV_Cust_Address);
    init(sV_CustId_Type);
    init(sV_Id_Iccid); 
    init(offenFunc);
    init(vrunCode);
    init(newfuncList);
  	init(oldfuncList);
  	init(newfuncStr);
  	init(defaultfuncList);
    init(vPassWord);
    
    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
    init(vCallServiceName);
    init(mRegion_Code);
    init(pPayFlag);
    init(vOrg_Code);
    init(cV_Prepay_fee_List);
    init(cV_Sum_Check_Pay);
  	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/
    
    strcpy(service_name,"s1114Cfm");
    init(inResv1);
    strcpy(inResv1,"ALL");
    Trim(inResv1);
    
    TUX_SERVICE_BEGIN(iLoginAccept,sIn_Op_Code,sIn_Login_no,sIn_Op_Note,ORDERIDTYPE_CUST,sIn_Cust_Id,inResv1,NULL);
    
    /* ����ϵͳĬ��ֵ */
    /*strcpy(sV_Credit_Code , "E");*/ 
    strcpy(sV_Credit_Code , "S"); 
    iV_Credit_Value = 100;
    strcpy(sV_Pay_Code  , "0"); 
    strcpy(sV_Pay_Type  , "0"); 
    iV_Cmd_Right = 1; 
    strcpy(sV_Machine_Code   , "ZZZ"); 
    strcpy(sV_Assure_Name , "NULL");
    strcpy(sV_Id_Type, "Z"); 
    strcpy(sV_Assure_Id , "");
    strcpy(sV_Assure_Phone ,"NULL"); 
    strcpy(sV_Assure_Address ,"NULL");
    strcpy(sV_Assure_ZIP  , "NULL");
    strcpy(sV_Post_Flag , "0");
    strcpy(sV_Post_Type , "Z");
    strcpy(sV_Post_Address , "NULL");
    strcpy(sV_Post_ZIP  , "NULL");
    strcpy(sV_Post_Fax , "NULL");
    strcpy(sV_Post_Mail , "NULL");
    strcpy(sV_Post_Content , "");
    fV_Machine_Fee = 0;
    fV_Innet_Fee = 0;
    fV_Other_Fee = 0; 
    fV_Hand_Fee  = 0; 
    strcpy(vrunCode,"AA");
	time(&t_beg);


    /* ȡ��������� */
    strcpy(sTemp,    input_parms[0]);
    strcpy(sIn_Ip_Address,  input_parms[1]);
    strcpy(sIn_Op_Code,     input_parms[2]);
    strcpy(sIn_System_Note, input_parms[3]);
    strcpy(sIn_Op_Note,     input_parms[4]);
    strcpy(sIn_Sim_Type,    input_parms[5]);

    strcpy(sIn_Belong_Code, input_parms[6]);       
    strcpy(sIn_CustDocFlag, input_parms[7]);
    strcpy(sIn_Cust_Id,     input_parms[8]);
    strcpy(sIn_Cust_Passwd, input_parms[9]);
    strcpy(sIn_Sm_code,     input_parms[10]);
    strcpy(sIn_Service_type,input_parms[11]);
    strcpy(sIn_Innet_code,  input_parms[12]);
    strcpy(sIn_Org_code,    input_parms[13]);
    iIn_Bill_Type = atoi(   input_parms[14]);
    strcpy(sIn_PayFlag,     input_parms[15]);
    fIn_Sim_fee = atof(     input_parms[16]);
    strcpy(sIn_Fee_List,    input_parms[17]);
    strcpy(sIn_Phone_no_List,   input_parms[18]);
    strcpy(sIn_Sim_no_List,     input_parms[19]);
    strcpy(sIn_Choice_fee_List, input_parms[20]);
    strcpy(sIn_Prepay_fee_List, input_parms[21]);
    strcpy(sIn_Package_List,    input_parms[22]);
    strcpy(sIn_CustInfo,        input_parms[23]);
    strcpy(sIn_Favour_List,     input_parms[24]);
    strcpy(sIn_Password,        input_parms[25]);
    strcpy(sIn_NewOldFlag,      input_parms[26]);
    strcpy(sIn_Login_Accept,    input_parms[27]);
    strcpy(sIn_Check_No,        input_parms[28]);
    strcpy(sIn_Bank_Code,       input_parms[29]);
    strcpy(pUsedFunc_List,      input_parms[30]);
    strcpy(pUnUsedFunc_List,    input_parms[31]);
    strcpy(pAddAddFunc_List,    input_parms[32]);
    strcpy(sV_Flag,             input_parms[33]);
    
   
    strcpy(vPassWord,input_parms[34]); /*�����������*/

	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	strcpy(vaccflag,input_parms[35]);  /*�����Ƿ�����ڱ�־*/
	strcpy(vcycleDay,input_parms[36]);  /*����Ƕ������������ڣ�����ʹ�����Ȼ���� 1*/
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/

    /*strcpy(vPassWord,"COJKPFLAJGAPLGEB");*/
    strncpy(sIn_Login_no,sTemp,6);
    strncpy(sTrue_flag,sTemp+6,1);
    strncpy(sCustinfoType,sTemp+6+1,2);
    strncpy(sagreement_type,sTemp+6+1+2,1);
    
    
    
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sIn_Login_no = [%s]",sIn_Login_no);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Ip_Address = [%s]",sIn_Ip_Address);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Code = [%s]",sIn_Op_Code);
        pubLog_Debug(_FFL_, service_name, "", "sIn_System_Note = [%s]",sIn_System_Note);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Note = [%s]",sIn_Op_Note);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Sim_Type = [%s]",sIn_Sim_Type);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Belong_Code = [%s]",sIn_Belong_Code);
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustDocFlag = [%s]",sIn_CustDocFlag);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Id = [%s]",sIn_Cust_Id);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Passwd = [%s]",sIn_Cust_Passwd);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Sm_code = [%s]",sIn_Sm_code);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Service_type = [%s]",sIn_Service_type);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Innet_code = [%s]",sIn_Innet_code);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Org_code = [%s]",sIn_Org_code);
        pubLog_Debug(_FFL_, service_name, "", "iIn_Bill_Type = [%d]",iIn_Bill_Type);
        pubLog_Debug(_FFL_, service_name, "", "sIn_PayFlag = [%s]",sIn_PayFlag);
        pubLog_Debug(_FFL_, service_name, "", "fIn_Sim_fee = [%f]",fIn_Sim_fee);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Fee_List = [%s]",sIn_Fee_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Phone_no_List = [%s]",sIn_Phone_no_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Sim_no_List = [%s]",sIn_Sim_no_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Choice_fee_List = [%s]",sIn_Choice_fee_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Prepay_fee_List = [%s]",sIn_Prepay_fee_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Package_List = [%s]",sIn_Package_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_CustInfo = [%s]",sIn_CustInfo);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Favour_List = [%s]",sIn_Favour_List);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Password = [%s]",sIn_Password);
        pubLog_Debug(_FFL_, service_name, "", "sIn_NewOldFlag = [%s]",sIn_NewOldFlag);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Login_Accept = [%s]",sIn_Login_Accept);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Check_No = [%s]",sIn_Check_No);
        pubLog_Debug(_FFL_, service_name, "", "sIn_Bank_Code = [%s]",sIn_Bank_Code);
        pubLog_Debug(_FFL_, service_name, "", "pUsedFunc_List = [%s]",pUsedFunc_List);
        pubLog_Debug(_FFL_, service_name, "", "pUnUsedFunc_List = [%s]",pUnUsedFunc_List);
        pubLog_Debug(_FFL_, service_name, "", "pAddAddFunc_List = [%s]",pAddAddFunc_List);
        pubLog_Debug(_FFL_, service_name, "", "sV_Flag = [%s]",sV_Flag);

		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
		pubLog_Debug(_FFL_, service_name, "", "vaccflag  =[%s]",    vaccflag     );
		pubLog_Debug(_FFL_, service_name, "", "vcycleDay=[%s]",vcycleDay);
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/

    #endif
	
 /*--������֯�������� liuhao    200090520 begin*/	
	sprintf (oRetCode, "%06d", spublicRight( LABELDBCHANGE, sIn_Login_no, vPassWord, sIn_Op_Code ));
	if (strcmp(oRetCode, "000000") != 0)
	{
		strcpy(oRetMsg,"����Ȩ����֤ʧ��!");
		pubLog_Debug(_FFL_, service_name, "000001", oRetMsg);		
		RAISE_ERROR(oRetCode,oRetMsg);
	}                                      
/*--������֯�������� liuhao    200090520 end*/		
	Vret=0;
	Vret=sGetLoginOrgid(sIn_Login_no,vOrgId);
	if(Vret!=0)
	{
		sprintf(oRetCode, "%06d",Vret);
		strcpy(oRetMsg,"ȡ�˻���������!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);			
	}
	
    /* �����û����� */
    /* ����ǲ��ϻ� ���� sV_Attr_Code = dCustDoc.OWNER_GRADE + '000'
                  ����   sV_Attr_Code �� '000000'                 */
    
    sprintf(oRetCode, "%06d", spublicLimit(sIn_Login_no,sIn_Op_Code,vLimitCause,vDealType));
	Trim(vLimitCause);
	pubLog_Debug(_FFL_, service_name, "", "oRetCode[%s]vLimitCause[%s]vDealType[%s]",oRetCode,vLimitCause,vDealType);
	if(strcmp(oRetCode,"000000")!=0)
	{
		if(strcmp(vDealType,"0")!=0)
		{
    		pubLog_Debug(_FFL_, service_name, "", "vLimitCause[%s]oRetCode[%s]", vLimitCause, oRetCode);
    		strcpy(oRetMsg,vLimitCause);
    		PUBLOG_DBDEBUG(service_name);
    		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    		RAISE_ERROR(oRetCode,oRetMsg);
		}
	}
    
    
    if(strcmp(sIn_NewOldFlag,"1")==0)
    {
        /* ����ǲ��ϻ� */  
        init(sV_Attr_Code);
        /* �������� vAttr_Code = dCustDoc.OWNER_GRADE + '000' */
        EXEC SQL select OWNER_GRADE||'000Y' into :sV_Attr_Code
                 from dCustDoc 
                 where CUST_ID = :sIn_Cust_Id;
        if (SQLCODE != 0 )
        {
            strcpy(oRetCode,"111488");
            strcpy(oRetMsg, "��ѯ dCustDoc �����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);	
        }
    }
    else
    {
    	strcpy(sV_Attr_Code , "00000Y"); 
    }
    
    Trim(sV_Attr_Code);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_Attr_Code = [%s]",sV_Attr_Code);
    #endif
	
    /* ��ѯ �������� */
    EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss'),to_char(sysdate,'yyyymmdd'),
                    to_char(sysdate,'yyyymm'),substr(:sIn_Org_code,1,2),
                    TO_CHAR(add_months(SysDate,1),'yyyymm')||'01 00:00:00',to_char(sysdate,'dd')
            INTO :sV_Sys_date,:sV_Total_date  ,:sV_YYYYMM , :sV_Region_code,:iEffectTime,:vnowdate
            FROM dual;
    if (SQLCODE !=0)
    {
        strcpy(oRetCode,"111401");
        strcpy(oRetMsg,"��ѯ�������ڳ���!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);	
    }
    
    Trim(sV_Sys_date);
    Trim(sV_Total_date);
    Trim(sV_YYYYMM);
    Trim(sV_Region_code);
    Trim(vnowdate);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_Sys_date=[%s],sV_Total_date = [%s]",sV_Sys_date,sV_Total_date);
        pubLog_Debug(_FFL_, service_name, "", "sV_YYYYMM = [%s] ",sV_YYYYMM); 
        pubLog_Debug(_FFL_, service_name, "", "sV_Region_code = [%s] ",sV_Region_code); 
        pubLog_Debug(_FFL_, service_name, "", "vnowdate = [%s] ",vnowdate); 
    #endif
    
    /* ȡ�ò�����ˮ */
    Trim(sIn_Login_Accept);
    if  ((strlen(sIn_Login_Accept)==0)||(strcmp(sIn_Login_Accept,"0")==0))
    {
        EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :sV_Login_accept
             FROM DUAL;
             if(SQLCODE!=0){
                 pubLog_Debug(_FFL_, service_name, "", "ȡ����������ˮ����!");
                 strcpy(oRetCode,"111488");
                 strcpy(oRetMsg, "ȡ����������ˮ����!");
                 PUBLOG_DBDEBUG(service_name);
                 pubLog_DBErr(_FFL_, service_name, "111488","%s",  oRetMsg);
                 RAISE_ERROR(oRetCode,oRetMsg);
             }
    
    }
    else
    {
    	strcpy(sV_Login_accept , sIn_Login_Accept);
    }
    
    Trim(sV_Login_accept);
    
    iLoginAccept=atol(sV_Login_accept);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " sV_Login_accept = [%s]",sV_Login_accept);
    #endif
    
    sprintf(strTime,"%ld",getSysTime(112));

    
    /*----------------------- �����Ż���Ϣ�� --------------------------*/
    if(strlen(ltrim(rtrim(sIn_Favour_List)))!=0)
    {        
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        memcpy(vTmp_List,sIn_Favour_List,sizeof(sIn_Favour_List)-1);
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
        
        strcpy(sTmp_Str,"MULL");
        for(i=0;i<=tmplen;i++)
        {
            if(vTmp_List[i]=='~')
            {
                k=k+1;
                end = i;
                num = end - begin + 1;
                   
            
                memset(vTmp_List1,0x00,sizeof(vTmp_List1));
                strcpy(vTmp_List1,rtrim(substr(vTmp_List,begin,num)));
            
                tmplen1=strlen(vTmp_List1);
                begin1 = 1; end1 = 1; k1=0;
            
                for(i1=0;i1<=tmplen1;i1++)
                {
                    if(vTmp_List1[i1]=='&')
                    {
                        k1=k1+1;
                        end1 = i1;
                        num1 = end1 - begin1 + 1;
                        vTmp_List1[i1]='\0';
                        if(k1==1) strcpy(sV_Favour_Code[k-1],substr(vTmp_List1,begin1,num1));
                        if(k1==2) strcpy(sV_Should_Data[k-1],substr(vTmp_List1,begin1,num1));
                        if(k1==3) strcpy(sV_Actual_Data[k-1],substr(vTmp_List1,begin1,num1));
                    
                        begin1 = end1 +2;
                    }
               }
            
               #ifdef _DEBUG_
                   pubLog_Debug(_FFL_, service_name, "", "�Żݴ���[%d]       = [%s]",k,sV_Favour_Code[k-1]);
                   pubLog_Debug(_FFL_, service_name, "", "�Ż�ǰ����[%d]     = [%s]",k,sV_Should_Data[k-1]);
                   pubLog_Debug(_FFL_, service_name, "", "�Żݺ�����[%d]     = [%s]",k,sV_Actual_Data[k-1]);
               #endif
               ret=-1;
               
               strcpy(sV_User_Id,"0");
               
               ret=pubOpenAcctFav(sV_User_Id,            sTmp_Str,           sIn_Sm_code,         
                                  sIn_Belong_Code,       sV_Favour_Code[k-1],           sV_Should_Data[k-1],
                                  sV_Actual_Data[k-1],   sIn_Login_no,                  sV_Login_accept,    
                                  sIn_Op_Note,           sIn_System_Note,               sIn_Op_Code,            
                                  sV_Total_date,         sV_Sys_date,vOrgId);
               if(ret != 0)
               {
                   pubLog_Debug(_FFL_, service_name, "", "�����Ż���Ϣ����!");
                   strcpy(oRetCode,"111402");
                   strcpy(oRetMsg, "�����Ż���Ϣ����!");
                   PUBLOG_DBDEBUG(service_name);
                   pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                   RAISE_ERROR(oRetCode,oRetMsg);
               }
               begin = end +2;
            }
        }
    }
    
    /*----------------------- �����ײ���Ϣ����ʼ --------------------------*/
    if(strlen(ltrim(rtrim(sIn_Package_List)))!=0){
        memset(vTmp_List,0x00,sizeof(vTmp_List));       
        memcpy(vTmp_List,sIn_Package_List,sizeof(sIn_Package_List)-1);
        pubLog_Debug(_FFL_, service_name, "", "----Tmp_List=[%s]",vTmp_List);        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;                        
                                                        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                end = i;
                num = end - begin + 1;
                strcpy(sV_BillMode[k],substr(vTmp_List,begin,num));
                begin = end +2;
                k++;                
            }
        }
        iV_BillModeNum = k;
        #ifdef _DEBUG_
            for(i=0; i<=iV_BillModeNum; i++){
                pubLog_Debug(_FFL_, service_name, "", "�ʷѴ���=[%s]",sV_BillMode[i]);
            }           
        #endif
    }        
    /*----------------------- �����ײ���Ϣ������ --------------------------*/
    
    
    /* ��ֿͻ���Ϣ�� */     
    if(strlen(ltrim(rtrim(sIn_CustInfo)))!=0)
    {    
        memset(vTmp_List,0x00,sizeof(vTmp_List));    
        memcpy(vTmp_List,sIn_CustInfo,sizeof(sIn_CustInfo)-1);
        
        tmplen=strlen(vTmp_List);                       
        begin = 1; end = 1; k=0;     
        
        for(i=0;i<=tmplen;i++){                         
        if(vTmp_List[i]=='~'){
                k=k+1;
                end = i;
                num = end - begin + 1;
                /*
                vTmp_List[i]='\0';
                */
                if(k==1) strcpy(sV_Cust_Name,substr(vTmp_List,begin,num));
                if(k==2) strcpy(sV_Cust_Address,substr(vTmp_List,begin,num)); 
                if(k==3) strcpy(sV_CustId_Type,substr(vTmp_List,begin,num));
                if(k==4) strcpy(sV_Id_Iccid,substr(vTmp_List,begin,num));
                begin = end +2;
            }
        }
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�����  =[%s]",sV_Cust_Name);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ���ַ  =[%s]",sV_Cust_Address);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������ =[%s]",sV_CustId_Type);
            pubLog_Debug(_FFL_, service_name, "", "�ͻ�֤������ =[%s]",sV_Id_Iccid);  
        #endif      
             
    }   
    
    /* ����ܷ�����Ϣ�� */
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,sIn_Fee_List,sizeof(sIn_Fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            k=k+1;
            end = i;
            num = end - begin + 1;
            
            if(k==1) fV_Sum_Cash_Pay = atof(substr(vTmp_List,begin,num));
            if(k==2) fV_Sum_Check_Pay = atof(substr(vTmp_List,begin,num));
            if(k==3) fV_Sum_Prepay_Fee = atof(substr(vTmp_List,begin,num));
            if(k==4) fV_Sum_Sim_Fee = atof(substr(vTmp_List,begin,num));
            if(k==5) fV_Sum_Choice_Fee = atof(substr(vTmp_List,begin,num));
            if(k==6) fV_Hand_Fee = atof(substr(vTmp_List,begin,num));
            if(k==7) strcpy(vAreaCode, substr(vTmp_List,begin,num));
	    if(k==8) strcpy(pGroupNo, substr(vTmp_List,begin,num));     
	    if(k==9) strcpy(sV_Iccid, substr(vTmp_List,begin,num));   
	    if(k==10) strcpy(vIsOutright,substr(vTmp_List,begin,num));	 
	    if(k==11) vIsNetAmount=atoi(substr(vTmp_List,begin,num));
      if(k==12) vReward_Fee=atof(substr(vTmp_List,begin,num));
      if(k==13) vTax_Fee=atof(substr(vTmp_List,begin,num));
            begin = end +2;
        }
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "���ֽ𽻿�=[%10.2f]",fV_Sum_Cash_Pay);
        pubLog_Debug(_FFL_, service_name, "", "��֧Ʊ����=[%10.2f]",fV_Sum_Check_Pay);
        pubLog_Debug(_FFL_, service_name, "", "��Ԥ���=[%10.2f]",fV_Sum_Prepay_Fee);
        pubLog_Debug(_FFL_, service_name, "", "��SIM����=[%10.2f]",fV_Sum_Sim_Fee);
        pubLog_Debug(_FFL_, service_name, "", "��ѡ�ŷ�=[%10.2f]",fV_Sum_Choice_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",fV_Hand_Fee);
        pubLog_Debug(_FFL_, service_name, "", "areaCode=[%s]",vAreaCode);
        pubLog_Debug(_FFL_, service_name, "", "pGroupNo=[%s]",pGroupNo);
        pubLog_Debug(_FFL_, service_name, "", "sV_Iccid=[%s]",sV_Iccid);
        pubLog_Debug(_FFL_, service_name, "", "�����̳��=[%10.2f]",vReward_Fee);
        pubLog_Debug(_FFL_, service_name, "", "����˰��=[%10.2f]",vTax_Fee);
    #endif
    Trim(vAreaCode);
    Trim(pGroupNo);
    /* ��� �ֻ����봮 */
    iV_PhoneCount = 0;
    memset(sV_Phone_no_List,0,sizeof(sV_Phone_no_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Phone_no_List,sizeof(sIn_Phone_no_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, service_name, "", "phone=[%s]",sTmp_Str );
            #endif
            
            strcpy(sV_Phone_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_PhoneCount = k;
    
    pubLog_Debug(_FFL_, service_name, "", "�����ֻ�����=[%d]",iV_PhoneCount);
    
    if (iV_PhoneCount < 1 )
    {
        strcpy(oRetCode,"111430");
        strcpy(oRetMsg,"������ֻ���������Ϊ0!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);	
    }
    
    for (i=0;i<iV_PhoneCount;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]���ֻ����룽[%s]",i,sV_Phone_no_List[i]);
        #endif
    }

    /* ����û����봮 */
    if(strcmp(sIn_Op_Code,"1116")==0 || strcmp(sIn_Op_Code,"1118")==0)
    {
        iV_PassCount = 0;
        memset(sV_Password_List,0,sizeof(sV_Password_List));
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        
        memcpy(vTmp_List,sIn_Password,sizeof(sIn_Password)-1);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
        #endif
        
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
        
        for(i=0;i<=tmplen;i++)
        {
            if(vTmp_List[i]=='~')
            {
                end = i;
                num = end - begin + 1;
                
                strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
                
                #ifdef _DEBUG_
                    pubLog_Debug(_FFL_, service_name, "", "password=[%s]",sTmp_Str );
                #endif
                
                strcpy(sV_Password_List[k] ,  sTmp_Str);
                
                k++;
                begin = end +2;
            }
        }
        iV_PassCount = k;
        
        pubLog_Debug(_FFL_, service_name, "", "�û���������=[%d]",iV_PassCount);
        
        if (iV_PassCount < 1 )
        {
            strcpy(oRetCode,"111430");
            strcpy(oRetMsg,"������û���������Ϊ0!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);	
        }
        
        for (i=0;i<iV_PassCount;i++)
        {
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, service_name, "", "��[%d]���û����룽[%s]",i,sV_Password_List[i]);
            #endif
        }
    }
    
    /* ���SIM�����봮 */
    iV_Count = 0;
    init(sTmp_Str);
    memset(sV_Sim_no_List,0,sizeof(sV_Sim_no_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Sim_no_List,sizeof(sIn_Sim_no_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            pubLog_Debug(_FFL_, service_name, "", "sTmp_Str=[%s]",sTmp_Str );
            
            strcpy(sV_Sim_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_, service_name, "", "����SIM������=[%d]",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]��SIM�����룽[%s]",i,sV_Sim_no_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(oRetCode,"111403");
        strcpy(oRetMsg,"������ֻ�����������SIM��������һ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);		
    }
    
    /* ��� ѡ�ŷѴ� */
    iV_Count = 0;
    init(sTmp_Str);
    memset(fV_Choice_fee_List,0,sizeof(fV_Choice_fee_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Choice_fee_List,sizeof(sIn_Choice_fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            pubLog_Debug(_FFL_, service_name, "", "sTmp_Str=[%s]",sTmp_Str );
            
            fV_Choice_fee_List[k] = atof(sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_, service_name, "", "����ѡ�ŷ�����=[%d]",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]��ѡ�ŷѣ�[%f]",i,fV_Choice_fee_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(oRetCode,"111404");
        strcpy(oRetMsg,"������ֻ�����������ѡ�ŷ�������һ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);		
    }
    
    /* ��� ѡ��Ԥ�洮 */
    /* ���� �� ����ʱ������ѡ��Ԥ�� */
    
    /**/
    iV_Count = 0;
    init(sTmp_Str);
    memset(fV_Prepay_fee_List,0,sizeof(fV_Prepay_fee_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Prepay_fee_List,sizeof(sIn_Prepay_fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            pubLog_Debug(_FFL_, service_name, "", "sTmp_Str=[%s]",sTmp_Str );
            
            fV_Prepay_fee_List[k] = atof(sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_, service_name, "", "����Ԥ������=[%d]",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]��Ԥ�棽[%f]",i,fV_Prepay_fee_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(oRetCode,"111405");
        strcpy(oRetMsg,"������ֻ�����������Ԥ���������һ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);		
    }
    
    /*add in 2008-9-10*/
    /* 
    ret=-1;
    pubLog_Debug(_FFL_, service_name, "", "sV_Region_code is %s",sV_Region_code);
    pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Code is %s",sIn_Op_Code);
    pubLog_Debug(_FFL_, service_name, "", "sIn_Sm_code is %s",sIn_Sm_code);
    pubLog_Debug(_FFL_, service_name, "", "vrunCode is %s",vrunCode);
   
    ret=soffencommand(pId_No,sV_Region_code,sIn_Op_Code,sIn_Sm_code,vrunCode,offenFunc);
    if(ret != 0)
    {
        pubLog_Debug(_FFL_, service_name, "", "soffencommandȡ���ػ��������!");
        strcpy(oRetCode,"111491");
        strcpy(sErrorMess, "ȡ���ػ��������!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
        RAISE_ERROR(oRetCode,oRetMsg);
    }
  */  
    /**/
    strncpy(sV_service_type1,sIn_Service_type,2);
    for (iPhone=0;iPhone<iV_PhoneCount;iPhone++)
    {
    	iV_Count=0;
  		EXEC SQL SELECT count(*)
  			   into :iV_Count
	           from dphonereservemsg
	          where phone_no=:sV_Phone_no_List[iPhone]
	            and reservation_flag='0';
			if(SQLCODE!=0)
			{
				strcpy(oRetCode,"111490");
  		  sprintf(oRetMsg, "��ѯdphonereservemsgʧ��[%d]",SQLCODE);
  		  PUBLOG_DBDEBUG(service_name);
  		  pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
  		  RAISE_ERROR(oRetCode,oRetMsg);
			}
			if(iV_Count>0)
			{
				strcpy(oRetCode,"111491");
  		  sprintf(oRetMsg, "ԤԼ����[%s]�뵽��ͨ����ģ�鿪��",sV_Phone_no_List[iPhone]);
  		  PUBLOG_DBDEBUG(service_name);
  		  pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
  		  RAISE_ERROR(oRetCode,oRetMsg);
			}
    	/* ������������� �� ���ɿͻ�ID */
        if (strcmp(sIn_CustDocFlag,"1")==0)
        {
            /* �����µĿͻ�ID */
            if(getMaxID(sV_Region_code, 0, sIn_Cust_Id ) == NULL)
            {
                strcpy(oRetCode,"111406");
                strcpy(oRetMsg,"spubGetId���д���!");
                PUBLOG_DBDEBUG(service_name);
                pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                RAISE_ERROR(oRetCode,oRetMsg);
            }	
            
            Trim(sIn_Cust_Id);
    	    
    	    #ifdef _DEBUG_
                pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Id[%s]",sIn_Cust_Id);
            #endif
        }
    	
    	/* ��ѯ�û�ID , �ʻ�ID */
        if(getMaxID(sV_Region_code, 1, sV_User_Id ) == NULL)
        {
            strcpy(oRetCode,"111407");
            strcpy(oRetMsg,"spubGetId���д���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    	
    	Trim(sV_User_Id);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_User_Id��[%s]",sV_User_Id);
        #endif
        
        if(getMaxID(sV_Region_code, 1, sV_Acct_Id ) == NULL)
        {
            strcpy(oRetCode,"111408");
            strcpy(oRetMsg,"spubGetId���д���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    	
    	Trim(sV_Acct_Id);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_Acct_Id[%s]",sV_Acct_Id);
        #endif    	
    	
    	/* ADD BY ZHCY : 2009-03-11 */
        /*
        *	TD���� �������Ϳ��ػ�����
        */
    	ret=-1;
    	ret=soffencommand(sV_User_Id, sV_Region_code,sIn_Op_Code,sIn_Sm_code,vrunCode,offenFunc);
    	if(ret != 0)
    	{
    	    pubLog_Debug(_FFL_, service_name, "", "soffencommandȡ���ػ��������!");
    	    strcpy(oRetCode,"111491");
    	    strcpy(oRetMsg, "ȡ���ػ��������!");
    	    PUBLOG_DBDEBUG(service_name);
    	    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    	    RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	
    	
    	
        /* �����롢SIM����Ч��*/
        ret=-1;
        ret=CheckPhoneSimNo(sV_Phone_no_List[iPhone],     sIn_Org_code,      sIn_Sm_code,      
                            sV_Sim_no_List[iPhone],       sIn_Sim_Type,       sV_Iccid,sIn_Login_no,oRetMsg,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�����롢SIM����Ч�Դ���[%s]",oRetMsg);
            sprintf(oRetCode,"%d",ret);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        
        /* ���º��롢SIM����Ϣ */
        ret=-1;
        ret=pubOpenAcctRes(sV_User_Id,        sV_Phone_no_List[iPhone],      sV_Sim_no_List[iPhone],
                           sIn_Belong_Code,  sIn_Login_no,      sV_Login_accept, 
                           sIn_Op_Code,      sV_Total_date,    sV_Sys_date,pGroupNo);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "���º���SIM����Դ����!");
            strcpy(oRetCode,"111409");
            strcpy(oRetMsg, "���º���SIM����Դ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }

        fV_Other_Fee = (vTax_Fee - vReward_Fee)/iV_PhoneCount;
        /* ����֧Ʊ���� , �ֽ𽻿� */
        if (strcmp(sIn_PayFlag,"0")==0)
        {
            fV_Cash_Pay = fIn_Sim_fee + fV_Choice_fee_List[iPhone] + fV_Prepay_fee_List[iPhone]+fV_Other_Fee;	
            fV_Check_Pay = 0;
        }
        else
        {
            fV_Cash_Pay =  0;	
            fV_Check_Pay = fIn_Sim_fee + fV_Choice_fee_List[iPhone] + fV_Prepay_fee_List[iPhone]+fV_Other_Fee;
        }
        
        /* ��¼�ͻ����û����ʻ���Ϣ */
        /*
        fV_Other_Fee = fV_Prepay_fee_List[i];
        */
        /*
        fV_Prepay_fee_List[iPhone] = 0.00;
        */
        ret=-1;    
        pubLog_Debug(_FFL_, service_name, "", "sIn_Service_type[%d][%s]",iPhone,sV_service_type1);
        
        if(strcmp(sIn_Op_Code,"1116")!=0 && strcmp(sIn_Op_Code,"1118")!=0)
        {
            strcpy(sV_Password_List[iPhone],sIn_Password);
        }
        else
        {
        	strcpy(sIn_Cust_Passwd,sV_Password_List[iPhone]);
        }
        
        
        vOtaCount=0;
		/*EXEC SQL select count(1)
    				into :vOtaCount    			
    		from sotasimtype
    		where res_code in 
    		(select sim_type from dSimRes where sim_no=:sV_Sim_no_List[iPhone]);
    	if(SQLCODE!=0)
    	{
    	 	strcpy(oRetCode,"111442");
         	strcpy(sErrorMess, "ȡOTA��Ϣʧ��");
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
         	RAISE_ERROR(oRetCode,oRetMsg);
   		}
   		if(vOtaCount>0)
   		{
   			strcpy(oRetCode,"111442");
         	strcpy(sErrorMess, "OTA���������ڴ���������");
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
         	RAISE_ERROR(oRetCode,oRetMsg);
   		}*/ /*modify by wangdp 20080728 ���ڶ�ȫ��ͨƷ�ƿ���OTA��׼��������Ʒ��OTA����������������ϵ�����*/
        
        init(vSimPassWd);
    	EXEC SQL select pass_word
    			into :vSimPassWd
    			from dsimrespassword
    			where sim_no=:sV_Sim_no_List[iPhone]
    			and (region_code=substr(:sIn_Belong_Code,1,2) or region_code='99');
    	if(SQLCODE!=0 && SQLCODE!=1403)
    	{
    	 	strcpy(oRetCode,"111452");
         	sprintf(oRetMsg, "dsimrespassword��Ϣ��ȫ[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
         	PUBLOG_DBDEBUG(service_name);
         	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
         	RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	else if(SQLCODE==0)
    	{
    		init(sV_Password_List[iPhone]);
    		strcpy(sV_Password_List[iPhone],vSimPassWd);
    	
    		if (strcmp(sIn_CustDocFlag,"1")==0)
        	{
        		init(sIn_Cust_Passwd);
    			strcpy(sIn_Cust_Passwd,vSimPassWd);
    		}
    		
    		EXEC SQL update dsimrespassword set total_date=to_number(:sV_Total_date)
    					where sim_no=:sV_Sim_no_List[iPhone]
    					and (region_code=substr(:sIn_Belong_Code,1,2) or region_code='99');
    		if(SQLCODE!=0 && SQLCODE!=1403)
    		{
    		 	strcpy(oRetCode,"111453");
        	 	sprintf(oRetMsg, "dsimrespassword2��Ϣ��ȫ[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
        	 	PUBLOG_DBDEBUG(service_name);
        	 	pubLog_DBErr(_FFL_, service_name, "%s", "%s",oRetCode, oRetMsg);
        	 	RAISE_ERROR(oRetCode,oRetMsg);
    		}
    	}

        pubLog_Debug(_FFL_, service_name, "", "sTrue_flag:[%s]",sTrue_flag);
        pubLog_Debug(_FFL_, service_name, "", "sCustinfoType[%s]",sCustinfoType);
        pubLog_Debug(_FFL_, service_name, "", "sV_Credit_Code[%s]",sV_Credit_Code);
        
        strcpy(sIn_Sm_code,input_parms[10]);
        
        ret=pubOpenAcctConfirm(sIn_Cust_Id,    sV_Acct_Id,          sV_User_Id, 
                             sIn_Cust_Passwd,  sV_Password_List[iPhone],sV_Password_List[iPhone],
                             fV_Limit_Owe,     sV_Phone_no_List[iPhone], sIn_Sm_code ,     
                             sIn_Service_type, sV_Attr_Code,        sV_Credit_Code,  
                             iV_Credit_Value,  sV_Pay_Code,         sV_Pay_Type,     
                             iIn_Bill_Type,    iV_Cmd_Right,        sV_Sim_no_List[iPhone],       
                             sIn_Belong_Code,  sIn_Op_Note,         sIn_System_Note,
                             sIn_Login_no,     sIn_Ip_Address,      sV_Login_accept, 
                             sIn_Op_Code,      sIn_Innet_code,      sV_Machine_Code,
                             sV_Assure_Name,   sV_Id_Type,          sV_Assure_Id,
                             sV_Assure_Phone,  sV_Assure_Address,   sV_Assure_ZIP,
                             sV_Post_Flag,     sV_Post_Type,        sV_Post_Address,
                             sV_Post_ZIP,      sV_Post_Fax,         sV_Post_Mail, sV_Post_Content,
                             fV_Cash_Pay,      fV_Check_Pay,        fV_Prepay_fee_List[iPhone],
                             fIn_Sim_fee,      fV_Machine_Fee,      fV_Innet_Fee,
                             fV_Choice_fee_List[iPhone],   fV_Other_Fee,   fV_Hand_Fee,
                             sV_Total_date,   sV_Sys_date,
                             sV_Cust_Name , sV_Cust_Address ,
                             sV_CustId_Type , sV_Id_Iccid , sIn_NewOldFlag , sIn_CustDocFlag, vAreaCode,pGroupNo,sTrue_flag,sCustinfoType,sagreement_type,vOrgId);
                             
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "����ͻ� �û� �ʻ���Ϣ����!");
            strcpy(oRetCode,"111410");
            strcpy(oRetMsg, "����ͻ� �û� �ʻ���Ϣ����!");
            pubLog_Debug(_FFL_, service_name, "", "call pubOpenAcctConfirm ret = [%d]",ret);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s",oRetCode, oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        } 
		
		/*���ֶ����� �����û��˺���Ϣ�� 2012/5/22 13:47:45 S.H begin */
		if(strcmp(vaccflag,"Y")==0)
		{
			pubLog_Debug(_FFL_, service_name, "", "sV_Acct_Id:[%s]",sV_Acct_Id);
			pubLog_Debug(_FFL_, service_name, "", "sV_User_Id:[%s]",sV_User_Id);
			
			pubLog_Debug(_FFL_, service_name, "", "sV_Phone_no_List[iPhone]:[%s]",sV_Phone_no_List[iPhone]);
			pubLog_Debug(_FFL_, service_name, "", "vcycleDay:[%s]",vcycleDay);
			
			pubLog_Debug(_FFL_, service_name, "", "iLoginAccept:[%ld]",iLoginAccept);
			pubLog_Debug(_FFL_, service_name, "", "sIn_Login_no:[%s]",sIn_Login_no);
			
			pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Note:[%s]",sIn_Op_Note);
			pubLog_Debug(_FFL_, service_name, "", "sV_Region_code:[%s]",sV_Region_code);
			pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Code:[%s]",sIn_Op_Code);
			
			/*���ú���pubCustAcctCycle �������ڱ�dCustAcctCycle*/
			iaccnum=pubCustAcctCycle(atol(sV_Acct_Id),atol(sV_User_Id),sV_Phone_no_List[iPhone],atol(vcycleDay),iLoginAccept,sIn_Login_no,sIn_Op_Note,sV_Region_code,sIn_Op_Code);
			if(iaccnum != 0)
			{
				pubLog_Debug(_FFL_, service_name, "", "���ú���pubCustAcctCycle����! ret = [%d]",iaccnum);
				strcpy(oRetCode,"110405");
				strcpy(oRetMsg, "�����û���������Ϣ�����!");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
		}
		
	
		/*���ֶ����� �����û��˺���Ϣ�� 2012/5/22 13:47:45 S.H end */
        vCount=0;
    	EXEC SQL select count(1)
    				into :vCount
    				from dGoodPhoneRes
    				where  phone_no = :sV_Phone_no_List[iPhone];
    	if(SQLCODE!=0)
    	{
    		strcpy(oRetCode,"111487");
    	    sprintf(oRetMsg, "��ѯ[%s]��������ʧ��[%d]",sV_Phone_no_List[iPhone],SQLCODE);
    	    PUBLOG_DBDEBUG(service_name);
    	    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    	    RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	
    	if(vCount>0)
    	{
    		init(vGCustName);
    		init(vGIdIccid);
    		init(vGtrueFlag);
    		EXEC SQL select cust_name,Id_Iccid,nvl(true_flag,'N')
    				into :vGCustName,:vGIdIccid,:vGtrueFlag
    				from dcustdoc 
    				where cust_id=to_number(:sIn_Cust_Id);
    		if(SQLCODE!=0)
    		{
    			strcpy(oRetCode,"111487");
    	    	sprintf(oRetMsg, "��ѯ[%s]dcustdocʧ��[%d]",sIn_Cust_Id,SQLCODE);
    	    	PUBLOG_DBDEBUG(service_name);
    	    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    	    	RAISE_ERROR(oRetCode,oRetMsg);
    		}
    		Trim(vGCustName);
    		Trim(vGIdIccid);
    		Trim(vGtrueFlag);
    		if(strcmp(vGCustName,"Ԥ����")==0 || strcmp(vGIdIccid,"111111111111111111")==0 || strcmp(vGtrueFlag,"Y")!=0)
    		{
    			strcpy(oRetCode,"111487");
    	    	sprintf(oRetMsg, "����������Ҫ��ʵ��д�ͻ�����[%s]��֤������[%s],�û����ϱ������Ѻ�ʵ",vGCustName,vGIdIccid);
    	    	PUBLOG_DBDEBUG(service_name);
    	    	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
    	    	RAISE_ERROR(oRetCode,oRetMsg);
    		}
    	} 
        
        /* ȡ���ػ���ˮ */
        EXEC SQL SELECT sOffon.nextval  INTO :sV_Offon_accept  FROM DUAL;
                   
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_Offon_accept     = [%s]",sV_Offon_accept);
        #endif
        
        /* �����û���Ч��  �ڿ�ͨ�ײ�ʱ�ᴦ��
        ret=-1;
        ret=pubCBXExpire(sV_User_Id ,     sIn_Belong_Code,   fV_Prepay_fee_List[iPhone],
                         sIn_Op_Note,     sIn_Login_no,      sV_Login_accept, 
                         sIn_Op_Code,     sV_Total_date,     sV_Sys_date,
                         sIn_Sm_code,     sV_BillMode[0]);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�����û���Ч�ڴ���!");
            strcpy(oRetCode,"111412");
            strcpy(oRetMsg, "�����û���Ч�ڴ���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        pubLog_Debug(_FFL_, service_name, "", "gggggggsIn_Service_type[%d][%s]",iPhone,sIn_Service_type);
        */
        
         /* ��ѯ  HLR���� */
        EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
                 where  phoneno_head = substr(:sV_Phone_no_List[iPhone],1,7);
        if (SQLCODE != 0)
        {
            strcpy(oRetCode,"111416");
            strcpy(oRetMsg, "��ѯhlr����ʧ��!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        Trim(sV_hlrCode);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_hlrCode      =[%s]",      sV_hlrCode);
        #endif
        
        /*update 2008-9-24*/
        init(defaultfuncList);
        if(strcmp(sV_Flag,"n")==0)
        {
            ret=-1;
            if(strlen(ltrim(rtrim(sIn_Package_List)))!=0)
            {
                /* ���ʷ��ײͣ����ʷ��ײͣ�vBillMode[0] */
                ret = newpubDefaultFunc(sV_User_Id , sIn_Login_no ,
                     sIn_Belong_Code , sIn_Sm_code ,
                     sV_Total_date , sV_Sys_date ,
                     sV_Login_accept , sIn_Op_Code ,
                     oRetMsg , sV_BillMode[0],
                     defaultfuncList);
                pubLog_Debug(_FFL_, service_name, "", "eeeeeesIn_Service_type[%d][%s]",iPhone,sIn_Service_type);
            }
            else
            {
                /* ���ʷ��ײ� */
                ret = newpubDefaultFunc(sV_User_Id , sIn_Login_no ,
                     sIn_Belong_Code , sIn_Sm_code ,
                     sV_Total_date , sV_Sys_date ,
                     sV_Login_accept , sIn_Op_Code ,
                     oRetMsg , "0",
                     defaultfuncList);
                pubLog_Debug(_FFL_, service_name, "", "eeffffffffsIn_Service_type[%d][%s]",iPhone,sIn_Service_type);
            }
            pubLog_Debug(_FFL_, service_name, "", "call pubDefaultFunc ,ret=[%d][%s]!" , ret,defaultfuncList);
            if(ret != 0)
            {
                pubLog_Debug(_FFL_, service_name, "", "���Ĭ���ط�����,ret=[%d]!" , ret);
                strcpy(oRetCode,"111411");
                pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg);
                PUBLOG_DBDEBUG(service_name);
                pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                RAISE_ERROR(oRetCode,oRetMsg);
            }
        }
        
        /* ���ػ��ӿ� add in 2008-9-10 */   
        ret=-1;
        init(newfuncList);
        init(oldfuncList);
        init(newfuncStr);
        init(tpUsedFunc_List);
        Trim(pUsedFunc_List);
        Trim(defaultfuncList);
        sprintf(tpUsedFunc_List,"%s%s",pUsedFunc_List,defaultfuncList);
	    Trim(tpUsedFunc_List);
	    pubLog_Debug(_FFL_, service_name, "", "++++++tpUsedFunc_List=[%s],pUsedFunc_List=[%s],defaultfuncList=[%s]++++++",tpUsedFunc_List,pUsedFunc_List,defaultfuncList);
	    ret=sNewfuncList(tpUsedFunc_List,offenFunc,newfuncList,oldfuncList,newfuncStr,sV_hlrCode);
	    if(ret != 0)
	    {
	        pubLog_Debug(_FFL_, service_name, "", "sNewfuncList�ϲ����ػ�ָ�����!");
	        strcpy(oRetCode,"111492");
	        strcpy(oRetMsg, "�ϲ����ػ�ָ�����!");
	        PUBLOG_DBDEBUG(service_name);
	        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	        RAISE_ERROR(oRetCode,oRetMsg);
	    }
	    
	    pubLog_Debug(_FFL_, service_name, "", "+++pUsedFunc_List=[%s]",pUsedFunc_List);
	    pubLog_Debug(_FFL_, service_name, "", "+++offenFunc=[%s]",offenFunc);
	    pubLog_Debug(_FFL_, service_name, "", "+++newfuncList=[%s]",newfuncList);
	    pubLog_Debug(_FFL_, service_name, "", "+++newfuncStr=[%s]",newfuncStr);
	    pubLog_Debug(_FFL_, service_name, "", "+++oldfuncList=[%s]",oldfuncList);
        
        /*ret=-1;
        ret=pubOpenAcctSendCmd(sV_User_Id,           sV_Phone_no_List[iPhone],       sV_Attr_Code,
                               sIn_Sm_code,           sIn_Belong_Code,           sIn_Login_no,
                               sV_Login_accept,       sIn_Op_Note,               sIn_System_Note,
                               sIn_Op_Code,           sV_Total_date,             sV_Sys_date,
                               sV_Offon_accept);
        if(ret != 0){
             pubLog_Debug(_FFL_, service_name, "", "���Ϳ��ػ��������!");
            strcpy(oRetCode,"111414");
            strcpy(sErrorMess, "���Ϳ��ػ��������!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
            RAISE_ERROR(oRetCode,oRetMsg);
        }*/
        pubLog_Debug(_FFL_, service_name, "", "bbbbbbsIn_Service_type[%d][%s]",iPhone,sIn_Service_type);

        /* Ĭ���ط� */
          /* ��ѯ  IMSI���� */
        EXEC SQL select substr(trim(imsi_no),1,15) into :sV_ImsiNo
                 from   dSimRes
                 where  sim_no = :sV_Sim_no_List[iPhone];
        if (SQLCODE != 0)
        {
            strcpy(oRetCode,"111415");
            strcpy(oRetMsg, "��ѯIMSI��ʧ��!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        Trim(sV_ImsiNo);
    
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_ImsiNo      =[%s]",      sV_ImsiNo);
        #endif
    
        /* ���ͻ���Ϣ������ֵ */
        funcEnv.transIN=transIN;
        funcEnv.transOUT=transOUT; 
        funcEnv.temp_buf=temp_buf;
        funcEnv.totalDate=sV_Total_date;
        funcEnv.opTime=sV_Sys_date;
        funcEnv.opCode=sIn_Op_Code;
        funcEnv.loginNo=sIn_Login_no;
        funcEnv.orgCode=sIn_Org_code;
        funcEnv.loginAccept=sV_Login_accept;
        funcEnv.idNo=sV_User_Id;
        funcEnv.phoneNo=sV_Phone_no_List[iPhone];
        funcEnv.smCode=sIn_Sm_code;
        funcEnv.cmdRight="1";
        funcEnv.simNo=sV_Sim_no_List[iPhone];
        funcEnv.belongCode=sIn_Belong_Code;
        funcEnv.offonAccept = ""; /* not use  */
        funcEnv.imsiNo=sV_ImsiNo;
        funcEnv.hlrCode=sV_hlrCode;
        
        ret =-1;
		ret =NewpubOpenAcctSendCmd(&funcEnv,'1',sIn_Op_Code,newfuncList);
		if(ret != 0)
	    {
	        pubLog_Debug(_FFL_, service_name, "", "�û��������ػ��������!");
	        strcpy(oRetCode,"110411");
	        pubLog_Debug(_FFL_, service_name, "", " ret = [%d] ",ret);
	        strcpy(oRetMsg, "�û��������ػ��������!");
	        PUBLOG_DBDEBUG(service_name);
	        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	        RAISE_ERROR(oRetCode,oRetMsg);
	    }
		
		ret =-1;
		V_LimitFee=0;
		init(sv_funccode);
		ret =NewfAddUsedFuncs(&funcEnv,newfuncStr,fV_Prepay_fee_List[iPhone],&V_LimitFee,sv_funccode);
		if(ret != 0)
	    {
	        if(ret==121958)
			{
				sprintf(oRetMsg, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sv_funccode,V_LimitFee);
			}
			else
			{
				strcpy(oRetMsg, "����������Ч���ط�����!");
			}
	        
	        pubLog_Debug(_FFL_, service_name, "", "����������Ч���ط�����!");
	        strcpy(oRetCode,"110411");
	        pubLog_Debug(_FFL_, service_name, "", " ret = [%d] ",ret);
	        PUBLOG_DBDEBUG(service_name);
	        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	        RAISE_ERROR(oRetCode,oRetMsg);
	    }

        ret = pubOpenFunc(transIN , transOUT , temp_buf ,sV_Total_date ,
            sV_Sys_date , sIn_Op_Code , sIn_Login_no , sIn_Org_code ,
            sV_Login_accept , sV_User_Id , sV_Phone_no_List[iPhone] , sIn_Sm_code ,
            sIn_Belong_Code , "1" ,
            oldfuncList , pUnUsedFunc_List , pAddAddFunc_List ,
            "1" , sV_Sim_no_List[iPhone] ,fV_Prepay_fee_List[iPhone],
            oRetMsg);
        if(ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "�û������ط���Ϣ����,ret=[%d]!" , ret);
            strcpy(oRetCode,"111411");
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        
        /* �ʷѴ��� */
        
        for(i=0;(i<iV_BillModeNum);i++)
        {
			pubLog_Debug(_FFL_, service_name, "", "lixg: sV_Phone_no_List[%d] = [%s]", iPhone, sV_Phone_no_List[iPhone]);
            ret = -1;
    	    if(strlen(sV_BillMode[i])==0)
    	        continue;
			if (i==0)
			{
				memset(vModeCode1, '\0', sizeof(vModeCode1));
				strcpy(vModeCode1,sV_BillMode[i]);
				memset(sV_Phone_no_1, '\0', sizeof(sV_Phone_no_1));
				strcpy(sV_Phone_no_1,sV_Phone_no_List[iPhone]);
				init(pId_No);
				exec sql select to_char(id_no) into :pId_No from dCustMsg where phone_no=:sV_Phone_no_1 and substr(run_code,2,1)<'a';
				if(SQLCODE!=OK)
				{
					pubLog_Debug(_FFL_, service_name, "", "��ѯidno ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
					EXEC SQL ROLLBACK;
					PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode,oRetMsg);
				}
				sprintf(TableName,"dinnetmodecode%ld",atol(pId_No)%10);
				sprintf(dynStmt,"insert into %s (ID_NO,MODE_CODE,TOTAL_DATE) values(TO_NUMBER(:vi),:v2,TO_NUMBER(:v3))",TableName);
				EXEC SQL PREPARE sql_stmt FROM :dynStmt;
				EXEC SQL EXECUTE sql_stmt using :pId_No,:vModeCode1,:sV_Total_date;
				if (SQLCODE !=OK)
				{
					pubLog_Debug(_FFL_, service_name, "", "dinnetmodecode ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
					EXEC SQL ROLLBACK;
					PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode,oRetMsg);
				}
			      /**insert dAllUser*/  
			      /*������ edit by cencm at 20100414--------begin-------------*/
			      /* exec sql insert into dAllUser(
			       msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
			       start_time,end_time,region_code,id_no,group_id,product_code)
			   values(
			       :sV_Phone_no_List[iPhone],:sIn_Sm_code,'user','user',0,'1',sysdate,
			       to_date('20500101 00:00:00','YYYYMMDD HH24:MI:SS'),substr(:sIn_Belong_Code,1,2),
			       	:pId_No,'null',:vModeCode1);
			  				if(SQLCODE!=OK)
							{
								pubLog_Debug(_FFL_, service_name, "", "����dAllUser���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
								EXEC SQL ROLLBACK;
								PUBLOG_DBDEBUG(service_name);
								pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
								RAISE_ERROR(oRetCode,oRetMsg);
							}     	
							*/
							/*������ edit by cencm at 20100414--------end-------------*/
			}
    	    if(strncmp(sV_BillMode[i],"gn24",4)==0 || strncmp(sV_BillMode[i],"gn1m",4)==0) /* ������� */
    	    {
    	    	/*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H begin*/
	    		if(strcmp(vaccflag,"Y")==0)
				{
					pubLog_Debug(_FFL_, service_name, "", "��ǰʱ��DD[%s]�ɹ���",vnowdate);
					if(strcmp(vcycleDay,vnowdate)==0)
					{
						ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    sV_Sys_date,
												sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
												sV_Sys_date,     "0",        "", "",
												vErrMsg);
					}
					else if(strcmp(vcycleDay,vnowdate)<0) /*����������29---31�ż�� ������� �¸�������Ч*/
					{
						/*ȡ�û��¸����ڿ�ʼʱ��*/
						/*�����ڸ���   ��ȡ��һ���� ��ʼ������ʱ�� S.H  2012/6/11 15:54:52 begin */
						vRetout=0;
						vRetout=pubGetCustAcctCyCle(sV_User_Id,sV_Sys_date,1,0,vCustAcctCycleBeginTime,vCustAcctCycleEndTime,vErrMsg );
						if(vRetout != 0)
						{
							strcpy(oRetCode,"110473");
							sprintf(oRetMsg, "��ȡ����ʱ��ʧ��,vRetout=[%d] vErrMsg=[%s]\n",vRetout,vErrMsg);
							PUBLOG_DBDEBUG(service_name);
							pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
							RAISE_ERROR(oRetCode,oRetMsg);
							
						}
						pubLog_Debug(_FFL_, service_name, "", "��һ�����ڿ�ʼʱ��[%s]�ɹ���",vCustAcctCycleBeginTime);
						pubLog_Debug(_FFL_, service_name, "", "��һ�����ڽ���ʱ��[%s]�ɹ���",vCustAcctCycleEndTime);
						
						ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    vCustAcctCycleBeginTime,
												sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
												sV_Sys_date,     "1",        "", "",
												vErrMsg);
					}
	    		}
	    		else
	    		{
	                ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    iEffectTime,
	                                       sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
	                                       sV_Sys_date,     "1",        "", "",
	                                       vErrMsg);
	                                       
	               /*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H end*/
                }
    	    }
    	    else
    	    {
                ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    sV_Sys_date,
                                       sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
                                       sV_Sys_date,     "0",        "", "",
                                       vErrMsg);
    	    }
            if(ret!=0){
                sprintf(oRetCode,"%d",ret);            
                sprintf(oRetMsg,"�����ʷ�[%s]����%s",sV_BillMode[i],vErrMsg);
                PUBLOG_DBDEBUG(service_name);
                pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                RAISE_ERROR(oRetCode,oRetMsg);
            }    	                       
        }        
		    /* ����û��ʷ���Ϣ�Ƿ����� */
    	ret = pubCheckBillMsg(sV_User_Id);
    	if(ret != 0)
    	{
        	pubLog_Debug(_FFL_, service_name, "", "�û��ʷ���Ϣû������,ret=[%d]!" , ret);
        	strcpy(oRetCode,"111410");
        	sprintf(oRetMsg, "�û�[%s]�ʷ���Ϣû������,�����²���!",sV_Phone_no_List[iPhone]);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
    	}
    	
    	
	    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/
	    /*get org_Code Begin*/
	        EXEC SQL SELECT Org_Code  INTO :vOrg_Code
	             FROM   dLoginMsg 
	             WHERE  Login_No = :sIn_Login_no;
	         if(SQLCODE!=0){
	             strcpy(oRetCode,"119402");
	             strcpy(oRetMsg, "ȡ������Ϣ����!");    
	             PUBLOG_DBDEBUG(service_name);
	             pubLog_DBErr(_FFL_, service_name, " ", "%s,%s", oRetCode,oRetMsg);
	             RAISE_ERROR(oRetCode,oRetMsg);
	         }
	    /*get org_Code end*/         
	    
	    EXEC SQL SELECT DECODE(SUBSTR(:sIn_Belong_Code,1,2), '99',SUBSTR(:vOrg_Code,1,2),SUBSTR(:sIn_Belong_Code,1,2))
	           INTO :mRegion_Code
	           From dual;
	    if (SQLCODE != 0) {
	    	strcpy(oRetCode,"119403");
	    	strcpy(oRetMsg,"��ѯRegion_code����");
	        PUBLOG_DBDEBUG("s1114Cfm");
			pubLog_DBErr(_FFL_,"s1114Cfm","-2","��ѯRegion_code����");                         
	        RAISE_ERROR(oRetCode,oRetMsg);
	    }
	        
	    EXEC SQL select to_char(count(*)) into :pPayFlag from sPackCode where region_code=:mRegion_Code and mach_code=:sV_Machine_Code;
	    if(atoi(pPayFlag)>0) /* ��Ӫ����Ԥ�� */
	        strcpy(pPayFlag,"j");
	    else if(strcmp(sIn_Sm_code,"d0")==0) /* ���ݿ� */
	        strcpy(pPayFlag,"k");
	    else
	        strcpy(pPayFlag,"0");
	    
	    iGoodNoCount=0;
		EXEC SQL select count(*)
				into :iGoodNoCount
				from dGoodPhoneRes
				where phone_no=:sV_Phone_no_List[iPhone]
				and good_type in (select good_type from sgoodnotype);
		if(SQLCODE != 0) 
		{
			strcpy(oRetCode,"119404");
	    	strcpy(oRetMsg,"��ѯdGoodPhoneRes����");
	        PUBLOG_DBDEBUG("s1104Cfm");
		    pubLog_DBErr(_FFL_,"s1104Cfm","-39","��ѯdGoodPhoneRes����");
			EXEC SQL ROLLBACK;
			RAISE_ERROR(oRetCode,oRetMsg);
	    }
	    if(iGoodNoCount>0)
	    {
	    	init(pPayFlag);
	    	strcpy(pPayFlag,"J");
	    }
	    /*payType��ֵ*/
	    if(fV_Prepay_fee_List[iPhone]>0)
	    {
	        if(iGoodNoCount>0)
	        {
	            init(sV_Pay_Type);
	    	    strcpy(sV_Pay_Type,"J");
	        }    
	    }   
	    /*֧Ʊ���� paytype=1*/
	    if(fV_Check_Pay>0)
        {
            init(sV_Pay_Type);
            strcpy(sV_Pay_Type,"1");
        }	         
	    	   
	    memset(vParamList,0,DLMAXITEMS*2*DLINTERFACEDATA);
	    init(vCallServiceName);
	    strcpy(vCallServiceName,"bodb_sPDataCfm");
		vSendParaNum=18; /* ҵ�񹤵���������������� */
		vRecpParaNum=2;/* ҵ�񹤵���������������� */
		
		init(cV_Prepay_fee_List);
		sprintf(cV_Prepay_fee_List,"%f",fV_Prepay_fee_List[iPhone]);
		
		strcpy(vParamList[0][0],"pId_No");
		strcpy(vParamList[0][1],sV_User_Id);	
	    
		strcpy(vParamList[1][0],"pCon_Id");
		strcpy(vParamList[1][1],sV_Acct_Id);	
	    		
		strcpy(vParamList[2][0],"vPrepay_Fee");
		strcpy(vParamList[2][1],cV_Prepay_fee_List);	
		
		strcpy(vParamList[3][0],"pSysDate");
		strcpy(vParamList[3][1],sV_Sys_date);		
		
		strcpy(vParamList[4][0],"pPayFlag");
		strcpy(vParamList[4][1],pPayFlag);
					
		
		strcpy(vParamList[5][0],"pPay_Type");
		strcpy(vParamList[5][1],sV_Pay_Type);
			
		
		strcpy(vParamList[6][0],"pTotal_Date"); 
		strcpy(vParamList[6][1],sV_Total_date);
				
		
		strcpy(vParamList[7][0],"pLogin_Accept"); 
		strcpy(vParamList[7][1],sV_Login_accept);		
		
		strcpy(vParamList[8][0],"pCust_Id"); 
		strcpy(vParamList[8][1],sIn_Cust_Id);
		
		strcpy(vParamList[9][0],"pPhone_No");
		strcpy(vParamList[9][1],sV_Phone_no_List[iPhone]);						
		
		strcpy(vParamList[10][0],"pBelong_Code");
		strcpy(vParamList[10][1],sIn_Belong_Code);
		
		strcpy(vParamList[11][0],"pSm_Code");
		strcpy(vParamList[11][1],sIn_Sm_code);
		
		strcpy(vParamList[12][0],"pLogin_No");
		strcpy(vParamList[12][1],sIn_Login_no);
		
		strcpy(vParamList[13][0],"pOrg_Code");
		strcpy(vParamList[13][1],vOrg_Code);
		
		strcpy(vParamList[14][0],"pOp_Code"); 
		strcpy(vParamList[14][1],sIn_Op_Code);
		
		strcpy(vParamList[15][0],"pOp_Note"); 
		strcpy(vParamList[15][1],sIn_Op_Note);
		
		strcpy(vParamList[16][0],"vOrgId"); 
		strcpy(vParamList[16][1],vOrgId);
		
		strcpy(vParamList[17][0],"pGroupNo");
		strcpy(vParamList[17][1],pGroupNo);
				
		v_ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
				                          ORDERIDTYPE_USER,sV_User_Id,100,sV_Login_accept,sIn_Op_Code,sIn_Login_no,sIn_Op_Note);			
		if (v_ret != 0) 
		{
		    strcpy(oRetCode, "295338");
		    sprintf(oRetMsg,"��ʼ���˱�,�ɷ���ˮ��ʧ�� ret=[%d]",v_ret);
			PUBLOG_DBDEBUG("s1114Cfm");
			pubLog_DBErr(_FFL_, "s1114Cfm", "%s", "%s", oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}	    
	    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/          	
	    /***************add for ��𾻶���� 2012.6.18*********************/
	    Trim(vIsOutright);
	    if( vIsNetAmount == 0 && strcmp(vIsOutright,"Y") == 0)
	    {
	        EXEC SQL insert into wnetamountdetail 
	                             (LOGIN_ACCEPT, GROUP_ID, RES_CODE, 
	                             OP_CODE, LOGIN_NO, OP_TIME, 
	                             BEGIN_NO, END_NO, SM_CODE, 
	                             SUM_NUM, SHOULD_FEE, REAL_FEE, 
	                             REWARD_FEE, TAX_FEE, BUSI_CODE ) 
	                       values(:sIn_Login_Accept, :pGroupNo, :sIn_Sim_Type,  
	                              :sIn_Op_Code, :sIn_Login_no,to_date(:sV_Sys_date,'YYYYMMDD HH24:MI:SS'),  
	                              :sV_Phone_no_List[iPhone], :sV_Phone_no_List[iPhone], :sIn_Sm_code,  
	                              '1', :fV_Sum_Prepay_Fee/:iV_PhoneCount, :fV_Sum_Cash_Pay/:iV_PhoneCount,  
	                              :vReward_Fee/:iV_PhoneCount, :vTax_Fee/:iV_PhoneCount, '04');
	        if(SQLCODE!=0)
	        {
	            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetail�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
	            strcpy(oRetCode,"110407");
	            strcpy(oRetMsg,"����wnetamountdetail�����!");
	            PUBLOG_DBDEBUG(service_name);
	            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	            RAISE_ERROR(oRetCode,oRetMsg);
	        }
	        
	        EXEC SQL insert into wnetamountdetailhis
	                             (LOGIN_ACCEPT, GROUP_ID, RES_CODE, 
	                             OP_CODE, LOGIN_NO, OP_TIME, 
	                             BEGIN_NO, END_NO, SM_CODE, 
	                             SUM_NUM, SHOULD_FEE, REAL_FEE, 
	                             REWARD_FEE, TAX_FEE, BUSI_CODE ,
	                             UPDATE_OP_CODE, UPDATE_LOGIN, UPDATE_NOTE, 
	                             UPDATE_TIME, UPDATE_ACCEPT, UPDATE_TYPE) 
	                       values(:sIn_Login_Accept, :pGroupNo, :sIn_Sim_Type,  
	                              :sIn_Op_Code, :sIn_Login_no,to_date(:sV_Sys_date,'YYYYMMDD HH24:MI:SS'),  
	                              :sV_Phone_no_List[iPhone], :sV_Phone_no_List[iPhone], :sIn_Sm_code,  
	                              '1', :fV_Sum_Prepay_Fee/:iV_PhoneCount, :fV_Sum_Cash_Pay/:iV_PhoneCount,  
	                              :vReward_Fee/:iV_PhoneCount, :vTax_Fee/:iV_PhoneCount, '04',
	                              :sIn_Op_Code,:sIn_Login_no,'ȫ��ͨ��������',
	                              sysdate,:sIn_Login_Accept,'a');
	        if(SQLCODE!=0)
	        {
	            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetailhis�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
	            strcpy(oRetCode,"110408");
	            strcpy(oRetMsg,"����wnetamountdetailhis�����!");
	            PUBLOG_DBDEBUG(service_name);
	            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	            RAISE_ERROR(oRetCode,oRetMsg);
	        }
	    }
	    /***************add for ��𾻶���� 2012.6.18*********************/
			/***************add for ��������֤ǰ̨����ķǾ������Ƿ���ȷ 2012.10.29 liujj begin*******/
			else
			{
				iRet = 0;
				iRet = pubIsNetAmount(sV_Phone_no_List[iPhone],sIn_Belong_Code,sIn_Sm_code,fV_Sum_Prepay_Fee/iV_PhoneCount);
				if(iRet == 0)
				{
					pubLog_Debug(_FFL_, service_name, "", "ҳ��������жϾ�Ϊ�Ǿ���[%s]!",sV_Phone_no_List[iPhone]);
				}
				else if(iRet == 1)
				{
					pubLog_Debug(_FFL_, service_name, "", "�����жϺ���[%s]������Ҫ����!",sV_Phone_no_List[iPhone]);
					pubLog_Debug(_FFL_, service_name, "", "ҳ�洫�����:[%d][%s]!",vIsNetAmount,vIsOutright);
					strcpy(oRetCode,"110412");
					strcpy(oRetMsg,"�����ж�Ϊ����,��ҳ�洫���������,�����²���!");
					PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode,oRetMsg);
				}
				else
				{
					strcpy(oRetCode,"110413");
					strcpy(oRetMsg,"�ж��Ƿ񾻶�ʧ��!");
					PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode,oRetMsg);
				}
			}
			/***************add for ��������֤ǰ̨����ķǾ������Ƿ���ȷ 2012.10.29 liujj end*******/
    	
    }  /* end -- for ((i=0;i<iV_PhoneCount;i++)  */
    
    /* ��¼֧Ʊ��Ϣ */  
    if(fV_Sum_Check_Pay>0)
    {
        ret=-1;
        ret=pubOpenAcctCheck_1(sIn_Check_No,   fV_Sum_Check_Pay,   sIn_Bank_Code,
                           sIn_Op_Note,      sIn_Login_no,       sV_Login_accept, 
                           sIn_Op_Code,      sV_Total_date,      sV_Sys_date,
                           sIn_System_Note,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�޸�֧Ʊ��Ϣ����,ret=[%d]",ret);
            strcpy(oRetCode,"110409");
            strcpy(oRetMsg, "�޸�֧Ʊ��Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }      
    }
    
    /*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 begin*/   
    memset(vParamList,0,DLMAXITEMS*2*DLINTERFACEDATA);
    init(vCallServiceName);
    strcpy(vCallServiceName,"bodb_sCheckCfm");
	vSendParaNum=4; /* ҵ�񹤵���������������� */
	vRecpParaNum=2;/* ҵ�񹤵���������������� */

    init(cV_Sum_Check_Pay);
	sprintf(cV_Sum_Check_Pay,"%f",fV_Sum_Check_Pay);	
	strcpy(vParamList[0][0],"pSysDate");
	strcpy(vParamList[0][1],sV_Sys_date);
	
	strcpy(vParamList[1][0],"pBank_Code"); 
	strcpy(vParamList[1][1],sIn_Bank_Code);
	
	strcpy(vParamList[2][0],"pCheck_No"); 
	strcpy(vParamList[2][1],sIn_Check_No);
	
	strcpy(vParamList[3][0],"vCheck_Pay"); 
	strcpy(vParamList[3][1],cV_Sum_Check_Pay);
	
	v_ret = 0;
	v_ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
			                          ORDERIDTYPE_USER,sV_User_Id,100,sV_Login_accept,sIn_Op_Code,sIn_Login_no,sIn_Op_Note);			
	if (v_ret != 0) 
	{
		strcpy(oRetCode, "295338");
		sprintf(oRetMsg,"����֧Ʊ��Ϣʧ�� ret=[%d]",v_ret);
		PUBLOG_DBDEBUG("s1114Cfm");
		pubLog_DBErr(_FFL_, "s1114Cfm", "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	
	/*NG ����  ҵ�񹤵����� Add by miaoyl 20091001 end*/      
	
	
	if (strcmp(sIn_PayFlag,"1")==0)
	{
	    fV_Sum_Cash_Pay = fV_Sum_Check_Pay;	
	}
	
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, service_name, "", "fV_Sum_Cash_Pay     = [%f]",fV_Sum_Cash_Pay);
		pubLog_Debug(_FFL_, service_name, "", "begin insert wLoginOprYYYYMM ");
	#endif
	


	/* ���� wLoginOprYYYYMM �� */
	/* 
	    ���������� ���ܵ�����¼�ֻ������Id_no , ����ʹ��phone_no �ֶ�����¼ �� 
	    ��һ �� ������������־ �� Id_no �� 0
	
	**init(vTmp_List);
	**sprintf(vTmp_List,"insert into wLoginOpr%s(total_date,login_accept,op_code,pay_type,pay_money,sm_code,id_no,"
	**        " phone_no,org_code,login_no,op_time,op_note,IP_ADDR) "
	**    	" values(to_number(:sV_Total_date),to_number(:sV_Login_accept),:sIn_Op_Code,:sIn_PayFlag,%f,:sIn_Sm_code,0,"
	**        " :sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,to_date(:sV_Sys_date,'yyyymmdd hh24:mi:ss'),:sIn_System_Note,"
	**        " :sIn_Ip_Address)",sV_YYYYMM,fV_Sum_Cash_Pay);
	**    
	**pubLog_Debug(_FFL_, service_name, "", "insert wLoginOpr%s' sql_str = [%s]",sV_YYYYMM,vTmp_List);
	**EXEC SQL PREPARE ins_stmt From :vTmp_List; 
	**EXEC SQL EXECUTE ins_stmt using :sV_Total_date,:sV_Login_accept,:sIn_Op_Code,:sIn_PayFlag,:sIn_Sm_code,
	**                                :sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,:sV_Sys_date,:sIn_System_Note,
	**                                :sIn_Ip_Address; 
	**
	**if(SQLCODE != 0)
	**{
	**    strcpy(oRetCode,"111413");
	**    strcpy(sErrorMess, "����wLoginOpr�����!");
	**    PUBLOG_DBDEBUG(service_name);
	**    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
	**    RAISE_ERROR(oRetCode,oRetMsg);
	**}
	*/
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/
	vLog.vId_No                            =  0;
	vLog.vTotal_Date                       =   atol(sV_Total_date);
	vLog.vLogin_Accept                     =  atol(sV_Login_accept);

	strncpy(vLog.vSm_Code,       sIn_Sm_code,2);
	strncpy(vLog.vBelong_Code,   sIn_Belong_Code,7);
	strncpy(vLog.vPhone_No,      sIn_CustDocFlag,15);
	strncpy(vLog.vOrg_Code,      sIn_Org_code,9);
	strncpy(vLog.vLogin_No,      sIn_Login_no,6);
	strncpy(vLog.vOp_Code,       sIn_Op_Code,4);
	strncpy(vLog.vOp_Time,       sV_Sys_date,17);
	strncpy(vLog.vMachine_Code,  "0", 3);
	strncpy(vLog.vBack_Flag,     "0", 1);
	strncpy(vLog.vEncrypt_Fee,   "0", 16);
	strncpy(vLog.vSystem_Note,   sIn_System_Note,60 );
	strncpy(vLog.vOp_Note,       sIn_Op_Note,60 );
	strncpy(vLog.vGroup_Id,      pGroupNo,10);
	strncpy(vLog.vOrg_Id,        vOrgId,10);
	Trim(sIn_PayFlag);
	strcpy(vLog.vPay_Type,      sIn_PayFlag);/*�������*/
	strncpy(vLog.vIp_Addr,       sIn_Ip_Address,15);
	vLog.vPay_Money              = fV_Sum_Cash_Pay;
	vLog.vCash_Pay               = 0;
	vLog.vCheck_Pay          	  = 0;
	vLog.vSim_Fee            	  = 0;
	vLog.vMachine_Fee        	  = 0;
	vLog.vInnet_Fee          	  = 0;
	vLog.vChoice_Fee         	  = 0;
	vLog.vOther_Fee           	  = vTax_Fee - vReward_Fee;
	vLog.vHand_Fee           	  = 0;
	vLog.vDeposit            	  = 0;
	
	Vret = 0;
	strcpy(vChgFlag,"N");
	strcpy(vCustFlag,"N");
	
	pubLog_Debug(_FFL_, service_name, "", "========%ld====",vLog.vId_No); 
	
	Vret = recordOprLog(&vLog,oRetMsg,vChgFlag,vCustFlag);
	if(Vret != 0)
		{
		sprintf(oRetCode,"%06d",Vret);
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}

SERVICE_FINALLY_DECLARE();
	PUBLOG_DBDEBUG(service_name);
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg, oRetCode);
	    /* ѹ�뷵����Ϣ  */
	pubLog_Debug(_FFL_, service_name, "", "oRetCode �� [%s],oRetMsg=[%s]",oRetCode,oRetMsg);
	pubLog_Debug(_FFL_, service_name, "", "SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);

	if(!strcmp(oRetCode,"000000"))
		EXEC SQL commit;
	else
		exec sql rollback;

	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	transOUT = Add_Ret_Value(GPARM32_2,sV_Login_accept);
	time(&t_end);
	sprintf(strTime2,"%ld",getSysTime(112));
	
	/*��ȡ��������ʱ��*/
	getTimeDif(&vUsedTime, vStartSec);
	
	/*Ϊ������־�ṹ�帳ֵ*/
	strcpy(logShip.vBegin_Time, vStartTime);
	logShip.vUsed_Time = vUsedTime;
	logShip.vLogion_Accept = atol(sV_Login_accept);
	strcpy(logShip.vLogin_No, sIn_Login_no);
	strcpy(logShip.vOp_Code, sIn_Op_Code);
	strcpy(logShip.vServ_Name, service_name);
	strcpy(logShip.vPhone_No, sIn_CustDocFlag);
	logShip.vId_No = atol(pId_No);
	logShip.vOther_No = atol(sIn_Cust_Id);
	strcpy(logShip.vRetCode, oRetCode);
	strcpy(logShip.vRetMsg, oRetMsg); 
	/*����������־��־λ*/
	SetBomcLogFlag(&logShip);
	/*��¼������־*/
	WriteBomcLog(&logShip);
TUX_SERVICE_END();
}

/***************************************
 ��������  s1118Cfm
 ��������  2003/10/09
 ������Ա  Yuanby
 ���ù��̣�
 ����������������������������
 ��������� 
	��������                                   
        ӪҵԱ��½IP                               
        ��������                                   
        ϵͳ��ע                                   
        ������ע                                   
        SIM������                                  
        �����������                               
        ��������                                   
        �ͻ����ϱ�־   0 ��һ��Ϣ   1  ������Ϣ   
        �ͻ�Id                                    
        �ͻ�����                                  
        �ͻ�����                                  
        �ͻ���ַ                                  
        �ͻ�֤������                              
        �ͻ�֤������                              
        ҵ������                                  
        ���ѷ�ʽ   0  �ֽ�  1  ֧Ʊ               
        SIM����                                    
        �ܷ�����Ϣ��                               
        �ֻ����봮                                 
        SIM�����봮                                
        ѡ�ŷѴ�                                   
        �Ż���Ϣ��                                 
        �û��ʻ�����      
        ������ˮ                         
 
	        
 ���������     
      �������  
      ������Ϣ
      ������ˮ  
                
                
�޸ļ�¼:       
		�޸���Ա��
                �޸����ڣ�
		�޸�ǰ�汾�ţ�
		�޸ĺ�汾��:
		�޸�ԭ��
                                  
****************************************/
_TUX_SVC_ s1118Cfm( TPSVCINFO *transb )
{
	char	          temp_buf[61];        /*��ʱʹ�õ��ַ�������*/
	char            service_name[20];
/*add by liupenga ��֯��������20090305*/    
	int Vret = 0;
	tLoginOpr vLog; 
	
	/*---��֯������������޸Ĳ��� add by liuhao at 19/05/2009---*/
	char		  vStartTime[20+1];			/* ����ʼʱ�� YYYYMMDDHHMISS*/
	double		vUsedTime=0.00;				/* ��������ʱ�� ��λ����*/
	double		vStartSec=0.00;				/* ����ʼʱ�� ��λ����*/
	BOMC_LOG 	logShip;					/*������־��¼�ṹ���������*/
	EXEC SQL BEGIN DECLARE SECTION;
	char    sTemp[8+1];                           /*����������ʵ����ʶ*/
	char    sIn_Login_no[6+1];                    /* ��������        */
	char    sIn_Ip_Address[15+1];                 /* ӪҵԱ��½IP    */
	char    sIn_Op_Code[4+1];                     /* ��������        */
	char    sIn_System_Note[60+1];                /* ϵͳ��ע        */        
	char    sIn_Op_Note[60+1];                    /* ������ע        */
	char    sIn_Sim_Type[5+1];                    /* SIM������       */     
	char    sIn_Belong_Code[7+1];                 /* �����������    */   
	char    sIn_Org_code[9+1];                    /* ��������        */
	char    sIn_CustDocFlag[2+1];                 /* �ͻ����ϱ�־   0 ��һ��Ϣ   1  ������Ϣ  */
	char    sIn_Cust_Id[22+1];                    /* �ͻ�Id          */
	char    sIn_Cust_Passwd[6+1];                 /* �ͻ�����        */ 
	char    sIn_Cust_Name[60+1];                  /* �ͻ�����        */
	char    sIn_Cust_Address[60+1];               /* �ͻ���ַ        */
	char    sIn_CustId_Type[2+1];                 /* �ͻ�֤������    */
	char    sIn_Id_Iccid[20+1];                   /* �ͻ�֤������    */     
	char    sIn_Sm_code[2+1];                     /* ҵ������        */
	char    sIn_PayFlag[2+1];                     /* ���ѷ�ʽ   0  �ֽ�  1  ֧Ʊ  */
	float   fIn_Sim_fee = 0.00;                   /* SIM����         */
	char    sIn_Fee_List[500];                    /* �ܷ�����Ϣ��    */
	char    sIn_Phone_no_List[2048];              /* �ֻ����봮      */
	char    sIn_Sim_no_List[2048];                /* SIM�����봮     */
	char    sIn_Choice_fee_List[2048];            /* ѡ�ŷѴ�        */
	char    sIn_Favour_List[2048];                /* �Ż���Ϣ��      */
	char    sIn_Password[8+1];                    /* �û��ʻ�����    */
	char    sIn_Login_Accept[22+1];               /* ������ˮ        */
	char    sIn_Check_No[20+1];                   /* ֧Ʊ����        */
	char    sIn_Bank_Code[5+1];                   /* ���д���        */
	
	char    sV_Login_accept[22+1] ;               /* ������ˮ        */
	char    sV_Offon_accept[22+1];                /* ���ػ���ˮ      */    
	char    sV_Total_date[20+1];                  /* ��������        */
	char    sV_Sys_date[20+1];                    /* ϵͳ����        */
	char    sV_Region_code[2+1];                  /* ��������        */
	char    sV_YYYYMM[6+1];                       /* ����            */
	char    sV_User_Id[22+1];                     /* �û�ID          */
	
	int     iV_PhoneCount = 0 ;                   /* �ֻ���������    */
	int     iV_Count = 0;                         /* ����������      */
	char    sV_Phone_no_List[100][16];            /* �ֻ����봮      */
	char    sV_Sim_no_List[100][21];              /* SIM�����봮     */
	float   fV_Choice_fee_List[100];              /* ѡ�ŷѴ�        */  
	char    sV_Status[100][30+1];                 /* ����״̬�������ɹ��򿪻�ʧ����Ϣ */
	
	int     iV_FavNum=0;                          /* ӪҵԱ�Ż�����  */
	char    sV_Favour_Code[100][4+1];             /* �Żݴ���        */
	char    sV_Should_Data[100][17+1];            /* �Ż�ǰ����      */
	char    sV_Actual_Data[100][17+1];            /* �Żݺ�����      */
	
	float   fV_Cash_Pay = 0.00 ;                  /* �ֽ𽻿�        */ 
	float   fV_Check_Pay = 0.00 ;                 /* ֧Ʊ����        */ 
	float   fV_Sum_Cash_Pay = 0.00 ;              /* ���ֽ𽻿�      */   
	float   fV_Sum_Check_Pay = 0.00 ;             /* ��֧Ʊ����      */
	float   fV_Sum_Sim_Fee = 0.00 ;               /* ��SIM����       */
	float   fV_Sum_Choice_Fee = 0.00 ;            /* ��ѡ�ŷ�        */
	float   fV_SumHand_Fee = 0.00 ;               /* ������          */
	
	char    vTmp_List[2048];
	char    vTmp_List1[2048];
	int     tmplen=0,begin=1,end=1,num=0,i=0,k=1;
	int     tmplen1=0,begin1=1,end1=1,num1=0,i1=0,k1=1;
	
	char    sTmp_Str[30];                         /* ��ʱ�ַ���      */ 
	int     ret =0;                               /* ��������ֵ      */
	int     smap_ret=0;
	
	/* ϵͳĬ������ */
	float   fV_Machine_fee = 0.00;                /* ������          */
	float   fV_Hand_fee = 0.00;                   /* ������          */
	
	char    sV_Attr_code[8+1];                    /* ���Դ���        */
	char    sV_ImsiNo[15+1];
	char    sV_hlrCode[1+1];
	
	

/*
        sIn_Fee_List = �ֽ𽻿� ~ ֧Ʊ���� ~ SIM����  ~ ѡ�ŷ� ~ ������
        sIn_Favour_List = �Żݴ��� & �Ż�ǰ���� & �Żݺ����� ~ �Żݴ��� & �Ż�ǰ���� & �Żݺ����� ~
*/      
		char sTrue_flag[1+1];  
		/*add by liupenga ��֯��������20090305*/
	char vOrgId[10+1];
	char vGroupId[10+1];    
	char vChgFlag[1+1];
	char vCustFlag[1+1];        
	char vPassWord[16+1];
	long iLoginAccept=0;
	char inResv1[5+1];
	EXEC SQL END DECLARE SECTION;
	
	tFuncEnv funcEnv;
	
	init(vStartTime);
	memset(&logShip, 0, sizeof(logShip));
	/*��ȡ����ʼʱ��*/
	getBeginTime(&vStartTime,&vStartSec);
	
	/* ���Ի����� */
	init(sIn_Login_no); 
	init(sIn_Ip_Address);
	init(sIn_Op_Code);
	init(sIn_System_Note);
	init(sIn_Op_Note); 
	init(sIn_Sim_Type);
	init(sIn_Belong_Code);
	init(sIn_Org_code);
	init(sIn_CustDocFlag); 
	init(sIn_Cust_Id);
	init(sIn_Cust_Passwd);
	init(sIn_Cust_Name);
	init(sIn_Cust_Address); 
	init(sIn_CustId_Type);
	init(sIn_Id_Iccid);
	init(sIn_Sm_code);
	init(sIn_PayFlag);
	init(sIn_Fee_List);
	init(sIn_Phone_no_List);
	init(sIn_Sim_no_List);
	init(sIn_Choice_fee_List);
	init(sIn_Favour_List);
	init(sIn_Password);
	init(sIn_Login_Accept);
	init(sIn_Check_No);
	init(sIn_Bank_Code);
	
	init(sV_Login_accept);
	init(sV_Offon_accept);
	init(sV_Total_date);
	init(sV_Sys_date);
	init(sV_Region_code);
	init(sV_YYYYMM);
	init(sV_User_Id);
	init(sV_ImsiNo);
	init(sV_hlrCode);
	
	init(sTrue_flag);
	init(sTemp);
	init(vOrgId);
	init(vGroupId);
	init(vChgFlag);
	init(vCustFlag);
	init(vPassWord);
	memset(&vLog,0,sizeof(vLog));    

	for (i=0;i<100;i++)
	{
		init(sV_Favour_Code[i]);
		init(sV_Should_Data[i]);
		init(sV_Actual_Data[i]);
		init(sV_Phone_no_List[i]);
		init(sV_Sim_no_List[i]);
		init(sV_Status[i]);
		
		fV_Choice_fee_List[i] = 0.00;
	}
	
	strcpy(service_name,"s1118Cfm");
	init(inResv1);
	strcpy(inResv1,"ALL");
	Trim(inResv1);
	
	TUX_SERVICE_BEGIN(iLoginAccept,sIn_Op_Code,sIn_Login_no,sIn_Op_Note,ORDERIDTYPE_CUST,sIn_Cust_Id,inResv1,NULL);
	
	/* ��ʼ��Ĭ������ */
	strcpy(sV_Attr_code,"00000000");
	
	/* ȡ��������� */
	strcpy(sTemp,      input_parms[0]);
	strcpy(sIn_Ip_Address,    input_parms[1]);
	strcpy(sIn_Op_Code,       input_parms[2]);
	strcpy(sIn_System_Note,   input_parms[3]);
	strcpy(sIn_Op_Note,       input_parms[4]);
	strcpy(sIn_Sim_Type,      input_parms[5]);
	strcpy(sIn_Belong_Code,   input_parms[6]);
	strcpy(sIn_Org_code,      input_parms[7]);
	strcpy(sIn_CustDocFlag,   input_parms[8]);
	strcpy(sIn_Cust_Id,       input_parms[9]);
	strcpy(sIn_Cust_Passwd,   input_parms[10]);
	strcpy(sIn_Cust_Name,     input_parms[11]);
	strcpy(sIn_Cust_Address,  input_parms[12]);
	strcpy(sIn_CustId_Type,   input_parms[13]);
	strcpy(sIn_Id_Iccid,      input_parms[14]);
	strcpy(sIn_Sm_code,       input_parms[15]);
	strcpy(sIn_PayFlag,       input_parms[16]);
	fIn_Sim_fee = atof(       input_parms[17] );
	strcpy(sIn_Fee_List,      input_parms[18]);
	strcpy(sIn_Phone_no_List, input_parms[19]);
	strcpy(sIn_Sim_no_List,   input_parms[20]);
	strcpy(sIn_Choice_fee_List, input_parms[21]);
	strcpy(sIn_Favour_List,     input_parms[22]);
	strcpy(sIn_Password,        input_parms[23]);
	strcpy(sIn_Login_Accept,        input_parms[24]);
	strcpy(sIn_Check_No,        input_parms[25]);
	strcpy(sIn_Bank_Code,        input_parms[26]);  
	   
	strncpy(sIn_Login_no,sTemp,6);
	strncpy(sTrue_flag,sTemp+6,1);
	strncpy(vPassWord,input_parms[27],16); /*�����������*/
	
	pubLog_Debug(_FFL_, service_name, "", "input_parms[21] = [%s]" ,  input_parms[21]);	
	  
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, service_name, "", "sIn_Login_no = [%s]",sIn_Login_no);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Ip_Address = [%s]",sIn_Ip_Address);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Code = [%s]",sIn_Op_Code);
		pubLog_Debug(_FFL_, service_name, "", "sIn_System_Note = [%s]",sIn_System_Note);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Op_Note = [%s]",sIn_Op_Note);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Sim_Type = [%s]",sIn_Sim_Type);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Belong_Code = [%s]",sIn_Belong_Code);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Org_code = [%s]",sIn_Org_code);
		pubLog_Debug(_FFL_, service_name, "", "sIn_CustDocFlag = [%s]",sIn_CustDocFlag);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Id = [%s]",sIn_Cust_Id);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Passwd = [%s]",sIn_Cust_Passwd);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Name = [%s]",sIn_Cust_Name);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Address = [%s]",sIn_Cust_Address);
		pubLog_Debug(_FFL_, service_name, "", "sIn_CustId_Type = [%s]",sIn_CustId_Type);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Id_Iccid = [%s]",sIn_Id_Iccid);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Sm_code = [%s]",sIn_Sm_code);
		pubLog_Debug(_FFL_, service_name, "", "sIn_PayFlag = [%s]",sIn_PayFlag);
		pubLog_Debug(_FFL_, service_name, "", "fIn_Sim_fee = [%f]",fIn_Sim_fee);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Fee_List = [%s]",sIn_Fee_List);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Phone_no_List = [%s]",sIn_Phone_no_List);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Sim_no_List = [%s]",sIn_Sim_no_List);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Choice_fee_List = [%s]",sIn_Choice_fee_List);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Favour_List = [%s]",sIn_Favour_List);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Password = [%s]",sIn_Password);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Login_Accept = [%s]",sIn_Login_Accept);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Check_No = [%s]",sIn_Check_No);
		pubLog_Debug(_FFL_, service_name, "", "sIn_Bank_Code = [%s]",sIn_Bank_Code);
	#endif


	Vret=0;
	Vret=sGetLoginOrgid(sIn_Login_no,vOrgId);
	if(Vret!=0)
	{
		sprintf(oRetCode, "%06d",Vret);
		strcpy(oRetMsg,"ȡ�˻���������2��");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);			
	}	
	Trim(vOrgId);
 /*--������֯�������� liuhao    200090520 begin*/	
	sprintf (oRetCode, "%06d", spublicRight( LABELDBCHANGE, sIn_Login_no, vPassWord, sIn_Op_Code ));
	if (strcmp(oRetCode, "000000") != 0)
	{
		strcpy(oRetMsg,"����Ȩ����֤ʧ��!");
		pubLog_Debug(_FFL_, service_name, "000001", oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}                                      
/*--������֯�������� liuhao    200090520 end*/	
    	
    /* ��ѯ �������� */
    EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss'),to_char(sysdate,'yyyymmdd'),
                    to_char(sysdate,'yyyymm'),substr(:sIn_Org_code,1,2)
            INTO :sV_Sys_date,:sV_Total_date  ,:sV_YYYYMM , :sV_Region_code
            FROM dual;
    if (SQLCODE !=0)
    {
		strcpy(oRetCode,"111801");
		strcpy(oRetMsg,"��ѯ�������ڳ���!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);	
    }
    
    Trim(sV_Sys_date);
    Trim(sV_Total_date);
    Trim(sV_YYYYMM);
    Trim(sV_Region_code);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "sV_Sys_date=[%s],sV_Total_date = [%s]",sV_Sys_date,sV_Total_date);
        pubLog_Debug(_FFL_, service_name, "", "sV_YYYYMM = [%s] ",sV_YYYYMM); 
        pubLog_Debug(_FFL_, service_name, "", "sV_Region_code = [%s] ",sV_Region_code); 
    #endif
    
    /* ȡ�ò�����ˮ */
    Trim(sIn_Login_Accept);
    if  ((strlen(sIn_Login_Accept)==0)||(strcmp(sIn_Login_Accept,"0")==0))
    {
    EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :sV_Login_accept
             FROM DUAL;
             if(SQLCODE!=0){
                 pubLog_Debug(_FFL_, service_name, "", "ȡ����������ˮ����!");
                 strcpy(oRetCode,"111866");
                 strcpy(oRetMsg, "ȡ����������ˮ����!");
                 PUBLOG_DBDEBUG(service_name);
                 pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                 RAISE_ERROR(oRetCode,oRetMsg);
             }
    
    }
    else
    {
    	strcpy(sV_Login_accept ,sIn_Login_Accept);
    }
    
    Trim(sV_Login_accept);
    
    iLoginAccept=atol(sV_Login_accept);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " sV_Login_accept = [%s]",sV_Login_accept);
    #endif
    
       
    
    /*----------------------- �����Ż���Ϣ�� --------------------------*/
    if(strlen(ltrim(rtrim(sIn_Favour_List)))!=0)
    {        
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        memcpy(vTmp_List,sIn_Favour_List,sizeof(sIn_Favour_List)-1);
        tmplen=strlen(vTmp_List);
        begin = 1; end = 1; k=0;
        
        strcpy(sTmp_Str,"MULL");
        for(i=0;i<=tmplen;i++)
        {
            if(vTmp_List[i]=='~')
            {
                k=k+1;
                end = i;
                num = end - begin + 1;
                   
            
                memset(vTmp_List1,0x00,sizeof(vTmp_List1));
                strcpy(vTmp_List1,rtrim(substr(vTmp_List,begin,num)));
            
                tmplen1=strlen(vTmp_List1);
                begin1 = 1; end1 = 1; k1=0;
            
                for(i1=0;i1<=tmplen1;i1++)
                {
                    if(vTmp_List1[i1]=='&')
                    {
                        k1=k1+1;
                        end1 = i1;
                        num1 = end1 - begin1 + 1;
                        vTmp_List1[i1]='\0';
                        if(k1==1) strcpy(sV_Favour_Code[k-1],substr(vTmp_List1,begin1,num1));
                        if(k1==2) strcpy(sV_Should_Data[k-1],substr(vTmp_List1,begin1,num1));
                        if(k1==3) strcpy(sV_Actual_Data[k-1],substr(vTmp_List1,begin1,num1));
                    
                        begin1 = end1 +2;
                    }
               }
            
               #ifdef _DEBUG_
                   pubLog_Debug(_FFL_, service_name, "", "�Żݴ���[%d]       = [%s]",k,sV_Favour_Code[k-1]);
                   pubLog_Debug(_FFL_, service_name, "", "�Ż�ǰ����[%d]     = [%s]",k,sV_Should_Data[k-1]);
                   pubLog_Debug(_FFL_, service_name, "", "�Żݺ�����[%d]     = [%s]",k,sV_Actual_Data[k-1]);
               #endif
               ret=-1;
               
               strcpy(sV_User_Id,"0");
               
               ret=pubOpenAcctFav(sV_User_Id,            sTmp_Str,                      sIn_Sm_code,         
                                  sIn_Belong_Code,       sV_Favour_Code[k-1],           sV_Should_Data[k-1],
                                  sV_Actual_Data[k-1],   sIn_Login_no,                  sV_Login_accept,    
                                  sIn_Op_Note,           sIn_System_Note,               sIn_Op_Code,            
                                  sV_Total_date,         sV_Sys_date,vOrgId);
               if(ret != 0)
               {
                   pubLog_Debug(_FFL_, service_name, "", "�����Ż���Ϣ����!");
                   strcpy(oRetCode,"111802");
                   strcpy(oRetMsg, "�����Ż���Ϣ����!");
                   PUBLOG_DBDEBUG(service_name);
                   pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                   RAISE_ERROR(oRetCode,oRetMsg);
               }
               begin = end +2;
            }
        }
    }
    
    /* ����ܷ�����Ϣ�� */
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,sIn_Fee_List,sizeof(sIn_Fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            k=k+1;
            end = i;
            num = end - begin + 1;
            
            if(k==1) fV_Sum_Cash_Pay = atof(substr(vTmp_List,begin,num));
            if(k==2) fV_Sum_Check_Pay = atof(substr(vTmp_List,begin,num));
            if(k==3) fV_Sum_Sim_Fee = atof(substr(vTmp_List,begin,num));
            if(k==4) fV_Sum_Choice_Fee = atof(substr(vTmp_List,begin,num));
            if(k==5) fV_SumHand_Fee = atof(substr(vTmp_List,begin,num));
        
            begin = end +2;
        }
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "���ֽ𽻿�=[%10.2f]",fV_Sum_Cash_Pay);
        pubLog_Debug(_FFL_, service_name, "", "��֧Ʊ����=[%10.2f]",fV_Sum_Check_Pay);
        pubLog_Debug(_FFL_, service_name, "", "��SIM����=[%10.2f]",fV_Sum_Sim_Fee);
        pubLog_Debug(_FFL_, service_name, "", "��ѡ�ŷ�=[%10.2f]",fV_Sum_Choice_Fee);
        pubLog_Debug(_FFL_, service_name, "", "������=[%10.2f]",fV_SumHand_Fee);
    #endif
    
    /* ��� �ֻ����봮 */
    iV_PhoneCount = 0;
    memset(sV_Phone_no_List,0,sizeof(sV_Phone_no_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Phone_no_List,sizeof(sIn_Phone_no_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_, service_name, "", "phone=[%s]",sTmp_Str );
            #endif
            
            strcpy(sV_Phone_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_PhoneCount = k;
    
    pubLog_Debug(_FFL_, service_name, "", "�����ֻ�����=[%d]",iV_PhoneCount);
    
    for (i=0;i<iV_PhoneCount;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]���ֻ����룽[%s]",i,sV_Phone_no_List[i]);
        #endif
    }
    
    /* ���SIM�����봮 */
    iV_Count = 0;
    init(sTmp_Str);
    memset(sV_Sim_no_List,0,sizeof(sV_Sim_no_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Sim_no_List,sizeof(sIn_Sim_no_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            pubLog_Debug(_FFL_, service_name, "", "sTmp_Str=[%s]",sTmp_Str );
            
            strcpy(sV_Sim_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_, service_name, "", "����SIM������=[%d]",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]��SIM�����룽[%s]",i,sV_Sim_no_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(oRetCode,"111803");
        strcpy(oRetMsg,"������ֻ�����������SIM��������һ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);		
    }
    
    
    /* ��� ѡ�ŷѴ� */
    iV_Count = 0;
    init(sTmp_Str);
    memset(fV_Choice_fee_List,0,sizeof(fV_Choice_fee_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Choice_fee_List,sizeof(sIn_Choice_fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", " vTmp_List = [%s]",vTmp_List);
    #endif

    tmplen=strlen(vTmp_List);
    begin = 1; end = 1; k=0;

    for(i=0;i<=tmplen;i++)
    {
        if(vTmp_List[i]=='~')
        {
            end = i;
            num = end - begin + 1;
            
            strcpy(sTmp_Str ,substr(vTmp_List,begin,num)); 
            pubLog_Debug(_FFL_, service_name, "", "sTmp_Str=[%s]",sTmp_Str );
            
            fV_Choice_fee_List[k] = atof(sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_, service_name, "", "����ѡ�ŷ�����=[%d]",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "��[%d]��ѡ�ŷѣ�[%f]",i,fV_Choice_fee_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(oRetCode,"111804");
        strcpy(oRetMsg,"������ֻ�����������ѡ�ŷ�������һ��!");
        PUBLOG_DBDEBUG(service_name);
        pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
        RAISE_ERROR(oRetCode,oRetMsg);		
    }
    
    for (i=0;i<iV_PhoneCount;i++)
    {
    	/* ������������������� �� �����ǵ�һ���� */
        if ((strcmp(sIn_Op_Code,"1118")==0)&&(strcmp(sIn_CustDocFlag,"1")==0))
        {   
            /* �����µĿͻ�ID */
            if(getMaxID(sV_Region_code, 0, sIn_Cust_Id ) == NULL)
            {
                strcpy(oRetCode,"111805");
                strcpy(oRetMsg,"spubGetId���д���!");
                PUBLOG_DBDEBUG(service_name);
                pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
                RAISE_ERROR(oRetCode,oRetMsg);
            }	
            
            Trim(sIn_Cust_Id);
    	
    	    #ifdef _DEBUG_
                pubLog_Debug(_FFL_, service_name, "", "sIn_Cust_Id[%s]",sIn_Cust_Id);
            #endif
        	
        }
    	
    	/* ��ѯ�û�ID */
        if(getMaxID(sV_Region_code, 1, sV_User_Id ) == NULL)
        {
            strcpy(oRetCode,"111806");
            strcpy(oRetMsg,"spubGetId���д���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    	
    	Trim(sV_User_Id);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_User_Id��[%s]",sV_User_Id);
        #endif
    	
    	
    	
    	/* �����롢SIM����Ч��*/
        ret=-1;
        ret=CheckPhoneSimNo(sV_Phone_no_List[i],     sIn_Org_code,      sIn_Sm_code,      
                            sV_Sim_no_List[i],       sIn_Sim_Type,      sIn_Id_Iccid,sIn_Login_no,oRetMsg,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�����롢SIM����Ч�Դ���[%s]",oRetMsg);
            sprintf(oRetCode,"%d",ret);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
        
        /* ����֧Ʊ���� , �ֽ𽻿� */
        if (strcmp(sIn_PayFlag,"0")==0)
        {
            fV_Cash_Pay = fIn_Sim_fee + fV_Choice_fee_List[i] ;	
            fV_Check_Pay = 0;
        }
        else
        {
            fV_Cash_Pay =  0;	
            fV_Check_Pay = fIn_Sim_fee + fV_Choice_fee_List[i] ;
        }
        
        /* ������ҵ��ȷ�� */
          /* ������������������ */
/*
        smap_ret=0;
        ret=-1;
        ret=pubSMAPConfirm(sV_Phone_no_List[i],      sIn_Op_Code,      sV_Sys_date,
                           sIn_Belong_Code,          sIn_Login_no);
        if(ret != 0){
            smap_ret=ret;
            pubLog_Debug(_FFL_, service_name, "", "������������ʧ��!");
            strcpy(oRetCode,"110803");
            strcpy(sErrorMess, "������ҵ��ȷ�ϴ���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
            RAISE_ERROR(oRetCode,oRetMsg);
        }                         
*/                       
        ret=-1;    
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "fV_Cash_Pay[%f]        ",  fV_Cash_Pay);
            pubLog_Debug(_FFL_, service_name, "", "fV_Check_Pay[%f]        ",  fV_Check_Pay);
            pubLog_Debug(_FFL_, service_name, "", "fIn_Sim_fee[%f]         ",  fIn_Sim_fee);
            pubLog_Debug(_FFL_, service_name, "", "fV_Choice_fee_List[%d],[%f]         ", i ,  fV_Choice_fee_List[i]);
            pubLog_Debug(_FFL_, service_name, "", "fV_Hand_fee[%f]         ",  fV_Hand_fee);
        #endif
        
        
        ret=pubSZXConfirm(sIn_Cust_Id,         sV_User_Id,       sIn_Cust_Passwd, 
                          sIn_Password,        sIn_Cust_Name,    sIn_Cust_Address,
                          sIn_CustId_Type,     sIn_Id_Iccid,     sV_Phone_no_List[i],     
                          sV_Sim_no_List[i],   sIn_Belong_Code,  sIn_Ip_Address,   
                          fV_Cash_Pay,         fV_Check_Pay,     fIn_Sim_fee,      
                          fV_Machine_fee,      fV_Choice_fee_List[i],   fV_Hand_fee,     
                          sIn_Login_no,        sV_Login_accept,         sIn_Op_Note,      
                          sIn_System_Note,     sIn_Op_Code,             sV_Total_date,   
                          sV_Sys_date,sTrue_flag);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "������ҵ��ȷ�ϴ���!");
            strcpy(oRetCode,"111807");
            strcpy(oRetMsg, "������ҵ��ȷ�ϴ���!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }  
    
        /* ȡ���ػ���ˮ */
        EXEC SQL SELECT sOffon.nextval  INTO :sV_Offon_accept  FROM DUAL;
                   
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_Offon_accept     = [%s]",sV_Offon_accept);
        #endif
        
        ret=-1;
        ret=pubOpenAcctSendCmd(sV_User_Id,           sV_Phone_no_List[i],       sV_Attr_code,
                               sIn_Sm_code,          sIn_Belong_Code,           sIn_Login_no,
                               sV_Login_accept,      sIn_Op_Note,               sIn_System_Note,
                               sIn_Op_Code,          sV_Total_date,             sV_Sys_date ,
                               sV_Offon_accept);
        if(ret != 0){
             pubLog_Debug(_FFL_, service_name, "", "���Ϳ��ػ��������!");
            strcpy(oRetCode,"111808");
            strcpy(oRetMsg, "���Ϳ��ػ��������!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }       
    	
        /**** Ĭ���ط���Ϣ ****/
          /* ��ѯ  IMSI���� */
        EXEC SQL select substr(trim(imsi_no),1,15) into :sV_ImsiNo
                 from   dSimRes
                 where  sim_no = :sV_Sim_no_List[i];
        if (SQLCODE != 0)
        {
            strcpy(oRetCode,"111809");
            strcpy(oRetMsg, "��ѯIMSI��ʧ��!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    
        Trim(sV_ImsiNo);
    
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_ImsiNo      =[%s]",      sV_ImsiNo);
        #endif
    
        /* ��ѯ  HLR���� */
        EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
                 where  phoneno_head = substr(:sV_Phone_no_List[i],1,7);
        if (SQLCODE != 0)
        {
            strcpy(oRetCode,"111810");
            strcpy(oRetMsg, "��ѯhlr����ʧ��!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    
        Trim(sV_hlrCode);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_, service_name, "", "sV_hlrCode      =[%s]",      sV_hlrCode);
        #endif
    
        /* ���ͻ���Ϣ������ֵ */
        funcEnv.transIN=transIN;
        funcEnv.transOUT=transOUT; 
        funcEnv.temp_buf=temp_buf;
        funcEnv.totalDate=sV_Total_date;
        funcEnv.opTime=sV_Sys_date;
        funcEnv.opCode=sIn_Op_Code;
        funcEnv.loginNo=sIn_Login_no;
        funcEnv.orgCode=sIn_Org_code;
        funcEnv.loginAccept=sV_Login_accept;
        funcEnv.idNo=sV_User_Id;
        funcEnv.phoneNo=sV_Phone_no_List[i];
        funcEnv.smCode=sIn_Sm_code;
        funcEnv.cmdRight="1";
        funcEnv.simNo=sV_Sim_no_List[i];
        funcEnv.belongCode=sIn_Belong_Code;
        funcEnv.offonAccept = ""; /* not use  */
        funcEnv.imsiNo=sV_ImsiNo;
        funcEnv.hlrCode=sV_hlrCode;

        ret=-1;
        ret = pubDefaultFunc(sV_User_Id , sIn_Login_no ,
             sIn_Belong_Code , sIn_Sm_code ,
             sV_Total_date , sV_Sys_date ,
             sV_Login_accept , sIn_Op_Code ,
             oRetMsg ,"0",
             &funcEnv);
        pubLog_Debug(_FFL_, service_name, "", "call pubDefaultFunc ,ret=[%d]!" , ret);
        if(ret != 0)
        {
            pubLog_Debug(_FFL_, service_name, "", "���Ĭ���ط�����,ret=[%d]!" , ret);
            strcpy(oRetCode,"111811");
            pubLog_Debug(_FFL_, service_name, "", "oRetMsg = [%s] ",oRetMsg);
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
/*
end_apply:
        if(strncmp(oRetCode,"000000",6)!=0)
        {
            EXEC SQL ROLLBACK;
            strncpy(sV_Status[i],"����ʧ��",8);

    	    if(smap_ret!=0)  �������������ӿڴ���Ҫ�ع��ӿڱ� 
    	    {
                ret=-1;
                ret=pubSMAPConfirm(sV_Phone_no_List[i],      "1105",        sV_Sys_date,
                               sIn_Belong_Code,          sIn_Login_no);
                if(ret != 0){
                    smap_ret=ret;
                    pubLog_Debug(_FFL_, service_name, "", "������������ʧ��!");
                    strcpy(oRetCode,"111812");
                    strcpy(sErrorMess, "������ҵ��ȷ�ϴ���!");
                }
            }                         
        }
        else
        {
            EXEC SQL COMMIT;
            strncpy(sV_Status[i],"�����ɹ�",8);
        }
*/
    }   /*  end -- for (i=0;i<iV_PhoneCount;i++)  */
    
    /* �����ܷ��� */
    if (strcmp(sIn_PayFlag,"1")==0)
    {
        fV_Sum_Cash_Pay = fV_Sum_Check_Pay;	
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_, service_name, "", "fV_Sum_Cash_Pay     = [%f]",fV_Sum_Cash_Pay);
    #endif
        
    /* ��¼֧Ʊ��Ϣ */  
    if(fV_Sum_Check_Pay>0)
    {
        ret=-1;
        ret=pubOpenAcctCheck_1(sIn_Check_No,   fV_Sum_Check_Pay,   sIn_Bank_Code,
                           sIn_Op_Note,      sIn_Login_no,       sV_Login_accept, 
                           sIn_Op_Code,      sV_Total_date,      sV_Sys_date,
                           sIn_System_Note,vOrgId);
        if(ret != 0){
            pubLog_Debug(_FFL_, service_name, "", "�޸�֧Ʊ��Ϣ����,ret=[%d]",ret);
            strcpy(oRetCode,"111809");
            strcpy(oRetMsg, "�޸�֧Ʊ��Ϣ����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
            RAISE_ERROR(oRetCode,oRetMsg);
        }
    }    
        
    /* ���� wLoginOprYYYYMM �� */
    /*
    **init(vTmp_List);
    **sprintf(vTmp_List,"insert into wLoginOpr%s(total_date,login_accept,op_code,pay_type,pay_money,sm_code,"
    **        " id_no,phone_no,org_code,login_no,"
    **        " op_time,op_note,IP_ADDR) "
    **    	" values(to_number(:sV_Total_date),to_number(:sV_Login_accept),:sIn_Op_Code,:sIn_PayFlag,%f,:sIn_Sm_code,"
    **        " to_number(:sV_User_Id),:sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,"
    **        " to_date(:sV_Sys_date,'yyyymmdd hh24:mi:ss'),:sIn_System_Note,:sIn_Ip_Address)",sV_YYYYMM,fV_Sum_Cash_Pay);
    **    
    **pubLog_Debug(_FFL_, service_name, "", "insert wLoginOpr%s' sql_str = [%s]",sV_YYYYMM,vTmp_List);
    **EXEC SQL PREPARE ins_stmt From :vTmp_List; 
    **EXEC SQL EXECUTE ins_stmt using :sV_Total_date,:sV_Login_accept,:sIn_Op_Code,:sIn_PayFlag,:sIn_Sm_code,:sV_User_Id,
    **                                :sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,:sV_Sys_date,:sIn_System_Note,
    **                                :sIn_Ip_Address; 
    **
    **if(SQLCODE != 0)
    **{
    **    strcpy(oRetCode,"111810");
    **    strcpy(sErrorMess, "����wLoginOpr�����!");
    **    PUBLOG_DBDEBUG(service_name);
    **    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,sErrorMess);
    **    RAISE_ERROR(oRetCode,oRetMsg);
    **}
    ***/
/*add by liupenga ��֯��������20090305*/
/*********��֯�������쿪����������  edit by liupenga at 03/03/2009*******/
    vLog.vId_No                            =  atol(sV_User_Id);
    vLog.vTotal_Date                       =   atol(sV_Total_date);
    vLog.vLogin_Accept                     =  atol(sV_Login_accept);

	 strncpy(vLog.vSm_Code,       sIn_Sm_code,2);
	 strncpy(vLog.vBelong_Code,   sIn_Belong_Code,7);
	 strncpy(vLog.vPhone_No,      sIn_CustDocFlag,11);
	 strncpy(vLog.vOrg_Code,      sIn_Org_code,9);
	 strncpy(vLog.vLogin_No,      sIn_Login_no,6);
	 strncpy(vLog.vOp_Code,       sIn_Op_Code,4);
	 strncpy(vLog.vOp_Time,       sV_Sys_date,17);
	 strncpy(vLog.vMachine_Code,  "000",3);
	 strncpy(vLog.vBack_Flag,     "0",  1);
	 strncpy(vLog.vEncrypt_Fee,   "0",  1);
	 strncpy(vLog.vSystem_Note,   sIn_System_Note,60 );
	 strncpy(vLog.vOp_Note,       sIn_Op_Note,60 );
	 strncpy(vLog.vGroup_Id,      vGroupId,10);
	 strncpy(vLog.vOrg_Id,        vOrgId,10);
	 Trim(sIn_PayFlag);
	 strcpy(vLog.vPay_Type,      sIn_PayFlag); /*�������*/
	 strcpy(vLog.vIp_Addr,        sIn_Ip_Address);
	 vLog.vPay_Money              = fV_Sum_Cash_Pay;
	 vLog.vCash_Pay               = 0;
	 vLog.vCheck_Pay          	  = 0;
	 vLog.vSim_Fee            	  = 0;
	 vLog.vMachine_Fee        	  = 0;
	 vLog.vInnet_Fee          	  = 0;
	 vLog.vChoice_Fee         	  = 0;
	 vLog.vOther_Fee           	  = 0;
	 vLog.vHand_Fee           	  = 0;
	 vLog.vDeposit            	  = 0;
   
   Vret = 0;
   strcpy(vChgFlag,"N");
   strcpy(vCustFlag,"N");
  
   pubLog_Debug(_FFL_, service_name, "", "========%ld====",vLog.vId_No); 
  
   Vret = recordOprLog(&vLog,oRetMsg,vChgFlag,vCustFlag);
   if(Vret != 0)
   	{
   	  sprintf(oRetCode,"%06d",Vret);
	    PUBLOG_DBDEBUG(service_name);
	    pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
	    RAISE_ERROR(oRetCode,oRetMsg);
   	}       
    
SERVICE_FINALLY_DECLARE();
	PUBLOG_DBDEBUG(service_name);
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetMsg, oRetCode);
        /* ѹ�뷵����Ϣ  */
    pubLog_Debug(_FFL_, service_name, "", "oRetCode �� [%s],oRetMsg=[%s]",oRetCode,oRetMsg);
    pubLog_Debug(_FFL_, service_name, "", "SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        
    if(strncmp(oRetCode,"000000",6)!=0) /* ȷ��ʧ�� */
    	EXEC SQL ROLLBACK;
    else        /* ����ȷ�ϳɹ� */
        EXEC SQL COMMIT;
    
    transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	transOUT = Add_Ret_Value(GPARM32_2,sV_Login_accept);

	/*��ȡ��������ʱ��*/
	getTimeDif(&vUsedTime, vStartSec);
	
	/*Ϊ������־�ṹ�帳ֵ*/
	strcpy(logShip.vBegin_Time, vStartTime);
	logShip.vUsed_Time = vUsedTime;
	logShip.vLogion_Accept = atol(sV_Login_accept);
	strcpy(logShip.vLogin_No, sIn_Login_no);
	strcpy(logShip.vOp_Code, sIn_Op_Code);
	strcpy(logShip.vServ_Name, service_name);
	strcpy(logShip.vPhone_No, "1");
	logShip.vId_No = atol(sIn_Login_no);
	logShip.vOther_No = atol(sIn_Cust_Id);
	strcpy(logShip.vRetCode, oRetCode);
	strcpy(logShip.vRetMsg, oRetMsg); 
	/*����������־��־λ*/
	SetBomcLogFlag(&logShip);
	/*��¼������־*/
	WriteBomcLog(&logShip);
TUX_SERVICE_END();
}

