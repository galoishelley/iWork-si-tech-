/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/

/***************************************
 *function  ���Ŵ����ú���
 *author    yangzh
 *created   2004-11-08 15:43
****************************************/

/*******�������֯�ṹ���ڸ��� edit by  liuhao 07/05/2009 <sXXXXCfm(opCode1,opCode3....)> ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "publicsrv.h"

/*����ҵ�����ȡ����sSmProduct���sm_code*/
#if PROVINCE_RUN == PROVINCE_JILIN
	#define VPMN_SM_CODE           "vp" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
	#define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_IP_SM_CODE      "04" /*IPר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_DIANLU_SM_CODE  "13" /*��·����ҵ���Ʒ�ƴ���*/
	#define ADC_SM_CODE  "44" 	/*ADC*/
#else
	#define VPMN_SM_CODE           "vp" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
	#define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_IP_SM_CODE      "ip" /*IPר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
	#define ZHUANX_DIANLU_SM_CODE  "dl" /*��·����ҵ���Ʒ�ƴ���*/
#endif

EXEC SQL INCLUDE SQLCA;


/**
 *     �õ������û��ⲿ���롣
 * @author      lugz
 * @version %I%, %G%
 * @since 1.00
 * @inparam     regionCode      ���д���
 * @inparam     smCode  ҵ�����
 * @outparam grpUserNo  �����û�����
 * @return ����0��ʾ��ȷ���������
 */
int GetGrpUserNo(const char *regionCode, const char *smCode, char* grpUserNo, char *retMsg)
{
		char            tempBuf[100];

		short in;
		char dynStmt[1024];
		char seqFlag[1+1];
		char codePrefix[10+1];
		char codeSuffix[10+1];
		char seqName[100+1];
		char seqNameExt[2+1];
		char seqFullName[127+1];

		memset(seqFlag, 0, sizeof(seqFlag));
		memset(codePrefix, 0, sizeof(codePrefix));
		memset(codeSuffix, 0, sizeof(codeSuffix));
		memset(seqName, 0, sizeof(seqName));
		memset(seqNameExt, 0, sizeof(seqNameExt));

		EXEC SQL        SELECT SEQ_FLAG,CODE_PREFIX,CODE_SUFFIX,SEQ_NAME,SEQ_NAME_EXT
								  INTO :seqFlag,:codePrefix indicator :in,:codeSuffix indicator :in,
									   :seqName indicator :in, :seqNameExt  indicator :in
								  FROM sGrpSmCode
								 WHERE sm_code = :smCode
								   AND seq_flag = 'Y';
		Trim(seqFlag);
		Trim(codePrefix);
		Trim(codeSuffix);
		Trim(seqName);
		Trim(seqNameExt);
		if (SQLCODE !=0 || seqFlag[0] == '\0')
		{
				sprintf(retMsg, "û�м����ⲿ��������¼,SQLCODE=[%d]",SQLCODE);
				PUBLOG_DBDEBUG("GetGrpUserNo");
				pubLog_DBErr(_FFL_,"GetGrpUserNo","001000",retMsg);
				return 1000;
		}

		memset(seqFullName, 0, sizeof(seqFullName));
		strcpy(seqFullName, seqName);

		if (strncmp(seqNameExt, "10", 2) == 0)
		{
				/*�����н�ORACLE����*/
				strcat(seqFullName, regionCode);
		}
		else
		{
				/*������ORACLE���еĹ���*/
		}

		memset(dynStmt, 0, sizeof(dynStmt));
		sprintf(dynStmt,        "SELECT %s.nextval"
												"  FROM dual",
												seqFullName);

		memset(tempBuf, 0, sizeof(tempBuf));
		EXEC SQL EXECUTE
		BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :tempBuf;
		END;
		END-EXEC;
		Trim(tempBuf);

		if (SQLCODE !=0 || tempBuf[0] == '\0')
		{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "GetGrpUserNo", "", "dynStmt=[%s]", dynStmt);
#endif
				strcpy(retMsg, "�õ�����ֵ����");
				PUBLOG_DBDEBUG("GetGrpUserNo");
				pubLog_DBErr(_FFL_,"GetGrpUserNo","001001",retMsg);
				return 1001;
		}

		pubLog_Debug(_FFL_, "GetGrpUserNo", "", "dynStmt=[%s]", dynStmt);
		pubLog_Debug(_FFL_, "GetGrpUserNo", "", "tempBuf=[%s]", tempBuf);

		strcpy(grpUserNo, codePrefix);
		strcat(grpUserNo, tempBuf);
		strcat(grpUserNo, codeSuffix);
		Trim(grpUserNo);
		pubLog_Debug(_FFL_, "GetGrpUserNo", "", "grpUserNo=[%s]", grpUserNo);

		return 0;
}

/************************************************************************
��������:fpubCreateGrpUser
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û��������ú���
�������:�����û���Ϣ
		 ��Ա�ֻ�����
		 ����Ա����
		 ��������
		 ��������
		 ������ˮ��
		 ������ע
ng ���� ҵ�񹤵�����  �����������
		 ������Ϣ��
		 ϵͳ��ע
�������:�������
�� �� ֵ:0������ȷ����,��������

����ʱ��:2006/02/17
��������:���ӻ�����Ϣ��dMarkMsg
************************************************************************/
int fpubCreateGrpUser(FBFR32 *transIN, int ProdIndx, int SrvIndx, tGrpUserMsg *grpUserMsg,
	char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *feeList, char *systemNote)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char    vCombFlag[1+1];
		char	vSubProductCode[8+1];
		tUserDoc  userDoc;     /*�û������ṹ��*/
		tAccDoc   accDoc;     /*�ʻ������ṹ��*/

		int     rowNum = 0;
		char    vCust_Id[14+1];         /* ���ſͻ�ID   */
		char    vGrp_Id[14+1];          /* �����û�ID   */
		char    vGrp_No[100+1];          /* �����û����,�ⲿ���� */
		char    vGrp_UserNo[15+1];     /* �����û����,�ڲ����� */
		char    vGrp_Name[60+1];        /* �û�����     */
		char    serviceType[2+1];
		char    vSm_Code[2+1];          /* ��ƷƷ��     */
		char    vProduct_Type[8+1];     /* ��Ʒ����     */
		char    vProduct_Code[8+1];     /* ��Ʒ����     */
		char    vOperation_Code[2+1];   /* GRP����ҵ������*/
		char    vGrpProduct_Code[2+1];  /* GRP���ŵĲ�Ʒ����*/
		char    vUser_Passwd[8+1];      /* ҵ���������� */
		char    vProvince[2+1];         /* ����ʡ       */
		char    vRegion_Code[2+1];      /* ��������     */
		char    vDistrict_Code[2+1];    /* ������       */
		char    vTown_Code[3+1];        /* ����Ӫҵ��   */
		char    vTerritory_Code[20+1];  /* ���ش���     */
		char    vCredit_Code[2+1];      /* ���õȼ�     */
		char    vCredit_Value[18+1];    /* ���ö�       */
		char    vBill_Type[4+1];        /* �������ڴ��� */
		char    vOp_Time[20+1];         /* ����ʱ��     */
		char    vSrv_Stop[10+1];        /* ҵ��������� */
		char    vGroup_Id[10+1];        /* ��Դ��group_id*/
		char    vOrg_Id[10+1];          /* ��Դ��org_id */
		char    vBak_Field[16+1];       /* Ԥ���ֶ�     */
		char    vGrp_Flag[1+1];         /* ���Ų�Ʒ��־ */
		char    vPay_Code[1+1];         /* �����ʻ����ѷ�ʽ */
		char    payNoId[22+1];
		char    payNo[15+1];
		char  	vProduct_Attr[8+1];
		char  	vProduct_Price[40+1];
		char  	vService_Attr[40+1];
		int 	ret=0;
		char  	vOrgCode[9+1];
		int   i, inx, retCode, vCount = 0;
		int   isBillRowNum=0;         /* �Ƿ���ڼƷ��¼� */
		int   vPower_right = 0;
		int   vFav_Order = 0;
		long  lLoginAccept;
		char  strLoginAccept[14+1];
		char  vBelong_Code[7+1];
		char  vLogin_No[6+1];
		char  vOp_Code[4+1];
		char  vOp_Note[60+1];
		char  dynStmt[2048];
		char  vTotal_Date[8+1];      /* ��������     */
		char  vDateChar[6+1];        /* ��������     */
		char  vUnit_Id[22+1];        /* ����ID       */
		char  vUnit_Code[22+1];      /* ���Ŵ���     */
		char  vAccount_Id[14+1];     /* �ʻ�ID       */
		char  vAccount_Passwd[8+1];  /* �ʻ�����     */
		char  vContract_Id[22+1];    /* ���ź�ͬID   */
		char  vContract_No[22+1];    /* ���ź�ͬ���� */
		char  vProduct_Id[22+1];     /* ��ƷID       */
		char  vOrder_GoodsId[22+1];  /* ����ID       */
		char  vProduct_List[1536];   /* ��Ʒ�����б� */
		char  vProduct_All[1536];    /* ���Ӳ�Ʒ�б� */
		char  sqlStr[3072];
		char  vService_Type[1+1];
		char  vService_Code[4+1];
		char  vMain_Flag[1+1];
		char  vPrice_Code[4+1];
		char  vVPMN_Flag[1+1];
		char  vService_No[6+1];
		char  personFlag[1+1];
		int	  favcount=0;
		char  favbizCode[10+1];
		int	  favMonth=0;

		/* ng����  ҵ�񹤵�����  by liuzhou 20090930*/
		char  payType[4+1];
		char  checkNo[20+1];
		char  bankCode[5+1];
		char  checkPay[12+1];
		char  cashPay[12+1];
		char  shouldHandFee[12+1];
		char  handFee[12+1];
		char  sChkFlag[1+1];
		char  sFeeCodeList[3072];
		char  sDetailCodeList[3072];
		char  sRealFeeList[3072];
		/* ng����  ҵ�񹤵�����  by liuzhou 20090930*/
	EXEC SQL END DECLARE SECTION;

	/*ng ���� ҵ�񹤵�����*/
	char  vCallServiceName[30+1];
	int   vSendParaNum = 0;
	int   vRecpParaNum = 0;
	int   nFeeCount = 0;
	char  vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA];
	const int iOrderRight = 100;
	init(vCallServiceName);

	/*ng���� by yaoxc begin*/
	int  v_ret=0;
	TdConUserMsg tdConUserMsg;
	TdCustConMsg tdCustConMsg;
	TdBaseFav tdBaseFav;
	TdGrpUserMsg tdGrpUserMsg;
	TdAccountIdInfo tdAccountIdInfo;
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	char sBeginTm[6+1];
	/*ng���� by yaoxc end*/

	/*ng���� by yaoxc begin*/
	memset(&tdConUserMsg,0,sizeof(tdConUserMsg));
	memset(&tdCustConMsg,0,sizeof(tdCustConMsg));
	memset(&tdBaseFav,0,sizeof(tdBaseFav));
	memset(&tdGrpUserMsg,0,sizeof(tdGrpUserMsg));
	memset(&tdAccountIdInfo,0,sizeof(tdAccountIdInfo));
	init(v_parameter_array);
	init(sTempSqlStr);
	init(sBeginTm);
	/*ng���� by yaoxc end*/

	/*ng ����  ҵ�񹤵����� by liuzhou 20090930*/
	init(payType);
	init(checkNo);
	init(bankCode);
	init(checkPay);
	init(cashPay);
	init(shouldHandFee);
	init(handFee);
	init(sChkFlag);
	strcpy(sChkFlag, "N");
	init(sFeeCodeList);
	init(sDetailCodeList);
	init(sRealFeeList);
	/*ng ����  ҵ�񹤵����� by liuzhou 20090930*/
	memset(&accDoc, 0, sizeof(accDoc));
	memset(&userDoc, 0, sizeof(userDoc));
	init(vBelong_Code     );
	init(favbizCode);
	init(vLogin_No        );
	init(vOp_Code         );
	init(vOp_Note         );
	init(dynStmt          );
	init(vTotal_Date      );
	init(vDateChar        );
	init(vUnit_Id         );
	init(vUnit_Code       );
	init(vContract_Id     );
	init(vContract_No     );
	init(vProduct_Id      );
	init(vOrder_GoodsId   );
	init(vService_Type    );
	init(vService_Code    );
	init(vMain_Flag       );
	init(vPrice_Code      );
	init(vOrgCode         );

	init(vCust_Id         );
	init(vGrp_Id          );
	init(vAccount_Id      );
	init(vAccount_Passwd  );
	init(vGrp_No          );
	init(vGrp_Name        );
	init(vSm_Code         );
	init(vProduct_Type    );
	init(vProduct_Code    );
	init(vUser_Passwd     );
	init(vProvince        );
	init(vRegion_Code     );
	init(vDistrict_Code   );
	init(vTown_Code       );
	init(vTerritory_Code  );
	init(vCredit_Code     );
	init(vCredit_Value    );
	init(vBill_Type       );
	init(vOp_Time         );
	init(vSrv_Stop        );
	init(vGroup_Id        );
	init(vOrg_Id          );
	init(vBak_Field       );
	init(vVPMN_Flag       );
	init(vOperation_Code  );
	init(vGrpProduct_Code );
	init(vProduct_List    );
	init(vProduct_All     );
	init(vGrp_Flag        );
	init(sqlStr           );
	init(vGrp_UserNo      );
	init(payNoId          );
	init(payNo            );
	init(serviceType      );
	init(strLoginAccept   );
	init(vPay_Code        );
	init(personFlag);
	init(vProduct_Attr);
	init(vProduct_Price);

	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "call publicGrp.cp-----------fpubCreateGrpUser");

	lLoginAccept = lSysAccept;
	strcpy(vLogin_No, login_no);
	strcpy(vOp_Code,  op_code);
	strcpy(vOp_Note,  op_note);
	strcpy(vTotal_Date, total_date);
	strncpy(vDateChar,  total_date, 6);
	sprintf(strLoginAccept, "%ld", lLoginAccept);
	strcpy(serviceType, "60");
	strcpy(vBelong_Code,    grpUserMsg->belong_code);
	strcpy(vCust_Id,        grpUserMsg->cust_id);
	strcpy(vGrp_Id,         grpUserMsg->id_no);
	strcpy(vGrp_No,         grpUserMsg->user_no);
	strcpy(vGrp_Name,       grpUserMsg->user_name);
	strcpy(vAccount_Id,     grpUserMsg->account_no);
	strcpy(vAccount_Passwd, grpUserMsg->account_passwd);
	strcpy(vSm_Code,        grpUserMsg->sm_code);
	strcpy(vProduct_Code,   grpUserMsg->product_code);
	strcpy(vUser_Passwd,    grpUserMsg->user_passwd);
	strcpy(vProvince,       grpUserMsg->prov_code);
	strcpy(vRegion_Code,    grpUserMsg->region_code);
	strcpy(vDistrict_Code,  grpUserMsg->district_code);
	strcpy(vTown_Code,      grpUserMsg->town_code);
	strcpy(vTerritory_Code, grpUserMsg->territory_code);
	strcpy(vCredit_Code,    grpUserMsg->credit_code);
	strcpy(vCredit_Value,   grpUserMsg->credit_value);
	strcpy(vBill_Type,      grpUserMsg->bill_type);
	strcpy(vOp_Time,        grpUserMsg->op_time);
	strcpy(vSrv_Stop,       grpUserMsg->srv_stop);
	strcpy(vGroup_Id,       grpUserMsg->group_id);
	strcpy(vOrg_Id,         grpUserMsg->org_id);
	strcpy(vBak_Field,      grpUserMsg->bak_field);
	strcpy(vPay_Code,       grpUserMsg->pay_code);
	Trim(vBelong_Code);
	Trim(vProduct_Code);
	Trim(vOp_Time);
	Trim(strLoginAccept);
	Trim(vGroup_Id);


	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->belong_code    =[%s]", grpUserMsg->belong_code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->cust_id        =[%s]", grpUserMsg->cust_id       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->id_no          =[%s]", grpUserMsg->id_no         );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->user_no        =[%s]", grpUserMsg->user_no       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->user_name      =[%s]", grpUserMsg->user_name     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->account_no     =[%s]", grpUserMsg->account_no    );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->account_passwd =[%s]", grpUserMsg->account_passwd);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->sm_code        =[%s]", grpUserMsg->sm_code       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->product_code   =[%s]", grpUserMsg->product_code  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->user_passwd    =[%s]", grpUserMsg->user_passwd   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->prov_code      =[%s]", grpUserMsg->prov_code     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->region_code    =[%s]", grpUserMsg->region_code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->district_code  =[%s]", grpUserMsg->district_code );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->town_code      =[%s]", grpUserMsg->town_code     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->territory_code =[%s]", grpUserMsg->territory_code);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->credit_code    =[%s]", grpUserMsg->credit_code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->credit_value   =[%s]", grpUserMsg->credit_value  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->bill_type      =[%s]", grpUserMsg->bill_type     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->op_time        =[%s]", grpUserMsg->op_time       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->srv_stop       =[%s]", grpUserMsg->srv_stop      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->group_id       =[%s]", grpUserMsg->group_id      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->org_id         =[%s]", grpUserMsg->org_id        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->bak_field      =[%s]", grpUserMsg->bak_field     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->pay_code       =[%s]", grpUserMsg->pay_code      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->account_type   =[%s]", grpUserMsg->account_type  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->channel_id     =[%s]", grpUserMsg->channel_id    );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "lLoginAccept[%d]",    lLoginAccept   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLogin_No[%s]",       vLogin_No      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Code[%s]",        vOp_Code       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Note[%s]",        vOp_Note       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTotal_Date[%s]",     vTotal_Date    );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vDateChar[%s]",       vDateChar      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBelong_Code[%s]",    vBelong_Code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCust_Id[%s]",        vCust_Id       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Id[%s]",         vGrp_Id        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_No[%s]",         vGrp_No        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Name[%s]",       vGrp_Name      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Code[%s]",   vProduct_Code  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vUser_Passwd[%s]",    vUser_Passwd   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProvince[%s]",       vProvince      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vRegion_Code[%s]",    vRegion_Code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vDistrict_Code[%s]",  vDistrict_Code );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTown_Code[%s]",      vTown_Code     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTerritory_Code[%s]", vTerritory_Code);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Code[%s]",    vCredit_Code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Value[%s]",   vCredit_Value  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBill_Type[%s]",      vBill_Type     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Time[%s]",        vOp_Time       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSrv_Stop[%s]",       vSrv_Stop      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBak_Field[%s]",      vBak_Field     );


	if (strlen(vPay_Code) == 0) {
		sprintf(grpUserMsg->return_message, "�ʻ����ʽ����Ϊ��!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190001", "%s",grpUserMsg->return_message);
		return 190001;
	}

	/* ��ϲ�Ʒ��sm_code��product_type��Ҫ���� */
	memset(vProduct_Type, 0, sizeof(vProduct_Type));
	EXEC SQL SELECT a.product_code INTO :vProduct_Type
		FROM sGrpOperationProductCode a, sGrpSmCode b
		WHERE a.product_code = b.product_code AND b.sm_code = :vSm_Code;
	if (SQLCODE !=0)
	{
		sprintf(grpUserMsg->return_message, "��ѯsGrpOperationProductCode����!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190002", "%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190002;
	}
	Trim(vProduct_Type);

	/*�Ӽ��ŷ���Ʒ�Ʊ�õ�grp_flag���ж��Ƿ���Ҫ¼���ͬ�����õ�GRPϵͳ�Ĳ�Ʒ����*/
	EXEC SQL SELECT GRP_FLAG, PRODUCT_CODE
		INTO :vGrp_Flag, :vGrpProduct_Code
		FROM sGrpSmCode
		WHERE SM_CODE = :vSm_Code;
	if(SQLCODE == 1403)
	{
		sprintf(grpUserMsg->return_message, "�޸ò�ƷƷ��[%s]����!", vSm_Code);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190005",grpUserMsg->return_message);
		return 190005;
	}
	else if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯsGrpSmCode�����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190006",grpUserMsg->return_message);
		return 190006;
	}

	if (vGrp_Flag[0] == 'Y')
	{
		Sinitn(vService_No);
		/*��ѯ���ŵ�����Ϣ*/

		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
		/*
		**EXEC SQL SELECT Unit_Id, Trim(Unit_Code), nvl(VPMN_FLAG,' '), Service_No
		**		   INTO :vUnit_Id, :vUnit_Code, :vVPMN_Flag, :vService_No
		**		   FROM dCustDocOrgAdd
		**		  WHERE Cust_Id = To_Number(:vCust_Id);
		***/
		EXEC SQL SELECT Unit_Id, Trim(Unit_Id),nvl(VPMN_FLAG,' '),Service_No
			INTO :vUnit_Id, :vUnit_Code,:vVPMN_Flag, :vService_No
			FROM dCustDocOrgAdd
			WHERE Cust_Id = To_Number(:vCust_Id);
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
		if(SQLCODE == 1403)
		{
			sprintf(grpUserMsg->return_message, "����[%s]��Ϣ������2222!", vCust_Id);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190002",grpUserMsg->return_message);
			return 190002;
		}
		else if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��ѯdCustDocOrgAdd�����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190003",grpUserMsg->return_message);
			return 190003;
		}
		Trim(vUnit_Id);
		Trim(vUnit_Code);
		Trim(vVPMN_Flag);
		Trim(vService_No);
	}

	/*��VPMN���ſͻ�����������VPMN��Ʒ*/
#if PROVINCE_RUN == PROVINCE_JILIN
	if (('0' == vVPMN_Flag[0]) && (0 == strcmp(vSm_Code, VPMN_SM_CODE)))
	{
		sprintf(grpUserMsg->return_message, "��VPMN���ſͻ�,��������VPMN��Ʒ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190004", grpUserMsg->return_message);
		return 190004;
	}
#endif

	/* �жϼ��ſ����Ƿ���¼���ͬ */
	if (vGrp_Flag[0] == 'Y')
	{
		init(vContract_Id);
		init(vContract_No);
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
		/*EXEC SQL SELECT Contract_Id, Trim(Contract_No)
		**		   INTO :vContract_Id, :vContract_No
		**		   FROM dGrpContract
		**		  WHERE Cust_Id = To_Number(:vCust_Id)
		**		and rownum < 2;
		*/
		EXEC SQL SELECT Contract_Id,Trim(Contract_Id)
			INTO :vContract_Id,:vContract_No
			FROM dGrpContract
			WHERE Cust_Id = To_Number(:vCust_Id)
			and rownum < 2;
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
		if(SQLCODE == 1403)
		{
			sprintf(grpUserMsg->return_message, "����[%s]��ͬ��Ϣ��δ¼��!", vCust_Id);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190007",grpUserMsg->return_message);
			return 190007;
		}
		else if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��ѯdGrpContract�����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190008",grpUserMsg->return_message);
			return 190008;
		}
	}

	/*VPMN�û�����ʱ�����ؼ��ſͻ�ϵͳ��BOSS_VPMN_CODE�ֶ�*/
	Trim(vGrp_No);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "update dGrpCustMsg vVPMN_Flag=[%c], vSm_Code=[%s]", vVPMN_Flag[0], vSm_Code);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**if ((0 == strcmp(vSm_Code, VPMN_SM_CODE)) || (0 == strcmp(vSm_Code, "vq"))) {
	**	EXEC SQL UPDATE dCustDocOrgAdd SET BOSS_VPMN_CODE = :vGrp_No,VPMN_FLAG='1'
	**		WHERE Cust_Id = To_Number(:vCust_Id);
	**	if(SQLCODE!=OK){
	**		sprintf(grpUserMsg->return_message, "����dCustDocOrgAdd�����![%d][%s]",SQLCODE,vCust_Id);
	**		PUBLOG_DBDEBUG("fpubCreateGrpUser");
	**		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190009",grpUserMsg->return_message);
	**		return 190009;
	**	}
	**}
	***/
	if ((0 == strcmp(vSm_Code, VPMN_SM_CODE)) || (0 == strcmp(vSm_Code, "vq")))
	{
		EXEC SQL UPDATE dCustDocOrgAdd SET VPMN_FLAG='1'
			WHERE Cust_Id = To_Number(:vCust_Id);
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "����dCustDocOrgAdd�����![%d][%s]",SQLCODE,vCust_Id);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190009",grpUserMsg->return_message);
			return 190009;
		}
	}
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
	Trim(vUnit_Id);
	Trim(vUnit_Code);
	Trim(vContract_Id);
	Trim(vContract_No);

	/* �жϲ�Ʒ�Ƿ��ظ����� */
	ret = chkGrpProdExist(vCust_Id, vGrp_Id, vProduct_Code, vSm_Code, grpUserMsg->return_message);
	if (ret != 0)
	{
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190010","�жϲ�Ʒ�Ƿ��ظ�����" );
		return 190010;
	}

	/*�Ƿ���ڼ����¼� chenxuan boss3
	**isBillRowNum=0;
	**EXEC SQL SElECT count(*) INTO :isBillRowNum
	**  FROM sProductAttr
	**  WHERE product_attr = (select product_attr from sProductCode where product_code=:vProduct_Code)
	**	AND isbill = 'Y';
	**if(SQLCODE!=OK){
	**		sprintf(grpUserMsg->return_message, "�ж��Ƿ���ڼƷ��¼�ʧ��!");
	**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**		return 190012;
	**}
	*/

	strcpy(payNo, grpUserMsg->bak_field);
	Trim(payNo);

	/*���ɼ����ڲ�����*/
	GetInnerUserNo(vRegion_Code, vSm_Code, vGrp_UserNo, grpUserMsg->return_message);
	Trim(vGrp_UserNo);
	if (atol(vGrp_UserNo) <= 0)
	{
		sprintf(grpUserMsg->return_message, "ȡ�����ڲ����벻�ɹ���������ȷ��!");
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser",payNo,grpUserMsg->return_message);
		return ERR_GET_MAX_SYS_ACCEPT;
	}

	vCount = 0;
	EXEC SQL SELECT count(*) INTO :vCount
		FROM dAccountIdInfo
		WHERE ((service_type = :vSm_Code AND service_no=:vGrp_No) OR (msisdn = :vGrp_UserNo))
		AND substr(run_code,2,1) < 'a';
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "��ѯdAccountIdinfoʧ�ܣ�SQLCODE[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190013",grpUserMsg->return_message);
		return 190013;
	}
	if (vCount != 0)
	{
		sprintf(grpUserMsg->return_message, "�Ѿ����ڴ˼�����Ϣ!");
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190014",grpUserMsg->return_message);
		return 190014;
	}

	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "@##@#@#@###############################@@@@@@@@@@@@");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO dAccountIdInfo ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "( ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "        MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", ") ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "VALUES ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "( ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "'%s', '%s', '%s', 'AA', to_date('%s', 'YYYYMMDD HH24:MI:SS'), ", vGrp_UserNo, vSm_Code, vGrp_No, vOp_Time);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "to_date('%s', 'YYYYMMDD HH24:MI:SS'),'%s' ", vOp_Time, vOp_Note);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "); ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", " ");

	/*ng����
	**EXEC SQL INSERT INTO dAccountIdInfo
	**	(MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE)
	**  VALUES
	**	(:vGrp_UserNo, :vSm_Code, :vGrp_No, 'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**	to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vOp_Note);
	**if (SQLCODE != 0)
	**{
	**	sprintf(grpUserMsg->return_message, "�����dAccountIdInfo����![%d]", SQLCODE);
	**	PUBLOG_DBDEBUG("fpubCreateGrpUser");
	**	pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190015",grpUserMsg->return_message);
	**	return 190015;
	**}
	**ng����*/
	/*ng���� by yaoxc begin*/
	strcpy(tdAccountIdInfo.sMsisdn    		, vGrp_UserNo);
	strcpy(tdAccountIdInfo.sServiceType 	, vSm_Code);
	strcpy(tdAccountIdInfo.sServiceNo 		, vGrp_No);
	strcpy(tdAccountIdInfo.sRunCode 		, "AA");
	strcpy(tdAccountIdInfo.sRunTime 		, vOp_Time);
	strcpy(tdAccountIdInfo.sInnetTime 		, vOp_Time);
	strcpy(tdAccountIdInfo.sNote 	 		, vOp_Note);

	v_ret=OrderInsertAccountIdInfo(ORDERIDTYPE_USER,vGrp_Id,100,
							vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							tdAccountIdInfo);
	printf("OrderInsertAccountIdInfo ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		sprintf(grpUserMsg->return_message, "�����dAccountIdInfo����![%d]", SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190015",grpUserMsg->return_message);
		return 190015;
	}
	/*ng���� by yaoxc end*/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO dAccountIdInfoHis");
#endif

	EXEC SQL INSERT INTO dAccountIdInfoHis
			(MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE,UPDATE_LOGIN,
			UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT,UPDATE_CODE,UPDATE_TYPE)
		VALUES
			(:vGrp_UserNo, :vSm_Code, :vGrp_No, 'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
			to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vOp_Note,
			:vLogin_No, to_number(:vTotal_Date), to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:lLoginAccept,:vOp_Code,'a');
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "�����dAccountIdInfoHis����![%d]", SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190016",grpUserMsg->return_message);
		return 190016;
	}

#ifdef _CHGTABLE_
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO wAccountIdInfoChg");
#endif

	EXEC SQL INSERT INTO wAccountIdInfoChg
			(MSISDN,SERVICE_TYPE,SERVICE_NO,UPDATE_TYPE,RUN_CODE,OP_TIME)
		VALUES
			(:vGrp_UserNo, :vSm_Code, :vGrp_No, 'I', 'AA',to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'));
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "�����wAccountIdInfoChg����![%d]", SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190017",grpUserMsg->return_message);
		return 190017;
	}
#endif
#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "SELECT FROM dCustMsg");
#endif

	memset(payNoId, 0, sizeof(payNoId));
	if (payNo[0] != '\0')
	{
		EXEC SQL SELECT to_char(id_no) INTO :payNoId
			FROM dCustMsg
			WHERE phone_no = :payNo AND substr(run_code,2,1) < 'a';
		if (payNoId[0] == '\0')
		{
			sprintf(grpUserMsg->return_message, "��ѯ���Ѻ����ID����![%d]", SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190018", "%%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190018;
		}
		Trim(payNoId);
	}
	else
	{
		strcpy(payNoId, "0");
	}

	vCount = 0;
	/*��ѯdConMsg������Ƿ���ڼ�¼*/
	EXEC SQL SELECT COUNT(*) INTO :vCount
		FROM dConMsg
		WHERE CONTRACT_NO = To_Number(:vAccount_Id);
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯ�����û�dConMsg�����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190019",grpUserMsg->return_message);
		return 190019;
	}

	/*��¼dConMsg������¼*/
	/*ÿ�������û�����������һ��Ĭ���ʻ���99999999ΪĬ���ʻ�*/
	if (vCount == 0) /*�ʻ���������ڣ��ͽ����ʻ���������,�����û�����ʱ����ѡ��ԭ�����ʻ�*/
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "���ɼ����û�ͳһ�����ʻ�");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO dConMsgHis ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   ACCOUNT_TYPE, UPDATE_LOGIN,  UPDATE_ACCEPT,   UPDATE_DATE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   UPDATE_TIME,  UPDATE_CODE,   UPDATE_TYPE) ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", " VALUES ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  ('%s', '%s', '%s', '%s', ", vAccount_Id, vCust_Id, vAccount_Passwd, vGrp_Name);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   0,            '%s',    0,               to_date('%s', 'YYYYMMDD HH24:MI:SS'), ", vBelong_Code, vOp_Time);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   'Y',          to_date('%s', 'YYYYMMDD HH24:MI:SS'),       '0',             0, ", vOp_Time);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   '%s',   0,             0,               'A', ", vDateChar);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   '%s',          ' ',           ' ',             ' ', ", vPay_Code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   '1',         '%s',    %d,   '%s', ", vLogin_No, lLoginAccept, vTotal_Date);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   to_date('%s', 'YYYYMMDD HH24:MI:SS'),      '%s',     'A'); ", vOp_Time, vOp_Code);
#endif

		/*EXEC SQL INSERT INTO dConMsgHis
		**	(CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
		**		ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
		**		STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
		**		MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
		**		PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
		**		ACCOUNT_TYPE, UPDATE_LOGIN,  UPDATE_ACCEPT,   UPDATE_DATE,
		**		UPDATE_TIME,  UPDATE_CODE,   UPDATE_TYPE     )
		**	VALUES
		**		(:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
		**		0,            :vBelong_Code,    0,               to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
		**		'Y',          to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       '0',             0,
		**		:vDateChar,   0,             0,               'A',
		**		:vPay_Code,   ' ',           ' ',             ' ',
		**		:grpUserMsg->account_type,          :vLogin_No,    :lLoginAccept,   :vTotal_Date,
		**		to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),      :vOp_Code,'A');
		**if(SQLCODE!=OK){
		**	sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgHis�����![%d]",SQLCODE);
		**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**	return 190020;
		**}
        **
		**#ifdef _DEBUG_
		**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConMsg");
		**#endif
		**EXEC SQL INSERT INTO dConMsg
		**	   (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
		**		ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
		**		STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
		**		MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
		**		PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
		**		ACCOUNT_TYPE)
		**	  VALUES
		**	   (:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
		**		0,            :vBelong_Code,    0,               to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
		**		'Y',          to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       '0',             0,
		**		:vDateChar,   0,             0,               'A',
		**		:vPay_Code,   'NULL',        'NULL',          'NULL',
		**		:grpUserMsg->account_type);
		**if(SQLCODE!=OK){
		**	sprintf(grpUserMsg->return_message, "���뼯���û�dConMsg�����![%d]",SQLCODE);
		**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**	return 190021;
		**}
		*/
		/*  ��֯�ṹ���� majha   add����createConMsg()������Ϣat 27/02/2009	*/
		EXEC SQL SELECT ORG_CODE
			INTO  :vOrgCode
			FROM  dloginmsg
			WHERE  login_no=:login_no;
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��ȡORG_CODE����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190020",grpUserMsg->return_message);
			return 190020;
		}

		sprintf(accDoc.vLogin_Accept,      	"%ld", lLoginAccept);
		strncpy(accDoc.vContract_No,       vAccount_Id,         14);
		strncpy(accDoc.vCon_Cust_Id,       vCust_Id,            10);
		strncpy(accDoc.vContract_Passwd,	 vAccount_Passwd,    8);
		strncpy(accDoc.vBank_Cust,      	 vGrp_Name,         60);
		strncpy(accDoc.vBelong_Code,       vBelong_Code,         7);
		strncpy(accDoc.vAccount_Limit,     "A",                  1);
		strncpy(accDoc.vStatus,            "Y",                  1);
		strncpy(accDoc.vPost_Flag,         "0",                  1);
		strncpy(accDoc.vPay_Code,          vPay_Code,            1);
		strncpy(accDoc.vBank_Code,      	 "NULL",             5);
		strncpy(accDoc.vPost_Bank_Code,    "NULL",               5);
		strncpy(accDoc.vAccount_No,      	 "NULL",            30);
		strncpy(accDoc.vAccount_Type,   grpUserMsg->account_type,1);
		strncpy(accDoc.vBegin_Time,       vOp_Time,             17);
		strncpy(accDoc.vEnd_Time,          "",                  17);   /*��¼*/
		strncpy(accDoc.vOp_Code,           vOp_Code,             4);
		strncpy(accDoc.vLogin_No,          vLogin_No,            6);
		strncpy(accDoc.vOrg_Code,          vOrgCode,             9);
		strncpy(accDoc.vOp_Note,           op_note,             60);
		strncpy(accDoc.vSystem_Note,       op_note,             60);
		strncpy(accDoc.vIp_Addr,           "0.0.0.0",           15);
		strncpy(accDoc.vGroup_Id,          vGroup_Id,           10);
		strncpy(accDoc.vUpdate_Type,       "A",                  1);
		accDoc.vOddment     = 0.00;
		accDoc.vPrepay_Fee  = 0.00;
		accDoc.vDeposit     = 0.00;
		accDoc.vOwe_Fee     = 0.00;
		accDoc.vAccount_Mark = 0;

		ret = 0;
		/************ng���� Ϊ����send_idһ�� �滻ԭ�����ʻ���Ϣ�������� begin****************/

		/*ret = createConMsg(&accDoc,grpUserMsg->return_message);*/
		ret = createConMsg_ng(ORDERIDTYPE_USER, vGrp_Id, &accDoc, grpUserMsg->return_message);

		/************ng���� Ϊ����send_idһ�� �滻ԭ�����ʻ���Ϣ�������� end******************/
		if(ret != 0)
		{
			sprintf(grpUserMsg->return_message, "����createConMsg()����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190009", "-----%d:%s---",ret,grpUserMsg->return_message);
			return 190009;
		}
		/*  ��֯�ṹ���� majha   end����createConMsg()������Ϣat 27/02/2009	*/
		/*ng ���� ҵ�񹤵����� by liuzhou 20090930 begin*/
		/*
		**#ifdef _DEBUG_
		**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConMsgPre");
		**#endif
		**EXEC SQL INSERT INTO dConMsgPre
		**			  ( CONTRACT_NO, PAY_TYPE,  PREPAY_FEE, LAST_PREPAY,
		**				ADD_PREPAY,  LIVE_FLAG, ALLOW_PAY,  BEGIN_DT,
		**				END_DT)
		**		 VALUES
		**			 ( To_Number(:vAccount_Id), '0', 0, 0,
		**			   0, '0', 0, :total_date,
		**			   :vSrv_Stop);
		**if (SQLCODE != 0) {
		**	sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgPre����![%d]",SQLCODE);
		**	PUBLOG_DBDEBUG("fpubCreateGrpUser");
		**	pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190022",grpUserMsg->return_message);
		**	return 190022;
		**}
		***/
		/*ng ���� ҵ�񹤵����� by liuzhou 20090930 end*/

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustConMsgHis");
		#endif
		EXEC SQL INSERT INTO dCustConMsgHis
					(   Cust_Id,         Contract_No,     Begin_Time,
						End_Time,        Update_Accept,   Update_Login,
						Update_Date,     Update_Time,     Update_Code,
						Update_Type
					)
					VALUES
					(   To_Number(:vCust_Id),To_Number(:vAccount_Id), to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
						To_Date(:vSrv_Stop,'YYYYMMDD'),
						To_Number(:lSysAccept),  :vLogin_No,
						To_Number(:total_date),  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), :op_code,
						'a'
					);
		if (SQLCODE != 0)
		{
			sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsgHis����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190023",grpUserMsg->return_message);
			return 190023;
		}

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustConMsg");
		#endif
		/*ng����
		**EXEC SQL INSERT INTO dCustConMsg
		**			(  Cust_Id,  Contract_No,  Begin_Time,    End_Time )
		**			VALUES
		**			(   To_Number(:vCust_Id), To_Number(:vAccount_Id),
		**				to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), To_Date(:vSrv_Stop,'YYYYMMDD')
		**			);
		**if (SQLCODE != 0) {
		**	sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
		**	PUBLOG_DBDEBUG("fpubCreateGrpUser");
		**	pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190024",grpUserMsg->return_message);
		**	return 190024;
		**}
		**ng����*/
		/*ng���� by yaoxc begin*/
		strcpy(tdCustConMsg.sCustId    		, vCust_Id);
		strcpy(tdCustConMsg.sContractNo 	, vAccount_Id);
		strcpy(tdCustConMsg.sBeginTime 		, vOp_Time);
		strcpy(tdCustConMsg.sEndTime 	 	, vSrv_Stop);

		v_ret=OrderInsertCustConMsg(ORDERIDTYPE_USER,vGrp_Id,100,
							   vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							   tdCustConMsg);
		printf("OrderInsertCustConMsg ,ret=[%d]\n",v_ret);
		if(0!=ret)
		{
			sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190024",grpUserMsg->return_message);
			return 190024;
		}
		/*ng���� by yaoxc end*/
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustUserHis");
#endif

	EXEC SQL INSERT INTO dCustUserHis
		(CUST_ID,ID_NO,REP_FLAG,CUST_FLAG, UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_ACCEPT,OP_CODE)
		VALUES
		(to_number(:vCust_Id), to_number(:vGrp_Id), '0','0',to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vLogin_No,'a',  :lLoginAccept,:vOp_Code);
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "����dCustUserHis����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190025",grpUserMsg->return_message);
		return 190025;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustUserMsg");
