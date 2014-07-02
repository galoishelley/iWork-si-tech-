/******************************************
* 版本: sxboss1.5/iboss1.8 (2004.11.10.1)
* 编者：caozm
* 说明：
* 缴费程序专用.h头文件，包括了所有缴费程序
* 调用的函数。另作为公用函数，提供给其他模
* 块使用。所有声明对应的定义为include13.cp
* 
******************************************/

#define MaxNumOfGroup   50
#define MaxNumOfCancel  400
#define MAX_MONTH       36
#define MAXBUFRECORDS   100
#define RET_OK          0
#define DETAILCODELEN   5
#define FEELEN          12
#define MaxContractNum 10000 /**王梅 060605 添加的***/
/**
#define MAXLINE         500
**/

#include <sys/timeb.h>

typedef struct
{
	char phone_no[15];
	long id_no;
	char sm_code[3];
	char belong_code[7+1];
	char group_id[10+1];
	double remonth_fee;
	double owe_fee;
}sPHONE;

typedef struct 
{
	char pay_type[1+1];
	double prepay_fee;
	double pay_money;
	double prepay_remain;
	double pay_remain;
}sCANCELFEE;

typedef struct 
{
	char phone_no[15];
	char pay_type[1+1];
	double used_prepay;
	double used_pay;
	double new_prepay;
	double cancel_owe;
	double cancel_delay;
	double cancel_remonth;
}sCANCELDET;

typedef struct
{
	int   vBill_Year;
	int   vBill_Month;
	int   vBegin_Day;
	int   vEnd_Day;
	double vCom_Fee;
	double vRe_Fee;
}Com_Fee;

typedef struct
{
	int   vBill_Year;
	int   vBill_Month;
	int   vBill_Day;
	double vCom_Fee;
	char  vBill_Type[2];
}Com_FeeTotal;

typedef struct
{
	long   vID_NO;
	long   vCon_NO;
	int   vBill_Year;
	int   vBill_Month;
	int   vBill_Day;
	double vCom_Fee;
	char  vBill_Type[2];
}Com_FeeTotal1;

/*  话音费结构体 */
typedef struct            
{
	int   vBill_Year;             /*  计费年       */
	int   vBill_Month;            /*  计费月       */
	int   vBegin_Day;             /*  记费开始天   */
	int   vEnd_Day;               /*  记费结束天   */
	double vVoice_Fee;             /*  每月话音费   */ 
}Voice_Fee;                          

/*  数据业务费结构体 */
typedef struct
{
	int   vBill_Year;             
	int   vBill_Month;            
	int   vBegin_Day;
	int   vEnd_Day;
	double vData_Fee;                        
}Data_Fee;

/*  商务费结构体 */
typedef struct                            
{
	int   vBill_Year;                       
	int   vBill_Month;                      
	int   vBegin_Day;
	int   vEnd_Day;
	double vBusi_Fee;                        
}Busi_Fee;

/*  移动秘书费结构体 */
typedef struct  
{
	int   vBill_Year;                       
	int   vBill_Month;                    
	int   vBegin_Day;
	int   vEnd_Day;
	double vMobSEC_Fee;                     
}MobSEC_Fee;                       

/*  信息费结构体 */
typedef struct
{
	int   vBill_Year;                     
	int   vBill_Month;                    
	int   vBegin_Day;
	int   vEnd_Day;
	double vMsg_Fee;                         
}Msg_Fee;

/**预存款明细**/
struct DCONMSGPRE_TYPE{
	char  payType[1+1];
	char  beginTime[8+1];
	char  endTime[8+1];
	double prepayFee;
	struct DCONMSGPRE_TYPE *next;
};
typedef struct DCONMSGPRE_TYPE	DCONMSGPRE_TYPE;

/***帐单明细表结构**/

struct DCUSTOWEDETAIL_TYPE{
	char  FeeCode[2+1];
	char  DetailCode[5+1];
	double ShouldPay;
	double FavourFee;
	double PayedPrepay;
	double PayedLater;
	struct DCUSTOWEDETAIL_TYPE *next;
};
typedef struct DCUSTOWEDETAIL_TYPE	DCUSTOWEDETAIL_TYPE;
/**
用户资料
ID_NO, CUST_ID, CONTRACT_NO, IDS, PHONE_NO, SM_CODE, SERVICE_TYPE, ATTR_CODE,
 USER_PASSWD, OPEN_TIME, BELONG_CODE, LIMIT_OWE, CREDIT_CODE, CREDIT_VALUE, RUN_CODE,
 RUN_TIME, BILL_DATE, BILL_TYPE, ENCRYPT_PREPAY, CMD_RIGHT, LAST_BILL_TYPE, 
 BAK_FIELD, GROUP_ID, STOP_ATTR
***/

struct DCUSTMSG_TYPE{
	long  id_no;
	long  cust_id;
	long  contract_no;
	int   ids;
	char  phone_no[15+1];
	char  sm_code[2+1];
	char  bill_code[8+1];
	char  service_type[2+1];
	char  attr_code[8+1];
	char  user_passwd[8+1];
	char  open_time[17+1];
	char  belong_code[7+1];
	double limit_owe;
	char  credit_code[1+1];
	int   credit_value;
	char  run_code[2+1];
	char  run_time[17+1];
	char  bill_date[17+1];
	int   bill_type;
	char  encrypt_prepay[16+1];
	int   cmd_right;
	char  last_bill_type[1+1];
	char  bak_field[12+1];
	char  group_id[10+1];
	char  stop_attr[3+1];
};
typedef struct DCUSTMSG_TYPE  DCUSTMSG_TYPE;

