#include "boss_srv.h"
#include "publicsrv.h"

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/********************************************************************
 * 函数名称:SetOrderBusiSend
 * 功能描述:发送业务工单
 * 输入参数:
	char 	vCallServiceName[30+1]		服务名称
	int 	vSendParaNum				输入参数个数
	int 	vRecpParaNum				输出参数个数
	char	vParamList[MAXINPARALINE][2][DLINTERFACEDATA]	输入参数，二维字符串数组，使用方式为
											strcpy(vParamList[1][0],"phone_no"); 
											strcpy(vParamList[1][1],"13500800889");
											strcpy(vParamList[2][0],"run_code"); 
											strcpy(vParamList[2][1],"B");        
	char 	vIdType[1+1]				ID类型
	char	vIdNo						ID值
	int		iOrderRight					工单权重，一般都输入100
	char 	vOpAccept[22+1]				操作流水
	char	vOpCode[5+1]				操作代码
	char 	vOpLogin[6+1]				操作工号
	char	vOpNote[100+1]				操作备注
**********************************************************************/

int SetOrderBusiSendCrm(char *vCallServiceName, int vSendParaNum, int vRecpParaNum,
		char vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA],
		char *vIdType, char *vIdNo,int iOrderRight,
		char *vOpAccept, char *vOpCode, char *vOpLogin,char *vOpNote)
{
	int i, v_ret = 0;
	int vArrayNum = 0;
	MsgBodyType vMsgBodyType;

	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable vOrderData;
	int vDetNum;
	char retMsg[60+1];
	init(retMsg);
	init(vXmlString);
	
	memset(&vOrderData, 0, sizeof(OrderTable));

	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEBUSI);

	OrderSetServiceInfo(&vMsgBodyType,vCallServiceName,vSendParaNum,vRecpParaNum);

	for(i = 0; i < vSendParaNum; i ++){
		OrderSetParameter(&vMsgBodyType, vParamList[i][0], vParamList[i][1]);
	}

	if(OrderGenXml(&vMsgBodyType,vXmlString)){
		printf("+++++++++++error occurs during OrderGenXml+++++++++++++\n");
		OrderDestroyMsgBody(&vMsgBodyType);
		return -500001;
	}
	
	printf("vIdNo=[%s]\n",vIdNo);
	OrderInitStructOrder(&vOrderData);
	strcpy(vOrderData.order_type,  ORDERTYPEBUSI);
	strcpy(vOrderData.id_type,     vIdType);
	strcpy(vOrderData.id_no,       vIdNo);
	strcpy(vOrderData.busi_code,   vCallServiceName);
	vOrderData.order_right =       iOrderRight;
	strcpy(vOrderData.op_type,     ORDEROPTYPE_OTHER);
	strcpy(vOrderData.op_code,     vOpCode);
	vOrderData.op_accept =         atol(vOpAccept);
	strcpy(vOrderData.op_login,    vOpLogin);
	strcpy(vOrderData.op_note,     vOpNote);
	strcpy(vOrderData.data_format, ORDERDATAFORMAT_XML);
	v_ret = OrderInserSendCrm(&vOrderData, vXmlString);
	if (v_ret != 0){
		printf("+++++++++++error occurs during OrderInserSendCrm+++++++++++++,ret=%d\n",v_ret);
		OrderDestroyMsgBody(&vMsgBodyType);
		return -500002;
	}

	/* 调用pubBusiOrderExecCrm处理工单数据 
	v_ret = pubBusiOrderExecCrm(&vMsgBodyType);
	if (v_ret != 0){
		printf("pubBusiOrderExecCrm: v_ret=[%d]\n", v_ret);
		return -500003;
	}
	*/
	OrderDestroyMsgBody(&vMsgBodyType);
	
	return 0;
}

