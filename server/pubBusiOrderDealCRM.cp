#include "boss_srv.h"
#include "publicsrv.h"

#define _DEBUG_
#define JIANGLIJIFEN 2

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;


/*
 * 函数名称:bodc_ServiceOpen
 * 功能描述:服务开通函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		long	v_id_no;
		char	v_new_run[1+1];
		
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_time[17+1];
		char	v_op_note[100+1];
		char	v_org_code[9+1];
		char	v_org_id[10+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/

int bodc_ServiceOpen(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long	v_id_no;
		char	v_new_run[1+1];
		
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_time[17+1];
		char	v_op_note[100+1];
		char	v_org_code[9+1];
		char	v_org_id[10+1];
		
		char	v_id_str[22+1];
		char	v_phone_no[15+1];
		char	v_sm_code[2+1];
		char	v_belong_code[7+1];
		char	v_attr_code[8+1];
		char	v_group_id[10+1];
		long	v_offon_accept;
		char	v_total_date[8+1];
		char	v_run_code[2+1];
	
	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	tChgList  chgList;
	int 	v_ret=0;
	
	init(v_parameter_array);		init(v_phone_no);
	init(v_sm_code);				init(v_belong_code);
	init(v_attr_code);				init(v_group_id);
	init(v_total_date);				init(v_run_code);
	init(v_ret_code);				init(v_ret_msg);
	memset(&chgList,0,sizeof(chgList));

	v_id_no=atol(in_para[0]);
	strcpy(v_id_str,in_para[0]);
	strcpy(v_new_run,in_para[1]);
	strcpy(v_op_login,in_para[2]);
	strcpy(v_op_code,in_para[3]);
	v_op_accept=atol(in_para[4]);
	strcpy(v_op_time,in_para[5]);
	strcpy(v_op_note,in_para[6]);
	strcpy(v_org_code,in_para[7]);
	strcpy(v_org_id,in_para[8]);

	strcpy(v_parameter_array[0],v_new_run);
	strcpy(v_parameter_array[1],v_op_time);
	strcpy(v_parameter_array[2],v_id_str);
	strcpy(v_parameter_array[3],v_op_time);

	/*更新用户信息表*/
	v_ret=OrderUpdateCustMsg(ORDERIDTYPE_USER,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
			v_id_str," run_code=substr(run_code,2,1)||:v1,run_time=to_date(:v2,'YYYYMMDD HH24:MI:SS') ",
			" AND run_time<=to_date(:v3,'YYYYMMDD HH24:MI:SS') ",v_parameter_array);
	if (v_ret<0){
		strcpy(v_ret_code,"600000");
		sprintf(v_ret_msg,"更新用户信息错误[%d]!",v_ret);
		return -600000;		
	}
	if (v_ret>0){/*工单已经失效，不再继续处理*/
		strcpy(v_ret_code,"000000");
		strcpy(v_ret_msg,"处理成功!");		
		return 0;		
	}
	
	EXEC SQL SELECT run_code,phone_no,sm_code,belong_code,attr_code,group_id
			INTO :v_run_code,:v_phone_no,:v_sm_code,:v_belong_code,:v_attr_code,:v_group_id
		FROM dCustMsg
		WHERE id_no=:v_id_no;
	if (SQLCODE!=OK){
		strcpy(v_ret_code,"600001");
		strcpy(v_ret_msg,"获取用户信息错误!");
		return -600001;
	}
	
	/*处理开关机命令*/
	EXEC SQL SELECT sOffOn.nextval INTO :v_offon_accept FROM dual;
	if (SQLCODE!=OK){
		strcpy(v_ret_code,"600002");
		strcpy(v_ret_msg,"获取开关机流水错误!");
		return -600002;
	}
	
	strncpy(v_total_date,v_op_time,8);
	chgList.vCommand_Id          =  v_offon_accept;
	chgList.vId_No               =  v_id_no;
	chgList.vTotal_Date          =  atoi(v_total_date);
	chgList.vLogin_Accept        =  v_op_accept;
	strncpy(chgList.vBelong_Code,   v_belong_code,    7);
	strncpy(chgList.vSm_Code,       v_sm_code,        2);
	strncpy(chgList.vAttr_Code,     v_attr_code,      8);
	strncpy(chgList.vPhone_No,      v_phone_no,      15);
	strncpy(chgList.vRun_Code,      v_run_code,        2);
	strncpy(chgList.vOrg_Code,      v_org_code,       9);
	strncpy(chgList.vLogin_No,      v_op_login,       6);
	strncpy(chgList.vOp_Code,       v_op_code,        4);
	strcpy(chgList.vChange_Reason,  v_op_note		   );
	strncpy(chgList.vGroup_Id,      v_group_id,      10);
	strncpy(chgList.vOrg_Id,        v_org_id,        10);	

	v_ret = recordChgList(&chgList,v_ret_msg);
	if(v_ret != RET_OK){
		strcpy(v_ret_code,"600003");
		sprintf(v_ret_msg,"进行开关机操作错误[%d]!",v_ret);
		return -600003;
	}	

	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
			
	return 0;
}

/*
 * 函数名称:bodc_UpdateRunCode
 * 功能描述:更新用户运行状态，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		long	v_id_no;
		char	v_new_run[1+1];
		
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_time[17+1];
		char	v_op_note[100+1];
		char	v_org_code[9+1];
		char	v_org_id[10+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
/*update by liuzhou at 20090911 改动bodc_ServiceOpen函数,CRM侧只负责更新用户的运行状态*/

int bodc_UpdateRunCode(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
		char	v_id_str[22+1];
		char	v_new_run[1+1];		
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_time[17+1];
		char	v_op_note[100+1];

	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int 	v_ret = 0;
	v_op_accept = 0;
	
	init(v_parameter_array);	
	init(v_ret_code);				
	init(v_ret_msg);

	strcpy(v_id_str, in_para[0]);
	strcpy(v_new_run, in_para[1]);
	strcpy(v_op_login, in_para[2]);
	strcpy(v_op_code, in_para[3]);
	v_op_accept = atol(in_para[4]);
	strcpy(v_op_time, in_para[5]);
	strcpy(v_op_note, in_para[6]);

	strcpy(v_parameter_array[0], v_new_run);
	strcpy(v_parameter_array[1], v_op_time);
	strcpy(v_parameter_array[2], v_id_str);
	strcpy(v_parameter_array[3], v_op_time);

	/*更新用户信息表*/
	v_ret=OrderUpdateCustMsg(ORDERIDTYPE_USER, v_id_str, 100, v_op_code, v_op_accept, v_op_login, v_op_note,
			v_id_str, " run_code=substr(run_code,2,1)||:v1,run_time=to_date(:v2,'YYYYMMDD HH24:MI:SS') ",
			" AND run_time<=to_date(:v3,'YYYYMMDD HH24:MI:SS') ", v_parameter_array);
	if (v_ret < 0)
	{
		strcpy(v_ret_code,"600000");
		sprintf(v_ret_msg,"更新用户信息错误[%d]!", v_ret);
		return -600000;		
	}
	if (v_ret > 0){/*工单已经失效，不再继续处理*/
		strcpy(v_ret_code, "000000");
		strcpy(v_ret_msg, "处理成功!");		
		return 0;		
	}
		
	strcpy(v_ret_code, "000000");
	strcpy(v_ret_msg, "处理成功!");
			
	return 0;
}

