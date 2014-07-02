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
/* �߶�� */
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
/* �ʵ��Żݲ�ѯ */
#define RET_GET_CUST_OWE_FAV_ERR      1555
#define RET_CUST_OWE_FAV_NOT_EXIT     1556
#define RET_GET_FAVOURED_FEE_ERR	3640
#define RET_GET_TOTAL_FAV_FEE_ERR	3641
/* Ԥ�𡢾ֲ� */
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

/* �������� */
#define RET_THIS_PHONE_NO_NOT_EXIT	1205
#define RET_INSERT_CUST_OWE_DEAD_ERR	2350
#define RET_INSERT_CUST_OWE_DEAD_TOTAL_ERR	2351
/* ��Ϣ */
#define RET_UPDATE_CUST_FEE_TOTAL_ERR	2340
#define RET_UPDATE_CUST_PREPAY_ERR	2341
#define RET_INSERT_CUSTINTERESTDAY_ERR	2342
#define RET_DELETE_CUST_INTEREST_ERR	2343
/* ����ɾ�� */
#define RET_INSERT_UN_OWNER_HIS_ERR	2406
#define RET_GET_UN_OWNER_NUM_ERR	2418
#define RET_GET_UN_OWNER_FEE_ERR	2419
#define RET_INSERT_UN_OWNER_TOTAL_HIS_ERR	2420
#define RET_DELETE_UN_OWNER_ERR	2421
#define RET_DELETE_UN_OWNER_TOTAL_ERR	2422
#define RET_UN_OWNER_NOT_EXIT	2423
/* ����׷�� */
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
  char sGrpId_no[14+1];      /* �����û�ID_NO*/
  char sLogin_no[6+1];       /* ����Ա������*/
  char sOldSys_accept[14+1]; /* ������ˮ��old*/
  char sOp_code[4+1];        /* ��������*/
  char sOp_time[20+1];       /* ϵͳʱ��*/
  char sSys_accept[14+1];    /* ������ˮ��new*/
  char sOp_note[60+1];       /* ��������*/
  char sReturnMsg[60+1];     /* ���ش�����Ϣ*/
}tPubCancelGrpUser;

/* �����û���Ʒ���.ɾ����Ա�Ʒ���Ϣ*/

typedef struct tDeleteGrpUserMebBaseFavTag
{
  char sGrpId_no[14+1];      /* �����û�ID_NO*/
  char sMember_no[14+1];     /* ��Ա�û�PHONE_NO, ALL��ʾȫ����Ա*/
  char sLogin_no[6+1];       /* ����Ա������*/
  char sOp_code[4+1];        /* ��������*/
  char sOp_time[20+1];       /* ϵͳʱ��*/
  char sSys_accept[14+1];    /* ������ˮ��new*/
  char sOp_note[60+1];       /* ������־*/
  char sReturnMsg[60+1];     /* ���ش�����Ϣ*/
}tDeleteGrpUserMebBaseFav;

