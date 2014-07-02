#include "boss_srv.h"
#include "publicsrv.h"

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

typedef struct StructOrderEMTable
{
	long		order_accept;
	char		order_type[DLCOMMTYPE];
	char		id_type[DLCOMMTYPE];
	char		id_no[31];
	char		interface_type[3];
	char		busi_type[21];
	char		busi_code[31];
	int			partition_column;
	int			order_right;
	char		op_type[DLCOMMTYPE];
	char		op_code[7];
	long		op_accept;
	char		op_login[7];
	int			total_date;
	char		op_time[DLDATETIME];
	char		op_note[101];
	char		data_format[DLCOMMTYPE];
	char		det_flag[DLCOMMTYPE];
	char		interface_data[DLINTERFACEDATA];
	char		commuinfo_flag[DLCOMMTYPE];
	int			send_num;
	char		send_status[DLCOMMTYPE];
	char		send_time[DLDATETIME];
	char		resp_code[33];
	char		resp_time[DLDATETIME];
	char		labeldbflag[21];
} OrderEMTable;
/*插入工单总表*/
int OrderInserTableEM(v_order_data,v_order_string)
OrderEMTable *v_order_data;
char *v_order_string;
{
	EXEC SQL BEGIN DECLARE SECTION;
		long v_order_accept;
		int i;
		OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
		
		char v_tmp_buf[DLDBSQL];
		char v_data_type[DLCOMMTYPE];
		char v_data_msg[DLINTERFACEDATA];
		
		char v_order_type[DLCOMMTYPE];
		char v_id_type[DLCOMMTYPE];
		char v_id_no[31];
		char v_interface_type[3];
		char v_busi_type[21];
		char v_busi_code[31];
		int	 v_order_right;
		char v_op_type[DLCOMMTYPE];
		char v_op_code[7];
		long v_op_accept;
		char v_op_login[7];
		char v_op_note[101];
		char v_data_format[DLCOMMTYPE];
		char v_det_flag[DLCOMMTYPE];
		char v_interface_data[DLINTERFACEDATA];
		char v_commuinfo_flag[DLCOMMTYPE];
		char v_labeldbflag[21];
    EXEC SQL END DECLARE SECTION;
    char v_xml_array[DLINTERFACEARRAY][DLINTERFACEDATA];
	int v_array_num;

	init(v_xml_array);
	v_array_num=OrderSpilitArray(v_order_string,v_xml_array);
	
	if (v_array_num==1){
		strcpy(v_order_data->det_flag,ORDERDETFLAG_N);
		strcpy(v_order_data->interface_data,v_xml_array[0]);
	}
	else{
		strcpy(v_order_data->det_flag,ORDERDETFLAG_Y);
		for (i=0;i<v_array_num;i++){
			strcpy(v_orderdet_data[i].data_type,"0");	
			strcpy(v_orderdet_data[i].data_msg,v_xml_array[i]);
		}
	}	
	
	init(v_tmp_buf);
	strcpy(v_tmp_buf,"SELECT seq_ordersend_accept.nextval FROM dual");
	EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :v_tmp_buf INTO :v_order_accept;
		END;
	END-EXEC;
	if (SQLCODE!=SQLOK)
	{
		printf("--------------------------------SQLCODE =[%d]---------\n",SQLCODE);
		return -1;	
	}

	strcpy(v_order_type    ,v_order_data->order_type    );
	strcpy(v_id_type       ,v_order_data->id_type       );
	strcpy(v_id_no         ,v_order_data->id_no         );
	strcpy(v_interface_type,v_order_data->interface_type);
	strcpy(v_busi_type     ,v_order_data->busi_type     );
	strcpy(v_busi_code     ,v_order_data->busi_code     );
	v_order_right=          v_order_data->order_right    ;
	strcpy(v_op_type       ,v_order_data->op_type       );
	strcpy(v_op_code       ,v_order_data->op_code       );
	v_op_accept=            v_order_data->op_accept      ;
	strcpy(v_op_login      ,v_order_data->op_login      );
	strcpy(v_op_note       ,v_order_data->op_note       );
	strcpy(v_data_format   ,v_order_data->data_format   );
	strcpy(v_det_flag      ,v_order_data->det_flag      );
	strcpy(v_interface_data,v_order_data->interface_data);
	strcpy(v_commuinfo_flag,v_order_data->commuinfo_flag);
	strcpy(v_labeldbflag   ,v_order_data->labeldbflag	);
			
	init(v_tmp_buf);
	strcpy(v_tmp_buf,"INSERT INTO WORDERRECEIPTEM(order_accept,order_type,id_type,id_no,interface_type,\
					busi_type,busi_code,partition_column,order_right,op_type,op_code,\
					op_accept,op_login,total_date,op_time,op_note,\
					data_format,det_flag,interface_data,commuinfo_flag,send_num,\
					send_status,send_time,resp_code,resp_time,labeldbflag) \
			VALUES(:v1,:v2,:v3,trim(:v4),:v5,\
					:v6,:v7,mod(to_number(to_char(sysdate,'YYYYMMDD')),10),:v8,:v9,:v10,\
					:v11,:v12,to_number(to_char(sysdate,'YYYYMMDD')),sysdate,:v13,\
					:v14,:v15,:v16,:v17,0,'0',null,null,null,:v18)");

	EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :v_tmp_buf USING :v_order_accept,:v_order_type,:v_id_type,:v_id_no,:v_interface_type,
				:v_busi_type,:v_busi_code,:v_order_right,:v_op_type,:v_op_code,
				:v_op_accept,:v_op_login,:v_op_note,
				:v_data_format,:v_det_flag,:v_interface_data,:v_commuinfo_flag,:v_labeldbflag;
		END;
	END-EXEC;				
	if (SQLCODE!=SQLOK){
		printf("+++++++++++++++++++SQLCODE=[%d]+++++++++++++++++++",SQLCODE);
		return -2;
	}

	if (v_array_num>1){
		for (i=0;i<v_array_num;i++)
		{
			strcpy(v_data_type,v_orderdet_data[i].data_type);
			strcpy(v_data_msg,v_orderdet_data[i].data_msg);
			
			init(v_tmp_buf);
			strcpy(v_tmp_buf,"INSERT INTO WORDERRECEIPTEMDataDet(order_accept,data_type,order_no,data_msg) \
					VALUES(:v1,:v2,:v3,:v4)");
			EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :v_tmp_buf USING :v_order_accept,:v_data_type,:i,:v_data_msg;
				END;
			END-EXEC;
			if (SQLCODE!=SQLOK)
				return -3;			
		}
	}

	return 0;
}

