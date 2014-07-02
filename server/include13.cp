/************ please set tab = 4 first *******************
文  件:  include13.cp
作  者:  yangbo@si-tech.com.cn
日  期:  2003/09/09
*********************************************************/
#include "boss_srv.h"
#include "publicsrv.h"
#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;
/*******************************
功能：
	对小数点后num位进行四舍五入
输入：
	source    double  源数值
	num       int     精度
输出：
	tmpdouble double  四舍五入后数值
********************************/
	 
double round_F(double source, int num)
{
	char tmpstr[64+1];
	char tmp1[12+1];
	double tmpdouble = 0.00;
	
	memset(tmpstr, 0, sizeof(tmpstr));
	memset(tmp1, 0, sizeof(tmp1));
	
	sprintf(tmp1, "%%12.%df", num);
	sprintf(tmpstr, tmp1, source);
	tmpdouble = atof(tmpstr);
	return tmpdouble;
}


/*************************************
功能:
	读取默认帐户的预存款和付款方式
输入参数：
InContractNo     long     帐户号码
输出参数：
*OutPrepayFee    double   帐户预存款
*OutPayCode      char     付款方式
返回码：
	130008  查询帐户信息表出错
**************************************/
int GetAccountMsgOwe(long InContractNo, double *OutPrepayFee, char *OutPayCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
		double  PrepayFee = 0.00;
		double  AccountLimit = 0.00;
		char    PayCode[1+1];
		long    ContractNo = 0;
	EXEC SQL END DECLARE SECTION;
	
	ContractNo = InContractNo;
	init(PayCode);
	
	EXEC SQL SELECT  prepay_fee,pay_code  
	         INTO    :PrepayFee,:PayCode
	         FROM    dConMsg  
	         WHERE   contract_no=:ContractNo;
	if (sqlca.sqlcode != 0) 
	{
		if (sqlca.sqlcode == 1403)
		{
			printf("<GetAccountMsgOwe>没有找到帐户信息\n");
			return 130008;
		}
		else
		{
			printf("<GetAccountMsgOwe>查询帐户信息表出错\n");
	  		return 130008;
	  	}
	}   
	*OutPrepayFee = PrepayFee;
	strcpy(OutPayCode, PayCode);
    return 0;
}


/********************************************************
功能:
	默认帐户的预存款和付款方式
输入参数：
	InContractNo      long    帐户号码
输出参数：
	*OutPrepayFee     double  帐户预存款
	*OutPayCode       char    付款方式
	*OutMinYm         int     最小欠费年月
	*OutBelongCode    char    归属代码
	*OutDelayFlag     int     滞纳金标志
	*OutDelayRate     float   滞纳金率
	*OutDelayBegin    int     开始天数
返回码：
	130008  查询帐户信息表出错
	130009  查询滞纳金参数出错
********************************************************/
int GetAccountMsgOweAll(long  InContractNo,   double *OutPrepayFee,
						char  *OutPayCode,    int    *OutMinYm,
						char  *OutBelongCode, int    *OutDelayFlag,
						float *OutDelayRate,  int    *OutDelayBegin)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int     delay_flag = 0;
		int     delay_begin = 0; 
		int     MinYm = 0;		
		long    ContractNo = 0;
		float   delay_rate = 0; 
		double  AccountLimit = 0.00;
		double  PrepayFee = 0.00;
		char    BelongCode[7+1];
		char    PayCode[1+1];
	EXEC SQL   END DECLARE SECTION;
	ContractNo = InContractNo;
	init(PayCode);
	
	EXEC SQL SELECT  prepay_fee,pay_code,min_ym,belong_code  
			 INTO    :PrepayFee,:PayCode,:MinYm,:BelongCode
	         FROM    dConMsg  
	         WHERE   contract_no=:ContractNo;
	if (sqlca.sqlcode != 0) 
	{
		if (sqlca.sqlcode == 1403)
		{
		    return 130008;
		}
		else
		{
			return  130008;
		}
	} 
	delay_flag = 0;
	delay_rate = 0;
	delay_begin = 0;
	
	/*** 取用户滞纳金指标 ***/
    EXEC SQL SELECT to_number(delay_flag),delay_rate,delay_begin
			 INTO   :delay_flag,:delay_rate,:delay_begin
			 FROM   sPayCode
			 WHERE  pay_code=:PayCode
			 AND    region_code=substr(:BelongCode,1,2);
    if (SQLCODE != SQL_OK) 
    {
		return 130009;
    }  
	*OutDelayFlag = delay_flag;
	*OutDelayRate = delay_rate;
	*OutDelayBegin = delay_begin;
	*OutPrepayFee = PrepayFee;
	
	strcpy(OutPayCode, PayCode);
	*OutMinYm = MinYm;
	strcpy(OutBelongCode, BelongCode);
    return 0;
}


/*******************************************
功能:
	取得默认帐户的预存款和付款方式
输入参数：
	InContractNo     long   帐户号码
输出参数：
	*OutMinYm        int    最小欠费年月
	*OutPayCode      char   付款方式
	*OutBelongCode   char   归属代码
返回码：
	130008  查询帐户信息表出错
********************************************/
int GetAccountMinYm(long InContractNo, int  *OutMinYm,
					char *OutPayCode,  char *OutBelongCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int     MinYm=0;
		long    ContractNo=0;
		char    PayCode[1+1];
		char    BelongCode[7+1];
	EXEC SQL END DECLARE SECTION;
	ContractNo = InContractNo;
	init(PayCode);
	init(BelongCode);
	
	EXEC SQL SELECT  min_ym,pay_code,belong_code  
			 INTO    :MinYm,:PayCode,:BelongCode
	         FROM    dConMsg  
	         WHERE   contract_no=:ContractNo;
	if (sqlca.sqlcode != 0) 
	{
		if (sqlca.sqlcode == 1403)
		{
			return 130007;
		}
		else
		{
			return  130008;
		}
	}   
	*OutMinYm = MinYm;
	strcpy(OutPayCode, PayCode);
	strcpy(OutBelongCode, BelongCode);
    return 0;
}



/***************************************
功能:
	更新用户的运行状态
输入参数：
	InIdNo         long    用户ID
	*InRunCode     char    运行状态代码
	*InRunTime     char    更新时间
输出参数：
返回代码：
	131004 更新用户资料表出错
***************************************/
int PublicUpdateCustMsgRunCode(long InIdNo, char *InRunCode, char *InRunTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long IdNo = 0;
		char RunCode[RUNCODELEN+RUNCODELEN+1];
		char RunTime[19+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(RunCode);
	Sinitn(RunTime);
	
	IdNo = InIdNo;
	strcpy(RunCode, InRunCode);
	strcpy(RunTime, InRunTime);
	
	EXEC SQL UPDATE dCustMsg
	         SET    run_code=:RunCode,
	                run_time=to_date(:RunTime,'YYYYMMDD HH24:MI:SS')
	         WHERE  id_no=:IdNo;
	
	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicUpdateCustMsgRunCode>更新用户状态错误！\n");
		return 131004;
	}
	return RET_OK;
}


/**********************************************************************
功能：
	查找用户欠费（按照手机号码查找用户欠费,只是在库中的欠费不包括内存帐单）
输入参数：
输出参数：
	InIdNo               long     用户ID
	*OutStillOwe         double   欠费
	*OutRemonthFee       double   补收月租
	*OutDelayFee         double   滞纳金
	*OutOweMonth         int      欠费月数
	*OutShouldPay        double   应收
	*OutFavourFee        double   优惠
	*OutPayedPrepay      double   预存划拨
	*OutPayedLater       double   新交款
返回结果：
***********************************************************************/
int GetPhoneOwe(long   InIdNo,         double *OutStillOwe,
                double *OutRemonthFee ,double *OutDelayFee,
                int    *OutOweMonth,   double *OutShouldPay,
                double *OutFavourFee,  double *OutPayedPrepay, 
                double *OutPayedLater)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int    MinYm;
		int    delay_flag=0;
		int    delay_begin=0;
		int    CurYm=0; 
		int    MinMinYm=0;
		int    RetCode=0; 
		long   ContractNo=0;
		long   IdNo=0;
		char   OweTableName[30+1];
		char   PayCode[1+1];
		char   BelongCode[7+1];
		char   TmpSqlStr[TMPLEN+1];
		float  delay_rate=0.00;
		double ShouldPay=0.00;
		double FavourFee=0.00;
		double PayedPrepay=0.00;
		double PayedLater=0.00;
		double StillOwe=0.00;
		double RemontFee=0.00;
		double DelayFee=0.00;
		double fShouldPay=0.00;
		double fFavourFee=0.00;
        double fPayedPrepay=0.00;
		double fPayedLater=0.00;
		double fStillOwe=0.00;
		double fRemontFee=0.00;
		double fDelayFee=0.00;    
	EXEC SQL   END DECLARE SECTION;
	
    /***判断用户ID是否存在于用户帐户关系表中**/
    /***查找用户相关的帐户***/
    /***取当前月数***/
	CurYm = GetCurYm();
	MinMinYm = CurYm;
	IdNo = InIdNo;
	init(TmpSqlStr);
	init(OweTableName);
	init(PayCode);
	init(BelongCode);
    printf("\n 111111111111111111111111");
	sprintf(TmpSqlStr,"select contract_no  from dConUserMsg  \
					   where id_no=:v1");

	EXEC SQL PREPARE GetPhoneOweStr1  FROM :TmpSqlStr;
	EXEC SQL DECLARE GetPhoneOweCur1 CURSOR FOR GetPhoneOweStr1;
	EXEC SQL OPEN GetPhoneOweCur1 using :IdNo;
	for(;;)
	{
		EXEC SQL FETCH GetPhoneOweCur1 INTO :ContractNo;
		if (SQLCODE==NOTFOUND || SQLCODE<0)
		{
			break;
		}
		init(PayCode);
		init(BelongCode);
		MinYm = 0;
		RetCode = GetAccountMinYm(ContractNo,&MinYm,PayCode,BelongCode);
		if(RetCode != SQL_OK)
		{
			printf("<GetPhoneOwe>TmpSqlStr=[%s]\n", TmpSqlStr);
			EXEC SQL CLOSE GetPhoneOweCur1;
			return 9991;
		}
		if(MinMinYm > MinYm)
		{
			MinMinYm = MinYm;
		}
		delay_flag = 0;
		delay_rate = 0;
		delay_begin = 0;
		
		/***调用取用户滞纳金参数***/
		EXEC SQL SELECT to_number(delay_flag),delay_rate,delay_begin
 				 INTO   :delay_flag,:delay_rate,:delay_begin
        		 FROM   sPayCode
                 WHERE  pay_code=:PayCode
                 AND    region_code=substr(:BelongCode,1,2);
		if (SQLCODE != SQL_OK) 
		{
			EXEC SQL CLOSE GetPhoneOweCur1;
			printf("TmpSqlStr[%s]\n",TmpSqlStr);
			return 9992;
		}
		for (;;)
		{
			if (MinYm > CurYm)  
			{
				break;
			}
			printf("\nMinYm=[%d],[%d]",MinYm,CurYm);
			init(OweTableName);
			sprintf(OweTableName, "dCustOweTotal%6d%ld", MinYm, ContractNo%10);
		  
			ShouldPay = 0.00;  
			FavourFee = 0.00;  
			PayedPrepay = 0.00;
			PayedLater = 0.00; 
			StillOwe = 0.00;   
			RemontFee = 0.00;     
			DelayFee = 0.00;  
		
			init(TmpSqlStr);                   
			sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0),\
		                   nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0), \
		                   nvl(sum(should_pay),0),nvl(sum(favour_fee),0), nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
		                   from %s \
		                   where id_no=:v5  and contract_no=:v6  \
		                   and payed_status=any('0','9') ",OweTableName);
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :TmpSqlStr INTO :StillOwe,:DelayFee,:ShouldPay,:FavourFee,:PayedPrepay,:PayedLater
				                             using :delay_flag,:delay_begin,:delay_begin,:delay_rate,:IdNo,:ContractNo;
			END;
			END-EXEC;
			if (SQLCODE != SQL_OK) 
			{
 				EXEC SQL CLOSE GetPhoneOweCur1;
 				printf("TmpSqlStr[%s]\n",TmpSqlStr);
				return 9993;
			}
			      
			/***取补收月租费的函数***/
			fShouldPay   = fShouldPay   + ShouldPay   ;   
			fFavourFee   = fFavourFee   + FavourFee   ;   
			fPayedPrepay = fPayedPrepay + PayedPrepay ; 
			fPayedLater  = fPayedLater  + PayedLater  ;  
			fStillOwe    = fStillOwe    + StillOwe    ;    
			fRemontFee   = fRemontFee   + RemontFee   ;   
			fDelayFee    = fDelayFee    + DelayFee    ;    
	        printf("\n 222222222222222222");
			MinYm = PublicAddMon(MinYm);
	        printf("\n 3333333333333333");
			printf("\nMinYm=[%d]",MinYm);
		}
	}
	EXEC SQL CLOSE GetPhoneOweCur1;
	printf("\n aaaaaaaaaabbbbbbbbbbbbbbbb");
	*OutStillOwe = fStillOwe;
	*OutDelayFee = fDelayFee;
	*OutRemonthFee = 0.00;
	*OutOweMonth = CurYm - MinMinYm;
	*OutShouldPay = fShouldPay;
	*OutFavourFee = fFavourFee;
	*OutPayedPrepay = fPayedPrepay;
	*OutPayedLater = fPayedLater;
	printf("\n aaaaaaaaaabbbbbbbbbbbbbbbb");
	return 0;
}

