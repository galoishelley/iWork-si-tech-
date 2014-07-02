#ifndef __PUBORDER_H__
#define __PUBORDER_H__


typedef struct tagdConMsg
{
	char sContract_no[14+1];        /* CONTRACT_NO     NUMBER(14)   */
	char sConCustId[14+1];          /* CON_CUST_ID     NUMBER(14)   */
	char sContractPasswd[8+1];      /* CONTRACT_PASSWD CHAR(8)      */
	char sBankCust[60+1];           /* BANK_CUST       VARCHAR2(60) */
	char sOddment[5];               /* ODDMENT         NUMBER(2,2)  */
	char sBelongCode[7+1];          /* BELONG_CODE     CHAR(7)      */
	char sPrepayFee[17];            /* PREPAY_FEE      NUMBER(14,2) */
	char sPrepayTime[17+1];         /* PREPAY_TIME     DATE         */
	char sStatus[1+1];              /* STATUS          CHAR(1)      */
	char sStatus_time[17+1];        /* STATUS_TIME     DATE         */
	char sPostFlag[1+1];            /* POST_FLAG       CHAR(1)      */
	char sDeposit[17];              /* DEPOSIT         NUMBER(14,2) */
	char sMinYm[6+1];               /* MIN_YM          NUMBER(6)    */
	char sOweFee[17];               /* OWE_FEE         NUMBER(14,2) */
	char sAccountMark[10+1];        /* ACCOUNT_MARK    NUMBER(10)   */
	char sAccountLimit[1+1];        /* ACCOUNT_LIMIT   CHAR(1)      */
	char sPayCode[5+1];             /* PAY_CODE        CHAR(1)      */
	char sBankCode[5+1];            /* BANK_CODE       CHAR(5)      */
	char sPostBankCode[5+1];        /* POST_BANK_CODE  CHAR(5)      */
	char sAccountNo[30+1];          /* ACCOUNT_NO      CHAR(30)     */
	char sAccountType[1+1];         /* ACCOUNT_TYPE    CHAR(1)      */
	char sGroupId[10];              /* GROUP_ID        VARCHAR2(10) */
} TdConMsg;

/****jiangqing add 20090401 begin****/
typedef struct tagdConMsgDead
{
	char sContractNo[14+1];        /* CONTRACT_NO     NUMBER(14)   */
	char sConCustId[14+1];          /* CON_CUST_ID     NUMBER(14)   */
	char sContractPasswd[8+1];      /* CONTRACT_PASSWD CHAR(8)      */
	char sBankCust[60+1];           /* BANK_CUST       VARCHAR2(60) */
	char sOddment[5];               /* ODDMENT         NUMBER(2,2)  */
	char sBelongCode[7+1];          /* BELONG_CODE     CHAR(7)      */
	char sPrepayFee[17];            /* PREPAY_FEE      NUMBER(14,2) */
	char sPrepayTime[20+1];         /* PREPAY_TIME     DATE         */
	char sStatus[1+1];              /* STATUS          CHAR(1)      */
	char sStatusTime[20+1];        /* STATUS_TIME     DATE         */
	char sPostFlag[1+1];            /* POST_FLAG       CHAR(1)      */
	char sDeposit[17];              /* DEPOSIT         NUMBER(14,2) */
	char sMinYm[6+1];               /* MIN_YM          NUMBER(6)    */
	char sOweFee[17];               /* OWE_FEE         NUMBER(14,2) */
	char sAccountMark[10+1];        /* ACCOUNT_MARK    NUMBER(10)   */
	char sAccountLimit[1+1];        /* ACCOUNT_LIMIT   CHAR(1)      */
	char sPayCode[5+1];             /* PAY_CODE        CHAR(1)      */
	char sBankCode[5+1];            /* BANK_CODE       CHAR(5)      */
	char sPostBankCode[5+1];        /* POST_BANK_CODE  CHAR(5)      */
	char sAccountNo[30+1];          /* ACCOUNT_NO      CHAR(30)     */
	char sAccountType[1+1];         /* ACCOUNT_TYPE    CHAR(1)      */
	char sGroupId[10];              /* GROUP_ID        VARCHAR2(10) */
} TdConMsgDead;
/****jiangqing add 20090401 end****/

typedef struct tagdCustMsg
{
	char sIdNo[14+1];
	char sCustId[10+1];
	char sContractNo[14+1];
	char sIds[4+1];
	char sPhoneNo[15+1];
	char sSmCode[2+1];
	char sServiceType[2+1];
	char sAttrCode[8+1];
	char sUserPasswd[8+1];
	char sOpenTime[17+1];
	char sBelongCode[7+1];
	char sLimitOwe[15+1];
	char sCreditCode[1+1];
	char sCreditValue[8+1];
	char sRunCode[2+1];
	char sRunTime[17+1];
	char sBillDate[17+1];
	char sBillType[4+1];
	char sEncryptPrepay[16+1];
	char sCmdRight[10+1];
	char sLastBillType[1+1];
	char sBakField[12+1];
	char sGroupId[20+1];
	char sGroupNo[10+1];
	char sgroupmsg[20+1];
}TdCustMsg;

/*********************added by zhangmeng begin **********************/
typedef struct tagdBillCustDetail{
	char sIdNo[14+1];				/* ID_NO			NUMBER(14)	*/
	char sDetailType[1+1];			/* DETAIL_TYPE		CHAR(1)		*/
	char sDetailCode[4+1];			/* DETAIL_CODE		CHAR(4)		*/
	char sBeginTime[17+1];			/* BEGIN_TIME		DATE		*/
	char sEndTime[17+1];			/* END_TIME			DATE		*/
	char sFavOrder[4+1];			/* FAV_ORDER		NUMBER(4)	*/
	char sModeCode[8+1];			/* MODE_CODE		CHAR(8)		*/
	char sModeFlag[1+1];			/* MODE_FLAG		CHAR(1)		*/
	char sModeTime[1+1];			/* MODE_TIME		CHAR(1)		*/
	char sModeStatus[1+1];			/* MODE_STATUS		CHAR(1)		*/
	char sLoginAccept[14+1];		/* LOGIN_ACCEPT		NUMBER(14)	*/
	char sOpCode[4+1];				/* OP_CODE			CHAR(4)		*/
	char sTotalDate[8+1];			/* TOTAL_DATE		NUMBER(8)	*/
	char sOpTime[17+1];				/* OP_TIME			DATE		*/
	char sLoginNo[6+1];				/* LOGIN_NO			CHAR(6)		*/
	char sRegionCode[2+1];			/* REGION_CODE		CHAR(2)		*/
}TdBillCustDetail;

typedef struct tagdBillCustDetailIndex{
	char sIdNo[14+1];				/* ID_NO			NUMBER(14)	*/
	char sModeCode[8+1];			/* MODE_CODE		CHAR(8)		*/
	char sLoginAccept[14+1];		/* LOGIN_ACCEPT		NUMBER(14)	*/
	char sDetailType[1+1];			/* DETAIL_TYPE		CHAR(1)		*/
	char sDetailCode[4+1];			/* DETAIL_CODE		CHAR(4)		*/
}TdBillCustDetailIndex;

typedef struct tagdConUserMsg
{
	char sIdNo[14+1];            	/* ID_NO		   NUMBER(14) 	*/
	char sContractNo[14+1];			/* CONTRACT_NO	   NUMBER(14)	*/
	char sBillOrder[8+1];			/* BILL_ORDER	   NUMBER(8)	*/
	char sSerialNo[10+1];			/* SERIAL_NO	   NUMBER(10)	*/
	char sPayOrder[8+1];			/* PAY_ORDER	   NUMBER(8)	*/
	char sBeginYmd[8+1];			/* BEGIN_YMD	   CHAR(8)		*/
	char sBeginTm[6+1];				/* BEGIN_TM	       CHAR(6)		*/
	char sEndYmd[8+1];				/* END_YMD		   CHAR(8)		*/
	char sEndTm[6+1];				/* END_TM		   CHAR(6)		*/
	char sLimitPay[14+1];			/* LIMIT_PAY	   NUMBER(14,2)	*/
	char sRateFlag[1+1];			/* RATE_FLAG	   CHAR(1)		*/
	char sStopFlag[1+1];			/* STOP_FLAG	   CHAR(1)		*/
}TdConUserMsg;
/*********************added by zhangmeng end **********************/

typedef struct tagdConUserMsgIndex
{
	char sIdNo[14+1];
	char sContractNo[14+1];
	char sSerialNo[10+1];
}TdConUserMsgIndex;

