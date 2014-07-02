/*************************************************************
* * Auth: zhaohao
* * Date: 2002-03-01					      
* * Type: head file					      * 
*************************************************************/ 
#ifndef _PUBDEF_H 
#define _PUBDEF_H

/* define database user */
#define 	LABELBANK	"bank"
#define 	LABELOFFON	"offon"
#define 	LABELTUXEDO	"tuxdo"
#define 	LABELPRINTER	"printer"
#define 	LABELDBNEW	"dbnew"
#define 	LABELDBQUERY	"dbquery"
#define 	LABELDBANALY	"dbanaly"
#define 	LABELDBBANK	"dbbank"
#define 	LABELDBOFFON	"dboffon"
#define 	LABELDBCARD	"dbcar"
#define 	LABELDBCHANGE	"dbchange"
#define 	LABELDBOPEN	"dbopen"
#define 	LABELBIG_OPER	"big_oper"
#define 	LABELDBPAY	"dbpay"
#define 	LABELDBBILL	"dbbill"
#define 	LABELBILL	"bill"
#define 	LABELDBRPT	"dbrpt"
#define 	LABELDBMNG	"dbmng"
#define 	LABELDBAGENT	"dbagent"
#define 	LABELDBSIM	"dbsim"
#define 	LABELDBSERV1	"dbserv1"
#define 	LABELDBSERV2	"dbserv2"
#define 	LABELDBSYS	"dbsys"
#define 	LABELBOSSRPT	"bossrpt"
#define 	LABELOFFONRPT	"offonrpt"
#define 	LABELDBSA	"dbsa"
#define 	LABELDBSYB12	"dbsyb12"
#define 	LABELDBBILLCFG	"dbbillcfg"
#define 	LABELSIMC	"simc"
#define 	LABELSIMR	"simr"
#define 	LABELBOSSBILL	"bossbill"
#define 	LABELSADBPAY	"sadbpay"
#define 	LABELSBDBPAY	"sbdbpay"
#define 	LABELSCDBPAY	"scdbpay"
#define 	LABELDOFFON	"doffon"
#define         LABELDBESOP     "dbesop"
#define         LABELCIESOPJL     "ciesopjl"
#define		LABELDBEMCRM		"dbemeradm"
#define		LABELDBEMBOSS		"dbemeracc"
#define		LABELDBEM		"dbem"   
#define   LABELNEWHIS "dbnewhis" 
#define		LABELDBIS		"dbis"
#define		LABELDBHIS		"crmhis"
#define   LABELDBESOPSC     "dbesopsc"
#define		LABELDBCNTT	"dbcntt"
#define    ENCRYPTKEY "PLMOKNIJBUHYGVTFCRDXESZAQWqazwsxedcrfvtgbyhnujmikolp1234567890~`!@#$%^&*()-_+=|\\[]{}.,><?/;:"

#define 	CONNECT0	"0"
#define		CONNECT1	"1"

#define		COUNTBYPHONE  "a"
#define 	COUNTBYCONTRACT	"b"

#define	SYSACCEPT	0
#define	PAYACCEPT	1
#define OPENACCEPT	2

#define SALSEDEPARTMENTPAY 0
#define BANKPAY	1
#define CARDPAY 2
/* define len */
#define YEARLEN		4
#define MONTHLEN	2
#define DAYLEN		4
#define MAXOWERECORDS	10
#define MAXBUFRECORDS	100
#define MAXDETAILRECORDS	20
#define TMPLEN		512*3
#define REGIONCODELEN	2
#define REGIONNAMELEN 	20
#define DISTRICTCODELEN	2
#define DISTRICTNAMELEN	20
#define BILLFLAGLEN	1
#define REMONTHFLAGLEN	1
#define REMONTHFEELEN	12
#define SMCODELEN	2
#define SMNAMELEN	20
#define BILLCODELEN	8
#define BILLNAMELEN	20
#define RUNCODELEN	1
#define RUNNAMELEN	12
#define UNBILLFEELEN	12
#define PHONENOLEN	15
#define MAXPHONELEN     20
#define PHONEHEADLEN3	3
#define PHONEHEADLEN7	7
#define GROUPNOLEN      10
#define GROUPNAMELEN    40
#define GROUPLANGUAGELEN 1
#define GROUPADDRESSLEN 60
#define LOCALCODELEN    1
#define BILLDATELEN	6
#define BILLTYPELEN	1
#define BILLDAYLEN	4
#define FEELEN		12
#define OWEFEELEN	12
#define DELAYFEELEN	12
#define SHOULDPAYLEN	12
#define FAVOURFEELEN	12
#define PAYEDPREPAYLEN	12
#define PAYEDLATERLEN	12
#define TABLECHARLEN	3
#define MAXTABLENAMELEN 20
#define OWNERCODELEN	2
#define TYPENAMELEN	20
#define PAYCODELEN	8
#define DELAYDATELEN	8
#define RETURNCODELEN	4
#define TOTALDATELEN	8
#define PAYSIGNLEN	32
#define FAVOURCODELEN	4
#define WORKNOLEN 	6
#define ORGCODELEN	9
#define OPCODELEN	4
#define OPTYPELEN	1
#define PAYNOTELEN	60
#define OPNOTELEN	60
#define PAYTYPELEN	1
#define PAYNAMELEN	12
#define ENCRYPTFEELEN	16
#define FLAGLEN		1
#define ALLFEEFLAGLEN	6
#define SIGNFLAGLEN	1
#define CUSTNAMELEN	60
#define CUSTUNITLEN	60
#define BELONGCODELEN	9
#define PREPAYFEELEN	12
#define LIMITOWELEN	12
#define BANKCODELEN	5
#define CHECKNOLEN	20
#define RUNCODELEN	1
#define RUNNAMELEN	12
#define MAXTIMELEN	30
#define ATTRCODELEN	8
#define CHANGEFLAGLEN	1
#define FEECODELEN	2
#define FEECODENUM	100
#define PRINTFLAGLEN	1
#define FEETEXTLEN	FEECODENUM*(FEECODELEN+1)	
#define PAYEDSTATUSLEN	1
#define BACKFLAGLEN	1
#define CHGRESONLEN	60
#define ACCEPTLEN	12
#define OWEDAYLEN	5
#define CONFIGFILENAMELEN  20
#define LOGINNAMELEN 20
#define MAXCUSTARRAYSIZE 50
#define MAXQRYRESULTROW 1000
#define IDCARDLEN 20
#define OWNERRATELEN 1
#define OWNERRATENAMELEN 20
#define RATECODELEN 4
#define RATENAMELEN 20
#define MONTHCODELEN 1
#define PHONEPOSTFIXLEN 3
#define PAYCARDNOLEN 13
#define PAYCARDPWLEN 18
#define HIGHFEEFLAGLEN	1
#define MAXROWPRNLINE  120
#define MAXFEEITEMPERLINE 40
#define TOWNNAMELEN		60
#define ALLOWPAYLEN	8
#define ICCARDNOLEN	11
#define LAYOUTTYPELEN	5
#define LAYOUTITEMCODELEN	2
#define LAYOUTITEMNAMELEN	20
#define LAYOUTFEECODESETLEN	100
#define VIPFLAGLEN	1

/* the output fee column number is 2,and the max out put print line is less than 100*/
#define COLUMNNUM 	2
#define MAXPRINTLINE 	100
#define FREENAMELEN		20
#define RATENAMELEN		20

/*field name of tables */
#define SHOULDPAYLAYOUTSTRING	"should_pay"
#define FAVOURFEELAYOUTSTRING	"favour_fee"
#define CURRENTOWELAYOUTSTRING	"should_pay-favour_fee-payed_prepay-payed_later"

/* define error code */ 
#define RETURN_OK	"0000" 
#define RET_OK		0 
#define RET_CANT_APPLY_OTHER_DAY_UNPAY	3590
#define RET_CANT_APPLY_OTHER_OPERATOR_UNPAY	3591
#define RET_BILLING_ON	3592 
#define RET_BILLING_ERR	3595 
#define RET_REMONTH_ERR	3501
#define RET_UPDATE_REMONTH_ERR	3503
#define RET_UNBILLFEE_ERR	3491
#define RET_DCUSTOWESTAT_ERR	3333
#define RET_MAX_ACCEPT_ERR	1300
#define RET_INSERT_PAY_SIGN_ERR 3612
#define RET_GET_REMONTH_ERR	3501
#define RET_FETCH_REMONTH_ERR	3501
#define RET_FETCH_CUST_ERR	3405
#define RET_REFUNDABLE_PREPAY_NOT_EXIST	3415
#define RET_NOT_FIND_PAYED_MSG	3613