/*
 * 函数名称:bodc_UpdateCheckPrepay
 * 功能描述:更新支票预存函数，本函数用于替换PublicInsertCheckPrepay函数,本函数在BOSS函数库中，需要从BOSS函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
	
		int		v_total_date;
		long	v_in_pay_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_bank_code[5+1];
		char	v_check_no[20+1];
		double	v_pay_money;
		char	v_pay_note[60+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_UpdateCheckPrepay(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int		v_total_date;
		long	v_in_pay_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_bank_code[5+1];
		char	v_check_no[20+1];
		double	v_pay_money;
		char	v_pay_note[60+1];
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	v_total_date=atoi(in_para[0]);
	v_in_pay_accept=atol(in_para[1]);
	strcpy(v_op_code,in_para[2]);
	strcpy(v_login_no,in_para[3]);
	strcpy(v_org_code,in_para[4]);
	strcpy(v_bank_code,in_para[5]);
	strcpy(v_check_no,in_para[6]);
	v_pay_money=atof(in_para[7]);
	strcpy(v_pay_note,in_para[8]);
	
	v_ret=PublicInsertCheckPrepay_crm(v_total_date,v_in_pay_accept,v_op_code,v_login_no,v_org_code,
			v_bank_code,v_check_no,v_pay_money,v_pay_note);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600010");
		sprintf(v_ret_msg,"更新支票信息错误[%d]!",v_ret);
		return -600010;			
	}

	return 0;	
}

/*
 * 函数名称:bodc_UndoExpireTime
 * 功能描述:有效期回退函数，本函数用于替换PublicUndoExpireTime函数,本函数在BOSS函数库中，需要从BOSS函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		long	v_id_no;
		int		v_pay_date;
		long	v_pay_accept;
		int		v_total_date;
		long	v_login_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_op_time[17+1];
		char	v_op_note[60+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_UndoExpireTime(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long	v_id_no;
		int		v_pay_date;
		long	v_pay_accept;
		int		v_total_date;
		long	v_login_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_op_time[17+1];
		char	v_op_note[60+1];
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	v_id_no=atol(in_para[0]);
	v_pay_date=atoi(in_para[1]);
	v_pay_accept=atol(in_para[2]);
	v_total_date=atoi(in_para[3]);
	v_login_accept=atol(in_para[4]);
	strcpy(v_op_code,in_para[5]);
	strcpy(v_login_no,in_para[6]);
	strcpy(v_org_code,in_para[7]);
	strcpy(v_op_time,in_para[8]);
	strcpy(v_op_note,in_para[9]);

	v_ret=PublicUndoExpireTime_crm(v_id_no,v_pay_date,v_pay_accept,v_total_date,v_login_accept,
			v_op_code,v_login_no,v_org_code,v_op_time,v_op_note);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600020");
		sprintf(v_ret_msg,"回退有效期错误[%d]!",v_ret);
		return -600020;			
	}

	return 0;	
}

/*
 * 函数名称:bodc_UpdateExpireTime
 * 功能描述:有效期更新函数，本函数用于替换PublicUpdateExpireTime函数,本函数在BOSS函数库中，需要从BOSS函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		long	v_id_no;
		char	v_sm_code[2+1];
		char	v_region_code[2+1];
		double	v_pay_money=0.0;
		int		v_total_date;
		long	v_login_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_op_time[17+1];
		char	v_op_note[60+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_UpdateExpireTime(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long	v_id_no;
		char	v_sm_code[2+1];
		char	v_region_code[2+1];
		double	v_pay_money=0.0;
		int		v_total_date;
		long	v_login_accept;
		char	v_op_code[4+1];
		char	v_login_no[6+1];
		char	v_org_code[9+1];
		char	v_op_time[17+1];
		char	v_op_note[60+1];
		
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	v_id_no=atol(in_para[0]);
	strcpy(v_sm_code,in_para[1]);
	strcpy(v_region_code,in_para[2]);
	v_pay_money=atof(in_para[3]);
	v_total_date=atoi(in_para[4]);
	v_login_accept=atol(in_para[5]);
	strcpy(v_op_code,in_para[6]);
	strcpy(v_login_no,in_para[7]);
	strcpy(v_org_code,in_para[8]);
	strcpy(v_op_time,in_para[9]);
	strcpy(v_op_note,in_para[10]);

	v_ret=PublicUpdateExpireTime_crm(v_id_no,v_sm_code,v_region_code, v_pay_money,
                            v_total_date,v_login_accept,v_op_code,v_login_no,
                            v_org_code,v_op_time,v_op_note);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600020");
		sprintf(v_ret_msg,"更新有效期错误[%d]!",v_ret);
		return -600020;			
	}

	return 0;	
}

/*
 * 函数名称:bodc_MarkAwardBack
 * 功能描述:积分冲正函数函数，本函数用于调用MarkAwardBack函数，本函数在CRM函数库中，不要在本文件中重写
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		char	v_back_accept[22+1];
		int		v_back_ym;
		char	v_login_accept[22+1];
		char	v_login_no[6+1];
		char	v_op_code[4+1];
		char	v_op_time[17+1];
		int		v_total_date;
		char	v_phone_no[15+1];
		char	v_goods_type[4+1];
		char 	v_idgoods_no[20+1];
		long	v_volume;
		float	v_consume_mark;
		float	v_favour_mark;
		char	v_system_note[60+1];
		char	v_op_note[60+1];
		char	v_org_id[10+1];
		char	v_back_flag[1+1];
		int		v_bill_yearmonth;
		
	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_MarkAwardBack(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_back_accept[22+1];
		int		v_back_ym;
		char	v_login_accept[22+1];
		char	v_login_no[6+1];
		char	v_op_code[4+1];
		char	v_op_time[17+1];
		int		v_total_date;
		char	v_phone_no[15+1];
		char	v_goods_type[4+1];
		char 	v_idgoods_no[20+1];
		long	v_volume;
		float	v_consume_mark;
		float	v_favour_mark;
		char	v_system_note[60+1];
		char	v_op_note[60+1];
		char	v_org_id[10+1];
		char	v_back_flag[1+1];
		int		v_bill_yearmonth;

		
		long	lContractNo;
		long	lIdNo;
		char	sSmCode[2+1];
		char	sOrgCode[9+1];
		char	sIpAddr[15+1];
		char    sSqlStr[1000];
		char    vGroupId[10+1];
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	init(vGroupId);
	
	strcpy(v_back_accept,in_para[0]);
	v_back_ym=atoi(in_para[1]);
	strcpy(v_login_accept,in_para[2]);
	strcpy(v_login_no,in_para[3]);
	strcpy(v_op_code,in_para[4]);
	strcpy(v_op_time,in_para[5]);
	v_total_date=atoi(in_para[6]);
	strcpy(v_phone_no,in_para[7]);
	strcpy(v_goods_type,in_para[8]);
	strcpy(v_idgoods_no,in_para[9]);
	v_volume=atol(in_para[10]);
	v_consume_mark=atof(in_para[11]);
	v_favour_mark=atof(in_para[12]);
	strcpy(v_system_note,in_para[13]);
	strcpy(v_op_note,in_para[14]);
	strcpy(v_org_id,in_para[15]);
	strcpy(v_back_flag,in_para[16]);
	v_bill_yearmonth=atoi(in_para[17]);

	v_ret=MarkAwardBack(v_back_accept,v_back_ym,v_login_accept,v_login_no,v_op_code,
			v_op_time,v_total_date,v_ret_code,v_ret_msg);
	if (v_ret!=RET_OK){
		return -600030;			
	}
	
	/***从dcustmsg表里取指定号码的记录***/
    EXEC SQL SELECT id_no,contract_no,sm_code,group_no
    		 INTO   :lIdNo,:lContractNo,:sSmCode,:vGroupId
    		 FROM 	dCustMsg
    		 WHERE 	phone_no = :v_phone_no
    		 AND 	substr(run_code,2,1) < 'a';

	if(SQLCODE != 0)
	{
		return 123401;
	}
	Trim(vGroupId);
	/***从dloginmsg表里取指定工号的记录***/
	EXEC SQL SELECT org_code,ip_address
        	 INTO   :sOrgCode,:sIpAddr
        	 FROM 	dloginmsg
        	 WHERE 	login_no = :v_login_no;

	if(SQLCODE != 0&&SQLCODE!=1403)
	{
		return 123402;
	}
	else if(SQLCODE==1403)
	{
		strcpy(sOrgCode,v_login_no);
		strcpy(sIpAddr,"0.0.0.0");
	}
	
    sprintf(sSqlStr,"insert into wMarkExchangeOpr%d(LOGIN_ACCEPT,CONTRACT_NO,goods_type,idgoods_no,VOLUME,"
                      " CONSUME_MARK,FAVOUR_MARK,ID_NO,SM_CODE,PHONE_NO,ORG_CODE,LOGIN_NO,TOTAL_DATE,"
                      " OP_TIME,SYSTEM_NOTE,OP_NOTE,IP_ADDR,ORG_ID,BACK_FLAG,OP_CODE,GROUP_ID) "
                      " values(to_number(:v1),:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13, "
                     "  to_date(:v14,'YYYYMMDD HH24:MI:SS'),:v15,:v16,:v17,:v18,:v19,:v20,:v21) ", v_bill_yearmonth);
	
    EXEC SQL EXECUTE
    BEGIN
        EXECUTE IMMEDIATE :sSqlStr using :v_login_accept,:lContractNo,:v_goods_type,:v_idgoods_no,:v_volume,
                                           :v_consume_mark,:v_favour_mark,:lIdNo,:sSmCode,:v_phone_no,
                                           :sOrgCode,:v_login_no,:v_total_date,:v_op_time,:v_system_note,
                                           :v_op_note,:sIpAddr,:v_org_id,:v_back_flag,:v_op_code,:vGroupId;

    END;
    END-EXEC;

    if (SQLCODE != 0)
    {
        return 123403;
    }

	return 0;	
}

