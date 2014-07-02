/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#ifndef _ORDER_XML_H_
#define _ORDER_XML_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <varargs.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <signal.h>


#include <netdb.h>
#include <sys/ipc.h>
#include <sys/socket.h>

#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stream.h>
#include <dirent.h>
#include <fcntl.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "des.h"

#define init(a) memset(a, 0, sizeof(a))

#define PUB_MAPPARM PROT_READ|PROT_WRITE

/*配置文件信息*/
#define PUB_CFG_FILE "common.cfg"
/*配置文件中公共参数部分*/
#define PUB_HOME_LABEL 				"ORDERPUB"
#define PUB_PROG_PARAMETER			"PUB_PARAMETER"
#define PUB_DB_LABEL_N 				0
#define PUB_DB_LABEL_Y 				1
#define PUB_DEFAULT_LOGPATH_LABEL 	"DEFAULT_LOG_PATH"
#define PUB_LOGPATH_ATTRNAME 		"LOG_PATH"

#define DLCFGATTR 		256
#define DLDIRNAME 		256
#define DLFILENAME 		64
#define DLMAXCFGNUM 	20
#define DLDBSQL 		10000
#define DLDATETIME		18
#define DLTABLELABEL	3
#define DLCOMMTYPE		2

#define DLDATASERVER	30
#define DLDATABASE 		30
#define DLDATAUSER 		30
#define DLDATAPASSWD 	30

#define NODE_ARRAY 100		/*处理任务节点数组大小*/

#define PUB_BATCHORDER_NUM	 500		/*工单处理一批次数量*/

/*返回代码*/
#define FORKERROR		-101

/*状态信息*/
#define PUB_REFRESH_N 	'0'
#define PUB_REFRESH_Y 	'1'
#define PUB_REFRESH_I 	'2'	/*轮空标志*/
#define PUB_STATUS_N 	0
#define PUB_STATUS_Y 	1
#define PACKETSIZE		512 

/*加密信息*/
/*#define PUB_MAINKEY "CHANNEL"*/
#define PUB_MAINKEY "bossboss"
#define PUB_MAXENCRYPTLEN 65
#define PUB_MAXDECRYPTLEN 65

#define DLDATANAME			51
#define DLDATAVALUE			2001
#define DLTABLENAME			51
#define DLPARAMETERVALUE	2001
#define DLSERVICENAME		501
#define DLLABELNAME			51

#define DLINTERFACEDATA		4001
#define DLINTERFACEARRAY	8

#define DLMAXITEMS 150
#define DLMAXVNAMELEN 31
#define DLMAXINAMELEN 31


#define ORDERDATAFORMAT_STRING		0
#define ORDERDATAFORMAT_NUMBER		1
#define ORDERDATAFORMAT_DATE		2

#define ORDEROWNER_CRM				"DBCUSTADM"
#define ORDEROWNER_BOSS				"DBACCADM"

/*数据常量定义*/

/*工单类型 1：数据工单；2：业务工单；3：接口工单*/
#define ORDERTYPEDATA		"1"
#define ORDERTYPEBUSI		"2"
#define ORDERTYPEINTR		"3"

/*ID类型 0：手机号码；1：客户；2：用户；3：帐户；4：操作员；5：流水；9：空值*/
#define ORDERIDTYPE_PHONE			"0"
#define ORDERIDTYPE_CUST			"1"
#define ORDERIDTYPE_USER			"2"
#define ORDERIDTYPE_CNTR			"3"
#define ORDERIDTYPE_OPER			"4"
#define ORDERIDTYPE_ACCP			"5"
#define ORDERIDTYPE_NULL			"9"

/*操作类型 1：增加；2：删除；3：修改；4：其它*/
#define ORDEROPTYPE_INSERT			"1"
#define ORDEROPTYPE_DELETE			"2"
#define ORDEROPTYPE_UPDATE			"3"
#define ORDEROPTYPE_OTHER			"4"

/*数据格式 0：无内容；1：XML格式；2：MML格式*/
#define ORDERDATAFORMAT_NULL		"0"
#define ORDERDATAFORMAT_XML			"1"
#define ORDERDATAFORMAT_MML			"2"

/*服务调用方式 1:tp_call;	2:应用集成平台*/
#define CALLSERV_TP	1
#define CALLSERV_CA	2

/*明细标志
0：数据在主表中存放；
1：数据超过4000，在明细表中存放；
*/
#define ORDERDETFLAG_N			"0"
#define ORDERDETFLAG_Y			"1"

#define LABELTABLENAME			"TableName"
#define LABELPRIMARYKEYLIST		"PrimaryKeyList"
#define LABELDATAITEMLIST		"DataItemList"
#define LABELDATANAME			"DataName"
#define LABELDATAFORMAT			"DataFormat"
#define LABELDATAVALUE			"DataValue"
#define LABELSERVICENAME		"CallServiceName"
#define LABELSENDPARANUM		"SendParaNum"
#define LABELRECPPARANUM		"RecpParaNum"
#define LABELPARAMETERNAME		"ParameterName"
#define LABELPARAMETERVALUE		"ParameterValue"