#endif

	EXEC SQL INSERT INTO dCustUserMsg
		(CUST_ID,ID_NO,REP_FLAG,CUST_FLAG)
		VALUES
		(to_number(:vCust_Id), to_number(:vGrp_Id), '0','0');
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "����dCustUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190026",grpUserMsg->return_message);
		return 190026;
	}

	/* ���ڼ��Ų�Ʒ����dCustMsg�����ų�����Ҫ chenxuan boss3 */
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustMsgHis");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO dCustMsgHis ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "( ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE, ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE, ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    RUN_CODE,OPEN_TIME,RUN_TIME, ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD, ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_ID ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", ") ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "VALUES ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "( ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    to_number('%s'), to_number('%s'), to_number('%s'),1,'%s','%s','%s', ", vGrp_Id, vCust_Id, vAccount_Id, vGrp_UserNo, vSm_Code, serviceType);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    '00000000','%s','%s',0, 'A', 0, ", vUser_Passwd, vBelong_Code);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    'AA', to_date('%s', 'YYYYMMDD HH24:MI:SS'),to_date('%s', 'YYYYMMDD HH24:MI:SS'), ", vOp_Time, vOp_Time);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    to_date('%s', 'YYYYMMDD HH24:MI:SS'), to_number('%s'),'NULL',   0,'%s','001', ", vOp_Time, vBill_Type, vBill_Type);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    '%d', to_date('%s', 'YYYYMMDD HH24:MI:SS'),to_number('%s'), ", lLoginAccept, vOp_Time, vTotal_Date);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "    '%s', 'a', '%s','%s' ", vLogin_No, vOp_Code, vGroup_Id);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "); ");
#endif
	/*   EXEC SQL INSERT INTO dCustMsgHis
	**  (ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**  RUN_CODE,OPEN_TIME,RUN_TIME,
	**  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
	**  UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_ID)
	**  VALUES
	**  (to_number(:vGrp_Id), to_number(:vCust_Id), to_number(:vAccount_Id),1,:vGrp_UserNo,:vSm_Code,:serviceType,
	**  '00000000',:vUser_Passwd,:vBelong_Code,0, 'A', 0,
	**  'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), to_number(:vBill_Type),'NULL',       0,:vBill_Type,'001',
	**  :lLoginAccept, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_number(:vTotal_Date),
	**  :vLogin_No, 'a', :vOp_Code,:vGroup_Id);
	**if (SQLCODE != 0) {
	**		sprintf(grpUserMsg->return_message, "����dCustMsgHis����![%d]",SQLCODE);
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**		return 190027;
	**}
	** #ifdef _DEBUG_
	**pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustMsg");
	**#endif
	**
	**EXEC SQL INSERT INTO dCustMsg
	**  (ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**  RUN_CODE,OPEN_TIME,RUN_TIME,
	**  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,GROUP_ID)
	**  VALUES
	**  (to_number(:vGrp_Id), to_number(:vCust_Id), to_number(:vAccount_Id),1,:vGrp_UserNo,:vSm_Code,:serviceType,
	**  '00000000',:vUser_Passwd,:vBelong_Code,0, 'A', 0,
	**  'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), to_number(:vBill_Type),'NULL',       0,:vBill_Type,'001',:vGroup_Id);
	**if (SQLCODE != 0) {
	**  sprintf(grpUserMsg->return_message, "����dCustMsg����![%d]",SQLCODE);
	**  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**  return 190028;
	**}
	*/

	/*��֯�������� majha   add����createCustMsg()������Ϣ	*/

	userDoc.vCust_Id=atol(vCust_Id);
	userDoc.vId_No=atol(vGrp_Id);
	userDoc.vContract_No=atol(vAccount_Id);
	strncpy(userDoc.vPhone_No,       	vGrp_UserNo,       15);
	strncpy(userDoc.vSm_Code,        	vSm_Code,          	2);
	strncpy(userDoc.vService_Type,   	serviceType,     	2);
	strncpy(userDoc.vAttr_Code,      	"000000",        	8);
	strncpy(userDoc.vUser_Passwd,    	vUser_Passwd,       8);
	strncpy(userDoc.vOpen_Time,      	vOp_Time,          17);
	strncpy(userDoc.vBelong_Code,    	vBelong_Code,      	7);
	strncpy(userDoc.vCredit_Code,    	"A",      	        1);
	strncpy(userDoc.vRun_Code,       	"AA",              	2);
	strncpy(userDoc.vRun_Time,       	vOp_Time,          17);
	strncpy(userDoc.vBill_Date,      	vOp_Time,          17);
	strncpy(userDoc.vEncrypt_Prepay, 	"NULL",            16);
	strncpy(userDoc.vLast_Bill_Type, 	vBill_Type,        	1);
	strncpy(userDoc.vBak_Field,      	"001",         	   12);
	strncpy(userDoc.vOp_Time,        	vOp_Time,          17);
	strncpy(userDoc.vLogin_No,       	vLogin_No,         	6);
	strncpy(userDoc.vOp_Code,        	vOp_Code,          	4);
	strncpy(userDoc.vGroup_Id,       	vGroup_Id,         10);
	strncpy(userDoc.varea_code ,		"",                10);
	userDoc.vIds=1;
	userDoc.vLimit_Owe=0;
	userDoc.vCredit_Value=0;
	userDoc.vCmd_Right=0;
	userDoc.vBill_Type=atoi(vBill_Type);
	userDoc.vLogin_Accept=lLoginAccept;
	userDoc.vTotal_Date=atoi(vTotal_Date);

	ret=0;
	ret = createCustMsg(&userDoc,grpUserMsg->return_message);
	if(ret != 0)
	{
		sprintf(grpUserMsg->return_message, "����dCustMsgHis����![%d]",ret);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190027", "%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190027;
	}
	/*��֯�������� majha   end����createCustMsg()������Ϣ	*/

#ifdef _CHGTABLE_
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into wUserMsgChg");
#endif

#if PROVINCE_RUN != PROVINCE_JILIN
	EXEC SQL INSERT INTO wUserMsgChg
	(
		OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
		ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
	)
	VALUES
	(
		sMaxBillChg.NEXTVAL, '1', TO_NUMBER(:vGrp_Id), TO_NUMBER(:vCust_Id),:vGrp_UserNo,:vSm_Code,
		'00000000', :vBelong_Code, 'AA',
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),:vGroup_Id
	);
#else
	EXEC SQL INSERT INTO wUserMsgChg
		(OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
		ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
		)
	VALUES
		(
		sMaxBillChg.NEXTVAL, '1', TO_NUMBER(:vGrp_Id), TO_NUMBER(:vCust_Id),:vGrp_UserNo,:vSm_Code,
		'00000000', :vBelong_Code, 'AA',
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),:vGroup_Id
		);
#endif
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "����wUserMsgChg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190029",grpUserMsg->return_message);
		return 190029;
	}
#endif
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConUserMsgHis");
#endif

	EXEC SQL INSERT INTO dConUserMsgHis
	(
		Id_No,        Contract_No,  Serial_No,
		Bill_Order,   Pay_Order,    Begin_YMD,
		Begin_TM,     End_YMD,      End_TM,
		Limit_Pay,    Rate_Flag,    Stop_Flag,
		Update_Accept,Update_Login, Update_Date,
		Update_Time,  Update_Code,  Update_Type
	)
	VALUES
	(
		To_Number(:vGrp_Id),To_Number(:vAccount_Id),0,
		99999999,     1,           :total_date,
		SubStr(:total_date,1,6),   :vSrv_Stop,   Substr(:vSrv_Stop, 1,6),
		9999999999,  'N',          'Y',
		To_Number(:lLoginAccept), :vLogin_No, To_Number(:total_date),
		to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),     :op_code,    'a'
	);
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsgHis����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190030", "%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190030;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConUserMsg");
#endif
	/*ng����
	**EXEC SQL INSERT INTO dConUserMsg
	**  (Id_No,      Contract_No,  Serial_No,
	**				Bill_Order, Pay_Order,    Begin_YMD,
	**				Begin_TM,   End_YMD,      End_TM,
	**				Limit_Pay,  Rate_Flag,    Stop_Flag
	**  )
	**  VALUES
	**  (
	**  To_Number(:vGrp_Id),To_Number(:vAccount_Id),0,
	**  99999999,   1,           :total_date,
	**  to_char(to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'), 'hh24miss'),
	**  :vSrv_Stop, '235959',
	**  9999999999, 'N', 'Y'
	**  );
	**if (SQLCODE != 0) {
	**	sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
	**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190031", "%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**	return 190031;
	**}
	**ng����*/
	/*ng���� by yaoxc begin*/
	EXEC SQL select to_char(to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'), 'hh24miss') into :sBeginTm from dual;

	strcpy(tdConUserMsg.sIdNo    		, vGrp_Id);
	strcpy(tdConUserMsg.sContractNo 	, vAccount_Id);
	strcpy(tdConUserMsg.sBillOrder 		, "99999999");
	strcpy(tdConUserMsg.sSerialNo 	 	, "0");
	strcpy(tdConUserMsg.sPayOrder 		, "1");
	strcpy(tdConUserMsg.sBeginYmd   	, total_date);
	strcpy(tdConUserMsg.sBeginTm    	, sBeginTm);
	strcpy(tdConUserMsg.sEndYmd     	, vSrv_Stop);
	strcpy(tdConUserMsg.sEndTm 			, "235959");
	strcpy(tdConUserMsg.sLimitPay   	, "9999999999");
	strcpy(tdConUserMsg.sRateFlag   	, "N");
	strcpy(tdConUserMsg.sStopFlag   	, "Y");

	v_ret=OrderInsertConUserMsg(ORDERIDTYPE_USER,vGrp_Id,100,
						   vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
						   tdConUserMsg);
	printf("OrderInsertConUserMsg ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190031", "%s-- [%d][%s]",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190031;
	}
	/*ng���� by yaoxc end*/

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into wConUserChg");
#endif

	/* ���ڼ��Ų�Ʒ����dCustMsg�����ų�����Ҫ chenxuan boss3 */
#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
	EXEC SQL INSERT INTO wConUserChg
			(
			Op_No,       Op_Type,    Id_No,
			Contract_No, Bill_Order, Conuser_Update_Time,
			Limit_Pay,   Rate_Flag,  Fee_Code, DETAIL_CODE,
			Fee_Rate,   Op_Time,phone_no
			)
		VALUES
			(
			sMaxBillChg.NextVal,     '10',     To_Number(:vGrp_Id),
			To_Number(:vAccount_Id), 99999999, to_char(TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
			0,          'N',      '*', '*',
			0,    to_char(TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
			:vGrp_UserNo
			);
#else
	EXEC SQL INSERT INTO wConUserChg
			(
			Op_No,       Op_Type,    Id_No,
			Contract_No, Bill_Order, Conuser_Update_Time,
			Limit_Pay,   Rate_Flag,  Fee_Code, DETAIL_CODE,
			Fee_Rate,   Op_Time
			)
		VALUES
			(
			sMaxBillChg.NextVal,     '10',     To_Number(:vGrp_Id),
			To_Number(:vAccount_Id), 99999999, to_char(TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
			0,          'N',      '*', '*',
			0,    to_char(TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS')
			);
#endif
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�wConUserChg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190032",grpUserMsg->return_message);
		return 190032;
	}
#endif
	/* Ĭ�϶�����ϵ boss2.0 add 2006-11-16 lixg */
	Trim(vOp_Time);
	char grpModeCode[8+1];
	init(grpModeCode);
	strcpy(grpModeCode,DEFAULT_GRPTOPERSON_CODE);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "pubBillBeginMode begin...");
	retCode = pubBillBeginMode(vGrp_UserNo,grpModeCode,vOp_Time, vOp_Code,
			vLogin_No,strLoginAccept,vOp_Time,"0","","", grpUserMsg->return_message);
	if(0 != retCode)
	{
#ifdef _DEBUG
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190033", "%s:[%s]", __FUNCTION__, grpUserMsg->return_message);
#endif
		return 190033;
	}

	/*4.��¼�����û���dGrpUserMsg��¼,BOSS�������*/
	EXEC SQL INSERT INTO dGrpUserMsgHis
			(CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
			IDS,         USER_NAME,     SM_CODE,       PRODUCT_TYPE,  PRODUCT_CODE,
			USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
			REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
			LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
			OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
			LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     GROUP_ID, UPDATE_LOGIN,
			UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
			UPDATE_TYPE)
		VALUES
			(:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_UserNo,
			1,             :vGrp_Name,    :vSm_Code,      :vProduct_Type, :vProduct_Code,
			:vUser_Passwd, ' ',           ' ',            :vProvince,
			:vRegion_Code, :vDistrict_Code, :vTown_Code,  :grpUserMsg->channel_id,
			1000000,     :vCredit_Code, :vCredit_Value,   'A',
			'A',           to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       To_Date(:vSrv_Stop, 'YYYYMMDD'), :vBill_Type,
			:vBill_Type,   to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       :payNoId,      :vGroup_Id, :vLogin_No,
			:lLoginAccept, :vTotal_Date,  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),        :vOp_Code,
			'A');
	if(SQLCODE!=OK)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsgHis����Ϣ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "INSERT INTO dGrpUserMsgHis ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   IDS,         USER_NAME,     SM_CODE, PRODUCT_TYPE,  PRODUCT_CODE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     GROUP_ID, UPDATE_LOGIN, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE, ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "   UPDATE_TYPE) ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "VALUES ");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", " ('%s','%s','%s','%s', ", vCust_Id, vGrp_Id, vAccount_Id, vGrp_UserNo);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  1, '%s', '%s', '%s', '%s', ", vGrp_Name, vSm_Code, vProduct_Type, vProduct_Code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  '%s', ' ', ' ', '%s', ", vUser_Passwd, vProvince);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  '%s', '%s', '%s', '0', ", vRegion_Code, vDistrict_Code, vTown_Code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  1000000, '%s', '%s', 'A', ", vCredit_Code, vCredit_Value);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  'A', to_date('%s', 'YYYYMMDD HH24:MI:SS'), To_Date('%s', 'YYYYMMDD'), '%s', ", vOp_Time, vSrv_Stop, vBill_Type);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), '%s', '%s', '%s', ", vBill_Type, vOp_Time, vOp_Code, vGroup_Id, vLogin_No);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  %d, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'),'%s', ", lLoginAccept, vTotal_Date, vOp_Time, vOp_Code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "  'A'); ");
#endif
		sprintf(grpUserMsg->return_message, "����dGrpUserMsgHis�����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190033",grpUserMsg->return_message);
		return 190033;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsg����Ϣ");
#endif

	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsg�е�ֵ");
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCust_Id                 = [%s]",vCust_Id              );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Id                  = [%s]",vGrp_Id               );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vAccount_Id              = [%s]",vAccount_Id           );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_UserNo              = [%s]",vGrp_UserNo           );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Name                = [%s]",vGrp_Name             );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSm_Code                 = [%s]",vSm_Code              );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Type            = [%s]",vProduct_Type         );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Code            = [%s]",vProduct_Code         );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vUser_Passwd             = [%s]",vUser_Passwd          );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProvince                = [%s]",vProvince             );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vRegion_Code             = [%s]",vRegion_Code          );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vDistrict_Code           = [%s]",vDistrict_Code        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTown_Code               = [%s]",vTown_Code            );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->channel_id   = [%s]",grpUserMsg->channel_id);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Code             = [%s]",vCredit_Code          );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Value            = [%s]",vCredit_Value         );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Time                 = [%s]",vOp_Time              );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSrv_Stop                = [%s]",vSrv_Stop             );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBill_Type               = [%s]",vBill_Type            );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBill_Type               = [%s]",vBill_Type            );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Time                 = [%s]",vOp_Time              );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "payNoId                  = [%s]",payNoId               );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGroup_Id                = [%s]",vGroup_Id             );

	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSm_Code=[%s]", vSm_Code);
	/*ng����
	**EXEC SQL INSERT INTO dGrpUserMsg
	**	  (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
	**	   IDS,         USER_NAME,     SM_CODE,       PRODUCT_TYPE,  PRODUCT_CODE,
	**	   USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
	**	   REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
	**	   LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
	**	   OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
	**	   LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     GROUP_ID)
	**	VALUES
	**	 (:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_UserNo,
	**	  1,             :vGrp_Name,    :vSm_Code,      :vProduct_Type, :vProduct_Code,
	**	  :vUser_Passwd, ' ',           ' ',            :vProvince,
	**	  :vRegion_Code, :vDistrict_Code, :vTown_Code,  :grpUserMsg->channel_id,
	**	  1000000,       :vCredit_Code, :vCredit_Value, 'A',
	**	  'A',           to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       To_Date(:vSrv_Stop, 'YYYYMMDD'), :vBill_Type,
	**	  :vBill_Type,   to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),       :payNoId, :vGroup_Id);
	**if (SQLCODE!=OK){
	**	  sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
	**	  PUBLOG_DBDEBUG("fpubCreateGrpUser");
	**	  pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190034",grpUserMsg->return_message);
	**	  return 190034;
	**}
	**ng����*/
	/*ng���� by yaoxc begin*/
	strcpy(tdGrpUserMsg.sCustId    		, vCust_Id);
	strcpy(tdGrpUserMsg.sIdNo 			, vGrp_Id);
	strcpy(tdGrpUserMsg.sAccountId 		, vAccount_Id);
	strcpy(tdGrpUserMsg.sUserNo 	 	, vGrp_UserNo);
	strcpy(tdGrpUserMsg.sIds 			, "1");
	strcpy(tdGrpUserMsg.sUserName   	, vGrp_Name);
	strcpy(tdGrpUserMsg.sProductType   	, vProduct_Type);
	strcpy(tdGrpUserMsg.sProductCode    , vProduct_Code);
	strcpy(tdGrpUserMsg.sUserPasswd 	, vUser_Passwd);
	strcpy(tdGrpUserMsg.sLoginName   	, " ");
	strcpy(tdGrpUserMsg.sLoginPasswd   	, " ");
	strcpy(tdGrpUserMsg.sProvCode   	, vProvince);
	strcpy(tdGrpUserMsg.sRegionCode    	, vRegion_Code);
	strcpy(tdGrpUserMsg.sDistrictCode 	, vDistrict_Code);
	strcpy(tdGrpUserMsg.sTownCode 		, vTown_Code);
	strcpy(tdGrpUserMsg.sTerritoryCode 	, grpUserMsg->channel_id);
	strcpy(tdGrpUserMsg.sLimitOwe 		, "1000000");
	strcpy(tdGrpUserMsg.sCreditCode   	, vCredit_Code);
	strcpy(tdGrpUserMsg.sCreditValue 	, vCredit_Value);
	strcpy(tdGrpUserMsg.sRunCode     	, "A");
	strcpy(tdGrpUserMsg.sOldRun 		, "A");
	strcpy(tdGrpUserMsg.sRunTime   		, vOp_Time);
	strcpy(tdGrpUserMsg.sBillDate   	, vSrv_Stop);
	strcpy(tdGrpUserMsg.sBillType   	, vBill_Type);
	strcpy(tdGrpUserMsg.sLastBillType   , vBill_Type);
	strcpy(tdGrpUserMsg.sOpTime   		, vOp_Time);
	strcpy(tdGrpUserMsg.sBakField   	, payNoId);
	strcpy(tdGrpUserMsg.sSmCode   		, vSm_Code);
	strcpy(tdGrpUserMsg.sGroupId   		, vGroup_Id);

	v_ret=OrderInsertGrpUserMsg(ORDERIDTYPE_USER,vGrp_Id,100,
								vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
								tdGrpUserMsg);
	printf("OrderInsertGrpUserMsg ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190034",grpUserMsg->return_message);
		return 190034;
	}
	/*ng���� by yaoxc end*/
	/* ���ӻ�����Ϣ��dMarkMsg 20060217
	**EXEC SQL INSERT INTO dMarkMsg(
	**					ID_NO          ,OPEN_TIME      ,INIT_POINT     ,
	**					CURRENT_POINT  ,YEAR_POINT     ,ADD_POINT      ,
	**					OP_TIME        ,BEGIN_TIME     ,TOTAL_USED     ,
	**					TOTAL_PRIZE    ,TOTAL_PUNISH   ,LASTYEAR_POINT ,
	**					BASE_POINT     ,SM_CODE        )
	**			VALUES( :vGrp_Id       ,to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),0,
	**					0              ,0              ,0              ,
	**					to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),0,
	**					0              ,0              ,0              ,
	**					0              ,:vSm_Code       );
	**if (SQLCODE!=OK){
	**	  sprintf(grpUserMsg->return_message, "����dMarkMsg�����![%d]",SQLCODE);
	**	  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**	  return 190050;
	**}
	*/

	/*���ٴ���VIP��ر� chenxuan boss3
	**if (vGrp_Flag[0] == 'Y') {
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯsGrpOperationProductCode����Ϣ");
	**		#endif
	**		EXEC SQL SELECT OPERATION_CODE
	**				   INTO :vOperation_Code
	**				   FROM sGrpOperationProductCode
	**				  WHERE Product_Code = :vGrpProduct_Code;
	**		if(SQLCODE!=OK)
	**		{
	**			strcpy(grpUserMsg->return_message,"��ѯsGrpOperationProductCode����!");
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			return 190035;
	**		}
    **
    **
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ò�ƷID");
	**		#endif
	**		EXEC SQL SELECT To_Char(s_Product_Id.NextVal) INTO :vProduct_Id FROM DUAL;
	**		if(SQLCODE!=OK)
	**		{
	**			strcpy(grpUserMsg->return_message,"��ѯs_Product_Id����!");
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			return 190036;
	**		}
    **
	**	#ifdef _DEBUG_
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dCustDocOrgAddHis���¼");
	**	#endif
	**	EXEC SQL BEGIN DECLARE SECTION;
	**		char sIn_Grp_OrgCode[10+1];
	**	EXEC SQL END DECLARE SECTION;
	**	init(sIn_Grp_OrgCode);
	**	EXEC SQL SELECT Org_Code
	**	   INTO :sIn_Grp_OrgCode
	**	   FROM dCustDocOrgAdd
	**	   WHERE Cust_Id = To_Number(:vCust_Id)
	**	   AND Unit_Id = To_Number(:vUnit_Id);
	**	if(SQLCODE!=OK)
	**	{
	**	strcpy(grpUserMsg->return_message,"��ѯsIn_Grp_OrgCode����!");
	**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**	return 190036 ;
	**	}
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpProductHis����Ϣ");
	**		#endif
	**		EXEC SQL INSERT INTO dGrpProductHis
	**			 (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
	**			  PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
	**			  BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
	**			  PRODUCT_STATUS, OPERATION_CODE,
	**			  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
	**			  UPDATE_CODE,  UPDATE_TYPE, ORG_CODE)
	**			 VALUES
	**			  (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
	**			   :vGrpProduct_Code, :vContract_Id, :vContract_No,
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   :vOp_Note,
	**			   '1',            :vOperation_Code,
	**			   :vLogin_No,     :lLoginAccept, :vTotal_Date,  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**			   :vOp_Code,      'A',     :sIn_Grp_OrgCode);
	**		if(SQLCODE!=OK){
	**			  sprintf(grpUserMsg->return_message, "����dGrpProductHis�����![%d]",SQLCODE);
	**			  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			  return 190037;
	**		}
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpProduct����Ϣ");
	**		#endif
	**		EXEC SQL INSERT INTO dGrpProduct
	**			 (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
	**			  PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
	**			  BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
	**			  PRODUCT_STATUS, OPERATION_CODE, ORG_CODE)
	**			 VALUES
	**			  (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
	**			   :vGrpProduct_Code, :vContract_Id, :vContract_No,
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**			   :vOp_Note,
	**			   '1',            :vOperation_Code,:sIn_Grp_OrgCode);
    **
	**		if(SQLCODE!=OK){
	**			  sprintf(grpUserMsg->return_message, "����dGrpProduct�����![%d]",SQLCODE);
	**			  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			  return 190038;
	**		}
    **
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ö���ID");
	**		#endif
	**		EXEC SQL SELECT To_Char(s_order_goods_id.NextVal) INTO :vOrder_GoodsId FROM DUAL;
	**		if(SQLCODE!=OK)
	**		{
	**			strcpy(grpUserMsg->return_message,"��ѯs_order_goods_id����!");
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			return 190039;
	**		}
    **
	**		Trim(vProduct_Id);
	**		Trim(vOrder_GoodsId);
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOrder_GoodsId---------------[%s]",vOrder_GoodsId);
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpOrderGoodsHis����ϢvOrder_GoodsId=[%s]",vOrder_GoodsId);
	**		#endif
	**		EXEC SQL INSERT INTO dGrpOrderGoodsHis
	**			   (ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
	**				UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
	**				DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
	**				CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
	**				CURR_TACHE,
	**				UPDATE_LOGIN,   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
	**				UPDATE_CODE,    UPDATE_TYPE,   ORG_CODE)
	**			   VALUES
	**			   (:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
	**				:vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
	**				To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**				To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**				:vGrpProduct_Code, :vOp_Note,
	**				'1', '01', '01', '01',
	**				'01',
	**				:vLogin_No,      :lLoginAccept, :vTotal_Date,  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**				:vOp_Code,       'A',           :sIn_Grp_OrgCode);
	**		if(SQLCODE!=OK){
	**			  sprintf(grpUserMsg->return_message, "����dGrpOrderGoodsHis�����![%d]",SQLCODE);
	**			  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			  return 190040;
	**		}
    **
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpOrderGoods����ϢvOrder_GoodsId=[%s]",vOrder_GoodsId);
	**		#endif
	**		EXEC SQL INSERT INTO dGrpOrderGoods
	**			   (ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
	**				UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
	**				DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
	**				CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
	**				CURR_TACHE,     ORG_CODE)
	**			   VALUES
	**			   (:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
	**				:vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
	**				To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**				To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**				:vGrpProduct_Code, :vOp_Note,
	**				'1', '01', '01', '01',
	**				'01',:sIn_Grp_OrgCode );
	**		if(SQLCODE!=OK){
	**			  sprintf(grpUserMsg->return_message, "����dGrpOrderGoods�����![%d]",SQLCODE);
	**			  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			  return 190041;
	**		}
    **
	**		#ifdef _DEBUG_
	**			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼wGrpOrderOpr����Ϣ");
	**		#endif
	**		EXEC SQL insert into wGrpOrderOpr
	**				(LOGIN_ACCEPT, CUST_ID,       UNIT_ID,    DIS_ORDER_GOODS_ID,
	**				 OPERATOR,     OPERATOR_TIME, CURR_TACHE, CURR_STATUS,
	**				 OPERATOR_CODE)
	**			  values
	**				(:lLoginAccept, :vCust_Id,    :vUnit_Id,  :vOrder_GoodsId,
	**				 :vLogin_No,    to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),      '01',       '01',
	**				 '01');
	**		if(SQLCODE!=OK){
	**			  sprintf(grpUserMsg->return_message, "����wGrpOrderOpr�����![%d]",SQLCODE);
	**			  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**			  return 190042;
	**		}
	**}*/

	/* ���뼯�Ų�Ʒ
	**rowNum = Foccur32(transIN, GPARM32_0+ProdIndx);
	**for(i=0; i < rowNum; i ++)
	**{
	**			memset(grpUserMsg->product_code, 0, sizeof(grpUserMsg->product_code));
	**			if(Fget32(transIN,GPARM32_0+ProdIndx, i, grpUserMsg->product_code, NULL) == -1)
	**			{
	**		sprintf(grpUserMsg->return_message, "�õ���Ʒ�������!");
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
	**		return 190043;
	**			}
	**			Trim(grpUserMsg->product_code);
	**			if (grpUserMsg->product_code[0] == '\0')
	**			{
	**					continue;
	**			}
    **
	**			memset(vProduct_Price, 0, sizeof(vProduct_Price));
	**			if(Fget32(transIN,GPARM32_1+ProdIndx, i, vProduct_Price, NULL) == -1)
	**			{
	**		sprintf(grpUserMsg->return_message, "�õ���Ʒ����۸����!");
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
	**		return 190044;
	**			}
	**			Trim(vProduct_Price);
	**			memset(vProduct_Attr, 0, sizeof(vProduct_Attr));
	**			if(Fget32(transIN,GPARM32_2+ProdIndx, i, vProduct_Attr, NULL) == -1)
	**			{
	**		sprintf(grpUserMsg->return_message, "�õ���Ʒ���Գ���!");
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
	**		return 190045;
	**			}
	**			Trim(vProduct_Attr);
	**			if ( (retCode = beginProduct(vGrp_UserNo,grpUserMsg->product_code,vProduct_Price,vProduct_Attr,grpUserMsg->op_time,MAXENDTIME,
	**															vOp_Code,vLogin_No,strLoginAccept,grpUserMsg->op_time,grpUserMsg->return_message)) != 0)
	**			{
	**		sprintf(grpUserMsg->return_message, "�����Ʒ[%s]ʧ��!", grpUserMsg->product_code);
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s:%s",__FILE__,grpUserMsg->return_message,vLogin_No);
	**		return retCode;
	**			}
	**}
	***/

	/* ���ֵ�������ADC��MAS�ķ������ */