/***
帐户资料
CONTRACT_NO, CON_CUST_ID, CONTRACT_PASSWD, BANK_CUST, ODDMENT, 
BELONG_CODE, PREPAY_FEE, PREPAY_TIME, STATUS, STATUS_TIME, 
POST_FLAG, DEPOSIT, MIN_YM, OWE_FEE, 
ACCOUNT_MARK, ACCOUNT_LIMIT, PAY_CODE, BANK_CODE, 
POST_BANK_CODE, ACCOUNT_NO, ACCOUNT_TYPE, GROUP_ID

****/
struct DCONMSG_TYPE{
	long  contract_no;
	long  con_cust_id;
	char  contract_passwd;
	char  bank_cust[60+1];
	double   oddment;
	char  belong_code[7+1];
	double prepay_fee;
	char  prepay_time[17+1];
	char  status[1+1];
	char  status_time[17+1];
	char  post_flag[1+1];
	double deposit;
	int   min_ym;
	double owe_fee;
	int  account_mark;
	char account_limit[1+1];
	char pay_code[1+1];
	char bank_code[5+1];
	char post_bank_code[5+1];
	char account_no[30+1];
	char account_type[1+1];
	char group_id[10+1];
};
typedef struct DCONMSG_TYPE DCONMSG_TYPE;
/***
操作员记录表
***/
struct  DLOGINMSG_TYPE{
	char login_no[6+1];
	char login_name[20+1];
	char password[16+1];
	char op_code[4+1];
	int  total_date;
	long login_accept;
	char org_code[9+1];
	char org_id[10+1];
	char group_id[10+1];
	char ip_address[15+1];
	char op_note[60+1];
	char op_time[19+1];
	char pay_type[1+1];
	double pay_money;
	char end_dt[8+1];
	double allow_pay;
	char unpay_end[8+1];
	float remonth_fav_rate;
	float delay_fav_rate;
	long  login_accept_cx;
};
typedef struct DLOGINMSG_TYPE DLOGINMSG_TYPE;

/*********               
滞纳金优惠表
**********/
struct DDELAYFAVOUR_TYPE{
    long   contract_no;
    long   id_no;
    int    year_month;
    double delay_fee;
    char   favour_type[1+1];
    double favour_fee;
    double  favour_rate;
    int    total_date;
    long   login_accept;
    char   login_no[6+1];      
    char   org_code[9+1];      
};
typedef struct DDELAYFAVOUR_TYPE  DDELAYFAVOUR_TYPE;


/***
综合帐单表
****/

struct DCUSTOWETOT_TYPE{
	long contract_no;
	long id_no;
	int  year_month;
	char phone_no[15+1];
	char sm_code[2+1];
	char bill_code[8+1];
	char attr_code[8+1];
	char group_id[10+1];
	double should_pay;
	double favour_fee;
	double payed_prepay;
	double payed_later;
	double delay_fee;
	int    times;
	int    durations;
	char   bill_begin[17+1];
	char   bill_end[17+1];
	char   payed_status[1+1];
};

typedef struct DCUSTOWETOT_TYPE  DCUSTOWETOT_TYPE;

/*struct  INPUTPARM_TYPE
   {
               long        contract_no;
               int       year_month;
               char        show_code[8+1];
               char      print_type[1+1];
               char        work_no[6+1];
               char      org_code[9+1];
               char      op_code[4+1];
};

typedef struct INPUTPARM_TYPE INPUTPARM_TYPE;*/

struct  INPUTPARM_TYPE
   {
               long        contract_no;
               int       year_month;
               char        show_code[8+1];
               char      print_type[1+1];
               char        work_no[6+1];
               char      org_code[9+1];
               char      op_code[4+1];
               long       login_accept;
               int      pay_month;
               int       total_date;
               long     cust_id;
};
typedef struct INPUTPARM_TYPE INPUTPARM_TYPE;

#ifdef _extern_
extern double round_F(double source, int num);
extern int PublicUpdateAccountPrepay(long   InContractNo, char *InPayType, 
                       double InPrepay,     char *InSysTime, 
                       double InPayedOwe);
extern int PublicUpdateAccountPrepayAndYm(long   InContractNo, char *InPayType, 
                       double InPrepay,     char *InSysTime, 
                       double InPayedOwe,int InMinYm);
extern int GetAccountMsgOwe(long InContractNo, double *OutPrepayFee, char *OutPayCode);
extern int GetAccountMsgOweAll(long  InContractNo,   double *OutPrepayFee,
				char  *OutPayCode,    int    *OutMinYm,
				char  *OutBelongCode, int    *OutDelayFlag,
				float *OutDelayRate,  int    *OutDelayBegin);
extern int GetAccountMinYm(long InContractNo, int  *OutMinYm,
			char *OutPayCode,  char *OutBelongCode);
extern int PublicSendOffon(long InContractNo, long InIdNo, char *InSmCode, 
			char *InRegionCode, char *InOpCode, double InPayedPrepay,
			int InTotalDate, long InPayAccept, char *InPhoneNo, 
			char *InOrgCode, char *InWorkNo, double InPayMoney, 
			double InStillOwe, double InStillRemonth, double InLimitOwe,
 			double InNoAccountFee,char *OutUpdateRunCode,char *OutUpdateRunTime);
extern int PublicUpdateCustMsgRunCode(long InIdNo, char *InRunCode, char *InRunTime);
extern int GetPhoneOwe(long   InIdNo,         double *OutStillOwe,
         double *OutRemonthFee ,double *OutDelayFee,
         int    *OutOweMonth,   double *OutShouldPay,
         double *OutFavourFee,  double *OutPayedPrepay, 
         double *OutPayedLater);
/**
extern int GetPhoneDelay(long InIdNo, long InContractNo, double *OutDelayFee);
**/
extern int GetPhoneFee(long InIdNo, long InContractNo, double *OutOweFee, double *OutDelayFee);
extern int GetCurYm();
extern int GetCurDate(int *InTotalDate);
extern int PublicPayOwe1(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InPhoneNo,
                 double InPrepayRemain,  double InPayRemain
                 float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutPayedOwe,    double *OutDelayFee,
                 double *OutUsedPay,     double *OutUsedPrepay);
                 