/*
 * 函数名称:bodc_dealDsmpProd
 * 功能描述:处理DSMP业务绑定函数，本函数用于调用_dealDsmpProd函数，本函数在CRM函数库中，不要在本文件中重写
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

    char v_cmd_code[1+1];
	char v_id_no[14+1];
	char v_phone_no[15+1];
	char v_belong_code[7+1];
	char v_sm_code[2+1];
	char v_mode_code[8+1];
	char v_op_code[4+1];
	char v_op_time[17+1];
	char v_effect_time[17+1];
	int  v_total_date;
	char v_login_no[6+1];
	char v_login_accept[14+1];

		
	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_dealDsmpProd(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
    	char v_cmd_code[1+1];
		TPubBillInfo tPubBillInfo;
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
    strcpy(v_cmd_code     ,	in_para[0]);
	strcpy(tPubBillInfo.IdNo        ,	in_para[1]);
	strcpy(tPubBillInfo.PhoneNo     ,	in_para[2]);
	strcpy(tPubBillInfo.BelongCode  ,	in_para[3]);
	strcpy(tPubBillInfo.SmCode      ,	in_para[4]);
	strcpy(tPubBillInfo.ModeCode    ,	in_para[5]);
	strcpy(tPubBillInfo.OpCode      ,	in_para[6]);
	strcpy(tPubBillInfo.OpTime      ,	in_para[7]);
	strcpy(tPubBillInfo.EffectTime  ,	in_para[8]);
	tPubBillInfo.TotalDate=atoi(in_para[9]);
	strcpy(tPubBillInfo.LoginNo     ,	in_para[10]);
	strcpy(tPubBillInfo.LoginAccept ,	in_para[11]);


	v_ret=_dealDsmpProd(v_cmd_code,&tPubBillInfo,v_ret_code,v_ret_msg);
	if (v_ret!=RET_OK){
		return -600030;			
	}

	return 0;	
}

/*
 * 函数名称:bodc_pubBillEndMode
 * 功能描述:产品取消函数
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		char	v_phone_no[15+1];
	  char	v_old_mode[15+1];
	  long	v_old_accept;
	  char	v_effect_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  long	v_login_accept;
	  char	v_op_time[17+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_pubBillEndMode(char in_para[DLMAXITEMS][DLINTERFACEDATA],char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	  char	v_phone_no[15+1];
	  char	v_old_mode[8+1];
	  char	v_old_accept[14+1];
	  char	v_effect_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  char	v_login_accept[14+1];
	  char	v_op_time[17+1];
	  		
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(v_phone_no,in_para[0]);
	strcpy(v_old_mode,in_para[1]);
	strcpy(v_old_accept,in_para[2]);
	strcpy(v_effect_time,in_para[3]);
	strcpy(v_op_code,in_para[4]);
	strcpy(v_login_no,in_para[5]);
	strcpy(v_login_accept,in_para[6]);
	strcpy(v_op_time,in_para[7]);
	v_ret=pubBillEndMode(v_phone_no,v_old_mode,v_old_accept,v_effect_time,v_op_code,
			v_login_no,v_login_accept,v_op_time,v_ret_msg);
	if(v_ret != 0 )
	{
		strcpy(v_ret_code,"600021");
		sprintf(v_ret_msg,"产品取消错误[%d]!",v_ret);
		return -600021;			
	}

	return 0;	
}


/*
 * 函数名称:bodc_pubBillBeginMode
 * 功能描述:产品订购函数
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		char	v_phone_no[15+1];
	  char	v_new_mode[8+1];
	  char	v_effect_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  char	v_login_accept[14+1];
	  char	v_op_time[17+1];
	  char	v_send_flag[1+1];
	  char	v_flag_str[1+1];
	  char	v_bunch_no[20+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_pubBillBeginMode(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	  char	v_phone_no[15+1];
	  char	v_new_mode[8+1];
	  char	v_effect_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  char	v_login_accept[14+1];
	  char	v_op_time[17+1];
	  char	v_send_flag[1+1];
	  char	v_flag_str[1+1];
	  char	v_bunch_no[20+1];
	  		
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(v_phone_no,in_para[0]);
	strcpy(v_new_mode,in_para[1]);
	strcpy(v_effect_time,in_para[2]);
	strcpy(v_op_code,in_para[3]);
	strcpy(v_login_no,in_para[4]);
	strcpy(v_login_accept,in_para[5]);
	strcpy(v_op_time,in_para[6]);
	strcpy(v_send_flag,in_para[7]);
	strcpy(v_flag_str,in_para[8]);
	strcpy(v_bunch_no,in_para[9]);

	v_ret=pubBillBeginMode(v_phone_no,v_new_mode,v_effect_time,v_op_code,
			v_login_no,v_login_accept,v_op_time,v_send_flag,v_flag_str,v_bunch_no,v_ret_msg);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600022");
		sprintf(v_ret_msg,"产品订购错误[%d]!",v_ret);
		return -600022;			
	}

	return 0;	
}
/*
 * 函数名称:bodc_pubBillBeginTimeMode
 * 功能描述:产品订购函数
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数

		char	v_phone_no[15+1];
	  char	v_new_mode[8+1];
	  char	v_effect_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  char	v_login_accept[14+1];
	  char	v_op_time[17+1];
	  char	v_send_flag[1+1];
	  char	v_flag_str[1+1];
	  char	v_bunch_no[20+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_pubBillBeginTimeMode(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	  char	v_phone_no[15+1];
	  char	v_new_mode[8+1];
	  char	v_effect_time[17+1];
	  char	v_end_time[17+1];
	  char	v_op_code[4+1];
	  char	v_login_no[6+1];
	  char	v_login_accept[14+1];
	  char	v_op_time[17+1];
	  char	v_send_flag[1+1];
	  char	v_flag_str[1+1];
	  char	v_bunch_no[20+1];
	  		
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(v_phone_no,in_para[0]);
	strcpy(v_new_mode,in_para[1]);
	strcpy(v_effect_time,in_para[2]);
	strcpy(v_end_time,in_para[3]);
	strcpy(v_op_code,in_para[4]);
	strcpy(v_login_no,in_para[5]);
	strcpy(v_login_accept,in_para[6]);
	strcpy(v_op_time,in_para[7]);
	strcpy(v_send_flag,in_para[8]);
	strcpy(v_flag_str,in_para[9]);
	strcpy(v_bunch_no,in_para[10]);

	v_ret=pubBillBeginTimeMode(v_phone_no,v_new_mode,v_effect_time,v_end_time,v_op_code,
			v_login_no,v_login_accept,v_op_time,v_send_flag,v_flag_str,v_bunch_no,v_ret_msg);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600023");
		sprintf(v_ret_msg,"产品订购错误[%d]!",v_ret);
		return -600023;			
	}

	return 0;	
}


/*
 * 函数名称:bodc_AddCustAssuer
 * 功能描述:增加担保信息记录  本函数用于替换PublicInsertCheckPrepay函数,本函数在pubFunction.cp中的fAddCustAssuer@CRM改造函数库中，需要从CRM函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
	
		struct sCustAssure *tmpCustAssure结构体中
		char	idNo		[22+1];
		char	opCode		[4+1];
		char	custName	[30+1];
		char	idType		[2+1];
		char	idAddress	[60+1];
		char	idIccid		[20+1];
		char    contactPhone	[20+1];
		char    contactAddress	[100+1];
		char    loginNo		[6+1];
		char    opTime		[20+1];
		char    notes		[120+1];
    
	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_AddCustAssuer(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
        struct sCustAssure tmpCustAssure;
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(tmpCustAssure.idNo     ,	in_para[0]);
    strcpy(tmpCustAssure.opCode     ,	in_para[1]);
    strcpy(tmpCustAssure.custName     ,	in_para[2]);
    strcpy(tmpCustAssure.idType     ,	in_para[3]);
    strcpy(tmpCustAssure.idAddress     ,	in_para[4]);
    strcpy(tmpCustAssure.idIccid     ,	in_para[5]);
    strcpy(tmpCustAssure.contactPhone     ,	in_para[6]);
    strcpy(tmpCustAssure.contactAddress     ,	in_para[7]);
    strcpy(tmpCustAssure.loginNo     ,	in_para[8]);
    strcpy(tmpCustAssure.opTime     ,	in_para[9]);
    strcpy(tmpCustAssure.notes     ,	in_para[10]);

	v_ret=fAddCustAssuer(&tmpCustAssure,v_ret_msg);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600020");
		sprintf(v_ret_msg,"回退有效期错误[%s]!",v_ret_msg);
		return -600010;			
	}
    
	return 0;	
}


/*
 * 函数名称:bodc_MarkDeduct
 * 功能描述:积分冲销扣减 积分兑奖扣减 本函数用于替换MarkDeduct函数,本函数在pubFunction.cp中的fAddCustAssuer@CRM改造函数库中，需要从CRM函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	      	输入参数
	
		long    lContractNo			            积分帐户
     	char    charsDetailCodeIn[10 + 1]		积分二级科目  按科目扣减时不能为NULL
     	char    sChangeType[2 + 1]			    变更类型
     	double  fTotalCount 			        扣减积分/也对应consumeMar
     	long    lLoginAccept      		     	操作流水/loginAccept
     	char    sLoginNo[6 + 1]			        操作工号/loginNo
     	char    sOpCode[4+ 1]				    操作代码opCode
     	char    sOpTime[17+ 1]				    操作时间/opTime
     	int     iSysYearMonth		            系统时间
     	
     	long	loginAccept2					操作流水
    	char    phoneNo[15 + 1]				    手机号码
    	char 	sgoodsType[4+1]		        	实物类别
    	char 	sidgoodsNo[20+1]				资源代码   可为空
    	long	lVolume;				    	兑换数量
    	double	consumeMark						消费积分
    	double	dFavourMark			            优惠积分
    	int		iBillYearMonth         			表年月
    	char	loginNo[6 + 1]					操作工号
    	int     totalDate			           	帐务日期
    	char	opTime[17 + 1]					操作时间
    	char	sSystemNote[60+1]				系统备注
    	char	sOpNote[60+1]					操作备注
    	char	sOrgId[10+1];					工号Ogr_ID
    	char	sBackFlag[1+1]			    	冲正标志
    	char	opCode[4 + 1]					操作代码
    
	char	v_ret_code[6+1]				        返回代码
	char	v_ret_msg[100+1]			        返回信息
	
*/

int bodc_MarkDeduct(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	    long    lContractNo;
     	char    charsDetailCodeIn[10 + 1];
     	char    sChangeType[2 + 1];
     	double  fTotalCount;
     	long    lLoginAccept;
     	char    sLoginNo[6 + 1];
     	char    sOpCode[4+ 1];
     	char    sOpTime[17+ 1];
     	int     iSysYearMonth;
	    
	    long 	loginAccept2;
	    char	sPhoneNo[15+1]; 
	    char 	sgoodsType[4+1]; 
	    char 	sidgoodsNo[20+1]; 
	    long	lVolume;
	    double	consumeMark;
	    double	dFavourMark; 
	    int		iBillYearMonth;
	    char	sLoginNo2[6 + 1];
	    int		iTotalDate;
	    char	opTime2[17 + 1]; 
	    char	sSystemNote[60+1]; 
	    char	sOpNote[60+1]; 
	    char	sOrgId[10+1];
	    char	sBackFlag[1+1];
	    char	opCode2[4 + 1];
    	
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	lContractNo = atol(in_para[0]);
    strcpy(charsDetailCodeIn     ,	in_para[1]);
    strcpy(sChangeType     ,	in_para[2]);
    fTotalCount = atof(in_para[3]);
    lLoginAccept = atol(in_para[4]);
    strcpy(sLoginNo     ,	in_para[5]);
    strcpy(sOpCode     ,	in_para[6]);
    strcpy(sOpTime     ,	in_para[7]);
    iSysYearMonth = atoi(in_para[8]);
    
    loginAccept2 = atol(in_para[4]);
    strcpy(sPhoneNo     ,	in_para[9]);
    strcpy(sgoodsType     ,	in_para[10]);
    strcpy(sidgoodsNo     ,	in_para[11]);
    lVolume  = atol(in_para[12]);
    consumeMark = atof(in_para[3]);
    dFavourMark = atof(in_para[13]);
    iBillYearMonth = atoi(in_para[14]);
    strcpy(sLoginNo2     ,	in_para[5]); 
    iTotalDate = atoi(in_para[15]);
    strcpy(opTime2     ,	in_para[7]);
    strcpy(sSystemNote     ,	in_para[16]);
    strcpy(sOpNote     ,	in_para[17]);
    strcpy(sOrgId     ,	in_para[18]);
    strcpy(sBackFlag     ,	in_para[19]);
    strcpy(opCode2     ,	in_para[6]);
    
	v_ret=MarkDeduct(lContractNo, charsDetailCodeIn, sChangeType, fTotalCount,
	                 lLoginAccept, sLoginNo,sOpCode, sOpTime, iSysYearMonth, v_ret_code, v_ret_msg);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600010");
		sprintf(v_ret_msg,"积分扣减错误[%s]!",v_ret_msg);
		return -600010;			
	}
	v_ret = 0;
	v_ret = PublicMarkOpr(loginAccept2, sPhoneNo, sgoodsType, sidgoodsNo, lVolume, consumeMark, dFavourMark,
	iBillYearMonth, sLoginNo2, iTotalDate, opTime2, sSystemNote, sOpNote, sOrgId, sBackFlag, opCode2);
    if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600020");
		sprintf(v_ret_msg,"插入wMarkExchangeOpr表错误[%d]!",v_ret);
		return -600020;			
	}
    
	return 0;	
}

