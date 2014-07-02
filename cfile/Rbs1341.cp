#include "boss_srv.h" 
#include "publicsrv.h" 
 
#include "detailbill.h" 
#include "rtOweQry.h" 
 
/*包含ORACLE的SQL通信区*/ 
EXEC SQL INCLUDE SQLCA; 
EXEC SQL INCLUDE SQLDA; 
  
/*定义输入输出参数数组,input_parms用来接收传给服务的参数， 
   output_parms用来存储服务返回的参数 */ 
 
EXEC SQL BEGIN DECLARE SECTION; 
   char input_parms[MAX_PARMS_NUM][1000]; 
   char output_parms[MAX_PARMS_NUM][1000]; 
EXEC SQL END DECLARE SECTION; 
 
int tpsvrinit(int argc, char *argv[]) 
{ 
    FBFR32 *transIN = NULL; 
    FBFR32 *transOUT = NULL; 
    sChkDBLogin(transIN,transOUT,"bodc_s1341Cfm",LABELDBCHANGE); 
    return(0); 
} 
 
void tpsvrdone() 
{ 
    spublicDBClose(LABELDBCHANGE); 
} 
/*************************************** 
 服务名称： 
 编码日期：2003/12/03 
 编码人员：zhangjb 
 调用过程： 
 功能描述： 
 传送参数： 
		in_para[DLMAXITEMS][DLINTERFACEDATA]	 
 
  
 接收参数： 
    return_code   返回代码 
    return_msg     返回信息 
 
  
****************************************/ 
void bodc_s1341Cfm( TPSVCINFO *transb ){ 
    int         input_par_num = 0;  /*输入参数个数*/ 
    int         output_par_num = 0; /*输出参数个数*/ 
    FBFR32      *transIN = NULL;    /*输入缓冲区*/ 
    FBFR32      *transOUT = NULL;   /*输出缓冲区*/ 
    FLDLEN32    len;                /*输出缓存区分配的空间*/  
	int ret=0; 
	int i=0; 
	char tempbuf[100+1]; 
	char in_para[DLMAXITEMS][DLINTERFACEDATA]; 
  EXEC SQL BEGIN DECLARE SECTION; 
    char    return_code[6+1]; 
    char    return_msg[256+1]; 
  EXEC SQL END DECLARE SECTION; 
  init(tempbuf); 
  memset(in_para,0,DLMAXITEMS*DLINTERFACEDATA); 
  memset(return_code, 0, sizeof(return_code)); 
  memset(return_msg, 0, sizeof(return_msg)); 
     
  printf("------------服务bodc_s1341Cfm开始执行---------------\n"); 
    
  strcpy(return_code,"000000"); 
  strcpy(return_msg,"操作成功"); 
  transIN=(FBFR32 *)transb->data; 
  get_input_parms32(transIN, &input_par_num, &output_par_num,LABELDBCHANGE, CONNECT0); 
  len=(FLDLEN32)(FIRST_OCCS*output_par_num*50); 
  transOUT=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len); 
  if(transOUT == (FBFR32 *)NULL)  
  { 
    sprintf(tempbuf,"E200: alloc error in bodc_s1341Cfm,size(%d)", len); 
    error_handler32(transIN,transOUT,"200",tempbuf,LABELDBCHANGE,CONNECT0); 
  } 
    sChkDBLogin(transIN,transOUT,"bodc_s1341Cfm",LABELDBCHANGE); 
        
  EXEC SQL WHENEVER SQLERROR CONTINUE; 
  EXEC SQL WHENEVER SQLWARNING CONTINUE; 
  EXEC SQL WHENEVER NOT FOUND CONTINUE; 
	 
	for(i = 0; i<input_par_num; i++) 
		printf( " ++++ input_parms[%d] = [%s]+++ \n",i,input_parms[i]); 
  /*得到输入参数*/ 
   /* 读取输入参数 */ 
    strcpy(in_para[0],         input_parms[0]); 
    strcpy(in_para[1],         input_parms[1]); 
    strcpy(in_para[2],         input_parms[2]); 
    strcpy(in_para[3],         input_parms[3]); 
    strcpy(in_para[4],         input_parms[4]); 
    strcpy(in_para[5],         input_parms[5]); 
    strcpy(in_para[6],         input_parms[6]); 
    strcpy(in_para[7],         input_parms[7]); 
    strcpy(in_para[8],         input_parms[8]); 
    strcpy(in_para[9],         input_parms[9]); 
    strcpy(in_para[10],         input_parms[10]); 
    strcpy(in_para[11],         input_parms[11]); 
    strcpy(in_para[12],         input_parms[12]); 
    strcpy(in_para[13],         input_parms[13]); 
    strcpy(in_para[14],         input_parms[14]); 
    strcpy(in_para[15],         input_parms[15]); 
    strcpy(in_para[16],         input_parms[16]); 
    strcpy(in_para[17],         input_parms[17]); 
    strcpy(in_para[18],         input_parms[18]); 
	strcpy(in_para[19],         input_parms[19]); 
    strcpy(in_para[20],         input_parms[20]); 
    strcpy(in_para[21],         input_parms[21]); 
    /*20101115 edit by houxue 关于缴费回馈时长业务需求的优化*/
    strcpy(in_para[22],         input_parms[22]); 
    strcpy(in_para[23],         input_parms[23]); 
    /*20101115 end by houxue */
	ret= func_s1341Cfm(in_para,return_code,return_msg); 
  	if(ret!=0) 
  	{ 
  		sprintf(return_code,"%6d",ret); 
  		printf("处理工单bodc_s1341Cfm错误！\n"); 
  		goto end_Valid; 
  	} 
  
  
 