extern int PublicPayOwe(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee);
extern int PublicGetFeeCode(long InIdNo,     long InContractNo, 
              char *InPayType, char *OutFeeText);
extern int PublicInsertCustPayOwe(long   InIdNo,       long   InContractNo,
                    int    InYearMonth,  long   InPayAccept, 
			       char   *InPayType,   char   *InBillType, 
			       int    InBillDay,    char   *InPhoneNo, 
			       int    InTotalDate,  long   InPayAcceptCon, 
			       double InOweFee,     double InDelayFee,
			       double InBadFee,     double InDeadFee,
			       double InPayedLater, char   *InWorkNo,
			       char   *InOpCode,    double InPrintFee,
			       char   *InPrintFlag, long   InPrintAccept,
			       char   *InOweStatus, char   *InFeeText);
extern int PublicUpdateCustOweTotal2(double InPayedLater, char *InOweStatus, 
                      long   InIdNo,       long InContractNo, 
                      int    InBillDate,   char *InBillType, 
                      int    InBillDay ,double  );
extern int PublicUpdateExpireTime(DCUSTMSG_TYPE *dCustMsg,DLOGINMSG_TYPE *dLoginMsg,int  InDays);

extern int PublicUndoExpireTime( long InIdNo, int InPayDate,long InPayAccept,
                   int InTotalDate, long InLoginAccept,
                   char *InOpCode, char *InLoginNo,
                   char *InOrgCode, char *InOpTime, char *InOpNote);
extern int PublicSendOffonAccount(long InContractNo,long InIdNo, char *InSmCode, char *InRegionCode, 
 char *InOpCode, double InPayedPrepay, int InTotalDate,
 long InPayAccept, char *InPhoneNo, char *InOrgCode, char *InWorkNo,
 double InPayMoney, double InStillOwe, double InStillRemonth, double InLimitOwe,
 double InNoAccountFee,double InCurPrepay);
extern int PublicGetBankWorkNo(char *InWorkNo);
extern int PublicIsCardPayWorkNo(char *InWorkNo);
extern int GetPhoneAccount(char *InPhoneNo,long *OutContractNo,long *OutIdNo,
             char *OutAttrCode,char *OutCreditCode,
								char *OutRunCode,int *OutReopFlag);
extern int PublicCheckDuplicatePay(char *InPaySign, long *OutPayAccept, int *OutTotalDate);
extern int PublicRecordOprRec(char   *InPayType,    double InPayMoney, 
                float  InDelayRate,   double InDelayFee,
                float  InRemonthRate, double InRemonthFee, 
                int    InTotalDate,   long    InPayAccept, 
                long   InIdNo,        char   *InPhoneNo, 
                char   *InSmCode,     char   *InWorkNo, 
                char   *InOrgCode,    char   *InOpCode, 
                char   *InPayNote,    char   *InBankCode, 
                char   *InCheckNo,    char   *InPaySign);
extern int PublicInsertPaySign(int InTotalDate, char *InPaySign, long InPayAccept);
extern int PublicInsertOprRecord(int  InTotalDate, long InPayAccept,  char *InOpCode,
                          char *InPayType,  double InPayMoney, char *InSmCode, 
                          long InIdNo,      char *InPhoneNo,   char *InOrgCode,
        				  char *InWorkNo,   char *InPayNote); 				  
extern int PublicInsertOprRecord2(int  InTotalDate, long InPayAccept,  char *InOpCode,
                   char *InPayType,  double InPayMoney, char *InSmCode, 
                   long InIdNo,      char *InPhoneNo,   char *InOrgCode,
 				  char *InWorkNo,   char *InPayNote,char *InOrgId);
extern int PublicInsertFavourRecord(double InRate, double InFee, char *InFavourCode, 
                             int InTotalDate, long InPayAccept, long InIdNo, 
                             char *InPhoneNo, char *InSmCode, char *InWorkNo, 
                             char *InOrgCode, char *InOpCode, char *InPayNote); 
extern int PublicInsertFavourRecord2(double InRate, double InFee, char *InFavourCode, 
                      int InTotalDate, long InPayAccept, long InIdNo, 
                      char *InPhoneNo, char *InSmCode, char *InWorkNo, 
                      char *InOrgCode, char *InOpCode, char *InPayNote,char *InOrgId);
extern int InsertWpaypre(long InLoginAccept,int  InTotalDate,
           long InContractNo, char *InOrgCode,
           long InIdNo,char *InOrgId,char *InWorkNo);
extern int DeleteWpaypre(long InContractNo, long  InLoginAccept,int   InTotalDate,
           char *InOrgCode);
extern int PublicGetYearMonth(char *curdate);
extern int PublicGetRegionName(char *InBelongCode, char *OutDistrictName);
extern int PublicAddMon(int old);
extern int PublicDecMonth(int yearmonth,int num);
extern int AddMonth(int yearmonth,int addmonth);
extern int PublicGetUnbill(long ,long ,int , char *,int ,double *,double *,double *);
extern int PublicSendCardPayMsg(long InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney);			  
extern int PublicSendCardPayMsg3(long InLoginAccept,char *InPhoneNo,long InContractNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,double InOweFee,,double InPayMoneyBack);
extern int PublicGetWorkNoType(char *InWorkNo);
extern int GetCustDoc(char *InCustId,char *OutCustName);
extern int PublicGetRunName(char *run_code,char *region_code,char *run_name);
extern int PublicGetLimitOwe(char *InBelongCode,char *InAttrCode,char *InSmCode,
               char *InCreditCode,double *OutLimitOwe);