/*
 * 函数名称:bodc_MarkAward
 * 功能描述:积分冲销扣减，积分兑奖扣减  本函数用于替换MarkAwardReduce函数,本函数在pubFunction.cp中的fAddCustAssuer@CRM改造函数库中，需要从CRM函数库中剥离到本程序中
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	        输入参数
	
    输入：
	char	sContractNo[14 + 1] 		    兑奖帐号
 	couble	fTotalCount 		    		兑奖积分
 	char	sLoginAccept[14 + 1]		    操作流水
 	char	sLoginNo[6 + 1]				    操作工号
 	char	sOpCode[4 + 1]				    操作代码
 	char	sOpTime[17 + ]				    操作时间
 	int		iSysYearMonth		    		当前时间
 	
	long	loginAccept2					操作流水
	char    phoneNo[15 + 1]				    手机号码
	char 	sgoodsType[4+1]		        	实物类别
	char 	sidgoodsNo[20+1]				资源代码   可为空
	long	lVolume;				    	兑换数量
	double	consumeMark						消费积分
	double	dFavourMark			            优惠积分
	int		iBillYearMonth         			表年月
	char	loginNo[6 + 1]					操作工号
	int     totalDate			           	帐务日期
	char	opTime[17 + 1]					操作时间
	char	sSystemNote[60+1]				系统备注
	char	sOpNote[60+1]					操作备注
	char	sOrgId[10+1];					工号Ogr_ID
	char	sBackFlag[1+1]			    	冲正标志
	char	opCode[4 + 1]					操作代码

    返回:
 	pfTotalCountRemain   剩余的兑换积分   >0 表示兑奖时出现透支 （但函数中未进行插入透支操作）
	char	v_ret_code[6+1]				        返回代码
	char	v_ret_msg[100+1]			        返回信息

*/

int bodc_MarkAward(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	    char    lContractNo[14 + 1];
     	double  fTotalCount;
     	char    sLoginAccept[14 + 1];
     	char    sLoginNo[6 + 1];
     	char    sOpCode[4+ 1];
     	char    sOpTime[17+ 1];
     	int     iSysYearMonth;
     	
	    long 	sLoginAccept2;
	    char	sPhoneNo[15+1]; 
	    char 	sgoodsType[4+1]; 
	    char 	sidgoodsNo[20+1]; 
	    long	lVolume;
	    double	consumeMark;
	    double	dFavourMark; 
	    int		iBillYearMonth;
	    char	sLoginNo2[6 + 1];
	    int		iTotalDate;
	    char	opTime2[17 + 1]; 
	    char	sSystemNote[60+1]; 
	    char	sOpNote[60+1]; 
	    char	sOrgId[10+1];
	    char	sBackFlag[1+1];
	    char	opCode2[4 + 1];
    	
	EXEC SQL END DECLARE SECTION;
	int		v_ret=0;
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(lContractNo	,	in_para[0]);
    fTotalCount = atof(in_para[1]);;
    strcpy(sLoginAccept     ,	in_para[2]);
    strcpy(sLoginNo     ,	in_para[3]);
    strcpy(sOpCode     ,	in_para[4]);
    strcpy(sOpTime     ,	in_para[5]);
    iSysYearMonth = atoi(in_para[6]);
    
    sLoginAccept2 = atol(in_para[2]);
    strcpy(sPhoneNo     ,	in_para[7]);
    strcpy(sgoodsType     ,	in_para[8]);
    strcpy(sidgoodsNo     ,	in_para[9]);
    lVolume  = atol(in_para[10]);
    consumeMark = atof(in_para[1]);
    dFavourMark = atof(in_para[11]);
    iBillYearMonth = atoi(in_para[12]);
    strcpy(sLoginNo2     ,	in_para[3]); 
    iTotalDate = atoi(in_para[13]);
    strcpy(opTime2     ,	in_para[5]);
    strcpy(sSystemNote     ,	in_para[14]);
    strcpy(sOpNote     ,	in_para[15]);
    strcpy(sOrgId     ,	in_para[16]);
    strcpy(sBackFlag     ,	in_para[17]);
    strcpy(opCode2     ,	in_para[4]);
    
	v_ret=MarkAward(lContractNo, fTotalCount, sLoginAccept, sLoginNo,
	                 sOpCode, sOpTime, iSysYearMonth, v_ret_code, v_ret_msg);
	if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600010");
		sprintf(v_ret_msg,"积分扣减错误[%s]!",v_ret_msg);
		return -600010;			
	}
	v_ret = 0;
	v_ret = PublicMarkOpr(sLoginAccept2, sPhoneNo, sgoodsType, sidgoodsNo, lVolume, consumeMark, dFavourMark,
	iBillYearMonth, sLoginNo2, iTotalDate, opTime2, sSystemNote, sOpNote, sOrgId, sBackFlag, opCode2);
    if (v_ret!=RET_OK){
		strcpy(v_ret_code,"600020");
		sprintf(v_ret_msg,"插入wMarkExchangeOpr表错误[%d]!",v_ret);
		return -600020;			
	}
    
	return 0;	
}

/*
 * 函数名称:bodc_dConMsgUpdate
 * 功能描述:服务开通函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_flag[17+1];运算标识，1表示＋，0表示 -
		char	v_op_note[100+1];
		double v_pay_money=0;	
		char	v_id_str[22+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_dConMsgUpdate(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_note[100+1];
		char  v_pay_money[19+1];	
		char	v_id_str[22+1];
    
    char	v_flag[1+1];
	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int 	v_ret=0;
	
	init(v_parameter_array);	
	init(v_ret_code);				init(v_ret_msg);

	strcpy(v_id_str,in_para[0]);/*contract_no*/
	strcpy(v_op_login,in_para[1]);
	strcpy(v_op_code,in_para[2]);
	v_op_accept=atol(in_para[3]);
	strcpy(v_flag,in_para[4]);
	strcpy(v_op_note,in_para[5]);
	strcpy(v_pay_money,in_para[6]);

	strcpy(v_parameter_array[0],v_pay_money);
	strcpy(v_parameter_array[1],v_id_str);
	/*更新账户信息表*/

  if (memcmp(v_flag,"1",1) == 0)   

	{
		v_ret=OrderUpdateConMsg(ORDERIDTYPE_CNTR,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
				v_id_str," deposit=deposit+round(to_number(:v1),2) ",
				"",v_parameter_array);
	}
	else
	{
		v_ret=OrderUpdateConMsg(ORDERIDTYPE_CNTR,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
				v_id_str," deposit=deposit-round(to_number(:v1),2) ",
				"",v_parameter_array);
	}
	if (v_ret<0)
	{
		strcpy(v_ret_code,"600000");
		sprintf(v_ret_msg,"更新账户信息错误[%d]!",v_ret);
		return -600025;		
	}
		
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
				
	return 0;
}


/*
 * 函数名称:bodc_SQSP_APPLY_RECUpdate
 * 功能描述:服务开通函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_flag[17+1];运算标识，1表示＋，0表示 -
		char	v_op_note[100+1];
		double v_pay_money=0;	
		char	v_id_str[22+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_SQSP_APPLY_RECUpdate(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_note[100+1];
		double v_pay_money=0;	
		char	v_id_str[22+1];
		char	v_apply_no[22+1];
		char	v_phone_no[15+1];
		char	v_org_code[9+1];
    char sTempSqlStr[1024];
    int		icount=0;
	
	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int 	v_ret=0;
	
	init(v_parameter_array);	init(sTempSqlStr);
	init(v_ret_code);				init(v_ret_msg);
	
	strcpy(v_id_str,in_para[0]);/*phone_no*/                       
  strcpy(v_op_login,in_para[1]);                
  strcpy(v_op_code,in_para[2]);                 
  v_op_accept=atol(in_para[3]);                 
  strcpy(v_op_note,in_para[4]);                 
  strcpy(v_phone_no,in_para[5]);                
  strcpy(v_org_code,in_para[6]);                

	
	sprintf(sTempSqlStr,"select APPLY_NO from SQSP_APPLY_REC where phone_no=:v1 and func_code=:v2"
	                    "and process_status='3' and org_code=substr(:v3,1,7) ");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
    	EXEC SQL declare ngcur01 cursor for sql_str;
    	EXEC SQL open ngcur01 using :v_phone_no,:v_op_code,:v_org_code;
  for(;;)
  {
  	init(v_apply_no);
  	EXEC SQL FETCH ngcur01 into :v_apply_no;
  	if((0!=SQLCODE)&&(1403!=SQLCODE))
  	{
  		EXEC SQL CLOSE ngcur01;
  		PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-4","错误提示未定义phoneNo[%s]",v_phone_no);
			return -16;
  	}
  	if(1403==SQLCODE) break;
  	
  	Trim(v_apply_no);
  		
  	strcpy(v_parameter_array[0],v_apply_no);
		strcpy(v_parameter_array[1],v_phone_no);
		strcpy(v_parameter_array[2],v_op_code);
		strcpy(v_parameter_array[3],v_org_code);
  	
		/*更新SQSP_APPLY_REC表*/
  	
			v_ret=OrderUpdateSQSP_APPLY_REC(ORDERIDTYPE_PHONE,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
					v_apply_no," process_status='4' ",
					" AND phone_no=:v1 and func_code=:v2 and process_status='3' and org_code=substr(:v3,1,7) ",v_parameter_array);
		if (v_ret<0)
		{
			EXEC SQL CLOSE ngcur01;   
			strcpy(v_ret_code,"600000");
			sprintf(v_ret_msg,"更新SQSP_APPLY_REC信息错误[%d]!",v_ret);
			return -600026;		
		}
		printf("[%d][%d]\n",v_ret,SQLCODE);
	}
	EXEC SQL CLOSE ngcur01;       
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
				
	return 0;
}


