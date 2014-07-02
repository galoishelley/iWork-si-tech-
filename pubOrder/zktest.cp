/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#include "order_xml.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

typedef struct tagdConMark
{
	char sContractNo[14+1];
	char sInitPoint[15+1];
	char sCurrentPoint[15+1];
	char sCurrentTime[17+1];
	char sMonthPoint[15+1];
	char sYearPoint[15+1];
	char sLastyearPoint[15+1];
	char sAddPoint[15+1];
	char sOwePoint[15+1];
	char sLastPoint[15+1];
	char sMonthUsed[15+1];
	char sYearUsed[15+1];
	char sTotalUsed[15+1];
	char sMinYm[6+1];
	char sContractStatus[2+1];
	char sOweLimit[15+1];
	char sStatusTime[17+1];
	char sChgFlag[1+1];
}TdConMark;

int OrderUpdateConMark(i_sendid_type,i_sendid_no,i_order_right,
					   i_op_code,i_op_accept,i_op_login,i_op_note,
					   i_contract_no,
					   i_update_sql,i_where_sql,i_parameter_array
					   )
char *i_sendid_type;
char *i_sendid_no;
int  i_order_right;

char *i_op_code;
long i_op_accept;
char *i_op_login;
char *i_op_note;

char *i_contract_no;