/**********************************************************************
功能：
	查找用户某个帐号的滞纳金
输入参数：
输出参数：
	InIdNo               long     用户ID
	InContractNo         long     帐号
	*OutDelayFee         double   滞纳金
返回结果：
***********************************************************************/
int GetPhoneDelay(long InIdNo, long InContractNo, double *OutDelayFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int    MinYm = 0;
		int    delay_flag=0;
		int    delay_begin=0;
		int    CurYm=0; 
		int    MinMinYm=0;
		int    RetCode=0; 
		long   ContractNo=0;
		long   IdNo=0;
		char   OweTableName[30+1];
		char   PayCode[1+1];
		char   BelongCode[7+1];
		char   TmpSqlStr[TMPLEN+1];
		float  delay_rate=0.00;
		double DelayFee=0.00;
		double fDelayFee=0.00;    
	EXEC SQL   END DECLARE SECTION;
	
	
	CurYm = GetCurYm();
	MinMinYm = CurYm;
	init(TmpSqlStr);
	init(OweTableName);
	init(PayCode);
	init(BelongCode);
	IdNo = InIdNo;
	ContractNo = InContractNo;
	sprintf(TmpSqlStr,"select contract_no from dConUserMsg  \
					   where id_no=:v1 and contract_no=:v2");

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :ContractNo using :IdNo,:ContractNo;
	END;
	END-EXEC;
	if (SQLCODE!=0)
	{
		return -1;
	}
	
	
	init(PayCode);
	init(BelongCode);
	MinYm = 0;
	RetCode = GetAccountMinYm(ContractNo,&MinYm,PayCode,BelongCode);
	if(RetCode != SQL_OK)
	{
		return 1;
	}
	if(MinMinYm > MinYm)
	{
		MinMinYm = MinYm;
	}
	delay_flag = 0;
	delay_rate = 0;
	delay_begin = 0;
	
	/***调用取用户滞纳金参数***/
	EXEC SQL SELECT to_number(delay_flag),delay_rate,delay_begin
 			 INTO   :delay_flag,:delay_rate,:delay_begin
    		 FROM   sPayCode
             WHERE  pay_code=:PayCode
             AND    region_code=substr(:BelongCode,1,2);
	if (SQLCODE != SQL_OK) 
	{
		return 2;
	}
	for (;;)
	{
		if (MinYm > CurYm)  
		{
			break;
		}
		init(OweTableName);
		sprintf(OweTableName, "dCustOweTotal%6d%ld", MinYm, ContractNo%10);
	  
		DelayFee = 0.00;  
	
		init(TmpSqlStr);                   
		sprintf(TmpSqlStr,"select nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0) \
					       FROM %s where id_no=:v5  and contract_no=:v6 \
                           and payed_status=any('0','9') ",OweTableName);

 
		EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :TmpSqlStr INTO :DelayFee using :delay_flag,:delay_begin,:delay_begin,:delay_rate,:IdNo,:ContractNo;
		END;
		END-EXEC;
		if (SQLCODE != 0&&SQLCODE!=NOTFOUND) 
		{
			return 3;
		}
		else if(SQLCODE==NOTFOUND)
		{
			break;
		}
		fDelayFee = fDelayFee + DelayFee;    
		MinYm = PublicAddMon(MinYm);
	}
	EXEC SQL CLOSE GetPhoneOweCur1;
	*OutDelayFee = fDelayFee;
	return 0;
}

/**********************************************************************
功能：
	查找用户某个帐户的欠费帐单帐单费用
输入参数：
输出参数：
	InIdNo               long     用户ID
	InContractNo         long     帐号
	*OutDelayFee         double   滞纳金
返回结果：
***********************************************************************/
int GetPhoneFee(long InIdNo, long InContractNo, double *OutOweFee, double *OutDelayFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int   MinYm;
		int   delay_flag=0;
		int   delay_begin=0;
		int   CurYm=0; 
		int   MinMinYm=0;
		int   RetCode=0; 
		long  ContractNo=0;
		long  IdNo=0;
		char  OweTableName[30+1];
		char  PayCode[1+1];
		char  BelongCode[7+1];
		char  TmpSqlStr[TMPLEN+1];
		float delay_rate=0.00;
		double DelayFee=0.00;
		double fDelayFee=0.00;
		double OweFee = 0.00;
		double fOweFee = 0.00;
	EXEC SQL END DECLARE SECTION;
	init(OweTableName);
	init(PayCode);
	init(BelongCode);
	init(TmpSqlStr);
	CurYm = GetCurYm();
	MinMinYm = CurYm;
	ContractNo = InContractNo;
	IdNo = InIdNo;

	
	init(PayCode);
	init(BelongCode);
	MinYm = 0;
	RetCode = GetAccountMinYm(ContractNo,&MinYm,PayCode,BelongCode);
	if(RetCode != SQL_OK)
	{
		printf("RetCode[%d]\n",RetCode);
		return 1;
	}
	if(MinMinYm > MinYm)
	{
		MinMinYm = MinYm;
	}
	delay_flag = 0;
	delay_rate = 0;
	delay_begin = 0;
	
	/***调用取用户滞纳金参数***/
	EXEC SQL SELECT to_number(delay_flag),delay_rate,delay_begin
 			 INTO   :delay_flag,:delay_rate,:delay_begin
    		 FROM   sPayCode
             WHERE  pay_code=:PayCode
             AND    region_code=substr(:BelongCode,1,2);
	if (SQLCODE != SQL_OK) 
	{
		return 2;
	}
	for (;;)
	{
		if (MinYm > CurYm)  
		{
			break;
		}
		init(OweTableName);
		sprintf(OweTableName, "dCustOweTotal%6d%ld", MinYm, ContractNo%10);
	  
		DelayFee = 0.00;  
	
		init(TmpSqlStr);                   
		sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0),\
		                   nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0) \
					       FROM %s where id_no=:v5  and contract_no=:v6 \
                           and payed_status=any('0','9') ",OweTableName);

 
		EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :TmpSqlStr INTO :OweFee,:DelayFee using :delay_flag,:delay_begin,:delay_begin,:delay_rate,:IdNo,:ContractNo;
		END;
		END-EXEC;
		if (SQLCODE != 0&&SQLCODE!=NOTFOUND) 
		{
			return 3;
		}
		else if(SQLCODE==NOTFOUND)
		{
			break;
		}
		fDelayFee = fDelayFee + DelayFee; 
		fOweFee = fOweFee + OweFee;   
		MinYm = PublicAddMon(MinYm);
	}
	*OutDelayFee = fDelayFee;
	*OutOweFee = fOweFee;
	return 0;
}



/*************************
功能：
	取当前年月
输入参数：
输出参数：
	YearMonth int 当前年月
*************************/
int GetCurYm()
{
	EXEC SQL BEGIN DECLARE SECTION;
		int  YearMonth=0;
	EXEC SQL   END DECLARE SECTION;
	EXEC SQL SELECT to_number(substr(to_char(sysdate,'YYYYMMDD HH24:MI:SS'),1,6))  
	         INTO   :YearMonth  FROM dual;
	if (SQLCODE != SQL_OK) 
	{
		return 130012;
	}
	return YearMonth;
}


/*************************
功能：
	取当前年月日
输入参数：
输出参数：
	YearMonth int 当前年月日
*************************/
int GetCurDate(int *InTotalDate)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int  YearMonth=0;
	EXEC SQL END DECLARE SECTION;
	EXEC SQL SELECT to_number(substr(to_char(sysdate,'YYYYMMDD HH24:MI:SS'),1,8))  
	         INTO   :YearMonth FROM dual;
	if (SQLCODE != SQL_OK) 
	{
		return 130012;
	}
	*InTotalDate = YearMonth;
	return 0;
}


/********************************************************************
已知帐户ID，用户ID，对于帐单进行冲销函数
输入函数：
	InIdNo                 long   	用户ID
	InContractNo           long   	帐户ID
	InYearMonth            int    	年月
	*InRegionCode          char   	地区
	*InDistrictCode        char   	区县
	*InPhoneNo             char   	手机号码
	InRemainMoney          double 	冲销款
	InDelayFavourRate      double 	滞纳金优惠率
	*InWorkNo              char   	工号
	InTotalDate            int    	帐务日期
	InPayAccept            int    	交费流水
	InPayAcceptCon         int    	冲销流水
	*InPayType             char   	交费类型
	*InPayCode             char   	交费方式
	*InOpCode              char   	操作代码
	InDelayFlag            int    	滞纳金标志
	InDelayRate            double 	滞纳金率
	InDelayBegin           int    	开始天
输出函数：                        	
	*OutRemainMoney        double 	剩余可用冲销金额
	*OutPayedOwe           double 	冲销金额
	*OutDelayFee           double 	滞纳金
*********************************************************************/
int PublicPayOwe(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int    TotalDate = 0;
		int    BillDay = 0;
		int    OweDay = 0;
		int    YearMonth = 0;
		int    RetValue = 0;
		int    DelayFlag = 0;
		int    DelayBegin = 0;
		long   PayAcceptCon = 0;
		long   PrintAccept = 0;
		int    PrintSerial = 0;
		int    SignFlag = 0;
		int    i = 0;
		int    haveFeeCode = 0;
		long   ContractNo = 0;
		long   IdNo = 0;
		double RemainMoney = 0.00;
		float  DelayRate = 0.00;
		double PayedOwe = 0.00;
		double PayedDelay = 0.00;
		double StillOwe = 0.00;
		double DelayFee = 0.00;
		double ShouldPay = 0.00;
		double FavourFee = 0.00;
		double PayedPrepay = 0.00;
		double PayedLater = 0.00;
		double OweFee = 0;
		double temp_fee = 0.00;
        double PrintFee = 0.00;
		char   RegionCode[REGIONCODELEN+1];
		char   DistrictCode[DISTRICTCODELEN+1];
		char   PhoneNo[PHONENOLEN+1];
		char   WorkNo[WORKNOLEN+1];
		char   TmpSqlStr[TMPLEN+TMPLEN+1];
		char   OweTableName[30+1];
		char   PayType[PAYTYPELEN+1];
		char   PayCode[PAYCODELEN+1];
		char   Flag[FLAGLEN+1]; 
		char   OweStatus[1+1];
		char   BillType[1+1];
		char   PrintFlag[PRINTFLAGLEN+1];
		char   FeeText[300*4+1];
		/*char   FeeText[FEETEXTLEN+1];*/
	EXEC SQL END DECLARE SECTION; 
	
	init(RegionCode);
	init(DistrictCode);
	init(PhoneNo);
	init(WorkNo);
	init(PayType);
	init(PayCode);
	init(Flag);
	init(TmpSqlStr);
	init(OweTableName);
	init(OweStatus);
	init(PrintFlag);
	init(FeeText);
	strcpy(PayType,InPayType);
	
	DelayRate = InDelayRate;
	haveFeeCode = 0;

	Sinitn(TmpSqlStr);
	sprintf(TmpSqlStr,"select sign_flag,print_serial \
	                   FROM sPayType where pay_type=:v1");
	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr into :SignFlag,:PrintSerial using :PayType;
	END;
	END-EXEC;
	
	if (SQLCODE!=0)
	{
		return 130351;
	}

	init(PrintFlag);
	if (0 == SignFlag)
	{
		strcpy(PrintFlag,"0");
		PrintAccept = 0;
	}
	else 
	{
		strcpy(PrintFlag,"1");
		PrintAccept = InPayAccept;
	}

	strcpy(RegionCode, InRegionCode);
	strcpy(DistrictCode, InDistrictCode);
	strcpy(PhoneNo, InPhoneNo);
	strcpy(WorkNo, InWorkNo);
	TotalDate=InTotalDate;
	strcpy(PayType, InPayType);
	strcpy(PayCode, InPayCode);
	ContractNo = InContractNo;
	IdNo = InIdNo;
	YearMonth = InYearMonth;
	DelayFlag = InDelayFlag;
	DelayBegin = InDelayBegin;
	DelayRate = InDelayRate;
	RemainMoney = round_F(InRemainMoney, 2);
	
	

	sprintf(OweTableName,"dCustOweTotal%6d%ld",InYearMonth,ContractNo%10);
 
	init(TmpSqlStr);                   
	sprintf(TmpSqlStr,"select bill_type,bill_day,nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0), \
                  nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(:v2*100+1+:v12,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(:v3*100+1+:v4,'YYYYMMDD'),2))*:v5*(should_pay-favour_fee-payed_prepay-payed_later),2)),0),\
                  nvl(sum(should_pay),0),nvl(sum(favour_fee),0),\
                  nvl(sum(payed_prepay),0),nvl(sum(payed_later),0), \
                  (sign(sign(floor(sysdate-add_months(to_date(:v6*100+1+:v7,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(:v8*100+1+:v9,'YYYYMMDD'),2)) \
                  from %s where id_no=:v10  and contract_no=:v11  and payed_status=any('0','9','a') group by bill_type,bill_day order by bill_type,bill_day",OweTableName);
	
 
	EXEC SQL PREPARE PublicPayOweStr1 FROM :TmpSqlStr;
	EXEC SQL DECLARE PublicPayOweCur1 CURSOR FOR PublicPayOweStr1;
	EXEC SQL OPEN PublicPayOweCur1  using :DelayFlag,:YearMonth,:DelayBegin,:YearMonth,
	                                      :DelayBegin,:DelayRate,:YearMonth,:DelayBegin,
	                                      :YearMonth,:DelayBegin,:IdNo,:ContractNo;
	for (;;)
	{
		BillDay = 0;
		StillOwe = 0.00;
		DelayFee = 0.00;
		ShouldPay = 0.00;
		FavourFee = 0.00;
		PayedPrepay = 0.00;
		PayedLater = 0.00;
		OweDay = 0;
		init(BillType);
		EXEC SQL FETCH PublicPayOweCur1 INTO :BillType,:BillDay,:StillOwe,:DelayFee,:ShouldPay,:FavourFee,:PayedPrepay,:PayedLater,:OweDay; 
		if (SQLCODE==NOTFOUND || SQLCODE<0) break;
   
		printf("\n[%12.2f][%12.2f][%12.2f]",RemainMoney,OweFee,DelayFee);
		RemainMoney = round_F(RemainMoney, 2);       
		OweFee = round_F(StillOwe, 2); 
		DelayFee = round_F(DelayFee-DelayFee*InDelayFavourRate, 2);

		/************
		判断帐单状态
		9：托收未决
		0：欠费未交
		2：欠费交清
		*************/
		init(OweStatus);
		printf("\n[%12.2f][%12.2f][%12.2f]",RemainMoney , OweFee , DelayFee);
		if (RemainMoney - OweFee - DelayFee < -0.005)
		{
			
			OweFee = round_F(RemainMoney/(1+DelayRate*OweDay-DelayRate*OweDay*InDelayFavourRate),2);
			DelayFee = round_F(RemainMoney,2) - round_F(OweFee, 2);
			if (!strcmp(InPayCode, "4"))
			{
				strcpy(OweStatus, "9");
			}
			else   
			{
				haveFeeCode=0;
				strcpy(OweStatus, OWE_UNPAYED);
			}
		}
		else
		{
			haveFeeCode=1;
			strcpy(OweStatus, OWE_PAYED);
		} 
		
		if (RemainMoney != 0) 
		{
			if (haveFeeCode==0)
			{
				haveFeeCode = 1;
				init(FeeText);
				RetValue = PublicGetFeeCode(IdNo, ContractNo, PayType, FeeText);
				if (RetValue != RET_OK) 
				{
					EXEC SQL CLOSE PublicPayOweCur1;
					return RetValue;
				}
			}
       
			RetValue = PublicInsertCustPayOwe(IdNo, ContractNo,YearMonth,InPayAccept, 
                                   PayType, BillType, BillDay,  PhoneNo, 
                                   TotalDate, InPayAcceptCon, OweFee, 
                                   DelayFee,temp_fee, temp_fee, OweFee, WorkNo, 
                                   InOpCode, PrintFee, PrintFlag, PrintAccept,
                                   OweStatus, FeeText);
			if (RetValue != RET_OK) 
			{
				EXEC SQL CLOSE PublicPayOweCur1;
				return RetValue;
			}
                 
			/*-- 更新欠费汇总表dCustOweTotal_XXX --*/
			RetValue = PublicUpdateCustOweTotal(OweFee, OweStatus, IdNo, ContractNo,YearMonth, BillType, BillDay);
			if (RetValue != RET_OK) 
			{
				EXEC SQL CLOSE PublicPayOweCur1;
				return RetValue;
			}
	                        
			RemainMoney = round_F(RemainMoney,2) - round_F(OweFee,2) - round_F(DelayFee, 2);
			PayedOwe = round_F(PayedOwe,2) + round_F(OweFee, 2);
			PayedDelay = round_F(PayedDelay,2) + round_F(DelayFee, 2);
		}
		if (RemainMoney == 0)
		{
			break;
		} 
	}                                                                                          

	EXEC SQL CLOSE PublicPayOweCur1;
	*OutRemainMoney = RemainMoney;
	*OutPayedOwe = PayedOwe;
	*OutDelayFee = PayedDelay;
	return 0;

}