/********************************************************************
 * 函数名称:SetOrderBusiSendEM
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

int SetOrderBusiSendEM(char *psDBUse,char *vCallServiceName, int vSendParaNum, int vRecpParaNum,
		char vParamList[][2000], int	vCountList,
		char *vIdType, char *vIdNo,int iOrderRight,
		char *vOpAccept, char *vOpCode, char *vOpLogin,char *vOpNote)
{
	int i = 0,j = 0, v_ret = 0;
	int vArrayNum = 0;
	MsgBodyType vMsgBodyType;

	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderEMTable vOrderData;
	int vDetNum;
	char retMsg[60+1];
	init(retMsg);
	init(vXmlString);
	
	memset(&vOrderData, 0, sizeof(OrderEMTable));

	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEBUSI);

	OrderSetServiceInfo(&vMsgBodyType,vCallServiceName,vSendParaNum,vRecpParaNum);
	
	i = 0;j = 0;
	while(i < vCountList)
	{
		OrderSetParameter(&vMsgBodyType, vParamList[i], vParamList[i+1]);
		i = i + 2;
	}

	if(OrderGenXml(&vMsgBodyType,vXmlString)){
		printf("+++++++++++error occurs during OrderGenXml+++++++++++++\n");
		OrderDestroyMsgBody(&vMsgBodyType);
		return -500001;
	}
		
	/*OrderInitStructOrder(&vOrderData);*/
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
	strcpy(vOrderData.labeldbflag, psDBUse);
	v_ret = OrderInserTableEM(&vOrderData, vXmlString);
	if (v_ret != 0){
		printf("call OrderInserTableEM error ,ret=%d\n",v_ret);
		printf("+++++++++++error occurs during OrderInserSendCrm+++++++++++++,ret=%d\n",v_ret);
		OrderDestroyMsgBody(&vMsgBodyType);
		return -500002;
	}

	OrderDestroyMsgBody(&vMsgBodyType);
	
	return 0;
}