/*********************added by yangsong begin************************/
typedef struct tagdCustRealDoc
{
	char sReal_id[14+1];            /* REAL_ID         NUMBER(14)   */
	char sRealStatus[3+1];          /* REAL_STATUS     CHAR(3)      */
	char sRegionCode[2+1];          /* REGION_CODE     CHAR(2)      */
	char sDistrictCode[2+1];        /* DISTRICT_CODE   CHAR(2)		*/
	char sTownCode[3+1];            /* TOWN_CODE       CHAR(3)		*/
	char sRealName[60+1];           /* REAL_NAME       VARCHAR2(60) */
	char sRealAddr[60+1];           /* REAL_ADDR       VARCHAR2(60) */
	char sRealIdType[1+1];          /* REAL_IDTYPE     CHAR(1)		*/
	char sRealIccid[20+1];          /* REAL_ICCID      VARCHAR2(20)	*/
	char sRealIdaddr[60+1];         /* REAL_IDADDR     VARCHAR2(60)	*/
	char sRealIdedate[8+1];        /* REAL_IDEDATE    	DATE			*/
	char sOrgName[60+1];            /* ORG_NAME        CHAR(60)		*/
	char sOrgCode[60+1];            /* ORG_CODE        CHAR(60)		*/
	char sContactType[1+1];         /* CONTACT_TYPE    CHAR(1)		*/
	char sContactName[60+1];        /* CONTACT_NAME    VARCHAR2(60)	*/
	char sContactAddr[60+1];        /* CONTACT_ADDR    VARCHAR2(60)	*/
	char sContactPhone[20+1];       /* CONTACT_PHONE   VARCHAR2(20)	*/
	char sContactIdtype[1+1];       /* CONTACT_IDTYPE  VARCHAR2(1)	*/
	char sContactIccid[20+1];       /* CONTACT_ICCID   VARCHAR2(20)	*/
	char sContactIdaddr[60+1];      /* CONTACT_IDADDR  CHAR(60)		*/
	char sContactIdedate[8+1];     /* CONTACT_IDEDATE	DATE			*/
	char sContactEmaill[30+1];      /* CONTACT_EMAILL  VARCHAR2(30)	*/
	char sLoginNo[6+1];             /* LOGIN_NO    	   CHAR(6)		*/
	char sOpChn[8+1];               /* OP_CHN          CHAR(8)		*/
	char sOpTime[17+1];             /* OP_TIME         DATE			*/
	char sRealNote[60+1];           /* REAL_NOTE       CHAR(60)		*/
	char sGroupId[10+1];            /* GROUP_ID        VARCHAR2(10) */
}TdCustRealDoc;

/* by wangls */
typedef struct tagdCustDoc{

	char	sCustId[10+1];				/*cust_id			number(10)*/
	char	sRegionCode[2+1];			/*region_code		CHAR(2)*/
	char	sDistrictCode[2+1];			/*district_code		CHAR(2)*/
	char	sTownCode[3+1];				/*town_code			CHAR(3)*/
	char	sCustName[60+1];			/*cust_name			varchar2(60)*/
	char	sCustPasswd[8+1];			/*cust_passwd		CHAR(8)*/
	char	sCustStatus[2+1];			/*cust_status		CHAR(2)*/
	char	sRunTime[17+1];				/*run_time			DATE*/
	char	sOwnerGrade[2+1];			/*owner_grade		CHAR(2)*/
	char	sOwnerType[2+1];			/*owner_type		CHAR(2)*/
	char	sCustAddress[60+1];			/*cust_address		varchar2(60)*/
	char	sIdType[2+1];				/*id_type			CHAR(2)*/
	char	sIdIccid[20+1];				/*id_iccid			CHAR(20)*/
	char	sIdAddress[60+1];			/*id_address		varchar2(60)*/
	char	sIdValiddate[17+1];			/*id_validdate		DATE*/
	char	sContactPerson[20+1];		/*contact_person	CHAR(20)*/
	char	sContactPhone[20+1];		/*contact_phone		CHAR(20)*/
	char	sContactAddress[100+1];		/*contact_address	varchar2(100)*/
	char	sContactPost[6+1];			/*contact_post		CHAR(6)*/
	char	sContactMailaddress[60+1];	/*contact_mailaddress	varchar2(60)*/
	char	sContactFax[30+1];			/*contact_fax		CHAR(30)*/
	char	sContactEmaill[30+1];		/*contact_emaill	CHAR(30)*/
	char	sOrgCode[9+1];				/*org_code			CHAR(9)*/
	char	sCreateTime[17+1];			/*create_time		DATE*/
	char	sCloseTime[17+1];			/*close_time		DATE*/
	char	sParentId[14+1];			/*parent_id			number(14)*/
	char	sCreateNote[60+1];			/*create_note		varchar2(60)*/
	char	sTrueFlag[1+1];
	char	sInfoType[2+1];
	char	sAgreementType[1+1];
	char	sGroupId[10+1];				/*group_id			varchar2(10) */
	char	sOrgId[10+1];				/*org_id			varchar2(10) */
}TdCustDoc;		/* by wangls */

/**************added by MengPan end******************/
typedef struct tagdCustDocInAdd {
	char scust_id[14+1];
    char sSex[1+1];
	char sBirthday[17+1];
	char sProfessionId[4+1];
	char sEduLevel[2+1];
	char sCustLove[60+1];
	char sCustHabit[60+1];
}TdCustDocInAdd;
/****************************************************/

typedef struct tagdConUserRate {
	char sIdNo[14+1];
	char sContractNo[14+1];
	char sBillOrder[8+1];
	char sFeeCode[4+1];
	char sPayOrder[8+1];
	char sFeeRate[15+1];
	char sDetailCode[6+1];
}TdConUserRate;

typedef struct tagdConUserRateIndex{
	char sIdNo[14+1];
	char sContractNo[14+1];
	char sBillOrder[8+1];
	char sFeeCode[6+1];
	char sDetailCode[6+1];
}TdConUserRateIndex;

typedef struct tagdApprovalRelmsg
{
        char sApplyLogin[6+1];
        char sApplyPhone[15+1];
        char sApprovalLogin[6+1];
        char sApprovalPhone[15+1];
        char sApplyOrder[1+1];
        char sApprovalOrder[1+1];
        char sEffDate[17+1];
        char sExpDate[17+1];
}TdApprovalRelmsg;

typedef struct tagdApprovalOprhis
{
        char sLoginAccept[14+1];
        char sCustId[10+1];
        char sIdNo[14+1];
        char sContractNo[14+1];
        char sProductCode[8+1];
        char sMinYM[6+1];
        char sOweFee[17+1];
        char sOpType[1+1];
        char sTypeOrder[1+1];
        char sApplyFlag[1+1];
        char sApplyLogin[6+1];
        char sApprovalLogin[6+1];
        char sOpTime[17+1];
        char sInsetTime[17+1];
}TdApprovalOprhis;

typedef struct tagdBaseFav {
	char sMsisdn[15+1];
	char sRegionCode[2+1];
	char sFavBrand[2+1];
	char sFavType[4+1];
	char sFlagCode[10+1];
	char sFavOrder[1+1];
	char sFavDay[1+1];
	char sStartTime[17+1];
	char sEndTime[17+1];
	char sServiceId[11+1];
	char sFavPeriod[1+1];
	char sFreeValue[10+1];
	char sIdNo[14+1];
	char sGroupId[10+1];
	char sProductCode[8+1];
}TdBaseFav;

typedef struct tagdBaseFavIndex{
	char sMsisdn[15+1];
	char sFavType[4+1];
	char sFlagCode[10+1];
	char sStartTime[17+1];
}TdBaseFavIndex;

typedef struct tagdApprovalOpr
{
        char sLoginAccept[14+1];
        char sCustId[10+1];
        char sIdNo[14+1];
        char sContractNo[14+1];
        char sProductCode[8+1];
        char sMinYM[6+1];
        char sOweFee[17+1];
        char sOpType[1+1];
        char sTypeOrder[1+1];
        char sApplyLogin[6+1];
        char sApprovalLogin[6+1];
        char sOpTime[17+1];
}TdApprovalOpr;

typedef struct tagdBaseVpmn{
	char sMsisdn[11+1];
	char sGroupId[10+1];
	char sValid[17+1];
	char sInvalid[17+1];
	char sMocrate[8+1];
	char sMtcrate[8+1];
}TdBaseVpmn;

typedef struct tagdBaseVpmnIndex{
	char sMsisdn[11+1];
	char sGroupId[10+1];
	char sValid[17+1];
}TdBaseVpmnIndex;

typedef struct tagdRelationFav{
	char sMsisdn[15+1];
	char sRegionCode[2+1];
	char sFavType[5+1];
	char sOtherCode[20+1];
	char sStartTime[17+1];
	char sEndTime[17+1];
	char sIdNo[14+1];
}TdRelationFav;

typedef struct tagdRelationFavIndex{
	char sMsisdn[15+1];
	char sFavType[5+1];
	char sOtherCode[20+1];
	char sStartTime[17+1];
}TdRelationFavIndex;

typedef struct tagdIngwUser{
	char sIdNo[14+1];
	char sMsisdn[15+1];
	char sRegionCode[2+1];
	char sBeginDate[17+1];
	char sEndDate[17+1];
	char sPrepayFee[15+1];
	char sOptList[36+1];
}TdIngwUser;

typedef struct tagdIngwUserIndex{
	char sMsisdn[15+1];
	char sBeginDate[17+1];
}TdIngwUserIndex;