#define RET_INSERT_CON_REVIEW_ERR	3614
#define RET_GET_LOGIN_OPR_ERR 	1229
#define RET_CUST_NOT_EXIST	3401
#define RET_BILL_CODE_NOT_EXIST	1202
#define RET_BELONG_CODE_NOT_EXIST	2402
#define RET_PAY_TYPE_NOT_EXIST		3421
#define RET_GET_BILL_PAY_ERR		3425
#define RET_PAY_NOT_ALLOWED			3426
#define RET_RUN_CODE_NOT_EXIST		3431
#define RET_PAY_CODE_NOT_EXIST		3480
#define RET_TABLE_CHAR_NOT_EXIST	3620
#define RET_SRV_CODE_NOT_EXIST		3621
#define RET_OWNER_CODE_NOT_EXIST	3622
#define RET_SM_CODE_NOT_EXIST		3623
#define RET_TOWN_CODE_ERR			3624
#define RET_DELETE_UN_OWNER_DAY_TOT_ERR	3631
#define RET_UPDATE_CUST_FAV_ERR 	3630
#define RET_INSERT_TEMP_OWNER_ERR	3632
#define RET_GET_UN_OWNER_DAY_TOT_ERR	3633
#define RET_PAY_SIGN_EXIST		3617
#define RET_TOTAL_DATE_NOT_EXIST	3601
#define RET_LAST_TOTAL_DATE_NOT_EXIST	3701
#define RET_INSERT_W_LOGIN_FAVOUR_ERR	3702
#define RET_GET_TALLY_ID_ERR		3703
#define RET_GET_TALLY_ID_NULL		3704
#define RET_DELETE_TALLY_ID_ERR		3705
#define RET_INSERT_CUST_TALLY_ERR	3706
#define RET_1300_PAY_OWE_ERR		3336
#define RET_INSERT_CUST_OWE_TOTAL_ERR	3337
#define RET_UPDATE_CUST_OWE_TOTAL_ERR	3338
#define RET_UPDATE_CUST_OWE_ERR		3339
#define RET_UPDATE_CUST_ACC_PRE_ERR	3340
#define RET_UPDATE_CUST_MSG_ERR		1210
#define RET_INSERT_ACCOUNT_USER_OPR_ERR	3341
#define RET_GET_DEFAULT_ACCOUNT_ERR	3342
#define RET_ACCOUNT_FLAG_ERR		3343
#define RET_GET_CUST_ACCOUNT_ERR	3344
#define RET_GET_CONTRACT_NO_ERR		3707
#define RET_INSERT_LOGIN_OPR_ERR	1216
#define RET_INSERT_INVOICE_DAY_ERR	3708
#define RET_INSERT_PAY_DAY_ERR		3709
#define RET_GET_CUST_MSG_ERR		3401
#define RET_GET_PAY_STAT_CODE_ERR	3511
#define RET_INSERT_CUST_INTEREST_ERR	3532
#define RET_INSERT_CHG_LIST_ERR		3522
#define RET_INSERT_CUST_PAY_OWE_ERR	3334
#define RET_UPDATE_CUST_UNPAY_OWE_ERR	3331
#define RET_INSERT_CUST_UNPAY_OWE_ERR	3334
#define RET_SELECT_CUST_UNPAY_OWE_ERR	3332
#define RET_COUNT_ODD_ERR		3333
#define RET_DELETE_CUST_PAY_OWE_ERR	3332
#define RET_INSERT_CUST_PAY_OWE_DETAIL_ERR 3335
#define RET_GET_FEE_TEXT_ERR		3406
#define RET_GET_FEE_CODE_ERR		3407
#define RET_TOO_MANY_FEE_CODE		3408
#define RET_GET_TABLE_CHAR_MULTI_NOT_EXIT	3409
#define RET_GET_W_CON_REVIEW_NOT_EXIST	3450
#define RET_GET_W_CON_REVIEW_ERR	3451
#define RET_GET_LOGIN_MSG_ERR		3001
#define RET_TPALLOC_ERR				1105
#define RET_TPCALL_ERR				3900
#define RET_GET_EXT_FEE_ERR			3901
#define RET_INSERT_EXT_FEE_HIS_ERR	3902
#define RET_INSERT_EXT_FEE_ERR_ERR 	3903
#define RET_DELETE_EXT_FEE_ERR		3904

#define RET_POWER_ERR_OTHER_DAY_UNDO	3098
#define RET_POWER_ERR_OTHER_OPERATOR_UNDO	3099

#define RET_HAD_UNPAYED			3598
#define RET_GET_PAYED_ERR		3350
#define RET_GET_PAY_DAY_ERR		3351
#define RET_GET_PAY_TOTAL_ERR		3352
#define RET_INSERT_PAY_TOTAL_BACK_ERR	3353
#define RET_UPDATE_PAY_DAY_ERR		3354
#define RET_UPDATE_PAY_TOTAL_ERR	3355
#define RET_GET_DETAIL_INFORMATION_ERR	3370
#define RET_DETAIL_INFORMATION_NOT_EXIST	3371
#define RET_NO_ENOUGH_MEMO_SPACE	3372
#define RET_GET_FEE_CODE_LAYOUT_NOT_EXIST	3373
#define RET_GET_FEE_CODE_LAYOUT_ERR	3374
#define RET_GET_CJ_MSG_ERR		3375
#define RET_GET_CHG_LIST_ERR		2219
#define RET_CANT_REFUND_BECAUSE_OWE	3552
#define RET_CANT_REFUND_BECAUSE_ZERO	3555
#define RET_GET_REFUND_MSG_ERR		3556
#define RET_REFUND_OVERFLOW			3557
#define RET_GET_CUST_PAY_OWE_ERR	3585
#define RET_GET_CUST_PAY_OWE_DETAIL_ERR	3586
#define RET_INSERT_CHECK_OPR_ERR	3562
#define RET_UPDATE_CHECK_PREPAY_ERR 	2819	
#define RET_GET_CHECK_OPR_MSG_ERR	3560
#define RET_GET_TIANFU_PAYED_PREPAY_ERR	3726
#define RET_GET_PAYED_PREPAY_ERR	3727
#define RET_UPDATE_PAY_OWE_ERR		3728
#define RET_GET_S_PAY_TIME_ERR		3730
#define RET_GET_S_PAY_TIME_MAX_ERR_OR_CUST_EXPIRE_ERR	3731
#define RET_UPDATE_CUST_EXPIRE_ERR	3732
#define RET_INSERT_W_EXPIRE_LIST_ERR 	3733
#define RET_GET_W_EXPIRE_LIST_ERR	3734
#define RET_GET_PRE_DEL_MONEY_NOT_EXIST	3735
#define RET_GET_PRE_DEL_MONEY_ERR	3736
#define RET_INSERT_PRE_DEL_MONEY_ERR	3737
#define RET_UPDATE_PRE_DEL_MONEY_ERR	3738
#define RET_GET_CUST_OWE_ERR 1633
#define RET_GET_CUST_OWE_TOTAL_ERR 1634
#define RET_GET_CUST_OWE_TOTAL_DURATION_ERR 1635
#define RET_GET_CUST_OWE_BAD_ERR 1643
#define RET_UPDATE_CUST_OWE_BAD_TOTAL_ERR 1644
#define RET_UPDATE_CUST_OWE_BAD_ERR 1645
#define RET_GET_CUST_OWE_DEAD_ERR 1653
#define RET_UPDATE_CUST_OWE_DEAD_TOTAL_ERR 1654
#define RET_UPDATE_CUST_OWE_DEAD_ERR 1655
#define RET_INSERT_PAY_REVIEW_ERR	1498
#define RET_GET_PAY_REVIEW_ERR 1660
#define RET_PRINT_ERR  1661
#define RET_SMAP_BACK_ERR 1662
#define  RET_SMAP_BACK_NOT_EXIST 1663
#define  RET_INSERT_SMAP_SEND_ERR 1664
#define  RET_INSERT_CUST_MSG_HIS_ERR	1332
 