/*
 * 函数名称:bodc_wLocalBlackListDelete
 * 功能描述:服务开通函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_note[100+1];
		char	v_id_str[22+1];
		char	v_black_no[20+1];
		char	v_black_type[1+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_wLocalBlackListDelete(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_op_note[100+1];
		char	v_id_str[22+1];
		char	v_black_no[20+1];
		char	v_black_type[1+1];
		TwLocalBlackListHis	sTwLocalBlackListHis;				/*ng_解耦_数据工单 add by mengfy@10/03/2009*/

	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int 	v_ret=0;
	TwLocalBlackList oldIndex;
	
	init(v_parameter_array);	
	init(v_ret_code);				init(v_ret_msg);
  memset(&oldIndex,0,sizeof(oldIndex));  
    strcpy(v_id_str,in_para[0]);/*操作员*/
	strcpy(v_op_login,in_para[1]);
	strcpy(v_op_code,in_para[2]);
	v_op_accept=atol(in_para[3]);
	strcpy(v_op_note,in_para[4]);
	strcpy(v_black_no,in_para[5]);
	strcpy(v_black_type,in_para[6]);
	/*ng_解耦_数据工单 add by mengfy@10/03/2009
	 **EXEC SQL insert into wLocalBlackListHIs(
     **       BLACK_NO,BLACK_TYPE,BELONG_CODE,OP_TIME,ORG_CODE,LOGIN_NO,OP_CODE,OP_NOTE,login_accept,GROUP_ID,ORG_ID)
     **       select BLACK_NO,BLACK_TYPE,BELONG_CODE,OP_TIME,ORG_CODE,LOGIN_NO,OP_CODE,OP_NOTE,:v_op_accept,group_id,org_id
     **       from wLocalBlackList where black_no=:v_black_no and black_type=:v_black_type and trim(black_no) is not  null;
     **       if(SQLCODE<0)
     **       {    
     **   		printf("Fail---取交费查询记录出错[%d][%s]\n", SQLCODE,SQLERRMSG);
     **   		}
*/
	EXEC SQL select  BLACK_NO,BLACK_TYPE,BELONG_CODE,OP_TIME,ORG_CODE,LOGIN_NO,OP_CODE,OP_NOTE,:v_op_accept,group_id,org_id
			 into :sTwLocalBlackListHis.sBlackNo, :sTwLocalBlackListHis.sBlackType, :sTwLocalBlackListHis.sBelongCode,
			 	  :sTwLocalBlackListHis.sOpTime, :sTwLocalBlackListHis.sOrgCode, :sTwLocalBlackListHis.sLoginNo,
			 	  :sTwLocalBlackListHis.sOpCode, :sTwLocalBlackListHis.sOpNote, :sTwLocalBlackListHis.sLoginAccept,
				  :sTwLocalBlackListHis.sGroupId, :sTwLocalBlackListHis.sOrgId
            from wLocalBlackList where black_no=:v_black_no and black_type=:v_black_type and trim(black_no) is not  null;       		
	v_ret = 0;
	v_ret = OrderInsertLocalBlackListHis(ORDERIDTYPE_OPER,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
			sTwLocalBlackListHis);
    if(v_ret < 0)
    {    
		printf("Fail---取交费查询记录出错[%d][%s]\n", SQLCODE,SQLERRMSG);
	}
	/*ng_解耦_数据工单 add by mengfy@10/03/2009*/


  strcpy(oldIndex.sBlackNo , v_black_no);                
  strcpy(oldIndex.sBlackType , v_black_type);                        
    
	strcpy(v_parameter_array[0],v_black_no);
	strcpy(v_parameter_array[1],v_black_type);

	/*更新wLocalBlackList表*/
	v_ret = 0;
        	  
		v_ret=OrderDeleteLocalBlackList(ORDERIDTYPE_OPER,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,
				oldIndex,"",v_parameter_array);
	if (v_ret<0)
	{
		strcpy(v_ret_code,"600000");
		sprintf(v_ret_msg,"删除wLocalBlackList信息错误[%d]!",v_ret);
		return -600027;		
	}
	printf("[%d][%d]\n",v_ret,SQLCODE);        
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
				
	return 0;
}


/*
 * 函数名称:bodc_wLocalBlackListInsert
 * 功能描述:回退黑名单函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		char	v_op_login[6+1];
		char	v_op_code[5+1];
		long	v_op_accept;
		char	v_flag[17+1];运算标识，1表示＋，0表示 -
		char	v_op_note[100+1];
		double v_pay_money=0;	
		char	v_id_str[22+1];

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/
int bodc_wLocalBlackListInsert(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_op_login[6+1];
		char	v_op_code[4+1];
		long	v_op_accept;
		char	v_op_note[100+1];
		char	v_id_str[22+1];
		char	v_black_no[20+1];
		char	v_black_type[1+1];
		char	v_belong_code[7+1];
		char	v_op_time[17+1];
		char	v_org_code[9+1];
		char	v_group_id[10+1];
		char	v_org_id[10+1];
		char  v_pay_accept[14+1];
		char  sTempSqlStr[1024];
    int		icount=0;
	
	EXEC SQL END DECLARE SECTION;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int 	v_ret=0;
	TwLocalBlackList	sTwLocalBlackList;
	
	init(v_parameter_array);	init(sTempSqlStr);
	init(v_ret_code);				init(v_ret_msg);

	strcpy(v_id_str,in_para[0]);/*操作员*/
	strcpy(v_op_login,in_para[1]);
	strcpy(v_op_code,in_para[2]);
	v_op_accept=atol(in_para[3]);
	strcpy(v_op_note,in_para[4]);
	strcpy(v_pay_accept,in_para[5]);

	sprintf(sTempSqlStr,"select BLACK_NO,BLACK_TYPE,BELONG_CODE,OP_TIME,ORG_CODE,LOGIN_NO,OP_CODE,OP_NOTE,GROUP_ID,ORG_ID "
  										 "from wLocalBlackLIstHis where login_accept=:v1 ");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
  EXEC SQL declare ngcur09 cursor for sql_str;
  EXEC SQL open ngcur09 using :v_pay_accept;
  for(;;)
  {
  	EXEC SQL FETCH ngcur09 into :v_black_no ,:v_black_type,:v_belong_code,:v_op_time,:v_org_code,:v_op_login,:v_op_code,:v_op_note,:v_group_id,:v_org_id;
  	if((0!=SQLCODE)&&(1403!=SQLCODE))
  	{
  		EXEC SQL CLOSE ngcur09;
  		PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-4","错误提示未定义v_op_login[%s]",v_op_login);
			return -16;
  	}
  	if(1403==SQLCODE) break;
  	
  	strcpy(sTwLocalBlackList.sBlackNo,v_black_no);
		strcpy(sTwLocalBlackList.sBlackType,v_black_type);       
		strcpy(sTwLocalBlackList.sBelongCode,v_belong_code);        
		strcpy(sTwLocalBlackList.sOpTime,v_op_time);                 
		strcpy(sTwLocalBlackList.sOrgCode,v_org_code);             
		strcpy(sTwLocalBlackList.sLoginNo,v_op_login);           
    strcpy(sTwLocalBlackList.sOpCode,v_op_code);     
  	strcpy(sTwLocalBlackList.sOpNote,v_op_note);   
  	strcpy(sTwLocalBlackList.sOpCode,v_group_id);     
  	strcpy(sTwLocalBlackList.sOpNote,v_org_id);  
  	
		/*更新wLocalBlackList表*/
  	
		v_ret=OrderInsertLocalBlackList(ORDERIDTYPE_OPER,v_id_str,100,v_op_code,v_op_accept,v_op_login,v_op_note,sTwLocalBlackList);
		if (v_ret<0)
		{
			EXEC SQL CLOSE ngcur09;   
			strcpy(v_ret_code,"600000");
			sprintf(v_ret_msg,"更新wLocalBlackList信息错误[%d]!",v_ret);
			return -600026;		
		}
		printf("[%d][%d]\n",v_ret,SQLCODE);
	}
	EXEC SQL CLOSE ngcur09;       
		
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
				
	return 0;
}


