/*
**Created: Tophill(zhangkun)
**Date:    2009.03.19
*/

#include "order_xml.h"

#define PROG_NAME_LABEL	"ORDERTRANSFERCRM"

extern char g_PUB_Source[DLDATASERVER];
extern char g_PUB_Dept[DLDATASERVER];
extern PUB_PassType g_PUB_DbCfg;

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

void PubLogicDeal(i_prog_label,i_db_source,i_db_dest,i_table_label)
char *i_prog_label;
char *i_db_source;
char *i_db_dest;
char *i_table_label;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_sql_buf[DLDBSQL];
		int  v_rownum=PUB_BATCHORDER_NUM;
		int  v_orderdet_num=0;
		OrderTable v_order_data[PUB_BATCHORDER_NUM];
		OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
    EXEC SQL END DECLARE SECTION;	
    int i,j;

	init(v_sql_buf);	init(v_order_data);

	sprintf(v_sql_buf,"SELECT order_accept,order_type,id_type,nvl(id_no,' '),nvl(interface_type,' '),nvl(busi_type,' '),\
				nvl(busi_code,' '),partition_column,nvl(order_right,0),nvl(op_type,' '),nvl(op_code,' '),nvl(op_accept,0),\
				nvl(op_login,' '),total_date,nvl(to_char(op_time,'YYYYMMDD HH24:MI:SS'),' '),nvl(op_note,' '),\
				data_format,det_flag,nvl(interface_data,' '),\
				nvl(commuinfo_flag,' '),nvl(send_num,0),nvl(send_status,' '),\
				nvl(to_char(send_time,'YYYYMMDD HH24:MI:SS'),' '),nvl(resp_code,' '),\
				nvl(to_char(resp_time,'YYYYMMDD HH24:MI:SS'),' ')\
			FROM wOrderSend%s \
			WHERE order_type=any('1','2') ORDER BY order_accept",i_table_label);
	
	EXEC SQL AT :i_db_source PREPARE SelectStr FROM :v_sql_buf;
	
	if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND)
	{
		errLog(i_prog_label,"Select wOrderSend%s PREPARE Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",
			i_table_label,SQLCODE,SQLERRMSG);
		return;
	}
	
	EXEC SQL AT :i_db_source DECLARE SelectCur CURSOR FOR SelectStr;
	EXEC SQL AT :i_db_source OPEN SelectCur;
		
	if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND)
	{
		errLog(i_prog_label,"Select wOrderSend%s OPEN Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",
			i_table_label,SQLCODE,SQLERRMSG);
		EXEC SQL AT :i_db_source CLOSE SelectCur;
		return;
	}
	
	EXEC SQL AT :i_db_source FOR :v_rownum FETCH SelectCur INTO :v_order_data;
	if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND)
	{
		errLog(i_prog_label,"Select wOrderSend%s FETCH Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",
			i_table_label,SQLCODE,SQLERRMSG);
		EXEC SQL AT :i_db_source CLOSE SelectCur;
		return;
	}
	
	EXEC SQL AT :i_db_source CLOSE SelectCur;

	if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND)
	{
		errLog(i_prog_label,"Select wOrderSend%s Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",
			i_table_label,SQLCODE,SQLERRMSG);
		return;
	}
	
	v_rownum=SQLNUM;

	if (v_rownum<=0)
		return;
	
	for (i=0;i<v_rownum;i++){
		strim(v_order_data[i].id_no);			strim(v_order_data[i].interface_type);
		strim(v_order_data[i].busi_code);		strim(v_order_data[i].op_type);
		strim(v_order_data[i].op_code);			strim(v_order_data[i].op_login);
		strim(v_order_data[i].op_time);			strim(v_order_data[i].op_note);	
		strim(v_order_data[i].interface_data);	strim(v_order_data[i].commuinfo_flag);
		strim(v_order_data[i].send_status);		strim(v_order_data[i].send_time);
		strim(v_order_data[i].resp_code);		strim(v_order_data[i].resp_time);
		strim(v_order_data[i].busi_type);
	}

	init(v_sql_buf);
	sprintf(v_sql_buf,"INSERT INTO wOrderReceipt%s \
			VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,\
				to_date(:v15, 'YYYYMMDD HH24:MI:SS'),:v16,:v17,:v18,:v19,:v20,:v21,\
				:v22,to_date(:v23, 'YYYYMMDD HH24:MI:SS'),:v24,to_date(:v25, 'YYYYMMDD HH24:MI:SS'))",
		i_table_label);
	EXEC SQL AT :i_db_dest PREPARE InsertStr FROM :v_sql_buf;
	EXEC SQL AT :i_db_dest FOR :v_rownum EXECUTE InsertStr USING :v_order_data;
	if (SQLCODE!=SQLOK){
		errLog(i_prog_label,"Insert wOrderReceipt%s Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",i_table_label,SQLCODE,SQLERRMSG);
		EXEC SQL AT :i_db_dest ROLLBACK;
		return;	
	}
	
	init(v_sql_buf);
	sprintf(v_sql_buf,"INSERT INTO wOrderSendHis \
			VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,\
				to_date(:v15, 'YYYYMMDD HH24:MI:SS'),:v16,:v17,:v18,:v19,:v20,:v21,\
				:v22,to_date(:v23, 'YYYYMMDD HH24:MI:SS'),:v24,to_date(:v25, 'YYYYMMDD HH24:MI:SS'),'9',sysdate)");
	EXEC SQL AT :i_db_source PREPARE InsertHisStr FROM :v_sql_buf;
	EXEC SQL AT :i_db_source FOR :v_rownum EXECUTE InsertHisStr USING :v_order_data;
	if (SQLCODE!=SQLOK){
		errLog(i_prog_label,"Insert wOrderSendHis Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
		EXEC SQL AT :i_db_source ROLLBACK;
		EXEC SQL AT :i_db_dest ROLLBACK;
		return;	
	}

	init(v_sql_buf);
	sprintf(v_sql_buf,"DELETE wOrderSend%s WHERE order_accept=:v1",i_table_label);	

	EXEC SQL AT :i_db_source PREPARE DeleteStr FROM :v_sql_buf;
	for (i=0;i<v_rownum;i++){
		EXEC SQL AT :i_db_source EXECUTE DeleteStr USING :v_order_data[i].order_accept;
		if (SQLCODE!=SQLOK){
			errLog(i_prog_label,"Delete wOrderSend%s Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",i_table_label,SQLCODE,SQLERRMSG);
			EXEC SQL AT :i_db_source ROLLBACK;
			EXEC SQL AT :i_db_dest ROLLBACK;	
			return;
		}
	}

	/*处理数据明细表开始*/
	for (i=0;i<v_rownum;i++){
		if (strcmp(v_order_data[i].det_flag,ORDERDETFLAG_Y)==0){
			v_orderdet_num=DLINTERFACEARRAY;
			init(v_orderdet_data);
			
			EXEC SQL AT :i_db_source 
				SELECT order_accept,data_type,order_no,nvl(data_msg,' ') INTO :v_orderdet_data
					FROM wOrderSendDataDet
					WHERE order_accept=:v_order_data[i].order_accept AND data_type='0';
			if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
				errLog(i_prog_label,"Select wOrderSendDataDet Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
				EXEC SQL AT :i_db_source ROLLBACK;
				EXEC SQL AT :i_db_dest ROLLBACK;
				return;
			}
			
			v_orderdet_num=SQLNUM;	
			
			if (v_orderdet_num==0)
				continue;

			for (j=0;j<v_orderdet_num;j++){
				strim(v_orderdet_data[j].data_msg);	
			}
			
			init(v_sql_buf);
			sprintf(v_sql_buf,"INSERT INTO wOrderReceiptDataDet VALUES(:v1,:v2,:v3,:v4)");
			EXEC SQL AT :i_db_dest PREPARE InsertDataDetStr FROM :v_sql_buf;
			EXEC SQL AT :i_db_dest FOR :v_orderdet_num EXECUTE InsertDataDetStr USING :v_orderdet_data;
			if (SQLCODE!=SQLOK){
				errLog(i_prog_label,"Insert wOrderReceiptDataDet Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
				EXEC SQL AT :i_db_source ROLLBACK;
				EXEC SQL AT :i_db_dest ROLLBACK;
				return;	
			}
			
			EXEC SQL AT :i_db_source INSERT INTO wOrderSendDataDetHis
				SELECT order_accept,data_type,order_no,data_msg FROM wOrderSendDataDet
					WHERE order_accept=:v_order_data[i].order_accept AND data_type='0';
			if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
				errLog(i_prog_label,"Insert wOrderSendDataDetHis Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
				EXEC SQL AT :i_db_source ROLLBACK;
				EXEC SQL AT :i_db_dest ROLLBACK;
				return;
			}
			
			EXEC SQL AT :i_db_source DELETE wOrderSendDataDet
					WHERE order_accept=:v_order_data[i].order_accept AND data_type='0';
			if (SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
				errLog(i_prog_label,"Delete wOrderSendDataDet Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
				EXEC SQL AT :i_db_source ROLLBACK;
				EXEC SQL AT :i_db_dest ROLLBACK;
				return;
			}
		}
	}
	/*处理数据明细表结束*/
	EXEC SQL AT :i_db_dest COMMIT;	
	if (SQLCODE!=SQLOK){
		errLog(i_prog_label,"Commit BOSSDB Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
		EXEC SQL AT :i_db_source ROLLBACK;
		return;
	}

	EXEC SQL AT :i_db_source COMMIT;

	return;
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
	strcpy(g_PUB_Dept,v_chn_cfg_msg[4].cfgValue);

	startDaemon(PROG_NAME_LABEL);

	v_ret=PubReadNodeMsg(PROG_NAME_LABEL,v_process_num);
	if (v_ret<0){
		printf("PubReadNodeMsg Error!!\n");
		exit(-1);	
	}

	PubFreeSysRes(v_process_num,v_mainmsg_label,v_submsg_label);
	
	PubInitMain(PROG_NAME_LABEL,v_process_num,v_mainmsg_label,v_submsg_label);

	v_ret=PubInitTaskListTwoDB(PROG_NAME_LABEL,v_process_num,PubLogicDeal);
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

	exit(0);
}
