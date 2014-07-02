#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"
#include <sys/timeb.h>
#include <time.h>

#define ROWLEN 2048

EXEC SQL INCLUDE SQLCA;

/******************************************************************
** ��������	wideAreaNewWork(�ɵ�����ƴװ)
** ��������	2010-11-2 11:09:43
** ���ù��̣�
** ����������ƴװ�ɵ������������ģ���������ɵ������������ġ�
** ���������

** 		businessType ҵ������
** 		busiName     ҵ������
** 		specType     רҵ����
** 		orderId      CRM�ඩ����
** 		userId       CRM����û���ƷΨһ��ʶ
** 		productId    ��Ʒ����[O]
** 		productName  ��Ʒ����
** 		serviceNum   ����˺�
** 		serviceKey   �������
** 		customNo     �ͻ�����[O]
** 		customName   �ͻ�����[O]
** 		linkman      ��ϵ��
** 		contactPhone ��ϵ�˵绰
** 		addrId       ��ַ���
** 		addrName     ��ַ����
** 		userGroupid  �û�����groupId
** 		appointDate  ԤԼʱ��
** 		description  ��ע[O]
** 		login_No     ��������

** ���ز�����(ʾ������)
** 		businessType=002,BusiName=���ҵ��ͨ,specType=006,orderId=2010111012931343,
** 		userId=10200043107355,productId=006,productName=���˿��,serviceNum=cc10010132546,
** 		serviceKey=BGCEQz,customNo=1032307886,customName=�������,linkman=�������,
** 		contactPhone=13656780987,addrId=103425,addrName=���ֳ����Ϲ������ɴ�·2671���Ҹ�С��1��2��Ԫ2��202��,
** 		districtId=85,districtName=��������,appointDate=,description=����Աaa021A���û�10010132546���п����������,
** 		siteName=�Խ���Ӫ�����л���Ӫҵ��
*****************************************************************/