end_Valid: 
#ifdef _DEBUG_ 
  printf("bodc_s1341Cfm return_code=[%s]\n", return_code); 
  printf("bodc_s1341Cfm returnmsg: %s\n", return_msg); 
#endif 
 
	if(strcmp(return_code,"000000")==0) 
	{ 
		EXEC SQL COMMIT;	 
	} 
	else 
	{ 
		EXEC SQL ROLLBACK; 
	} 
  transOUT=add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, 
            "bodc_s1341Cfm", GPARM32_0, return_code, LABELDBCHANGE, CONNECT0 ); 
  transOUT=add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, 
            "bodc_s1341Cfm", GPARM32_1, return_msg, LABELDBCHANGE, CONNECT0 ); 
  Fchg32(transOUT,SVC_ERR_NO32,0,return_code,(FLDLEN32)0); 
  Fchg32(transOUT,SVC_ERR_MSG32,0,return_msg,(FLDLEN32)0); 
 
  /* 断开数据库连接,如果断开操作失败,公用的函数会清理现场,结束程序运行*/ 
 
  /*如果开启了网管日志开关,则记录对应BOSS网管需要的日志*/ 
  #ifdef _DEBUG_ 
     printf("服务bodc_s1341Cfm执行结束[] \n"); 
  #endif 
 
  /**如果定义了关键业务操作重做日志，需要做如下的处理 */ 
  #ifdef _KBOREDOLOG_ 
  #endif 
 
  /*服务返回，缓冲区的内容返回给客户端 */ 
  tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0); 
} 