#define RET_OPERATOR_NOT_FOUND 1601
#define RET_OPERATOR_ERR 1600
#define RET_TOO_MANY_ROWS 1602
#define RET_POWER_NOT_FOUND 1603
#define RET_GET_POWER_ERR 1604
#define RET_POWER_RIGHT_AUTH_ERR 1605
#define RET_CUST_OWE_ADJ_ERR 1628
#define RET_CUST_OWE_ADJ_TOTAL_ERR 1629
#define RET_GET_WREBILLLIST_ERR 2600
#define RET_OWNER_RATE_NOT_EXIST 2601
#define RET_BILL_RATE_NOT_EXIST 2602

#define RET_GET_MSG_OWE_ERR 2604
#define RET_GET_FUNC_OWE_ERR 2605
#define RET_PHONE_POSTFIX_ERR 2606
#define RET_GET_PAYCARD_PASSWORD_ERR 2704
#define RET_PAY_CARD_PW_AUTH_ERR 2706
#define RET_GET_PAYCARD_BEAU_ERR 2707
#define RET_CONNECT_DB_PAYCARD_ERR 1801
#define RET_EXECUTE_PROC_CARDPHONELOG_ERR 2705
#define RET_CONNECT_DB_CHANGE_ERR 1802
#define RET_CONNECT_DB_PAY_ERR 1803
#define RET_QRY_CITY_LIST_ERR 2708
#define RET_CARD_PAY_INFO_NOT_EXIST 2709
#define RET_EXEC_PROC_NOUSER_ERR 2750
#define RET_GET_GROUP_DOC_NOT_EXIST     3720
#define RET_GET_GROUP_DOC_ERR           3721
#define RET_GET_CUST_ACC_PRE_NOT_EXIST  3722
#define RET_GET_CUST_ACC_PRE_ERR        3723
#define RET_GET_FUNC_CODE_ERR		3724
#define RET_GET_CUST_FEE_TOTAL_ERR	3725
#define RET_INVALID_BANK_PARA		3790
#define RET_PAY_CARD_COMM_ERR		3800
#define RET_PAY_CARD_PASSWORD_LENGTH_ERR_OR_ENCRY_MACHINE_BUSY	3801
#define RET_PAY_CARD_INVALID_CARDNO_OR_PASSWORD	3802
#define RET_PAY_CARD_BUSY_OR_INVALID_PHONE_NO	3803
#define RET_PAY_CARD_BUSY	3804
#define RET_INVALID_PAY_CARD_NO 	3805
#define RET_GET_CARD_FEE_ERR	3806
#define RET_GET_MAX_CARD_ERR	3807
#define RET_GET_CARD_PHONENO_ERR	3808
#define RET_INSERT_DIFF_PAY_DAY_ERR	3711
#define RET_GET_BASE_CODE_ERR		3419
#define RET_INSERT_WCOMMONSHORTMSG_ERR	3640

#define DELAY_FAVOUR_CODE	"a040"
#define REMONTH_FAVOUR_CODE	"a041"
#define CORPUS_FAVOUR_CODE "a042"

/* begin added by hy*/
/* 高额报告 */
#define RET_PHONE_NO_NOT_EXIT         5000
#define RET_GET_PHONE_NO_ERR          5001
#define RET_READ_BELONG_NAME_ERR      2402
#define RET_GET_RUN_NAME_ERR          5002
#define RET_GET_UNOWNER_MSG_ERR			2751
#define RET_INSERT_HIGH_REPORT_ERR    5003
#define RET_INSERT_CHG_DAY_ERR        3019
#define RET_HIGH_REPORT_ALREADY_EXIT  5004
#define RET_COUNT_DISPOSE_NUM_ERR     5005
#define RET_GET_HIGH_REPORT_ERR       5006
#define RET_ID_NO_NOT_EXIT            5007
#define RET_INSERT_HIGH_REPORT_HIS_ERR   5008
#define RET_UPDATE_HIGH_REPORT_ERR       5009
/* 帐单优惠查询 */
#define RET_GET_CUST_OWE_FAV_ERR      1555
#define RET_CUST_OWE_FAV_NOT_EXIT     1556
#define RET_GET_FAVOURED_FEE_ERR	3640
#define RET_GET_TOTAL_FAV_FEE_ERR	3641
/* 预拆、局拆 */
#define RET_INSERT_CUST_CHG_OWE_DET_ERR        2000
#define RET_INSERT_CUST_CHG_OWE_TOT_ERR	2001
#define RET_UPDATE_CUST_RES_ERR	2002
#define RET_INSERT_LOCAL_BACK_LIST_ERR	2004
#define RET_GET_ID_ICCID_NOT_EXIT     2300
#define RET_GET_LOCK_MONT_NOT_EXIT    2301
#define  RET_GET_CUST_RATE_ERR	2302
#define  RET_INSERT_BASE_FAV_TMP_ERR	2303
#define  RET_UPDATE_BASE_FAV_ERR	2304
#define  RET_INSERT_CUST_CHG_OWE_DETAIL_ERR	2305

/* 核销坏帐 */
#define RET_THIS_PHONE_NO_NOT_EXIT	1205
#define RET_INSERT_CUST_OWE_DEAD_ERR	2350
#define RET_INSERT_CUST_OWE_DEAD_TOTAL_ERR	2351
/* 结息 */
#define RET_UPDATE_CUST_FEE_TOTAL_ERR	2340
#define RET_UPDATE_CUST_PREPAY_ERR	2341
#define RET_INSERT_CUSTINTERESTDAY_ERR	2342
#define RET_DELETE_CUST_INTEREST_ERR	2343
/* 无主删除 */
#define RET_INSERT_UN_OWNER_HIS_ERR	2406
#define RET_GET_UN_OWNER_NUM_ERR	2418
#define RET_GET_UN_OWNER_FEE_ERR	2419
#define RET_INSERT_UN_OWNER_TOTAL_HIS_ERR	2420
#define RET_DELETE_UN_OWNER_ERR	2421
#define RET_DELETE_UN_OWNER_TOTAL_ERR	2422
#define RET_UN_OWNER_NOT_EXIT	2423
/* 费用追加 */
#define RET_GET_UN_OWNER_YEAR_MONTH_ERR	2450
#define RET_GET_CUST_FAV_ERR	2451
#define RET_INSRET_CUST_OWE_ERR	2452
#define RET_INSRET_CUST_OWE_ADJ_ERR	2453
#define RET_INSRET_CUST_OWE_ADJ_TOTAL_ERR	2454
#define RET_INSERT_CUST_CON_ERR	2455
#define RET_INSERT_CUST_CON_TOTAL_ERR	2456
/* end  added by hy*/


/* define init value */
#define SEPERATE	","
#define IDFLAG		0
#define ACCOUNTFLAG	1
#define IS_BILLING	"1"
#define IS_NOT_BILLING	"0"
#define IS_BANK_WORK_NO	1
#define IS_NOT_BANK_WORK_NO	0
#define IS_CARD_PAY_WORK_NO	1
#define IS_NOT_CARD_PAY_WORK_NO	0

#define NEED_REMONTH	"1"
#define NEED_NOT_REMONTH	"0"

#define CASH_PAY	"0"
#define CHECK_PAY	"9"
#define OWE_PAYED	"2"
#define OWE_UNPAYED	"0"
#define OWEDEAD_UNPAY "4"
#define OWEDEAD_PAID "6"
#define OWEBAD_UNPAY "1"
#define OWEBAD_PAID "3"
#define COMMONINVOICE 4
#define BADINVOICE  5
#define DEADINVOICE 6

#endif

struct STRU_OWE_FEE {
	char BillDate[BILLDATELEN+1];
	char OweFee[OWEFEELEN+1];
	char DelayFee[DELAYFEELEN+1];
	char ShouldPay[SHOULDPAYLEN+1];
	char FavourFee[FAVOURFEELEN+1];
	char PayedPrepay[PAYEDPREPAYLEN+1];
	char PayedLater[PAYEDLATERLEN+1];
};

struct STRU_OWE_FEE_NUM {
	int  RetValue;
	int  TotalRecords;
	struct STRU_OWE_FEE StruOweFee[MAXOWERECORDS]; 
};

/*BOSS3.0---20080114-begin*/
typedef struct tPubCancelGrpUserTag
{
  char sGrpId_no[14+1];      /* 集团用户ID_NO*/
  char sLogin_no[6+1];       /* 操作员工代码*/
  char sOldSys_accept[14+1]; /* 开户流水号old*/
  char sOp_code[4+1];        /* 操作代码*/
  char sOp_time[20+1];       /* 系统时间*/
  char sSys_accept[14+1];    /* 冲正流水号new*/
  char sOp_note[60+1];       /* 操作日子*/
  char sReturnMsg[60+1];     /* 返回错误信息*/
}tPubCancelGrpUser;