/********************************************************
功能：
	获得某帐号针对某个用户的付费计划费用类型代码串
输入参数：
	InIdNo       long  用户ID
	InContractNo long  帐户ID，
	InPayType    char* 付费类型
	OutFeeText   char* 付费计划费用类型代码串
输出参数：
	冲销顺序串
返回值：
说明：
	需要确定是否用户付费计划是完整的所有FEECODE，如果只是部分，此函数需要修改
********************************************************/
int PublicGetFeeCode(long InIdNo,     long InContractNo, 
                     char *InPayType, char *OutFeeText)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char FeeCode[300][FEECODELEN+1];
		/*char FeeText[FEETEXTLEN+1];*/
		char FeeText[300*4+1];
		char TmpSqlStr[TMPLEN+1];
		char PayType[1+1];
		long IdNo = 0;
		long ContractNo = 0;
		int  i=0;
	EXEC SQL   END DECLARE SECTION;
	
	int  TmpRows=0;
	IdNo = InIdNo;
	ContractNo = InContractNo;
	Sinitn(FeeCode);
	Sinitn(FeeText);
	Sinitn(TmpSqlStr);
	strcpy(PayType,InPayType);
	sprintf(TmpSqlStr, "SELECT distinct fee_code FROM (select fee_code from dConUserRate \
					    WHERE id_no=:v1 AND contract_no=:v2  order BY pay_order)");

	EXEC SQL PREPARE PublicGetFeeCodeStr  FROM :TmpSqlStr;
	EXEC SQL DECLARE PublicGetFeeCodeCur1  CURSOR FOR PublicGetFeeCodeStr;
	EXEC SQL OPEN PublicGetFeeCodeCur1 using :IdNo,:ContractNo;
	if (SQLCODE < SQL_OK) 
	{
		printf("<PublicGetFeeCode>TmpSqlStr=[%s]\n", TmpSqlStr);
		return 130352;
	}
	i=0;
	EXEC SQL FETCH PublicGetFeeCodeCur1 INTO  :FeeCode[i];
	if (SQLCODE != NOTFOUND )
	{
		for (;;)
		{
			i++;
			EXEC SQL FETCH PublicGetFeeCodeCur1 INTO  :FeeCode[i]; 
			if (SQLCODE==NOTFOUND || SQLCODE<0) break;
		}
		EXEC SQL CLOSE PublicGetFeeCodeCur1;
	}

	init(TmpSqlStr);
	sprintf(TmpSqlStr, "SELECT fee_code \
	                    FROM dDefaultOwePlan WHERE pay_type=:v1 order by fee_order");
	EXEC SQL PREPARE PublicGetFeeCodeStr1 FROM :TmpSqlStr;
	EXEC SQL DECLARE PublicGetFeeCodeCur2 CURSOR FOR PublicGetFeeCodeStr1;
	EXEC SQL OPEN PublicGetFeeCodeCur2 using :PayType;
	for (;;)
	{
		EXEC SQL FETCH PublicGetFeeCodeCur2 INTO :FeeCode[i];
		if (SQLCODE==NOTFOUND || SQLCODE<0) break;
		i++;
	}
	EXEC SQL CLOSE PublicGetFeeCodeCur2;
         
	TmpRows = 0;
	
	while (1) 
	{
		if (TmpRows > 300-1)
		{
		    return 130353;
		}
		if (FeeCode[TmpRows][0] != 0) 
		{
			
			if (TmpRows == 0) 
			{     
				strcpy(FeeText, FeeCode[TmpRows]);
			}
			else 
			{
				strcat(FeeText, SEPERATE);      
				strcat(FeeText, FeeCode[TmpRows]);
			}
		}
		else 
		{
			break;
		}
		TmpRows++;
	}
	strcpy(OutFeeText, FeeText);
	if (FeeText[0]==0||i==0)
	{
		printf("没有找到冲销计划\n");
		return 130354;
	}
	return 0;
}


/*******************************************
功能：
	记录综合帐和明细帐冲销表，更新明细帐单状态
输入参数：
	InIdNo          long    用户ID
	InContractNo	long    帐户ID
	InYearMonth		int     帐务年月
	InPayAccept		int     交费流水
	*InPayType      char    付费类型
	*InBillType     char    帐单类型
	InBillDay       int     帐单批次 
	*InPhoneNo      char    手机号码 
	InTotalDate     int     帐务日期 
	InPayAcceptCon  int     冲销流水 
	InOweFee        double  欠费 
	InDelayFee      double  滞纳金 
	InBadFee        double  坏帐 
	InDeadFee       double  死帐 
	InPayedLater    double  新交款 
	*InWorkNo       char    工号 
	*InOpCode       char    操作代码 
	InPrintFee      double  打印发票金额 
	*InPrintFlag    char    打印标志 
	InPrintAccept   int     打印流水 
	*InOweStatus    char    帐单状态 
	*InFeeText      char    冲销顺序代码 
********************************************/
int PublicInsertCustPayOwe(long   InIdNo,       long   InContractNo,
                           int    InYearMonth,  long   InPayAccept, 
					       char   *InPayType,   char   *InBillType, 
					       int    InBillDay,    char   *InPhoneNo, 
					       int    InTotalDate,  long   InPayAcceptCon, 
					       double InOweFee,     double InDelayFee,
					       double InBadFee,     double InDeadFee,
					       double InPayedLater, char   *InWorkNo,
					       char   *InOpCode,    double InPrintFee,
					       char   *InPrintFlag, long   InPrintAccept,
					       char   *InOweStatus, char   *InFeeText) 
{
	EXEC SQL BEGIN DECLARE SECTION;
		long PayAccept = 0;
		long SmallAccept = 0;
		double PayOwe = 0.00;
		double PayedLater = 0.00;
		double PayedOwe = 0.00;
		double PayedLaterDetail = 0.00;
		double RemainMoney = 0.00;		
		char TmpSqlStr[2000+1];
		char PayType[1+1];
		char PayedStatusDetail[1+1];
		char *FeeCode=NULL;
		char DetailCode[2+1];
		char BillType[1+1];
		char OweTableName[30+1];
		char OweTableName1[30+1];
		char OweTableName2[30+1];
		char OweTableName3[30+1];
		char PrintFlag[PRINTFLAGLEN+1];
		int TotalDate = 0;
		double DelayFee = 0.00;
		double BadFee = InBadFee;
		double DeadFee = InDeadFee;
		char WorkNo[6+1];
		char OpCode[4+1];
		double PrintFee = 0.00;
		long IdNo = 0;
		long ContractNo = 0;
		int BillDay = 0;
		long PrintAccept = 0;
	EXEC SQL END DECLARE SECTION;
	
	int RetValue = 0;
	init(TmpSqlStr);
	init(PayType);
	init(PayedStatusDetail);
	init(OweTableName);
	init(OweTableName1);
	init(OweTableName2);
	init(OweTableName3);
	init(DetailCode);

	PayAccept = InPayAccept;
	PayOwe = InOweFee;
	PayedLater = InPayedLater;
	InPrintAccept = InPayAccept;
	PrintAccept = InPrintAccept;
	RemainMoney = InPayedLater;
	strcpy(PayType, InPayType);

	DelayFee = InDelayFee;
	BadFee = InBadFee;
	DeadFee = InDeadFee;
	strcpy(WorkNo,InWorkNo);
	strcpy(OpCode,InOpCode);
	PrintFee = PrintFee;
	strcpy(PrintFlag,InPrintFlag);
	strcpy(BillType,InBillType);
	TotalDate = InTotalDate;
	IdNo = InIdNo;
	ContractNo = InContractNo;
	BillDay = InBillDay;

	

	SmallAccept = InPayAcceptCon;
	sprintf(OweTableName,"dCustOweTotal%7ld",InYearMonth*10+InContractNo%10);
	sprintf(OweTableName1,"dCustPayOwe%7ld",InTotalDate/100*10+InContractNo%10);
	sprintf(OweTableName2,"dCustOwe%8ld",InYearMonth*100+InContractNo%100);
	sprintf(OweTableName3,"dCustPayOweDet%8ld",InTotalDate/100*100+InContractNo%100); 
	/*组织机构改造插入表字段group_id和org_id  edit by zhangxha at 04/03/2009*/
	sprintf(TmpSqlStr,"INSERT INTO %s (id_no,cust_id,contract_no,con_cust_id,\
                       pay_type,login_accept,year_month,bill_day,\
                       bill_type,phone_no,sm_code,bill_code,\
                       attr_code,belong_code,payed_owe,delay_owe,\
                       bad_fee,dead_fee,payed_prepay,login_no,\
                       op_time,op_code,print_fee,print_flag,\
                       print_accept,total_date,accept_detail,group_id) \
                       SELECT id_no,cust_id,contract_no,con_cust_id,\
                       :v1,:v2,year_month,bill_day,bill_type,\
                       phone_no,sm_code,bill_code,attr_code,belong_code,\
                       :v3,:v4,\
                       :v5,:v6,0.00,:v7,\
                       sysdate,:v8,:v9,'0',0,:v10,:v11,group_id \
                       FROM %s WHERE id_no=:v12 AND contract_no=:v13 \
                       AND bill_type=:v14 \
                       AND bill_day=:v15 AND payed_status=any('0','9')",
                       OweTableName1,OweTableName);
                        
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :PayType,:PayAccept,:PayOwe, :DelayFee, 
                                    :BadFee, :DeadFee, :WorkNo,:OpCode,:PrintFee, 
                                    :TotalDate,:SmallAccept,:IdNo, 
                                    :ContractNo,:BillType, :BillDay;

	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicInsertCustPayOwe>TmpSqlStr=[%s]\n", TmpSqlStr);
		printf("[%s][%d]\n",SQLERRMSG,SQLCODE);
		return 130355;
	}
       
	if (!strcmp(InOweStatus, OWE_PAYED)) /*** 全部冲销 ***/
	{
		sprintf(TmpSqlStr, "INSERT INTO %s (ID_NO,CUST_ID,CONTRACT_NO,\
		                   CON_CUST_ID,PAY_TYPE,\
					       LOGIN_ACCEPT,YEAR_MONTH,BILL_DAY,FEE_CODE,\
					       BILL_TYPE,SHOULD_PAY,FAVOUR_FEE,PAYED_PREPAY,\
					       PAYED_LATER,ACCEPT_DETAIL,detail_code)\
                           SELECT id_no,cust_id,contract_no,con_cust_id,\
                           :v1,:v2,year_month,bill_day,fee_code,bill_type,\
                           should_pay,favour_fee,0.00, \
        				   should_pay-favour_fee-payed_prepay-payed_later,:v3,\
						   detail_code \
    					   FROM %s WHERE id_no=:v4 AND contract_no=:v5 \
    					   AND bill_type=:v6 \
                           AND bill_day=:v7 \
    					   and payed_status=any('0','9')", 
            			   OweTableName3,OweTableName2);
		EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
		EXEC SQL EXECUTE sql_stmt using :PayType,:PayAccept,:SmallAccept,:IdNo,:ContractNo,
		                                :BillType,:BillDay;
	    if (SQLCODE != SQL_OK) 
	    {
	    	printf("<PublicInsertCustPayOwe>TmpSqlStr=[%s]\n", TmpSqlStr);
			printf("\n[%s],[%d]",SQLERRMSG,SQLCODE);
			return 130356;
	    }

		Sinitn(TmpSqlStr);      
		sprintf(TmpSqlStr, "UPDATE %s SET payed_later=should_pay-favour_fee-payed_prepay,\
		                    payed_status='2' WHERE id_no=:v1 \
		                    AND contract_no=:v2 AND \
                            bill_type=:v3 AND bill_day=:v4 \
                            and payed_status=any('0','9')",OweTableName2);
		EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
		EXEC SQL EXECUTE sql_stmt using :IdNo,:ContractNo,:BillType,:BillDay;                     
		if (SQLCODE != SQL_OK) 
		{
			printf("[%d]<PublicInsertCustPayOwe>TmpSqlStr=[%s]\n",getpid(),TmpSqlStr);
			printf("\n[%s],[%d]",SQLERRMSG,SQLCODE);
			return 130357;
		}
	}
	else /*** 部分冲销 ***/
	{  
		FeeCode = strtok(InFeeText, SEPERATE);
		while (FeeCode != NULL) 
		{
	        init(TmpSqlStr);
	        sprintf(TmpSqlStr,"select detail_code from sFeeCodeDetail \
	                           where fee_code=:v1");
                      
	        EXEC SQL PREPARE PublicInsertCustPayOweStr FROM :TmpSqlStr;
	        EXEC SQL DECLARE PublicInsertCustPayOweCur1 CURSOR FOR PublicInsertCustPayOweStr;
	        EXEC SQL OPEN PublicInsertCustPayOweCur1 using :FeeCode;
	        for(;;)
	        {
		        EXEC SQL FETCH  PublicInsertCustPayOweCur1 INTO :DetailCode;
		        if (SQLCODE==NOTFOUND || SQLCODE<0) break;
		        
		        /* 要按FeeCode的顺序进行冲销 */ 
		        PayedLaterDetail = 0.00;
				sprintf(TmpSqlStr, "SELECT should_pay-favour_fee-payed_prepay-payed_later \
				                    FROM %s WHERE id_no=:v1 and contract_no=:v2 \
				                    AND bill_type=:v3 AND bill_day=:v4 \
                                    AND fee_code=:v5 and Detail_code=:v6 \
                                    AND payed_status=any('0','9')",OweTableName2);
				EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :TmpSqlStr into :PayedLaterDetail using :IdNo,:ContractNo,
					                             :BillType,:BillDay,:FeeCode,:DetailCode;
				END;
				END-EXEC;

				if (SQLCODE == CNO_MORE_ROWS)
				{
					continue;
				}
				printf("1.PayedLaterDetial[%10.2f]\n",PayedLaterDetail);
				printf("fee_code[%s]\n",FeeCode);
				printf("DetailCode[%s]\n",DetailCode);
				printf("RemainMoney[%10.2f]\n",RemainMoney);
								
				if (RemainMoney >= PayedLaterDetail) 
				{
					RemainMoney = RemainMoney - PayedLaterDetail;
					strcpy(PayedStatusDetail, OWE_PAYED);
				}
				else 
				{
					PayedLaterDetail = RemainMoney;
					RemainMoney = 0;
					strcpy(PayedStatusDetail, OWE_UNPAYED);
				}
				printf("2.PayedLaterDetial[%10.2f]\n",PayedLaterDetail);
				printf("2.RemainMoney[%10.2f]\n",RemainMoney);
				
				sprintf(TmpSqlStr, "INSERT INTO %s (ID_NO,CUST_ID,CONTRACT_NO,\
				            CON_CUST_ID,PAY_TYPE,\
							LOGIN_ACCEPT,YEAR_MONTH,BILL_DAY,FEE_CODE,\
							BILL_TYPE,SHOULD_PAY,FAVOUR_FEE,PAYED_PREPAY,\
							PAYED_LATER,ACCEPT_DETAIL,detail_code) \
							SELECT id_no,cust_id,contract_no,con_cust_id,:v1,:v2,\
							year_month,bill_day,fee_code,bill_type,should_pay,\
							favour_fee,0.00,:v3,:v4,detail_code \
				            FROM %s WHERE id_no=:v5 AND contract_no=:v6 \
				            AND bill_type=:v7 AND  \
				            bill_day=:v8 AND fee_code=:v9 and Detail_code=:v10 \
				            AND payed_status=any('0','9')", 
				            OweTableName3,OweTableName2);

				EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
				EXEC SQL EXECUTE sql_stmt using :PayType,:PayAccept,:PayedLaterDetail,
				                                :SmallAccept,:IdNo,:ContractNo,
				                                :BillType,:BillDay,:FeeCode,:DetailCode;

				if (SQLCODE != SQL_OK && SQLCODE != CNO_MORE_ROWS) 
				{
					printf("<PublicInsertCustPayOwe>TmpSqlStr=[%s]\n", TmpSqlStr);
					printf("\n SQLCODE=[%d] \n",SQLCODE);
					return 130358;
				}

				sprintf(TmpSqlStr, "UPDATE %s SET payed_later=payed_later+:v1,\
				                    payed_status=:v2 WHERE id_no=:v3 \
				                    AND contract_no=:v4 \
				                    AND bill_type=:v5 \
                                    AND bill_day=:v6 AND fee_code=:v7 \
                                    and detail_code=:v8 AND payed_status=any('0','9')", 
                                    OweTableName2);
                        
				EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
				EXEC SQL EXECUTE sql_stmt using :PayedLaterDetail,:PayedStatusDetail,:IdNo, 
                                                :ContractNo,:BillType, 
                                                :BillDay,:FeeCode,:DetailCode;
                                                
                                   
				if (SQLCODE != SQL_OK && SQLCODE != CNO_MORE_ROWS) 
				{
					printf("<PublicInsertCustPayOwe>TmpSqlStr=[%s]\n",TmpSqlStr);
					printf("\n SQLCODE=[%d] \n",SQLCODE);
					return 130359;
				}
				
				if (RemainMoney <= 0) 
				{
					printf("break:detail_code[%s]\n",DetailCode);
					break;
				}
			}/**DetailCode***/ 
			if (RemainMoney <= 0)
			{
				printf("break:fee_code[%s]\n",FeeCode);
				break;
			}
			FeeCode = strtok(NULL, SEPERATE);
		}
	}
	return 0;
}