char *wideAreaNewWork(char *businessType,char *busiName,char *specType,char *orderId,char *userId,char *productId,
					  char *productName,char *serviceNum,char *serviceKey,char *customNo,
					  char *customName,char *linkman,char *contactPhone,char *addrId,
					  char *addrName,char *userGroupid,char *appointDate,char *description,char *login_No)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vBusinessType[5+1];/**ҵ������**/
	char vBusiName[32+1];/**ҵ������**/
	char vSpecType[5+1];/**רҵ����**/
	char vOrderId[20+1];/**CRM�ඩ����**/
	char vUserId[14+1];/**CRM����û���Ʒʵ����ʶ����Ϊ�û�ռ����Դ��Ψһ��ʶ��**/
	char vProductId[2+1];/**��Ʒ���룬��ҪCRM�ṩ�����б�2λ**/
	char vProductName[32+1];/**��Ʒ����**/
	char vServiceNum[20+1];/**����˺�**/
	char vServicePass[32+1];/**�������**/
	char vCustomNo[20+1];/**�ͻ�����**/
	char vCustomName[300+1];/**�ͻ�����**/
	char vLinkman[300+1];/**��ϵ��**/
	char vContactPhone[300+1];/**��ϵ�绰**/
	char vAddrId[20+1];/**��ַ���**/
	char vAddName[100+1];/**�ͻ���ַ**/
	char vDistrictId[30+1];/**С������**/
	char vDistrictName[300+1];/**С������**/
	char vAppointDate[32+1];/**ԤԼʩ��ʱ���ʽ��YYYY-MM-DD HH24:MM:SS������Ϊ��**/
	char vDescription[500+1];/**��ע**/
	char vSiteName[300+1];/**Ӫҵ������**/
	static char retMsg[8000+1];/**���ص���Ϣ**/
	char temMsg[1024+1];
	char vUserGroupId[10+1];/**�û�����groupId**/
	char vLogin_No[6+1];/**����**/
	EXEC SQL END   DECLARE SECTION;

	Sinitn(vBusinessType);Sinitn(vSpecType);Sinitn(vOrderId);Sinitn(vUserId);
	Sinitn(vProductId);Sinitn(vServiceNum);Sinitn(vServicePass);Sinitn(vCustomNo);
	Sinitn(vCustomName);Sinitn(vLinkman);Sinitn(vContactPhone);Sinitn(vAddrId);
	Sinitn(vAddName);Sinitn(vDistrictId);Sinitn(vDistrictName);Sinitn(vAppointDate);
	Sinitn(vDescription);Sinitn(vSiteName);Sinitn(retMsg);Sinitn(temMsg);
	Sinitn(vUserGroupId);Sinitn(vLogin_No);Sinitn(vBusiName);Sinitn(vProductName);

	sprintf(vBusinessType,businessType);
	sprintf(vSpecType,specType);
	sprintf(vOrderId,orderId);
	sprintf(vUserId,userId);
	sprintf(vProductId,productId);
	sprintf(vServiceNum,serviceNum);
	sprintf(vServicePass,serviceKey);

	spublicEnDePasswd(serviceKey,vServicePass, MASTERKEY,DECRYPT);

	sprintf(vCustomNo,customNo);
	sprintf(vCustomName,customName);
	sprintf(vLinkman,linkman);
	sprintf(vContactPhone,contactPhone);
	sprintf(vAddrId,addrId);
	sprintf(vAddName,addrName);
	sprintf(vUserGroupId,userGroupid);
	sprintf(vBusiName,busiName);
	sprintf(vProductName,productName);
	Trim(vUserGroupId);

	if(strlen(vUserGroupId)!=0)
	{
		EXEC SQL SELECT TO_CHAR(B.GROUP_ID),B.GROUP_NAME
					INTO :vDistrictId,vDistrictName
					FROM DCHNGROUPINFO A,DCHNGROUPMSG B
					WHERE B.GROUP_ID=A.PARENT_GROUP_ID
					AND A.GROUP_ID=:vUserGroupId
					AND B.ROOT_DISTANCE=3;

		if (SQLCODE != 0 && SQLCODE!=1403)
		{
			sprintf(vDistrictId,"");
			sprintf(vDistrictName,"");
		}
	}

	if(strlen(vDistrictId)==0 && strlen(vLogin_No)!=0)
	{
		EXEC SQL  SELECT TO_CHAR(B.GROUP_ID),B.GROUP_NAME
					INTO :vDistrictId,vDistrictName
					FROM DCHNGROUPINFO A,DCHNGROUPMSG B,DLOGINMSG C
					WHERE B.GROUP_ID=A.PARENT_GROUP_ID
					AND A.GROUP_ID=C.ORG_ID
					AND B.ROOT_DISTANCE=3
					AND C.LOGIN_NO=:vLogin_No;
		if (SQLCODE != 0 && SQLCODE!=1403)
		{
			sprintf(vDistrictId,"");
			sprintf(vDistrictName,"");
		}
	}

	if(strlen(appointDate)!=0)
	{
		EXEC SQL SELECT TO_CHAR(TO_DATE(TRIM(:appointDate),'YYYY-MM-DD HH24:MI:SS'),'YYYY-MM-DD HH24:MI:SS')
						INTO :vAppointDate
						FROM DUAL;
	}

	sprintf(vDescription,description);
	sprintf(vLogin_No,login_No);
	Trim(vLogin_No);

	if(strlen(vLogin_No)!=0)
	{
		EXEC SQL SELECT B.GROUP_NAME
					INTO :vSiteName
					FROM DLOGINMSG A,DCHNGROUPMSG B
					WHERE B.GROUP_ID=A.ORG_ID AND A.LOGIN_NO=:vLogin_No;

		if (SQLCODE != 0)
		{
			sprintf(vSiteName,"");
		}
	}
	if(strlen(vCustomName) == 0 && strlen(vCustomNo) != 0)
	{
		Sinitn(vCustomName);
		EXEC SQL SELECT CUST_NAME INTO :vCustomName
				 FROM DCUSTDOC
				 WHERE CUST_ID=to_number(:vCustomNo);

		if(SQLCODE != 0 && SQLCODE !=1403 )
		{
			strcpy(vCustomName," ");
		}
	}

	Trim(vBusinessType);Trim(vSpecType);Trim(vOrderId);Trim(vUserId);
	Trim(vProductId);Trim(vServiceNum);Trim(vServicePass);Trim(vCustomNo);
	Trim(vCustomName);Trim(vLinkman);Trim(vContactPhone);Trim(vAddrId);
	Trim(vAddName);Trim(vDistrictId);Trim(vDistrictName);Trim(vAppointDate);
	Trim(vDescription);Trim(vSiteName);Trim(vBusiName);Trim(vProductName);