/* 集团用户产品变更.删除成员计费信息*/

typedef struct tDeleteGrpUserMebBaseFavTag
{
  char sGrpId_no[14+1];      /* 集团用户ID_NO*/
  char sMember_no[14+1];     /* 成员用户PHONE_NO, ALL表示全部成员*/
  char sLogin_no[6+1];       /* 操作员工代码*/
  char sOp_code[4+1];        /* 操作代码*/
  char sOp_time[20+1];       /* 系统时间*/
  char sSys_accept[14+1];    /* 冲正流水号new*/
  char sOp_note[60+1];       /* 操作日志*/
  char sReturnMsg[60+1];     /* 返回错误信息*/
}tDeleteGrpUserMebBaseFav;

/* 集团用户产品变更.增加成员计费信息*/
typedef struct tCreateGrpUserMebBaseFavTag
{
  char sGrpId_no[14+1];      /* 集团用户ID_NO*/
  char sMember_no[14+1];     /* 成员用户PHONE_NO, ALL表示全部成员*/
  char sLogin_no[6+1];       /* 操作员工代码*/
  char sOp_code[4+1];        /* 操作代码*/
  char sOp_time[20+1];       /* 系统时间*/
  char sSys_accept[14+1];    /* 冲正流水号new*/
  char sOp_note[60+1];       /* 操作日志*/
  char sReturnMsg[60+1];     /* 返回错误信息*/
}tCreateGrpUserMebBaseFav;


/*BOSS3.0---20080114-end*/


typedef struct PhoneId
{
        char PhoneNo[PHONENOLEN+1];
        int IdNo;
}STRUCT_PHONEID;

typedef struct MonthlyOwe
{
        int YearMonth;
	int OweFee;
	int ShouldPay;
	int DelayFee;
	int FavourFee;
	int PayedPrepay;
	int PayedLater;
	int MonthRentFee;
	int FunctionFee;
	int LocalNetFee;
	int RoamFee;
	int TollFee;
	int AddedFee;
	int OtherFee;
	int MsgFee;
	int OccFee;
	int LastFee;
}STRUCT_MONTHLYOWE;

typedef struct MonthlyFee
{
        int YearMonth;
	/*应收*/
	float MonthRentFee;
	float FunctionFee;
	float LocalNetFee;
	float LocalTollFee;
	float NativeTollFee;
	float InternationalTollFee;
	float HMTFee;
	float RoamTollFee;
	float LocalRoamFee;
	float NativeRoamFee;
	float InternationalRoamFee;
	float IPFee;
	float MsgFee;
	float OccFee;
	float LastFee;
	float GPRSFee;
	float WADFee;
	/*优惠*/
	float MonthRentFavFee;
	float FunctionFavFee;
	float LocalNetFavFee;
	float LocalTollFavFee;
	float NativeTollFavFee;
	float InternationalTollFavFee;
	float HMTFavFee;
	float RoamTollFavFee;
	float LocalRoamFavFee;
	float NativeRoamFavFee;
	float InternationalRoamFavFee;
	float IPFavFee;
	float MsgFavFee;
	float OccFavFee;
	float LastFavFee;
	float GPRSFavFee;
	float WADFavFee;
	/*其他*/
	float OweFee;
	float ShouldPay;
	float FavourFee;
	float PayedPrepay;
	float TianFuPayedPrepay;
	float DelayFee;
	float TianFuFee;
}STRUCT_MONTHLYFEE;
typedef struct tagUserOwe
{
	double totalOwe;		/*总欠费*/
	double totalPrepay;		/*总预存*/
	double unbillFee;		/*未出帐话费*/
	char   accountNo[23];	/*第一个帐户*/
	double accountOwe;		/*第一个帐户欠费*/
	char   creditname[30+1];  /*运行状态名称-BOSS返回 */
}tUserOwe;
typedef struct tUserBaseInfo
{
	char		id_no[23];			/*用户id*/
	char		cust_id[23];        /*客户id*/
    char        contract_no[22+1];  /*默认帐号*/
	char        belong_code[7+1];   /*用户归属*/
	char		sm_code[2+1];       /*业务类型代码*/
	char		sm_name[20+1];      /*业务类型名称*/
	char		cust_name[60+1];    /*客户名称*/
	char		user_passwd[8+1];   /*用户密码*/
	char		run_code[1+1];      /*状态代码*/
	char		run_name[30+1];     /*状态名称*/
	char		owner_grade[2+1];   /*等级代码*/
	char		grade_name[20+1];   /*等级名称*/
	char		owner_type[2+1];    /*用户类型代码*/
	char		type_name[20+1];    /*用户类型名称*/
	char		cust_address[60+1]; /*客户住址*/
	char		id_type[1+1];       /*证件类型*/
	char		id_name[12+1];      /*证件名称*/
	char		id_iccid[20+1];     /*证件号码*/
	char 		card_type[2+1];     /*客户卡类型*/
	char 		card_name[20+1];    /*客户卡类型名称*/
	char 		vip_no[20+1];       /*VIP卡号*/
	char		grpbig_flag[1+1];   /*集团客户标志*/
	char		grpbig_name[20+1];  /*集团客户名称*/
	char 		bak_field[128];	    /*备用字段*/
	tUserOwe	user_owe;           /*用户欠费*/
	char		group_id[10+1];      /*归属标识*/
	char            contact_person[20+1]; /* 联系人  */
	char            contact_phone[20+1];  /* 联系电话 */
	char            owner_code[2+1];      /* 用户属性 */
	char            credit_code[1+1];     /* 信用类型 */
	int             cycle_begin_day;        /*用户账期*/
}tUserBase;

typedef struct tagFuncEnv
{
	FBFR32 *transIN;
	FBFR32 *transOUT;
	char  *temp_buf;
	const char *totalDate;
	const char *opTime;
	const char *opCode;
	const char *loginNo;
	const char *orgCode;
	const char *loginAccept;
	const char *idNo;
	const char *phoneNo;
	const char *smCode;
	const char *belongCode;
	const char *cmdRight;
	const char *imsiNo;
	const char *simNo;
	const char *hlrCode;
	const char *offonAccept;
} tFuncEnv;

typedef struct tagBizProcReq
{
    char loginNo[6+1];
    char orgCode[9+1];
    char opCode[4+1];
    char totalDate[8+1];
    char idType[2+1];
    char idValue[32+1];
    char oprCode[2+1];
    char bizType[2+1];
    char newPaswd[64+1];
    char oprTime[14+1];
    char spid[1001];
    char spBizCode[1001];
    char infoCode[1001];
    char infoValue[1001];
    char userStatus[2+1];
    char chgTime[14+1];
    char otherStatus[2+1];
    char homeProv[3+1];
    char opNote[60+1];
} tBizProcReq;

/* 担保信息表对应结构 */
struct sCustAssure{
		char	idNo		[22+1];	 /*用户ID	*/
		char	opCode		[4+1];	 /*操作代码	*/
		char	custName	[30+1];	 /*担保人姓名	*/
		char	idType		[1+1];	 /*担保人证件类型*/
		char	idAddress	[60+1];	 /*担保人证件地址*/
		char	idIccid		[20+1];	 /*担保人证件号码*/
		char    contactPhone	[20+1];	 /*担保人联系电话*/
		char    contactAddress	[100+1]; /*担保人联系地址*/
		char    loginNo		[6+1];	 /*操作工号	*/
		char    opTime		[20+1];	 /*操作时间	*/
		char    notes		[120+1]; /*担保人备注	*/
};

/*渠道类型结构*/
typedef struct GroupClass
{
	char	vClass_Kind[1+1];	/*渠道类型*/
	char	vKind_Name[40+1];	/*渠道名称*/
	char    vIs_Account[1+1];   /*是否生成账户Y,N*/
	char    vIs_Town[1+1];		/*是否是营业厅Y,N*/
	char	vIs_Agent[1+1];		/*是否是代理点Y,N*/
	char	vDoc_Type[1+1];		/*渠道类型,对应SCHNCLASSKINDDOC*/
}tGroupClass;			
	