#if PROVINCE_RUN == PROVINCE_JILIN
	rowNum = Foccur32(transIN, GPARM32_0 + ProdIndx);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "BOSS3 product_num = [%d]", rowNum);

	for (i=0; i < rowNum; i++)
	{
		memset(grpUserMsg->product_code, 0, sizeof(grpUserMsg->product_code));
		if(Fget32(transIN, GPARM32_0+ProdIndx, i, grpUserMsg->product_code, NULL) == -1)
		{
			sprintf(grpUserMsg->return_message, "�õ���Ʒ�������!");
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190043", "%s",grpUserMsg->return_message);
			return 190043;
		}
		Trim(grpUserMsg->product_code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "grpUserMsg->product_code=[%s]", grpUserMsg->product_code);
		if(strlen(grpUserMsg->product_code) == 0)continue;

		/*add by zhangwm 2009-5-13 ���Ų�Ʒ���ʹ��һ��ʱ�� BEGIN*/
		if(strcmp(vSm_Code,"98")==0 || strcmp(vSm_Code,"44")==0)
		{
			EXEC SQL SELECT trim(field_value) INTO :favbizCode FROM dgrpusermsgadd WHERE id_no=:vGrp_Id AND field_code='bcode';
			if(SQLCODE != 0)
			{
				sprintf(grpUserMsg->return_message, "��ѯdgrpusermsgadd����![%d]",SQLCODE);
				printf("%s-- [%d][%s]\n",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				return 190090;
			}
		}
		else
		{
			strcpy(favbizCode,grpUserMsg->product_code);
		}

		Trim(favbizCode);

		favcount=0;
		EXEC SQL SELECT count(*) INTO :favcount FROM sproductfavcfg WHERE product_code=:favbizCode;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(grpUserMsg->return_message, "��ѯsproductfavcfg����![%d]",SQLCODE);
			printf("%s-- [%d][%s]\n",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190091;
		}

		if(favcount > 0)
		{
			EXEC SQL SELECT nvl(fav_month,0) INTO :favMonth FROM sproductfavcfg WHERE product_code=:favbizCode;
			if(SQLCODE != 0)
			{
				sprintf(grpUserMsg->return_message, "��ѯsproductfavcfg����![%d]",SQLCODE);
				printf("%s-- [%d][%s]\n",grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				return 190092;
			}
		}
		else
		{
			favMonth=0;
		}

		/*add by zhangwm 2009-5-13 ���Ų�Ʒ���ʹ��һ��ʱ�� END*/
		/* ���ò�Ʒ�Ƿ���ϲ�Ʒ lixgAdd 20070328 */
		memset(vCombFlag, 0, sizeof(vCombFlag));
		EXEC SQL select COMB_FLAG into :vCombFlag
			from sProductCode
			where product_code = :grpUserMsg->product_code and end_time >= sysdate;
		if (SQLCODE != 0)
		{
			sprintf(grpUserMsg->return_message, "����Ʒ�Ƿ���ϲ�Ʒ����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","191122",grpUserMsg->return_message);
			return 191122;
		}

		if (strcmp(vCombFlag, "0") == 0)
		{
			sprintf(sqlStr, "select product_code from sProductCode where product_code = :v1 and end_time > sysdate");
		}
		else
		{
			sprintf(sqlStr, "select product_code from sProductCombInfo where comb_id = :v1 and end_time > sysdate");
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "Debug:sqlStr[%s]", sqlStr);
#endif

		memset(vSubProductCode, 0, sizeof(vSubProductCode));
		EXEC SQL PREPARE pre FROM :sqlStr;
		EXEC SQL DECLARE curCombProd CURSOR FOR pre;
		EXEC SQL OPEN curCombProd USING :grpUserMsg->product_code;

		while (1)
		{
			EXEC SQL FETCH curCombProd INTO :vSubProductCode;
			if(SQLCODE != 0) break;

			init(sqlStr);
			sprintf(sqlStr, "select b.product_code, a.service_type, b.service_code, b.main_flag, b.fav_order, c.price_code from sSrvCode a, sProductSrv b, sSrvPrice c where a.service_code = b.service_code and a.service_code = c.service_code and b.product_code = :v1");

#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "+++++++++++Debug:sqlStr[%s]", sqlStr);
#endif

			EXEC SQL PREPARE stmt002 FROM :sqlStr;
			if (SQLCODE != OK) {
				sprintf(grpUserMsg->return_message, "������Ʒ�����ϵ�α����![%d]",SQLCODE);
				PUBLOG_DBDEBUG("fpubCreateGrpUser");
				pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190020",grpUserMsg->return_message);
				return 190020;
			}

			EXEC SQL DECLARE srv_cur CURSOR for stmt002;
			if(SQLCODE!=OK){
				sprintf(grpUserMsg->return_message, "��ѯ��Ʒ������Ӧ��ϵ����![%d]",SQLCODE);
				PUBLOG_DBDEBUG("fpubCreateGrpUser");
				pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190021",grpUserMsg->return_message);
				return 190021;
			}

			EXEC SQL OPEN srv_cur USING :vSubProductCode;
			if(SQLCODE!=OK){
				EXEC SQL CLOSE srv_cur;
				sprintf(grpUserMsg->return_message, "�򿪲�Ʒ������Ӧ��ϵ�α����![%d]",SQLCODE);
				PUBLOG_DBDEBUG("fpubCreateGrpUser");
				pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190022",grpUserMsg->return_message);
				return 190022;
			}

			init(vProduct_Code);
			EXEC SQL FETCH srv_cur INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;

			if (SQLCODE != 0)
			{
				EXEC SQL CLOSE srv_cur;
				strcpy(grpUserMsg->return_message, "���Ų�Ʒ��Ϣ���ô���");
				PUBLOG_DBDEBUG("fpubCreateGrpUser");
				pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190022",grpUserMsg->return_message);
				return 190022;

			}

			while (SQLCODE == 0)
			{
				/*��¼�����û�������Ʒ��,BOSS�������*/
#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpSrvMsgHis����Ϣ");
#endif
				EXEC SQL INSERT INTO dGrpSrvMsgHis
					(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
					SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
					MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
					OP_TIME,   UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
					UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,use_flag, person_flag, UNVAILD_TIME)
				VALUES
					(:vGrp_Id,    :vService_Type, :vService_Code, :vPrice_Code,
					:vFav_Order, :vProduct_Code, add_months(sysdate,:favMonth), To_Date(:vSrv_Stop, 'YYYYMMDD'),
					:vMain_Flag, :lLoginAccept,  :vLogin_No,     :vOp_Code,
					sysdate,     :vLogin_No,     :lLoginAccept,  :vTotal_Date,
					sysdate,     :vOp_Code,       'A', 'Y', 'N',To_Date(:vSrv_Stop, 'YYYYMMDD'));
				if(SQLCODE!=OK)
				{
					sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsgHis�����![%d]",SQLCODE);
					PUBLOG_DBDEBUG("fpubCreateGrpUser");
					pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190023",grpUserMsg->return_message);
					return 190023;
				}
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "ins dGrpSrvMsgHis sqlcode = [%d]",SQLCODE);

				/* �����Ƿ���Ի��Żݱ�ʶ
				**personFlag[0] = 'N';
				**for(i=0; i < productPricesCount; i++){
				**	if (strncmp(serviceCode, tProductPrices[i].serviceCode, 4) == 0 &&
				**		strncmp(actualPrice, tProductPrices[i].priceCode, 4) == 0){
				**		personFlag[0] = 'Y';
				**		break;
				**	}
				**} */

				if((vService_Type[0] == '0') && (strcmp(vSm_Code,"98")==0 || strcmp(vSm_Code,"44")==0))
				{
					#ifdef _DEBUG_
					pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dBaseFav����Ϣ");
					#endif
					Trim(vGrp_Id);
					/*ng����
					**EXEC SQL	INSERT INTO dBaseFav
					**(
					**msisdn,region_code,fav_brand,fav_type,flag_code,fav_order, fav_day,
					**start_time,end_time,id_no
					**)
					**select
					**:vGrp_UserNo,substr(:vBelong_Code,1,2),:vSm_Code,a.service_code,a.service_code,a.fav_order,'0',
					**to_date(:vOp_Time,'YYYYMMDD HH24:MI:SS'),to_date(:vSrv_Stop,'YYYYMMDD HH24:MI:SS'),:vGrp_Id
					**from sproductsrv a,ssrvcode b
					**where a.product_code=:vProduct_Code and a.service_code=b.service_code
					**and b.service_type='0';
					**if(SQLCODE!=OK){
					**sprintf(grpUserMsg->return_message, "��¼dBaseFav�����![%d]",SQLCODE);
					**PUBLOG_DBDEBUG("fpubCreateGrpUser");
					**pubLog_DBErr(_FFL_,"fpubCreateGrpUser","192224",grpUserMsg->return_message);
					**return 192224;
					**}
					**ng����*/
					/*ng���� by yaoxc begin*/
					init(sTempSqlStr);
					sprintf(sTempSqlStr,"select :vGrp_UserNo,substr(:vBelong_Code,1,2), "
										":vSm_Code,a.service_code, "
										"a.service_code,a.fav_order,'0', "
										":vOp_Time, "
										":vSrv_Stop,nvl('',chr(0)),'0','0',:vGrp_Id,'0','0' "
										"from sproductsrv a,ssrvcode b "
										"where a.product_code=:vProduct_Code and a.service_code=b.service_code "
										"and b.service_type='0'");
					EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
					EXEC SQL DECLARE ngcur04 cursor for sql_str;
					EXEC SQL OPEN ngcur04 using :vGrp_UserNo,:vBelong_Code,:vSm_Code,:vOp_Time,:vSrv_Stop,:vGrp_Id,:vProduct_Code;
					for(;;)
					{
						memset(&tdBaseFav,0,sizeof(tdBaseFav));
						EXEC SQL FETCH ngcur04 INTO :tdBaseFav;

						if((0!=SQLCODE) && (1403!=SQLCODE))
						{
							sprintf(grpUserMsg->return_message, "��ѯsproductsrv,ssrvcode����\n");
							printf("sTempSqlStr=[%s]\n",sTempSqlStr);
							EXEC SQL CLOSE ngcur04;
							return -1;
						}
						if(1403==SQLCODE)
						{
							break;
						}

						v_ret=OrderInsertBaseFav(ORDERIDTYPE_USER,vGrp_Id,100,
												vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
												tdBaseFav);
						printf("OrderInsertBaseFav ,ret=[%d]\n",v_ret);
						if(0!=v_ret)
						{
							sprintf(grpUserMsg->return_message, "��¼dBaseFav�����![%d]",SQLCODE);
							PUBLOG_DBDEBUG("fpubCreateGrpUser");
							pubLog_DBErr(_FFL_,"fpubCreateGrpUser","192224",grpUserMsg->return_message);
							EXEC SQL CLOSE ngcur04;
							return 192224;
						}
					}
					EXEC SQL CLOSE ngcur04;
					/*ng���� by yaoxc end*/
					/* �ͼƷ�ȷ��һ��,����"aa00"д���᲻����Ӱ��? */
#ifdef _CHGTABLE_
					EXEC SQL INSERT INTO wBaseFavChg
						(
							msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,fav_day,
							start_time,end_time,flag,deal_time,id_no
						)
					select
						:vGrp_UserNo,substr(:vBelong_Code,1,2),:vSm_Code,a.service_code,a.service_code,a.fav_order,'0',
						to_date(:vOp_Time,'YYYYMMDD HH24:MI:SS'),to_date(:vSrv_Stop,'YYYYMMDD HH24:MI:SS'),
						'1', to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'),:vGrp_Id
					from sproductsrv a,ssrvcode b
						where a.product_code=:vProduct_Code and a.service_code=b.service_code
						and b.service_type='0';
					if(SQLCODE!=OK)
					{
						sprintf(grpUserMsg->return_message, "��¼wBaseFavChg�����![%d]",SQLCODE);
						PUBLOG_DBDEBUG("fpubCreateGrpUser");
						pubLog_DBErr(_FFL_,"fpubCreateGrpUser","191124",grpUserMsg->return_message);
						return 191124;
					}
#endif
				}

#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpSrvMsg����Ϣ");
#endif
				EXEC SQL INSERT INTO dGrpSrvMsg
						(ID_NO, SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
						SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
						MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
						OP_TIME, use_flag, person_flag, UNVAILD_TIME)
					VALUES
						(:vGrp_Id, :vService_Type, :vService_Code, :vPrice_Code,
						:vFav_Order, :vProduct_Code, add_months(sysdate,:favMonth),
						To_Date(:vSrv_Stop, 'YYYYMMDD'),
						:vMain_Flag, :lLoginAccept, :vLogin_No, :vOp_Code,
						sysdate, 'Y', 'N',To_Date(:vSrv_Stop, 'YYYYMMDD'));
				if(SQLCODE!=OK)
				{
					sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsg�����![%d]",SQLCODE);
					PUBLOG_DBDEBUG("fpubCreateGrpUser");
					pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190024",grpUserMsg->return_message);
					return 190024;
				}
#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "ins dGrpSrvMsg sqlcode = [%d]",SQLCODE);
#endif
				EXEC SQL FETCH srv_cur INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
			}
			EXEC SQL CLOSE srv_cur;
		}
		EXEC SQL CLOSE curCombProd;
	} /* end rowNum */
#endif

#if PROVINCE_RUN == PROVINCE_SHANXI
	rowNum = Foccur32(transIN, GPARM32_0 + SrvIndx);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "rowNum:%d",rowNum);
	for (i=0; i < rowNum; i++)
	{
		memset(vProduct_Type, 0, sizeof(vProduct_Type));
		if(Fget32(transIN,GPARM32_0+SrvIndx, i, vProduct_Type, NULL) == -1)
		{
			sprintf(grpUserMsg->return_message, "�õ���Ʒ���ͳ���!");
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s",grpUserMsg->return_message);
			return 190046;
		}
		Trim(vProduct_Type);
		if (vProduct_Type[0] == '\0' || strncmp(vProduct_Type, "��", 2) != 0)
		{
			continue;
		}

		memset(vService_Code, 0, sizeof(vService_Code));
		if(Fget32(transIN,GPARM32_1+SrvIndx, i, vService_Code, NULL) == -1)
		{
			sprintf(grpUserMsg->return_message, "�õ�����������!");
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190047", "%s",grpUserMsg->return_message);
			return 190047;
		}
		Trim(vService_Code);

		memset(vService_Attr, 0, sizeof(vService_Attr));
		if(Fget32(transIN,GPARM32_2+SrvIndx, i, vService_Attr, NULL) == -1)
		{
			sprintf(grpUserMsg->return_message, "�õ��������Գ���!");
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "190048", "%s",grpUserMsg->return_message);
			return 190048;
		}
		Trim(vService_Attr);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "210 beginService");
		if ( (retCode = beginService(vGrp_UserNo,vService_Code,vServvice(vGrp_UserNo,vService_Code,vServrvice(vGrp_UserNo,vService_Code,vService_Attr,grpUserMsg->op_time,MAXENDTIME,
													vOp_Code,vLogin_No,strLoginAccept,grpUserMsg->op_time,grpUserMsg->return_message)) != 0)
		{
			sprintf(grpUserMsg->return_message, "�������[%s]ʧ��!", vService_Code);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", retCode, "%s",grpUserMsg->return_message);
			return retCode;
		}
	}
#endif

	/* ��¼�ⲿ���� */
	/* ng����  ҵ�񹤵�����  by liuzhou 20090930*/
	/*
	**if(0 != pubInsGrpOutSideFee(vGrp_Id, grpUserMsg)){
	**	sprintf(grpUserMsg->return_message, "��¼wGrpOutSideFee�����!");
	**	pubLog_Debug(_FFL_, "fpubCreateGrpUser","190025", "%s",grpUserMsg->return_message);
	**	return 190025;
	**}
	*/

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","[pubInsGrpOutSideFee ...[%d].]",atoi(grpUserMsg->feeList.iFeeCount));
#endif

	for(i=0; i< atoi(grpUserMsg->feeList.iFeeCount); i++)
	{
		printf("grpUserMsg->feeList.charge_flag[%d] == [%s]\n", i, grpUserMsg->feeList.charge_flag[i]);
		if(grpUserMsg->feeList.charge_flag[i][0] == '2')
		{
			/*
			**memset(feeCode, 0, sizeof(feeCode));
			**memset(detailCode, 0, sizeof(detailCode));
			**strcpy(feeCode, grpUserMsg->feeList.fee_code[i]);
			**strcpy(detailCode, grpUserMsg->feeList.detail_code[i]);
			**real_fee = atof(grpUserMsg->feeList.action_fee[i]);
			**
			**
			**EXEC SQL insert into wGrpOutSideFee
			**	(CONTRACT_NO, USER_NO, FEE_CODE, DETAIL_CODE, SHOULD_PAY, BILL_YM)
			**	values(
			**	:contractNo, :userNo, :feeCode, :detailCode, :real_fee, :billYM);
			**if(SQLCODE != 0)
			**{
			**	PUBLOG_DBDEBUG("");
			**	pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
			**	return -2001;
			**}
			*/
			sprintf(sFeeCodeList, "%s%s~", sFeeCodeList, grpUserMsg->feeList.fee_code[i]);
			sprintf(sDetailCodeList, "%s%s~", sDetailCodeList, grpUserMsg->feeList.detail_code[i]);
			sprintf(sRealFeeList, "%s%s~", sRealFeeList, grpUserMsg->feeList.action_fee[i]);
			nFeeCount++;
		}
	}
	/* ng����  ҵ�񹤵�����  by liuzhou 20090930*/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","[pubInsGrpOutSideFee end....]");
#endif

	/* SI����ԭ��¼�� */
	EXEC SQL insert into  dParterProdBusiMsg
		(PARTER_ID, ID_NO, BUSI_TYPE, BUSI_ATTR, SPLIT_MONEY)
		select
		PARTER_ID, ID_NO, BUSI_TYPE, BUSI_ATTR, SPLIT_MONEY
		from dMidPTProdBusiMsg
		where id_no = to_number(:vGrp_Id);
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		sprintf(grpUserMsg->return_message, "��¼dMidPTProdBusiMsg�����[%d]!", SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190035",grpUserMsg->return_message);
		return 190035;
	}

	/* ng����  ҵ�񹤵�����  by liuzhou 20090930*/
	Trim(feeList);
	if ( strToFields(feeList, '~', payType,checkNo,bankCode,checkPay,cashPay,shouldHandFee,handFee) != 7)
	{
		strcpy(grpUserMsg->return_message, "��ȡ�ͻ���Ϣ������!");
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190036",grpUserMsg->return_message);
		return 190036;
	}
	Trim(payType);
	Trim(checkNo);
	Trim(bankCode);
	Trim(checkPay);
	Trim(cashPay);
	Trim(shouldHandFee);
	Trim(handFee);
	if (payType[0] == '\0')
	{
		payType[0] = '0';
	}

	if (atof(checkPay) >= 0.05)
	{
#if PROVINCE_RUN != PROVINCE_JILIN
		if ((ret = pubOpenAcctCheck(checkNo,   atof(checkPay),  bankCode,
						vOp_Note,    vLogin_No,  strLoginAccept,
						vOp_Code,    vTotal_Date, vOp_Time,
						systemNote,  vOrgCode,  vOrg_Id)) != 0)
#else
		/*��֯������������org_code,org_id  edit by  zhangzhuo  26/05/2009  begin */
		if ((ret = pubOpenAcctCheck(checkNo,   atof(checkPay),  bankCode,
						vOp_Note,    vLogin_No,  strLoginAccept,
						vOp_Code,    vTotal_Date, vOp_Time,
						systemNote,  vOrgCode, vOrg_Id)) != 0)
		/*��֯������������org_code,org_id  edit by  zhangzhuo  26/05/2009  end  */
#endif
		{
			strcpy(grpUserMsg->return_message, "֧Ʊ�������!");
			PUBLOG_DBDEBUG("fpubCreateGrpUser");
			pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190037",grpUserMsg->return_message);
			return 190037;
		}
		strcpy(sChkFlag, "Y");
	}
	
	
	/*ng ���� ҵ�񹤵����� by liuzhou 20090930 begin*/
	v_ret = 0;
	memset(vParamList, 0, MAX_PARMS_NUM*2*DLINTERFACEDATA);

	strcpy(vCallServiceName,"bodb_accoutchk");
	vSendParaNum=13; /* ҵ�񹤵���������������� */
	vRecpParaNum=2;/* ҵ�񹤵���������������� */

	strcpy(vParamList[0][0], "contract_no");
	strcpy(vParamList[0][1], vAccount_Id);

	strcpy(vParamList[1][0], "begin_dt");
	strcpy(vParamList[1][1], total_date);

	strcpy(vParamList[2][0], "end_dt");
	strcpy(vParamList[2][1], vSrv_Stop);

	strcpy(vParamList[3][0], "fee_code");
	strcpy(vParamList[3][1], sFeeCodeList);

	strcpy(vParamList[4][0], "detail_code");
	strcpy(vParamList[4][1], sDetailCodeList);

	strcpy(vParamList[5][0], "real_fee");
	strcpy(vParamList[5][1], sRealFeeList);

	strcpy(vParamList[6][0], "fee_count");
	sprintf(vParamList[6][1], "%d", nFeeCount);

	strcpy(vParamList[7][0], "user_no");
	strcpy(vParamList[7][1], vGrp_UserNo);

	strcpy(vParamList[8][0], "op_time");
	strcpy(vParamList[8][1], vOp_Time);

	strcpy(vParamList[9][0], "chk_flag");
	strcpy(vParamList[9][1], sChkFlag);

	strcpy(vParamList[10][0], "bank_code");
	strcpy(vParamList[10][1], bankCode);

	strcpy(vParamList[11][0], "check_no");
	strcpy(vParamList[11][1], checkNo);

	strcpy(vParamList[12][0], "check_pay");
	strcpy(vParamList[12][1], checkPay);

	v_ret = SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
								ORDERIDTYPE_USER, vGrp_Id, iOrderRight, strLoginAccept, vOp_Code, vLogin_No, vOp_Note);
	if(0 != v_ret)
	{
		sprintf(grpUserMsg->return_message, "����ҵ�񹤵�����[%d]!", v_ret);
		PUBLOG_DBDEBUG("fpubCreateGrpUser");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser","190038",grpUserMsg->return_message);
		return 190038;
	}
	/*ng ���� ҵ�񹤵����� by liuzhou 20090930 end  */

	return 0;
}

/************************************************************************
��������:fpubDelGrpUser
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û�ɾ�����ú���
�������:�����û�id
		 ����Ա����
		 ��������
		 ����ʱ��
		 ������ˮ��
		 ������ע
		 ������Ϣ
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubDelGrpUser(char *inGrpId_No, char *login_no, char *op_code, char *op_time,
	long lSysAccept, char *op_note, char *return_message)
{
	int  retCode=0;
	TDeleteGrpUser stDelGrpUserMsg;

	EXEC SQL BEGIN DECLARE SECTION;
		 int  vCnt = 0;
		 int  isBillFlag=0;    /*���Ų�Ʒ�Ƿ�Ʒѱ�־*/
		 int  AcctFlag=0;      /*�Ƿ�ɾ�������ʻ���־*/
		 long lLoginAccept = 0;
		 char strLoginAccept[14+1];
		 char vGrpCust_Id[22+1];
		 char vGrpId_No[22+1];
		 char vGrpUser_No[20+1];
		 char vContract_No[14+1];
		 char vLogin_No[6+1];
		 char vOp_Code[4+1];
		 char vTotal_Date[8+1];
		 char vOp_Time[24+1];
		 char vLastMonth1[24+1];   /*����1��*/
		 char vOp_Note[60+1];
		 char vSm_Code[2+1];
		 char vProduct_Code[8+1];
		 char vGrpProduct_Code[2+1];
		 char vBelong_Code[9+1];
		 char vGroup_Id[10+1];
		 char vOrg_Id[10+1];
		 char vMember_Id[14+1];
		 char vMember_No[15+1];
		 char vOld_LoginAccept[14+1];
		 int  ret;
	EXEC SQL END DECLARE SECTION;

	/*ng���� by yaoxc begin*/
	int v_ret=0;
	TdCustConMsgIndex tdCustConMsgIndex;
	TdCustMsgDead tdCustMsgDead;
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	/*ng���� by yaoxc end*/

	/*ng ���� ҵ�񹤵�����  ���Ӳ���*/
	char  vCallServiceName[30+1];
	int   vSendParaNum = 0;
	int   vRecpParaNum = 0;
	char  vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA];
	const int iOrderRight = 100;
	init(vCallServiceName);

	init(strLoginAccept   );
	init(vGrpCust_Id      );
	init(vGrpId_No        );
	init(vGrpUser_No      );
	init(vContract_No     );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vOp_Time         );
	init(vOp_Note         );
	init(vSm_Code         );
	init(vProduct_Code    );
	init(vGrpProduct_Code );
	init(vOld_LoginAccept );

	/*ng���� by yaoxc begin*/
	memset(&tdCustConMsgIndex,0,sizeof(tdCustConMsgIndex));
	memset(&tdCustMsgDead,0,sizeof(tdCustMsgDead));
	init(v_parameter_array);
	init(sTempSqlStr);
	/*ng���� by yaoxc end*/

	/*������ֵ*/
	lLoginAccept = lSysAccept;
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "call publicGrp.cp--------fubDelGrpUser");
	sprintf(strLoginAccept, "%ld", lLoginAccept);
	strcpy(vGrpId_No,   inGrpId_No);
	strcpy(vLogin_No,   login_no  );
	strcpy(vOp_Code,    op_code   );
	strcpy(vOp_Time,    op_time   );
	strcpy(vOp_Note,    op_note   );
	strncpy(vTotal_Date, vOp_Time, 8);
	Trim(vGrpId_No);
	Trim(vOp_Time);

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "lLoginAccept  =[%d]",   lLoginAccept  );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "strLoginAccept=[%s]",   strLoginAccept);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vGrpId_No     =[%s]",   vGrpId_No     );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vLogin_No     =[%s]",   vLogin_No     );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vOp_Code      =[%s]",   vOp_Code      );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vOp_Time      =[%s]",   vOp_Time      );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vOp_Note      =[%s]",   vOp_Note      );
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "vTotal_Date   =[%s]",   vTotal_Date   );
	#endif

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "��ѯdGrpUserMsg����Ϣ");
	#endif
	EXEC SQL SELECT Cust_Id, User_No, Sm_Code, Product_Code, Account_Id,
					region_code||district_code||town_code, Group_Id,
					to_char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD HH24:MI:SS')
			   INTO :vGrpCust_Id, :vGrpUser_No, :vSm_Code, :vProduct_Code, :vContract_No,
					:vBelong_Code, :vGroup_Id,
					:vLastMonth1
			   FROM dGrpUserMsg
			  WHERE ID_NO = To_Number(:vGrpId_No)
				AND Bill_Date > Last_Day(sysdate) + 1;
	if(SQLCODE == 1403){
		sprintf(return_message,"�����û�[%s]������",vGrpId_No);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190101",return_message);
		return 190101;
	}
	else if(SQLCODE!=OK){
		sprintf(return_message,"��ѯdGrpUserMsg����Ϣ[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190102",return_message);
		return 190102;
	}
	Trim(vGrpCust_Id  );
	Trim(vGrpUser_No);
	Trim(vSm_Code);
	Trim(vProduct_Code);
	Trim(vContract_No);
	Trim(vBelong_Code);
	Trim(vGroup_Id);
	Trim(vLastMonth1);

	/*��ѯ����Ա��org_id*/
#if PROVINCE_RUN == PROVINCE_JILIN
	   /* EXEC SQL select 'unknown'*/
		  EXEC SQL select group_id
				into :vOrg_Id
				from dLoginMsg where login_no=:vLogin_No;
#else
		EXEC SQL select org_id
				into :vOrg_Id
				from dLoginMsg where login_no=:vLogin_No;
#endif /*PROVINCE_RUN != PROVINCE_JILIN*/
		if(SQLCODE!=0)
		{
			sprintf(return_message,"��ѯdLoginMsg��Org_Idʧ��[%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190103",return_message);
			return 190103;
		}

		/* ���ٴ���VIP��ر� chenxuan boss3
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "��ѯ���ſͻ�ϵͳ�Ĳ�Ʒ����");
#endif

	EXEC SQL SELECT Product_Code INTO :vGrpProduct_Code
			   FROM sGrpSmCode
			  WHERE Sm_Code = :vSm_Code;
	if(SQLCODE!=OK){
		sprintf(return_message,"��ѯsGrpSmCode���ſͻ�ϵͳ��Ʒ�������[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190104;
	}
	Trim(vGrpProduct_Code);

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "������״̬����Ϊ��Ч");
	#endif
	EXEC SQL UPDATE dGrpOrderGoods
				SET Curr_Status = '2'
			  WHERE Product_Id in
				 ( SELECT Product_Id FROM dGrpProduct
					WHERE Cust_Id = To_Number(:vGrpCust_Id)
					  AND Product_Code = :vGrpProduct_Code
				  );
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		sprintf(return_message, "���¶�����״̬����![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s|vGrpCust_Id:%s,vGrpProduct_Code:%s",return_message,vGrpCust_Id,vGrpProduct_Code);
		return 190105;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "������Ϣ��dGrpProductHis��");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "INSERT INTO dGrpProductHis ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", " (CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  APN1,           APN2,              APN3,          INPUT_DATE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME, ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  UPDATE_CODE,  UPDATE_TYPE) ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", " SELECT ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  APN1,           APN2,              APN3,          INPUT_DATE,  ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  '%s', %d, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), ", vLogin_No, lLoginAccept, vTotal_Date, vOp_Time);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  '%s',     'D' ", vOp_Code);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", " FROM dGrpProduct ");
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "WHERE Cust_Id = To_Number('%s') ", vGrpCust_Id);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  AND Product_Code = '%s'; ", vGrpProduct_Code);
	#endif

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "��¼dCustDocOrgAddHis���¼");
#endif
		EXEC SQL BEGIN DECLARE SECTION;
			char sIn_Grp_OrgCode[10+1];
		EXEC SQL END DECLARE SECTION;
		init(sIn_Grp_OrgCode);
		EXEC SQL SELECT Org_Code
		   INTO :sIn_Grp_OrgCode
		   FROM dCustDocOrgAdd
		   WHERE Cust_Id = To_Number(:vGrpCust_Id);
		if(SQLCODE!=OK)
		{
		sprintf(return_message,":sIn_Grp_OrgCode��ѯʧ��",vGrpId_No);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190036 ;
		}

	EXEC SQL INSERT INTO dGrpProductHis
			  (CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,
			   PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,
			   BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,
			   PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,
			   BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,
			   NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,
			   CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,
			   APN1,           APN2,              APN3,          INPUT_DATE,
			   UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
			   UPDATE_CODE,  UPDATE_TYPE,   ORG_CODE)
			  SELECT
			   CUST_ID,        UNIT_ID,           UNIT_CODE,     PRODUCT_ID,
			   PRODUCT_CODE,   CONTRACT_ID,       CONTRACT_NO,   ORDER_TIME,
			   BEGIN_TIME,     END_TIME,          SOLVE_CODE,    SOLVE_PROJECT,
			   PRODUCT_STATUS, RESOURCE_CODE,     RESOURCE_INFO, BILL_CODE,
			   BILL_INFO,      OPERATION_CODE,    OTHER_OPERATION_CODE, PRODUCT_DESC,
			   NOTE,           BIG_CUSTOMER_CODE, FEE_PHONE,     INCOME_BILL_CODE,
			   CARD_ID,        TREATY_FILE_NAME,  FILE_FORMAT,   TELE,
			   APN1,           APN2,              APN3,          INPUT_DATE,
			   :vLogin_No,   :lLoginAccept,   :vTotal_Date, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
			   :vOp_Code,     'D',            :sIn_Grp_OrgCode
			  FROM dGrpProduct
			 WHERE Cust_Id = To_Number(:vGrpCust_Id)
			   AND Product_Code = :vGrpProduct_Code;
   if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		sprintf(return_message, "����dGrpProductHis����![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190106;
	}
	EXEC SQL UPDATE dGrpProduct
				SET Product_Status = '2'
			  WHERE Cust_Id = To_Number(:vGrpCust_Id)
				AND Product_Code = :vGrpProduct_Code;
   if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		sprintf(return_message, "��dGrpProduct���¼��Ϊ��Чʧ��![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190107;
	}*/

	/*�����񶨳�ԤԼɾ��������1����Ч*/
#if PROVINCE_RUN == PROVINCE_JILIN
		memset(&stDelGrpUserMsg, 0, sizeof(stDelGrpUserMsg));
	  sprintf(stDelGrpUserMsg.loginAccept, "%ld", lLoginAccept);
		strcpy(stDelGrpUserMsg.grpIdNo, vGrpId_No);
		strcpy(stDelGrpUserMsg.opCode,vOp_Code);
		strcpy(stDelGrpUserMsg.opTime,vOp_Time);
		strcpy(stDelGrpUserMsg.loginNo,vLogin_No);
		strcpy(stDelGrpUserMsg.productCode,vGrpProduct_Code);
		strcpy(stDelGrpUserMsg.effectTime,vLastMonth1);
		if ( (retCode = endGrpAllProductService(stDelGrpUserMsg, return_message)) != 0)
	{
			pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "���� endGrpProductService ʧ�ܣ�retCode[%d]", retCode);
		return retCode;
	}
#else
	if ( (retCode=cancelAllBill(vGrpId_No, vGrpUser_No, vLastMonth1, vOp_Code, vLogin_No, strLoginAccept, "dGrpSrvMsg", return_message)) != 0)
	{
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "����cancelAllBillʧ�ܣ�retCode[%d]", retCode);
		return retCode;
	}
#endif

	/*����dAccountIdInfo��¼*/
	EXEC SQL INSERT INTO dAccountIdInfoHis
	  (
	  MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE,
	  UPDATE_LOGIN,UPDATE_DATE,UPDATE_TIME,UPDATE_ACCEPT,UPDATE_CODE,UPDATE_TYPE
	  )
	  SELECT
	  MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE,
	  :vLogin_No, to_number(:vTotal_Date), to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:lLoginAccept,:vOp_Code,'a'
	  FROM dAccountIdInfo
	  WHERE msisdn = :vGrpUser_No;
	if (SQLCODE != 0)
	{
		sprintf(return_message, "�����dAccountIdInfoHis����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190108",return_message);
		return 190108;
	}

#ifdef _CHGTABLE_
	EXEC SQL INSERT INTO wAccountIdInfoChg
	  (MSISDN,SERVICE_TYPE,SERVICE_NO,UPDATE_TYPE,RUN_CODE,OP_TIME)
	  SELECT
	  MSISDN, SERVICE_TYPE, SERVICE_NO, 'D', 'Aa',to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS')
	  FROM dAccountIdInfo
	  WHERE msisdn = :vGrpUser_No;
	if (SQLCODE != 0) {
		sprintf(return_message, "�����wAccountIdInfoChg����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190109",return_message);
		return 190109;
	}
#endif
	/*ng����
	EXEC SQL DELETE FROM dAccountIdInfo WHERE msisdn = :vGrpUser_No;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(return_message, "ɾ��dAccountIdInfo�����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190110",return_message);
	  return 190110;
	}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);

	strcpy(v_parameter_array[0],vGrpUser_No);

	v_ret=OrderDeleteAccountIdInfo(ORDERIDTYPE_USER,vGrpId_No,100,
							vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							vGrpUser_No,"",v_parameter_array);
	printf("OrderDeleteAccountIdInfo ,ret=[%d]\n",v_ret);
	if(0 > v_ret)
	{
		sprintf(return_message, "ɾ��dAccountIdInfo�����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190110",return_message);
		return 190110;
	}
	/*ng���� by yaoxc end*/
	/*����Ƿ��м��ſͻ��������û����ø��ʻ�*/
	AcctFlag = 0;
	EXEC SQL SELECT Count(*) INTO :AcctFlag
	  FROM dGrpUserMsg
	  WHERE Cust_Id = :vGrpCust_Id AND Account_Id = :vContract_No;
	if (SQLCODE != 0) {
		sprintf(return_message, "��ѯdGrpUserMsg���ʻ�[%s]ʧ��![%d]", vContract_No, SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190111",return_message);
		return 190111;
	}

		/*û�����������û����ø��ʻ�������ɾ�����ʻ�*/
	if (AcctFlag == 1) {
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dConMsgHis
		  (
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,GROUP_ID,
				  Update_Code,    Update_Date,  Update_Time,
				  Update_Login,   Update_Accept,Update_Type
		  )
		  SELECT
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,GROUP_ID,
				  :vOp_Code,      TO_NUMBER(:vTotal_Date),
				  TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),
				  TE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),
				  DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),
				  :vLogin_No,    :lLoginAccept,'a'
		FROM dConMsg
		WHERE contract_no = to_number(:vContract_No);
