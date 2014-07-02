/**********************
 * 源文件名：pubPbossInterFace.cp
 * 版本号：1.0
 * 创建作者：lugy
 * 创建时间：2012-10-12
 * 代码依赖：
 * 功能描述：物联网CRM向PBoss侧发送请求核心函数
 *   
 * 修改历史:
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
函数名称:sendPerToPbossInfo
编码时间:2012/10/12
编码人员:lugy
功能描述:用户信息与个人产品订购关系信息同步（发起方）
输入参数:
    ProcID	                    发起方填写的包含此交易业务的流水号
    TransIDO	                在发起方唯一标识一个交易的流水号，系统内唯一
    ProcessTime	                "发起方发起请求的时间/应答方处理请求的时间YYYYMMDDHHMMSS"
    OprSeq	                    本次操作的流水
    OprTime	                    操作时间
    OprCode	                    操作编码
    ApplyChannel	            业务受理的渠道
    PersonInfo_ProdID	        个人产品编码
    PersonInfo_ProdInstID	    个人产品订购实例唯一标识
    PersonInfo_SubsID	        用户ID
    PersonInfo_ProdInstEffTime	个人产品订购实例生效时间
    PersonInfo_ProdInstExpTime	个人产品订购实例失效时间
    ProdInfo_ProdID	            原子产品编码
    PkgProdID	                归属产品包编码
    ProdInfo_ProdInstID	        产品订购实例唯一标识
    ProdInfo_SubsID	            用户ID
    ProdInfo_ProdInstEffTime	产品订购实例生效时间
    ProdInfo_ProdInstExpTime	产品订购实例失效时间
    ProdInfo_OprType	        操作类型
    ServiceID	                服务编码
    AttrKey	                    附加属性编码
    AttrValue	                附加属性值
    ProdAttrInfo_OprType	    操作类型
    MobNum	                    终端MSISDN
    CardType	                卡类型
    CardPhysicalType	        卡物理类型
    IMSI	                    IMSI
    kKeyID	                    K密钥信息标识
    opcKeyID	                OPC密钥信息标识
    MobIMEI	                    终端IMEI
    ProvinceID	                销售省编码
    UserStatus	                用户状态
    OpenTime	                开户时间
    UseTime	                    计费时间
    UserNum	                    客户联系电话
    Addr	                    地址
    Email	                    Email
    PostCode	                邮编
    Fax	                        传真
    IndustryID	                应用行业
    SubsInfo_SubsID	            用户ID
    ServNumber	                服务号码
    SubsInfo_ProdID	            对应的主体产品编码
    CreateDate	                创建时间
    StartDate	                开始时间
    InvalidDate	                失效时间
    Status	                    用户状态
    InfoCode	                信息代码
    InfoValue	                信息值
输出参数:
    返 回 值:0代表正确返回,其他错误
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
	
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "开始调用sendPerToPbossInfo函数！");
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
		strcpy(oRetMsg, "查询dpbossusermsg失败!");
		PUBLOG_APPERR("sendPerToPbossInfo","", "select dpbossusermsg [%s][%d]",oRetMsg,SQLCODE);
		return -1;
	}
	
	pubLog_Debug(_FFL_, "sendPerToPbossInfo", "", "开始调用应用集成平台");
	
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
		PUBLOG_DEBUG("sendPerToPbossInfo", "", "用户信息与个人产品订购关系信息同步基本参数=[%s]",vInmsg);
	#endif
	
	PUBLOG_APPERR("sendPerToPbossInfo","","vNewaddress=[%s]",vNewaddress);
	PUBLOG_APPERR("sendPerToPbossInfo","","vAppid=[%d]",vAppid);
	PUBLOG_APPERR("sendPerToPbossInfo","","vPriority=[%d]",vPriority);
	PUBLOG_APPERR("sendPerToPbossInfo","","vTranscode=[%s]",vTranscode);
	PUBLOG_APPERR("sendPerToPbossInfo","","vInmsg=[%s]",vInmsg);
		
	/**调取应用平台接口**/
	
	ret = LG_Getinter(vNewaddress,vAppid,vPriority,vTranscode,vInmsg,vOutmsg);
	if( ret != 0 )
	{
		strcpy(oRetMsg, "调用LG_Getinter接口失败!");
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