/**********************************************************************
更新综合帐单
输入参数：
	InPayedLater	double  新交款
	*InOweStatus	char    帐单状态
 	InIdNo,			long    用户ID
	InContractNo	long    帐户ID
 	InBillDate		int     帐单年月
	*InBillType		char    帐单类型
 	InBillDay		int     帐单批次
输出参数：
***********************************************************************/
int PublicUpdateCustOweTotal(double InPayedLater, char *InOweStatus, 
                             long   InIdNo,       long InContractNo, 
                             int    InBillDate,   char *InBillType, 
                             int    InBillDay)
{
        
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+1];
		char OweTableName[30+1];
		double PayedLater = 0.00;
		char OweStatus[1+1];
		long IdNo = 0.00;
		long ContractNo = 0.00;
		int BillDate = 0;
		char BillType[1+1];
		int BillDay = 0;
	EXEC SQL   END DECLARE SECTION;
	
	init(TmpSqlStr);
	init(OweTableName);
	init(OweStatus);
	init(BillType);
	PayedLater = InPayedLater;
	IdNo = InIdNo;
	ContractNo = InContractNo;
	BillDate = InBillDate;
	BillDay = InBillDay;
	strcpy(OweStatus,InOweStatus);
	strcpy(BillType,InBillType);
	
	/*-- 更新欠费汇总表dCustOweTotal_XXX表 --*/
	sprintf(OweTableName,"dCustOweTotal%6d%ld",InBillDate,InContractNo%10);
                 
	sprintf(TmpSqlStr, "UPDATE %s SET payed_later=payed_later+round(:v1,2),\
	                    payed_status=:v2\
	                    WHERE id_no=:v3 AND contract_no=:v4 \
	                    AND bill_type=:v5 AND bill_day=:v6", 
	                    OweTableName);                    
	
	EXEC SQL PREPARE ins_stmt From :TmpSqlStr;
	EXEC SQL EXECUTE ins_stmt USING :PayedLater,:OweStatus, :IdNo,
	                    :ContractNo, :BillType, :BillDay;

	if (SQLCODE != 0) 
	{
		
		printf("[%d]<PublicUpdateCustOweTotal>Fail!\n",getpid());
		return 130360;
	}
	return RET_OK;
}


/************************************************************
更新长白行有效期
************************************************************/
int PublicUpdateExpireTime( long InIdNo,        char  *InSmCode, 
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
	if (Days2Expire < 0)
	{
		AddedDays = AddedDays - Days2Expire;
	}
	*/
	/*************************************************
	AddedDays = min(Days,MaxDays - Days2Expire);
	update 'old_expire' as 'expire_time' ,and then 'expire_time' 
	as currently calulated time,in table 'dCustExpire'
	*************************************************/
	strcpy(TmpSqlStr,"UPDATE dCustExpire \
	                   SET old_expire=expire_time,expire_time=expire_time + :v1 \
	                   WHERE id_no=:v2");

	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :AddedDays,:IdNo;

	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERRMSG[%s]\n",SQLCODE,SQLERRMSG);
		return RET_UPDATE_CUST_EXPIRE_ERR;
	}
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(LoginNo,OrgId);
	if(ret <0)
	{
		printf("获取工号org_id失败![%s]\n",LoginNo);
		return 103221; 
	}
	Trim(OrgId);
 				
	/*******************************************************
	insert a record into wExpireList,recording the expire change action
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
	/*---组织机构改造插入表字段edit by zhangxha at 04/03/2009---end*/
	return RET_OK;
}

/*********************************************
有效期回退
*********************************************/

int PublicUndoExpireTime( long InIdNo, int InPayDate,long InPayAccept,
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
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(TmpSqlStr);
	Sinitn(TimeCode);
	init(OpCode);
	init(LoginNo);
	init(OrgCode);
	init(OpTime);
	init(OpNote);
	init(OrgId);
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
	
	strcpy(TmpSqlStr,"SELECT nvl(days,0) ,nvl(time_code,' ') \
	                   FROM   wExpireList \
	                   WHERE  id_no=:v1 \
	                   AND    total_date=:v2 \
	                   AND    login_accept=:v3");

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :AddedDays,:TimeCode using :IdNo,:PayDate,:PayAccept;
	END;
	END-EXEC;

	if (SQLCODE==NOTFOUND )
	{
		return 0;
	}
	else if(SQLCODE!=0)
	{
		printf("1:[%s]\n",TmpSqlStr);
		return -1;
	}
	
	
	fAddedDays = (-1)*AddedDays;
	sprintf(TmpSqlStr,"UPDATE dCustExpire \
	                   SET expire_time=expire_time+:v1 \
	                   WHERE id_no=:v2");
	
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :fAddedDays,:IdNo;

	if ( SQLCODE < 0) 
	{
		printf("UndoExpirTime:%s\n",TmpSqlStr);
		return -1;
	}
	if(SQLCODE==1403)
	return 0;
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(LoginNo,OrgId);
	if(ret <0)
	{
	  	printf("获取工号org_id失败![%s]\n",LoginNo);
	  	return -1; 
	}
	Trim(OrgId);
	
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
***/
	if ( SQL_OK != SQLCODE ) 
	{
		printf("UndoExpire:%d[%s][%s][%ld][%d][%ld][%s][%s][%d][%s][%s][%s][%s]\n",SQLCODE,TmpSqlStr,SQLERRMSG,IdNo,TotalDate,LoginAccept,OpCode,TimeCode,fAddedDays,LoginNo,OrgCode,OpTime,OpNote);
		return -1;
	}
	
	return RET_OK;
}



/*****************************
功能：判断是否为银行工号
*****************************/
int PublicGetBankWorkNo(char *InWorkNo)
{
	char WorkNo[6+1];
	strncpy(WorkNo,InWorkNo,5);
	if (strcasecmp(InWorkNo,"~~~~~")==0)
	{
		return 0;
	}
	return 9999;
}


/*****************************
判断是否为交费卡的工号
*******************************/
int PublicIsCardPayWorkNo(char *InWorkNo)
{
	if ( 0 == strcasecmp(InWorkNo,"@@@@@@")||0 == strcasecmp(InWorkNo,"######")||0 == strcasecmp(InWorkNo,"$$$$$$"))
	{
		return 0;
	}
	
	return 9999;
}