#else
		EXEC SQL INSERT INTO dConMsgHis
		  (
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,
				  Update_Code,    Update_Date,  Update_Time,
				  Update_Login,   Update_Accept,Update_Type,
				  Group_Id
		  )
		  SELECT
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,
				  :vOp_Code,      TO_NUMBER(:vTotal_Date),
				  TO_DATE(:vOp_Time,'YYYYMMDD HH24:MI:SS'),
				  :vLogin_No,    :lLoginAccept,'a',
				  group_id
		   FROM dConMsg
		  WHERE contract_no = to_number(:vContract_No);
#endif
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����dConMsgHis ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190112",return_message);
			return 190112;
		}

#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL INSERT INTO dConMsgDead
		  (
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,tract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,Group_Id
		  )
		  SELECT
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,Group_Id
		   FROM dConMsg
		  WHERE contract_no = to_number(:vContract_No);
#else
		EXEC SQL INSERT INTO dConMsgDead
		  (
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,Group_Id
		  )
		  SELECT
				  Contract_No,    Con_Cust_Id,  Contract_Passwd,
				  Bank_Cust,      Oddment,      Belong_Code,
				  PrePay_FEE,     Prepay_Time,  Status,
				  Status_Time,    Post_Flag ,   Deposit ,
				  Min_Ym,         Owe_Fee,      Account_Mark,
				  Account_Limit,  Pay_Code,     Bank_Code,
				  Post_Bank_Code, Account_No,   Account_Type,Group_Id
		   FROM dConMsg
		  WHERE contract_no = to_number(:vContract_No);
#endif
		if (SQLCODE != 0) {
			sprintf(return_message, "����dConMsgDead ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190113",return_message);
			return 190113;
		}

/* �˵���ѯ�������˻� */

		EXEC SQL DELETE FROM dConMsg WHERE contract_no = to_number(:vContract_No);
		if (SQLCODE != 0)
		{
			sprintf(return_message, "ɾ��dConMsg ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190114",return_message);
			return 190114;
		}

		/*ng���� ҵ�񹤵�����  by liuzhou 20091001*/
		memset(vParamList, 0, MAX_PARMS_NUM*2*DLINTERFACEDATA);
		Trim(vOp_Time);

		strcpy(vCallServiceName, "bodb_DeldCon");
		vSendParaNum=5; /* ҵ�񹤵���������������� */
		vRecpParaNum=2;/* ҵ�񹤵���������������� */

		strcpy(vParamList[0][0], "contract_no");
		strcpy(vParamList[0][1], vContract_No);

		strcpy(vParamList[1][0], "login_no");
		strcpy(vParamList[1][1], vLogin_No);

		strcpy(vParamList[2][0], "login_accept");
		strcpy(vParamList[2][1], strLoginAccept);

		strcpy(vParamList[3][0], "op_time");
		strcpy(vParamList[3][1], vOp_Time);

		strcpy(vParamList[4][0], "op_code");
		strcpy(vParamList[4][1], vOp_Code);

		v_ret = 0;
		v_ret = SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
					ORDERIDTYPE_USER, vGrpId_No, iOrderRight, strLoginAccept, vOp_Code, vLogin_No, vOp_Note);
		if (0 != v_ret)
		{
			sprintf(return_message, "����ҵ�񹤵�����![%d]", v_ret);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190114",return_message);
			return 190114;
		}
		/*ng���� ҵ�񹤵�����  by liuzhou 20091001*/

		/*ng���� by yaoxc end*/
		EXEC SQL INSERT INTO dCustConMsgHis
		  (
				  Cust_Id,         Contract_No,     Begin_Time,
				  End_Time,        Update_Login,  Update_Accept,
				  Update_Date,     Update_Time,     Update_Code,
				  Update_Type
		  )
		  SELECT
				  Cust_Id,         Contract_No,     Begin_Time,   End_Time,
				  :vLogin_No, :lLoginAccept,to_number(:vTotal_Date), to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vOp_Code,'a'
		   FROM dCustConMsg
		  WHERE cust_id = to_number(:vGrpCust_Id)
			AND contract_no = to_number(:vContract_No);
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����dCustConMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190115",return_message);
			return 190115;
		}
		/*ng����
		EXEC SQL DELETE FROM dCustConMsg
		  WHERE cust_id = to_number(:vGrpCust_Id) AND contract_no = to_number(:vContract_No);
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(return_message, "ɾ��dCustConMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190116",return_message);
			return 190116;
		}
		ng����*/
		/*ng���� by yaoxc begin*/
		init(v_parameter_array);

		strcpy(tdCustConMsgIndex.sCustId,vGrpCust_Id);
		strcpy(tdCustConMsgIndex.sContractNo,vContract_No);

		strcpy(v_parameter_array[0],vGrpCust_Id);
		strcpy(v_parameter_array[1],vContract_No);

		v_ret=OrderDeleteCustConMsg(ORDERIDTYPE_USER,vGrpId_No,100,
								vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
								tdCustConMsgIndex,"",v_parameter_array);
		printf("OrderDeleteCustConMsg ,ret=[%d]\n",v_ret);
		if(0 > v_ret)
		{
			sprintf(return_message, "ɾ��dCustConMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190116",return_message);
			return 190116;
		}
		/*ng���� by yaoxc end*/
	}
	/*�ʻ��������*/

	EXEC SQL INSERT INTO dCustUserHis
	  (CUST_ID,ID_NO,REP_FLAG,CUST_FLAG,
	  UPDATE_LOGIN,UPDATE_TIME,UPDATE_ACCEPT,OP_CODE,UPDATE_TYPE
	  )
	  SELECT
	  CUST_ID,ID_NO,REP_FLAG,CUST_FLAG,
	  :vLogin_No, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:lLoginAccept,:vOp_Code,'a'
	  FROM dCustUserMsg
	  WHERE id_no = to_number(:vGrpId_No);
	if (SQLCODE != 0) {
		sprintf(return_message, "����dCustUserHis����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190117",return_message);
		return 190117;
	}

	EXEC SQL DELETE FROM dCustUserMsg WHERE id_no=to_number(:vGrpId_No);
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(return_message, "ɾ��dCustUserMsg����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190118",return_message);
		return 190118;
	}

	isBillFlag=0;
	EXEC SQL SElECT count(*) INTO :isBillFlag
	  FROM sProductAttr
	  WHERE product_attr = (select product_attr from sProductCode where product_code = :vProduct_Code)
	  AND isbill = 'Y';

	/* Ĭ�϶�����ϵ boss2.0 add 2006-11-16
	EXEC SQL select login_accept into :vOld_LoginAccept
		from dGrpSrvMsg where main_flag = 'Y' and id_no = :vGrpId_No;
	if(SQLCODE!=OK){
		sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190132;
	}

	Trim(vOp_Time);
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "pubBillEndMode begin...");
	retCode = pubBillEndMode(vGrpUser_No, DEFAULT_GRPTOPERSON_CODE, vOld_LoginAccept,
		vOp_Time, vOp_Code, vLogin_No, strLoginAccept, vOp_Time, return_message);
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "[%s][%d]:pubBillEndMode retCode = [%d]",__FILE__,__LINE__,retCode);
	if(0 != retCode){
#ifdef _DEBUG
	   pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s:[%s]", __FUNCTION__, return_message);
#endif
	   return 190133;
	}lixg */



	/* ���ڼ��Ų�Ʒȫ�����мƷ��¼� chenxuan boss3 */
	EXEC SQL INSERT INTO dCustMsgHis
	  (
	  ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	  RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,
	  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
	  UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_NO
	  )
	  SELECT
	  ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	  RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,
	  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
	  :lLoginAccept, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_number(:vTotal_Date),
	  :vLogin_No, 'd', :vOp_Code,GROUP_NO
	  FROM dCustMsg
	  WHERE id_no = to_number(:vGrpId_No);
	if (SQLCODE != 0) {
			sprintf(return_message, "����dCustMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190119",return_message);
			return 190119;
	}
	/*ng����
	EXEC SQL INSERT INTO dCustMsgDead
	  (
			  ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
			  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
			  RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
			  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
	  )
	  SELECT
			  ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
			  ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
			  RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
			  BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
	   FROM dCustMsg
	  WHERE id_no = to_number(:vGrpId_No);
	if(SQLCODE != 0)
	{
			sprintf(return_message, "����dCustMsgDead����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190120",return_message);
			return 190120;
				}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(sTempSqlStr);
	sprintf(sTempSqlStr,"select to_char(ID_NO),to_char(CUST_ID),to_char(CONTRACT_NO),to_char(IDS),PHONE_NO,SM_CODE,SERVICE_TYPE, "
						"ATTR_CODE,USER_PASSWD,to_char(OPEN_TIME,'YYYYMMDD HH24:MI:SS'),BELONG_CODE,to_char(LIMIT_OWE),CREDIT_CODE,to_char(CREDIT_VALUE), "
						"RUN_CODE,to_char(RUN_TIME,'YYYYMMDD HH24:MI:SS'),to_char(BILL_DATE,'YYYYMMDD HH24:MI:SS'),to_char(BILL_TYPE), "
						"ENCRYPT_PREPAY, to_char(CMD_RIGHT), "
						"LAST_BILL_TYPE,BAK_FIELD,nvl(GROUP_ID,chr(0)),nvl(GROUP_NO,chr(0)) "
						"from dCustMsg "
						"WHERE id_no = to_number(:vGrpId_No) ");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
	EXEC SQL DECLARE ngcur05 cursor for sql_str;
	EXEC SQL OPEN ngcur05 using :vGrpId_No;
	for(;;)
	{
		memset(&tdCustMsgDead,0,sizeof(tdCustMsgDead));
		EXEC SQL FETCH ngcur05 INTO :tdCustMsgDead;
		if((0!=SQLCODE) && (1403!=SQLCODE))
		{
			sprintf(return_message, "��ѯdCustMsgDead����!\n");
			EXEC SQL CLOSE ngcur05;
			return -1;
		}
		if(1403==SQLCODE) break;

		v_ret=OrderInsertCustMsgDead(ORDERIDTYPE_USER,vGrpId_No,100,
								vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
								tdCustMsgDead);
		printf("OrderInsertCustMsgDead ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			sprintf(return_message, "����dCustMsgDead����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190120",return_message);
			EXEC SQL CLOSE ngcur05;
			return 190120;
		}
	}
	EXEC SQL CLOSE ngcur05;
	/*ng���� by yaoxc end*/
	/*ng����
	EXEC SQL DELETE FROM dCustMsg WHERE id_no=:vGrpId_No;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
			sprintf(return_message, "ɾ��dCustMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190121",return_message);
			return 190121;
	}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);

	strcpy(v_parameter_array[0],vGrpId_No);

	v_ret=OrderDeleteCustMsg(ORDERIDTYPE_USER,vGrpId_No,100,
							  vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							  vGrpId_No,"",v_parameter_array);
	printf("OrderDeleteCustMsg ,ret=[%d]\n",v_ret);
	if(0 > v_ret)
	{
		sprintf(return_message, "ɾ��dCustMsg����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190121",return_message);
		return 190121;
	}
	/*ng���� by yaoxc end*/
#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
	EXEC SQL INSERT INTO wUserMsgChg
	  (
			  OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
			  ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
	  )
	  VALUES
	  (
			  sMaxBillChg.NEXTVAL, '3', TO_NUMBER(:vGrpId_No), TO_NUMBER(:vGrpCust_Id),:vGrpUser_No,:vSm_Code,
			  '00000000', :vBelong_Code, 'Aa',
			  SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
			  ||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),
			  SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
			  ||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),:vGroup_Id
	  );
#else
	EXEC SQL        INSERT INTO wUserMsgChg
	(
			OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
			ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
	)
	VALUES
	(
			sMaxBillChg.NEXTVAL, '3', TO_NUMBER(:vGrpId_No), TO_NUMBER(:vGrpCust_Id),:vGrpUser_No,:vSm_Code,
			'00000000', :vBelong_Code, 'Aa',
			SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
			||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),
			SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
			||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),:vGroup_Id
	);
#endif
	  if (SQLCODE != 0) {
			sprintf(return_message, "����wUserMsgChg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190122",return_message);
			return 190122;
	  }
#endif
		EXEC SQL SELECT count(member_id) into :vCnt
		FROM dGrpUserMebMsg
		WHERE Id_No = To_Number(:vGrpId_No) AND End_Time > Last_Day(sysdate) + 1;
	  if (SQLCODE != 0) {
			sprintf(return_message, "��ѯdGrpUserMebMsg�����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fpubDelGrpUser");
			pubLog_DBErr(_FFL_,"fpubDelGrpUser","190123",return_message);
			return 190123;
	  }
	  if (vCnt > 0) {
			sprintf(return_message, "��ǰ�����г�Ա������������![%d][%s]", SQLCODE, SQLERRMSG);
			return 190124;
	  }

		/*��ѯ�����û��ĳ�Ա chenxuan boss3
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpMemberMsg������");
#endif

	init(vMember_Id);
	init(vMember_No);
	Trim(vGrpCust_Id);
	EXEC SQL DECLARE member_cur CURSOR for
	  SELECT Member_Id, member_no
	  FROM dGrpUserMebMsg
	  WHERE Id_No = To_Number(:vGrpId_No) AND End_Time > Last_Day(sysdate) + 1;
	EXEC SQL OPEN member_cur;
	EXEC SQL FETCH member_cur INTO :vMember_Id,:vMember_No;
	while(SQLCODE == 0)
	{
		Trim(vMember_Id);
		Trim(vMember_No);
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "���ú������Լ����û���Ա�ĸ��Ѽƻ����б��");
		#endif
		retCode = fpubDisConUserMsg(vMember_Id, vMember_No, vContract_No, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
		if (retCode != 0) {
			pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "����fpubDisConUserMsg��������,Id_No[%s],Account_Id[%s],retCode[%d]", vMember_Id, vContract_No, retCode);
			EXEC SQL CLOSE member_cur;
			return 190123;
		}

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "��ѯdGrpUserMebMsg��[%s]������", vMember_Id);
		#endif
		EXEC SQL SELECT COUNT(*) INTO :vCnt
				   FROM dGrpUserMebMsg
				  WHERE Member_Id = To_Number(:vMember_Id)
					AND Id_No In
						(SELECT Id_No FROM dGrpUserMsg
						  WHERE Cust_Id = To_Number(:vGrpCust_Id)
							AND Bill_Date > Last_Day(sysdate) + 1
						)
				   AND End_Time > Last_Day(sysdate) + 1;
			if (SQLCODE != 0 && SQLCODE != NOTFOUND)
		{
			EXEC SQL CLOSE member_cur;
			sprintf(return_message,"��ѯdGrpUserMebMsg��[%s]������[%d]", vMember_Id, SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
			return 190124;
		}
VIP���ų�Աɾ����ȡ����cust_id���� luzp 20050919
		if (vCnt < 2) { �û�ֻ��һ�����ŵĳ�Աʱ��ɾ��
			#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpMemberMsg��[%s]����", vMember_Id);
			#endif
#if PROVINCE_RUN != PROVINCE_JILIN
			EXEC SQL INSERT INTO dGrpMemberMsgHis
					 (CUST_ID, UNIT_ID, UNIT_CODE, ID_NO, UNIT_ID_NO, BIG_FLAG,
					  PHONE_NO, NAME, USER_NAME, MALE_FLAG, BELONG_CODE,
										  SERVICE_NO, CARD_CODE,
					  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					  GMEMBER_STATUS_CODE, UPDATE_LOGIN, UPDATE_ACCEPT,
					  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			   SELECT CUST_ID, UNIT_ID, UNIT_CODE, ID_NO, UNIT_ID_NO,BIG_FLAG,
					  PHONE_NO, NAME, USER_NAME,MALE_FLAG, BELONG_CODE,
										  SERVICE_NO, CARD_CODE,
					  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					  GMEMBER_STATUS_CODE, :vLogin_No, :lLoginAccept,
					  :vTotal_Date,  to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), :vOp_Code, 'd'
				 FROM dGrpMemberMsg
				WHERE ID_NO = To_Number(:vMember_Id);
#else
			#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpMemberMsg��[%s]����", vMember_Id);
			#endif
			EXEC SQL INSERT INTO dGrpMemberMsgHis
					 (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,
					  PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,CARD_CODE,
					  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					  GMEMBER_STATUS_CODE, UPDATE_LOGIN, UPDATE_ACCEPT,
					  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, ORG_CODE)
			   SELECT CUST_ID, UNIT_ID, UNIT_CODE, ID_NO, ID_NO,
					  PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,CARD_CODE,
					  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					  GMEMBER_STATUS_CODE, :vLogin_No, :lLoginAccept,
					  :vTotal_Date,  sysdate, :vOp_Code, 'd', :sIn_Grp_OrgCode
				 FROM dGrpMemberMsg
				WHERE ID_NO = To_Number(:vMember_Id);
#endif
			if (SQLCODE != 0 && SQLCODE != 1403){
				sprintf(return_message,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
				EXEC SQL CLOSE member_cur;
				return 190125;
			}

			EXEC SQL delete FROM dGrpMemberMsg
				WHERE  Cust_Id = To_Number(:vGrpCust_Id)
				AND  ID_NO = To_Number(:vMember_Id);
			if (SQLCODE != 0 && SQLCODE != 1403){
				EXEC SQL CLOSE member_cur;
				sprintf(return_message,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
				return 190126;
			}


			EXEC SQL    INSERT INTO dGrpMemberUserMsgHis
												(
										UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD,
												OP_TIME, PRODUCT_CODE,
										UPDATE_ACCEPT, UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN,
										UPDATE_TYPE, UPDATE_CODE
												)
												SELECT UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD,
												OP_TIME, PRODUCT_CODE,:lLoginAccept,to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
												to_number(:vTotal_Date),:vLogin_No,'d',:vOp_Code
												from dGrpMemberUserMsg
												WHERE member_id = To_Number(:vMember_Id);
			if (SQLCODE != 0 && SQLCODE != 1403){
				EXEC SQL CLOSE member_cur;
				sprintf(return_message,"ɾ��dGrpMemberUserMsgHis�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
				return 190126;
			}


			EXEC SQL delete FROM dGrpMemberUserMsg
				WHERE member_id = To_Number(:vMember_Id);
			if (SQLCODE != 0 && SQLCODE != 1403){
				EXEC SQL CLOSE member_cur;
				sprintf(return_message,"ɾ��dGrpMemberUserMsg�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
				return 190126;
			}
		}

		init(vMember_Id);
			EXEC SQL FETCH member_cur INTO :vMember_Id,:vMember_No;
	}
	EXEC SQL CLOSE member_cur;
		*/

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpUserMsg����Ϣ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "INSERT INTO dGrpUserMsgHis ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "   (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    UPDATE_TYPE) ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", " SELECT ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     '%s', ", vLogin_No);
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    %d, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), '%s', ", lLoginAccept, vTotal_Date, vOp_Time, vOp_Code);
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    'D' ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "   FROM dGrpUserMsg ");
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "  WHERE ID_NO = To_Number('%s') ", vGrpId_No);
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "    AND Bill_Date > Last_Day(sysdate) + 1; ");
#endif

	EXEC SQL INSERT INTO dGrpUserMsgHis
		  (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		   IDS,         USER_NAME,     SM_CODE,       GROUP_ID,
		   PRODUCT_TYPE,  PRODUCT_CODE,
		   USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		   REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		   LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		   OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		   LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
		   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
		   UPDATE_TYPE)
		SELECT
		   CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		   IDS,         USER_NAME,     SM_CODE,       GROUP_ID,
		   PRODUCT_TYPE,  PRODUCT_CODE,
		   USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		   REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		   LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		   OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		   LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     :vLogin_No,
		   :lLoginAccept, :vTotal_Date, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),      :vOp_Code,
		   'D'
		  FROM dGrpUserMsg
		 WHERE ID_NO = To_Number(:vGrpId_No)
		   AND Bill_Date > Last_Day(sysdate) + 1;
	if(SQLCODE!=OK){
		sprintf(return_message,"����dGrpUserMsgHis���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190127",return_message);
		return 190127;
	}

	/*ֻҪbill_date��������1�գ�����Ϊ���Ѿ������ļ����û�*/
	/*ng����
	EXEC SQL UPDATE dGrpUserMsg
	  SET old_run = run_code, run_code = 'I', bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
	  WHERE id_no = To_Number(:vGrpId_No) AND Bill_Date > Last_Day(sysdate) + 1;
	if(SQLCODE!=OK){
		sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190128",return_message);
		return 190128;
	}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);

	strcpy(v_parameter_array[0],vTotal_Date);
	strcpy(v_parameter_array[1],vGrpId_No);

	v_ret=OrderUpdateGrpUserMsg(ORDERIDTYPE_USER,vGrpId_No,100,
							vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							vGrpId_No,
							" old_run = run_code, run_code = 'I', bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1 ",
							" AND Bill_Date > Last_Day(sysdate) + 1",v_parameter_array);
	printf("OrderUpdateGrpUserMsg ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190128",return_message);
		return 190128;
	}
	/*ng���� by yaoxc end*/
	EXEC SQL DELETE FROM dGrpUserMsgAdd WHERE id_no = To_Number(:vGrpId_No);
	if(SQLCODE!=OK && SQLCODE != 1403){
		sprintf(return_message,"ɾ��dGrpUserMsgAdd���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDelGrpUser");
		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190129",return_message);
		return 190129;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "���ú������Լ����û��ʻ��ĸ��Ѽƻ����б��");
#endif
	retCode = fpubDisConUserMsg(vGrpId_No, vGrpUser_No, vContract_No, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
	if (retCode != 0) {
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "����fpubDisConUserMsg��������,Id_No[%s],Account_Id[%s],retCode[%d]", vGrpId_No, vContract_No, retCode);
		return 190130;
	}

	/*�����������ٴ����Ա��ϵ���ڳ�Աɾ��ʱ�������� chenxuan boss3
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpUserMebMsg������");
#endif
	EXEC SQL INSERT INTO dGrpUserMebMsgHis
				 (ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
				  BEGIN_TIME, END_TIME,
				  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
				  UPDATE_CODE, UPDATE_TYPE)
			  SELECT
				  ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
				  BEGIN_TIME, END_TIME,
				  :vLogin_No, :lLoginAccept, :vTotal_Date, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
				  :vOp_Code, 'd'
				FROM dGrpUserMebMsg
			   WHERE Id_No = To_Number(:vGrpId_No)
				 AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0 && SQLCODE != 1403){
		sprintf(return_message,"����dGrpUserMebMsgHis�����Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190131;
	}

	EXEC SQL UPDATE dcustmsg
	   SET attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)
	 WHERE id_no in (select member_id from dGrpUserMebMsg
					  where id_no = To_Number(:vGrpId_No)
						and End_Time > Last_Day(sysdate) + 1
					  );
	if (SQLCODE != 0 && SQLCODE != 1403){
		sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190232;
	}
	EXEC SQL UPDATE dGrpUserMebMsg
				SET end_time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
			  WHERE Id_No = To_Number(:vGrpId_No)
				AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0 && SQLCODE != 1403){
		sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190133;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "ɾ��dGrpUserMebProdMsg������");
	#endif
#if PROVINCE_RUN != PROVINCE_JILIN
	EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
			   (ID_NO,  MEMBER_ID, MEMBER_NO,
								SM_CODE, MODE_CODE,FAV_ID,
				PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
				BEGIN_TIME, END_TIME,
				BAK_FIELD, UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
				UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			 SELECT
				ID_NO,  MEMBER_ID, MEMBER_NO,
								SM_CODE, MODE_CODE,FAV_ID,
				PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
				BEGIN_TIME, END_TIME,
				BAK_FIELD, :vLogin_No, :lLoginAccept, :vTotal_Date,
				to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), :vOp_Code, 'd'
			   FROM dGrpUserMebProdMsg
			  WHERE ID_No = To_Number(:vGrpId_No)
				AND End_Time > Last_Day(sysdate) + 1;
#else
	EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
			   (ID_NO,  USER_NO,MEMBER_ID, MEMBER_NO,
				PRODUCT_TYPE,
				PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
				BEGIN_TIME, END_TIME,
				BAK_FIELD, UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
				UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			 SELECT
				ID_NO,  USER_NO,MEMBER_ID, MEMBER_NO,
				PRODUCT_TYPE,
				PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
				BEGIN_TIME, END_TIME,
				BAK_FIELD, :vLogin_No, :lLoginAccept, :vTotal_Date,
				to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), :vOp_Code, 'd'
			   FROM dGrpUserMebProdMsg
			  WHERE ID_No = To_Number(:vGrpId_No)
				AND End_Time > Last_Day(sysdate) + 1;
#endif
	if (SQLCODE != 0 && SQLCODE != 1403){
		sprintf(return_message,"����dGrpUserMebProdMsgHis�����Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190134;
	}
	EXEC SQL UPDATE dGrpUserMebProdMsg
				SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
			  WHERE ID_No = To_Number(:vGrpId_No)
				AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0 && SQLCODE != 1403){
		sprintf(return_message,"ɾ��dGrpUserMebProdMsg�����Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
		return 190135;
	}
	*/

	return 0;
}

/************************************************************************
��������:fpubDisConUserMsg
����ʱ��:2005/01/12
������Ա:yangzh
��������:��ֹ�û����Ѽƻ�
�������:�û�ID
		 �ʺ�ID
		 ����Ա����
		 ��������
		 ��������
		 ������ˮ��
		 ������ע
		 ������Ϣ
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubDisConUserMsg(char *inId_No, char *srvNo, char *inContract_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{

	EXEC SQL BEGIN DECLARE SECTION;
		 int  vcnt = 0;  /*�����ж��Ǹ����û����Ǽ����û���ͨ����ѯdCustMsg��ʵ��*/
		 long lMaxSerialNo = 0;
		 long lLoginAccept = 0;
		 char vId_No[14+1];
		 char vContract_No[14+1];
		 char vLogin_No[6+1];
		 char vOp_Code[4+1];
		 char vTotal_Date[8+1];
		 char vOp_Time[24+1];
		 char vOp_Note[60+1];
	EXEC SQL END DECLARE SECTION;

		/*ng���� by yaoxc begin*/
		int  v_ret=0;
		TdConUserMsgIndex tdConUserMsgIndex;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char sTempSqlStr[1024];
		/*ng���� by yaoxc end*/

		/*ng���� by yaoxc begin*/
		memset(&tdConUserMsgIndex,0,sizeof(tdConUserMsgIndex));
		init(v_parameter_array);
		init(sTempSqlStr);
		/*ng���� by yaoxc end*/

	init(vId_No           );
	init(vContract_No     );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vOp_Time         );
	init(vOp_Note         );
	Trim(inId_No          );
	Trim(inContract_No    );

	/*������ֵ*/
	lLoginAccept = lSysAccept;
	pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "��ˮ=%ld ",lSysAccept);
	strcpy(vId_No,       inId_No       );
	strcpy(vContract_No, inContract_No );
	strcpy(vLogin_No,    login_no      );
	strcpy(vOp_Code,     op_code       );
	strcpy(vTotal_Date,  total_date    );
	strcpy(vOp_Note,     op_note       );

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "insert into dConUserMsgHis");
	#endif
	EXEC SQL INSERT INTO dConUserMsgHis
				(   Id_No,         Contract_No,  Serial_No,
					Bill_Order,    Pay_Order,    Begin_YMD,
					Begin_TM,      End_YMD,      End_TM,
					Limit_Pay,     Rate_Flag,    Stop_Flag,
					Update_Accept, Update_Login, Update_Date,
					Update_Time,   Update_Code,  Update_Type  )
			 SELECT
					Id_No,         Contract_No,  Serial_No,
					Bill_Order,    Pay_Order,    Begin_YMD,
					Begin_TM,      End_YMD,      End_TM,
					Limit_Pay,     Rate_Flag,    Stop_Flag,
					:lLoginAccept, :vLogin_No,   To_Number(:vTotal_Date),
					sysdate,       :vOp_Code,    'U'
			   FROM dConUserMsg
			  WHERE Id_No = To_Number(:vId_No)
				AND Contract_No = To_Number(:vContract_No);

		if (SQLCODE != 0) {
			sprintf(return_message, "����dConUserMsgHis����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubDisConUserMsg");
			pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191001",return_message);
			return 191001;
		}
		#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "������Ϣ��dGrpProductHis��");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "INSERT INTO dConUserMsgHis ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", " (   Id_No,         Contract_No,  Serial_No,  ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Bill_Order,    Pay_Order,    Begin_YMD,  ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Begin_TM,      End_YMD,      End_TM, ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Update_Accept, Update_Login, Update_Date, ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Update_Time,   Update_Code,  Update_Type  ) ");

		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", " SELECT ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", " Id_No,         Contract_No,  Serial_No, ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Bill_Order,    Pay_Order,    Begin_YMD, ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", " Begin_TM,      End_YMD,      End_TM, ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  '%ld', '%s', to_number('%s'), ", lLoginAccept, vLogin_No, vTotal_Date);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  to_date(),'%s','U' ", vOp_Code);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", " FROM dConUserMsg ");
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "WHERE Id_No = To_Number('%s') ", vId_No);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "  AND Contract_No = to_number('%s'); ", vContract_No);
	#endif

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "��ѯ����SerialNo");
	#endif
	EXEC SQL SELECT Nvl(Max(Serial_No), 0)
			   INTO :lMaxSerialNo
			   FROM dConUserMsg
			  WHERE Id_No = To_Number(:vId_No)
				AND Contract_No = To_Number(:vContract_No);
	if (SQLCODE != 0) {
		sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191002",return_message);
		return 191002;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "��ѯdCustMsg�Ƿ��Ǹ����û�");
	#endif
	EXEC SQL SELECT Count(*) INTO :vcnt FROM dCustMsg
			  WHERE Id_No = To_Number(:vId_No);
	if (SQLCODE != 0) {
		sprintf(return_message, "��ѯdCustMsg�Ƿ��Ǹ����û�ʧ��![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191003",return_message);
		return 191003;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "update dConUserMsg");
	#endif
/**
	EXEC SQL UPDATE dConUserMsg
				SET Serial_No = :lMaxSerialNo + 1,
					End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD')
			  WHERE Id_No = To_Number(:vId_No)
				AND Contract_No = To_Number(:vContract_No)
				AND Serial_No = 0
				AND END_YMD > To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD');
 **/
	/*ng����
	EXEC SQL UPDATE dConUserMsg
				SET End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),
					End_TM='000000'
			  WHERE Id_No = To_Number(:vId_No)
				AND Contract_No = To_Number(:vContract_No)
				AND Serial_No = 0;
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191004",return_message);
		return 191004;
	}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(v_parameter_array);

	strcpy(tdConUserMsgIndex.sIdNo,vId_No);
	strcpy(tdConUserMsgIndex.sContractNo,vContract_No);
	strcpy(tdConUserMsgIndex.sSerialNo,"0");

	strcpy(v_parameter_array[0],vTotal_Date);
	strcpy(v_parameter_array[1],vId_No);
	strcpy(v_parameter_array[2],vContract_No);
	strcpy(v_parameter_array[3],"0");

	v_ret=OrderUpdateConUserMsg(ORDERIDTYPE_USER,vId_No,100,
							vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
							tdConUserMsgIndex,tdConUserMsgIndex,
							" End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),End_TM='000000' ","",v_parameter_array);
	printf("OrderUpdateConUserMsg ,ret=[%d]\n",v_ret);
	if(0 > v_ret)
	{
		sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191004",return_message);
		return 191004;
	}
	/*ng���� by yaoxc end*/
	/* yangzh disabled 20050124 ����dCustMsg�����,�Ͳ�����wConUserChg*/
	if ((SQLROWS == 1) && (vcnt == 1)) {
		/*����û����Ѽƻ������ڣ��������Update���ִ�лᷢ������
		  �������û��ĸ��Ѽƻ���û�з����������������wConUserChg��¼
		  OP_TYPE��10-���ӣ�20-�޸ģ�30-ɾ��
		 */
#ifdef _CHGTABLE_
		#ifdef _DEBUG_
		   pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "insert into wConUserChg");
		#endif
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL INSERT INTO wConUserChg
					( Op_No,       Op_Type,    Id_No,
					  Contract_No, Bill_Order, Conuser_Update_Time,
					  Limit_Pay,   Rate_Flag,  Fee_Code,  DETAIL_CODE,
					  Fee_Rate,    Op_Time,phone_no
					)
				  VALUES
					( sMaxBillChg.NextVal,     '20',     To_Number(:vId_No),
					  To_Number(:vContract_No), 99999999, To_Char(sysdate,'YYYYMMDDHH24MISS'),
					  0,          '0',      '*', '*',
					  0,    To_Char(sysdate,'YYYYMMDDHH24MISS'), :srvNo
					 );
#else
		EXEC SQL INSERT INTO wConUserChg
					( Op_No,       Op_Type,    Id_No,
					  Contract_No, Bill_Order, Conuser_Update_Time,
					  Limit_Pay,   Rate_Flag,  Fee_Code,  DETAIL_CODE,
					  Fee_Rate,    Op_Time
					)
				  VALUES
					( sMaxBillChg.NextVal,     '20',     To_Number(:vId_No),
					  To_Number(:vContract_No), 99999999, To_Char(sysdate,'YYYYMMDDHH24MISS'),
					  0,          '0',      '*', '*',
					  0,    To_Char(sysdate,'YYYYMMDDHH24MISS')
					 );
#endif
		if (SQLCODE != 0) {
			sprintf(return_message, "����wConUserChg����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("fpubDisConUserMsg");
			pubLog_DBErr(_FFL_,"fpubDisConUserMsg","191005",return_message);
			return 191005;
		}
#endif
	}

	return 0;
}


