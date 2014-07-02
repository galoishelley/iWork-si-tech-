#include "boss_srv.h"
#include "publicsrv.h"
#include "boss_define.h"
#include "funcParam.h"
#include "pubProduct.h"
#include "province.h"
#include "rtOweQry.h"

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;


int  GetLoginMsgEasy(char *InWorkNo,DLOGINMSG_TYPE *dLoginMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char  work_no[6+1];
	EXEC SQL   END DECLARE SECTION;

		init(work_no);
		strcpy(work_no,InWorkNo);

		EXEC SQL select login_no,login_name,password,org_code,org_code,group_id,
			sMaxPayAccept.nextval,to_char(sysdate,'YYYYMMDD HH24:MI:SS'),
			to_number(to_char(sysdate,'YYYYMMDD'))
			into  :dLoginMsg->login_no,
			:dLoginMsg->login_name,:dLoginMsg->password,
			:dLoginMsg->org_code,:dLoginMsg->org_id,:dLoginMsg->group_id,
			:dLoginMsg->login_accept_cx,:dLoginMsg->op_time,:dLoginMsg->total_date
			from dLoginMsg where login_no=:work_no;
		if(SQLCODE!=0)
		{
			printf("\n query dLoginMsg error [%s][%d],work_no=[%s]",SQLERRMSG,SQLCODE,work_no);
			return 139283;
		}

		return 0;
}


/***
取用户资料
****/
int  GetCustMsgAllEasy(char  *InPhoneNo,char *service_name,DCUSTMSG_TYPE *dCustMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char  phone_no[15+1];
		char  bill_code[8+1];
		int   RetValue=0;
		long  id_no=0;
	EXEC SQL   END DECLARE SECTION;

	
	init(phone_no);
	strcpy(phone_no,InPhoneNo);
	
	EXEC SQL select id_no, cust_id, contract_no, ids, phone_no, sm_code, 
			service_type, attr_code, user_passwd, open_time,
			belong_code, limit_owe, credit_code, credit_value,
  			run_code, to_char(run_time,'YYYYMMDD HH24:MI:SS'), bill_date, bill_type, nvl(encrypt_prepay,' '), cmd_right,
   			last_bill_type, nvl(bak_field,' '), nvl(group_id,' ')
   		 into  :dCustMsg->id_no, :dCustMsg->cust_id, :dCustMsg->contract_no, :dCustMsg->ids, :dCustMsg->phone_no, :dCustMsg->sm_code, 
			:dCustMsg->service_type, :dCustMsg->attr_code, :dCustMsg->user_passwd, :dCustMsg->open_time,
			:dCustMsg->belong_code, :dCustMsg->limit_owe, :dCustMsg->credit_code, :dCustMsg->credit_value,
  			:dCustMsg->run_code, :dCustMsg->run_time, :dCustMsg->bill_date, :dCustMsg->bill_type, 
  			:dCustMsg->encrypt_prepay, :dCustMsg->cmd_right,
   			:dCustMsg->last_bill_type, :dCustMsg->bak_field, :dCustMsg->group_id
   		from  dCustMsg  where phone_no=:phone_no;
	if(SQLCODE==1403)
	{
		printf("\n query dCustMsg  [%s][%d]",SQLERRMSG,SQLCODE);
		return  130267;	
		/***
		取用户的bill_code
		****/
	}
	else if(SQLCODE!=0)
	{
   			printf("\n query dCustMsg error[%s][%d]~~~~",SQLERRMSG,SQLCODE);
   			return  130267;		
	}
	/***
	取用户的bill_code
	****/
	
	printf("\nquery dcustmsg++++++++++&&&&&&&&&&&&&&&&  cust_id=[%ld],id_no=[%ld]",dCustMsg->cust_id,dCustMsg->id_no);
	
	init(bill_code);
	id_no=dCustMsg->id_no;
	RetValue=GetBillCode(id_no,bill_code);
	if(RetValue!=0)
	{
	      printf("\n quere bill_code [%d]",RetValue);
	      return RetValue;
	}	
	strcpy(dCustMsg->bill_code,bill_code);
		
	return 0;
}

