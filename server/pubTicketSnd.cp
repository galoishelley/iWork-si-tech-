/***************************************
 *function  电子券与联动优势的相关接口
 *author    chendx
 *created   20130206
****************************************/

#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"


#include <sys/timeb.h>
#include <time.h>

#define ROWLEN 2048
#define _DEBUG_

EXEC SQL INCLUDE SQLCA;


/*************电子券发放接口**************
  MCODE		功能码	    101810
  MID		业务流水号	每笔交易必须唯一
  DATE		交易日期	YYYYMMDD请求日期
  TIME		交易时间	hhmmss请求时间
  ACT_ID	批次号	    即建立电子券活动的活动号
  MBL_NO	手机号	
  BON_AMT	电子券金额	以分为单位
  MLOG_NO	电子券订单号
  TCNL_TYP	省渠道	    连BOSS的话，填BOSS
  OPTYP	    开户标识	0:不开户1:开户 缺省值为0
  TTXN_DT	日期	    YYYYMMDD
  MERID	    商户号	    BOSS
  SIGN	    签名
  *****************************/
int pTicketSnd(long LoginAccept,char *RequestDate,char *RequestTime,char *ActId,
               char *PhoneNo,char *Cash,char *SendDate,char *SendTime,int SourceFlag,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		int iCount = 0;
		char vPhoneNo[15+1];
		long vLoginAccept = 0;
		char vActId[50+1];		/*营销活动id*/
		char vCash[50+1];		/*面额,以分为单位*/
		char vRequestDate[8+1];	/*电子券发送请求日期*/
		char vRequestTime[6+1];	/*电子券发送请求时间*/
		char vSendDate[8+1];	/*发送时间 YYYYMMDD*/
		char vSendTime[6+1];	/*hh24miss*/
		
		char vTemp[512];
		char vSign[172+1];		/*签名信息*/
		char vSendMsg[1024];	/*发送信息*/
		char retemp[256]; 		/*反馈信息*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
    	int  vStatus = 0;		/*电子券当前状态 0未发放 1已发放 2已撤销*/
	
		char vNewAddress[50];
		char vBunchNo[15+1];	/*电子卡串号*/
		char vIdNo[14+1];
		long LoginAcceptTmp = 0;/*本次修改已赠电子卷的流水*/
		char update_sql[1000];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vActId);
	init(vCash);
	init(vRequestDate);
	init(vRequestTime);
	init(vSendDate);
	init(vSendTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	init(vBunchNo);
	init(vIdNo);
	init(update_sql);
	memset(v_parameter_array, 0, DLMAXITEMS*DLINTERFACEDATA);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vCash,Cash);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	strcpy(vActId,ActId);
	strcpy(vSendDate,SendDate);
	strcpy(vSendTime,SendTime);	
	
	vLoginAccept = LoginAccept;
	
	Trim(vPhoneNo);
	Trim(vCash);
	Trim(vRequestDate);
	Trim(vRequestTime);
	Trim(vActId);
	Trim(vSendDate);
	Trim(vSendTime);

	/*判断用户是否有手机支付功能
	EXEC SQL select count(*) into :iCount 
		       from ddsmpordermsg
		      where phone_no =:vPhoneNo  
		        and serv_code ='54' 
		        and spid ='698000' 
		        and bizcode ='00000001'
				and sysdate between eff_time and end_time ;
	if(iCount <= 0)
	{		
		strcpy(RspCode,"999999");
		strcpy(RspMsg,"该用户未开通手机支付账户，不允许办理电子券相关业务");
		pubLog_Debug(_FFL_, "pTicketSnd", "","%s",ReternMsg);
	}*/
	
	/*如果日对账出来的差异电子券已撤销的，可以直接删除不再处理
	if(1 == SourceFlag)
	{
		iCount = 0;
		EXEC  SQL  select count(*) into :iCount
		 	         from wCmpExcTicket a,dticketsend b
		            where a.login_accept = :vLoginAccept
		              and a.login_accept = b.login_accept
		              and b.status = '3';
		 if(iCount > 0)
		 {
		 	EXEC SQL delete from wCmpExcTicket where login_accept = :vLoginAccept;
		 	return 0 ;	
		 }
	}*/
	
	
	/*生成签名的加密字段*/
	sprintf(vTemp,"MCODE=101810&MID=%ld&DATE=%s&TIME=%s&ACT_ID=%s&"
	              "MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%ld&TCNL_TYP=BOSS&"
				  "OPTYP=1&TTXN_DT=%s&MERID=BOSS",
					vLoginAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					vLoginAccept,
					vSendDate);
	printf("\n待签名信息[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(RspCode,"999999");
		strcpy(RspMsg,"签名加密失败");
		pubLog_Debug(_FFL_, "pTicketSnd", "","%s",ReternMsg);
	}
	sprintf(vSendMsg,"MCODE=101810,MID=%ld,DATE=%s,TIME=%s,ACT_ID=%s,"
	              "MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%ld,TCNL_TYP=BOSS,"
				  "OPTYP=1,TTXN_DT=%s,MERID=BOSS,SIGN=%s",
					vLoginAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					vLoginAccept,
					vSendDate,
					vSign);
	Trim(vSendMsg);
	printf("\n发放接口信息\n[%s]\n",vSendMsg);
	
	/*下发接口*/
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"2",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC", RspMsg, sizeof(RspMsg));
		if(strcmp(RspCode,"000000") == 0 )
		{
			vStatus = 1;/*发放成功，状态改为已发放*/
		}
		else
		{
			vStatus = 2;/*发放失败，状态改为发放失败*/
		}
	}
	else
	{
		strcpy(RspCode,"999999");
		sprintf(RspMsg,"电子券发放接口错误[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
	}
	Trim(RspCode);
	Trim(RspMsg);
	
	/*正常发放情况或丢失补发情况*/
	if(0 == SourceFlag || 2 == SourceFlag)
	{
		/*记录发送受理表，并记录发送结果*/
		EXEC SQL insert into dticketsend(max_accept,login_accept,id_no,phone_no,
	                                ACT_ID,PACKAGE_ID,CASH,request_time,
	                                send_date,send_time,rsp_code,rsp_msg,status)
		                     select max_accept,login_accept,id_no,phone_no,
		                            ACT_ID,PACKAGE_ID,CASH,request_time,
		                            :vSendDate,:vSendTime,:RspCode,:RspMsg,:vStatus
		                       from wticketsend
		                      where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"记录发送受理表错误![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -1;
		}
		
		/*只有正常发放情况时，修改一次赠送记录*/
		if(0 == SourceFlag)
		{
			/*修改已赠送记录*/
			EXEC SQL select bunch_no,to_char(b.id_no) 
			           into :vBunchNo,:vIdNo
					   from dCustPresent a,wticketsend b
					  where a.login_accept = b.max_accept
					    and b.login_accept = :vLoginAccept
					    and rownum < 2;
			if(SQLCODE!=0)
			{
				sprintf(ReternMsg,"记录赠送历史表错误![%ld]",vLoginAccept);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -2;
			}
			Trim(vBunchNo);
			Trim(vIdNo);
			
			EXEC SQL select sMaxSysAccept.nextval into :LoginAcceptTmp from dual;
				     	
			EXEC SQL insert into dCustPresentHis(id_no,phone_no,belong_code,
					fav_code,bunch_no,prepay_fee,fav_money,month_fav,month_num,
					contract_no,begin_time,end_time,fav_fee,login_accept,login_no,
					open_time,total_date,op_time,op_code,update_accept,update_time,
					update_date,update_login,update_type,update_code,channel,group_id,fav_type,ACT_ID,PACKAGE_ID)
				select id_no,phone_no,belong_code,
					fav_code,bunch_no,prepay_fee,fav_money,month_fav,month_num,
					contract_no,begin_time,end_time,fav_fee,login_accept,login_no,
					open_time,total_date,op_time,op_code,:LoginAcceptTmp,
					to_date((:vSendDate||' '||:vSendTime),'yyyymmdd hh24:mi:ss'),
					:vSendDate,'system','u','7879',channel,group_id,fav_type,ACT_ID,PACKAGE_ID
				from dCustPresent
			   where bunch_no = :vBunchNo
			   	 and rownum < 2;
			if(SQLCODE!=0)
			{
				sprintf(ReternMsg,"记录赠送历史表错误![%d][%ld]",SQLCODE,vLoginAccept);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -3;
			}
				
			strcpy(v_parameter_array[0],vCash);
			sprintf(v_parameter_array[1],"%s%s",vSendDate,vSendTime);
			strncpy(v_parameter_array[2],vSendDate,6);
			strcpy(v_parameter_array[3], vBunchNo);
	
			strcpy(update_sql, "fav_fee=fav_fee+to_number(:v1)/100,op_time=to_date(:v2,'yyyymmdd hh24:mi:ss'),	pres_total_date=to_number(:v3)");
	
			ret = 0;
			ret = OrderUpdateCustPresent(ORDERIDTYPE_USER,vIdNo, 100,"7879", LoginAcceptTmp,
					"system", "pTicketSnd", vBunchNo, vBunchNo, update_sql, "", v_parameter_array);
			if (ret != 0)
			{
				sprintf(ReternMsg,"修改赠送记录表错误![%s][%d]",vPhoneNo,ret);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -4;
			}
		}
	}
	/*日对账补发情况*/
	if(1 == SourceFlag)
	{
		/*修改受理发送记录*/
		EXEC SQL update dticketsend 
					set send_date = :vSendDate,
						send_time = :vSendTime,
						rsp_code = :RspCode,
						rsp_msg	 = :RspMsg,
						status = :vStatus
				  where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"修改发送信息错误![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -5;
		}		
		
		/*修改日对账差异记录表*/
		EXEC SQL update wCmpExcTicket
		            set deal_flag = :vStatus,
		            	rsp_code  = :RspCode,
		            	rsp_msg	 = :RspMsg
		          where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"修改日对账差异记录处理标识错误![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -6;
		}	
	}
	
	
	return 0;
}