#ifdef _DEBUG_
	PUBLOG_DEBUG("wideAreaNewWork", "", "vBusinessType=[%s]",vBusinessType);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vBusiName=[%s]",vBusiName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vSpecType=[%s]",vSpecType);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vOrderId=[%s]",vOrderId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vUserId=[%s]",vUserId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vProductId=[%s]",vProductId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vProductName=[%s]",vProductName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vServiceNum=[%s]",vServiceNum);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vServicePass=[%s]",vServicePass);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vCustomNo=[%s]",vCustomNo);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vCustomName=[%s]",vCustomName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vLinkman=[%s]",vLinkman);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vContactPhone=[%s]",vContactPhone);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vAddrId=[%s]",vAddrId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vAddName=[%s]",vAddName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vDistrictId=[%s]",vDistrictId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vDistrictName=[%s]",vDistrictName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vAppointDate=[%s]",vAppointDate);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vDescription=[%s]",vDescription);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vSiteName=[%s]",vSiteName);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vUserGroupId=[%s]",vUserGroupId);
	PUBLOG_DEBUG("wideAreaNewWork", "", "vLogin_No=[%s]",vLogin_No);
#endif

	sprintf(retMsg,"businessType=%s,",vBusinessType);
	sprintf(temMsg,"BusiName=%s,",vBusiName); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"specType=%s,",vSpecType); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"orderId=%s,",vOrderId); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"userId=%s,",vUserId); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"productId=%s,",vProductId); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"productName=%s,",vProductName); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"serviceNum=%s,",vServiceNum); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"serviceKey=%s,",vServicePass); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"customNo=%s,",vCustomNo); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"customName=%s,",vCustomName); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"linkman=%s,",vLinkman); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"contactPhone=%s,",vContactPhone); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"addrId=%s,",vAddrId); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"addrName=%s,",vAddName); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"districtId=%s,",vDistrictId); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"districtName=%s,",vDistrictName); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"appointDate=%s,",vAppointDate); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"description=%s,",vDescription); strcat(retMsg,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"siteName=%s",vSiteName); strcat(retMsg,temMsg); Sinitn(temMsg);

	#ifdef _DEBUG_
		PUBLOG_DEBUG("wideAreaNewWork", "", "PBOSS�����ɷ���������=[%s]",retMsg);
	#endif

	return retMsg;
}

/*****************************************************
** ��������	wideAreaPublicParams(�������������������)
** ��������	2010-11-2 11:09:43
** ���ù��̣�
** ������������������������ġ�

** ���������

**   InLoginNo ��������
**   InLoginAccept ������ˮ
**   InGroupId ���Ź���

** ���ز���:(ʾ������)

**	 systemName=JL_PBOSS,serCaller=JL_CRM,callerPwd=,callTime=2010-11-08 11:01:37,
**	 callUserId=aa021A,callUserName=����ҵ�����,callUserPhone=15164387636,recSeq=1012931343,
**	 cityId=3,cityName=��������,countyId=14,countyName=������,orderSource=order_source_crm

*******************************************************/