/********************************
取手机号码的默认帐户和用户ID
********************************/
int GetPhoneAccount(char *InPhoneNo,long *OutContractNo,long *OutIdNo,
                    char *OutAttrCode,char *OutCreditCode,
										char *OutRunCode,int *OutReopFlag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char  phone_no[15+1];
		long  contract_no = 0;
		long  id_no = 0;
		char  attr_code[8+1];
		char  credit_code[1+1];
		char  run_code[2+1];
		int   ids=0;
	EXEC SQL END DECLARE SECTION;

	init(phone_no);
	init(attr_code);
	init(credit_code);
	init(run_code);
	strcpy(phone_no,InPhoneNo);
	         
	EXEC SQL SELECT contract_no,id_no,attr_code,credit_code,run_code,ids
	     INTO  :contract_no,:id_no,:attr_code,:credit_code,:run_code,:ids
	     FROM  dcustmsg 
	     WHERE substr(run_code,2,1)<'a' and phone_no=rpad(:phone_no,15);
	     
	if(SQLCODE!=0)
	{
		printf("<GetPhoneAccount>[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
		return  9999;
	}
	if(ids==1)
	{
			EXEC SQL select count(*) into :ids from dConUserMsg
					 where  contract_no=:contract_no
					 and serial_no=0;
			if(SQLCODE!=0)
			{
				printf("<GetPhoneAccount ids >[%d][%s]\n", sqlca.sqlcode, SQLERRMSG);
				return  9999;
			}
	}
	if(ids>1)
		*OutReopFlag=1;
	else
		*OutReopFlag=0;
	*OutContractNo = contract_no;
	*OutIdNo = id_no;
	strcpy(OutAttrCode,attr_code);
	strcpy(OutCreditCode,credit_code);
	strcpy(OutRunCode,run_code);
	return 0;
}







/**************************
插入操作员记录表
**************************/
int PublicInsertOprRecord(int  InTotalDate, long InPayAccept,  char *InOpCode,
                          char *InPayType,  double InPayMoney, char *InSmCode, 
                          long InIdNo,      char *InPhoneNo,   char *InOrgCode,
        				  char *InWorkNo,   char *InPayNote)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[1000+1];
		int  CurYm = 0;
		int  RetValue = 0;
		int TotalDate = 0;
		long PayAccept = 0;
		char OpCode[4+1];
		char PayType[1+1];
		double PayMoney = 0.00;
		char SmCode[2+1];
		long IdNo = 0;
		char PhoneNo[15+1];
		char OrgCode[9+1];
		char WorkNo[6+1];
		char PayNote[60+1];
		char OrgId[10+1];
		int  ret=0;
	EXEC SQL   END DECLARE SECTION;
	init(TmpSqlStr);
	init(OpCode);
	init(PayType);
	init(SmCode);
	init(PhoneNo);
	init(OrgCode);
	init(WorkNo);
	init(PayNote);
	init(OrgId);
	
	TotalDate = InTotalDate;
	PayAccept = InPayAccept;
	strcpy(OpCode,InOpCode);
	strcpy(PayType,InPayType);
	PayMoney = InPayMoney;
	strcpy(SmCode,InSmCode);
	IdNo = InIdNo;
	strcpy(PhoneNo,InPhoneNo);
	strcpy(OrgCode,InOrgCode);
	strcpy(WorkNo,InWorkNo);
	strcpy(PayNote,InPayNote);
	CurYm = GetCurYm();
	if(RetValue != 0)
	{
		return 9999;
	}
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(WorkNo,OrgId);
	if(ret <0)
	{
		printf("<PublicInsertOprRecord>获取用户org_id失败![%s]\n", WorkNo);
  		return 9999; 
	}
	Trim(OrgId);
	sprintf(TmpSqlStr,"insert into wLoginOpr%6d(total_date,login_accept,op_code,pay_type,\
	                   pay_money,sm_code,id_no,phone_no,org_code,login_no,\
	                   op_time,op_note,ip_addr,org_id) \
	                   values(:v1,:v2,:v3,:v4,round(:v5,2),:v6,:v7,:v8,:v9,:v10,\
	                   sysdate,:v11,'null',:v12)",CurYm);
    EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
    EXEC SQL EXECUTE sql_stmt using :TotalDate,:PayAccept,:OpCode,:PayType,:PayMoney,:SmCode,
                                    :IdNo,:PhoneNo,:OrgCode,:WorkNo,:PayNote,:OrgId;                

	if (SQLCODE != SQL_OK) 
	{
		printf("<PublicInsertOprRecord>TmpSqlStr=[%s]\n", TmpSqlStr);
		printf("Insert into wLoginOpr FAIL [%d]\n",SQLCODE);
		printf("TotalDate==[%d]\n",TotalDate);      
		printf("PayAccept==[%ld]\n",PayAccept);               
		printf("OpCode==[%s]\n",OpCode);            
		printf("PayType==[%s]\n",PayType);            
		printf("PayMoney==[%f]\n",PayMoney);          
		printf("SmCode==[%s]\n",SmCode);             
		printf("IdNo==[%ld]\n",IdNo);                
		printf("PhoneNo==[%s]\n",PhoneNo);       
		printf("OrgCode==[%s]\n",OrgCode);             
		printf("WorkNo==[%s]\n",WorkNo);               
		printf("PayNote=[%s]",PayNote);           
		                       
		
		
		return 9999;
	}
	return 0;
}


/************************
记操作员优惠记录表
************************/

int PublicInsertFavourRecord(double InRate, double InFee, char *InFavourCode, 
                             int InTotalDate, long InPayAccept, long InIdNo, 
                             char *InPhoneNo, char *InSmCode, char *InWorkNo, 
                             char *InOrgCode, char *InOpCode, char *InPayNote)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char   FavourCode[FAVOURCODELEN+1];
		int    TotalDate = 0;
		long   PayAccept = 0;
		long   IdNo = 0;
		char   PhoneNo[PHONENOLEN+1];
		char   SmCode[SMCODELEN+1];
		char   WorkNo[WORKNOLEN+1];
		char   OrgCode[ORGCODELEN+1];
		char   OpCode[OPCODELEN+1];
		char   PayNote[PAYNOTELEN+1];
		double ShouldPay = 0.00;
		double PayedPay = 0.00;
		
		int    ret=0;
		char   OrgId[10+1];

	EXEC SQL   END DECLARE SECTION;
	
	printf("\n InRate[%f][%f]",InRate,InFee);

	init(FavourCode);
	init(PhoneNo);
	init(SmCode);
	init(WorkNo);
	init(OrgCode);
	init(OpCode);
	init(PayNote);
	init(OrgId);
	IdNo = InIdNo;
	PayAccept = InPayAccept;
	strncpy(FavourCode, InFavourCode, FAVOURCODELEN);
	TotalDate = InTotalDate;
	strcpy(PhoneNo, InPhoneNo);
	strcpy(SmCode, InSmCode);
	strcpy(WorkNo, InWorkNo);
	strcpy(OrgCode, InOrgCode);
	strcpy(OpCode, InOpCode);
	strcpy(PayNote, InPayNote);
	
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret = sGetLoginOrgid(WorkNo,OrgId);
	if(ret <0)
	{
  		printf("获取工号org_id失败![%s]\n",WorkNo);
  		return 9999; 
	}
	Trim(OrgId);
	
	printf("\n InRate[%f][%f]",InRate,InFee);
	if ( (InRate *InFee < 0.005) && (InRate *InFee >-0.005))
	{
		return RET_OK;
	}
	else 
	{
		ShouldPay = InFee;
		PayedPay = InFee*(1-InRate);            
		EXEC SQL INSERT INTO wLoginFav(total_date,login_accept,
                                     favour_code,org_code,login_no,
                                     op_code,op_time,sm_code,id_no,
                                     phone_no,should_pay,actual_pay,
                                     should_bill,actual_bill,op_note,org_id)
		                VALUES(:TotalDate,:PayAccept,:FavourCode,
                                     :OrgCode,:WorkNo,:OpCode,
                                     to_char(sysdate,'YYYYMMDD HH24:MI:SS'),
                                     :SmCode,:IdNo,:PhoneNo,
                                     round(:ShouldPay,2),round(:PayedPay,2),
                                     to_char(sysdate,'YYYYMMDD HH24:MI:SS'),
		                     to_char(sysdate,'YYYYMMDD HH24:MI:SS'),
                                     :PayNote,:OrgId);
		if (SQLCODE != SQL_OK) 
		{
			printf("\n [%s][%d]",SQLERRMSG,SQLCODE);
			return 9999;
		}
	}
	return RET_OK;
}



/*得到数据库系统的年月*/
int PublicGetYearMonth(char *curdate)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char date[6+1];
	EXEC SQL END DECLARE SECTION;

	EXEC SQL select to_char(sysdate,'YYYYMM') into :date from dual;
	if ( SQLCODE != SQL_OK )
	{
		return -1002;
	}

	strcpy(curdate,date);
	return 0;
}

/*得到地区名称*/
int PublicGetRegionName(char *InBelongCode, char *OutDistrictName) 
{
	EXEC SQL BEGIN DECLARE SECTION;
		char RegionCode[REGIONCODELEN+1];
		char DistrictName[DISTRICTNAMELEN+1];
	EXEC SQL   END DECLARE SECTION;

	memset(RegionCode, 0, sizeof(RegionCode));
	memset(DistrictName, 0, sizeof(DistrictName));

	strncpy(RegionCode, InBelongCode, REGIONCODELEN);
	
	EXEC SQL SELECT rtrim(region_name)
              	   INTO :DistrictName
               	   FROM sRegionCode
               	  WHERE region_code=:RegionCode;

	if (SQLCODE != SQL_OK) 
	{
		return -1003;
	}
		
	strcpy(OutDistrictName, DistrictName);
	
	return RET_OK;
}

/*月份加一*/
int PublicAddMon(int old)
{
	int year,mon;

	year=old/100;
	mon=old%year;

	if(++mon > 12)
	{
		mon=1;
		year++;
	}

	return year*100+mon;
}

/*月份减num*/
int PublicDecMonth(int yearmonth,int num)
{
	int year,month,i;
	year = yearmonth/100;
	month = yearmonth%year;
	for (i=1;i<=num;i++)
	{
		if (--month < 1)
		{
			month = 12;
			year--;
		}
	}
	return year*100+month;
}

int PublicGetUnbill(long in_contract_no,long in_id_no,int in_type,
                    char *in_phone_no,int in_timeout,double *owe_fee,
                    double *should_pay,double *prepay_fee)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long contract_no = 0;
		long id_no = 0;
		int  type = 0;
		int  timeout = 0;
		int  Port = 0;
		char PhoneNo[PHONENOLEN+1];
		char IP[15+1];
		char DB[32+1];
		char PhoneHead[7+1];
		char sysdate[19+1];
		double out_owe_fee = 0.00;
		double out_should_pay = 0.00;
		double out_prepay_fee = 0.00;
	EXEC SQL END DECLARE SECTION;
	int ret = 0;
	init(PhoneNo);
	init(DB);
	init(PhoneHead);
	init(sysdate);
	
	contract_no = in_contract_no;
	id_no = in_id_no;
	type = in_type;
	timeout = in_timeout;
	strcpy(PhoneNo,in_phone_no);	
	strncpy(PhoneHead,PhoneNo,7);

	if(type==1)
	{
		EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME,to_char(sysdate,'YYYYMMDD HH24:MI:SS')
		         INTO   :IP,:Port,:DB,:sysdate
		         FROM   sPhoneComm
		         WHERE  PHONE_HEAD = :PhoneHead
				 and    query_type='01';
	}
	if(type==0)
	{
		EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME,to_char(sysdate,'YYYYMMDD HH24:MI:SS')
		         INTO   :IP,:Port,:DB,:sysdate
		         FROM   sPhoneComm
		         WHERE  PHONE_HEAD = :PhoneHead
				 and    query_type='02';
    
	}
	if (SQLCODE !=0)
	{
		return 137112;
	}       
	ret = querybill_info(type,contract_no,id_no,PhoneNo,IP,Port,
	                     timeout,DB,&out_owe_fee,&out_should_pay,&out_prepay_fee);
	                     
	printf("[%s]IP=[%s],Port=[%d],DB=[%s][%s][%ld][%ld]get unbill[%d]\n",sysdate,IP,Port,DB,PhoneNo,contract_no,id_no,ret);
	if (ret < 0)
	{
		return 137113;
	}
	*should_pay = out_should_pay;
	*owe_fee = out_owe_fee;
	*prepay_fee = out_prepay_fee;
	return 0;
}

int PublicSendCardPayMsg(long InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+TMPLEN+1];
	    long LoginAccept = 0;
	    char PhoneNo[15+1];
	    char DxOpCode[4+1];
	    char LoginNo[6+1];
	    double PayMoney=0.00;
	EXEC SQL   END DECLARE SECTION;
	
	Sinitn(TmpSqlStr);
	init(PhoneNo);
	init(DxOpCode);
	init(LoginNo);
	LoginAccept = InLoginAccept;
	PayMoney = InPayMoney;
	strcpy(PhoneNo,InPhoneNo);
	strcpy(DxOpCode,InDxOpCode);
	strcpy(LoginNo,InLoginNo);
	
	sprintf(TmpSqlStr,"INSERT INTO wCommonShortMsg (COMMAND_ID,login_accept, \
                           msg_level , phone_no , msg  , order_code , \
                           back_code , dx_op_code , login_no, op_time) \
			   values(SMSG_SEQUENCE.nextval,:v1,100,:v2, \
			   '尊敬的用户: 您所充值的'||to_char(:v3)||'元已到帐，谢谢您的合作。', \
					0,0,:v3, \
					:v4,sysdate)");
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :LoginAccept,:PhoneNo,:PayMoney,:DxOpCode,:LoginNo;

	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERROR[%s]\n",SQLCODE,SQLERRMSG);
		return RET_INSERT_WCOMMONSHORTMSG_ERR;
	}
	return RET_OK;
}

int PublicSendCardPayMsg1(long InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,char *InMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+TMPLEN+1];
	    long LoginAccept = 0;
	    char PhoneNo[15+1];
	    char DxOpCode[4+1];
	    char LoginNo[6+1];
		char Msg[255+1];
	    double PayMoney=0.00;
	EXEC SQL   END DECLARE SECTION;
	
	Sinitn(TmpSqlStr);
	init(PhoneNo);
	init(DxOpCode);
	init(LoginNo);
	init(Msg);
	LoginAccept = InLoginAccept;
	PayMoney = InPayMoney;
	strcpy(PhoneNo,InPhoneNo);
	strcpy(DxOpCode,InDxOpCode);
	strcpy(LoginNo,InLoginNo);
	strcpy(Msg,InMsg);
	
	sprintf(TmpSqlStr,"INSERT INTO wCommonShortMsg (COMMAND_ID,login_accept, \
                           msg_level , phone_no , msg  , order_code , \
                           back_code , dx_op_code , login_no, op_time) \
			   values(SMSG_SEQUENCE.nextval,:v1,100,:v2,:v3, \
					0,0,:v4, \
					:v5,sysdate)");
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :LoginAccept,:PhoneNo,:Msg,:DxOpCode,:LoginNo;

	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERROR[%s]\n",SQLCODE,SQLERRMSG);
		return RET_INSERT_WCOMMONSHORTMSG_ERR;
	}
	return RET_OK;
}

