/*
**Created: Tophill(zhangkun)
**Date:    2009.03.19
*/

#include <atmi.h>
#include <userlog.h>
#include <fml.h>   
#include <Usysflds.h>
#include <fml32.h>   

#include "order_xml.h"
#include "general32.flds.h"

/*数据工单处理--头文件*/
#include "dataOrder.h"

#define PROG_NAME_LABEL	"ORDERDEALCRM"

extern char g_PUB_Source[DLDATASERVER];
extern PUB_PassType g_PUB_DbCfg;

int g_PUB_PartitionCfg;
int g_PUB_OtherRouteFlag;

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

int OrderUpdateCustMsg(
	i_sendid_type,i_sendid_no,i_order_right,
	i_op_code,i_op_accept,i_op_login,i_op_note,
	i_id_no,
	i_update_sql,i_where_sql,i_parameter_array
	)
char *i_sendid_type;
char *i_sendid_no;
int  i_order_right;

char *i_op_code;
long i_op_accept;
char *i_op_login;
char *i_op_note;

char *i_id_no;

char *i_update_sql;
char *i_where_sql;
char i_parameter_array[][DLMAXITEMS][DLINTERFACEDATA];
{
	int v_ret;
	MsgBodyType vMsgBodyType;
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable v_order_data;
	OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
	char v_table_name[DLTABLENAME];

	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buff[DLMAXITEMS*DLINTERFACEDATA];
		char v_id_no[14+1];
		char v_cust_id[10+1];
		char v_contract_no[14+1];
		char v_ids[4+1];
		char v_phone_no[15+1];
		char v_sm_code[2+1];
		char v_service_type[2+1];
		char v_attr_code[8+1];
		char v_user_passwd[8+1];
		char v_open_time[17+1];
		char v_belong_code[7+1];
		char v_limit_owe[15+1];
		char v_credit_code[1+1];
		char v_credit_value[8+1];
		char v_run_code[2+1];
		char v_run_time[17+1];
		char v_bill_date[17+1];
		char v_bill_type[4+1];
		char v_encrypt_prepay[16+1];
		char v_cmd_right[10+1];
		char v_last_bill_type[1+1];
		char v_bak_field[12+1];
		char v_group_id[20+1];
		char v_group_no[10+1];
		char v_group_msg[20+1];
	EXEC SQL END DECLARE SECTION;

	init(v_sql_buff);

	Coltrim(i_sendid_no);

	sprintf(v_sql_buff,"UPDATE dCustMsg SET %s WHERE id_no=to_number(:NG_Param0) %s ",i_update_sql,i_where_sql);

	v_ret=OrderMultiParaDML(v_sql_buff,&i_parameter_array[0]);
	
	if(v_ret<0){
		printf("OrderMultiParaDML,ret=%d\n",v_ret);
		return -500016;
	}
	
	/*没有更新到数据，可以不做后续处理*/
	if(v_ret>0)
	{
		return 1;
	}

	EXEC SQL SELECT id_no,cust_id,contract_no,ids,
					phone_no,sm_code,service_type,attr_code,
					user_passwd,open_time,belong_code,limit_owe,
					credit_code,credit_value,run_code,run_time,
					bill_date,bill_type,encrypt_prepay,cmd_right,
					last_bill_type,bak_field,nvl(group_id,chr(0)),nvl(group_no,chr(0)),nvl(group_msg,chr(0))
		INTO :v_id_no,:v_cust_id,:v_contract_no,:v_ids,
			 :v_phone_no,:v_sm_code,:v_service_type,:v_attr_code,
			 :v_user_passwd,:v_open_time,:v_belong_code,:v_limit_owe,
			 :v_credit_code,:v_credit_value,:v_run_code,:v_run_time,
			 :v_bill_date,:v_bill_type,:v_encrypt_prepay,:v_cmd_right,
			 :v_last_bill_type,:v_bak_field,:v_group_id,:v_group_no,:v_group_msg
		FROM dCustMsg
		WHERE id_no=:i_id_no;
	if(SQLCODE!=SQLOK)
		return -500017;

	Coltrim(v_id_no);
	Coltrim(v_cust_id);
	Coltrim(v_contract_no);
	Coltrim(v_ids);
	Coltrim(v_phone_no);
	Coltrim(v_sm_code);
	Coltrim(v_service_type);
	Coltrim(v_attr_code);
	Coltrim(v_user_passwd);
	Coltrim(v_open_time);
	Coltrim(v_belong_code);
	Coltrim(v_limit_owe);
	Coltrim(v_credit_code);
	Coltrim(v_credit_value);
	Coltrim(v_run_code);
	Coltrim(v_run_time);
	Coltrim(v_bill_date);
	Coltrim(v_bill_type);
	Coltrim(v_encrypt_prepay);
	Coltrim(v_cmd_right);
	Coltrim(v_last_bill_type);
	Coltrim(v_bak_field);
	Coltrim(v_group_id);
	Coltrim(v_group_no);
	Coltrim(v_group_msg);

	strcpy(v_table_name,"DCUSTMSG");
	/*组装报文开始*/
	init(vXmlString);

	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEDATA);

	OrderSetTableName(&vMsgBodyType,v_table_name);

	OrderSetPrimaryKey(&vMsgBodyType,"id_no",ORDERDATAFORMAT_NUMBER,i_id_no);

	OrderSetDataItem(&vMsgBodyType,"id_no",         ORDERDATAFORMAT_NUMBER, v_id_no);
	OrderSetDataItem(&vMsgBodyType,"cust_id",       ORDERDATAFORMAT_NUMBER, v_cust_id);
	OrderSetDataItem(&vMsgBodyType,"contract_no",   ORDERDATAFORMAT_NUMBER, v_contract_no);
	OrderSetDataItem(&vMsgBodyType,"ids",           ORDERDATAFORMAT_NUMBER, v_ids);
	OrderSetDataItem(&vMsgBodyType,"phone_no",      ORDERDATAFORMAT_STRING, v_phone_no);
	OrderSetDataItem(&vMsgBodyType,"sm_code",       ORDERDATAFORMAT_STRING, v_sm_code);
	OrderSetDataItem(&vMsgBodyType,"service_type",  ORDERDATAFORMAT_STRING, v_service_type);
	OrderSetDataItem(&vMsgBodyType,"attr_code",     ORDERDATAFORMAT_STRING, v_attr_code);
	OrderSetDataItem(&vMsgBodyType,"user_passwd",   ORDERDATAFORMAT_STRING, v_user_passwd);
	OrderSetDataItem(&vMsgBodyType,"open_time",     ORDERDATAFORMAT_DATE,   v_open_time);
	OrderSetDataItem(&vMsgBodyType,"belong_code",   ORDERDATAFORMAT_STRING, v_belong_code);
	OrderSetDataItem(&vMsgBodyType,"limit_owe",     ORDERDATAFORMAT_NUMBER, v_limit_owe);
	OrderSetDataItem(&vMsgBodyType,"credit_code",   ORDERDATAFORMAT_STRING, v_credit_code);
	OrderSetDataItem(&vMsgBodyType,"credit_value",  ORDERDATAFORMAT_NUMBER, v_credit_value);
	OrderSetDataItem(&vMsgBodyType,"run_code",      ORDERDATAFORMAT_STRING, v_run_code);
	OrderSetDataItem(&vMsgBodyType,"run_time",      ORDERDATAFORMAT_DATE,   v_run_time);
	OrderSetDataItem(&vMsgBodyType,"bill_date",     ORDERDATAFORMAT_DATE,   v_bill_date);
	OrderSetDataItem(&vMsgBodyType,"bill_type",     ORDERDATAFORMAT_NUMBER, v_bill_type);
	OrderSetDataItem(&vMsgBodyType,"encrypt_prepay",ORDERDATAFORMAT_STRING, v_encrypt_prepay);
	OrderSetDataItem(&vMsgBodyType,"cmd_right",     ORDERDATAFORMAT_NUMBER, v_cmd_right);
	OrderSetDataItem(&vMsgBodyType,"last_bill_type",ORDERDATAFORMAT_STRING, v_last_bill_type);
	OrderSetDataItem(&vMsgBodyType,"bak_field",     ORDERDATAFORMAT_STRING, v_bak_field);
	OrderSetDataItem(&vMsgBodyType,"group_id",      ORDERDATAFORMAT_STRING, v_group_id);
	OrderSetDataItem(&vMsgBodyType,"group_no",      ORDERDATAFORMAT_STRING, v_group_no);
	OrderSetDataItem(&vMsgBodyType,"group_msg",     ORDERDATAFORMAT_STRING, v_group_msg);

	v_ret=OrderGenXml(&vMsgBodyType,vXmlString);
	if(v_ret<0)
		return -500018;

	OrderDestroyMsgBody(&vMsgBodyType);

	/*组装报文结束*/
	OrderInitStructOrder(&v_order_data);	init(v_orderdet_data);

	strcpy(v_order_data.order_type,ORDERTYPEDATA);
	strcpy(v_order_data.id_type,i_sendid_type);
	strcpy(v_order_data.id_no,i_sendid_no);
	strcpy(v_order_data.busi_code,v_table_name);
	v_order_data.order_right=i_order_right;
	strcpy(v_order_data.op_type,ORDEROPTYPE_UPDATE);
	strcpy(v_order_data.op_code,i_op_code);
	v_order_data.op_accept=i_op_accept;
	strcpy(v_order_data.op_login,i_op_login);
	strcpy(v_order_data.op_note,i_op_note);
	strcpy(v_order_data.data_format,ORDERDATAFORMAT_XML);

	v_ret=OrderInserSendCrm(&v_order_data,vXmlString);

	if(v_ret!=0)
		return -500019;
	return 0;
}