typedef struct CustDoc
{
    long      vCust_Id;                /* 客户id               */
    char      vRegion_Code[2+1];       /* 客户地区代码         */
    char      vDistrict_Code[2+1];     /* 客户市县代码         */
    char      vTown_Code[3+1];         /* 客户网点代码         */
    char      vOrg_Code[9+1];          /* 机构编码             */
    char      vGroup_Id[10+1];         /* 归属节点             */
    char      vOrg_Id[10+1];           /* 工号识别             */
    
    char      vCust_Name[60+1];        /* 客户名称             */
    char      vCust_Passwd[8+1];       /* 客户密码             */
    char      vCust_Status[2+1];       /* 客户状态             */
    char      vCust_Sex[2+1];          /* 客户性别             */
    char      vBirthday[8+1];          /* 客户生日             */
    
    char      vProfession_Id[4+1];     /* 职业类型             */
    char      vEdu_Level[2+1];         /* 学历                 */
    char      vCust_Love[60+1];        /* 客户爱好             */
    char      vCust_Habit[60+1];       /* 客户习惯             */
    
    char      vRun_Time[17+1];         /* 客户状态时间         *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vCreate_Time[17+1];      /* 档案建立时间         *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vClose_Time[17+1];       /* 档案注销时间         *
                                        * YYYYMMDD HH24:MI:SS  */
                                        
    char      vOwner_Grade[2+1];       /* 客户等级代码         */
    char      vOwner_Type[2+1];        /* 客户类型             */
    char      vCust_Addr[60+1];        /* 客户住址             */
    char      vId_Type[1+1];           /* 证件类型             */
    char      vId_Iccid[20+1];         /* 证件号码             */
    char      vId_Addr[60+1];          /* 证件地址             */
    char      vId_ValidDate[17+1];     /* 证件有效期           *
                                        * YYYYMMDD*/
    char      vContact_Person[20+1];   /* 联系人               */
    char      vContact_Phone[20+1];    /* 联系电话             */
    char      vContact_Addr[100+1];    /* 联系地址             */
    char      vContact_Post[6+1];      /* 联系人邮政编码       */
    char      vContact_MailAddr[60+1]; /* 联系人通信地址       */
    char      vContact_Fax[30+1];      /* 联系人传真           */
    char      vContact_Email[30+1];    /* 联系人E-mail         */
                                       
    char      vParent_Id[14+1];        /* 上级客户ID           */
    char      vCreate_Note[60+1];      /* 创建备注             */
    
    long      vLogin_Accept;           /* 操作流水             */
    char      vOp_Time[17+1];          /* 操作时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* 帐务日期             */         
    char      vLogin_No[6+1];          /* 操作工号             */
    char      vOp_Code[4+1];           /* 操作代码             */
    char	  vTrue_Flag[1+1];		   /* 实名制标志		   */
    char	  vInfo_Type[2+1];
    char	  vAgreement_Type[1+1];		
} tCustDoc;

typedef struct FeeDetail
{
   char          vFee_Code[4+1];       /* 费用代码       */
   float         vFee;                 /* 费用值         */
   float         vFee_Old;             /* 优惠前费用值   */
   char          vFavour_Code[4+1];    /* 优惠代码       */
} tFeeDetail;

typedef struct LoginOpr
{
    long        vId_No;                /* 用户ID      */
    int         vTotal_Date;           /* 帐务日期    */
    long        vLogin_Accept;         /* 操作流水    */
    int         vOld_Date;             /* 原帐务日期  */
    long        vOld_Accept;           /* 原操作流水  */
    char        vSm_Code[2+1];         /* 业务代码    */
    char        vBelong_Code[7+1];     /* 归属代码    */
    char        vPhone_No[15+1];       /* 移动号码    */
    char        vOrg_Code[9+1];        /* 机构编码    */
    char        vLogin_No[6+1];        /* 操作工号    */
    char        vOp_Code[4+1];         /* 操作代码    */
    char        vOp_Time[17+1];        /* 系统时间    */
    char        vMachine_Code[3+1];    /* 机器代码    */
    float       vPay_Money;            /* 总金额      *
                                        * 受理费用 +  *
                                        * 冲销预存    */
                                        
    float       vCash_Pay;             /* 现金        */
    float       vCheck_Pay;            /* 支票        */
    float       vSim_Fee;              /* SIM卡费     */
    float       vMachine_Fee;          /* 机器费      */
    float       vInnet_Fee;            /* 入网费      */
    float       vChoice_Fee;           /* 选号费      */
    float       vOther_Fee;            /* 其它费      */
    float       vHand_Fee;             /* 手续费      */
    float       vDeposit;              /* 押金        */
    float       vSim_Prepay;           /* SIM卡预充值 */
    char        vBack_Flag[1+1];       /* 回退标志    */
    char        vEncrypt_Fee[16+1];    /* 加密字段    */
    char        vSystem_Note[60+1];    /* 系统备注    */
    char        vOp_Note[60+1];        /* 操作备注    */
    char        vGroup_Id[10+1];       /* 归属ID      */
    char        vOrg_Id[10+1];         /* 工号标识    */

    char        vPay_Type[4+1];        /* 交费类型    */
    char        vIp_Addr[15+1];        /* IP地址      */

    float       vSim_Fee_Old;          /* SIM卡费(Old)*/
    float       vMachine_Fee_Old;      /* 机器费(Old) */
    float       vInnet_Fee_Old;        /* 入网费(Old) */
    float       vChoice_Fee_Old;       /* 选号费(Old) */
    float       vHand_Fee_Old;         /* 手续费(Old) */
    
	tFeeDetail  vLogFee[MAXBUFRECORDS];/*受理费用明细*/
    tFeeDetail  *logFee[MAXBUFRECORDS];
} tLoginOpr;


typedef struct SzxMsg
{
    long      vCust_Id;                /* 客户id               */
    long      vId_No;                  /* 用户ID               */
    char      vPhone_No[15+1];         /* 神州行号码           */
    char      vSim_No[20+1];           /* SIM卡号              */
    char      vImsi_No[20+1];          /* IMSI号               */
    char      vSm_Code[2+1];           /* 业务代码             */
    char      vOpen_Time[17+1];        /* 开户时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vExpire_Time[17+1];      /* 到期时间             */
    float     vPrepay_Fee;             /* 神州行预存款         */
    float     vPrepay_Rate;            /* 保留字段             */
    float     vOwe_Fee;                /* 欠费                 */
    char      vPassword[6+1];          /* 用户密码             */
    char      vRun_Code[4+1];          /* 用户状态             */
    char      vRun_Time[17+1];         /* 状态时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vBelong_Code[7+1];       /* 归属代码             */
    char      vBill_Code[8+1];         /* 套餐代码             */
    char      vBegin_Time[17+1];       /* 开始时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vBak_Field1[20+1];       /* 备用字段(字符串)     */
    float     vBak_Field2;             /* 备用字段(单精度浮点) */
    char      vBak_Field3[17+1];       /* 备用字段(日期类型)   */
    char      vGroup_Id[10+1];         /* 归属节点             */

    long      vLogin_Accept;           /* 操作流水             */
    char      vOp_Time[17+1];          /* 操作时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* 帐务日期             */
    char      vLogin_No[6+1];          /* 操作工号             */
    char      vOp_Code[4+1];           /* 操作代码             */
    char      vOp_Type[1+1];           /* 操作方式             */

} tSzxMsg;


typedef struct PhoneSim
{
    char      vPhone_No[15+1];         /* 神州行号码           */
    char      vSim_No[20+1];           /* SIM卡号              */
    char      vOpen_Flag[1+1];         /* 开通标志             *
                                        * 0: 未开通            *
                                        * 1" 已开通            */
    long      vLogin_Accept;           /* 操作流水             */

    long      vUpdate_Accept;          /* 更新流水             */
    char      vOp_Time[17+1];          /* 操作时间             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* 帐务日期             */
    char      vLogin_No[6+1];          /* 操作工号             */
    char      vOp_Code[4+1];           /* 操作代码             */
    char      vOp_Type[1+1];           /* 操作方式             */

    char      vOld_Open_Flag[1+1];     /* 原开通标志           */

} tPhoneSim;