typedef struct tagdVpmnMember{
	char sGroupNo[10+1];
	char sPhoneNumber[24+1];
	char sPhoneType[8+1];
	char sRights[15+1];
	char sClosedNum1[10+1];
	char sClosedNum2[10+1];
	char sClosedNum3[10+1];
	char sClosedNum4[10+1];
	char sClosedNum5[10+1];
	char sBeginDate[17+1];
	char sEndDate[17+1];
	char sNumberFavType[4+1];
	char sNumberOutFavType[4+1];
}TdVpmnMember;

typedef struct tagdVpmnMemberIndex{
	char sGroupNo[10+1];
	char sPhoneNumber[24+1];
	char sBeginDate[17+1];
}TdVpmnMemberIndex;

typedef struct tagdVpmnInfo{
	char sGroupNo[10+1];
	char sClosedNo[10+1];
	char sRights[15+1];
	char sClosedGroupType[1+1];
	char sGroupFavType[4+1];
	char sGroupOutFavType[4+1];
	char sTelephonistType[1+1];
	char sBeginDate[17+1];
	char sEndDate[17+1];
}TdVpmnInfo;

typedef struct tagdVpmnInfoIndex{
	char sGroupNo[10+1];
	char sClosedNo[10+1];
	char sBeginDate[17+1];
}TdVpmnInfoIndex;

typedef struct tagdAllUser{
	char sMsisdn[15+1];
    char sRegionCode[2+1];
    char sFavBrand[2+1];
    char sFavType[4+1];
    char sFlagCode[10+1];
    char sFavOrder[1+1];
    char sFavDay[1+1];
    char sStartTime[17+1];
    char sEndTime[17+1];
    char sServiceId[11+1];
    char sFavPeriod[1+1];
    char sFreeValue[10+1];
    char sIdNo[14+1];
    char sGroupId[10+1];
    char sProductCode[8+1];
}TdAllUser;

typedef struct tagdAllUserIndex{
	char sMsisdn[15+1];
	char sFavType[4+1];
	char sFlagCode[10+1];
	char sStartTime[17+1];
}TdAllUserIndex;

typedef struct tagdCustInnet{
	char sIdNo[14+1];
	char sCustId[10+1];
	char sBuyName[20+1];
	char sBelongCode[7+1];
	char sTownCode[3+1];
	char sInnetType[2+1];
	char sOpenTime[17+1];
	char sLoginAccept[14+1];
	char sLoginNo[6+1];
	char sMachineCode[3+1];
	char sCashPay[18+1];
	char sCheckPay[18+1];
	char sSimFee[18+1];
	char sMachineFee[18+1];
	char sInnetFee[18+1];
	char sChoiceFee[18+1];
	char sOtherFee[18+1];
	char sHandFee[18+1];
	char sDeposit[18+1];
	char sBackFlag[1+1];
	char sEncryptFee[16+1];
	char sSystemNote[60+1];
	char sOpNote[60+1];
	char sAssureNo[14+1];
	char sGroupId[10+1];
}TdCustInnet;

typedef struct tagdCustFunc{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
	char sOpTime[17+1];
}TdCustFunc;

typedef struct tagdCustFuncIndex{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
}TdCustFuncIndex;

typedef struct tagdGrpCustAwoke
{
        char sCustID[14+1];
        char sIdNo[14+1];
        char sAwokeType[2+1];
        char sAwokeFeeFz[14+1];
        char sReAwokeDay[2+1];
        char sTimeFlag[2+1];
        char sWeekDay[1+1];
        char sMonthDay[2+1];
        char sLoginAccept[14+1];
        char sNote[60+1];
        char sSendPhone[15+1];
        char sOpTime[17+1];
}TdGrpCustAwoke;

typedef struct tagdCustFuncHis{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
	char sOpTime[17+1];
	char sUpdateLogin[6+1];
	char sUpdateAccept[14+1];
	char sUpdateDate[8+1];
	char sUpdateTime[17+1];
	char sUpdateCode[4+1];
	char sUpdateType[1+1];
}TdCustFuncHis;

typedef struct tagdCustFuncHisIndex{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
	char sUpdateAccept[14+1];
	char sUpdateDate[8+1];
	char sUpdateType[1+1];
}TdCustFuncHisIndex;

typedef struct tagdCustMsgAdd{
	char sIdNo[14+1];
	char sBusiType[4+1];
	char sUserType[4+1];
	char sFieldCode[5+1];
	char sFieldOrder[8+1];
	char sFieldValue[255+1];
	char sOtherValue[255+1];
}TdCustMsgAdd;

typedef struct tagdCustMsgAddIndex{
	char sIdNo[14+1];
	char sBusiType[4+1];
	char sUserType[4+1];
	char sFieldCode[5+1];
	char sFieldOrder[8+1];
}TdCustMsgAddIndex;

typedef struct tagdCustMsgDead
{
	char sIdNo[14+1];
	char sCustId[10+1];
	char sContractNo[14+1];
	char sIds[4+1];
	char sPhoneNo[15+1];
	char sSmCode[2+1];
	char sServiceType[2+1];
	char sAttrCode[8+1];
	char sUserPasswd[8+1];
	char sOpenTime[17+1];
	char sBelongCode[7+1];
	char sLimitOwe[16+1];
	char sCreditCode[1+1];
	char sCreditValue[8+1];
	char sRunCode[2+1];
	char sRunTime[17+1];
	char sBillDate[17+1];
	char sBillType[4+1];
	char sEncryptPrepay[16+1];
	char sCmdRight[4+1];
	char sLastBillType[1+1];
	char sBakField[12+1];
	char sGroupId[20+1];
	char sGroupNo[10+1];
}TdCustMsgDead;

