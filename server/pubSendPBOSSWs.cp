#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"
#include <sys/timeb.h>
#include <time.h>

#define ROWLEN 2048

EXEC SQL INCLUDE SQLCA;

/******************************************************************
** 服务名称	wideAreaNewWork(派单报文拼装)
** 编码日期	2010-11-2 11:09:43
** 调用过程：
** 功能描述：拼装派单公共参数报文，宽带工单派单基础参数报文。
** 输入参数：

** 		businessType 业务类型
** 		busiName     业务名称
** 		specType     专业类型
** 		orderId      CRM侧订单号
** 		userId       CRM侧的用户产品唯一标识
** 		productId    产品编码[O]
** 		productName  产品名称
** 		serviceNum   宽带账号
** 		serviceKey   宽带密码
** 		customNo     客户编码[O]
** 		customName   客户姓名[O]
** 		linkman      联系人
** 		contactPhone 联系人电话
** 		addrId       地址编号
** 		addrName     地址名称
** 		userGroupid  用户归属groupId
** 		appointDate  预约时间
** 		description  备注[O]
** 		login_No     操作工号

** 返回参数：(示例报文)
** 		businessType=002,BusiName=宽带业务开通,specType=006,orderId=2010111012931343,
** 		userId=10200043107355,productId=006,productName=个人宽带,serviceNum=cc10010132546,
** 		serviceKey=BGCEQz,customNo=1032307886,customName=宽带测试,linkman=宽带测试,
** 		contactPhone=13656780987,addrId=103425,addrName=吉林长春南关区自由大路2671号幸福小区1栋2单元2层202号,
** 		districtId=85,districtName=汇文区域,appointDate=,description=操作员aa021A对用户10010132546进行宽带开户操作,
** 		siteName=自建自营长春市汇文营业厅
*****************************************************************/

char *wideAreaNewWork(char *businessType,char *busiName,char *specType,char *orderId,char *userId,char *productId,
					  char *productName,char *serviceNum,char *serviceKey,char *customNo,
					  char *customName,char *linkman,char *contactPhone,char *addrId,
					  char *addrName,char *userGroupid,char *appointDate,char *description,char *login_No)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vBusinessType[5+1];/**业务类型**/
	char vBusiName[32+1];/**业务名称**/
	char vSpecType[5+1];/**专业类型**/
	char vOrderId[20+1];/**CRM侧订单号**/
	char vUserId[14+1];/**CRM侧的用户产品实例标识，作为用户占用资源的唯一标识。**/
	char vProductId[2+1];/**产品编码，需要CRM提供编码列表。2位**/
	char vProductName[32+1];/**产品名称**/
	char vServiceNum[20+1];/**宽带账号**/
	char vServicePass[32+1];/**宽带密码**/
	char vCustomNo[20+1];/**客户编码**/
	char vCustomName[300+1];/**客户姓名**/
	char vLinkman[300+1];/**联系人**/
	char vContactPhone[300+1];/**联系电话**/
	char vAddrId[20+1];/**地址编号**/
	char vAddName[100+1];/**客户地址**/
	char vDistrictId[30+1];/**小区编码**/
	char vDistrictName[300+1];/**小区名称**/
	char vAppointDate[32+1];/**预约施工时间格式：YYYY-MM-DD HH24:MM:SS，可以为空**/
	char vDescription[500+1];/**备注**/
	char vSiteName[300+1];/**营业厅名称**/
	static char retMsg[8000+1];/**返回的信息**/
	char temMsg[1024+1];
	char vUserGroupId[10+1];/**用户归属groupId**/
	char vLogin_No[6+1];/**工号**/
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
		PUBLOG_DEBUG("wideAreaNewWork", "", "PBOSS订单派发参数报文=[%s]",retMsg);
	#endif

	return retMsg;
}

/*****************************************************
** 服务名称	wideAreaPublicParams(宽带公共基础参数报文)
** 编码日期	2010-11-2 11:09:43
** 调用过程：
** 功能描述：宽带基础公共报文。

** 输入参数：

**   InLoginNo 操作工号
**   InLoginAccept 操作流水
**   InGroupId 工号归属

** 返回参数:(示例报文)

**	 systemName=JL_PBOSS,serCaller=JL_CRM,callerPwd=,callTime=2010-11-08 11:01:37,
**	 callUserId=aa021A,callUserName=长春业务测试,callUserPhone=15164387636,recSeq=1012931343,
**	 cityId=3,cityName=长春地区,countyId=14,countyName=长春市,orderSource=order_source_crm

*******************************************************/