char *i_update_sql;
char *i_where_sql;
char i_parameter_array[][DLMAXITEMS][DLINTERFACEDATA];
{
	int v_ret;
	MsgBodyType	vMsgBodyType;
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable v_order_data;
	OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
	char v_table_name[DLTABLENAME];

	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buff[DLMAXITEMS*DLINTERFACEDATA];
		TdConMark	sTdConMark;
	EXEC SQL END DECLARE SECTION;

	Coltrim(i_sendid_no);

	init(v_sql_buff);

	sprintf(v_sql_buff,"UPDATE dConMark a SET %s WHERE a.contract_no=to_number(:NG_Param0) %s ",i_update_sql,i_where_sql);

	v_ret=OrderMultiParaDML(v_sql_buff,&i_parameter_array[0]);
	if (v_ret<0)
		return -500036;

	/*没有更新到数据，可以不做后续处理*/
	if (v_ret > 0 )
	{
		return 1;
	}
	/*没有更新到数据，可以不做后续处理*/

	EXEC SQL SELECT to_char(contract_no),
					to_char(init_point),
					to_char(current_point),
					nvl(to_char(current_time,'yyyymmdd hh24:mi:ss'),chr(0)),
					to_char(month_point),
					to_char(year_point),
					to_char(lastyear_point),
					to_char(add_point),
					to_char(owe_point),
					to_char(last_point),
					to_char(month_used),
					to_char(year_used),
					to_char(total_used),
					to_char(min_ym),
					to_char(contract_status),
					to_char(owe_limit),
					to_char(status_time,'yyyymmdd hh24:mi:ss'),
					nvl(chg_flag,chr(0))
			INTO :sTdConMark.sContractNo,
				 :sTdConMark.sInitPoint,
				 :sTdConMark.sCurrentPoint,
				 :sTdConMark.sCurrentTime,
				 :sTdConMark.sMonthPoint,
				 :sTdConMark.sYearPoint,
				 :sTdConMark.sLastyearPoint,
				 :sTdConMark.sAddPoint,
				 :sTdConMark.sOwePoint,
				 :sTdConMark.sLastPoint,
				 :sTdConMark.sMonthUsed,
				 :sTdConMark.sYearUsed,
				 :sTdConMark.sTotalUsed,
				 :sTdConMark.sMinYm,
				 :sTdConMark.sContractStatus,
				 :sTdConMark.sOweLimit,
				 :sTdConMark.sStatusTime,
				 :sTdConMark.sChgFlag
			FROM dConMark
			WHERE contract_no = :i_contract_no;

	if (SQLCODE!=SQLOK)
		return -500037;

    Coltrim(sTdConMark.sContractNo);
	Coltrim(sTdConMark.sInitPoint);
	Coltrim(sTdConMark.sCurrentPoint);
	Coltrim(sTdConMark.sCurrentTime);
	Coltrim(sTdConMark.sMonthPoint);
	Coltrim(sTdConMark.sYearPoint);
	Coltrim(sTdConMark.sLastyearPoint);
	Coltrim(sTdConMark.sAddPoint);
	Coltrim(sTdConMark.sOwePoint);
	Coltrim(sTdConMark.sLastPoint);
	Coltrim(sTdConMark.sMonthUsed);
	Coltrim(sTdConMark.sYearUsed);
	Coltrim(sTdConMark.sTotalUsed);
	Coltrim(sTdConMark.sMinYm);
	Coltrim(sTdConMark.sContractStatus);
	Coltrim(sTdConMark.sOweLimit);
	Coltrim(sTdConMark.sStatusTime);
	Coltrim(sTdConMark.sChgFlag);

	strcpy(v_table_name,"DCONMARK");
	/*组装报文开始*/
	init(vXmlString);

	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEDATA);

	OrderSetTableName(&vMsgBodyType,v_table_name);

	OrderSetPrimaryKey(&vMsgBodyType,"contract_no",     ORDERDATAFORMAT_NUMBER,i_contract_no);

	OrderSetDataItem(&vMsgBodyType,"contract_no",       ORDERDATAFORMAT_NUMBER , sTdConMark.sContractNo);
	OrderSetDataItem(&vMsgBodyType,"init_point",        ORDERDATAFORMAT_NUMBER , sTdConMark.sInitPoint);
	OrderSetDataItem(&vMsgBodyType,"current_point",     ORDERDATAFORMAT_NUMBER , sTdConMark.sCurrentPoint);
	OrderSetDataItem(&vMsgBodyType,"current_time",      ORDERDATAFORMAT_DATE   , sTdConMark.sCurrentTime);
	OrderSetDataItem(&vMsgBodyType,"month_point",       ORDERDATAFORMAT_NUMBER , sTdConMark.sMonthPoint);
	OrderSetDataItem(&vMsgBodyType,"year_point",        ORDERDATAFORMAT_NUMBER , sTdConMark.sYearPoint);
	OrderSetDataItem(&vMsgBodyType,"lastyear_point",    ORDERDATAFORMAT_NUMBER , sTdConMark.sLastyearPoint);
	OrderSetDataItem(&vMsgBodyType,"add_point",         ORDERDATAFORMAT_NUMBER , sTdConMark.sAddPoint);
	OrderSetDataItem(&vMsgBodyType,"owe_point",         ORDERDATAFORMAT_NUMBER , sTdConMark.sOwePoint);
	OrderSetDataItem(&vMsgBodyType,"last_point",        ORDERDATAFORMAT_NUMBER , sTdConMark.sLastPoint);
	OrderSetDataItem(&vMsgBodyType,"month_used",        ORDERDATAFORMAT_NUMBER , sTdConMark.sMonthUsed);
	OrderSetDataItem(&vMsgBodyType,"year_used",         ORDERDATAFORMAT_NUMBER , sTdConMark.sYearUsed);
	OrderSetDataItem(&vMsgBodyType,"total_used",        ORDERDATAFORMAT_NUMBER , sTdConMark.sTotalUsed);
	OrderSetDataItem(&vMsgBodyType,"min_ym",            ORDERDATAFORMAT_NUMBER , sTdConMark.sMinYm);
	OrderSetDataItem(&vMsgBodyType,"contract_status",   ORDERDATAFORMAT_NUMBER , sTdConMark.sContractStatus);
	OrderSetDataItem(&vMsgBodyType,"owe_limit",         ORDERDATAFORMAT_NUMBER , sTdConMark.sOweLimit);
	OrderSetDataItem(&vMsgBodyType,"status_time",       ORDERDATAFORMAT_DATE   , sTdConMark.sStatusTime);
	OrderSetDataItem(&vMsgBodyType,"chg_flag",          ORDERDATAFORMAT_STRING , sTdConMark.sChgFlag);

	v_ret = OrderGenXml(&vMsgBodyType,vXmlString);
	if (v_ret<0)
		return -500038;
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
	if (v_ret != 0)
	{
		printf("v_ret=%d\n",v_ret);
		return -500039;
	}

	return 0;

}

int OrderInsertCustMsgAdd(
	i_sendid_type,i_sendid_no,i_order_right,/*分表关键字及工单处理权重*/
	i_op_code,i_op_accept,i_op_login,i_op_note,/*操作类数据*/
	i_id_no,i_busi_type,i_user_type,i_field_code,i_field_order,i_field_value,i_other_value/*表信息类数据*/
)
char *i_sendid_type;
char *i_sendid_no;
int  i_order_right;

char *i_op_code;
long i_op_accept;
char *i_op_login;
char *i_op_note;