int PublicSendCardPayMsg2(long InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,char *InMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char TmpSqlStr[TMPLEN+TMPLEN+1];
	    long LoginAccept = 0;
	    char PhoneNo[15+1];
	    char DxOpCode[4+1];
	    char LoginNo[6+1];
		char Msg[255+1];
	    double PayMoney=0.00;
	EXEC SQL   END DECLARE SECTION;
	
	Sinitn(TmpSqlStr);
	init(PhoneNo);
	init(DxOpCode);
	init(LoginNo);
	init(Msg);
	LoginAccept = InLoginAccept;
	PayMoney = InPayMoney;
	strcpy(PhoneNo,InPhoneNo);
	strcpy(DxOpCode,InDxOpCode);
	strcpy(LoginNo,InLoginNo);
	strcpy(Msg,InMsg);
	
	sprintf(TmpSqlStr,"INSERT INTO wCommonShortMp (COMMAND_ID,login_accept, \
                           msg_level , phone_no , msg  , order_code , \
                           back_code , dx_op_code , login_no, op_time) \
			   values(SMSG_SEQUENCE.nextval,:v1,100,:v2,:v3, \
					0,0,:v4, \
					:v5,sysdate)");
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :LoginAccept,:PhoneNo,:Msg,:DxOpCode,:LoginNo;

	if ( SQL_OK != SQLCODE ) 
	{
		printf("TmpSqlStr[%s]\n",TmpSqlStr);
		printf("SQLCODE[%d]SQLERROR[%s]\n",SQLCODE,SQLERRMSG);
		return RET_INSERT_WCOMMONSHORTMSG_ERR;
	}
	return RET_OK;
}
/****************
获得工号类型
0:普通工号
1:交费卡
2:银行
****************/
int PublicGetWorkNoType(char *InWorkNo)
{
	int WorkType = 0;
	char WorkNo[6+1];
	char WorkNo1[6+1];
	init(WorkNo);
	init(WorkNo1);
	strcpy(WorkNo1,InWorkNo);
	strncpy(WorkNo,InWorkNo,5);
	
	if ( 0 == strncmp(WorkNo1,"@@@@@@",6))
	{
		WorkType = 2;
	}
	else if(strncmp(&WorkNo1[2],"@@@@",4)==0)
	{
		WorkType = 3;
	}
	/**add by wangdp shoujiqianbao end**/	
	if (strcasecmp(WorkNo,"~~~~~")==0)
	{
		WorkType = 1;
	}
	return WorkType;
}


/******************************
获得客户姓名
******************************/
int GetCustDoc(char *InCustId,char *OutCustName)
{
	EXEC SQL BEGIN DECLARE SECTION;
	   char  CustName[60+1];
	   char  CustId[22+1];
	EXEC SQL END DECLARE SECTION;
	
	init(CustName);
	init(CustId);
	strcpy(CustId, InCustId);
	
	EXEC SQL select cust_name into :CustName 
	         from   dCustDoc
	         where  cust_id=to_number(:CustId);       
	if(SQLCODE!=0)
	{
		printf("\n[%s],[%d]",SQLERRMSG,SQLCODE);
		return 131002;
	}        
	strcpy(OutCustName,CustName);
	return 0;
}

int PublicGetRunName(char *run_code,char *region_code,char *run_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	   char  RunName[12+1];
	   char  RunCode[1+1];
	   char  RegionCode[2+1];
	EXEC SQL END DECLARE SECTION;
	init(RunName);
	init(RunCode);
	init(RegionCode);
	strcpy(RunCode,run_code);
	strcpy(RegionCode,region_code);
	
	EXEC SQL SELECT run_name 
	         INTO   :RunName
	         FROM   sRunCode
	         WHERE  region_code = :RegionCode
	         AND    run_code = :RunCode;
	if (SQLCODE!=0)
	{
		printf("[%d][%s]\n",SQLCODE,SQLERRMSG);
		strcpy(run_name,"未知");
		return 0;
	}
	strcpy(run_name,RunName);
	return 0;
}