typedef struct CustResLog
{
    char        vPhone_No[15+1];       /* 移动号码    */
    long        vLogin_Accept;         /* 操作流水    */
    int         vTotal_Date;           /* 帐务日期    */
    char        vGroup_Id[10+1];       /* 归属节点    */
    char        vOrg_Id[10+1];         /* 工号识别    */
    char        vLogin_No[6+1];        /* 新操作工号  */
    char        vOp_Code[4+1];         /* 操作代码    */
    char        vOp_Time[17+1];        /* 操作时间    */
    char        vOp_Note[60+1];        /* 操作备注    */
    char        vBak_Field[10+1];      /* 备用字段    */
    char        vResource_Code[1+1];   /* 号码状态    */
    char        vOrg_Code[9+1];        /* 机构编码    */
    char        vBelong_Code[7+1];     /* 归属代码    */

} tCustResLog;

typedef struct SimOpr
{
   	long    vLogin_Accept;		/* 操作流水    */
 	char    vTotal_Date[8+1];	/* 帐务日期    */
 	char    vSim_No[20+1];		/* SIM卡号码   */
 	float   vAsn_Price;             /* 销售单价    */ 
   	long    vShould_Number;         /* 应销售数量  */
 	float   vShould_Money;          /* 应销售金额  */
 	long    vAsn_Number;            /* 实销售数量  */
 	float   vAsn_Money;             /* 实销售金额  */
 	char    vSim_Status[1+1];	/*SIM卡状态    */
        long    vId_No;                 /* 用户ID      */
        char    vPhone_No[15+1];        /* 移动号码    */
        char    vSm_Code[2+1];          /* 业务代码    */
        char    vOp_Time[14+1];         /* 操作时间    */
        char    vOp_Code[4+1];          /* 操作代码    */
        char    vLogin_No[6+1];         /* 操作工号    */
        char    vOrg_Group[10+1];       /* 归属节点    */
        char    vYear_Month[6+1];       /* 当前年月    */
        char    vOrg_Code[9+1];         /* 机构编码    */
        char    vBelong_Code[7+1];      /* 归属地      */
}tSimOpr;


typedef struct ChgList
{
        long    vCommand_Id;          /* 标识号     */
        long    vId_No;               /* 用户ID     */
        long    vTotal_Date;          /* 帐务日期   */
        long    vLogin_Accept;        /* 操作流水   */
        char    vBelong_Code[7+1];    /* 归属地     */
        char    vSm_Code[2+1];        /* 用户类型   */
        char    vAttr_Code[8+1];      /* 用户属性   */
        char    vPhone_No[15+1];      /* 服务号码   */
        char    vOld_Time[17+1];      /* 变更时间   *//*add by liuweib 20903004*/
        char    vRun_Code[2+1];       /* 运行状态   */
        char    vOrg_Code[9+1];       /* 机构编码   */
        char    vLogin_No[6+1];       /* 操作工号   */
        char    vOp_Code[4+1];        /* 操作代码   */
        char    vChange_Reason[60+1]; /* 操作备注   */
        char    vGroup_Id[10+1];      /* 节点标识号 */
        char    vOrg_Id[10+1];        /* 营业员标识 */
}tChgList;

typedef struct AccDoc
{
        char    vLogin_Accept[14+1];         /* 操作流水     */
        char    vContract_No[14+1];          /* 帐户ID       */
        char    vCon_Cust_Id[10+1];          /* 客户ID       */
        char    vContract_Passwd[8+1];       /* 帐户密码     */
        char    vBank_Cust[60+1];            /* 帐户名称     */
        float   vOddment;                    /* 帐户零头     */
        char    vBelong_Code[7+1];           /* 归属代码     */
        float   vPrepay_Fee;                 /* 预付费       */
        char    vAccount_Limit[1+1];         /* 帐户信誉度   */
        char    vStatus[1+1];                /* 帐户状态     */
        char    vPost_Flag[1+1];             /* 邮寄标志     */
        float   vDeposit;                    /* 押金         */     
        float   vOwe_Fee;                    /* 帐户欠费     */
        int     vAccount_Mark;               /* 帐户积分     */
        char    vPay_Code[1+1];              /* 付款方式     */
        char    vBank_Code[5+1];             /* 银行代码     */
        char    vPost_Bank_Code[5+1];        /* 局方银行代码 */
        char    vAccount_No[30+1];           /* 帐号         */
        char    vAccount_Type[1+1];          /* 帐户类型     */
        char    vBegin_Time[17+1];           /* 开始日期     */
        char    vEnd_Time[17+1];             /* 结束日期     */
        char    vOp_Code[4+1];               /* 操作代码     */
        char    vLogin_No[6+1];              /* 操作工号     */
        char    vOrg_Code[9+1];              /* 机构编码     */
        char    vOp_Note[60+1];              /* 操作备注     */
        char    vSystem_Note[60+1];          /* 系统备注     */
        char    vIp_Addr[15+1];              /* IP_ADDR      */
        char    vGroup_Id[10+1];             /* 节点标识     */
        char    vUpdate_Type[1+1];           /* 更新类型     */
} tAccDoc;


typedef struct UserDoc
{
        long      vCust_Id;                  /* 客户Id       */
        long      vId_No;                    /* 用户ID       */
        long      vContract_No;              /* 默认帐户     */
        long      vIds;                      /* 帐户数量     */
        char      vPhone_No[15+1];           /* 服务号码     */
        char      vSm_Code[2+1];             /* 业务代码     */
        char      vService_Type[2+1];        /* 服务类型     */
        char      vAttr_Code[8+1];           /* 客户属性代码 */
        char      vUser_Passwd[8+1];         /* 用户密码     */
        char      vOpen_Time[17+1];          /* 入网时间     */
        char      vBelong_Code[7+1];         /* 归属代码     */
        float     vLimit_Owe;                /* 信誉度       */
        char      vCredit_Code[1+1];         /* 信誉等级     */
        int       vCredit_Value;             /* 信誉等级阀值 */
        char      vRun_Code[2+1];            /* 运行状态     */
        char      vRun_Time[17+1];           /* 状态时间     */
        char      vBill_Date[17+1];          /* 计费日期     */
        int       vBill_Type;                /* 出帐周期     */
        char      vEncrypt_Prepay[16+1];     /* 加密字段     */
        int       vCmd_Right;                /* 开关机权限值 */
        char      vLast_Bill_Type[1+1];      /* 上次出帐周期 */
        char      vBak_Field[12+1];          /* 备用字段     */
        char      vStop_Attr[3+1];           /* 停机属性     */
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
        char	  vGroup_Id[10+1];			 /* 组织机构:对应dcustmsg的group_no	 */
        char	  varea_code[10+1];			 /* 区域, 对应dcustmsg的group_id,group_msg*/
} tUserDoc;



