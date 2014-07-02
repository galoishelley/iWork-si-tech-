/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
#include "boss_srv.h"
#include "publicsrv.h"
#include "pubLog.h"
/**  �����������б�
  *  CheckPhoneSimNo     : ��������ֻ����롢SIM��״̬          
  *  pubOpenAcctRes      : �û������޸���Դ��Ϣ                 
  *  pubOpenAcctFunc     : �û����������ط���Ϣ(��ʱ��Ҫ)                 
  *  pubOpenAcctCheck_2    : �û�����֧Ʊ���ݴ���                 
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

/*����ORACLE��SQLͨ����*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/*�������������������,input_parms�������մ�������Ĳ�����
   output_parms�����洢���񷵻صĲ��� */

EXEC SQL BEGIN DECLARE SECTION;
   char input_parms[MAX_PARMS_NUM][1000];
   char output_parms[MAX_PARMS_NUM][1000];
EXEC SQL END DECLARE SECTION;

int tpsvrinit(int argc, char *argv[])
{
    spublicDBLogin(NULL,NULL,"CheckPhoneSimNo",LABELDBCHANGE);
    return(0);
}

void tpsvrdone()
{
    spublicDBClose(LABELDBCHANGE);
}

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
                    char sIn_Phone_logNoA[6],char * sErrorMessA)
{
    EXEC SQL BEGIN DECLARE SECTION;                               
        char    sIn_Phone_no[15+1];          /* �ֻ�����      */
        char    sIn_OrgCode[9+1];            /* ��������      */
        char    sIn_Sm_code[2+1];            /* ҵ������      */
        char    sIn_Sim_no[20+1];            /* SIM������     */
        char    sIn_Sim_type[5+1];           /* SIM������     */
        char    sIn_Phone_Icc[20+1];	     /* ֤������      */
        char	sIn_Phone_logNo[6+1];	     /* ����          */
                                                              
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
        int 	vvCount=0;
        char    vLchntype[1+1];			/*���Ź�����������*/
        char    vPchntype[1+1];			/*���������������*/
        char    vLgroupid[10+1];		/*����group_id*/
        char    vPgroupid[10+1];		/*����group_id*/
        
		char	sGroup_Id[10+1];
		int		ret=0;
		char 	RetMsg[200+1];
		char	ClassCode1[3+1]="11";
		char	ClassCode2[3+1]="12";
		char	classcode[3+1];
		char	Flag[1+1];
		char	dGroupId[10+1];
	  	char	vIsTown[1+1];	
    EXEC SQL END DECLARE SECTION;
    
    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","--------------- begin function CheckPhoneSimNo ----------");
    
    /* ���Ի����� */
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
    init(sGroup_Id);
    init(RetMsg);
    init(classcode);
    init(Flag);	
    init(dGroupId);
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
       
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_OrgCode       =[%s]",      sIn_OrgCode); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Sm_code       =[%s]",      sIn_Sm_code);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Sim_no        =[%s]",      sIn_Sim_no); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Sim_type      =[%s]",      sIn_Sim_type); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Phone_Icc     =[%s]",      sIn_Phone_Icc);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Phone_logNo   =[%s]",      sIn_Phone_logNo);
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
		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999901","sErrorMessA=[%s]",sErrorMessA);
        return 999901;
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Region_code   =[%s] ", sV_Region_code);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_District_code =[%s] ", sV_District_code);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Town_code     =[%s] ", sV_Town_code); 
    #endif
    
     /*--��֯���������ȡgroup_id edit by magang at 06/05/2009--begin*/
    /*����group_id��*/
	EXEC SQL select ORG_ID into :vLgroupid from dloginmsg where login_no=:sIn_Phone_logNo;
	if(SQLCODE!=0)
	{
       	    strcpy(sErrorMessA,"��ѯ����group_idʧ��!");
       	    PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999806","sErrorMessA=[%s]",sErrorMessA);
       	    return 999806;
	}  
    /* ��ѯ�������� */
    EXEC SQL SELECT  No_Type  
               INTO :sV_No_type
               FROM  sNoTypecode 
              WHERE  region_code = :sV_Region_code
                AND  Sm_Code     = :sIn_Sm_code;
    if (SQLCODE !=0)
    {
        strcpy(sErrorMessA,"��ѯsNoTypecode�����!");
       	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
      	pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999902","sErrorMessA=[%s]",sErrorMessA);
        return 999902 ;	
    }
     
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_No_type = [%s]",sV_No_type); 
    #endif
    
    /* ��ѯ����� �������� , ���ش��� , ������� , ����״̬ */
    init(sTmp_Region_code);
    init(sTmp_District_code);
    init(sTmp_Town_code);
    init(sTmp_State);
    init(sTmp_No_type);
    
    EXEC SQL SELECT  Region_Code,      District_Code,  
                     Town_Code,        Resource_Code,       
                     substr(No_Type,5,1),nvl(group_id,'')
               INTO :sTmp_Region_code,:sTmp_District_code,
                    :sTmp_Town_code , :sTmp_State ,
                    :sTmp_No_type ,		:dGroupId
               FROM dCustRes
              WHERE Phone_No = :sIn_Phone_no;
    if (SQLCODE !=0)
    {
    	if (SQLCODE == 1403)
    	{
    	    strcpy(sErrorMessA,"�ֻ����벻������dCustRes��!");
    	    PUBLOG_DBDEBUG("CheckPhoneSimNo");	
      		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999904","sErrorMessA=[%s]",sErrorMessA);
            return 999904 ;
    	}
    	else
    	{
            strcpy(sErrorMessA,"��ѯdCustRes�����!");
            PUBLOG_DBDEBUG("CheckPhoneSimNo");	
      		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999905","sErrorMessA=[%s]",sErrorMessA);
            return 999905 ;     
        }	
    }
    
    Trim(sTmp_Region_code);
    Trim(sTmp_District_code);
    Trim(sTmp_Town_code);
    Trim(sTmp_State);
    Trim(sTmp_No_type);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_No_type       = [%s] ",  sTmp_No_type);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_Region_code   = [%s] ",  sTmp_Region_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_District_code = [%s] ",  sTmp_District_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_Town_code     = [%s] ",  sTmp_Town_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_State         = [%s] ",  sTmp_State); 
    #endif
    
    if ((strcmp(sTmp_State,"0")!=0)&&(strcmp(sTmp_State,"h")!=0))
    {
        strcpy(sErrorMessA,"�ֻ������Ѿ���ʹ��!");
        pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999911","sErrorMessA = [%s]",sErrorMessA);
        return 999911 ; 
    }
    
    Trim(sIn_Phone_Icc);
    Trim(sIn_Phone_no);
    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++sIn_Phone_Icc[%s]",sIn_Phone_Icc);
    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++sIn_Phone_no[%s]",sIn_Phone_no);
    
    vCount=0;
    EXEC SQL select count(*) into :vCount 
    	from dCustOccupied 
    	where phone_no=:sIn_Phone_no and op_kind='0' and begin_time >sysdate-1/24;
    if(SQLCODE!=0)
    {
        	strcpy(sErrorMessA,"��ѯdCustOccupied��ʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999801","sErrorMessA=[%s]",sErrorMessA);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999819","sErrorMessA=[%s]",sErrorMessA);
        	return 999819;
    	}
    	if(vvCount==0)
    	{
    		strcpy(sErrorMessA,"�û�������֤��������ԤԼ֤�����벻һ��!");
    		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999802",sErrorMessA);
    		return 999802;
    	}
    	EXEC SQL insert into dCustOccupiedhis(ID_NO,PHONE_NO,BEGIN_TIME,LOGIN_ACCEPT,BAK_FIELD,
    					UPDATE_ACCEPT,UPDATE_LOGIN,UPDATE_TIME,UPDATE_CODE)
    				select ID_NO,PHONE_NO,BEGIN_TIME,sMaxSysAccept.nextVal,'',
    					UPDATE_ACCEPT,:sIn_Phone_logNoA,sysdate,'2592'
    			from dCustOccupied 
    			where phone_no=:sIn_Phone_no 
    			and id_no=trim(:sIn_Phone_Icc) 
    			and op_kind='0' 
    			and begin_time >sysdate-1/24;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"����dCustOccupiedhis��ʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999820","sErrorMessA=[%s]",sErrorMessA);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999905","sErrorMessA=[%s]",sErrorMessA);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999801","sErrorMessA=[%s]",sErrorMessA);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999819","sErrorMessA=[%s]",sErrorMessA);
        	return 999819;
    	}
    	if(vvCount==0)
    	{
    		strcpy(sErrorMessA,"�û�������֤��������ԤԼ֤�����벻һ��!");
    		pubLog_Debug(_FFL_,"CheckPhoneSimNo","999802",sErrorMessA);
    		return 999802;
    	}
    	EXEC SQL update webPhoneres set resource_code='5' where phone_no=:sIn_Phone_no;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"���±�webPhoneresʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999853","sErrorMessA=[%s]",sErrorMessA);
        	return 999853;
    	}
    	EXEC SQL insert into webPhoneres_tmp (LOGIN_ACCEPT,PHONE_NO,NO_TYPE,RESOURCE_CODE,CHOICE_FEE,
    				LIMIT_FEE,REGION_CODE,GROUP_ID,GROUP_NAME)
    		select 0,PHONE_NO,NO_TYPE,RESOURCE_CODE,CHOICE_FEE,
    			LIMIT_FEE,REGION_CODE,GROUP_ID,GROUP_NAME
    		from webPhoneres where phone_no=:sIn_Phone_no;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"insert webPhoneres_tmpʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999854","sErrorMessA=[%s]",sErrorMessA);
        	return 999854;
    	}
    }     
    	
	/*--��֯��������edit by magang at 06/05/2009--*/
    
    /*if (strcmp(sTmp_Region_code,sV_Region_code)!=0){
        strcpy(sErrorMessA,"���ֻ����벻���ڱ�����!");
        pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999908","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return 999908 ; 
    } 
    */
 	ret=0;
   	ret = sCheckGroupId(dGroupId,vLgroupid,ClassCode1,Flag,RetMsg);
   	Trim(Flag);
	if(strcmp(Flag,"Y")!=0)
	{
		strcpy(sErrorMessA,"SIM�����벻���ڱ�����!");
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","999916","sErrorMessA = [%s]",sErrorMessA);
        return 999916 ; 	
   	}     
    /*--��֯��������edit by magang at 06/05/2009--end*/
    
    /*if ((strcmp(sTmp_District_code,sV_District_code)!=0) 
       &&(strcmp(sTmp_District_code,"99")!=0)){
        strcpy(sErrorMessA,"�ֻ����벻���ڱ����ػ���!");	
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	return 999909 ; 
    }  
    
    if ((strcmp(sTmp_Town_code,sV_Town_code)!=0) 
       &&(strcmp(sTmp_Town_code,"999")!=0))
    {
       	iV_Count=0;
       	EXEC SQL select count(*) into :iV_Count from wtowntown where region_code=:sV_Region_code
       	    and district_code=:sV_District_code and town_code=:sV_Town_code and allow_town=:sTmp_Town_code;
       	if(SQLCODE!=0 && SQLCODE!=1403)
       	{
       	    strcpy(sErrorMessA,"��ѯwtowntown��ʧ��!");	
       	    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
       	    return 999910;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"�ֻ����벻���ڱ��������������!");	
            pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	    return 999910 ; 
    	}
    }*/   
    /*--��֯��������edit by magang at 06/05/2009--*/
    
    /*if ((strcmp(sTmp_District_code,sV_District_code)!=0))
    {
        strcpy(sErrorMessA,"�ֻ����벻���ڱ�����!");
        pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999803","");
    	return 999803 ; 
    }*/
	ret=0;
   	ret = sCheckGroupId(dGroupId,vLgroupid,ClassCode2,Flag,RetMsg);
   	Trim(Flag);
	if(strcmp(Flag,"Y")!=0)
	{
		strcpy(sErrorMessA,"SIM�����벻���ڱ�����!");
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","999916","sErrorMessA = [%s]",sErrorMessA);
        return 999916 ; 	
   	}    
	/* --��֯��������edit by magang at 06/05/2009--end*/


	 
    /*���Ź����������ͣ�*/
	EXEC SQL select distinct c.chntype 
	into :vLchntype 
	from dloginmsg a,dchngroupmsg b,schnclass c
	where a.login_no=:sIn_Phone_logNo and a.org_id = b.group_id /*and a.group_id=b.group_id  edit by liuweib20091023*/
	and b.class_code=c.class_code;
	if(SQLCODE!=0)
       	{
       	    strcpy(sErrorMessA,"��ѯ���Ź�����������ʧ��!");	
       	    PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999804","sErrorMessA=[%s]",sErrorMessA);
       	    return 999804;
       	}
       	if((strcmp(vLchntype,"0")!=0)&(strcmp(vLchntype,"2")!=0))
       	{
       		strcpy(sErrorMessA,"�ù��Ź����������ʹ���!");	
       	    	pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999899","�ù��Ź����������ʹ���[%s][%s]",sIn_Phone_logNo,vLchntype);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999805","sErrorMessA=[%s]",sErrorMessA);
       	    return 999805;
       	}
       	if((strcmp(vPchntype,"0")!=0)&(strcmp(vPchntype,"1")!=0)&(strcmp(vPchntype,"2")!=0)&(strcmp(vPchntype,"3")!=0))
       	{
       		strcpy(sErrorMessA,"�ú�������������ʹ���!");	
       	    	pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999898","�ú�������������ʹ���[%s][%s]",sIn_Phone_no,vPchntype);
       	    	return 999898;
       	}
   
   /*����group_id��*/
	EXEC SQL select group_id into :vPgroupid from dcustres where phone_no=:sIn_Phone_no;
	if(SQLCODE!=0)
       	{
       	    strcpy(sErrorMessA,"��ѯ����group_idʧ��!");	
       	   	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999807","sErrorMessA=[%s]",sErrorMessA);
       	    return 999807;
       	} 
        
        
        /*�Ƿ�����������ж���Դ�Ĺ���group_id�Ƿ�Ϊ��½���ŵĸ����*/
       	EXEC SQL select count(*) into :vCount 
    			from dChnGroupInfo 
    			where parent_group_id=:vPgroupid and group_id=:vLgroupid;
    	if(SQLCODE!=0)
    	{
        	strcpy(sErrorMessA,"��ѯdChnGroupInfo��ʧ��!");	
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999829","sErrorMessA=[%s]",sErrorMessA);
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
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999830","sErrorMessA=[%s]",sErrorMessA);
        	return 999830;
    	}  	
       	
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vLchntype[%s]+++++++++++++",vLchntype);
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vPchntype[%s]+++++++++++++",vPchntype);
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vLgroupid[%s]+++++++++++++",vLgroupid);
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vPgroupid[%s]+++++++++++++",vPgroupid);
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vCount[%d]+++++++++++++",vCount);
       	pubLog_Debug(_FFL_,"CheckPhoneSimNo","","+++++++++++vvCount[%d]+++++++++++++",vvCount);
       	
       	
       	if(strcmp(vLchntype,"0")==0)
       	{
       		if(strcmp(vPchntype,"2")==0)
       		{
       			strcpy(sErrorMessA,"�ú�����Դ������������Ϊ����Ӫҵ�����������!");	
       			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999808","");
       	    		return 999808;
       		}
       		if(strcmp(vPchntype,"0")==0)
       		{
       			if(strcmp(vLgroupid,vPgroupid)!=0 && vCount==0 && vvCount==0)
       			{
       				strcpy(sErrorMessA,"�ú�����Դ�����빤����Դ������ͬ�����Ҹ���Դ���ܱ��ù��Ź������������!");
       					pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999809","");
       	    			return 999809;
       			}	
       		}
       	}
       	if(strcmp(vLchntype,"2")==0)
       	{
       		if((strcmp(vPchntype,"0")==0)||(strcmp(vPchntype,"1")==0))
       		{
       			strcpy(sErrorMessA,"�ú�����Դ������������Ϊ�԰�Ӫҵ���԰칲�����������!");	
       	    		pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999810","");
       	    		return 999810;
       		}
       		if(strcmp(vPchntype,"2")==0)
       		{
       			if(strcmp(vLgroupid,vPgroupid)!=0 && vCount==0 && vvCount==0)
       			{
       				strcpy(sErrorMessA,"�ú�����Դ�����빤����Դ������ͬ�����Ҹ���Դ���ܱ��ù��Ź������������!");	
       	    			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999810","");
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
    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sIn_Sim_no=[%s]",sIn_Sim_no);
    /*--��֯���������ȡgroup_id edit by magang at 06/05/2009--begin*/
    EXEC SQL SELECT  Sm_Code,    Sim_Type,
                     Region_Code,          nvl(District_Code,'ZZ'),
                     nvl(Town_Code,'ZZZ'), Sim_Status , PHONENO_HEAD, nvl(BEL_LOGIN,' '),nvl(GROUP_ID,' ')
               INTO :sV_Sm_code , :sV_Sim_type , 
                    :sTmp_Region_code,:sTmp_District_code,
                    :sTmp_Town_code,:sTmp_State , :sV_Phone_head, :sV_Bel_login,:sGroup_Id
               FROM  dSimRes
              WHERE Sim_No = :sIn_Sim_no;
    if (SQLCODE !=0)
    {
    	if (SQLCODE ==1403)
    	{
    	    strcpy(sErrorMessA,"SIM�����벻����dSimRes��!");
    	    PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999912","sErrorMessA=[%s]",sErrorMessA);
            return 999912 ;  	
    	}
    	else
    	{
            strcpy(sErrorMessA,"��ѯdSimRes�����!");
            PUBLOG_DBDEBUG("CheckPhoneSimNo");	
			pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999913","sErrorMessA=[%s]",sErrorMessA);
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
    Trim(sGroup_Id);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Sm_code         = [%s]",    sV_Sm_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Sim_type        = [%s]",    sV_Sim_type); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_Region_code   = [%s]",    sTmp_Region_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_District_code = [%s]",    sTmp_District_code); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_Town_code     = [%s]",    sTmp_Town_code);
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sTmp_State         = [%s]",    sTmp_State); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Phone_head      = [%s]",    sV_Phone_head); 
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Bel_login       = [%s]",    sV_Bel_login);
    #endif
    
     if (strcmp(sTmp_State,"0")!=0){
        strcpy(sErrorMessA,"SIM�������Ѿ���ʹ��!");
        pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999914","sErrorMessA = [%s]",sErrorMessA);
        return 999914 ; 
    }  

    if (strcmp(sV_Sim_type,sIn_Sim_type)!=0){
        strcpy(sErrorMessA,"SIM�����벻���ڱ�SIM������!");
        pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999915","sErrorMessA = [%s]",sErrorMessA);
        return 999915 ; 
    }  
 /*--��֯�����������dit by magang at 12/05/2009--begin-  */
    EXEC SQL SELECT class_code 
    		INTO :classcode 
    		FROM dchngroupmsg 
    		WHERE group_id=:sGroup_Id;
    Trim(classcode);
   /*--��֯�����������dit by magang at 12/05/2009--begin-         
    if (strcmp(sTmp_Region_code,sV_Region_code)!=0)  	
	{
        strcpy(sErrorMessA,"SIM�����벻���ڱ�����!");
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return 999916 ; 
    }  
    --��֯����������� edit by magang at 12/05/2009--end-*/
    init(Flag);
	ret=0;
   	ret = sCheckGroupId(sGroup_Id,vLgroupid,ClassCode1,Flag,RetMsg);
   	Trim(Flag);
	if(strcmp(Flag,"Y")!=0)
	{
		strcpy(sErrorMessA,"SIM�����벻���ڱ�����!");
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","999916","sErrorMessA = [%s]",sErrorMessA);
        return 999916 ; 	
   	}   
   	
    /*--��֯����������� edit by magang at 12/05/2009--begin-  
    if ((strcmp(sTmp_District_code,sV_District_code)!=0) 
       &&(strcmp(sTmp_District_code,"99")!=0))
    {
        strcpy(sErrorMessA,"SIM�����벻���ڱ����ػ���!");	
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	return 999917 ; 
    }  
	--��֯����������� edit by magang at 12/05/2009--end-*/
	
	init(Flag);
	ret=0;
   	ret = sCheckGroupId(sGroup_Id,vLgroupid,ClassCode2,Flag,RetMsg);
   	Trim(Flag);
	if((strcmp(Flag,"Y")!=0) && (strcmp(classcode,"11")!=0))
	{
		strcpy(sErrorMessA,"SIM�����벻���ڱ����ػ���!");	
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","999917","sErrorMessA = [%s]",sErrorMessA);
    	return 999917 ;	
   	}   
   	
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
       	    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
       	    return 999918;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"SIM�����벻���ڱ��������������!");	
            pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	    return 999918 ; 
    	}
    }   
 */
	/*���ڶ���sim����ԴҪ����䵽ӪҵԱ��ʹ��*/
	
	/*--��֯����������� edit by magang at 12/05/2009--begin-*/
    
    /*if ((strcmp(sTmp_Town_code,sV_Town_code)!=0) 
       &&(strcmp(sTmp_Town_code,"999")!=0))
    {
		strcpy(sErrorMessA,"SIM�����벻���ڱ��������!");	
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
    	return 999918 ;
    } 
    */
    if((strcmp(sGroup_Id,vLgroupid)!=0) && (strcmp(classcode,"12")!=0)&& (strcmp(classcode,"11")!=0))
    {
    	strcpy(sErrorMessA,"SIM�����벻���ڱ��������!");	
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","999918","sErrorMessA = [%s]",sErrorMessA);
    	return 999918 ;
    }
   /*--��֯����������� edit by magang at 12/05/2009--end-*/
     /*--��֯���������ȡgroup_id edit by magang at 06/05/2009--begin
    if ((strcmp(sIn_Phone_logNo,sV_Bel_login)!=0) && (strcmp(sTmp_Town_code,"999")!=0) && (strcmp(sTmp_Town_code,sV_Town_code)!=0))
     */
     
  	if ((strcmp(sIn_Phone_logNo,sV_Bel_login)!=0) && (strcmp(sGroup_Id,vLgroupid)!=0) && (strcmp(classcode,"12")!=0)&&(strcmp(classcode,"11")!=0))	
    {
       	iV_Count=0;
       	EXEC SQL select count(*) into :iV_Count from wtowntown where 
       		/*--��֯����������� edit by magang at 12/05/2009--begin-
       		region_code=:sV_Region_code
       	    and district_code=:sV_District_code and town_code=:sV_Town_code and allow_town=:sTmp_Town_code;
       	   --��֯����������� edit by magang at 12/05/2009--end-*/
       	   group_id=:vLgroupid 
       	   and allow_groupid =:sGroup_Id;
       	    
       	if(SQLCODE!=0 && SQLCODE!=1403)
       	{
       	    strcpy(sErrorMessA,"��ѯwtowntown��ʧ��!");	
       	    PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999919","sErrorMessA = [%s]",sErrorMessA);
       	    return 999919;
       	}
       	if(iV_Count==0)
       	{
            strcpy(sErrorMessA,"SIM�����벻���ڸõ�½���Ż������!");	
             PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999920","sErrorMessA = [%s]",sErrorMessA);
    	    return 999920 ; 
    	}
    }
    /*--��֯���������ȡgroup_id edit by magang at 06/05/2009--end*/	
  /*****/
    strncpy(sV_Phone_head,sIn_Phone_no,7);
    
    
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
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999921","sErrorMessA = [%s]",sErrorMessA);
            return 999921 ;  
        }
        else{
            strcpy(sErrorMessA,"���ֻ�����ĺŶ�û��ע�ᵽsHlrCode��!");
        	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999922","sErrorMessA = [%s]",sErrorMessA);
            return 999922 ;  	
        }	
    }
    
    Trim(sV_Phone_hlrcode);
   
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Phone_hlrcode = [%s]",sV_Phone_hlrcode); 
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
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999923","sErrorMessA = [%s]",sErrorMessA);
            return 999923 ;
        }
        else{
            strcpy(sErrorMessA,"SIM�������Ӧ�ĺŶ�û����sHlrcode����ע��!");
			PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999924","sErrorMessA = [%s]",sErrorMessA);
            return 999924 ;	
            
        }	
    }
    
    Trim(sV_Sim_hlrcode);
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"CheckPhoneSimNo","","sV_Sim_hlrcode = [%s]",sV_Sim_hlrcode); 
    #endif
    
    /* �Ƚ�����HlrCode */
    if (strcmp(sV_Phone_hlrcode,sV_Sim_hlrcode)!=0){
        strcpy(sErrorMessA,"�ֻ�������SIM����hlr����Ӧ!");
         	PUBLOG_DBDEBUG("CheckPhoneSimNo");	
       	    pubLog_DBErr(_FFL_,"CheckPhoneSimNo","999925","sErrorMessA = [%s]",sErrorMessA);
        return 999925 ;
    }
    
    strcpy(sErrorMessA,"SUCCESSED!");
   
    pubLog_Debug(_FFL_,"CheckPhoneSimNo","","--------------- end function CheckPhoneSimNo ----------");
    
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
int pubOpenCustConfirm(char *sIn_CustMsg,char *sIn_CustId,char *sIn_CustContact,char *sIn_CustAttr,char *sIn_CustAdd,
                       char *sV_Total_date,char *sV_LoginAccept,char *sV_Op_time,char *sV_YYYYMM,char *sErrorMess)
{
    int i=0,j=0,k=0,p=0,q=0,begin=0,end=0,tmplen=0,num=0;
    char vTmp_List[1000];
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
        
        int     sV_row=0;                           /* ��¼��            */

		TdCustDoc	stdCustDoc;						 	/*ng_���� add by mengfy@08/04/2009---*/
		int		v_ret = 0;
    
    EXEC SQL END DECLARE SECTION;

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","==========begin pubOpenCustConfirm =============="); 
    #endif
	
    /* ���Ի����� */
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
    
    memset(&stdCustDoc, 0, sizeof(stdCustDoc));			/*ng_���� add by mengfy@08/04/2009---*/
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","==========  ��ʼȡ��������� =============="); 
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
                if(k==2) strcpy(sIn_BelongCode,substr(vTmp_List,begin,num)); 
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
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_id  =[%s]",sIn_Cust_id);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_BelongCode  =[%s]",sIn_BelongCode);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_name=[%s]",sIn_Cust_name);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_passwd=[%s]",sIn_Cust_passwd);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_status=[%s]",sIn_Cust_status);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Owner_grade=[%s]",sIn_Owner_grade);            
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Owner_type=[%s]",sIn_Owner_type);            
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_address=[%s]",sIn_Cust_address);            
        #endif
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
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_type  =[%s]",sIn_Id_type);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_iccid  =[%s]",sIn_Id_iccid);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_address=[%s]",sIn_Id_address);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_validdate=[%s]",sIn_Id_validdate);
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
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_person  =[%s]",sIn_Contact_person);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_phone  =[%s]",sIn_Contact_phone);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_address=[%s]",sIn_Contact_address);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_post=[%s]",sIn_Contact_post);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_mailaddress=[%s]",sIn_Contact_mailaddress);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_fax=[%s]",sIn_Contact_fax);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_emaill=[%s]",sIn_Contact_emaill);
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
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Org_code  =[%s]",sIn_Org_code);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Parent_id  =[%s]",sIn_Parent_id);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_sex=[%s]",sIn_Cust_sex);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Birthday=[%s]",sIn_Birthday);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Profession_id=[%s]",sIn_Profession_id);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Edu_level=[%s]",sIn_Edu_level);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_love=[%s]",sIn_Cust_love);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_habit=[%s]",sIn_Cust_habit);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Orgcharacter_id=[%s]",sIn_Orgcharacter_id);
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
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Op_code  =[%s]",sIn_Op_code);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Work_no  =[%s]",sIn_Work_no);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Sys_note=[%s]",sIn_Sys_note);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Op_note=[%s]",sIn_Op_note);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Ip_Addr=[%s]",sIn_Ip_Addr);
            pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Group_index=[%s]",sIn_Group_index);
        #endif
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_id= [%s]  sIn_BelongCode=[%s]",sIn_Cust_id,sIn_BelongCode);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_name= [%s]  sIn_Cust_passwd=[%s]",sIn_Cust_name,sIn_Cust_passwd);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_status= [%s]  sIn_Owner_grade=[%s]",sIn_Cust_status,sIn_Owner_grade);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Owner_type= [%s]  sIn_Cust_address=[%s]",sIn_Owner_type,sIn_Cust_address);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Owner_type= [%s]  sIn_Cust_address=[%s]",sIn_Owner_type,sIn_Cust_address);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_type= [%s]  sIn_Id_iccid=[%s]",sIn_Id_type,sIn_Id_iccid);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Id_address= [%s]  sIn_Id_validdate=[%s]",sIn_Id_address,sIn_Id_validdate);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_person= [%s]  sIn_Contact_phone=[%s]",sIn_Contact_person,sIn_Contact_phone);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_address= [%s]  sIn_Contact_post=[%s]",sIn_Contact_address,sIn_Contact_post);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_mailaddress= [%s]  sIn_Contact_fax=[%s]",sIn_Contact_mailaddress,sIn_Contact_fax);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Contact_emaill= [%s]  sIn_Org_code=[%s]",sIn_Contact_emaill,sIn_Org_code);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Parent_id= [%s]  sIn_Cust_sex=[%s]",sIn_Parent_id,sIn_Cust_sex);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Birthday= [%s]  sIn_Profession_id=[%s]",sIn_Birthday,sIn_Profession_id);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Edu_level= [%s]  sIn_Cust_love=[%s]",sIn_Edu_level,sIn_Cust_love);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Cust_habit= [%s]  sIn_Orgcharacter_id=[%s]",sIn_Cust_habit,sIn_Orgcharacter_id);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Business_licencetype= [%s]  sIn_Business_licenceno=[%s]",sIn_Business_licencetype,sIn_Business_licenceno);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Licence_validdate= [%s]  sIn_Org_legalrep=[%s]",sIn_Licence_validdate,sIn_Org_legalrep);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Vpmngrp_info= [%s]  sIn_Op_code=[%s]",sIn_Vpmngrp_info,sIn_Op_code);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Work_no= [%s]  sIn_Sys_note=[%s]",sIn_Work_no,sIn_Sys_note);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Op_note= [%s]  sIn_Ip_Addr=[%s]  ",sIn_Op_note,sIn_Ip_Addr);
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sIn_Group_index= [%s]",sIn_Group_index);
    #endif
	
    /* ���  �������� */
    EXEC SQL SELECT substr(:sIn_BelongCode,1,2),substr(:sIn_BelongCode,3,2),substr(:sIn_BelongCode,5,3)
             INTO :sV_Region_code,:sV_District_code,:sV_Town_code FROM dual;
    if(SQLCODE!=OK)
    {
        strcpy(sErrorMess,"��ֹ����������!");
        PUBLOG_DBDEBUG("pubOpenCustConfirm");	
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110002","");
        return 110002;	
    }
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]",\
        sV_Region_code,sV_District_code,sV_Town_code); 
    #endif
    
    Trim(sIn_Id_type);
pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","[%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s]",\
                   sIn_Cust_id,sV_Region_code,sV_District_code,sV_Town_code,\
                   sIn_Cust_name,sIn_Cust_passwd,sIn_Cust_status,sV_Op_time,\
                   sIn_Owner_grade,sIn_Owner_type,sIn_Cust_address,sIn_Id_type,\
                   sIn_Id_iccid,sIn_Id_address,sIn_Id_validdate,sIn_Contact_person,\
                   sIn_Contact_phone,sIn_Contact_address,sIn_Contact_post,sIn_Contact_mailaddress,\
                   sIn_Contact_fax,sIn_Contact_emaill,sIn_Org_code,sV_Op_time,\
                   sIn_Parent_id,sV_LoginAccept,sV_Op_time,sIn_Work_no,\
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
        
    EXEC SQL insert into dCustDocHis(CUST_ID,
                  REGION_CODE,DISTRICT_CODE, TOWN_CODE, 
                  CUST_NAME, CUST_PASSWD,
                  CUST_STATUS, RUN_TIME, 
                  OWNER_GRADE, OWNER_TYPE,CUST_ADDRESS, ID_TYPE, 
                  ID_ICCID, ID_ADDRESS,
                  ID_VALIDDATE,CONTACT_PERSON,
                  CONTACT_PHONE,CONTACT_ADDRESS,CONTACT_POST,CONTACT_MAILADDRESS,
                  CONTACT_FAX,CONTACT_EMAILL,ORG_CODE,
                  CREATE_TIME,CLOSE_TIME,
                  PARENT_ID,
                  UPDATE_ACCEPT,UPDATE_TIME,
                  UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE)
             values(
                   to_number(:sIn_Cust_id),:sV_Region_code,:sV_District_code,:sV_Town_code,
                   :sIn_Cust_name,:sIn_Cust_passwd,
                   :sIn_Cust_status,to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
                   :sIn_Owner_grade,:sIn_Owner_type,:sIn_Cust_address,:sIn_Id_type,
                   :sIn_Id_iccid,:sIn_Id_address,
                   to_date(:sIn_Id_validdate,'YYYYMMDD'),:sIn_Contact_person,
                   :sIn_Contact_phone,:sIn_Contact_address,:sIn_Contact_post,:sIn_Contact_mailaddress,
                   :sIn_Contact_fax,:sIn_Contact_emaill,:sIn_Org_code,
                   to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
                   to_date('20501230','yyyymmdd'),
                   to_number(:sIn_Parent_id),
                   to_number(:sV_LoginAccept),to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
                   :sIn_Work_no,'a',:sIn_Op_code,to_number(:sV_Total_date));
    if (SQLCODE !=OK)
    {
        strcpy(sErrorMess,"����dCustDocHis�����!");
        PUBLOG_DBDEBUG("pubOpenCustConfirm");	
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110004","sErrorMessA = [%s]",sErrorMess);
        return 110004 ;
    }


    /* ���� dCustDoc �� */
 	/*ng_���� add by mengfy@08/04/2009---*/
/*    EXEC SQL insert into dCustDoc(
        cust_id,region_code,district_code,town_code,
        cust_name,cust_passwd,cust_status,run_time,
        owner_grade,owner_type,cust_address,id_type,
        id_iccid,id_address,id_validdate,contact_person,
        contact_phone,contact_address,contact_post,contact_mailaddress,
        contact_fax,contact_emaill,org_code,create_time,CLOSE_TIME,parent_id)
    values(
        to_number(:sIn_Cust_id),:sV_Region_code,:sV_District_code,:sV_Town_code,
        :sIn_Cust_name,:sIn_Cust_passwd,:sIn_Cust_status,to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
        :sIn_Owner_grade,:sIn_Owner_type,:sIn_Cust_address,:sIn_Id_type,
        :sIn_Id_iccid,:sIn_Id_address,to_date(:sIn_Id_validdate,'YYYYMMDD'),:sIn_Contact_person,
        :sIn_Contact_phone,:sIn_Contact_address,:sIn_Contact_post,:sIn_Contact_mailaddress,
        :sIn_Contact_fax,:sIn_Contact_emaill,:sIn_Org_code,to_date(:sV_Op_time,'yyyymmdd hh24:mi:ss'),
        to_date('20501230','yyyymmdd'),
        to_number(:sIn_Parent_id));
    if (SQLCODE !=OK)
    {
        strcpy(sErrorMess,"����dCustDoc�����!");
        PUBLOG_DBDEBUG("pubOpenCustConfirm");
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110005","sErrorMessA = [%s]",sErrorMess);
        return 110005 ;
    }
*/
	memset(&stdCustDoc, 0, sizeof(stdCustDoc));
	
	strncpy(stdCustDoc.sCustId, sIn_Cust_id, 10);
	strncpy(stdCustDoc.sRegionCode, sV_Region_code, 2);
	strncpy(stdCustDoc.sDistrictCode, sV_District_code, 2);
	strncpy(stdCustDoc.sTownCode, sV_Town_code, 3);
	strncpy(stdCustDoc.sCustName, sIn_Cust_name, 60);
	strncpy(stdCustDoc.sCustPasswd, sIn_Cust_passwd, 8);
	strncpy(stdCustDoc.sCustStatus, sIn_Cust_status, 2);
	strncpy(stdCustDoc.sRunTime, sV_Op_time, 17);
	strncpy(stdCustDoc.sOwnerGrade, sIn_Owner_grade, 2);
	strncpy(stdCustDoc.sOwnerType, sIn_Owner_type, 2);
	strncpy(stdCustDoc.sCustAddress, sIn_Cust_address, 17);
	strncpy(stdCustDoc.sIdType, sIn_Id_type, 2);
	strncpy(stdCustDoc.sIdIccid, sIn_Id_address, 20);
	strncpy(stdCustDoc.sIdAddress, sIn_Cust_address, 60);
	strncpy(stdCustDoc.sIdValiddate, sIn_Id_validdate, 17);
	strncpy(stdCustDoc.sContactPerson, sIn_Contact_person, 20);
	strncpy(stdCustDoc.sContactPhone, sIn_Contact_phone, 20);
	strncpy(stdCustDoc.sContactAddress, sIn_Contact_address, 100);
	strncpy(stdCustDoc.sContactPost, sIn_Contact_post, 6);
	strncpy(stdCustDoc.sContactMailaddress, sIn_Contact_mailaddress, 60);
	strncpy(stdCustDoc.sContactFax, sIn_Contact_fax, 30);
	strncpy(stdCustDoc.sContactEmaill, sIn_Contact_emaill, 30);
	strncpy(stdCustDoc.sOrgCode, sIn_Org_code, 9);
	strncpy(stdCustDoc.sCreateTime, sV_Op_time, 17);
	strcpy(stdCustDoc.sCloseTime, "20501230");
	strncpy(stdCustDoc.sParentId, sIn_Parent_id, 14);
	
	v_ret = 0;
	v_ret = OrderInsertCustDoc(ORDERIDTYPE_CUST, sIn_Cust_id, 100, sIn_Op_code, atol(sV_LoginAccept), sIn_Work_no,
			sIn_Op_note, stdCustDoc);
	if (0 != v_ret)
	{
        strcpy(sErrorMess,"����dCustDoc�����!");
        PUBLOG_DBDEBUG("pubOpenCustConfirm");
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110005","sErrorMessA = [%s],v_ret=[%d]",sErrorMess,v_ret);
        return 110005 ;		
	}
 	/*ng_���� add by mengfy@08/04/2009---*/
    if (strcmp(sIn_Owner_type,"01")==0){
        /* ���� dCustDocInAdd �� */
        init(sql_str);
        sprintf(sql_str,"insert into dCustDocInAdd(cust_id,cust_sex,birthday,profession_id,edu_level,cust_love,cust_habit)\
                 values(to_number(:v1),:v2,to_date(:v3,'YYYYMMDD'),:v4,:v5,:v6,:v7)");
Trim(sIn_Profession_id);
Trim(sIn_Cust_id);
pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","[%s][%s][%s][%s][%s][%s][%s]",sIn_Cust_id,sIn_Cust_sex,sIn_Birthday,sIn_Profession_id,sIn_Edu_level,sIn_Cust_love,sIn_Cust_habit);
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","insert dCustDocInAdd' sql_str = [%s]",sql_str);

        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt using :sIn_Cust_id,:sIn_Cust_sex,:sIn_Birthday,:sIn_Profession_id,:sIn_Edu_level,
        				:sIn_Cust_love,:sIn_Cust_habit;
        if (SQLCODE !=0)
        {
            strcpy(sErrorMess,"����dCustDocInAdd�����!");
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110006","sErrorMessA = [%s]",sErrorMess);
            return 110006 ;
        }
pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","[%s][%s][%s][%s][%s][%s][%s]",sIn_Cust_id,sIn_Cust_sex,sIn_Birthday,sIn_Profession_id,sIn_Edu_level,sIn_Cust_love,sIn_Cust_habit);
        
        /* ���� dCustDocInAddHis �� */
        /*������ edit by cencm at 20100414----------begin-----------------*/
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
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110007","sErrorMessA = [%s]",sErrorMess);
            return 110007 ;
        }
        */
        /*������ edit by cencm at 20100414 ------------end--------------*/
        
    }
    else  if (strcmp(sIn_Owner_type,"03")==0){
        /* ���� dGrpMid �� */
        sV_row=0;
        EXEC SQL select count(*) into :sV_row from dGrpMid where group_index=:sIn_Group_index;
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"��ѯdGrpMid�����!");
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110017","sErrorMessA = [%s]",sErrorMess);
            return 110017 ;
        }
        if(sV_row>0 && strcmp(sIn_Group_index,"0")!=0)
        {
            sprintf(sErrorMess,"ԭ���ź��Ѿ����ڣ�����������[%s]!",sIn_Group_index);
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110018","sErrorMessA = [%s]",sErrorMess);
            return 110018 ;
        }
        sV_row=0;
        EXEC SQL select count(*) into :sV_row from dGrpDoc where group_index=:sIn_Group_index;
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"��ѯdGrpDoc�����!");
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110017","sErrorMessA = [%s]",sErrorMess);
            return 110017 ;
        }
        if(sV_row>0 && strcmp(sIn_Group_index,"0")!=0)
        {
            sprintf(sErrorMess,"ԭ���ź��Ѿ����ڣ�����������[%s]!",sIn_Group_index);
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110018","sErrorMessA = [%s]",sErrorMess);
            return 110018 ;
        }
        
        if(strcmp(sIn_Group_index,"0")==0)
            strcpy(sIn_Group_index,"");
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","sIn_Group_index=[%s]",sIn_Group_index);
        
        EXEC SQL insert into dGrpMid(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate);
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpMid�����!");
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110008","sErrorMessA = [%s]",sErrorMess);
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
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110009","sErrorMessA = [%s]",sErrorMess);
            return 110009 ;
        }
        EXEC SQL insert into dGrpDoc(CUST_ID,group_index,OWNER_TYPE,CUST_NAME,REGION_CODE,DISTRICT_CODE,RUN_CODE,DEAL_FLAG,op_time)
                 values(to_number(:sIn_Cust_id),Trim(:sIn_Group_index),:sIn_Owner_type,:sIn_Cust_name,:sV_Region_code,:sV_District_code,'A','N',sysdate);
        if (SQLCODE !=OK){
            strcpy(sErrorMess,"����dGrpDoc�����!");
            PUBLOG_DBDEBUG("pubOpenCustConfirm");
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110008","sErrorMessA = [%s]",sErrorMess);
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
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110009","sErrorMessA = [%s]",sErrorMess);
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
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110015","sErrorMessA = [%s]",sErrorMess);
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
        	pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110016","sErrorMessA = [%s]",sErrorMess);
            return 110016 ;
        }
    }
    else{
        strcpy(sErrorMess,"����Ŀͻ����ͳ���!");
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","110012","");
        return 110012 ;	
    }
        
        /* ����ͻ��������ˮ�� */
        /*
	init(sql_str);
	sprintf(sql_str,"insert into wChg%s(id_no,total_date,login_accept,sm_code,belong_code,phone_no,\
            org_code,login_no,op_code,op_time,machine_code,cash_pay,check_pay,sim_fee,machine_fee,\
            innet_fee,choice_fee,other_fee,hand_fee,deposit,back_flag,encrypt_fee,system_note,op_note)\
        values(%s,'%s',%s,'zz','%s','z','%s','%s','%s',to_date('%s','yyyymmdd hh24:mi:ss'),'zz',0,0,0,0,0,0,0,0,0,'0',0,'%s','%s')",\
            sV_YYYYMM,sIn_Cust_id,sV_Total_date,sV_LoginAccept,sIn_BelongCode,sIn_Org_code,sIn_Work_no,sIn_Op_code,\
            sV_Op_time,sIn_Sys_note,sIn_Op_note);
            
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","insert wChg%s' sql_str = [%s]",sV_YYYYMM,sql_str);
        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt ; 
        if ( SQLCODE != OK )
        {
            strcpy(return_code,"110013");
            init(sErrorMess);
            sprintf(sErrorMess,"����wChg%s��ʧ��!",sV_YYYYMM);
            pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","%s:%s-- [%d][%s]",__FILE__,sErrorMess,SQLCODE,SQLERRMSG);
            goto end_valid ;
        }
        */
        /*
        init(sql_str);
        sprintf(sql_str,"insert into wLoginOpr%s( \
            total_date,login_accept,op_code,pay_type,pay_money, \
            sm_code,id_no,phone_no,org_code,login_no,op_time,op_note,IP_ADDR) \
        values(to_number(:sV_Total_date),to_number(:sV_LoginAccept),:sIn_Op_code,'0',0,\
            'ZZ',to_number(:sIn_Cust_id),'Z',:sIn_Org_code,\
            :sIn_Work_no,sysdate,:sIn_Op_note,:sIn_Ip_Addr)",sV_YYYYMM);
            
        pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","insert wLoginOpr%s' sql_str = [%s]",sV_YYYYMM,sql_str);
        EXEC SQL PREPARE ins_stmt From :sql_str; 
        EXEC SQL EXECUTE ins_stmt using :sV_Total_date,:sV_LoginAccept,:sIn_Op_code,:sIn_Cust_id,:sIn_Org_code,:sIn_Work_no,:sIn_Op_note,:sIn_Ip_Addr;
        if ( SQLCODE != 0 )
        {
            strcpy(sErrorMess,"����wLoginOpr��ʧ��!");
            pubLog_DBErr(_FFL_,"pubOpenCustConfirm","","%s:%s-- [%d][%s]",__FILE__,sErrorMess,SQLCODE,SQLERRMSG);
            return 110014;
        }
        */
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenCustConfirm","","==========end pubOpenCustConfirm =============="); 
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
       
       int	ret1=0;
       char class_code[6+1]="11";
       char vPGroupId1[10+1];
       char vRetMsg[40+1];
	   
	   int v_ret = 0;			/*ng_���� add by mengfy@08/04/2009*/
	   char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	   char	sTempSqlStr[2204];
	   char vImsi_No_o[20+1];
	   TdCustSim	stdCustSim;				
	   TwPhoneImsi	stwPhoneImsi;

    EXEC SQL END   DECLARE SECTION;
    
	
    /* �ֲ�������ʼ�� */
    init(TmpSqlStr);   init(pPhone_No);    init(pSim_No);
    init(vImsi_No);    init(pBelong_Code); init(pLogin_No);
    init(pTotal_Date); init(pSysDate);     init(vOrg_Code);  init(pGroupNo);  
    init(pOp_Code);    init(pId_No);	    init(vGroupId);
    init(vYearMonth);  init(oldSimGroupId); init(oldSimLoginNo);
	init(vbillCode);
	init(vgoodType);
	init(vPGroupId1);
	init(vRetMsg);
    memset(v_parameter_array, 0, sizeof(v_parameter_array));		/*ng_���� add by mengfy@08/04/2009*/
	memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
	memset(vImsi_No_o, 0, sizeof(vImsi_No_o));
	memset(&stwPhoneImsi, 0, sizeof(stwPhoneImsi));	
	memset(&stdCustSim, 0, sizeof(stdCustSim));				

    pubLog_Debug(_FFL_,"pubOpenAcctRes","","-------- �û������޸���Դ��Ϣ pubOpenAcctRes() BEGIN --------");

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
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Phone_No       = [%s]",   pPhone_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Sim_No         = [%s]",   pSim_No         );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Belong_code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","pOp_Code       = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","pTotal_Date    = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","pSysDate       = [%s]",   pSysDate        );
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","pGroupNo       = [%s]",   pGroupNo        );
#endif

    EXEC SQL SELECT Org_Code ,org_id,to_char(sysdate, 'yyyymm') /*group_id ->org_id edit by liuweib20091023*/
    INTO :vOrg_Code, :vGroupId, :vYearMonth
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
        if(SQLCODE!=0){
            pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ȡ������Ϣ����!");
           
            EXEC SQL ROLLBACK;
            return -1;
        }
    init(vGroupId);
    strcpy(vGroupId,pGroupNo);
        
    EXEC SQL SELECT Imsi_No ,group_Id, nvl(login_no, ' ') INTO :vImsi_No,:oldSimGroupId, :oldSimLoginNo 
             FROM   dSimRes
             WHERE  Sim_No=:pSim_No;
        if(SQLCODE!=0){
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
          pubLog_DBErr(_FFL_,"pubOpenAcctRes","-33","TmpSqlStr =[%s][%d]", TmpSqlStr, SQLCODE);
          pubLog_DBErr(_FFL_,"pubOpenAcctRes","-33","v1 = [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s] [%s]", pLogin_Accept,pTotal_Date,vGroupId,pLogin_No,pSysDate,pOp_Code,pLogin_No,vGroupId,pPhone_No);
#endif
            EXEC SQL ROLLBACK;
            return -33;
        }
        
        /* ���� dCustResHis �� */
    	/*EXEC SQL insert into dCustResHis(PHONE_NO , NO_TYPE , PASSWORD , OTHER_ATTR ,
                  RESOURCE_CODE , CHOICE_FEE , REGION_CODE , DISTRICT_CODE , 
                  TOWN_CODE  , BEGIN_TIME , OP_TIME , LOGINGIN_TIME , OP_TIME , LOGINGIN_TIME , OP_TIME , LOGINGIN_TIME , OP_TIME , LOGIN BEGIN_TIME , OP_TIME , LOGIN_NO , ORG_CODE , 
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
        	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����dCustResHis��ʧ��!");
        	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","���º�����Դ�����!");
           
            EXEC SQL ROLLBACK;
            return -1;
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
                    op_code 	=:pOp_Code
              WHERE Sim_No =:pSim_No; 
/** ������������ **
                AND Sim_Status='9'; 
**/
        if(SQLROWS!=1){
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
        EXEC SQL PREPARE prepare1 From :TmpSqlStr;
        EXEC SQL EXECUTE prepare1 USING :oldSimGroupId,:oldSimLoginNo,:pSim_No;
#ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenAcctRes","","dynStmt=[%s] +++ SqlCode = [%d]+++", TmpSqlStr,SQLCODE);
#endif
    if (SQLCODE != 0)
    {
                pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����wSimResOpr%s����[%d]!", vYearMonth, SQLCODE);
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
            pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ��ʷ�����!");
           
            EXEC SQL ROLLBACK;
            return -1;
        }

 	/*ng_���� add by mengfy@08/04/2009---*/
/*    EXEC SQL INSERT INTO dCustSim 
                        (Id_No,   Switch_No, Sim_No)
                  VALUES(TO_NUMBER(:pId_No), :vImsi_No, :pSim_No);
        if(SQLCODE!=0){
            pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ�����!");
           
            EXEC SQL ROLLBACK;
            return -1;
        }
*/
	memset(&stdCustSim, 0, sizeof(stdCustSim));
	
	strcpy(stdCustSim.sIdNo, pId_No);
	strcpy(stdCustSim.sSwitchNo, vImsi_No);
	strcpy(stdCustSim.sSimNo, pSim_No);
	
	pubLog_Debug(_FFL_, "", "", "pId_No=[%s]", pId_No);
	pubLog_Debug(_FFL_, "", "", "vImsi_No=[%s]", vImsi_No);
	pubLog_Debug(_FFL_, "", "", "pSim_No=[%s]", pSim_No);
	
	v_ret = 0;
	v_ret = OrderInsertCustSim(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", stdCustSim);
	if (0 != v_ret)
	{
		PUBLOG_DBDEBUG("");
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����SIM����Ӧ��ϵ�����!,v_ret=[%d]", v_ret);
       
        EXEC SQL ROLLBACK;
        return -1;
	}	
	
 	/*ng_���� add by mengfy@08/04/2009---*/
    /* 
    EXEC SQL INSERT INTO wPhoneImsi1
                   (Phone_No,    Imsi_No,     Op_Time,
                    Enable_Flag, Update_Code, Id_Code) 
            VALUES(:pPhone_No,  :vImsi_No,    
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                   '0',          0,          :pLogin_Accept);
       if (SQLCODE != 0) {
           pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi1) ����!");
           EXEC SQL ROLLBACK;
           return -1;
       }
    */
    /* ��ѯ��ˮ */
    EXEC SQL select sMaxIdCode.nextval into :sMax_IdCode from dual;
    if (SQLCODE != 0) {
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
    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","IMSI�ڹ�ϵ��Ӧ��(wPhoneImsi)����ʹ�õļ�¼");
    	EXEC SQL ROLLBACK;
    	return -1;
    }
    EXEC SQL insert into wPhoneImsiHis(ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, UPDATE_ACCEPT, 
                                       UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN, UPDATE_TYPE, UPDATE_CODE)
             select ID_CODE, PHONE_NO, IMSI_NO, BEGIN_TIME, END_TIME, to_number(:pLogin_Accept), 
                     TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),TO_NUMBER(:pTotal_Date),:pLogin_No,'d',:pOp_Code
             from wPhoneImsi where imsi_no=:vImsi_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��ʷ��(wPhoneImsiHis) imsiʧ��!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
	/*ng_���� add by mengfy@08/04/2009--- begin---*/
/*    EXEC SQL delete wPhoneImsi where imsi_no=:vImsi_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) imsiʧ��!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
*/
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	strcpy(v_parameter_array[0],vImsi_No);
	
	v_ret = 0;
	/*
	v_ret = OrderDeleteWPhoneImsi(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", vImsi_No, "", v_parameter_array);	*/
	v_ret = OrderDeleteOtherWPhoneImsi(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", vImsi_No, "", v_parameter_array);			
	if (v_ret < 0)
	{
		PUBLOG_DBDEBUG("");
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) imsiʧ��!,v_ret=[%d]",v_ret);
       
        EXEC SQL ROLLBACK;
        return -1;
	}		
	
 	/*ng_���� add by mengfy@08/04/2009--- end---*/
   
/*
    flag=0;
    EXEC SQL select count(*) into :flag from wPhoneImsi where phone_no=:pPhone_No and end_time>sysdate;
    if(flag>0)
    {
    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�ֻ������ڹ�ϵ��Ӧ��(wPhoneImsi)����ʹ�õļ�¼");
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
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��ʷ��(wPhoneImsiHis) phoneʧ��!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
	/*ng_���� add by mengfy@08/04/2009--- begin---*/
/*    EXEC SQL delete wPhoneImsi where phone_no=:pPhone_No;
    if (SQLCODE != 0 && SQLCODE != 1403) {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) phoneʧ��!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
 */
	init(sTempSqlStr);
	sprintf(sTempSqlStr,"select imsi_no from wPhoneImsi where phone_no = :v1 ");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
	EXEC SQL declare ngcur02 cursor for sql_str;
	EXEC SQL open ngcur02 using :pPhone_No;
	for(;;)
	{
		init(vImsi_No_o);
		EXEC SQL FETCH ngcur02 into :vImsi_No_o;
		if ((0 !=SQLCODE ) && (1403 != SQLCODE))
		{
			EXEC SQL CLOSE ngcur02;
			PUBLOG_DBDEBUG("pubOpenAcctRes");
			pubLog_DBErr(_FFL_, "pubOpenAcctRes", "", "������ʾδ����pPhone_No[%s]", pPhone_No);
			return -1; 
		}
		if (1403 == SQLCODE) break;
		strcpy(v_parameter_array[0],vImsi_No_o);
		strcpy(v_parameter_array[1],pPhone_No);
		
		v_ret = 0;
		/*
		v_ret=OrderDeleteWPhoneImsi(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", vImsi_No_o, "and phone_no=:pPhone_No", v_parameter_array);*/
		v_ret=OrderDeleteOtherWPhoneImsi(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", vImsi_No_o, "and phone_no=:pPhone_No", v_parameter_array);			
		if( v_ret < 0 )
		{
			EXEC SQL CLOSE ngcur02;
			PUBLOG_DBDEBUG("");
	        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ɾ���ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) phoneʧ��!v_ret=[%d]",v_ret);
	       
	        EXEC SQL ROLLBACK;
			return -1;
		}		
	}
	EXEC SQL CLOSE ngcur02;
 
	
 	/*ng_���� add by mengfy@08/04/2009--- end---*/
 	/*ng_���� add by mengfy@08/04/2009--- begin---*/

/*    EXEC SQL insert into wPhoneImsi(
                   ID_CODE   ,   PHONE_NO ,  IMSI_NO ,
                   BEGIN_TIME ,  END_TIME ) 
             values(to_number(:sMax_IdCode), :pPhone_No, :vImsi_No , 
                   TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                   to_date('20501230 01:01:01','YYYYMMDD HH24:MI:SS'));
    if (SQLCODE != 0) {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) ����!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
*/
	memset(&stwPhoneImsi, 0, sizeof(stwPhoneImsi));
	
	strcpy(stwPhoneImsi.sIdCode, sMax_IdCode);
	strcpy(stwPhoneImsi.sPhoneNo, pPhone_No);
	strcpy(stwPhoneImsi.sImsiNo, vImsi_No);
	strcpy(stwPhoneImsi.sBeginTime, pSysDate);
	strcpy(stwPhoneImsi.sEndTime, "20501230 01:01:01");

	v_ret = 0;
	v_ret = OrderInsertWPhoneImsi(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
			"pubOpenAcctRes", stwPhoneImsi);
	if (0 != v_ret)
	{
		PUBLOG_DBDEBUG("");
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","�����ƶ�������IMSI��ϵ��Ӧ��(wPhoneImsi) ����!,v_ret=[%d]", v_ret);
       
        EXEC SQL ROLLBACK;
        return -1;
	}

 	/*ng_���� add by mengfy@08/04/2009--- end---*/
	/*ng_���� add by mengfy@08/21/2009---*/
#ifdef _CHGTABLE_	
    flag=0;
    /*ng���� ��������-����-�ٿ��������������� by liuzhou at 20091021
    EXEC SQL select count(*) into :flag from wPhoneImsiChg where IMSI_NO=:vImsi_No and flag='2';
    if (SQLCODE != 0 && SQLCODE!=1403) 
    {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","��ѯwPhoneImsiChg��ʧ��!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
    if(flag>0)
    {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-2","�ÿ�Ŀǰ����ʹ�ã�����30����֮��ʹ�ã�������������!");
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-2","SQLERRMSG = [%s]",SQLERRMSG);
        EXEC SQL ROLLBACK;
        return -2;
    }
	*/
	
    EXEC SQL insert into wPhoneImsiChg(
                 ID_CODE  , PHONE_NO ,  IMSI_NO ,
                 BEGIN_TIME , END_TIME , FLAG )
             values(to_number(:sMax_IdCode), :pPhone_No, :vImsi_No , 
                 TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 
                 to_date('20501230 01:01:01','YYYYMMDD HH24:MI:SS') ,
                 '1');
    if (SQLCODE != 0) {
        pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","����wPhoneImsiChg����!");
       
        EXEC SQL ROLLBACK;
        return -1;
    }
#endif    /*_CHGTABLE_*/
	/*ng_���� add by mengfy@08/21/2009---*/
    
    /*���������޶�*/
    EXEC SQL SELECT bill_code,good_type INTO :vbillCode,:vgoodType FROM dgoodphoneres
    		 WHERE phone_no=:pPhone_No;
    if(SQLCODE != 0 && SQLCODE != 1403)
    {
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
/** ��֯�������� liuhao    200090330 begin*/    						
    						 phone_no,region_code,district_code,good_type,group_id
    						)
    					VALUES
    						(
    						 :pPhone_No,substr(:pBelong_Code,1,2),substr(:pBelong_Code,3,2),:vgoodType,:vGroupId
/** ��֯�������� liuhao    200090330 end*/    						 	
    						);
    		if(SQLCODE != 0)
		    {
		    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","INSERT INTO dgoodphonenolimit����!");
		       
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
    	}
    	else
    	{
    		/*--��֯���������ȡgroup_id edit by magang at 06/05/2009--begin*/
			ret1=0;
			ret1 = sGetGroupId(vGroupId,vPGroupId1,class_code,vRetMsg); 
			if(ret1 !=0)
   			{
				pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","ȡ���ڵ�ʧ��");
				return -1;
   		
   			}
    		EXEC SQL SELECT limit_fee INTO :ilimitFee FROM dreggoodbillcfg
    				 WHERE 
    				 /*--��֯����������� edit by magang at 12/05/2009--begin-
    				 region_code=substr(:pBelong_Code,1,2) 
    				 --��֯����������� edit by magang at 12/05/2009--end-*/
    				 GROUP_ID=:vPGroupId1
    				 AND bill_code=:vbillCode
    				 AND good_type=:vgoodType;
    		if(SQLCODE != 0)
		    {
		    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SELECT dreggoodbillcfg����!");
		       
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
		    
		    EXEC SQL SELECT limit_fee INTO :tlimitFee FROM dreggoodbillcfg
    				 WHERE     				  
    				 region_code='00' 
    				 AND use_flag='Y'
    				 AND good_type=:vgoodType;
    		if(SQLCODE != 0)
		    {
		    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","SELECT t dreggoodbillcfg����!");
		       
		        EXEC SQL ROLLBACK;
		        return -1;
		    }
		    
		    if(ilimitFee < tlimitFee)
		    {
		    	EXEC SQL INSERT INTO dgoodphonenolimit
    						(
/** ��֯�������� liuhao    200090330 begin*/    						
    						 phone_no,region_code,district_code,good_type,group_id
    						)
    					VALUES
    						(
    						 :pPhone_No,substr(:pBelong_Code,1,2),substr(:pBelong_Code,3,2),:vgoodType,:vGroupId
/** ��֯�������� liuhao    200090330 end*/    						 	
    						);
	    		if(SQLCODE != 0)
			    {
			    	pubLog_DBErr(_FFL_,"pubOpenAcctRes","-1","INSERT INTO dgoodphonenolimit����!");
			       
			        EXEC SQL ROLLBACK;
			        return -1;
			    }
		    }
    	}
    }
    
    pubLog_Debug(_FFL_,"pubOpenAcctRes","","------------ �û������޸���Դ��Ϣ pubOpenAcctRes() END --------");
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

    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","---------- �û����������ط���Ϣ pubOpenAcctFunc() BEGIN --------");
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Phone_No       = [%s]",   pPhone_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Function_Code  = [%s]",   pFunction_Code  );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Function_Type  = [%s]",   pFunction_Type  );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Favour_Month   = [%d]",   pFavour_Month   );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Add_No         = [%s]",   pAdd_No         );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Other_Attr     = [%s]",   pOther_Attr     );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Belong_code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Op_Code        = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","pTotal_Date    = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","pSysDate       = [%s]",   pSysDate        );
#endif

    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
			PUBLOG_DBDEBUG("pubOpenCustConfirm");	
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
			PUBLOG_DBDEBUG("pubOpenCustConfirm");	
            pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط���ʷ�����![%d]", SQLCODE);          
            EXEC SQL ROLLBACK;
            return -1;
        }

    EXEC SQL INSERT INTO dCustFunc 
                        (Id_No,Function_Type,Function_Code,Op_Time)
                  VALUES(TO_NUMBER(:pId_No),:pFunction_Type,:pFunction_Code,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'));
        if(SQLCODE!=0){
			PUBLOG_DBDEBUG("pubOpenCustConfirm");	
            pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط���Ϣ��dCustFunc����![%d][%s]", SQLCODE,sqlca.sqlerrm.sqlerrmc);
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
			PUBLOG_DBDEBUG("pubOpenCustConfirm");	
            pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����û������ط���ʷ��(dCustFuncAddHis)����");
           
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
            pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","���븽���ط���Ϣ��dCustFuncAdd ���� ");
           PUBLOG_DBDEBUG("pubOpenCustConfirm");	
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
				PUBLOG_DBDEBUG("pubOpenCustConfirm");	
                pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","���º�����Դ��(dCustRes) ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
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
                pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","����ĺ�ת����(wPhId) ����,");
               PUBLOG_DBDEBUG("pubOpenCustConfirm");	
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
				PUBLOG_DBDEBUG("pubOpenCustConfirm");		
                pubLog_DBErr(_FFL_,"pubOpenAcctFunc","-1","�����ط�������ˮ��(wCustFuncDay) ����,");        
            #endif
            EXEC SQL ROLLBACK;
            return -1;
        }                        
    
    
    pubLog_Debug(_FFL_,"pubOpenAcctFunc","","------------ �û����������ط���Ϣ pubOpenAcctFunc() END --------");
    return 0;

}