char *i_id_no;
char *i_busi_type;
char *i_user_type;
char *i_field_code;
char *i_field_order;
char *i_field_value;
char *i_other_value;
{
	int v_ret,i,j;
	MsgBodyType	vMsgBodyType;
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable v_order_data;
	OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
	char v_table_name[DLTABLENAME];

	EXEC SQL INSERT INTO dCustMsgAdd(id_no, busi_type, user_type,field_code, field_order, field_value, other_value)
		VALUES(to_number(:i_id_no),:i_busi_type,:i_user_type,:i_field_code,to_number(:i_field_order),:i_field_value,:i_other_value);
	if (SQLCODE!=SQLOK)
		return -500000;

	strcpy(v_table_name,"dCustMsgAdd");
	/*组装报文开始*/
	init(vXmlString);
	
	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEDATA);
	
	OrderSetTableName(&vMsgBodyType,v_table_name);

	OrderSetDataItem(&vMsgBodyType,"id_no",ORDERDATAFORMAT_NUMBER,i_id_no);	
	OrderSetDataItem(&vMsgBodyType,"busi_type",ORDERDATAFORMAT_STRING,i_busi_type);
	OrderSetDataItem(&vMsgBodyType,"user_type",ORDERDATAFORMAT_STRING,i_user_type);
	OrderSetDataItem(&vMsgBodyType,"field_code",ORDERDATAFORMAT_STRING,i_field_code);
	OrderSetDataItem(&vMsgBodyType,"field_order",ORDERDATAFORMAT_NUMBER,i_field_order);
	OrderSetDataItem(&vMsgBodyType,"field_value",ORDERDATAFORMAT_STRING,i_field_value);
	OrderSetDataItem(&vMsgBodyType,"other_value",ORDERDATAFORMAT_STRING,i_other_value);

	v_ret=OrderGenXml(&vMsgBodyType,vXmlString);
	if (v_ret<0)
		return -500001;	

	OrderDestroyMsgBody(&vMsgBodyType);		
	/*组装报文结束*/

	OrderInitStructOrder(&v_order_data);	init(v_orderdet_data);
	
	strcpy(v_order_data.order_type,ORDERTYPEDATA);
	strcpy(v_order_data.id_type,i_sendid_type);
	strcpy(v_order_data.id_no,i_sendid_no);
	strcpy(v_order_data.busi_code,v_table_name);
	v_order_data.order_right=i_order_right;
	strcpy(v_order_data.op_type,ORDEROPTYPE_INSERT);
	strcpy(v_order_data.op_code,i_op_code);
	v_order_data.op_accept=i_op_accept;
	strcpy(v_order_data.op_login,i_op_login);
	strcpy(v_order_data.op_note,i_op_note);
	strcpy(v_order_data.data_format,ORDERDATAFORMAT_XML);
	
	v_ret=OrderInserSendCrm(&v_order_data,vXmlString);
	if (v_ret!=0)
		return -500002;
	
	return 0;
}

int OrderDeleteCustMsgAdd(
	i_sendid_type,i_sendid_no,i_order_right,/*分表关键字及工单处理权重*/
	i_op_code,i_op_accept,i_op_login,i_op_note,/*操作类数据*/
	i_id_no,/*主键*/
	i_where_sql,i_parameter_array/*删除辅助信息*/
)
char *i_sendid_type;
char *i_sendid_no;
int  i_order_right;

char *i_op_code;
long i_op_accept;
char *i_op_login;
char *i_op_note;

char *i_id_no;

char *i_where_sql;
char i_parameter_array[][DLMAXITEMS][DLINTERFACEDATA];
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buff[DLMAXITEMS*DLINTERFACEDATA];	
	EXEC SQL END DECLARE SECTION;
	int v_ret,v_array_num=0,i,j;
	MsgBodyType	vMsgBodyType;
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable v_order_data;
	OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
	char v_table_name[DLTABLENAME];

	init(v_sql_buff);
	
	sprintf(v_sql_buff,"DELETE dCustMsgAdd a WHERE a.id_no=to_number(:v0) %s ",i_where_sql);