int func_s1341Cfm(char in_para[DLMAXITEMS][DLINTERFACEDATA], char *v_ret_code, char *v_ret_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	v_system_note[60 + 1];
		char	v_login_no[6 + 1];
		char    v_phone_no[15 + 1];
		long	v_id_no;
		char	v_id_str[14 + 1];
		long	v_total_date;
		long	v_login_accept;
		char 	v_login_accept_str[14 + 1];
		char 	v_sm_code[2 + 1];
		char 	v_belong_code[7 + 1];
		char	v_org_code[9 + 1];		
		char	v_op_code[4 + 1];	
		char	v_op_time[17 + 1];		
		char	v_op_note[60 + 1];
		char	v_org_id[10 + 1];
		char	v_group_id[10 + 1];
		char 	v_chg_flag[1 + 1];
		char	v_cust_flag[1 + 1];		
		char	v_mode_code[8 + 1];
		int		v_diff_days;
		char	v_end_time[17 + 1];
		char	v_effect_time[17 + 1];
		char	v_region_code[2 + 1];
		char	v_prePay_fee[14 + 1];
		char	v_detail_type[1 + 1];
		char	v_detail_code[4 + 1];
		char 	v_vlogin_accept[22 + 1];
		char	v_temp_buf[1000];
		char	v_temp_buf2[1000];
		/*20100414  add by houxue  */ 
		char	v_pre_mode_code[8 + 1];
		int		v_pre_diff_days;
		char	v_pre_end_time[17 + 1];
		/*20101115  add by houxue  */ 
		char  v_pre_effect_time[17 + 1];
		char  c_mode_code[8+1];
		char  c_mode_str[256+1];
		char  c_pre_old_accept[22 + 1];			
		char  c_pre_accept[14 + 1];
		char  c_pre_effect_time[22];
		char  c_pre_op_time[22];	
		char  c_detail_type[1 + 1];
		char  c_detail_code[4 + 1];
		char  c_vlogin_accept[22 + 1];
		int   i=0;
		char  v_beginTime[17+1];
	EXEC SQL END DECLARE SECTION;
		tLoginOpr v_log_opr; 
		TdBillCustDetailIndex oldIndex;
		TdBaseFavIndex			vTdBaseFavIndex;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		int 	v_ret = 0;	
		int		v_count = 0;
		int		v_count1 = 0;
			
		/*初始化*/
		v_id_no = 0;
		v_total_date = 0;
		v_login_accept = 0;
		v_diff_days = 0;
		init(v_vlogin_accept);
		init(v_id_str);
		init(v_login_accept_str);
		init(v_system_note);
		init(v_login_no);	
		init(v_phone_no);
		init(v_sm_code);
		init(v_belong_code);
		init(v_org_code);
		init(v_op_code);
		init(v_op_time);
		init(v_op_note);
		init(v_org_id);
		init(v_group_id);
		init(v_chg_flag);
		init(v_cust_flag);
		init(v_mode_code);
		init(v_end_time);
		init(v_effect_time);
		init(v_temp_buf);
		init(v_region_code);
		init(v_prePay_fee);
		init(v_detail_type);
		init(v_detail_code);
		init(v_pre_mode_code);
		init(v_pre_end_time);
		init(v_beginTime);
		init(v_temp_buf2);

		memset(&v_log_opr, 0, sizeof(v_log_opr));
		memset(&oldIndex, 0, sizeof(oldIndex));	
		memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
		
		/*20101115  add by houxue  */ 
		init(v_pre_effect_time);
		init(c_mode_code);
		init(c_mode_str);
		init(c_pre_old_accept);		
		init(c_pre_accept);
		init(c_pre_effect_time);
		init(c_pre_op_time);
		init(c_detail_type);
		init(c_detail_code);
		init(c_vlogin_accept);
		
		strcpy(v_id_str, in_para[0]);	
		v_total_date = atol(in_para[1]);		
		strcpy(v_login_accept_str, in_para[2]);	
		strcpy(v_sm_code, in_para[3]);
		strcpy(v_belong_code, in_para[4]);
		strcpy(v_phone_no, in_para[5]);
		strcpy(v_org_code, in_para[6]);
		strcpy(v_login_no, in_para[7]);	
		strcpy(v_op_code, in_para[8]);
		strcpy(v_op_time, in_para[9]);
		strcpy(v_op_note, in_para[10]);	
		strcpy(v_group_id, in_para[11]);			
		strcpy(v_org_id, in_para[12]);
		strcpy(v_mode_code, in_para[13]);
		v_diff_days = atoi(in_para[14]);	
		strcpy(v_end_time, in_para[15]);	
		strcpy(v_effect_time, in_para[16]);		
		strcpy(v_region_code, in_para[17]);		
		strcpy(v_prePay_fee, in_para[18]);	
		strcpy(v_pre_mode_code, in_para[19]);
		v_pre_diff_days = atoi(in_para[20]);	
		strcpy(v_pre_end_time, in_para[21]);
		strcpy(v_pre_effect_time, in_para[22]);
		strcpy(c_mode_str, in_para[23]);
		Trim(v_id_str);		
		Trim(v_login_accept_str);
		Trim(v_login_no);
		Trim(v_phone_no);
		Trim(v_sm_code);
		Trim(v_belong_code);
		Trim(v_org_code);
		Trim(v_op_code);
		Trim(v_op_time);
		Trim(v_op_note);
		Trim(v_org_id);
		Trim(v_group_id);
		Trim(v_mode_code);
		Trim(v_end_time);
		Trim(v_effect_time);
		Trim(v_region_code);	
		Trim(v_prePay_fee);
		Trim(v_pre_mode_code);
		Trim(v_pre_end_time);
		Trim(c_mode_str);
		Trim(v_pre_effect_time);
		/*20101115  end by houxue  */
							
		v_id_no = atol(v_id_str);		
		v_login_accept = atol(v_login_accept_str);		
				
		sprintf(v_system_note, "操作员%s对用户%s进行季度缴费回馈受理操作", v_login_no, v_phone_no);
		v_log_opr.vId_No        = v_id_no;
		v_log_opr.vTotal_Date   = v_total_date;
		v_log_opr.vLogin_Accept = v_login_accept;	
		strncpy(v_log_opr.vSm_Code,      v_sm_code,     2);
		strncpy(v_log_opr.vBelong_Code,  v_belong_code, 7);
		strncpy(v_log_opr.vPhone_No,     v_phone_no,    15);
		strncpy(v_log_opr.vOrg_Code,     v_org_code,    9);
		strncpy(v_log_opr.vLogin_No,     v_login_no,    6);
		strncpy(v_log_opr.vOp_Code,      v_op_code,     4);
		strncpy(v_log_opr.vOp_Time,      v_op_time,     17);
		strncpy(v_log_opr.vMachine_Code, " ",           3);
		strncpy(v_log_opr.vBack_Flag,    "0",           1);
		strncpy(v_log_opr.vEncrypt_Fee,  "0",           16);
		strncpy(v_log_opr.vSystem_Note,  v_system_note, 60);  
		strncpy(v_log_opr.vOp_Note,      v_op_note,     60);
		strncpy(v_log_opr.vGroup_Id,     v_group_id,    10);
		strncpy(v_log_opr.vOrg_Id,       v_org_id,      10);
		v_log_opr.vCash_Pay    	= 0;
		v_log_opr.vCheck_Pay   	= 0;
		v_log_opr.vSim_Fee     	= 0;
		v_log_opr.vMachine_Fee 	= 0;
		v_log_opr.vInnet_Fee   	= 0;
		v_log_opr.vChoice_Fee  	= 0;
		v_log_opr.vOther_Fee   	= 0;
		v_log_opr.vHand_Fee    	= 0;
		v_log_opr.vDeposit     	= 0;  
		strncpy(v_chg_flag,  "P", 1);         
		strncpy(v_cust_flag, "N", 1);        
			
		v_ret = recordOprLog(&v_log_opr, v_ret_msg, v_chg_flag, v_cust_flag);
		if(0 != v_ret)
		{
				strcpy(v_ret_code, "600040");
				sprintf(v_ret_msg, "插变更信息错误[%d]!", v_ret);
				return -600040;		      		
		}	
		

		if(0 != strcmp(v_mode_code,"00000000"))
		{
				if(v_diff_days >= 0)
				{
						strcpy(v_ret_code, "600041");
						strcpy(v_ret_msg, "申请的最底消费结束时间无效!");
						return -600041;				
				}
				
				EXEC SQL select count(*) into :v_count from dbillcustdetail where id_no = :v_id_no and mode_code= :v_mode_code
												and end_time >= to_date(:v_end_time, 'yyyymmdd hh24:mi:ss');
				if(0 != SQLCODE)
				{
						strcpy(v_ret_code, "600042");
						sprintf(v_ret_msg, "查询最底消费失败[%d][%s]!", SQLCODE, v_mode_code);
						return -600042;						
				}
				printf("v_count=[%d]\n",v_count);
				EXEC SQL select count(*) into :v_count1 from dbillcustdetail where id_no = :v_id_no and mode_code = :v_mode_code
												and end_time < to_date(:v_end_time, 'yyyymmdd hh24:mi:ss') and end_time >to_date(:v_op_time, 'yyyymmdd hh24:mi:ss') ;
				if(0 != SQLCODE)
				{
						strcpy(v_ret_code, "600043");
						sprintf(v_ret_msg, "查询最底消费失败[%d][%s]!", SQLCODE, v_mode_code);
						return -600043;					
				}
				printf("v_count1=[%d]\n",v_count1);
				if(v_count1 > 0)
				{
						init(v_temp_buf);
						sprintf(v_temp_buf,"select detail_type,  detail_code, to_char(login_accept) from dBillCustDetail%c "
											" where id_no = :v1 and mode_code = :v2 and end_time <to_date(:v3,'yyyymmdd hh24:mi:ss') and end_time >to_date(:v4, 'yyyymmdd hh24:mi:ss')", v_id_str[strlen(v_id_str)-1]);
											
						EXEC SQL PREPARE sql_str FROM :v_temp_buf;
   						EXEC SQL declare ngcur cursor for sql_str;
   						EXEC SQL open ngcur using :v_id_no, :v_mode_code,:v_end_time,:v_op_time;
   						
   						for(;;)
   						{
   							init(v_vlogin_accept);
   							init(v_detail_type);
   							init(v_detail_code);
   							init(v_parameter_array);
							memset(&oldIndex, 0, sizeof(oldIndex));
							
							EXEC SQL FETCH ngcur into :v_detail_type, :v_detail_code, :v_vlogin_accept;	     														
							
							if(SQLCODE!=0 && SQLCODE!=1403)
							{
								strcpy(v_ret_code, "600044");
								sprintf(v_ret_msg, "查询dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
								EXEC SQL close ngcur; 
								return -600044;								
							}
							else if(SQLCODE == 1403)
							{
								break;
							}
							else
							{
								Trim(v_detail_type);
								Trim(v_detail_code);
								Trim(v_vlogin_accept);								
								strcpy(v_parameter_array[0], "Y");
								strcpy(v_parameter_array[1], v_end_time);	
								strcpy(v_parameter_array[2], v_id_str);		
								strcpy(v_parameter_array[3], v_mode_code);	
								strcpy(v_parameter_array[4], v_vlogin_accept);	
								strcpy(v_parameter_array[5], v_detail_type);		
								strcpy(v_parameter_array[6], v_detail_code);									
									
			   					strcpy(oldIndex.sIdNo, v_id_str);
								strcpy(oldIndex.sModeCode, v_mode_code);
								strcpy(oldIndex.sLoginAccept, v_vlogin_accept);
								strcpy(oldIndex.sDetailType, v_detail_type);
								strcpy(oldIndex.sDetailCode, v_detail_code);
	
								v_ret = 0;
								v_ret = OrderUpdateBillCustDetail(ORDERIDTYPE_USER, v_id_str, 100, v_op_code, v_login_accept, v_login_no,
											v_op_note, oldIndex, oldIndex, " mode_status=:v0, end_time = to_date(:v1,'yyyymmdd hh24:mi:ss') ",
												"", v_parameter_array);																																						
								if(v_ret != 0)
								{
									strcpy(v_ret_code, "600044");
									sprintf(v_ret_msg, "更新dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
									EXEC SQL close ngcur; 
									return -600044;								
								}																						
							}																						
   						}
   						EXEC SQL close ngcur; 
				}

				if(v_count==0 && v_count1==0)
				{	
					sprintf(v_ret_code, "%06d", pubBillBeginTimeMode(v_phone_no, v_mode_code, v_effect_time, v_end_time,
								v_op_code, v_login_no, v_login_accept_str, v_op_time, "1", "", "", v_ret_msg));
						if(0 != strcmp(v_ret_code,"000000"))
						{
								strcpy(v_ret_code, "600045");
								sprintf(v_ret_msg, "申请最底消费失败!");
								return -600045;									
						}
				}
			
		}
		/*20100414  edit by houxue  */ 		
		if(0 != strcmp(v_pre_mode_code,"00000000"))
		{
				if(v_pre_diff_days >= 0)
				{
						strcpy(v_ret_code, "600047");
						strcpy(v_ret_msg, "申请的赠送套餐结束时间无效!");
						return -600047;				
				}
				v_count=0;
				EXEC SQL select count(*) into :v_count from dbillcustdetail where id_no = :v_id_no and mode_code= :v_pre_mode_code
												and end_time >= to_date(:v_pre_end_time, 'yyyymmdd hh24:mi:ss');
				if(0 != SQLCODE)
				{
						strcpy(v_ret_code, "600048");
						sprintf(v_ret_msg, "查询赠送套餐失败[%d][%s]!", SQLCODE, v_pre_mode_code);
						return -600048;						
				}
				printf("v_count=[%d]\n",v_count);
				v_count1=0;
				EXEC SQL select count(*) into :v_count1 from dbillcustdetail where id_no = :v_id_no and mode_code = :v_pre_mode_code
												and end_time < to_date(:v_pre_end_time, 'yyyymmdd hh24:mi:ss') and end_time >to_date(:v_op_time, 'yyyymmdd hh24:mi:ss') ;
				if(0 != SQLCODE)
				{
						strcpy(v_ret_code, "600049");
						sprintf(v_ret_msg, "查询赠送套餐失败[%d][%s]!", SQLCODE, v_pre_mode_code);
						return -600049;					
				}
				printf("v_count1=[%d]\n",v_count1);
				if(v_count1 > 0)
				{
						init(v_temp_buf);
						sprintf(v_temp_buf,"select detail_type,  detail_code, to_char(login_accept),to_char(begin_time,'yyyymmddhh24miss') from dBillCustDetail%c "
											" where id_no = :v1 and mode_code = :v2 and end_time <to_date(:v3,'yyyymmdd hh24:mi:ss') and end_time >to_date(:v4, 'yyyymmdd hh24:mi:ss')", v_id_str[strlen(v_id_str)-1]);
											
						EXEC SQL PREPARE sql_str FROM :v_temp_buf;
   						EXEC SQL declare ngcur1 cursor for sql_str;
   						EXEC SQL open ngcur1 using :v_id_no, :v_pre_mode_code,:v_pre_end_time,:v_op_time;
   						
   						for(;;)
   						{
   							init(v_vlogin_accept);
   							init(v_detail_type);
   							init(v_detail_code);
   							init(v_parameter_array);
   							init(v_beginTime);
							memset(&oldIndex, 0, sizeof(oldIndex));
							
							EXEC SQL FETCH ngcur1 into :v_detail_type, :v_detail_code, :v_vlogin_accept,:v_beginTime;	     														
							
							if(SQLCODE!=0 && SQLCODE!=1403)
							{
								strcpy(v_ret_code, "600050");
								sprintf(v_ret_msg, "查询dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
								EXEC SQL close ngcur1; 
								return -600050;								
							}
							else if(SQLCODE == 1403)
							{
								break;
							}
							else
							{
								Trim(v_detail_type);
								Trim(v_detail_code);
								Trim(v_vlogin_accept);								
								strcpy(v_parameter_array[0], "Y");
								strcpy(v_parameter_array[1], v_pre_end_time);	
								strcpy(v_parameter_array[2], v_id_str);		
								strcpy(v_parameter_array[3], v_pre_mode_code);	
								strcpy(v_parameter_array[4], v_vlogin_accept);	
								strcpy(v_parameter_array[5], v_detail_type);		
								strcpy(v_parameter_array[6], v_detail_code);									
									
			   					strcpy(oldIndex.sIdNo, v_id_str);
								strcpy(oldIndex.sModeCode, v_pre_mode_code);
								strcpy(oldIndex.sLoginAccept, v_vlogin_accept);
								strcpy(oldIndex.sDetailType, v_detail_type);
								strcpy(oldIndex.sDetailCode, v_detail_code);
	
								v_ret = 0;
								v_ret = OrderUpdateBillCustDetail(ORDERIDTYPE_USER, v_id_str, 100, v_op_code, v_login_accept, v_login_no,
											v_op_note, oldIndex, oldIndex, " mode_status=:v0, end_time = to_date(:v1,'yyyymmdd hh24:mi:ss') ",
												"", v_parameter_array);																																						
								if(v_ret != 0)
								{
									strcpy(v_ret_code, "600051");
									sprintf(v_ret_msg, "更新dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
									EXEC SQL close ngcur1; 
									return -600051;								
								}
								printf("v_ret=[%d]\n",v_ret);
								/*add by zhangwm bug 20110609*/
								printf("v_detail_type=[%s],v_detail_code=[%s],v_beginTime=[%s],v_phone_no=[%s]\n",v_detail_type,v_detail_code,v_beginTime,v_phone_no);
								if(strcmp(v_detail_type,"0") == 0)
								{
									init(v_temp_buf2);
									Trim(v_beginTime);
									sprintf(v_temp_buf2,"select msisdn,fav_type,flag_code,to_char(start_time,'yyyymmddhh24miss') from dbasefav "
														" where msisdn = :v1 and flag_code=:v2 and to_char(start_time,'yyyymmddhh24miss')=:v3");
									EXEC SQL PREPARE sql_str1 FROM :v_temp_buf2;
				   					EXEC SQL declare ngcur2 cursor for sql_str1;
				   					EXEC SQL open ngcur2 using :v_phone_no,:v_detail_code,:v_beginTime;
				   					for(;;)
				   					{
										memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
										memset(v_parameter_array, 0, sizeof(v_parameter_array));
										EXEC SQL FETCH ngcur2 into :vTdBaseFavIndex.sMsisdn, :vTdBaseFavIndex.sFavType, :vTdBaseFavIndex.sFlagCode,:vTdBaseFavIndex.sStartTime;	
										if(SQLCODE!=0 && SQLCODE!=1403)
										{
											strcpy(v_ret_code, "600065");
											sprintf(v_ret_msg, "查询dbasefav错误[%d]!",SQLCODE);
											EXEC SQL close ngcur2; 
											return -600065;								
										}
										printf("SQLCODE=[%d]\n",SQLCODE);
										if(SQLCODE == 1403) break;
										
										strcpy(v_parameter_array[0],v_pre_end_time);
										strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
										strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
										strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
										strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);
										v_ret = 0;
										v_ret = OrderUpdateBaseFav(ORDERIDTYPE_USER,v_id_str,100,v_op_code,v_login_accept,v_login_no,v_op_note,vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:v11,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
										printf("v_ret=[%d]\n",v_ret);
										if (v_ret != 0)
										{
											strcpy(v_ret_code, "600066");
											sprintf(v_ret_msg, "更新dbasefav错误[%d]!",SQLCODE);
											EXEC SQL close ngcur2; 
											return -600066;								
										}
									}
									EXEC SQL close ngcur2; 
								}																				
							}																						
   						}
   						EXEC SQL close ngcur1; 
				}

				if(v_count==0 && v_count1==0)
				{	
					sprintf(v_ret_code, "%06d", pubBillBeginTimeMode(v_phone_no, v_pre_mode_code, v_pre_effect_time, v_pre_end_time,
								v_op_code, v_login_no, v_login_accept_str, v_op_time, "1", "", "", v_ret_msg));
						if(0 != strcmp(v_ret_code,"000000"))
						{
								printf("SQLCODE[%d]\n",SQLCODE);
								printf("v_ret_code[%s],v_ret_msg[%s]\n",v_ret_code,v_ret_msg);
								strcpy(v_ret_code, "600052");
								sprintf(v_ret_msg, "申请赠送套餐失败!");
								return -600052;									
						}
				}
			
		}
		/*20100414  end by houxue  */ 
		/*20101115 edit by houxue 关于缴费回馈时长业务需求的优化*/
		while(getValueByIndex(c_mode_str, i, '|', c_mode_code)>=0)
		{
			Trim(c_mode_code);
			init(c_pre_old_accept);		
			init(c_pre_accept);
			init(c_pre_effect_time);
			init(c_pre_op_time);
			init(v_temp_buf);
			v_count=0;
			EXEC SQL select count(*)
					   into :v_count
					   from dBillCustDetail 
					  where id_no = :v_id_no 
					  	and mode_code = :c_mode_code 
					  	and mode_time = 'Y' 
					  	and mode_status='Y' 
					  	and begin_time<=sysdate
					  	and end_time>add_months(sysdate,1);	
			if(0!=SQLCODE && 1403!=SQLCODE)
			{
				strcpy(v_ret_code, "600053");
				sprintf(v_ret_msg, "查询套餐dBillCustDetail失败[%d]!",SQLCODE);
				return -600053;							    		
			}
			if(v_count>0)
			{
				init(v_temp_buf);
				sprintf(v_temp_buf,"select detail_type,  detail_code, to_char(login_accept),to_char(add_months(sysdate,1),'yyyymm')||'01 00:00:00',to_char(begin_time,'yyyymmddhh24miss') from dBillCustDetail%c "
									" where id_no = :v1 and mode_code = :v2 and begin_time<=sysdate and end_time>add_months(sysdate,1) ", v_id_str[strlen(v_id_str)-1]);
									
				EXEC SQL PREPARE sql_str1 FROM :v_temp_buf;
   				EXEC SQL declare ngcur4 cursor for sql_str1;
   				EXEC SQL open ngcur4 using :v_id_no, :c_mode_code;
   				
   				for(;;)
   				{
   					init(c_vlogin_accept);
   					init(c_detail_type);
   					init(c_detail_code);
   					init(c_pre_effect_time);
   					init(v_parameter_array);
   					init(v_beginTime);
					memset(&oldIndex, 0, sizeof(oldIndex));
					
					EXEC SQL FETCH ngcur4 into :c_detail_type, :c_detail_code, :c_vlogin_accept,:c_pre_effect_time,:v_beginTime;	     														
					
					if(SQLCODE!=0 && SQLCODE!=1403)
					{
						strcpy(v_ret_code, "600054");
						sprintf(v_ret_msg, "查询dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
						EXEC SQL close ngcur4; 
						return -600054;								
					}
					else if(SQLCODE == 1403)
					{
						break;
					}
					else
					{
						Trim(c_detail_type);
						Trim(c_detail_code);
						Trim(c_vlogin_accept);	
						Trim(c_pre_effect_time);							
						strcpy(v_parameter_array[0], "Y");
						strcpy(v_parameter_array[1], c_pre_effect_time);	
						strcpy(v_parameter_array[2], v_id_str);		
						strcpy(v_parameter_array[3], c_mode_code);	
						strcpy(v_parameter_array[4], c_vlogin_accept);	
						strcpy(v_parameter_array[5], c_detail_type);		
						strcpy(v_parameter_array[6], c_detail_code);									
							
			   			strcpy(oldIndex.sIdNo, v_id_str);
						strcpy(oldIndex.sModeCode, c_mode_code);
						strcpy(oldIndex.sLoginAccept, c_vlogin_accept);
						strcpy(oldIndex.sDetailType, c_detail_type);
						strcpy(oldIndex.sDetailCode, c_detail_code);
	
						v_ret = 0;
						v_ret = OrderUpdateBillCustDetail(ORDERIDTYPE_USER, v_id_str, 100, v_op_code, v_login_accept, v_login_no,
									v_op_note, oldIndex, oldIndex, " mode_status=:v0, end_time = to_date(:v1,'yyyymmdd hh24:mi:ss') ",
										"", v_parameter_array);																																						
						if(v_ret != 0)
						{
							strcpy(v_ret_code, "600055");
							sprintf(v_ret_msg, "更新dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
							EXEC SQL close ngcur4; 
							return -600055;								
						}
						
						/*add by zhangwm bug 20110609*/
						if(strcmp(c_detail_type,"0") == 0)
						{
							init(v_temp_buf2);
							Trim(v_beginTime);
							sprintf(v_temp_buf2,"select msisdn,fav_type,flag_code,to_char(start_time,'yyyymmddhh24miss') from dbasefav "
												" where msisdn = :v1 and flag_code=:v2 and to_char(start_time,'yyyymmddhh24miss')=:v3");
							EXEC SQL PREPARE sql_str2 FROM :v_temp_buf2;
				   			EXEC SQL declare ngcur3 cursor for sql_str2;
				   			EXEC SQL open ngcur3 using :v_phone_no, :c_detail_code,:v_beginTime;
				   			for(;;)
				   			{
								memset(&vTdBaseFavIndex, 0, sizeof(vTdBaseFavIndex));
								memset(v_parameter_array, 0, sizeof(v_parameter_array));
								EXEC SQL FETCH ngcur3 into :vTdBaseFavIndex.sMsisdn, :vTdBaseFavIndex.sFavType, :vTdBaseFavIndex.sFlagCode,:vTdBaseFavIndex.sStartTime;	
								if(SQLCODE!=0 && SQLCODE!=1403)
								{
									strcpy(v_ret_code, "600068");
									sprintf(v_ret_msg, "查询dbasefav错误[%d]!",SQLCODE);
									EXEC SQL close ngcur3; 
									return -600068;								
								}
								if(SQLCODE == 1403) break;
								
								strcpy(v_parameter_array[0],c_pre_effect_time);
								strcpy(v_parameter_array[1],vTdBaseFavIndex.sMsisdn);
								strcpy(v_parameter_array[2],vTdBaseFavIndex.sFavType);
								strcpy(v_parameter_array[3],vTdBaseFavIndex.sFlagCode);
								strcpy(v_parameter_array[4],vTdBaseFavIndex.sStartTime);
								v_ret = 0;
								v_ret = OrderUpdateBaseFav(ORDERIDTYPE_USER,v_id_str,100,v_op_code,v_login_accept,v_login_no,v_op_note,vTdBaseFavIndex,vTdBaseFavIndex,"end_time=to_date(:v11,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
								if (v_ret != 0)
								{
									strcpy(v_ret_code, "600069");
									sprintf(v_ret_msg, "更新dbasefav错误[%d]!",SQLCODE);
									EXEC SQL close ngcur3; 
									return -600069;								
								}
							}
							EXEC SQL close ngcur3; 
						}																		
					}																						
   				}
   				EXEC SQL close ngcur4; 
			}
			else
			{
				EXEC SQL select to_char(login_accept),to_char(sMaxSysAccept.NEXTVAL),to_char(sysdate,'YYYYMMDD HH24:MI:SS'),to_char(sysdate,'YYYYMMDD HH24:MI:SS')
						   into :c_pre_old_accept, :c_pre_accept, :c_pre_effect_time, :c_pre_op_time
						   from dBillCustDetail 
						  where id_no = :v_id_no and mode_code = :c_mode_code and mode_time = 'Y' and mode_status='Y' and end_time > sysdate;	
				if(0!=SQLCODE && 1403!=SQLCODE)
				{
					strcpy(v_ret_code, "600056");
					sprintf(v_ret_msg, "查询dBillCustDetail%c错误[%d]!", v_id_str[strlen(v_id_str)-1], SQLCODE);
					return -600056;					    		
				}
				Trim(c_pre_old_accept);
				Trim(c_pre_accept);
				Trim(c_pre_effect_time);
				Trim(c_pre_op_time);
				v_ret = 0;
				v_ret = pubBillEndMode(v_phone_no, c_mode_code, c_pre_old_accept, c_pre_effect_time, v_op_code, v_login_no, c_pre_accept,
																	c_pre_op_time, v_temp_buf);
				if(0 != v_ret)
				{
					    printf("v_ret[%d],SQLCODE[%d]\n",v_ret,SQLCODE);
						printf("取消可选套餐[%s]%s\n", c_mode_code, v_temp_buf);	
						strcpy(v_ret_code, "600057");
						sprintf(v_ret_msg, "取消可选套餐错误[%s]%s!",  c_mode_code, v_temp_buf);
						return -600057;							
				}
			}

			i++;			
		}
		/*20101115  end by houxue  */ 
		/***********************************
		更新有效期
		***********************************/
		v_ret = 0; 		
		v_ret = PublicUpdateExpireTime_crm(v_id_no, v_sm_code, v_region_code, atof(v_prePay_fee), v_total_date,
																			v_login_accept, v_op_code, v_login_no, v_org_code, v_op_time, v_op_note);
		if (0 != v_ret)
		{
			strcpy(v_ret_code, "600046");
			sprintf(v_ret_msg, "更新有效期失败[%d]!", v_ret);
			return -600046;						
		}		
		return 0;
}