int func_GoStop(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg){
    EXEC SQL BEGIN DECLARE SECTION;
	char in_id_no[14+1];
	char in_cust_id[10+1];
	char in_phone_no[15+1];
	char in_sm_code[2+1];	
	char in_attr_code[8+1];
	char in_belong_code[7+1];
	char in_group_id[10+1];
	char in_op_time[17+1];
	char in_login_no[6+1];
	char in_org_code[9+1];
	char in_org_id[10+1];
	char in_op_code[4+1];
	char in_login_accept[22+1];
	char in_run_code[2+1];
	char in_stop_flag[1+1];
	char in_tmp_run_code[2+1];
	char year_month[6+1];
	char TmpSqlStr[1536+1];
	char in_total_date[8+1];
	
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];		
	int  v_ret = 0;

    EXEC SQL END DECLARE SECTION;
	init(in_id_no);
	init(in_cust_id);
	init(in_phone_no);
	init(in_sm_code);
	init(in_attr_code);
	init(in_belong_code);
	init(in_run_code);
	init(in_group_id);
	init(in_op_time);
	init(in_login_no);
	init(in_org_code);
	init(in_org_id);
	init(in_op_code);
	init(in_login_accept);
	init(in_stop_flag);
	init(in_tmp_run_code);
	init(year_month);
	init(in_total_date);
	
	memset(v_parameter_array, 0 ,sizeof(v_parameter_array));
	
	strcpy(in_id_no			,in_para[0]	);
	strcpy(in_cust_id		,in_para[1]	);
	strcpy(in_phone_no		,in_para[2]	);
	strcpy(in_sm_code		,in_para[3]	);
	strcpy(in_attr_code		,in_para[4]	);
	strcpy(in_belong_code	,in_para[5]	);
	strcpy(in_tmp_run_code	,in_para[6]	);
	strcpy(in_group_id		,in_para[7]	);
	strcpy(in_op_time		,in_para[8]	);
	strcpy(in_login_no		,in_para[9]	);
	strcpy(in_org_code		,in_para[10]);
	strcpy(in_org_id		,in_para[11]);
	strcpy(in_op_code		,in_para[12]);
	strcpy(in_login_accept	,in_para[13]);
	strcpy(in_stop_flag		,in_para[14]);
	strcpy(in_run_code 		,in_para[15]);
	
	strim(in_id_no);
	strim(in_cust_id);
	strim(in_phone_no);
	strim(in_sm_code);
	strim(in_attr_code);
	strim(in_belong_code);
	strim(in_run_code);
	strim(in_group_id);
	strim(in_op_time);
	strim(in_login_no);
	strim(in_org_code);
	strim(in_org_id);
	strim(in_op_code);
	strim(in_login_accept);
	strim(in_stop_flag);
	strim(in_tmp_run_code);
	
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"操作成功！");
	
	strncpy(in_total_date,in_op_time,8);
	strncpy(year_month,in_op_time,6);
	strim(in_total_date);
	strim(year_month);
	
	if(strcmp(in_run_code,"99")!=0&&atoi(in_stop_flag)!=2&&(strncmp(in_op_code,"2314",4)!=0)&&atoi(in_stop_flag)!=3){
		
		memset(v_parameter_array, 0 ,sizeof(v_parameter_array));
			
		strcpy(v_parameter_array[0], in_tmp_run_code);
		strcpy(v_parameter_array[1], in_op_time);
		strcpy(v_parameter_array[2], in_id_no);
		strcpy(v_parameter_array[3], in_op_time);
					
		v_ret = 0;
		v_ret = OrderUpdateCustMsg(ORDERIDTYPE_USER, in_id_no, 100, in_op_code, atol(in_login_accept), 
				in_login_no, "func_GoStop", in_id_no, 
				"run_code=:v0, run_time=to_date(:v1,'YYYYMMDD HH24:MI:SS')",
				" and run_time <=to_date(:v2,'YYYYMMDD HH24:MI:SS')", v_parameter_array);
		if (v_ret<0){
			strcpy(v_ret_code,"200001");
			init(v_ret_msg);
			sprintf(v_ret_msg,"数据工单错误，更新 dCustMsg 信息错误[%d]",v_ret);
			return  -1;
		}
		
		/* 有更新到dCustMsg情况的时候才来记录小表 */
		if(v_ret==0) {
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"insert into wUserMsgChg(op_no,op_type,id_no,cust_id,\
				phone_no,sm_code,attr_code,belong_code,\
				run_code,run_time,op_time,group_id ) values(\
				sMaxBillChg.NEXTVAL,2,to_number(:v1),to_number(:v2),:v3,:v4,:v5,:v6,:v7,to_char(to_date(:v8, 'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'),\
				to_char(sysdate,'yyyymmddhh24miss'),:v9)");
			EXEC SQL prepare sql_tmt from :TmpSqlStr;
			EXEC SQL EXECUTE sql_tmt using :in_id_no,:in_cust_id,:in_phone_no,:in_sm_code,:in_attr_code,:in_belong_code,:in_tmp_run_code,:in_op_time,:in_group_id;
			if(SQLCODE!=0){
				strcpy(v_ret_code,"200002");
				init(v_ret_msg);
				sprintf(v_ret_msg,"插入 wUserMsgChg 信息错误[%d]",SQLCODE);
				return  -1;
			}
		}
	}
	
	if(atoi(in_stop_flag)!=0){
		if(!((in_run_code[1]=='B')||(in_run_code[1]=='C')||(in_run_code[1]=='8')||(in_run_code[0]=='8'&&in_run_code[1]=='A'))){
			if(strncmp(in_op_code,"2314",4)!=0){
				init(TmpSqlStr);
                sprintf(TmpSqlStr,"insert into wChg%s(id_no,total_date,login_accept,sm_code,belong_code,phone_no,org_code,login_no,\
                                op_code,op_time,machine_code,cash_pay,check_pay,sim_fee,machine_fee,innet_fee, \
                                choice_fee,other_fee,hand_fee,deposit,back_flag,encrypt_fee,system_note,op_note,group_id,org_id)\
                                values(to_number(:v1),:v2,to_number(:v3),:v4,:v5,:v6,:v7,:v8,\
                                :v9,to_date(:v10,'YYYYMMDD HH24:MI:SS'),'ZZ',0,0,0,0,0,0,0,0,0,'0','00000','时实停机','时实停机',:v11,:v12)",year_month);
				EXEC SQL prepare sql_tmt from :TmpSqlStr;
				EXEC SQL EXECUTE sql_tmt using :in_id_no,:in_total_date,:in_login_accept,:in_sm_code,:in_belong_code,:in_phone_no,:in_org_code,:in_login_no,:in_op_code,:in_op_time,:in_group_id,:in_org_id;
				if(SQLCODE!=0){
					strcpy(v_ret_code,"200003");
					init(v_ret_msg);
					sprintf(v_ret_msg,"插入 wChg 信息错误[%d]",SQLCODE);
					return  -1;
				}
			}
		}
	}

	return 0;
}