/**
 *@name fCreateUser
 *@description  ��������������һ�����û����ϵ��û���Ϣ
 *@author       lugz
 *@created      2004-8-17 10:35
 *@input parameter information
 *@inparam      loginAccept     ��ˮ            char[22+1] �������룬������������ڷ�����ȡ��ˮ
 *@inparam      opCode  ���ܴ���
 *@inparam      loginNo ��������
 *@inparam      orgCode �������Ź���
 *@inparam      phoneNo �������
 *@inparam      smCode  ҵ������
 *@inparam      serviceType     �������� dCustMsg::service_type
 *@inparam      belongCode      ��������
 *@inparam      opTime          ����ʱ��        'YYYYMMDD HH24:MI:SS'
 *@inparam      custId  �ͻ�ID
 *@output parameter information
 *@outparam     retCode �������
 *@outparam     retMsg  ������Ϣ
 *@return ������-1����ȷ����0
 *@detail
 *      �������±�
 *              dCustMsgHis
 *              dCustMsg
 *                      attr_code��ʼ��Ϊ'00000000'
 *                      �û������ʼ��Ϊ��111111��������BGCEQz��
 *                      �������ڳ�ʼ��Ϊ"0"
 *              dCustInnetHis
 *                      �������ͳ�ʼ��Ϊ"01", ��:��ͨ�԰�Ӫҵ��
 *              dCustInnet
 *              dCustUserHis
 *              dCustUserMsg
 *              wUserMsgChg
 *              dConMsgHis
 *              dConMsg
 *                      �ʻ������ʼ��Ϊ�û�����
 *                      �ʻ��ʼķ�ʽ��ʼ��Ϊ��1���ʼ�
 *              dCustConMsgHis
 *              dCustConMsg
 *              dConUserMsgHis
 *              dConUserMsg
 *              wConUserChg
 *      �������µĿͻ���dCustDoc���ͻ�ID���������������
 *      ���ǵ�������Щ�����õ��������Ҳ����룺
 *              dMarkMsg
 *              dCustPost
 *              dCustPostHis
 *              dAssuMsg
 *              dCustStop
 *
 * ����ʱ�䣺20060217
 * �������ݣ�����dMarkMsg���¼
 */

int     fCreateUser(FBFR32 *transIN,    FBFR32 *transOUT,       char  *temp_buf,
								const char *loginAccept, const char *opCode, const char *loginNo,
								const char *orgCode,    const char *phoneNo, const char *smCode,
								const char *serviceType, const char *belongCode, const char *opTime,
								const char *custId,const char* vgroup_id,const char* vOrg_id,char *retCode,     char *retMsg)
{
		EXEC SQL BEGIN DECLARE SECTION;
				tUserDoc   userDoc ;
				tAccDoc    accDoc;     /*�ʻ������ṹ��*/
				int     mCount;
				char    mLoginAccept[22+1];
				char    mOpCode[4+1];
				char    mLoginNo[6+1];
				char    mOrgCode[9+1];
				char    mPhoneNo[15+1];
				char    mSmCode[2+1];
				char    mServiceType[2+1];
				char    mBelongCode[7+1];
				char    mOpTime[17+1];
				char    mCustId[22+1];


				char    mRegionCode[2+1];
				char    mIdNo[22+1];
				char    mConId[22+1];
				char    mAttrCode[8+1];
				char    mUserPwd[8+1];
				char    mBillType[5+1];
				char    mTotalDate[8+1];
				char    mInnetType[2+1];
				char    mPostType[1+1];
				char    mPayCode[1+1];
				char    mDefaultProduct[8+1];
				char    mSql[1024];
		EXEC SQL END DECLARE SECTION;
		int ret;
		/*ng���� by yaoxc begin*/
		int v_ret=0;
		TdCustMsg tdCustMsg;
		TdConUserMsg tdConUserMsg;
		TdCustConMsg tdCustConMsg;
		TdCustInnet tdCustInnet;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char sTempSqlStr[1024];
		char sTownCode[2+1];
		char sBeginTm[6+1];
		/*ng���� by yaoxc end*/

		/*ng���� by yaoxc begin*/
		memset(&tdCustMsg,0,sizeof(tdCustMsg));
		memset(&tdConUserMsg,0,sizeof(tdConUserMsg));
		memset(&tdCustConMsg,0,sizeof(tdCustConMsg));
		memset(&tdCustInnet,0,sizeof(tdCustInnet));
		init(v_parameter_array);
		init(sTempSqlStr);
		init(sTownCode);
		init(sBeginTm);
		/*ng���� by yaoxc end*/

		memset(&userDoc, 0, sizeof(userDoc));
		memset(&accDoc, 0, sizeof(accDoc));

		memset(mLoginAccept, 0, sizeof(mLoginAccept));
		memset(mOpCode, 0, sizeof(mOpCode));
		memset(mLoginNo, 0, sizeof(mLoginNo));
		memset(mOrgCode, 0, sizeof(mOrgCode));
		memset(mPhoneNo, 0, sizeof(mPhoneNo));
		memset(mSmCode, 0, sizeof(mSmCode));
		memset(mServiceType, 0, sizeof(mServiceType));
		memset(mBelongCode, 0, sizeof(mBelongCode));
		memset(mOpTime, 0, sizeof(mOpTime));
		memset(mCustId, 0, sizeof(mCustId));

		strcpy(mLoginAccept, loginAccept);
		strcpy(mOpCode, opCode);
		strcpy(mLoginNo, loginNo);
		strcpy(mOrgCode, orgCode);
		strcpy(mPhoneNo, phoneNo);
		strcpy(mSmCode, smCode);
		strcpy(mServiceType, serviceType);
		strcpy(mBelongCode, belongCode);
		strcpy(mOpTime, opTime);
		strcpy(mCustId, custId);

		strncpy(mRegionCode, mOrgCode,2);
		mRegionCode[2] = '\0';

		strcpy(mAttrCode, "01000000");
		strcpy(mUserPwd, "BGCEQz");

		memset(mBillType, 0, sizeof(mBillType));
		strcpy(mBillType, "0");

		memset(mInnetType, 0, sizeof(mInnetType));
		strcpy(mInnetType, "01");

		memset(mPostType, 0, sizeof(mPostType));
		strcpy(mPostType, "1");

		memset(mPayCode, 0, sizeof(mPayCode));
		strcpy(mPayCode, "0");

		memset(mTotalDate, 0, sizeof(mTotalDate));
		strncpy(mTotalDate, mOpTime, 8);

		mCount = 0;
		EXEC SQL        SELECT COUNT(*)
								  INTO :mCount
								  FROM dCustMsg
								 WHERE phone_no=:mPhoneNo
								   AND substr(run_code,2,1) < 'a';
		if (mCount > 0)
		{
				strcpy(retCode,"140001");
				sprintf(retMsg, "�Ѿ����ڴ˷������[%s]�Ŀ�����¼!", mPhoneNo);
				return -1;
		}

		memset(mIdNo, 0, sizeof(mIdNo));
		if(getMaxID(mRegionCode, 1, mIdNo) == NULL)
		{
				strcpy(retCode,"140002");
				strcpy(retMsg,"�õ��û�ID����!");
				return -1;
		}
		Trim(mIdNo);

		memset(mConId, 0, sizeof(mConId));
		if(getMaxID(mRegionCode, 1, mConId) == NULL)
		{
				strcpy(retCode,"140002");
				strcpy(retMsg,"�õ��ʻ�ID����!");
				return -1;
		}
		Trim(mConId);


#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustMsgHis
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,STOP_ATTR,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_ID
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), to_number(:mConId),1,:mPhoneNo,:mSmCode,:mServiceType,
										:mAttrCode,:mUserPwd,:mBelongCode,0, 'E', 0,
										'AA', to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mBillType),'NULL', 0,:mBillType,'001','001',
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode,:vgroup_id
								);
#else

	 /*       EXEC SQL        INSERT INTO dCustMsgHis
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_ID,GROUP_NO
								)
							VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), to_number(:mConId),1,:mPhoneNo,:mSmCode,:mServiceType,
										:mAttrCode,:mUserPwd,:mBelongCode,0, 'E', 0,
										'AA', to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mBillType),'NULL', 0,:mBillType,'001',
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode,:vgroup_id,'unknown'
								);
		#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140003");
				sprintf(retMsg,"�����dCustMsgHis����[%d]!",SQLCODE);
				return -1;
		}
   */


#endif
	if(SQLCODE != 0)
	{
			strcpy(retCode,"140003");
			sprintf(retMsg,"�����dCustMsgHis����[%d]!",SQLCODE);
			return -1;
	}



#if PROVINCE_RUN != PROVINCE_JILIN

		EXEC SQL        INSERT INTO dCustMsg
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,STOP_ATTR,
										GROUP_ID
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), to_number(:mConId),1,:mPhoneNo,:mSmCode,:mServiceType,
										:mAttrCode,:mUserPwd,:mBelongCode,0, 'E', 0,
										'AA', to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mBillType),'NULL', 0,:mBillType,'001','001',:vgroup_id
								);
		/*
		ng���� by yaoxc begin
		strcpy(tdCustMsg.sIdNo    		, mIdNo);
		strcpy(tdCustMsg.sCustId 		, mCustId);
		strcpy(tdCustMsg.sContractNo 	, mConId);
		strcpy(tdCustMsg.sIds 	 		, "1");
		strcpy(tdCustMsg.sPhoneNo 		, mPhoneNo);
		strcpy(tdCustMsg.sSmCode   		, mSmCode);
		strcpy(tdCustMsg.sServiceType   , mServiceType);
		strcpy(tdCustMsg.sAttrCode     	, mAttrCode);
		strcpy(tdCustMsg.sUserPasswd 	, mUserPwd);
		strcpy(tdCustMsg.sOpenTime   	, mOpTime);
		strcpy(tdCustMsg.sBelongCode   	, mBelongCode);
		strcpy(tdCustMsg.sLimitOwe   	, "0");
		strcpy(tdCustMsg.sCreditCode    , "E");
		strcpy(tdCustMsg.sCreditValue 	, "0");
		strcpy(tdCustMsg.sRunCode 		, "AA");
		strcpy(tdCustMsg.sRunTime 		, mOpTime);
		strcpy(tdCustMsg.sBillDate 		, mOpTime);
		strcpy(tdCustMsg.sBillType   	, mBillType);
		strcpy(tdCustMsg.sEncryptPrepay , "NULL");
		strcpy(tdCustMsg.sCmdRight     	, "0");
		strcpy(tdCustMsg.sLastBillType 	, mBillType);
		strcpy(tdCustMsg.sBakField   	, "001");
		strcpy(tdCustMsg.sGroupId   	, "001");
		strcpy(tdCustMsg.sGroupNo   	, vgroup_id);
		strcpy(tdCustMsg.sgroupmsg   	, "");

		v_ret=OrderInsertCustMsg("2",mIdNo,100,
							   mOpCode,atol(mLoginAccept),mLoginNo,"fCreateUser",
							   tdCustMsg);
		printf("OrderInsertCustMsg ,ret=[%d]\n",v_ret);
		ng���� by yaoxc end*/
#else
	/*    EXEC SQL        INSERT INTO dCustMsg
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
										GROUP_ID,GROUP_NO
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), to_number(:mConId),1,:mPhoneNo,:mSmCode,:mServiceType,
										:mAttrCode,:mUserPwd,:mBelongCode,0, 'E', 0,
										'AA', to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mBillType),'NULL', 0,:mBillType,'001',
										'unknown',:vgroup_id
								);
		 */

	/*��֯�������� majha   add����createCustMsg()������Ϣ	*/

	userDoc.vCust_Id=atol(mCustId);
	userDoc.vId_No=atol(mIdNo);
	userDoc.vContract_No=atol(mConId );
	userDoc.vIds=1;
	strncpy(userDoc.vPhone_No,       	mPhoneNo,          15);
	strncpy(userDoc.vSm_Code,        	mSmCode,          	2);
	strncpy(userDoc.vService_Type,   	mServiceType,     	2);
	strncpy(userDoc.vAttr_Code,      	mAttrCode,        	8);
	strncpy(userDoc.vUser_Passwd,    	mUserPwd,           8);
	strncpy(userDoc.vOpen_Time,      	mOpTime,         	17);
	strncpy(userDoc.vBelong_Code,    	mBelongCode,      	7);
	userDoc.vLimit_Owe=0;
	strncpy(userDoc.vCredit_Code,    	"E",      	        1);
	userDoc.vCredit_Value=0;
	strncpy(userDoc.vRun_Code,       	"AA",              	2);
	strncpy(userDoc.vRun_Time,       	mOpTime,           17);
	strncpy(userDoc.vBill_Date,      	mOpTime,           17);
	userDoc.vBill_Type=atoi(       	    mBillType       	 ); /* int ����*/
	strncpy(userDoc.vEncrypt_Prepay, 	"NULL",         	16);
	userDoc.vCmd_Right=0;
	strncpy(userDoc.vLast_Bill_Type, 	mBillType,        	1); /* char ����*/
	strncpy(userDoc.vBak_Field,      	"001",         	   12);
	userDoc.vLogin_Accept=atol(mLoginAccept);
	strncpy(userDoc.vOp_Time,        	mOpTime,           17);
	userDoc.vTotal_Date=atoi(        mTotalDate          );     /*  int ���� */
	strncpy(userDoc.vLogin_No,       	mLoginNo,         	6);
	strncpy(userDoc.vOp_Code,        	mOpCode,          	4);
	strncpy(userDoc.vGroup_Id,       	vgroup_id,         	10);
	strncpy(userDoc.varea_code ,		"",                 10);

	ret=0;
	ret = createCustMsg(&userDoc,retMsg);
	if(ret != 0)
	{
		strcpy(retCode,"140021");
		sprintf(retMsg,"����createCustMsg()����[%d]!",SQLCODE);
		return 190027;
	}
	/*��֯�������� majha   end����createCustMsg()������Ϣ	*/


#endif

		if(SQLCODE != 0)
		{
				strcpy(retCode,"140004");
				sprintf(retMsg,"�����dCustMsg����[%d]!",SQLCODE);
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}


#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustInnetHis
								(
										ID_NO,CUST_ID,BUY_NAME,BUY_ID,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), NULL, NULL, :mBelongCode, substr(:mBelongCode,3,2),:mInnetType,
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mLoginAccept),:mLoginNo,NULL,0,0,
										0,0,0,0,0,0,
										0,'0','NULL','NULL','NULL',0,:vgroup_id,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode
								);
#else
		EXEC SQL        INSERT INTO dCustInnetHis
								(
										ID_NO,CUST_ID,BUY_NAME,BUY_ID,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), NULL, NULL, :mBelongCode, substr(:mBelongCode,3,2),:mInnetType,
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mLoginAccept),:mLoginNo,NULL,0,0,
										0,0,0,0,0,0,
										0,'0','NULL','NULL','NULL',0,:vgroup_id,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode
								);
#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140005");
				strcpy(retMsg,"�����dCustInnetHis����!");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}

#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustInnet
								(
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,
										GROUP_ID
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), NULL, :mBelongCode, substr(:mBelongCode,3,2),:mInnetType,
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mLoginAccept),:mLoginNo,NULL,0,0,
										0,0,0,0,0,0,
										0,'0','NULL','NULL','NULL',0,
										:vgroup_id
								);
		/*
		ng���� by yaoxc begin
		EXEC SQL select substr(:mBelongCode,3,2) into :sTownCode from dual;

		strcpy(tdCustInnet.sIdNo    	, mIdNo);
		strcpy(tdCustInnet.sCustId 		, mCustId);
		strcpy(tdCustInnet.sBuyName 	, "NULL");
		strcpy(tdCustInnet.sBelongCode 	, mBelongCode);
		strcpy(tdCustInnet.sTownCode 	, sTownCode);
		strcpy(tdCustInnet.sInnetType   , mInnetType);
		strcpy(tdCustInnet.sOpenTime 	, mOpTime);
		strcpy(tdCustInnet.sLoginAccept , mLoginAccept);
		strcpy(tdCustInnet.sLoginNo 	, mLoginNo);
		strcpy(tdCustInnet.sMachineCode , "NULL");
		strcpy(tdCustInnet.sCashPay    	, "0");
		strcpy(tdCustInnet.sCheckPay 	, "0");
		strcpy(tdCustInnet.sSimFee 		, "0");
		strcpy(tdCustInnet.sMachineFee 	, "0");
		strcpy(tdCustInnet.sInnetFee 	, "0");
		strcpy(tdCustInnet.sChoiceFee   , "0");
		strcpy(tdCustInnet.sOtherFee 	, "0");
		strcpy(tdCustInnet.sHandFee 	, "0");
		strcpy(tdCustInnet.sDeposit 	, "0");
		strcpy(tdCustInnet.sBackFlag 	, "0");
		strcpy(tdCustInnet.sEncryptFee  , "NULL");
		strcpy(tdCustInnet.sSystemNote 	, "NULL");
		strcpy(tdCustInnet.sOpNote 		, "NULL");
		strcpy(tdCustInnet.sAssureNo 	, "0");
		strcpy(tdCustInnet.sGroupId 	, vgroup_id);

		v_ret=OrderInsertCustInnet("2",mIdNo,100,
							   mOpCode,atol(mLoginAccept),mLoginNo,"fCreateUser",
							   tdCustInnet);
		printf("OrderInsertCustInnet ,ret=[%d]\n",v_ret);
		ng���� by yaoxc end*/
#else
		/*ng����
		EXEC SQL        INSERT INTO dCustInnet
								(
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,
										GROUP_ID
								)
								VALUES
								(
										to_number(:mIdNo), to_number(:mCustId), NULL, :mBelongCode, substr(:mBelongCode,3,2),:mInnetType,
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'), to_number(:mLoginAccept),:mLoginNo,NULL,0,0,
										0,0,0,0,0,0,
										0,'0','NULL','NULL','NULL',0,
										:vgroup_id
								);
		ng����*/
		/*ng���� by yaoxc begin*/
		EXEC SQL select substr(:mBelongCode,3,2) into :sTownCode from dual;

		strcpy(tdCustInnet.sIdNo    	, mIdNo);
		strcpy(tdCustInnet.sCustId 		, mCustId);
		strcpy(tdCustInnet.sBuyName 	, "");
		strcpy(tdCustInnet.sBelongCode 	, mBelongCode);
		strcpy(tdCustInnet.sTownCode 	, sTownCode);
		strcpy(tdCustInnet.sInnetType   , mInnetType);
		strcpy(tdCustInnet.sOpenTime 	, mOpTime);
		strcpy(tdCustInnet.sLoginAccept , mLoginAccept);
		strcpy(tdCustInnet.sLoginNo 	, mLoginNo);
		strcpy(tdCustInnet.sMachineCode , "");
		strcpy(tdCustInnet.sCashPay    	, "0");
		strcpy(tdCustInnet.sCheckPay 	, "0");
		strcpy(tdCustInnet.sSimFee 		, "0");
		strcpy(tdCustInnet.sMachineFee 	, "0");
		strcpy(tdCustInnet.sInnetFee 	, "0");
		strcpy(tdCustInnet.sChoiceFee   , "0");
		strcpy(tdCustInnet.sOtherFee 	, "0");
		strcpy(tdCustInnet.sHandFee 	, "0");
		strcpy(tdCustInnet.sDeposit 	, "0");
		strcpy(tdCustInnet.sBackFlag 	, "0");
		strcpy(tdCustInnet.sEncryptFee  , "NULL");
		strcpy(tdCustInnet.sSystemNote 	, "NULL");
		strcpy(tdCustInnet.sOpNote 		, "NULL");
		strcpy(tdCustInnet.sAssureNo 	, "0");
		strcpy(tdCustInnet.sGroupId 	, vgroup_id);

		v_ret=OrderInsertCustInnet(ORDERIDTYPE_USER,mIdNo,100,
							   mOpCode,atol(mLoginAccept),mLoginNo,"fCreateUser",
							   tdCustInnet);
		printf("OrderInsertCustInnet ,ret=[%d]\n",v_ret);
		/*ng���� by yaoxc end*/
#endif
		if(v_ret != 0)
		{
				strcpy(retCode,"140006");
				strcpy(retMsg,"�����dCustInnet����!");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}

		EXEC SQL        INSERT INTO dCustUserHis
								(
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG,
										UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_ACCEPT,OP_CODE
								)
								VALUES
								(
										to_number(:mCustId), to_number(:mIdNo), '0','0',
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),:mLoginNo,'a',  to_number(:mLoginAccept),:mOpCode
								);
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140007");
				strcpy(retMsg,"�����dCustUserHis����!");
				return -1;
		}

		EXEC SQL        INSERT INTO dCustUserMsg
								(
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG
								)
								VALUES
								(
										to_number(:mCustId), to_number(:mIdNo), '0','0'
								);
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140008");
				strcpy(retMsg,"�����dCustUserMsg����!");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}

#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO wUserMsgChg
								(
										OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
										ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
								)
								VALUES
								(
										sMaxBillChg.NEXTVAL, '1', TO_NUMBER(:mIdNo), TO_NUMBER(:mCustId),:mPhoneNo,:mSmCode,
										:mAttrCode, :mBelongCode, 'AA',
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),:vgroup_id
								);
#else
		EXEC SQL        INSERT INTO wUserMsgChg
								(
										OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
										ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
								)
								VALUES
								(
										sMaxBillChg.NEXTVAL, '1', TO_NUMBER(:mIdNo), TO_NUMBER(:mCustId),:mPhoneNo,:mSmCode,
										:mAttrCode, :mBelongCode, 'AA',
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),:vgroup_id
								);
#endif
		if (SQLCODE != 0)
		{
				strcpy(retCode,"140009");
				strcpy(retMsg,"�����wUserMsgChg����!");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
#endif
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dConMsgHis
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,
										Post_Bank_Code, Account_No,   Account_Type,
										Update_Code,    Update_Date,  Update_Time,
										Update_Login,   Update_Accept,Update_Type,Group_Id
								)
								VALUES
								(
										TO_NUMBER(:mConId),TO_NUMBER(:mCustId), :mUserPwd,
										:mPhoneNo,         0,           :mBelongCode,
										0,TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'Y',
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'), :mPostType,   0,
										SUBSTR(:mOpTime,1,6), 0,   0,
										'E',           :mPayCode,   'NULL',
										'NULL',        'NULL',       '0',
										:mOpCode,      TO_NUMBER(:mTotalDate),
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										:mLoginNo,    to_number(:mLoginAccept),'a',:vgroup_id
								);
#else
   /*     EXEC SQL        INSERT INTO dConMsgHis
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,
										Post_Bank_Code, Account_No,   Account_Type,
										Update_Code,    Update_Date,  Update_Time,
										Update_Login,   Update_Accept,Update_Type
								)
								VALUES
								(
										TO_NUMBER(:mConId),TO_NUMBER(:mCustId), :mUserPwd,
										:mPhoneNo,         0,           :mBelongCode,
										0,TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'Y',
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'), :mPostType,   0,
										SUBSTR(:mOpTime,1,6), 0,   0,
										'E',           :mPayCode,   'NULL',
										'NULL',        'NULL',       '0',
										:mOpCode,      TO_NUMBER(:mTotalDate),
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										:mLoginNo,    to_number(:mLoginAccept),'a'
								);
		*/
#endif
		if (SQLCODE != 0) {
				strcpy(retCode, "140010");
				sprintf(retMsg, "����dConMsgHis ����!\n");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}

pubLog_Debug(_FFL_, "fCreateUser", "", "--chenxuan note-- mConId=[%s], mCustId=[%s], mUserPwd=[%s], mPhoneNo=[%s], mBelongCode=[%s],mOpTime=[%s], mPostType=[%s], mPayCode=[%s]",
	mConId, mCustId, mUserPwd, mPhoneNo, mBelongCode, mOpTime, mPostType, mPayCode);

#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dConMsg
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,
										Post_Bank_Code, Account_No,   Account_Type,Group_Id
								)
								VALUES
								(
										TO_NUMBER(:mConId),TO_NUMBER(:mCustId), :mUserPwd,
										:mPhoneNo,         0,           :mBelongCode,
										0,TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'Y',
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'), :mPostType,   0,
										SUBSTR(:mOpTime,1,6), 0,   0,
										'E',           :mPayCode,   'NULL',
										'NULL',        'NULL',       '0',:vgroup_id
								);
#else
	/*    EXEC SQL        INSERT INTO dConMsg
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,
										Post_Bank_Code, Account_No,   Account_Type
								)
								VALUES
								(
										TO_NUMBER(:mConId),TO_NUMBER(:mCustId), :mUserPwd,
										:mPhoneNo,         0,           :mBelongCode,
										0,TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'Y',
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'), :mPostType,   0,
										SUBSTR(:mOpTime,1,6), 0,   0,
										'E',           :mPayCode,   'NULL',
										'NULL',        'NULL',       '0'
								);
	*/
		  /*��֯�������� majha   add����createConMsg()������Ϣ	*/


		strncpy(accDoc.vLogin_Accept,          mLoginAccept,       14);
		strncpy(accDoc.vContract_No,       	 mConId,             14);
		strncpy(accDoc.vCon_Cust_Id,       	 mCustId,            10);
		strncpy(accDoc.vContract_Passwd,	     mUserPwd,            8);
		strncpy(accDoc.vBank_Cust,      	     mPhoneNo,           60);
		strncpy(accDoc.vBelong_Code,           mBelongCode,         7);
		strncpy(accDoc.vAccount_Limit,     	 "E",                 1);
		strncpy(accDoc.vStatus,            	 "Y",                 1);
		strncpy(accDoc.vPost_Flag,         	  mPostType,          1);
		strncpy(accDoc.vPay_Code,          	  mPayCode,           1);
		strncpy(accDoc.vBank_Code,      	      "null",             5);
		strncpy(accDoc.vPost_Bank_Code,    	  "null",             5);
		strncpy(accDoc.vAccount_No,      	      "null",            30);
		strncpy(accDoc.vAccount_Type,    	       "0",               1);
		strncpy(accDoc.vBegin_Time,      	      mOpTime,           17);
		strncpy(accDoc.vEnd_Time,         	  "",                17);
		strncpy(accDoc.vOp_Code,           	  mOpCode,            4);
		strncpy(accDoc.vLogin_No,          	  mLoginNo,           6);
		strncpy(accDoc.vOrg_Code,          	  orgCode,            9);
		strncpy(accDoc.vOp_Note,           	  "",                60);
		strncpy(accDoc.vSystem_Note,       	  "",                60);
		strncpy(accDoc.vIp_Addr,           	  "0.0.0.0",         15);
		strncpy(accDoc.vGroup_Id,          	  vgroup_id,         10);
		strncpy(accDoc.vUpdate_Type,       	  "a",                1);
		accDoc.vOddment  = 0.00;
		accDoc.vPrepay_Fee  = 0.00;
		accDoc.vDeposit = 0.00;
		accDoc.vOwe_Fee = 0.00;
		accDoc.vAccount_Mark = 0;

		ret = 0;
		
		/***********ng ���� �滻createConMsg_ng ����id_typeһ��*********************/
		/*ret = createConMsg(&accDoc,retMsg);*/
		ret = createConMsg_ng(ORDERIDTYPE_USER, mIdNo, &accDoc,retMsg);
		/***********ng ���� �滻createConMsg_ng ����id_typeһ��*********************/
		if(ret != 0)
		{
			strcpy(retCode,"140020");
			sprintf(retMsg, "����createConMsg() ����!\n");
			return -1;
		}


#endif
		if (SQLCODE != 0) {
				strcpy(retCode, "140011");
				sprintf(retMsg, "����dConMsg ����!\n");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}

		EXEC SQL        INSERT INTO dCustConMsgHis
								(
										Cust_Id,         Contract_No,     Begin_Time,
										End_Time,        Update_Accept,   Update_Login,
										Update_Date,     Update_Time,     Update_Code,
										Update_Type
								)
								VALUES
								(
										TO_NUMBER(:mCustId),TO_NUMBER(:mConId),    TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										TO_DATE('20500101','YYYYMMDD'), to_number(:mLoginAccept),  :mLoginNo,
										TO_NUMBER(:mTotalDate),   TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),      :mOpCode,      'a'
								);
		if (SQLCODE != 0) {
				strcpy(retCode, "140012");
				sprintf(retMsg, "����ddCustConMsgHis ����!\n");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
		/*ng����
		EXEC SQL        INSERT INTO dCustConMsg
								(
										Cust_Id,         Contract_No,     Begin_Time,
										End_Time
								)
								VALUES
								(
										TO_NUMBER(:mCustId),TO_NUMBER(:mConId),    TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										TO_DATE('20500101','YYYYMMDD')
								);
		if (SQLCODE != 0) {
				strcpy(retCode, "140013");
				sprintf(retMsg, "����ddCustConMsg����!\n");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
		ng����*/
		/*ng���� by yaoxc begin*/
		strcpy(tdCustConMsg.sCustId    		, mCustId);
		strcpy(tdCustConMsg.sContractNo 	, mConId);
		strcpy(tdCustConMsg.sBeginTime 		, mOpTime);
		strcpy(tdCustConMsg.sEndTime 	 	, "20500101 00:00:00");

		v_ret=OrderInsertCustConMsg(ORDERIDTYPE_USER,mIdNo,100,
							   mOpCode,atol(mLoginAccept),mLoginNo,"fCreateUser",
							   tdCustConMsg);
		printf("OrderInsertCustConMsg ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode, "140013");
			sprintf(retMsg, "����ddCustConMsg����!\n");
			PUBLOG_DBDEBUG("fCreateUser");
			pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
			return -1;
		}
		/*ng���� by yaoxc end*/
		EXEC SQL INSERT INTO dConUserMsgHis
						(
								Id_No,        Contract_No,  Serial_No,
								Bill_Order,   Pay_Order,    Begin_YMD,
								Begin_TM,     End_YMD,      End_TM,
								Limit_Pay,    Rate_Flag,    Stop_Flag,
								Update_Accept,Update_Login, Update_Date,
								Update_Time,  Update_Code,  Update_Type
						)
						VALUES
						(
								TO_NUMBER(:mIdNo),TO_NUMBER(:mConId),0,
								99999999,     1,           :mTotalDate,
								to_char(to_date(:mOpTime,'yyyymmdd hh24:mi:ss'), 'hh24miss'),   '20500101',   '235959',
								9999999999,  'N',          'Y',
								to_number(:mLoginAccept),:mLoginNo,   TO_NUMBER(:mTotalDate),
								TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),        :mOpCode,    'a'
						);
		if (SQLCODE != 0) {
				sprintf(retMsg, "����dConUserMsgHis����!\n");
				strcpy(retCode, "810014");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
		/*ng����
		EXEC SQL INSERT INTO dConUserMsg
						(
								Id_No,        Contract_No,  Serial_No,
								Bill_Order,   Pay_Order,    Begin_YMD,
								Begin_TM,     End_YMD,      End_TM,
								Limit_Pay,    Rate_Flag,    Stop_Flag
						)
						VALUES
						(
								TO_NUMBER(:mIdNo),TO_NUMBER(:mConId),0,
								99999999,     1,           :mTotalDate,
								to_char(to_date(:mOpTime,'yyyymmdd hh24:mi:ss'), 'hh24miss'),   '20500101',   '235959',
								9999999999,  'N',          'Y'
						);
		if (SQLCODE != 0) {
				sprintf(retMsg, "����dConUserMsg����!\n");
				strcpy(retCode, "140015");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
		ng����*/
		/*ng���� by yaoxc begin*/
		EXEC SQL select to_char(to_date(:mOpTime,'yyyymmdd hh24:mi:ss'), 'hh24miss') into :sBeginTm from dual;

		strcpy(tdConUserMsg.sIdNo    		, mIdNo);
		strcpy(tdConUserMsg.sContractNo 	, mConId);
		strcpy(tdConUserMsg.sBillOrder 		, "99999999");
		strcpy(tdConUserMsg.sSerialNo 	 	, "0");
		strcpy(tdConUserMsg.sPayOrder 		, "1");
		strcpy(tdConUserMsg.sBeginYmd   	, mTotalDate);
		strcpy(tdConUserMsg.sBeginTm    	, sBeginTm);
		strcpy(tdConUserMsg.sEndYmd     	, "20500101");
		strcpy(tdConUserMsg.sEndTm 			, "235959");
		strcpy(tdConUserMsg.sLimitPay   	, "9999999999");
		strcpy(tdConUserMsg.sRateFlag   	, "N");
		strcpy(tdConUserMsg.sStopFlag   	, "Y");

		v_ret=OrderInsertConUserMsg(ORDERIDTYPE_USER,mIdNo,100,
							   mOpCode,atol(mLoginAccept),mLoginNo,"fCreateUser",
							   tdConUserMsg);
		printf("OrderInsertConUserMsg ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			 sprintf(retMsg, "����dConUserMsg����!\n");
			 strcpy(retCode, "140015");
			 PUBLOG_DBDEBUG("fCreateUser");
			 pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
			 return -1;
		}
		/*ng���� by yaoxc end*/
#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO wConUserChg
								(
										Op_No,       Op_Type,    Id_No,
										Contract_No, Bill_Order, Conuser_Update_Time,
										Limit_Pay,   Rate_Flag,  Fee_Code,DETAIL_CODE,
										Fee_Rate,   Op_Time,phone_no
								)
								select
										sMaxBillChg.NEXTVAL,     '10',         TO_NUMBER(:mIdNo),
										TO_NUMBER(:mConId), 99999999, to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
										0,          'N',      '*', '*',
										0,      to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),:mPhoneNo
								  from dual;
#else
		EXEC SQL        INSERT INTO wConUserChg
								(
										Op_No,       Op_Type,    Id_No,
										Contract_No, Bill_Order, Conuser_Update_Time,
										Limit_Pay,   Rate_Flag,  Fee_Code,DETAIL_CODE,
										Fee_Rate,   Op_Time
								)
								select
										sMaxBillChg.NEXTVAL,     '10',         TO_NUMBER(:mIdNo),
										TO_NUMBER(:mConId), 99999999, to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
										0,          'N',      '*', '*',
										0,      to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS')
								  from dual;
#endif
		if (SQLCODE != 0)
		{
				sprintf(retMsg, "����wConUserChg ����!\n");
				strcpy(retCode, "140016");
				PUBLOG_DBDEBUG("fCreateUser");
				pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
				return -1;
		}
#endif
		/* ���ӻ�����Ϣ��dMarkMsg 20060217
	EXEC SQL INSERT INTO dMarkMsg(
						ID_NO          ,OPEN_TIME      ,INIT_POINT     ,
						CURRENT_POINT  ,YEAR_POINT     ,ADD_POINT      ,
						OP_TIME        ,BEGIN_TIME     ,TOTAL_USED     ,
						TOTAL_PRIZE    ,TOTAL_PUNISH   ,LASTYEAR_POINT ,
						BASE_POINT     ,SM_CODE        )
				VALUES( TO_NUMBER(:mIdNo) ,TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),0,
						0              ,0              ,0              ,
						TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),0,
						0              ,0              ,0              ,
						0              ,:mSmCode        );
   if (SQLCODE!=OK){
		 sprintf(retMsg, "����dMarkMsg ����!\n");
		 strcpy(retCode, "140020");
		 return -1;
	}
	*/

		/*
		 *  ����IP���в���ʹ��Ĭ�ϲ�Ʒ������ʹ����ѡ��Ʒ����˸ô���Ҫ������ע������IPҵ�����
		 *
		 *  commented by lab 2006.1.10
		 *
		 * if ((strncmp(mSmCode, "gd", 2) == 0) || (strncmp(mSmCode, "yx", 2) == 0))
		 *
		 */
		if (strncmp(mSmCode, "gd", 2) == 0)
		{

#if PROVINCE_RUN == PROVINCE_SHANXI
				/**
				 * ��sSmProduct�У�ֻ�ܴ���һ����Ʒ�Ƶ�Ĭ������Ʒ��¼����֤���ݵ�������
				 */
				memset(mDefaultProduct, 0, sizeof(mDefaultProduct));
		pubLog_Debug(_FFL_, "fCreateUser", "", "---20051206 msMCode=[%s]",mSmCode);
				EXEC SQL        SELECT a.product_code
											  INTO :mDefaultProduct
											  FROM sSmProduct a,sProductRelease b
											 WHERE a.product_code=b.product_code
											   AND a.sm_code = :mSmCode
											   AND b.group_id= '1000';
				if (SQLCODE != 0)
				{
						sprintf(retMsg, "ȡĬ������Ʒ����!\n");
						strcpy(retCode, "140017");
						PUBLOG_DBDEBUG("fCreateUser");
						pubLog_DBErr(_FFL_,"fCreateUser",retCode,retMsg);
						return -1;
				}

				if ( (ret = beginProduct(mPhoneNo,mDefaultProduct,"","",mOpTime,MAXENDTIME,
																mOpCode,mLoginNo,mLoginAccept,mOpTime,retMsg)) != 0)
				{
						sprintf(retCode,"%d",ret);
						return -1;
				}
#else
				sprintf(retCode,"%06d",pubBillBeginMode(mPhoneNo,"gdhm0001",mOpTime,mOpCode,
					mLoginNo,mLoginAccept,mOpTime,"0","",retMsg));
				if (strncmp(retCode, "000000", 6) != 0)
				{
						return -2;
				}
#endif /*PROVINCE_RUN = PROVINCE_SHANXI*/
		}
		return 0;
}