extern int PublicSendOffonAcc(long in_contract_no,char *in_op_code,char *in_login_no,
                       char *in_org_code,double in_prepay_fee,long pay_accept,
					   float  in_delay_rate,char *in_org_id);
extern int PublicSendOffonAcc2(long in_contract_no,char *in_op_code,char *in_login_no,
                char *in_org_code,double in_prepay_fee,long pay_accept,
			   float  in_delay_rate,float in_remonth_rate);
extern int GoStop(long in_contract_no,long in_id_no,char *in_phone_no,char *in_run_code,
    char *in_sm_code,char *in_attr_code,char *in_belong_code,long in_cust_id,
    long in_login_accept,char *in_op_code,char *in_login_no,char *in_org_code);
extern int PublicInsertCheckPrepay(int InTotalDate, long InPayAccept, char *InOpCode,
        char *InWorkNo, char *InOrgCode, char *InBankCode, char *InCheckNo,
        double InPayMoney, char *InPayNote);
extern int PublicInsertCheckPrepay2(int InTotalDate, long InPayAccept, char *InOpCode,
 char *InWorkNo, char *InOrgCode, char *InBankCode, char *InCheckNo,
 double InPayMoney, char *InPayNote,char *InOrgId);
extern int PublicUpdateAccountPrepayDead(long   InContractNo, char *InPayType, 
                              double InPrepay,double  InOweFee, char *InSysTime);
extern int PublicUpdateAccountPrepayDead2(long   InContractNo, char *InPayType, 
                       double InPrepay, char *InSysTime);
extern int PublicGetLastDayOfMonth(int YearMonth);
extern int getLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);

extern int getPrintFeeCode(char *OutFeeCode,char *InPublishType);
extern int getRemonthFee(long in_id_no,double *fremonth_fee);
extern int getAcceptTotal(char *in_org_code,
             int *fv_pay_accept,char *fv_total_date);   
                                                                                                  					                                                                                                                      				                         										                                                                                           					                                                                     														                              
extern int Run1300CfmAcc(char *InWorkNo,char *InOrgCode,char *InOrgId,char *InOpCode,long InIdNo,
                  long InContractNo,double InPayMoney,char *InPayType,float InDelayRate,
                  float InRemonthRate,long InPayAccept,int InTotalDate,char *InPayNote,char *InPayTime,
                  sCANCELFEE *CANCELFEE,int PayTypeNum);

extern void InsertCancelDet(sCANCELDET *CANCELDET,char *phone_no,char *pay_type,double used_prepay,
                double used_pay,double cancel_owe,double cancel_delay,double cancel_remonth,
                double new_prepay);

extern int InsertwPay(sCANCELDET *CANCELDET,int DetNum,sPHONE *PHONE,int PhoneNum,long InContractNo,
               int InTotalDate,long InPayAccept,long InCustId,char *InWorkNo,char *InOrgCode,
               char *InOrgId,char *InPayTime,char *InOpCode,char *InOpNote,
               double InCurPrepay);
               
extern int RedoMinym(long in_contract_no,int in_update_ym,int in_min_min_ym,int *out_year_month);  

extern int GetRunNameAndRemonthFlag(char *run_code,char *region_code,char *run_name,char *remonth_flag);

extern int PublicPayRemonth(long InIdNo,long InContractNo,double InRemonthRate,long InPayAccept,
                     int InTotalDate,double InPrepayRemain,double InPayRemain,double *OutPayRemonth,
                     double *OutUsedPay,double *OutUsedPrepay);
extern void showmeCD(sCANCELDET *CANCELDET,int i);
extern void showmePH(sPHONE *PHONE,int i);
extern void showmeCF(sCANCELFEE *CANCELFEE,int i);
extern int GetAcceptDetail(long InContractNo,int InTotalDate,long InLoginAccept,
                    long *OutLoginAccept,int *OutTotalDate);                   
/*extern void get_input_one(FBFR32 *transIN, int *input_par_num, int begin_no,int record_no,char *dbconnect_name,char *connect_type);*/
extern int PublicGetInterest2(long in_contract_no,double *out_interest_fee);
extern int ComputeInterest2(double in_fee,char *in_op_time,double *out_fee);
extern int GetBackBaseMsg1356(char *InTotalDate,char *InLoginAccept,char *OutLoginOn,
                   char *OutOpCode,char *OutPayType,char *OutContractNo,
                   int *OutNumber,char *OutOpTime,char *OutCustId,char *InOpCode);
extern int GetCustMsg2(char *InPhoneNo, long *OutIdNo, int *OutNumber,
               long *OutCustId, char *OutBelongCode, char *OutGroupId, char *OutSmCode,
               long *OutContractNo,char *OutRunCode,double *OutLimitOwe);        
extern int GetPhoneOwe0(long  InIdNo, long InContractNo,double *OutStillOwe, double *OutRemonthFee ,
                 double *OutDelayFee, int *OutOweMonth, double *OutShouldPay,
                 double *OutFavourFee, double *OutPayedPrepay, double *OutPayedLater);                          
extern int InsertWpaypreFund(long InLoginAccept,int  InTotalDate,
                  long InContractNo, char *InOrgId,
                  long InIdNo);  
extern int getCancelFee(long InContractNo,char *InPayType,double InPayMoney,
               sCANCELFEE *CANCELFEE,int *FeeNum);

extern int getFundLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);               
extern int getConLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);
extern void printerr(char *filename,int line,char *errmsg,int errtype);  
extern int  PublicGetCustMsgAll(char  *,char *,DCUSTMSG_TYPE *);
extern int  PublicGetConMsg(long ,char *,DCONMSG_TYPE *);
extern int  PublicGetLoginMsg(char *,char *,char  *,
			DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *);
extern int  PublicBillFlag();
extern int PublicQueryPrepay(long  ,char  *,char  *,double  *);
extern int PublicRePayFlag(char *,DCONMSG_TYPE *,DLOGINMSG_TYPE *,
			int *,long *,int *);