/*主处理函数处理指定数据库实例上的 0：手机号码；1：客户；2：用户；3：帐户； 以及所有的4：操作员；5：流水；9：空值
  副处理函数之处理指定数据库实例上的 0：手机号码；1：客户；2：用户；3：帐户；
返回参数: 0:能够路由;1:不能路由;-1:程序出错
  
  */
int JudgeRoute(i_prog_label,i_id_type,i_id_no)
char *i_prog_label;
char *i_id_type;
char *i_id_no;
{
	EXEC SQL BEGIN DECLARE SECTION;
		int v_route_code=0;
		int v_partition_code=0;
		char v_error_msg[100+1];
    EXEC SQL END DECLARE SECTION;	
	
	EXEC sql  select user into : v_error_msg from dual;
	
	DEBUG(i_prog_label,"v_error_msg[%s]\n",v_error_msg);  	
	
    v_route_code=atoi(i_id_type);

	if (v_route_code==1 || v_route_code==2 || v_route_code==3){
		EXEC SQL SELECT partition_code INTO :v_partition_code FROM sRouteData 
			WHERE route_code=:v_route_code AND :i_id_no>=key_value AND :i_id_no<key_value2;
		if (SQLCODE!=SQLOK){
			errLog(i_prog_label,"JudgeRoute Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
			return -1;	
		}
		
		if (g_PUB_PartitionCfg%v_partition_code==0)
			return 0;
		else
			return 1;
	}
	else if (v_route_code==4){
		EXEC SQL SELECT partition_code INTO :v_partition_code FROM sRouteData 
			WHERE route_code=:v_route_code AND substr(:i_id_no,1,1)=key_value;
		if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
			errLog(i_prog_label,"JudgeRoute Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
			return -1;	
		}
		if (SQLCODE==NOTFOUND)
			if (g_PUB_OtherRouteFlag==0)
				return 1;
			else
				return 0;
		
		if (g_PUB_PartitionCfg%v_partition_code==0)
			return 0;	
		else
			return 1;
	}
	else if (v_route_code==0){
		EXEC SQL SELECT partition_code INTO :v_partition_code FROM sRouteData 
			WHERE route_code=:v_route_code AND substr(:i_id_no,1,7)=key_value;
		if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
			errLog(i_prog_label,"JudgeRoute Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
			return -1;	
		}
		if (SQLCODE==NOTFOUND)
			if (g_PUB_OtherRouteFlag==0)
				return 1;
			else
				return 0;
		
		if (g_PUB_PartitionCfg%v_partition_code==0)
			return 0;	
		else
			return 1;
	}
	else if (g_PUB_OtherRouteFlag==0)/*如果其他路由标志为假则不路由*/
		return 1;

	return 0;
}

int OperOrderTable(i_prog_label,i_table_label,i_op_type,i_order_accept,i_det_flag,i_ret_code,i_ret_msg)
char *i_prog_label;
char *i_table_label;
char *i_op_type;
long i_order_accept;
char *i_det_flag;
char *i_ret_code;
char *i_ret_msg;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buf[DLDBSQL];
		char v_error_code[6+1];
		char v_error_msg[100+1];
    EXEC SQL END DECLARE SECTION;	

	if (strcmp(i_op_type,"0")==0){
		init(v_sql_buf);
		sprintf(v_sql_buf,"INSERT INTO wOrderReceiptHis \
				SELECT a.*,'1',sysdate \
					FROM wOrderReceipt%s a WHERE order_accept=:v1",i_table_label);
		EXEC SQL PREPARE sql_tmt FROM :v_sql_buf;
		EXEC SQL EXECUTE sql_tmt USING :i_order_accept;
		if (SQLCODE!=SQLOK && SQLCODE!=-1){
			errLog(i_prog_label,"INSERT INTO wOrderReceiptHis Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
		}
		if (SQLCODE==-1){/*重复工单异常*/
			EXEC SQL ROLLBACK;	
		}
		
		if (strcmp(i_det_flag,ORDERDETFLAG_Y)==0){
			EXEC SQL INSERT INTO wOrderReceiptDataDetHis
				SELECT order_accept,data_type,order_no,data_msg FROM wOrderReceiptDataDet
				WHERE order_accept=:i_order_accept;
			if (SQLCODE!=SQLOK){
				errLog(i_prog_label,"INSERT INTO wOrderReceiptDataDetHis Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
			}
		
			EXEC SQL DELETE wOrderReceiptDataDet WHERE order_accept=:i_order_accept;
			if (SQLCODE!=SQLOK){
				errLog(i_prog_label,"DELETE wOrderReceiptDataDetHis Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
			}				
		}

	}
	else{
		init(v_error_code);		init(v_error_msg);
		sprintf(v_error_code,"%d",SQLCODE);
		strcpy(v_error_msg,SQLERRMSG);
		
		init(v_sql_buf);
		sprintf(v_sql_buf,"INSERT INTO wOrderException \
				SELECT a.*,:v1,:v2,:v3,:v4 \
					FROM wOrderReceipt%s a WHERE order_accept=:v5",i_table_label);
		EXEC SQL PREPARE sql_tmt FROM :v_sql_buf;
		EXEC SQL EXECUTE sql_tmt USING :i_ret_code,:i_ret_msg,:v_error_code,:v_error_msg,:i_order_accept;
		if (SQLCODE!=SQLOK && SQLCODE!=-1){
			errLog(i_prog_label,"INSERT INTO wOrderException Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
			return -2;
		}

		if (SQLCODE==-1){/*重复工单异常*/
			errLog(i_prog_label,"INSERT INTO wOrderException Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
		}
	}
	
	init(v_sql_buf);
	sprintf(v_sql_buf,"DELETE wOrderReceipt%s WHERE order_accept=:v1",i_table_label);
	EXEC SQL PREPARE sql_tmt FROM :v_sql_buf;
	EXEC SQL EXECUTE sql_tmt USING :i_order_accept;	
	if (SQLCODE!=SQLOK)
	{
		errLog(i_prog_label,"DELETE wOrderReceipt Error,order_accept=[%ld],SQLCODE=[%d],SQLERRMSG=[%s]\n",i_order_accept,SQLCODE,SQLERRMSG);
		return -3;
	}

	return 0;
}

void PubLogicDeal(i_prog_label,i_table_label)
char *i_prog_label;
char *i_table_label;
{
	FBFR32		*trans_in = NULL;		/* 输入缓冲区           */
	FBFR32		*trans_out = NULL;		/* 输出缓冲区           */
	FLDLEN32	len=4096;				/* 输出缓存区分配的空间 */
	long	ilen,olen;
	char	v_tmp_code[1000+1];
	char	v_ret_code[6+1];
	char	v_ret_msg[100+1];
	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buf[DLDBSQL];
		int  v_rownum=PUB_BATCHORDER_NUM;
		int  v_orderdet_num=0;
		int  v_record_num=0;
		OrderTable v_order_data[PUB_BATCHORDER_NUM];
		OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
    EXEC SQL END DECLARE SECTION;	

	MsgBodyType	vMsgBodyType;
	ParameterListType	*pParameterListType,*vParameterListType;
    char vXmlData[DLINTERFACEARRAY*DLINTERFACEDATA];
    int i,j,v_ret;
    char v_service_name[DLSERVICENAME];
    char v_sendpara_num[10];
    char v_recppara_num[10];
    char in_para[DLMAXITEMS][DLINTERFACEDATA];

	init(v_sql_buf);	init(v_order_data);

	sprintf(v_sql_buf,"SELECT order_accept,order_type,id_type,nvl(id_no,' '),nvl(interface_type,' '),nvl(busi_code,' '),\
				nvl(busi_code,' '),partition_column,nvl(order_right,0),nvl(op_type,' '),nvl(op_code,' '),nvl(op_accept,0),\
				nvl(op_login,' '),total_date,nvl(to_char(op_time,'YYYYMMDD HH24:MI:SS'),' '),nvl(op_note,' '),\
				data_format,det_flag,nvl(interface_data,' '),\
				nvl(commuinfo_flag,' '),nvl(send_num,0),nvl(send_status,' '),\
				nvl(to_char(send_time,'YYYYMMDD HH24:MI:SS'),' '),nvl(resp_code,' '),\
				nvl(to_char(resp_time,'YYYYMMDD HH24:MI:SS'),' ')\
			FROM wOrderReceipt%s \
			WHERE order_type=any('1','2') ORDER BY order_accept",i_table_label);
	EXEC SQL PREPARE SelectStr FROM :v_sql_buf;
	EXEC SQL DECLARE SelectCur CURSOR FOR SelectStr;
	EXEC SQL OPEN SelectCur;
	EXEC SQL FOR :v_rownum FETCH SelectCur INTO :v_order_data;
	EXEC SQL CLOSE SelectCur;
	if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
		errLog(i_prog_label,"Select wOrderReceipt%s Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",i_table_label,SQLCODE,SQLERRMSG);
		return;
	}

	v_rownum=SQLNUM;

	if (v_rownum<=0)
		return;

	/*分配服务输入输出缓冲区开始*/
	trans_in=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(trans_in == (FBFR32 *)NULL){
		errLog(i_prog_label,"Process [%s] Alloc FMLTYPE32 Error [%s]\n",i_table_label,tpstrerror(tperrno));
		goto DealEnd;
	}
	
	trans_out=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(trans_out == (FBFR32 *)NULL){
		errLog(i_prog_label,"Process [%s] Alloc FMLTYPE32 Error [%s]\n",i_table_label,tpstrerror(tperrno));
		goto DealEnd;
	}
	/*分配服务输入输出缓冲区结束*/

	for (i=0;i<v_rownum;i++){
/*
		EXEC SQL SELECT count(*) INTO :v_record_num FROM wOrderReceiptHis WHERE order_accept=:v_order_data[i].order_accept;
		if (v_record_num>0){
			init(v_sql_buf);
			sprintf(v_sql_buf,"DELETE wOrderReceipt%s WHERE order_accept=:v1",i_table_label);
			EXEC SQL PREPARE sql_tmt FROM :v_sql_buf;
			EXEC SQL EXECUTE sql_tmt USING :v_order_data[i].order_accept;	
			if (SQLCODE!=SQLOK){
				EXEC SQL ROLLBACK;
				errLog(i_prog_label,"Process [%s] order_accept=[%ld] Delete wOrderReceipt%s SQLCODE=[%d]\n",
						i_table_label,v_order_data[i].order_accept,i_table_label,SQLCODE);	
			}			
			continue;
		}
		
		EXEC SQL SELECT count(*) INTO :v_record_num FROM wOrderException WHERE order_accept=:v_order_data[i].order_accept;
		if (v_record_num>0){
			init(v_sql_buf);
			sprintf(v_sql_buf,"DELETE wOrderReceipt%s WHERE order_accept=:v1",i_table_label);
			EXEC SQL PREPARE sql_tmt FROM :v_sql_buf;
			EXEC SQL EXECUTE sql_tmt USING :v_order_data[i].order_accept;	
			if (SQLCODE!=SQLOK){
				EXEC SQL ROLLBACK;
				errLog(i_prog_label,"Process [%s] order_accept=[%ld] Delete wOrderReceipt%s SQLCODE=[%d]\n",
						i_table_label,v_order_data[i].order_accept,i_table_label,SQLCODE);	
			}			
			continue;
		}
*/		

		strim(v_order_data[i].id_type);	strim(v_order_data[i].id_no);
		v_ret=JudgeRoute(i_prog_label,v_order_data[i].id_type,v_order_data[i].id_no);

		if (v_ret!=0)
			continue;

		strim(v_order_data[i].order_type);		strim(v_order_data[i].interface_type);
		strim(v_order_data[i].busi_code);		strim(v_order_data[i].op_type);
		strim(v_order_data[i].op_code);			strim(v_order_data[i].op_login);
		strim(v_order_data[i].op_time);			strim(v_order_data[i].op_note);	
		strim(v_order_data[i].data_format);		strim(v_order_data[i].det_flag);
		strim(v_order_data[i].interface_data);	strim(v_order_data[i].commuinfo_flag);
		strim(v_order_data[i].send_status);		strim(v_order_data[i].send_time);
		strim(v_order_data[i].resp_code);		strim(v_order_data[i].resp_time);
		strim(v_order_data[i].busi_type);

		init(vXmlData);
		if (strcmp(v_order_data[i].det_flag,ORDERDETFLAG_Y)==0){
			init(v_orderdet_data);
			EXEC SQL SELECT order_accept,data_type,order_no,nvl(data_msg,' ') INTO :v_orderdet_data
					FROM wOrderReceiptDataDet
					WHERE order_accept=:v_order_data[i].order_accept AND data_type='0'
					ORDER BY order_no;

			if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
				EXEC SQL ROLLBACK;
				errLog(i_prog_label,"Select wOrderReceiptDataDet Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
				continue;
			}
			
			v_orderdet_num=SQLNUM;

			for (j=0;j<v_orderdet_num;j++){
				strim(v_orderdet_data[j].data_msg);
				strcat(vXmlData,v_orderdet_data[j].data_msg);
			}
		}
		else
			strcpy(vXmlData,v_order_data[i].interface_data);
	
		v_ret=OrderParseXml(&vMsgBodyType,vXmlData,v_order_data[i].order_type);
		if (v_ret!=0){
			OrderDestroyMsgBody(&vMsgBodyType);
			errLog(i_prog_label,"Process [%s] order_accept=[%ld] OrderParseXml error[%d]\n",
				i_table_label, v_order_data[i].order_accept, v_ret);		
			continue;
		}
			
		/*数据工单*/
		if (strcmp(v_order_data[i].order_type,ORDERTYPEDATA)==0){
			v_ret = dealDataOrder(&(v_order_data[i]), &vMsgBodyType, i_prog_label);
			if (v_ret){
				EXEC SQL ROLLBACK;
				errLog(i_prog_label,"Process [%s] order_accept=[%ld] dealDataOrder error[%d]\n",
						i_table_label, v_order_data[i].order_accept, v_ret);

				init(v_ret_code);
				sprintf(v_ret_code,"%d",v_ret);
				v_ret = OperOrderTable(i_prog_label,i_table_label,"1",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
				if (v_ret!=0){
					EXEC SQL ROLLBACK;
					errLog(i_prog_label, "Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
						i_table_label, v_order_data[i].order_accept, v_ret);
				}
				EXEC SQL COMMIT;				
				OrderDestroyMsgBody(&vMsgBodyType);

				continue;
			}

			v_ret=OperOrderTable(i_prog_label,i_table_label,"0",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
			if (v_ret!=0){
				EXEC SQL ROLLBACK;
				errLog(i_prog_label, "Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
					i_table_label, v_order_data[i].order_accept, v_ret);
			}
			EXEC SQL COMMIT;
		}

		/*业务工单*/
		else if (strcmp(v_order_data[i].order_type,ORDERTYPEBUSI)==0){
			if (strncmp(v_order_data[i].op_code,"2310",4)==0 || strncmp(v_order_data[i].op_code,"2312",4)==0){
				init(in_para);
				j=0;
				pParameterListType=vMsgBodyType.BusiOrder->ParameterList;
				while(pParameterListType!=NULL){
					strcpy(in_para[j],         pParameterListType->ParameterItem->ParameterValue);
					pParameterListType=pParameterListType->next;
					j++;
				}				
				v_ret=func_GoStop(in_para,v_ret_code,v_ret_msg);
				if (v_ret!=0){
					EXEC SQL ROLLBACK;
					errLog(i_prog_label,"Process [%s] order_accept=[%ld] service error[%s]\n",
							i_table_label,v_order_data[i].order_accept,v_ret_code);
					
					v_ret=OperOrderTable(i_prog_label,i_table_label,"1",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
					if (v_ret!=0){
						EXEC SQL ROLLBACK;
						errLog(i_prog_label,"Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
								i_table_label,v_order_data[i].order_accept,v_ret);							
					}
					EXEC SQL COMMIT;
					OrderDestroyMsgBody(&vMsgBodyType);
					continue;
				}

				v_ret=OperOrderTable(i_prog_label,i_table_label,"0",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
				if (v_ret!=0){
					EXEC SQL ROLLBACK;
					errLog(i_prog_label,"Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
							i_table_label,v_order_data[i].order_accept,v_ret);
				}			
				EXEC SQL COMMIT;
			}	
			else{
				/*目前只支持一维数组的服务调用*/
				init(v_service_name);	init(v_sendpara_num);	init(v_recppara_num);
				strcpy(v_service_name,vMsgBodyType.BusiOrder->CallServiceName);	
				sprintf(v_sendpara_num,"%d",vMsgBodyType.BusiOrder->SendParaNum);
				sprintf(v_recppara_num,"%d",vMsgBodyType.BusiOrder->RecpParaNum);
	
				Finit32(trans_in, len);
				Fadds32(trans_in, SEND_PARMS_NUM32, v_sendpara_num);
				Fadds32(trans_in, RECP_PARMS_NUM32, v_recppara_num);
	
				j=0;
				pParameterListType=vMsgBodyType.BusiOrder->ParameterList;
				while(pParameterListType!=NULL){
					Fadds32(trans_in, GPARM32_0+j,pParameterListType->ParameterItem->ParameterValue);
					pParameterListType=pParameterListType->next;
					j++;
				}
	
				v_ret = tpcall(v_service_name, (char *)trans_in, (long)len, (char **)&trans_out, &olen, 0);
				if (v_ret!=0){
					errLog(i_prog_label,"Process [%s] order_accept=[%ld] tpcall error[%d]\n",
							i_table_label,v_order_data[i].order_accept,v_ret);	
					
					v_ret=OperOrderTable(i_prog_label,i_table_label,"1",v_order_data[i].order_accept,v_order_data[i].det_flag,"10000","调用服务失败");
					if (v_ret!=0){
						EXEC SQL ROLLBACK;
						errLog(i_prog_label,"Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
								i_table_label,v_order_data[i].order_accept,v_ret);	
					}
					EXEC SQL COMMIT;
					OrderDestroyMsgBody(&vMsgBodyType);
					continue;
				}
				
				init(v_tmp_code);
				Fgets32(trans_out, SVC_ERR_NO32, 0, v_tmp_code);
				strcpy(v_ret_code,v_tmp_code);
				v_ret_code[6]='\0';
				init(v_tmp_code);
				Fgets32(trans_out, SVC_ERR_MSG32, 0, v_tmp_code);
				strcpy(v_ret_msg,v_tmp_code);
				v_ret_msg[100]='\0';			
				if(strcmp(v_ret_code, "000000") != 0){
					errLog(i_prog_label,"Process [%s] order_accept=[%ld] service error[%s]\n",
							i_table_label,v_order_data[i].order_accept,v_ret_code);
					
					v_ret=OperOrderTable(i_prog_label,i_table_label,"1",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
					if (v_ret!=0){
						EXEC SQL ROLLBACK;
						errLog(i_prog_label,"Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
								i_table_label,v_order_data[i].order_accept,v_ret);							
					}
					EXEC SQL COMMIT;
					OrderDestroyMsgBody(&vMsgBodyType);
					continue;
				}
				
				v_ret=OperOrderTable(i_prog_label,i_table_label,"0",v_order_data[i].order_accept,v_order_data[i].det_flag,v_ret_code,v_ret_msg);
				if (v_ret!=0){
					EXEC SQL ROLLBACK;
					errLog(i_prog_label,"Process [%s] order_accept=[%ld] OperOrderTable error[%d]\n",
							i_table_label,v_order_data[i].order_accept,v_ret);
				}			
				EXEC SQL COMMIT;
			}
		}

		OrderDestroyMsgBody(&vMsgBodyType);
	}

DealEnd:
    if(trans_in != 0)
		tpfree((char *)trans_in);
    if(trans_out != 0)
		tpfree((char *)trans_out);

	EXEC SQL COMMIT;

}

int main (argc,argv)
	int argc;
	char **argv;
{
	int		v_ret=0,i=0;
	int		v_process_num=0,v_mainmsg_label=0,v_submsg_label=0;
	PUB_CfgMsg v_chn_cfg_msg[DLMAXCFGNUM];
	
	memset(v_chn_cfg_msg,0,DLMAXCFGNUM*sizeof(PUB_CfgMsg));

	v_ret=readEnvCfg(PUB_HOME_LABEL);
	if (v_ret<0){
		printf("Get Environment Variable Error!!\n");
		exit(-1);	
	}
	
	v_ret=readProgCfg(PROG_NAME_LABEL,v_chn_cfg_msg);
	if (v_ret<0){
		printf("Get Config Values Error!!\n");
		exit(-1);	
	}

	v_process_num=atoi(v_chn_cfg_msg[0].cfgValue);
	v_mainmsg_label=atoi(v_chn_cfg_msg[1].cfgValue);
	v_submsg_label=atoi(v_chn_cfg_msg[2].cfgValue);
	strcpy(g_PUB_Source,v_chn_cfg_msg[3].cfgValue);
	g_PUB_PartitionCfg=atoi(v_chn_cfg_msg[4].cfgValue);
	g_PUB_OtherRouteFlag=atoi(v_chn_cfg_msg[5].cfgValue);

	startDaemon(PROG_NAME_LABEL);

	v_ret=PubReadNodeMsg(PROG_NAME_LABEL,v_process_num);
	if (v_ret<0){
		printf("PubReadNodeMsg Error!!\n");
		exit(-1);	
	}
	
	PubFreeSysRes(v_process_num,v_mainmsg_label,v_submsg_label);
	
	PubInitMain(PROG_NAME_LABEL,v_process_num,v_mainmsg_label,v_submsg_label);

	v_ret=PubInitTaskListOneDB(PROG_NAME_LABEL,v_process_num,PubLogicDeal);
	if (v_ret<0){
		DEBUG(PROG_NAME_LABEL,"PubInitTaskList Error!!\n");
		exit(-1);	
	}

	v_ret=PubAssignTask(PROG_NAME_LABEL,v_process_num);
	if (v_ret<0){
		DEBUG(PROG_NAME_LABEL,"PubAssignTask Error!!\n");
		exit(-1);	
	}

	PubFreeSysRes(v_process_num,v_mainmsg_label,v_submsg_label);

/*
	int		v_ret=0,i=0;
	int		v_process_num=0,v_mainmsg_label=0,v_submsg_label=0;
	PUB_CfgMsg v_chn_cfg_msg[DLMAXCFGNUM];	
		
	memset(v_chn_cfg_msg,0,DLMAXCFGNUM*sizeof(PUB_CfgMsg));

	v_ret=readEnvCfg(PUB_HOME_LABEL);
	if (v_ret<0){
		printf("Get Environment Variable Error!!\n");
		exit(-1);	
	}
	
	v_ret=readProgCfg(PROG_NAME_LABEL,v_chn_cfg_msg);
	if (v_ret<0){
		printf("Get Config Values Error!!\n");
		exit(-1);	
	}

	v_process_num=atoi(v_chn_cfg_msg[0].cfgValue);
	v_mainmsg_label=atoi(v_chn_cfg_msg[1].cfgValue);
	v_submsg_label=atoi(v_chn_cfg_msg[2].cfgValue);
	strcpy(g_PUB_Source,v_chn_cfg_msg[3].cfgValue);
	g_PUB_PartitionCfg=atoi(v_chn_cfg_msg[4].cfgValue);
	g_PUB_OtherRouteFlag=atoi(v_chn_cfg_msg[5].cfgValue);

	v_ret=readDBCfg(g_PUB_Source);
	if (v_ret<0){
		errLog(PROG_NAME_LABEL,"Get DB Config Error!!\n");
		exit(-1);	
	}

	v_ret=connectDB(PROG_NAME_LABEL,PUB_DB_LABEL_N,g_PUB_Source);
	printf("[%d][%s]\n",v_ret,g_PUB_Source);

	v_ret=PubReadNodeMsg(PROG_NAME_LABEL);
	if (v_ret<0){
		printf("PubReadNodeMsg Error!!\n");
		exit(-1);	
	}
	
	PubLogicDeal(PROG_NAME_LABEL,"0");
*/
	exit(0);
}