printf("--[%s]\n",i_parameter_array[0][0]);
	
	v_ret=OrderMultiParaDML(v_sql_buff,&i_parameter_array[0]);
	if (v_ret<0)
		return -500003;
	
	/*没有更新到数据，可以不做后续处理*/
	if (v_ret>0)
		return 0;

	strcpy(v_table_name,"dCustMsgAdd");

	/*组装报文开始*/
	init(vXmlString);
	
	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEDATA);
	
	OrderSetTableName(&vMsgBodyType,v_table_name);
	
	OrderSetPrimaryKey(&vMsgBodyType,"id_no",ORDERDATAFORMAT_NUMBER,i_id_no);

	v_ret=OrderGenXml(&vMsgBodyType,vXmlString);
	if (v_ret<0)
		return -500004;	

	OrderDestroyMsgBody(&vMsgBodyType);		
	/*组装报文结束*/		

	OrderInitStructOrder(&v_order_data);	init(v_orderdet_data);
	
	strcpy(v_order_data.order_type,ORDERTYPEDATA);
	strcpy(v_order_data.id_type,i_sendid_type);
	strcpy(v_order_data.id_no,i_sendid_no);
	strcpy(v_order_data.busi_code,v_table_name);
	v_order_data.order_right=i_order_right;
	strcpy(v_order_data.op_type,ORDEROPTYPE_DELETE);
	strcpy(v_order_data.op_code,i_op_code);
	v_order_data.op_accept=i_op_accept;
	strcpy(v_order_data.op_login,i_op_login);
	strcpy(v_order_data.op_note,i_op_note);
	strcpy(v_order_data.data_format,ORDERDATAFORMAT_XML);
	
	v_ret=OrderInserSendCrm(&v_order_data,vXmlString);
	if (v_ret!=0)
		return -500005;
	
	return 0;
}

int OrderUpdateCustMsgAdd(
	i_sendid_type,i_sendid_no,i_order_right,/*分表关键字及工单处理权重*/
	i_op_code,i_op_accept,i_op_login,i_op_note,/*操作类数据*/
	i_id_no,/*主键*/
	i_update_sql,i_where_sql,i_parameter_array/*更新辅助信息*/
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
	int v_ret,v_array_num=0,i,j;
	MsgBodyType	vMsgBodyType;
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	OrderTable v_order_data;
	OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
	char v_table_name[DLTABLENAME];
	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buff[DLMAXITEMS*DLINTERFACEDATA];
		char v_busi_type[4+1];
		char v_field_code[5+1];
		char v_field_order[8+1];
		char v_field_value[255+1];
		char v_other_value[255+1];		
	EXEC SQL END DECLARE SECTION;
	
	init(v_sql_buff);
	
	sprintf(v_sql_buff,"UPDATE dCustMsgAdd a SET %s WHERE a.id_no=to_number(:v0) %s ",i_update_sql,i_where_sql);
printf("--[%s][%s]\n",i_parameter_array[0][0],i_parameter_array[0][1]);	
	v_ret=OrderMultiParaDML(v_sql_buff,&i_parameter_array[0]);
	if (v_ret<0)
		return -500006;
	
	/*没有更新到数据，可以不做后续处理*/
	if (v_ret>0)
		return 0;
		
	EXEC SQL SELECT busi_type, field_code, to_char(field_order), field_value, other_value
				INTO :v_busi_type,:v_field_code,:v_field_order,:v_field_value,:v_other_value
			FROM dCustMsgAdd
			WHERE id_no=:i_id_no;
	if (SQLCODE!=SQLOK)
		return -500007;
	strim(v_busi_type);		strim(v_field_code);	strim(v_field_order);
	strim(v_field_value);	strim(v_other_value);

	strcpy(v_table_name,"dCustMsgAdd");
	
	/*组装报文开始*/
	init(vXmlString);
	
	OrderInitMsgBody(&vMsgBodyType,ORDERTYPEDATA);
	
	OrderSetTableName(&vMsgBodyType,v_table_name);
	
	OrderSetPrimaryKey(&vMsgBodyType,"id_no",ORDERDATAFORMAT_NUMBER,i_id_no);
	OrderSetDataItem(&vMsgBodyType,"id_no",ORDERDATAFORMAT_NUMBER,i_id_no);
	OrderSetDataItem(&vMsgBodyType,"busi_type",ORDERDATAFORMAT_STRING,v_busi_type);
	OrderSetDataItem(&vMsgBodyType,"field_code",ORDERDATAFORMAT_STRING,v_field_code);
	OrderSetDataItem(&vMsgBodyType,"field_order",ORDERDATAFORMAT_NUMBER,v_field_order);
	OrderSetDataItem(&vMsgBodyType,"field_value",ORDERDATAFORMAT_STRING,v_field_value);
	OrderSetDataItem(&vMsgBodyType,"other_value",ORDERDATAFORMAT_STRING,v_other_value);

	v_ret=OrderGenXml(&vMsgBodyType,vXmlString);
	if (v_ret<0)
		return -500008;	

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
	if (v_ret!=0)
		return -500009;
	
	return 0;
}

/*
返回值:
	0:正常
	1:NOTFOUND
	负值:错误
	
*/
SQLDA *bind_dp;