/**
  * Auth: PeiJH
  * Name: pubOpenAcctCheck_2
  * Func: �û�����֧Ʊ���ݴ���
  * Date: 2003/10/14
  * Stat: Test Pass
  */
int pubOpenAcctCheck_2(char *InCheck_No,  float InCheck_Pay,  char *InBank_Code,                       
                   char *InOp_Note,   char  *InLogin_No,  char InLogin_Accept[23],
                   char *InOp_Code,   char *InTotal_Date, char *InSysDate,
                   char *InSystem_Note)
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
       
       char vOrgId[10+1];
       int ret=0;
    EXEC SQL END   DECLARE SECTION;

    init(pCheck_No);   init(pBank_Code); init(pOp_Note);
    init(pOp_Code);    init(pLogin_No);  init(pTotal_Date); 
    init(pSysDate);    init(vOrg_Code);  init(pLogin_Accept);
    init(vOrgId);                                                
    pCheck_Pay=InCheck_Pay;
    memcpy(pLogin_Accept,   InLogin_Accept,  22);    
    memcpy(pCheck_No,       InCheck_No,      20);
    memcpy(pBank_Code,      InBank_Code,     5);
    memcpy(pOp_Note,        InOp_Note,       60);
    memcpy(pLogin_No,       InLogin_No,      6);
    memcpy(pOp_Code,        InOp_Code,       4);
    memcpy(pTotal_Date,     InTotal_Date,    8);
    memcpy(pSysDate,        InSysDate,       20);

    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","------------ �û�����֧Ʊ���� pubOpenAcctCheck_2() BEGIN --------");
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Check_No       = [%s]",   InCheck_No      );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Check_Pay      = [%.2f]", InCheck_Pay     );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Bank_Code      = [%s]",   InBank_Code     );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Op_Note        = [%s]",   InOp_Note       );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Login_Accept   = [%d]",   InLogin_Accept  );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Login_No       = [%s]",   InLogin_No      );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Op_Code        = [%s]",   InOp_Code       );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","Total_Date     = [%s]",   InTotal_Date    );
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","SysDate        = [%s]",   InSysDate       );

    
    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
             pubLog_DBErr(_FFL_,"pubOpenAcctCheck_2","-1","ȡ������Ϣ����!");
             return -1;
         }
    /** ��֯�������� liuhao    200090331 begin*/     
	ret = sGetLoginOrgid(pLogin_No,vOrgId);
	if(ret <0)
	{
		pubLog_DBErr(_FFL_,"pubOpenAcctCheck_2","-1","");
		return -1;

	}
	Trim(vOrgId);
	/** ��֯�������� liuhao    200090331 end*/
    EXEC SQL INSERT INTO wCheckOpr
                        (Total_Date,      Login_Accept,     Org_Code,
                         Login_No,        Op_Code,          Bank_Code,
                         Check_No,        Check_Pay,        Op_Time,
                         Op_Note,Org_id)
                  VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),   :vOrg_Code,
                         :pLogin_No,     :pOp_Code,        :pBank_Code,
                         :pCheck_No,     :pCheck_Pay,       
                          TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pOp_Note,:vOrgId);
    if (SQLCODE != 0) {
		PUBLOG_DBDEBUG("pubOpenAcctCheck_2");	
        pubLog_DBErr(_FFL_,"pubOpenAcctCheck_2","-1","����֧Ʊ������ˮ��(wCheckOpr) ���� ");
        EXEC SQL ROLLBACK;
        return -1;
    }

	/* ng����  ҵ�񹤵����� 
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","---- bank_code=[%s]",pBank_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","---- check_no =[%s]",pCheck_No);

    EXEC SQL SELECT Check_Prepay INTO :vCheck_Prepay
               FROM dCheckPrepay
              WHERE Bank_Code  = :pBank_Code
                AND Check_No   = :pCheck_No;             
    if (SQLCODE != 0) {
		PUBLOG_DBDEBUG("pubOpenAcctCheck_2");	
        pubLog_DBErr(_FFL_,"pubOpenAcctCheck_2","-1","��ѯ֧Ʊ����(dCheckPrepay) ����");
        EXEC SQL ROLLBACK;
        return -1;
    }                

    if(vCheck_Prepay >= InCheck_Pay){
        EXEC SQL UPDATE dCheckPrepay
                    SET Check_Prepay = Check_Prepay   - :pCheck_Pay,
                             Op_Time = TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') 
                  WHERE Bank_Code  = :pBank_Code
                    AND Check_No   = :pCheck_No;
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubOpenAcctCheck_2");	
            pubLog_DBErr(_FFL_,"pubOpenAcctCheck_2","-1","����֧Ʊ����(dCheckPrepay) �����֧Ʊ���� ");
            EXEC SQL ROLLBACK;
            return -1;
        }                    
    }
	*/
    pubLog_Debug(_FFL_,"pubOpenAcctCheck_2","","------------ �û�����֧Ʊ���� pubOpenAcctCheck_2() End --------");
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

     pubLog_Debug(_FFL_,"pubCBXExpire","","------------ ��Ч�ڴ��� pubCBXExpire() Begin --------");
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
        pubLog_Debug(_FFL_,"pubCBXExpire","","Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Prepay_Fee     = [%.2f]", pPrepay_Fee     );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Op_Note        = [%s]",   pOp_Note        );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_,"pubCBXExpire","","Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_,"pubCBXExpire","","Total_Date     = [%s]",   pTotal_Date     ); 
       	pubLog_Debug(_FFL_,"pubCBXExpire","","SysDate        = [%s]",   pSysDate        );     
        pubLog_Debug(_FFL_,"pubCBXExpire","","InSm_Code      = [%s]",   InSm_Code        );     
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
        pubLog_Debug(_FFL_,"pubCBXExpire","","vOrg_Code          = [%s]",   vOrg_Code          );
        pubLog_Debug(_FFL_,"pubCBXExpire","","sV_Region_code    = [%s]",   sV_Region_code    );
    #endif
    /*****    
    EXEC SQL SELECT expire_flag into :pExpire_Flag
             FROM   sSmCode
             WHERE  region_code=:sV_Region_code and sm_code=:InSm_Code;
         if(SQLCODE==1403)
         {
             printf("sSmCode��û�з�������������");
             return 0;
         } 
         else if(SQLCODE!=0){
             printf("ȡ��Ч�ڱ�־����!");
             return -1;
         }
printf("pExpire_Flag=[%s]",pExpire_Flag);
    if(strncmp(pExpire_Flag,"N",1)==0)
    {
    	printf("��Ч�ڱ�־ΪN������Ҫ������Ч�ڱ�");
    	return 0;
    }
    *****/
    
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
        pubLog_DBErr(_FFL_,"pubCBXExpire","-1","��ѯsPayTime����![%d]",SQLCODE);
        return -1;
    }
    if(SQLCODE==1403)
    {
    	return 0;
    }
            
    EXEC SQL INSERT INTO dCustExpire 
                        (Id_No,      Open_Time, Begin_Flag,
                         Begin_Time, Old_Expire,EXPIRE_TIME,
                         Bak_Field)
                  VALUES(TO_NUMBER(:pId_No),    
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                        'N',        TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS')+:vDays,
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS')+:vDays,
                        'NULL');
    if(SQLCODE !=0){
        #ifdef _DEBUG_
			PUBLOG_DBDEBUG("pubCBXExpire");	
            pubLog_DBErr(_FFL_,"pubCBXExpire","-1","����dCustExpire ����");
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
            pubLog_DBErr(_FFL_,"pubCBXExpire","-1","����wExpireList ���� ");
        #endif
        EXEC SQL ROLLBACK;
        return -1;
    }
    pubLog_Debug(_FFL_,"pubCBXExpire","","------------ ��Ч�ڴ��� pubCBXExpire() End --------");
    
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
            char *InSysDate,      char *InMsgErr)
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

    pubLog_Debug(_FFL_,"pubPack","","------------ Ӫ�������� pubPack() Begin --------");
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
        pubLog_Debug(_FFL_,"pubPack","","Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_,"pubPack","","pPhone_No      = [%s]",   pPhone_No       );
        pubLog_Debug(_FFL_,"pubPack","","Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_,"pubPack","","pMachine_Code  = [%s]",   pMachine_Code   );
        pubLog_Debug(_FFL_,"pubPack","","pPack_Id       = [%s]",   pPack_Id        );
        pubLog_Debug(_FFL_,"pubPack","","Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_,"pubPack","","pOrg_Code      = [%s]",   pOrg_Code       );
        pubLog_Debug(_FFL_,"pubPack","","Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_,"pubPack","","Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_,"pubPack","","Total_Date     = [%s]",   pTotal_Date     ); 
        pubLog_Debug(_FFL_,"pubPack","","SysDate        = [%s]",   pSysDate        );     
    #endif

    EXEC SQL select month_num into :vMonth_Num from sPackCode where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
		PUBLOG_DBDEBUG("pubPack");
        pubLog_DBErr(_FFL_,"pubPack","-1","��ѯsPackCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsPackCode��ʧ��!");
        return -1;
    }

    EXEC SQL insert into dPackMsg(id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time )
             select to_number(:pId_No) , :pPhone_No , :pBelong_Code , :pMachine_Code , :pPack_Id , 
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),add_months(to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),month_num) ,
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , :InMachine_Fee , Month_num , :pLogin_No , :pOrg_Code , 
                    to_number(:pLogin_Accept) , :pOp_Code , :pTotal_Date , to_date(:pSysDate,'yyyymmdd hh24:mi:ss')
             from sPackCode
             where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
		PUBLOG_DBDEBUG("pubPack");
        pubLog_DBErr(_FFL_,"pubPack","-1","����dPackMsg��ʧ��!");
        strcpy(InMsgErr,"����dPackMsg��ʧ��!");
        return -1;
    }
                    
    EXEC SQL insert into dPackMsgHis(id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , Update_accept , Update_time , Update_date , Update_login , 
                    Update_type , Update_code )
             select id_no , phone_no , Belong_code , Mach_code , pack_id , Begin_time , End_time , 
                    Prepay_fee , Should_pay , Sell_fee , Subsidy_fee , Machine_fee , Month_num , Login_no , Org_code , 
                    Login_accept , Op_code , Total_date , Op_time , to_number(:pLogin_Accept) , 
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss') , to_number(:pTotal_Date) , :pLogin_No , 'a' , :pOp_Code
             from dPackMsg
             where id_no=:pId_No;
    if(SQLCODE!=0)
    {
		PUBLOG_DBDEBUG("pubPack");
        pubLog_DBErr(_FFL_,"pubPack","-1","����dPackMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dPackMsgHis��ʧ��!");
        return -1;
    }

    EXEC SQL insert into dBillPackMsg(id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                                      Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , Total_date , Op_time )
             select to_number(:pId_No) , :pBelong_Code , :pMachine_Code , mode_code,mode_flag,mode_type,
                    to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),add_months(to_date(:pSysDate,'yyyymmdd hh24:mi:ss'),:vMonth_Num) ,
                    :pLogin_No , :pOrg_Code , to_number(:pLogin_Accept) , :pOp_Code , :pTotal_Date , to_date(:pSysDate,'yyyymmdd hh24:mi:ss')
             from sBillPackCode
             where region_code=substr(:pBelong_Code,1,2) and mach_code=:pMachine_Code;
    if(SQLCODE!=0)
    {
		PUBLOG_DBDEBUG("pubPack");
        pubLog_DBErr(_FFL_,"pubPack","-1","����dBillPackMsg��ʧ��!");
        strcpy(InMsgErr,"����dBillPackMsg��ʧ��!");
        return -1;
    }
    
    EXEC SQL insert into dBillPackMsgHis(id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                                         Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                                         Total_date , Op_time , Update_accept , Update_time , 
                                         Update_date , Update_login , Update_type , Update_code )
             select id_no , Belong_code , Mach_code , Mode_code , Mode_flag , Mode_type , 
                    Begin_time , End_time , Login_no , Org_code , Login_accept , Op_code , 
                    Total_date , Op_time , to_number(:pLogin_Accept) , to_date(:pSysDate,'yyyymmdd hh24:mi:ss') , 
                    to_number(:pTotal_Date) , :pLogin_No , 'a' , :pOp_Code
             from dBillPackMsg
             where id_no=:pId_No;
    if(SQLCODE!=0)
    {
		PUBLOG_DBDEBUG("pubPack");
        pubLog_DBErr(_FFL_,"pubPack","-1","����dBillPackMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dBillPackMsgHis��ʧ��!");
        return -1;
    }

    pubLog_Debug(_FFL_,"pubPack","","------------ Ӫ�������� pubPack() End --------");
    
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
                 char *InTrue_flag,char *sVCustinfoType,char *sV_Agreement_Type)
{
	
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
        char  pPay_Type[1+1]; /** ?? **/
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

	  	char vGroupId[10+1];
	  	char vOrgId[10+1];
	  	char sErrorMess[60];
      	char vChgFlag[1+1];
      	char vCustFlag[1+1];	  	
 		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];	/*ng_���� add by mengfy@08/04/2009--- begin---*/
		char pAssure_No[12+1];	
		char pAssure_Num[4+1];		
 	    int v_ret = 0;
		TdAssuMsg sTdAssuMsg;
		char sTempSqlStr[2024];
		TdConMark	sdConMark;
		TdConUserMsg sdConUserMsg;
		TdCustPost	stdCustPost;
		TdCustInnet	stdCustInnet;
		TdCustDoc	stdCustDoc;
		TdCustConMsg stdCustConMsg;				
		char vAssure_No_ng[12+1];
		char vSysdate[6+1];

    EXEC SQL END   DECLARE SECTION;
    tCustDoc  custDoc={0};	
	tLoginOpr vLog={0}; 
	tAccDoc   accDoc={0};
	tUserDoc custStruc={0};    
    int Vret=0;
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
    init(vOrgId);
    init(sErrorMess);
    init(vChgFlag);
    init(vCustFlag);
    memset(&sTdAssuMsg, 0, sizeof(sTdAssuMsg));		/*ng_����	add by mengfy */
    memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
    memset(&sdConMark, 0, sizeof(sdConMark));
    memset(&sdConUserMsg, 0, sizeof(sdConUserMsg));
    memset(&stdCustPost, 0, sizeof(stdCustPost));
    memset(&stdCustInnet, 0, sizeof(stdCustInnet));
    memset(&stdCustDoc, 0, sizeof(stdCustDoc));
    memset(&stdCustConMsg, 0, sizeof(stdCustConMsg));
    
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","-------- �û�����ȷ�� pubOpenAcctConfirm() BEGIN --------");

    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cust_Id        = [%s]",   InCust_Id        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Con_Id         = [%s]",   InCon_Id         );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Id_No          = [%s]",   InId_No          );
	pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cust_Passwd    = [%s]",   InCust_Passwd    );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Con_Passwd     = [%s]",   InCon_Passwd     );   
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","User_Passwd    = [%s]",   InUser_Passwd    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Limit_Owe      = [%.2f]", InLimit_Owe      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Credit_Value   = [%d]",   InCredit_Value   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Bill_Type      = [%d]",   InBill_Type      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cmd_Right      = [%d]",   InCmd_Right      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Login_Accept   = [%s]",   InLogin_Accept   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cash_Pay       = [%.2f]", InCash_Pay       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Check_Pay      = [%.2f]", InCheck_Pay      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Prepay_Fee     = [%.2f]", InPrepay_Fee     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sim_Fee        = [%.2f]", InSim_Fee        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Machine_Fee    = [%.2f]", InMachine_Fee    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Innet_Fee      = [%.2f]", InInnet_Fee      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Choice_Fee     = [%.2f]", InChoice_Fee     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Other_Fee      = [%.2f]", InOther_Fee      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Hand_Fee       = [%.2f]", InHand_Fee       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Phone_No       = [%s]",   InPhone_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sm_Code        = [%s]",   InSm_Code        ); 
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Service_Type   = [%s]",   InService_Type   );         
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Attr_Code      = [%s]",   InAttr_Code      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Credit_Code    = [%s]",   InCredit_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Pay_Code       = [%s]",   InPay_Code       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Pay_Type       = [%s]",   InPay_Type       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sim_No         = [%s]",   InSim_No         );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Belong_Code    = [%s]",   InBelong_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Op_Note        = [%s]",   InOp_Note        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","System_Note    = [%s]",   InSystem_Note    );    
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Login_No       = [%s]",   InLogin_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Ip_Address     = [%s]",   InIp_Address     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Op_Code        = [%s]",   InOp_Code        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Innet_Type     = [%s]",   InInnet_Type     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Machine_Code   = [%s]",   InMachine_Code   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Name    = [%s]",   InAssure_Name    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Id_Type        = [%s]",   InId_Type        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Id      = [%s]",   InAssure_Id      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Phone   = [%s]",   InAssure_Phone   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Address = [%s]",   InAssure_Address );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_ZIP     = [%s]",   InAssure_ZIP     );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Flag      = [%s]",   InPost_Flag      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Type      = [%s]",   InPost_Type      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Address   = [%s]",   InPost_Address   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_ZIP       = [%s]",   InPost_ZIP       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Fax       = [%s]",   InPost_Fax       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Mail      = [%s]",   InPost_Mail      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Content      = [%s]",   InPost_Content);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Total_Date     = [%s]",   InTotal_Date     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","SysDate        = [%s]",   InSysDate        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","In_NewOldFlag        = [%s]",   In_NewOldFlag        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","In_CustDocFlag        = [%s]",   In_CustDocFlag        );   

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
    memcpy(pCust_Passwd,    InCust_Passwd,   8);    
    memcpy(pCon_Passwd,     InCon_Passwd,    8);    
    memcpy(pUser_Passwd,    InUser_Passwd,   8);    
    memcpy(pPhone_No,       InPhone_No,     15);
    memcpy(pService_Type ,  InService_Type,  2);      
    memcpy(pAttr_Code,      InAttr_Code,     8);
    memcpy(pCredit_Code,    InCredit_Code,   1);
    memcpy(pPay_Code,       InPay_Code,      1);
    memcpy(pPay_Type,       InPay_Type,      1);
    memcpy(pSim_No,         InSim_No,        20);
    memcpy(pBelong_Code,    InBelong_Code,    7);
    memcpy(pAreaCode,    InAreaCode, 20);
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
    memcpy(pSm_Code ,       InSm_Code,       2); 
     
#ifdef _DEBUG_
	
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cust_Id        = [%s]",   pCust_Id        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Con_Id         = [%s]",   pCon_Id         );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Id_No          = [%s]",   pId_No          );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cust_Passwd    = [%s]",   pCust_Passwd    );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Con_Passwd     = [%s]",   pCon_Passwd     );   
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","User_Passwd    = [%s]",   pUser_Passwd    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Limit_Owe      = [%.2f]", pLimit_Owe      );
   	pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Credit_Value   = [%d]",   pCredit_Value   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Bill_Type      = [%d]",   pBill_Type      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cmd_Right      = [%d]",   pCmd_Right      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Login_Accept   = [%s]",   pLogin_Accept   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Cash_Pay       = [%.2f]", pCash_Pay       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Check_Pay      = [%.2f]", pCheck_Pay      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Prepay_Fee     = [%.2f]", pPrepay_Fee     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sim_Fee        = [%.2f]", pSim_Fee        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Machine_Fee    = [%.2f]", pMachine_Fee    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Innet_Fee      = [%.2f]", pInnet_Fee      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Choice_Fee     = [%.2f]", pChoice_Fee     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Other_Fee      = [%.2f]", pOther_Fee      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Hand_Fee       = [%.2f]", pHand_Fee       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Phone_No       = [%s]",   pPhone_No       );
   	pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sm_Code        = [%s]",   pSm_Code        ); 
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","pService_Type  = [%s]",   pService_Type   );     
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Attr_Code      = [%s]",   pAttr_Code      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Credit_Code    = [%s]",   pCredit_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Pay_Code       = [%s]",   pPay_Code       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Pay_Type       = [%s]",   pPay_Type       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Sim_No         = [%s]",   pSim_No         );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Belong_Code    = [%s]",   pBelong_Code    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","AreaCode    = [%s]",   pAreaCode    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Op_Note        = [%s]",   pOp_Note        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","System_Note    = [%s]",   pSystem_Note    );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Login_No       = [%s]",   pLogin_No       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Ip_Address     = [%s]",   pIp_Address     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Op_Code        = [%s]",   pOp_Code        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Innet_Type     = [%s]",   pInnet_Type     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Machine_Code   = [%s]",   pMachine_Code   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Name    = [%s]",   pAssure_Name    );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Id_Type        = [%s]",   pId_Type        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_No      = [%s]",   pAssure_Id      );        
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Phone   = [%s]",   pAssure_Phone   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_Address = [%s]",   pAssure_Address );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Assure_ZIP     = [%s]",   pAssure_ZIP     );    
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Flag      = [%s]",   pPost_Flag      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Type      = [%s]",   pPost_Type      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Address   = [%s]",   pPost_Address   );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_ZIP       = [%s]",   pPost_ZIP       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Fax       = [%s]",   pPost_Fax       );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Mail      = [%s]",   pPost_Mail      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Post_Content      = [%s]",   pPost_Content      );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Total_Date     = [%s]",   pTotal_Date     );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","SysDate        = [%s]",   pSysDate        );
    
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_Cust_Name        = [%s]",   p_Cust_Name        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_Cust_Address        = [%s]",   p_Cust_Address        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_Id_Type        = [%s]",   p_Id_Type        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_Id_Iccid        = [%s]",   p_Id_Iccid        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_NewOldFlag        = [%s]",   p_NewOldFlag        );
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","p_In_CustDocFlag        = [%s]",   p_In_CustDocFlag        );
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
             pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","ȡ������Ϣ����!");
             return -1;
         }

    Trim(vOrg_Code);
	strcpy(vGroupId,pGroupNo);
	
	
	if(sGetLoginOrgid(pLogin_No,vOrgId) !=0)
	{
		pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
		return -1;
				
	}         

    Trim(vOrg_Code);
	Trim(vOrgId);
    EXEC SQL SELECT SUBSTR(:pTotal_Date,1,6) INTO :vDateChar
             FROM   DUAL;

    EXEC SQL SELECT DECODE(SUBSTR(:pBelong_Code,1,2), '99',SUBSTR(:vOrg_Code
,1,2),   SUBSTR(:pBelong_Code,1,2)),
                    DECODE(SUBSTR(:pBelong_Code,3,2), '99',SUBSTR(:vOrg_Code
,3,2),   SUBSTR(:pBelong_Code,3,2)),
                    DECODE(SUBSTR(:pBelong_Code,5,3), '999',SUBSTR(:vOrg_Code
,5,3),  SUBSTR(:pBelong_Code,5,3))
               INTO :vRegion_Code,:vDistrict_Code,:vTown_Code
               From dual;
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-2","��ѯdLoginMsg ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-2","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -2;
        }