char *wideAreaPublicParams(char *InLoginNo,char *InLoginAccept,char *InGroupId)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char vSystemName[10+1];/**服务提供方，格式为：省份编码_系统名称**/
	char vSerCaller[10+1];/**服务调用方，格式为：省份编码_系统名称**/
	char vPassword[16+1];/**服务调用方密码（原来是必填，吉林可以为空）**/
	char vSysDate[20+1];/*服务调用时间，时间格式：2008-10-30 23:59:59*/
	char vLoginNo[6+1];/**操作人（操作员工号，没有就是系统代码）**/
	char vLoginName[20+1];/**操作人姓名（可空）**/
	char vLoginPhone[15+1];/**操作人联系电话（可空）**/
	char vLoginAccept[14+1];/**业务受理流水号或投诉流水号等（14位字符串）**/
	char vGroupId[10+1];/**用户的归属**/
	char vRegionCode[10+1];/**集团(个人)用户地市ID**/
	char vRegionName[100+1];/**集团(个人)用户地市名称**/
	char vDisCode[10+1];/**集团(个人)用户区县ID**/
	char vDisName[10+1];/**集团(个人)用户区县名称**/
	char vOrderSource[20+1];/**渠道，字典，CRM提供，（新加字段）取值投诉对应来源为order_source_csp，crm对应值order_source_crm**/
		static char retMsgWide[8000+1];/**返回的信息**/
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
		PUBLOG_DBERR("wideAreaPublicParams", "","获取工号的用户名、联系方式失败！[%s]", retMsgWide);
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
		PUBLOG_DBERR("wideAreaPublicParams", "","获取组织结构regionCode、regionName失败！[%s]", retMsgWide);
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
		PUBLOG_DBERR("wideAreaPublicParams", "","获取组织结构regionCode、regionName失败！[%s]", retMsgWide);
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

	/**拼装公共报文参数**/
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
		PUBLOG_DEBUG("wideAreaNewWork", "", "PBOSS宽带接口基本参数=[%s]",retMsgWide);
	#endif

	endRet:

	return retMsgWide;
}

/*****************************************************
** 服务名称	CrmToPbossPubWebServ
** 编码日期	2010-11-2 11:09:43
** 调用过程：
** 功能描述：向PBOSS发送报文、并存入DWIDEORDERDETAIL工单数据。

** 输入参数：

		loginNo       操作工号
		groupId       工号归属
		idNo          用户标识
		phoneNo 	  入网号码
		yearMonth 	  操作年月
		regionCode 	  用户地区ID
		contractPhone 联系电话
		detailAddr 	  地址名称
		packageList   资费编码
		cfmPwd 		  宽带密码(明文)
		billMode      资费代码
		areaCode      地址编码
		areaName 	  地址名称
		custName      客户名称
		cfmLogin      宽带登陆帐号
		appid         集成平台APPID
		transcode     集成平台TRANSCODE
		inMsg         报文
		retCode       返回代码
		retMsg        返回信息
		sendFlag      是否立即发送[0、立即发送 1、预约发送]

** 返回参数:
		0    成功
		其他 失败

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

	/**拼装基础参数报文**/
	strcpy(vInmsg,wideAreaPublicParams(vLoginNo,vLoginAccept,vGroupId));

	if(strcmp(vInmsg,"ERROR")==0)
	{
		strcpy(retCode, "000001");
		strcpy(retMsg, "CrmToPbossPubWebServ基本报文拼装失败!");
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
		strcpy(retMsg, "插入宽带订单明细表出错!");
		PUBLOG_DBERR("CrmToPbossPubWebServ", retCode, "插入宽带个性历史表出错 [%s]!", retMsg);
		return -4;
	}

	if(strcmp(vSendFlag,"0")==0)
	{

	PUBLOG_APPERR("CrmToPbossPubWebServ","","vNewaddress=[%s]",vNewaddress);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vAppid=[%d]",vAppid);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vPriority=[%d]",vPriority);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vTranscode=[%s]",vTranscode);
	PUBLOG_APPERR("CrmToPbossPubWebServ","","vInmsg=[%s]",vInmsg);

	/**调取应用平台接口**/

	vWret = LG_Getinter(vNewaddress,vAppid,vPriority,vTranscode,vInmsg,vOutmsg);
	if( vWret != 0 )
	{
		strcpy(retCode, "000002");
		strcpy(retMsg, "调用LG_Getinter接口失败!");
		PUBLOG_APPERR("CrmToPbossPubWebServ",retCode, "LG_Getinter send fail vWret=[%s][%d]",retMsg,vWret);
		return -2;
	}

	/**解析华为PBOSS接口返回的参数**/
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
		sprintf(retMsg,"发送工单失败[%s]",vRetMsgPboss);
		PUBLOG_APPERR("CrmToPbossPubWebServ",retCode,retMsg);
		return -3;
	}
	#ifdef _DEBUG_
		PUBLOG_DEBUG( "CrmToPbossPubWebServ", "", "华为接口返回参数  =[%s]",vOutmsg);
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