/*
 * 函数名称:bodc_dGrpPostOperation
 * 功能描述:对dGrpPost表进行增删改操作 edit by huangtao at 28/07/2009
 * iOpType = 1  add
 * iOpType = 2  update
 * iOpType = 2  delete
 * 输入参数:
	char 	in_para[DLMAXITEMS][DLINTERFACEDATA]	输入参数
		char	v_id_str[22 + 1];
		char	v_op_code[4 + 1];
		long	v_op_accept = 0;
		char	v_op_login[6 + 1];
		char	v_op_note[60 + 1];
		char	iOpType[9 + 1];
		sTdGrpPost.sCustId;
		sTdGrpPost.sPostFlag;
		sTdGrpPost.sPostType;
		sTdGrpPost.sPostAddress;
		sTdGrpPost.sPostZip;
		sTdGrpPost.sFaxNo;
		sTdGrpPost.sMailAddress;
		sTdGrpPost.sContactTel;
		sTdGrpPost.sPostName;
		sTdGrpPost.sPostnoFlag;
		sTdGrpPost.sOpTime;

	char	v_ret_code[6+1]				返回代码
	char	v_ret_msg[100+1]			返回信息

*/ 
int bodc_dGrpPostOperation(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	iOpType[9 + 1];
		char	v_id_str[22+1];
		char	v_op_code[4 + 1];
		long	v_op_accept = 0;
		char	v_op_login[6 + 1];
		char	v_op_note[60 + 1];
		char	v_CustId[10 + 1];
		char	vPostType[1 + 1];
		TdGrpPost	sTdGrpPost;
	EXEC SQL END DECLARE SECTION;
	int  	vCount = 0;
	int		v_ret = 0,k=0;
	TdGrpPostIndex oldIndex;
	TdGrpPostIndex newIndex;
	char	v_update_sql[1000 + 1];
	char	v_where_sql[500 + 1];
	char	sTempSqlStr[1000 + 1];
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	
	
	init(v_id_str);					init(v_op_login);
	init(v_op_note);				init(iOpType);
	memset(&oldIndex,0,sizeof(TdGrpPostIndex));
	memset(&newIndex,0,sizeof(TdGrpPostIndex));
	init(v_update_sql);				init(v_where_sql);
	memset(&sTdGrpPost,0,sizeof(TdGrpPost));
	init(sTempSqlStr);				init(v_CustId);
	init(v_parameter_array);		init(vPostType);
	
	/*入参打印*/				
	for (k = 0;k<17;k++)
	{
		printf("in_para:[%d]=[%s]\n",k,in_para[k]);
	}
	strcpy(v_id_str,in_para[0]);/*null*/
	strcpy(v_op_code,in_para[1]);
	v_op_accept=atol(in_para[2]);
	strcpy(v_op_login,in_para[3]);
	strcpy(v_op_note,in_para[4]);
	
	strcpy(iOpType, in_para[5]);
	strcpy(sTdGrpPost.sCustId, in_para[6]);
	strcpy(v_CustId, in_para[6]);
	strcpy(sTdGrpPost.sPostFlag, in_para[7]);
	strcpy(sTdGrpPost.sPostType, in_para[8]);
	strcpy(sTdGrpPost.sPostAddress, in_para[9]);
	strcpy(sTdGrpPost.sPostZip, in_para[10]);
	strcpy(sTdGrpPost.sFaxNo, in_para[11]);
	strcpy(sTdGrpPost.sMailAddress, in_para[12]);
	strcpy(sTdGrpPost.sContactTel, in_para[13]);
	strcpy(sTdGrpPost.sPostName, in_para[14]);
	strcpy(sTdGrpPost.sPostnoFlag, in_para[15]);
	strcpy(sTdGrpPost.sOpTime, in_para[16]);
	
	Trim(v_CustId);
	Trim(iOpType);
	

	if(atoi(iOpType) == 1)
	{
		EXEC SQL SELECT count(*)
           INTO :vCount
           FROM	dgrppost 
			     WHERE cust_id=to_number(:v_CustId);

		if(vCount==0)
		{
			printf("插入dgrppost表!\n");
			v_ret = OrderInsertGrpPost(ORDERIDTYPE_CUST, v_id_str, 100, v_op_code,
										v_op_accept, v_op_login, v_op_note, 
										sTdGrpPost);
			PUBLOG_DBDEBUG("bodc_dGrpPostOperation");
			if(v_ret != 0)
			{
				strcpy(v_ret_code,"600027");
				sprintf(v_ret_msg,"插入表GrpPost错误[%d]!",v_ret);
				return -600027;				
			}
		  	printf("插入dgrpposthis表!\n");
			EXEC SQL insert into dgrpposthis(CUST_ID,POST_FLAG,POST_TYPE,POST_ADDRESS, 
                                  POST_ZIP,FAX_NO,MAIL_ADDRESS,CONTACT_TEL,   
                                  POST_NAME,POSTNO_FLAG,OP_TIME,UPDATE_LOGINNO,
                                  UPDATE_TIME,UPDATE_OPCODE,UPDATE_TYPE)
				values(to_number(:sTdGrpPost.sCustId),'0','0',:sTdGrpPost.sPostAddress,
				     :sTdGrpPost.sPostZip,:sTdGrpPost.sFaxNo,:sTdGrpPost.sMailAddress,:sTdGrpPost.sContactTel,
				     :sTdGrpPost.sPostName,0,to_date(:sTdGrpPost.sOpTime,'yyyymmdd hh24:mi:ss'),:v_op_login,
				     to_date(:sTdGrpPost.sOpTime,'yyyymmdd hh24:mi:ss'),:v_op_code,'A');
		  	if(SQLCODE!=0)
			{
				strcpy(v_ret_code,"600029");
				strcpy(v_ret_msg,"插入dgrpposthis表失败!");
			  	return -600029;	
			}
	 	}
	 	else if(vCount<0)
	    {
	  		strcpy(v_ret_code,"600031");
			strcpy(v_ret_msg,"获取信息失败!");
			return -600031;	
	    }
		else
		{
	  		strcpy(v_ret_code,"600033");
			strcpy(v_ret_msg,"该客户已申请帐单邮寄!");
			return -600033;	
		}		
	}
	else if(atoi(iOpType) == 2)
	{
		
		EXEC SQL SELECT count(*)
		  INTO :vCount
	    FROM	dgrppost 
			WHERE cust_id=to_number(:v_CustId);
		if(vCount<0)
	  	{
	  		strcpy(v_ret_code,"600035");
			strcpy(v_ret_msg,"获取信息失败!");
			return -600035;	
	  	}
	  	else if(vCount==0)
	  	{
	  		strcpy(v_ret_code,"600036");
			strcpy(v_ret_msg,"该客户尚未申请帐单邮寄!");
			return -600036;	
	  	}
	  	else
	  	{
	  		printf("插入dgrpposthis表!\n");
			EXEC SQL insert into dgrpposthis(CUST_ID,POST_FLAG,POST_TYPE,POST_ADDRESS, 
                                  POST_ZIP,FAX_NO,MAIL_ADDRESS,CONTACT_TEL,   
                                  POST_NAME,POSTNO_FLAG,OP_TIME,UPDATE_LOGINNO,
                                  UPDATE_TIME,UPDATE_OPCODE,UPDATE_TYPE)
			     select CUST_ID,POST_FLAG,POST_TYPE,POST_ADDRESS, 
                                  POST_ZIP,FAX_NO,MAIL_ADDRESS,CONTACT_TEL,   
                                  POST_NAME,POSTNO_FLAG,OP_TIME,:v_op_login,
				                  to_date(:sTdGrpPost.sOpTime,'yyyymmdd hh24:mi:ss'),:v_op_code,'U'
	                       from dgrppost
	                       where cust_id=to_number(:v_CustId);
	  	if(SQLCODE!=0)
	  	{				
	  		strcpy(v_ret_code,"600038");				
	  		strcpy(v_ret_msg,"插入dgrpposthis表失败!");
	  		return -600038;	
	  	}
		printf("更新dgrppost表!\n");
		init(sTempSqlStr);
		sprintf(sTempSqlStr,"select post_type from dGrpPost where cust_id = to_number(:v1)");
			EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
        	EXEC SQL declare ngcur03 cursor for sql_str;
        	EXEC SQL open ngcur03 using :v_CustId;
        	for(;;)
        	{
        		init(vPostType);
        		EXEC SQL FETCH ngcur03 into :vPostType;
        		if((0!=SQLCODE)&&(1403!=SQLCODE))
        		{
        			PUBLOG_DBDEBUG("bodc_dGrpPostOperation");
        			pubLog_DBErr(_FFL_,"bodc_dGrpPostOperation","","游标取数据错误！CustId:[%s]",v_CustId);
        			EXEC SQL CLOSE ngcur03;
					return -600039;	
        		};
        		if(1403==SQLCODE) break;
        		
        		Trim(vPostType);
        		
        		memset(&oldIndex,0,sizeof(TdGrpPostIndex));
				memset(&newIndex,0,sizeof(TdGrpPostIndex));
				init(v_parameter_array);
				init(v_update_sql);
				init(v_where_sql);
				
				strcpy(oldIndex.sCustId,v_CustId);
				strcpy(oldIndex.sPostType,vPostType);
				strcpy(newIndex.sCustId,v_CustId);
				strcpy(newIndex.sPostType,vPostType);
				
				strcpy(v_update_sql," POST_FLAG =:v1,"
		               	   "POST_TYPE =:v2,"
		               	   "POST_ADDRESS =:v3,"
		               	   "POST_ZIP =:v4,"
		                   "FAX_NO =:v5,"
		                   "MAIL_ADDRESS =:v6,"
		                   "CONTACT_TEL =:v7,"
		                   "POST_NAME =:v8,"
		                   "POSTNO_FLAG=to_number(:v9)");
		        strcpy(v_where_sql,"");
		        strcpy(v_parameter_array[0],sTdGrpPost.sPostFlag);
				strcpy(v_parameter_array[1],vPostType);
				strcpy(v_parameter_array[2],sTdGrpPost.sPostAddress);
				strcpy(v_parameter_array[3],sTdGrpPost.sPostZip);
				strcpy(v_parameter_array[4],sTdGrpPost.sFaxNo);
				strcpy(v_parameter_array[5],sTdGrpPost.sMailAddress);
				strcpy(v_parameter_array[6],sTdGrpPost.sContactTel);
				strcpy(v_parameter_array[7],sTdGrpPost.sPostName);
				strcpy(v_parameter_array[8],sTdGrpPost.sPostnoFlag);
				strcpy(v_parameter_array[9],v_CustId);
				strcpy(v_parameter_array[10],vPostType);

				v_ret = 0;
				v_ret = OrderUpdateGrpPost(ORDERIDTYPE_CUST, v_id_str, 100, v_op_code,
									v_op_accept, v_op_login, v_op_note, 
									oldIndex,newIndex,v_update_sql,
									v_where_sql,v_parameter_array);
				printf("bodc_dGrpPostOperation ,ret=[%d]\n",v_ret);
				if(0 != v_ret)
				{
					PUBLOG_DBDEBUG("bodc_dGrpPostOperation");
					pubLog_DBErr(_FFL_,"","-4","更新dgrppost表失败！CustId:[%s]",v_CustId);
					EXEC SQL CLOSE ngcur03;
					strcpy(v_ret_code,"600044");
					strcpy(v_ret_msg,"更新dgrppost表失败!");
					return -600044;	
				}
        	}
        	EXEC SQL CLOSE ngcur03;
		}
	}
	else if(atoi(iOpType) == 3)
	{ 
		EXEC SQL SELECT count(*)
           INTO :vCount
           FROM	dgrppost 
			     WHERE cust_id=to_number(:v_CustId);
		if(vCount<0)
	    {
	  		strcpy(v_ret_code,"600045");
			strcpy(v_ret_msg,"获取信息失败!");
			return -600045;	
	    }
	    else if(vCount==0)
	    {
	  		strcpy(v_ret_code,"600046");
			strcpy(v_ret_msg,"该客户尚未申请帐单邮寄!");
			return -600046;	
	    }
	    else
	    {
			printf("插入dgrpposthis表!\n");
	  		EXEC SQL insert into dgrpposthis(CUST_ID,POST_FLAG,POST_TYPE,POST_ADDRESS, 
                                  POST_ZIP,FAX_NO,MAIL_ADDRESS,CONTACT_TEL,   
                                  POST_NAME,POSTNO_FLAG,OP_TIME,UPDATE_LOGINNO,
                                  UPDATE_TIME,UPDATE_OPCODE,UPDATE_TYPE)
			     select CUST_ID,POST_FLAG,POST_TYPE,POST_ADDRESS, 
                                  POST_ZIP,FAX_NO,MAIL_ADDRESS,CONTACT_TEL,   
                                  POST_NAME,POSTNO_FLAG,OP_TIME,:v_op_login,
				                  to_date(:sTdGrpPost.sOpTime,'yyyymmdd hh24:mi:ss'),:v_op_code,'D'
	                       from dgrppost
	                       where cust_id=to_number(:v_CustId);
	  		if(SQLCODE!=0)
			{
				strcpy(v_ret_code,"600047");
				strcpy(v_ret_msg,"插入dgrpposthis表失败!");
				return -600047;	
			}	
		    printf("删除dgrppost表中记录!\n");
		    init(sTempSqlStr);
		    sprintf(sTempSqlStr,"select post_type from dGrpPost where cust_id = to_number(:v1)");
			EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
        	EXEC SQL declare ngcur02 cursor for sql_str;
        	EXEC SQL open ngcur02 using :v_CustId;
        	for(;;)
        	{
        		init(vPostType);
        		EXEC SQL FETCH ngcur02 into :vPostType;
        		if((0!=SQLCODE)&&(1403!=SQLCODE))
        		{
        			PUBLOG_DBDEBUG("bodc_dGrpPostOperation");
        			pubLog_DBErr(_FFL_,"","-4","游标取数据错误！CustId:[%s]",v_CustId);
        			EXEC SQL CLOSE ngcur02;
					return -600049;	
        		};
        		if(1403==SQLCODE) break;
        		
        		Trim(vPostType);
        		
        		memset(&oldIndex,0,sizeof(TdGrpPostIndex));
				init(v_parameter_array);
				init(v_where_sql);
				
				strcpy(oldIndex.sCustId,v_CustId);
				strcpy(oldIndex.sPostType,vPostType);
				
		        strcpy(v_where_sql,"");
		        strcpy(v_parameter_array[0],v_CustId);
				strcpy(v_parameter_array[1],vPostType);
				v_ret = 0;
				v_ret = OrderDeleteGrpPost(ORDERIDTYPE_CUST, v_id_str, 100,
							v_op_code,v_op_accept,v_op_login,v_op_note,
							oldIndex,
							v_where_sql,v_parameter_array
						   );
				printf("bodc_dGrpPostOperation ,ret=[%d]\n",v_ret);
				if(v_ret != 0)
				{
					strcpy(v_ret_code,"600050");
					strcpy(v_ret_msg,"删除dgrppost表中记录失败!");
					return -600050;	
				}
        	}
        	EXEC SQL CLOSE ngcur02;
		}
	}
	else 
	{
		strcpy(v_ret_code,"600055");
		strcpy(v_ret_msg,"输入操作类型有误!");
		return -600055;	
	}
	strcpy(v_ret_code,"000000");
	strcpy(v_ret_msg,"处理成功!");
	
	return 0;
}