/**
 *     ����һ�������û����û���Ϣ�����п��ܵ���fCreateUser���������û���Ϣ��
 * ���������ʻ���Ϊ�մ��ʺŽ�������Ա�û������з��ã��˺�������������ʻ�
 * �Ĳ��ָ��ѹ��ܣ�����ͨ�����Ÿ��ѱ��ģ��ʵ�ִ˹��ܡ��ڴ˺��������Ӷ�ԤԼ
 * ��Ա��ϵԤԼ��֧�֡�
 * @author      lugz
 * @created     2005-10-30 18:13
 * @inparam     grpUser �ο��ṹ���˵��
 * @outparam    retCode �������
 * @outparam    retMsg  ������Ϣ
 * @return ������-1����ȷ����0
 */
int     CreateGrpUser(FBFR32 *transIN,  FBFR32 *transOUT,       char  *temp_buf,TCreateGrpUser *grpUser,
								char *retCode,  char *retMsg)
{
		EXEC SQL BEGIN DECLARE SECTION;
		int   mCount;
		int   mretCode=0;
		char  vTmpUnitId[10+1];
		char  vUnit_Id[10+1];
		char  vUnit_Code[10+1];
		char  vGrpCust_Id[22+1];
		char  vService_No[6+1];
		char  vGrp_Userno[15+1];
		char  vGrp_Smcode[2+1];
		char  vProduct_Type[2+1];
		char  vEnd_Date[8+1];
		char  vGroup_Type_Code[1+1];
		char  vDepartment[36+1];
		char  vLmtFee[14+1];
		char  vShort_PhoneNo[11+1];
		char  vBill_Code[8+1];
		char  mProductCode[2+1];
		char  vRun_Time[17+1];       /*add by miaoyl at 20090629 ����ʱ�� */
		int   ret;                   /*add by miaoyl at 20090708  */
		char  vOrg_Id[10+1];         /*add by miaoyl at 20090708  */

		/*middle*/
		char    mSql[1024];
		char    mIdNo[22+1];
		char    mTotalDate[8+1];
		int     mBillOrder=0;
		char    grpOrgCode[10+1];
		char    vGrpUserName[60+1];
		char    vGrp_Flag[1+1];
		EXEC SQL END DECLARE SECTION;

		/*ng���� by yaoxc begin*/
		int icount=0;
		int  v_ret=0;
		TdGrpUserMebMsg tdGrpUserMebMsg;
		TdGrpUserMebMsgIndex oldIndex;
		TdGrpUserMebMsgIndex newIndex;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char sTempSqlStr[1024];
		char sIdNo[14+1];
		char sBeginTime[17+1];
		char sEndTime[17+1];
		/*ng���� by yaoxc end*/

		/*ng���� by yaoxc begin*/
		memset(&tdGrpUserMebMsg,0,sizeof(tdGrpUserMebMsg));
		memset(&oldIndex,0,sizeof(oldIndex));
		memset(&newIndex,0,sizeof(newIndex));
		init(v_parameter_array);
		init(sTempSqlStr);
		init(sIdNo);
		init(sBeginTime);
		init(sEndTime);
		/*ng���� by yaoxc end*/

		memset(vTmpUnitId, 0, sizeof(vTmpUnitId));
		memset(vUnit_Id, 0, sizeof(vUnit_Id));
		memset(vUnit_Code, 0, sizeof(vUnit_Code));
		memset(vGrpCust_Id, 0, sizeof(vGrpCust_Id));
		memset(vService_No, 0, sizeof(vService_No));
		memset(vGrp_Userno, 0, sizeof(vGrp_Userno));
		memset(vGrp_Smcode, 0, sizeof(vGrp_Smcode));
		memset(vProduct_Type, 0, sizeof(vProduct_Type));
		memset(vEnd_Date, 0, sizeof(vEnd_Date));
		memset(vGroup_Type_Code, 0, sizeof(vGroup_Type_Code));
		memset(vDepartment, 0, sizeof(vDepartment));
		memset(vLmtFee, 0, sizeof(vLmtFee));
		memset(vShort_PhoneNo, 0, sizeof(vShort_PhoneNo));
		memset(vBill_Code, 0, sizeof(vBill_Code));
		memset(mProductCode, 0, sizeof(mProductCode));
		memset(vGrpUserName, 0, sizeof(vGrpUserName));
		memset(vGrp_Flag, 0, sizeof(vGrp_Flag));
		memset(vRun_Time, 0, sizeof(vRun_Time));    /*add by miaoyl at 20090629 */
		memset(vOrg_Id, 0, sizeof(vOrg_Id));        /*add by miaoyl at 20090708 */
		memset(mTotalDate, 0, sizeof(mTotalDate));
		strncpy(mTotalDate, grpUser->opTime, 8);

		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
		ret=0;
		ret = sGetLoginOrgid(grpUser->loginNo,vOrg_Id);
		if(ret <0)
		{
			strcpy(retCode, "332012");
			sprintf(retMsg, "��ѯorg_idʧ��![%d]",SQLCODE);
			return -1;
		}
		Trim(vOrg_Id);
	   /*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

		/*Լ��:����״̬Ϊ�����̶�������dCustMsg��ֻ����һ����¼*/
		memset(mIdNo, 0, sizeof(mIdNo));
		EXEC SQL        SELECT to_char(id_no), To_Char(bill_type)
								  INTO :mIdNo, :vBill_Code
								  FROM dCustMsg
								 WHERE phone_no = :grpUser->phoneNo
								   AND substr(run_code,2,1) < 'a';
		pubLog_Debug(_FFL_, "CreateGrpUser", "", "mIdNo = [%s]",mIdNo);
		pubLog_Debug(_FFL_, "CreateGrpUser", "", "GrpUser->phone_no = [%s]",grpUser->phoneNo);
		if (mIdNo[0] == '\0')
		{
				/*
				 *�̶����뿪��ʱ��ҵ�����ʹ���sm_codeͳһ��"gd", ��������service_type��"40".
				 */
				pubLog_Debug(_FFL_, "CreateGrpUser", "", "��ʼ����fCreateUser�����û�");
				if (fCreateUser(transIN, transOUT, temp_buf, grpUser->loginAccept, grpUser->opCode,
										grpUser->loginNo,grpUser->orgCode, grpUser->phoneNo, grpUser->smCode,
										grpUser->serviceType,grpUser->belongCode,grpUser->opTime,grpUser->custId,
										grpUser->groupId,grpUser->orgId,retCode, retMsg) != 0)
				{
#ifdef _DEBUG_
						pubLog_Debug(_FFL_, "CreateGrpUser", "", "fCreateUser����ʧ��[%s][%s]",retCode, retMsg);
#endif
						return -1;
				}
				pubLog_Debug(_FFL_, "CreateGrpUser", "", "��������fCreateUser�����û�");

				memset(mIdNo, 0, sizeof(mIdNo));
				EXEC SQL        SELECT to_char(id_no)
										  INTO :mIdNo
										  FROM dCustMsg
										 WHERE phone_no = :grpUser->phoneNo
										   AND substr(run_code,2,1) < 'a';
				if (mIdNo[0] == '\0')
				{
						strcpy(retCode,"140201");
						sprintf(retMsg, "��ѯ��dCustMsg::mPhoneNo=[%s]����!", grpUser->phoneNo);
						return -1;
				}
		}
		else
		{
			pubLog_Debug(_FFL_, "CreateGrpUser", "", "���û��Ѿ�����,�����ٿ���");
		}
		Trim(mIdNo);
		mCount = 0;
		/*edit by cencm at 2011-5-20 20:36:45 
		EXEC SQL        SELECT COUNT(*)
								  INTO :mCount
								  FROM dGrpUserMebMsg
								 WHERE id_no = to_number(:grpUser->grpIdNo)
								   AND member_id = to_number(:mIdNo)
								   AND end_time > to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS');*/
		EXEC SQL        SELECT COUNT(*)
								  INTO :mCount
								  FROM dGrpUserMebMsg
								 WHERE id_no = to_number(:grpUser->grpIdNo)
								   AND member_id = to_number(:mIdNo)
								   AND end_time > Last_Day(sysdate)+1;
		if ((mCount > 0) && ((grpUser->createFlag & CGU_NEW_USER) != 0))
		{
				strcpy(retCode,"140202");
				sprintf(retMsg, "����[%s]�Ѿ����뼯��[%s]��Ʒ!", grpUser->phoneNo, grpUser->grpIdNo);
				return -1;
		}
		else if ((mCount == 0) && ((grpUser->createFlag & CGU_RESUME_USER) != 0))
		{
				strcpy(retCode,"140203");
				sprintf(retMsg, "����[%s]δ���뼯�Ż��Ѿ�ȡ��[%s]��Ʒ!", grpUser->phoneNo, grpUser->grpIdNo);
				return -1;
		}

	/*��ѯ�����û�����*/
	EXEC SQL SELECT Cust_Id, User_No, Sm_Code, To_Char(Bill_Date, 'YYYYMMDD')
			   INTO :vGrpCust_Id, :vGrp_Userno, :vGrp_Smcode, :vEnd_Date
			   FROM dGrpUserMsg
			  WHERE ID_NO = To_Number(:grpUser->grpIdNo)
				AND Bill_Date > Last_Day(sysdate) + 1;
	if (SQLCODE !=0){
		strcpy(retCode, "140203");
		sprintf(retMsg, "��ѯdGrpUserMsg����[%d]",SQLCODE);
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
		return -1;
	}
	Trim(vGrpCust_Id);
	Trim(vGrp_Userno);
	Trim(vProduct_Type);
	Trim(vGrp_Smcode);

	/*�Ӽ��ŷ���Ʒ�Ʊ�õ�grp_flag���ж��Ƿ���Ҫ¼���ͬ�����õ�GRPϵͳ�Ĳ�Ʒ����*/
	EXEC SQL SELECT GRP_FLAG, product_code
			   INTO :vGrp_Flag, :mProductCode
			   FROM sGrpSmCode
			  WHERE SM_CODE = :vGrp_Smcode;
	if(SQLCODE != 0)
	{
		strcpy(retCode, "140204");
		sprintf(retMsg, "�޸ò�ƷƷ��[%s]����!", vGrp_Smcode);
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
		return -1;
	}
	Trim(vGrp_Flag);
	Trim(mProductCode);

		if (vGrp_Flag[0] == 'Y')
		{
			/*��ѯ���ſͻ�����*/
			memset(grpOrgCode, 0, sizeof(grpOrgCode));
#if PROVINCE_RUN != PROVINCE_JILIN
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
			/*
			EXEC SQL SELECT To_Char(Unit_Id), Trim(Unit_Code), nvl(Service_No, ' '), org_code
					   INTO :vUnit_Id, :vUnit_Code, :vService_No, :grpOrgCode
					   FROM dCustDocOrgAdd
					  WHERE Cust_Id = To_Number(:vGrpCust_Id);
			*/
		  EXEC SQL SELECT To_Char(Unit_Id), Trim(Unit_Id), nvl(Service_No, ' ')
			   INTO :vUnit_Id, :vUnit_Code, :vService_No
			   FROM dCustDocOrgAdd
		  WHERE Cust_Id = To_Number(:vGrpCust_Id);
		  /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
#else
		  /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
		   /* EXEC SQL SELECT To_Char(Unit_Id), Trim(Unit_Code), nvl(Service_No, ' '), 'unknown'
					   INTO :vUnit_Id, :vUnit_Code, :vService_No, :grpOrgCode
					   FROM dCustDocOrgAdd
					  WHERE Cust_Id = To_Number(:vGrpCust_Id);*/
		 EXEC SQL SELECT To_Char(Unit_Id), Trim(Unit_Id), nvl(Service_No, ' ')
					   INTO :vUnit_Id, :vUnit_Code, :vService_No
					   FROM dCustDocOrgAdd
					  WHERE Cust_Id = To_Number(:vGrpCust_Id);
		  /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
#endif

			if (SQLCODE !=0){
				strcpy(retCode, "140203");
				sprintf(retMsg, "��ѯdCustDocOrgAdd����[%d]",SQLCODE);
				PUBLOG_DBDEBUG("CreateGrpUser");
				pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
				return -1;
			}
			Trim(grpOrgCode);
		}

		/* yangzh add 2005/04/14 һ���û����ܼ���ͬһ��Ʒ�Ƶ����������û� begin */
	mCount = 0;
	/*edit by cencm at  2011-5-20 20:38:46
	EXEC SQL    SELECT COUNT(*)
				  INTO :mCount
				  FROM dGrpUserMebMsg a, dGrpUserMsg b
				 WHERE a.id_no = b.id_no
				   AND a.member_id = To_Number(:mIdNo)
				   AND a.end_time > to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS')
				   AND b.sm_code = :vGrp_Smcode
				   AND b.bill_date > Last_Day(sysdate) + 1;*/
	EXEC SQL    SELECT COUNT(*)
				  INTO :mCount
				  FROM dGrpUserMebMsg a, dGrpUserMsg b
				 WHERE a.id_no = b.id_no
				   AND a.member_id = To_Number(:mIdNo)
				   AND a.end_time > Last_Day(sysdate) + 1
				   AND b.sm_code = :vGrp_Smcode
				   AND b.bill_date > Last_Day(sysdate) + 1;	
	if (SQLCODE !=0)
	{
		strcpy(retCode, "140233");
		sprintf(retMsg,"��ѯ�Ƿ����ͬһƷ���û�ʧ��[%d]",SQLCODE);
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
		return -1;
	}

		if ((mCount > 0) && ((grpUser->createFlag & CGU_NEW_USER) != 0))
	{
		strcpy(retCode, "140234");
		sprintf(retMsg, "���û��Ѿ�����ͬһƷ�Ƶ����������û�");
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
		return -1;
	}

	pubLog_Debug(_FFL_, "CreateGrpUser", "", "�û�û�м���ͬһƷ�Ƶ���������");
	pubLog_Debug(_FFL_, "CreateGrpUser", "", "vGrp_Flag = [%s]",vGrp_Flag);
		/* lixg add :2005-9-3 for VipSystem request */
		if (vGrp_Flag[0] == 'Y')
		{
/* chenxuan boss3 ���ٴ����ѷϳ���dGrpMemberuserMsg
				mCount = 0;
				 EXEC SQL  SELECT COUNT(*)
			  INTO :mCount
				  FROM  dGrpMemberUserMsg
				where member_id = to_number(:mIdNo)
				and   product_code <> '99';

				pubLog_Debug(_FFL_, "CreateGrpUser", "", "mIdNo = [%s] grpIdNo = [%s] mCount = [%d]", mIdNo, grpUser->grpIdNo,mCount);

				if(mCount == 0)
				{
						EXEC SQL delete from dGrpMemberuserMsg
						where member_id = to_number(:mIdNo);
						if(SQLCODE != 0 && SQLCODE != 1403)
						{
						strcpy(retCode, "140284");
						sprintf(retMsg, "Del dGrpMemberuserMsg99[%s]  Error[%d]!", mIdNo,SQLCODE);
						pubLog_Debug(_FFL_, "CreateGrpUser", "", "retCode = [%s],retMsg = [%s]",retCode, retMsg);
						return -1;
						}
						EXEC SQL delete from dGrpMemberMsg
						where   id_no = to_number(:mIdNo);
						if(SQLCODE != 0 && SQLCODE != 1403)
						{
						strcpy(retCode, "140284");
						sprintf(retMsg, "Del dGrpMemberMsg99[%s] Error[%d]!", vUnit_Id,SQLCODE);
						pubLog_Debug(_FFL_, "CreateGrpUser", "", "%s", retMsg);
						return -1;
						}
				}
				pubLog_Debug(_FFL_, "CreateGrpUser", "", "mTotalDate:[%s]mLoginAccept[%s]opCode[%s]mOpTime[%s]",mTotalDate,grpUser->loginAccept,grpUser->opCode,grpUser->opTime);
*/
#if PROVINCE_RUN != PROVINCE_JILIN
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*          EXEC SQL INSERT INTO dGrpMemberMsgHis
					 (unit_id, unit_code, unit_id_no,id_no, big_flag, phone_no, Name, user_name,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  UPDATE_LOGIN, UPDATE_ACCEPT,
					  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,
					  GROUP_ID)
			   SELECT unit_id, unit_code, unit_id_no,id_no, big_flag, phone_no, Name, user_name,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  :grpUser->loginNo, to_number(:grpUser->loginAccept),
					  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd',
					  GROUP_ID
				 FROM dGrpMemberMsg
				WHERE ID_NO = To_Number(:mIdNo);
	*/

			EXEC SQL INSERT INTO dGrpMemberMsgHis
				 (unit_id,id_no, big_flag, phone_no, user_name,
				  belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date,
				  stop_flag,
				  service_access, sm_code,
				  note,
				  cust_id, modify_date, high_value_flag,
				  group_member_consumption_code, member_cust_id,
				  UPDATE_LOGIN, UPDATE_ACCEPT,
				  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,
				  GROUP_ID,
				  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				  correct_note,ORG_ID)
		   SELECT unit_id,id_no, big_flag, phone_no, user_name,
				  belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date,
				  stop_flag,
				  service_access, sm_code,
				  note,
				  cust_id, modify_date, high_value_flag,
				  group_member_consumption_code, member_cust_id,
				  :grpUser->loginNo, to_number(:grpUser->loginAccept),
				  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd',
				  GROUP_ID,
				  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				  correct_note,ORG_ID
			 FROM dGrpMemberMsg
			WHERE ID_NO = To_Number(:mIdNo);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---END*/
#else
		/* LINA VIP2.0 2006_11_05 begin here*/
		/*ȡvip��֯��dgrpgroups��org_code*/
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "CreateGrpUser", "", "��¼dCustDocOrgAddHis���¼");
		#endif
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/

		/* edit by miaoyl
		EXEC SQL SELECT Org_Code
		   INTO :sIn_Grp_OrgCode
		   FROM dCustDocOrgAdd
		   WHERE Cust_Id = To_Number(:vGrpCust_Id);
		if(SQLCODE!=OK)
		{
		strcpy(retCode,"110001");
		strcpy(retMsg,"��ѯvip��֯��dgrpgroups��org_code����!");
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser","190036",retMsg);
		return 190036 ;
		}*/
		/*
		EXEC SQL INSERT INTO dGrpMemberMsgHis
				 (unit_id, unit_code, id_no, big_flag, phone_no, Name, user_name,
				  unit_id_no,
				  male_flag, belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
				  stop_flag, month_fee, important_family, consume_habit_code,
				  service_access, love_redound_code, love_service_mode, sm_code,
				  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
				  cust_id, modify_date, load_date, high_value_flag, group_type_code,
				  gmember_status_code, phone_status_code, group_member_level_code,
				  group_member_consumption_code, deal_indicate, member_cust_id,
				  UPDATE_LOGIN, UPDATE_ACCEPT,
				  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, ORG_CODE,
				  GROUP_ID)
		   SELECT unit_id, unit_code, id_no, big_flag, phone_no, Name, user_name,
				  unit_id_no,
				  male_flag, belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
				  stop_flag, month_fee, important_family, consume_habit_code,
				  service_access, love_redound_code, love_service_mode, sm_code,
				  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
				  cust_id, modify_date, load_date, high_value_flag, group_type_code,
				  gmember_status_code, phone_status_code, group_member_level_code,
				  group_member_consumption_code, deal_indicate, member_cust_id,
				  :grpUser->loginNo, to_number(:grpUser->loginAccept),
				  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd', :sIn_Grp_OrgCode,
				  group_id
			 FROM dGrpMemberMsg
			WHERE ID_NO = To_Number(:mIdNo);*/

			EXEC SQL INSERT INTO dGrpMemberMsgHis
				 (unit_id, id_no, big_flag, phone_no, user_name,
				  belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date,
				  stop_flag,
				  service_access, sm_code,
				  note,
				  cust_id, modify_date,high_value_flag,
				  group_member_consumption_code,  member_cust_id,
				  UPDATE_LOGIN, UPDATE_ACCEPT,
				  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,
				  GROUP_ID,
				  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				  correct_note,ORG_ID)
		   SELECT unit_id, id_no, big_flag, phone_no, user_name,
				  belong_code, service_no, card_code, member_code, card_no,
				  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
				  contact_phone, fax, email, love, ingrp_date, out_date,
				  stop_flag,
				  service_access, sm_code,
				  note,
				  cust_id, modify_date, high_value_flag,
				  group_member_consumption_code,  member_cust_id,
				  :grpUser->loginNo, to_number(:grpUser->loginAccept),
				  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd',
				  group_id,
				  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				  correct_note,:vOrg_Id
			 FROM dGrpMemberMsg
			WHERE ID_NO = To_Number(:mIdNo);

	  /*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
			/*LINA END*/
#endif
			if (SQLCODE != 0 && SQLCODE != 1403)
			{
					strcpy(retCode,"140201");
					sprintf(retMsg,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
					PUBLOG_DBDEBUG("CreateGrpUser");
					pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
					return -1;
			}

			EXEC SQL delete FROM dGrpMemberMsg
				 WHERE ID_NO = To_Number(:mIdNo);
			if (SQLCODE != 0 && SQLCODE != 1403){
					strcpy(retCode,"140201");
					sprintf(retMsg,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
					PUBLOG_DBDEBUG("CreateGrpUser");
					pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
					return -1;
			}

	   /*--- ��ͻ���ṹ���� add by MIAOYL at 20090706---begin*/
	   /*ȡ����ʱ��*/
		EXEC SQL select to_char(open_time,'yyyymmdd hh24:mi:ss') INTO :vRun_Time
				   from dCustMsg
				  where id_no =To_Number(:grpUser->grpIdNo);
	   if (SQLCODE != 0)
		{
			strcpy(retCode, "140221");
			sprintf(retMsg,"��ѯdCustMsg����ʱ��open_time�����쳣[%d][%s]",SQLCODE,grpUser->grpIdNo);
			pubLog_Debug(_FFL_, "CreateGrpUser", "", "%s", retMsg);
			return 190158;
		}

		/*--- ��ͻ���ṹ���� add by MIAOYL at 20090706---end*/
#if PROVINCE_RUN != PROVINCE_JILIN
/*--- ��ͻ���ṹ���� edit by MIAOYL at 20090629---begin*/
/*
		EXEC SQL INSERT INTO dGrpMemberMsg
			 (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO,ID_NO,
			  PHONE_NO, NAME, USER_NAME, BELONG_CODE,
			  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			  big_flag, male_flag,
			  card_code, member_code,
			  ingrp_date, service_access, sm_code,
			  pay_code, bill_code, load_date,
			  group_type_code, gmember_status_code,
			  pay_fee_flag, payed_fee, short_phoneno,
			  service_no, dept, note,ORG_CODE,
			  group_id)
	   SELECT to_number(:vGrpCust_Id), to_number(:vUnit_Id), :vUnit_Code, to_number(:grpUser->grpIdNo), to_number(:mIdNo),
			  :grpUser->phoneNo, substr(cust_name,1,10), substr(cust_name,1,10), region_code||district_code||town_code,
			  Id_Type, Id_Iccid, substr(CONTACT_ADDRESS,1,30), CONTACT_POST,
			  CONTACT_PHONE, substrb(CONTACT_FAX,1,10), CONTACT_EMAILL, to_number(:grpUser->custId),
			  '0', '2',
			  '99', 'C1',
			  To_Char(sysdate, 'yyyy-mm-dd'), ' ', :grpUser->smCode,
			  '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
			  '2', '1',
			  '0', 0, '0',
			  :vService_No, 'δ֪', ' ',to_number(:grpOrgCode),
			  group_id
				FROM dCustDoc
				WHERE Cust_Id = ( select cust_id
									from dCustMsg
								   where phone_no = :grpUser->phoneNo
										 and substr(run_code,2,1) < 'a' );
*/
		EXEC SQL INSERT INTO dGrpMemberMsg
				 (CUST_ID, UNIT_ID,ID_NO,
				  PHONE_NO, USER_NAME, BELONG_CODE,
				  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
				  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
				  big_flag,
				  card_code, member_code,
				  ingrp_date, service_access, sm_code,
				  service_no, dept, note,
				  group_id,
				  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				  correct_note,ORG_ID)
		   SELECT to_number(:vGrpCust_Id), to_number(:vUnit_Id), to_number(:mIdNo),
				  :grpUser->phoneNo, substr(cust_name,1,10), region_code||district_code||town_code,
				  Id_Type, Id_Iccid, substr(CONTACT_ADDRESS,1,30), CONTACT_POST,
				  CONTACT_PHONE, substrb(CONTACT_FAX,1,10), CONTACT_EMAILL, to_number(:grpUser->custId),
				  '0',
				  '99', 'C1',
				  sysdate, ' ', :grpUser->smCode,
				  :vService_No, 'δ֪', ' ',
				  group_id,
				  NULL,NULL,NULL,NULL,
				  NULL,NULL,NULL,NULL,
				  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2',NULL,NULL,
				  NULL,:vOrg_Id
		  FROM dCustDoc
		  WHERE Cust_Id = ( select cust_id
							from dCustMsg
						   where phone_no = :grpUser->phoneNo
								 and substr(run_code,2,1) < 'a' );

/*--- ��ͻ���ṹ���� edit by MIAOYL at 20090629---END*/
#else
/*--- ��ͻ���ṹ���� edit by MIAOYL at 20090629---begin*/
		/*LINA ADD HERE*/
		/*
		EXEC SQL INSERT INTO dGrpMemberMsg
			 (CUST_ID, UNIT_ID, UNIT_CODE, ID_NO,
			  UNIT_ID_NO,
			  PHONE_NO, NAME, USER_NAME, BELONG_CODE,
			  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			  big_flag, male_flag,
			  card_code, member_code,
			  ingrp_date, service_access, sm_code,
			  pay_code, bill_code, load_date,
			  group_type_code, gmember_status_code,
			  pay_fee_flag, payed_fee, short_phoneno,
			  service_no, dept, note, org_code,
			  group_id)
	   SELECT to_number(:vGrpCust_Id), to_number(:vUnit_Id), :vUnit_Code,  to_number(:mIdNo),
			  to_number(:grpUser->grpIdNo),
			  :grpUser->phoneNo, substr(cust_name,1,10), substr(cust_name,1,10), region_code||district_code||town_code,
			  Id_Type, Id_Iccid, substr(CONTACT_ADDRESS,1,30), CONTACT_POST,
			  CONTACT_PHONE, substrb(CONTACT_FAX,1,10), CONTACT_EMAILL, to_number(:grpUser->custId),
			  '0', '2',
			  '99', 'C1',
			  To_Char(sysdate, 'yyyy-mm-dd'), ' ', :grpUser->smCode,
			  '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
			  '2', '1',
			  '0', 0, '0',
			  :vService_No, 'δ֪', ' ', :sIn_Grp_OrgCode,
			  group_id
				FROM dCustDoc
				WHERE Cust_Id = ( select cust_id
									from dCustMsg
								   where phone_no = :grpUser->phoneNo
										 and substr(run_code,2,1) < 'a' );
			*/
		   /*LINA END HERE*/
		EXEC SQL INSERT INTO dGrpMemberMsg
			 (CUST_ID, UNIT_ID, ID_NO,
			  PHONE_NO, USER_NAME, BELONG_CODE,
			  ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			  CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			  big_flag,
			  card_code, member_code,
			  ingrp_date, service_access, sm_code,
			  service_no, dept, note,
			  group_id,
			  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			  correct_note,ORG_ID)
	   SELECT to_number(:vGrpCust_Id), to_number(:vUnit_Id),  to_number(:mIdNo),
			  :grpUser->phoneNo, substr(cust_name,1,10), region_code||district_code||town_code,
			  Id_Type, Id_Iccid, substr(CONTACT_ADDRESS,1,30), CONTACT_POST,
			  CONTACT_PHONE, substrb(CONTACT_FAX,1,10), CONTACT_EMAILL, to_number(:grpUser->custId),
			  '0',
			  '99', 'C1',
			  sysdate, ' ', :grpUser->smCode,
			  :vService_No, 'δ֪', ' ',
			  group_id,
			  '','','','',
			  '','','','',
			  to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',
			  '',:vOrg_Id
		FROM dCustDoc
		WHERE Cust_Id = ( select cust_id
							from dCustMsg
						   where phone_no = :grpUser->phoneNo
								 and substr(run_code,2,1) < 'a' );

/*--- ��ͻ���ṹ���� edit by MIAOYL at 20090629---end*/

#endif
		if (SQLCODE != 0){
				strcpy(retCode, "140203");
				sprintf(retMsg, "��ѯdCustDoc��,����dGrpMemberMsg��Ա���Ϸ����쳣[%d]",SQLCODE);
				PUBLOG_DBDEBUG("CreateGrpUser");
				pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
				return -1;
		}

				EXEC SQL        INSERT INTO dGrpMemberUserMsgHis
						(
						  UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD,
						  OP_TIME, PRODUCT_CODE,
						  UPDATE_ACCEPT, UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN,
						  UPDATE_TYPE, UPDATE_CODE
						)
						select
								to_number(:vUnit_Id), to_number(:grpUser->grpIdNo), to_number(:mIdNo), :grpUser->phoneNo, ' ',
								To_Date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), product_code,
								to_number(:grpUser->loginAccept), TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'),TO_NUMBER(:mTotalDate), :grpUser->loginNo,
					'a', product_code
						from    sGrpSmCode
				  where   sm_code = :vGrp_Smcode;

				if (SQLCODE != 0)
				{
						strcpy(retCode,"140204");
						sprintf(retMsg, "�����dGrpMemberUserMsgHis����[%d]!", SQLCODE);
						PUBLOG_DBDEBUG("CreateGrpUser");
						pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
						return -1;
				}

				mCount  = 0;
				EXEC SQL        select count(*)
										  into :mCount
										  FROM dGrpMemberUserMsg a
										 WHERE id_no = to_number(:grpUser->grpIdNo)
										   AND MEMBER_ID = to_number(:mIdNo)
										   AND PRODUCT_CODE =:mProductCode;
				if(mCount > 0)
				{
						/*
						strcpy(retCode,"140204");
						sprintf(retMsg, "�û�[%s]�Ѽ���BOSS��������VPMN����!", grpUser->phoneNo);
						return -1;
						*/
				}
				else
				{
						EXEC SQL        INSERT INTO dGrpMemberUserMsg
								(
								  UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD, OP_TIME, PRODUCT_CODE
								)
						select
										to_number(:vUnit_Id), to_number(:grpUser->grpIdNo), to_number(:mIdNo), :grpUser->phoneNo, ' ',
										To_Date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), product_code
						from    sGrpSmCode
						where   sm_code = :vGrp_Smcode;
						if (SQLCODE != 0)
						{
								strcpy(retCode,"140204");
								pubLog_Debug(_FFL_, "CreateGrpUser", "", "vUnit_Id:%s,mGrpIdNo:%s,mIdNo:%s,mPhoneNo:%s,mOpTime:%s",vUnit_Id,grpUser->grpIdNo,mIdNo,grpUser->phoneNo,grpUser->opTime);
								sprintf(retMsg, "�����dGrpMemberUserMsg����[%d]!",SQLCODE);
								PUBLOG_DBDEBUG("CreateGrpUser");
								pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
								return -1;
						}
				}
		}


	/*yangzh add 2005-01-24 �����û����뼯��ʱ����attr_codeΪ���ſͻ���־*/
	/*ng����
	EXEC SQL    UPDATE dcustmsg
							   SET attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)
							 WHERE phone_no = :grpUser->phoneNo
							   AND substr(run_code,2,1) < 'a';
	if (SQLCODE != 0)
	{
		strcpy(retCode, "140203");
		sprintf(retMsg, "����dCustMsg��attr_code����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("CreateGrpUser");
		pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
		return -1;
	}
	ng����*/
	/*ng���� by yaoxc begin*/
	init(sTempSqlStr);
	sprintf(sTempSqlStr,"select to_char(id_no) from dcustmsg "
						"where phone_no = rpad(:v1,15) "
						"and  substr(run_code,2,1) < 'a'");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
	EXEC SQL declare ngcur01 cursor for sql_str;
	EXEC SQL open ngcur01 using :grpUser->phoneNo;
	for(;;)
	{
		EXEC SQL FETCH ngcur01 into :sIdNo;
		if(1403==SQLCODE) break;
		if((0!=SQLCODE) & (1403!=SQLCODE))
		{
			sprintf(retMsg, "��ѯdcustmsg����!\n");
			strcpy(retCode, "140203");
			EXEC SQL CLOSE ngcur01;
			return -1;
		}

		init(v_parameter_array);

		strcpy(v_parameter_array[0],sIdNo);

		v_ret=OrderUpdateCustMsg(ORDERIDTYPE_USER,sIdNo,100,
								grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
								sIdNo,
								" attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1) ","",v_parameter_array);
		printf("OrderUpdateCustMsg ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode, "140203");
			sprintf(retMsg, "����dCustMsg��attr_code����![%d]",SQLCODE);
			PUBLOG_DBDEBUG("CreateGrpUser");
			pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
			EXEC SQL CLOSE ngcur01;
			return -1;
		}
		icount++;
	}
	EXEC SQL CLOSE ngcur01;
	if(icount == 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-13","ɾ��dConUserMsg��ʧ��!");
		return -13;
	}
	/*ng���� by yaoxc end*/

		if ((grpUser->createFlag & CGU_NEW_USER) != 0)
		{		/*ng����
				EXEC SQL        INSERT INTO dGrpUserMebMsg
										(
												ID_NO,MEMBER_ID,MEMBER_NO,BAK_FIELD,
												BEGIN_TIME, END_TIME
										)
										VALUES
										(
												to_number(:grpUser->grpIdNo), to_number(:mIdNo), :grpUser->phoneNo, ' ',
												To_Date(:grpUser->effectTime, 'YYYYMMDD HH24:MI:SS'), To_Date(:vEnd_Date, 'YYYYMMDD')
										);
				ng����*/
				/*ng���� by yaoxc begin*/
				strcpy(tdGrpUserMebMsg.sIdNo    	, grpUser->grpIdNo);
				strcpy(tdGrpUserMebMsg.sMemberId 	, mIdNo);
				strcpy(tdGrpUserMebMsg.sMemberNo 	, grpUser->phoneNo);
				strcpy(tdGrpUserMebMsg.sBeginTime 	, grpUser->effectTime);
				strcpy(tdGrpUserMebMsg.sEndTime 	, vEnd_Date);
				strcpy(tdGrpUserMebMsg.sBakField   	, " ");

				/*********************ng���� liuzhou ����id_typeһ�£��Ĵ���Աid  ******************************/
				/*v_ret=OrderInsertGrpUserMebMsg(ORDERIDTYPE_USER,grpUser->grpIdNo,100,
										grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
										tdGrpUserMebMsg);*/

				v_ret=OrderInsertGrpUserMebMsg(ORDERIDTYPE_USER, mIdNo,100,
										grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
										tdGrpUserMebMsg);

				/*********************ng���� liuzhou ����id_typeһ�£��ô���Աid  ******************************/

				printf("OrderInsertGrpUserMebMsg ,ret=[%d]\n",v_ret);
				/*ng���� by yaoxc end*/
		}
		else if ((grpUser->createFlag & CGU_RESUME_USER) != 0)
		{		/*ng����
				EXEC SQL        UPDATE dGrpUserMebMsg
										   SET END_TIME = To_Date(:vEnd_Date, 'YYYYMMDD')
										 WHERE ID_NO= to_number(:grpUser->grpIdNo)
										   AND MEMBER_ID = to_number(:mIdNo)
										   AND END_TIME > To_Date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS');
				ng����*/
				/*ng���� by yaoxc begin*/
				init(sTempSqlStr);
				sprintf(sTempSqlStr,"select to_char(begin_time,'YYYYMMDD HH24:MI:SS'),to_char(end_time,'YYYYMMDD HH24:MI:SS') "
									"from dGrpUserMebMsg where ID_NO= to_number(:v1) "
									"and  MEMBER_ID = to_number(:v2) "
									"and  END_TIME > To_Date(:v3, 'YYYYMMDD HH24:MI:SS')");
				EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
				EXEC SQL declare ngcur02 cursor for sql_str;
				EXEC SQL open ngcur02 using :grpUser->grpIdNo,:mIdNo,:grpUser->opTime;
				for(;;)
				{
					memset(&oldIndex,0,sizeof(oldIndex));
					memset(&newIndex,0,sizeof(newIndex));
					EXEC SQL FETCH ngcur02 into :sBeginTime,:sEndTime;
					if((0!=SQLCODE)&&(1403!=SQLCODE))
					{
						sprintf(retMsg, "��ѯdGrpUserMebMsg����!\n");
						strcpy(retCode, "140205");
						EXEC SQL CLOSE ngcur02;
						return -1;
					}
					if(1403==SQLCODE) break;

					strcpy(oldIndex.sIdNo  	    , grpUser->grpIdNo);
					strcpy(oldIndex.sMemberId  	, mIdNo);
					strcpy(oldIndex.sBeginTime 	, sBeginTime);
					strcpy(oldIndex.sEndTime   	, sEndTime);
					strcpy(newIndex.sIdNo  	    , grpUser->grpIdNo);
					strcpy(newIndex.sMemberId  	, mIdNo);
					strcpy(newIndex.sBeginTime 	, sBeginTime);
					strcpy(newIndex.sEndTime   	, vEnd_Date);

					init(v_parameter_array);

					strcpy(v_parameter_array[0],vEnd_Date);
					strcpy(v_parameter_array[1],grpUser->grpIdNo);
					strcpy(v_parameter_array[2],mIdNo);
					strcpy(v_parameter_array[3],sBeginTime);
					strcpy(v_parameter_array[4],sEndTime);

				/*********************ng���� liuzhou ����id_typeһ�£��Ĵ���Աid  ******************************/
					/*v_ret=OrderUpdateGrpUserMebMsg(ORDERIDTYPE_USER,grpUser->grpIdNo,100,
											grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
											oldIndex,newIndex,
											" END_TIME = To_Date(:vEnd_Date, 'YYYYMMDD') ","",v_parameter_array);*/

					v_ret=OrderUpdateGrpUserMebMsg(ORDERIDTYPE_USER, mIdNo,100,
											grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
											oldIndex,newIndex,
											" END_TIME = To_Date(:vEnd_Date, 'YYYYMMDD') ","",v_parameter_array);

					/*********************ng���� liuzhou ����id_typeһ�£��Ĵ���Աid  ******************************/



					printf("OrderUpdateGrpUserMebMsg ,ret=[%d]\n",v_ret);
				}
				EXEC SQL CLOSE ngcur02;
		}
		if (v_ret != 0)
		{
				strcpy(retCode,"140204");
				sprintf(retMsg, "�����dGrpUserMebMsg����!");
				PUBLOG_DBDEBUG("CreateGrpUser");
				pubLog_DBErr(_FFL_,"CreateGrpUser",retCode,retMsg);
				return -1;
		}
		/*ng���� by yaoxc end*/
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "CreateGrpUser", "", "CreateGrpUser:mContractNo=[%s]",grpUser->contractNo);
#endif
/*
		if (grpUser->contractNo[0] != '\0')
		{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "CreateGrpUser", "", "CreateGrpUser:mContractNo[0]=[%c]",grpUser->contractNo[0]);
#endif
				mCount = 0;
				EXEC SQL        SELECT COUNT(*)
										  INTO :mCount
										  FROM dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND serial_no = 0
										   AND contract_no = :grpUser->contractNo;
				if (mCount > 0 )
				{
						strcpy(retCode,"140205");
						sprintf(retMsg, "�û�[%s]�Ѵ��ڸ����ʻ�[%s]����Ϣ������������!", mIdNo, grpUser->contractNo);
						return -1;
				}

				mBillOrder = 0;
				EXEC SQL        SELECT nvl(max(bill_order), 0)
										  INTO :mBillOrder
										  FROM dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND bill_order != 99999999;

				mBillOrder += 1;
				EXEC SQL INSERT INTO dConUserMsgHis
								(
										Id_No,        Contract_No,  Serial_No,
										Bill_Order,   Pay_Order,    Begin_YMD,
										Begin_TM,     End_YMD,      End_TM,
										Limit_Pay,    Rate_Flag,    Stop_Flag,
										Update_Accept,Update_Login, Update_Date,
										Update_Time,  Update_Code,  Update_Type
								)
								VALUES
								(
										TO_NUMBER(:mIdNo),TO_NUMBER(:grpUser->contractNo),0,
										:mBillOrder,     1,           :mTotalDate,
										to_char(TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'), 'HH24MISS'),   '20500101',   '235959',
										9999999999,  'N',          'Y',
										to_number(:grpUser->loginAccept),:grpUser->loginNo,   TO_NUMBER(:mTotalDate),
										TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'),        :grpUser->opCode,    'a'
								);
				if (SQLCODE != 0) {
						sprintf(retMsg, "����dConUserMsgHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140206");
						return -1;
				}

				EXEC SQL        INSERT INTO dConUserMsg
										(
												Id_No,        Contract_No,  Serial_No,
												Bill_Order,   Pay_Order,    Begin_YMD,
												Begin_TM,     End_YMD,      End_TM,
												Limit_Pay,    Rate_Flag,    Stop_Flag
										)
										VALUES
										(
												TO_NUMBER(:mIdNo),TO_NUMBER(:grpUser->contractNo),0,
												:mBillOrder,     1,           :mTotalDate,
												to_char(TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'), 'HH24MISS'),   '20500101',   '235959',
												9999999999,  'N',          'Y'
										);
				if (SQLCODE != 0)
				{
						sprintf(retMsg, "����dConUserMsg����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140207");
						return -1;
				}

#if PROVINCE_RUN != PROVINCE_JILIN
				EXEC SQL        INSERT INTO wConUserChg
										(
												OP_NO,  OP_TYPE,        ID_NO,  CONTRACT_NO,    BILL_ORDER,
												CONUSER_UPDATE_TIME,    LIMIT_PAY,      RATE_FLAG,
												FEE_CODE,       DETAIL_CODE,FEE_RATE,   OP_TIME, phone_no
										)
										SELECT
												sMaxBillChg.nextVal,    '10',   ID_NO,  CONTRACT_NO,Bill_Order,
												to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'), LIMIT_PAY, RATE_FLAG,
												'*', '*',0,to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'),
														:grpUser->phoneNo
										  FROM dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND bill_order = :mBillOrder;
#else
				EXEC SQL        INSERT INTO wConUserChg
										(
												OP_NO,  OP_TYPE,        ID_NO,  CONTRACT_NO,    BILL_ORDER,
												CONUSER_UPDATE_TIME,    LIMIT_PAY,      RATE_FLAG,
												FEE_CODE,       DETAIL_CODE,FEE_RATE,   OP_TIME
										)
										SELECT
												sMaxBillChg.nextVal,    '10',   ID_NO,  CONTRACT_NO,Bill_Order,
												to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'), LIMIT_PAY, RATE_FLAG,
												'*', '*',0,to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss')
										  FROM dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND bill_order = :mBillOrder;
#endif
				if ((SQLCODE != 0) || (SQLROWS == 0))
				{
						sprintf(retMsg, "�����wConUserChgʱ����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140208");
						return -1;
				}

				EXEC SQL        UPDATE dCustMsg
										   SET ids = ids + 1
										 WHERE id_no = to_number(:mIdNo);
				if ((SQLCODE != 0) || (SQLROWS == 0))
				{
						sprintf(retMsg, "����dCustMsg.idsʱ����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140208");
						return -1;
				}
		}
*/
		return 0;
}