typedef struct tagdCustConMsg
{
	char sCustId[10+1];
	char sContractNo[14+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
}TdCustConMsg;

typedef struct tagdCustConMsgIndex{
	char sCustId[10+1];
	char sContractNo[14+1];
}TdCustConMsgIndex;

typedef struct tagdGrpUserMsg
{
	char sCustId[14+1];
	char sIdNo[14+1];
	char sAccountId[14+1];
	char sUserNo[15+1];
	char sIds[4+1];
	char sUserName[60+1];
	char sProductType[4+1];
	char sProductCode[8+1];
	char sUserPasswd[8+1];
	char sLoginName[20+1];
	char sLoginPasswd[20+1];
	char sProvCode[2+1];
	char sRegionCode[2+1];
	char sDistrictCode[2+1];
	char sTownCode[3+1];
	char sTerritoryCode[20+1];
	char sLimitOwe[16+1];
	char sCreditCode[2+1];
	char sCreditValue[16+1];
	char sRunCode[1+1];
	char sOldRun[1+1];
	char sRunTime[17+1];
	char sBillDate[17+1];
	char sBillType[4+1];
	char sLastBillType[1+1];
	char sOpTime[17+1];
	char sBakField[16+1];
	char sSmCode[2+1];
	char sGroupId[10+1];
	char sProductPerson[32+1];
	char sProConPhone[15+1];
	char sProPersonId[14+1];
	char sAccountPerson[30+1];
	char sAccountPhone[15+1];
}TdGrpUserMsg;

typedef struct tagdGrpUserMebMsg
{
	char sIdNo[14+1];
	char sMemberId[14+1];
	char sMemberNo[20+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sBakField[16+1];
}TdGrpUserMebMsg;

typedef struct tagdGrpUserMebMsgIndex
{
	char sIdNo[14+1];
	char sMemberId[14+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
}TdGrpUserMebMsgIndex;

typedef struct tagdCustSim
{
	char sIdNo[14+1];
	char sSwitchNo[20+1];
	char sSimNo[20+1];
}TdCustSim;

typedef struct tagdCustExPire
{
	char sIdNo[14+1];
	char sOpenTime[17+1];
	char sBeginFlag[1+1];
	char sBeginTime[17+1];
	char sOldExpire[17+1];
	char sExpireTime[17+1];
	char sBakField[20+1];
	char sCannelFlag[1+1];
	char sUpdateFlagTime[17+1];
}TdCustExpire;

typedef struct tagdCustTransMsg
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sAgentCode[7+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sOpTime[17+1];
	char sLoginNo[15+1];
	char sTotalDate[8+1];
}TdCustTransMsg;

typedef struct tagdConMark
{
	char sContractNo[14+1];
	char sInitPoint[15+1];
	char sCurrentPoint[15+1];
	char sCurrentTime[17+1];
	char sMonthPoint[15+1];
	char sYearPoint[15+1];
	char sLastyearPoint[15+1];
	char sAddPoint[15+1];
	char sOwePoint[15+1];
	char sLastPoint[15+1];
	char sMonthUsed[15+1];
	char sYearUsed[15+1];
	char sTotalUsed[15+1];
	char sMinYm[6+1];
	char sContractStatus[2+1];
	char sOweLimit[15+1];
	char sStatusTime[17+1];
	char sChgFlag[1+1];
}TdConMark;

typedef struct tagdConFund
{
	char sContractNo[14+1];
	char sInitFund[15+1];
	char sCurrentFund[15+1];
	char sCurrentTime[17+1];
	char sMonthFund[15+1];
	char sYearFund[15+1];
	char sAddFund[15+1];
	char sMonthUsed[15+1];
	char sYearUsed[15+1];
	char sTotalUsed[15+1];
	char sMinYm[6+1];
	char sContractStatus[2+1];
	char sStatusTime[17+1];
	char sOpTime[17+1];
	char sChgFlag[1+1];
}TdConFund;

typedef struct tagdUserPost
{
	char sIdNo[14+1];
	char sPostFlag[1+1];
	char sPostType[1+1];
	char sPostAddress[200+1];
	char sPostZip[10+1];
	char sFaxNo[30+1];
	char sMailAddress[30+1];
	char sContractNo[14+1];
	char sPostName[60+1];
	char sPostnoFlag[1+1];
	char sEmailFlag[1+1];
	char sEmailType[1+1];
	char sSmsgFlag[1+1];
	char sClomsgFlag[1+1];
	char sOrderTime[8+1];
	char sSendflag[1+1];
}TdUserPost;

typedef struct tagdAssuMsg
{
	char sAssureNo[12+1];
	char sAssureName[60+1];
	char sIdType[1+1];
	char sAssureId[20+1];
	char sAssurePhone[20+1];
	char sAssureZip[6+1];
	char sAssureAddress[60+1];
	char sAssureNum[4+1];
}TdAssuMsg;

typedef struct tagdConUserMsgDead
{
	char sIdNo[14+1];            	/* ID_NO		   NUMBER(14) 	*/
	char sContractNo[14+1];			/* CONTRACT_NO	   NUMBER(14)	*/
	char sBillOrder[8+1];			/* BILL_ORDER	   NUMBER(8)	*/
	char sSerialNo[10+1];			/* SERIAL_NO	   NUMBER(10)	*/
	char sPayOrder[8+1];			/* PAY_ORDER	   NUMBER(8)	*/
	char sBeginYmd[8+1];			/* BEGIN_YMD	   CHAR(8)		*/
	char sBeginTm[6+1];				/* BEGIN_TM	       CHAR(6)		*/
	char sEndYmd[8+1];				/* END_YMD		   CHAR(8)		*/
	char sEndTm[6+1];				/* END_TM		   CHAR(6)		*/
	char sLimitPay[14+1];			/* LIMIT_PAY	   NUMBER(14,2)	*/
	char sRateFlag[1+1];			/* RATE_FLAG	   CHAR(1)		*/
	char sStopFlag[1+1];			/* STOP_FLAG	   CHAR(1)		*/
}TdConUserMsgDead;

typedef struct tagdConUserMsgDeadIndex
{
	char sIdNo[14+1];
	char sContractNo[14+1];
	char sSerialNo[10+1];
}TdConUserMsgDeadIndex;

typedef struct tagdCustPresent
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sBelongCode[7+1];
	char sFavCode[20+1];
	char sBunchNo[15+1];
	char sPrepayFee[10+1];
	char sFavMoney[10+1];
	char sMonthFav[10+1];
	char sMonthNum[5+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sFavFee[10+1];
	char sLoginAccept[14+1];
	char sLoginNo[6+1];
	char sOpenTime[17+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
	char sOpCode[4+1];
	char sContractNo[14+1];
	char sChannel[30+1];
	char sPresTotalDate[8+1];
	char sSellCode[4+1];
	char sImeibindMonths[2+1];
	char sOtherphoneNo[15+1];
	char sOtheridNo[14+1];
	char sGroupId[10+1];
	char sFavType[1+1];
	char sActId[50+1];
	char sPackageId[50+1];
}TdCustPresent;

typedef struct tagdGCardStore
{
	char sPassWord[38+1];
	char sCardNo[20+1];
	char sCardFee[5+1];
	char sStartTime[14+1];
	char sExpireTime[17+1];
	char sBelCode[2+1];
	char sCardStatus[1+1];
	char sLoginNo[6+1];
	char sOprCode[2+1];
	char sPhoneNo[15+1];
	char sOpTime[17+1];
	char sUpdateLogin[6+1];
	char sDgcardstore[6+1];
}TdGCardStore;

typedef struct tagdGCardStoreIndex
{
	char sPassWord[38+1];
	char sCardNo[20+1];
}TdGCardStoreIndex;

typedef struct tagdGrpSrvMsgAdd
{
	char sIdNo[14+1];
	char sProductCode[8+1];
	char sAttrCode[2+1];
	char sAttrType[1+1];
	char sAttrValue[256+1];
	char sOpTime[17+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sOpCode[4+1];
	char sLoginNo[6+1];
	char sLoginAccept[14+1];
}TdGrpSrvMsgAdd;

typedef struct tagdGrpSrvMsgAddIndex
{
	char sIdNo[14+1];
	char sProductCode[8+1];
	char sAttrCode[2+1];
}TdGrpSrvMsgAddIndex;

typedef struct tagdMotiveUserMsg
{
	char sCustId[14+1];
	char sMotiveCode[8+1];
	char sMotiveName[128+1];
	char sParentId[14+1];
	char sParentContract[14+1];
	char sIdNo[14+1];
	char sContractNo[14+1];
	char sProductCode[10+1];
	char sProductFlag[1+1];
	char sBeginDate[17+1];
	char sEndDate[17+1];
	char sMotiveStatus[1+1];
	char sLoginAccept[14+1];
	char sLoginNo[6+1];
	char sModeCode[8+1];
}TdMotiveUserMsg;

typedef struct tagdMotiveUserMsgIndex
{
	char sCustId[14+1];
	char sMotiveCode[8+1];
	char sParentId[14+1];
	char sIdNo[14+1];
	char sBeginDate[17+1];
}TdMotiveUserMsgIndex;

typedef struct tagdTermSellMsg
{
	char sPhoneNo[15+1];
	char sIdNo[14+1];
	char sBelongCode[7+1];
	char sImei[15+1];
	char sMachCode[128+1];
	char sSmCode[2+1];
	char sGradeCode[2+1];
	char sInnetTime[17+1];
	char sSupplyShop[80+1];
	char sBusinessShop[80+1];
	char sProductFee[18+1];
	char sProductPayed[18+1];
	char sCastFee[18+1];
	char sBusinPrepay[18+1];
	char sFavMoney[18+1];
	char sShouldFee[18+1];
	char sPayedFee[18+1];
	char sPromptFee[18+1];
	char sPlanFee[18+1];
	char sPlanMonths[2+1];
	char sMarkProduct[18+1];
	char sProductUsed[10+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sOpTime[17+1];
	char sLoginAccept[14+1];
	char sContractNo[14+1];
	char sPlanFlag[1+1];
	char sModeCode[8+1];
	char sImpowerFlag[1+1];
	char sCostFlag[1+1];
	char sObjectFlag[1+1];
	char sGrantAccept[14+1];
	char sCustplanFlag[1+1];
	char sCustplanFee[16+1];
	char sCustplanMonths[14+1];
	char sRepeatlimitMonths[2+1];
	char sImeibindMonths[2+1];
	char sSellChntype[1+1];
	char sSellcardType[2+1];
	char sOtherphoneNo[15+1];
	char sOtheridNo[14+1];
/*生产库表结构*/
	char sOrgId[10+1];
	char sGroupId[10+1];
	char sSellType[1+1];
	char sMachType[2+1];
	char sCollectFlag[1+1];
	char sGoodForProduct[18+1];
	char sGoodForPrepay[18+1];
/*测试库表结构*/
/*	char sBankFee[18+1];
	char sBankNo[30+1];
	char sInstallmentNum[4+1];
	char sGroupId[10+1];
	char sOrgId[10+1];*/
}TdTermSellMsg;

typedef struct tagdCCardStore
{
	char sPassWord[38+1];
	char sCardNo[20+1];
	char sCardFee[5+1];
	char sStartTime[14+1];
	char sExpireTime[17+1];
	char sBelCode[2+1];
	char sCardStatus[1+1];
	char sLoginNo[6+1];
	char sOprCode[2+1];
	char sPhoneNo[15+1];
	char sOpTime[17+1];
	char sUpdateLogin[6+1];
}TdCCardStore;

typedef struct tagdCCardStoreIndex
{
	char sPassWord[38+1];
	char sCardNo[20+1];
}TdCCardStoreIndex;

typedef struct tagwLocalBlackList
{
	char sBlackNo[20+1];
	char sBlackType[1+1];
	char sBelongCode[7+1];
	char sOpTime[17+1];
	char sOrgCode[9+1];
	char sLoginNo[6+1];
	char sOpCode[4+1];
	char sOpNote[60+1];
	char sOrgId[10+1];
	char sGroupId[10+1];
}TwLocalBlackList;

typedef struct tagwLocalBlackListIndex
{
	char sBlackNo[20+1];
	char sBlackType[1+1];
}TwLocalBlackListIndex;

typedef struct tagwLocalBlackListHis
{
	char sBlackNo[20+1];
	char sBlackType[1+1];
	char sBelongCode[7+1];
	char sOpTime[17+1];
	char sOrgCode[9+1];
	char sLoginNo[6+1];
	char sOpCode[4+1];
	char sOpNote[60+1];
	char sLoginAccept[14+1];
	char sOrgId[10+1];
	char sGroupId[10+1];
}TwLocalBlackListHis;

typedef struct tagwLocalBlackListHisIndex
{
	char sBlackNo[20+1];
	char sBlackType[1+1];
	char sLoginAccept[14+1];
}TwLocalBlackListHisIndex;

typedef struct tagdCheckPrepay
{
	char sBankCode[5+1];
	char sCheckNo[20+1];
	char sCheckPrepay[18+1];
	char sOpTime[17+1];
}TdCheckPrepay;

typedef struct tagdCheckPrepayIndex
{
	char sBankCode[5+1];
	char sCheckNo[20+1];
}TdCheckPrepayIndex;

typedef struct tagdCustType
{
	char sIdNo[14+1];
	char sMainCode[2+1];
	char sMainNote[60+1];
}TdCustType;

typedef struct tagdCustTypeIndex
{
	char sIdNo[14+1];
	char sMainCode[2+1];
}TdCustTypeIndex;

typedef struct tagdBillLimit
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sContractNo[14+1];
	char sOpTime[17+1];
}TdBillLimit;

typedef struct tagdBillLimitIndex
{
	char sPhoneNo[15+1];
	char sContractNo[14+1];
}TdBillLimitIndex;

typedef struct tagSQSP_APPLY_REC
{
	char sApplyNo[14+1];
	char sPhoneNo[11+1];
	char sApplyWorkId[6+1];
	char sApplyTime[17+1];
	char sFuncCode[5+1];
	char sProcessStatus[1+1];
	char sOrgCode[7+1];
	char sOrgId[10+1];
}T_SQSP_APPLY_REC;

typedef struct tagdConMarkPreYYYYMM
{
	char sContractNo[14+1];
	char sDetailCode[10+1];
	char sBillYm[6+1];
	char sTotalMark[15+1];
	char sAvailableMark[15+1];
	char sOpTime[17+1];
}TdConMarkPreYYYYMM;

typedef struct tagdConMarkPreYYYYMMIndex
{
	char sContractNo[14+1];
	char sDetailCode[10+1];
}TdConMarkPreYYYYMMIndex;

typedef struct tagdMarkChangeDetailYYYYMM
{
	char sOpAccept[14+1];
	char sContractNo[14+1];
	char sDetailCode[10+1];
	char sBillYm[6+1];
	char sChangeSeq[6+1];
	char sChangetype[2+1];
	char sPreDetailValue[15+1];
	char sPreTotalValue[15+1];
	char sChangeValue[15+1];
	char sAfterDetailValue[15+1];
	char sAfterTotalValue[15+1];
	char sLoginNo[6+1];
	char sOpTime[17+1];
	char sBackFlag[1+1];
	char sOpCode[4+1];
	char sSmCode[2+1];
}TdMarkChangeDetailYYYYMM;

typedef struct tagdMarkChangeDetailYYYYMMIndex
{
	char sOpAccept[14+1];
	char sContractNo[14+1];
	char sDetailCode[10+1];
	char sBillYm[6+1];
	char sChangeSeq[6+1];
}TdMarkChangeDetailYYYYMMIndex;

typedef struct tagdCustSignMsg
{
	char sPhoneNo[15+1];
	char sIdNo[14+1];
}TdCustSignMsg;

typedef struct tagdTransBind
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sTownCode[7+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sOpTime[17+1];
	char sLoginAccept[14+1];
	char sOrgId[10+1];
	char sGroupId[10+1];
}TdTransBind;

typedef struct tagdTransBindIndex
{
	char sIdNo[14+1];
	char sOrgCode[9+1];
}TdTransBindIndex;

typedef struct tagwMarkExchangeOprYYYYMM
{
	char sLoginAccept[14+1];
	char sContractNo[14+1];
	char sGoodsType[4+1];
	char sIdgoodsNo[20+1];
	char sVolume[10+1];
	char sConsumeMark[15+1];
	char sFavourMark[15+1];
	char sSmCode[2+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sOrgCode[9+1];
	char sLoginNo[6+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
	char sSystemNote[60+1];
	char sOpNote[60+1];
	char sIpAddr[15+1];
	char sOrgId[10+1];
	char sBackFlag[1+1];
	char sOpCode[4+1];
}TwMarkExchangeOprYYYYMM;

typedef struct tagwMarkExchangeOprYYYYMMIndex
{
	char sLoginAccept[14+1];
	char sContractNo[14+1];
	char sGoodsType[4+1];
}TwMarkExchangeOprYYYYMMIndex;

typedef struct tagdCustFuncAdd
{
	char sIdNo[14+1];
	char sFunctionCode[2+1];
	char sFunctionType[1+1];
	char sBeginTime[17+1];
	char sBillTime[17+1];
	char sFavourMonth[5+1];
	char sAddNo[15+1];
	char sOtherAttr[40+1];
}TdCustFuncAdd;

typedef struct tagdCustFuncAddIndex
{
	char sIdNo[14+1];
	char sFunctionCode[2+1];
	char sFunctionType[1+1];
}TdCustFuncAddIndex;

typedef struct tagdCustFuncAddHis
{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
	char sBeginTime[17+1];
	char sBillTime[17+1];
	char sFavourMonth[5+1];
	char sAddNo[15+1];
	char sOtherAttr[40+1];
	char sUpdateLogin[6+1];
	char sUpdateAccept[14+1];
	char sUpdateDate[8+1];
	char sUpdateTime[17+1];
	char sUpdateCode[4+1];
	char sUpdateType[1+1];
}TdCustFuncAddHis;

typedef struct tagdCustFuncAddHisIndex
{
	char sIdNo[14+1];
	char sFunctionType[1+1];
	char sFunctionCode[2+1];
	char sUpdateAccept[14+1];
	char sUpdateDate[8+1];
	char sUpdateType[1+1];
}TdCustFuncAddHisIndex;

typedef struct tagdCustPost
{
	char sContractNo[14+1];
	char sPostFlag[1+1];
	char sPostType[1+1];
	char sPostAddress[200+1];
	char sPostZip[6+1];
	char sFaxNo[30+1];
	char sMailAddress[30+1];
	char sPostContCode[1+1];
	char sOpTime[17+1];
}TdCustPost;

typedef struct tagdCustTestCard
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sBelongCode[7+1];
	char sLimitOwe[16+1];
	char sBeginDate[17+1];
	char sExpireDate[17+1];
	char sStopFlag[1+1];
	char sOpTime[17+1];
	char sGroupId[10+1];
}TdCustTestCard;

typedef struct tagdDesignLogin
{
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sOrgId[10+1];
}TdDesignLogin;

typedef struct tagdGotobCustMsg
{
	char sIdNo[14+1];
	char sCustId[10+1];
	char sContractNo[14+1];
	char sIds[4+1];
	char sPhoneNo[15+1];
	char sSmCode[2+1];
	char sAttrCode[8+1];
	char sOpenTime[17+1];
	char sBelongCode[7+1];
	char sRunCode[2+1];
	char sRunTime[17+1];
	char sLoginNo[6+1];
	char sOpCode[4+1];
	char sOpTime[17+1];
	char sOpDate[8+1];
	char sDealFlag[1+1];
	char sRegionCode[2+1];
	char sGroupId[10+1];
}TdGotobCustMsg;

typedef struct tagdGrpPost
{
	char sCustId[10+1];
	char sPostFlag[1+1];
	char sPostType[1+1];
	char sPostAddress[200+1];
	char sPostZip[10+1];
	char sFaxNo[30+1];
	char sMailAddress[30+1];
	char sContactTel[15+1];
	char sPostName[60+1];
	char sPostnoFlag[1+1];
	char sOpTime[17+1];
}TdGrpPost;

typedef struct tagdGrpPostIndex
{
	char sCustId[10+1];
	char sPostType[1+1];
}TdGrpPostIndex;

typedef struct tagdNetCardMsg
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sBelongCode[7+1];
	char sGroupId[10+1];
	char sUsimPhoneno[15+1];
	char sUsimRuncode[2+1];
	char sModeId[32+1];
	char sModeCode[8+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sOrgId[10+1];
	char sOpTime[17+1];
	char sTotalDate[8+1];
	char sLoginAccept[32+1];
	char sLastLoginno[6+1];
	char sOpCode[4+1];
}TdNetCardMsg;

typedef struct tagdUserMail
{
	char sIdNo[14+1];
	char sPostFlag[1+1];
	char sPostAddress[200+1];
	char sPostZip[10+1];
	char sContactTel[14+1];
	char sPostName[60+1];
	char sBizcode[21+1];
}TdUserMail;

typedef struct tagdUserMailIndex
{
	char sIdNo[14+1];
	char sBizcode[21+1];
}TdUserMailIndex;

typedef struct tagdSmAwakeBlackMsg
{
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sContractNo[14+1];
	char sBlackAwake[2+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sLoginAccept[14+1];
	char sAwakeCode[2+1];
	char sAwakeDetail[2+1];
	char sOrgId[10+1];
}TdSmAwakeBlackMsg;

typedef struct tagdSmAwakeBlackMsgIndex
{
	char sIdNo[14+1];
	char sBlackAwake[2+1];
	char sAwakeCode[2+1];
	char sAwakeDetail[2+1];
}TdSmAwakeBlackMsgIndex;

typedef struct tagdGrpManagerMsg
{
	char sServiceNo[6+1];
	char sOrgCode[10+1];
	char sLoginType[1+1];
	char sBossOrgCode[13+1];
	char sName[30+1];
	char sDuty[30+1];
	char sIdType[2+1];
	char sSex[1+1];
	char sIdentifyNo[20+1];
	char sBelongRegion[10+1];
	char sBirthday[10+1];
	char sPolity[2+1];
	char sProfession[50+1];
	char sJoinDate[10+1];
	char sPhoneNo[15+1];
	char sEmail[30+1];
	char sBelongDept[32+1];
	char sEduLevelid[2+1];
	char sSpeciality[50+1];
	char sHomeCall[12+1];
	char sHomeAddress[80+1];
	char sManagerLevel[2+1];
	char sInterest[100+1];
	char sCreateDate[10+1];
	char sUpdateDate[10+1];
	char sUpServiceNo[6+1];
	char sSystemType[1+1];
	char sAge[10+1];
	char sNativeplace[10+1];
	char sJobAge[10+1];
	char sTrainStatus[80+1];
	char sMarrowName[20+1];
	char sMarrowJob[40+1];
	char sMarrowPosition[20+1];
	char sGraTime[10+1];
	char sGraAcademy[40+1];
	char sCharacter[100+1];
	char sMarry[2+1];
	char sCompany[2+1];
	char sStationId[2+1];
	char sStationLevel[2+1];
	char sStationChange[2+1];
}TdGrpManagerMsg;

typedef struct tagdObspbizInfoMsg
{
	char sSpid[12+1];
	char sBizcode[21+1];
	char sBiztype[32+1];
	char sBizdesc[256+1];
	char sAccessmodel[32+1];
	char sPrice[12+1];
	char sBillingtype[32+1];
	char sBizstatus[1+1];
	char sProvaddr[128+1];
	char sProvport[6+1];
	char sUsagedesc[512+1];
	char sIntrourl[128+1];
	char sLocalcode[1+1];
	char sServattr[2+1];
	char sServtype[5+1];
	char sServname[64+1];
	char sSubmethod[5+1];
	char sIcpname[100+1];
	char sServidalias[12+1];
	char sIcpshortname[12+1];
	char sName[40+1];
	char sFixedline[20+1];
}TdObspbizInfoMsg;

typedef struct tagdObspbizInfoMsgIndex
{
	char sSpid[12+1];
	char sBizcode[21+1];
}TdObspbizInfoMsgIndex;

typedef struct tagdGrpCustMsg
{
	char sCustId[22+1];
	char sPartyId[14+1];
	char sUnitId[14+1];
	char sCustName[80+1];
	char sTradeCode[10+1];
	char sBankName[60+1];
	char sBankAccountsCode[30+1];
	char sUnitAddr[100+1];
	char sUnitZip[10+1];
	char sContactPerson[20+1];
	char sContactPhone[60+1];
	char sContactMobilePhone[15+1];
	char sFax[60+1];
	char sWeb[60+1];
	char sUnitLeader[20+1];
	char sRunArea[100+1];
	char sEnrolFee[24+1];
	char sYearTurnover[24+1];
	char sYearGain[24+1];
	char sTopRepreName[20+1];
	char sTopReprePhone[20+1];
	char sUnitCreateDay[17+1];
	char sLicenseNumber[30+1];
	char sInstitutionType[10+1];
	char sLicenseValidate[17+1];
	char sEntType[5+1];
	char sVpmnFlag[2+1];
	char sStatusCode[5+1];
	char sAreaType[5+1];
	char sProvinceCode[15+1];
	char sLocalNet[15+1];
	char sAreaCode[15+1];
	char sOwnerCode[5+1];
	char sValidFlag[5+1];
	char sCreateLogin[20+1];
	char sCreateDate[17+1];
	char sLeaveDate[17+1];
	char sRevokeDate[17+1];
	char sJionDate[17+1];
	char sGroupId[14+1];
	char sServiceNo[20+1];
	char sPayType[5+1];
	char sComeChannelCode[5+1];
	char sServiceChannel[5+1];
	char sEnterPro[20+1];
	char sInformationFlag[2+1];
	char sNationUnitCode[20+1];
	char sUniPayNo[30+1];
	char sPhotoCode[2+1];
	char sUnitPhoto[1+1];
	char sBossPrivilege[1+1];
	char sGradeCode[5+1];
	char sNote[255+1];
	char sUpdateLoginNo[20+1];
	char sUpdateDate[17+1];
	char sOprtType[2+1];
	char sOperType[2+1];
	char sOperDate[17+1];
	char sBak1[500+1];
	char sBak2[500+1];
	char sBak3[500+1];
	char sBak4[500+1];
	char sBak5[500+1];
	char sOrgId[10+1];
	char sModi_type[1+1];
	char sModi_date[10+1];
}TdGrpCustMsg;

typedef struct tagDIAGWSERVOPER
{
	char sServCode[21+1];
	char sOperCode[10+1];
	char sMsisdn[13+1];
	char sFee[12+1];
	char sStartTime[17+1];
	char sEndTime[17+1];
	char sBillType[2+1];
	char sFeeObject[1+1];
}TDIAGWSERVOPER;

typedef struct tagDIAGWSERVOPERindex
{
	char sServCode[21+1];
	char sOperCode[10+1];
	char sMsisdn[13+1];
	char sFee[12+1];
	char sStartTime[17+1];
}TDIAGWSERVOPERIndex;

typedef struct tagDIAGWMOSERVOPER
{
	char sServCode[21+1];
	char sOperCode[10+1];
	char sFee[12+1];
	char sStartTime[17+1];
	char sEndTime[17+1];
	char sFeeFlag[1+1];
}TDIAGWMOSERVOPER;

typedef struct tagDIAGWMOSERVOPERIndex
{
	char sServCode[21+1];
	char sOperCode[10+1];
	char sStartTime[17+1];
}TDIAGWMOSERVOPERIndex;

typedef struct tagdLineSwitch
{
	char sSwitchNo[10+1];
	char sRepeatersNo[16+1];
	char sLongCode[4+1];
	char sFlag[1+1];
	char sUserId[11+1];
	char sFeeCode[4+1];
	char sCfeeDiscount[3+1];
	char sLfeeDiscount[3+1];
	char sStartDatetime[17+1];
	char sEndDatetime[17+1];
}TdLineSwitch;

typedef struct tagdLineSwitchIndex
{
	char sSwitchNo[10+1];
	char sRepeatersNo[16+1];
	char sUserId[11+1];
	char sStartDatetime[17+1];
}TdLineSwitchIndex;

typedef struct tagwPhoneImsi
{
	char sIdCode[22+1];
	char sPhoneNo[15+1];
	char sImsiNo[20+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
}TwPhoneImsi;

typedef struct tagdGprsReduce
{
	char sPhoneNo[15+1];
	char sTotalMonth[6+1];
	char sLoginNo[8+1];
	char sOpTime[17+1];
}TdGprsReduce;

typedef struct tagdGprsReduceIndex
{
	char sPhoneNo[15+1];
	char sTotalMonth[6+1];
}TdGprsReduceIndex;

typedef struct tagdCardRes
{
	char sRegionCode[2+1];
	char sDistrictCode[2+1];
	char sTownCode[3+1];
	char sCardStatus[1+1];
	char sCartType[2+1];
	char sKind[2+1];
	char sSellunit[2+1];
	char sInPrice[17+1];
	char sOutPrice[17+1];
	char sSellDate[8+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sLoginAccept[14+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
	char sTollNo[12+1];
	char sEndNo[30+1];
	char sBeginNo[30+1];
	char sGroupId[10+1];
	char sOrgId[10+1];
}TdCardRes;

typedef struct tagdCardResIndex
{
	char sRegionCode[2+1];
	char sCartType[2+1];
	char sBeginNo[30+1];
	char sEndNo[30+1];
}TdCardResIndex;

typedef struct tagRebackPoint
{
	char sOutputCode[4+1];
	char sLongCode[4+1];
	char sUserInfo[5+1];
	char sRecType[2+1];
	char sSegnum[22+1];
	char sCdrType[2+1];
	char sDepositType[2+1];
	char sChargeDn[11+1];
	char sUserType[1+1];
	char sUserAttr[1+1];
	char sMsisdnRec[11+1];
	char sOprType[2+1];
	char sSrcType[2+1];
	char sDevCode[7+1];
	char sSpCode[20+1];
	char sOperCode[20+1];
	char sContentCode[12+1];
	char sIngressIp[15+1];
	char sVisitProv[3+1];
	char sStartTime[14+1];
	char sStopTime[14+1];
	char sDnloadDur[9+1];
	char sValidTimes[9+1];
	char sOnlineDur[9+1];
	char sDataSize[15+1];
	char sBillingType[2+1];
	char sInfoFee[10+1];
	char sDisInfoFee[10+1];
	char sDiscount[3+1];
	char sCarryType[1+1];
	char sStatus[1+1];
	char sPriority[1+1];
	char sExchangeRate[6+1];
	char sPointCost[10+1];
	char sChannelId[8+1];
	char sTranId[14+1];
	char sChargeupNum[12+1];
	char sReserved[3+1];
	char sDealDate[14+1];
	char sDealFlag[1+1];
	char sFileName[17+1];
	char sPartitionId[1+1];
	char sOpTime[17+1];
	char sLoginNo[6+1];
	char sLoginAccept[14+1];
}TRebackPoint;

typedef struct tagRebackFee
{
	char sOutputCode[4+1];
	char sLongCode[4+1];
	char sUserInfo[5+1];
	char sSegnum[22+1];
	char sCdrType[2+1];
	char sDepositType[2+1];
	char sMsisdn[21+1];
	char sUserType[2+1];
	char sUserAttr[2+1];
	char sOtherParty[24+1];
	char sSrcType[2+1];
	char sDevCode[7+1];
	char sSpCode[20+1];
	char sOperCode[20+1];
	char sIngressIp[15+1];
	char sVisitProv[3+1];
	char sStartTime[14+1];
	char sEndDatetime[14+1];
	char sDnloadDur[6+1];
	char sValidTimes[3+1];
	char sOnlineDur[4+1];
	char sDataSize[10+1];
	char sBillingType[2+1];
	char sMsgFee[10+1];
	char sOldMsgFee[10+1];
	char sDiscount[3+1];
	char sCarryType[1+1];
	char sStatus[1+1];
	char sPriority[1+1];
	char sExchangeRate[5+1];
	char sPointCost[10+1];
	char sChannelId[8+1];
	char sContentCode[12+1];
	char sChargeupNum[12+1];
	char sReserved[20+1];
	char sDealDate[14+1];
	char sDealFlag[5+1];
	char sFileName[40+1];
	char sPartitionId[5+1];
	char sMonthFee[10+1];
	char sOpTime[17+1];
	char sLoginNo[6+1];
	char sLoginAccept[14+1];
}TRebackFee;

typedef struct tagwPhonePayMark
{
	char sTotalDate[8+1];
	char sLoginAccept[14+1];
	char sDealType[4+1];
	char sOpCode[4+1];
	char sPhoneNo[15+1];
	char sTotalPoint[15+1];
	char sChnType[4+1];
	char sOpTime[17+1];
	char sBackFlag[1+1];
	char sDealTime[14+1];
}TwPhonePayMark;

typedef struct tagdMobileInfoMsg
{
	char sMsisdn[15+1];
	char sSwitchType[4+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sIdNo[14+1];
	char sOpTime[17+1];
	char sLoginAccept[14+1];
	char sLoginNo[6+1];
	char sOpNote[60+1];
}TdMobileInfoMsg;

typedef struct tagdMobileInfoMsgIndex
{
	char sMsisdn[15+1];
	char sSwitchType[4+1];
	char sBeginTime[17+1];
}TdMobileInfoMsgIndex;

typedef struct tagdAccountIdInfo
{
	char sMsisdn[15+1];
	char sServiceType[2+1];
	char sServiceNo[100+1];
	char sRunCode[2+1];
	char sRunTime[17+1];
	char sInnetTime[17+1];
	char sNote[60+1];
}TdAccountIdInfo;

typedef struct tagwPhoneImsiRent
{
	char sLocalImsi[15+1];
	char sLocalPhone[11+1];
	char sRoamPhone[15+1];
	char sStartDate[17+1];
	char sEndDate[17+1];
	char sFlag[1+1];
	char sOpTime[17+1];
}TwPhoneImsiRent;

typedef struct tagdCustBillQryReq
{
	char sIdNo[14+1];
	char sOpCode[5+1];
	char sQryFlag[1+1];
	char sQryNote[60+1];
}TdCustBillQryReq;

typedef struct tagdCustBillQryReqIndex
{
	char sIdNo[14+1];
	char sOpCode[5+1];
}TdCustBillQryReqIndex;

typedef struct tagdFamilyInfo {
	char sFamilyId[14+1];
  char sCust_id[10+1];
	char sPhoneNo[15+1];
	char sUserNum[14+1];
	char sFamilyIncome[14+1];
	char sOtherProduct[60+1];
	char sHomePayNo[30+1];
	char sPayMoney[14+1];
  char sComputerNum[20+1];
	char sIntranet[60+1];
	char sOtherQuip[60+1];
	char sLoveService[60+1];
	char sApplyFee[14+1];
	char sApplyNum[20+1];
	char sLikeRedound[60+1];
	char sSemaphore[60+1];
  char sAverageFee[14+1];
	char sFinishFlag[1+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];
	char sBak1[12+1];
	char sBak2[12+1];
	char sBak3[12+1];
  char sOpTime[17+1];
	char sLoginAccept[14+1];
	char sConFlag[1+1];

}TdFamilyInfo;

typedef struct tagdFamilyInfoIndex
{
	char sFamilyId[14+1];
}TdFamilyInfoIndex;

typedef struct tagdFamilyProInfo {
	char sFamilyId[14+1];
  	char sFbizCode[10+1];
	char sModeCode[8+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sDetailCode[4+1];
	char sDetailType[1+1];
	char sConFlag[1+1];
  	char sContractNo[14+1];
	char sEffTime[17+1];
	char sExpTime[17+1];
	char sLoginNo[6+1];
	char sLoginAccept[14+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
	char sOpCode[4+1];
	char sPayFlag[1+1];
	char sBak2[12+1];
	char sBak3[12+1];

}TdFamilyProInfo;

typedef struct tagdFamilyProInfoIndex
{
	char sFamilyId[14+1];
  	char sFbizCode[10+1];
  	char sModeCode[8+1];
  	char sIdNo[14+1];
  	char sDetailCode[4+1];
	char sDetailType[1+1];
	char sEffTime[17+1];
	char sExpTime[17+1];
}TdFamilyProInfoIndex;

typedef struct tagdFamilyProMember {
	char sFamilyId[14+1];
  	char sFbizCode[10+1];
	char sMebOrder[10+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sMebType[3+1];
	char sMainFlag[1+1];
	char sEffTime[17+1];
	char sExpTime[17+1];
	char sLoginAccept[14+1];
	char sBak1[12+1];
	char sBak2[12+1];
	char sBak3[12+1];
	char sPointFLag[1+1];

}TdFamilyProMember;

typedef struct tagdFamilyProMemberIndex
{
	char sFamilyId[14+1];
  	char sFbizCode[10+1];
  	char sIdNo[14+1];
  	char sEffTime[17+1];
	char sExpTime[17+1];
}TdFamilyProMemberIndex;

typedef struct tagdFamilyMebShaMod {
	char sFamilyId[14+1];
 	char sFbizCode[10+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sModeCode[8+1];
	char sDetailCode[4+1];
	char sDetailType[1+1];
	char sMainFlag[1+1];
	char sEffTime[17+1];
	char sExpTime[17+1];
	char sOpFlag[1+1];
	char sRegionCode[2+1];
	char sLoginNo[6+1];
	char sLoginAccept[14+1];
	char sTotalDate[8+1];
	char sDealTime[17+1];
	char sOpCode[4+1];
	char sBak1[12+1];
	char sBak2[12+1];
	char sBak3[12+1];
	char sShareFlag[1+1];

}TdFamilyMebShaMod;

typedef struct tagdFamilyMebShaModIndex
{
	char sFamilyId[14+1];
  	char sFbizCode[10+1];
  	char sModeCode[8+1];
  	char sDetailCode[4+1];
	char sDetailType[1+1];
	char sEffTime[17+1];
	char sExpTime[17+1];
}TdFamilyMebShaModIndex;

typedef struct tagdWideLoginMsg {
	char sIdNo[14+1];
	char sPhoneNo[15+1];
  char sCfmLogin[20+1];
	char sBeginTime[17+1];
	char sEndTime[17+1];

}TdWideLoginMsg;

typedef struct tagdWideLoginMsgIndex
{
	char sIdNo[15+1];
  char sCfmLogin[20+1];
  char sBeginTime[17+1];
	char sEndTime[17+1];
}TdWideLoginMsgIndex;

typedef struct tagDApnMsg
{
  char sLoginAccept[14+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sTotalDate[8+1];
	char sLoginNo[6+1];
	char sOrgCode[9+1];
	char sOpType[1+1];
	char sOpTime[17+1];
	char sPdpModel[3+1];
	char sApnCode[20+1];
	char sApnName[50+1];
	char sHlrCode[1+1];
	char sCommandCode[2+1];
	char sApnIp[20+1];
	char sNote[60+1];
	char sOrgId[10+1];
	char sPdpId[3+1];
}TdApnMsg;

typedef struct tagDApnMsgIndex
{
	char sPhoneNo[15+1];
	char sPdpModel[3+1];
}TdApnMsgIndex;

typedef struct tagdGrpProdStop {
	char sIdNo[14+1];
  	char sAccountId[14+1];
	char sProductCode[8+1];
	char sRunCode[2+1];
	char sRunTime[17+1];
	char sLoginAccept[14+1];

}TdGrpProdStop;

typedef struct tagdGrpProdStopIndex
{
	char sIdNo[14+1];
}TdGrpProdStopIndex;

typedef struct tagdUmPayMsg{
        char sPhoneNo[15+1];
        char sCustName[8+1];
        char sRegionName[20+1];
        char sCityName[20+1];
        char sPayName[15+1];
        char sPayAddr[100+1];
        char sIccidId[20+1];
        char sBankId[20+1];
        char sIsHave[10+1];
        char sLoginNo[6+1];
        char sOpTime[17+1];
        char sOpNote[60+1];
        char sLimitFee[16+1];
        char sIsRegister[1+1];
}TdUmPayMsg;

typedef struct tagdUmPayMsgIndex{
        char sPhoneNo[15+1];
}TdUmPayMsgIndex;

typedef struct tagDBankPayCon
{
	char sBankEngName[10 + 1];	/* 银行名英文简写 */
	char sBankName[60 + 1];		/* 银行名称 */
	char sIdNo[14 + 1];		/* 用户ID */
	char sPhoneNo[15 + 1];		/* 手机号码 */
	char sOpCode[4 + 1];		/* 操作代码 */
	char sOpTime[14 + 1];		/* 操作时间 */
	char sTotalDate[8 + 1];		/* 操作日期 */
	char sLoginNo[6 + 1];		/* 操作工号 */
	char sOrgId[10 + 1];		/* 工号归属 */
	char sLoginAccept[14 + 1];	/* 操作流水 */
}TdBankPayCon;

typedef struct tagDBankPayConIndex
{
	char sBankEngName[10 + 1];	/* 银行名英文简写 */
	char sIdNo[14 + 1];		/* 用户ID */
}TdBankPayConIndex;


typedef struct tagDMobilePayCon
{
	char sIdNo[14 + 1];
	char sContractNo[14 + 1];
	char sPhoneNo[15 + 1];
	char sPayFlag[1 + 1];
	char sLimitFee[16 + 1];
	char sUserType[1 + 1];
	char sPayFee[16 + 1];
}TdMobilePayCon;

typedef struct tagdConsumeAwakeMsg
{
	char sAcceptNo[14+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sAwakeType[2+1];
	char sOpCode[4+1];
	char sLoginNo[6+1];
	char sTotalDate[8+1];
	char sOpTime[17+1];
}TdConsumeAwakeMsg;

typedef struct tagdConsumeAwakeMsgIndex
{
	char sIdNo[14+1];
	char sAwakeType[2+1];
}TdConsumeAwakeMsgIndex;

typedef struct tagDOneWapUserLogon
{
        char sIdentCode[32 + 1];	/* 身份凭证 */
        char sIdentEfft[14 + 1];	/* 身份凭证生效时间 */
        char sIdentUnefft[14 + 1];	/* 身份凭证失效时间 */
        char sAuthGrade[2 + 1];		/* 身份认证等级 */
        char sIdType[2 + 1];		/* 用户标识类型 */
        char sIdItemRange[32 + 1];	/* 用户标识 */
        char sWapSessionId[32 + 1];	/* 一级WAPsessionID */
        char sOprNumb[32 + 1];		/* 本次操作流水 */
}TdOneWapUserLogon;

typedef struct tagDOneWapUserLogonIndex
{
        char sIdType[2 + 1];		/* 用户标识类型 */
        char sIdItemRange[32 + 1];	/* 用户标识 */
		char sIdentCode[32 + 1];	/* 身份凭证 */
}TdOneWapUserLogonIndex;

typedef struct tagdMifiAwakeMsg
{
	char sIdNo[14+1];		/*MIFI受理号码ID*/
	char sPhoneNo[15+1];	/*MIFI受理号码*/
	char sCustId[10+1];		/*MIFI受理号码的cust_id*/
	char sGprsPhone[15+1];	/*MIFI的GPRS流量提醒号码*/
	char sModeCode[8+1];	/*MIFI绑定的套餐*/
	char sOpTime[17+1];		/*操作时间*/
	char sBeginTime[17+1];	/*套餐开始时间*/
	char sEndTime[17+1];	/*套餐截止时间*/
}TdMifiAwakeMsg;

typedef struct tagDCustAcctCycleIndex
{
        char sIdNo[14+1];
        char sPhoneNo[15+1];
        char sBeginTime[14+1];
}TdCustAcctCycleIndex;

typedef struct tagDCustAcctCycle
{
        char sIdNo[14+1];
        char sPhoneNo[15+1];
        char sContractNo[14+1];
        char sBeginTime[14+1];
        char sEndTime[14+1];
        char sAcctCycleType[1+1];
        char sCycleBeginDay[2+1];
        char sRegionCode[2+1];
        char sOpNo[14+1];
        char sOpTime[17+1];
}TdCustAcctCycle;

typedef struct tagdDateFav {
	char sMsisdn[15+1];
	char sRegionCode[2+1];
	char sFavDate[8+1];
	char sFavType[4+1];
	char sFlagCode[10+1];
	char sDateMask[8+1];
	char sStartTime[17+1];
	char sEndTime[17+1];
	char sFreeValue[10+1];
	char sIdNo[14+1];
	char sGroupId[10+1];
	char sProductCode[8+1];
}TdDateFav;

typedef struct tagdNetCardSale
{
	char sLoginAccept[14+1];
	char sIdNo[14+1];
	char sMainPhone[15+1];
	char sOpTime[17+1];
	char sTotalDate[8+1];
	char sBindPhone[15+1];
	char sModeCode[8+1];
	char sLoginNo[6+1];
	char sOrgId[10+1];
	char sBeginTime[20+1];
	char sEndTime[20+1];
	char sBindId[14+1];
}TdNetCardSale;

typedef struct tagdNetCardSaleIndex
{
	char sIdNo[14+1];
	char sModeCode[8+1];			/* MODE_CODE		CHAR(8)		*/
	char sLoginAccept[14+1];		/* LOGIN_ACCEPT		NUMBER(14)	*/
}TdNetCardSaleIndex;

typedef struct tagdObTransMsg
{
	char sLoginAccept[14+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sHomeBelong[7+1];
	char sHomeGroup[10+1];
	char sBelongOut[7+1];
	char sGroupOut[10+1];
	char sBelongIn[7+1];
	char sGroupIn[10+1];
	char sLoginNo[6+1];
	char sOrgId[10+1];
	char sOpTime[17+1];
	char sBeginTime[20+1];
	char sEndTime[20+1];
}TdTransMsg;

typedef struct tagdObTransMsgIndex
{
        char sIdNo[14+1];
        char sPhoneNo[15+1];
        char sBeginTime[14+1];
}TdTransMsgIndex;

typedef struct tagdChangePhone
{
	char sLoginAccept[14+1];
	char sOldPhone[15+1];
	char sNewPhone[15+1];
	char sHomePhone[15+1];
	char sHomeId[14+1];
	char sWhhandle[2+1];
	char sChannel[2+1];
	char sIdcardType[2+1];
	char sIdcardNum[20+1];
	char sOprCode[2+1];
	char sOpTime[17+1];
	char sBeginTime[20+1];
	char sEndTime[20+1];
	char sImsiNo[20+1];
	char sMarkTransFlag[1+1];
}TdChangePhone;

typedef struct tagwChangeCardRecord
{
	char sTotalDate[8+1];
	char sLoginAccept[14+1];
	char sOpCode[4+1];
	char sIdNo[14+1];
	char sPhoneNo[15+1];
	char sLoginNo[6+1];
	char sOpTime[17+1];
	char sOrgId[10+1];
}TwChangeCardRecord;


typedef struct tagDGPRSRECORDMSG
{
	char sPhoneNo[15+1];
	char sIdNo[14+1];
	char sBeginTime[17+1];
	char sLoginNo[6+1];
	char sOpTime[17+1];
	char sLoginAccept[14+1];
}TDGPRSRECORDMSG;

typedef struct tagDGPRSRECORDMSGIndex
{
	char sPhoneNo[15+1];
	char sIdNo[14+1];
}TDGPRSRECORDMSGIndex;

typedef struct tagDMPAYSUBPHONE
{
	char sLoginAccept[32 + 1];
	char sIdNo[14 + 1];
	char sContractNo[14 + 1];
	char sPhoneNo[15 + 1];
	char sSubId[14 + 1];
	char sSubContract[14 + 1];
	char sSubPhone[15 + 1];
	char sOpTime[17 + 1];
}TMPaySubPhone;

typedef struct tagdMPAYSUBPHONEIndex
{
	char sPhoneNo[15+1];
	char sSubPhone[15+1];
}TdMpaySubPhoneIndex;

/*聚会好友成员信息*/
typedef struct tagdJHHYGroupMebMsg
{
	char sGroupId[14+1];
	char sMemberNo[15+1];
	char sMemberId[14+1];
	char sMemberType[1+1];
  	char sProvinceFlag[1+1];
  	char sModeCode[8+1];
  	char sRspNo[14+1];
  	char sFbizCode[10+1];
  	char sMainFlag[1+1];
  	char sEffTime[17+1];
  	char sExpTime[17+1];
}TdJHHYGroupMebMsg;

typedef struct tagdJHHYGroupMebMsgIndex
{
	char sGroupId[14+1];
  	char sMemberNo[15+1];
}TdJHHYGroupMebMsgIndex;
#endif /*__PUBORDER_H__*/