int PublicGetLimitOwe(char *InBelongCode,char *InAttrCode,char *InSmCode,
                      char *InCreditCode,double *OutLimitOwe)
{
	EXEC SQL BEGIN DECLARE SECTION;
			char   BelongCode[7+1];
			char   AttrCode[8+1];
			char   SmCode[2+1];
			char   CreditCode[1+1];
			double LimitOwe = 0.00;
	EXEC SQL END DECLARE SECTION;
	init(BelongCode);
	init(AttrCode);
	init(SmCode);
	init(CreditCode);
	strcpy(BelongCode,InBelongCode);
	strcpy(AttrCode,InAttrCode);
	strcpy(SmCode,InSmCode);
	strcpy(CreditCode,InCreditCode);
	
	printf("\n [%s][%s][%s][%s]",InBelongCode,InAttrCode,InSmCode,InCreditCode);
	EXEC SQL SELECT over_fee
		     INTO   :LimitOwe
		     FROM   sCardCodeStop
		     WHERE  region_code=substr(:BelongCode,1,2)
		     AND    card_type=substr(:AttrCode,3,2);
	
	if(SQLCODE!=0&&SQLCODE!=NOTFOUND)
	{
	    printf("查sCardCodeStop出错[%d][%s]\n",SQLCODE,SQLERRMSG);
	    return 12008;
	}				        
	else if(SQLCODE==NOTFOUND)
	{
		EXEC SQL SELECT over_fee
		         INTO   :LimitOwe
		         FROM   sSmCodeStop
		         WHERE  region_code=substr(:BelongCode,1,2)
		         AND    sm_code=:SmCode;
		if(SQLCODE!=0&&SQLCODE!=NOTFOUND)
		{
			printf("查sSmCodeStop出错[%d][%s]\n",SQLCODE,SQLERRMSG);
			return 12008;
		}	
		if(SQLCODE==NOTFOUND)
		{				       
		    EXEC SQL SELECT over_fee
		             INTO   :LimitOwe
		             FROM   sCreditCode
		             WHERE  region_code=substr(:BelongCode,1,2)
		             AND    credit_code=:CreditCode;
		    if(SQLCODE!=0)
		    {
				printf("查sCreditCode出错[%d][%s][%s][%s]",SQLCODE,SQLERRMSG,BelongCode,CreditCode);
				return 12008;
		    }
		}			    
	}
	*OutLimitOwe = LimitOwe; 
	return 0;
}


  
int GoStop(long in_contract_no,long in_id_no,char *in_phone_no,char *in_run_code,
           char *in_sm_code,char *in_attr_code,char *in_belong_code,long in_cust_id,
           long in_login_accept,char *in_op_code,char *in_login_no,char *in_org_code)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long contract_no = 0;
		long id_no = 0;
		char phone_no[15+1];
		double  should_pay=0;
		int stop_flag=0;
		char TmpSqlStr[2000+1];
		char run_code[2+1];
		char sm_code[2+1];
		char attr_code[8+1];
		char belong_code[7+1];
		long login_accept = 0;
		char op_code[4+1];
		char login_no[6+1];
		char org_code[9+1];
		int  err_code=0;
		int  ids=0;
		char  op_time[17+1];
		long  cust_id=0;
		char year_month[6+1];
		char total_date[8+1];
		
		int  ret=0;
		tChgList  chgList;
		long offon_accept=0;
		char GroupId[10+1];
		char OrgId[10+1];
	  	char ChgReson[256+1];
	  	char return_message[256];
	EXEC SQL END DECLARE SECTION;
	init(run_code);
	init(year_month);
	init(op_time);
	init(total_date);
	init(GroupId);
	init(OrgId);
	init(ChgReson);
	init(return_message);
	memset(&chgList,0,sizeof(chgList));
  
	strcpy(run_code,in_run_code);
	id_no = in_id_no;
	cust_id = in_cust_id;
	contract_no = in_contract_no;
	strcpy(phone_no,in_phone_no);
	strcpy(sm_code,in_sm_code);
	strcpy(attr_code,in_attr_code);
	strcpy(belong_code,in_belong_code);
	login_accept = in_login_accept;
	strcpy(op_code,in_op_code);
	strcpy(login_no,in_login_no);
	strcpy(org_code,in_org_code);
	
	
	EXEC SQL select to_char(sysdate,'YYYYMMDD HH24:MI:SS') into :op_time from dual;
	if(SQLCODE!=0)
	{
		printf("取系统时间出错SQLCODE[%d]SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
		err_code=12012;
		return -1;
	}
	strncpy(year_month,op_time,6);
	strncpy(total_date,op_time,8);
	
	EXEC SQL UPDATE dCustMsg 
	         SET    run_code=:run_code,
	                run_time=to_date(:op_time,'YYYYMMDD HH24:MI:SS')
	         WHERE  id_no=:id_no;
	if(SQLCODE!=0)
	{
		printf("更新用户资料SQLCODE[%d]SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
		printf("run_code[%s]op_time[%s]\n",run_code,op_time);
		return -1;            
	}
	/*--组织机构改造插入表字段edit by zhangxha at 04/03/2009--begin*/
	ret =0;
	ret = sGetUserGroupid(phone_no,GroupId);
	if(ret <0)
	{
		printf("获取用户group_id失败![%s]\n",phone_no);
		return -1;    
	}
	Trim(GroupId);
	ret =0;
	ret = sGetLoginOrgid(login_no,OrgId);
	if(ret <0)
	{
	  	printf("获取工号org_id失败![%s]\n",login_no);
	    return -1;    
	}
	Trim(OrgId);
	
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"insert into wUserMsgChg(op_no,op_type,id_no,cust_id,\
	                   phone_no,sm_code,attr_code,belong_code,\
	                   run_code,run_time,op_time,group_id ) values( \
	                   sMaxBillChg.NEXTVAL,'2',:v1,:v2,:v3,:v4,:v5,:v6,\
	                   :v7,to_char(sysdate,'yyyymmddhh24miss'),\
	                   to_char(sysdate,'yyyymmddhh24miss'),:v8)");     
	
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :id_no,:cust_id,:phone_no,:sm_code,
	                                :attr_code,:belong_code,:run_code,:GroupId;
	if(SQLCODE!=0)
	{
		printf("\n  TmpSql=[%s]",TmpSqlStr);
		printf("插入wUserMsgChg Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
		return  -1;            
	}         
	/***发开关机命令***/
	/*
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"insert into wChgList(command_id,id_no,total_date,login_accept,belong_code,sm_code,\
	                   attr_code,phone_no,run_code,old_time,op_time,org_code,\
	                   login_no,op_code,cmd_right,change_reason)\
	                   values(sOffOn.nextval,:v1,to_number(:v2),:v3,:v4,:v5,:v6,:v7,:v8,to_date(:v9,'YYYYMMDD HH24:MI:SS'),\
	                   to_date(:v10,'YYYYMMDD HH24:MI:SS'),:v11,:v12,:v13,1,'帐户交费')");
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt using :id_no,:total_date,:login_accept,:belong_code,:sm_code,
	                                :attr_code,:phone_no,:run_code,:op_time,:op_time,:org_code,
	                                :login_no,:op_code;
	*/
/***
	EXEC SQL EXECUTE IMMEDIATE :TmpSqlStr;
***/
	/*	
	if(SQLCODE!=0)
	{
		printf("\n  TmpSql=[%s]",TmpSqlStr);                                                        
		printf("插入 wChgList Err SQLCODE[%d],SQLERRMSG[%s]",SQLCODE,SQLERRMSG);
		return  -1;            
	}
	*/
	
	EXEC SQL select sOffOn.nextval into :offon_accept from dual;
	if (SQLCODE != SQL_OK) 
	{
		printf("取流水出错。[%d]\n",SQLCODE);
		return -1;
	}
	strcpy(ChgReson, "帐户交费");
	Trim(ChgReson);
	
	Trim(belong_code);
	Trim(sm_code);
	Trim(attr_code);
	Trim(phone_no);
	Trim(run_code);
	Trim(org_code);
	Trim(login_no);
	Trim(op_code);
	
	chgList.vCommand_Id          =  offon_accept;
	chgList.vId_No               =  id_no;
	chgList.vTotal_Date          =  atoi(total_date);
	chgList.vLogin_Accept        =  login_accept;
	
	strncpy(chgList.vBelong_Code,   belong_code,    7);
	strncpy(chgList.vSm_Code,       sm_code,        2);
	strncpy(chgList.vAttr_Code,     attr_code,      8);
	strncpy(chgList.vPhone_No,      phone_no,      15);
	strncpy(chgList.vRun_Code,      run_code,       2);
	strncpy(chgList.vOrg_Code,      org_code,       9);
	strncpy(chgList.vLogin_No,      login_no,       6);
	strncpy(chgList.vOp_Code,       op_code,        4);
	strcpy(chgList.vChange_Reason,  ChgReson		     );
	strncpy(chgList.vGroup_Id,      GroupId,   	   10);
	strncpy(chgList.vOrg_Id,        OrgId,         10);
  
	ret=0;
	ret = recordChgList(&chgList,return_message);
	if(ret != 0)
	{
		printf("调用函数recordChgList出错[%s]。",return_message); 
		return -1;
	}	  
 	/*---组织机构改造插入表字段edit by zhangxha at 04/03/2009---end*/
	return 0;                                          
}
int PublicInsertCheckPrepay(int InTotalDate, long InPayAccept, char *InOpCode,
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
int PublicUpdateAccountPrepayDead(long   InContractNo, char *InPayType, 
                              double InPrepay,double  InOweFee, char *InSysTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char   TmpSqlStr[1000+1];
		char   SysTime[19+1]; 
		char   PayType[1+1];
		int    vTotalDate = 0;
		double PrePay = 0.00;
		double PayedOwe = 0.00; 
		long   ContractNo = 0; 
		double OweFee=0.00;

	EXEC SQL   END DECLARE SECTION;
	init(TmpSqlStr);
	init(SysTime);
	init(PayType);
	PrePay = InPrepay;
	ContractNo = InContractNo;
	OweFee=InOweFee;

	strcpy(SysTime,InSysTime);
	strcpy(PayType,InPayType);
	printf("\ncomm hee");
	
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"UPDATE dConMsgDead SET prepay_fee=nvl((prepay_fee+round(:v1,2)),0),\
				owe_fee=owe_fee-round(:v2,2),\
	                          prepay_time=to_date(:v3,'YYYYMMDD HH24:MI:SS')\
	                   WHERE  contract_no=:v4");

	printf("<PublicUpdateAccountPrepay>[%s]\n", TmpSqlStr);                   
	EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
	EXEC SQL EXECUTE sql_stmt USING :PrePay,:OweFee,:SysTime,:ContractNo;
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
	sprintf(TmpSqlStr,"UPDATE dConMsgPreDead \
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
	        sprintf(TmpSqlStr,"INSERT INTO dConMsgPreDead(contract_no,pay_type,\
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


/**
判断是否为集团统一付费帐户
0为不是
1为是
***/
/*int  PubGrpAccount(long  in_contract_no)
{
    EXEC SQL BEGIN DECLARE SECTION;
	long  contract_no=0;
	int   flag=0;
    EXEC SQL   END DECLARE SECTION;
    contract_no=in_contract_no;
    EXEC SQL select  count(*) into :flag from dGrpUserMsg where account_id=:contract_no and bill_date>sysdate;
    if(SQLCODE!=0)
    {
    	printf("\n查询集团用户表出错,[%s][%d]",SQLERRMSG,SQLCODE);
    	return -1;
    }
    if(flag==0)  return 0;
    else   return 1;
}*/
int  PublicGrpPayOwe(long in_contract_no,long  in_pay_accept,int in_cur_ym,char *in_work_no,char *in_op_code)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char   TmpSqlStr[1000+1];
		char   SysTime[19+1]; 
		char   PayType[1+1];
		int    vTotalDate = 0;
		double PrePay = 0.00;
		double PayedOwe = 0.00; 
		long   contract_no = 0; 
		long   pay_accept=0;
		int    year_month=0;
		double  payed_owe=0.00;
		double  payed_prepay=0.00;
		double    delay_owe=0.00;
		char   payed_status[1+1];
		char   fee_code[2+1];
		char   detail_code[2+1];
		double payed_owe_det,payed_prepay_det;
		double  OweFee=0.00;
		int  all_payed_flag=0;   
		int  cur_ym=0; 
		char login_no[6+1];
		char op_code[4+1];
	EXEC SQL   END DECLARE SECTION;	
	contract_no=in_contract_no;
	pay_accept=in_pay_accept;
	cur_ym=in_cur_ym;
	init(login_no);
	init(op_code);
	strcpy(login_no,in_work_no);
	strcpy(op_code,in_op_code);
	init(TmpSqlStr);
	sprintf(TmpSqlStr,"select year_month,sum(payed_owe),sum(delay_owe),sum(payed_prepay)\
			from dCustPayOwe%6d%ld where contract_no=:v1 and login_accept=:v2 group by year_month",cur_ym,contract_no%10);
	EXEC SQL PREPARE s1300GrpCfmConStr2 FROM :TmpSqlStr;
	EXEC SQL DECLARE s1300GrpCfmConCur2 CURSOR FOR s1300GrpCfmConStr2;
	EXEC SQL OPEN    s1300GrpCfmConCur2 using :contract_no,:pay_accept; 
	for(;;)
	{  
		year_month=0;
		payed_owe=0.00;
		delay_owe=0.00;
		payed_prepay=0.00;
		init(payed_status);
		OweFee=0.00;
		all_payed_flag=0;
		EXEC SQL FETCH s1300GrpCfmConCur2 INTO :year_month,:payed_owe,:delay_owe,:payed_prepay; 
		if(SQLCODE==NOTFOUND||SQLCODE!=0)
		{
			break;
		}	

		if(year_month<200501) continue;
		if(payed_owe+payed_prepay+delay_owe>0.005||payed_owe+payed_prepay+delay_owe<-0.005)
		{
			printf("\n 111111111111111111111");
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"select should_pay-favour_fee-payed_prepay-payed_later,payed_status from \
					  dGrpOweTotal%6d where contract_no=:v1",year_month);
			EXEC SQL EXECUTE
			BEGIN
		    	    EXECUTE IMMEDIATE :TmpSqlStr INTO :OweFee,:payed_status using :contract_no;
		    	
			END;
			END-EXEC;
		        if (SQLCODE!=0)
		                 {
						#ifdef _DEBUG_        
						printf("\n[%s]select  dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;             
		                 }	
			if(OweFee==payed_owe+payed_prepay)
			{
				strcpy(payed_status,"2");
				all_payed_flag=1;
			}
		}
		else
		   break;	
		
		printf("\n payed_status=[%s]",payed_status);  	
		
		init(TmpSqlStr);
		sprintf(TmpSqlStr,"update dGrpOweTotal%6d set payed_prepay=payed_prepay+:v1,\
				   payed_later=payed_later+:v2 ,payed_status=:v3\
				   where contract_no=:v4",year_month);
		EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
		EXEC SQL EXECUTE sql_stmt using :payed_prepay,:payed_owe,:payed_status,:contract_no;
		if (SQLCODE!=0)
		                 {
						#ifdef _DEBUG_        
						printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;             
		                 }	
	
		if(all_payed_flag==1)
		{
			if(payed_prepay>0.005||payed_prepay<-0.005)
			{
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"update dGrpOwe%6d%ld set payed_prepay=should_pay-favour_fee-payed_later,\
					   payed_status=:v3\
					   where contract_no=:v4",year_month,contract_no%10);
			EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
			EXEC SQL EXECUTE sql_stmt using :payed_status,:contract_no;
			if (SQLCODE!=0)
			                 {
						#ifdef _DEBUG_        
						printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;     
			                 }				
			}
			else
			{
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"update dGrpOwe%6d%ld set \
					   payed_later=should_pay-favour_fee-payed_prepay,payed_status=:v3\
					   where contract_no=:v4",year_month,contract_no%10);
			EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
			EXEC SQL EXECUTE sql_stmt using :payed_status,:contract_no;
			if (SQLCODE!=0)
			                 {
						#ifdef _DEBUG_        
						printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;     
			                 }				
			}

			/*集团发票表*/
/***
  CONTRACT_NO   NUMBER (14)   NOT NULL, 
  LOGIN_ACCEPT  NUMBER (14)   NOT NULL, 
  ID_NO         NUMBER (14)   NOT NULL, 
  CUST_ID       NUMBER (10)   NOT NULL, 
  CON_CUST_ID   NUMBER (10)   NOT NULL, 
  YEAR_MONTH    NUMBER (6)    NOT NULL, 
  PRODUCT_TYPE  CHAR (4), 
  PRODUCT_CODE  CHAR (8), 
  FEE_CODE      CHAR (2)      NOT NULL, 
  DETAIL_CODE   CHAR (2)      NOT NULL, 
  PAYED_PREPAY  NUMBER (14,2) NOT NULL, 
  PAYED_LATER   NUMBER (14,2) NOT NULL,
  LOGIN_NO       CHAR (6)     NOT NULL, 
  OP_TIME        DATE	      NOT NULL, 
  OP_CODE        CHAR (4)     NOT NULL, 
  PRINT_FEE      NUMBER (14)  NOT NULL, 
  PRINT_FLAG     CHAR (1)     NOT NULL, 
  PRINT_ACCEPT   NUMBER (14)  NOT NULL,
**/
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"insert into dGrpPayOwe%ld \
				(CONTRACT_NO, LOGIN_ACCEPT, ID_NO, CUST_ID, CON_CUST_ID,\
				YEAR_MONTH, PRODUCT_TYPE, PRODUCT_CODE, FEE_CODE, DETAIL_CODE,\
				PAYED_PREPAY, PAYED_LATER, LOGIN_NO, OP_TIME, OP_CODE, PRINT_FEE,\
				PRINT_FLAG, PRINT_ACCEPT)\
				select contract_no,:v1,id_no,cust_id,con_cust_id,year_month,product_type,product_code,\
				fee_code,detail_code,sum(payed_prepay),sum(payed_later),:v2,sysdate,:v3,0,'0',:v4 \
				from dGrpOwe%6d%ld where contract_no=:v5 \
				group by contract_no,id_no,cust_id,con_cust_id,year_month,product_type,product_code,\
				fee_code,detail_code ",contract_no%10,year_month,contract_no%10);
			
			EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
			EXEC SQL EXECUTE sql_stmt using :pay_accept,:login_no,:op_code,:pay_accept,:contract_no;
			if (SQLCODE!=0)
			                 {
						#ifdef _DEBUG_        
						printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;     
			                 }							
			
			if(delay_owe>0.005)
			{
			    init(TmpSqlStr);
			    sprintf(TmpSqlStr,"insert into dGrpPayOwe%ld \
			    (CONTRACT_NO, LOGIN_ACCEPT, ID_NO, CUST_ID, CON_CUST_ID,\
			     YEAR_MONTH, PRODUCT_TYPE, PRODUCT_CODE, FEE_CODE, DETAIL_CODE,\
			      PAYED_PREPAY, PAYED_LATER, LOGIN_NO, OP_TIME, OP_CODE, PRINT_FEE,\
			      PRINT_FLAG, PRINT_ACCEPT) \
			    values(:v1,:v2,0,0,0,v3,'0000','0000','AA','AA',0,:v4,:v5,sysdate,'0',:v6,)",
			    contract_no%10);
			
				EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
				EXEC SQL EXECUTE sql_stmt using :contract_no,:year_month,:pay_accept,:login_no,:op_code,:pay_accept;
				if (SQLCODE!=0)
			        {
				#ifdef _DEBUG_        
				printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
				#endif   
				return -1;     
				}			    
			    
			}
		}
		else
		{
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"select fee_code,detail_code,sum(payed_later),sum(payed_prepay)\
					from dCustPayOweDet%8ld where contract_no=:v1 and login_accept=:v2  and year_month=:v3\
					 group by fee_code,detail_code",cur_ym*100+contract_no%100);
			EXEC SQL PREPARE s1300GrpCfmConStr3 FROM :TmpSqlStr;
			EXEC SQL DECLARE s1300GrpCfmConCur3 CURSOR FOR s1300GrpCfmConStr3;
			EXEC SQL OPEN    s1300GrpCfmConCur3 using :contract_no,:pay_accept,:year_month; 						
			for(;;)
			{
				init(detail_code);
				init(fee_code);
				payed_owe_det=0;
			    payed_prepay_det=0;
		
				EXEC SQL FETCH s1300GrpCfmConCur3 INTO :fee_code,:detail_code,:payed_owe_det,:payed_prepay_det; 
				if(SQLCODE==NOTFOUND||SQLCODE!=0)
				{
					break;
				}					
				if(payed_prepay_det>0.005||payed_prepay_det<-0.005)
				{
				init(TmpSqlStr);
				sprintf(TmpSqlStr,"update dGrpOwe%6d%ld set payed_prepay=payed_prepay+:v1,\
						   payed_status=:v2\
						   where contract_no=:v3 and fee_code=:v4 and detail_code=:v5",year_month,contract_no%10);
				EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
				EXEC SQL EXECUTE sql_stmt using :payed_prepay_det,:payed_status,:contract_no,:fee_code,:detail_code;
				if (SQLCODE!=0)
				                 {
						#ifdef _DEBUG_        
						printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
						#endif   
						return -1;     
				                 }				
				}
				else
				{
				init(TmpSqlStr);
				sprintf(TmpSqlStr,"update dGrpOwe%6d%ld set payed_later=payed_later+:v1,\
						   payed_status=:v2\
						   where contract_no=:v3 and fee_code=:v4 and detail_code=:v5",year_month,contract_no%10);
				EXEC SQL PREPARE sql_stmt FROM :TmpSqlStr;
				EXEC SQL EXECUTE sql_stmt using :payed_owe_det,:payed_status,:contract_no,:fee_code,:detail_code;
				if (SQLCODE!=0)
				                 {
							#ifdef _DEBUG_        
							printf("\n[%s]update dGrpOweTotal[%d],[%s]",TmpSqlStr,SQLCODE,SQLERRMSG);
							#endif   
							return -1;     
				                 }				
				}				
			}
			EXEC SQL CLOSE    s1300GrpCfmConCur3;
		}
	
	}
	EXEC SQL CLOSE    s1300GrpCfmConCur2;
	return 0;
}
int PubGrpOpCode( char *in_op_code)
{
     char op_code[4+1];
     init(op_code);
     strcpy(op_code,in_op_code);
     if ( 0 == strncmp(op_code,"139",3))
     {
     	return 1;
     }
     else
        return 0;
}


/****
系统后台批量赠送话费时发送短信通知函数
函数名称:vSystemSnedPreMsg
输入参数:
		用户号码
		操作流水
		通知短信内容
		操作代码
		工号
输出参数:
		0 --正常
		非0 --错误
****/
int vSystemSnedPreMsg(char *phone_no,long login_accept,char *send_msg,char *op_code,char *login_no)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	phoneNo[15+1];
	long	loginAccept=0;
	char	sendMsg[255+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	sqlStr[1024];
	
	EXEC SQL END DECLARE SECTION;
	
	memset(phoneNo,0,sizeof(phoneNo));
	memset(sendMsg,0,sizeof(sendMsg));
	memset(opCode,0,sizeof(opCode));
	memset(loginNo,0,sizeof(loginNo));
	memset(sqlStr,0,sizeof(sqlStr));
	
	strcpy(phoneNo,phone_no);
	loginAccept=login_accept;
	strcpy(sendMsg,send_msg);
	strcpy(opCode,op_code);
	strcpy(loginNo,login_no);
	
	init(sqlStr);
	sprintf(sqlStr," insert into WCOMMONSHORTPREMSG (COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,PHONE_NO,MSG,ORDER_CODE, "
				   " BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME ) "
				   " values(SMSG_SEQUENCE.nextval,:v1,100,:v2,:v3,0,0,:v4,:v5,sysdate)");
	EXEC SQL PREPARE sql_stmt FROM :sqlStr;
	EXEC SQL EXECUTE sql_stmt using :loginAccept,:phoneNo,:sendMsg,:opCode,:loginNo;
	if(SQLCODE != 0)
	{
		printf("insert WCOMMONSHORTPREMSG error[%d][%s]\n",SQLCODE,phoneNo);
		return -1;
	}
	
	return 0;	
}