/*
    if(strcmp(pSm_Code,"cb")==0)
    {
        EXEC SQL select password into :sV_Cb_passwd from dCustRes where phone_no=:pPhone_No;
        if(SQLCODE!=0)
        {
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","��ȡ�û�����ʧ��[%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -2;
        }

        Trim(sV_Cb_passwd);
        if(strlen(sV_Cb_passwd)==0)
        {
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","�û����벻��Ϊ��");
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
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-3","����dCustInnetHis�������");
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
                PUBLOG_DBDEBUG("pubOpenAcctConfirm");;
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-4","����dCustInnetHis�������");
                EXEC SQL ROLLBACK;
                return -4;
            }

    if(strncmp(p_NewOldFlag,"1",1)!=0) /* ���ǲ��Ͽͻ��Ͳ�dCustDoc�� */
    {
        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","���ǲ��Ͽͻ��Ͳ�dCustDoc��  ");
        
        strcpy(vParent_Id,pId_No);
        
        EXEC SQL SELECT COUNT(*) INTO :sV_Cust_num FROM dCustDoc where cust_id=:pCust_Id;
        if(sV_Cust_num == 0) 
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","sV_true_flag        = [%s]",   sV_true_flag        );
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","sIn_Agreement_Type        = [%s]",   sIn_Agreement_Type        );
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","sVCustinfoType        = [%s]",   sVCustinfoType        );      
            /*
            sprintf(sV_SqlStr,"insert into dCustDoc(Cust_Id,Region_Code,District_Code,Town_Code,Cust_Name,Cust_Passwd,\
                    Cust_Status,Run_Time,Owner_grade,Owner_Type,Cust_Address,Id_Type,Id_Iccid,\
                    Id_Address, Id_Validdate,Contact_person,Contact_Phone,Contact_Address,Contact_Post,\
                    Contact_Mailaddress,Contact_Fax,Contact_Emaill,Org_Code,Create_Time,\
                    Close_Time,Parent_Id,true_flag,info_type,agreement_type) \
                VALUES(to_number(:pCust_Id) , :vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,\
                    '01',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'00','01',:p_Cust_Address,:p_Id_Type,:p_Id_Iccid,\
                    'NULL',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'NuLL','Null','Null','Null',\
                    'Null','Null','Null',:vOrg_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),\
                    TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),to_number(:pCust_Id),:sV_true_flag,:sVCustinfoType,:sIn_Agreement_Type) ");
                
            #ifdef _DEBUG_
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","���� dCustDoc  :sV_SqlStr=[%s]", sV_SqlStr);
            #endif   
       
            EXEC SQL PREPARE ins_stmt From :sV_SqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCust_Id,:vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,
                                            :pSysDate,:p_Cust_Address ,:p_Id_Type,:p_Id_Iccid,:pSysDate,:vOrg_Code,
                                            :pSysDate,:pCust_Id,:sV_true_flag,:sVCustinfoType,:sIn_Agreement_Type;
        
            if (SQLCODE != 0)
            {
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����ͻ�������ʷ��(dCustDoc)����[%d]",SQLCODE);
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
                EXEC SQL ROLLBACK;
                return -5;
            }
        
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","begin  insert into dCustDocHis");
            init(sV_SqlStr);
      		*/
 			/*edit organizational liuhao 20090331 begin*/
		    custDoc.vCust_Id=    atol(pCust_Id);
	        strncpy(custDoc.vRegion_Code		,vRegion_Code,2);
	        strncpy(custDoc.vDistrict_Code	,vDistrict_Code,2);
	        strncpy(custDoc.vTown_Code		,vTown_Code,3);
	        strncpy(custDoc.vOrg_Code		,vOrg_Code,9);
	        strncpy(custDoc.vGroup_Id		,vGroupId,10);
	        strncpy(custDoc.vOrg_Id			,vOrgId,10);
	        strncpy(custDoc.vCust_Name		,p_Cust_Name,60);
	        /*strncpy(custDoc.vCust_Passwd		,pCust_Passwd,8);*/
	        
	        /*********ng ���� �ͻ����벻��Ϊ��ֵ***************/
	        if(strlen(pCust_Passwd) == 0)
	        	strcpy(custDoc.vCust_Passwd, "    ");
	        else
	        	strncpy(custDoc.vCust_Passwd		,pCust_Passwd,8);	  
	        /*********ng ���� �ͻ����벻��Ϊ��ֵ***************/
	        	        	
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

      		/***********ng ����  ����id_typeһ��, �滻createCustDoc_ng****************/      		      		
      		/*if (createCustDoc(&custDoc,sErrorMess) <0 )*/
      		if(createCustDoc_ng(ORDERIDTYPE_USER, pId_No, &custDoc,sErrorMess) != 0)
      		{
      			pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
      			return -1;
      		}
      		 /*edit organizational liuhao 20090331 end*/	
      		/*
           sprintf(sV_SqlStr,"insert into dCustDocHis(Cust_Id,Region_Code,District_Code,Town_Code,Cust_Name,Cust_Passwd,\
                    Cust_Status,Run_Time,Owner_grade,Owner_Type,Cust_Address,Id_Type,Id_Iccid,\
                    Id_Address, Id_Validdate,Contact_person,Contact_Phone,Contact_Address,Contact_Post,\
                    Contact_Mailaddress,Contact_Fax,Contact_Emaill,Org_Code,Create_Time,\
                    Close_Time,Parent_Id,UPDATE_ACCEPT,\
                    UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE) \
                VALUES(to_number(:pCust_Id) , :vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,:pCust_Passwd,\
                    '01',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'00','01',:p_Cust_Address,:p_Id_Type,:p_Id_Iccid,\
                    'NULL',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),'NuLL','Null','Null','Null',\
                    'Null','Null','Null',:vOrg_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),\
                    TO_DATE('20500101','YYYYMMDD HH24:MI:SS'),to_number(:pCust_Id),to_number(:pLogin_Accept), \
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),to_number(:pTotal_Date),:pLogin_No,'a',:pOp_Code) ");
            
            #ifdef _DEBUG_
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","���� dCustDocHis  :sV_SqlStr=[%s]", sV_SqlStr);
            #endif   
        
            EXEC SQL PREPARE ins_stmt From :sV_SqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCust_Id,:vRegion_Code,:vDistrict_Code,:vTown_Code,:p_Cust_Name,
                                            :pCust_Passwd,:pSysDate,:p_Cust_Address ,:p_Id_Type,:p_Id_Iccid,:pSysDate,
                                            :vOrg_Code,:pSysDate,:pCust_Id  ,:pLogin_Accept,:pSysDate,:pTotal_Date,
                                            :pLogin_No,:pOp_Code;  
           
            if (SQLCODE != 0) 
            {
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����ͻ����ϱ�(dCustDocHis) ���� [%d]",SQLCODE);
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
                EXEC SQL ROLLBACK;
                return -6;
            }   */   
        }    
    }
    
    /* �����ʼ�û�������Ϣ */
/**********2006-10-24 tongqp����**********/
 	/*ng_���� add by mengfy@08/04/2009---*/
/*	EXEC SQL insert into dconmark
				   (CONTRACT_NO,INIT_POINT,CURRENT_POINT,CURRENT_TIME,
				    MONTH_POINT,YEAR_POINT,LASTYEAR_POINT,ADD_POINT,OWE_POINT,LAST_POINT,
				    MONTH_USED,YEAR_USED,TOTAL_USED,MIN_YM,CONTRACT_STATUS,OWE_LIMIT,STATUS_TIME)
             values(TO_NUMBER(:pCon_Id), 0 , 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                	0 , 0 , 0 , 0 , 0 , 0 ,
                	0 , 0 , 0 , TO_char(sysdate,'YYYYMM') , 0 , 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'));
             if (SQLCODE != 0) 
             {	
				PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-7","���� dconmark ����� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
                  EXEC SQL ROLLBACK;
                  return -7;
             }
*/
			memset(&sdConMark, 0, sizeof(sdConMark));
			
	
			strncpy(sdConMark.sContractNo, pCon_Id, 14);
			strcpy(sdConMark.sInitPoint, "0");
			strcpy(sdConMark.sCurrentPoint, "0");
			strncpy(sdConMark.sCurrentTime, pSysDate,17);
			strcpy(sdConMark.sMonthPoint, "0");
			strcpy(sdConMark.sYearPoint, "0");
			strcpy(sdConMark.sLastyearPoint, "0");
			strcpy(sdConMark.sAddPoint, "0");
			strcpy(sdConMark.sOwePoint, "0");
			strcpy(sdConMark.sLastPoint, "0");
			strcpy(sdConMark.sMonthUsed, "0");
			strcpy(sdConMark.sYearUsed, "0");
			strcpy(sdConMark.sTotalUsed, "0");
			strncpy(sdConMark.sMinYm, pSysDate,6);
			strcpy(sdConMark.sContractStatus, "0");
			strcpy(sdConMark.sOweLimit, "0");
			strncpy(sdConMark.sStatusTime, pSysDate,17);
			strcpy(sdConMark.sChgFlag,"1");	/*09����ϵͳ�������� add by majha at 20091030*/

			pubLog_Debug(_FFL_, "", "", "pCon_Id=[%s]",pCon_Id);
			pubLog_Debug(_FFL_, "", "", "pSysDate=[%s]",pSysDate);
		
			v_ret = 0;
			v_ret = OrderInsertConMark(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No,
					 pOp_Note,sdConMark);
			if (0 != v_ret)
			{
				PUBLOG_DBDEBUG("pubOpenAcctConfirm");
				pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-7","���� dconmark ����� [%d][%s],v_ret=[%d]",SQLCODE,sqlca.sqlerrm.sqlerrmc, v_ret);
				EXEC SQL ROLLBACK;
				return -7;
			}

 	/*ng_���� add by mengfy@08/04/2009---*/
/**********2006-10-24 tongqp����**********/

/*************2006-10-24 tongqp����**********
    EXEC SQL insert into dMarkMsg(ID_NO , OPEN_TIME , INIT_POINT , CURRENT_POINT,
                 YEAR_POINT , OP_TIME , BEGIN_TIME , ADD_POINT , TOTAL_USED ,
                 TOTAL_PRIZE , TOTAL_PUNISH , LASTYEAR_POINT , BASE_POINT,SM_CODE)
             values(TO_NUMBER(:pId_No),TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'), 0 , 0 ,
                 0 , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') , TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS') , 0 , 0 ,
                 0 , 0 , 0 , 0,:pSm_Code);
             if (SQLCODE != 0) {
                  pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","���� dMarkMsg ����� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
                  EXEC SQL ROLLBACK;
                  return -7;
             }
*************2006-10-24 tongqp����**********/
    vIds=1;
pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","pService_Type=[%s]",pService_Type);
   /* EXEC SQL INSERT INTO dCustMsgHis
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����ͻ�������ʷ��dCustMsgHis ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustMsg ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -8;
        }*/
   		/*edit organizational liuhao 20090331 begin*/	
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
		/*strncpy(custStruc.vUser_Passwd  ,  pUser_Passwd,8); */
		
	    /*********ng ���� �ͻ����벻��Ϊ��ֵ***************/
	    if(strlen(pUser_Passwd) == 0)
			strcpy(custStruc.vUser_Passwd, "    ");
		else
			strncpy(custStruc.vUser_Passwd  ,  pUser_Passwd,8); 				  
	    /*********ng ���� �ͻ����벻��Ϊ��ֵ***************/		
		  
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
		
	   Vret = createCustMsg(&custStruc,sErrorMess);
	   if(Vret != 0)
	   	{
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
		    return -1;
	   	}           
        /*edit organizational liuhao 20090331 begin*/   
     
        
	EXEC SQL INSERT INTO dinnetmsg
                        (PHONE_NO,ID_NO,CUST_ID,CONTRACT_NO,SM_CODE,
                        MODE_CODE,OPEN_TIME,GROUP_NO,FLAG)
                  VALUES(:pPhone_No,TO_NUMBER(:pId_No),TO_NUMBER(:pCust_Id),
                         TO_NUMBER(:pCon_Id),:pSm_Code,'00000000',TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         :pGroupNo,'0');
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-37","����dinnetmsg ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -37;
        }
        
    /* ng���� ҵ�񹤵����� 
    sprintf(sV_SqlStr,"INSERT INTO dCustOweStop%d(ID_NO, CONTRACT_NO, RUN_CODE, RUN_TIME, CALLING_TIMES, \
                           LOGIN_ACCEPT, AWAKE_TIMES, UNBILL_FEE) \
                       VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pCon_Id),'AA',sysdate,0,0,0,0)",atol(pId_No)%100);
    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","sV_SqlStr=[%s]",sV_SqlStr);
    EXEC SQL PREPARE ins_stmt From :sV_SqlStr;
    EXEC SQL EXECUTE ins_stmt using :pId_No,:pCon_Id;
    if (SQLCODE != 0) 
    {
		PUBLOG_DBDEBUG("pubOpenAcctConfirm");
        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-8","����dCustOweStop ���� [%s]",sqlca.sqlerrm.sqlerrmc);
        EXEC SQL ROLLBACK;
        return -8;
    }
    */


/*******
 ����ϵͳ�ṩ�������� op_no
 Op_Type: 1 Ins , 2 Upd , 3 Del
********/
    EXEC SQL SELECT sMaxBillChg.NEXTVAL INTO :vOp_No
             FROM DUAL;
        if(SQLCODE != 0){
			PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-9","ȡ����ӿ���ˮsMaxBillChg,���� [%d]",SQLCODE);
            return -9;
        }

    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","op_no=[%ld]",vOp_No);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","id_no=[%s]",pId_No);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","Parent_Id=[%s]",vParent_Id);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","pCust_Id=[%s]",pCust_Id);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","phone_no=[%s]",pPhone_No);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","sm_code=[%s]",pSm_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","attr_code=[%s]",pAttr_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","belong_code=[%s]",pBelong_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","sysdate=[%s]",pSysDate);
        /*edit organizational liuhao 20090331 begin*/   

	/*ng_���� add by mengfy@08/21/2009---*/
#ifdef _CHGTABLE_	
    EXEC SQL INSERT INTO wUserMsgChg
                        (Op_No,        Op_Type,       Id_No,
                         Cust_Id,      Phone_No,      Sm_Code,
                         Attr_Code,    Belong_Code,   Run_Code,
                         Run_Time,     Op_Time,group_id)
                  VALUES(:vOp_No,     '1',          TO_NUMBER(:pId_No),
                          TO_NUMBER(:pCust_Id),:pPhone_No,:pSm_Code, 
                         :pAttr_Code, :pBelong_Code, 'AA',
                          SUBSTR(:pSysDate,1,8)||SUBSTR(:pSysDate,10,2)
                       ||SUBSTR(:pSysDate,13,2)||SUBSTR(:pSysDate,16,2),
                          SUBSTR(:pSysDate,1,8)||SUBSTR(:pSysDate,10,2)
                       ||SUBSTR(:pSysDate,13,2)||SUBSTR(:pSysDate,16,2),
                        :vGroupId);
        /*edit organizational liuhao 20090331 end*/   
                        	
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-10","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -10;
        }
#endif	/*_CHGTABLE_*/
	/*ng_���� add by mengfy@08/21/2009---*/
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dConMsgHis ���� [%d]",SQLCODE);
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dConMsg ���� [%d]",SQLCODE);
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -12;
        }*/
/*********��֯�������쿪����������  edit by liuhao at 03/31/2009 begin*******/
    
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
        /*********ng ���� ����id_typeһ��****************/
        /*Vret = createConMsg(&accDoc,sErrorMess); */      
        Vret = createConMsg_ng(ORDERIDTYPE_USER, pId_No, &accDoc,sErrorMess);
        if(Vret != 0)
        {/*
			sprintf(return_code, "%06d",Vret);
			goto end_valid;
			*/
			pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
			return -1;
			
        }        
 /*********��֯�������쿪����������  edit by liuhao at 03/31/2009 end*******/
 
    
    EXEC SQL select to_char(count(*)) into :vPayFlag from sPackCode where region_code=:vRegion_Code and mach_code=:pMachine_Code;
    if(atoi(vPayFlag)>0) /* ��Ӫ����Ԥ�� */
        strcpy(vPayFlag,"j");
    else if(strcmp(pSm_Code,"d0")==0) /* ���ݿ� */
        strcpy(vPayFlag,"k");
    else
        strcpy(vPayFlag,"0");
      
    /* ng���� ҵ�񹤵�����    
    EXEC SQL INSERT INTO dConMsgPre(CONTRACT_NO,PAY_TYPE,PREPAY_FEE,LAST_PREPAY,ADD_PREPAY,LIVE_FLAG,ALLOW_PAY,
                    BEGIN_DT,END_DT)
                VALUES(TO_NUMBER(:pCon_Id),:vPayFlag,:pPrepay_Fee,0,:pPrepay_Fee,'0',0,
                    substr(:pSysDate,1,8),'20200101');
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-34","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -34;
        }
    */

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
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-13","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -13;
        }

	/*
    EXEC SQL INSERT INTO dCustConMsg
                        (Cust_Id,  Contract_No,  Begin_Time,
                         End_Time)
                  VALUES(TO_NUMBER(:pCust_Id),TO_NUMBER(:pCon_Id),    
                         TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                         TO_DATE('20500101','YYYYMMDD'));
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-14","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -14;
        }
     */
    memset(&stdCustConMsg, 0, sizeof(stdCustConMsg));
	strncpy(stdCustConMsg.sCustId, pCust_Id, 10);
	strncpy(stdCustConMsg.sContractNo, pCon_Id, 14);
	strncpy(stdCustConMsg.sBeginTime, pSysDate, 17);
	strncpy(stdCustConMsg.sEndTime, "20500101", 8);
	v_ret = 0;
    v_ret=OrderInsertCustConMsg(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
								pOp_Note, stdCustConMsg);
						     
	if (v_ret!=0)
	{
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-14","����dCustConMsg ����!,ret=[%d]",v_ret);                                     
            EXEC SQL ROLLBACK;
            return -14;		  	
	}	     
    

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
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-15","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -15;
        }

 	/*ng_���� add by mengfy@08/04/2009---*/
/*    EXEC SQL INSERT INTO dConUserMsg
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
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");;
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-16","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -16;
        }
*/
		EXEC SQL SELECT to_char(sysdate,'hh24miss') 
				 INTO :vSysdate
				 FROM dual;
		if (SQLCODE != 0)
		{
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");;
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-16","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -47;			
		}
		memset(&sdConUserMsg, 0, sizeof(sdConUserMsg));
		
		strncpy(sdConUserMsg.sIdNo, pId_No, 14);
		strncpy(sdConUserMsg.sContractNo, pCon_Id, 14);
		strcpy(sdConUserMsg.sBillOrder, "99999999");
		strcpy(sdConUserMsg.sSerialNo, "0");
		strcpy(sdConUserMsg.sPayOrder, "1");
		strncpy(sdConUserMsg.sBeginYmd, pSysDate, 8);
		strncpy(sdConUserMsg.sBeginTm, vSysdate, 6);
		strcpy(sdConUserMsg.sEndYmd, "20500101");
		strcpy(sdConUserMsg.sEndTm, "000000");
		strcpy(sdConUserMsg.sLimitPay, "0");
		strcpy(sdConUserMsg.sRateFlag, "N"); 
		strcpy(sdConUserMsg.sStopFlag, "Y");
	
		v_ret = 0;
		v_ret = OrderInsertConUserMsg(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No, 
				pOp_Note, sdConUserMsg);
		if (0 != v_ret)
		{
	        PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-16","����dCustInnetHis�������,v_ret=[%d]",v_ret);
	        EXEC SQL ROLLBACK;
	        return -16;
		}			
 	/*ng_���� add by mengfy@08/04/2009---*/
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-17","����dCustInnetHis�������");
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-18","ȡ����ӿ���ˮsMaxBillChg,���� [%d]",SQLCODE);
            return -18;
        }
    
    /*ng_���� add by mengfy@08/21/2009---*/
/**chendh CRM3.5�������ڵ���,sundm���,2012.3.17**/
/**
**#ifdef _CHGTABLE_
**		EXEC SQL INSERT INTO wConUserChg
**			(Op_No,Op_Type,Id_No,
**			Contract_No, Bill_Order, Conuser_Update_Time,
**			Limit_Pay,Rate_Flag,Fee_Code,
**			Fee_Rate,Op_Time,DETAIL_CODE)
**			VALUES(:vOp_No,'10',TO_NUMBER(:pId_No),
**			TO_NUMBER(:pCon_Id),99999999,
**			to_char(sysdate,'YYYYMMDDHH24MISS'),
**			0,	'N',	' ',	
**			0,
**			to_char(sysdate,'YYYYMMDDHH24MISS'),'*');
**		if (SQLCODE != 0){
**			PUBLOG_DBDEBUG("pubOpenAcctConfirm");
**			pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-19","����wConUserChg ����[%s]",sqlca.sqlerrm.sqlerrmc);
**			EXEC SQL ROLLBACK;
**			return -19;
**		}
**#endif	 
**/

    /*ng_���� add by mengfy@08/21/2009---*/ 
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
                             :pOp_Code,     'a');
            if (SQLCODE != 0) {
                PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-20","����dCustInnetHis�������");
                EXEC SQL ROLLBACK;
                return -20;
            }
 	/*ng_���� add by mengfy@08/04/2009---*/
/*        EXEC SQL INSERT INTO dCustPost
                            (Contract_No,    Post_Flag,   Post_Type,
                             Post_Address,   Post_ZIP,    Fax_No,
                             Mail_Address, post_cont_code,op_time)
                      VALUES(TO_NUMBER(:pCon_Id),:pPost_Flag,:pPost_Type,
                             :pPost_Address, :pPost_ZIP,  :pPost_Fax,
                             :pPost_Mail, :pPost_Content,sysdate);
            if (SQLCODE != 0) {
                PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-21","����dCustInnetHis�������");
                EXEC SQL ROLLBACK;
                return -21;
            }
*/		
		memset(&stdCustPost, 0, sizeof(stdCustPost));
		
		strncpy(stdCustPost.sContractNo, pCon_Id, 14);
		strcpy(stdCustPost.sPostFlag, pPost_Flag);
		strcpy(stdCustPost.sPostType, pPost_Type);
		strncpy(stdCustPost.sPostAddress, pPost_Address, 200);
		strncpy(stdCustPost.sPostZip, pPost_ZIP, 6);
		strncpy(stdCustPost.sFaxNo, pPost_Fax, 30);
		strncpy(stdCustPost.sMailAddress, pPost_Mail, 30);
		strncpy(stdCustPost.sPostContCode, pPost_Content, 1);
		strncpy(stdCustPost.sOpTime, pSysDate, 17);

		v_ret = 0;
		v_ret = OrderInsertCustPost(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No,
				pOp_Note, stdCustPost);
		if (v_ret != 0)
		{
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-21","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -21;
		}		
 	/*ng_���� add by mengfy@08/04/2009---*/
    }

    /* ���µ�����Ϣ���еĵ������� */
    if(strlen(ltrim(rtrim(pAssure_Id)))!=0)
    {
		/*ng_���� add by mengfy@08/04/2009---*/
 /*       EXEC SQL update dAssuMsg set Assure_Num = Assure_Num +1
                 where Assure_Id = :pAssure_Id 
                     and  Id_Type   = :pId_Type;  
                     
        if ((SQLCODE != 0)&&(SQLCODE != 1403))
        {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-22"," SQLERRMSG = [%s]" , SQLERRMSG);
            return -22;		
        }
        
        if(SQLCODE == 1403)
        {
*/             /* ���뵣����Ϣ */ 
 /*            EXEC SQL SELECT sMaxAssuNo.NEXTVAL INTO :vAssure_No FROM DUAL;
             if(SQLCODE!=0){
				PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","ȡ������ vAssure_No,����");
                 return -1;
             }	
             pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","vAssure_No =[%d] ",vAssure_No); 
             
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
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-23","����dCustInnetHis�������");
                 return -23;
             }	
             
             EXEC SQL INSERT INTO dAssuMsg(
                           Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address,   
                           Assure_ZIP,   Assure_Num)
                      VALUES(:vAssure_No,  :pAssure_Name,  :pId_Type,
                           :pAssure_Id,  :pAssure_Phone, :pAssure_Address,  
                           :pAssure_ZIP, 1);   
             if(SQLCODE!=0){
                 PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis�������");
                 return -24;
             }	   
        
        }
        else
        {
             EXEC SQL select assure_no into :vAssure_No from dAssuMsg 
                 where Assure_Id = :pAssure_Id 
                     and  Id_Type   = :pId_Type; 
             if(SQLCODE!=0){
                 PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis�������");
                 return -24;
             }	   
*/             /* ������ʷ�� */
/*             EXEC SQL INSERT INTO dAssuMsgHis(
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
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-25","����dCustInnetHis�������");
                 return -25;
              }	  
        }   
*/
		EXEC SQL SELECT assure_no INTO :vAssure_No 
				 FROM	dAssuMsg 
                 WHERE	trim(Assure_Id) = trim(:pAssure_Id) 
                 AND	trim(Id_Type)   = trim(:pId_Type); 
        if ((SQLCODE != 0) && (SQLCODE != 1403))
        {
             PUBLOG_DBDEBUG("pubOpenAcctConfirm");
             pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis�������");
             return -24;
        } else if (SQLCODE == 1403)
        {
        	 /* ���뵣����Ϣ */ 
             EXEC SQL SELECT sMaxAssuNo.NEXTVAL INTO :vAssure_No FROM DUAL;
             if(SQLCODE!=0){
				PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","ȡ������ vAssure_No,����");
                 return -1;
             }	
             pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","vAssure_No =[%d] ",vAssure_No); 
             
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
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-23","����dCustInnetHis�������");
                 return -23;
             }	
             
			/* ng_���� add by mengfy@08/04/2009---begin---*/	
/*             EXEC SQL INSERT INTO dAssuMsg(	
                           Assure_No,    Assure_Name,    Id_Type,
                           Assure_Id,    Assure_Phone,   Assure_Address, 	  
                           Assure_ZIP,   Assure_N	um)
                      VALUES(:vAssure_No,  :pAssure_Name,  :pId_Type,
                           :pAssure_Id,  :pAssure_Phone, :pAssure_Addre	ss,  
                           :pAssure_ZIP, 	1);   
             if(SQLCODE!=0){
                 PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis��	�����");
                 return -24;
   	   	       }
*/
				memset(&sTdAssuMsg, 0, sizeof(sTdAssuMsg));
				
				sprintf(sTdAssuMsg.sAssureNo, "%ld", vAssure_No);
				strcpy(sTdAssuMsg.sAssureName, pAssure_Name);		       
				strcpy(sTdAssuMsg.sIdType, pId_Type);		       
				strcpy(sTdAssuMsg.sAssureId, pAssure_Id);		       
				strcpy(sTdAssuMsg.sAssurePhone, pAssure_Phone);		       
				strcpy(sTdAssuMsg.sAssureZip, pAssure_ZIP);		       
				strcpy(sTdAssuMsg.sAssureAddress, pAssure_Address);		       
				strcpy(sTdAssuMsg.sAssureNum, "1");			       
	
				v_ret = 0;
				v_ret = OrderInsertAssuMsg(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), 
						pLogin_No, pOp_Note, sTdAssuMsg);	
				if (v_ret != 0)
				{
	                 PUBLOG_DBDEBUG("pubOpenAcctConfirm");
	                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis�������v_ret=[%d]",v_ret)	;
	                 return -24;
				}	    
			/* ng_���� add by mengfy@08/04/2009---end---*/

        } else
        {
        	memset(v_parameter_array, 0, sizeof(v_parameter_array));
			sprintf(v_parameter_array[0], "%ld", vAssure_No);		
			strcpy(v_parameter_array[1],pAssure_Id);		
			strcpy(v_parameter_array[2],pId_Type);		
			
			sprintf(vAssure_No_ng, "%ld", vAssure_No);
			v_ret = 0;
			v_ret = OrderUpdateAssuMsg(ORDERIDTYPE_USER,pId_No,100,
					pOp_Code, atol(pLogin_Accept), pLogin_No, pOp_Note, vAssure_No_ng,
					"Assure_Num = Assure_Num +1", "and trim(Assure_Id) = trim(:pAssure_Id) and trim(Id_Type) = trim(:pId_Type)", v_parameter_array);
			if (v_ret < 0)
			{
                 PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-24","����dCustInnetHis�������");
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
                 pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-25","����dCustInnetHis�������");
                 return -25;
              }	  
        }
 	/*ng_���� add by mengfy@08/04/2009---*/        
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
                         Update_Code,    Update_Date)
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
                         TO_NUMBER(:pTotal_Date));
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-26","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -26;
        }

 	/*ng_���� add by mengfy@08/04/2009---*/
