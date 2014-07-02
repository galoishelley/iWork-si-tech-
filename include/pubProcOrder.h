#define 	BIZ_OPR_REGIST		 "01" 	/* 用户注册 */
#define 	BIZ_OPR_EXIT		   "02"	/* 用户注销 */
#define 	BIZ_OPR_CHGPWD		 "03"	/* 密码修改 */
#define 	BIZ_OPR_SUSPEND		 "04"	/* 业务暂停 */
#define 	BIZ_OPR_RESUME		 "05"	/* 业务恢复 */
#define 	BIZ_OPR_DEST		   "06" 	/* 服务定购 */
#define 	BIZ_OPR_UNDEST		 "07"	/* 服务订购取消 */
#define 	BIZ_OPR_CHGDAT		 "08"	/* 资料变更 */
#define 	BIZ_OPR_PWDRESET   "09"	/* 密码重置 */
#define		BIZ_OPR_PRECANCLE	 "17"	/* 预退订 */
#define 	BIZ_OPR_UNDESTALL	 "99"	/* 全业务退订 */
#define   BOSS_OPRSOURCE     "08"    /* 订购渠道BOSS默认编码*/
#define   LOCALCODE      		 0   /* 本地发起标志 */
#define   SYNCFLAG           1   /* 同步调用标志 */
#define   DISSYNCFLAG        0   /* 异步调用标志 */

#define 	BIZ_OPR_11		 "11"   /* 11-赠送 */
#define 	BIZ_OPR_12		 "12"   /* 12-无线音乐注销 */
#define 	BIZ_OPR_13		 "13"   /* 13-无线音乐变更 */
#define 	BIZ_OPR_14		 "14"   /* 14-用户主动暂停 */   
#define 	BIZ_OPR_15		 "15"   /* 15-用户主动恢复 */ 
#define 	BIZ_OPR_21		 "21"   /* 21-订购确认,21-主动暂停,多媒体彩铃使用   */         
#define 	BIZ_OPR_22		 "22"   /* 22-主动恢复  ,多媒体彩铃 */         
#define 	BIZ_OPR_89		 "89"   /* 89-SP全业务退订  */ 
#define 	BIZ_OPR_99		 "99"   /* 99-DSMP全业务退订 */
#define 	BIZ_OPR_80		 "80"   /* 80-用户业务开关  */ 
#define 	BIZ_OPR_90		 "90"   /* 90-移动信息服务总开关 */
#define 	BIZ_OPR_25		 "25"   /* 28|游戏业务25 – 点播 */
#define 	COMB_SYNCFLAG    "1"    /* DSMP捆绑处理同步、异步设置 0：异步调用 */
/*
结构名称：SORDERREQMSG
编写时间：2007/05/15
功能描述：保存梦网业务订购信息
*/
typedef struct SORDERREQMSG{
		long  lLoginAccept;         /* 操作流水 */
    char  vOp_Code[4+1];        /* 操作代码 */
    char  vLogin_No[6+1];       /* 操作员工号 */
    char  vLogin_Pass[16+1];    /* 操作员密码 */
    char  vOrg_Code[9+1];       /* 归属代码 */
    char  vPhoneType[2+1];      /* 标识类型 */
    char  vPhone_No[15+1];      /* 标识号码 */
    char  vBusi_Type[2+1];      /* 业务类型 */
    char  vOp_Type[2+1];        /* 交易类型 */
    char  vPasswd[8+1];         /* 业务密码 */
    char  vSp_Code[12+1];       /* 企业代码 */
    char  vSp_Bizcode[21+1];    /* 业务代码 */
    char  vOp_Time[20+1];       /* 业务处理时间 */
    char  vUserStatus[2+1];     /* 用户状态 */
    char  vStatusChgTime[20+1]; /* 用户状态变更时间 */
    char  vImsi[20+1];          /* BlackBerry用户IMSI号 */
    char  vServType[2+1];       /* BlackBerry业务类型，01- 集团客户业务02- 集团客户扩展业务*/
    char  vProvinceCode[3+1];   /* 省份代码 */
    char  vInfoCode[100+1];     /* 受理信息代码 */
    char  vInfoValue[100+1];    /* 受理信息值 */
    char  vOp_Note[60+1];       /* 操作备注 */
    char  vTotal_Date[8+1];     /* 帐务日期 */
    char  vDateChar[6+1];       /* 帐务年月 */
    char  vSource[2+1];         /* 发起来源 */
    char  vLocalCode[1+1];      /* 本地标志,0:BOSS发起受理,1:业务平台发起受理 */
    char  vSyncFlag[1+1];       /* 同步标志,0:异步调用，1：同步调用 */
    char  vRemark[512+1];       /* 操作备注，无线音乐俱乐部传如下值：会员级别~增值业务代码~生效时间~用户品牌~是否彩铃用户 */
    char  vReturnCode[6+1];     /* 返回代码 0：成功，其他失败 */
    char  vReturnMsg[128+1];    /* 返回消息 */
}SORDERREQMSG;

/*
结构名称：DSMPORDEROpr
编写时间：2008/01/15
功能描述：DSMP产品定购操作信息
*/
typedef struct DSMPORDEROpr
{
	long   order_id;        
	char   id_no                [14+1];  
	char   phone_no             [15+1];  
	char   serv_code            [2+1];   
	char   opr_code             [2+1];   
	char   third_id_no          [14+1];  
	char   third_phoneno        [15+1];  
	char   spid                 [20+1];  
	char   bizcode              [21+1];  
	char   prod_code            [21+1];  
	char   comb_flag            [1+1];   
	char   mode_code            [8+1];   
	char   chg_flag             [1+1];   
	char   chrg_type            [1+1];   
	char   channel_id           [6+1];   
	char   opr_source           [2+1];   
	char   first_order_date     [17+1];  
	char   first_login_no       [6+1];   
	char   last_order_date      [17+1];  
	char   opr_time             [17+1];  
	char   eff_time             [17+1];  
	char   end_time             [17+1];  
	char   local_code           [1+1];   
	char   valid_flag           [1+1];   
	char   reserved             [100+1]; 
	char   oprnumb              [22+1];  
	char   login_no             [6+1];   
	char   op_code              [6+1];   
	char   belong_code          [8+1];   
	char   total_date           [8+1];   
	char   login_accept         [14+1];  
	char   sys_note             [100+1]; 
	char   op_note              [100+1]; 
    char   sm_code              [2+1];
    char   password             [32+1];
    char   infocode             [1001];
    char   infovalue            [1001];
    char   old_password         [32+1]; 
	char   send_spid            [20+1];  
	char   send_bizcode         [21+1]; 
	char   sync_flag            [1+1];   /* 1：同步  0：异步调用 */ 
	char   multi_order_flag     [1+1];   /** 多条定购的处理标志  1：多条  0：表示单条。用于02、99、89等操作发送同步接口表信息 **/
	char   send_eff_time        [17+1];  /* 操作的生效时间，发送到平台的时间*/
	char   msg_oprcode          [2+1];   /** 短信操作提示语代码 二次确认：RC  **/
	/*add by shijing begin*/
	char   para_num          [12+1];
	char   para_name         [10+1];
	char   para_value        [256+1];
	/*add by shijing end*/
}DSMPORDEROpr ;

#ifdef _extern_
extern int procOrderReq(SORDERREQMSG *sOrderReqMsg);
#endif