int  GetBillCode(long  InIdNo,  char *OutBillCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	      char   TmpSqlStr[1000+1];
	      long   id_no=0;
	      char   phone_no[15+1];
	      char   sm_code[2+1];
	      char   belong_code[7+1];
	      char   bill_code[8+1];
	EXEC SQL   END DECLARE SECTION;	


	id_no=0;
	
	id_no=InIdNo;
	
	init(bill_code);
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"select mode_code from dBillCustDetail%ld \
	                        where id_no=:v1 \
	                        and   begin_time<=sysdate\
	                        and   end_time>sysdate\
	                        and   mode_flag='0' and mode_time='Y' and rownum < 2",
	                        id_no%10);

        EXEC SQL EXECUTE
        BEGIN
                EXECUTE IMMEDIATE :TmpSqlStr into : bill_code using :id_no;
        END;
        END-EXEC;
/**huhx modify******
	if(SQLCODE!=0)
**/
	if(SQLCODE!=0 && SQLCODE != 1403)
	{
	                printf("\n  TmpSql=[%s][%ld]",TmpSqlStr,id_no);
	                printf("dBillCustDetail  Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
	                return  139003;
	}

	if ( SQLCODE==1403)
		strcpy(bill_code,"00000000");

	strcpy(OutBillCode,bill_code);
	return 0;
}

int InsertPayEasy(DCUSTMSG_TYPE *dCustMsg,DCONMSG_TYPE *dConMsg,DLOGINMSG_TYPE *dLoginMsg,double InPrepayFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int total_date = 0;
		long id_no = 0;
		long contract_no = 0;
		long cust_id = 0;
		long pay_accept = 0;
		double used_prepay = 0.00;
		double used_pay = 0.00;
		double new_prepay = 0.00;
		double cancel_owe = 0.00;
		double cancel_remonth = 0.00;
		double cancel_delay = 0.00;
		double tmp_fee = 0.00;
		double add_money = 0.00;
		double all_cur_prepay = 0.00;
		char phone_no[15+1];
		char pay_type[1+1];
		char op_code[4+1];
		char op_note[60+1];
		char work_no[6+1];
		char org_code[9+1];
		char org_id[10+1];
		char belong_code[7+1];
		char group_id[10+1];
		char pay_time[19+1];
		char fetch_no[8+1];
		char sm_code[2+1];
		char TmpSqlStr[1000];
		
		char GroupId[10+1];

	EXEC SQL   END DECLARE SECTION;

	init(phone_no    );
	init(pay_type    );
	init(op_code     );
	init(op_note     );
	init(work_no     );
	init(org_code    );
	init(org_id      );
	init(belong_code );
	init(group_id    );
	init(pay_time    );
	init(sm_code     );
	init(fetch_no    );
	init(GroupId	 );

	
	strcpy(fetch_no,"000000");
	
	contract_no=dConMsg->contract_no;
	id_no=dCustMsg->id_no;
	total_date=dLoginMsg->total_date;
	pay_accept=dLoginMsg->login_accept;
	cust_id=dCustMsg->cust_id;
	
	if(dCustMsg==NULL)
	{
		strcpy(phone_no,"99999999999");
		strcpy(belong_code,dConMsg->belong_code);
		strcpy(sm_code,"ss");
	}
	else
	{
		strcpy(phone_no,dCustMsg->phone_no);
		strcpy(belong_code,dCustMsg->belong_code);
		strcpy(sm_code,dCustMsg->sm_code);
	}
	strcpy(work_no,dLoginMsg->login_no);

	printf("11111111 dLoginMsg->op_time=[%s]\n",dLoginMsg->op_time);
	strcpy(pay_time,dLoginMsg->op_time);
	printf("22222222 pay_time=[%s]\n",pay_time);
	strcpy(op_code,dLoginMsg->op_code);
	strcpy(pay_type,dLoginMsg->pay_type);
	add_money=InPrepayFee;
	used_pay=InPrepayFee;
	used_prepay=0.00;
	new_prepay=InPrepayFee;
	all_cur_prepay=dConMsg->prepay_fee;
	tmp_fee=0.00;
	strcpy(org_code,dLoginMsg->org_code);
	strcpy(op_note,dLoginMsg->op_note);
	strcpy(group_id,dLoginMsg->group_id);
	strcpy(org_id,dLoginMsg->org_id);
	strcpy(GroupId,dCustMsg->group_id);
	Trim(GroupId);
	
	init(TmpSqlStr);
/**/
	pay_time[19]='\0';
/***/
		sprintf(TmpSqlStr,"Insert into wPay%d(contract_no,id_no,total_date,login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code,login_no,org_code, op_time,\
		               op_code,pay_type,add_money,pay_money,out_prepay,prepay_fee, current_prepay,payed_owe,delay_fee,other_fee,dead_fee, bad_fee,back_flag,encrypt_fee,\
		               return_code,pay_note,org_id,group_id )values(%ld,%ld,%d,%ld,%ld,'%s','%s','%s','%s','%s','%s',to_date('%s','YYYYMMDD HH24:MI:SS'),\
		               '%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,'0','NULL','00','%s','%s','%s')",total_date/100,contract_no,id_no,total_date,pay_accept,cust_id,phone_no,belong_code,fetch_no,sm_code,work_no,org_code,pay_time,op_code,pay_type,add_money,used_pay,used_prepay,new_prepay,all_cur_prepay,tmp_fee,tmp_fee,tmp_fee,tmp_fee,tmp_fee,op_note,org_id,GroupId);
		
		printf("TmpSqlStr=[%s]\n", TmpSqlStr);
		
		EXEC SQL EXECUTE IMMEDIATE :TmpSqlStr;		

		if(SQLCODE != 0)
		{
			printf("hhx add[%s][%ld][%ld][%ld][%s]\n",TmpSqlStr,contract_no,id_no,pay_accept,pay_type);        
			printf("hhx [%ld][%.2f][%.2f][%.2f][%.2f][%.2f][%.2f][%ld]\n", cust_id, add_money,used_pay,used_prepay,new_prepay,all_cur_prepay, tmp_fee, pay_accept);
			printf("\nPAY_TIME=[%s]\n",pay_time);
			printf("插入帐户交费表出错[%d],[%s]\n",SQLCODE,SQLERRMSG);
			printf("values(%ld,%ld,%d/100,%ld,%ld,'%s','%s','%s','%s','%s','%s',to_date('%s','YYYYMMDD HH24:MI:SS'),'%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,'0','NULL','00','%s')\n",contract_no,id_no,total_date,pay_accept,cust_id,phone_no,belong_code,fetch_no,sm_code,work_no,org_code,pay_time,op_code,pay_type,add_money,used_pay,used_prepay,new_prepay,all_cur_prepay,tmp_fee,tmp_fee,tmp_fee,tmp_fee,tmp_fee,op_note);
			return -1;
		}	
		return 0;
}