/* �����û���Ʒ���.���ӳ�Ա�Ʒ���Ϣ*/
typedef struct tCreateGrpUserMebBaseFavTag
{
  char sGrpId_no[14+1];      /* �����û�ID_NO*/
  char sMember_no[14+1];     /* ��Ա�û�PHONE_NO, ALL��ʾȫ����Ա*/
  char sLogin_no[6+1];       /* ����Ա������*/
  char sOp_code[4+1];        /* ��������*/
  char sOp_time[20+1];       /* ϵͳʱ��*/
  char sSys_accept[14+1];    /* ������ˮ��new*/
  char sOp_note[60+1];       /* ������־*/
  char sReturnMsg[60+1];     /* ���ش�����Ϣ*/
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
	/*Ӧ��*/
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
	/*�Ż�*/
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
	/*����*/
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
	double totalOwe;		/*��Ƿ��*/
	double totalPrepay;		/*��Ԥ��*/
	double unbillFee;		/*δ���ʻ���*/
	char   accountNo[23];	/*��һ���ʻ�*/
	double accountOwe;		/*��һ���ʻ�Ƿ��*/
	char   creditname[30+1];  /*����״̬����-BOSS���� */
}tUserOwe;
typedef struct tUserBaseInfo
{
	char		id_no[23];			/*�û�id*/
	char		cust_id[23];        /*�ͻ�id*/
    char        contract_no[22+1];  /*Ĭ���ʺ�*/
	char        belong_code[7+1];   /*�û�����*/
	char		sm_code[2+1];       /*ҵ�����ʹ���*/
	char		sm_name[20+1];      /*ҵ����������*/
	char		cust_name[60+1];    /*�ͻ�����*/
	char		user_passwd[8+1];   /*�û�����*/
	char		run_code[1+1];      /*״̬����*/
	char		run_name[30+1];     /*״̬����*/
	char		owner_grade[2+1];   /*�ȼ�����*/
	char		grade_name[20+1];   /*�ȼ�����*/
	char		owner_type[2+1];    /*�û����ʹ���*/
	char		type_name[20+1];    /*�û���������*/
	char		cust_address[60+1]; /*�ͻ�סַ*/
	char		id_type[1+1];       /*֤������*/
	char		id_name[12+1];      /*֤������*/
	char		id_iccid[20+1];     /*֤������*/
	char 		card_type[2+1];     /*�ͻ�������*/
	char 		card_name[20+1];    /*�ͻ�����������*/
	char 		vip_no[20+1];       /*VIP����*/
	char		grpbig_flag[1+1];   /*���ſͻ���־*/
	char		grpbig_name[20+1];  /*���ſͻ�����*/
	char 		bak_field[128];	    /*�����ֶ�*/
	tUserOwe	user_owe;           /*�û�Ƿ��*/
	char		group_id[10+1];      /*������ʶ*/
	char            contact_person[20+1]; /* ��ϵ��  */
	char            contact_phone[20+1];  /* ��ϵ�绰 */
	char            owner_code[2+1];      /* �û����� */
	char            credit_code[1+1];     /* �������� */
	int             cycle_begin_day;        /*�û�����*/
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

/* ������Ϣ���Ӧ�ṹ */
struct sCustAssure{
		char	idNo		[22+1];	 /*�û�ID	*/
		char	opCode		[4+1];	 /*��������	*/
		char	custName	[30+1];	 /*����������	*/
		char	idType		[1+1];	 /*������֤������*/
		char	idAddress	[60+1];	 /*������֤����ַ*/
		char	idIccid		[20+1];	 /*������֤������*/
		char    contactPhone	[20+1];	 /*��������ϵ�绰*/
		char    contactAddress	[100+1]; /*��������ϵ��ַ*/
		char    loginNo		[6+1];	 /*��������	*/
		char    opTime		[20+1];	 /*����ʱ��	*/
		char    notes		[120+1]; /*�����˱�ע	*/
};

/*�������ͽṹ*/
typedef struct GroupClass
{
	char	vClass_Kind[1+1];	/*��������*/
	char	vKind_Name[40+1];	/*��������*/
	char    vIs_Account[1+1];   /*�Ƿ������˻�Y,N*/
	char    vIs_Town[1+1];		/*�Ƿ���Ӫҵ��Y,N*/
	char	vIs_Agent[1+1];		/*�Ƿ��Ǵ����Y,N*/
	char	vDoc_Type[1+1];		/*��������,��ӦSCHNCLASSKINDDOC*/
}tGroupClass;			
	

typedef struct CustDoc
{
    long      vCust_Id;                /* �ͻ�id               */
    char      vRegion_Code[2+1];       /* �ͻ���������         */
    char      vDistrict_Code[2+1];     /* �ͻ����ش���         */
    char      vTown_Code[3+1];         /* �ͻ��������         */
    char      vOrg_Code[9+1];          /* ��������             */
    char      vGroup_Id[10+1];         /* �����ڵ�             */
    char      vOrg_Id[10+1];           /* ����ʶ��             */
    
    char      vCust_Name[60+1];        /* �ͻ�����             */
    char      vCust_Passwd[8+1];       /* �ͻ�����             */
    char      vCust_Status[2+1];       /* �ͻ�״̬             */
    char      vCust_Sex[2+1];          /* �ͻ��Ա�             */
    char      vBirthday[8+1];          /* �ͻ�����             */
    
    char      vProfession_Id[4+1];     /* ְҵ����             */
    char      vEdu_Level[2+1];         /* ѧ��                 */
    char      vCust_Love[60+1];        /* �ͻ�����             */
    char      vCust_Habit[60+1];       /* �ͻ�ϰ��             */
    
    char      vRun_Time[17+1];         /* �ͻ�״̬ʱ��         *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vCreate_Time[17+1];      /* ��������ʱ��         *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vClose_Time[17+1];       /* ����ע��ʱ��         *
                                        * YYYYMMDD HH24:MI:SS  */
                                        
    char      vOwner_Grade[2+1];       /* �ͻ��ȼ�����         */
    char      vOwner_Type[2+1];        /* �ͻ�����             */
    char      vCust_Addr[60+1];        /* �ͻ�סַ             */
    char      vId_Type[1+1];           /* ֤������             */
    char      vId_Iccid[20+1];         /* ֤������             */
    char      vId_Addr[60+1];          /* ֤����ַ             */
    char      vId_ValidDate[17+1];     /* ֤����Ч��           *
                                        * YYYYMMDD*/
    char      vContact_Person[20+1];   /* ��ϵ��               */
    char      vContact_Phone[20+1];    /* ��ϵ�绰             */
    char      vContact_Addr[100+1];    /* ��ϵ��ַ             */
    char      vContact_Post[6+1];      /* ��ϵ����������       */
    char      vContact_MailAddr[60+1]; /* ��ϵ��ͨ�ŵ�ַ       */
    char      vContact_Fax[30+1];      /* ��ϵ�˴���           */
    char      vContact_Email[30+1];    /* ��ϵ��E-mail         */
                                       
    char      vParent_Id[14+1];        /* �ϼ��ͻ�ID           */
    char      vCreate_Note[60+1];      /* ������ע             */
    
    long      vLogin_Accept;           /* ������ˮ             */
    char      vOp_Time[17+1];          /* ����ʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* ��������             */         
    char      vLogin_No[6+1];          /* ��������             */
    char      vOp_Code[4+1];           /* ��������             */
    char	  vTrue_Flag[1+1];		   /* ʵ���Ʊ�־		   */
    char	  vInfo_Type[2+1];
    char	  vAgreement_Type[1+1];		
} tCustDoc;

typedef struct FeeDetail
{
   char          vFee_Code[4+1];       /* ���ô���       */
   float         vFee;                 /* ����ֵ         */
   float         vFee_Old;             /* �Ż�ǰ����ֵ   */
   char          vFavour_Code[4+1];    /* �Żݴ���       */
} tFeeDetail;

typedef struct LoginOpr
{
    long        vId_No;                /* �û�ID      */
    int         vTotal_Date;           /* ��������    */
    long        vLogin_Accept;         /* ������ˮ    */
    int         vOld_Date;             /* ԭ��������  */
    long        vOld_Accept;           /* ԭ������ˮ  */
    char        vSm_Code[2+1];         /* ҵ�����    */
    char        vBelong_Code[7+1];     /* ��������    */
    char        vPhone_No[15+1];       /* �ƶ�����    */
    char        vOrg_Code[9+1];        /* ��������    */
    char        vLogin_No[6+1];        /* ��������    */
    char        vOp_Code[4+1];         /* ��������    */
    char        vOp_Time[17+1];        /* ϵͳʱ��    */
    char        vMachine_Code[3+1];    /* ��������    */
    float       vPay_Money;            /* �ܽ��      *
                                        * ������� +  *
                                        * ����Ԥ��    */
                                        
    float       vCash_Pay;             /* �ֽ�        */
    float       vCheck_Pay;            /* ֧Ʊ        */
    float       vSim_Fee;              /* SIM����     */
    float       vMachine_Fee;          /* ������      */
    float       vInnet_Fee;            /* ������      */
    float       vChoice_Fee;           /* ѡ�ŷ�      */
    float       vOther_Fee;            /* ������      */
    float       vHand_Fee;             /* ������      */
    float       vDeposit;              /* Ѻ��        */
    float       vSim_Prepay;           /* SIM��Ԥ��ֵ */
    char        vBack_Flag[1+1];       /* ���˱�־    */
    char        vEncrypt_Fee[16+1];    /* �����ֶ�    */
    char        vSystem_Note[60+1];    /* ϵͳ��ע    */
    char        vOp_Note[60+1];        /* ������ע    */
    char        vGroup_Id[10+1];       /* ����ID      */
    char        vOrg_Id[10+1];         /* ���ű�ʶ    */

    char        vPay_Type[4+1];        /* ��������    */
    char        vIp_Addr[15+1];        /* IP��ַ      */

    float       vSim_Fee_Old;          /* SIM����(Old)*/
    float       vMachine_Fee_Old;      /* ������(Old) */
    float       vInnet_Fee_Old;        /* ������(Old) */
    float       vChoice_Fee_Old;       /* ѡ�ŷ�(Old) */
    float       vHand_Fee_Old;         /* ������(Old) */
    
	tFeeDetail  vLogFee[MAXBUFRECORDS];/*���������ϸ*/
    tFeeDetail  *logFee[MAXBUFRECORDS];
} tLoginOpr;


typedef struct SzxMsg
{
    long      vCust_Id;                /* �ͻ�id               */
    long      vId_No;                  /* �û�ID               */
    char      vPhone_No[15+1];         /* �����к���           */
    char      vSim_No[20+1];           /* SIM����              */
    char      vImsi_No[20+1];          /* IMSI��               */
    char      vSm_Code[2+1];           /* ҵ�����             */
    char      vOpen_Time[17+1];        /* ����ʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vExpire_Time[17+1];      /* ����ʱ��             */
    float     vPrepay_Fee;             /* ������Ԥ���         */
    float     vPrepay_Rate;            /* �����ֶ�             */
    float     vOwe_Fee;                /* Ƿ��                 */
    char      vPassword[6+1];          /* �û�����             */
    char      vRun_Code[4+1];          /* �û�״̬             */
    char      vRun_Time[17+1];         /* ״̬ʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vBelong_Code[7+1];       /* ��������             */
    char      vBill_Code[8+1];         /* �ײʹ���             */
    char      vBegin_Time[17+1];       /* ��ʼʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    char      vBak_Field1[20+1];       /* �����ֶ�(�ַ���)     */
    float     vBak_Field2;             /* �����ֶ�(�����ȸ���) */
    char      vBak_Field3[17+1];       /* �����ֶ�(��������)   */
    char      vGroup_Id[10+1];         /* �����ڵ�             */

    long      vLogin_Accept;           /* ������ˮ             */
    char      vOp_Time[17+1];          /* ����ʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* ��������             */
    char      vLogin_No[6+1];          /* ��������             */
    char      vOp_Code[4+1];           /* ��������             */
    char      vOp_Type[1+1];           /* ������ʽ             */

} tSzxMsg;


typedef struct PhoneSim
{
    char      vPhone_No[15+1];         /* �����к���           */
    char      vSim_No[20+1];           /* SIM����              */
    char      vOpen_Flag[1+1];         /* ��ͨ��־             *
                                        * 0: δ��ͨ            *
                                        * 1" �ѿ�ͨ            */
    long      vLogin_Accept;           /* ������ˮ             */

    long      vUpdate_Accept;          /* ������ˮ             */
    char      vOp_Time[17+1];          /* ����ʱ��             *
                                        * YYYYMMDD HH24:MI:SS  */
    int       vTotal_Date;             /* ��������             */
    char      vLogin_No[6+1];          /* ��������             */
    char      vOp_Code[4+1];           /* ��������             */
    char      vOp_Type[1+1];           /* ������ʽ             */

    char      vOld_Open_Flag[1+1];     /* ԭ��ͨ��־           */

} tPhoneSim;


typedef struct CustResLog
{
    char        vPhone_No[15+1];       /* �ƶ�����    */
    long        vLogin_Accept;         /* ������ˮ    */
    int         vTotal_Date;           /* ��������    */
    char        vGroup_Id[10+1];       /* �����ڵ�    */
    char        vOrg_Id[10+1];         /* ����ʶ��    */
    char        vLogin_No[6+1];        /* �²�������  */
    char        vOp_Code[4+1];         /* ��������    */
    char        vOp_Time[17+1];        /* ����ʱ��    */
    char        vOp_Note[60+1];        /* ������ע    */
    char        vBak_Field[10+1];      /* �����ֶ�    */
    char        vResource_Code[1+1];   /* ����״̬    */
    char        vOrg_Code[9+1];        /* ��������    */
    char        vBelong_Code[7+1];     /* ��������    */

} tCustResLog;

typedef struct SimOpr
{
   	long    vLogin_Accept;		/* ������ˮ    */
 	char    vTotal_Date[8+1];	/* ��������    */
 	char    vSim_No[20+1];		/* SIM������   */
 	float   vAsn_Price;             /* ���۵���    */ 
   	long    vShould_Number;         /* Ӧ��������  */
 	float   vShould_Money;          /* Ӧ���۽��  */
 	long    vAsn_Number;            /* ʵ��������  */
 	float   vAsn_Money;             /* ʵ���۽��  */
 	char    vSim_Status[1+1];	/*SIM��״̬    */
        long    vId_No;                 /* �û�ID      */
        char    vPhone_No[15+1];        /* �ƶ�����    */
        char    vSm_Code[2+1];          /* ҵ�����    */
        char    vOp_Time[14+1];         /* ����ʱ��    */
        char    vOp_Code[4+1];          /* ��������    */
        char    vLogin_No[6+1];         /* ��������    */
        char    vOrg_Group[10+1];       /* �����ڵ�    */
        char    vYear_Month[6+1];       /* ��ǰ����    */
        char    vOrg_Code[9+1];         /* ��������    */
        char    vBelong_Code[7+1];      /* ������      */
}tSimOpr;


typedef struct ChgList
{
        long    vCommand_Id;          /* ��ʶ��     */
        long    vId_No;               /* �û�ID     */
        long    vTotal_Date;          /* ��������   */
        long    vLogin_Accept;        /* ������ˮ   */
        char    vBelong_Code[7+1];    /* ������     */
        char    vSm_Code[2+1];        /* �û�����   */
        char    vAttr_Code[8+1];      /* �û�����   */
        char    vPhone_No[15+1];      /* �������   */
        char    vOld_Time[17+1];      /* ���ʱ��   *//*add by liuweib 20903004*/
        char    vRun_Code[2+1];       /* ����״̬   */
        char    vOrg_Code[9+1];       /* ��������   */
        char    vLogin_No[6+1];       /* ��������   */
        char    vOp_Code[4+1];        /* ��������   */
        char    vChange_Reason[60+1]; /* ������ע   */
        char    vGroup_Id[10+1];      /* �ڵ��ʶ�� */
        char    vOrg_Id[10+1];        /* ӪҵԱ��ʶ */
}tChgList;

typedef struct AccDoc
{
        char    vLogin_Accept[14+1];         /* ������ˮ     */
        char    vContract_No[14+1];          /* �ʻ�ID       */
        char    vCon_Cust_Id[10+1];          /* �ͻ�ID       */
        char    vContract_Passwd[8+1];       /* �ʻ�����     */
        char    vBank_Cust[60+1];            /* �ʻ�����     */
        float   vOddment;                    /* �ʻ���ͷ     */
        char    vBelong_Code[7+1];           /* ��������     */
        float   vPrepay_Fee;                 /* Ԥ����       */
        char    vAccount_Limit[1+1];         /* �ʻ�������   */
        char    vStatus[1+1];                /* �ʻ�״̬     */
        char    vPost_Flag[1+1];             /* �ʼı�־     */
        float   vDeposit;                    /* Ѻ��         */     
        float   vOwe_Fee;                    /* �ʻ�Ƿ��     */
        int     vAccount_Mark;               /* �ʻ�����     */
        char    vPay_Code[1+1];              /* ���ʽ     */
        char    vBank_Code[5+1];             /* ���д���     */
        char    vPost_Bank_Code[5+1];        /* �ַ����д��� */
        char    vAccount_No[30+1];           /* �ʺ�         */
        char    vAccount_Type[1+1];          /* �ʻ�����     */
        char    vBegin_Time[17+1];           /* ��ʼ����     */
        char    vEnd_Time[17+1];             /* ��������     */
        char    vOp_Code[4+1];               /* ��������     */
        char    vLogin_No[6+1];              /* ��������     */
        char    vOrg_Code[9+1];              /* ��������     */
        char    vOp_Note[60+1];              /* ������ע     */
        char    vSystem_Note[60+1];          /* ϵͳ��ע     */
        char    vIp_Addr[15+1];              /* IP_ADDR      */
        char    vGroup_Id[10+1];             /* �ڵ��ʶ     */
        char    vUpdate_Type[1+1];           /* ��������     */
} tAccDoc;


typedef struct UserDoc
{
        long      vCust_Id;                  /* �ͻ�Id       */
        long      vId_No;                    /* �û�ID       */
        long      vContract_No;              /* Ĭ���ʻ�     */
        long      vIds;                      /* �ʻ�����     */
        char      vPhone_No[15+1];           /* �������     */
        char      vSm_Code[2+1];             /* ҵ�����     */
        char      vService_Type[2+1];        /* ��������     */
        char      vAttr_Code[8+1];           /* �ͻ����Դ��� */
        char      vUser_Passwd[8+1];         /* �û�����     */
        char      vOpen_Time[17+1];          /* ����ʱ��     */
        char      vBelong_Code[7+1];         /* ��������     */
        float     vLimit_Owe;                /* ������       */
        char      vCredit_Code[1+1];         /* �����ȼ�     */
        int       vCredit_Value;             /* �����ȼ���ֵ */
        char      vRun_Code[2+1];            /* ����״̬     */
        char      vRun_Time[17+1];           /* ״̬ʱ��     */
        char      vBill_Date[17+1];          /* �Ʒ�����     */
        int       vBill_Type;                /* ��������     */
        char      vEncrypt_Prepay[16+1];     /* �����ֶ�     */
        int       vCmd_Right;                /* ���ػ�Ȩ��ֵ */
        char      vLast_Bill_Type[1+1];      /* �ϴγ������� */
        char      vBak_Field[12+1];          /* �����ֶ�     */
        char      vStop_Attr[3+1];           /* ͣ������     */
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
        char	  vGroup_Id[10+1];			 /* ��֯����:��Ӧdcustmsg��group_no	 */
        char	  varea_code[10+1];			 /* ����, ��Ӧdcustmsg��group_id,group_msg*/
} tUserDoc;



typedef struct CustUserRelation
{
        long      vCust_Id;                  /* �ͻ�Id       */
        long      vId_No;                    /* �û�ID       */
        char      vRep_Flag[1+1];            /* �����ֶ�     */
        char      vCust_Flag[1+1];           /* �����ֶ�     */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tCustUserRelation;


typedef struct UserAccRelation
{
        long      vId_No;                    /* �û�ID       */
        long      vContract_No;              /* �ʻ�ID       */
        int       vBill_Order;               /* ���ʳ���˳�� */
        long      vSerial_No;                /* �� �� ��     */
        int       vPay_Order;                /* ���ѳ���˳�� */
        char      vBegin_YMD[8+1];           /* ��ʼ����     */
        char      vBegin_TM[6+1];            /* ��ʼʱ��     */
        char      vEnd_YMD[8+1];             /* ��������     */
        char      vEnd_TM[6+1];              /* ����ʱ��     */
        float     vLimit_Pay;                /* ����ȷ       */
        char      vRate_Flag[1+1];           /* ����ȷ       */
        char      vStop_Flag[1+1];           /* ����ȷ       */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tUserAccRelation;

typedef struct CustSim
{
        long      vId_No;                    /* �û�ID       */
        char      vPhone_No[15+1];           /* �������     */
        char      vSim_No[20+1];             /* SIM����      */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tCustSim;

typedef struct CustPost
{
        long      vId_No;                    /* �û�ID       */
        long      vContract_No;              /* �ʻ�ID       */
        char      vPost_Flag[1+1];           /* �Ƿ��ʼ�     *
                                              * 0:��,1:��    */  
        char      vPost_Type[1+1];           /* �ʼķ�ʽ     *
                                              * 0:����       *
                                              * 1:�ʼ�       *
                                              * 2:�����ʼ�   */
        char      vPost_Addr[100+1];         /* �ʼĵ�ַ     */
        char      vPost_Zip[6+1];            /* ��������     */
        char      vFax_No[30+1];             /* �������     */
        char      vMail_Addr[30+1];          /* Email��ַ    */
        char      vPost_Name[60+1];          /* �ʼ�����     */
        char      vPostNo_Flag[1+1];         /* �ʼ�����     *
                                              * 0:���û��ʼ� *
                                              * 1:���ʻ��ʼ� */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tCustPost;

typedef struct CustInnet
{
        long      vId_No;                    /* �û�ID       */
        long      vCust_Id;                  /* �ͻ�ID       */
        char      vBuy_Name[20+1];           /* ������       */
        char      vBelong_Code[7+1];         /* ��������     */
        char      vTown_Code[3+1];           /* �������     */
        char      vInnet_Type[2+1];          /* ��������     */
        char      vOpen_Time[17+1];          /* ����ʱ��     */
        char      vMachine_Code[3+1];        /* ��������     */
        float     vCash_Pay;                 /* �ֽ𽻿�     */
        float     vCheck_Pay;                /* ֧Ʊ����     */
        float     vSim_Fee;                  /* SIM����      */
        float     vMachine_Fee;              /* ������       */
        float     vInnet_Fee;                /* ������       */
        float     vChoice_Fee;               /* ѡ�ŷ�       */
        float     vOther_Fee;                /* ������       */
        float     vHand_Fee;                 /* ������       */
        float     vDeposit;                  /* Ѻ��         */
        char      vBack_Flag[1+1];           /* ��Ч��־     *
                                              * 0:��Ч,1:����*/
        char      vEncrypt_Fee[16+1];        /* �����ֶ�     */
        char      vSystem_Note[60+1];        /* ϵͳ��ע     */
        char      vOp_Note[60+1];            /* ������ע     */
        long      vAssure_No;                /* ����ID       */
        char      vGroup_Id[10+1];           /* �����ڵ�     */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tCustInnet;

typedef struct AssuMsg
{
        long      vAssure_No;                /* ������(output) */
        char      vAssure_Name[60+1];        /* ����������     */
        char      vId_Type[1+1];             /* ������֤������ */
        char      vAssure_Id[20+1];          /* ������֤������ */
        char      vAssure_Phone[20+1];       /* ��������ϵ�绰 */
        char      vAssure_Zip[6+1];          /* �������������� */
        char      vAssure_Addr[60+1];        /* �����˵�ַ     */
        int       vAssure_Num;               /* ��������       */
        
        long      vLogin_Accept;             /* ������ˮ     */
        char      vOp_Time[17+1];            /* ����ʱ��     */
        int       vTotal_Date;               /* ��������     */
        char      vLogin_No[6+1];            /* ��������     */
        char      vOp_Code[4+1];             /* ��������     */
} tAssuMsg;


typedef struct CredDoc
{
        char      vCred_Id[21+1];            /* ��E��ƾ֤���� */
        char      vPhone_No[15+1];           /* �ƶ�����      */
        char      vUse_Type[1+1];            /* ʹ�÷�ʽ      *
                                              * 1:��E�п���   */
        long      vId_No;                    /* �û�ID        */
        
        long      vLogin_Accept;             /* ������ˮ      */
        char      vOp_Time[17+1];            /* ����ʱ��      */
        int       vTotal_Date;               /* ��������      */
        char      vLogin_No[6+1];            /* ��������      */
        char      vOp_Code[4+1];             /* ��������      */
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
    char    vyShouldFee[15+1];   /* ӪҵӦ��*/
    char    vyRealFee[15+1];     /* Ӫҵʵ�� */
    char    vzShouldFee[15+1];   /* ����Ӧ�� */
    char    vzRealFee[15+1];     /* ����ʵ��*/
} tProdSrvFeeList;

/* �����û���Ϣ��Ӧ�ṹ */
typedef struct tGrpUserMsg
{
    char    org_code[7+1];         /* �������     */
    char    login_name[20+1];      /* �Է����û��� */
    char    login_passwd[20+1];    /* �Է�������   */
    char    run_code[1+1];         /* �û�״̬     */
    char    old_run[1+1];          /* ǰһ״̬     */
    char    run_time[20+1];        /* ״̬�仯ʱ�� */
    char    bill_date[20+1];       /* ����ʱ��     */
    char    last_bill_type[1+1];   /* �ϴγ������� */
    char    limit_owe[18+1];       /* ͸֧���     */
    char    ids[4+1];              /* �ʻ���       */
    char    belong_code[7+1];         /* �������     */
    char    cust_id[14+1];         /* ���ſͻ�ID   */
    char    id_no[14+1];           /* �����û�ID   */
    char    user_no[20+1];         /* �û����     */
    char    account_no[14+1];      /* �û��ʺ�     */
    char    account_type[1+1];     /* �ʻ�����     */
    char    account_passwd[8+1];   /* �û��ʻ����� */
    char    user_name[60+1];       /* �û�����     */
    char    sm_code[2+1];          /* ��ƷƷ��     */
    char    product_code[8+1];     /* ��Ʒ����     */
    char    product_type[8+1];     /* ��Ʒ����     */
    char    user_passwd[8+1];      /* ҵ���������� */
    char    prov_code[2+1];        /* ����ʡ       */
    char    region_code[2+1];      /* ��������     */
    char    district_code[2+1];    /* ������       */
    char    town_code[3+1];        /* ����Ӫҵ��   */
    char    territory_code[20+1];  /* ���ش���     */
    char    credit_code[2+1];      /* ���õȼ�     */
    char    credit_value[18+1];    /* ���ö�       */
    char    bill_type[4+1];        /* �������ڴ��� */
    char    op_time[20+1];         /* ����ʱ��     */
    char    srv_start[10+1];       /* ҵ����ʼ���� */
    char    srv_stop[10+1];        /* ҵ��������� */
    char    bak_field[16+1];       /* Ԥ���ֶ�     */
    char    group_id[10+1];        /* GROUP_ID     */
    char    org_id[10+1];          /* ������GROUP_ID*/
    char    channel_id[10+1];      /* �����ӵ�      */
    char    return_message[60+1];  /* ���÷�����Ϣ */
    char    pay_code[1+1];         /* �����ʻ����ʽ */
    char	service_code[4+1];	   /*�����Żݴ���*/
    tProdSrvFeeList feeList;
}tGrpUserMsg;


typedef struct ShortMsg
{
    long    vLogin_Accept;         /* ������ˮ     */
    long    vMsg_Level;            /* ��Ϣ�ȼ�     */
    char    vPhone_No[15+1];       /* �ƶ�����     */
    char    vSend_Msg[255+1];      /* ������Ϣ     */
    int     vOrder_Code;           /* ����˳��     */
    int     vBack_Code;            /* ���ش���     */
    char    vOp_Code[4+1];         /* ��������     */
    char    vLogin_No[6+1];        /* ��������     */
} tShortMsg;

typedef struct CustFunc
{
    long    vId_No;                 /* �û�ID      */
    char    vPhone_No[15+1];        /* �ƶ�����    */
    char    vSm_Code[2+1];          /* ҵ�����    */
    char    vOrg_Code[9+1];         /* ��������    */
    char    vBelong_Code[7+1];      /* ��������    */
    char    vFunc_Type[1+1];        /* �ط�����    */
    char    vFunc_Code[2+1];        /* �ط�����    */
    char    vBegin_Time[17+1];      /* ��ͨʱ��    *
                                     * YYYYMMDD    */
    char    vAdd_No[15+1];          /* ���Ӻ���    */
    char    vOther_Attr[40+1];      /* ��������    */
    char    vExpire_Time[17+1];     /* ԤԼʱ��    */
    
    char    vOp_Note[60+1];         /* ������ע    */
    char    vOrg_Id[10+1];          /* ����ʶ��    */
    
    long    vLogin_Accept;          /* ������ˮ    */
    char    vOp_Time[17+1];         /* ����ʱ��    */
    int     vTotal_Date;            /* ��������    */
    char    vLogin_No[6+1];         /* ��������    */
    char    vOp_Code[4+1];          /* ��������    */
} tCustFunc;

typedef struct LockPhone
{
    char        vOp_Type[1+1];         /* ��������    *
                                        * 0:��ͨ����  *
                                        * 1:��������  *
                                        * 2:�츮�м���*/
    char        vPhone_No[15+1];       /* �ƶ�����    */
    char        vGroup_Id[10+1];       /* �����ڵ�    */
    char        vOp_Code[4+1];         /* ��������    */
    char        vSm_Code[2+1];         /* ҵ�����    */
    char        vLock_Flag[1+1];       /* �ӽ�����־  *
                                        * 0: ����     *
                                        * 1: ����     */
    float       vChoice_Fee;           /* ѡ�ŷ�      */
    float       vChoice_Prepay;        /* ѡ��Ԥ��    */
} tLockPhone;

typedef struct LockSim
{
    char        vPhone_No[15+1];       /* �ƶ�����    */
    char        vGroup_Id[10+1];       /* �����ڵ�    */
    char        vOp_Code[4+1];         /* ��������    */
    char        vSm_Code[2+1];         /* ҵ�����    */
    char        vSim_No[20+1];         /* SIM����     */
    char        vLock_Flag[1+1];       /* �ӽ�����־  *
                                        * 0: ����     *
                                        * 1: ����     */
    float       vSim_Fee;              /* SIM����     */
    float       vSim_Value;            /* SIM����ֵ   */
} tLockSim;

/* lixg define: user product infomation begin */
#define	USER_MODE_NUM	10	/* �û���󶨹��ײ����� */
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
* �ɷѻ������ݽṹ����
**/
#define  PROD_SEND_NOW	'0' 	/* �������� */
#define  PROD_SEND_NEXT	'1' 	/* �������� */

#define  PROD_SENDFEE_CODE	'C' 	/* �ɷѻ��� */
#define  PROD_MACHFEE_CODE	'E'	/* ���ֻ��ͻ��� */

#define  GRANT_DETTYPE_SENDXJ	'0'	/* �����ֽ� */
#define  GRANT_DETTYPE_SENDSW	'1'	/* ����ʵ�� */
#define  GRANT_DETTYPE_SENDJF	'2'	/* �������� */
#define  GRANT_DETTYPE_REQYC	'P'	/* Ԥ���� */

typedef struct{
	char	applyType[1+1]; /* C: �ɷѻ���, E:���ֻ������� */
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
 *     ���Ž���Ա���Ʊ�������
 */
#define CGU_NEW_USER    1                       /*�½�һ���û�*/
#define CGU_RESUME_USER 2                       /*�ָ�ԤԼɾ���ĳ�Ա�û�*/
typedef struct tagCreateGrpUser
{
        /**
         *     ���½�һ����Ա�û������ǻָ�ԤԼɾ���ĳ�Ա�û�,
         * ֵ���壺
         *     CGU_NEW_USER     �½�һ���û�
         *     CGU_RESUME_USER  �ָ�ԤԼɾ���ĳ�Ա�û�
         */
        int             createFlag;

        char    loginAccept[22+1];              /*��ˮ*/
        char    opCode[4+1];                    /*���ܴ���*/
        char    loginNo[6+1];                   /*��������*/
        char    orgCode[9+1];                   /*�������Ź���*/
        char    phoneNo[15+1];                  /*�������*/
        char    smCode[2+1];                    /*ҵ������*/
        char    serviceType[2+1];               /*�������� dCustMsg::service_type*/
        char    belongCode[7+1];                /*��������*/
        char    opTime[17+1];                   /*����ʱ��      'YYYYMMDD HH24:MI:SS'*/
        char    custId[22+1];                   /*�ͻ�ID*/
        char    productCode[8+1];               /*���Ų�Ʒ����*/
        char    grpIdNo[22+1];                  /*�����û�ID*/
        char    contractNo[22+1];               /*�����ʺ�      ���ΪNULL����ʾû�и����ʻ���ע����ʺŽ�����Ա�û������з���*/
        char    groupId[10+1];                  /*����Ĺ���*/
        char    orgId[10+1];                    /*���ŵ�ID*/
        char    effectTime[17+1];               /*�ɺŹ�ϵ����Чʱ�䣺dGrpUserMebMsg:begin_time*/
} TCreateGrpUser;


typedef struct tagDeleteGrpUser
{
        char    loginAccept[22+1];              /*��ˮ*/
        char    opCode[4+1];                    /*���ܴ���*/
        char    loginNo[6+1];                   /*��������*/
        char    orgCode[9+1];                   /*�������Ź���*/
        char    idNo[22+1];                             /*��������ID*/
        char    opTime[17+1];                   /*����ʱ��      'YYYYMMDD HH24:MI:SS'*/
        char    grpIdNo[22+1];                  /*�����û�ID*/
        char    productCode[8+1];               /*���Ų�Ʒ����*/
        char    contractNo[22+1];               /*�����ʺ�      ���ΪNULL����ʾû�и����ʻ���ע����ʺŽ�����Ա�û������з���*/
        char    groupId[10+1];                  /*����Ĺ���*/
        char    orgId[10+1];                    /*���ŵ�ID*/
        char    effectTime[17+1];               /*�ɺŹ�ϵ����Чʱ�䣺dGrpUserMebMsg:begin_time*/
} TDeleteGrpUser;

/* ��������Ϣ�ṹ�� */
struct DCUSTFAZE_TYPE{
	char phone_no[16];    /* �绰���� */
	char faze_type[2];    /* ɧ�����ͣ�0����ɧ�ţ� 2������ɧ�ţ�1���绰ɧ�� */
	char disposeway[3];   /* ���ͷ�ʽ��00�����ˣ�01������SP��02�����ſͻ����� */
	char belong_type[2];  /* �������ͣ�0����ʡ�û���1����ʡ�û���2�������û� */
	char grade_code[2];   /* ���뼶��0�����ƺ�������1����������2�������� */
	char begin_time[18];  /* ����ʱ�� */
	char content[280];
	char keyword[170];
	char reason[2];
	char phonenoflag[2];
	char dstarea[3];
	char smstype[2];
};
typedef struct DCUSTFAZE_TYPE  DCUSTFAZE_TYPE;

/* �����ʷ������Ϣ�ṹ�� */
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