/*    EXEC SQL INSERT INTO dCustInnet
                        (Id_No,          Cust_Id,      Belong_Code,
                         Town_Code,      Innet_Type,   Open_Time,
                         Login_Accept,   Login_No,     Machine_Code,
                         Cash_Pay,       Check_Pay,     Sim_Fee,
                         Machine_Fee,    Innet_Fee,    Choice_Fee,
                         Other_Fee,      Hand_Fee,     Deposit,
                         Back_Flag,      Encrypt_Fee,  System_Note,
                         Op_Note,        Assure_No,group_id)
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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-27","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -27;
        }
*/
	memset(&stdCustInnet, 0, sizeof(stdCustInnet));
	
	strncpy(stdCustInnet.sIdNo, pId_No, 14);
	strncpy(stdCustInnet.sCustId, pCust_Id, 10);
	strncpy(stdCustInnet.sBelongCode, pBelong_Code, 7);
	strncpy(stdCustInnet.sTownCode, vTown_Code, 3);
	strncpy(stdCustInnet.sInnetType, pInnet_Type, 2);
	strncpy(stdCustInnet.sOpenTime, pSysDate, 17);
	strncpy(stdCustInnet.sLoginAccept, pLogin_Accept, 14);
	strncpy(stdCustInnet.sLoginNo, pLogin_No, 6);
	strncpy(stdCustInnet.sMachineCode, pMachine_Code, 3);
	sprintf(stdCustInnet.sCashPay, "%0.2f", pCash_Pay_2);
	sprintf(stdCustInnet.sCheckPay, "%0.2f", pCheck_Pay_2);
	sprintf(stdCustInnet.sSimFee, "%0.2f", pSim_Fee);
	sprintf(stdCustInnet.sMachineFee, "%0.2f", pMachine_Fee);
	sprintf(stdCustInnet.sInnetFee, "%0.2f", pInnet_Fee);
	sprintf(stdCustInnet.sChoiceFee, "%0.2f", pChoice_Fee);
	sprintf(stdCustInnet.sOtherFee, "%0.2f", pOther_Fee);
	sprintf(stdCustInnet.sHandFee, "%0.2f", pHand_Fee);
	strcpy(stdCustInnet.sDeposit, "0");
	strcpy(stdCustInnet.sBackFlag, "0");
	strcpy(stdCustInnet.sEncryptFee, "UnKnown");
	strncpy(stdCustInnet.sSystemNote, pSystem_Note, 60);
	strncpy(stdCustInnet.sOpNote, pOp_Note, 60);
	sprintf(stdCustInnet.sAssureNo, "%ld", vAssure_No);
	strncpy(stdCustInnet.sGroupId, vGroupId, 10);
	
	pubLog_Debug(_FFL_, "", "", "vAssure_No=[%ld]", vAssure_No);

	v_ret = 0;
	v_ret = OrderInsertCustInnet(ORDERIDTYPE_USER, pId_No, 100, pOp_Code, atol(pLogin_Accept), pLogin_No,
			pOp_Note, stdCustInnet);	
	if (v_ret != 0)
	{
 		PUBLOG_DBDEBUG("pubOpenAcctConfirm");
        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-27","����dCustInnet�������,v_ret=[%d]",v_ret);
        EXEC SQL ROLLBACK;
        return -27;
	}
	
 	/*ng_���� add by mengfy@08/04/2009---*/
 	
 	/* ng���� ҵ�񹤵�����
    EXEC SQL INSERT INTO dCustStop
                   (Id_No,     Stop_Times, Stop_Durations,
                    Stop_Time, Open_Time,  Should_Pay)
            VALUES( TO_NUMBER(:pId_No), 0, 0,
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),   
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),    
                    0);
        if (SQLCODE != 0) {
            PUBLOG_DBDEBUG("pubOpenAcctConfirm");
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-28","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -28;
        }
     */

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
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-29","����dCustInnetHis�������");
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
                        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
                        EXEC SQL ROLLBACK;
                        return -1;
                    }
            }
            else{
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-30","����wPhoneSim ���� ");
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

		/* ng���� ҵ�񹤵�����
        if(pPrepay_Fee > 0)
        {
            init(TmpSqlStr); 
         
            sprintf(TmpSqlStr , "insert into wPay%s(CONTRACT_NO, ID_NO, TOTAL_DATE,LOGIN_ACCEPT,\
                    CON_CUST_ID,PHONE_NO,BELONG_CODE , FETCH_NO , SM_CODE , LOGIN_NO , ORG_CODE , \
                    OP_TIME , OP_CODE , PAY_TYPE , ADD_MONEY , PAY_MONEY , OUT_PREPAY , PREPAY_FEE , CURRENT_PREPAY , \
                    PAYED_OWE , DELAY_FEE , OTHER_FEE , DEAD_FEE , BAD_FEE , BACK_FLAG , ENCRYPT_FEE , RETURN_CODE , PAY_NOTE,org_id,group_id )  \
                values(to_number(:pCon_Id) , to_number(:pId_No), to_number(:pTotal_Date), to_number(:pLogin_Accept) , \
                    to_number(:vCon_Cust_Id) ,:pPhone_No, :pBelong_Code, '000000', :pSm_Code, :pLogin_No, :vOrg_Code,\
                    TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Code,:pPay_Type, to_number(:pPrepay_Fee) , \
                    to_number(:pPrepay_Fee), 0 , to_number(:pPrepay_Fee) , 0, \
                    0 , 0 , 0 , 0 , 0 , '0' , 'NULL' , '0' ,:pOp_Note,:vOrgId,:VGroupID)",vDateChar);
        
            #ifdef _DEBUG_
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wPay%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
                
            EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
            EXEC SQL EXECUTE ins_stmt using :pCon_Id ,:pId_No ,:pTotal_Date  ,:pLogin_Accept ,:vCon_Cust_Id, :pPhone_No,
                                            :pBelong_Code,:pSm_Code,:pLogin_No ,:vOrg_Code,:pSysDate,:pOp_Code,
                                            :pPay_Type , :pPrepay_Fee , :pPrepay_Fee, :pPrepay_Fee , :pOp_Note ,:vOrgId,:vGroupId;  
        
            if (SQLCODE != 0) {
				PUBLOG_DBDEBUG("pubOpenAcctConfirm");
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-31","����wPay%s ����]" ,vDateChar);                
                EXEC SQL ROLLBACK;
                return -31;
            }
        }
        */  
          
    /********** 2 �� Machine_Fee **********/
/*
    init(TmpSqlStr);

    sprintf(TmpSqlStr, "INSERT INTO wChg%s (ID_NO,TOTAL_DATE,LOGIN_ACCEPT,SM_CODE,BELONG_CODE,PHONE_NO,\
            ORG_CODE,LOGIN_NO,OP_CODE,OP_TIME,MACHINE_CODE,CASH_PAY,CHECK_PAY,SIM_FEE,MACHINE_FEE,\
            INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE)\
        VALUES(TO_NUMBER(:pId_No),TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pSm_Code,:pBelong_Code,:pPhone_No,\
            :vOrg_Code,:pLogin_No,:pOp_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pMachine_Code,\
            to_number(:pCash_Pay_2),to_number(:pCheck_Pay_2),to_number(:pSim_Fee),to_number(:pMachine_Fee),\
            to_number(:pInnet_Fee),to_number(:pChoice_Fee),to_number(:pOther_Fee),to_number(:pHand_Fee),0,'0','UnKnown',:pSystem_Note,:pOp_Note)",\
            vDateChar);

        #ifdef _DEBUG_
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wChg%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
        #endif

    EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
    EXEC SQL EXECUTE ins_stmt using :pId_No,:pTotal_Date,:pLogin_Accept,:pSm_Code,:pBelong_Code,:pPhone_No,:vOrg_Code,
                                    :pLogin_No,:pOp_Code,:pSysDate,:pMachine_Code,:pCash_Pay_2,:pCheck_Pay_2,:pSim_Fee,
                                    :pMachine_Fee,:pInnet_Fee,:pChoice_Fee,:pOther_Fee,:pHand_Fee,:pSystem_Note,:pOp_Note;
    if (SQLCODE != 0) {
        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wChg%s ���� [%d]" ,vDateChar,SQLCODE);
        pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
        EXEC SQL ROLLBACK;
        return -32;
    }
    */
/*edit organizational liuhao 20090331 begin */    
    vLog.vId_No                            =  atol(pId_No);
    vLog.vTotal_Date                       =   atoi(pTotal_Date);
    vLog.vLogin_Accept                     =  atol(pLogin_Accept);

	 strncpy(vLog.vSm_Code,       pSm_Code,2);
	 strncpy(vLog.vBelong_Code,   vOrg_Code,         7);
	 strncpy(vLog.vPhone_No,       pPhone_No,15);
	 strncpy(vLog.vOrg_Code,      vOrg_Code,9       );
	 strncpy(vLog.vLogin_No,       pLogin_No,6);
	 strncpy(vLog.vOp_Code,       pOp_Code,4);
	 strncpy(vLog.vOp_Time,       pSysDate,17);
	 strncpy(vLog.vMachine_Code,     pMachine_Code,3);
	 strncpy(vLog.vBack_Flag,         "0",1);
	 strncpy(vLog.vEncrypt_Fee,       "0",16);
	 strncpy(vLog.vSystem_Note,                  pSystem_Note,60 );
	 strncpy(vLog.vOp_Note,                      pOp_Note,60 );
	 strncpy(vLog.vGroup_Id,       vGroupId,10);
	 strncpy(vLog.vOrg_Id,         vOrgId,10);
	 strncpy(vLog.vPay_Type,       pPay_Type,1);
	 strncpy(vLog.vIp_Addr,                       pIp_Address,15);
	 vLog.vPay_Money                                  = vPay_Money;
	 vLog.vCash_Pay                                   = pCash_Pay_2;
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
  
     pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","========%ld====",vLog.vId_No); 
  
   Vret = recordOprLog(&vLog,sErrorMess,vChgFlag,vCustFlag);
   if(Vret != 0)
   	{
   		/*
   	  sprintf(return_code,"%06d",Vret);
	    goto end_Apply;
	    */
	    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
	    return -1;
	    
   	}       
 /*edit organizational liuhao 20090331 begin*/    
    
    if ((strcmp(pOp_Code,"1114")!=0)&&(strcmp(pOp_Code,"1116")!=0) && (strcmp(pOp_Code,"1118")!=0))
    {
    	/* ��������������� �� ����˱� */
        
        /*init(TmpSqlStr);
        sprintf(TmpSqlStr,"INSERT INTO wLoginOpr%s (TOTAL_DATE , LOGIN_ACCEPT , OP_CODE , PAY_TYPE , PAY_MONEY,SM_CODE ,\
                ID_NO , PHONE_NO , ORG_CODE , LOGIN_NO , OP_TIME , OP_NOTE , IP_ADDR ) \
            VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pOp_Code,:pPay_Type,to_number(:vPay_Money),:pSm_Code,\
                TO_NUMBER(:pId_No),:pPhone_No,:vOrg_Code,:pLogin_No,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Note,:pIp_Address)", vDateChar);
            #ifdef _DEBUG_
                pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wLoginOpr%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
        
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pTotal_Date,:pLogin_Accept,:pOp_Code,:pPay_Type,:vPay_Money,:pSm_Code,:pId_No,
                                        :pPhone_No,:vOrg_Code,:pLogin_No,:pSysDate,:pOp_Note,:pIp_Address;
        if (SQLCODE != 0) {
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����wLoginOpr%s ���� [%d]",vDateChar,SQLCODE);
            pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","����dCustInnetHis�������");
            EXEC SQL ROLLBACK;
            return -33;
        }*/
/*********��֯�������쿪����������  edit by liuhao at 03/31/2009 begin******/

    vLog.vId_No                            =  atol(pId_No);
    vLog.vTotal_Date                       =   atol(pTotal_Date);
    vLog.vLogin_Accept                     =  atol(pLogin_Accept);

	 strncpy(vLog.vSm_Code,       pSm_Code,2);
	 strncpy(vLog.vBelong_Code,   vOrg_Code,         7);
	 strncpy(vLog.vPhone_No,       pPhone_No,15);
	 strncpy(vLog.vOrg_Code,      vOrg_Code,9       );
	 strncpy(vLog.vLogin_No,       pLogin_No,6);
	 strncpy(vLog.vOp_Code,       pOp_Code,4);
	 strncpy(vLog.vOp_Time,       pSysDate,17);
	 strncpy(vLog.vMachine_Code,     pMachine_Code,3);
	 strncpy(vLog.vBack_Flag,         "0",1);
	 strncpy(vLog.vEncrypt_Fee,       "0",16);
	 strncpy(vLog.vSystem_Note,                  pSystem_Note,60 );
	 strncpy(vLog.vOp_Note,                      pOp_Note,60 );
	 strncpy(vLog.vGroup_Id,       vGroupId,10);
	 strncpy(vLog.vOrg_Id,         vOrgId,10);
	 strncpy(vLog.vPay_Type,       pPay_Type,1);
	 strncpy(vLog.vIp_Addr,                       pIp_Address,15);
	 vLog.vPay_Money                                  = vPay_Money;
	 vLog.vCash_Pay                                   = pCash_Pay_2;
	 vLog.vCheck_Pay          											   = pCheck_Pay_2;
	 vLog.vSim_Fee            											   = pSim_Fee;
	 vLog.vMachine_Fee        											   = pMachine_Fee;
	 vLog.vInnet_Fee          											   = pInnet_Fee;
	 vLog.vChoice_Fee         											   = pChoice_Fee;
	 vLog.vOther_Fee           											 = pOther_Fee;
	 vLog.vHand_Fee           											   = pHand_Fee;
	 vLog.vDeposit            											   = 0;
   
   Vret = 0;
   strcpy(vChgFlag,"N");
   strcpy(vCustFlag,"N");
  
     pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","========%ld====",vLog.vId_No); 
  
   Vret = recordOprLog(&vLog,sErrorMess,vChgFlag,vCustFlag);
	   if(Vret != 0)
	   	{/*
	   	  sprintf(return_code,"%06d",Vret);
		    goto end_Apply;
		    */
		    pubLog_DBErr(_FFL_,"pubOpenAcctConfirm","-1","");
		    return -1;
		    
	   	}           
  /*********��֯�������쿪����������  edit by liuhao at 03/31/2009 end*******/      
        
    } /* end --  if (strcmp(pOp_Code,"1114")!=0)  */

       
/******* û�д����˱�
    EXEC SQL INSERT wSimSalesDay

    EXEC SQL INSERT wMsisdnOpen
********/
    
    pubLog_Debug(_FFL_,"pubOpenAcctConfirm","","-------- �û�����ȷ�� pubOpenAcctConfirm() END --------");

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
                   char *InTotal_Date,  char *InSysDate)
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
       
       char vOrg_Id[10+1];
    EXEC SQL END   DECLARE SECTION;

    init(pPhone_No);   init(pSm_Code);
    init(pBelong_Code);init(pLogin_No);  init(pOp_Note);   
    init(pOp_Code);    init(pTotal_Date);init(pSysDate);    
    init(vOrg_Code);   init(pId_No);     init(pFavour_Code);
    init(pShould_Data);init(pActual_Data);
    init(vShould_Bill);init(vActual_Bill);
    init(vFavour_Type);  init(pLogin_Accept);
	init(vOrg_Id);
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
    
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","---------- �û������Żݴ��� pubOpenAcctFav() BEGIN --------");
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Id_No        = [%s]",pId_No);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Phone_No     = [%s]",pPhone_No);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Sm_Code      = [%s]",pSm_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Belong_Code  = [%s]",pBelong_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","pFavour_Code = [%s]",pFavour_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","pShould_Data = [%s]",pShould_Data);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","pActual_Data = [%s]",pActual_Data);            
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Login_No     = [%s]",pLogin_No);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Login_Accept = [%s]",pLogin_Accept);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","System_Note  = [%s]",pSystem_Note);    
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Op_Note      = [%s]",pOp_Note);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Op_Code      = [%s]",pOp_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","Total_Date   = [%s]",pTotal_Date);
    pubLog_Debug(_FFL_,"pubOpenAcctFav","","SysDate      = [%s]",pSysDate);

    /* Ӧ��Ϊ��϶��������Ż� */
    if(strcmp(pShould_Data,"0")==0)
        return 0;
        
    strcpy(vFavour_Type,"0"); 

    EXEC SQL SELECT Org_Code  INTO :vOrg_Code
             FROM   dLoginMsg 
             WHERE  Login_No = :pLogin_No;
         if(SQLCODE!=0){
             pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","ȡ������Ϣ����!");
             pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","SQLCODE = [%d], SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
             EXEC SQL ROLLBACK;
             return -1;
         }
    Trim(pLogin_No);  
    /** ��֯�������� liuhao    200090331 begin*/   	
	if(sGetLoginOrgid(pLogin_No,vOrg_Id) <0)
	{	
		pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","ȡ����Org_id����!");
		return -1;
		
	}
	Trim(vOrg_Id);
	/** ��֯�������� liuhao    200090331 end*/
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

    if(strcmp(vFavour_Type,"0")==0){
        EXEC SQL INSERT INTO wLoginFav
                            (Total_Date,      Login_Accept,       Favour_Code,
                             Org_Code,        Login_No,           Op_Code,
                             Op_Time,         Sm_Code,            Id_No,
                             Phone_No,        Should_Pay,         Actual_Pay,
                             Should_Bill,     Actual_Bill,        Op_Note, Org_id)
                     VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pFavour_Code,
                            :vOrg_Code,      :pLogin_No,         :pOp_Code,
                             TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                            :pSm_Code,       :pId_No,
                            :pPhone_No,      :vShould_Pay,       :vActual_Pay,
                             NULL,            NULL,               :pOp_Note, :vOrg_Id);
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubOpenAcctFav");
            pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","����wLoginFav ����[%s]",sqlca.sqlerrm.sqlerrmc);
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
                             Should_Bill,     Actual_Bill,        Op_Note,Org_id)
                     VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pFavour_Code,
                            :vOrg_Code,      :pLogin_No,         :pOp_Code,
                             TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),
                            :pSm_Code,       :pId_No,
                            :pPhone_No,       0,                  0,
                             TO_DATE(:vShould_Bill,'YYYYMMDD HH24:MI:SS'),   
                             TO_DATE(:vActual_Bill,'YYYYMMDD HH24:MI:SS'),
                            :pOp_Note,:vOrg_Id);
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubOpenAcctFav");
            pubLog_DBErr(_FFL_,"pubOpenAcctFav","-1","����wLoginFav ���� [%s]",sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -1;
        }
        
    }    


    pubLog_Debug(_FFL_,"pubOpenAcctFav","","---------- �û������Żݴ��� pubOpenAcctFav() END --------");    
    
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
    EXEC SQL END   DECLARE SECTION;

    init(pPhone_No);   init(pAttr_Code); init(pSm_Code);
    init(pBelong_Code);init(pLogin_No);  init(pOp_Note);   
    init(pOp_Code);    init(pTotal_Date);init(pSysDate);    
    init(vOrg_Code);   init(pId_No);     init(pSystem_Note);
    init(sIn_Offon_accept);              init(pLogin_Accept);

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
    
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","---------- �û��������ػ����� pubOpenAcctSendCmd() BEGIN --------");
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Id_No       = [%s]",pId_No);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Phone_No    = [%s]",pPhone_No);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Attr_Code   = [%s]",pAttr_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Sm_Code     = [%s]",pSm_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Belong_Code = [%s]",pBelong_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Login_No    = [%s]",pLogin_No);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Login_Accept= [%s]",pLogin_Accept);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","System_Note = [%s]",pSystem_Note);    
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Op_Note     = [%s]",pOp_Note);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Op_Code     = [%s]",pOp_Code);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","Total_Date  = [%s]",pTotal_Date);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","SysDate     = [%s]",pSysDate);
    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","sIn_Offon_accept     = [%s]",sIn_Offon_accept);


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
            pubLog_DBErr(_FFL_,"pubOpenAcctSendCmd","-1","����wChgList ���� [%d][%s]",sqlca.sqlerrm.sqlerrmc);
            EXEC SQL ROLLBACK;
            return -1;
        }

    pubLog_Debug(_FFL_,"pubOpenAcctSendCmd","","---------- �û��������ػ����� pubOpenAcctSendCmd() END --------");
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
       char  return_msg[61];       /*������Ϣ*/ 
       char  vBack_Code[4+1];      /* ���������ش��� */
       char  vBack_Desc[255+1];    /* �������������� */
       int   vret_code=0;
       int   i=0;

    EXEC SQL END   DECLARE SECTION;
    
    init(pPhone_No);    init(pOp_Code);     init(pSysDate);
    init(pBelong_Code); init(vLogin_Accept);init(cmdtext);
    init(return_msg);   init(vBack_Code);   init(vBack_Desc);

    memcpy(pPhone_No,     InPhone_No,      15);
    memcpy(pOp_Code,      InOp_Code,        4);
    memcpy(pSysDate,      InSysDate,       20);
    memcpy(pBelong_Code,  InBelong_Code,    7);
    memcpy(pLogin_No,     InLogin_No,       6);
    
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","---------- ����������� pubSMAPConfirm() BEGIN --------");

    pubLog_Debug(_FFL_,"pubSMAPConfirm","","pPhone_No     = [%s]",   pPhone_No      );
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","pOp_Code      = [%s]",   pOp_Code       );    
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","pSysDate      = [%s]",   pSysDate       );
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","pBelong_Code  = [%s]",   InBelong_Code  );    

    /* ��SMAP�������� */
    EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :vLogin_Accept FROM DUAL;
             if(SQLCODE!=0){
                 pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","ȡϵͳ��ˮ����![%d]",SQLCODE);
                 pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","SQLERRMSG = [%s]",SQLERRMSG);
                 EXEC SQL ROLLBACK;
                 return -1;
             }
    if(strncmp(pOp_Code,"1104",4)==0) /* ���� */
        sprintf(cmdtext,"aaaaa");
    else if(strncmp(pOp_Code,"1105",4)==0) /* �������� */
        sprintf(cmdtext,"aaaaa");
    else
    {
        pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","�����ڵ�OP_CODE!");
        return -1;
    }
       
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","loginaccept:%s",vLogin_Accept);
    vret_code = pubSmapSend(pLogin_No, pOp_Code, pBelong_Code,cmdtext,"vall_no",vdelay_time,vLogin_Accept,return_msg);

    if ( vret_code > 0 )
    {
        pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","����smap_send��ʧ��![%d]SQLERRMSG = [%s]",vret_code,return_msg);
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
        pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","");
        return -1;
    }
    else if(strcmp(vBack_Code,"0000")!=0){
        EXEC SQL ROLLBACK;
        pubLog_DBErr(_FFL_,"pubSMAPConfirm","-1","");
        return -1;
    }
    pubLog_Debug(_FFL_,"pubSMAPConfirm","","---------- ����������� pubSMAPConfirm() END --------");
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
                  char *InSysDate)
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
       char  vPay_Type[1+1];
       char  cmdtext[255];
       char  vLogin_Accept[22+1];
       int   vret_code=0;
       int   vdelay_time=0;        /* �ӳ�ʱ�� */
       char  return_msg[61];       /*������Ϣ*/ 
       int   i=0;
       char  vBack_Code[4+1];      /* ���������ش��� */
       char  vBack_Desc[255+1];    /* �������������� */
           char  vGroupId[10+1];
           char  vYearMonth[6+1];
           
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
    init(pLogin_Accept);init(cmdtext);      init(return_msg);
    init(vLogin_Accept);init(vBack_Code);   init(vBack_Desc);
	init(vGroupId); init(vYearMonth);
		
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
    
    pubLog_Debug(_FFL_,"pubSZXConfirm","","---------- �����п���ȷ�ϴ��� pubSZXConfirm() BEGIN --------");

    pubLog_Debug(_FFL_,"pubSZXConfirm","","Cust_Id       = [%s]",   pCust_Id       );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Id_No         = [%s]",   pId_No         );    
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Cust_Passwd   = [%s]",   pCust_Passwd   );    
    pubLog_Debug(_FFL_,"pubSZXConfirm","","User_Passwd   = [%s]",   pUser_Passwd   );    
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Cust_Name     = [%s]",   pCust_Name   );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Cust_Address  = [%s]",   pCust_Address   );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Id_Type       = [%s]",   pId_Type   );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Id_Iccid      = [%s]",   pId_Iccid   );                
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Phone_No      = [%s]",   pPhone_No      );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Sim_No        = [%s]",   pSim_No        );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Belong_Code   = [%s]",   pBelong_Code   );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Ip_Address    = [%s]",   pIp_Address    );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Cash_Pay      = [%.2f]", pCash_Pay      );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Check_Pay     = [%.2f]", pCheck_Pay     );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Sim_Fee       = [%.2f]", pSim_Fee       );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Machine_Fee   = [%.2f]", pMachine_Fee   );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Choice_Fee    = [%.2f]", pChoice_Fee    );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Hand_Fee      = [%.2f]", pHand_Fee      );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Login_No      = [%s]",   pLogin_No      );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Login_Accept  = [%s]",   pLogin_Accept  );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","System_Note   = [%s]",   pSystem_Note   );    
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Op_Note       = [%s]",   pOp_Note       );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Op_Code       = [%s]",   pOp_Code       );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","Total_Date    = [%s]",   pTotal_Date    );
    pubLog_Debug(_FFL_,"pubSZXConfirm","","SysDate       = [%s]",   pSysDate       );

    strcpy(vSm_Code,"z0");
    strcpy(vPay_Type,"0");
    strcpy(vBill_Code,"UnKnown");
    vPay_Money=pCheck_Pay + pCash_Pay;

    EXEC SQL SELECT Org_Code, to_char(sysdate,'yyyymm')  
    INTO :vOrg_Code, :vYearMonth
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
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","��ѯdLoginMsg ���� [%s]",sqlca.sqlerrm.sqlerrmc);
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
				PUBLOG_DBDEBUG("pubSZXConfirm");
                pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","lixg2 TmpSqlStr =[%s]", TmpSqlStr);
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
			PUBLOG_DBDEBUG("pubSZXConfirm");
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","���º�����Դ�����!");
            
            EXEC SQL ROLLBACK;
            return -1;
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
                    op_code 	=:pOp_Code
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
                    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����ͻ�������ʷ��(dCustDocHis)����[%s]",sqlca.sqlerrm.sqlerrmc);                
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
                         Create_Time,    Close_Time,     Parent_Id)
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
                         TO_NUMBER(:pCust_Id));
            
            if (SQLCODE != 0)
            {
            	if (strcmp(pOp_Code,"1118")!=0)
            	{
            	    /* ��������������������ظ���¼������ */
                    pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����ͻ�������ʷ��(dCustDoc)����[%s]",sqlca.sqlerrm.sqlerrmc);                
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
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����dSimMsgHis����[%s]",sqlca.sqlerrm.sqlerrmc);
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
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����dSimMsg���� [%s]",sqlca.sqlerrm.sqlerrmc);
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
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","���� dAgRes ���� ");
            
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
                        pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wPhoneSim ���� ");
                        
                        EXEC SQL ROLLBACK;
                        return -1;
                    }
            }
            else{
				PUBLOG_DBDEBUG("pubSZXConfirm");
                pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wPhoneSim ���� ");
                
                EXEC SQL ROLLBACK;
                return -1;
            }
        }

    /* ����wChgYYYYMM */
    init(TmpSqlStr);

    sprintf(TmpSqlStr,"INSERT INTO wChg%s (ID_NO,TOTAL_DATE,LOGIN_ACCEPT,SM_CODE,BELONG_CODE,PHONE_NO,\
            ORG_CODE,LOGIN_NO,OP_CODE,OP_TIME,MACHINE_CODE,CASH_PAY,CHECK_PAY,SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,\
            OTHER_FEE,HAND_FEE,DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,\
            OP_NOTE) \
        VALUES(to_number(:pId_No),to_number(:pTotal_Date),to_number(:pLogin_Accept),:vSm_Code,:pBelong_Code,:pPhone_No,\
            :vOrg_Code,:pLogin_No,:pOp_Code,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),' ',to_number(:pCash_Pay), \
            to_number(:pCheck_Pay),to_number(:pSim_Fee),to_number(:pMachine_Fee),0,to_number(:pChoice_Fee),\
            0,to_number(:pHand_Fee),0,'0','UnKnown',:pSystem_Note,:pOp_Note)",vDateChar);

        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"pubSZXConfirm","","����wChg%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
        #endif

        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pId_No,:pTotal_Date,:pLogin_Accept,:vSm_Code,:pBelong_Code,:pPhone_No,
                                        :vOrg_Code,:pLogin_No,:pOp_Code,:pSysDate,:pCash_Pay,:pCheck_Pay,:pSim_Fee,
                                        :pMachine_Fee,:pChoice_Fee,:pHand_Fee,:pSystem_Note,:pOp_Note;

    if (SQLCODE != 0) {
		PUBLOG_DBDEBUG("pubSZXConfirm");
        pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wChg%s ���� ",vDateChar);;
        
        EXEC SQL ROLLBACK;
        return -1;
    }

    /* ����wLoginOprYYYYMM */
    if (strcmp(pOp_Code,"1118")!=0)
    {
        init(TmpSqlStr);
        sprintf(TmpSqlStr,"INSERT INTO wLoginOpr%s  (TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,PAY_TYPE,PAY_MONEY,SM_CODE,\
                ID_NO,PHONE_NO,ORG_CODE,LOGIN_NO,OP_TIME,OP_NOTE,IP_ADDR) \
            VALUES(TO_NUMBER(:pTotal_Date),to_number(:pLogin_Accept),:pOp_Code,:vPay_Type,to_number(:vPay_Money),:vSm_Code,\
                TO_NUMBER(:pId_No),:pPhone_No,:vOrg_Code,:pLogin_No,TO_DATE(:pSysDate,'YYYYMMDD HH24:MI:SS'),:pOp_Note,:pIp_Address)", vDateChar);
            #ifdef _DEBUG_
                pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wLoginOpr%s :TmpSqlStr=[%s]", vDateChar,TmpSqlStr);
            #endif
        
        EXEC SQL PREPARE ins_stmt From :TmpSqlStr; 
        EXEC SQL EXECUTE ins_stmt using :pTotal_Date,:pLogin_Accept,:pOp_Code,:vPay_Type,:vPay_Money,:vSm_Code,:pId_No,
                                         :pPhone_No,:vOrg_Code,:pLogin_No,:pSysDate,:pOp_Note,:pIp_Address;
        
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("pubSZXConfirm");
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wLoginOpr%s ����",vDateChar);         
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
			PUBLOG_DBDEBUG("pubSZXConfirm");
            pubLog_DBErr(_FFL_,"pubSZXConfirm","-1","����wSimChgDay ���� ");
            
            EXEC SQL ROLLBACK;
            return -1;
        }
           