/**
 *    ��Ա��ϵ����Чʱ��Ϊ������Ч������ͬCreateGrpUser������
 */
int     fCreateGrpUser(FBFR32 *transIN, FBFR32 *transOUT,       char  *temp_buf,
								const char *loginAccept, const char *opCode, const char *loginNo,
								const char *orgCode,    const char *phoneNo, const char *smCode,
								const char *serviceType, const char *belongCode, const char *opTime,
								const char *custId,     const char* productCode, const char *grpIdNo, const char* contractNo,
								const char *groupId, const char* orgId,
								char *retCode,  char *retMsg)
{
		TCreateGrpUser grpUser;

		memset(&grpUser, 0, sizeof(grpUser));
		grpUser.createFlag = CGU_NEW_USER;
		strcpy(grpUser.loginAccept , loginAccept );
		strcpy(grpUser.opCode      , opCode      );
		strcpy(grpUser.loginNo     , loginNo     );
		strcpy(grpUser.orgCode     , orgCode     );
		strcpy(grpUser.phoneNo     , phoneNo     );
		strcpy(grpUser.smCode      , smCode      );
		strcpy(grpUser.serviceType , serviceType );
		strcpy(grpUser.belongCode  , belongCode  );
		strcpy(grpUser.opTime      , opTime      );
		strcpy(grpUser.custId      , custId      );
		strcpy(grpUser.productCode , productCode );
		strcpy(grpUser.grpIdNo     , grpIdNo     );
		strcpy(grpUser.contractNo  , contractNo  );
		strcpy(grpUser.groupId     , groupId     );
		strcpy(grpUser.orgId       , orgId       );
		strcpy(grpUser.effectTime  , opTime      );
		return  CreateGrpUser(transIN,transOUT,temp_buf,&grpUser,retCode,retMsg);
}

/**
 *     ɾ��һ�����û����ϵ��û���Ϣ
 * @author      lugz
 * @created     2004-8-17 10:35
 * @inparam     loginAccept     ��ˮ            char[22+1] �������룬������������ڷ�����ȡ��ˮ
 * @inparam     opCode  ���ܴ���
 * @inparam     loginNo ��������
 * @inparam     orgCode �������Ź���
 * @inparam     idNo    ��������ID
 * @inparam     opTime          ����ʱ��        'YYYYMMDD HH24:MI:SS'
 * @outparam    retCode �������
 * @outparam    retMsg  ������Ϣ
 * @return ������-1����ȷ����0
 *      �������±�
 *              dCustMsg
 *      �������±�
 *              dCustMsgHis
 *              dCustMsgDead
 *              dCustInnetHis
 *              dCustInnetDead
 *              dCustUserHis
 *              dCustUserMsgDead
 *              wUserMsgChg
 *              dConMsgHis
 *              dCustConMsgHis
 *              dCustConMsgDead
 *              dConUserMsgHis
 *              dConUserMsgDead
 *              wConUserChg
 *              dConMsgDead
 *      ɾ�����±�
 *              dCustMsg
 *              dCustInnet
 *              dCustUserMsg
 *              dConMsg
 *              dCustConMsg
 *              dConUserMsg
 *      ���ǵ�������Щ�����õ��������Ҳ�������
 *              dMarkMsg
 *              dCustPost
 *              dCustPostHis
 *              dAssuMsg
 *              dCustStop
 */

int     fDeleteUser(FBFR32 *transIN,    FBFR32 *transOUT,       char  *temp_buf,
								const char *loginAccept, const char *opCode, const char *loginNo,
								const char *orgCode,    const char *idNo,       const char *opTime,
								const char* vgroup_id,const char* vOrg_id,
								char *retCode,  char *retMsg)
{
		EXEC SQL BEGIN DECLARE SECTION;
				int             mCount;
				int             mSerialNo;
				char    mLoginAccept[22+1];
				char    mOpCode[4+1];
				char    mLoginNo[6+1];
				char    mOrgCode[9+1];
				char    mIdNo[22+1];
				char    mOpTime[17+1];
				char    mPhoneNo[15+1];

				char    mRegionCode[2+1];
				char    mConId[22+1];
				char    mCustId[22+1];
				char    mTotalDate[8+1];
				char    mSql[1024];
		EXEC SQL END DECLARE SECTION;
		int ret;

pubLog_Debug(_FFL_, "fDeleteUser", "", "1-100");
		memset(mLoginAccept, 0, sizeof(mLoginAccept));
		memset(mOpCode, 0, sizeof(mOpCode));
		memset(mLoginNo, 0, sizeof(mLoginNo));
		memset(mOrgCode, 0, sizeof(mOrgCode));
		memset(mIdNo, 0, sizeof(mIdNo));
		memset(mOpTime, 0, sizeof(mOpTime));
		memset(mPhoneNo, 0, sizeof(mPhoneNo));

		strcpy(mLoginAccept, loginAccept);
		strcpy(mOpCode, opCode);
		strcpy(mLoginNo, loginNo);
		strcpy(mOrgCode, orgCode);
		strcpy(mIdNo, idNo);
		strcpy(mOpTime, opTime);

		strncpy(mRegionCode, mOrgCode,2);
		mRegionCode[2] = '\0';

		memset(mTotalDate, 0, sizeof(mTotalDate));
		strncpy(mTotalDate, mOpTime, 8);

pubLog_Debug(_FFL_, "fDeleteUser", "", "1-111");

		mCount = 0;
		EXEC SQL        SELECT Phone_No,cust_id
								  INTO :mPhoneNo,:mCustId
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
		if (mPhoneNo[0] == '\0')
		{
				strcpy(retCode,"140101");
				sprintf(retMsg, "�����ڴ˷������[%s]�Ŀ�����¼!", mIdNo);
				return -1;
		}
		Trim(mPhoneNo);
		Trim(mCustId);
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-112");
#if PROVINCE_RUN == PROVINCE_SHANXI
		if ( (ret=cancelAllBill(mIdNo,mPhoneNo, mOpTime, mOpCode, mLoginNo, mLoginAccept, "dSrvMsg", retMsg)) != 0)
		{
				sprintf(retCode,"%d", ret);
				pubLog_Debug(_FFL_, "fDeleteUser", "", "cancelAllBill error[%s]", retMsg);
				return -1;
		}
#else
		/***********************************************************
		if ((ret =  CancelAllMode(mIdNo,  mPhoneNo, mOpTime,mOpCode,
		 mLoginNo,mLoginAccept, retMsg)) != 0)
		 ***********************************************************/
		if ((ret =  CancelAllMode(mIdNo, retMsg)) != 0)
		{
				sprintf(retCode,"%06d",ret);
				return -2;
		}
#endif /*PROVINCE_SHANXI*/
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-113");
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustMsgHis
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,STOP_ATTR,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_NO
								)
								SELECT
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,STOP_ATTR,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'd', :mOpCode,group_no
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-114");
		EXEC SQL        INSERT INTO dCustMsgHis
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								SELECT
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'd', :mOpCode
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140103");
				sprintf(retMsg,"�����dCustMsgHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}

pubLog_Debug(_FFL_, "fDeleteUser", "", "1-115");
		EXEC SQL        UPDATE dCustMsg
								   SET run_code = substr(run_code,2,1) || 'a',
									   run_time = to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS')
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140104");
				sprintf(retMsg,"���±�dCustMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-116");
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustMsgDead
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,STOP_ATTR,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
								)
								SELECT
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,STOP_ATTR,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-117");
		EXEC SQL        INSERT INTO dCustMsgDead
								(
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
								)
								SELECT
										ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
										ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
										RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
										BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140105");
				sprintf(retMsg,"�����dCustMsgDead����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-118");
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dCustInnetHis
								(
										ID_NO,CUST_ID,BUY_NAME,BUY_ID,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								SELECT
										ID_NO,CUST_ID,BUY_NAME,NULL,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode
								  FROM dCustInnet
								 WHERE id_no=:mIdNo;
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-119");
		EXEC SQL        INSERT INTO dCustInnetHis
								(
										ID_NO,CUST_ID,BUY_NAME,BUY_ID,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								SELECT
										ID_NO,CUST_ID,BUY_NAME,NULL,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID,
										to_number(:mLoginAccept), to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),to_number(:mTotalDate),
										:mLoginNo, 'a', :mOpCode
								  FROM dCustInnet
								 WHERE id_no=:mIdNo;
#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140106");
				sprintf(retMsg,"�����dCustInnetHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}

#if PROVINCE_RUN != PROVINCE_JILIN
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1120");
		EXEC SQL        INSERT INTO dCustInnetDead
								(
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID
								)
								SELECT
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID
								  FROM dCustInnet
								 WHERE id_no=:mIdNo;
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1121");
		EXEC SQL        INSERT INTO dCustInnetDead
								(
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID
								)
								SELECT
										ID_NO,CUST_ID,BUY_NAME,BELONG_CODE,TOWN_CODE,INNET_TYPE,
										OPEN_TIME,LOGIN_ACCEPT,LOGIN_NO,MACHINE_CODE,CASH_PAY,CHECK_PAY,
										SIM_FEE,MACHINE_FEE,INNET_FEE,CHOICE_FEE,OTHER_FEE,HAND_FEE,
										DEPOSIT,BACK_FLAG,ENCRYPT_FEE,SYSTEM_NOTE,OP_NOTE,ASSURE_NO,GROUP_ID
								  FROM dCustInnet
								 WHERE id_no=:mIdNo;
#endif
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140107");
				sprintf(retMsg,"�����dCustInnetDead����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}

		EXEC SQL        DELETE FROM dCustInnet
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140108");
				sprintf(retMsg,"ɾ����dCustInnet����[%d][%s]\n", SQLCODE, SQLERRMSG);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1131");
		EXEC SQL        INSERT INTO dCustUserHis
								(
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG,
										UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_ACCEPT,OP_CODE
								)
								SELECT
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG,
										to_date(:mOpTime, 'YYYYMMDD HH24:MI:SS'),:mLoginNo,'a',  to_number(:mLoginAccept),:mOpCode
								  FROM dCustUserMsg
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140109");
				sprintf(retMsg,"�����dCustUserHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
/*
		EXEC SQL        INSERT INTO dCustUserMsgDead
								(
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG
								)
								SELECT
										CUST_ID,ID_NO,REP_FLAG,CUST_FLAG
								  FROM dCustUserMsg
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140110");
				strcpy(retMsg,"�����dCustUserMsgDead����!");
				return -1;
		}
*/
		EXEC SQL        DELETE FROM dCustUserMsg
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140111");
				sprintf(retMsg,"ɾ����dCustUserMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1141");
#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO wUserMsgChg
								(
										OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
										ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
								)
								SELECT
										sMaxBillChg.NEXTVAL, '3', id_no, cust_id,phone_no,sm_code,
										attr_code, belong_code, run_code,to_char(run_time, 'YYYYMMDDHH24MISS'),
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),:vgroup_id
								 FROM dCustMsg
								WHERE id_no=:mIdNo;
#else
		EXEC SQL        INSERT INTO wUserMsgChg
								(
										OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
										ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME,GROUP_ID
								)
								SELECT
										sMaxBillChg.NEXTVAL, '3', id_no, cust_id,phone_no,sm_code,
										attr_code, belong_code, run_code,to_char(run_time, 'YYYYMMDDHH24MISS'),
										SUBSTR(:mOpTime,1,8)||SUBSTR(:mOpTime,10,2)
										||SUBSTR(:mOpTime,13,2)||SUBSTR(:mOpTime,16,2),:vgroup_id
								 FROM dCustMsg
								WHERE id_no=:mIdNo;
#endif
		if (SQLCODE != 0)
		{
				strcpy(retCode,"140112");
				sprintf(retMsg,"�����wUserMsgChg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
#endif
		pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1151");
		memset(mConId, 0, sizeof(mConId));
		EXEC SQL        SELECT to_char(contract_no)
								  INTO :mConId
								  FROM dCustMsg
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140113");
				sprintf(retMsg,"��ȡ��dCustMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
		Trim(mConId);


		EXEC SQL        DELETE FROM dCustMsg
								 WHERE id_no=:mIdNo;
		if(SQLCODE != 0)
		{
				strcpy(retCode,"140114");
				strcpy(retMsg,"ɾ����dCustMsg����!");
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}

		/*
		 *    ɾ��Ĭ���ʺż���dCustMsg�е�contract_no�ֶ�
		 */
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1161");
		EXEC SQL        DELETE FROM dConMsgHis
								 WHERE contract_no = to_number(:mConId)
								   AND update_accept = to_number(:mLoginAccept);

#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dConMsgHis
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code, GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type,
										Update_Code,    Update_Date,  Update_Time,
										Update_Login,   Update_Accept,Update_Type
								)
								SELECT
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type,
										:mOpCode,      TO_NUMBER(:mTotalDate),
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										:mLoginNo,    to_number(:mLoginAccept),'d'
								 FROM dConMsg
								WHERE contract_no = to_number(:mConId);
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1171");
		EXEC SQL        INSERT INTO dConMsgHis
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type,
										Update_Code,    Update_Date,  Update_Time,
										Update_Login,   Update_Accept,Update_Type
								)
								SELECT
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type,
										:mOpCode,      TO_NUMBER(:mTotalDate),
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),
										:mLoginNo,    to_number(:mLoginAccept),'d'
								 FROM dConMsg
								WHERE contract_no = to_number(:mConId);
#endif
		if (SQLCODE != 0) {
				strcpy(retCode, "140115");
				sprintf(retMsg, "����dConMsgHis ����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1181");
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL        INSERT INTO dConMsgDead
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code, GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type
								)
								SELECT
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code, GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type
								 FROM dConMsg
								WHERE contract_no = to_number(:mConId);
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1191");
		EXEC SQL        INSERT INTO dConMsgDead
								(
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type
								)
								SELECT
										Contract_No,    Con_Cust_Id,  Contract_Passwd,
										Bank_Cust,      Oddment,      Belong_Code,
										PrePay_FEE,     Prepay_Time,  Status,
										Status_Time,    Post_Flag ,   Deposit ,
										Min_Ym,         Owe_Fee,      Account_Mark,
										Account_Limit,  Pay_Code,     Bank_Code,GROUP_ID,
										Post_Bank_Code, Account_No,   Account_Type
								 FROM dConMsg
								WHERE contract_no = to_number(:mConId);
#endif
		if (SQLCODE != 0 && SQLCODE != 1403) {
				strcpy(retCode, "140116");
				sprintf(retMsg, "����dConMsgDead ����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1211");
		EXEC SQL        DELETE FROM dConMsg
								 WHERE contract_no = to_number(:mConId);
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode,"140117");
				strcpy(retMsg,"ɾ����dConMsg����!");
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1122");
		EXEC SQL        INSERT INTO dCustConMsgHis
								(
										Cust_Id,         Contract_No,     Begin_Time,
										End_Time,        Update_Accept,   Update_Login,
										Update_Date,     Update_Time,     Update_Code,
										Update_Type
								)
								select
										Cust_Id,         Contract_No,     Begin_Time,End_Time,
										to_number(:mLoginAccept),  :mLoginNo,
										TO_NUMBER(:mTotalDate),   TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),      :mOpCode,      'a'
								  FROM dCustConMsg
								 WHERE cust_id = to_number(:mCustId)
								   AND contract_no = to_number(:mConId);
		if (SQLCODE != 0 && SQLCODE != 1403) {
				strcpy(retCode, "140118");
				sprintf(retMsg, "����dCustConMsgHis ����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
/*
		EXEC SQL        INSERT INTO dCustConMsgDead
								(
										Cust_Id,         Contract_No,     Begin_Time,   End_Time
								)
								SELECT
										Cust_Id,         Contract_No,     Begin_Time,   End_Time
								  FROM dCustConMsg
								 WHERE contract_no = to_number(:mConId);
		if (SQLCODE != 0) {
				strcpy(retCode, "140119");
				sprintf(retMsg, "����dCustConMsgDead����!\n");
				return -1;
		}
*/
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1123");
		EXEC SQL        DELETE FROM dCustConMsg
								 WHERE cust_id = to_number(:mCustId)
								   AND contract_no = to_number(:mConId);
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode,"140120");
				sprintf(retMsg,"ɾ����dCustConMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1124");
		EXEC SQL        SELECT nvl(max(serial_no), 0)
								  INTO :mSerialNo
								  FROM dConUserMsg
								 WHERE contract_no = to_number(:mConId)
								   AND id_no = to_number(:mIdNo);
		if (SQLCODE != 0)
		{
				strcpy(retCode, "140121");
				sprintf(retMsg, "��ѯ��dConUserMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}

		pubLog_Debug(_FFL_, "fDeleteUser", "", "vId_No=[%s]",mIdNo);
		pubLog_Debug(_FFL_, "fDeleteUser", "", "vmConId=[%s]",mConId);
		pubLog_Debug(_FFL_, "fDeleteUser", "", "mTotalDate=[%s]",mTotalDate);
		pubLog_Debug(_FFL_, "fDeleteUser", "", "mOpTime=[%s]",mOpTime);
		pubLog_Debug(_FFL_, "fDeleteUser", "", "mSerialNo=[%d]",mSerialNo);
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1125");
				EXEC SQL        INSERT INTO dConUserMsgHis
										(
												Id_No,        Contract_No,  Serial_No,
												Bill_Order,   Pay_Order,    Begin_YMD,
												Begin_TM,     End_YMD,      End_TM,
												Limit_Pay,    Rate_Flag,    Stop_Flag,
												Update_Accept,Update_Login, Update_Date,
												Update_Time,  Update_Code,  Update_Type
										)
										SELECT
												Id_No,        Contract_No,  Serial_No,
												Bill_Order,   Pay_Order,    Begin_YMD,
												Begin_TM,     End_YMD,      End_TM,
												Limit_Pay,    Rate_Flag,    Stop_Flag,
												to_number(:mLoginAccept),:mLoginNo,   TO_NUMBER(:mTotalDate),
												TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),        :mOpCode,    'a'
										  FROM dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND contract_no = to_number(:mConId)
										   AND serial_no = 0;
				if (SQLCODE != 0 && SQLCODE != 1403) {
						sprintf(retMsg, "����dConUserMsgHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140412");
						PUBLOG_DBDEBUG("fDeleteUser");
						pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
						return -1;
				}
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1126");
		EXEC SQL        UPDATE dConUserMsg
								   SET END_YMD=:mTotalDate,
									   END_TM=to_char(to_date(:mOpTime,'yyyymmdd hh24:mi:ss'), 'hh24miss'),
									   serial_no = :mSerialNo + 1
								 WHERE id_no = to_number(:mIdNo)
								   AND contract_no = to_number(:mConId)
								   AND serial_no = 0;

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode, "140122");
				pubLog_Debug(_FFL_, "fDeleteUser", "", "mIdNo=[%s]",mIdNo);
				pubLog_Debug(_FFL_, "fDeleteUser", "", "mConId=[%s]",mConId);
				pubLog_Debug(_FFL_, "fDeleteUser", "", "mOpTime=[%s]",mOpTime);
				sprintf(retMsg, "���±�dConUserMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
		else if (SQLCODE == 0)
		{
#ifdef _CHGTABLE_
#if PROVINCE_RUN != PROVINCE_JILIN
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1127");
				EXEC SQL        INSERT INTO wConUserChg
										(
												Op_No,       Op_Type,    Id_No,
												Contract_No, Bill_Order, Conuser_Update_Time,
												Limit_Pay,   Rate_Flag,  Fee_Code,     DETAIL_CODE,
												Fee_Rate,   Op_Time, phone_no
										)
										select
												sMaxBillChg.NEXTVAL,     '30',        id_no,
												contract_no, bill_order, BEGIN_TM||END_TM,
												LIMIT_PAY,          RATE_FLAG,      '*', '*',
												0,      to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
												:mPhoneNo
										  from dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND contract_no = to_number(:mConId);
#else
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1128");
				EXEC SQL        INSERT INTO wConUserChg
										(
												Op_No,       Op_Type,    Id_No,
												Contract_No, Bill_Order, Conuser_Update_Time,
												Limit_Pay,   Rate_Flag,  Fee_Code,     DETAIL_CODE,
												Fee_Rate,   Op_Time
										)
										select
												sMaxBillChg.NEXTVAL,     '30',        id_no,
												contract_no, bill_order, to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS'),
												LIMIT_PAY,          RATE_FLAG,      '*', '*',
												0,      to_char(TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),'YYYYMMDDHH24MISS')
										  from dConUserMsg
										 WHERE id_no = to_number(:mIdNo)
										   AND contract_no = to_number(:mConId);
#endif
				if (SQLCODE != 0) {
						strcpy(retCode, "140124");
						sprintf(retMsg, "����wConUserChg ����![%d][%s]\n", SQLCODE, SQLERRMSG);
						PUBLOG_DBDEBUG("fDeleteUser");
						pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
						return -1;
				}
#endif
		}

/*
		EXEC SQL        INSERT INTO dConUserMsgDead
								(
										Id_No,        Contract_No,  Serial_No,
										Bill_Order,   Pay_Order,    Begin_YMD,
										Begin_TM,     End_YMD,      End_TM,
										Limit_Pay,    Rate_Flag,    Stop_Flag,
										Update_Accept,Update_Login, Update_Date,
										Update_Time,  Update_Code,  Update_Type
								)
								SELECT
										Id_No,        Contract_No,  Serial_No,
										Bill_Order,   Pay_Order,    Begin_YMD,
										Begin_TM,     End_YMD,      End_TM,
										Limit_Pay,    Rate_Flag,    Stop_Flag,
										to_number(:mLoginAccept),:mLoginNo,   TO_NUMBER(:mTotalDate),
										TO_DATE(:mOpTime,'YYYYMMDD HH24:MI:SS'),        :mOpCode,    'a'
								  FROM dConUserMsg
								 WHERE id_no = to_number(:mIdNo)
								   AND contract_no = to_number(:mConId);
		if (SQLCODE != 0)
		{
				strcpy(retCode, "140123");
				sprintf(retMsg, "����dConUserMsgDead����!\n");
				return -1;
		}
*/
pubLog_Debug(_FFL_, "fDeleteUser", "", "1-1129");
		EXEC SQL        DELETE FROM dConUserMsg
								 WHERE id_no = to_number(:mIdNo)
								   AND contract_no = to_number(:mConId);
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode,"140125");
				sprintf(retMsg,"ɾ����dConUserMsg����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fDeleteUser");
				pubLog_DBErr(_FFL_,"fDeleteUser",retCode,retMsg);
				return -1;
		}
/*
		EXEC SQL        DELETE FROM dCustMsgAdd
								 WHERE id_no = to_number(:mIdNo);
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode,"140126");
				sprintf(retMsg,"ɾ����dCustMsgAdd����[%d][%s]\n", SQLCODE, SQLERRMSG);
				return -1;
		}
*/
		return 0;
}



/**
 *@name fDeleteGrpUser
 *@description  ����������ɾ��һ�������û����û���Ϣ�����п��ܵ���fDeleteUser����ɾ���û���Ϣ
 *@author       lugz
 *@created      2004-8-17 10:35
 *@input parameter information
 *@inparam      grpUser �ο��ṹ���˵��
 *@output parameter information
 *@outparam     retCode �������
 *@outparam     retMsg  ������Ϣ
 *@return ������-1����ȷ����0
 */