#define SQLCODE 		sqlca.sqlcode
#define SQLNUM 			sqlca.sqlerrd[2]
#define SQLERRMSG 		sqlca.sqlerrm.sqlerrmc		
#define NOTFOUND		1403
#define SQLOK			0

/*数据类型定义*/
typedef struct{
	char cfgName[DLCFGATTR];
	char cfgValue[DLCFGATTR];
}PUB_CfgMsg;

/*进程控制*/
typedef struct{
	pid_t pid;
	int pos;
}OrderTaskShmType;

/*消息队列结构*/
typedef struct{
	long mtype;
	char tableLebel[DLTABLELABEL];
	int	 msgPos;				/*对应消息队列序号*/	
	char refresh;
}OrderMsgInfoType;
#define ORDERMSGINFOTYPESIZE sizeof(OrderMsgInfoType)

typedef struct{
	char srvName[DLDATASERVER];
	char dataBase[DLDATABASE];
	char loginNo[DLDATAUSER];
	char passWord[DLDATAPASSWD];
}PUB_PassType;

/*节点配置信息*/
typedef struct{
	char tableLebel[DLTABLELABEL];
	int  status;
}PubNodeCfg;
#define PUBNODECFGSIZE sizeof(PubNodeCfg)

typedef struct{
	char				DataName[DLDATANAME];
	int					DataFormat;/*0:STRING;1:NUMBER;2:DATE*/
	char				DataValue[DLDATAVALUE];
}DataItemType;
#define DATAITEMTYPESIZE sizeof(DataItemType)

struct StructDataItemListType{
	DataItemType		*DataItem;
	struct StructDataItemListType	*next;	
};
typedef struct StructDataItemListType DataItemListType;
#define DATAITEMLISTTYPESIZE sizeof(DataItemListType)

typedef struct{
	char				TableName[DLTABLENAME];
	DataItemListType	*PrimaryKeyList;
	DataItemListType	*DataItemList;
}DataOrderType;
#define DATAORDERTYPESIZE sizeof(DataOrderType)

typedef struct{
	char				ParameterName[DLDATANAME];
	char				ParameterValue[DLPARAMETERVALUE];
}ParameterItemType;
#define PARAMETERITEMTYPESIZE sizeof(ParameterItemType)

struct StructParameterListType{
	ParameterItemType		*ParameterItem;
	struct StructParameterListType	*next;	
};
typedef struct StructParameterListType ParameterListType;
#define PARAMETERLISTTYPESIZE sizeof(ParameterListType)

typedef struct{
	char				CallServiceName[DLSERVICENAME];
	int					SendParaNum;
	int					RecpParaNum;
	ParameterListType	*ParameterList;
}BusiOrderType;
#define BUSIORDERTYPESIZE sizeof(BusiOrderType)

typedef struct{
	DataOrderType		*DataOrder;
	BusiOrderType		*BusiOrder;
}MsgBodyType;
#define MSGBODYTYPESIZE sizeof(MsgBodyType)

struct StructOrderTable{
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
};
typedef struct StructOrderTable OrderTable;
#define ORDERTABLESIZE sizeof(OrderTable)
 
struct StructOrderDetTable{
	long		order_accept;
	char		data_type[DLCOMMTYPE];
	int			order_no;
	char		data_msg[DLINTERFACEDATA];
};
typedef struct StructOrderDetTable OrderDetTable;
#define ORDERDETTABLESIZE sizeof(OrderDetTable)
  
extern struct SQLDA *sqlald( int, size_t, size_t );

int OrderSetPrimaryKey(MsgBodyType	*vMsgBodyType,char *vDataName,int  vDataFormat,char *vDataValue);
int OrderSetTableName(MsgBodyType	*vMsgBodyType,char *vTableName);
void OrderInitStructOrder(OrderTable *v_order_data);
int OrderInitMsgBody(MsgBodyType	*vMsgBodyType,char *vOrderType);
int OrderSetDataItem(MsgBodyType	*vMsgBodyType,char *vDataName,int  vDataFormat,char *vDataValue);
int OrderGenXml(MsgBodyType	*vMsgBodyType,char *vXmlString);
int OrderMultiParaDML(char *vSqlBuf,char vParameterArray[][DLMAXITEMS][DLINTERFACEDATA]);
int OrderDestroyMsgBody(MsgBodyType	*vMsgBodyType);
int OrderInserSend(char *v_table_owner,OrderTable *v_order_data,char *v_order_string);
int OrderInserSendCrm(OrderTable *v_order_data,char *v_order_string);
int OrderInserSendBoss(OrderTable *v_order_data,char *v_order_string);

#endif