/*********
wSimSalesDay

wMsisdnOpen
*********/

    pubLog_Debug(_FFL_,"pubSZXConfirm","","---------- �����п���ȷ�ϴ��� pubSZXConfirm() END --------");

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
    
    pubLog_Debug(_FFL_,"pubDefaultFunc","","--------------- begin function pubDefaultFunc ----------");
    
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
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Belong_Code      =[%s]",      sIn_Belong_Code);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Sm_Code      =[%s]",      sIn_Sm_Code);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Total_date      =[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Login_accept      =[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sIn_Op_code      =[%s]",      sIn_Op_code);
    #endif
    
    /* ���  �������� */
    EXEC SQL SELECT substr(:sIn_Belong_Code,1,2),substr(:sIn_Belong_Code,3,2),substr(:sIn_Belong_Code,5,3)
             INTO :sV_Region_code,:sV_District_code,:sV_Town_code FROM dual;
    if(SQLCODE!=0){
        sprintf(sErrMessA,"%s","��ֹ�������ʧ��!");
		PUBLOG_DBDEBUG("pubDefaultFunc");
        pubLog_DBErr(_FFL_,"pubDefaultFunc","-1","sErrMessA=[%s]",sErrMessA);
        return -1;
    } 
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubDefaultFunc","","sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]",\
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
    	pubLog_Debug(_FFL_,"pubDefaultFunc","","û��Ĭ���ط�");
    	return 0;
    }
        
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
    	sprintf(sErrMessA,"%s","��ѯsFuncList��ʧ��!");
    	PUBLOG_DBDEBUG("pubDefaultFunc");
   	  pubLog_DBErr(_FFL_,"pubDefaultFunc","-2","sErrMessA=[%s]",sErrMessA);
        return -2;
    }
    
    while(SQLCODE == 0)
    {
        Trim(sV_Function_type);
        Trim(sV_Function_code);
        Trim(sV_Add_flag);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"pubDefaultFunc","","sV_Function_type      =[%s]",      sV_Function_type);
            pubLog_Debug(_FFL_,"pubDefaultFunc","","sV_Function_code      =[%s]",      sV_Function_code);
            pubLog_Debug(_FFL_,"pubDefaultFunc","","sV_Add_flag      =[%s]",      sV_Add_flag);
        #endif
        
        EXEC SQL select count(*) into :sV_flag from dCustFunc where id_no=to_number(:sIn_User_id) and function_code=:sV_Function_code;
        if(sV_flag>0)
            pubLog_Debug(_FFL_,"pubDefaultFunc","","aaaaaaaaaaaasV_Function_code=[%s]",sV_Function_code);
        else
        {
        /* ���� dCustFunc �� */
        EXEC SQL insert into dCustFunc(ID_NO , FUNCTION_TYPE , FUNCTION_CODE , OP_TIME )
                 values(to_number(:sIn_User_id),:sV_Function_type,:sV_Function_code,
                        TO_DATE(:sIn_Sys_date,'YYYYMMDD HH24:MI:SS'));
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFunc��ʧ��!");
           PUBLOG_DBDEBUG("pubDefaultFunc");
            pubLog_DBErr(_FFL_,"pubDefaultFunc","-3","sErrMessA=[%s]",sErrMessA);
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
            pubLog_DBErr(_FFL_,"pubDefaultFunc","-4","sErrMessA=[%s]",sErrMessA);
            return -4;
        } 
        }
        /* ���� dCustFuncAdd �� 
        EXEC SQL insert into dCustFuncAdd(ID_NO , FUNCTION_CODE , FUNCTION_TYPE )
                 values(to_number(:sIn_User_id),:sV_Function_code ,:sV_Function_type);
        if(SQLCODE!=0){
            sprintf(sErrMessA,"%s","����dCustFuncAdd��ʧ��!");
            pubLog_Debug(_FFL_,"pubDefaultFunc","","SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            pubLog_Debug(_FFL_,"pubDefaultFunc","","SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            pubLog_DBErr(_FFL_,"pubDefaultFunc","-7","sErrMessA=[%s]",sErrMessA);
            return -7;
        } 
        Trim(sV_Function_code);
	ret = fSendCmd(funcEnv, '1', "1219", sV_Function_code);
	if(ret != 0)
	{
            sprintf(sErrMessA,"%s","����fSendCmd����wSndCmdDayʧ��!");
           pubLog_DBErr(_FFL_,"pubDefaultFunc","-8","ret = [%d]",ret);
            return -8;
	}

        EXEC SQL FETCH c102 INTO :sV_Function_type , :sV_Function_code , :sV_Add_flag;
    }
    EXEC SQL CLOSE c102 ;
    
   
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_,"pubDefaultFunc","","--------------- end function pubDefaultFunc ----------");
    
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
      char sIn_SimNoA[21], double sIn_PreFeeA, char * sErrMessA)
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
    
    pubLog_Debug(_FFL_,"pubOpenFunc","","--------------- begin function pubOpenFunc ----------");
    
    /* ���Ի����� */
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
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Total_date      =[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Op_code      =[%s]",      sIn_Op_code);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Org_code      =[%s]",      sIn_Org_code);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Login_accept      =[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Sm_code      =[%s]",      sIn_Sm_code);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Belong_code      =[%s]",      sIn_Belong_code);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_Offon_accept      =[%s]",      sIn_Offon_accept);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_UsedFunc_List      =[%s]",      sIn_UsedFunc_List);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_UnUsedFunc_List      =[%s]",      sIn_UnUsedFunc_List);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_AddAddFunc_List      =[%s]",      sIn_AddAddFunc_List);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_CmdRight      =[%s]",      sIn_CmdRight);
        pubLog_Debug(_FFL_,"pubOpenFunc","","sIn_SimNo      =[%s]",      sIn_SimNo);
    #endif
    
    if ((strlen(ltrim(rtrim(sIn_UsedFunc_List)))==0) && 
        (strlen(ltrim(rtrim(sIn_UnUsedFunc_List)))==0) &&
        (strlen(ltrim(rtrim(sIn_UnUsedFunc_List)))==0) &&
        (strlen(ltrim(rtrim(sIn_AddAddFunc_List)))==0))
    {
        strcpy(sErrMessA,"SUCCESSED!");
        
        pubLog_Debug(_FFL_,"pubOpenFunc","","������û��ѡ���ط�");
        pubLog_Debug(_FFL_,"pubOpenFunc","","--------------- end function pubOpenFunc ----------");
    
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
        pubLog_DBErr(_FFL_,"pubOpenFunc","-10","sErrMessA=[%s]",sErrMessA);
        return -10;         
    }
    
    Trim(sV_ImsiNo);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenFunc","","sV_ImsiNo      =[%s]",      sV_ImsiNo);
    #endif
    
    /* ��ѯ  HLR���� */
    EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
             where  phoneno_head like substr(:sIn_Phone_no,1,7)||'%';
    if (SQLCODE != 0)
    {
        sprintf(sErrMessA, "��ѯsHlrCode����!"); 
		PUBLOG_DBDEBUG("pubOpenFunc");
        pubLog_DBErr(_FFL_,"pubOpenFunc","-11","sErrMessA=[%s]",sErrMessA);
        return -11;         
    }
    
    Trim(sV_hlrCode);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"pubOpenFunc","","sV_hlrCode      =[%s]",      sV_hlrCode);
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
    
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.totalDate=[%s]",funcEnv.totalDate);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.opTime=[%s]",funcEnv.opTime);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.opCode=[%s]",funcEnv.opCode);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.loginNo=[%s]",funcEnv.loginNo);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.orgCode=[%s]",funcEnv.orgCode);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.loginAccept=[%s]",funcEnv.loginAccept);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.idNo=[%s]",funcEnv.idNo);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.phoneNo=[%s]",funcEnv.phoneNo);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.smCode=[%s]",funcEnv.smCode);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.cmdRight=[%s]",funcEnv.cmdRight);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.simNo=[%s]",funcEnv.simNo);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.belongCode=[%s]",funcEnv.belongCode);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.imsiNo=[%s]",funcEnv.imsiNo);
    pubLog_Debug(_FFL_,"pubOpenFunc","","funcEnv.hlrCode=[%s]",funcEnv.hlrCode);

    pubLog_Debug(_FFL_,"pubOpenFunc",""," --- begin process ---- ");
    
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
            pubLog_Debug(_FFL_,"pubOpenFunc","","sV_Func_Code      =[%s]",      sV_Func_Code);
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
    
    pubLog_Debug(_FFL_,"pubOpenFunc","","call checkBillBindFunc retcode = [%d]",sV_RetCode);
    if ( strcmp(sV_RetCode, "000000") != 0)
    {
        sprintf(sErrMessA, "���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�[%s]���б��!", sV_RetCode);
        return -2;
    }
    */
    
    
    /*����������Ч���ط�*/
    /*
    iFuncRet =  fAddUsedFuncs(&funcEnv, sIn_UsedFunc_List) ;
    pubLog_Debug(_FFL_,"pubOpenFunc","","call fAddUsedFuncs retcode = [%d]",iFuncRet);
    if (iFuncRet != 0)
    {            
       sprintf(sErrMessA, "����������Ч���ط�����!");  
        return -3; 
    }*/
    
    /*����ԤԼ��Ч���ط�*/
    /*iFuncRet = fAddUnUsedFuncs(&funcEnv, sIn_UnUsedFunc_List);
    pubLog_Debug(_FFL_,"pubOpenFunc","","call fAddUnUsedFuncs retcode = [%d]",iFuncRet);
    if (iFuncRet != 0)
    {
        sprintf(sErrMessA, "����ԤԼ��Ч���ط�����!");  
        return -4;
    }*/
    /*����������Ч���ط�*/
	iFuncRet=fAddUsedLimitFuncs(&funcEnv, sIn_UsedFunc_List,sIn_PreFee,&sV_LimitFee,sV_FunCode);
	if ( iFuncRet != 0)
	{
		if(iFuncRet==121958)
		{
			pubLog_DBErr(_FFL_,"pubOpenFunc","-6","1vFunCode[%s]1vLimitFee[%f]",sV_FunCode,sV_LimitFee);
			sprintf(sErrMessA, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sV_FunCode,sV_LimitFee);
		}
		else
		{
			strcpy(sErrMessA, "����������Ч���ط�����!");
		}
		pubLog_DBErr(_FFL_,"pubOpenFunc","-6","");
		return -6; 
	}
	/*����ԤԼ��Ч���ط�*/
	iFuncRet=fAddUnUsedLimitFuncs(&funcEnv, sIn_UnUsedFunc_List,sIn_PreFee,&sV_LimitFee,sV_FunCode);
	if ( iFuncRet != 0)
	{
		if(iFuncRet==121956)
		{
pubLog_DBErr(_FFL_,"pubOpenFunc","-6","1vFunCode[%s]1vLimitFee[%f]",sV_FunCode,sV_LimitFee);
			sprintf(sErrMessA, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sV_FunCode,sV_LimitFee);
		}
		else
		{
			strcpy(sErrMessA, "����ԤԼ��Ч���ط�����!");
		}
		pubLog_DBErr(_FFL_,"pubOpenFunc","-7","");
		return -7; 
	}
    
    /*���Ӵ��̺ŵ��ط���*/
    iFuncRet =  fAddAddFuncs(&funcEnv, sIn_AddAddFunc_List);
    pubLog_Debug(_FFL_,"pubOpenFunc","","call fAddAddFuncs retcode = [%d]",iFuncRet);
    if (iFuncRet != 0)
    {
        sprintf(sErrMessA, "���Ӵ��̺ŵ��ط�����!"); 
        pubLog_DBErr(_FFL_,"pubOpenFunc","-5","");
        return -5;         
    }
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_,"pubOpenFunc","","--------------- end function pubOpenFunc ----------");
    
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
    
    pubLog_Debug(_FFL_,"pubOpenDataCard","","--------------- begin function pubOpenDataCard ----------");
    
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
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Cred_id      =[%s]",      sIn_Cred_id);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Phone_no      =[%s]",      sIn_Phone_no);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Use_type      =[%s]",      sIn_Use_type);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Gprs_rate      =[%s]",      sIn_Gprs_rate);
        pubLog_Debug(_FFL_,"pubOpenDataCard","","sIn_Wlan_rate      =[%s]",      sIn_Wlan_rate);
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
             from  dCredRes
             where cred_id = :sIn_Cred_id ; 
    if(SQLCODE!=0)
    {
    	if(SQLCODE==1403)
    	{
            strcpy(sErrMessA,"���ݿ���û�и�ƾ֤����!");
			PUBLOG_DBDEBUG("pubOpenDataCard");
	 pubLog_DBErr(_FFL_,"pubOpenDataCard","-2","sErrMessA=[%s]",sErrMessA);
            return -1;
        }
        else
        {
            strcpy(sErrMessA,"����dCredCust�����!");
            PUBLOG_DBDEBUG("pubOpenDataCard");
        pubLog_DBErr(_FFL_,"pubOpenDataCard","-2","sErrMessA=[%s]",sErrMessA);
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
        pubLog_DBErr(_FFL_,"pubOpenDataCard","-3","sErrMessA=[%s]",sErrMessA);
        return -3;
    }
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    pubLog_Debug(_FFL_,"pubOpenDataCard","","--------------- end function pubOpenDataCard ----------");
    
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

    pubLog_Debug(_FFL_,"pubDataCred","","------------ ���ݿ����� pubDataCred() Begin --------");
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
        pubLog_Debug(_FFL_,"pubDataCred","","InPrepay_Fee   = [%.2f]", InPrepay_Fee    );
        pubLog_Debug(_FFL_,"pubDataCred","","Id_No          = [%s]",   pId_No          );
        pubLog_Debug(_FFL_,"pubDataCred","","pPhone_No      = [%s]",   pPhone_No       );
        pubLog_Debug(_FFL_,"pubDataCred","","pMode_code     = [%s]",   pMode_code      );
        pubLog_Debug(_FFL_,"pubDataCred","","Belong_Code    = [%s]",   pBelong_Code    );
        pubLog_Debug(_FFL_,"pubDataCred","","Login_No       = [%s]",   pLogin_No       );
        pubLog_Debug(_FFL_,"pubDataCred","","pOrg_Code      = [%s]",   pOrg_Code       );
        pubLog_Debug(_FFL_,"pubDataCred","","Op_Code        = [%s]",   pOp_Code        );  
        pubLog_Debug(_FFL_,"pubDataCred","","Login_Accept   = [%s]",   pLogin_Accept   );
        pubLog_Debug(_FFL_,"pubDataCred","","Total_Date     = [%s]",   pTotal_Date     ); 
        pubLog_Debug(_FFL_,"pubDataCred","","SysDate        = [%s]",   pSysDate        );     
    #endif

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
		PUBLOG_DBDEBUG("pubDataCred");
        pubLog_DBErr(_FFL_,"pubDataCred","-1","����dDataCredMsg��ʧ��!");
        strcpy(InMsgErr,"����dDataCredMsg��ʧ��!");
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
		PUBLOG_DBDEBUG("pubDataCred");
        pubLog_DBErr(_FFL_,"pubDataCred","-1","����dDataCredMsgHis��ʧ��!");
        strcpy(InMsgErr,"����dDataCredMsgHis��ʧ��!");
        return -1;
    }

    /*EXEC SQL insert into wTotalFavChg(OP_NO,OP_TYPE,PHONE_NO, ORDER_CODE, ID_NO, FAVOUR_OJBECTS,FAVOUR_FEE, OP_TIME)
            select sMaxBillChg.NEXTVAL,'3',phone_no,order_code,id_no,fee_codes,favour_fee,sysdate
            from wTotalFav where phone_no=:pPhone_No;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        printf("����wTotalFavChg��ʧ��!");
        strcpy(InMsgErr,"����wTotalFavChg��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    EXEC SQL delete wTotalFav where phone_no=:pPhone_No;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        printf("ɾ��wTotalFav��ʧ��!");
        strcpy(InMsgErr,"ɾ��wTotalFav��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    EXEC SQL select order_code into :vOrderCode from wTotalFav where id_no=to_number(:pId_No);
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        printf("��ѯwTotalFav��ʧ��!");
        strcpy(InMsgErr,"��ѯwTotalFav��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    vOrderCode++;
    
    EXEC SQL select begin_money,month_num,month_code into :vMoneyFee,vMonthNum,:vMonthCode
             from sDataCredCode
             where :InPrepay_Fee>=begin_money and :InPrepay_Fee<end_money and rownum<2;
    if(SQLCODE!=0)
    {
        printf("��ѯsDataCredCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsDataCredCode��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    
    EXEC SQL select sum(a.month_fee) into :vModeFee from sBillMonthCode a,sBillModeDetail b
            where a.region_code=b.region_code and a.region_code=substr(:pBelong_Code,1,2) 
                and a.month_code=b.detail_code and b.mode_code=:pMode_code and b.detail_type='1';
    if(SQLCODE!=0)
    {
        printf("��ѯsBillMonthCode��ʧ��!");
        strcpy(InMsgErr,"��ѯsBillMonthCode��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    }
    
    vNum=0;
    EXEC SQL select count(*),to_char(add_months(sysdate,1),'yyyymm')||'01' into :vNum,vEndTime from sDataCredCfg where mode_code=:pMode_code;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
        printf("��ѯsDataCredCfg��ʧ��!");
        strcpy(InMsgErr,"��ѯsDataCredCfg��ʧ��!");
        printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            printf("��¼dBillCustDetail����!");
            strcpy(InMsgErr,"��¼dBillCustDetail����!");
            printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            printf("��¼dBillCustDetail����!");
            strcpy(InMsgErr,"��¼dBillCustDetail����!");
            printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
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
            printf("����wTotalFav��ʧ��!");
            strcpy(InMsgErr,"����wTotalFav��ʧ��!");
            printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
        EXEC SQL insert into wTotalFavChg(OP_NO,OP_TYPE,PHONE_NO, ORDER_CODE, ID_NO, FAVOUR_OJBECTS,FAVOUR_FEE, OP_TIME)
                values(sMaxBillChg.NEXTVAL,'1',:pPhone_No,:vOrderCode,to_number(:pId_No),'r0',:vFavFee,to_char(sysdate,'yyyymmddhh24miss'));
        if(SQLCODE!=0)
        {
            printf("����wTotalFavChg��ʧ��!");
            strcpy(InMsgErr,"����wTotalFavChg��ʧ��!");
            printf("SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
            return -1;
        }
    }
	*/
    pubLog_Debug(_FFL_,"pubDataCred","","------------ ���ݿ����� pubDataCred() End --------");
    
    return 0;
}
/***************************************
 ��������  s1114CfmF
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
void s1114CfmF( TPSVCINFO *transb )
{       
    int		input_par_num =	0;	     /*�����������        */
    int		output_par_num = 0;          /*�����������        */
    FBFR32		*transIN = NULL;     /*���뻺����          */
    FBFR32		*transOUT = NULL;    /*���������          */
    FLDLEN32	len;			     /*�������������Ŀռ�*/
    char	        temp_buf[61];        /*��ʱʹ�õ��ַ�������*/
    char            service_name[20];
    char            return_code[6+1];
    char            sErrorMess[40+1];        /* ������� */
	int Vret = 0;
	tLoginOpr vLog; 
	char		vStartTime[20+1];			/* ����ʼʱ�� YYYYMMDDHHMISS*/
	double		vUsedTime=0.00;				/* ��������ʱ�� ��λ����*/
	double		vStartSec=0.00;				/* ����ʼʱ�� ��λ����*/
	BOMC_LOG 	logShip; 
        
    EXEC SQL BEGIN DECLARE SECTION;
    
    	char    sTemp[10+1];
        char    sIn_Login_no[6+1];                    /* ��������        */
        char    sIn_Ip_Address[15+1];                 /* ӪҵԱ��½IP    */
        char    sIn_Op_Code[4+1];                     /* ��������        */
        char    sIn_System_Note[60+1];                /* ϵͳ��ע        */        
        char    sIn_Op_Note[60+1];                    /* ������ע        */
        char    sIn_Sim_Type[2048];                    /* SIM������       */     
        char    sIn_Belong_Code[7+1];                 /* �����������    */   
		char    vAreaCode[20+1];
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
        char    sV_Sim_Type_List[100][6];            /* �ֻ����봮      */
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

         /* ����ϵͳĬ��ֵ */
         float  fV_Limit_Owe  = 0.00;                /* ������           */
         char   sV_Attr_Code[8+1];                   /* �û�����         */
         char   sV_Credit_Code[2+1];                 /* �����ȴ���       */
         int    iV_Credit_Value = 0;                 /* ������ֵ         */
         char   sV_Pay_Code[1+1];                    /* ���Ѵ���         */
         char   sV_Pay_Type[1+1];                    /* ��������         */
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
         int   iV_SimTypeCount=0;
         int   vPermitTime=0;                        /*�������ʱ��*/
		 char  vModeCode1[8+1];
		 char	dynStmt[1024];
		 char	TableName[30+1];
		 char 	vOtaOpCode[4+1];
		 int	vOtaCount=0;
		 
		 char  sTrue_flag[1+1];
         char  sCustinfoType[2+1];
         char  sagreement_type[1+1];
         char vSimPassWd[8+1];
         int  vGCount=0;
         char vGCustName[60+1];
         char vGIdIccid[20+1];
         char vGtrueFlag[1+1];
         char offenFunc[32+1];
		 char vrunCode[2+1];
		 double V_LimitFee=0;
		 char	sv_funccode[25+1];
		 char	newfuncList[128+1];
  		 char	oldfuncList[128+1];
  		 char	newfuncStr[128+1];
  		 char	defaultfuncList[64+1];
  		 char   tpUsedFunc_List[ROWLEN];
	     char vOrgId[10+1];
	     char vGroupId[10+1];    
	     char vChgFlag[1+1];
	     char vCustFlag[1+1];  
	     
	      char vGroupID_dis[10+1];
	      char tmp_org_code[30+1];
	      char tmp_areacode[2+1];
	      char vsql_t[1024];	     		 
		char vPassWord[16+1];
		
		/*ng���� ҵ�񹤵����� ���Ӳ���*/
		char  sOrgCode[9+1];
    	char  sRegionCode[2+1];	
    	char  sPayFlag[1+1];	
    	/*ng���� ҵ�񹤵����� ���Ӳ���*/	
    /***************add for ��𾻶���� 2012.6.18*********************/
    int   vIsNetAmount=0;            /*   �Ƿ񾻶����(0 ���ʽ, 1 �Ǿ��ʽ)  */
    char  vIsOutright[1+1];          /*   �Ƿ���ϴ�����(Y:��ʾ���ڷ���ϴ�����)  */
    float vReward_Fee=0;             /*   �����̳��     */
    float vTax_Fee=0;                /*   ����˰��       */
    /***************add for ��𾻶���� *********************/
    int iRet = 0;
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
		
		char iLoginPwd[16+1];
    EXEC SQL END DECLARE SECTION;
    
    tFuncEnv funcEnv;
    sChkDBLogin(transIN,transOUT,"s1114Cfm","dbchange");
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","==========begin s1114CfmF ============== "); 
    #endif
	
    memset(input_parms, 0, sizeof(input_parms));
    memset(output_parms, 0, sizeof(output_parms));
    /*
    memset(funcEnv, 0, sizeof(funcEnv));
    */
    /* ���Ի����� */
    init(vSimPassWd);
    init(sTrue_flag);
    init(sCustinfoType);
    init(sagreement_type);
    init(vOtaOpCode);
	init(TableName);
	init(dynStmt);
	init(vModeCode1);
	init(sTemp);
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
    init(sCustinfoType);
    init(vOrgId);
    init(vGroupId);
    init(vChgFlag);
    init(vCustFlag);
    init(vGroupID_dis);
	init(tmp_org_code);
	init(tmp_areacode);
    init(vsql_t);
	memset(&vLog,0,sizeof(vLog));
	memset(vPassWord,0,sizeof(vPassWord));              
	init(vStartTime);
 	memset(&logShip,0,sizeof(logShip));
 	
 	init(iLoginPwd);
 	
 	/*ng���� ҵ�񹤵����� ���Ӳ���*/
 	init(sOrgCode);
 	init(sRegionCode);
 	init(sPayFlag);	   	
	int	  v_ret = 0;
	char  vCallServiceName[30+1];
	int   vSendParaNum = 0;
	int   vRecpParaNum = 0;
	char  vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA];
	const int iOrderRight = 100;     		
    /*ng���� ҵ�񹤵����� ���Ӳ���*/
      
    for (i=0;i<100;i++)
    {
         init(sV_Favour_Code[i]);
         init(sV_Should_Data[i]);
         init(sV_Actual_Data[i]);
         init(sV_Phone_no_List[i]);
         init(sV_Sim_Type_List[i]);
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
  	init(sv_funccode);
  	init(defaultfuncList);
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
    
    strcpy(service_name,"s1114CfmF"); 
    strcpy(return_code,"000000");
    strcpy(sErrorMess,"SUCCESS!");
    
    /* ����ϵͳĬ��ֵ */ 
    /*strcpy(sV_Credit_Code , "E"); */
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
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","==========  ��ʼ���������ڴ� ============== "); 
    #endif
    
    transIN	= (FBFR32 *)transb->data;
    get_input_parms32(transIN, &input_par_num, &output_par_num,"dbchange", CONNECT0);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","==========  ��ʼ��������ڴ�  ============== "); 
    #endif
    
    len = (FLDLEN32)(FIRST_OCCS32 *	output_par_num * 50);
    
    transOUT = (FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
    if (transOUT ==	(FBFR32	*)NULL)	
    {
        /*���仺����ʧ�ܺ���������,�Ͽ�(�򲻶Ͽ�)���ݿ�����,������������*/
        sprintf(temp_buf,"E200:	alloc error in s1920Qry,size(%ld)", (long)len);
        error_handler32(transIN,transOUT,"200",temp_buf,"dbchange",CONNECT0);
    }
	
    /*
    EXEC SQL WHENEVER SQLERROR DO error_handler32(transIN,transOUT,
    	"200",temp_buf,"dbchange",CONNECT0);
    EXEC SQL WHENEVER SQLWARNING DO	warning_handler32();
    EXEC SQL WHENEVER NOT FOUND CONTINUE;
    */
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","==========  ��ʼ�������ݿ� ============== "); 
    #endif
    
    /* �������ݿ� */
    getBeginTime(&vStartTime,&vStartSec);

    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","==========  ��ʼȡ��������� ============== "); 
    #endif
    
    for (i=0;i<25;i++)
    {
        pubLog_Debug(_FFL_,"s1114CfmF","","input_parms[%d]= [%s] ",i,input_parms[i]);	
    }
	
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
    strcpy(sIn_Prepay_fee_List, input_parms[21]);;
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
	/// sprintf(vPassWord,"%s",input_parms[34]);  �����ǿ��������汾�Ƿ��д���ֵ�������ҳ������Ҫ�����޸�
	
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
	strcpy(vaccflag,input_parms[34]);  /*�����Ƿ�����ڱ�־*/
	strcpy(vcycleDay,input_parms[35]);  /*����Ƕ������������ڣ�����ʹ�����Ȼ���� 1*/
	/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/

	
	strncpy(sIn_Login_no,sTemp,6);
    strncpy(sTrue_flag,sTemp+6,1);
    strncpy(sCustinfoType,sTemp+6+1,2);
    strncpy(sagreement_type,sTemp+6+1+2,1);
	
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Login_no = [%s] ",sIn_Login_no);
        pubLog_Debug(_FFL_,"s1114CfmF","","sTrue_flag = [%s] ",sTrue_flag);
        pubLog_Debug(_FFL_,"s1114CfmF","","sCustinfoType = [%s] ",sCustinfoType);
        pubLog_Debug(_FFL_,"s1114CfmF","","sagreement_type = [%s] ",sagreement_type);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Ip_Address = [%s] ",sIn_Ip_Address);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Op_Code = [%s] ",sIn_Op_Code);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_System_Note = [%s] ",sIn_System_Note);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Op_Note = [%s] ",sIn_Op_Note);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Sim_Type = [%s] ",sIn_Sim_Type);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Belong_Code = [%s] ",sIn_Belong_Code);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_CustDocFlag = [%s] ",sIn_CustDocFlag);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Cust_Id = [%s] ",sIn_Cust_Id);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Cust_Passwd = [%s] ",sIn_Cust_Passwd);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Sm_code = [%s] ",sIn_Sm_code);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Service_type = [%s] ",sIn_Service_type);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Innet_code = [%s] ",sIn_Innet_code);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Org_code = [%s] ",sIn_Org_code);
        pubLog_Debug(_FFL_,"s1114CfmF","","iIn_Bill_Type = [%d] ",iIn_Bill_Type);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_PayFlag = [%s] ",sIn_PayFlag);
        pubLog_Debug(_FFL_,"s1114CfmF","","fIn_Sim_fee = [%f] ",fIn_Sim_fee);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Fee_List = [%s] ",sIn_Fee_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Phone_no_List = [%s] ",sIn_Phone_no_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Sim_no_List = [%s] ",sIn_Sim_no_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Choice_fee_List = [%s] ",sIn_Choice_fee_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Prepay_fee_List = [%s] ",sIn_Prepay_fee_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Package_List = [%s] ",sIn_Package_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_CustInfo = [%s] ",sIn_CustInfo);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Favour_List = [%s] ",sIn_Favour_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Password = [%s] ",sIn_Password);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_NewOldFlag = [%s] ",sIn_NewOldFlag);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Login_Accept = [%s] ",sIn_Login_Accept);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Check_No = [%s] ",sIn_Check_No);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Bank_Code = [%s] ",sIn_Bank_Code);
        pubLog_Debug(_FFL_,"s1114CfmF","","pUsedFunc_List = [%s] ",pUsedFunc_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","pUnUsedFunc_List = [%s] ",pUnUsedFunc_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","pAddAddFunc_List = [%s] ",pAddAddFunc_List);
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_Flag = [%s] ",sV_Flag);
        
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H begin*/
		pubLog_Debug(_FFL_, service_name, "", "vaccflag  =[%s]",    vaccflag     );
		pubLog_Debug(_FFL_, service_name, "", "vcycleDay=[%s]",vcycleDay);
		/*���ֶ����ڸ��� �����û�������Ϣ�� 2012/5/22 14:47:40 S.H end*/
        
    #endif

        if(strcmp(sIn_Op_Code,"118b")==0)
        {
        	strcpy(vOtaOpCode,sIn_Op_Code);
        	init(sIn_Op_Code);
        	strcpy(sIn_Op_Code,"1118");
        	strcpy(sCustinfoType,"03");
        }
        else
        {
        	strcpy(vOtaOpCode,"1111");
        }
        
        pubLog_Debug(_FFL_,"s1114CfmF","","vOtaOpCode=[%s] ",vOtaOpCode);
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Op_Code=[%s] ",sIn_Op_Code);
        
        

    /* �����û����� */
    /* ����ǲ��ϻ� ���� sV_Attr_Code = dCustDoc.OWNER_GRADE + '000'
                  ����   sV_Attr_Code �� '000000'                 */
    
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
            strcpy(return_code,"111488");
            strcpy(sErrorMess, "��ѯ dCustDoc �����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_Debug(_FFL_,"s1114CfmF","","[%s],[%s] ",return_code,sErrorMess);
            goto end_valid;	
        }
    }
    else
    {
    	strcpy(sV_Attr_Code , "00000Y"); 
    }
    
    Trim(sV_Attr_Code);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_Attr_Code = [%s] ",sV_Attr_Code);
    #endif
	
	
	Vret=0;
	Vret=sGetLoginOrgid(sIn_Login_no,vOrgId);
	if(Vret!=0)
	{
		sprintf(return_code, "%06d",Vret);
		strcpy(sErrorMess,"ȡ�˻���������2��");
		goto end_valid;			
	}	
	Trim(vOrgId);
	
	/*��֤����Ȩ��*/
	exec sql select password into :iLoginPwd from dloginmsg where login_no = :sIn_Login_no;
	if(SQLCODE != 0)
	{
		sprintf(return_code,"110499");
		sprintf(sErrorMess,"ȡ������Ϣ����[%d]",SQLCODE);
		goto end_valid ;
	}
	Trim(iLoginPwd);
	sprintf (return_code, "%06d", spublicRight( LABELDBCHANGE, sIn_Login_no, iLoginPwd, sIn_Op_Code ));
	if (strcmp(return_code, "000000") != 0)
	{
		strcpy(sErrorMess,"����Ȩ����֤ʧ��");
		goto end_valid ;
	}
	
    /* ��ѯ �������� */
    EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss'),to_char(sysdate,'yyyymmdd'),
                    to_char(sysdate,'yyyymm'),substr(:sIn_Org_code,1,2),
                    TO_CHAR(add_months(SysDate,1),'yyyymm')||'01 00:00:00',
                    to_number(substr(to_char(sysdate,'yyyymmddhh24miss'),9,4)),to_char(sysdate,'dd')
            INTO :sV_Sys_date,:sV_Total_date  ,:sV_YYYYMM , :sV_Region_code,:iEffectTime,:vPermitTime,:vnowdate
            FROM dual;
    if (SQLCODE !=0)
    {
        strcpy(return_code,"111401");
        strcpy(sErrorMess,"��ѯ�������ڳ���!");
        PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_,"s1114CfmF","","[%s],[%s] ",return_code,sErrorMess);
        goto end_valid ;	
    }
    
    Trim(sV_Sys_date);
    Trim(sV_Total_date);
    Trim(sV_YYYYMM);
    Trim(sV_Region_code);
    Trim(vnowdate);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_Sys_date=[%s],sV_Total_date = [%s] ",sV_Sys_date,sV_Total_date);
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_YYYYMM = [%s]  ",sV_YYYYMM); 
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_Region_code = [%s]  ",sV_Region_code);
        pubLog_Debug(_FFL_,"s1114CfmF","","vnowdate = [%s]  ",vnowdate);
    #endif
    
    /* ȡ�ò�����ˮ */
    Trim(sIn_Login_Accept);
    if  ((strlen(sIn_Login_Accept)==0)||(strcmp(sIn_Login_Accept,"0")==0))
    {
        EXEC SQL SELECT sMaxSysAccept.NEXTVAL INTO :sV_Login_accept
            FROM DUAL;
            if(SQLCODE!=0){
				PUBLOG_DBDEBUG("s1114CfmF");
                pubLog_DBErr(_FFL_,"s1114CfmF","","ȡ����������ˮ����! ");
                strcpy(return_code,"111488");
                strcpy(sErrorMess, "ȡ����������ˮ����!");
                goto end_valid;
            }
    
    }
    else
    {
    	strcpy(sV_Login_accept , sIn_Login_Accept);
    }
    
    Trim(sV_Login_accept);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF",""," sV_Login_accept = [%s] ",sV_Login_accept);
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
                   pubLog_Debug(_FFL_,"s1114CfmF","","�Żݴ���[%d]       = [%s] ",k,sV_Favour_Code[k-1]);
                   pubLog_Debug(_FFL_,"s1114CfmF","","�Ż�ǰ����[%d]     = [%s] ",k,sV_Should_Data[k-1]);
                   pubLog_Debug(_FFL_,"s1114CfmF","","�Żݺ�����[%d]     = [%s] ",k,sV_Actual_Data[k-1]);
               #endif
               ret=-1;
               
               strcpy(sV_User_Id,"0");
               
               ret=pubOpenAcctFav(sV_User_Id,            sTmp_Str,           sIn_Sm_code,         
                                  sIn_Belong_Code,       sV_Favour_Code[k-1],           sV_Should_Data[k-1],
                                  sV_Actual_Data[k-1],   sIn_Login_no,                  sV_Login_accept,    
                                  sIn_Op_Note,           sIn_System_Note,               sIn_Op_Code,            
                                  sV_Total_date,         sV_Sys_date);
               if(ret != 0)
               {
                   pubLog_Debug(_FFL_,"s1114CfmF","","�����Ż���Ϣ����! ");
                   strcpy(return_code,"111402");
                   strcpy(sErrorMess, "�����Ż���Ϣ����!");
                   goto end_valid;
               }
               begin = end +2;
            }
        }
    }
    
    /*----------------------- �����ײ���Ϣ����ʼ --------------------------*/
    if(strlen(ltrim(rtrim(sIn_Package_List)))!=0){
        memset(vTmp_List,0x00,sizeof(vTmp_List));       
        memcpy(vTmp_List,sIn_Package_List,sizeof(sIn_Package_List)-1);
        pubLog_Debug(_FFL_,"s1114CfmF","","----Tmp_List=[%s] ",vTmp_List);        
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
                pubLog_Debug(_FFL_,"s1114CfmF","","�ʷѴ���=[%s] ",sV_BillMode[i]);
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
            pubLog_Debug(_FFL_,"s1114CfmF","","�ͻ�����  =[%s] ",sV_Cust_Name);
            pubLog_Debug(_FFL_,"s1114CfmF","","�ͻ���ַ  =[%s] ",sV_Cust_Address);
            pubLog_Debug(_FFL_,"s1114CfmF","","�ͻ�֤������ =[%s] ",sV_CustId_Type);
            pubLog_Debug(_FFL_,"s1114CfmF","","�ͻ�֤������ =[%s] ",sV_Id_Iccid);  
        #endif      
             
    }   
    
    /* ����ܷ�����Ϣ�� */
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    memcpy(vTmp_List,sIn_Fee_List,sizeof(sIn_Fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
        pubLog_Debug(_FFL_,"s1114CfmF","","���ֽ𽻿�=[%10.2f] ",fV_Sum_Cash_Pay);
        pubLog_Debug(_FFL_,"s1114CfmF","","��֧Ʊ����=[%10.2f] ",fV_Sum_Check_Pay);
        pubLog_Debug(_FFL_,"s1114CfmF","","��Ԥ���=[%10.2f] ",fV_Sum_Prepay_Fee);
        pubLog_Debug(_FFL_,"s1114CfmF","","��SIM����=[%10.2f] ",fV_Sum_Sim_Fee);
        pubLog_Debug(_FFL_,"s1114CfmF","","��ѡ�ŷ�=[%10.2f] ",fV_Sum_Choice_Fee);
        pubLog_Debug(_FFL_,"s1114CfmF","","������=[%10.2f] ",fV_Hand_Fee);
        pubLog_Debug(_FFL_,"s1114CfmF","","areaCode=[%s] ",vAreaCode);
        pubLog_Debug(_FFL_,"s1114CfmF","","pGroupNo=[%s] ",pGroupNo);
        pubLog_Debug(_FFL_,"s1114CfmF","","sV_Iccid=[%s] ",sV_Iccid);
    #endif
    Trim(vAreaCode);
    Trim(pGroupNo);
    /* ��� �ֻ����봮 */
    iV_PhoneCount = 0;
    memset(sV_Phone_no_List,0,sizeof(sV_Phone_no_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Phone_no_List,sizeof(sIn_Phone_no_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
                pubLog_Debug(_FFL_,"s1114CfmF","","phone=[%s] ",sTmp_Str );
            #endif
            
            strcpy(sV_Phone_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_PhoneCount = k;
    
    pubLog_Debug(_FFL_,"s1114CfmF","","�����ֻ�����=[%d] ",iV_PhoneCount);
    
    if (iV_PhoneCount < 1 )
    {
        strcpy(return_code,"111430");
        strcpy(sErrorMess,"������ֻ���������Ϊ0!");
        goto end_valid ;	
    }
    /*��ʾ����18:30����6:00֮��*/
	if( (vPermitTime>=0 && vPermitTime<=600) || (vPermitTime>=1830 && vPermitTime<=2400 ) ) 
    {
    	if(iV_PhoneCount>50)
    	{
    		strcpy(return_code,"111473");
        	strcpy(sErrorMess,"��������������ܳ���50��"); 
        	goto end_valid ;
    	}
    }
   
    else
    {
    	strcpy(return_code,"111474");
        strcpy(sErrorMess,"��ʱ�䲻������� ���԰���ʱ��Ϊ��18:30����6:00");
        goto end_valid ;
    }
    for (i=0;i<iV_PhoneCount;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]���ֻ����룽[%s] ",i,sV_Phone_no_List[i]);
        #endif
    }
   
    /*******************************************************/
    /* ���simtype���봮 */
    pubLog_Debug(_FFL_,"s1114CfmF","","���simtype���봮 ");
    iV_SimTypeCount = 0;
    memset(sV_Sim_Type_List,0,sizeof(sV_Sim_Type_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Sim_Type,sizeof(sIn_Sim_Type)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
                pubLog_Debug(_FFL_,"s1114CfmF","","simtype=[%s] ",sTmp_Str );
            #endif
            
            strcpy(sV_Sim_Type_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_SimTypeCount = k;
    
    pubLog_Debug(_FFL_,"s1114CfmF","","SIMTYPE����=[%d] ",iV_SimTypeCount);
    
    if (iV_PhoneCount < 1 )
    {
        strcpy(return_code,"111430");
        strcpy(sErrorMess,"�����SIMTYPE����Ϊ0!");
        goto end_valid ;	
    }
    if (iV_SimTypeCount != iV_PhoneCount)
    {
        strcpy(return_code,"111403");
        strcpy(sErrorMess,"������ֻ�����������SIMTYPE������һ��!");
        goto end_valid ;		
    }
    for (i=0;i<iV_SimTypeCount;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]��simtype��[%s] ",i,sV_Sim_Type_List[i]);
        #endif
    }
    /*******************************************************/

    /* ����û����봮 */
    if(strcmp(sIn_Op_Code,"1116")==0||strcmp(sIn_Op_Code,"1118")==0)
    {
        iV_PassCount = 0;
        memset(sV_Password_List,0,sizeof(sV_Password_List));
        memset(vTmp_List,0x00,sizeof(vTmp_List));
        
        memcpy(vTmp_List,sIn_Password,sizeof(sIn_Password)-1);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
                    pubLog_Debug(_FFL_,"s1114CfmF","","password=[%s] ",sTmp_Str );
                #endif
                
                strcpy(sV_Password_List[k] ,  sTmp_Str);
                
                k++;
                begin = end +2;
            }
        }
        iV_PassCount = k;
        
        pubLog_Debug(_FFL_,"s1114CfmF","","�û���������=[%d] ",iV_PassCount);
        
        if (iV_PassCount < 1 )
        {
        	if(strcmp(vOtaOpCode,"118b")!=0)
        	{
            	strcpy(return_code,"111430");
            	strcpy(sErrorMess,"������û���������Ϊ0!");
            	goto end_valid ;	
            }
        }
        
        for (i=0;i<iV_PassCount;i++)
        {
            #ifdef _DEBUG_
                pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]���û����룽[%s] ",i,sV_Password_List[i]);
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
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
            pubLog_Debug(_FFL_,"s1114CfmF","","sTmp_Str=[%s] ",sTmp_Str );
            
            strcpy(sV_Sim_no_List[k] ,  sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_,"s1114CfmF","","����SIM������=[%d] ",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]��SIM�����룽[%s] ",i,sV_Sim_no_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(return_code,"111403");
        strcpy(sErrorMess,"������ֻ�����������SIM��������һ��!");
        goto end_valid ;		
    }
    
    
    
    vOtaCount=0;
    /*EXEC SQL select count(1)
    				into :vOtaCount
    		from dsimrespasswd
    		where sim_no=:sV_Sim_no_List[0];
    EXEC SQL select count(1)
    				into :vOtaCount    			
    		from sotasimtype
    		where res_code in 
    		(select sim_type from dSimRes where sim_no=:pSim_No);
    if(SQLCODE!=0)
    {
    	 strcpy(return_code,"110442");
         strcpy(sErrorMess, "ȡOTA��Ϣʧ��");
         goto end_valid;
    }
    if(strcmp(vOtaOpCode,"118b")==0)
    {
    	if(vOtaCount==0)
    	{
    		strcpy(return_code,"110442");
         	strcpy(sErrorMess, "�˿�����OTA��,�뵽��ͨ�ļ���������ҳ����в���");
         	goto end_valid;
    	}
    }
    else
    {
    	if(vOtaCount>0)
    	{
    		strcpy(return_code,"110442");
         	strcpy(sErrorMess, "�˿���OTA��,�뵽OTA�ļ���������ҳ����в���");
         	goto end_valid;
    	}
    }*/

    /* ��� ѡ�ŷѴ� */
    iV_Count = 0;
    init(sTmp_Str);
    memset(fV_Choice_fee_List,0,sizeof(fV_Choice_fee_List));
    memset(vTmp_List,0x00,sizeof(vTmp_List));
    
    memcpy(vTmp_List,sIn_Choice_fee_List,sizeof(sIn_Choice_fee_List)-1);

    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
            pubLog_Debug(_FFL_,"s1114CfmF","","sTmp_Str=[%s] ",sTmp_Str );
            
            fV_Choice_fee_List[k] = atof(sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_,"s1114CfmF","","����ѡ�ŷ�����=[%d] ",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]��ѡ�ŷѣ�[%f] ",i,fV_Choice_fee_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(return_code,"111404");
        strcpy(sErrorMess,"������ֻ�����������ѡ�ŷ�������һ��!");
        goto end_valid ;		
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
        pubLog_Debug(_FFL_,"s1114CfmF",""," vTmp_List = [%s] ",vTmp_List);
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
            pubLog_Debug(_FFL_,"s1114CfmF","","sTmp_Str=[%s] ",sTmp_Str );
            
            fV_Prepay_fee_List[k] = atof(sTmp_Str);
            
            k++;
            begin = end +2;
        }
    }
    iV_Count = k;
    
    pubLog_Debug(_FFL_,"s1114CfmF","","����Ԥ������=[%d] ",iV_Count);
    
    for (i=0;i<iV_Count;i++)
    {
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","��[%d]��Ԥ�棽[%f] ",i,fV_Prepay_fee_List[i]);
        #endif
    }
    
    if (iV_Count != iV_PhoneCount)
    {
        strcpy(return_code,"111405");
        strcpy(sErrorMess,"������ֻ�����������Ԥ���������һ��!");
        goto end_valid ;		
    }
    
    /*add in 2008-9-10*/
    /*
    ret=-1;
    ret=soffencommand(pId_No,sV_Region_code,sIn_Op_Code,sIn_Sm_code,vrunCode,offenFunc);
    if(ret != 0)
    {
        pubLog_Debug(_FFL_,"s1114CfmF","","soffencommandȡ���ػ��������! ");
        strcpy(return_code,"111491");
        strcpy(sErrorMess, "ȡ���ػ��������!");
        goto end_valid;
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
				strcpy(return_code,"111490");
  		  		sprintf(sErrorMess, "��ѯdphonereservemsgʧ��[%d]",SQLCODE);
  		  		goto end_valid;
			}
			if(iV_Count>0)
			{
				strcpy(return_code,"111491");
  		  		sprintf(sErrorMess, "ԤԼ����[%s]�뵽��ͨ����ģ�鿪��",sV_Phone_no_List[iPhone]);
  		  		goto end_valid;
			}    	
    	/* ������������� �� ���ɿͻ�ID */
        if (strcmp(sIn_CustDocFlag,"1")==0)
        {
            /* �����µĿͻ�ID */
            if(getMaxID(sV_Region_code, 0, sIn_Cust_Id ) == NULL)
            {
                strcpy(return_code,"111406");
                strcpy(sErrorMess,"spubGetId���д���!");
                goto end_valid;
            }	
            
            Trim(sIn_Cust_Id);
    	    
    	    #ifdef _DEBUG_
                pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Cust_Id[%s] ",sIn_Cust_Id);
            #endif
        }
    	
    	/* ��ѯ�û�ID , �ʻ�ID */
        if(getMaxID(sV_Region_code, 1, sV_User_Id ) == NULL)
        {
            strcpy(return_code,"111407");
            strcpy(sErrorMess,"spubGetId���д���!");
            goto end_valid;
        }
    	
    	Trim(sV_User_Id);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","sV_User_Id��[%s] ",sV_User_Id);
        #endif
        
        if(getMaxID(sV_Region_code, 1, sV_Acct_Id ) == NULL)
        {
            strcpy(return_code,"111408");
            strcpy(sErrorMess,"spubGetId���д���!");
            goto end_valid;
        }
    	
    	Trim(sV_Acct_Id);
    	
    	#ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","sV_Acct_Id[%s] ",sV_Acct_Id);
        #endif
       /* ADD BY ZHCY : 2009-03-11 */
        /*
        *	TD����	�������Ϳ��ػ�����
        */
        ret=-1;
    	ret=soffencommand(sV_User_Id, sV_Region_code,sIn_Op_Code,sIn_Sm_code,vrunCode,offenFunc);
    	if(ret != 0)
    	{
    	    pubLog_Debug(_FFL_,"s1114CfmF","","soffencommandȡ���ػ��������! ");
    	    strcpy(return_code,"111491");
    	    strcpy(sErrorMess, "ȡ���ػ��������!");
    	    goto end_valid;
    	}    	
    	
        /* �����롢SIM����Ч��*/
        ret=-1;
        ret=CheckPhoneSimNo(sV_Phone_no_List[iPhone],     sIn_Org_code,      sIn_Sm_code,      
                            sV_Sim_no_List[iPhone],       sV_Sim_Type_List[iPhone],       sV_Iccid,sIn_Login_no,sErrorMess);
        if(ret != 0){
            pubLog_Debug(_FFL_,"s1114CfmF","","�����롢SIM����Ч�Դ���[%s] ",sErrorMess);
            sprintf(return_code,"%d",ret);
            goto end_valid;
        }
        
        /* ���º��롢SIM����Ϣ */
        ret=-1;
        ret=pubOpenAcctRes(sV_User_Id,        sV_Phone_no_List[iPhone],      sV_Sim_no_List[iPhone],
                           sIn_Belong_Code,  sIn_Login_no,      sV_Login_accept, 
                           sIn_Op_Code,      sV_Total_date,    sV_Sys_date,pGroupNo);
        if(ret != 0){
            pubLog_Debug(_FFL_,"s1114CfmF","","���º���SIM����Դ����! ");            
            sprintf(return_code,"%6d",ret);
            strcpy(sErrorMess, "���º���SIM����Դ����!");
            goto end_valid;
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
        pubLog_Debug(_FFL_,"s1114CfmF","","sIn_Service_type[%d][%s] ",iPhone,sV_service_type1);
        if(strcmp(sIn_Op_Code,"1116")!=0 && strcmp(sIn_Op_Code,"1118")!=0 )
            strcpy(sV_Password_List[iPhone],sIn_Password);
        
    vOtaCount=0;
    /*EXEC SQL select count(1)
    				into :vOtaCount    			
    		from sotasimtype
    		where res_code in 
    		(select sim_type from dSimRes where sim_no=:sV_Sim_no_List[iPhone]);
    if(SQLCODE!=0)
    {
    	 strcpy(return_code,"110442");
         strcpy(sErrorMess, "ȡOTA��Ϣʧ��");
         goto end_valid;
    }
    if(strcmp(vOtaOpCode,"118b")==0)
    {
    	if(vOtaCount==0)
    	{
    		strcpy(return_code,"111442");
         	strcpy(sErrorMess, "�˿�����OTA��,�뵽��ͨ�ļ���������ҳ����в���");
         	goto end_valid;
    	}
    }
    else
    {
    	if(vOtaCount>0)
    	{
    		strcpy(return_code,"111442");
         	strcpy(sErrorMess, "�˿���OTA��,�뵽OTA�ļ���������ҳ����в���");
         	goto end_valid;
    	}
    }
        
        if(vOtaCount==0)
        {*/
        	init(vSimPassWd);
    		EXEC SQL select pass_word
    				into :vSimPassWd
    				from dsimrespassword
    				where sim_no=:sV_Sim_no_List[iPhone]
    				and (region_code=substr(:sIn_Belong_Code,1,2) or region_code='99');
    		if(SQLCODE!=0 && SQLCODE!=1403)
    		{
    		 	strcpy(return_code,"111452");
        	 	sprintf(sErrorMess, "dsimrespassword��Ϣ��ȫ[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
        	 	goto end_valid;
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
    			 	strcpy(return_code,"111453");
        		 	sprintf(sErrorMess, "dsimrespassword2��Ϣ��ȫ[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
        		 	goto end_valid;
    			}
    		}
        /*}
        else if (vOtaCount>0)
        {
        	init(vSimPassWd);
    		EXEC SQL select pass_word
    				into :vSimPassWd
    				from dsimrespassword
    				where sim_no=:sV_Sim_no_List[iPhone]
    				and (region_code=substr(:sIn_Belong_Code,1,2) or region_code='99');
    		if(SQLCODE!=0 )
    		{
    		 	strcpy(return_code,"111452");
        	 	sprintf(sErrorMess, "OTA��Ϣ��ȫ[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
        	 	goto end_valid;
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
    			 	strcpy(return_code,"111453");
        		 	sprintf(sErrorMess, "OTA��Ϣ��ȫ2[%d][%s]",SQLCODE,sV_Sim_no_List[iPhone]);
        		 	goto end_valid;
    			}
    		}
        }*/
        ret=pubOpenAcctConfirm(sIn_Cust_Id,    sV_Acct_Id,          sV_User_Id, 
                             sV_Password_List[iPhone],  sV_Password_List[iPhone],sV_Password_List[iPhone],
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
                             sV_CustId_Type , sV_Id_Iccid , sIn_NewOldFlag , sIn_CustDocFlag, vAreaCode,pGroupNo,
                             sTrue_flag,sCustinfoType,sagreement_type);
                             
        if(ret != 0){
            pubLog_Debug(_FFL_,"s1114CfmF","","����ͻ� �û� �ʻ���Ϣ����! ");
            strcpy(return_code,"111410");
            strcpy(sErrorMess, "����ͻ� �û� �ʻ���Ϣ����!");
            pubLog_Debug(_FFL_,"s1114CfmF","","call pubOpenAcctConfirm ret = [%d] ",ret);
            goto end_valid;
        }

		/*���ֶ����� �����û��˺���Ϣ�� 2012/5/22 13:47:45 S.H begin */
		if(strcmp(vaccflag,"Y")==0)
		{
			/*���ú���pubCustAcctCycle �������ڱ�dCustAcctCycle*/
			iaccnum=pubCustAcctCycle(atol(sV_Acct_Id),atol(sV_User_Id),sV_Phone_no_List[iPhone],atol(vcycleDay),atol(sV_Login_accept),sIn_Login_no,sIn_Op_Note,sV_Region_code,sIn_Op_Code);
			if(iaccnum != 0)
			{
				pubLog_Debug(_FFL_, "s1114CfmF", "", "���ú���pubCustAcctCycle����! ret = [%d]",iaccnum);
				strcpy(return_code,"110405");
				strcpy(sErrorMess, "�����û���������Ϣ�����!");
				goto end_valid;
			}
		}

        vGCount=0;
    	EXEC SQL select count(1)
    				into :vGCount
    				from dGoodPhoneRes
    				where phone_no = :sV_Phone_no_List[iPhone];
    	if(SQLCODE!=0)
    	{
    		strcpy(return_code,"111487");
    	    sprintf(sErrorMess, "��ѯ[%s]��������ʧ��[%d]",sV_Phone_no_List[iPhone],SQLCODE);
    	    goto end_valid;
    	}
    	
    	if(vGCount>0)
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
    			strcpy(return_code,"111487");
    	    	sprintf(sErrorMess, "��ѯ[%s]dcustdocʧ��[%d]",sIn_Cust_Id,SQLCODE);
    	    	goto end_valid;
    		}
    		Trim(vGCustName);
    		Trim(vGIdIccid);
    		Trim(vGtrueFlag);
    		if(strcmp(vGCustName,"Ԥ����")==0 || strcmp(vGIdIccid,"111111111111111111")==0 || strcmp(vGtrueFlag,"Y")!=0)
    		{
    			strcpy(return_code,"111487");
    	    	sprintf(sErrorMess, "����������Ҫ��ʵ��д�ͻ�����[%s]��֤������[%s],�û����ϱ������Ѻ�ʵ",vGCustName,vGIdIccid);
    	    	goto end_valid;
    		}
    	}
        
        /* ȡ���ػ���ˮ */
        EXEC SQL SELECT sOffon.nextval  INTO :sV_Offon_accept  FROM DUAL;
                   
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","sV_Offon_accept     = [%s] ",sV_Offon_accept);
        #endif
        
        /* �����û���Ч��  �ڿ�ͨ�ײ�ʱ�ᴦ��
        ret=-1;
        ret=pubCBXExpire(sV_User_Id ,     sIn_Belong_Code,   fV_Prepay_fee_List[iPhone],
                         sIn_Op_Note,     sIn_Login_no,      sV_Login_accept, 
                         sIn_Op_Code,     sV_Total_date,     sV_Sys_date,
                         sIn_Sm_code,     sV_BillMode[0]);
        if(ret != 0){
            pubLog_Debug(_FFL_,"s1114CfmF","","�����û���Ч�ڴ���! ");
            strcpy(return_code,"111412");
            strcpy(sErrorMess, "�����û���Ч�ڴ���!");
            goto end_valid;
        }
        pubLog_Debug(_FFL_,"s1114CfmF","","gggggggsIn_Service_type[%d][%s] ",iPhone,sIn_Service_type);
        */
        
        /* ��ѯ  HLR���� */
        EXEC SQL select hlr_code into :sV_hlrCode from sHlrCode
                 where  phoneno_head = substr(:sV_Phone_no_List[iPhone],1,7);
        if (SQLCODE != 0)
        {
            strcpy(return_code,"111416");
            strcpy(sErrorMess, "��ѯhlr����ʧ��!");
            goto end_valid;
        }
    
        Trim(sV_hlrCode);
        
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","sV_hlrCode      =[%s] ",      sV_hlrCode);
        #endif
        
        /* ���ػ��ӿ� */   
        /*ret=-1;
        ret=pubOpenAcctSendCmd(sV_User_Id,           sV_Phone_no_List[iPhone],       sV_Attr_Code,
                               sIn_Sm_code,           sIn_Belong_Code,           sIn_Login_no,
                               sV_Login_accept,       sIn_Op_Note,               sIn_System_Note,
                               sIn_Op_Code,           sV_Total_date,             sV_Sys_date,
                               sV_Offon_accept);
        if(ret != 0){
             pubLog_Debug(_FFL_,"s1114CfmF","","���Ϳ��ػ��������! ");
            strcpy(return_code,"111414");
            strcpy(sErrorMess, "���Ϳ��ػ��������!");
            goto end_valid;
        }*/
        pubLog_Debug(_FFL_,"s1114CfmF","","bbbbbbsIn_Service_type[%d][%s] ",iPhone,sIn_Service_type);
		
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
                     sErrorMess , sV_BillMode[0],
                     defaultfuncList);
                pubLog_Debug(_FFL_,"s1114CfmF","","eeeeeesIn_Service_type[%d][%s] ",iPhone,sIn_Service_type);
            }
            else
            {
                /* ���ʷ��ײ� */
                ret = newpubDefaultFunc(sV_User_Id , sIn_Login_no ,
                     sIn_Belong_Code , sIn_Sm_code ,
                     sV_Total_date , sV_Sys_date ,
                     sV_Login_accept , sIn_Op_Code ,
                     sErrorMess , "0",
                     defaultfuncList);
                pubLog_Debug(_FFL_,"s1114CfmF","","eeffffffffsIn_Service_type[%d][%s] ",iPhone,sIn_Service_type);
            }
            pubLog_Debug(_FFL_,"s1114CfmF","","call pubDefaultFunc ,ret=[%d][%s]! " , ret,defaultfuncList);
            if(ret != 0)
            {
                pubLog_Debug(_FFL_,"s1114CfmF","","���Ĭ���ط�����,ret=[%d]! " , ret);
                strcpy(return_code,"111411");
                pubLog_Debug(_FFL_,"s1114CfmF","","sErrorMess = [%s]  ",sErrorMess);
                goto end_valid;
            }
        }
		
		/* ���ػ��ӿ� add in 2008-9-10 */   
        ret=-1;
        init(newfuncList);
        init(oldfuncList);
        init(newfuncStr);
        init(tpUsedFunc_List);
        Trim(defaultfuncList);
        Trim(pUsedFunc_List);
        sprintf(tpUsedFunc_List,"%s%s",pUsedFunc_List,defaultfuncList);
	    Trim(tpUsedFunc_List);
	    pubLog_Debug(_FFL_,"s1114CfmF","","++++++tpUsedFunc_List=[%s],pUsedFunc_List=[%s],defaultfuncList=[%s]++++++ ",tpUsedFunc_List,pUsedFunc_List,defaultfuncList);
	    ret=sNewfuncList(tpUsedFunc_List,offenFunc,newfuncList,oldfuncList,newfuncStr,sV_hlrCode);
	    if(ret != 0)
	    {
	        pubLog_Debug(_FFL_,"s1114CfmF","","sNewfuncList�ϲ����ػ�ָ�����! ");
	        strcpy(return_code,"111492");
	        strcpy(sErrorMess, "�ϲ����ػ�ָ�����!");
	        goto end_valid;
	    }
	    
	    pubLog_Debug(_FFL_,"s1114CfmF","","+++pUsedFunc_List=[%s] ",pUsedFunc_List);
	    pubLog_Debug(_FFL_,"s1114CfmF","","+++offenFunc=[%s] ",offenFunc);
	    pubLog_Debug(_FFL_,"s1114CfmF","","+++newfuncList=[%s] ",newfuncList);
	    pubLog_Debug(_FFL_,"s1114CfmF","","+++newfuncStr=[%s] ",newfuncStr);
	    pubLog_Debug(_FFL_,"s1114CfmF","","+++oldfuncList=[%s] ",oldfuncList);
		
        /* Ĭ���ط� */
          /* ��ѯ  IMSI���� */
        EXEC SQL select substr(trim(imsi_no),1,15) into :sV_ImsiNo
                 from   dSimRes
                 where  sim_no = :sV_Sim_no_List[iPhone];
        if (SQLCODE != 0)
        {
            strcpy(return_code,"111415");
            strcpy(sErrorMess, "��ѯIMSI��ʧ��!");
            goto end_valid;
        }
        Trim(sV_ImsiNo);
    
        #ifdef _DEBUG_
            pubLog_Debug(_FFL_,"s1114CfmF","","sV_ImsiNo      =[%s] ",      sV_ImsiNo);
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
	        pubLog_Debug(_FFL_,"s1114CfmF","","�û��������ػ��������! ");
	        strcpy(return_code,"110411");
	        pubLog_Debug(_FFL_,"s1114CfmF",""," ret = [%d]  ",ret);
	        strcpy(sErrorMess, "�û��������ػ��������!");
	        goto end_valid;
	    }
		
		ret =-1;
		V_LimitFee=0;
		init(sv_funccode);
		ret =NewfAddUsedFuncs(&funcEnv,newfuncStr,fV_Prepay_fee_List[iPhone],&V_LimitFee,sv_funccode);
		if(ret != 0)
	    {
	        if(ret==121958)
			{
				sprintf(sErrorMess, "�û��������Ϊ[%s]���ط�,���������[%.2f]Ԫ",sv_funccode,V_LimitFee);
			}
			else
			{
				strcpy(sErrorMess, "����������Ч���ط�����!");
			}
	        
	        pubLog_Debug(_FFL_,"s1114CfmF","","����������Ч���ط�����! ");
	        strcpy(return_code,"110411");
	        pubLog_Debug(_FFL_,"s1114CfmF",""," ret = [%d]  ",ret);
	        goto end_valid;
	    }
		
		pubLog_Debug(_FFL_,"s1114CfmF","","111sV_Phone_no_List[%d]=[%s] ",iPhone,sV_Phone_no_List[iPhone]);
        ret = pubOpenFunc(transIN , transOUT , temp_buf ,sV_Total_date ,
            sV_Sys_date , sIn_Op_Code , sIn_Login_no , sIn_Org_code ,
            sV_Login_accept , sV_User_Id , sV_Phone_no_List[iPhone] , sIn_Sm_code ,
            sIn_Belong_Code , "1" ,
            oldfuncList, pUnUsedFunc_List , pAddAddFunc_List ,
            "1" , sV_Sim_no_List[iPhone] ,fV_Prepay_fee_List[iPhone],
            sErrorMess);
        if(ret != 0)
        {
            pubLog_Debug(_FFL_,"s1114CfmF","","�û������ط���Ϣ����,ret=[%d]! " , ret);
            strcpy(return_code,"111411");
            pubLog_Debug(_FFL_,"s1114CfmF","","sErrorMess = [%s]  ",sErrorMess);
            goto end_valid;
        }         
        pubLog_Debug(_FFL_,"s1114CfmF","","222sV_Phone_no_List[%d]=[%s] ",iPhone,sV_Phone_no_List[iPhone]);

        /* �ʷѴ��� */
        /* */
        pubLog_Debug(_FFL_,"s1114CfmF","","333sV_Phone_no_List[%d]=[%s] ",iPhone,sV_Phone_no_List[iPhone]);
        for(i=0;(i<iV_BillModeNum);i++)
        {
        	pubLog_Debug(_FFL_,"s1114CfmF","","444sV_Phone_no_List[%d]=[%s] ",iPhone,sV_Phone_no_List[iPhone]);
            ret = -1;
    	    if(strlen(sV_BillMode[i])==0)
    	        continue;
			if (i==0)
			{
				memset(vModeCode1, '\0', sizeof(vModeCode1));
				strcpy(vModeCode1,sV_BillMode[i]);

				sprintf(TableName,"dinnetmodecode%ld",atol(sV_User_Id)%10);
				sprintf(dynStmt,"insert into %s (ID_NO,MODE_CODE,TOTAL_DATE) values(TO_NUMBER(:vi),:v2,TO_NUMBER(:v3))",TableName);
				EXEC SQL PREPARE sql_stmt FROM :dynStmt;
				EXEC SQL EXECUTE sql_stmt using :sV_User_Id,:vModeCode1,:sV_Total_date;
				if (SQLCODE != 0)
				{
					pubLog_Debug(_FFL_,"s1114CfmF","","dinnetmodecode ���� [%d][%s] ",SQLCODE,sqlca.sqlerrm.sqlerrmc);
					EXEC SQL ROLLBACK;
					goto end_valid;
				}
			    /**insert dAllUser*/  
			   /*������ edit by cencm at 20100414 --------begin---------------*/			      
			    /*exec sql insert into dAllUser(
			    msisdn,fav_brand,fav_type,flag_code,fav_order,fav_day,
			    start_time,end_time,region_code,id_no,group_id,product_code)
			   	values(
			    :sV_Phone_no_List[iPhone],:sIn_Sm_code,'user','user',0,'1',sysdate,
			    to_date('20500101 00:00:00','YYYYMMDD HH24:MI:SS'),substr(:sIn_Belong_Code,1,2),
			    :sV_User_Id,'null',:vModeCode1);
			  	if(SQLCODE!=OK)
				{
					pubLog_Debug(_FFL_,"s1114CfmF","","����dAllUser���� [%d][%s] ",SQLCODE,sqlca.sqlerrm.sqlerrmc);
					EXEC SQL ROLLBACK;
					goto end_valid;
				} */
				 /*������ edit by cencm at 20100414 --------end---------------*/
			}
			
			pubLog_Debug(_FFL_,"s1114CfmF","","555sV_Phone_no_List[%d]=[%s] ",iPhone,sV_Phone_no_List[iPhone]);
			
    	    if(strncmp(sV_BillMode[i],"gn24",4)==0 || strncmp(sV_BillMode[i],"gn1m",4)==0) /* ������� */
    	    {
    	    	/*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H begin*/
	    		if(strcmp(vaccflag,"Y")==0)
				{
					
					pubLog_Debug(_FFL_, "s1114CfmF", "", "��ǰʱ��DD[%s]�ɹ���",vnowdate);
					
					if(strcmp(vcycleDay,vnowdate)==0)
					{
						ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    sV_Sys_date,
												sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
												sV_Sys_date,     "0",        "","",
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
							strcpy(return_code,"110473");
							sprintf(sErrorMess, "��ȡ����ʱ��ʧ��,vRetout=[%d] vErrMsg=[%s]\n",vRetout,vErrMsg);
							PUBLOG_DBDEBUG("s1114CfmF");
							pubLog_DBErr(_FFL_, "s1114CfmF", "%s", "%s", return_code,sErrorMess);
							goto end_valid;
						}
						pubLog_Debug(_FFL_, "s1114CfmF", "", "��һ�����ڿ�ʼʱ��[%s]�ɹ���",vCustAcctCycleBeginTime);
						pubLog_Debug(_FFL_, "s1114CfmF", "", "��һ�����ڽ���ʱ��[%s]�ɹ���",vCustAcctCycleEndTime);
						
						ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    vCustAcctCycleBeginTime,
												sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
												sV_Sys_date,     "1",        "","",
												vErrMsg);
					}
				}
    			else
    			{
					ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    iEffectTime,
											sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
											sV_Sys_date,     "1",        "","",
											vErrMsg);
				}
					/*���ֶ����ڸ��� ������� ������Ч 2012/5/29 15:03:35 S.H end*/
			}
			else
			{
				ret = pubBillBeginMode(sV_Phone_no_List[iPhone],    sV_BillMode[i],    sV_Sys_date,
										sIn_Op_Code,     sIn_Login_no,       sV_Login_accept,
										sV_Sys_date,     "0",        "","",
										vErrMsg);
			}
			if(ret!=0){
					sprintf(return_code,"%d",ret);            
					sprintf(sErrorMess,"�����ʷ�[%s]����%s",sV_BillMode[i],vErrMsg);
					goto end_valid;
			}       
        } 
        
        /* ng���� ҵ�񹤵����� by liuzhou at 20091019 begin */
		EXEC SQL SELECT Org_Code INTO :sOrgCode FROM dLoginMsg WHERE  Login_No = :sIn_Login_no;
        if(SQLCODE!=0)
        {
			PUBLOG_DBDEBUG("s1114CfmF");
			pubLog_DBErr(_FFL_,"s1114CfmF","-1","ȡ������Ϣ����!");
            goto end_valid;
        }
    	Trim(sOrgCode);
    	
        EXEC SQL SELECT DECODE(SUBSTR(:sIn_Belong_Code,1,2), '99', SUBSTR(:sOrgCode,1,2), SUBSTR(:sIn_Belong_Code,1,2))
               	   INTO :sRegionCode
                   From dual;
        if (SQLCODE != 0) {
			PUBLOG_DBDEBUG("s1114CfmF");
            pubLog_DBErr(_FFL_,"s1114CfmF","-2","��ѯdLoginMsg ���� [%d][%s]",SQLCODE,sqlca.sqlerrm.sqlerrmc);
            pubLog_DBErr(_FFL_,"s1114CfmF","-2","��ѯdLoginMsg����");
            goto end_valid;
        }
        
	    EXEC SQL select to_char(count(*)) into :sPayFlag from sPackCode where region_code=:sRegionCode and mach_code=:sV_Machine_Code;
	    if(atoi(sPayFlag)>0) /* ��Ӫ����Ԥ�� */
	        strcpy(sPayFlag,"j");
	    else if(strcmp(sIn_Sm_code,"d0")==0) /* ���ݿ� */
	        strcpy(sPayFlag,"k");
	    else
	        strcpy(sPayFlag,"0");
        
		memset(vParamList, 0, MAX_PARMS_NUM*2*DLINTERFACEDATA);
		memset(vCallServiceName, '\0', sizeof(vCallServiceName));
		strcpy(vCallServiceName, "bodb_sPDataCfm");
		vSendParaNum=18; /* ҵ�񹤵���������������� */
		vRecpParaNum=2;/* ҵ�񹤵���������������� */

		strcpy(vParamList[0][0], "id_no");
		strcpy(vParamList[0][1], sV_User_Id);
	    
		strcpy(vParamList[1][0], "contract_no");
		strcpy(vParamList[1][1], sV_Acct_Id);
		
		strcpy(vParamList[2][0], "prepay_fee");
		sprintf(vParamList[2][1], "%f", fV_Prepay_fee_List[iPhone]);
		
		strcpy(vParamList[3][0], "run_time");
		strcpy(vParamList[3][1], sV_Sys_date);
		
		strcpy(vParamList[4][0], "pay_type1");
		strcpy(vParamList[4][1], sPayFlag);

		strcpy(vParamList[5][0], "pay_type2");
    	if(fV_Check_Pay > 0)
    	{
        	strcpy(vParamList[5][1], "1");	
        }
        else
        {
			strcpy(vParamList[5][1], "0");	        	
        }	
				
		strcpy(vParamList[6][0], "total_date");
		strcpy(vParamList[6][1], sV_Total_date);
		
		strcpy(vParamList[7][0], "login_accept");
		strcpy(vParamList[7][1], sV_Login_accept);
		
		strcpy(vParamList[8][0], "con_cust_id");
		strcpy(vParamList[8][1], sIn_Cust_Id);
		
		strcpy(vParamList[9][0], "phone_no");
		strcpy(vParamList[9][1], sV_Phone_no_List[iPhone]);		
		
		strcpy(vParamList[10][0], "belong_code");
		strcpy(vParamList[10][1], sIn_Belong_Code);				
		
		strcpy(vParamList[11][0], "sm_code");
		strcpy(vParamList[11][1], sIn_Sm_code);			
				
		strcpy(vParamList[12][0], "login_no");
		strcpy(vParamList[12][1], sIn_Login_no);				
		
		strcpy(vParamList[13][0], "org_code");
		strcpy(vParamList[13][1], sOrgCode);			
				
		strcpy(vParamList[14][0], "op_code");
		strcpy(vParamList[14][1], sIn_Op_Code);			
			
		strcpy(vParamList[15][0], "pay_note");
		strcpy(vParamList[15][1], sIn_Op_Note);				
		
		strcpy(vParamList[16][0], "org_id");
		strcpy(vParamList[16][1], vOrgId);			
		
		strcpy(vParamList[17][0], "group_id");
		strcpy(vParamList[17][1], pGroupNo);				
		
		v_ret = 0;
		v_ret = SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
				                          ORDERIDTYPE_USER, sV_User_Id, iOrderRight, sV_Login_accept, sIn_Op_Code, sIn_Login_no, sIn_Op_Note);			
		if (0 != v_ret) 
		{
		    pubLog_Debug(_FFL_, "s1114CfmF", "", "����ҵ�񹤵�����,v_ret=[%d] ",v_ret);
		    strcpy(return_code, "111416");
		    strcpy(sErrorMess, "����ҵ�񹤵�����!");
		    goto end_valid;		
		}				                            
        /* ng���� ҵ�񹤵����� by liuzhou at 20091019 end   */ 
        
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
                       values(:sIn_Login_Accept, :pGroupNo, :sV_Sim_Type_List[iPhone],  
                              :sIn_Op_Code, :sIn_Login_no,to_date(:sV_Sys_date,'YYYYMMDD HH24:MI:SS'),  
                              :sV_Phone_no_List[iPhone], :sV_Phone_no_List[iPhone], :sIn_Sm_code,  
                              '1', :fV_Sum_Prepay_Fee/:iV_PhoneCount, :fV_Sum_Cash_Pay/:iV_PhoneCount,  
                              :vReward_Fee/:iV_PhoneCount, :vTax_Fee/:iV_PhoneCount, '04');
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetail�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(return_code,"111454");
            strcpy(sErrorMess,"����wnetamountdetail�����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,sErrorMess);
            goto end_valid;
            
            
        }
        
        EXEC SQL insert into wnetamountdetailhis
                             (LOGIN_ACCEPT, GROUP_ID, RES_CODE, 
                             OP_CODE, LOGIN_NO, OP_TIME, 
                             BEGIN_NO, END_NO, SM_CODE, 
                             SUM_NUM, SHOULD_FEE, REAL_FEE, 
                             REWARD_FEE, TAX_FEE, BUSI_CODE ,
                             UPDATE_OP_CODE, UPDATE_LOGIN, UPDATE_NOTE, 
                             UPDATE_TIME, UPDATE_ACCEPT, UPDATE_TYPE) 
                       values(:sIn_Login_Accept, :pGroupNo, :sV_Sim_Type_List[iPhone],  
                              :sIn_Op_Code, :sIn_Login_no,to_date(:sV_Sys_date,'YYYYMMDD HH24:MI:SS'),  
                              :sV_Phone_no_List[iPhone], :sV_Phone_no_List[iPhone], :sIn_Sm_code,  
                              '1', :fV_Sum_Prepay_Fee/:iV_PhoneCount, :fV_Sum_Cash_Pay/:iV_PhoneCount,  
                              :vReward_Fee/:iV_PhoneCount, :vTax_Fee/:iV_PhoneCount, '04',
                              :sIn_Op_Code,:sIn_Login_no,'ȫ��ͨ��������',
                              sysdate,:sIn_Login_Accept,'a');
        if(SQLCODE!=0)
        {
            pubLog_Debug(_FFL_, service_name, "", "����wnetamountdetailhis�����! sqlcode = [%d][%s]",SQLCODE,SQLERRMSG);
            strcpy(return_code,"110408");
            strcpy(sErrorMess,"����wnetamountdetailhis�����!");
            PUBLOG_DBDEBUG(service_name);
            pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,sErrorMess);
            goto end_valid;
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
				strcpy(return_code,"110412");
				strcpy(sErrorMess,"�����ж�Ϊ����,��ҳ�洫���������,�����²���!");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,sErrorMess);
				goto end_valid;
			}
			else
			{
				strcpy(return_code,"110413");
				strcpy(sErrorMess,"�ж��Ƿ񾻶�ʧ��!");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,sErrorMess);
				goto end_valid;
			}
		}
		/***************add for ��������֤ǰ̨����ķǾ������Ƿ���ȷ 2012.10.29 liujj end*******/
    	      
    }  /* end -- for ((i=0;i<iV_PhoneCount;i++)  */
    /* ��¼֧Ʊ��Ϣ */  
    if(fV_Sum_Check_Pay>0)
    {
        ret=-1;
        ret=pubOpenAcctCheck_2(sIn_Check_No,   fV_Sum_Check_Pay,   sIn_Bank_Code,
                           sIn_Op_Note,      sIn_Login_no,       sV_Login_accept, 
                           sIn_Op_Code,      sV_Total_date,      sV_Sys_date,
                           sIn_System_Note);
        if(ret != 0){
            pubLog_Debug(_FFL_,"s1114CfmF","","�޸�֧Ʊ��Ϣ����,ret=[%d] ",ret);
            strcpy(return_code,"110409");
            strcpy(sErrorMess, "�޸�֧Ʊ��Ϣ����!");
            goto end_valid;
        }
    }
    
    /* ng���� ҵ�񹤵����� by liuzhou at 20091019 begin */
	memset(vParamList, 0, MAX_PARMS_NUM*2*DLINTERFACEDATA);
	memset(vCallServiceName, '\0', sizeof(vCallServiceName));
	strcpy(vCallServiceName, "bodb_sCheckCfm");
	vSendParaNum=4; /* ҵ�񹤵���������������� */
	vRecpParaNum=2;/* ҵ�񹤵���������������� */

	strcpy(vParamList[0][0], "op_time");
	strcpy(vParamList[0][1], sV_Sys_date);
    
	strcpy(vParamList[1][0], "bank_code");
	strcpy(vParamList[1][1], sIn_Bank_Code);
	
	strcpy(vParamList[2][0], "check_no");
	strcpy(vParamList[2][1], sIn_Check_No);
	
	strcpy(vParamList[3][0], "check_pay");
	sprintf(vParamList[3][1], "%f", fV_Sum_Check_Pay);
	
	v_ret = 0;
	v_ret = SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
			                          ORDERIDTYPE_USER, sV_User_Id, iOrderRight, sV_Login_accept, sIn_Op_Code, sIn_Login_no, sIn_Op_Note);			
	if (0 != v_ret) 
	{
	    pubLog_Debug(_FFL_, "s1114CfmF", "", "����ҵ�񹤵��޸�֧Ʊ��Ϣ����,v_ret=[%d] ",v_ret);
	    strcpy(return_code, "110409");
	    strcpy(sErrorMess, "����ҵ�񹤵��޸�֧Ʊ��Ϣ����!");
	    goto end_valid;		
	}				                    
    /* ng���� ҵ�񹤵����� by liuzhou at 20091019 end   */
    
    
    if (strcmp(sIn_PayFlag,"1")==0)
    {
        fV_Sum_Cash_Pay = fV_Sum_Check_Pay;	
    }
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"s1114CfmF","","fV_Sum_Cash_Pay     = [%f] ",fV_Sum_Cash_Pay);
        pubLog_Debug(_FFL_,"s1114CfmF","","begin insert wLoginOprYYYYMM  ");
    #endif

    /* ���� wLoginOprYYYYMM �� */
    /* 
        ���������� ���ܵ�����¼�ֻ������Id_no , ����ʹ��phone_no �ֶ�����¼ �� 
        ��һ �� ������������־ �� Id_no �� 0
    
    */
    /*
    init(vTmp_List);
    sprintf(vTmp_List,"insert into wLoginOpr%s(total_date,login_accept,op_code,pay_type,pay_money,sm_code,id_no,\
            phone_no,org_code,login_no,op_time,op_note,IP_ADDR) \
        values(to_number(:sV_Total_date),to_number(:sV_Login_accept),:sIn_Op_Code,:sIn_PayFlag,%f,:sIn_Sm_code,0,\
            :sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,to_date(:sV_Sys_date,'yyyymmdd hh24:mi:ss'),:sIn_System_Note,\
            :sIn_Ip_Address)",sV_YYYYMM,fV_Sum_Cash_Pay);
        
    pubLog_Debug(_FFL_,"s1114CfmF","","insert wLoginOpr%s' sql_str = [%s] ",sV_YYYYMM,vTmp_List);
    EXEC SQL PREPARE ins_stmt From :vTmp_List; 
    EXEC SQL EXECUTE ins_stmt using :sV_Total_date,:sV_Login_accept,:sIn_Op_Code,:sIn_PayFlag,:sIn_Sm_code,
                                    :sIn_CustDocFlag,:sIn_Org_code,:sIn_Login_no,:sV_Sys_date,:sIn_System_Note,
                                    :sIn_Ip_Address; 
    
    if(SQLCODE != 0)
    {
        strcpy(return_code,"111413");
        strcpy(sErrorMess, "����wLoginOpr�����!");
        goto end_valid;
    }
    
    */ 