int GetConMsgEasy(long InContractNo,char *service_name,DCONMSG_TYPE *dConMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long   contract_no;
		char   pay_type[1+1];
		char   query_type[1+1];
		double prepay_fee=0.00;
		int    RetValue=0;
	EXEC SQL   END DECLARE SECTION;	
		contract_no=InContractNo;

			
	EXEC SQL select contract_no, con_cust_id, contract_passwd, bank_cust, oddment, 
			belong_code, prepay_fee, prepay_time, status, status_time, 
			post_flag, deposit, min_ym, owe_fee, 
			account_mark, account_limit, pay_code, nvl(bank_code,' '), 
			nvl(post_bank_code,' '), account_no, account_type
		into    :dConMsg->contract_no, :dConMsg->con_cust_id, :dConMsg->contract_passwd, :dConMsg->bank_cust, :dConMsg->oddment, 
			:dConMsg->belong_code, :dConMsg->prepay_fee, :dConMsg->prepay_time, :dConMsg->status, :dConMsg->status_time, 
			:dConMsg->post_flag, :dConMsg->deposit, :dConMsg->min_ym, :dConMsg->owe_fee, 
			:dConMsg->account_mark, :dConMsg->account_limit, :dConMsg->pay_code, :dConMsg->bank_code, 
			:dConMsg->post_bank_code, :dConMsg->account_no, :dConMsg->account_type
		 from  dConMsg
		 where contract_no=:contract_no;
        if(SQLCODE==1403)
        {
						EXEC SQL select contract_no, con_cust_id, nvl(contract_passwd,' '), bank_cust, oddment, 
								belong_code, prepay_fee, prepay_time, status, status_time, 
								post_flag, deposit, min_ym, owe_fee, 
								account_mark, account_limit, pay_code, bank_code, 
								post_bank_code, account_no, account_type
							into    :dConMsg->contract_no, :dConMsg->con_cust_id, :dConMsg->contract_passwd, :dConMsg->bank_cust, :dConMsg->oddment, 
								:dConMsg->belong_code, :dConMsg->prepay_fee, :dConMsg->prepay_time, :dConMsg->status, :dConMsg->status_time, 
								:dConMsg->post_flag, :dConMsg->deposit, :dConMsg->min_ym, :dConMsg->owe_fee, 
								:dConMsg->account_mark, :dConMsg->account_limit, :dConMsg->pay_code, :dConMsg->bank_code, 
								:dConMsg->post_bank_code, :dConMsg->account_no, :dConMsg->account_type
							 from  dConMsgDead
							 where contract_no=:contract_no;       
        }
        /*** else ***/
			if(SQLCODE!=0)
      {
	   			printf("PublicGetConMsg_ERR:[%d][%s][%ld]", sqlca.sqlcode, SQLERRMSG,contract_no);
	   			return  130255;	        
      }
        
	 strcpy(pay_type,"*");
	 strcpy(query_type,"*");
	 RetValue=QueryPrepayEasy(contract_no,pay_type,query_type,&prepay_fee);
	 if(RetValue!=0)
	 {
		 	printf("\n PublicQueryPrepay error~~~");
		 	return RetValue;
	 }
	 dConMsg->prepay_fee=prepay_fee;
	 printf("\n---------aaa----------[%.2f][%.2f]\n",dConMsg->prepay_fee,dConMsg->owe_fee);
	 printf("\nbank_cust=[%s]",dConMsg->bank_cust);
	return 0;
}