/*#######################################################################*/
/*更新支票信息*/
int PublicInsertCheckPrepay_crm(int InTotalDate, long InPayAccept, char *InOpCode,
        char *InWorkNo, char *InOrgCode, char *InBankCode, char *InCheckNo,
        double InPayMoney, char *InPayNote)
{
        EXEC SQL BEGIN DECLARE SECTION;
                char TmpSqlStr[TMPLEN+1];
				int  TotalDate;
				long PayAccept;
				char OpCode[4+1];
				char WorkNo[6+1];
			    char OrgCode[9+1]; 	
				char BankCode[5+1];
				char CheckNo[20+1];
				double PayMoney=0.00;
				char PayNote[60+1];
				
				char OrgId[10+1]; 
				int  ret=0;
        EXEC SQL   END DECLARE SECTION;
				TotalDate=InTotalDate;
				PayAccept=InPayAccept;
				strcpy(OpCode,InOpCode);
				strcpy(WorkNo,InWorkNo);
				strcpy(OrgCode,InOrgCode);
				strcpy(BankCode,InBankCode);
				strcpy(CheckNo,InCheckNo);
				PayMoney=InPayMoney;
				strcpy(PayNote,InPayNote);
        
		int  RetValue=0;
		/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
		memset(OrgId, 0, sizeof(OrgId));
		ret = sGetLoginOrgid(WorkNo,OrgId);
		if(ret <0)
		{
			printf("获取工号org_id失败![%s]\n",WorkNo);
			return -1;
		}
		Trim(OrgId);
					        
		EXEC SQL insert into wCheckOpr(TOTAL_DATE,LOGIN_ACCEPT,ORG_CODE,LOGIN_NO,OP_CODE,BANK_CODE,CHECK_NO,CHECK_PAY,OP_TIME,OP_NOTE,ORG_ID) 
		values(:TotalDate,:PayAccept,:OrgCode,:WorkNo,:OpCode,:BankCode,:CheckNo,:PayMoney,sysdate,:PayNote,:OrgId);
        if (SQLCODE != SQL_OK) {
				printf("\n[%s][%d] 插入支票表出错",SQLERRMSG,SQLCODE);
                return -1;
        }
        /*---组织机构改造插入表字段edit by zhangxha at 04/03/2009---end*/ 
		EXEC SQL update dCheckPrepay set check_prepay=round(check_prepay-:PayMoney,2),op_time=sysdate
		where bank_code=:BankCode
		and   check_no=:CheckNo
		and   check_prepay>=:PayMoney;
        if (SQLCODE != SQL_OK) {
				printf("\n[%s][%d] 更新支票表出错",SQLERRMSG,SQLCODE);
                return -1;
        }
        return RET_OK;
}

/*********************************************
有效期回退
*********************************************/
int PublicUndoExpireTime_crm( long InIdNo, int InPayDate,long InPayAccept,
                          int InTotalDate, long InLoginAccept,
                          char *InOpCode, char *InLoginNo,
                          char *InOrgCode, char *InOpTime, char *InOpNote)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+1];
		char TimeCode[2+1];
		int  AddedDays = 0;
		int  fAddedDays = 0;
		long IdNo = 0;
		int  PayDate = 0;
		long PayAccept = 0;
		int  TotalDate = 0;
		long LoginAccept = 0;
		char OpCode[4+1];
		char LoginNo[6+1];
		char OrgCode[9+1];
		char OpTime[19+1];
		char OpNote[60+1];
		int  ret=0;
		char OrgId[10+1];
		char sIdNo[14+1];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		int  v_ret=0;
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(TmpSqlStr);
	Sinitn(TimeCode);
	init(OpCode);
	init(LoginNo);
	init(OrgCode);
	init(OpTime);
	init(OpNote);
	init(OrgId);
	init(sIdNo);
	IdNo = InIdNo;
	PayDate = InPayDate;
	PayAccept = InPayAccept;
	TotalDate = InTotalDate;
	LoginAccept = InLoginAccept;
	strcpy(OpCode,InOpCode);
	strcpy(LoginNo,InLoginNo);
	strcpy(OrgCode,InOrgCode);
	strcpy(OpTime,InOpTime);
	strcpy(OpNote,InOpNote);
	sprintf(sIdNo, "%ld",IdNo);
	Trim(sIdNo);
	
	sprintf(TmpSqlStr,"SELECT nvl(days,0) ,nvl(time_code,' ') \
	                   FROM   wExpireList \
	                   WHERE  id_no=:v1 \
	                   AND    total_date=:v2 \
	                   AND    login_accept=:v3");

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :AddedDays,:TimeCode using :IdNo,:PayDate,:PayAccept;
	END;
	END-EXEC;
	printf("zhangjb----------1[%d]\n",SQLCODE);
	if (SQLCODE==NOTFOUND )
	{
		return 0;
	}
	else if(SQLCODE!=0)
	{
		printf("1:[%s][%d][%ld[%d][%ld]\n",TmpSqlStr,SQLCODE,IdNo,PayDate,PayAccept);
		return -1;
	}
	
	printf("zhangjb----------2[%ld]\n",IdNo);
	fAddedDays = (-1)*AddedDays;
	/*ng_解耦_数据工单 add by mengfy@10/03/2009
	**sprintf(TmpSqlStr,"UPDATE dCustExpire \
	**                   SET expire_time=expire_time+:v1 \
	**                   WHERE id_no=:v2");
	**
	**EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	**EXEC SQL EXECUTE sql_stmt using :fAddedDays,:IdNo;
    **
	**if ( SQLCODE < 0) 
	**{
	**	printf("UndoExpirTime:%s\n",TmpSqlStr);
	**	return -1;
	**}
	**if(SQLCODE==1403)
	**return 0;
*/
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	sprintf(v_parameter_array[0],"%d", fAddedDays);
	sprintf(v_parameter_array[1], "%ld",IdNo);
	Trim(v_parameter_array[0]);
	Trim(v_parameter_array[1]);
	printf("zhangjb----------3[%ld]\n",IdNo);
	printf("zhangjb----------v_parameter_array[0][%s]\n",v_parameter_array[0]);
	printf("zhangjb----------v_parameter_array[1][%s]\n",v_parameter_array[1]);
	v_ret = 0;
	v_ret = OrderUpdateCustExpire(ORDERIDTYPE_USER, sIdNo, 100, OpCode, LoginAccept, LoginNo,
			OpNote, sIdNo, "expire_time=expire_time+to_number(:fAddedDays)", "", v_parameter_array);
	printf("zhangjb----------43[%d]\n",v_ret);
	if (v_ret < 0)
	{
		printf("UndoExpirTime:%s\n",TmpSqlStr);
		return -1;
	}	
	if (v_ret > 0)
		return 0;
	printf("zhangjb----------4[%d]\n",v_ret);
	/*ng_解耦_数据工单 add by mengfy@10/03/2009*/	
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(LoginNo,OrgId);
	if(ret <0)
	{
	  	printf("获取工号org_id失败![%s]\n",LoginNo);
	  	return -1; 
	}
	Trim(OrgId);
	printf("zhangjb----------5[%d]\n",ret);
	sprintf(TmpSqlStr,"INSERT INTO wExpireList(id_no,total_date,\
	                   login_accept,op_code,time_code,days,\
	                   login_no,org_code,op_time,op_note,org_id) \
	                   VALUES( :v1,:v2,:v3,:v4,:v5,:v6,:v7,\
	                   :v8,to_date(:v9,'YYYYMMDD HH24:MI:SS'),:v10,:v11)");

	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :IdNo,:TotalDate,:LoginAccept,:OpCode,
	                           :TimeCode,:fAddedDays,:LoginNo,:OrgCode,
	                           :OpTime,:OpNote,:OrgId;		
	/*---组织机构改造插入表字段edit by zhangxha at 04/03/2009---end*/
/**
	EXEC SQL EXECUTE IMMEDIATE :TmpSqlStr;
***/printf("zhangjb----------5[%d]\n",SQLCODE);
	if ( SQL_OK != SQLCODE ) 
	{
		printf("UndoExpire:%d[%s][%s][%ld][%d][%ld][%s][%s][%d][%s][%s][%s][%s]\n",SQLCODE,TmpSqlStr,SQLERRMSG,IdNo,TotalDate,LoginAccept,OpCode,TimeCode,fAddedDays,LoginNo,OrgCode,OpTime,OpNote);
		return -1;
	}
	
	return RET_OK;
}