extern int PublicGetPayCode(DCONMSG_TYPE *,int *,float *,int *);
extern int PublicSendOffonPhone(DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *);
extern int PublicRecordOprRecNew(DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *,
			 float  ,   double ,   float , double ,   char   *, 
                       char   *,    char   *);
extern int Public1300CfmCon( DCONMSG_TYPE * ,DCUSTMSG_TYPE *,
			DLOGINMSG_TYPE *,double ,double );
extern int Public1300CfmCon1( DCONMSG_TYPE * ,DCUSTMSG_TYPE *,
			DLOGINMSG_TYPE *,double ,double );			
extern int PublicInsertPay(DCUSTMSG_TYPE *dCustMsg,DCONMSG_TYPE *dConMsg,DLOGINMSG_TYPE *dLoginMsg,double InPrepayFee);
extern int PublicUpdateAccountYmNew(long   InContractNo,char *InPhoneNo, char *InSysTime,char  *InBackFlag,
					DLOGINMSG_TYPE  *dLoginMsg);
extern int PublicUpdateAccountPrepayAndYmTime(long   InContractNo, char *InPayType, 
                              double InPrepay,     char *InSysTime, 
                              double InPayedOwe,int InMinYm,char  *InImonthFlag,char *InDmonthFlag,
                              int InImonthNum,int InDmonthNum,char *,double ,char *);
extern int  PublicQueryPrePayDet(long  InContractNo,char  *InPayType,DCONMSGPRE_TYPE **detail);
extern int  PublicQueryLast(long   InContractNo,double *OutOweFee);
extern int  PublicQueryPayOwe(long  InContractNo,long  InIdNo,int  InMinYm,double InRemainMoney,
			char  *InPayType,double *OutRemainMoney,double *OutPayedOwe);
extern int  PublicQueryPayOweDet(long  InContractNo,long InIdNo,int InMinYm,DCUSTOWEDETAIL_TYPE **detail);
extern int  PublicSendOffonAccNew(DCONMSG_TYPE *,DLOGINMSG_TYPE  *);
extern int  PubChangePrepay(long  ,char  *,char *, long  ,char *,char  *,char *,DLOGINMSG_TYPE  *);
extern int  PublicQueryCustOweTotal(DCONMSG_TYPE *,long ,int ,DCUSTOWETOT_TYPE *);
extern int  PublicGetConUnbill(long  ,double *);
extern int PublicGetOtherOwe(long  ,long ,long *,double  *);
extern void PrintTimeConsume(struct timeb start,int lines,char *filename);
extern int PubBnQuery(char *InPhoneNo,double *TotalFav,double *FavouredFee,double *NoFavouredFee,char *FavEnd);
extern int PublicPayOweAdj(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee);
extern int PublicPayOweAdj(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee);  
                 
extern int PublicInsertCustPayOweAdj(long   InIdNo,       long   InContractNo,
                           int    InYearMonth,  long   InPayAccept, 
					       char   *InPayType,   char   *InBillType, 
					       int    InBillDay,    char   *InPhoneNo, 
					       int    InTotalDate,  long   InPayAcceptCon, 
					       double InOweFee,     double InDelayFee,
					       double InBadFee,     double InDeadFee,
					       double InPayedLater, char   *InWorkNo,
					       char   *InOpCode,    double InPrintFee,
					       char   *InPrintFlag, long   InPrintAccept,
					       char   *InOweStatus, char   *InFeeText) ;   
extern int recordOprLogBoss(tLoginOpr *log,char *retMsg,char *ChgFlag,char *CustFlag);					                                    
extern int PublicUpdateCustOweTotalAdj(double InPayedLater, char *InOweStatus, 
                             long   InIdNo,       long InContractNo, 
                             int    InBillDate,   char *InBillType, 
                             int    InBillDay ,double  InDelayFee);
extern int GetCustNote(long InIdNo,char *OutNoteStr);                           
extern int CheckUnPay(int  total_date,long login_accept,int pay_date,
                    long pay_accept,char *org_code,char *login_no,
                    char *op_code,char *op_note);
extern int GetShowPrepay(long InContractNo,double *OutPrepayFee);   
extern int PublicQueryLastPay(long   InContractNo,double *OutOweFee);
extern int PublicGetOtherOwePay(long  in_id_no,long in_contract_no,long *out_contract_no,double  *owe_fee);                 
extern int PublicGetPayPrepay(long InContractNo,double *OutPrepayFee);
extern int addPayAward(long idNo, const char *phoneNo,
		int totalDate, const char* opTime, long lLoginAccept, const char *opCode,
		const char *orgCode, const char* opNote, double dPrepayFee, char *retMsg,char *OutAwardId);
extern int PublicSendAwardMsg(long InLoginAccept,char *InPhoneNo,long InContractNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,char *InAwardNo,double InPayMoneyBack);
#endif



#ifndef _extern_
double round_F(double source, int num);
int PublicUpdateAccountPrepay(long   InContractNo, char *InPayType, 
                double InPrepay,     char *InSysTime, 
                double InPayedOwe);
int PublicUpdateAccountPrepayAndYm(long   InContractNo, char *InPayType, 
                double InPrepay,     char *InSysTime, 
                double InPayedOwe,int InMinYm);
int GetAccountMsgOwe(long InContractNo, double *OutPrepayFee, char *OutPayCode);
int GetAccountMsgOweAll(long  InContractNo,   double *OutPrepayFee,
		char  *OutPayCode,    int    *OutMinYm,
		char  *OutBelongCode, int    *OutDelayFlag,
		float *OutDelayRate,  int    *OutDelayBegin);
int GetAccountMinYm(long InContractNo, int  *OutMinYm,
	char *OutPayCode,  char *OutBelongCode);