/*********��֯�������쿪����������  edit by liupenga at 31/03/2009  begin*******/
    vLog.vId_No                            =  0L;
    vLog.vTotal_Date                       =  atol(sV_Total_date);
    vLog.vLogin_Accept                     =  atol(sV_Login_accept);

	 strncpy(vLog.vSm_Code,       sIn_Sm_code,        		2);
	 strncpy(vLog.vBelong_Code,   sIn_Belong_Code,         	7);
	 strncpy(vLog.vPhone_No,      sIn_CustDocFlag,         15);
	 strncpy(vLog.vOrg_Code,      sIn_Org_code,       		9);
	 strncpy(vLog.vLogin_No,      sIn_Login_no,       		6);
	 strncpy(vLog.vOp_Code,       sIn_Op_Code,        		4);
	 strncpy(vLog.vOp_Time,       sV_Sys_date,             17);
	 strncpy(vLog.vMachine_Code,      "0",        			3);
	 strncpy(vLog.vBack_Flag,         "0",        	    	1);
	 strncpy(vLog.vEncrypt_Fee,       "0",        		   16);
	 strncpy(vLog.vSystem_Note,   sIn_System_Note,		   60);
	 strncpy(vLog.vOp_Note,       sIn_Op_Note,			   60);
	 strncpy(vLog.vGroup_Id,      pGroupNo,       	 	   10);
	 strncpy(vLog.vOrg_Id,        vOrgId,        		   10);
	 strncpy(vLog.vPay_Type,      sIn_PayFlag,        		1);
	 strncpy(vLog.vIp_Addr,       sIn_Ip_Address,		   15);
	 vLog.vPay_Money                                  = fV_Sum_Cash_Pay;
	 vLog.vCash_Pay                                   = 0;
	 vLog.vCheck_Pay          											   = 0;
	 vLog.vSim_Fee            											   = 0;
	 vLog.vMachine_Fee        											   = 0;
	 vLog.vInnet_Fee          											   = 0;
	 vLog.vChoice_Fee         											   = 0;
	 vLog.vOther_Fee           											   = vTax_Fee - vReward_Fee;
	 vLog.vHand_Fee           											   = 0;
	 vLog.vDeposit            											   = 0;
   
   	Vret = 0;
   	strcpy(vChgFlag,"N");
   	strcpy(vCustFlag,"N");
  
    pubLog_Debug(_FFL_,"s1114CfmF","","========%ld==== ",vLog.vId_No); 
  
   	Vret = recordOprLog(&vLog,sErrorMess,vChgFlag,vCustFlag);
   	if(Vret != 0)
   	{
   	 	sprintf(return_code,"%06d",Vret);
	    goto end_valid;
   	}   
  /*********��֯�������쿪����������  edit by liupenga at 31/03/2009  end*******/  
                                
    strcpy(return_code,"000000");
        
