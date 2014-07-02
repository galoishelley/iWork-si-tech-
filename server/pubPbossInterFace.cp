/**********************
 * Դ�ļ�����pubPbossInterFace.cp
 * �汾�ţ�1.0
 * �������ߣ�lugy
 * ����ʱ�䣺2012-10-12
 * ����������
 * ����������������CRM��PBoss�෢��������ĺ���
 *   
 * �޸���ʷ:
 *   
 *********************/
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"
#include <sys/timeb.h>
#include <time.h>

#define ROWLEN 2048

#define _DEBUG_
/*#undef _DEBUG_*/

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;
/************************************************************************
��������:sendPerToPbossInfo
����ʱ��:2012/10/12
������Ա:lugy
��������:�û���Ϣ����˲�Ʒ������ϵ��Ϣͬ�������𷽣�
�������:
    ProcID	                    ������д�İ����˽���ҵ�����ˮ��
    TransIDO	                �ڷ���Ψһ��ʶһ�����׵���ˮ�ţ�ϵͳ��Ψһ
    ProcessTime	                "���𷽷��������ʱ��/Ӧ�𷽴��������ʱ��YYYYMMDDHHMMSS"
    OprSeq	                    ���β�������ˮ
    OprTime	                    ����ʱ��
    OprCode	                    ��������
    ApplyChannel	            ҵ�����������
    PersonInfo_ProdID	        ���˲�Ʒ����
    PersonInfo_ProdInstID	    ���˲�Ʒ����ʵ��Ψһ��ʶ
    PersonInfo_SubsID	        �û�ID
    PersonInfo_ProdInstEffTime	���˲�Ʒ����ʵ����Чʱ��
    PersonInfo_ProdInstExpTime	���˲�Ʒ����ʵ��ʧЧʱ��
    ProdInfo_ProdID	            ԭ�Ӳ�Ʒ����
    PkgProdID	                ������Ʒ������
    ProdInfo_ProdInstID	        ��Ʒ����ʵ��Ψһ��ʶ
    ProdInfo_SubsID	            �û�ID
    ProdInfo_ProdInstEffTime	��Ʒ����ʵ����Чʱ��
    ProdInfo_ProdInstExpTime	��Ʒ����ʵ��ʧЧʱ��
    ProdInfo_OprType	        ��������
    ServiceID	                �������
    AttrKey	                    �������Ա���
    AttrValue	                ��������ֵ
    ProdAttrInfo_OprType	    ��������
    MobNum	                    �ն�MSISDN
    CardType	                ������
    CardPhysicalType	        ����������
    IMSI	                    IMSI
    kKeyID	                    K��Կ��Ϣ��ʶ
    opcKeyID	                OPC��Կ��Ϣ��ʶ
    MobIMEI	                    �ն�IMEI
    ProvinceID	                ����ʡ����
    UserStatus	                �û�״̬
    OpenTime	                ����ʱ��
    UseTime	                    �Ʒ�ʱ��
    UserNum	                    �ͻ���ϵ�绰
    Addr	                    ��ַ
    Email	                    Email
    PostCode	                �ʱ�
    Fax	                        ����
    IndustryID	                Ӧ����ҵ
    SubsInfo_SubsID	            �û�ID
    ServNumber	                �������
    SubsInfo_ProdID	            ��Ӧ�������Ʒ����
    CreateDate	                ����ʱ��
    StartDate	                ��ʼʱ��
    InvalidDate	                ʧЧʱ��
    Status	                    �û�״̬
    InfoCode	                ��Ϣ����
    InfoValue	                ��Ϣֵ
�������:
    �� �� ֵ:0������ȷ����,��������
************************************************************************/
int sendPerToPbossInfo(const char* loginAccept, const char* idNo, char *oRetMsg)
{

	EXEC SQL BEGIN DECLARE SECTION;
		long lLoginAccept = atol(loginAccept);
		long lCustId=0;
		int i=0, ret=0, iCount = 0;
			
		int vAppid=0;
		int vPriority=1;
		char vNewaddress[100];
		char vTranscode[100];
		char vInmsg[8000];
		char vOutmsg[ROWLEN];
		char vRetCodeAdc[20+1];
		char temMsg[1024+1];
		char TmpSqlStr[1000+1]; 
		char vParamValue[128+1];
		char retCode[6+1];
		int  tmplen=0,begin=1,end=1,num=0,k=0;
		char vTmp_List[2048];
		char vTmpValue[256];
		char vOprNumb[17+1];
		          
		char serviceId[16+1];       
		char attrKey[16+1];         
		char attrValue[20+1];       
		char mobNum[20+1];          
		char cardType[2+1];         
		char cardPhysicalType[2+1];
		char imsi[2+1];            
		char kKeyID [32+1];         
		char opcKeyID[32+1];        
		char mobIMEI[16+1];         
		char provinceId[16+1];      
		char userStatus[2+1];       
		char openTime[17+1];        
		char useTime[17+1];         
		char industryId[2+1];       
		
		
	EXEC SQL END DECLARE SECTION;
	
	int  v_ret=0;
	TDIAGWSERVOPERIndex tDIAGWSERVOPERIndex;
	TDIAGWSERVOPER tDIAGWSERVOPER;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sTempSqlStr[1024];
	
	memset(&tDIAGWSERVOPERIndex,0,sizeof(tDIAGWSERVOPERIndex));
	memset(&tDIAGWSERVOPER,0,sizeof(tDIAGWSERVOPER));
	init(v_parameter_array);
	init(sTempSqlStr);
	
	Sinitn(vOprNumb);
	Sinitn(vNewaddress);
	Sinitn(vTranscode);
	Sinitn(vInmsg);
	Sinitn(vOutmsg);
	Sinitn(vRetCodeAdc);
	Sinitn(temMsg);
	Sinitn(TmpSqlStr);
	Sinitn(retCode);
	Sinitn(vTmp_List);
	Sinitn(vTmpValue);
	
	Sinitn(serviceId);
	Sinitn(attrKey);
	Sinitn(attrValue);
	Sinitn(mobNum);
	Sinitn(cardType);
	Sinitn(cardPhysicalType);
	Sinitn(imsi);
	Sinitn(kKeyID);
	Sinitn(opcKeyID);
	Sinitn(mobIMEI);
	Sinitn(provinceId);
	Sinitn(userStatus);
	Sinitn(openTime);
	Sinitn(useTime);
	Sinitn(industryId);
	
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "��ʼ����sendPerToPbossInfo������");
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "input lLoginAccept = [%ld]", lLoginAccept);
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "input idNo = [%s]", idNo);
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "input oRetMsg = [%s]", oRetMsg);
	
	EXEC SQL select 
			    SERVICE_ID, ATTR_KEY, ATTR_VALUE, MOB_NUM, 
			    CARD_TYPE, CARDPHYSICAL_TYPE,IMSI, K_KEYID, OPC_KEYID,
			    MOB_IMEI, PROVINCE_ID, USER_STATUS,OPEN_TIME,USE_TIME,
			    INDUSTRYID
			 into
			    :serviceId, :attrKey, :attrValue, :mobNum, 
			    :cardType, :cardPhysicalType, :imsi, :kKeyID, :opcKeyID, 
			    :mobIMEI, :provinceId, :userStatus, :openTime, :useTime, 
			    :industryId
			 from dpbossusermsg
		    where id_no = :idNo;
	if (SQLCODE != 0)
	{		 
		strcpy(oRetMsg, "��ѯdpbossusermsgʧ��!");
		PUBLOG_APPERR("sendPerToPbossInfo","", "select dpbossusermsg [%s][%d]",oRetMsg,SQLCODE);
		return -1;
	}
	
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "��ʼ����Ӧ�ü���ƽ̨");
	
	sprintf(vInmsg,"ProcID=%ld,",lLoginAccept);
	sprintf(temMsg,"TransIDO=%s,","11"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProcessTime=%s,","22"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"OprSeq=%s,","33"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"OprTime=%s,","44"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"OprCode=%s,","55"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ApplyChannel=%s,","66"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PersonInfo_ProdID=%s,","77"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PersonInfo_ProdInstID=%s,","88"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PersonInfo_SubsID=%s,","99"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PersonInfo_ProdInstEffTime=%s,","1a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PersonInfo_ProdInstExpTime=%s,","2a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_ProdID=%s,","3a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PkgProdID=%s,","4a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_ProdInstID=%s,","5a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_SubsID=%s,","6a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_ProdInstEffTime=%s,","7a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_ProdInstExpTime=%s,","8a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdInfo_OprType=%s,","9a"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ServiceID=%s,","1b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"AttrKey=%s,","2b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"AttrValue=%s,","3b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProdAttrInfo_OprType=%s,","4b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"MobNum=%s,","5b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"CardType=%s,","6b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"CardPhysicalType=%s,","7b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"IMSI=%s,","8b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"kKeyID=%s,","9b"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"opcKeyID=%s,","1c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"MobIMEI=%s,","2c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ProvinceID=%s,","3c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"UserStatus=%s,","4c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"OpenTime=%s,","5c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"UseTime=%s,","6c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"UserNum=%s,","7c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"Addr=%s,","8c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"Email=%s,","9c"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"PostCode=%s,","1d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"Fax=%s,","2d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"IndustryID=%s,","3d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"SubsInfo_SubsID=%s,","4d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"ServNumber=%s,","5d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"SubsInfo_ProdID=%s,","6d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"CreateDate=%s,","7d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"StartDate=%s,","8d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"InvalidDate=%s,","9d"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"Status=%s,","1e"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"InfoCode=%s,","2e"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"InfoValue=%s,","3e"); strcat(vInmsg,temMsg); Sinitn(temMsg);
	
	
	sprintf(vNewaddress,"ADDRESS=%s",getenv("ADDRESS"));
	vAppid=701001;
	strcpy(vTranscode, "10");
	
	#ifdef _DEBUG_
		PUBLOG_DEBUG("sendPerToPbossInfo", "", "�û���Ϣ����˲�Ʒ������ϵ��Ϣͬ����������=[%s]",vInmsg);
	#endif
	
	PUBLOG_APPERR("sendPerToPbossInfo","","vNewaddress=[%s]",vNewaddress);
	PUBLOG_APPERR("sendPerToPbossInfo","","vAppid=[%d]",vAppid);
	PUBLOG_APPERR("sendPerToPbossInfo","","vPriority=[%d]",vPriority);
	PUBLOG_APPERR("sendPerToPbossInfo","","vTranscode=[%s]",vTranscode);
	PUBLOG_APPERR("sendPerToPbossInfo","","vInmsg=[%s]",vInmsg);
		
	/**��ȡӦ��ƽ̨�ӿ�**/
	
	ret = LG_Getinter(vNewaddress,vAppid,vPriority,vTranscode,vInmsg,vOutmsg);
	if( ret != 0 )
	{
		strcpy(oRetMsg, "����LG_Getinter�ӿ�ʧ��!");
		PUBLOG_APPERR("sendPerToPbossInfo","", "LG_Getinter send fail ret=[%s][%d]",oRetMsg,ret);
		return -2;
	}
	
	strcat(vOutmsg,",");
	PUBLOG_APPERR("sendPerToPbossInfo","","vOutmsg=[%s]",vOutmsg);
	
	Trim(vOutmsg);
	
	if(strlen(ltrim(rtrim(vOutmsg)))!=0)
	{
		memset(vTmp_List,0x00,sizeof(vTmp_List));
		memcpy(vTmp_List,vOutmsg,sizeof(vOutmsg)-1);
		tmplen=strlen(vTmp_List);
		begin = 1; end = 1; k=0;
		for(i=0;i<=tmplen;i++)
		{
			if(vTmp_List[i]==',')
			{
				Sinitn(vTmpValue);
				k=k+1;
				end = i;
				num = end - begin + 1;
				if(k==5) strcpy(vRetCodeAdc,Ssubstr(vTmp_List,begin,num,vTmpValue));
				begin = end +2;
			}
		}
	}
	
	PUBLOG_APPERR("sendPerToPbossInfo","","vRetCodeAdc=[%s]",vRetCodeAdc);
	
	return 0;
}