int PublicSendOffon(long InContractNo, long InIdNo, char *InSmCode, 
			char *InRegionCode, char *InOpCode, double InPayedPrepay,
			int InTotalDate, long InPayAccept, char *InPhoneNo, 
			char *InOrgCode, char *InWorkNo, double InPayMoney, 
			double InStillOwe, double InStillRemonth, double InLimitOwe,
 			double InNoAccountFee,char *OutUpdateRunCode,char *OutUpdateRunTime);
int PublicUpdateCustMsgRunCode(long InIdNo, char *InRunCode, char *InRunTime);
int GetPhoneOwe(long   InIdNo,         double *OutStillOwe,
  	double *OutRemonthFee ,double *OutDelayFee,
  	int    *OutOweMonth,   double *OutShouldPay,
  	double *OutFavourFee,  double *OutPayedPrepay, 
  	double *OutPayedLater);
/**
int GetPhoneDelay(long InIdNo, long InContractNo, double *OutDelayFee);
**/
int GetPhoneFee(long InIdNo, long InContractNo, double *OutOweFee, double *OutDelayFee);
int GetCurYm();
int GetCurDate(int *InTotalDate);
int PublicPayOwe1(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InPhoneNo,
                 double InPrepayRemain,  double InPayRemain,
                 float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutPayedOwe,    double *OutDelayFee,
                 double *OutUsedPay,     double *OutUsedPrepay);
                 
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
                 double *OutDelayFee);
int PublicGetFeeCode(long InIdNo,     long InContractNo, 
              char *InPayType, char *OutFeeText);
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
	       char   *InOweStatus, char   *InFeeText);
int PublicUpdateCustOweTotal2(double InPayedLater, char *InOweStatus, 
               long   InIdNo,       long InContractNo, 
               int    InBillDate,   char *InBillType, 
               int    InBillDay ,double );
int PublicUndoExpireTime( long InIdNo, int InPayDate,long InPayAccept,
                   int InTotalDate, long InLoginAccept,
                   char *InOpCode, char *InLoginNo,
                   char *InOrgCode, char *InOpTime, char *InOpNote);
int PublicSendOffonAccount(long InContractNo,long InIdNo, char *InSmCode, char *InRegionCode, 
 char *InOpCode, double InPayedPrepay, int InTotalDate,
 long InPayAccept, char *InPhoneNo, char *InOrgCode, char *InWorkNo,
 double InPayMoney, double InStillOwe, double InStillRemonth, double InLimitOwe,
 double InNoAccountFee,double InCurPrepay);
int PublicGetBankWorkNo(char *InWorkNo);
int PublicIsCardPayWorkNo(char *InWorkNo);
int GetPhoneAccount(char *InPhoneNo,long *OutContractNo,long *OutIdNo,
    char *OutAttrCode,char *OutCreditCode,
						char *OutRunCode,int *OutReopFlag);
int PublicCheckDuplicatePay(char *InPaySign, long *OutPayAccept, int *OutTotalDate);
int PublicRecordOprRec(char   *InPayType,    double InPayMoney, 
                float  InDelayRate,   double InDelayFee,
                float  InRemonthRate, double InRemonthFee, 
                int    InTotalDate,   long    InPayAccept, 
                long   InIdNo,        char   *InPhoneNo, 
                char   *InSmCode,     char   *InWorkNo, 
                char   *InOrgCode,    char   *InOpCode, 
                char   *InPayNote,    char   *InBankCode, 
                char   *InCheckNo,    char   *InPaySign);
int PublicInsertPaySign(int InTotalDate, char *InPaySign, long InPayAccept);

int PublicInsertOprRecord(int  InTotalDate, long InPayAccept,  char *InOpCode,
                          char *InPayType,  double InPayMoney, char *InSmCode, 
                          long InIdNo,      char *InPhoneNo,   char *InOrgCode,
        				  char *InWorkNo,   char *InPayNote); 
int PublicInsertFavourRecord(double InRate, double InFee, char *InFavourCode, 
                             int InTotalDate, long InPayAccept, long InIdNo, 
                             char *InPhoneNo, char *InSmCode, char *InWorkNo, 
                             char *InOrgCode, char *InOpCode, char *InPayNote); 	
int PublicInsertOprRecord2(int  InTotalDate, long InPayAccept,  char *InOpCode,
            char *InPayType,  double InPayMoney, char *InSmCode, 
            long InIdNo,      char *InPhoneNo,   char *InOrgCode,
		  char *InWorkNo,   char *InPayNote,char *InOrgId);
int PublicInsertFavourRecord2(double InRate, double InFee, char *InFavourCode, 
               int InTotalDate, long InPayAccept, long InIdNo, 
               char *InPhoneNo, char *InSmCode, char *InWorkNo, 
               char *InOrgCode, char *InOpCode, char *InPayNote,char *InOrgId);
int InsertWpaypre(long InLoginAccept,int  InTotalDate,
    long InContractNo, char *InOrgCode,
    long InIdNo,char *InOrgId,char *InWorkNo);
int DeleteWpaypre(long InContractNo,long  InLoginAccept,int InTotalDate,
    char *InOrgCode);
int PublicGetYearMonth(char *curdate);
int PublicGetRegionName(char *InBelongCode, char *OutDistrictName);
int PublicAddMon(int old);
int PublicDecMonth(int yearmonth,int num);
int AddMonth(int yearmonth,int addmonth);
int PublicGetUnbill(long ,long ,int , char *,int ,double *,double *,double *);
int PublicSendCardPayMsg(long InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney);			  
int PublicSendCardPayMsg3(long InLoginAccept,char *InPhoneNo,long InContractNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,double InOweFee,double InPayMoneyBack);
int PublicGetWorkNoType(char *InWorkNo);
int GetCustDoc(char *InCustId,char *OutCustName);
int PublicGetRunName(char *run_code,char *region_code,char *run_name);
int PublicGetLimitOwe(char *InBelongCode,char *InAttrCode,char *InSmCode,
               char *InCreditCode,double *OutLimitOwe);