end_valid:
        /* ѹ�뷵����Ϣ  */
    pubLog_Debug(_FFL_,"s1114CfmF","","return_code �� [%s],sErrorMess=[%s] ",return_code,sErrorMess);
    pubLog_Debug(_FFL_,"s1114CfmF","","SQLCODE = [%d]  ,SQLERRMSG = [%s] ",SQLCODE,SQLERRMSG);
        
    if(!strcmp(return_code,"000000"))
        EXEC SQL commit;
    else
        exec sql rollback;
    	
    transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50,
        service_name,GPARM32_0,return_code,"dbchange",CONNECT0);
    transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50,
        service_name,GPARM32_1,sErrorMess,"dbchange",CONNECT0);
    transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50,
        service_name,GPARM32_2,sV_Login_accept,"dbchange",CONNECT0);
    
    
    pubLog_Debug(_FFL_,"s1114CfmF","","==========end  s1114CfmF ============== "); 
    
    Fchg32(transOUT,SVC_ERR_NO32,0,return_code,(FLDLEN32)0);
    Fchg32(transOUT,SVC_ERR_MSG32,0,sErrorMess,(FLDLEN32)0);

    
    /*��ȡ��������ʱ��*/
	getTimeDif(&vUsedTime,vStartSec);

	/*Ϊ������־�ṹ�帳ֵ*/
	strcpy(logShip.vBegin_Time,vStartTime);
	logShip.vUsed_Time = vUsedTime;
	logShip.vLogion_Accept = atol(sV_Login_accept);
	strcpy(logShip.vLogin_No,sIn_Login_no);
	strcpy(logShip.vOp_Code,sIn_Op_Code);
	strcpy(logShip.vServ_Name,service_name);
	strcpy(logShip.vPhone_No,sIn_CustDocFlag);
	logShip.vId_No = atol(sV_User_Id);
	logShip.vOther_No = 0;
	strcpy(logShip.vRetCode,return_code);
	strcpy(logShip.vRetMsg,sErrorMess); 
	/*����������־��־λ*/
	SetBomcLogFlag(&logShip);
	/*��¼������־*/
	WriteBomcLog(&logShip);	
    
    tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0);   
}
