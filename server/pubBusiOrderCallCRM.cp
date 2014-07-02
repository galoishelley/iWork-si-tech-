#include "boss_srv.h"
#include "publicsrv.h"

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/********************************************************************
 * ��������:SetOrderBusiSend
 * ��������:����ҵ�񹤵�
 * �������:
	char 	vCallServiceName[30+1]		��������
	int 	vSendParaNum				�����������
	int 	vRecpParaNum				�����������
	char	vParamList[MAXINPARALINE][2][DLINTERFACEDATA]	�����������ά�ַ������飬ʹ�÷�ʽΪ
											strcpy(vParamList[1][0],"phone_no"); 
											strcpy(vParamList[1][1],"13500800889");
											strcpy(vParamList[2][0],"run_code"); 
											strcpy(vParamList[2][1],"B");        
	char 	vIdType[1+1]				ID����
	char	vIdNo						IDֵ
	int		iOrderRight					����Ȩ�أ�һ�㶼����100
	char 	vOpAccept[22+1]				������ˮ
	char	vOpCode[5+1]				��������
	char 	vOpLogin[6+1]				��������
	char	vOpNote[100+1]				������ע
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

	/* ����pubBusiOrderExecCrm���������� 
	v_ret = pubBusiOrderExecCrm(&vMsgBodyType);
	if (v_ret != 0){
		printf("pubBusiOrderExecCrm: v_ret=[%d]\n", v_ret);
		return -500003;
	}
	*/
	OrderDestroyMsgBody(&vMsgBodyType);
	
	return 0;
}