int PublicSendOffonAcc(long in_contract_no,char *in_op_code,char *in_login_no,
                       char *in_org_code,double in_prepay_fee,long pay_accept,
					   float  in_delay_rate,char *in_org_id);
int PublicSendOffonAcc2(long in_contract_no,char *in_op_code,char *in_login_no,
                char *in_org_code,double in_prepay_fee,long pay_accept,
			   float  in_delay_rate,float in_remonth_rate);
int GoStop(long in_contract_no,long in_id_no,char *in_phone_no,char *in_run_code,
    char *in_sm_code,char *in_attr_code,char *in_belong_code,long in_cust_id,
    long in_login_accept,char *in_op_code,char *in_login_no,char *in_org_code);
int PublicInsertCheckPrepay(int InTotalDate, long InPayAccept, char *InOpCode,
        char *InWorkNo, char *InOrgCode, char *InBankCode, char *InCheckNo,
        double InPayMoney, char *InPayNote);
int PublicInsertCheckPrepay2(int InTotalDate, long InPayAccept, char *InOpCode,
 	char *InWorkNo, char *InOrgCode, char *InBankCode, char *InCheckNo,
 	double InPayMoney, char *InPayNote,char *InOrgId);
int PublicUpdateAccountPrepayDead(long   InContractNo, char *InPayType, 
                              double InPrepay,double  InOweFee, char *InSysTime);
int PublicUpdateAccountPrepayDead2(long InContractNo, char *InPayType, 
                       double InPrepay, char *InSysTime);
int PublicGetLastDayOfMonth(int YearMonth);

int getLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);

int getPrintFeeCode(char *OutFeeCode,char *InPublishType);
int getRemonthFee(long in_id_no,double *fremonth_fee);
int getAcceptTotal(char *in_org_code,int *fv_pay_accept,char *fv_total_date);   
                                                                                                  					                                                                                                                      				                         										                                                                                           					                                                                     														                              
int Run1300CfmAcc(char *InWorkNo,char *InOrgCode,char *InOrgId,char *InOpCode,long InIdNo,
                  long InContractNo,double InPayMoney,char *InPayType,float InDelayRate,
                  float InRemonthRate,long InPayAccept,int InTotalDate,char *InPayNote,char *InPayTime,
                  sCANCELFEE *CANCELFEE,int PayTypeNum);

void InsertCancelDet(sCANCELDET *CANCELDET,char *phone_no,char *pay_type,double used_prepay,
                double used_pay,double cancel_owe,double cancel_delay,double cancel_remonth,
                double new_prepay);

int InsertwPay(sCANCELDET *CANCELDET,int DetNum,sPHONE *PHONE,int PhoneNum,long InContractNo,
               int InTotalDate,long InPayAccept,long InCustId,char *InWorkNo,char *InOrgCode,
               char *InOrgId,char *InPayTime,char *InOpCode,char *InOpNote,
               double InCurPrepay);
               
int RedoMinym(long in_contract_no,int in_update_ym,int in_min_min_ym,int *out_year_month);

int GetRunNameAndRemonthFlag(char *run_code,char *region_code,char *run_name,char *remonth_flag);  

int PublicPayRemonth(long InIdNo,long InContractNo,double InRemonthRate,long InPayAccept,
                     int InTotalDate,double InPrepayRemain,double InPayRemain,double *OutPayRemonth,
                     double *OutUsedPay,double *OutUsedPrepay);  
	                        
int GetAcceptDetail(long InContractNo,int InTotalDate,long InLoginAccept,
                    long *OutLoginAccept,int *OutTotalDate);   
                    
void showmeCD(sCANCELDET *CANCELDET,int i);
void showmePH(sPHONE *PHONE,int i);
void showmeCF(sCANCELFEE *CANCELFEE,int i);

/*void get_input_one(FBFR32 *transIN, int *input_par_num, int begin_no,int record_no,char *dbconnect_name,char *connect_type);*/
int PublicGetInterest2(long in_contract_no,double *out_interest_fee);
int ComputeInterest2(double in_fee,char *in_op_time,double *out_fee);      
int GetBackBaseMsg1356(char *InTotalDate,char *InLoginAccept,char *OutLoginOn,
                   char *OutOpCode,char *OutPayType,char *OutContractNo,
                   int *OutNumber,char *OutOpTime,char *OutCustId,char *InOpCode);  
int GetCustMsg2(char *InPhoneNo, long *OutIdNo, int *OutNumber,
               long *OutCustId, char *OutBelongCode, char *OutGroupId, char *OutSmCode,
               long *OutContractNo,char *OutRunCode,double *OutLimitOwe); 
int GetPhoneOwe0(long  InIdNo, long InContractNo,double *OutStillOwe, double *OutRemonthFee ,
                 double *OutDelayFee, int *OutOweMonth, double *OutShouldPay,
                 double *OutFavourFee, double *OutPayedPrepay, double *OutPayedLater);
int InsertWpaypreFund(long InLoginAccept,int  InTotalDate,
                  long InContractNo, char *InOrgId,
                  long InIdNo);
int getCancelFee(long InContractNo,char *InPayType,double InPayMoney,sCANCELFEE *CANCELFEE,int *FeeNum); 
int getFundLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);   
int getConLastPayTime(long in_id_no,long in_contract_no,
                   long in_login_accept,int in_total_date,char *in_pay_time,
                   long *last_accept,int *last_total,char *last_time,
                   int  *last_yearmonth);  
