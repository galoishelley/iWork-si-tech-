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

int SetOrderBusiSendBoss(char *vCallServiceName, int vSendParaNum, int vRecpParaNum,
		char vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA],
		char *vIdType, char *vIdNo,int iOrderRight,
		char *vOpAccept, char *vOpCode, char *vOpLogin,char *vOpNote)
{
	int i, v_ret = 0;
	int vArrayNum = 0;
	MsgBodyType vMsgBodyType;

	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable vOrderData;
	OrderDetTable vOrderDetData[DLINTERFACEARRAY];
	int vDetNum;
	char retMsg[60+1];
	init(retMsg);
	init(vXmlString);
	
	memset(&vOrderData, 0, sizeof(OrderTable));
	memset(&vOrderDetData, 0, sizeof(OrderDetTable));

	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEBUSI);

	OrderSetServiceInfo(&vMsgBodyType,vCallServiceName,vSendParaNum,vRecpParaNum);

	for(i = 0; i < vSendParaNum; i ++){
		OrderSetParameter(&vMsgBodyType, vParamList[i][0], vParamList[i][1]);
	}

	if(OrderGenXml(&vMsgBodyType,vXmlString)){
		return -500001;
	}
	
	
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
	v_ret = OrderInserSendBOSS(&vOrderData, vXmlString);
	if (v_ret != 0){
		printf("call OrderInserSendCrm error ,ret=%d\n",v_ret);
		return -500002;
	}

	/* 调用pubBusiOrderExecBoss处理工单数据 */
	v_ret = pubBusiOrderExecBoss(&vMsgBodyType);
	if (v_ret != 0){
		printf("pubBusiOrderExecBoss: v_ret=[%d]\n", v_ret);
		return -500003;
	}
	
	OrderDestroyMsgBody(&vMsgBodyType);
	
	return 0;
}

int pubBusiOrderExecBoss(MsgBodyType *vMsgBodyType){
	char	v_tmp_code[1000+1];
	char	v_ret_code[6+1];
	char	v_ret_msg[100+1];
	ParameterListType	*pParameterListType;
	
	char v_service_name[DLSERVICENAME];
    char v_sendpara_num[10];
    char v_recppara_num[10];
	int i=0,j=0,v_ret=0;
	
	char in_para[MAX_PARMS_NUM][200];
	init(v_service_name);	
	init(v_sendpara_num);	
	init(v_recppara_num);
	init(v_ret_code);
	init(v_ret_msg);
	
	memset(in_para,0,200*MAX_PARMS_NUM);
	
	strcpy(v_service_name,vMsgBodyType->BusiOrder->CallServiceName);	
	sprintf(v_sendpara_num,"%d",vMsgBodyType->BusiOrder->SendParaNum);
	sprintf(v_recppara_num,"%d",vMsgBodyType->BusiOrder->RecpParaNum);
	pParameterListType=vMsgBodyType->BusiOrder->ParameterList;
	while(pParameterListType!=NULL){
		strcpy(in_para[j],pParameterListType->ParameterItem->ParameterValue);
		pParameterListType=pParameterListType->next;
		j++;
	}

	if (j!=vMsgBodyType->BusiOrder->SendParaNum){
		printf("pubBusiOrderExecBoss Input Parameter Num Error!\n");
		return -500009;
	}
/***
	**if(0==strcmp(v_service_name,"bodc_ServiceOpen")){
	**	v_ret=bodc_ServiceOpen(in_para,v_ret_code, v_ret_msg);
	**	if(v_ret!=0){
	**		printf("Call bodc_ServiceOpen error![%d][%s]\n",v_ret,v_ret_msg);
	**		return -500010;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_pubBillEndMode")){
	**	v_ret=bodc_pubBillEndMode(in_para,v_ret_code, v_ret_msg);
	**	if(v_ret!=0){
	**		printf("Call bodc_pubBillEndMode error![%d][%s]\n",v_ret,v_ret_msg);
	**		return -500011;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_pubBillBeginTimeMode")){
	**v_ret=bodc_pubBillBeginTimeMode(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_pubBillBeginTimeMode error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500012;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_pubBillBeginMode")){
	**	v_ret=bodc_pubBillBeginMode(in_para,v_ret_code, v_ret_msg);
	**	if(v_ret!=0){
	**		printf("Call bodc_pubBillBeginMode error![%d][%s]\n",v_ret,v_ret_msg);
	**		return -500013;
	**	}
	**}
	**
	**if(0==strcmp(v_service_name,"bodc_AddCustAssuer")){
	**	v_ret=bodc_AddCustAssuer(in_para,v_ret_code, v_ret_msg);
	**	if(v_ret!=0){
	**		printf("Call bodc_AddCustAssuer error![%d][%s]\n",v_ret,v_ret_msg);
	**		return -500014;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_MarkDeduct")){
	**v_ret=bodc_MarkDeduct(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_MarkDeduct error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500015;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_MarkAward")){
	**v_ret=bodc_MarkAward(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_MarkAward error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500016;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_dConMsgUpdate")){
	**v_ret=bodc_dConMsgUpdate(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_dConMsgUpdate error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500017;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_SQSP_APPLY_RECUpdate")){
	**v_ret=bodc_SQSP_APPLY_RECUpdate(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_SQSP_APPLY_RECUpdate error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500018;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_wLocalBlackListDelete")){
	**v_ret=bodc_wLocalBlackListDelete(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_wLocalBlackListDelete error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500019;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_wLocalBlackListInsert")){
	**v_ret=bodc_wLocalBlackListInsert(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_wLocalBlackListInsert error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500020;
	**	}
	**}
	**if(0==strcmp(v_service_name,"bodc_dGrpPostOperation")){
	**v_ret=bodc_dGrpPostOperation(in_para,v_ret_code, v_ret_msg);
	**if(v_ret!=0){
	**	printf("Call bodc_dGrpPostOperation error![%d][%s]\n",v_ret,v_ret_msg);
	**	return -500021;
	**	}
	**}
	***/
	return 0;
}