int UpdateAccountPrepay(long   InContractNo, char *InPayType, 
                              double InPrepay,     char *InSysTime, 
                              double InPayedOwe)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char   TmpSqlStr[1000+1];
		char   SysTime[19+1]; 
		char   PayType[1+1];
		int    vTotalDate = 0;
		double PrePay = 0.00;
		double PayedOwe = 0.00; 
		long   ContractNo = 0; 
		int    min_ym=0;
	EXEC SQL   END DECLARE SECTION;
	init(TmpSqlStr);
	init(SysTime);
	init(PayType);
	PrePay = InPrepay;
	PayedOwe = InPayedOwe;
	ContractNo = InContractNo;
	strcpy(SysTime,InSysTime);
	strcpy(PayType,InPayType);
	
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"UPDATE dConMsg SET prepay_fee=prepay_fee+round(:v1,2),\
	                          owe_fee=owe_fee-round(:v2,2),\
	                          prepay_time=to_date(:v3,'YYYYMMDD HH24:MI:SS') \
	                   WHERE  contract_no=:v4");
	                   
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt USING :PrePay,:PayedOwe,:SysTime,:ContractNo;
	if (sqlca.sqlcode != 0) 
	{
		#ifdef _DEBUG_
			printf("<PublicUpdateAccountPrepay>[%s]\n", TmpSqlStr);
			printf("<PublicUpdateAccountPrepay>[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
		#endif
		return 131001;
	}
	if((InPrepay<0.005) && (InPrepay > -0.005))
	{
		return 0;
	}
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"UPDATE dConMsgPre \
                       SET    last_prepay=prepay_fee,\
                              prepay_fee=prepay_fee+round(:v1,2),\
                              add_prepay=round(:v2,2) \
                       WHERE  contract_no=:v3 \
                       AND    pay_type=:v4");
    printf("PayType[%s]\n",PayType);                   
	EXEC SQL PREPARE sql_stmt From :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt USING :PrePay,:PrePay,:ContractNo,:PayType;
	
	if (sqlca.sqlcode != 0) 
	{
		if (sqlca.sqlcode == 1403) 
		{
			GetCurDate(&vTotalDate);
	        init(TmpSqlStr);
	        sprintf(TmpSqlStr,"INSERT INTO dConMsgPre(contract_no,pay_type,\
	        						  prepay_fee,last_prepay,add_prepay,\
	        						  live_flag,allow_pay,begin_dt,end_dt) \
	                           VALUES(:v1,:v2,round(:v3,2),0,round(:v4,2),'1',\
	                                  0,:v5,'20500101')");

	        EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	        EXEC SQL EXECUTE sql_stmt USING :ContractNo,:PayType,:PrePay,:PrePay,
	                                  :vTotalDate;
	        if (sqlca.sqlcode != 0)
	        { 
				printf("<PublicUpdateAccountPrepay>[%s]\n", TmpSqlStr);
				printf("<PublicUpdateAccountPrepay>[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
				return 132001;
			}
        }
		else
		{
			printf("<PublicUpdateAccountPrepay>[%s]\n", TmpSqlStr);
			printf("<PublicUpdateAccountPrepay>[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
			return 131002;
		}
	}
	return 0;
}

int dcustSendMsg(char *phone_no,char *login_no,char *op_code)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	phoneNo[15+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	vloginNo[6+1];
	int		mcount=0;
	
	EXEC SQL END DECLARE SECTION;
	
	init(phoneNo);
	init(opCode);
	init(loginNo);
	init(vloginNo);
	
	strcpy(phoneNo,phone_no);
	strcpy(opCode,op_code);
	strcpy(loginNo,login_no);
	
	if(strlen(phoneNo) == 0)
	{
		return 0;
	}
	
	EXEC SQL SELECT count(*) INTO :mcount FROM dloginmsg WHERE person_id=1000028386 
			 AND power_code='0117' AND login_no <>'www000' 
			 AND login_no=:loginNo;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		printf("Fail---查询dcustSendMsg_dloginmsg[%d]\n",SQLCODE);
		return -1;
	}
	
	if(mcount > 0)
	{
		strcpy(vloginNo,"auto00");
	}
	else
	{
		return 0;
	}
	
	EXEC SQL INSERT INTO WCOMMONSHORTMSGTX(COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,
    	   				PHONE_NO,MSG,ORDER_CODE,BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME)
    	   	 SELECT SMSG_SEQUENCE.nextval,sMaxSysAccept.nextVal,1,:phoneNo,send_msg,
    	   			0,0,:opCode,:loginNo,sysdate 
    	   			FROM sfuncopsendmsg
    	   			WHERE substr(func_code,1,4)='1300'
    	   			AND op_type='a'
    	   			AND op_login=:vloginNo;
    if(SQLCODE!=0 && SQLCODE!=1403)
    {
    	printf("发送提醒短信错误[%d]\n",SQLCODE);
    	return -2;
    }
    
    return 0;
}