char *wideAreaPublicParams(char *InLoginNo,char *InLoginAccept,char *InGroupId)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char vSystemName[10+1];/**�����ṩ������ʽΪ��ʡ�ݱ���_ϵͳ����**/
	char vSerCaller[10+1];/**������÷�����ʽΪ��ʡ�ݱ���_ϵͳ����**/
	char vPassword[16+1];/**������÷����루ԭ���Ǳ�����ֿ���Ϊ�գ�**/
	char vSysDate[20+1];/*�������ʱ�䣬ʱ���ʽ��2008-10-30 23:59:59*/
	char vLoginNo[6+1];/**�����ˣ�����Ա���ţ�û�о���ϵͳ���룩**/
	char vLoginName[20+1];/**�������������ɿգ�**/
	char vLoginPhone[15+1];/**��������ϵ�绰���ɿգ�**/
	char vLoginAccept[14+1];/**ҵ��������ˮ�Ż�Ͷ����ˮ�ŵȣ�14λ�ַ�����**/
	char vGroupId[10+1];/**�û��Ĺ���**/
	char vRegionCode[10+1];/**����(����)�û�����ID**/
	char vRegionName[100+1];/**����(����)�û���������**/
	char vDisCode[10+1];/**����(����)�û�����ID**/
	char vDisName[10+1];/**����(����)�û���������**/
	char vOrderSource[20+1];/**�������ֵ䣬CRM�ṩ�����¼��ֶΣ�ȡֵͶ�߶�Ӧ��ԴΪorder_source_csp��crm��Ӧֵorder_source_crm**/
		static char retMsgWide[8000+1];/**���ص���Ϣ**/
	char temMsg[1024+1];

	EXEC SQL END DECLARE SECTION;

	Sinitn(vSystemName);Sinitn(vSerCaller);Sinitn(vPassword);Sinitn(vSysDate);
	Sinitn(vLoginNo);Sinitn(vLoginName);Sinitn(vLoginPhone);Sinitn(vLoginAccept);
	Sinitn(vGroupId);Sinitn(vRegionCode);Sinitn(vRegionName);Sinitn(vDisCode);
	Sinitn(vDisName);Sinitn(vOrderSource);Sinitn(retMsgWide);

	sprintf(vSystemName,"JL_PBOSS");
	sprintf(vSerCaller,"JL_CRM");
	sprintf(vPassword,"");

	EXEC SQL SELECT TO_CHAR(SysDate,'YYYY-MM-DD HH24:MI:SS')
				INTO :vSysDate
				FROM DUAL;

	sprintf(vLoginNo,InLoginNo);

	EXEC SQL SELECT LOGIN_NAME,CONTRACT_PHONE
				INTO :vLoginName,:vLoginPhone
				FROM DLOGINMSG
				WHERE LOGIN_NO=:vLoginNo;
	if (SQLCODE != 0)
	{
		sprintf(retMsgWide,"ERROR");
		PUBLOG_DBERR("wideAreaPublicParams", "","��ȡ���ŵ��û�������ϵ��ʽʧ�ܣ�[%s]", retMsgWide);
		goto endRet;
	}

	sprintf(vLoginAccept,InLoginAccept);
	sprintf(vGroupId,InGroupId);

	EXEC SQL SELECT TO_CHAR(A.GROUP_ID),A.GROUP_NAME
				INTO :vRegionCode,:vRegionName
				FROM DCHNGROUPMSG A,DCHNGROUPINFO B
				WHERE A.GROUP_ID=B.PARENT_GROUP_ID
					AND A.ROOT_DISTANCE=1
					AND B.GROUP_ID=:vGroupId;
	if (SQLCODE != 0)
	{
		sprintf(retMsgWide,"ERROR");
		PUBLOG_DBERR("wideAreaPublicParams", "","��ȡ��֯�ṹregionCode��regionNameʧ�ܣ�[%s]", retMsgWide);
		goto endRet;
	}

	EXEC SQL SELECT TO_CHAR(A.GROUP_ID),A.GROUP_NAME
				INTO :vDisCode,:vDisName
				FROM DCHNGROUPMSG A,DCHNGROUPINFO B
				WHERE A.GROUP_ID=B.PARENT_GROUP_ID
				AND A.ROOT_DISTANCE=2
				AND B.GROUP_ID=:vGroupId;
	if (SQLCODE != 0)
	{
		sprintf(retMsgWide,"ERROR");
		PUBLOG_DBERR("wideAreaPublicParams", "","��ȡ��֯�ṹregionCode��regionNameʧ�ܣ�[%s]", retMsgWide);
		goto endRet;
	}

	sprintf(vOrderSource,"order_source_crm");
	Trim(vSystemName);
	Trim(vSerCaller);
	Trim(vPassword);
	Trim(vSysDate);
	Trim(vLoginNo);
	Trim(vLoginName);
	Trim(vLoginPhone);
	Trim(vLoginAccept);
	Trim(vRegionCode);
	Trim(vRegionName);
	Trim(vDisCode);
	Trim(vDisName);
	Trim(vOrderSource);

	#ifdef _DEBUG_
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vSystemName=[%s]",vSystemName);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vSerCaller=[%s]",vSerCaller);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vPassword=[%s]",vPassword);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vSysDate=[%s]",vSysDate);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vLoginNo=[%s]",vLoginNo);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vLoginName=[%s]",vLoginName);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vLoginPhone=[%s]",vLoginPhone);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vLoginAccept=[%s]",vLoginAccept);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vRegionCode=[%s]",vRegionCode);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vRegionName=[%s]",vRegionName);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vDisCode=[%s]",vDisCode);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vDisName=[%s]",vDisName);
		PUBLOG_DEBUG("wideAreaPublicParams", "", "vOrderSource=[%s]",vOrderSource);
	#endif

	/**ƴװ�������Ĳ���**/
	sprintf(retMsgWide,"systemName=%s,",vSystemName);
	sprintf(temMsg,"serCaller=%s,",vSerCaller); strcat(retMsgWide,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"callerPwd=%s,",vPassword); strcat(retMsgWide,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"callTime=%s,",vSysDate); strcat(retMsgWide,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"callUserId=%s,",vLoginNo); strcat(retMsgWide,temMsg); Sinitn(temMsg);
	sprintf(temMsg,"callUserName=%s,",vLoginName); strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"callUserPhone=%s,",vLoginPhone);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"recSeq=%s,",vLoginAccept);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"cityId=%s,",vRegionCode);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"cityName=%s,",vRegionName);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"countyId=%s,",vDisCode);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"countyName=%s,",vDisName);strcat(retMsgWide,temMsg);Sinitn(temMsg);
	sprintf(temMsg,"orderSource=%s",vOrderSource);strcat(retMsgWide,temMsg);

	#ifdef _DEBUG_
		PUBLOG_DEBUG("wideAreaNewWork", "", "PBOSS����ӿڻ�������=[%s]",retMsgWide);
	#endif

	endRet:

	return retMsgWide;
}