/*************发撤销电子券接口************
  MCODE		功能码	    101810
  MID		业务流水号	每笔交易必须唯一
  DATE		交易日期	YYYYMMDD请求日期
  TIME		交易时间	hhmmss请求时间
  ACT_ID	批次号	    即建立电子券活动的活动号
  MBL_NO	手机号	
  BON_AMT	电子券金额	以分为单位
  MLOG_NO	电子券订单号
  TTXN_DT	电子券发放日期	YYYYMMDD
  MERID	    商户号	    测试使用888000730000009
  SIGN	    签名
****************************************/	
int pTicketCancelSnd(long vTicketAccept,long OldLoginAccept,char *RequestDate,char *RequestTime,char *ActId,
               char *PhoneNo,char *Cash,char *OldSendDate,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		int iCount = 0;
		char vPhoneNo[15+1];
		char vActId[50+1];		/*营销活动id*/
		char vCash[50+1];		/*面额,以分为单位*/
		char vRequestDate[8+1];	/*电子券发送请求日期*/
		char vRequestTime[6+1];	/*电子券发送请求时间*/
		char vOldSendDate[8+1];	/*发送时间 YYYYMMDD*/
		
		char vTemp[512];
		char vSign[172+1];		/*签名信息*/
		char vSendMsg[1024];	/*发送信息*/
		char retemp[256]; 		/*反馈信息*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vActId);
	init(vCash);
	init(vRequestDate);
	init(vRequestTime);
	init(vOldSendDate);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	strcpy(ReternMsg,"\0");
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vCash,Cash);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	strcpy(vActId,ActId);
	strcpy(vOldSendDate,OldSendDate);
	
	Trim(vPhoneNo);
	Trim(vCash);
	Trim(vRequestDate);
	Trim(vRequestTime);
	Trim(vActId);
	
	/*
	EXEC SQL select count(*) into :iCount 
		       from ddsmpordermsg
		      where phone_no =:vPhoneNo  
		        and serv_code ='54' 
		        and spid ='698000' 
		        and bizcode ='00000001'
				and sysdate between eff_time and end_time ;
	if(iCount <= 0)
	{
		sprintf(ReternMsg,"该用户未开通手机支付账户，不允许办理该业务[%s]",vPhoneNo);
		printf("ReternMsg[%s]\n",ReternMsg);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -1;
	}*/

	sprintf(vTemp,"MCODE=101820&MID=%ld&DATE=%s&TIME=%s&ACT_ID=%s&"
	              "MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%ld&TTXN_DT=%s&MERID=888000730000009",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					OldLoginAccept,
					vOldSendDate);

	printf("\n待签名信息[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"签名加密失败");
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101820,MID=%ld,DATE=%s,TIME=%s,ACT_ID=%s,"
	              "MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%ld,TTXN_DT=%s,"
	              "MERID=888000730000009,SIGN=%s",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					OldLoginAccept,
					vOldSendDate,
					vSign);
	Trim(vSendMsg);
	
	printf("\n撤销接口信息\n[%s]\n",vSendMsg);
		
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"4",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC", RspMsg, sizeof(RspMsg));
		/*P31A34为已撤销不允许重复撤销,SC6001为系统忙，请稍后再试*/
		if(strcmp(RspCode,"000000") != 0 &&strcmp(RspCode,"SC6001") != 0 && strcmp(RspCode,"P31A34") != 0)
		/*if(strcmp(RspCode,"000000") != 0)*/
		{
			sprintf(ReternMsg,"电子券撤销接口返回失败[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -3;
		}
	}
	else
	{
		sprintf(ReternMsg,"调电子券撤销接口错误[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -4;
	}

	/*修改原交易电子券状: 0未发放 1已发放 2发放失败 3已撤销*/
	EXEC SQL update dticketsend set status = 3
	          where login_accept = :OldLoginAccept;
	if(0 != SQLCODE)
	{
		sprintf(ReternMsg,"修改受理状态错误[%d][%ld]",SQLCODE,OldLoginAccept);
		EXEC SQL ROLLBACK;
		return -5;
	}
	          	
	/*记录撤销记录表 实时接口 撤销时间=请求时间*/
	EXEC SQL insert into wticketCancel(max_accept,oldlogin_accept,login_accept,phone_no,Cancel_time)
	                     select max_accept,login_accept,:vTicketAccept,phone_no,:vRequestDate||:vRequestTime
	                       from dticketsend
	                      where login_accept = :OldLoginAccept;
	if(0 != SQLCODE)
	{
		sprintf(ReternMsg,"记录撤销记录表错误[%ld][%d]",OldLoginAccept,SQLCODE);
		printf("记录撤销记录表错误![%ld][%d]\n",OldLoginAccept,SQLCODE);
		EXEC SQL ROLLBACK;
		return -6;
	}
	return 0;
}

/****每张电子券联机余额查询接口*****
	MCODE	功能码	101811
	MID		业务流水号	每笔交易必须唯一
	DATE	交易日期	YYYYMMDD
	TIME	交易时间	HHMMSS
	MERID	商户编号	商户接入时由省平台下发 测试使用888000730000009
	ACT_ID	活动编号	
	MBL_NO	手机号码	
	BON_AMT	电子券面额	单位：分
	MLOG_NO	发放订单号	16位	
	TTXN_DT	发放日期	
	SIGN	签名	172位
******************************/	
int pTicketQrySnd(char *ActId,char *PhoneNo,char *Cash,char *OldLoginAccept,char *SendDate,char *TicketName,char *Balance,char *EffDate,char *ExpDate,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		long vTicketAccept = 0;	/*交易流水*/
		char vPhoneNo[15+1];	/*手机号码*/
		char vActId[50+1];		/*活动id*/
		char vCash[50+1];		/*面值*/
		char vOldLoginAccept[14+1];/*原电子券订单号*/
		char vSendDate[8+1];	/*原发放日期*/
		
		char vDate[8+1];		/*交易日期 yyyymmdd*/
		char vTime[6+1];		/*交易时间 hh24miss*/
		char vTemp[512];
		char vSign[172+1];
		char vSendMsg[1024];	/*发送信息*/
		char retemp[256]; 		/*反馈信息*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vOldLoginAccept);
	init(vActId);
	init(vCash);
	init(vSendDate);
	init(vDate);
	init(vTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vOldLoginAccept,OldLoginAccept);
	strcpy(vActId,ActId);
	strcpy(vCash,Cash);
	strcpy(vSendDate,SendDate);
	
	Trim(vPhoneNo);
	Trim(vActId);
	Trim(vCash);
	Trim(vSendDate);
	
	EXEC SQL SELECT STICKETSYSACCEPT.nextVal INTO :vTicketAccept from dual;
	if(SQLCODE!=0 || vTicketAccept<=0)
	{
		strcpy(ReternMsg,"取电子券发放流水失败");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -1;
	}
	
	EXEC SQL SELECT to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss')
			   INTO :vDate,:vTime
		       from dual;
	if(SQLCODE!=0)
	{
		strcpy(ReternMsg,"获取系统时间错误");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -2;
	}
	
	sprintf(vTemp,"MCODE=101811&MID=%ld&DATE=%s&TIME=%s&MERID=888000730000009&"
				  "ACT_ID=%s&MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%s&TTXN_DT=%s", 
					vTicketAccept,
					vDate,
					vTime,
					vActId,	
					vPhoneNo,
					vCash,
					vOldLoginAccept,
					vSendDate);

	printf("\n待签名信息[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"签名加密失败");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101811,MID=%ld,DATE=%s,TIME=%s,MERID=888000730000009,"
				  "ACT_ID=%s,MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%s,TTXN_DT=%s,SIGN=%s", 
					vTicketAccept,
					vDate,
					vTime,
					vActId,	
					vPhoneNo,
					vCash,
					vOldLoginAccept,
					vSendDate,
					vSign);
	Trim(vSendMsg);
	
	printf("\n联机查询余额接口信息\n[%s]\n",vSendMsg);
	
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"5",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, 	sizeof(RspCode));
		mml_getvalue(retemp,"DESC",  RspMsg, 	sizeof(RspMsg));
		mml_getvalue(retemp,"BON_NM",TicketName,sizeof(TicketName));/*电子券名称*/		
		mml_getvalue(retemp,"CUR_AC_BAL",Balance,sizeof(Balance));	/*电子券余额*/		
		mml_getvalue(retemp,"EFF_DT",EffDate, 	sizeof(EffDate));	/*电子券生效时间*/		
		mml_getvalue(retemp,"EXP_DT",ExpDate, 	sizeof(ExpDate));	/*电子券失效时间*/		
		if(strcmp(RspCode,"000000") != 0)
		{
			sprintf(ReternMsg,"电子券余额接口返回错误[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
			return -3;	
		}
		Trim(TicketName);
		Trim(Balance);
		Trim(EffDate);
		Trim(ExpDate);
		printf("\nTicketName[%s],Balance[%s],EffDate[%s],ExpDate[%s]\n\n",TicketName,Balance,EffDate,ExpDate);
	}
	else
	{
		sprintf(ReternMsg,"调平台接口错误[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketQrySnd",  "","%s",ReternMsg);
		return -4;	
	}
	
	return 0;
}
/****查询手机号码可用电子券总余额*****
  MCODE		功能码	    101760
  MID		业务流水号	每笔交易必须唯一
  DATE		交易日期	YYYYMMDD请求日期
  TIME		交易时间	hhmmss请求时间
  MBL_NO	手机号	
  MERID	    商户号	    BOSS
  SIGN	    签名
******************************/	
int pTicketQryAllSnd(char *RequestDate,char *RequestTime,char *PhoneNo,char *Balance,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		long vTicketAccept = 0;	/*交易流水*/
		char vPhoneNo[15+1];
		char vRequestDate[8+1];	/*电子券发送请求日期*/
		char vRequestTime[6+1];	/*电子券发送请求时间*/
		
		char vTemp[512];
		char vSign[172+1];
		char vSendMsg[1024];	/*发送信息*/
		char retemp[256]; 		/*反馈信息*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vRequestDate);
	init(vRequestTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	
	Trim(vPhoneNo);
	Trim(vRequestDate);
	Trim(vRequestTime);
	
	EXEC SQL SELECT STICKETSYSACCEPT.nextVal INTO :vTicketAccept from dual;
	if(SQLCODE!=0 || vTicketAccept<=0)
	{
		strcpy(ReternMsg,"取电子券发放流水失败");
		pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
		return -1;
	}
	sprintf(vTemp,"MCODE=101760&MID=%ld&DATE=%s&TIME=%s&MERID=888000730000009&MOBILEID=%s", 
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vPhoneNo);

	printf("\n待签名信息[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"签名加密失败");
		pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101760,MID=%ld,DATE=%s,TIME=%s,"
	               "MERID=888000730000009,MOBILEID=%s,SIGN=%s",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vPhoneNo,
					vSign);
	Trim(vSendMsg);
	
	printf("\n根据手机号码查询总额接口信息\n[%s]\n",vSendMsg);
	
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"3",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC",  RspMsg, sizeof(RspMsg));
		mml_getvalue(retemp,"MER_RED_BAL",Balance, sizeof(Balance));
		if(strcmp(RspCode,"000000") != 0)
		{
			sprintf(ReternMsg,"电子券余额接口返回错误[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
			return -3;	
		}
		Trim(Balance);
	}
	else
	{
		sprintf(ReternMsg,"调平台接口错误[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketQryAllSnd",  "","%s",ReternMsg);
		return -4;	
	}
	
	return 0;
}