typedef struct tagdCheckPrepayIndex
{
	char sBankCode[5+1];
	char sCheckNo[20+1];
}TdCheckPrepayIndex;


typedef struct tagwLocalBlackListIndex
{
	char sBlackNo[20+1];
	char sBlackType[1+1];
}TwLocalBlackListIndex;


int myTest(oldIndex,i_parameter_array
					   )
TdCheckPrepayIndex oldIndex;
char i_parameter_array[][DLMAXITEMS][DLINTERFACEDATA];
{
printf("####[%s]\n",oldIndex.sBankCode);
printf("####[%s]\n",oldIndex.sCheckNo);
printf("####[%s]\n",i_parameter_array[0][0]);
printf("####[%s]\n",i_parameter_array[0][1]);
return 0;
}

int myTest1(oldIndex,i_parameter_array
					   )
TwLocalBlackListIndex oldIndex;
char i_parameter_array[][DLMAXITEMS][DLINTERFACEDATA];
{
printf("####[%s]\n",oldIndex.sBlackNo);
printf("####[%s]\n",oldIndex.sBlackType);
printf("####[%s]\n",i_parameter_array[0][0]);
printf("####[%s]\n",i_parameter_array[0][1]);
return 0;
}

int main (argc,argv)
	int argc;
	char **argv;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char this_user[30];
		char this_pwd[30];
		char this_srv[30];
		
		char v_first[40];
		char v_secend[40];
		char v_tmp_buf[10000];
		char vXmlData[DLINTERFACEARRAY*DLINTERFACEDATA];
		
		char v_a[4][11];

		char v_group_id[11];
		int  v_min_accept;
	EXEC SQL END DECLARE SECTION;	
	char v_id_no[30];
	char v_update_sql[1000];
	char v_where_sql[1000];
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int v_ret=0,i;
	TdCheckPrepayIndex oldIndex;
	TwLocalBlackListIndex myoldIndex;
	double vPoint=5.8;
	char	vContractNo[20];
	
	MsgBodyType	vMsgBodyType;
	
	init(v_parameter_array);
	memset(&oldIndex, 0, sizeof(oldIndex));
	memset(&myoldIndex, 0, sizeof(myoldIndex));
	
	strcpy(this_user,"dbcustadm");
	strcpy(this_pwd,"dbcustadm123");
	strcpy(this_srv,"THCUST2");
		
	EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd USING :this_srv;

	for (i=0;i<21;i++){
		printf("[%02d]\n",i);	
	}
	
exit(0);
	strcpy(vContractNo,"10101015543357");

	init(v_parameter_array);
	sprintf(v_parameter_array[0],"%f", vPoint);
	sprintf(v_parameter_array[1],"%f", vPoint);
	sprintf(v_parameter_array[2],"%f", vPoint);
	strcpy(v_parameter_array[3],vContractNo);

	v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,vContractNo,100,
		"1234",99999999,"system","测试",
		vContractNo,
		" month_used=month_used+to_number(:vPoint),total_used=total_used+to_number(:vPoint),year_used=year_used+to_number(:vPoint) ",
		"",v_parameter_array);	
	EXEC SQL COMMIT;
	exit(0);
						   		
/*	
	strcpy(v_a[0],"123");
	strcpy(v_a[1],"123    ");
	strcpy(v_a[2],"123");
	strcpy(v_a[3],"123    ");
	
	strcpy(v_parameter_array[0],"null");
	strcpy(v_parameter_array[1],"NULL    ");
	strcpy(v_parameter_array[2],"");
	strcpy(v_parameter_array[3],"");	
	
	init(vXmlData);
	strncpy(vXmlData,v_parameter_array[2],10);
	
	printf("[%s]\n",vXmlData);
	
	
	v_ret=OrderMultiParaDML("INSERT INTO ZKZK VALUES(:v1,:v2,:v3,to_number(:v4))",&v_parameter_array);
	

	
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	
	EXEC SQL COMMIT;
*/
	
/*
	v_ret=OrderInsertCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789","1111","V","22222","0","33333333","测试测试");
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;
*/

/*
	strcpy(v_parameter_array[0],"123456");
	strcpy(v_parameter_array[1],"123456789");
	v_ret=OrderUpdateCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789",
			"field_value=:v2","",v_parameter_array);
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL ROLLBACK;
	EXEC SQL COMMIT;
*/

/*
	strcpy(v_parameter_array[0],"123456789");
	v_ret=OrderDeleteCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789",
			"",&v_parameter_array);
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	
	EXEC SQL ROLLBACK;
	EXEC SQL COMMIT;
*/

	exit(0);
}