int pubGetPhoneFee(long InIdNo, long InContractNo,int InQueryYm,int InDelayFlag,int InDelayBegin,
                      float InDelayRate,char *InRunFlag,char *InBillFlag, double *OutOweFee, double *OutDelayFee,
                      double *OutShouldPay,double *OutFavourFee,double *OutPayedPrepaye,double *OutPayedLater,int *OutDataFlag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int   vQueryYm;
		int   vDelayFlag=0;
		int   vDelayBegin=0;
		int   vCurYm=0; 
		long  vContractNo=0;
		long  vIdNo=0;
		char  OweTableName[30+1];
		char  TmpSqlStr[TMPLEN+1];
		float vDelayRate=0.00;
		double vOweFee=0.00;
		double vDelayFee=0.00;
        double vShouldPay=0.00;
        double vFavourFee=0.00;
        double vPayedPrepaye=0.00;
        double vPayedLater=0.00;
        int vDataFlag=0;
		char   vRunFlag[1+1];
        char   vBillFlag[1+1];
		int    vTotalDate=0;
		int    vLastYm=0;
		int    data_flag=0;
	EXEC SQL END DECLARE SECTION;
	
	init(vRunFlag);
    init(vBillFlag);
    
	vContractNo = InContractNo;
	vIdNo = InIdNo;
	vQueryYm=InQueryYm;
	vDelayFlag=InDelayFlag;
	vDelayBegin=InDelayBegin;
	vDelayRate=InDelayRate;
	strcpy(vRunFlag,InRunFlag);
    strcpy(vBillFlag,InBillFlag);

	Trim(vRunFlag);
	Trim(vBillFlag);
	
	EXEC SQL select to_number(to_char(sysdate,'yyyymmdd')),to_number(to_char(sysdate,'yyyymm')),to_number(to_char(sysdate,'yyyymm'))
					into :vTotalDate,:vCurYm,:vLastYm
					from dual;
	if (SQLCODE !=0 ) 
	{
		printf("取系统时间错误,errcode:%d,errmsg:%s\n",SQLCODE,SQLERRMSG);
		return -1;
	}
	/*如果是每月1日，则出账月是上月*/
	if(vTotalDate%100==1) 
	{
		vCurYm=vLastYm;
	}
	
	init(OweTableName);
	sprintf(OweTableName, "dCustOweTotal%6d%ld",vQueryYm,vContractNo%10);
	init(TmpSqlStr);    
	printf("vQueryYm=[%d],vCurYm=[%d]\n",vQueryYm,vCurYm);
	if(vQueryYm==vCurYm&&strcmp(vBillFlag,"1")==0)              
	{
		sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0),\
		                   nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0), \
		                   nvl(sum(should_pay),0),nvl(sum(favour_fee),0), nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
		                   from %s \
		                   where id_no=:v5  and contract_no=:v6  \
		                   and payed_status=any('0','9') \
		                   and (bill_day<3600 or bill_day>=9000) ", OweTableName);

    	EXEC SQL EXECUTE
    	BEGIN
    	    EXECUTE IMMEDIATE :TmpSqlStr INTO :vOweFee,:vDelayFee,:vShouldPay,:vFavourFee,
						                      :vPayedPrepaye,:vPayedLater 
						                using :vDelayFlag,:vDelayBegin,:vDelayBegin,:vDelayRate,:vIdNo,:vContractNo;

    	END;
    	END-EXEC;
				
        if(SQLCODE==NOTFOUND)
        {
        	*OutOweFee=vOweFee;
			*OutDelayFee=vDelayFee;
			*OutShouldPay=vShouldPay;
			*OutFavourFee=vFavourFee;
			*OutPayedPrepaye=vPayedPrepaye;
			*OutPayedLater=vPayedLater;
			*OutDataFlag=data_flag;
        	return 0;
        }  
		if (SQLCODE != SQL_OK && SQLCODE != CNO_MORE_ROWS) 
		{
			printf("TmpSqlStr[%s]\n",TmpSqlStr);
			printf("取用户帐单信息错误,errcode:%d,errmsg:%s\n",SQLCODE,SQLERRMSG);
			return -1;
		}
		if (SQLCODE != CNO_MORE_ROWS) 
		{
			data_flag=1;
		}
	}
	else        
	{
		sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0),\
		                   nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0), \
		                   nvl(sum(should_pay),0),nvl(sum(favour_fee),0), nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
		                   from %s \
		                   where id_no=:v5  and contract_no=:v6  \
		                   and payed_status=any('0','9') ", 
		                   OweTableName);

    	EXEC SQL EXECUTE
    	BEGIN
    	    EXECUTE IMMEDIATE :TmpSqlStr INTO :vOweFee,:vDelayFee,:vShouldPay,:vFavourFee,
						                      :vPayedPrepaye,:vPayedLater 
						                using :vDelayFlag,:vDelayBegin,:vDelayBegin,:vDelayRate,:vIdNo,:vContractNo;

    	END;
    	END-EXEC;
				
        if(SQLCODE==NOTFOUND)
        {
        	*OutOweFee=vOweFee;
			*OutDelayFee=vDelayFee;
			*OutShouldPay=vShouldPay;
			*OutFavourFee=vFavourFee;
			*OutPayedPrepaye=vPayedPrepaye;
			*OutPayedLater=vPayedLater;
			*OutDataFlag=data_flag;
        	return 0;
        }  
		if (SQLCODE != SQL_OK && SQLCODE != CNO_MORE_ROWS) 
		{
			printf("TmpSqlStr[%s]\n",TmpSqlStr);
			printf("取用户帐单信息错误,errcode:%d,errmsg:%s\n",SQLCODE,SQLERRMSG);
			return -1;
		}
		if (SQLCODE != CNO_MORE_ROWS) 
		{
			data_flag=1;
		}
	}
	*OutOweFee=vOweFee;
	*OutDelayFee=vDelayFee;
	*OutShouldPay=vShouldPay;
	*OutFavourFee=vFavourFee;
	*OutPayedPrepaye=vPayedPrepaye;
	*OutPayedLater=vPayedLater;
	*OutDataFlag=data_flag;
	return 0;
}







/*
 * 函数名称:bodc_ServiceOpen
 * 功能描述:服务开通函数，本函数主要对BOSS侧缴费、停机程序产生的工单进行CRM侧用户状态更新及开关机处理，
 	本函数主要用于替换GoStop和实时停机程序中对于用户信息表以及wChgList表的处理
 * 输入参数:
	char 	in_para[MAX_PARMS_NUM][200]	输入参数
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

int bodc_ServiceOpen(char in_para[MAX_PARMS_NUM][200], char *v_ret_code, char *v_ret_msg)
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