int  DealdCustOweStop(long  in_contract_no, char *InRunTime)
{
EXEC SQL BEGIN DECLARE SECTION;
	long  contract_no=0;
	long  id_no=0;
	long  id_no2=0;
	int   query_type=0;
	char  phone_no[15+1];
	double owe_fee=0.00;
	double should_pay=0;
	double favour_fee=0.00;
	double all_owe=0.00;
	double remain_fee = 0.00;
	char   TmpSqlStr[2000];
	double owe_fee_tmp=0.00;
	double prepay_fee=0.00;
	int  RetValue;
	char RunTime[19+1];
EXEC SQL END DECLARE SECTION;

	Sinitn(RunTime);
	contract_no=in_contract_no;
	strcpy(RunTime, InRunTime);
	Trim(RunTime);
	
	EXEC SQL declare mystop_cur1  cursor  for 
								 select id_no 
								   from dConUserMsg 
								  where contract_no=:contract_no 
								  	and serial_no=0;
	EXEC SQL  open  mystop_cur1;
	for(;;)
	{
	    id_no=0;
	    init(phone_no);
	    owe_fee=0.00;
	    remain_fee=0.00;
	    EXEC SQL  fetch  mystop_cur1 into :id_no;
	    if(SQLCODE==1403)
	    {
	    	break;
	    }
	    else if(SQLCODE<0) 
		{
			EXEC SQL CLOSE mystop_cur1;
			printf("\n查询dConUserMsg出错");
			return -1;            
		}
	    EXEC SQL select phone_no 
	               into :phone_no 
	               from dCustMsg 
	              where id_no=:id_no;
	    if(SQLCODE!=0) 
		{
			printf("\n查询dCustMsg出错");
			return -1;            
		}
	    query_type=0;
	    RetValue=PublicGetUnbill(contract_no,id_no,query_type,phone_no,30,&owe_fee,&should_pay,&favour_fee);
	    if(RetValue<0)
	    {
	    	printf("\n取未出帐话费出错");
	    	return -1;
	    }
	    all_owe=all_owe+owe_fee;
	}
	EXEC SQL close  mystop_cur1;
	
	EXEC SQL select prepay_fee,owe_fee 
	           into :prepay_fee,:owe_fee_tmp 
	           from dConMsg 
			  where contract_no=:contract_no;
	if(SQLCODE!=0)
	{
		printf("\n[%ld]帐户不存在[%d]",contract_no,SQLCODE);
		return -1;
	}
	remain_fee=prepay_fee-owe_fee_tmp-all_owe;
	printf("\n prepay_fee [%12.2f]owe_fee_tmp [%12.2f]all_owe [%12.2f]\n",prepay_fee,owe_fee_tmp,all_owe);
	printf("\n remain_fee [%12.2f]\n",remain_fee);
    if(remain_fee>0)
	{
		init(TmpSqlStr);
		sprintf(TmpSqlStr,"select id_no from dConUserMsg  \
						   where contract_no=:v1 \
						   and   serial_no=0");
    	
		EXEC SQL PREPARE ConFlagStr11  FROM :TmpSqlStr;
		EXEC SQL DECLARE ConFlagCur12  CURSOR FOR ConFlagStr11;
		EXEC SQL OPEN ConFlagCur12 using :contract_no;
		for(;;)
		{
			id_no2=0;
			EXEC SQL FETCH ConFlagCur12 INTO :id_no2;
			if(SQLCODE==1403) 
			{
				break;
			}
			if(SQLCODE<0) 
			{
				EXEC SQL CLOSE ConFlagCur12;
				printf("\n查询dConUserMsg出错2");
				return -1;            
			}
		
		 	RetValue=UpdatedCustOweStop(id_no2,RunTime);          
			if (RetValue != SQL_OK) 
			{
				#ifdef _DEBUG_        
					printf("Fail---修改dCustOweStop出错\n");
				#endif                
				return -1;           
			}  
        }   	
		EXEC SQL CLOSE ConFlagCur12;
	}
	return 0;
}
int UpdatedCustOweStop(long InIdNo, char *InRunTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long IdNo = 0;
		char RunTime[19+1];
		char OldRunCode[1+1];
		char NewRunCode[2+1];
		char OweTableName[30+1];
		char TmpSqlStr[1024];
	EXEC SQL END DECLARE SECTION;

	Sinitn(RunTime);
	Sinitn(OldRunCode);
	Sinitn(NewRunCode);
	Sinitn(OweTableName);
	IdNo = InIdNo;
	strcpy(RunTime, InRunTime);
	
	sprintf(OweTableName, "dCustOweStop%ld",IdNo%100);

	
	init(TmpSqlStr);                   
	sprintf(TmpSqlStr,"select substr(run_code,2,1) FROM %s where id_no=:v1 ",OweTableName);

 
	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :TmpSqlStr INTO :OldRunCode using :IdNo;
	END;
	END-EXEC;
	if (SQLCODE != 0&&SQLCODE != 1403) 
	{
		printf("<UpdatedCustOweStop>查询%s错误！[%d]\n",OweTableName,SQLCODE);
		return 131005;
	}
	Trim(OldRunCode);
	printf("UpdatedCustOweStop--SQLCODE:%d\n",SQLCODE);
	if(strcmp(OldRunCode,"A")!=0)
	{
		sprintf(NewRunCode,"%sA",OldRunCode);
		Trim(NewRunCode);
		sprintf(TmpSqlStr, "UPDATE %s SET run_code=:v1, \
		                    run_time=to_date(:v2,'YYYYMMDD HH24:MI:SS') \
		                    WHERE id_no=:v3 ", 
		                    OweTableName);                    
		
		EXEC SQL PREPARE ins_stmt From :TmpSqlStr;
		EXEC SQL EXECUTE ins_stmt USING :NewRunCode,:RunTime,:IdNo;
    	
		if (SQLCODE != 0&&SQLCODE != 1403) 
		{
			
			printf("<UpdatedCustOweStop>修改%s错误！[%d]\n",OweTableName,SQLCODE);
			return 131006;
		}
	}
	
	
	return RET_OK;
}

   
     
/*出账不停营业改造  add by houxue  20090326*/
/*新增*/
int PublicGetPhoneFee(long InIdNo, long InContractNo,int InQueryYm,int InDelayFlag,int InDelayBegin,
                      float InDelayRate,char *InRunFlag,char *InBillFlag, double *OutOweFee, double *OutDelayFee,
                      double *OutShouldPay,double *OutFavourFee,double *OutPayedPrepaye,double *OutPayedLater,int *OutDataFlag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int   vQueryYm;
		int   vDelayFlag=0;
		int   vDelayBegin=0;
		int   vCurYm=0; 
		int   vUnbillYm=0;
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
	
	EXEC SQL select to_number(unbill_ym)
           	   into :vUnbillYm
           	   from cBillCond;
	if(SQLCODE!=0)
	{
		printf("\n检查出帐标志 err[%s][%d]",SQLERRMSG,SQLCODE);
		return 1;
	}
	
	init(OweTableName);
	sprintf(OweTableName, "dCustOweTotal%6d%ld",vQueryYm,vContractNo%10);
	init(TmpSqlStr);    
	printf("vQueryYm=[%d],vUnbillYm=[%d]\n",vQueryYm,vUnbillYm);
	if(vQueryYm==vUnbillYm&&strcmp(vRunFlag,"1")==0)              
	{
		sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0),\
		                   nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0), \
		                   nvl(sum(should_pay),0),nvl(sum(favour_fee),0), nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
		                   from %s \
		                   where id_no=:v5  and contract_no=:v6  \
		                   and payed_status=any('0','9') \
		                   and (bill_day<3600 or bill_day>9000) ", OweTableName);

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
/*出账不停营业改造  add by houxue  20090326*/
/*新增*/
int  PublicBillFlag(char *OutRunFlag,char *OutBillFlag)
{
	EXEC SQL BEGIN DECLARE SECTION;	
	  	char  vRunFlag[1+1];	
	  	char  vBillFlag[1+1];
	EXEC SQL   END DECLARE SECTION;
		init(vRunFlag);	
	  	init(vBillFlag);	

		EXEC SQL select run_flag,bill_flag 
           into  :vRunFlag,:vBillFlag 
           from  cBillCond;
		if(SQLCODE!=0)
		{
			printf("\n检查出帐标志 err[%s][%d]",SQLERRMSG,SQLCODE);
			return 1;
		}
		Trim(vRunFlag);
		Trim(vBillFlag);
		strcpy(OutRunFlag,vRunFlag);
		strcpy(OutBillFlag,vBillFlag);
        return  0;
}






/*************
function name:sUserPassWdChk
function desc:验证用户密码是否是弱密码
function input:
		 phone_no	用户号码
		 pass_word	用户密码（密文）
		 cust_id	客户ID
*************/
int sUserPassWdChk(char *phone_no,char *pass_word,char *cust_id,char *id_iccid)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	phoneNo[15+1];
	char	passWd[8+1];
	char	newpassWd[8+1];
	long	custId=0;
	int		icount=0;
	char	idIccid[20+1];
	
	EXEC SQL   END DECLARE SECTION;
	
	init(phoneNo);
	init(passWd);
	init(idIccid);
	
	strcpy(phoneNo,phone_no);
	strcpy(passWd,pass_word);
	strcpy(idIccid,id_iccid);
	custId=atol(cust_id);
	
	Trim(passWd);
	
	if(strcmp(passWd,"") == 0)
	{
		printf("密码为弱密码%s\n",passWd);
		return -2;
	}
	
	EXEC SQL SELECT count(1) INTO :icount FROM spasswordcfg WHERE pass_word=:passWd;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		printf("SELECT spasswordcfg ERROR %d\n",SQLCODE);
		return -1;
	}
	
	if(icount > 0)
	{
		printf("密码为弱密码%s\n",passWd);
		return -2;
	}
	
	unEncrypt(passWd, newpassWd);
	
	Trim(newpassWd);
	icount=0;
	EXEC SQL SELECT count(1) INTO :icount FROM dual WHERE :phoneNo like '%'||:newpassWd||'%';
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		printf("SELECT phone_no ERROR %d\n",SQLCODE);
		return -1;
	}
	
	if(icount > 0)
	{
		printf("密码为弱密码%s\n",passWd);
		return -2;
	}
	
	Trim(idIccid);
	icount=0;
	EXEC SQL SELECT count(1) INTO :icount FROM dual WHERE :idIccid like '%'||:newpassWd||'%';
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		printf("SELECT idIccid ERROR %d\n",SQLCODE);
		return -1;
	}
	
	if(icount > 0)
	{
		printf("密码为弱密码%s\n",passWd);
		return -2;
	}
	
	icount=0;
	EXEC SQL SELECT count(1) INTO :icount FROM dcustdoc WHERE cust_id=:custId AND id_iccid like '%'||:newpassWd||'%';
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		printf("SELECT dcustdoc ERROR %d\n",SQLCODE);
		return -1;
	}
	
	if(icount > 0)
	{
		printf("密码为弱密码%s\n",passWd);
		return -2;
	}
	
	return 0;
	
}
	
/***
取用户资料
****/
int  PublicGetCustMsgAll(char  *InPhoneNo,char *service_name,DCUSTMSG_TYPE *dCustMsg)
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
/****
		EXEC SQL select id_no, cust_id, contract_no, ids, phone_no, sm_code, 
				service_type, attr_code, user_passwd, open_time,
				belong_code, limit_owe, credit_code, credit_value,
	  			run_code, run_time, bill_date, bill_type, encrypt_prepay, cmd_right,
	   			last_bill_type, bak_field, group_id, stop_attr
	   		 into  :dCustMsg->id_no, :dCustMsg->cust_id, :dCustMsg->contract_no, :dCustMsg->ids, :dCustMsg->phone_no, :dCustMsg->sm_code, 
				:dCustMsg->service_type, :dCustMsg->attr_code, :dCustMsg->user_passwd, :dCustMsg->open_time,
				:dCustMsg->belong_code, :dCustMsg->limit_owe, :dCustMsg->credit_code, :dCustMsg->credit_value,
	  			:dCustMsg->run_code, :dCustMsg->run_time, :dCustMsg->bill_date, :dCustMsg->bill_type, 
	  			:dCustMsg->encrypt_prepay, :dCustMsg->cmd_right,
	   			:dCustMsg->last_bill_type, :dCustMsg->bak_field, :dCustMsg->group_id, :dCustMsg->stop_attr
	   		from  dCustMsgDead  where phone_no=:phone_no ;
   		if(SQLCODE==1403)
   		{
   			printf("\n this phoneno doesn't exist~~~");
   			return  130266;
   		}
   		else if(SQLCODE!=0)
   		{
   			printf("\n query dCustMsgDead error[%s][%d]~~~~",SQLERRMSG,SQLCODE);
   			return  130267;
   		}	
****/
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
	
	init(bill_code);
	id_no=dCustMsg->id_no;
	RetValue=PublicGetBillCode(id_no,bill_code);
	if(RetValue!=0)
	{
	      printf("\n quere bill_code [%d]",RetValue);
	      return RetValue;
	}	
	strcpy(dCustMsg->bill_code,bill_code);
		
	return 0;
}
int  PublicGetBillCode(long  InIdNo,  char *OutBillCode)
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
	sprintf(TmpSqlStr,"select mode_code from dBillCustDetail%ld "
	                  " where id_no=:v1 "
	                  " and   begin_time<=sysdate "
	                  " and   end_time>sysdate "
	                  " and   mode_flag='0' and mode_time='Y' and rownum < 2",
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