void printerr(char *filename,int line,char *errmsg,int errtype);
int  PublicGetCustMsgAll(char  *,char *,DCUSTMSG_TYPE *);
int  PublicGetConMsg(long ,char *,DCONMSG_TYPE *);
int  PublicGetLoginMsg(char *,char *,char  *,DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *);
int  PublicBillFlag(); 
int PublicQueryPrepay(long  ,char  *,char  *,double  *);                                                                   
int PublicRePayFlag(char *,DCONMSG_TYPE *,DLOGINMSG_TYPE *,
			int *,long *,int *);
int PublicGetPayCode(DCONMSG_TYPE *,int *,float *,int *);
int PublicSendOffonPhone(DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *);
int PublicRecordOprRecNew(DCONMSG_TYPE *,DCUSTMSG_TYPE *,DLOGINMSG_TYPE *,
			 float  ,   double ,   float , double ,   char   *, 
                       char   *,    char   *);
int PublicInsertPay(DCUSTMSG_TYPE *dCustMsg,DCONMSG_TYPE *dConMsg,DLOGINMSG_TYPE *dLoginMsg,double InPrepayFee);
int Public1300CfmCon( DCONMSG_TYPE * ,DCUSTMSG_TYPE *,
			DLOGINMSG_TYPE *,double ,double );
int Public1300CfmCon1( DCONMSG_TYPE * ,DCUSTMSG_TYPE *,
			DLOGINMSG_TYPE *,double ,double );			
int PublicUpdateAccountYmNew(long   InContractNo,char *InPhoneNo, char *InSysTime,char  *InBackFlag,
					DLOGINMSG_TYPE  *dLoginMsg);
int PublicUpdateAccountPrepayAndYmTime(long   InContractNo, char *InPayType, 
                              double InPrepay,     char *InSysTime, 
                              double InPayedOwe,int InMinYm,char  *InImonthFlag,char *InDmonthFlag,
                              int InImonthNum,int InDmonthNum,char *,double,char * );
int  PublicQueryPrePayDet(long  InContractNo,char  *InPayType,DCONMSGPRE_TYPE **detail);
int  PublicQueryLast(long   InContractNo,double *OutOweFee);
int  PublicQueryPayOwe(long  InContractNo,long  InIdNo,int  InMinYm,double InRemainMoney,
			char  *InPayType,double *OutRemainMoney,double *OutPayedOwe);
int  PublicQueryPayOweDet(long  InContractNo,long InIdNo,int InMinYm,DCUSTOWEDETAIL_TYPE **detail);
int  PublicSendOffonAccNew(DCONMSG_TYPE *,DLOGINMSG_TYPE  *);
int  PubChangePrepay(long  ,char  *,char *, long  ,
			char *,char *,char *,DLOGINMSG_TYPE  *);
int  PublicQueryCustOweTotal(DCONMSG_TYPE *,long ,int ,DCUSTOWETOT_TYPE *);
int  PublicGetConUnbill(long  ,double *);
int PublicGetOtherOwe(long  ,long ,long * ,double  *);
void PrintTimeConsume(struct timeb start,int lines,char *filename);
int PubBnQuery(char *InPhoneNo,double *TotalFav,double *FavouredFee,double *NoFavouredFee,char *FavEnd);
int PublicPayOweAdj(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee);
int PublicPayOweAdj(long   InIdNo,          long   InContractNo,    
                 int    InYearMonth,     char   *InRegionCode,
                 char   *InDistrictCode, char   *InPhoneNo,
                 double InRemainMoney,   float  InDelayFavourRate, 
                 char   *InWorkNo,       int    InTotalDate, 
                 long   InPayAccept,     long   InPayAcceptCon,   
                 char   *InPayType,      char   *InPayCode,
                 char   *InOpCode,       int    InDelayFlag,  
                 float  InDelayRate,     int    InDelayBegin,
                 double *OutRemainMoney, double *OutPayedOwe,
                 double *OutDelayFee);  
                 
int PublicInsertCustPayOweAdj(long   InIdNo,       long   InContractNo,
                           int    InYearMonth,  long   InPayAccept, 
					       char   *InPayType,   char   *InBillType, 
					       int    InBillDay,    char   *InPhoneNo, 
					       int    InTotalDate,  long   InPayAcceptCon, 
					       double InOweFee,     double InDelayFee,
					       double InBadFee,     double InDeadFee,
					       double InPayedLater, char   *InWorkNo,
					       char   *InOpCode,    double InPrintFee,
					       char   *InPrintFlag, long   InPrintAccept,
					       char   *InOweStatus, char   *InFeeText) ;   
					                                    
int PublicUpdateCustOweTotalAdj(double InPayedLater, char *InOweStatus, 
                             long   InIdNo,       long InContractNo, 
                             int    InBillDate,   char *InBillType, 
                             int    InBillDay ,double  InDelayFee);

int GetCustNote(long InIdNo,char *OutNoteStr);

int CheckUnPay(int  total_date,long login_accept,int pay_date,
                    long pay_accept,char *org_code,char *login_no,
                    char *op_code,char *op_note);
int recordOprLogBoss(tLoginOpr *log,char *retMsg,char *ChgFlag,char *CustFlag);
int GetShowPrepay(long InContractNo,double *OutPrepayFee);  
int PublicQueryLastPay(long   InContractNo,double *OutOweFee);
int PublicGetOtherOwePay(long  in_id_no,long in_contract_no,long *out_contract_no,double  *owe_fee);                    
int PublicGetPayPrepay(long InContractNo,double *OutPrepayFee);
int addPayAward(long idNo, const char *phoneNo,
		int totalDate, const char* opTime, long lLoginAccept, const char *opCode,
		const char *orgCode, const char* opNote, double dPrepayFee, char *retMsg,char *OutAwardId);
int PublicSendAwardMsg(long InLoginAccept,char *InPhoneNo,long InContractNo,char * InDxOpCode,char * InLoginNo,double InPayMoney,char *InAwardNo,double InPayMoneyBack);
#endif