int DeleteGrpUser(FBFR32 *transIN, FBFR32 *transOUT, char *temp_buf, TDeleteGrpUser *grpUser,
  char *retCode,  char *retMsg)
{
		EXEC SQL BEGIN DECLARE SECTION;
				int             mCount;
				char    vGrpCust_Id[22+1]; /*���ſͻ���Cust_Id*/
				char    mSmCode[2+1];
				char    mServiceType[2+1];
				char    mBelongCode[7+1];
				char    mCustId[22+1];

				/*middle*/
				char    mSql[1024];
				char    mTotalDate[8+1];
				int     mSerialNo=0;
				char    mPhoneNo[15+1];
				char    vGrp_Smcode[2+1];
				char    vGrp_Flag[1+1];
				int     vCount=0;
				char  vRun_Time[10+1];       /*add by miaoyl at 20090629 ����ʱ�� */

		EXEC SQL END DECLARE SECTION;

		/*ng���� by yaoxc begin*/
		int  v_ret=0;
		TdGrpUserMebMsgIndex oldIndex;
		TdGrpUserMebMsgIndex newIndex;
		TdGrpUserMebMsgIndex tdGrpUserMebMsgIndex;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char sTempSqlStr[1024];
		char sIdNo[1024];
		char sBeginTime[17+1];
		char sEndTime[17+1];
		int iCount = 0;
		/*ng���� by yaoxc end*/

		/*ng���� by yaoxc begin*/
		memset(&oldIndex,0,sizeof(oldIndex));
		memset(&newIndex,0,sizeof(newIndex));
		memset(&tdGrpUserMebMsgIndex,0,sizeof(tdGrpUserMebMsgIndex));
		init(v_parameter_array);
		init(sTempSqlStr);
		init(sIdNo);
		init(sBeginTime);
		init(sEndTime);
		/*ng���� by yaoxc end*/

		memset(mSmCode, 0, sizeof(mSmCode));
		memset(mServiceType, 0, sizeof(mServiceType));
		memset(vGrpCust_Id, 0, sizeof(vGrpCust_Id));

		memset(mTotalDate, 0, sizeof(mTotalDate));
		memset(vRun_Time, 0, sizeof(vRun_Time)); /*add by miaoyl at 20090629 */
		strncpy(mTotalDate, grpUser->opTime, 8);

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mLoginAccept=[%s]", grpUser->loginAccept );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mOpCode     =[%s]", grpUser->opCode      );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mLoginNo    =[%s]", grpUser->loginNo     );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mOrgCode    =[%s]", grpUser->orgCode     );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mIdNo       =[%s]", grpUser->idNo        );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mOpTime     =[%s]", grpUser->opTime      );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mProductCode=[%s]", grpUser->productCode );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mGrpIdNo    =[%s]", grpUser->grpIdNo     );
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mTotalDate  =[%s]", mTotalDate   );
#endif

		mCount = 0;
		EXEC SQL SELECT COUNT(*) INTO :mCount FROM dCustMsg WHERE id_no = :grpUser->idNo;
		if (mCount == 0)
		{
				strcpy(retCode,"140401");
				sprintf(retMsg, "�����ڴ˷������[%s]�Ŀ�����¼!", grpUser->idNo);
				return -1;
		}

		memset(mPhoneNo, 0, sizeof(mPhoneNo));
		EXEC SQL SELECT phone_no, service_type
								  INTO :mPhoneNo, :mServiceType
								  FROM dCustMsg
								 WHERE id_no=:grpUser->idNo;
		if (mCount == 0)
		{
				strcpy(retCode,"140401");
				sprintf(retMsg, "��ѯ�ͻ�[%s]service_type����\n!", grpUser->idNo);
				return -1;
		}
		Trim(mPhoneNo);

		Sinitn(vGrpCust_Id);
		Sinitn(vGrp_Smcode);
	EXEC SQL SELECT Cust_Id, sm_code
						  INTO :vGrpCust_Id, :vGrp_Smcode
								  FROM dGrpUserMsg
								 WHERE Id_No = To_Number(:grpUser->grpIdNo);
	if (SQLCODE != 0) {
				strcpy(retCode,"140401");
				sprintf(retMsg, "��ѯdGrpUserMsg��[%s]�ͻ�ID����\n!", grpUser->grpIdNo);
				PUBLOG_DBDEBUG("DeleteGrpUser");
				pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
				return -1;
	}
	Trim(vGrpCust_Id);
	Trim(vGrp_Smcode);

		Sinitn(vGrp_Flag);
	/*�Ӽ��ŷ���Ʒ�Ʊ�õ�grp_flag*/
	EXEC SQL SELECT GRP_FLAG
			   INTO :vGrp_Flag
			   FROM sGrpSmCode
			  WHERE SM_CODE = :vGrp_Smcode;
	if(SQLCODE != 0)
	{
		strcpy(retCode,"140402");
		sprintf(retMsg, "�޸ò�ƷƷ��[%s]����!", vGrp_Smcode);
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s-- [%d][%s]",retMsg,SQLCODE,SQLERRMSG);
		PUBLOG_DBDEBUG("DeleteGrpUser");
		pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
		return -1;
	}
	Trim(vGrp_Flag);

		if (vGrp_Flag[0] == 'Y')
		{
			#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "ɾ��dGrpMemberMsg��[%s]����", grpUser->idNo);
			#endif
#if PROVINCE_RUN != PROVINCE_JILIN
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
			EXEC SQL INSERT INTO dGrpMemberMsgHis
					 (unit_id, unit_code, unit_id_no,id_no, big_flag, phone_no, Name, user_name,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  UPDATE_LOGIN, UPDATE_ACCEPT,group_id,
					  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			   SELECT unit_id, unit_code, unit_id_no,id_no, big_flag, phone_no, Name, user_name,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  :grpUser->loginNo, :grpUser->loginAccept,group_id,
					  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd'
				 FROM dGrpMemberMsg
				WHERE ID_NO = To_Number(:grpUser->idNo);
	*/

		EXEC SQL INSERT INTO dGrpMemberMsgHis
			 (unit_id,id_no, big_flag, phone_no, user_name,
			  belong_code, service_no, card_code, member_code, card_no,
			  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
			  contact_phone, fax, email, love, ingrp_date, out_date,
			  stop_flag,
			  service_access,  sm_code,
			  note,
			  cust_id, modify_date,  high_value_flag,
			  group_member_consumption_code, member_cust_id,
			  UPDATE_LOGIN, UPDATE_ACCEPT,group_id,
			  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,
			  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			  correct_note,ORG_ID)
	   SELECT unit_id,id_no, big_flag, phone_no, user_name,
			  belong_code, service_no, card_code, member_code, card_no,
			  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
			  contact_phone, fax, email, love, ingrp_date, out_date,
			  stop_flag,
			  service_access,   sm_code,
			  note,
			  cust_id, modify_date,  high_value_flag,
			  group_member_consumption_code,  member_cust_id,
			  :grpUser->loginNo, :grpUser->loginAccept,group_id,
			  to_number(to_char(sysdate,'yyyymmdd')),sysdate, :grpUser->opCode, 'd',
			  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			  correct_note,ORG_ID
		 FROM dGrpMemberMsg
		WHERE ID_NO = To_Number(:grpUser->idNo);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

#else
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
		/*LINA ADD HERE*/
		/*
			EXEC SQL INSERT INTO dGrpMemberMsgHis
					 (unit_id, unit_code, id_no, big_flag, phone_no, Name, user_name,
					  unit_id_no,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  UPDATE_LOGIN, UPDATE_ACCEPT,group_id,
					  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			   SELECT unit_id, unit_code, id_no, big_flag, phone_no, Name, user_name,
					  unit_id_no,
					  male_flag, belong_code, service_no, card_code, member_code, card_no,
					  contact_addr, owner_zip, Old, birthday, id_type, id_iccid, dept, duty,
					  contact_phone, fax, email, love, ingrp_date, out_date, indb_date,
					  stop_flag, month_fee, important_family, consume_habit_code,
					  service_access, love_redound_code, love_service_mode, sm_code,
					  pay_code, bill_code, pay_fee_flag, payed_fee, short_phoneno, note,
					  cust_id, modify_date, load_date, high_value_flag, group_type_code,
					  gmember_status_code, phone_status_code, group_member_level_code,
					  group_member_consumption_code, deal_indicate, member_cust_id,
					  :grpUser->loginNo, :grpUser->loginAccept,group_id,
					  :mTotalDate,  to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'), :grpUser->opCode, 'd'
				 FROM dGrpMemberMsg
				WHERE ID_NO = To_Number(:grpUser->idNo);
				*/
				/*LINA END HERE*/

		EXEC SQL INSERT INTO dGrpMemberMsgHis
			 (unit_id, id_no, big_flag, phone_no, user_name,
			  belong_code, service_no, card_code, member_code, card_no,
			  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
			  contact_phone, fax, email, love, ingrp_date, out_date,
			  stop_flag,
			  service_access,   sm_code,
			  note,
			  cust_id, modify_date, high_value_flag,
			  group_member_consumption_code,  member_cust_id,
			  UPDATE_LOGIN, UPDATE_ACCEPT,group_id,
			  UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,
			  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			  correct_note,ORG_ID)
	   SELECT unit_id, id_no, big_flag, phone_no, user_name,
			  belong_code, service_no, card_code, member_code, card_no,
			  contact_addr, owner_zip, birthday, id_type, id_iccid, dept, duty,
			  contact_phone, fax, email, love, ingrp_date, out_date,
			  stop_flag,
			  service_access,   sm_code,
			  note,
			  cust_id, modify_date,  high_value_flag,
			  group_member_consumption_code,  member_cust_id,
			  :grpUser->loginNo, :grpUser->loginAccept,group_id,
			  to_number(to_char(sysdate,'yyyymmdd')),sysdate, :grpUser->opCode, 'd',
			  AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			  GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			  OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			  correct_note,ORG_ID
		 FROM dGrpMemberMsg
		WHERE ID_NO = To_Number(:grpUser->idNo);
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
#endif
			if (SQLCODE != 0 && SQLCODE != 1403)
			{
					strcpy(retCode,"140401");
					sprintf(retMsg,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
					pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s", retMsg);
					PUBLOG_DBDEBUG("DeleteGrpUser");
					pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
					return -1;
			}

			EXEC SQL delete FROM dGrpMemberMsg
				 WHERE Cust_Id = To_Number(:vGrpCust_Id)
				 AND  ID_NO = To_Number(:grpUser->idNo);
			if (SQLCODE != 0 && SQLCODE != 1403)
			{
					strcpy(retCode,"140401");
					sprintf(retMsg,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
					pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s", retMsg);
					PUBLOG_DBDEBUG("DeleteGrpUser");
					pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
					return -1;
			}


/* chenxuan boss3 ���ٴ����ѷϳ��ı�dGrpMemberUserMsg
			EXEC SQL    INSERT INTO dGrpMemberUserMsgHis
						(
						  UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD,
								OP_TIME, PRODUCT_CODE,
						  UPDATE_ACCEPT, UPDATE_TIME, UPDATE_DATE, UPDATE_LOGIN,
						  UPDATE_TYPE, UPDATE_CODE
						)
						SELECT UNIT_ID, ID_NO, MEMBER_ID, PHONE_NO, BAK_FIELD,
						OP_TIME, PRODUCT_CODE,to_number(:grpUser->loginAccept),to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS'),
						to_number(:mTotalDate),:grpUser->loginNo,'d',:grpUser->opCode
						from dGrpMemberUserMsg
						WHERE member_id = To_Number(:grpUser->idNo);
			if (SQLCODE != 0 && SQLCODE != 1403)
			{
				  sprintf(retMsg,"ɾ��dGrpMemberUserMsgHis�����Ϸ����쳣[%d]",SQLCODE);
				  pubLog_Debug(_FFL_, "DeleteGrpUser", "", "ɾ��dGrpMemberUserMsgHis�����Ϸ����쳣id_no[%s]",grpUser->idNo);
				   return 140401;

			}

				EXEC SQL delete FROM dGrpMemberUserMsg
				WHERE member_id = To_Number(:grpUser->idNo);
				if (SQLCODE != 0 && SQLCODE != 1403)
				{
						sprintf(retMsg,"ɾ��dGrpMemberUserMsg�����Ϸ����쳣[%d]",SQLCODE);
						pubLog_Debug(_FFL_, "DeleteGrpUser", "", "ɾ��dGrpMemberUserMsgHis�����Ϸ����쳣id_no[%s]",grpUser->idNo);
						return 140401;
				}
*/
		}
	mCount = 0;
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "DeleteGrpUser", "", "��ѯdGrpUserMebMsg��");
	#endif
		EXEC SQL SELECT COUNT(*)
		  INTO :mCount
		  FROM dGrpUserMebMsg
		  WHERE Member_Id = To_Number(:grpUser->idNo)
		  AND End_Time > to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS');
		if (SQLCODE != 0)
		{
				strcpy(retCode,"140401");
				sprintf(retMsg,"��ѯdGrpUserMebMsg��[%s]������[%d]", grpUser->idNo, SQLCODE);
				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s", retMsg);
				PUBLOG_DBDEBUG("DeleteGrpUser");
				pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
				return -1;
		}

		/*�û�ֻ��һ�������û��ĳ�Աʱ��ɾ��*/
	if (mCount < 2)
	{
		/*yangzh add 2005-01-24 �����û��뿪����ʱ����attr_codeΪ���˿ͻ���־*/
		/*ng����
		EXEC SQL UPDATE dcustmsg
					SET attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)
				  WHERE id_no = To_Number(:grpUser->idNo);
		if (SQLCODE != 0){
					strcpy(retCode,"140402");
					sprintf(retMsg,"����dCustMsg��attr_code����![%d]",SQLCODE);
					pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s, mIdNo[%s]", retMsg, grpUser->idNo);
					PUBLOG_DBDEBUG("DeleteGrpUser");
					pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
					return -1;
	   }
	   ng����*/
		/*ng���� by yaoxc begin*/
		init(v_parameter_array);

		strcpy(v_parameter_array[0],grpUser->idNo);

		v_ret=OrderUpdateCustMsg(ORDERIDTYPE_USER,grpUser->idNo,100,
								grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"DeleteGrpUser",
								grpUser->idNo,
								" attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)","",v_parameter_array);
		printf("OrderUpdateCustMsg ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"140402");
			sprintf(retMsg,"����dCustMsg��attr_code����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "DeleteGrpUser", "", "%s, mIdNo[%s]", retMsg, grpUser->idNo);
			PUBLOG_DBDEBUG("DeleteGrpUser");
			pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
			return -1;
		}
		/*ng���� by yaoxc end*/
	}

		EXEC SQL        INSERT INTO dGrpUserMebMsgHis
								(
										ID_NO,MEMBER_ID,MEMBER_NO,BAK_FIELD,BEGIN_TIME,END_TIME,
										UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE
								)
								SELECT
										ID_NO,MEMBER_ID,MEMBER_NO,BAK_FIELD,BEGIN_TIME,END_TIME,
										:grpUser->loginAccept, sysdate, to_number(to_char(sysdate,'yyyymmdd')),
										:grpUser->loginNo, 'd', :grpUser->opCode
								  FROM dGrpUserMebMsg
								 WHERE id_no = to_number(:grpUser->grpIdNo)
								   AND member_id = to_number(:grpUser->idNo)
								   AND End_Time > Last_Day(sysdate) + 1;
		if (SQLCODE != 0)
		{
				strcpy(retCode,"140405");
				sprintf(retMsg, "�����dGrpUserMebMsgHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("DeleteGrpUser");
				pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
				return -1;
		}

		/*�����Ѿ���Ч���û���ϵ��ʱ��Ϊ����һ��*/
		/*ng����
		EXEC SQL        update dGrpUserMebMsg
								   set end_time = to_date(:grpUser->effectTime, 'YYYYMMDD HH24:MI:SS')
								 WHERE Id_No = To_Number(:grpUser->grpIdNo)
								   AND Member_Id = to_number(:grpUser->idNo)
								   AND begin_time <= to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS')
								   AND End_Time > to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS');
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
				strcpy(retCode,"140406");
				sprintf(retMsg, "���±�dGrpUserMebMsg����[%d]!", SQLCODE);
				PUBLOG_DBDEBUG("DeleteGrpUser");
				pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
				return -1;
		}
		ng����*/
		/*ng���� by yaoxc begin*/

		init(sTempSqlStr);
		sprintf(sTempSqlStr,"select to_char(begin_time,'YYYYMMDD HH24:MI:SS'),to_char(end_time,'YYYYMMDD HH24:MI:SS') "
							"from dGrpUserMebMsg where ID_NO= to_number(:v1) "
							"and  MEMBER_ID = to_number(:v2) "
							"and  begin_time <= to_date(:v3, 'YYYYMMDD HH24:MI:SS') "
							"and  END_TIME > To_Date(:v4, 'YYYYMMDD HH24:MI:SS')");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcur06 cursor for sql_str;
		EXEC SQL open ngcur06 using :grpUser->grpIdNo,:grpUser->idNo,:grpUser->opTime,:grpUser->opTime;
		for(;;)
		{
			memset(&oldIndex,0,sizeof(oldIndex));
			memset(&newIndex,0,sizeof(newIndex));
			EXEC SQL FETCH ngcur06 into :sBeginTime,:sEndTime;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				sprintf(retMsg, "��ѯdGrpUserMebMsg����!\n");
				strcpy(retCode, "140205");
				EXEC SQL CLOSE ngcur06;
				return -1;
			}
			if(1403==SQLCODE) break;
			iCount++;
			strcpy(oldIndex.sIdNo  	    , grpUser->grpIdNo);
			strcpy(oldIndex.sMemberId  	, grpUser->idNo);
			strcpy(oldIndex.sBeginTime 	, sBeginTime);
			strcpy(oldIndex.sEndTime   	, sEndTime);
			strcpy(newIndex.sIdNo  	    , grpUser->grpIdNo);
			strcpy(newIndex.sMemberId  	, grpUser->idNo);
			strcpy(newIndex.sBeginTime 	, sBeginTime);
			strcpy(newIndex.sEndTime   	, mTotalDate);
			
			init(v_parameter_array);

			strcpy(v_parameter_array[0],mTotalDate);
			strcpy(v_parameter_array[1],grpUser->grpIdNo);
			strcpy(v_parameter_array[2],grpUser->idNo);
			strcpy(v_parameter_array[3],sBeginTime);
			strcpy(v_parameter_array[4],sEndTime);

		/************************ng ����  ����id_typeһ���޸�**************************************/
			/*v_ret=OrderUpdateGrpUserMebMsg(ORDERIDTYPE_USER,grpUser->grpIdNo,100,
									grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"CreateGrpUser",
									oldIndex,newIndex,
									" END_TIME = to_date(:v5, 'YYYYMMDD HH24:MI:SS') ","",v_parameter_array);*/

			v_ret=OrderUpdateGrpUserMebMsg(ORDERIDTYPE_USER,grpUser->idNo,100,
									grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"DeleteGrpUser",
									oldIndex,newIndex,
									" END_TIME = To_Date(:v5,'YYYYMMDD') ","",v_parameter_array);

		/************************ng ����  ����id_typeһ���޸�**************************************/
			printf("OrderUpdateGrpUserMebMsg ,ret=[%d]\n",v_ret);

			if(0!=v_ret)
			{
				strcpy(retCode,"140406");
				sprintf(retMsg, "���±�dGrpUserMebMsg����[%d]!", SQLCODE);
				PUBLOG_DBDEBUG("DeleteGrpUser");
				pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
				EXEC SQL CLOSE ngcur06;
				return -1;
			}
		}
		EXEC SQL CLOSE ngcur06;
		/*ng���� by yaoxc end*/
		/*ng����
		else if (SQLCODE == NOTFOUND)
		ng����*/
		printf("*********iCount=[%d]******\n",iCount);
		if (iCount==0)
		{
				/*ɾ��δ��Ч���û���ϵ*/
				/*ng����
				EXEC SQL delete dGrpUserMebMsg
				  WHERE Id_No = To_Number(:grpUser->grpIdNo)
				  AND Member_Id = to_number(:grpUser->idNo)
				  AND begin_time >= to_date(:grpUser->opTime, 'YYYYMMDD HH24:MI:SS');
				if (SQLCODE != 0 && SQLCODE != 1403)
				{
						strcpy(retCode,"140406");
						sprintf(retMsg, "ɾ����dGrpUserMebMsg����[%d]!", SQLCODE);
						PUBLOG_DBDEBUG("DeleteGrpUser");
						pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
						return -1;
				}
				ng����*/
				/*ng���� by yaoxc begin*/
				init(sTempSqlStr);
				sprintf(sTempSqlStr,"select to_char(begin_time,'YYYYMMDD HH24:MI:SS'),to_char(end_time,'YYYYMMDD HH24:MI:SS') "
									"from dGrpUserMebMsg where ID_NO= to_number(:v1) "
									"and  Member_Id = to_number(:v2) "
									"and  begin_time >= to_date(:v3, 'YYYYMMDD HH24:MI:SS')");
				EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
				EXEC SQL declare ngcur03 cursor for sql_str;
				EXEC SQL open ngcur03 using :grpUser->grpIdNo,:grpUser->idNo,:grpUser->opTime;
				for(;;)
				{
					memset(&tdGrpUserMebMsgIndex,0,sizeof(tdGrpUserMebMsgIndex));
					EXEC SQL FETCH ngcur03 into :sBeginTime,:sEndTime;
					printf("*********SQLCODE=[%d]******\n",SQLCODE);
					if((0!=SQLCODE)&&(1403!=SQLCODE))
					{
						sprintf(retMsg, "��ѯdGrpUserMebMsg����!\n");
						strcpy(retCode, "140205");
						EXEC SQL CLOSE ngcur03;
						return -1;
					}
					if(1403==SQLCODE) break;

					strcpy(tdGrpUserMebMsgIndex.sIdNo  	    , grpUser->grpIdNo);
					strcpy(tdGrpUserMebMsgIndex.sMemberId  	, grpUser->idNo);
					strcpy(tdGrpUserMebMsgIndex.sBeginTime 	, sBeginTime);
					strcpy(tdGrpUserMebMsgIndex.sEndTime 	, sEndTime);

					init(v_parameter_array);

					strcpy(v_parameter_array[0],grpUser->grpIdNo);
					strcpy(v_parameter_array[1],grpUser->idNo);
					strcpy(v_parameter_array[2],sBeginTime);
					strcpy(v_parameter_array[3],sEndTime);

					v_ret=OrderDeleteGrpUserMebMsg(ORDERIDTYPE_USER,grpUser->grpIdNo,100,
											grpUser->opCode,atol(grpUser->loginAccept),grpUser->loginNo,"DeleteGrpUser",
											tdGrpUserMebMsgIndex,"",v_parameter_array);
					printf("OrderDeleteGrpUserMebMsg ,ret=[%d]\n",v_ret);
					if(0!=v_ret)
					{
						strcpy(retCode,"140406");
						sprintf(retMsg, "ɾ����dGrpUserMebMsg����[%d]!", SQLCODE);
						PUBLOG_DBDEBUG("DeleteGrpUser");
						pubLog_DBErr(_FFL_,"DeleteGrpUser",retCode,retMsg);
						EXEC SQL CLOSE ngcur03;
						return -1;
					}
				}
				EXEC SQL CLOSE ngcur03;
				/*ng���� by yaoxc end*/
		}

		/*if (grpUser->contractNo[0] != '\0')
		{

			vCount=0;
			EXEC SQL SELECT  count(*)
				into :vCount
				FROM dConUserMsg
				WHERE id_no = to_number(:grpUser->idNo)
				AND contract_no = :grpUser->contractNo
				AND serial_no = 0;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"140466");
						sprintf(retMsg, "select dConUserMsg����[%d]!", SQLCODE);
						return -1;
			}
			if(vCount>0)
			{
				EXEC SQL        INSERT INTO dConUserMsgHis
										(
												Id_No,        Contract_No,  Serial_No,
												Bill_Order,   Pay_Order,    Begin_YMD,
												Begin_TM,     End_YMD,      End_TM,
												Limit_Pay,    Rate_Flag,    Stop_Flag,
												Update_Accept,Update_Login, Update_Date,
												Update_Time,  Update_Code,  Update_Type
										)
										SELECT
												Id_No,        Contract_No,  Serial_No,
												Bill_Order,   Pay_Order,    Begin_YMD,
												Begin_TM,     End_YMD,      End_TM,
												Limit_Pay,    Rate_Flag,    Stop_Flag,
												to_number(:grpUser->loginAccept),:grpUser->loginNo,   TO_NUMBER(:mTotalDate),
												TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'),        :grpUser->opCode,    'a'
										  FROM dConUserMsg
										 WHERE id_no = to_number(:grpUser->idNo)
										   AND contract_no = :grpUser->contractNo
										   AND serial_no = 0;
				if (SQLCODE != 0) {
						sprintf(retMsg, "����dConUserMsgHis����[%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140412");
						return -1;
				}

#if PROVINCE_RUN != PROVINCE_JILIN
				EXEC SQL        INSERT INTO wConUserChg
										(
												OP_NO,  OP_TYPE,        ID_NO,  CONTRACT_NO,    BILL_ORDER,
												CONUSER_UPDATE_TIME,    LIMIT_PAY,      RATE_FLAG,
												FEE_CODE,       DETAIL_CODE, FEE_RATE,  OP_TIME,phone_no
										)
										SELECT
									 4           sMaxBillChg.nextVal,    '30',   ID_NO,  to_number(:grpUser->contractNo),BILL_ORDER,
												to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'), LIMIT_PAY, RATE_FLAG,
												'*', '*', 0, to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'),
												:mPhoneNo
										  FROM dConUserMsg
										 WHERE id_no = to_number(:grpUser->idNo)
										   AND contract_no = :grpUser->contractNo
										   and serial_no = 0;
#else
				EXEC SQL        INSERT INTO wConUserChg
										(
												OP_NO,  OP_TYPE,        ID_NO,  CONTRACT_NO,    BILL_ORDER,
												CONUSER_UPDATE_TIME,    LIMIT_PAY,      RATE_FLAG,
												FEE_CODE,       DETAIL_CODE, FEE_RATE,  OP_TIME
										)
										SELECT
												sMaxBillChg.nextVal,    '30',   ID_NO,  to_number(:grpUser->contractNo),BILL_ORDER,
												to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss'), LIMIT_PAY, RATE_FLAG,
												'*', '*', 0, to_char(to_date(:grpUser->opTime,'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss')
										  FROM dConUserMsg
										 WHERE id_no = to_number(:grpUser->idNo)
										   AND contract_no = :grpUser->contractNo
										   and serial_no = 0;
#endif
				if (SQLCODE != 0)
				{
						sprintf(retMsg, "�����wConUserChgʱ����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140413");
						return -1;
				}

				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "1aaaaaaaaaaa");
				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mIdNo=[%s], mContractNo=[%s]", grpUser->idNo, grpUser->contractNo);
				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "1ccccccccccccc");
				mSerialNo = 0;
				EXEC SQL        SELECT nvl(max(serial_no), 0)
										  INTO :mSerialNo
										  FROM dConUserMsg
										 WHERE id_no = to_number(:grpUser->idNo);
				mSerialNo += 1;
				pubLog_Debug(_FFL_, "DeleteGrpUser", "", "1AAAAAAAAAAAA");
				EXEC SQL        UPDATE dConUserMsg
										   SET
												END_YMD = :mTotalDate,
												END_TM = to_char(TO_DATE(:grpUser->opTime,'YYYYMMDD HH24:MI:SS'), 'HH24MISS'),
												serial_no = :mSerialNo
										 WHERE id_no = to_number(:grpUser->idNo)
										   AND contract_no = :grpUser->contractNo
										   and serial_no = 0;
				if (SQLCODE != 0)
				{
						pubLog_Debug(_FFL_, "DeleteGrpUser", "", "mIdNo=[%s], mContractNo=[%s]",grpUser->idNo, grpUser->contractNo);
						sprintf(retMsg, "�޸�dConUserMsg����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140414");
						return -1;
				}

pubLog_Debug(_FFL_, "DeleteGrpUser", "", "1BBBBBBBBBBBBBBBBBBB");
				EXEC SQL        UPDATE dCustMsg
										   SET ids = ids - 1
										 WHERE id_no = to_number(:grpUser->idNo)
										   AND ids >= 2;
				if ((SQLCODE != 0) && (SQLCODE != 1403))
				{
						sprintf(retMsg, "����dCustMsg.idsʱ����![%d][%s]\n", SQLCODE, SQLERRMSG);
						strcpy(retCode, "140208");
						return -1;
				}
			}
		}*/
		/*
		 *    ��ѯ���û��Ƿ���ʹ��������Ʒ���������ʹ�ý�ɾ�����û�����Ϣ��
		 */
		 pubLog_Debug(_FFL_, "DeleteGrpUser", "", "CCCCCCCCCCCCCCCCC");
		/*mCount = 0;
		EXEC SQL        SELECT COUNT(*)
								  INTO :mCount
								  FROM dGrpUserMebMsg
								 WHERE member_id = to_number(:grpUser->idNo)
								   AND End_Time > Last_Day(sysdate) + 1;
	if (mCount == 0)
		{
			if (strncmp(mServiceType, "01", 2) != 0)
			{*/
				/*��ͨ���������û�����ɾ��*/
				/*printf("DDDDDDDDDDDDDDDDD\n");
					if (fDeleteUser(transIN, transOUT, temp_buf, grpUser->loginAccept, grpUser->opCode,
						grpUser->loginNo,grpUser->orgCode, grpUser->idNo,  grpUser->opTime,
						grpUser->groupId,grpUser->orgId,retCode, retMsg) != 0)
					{
							return -1;
					}
					pubLog_Debug(_FFL_, "DeleteGrpUser", "", "EEEEEEEEEEEEEEEEEEE");
			}
		}*/
		return 0;
}

/**
 *    ɾ����Ա��ϵ����Чʱ��Ϊ������Ч������ͬDeleteGrpUser������
 */
int     fDeleteGrpUser(FBFR32 *transIN, FBFR32 *transOUT,       char  *temp_buf,
								const char *loginAccept, const char *opCode, const char *loginNo,
								const char *orgCode,    const char *idNo, const char *opTime,
								const char *grpIdNo, const char* productCode, const char* contractNo,
								const char *groupId,const char* orgId,
								char *retCode,  char *retMsg)
{
		TDeleteGrpUser grpUser;

		memset(&grpUser, 0, sizeof(grpUser));
		strcpy(grpUser.loginAccept , loginAccept );
		strcpy(grpUser.opCode      , opCode      );
		strcpy(grpUser.loginNo     , loginNo     );
		strcpy(grpUser.orgCode     , orgCode     );
		strcpy(grpUser.idNo        , idNo        );
		strcpy(grpUser.opTime      , opTime      );
		strcpy(grpUser.grpIdNo     , grpIdNo     );
		strcpy(grpUser.productCode , productCode );
		strcpy(grpUser.contractNo  , contractNo  );
		strcpy(grpUser.groupId     , groupId     );
		strcpy(grpUser.orgId       , orgId       );
		strcpy(grpUser.effectTime  , opTime      );
		return  DeleteGrpUser(transIN,transOUT,temp_buf,&grpUser,retCode,retMsg);
}

int createGrpSendCode(const char *iGrpIdNo, char *oGrpSendCode)
{
	int i = 0;
		long n = atol(iGrpIdNo);
		long n1 = 64;
		char value[8+1];
		long  tmpn = 0;
		int   x=0;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createGrpSendCode", "", "%s: input: [%s] ", __FUNCTION__, iGrpIdNo);
#endif

		memset(value, 0, sizeof(value));

		while(1){
				tmpn = n % 64;
				n = n / 64;
				pubLog_Debug(_FFL_, "createGrpSendCode", "", "n = [%ld] [%ld]", n, tmpn);
				if( n < 64 ){
						tmpn = n;
						if(tmpn < 26){
								tmpn = tmpn + 65;
						}
						else if (tmpn < 36){

								tmpn = tmpn - 26 + 48;
						}else{
								tmpn = tmpn - 36 + 97;
						}

						x = tmpn;
						sprintf(value, "%s%ld", value, tmpn);
						pubLog_Debug(_FFL_, "createGrpSendCode", "", "break: value = [%s]", value);
						break;
				}

				if(tmpn < 26){
						tmpn = tmpn + 65;
				}
				else if (tmpn < 36){
						tmpn = tmpn - 26 + 48;
				}else{
						tmpn = tmpn - 36 + 97;
				}

				x = tmpn;
				sprintf(value, "%s%c", value, x);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "createGrpSendCode", "", "value =[%d] [%c] [%s]", x, x, value);
#endif

		}

	sprintf(oGrpSendCode, "00%s", value);

	return 0;
}


/**********
 *@name			spublicGrpRight
 *@description  ���Ų���Ȩ����֤
 *@author       zwm
 *@created      2008-7-10
**********/

int spublicGrpRight(char *connect_name,char *login_no,char *op_code,char *grp_cust_id,char *phone_no,char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char	phoneNo[15+1];
	char	custId[14+1];
	char	logRegion[2+1];
	char	grpRegion[2+1];
	char	phoRegion[2+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	areaType[2+1];
	char	right_code[4+1];
	char	return_code[6+1];
	char	logPasswd[16+1];

	EXEC SQL END DECLARE SECTION;

	init(phoneNo);
	init(custId);
	init(logRegion);
	init(grpRegion);
	init(phoRegion);
	init(opCode);
	init(loginNo);
	init(areaType);
	init(right_code);
	init(return_code);
	init(logPasswd);

	strcpy(phoneNo,phone_no);
	strcpy(custId,grp_cust_id);
	strcpy(opCode,op_code);
	strcpy(loginNo,login_no);

	pubLog_Debug(_FFL_, "spublicGrpRight", "", "spublicGrpRight doing");

	sprintf(grpRegion,"0%c",custId[0]);
	pubLog_Debug(_FFL_, "spublicGrpRight", "", "spublicGrpRight phoneNo=[%s]",phoneNo);
	EXEC SQL SELECT substr(org_code,1,2),password INTO :logRegion,:logPasswd FROM dloginmsg
			 WHERE login_no=:loginNo;
	if(SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "spublicGrpRight", "", "slect dloginmsg error[%d]",SQLCODE);
		strcpy(retMsg,"��ѯ������Ϣ����!");
		return -1;
	}

	if(strcmp(logRegion,"00") == 0)
	{
		return 0;
	}

	EXEC SQL SELECT substr(belong_code,1,2) INTO :phoRegion FROM dcustmsg
			 WHERE phone_no=:phoneNo;
	if(SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "spublicGrpRight", "", "slect dcustmsg error[%d]",SQLCODE);
		strcpy(retMsg,"��ѯ������Ϣ����!");
		PUBLOG_DBDEBUG("spublicGrpRight");
		pubLog_DBErr(_FFL_,"spublicGrpRight","15518",retMsg);
		return -2;
	}

	if((strcmp(logRegion,grpRegion) == 0) && (strcmp(logRegion,phoRegion) == 0))
	{
		return 0;
	}
	EXEC SQL SELECT area_type INTO :areaType FROM dbvipadm.dgrpcustmsg WHERE cust_id=to_number(:custId);
	if(SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "spublicGrpRight", "", "slect dbvipadm.dgrpcustmsg error[%d]",SQLCODE);
		strcpy(retMsg,"��ѯdgrpcustmsg��Ϣ����!");
		PUBLOG_DBDEBUG("spublicGrpRight");
		pubLog_DBErr(_FFL_,"spublicGrpRight","15519",retMsg);
		return -3;
	}

	if(strcmp(areaType,"06") != 0)
	{
		pubLog_Debug(_FFL_, "spublicGrpRight", "", "���ǿ������ţ����������ҵ��!");
		strcpy(retMsg,"���ǿ������ţ����������ҵ��!");

		return -4;
	}

	if((strcmp(logRegion,grpRegion) != 0) && (strcmp(logRegion,phoRegion) != 0))
	{
		pubLog_Debug(_FFL_, "spublicGrpRight", "", "���Ų���ͬʱ������ؼ��ź���غ���!");
		strcpy(retMsg,"���Ų���ͬʱ������ؼ��ź���غ���!");
		return -5;
	}

	if((strcmp(logRegion,grpRegion) != 0) && (strcmp(logRegion,phoRegion) == 0))
	{
		/*��֤�����Ƿ��п����������ŵ�Ȩ��*/
		strcpy(right_code,"a885");
		sprintf(return_code, "%06d", spublicRight(connect_name,loginNo,logPasswd,right_code));
		if(strcmp(return_code, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "spublicGrpRight", "", "��Ȩ������������");
			strcpy(retMsg,"��Ȩ������������!");
			return -6;
		}
	}
	else if((strcmp(logRegion,grpRegion) == 0) && (strcmp(logRegion,phoRegion) != 0))
	{
		/*��֤�����Ƿ��п������������Ȩ��*/
		strcpy(right_code,"a886");
		sprintf(return_code, "%06d", spublicRight(connect_name,loginNo,logPasswd,right_code));
		if(strcmp(return_code, "000000") != 0)
		{
			pubLog_Debug(_FFL_, "spublicGrpRight", "", "��Ȩ������������");
			strcpy(retMsg,"��Ȩ������������!");
			return -7;
		}
	}

	return 0;
}
