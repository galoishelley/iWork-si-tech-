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

#define PROG_NAME_LABEL	"ORDERDEALBOSS"

extern char g_PUB_Source[DLDATASERVER];
extern PUB_PassType g_PUB_DbCfg;

int g_PUB_PartitionCfg;
int g_PUB_OtherRouteFlag;

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;


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