/*****************************************************
** ��������	CrmToPbossPubWebServ
** ��������	2010-11-2 11:09:43
** ���ù��̣�
** ������������PBOSS���ͱ��ġ�������DWIDEORDERDETAIL�������ݡ�

** ���������

		loginNo       ��������
		groupId       ���Ź���
		idNo          �û���ʶ
		phoneNo 	  ��������
		yearMonth 	  ��������
		regionCode 	  �û�����ID
		contractPhone ��ϵ�绰
		detailAddr 	  ��ַ����
		packageList   �ʷѱ���
		cfmPwd 		  �������(����)
		billMode      �ʷѴ���
		areaCode      ��ַ����
		areaName 	  ��ַ����
		custName      �ͻ�����
		cfmLogin      �����½�ʺ�
		appid         ����ƽ̨APPID
		transcode     ����ƽ̨TRANSCODE
		inMsg         ����
		retCode       ���ش���
		retMsg        ������Ϣ
		sendFlag      �Ƿ���������[0���������� 1��ԤԼ����]

** ���ز���:
		0    �ɹ�
		���� ʧ��

*******************************************************/

int CrmToPbossPubWebServ(char *loginNo,char *loginAccept,char *groupId,char *idNo,char *phoneNo,
	char *yearMonth,char *regionCode,char *contractPhone,char *detailAddr,char *packageList,
	char *cfmPwd,char *billMode,char *areaCode,char *areaName,char *custName,char *cfmLogin,
	int appid,char *transcode,char *inMsg,char *retCode,char *retMsg,char *sendFlag)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vLoginNo[6+1];
	char vLoginAccept[14+1];
	char vGroupId[10+1];
	int  vWret = -1;
	int vAppid=0;
	int vPriority=10;
	char vNewaddress[100];
	char vTranscode[100];
	char vInmsg[8000];
	char vOutmsg[ROWLEN];
	char vTmp_List[ROWLEN];
	char vTmpValue[ROWLEN];
	char vRetCodePboss[20+1];
	char vRetMsgPboss[100+1];
	char cNewValue[2500+1];
	char cOldValue[2500+1];
	char vYearMonth[6+1];
	char cOpNo[20+1];
	char vIdNo[20+1];
	char vPhoneNo[15+1];
	char vRegionCode[2+1];
	char vContractPhone[20+1];
	char vDetailAddr[100+1];
	char pPackage_List[ROWLEN];
	char vCfmPwd[30+1];
	char vBillMode[200+1];
	char vAreaCode[10+1];
	char vAreaName[100+1];
	char vCustName[60+1];
	char vCfmLogin[30+1];
	char vSendFlag[1+1];
	int begin=0,end=0,k=0,tmplen=0,num=0,j=0,i=0;
	EXEC SQL END DECLARE SECTION;

	Sinitn(vNewaddress);
	Sinitn(vTranscode);
	Sinitn(vInmsg);
	Sinitn(vOutmsg);
	Sinitn(vRetCodePboss);
	Sinitn(vRetMsgPboss);
	Sinitn(vLoginNo);
	Sinitn(vLoginAccept);
	Sinitn(vGroupId);
	Sinitn(vYearMonth);
	Sinitn(cOpNo);
	Sinitn(vIdNo);
	Sinitn(vPhoneNo);
	Sinitn(vRegionCode);
	Sinitn(vContractPhone);
	Sinitn(vDetailAddr);
	Sinitn(pPackage_List);
	Sinitn(vCfmPwd);
	Sinitn(vBillMode);
	Sinitn(vAreaCode);
	Sinitn(vAreaName);
	Sinitn(vCustName);
	Sinitn(vCfmLogin);
	Sinitn(vSendFlag);

	sprintf(vLoginNo,"%s",loginNo);
	sprintf(vLoginAccept,"%s",loginAccept);
	sprintf(vGroupId,"%s",groupId);
	vAppid=appid;
	sprintf(vTranscode,"%s",transcode);
	sprintf(vYearMonth,"%s",yearMonth);
	sprintf(vIdNo,"%s",idNo);
	sprintf(vPhoneNo,"%s",phoneNo);
	sprintf(vRegionCode,"%s",regionCode);
	sprintf(vContractPhone,"%s",contractPhone);
	sprintf(vDetailAddr,"%s",detailAddr);
	sprintf(cOpNo, "%s%s", vYearMonth, vLoginAccept);
	sprintf(pPackage_List, "%s", packageList);
	sprintf(vCfmPwd, "%s", cfmPwd);
	sprintf(vBillMode, "%s", billMode);
	sprintf(vAreaCode, "%s", areaCode);
	sprintf(vAreaName, "%s", areaName);
	sprintf(vCustName, "%s", custName);
	sprintf(vCfmLogin, "%s", cfmLogin);
	sprintf(vSendFlag,"%s",sendFlag);

	Trim(vLoginNo);
	Trim(vLoginAccept);
	Trim(vGroupId);
	Trim(vTranscode);
	Trim(vYearMonth);
	Trim(cOpNo);
	Trim(vPhoneNo);
	Trim(vRegionCode);
	Trim(vContractPhone);
	Trim(vDetailAddr);
	Trim(pPackage_List);
	Trim(vCfmPwd);
	Trim(vBillMode);
	Trim(vAreaCode);
	Trim(vAreaName);
	Trim(vCustName);
	Trim(vCfmLogin);

	sprintf(vNewaddress,"ADDRESS=%s",getenv("ADDRESS"));

	/**ƴװ������������**/
	strcpy(vInmsg,wideAreaPublicParams(vLoginNo,vLoginAccept,vGroupId));

	if(strcmp(vInmsg,"ERROR")==0)
	{
		strcpy(retCode, "000001");
		strcpy(retMsg, "CrmToPbossPubWebServ��������ƴװʧ��!");
		PUBLOG_APPERR("CrmToPbossPubWebServ",retCode, retMsg);
		return -1;
	 }

	strcat(vInmsg,",");
	strcat(vInmsg,inMsg);

	sprintf(cNewValue, "%s",vInmsg);
	sprintf(cOldValue, "%s",vInmsg);

	EXEC SQL INSERT INTO DWIDEORDERDETAIL
			(
				OP_NO, NEW_VALUE, OLD_VALUE,BILL_LIST
			)
			values
			(
				:cOpNo, :cNewValue, :cOldValue,:pPackage_List
			);
	if(SQLCODE != OK)
	{
		strcpy(retCode, "000004");
		strcpy(retMsg, "������������ϸ�����!");
		PUBLOG_DBERR("CrmToPbossPubWebServ", retCode, "������������ʷ����� [%s]!", retMsg);
		return -4;
	}

	if(strcmp(vSendFlag,"0")==0)
	{

	PUBLOG_APPERR("CrmToPbossPubWebServ","","vNewaddress=[%s]",vNewaddress);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vAppid=[%d]",vAppid);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vPriority=[%d]",vPriority);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vTranscode=[%s]",vTranscode);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vInmsg=[%s]",vInmsg);

	/**��ȡӦ��ƽ̨�ӿ�**/

	vWret = LG_Getinter(vNewaddress,vAppid,vPriority,vTranscode,vInmsg,vOutmsg);
	if( vWret != 0 )
	{
		strcpy(retCode, "000002");
		strcpy(retMsg, "����LG_Getinter�ӿ�ʧ��!");
		PUBLOG_APPERR("CrmToPbossPubWebServ",retCode, "LG_Getinter send fail vWret=[%s][%d]",retMsg,vWret);
		return -2;
	}

	/**������ΪPBOSS�ӿڷ��صĲ���**/
	strcat(vOutmsg,",");
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vOutmsg=[%s]",vOutmsg);

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
				if(k==1) strcpy(vRetCodePboss,Ssubstr(vTmp_List,begin,num,vTmpValue));
				if(k==2) strcpy(vRetMsgPboss,Ssubstr(vTmp_List,begin,num,vTmpValue));
				begin = end +2;
			}
		}
	}

	if(strcmp(vRetCodePboss,"returnCode=0")!=0)
	{
		strcpy(retCode, "000003");
		sprintf(retMsg,"���͹���ʧ��[%s]",vRetMsgPboss);
		PUBLOG_APPERR("CrmToPbossPubWebServ",retCode,retMsg);
		return -3;
	}
	#ifdef _DEBUG_
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "��Ϊ�ӿڷ��ز���  =[%s]",vOutmsg);
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "retCodePboss  =[%s]",vRetCodePboss);
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "retMsgPboss  =[%s]",vRetMsgPboss);
	#endif
	}
	else
	{
	#ifdef _DEBUG_
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "vSendFlag  =[%s]",vSendFlag);
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "retMsgPboss  =[%s]",vRetMsgPboss);
	#endif
	}
		return 0;
}
