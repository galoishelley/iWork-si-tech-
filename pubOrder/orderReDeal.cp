/*
**Created: Tophill(zhangkun)
**Date:    2010.01.04
*/

#include "order_xml.h"

#define PROG_NAME_LABEL	"ORDERREDEAL"

extern char g_PUB_Source[DLDATASERVER];
extern char g_PUB_Dept[DLDATASERVER];
extern PUB_PassType g_PUB_DbCfg;

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

void PubLogicDeal(i_prog_label,i_order_accept)
char *i_prog_label;
long i_order_accept;
{
	EXEC SQL BEGIN DECLARE SECTION;
		long v_order_accept;
    EXEC SQL END DECLARE SECTION;	

	v_order_accept=i_order_accept;
	
	EXEC SQL INSERT INTO wOrderReceiptXX
		SELECT order_accept,order_type,id_type,id_no,interface_type,busi_type,busi_code,
				partition_column,order_right,op_type,op_code,op_accept,op_login,total_date,
				op_time,op_note,data_format,det_flag,interface_data,commuinfo_flag,send_num,
				send_status,send_time,resp_code,resp_time
			FROM wOrderException WHERE order_accept=:v_order_accept;
	if (SQLCODE!=SQLOK){
		errLog(i_prog_label,"Insert wOrderReceiptXX Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
		EXEC SQL ROLLBACK;
		return;
	}
	
	EXEC SQL DELETE wOrderException WHERE order_accept=:v_order_accept;
	if (SQLCODE!=SQLOK){
		errLog(i_prog_label,"Delete wOrderException Error,SQLCODE=[%d],SQLERRMSG=[%s]\n",SQLCODE,SQLERRMSG);
		EXEC SQL ROLLBACK;
		return;
	}
			
	EXEC SQL COMMIT;

}

int main (argc,argv)
	int argc;
	char **argv;
{
	int		v_ret=0,i=0;
	long	v_order_accept;
	PUB_CfgMsg v_chn_cfg_msg[DLMAXCFGNUM];
    	
	memset(v_chn_cfg_msg,0,DLMAXCFGNUM*sizeof(PUB_CfgMsg));

	if (argc!=2){
		printf("Usage:orderReDeal OrderAccept!\n");
		exit(0);
	}
	
	v_order_accept=atol(argv[1]);
	
	if (v_order_accept<=0){
		printf("OrderAccept Has Error!\n");
		exit(0);
	}

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

	strcpy(g_PUB_Source,v_chn_cfg_msg[0].cfgValue);
	
	v_ret=readDBCfg(g_PUB_Source);
	if (v_ret<0){
		errLog(PROG_NAME_LABEL,"Get DB Config Error!!\n");
		exit(-1);	
	}

	v_ret=connectDB(PROG_NAME_LABEL,PUB_DB_LABEL_N,g_PUB_Source);
	if(v_ret!=0){
		errLog(PROG_NAME_LABEL,"connectDB Error!\n");
		exit(0);
	}
		
	PubLogicDeal(PROG_NAME_LABEL,v_order_accept);

	exit(0);
}