/************************************************************
更新长白行有效期
************************************************************/
int PublicUpdateExpireTime_crm( long InIdNo,        char  *InSmCode, 
                            char *InRegionCode, double InPayMoney,
                            int  InTotalDate,   long   InLoginAccept,
                            char *InOpCode,     char  *InLoginNo,
                            char *InOrgCode,    char  *InOpTime,
                            char *InOpNote)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+1];
		char ExpireFlag[1+1];
		char TimeCode[2+1];
		char RegionCode[2+1];
		char SmCode[2+1];
		char OpCode[4+1];
		char LoginNo[6+1];
		char OrgCode[9+1];
		char OpTime[19+1];
		char OpNote[60+1];
		double PayMoney = 0.00;
		int  Days = 0;
		int  MaxDays = 0;
		int  Days2Expire =0;
		int  AddedDays = 0;
		int  TotalDate = 0;
		long IdNo = 0;
		long LoginAccept = 0;
		char BillCode[8+1];
		int  CompDays =0;
		int  TwoyearsDay =0;
		
		int  ret=0;
		char OrgId[10+1];
		char v_id_no[14+1];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		int  v_ret=0;		
	EXEC SQL END DECLARE SECTION;
	init(TmpSqlStr);
	init(ExpireFlag);
	init(TimeCode);
	init(RegionCode);
	init(SmCode);
	init(OpCode);
	init(LoginNo);
	init(OrgCode);
	init(OpTime);
	init(OpNote);
	init(OrgId);
	init(v_id_no);
	strcpy(RegionCode,InRegionCode);
	strcpy(SmCode,InSmCode);
	strcpy(OpCode,InOpCode);
	strcpy(LoginNo,InLoginNo);
	strcpy(OrgCode,InOrgCode);
	strcpy(OpTime,InOpTime);
	strcpy(OpNote,InOpNote);
	
	PayMoney = InPayMoney;
	TotalDate = InTotalDate;
	IdNo = InIdNo;
	LoginAccept = InLoginAccept;
	
	/*****************************************
	check whether the customer's expire need changing after pay
	******************************************/
	Sinitn(TmpSqlStr);
	Sinitn(ExpireFlag);
	sprintf(TmpSqlStr,"SELECT nvl(expire_flag,'N') \
	                   FROM sSmCode \
	                   WHERE region_code=:v1 \
	                   AND sm_code=:v2");
	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr into :ExpireFlag using :RegionCode,:SmCode;
	END;
	END-EXEC;
	printf("zhangjb---------[%s][%s][%s]\n",TmpSqlStr,RegionCode,SmCode);
	if ( SQL_OK != SQLCODE ) 
	{
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_SM_CODE_NOT_EXIST;
	}
	
	if (strcmp(ExpireFlag,"Y")!= 0)
	{
		return RET_OK;
	}
	
	/*******************************************************
	query 'time_code' and 'days' from table 'sPayTime',according to 'region_code' 
	and payed money falling in the range of 'begin_money' and 'end_money'[左闭右闭区间]
	取得的days，表示在现有的失效日期基础上增加days天
	在sPayTime中增加bill_code字段 char(8) --- 20020717
	TFX系列有效期的处理：调整sPayTime，增加open_flag char(1) 
	初始化有效期处理代码 0 开户操作，1交费操作 2转网操作 ---- 20020718
	**************************************************************************/
	
/***
	取用户的mode_code
**/
	 init(BillCode);
     init(TmpSqlStr);
     sprintf(TmpSqlStr,"select mode_code from dBillCustDetail%ld \
                        where id_no=:v1 \
                        and   begin_time<=sysdate\
                        and   end_time>sysdate\
                        and   mode_flag='0' and mode_time='Y'",
                        IdNo%10);
                                                        
        EXEC SQL EXECUTE
        BEGIN
                EXECUTE IMMEDIATE :TmpSqlStr into :BillCode  using :IdNo;
        END;
        END-EXEC;

       if(SQLCODE!=0)
       {
                       printf("\n  TmpSql=[%s]",TmpSqlStr);
                       printf("dBillCustDetail  Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
                       return  130078;            
       }   
        
	Sinitn(TimeCode);
	sprintf(TmpSqlStr,"SELECT nvl(time_code,' '),nvl(days,0) \
	                   FROM sPayTime \
	                   WHERE region_code=:v1 \
	                   AND time_code=:v2 \
					   AND bill_code=:v3 \
	                   AND open_flag='1' \
	                   AND begin_money<=round(:v4,2) \
	                   AND end_money>=round(:v5,2)");
	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :TimeCode,:Days using :RegionCode,:SmCode,:BillCode,:PayMoney,:PayMoney;
	END;
	END-EXEC;
	printf("[%s][%s][%s][%f]\n",RegionCode,SmCode,BillCode,PayMoney);	
	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_OK;
	}
	printf("Days[%d]\n",Days);
	/***********************************************
	query 'max_days' from table 'sPayTimeMax', according to region_code
	取得的max_days，表示新的失效日期不能超过当前系统日期 max_days天
	***********************************************/
	if(strcmp(SmCode,"cb")==0||strcmp(SmCode,"z0")==0)
	{
	sprintf(TmpSqlStr,"SELECT nvl(a.max_days,0),floor(to_date(:v3,'YYYYMMDD HH24:MI:SS')-b.expire_time) \
	                   FROM  sPayTimeMax a,dCustExpire b \
                       WHERE a.region_code=:v1 \
                       AND   b.id_no=:v2 and b.begin_flag='Y'");

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :MaxDays,:Days2Expire using :OpTime,:RegionCode,:IdNo;
	END;
	END-EXEC;
	printf("MaxDays[%d]DaysExpire[%d]\n",MaxDays,Days2Expire);
	if ( SQLCODE==1403 )  return 0; 
	if ( SQLCODE<0 ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_GET_S_PAY_TIME_MAX_ERR_OR_CUST_EXPIRE_ERR;
	}
	}
	else
	{
			sprintf(TmpSqlStr,"SELECT nvl(a.max_days,0),floor(to_date(:v3,'YYYYMMDD HH24:MI:SS')-b.expire_time) \
							   FROM  sPayTimeMax a,dCustExpire b \
							   WHERE a.region_code=:v1 \
							   AND   b.id_no=:v2 ");

			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :TmpSqlStr INTO :MaxDays,:Days2Expire using :OpTime,:RegionCode,:IdNo;
			END;
			END-EXEC;
			printf("MaxDays[%d]DaysExpire[%d]\n",MaxDays,Days2Expire);
			if ( SQLCODE==1403 )  return 0; 
			if ( SQLCODE<0 ) 
			{
				printf("TmpSqlStr[%s]\n",TmpSqlStr);
				printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
				return RET_GET_S_PAY_TIME_MAX_ERR_OR_CUST_EXPIRE_ERR;
			}
	}
	if(Days2Expire>0)
	{
		AddedDays=Days+Days2Expire;
	}
	else
	{
		AddedDays = Days;
	}
	sprintf(TmpSqlStr,"SELECT floor(to_date(:v0,'yyyymmdd hh24:mi:ss')+:v1-expire_time),floor(:v4+expire_time-to_date(:v2,'yyyymmdd hh24:mi:ss')) \
					   FROM  dCustExpire \
					   WHERE id_no=:v3 ");

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :TwoyearsDay,:CompDays using :OpTime,:MaxDays,:AddedDays,:OpTime,:IdNo;
	END;
	END-EXEC;
	printf("CompDays[%d]TwoyearsDay=[%d]\n",CompDays,TwoyearsDay);
	if ( SQLCODE==1403 )  return 0; 
	if ( SQLCODE<0 ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_GET_S_PAY_TIME_MAX_ERR_OR_CUST_EXPIRE_ERR;
	}
	if (CompDays>MaxDays)
	{
		AddedDays=TwoyearsDay;
	}	
	/***********************************************
	如果该用户有效期已经超过当前时间：即Days2Expire<0 ，
	则有效期起算时间从当前算起[luoxiuchuan modi 20030124
	************************************************/
	/*
	**if (Days2Expire < 0)
	**{
	**	AddedDays = AddedDays - Days2Expire;
	**}
	***
	*************************************************
	**AddedDays = min(Days,MaxDays - Days2Expire);
	**update 'old_expire' as 'expire_time' ,and then 'expire_time' 
	**as currently calulated time,in table 'dCustExpire'
	**************************************************
	**ng_解耦_数据工单 add by mengfy@10/03/2009
	**sprintf(TmpSqlStr,"UPDATE dCustExpire \
	**                   SET old_expire=expire_time,expire_time=expire_time + :v1 \
	**                   WHERE id_no=:v2",
	**                   AddedDays,InIdNo);
    **
	**EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	**EXEC SQL EXECUTE sql_stmt using :AddedDays,:IdNo;
    **
	**if ( SQL_OK != SQLCODE ) 
	**{
	**	printf("TmpSqlStr[%s]\n",TmpSqlStr);
	**	printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
	**	return RET_UPDATE_CUST_EXPIRE_ERR;
	**}
*/
	sprintf(v_id_no,"%ld",IdNo);
	Trim(v_id_no);
	memset(v_parameter_array, 0, sizeof(v_parameter_array));
	sprintf(v_parameter_array[0], "%d",AddedDays);
	sprintf(v_parameter_array[1],"%ld" ,IdNo);
	Trim(v_parameter_array[0]);
	Trim(v_parameter_array[1]);
	v_ret = 0;
	v_ret = OrderUpdateCustExpire(ORDERIDTYPE_USER, v_id_no, 100, OpCode, LoginAccept, LoginNo,
			OpNote, v_id_no, "old_expire=expire_time,expire_time=expire_time + :AddedDays", "", v_parameter_array);
	if (v_ret != 0)
	{	
		printf("OrderUpdateCustExpire faile\n");
		printf("%d\n",v_ret);
		return RET_UPDATE_CUST_EXPIRE_ERR;
	}	
	/*ng_解耦_数据工单 add by mengfy@10/03/2009*/
	printf("\nUPDATE dCustExpire succeed\n");
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(LoginNo,OrgId);
	if(ret <0)
	{
		printf("获取工号org_id失败![%s]\n",LoginNo);
		return 103221; 
	}
	Trim(OrgId);
 				
	/*******************************************************
	**insert a record into wExpireList,recording the expire change action
	*******************************************************/
	sprintf(TmpSqlStr,"INSERT INTO wExpireList(id_no,total_date,login_accept,op_code,time_code,days,\
	                   login_no,org_code,op_time,op_note,org_id) \
	                   VALUES( :v1,:v2,:v3,:v4,:v5,:v6,\
	                   :v7,:v8,to_date(:v9,'YYYYMMDD HH24:MI:SS'),:v10,:v11)");

	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :IdNo,:TotalDate,:LoginAccept,:OpCode,
	                                :TimeCode,:AddedDays,:LoginNo,:OrgCode,:OpTime,:OpNote,:OrgId;
	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_INSERT_W_EXPIRE_LIST_ERR;
	}
	printf("INSERT INTO wExpireList succeed\n");
	/*---组织机构改造插入表字段edit by zhangxha at 04/03/2009---end*/
	return RET_OK;
}