typedef struct CustUserRelation
{
        long      vCust_Id;                  /* 客户Id       */
        long      vId_No;                    /* 用户ID       */
        char      vRep_Flag[1+1];            /* 保留字段     */
        char      vCust_Flag[1+1];           /* 保留字段     */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tCustUserRelation;


typedef struct UserAccRelation
{
        long      vId_No;                    /* 用户ID       */
        long      vContract_No;              /* 帐户ID       */
        int       vBill_Order;               /* 出帐冲销顺序 */
        long      vSerial_No;                /* 序 列 号     */
        int       vPay_Order;                /* 交费冲销顺序 */
        char      vBegin_YMD[8+1];           /* 起始年月     */
        char      vBegin_TM[6+1];            /* 起始时刻     */
        char      vEnd_YMD[8+1];             /* 结束年月     */
        char      vEnd_TM[6+1];              /* 结束时刻     */
        float     vLimit_Pay;                /* 不明确       */
        char      vRate_Flag[1+1];           /* 不明确       */
        char      vStop_Flag[1+1];           /* 不明确       */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tUserAccRelation;

typedef struct CustSim
{
        long      vId_No;                    /* 用户ID       */
        char      vPhone_No[15+1];           /* 服务号码     */
        char      vSim_No[20+1];             /* SIM卡号      */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tCustSim;

typedef struct CustPost
{
        long      vId_No;                    /* 用户ID       */
        long      vContract_No;              /* 帐户ID       */
        char      vPost_Flag[1+1];           /* 是否邮寄     *
                                              * 0:否,1:是    */  
        char      vPost_Type[1+1];           /* 邮寄方式     *
                                              * 0:传真       *
                                              * 1:邮寄       *
                                              * 2:电子邮件   */
        char      vPost_Addr[100+1];         /* 邮寄地址     */
        char      vPost_Zip[6+1];            /* 邮政编码     */
        char      vFax_No[30+1];             /* 传真号码     */
        char      vMail_Addr[30+1];          /* Email地址    */
        char      vPost_Name[60+1];          /* 邮寄姓名     */
        char      vPostNo_Flag[1+1];         /* 邮寄类型     *
                                              * 0:按用户邮寄 *
                                              * 1:按帐户邮寄 */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tCustPost;

typedef struct CustInnet
{
        long      vId_No;                    /* 用户ID       */
        long      vCust_Id;                  /* 客户ID       */
        char      vBuy_Name[20+1];           /* 经办人       */
        char      vBelong_Code[7+1];         /* 归属代码     */
        char      vTown_Code[3+1];           /* 网点代码     */
        char      vInnet_Type[2+1];          /* 入网类型     */
        char      vOpen_Time[17+1];          /* 入网时间     */
        char      vMachine_Code[3+1];        /* 机器代码     */
        float     vCash_Pay;                 /* 现金交款     */
        float     vCheck_Pay;                /* 支票交款     */
        float     vSim_Fee;                  /* SIM卡费      */
        float     vMachine_Fee;              /* 机器费       */
        float     vInnet_Fee;                /* 入网费       */
        float     vChoice_Fee;               /* 选号费       */
        float     vOther_Fee;                /* 其它费       */
        float     vHand_Fee;                 /* 手续费       */
        float     vDeposit;                  /* 押金         */
        char      vBack_Flag[1+1];           /* 有效标志     *
                                              * 0:有效,1:回退*/
        char      vEncrypt_Fee[16+1];        /* 保留字段     */
        char      vSystem_Note[60+1];        /* 系统备注     */
        char      vOp_Note[60+1];            /* 操作备注     */
        long      vAssure_No;                /* 担保ID       */
        char      vGroup_Id[10+1];           /* 归属节点     */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tCustInnet;

typedef struct AssuMsg
{
        long      vAssure_No;                /* 担保号(output) */
        char      vAssure_Name[60+1];        /* 担保人姓名     */
        char      vId_Type[1+1];             /* 担保人证件类型 */
        char      vAssure_Id[20+1];          /* 担保人证件号码 */
        char      vAssure_Phone[20+1];       /* 担保人联系电话 */
        char      vAssure_Zip[6+1];          /* 担保人邮政编码 */
        char      vAssure_Addr[60+1];        /* 担保人地址     */
        int       vAssure_Num;               /* 担保数量       */
        
        long      vLogin_Accept;             /* 操作流水     */
        char      vOp_Time[17+1];            /* 操作时间     */
        int       vTotal_Date;               /* 帐务日期     */
        char      vLogin_No[6+1];            /* 操作工号     */
        char      vOp_Code[4+1];             /* 操作代码     */
} tAssuMsg;


typedef struct CredDoc
{
        char      vCred_Id[21+1];            /* 随E行凭证号码 */
        char      vPhone_No[15+1];           /* 移动号码      */
        char      vUse_Type[1+1];            /* 使用方式      *
                                              * 1:随E行开户   */
        long      vId_No;                    /* 用户ID        */
        
        long      vLogin_Accept;             /* 操作流水      */
        char      vOp_Time[17+1];            /* 操作时间      */
        int       vTotal_Date;               /* 帐务日期      */
        char      vLogin_No[6+1];            /* 操作工号      */
        char      vOp_Code[4+1];             /* 操作代码      */
} tCredDoc;


typedef struct ProdSrvFeeList
{
    char    iFeeCount[4+1];
    char    service_code[50][4+1];
    char    price_code[50][4+1];
    char    fee_code[50][2+1];
    char    detail_code[50][2+1];
    char    charge_flag[50][1+1];
    char    price_fee[50][12+1];
    char    action_fee[50][12+1];
    char    vyShouldFee[15+1];   /* 营业应收*/
    char    vyRealFee[15+1];     /* 营业实收 */
    char    vzShouldFee[15+1];   /* 帐务应收 */
    char    vzRealFee[15+1];     /* 帐务实收*/
} tProdSrvFeeList;

/* 集团用户信息对应结构 */
typedef struct tGrpUserMsg
{
    char    org_code[7+1];         /* 区域代码     */
    char    login_name[20+1];      /* 自服务用户名 */
    char    login_passwd[20+1];    /* 自服务密码   */
    char    run_code[1+1];         /* 用户状态     */
    char    old_run[1+1];          /* 前一状态     */
    char    run_time[20+1];        /* 状态变化时间 */
    char    bill_date[20+1];       /* 出帐时间     */
    char    last_bill_type[1+1];   /* 上次出帐类型 */
    char    limit_owe[18+1];       /* 透支额度     */
    char    ids[4+1];              /* 帐户数       */
    char    belong_code[7+1];         /* 区域代码     */
    char    cust_id[14+1];         /* 集团客户ID   */
    char    id_no[14+1];           /* 集团用户ID   */
    char    user_no[20+1];         /* 用户编号     */
    char    account_no[14+1];      /* 用户帐号     */
    char    account_type[1+1];     /* 帐户类型     */
    char    account_passwd[8+1];   /* 用户帐户密码 */
    char    user_name[60+1];       /* 用户名称     */
    char    sm_code[2+1];          /* 产品品牌     */
    char    product_code[8+1];     /* 产品代码     */
    char    product_type[8+1];     /* 产品类型     */
    char    user_passwd[8+1];      /* 业务受理密码 */
    char    prov_code[2+1];        /* 归属省       */
    char    region_code[2+1];      /* 归属地区     */
    char    district_code[2+1];    /* 归属县       */
    char    town_code[3+1];        /* 归属营业厅   */
    char    territory_code[20+1];  /* 属地代码     */
    char    credit_code[2+1];      /* 信用等级     */
    char    credit_value[18+1];    /* 信用度       */
    char    bill_type[4+1];        /* 出帐周期代码 */
    char    op_time[20+1];         /* 开户时间     */
    char    srv_start[10+1];       /* 业务起始日期 */
    char    srv_stop[10+1];        /* 业务结束日期 */
    char    bak_field[16+1];       /* 预留字段     */
    char    group_id[10+1];        /* GROUP_ID     */
    char    org_id[10+1];          /* 机构的GROUP_ID*/
    char    channel_id[10+1];      /* 渠道接点      */
    char    return_message[60+1];  /* 调用返回信息 */
    char    pay_code[1+1];         /* 集团帐户付款方式 */
    char	service_code[4+1];	   /*集团优惠代码*/
    tProdSrvFeeList feeList;
}tGrpUserMsg;


typedef struct ShortMsg
{
    long    vLogin_Accept;         /* 操作流水     */
    long    vMsg_Level;            /* 信息等级     */
    char    vPhone_No[15+1];       /* 移动号码     */
    char    vSend_Msg[255+1];      /* 发送信息     */
    int     vOrder_Code;           /* 发送顺序     */
    int     vBack_Code;            /* 返回代码     */
    char    vOp_Code[4+1];         /* 操作代码     */
    char    vLogin_No[6+1];        /* 操作工号     */
} tShortMsg;

typedef struct CustFunc
{
    long    vId_No;                 /* 用户ID      */
    char    vPhone_No[15+1];        /* 移动号码    */
    char    vSm_Code[2+1];          /* 业务代码    */
    char    vOrg_Code[9+1];         /* 机构编码    */
    char    vBelong_Code[7+1];      /* 归属代码    */
    char    vFunc_Type[1+1];        /* 特服类型    */
    char    vFunc_Code[2+1];        /* 特服代码    */
    char    vBegin_Time[17+1];      /* 开通时间    *
                                     * YYYYMMDD    */
    char    vAdd_No[15+1];          /* 附加号码    */
    char    vOther_Attr[40+1];      /* 附加属性    */
    char    vExpire_Time[17+1];     /* 预约时间    */
    
    char    vOp_Note[60+1];         /* 操作备注    */
    char    vOrg_Id[10+1];          /* 工号识别    */
    
    long    vLogin_Accept;          /* 操作流水    */
    char    vOp_Time[17+1];         /* 操作时间    */
    int     vTotal_Date;            /* 帐务日期    */
    char    vLogin_No[6+1];         /* 操作工号    */
    char    vOp_Code[4+1];          /* 操作代码    */
} tCustFunc;

typedef struct LockPhone
{
    char        vOp_Type[1+1];         /* 操作类型    *
                                        * 0:普通开户  *
                                        * 1:二次入网  *
                                        * 2:天府行激活*/
    char        vPhone_No[15+1];       /* 移动号码    */
    char        vGroup_Id[10+1];       /* 归属节点    */
    char        vOp_Code[4+1];         /* 操作代码    */
    char        vSm_Code[2+1];         /* 业务代码    */
    char        vLock_Flag[1+1];       /* 加解锁标志  *
                                        * 0: 加锁     *
                                        * 1: 解锁     */
    float       vChoice_Fee;           /* 选号费      */
    float       vChoice_Prepay;        /* 选号预存    */
} tLockPhone;

typedef struct LockSim
{
    char        vPhone_No[15+1];       /* 移动号码    */
    char        vGroup_Id[10+1];       /* 归属节点    */
    char        vOp_Code[4+1];         /* 操作代码    */
    char        vSm_Code[2+1];         /* 业务代码    */
    char        vSim_No[20+1];         /* SIM卡号     */
    char        vLock_Flag[1+1];       /* 加解锁标志  *
                                        * 0: 加锁     *
                                        * 1: 解锁     */
    float       vSim_Fee;              /* SIM卡费     */
    float       vSim_Value;            /* SIM卡面值   */
} tLockSim;

/* lixg define: user product infomation begin */
#define	USER_MODE_NUM	10	/* 用户最大定购套餐数量 */
typedef struct {
	char	modeDetIndex[USER_MODE_NUM*10][2+1];
	char	mode_code[USER_MODE_NUM*10][8+1];
	char	mode_name[USER_MODE_NUM*10][60+1];
	char	detail_code[USER_MODE_NUM*10][4+1];
	char	detail_name[USER_MODE_NUM*10][60+1];
	char	flag_code[USER_MODE_NUM*10][10+1];
	char	flag_name[USER_MODE_NUM*10][256+1];
	char	detail_type[USER_MODE_NUM*10][1+1];
	char	detail_type_name[USER_MODE_NUM*10][60+1];
	char	begin_time[USER_MODE_NUM*10][17+1];
	char	end_time[USER_MODE_NUM*10][17+1];
	char	login_accept[USER_MODE_NUM*10][14+1];
	char	login_no[USER_MODE_NUM*10][6+1];
} tModeDetail;

typedef struct {
	char	modeNum[2+1];
	char	modeIdx[USER_MODE_NUM][2+1];
	char	sm_code[USER_MODE_NUM][2+1];
	char	mode_code[USER_MODE_NUM][8+1];
	char	mode_name[USER_MODE_NUM][60+1];
	char	mode_note[USER_MODE_NUM][120+1];
	char	begin_time[USER_MODE_NUM][17+1];
	char	end_time[USER_MODE_NUM][17+1];
	char	login_accept[USER_MODE_NUM][14+1];
	char	login_no[USER_MODE_NUM][6+1];
	char	op_code[USER_MODE_NUM][4+1];
	char	flag_code[USER_MODE_NUM][10+1];
	char	flag_name[USER_MODE_NUM][256+1];
	tModeDetail	tDetail[USER_MODE_NUM];
} tUserMode;

/**
* 缴费回馈数据结构定义
**/
#define  PROD_SEND_NOW	'0' 	/* 立即赠送 */
#define  PROD_SEND_NEXT	'1' 	/* 下月赠送 */

#define  PROD_SENDFEE_CODE	'C' 	/* 缴费回馈 */
#define  PROD_MACHFEE_CODE	'E'	/* 买手机送话费 */

#define  GRANT_DETTYPE_SENDXJ	'0'	/* 回馈现金 */
#define  GRANT_DETTYPE_SENDSW	'1'	/* 回馈实物 */
#define  GRANT_DETTYPE_SENDJF	'2'	/* 回馈积分 */
#define  GRANT_DETTYPE_REQYC	'P'	/* 预存款处理 */

typedef struct{
	char	applyType[1+1]; /* C: 缴费回馈, E:买手机赠话费 */
	char	phoneNo[15+1];
	char	loginNo[6+1];
	char	opCode[4+1];
	char	opNote[60+1];
	char    opTime[20+1];   /* YYYYMMDD HH24:MI:SS */
	char	totalDate[8+1]; /* YYYYMMDD */
	char	loginAccept[14+1];
	char	modeCode[8+1];
	char	detModeCode[4+1];
	char	machineId[20+1];
	char	oldLoginAccept[14+1];
} tGrantData;

typedef struct{
    char        mode_code[9];
    char        mode_status[2];
    char        begin_time[18];
    char        end_time[18];
    char        login_accept[23];
}BillRec;

typedef struct{
    char        phoneNo[15+1];
    char        loginNo[6+1];
    char        opCode[4+1];
    char        modeCode[8+1];
}BillCtrl;

/* lixg define: user product infomation end */



/**
 *     集团建成员控制变量集。
 */
#define CGU_NEW_USER    1                       /*新建一个用户*/
#define CGU_RESUME_USER 2                       /*恢复预约删除的成员用户*/
typedef struct tagCreateGrpUser
{
        /**
         *     是新建一个成员用户，还是恢复预约删除的成员用户,
         * 值定义：
         *     CGU_NEW_USER     新建一个用户
         *     CGU_RESUME_USER  恢复预约删除的成员用户
         */
        int             createFlag;

        char    loginAccept[22+1];              /*流水*/
        char    opCode[4+1];                    /*功能代码*/
        char    loginNo[6+1];                   /*操作工号*/
        char    orgCode[9+1];                   /*操作工号归属*/
        char    phoneNo[15+1];                  /*服务号码*/
        char    smCode[2+1];                    /*业务类型*/
        char    serviceType[2+1];               /*服务类型 dCustMsg::service_type*/
        char    belongCode[7+1];                /*归属代码*/
        char    opTime[17+1];                   /*操作时间      'YYYYMMDD HH24:MI:SS'*/
        char    custId[22+1];                   /*客户ID*/
        char    productCode[8+1];               /*集团产品代码*/
        char    grpIdNo[22+1];                  /*集团用户ID*/
        char    contractNo[22+1];               /*付费帐号      如果为NULL，表示没有付费帐户，注意此帐号将付成员用户的所有费用*/
        char    groupId[10+1];                  /*号码的归属*/
        char    orgId[10+1];                    /*工号的ID*/
        char    effectTime[17+1];               /*成号关系的生效时间：dGrpUserMebMsg:begin_time*/
} TCreateGrpUser;


typedef struct tagDeleteGrpUser
{
        char    loginAccept[22+1];              /*流水*/
        char    opCode[4+1];                    /*功能代码*/
        char    loginNo[6+1];                   /*操作工号*/
        char    orgCode[9+1];                   /*操作工号归属*/
        char    idNo[22+1];                             /*服务号码的ID*/
        char    opTime[17+1];                   /*操作时间      'YYYYMMDD HH24:MI:SS'*/
        char    grpIdNo[22+1];                  /*集团用户ID*/
        char    productCode[8+1];               /*集团产品代码*/
        char    contractNo[22+1];               /*付费帐号      如果为NULL，表示没有付费帐户，注意此帐号将付成员用户的所有费用*/
        char    groupId[10+1];                  /*号码的归属*/
        char    orgId[10+1];                    /*工号的ID*/
        char    effectTime[17+1];               /*成号关系的生效时间：dGrpUserMebMsg:begin_time*/
} TDeleteGrpUser;

/* 黑名单信息结构体 */
struct DCUSTFAZE_TYPE{
	char phone_no[16];    /* 电话号码 */
	char faze_type[2];    /* 骚扰类型：0－无骚扰； 2－短信骚扰；1－电话骚扰 */
	char disposeway[3];   /* 传送方式：00－个人；01－梦网SP；02－集团客户号码 */
	char belong_type[2];  /* 归属类型：0－本省用户；1－外省用户；2－外网用户 */
	char grade_code[2];   /* 号码级别：0－疑似黑名单；1－黑名单；2－白名单 */
	char begin_time[18];  /* 加入时间 */
	char content[280];
	char keyword[170];
	char reason[2];
	char phonenoflag[2];
	char dstarea[3];
	char smstype[2];
};
typedef struct DCUSTFAZE_TYPE  DCUSTFAZE_TYPE;

/* 订购资费相关信息结构体 */
typedef struct tagPubBillInfo {
	char IdNo[14+1];
	char PhoneNo[15+1];
	char BelongCode[7+1];
	char SmCode[2+1];
	char ModeCode[8+1];
	char OpCode[4+1];
	char OpTime[17+1];
	char EffectTime[17+1];
	int  TotalDate;
	char LoginNo[6+1];
	char LoginAccept[14+1];
} TPubBillInfo;
