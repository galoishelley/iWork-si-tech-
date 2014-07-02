/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
/***************************************
 *function  ���Ŵ����ú���
 *author    yangzh
 *created   2004-11-08 15:43
****************************************/

/*******�������֯�ṹ���ڸ��� edit by  zhengxg 07/05/2009 <sXXXXCfm(opCode1,opCode3....)> ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "publicsrv.h"

/*����ҵ�����ȡ����sSmProduct���sm_code*/
#define VPMN_SM_CODE           "01" /*VPMN��Ʒҵ���Ʒ�ƴ���*/
#define BOSSFAV_SM_CODE        "23" /*BOSS�Ż�ר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_IP_SM_CODE      "04" /*IPר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_NET_SM_CODE     "11" /*������ר��ҵ���Ʒ�ƴ���*/
#define ZHUANX_DIANLU_SM_CODE  "13" /*��·����ҵ���Ʒ�ƴ���*/

EXEC SQL INCLUDE SQLCA;

/************************************************************************
��������:fpubCreateGrpUser1
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
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubCreateGrpUser1(tGrpUserMsg *grpUserMsg, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note)
{
	tAccDoc   accDoc ;
	tUserDoc  userDoc;
	EXEC SQL BEGIN DECLARE SECTION;
		char	vSrvTypeTmp[1+1];
		char	vPriceCodeTmp[4+1];
		char	vServiceCode[4+1];		/*�����ײͱ���*/
		char	vDetailCode[4+1];		/*�����Żݴ���*/
		char	vGrp_UserNo[15+1];
		char	strLoginAccept[14+1];
		int		retCode = 0;
		char	vCust_Id[14+1];         /* ���ſͻ�ID   */
		char	vGrp_Id[14+1];          /* �����û�ID   */
		char	vLoginAccept[14+1];
		char	vOrgId[10+1];
		char	vGrp_No[15+1];          /* �û����     */
		char	vIds[4+1];              /* �ʻ���       */
		char	vGrp_Name[60+1];        /* �û�����     */
		char	vProduct_Type[8+1];     /* ��Ʒ����     */
		char	vProduct_Code[8+1];     /* ��Ʒ����     */
		char	vOperation_Code[2+1];   /* GRP����ҵ������*/
		char	vGrpProduct_Code[2+1];  /* GRP���ŵĲ�Ʒ����*/
		char	vUser_Passwd[8+1];      /* ҵ���������� */
		char	vLogin_Name[20+1];      /* �Է����û��� */
		char	vLogin_Passwd[20+1];    /* �Է�������   */
		char	vProvince[2+1];         /* ����ʡ       */
		char	vRegion_Code[2+1];      /* ��������     */
		char	vDistrict_Code[2+1];    /* ������       */
		char	vTown_Code[3+1];        /* ����Ӫҵ��   */
		char	vTerritory_Code[20+1];  /* ���ش���     */
		char	vLimit_Owe[18+1];       /* ͸֧���     */
		char	vCredit_Code[2+1];      /* ���õȼ�     */
		char	vCredit_Value[18+1];    /* ���ö�       */
		char	vRun_Code[1+1];         /* �û�״̬     */
		char	vOld_Run[1+1];          /* ǰһ״̬     */
		char	vRun_Time[20+1];        /* ״̬�仯ʱ�� */
		char	vBill_Date[20+1];       /* ����ʱ��     */
		char	vBill_Type[4+1];        /* �������ڴ��� */
		char	vLast_Bill_Type[1+1];   /* �ϴγ������� */
		char	vOp_Time[20+1];         /* ����ʱ��     */
		char	vSrv_Start[10+1];       /* ҵ����ʼ���� */
		char	vSrv_Stop[10+1];        /* ҵ��������� */
		char	vBak_Field[16+1];       /* Ԥ���ֶ�     */
		char	vSm_Code[2+1];          /* ��ƷƷ��     */
		char	vGroup_Id[10+1];
		char	serviceType[2+1];
		int		i, inx, vCount, ret= 0;
		int		vPower_right = 0;
		int		vFav_Order = 0;
		long	lLoginAccept;
		char	vOrg_Code[7+1];
		char	vLogin_No[6+1];
		char	vOp_Code[4+1];
		char	vOp_Note[60+1];
		char	vTotal_Date[8+1];      /* ��������     */
		char	vDateChar[6+1];        /* ��������     */
		char	vUnit_Id[22+1];        /* ����ID       */
		char	vUnit_Code[22+1];      /* ���Ŵ���     */
		char	vAccount_Id[14+1];     /* �ʻ�ID       */
		char	vAccount_Passwd[8+1];  /* �ʻ�����     */
		char	vContract_Id[22+1];    /* ���ź�ͬID   */
		char	vContract_No[22+1];    /* ���ź�ͬ���� */
		char	vProduct_Id[22+1];     /* ��ƷID       */
		char	vOrder_GoodsId[22+1];  /* ����ID       */
		char	vProduct_List[1536];   /* ��Ʒ�����б� */
		char	vProduct_All[1536];    /* ���Ӳ�Ʒ�б� */
		char	sqlStr[3072];
		char	vService_Type[1+1];
		char	vService_Code[4+1];
		char	vMain_Flag[1+1];
		char	vPrice_Code[4+1];
		char	vVPMN_Flag[1+1];
		char	vDisCode1[7+1];
		char	vDisCode2[7+1];
		char	vStatusTime[14+1];
		char	vPay_Code[1+1];         /* �����ʻ����ѷ�ʽ */
		char	vReturnMsg[256+1];
		/*NG��ź*/
		TdConUserMsg	tdConUserMsg;
		char	TotalDate1[20+1];
		char	SrvStop1[20+1];
		char	BeginTime1[20+1];
		char	modeMen[8+1];
		char	sendFalg[1+1];
		TdCustConMsg	sTdCustConMsg;
		int	v_ret=0;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		TdGrpUserMsg	sTdGrpUserMsg;
		TdAccountIdInfo	sTdAccountIdInfo;
		/*ng���� by zhaohx at 02/10/2009 begin*/
		char	vCallServiceName[20];
		int	vSendParaNum=0;
		int	vRecpParaNum=0;
		char	vParamList[MAX_PARMS_NUM][2][DLINTERFACEDATA];
		char	vIdType[1+1];
		int		iOrderRight=0;
		char	count[5+1];
		char	chargeflag_str[256+1];
		char	feecode_str[256+1];
		char	detailcode_str[256+1];
		char	actionfee_str[256+1];
		char	vOpNote[50];
		char    sBeginTm[6+1];
		char	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
			 /*ng���� by zhaohx at 02/09/2009 end*/
		TdBaseFav tdBaseFav;
	EXEC SQL END DECLARE SECTION;

		memset(&accDoc, 0, sizeof(accDoc));
		memset(&userDoc, 0, sizeof(userDoc));
		init(strLoginAccept   );
		init(vOrg_Code        );
		init(vReturnMsg       );
		init(vStatusTime      );
		init(vLogin_No        );
		init(vOp_Code         );
		init(vOp_Note         );
		init(vTotal_Date      );
		init(vDateChar        );
		init(vUnit_Id         );
		init(vUnit_Code       );
		init(vOrgId           );
		init(vContract_Id     );
		init(vContract_No     );
		init(vProduct_Id      );
		init(vOrder_GoodsId   );
		init(vLoginAccept	  );
		init(vService_Type    );
		init(vService_Code    );
		init(vMain_Flag       );
		init(vPrice_Code      );
		init(vSm_Code         );
		init(serviceType      );
		init(vGroup_Id        );
		init(vCust_Id         );
		init(vGrp_Id          );
		init(vAccount_Id      );
		init(vAccount_Passwd  );
		init(vGrp_No          );
		init(vIds             );
		init(vGrp_Name        );
		init(vProduct_Type    );
		init(vProduct_Code    );
		init(vUser_Passwd     );
		init(vLogin_Name      );
		init(vLogin_Passwd    );
		init(vProvince        );
		init(vRegion_Code     );
		init(vDistrict_Code   );
		init(vTown_Code       );
		init(vTerritory_Code  );
		init(vLimit_Owe       );
		init(vCredit_Code     );
		init(vCredit_Value    );
		init(vRun_Code        );
		init(vOld_Run         );
		init(vRun_Time        );
		init(vBill_Date       );
		init(vBill_Type       );
		init(vLast_Bill_Type  );
		init(vOp_Time         );
		init(vSrv_Start       );
		init(vSrv_Stop        );
		init(vBak_Field       );
		init(vVPMN_Flag       );
		init(vOperation_Code  );
		init(vGrpProduct_Code );
		init(vProduct_List    );
		init(vProduct_All     );
		init(sqlStr           );
		init(vDisCode1        );
		init(vDisCode2        );
		init(vPay_Code        );
		init(vGrp_UserNo      );
		init(modeMen);
		init(sendFalg);
		init(sBeginTm);
		init(vSrvTypeTmp);
		init(vPriceCodeTmp);
		init(vDetailCode);
		init(vServiceCode);
		memset(&tdBaseFav,0,sizeof(tdBaseFav));

		lLoginAccept = lSysAccept;
		strcpy(vLogin_No, login_no);
		strcpy(vOp_Code,  op_code);
		strcpy(vOp_Note,  op_note);
		strcpy(vTotal_Date, total_date);
		strncpy(vDateChar,  total_date, 6);
		strcpy(vOrg_Code,       grpUserMsg->org_code);
		strcpy(vCust_Id,        grpUserMsg->cust_id);
		strcpy(vGrp_Id,         grpUserMsg->id_no);
		strcpy(vGrp_No,         grpUserMsg->user_no);
		strcpy(vIds,            grpUserMsg->ids);
		strcpy(vGrp_Name,       grpUserMsg->user_name);
		strcpy(vAccount_Id,     grpUserMsg->account_no);
		strcpy(vAccount_Passwd, grpUserMsg->account_passwd);
		/*��sSmCode����,GRP���ŵĲ�Ʒ��������Ϊ�˲�ƷƷ��*/
		strcpy(vGrpProduct_Code,grpUserMsg->product_type);
		strcpy(vProduct_Type,   grpUserMsg->product_type);
		strcpy(vProduct_Code,   grpUserMsg->product_code);
		strcpy(vUser_Passwd,    grpUserMsg->user_passwd);
		strcpy(vLogin_Name,     grpUserMsg->login_name);
		strcpy(vLogin_Passwd,   grpUserMsg->login_passwd);
		strcpy(vProvince,       grpUserMsg->prov_code);
		strcpy(vRegion_Code,    grpUserMsg->region_code);
		strcpy(vDistrict_Code,  grpUserMsg->district_code);
		strcpy(vTown_Code,      grpUserMsg->town_code);
		strcpy(vTerritory_Code, grpUserMsg->territory_code);
		strcpy(vLimit_Owe,      grpUserMsg->limit_owe);
		strcpy(vCredit_Code,    grpUserMsg->credit_code);
		strcpy(vCredit_Value,   grpUserMsg->credit_value);
		strcpy(vRun_Code,       grpUserMsg->run_code);
		strcpy(vOld_Run,        grpUserMsg->old_run);
		strcpy(vRun_Time,       grpUserMsg->run_time);
		strcpy(vBill_Date,      grpUserMsg->bill_date);
		strcpy(vBill_Type,      grpUserMsg->bill_type);
		strcpy(vLast_Bill_Type, grpUserMsg->last_bill_type);
		strcpy(vOp_Time,        grpUserMsg->op_time);
		strcpy(vSrv_Start,      grpUserMsg->srv_start);
		strcpy(vSrv_Stop,       grpUserMsg->srv_stop);
		strcpy(vBak_Field,      grpUserMsg->bak_field);
		strcpy(vPay_Code,       grpUserMsg->pay_code);
		strcpy(vSm_Code,        grpUserMsg->sm_code);
		strcpy(vGroup_Id,       grpUserMsg->group_id);
		strcpy(vServiceCode,    grpUserMsg->service_code);
		sprintf(strLoginAccept, "%ld", lLoginAccept);
		strcpy(serviceType, "60");
		Trim(vOrg_Code);
		Trim(vProduct_Type);
		Trim(vProduct_Code);
		Trim(vGroup_Id);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "lLoginAccept[%ld]",    lLoginAccept   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLogin_No[%s]",       vLogin_No      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Code[%s]",        vOp_Code       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Note[%s]",        vOp_Note       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTotal_Date[%s]",     vTotal_Date    );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vDateChar[%s]",       vDateChar      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOrg_Code[%s]",       vOrg_Code      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCust_Id[%s]",        vCust_Id       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Id[%s]",         vGrp_Id        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_No[%s]",         vGrp_No        );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vIds[%s]",            vIds           );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGrp_Name[%s]",       vGrp_Name      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Type[%s]",   vProduct_Type  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Code[%s]",   vProduct_Code  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vUser_Passwd[%s]",    vUser_Passwd   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLogin_Name[%s]",     vLogin_Name    );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLogin_Passwd[%s]",   vLogin_Passwd  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProvince[%s]",       vProvince      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vRegion_Code[%s]",    vRegion_Code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vDistrict_Code[%s]",  vDistrict_Code );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTown_Code[%s]",      vTown_Code     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vTerritory_Code[%s]", vTerritory_Code);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLimit_Owe[%s]",      vLimit_Owe     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Code[%s]",    vCredit_Code   );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vCredit_Value[%s]",   vCredit_Value  );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vRun_Code[%s]",       vRun_Code      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOld_Run[%s]",        vOld_Run       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vRun_Time[%s]",       vRun_Time      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBill_Date[%s]",      vBill_Date     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBill_Type[%s]",      vBill_Type     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vLast_Bill_Type[%s]", vLast_Bill_Type);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vOp_Time[%s]",        vOp_Time       );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSrv_Start[%s]",      vSrv_Start     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSrv_Stop[%s]",       vSrv_Stop      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vBak_Field[%s]",      vBak_Field     );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "pay_code[%s]",        vPay_Code      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vSm_Code[%s]",        vSm_Code      );
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vGroup_Id[%s]",       vGroup_Id      );
#endif

	if (strlen(vPay_Code) == 0)
	{
		sprintf(grpUserMsg->return_message, "�ʻ����ʽ����Ϊ��!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s",__FILE__,grpUserMsg->return_message);
		return 190001;
	}

	/*��ȡ����Ա����Ȩ��*/
	EXEC SQL SELECT power_right INTO :vPower_right FROM dLoginMsg
		WHERE Login_No = :vLogin_No;
	if (SQLCODE != OK)
	{
		sprintf(grpUserMsg->return_message, "��ȡ����ԱȨ�޳���![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190001;
	}

	/*���ݲ���ԱȨ��,��������ͨ���ַ���*/
	/*����ԱȨ�ޣ�0��1��ʡ����Ȩ��;2��3������Ȩ��;4��5��6������Ȩ��;7������Ȩ��;
	              8��Ӫҵ�೤Ȩ��; 9��ӪҵԱȨ�� */
	if(vPower_right < 2)
	{
		strcpy(vDisCode1, "99");
		strcpy(vDisCode2, "");
	}
	else if ((vPower_right >= 2) && (vPower_right < 4))
	{
		strcpy(vDisCode1, "99");
		strncpy(vDisCode2, vOrg_Code, 2);
	}
	else if (( vPower_right >= 4) && (vPower_right <= 7))
	{
		strcpy(vDisCode1, "9999");
		strncpy(vDisCode2, vOrg_Code, 4);
	}
	else
	{
		strcpy(vDisCode1, "9999999");
		strncpy(vDisCode2, vOrg_Code, 7);
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ��Ʒ�Ƿ��Ѿ�����");
#endif
	/*��ѯ��Ʒ�Ƿ��Ѿ�����*/
	EXEC SQL SELECT Count(*) INTO :vCount FROM sProductRelease
	WHERE (GROUP_ID like :vDisCode1||'%' or GROUP_ID like :vDisCode2||'%' )
	AND RELEASE_STATUS = 'Y'
	AND Product_Code = :vProduct_Code;
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯsProductRelease�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190001;
	}
	if (vCount <= 0)
	{
		sprintf(grpUserMsg->return_message, "��Ʒ[%s]�����ڻ���δ����!",vProduct_Code);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190002;
	}

	/*��ѯ���ŵ�����Ϣ*/
	EXEC SQL SELECT VPMN_FLAG INTO :vVPMN_Flag
			FROM dCustDocOrgAdd
			WHERE Cust_Id = To_Number(:vCust_Id);
	if(SQLCODE == 1403)
	{
		sprintf(grpUserMsg->return_message, "����[%s]������Ϣ������!", vCust_Id);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190003;
	}
	else if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯdCustDocOrgAdd�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190004;
	}
	/*��VPMN���ſͻ�����������VPMN��Ʒ*/
	if (('0' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
	{
		sprintf(grpUserMsg->return_message, "��VPMN���ſͻ�,��������VPMN��Ʒ");
		return 190005;
	}

	/*��ѯ���ź�ͬ��Ϣ*/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ���ź�ͬ��Ϣ");
#endif
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL SELECT Unit_Id, Trim(Unit_Code), Contract_Id, Trim(Contract_No)
	**INTO :vUnit_Id, :vUnit_Code, :vContract_Id, :vContract_No
	**FROM dGrpContract
	**WHERE Cust_Id = To_Number(:vCust_Id);
	*/
	EXEC SQL SELECT Unit_Id, Trim(Unit_Id), Contract_Id,Contract_Id
	INTO :vUnit_Id, :vUnit_Code, :vContract_Id,:vContract_No
	FROM dGrpContract
	WHERE Cust_Id = To_Number(:vCust_Id);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

	if(SQLCODE == 1403)
	{
		sprintf(grpUserMsg->return_message, "����[%s]��ͬ��Ϣ��δ¼��!", vCust_Id);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190006;
	}
	else if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯdGrpContract�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190007;
	}

	/*VPMN�û�����ʱ�����ؼ��ſͻ�ϵͳ��BOSS_VPMN_CODE�ֶ�*/
	Trim(vGrp_No);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**if (('1' == vVPMN_Flag[0]) && (0 == strcmp(vGrpProduct_Code, VPMN_SM_CODE)))
	**{
	**    EXEC SQL UPDATE dCustDocOrgAdd
	**                SET BOSS_VPMN_CODE = :vGrp_No
	**              WHERE Cust_Id = To_Number(:vCust_Id);
	**    if(SQLCODE!=OK){
	**        sprintf(grpUserMsg->return_message, "����dCustDocOrgAdd�����![%d]",SQLCODE);
	**        pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**        return 190007;
	**    }
	**}
	*/

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

	Trim(vUnit_Id);
	Trim(vUnit_Code);
	Trim(vContract_Id);
	Trim(vContract_No);
	/*��ר��ҵ���⣬����ͬһ����Ʒ��������ͻ��ظ�����*/
	if ( (strcmp(vGrpProduct_Code, ZHUANX_IP_SM_CODE) != 0) &&
	(strcmp(vGrpProduct_Code, ZHUANX_DIANLU_SM_CODE) != 0) &&
	(strcmp(vGrpProduct_Code, ZHUANX_NET_SM_CODE) != 0) )
	{
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯdGrpUserMsg��Ϣ");
#endif
		EXEC SQL SELECT Count(*) INTO :vCount
			FROM dGrpUserMsg
			WHERE Cust_Id = To_Number(:vCust_Id)
			AND Product_Code = :vProduct_Code
			AND Bill_Date > Last_Day(sysdate) + 1;
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��ѯdGrpUserMsg�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190010;
		}
		if (vCount > 0)
		{
			sprintf(grpUserMsg->return_message, "�����û��Ѿ�����[%s]��Ʒ!",vProduct_Code);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190011;
		}
	}

	/*��ѯ�����ʻ���Ϣ*/
	/*yangzh modified 2004/12/13
	**#ifdef _DEBUG_
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ�����ʻ���Ϣ");
	**#endif
	**EXEC SQL SELECT Trim(Contract_No)
	**           INTO :vAccount_Id
	**           FROM dConMsg
	**          WHERE CON_CUST_ID = To_Number(:vCust_Id);
	**if(SQLCODE == 1403) {
	**    sprintf(grpUserMsg->return_message, "����[%s]�ʻ���Ϣ������!", vCust_Id);
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190008;
	**}
	**else if(SQLCODE!=OK){
	**    sprintf(grpUserMsg->return_message, "��ѯdConMsg�����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190009;
	**}
	**yangzh modified 2004/12/13*/

	/*��ѯdConMsg������Ƿ���ڼ�¼*/
	EXEC SQL SELECT COUNT(*) INTO :vCount
			FROM dConMsg
			WHERE CONTRACT_NO = To_Number(:vAccount_Id);
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯ�����û�dConMsg�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190009;
	}

	/*��¼dConMsg������¼*/
	if (vCount == 0)
	{ /*�ʻ���������ڣ��ͽ����ʻ���������,�����û�����ʱ����ѡ��ԭ�����ʻ�*/
		#ifdef _DEBUG_
		    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "���ɼ����û�ͳһ�����ʻ�");
		#endif

		/*��֯�������쿪����������  edit by liuweib at 23/02/2009*****
		**EXEC SQL INSERT INTO dConMsgHis
		**       (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
		**        ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
		**        STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
		**        MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
		**        PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
		**        ACCOUNT_TYPE, UPDATE_LOGIN,  UPDATE_ACCEPT,   UPDATE_DATE,
		**        UPDATE_TIME,  UPDATE_CODE,   UPDATE_TYPE, GROUP_ID)
		**      VALUES
		**       (:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
		**        0,            :vOrg_Code,    0,               sysdate,
		**        'Y',          sysdate,       '0',             0,
		**        :vDateChar,   0,             0,               'A',
		**        :vPay_Code,   ' ',           ' ',             ' ',
		**        '1',          :vLogin_No,    :lLoginAccept,   :vTotal_Date,
		**        sysdate,      :vOp_Code,     'A',:vGroupId);
		**if(SQLCODE!=OK){
		**    sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgHis�����![%d]",SQLCODE);
		**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**    return 190008;
		**}
		**EXEC SQL INSERT INTO dConMsg
		**       (CONTRACT_NO,  CON_CUST_ID,   CONTRACT_PASSWD, BANK_CUST,
		**        ODDMENT,      BELONG_CODE,   PREPAY_FEE,      PREPAY_TIME,
		**        STATUS,       STATUS_TIME,   POST_FLAG,       DEPOSIT,
		**        MIN_YM,       OWE_FEE,       ACCOUNT_MARK,    ACCOUNT_LIMIT,
		**        PAY_CODE,     BANK_CODE,     POST_BANK_CODE,  ACCOUNT_NO,
		**        ACCOUNT_TYPE, GROUP_ID)
		**      VALUES
		**       (:vAccount_Id, :vCust_Id,     :vAccount_Passwd, :vGrp_Name,
		**        0,            :vOrg_Code,    0,               sysdate,
		**        'Y',          sysdate,       '0',             0,
		**        :vDateChar,   0,             0,               'A',
		**        :vPay_Code,   'NULL',        'NULL',          'NULL',
		**        '1', :vGroupId);
		**if(SQLCODE!=OK){
		**    sprintf(grpUserMsg->return_message, "���뼯���û�dConMsg�����![%d]",SQLCODE);
		**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**    return 190009;
		**}
		***��֯�������쿪����������  edit by liuweib at 23/02/2009*/

		sprintf(vLoginAccept,"%ld",lLoginAccept);
		Trim(vLoginAccept);

		EXEC SQL select to_char(sysdate,'yyyymmddhh24miss')
				into :vStatusTime
				from dual;
		if(SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯʱ�����!");
			return 190099;
		}

	strcpy(accDoc.vLogin_Accept,      	vLoginAccept);
	strcpy(accDoc.vContract_No,       	 vAccount_Id);
	strcpy(accDoc.vCon_Cust_Id,       	    vCust_Id);
	strcpy(accDoc.vContract_Passwd,	 vAccount_Passwd);
	strcpy(accDoc.vBank_Cust,      	       vGrp_Name);
	strcpy(accDoc.vBelong_Code,            vOrg_Code);
	strcpy(accDoc.vAccount_Limit,     	         "A");
	strcpy(accDoc.vStatus,            	         "Y");
	strcpy(accDoc.vPost_Flag,         	         "0");
	strcpy(accDoc.vPay_Code,          	   vPay_Code);
	strcpy(accDoc.vBank_Code,      	          "NULL");
	strcpy(accDoc.vPost_Bank_Code,    	      "NULL");
	strcpy(accDoc.vAccount_No,      	        "NULL");
	strcpy(accDoc.vAccount_Type,    	           "1");
	strcpy(accDoc.vBegin_Time,      	   vStatusTime);
	strcpy(accDoc.vEnd_Time,         	            "");
	strcpy(accDoc.vOp_Code,           	    vOp_Code);
	strcpy(accDoc.vLogin_No,          	   vLogin_No);
	strcpy(accDoc.vOrg_Code,          	   vOrg_Code);
	strcpy(accDoc.vOp_Note,           	    vOp_Note);
	strcpy(accDoc.vSystem_Note,       	    vOp_Note);
	strcpy(accDoc.vIp_Addr,           	   "0.0.0.0");
	strcpy(accDoc.vGroup_Id,          	   vGroup_Id);
	strcpy(accDoc.vUpdate_Type,       	         "A");

	accDoc.vOddment  = 0.00;
	accDoc.vPrepay_Fee  = 0.00;
	accDoc.vDeposit = 0.00;
	accDoc.vOwe_Fee = 0.00;
	accDoc.vAccount_Mark = 0.00;

	ret = 0;
	/************ng���� Ϊ����send_idһ�� �滻ԭ�����ʻ���Ϣ�������� begin****************/
		/*      ret = createConMsg(&accDoc,vReturnMsg);*/
		ret = createConMsg_ng(ORDERIDTYPE_USER, vGrp_Id, &accDoc, vReturnMsg);
	/************ng���� Ϊ����send_idһ�� �滻ԭ�����ʻ���Ϣ�������� end******************/

	if(ret != 0)
	{
		sprintf(grpUserMsg->return_message, vReturnMsg);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "-----%s:%d:%s---",__FILE__,ret,vReturnMsg);
		return 190009;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConMsgPre");
	#endif
	/*ng���� by zhaohx at 02/10/2009 begin*/
	/***EXEC SQL INSERT INTO dConMsgPre
	**            ( CONTRACT_NO, PAY_TYPE,  PREPAY_FEE, LAST_PREPAY,
	**              ADD_PREPAY,  LIVE_FLAG, ALLOW_PAY,  BEGIN_DT,
	**              END_DT)
	**       VALUES
	**           ( To_Number(:vAccount_Id), '0', 0, 0,
	**             0, '0', 0, :total_date,
	**             :vSrv_Stop);
	**if (SQLCODE != 0) {
	**    sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgPre����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190010;
	**}*/

			init(count);
			init(vIdType);
			init(chargeflag_str);
			init(feecode_str);
			init(detailcode_str);
			init(actionfee_str);
			init(vOpNote);
	strcpy(count,grpUserMsg->feeList.iFeeCount);
	for(i=0; i< atoi(count); i++)
	{
		strcat(chargeflag_str,grpUserMsg->feeList.charge_flag[i]);
		strcat(feecode_str,grpUserMsg->feeList.fee_code[i]);
		strcat(detailcode_str,grpUserMsg->feeList.detail_code[i]);
		strcat(actionfee_str,grpUserMsg->feeList.action_fee[i]);

		strcat(chargeflag_str, "|");
		strcat(feecode_str, "|");
		strcat(detailcode_str, "|");
		strcat(actionfee_str, "|");
	}
	memset(vParamList,0,MAX_PARMS_NUM*2*DLINTERFACEDATA);
				iOrderRight=100;
				strcpy(vIdType,"2");
				strcpy(vOpNote,"ҵ�񹤵���¼dConMsgPre��wGrpOutSideFee��");

				strcpy(vCallServiceName,"bodb_CreGrpUser");
				vSendParaNum=9;
				vRecpParaNum=2;

				strcpy(vParamList[0][0],"contract_no");
				strcpy(vParamList[0][1],vAccount_Id);

				strcpy(vParamList[1][0],"total_date");
				strcpy(vParamList[1][1],total_date);

				strcpy(vParamList[2][0],"end_time");
				strcpy(vParamList[2][1],vSrv_Stop);

				strcpy(vParamList[3][0],"grpIdNo");
				strcpy(vParamList[3][1],vGrp_Id);

				strcpy(vParamList[4][0],"vCount");
				strcpy(vParamList[4][1],count);

				strcpy(vParamList[5][0],"chargeflag_str");
				strcpy(vParamList[5][1],chargeflag_str);

				strcpy(vParamList[6][0],"feecode_str");
				strcpy(vParamList[6][1],feecode_str);

				strcpy(vParamList[7][0],"detailcode_str");
				strcpy(vParamList[7][1],detailcode_str);

				strcpy(vParamList[8][0],"actionfee_str");
				strcpy(vParamList[8][1],actionfee_str);
				/*modify by piaoyi at 2011/12/21 ҵ�񹤵�Ӧ��dGrpUserMsg���ݹ�����
				ret=0;
				ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
				                          vIdType,vGrp_Id,iOrderRight,vLoginAccept,vOp_Code,vLogin_No,vOpNote);
				if(ret!=0)
				{
					sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgPre����![%d]",SQLCODE);
					pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
					return 190010;
				}
					printf("v_ret=%d\n",ret);
				*/
			/*ng���� by zhaohx at 02/10/2009 begin*/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustConMsgHis");
#endif
			EXEC SQL INSERT INTO dCustConMsgHis
			(	Cust_Id,         Contract_No,     Begin_Time,
				End_Time,        Update_Accept,   Update_Login,
				Update_Date,     Update_Time,     Update_Code,
				Update_Type
			)
			VALUES
			(	To_Number(:vCust_Id),To_Number(:vAccount_Id), sysdate,
				To_Date(:vSrv_Stop,'YYYYMMDD'),
				To_Number(:lSysAccept),  :vLogin_No,
				To_Number(:total_date),  sysdate, :op_code,
				'a'
			);
		if (SQLCODE != 0)
		{
			sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsgHis����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			printf("vSrv_Stop=[%s]total_date[%s]\n",vSrv_Stop,total_date);
			return 190010;
		}

		#ifdef _DEBUG_
		    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustConMsg");
		#endif
		/*NG��ź
		**EXEC SQL INSERT INTO dCustConMsg
		**            (  Cust_Id,  Contract_No,  Begin_Time,    End_Time )
		**            VALUES
		**            (   To_Number(:vCust_Id), To_Number(:vAccount_Id),
		**                sysdate, To_Date(:vSrv_Stop,'YYYYMMDD')
		**            );
		**if (SQLCODE != 0) {
		**    sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
		**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**    return 190010;
		**}
		NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		init(BeginTime1);
		EXEC SQL select to_char(sysdate,'yyyymmdd hh24:mi:ss')
				 into :BeginTime1
				 from dual;
		if (SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ȡʱ��ʧ��");
			return 192010;
		}
		Trim(BeginTime1);
		memset(&sTdCustConMsg,0,sizeof(TdCustConMsg));
		strcpy(sTdCustConMsg.sCustId,vCust_Id);
		strcpy(sTdCustConMsg.sContractNo,vAccount_Id);
		strcpy(sTdCustConMsg.sBeginTime,BeginTime1);
		strcpy(sTdCustConMsg.sEndTime,vSrv_Stop);
		v_ret=0;
		v_ret=OrderInsertCustConMsg("2",vGrp_Id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdCustConMsg);

		if(0!=v_ret)
		{
			sprintf(grpUserMsg->return_message, "���뼯���û�dCustConMsg����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190010;
		}
	/*ng����  by magang at 20090812 END*/
	}
/*add by lixiaoxin at 20130425 begin*/	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser1", "", "insert into dCustUserHis");
#endif

	EXEC SQL INSERT INTO dCustUserHis
		(CUST_ID,ID_NO,REP_FLAG,CUST_FLAG, UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_ACCEPT,OP_CODE)
		VALUES
		(to_number(:vCust_Id), to_number(:vGrp_Id), '0','0',to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vLogin_No,'a',  :lLoginAccept,:vOp_Code);
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "����dCustUserHis����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser1");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser1","190025",grpUserMsg->return_message);
		return 190025;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser1", "", "insert into dCustUserMsg");
#endif

	EXEC SQL INSERT INTO dCustUserMsg
		(CUST_ID,ID_NO,REP_FLAG,CUST_FLAG)
		VALUES
		(to_number(:vCust_Id), to_number(:vGrp_Id), '0','0');
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "����dCustUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fpubCreateGrpUser1");
		pubLog_DBErr(_FFL_,"fpubCreateGrpUser1","190026",grpUserMsg->return_message);
		return 190026;
	}
/*add by lixiaoxin at 20130425 end*/	

	GetInnerUserNo(vRegion_Code, vSm_Code, vGrp_UserNo, grpUserMsg->return_message);
	Trim(vGrp_UserNo);
	if(atol(vGrp_UserNo) <= 0)
	{
		sprintf(grpUserMsg->return_message, "ȡ�����ڲ����벻�ɹ���������ȷ��!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return ERR_GET_MAX_SYS_ACCEPT;
	}
	/*��֯�������������ֶ�group_id  edit by liuweib at 19/02/2009*/

#ifdef _CHGTABLE_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into wUserMsgChg");
	EXEC SQL    INSERT INTO wUserMsgChg
	(
		OP_NO,OP_TYPE,ID_NO,CUST_ID,PHONE_NO,SM_CODE,
		ATTR_CODE,BELONG_CODE,RUN_CODE,RUN_TIME,OP_TIME, GROUP_ID
	)
	VALUES
	(
		sMaxBillChg.NEXTVAL, '1', TO_NUMBER(:vGrp_Id), TO_NUMBER(:vCust_Id),:vGrp_UserNo,:vSm_Code,
		'00000000',:vRegion_Code||:vDistrict_Code||:vTown_Code, 'AA',
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),
		SUBSTR(:vOp_Time,1,8)||SUBSTR(:vOp_Time,10,2)
		||SUBSTR(:vOp_Time,13,2)||SUBSTR(:vOp_Time,16,2),:vGroup_Id
	);
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�wUserMsgChg����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190016;
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
		sysdate,     :op_code,    'a'
	);
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsgHis����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190010;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dConUserMsg");
	#endif
	/* NG����
	**EXEC SQL    INSERT INTO dConUserMsg
	**(
	**	Id_No,      Contract_No,  Serial_No,
	**	Bill_Order, Pay_Order,    Begin_YMD,
	**	Begin_TM,   End_YMD,      End_TM,
	**	Limit_Pay,  Rate_Flag,    Stop_Flag
	**)
	**VALUES
	**(
	**	To_Number(:vGrp_Id),To_Number(:vAccount_Id),0,
	**	99999999,   1,           :total_date,
	**	SubStr(:total_date,1,6),
	**	:vSrv_Stop, SubStr(:vSrv_Stop,1,6),
	**	9999999999, 'N', 'Y'
	**);
	**if (SQLCODE != 0) {
	**    sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190010;
	**}
	**NG���� */
				/*ng����  by magang at 20090812 begin*/
					init(TotalDate1);
					EXEC SQL SELECT  SubStr(:total_date,1,6),SubStr(:vSrv_Stop,1,6),to_char(to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'), 'hh24miss')
							INTO :TotalDate1,:SrvStop1,:sBeginTm
							 FROM	DUAL;
					if (SQLCODE !=0 )
					{
						sprintf(grpUserMsg->return_message, "��ȡʱ�����![%d]",SQLCODE);
						pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
						return 193010;
					}
					Trim(TotalDate1);
					Trim(SrvStop1);
					memset(&tdConUserMsg,0,sizeof(TdConUserMsg));
					strcpy(tdConUserMsg.sIdNo,vGrp_Id);
					strcpy(tdConUserMsg.sContractNo,vAccount_Id);
					strcpy(tdConUserMsg.sBillOrder,"99999999");
					strcpy(tdConUserMsg.sSerialNo,"0");
					strcpy(tdConUserMsg.sPayOrder,"1");
					strcpy(tdConUserMsg.sBeginYmd,total_date);
					strcpy(tdConUserMsg.sBeginTm,sBeginTm);
					strcpy(tdConUserMsg.sEndYmd,vSrv_Stop);
					strcpy(tdConUserMsg.sEndTm,"235959");
					strcpy(tdConUserMsg.sLimitPay,"9999999999");
					strcpy(tdConUserMsg.sRateFlag,"N");
					strcpy(tdConUserMsg.sStopFlag,"Y");
			v_ret=0;
			v_ret=OrderInsertConUserMsg("2",vGrp_Id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,tdConUserMsg);

			if(0!=v_ret)
			{
				sprintf(grpUserMsg->return_message, "���뼯���û�dConUserMsg����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				return 190010;
			}
		/*ng����  by magang at 20090811 end*/


/* yangzh disabled 20050124 ����dCustMsg�����,�Ͳ�����wConUserChg */

	#ifdef  _CHGTABLE_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into wConUserChg");
	EXEC SQL INSERT INTO wConUserChg
		(
			Op_No,       Op_Type,    Id_No,
			Contract_No, Bill_Order, Conuser_Update_Time,
			Limit_Pay,   Rate_Flag,  Fee_Code, detail_code,
			Fee_Rate,   Op_Time
		)
		VALUES
		(
			sMaxBillChg.NextVal,     '10',     To_Number(:vGrp_Id),
			To_Number(:vAccount_Id), 99999999, To_Char(sysdate,'YYYYMMDDHH24MISS'),
			0,          '0',      '*', '*',
			0,    To_Char(sysdate,'YYYYMMDDHH24MISS')
		 );
	if (SQLCODE != 0)
	{
		sprintf(grpUserMsg->return_message, "���뼯���û�wConUserChg����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190010;
	}
	#endif

		/* GetInnerUserNoNormal(vRegion_Code, vSm_Code, vGrp_UserNo, grpUserMsg->return_message); */

	/* ��¼ dCustMsg */
	/*��֯�������������ֶ� group_id  edit by liuweib at 19/02/2009*/

	/*��֯�������쿪����������  edit by liuweib at 23/02/2009*
	**EXEC SQL  INSERT INTO dCustMsgHis
	**(
	**        ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**        ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**        RUN_CODE,OPEN_TIME,RUN_TIME,
	**        BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,
	**        UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,GROUP_ID,GROUP_NO
	**)
	**VALUES
	**(
	**        to_number(:vGrp_Id), to_number(:vCust_Id), to_number(:vAccount_Id),1,:vGrp_UserNo,:vSm_Code,:serviceType,
	**        '00000000',:vUser_Passwd,:vRegion_Code||:vDistrict_Code||:vTown_Code,0, 'A', 0,
	**        'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**        to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), to_number(:vBill_Type),'NULL',       0,:vBill_Type,'001',
	**        :lLoginAccept, to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_number(:vTotal_Date),
	**        :vLogin_No, 'a', :vOp_Code,:vGroup_Id,:vGroupId
	**);
	**if(SQLCODE != 0)
	**{
	**		sprintf(grpUserMsg->return_message, "����dCustMsgHis����![%d]",SQLCODE);
	**		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**		return 190027;
	**}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "insert into dCustMsg");
#endif
	**EXEC SQL        INSERT INTO dCustMsg
	**(
	**	ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**	ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**	RUN_CODE,OPEN_TIME,RUN_TIME,
	**	BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,GROUP_ID,GROUP_NO
	**)
	**VALUES
	**(
	**	to_number(:vGrp_Id), to_number(:vCust_Id), to_number(:vAccount_Id),1,:vGrp_UserNo,:vSm_Code,:serviceType,
	**	'00000000',:vUser_Passwd,:vRegion_Code||:vDistrict_Code||:vTown_Code,0, 'A', 0,
	**	'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**	to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'), to_number(:vBill_Type),'NULL',       0,:vBill_Type,'001',:vGroup_Id, :vGroupId
	**);
	**if(SQLCODE != 0)
	**{
	**	sprintf(grpUserMsg->return_message, "����dCustMsg����![%d]",SQLCODE);
	**	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**	return 190028;
	**}
	*************��֯�������쿪����������  edit by liuweib at 23/02/2009*/

	strncpy(userDoc.vPhone_No,       	vGrp_UserNo,       15);
	strncpy(userDoc.vSm_Code,        	vSm_Code,          	2);
	strncpy(userDoc.vService_Type,   	serviceType,     	2);
	strncpy(userDoc.vAttr_Code,      	"00000000",        	8);
	strncpy(userDoc.vUser_Passwd,    	vUser_Passwd,         	8);
	strncpy(userDoc.vOpen_Time,      	vOp_Time,         	17);
	sprintf(userDoc.vBelong_Code,    	"%s%s%s",   vRegion_Code,vDistrict_Code,vTown_Code);
	strncpy(userDoc.vCredit_Code,    	"A",      	1);
	strncpy(userDoc.vRun_Code,       	"AA",              	2);
	strncpy(userDoc.vRun_Time,       	vOp_Time,         	17);
	strncpy(userDoc.vBill_Date,      	vOp_Time,         17);
	strncpy(userDoc.vEncrypt_Prepay, 	"UnKnown",         	7);
	strncpy(userDoc.vLast_Bill_Type, 	vBill_Type,        	1);
	strncpy(userDoc.vBak_Field,      	"001",         	3);
	strncpy(userDoc.vOp_Time,        	vOp_Time,         	17);
	strncpy(userDoc.vLogin_No,       	vLogin_No,         	6);
	strncpy(userDoc.vOp_Code,        	vOp_Code,          	4);
	strncpy(userDoc.vGroup_Id,       	vGroup_Id,       	10);
	userDoc.vCust_Id               	= atol(vCust_Id);
	userDoc.vId_No                 	= atol(vGrp_Id);
	userDoc.vContract_No           	= atol(vAccount_Id);
	userDoc.vLimit_Owe             	= 0;
	userDoc.vCredit_Value          	= 0;
	userDoc.vBill_Type             	= atoi(vBill_Type);
	userDoc.vCmd_Right             	= 0;
	userDoc.vTotal_Date            	= atoi(vTotal_Date);
	userDoc.vIds                   	= 1;
	userDoc.vLogin_Accept           = lLoginAccept;

	ret=0;
	ret = createCustMsg(&userDoc,vReturnMsg);
	if(ret != 0)
	{
		sprintf(grpUserMsg->return_message, vReturnMsg);
	  pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "-----%s:%d:%s--",__FILE__,ret,vReturnMsg);
	  return 190028;
	}

	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "vProduct_Type[%s]",vProduct_Type);
	init(vProduct_Type);
	EXEC SQL select PRODUCT_CODE
			into :vProduct_Type
			from sGrpSmCode
			where sm_code=:vSm_Code;
	if(SQLCODE!=0)
	{
		sprintf(grpUserMsg->return_message, "select sGrpSmCode err![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190038;
	}
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "sGrpSmCode.vProduct_Type[%s]",vProduct_Type);

	Trim(vProduct_Type);

	/*4.��¼�����û���dGrpUserMsg��¼,BOSS�������*/
	/*edit by cencm at 2011-11-7 16:39:34  ����˫V������*/
	if((strcmp(vSm_Code,"23") == 0) || (strcmp(vSm_Code,"01") == 0) || (strcmp(vSm_Code,"mn") == 0))
	{
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsgHis����Ϣ");
	 	#endif
		/*������ֶ�sm_code��group_id  edit by liuweib at 19/02/2009*/
		EXEC SQL INSERT INTO dGrpUserMsgHis
		(CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
		UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
		UPDATE_TYPE, SM_CODE, GROUP_ID)
		VALUES
		(:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_No,
		1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
		:vUser_Passwd, ' ',           ' ',            :vProvince,
		:vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
		1000000,     :vCredit_Code, :vCredit_Value,   'A',
		'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
		'1',           sysdate,       :vOp_Code,      :vLogin_No,
		:lLoginAccept, :vTotal_Date,  sysdate,        :vOp_Code,
		'A', :vSm_Code, :vGroup_Id);
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "����dGrpUserMsgHis�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190012;
		}


		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsg����Ϣ");
		#endif
		/*NG��ź
		**EXEC SQL INSERT INTO dGrpUserMsg
		**      (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		**       IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		**       USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		**       REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		**       LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		**       OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		**       LAST_BILL_TYPE, OP_TIME,    BAK_FIELD, SM_CODE, GROUP_ID)
		**    VALUES
		**     (:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_No,
		**      1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
		**      :vUser_Passwd, ' ',           ' ',            :vProvince,
		**      :vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
		**      1000000,       :vCredit_Code, :vCredit_Value, 'A',
		**      'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
		**      '1',           sysdate,       :vOp_Code, :vSm_Code, :vGroup_Id);
		**if (SQLCODE!=OK){
		**      sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
		**      pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**      return 190013;
		**}
		NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMsg!\n");
		memset(&sTdGrpUserMsg,0,sizeof(TdGrpUserMsg));
		init(TotalDate1);
		EXEC SQL SELECT  to_char(sysdate,'yyyymmdd hh24:mi:ss')
				INTO :TotalDate1
				FROM	DUAL;
		if (SQLCODE !=0 )
		{
			return 193010;
		}
		Trim(TotalDate1);
		strcpy(sTdGrpUserMsg.sCustId,vCust_Id);
		strcpy(sTdGrpUserMsg.sIdNo,vGrp_Id);
		strcpy(sTdGrpUserMsg.sAccountId,vAccount_Id);
		strcpy(sTdGrpUserMsg.sUserNo,vGrp_No);
		strcpy(sTdGrpUserMsg.sIds,"1");
		strcpy(sTdGrpUserMsg.sUserName,vGrp_Name);
		strcpy(sTdGrpUserMsg.sProductType,vProduct_Type);
		strcpy(sTdGrpUserMsg.sProductCode,vProduct_Code);
		strcpy(sTdGrpUserMsg.sUserPasswd,vUser_Passwd);
		strcpy(sTdGrpUserMsg.sLoginName," ");
		strcpy(sTdGrpUserMsg.sLoginPasswd," ");
		strcpy(sTdGrpUserMsg.sProvCode,vProvince);
		strcpy(sTdGrpUserMsg.sRegionCode,vRegion_Code);
		strcpy(sTdGrpUserMsg.sDistrictCode,vDistrict_Code);
		strcpy(sTdGrpUserMsg.sTownCode,vTown_Code);
		strcpy(sTdGrpUserMsg.sTerritoryCode,"0");
		strcpy(sTdGrpUserMsg.sLimitOwe,"1000000");
		strcpy(sTdGrpUserMsg.sCreditCode,vCredit_Code);
		strcpy(sTdGrpUserMsg.sCreditValue,vCredit_Value);
		strcpy(sTdGrpUserMsg.sRunCode,"A");
		strcpy(sTdGrpUserMsg.sOldRun,"A");
		strcpy(sTdGrpUserMsg.sRunTime,TotalDate1);
		strcpy(sTdGrpUserMsg.sBillDate,vSrv_Stop);
		strcpy(sTdGrpUserMsg.sBillType,"1");
		strcpy(sTdGrpUserMsg.sLastBillType,"1");
		strcpy(sTdGrpUserMsg.sOpTime,TotalDate1);
		strcpy(sTdGrpUserMsg.sBakField,vOp_Code);
		strcpy(sTdGrpUserMsg.sSmCode,vSm_Code);
		strcpy(sTdGrpUserMsg.sGroupId,vGroup_Id);
		v_ret=0;
		v_ret=OrderInsertGrpUserMsg("2",vGrp_Id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdGrpUserMsg);

		if(0!=v_ret)
		{
			sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190013;
		}
		/*ng����  by magang at 20090812 end*/
	}
	else
	{
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsgHis����Ϣ");
		#endif
		/*������ֶ�sm_code��group_id  edit by liuweib at 19/02/2009*/
		EXEC SQL INSERT INTO dGrpUserMsgHis
		(CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
		UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
		UPDATE_TYPE, SM_CODE, GROUP_ID)
		VALUES
		(:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_UserNo,
		1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
		:vUser_Passwd, ' ',           ' ',            :vProvince,
		:vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
		1000000,     :vCredit_Code, :vCredit_Value,   'A',
		'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
		'1',           sysdate,       :vOp_Code,      :vLogin_No,
		:lLoginAccept, :vTotal_Date,  sysdate,        :vOp_Code,
		'A', :vSm_Code, :vGroup_Id);
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "����dGrpUserMsgHis�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190012;
		}


		#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpUserMsg����Ϣ");
		#endif
		/*NG��ź
		**EXEC SQL INSERT INTO dGrpUserMsg
		**      (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		**       IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		**       USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		**       REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		**       LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		**       OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		**       LAST_BILL_TYPE, OP_TIME,    BAK_FIELD, SM_CODE, GROUP_ID)
		**    VALUES
		**     (:vCust_Id,     :vGrp_Id,      :vAccount_Id,   :vGrp_UserNo,
		**      1,             :vGrp_Name,    :vProduct_Type, :vProduct_Code,
		**      :vUser_Passwd, ' ',           ' ',            :vProvince,
		**      :vRegion_Code, :vDistrict_Code, :vTown_Code,  '0',
		**      1000000,       :vCredit_Code, :vCredit_Value, 'A',
		**      'A',           sysdate,       To_Date(:vSrv_Stop, 'YYYYMMDD'), '1',
		**      '1',           sysdate,       :vOp_Code, :vSm_Code, :vGroup_Id);
		**if (SQLCODE!=OK){
		**      sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
		**      pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		**      return 190013;
		**}
		NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMsg!\n");
		memset(&sTdGrpUserMsg,0,sizeof(TdGrpUserMsg));
		init(TotalDate1);
		EXEC SQL SELECT  to_char(sysdate,'yyyymmdd hh24:mi:ss')
				INTO :TotalDate1
				FROM	DUAL;
		if (SQLCODE !=0 )
		{
			return 193010;
		}
		Trim(TotalDate1);
		strcpy(sTdGrpUserMsg.sCustId,vCust_Id);
		strcpy(sTdGrpUserMsg.sIdNo,vGrp_Id);
		strcpy(sTdGrpUserMsg.sAccountId,vAccount_Id);
		strcpy(sTdGrpUserMsg.sUserNo,vGrp_UserNo);
		strcpy(sTdGrpUserMsg.sIds,"1");
		strcpy(sTdGrpUserMsg.sUserName,vGrp_Name);
		strcpy(sTdGrpUserMsg.sProductType,vProduct_Type);
		strcpy(sTdGrpUserMsg.sProductCode,vProduct_Code);
		strcpy(sTdGrpUserMsg.sUserPasswd,vUser_Passwd);
		strcpy(sTdGrpUserMsg.sLoginName," ");
		strcpy(sTdGrpUserMsg.sLoginPasswd," ");
		strcpy(sTdGrpUserMsg.sProvCode,vProvince);
		strcpy(sTdGrpUserMsg.sRegionCode,vRegion_Code);
		strcpy(sTdGrpUserMsg.sDistrictCode,vDistrict_Code);
		strcpy(sTdGrpUserMsg.sTownCode,vTown_Code);
		strcpy(sTdGrpUserMsg.sTerritoryCode,"0");
		strcpy(sTdGrpUserMsg.sLimitOwe,"1000000");
		strcpy(sTdGrpUserMsg.sCreditCode,vCredit_Code);
		strcpy(sTdGrpUserMsg.sCreditValue,vCredit_Value);
		strcpy(sTdGrpUserMsg.sRunCode,"A");
		strcpy(sTdGrpUserMsg.sOldRun,"A");
		strcpy(sTdGrpUserMsg.sRunTime,TotalDate1);
		strcpy(sTdGrpUserMsg.sBillDate,vSrv_Stop);
		strcpy(sTdGrpUserMsg.sBillType,"1");
		strcpy(sTdGrpUserMsg.sLastBillType,"1");
		strcpy(sTdGrpUserMsg.sOpTime,TotalDate1);
		strcpy(sTdGrpUserMsg.sBakField,vOp_Code);
		strcpy(sTdGrpUserMsg.sSmCode,vSm_Code);
		strcpy(sTdGrpUserMsg.sGroupId,vGroup_Id);
		v_ret=0;
		v_ret=OrderInsertGrpUserMsg("2",vGrp_Id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdGrpUserMsg);

		if(0!=v_ret)
		{
			sprintf(grpUserMsg->return_message, "����dGrpUserMsg�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190013;
		}
		/*ng����  by magang at 20090812 end*/
	}
	/*add by piaoyi at 2011/12/21  ҵ�񹤵�Ӧ�����ݹ����� begin*/
					ret=0;
				ret=SetOrderBusiSendCrm(vCallServiceName, vSendParaNum, vRecpParaNum, vParamList,
				                          vIdType,vGrp_Id,iOrderRight,vLoginAccept,vOp_Code,vLogin_No,vOpNote);
				if(ret!=0)
				{
					sprintf(grpUserMsg->return_message, "���뼯���û�dConMsgPre����![%d]",SQLCODE);
					pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
					return 190010;
				}
					printf("v_ret=%d\n",ret);
  /*add by piaoyi at 2011/12/21  ҵ�񹤵�Ӧ�����ݹ����� end*/

	/*NG��ź
	**EXEC SQL        INSERT INTO dAccountIdInfo
	**(
	**        MSISDN,SERVICE_TYPE,SERVICE_NO,RUN_CODE,RUN_TIME,INNET_TIME,NOTE
	**)
	**VALUES
	**(
	**        :vGrp_UserNo, :vSm_Code, :vGrp_No, 'AA', to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),
	**        to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS'),:vOp_Note
	**);
	**if (SQLCODE != 0)
	**{
	**    sprintf(grpUserMsg->return_message, "�����dAccountIdInfo����![%d]", SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190015;
	**}

	NG��ź*/
	/*ng����  by magang at 20090812 begin*/
	pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMsg!\n");
	Trim(vOp_Time);

	memset(&sTdAccountIdInfo,0,sizeof(TdAccountIdInfo));
	strcpy(sTdAccountIdInfo.sMsisdn,vGrp_UserNo);
	strcpy(sTdAccountIdInfo.sServiceType,vSm_Code);
	strcpy(sTdAccountIdInfo.sServiceNo,vGrp_No);
	strcpy(sTdAccountIdInfo.sRunCode,"AA");
	strcpy(sTdAccountIdInfo.sRunTime,vOp_Time);
	strcpy(sTdAccountIdInfo.sInnetTime,vOp_Time);
	strcpy(sTdAccountIdInfo.sNote,vOp_Note);
	v_ret=0;
		v_ret=OrderInsertAccountIdInfo("2",vGrp_Id,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,sTdAccountIdInfo);

		if(0!=v_ret)
		{
			sprintf(grpUserMsg->return_message, "�����dAccountIdInfo����![%d]", SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190015;

		}
	/*ng����  by magang at 20090812 end*/
	
	/*add by lixiaoxin at 20130425 begin*/
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
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
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
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190017;
	}
#endif
	/*add by lixiaoxin at 20130425 end*/
	/*��ѯsGrpOperationProductCode�õ�GRP���ҵ�����*/
	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯsGrpOperationProductCode����Ϣ");
	#endif
	EXEC SQL SELECT OPERATION_CODE
			INTO :vOperation_Code
			FROM sGrpOperationProductCode
			WHERE Product_Code = :vGrpProduct_Code;
	if(SQLCODE!=OK)
	{
		strcpy(grpUserMsg->return_message,"��ѯsGrpOperationProductCode����!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190014;
	}

	/*��ò�ƷID*/
	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ò�ƷID");
	#endif
	EXEC SQL SELECT To_Char(s_Product_Id.NextVal) INTO :vProduct_Id FROM DUAL;
	if(SQLCODE!=OK)
	{
	    strcpy(grpUserMsg->return_message,"��ѯs_Product_Id����!");
	    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	    return 190014;
	}

	/* LINA VIP2.0 2006_11_05 begin here*/
	/*ȡvip��֯��dgrpgroups��org_code*/
	EXEC SQL BEGIN DECLARE SECTION;
		char sIn_Grp_OrgCode[10+1];
	EXEC SQL END DECLARE SECTION;

	/*��ѯ���ſͻ���Ϣ*/
	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ���ſͻ���Ϣ");
	#endif
	/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL SELECT Org_Code
	**           INTO :sIn_Grp_OrgCode
	**           FROM dCustDocOrgAdd
	**          WHERE Cust_Id = To_Number(:vCust_Id)
	**            AND Unit_Id = To_Number(:vUnit_Id);
	**if(SQLCODE!=OK)
	**{
	**    strcpy(grpUserMsg->return_message,"��ѯOrg_Code����!");
	**    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
	**    return 190014;
	**}
	***/
	strcpy(sIn_Grp_OrgCode,            "");
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
	/*���뼯���û���չ��, ���Ų��*/
	/*product_statusȡ��dbgrpadm.sGrpProductStatusCode�ֵ��*/
	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpProductHis����Ϣ");
	#endif
	/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL INSERT INTO dGrpProductHis
	**     (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
	**      PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
	**      BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
	**      PRODUCT_STATUS, OPERATION_CODE,
	**      UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
	**      UPDATE_CODE,  UPDATE_TYPE, ORG_CODE)
	**     VALUES
	**      (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
	**       :vGrpProduct_Code, :vContract_Id, :vContract_No,
	**       To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**       :vOp_Note,
	**       '1',            :vOperation_Code,
	**       :vLogin_No,     :lLoginAccept, :vTotal_Date,  sysdate,
	**       :vOp_Code,      'A',           :sIn_Grp_OrgCode);
	***/

		EXEC SQL INSERT INTO dGrpProductHis
		(CUST_ID,      UNIT_ID,   PRODUCT_ID,
		PRODUCT_CODE, CONTRACT_ID, ORDER_TIME,
		BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		PRODUCT_STATUS, OPERATION_CODE,
		UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
		UPDATE_CODE,  UPDATE_TYPE,SAVE_FILE,CREATE_LOGIN,CREATE_DATE)
		VALUES
		(:vCust_Id,      :vUnit_Id,   :vProduct_Id,
		:vGrpProduct_Code, :vContract_Id,
		To_Date(:vTotal_Date,'yyyymmdd'),
		To_Date(:vSrv_Start,'yyyymmdd'),
		To_Date(:vSrv_Stop,'yyyymmdd'),
		To_Date(:vTotal_Date,'yyyymmdd'),
		:vOp_Note,
		'1',            :vOperation_Code,
		:vLogin_No,     :lLoginAccept, :vTotal_Date,  sysdate,
		:vOp_Code,      'A', '',:vLogin_No,sysdate);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "����dGrpProductHis�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190015;
	}
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpProduct����Ϣ");
	#endif

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL INSERT INTO dGrpProduct
	**     (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
	**      PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
	**      BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
	**      PRODUCT_STATUS, OPERATION_CODE, ORG_CODE)
	**     VALUES
	**      (:vCust_Id,      :vUnit_Id,     :vUnit_Code,   :vProduct_Id,
	**       :vGrpProduct_Code, :vContract_Id, :vContract_No,
	**       To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vSrv_Start,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vSrv_Stop,'yyyymmdd'), 'yyyy-mm-dd'),
	**       To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
	**       :vOp_Note,
	**       '1',            :vOperation_Code     ,:sIn_Grp_OrgCode);
	***/

	EXEC SQL INSERT INTO dGrpProduct
		(CUST_ID,      UNIT_ID,        PRODUCT_ID,
		PRODUCT_CODE, CONTRACT_ID , ORDER_TIME,
		BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		PRODUCT_STATUS, OPERATION_CODE,SAVE_FILE,CREATE_LOGIN,CREATE_DATE)
		VALUES
		(:vCust_Id,      :vUnit_Id,     :vProduct_Id,
		:vGrpProduct_Code, :vContract_Id,
		To_Date(:vTotal_Date,'yyyymmdd'),
		To_Date(:vSrv_Start,'yyyymmdd'),
		To_Date(:vSrv_Stop,'yyyymmdd'),
		To_Date(:vTotal_Date,'yyyymmdd'),
		:vOp_Note,
		'1',:vOperation_Code ,'',:vLogin_No,sysdate);


	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "����dGrpProduct�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190016;
	}

	/*��ö���ID*/
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ö���ID");
	#endif
	EXEC SQL SELECT To_Char(s_order_goods_id.NextVal) INTO :vOrder_GoodsId FROM DUAL;
	if(SQLCODE!=OK)
	{
		strcpy(grpUserMsg->return_message,"��ѯs_order_goods_id����!");
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190017;
	}

	/*���붩�����¼,���Ų��*/
	/*������Ч״̬CURR_STATUS��sGrpOrderGoodsStatusCode */
	/*����ʩ��״̬URR_TACHE��sGrpOrderGoodsTacheCode*/
	Trim(vProduct_Id);
	Trim(vOrder_GoodsId);
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpOrderGoodsHis����Ϣ");
	#endif
	/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
	EXEC SQL INSERT INTO dGrpOrderGoodsHis
		(ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
		UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
		DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
		CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
		CURR_TACHE,
		UPDATE_LOGIN,   UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
		UPDATE_CODE,    UPDATE_TYPE  , ORG_CODE)
		VALUES
		(:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
		:vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
		To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
		To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
		:vGrpProduct_Code, :vOp_Note,
		'1', '01', '01', '01',
		'01',
		:vLogin_No,      :lLoginAccept, :vTotal_Date,  sysdate,
		:vOp_Code,       'A'          , :sIn_Grp_OrgCode );
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "����dGrpOrderGoodsHis�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190018;
	}

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpOrderGoods����Ϣ");
	#endif
	EXEC SQL INSERT INTO dGrpOrderGoods
		(ORDER_GOODS_ID, UNIT_ID,       CUST_ID,       ORDER_GOODS_NO,
		UNIT_CODE,      PRODUCT_ID,    CONTRACT_ID,   CONTRACT_NO,
		DEAL_TIME,      INPUT_DATE,    PRODUCT_CODE,  NOTE,
		CURR_STATUS,    OPER_CODE,     APPLY_CODE,    OVER_CODE,
		CURR_TACHE ,    ORG_CODE)
		VALUES
		(:vOrder_GoodsId, :vUnit_Id,    :vCust_Id,      :vOrder_GoodsId,
		:vUnit_Code,     :vProduct_Id, :vContract_Id,  :vContract_No,
		To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
		To_Char(To_Date(:vTotal_Date,'yyyymmdd'), 'yyyy-mm-dd'),
		:vGrpProduct_Code, :vOp_Note,
		'1', '01', '01', '01',
		'01', :sIn_Grp_OrgCode);
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "����dGrpOrderGoods�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190019;
	}
	/* LINA VIP2.0 end here*/

	/*���붩��������¼��*/
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼wGrpOrderOpr����Ϣ");
	#endif
	EXEC SQL insert into wGrpOrderOpr
		(LOGIN_ACCEPT, CUST_ID,       UNIT_ID,    DIS_ORDER_GOODS_ID,
		 OPERATOR,     OPERATOR_TIME, CURR_TACHE, CURR_STATUS,
		 OPERATOR_CODE)
		values
		(:lLoginAccept, :vCust_Id,    :vUnit_Id,  :vOrder_GoodsId,
		 :vLogin_No,    sysdate,      '01',       '01',
		 '01');
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "����wGrpOrderOpr�����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190020;
	}

	/* ��ѯ��Ʒ������ϵ */
	vCount = 1;
	strToFilter(vProduct_Code, vProduct_List, strlen(vProduct_Code), sizeof(vProduct_List));
	strcpy(vProduct_All, vProduct_List);
	while (vCount > 0)
	{
		init(sqlStr);
		sprintf(sqlStr, "SELECT distinct a.product_code FROM sproductcode a, sproductrelease b WHERE a.product_code IN ( SELECT object_product FROM sproductcode a, sproductattach b WHERE a.product_code = b.product_code AND a.product_code IN (%s) MINUS SELECT object_product FROM sproductcode a, sProductExclude b WHERE a.product_code = b.product_code AND a.product_code IN (%s)) AND a.product_code = b.product_code AND b.release_status = 'Y' AND (b.GROUP_ID LIKE '%s%%' OR b.GROUP_ID LIKE '%s%%') AND SYSDATE BETWEEN b.begin_time AND b.end_time", vProduct_List, vProduct_List, vDisCode1, vDisCode2);
		#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ��Ʒ������ϵ, vDisCode1[%s], vDisCode[%s], vProduct_List[%s], vProduct_All[%s]", vDisCode1, vDisCode2, vProduct_List, vProduct_All);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
		#endif

		EXEC SQL PREPARE stmt001 FROM :sqlStr;
		if (SQLCODE != OK) {
		 sprintf(grpUserMsg->return_message, "׼����ѯ������Ʒ�α����![%d]",SQLCODE);
		 pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		 return 190020;
		}
		EXEC SQL DECLARE attachprod_cur CURSOR for stmt001;
		if (SQLCODE != OK) {
		 sprintf(grpUserMsg->return_message, "���������Ʒ�α����![%d]",SQLCODE);
		 pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		 return 190020;
		}

		vCount = 0;
		init(vProduct_List);
		EXEC SQL OPEN attachprod_cur;
		if(SQLCODE!=OK)
		{
			EXEC SQL CLOSE attachprod_cur;
			sprintf(grpUserMsg->return_message, "����wGrpOrderOpr�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			return 190020;
		}

		EXEC SQL FETCH attachprod_cur INTO :vProduct_Code;
		while(SQLCODE == 0)
		{
		vCount++;
		if (vCount > 1)
			sprintf(vProduct_List, "%s, '%s'", vProduct_List, vProduct_Code);
		else
			sprintf(vProduct_List, "'%s'", vProduct_Code);
		EXEC SQL FETCH attachprod_cur INTO :vProduct_Code;
		}
		EXEC SQL CLOSE attachprod_cur;

		if (strlen(vProduct_List) > 0)
		{
			sprintf(vProduct_All, "%s, %s", vProduct_All, vProduct_List);
		}
	}

	/* ��ѯ��Ʒ�����ϵ�� */
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ��Ʒ�����ϵ��");
	#endif
	init(sqlStr);
	sprintf(sqlStr, "select b.product_code, a.service_type, b.service_code, b.main_flag, b.fav_order, c.price_code from sSrvCode a, sProductSrv b, sSrvPrice c where a.service_code = b.service_code and a.service_code = c.service_code and b.product_code in (%s)", vProduct_All);
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��ѯ��Ʒ�����ϵ��, vProduct_All[%s]", vProduct_All);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "sqlStr[%s]", sqlStr);
	#endif

	EXEC SQL PREPARE stmt002 FROM :sqlStr;
	if (SQLCODE != OK)
	{
		sprintf(grpUserMsg->return_message, "������Ʒ�����ϵ�α����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190020;
	}

	EXEC SQL DECLARE srv_cur CURSOR for stmt002;
	if(SQLCODE!=OK)
	{
		sprintf(grpUserMsg->return_message, "��ѯ��Ʒ������Ӧ��ϵ����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190021;
	}

	EXEC SQL OPEN srv_cur;
	if(SQLCODE!=OK)
	{
		EXEC SQL CLOSE srv_cur;
		sprintf(grpUserMsg->return_message, "�򿪲�Ʒ������Ӧ��ϵ�α����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
		return 190022;
	}

	init(vProduct_Code);
	EXEC SQL FETCH srv_cur INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
	while(SQLCODE == 0)
	{

		/*��¼�����û�������Ʒ��,BOSS�������*/
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpSrvMsgHis����Ϣ");
		#endif
		EXEC SQL INSERT INTO dGrpSrvMsgHis
		(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		OP_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG,
		UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
		UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
		VALUES
		(:vGrp_Id,    :vService_Type, :vService_Code, :vPrice_Code,
		:vFav_Order, :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
		:vMain_Flag, :lLoginAccept,  :vLogin_No,     :vOp_Code,
		sysdate,     'Y', To_Date(:vSrv_Stop, 'YYYYMMDD'), 'N',
		:vLogin_No,     :lLoginAccept,  :vTotal_Date,
		sysdate,     :vOp_Code,       'A');
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsgHis�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			EXEC SQL CLOSE srv_cur;
			return 190027;
		}
		/*add by lixiaoxin at 20130425 begin*/
		/*��¼�Żݴ��뵽 dbasefav*/
		exec sql declare cur_dbasefav cursor for 
				select :vGrp_UserNo,substr(:userDoc.vBelong_Code, 1, 2),:vSm_Code,a.service_code,a.service_code,
				       a.fav_order,'0',:vOp_Time,:vSrv_Stop, nvl('', chr(0)),'0','0',:vGrp_Id,'0','0'
				  from sproductsrv a, ssrvcode b
				 where a.product_code = :vProduct_Code
				   and a.service_code = b.service_code
				   and b.service_type = '0';
		exec sql open cur_dbasefav;
		while(1)
		{
			memset(&tdBaseFav,0,sizeof(tdBaseFav));
			exec sql fetch cur_dbasefav into :tdBaseFav;
			if(SQLCODE != 0)
			{
				if(SQLCODE == 1403)
				{
					break;
				}
				else
				{
					sprintf(grpUserMsg->return_message, "��ѯsproductsrv,ssrvcode����![%d]",SQLCODE);
					pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
					exec sql close cur_dbasefav;
					EXEC SQL CLOSE srv_cur;
					return 190027;	
				}
			}
			v_ret=OrderInsertBaseFav(ORDERIDTYPE_USER,vGrp_Id,100,
												vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
												tdBaseFav);
			printf("OrderInsertBaseFav ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				sprintf(grpUserMsg->return_message, "��¼dBaseFav�����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				exec sql close cur_dbasefav;
				EXEC SQL CLOSE srv_cur;
				return 192224;
			}
			
		}
		exec sql close cur_dbasefav;
		#ifdef _CHGTABLE_
					EXEC SQL INSERT INTO wBaseFavChg
						(
							msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,fav_day,
							start_time,end_time,flag,deal_time,id_no
						)
					select
						:vGrp_UserNo,substr(:userDoc.vBelong_Code,1,2),:vSm_Code,a.service_code,a.service_code,a.fav_order,'0',
						to_date(:vOp_Time,'YYYYMMDD HH24:MI:SS'),to_date(:vSrv_Stop,'YYYYMMDD HH24:MI:SS'),
						'1', to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'),:vGrp_Id
					from sproductsrv a,ssrvcode b
						where a.product_code=:vProduct_Code and a.service_code=b.service_code
						and b.service_type='0';
					if(SQLCODE!=OK)
					{
						sprintf(grpUserMsg->return_message, "��¼wBaseFavChg�����![%d]",SQLCODE);
						pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
						EXEC SQL CLOSE srv_cur;
						return 191124;
					}
		#endif
		/*add by lixiaoxin at 20130425 end*/
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "��¼dGrpSrvMsg����Ϣ");
		#endif
		EXEC SQL INSERT INTO dGrpSrvMsg
		(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		OP_TIME,   USE_FLAG, UNVAILD_TIME, PERSON_FLAG)
		VALUES
		(:vGrp_Id,    :vService_Type, :vService_Code, :vPrice_Code,
		:vFav_Order, :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
		:vMain_Flag, :lLoginAccept, :vLogin_No,      :vOp_Code,
		sysdate,'Y', To_Date(:vSrv_Stop, 'YYYYMMDD'),'N');
		if(SQLCODE!=OK)
		{
			sprintf(grpUserMsg->return_message, "��¼dGrpSrvMsg�����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubCreateGrpUser1", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
			EXEC SQL CLOSE srv_cur;
			return 190024;
		}
		EXEC SQL FETCH srv_cur INTO :vProduct_Code, :vService_Type, :vService_Code, :vMain_Flag, :vFav_Order, :vPrice_Code;
	}

	EXEC SQL CLOSE srv_cur;
	
	

	/*��ͨv�������ں�v��,������������,ͨ�������ҵ��������Żݴ����¼��dgrpsrvmsg�� chendx*/
	if(strlen(vServiceCode) > 0)
	{
		pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "�����ײͱ���[%s]�ҵ��������Żݴ����ٴμ�¼dGrpSrvMsg����Ϣ",vServiceCode);

		EXEC SQL DECLARE svr_cur CURSOR  for 
				SELECT A.SERVICE_TYPE,C.PRICE_CODE,d.detail_code
				   FROM SSRVCODE A, sgrpproductsrv B, SSRVPRICE C,sgrpsrvdetail d
				  WHERE B.PRODUCT_CODE = :vProduct_Code
				    AND B.SERVICE_CODE = d.SERVICE_CODE
				    AND A.SERVICE_CODE = d.detail_code
				    AND A.SERVICE_CODE = C.SERVICE_CODE			
				    AND b.SERVICE_CODE = :vServiceCode;
		EXEC SQL OPEN svr_cur;
		for(;;)
		{
			init(vSrvTypeTmp);
			init(vPriceCodeTmp);
			init(vDetailCode);
			EXEC SQL FETCH svr_cur  into :vSrvTypeTmp,:vPriceCodeTmp,:vDetailCode;
			if(0 != SQLCODE)
			{
				if(1403 == SQLCODE)
				{
					printf("�����ݣ�\n");
					break;
				}
				sprintf(grpUserMsg->return_message, "��ȡ�Żݴ���ʧ��![%s][%d",vServiceCode,SQLCODE);
				pubLog_DBErr(_FFL_,"fpubCreateGrpUser1","","��ȡ�Żݴ���ʧ��[%s][%d]",vServiceCode,SQLCODE);
				EXEC SQL CLOSE svr_cur;
				return 190032;
			}
			Trim(vSrvTypeTmp);
			Trim(vPriceCodeTmp);
			Trim(vDetailCode);
	
			EXEC SQL INSERT INTO dGrpSrvMsgHis
				(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
				SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
				MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
				OP_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG,
				UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
				UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
			VALUES
				(:vGrp_Id,    :vSrvTypeTmp, :vDetailCode, :vPriceCodeTmp,
				 '0', :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
				 '0', :lLoginAccept,  :vLogin_No,     :vOp_Code,
				sysdate,     'Y', To_Date(:vSrv_Stop, 'YYYYMMDD'), 'N',
				:vLogin_No,     :lLoginAccept,  :vTotal_Date,
				sysdate,     :vOp_Code,       'A');
			if(SQLCODE!=OK)
			{
				sprintf(grpUserMsg->return_message, "��¼���Ų�Ʒ�Ż���ʷ�����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubCreateGrpUser1", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				EXEC SQL CLOSE svr_cur;
				return 190034;
			}
			/*add by lixiaoxin at 20130425 begin*/
			/*��¼�Żݴ��뵽 dbasefav*/
			exec sql declare cur_dbasefav1 cursor for 
					select :vGrp_UserNo,substr(:userDoc.vBelong_Code, 1, 2),:vSm_Code,a.service_code,a.service_code,
					       a.fav_order,'0',:vOp_Time,:vSrv_Stop, nvl('', chr(0)),'0','0',:vGrp_Id,'0','0'
					  from sproductsrv a, ssrvcode b
					 where a.product_code = :vProduct_Code
					   and a.service_code = b.service_code
					   and b.service_type = '0';
			exec sql open cur_dbasefav1;
			while(1)
			{
				memset(&tdBaseFav,0,sizeof(tdBaseFav));
				exec sql fetch cur_dbasefav1 into :tdBaseFav;
				if(SQLCODE != 0)
				{
					if(SQLCODE == 1403)
					{
						break;
					}
					else
					{
						sprintf(grpUserMsg->return_message, "��ѯsproductsrv,ssrvcode����![%d]",SQLCODE);
						pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
						exec sql close cur_dbasefav1;
						EXEC SQL CLOSE srv_cur;
						return 190027;	
					}
				}
				v_ret=OrderInsertBaseFav(ORDERIDTYPE_USER,vGrp_Id,100,
													vOp_Code,lLoginAccept,vLogin_No,vOp_Note,
													tdBaseFav);
				printf("OrderInsertBaseFav ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					sprintf(grpUserMsg->return_message, "��¼dBaseFav�����![%d]",SQLCODE);
					pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
					exec sql close cur_dbasefav1;
					EXEC SQL CLOSE srv_cur;
					return 192224;
				}
				
			}
			exec sql close cur_dbasefav1;
			#ifdef _CHGTABLE_
						EXEC SQL INSERT INTO wBaseFavChg
							(
								msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,fav_day,
								start_time,end_time,flag,deal_time,id_no
							)
						select
							:vGrp_UserNo,substr(:userDoc.vBelong_Code,1,2),:vSm_Code,a.service_code,a.service_code,a.fav_order,'0',
							to_date(:vOp_Time,'YYYYMMDD HH24:MI:SS'),to_date(:vSrv_Stop,'YYYYMMDD HH24:MI:SS'),
							'1', to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'),:vGrp_Id
						from sproductsrv a,ssrvcode b
							where a.product_code=:vProduct_Code and a.service_code=b.service_code
							and b.service_type='0';
						if(SQLCODE!=OK)
						{
							sprintf(grpUserMsg->return_message, "��¼wBaseFavChg�����![%d]",SQLCODE);
							pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
							EXEC SQL CLOSE srv_cur;
							return 191124;
						}
			#endif
			/*add by lixiaoxin at 20130425 end*/
			EXEC SQL INSERT INTO dGrpSrvMsg
				(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
				SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
				MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
				OP_TIME,   USE_FLAG, UNVAILD_TIME, PERSON_FLAG)
			VALUES
				(:vGrp_Id,    :vSrvTypeTmp, :vDetailCode, :vPriceCodeTmp,
				 '0', :vProduct_Code, sysdate, To_Date(:vSrv_Stop, 'YYYYMMDD'),
				 '0', :lLoginAccept, :vLogin_No,      :vOp_Code,
				sysdate,'Y', To_Date(:vSrv_Stop, 'YYYYMMDD'),'N');
			if(SQLCODE!=OK)
			{
				sprintf(grpUserMsg->return_message, "��¼���Ų�Ʒ�Żݱ����!![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubCreateGrpUser1", "", "%s:%s-- [%d][%s]",__FILE__,grpUserMsg->return_message,SQLCODE,SQLERRMSG);
				EXEC SQL CLOSE svr_cur;
				return 190035;
			}
		}		    	
		EXEC SQL CLOSE svr_cur;
	}

	/* Ĭ�϶�����ϵ boss2.0 add 2006-11-16 lixg*/
	Trim(vOp_Time);
	printf("322222222222231	=[%s]\n",vOp_Time);
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "pubBillBeginMode begin...");
	strcpy(modeMen,DEFAULT_GRPTOPERSON_CODE);
	strcpy(sendFalg,"0");
	retCode = pubBillBeginMode(vGrp_UserNo,modeMen,vOp_Time, vOp_Code,
		vLogin_No,strLoginAccept,vOp_Time,sendFalg,"","", grpUserMsg->return_message);

	if(0 != retCode)
	{
#ifdef _DEBUG
	pubLog_Debug(_FFL_, "fpubCreateGrpUser", "", "%s:[%s]", __FUNCTION__, grpUserMsg->return_message);
#endif
		return 190033;
	}

	return 0;
}

/************************************************************************
��������:fpubDelGrpUser1
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û�ɾ�����ú���
�������:�����û�id
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע
         ������Ϣ
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubDelGrpUser1(char *inGrpId_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{
	int  retCode = 0;

	EXEC SQL BEGIN DECLARE SECTION;
		int  vCnt = 0;
		long lLoginAccept = 0;
		char vGrpCust_Id[22+1];
		char vGrpId_No[22+1];
		char vContract_No[14+1];
		char vLogin_No[6+1];
		char vOp_Code[4+1];
		char vTotal_Date[8+1];
		char vOp_Note[60+1];
		char vProduct_Code[8+1];
		char vGrpProduct_Code[2+1];
		char vMember_Id[14+1];
		char vRun_Time[17+1];        /*add by miaoyl at 20090629 ����ʱ��*/
		int   ret=0;                   /*add by miaoyl at 20090708  */
		char  vOrg_Id[10+1];         /*add by miaoyl at 20090708  */
		/*NG��ź*/
		int	v_ret=0;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char	sTempSqlStr[1024+1];
		char	IdNo1[22+1];
		char	MemberId1[22+1];
		char	BeginTime1[20+1];
		char	EndTime1[20+1];
		TdGrpUserMebMsgIndex oldIndex1;
		TdGrpUserMebMsgIndex newIndex1;
		char	vTotal_Date1[22+1];
	EXEC SQL END DECLARE SECTION;

	init(vGrpCust_Id      );
	init(vGrpId_No        );
	init(vContract_No     );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vOp_Note         );
	init(vProduct_Code    );
	init(vGrpProduct_Code );
	init(vRun_Time        );    /*add by miaoyl at 20090629 ����ʱ��*/
	init(vOrg_Id          );    /*add by miaoyl at 20090708  */
	init(vTotal_Date1);
	/*������ֵ*/
	lLoginAccept = lSysAccept;
	strcpy(vGrpId_No,   inGrpId_No);
	strcpy(vLogin_No,   login_no  );
	strcpy(vOp_Code,    op_code   );
	strcpy(vTotal_Date, total_date);
	strcpy(vOp_Note,    op_note   );
	Trim(vGrpId_No);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
	/*ȡ����ORG_ID*/
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
		sprintf(return_message,"��ѯorg_idʧ��![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190100;
	}
	Trim(vOrg_Id);
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "��ѯdGrpUserMsg����Ϣ");
	#endif
	EXEC SQL SELECT Cust_Id, Product_Code, Account_Id
		INTO :vGrpCust_Id, :vProduct_Code, :vContract_No
		FROM dGrpUserMsg
		WHERE ID_NO = To_Number(:vGrpId_No)
		AND Bill_Date > Last_Day(sysdate) + 1;
	if(SQLCODE!=OK)
	{
		sprintf(return_message,"��ѯdGrpUserMsg����Ϣ[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190101;
	}
	Trim(vProduct_Code);

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "��ѯ���ſͻ�ϵͳ�Ĳ�Ʒ����");
	#endif
	EXEC SQL SELECT SM_CODE INTO :vGrpProduct_Code
			FROM sSmProduct
			WHERE Product_Code = :vProduct_Code;
	if(SQLCODE!=OK)
	{
		sprintf(return_message,"��ѯsSmProduct���ſͻ�ϵͳ��Ʒ�������[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190102;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "������״̬����Ϊ��Ч");
	#endif
	EXEC SQL UPDATE dGrpOrderGoods
			SET Curr_Status = '2'
			WHERE Product_Id in
			(	SELECT Product_Id FROM dGrpProduct
				WHERE Cust_Id = To_Number(:vGrpCust_Id)
				AND Product_Code = :vGrpProduct_Code
			);
	if (SQLCODE != 0)
	{
		sprintf(return_message, "���¶�����״̬����![%d][%s][%s][%s]", SQLCODE, SQLERRMSG,vGrpProduct_Code,vGrpCust_Id);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190103;
	}

	/* LINA VIP2.0 2006_11_05 begin here*/
	/*ȡvip��֯��dgrpgroups��org_code*/

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
	/*
	**EXEC SQL BEGIN DECLARE SECTION;
	**	char sIn_Grp_OrgCode[10+1];
	**EXEC SQL END DECLARE SECTION;
	**init(vOrgId);*/
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

	/*��ѯ���ſͻ���Ϣ*/
	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "��ѯ���ſͻ���Ϣ");
	#endif
	/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL SELECT Org_Code
	**           INTO :sIn_Grp_OrgCode
	**           FROM dCustDocOrgAdd
	**          WHERE Cust_Id = To_Number(:vGrpCust_Id);
	**    if (SQLCODE != 0){
	**        sprintf(return_message,"��Org_Codeʧ��",SQLCODE);
	**        pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
	**        return 190106;
	**    }
	*/
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "������Ϣ��dGrpProductHis��");
	#endif

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
		**EXEC SQL INSERT INTO dGrpProductHis
		**         (CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
		**          PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
		**          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		**          UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
		**          UPDATE_CODE,  UPDATE_TYPE, ORG_CODE)
		**         SELECT
		**          CUST_ID,      UNIT_ID,     UNIT_CODE,   PRODUCT_ID,
		**          PRODUCT_CODE, CONTRACT_ID, CONTRACT_NO, ORDER_TIME,
		**          BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		**          :vLogin_No,   :lLoginAccept,   :vTotal_Date, sysdate,
		**          :vOp_Code,     'D',         :sIn_Grp_OrgCode
		**         FROM dGrpProduct
		**        WHERE Cust_Id = :vGrpCust_Id
		**          AND Product_Code = :vGrpProduct_Code;
		***/

		EXEC SQL INSERT INTO dGrpProductHis
		(CUST_ID,      UNIT_ID,     PRODUCT_ID,
		PRODUCT_CODE, CONTRACT_ID, ORDER_TIME,
		BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
		UPDATE_CODE,  UPDATE_TYPE,SAVE_FILE,CREATE_DATE,CREATE_LOGIN)
		SELECT
		CUST_ID,      UNIT_ID,     PRODUCT_ID,
		PRODUCT_CODE, CONTRACT_ID,  ORDER_TIME,
		BEGIN_TIME,   END_TIME,    INPUT_DATE,  NOTE,
		:vLogin_No,   :lLoginAccept,   :vTotal_Date, sysdate,
		:vOp_Code,     'D', SAVE_FILE ,CREATE_DATE,CREATE_LOGIN
		FROM dGrpProduct
		WHERE Cust_Id = :vGrpCust_Id
		AND Product_Code = :vGrpProduct_Code;

	if ((SQLCODE != 0) || (SQLROWS == 0))
	{
		sprintf(return_message, "����dGrpProductHis����![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190104;
	}
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/
	EXEC SQL UPDATE dGrpProduct
		SET Product_Status = '2'
		WHERE Cust_Id = :vGrpCust_Id
		AND Product_Code = :vGrpProduct_Code;
	if (SQLCODE != 0)
	{
	sprintf(return_message, "��dGrpProduct���¼��Ϊ��Чʧ��![%d][%s]", SQLCODE, SQLERRMSG);
	pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
	return 190105;
	}
	/* LINA VIP2.0 end here*/

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "������Ϣ��dGrpSrvMsgHis��");
	#endif
	EXEC SQL INSERT INTO dGrpSrvMsgHis
		(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		OP_TIME,   UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
		UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
		SELECT
		ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		OP_TIME,   :vLogin_No,   :lLoginAccept, :vTotal_Date,
		sysdate,   :vOp_Code,    'D'
		FROM dGrpSrvMsg
		WHERE ID_NO = To_Number(:vGrpId_No)
		AND End_Time > Last_Day(sysdate) + 1;
	if ((SQLCODE != 0) || (SQLROWS == 0))
	{
		sprintf(return_message, "����dGrpSrvMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190106;
	}
	EXEC SQL UPDATE dGrpSrvMsg
		SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
		WHERE ID_NO = To_Number(:vGrpId_No)
		AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0)
	{
		sprintf(return_message, "����dGrpSrvMsg����Ϣʧ��![%d][%s]", SQLCODE, SQLERRMSG);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190107;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "ɾ��dGrpMemberMsg������");
	#endif
	init(vMember_Id);
	Trim(vGrpCust_Id);
	EXEC SQL DECLARE member_cur CURSOR for /*��ѯ�����û��ĳ�Ա*/
		SELECT Member_Id
		FROM dGrpUserMebMsg
		WHERE Id_No = To_Number(:vGrpId_No)
		AND End_Time > Last_Day(sysdate) + 1;
	EXEC SQL OPEN member_cur;
	EXEC SQL FETCH member_cur INTO :vMember_Id;
	while(SQLCODE == 0)
	{

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "���ú������Լ����û���Ա�ĸ��Ѽƻ����б��vGrpId_No=[%s]\n",vGrpId_No);
		#endif
		retCode = fpubDisConUserMsg1(vMember_Id, vContract_No, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
		if (retCode != 0)
		{
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "����fpubDisConUserMsg1��������,Id_No[%s],Account_Id[%s],retCode[%d]", vMember_Id, vContract_No, retCode);
			EXEC SQL CLOSE member_cur;
			return 190109;
		}

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "��ѯdGrpUserMebMsg��[%s]������", vMember_Id);
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
		if (SQLCODE != 0)
		{
			EXEC SQL CLOSE member_cur;
			sprintf(return_message,"��ѯdGrpUserMebMsg��[%s]������[%d]", vMember_Id, SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
			return 190108;
		}

		if (vCnt < 2)
		{ /*�û�ֻ��һ�����ŵĳ�Աʱ��ɾ��*/
			/* LINA VIP2.0 2006_11_05 begin here*/
			#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "ɾ��dGrpMemberMsg��[%s]����", vMember_Id);
			#endif
			/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
			/*
			**EXEC SQL INSERT INTO dGrpMemberMsgHis
			**         (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,
			**          PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,CARD_CODE,
			**          ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			**          CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			**          GMEMBER_STATUS_CODE, UPDATE_LOGIN, UPDATE_ACCEPT,
			**          UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, ORG_CODE, GROUP_ID)
			**   SELECT CUST_ID, UNIT_ID, UNIT_CODE, ID_NO, ID_NO,
			**          PHONE_NO, NAME, USER_NAME, BELONG_CODE,BIG_FLAG,MALE_FLAG,SERVICE_NO,CARD_CODE,
			**          ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			**          CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			**          GMEMBER_STATUS_CODE, :vLogin_No, :lLoginAccept,
			**          :vTotal_Date,  sysdate, :vOp_Code, 'd', :sIn_Grp_OrgCode,group_id
			**     FROM dGrpMemberMsg
			**    WHERE Cust_Id = To_Number(:vGrpCust_Id)
			**      AND ID_NO = To_Number(:vMember_Id);
			***/

			EXEC SQL INSERT INTO dGrpMemberMsgHis
				(CUST_ID, UNIT_ID, ID_NO,
				PHONE_NO, USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,CARD_CODE,
				ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
				CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
				UPDATE_LOGIN, UPDATE_ACCEPT,
				UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, GROUP_ID,
				AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				correct_note,ORG_ID)
			SELECT CUST_ID, UNIT_ID, ID_NO,
				PHONE_NO, USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,CARD_CODE,
				ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
				CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
				:vLogin_No, :lLoginAccept,
				:vTotal_Date,  sysdate, :vOp_Code, 'd',GROUP_ID,
				AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
				GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
				OPEN_TIME,SEX,SHORT_PHONE,correct_date,
				correct_note,:vOrg_Id
			 FROM dGrpMemberMsg
			WHERE Cust_Id = To_Number(:vGrpCust_Id)
				AND ID_NO = To_Number(:vMember_Id);

			if (SQLCODE != 0 && SQLCODE != 1403)
			{
				EXEC SQL CLOSE member_cur;
				sprintf(return_message,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
				return 190109;
			}
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---END*/

			EXEC SQL delete FROM dGrpMemberMsg
					WHERE Cust_Id = To_Number(:vGrpCust_Id)
					AND ID_NO = To_Number(:vMember_Id);
			if (SQLCODE != 0 && SQLCODE != 1403)
			{
				EXEC SQL CLOSE member_cur;
				sprintf(return_message,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
				return 190110;
			}
		/* LINA VIP2.0 end here*/
		}

		init(vMember_Id);
		EXEC SQL FETCH member_cur INTO :vMember_Id;
	}
		EXEC SQL CLOSE member_cur;

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "ɾ��dGrpUserMsg����Ϣ");
	#endif
	/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
	EXEC SQL INSERT INTO dGrpUserMsgHis
		(CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN,
		UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE,
		UPDATE_TYPE, SM_CODE ,GROUP_ID)
	SELECT
		CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO,
		IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE,
		USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE,
		REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE,
		LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE,
		OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE,
		LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     :vLogin_No,
		:lLoginAccept, :vTotal_Date, sysdate,      :vOp_Code,
		'D',SM_CODE, GROUP_ID
		FROM dGrpUserMsg
		WHERE ID_NO = To_Number(:vGrpId_No)
		AND Bill_Date > Last_Day(sysdate) + 1;
	if(SQLCODE!=OK)
	{
		sprintf(return_message,"����dGrpUserMsgHis���Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190111;
	}
	/*ֻҪbill_date��������1�գ�����Ϊ���Ѿ������ļ����û�*/
	/*NG��ź
	**EXEC SQL UPDATE dGrpUserMsg
	**            SET old_run = run_code,
	**                run_code = 'I',
	**                bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
	**          WHERE id_no = To_Number(:vGrpId_No)
	**            AND Bill_Date > Last_Day(sysdate) + 1;
	**if(SQLCODE!=OK){
	**    sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
	**    return 190112;
	**}
	**NG��ź*/
	/*ng����  by magang at 20090812 begin*/
	pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateGrpUserMsg!\n");

	init(i_parameter_array);
	strcpy(i_parameter_array[0],vTotal_Date);
	strcpy(i_parameter_array[1],vGrpId_No);

	v_ret=0;
	v_ret=OrderUpdateGrpUserMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,vGrpId_No,
								"old_run = run_code,run_code = 'I',bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1",
								"AND Bill_Date > Last_Day(sysdate) + 1",i_parameter_array);

	if(0!=v_ret)
	{
		sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190112;
	}
	/*ng����  by magang at 20090812 end*/
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "���ú������Լ����û��ʻ��ĸ��Ѽƻ����б��");
	#endif
	retCode = fpubDisConUserMsg1(vGrpId_No, vContract_No, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
	if (retCode != 0)
	{
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "����fpubDisConUserMsg1��������,Id_No[%s],Account_Id[%s],retCode[%d]", vGrpId_No, vContract_No, retCode);
		return 190112;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "ɾ��dGrpUserMebMsg������");
	#endif
	EXEC SQL INSERT INTO dGrpUserMebMsgHis
			(ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
			BEGIN_TIME, END_TIME,
			UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
			UPDATE_CODE, UPDATE_TYPE)
		SELECT
			ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
			BEGIN_TIME, END_TIME,
			:vLogin_No, :lLoginAccept, :vTotal_Date, sysdate,
			:vOp_Code, 'd'
			FROM dGrpUserMebMsg
			WHERE Id_No = To_Number(:vGrpId_No)
			AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		 sprintf(return_message,"����dGrpUserMebMsgHis�����Ϸ����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
		return 190113;
	}
	/*yangzh add 2005-01-24 �����û��뿪����ʱ����attr_codeΪ���˿ͻ���־*/
	/*NG��ź
	**EXEC SQL UPDATE dcustmsg
	**   SET attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)
	** WHERE id_no in (select member_id from dGrpUserMebMsg
	**                  where id_no = To_Number(:vGrpId_No)
	**                    and End_Time > Last_Day(sysdate) + 1
	**                  );
	**if (SQLCODE != 0 && SQLCODE != 1403){
	**    sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
	**    return 190207;
	**}
	**NG��ź*/
	/*ng����  by magang at 20090812 begin*/
	memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
	sprintf(sTempSqlStr,"select to_char(id_no) from dcustmsg where id_no in (select member_id  from dGrpUserMebMsg where id_no = to_number(:v0) and End_Time > Last_Day(sysdate) + 1 )");
	EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
	EXEC SQL declare ngcursor0 cursor for sql_str;
	EXEC SQL open ngcursor0 using :vGrpId_No;
		for(;;)
		{
			init(IdNo1);
			EXEC SQL FETCH ngcursor0 into :IdNo1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("fpubDelGrpUser1");
				pubLog_DBErr(_FFL_,"","","��ȡid_noʧ�� [%s]--\n",vGrpId_No);
				EXEC SQL CLOSE ngcursor0;
				return 191207;
			}
			if(1403==SQLCODE) break;
			Trim(IdNo1);
	 	pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateCustMsg!\n");
		init(i_parameter_array);
			strcpy(i_parameter_array[0],IdNo1);
			v_ret=0;
			v_ret=OrderUpdateCustMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,IdNo1,"attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)","",i_parameter_array);

			if(0!=v_ret && 1!=v_ret)
			{
				sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
				EXEC SQL CLOSE ngcursor0;
			 	return 190207;
			}
		}
		EXEC SQL CLOSE ngcursor0;

	/*ng����  by magang at 20090804 end*/
	/*ng����
	**EXEC SQL UPDATE dGrpUserMebMsg
	**	SET end_time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
	**	WHERE Id_No = To_Number(:vGrpId_No)
	**	AND End_Time > Last_Day(sysdate) + 1;
	**if (SQLCODE != 0 && SQLCODE != 1403)
	**{
	**	sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
	**	pubLog_Debug(_FFL_, "fpubDelGrpUser", "", "%s", return_message);
	**	return 190114;
	**}
	**ng����*/
	/*ng����  by magang at 20090812 begin*/
		pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateGrpUserMebMsg!\n");
		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select to_char(member_id),to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time,'yyyymmdd hh24:mi:ss'),to_char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD'))+1,'yyyymmdd hh24:mi:ss')  from dgrpusermebmsg where id_no =:v0 AND End_Time > Last_Day(sysdate) + 1");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcur0 cursor for sql_str;
		EXEC SQL open ngcur0 using  :vTotal_Date,:vGrpId_No;
		for(;;)
		{
			init(MemberId1);
			init(BeginTime1);
			init(EndTime1);
			init(vTotal_Date1);
			EXEC SQL FETCH ngcur0 into :MemberId1,:BeginTime1,:EndTime1,:vTotal_Date1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("fpubDelGrpUser1");
				pubLog_DBErr(_FFL_,"","","��ȡmember_idʧ�� [%s]--\n",vGrpId_No);
				EXEC SQL CLOSE ngcur0;
				return 190114;
			}
		if(1403==SQLCODE) break;

		Trim(MemberId1);
		Trim(BeginTime1);
		Trim(EndTime1);
		Trim(vGrpId_No);
		Trim(vTotal_Date1);
		memset(&oldIndex1,0,sizeof(TdGrpUserMebMsgIndex));
		memset(&newIndex1,0,sizeof(TdGrpUserMebMsgIndex));
		strcpy(oldIndex1.sIdNo,vGrpId_No);
		strcpy(oldIndex1.sMemberId,MemberId1);
		strcpy(oldIndex1.sBeginTime,BeginTime1);
		strcpy(oldIndex1.sEndTime,EndTime1);

		strcpy(newIndex1.sIdNo,vGrpId_No);
		strcpy(newIndex1.sMemberId,MemberId1);
		strcpy(newIndex1.sBeginTime,BeginTime1);
		strcpy(newIndex1.sEndTime,vTotal_Date1);

		init(i_parameter_array);
		strcpy(i_parameter_array[0],vTotal_Date);
			strcpy(i_parameter_array[1],vGrpId_No);
			strcpy(i_parameter_array[2],MemberId1);
			strcpy(i_parameter_array[3],BeginTime1);
			strcpy(i_parameter_array[4],EndTime1);
			v_ret=0;
			v_ret=OrderUpdateGrpUserMebMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex1,newIndex1,
												"end_time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1",
												"",i_parameter_array);

			if(0!=v_ret && 1!=v_ret)
			{
				sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
				EXEC SQL CLOSE ngcur0;
				return 190114;
			}
		}
			EXEC SQL CLOSE ngcur0;
			/*ng����  by magang at 20090811 end*/

	#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "ɾ��dGrpUserMebProdMsg������");
	#endif
	EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
		(ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
		PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
		BEGIN_TIME, END_TIME,
		BAK_FIELD, UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
		UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
		SELECT
		ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
		PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
		BEGIN_TIME, END_TIME,
		BAK_FIELD, :vLogin_No, :lLoginAccept, :vTotal_Date,
		sysdate, :vOp_Code, 'd'
		FROM dGrpUserMebProdMsg
		WHERE ID_No = To_Number(:vGrpId_No)
		AND End_Time > Last_Day(sysdate) + 1;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(return_message,"����dGrpUserMebProdMsgHis�����Ϸ����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
			return 190115;
		}
		EXEC SQL UPDATE dGrpUserMebProdMsg
			SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
			WHERE ID_No = To_Number(:vGrpId_No)
			AND End_Time > Last_Day(sysdate) + 1;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(return_message,"ɾ��dGrpUserMebProdMsg�����Ϸ����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpUser1", "", "%s", return_message);
			return 190116;
		}

	return 0;
}

/************************************************************************
��������:fpubAddGrpMember
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û����ӳ�Ա���ú���
�������:�����û�id
         ��Ա�ֻ�����
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע
         ���󷵻���Ϣ(��Ϊ���ز���ʹ��)
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubAddGrpMember(char *inGrpId_No, char *phone_no, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int  vCnt = 0;
		int  mCount = 0;
		int  vProduct_Level = 0;
		char vGrpCust_Id[22+1];
		char vGrpId_No[22+1];
		char vPhone_No[22+1];
		char vCust_Id[10+1];
		char vId_No[14+1];
		char vSm_Code[2+1];
		char vUnit_Id[10+1];
		char vUnit_Code[10+1];
		char vGrp_Userno[15+1];
		char vProduct_Type[8+1];
		char vProduct_Code[8+1];
		char vService_Code[4+1];
		char vService_No[6+1];
		char vBill_Code[8+1];
		char vGroup_Type_Code[1+1];
		char vDepartment[36+1];
		char vLmtFee[14+1];
		char vShort_PhoneNo[11+1];

		long lLoginAccept = 0;
		char vLogin_No[6+1];
		char vOp_Code[4+1];
		char vTotal_Date[8+1];
		char vEnd_Date[8+1];
		char vOp_Note[60+1];
		int  ret;
		char vLimitFlag[1+1];
		char vRun_Time[17+1];        /*add by miaoyl at 20090629 ����ʱ��*/
		char vOrg_Id[10+1];          /*add by miaoyl at 20090708  */

		/*NG��ź*/
		int	v_ret=0;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		TdGrpUserMebMsg	tdGrpUserMebMsg;
		char	BeginTime1[20+1];
		char	IdNo2[22+1];
		char	sTempSqlStr[1024+1];
		int    icount=0;
	EXEC SQL END DECLARE SECTION;

	/*������ʼ��*/
	init(vGrpCust_Id      );
	init(vGrpId_No        );
	init(vPhone_No        );
	init(vId_No           );
	init(vCust_Id         );
	init(vSm_Code         );
	init(vUnit_Id         );
	init(vUnit_Code       );
	init(vGrp_Userno      );
	init(vProduct_Type    );
	init(vProduct_Code    );
	init(vService_Code    );
	init(vService_No      );
	init(vBill_Code       );
	init(vGroup_Type_Code );
	init(vDepartment      );
	init(vLmtFee          );
	init(vShort_PhoneNo   );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vEnd_Date        );
	init(vOp_Note         );
	init(vLimitFlag       );
	init(vRun_Time        );    /*add by miaoyl at 20090629 ����ʱ��*/
	init(vOrg_Id          );    /*add by miaoyl at 20090708  */


	/*������ֵ*/
	lLoginAccept = lSysAccept;
	strcpy(vGrpId_No,   inGrpId_No);
	strcpy(vPhone_No,   phone_no  );
	strcpy(vLogin_No,   login_no  );
	strcpy(vOp_Code,    op_code   );
	strcpy(vTotal_Date, total_date);
	strcpy(vOp_Note,    op_note   );

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
	/*ȡ����ORG_ID*/
	ret = 0;
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
	    sprintf(return_message,"��ѯorg_idʧ��![%d]",SQLCODE);
	    pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	    return 190100;
	}
	Trim(vOrg_Id);
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

	/*��ѯ�����û�����*/
	EXEC SQL SELECT Cust_Id, User_No, Product_Type, Product_Code, To_Char(Bill_Date, 'YYYYMMDD')
		INTO :vGrpCust_Id, :vGrp_Userno, :vProduct_Type, :vProduct_Code, :vEnd_Date
		FROM dGrpUserMsg
		WHERE ID_NO = To_Number(:vGrpId_No) AND Bill_Date > Last_Day(sysdate) + 1;
	if (SQLCODE !=0)
	{
		sprintf(return_message,"��ѯdGrpUserMsg����[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		return 190103;
	}
	Trim(vGrpCust_Id);
	Trim(vGrp_Userno);
	Trim(vProduct_Type);
	Trim(vProduct_Code);

	/*��ѯ�ֻ��ͻ�����*/
	Trim(vPhone_No);
	EXEC SQL SELECT id_no, cust_id, sm_code, to_char(bill_type)
	INTO :vId_No, :vCust_Id, :vSm_Code, :vBill_Code
	FROM dCustMsg WHERE phone_no = :vPhone_No;
	if (SQLCODE != 0) {
	sprintf(return_message,"��ѯdCustMsg�����쳣[%d]",SQLCODE);
	pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	return 190101;
	}
	Trim(vBill_Code);

	/* ���Ƴ�Ա��μ���ͬһ���� */
	mCount = 0;
	EXEC SQL SELECT COUNT(*) INTO :mCount
		FROM dGrpUserMebMsg
		WHERE id_no = to_number(:vGrpId_No) and member_id = to_number(:vId_No)
		and end_time > last_day(sysdate) + 1;
	if (mCount > 0)
	{
		sprintf(return_message, "�ֻ�����[%s]�Ѿ����뼯��[%s]!", vPhone_No, vGrpId_No);
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		return 190102;
	}

	EXEC SQL select limit_flag into :vLimitFlag
		from sProductMebLimit where product_code = :vProduct_Code;
	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		sprintf(return_message, "��ѯsProductMebLimit�����[%d]", SQLCODE);
		return 190131;
	}

		if (SQLCODE == 1403) {
		/* yangzh add 2005/04/14 �̶����벻�ܼ���ͬһƷ�Ƶļ��ų�Ա begin */
		EXEC SQL SELECT COUNT(*) INTO :mCount
				FROM dGrpUserMebMsg a, dGrpUserMsg b
				WHERE a.id_no = b.id_no and a.member_id = to_number(:vId_No)
				and b.product_type = :vProduct_Type and b.bill_date > last_day(sysdate) + 1
				and a.end_time > last_day(sysdate) + 1;
		if (SQLCODE != 0)
		{
			sprintf(return_message,"�жϹ̶������Ƿ��Ѿ�����ͬһƷ�Ʋ�Ʒ����[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
			return 190131;
		}
		if (mCount > 0)
		{
			sprintf(return_message, "������ͬ���Ʒ");
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
			return 190132;
		}
		/* yangzh add 2005/04/14 �̶����벻�ܼ���ͬһƷ�Ƶļ����û� end */
		}
		else
		{
			ret = fCheckProdMebLimit(vGrpId_No, vId_No, vProduct_Code, vLimitFlag, return_message);
			if (ret != 0)
			{
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190131;
			}
		}
	/*��ѯ���ſͻ�����*/
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
	/*
	**EXEC SQL SELECT to_char(unit_id), trim(unit_code), service_no
	**  INTO :vUnit_Id, :vUnit_Code, :vService_No
	**  FROM dCustDocOrgAdd
	**  WHERE cust_id = to_number(:vGrpCust_Id);
	***/
	EXEC SQL SELECT to_char(unit_id), trim(unit_id), service_no
		INTO :vUnit_Id, :vUnit_Code, :vService_No
		FROM dCustDocOrgAdd
		WHERE cust_id = to_number(:vGrpCust_Id);
	if (SQLCODE != 0)
	{
		sprintf(return_message,"��ѯdCustDocOrgAdd����[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		return 190104;
	}
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

	/*�ж��Ƿ�VPMN���ſͻ�*/
	if (0 == strcmp(vProduct_Type, VPMN_SM_CODE)) {
	strcpy(vGroup_Type_Code, "1");
	} else {
	strcpy(vGroup_Type_Code, "2");
	}

	/*��ѯdGrpMemberMsg��ǰ��Ա�Ƿ��Ѿ�����*/
	EXEC SQL SELECT COUNT(*) INTO :mCount
	           FROM dGrpMemberMsg
	          WHERE Cust_Id = To_Number(:vGrpCust_Id)
	            AND Id_No = To_Number(:vId_No);
	if (SQLCODE !=0){
	    sprintf(return_message,"��ѯdGrpMemberMsg��[%s]��Ա��������[%d]", vId_No, SQLCODE);
	    pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	    return 190105;
	}
	if (mCount == 0)
	{ /*���ſͻ���Ա��������*/
		/*���û���¼���浽dGrpMemberMsgHis��
		**big_flag --��ͻ���ʶ���ȸ��³ɲ���('0'�ǣ�'1'��)
		**unit_id,                                           vrecord.unit_id,
		**unit_code,                                         TO_CHAR(vrecord.unit_id),
		**id_no,                                             v_id_no,
		**big_flag,                                          '0',--��ͻ���ʶ���ȸ��³ɲ���('0'�ǣ�'1'��)
		**phone_no,                                          TRIM(vrecord.real_no),
		**NAME,                                              NVL(SUBSTRB(TRIM(vrecord.username),1,20),'δ֪'),
		**user_name,                                         NVL(SUBSTRB(TRIM(vrecord.username),1,20),'δ֪'),
		**male_flag,                                         '2',
		**belong_code,                                       '*',
		**service_no,                                        'work99',
		**card_code,                                         '99',
		**member_code,                                       'C1',
		**id_iccid,                                          SUBSTRB(vrecord.iccid,1,20),
		**dept,                                              SUBSTRB(vrecord.department,1,20),
		**ingrp_date,                                        TO_CHAR(vrecord.op_time,'yyyy-mm-dd'),--����ʱ��
		**service_access,                                    '*',
		**sm_code,                                           '*',
		**pay_code,                                          '99',--���ѷ�ʽ��'99'δ֪
		**bill_code,                                         '*',
		**pay_fee_flag,                                      '*',
		**payed_fee,                                         vrecord.lmtfee,
		**short_phoneno,                                     TO_CHAR(vrecord.short_no),
		**note,                                              TRIM(vrecord.note),
		**cust_id,                                           TO_NUMBER(vrecord.real_no),--�ͻ���ţ���д���ֻ���
		**load_date,                                         TO_CHAR(SYSDATE,'yyyy-mm-dd'),--�������
		**group_type_code,                                   '1',
		**gmember_status_code,                               '1',--������������Ϊvpmn,��Ա״̬Ϊ��Ч
		**pay_fee_flag, payed_fee,
		**dept, substr(department,1,20),
		*/
		/* LINA VIP2.0 2006_11_05 begin here*/
		/*ȡvip��֯��dgrpgroups��org_code*/

		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
		/*	**EXEC SQL BEGIN DECLARE SECTION;
			**char sIn_Grp_OrgCode[10+1];
			**char vOrgId[10+1];
		EXEC SQL END DECLARE SECTION;
		init(vOrgId);*/
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

		/*��ѯ���ſͻ���Ϣ*/
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "��ѯ���ſͻ���Ϣ");
		#endif
		/*��֯�������������ֶ�group_id��org_id  edit by liuweib at 19/02/2009*/
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
		/*
		**EXEC SQL SELECT Org_Code
		**           INTO :sIn_Grp_OrgCode
		**           FROM dCustDocOrgAdd
		**          WHERE Cust_Id = To_Number(:vGrpCust_Id)
		**            AND Unit_Id = To_Number(:vUnit_Id);
		**    if (SQLCODE != 0){
		**        sprintf(return_message,"��Org_Codeʧ��",SQLCODE);
		**        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		**        return 190106;
		**    }
		***/
		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

		    pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "LINA");

			/*��֯�������������ֶ�group_id��org_id  edit by zhengxg at 07/05/2009*/
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/

			/*ȡ����ʱ��*/
			Trim(vId_No);
			EXEC SQL select to_char(open_time,'yyyymmdd hh24:mi:ss') INTO :vRun_Time
						from dCustMsg
						where id_no =To_Number(:vId_No);
			if (SQLCODE != 0)
			{
				sprintf(return_message,"��ѯdCustMsg����ʱ��open_time�����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190124;
			}

			/*
			**EXEC SQL INSERT INTO dGrpMemberMsgHis
			**         (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,
			**          PHONE_NO, NAME, USER_NAME, BELONG_CODE,
			**          ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			**          CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			**          big_flag, male_flag,
			**          card_code, member_code,
			**          ingrp_date, service_access, sm_code,
			**          pay_code, bill_code, load_date,
			**          group_type_code, gmember_status_code,
			**          pay_fee_flag, payed_fee, short_phoneno,
			**          service_no, dept, note,
			**          UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
			**          UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, Org_Code, GROUP_ID)
			**   SELECT :vGrpCust_Id, :vUnit_Id, :vUnit_Code,:vId_No, :vId_No,
			**          :vPhone_No, cust_name, cust_name, region_code||district_code||town_code,
			**          Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
			**          CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
			**          '0', '2',
			**          '99', 'C1',
			**          To_Char(sysdate, 'yyyy-mm-dd'), '*', :vSm_Code,
			**          '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
			**          :vGroup_Type_Code, '1',
			**          '*', 0, '0',
			**          :vService_No, 'δ֪', :vOp_Note,
			**          :vLogin_No, :lLoginAccept, :vTotal_Date,
			**          sysdate, :vOp_Code, 'A', :sIn_Grp_OrgCode, GROUP_ID
			**     FROM dCustDoc
			**    WHERE Cust_Id = To_Number(:vCust_Id);
			**    */

				EXEC SQL INSERT INTO dGrpMemberMsgHis
					(CUST_ID, UNIT_ID, ID_NO,
					PHONE_NO, USER_NAME, BELONG_CODE,
					ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					big_flag,
					card_code, member_code,
					ingrp_date, service_access, sm_code,
					service_no, dept, note,
					UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
					UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, GROUP_ID,
					AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
					GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
					OPEN_TIME,SEX,SHORT_PHONE,correct_date,
					correct_note,ORG_ID)
				SELECT :vGrpCust_Id, :vUnit_Id, :vId_No,
					:vPhone_No, cust_name, region_code||district_code||town_code,
					Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
					CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
					'0',
					'99', 'C1',
					sysdate, '*', :vSm_Code,
					:vService_No, 'δ֪', :vOp_Note,
					:vLogin_No, :lLoginAccept, :vTotal_Date,
					sysdate, :vOp_Code, 'A', GROUP_ID,
					'','','','',
					'','','','',
					to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',
					'',:vOrg_Id
				 FROM dCustDoc
				WHERE Cust_Id = To_Number(:vCust_Id);

			if (SQLCODE != 0)
			{
				sprintf(return_message,"��ѯdCustDoc��,����dGrpMemberMsgHis��Ա���Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190106;
			}
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

			/*��֯�������������ֶ�group_id��org_id  edit by zhengxg at 07/05/2009*/

			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
			/*
			**EXEC SQL INSERT INTO dGrpMemberMsg
			**         (CUST_ID, UNIT_ID, UNIT_CODE, UNIT_ID_NO, ID_NO,
			**          PHONE_NO, NAME, USER_NAME, BELONG_CODE,
			**          ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			**          CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			**          big_flag, male_flag,
			**          card_code, member_code,
			**          ingrp_date, service_access, sm_code,
			**          pay_code, bill_code, load_date,
			**          group_type_code, gmember_status_code,
			**          pay_fee_flag, payed_fee, short_phoneno,
			**          service_no, dept, note, Org_Code, GROUP_ID)
			**   SELECT :vGrpCust_Id, :vUnit_Id, :vUnit_Code, :vId_No, :vId_No,
			**          :vPhone_No, cust_name, cust_name, region_code||district_code||town_code,
			**          Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
			**          CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
			**          '0', '2',
			**          '99', 'C1',
			**          To_Char(sysdate, 'yyyy-mm-dd'), '*', :vSm_Code,
			**          '99', :vBill_Code,  To_Char(sysdate, 'yyyy-mm-dd'),
			**          :vGroup_Type_Code, '1',
			**          '*', 0, '0',
			**          :vService_No, 'δ֪', :vOp_Note, :sIn_Grp_OrgCode, GROUP_ID
			**     FROM dCustDoc
			**    WHERE Cust_Id = To_Number(:vCust_Id);
	*/

			EXEC SQL INSERT INTO dGrpMemberMsg
					(CUST_ID, UNIT_ID, ID_NO,
					PHONE_NO, USER_NAME, BELONG_CODE,
					ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
					CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
					big_flag,
					card_code, member_code,
					ingrp_date, service_access, sm_code,
					service_no, dept, note, GROUP_ID,
					AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
					GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
					OPEN_TIME,SEX,SHORT_PHONE,correct_date,
					correct_note,ORG_ID)
				SELECT :vGrpCust_Id, :vUnit_Id, :vId_No,
						:vPhone_No, cust_name, region_code||district_code||town_code,
						Id_Type, Id_Iccid, CONTACT_ADDRESS, CONTACT_POST,
						CONTACT_PHONE, CONTACT_FAX, CONTACT_EMAILL, :vCust_Id,
						'0',
						'99', 'C1',
						sysdate, '*', :vSm_Code,
						:vService_No, 'δ֪', :vOp_Note, GROUP_ID,
						'','','','',
						'','','','',
						to_date(:vRun_Time,'YYYYMMDD HH24:MI:SS'),'2','','',
						'',:vOrg_Id
						FROM dCustDoc
						WHERE Cust_Id = To_Number(:vCust_Id);

			if (SQLCODE != 0)
			{
				sprintf(return_message,"��ѯdCustDoc��,����dGrpMemberMsg��Ա���Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190107;
			}
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

			/* LINA VIP2.0 end here*/
		}

		#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "�ж��Ƿ�VPMN���ų�Ա�����¼��ſͻ���Ա��VPMN������Ϣ");
		#endif

		EXEC SQL SELECT Count(*) INTO :vCnt
		FROM dVPMNUsrMsg
		WHERE Real_No like :vPhone_No||'%';
		if (SQLCODE != 0){
		sprintf(return_message,"��ѯdVPMNUsrMsg�����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		return 190108;
		}
		if (1 == vCnt)
		{ /*�������ų�Ա����VPMN���ų�Ա,���Եõ�VPMN������Ϣ*/

			EXEC SQL SELECT substrb(nvl(department,'δ֪'), 1, 20), To_Char(lmtfee), short_no
				INTO :vDepartment, :vLmtFee, :vShort_PhoneNo
				FROM dVPMNUsrMsg
				WHERE Real_No like :vPhone_No||'%';
			if (SQLCODE != 0){
			sprintf(return_message,"��ѯdVPMNUsrMsg���û���Ϣ�����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
			return 190109;
			}
			Trim(vDepartment);
			Trim(vLmtFee);
			Trim(vShort_PhoneNo);

			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---begin*/
			/*
			**EXEC SQL UPDATE dGrpMemberMsg
			**            SET dept = substrb(nvl(:vDepartment, 'δ֪' ), 1, 20),
			**                payed_fee = To_Number(:vLmtFee),
			**                short_phoneno = :vShort_PhoneNo
			**          WHERE Cust_Id = :vGrpCust_Id
			**            AND Phone_No = :vPhone_No;
			***/
			EXEC SQL UPDATE dGrpMemberMsg
			SET dept = substrb(nvl(:vDepartment, 'δ֪' ), 1, 20)
			WHERE Cust_Id = :vGrpCust_Id
			AND Phone_No = :vPhone_No;
			/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

			if (SQLCODE != 0)
			{
				sprintf(return_message,"����dGrpMemberMsg���û���Ϣ�����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190109;
			}
		}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "���뼯���û���Ա��");
	#endif
	EXEC SQL INSERT INTO dGrpUserMebMsghis
		( ID_NO,        MEMBER_ID,   MEMBER_NO,   BAK_FIELD,
		BEGIN_TIME,   END_TIME,
		UPDATE_LOGIN, UPDATE_DATE, UPDATE_TIME, UPDATE_ACCEPT,
		UPDATE_CODE,  UPDATE_TYPE)
		VALUES
		( :vGrpId_No,   :vId_No,     :vPhone_No,  :vPhone_No,
		sysdate,      To_Date(:vEnd_Date, 'YYYYMMDD'),
		:vLogin_No,   :vTotal_Date,sysdate,     :lLoginAccept,
		:vOp_Code, 'A');
	if (SQLCODE != 0)
	{
		sprintf(return_message,"����dGrpUserMebMsghis�����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
		return 190108;
	}
	/*NG��ź
	**EXEC SQL INSERT INTO dGrpUserMebMsg
	**     ( ID_NO,        MEMBER_ID,   MEMBER_NO,   BAK_FIELD,
	**       BEGIN_TIME,   END_TIME )
	**    VALUES
	**     ( :vGrpId_No,    :vId_No,     :vPhone_No, :vPhone_No,
	**       sysdate, To_Date(:vEnd_Date, 'YYYYMMDD') );
	**if (SQLCODE != 0){
	**    sprintf(return_message,"����dGrpUserMebMsg�����쳣[%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**    return 190109;
	**}
	NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertGrpUserMebMsg!\n");
		init(BeginTime1);
		EXEC SQL select 	to_char(sysdate,'YYYYMMDD HH24:MI:SS')
				into :BeginTime1
				from  dual;
		if (SQLCODE != 0)
		{
			sprintf(return_message,"��ȡʱ��ʧ��[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
			return 192109;
		}
		Trim(BeginTime1);
		Trim(vGrpId_No);
		Trim(vId_No);
		Trim(vPhone_No);
		Trim(vEnd_Date);
		memset(&tdGrpUserMebMsg,0,sizeof(TdGrpUserMebMsg));
		strcpy(tdGrpUserMebMsg.sIdNo,vGrpId_No);
		strcpy(tdGrpUserMebMsg.sMemberId,vId_No);
		strcpy(tdGrpUserMebMsg.sMemberNo,vPhone_No);
		strcpy(tdGrpUserMebMsg.sBeginTime,BeginTime1);
		strcpy(tdGrpUserMebMsg.sEndTime,vEnd_Date);
		strcpy(tdGrpUserMebMsg.sBakField,vPhone_No);
		v_ret=0;
			v_ret=OrderInsertGrpUserMebMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,tdGrpUserMebMsg);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dGrpUserMebMsg�����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				return 190109;
			}

	/*ng����  by magang at 20090812 end*/
	/*yangzh add 2005-01-24 �����û����뼯��ʱ����attr_codeΪ���ſͻ���־*/
	/*NG��ź
	**EXEC SQL UPDATE dcustmsg
	**   SET attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)
	** WHERE phone_no = :vPhone_No;
	**if (SQLCODE != 0){
	**    sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**    return 190109;
	**}
	NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		icount=0;
		sprintf(sTempSqlStr,"select to_char(id_no)  from dcustmsg where phone_no = :vPhone_No");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcursor3 cursor for sql_str;
		EXEC SQL open ngcursor3 using :vPhone_No;
		for(;;)
		{
			init(IdNo2);
			EXEC SQL FETCH ngcursor3 into :IdNo2;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("fpubAddGrpMember");
				pubLog_DBErr(_FFL_,"","","��ȡid_noʧ�� [%s]--\n",vPhone_No);
				EXEC SQL CLOSE ngcursor3;
				return 191207;
			}
			if(1403==SQLCODE) break;
			Trim(IdNo2);
			pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateCustMsg!\n");
			init(i_parameter_array);
			strcpy(i_parameter_array[0],IdNo2);
			v_ret=0;
			v_ret=OrderUpdateCustMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,IdNo2,"attr_code = SUBSTR (attr_code, 1, 4) || '1' || SUBSTR (attr_code, 6, 1)","",i_parameter_array);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
				EXEC SQL CLOSE ngcursor3;
				return 190109;
			}
			icount++;
		}
		EXEC SQL CLOSE ngcursor3;
		if(icount==0)
		{
			sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
			return 290109;
		}
 	/*ng����  by magang at 20090804 END*/
/*���ݲ�Ʒ��ѯ������Ϣ��һ����Ʒ���Զ�Ӧ������� chenxuan boss3
	**EXEC SQL DECLARE prod_cur CURSOR for
	**         select a.product_level, b.service_code
	**           from sProductCode a, sProductSrv b
	**          where a.product_code = b.product_code
	**            and a.product_code = :vProduct_Code
	**            and a.product_status = 'Y'
	**            and a.begin_time < sysdate
	**            and a.end_time > sysdate;
	**if(SQLCODE!=OK){
	**      sprintf(return_message, "��ѯ��Ʒ������Ӧ��ϵ����![%d]",SQLCODE);
	**      pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**      return 190021;
	**}
	**
	**EXEC SQL OPEN prod_cur;
	**if(SQLCODE!=OK){
	**      EXEC SQL CLOSE prod_cur;
	**      sprintf(return_message, "�򿪲�Ʒ������Ӧ��ϵ�α����![%d]",SQLCODE);
	**      pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**      return 190022;
	**}
	**
	**EXEC SQL FETCH prod_cur INTO :vProduct_Level, :vService_Code;
	**while(SQLCODE == 0)
	**{
	**    ���뼯���û���Ա��Ʒ������,GRP���
	**    EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
	**         ( ID_NO,         USER_NO,      MEMBER_ID,     MEMBER_NO,
	**           PRODUCT_TYPE,  PRODUCT_CODE, PRODUCT_LEVEL, SERVICE_CODE,
	**           BEGIN_TIME,    END_TIME,
	**           BAK_FIELD,     UPDATE_LOGIN, UPDATE_DATE,   UPDATE_TIME,
	**           UPDATE_ACCEPT, UPDATE_CODE,  UPDATE_TYPE)
	**        VALUES
	**         ( :vGrpId_No,      :vGrp_Userno,   :vId_No,       :vPhone_No,
	**           :vProduct_Type,  :vProduct_Code, :vProduct_Level, :vService_Code,
	**           sysdate,         To_Date(:vEnd_Date, 'YYYYMMDD'),
	**           '0',             :vLogin_No,     :vTotal_Date,    sysdate,
	**           :lLoginAccept, :vOp_Code, 'A');
	**    if (SQLCODE != 0){
	**        sprintf(return_message,"����dGrpUserMebProdMsgHis�����쳣[%d]",SQLCODE);
	**        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**        EXEC SQL CLOSE prod_cur;
	**        return 190111;
	**    }
	**    EXEC SQL INSERT INTO dGrpUserMebProdMsg
	**         ( ID_NO,         USER_NO,      MEMBER_ID,     MEMBER_NO,
	**           PRODUCT_TYPE,  PRODUCT_CODE, PRODUCT_LEVEL, SERVICE_CODE,
	**           BEGIN_TIME,    END_TIME,
	**           BAK_FIELD)
	**        VALUES
	**         ( :vGrpId_No,      :vGrp_Userno,  :vId_No,    :vPhone_No,
	**           :vProduct_Type,  :vProduct_Code, :vProduct_Level, :vService_Code,
	**           sysdate,         To_Date(:vEnd_Date, 'YYYYMMDD'),
	**           '0');
	**    if (SQLCODE != 0){
	**        sprintf(return_message,"����dGrpUserMebProdMsg�����쳣[%d]",SQLCODE);
	**        pubLog_Debug(_FFL_, "fpubAddGrpMember", "", "%s", return_message);
	**        EXEC SQL CLOSE prod_cur;
	**        return 190112;
	**    }
	**
	**    EXEC SQL FETCH prod_cur INTO :vProduct_Level, :vService_Code;
	**}
	**EXEC SQL CLOSE prod_cur;
*/
    return 0;
}

/************************************************************************
��������:fpubDelGrpMember
����ʱ��:2004/11/21
������Ա:yangzh
��������:�����û�ɾ����Ա���ú���
�������:�����û�id
         ��Ա�ֻ�����
         ����Ա����
         ��������
         ��������
         ������ˮ��
         ������ע
         ������Ϣ
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubDelGrpMember(char *inGrpId_No, char *phone_no, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
{
	int  retCode = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		int  vCnt = 0;
		int  vProduct_Level = 0;
		char vGrpCust_Id[22+1];
		char vGrpId_No[22+1];
		char vPhone_No[22+1];
		char vCust_Id[10+1];
		char vId_No[14+1];
		char vSm_Code[2+1];
		char vUnit_Id[10+1];
		char vUnit_Code[10+1];
		char vGrp_Userno[15+1];
		char vProduct_Type[8+1];
		char vProduct_Code[8+1];
		char vService_Code[4+1];
		char vAccount_Id[14+1];

		long lLoginAccept = 0;
		char vLogin_No[6+1];
		char vOp_Code[4+1];
		char vTotal_Date[8+1];
		char vOp_Note[60+1];
		int   ret=0;                   /*add by miaoyl at 20090708  */
		char  vOrg_Id[10+1];           /*add by miaoyl at 20090708  */

		/*NG��ź*/
		int	v_ret=0;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char	MemberId1[22+1];
		char	BeginTime1[20+1];
		char	EndTime1[20+1];
		TdGrpUserMebMsgIndex oldIndex1;
		TdGrpUserMebMsgIndex newIndex1;
		char	sTempSqlStr[1024+1];
		char	IdNo3[22+1];
		char	vTotal_Date1[22+1];
		int 	ncount=0;
	EXEC SQL END DECLARE SECTION;

	/*������ʼ��*/
	init(vGrpCust_Id      );
	init(vGrpId_No        );
	init(vPhone_No        );
	init(vId_No           );
	init(vCust_Id         );
	init(vSm_Code         );
	init(vUnit_Id         );
	init(vUnit_Code       );
	init(vGrp_Userno      );
	init(vProduct_Type    );
	init(vProduct_Code    );
	init(vService_Code    );
	init(vAccount_Id      );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vOp_Note         );
	init(vAccount_Id      );
	init(vOrg_Id          );  /*add by miaoyl at 20090708  */
	init(vTotal_Date1	  );
	/*������ֵ*/
	lLoginAccept = lSysAccept;
	strcpy(vGrpId_No,   inGrpId_No);
	strcpy(vPhone_No,   phone_no  );
	strcpy(vLogin_No,   login_no  );
	strcpy(vOp_Code,    op_code   );
	strcpy(vTotal_Date, total_date);
	strcpy(vOp_Note,    op_note   );
	Trim(vPhone_No);

	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---begin*/
	/*ȡ����ORG_ID*/
	ret = sGetLoginOrgid(vLogin_No,vOrg_Id);
	if(ret <0)
	{
		sprintf(return_message,"��ѯorg_idʧ��![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
		return 190200;
	}
	Trim(vOrg_Id);
	/*--- ��ͻ���ṹ���� edit by miaoyl at 20090708---end*/

	/*��ѯ�����û�����*/
	EXEC SQL SELECT Cust_Id, User_No, Account_Id, Product_Type, Product_Code
			INTO :vGrpCust_Id, :vGrp_Userno, :vAccount_Id, :vProduct_Type, :vProduct_Code
			FROM dGrpUserMsg
			WHERE ID_NO = To_Number(:vGrpId_No)
			AND Bill_Date > Last_Day(sysdate) + 1;
	if (SQLCODE !=0)
	{
		sprintf(return_message,"��ѯdGrpUserMsg����[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
		return 190201;
	}

	/*��ѯ�ֻ��ͻ�����*/
	EXEC SQL SELECT id_no, CUST_ID, sm_code
			INTO :vId_No, :vCust_Id, :vSm_Code
			FROM dCustMsg
			WHERE phone_no = :vPhone_No ;
	if (SQLCODE != 0)
	{
		sprintf(return_message,"��ѯdCustMsg�����쳣[%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
		return 190202;
	}

		/* ��֤�Ƿ��Ѽ��� */
	EXEC SQL select count(*) into :vCnt from dGrpUserMebMsg
		where id_no = :vGrpId_No and member_id = :vId_No and end_time > last_day(sysdate)+1;
	if (vCnt == 0)
	{
		sprintf(return_message, "�ֻ�����[%s]δ���뼯��[%s]!", vPhone_No, vGrpId_No);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
		return 190203;;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "��ѯdGrpUserMebMsg��");
	#endif
	EXEC SQL SELECT COUNT(*) INTO :vCnt
			   FROM dGrpUserMebMsg
			  WHERE Member_Id = To_Number(:vId_No)
				AND Id_No In
						(SELECT Id_No FROM dGrpUserMsg
						  WHERE Cust_Id = To_Number(:vGrpCust_Id)
		  					AND Bill_Date > Last_Day(sysdate) + 1)
				AND End_Time > Last_Day(sysdate) + 1;
	if (SQLCODE != 0)
	{
		sprintf(return_message,"��ѯdGrpUserMebMsg��[%s]������[%d]", vId_No, SQLCODE);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
		return 190204;
	}

	if (vCnt < 2) 
	{ /*�û�ֻ��һ�������û��ĳ�Աʱ��ɾ��*/
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "LINA++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

		EXEC SQL INSERT INTO dGrpMemberMsgHis
			(CUST_ID, UNIT_ID,ID_NO,
			PHONE_NO, USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
			ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			UPDATE_LOGIN, UPDATE_ACCEPT,
			UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE, GROUP_ID,
			AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			correct_note,ORG_ID)
		SELECT CUST_ID, UNIT_ID, ID_NO,
			PHONE_NO,USER_NAME, BELONG_CODE,BIG_FLAG,SERVICE_NO,MEMBER_CODE,CARD_CODE,
			ID_TYPE, ID_ICCID, CONTACT_ADDR, OWNER_ZIP,
			CONTACT_PHONE, FAX, EMAIL, MEMBER_CUST_ID,
			:vLogin_No, :lLoginAccept,
			:vTotal_Date,  sysdate, :vOp_Code, 'd', GROUP_ID,
			AGE,BIG_PHOTO,CONSUME_HABIT,GRADE_CODE,
			GRP_PHOTO,GRP_TYPE,LOVE_REDOUND,LOVE_SERVICE,
			OPEN_TIME,SEX,SHORT_PHONE,correct_date,
			correct_note,:vOrg_Id
		FROM dGrpMemberMsg
		WHERE Cust_Id = To_Number(:vGrpCust_Id)
			AND ID_NO = To_Number(:vId_No);

		/*--- ��ͻ���ṹ���� edit by miaoyl at 20090629---end*/

		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(return_message,"����dGrpMemberMsgHis�����Ϸ����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
			return 190206;
		}

		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vLogin_No = [%s]",vLogin_No);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vTotal_Date = [%s]",vTotal_Date);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":lLoginAccept= [%d]",lLoginAccept);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vOp_Code= [%s]",vOp_Code);
		pubLog_Debug(_FFL_, "fpubDelGrpMember", "", ":vPhone_No= [%s]",vPhone_No);

		EXEC SQL delete FROM dGrpMemberMsg
			WHERE Cust_Id = To_Number(:vGrpCust_Id)
			AND ID_NO = To_Number(:vId_No);
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			sprintf(return_message,"ɾ��dGrpMemberMsg�����Ϸ����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
			return 190207;
		}

		ncount=0;
		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select to_char(id_no)  from dcustmsg where phone_no = :vPhone_No");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcursor4 cursor for sql_str;
		EXEC SQL open ngcursor4 using :vPhone_No;
		for(;;)
		{
			init(IdNo3);
			EXEC SQL FETCH ngcursor4 into :IdNo3;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("fpubDelGrpMember");
				pubLog_DBErr(_FFL_,"","","��ȡid_noʧ�� [%s]--\n",vPhone_No);
				EXEC SQL CLOSE ngcursor4;
				return 191207;
			}
			if(1403==SQLCODE) break;
			Trim(IdNo3);
		 	pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateCustMsg!\n");
			init(i_parameter_array);
			strcpy(i_parameter_array[0],IdNo3);
			v_ret=0;
			v_ret=OrderUpdateCustMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,IdNo3,"attr_code = SUBSTR (attr_code, 1, 4) || '0' || SUBSTR (attr_code, 6, 1)","",i_parameter_array);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
				EXEC SQL CLOSE ngcursor4;
				return 190208;
			}
			ncount++;
		}
		EXEC SQL CLOSE ngcursor4;
		if(ncount==0)
		{
			sprintf(return_message,"����dCustMsg��attr_code����![%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
			return 190208;
		}
		/*ng����  by magang at 20090804 END*/
	}

#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "���ú������Լ����û���Ա�ĸ��Ѽƻ����б��");
#endif

	/* ��Աɾ��ǰ�ж��и��Ѽƻ������в�����ɾ���������������Ѽƻ����
	**EXEC SQL select count(*) into :vCnt from dConUserMsg
	**  where id_no = :vId_No and contract_no = :vAccount_Id and serial_no = 0;
	**if (SQLCODE != 0) {
	**  sprintf(return_message, "��ѯ��Ա���Ѽƻ�����[%d]", SQLCODE);
	**  pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
	**  return 190109;
	**}
	**
	**if (vCnt > 0) {
	**  sprintf(return_message,"��Ա[%s]�и��Ѽƻ�������ɾ��������ִ�и��Ѽƻ������", vPhone_No);
	**  pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
	**  return 190110;
	**}
	**boss3.0 */

	/* ����������������ƣ�����ִ��������룬�������������Ա��Ժ��޸ķ���
	**retCode = fpubDisConUserMsg(vId_No, vAccount_Id, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
	**if (retCode != 0) {
	**  pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "����fpubDisConUserMsg��������,Id_No[%s],Account_Id[%s],retCode[%d]", vId_No, vAccount_Id, retCode);
	**  return 190111;
	**}
	boss3.0 */

	/*����dGrpUserMebMsg*/
	EXEC SQL INSERT INTO dGrpUserMebMsgHis
		(ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
		BEGIN_TIME, END_TIME,
		UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,
		UPDATE_CODE, UPDATE_TYPE)
	SELECT
		ID_NO, MEMBER_NO, MEMBER_ID, BAK_FIELD,
		BEGIN_TIME, END_TIME,
		:vLogin_No, :lLoginAccept, :vTotal_Date, sysdate,
		:vOp_Code, 'd'
	FROM dGrpUserMebMsg
	WHERE Id_No = To_Number(:vGrpId_No)
		AND Member_no = :vPhone_No
		AND End_Time > Last_Day(sysdate) + 1;
		if (SQLCODE != 0)
		{
			sprintf(return_message,"����dGrpUserMebMsgHis�����Ϸ����쳣[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
			return 190212;
		}
	/*NG��ź
	**EXEC SQL UPDATE dGrpUserMebMsg
	**            SET End_Time = Last_Day(To_Date(:vTotal_Date,'YYYYMMDD')) + 1
	**           WHERE Id_No = To_Number(:vGrpId_No)
	**             AND Member_no = :vPhone_No
	**             AND End_Time > Last_Day(sysdate) + 1;
	**if (SQLCODE != 0 || SQLROWS != 1){
	**    sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
	**    return 190213;
	**}
	NG��ź*/
	/*ng����  by magang at 20090812 begin*/
			pubLog_Debug(_FFL_,"", "", "begin call OrderUpdateGrpUserMebMsg!\n");

	EXEC SQL select to_char(member_id),to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time ,'yyyymmdd hh24:mi:ss'),to_char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD'))+1,'yyyymmdd hh24:mi:ss')
			into :MemberId1,:BeginTime1,:EndTime1,:vTotal_Date1
			from dGrpUserMebMsg
			where Id_No = To_Number(:vGrpId_No)
				AND Member_no = :vPhone_No
				AND End_Time > Last_Day(sysdate) + 1;

				if (SQLCODE != 0 || SQLROWS != 1)
				{
					sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
					pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
				 	return 190213;
				}

			Trim(vGrpId_No);
			Trim(MemberId1);
			Trim(BeginTime1);
			Trim(EndTime1);
			Trim(vTotal_Date1);
			Trim(vPhone_No);

			memset(&oldIndex1,0,sizeof(TdGrpUserMebMsgIndex));
			memset(&newIndex1,0,sizeof(TdGrpUserMebMsgIndex));
			strcpy(oldIndex1.sIdNo,vGrpId_No);
			strcpy(oldIndex1.sMemberId,MemberId1);
			strcpy(oldIndex1.sBeginTime,BeginTime1);
			strcpy(oldIndex1.sEndTime,EndTime1);

			strcpy(newIndex1.sIdNo,vGrpId_No);
			strcpy(newIndex1.sMemberId,MemberId1);
			strcpy(newIndex1.sBeginTime,BeginTime1);
			strcpy(newIndex1.sEndTime,vTotal_Date1);

			init(i_parameter_array);
			strcpy(i_parameter_array[0],vTotal_Date);
			strcpy(i_parameter_array[1],vGrpId_No);
			strcpy(i_parameter_array[2],MemberId1);
			strcpy(i_parameter_array[3],BeginTime1);
			strcpy(i_parameter_array[4],EndTime1);
			strcpy(i_parameter_array[5],vPhone_No);

			v_ret=0;
			v_ret=OrderUpdateGrpUserMebMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex1,newIndex1,
												"End_Time = Last_Day(To_Date(:vTotal_Date,'YYYYMMDD')) + 1",
												"AND Member_no = :vPhone_No ",i_parameter_array);

			if(0!=v_ret)
			{
				sprintf(return_message,"����dGrpUserMebMsg�����Ϸ����쳣[%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
				return 190213;
			}

	/*ng����  by magang at 20090811 end*/


/* ���ٴ����ѷϳ���dGrpUserMebProdMsg chenxuan boss3
    **EXEC SQL INSERT INTO dGrpUserMebProdMsgHis
    **           (ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
    **            PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
    **            BEGIN_TIME, END_TIME,
    **            BAK_FIELD, UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE,
    **            UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
    **         SELECT
    **            ID_NO,  USER_NO, MEMBER_ID, MEMBER_NO,
    **            PRODUCT_TYPE, PRODUCT_CODE, SERVICE_CODE, PRODUCT_LEVEL,
    **            BEGIN_TIME, END_TIME,
    **            BAK_FIELD, :vLogin_No, :lLoginAccept, :vTotal_Date,
    **            sysdate, :vOp_Code, 'd'
    **           FROM dGrpUserMebProdMsg
    **          WHERE ID_No = To_Number(:vGrpId_No)
    **            AND MEMBER_ID = To_Number(:vId_No);
    **if (SQLCODE != 0){
    **    sprintf(return_message,"����dGrpUserMebProdMsgHis�����Ϸ����쳣[%d]",SQLCODE);
    **    pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
    **    return 190209;
    **}
    **EXEC SQL UPDATE dGrpUserMebProdMsg
    **            SET End_Time = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
    **          WHERE ID_No = To_Number(:vGrpId_No)
    **            AND MEMBER_ID = To_Number(:vId_No)
    **            AND End_Time > Last_Day(sysdate) + 1;
    **if (SQLCODE != 0){
    **    sprintf(return_message,"ɾ��dGrpUserMebProdMsg�����Ϸ����쳣[%d]",SQLCODE);
    **    pubLog_Debug(_FFL_, "fpubDelGrpMember", "", "%s", return_message);
    **    return 190210;
    **}
*/
    return 0;
}

/************************************************************************
��������:fpubDisConUserMsg1
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
int fpubDisConUserMsg1(char *inId_No, char *inContract_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
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
		char vOp_Note[60+1];
		int  vBillOrder=0;
		/*NG��ź*/
		TdConUserMsgIndex oldIndex;
		TdConUserMsgIndex newIndex;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
			int 		v_ret=0;
	EXEC SQL END DECLARE SECTION;

	init(vId_No           );
	init(vContract_No     );
	init(vLogin_No        );
	init(vOp_Code         );
	init(vTotal_Date      );
	init(vOp_Note         );
	Trim(inId_No          );
	Trim(inContract_No    );

	/*������ֵ*/
	lLoginAccept = lSysAccept;
	strcpy(vId_No,       inId_No       );
	strcpy(vContract_No, inContract_No );
	strcpy(vLogin_No,    login_no      );
	strcpy(vOp_Code,     op_code       );
	strcpy(vTotal_Date,  total_date    );
	strcpy(vOp_Note,     op_note       );

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "insert into dConUserMsgHis");
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
	if (SQLCODE != 0)
	{
		sprintf(return_message, "����dConUserMsgHis����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
		return 191001;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "��ѯ����SerialNo");
	#endif
	EXEC SQL SELECT Nvl(Max(Serial_No), 0)
		INTO :lMaxSerialNo
		FROM dConUserMsg
		WHERE Id_No = To_Number(:vId_No)
		AND Contract_No = To_Number(:vContract_No);
	if (SQLCODE != 0)
	{
		sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
		return 191002;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "��ѯdCustMsg�Ƿ��Ǹ����û�");
	#endif
	EXEC SQL SELECT Count(*) INTO :vcnt FROM dCustMsg
		WHERE Id_No = To_Number(:vId_No);
	if (SQLCODE != 0)
	{
		sprintf(return_message, "��ѯdCustMsg�Ƿ��Ǹ����û�ʧ��![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
		return 191002;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "update dConUserMsg");
	#endif
/**
	**EXEC SQL UPDATE dConUserMsg
	**            SET Serial_No = :lMaxSerialNo + 1,
	**                End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD')
	**          WHERE Id_No = To_Number(:vId_No)
	**            AND Contract_No = To_Number(:vContract_No)
	**            AND Serial_No = 0
	**            AND END_YMD > To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD');
	**/


	EXEC SQL select Bill_Order
					into :vBillOrder
				from dConUserMsg
				WHERE Id_No = To_Number(:vId_No)
				AND Contract_No = To_Number(:vContract_No)
				AND Serial_No = 0;

	if (SQLCODE != 0 && SQLCODE != 1403)
	{
		sprintf(return_message, "vBillOrder from dConUserMsg����![%d]",SQLCODE);
		pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
		return 191003;
	}
	/*NG��ź
	**EXEC SQL UPDATE dConUserMsg
	**            SET End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),
	**            	End_TM='000000'
	**          WHERE Id_No = To_Number(:vId_No)
	**            AND Contract_No = To_Number(:vContract_No)
	**            AND Serial_No = 0;
	**
	**if (SQLCODE != 0 && SQLCODE != 1403) {
	**    sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
	**    pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
	**    return 191003;
	**}
	NG��ź*/
	/*ng����  by magang at 20090812 begin*/
			pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "begin call OrderUpdateConUserMsg!-[%s]-[%s]\n",vId_No,vContract_No);
			memset(&oldIndex,0,sizeof(TdConUserMsgIndex));
			memset(&newIndex,0,sizeof(TdConUserMsgIndex));
			strcpy(oldIndex.sIdNo,vId_No);
			strcpy(oldIndex.sContractNo,vContract_No);
			strcpy(oldIndex.sSerialNo,"0");
			strcpy(newIndex.sIdNo,vId_No);
			strcpy(newIndex.sContractNo,vContract_No);
			strcpy(newIndex.sSerialNo,"0");

			init(i_parameter_array);
			strcpy(i_parameter_array[0],vTotal_Date);
			strcpy(i_parameter_array[1],vId_No);
			strcpy(i_parameter_array[2],vContract_No);
			strcpy(i_parameter_array[3],"0");
			v_ret=0;
			v_ret=OrderUpdateConUserMsg("2",vId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex,newIndex,
										"End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),End_TM='000000'",
										"",i_parameter_array);

			if(0!=v_ret&&1!=v_ret)
			{
				sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
				pubLog_Debug(_FFL_, "fpubDisConUserMsg1", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
				return 191003;
			}
		/*ng����  by magang at 20090811 END*/


		/* yangzh disabled 20050124 ����dCustMsg�����,�Ͳ�����wConUserChg*/
		/*if ((SQLROWS == 1) && (vcnt == 1)) { */
		/*����û����Ѽƻ������ڣ��������Update���ִ�лᷢ������
		  �������û��ĸ��Ѽƻ���û�з����������������wConUserChg��¼
		  OP_TYPE��10-���ӣ�20-�޸ģ�30-ɾ��
		 */
		/*#ifdef _DEBUG_
		    pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "insert into wConUserChg");
		#endif*/
		/***EXEC SQL INSERT INTO wConUserChg
		**            ( Op_No,       Op_Type,    Id_No,
		**              Contract_No, Bill_Order, Conuser_Update_Time,
		**              Limit_Pay,   Rate_Flag,  Fee_Code,  DETAIL_CODE,
		**              Fee_Rate,    Op_Time
		**            )
		**          VALUES
		**            ( sMaxBillChg.NextVal,     '20',     To_Number(:vId_No),
		**              To_Number(:vContract_No), :vBillOrder, To_Char(sysdate,'YYYYMMDDHH24MISS'),
		**              0,          '0',      '*', '*',
		**              0,    To_Char(sysdate,'YYYYMMDDHH24MISS')
		**             );
		**if (SQLCODE != 0) {
		**    sprintf(return_message, "����wConUserChg����![%d]",SQLCODE);
		**    pubLog_Debug(_FFL_, "fpubDisConUserMsg", "", "%s:%s-- [%d][%s]",__FILE__,return_message,SQLCODE,SQLERRMSG);
		**    return 191004;
		**}
		}*/

	return 0;
}

int fCheckProdMebLimit(char *iGrpId, char *iMebId, char *iProductCode, char *iLimitFlag, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int vCount = 0;
	EXEC SQL END DECLARE SECTION;

	/*  */
	if (iLimitFlag[0] == '1')
	{
		EXEC SQL select count(*) into :vCount
		from dGrpUserMebMsg a, dGrpUserMsg b
		where a.id_no = b.id_no and a.member_id = to_number(:iMebId)
		and b.product_code = :iProductCode and b.bill_date > last_day(sysdate) + 1
		and a.end_time > last_day(sysdate) + 1;
		if (SQLCODE != 0)
		{
			sprintf(retMsg, "��ѯ���Ų�Ʒ�����[%d]", SQLCODE);
			return -1;
		}
		if (vCount > 0)
		{
			sprintf(retMsg, "��Ա[%s]�Ѽ���ͬ�༯��", iMebId);
			return -1;
		}
	}

	return 0;
}

int mogetGrpFeeList(char *comm_fee,tGrpUserMsg *grpUserMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char	service_code[4+1];
	char	price_code[4+1];
	char	charge_flag[1+1];
	float	price_fee = 0;
	char	feeCode[2+1];
	char	detailCode[2+1];
	int		i = 0;
	int		iFeeCount = 0;
	float	vyShouldFee = 0;   /* ӪҵӦ�� */
	float	vyRealFee = 0;     /* Ӫҵʵ�� */
	float	vzShouldFee = 0;   /* ����Ӧ�� */
	float	vzRealFee = 0;     /* ����ʵ�� */

	EXEC SQL END DECLARE SECTION;

	memset(service_code, 0, sizeof(service_code));
	memset(price_code, 0, sizeof(price_code));
	memset(charge_flag, 0, sizeof(charge_flag));
	memset(feeCode, 0, sizeof(feeCode));
	memset(detailCode, 0, sizeof(detailCode));
	price_fee = 0;

	/* service_type = 5 ��ʾһ���Է��� */
	EXEC SQL declare mycur01 cursor for
		SELECT a.service_code,a.price_code,b.price_rate*b.price_fee, nvl(b.fee_code, '00'), nvl(b.detail_code, '00')
			from sSrvPrice a, spriceCode b, sSrvCode c,sproductsrv d
			where  d.product_code=:grpUserMsg->product_code
			and d.service_code=c.service_code
			and a.price_code = b.price_code
			and a.service_code = c.service_code
			and c.service_type = '5';
	EXEC SQL open mycur01;
	for(;;)
	{
		memset(feeCode, 0, sizeof(feeCode));
		memset(detailCode, 0, sizeof(detailCode));
		price_fee = 0;
		EXEC SQL FETCH mycur01 INTO :service_code,:price_code,:price_fee, :feeCode, :detailCode;
		if(SQLCODE != 0)
		{
			break;
		}
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			EXEC SQL CLOSE mycur01;
			pubLog_Debug(_FFL_, "mogetGrpFeeList", "", "ȡһ���Է��ó���");
			return -1;
		}
		iFeeCount++;
		strcpy(grpUserMsg->feeList.service_code[i],service_code);
		strcpy(grpUserMsg->feeList.price_code[i],price_code);
		strcpy(grpUserMsg->feeList.charge_flag[i], "2");
		sprintf(grpUserMsg->feeList.price_fee[i],"0");
		strcpy(grpUserMsg->feeList.fee_code[i], feeCode);
		strcpy(grpUserMsg->feeList.detail_code[i], detailCode);
		sprintf(grpUserMsg->feeList.action_fee[i],"%.2f",price_fee);

		vzShouldFee = vzShouldFee + price_fee;
		vzRealFee = vzRealFee + atof(grpUserMsg->feeList.action_fee[i]);
	}
	EXEC SQL CLOSE mycur01;

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "mogetGrpFeeList", "", "feeList = [%.2f] [%.2f] [%.2f] [%.2f]", vyShouldFee, vyRealFee, vzShouldFee, vzRealFee);
	#endif

	vyShouldFee = vyShouldFee + 0;
	vyRealFee = vyRealFee + atof(comm_fee);

	sprintf(grpUserMsg->feeList.vyShouldFee, "%.2f", vyShouldFee);
	sprintf(grpUserMsg->feeList.vyRealFee, "%.2f", vyRealFee);
	sprintf(grpUserMsg->feeList.vzShouldFee, "%.2f", vzShouldFee);
	sprintf(grpUserMsg->feeList.vzRealFee, "%.2f", vzRealFee);
	sprintf(grpUserMsg->feeList.iFeeCount, "%d", iFeeCount);

	return 0;
}

/**************
function name :fpubMotiveOrder
function desc :����100ҵ��������͹�������
**************/
int fpubMotiveOrder(char *login_accept,char *login_no,char *vmotiveProdStr,char *vmotiveProdSrvStr,char *vmotiveProdSmCode,char *opNote,char *vmotiveSrvCode,tGrpUserMsg *grpUserMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char	motiveCode[8+1];
	char	loginNo[6+1];
	char	motiveProdStr[128+1];
	char	motiveProdSrvStr[64+1];
	char	motiveProdSmCode[64+1];
	char	mosonProduct[10+1];
	char	moprodSrv[4+1];
	char	moprodSmcode[2+1];
	char	orderSeq[17+1];
	long	custId=0;
	long	idNo=0;
	long	sonId=0;
	long	loginAccept=0;
	char	orderNote[1024+1];
	char	opTime[17+1];
	char	tempStr[3+1];
	int		i=0,j=0,icount=0;
	char	orderBefore[1+1];
	char	orderOwe[1+1];
	char	modeCode[8+1];
	char	motiveSrvCode[4+1];

	/*NG��ź*/
	int	v_ret=0;
	TdMotiveUserMsg	sTdMotiveUserMsg;
	char	sTempSqlStr[1024+1];
	char	MotiveName1[1024+1];
	char	AccountId1[22+1];
	char	lloginAccept[20+1];
	char	id_no1[22+1];
	char	account_id1[22+1];
	char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int	    vcount=0;
	EXEC SQL END DECLARE SECTION;

	init(motiveCode);
	init(loginNo);
	init(motiveProdStr);
	init(motiveProdSrvStr);
	init(motiveProdSmCode);
	init(mosonProduct);
	init(moprodSrv);
	init(moprodSmcode);
	init(orderSeq);
	init(opTime);
	init(tempStr);
	init(orderNote);
	init(orderBefore);
	init(orderOwe);
	init(modeCode);
	init(motiveSrvCode);

	strcpy(motiveCode,grpUserMsg->product_code);
	custId=atol(grpUserMsg->cust_id);
	idNo=atol(grpUserMsg->id_no);
	loginAccept=atol(login_accept);
	strcpy(loginNo,login_no);
	strcpy(motiveProdStr,vmotiveProdStr);
	strcpy(motiveProdSrvStr,vmotiveProdSrvStr);
	strcpy(motiveProdSmCode,vmotiveProdSmCode);
	strcpy(motiveSrvCode,vmotiveSrvCode);

	printf("\n++++fpubMotiveOrder BEGIN ++++\n");

	EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss') INTO :opTime FROM DUAL;

	Trim(login_accept);
	strncpy(tempStr,login_accept+strlen(login_accept)-3,3);
	sprintf(orderSeq,"%s%s",opTime,tempStr);

	strcpy(orderNote,opNote);

	/*���붩����DMOTIVEORDERINFO*/
	pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "���붩����DMOTIVEORDERINFO");
	/*EXEC SQL INSERT INTO DMOTIVEORDERINFO
				(
				 ORDER_SEQ,ORDER_TYPE,CUST_ID,MOTIVE_CODE,MOTIVE_NAME,ID_NO,
				 ORDER_STATUS,EFFT_DATE,LOGIN_ACCEPT,OP_TIME,
				 LOGIN_NO,ORDER_NOTE
				)
			SELECT
				 :orderSeq,'1',:custId,:motiveCode,motive_name,:idNo,
				 '1',to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
				 :loginAccept,sysdate,:loginNo,:orderNote
			FROM SmotiveProdCfg
			WHERE motive_code=:motiveCode;
	if(SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into DMOTIVEORDERINFO error[%d]",SQLCODE);
		return -1;
	}*/

	while(getValueByIndex(motiveProdSmCode,j,'|', moprodSmcode)>=0)
	{
		init(mosonProduct);
		getValueByIndex(motiveProdStr,j,'|', mosonProduct);
		getValueByIndex(motiveProdSrvStr,j,'|', moprodSrv);
		j++;

		icount=0;
		if(strcmp(moprodSmcode,"44") == 0)
		{
			EXEC SQL SELECT count(*) INTO :icount FROM dgrpusermsg a,dgrpusermsgadd b
					 WHERE a.cust_id=:custId AND a.id_no=b.id_no
					 AND a.run_code='A' AND b.field_code='bcode'
					 AND trim(b.field_value)=:mosonProduct;
			if(SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "select dgrpusermsg error[%d]",SQLCODE);
				return -2;
			}
		}
		else
		{
			EXEC SQL SELECT count(*) INTO :icount FROM dgrpusermsg
					 WHERE cust_id=:custId AND run_code='A'
					 AND product_code=:mosonProduct;
			if(SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "select dgrpusermsg error[%d]",SQLCODE);
				return -3;
			}
		}

		if(icount == 0)
		{
			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "δ�����Ӳ�Ʒ[%s]",mosonProduct);
			return -33;
		}
		pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "mosonProduct=[%s]",mosonProduct);
		pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "����ҵ������ϱ�DMOTIVEUSERMSG");

		if((strlen(motiveSrvCode) > 0) && (strcmp(motiveSrvCode,"0000") != 0))
		{
			EXEC SQL SELECT mode_code INTO :modeCode FROM SMOTIVEPRODMODECFG WHERE motive_code=:motiveCode AND product_code=:mosonProduct AND srv_code=:motiveSrvCode;
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "select SMOTIVEPRODMODECFG error[%d]",SQLCODE);
				return -4;
			}
			else if(SQLCODE == 1403)
			{
				strcpy(modeCode,"00000000");
			}
		}
		else
		{
			strcpy(modeCode,"00000000");
		}


		/*EXEC SQL INSERT INTO DMOTIVETASKINFO
						(
							ORDER_SEQ,CUST_ID,ID_NO,MOTIVE_CODE,MOTIVE_NAME,
							TASK_SEQ,TASK_TYPE,PRODUCT_CODE,SRV_CODE,PRICE_CODE,
							FAV_BEGIN,SRV_NAME,PRODUCT_NAME,TASK_STATUS,
							EFFT_DATE,PRODUCT_URL,OP_TIME,LOGIN_NO,TASK_NOTE,
							order_before,order_owe,mode_code
						)
					SELECT
						:orderSeq,:custId,:idNo,:motiveCode,a.motive_name,
						:orderSeq||to_char(:j),'1',:mosonProduct,:moprodSrv,c.price_code,
						decode(:orderBefore,'1',to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'yyyy-mm-dd'),'0',sysdate),
						c.srv_name,b.product_name,'1',
						to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
						d.product_url,sysdate,:loginNo,:orderNote,:orderBefore,:orderOwe,:modeCode
					FROM SmotiveProdCfg a,SMOTIVEPRODDETCFG b,SMOTIVEPRODSRVCFG c,SMOTIVEURLCFG d
					WHERE a.motive_code=:motiveCode AND a.motive_code=b.motive_code
					AND b.product_code=:mosonProduct AND b.motive_code=c.motive_code
					AND b.product_code=c.product_code AND c.srv_code=:moprodSrv
					AND b.product_smcode=d.sm_code;
		if(SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into DMOTIVETASKINFO error[%d]",SQLCODE);
			return -4;
		}*/

		/*ҵ������ϱ�*/

		Trim(mosonProduct);
		if(strlen(mosonProduct) == 10)
		{
			/*NG��ź
			EXEC SQL INSERT INTO DMOTIVEUSERMSG
							(
							 CUST_ID,MOTIVE_CODE,MOTIVE_NAME,PARENT_ID,
							 PARENT_CONTRACT,ID_NO,CONTRACT_NO,PRODUCT_CODE,PRODUCT_FLAG,
							 BEGIN_DATE,END_DATE,MOTIVE_STATUS,LOGIN_ACCEPT,LOGIN_NO,MODE_CODE
							)
						SELECT
							:custId,:motiveCode,a.motive_name,:idNo,b.account_id,
							c.id_no,c.account_id,:mosonProduct,'N',sysdate,to_date('20500101','yyyy-mm-dd'),
							'Y',:loginAccept,:loginNo,:modeCode
						FROM SmotiveProdCfg a,dgrpusermsg b,dgrpusermsg c,dgrpusermsgadd d
						WHERE a.motive_code=:motiveCode
						AND b.id_no=:idNo AND c.cust_id=:custId
						AND c.run_code='A' AND d.field_code='bcode'
					 	AND trim(d.field_value)=:mosonProduct
					 	AND d.id_no=c.id_no;
			if(SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into DMOTIVEUSERMSG error[%d]",SQLCODE);
				return -5;
			}
			NG��ź*/
			/*ng����  by magang at 20090812 begin*/
		pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "begin call OrderInsertMotiveUserMsg!");

		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select a.motive_name,b.account_id ,to_char(c.id_no),c.account_id\
							 FROM SmotiveProdCfg a,dgrpusermsg b,dgrpusermsg c,dgrpusermsgadd d\
							 WHERE a.motive_code=:motiveCode \
							AND b.id_no=:idNo AND c.cust_id=:custId\
							AND c.run_code='A' AND d.field_code='bcode'\
					 		AND trim(d.field_value)=:mosonProduct\
					 		AND d.id_no=c.id_no");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcur5 cursor for sql_str;
		EXEC SQL open ngcur5 using  :motiveCode,:idNo,:custId,:mosonProduct;
		for(;;)
		{
			init(MotiveName1);
			init(AccountId1);
			init(id_no1);
			init(account_id1);
			EXEC SQL FETCH ngcur5 into :MotiveName1,:AccountId1,:id_no1,:account_id1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("s1264Cfm");
				pubLog_DBErr(_FFL_,"","","��ȡserial_noʧ�� [%s]--\n",motiveCode);
				EXEC SQL CLOSE ngcur5;
				return -5;
			}
		if(1403==SQLCODE) break;

		Trim(MotiveName1);
		Trim(AccountId1);
		Trim(id_no1);
		Trim(account_id1);
		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertMotiveUserMsg!\n");
		memset(&sTdMotiveUserMsg,0,sizeof(TdMotiveUserMsg));
		strcpy(sTdMotiveUserMsg.sCustId,grpUserMsg->cust_id);
		strcpy(sTdMotiveUserMsg.sMotiveCode,motiveCode);
		strcpy(sTdMotiveUserMsg.sMotiveName,MotiveName1);
		strcpy(sTdMotiveUserMsg.sParentId,grpUserMsg->id_no);
		strcpy(sTdMotiveUserMsg.sParentContract,AccountId1);
		strcpy(sTdMotiveUserMsg.sIdNo,id_no1);
		strcpy(sTdMotiveUserMsg.sContractNo,account_id1);
		strcpy(sTdMotiveUserMsg.sProductCode,mosonProduct);
		strcpy(sTdMotiveUserMsg.sProductFlag,"N");
		strcpy(sTdMotiveUserMsg.sBeginDate,opTime);
		strcpy(sTdMotiveUserMsg.sEndDate,"20500101");
		strcpy(sTdMotiveUserMsg.sMotiveStatus,"Y");
		strcpy(sTdMotiveUserMsg.sLoginAccept,login_accept);
		strcpy(sTdMotiveUserMsg.sLoginNo,loginNo);
		strcpy(sTdMotiveUserMsg.sModeCode,modeCode);

		v_ret=0;
		v_ret=OrderInsertMotiveUserMsg("2",grpUserMsg->id_no,100,"7412",loginAccept,loginNo,orderNote,sTdMotiveUserMsg);

		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert DMOTIVEUSERMSG error[%d]",SQLCODE);
			EXEC SQL CLOSE ngcur5;
			return -5;
		}
	}
	EXEC SQL CLOSE ngcur5;
	/*ng����  by magang at 20090812 end*/

			if((strlen(moprodSrv) > 0) && (strcmp(moprodSrv,"0000") != 0))
			{
				EXEC SQL INSERT INTO dGrpSrvMsgHis
					(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
					SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
					MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
					OP_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG,
					UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
					UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
				SELECT
					a.id_no,'2', c.srv_code, c.price_code,
					1, a.product_code, sysdate, To_Date('20500101', 'YYYYMMDD'),
					'Y', :loginAccept,  :loginNo,     '7412',
					sysdate,     'Y', To_Date('20500101', 'YYYYMMDD'), 'N',
					:loginNo,     :loginAccept,  to_number(to_char(sysdate,'YYYYMMDD')),
					sysdate,'7412','A'
				FROM dgrpusermsg a,dgrpusermsgadd b,SMOTIVEPRODSRVCFG c
				WHERE a.cust_id=:custId AND a.id_no=b.id_no
					AND a.run_code='A' AND b.field_code='bcode'
					AND trim(b.field_value)=:mosonProduct
					AND c.product_code=:mosonProduct AND c.motive_code=:motiveCode
					AND c.srv_code=:moprodSrv;
			if(SQLCODE!=OK)
			{
					pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into dGrpSrvMsgHis error[%d]",SQLCODE);
					return -6;
			}

			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "��¼dGrpSrvMsg����Ϣ");
			EXEC SQL INSERT INTO dGrpSrvMsg
					(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
					SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
					MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
					OP_TIME,   USE_FLAG, UNVAILD_TIME, PERSON_FLAG)
			SELECT
				a.id_no,'2', c.srv_code, c.price_code,
				1, a.product_code, sysdate, To_Date('20500101', 'YYYYMMDD'),
				'Y', :loginAccept,  :loginNo,     '7412',
				sysdate,     'Y', To_Date('20500101', 'YYYYMMDD'), 'N'
			FROM dgrpusermsg a,dgrpusermsgadd b,SMOTIVEPRODSRVCFG c
			WHERE a.cust_id=:custId AND a.id_no=b.id_no
				AND a.run_code='A' AND b.field_code='bcode'
				AND trim(b.field_value)=:mosonProduct
				AND c.product_code=:mosonProduct AND c.motive_code=:motiveCode
				AND c.srv_code=:moprodSrv;
			if(SQLCODE!=OK)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into dGrpSrvMsg error[%d]",SQLCODE);
				return -7;
			}
			}
		}
		else
		{
			/*NG��ź
			**EXEC SQL INSERT INTO DMOTIVEUSERMSG
			**				(
			**				 CUST_ID,MOTIVE_CODE,MOTIVE_NAME,PARENT_ID,
			**				 PARENT_CONTRACT,ID_NO,CONTRACT_NO,PRODUCT_CODE,PRODUCT_FLAG,
			**				 BEGIN_DATE,END_DATE,MOTIVE_STATUS,LOGIN_ACCEPT,LOGIN_NO,MODE_CODE
			**				)
			**			SELECT
			**				:custId,:motiveCode,a.motive_name,:idNo,b.account_id,
			**				c.id_no,c.account_id,:mosonProduct,'N',sysdate,to_date('20500101','yyyy-mm-dd'),
			**				'Y',:loginAccept,:loginNo,:modeCode
			**			FROM SmotiveProdCfg a,dgrpusermsg b,dgrpusermsg c
			**			WHERE a.motive_code=:motiveCode
			**			AND a.motive_code=b.product_code
			**			AND b.id_no=:idNo AND c.cust_id=:custId
			**			AND c.product_code=:mosonProduct AND c.run_code='A';
			**if(SQLCODE != 0)
			**{
			**	pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into DMOTIVEUSERMSG error[%d]",SQLCODE);
			**	return -8;
			**}
			NG��ź*/
			/*ng����  by magang at 20090812 begin*/

		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select a.motive_name,b.account_id ,c.id_no,c.account_id\
							 from SmotiveProdCfg a,dgrpusermsg b,dgrpusermsg c\
							 WHERE a.motive_code=:motiveCode\
							AND a.motive_code=b.product_code\
							AND b.id_no=:idNo AND c.cust_id=:custId\
							AND c.product_code=:mosonProduct AND c.run_code='A'");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcur4 cursor for sql_str;
		EXEC SQL open ngcur4 using  :motiveCode,:idNo,:custId,:mosonProduct;
		for(;;)
		{
			init(MotiveName1);
			init(AccountId1);
			init(id_no1);
			init(account_id1);
			EXEC SQL FETCH ngcur4 into :MotiveName1,:AccountId1,:id_no1,:account_id1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("s1264Cfm");
				pubLog_DBErr(_FFL_,"","","��ȡserial_noʧ�� [%s]--\n",motiveCode);
				EXEC SQL CLOSE ngcur4;
				return -10;
			}
		if(1403==SQLCODE) break;


		Trim(MotiveName1);
		Trim(AccountId1);
		Trim(id_no1);
		Trim(account_id1);
		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertMotiveUserMsg!\n");
		memset(&sTdMotiveUserMsg,0,sizeof(TdMotiveUserMsg));
		strcpy(sTdMotiveUserMsg.sCustId,grpUserMsg->cust_id);
		strcpy(sTdMotiveUserMsg.sMotiveCode,motiveCode);
		strcpy(sTdMotiveUserMsg.sMotiveName,MotiveName1);
		strcpy(sTdMotiveUserMsg.sParentId,grpUserMsg->id_no);
		strcpy(sTdMotiveUserMsg.sParentContract,AccountId1);
		strcpy(sTdMotiveUserMsg.sIdNo,id_no1);
		strcpy(sTdMotiveUserMsg.sContractNo,account_id1);
		strcpy(sTdMotiveUserMsg.sProductCode,mosonProduct);
		strcpy(sTdMotiveUserMsg.sProductFlag,"N");
		strcpy(sTdMotiveUserMsg.sBeginDate,opTime);
		strcpy(sTdMotiveUserMsg.sEndDate,"20500101");
		strcpy(sTdMotiveUserMsg.sMotiveStatus,"Y");
		strcpy(sTdMotiveUserMsg.sLoginAccept,login_accept);
		strcpy(sTdMotiveUserMsg.sLoginNo,loginNo);
		strcpy(sTdMotiveUserMsg.sModeCode,modeCode);

		v_ret=0;
		v_ret=OrderInsertMotiveUserMsg("2",grpUserMsg->id_no,100,"7412",loginAccept,loginNo,orderNote,sTdMotiveUserMsg);

		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert DMOTIVEUSERMSG error[%d]",SQLCODE);
			EXEC SQL CLOSE ngcur4;
			return -5;
		}
	}
	EXEC SQL CLOSE ngcur4;
	 /*ng����  by magang at 20090812 end*/

			if((strlen(moprodSrv) > 0) && (strcmp(moprodSrv,"0000") != 0))
			{
				EXEC SQL INSERT INTO dGrpSrvMsgHis
					(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
					SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
					MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
					OP_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG,
					UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
					UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
				SELECT
					a.id_no,'2',c.srv_code, c.price_code,
					1, a.product_code, sysdate, To_Date('20500101', 'YYYYMMDD'),
					'Y', :loginAccept,  :loginNo,  '7412',
					sysdate,     'Y', To_Date('20500101', 'YYYYMMDD'), 'N',
					:loginNo,     :loginAccept,  to_number(to_char(sysdate,'YYYYMMDD')),
					sysdate,'7412','A'
				FROM dgrpusermsg a,SMOTIVEPRODSRVCFG c
				WHERE a.cust_id=:custId AND a.run_code='A'
					AND a.product_code=:mosonProduct
					AND c.product_code=:mosonProduct AND c.motive_code=:motiveCode
					AND c.srv_code=:moprodSrv;
				if(SQLCODE!=OK)
				{
					pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into dGrpSrvMsgHis error[%d]",SQLCODE);
					return -9;
				}

				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "��¼dGrpSrvMsg����Ϣ");
				EXEC SQL INSERT INTO dGrpSrvMsg
						(ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
						SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
						MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
						OP_TIME,   USE_FLAG, UNVAILD_TIME, PERSON_FLAG)
					SELECT
						a.id_no,'2',c.srv_code, c.price_code,
						1, a.product_code, sysdate, To_Date('20500101', 'YYYYMMDD'),
						'Y', :loginAccept,  :loginNo,     '7412',
						sysdate,     'Y', To_Date('20500101', 'YYYYMMDD'), 'N'
						FROM dgrpusermsg a,SMOTIVEPRODSRVCFG c
						WHERE cust_id=:custId AND run_code='A'
					AND a.product_code=:mosonProduct
					AND c.product_code=:mosonProduct AND c.motive_code=:motiveCode
					AND c.srv_code=:moprodSrv;
				if(SQLCODE!=OK)
				{
					pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert into dGrpSrvMsg error[%d]",SQLCODE);
					return -10;
				}
			}
		}

		if(strcmp(modeCode,"00000000") != 0)
		{
			EXEC SQL SELECT id_no INTO :sonId FROM DMOTIVEUSERMSG WHERE cust_id=:custId AND parent_id=:idNo AND product_code=:mosonProduct;
			if(SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "select DMOTIVEUSERMSG error[%d]",SQLCODE);
				return -11;
			}

			EXEC SQL INSERT INTO wMotiveProdModeOpr
						(
						 ID_NO,MEMBER_ID,PRODUCT_CODE,MODE_CODE,MODE_FLAG,START_TIME,
						 DO_FLAG,OP_TIME,LOGIN_ACCEPT,PHONE_NO
						)
					SELECT
						 :sonId,member_id,:mosonProduct,:modeCode,'1',
						 to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'yyyymmdd'),
						 '0',sysdate,sMaxSysAccept.nextval,member_no
					FROM dgrpusermebmsg
					WHERE id_no=:sonId AND end_time>last_day(sysdate)+1;
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert wMotiveProdModeOpr error[%d]",SQLCODE);
				return -11;
			}
		}
	}

	/*ҵ������ϱ�*/
	/*NG��ź
	pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "����ҵ������ϱ�DMOTIVEUSERMSG");
	**EXEC SQL INSERT INTO DMOTIVEUSERMSG
	**				(
	**				 CUST_ID,MOTIVE_CODE,MOTIVE_NAME,PARENT_ID,
	**				 PARENT_CONTRACT,ID_NO,CONTRACT_NO,PRODUCT_CODE,PRODUCT_FLAG,
	**				 BEGIN_DATE,END_DATE,MOTIVE_STATUS,LOGIN_ACCEPT,LOGIN_NO,MODE_CODE
	**				)
	**			SELECT
	**				:custId,:motiveCode,a.motive_name,:idNo,b.account_id,
	**				:idNo,b.account_id,:motiveCode,'Y',sysdate,to_date('20500101','yyyy-mm-dd'),
	**				'Y',:loginAccept,:loginNo,'00000000'
	**			FROM SmotiveProdCfg a,dgrpusermsg b
	**			WHERE a.motive_code=:motiveCode AND b.id_no=:idNo
	**			AND a.motive_code=b.product_code AND b.run_code='A';
	**if(SQLCODE != 0)
	**{
	**	pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert DMOTIVEUSERMSG error[%d]",SQLCODE);
	**	return -5;
	**}
	NG��ź*/
		/*ng����  by magang at 20090812 begin*/
		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select a.motive_name,b.account_id \
							 from SmotiveProdCfg a,dgrpusermsg b\
							 where a.motive_code=:motiveCode AND b.id_no=:idNo\
							 AND a.motive_code=b.product_code AND b.run_code='A'");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcur3 cursor for sql_str;
		EXEC SQL open ngcur3 using  :motiveCode,:idNo;
		for(;;)
		{
			init(MotiveName1);
			init(AccountId1);
			EXEC SQL FETCH ngcur3 into :MotiveName1,:AccountId1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				PUBLOG_DBDEBUG("s1264Cfm");
				pubLog_DBErr(_FFL_,"","","��ȡserial_noʧ�� [%s]--\n",motiveCode);
				EXEC SQL CLOSE ngcur3;
				return -9;
			}
		if(1403==SQLCODE) break;

		Trim(MotiveName1);
		Trim(AccountId1);

		pubLog_Debug(_FFL_,"", "", "begin call OrderInsertMotiveUserMsg!\n");
		memset(&sTdMotiveUserMsg,0,sizeof(TdMotiveUserMsg));
		strcpy(sTdMotiveUserMsg.sCustId,grpUserMsg->cust_id);
		strcpy(sTdMotiveUserMsg.sMotiveCode,motiveCode);
		strcpy(sTdMotiveUserMsg.sMotiveName,MotiveName1);
		strcpy(sTdMotiveUserMsg.sParentId,grpUserMsg->id_no);
		strcpy(sTdMotiveUserMsg.sParentContract,AccountId1);
		strcpy(sTdMotiveUserMsg.sIdNo,grpUserMsg->id_no);
		strcpy(sTdMotiveUserMsg.sContractNo,AccountId1);
		strcpy(sTdMotiveUserMsg.sProductCode,motiveCode);
		strcpy(sTdMotiveUserMsg.sProductFlag,"Y");
		strcpy(sTdMotiveUserMsg.sBeginDate,opTime);
		strcpy(sTdMotiveUserMsg.sEndDate,"20500101");
		strcpy(sTdMotiveUserMsg.sMotiveStatus,"Y");
		strcpy(sTdMotiveUserMsg.sLoginAccept,login_accept);
		strcpy(sTdMotiveUserMsg.sLoginNo,loginNo);
		strcpy(sTdMotiveUserMsg.sModeCode,"00000000");

		v_ret=0;
		v_ret=OrderInsertMotiveUserMsg("2",grpUserMsg->id_no,100,"7412",loginAccept,loginNo,orderNote,sTdMotiveUserMsg);

		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "fpubMotiveOrder", "", "insert DMOTIVEUSERMSG error[%d]",SQLCODE);
			EXEC SQL CLOSE ngcur3;
			return -5;
		}
	}
	EXEC SQL CLOSE ngcur3;
	/*ng����  by magang at 20090812 end*/

	printf("\n++++fpubMotiveOrder END ++++\n");

	return 0;
}

/**************
function name :fpubDelMotive
function desc :����100ҵ��������͹�������
**************/
int fpubDelMotive(char *login_accept,char *login_no,char *motive_id,char *op_note)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char	motiveCode[8+1];
	char	loginNo[6+1];
	char	orderSeq[17+1];
	long	custId=0;
	long	idNo=0;
	long	productId=0;
	long	loginAccept=0;
	char	orderNote[128+1];
	char	opTime[17+1];
	char	tempStr[3+1];
	int		i=0,j=0,icount=0,ret=0;
	char	sqlStr[512+1];
	char	orderBefore[1+1];
	char	orderOwe[1+1];
	char	motiveName[128+1];
	char	productCode[10+1];

	/*NG��ź*/
	int	v_ret=0;
	TdMotiveUserMsgIndex oldIndex;
	char	custId1[20+1];
	char	IdNo1[22+1];
	char	begin_date1[20+1];
	char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	EXEC SQL END DECLARE SECTION;

	init(motiveCode);
	init(loginNo);
	init(orderSeq);
	init(opTime);
	init(tempStr);
	init(orderNote);
	init(sqlStr);
	init(orderBefore);
	init(orderOwe);
	init(motiveName);
	init(productCode);
	/*NG��ź��ʼ������*/
	init(custId1);
	init(IdNo1);
	init(begin_date1);
	memset(&oldIndex,0,sizeof(TdMotiveUserMsgIndex));


	idNo=atol(motive_id);
	loginAccept=atol(login_accept);
	strcpy(loginNo,login_no);

	EXEC SQL SELECT to_char(sysdate,'yyyymmddhh24miss') INTO :opTime FROM DUAL;
	Trim(opTime);
	Trim(login_accept);
	strncpy(tempStr,login_accept+strlen(login_accept)-3,3);
	sprintf(orderSeq,"%s%s",opTime,tempStr);

	strcpy(orderNote,op_note);

	/*���붩����DMOTIVEORDERINFO*/
	/*	**EXEC SQL INSERT INTO DMOTIVEORDERINFO
		**		(
		**		 ORDER_SEQ,ORDER_TYPE,CUST_ID,MOTIVE_CODE,MOTIVE_NAME,ID_NO,
		**		 ORDER_STATUS,EFFT_DATE,LOGIN_ACCEPT,OP_TIME,
		**		 LOGIN_NO,ORDER_NOTE
		**		)
		**	SELECT
		**		 :orderSeq,'0',a.cust_id,a.product_code,b.product_name,:idNo,
		**		 '1',to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
		**		 :loginAccept,sysdate,:loginNo,:orderNote
		**	FROM dgrpusermsg a,sproductcode b
		**	WHERE a.id_no=:idNo AND a.product_code=b.product_code;
		**if(SQLCODE != 0)
		**{
		**	pubLog_Debug(_FFL_, "fpubDelMotive", "", "insert into DMOTIVEORDERINFO error[%d]",SQLCODE);
		**	return -1;
		**}
	    **
		**init(sqlStr);
		**sprintf(sqlStr, " select cust_id,motive_code,motive_name,id_no,product_code from DMOTIVEUSERMSG "
		**				" where parent_id=:v1 AND motive_status='Y' ");
		**EXEC SQL PREPARE motivemt FROM :sqlStr;
		**EXEC SQL DECLARE del_cur CURSOR for motivemt;
		**EXEC SQL OPEN del_cur using :idNo;
		**for(;;)
		**{
		**	custId=0;
		**	productId=0;
		**	init(motiveCode);
		**	init(motiveName);
		**	init(productCode);
		**	EXEC SQL FETCH del_cur INTO :custId,:motiveCode,:motiveName,:productId,:productCode;
		**	if(SQLCODE == 1403) break;
		**	if(SQLCODE != 0 && SQLCODE != 1403)
		**	{
		**		pubLog_Debug(_FFL_, "fpubDelMotive", "", "select DMOTIVEUSERMSG error [%d]",SQLCODE);
		**		EXEC SQL CLOSE del_cur;
		**		return -2;
		**	}
		**
		**	j++;
		**
		**	EXEC SQL SELECT count(*) INTO :icount FROM dgrpusermsg
		**				 WHERE id_no=:productId AND run_code='a';
		**	if(SQLCODE != 0 && SQLCODE != 1403)
		**	{
		**		pubLog_Debug(_FFL_, "fpubDelMotive", "", "select dgrpusermsg error[%d]",SQLCODE);
		**		EXEC SQL CLOSE del_cur;
		**		return -3;
		**	}
		**
		**	if(icount > 0)
		**	{
		**		strcpy(orderBefore,"1");
		**		strcpy(orderOwe,"1");
		**	}
		**	else
		**	{
		**		strcpy(orderBefore,"0");
		**		strcpy(orderOwe,"0");
		**	}
		**
		**	EXEC SQL INSERT INTO DMOTIVETASKINFO
		**					(
		**					 ORDER_SEQ,CUST_ID,ID_NO,MOTIVE_CODE,MOTIVE_NAME,
		**					 TASK_SEQ,TASK_TYPE,PRODUCT_CODE,SRV_CODE,PRICE_CODE,
		**					 FAV_BEGIN,SRV_NAME,PRODUCT_NAME,TASK_STATUS,
		**					 EFFT_DATE,PRODUCT_URL,OP_TIME,LOGIN_NO,TASK_NOTE,
		**					 order_before,order_owe,mode_code
		**					)
		**				SELECT
		**					:orderSeq,:custId,:idNo,:motiveCode,:motiveName,
		**					:orderSeq||to_char(:j),'0',:productCode,'0000','0000',
		**					to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'yyyy-mm-dd'),
		**					'��',b.product_name,'1',
		**					to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
		**					c.cancel_url,sysdate,:loginNo,:orderNote,:orderBefore,:orderOwe,'00000000'
		**				FROM dgrpusermsg a,sproductcode b,SMOTIVEURLCFG c
		**				WHERE a.id_no=:productId AND a.product_code=b.product_code
		**				AND a.sm_code=c.sm_code;
		**	if(SQLCODE != 0)
		**	{
		**		EXEC SQL CLOSE del_cur;
		**		pubLog_Debug(_FFL_, "fpubDelMotive", "", "insert into DMOTIVETASKINFO error[%d]",SQLCODE);
		**		return -4;
		**	}
		**}
		**EXEC SQL CLOSE del_cur;
		*/
	/*ҵ������ϱ�*/
	/* NG��ź
	EXEC SQL delete DMOTIVEUSERMSG
						  WHERE parent_id=:idNo;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		pubLog_Debug(_FFL_, "fpubDelMotive", "", "delete DMOTIVEUSERMSG error[%d]",SQLCODE);
		return -5;
	}
	NG��ź*/
	/*ng����  by magang at 20090812 begin*/
	 init(sqlStr);
	sprintf(sqlStr, " select to_char(cust_id),motive_code,to_char(id_no),to_char(begin_date,'yyyymmdd hh24:mi:ss') from DMOTIVEUSERMSG where parent_id=:v1 ");
	EXEC SQL PREPARE motivemt1 FROM :sqlStr;
	EXEC SQL DECLARE del_cur1 CURSOR for motivemt1;
	EXEC SQL OPEN del_cur1 using :idNo;
	for(;;)
	{
		init(custId1);
		init(IdNo1);
		init(motiveCode);
		init(begin_date1);

		EXEC SQL FETCH del_cur1 INTO :custId1,:motiveCode,:IdNo1,:begin_date1;
		if(SQLCODE == 1403) break;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			pubLog_Debug(_FFL_, "fpubDelMotive", "", "select DMOTIVEUSERMSG error [%d]",SQLCODE);
			EXEC SQL CLOSE del_cur1;
			return -2;
		}
		Trim(custId1);
		Trim(motiveCode);
		Trim(IdNo1);
		Trim(begin_date1);
		Trim(motive_id);

		pubLog_Debug(_FFL_,"", "", "begin call OrderDeleteMotiveUserMsg!\n");
		memset(&oldIndex,0,sizeof(TdMotiveUserMsgIndex));
		strcpy(oldIndex.sCustId,custId1);
		strcpy(oldIndex.sMotiveCode,motiveCode);
		strcpy(oldIndex.sParentId,motive_id);
		strcpy(oldIndex.sIdNo,IdNo1);
		strcpy(oldIndex.sBeginDate,begin_date1);

		init(i_parameter_array);
		strcpy(i_parameter_array[0],custId1);
		strcpy(i_parameter_array[1],motiveCode);
		strcpy(i_parameter_array[2],motive_id);
		strcpy(i_parameter_array[3],IdNo1);
  		strcpy(i_parameter_array[4],begin_date1);
	v_ret=OrderDeleteMotiveUserMsg("2",motive_id,100,"7413",loginAccept,loginNo,orderNote,oldIndex,"",i_parameter_array);

		if(0!=v_ret && 1!=v_ret)
		{
			pubLog_Debug(_FFL_, "fpubDelMotive", "", "delete DMOTIVEUSERMSG error[%d]",SQLCODE);
			EXEC SQL CLOSE del_cur1;
			return -5;
		}
	}
	EXEC SQL CLOSE del_cur1;
	/*ng����  by magang at 20090812 end*/
	return 0;
}

/**************
function name :fpubdropMotive
function desc :����100ҵ��������͹�������
**************/
int fpubdropMotive(char *login_accept,char *login_no,char *motive_id,char *op_note)
{
	EXEC SQL BEGIN DECLARE SECTION;

	char	motiveCode[8+1];
	char	loginNo[6+1];
	char	orderSeq[17+1];
	long	custId=0;
	long	idNo=0;
	long	productId=0;
	long	loginAccept=0;
	char	orderNote[128+1];
	char	opTime[17+1];
	char	tempStr[3+1];
	int		i=0,j=0,icount=0;
	char	sqlStr[512+1];
	char	orderBefore[1+1];
	char	orderOwe[1+1];
	char	motiveName[128+1];
	char	productCode[10+1];

	EXEC SQL END DECLARE SECTION;

	init(motiveCode);
	init(loginNo);
	init(orderSeq);
	init(opTime);
	init(tempStr);
	init(orderNote);
	init(sqlStr);
	init(orderBefore);
	init(orderOwe);
	init(motiveName);
	init(productCode);

	idNo=atol(motive_id);
	loginAccept=atol(login_accept);
	strcpy(loginNo,login_no);

	EXEC SQL SELECT to_char(sysdate,'yyyymmddhh24miss') INTO :opTime FROM DUAL;
	Trim(opTime);
	Trim(login_accept);
	strncpy(tempStr,login_accept+strlen(login_accept)-3,3);
	sprintf(orderSeq,"%s%s",opTime,tempStr);
	printf("orderSeq = [%s]\n",orderSeq);

	strcpy(orderNote,op_note);

	/*���붩����DMOTIVEORDERINFO*/
	EXEC SQL INSERT INTO DMOTIVEORDERINFO
				(
				 ORDER_SEQ,ORDER_TYPE,CUST_ID,MOTIVE_CODE,MOTIVE_NAME,ID_NO,
				 ORDER_STATUS,EFFT_DATE,LOGIN_ACCEPT,OP_TIME,
				 LOGIN_NO,ORDER_NOTE
				)
			SELECT
				 :orderSeq,'0',a.cust_id,a.product_code,b.product_name,:idNo,
				 '1',to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
				 :loginAccept,sysdate,:loginNo,:orderNote
			FROM dgrpusermsg a,sproductcode b
			WHERE a.id_no=:idNo AND a.product_code=b.product_code;
	if(SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "fpubdropMotive", "", "insert into DMOTIVEORDERINFO error[%d]",SQLCODE);
		return -1;
	}

	init(sqlStr);
	sprintf(sqlStr, " select cust_id,motive_code,motive_name,id_no,product_code from DMOTIVEUSERMSG "
					" where parent_id=:v1 ");
	EXEC SQL PREPARE motivemt1 FROM :sqlStr;
	EXEC SQL DECLARE drop_cur CURSOR for motivemt1;
	EXEC SQL OPEN drop_cur using :idNo;
	for(;;)
	{
		custId=0;
		productId=0;
		init(motiveCode);
		init(motiveName);
		init(productCode);
		EXEC SQL FETCH drop_cur INTO :custId,:motiveCode,:motiveName,:productId,:productCode;
		if(SQLCODE == 1403) break;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			pubLog_Debug(_FFL_, "fpubdropMotive", "", "select DMOTIVEUSERMSG error [%d]",SQLCODE);
			EXEC SQL CLOSE drop_cur;
			return -2;
		}

		j++;

		EXEC SQL SELECT count(*) INTO :icount FROM dgrpusermsg
					 WHERE id_no=:productId AND run_code='a';
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			pubLog_Debug(_FFL_, "fpubdropMotive", "", "select dgrpusermsg error[%d]",SQLCODE);
			EXEC SQL CLOSE drop_cur;
			return -3;
		}

		if(icount > 0)
		{
			strcpy(orderBefore,"1"); /*������*/
			strcpy(orderOwe,"1");
		}
		else
		{
			strcpy(orderBefore,"0");
			strcpy(orderOwe,"0");
		}

		EXEC SQL INSERT INTO DMOTIVETASKINFO
						(
						 ORDER_SEQ,CUST_ID,ID_NO,MOTIVE_CODE,MOTIVE_NAME,
						 TASK_SEQ,TASK_TYPE,PRODUCT_CODE,SRV_CODE,PRICE_CODE,
						 FAV_BEGIN,SRV_NAME,PRODUCT_NAME,TASK_STATUS,
						 EFFT_DATE,PRODUCT_URL,OP_TIME,LOGIN_NO,TASK_NOTE,
						 order_before,order_owe,mode_code
						)
					SELECT
						:orderSeq,:custId,:idNo,:motiveCode,:motiveName,
						:orderSeq||to_char(:j),'0',:productCode,'0000','0000',
						to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'yyyy-mm-dd'),
						'��',b.product_name,'1',
						to_date(to_char(last_day(sysdate),'yyyymmdd'),'yyyy-mm-dd'),
						c.cancel_url,sysdate,:loginNo,:orderNote,:orderBefore,:orderOwe,'00000000'
					FROM dgrpusermsg a,sproductcode b,SMOTIVEURLCFG c
					WHERE a.id_no=:productId AND a.product_code=b.product_code
					AND a.sm_code=c.sm_code;
		if(SQLCODE != 0)
		{
			EXEC SQL CLOSE drop_cur;
			pubLog_Debug(_FFL_, "fpubdropMotive", "", "insert into DMOTIVETASKINFO error[%d]",SQLCODE);
			return -4;
		}
	}
	EXEC SQL CLOSE drop_cur;

	return 0;
}

int fpubdelmotivemsg(char *mlLoginAccept,char *mvOpCode,char *mvLoginNo,char *mvLogin_Pass,char *mvOrgCode,
					 char *mvSys_Note,char *mvOp_Note,char *mvIp_Address,char *mvGrpIdNo,char *mdelMemFlag)
{
	char		temp_buf[61];       /*��ʱʹ�õ��ַ�������*/
	int			i=0, retCode=0;
	int			RetValue=0;
	double		v_unbill_fee=0.00;
	double		v_unbill_should = 0.00;
	double		v_unbill_favour = 0.00;
	int			retCodeTp=0;
	long		ilen=0;
	long		olen=0;
	char		return_code_tp[6+1]; /* ���ش��� */
	char		return_mess_tp[61]; /* ������Ϣ */
	char	nextMonthFirst[8+1];

	CFUNCCONDENTRY fe;

	/*---��֯������������޸Ĳ��� add by cencm at 30/06/2009---*/
	char		service_name[19 + 1];

	EXEC SQL BEGIN DECLARE SECTION;
		long  lLoginAccept=0;       /* ������ˮ  */
		char  strLoginAccept[14 + 1];
		char  vSys_Note[60+1];      /* ϵͳ��ע */
		char  vOp_Note[60+1];       /* ������ע  */
		char  return_code[7];       /* ���ش���  */
		char  return_mess[61];      /* ������Ϣ  */
		char	delMemFlag[1+1];
		char  vGrpIdNo[20+1];       /* �����û�ID*/
		char	vProductCode[8+1];
		char	vContractNo[14+1];
		char	vGrpSmCode[2+1];
		char	vMemberIdNo[14+1];
		char  vLoginNo[6+1];       /* ����       */
		char 	vLogin_Pass[16+1];    /* ����Ա���� */
		char 	vIp_Address[20+1];    /* ����ԱIP   */
		char  vOpCode[4+1];        /* ��������   */
		char 	vOrgCode[9+1];       /* ��������   */
		char	vOpTime[17+1];       /* ҵ����ʱ�� */
		char  retMsg[128+1];
		char  vOrgId[10+1];
		char  vGroupId[10+1];
		int	  iCount = 0;
		char 	vApnNo[20+1];
		char 	vApnTermIdNo[14+1];
		char 	ipAddress[15+1];
		char  dynStmt[2048+1];
		char  vRegionCode[2+1];
		char  vYearMonth[6+1];
		char  vDateChar[6+1];       /* ��������  */
		char  vGrpUserNo[15+1];
		char	str_retCode[6+1];
		double 	prepay_fee=0.00;
		char	unitId[20+1];
		char    URL1[128];
		char    URL2[128];
		char    inputBuf[512];
		char    outputBuf[1024];
		char    httpCode[6+1];
		char    httpMsg[256+1];
		/*��֯�ṹ���� add by cencm at 2009/7/2 */
		char  vGroup_Id[10+1];
		char  vOrg_Id[10+1];
		int		ret=0;
		char  vTotal_Date[8+1];
		tLoginOpr vLog;
		char vChgFlag[1+1];					/*recordOprLog���ر�־*/
		char vCustFlag[1+1];				/*recordOprLog���ر�־*/
		/*NG��ź���Ӳ���*/
		TdGrpUserMebMsgIndex oldIndex;
		int	v_ret=0;
		char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char	member_id1[22+1];
		char	begin_time1[20+1];
		char	end_time1[20+1];
		char	sTempSqlStr[1024];
		/*-- ����������ʱ���� edit by zhaohx at 14/07/2010 --*/
		FBFR32    *sendbuf, *rcvbuf;
		FLDLEN32  sendlen,rcvlen;
		int 	  sendNum=0;
		int 	  recvNum=0;
		char 	  buffer1[256];
		char	  buffer2[256];
		char    sCount[10+1];
	EXEC SQL END DECLARE SECTION;


	init(strLoginAccept	);
	init(vOp_Note    	);
	init(return_code 	);
	init(return_mess 	);
	init(delMemFlag	 	);
	/*---��֯������������޸Ĳ��� add by cencm at 30/06/2009---*/
	init(service_name);
	strcpy(service_name, "fpubdelmotivemsg");
	init(nextMonthFirst);

	init(vGrpIdNo    	);
	init(vProductCode	);
	init(vContractNo 	);
	init(vGrpSmCode  	);
	init(vMemberIdNo 	);
	init(vLoginNo    	);
	init(vLogin_Pass 	);
	init(vIp_Address 	);
	init(vOpCode     	);
	init(vOrgCode    	);
	init(vOpTime     	);
	init(retMsg		 	);
	init(vOrgId      	);
	init(vGroupId	 	);
	init(vApnNo		 	);
	init(vApnTermIdNo	);
	init(ipAddress	 	);
	init(dynStmt	 	);
	init(vRegionCode 	);
	init(vYearMonth	 	);
	init(vDateChar    );
	init(vGrpUserNo	 	);
	init(str_retCode 	);
	init(unitId);
	init(URL1);
	init(URL2);
	init(inputBuf);
	init(outputBuf);
	init(httpCode);
	init(httpMsg);
	/*��֯�ṹ���� add by cencm at 2009/7/2 */
	init(vOrg_Id      	);
	init(vGroup_Id	 	);
	memset(&vLog, 0, sizeof(vLog));
	init(vChgFlag);
	init(vCustFlag);
	init(vTotal_Date);
	init(buffer1);
	init(buffer2);
	init(sCount);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "����fpubdelmotivemsg��ʼִ��");
#endif

	lLoginAccept = atol(mlLoginAccept);
	strcpy(vOpCode,mvOpCode);
	strcpy(vLoginNo,mvLoginNo);
	strcpy(vLogin_Pass,mvLogin_Pass);
	strcpy(vOrgCode,mvOrgCode);
	strcpy(vSys_Note,mvSys_Note);
	strcpy(vOp_Note,mvOp_Note);
	strcpy(vIp_Address,mvIp_Address);
	strcpy(vGrpIdNo,mvGrpIdNo);
	strcpy(delMemFlag,mdelMemFlag);


#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "lLoginAccept =[%ld]", lLoginAccept  );
	pubLog_Debug(_FFL_, service_name, "", "vOpCode      =[%s] ", vOpCode   		);
	pubLog_Debug(_FFL_, service_name, "", "vLoginNo     =[%s] ", vLoginNo 			);
	pubLog_Debug(_FFL_, service_name, "", "vLogin_Pass  =[%s] ", vLogin_Pass 	);
	pubLog_Debug(_FFL_, service_name, "", "vOrgCode     =[%s] ", vOrgCode    	);
	pubLog_Debug(_FFL_, service_name, "", "vSys_Note    =[%s] ", vSys_Note 		);
	pubLog_Debug(_FFL_, service_name, "", "vOp_Note     =[%s] ", vOp_Note    	);
	pubLog_Debug(_FFL_, service_name, "", "vIp_Address  =[%s] ", vIp_Address   );
	pubLog_Debug(_FFL_, service_name, "", "vGrpIdNo     =[%s] ", vGrpIdNo     	);
	pubLog_Debug(_FFL_, service_name, "", "delMemFlag   =[%s] ", delMemFlag 		);
#endif

	EXEC SQL SELECT to_char(sysdate, 'yyyymmdd hh24:mi:ss'), to_char(sysdate,'YYYYMM'), to_char(add_months(sysdate, -1), 'YYYYMM'),to_char(sysdate,'yyyymmdd'),to_char(sysdate,'YYYYMMDD')
		into :vOpTime, :vYearMonth, :vDateChar,:nextMonthFirst,:vTotal_Date
	from dual;
	if(SQLCODE != 0)
	{
		strcpy(return_code, "352301");
		sprintf(return_mess, "��ȡ����ʱ��ʧ��![%d]",SQLCODE);
		pubLog_Debug(_FFL_, service_name, "", "��ȡ����ʱ��ʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
		return -1;
	}
	EXEC SQL select group_id, org_code into :vGroupId, :vOrgId
		from dLoginmsg
		where login_no = :vLoginNo;
	if(SQLCODE != 0){
		strcpy(return_code, "352302");
		sprintf(return_mess, "��ѯ������Ϣʧ��[%d][%s]",SQLCODE,vLoginNo);
		pubLog_Debug(_FFL_, service_name, "", "��ѯ������Ϣʧ��[%s]",vLoginNo);
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
		return -2;
	}

	/*�ж��Ƿ��г�Ա���ݣ���������޷����˲���*/

	EXEC SQL select count(*) INTO :iCount
	FROM dGrpUserMebMsg
		WHERE ID_NO= to_number(:vGrpIdNo) and end_time > last_day(sysdate) + 1;
	if (iCount != 0) {
	strcpy(return_code, "309602");
	sprintf(return_mess, "�ü����û����г�Ա����������������!");
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	return -3;
	}

	/* ��ѯ����Ԥ��״̬(I)�ļ����û����ϣ������Ǳ��³���ǰ���������� */
	EXEC SQL select region_code, product_code, to_char(account_id), user_no, sm_code
	into :vRegionCode, :vProductCode, :vContractNo, :vGrpUserNo, :vGrpSmCode
	from dGrpUserMsg
	where id_no = :vGrpIdNo and run_code = 'A';
	if(SQLCODE!=0)
	{
		strcpy(return_code, "352304");
	sprintf(return_mess, "�����û�����Ԥ��״̬������������![%d][%s]",SQLCODE,vGrpIdNo);
	pubLog_Debug(_FFL_, service_name, "", "ȡ�����û�ʧ��dgrpusermsg[%s]!",vGrpIdNo);
	PUBLOG_DBDEBUG(service_name);
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	return -4;
	}

/*��֯�ṹ���� edit by cencm at 2009/6/30*/
	ret=0;
	ret = sGetUserGroupid(vGrpUserNo,vGroup_Id);
	if(ret <0)
	{
		sprintf(return_code,"%06s","309692");
		strcpy(return_mess,"��ѯ�û�group_idʧ��!");
		return -5;
	}
	Trim(vGroup_Id);
	ret =0;
	ret = sGetLoginOrgid(vLoginNo,vOrg_Id);
	if(ret <0)
	{
		sprintf(return_code,"%06s","309691");
		strcpy(return_mess,"��ѯ����org_idʧ��!");
		return -6;
	}
	Trim(vOrg_Id);

	/* ҵ�����Ƽ�� chenxuan note 20070812 */
	sprintf(return_code, "%06d", fCheckFuncOpr(vGrpUserNo, vOpCode, vLoginNo, &fe));
	if (strcmp(return_code, "000000") != 0) {
	/*
	**getValueByParamCode(&fe, 1000, return_code);
	**getValueByParamCode(&fe, 1001, return_mess);
	*/
	pubLog_Debug(_FFL_, service_name, "", "ҵ�����Ƽ��ʧ��fCheckFuncOpr[%s][%s][%s]", vOpCode,return_code,vGrpUserNo);
	sprintf(return_mess, "ҵ�����Ƽ��ʧ��[%s][%s]" ,vGrpUserNo,return_code);
	strcpy(return_code, "352305");/*����return_code����ʾ �ٸ�ֵ*/
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	return -7;
	}

/*��ѯ�����Ƿ���δ���˵�,���в��������*/
	/*-- ����֮������ø��� edit by zhaohx  at 14/07/2010 begin --*/
/***sprintf(dynStmt, "select count(*) from dGrpOweTotal%s where id_no = :v1 and payed_status = 0", vDateChar);
	**pubLog_Debug(_FFL_, service_name, "", "dynStmt=[%s][%s]", dynStmt, vGrpIdNo);
	**EXEC SQL PREPARE pre FROM :dynStmt;
	**EXEC SQL DECLARE cur CURSOR FOR pre;
	**EXEC SQL OPEN cur USING :vGrpIdNo;
	**EXEC SQL FETCH cur INTO :iCount;
	**if (SQLCODE != 0) {
	**strcpy(return_code, "352304");
	**sprintf(return_mess, "��ѯ����δ���˵�����![%d][%s]", SQLCODE, vGrpIdNo);
	**EXEC SQL CLOSE cur;
	**PUBLOG_DBDEBUG(service_name);
	**pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	**return -8;
	**}
	**EXEC SQL CLOSE cur;
    **
	**if (iCount > 0)
	**{
	**	sprintf(return_mess, "����[%s]��δ���˵�����������", vGrpIdNo);
	**	strcpy(return_code, "352306");
	**	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	**	return -9;
	**}
	*/
	/*�����������*/
	sendNum = 1;

	/*�����������*/
	recvNum = 3;

	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		strcpy(return_code,"352304");
		strcpy(return_mess,"��ʼ�����������ʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_mess);
		return -9;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);

	Fchg32(sendbuf,GPARM32_0,0,vGrpIdNo,(FLDLEN32)0);

	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);

	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		strcpy(return_code,"352316");
		strcpy(return_mess,"��ʼ�����������ʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_mess);
		return -9;
	}

	long reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	ret = 0;
	ret = fPubCallSrv(CALLSERV_TP,"bs_qryIsGrpOwe",sendbuf,&rcvbuf);
	if(ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		strcpy(return_code,"352317");
		sprintf(return_mess,"call DGRPOWETOTAL%s failed!",vDateChar);
		PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_, service_name, "", "v_ret=[%d]!",ret);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_mess);
		return -9;
	}

	memset(buffer1, '\0', sizeof(buffer1));
	memset(buffer2, '\0', sizeof(buffer2));

	Fget32(rcvbuf,SVC_ERR_NO32,0,buffer1,(FLDLEN32)0);
	Fget32(rcvbuf,SVC_ERR_MSG32,0,buffer2,(FLDLEN32)0);

	Trim(buffer1);
	Trim(buffer2);
	printf("buffer1[%s]\n",buffer1);
	printf("buffer2[%s]\n",buffer2);

	Fget32(rcvbuf,GPARM32_2,0,sCount,(FLDLEN32)0);

	Trim(sCount);
  iCount=atoi(sCount);
	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);

	if (iCount > 0)
	{
		sprintf(return_mess, "����[%s]��δ���˵�����������", vGrpIdNo);
		strcpy(return_code, "352317");
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
		return -9;
	}
	/*-- ����֮������ø��� edit by zhaohx at 14/07/2010 end --*/

	/*�ж�Ƿ��,Ƿ�Ѽ��Ų��������*/
	v_unbill_fee=0.00;
	v_unbill_should = 0.00;
	v_unbill_favour = 0.00;
	RetValue=0;

	RetValue = fGetGrpunbillowe(vContractNo,&v_unbill_fee,&prepay_fee);
	if (RetValue != 0)
	{
		pubLog_Debug(_FFL_, service_name, "", "�鼯�Ż��ѳ���PublicGetUnbillChg[%d]",RetValue);
		sprintf(return_mess, "�鼯�Ż��ѳ���[%s][%d]" ,vGrpUserNo,RetValue);
		strcpy(return_code, "352306");
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
		return -10;
	}

	pubLog_Debug(_FFL_, service_name, "", "prepay_fee---[%.2f]",prepay_fee);
	pubLog_Debug(_FFL_, service_name, "", "v_unbill_fee---[%.2f]",v_unbill_fee);

	/*-- Ƿ���ж�Ԥ��������ܻ��ѵľ��� edit by zhaohx  at 28/2/2011 begin --*/

	/*if((prepay_fee-v_unbill_fee)<0)
	{
	pubLog_Debug(_FFL_, service_name, "", "����Ƿ��[%.2f][%.2f]",prepay_fee,v_unbill_fee);
	strcpy(return_mess, "Ƿ�Ѽ��Ų���������ҵ��!");
	strcpy(return_code, "352308");
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	return -11;
	}*/

	if(prepay_fee<0)
	{
	pubLog_Debug(_FFL_, service_name, "", "����Ƿ��[%.2f][%.2f]",prepay_fee,v_unbill_fee);
	strcpy(return_mess, "Ƿ�Ѽ��Ų���������ҵ��!");
	strcpy(return_code, "352308");
	pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
	return -11;
	}

	/*-- Ƿ���ж�Ԥ��������ܻ��ѵľ��� edit by zhaohx  at 28/2/2011 end --*/
	/**�жϼ����Ƿ�Ƿ�ѽ���**/


	/*���ϵͳ������ˮ*/
	if (lLoginAccept <= 0){
	EXEC SQL SELECT sMaxSysAccept.nextval INTO :lLoginAccept FROM DUAL;
	if (SQLCODE!=0)
	{
		strcpy(return_code, "352309");
		sprintf(return_mess, "��ȡ������ˮʧ��![%d]",SQLCODE);
		pubLog_Debug(_FFL_, service_name, "", "��ȡ������ˮʧ��!");
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
		return -12;
	}
		if(lLoginAccept <= 0)
		{
			sprintf(return_code, "%06s", "352310");
			sprintf(return_mess, "ȡ������ˮ�Ų��ɹ���������ȷ��!");
			pubLog_Debug(_FFL_, service_name, "", "��ȡ������ˮʧ��!");
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -13;
		}
	}
	sprintf(strLoginAccept, "%ld", lLoginAccept);
	pubLog_Debug(_FFL_, service_name, "", "luxc:login_accept=[%s]",strLoginAccept);

		/* ɾ�����ų�Ա���Ժ����޸�publicGrp.cp�е�DeleteGrpUser���� */
		/* NG��ź
		EXEC SQL delete from dGrpUserMebMsg where id_no = to_number(:vGrpIdNo);
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			strcpy(return_code, "352318");
			sprintf(return_mess, "ɾ�����ų�Աʧ��[%s]\n", vGrpIdNo);
			pubLog_Debug(_FFL_, service_name, "", "ɾ�����ų�Աʧ��fpubDelGrpUser[%s][%s][%s]", vGrpIdNo, vOpCode, retMsg);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -14;
		}
		 NG��ź*/
		/*ng����  by magang at 20090902 begin*/
		memset(sTempSqlStr, 0, sizeof(sTempSqlStr));
		sprintf(sTempSqlStr,"select to_char(member_id),to_char(begin_time,'yyyymmdd hh24:mi:ss'),to_char(end_time,'yyyymmdd hh24:mi:ss') from dGrpUserMebMsg where id_no = to_number(:vGrpIdNo)");
		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL declare ngcursor5 cursor for sql_str;
		EXEC SQL open ngcursor5 using :vGrpIdNo;
		for(;;)
		{
			init(member_id1);
			init(begin_time1);
			init(end_time1);
			EXEC SQL FETCH ngcursor5 into :member_id1,:begin_time1,:end_time1;
			if((0!=SQLCODE)&&(1403!=SQLCODE))
			{
				strcpy(return_mess, "��ѯ�α����!");
				strcpy(return_code,	"352318");
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_,"","","��ȡmember_idʧ�� [%s]--\n",vGrpIdNo);
				EXEC SQL CLOSE ngcursor5;
				return -14;
			}
			if(1403==SQLCODE) break;
			Trim(member_id1);
			Trim(begin_time1);
			Trim(end_time1);
			Trim(vGrpIdNo);
			memset(&oldIndex,0,sizeof(TdGrpUserMebMsgIndex));
			strcpy(oldIndex.sIdNo,vGrpIdNo);
			strcpy(oldIndex.sMemberId,member_id1);
			strcpy(oldIndex.sBeginTime,begin_time1);
			strcpy(oldIndex.sEndTime,end_time1);

			init(i_parameter_array);
			strcpy(i_parameter_array[0],vGrpIdNo);
			strcpy(i_parameter_array[1],member_id1);
			strcpy(i_parameter_array[2],begin_time1);
			strcpy(i_parameter_array[3],end_time1);
			v_ret=OrderDeleteGrpUserMebMsg("2",vGrpIdNo,100,vOpCode,lLoginAccept,vLoginNo,vOp_Note,oldIndex,"",i_parameter_array);


			if(0!=v_ret&&1!=v_ret)
			{
				strcpy(return_code, "352318");
				sprintf(return_mess, "ɾ�����ų�Աʧ��[%s][%d]\n", vGrpIdNo,SQLCODE);
				pubLog_Debug(_FFL_, service_name, "", "ɾ�����ų�Աʧ��OrderDeleteGrpUserMebMsg[%s][%s][%s]", vGrpIdNo, vOpCode, retMsg);
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
				return -14;
			}
		}
 		EXEC SQL CLOSE ngcursor5;
		/*ng����  by magang at 20090902 END*/
		pubLog_Debug(_FFL_, service_name, "", "all member del,last=[%s]",vMemberIdNo);
		pubLog_Debug(_FFL_, service_name, "", "luxc:fmotivepubDelGrpUser������ʼ");

		if(0 != fmotivepubDelGrpUser(vGrpIdNo, vLoginNo, vOpCode, vOpTime, lLoginAccept, "��������3523", retMsg))
		{
			strcpy(return_code, "352314");
			sprintf(return_mess, "ɾ�������û�ʧ��[%s]\n", vGrpIdNo);
			pubLog_Debug(_FFL_, service_name, "", "ɾ�������û�ʧ��fmotivepubDelGrpUser[%s][%s][%s]", vGrpIdNo, vOpCode, retMsg);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -15;
		}

		pubLog_Debug(_FFL_, service_name, "", "luxc:fmotivepubDelGrpUser��������");

		/*�����������潫dGrpUserMsg״̬��I,�����ٴ��ó�a,������������д��ʷ����,�������ﲻ��д*/
		/* NG��ź
		EXEC SQL update dGrpUserMsg	set run_code = 'a',op_time = to_date(:vOpTime, 'YYYYMMDD HH24:MI:SS') where id_no = :vGrpIdNo;
		if( SQLCODE != 0 )
		{
			strcpy(return_code, "352315");
			sprintf(return_mess, "���¼��Ų�Ʒ����״̬ʧ��[%s]",vGrpIdNo);
			pubLog_Debug(_FFL_, service_name, "", "���¼��Ų�Ʒ����״̬ʧ��dGrpUserMsg[%s]",vGrpIdNo);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -16;
		}
		NG��ź*/
 		/*ng����  by magang at 20090902 begin*/
 		init(i_parameter_array);
		strcpy(i_parameter_array[0],vOpTime);
		strcpy(i_parameter_array[1],vGrpIdNo);
		v_ret=0;
		v_ret=OrderUpdateGrpUserMsg("2",vGrpIdNo,100,vOpCode,lLoginAccept,vLoginNo,vOp_Note,vGrpIdNo,
									"run_code = 'a',op_time = to_date(:vOpTime, 'YYYYMMDD HH24:MI:SS')","",i_parameter_array);

			if(0!=v_ret)
			{
				strcpy(return_code, "352315");
				sprintf(return_mess, "���¼��Ų�Ʒ����״̬ʧ��[%s][%d]",vGrpIdNo,SQLCODE);
				pubLog_Debug(_FFL_, service_name, "", "���¼��Ų�Ʒ����״̬ʧ��dGrpUserMsg[%s]",vGrpIdNo);
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
				return -16;
			}
 		/*ng����  by magang at 20090902 END*/
		/* ��¼�������ʷ��Ϣ */
		memset(dynStmt, 0, sizeof(dynStmt));
		sprintf(dynStmt, "insert into  dGrpSrvMsgHis "
		"(ID_NO, PRODUCT_CODE, SERVICE_TYPE, SERVICE_CODE, PRICE_CODE, BEGIN_TIME, END_TIME, MAIN_FLAG, USE_FLAG, UNVAILD_TIME, PERSON_FLAG, LOGIN_ACCEPT, LOGIN_NO, OP_CODE, OP_TIME, "
		" UPDATE_ACCEPT, UPDATE_TIME, UPDATE_LOGIN, UPDATE_TYPE, UPDATE_CODE, UPDATE_DATE)"
		" select "
		" ID_NO, PRODUCT_CODE, SERVICE_TYPE, SERVICE_CODE, PRICE_CODE, BEGIN_TIME, END_TIME, MAIN_FLAG, USE_FLAG, UNVAILD_TIME, PERSON_FLAG, LOGIN_ACCEPT, LOGIN_NO, OP_CODE, OP_TIME, "
		" :v1, to_date(:v2,'yyyymmdd hh24:mi:ss'), :v3, 'U', :v4, :v5 "
		" from dGrpSrvMsg "
		" where id_no = to_number(:v6) "
		" and end_time >= to_date(:v7, 'YYYYMMDD')");
		EXEC SQL PREPARE prepare1 From :dynStmt;
		EXEC SQL EXECUTE prepare1 using :lLoginAccept,:vOpTime,:vLoginNo,:vOpCode,:vTotal_Date,:vGrpIdNo,:nextMonthFirst;
		if(SQLCODE != 0){
			strcpy(return_code, "352315");
			sprintf(return_mess, "���¼��Ų�Ʒ����״̬ʧ��[%s]",vGrpIdNo);
			pubLog_Debug(_FFL_, service_name, "", "���¼��Ų�Ʒ����״̬ʧ��dGrpUserMsg[%s]",vGrpIdNo);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -17;
		}

		memset(dynStmt, 0, sizeof(dynStmt));
		sprintf(dynStmt, "update dGrpSrvMsg "
			" set end_time = :v1||' 00:00:00', "
			" login_no = :v2, "
			" login_accept = :v3, "
			" op_code = :v4, "
			" op_time = to_date(:v5, 'YYYYMMDD HH24:MI:SS') "
			" where id_no =:v6 "
			" and to_date(substr(end_time,1,8), 'yyyymmdd') >= to_date(:v7, 'YYYYMMDD')");
		EXEC SQL PREPARE prepare1 From :dynStmt;
		EXEC SQL EXECUTE prepare1 using :nextMonthFirst,:vLoginNo,:lLoginAccept,:vOpCode,:vOpTime,:vGrpIdNo,:nextMonthFirst;
		if(SQLCODE != 0 && SQLCODE != 1403){
			strcpy(return_code, "352315");
			sprintf(return_mess, "���¼��Ų�Ʒ����״̬ʧ��[%s]",vGrpIdNo);
			pubLog_Debug(_FFL_, service_name, "", "���¼��Ų�Ʒ����״̬ʧ��dGrpUserMsg[%s]",vGrpIdNo);
			PUBLOG_DBDEBUG(service_name);
			pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code,return_mess);
			return -18;
		}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,service_name,"","vGrpSmCode = [%s]",vGrpSmCode);
#endif

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "����fpubdelmotivemsgִ�н���");
#endif

	return 0;
}

int fmotivepubDelGrpUser(char *inGrpId_No, char *login_no, char *op_code, char *op_time,
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
			/*NG��ź���Ӳ���*/
			int	v_ret=0;
			char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
			TdCustConMsgIndex oldIndex;
			TdCustMsgDead itdCustMsgDead;

		EXEC SQL END DECLARE SECTION;

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

	/*������ֵ*/
	lLoginAccept = lSysAccept;
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "call publicGrp.cp--------fubDelGrpUser");
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
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "lLoginAccept  =[%d]",   lLoginAccept  );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "strLoginAccept=[%s]",   strLoginAccept);
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vGrpId_No     =[%s]",   vGrpId_No     );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vLogin_No     =[%s]",   vLogin_No     );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vOp_Code      =[%s]",   vOp_Code      );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vOp_Time      =[%s]",   vOp_Time      );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vOp_Note      =[%s]",   vOp_Note      );
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "vTotal_Date   =[%s]",   vTotal_Date   );
	#endif

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "��ѯdGrpUserMsg����Ϣ");
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
	if(SQLCODE == 1403)
	{
		sprintf(return_message,"�����û�[%s]������",vGrpId_No);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190101",return_message);
		return 190101;
	}
	else if(SQLCODE!=OK)
	{
		sprintf(return_message,"��ѯdGrpUserMsg����Ϣ[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190102",return_message);
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
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190103",return_message);
	 		return 190103;
		}

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
			pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "���� endGrpProductService ʧ�ܣ�retCode[%d]", retCode);
			return retCode;
		}
#else
		if ( (retCode=cancelAllBill(vGrpId_No, vGrpUser_No, vLastMonth1, vOp_Code, vLogin_No, strLoginAccept, "dGrpSrvMsg", return_message)) != 0)
		{
			pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "����cancelAllBillʧ�ܣ�retCode[%d]", retCode);
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
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190108",return_message);
			return 190108;
		}

		EXEC SQL INSERT INTO wAccountIdInfoChg
			(MSISDN,SERVICE_TYPE,SERVICE_NO,UPDATE_TYPE,RUN_CODE,OP_TIME)
			SELECT
			MSISDN, SERVICE_TYPE, SERVICE_NO, 'D', 'Aa',to_date(:vOp_Time, 'YYYYMMDD HH24:MI:SS')
		FROM dAccountIdInfo
		WHERE msisdn = :vGrpUser_No;
		if (SQLCODE != 0)
		{
			sprintf(return_message, "�����wAccountIdInfoChg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190109",return_message);
			return 190109;
		}
		/*NG��ź
		**EXEC SQL DELETE FROM dAccountIdInfo WHERE msisdn = :vGrpUser_No;
		**if (SQLCODE != 0 && SQLCODE != 1403) {
		**sprintf(return_message, "ɾ��dAccountIdInfo�����![%d][%s]", SQLCODE, SQLERRMSG);
		**PUBLOG_DBDEBUG("fpubDelGrpUser");
		**pubLog_DBErr(_FFL_,"fpubDelGrpUser","190110",return_message);
		**return 190110;
		**}
	NG��ź*/
	/*ng����  by magang at 20090904 begin*/
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderDeleteAccountIdInfo !");
	init(i_parameter_array);
	strcpy(i_parameter_array[0],vGrpUser_No);
	v_ret=0;
	v_ret=OrderDeleteAccountIdInfo("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,vGrpUser_No,"",i_parameter_array);

	if(0!=v_ret && 1!=v_ret)
	{
		sprintf(return_message, "ɾ��dAccountIdInfo�����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190110",return_message);
		return 190110;
	}
	/*ng����  by magang at 20090904 end*/
	/*����Ƿ��м��ſͻ��������û����ø��ʻ�*/
	AcctFlag = 0;
	EXEC SQL SELECT Count(*) INTO :AcctFlag
		FROM dGrpUserMsg
		WHERE Cust_Id = :vGrpCust_Id AND Account_Id = :vContract_No;
	if (SQLCODE != 0)
	{
		sprintf(return_message, "��ѯdGrpUserMsg���ʻ�[%s]ʧ��![%d]", vContract_No, SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190111",return_message);
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
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190112",return_message);
			return 190112;
		}

#if PROVINCE_RUN != PROVINCE_JILIN
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
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����dConMsgDead ����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190113",return_message);
			return 190113;
		}

/* �˵���ѯ�������˻� */
		/*NG��ź
		**EXEC SQL DELETE FROM dConMsg WHERE contract_no = to_number(:vContract_No);
		**if (SQLCODE != 0)
		**{
		**    sprintf(return_message, "ɾ��dConMsg ����![%d][%s]", SQLCODE, SQLERRMSG);
		**	PUBLOG_DBDEBUG("fpubDelGrpUser");
		**	pubLog_DBErr(_FFL_,"fpubDelGrpUser","190114",return_message);
		**    return 190114;
		**}
		**NG��ź*/
	/*ng����  by magang at 20090904 begin*/
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderDeleteConMsg !");
	Trim(vContract_No);
	init(i_parameter_array);
	strcpy(i_parameter_array[0],vContract_No);
	v_ret=0;
	v_ret=OrderDeleteConMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,vContract_No,"",i_parameter_array);

	if(0!=v_ret)
	{
		sprintf(return_message, "ɾ��dConMsg ����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190114",return_message);
		return 190114;
	}
	/*ng����  by magang at 20090904 end*/
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
	PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190115",return_message);
	return 190115;
	}
		/*NG��ź
		**EXEC SQL DELETE FROM dCustConMsg
		**  WHERE cust_id = to_number(:vGrpCust_Id) AND contract_no = to_number(:vContract_No);
		**if (SQLCODE != 0 && SQLCODE != 1403)
		**{
		**    sprintf(return_message, "ɾ��dCustConMsg����![%d][%s]", SQLCODE, SQLERRMSG);
		**	PUBLOG_DBDEBUG("fpubDelGrpUser");
		**	pubLog_DBErr(_FFL_,"fpubDelGrpUser","190116",return_message);
		**    return 190116;
		**}
		**NG��ź*/
		/*ng����  by magang at 20090904 begin*/
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderDeleteCustConMsg !");
		Trim(vContract_No);
		Trim(vGrpCust_Id);
		memset(&oldIndex,0,sizeof(TdCustConMsgIndex));
		strcpy(oldIndex.sCustId,vGrpCust_Id);
		strcpy(oldIndex.sContractNo,vContract_No);

		init(i_parameter_array);
		strcpy(i_parameter_array[0],vGrpCust_Id);
		strcpy(i_parameter_array[1],vContract_No);
		v_ret=0;
		v_ret=OrderDeleteCustConMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex,"",i_parameter_array);

		if(0!=v_ret &&1!=v_ret  )
		{
			sprintf(return_message, "ɾ��dCustConMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190116",return_message);
			return 190116;
		}
		/*ng����  by magang at 20090904 end*/
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
		if(SQLCODE != 0)
		{
			sprintf(return_message, "����dCustUserHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190117",return_message);
			return 190117;
		}

	EXEC SQL DELETE FROM dCustUserMsg WHERE id_no=to_number(:vGrpId_No);
	if (SQLCODE != 0 && SQLCODE != 1403) {
		sprintf(return_message, "ɾ��dCustUserMsg����![%d][%s]", SQLCODE, SQLERRMSG);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190118",return_message);
		return 190118;
	}

		isBillFlag=0;
		EXEC SQL SElECT count(*) INTO :isBillFlag
		FROM sProductAttr
		WHERE product_attr = (select product_attr from sProductCode where product_code = :vProduct_Code)
		AND isbill = 'Y';

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
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����dCustMsgHis����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190119",return_message);
			return 190119;
		}
	/*NG��ź
	**EXEC SQL INSERT INTO dCustMsgDead
	**  (
	**          ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**          ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**          RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
	**          BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
	**  )
	**  SELECT
	**          ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
	**          ATTR_CODE,USER_PASSWD,BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
	**          RUN_CODE,OPEN_TIME,RUN_TIME,GROUP_ID,GROUP_NO,
	**          BILL_DATE,BILL_TYPE,ENCRYPT_PREPAY,     CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD
	**   FROM dCustMsg
	**  WHERE id_no = to_number(:vGrpId_No);
	**if(SQLCODE != 0)
	**{
	**        sprintf(return_message, "����dCustMsgDead����![%d][%s]", SQLCODE, SQLERRMSG);
	**		PUBLOG_DBDEBUG("fpubDelGrpUser");
	**		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190120",return_message);
	**        return 190120;
	**            }
	** NG��ź*/
	/*ng����  by magang at 20090904 begin*/
	memset(&itdCustMsgDead,0,sizeof(TdCustMsgDead));
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderInsertCustMsgDead !");
	EXEC SQL SELECT
			ID_NO,CUST_ID,CONTRACT_NO,IDS,PHONE_NO,SM_CODE,SERVICE_TYPE,
			ATTR_CODE,USER_PASSWD,to_char(OPEN_TIME,'YYYYMMDD HH24:MI:SS'),BELONG_CODE,LIMIT_OWE,CREDIT_CODE,CREDIT_VALUE,
			RUN_CODE,to_char(RUN_TIME,'YYYYMMDD HH24:MI:SS'), to_char(BILL_DATE,'YYYYMMDD HH24:MI:SS'),BILL_TYPE,
			ENCRYPT_PREPAY,CMD_RIGHT,LAST_BILL_TYPE,BAK_FIELD,nvl(GROUP_ID,chr(0)),nvl(GROUP_NO,chr(0))
		INTO  :itdCustMsgDead
		FROM dCustMsg
		WHERE id_no = to_number(:vGrpId_No);
		if(SQLCODE != 0)
		{
			sprintf(return_message, "��ѯdCustMsg��ʧ��![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190120",return_message);
			return 190120;
		}
		v_ret=0;
		v_ret=OrderInsertCustMsgDead("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,itdCustMsgDead);

		if(0!=v_ret)
		{
			sprintf(return_message, "����dCustMsgDead����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190120",return_message);
			return 190120;
		}

	/*ng����  by magang at 20090904 end*/
	/*NG��ź
		**EXEC SQL DELETE FROM dCustMsg WHERE id_no=:vGrpId_No;
		**if(SQLCODE != 0 && SQLCODE != 1403)
		**{
		**        sprintf(return_message, "ɾ��dCustMsg����![%d][%s]", SQLCODE, SQLERRMSG);
		**		PUBLOG_DBDEBUG("fpubDelGrpUser");
		**		pubLog_DBErr(_FFL_,"fpubDelGrpUser","190121",return_message);
		**        return 190121;
		**}
	 NG��ź*/
	/*ng����  by magang at 20090904 begin*/
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderDeleteCustMsg !");
		init(i_parameter_array);
		strcpy(i_parameter_array[0],vGrpId_No);
		v_ret=0;
		v_ret=OrderDeleteCustMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,vGrpId_No,"",i_parameter_array);

		if(0!=v_ret && 1!=v_ret)
		{
			sprintf(return_message, "ɾ��dCustMsg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190121",return_message);
			return 190121;
		}
	/*ng����  by magang at 20090904 end*/
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
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����wUserMsgChg����![%d][%s]", SQLCODE, SQLERRMSG);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190122",return_message);
			return 190122;
		}

			EXEC SQL SELECT count(member_id) into :vCnt
			FROM dGrpUserMebMsg
			WHERE Id_No = To_Number(:vGrpId_No) AND End_Time > Last_Day(sysdate) + 1;
			if (SQLCODE != 0)
			{
				sprintf(return_message, "��ѯdGrpUserMebMsg�����![%d][%s]", SQLCODE, SQLERRMSG);
				PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
				pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190123",return_message);
				return 190123;
			}
			if (vCnt > 0)
			{
				sprintf(return_message, "��ǰ�����г�Ա������������![%d][%s]", SQLCODE, SQLERRMSG);
				return 190124;
			}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "ɾ��dGrpUserMsg����Ϣ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "INSERT INTO dGrpUserMsgHis ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "   (CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     UPDATE_LOGIN, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME,   UPDATE_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    UPDATE_TYPE) ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", " SELECT ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    CUST_ID,     ID_NO,         ACCOUNT_ID,    USER_NO, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    IDS,         USER_NAME,     PRODUCT_TYPE,  PRODUCT_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    USER_PASSWD, LOGIN_NAME,    LOGIN_PASSWD,  PROV_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    REGION_CODE, DISTRICT_CODE, TOWN_CODE,     TERRITORY_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    LIMIT_OWE,   CREDIT_CODE,   CREDIT_VALUE,  RUN_CODE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    OLD_RUN,     RUN_TIME,      BILL_DATE,     BILL_TYPE, ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    LAST_BILL_TYPE, OP_TIME,    BAK_FIELD,     '%s', ", vLogin_No);
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    %d, '%s', to_date('%s', 'YYYYMMDD HH24:MI:SS'), '%s', ", lLoginAccept, vTotal_Date, vOp_Time, vOp_Code);
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    'D' ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "   FROM dGrpUserMsg ");
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "  WHERE ID_NO = To_Number('%s') ", vGrpId_No);
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "    AND Bill_Date > Last_Day(sysdate) + 1; ");
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
	if(SQLCODE!=OK)
	{
		sprintf(return_message,"����dGrpUserMsgHis���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190127",return_message);
		return 190127;
	}

	/*ֻҪbill_date��������1�գ�����Ϊ���Ѿ������ļ����û�*/
	/*NG��ź
	**EXEC SQL UPDATE dGrpUserMsg
	**  SET old_run = run_code, run_code = 'I', bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1
	**  WHERE id_no = To_Number(:vGrpId_No) AND Bill_Date > Last_Day(sysdate) + 1;
	**if(SQLCODE!=OK){
	**    sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
	**  	PUBLOG_DBDEBUG("fpubDelGrpUser");
	**	pubLog_DBErr(_FFL_,"fpubDelGrpUser","190128",return_message);
	**    return 190128;
	**}
	**NG��ź*/
	/*ng����  by magang at 20090904 begin*/
	pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","","begin call OrderUpdateGrpUserMsg !");
		init(i_parameter_array);
		strcpy(i_parameter_array[0],vTotal_Date);
		strcpy(i_parameter_array[1],vGrpId_No);
		v_ret=0;
		v_ret=OrderUpdateGrpUserMsg("2",vGrpId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,vGrpId_No,
									"old_run = run_code, run_code = 'I', bill_date = Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1",
									"AND Bill_Date > Last_Day(sysdate) + 1",i_parameter_array);

		if(0!=v_ret)
		{
			 sprintf(return_message,"ɾ��dGrpUserMsg���Ϸ����쳣[%d]",SQLCODE);
			PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
			pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190128",return_message);
			return 190128;
		}
	/*ng����  by magang at 20090904 end*/

	EXEC SQL DELETE FROM dGrpUserMsgAdd WHERE id_no = To_Number(:vGrpId_No);
	if(SQLCODE!=OK && SQLCODE != 1403)
	{
		sprintf(return_message,"ɾ��dGrpUserMsgAdd���Ϸ����쳣[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDelGrpUser");
		pubLog_DBErr(_FFL_,"fmotivepubDelGrpUser","190129",return_message);
	return 190129;
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "���ú������Լ����û��ʻ��ĸ��Ѽƻ����б��");
#endif
	retCode = fmotivepubDisConUserMsg(vGrpId_No, vGrpUser_No, vContract_No, vLogin_No, vOp_Code, vTotal_Date, lLoginAccept, op_note, return_message);
	if (retCode != 0)
	{
		pubLog_Debug(_FFL_, "fmotivepubDelGrpUser", "", "����fmotivepubDisConUserMsg��������,Id_No[%s],Account_Id[%s],retCode[%d]", vGrpId_No, vContract_No, retCode);
		return 190130;
	}

	return 0;
}

int fmotivepubDisConUserMsg(char *inId_No, char *srvNo, char *inContract_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message)
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
	/*NG��ź���Ӳ���*/
	int	v_ret=0;
	char i_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	TdConUserMsgIndex oldIndex;
	TdConUserMsgIndex newIndex;
	EXEC SQL END DECLARE SECTION;

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
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "��ˮ=%ld ",lSysAccept);
	strcpy(vId_No,       inId_No       );
	strcpy(vContract_No, inContract_No );
	strcpy(vLogin_No,    login_no      );
	strcpy(vOp_Code,     op_code       );
	strcpy(vTotal_Date,  total_date    );
	strcpy(vOp_Note,     op_note       );

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "insert into dConUserMsgHis");
	#endif
	EXEC SQL INSERT INTO dConUserMsgHis
		(Id_No,         Contract_No,  Serial_No,
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

	if (SQLCODE != 0)
	{
		sprintf(return_message, "����dConUserMsgHis����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191001",return_message);
		return 191001;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "������Ϣ��dGrpProductHis��");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "INSERT INTO dConUserMsgHis ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", " (   Id_No,         Contract_No,  Serial_No,  ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Bill_Order,    Pay_Order,    Begin_YMD,  ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Begin_TM,      End_YMD,      End_TM, ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Update_Accept, Update_Login, Update_Date, ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Update_Time,   Update_Code,  Update_Type  ) ");

	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", " SELECT ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", " Id_No,         Contract_No,  Serial_No, ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Bill_Order,    Pay_Order,    Begin_YMD, ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", " Begin_TM,      End_YMD,      End_TM, ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  Limit_Pay,     Rate_Flag,    Stop_Flag,  ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  '%ld', '%s', to_number('%s'), ", lLoginAccept, vLogin_No, vTotal_Date);
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  to_date(),'%s','U' ", vOp_Code);
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", " FROM dConUserMsg ");
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "WHERE Id_No = To_Number('%s') ", vId_No);
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "  AND Contract_No = to_number('%s'); ", vContract_No);
#endif

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "��ѯ����SerialNo");
	#endif
	EXEC SQL SELECT Nvl(Max(Serial_No), 0)
			INTO :lMaxSerialNo
			FROM dConUserMsg
			WHERE Id_No = To_Number(:vId_No)
			AND Contract_No = To_Number(:vContract_No);
	if (SQLCODE != 0)
	{
		sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191002",return_message);
		return 191002;
	}

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "��ѯdCustMsg�Ƿ��Ǹ����û�");
	#endif
	EXEC SQL SELECT Count(*) INTO :vcnt FROM dCustMsg
		WHERE Id_No = To_Number(:vId_No);
	if (SQLCODE != 0)
	{
		sprintf(return_message, "��ѯdCustMsg�Ƿ��Ǹ����û�ʧ��![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
		pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191003",return_message);
		return 191003;
	}

	#ifdef _DEBUG_
	 pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "update dConUserMsg");
	#endif
/**
	**EXEC SQL UPDATE dConUserMsg
	**            SET Serial_No = :lMaxSerialNo + 1,
	**                End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD')
	**          WHERE Id_No = To_Number(:vId_No)
	**            AND Contract_No = To_Number(:vContract_No)
	**            AND Serial_No = 0
	**            AND END_YMD > To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD');
 **/
 	/*NG��ź
	**EXEC SQL UPDATE dConUserMsg
	**            SET End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),
	**            	End_TM='000000'
	**          WHERE Id_No = To_Number(:vId_No)
	**            AND Contract_No = To_Number(:vContract_No)
	**            AND Serial_No = 0;
	**if (SQLCODE != 0 && SQLCODE != 1403) {
	**    sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
	**  	PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
	**	pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191004",return_message);
	**    return 191004;
	**}
	NG��ź*/
	/*ng����  by magang at 20090902 begin*/
		memset(&oldIndex,0,sizeof(TdConUserMsgIndex));
		memset(&newIndex,0,sizeof(TdConUserMsgIndex));
		strcpy(oldIndex.sIdNo,vId_No);
		strcpy(oldIndex.sContractNo,vContract_No);
		strcpy(oldIndex.sSerialNo,"0");
		strcpy(newIndex.sIdNo,vId_No);
		strcpy(newIndex.sContractNo,vContract_No);
		strcpy(newIndex.sSerialNo,"0");

 		init(i_parameter_array);
 		strcpy(i_parameter_array[0],vTotal_Date);
		strcpy(i_parameter_array[1],vId_No);
		strcpy(i_parameter_array[2],vContract_No);
		strcpy(i_parameter_array[3],"0");
		v_ret=OrderUpdateConUserMsg("2",vId_No,100,vOp_Code,lLoginAccept,vLogin_No,vOp_Note,oldIndex,newIndex,
									"End_YMD = To_Char(Last_Day(To_Date(:vTotal_Date, 'YYYYMMDD')) + 1, 'YYYYMMDD'),End_TM='000000'","",i_parameter_array);

			if(0!=v_ret && 1!=v_ret)
			{
				sprintf(return_message, "����dConUserMsg����![%d]",SQLCODE);
				PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
				pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191004",return_message);
				return 191004;
			}
	/*ng����  by magang at 20090902 end*/
	/* yangzh disabled 20050124 ����dCustMsg�����,�Ͳ�����wConUserChg*/
	if ((SQLROWS == 1) && (vcnt == 1))
	{
		/*����û����Ѽƻ������ڣ��������Update���ִ�лᷢ������
		�������û��ĸ��Ѽƻ���û�з����������������wConUserChg��¼
		OP_TYPE��10-���ӣ�20-�޸ģ�30-ɾ��
		*/
	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fmotivepubDisConUserMsg", "", "insert into wConUserChg");
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
		if (SQLCODE != 0)
		{
			sprintf(return_message, "����wConUserChg����![%d]",SQLCODE);
		 	PUBLOG_DBDEBUG("fmotivepubDisConUserMsg");
			pubLog_DBErr(_FFL_,"fmotivepubDisConUserMsg","191005",return_message);
			return 191005;
		}
	}

	return 0;
}

int InsertLogin(char *vDateChar,char *vtotal_date,char *pLogin_Accept, char *pCustId,
				char *pOrgCode,char *pGrpId, char *pWorkNo, char *pOpCode,
				char *pOperNote, char *return_msg)
{
	int retCode=0;
	char TmpSqlStr[1024];
	int ret=0;
	tLoginOpr vLog;

	EXEC SQL BEGIN DECLARE SECTION;

	char vOrgId[10+1];
	char vGroupId[10+1];
	char vChgFlag[1+1];
	char vCustFlag[1+1];
	char vSysDate[17+1];

	EXEC SQL END DECLARE SECTION;

	memset(vOrgId,0,sizeof(vOrgId));
	memset(vGroupId,0,sizeof(vGroupId));
	memset(vChgFlag,0,sizeof(vChgFlag));
	memset(vCustFlag,0,sizeof(vCustFlag));
	memset(vSysDate,0,sizeof(vSysDate));
	memset(&vLog,0,sizeof(vLog));


	/*--��֯���������ȡorg_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWorkNo,vOrgId);
	if(ret <0)
	{
		strcpy(return_msg,"��ѯ����org_idʧ��!");
		pubLog_DBErr(_FFL_,"", "320003", return_msg);
		return 320003;
	}
	Trim(vOrgId);
	/*---��֯���������ȡorg_id edit by magang at 16/04/2009---end*/

	EXEC SQL SELECT TO_CHAR(SYSDATE,'YYYYMMDDHH24MISS')
				INTO :vSysDate
				FROM dual;
	if(SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320003", return_msg);
		return 320004;
	}
	Trim(vSysDate);
		vLog.vId_No                            =  atol(pGrpId);
		vLog.vTotal_Date                       =   atoi(vtotal_date);
		vLog.vLogin_Accept                     =  atol(pLogin_Accept);

		strncpy(vLog.vSm_Code,       "gn",         2);
		strncpy(vLog.vBelong_Code, pOrgCode,      7);
		strncpy(vLog.vPhone_No,       pGrpId,       15);
		strncpy(vLog.vOrg_Code,    pOrgCode,         9);
		strncpy(vLog.vLogin_No,       pWorkNo,        6);
		strncpy(vLog.vOp_Code,       pOpCode,          4);
		strncpy(vLog.vOp_Time,       vSysDate,        17);
		strncpy(vLog.vMachine_Code,     "000",         3);
		strncpy(vLog.vBack_Flag,         "0",          1);
		strncpy(vLog.vEncrypt_Fee,       "0",         16);
		strncpy(vLog.vSystem_Note,   pOperNote,          60 );
		strncpy(vLog.vOp_Note,          pOperNote,       60);
		strncpy(vLog.vGroup_Id,       vOrgId,        10);
		strncpy(vLog.vOrg_Id,         vOrgId,          10);
		strncpy(vLog.vPay_Type,       "0",              1);
		strncpy(vLog.vIp_Addr,      "",         15);
		vLog.vPay_Money                      = 0;
		vLog.vCash_Pay                       = 0;
		vLog.vCheck_Pay      					  = 0;
		vLog.vSim_Fee            				  = 0;
		vLog.vMachine_Fee        				  = 0;
		vLog.vInnet_Fee          				  = 0;
		vLog.vChoice_Fee         				  = 0;
		vLog.vOther_Fee           				  = 0;
		vLog.vHand_Fee           		  = 0;
		vLog.vDeposit            				  = 0;

		ret = 0;
		strncpy(vChgFlag,        "Y",                   1);
		strncpy(vCustFlag,       "Y",                   1);


		ret = recordOprLog(&vLog,return_msg,vChgFlag,vCustFlag);
		if(ret != 0)
		{
			pubLog_DBErr(_FFL_,"", "%06d",return_msg,ret);
			return ret;
		}
	return retCode;
}

int InsertLoginExtend(char *vDateChar,char *vtotal_date,char *pLogin_Accept, char *pCustId,
					char *pOrgCode,char *pGrpId, char *pWorkNo, char *pOpCode,
					char *pOperNote, char *return_msg,char *sysnote)
{
	int retCode=0;
	char TmpSqlStr[1024];
	/*��֯�ṹ�������ӱ���---magang---20090417--*/
	int ret=0;
	tLoginOpr vLog;
	EXEC SQL BEGIN DECLARE SECTION;

	char vOrgId[10+1];
	char vGroupId[10+1];
	char vChgFlag[1+1];
	char vCustFlag[1+1];
	char vSysDate[17+1];

	EXEC SQL END DECLARE SECTION;

	memset(vOrgId,0,sizeof(vOrgId));
	memset(vGroupId,0,sizeof(vGroupId));
	memset(vChgFlag,0,sizeof(vChgFlag));
	memset(vCustFlag,0,sizeof(vCustFlag));
	memset(vSysDate,0,sizeof(vSysDate));
	memset(&vLog,0,sizeof(vLog));

	/*--��֯���������ȡorg_id edit by magang at 16/04/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(pWorkNo,vOrgId);
	if(ret <0)
	{
		strcpy(return_msg,"��ѯ����org_idʧ��!");
		pubLog_DBErr(_FFL_,"", "%06d",return_msg,ret);
		return ret;
	}
	Trim(vOrgId);

	EXEC SQL SELECT TO_CHAR(SYSDATE,'YYYYMMDDHH24MISS')
				INTO :vSysDate
				FROM dual;
	if(SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"", "320004", return_msg);
		return 320004;
	}
	Trim(vSysDate);
	vLog.vId_No                            =  atol(pGrpId);
	vLog.vTotal_Date                       =   atoi(vtotal_date);
	vLog.vLogin_Accept                     =  atol(pLogin_Accept);

	strncpy(vLog.vSm_Code,       "gn",         2);
	strncpy(vLog.vBelong_Code, pOrgCode,      7);
	strncpy(vLog.vPhone_No,       pGrpId,       15);
	strncpy(vLog.vOrg_Code,    pOrgCode,         9);
	strncpy(vLog.vLogin_No,       pWorkNo,        6);
	strncpy(vLog.vOp_Code,       pOpCode,          4);
	strncpy(vLog.vOp_Time,       vSysDate,        17);
	strncpy(vLog.vMachine_Code,     "000",         3);
	strncpy(vLog.vBack_Flag,         "0",          1);
	strncpy(vLog.vEncrypt_Fee,       "0",         16);
	strncpy(vLog.vSystem_Note,   sysnote,          60 );
	strncpy(vLog.vOp_Note,          pOperNote,       60);
	strncpy(vLog.vGroup_Id,       vOrgId,        10);
	strncpy(vLog.vOrg_Id,         vOrgId,          10);
	strncpy(vLog.vPay_Type,       "0",              1);
	strncpy(vLog.vIp_Addr,      "",         15);
	vLog.vPay_Money                      = 0;
	vLog.vCash_Pay                       = 0;
	vLog.vCheck_Pay      					  = 0;
	vLog.vSim_Fee            				  = 0;
	vLog.vMachine_Fee        				  = 0;
	vLog.vInnet_Fee          				  = 0;
	vLog.vChoice_Fee         				  = 0;
	vLog.vOther_Fee           				  = 0;
	vLog.vHand_Fee           		  = 0;
	vLog.vDeposit            				  = 0;

	ret = 0;
	strncpy(vChgFlag,        "Y",                   1);
	strncpy(vCustFlag,       "Y",                   1);


	ret = recordOprLog(&vLog,return_msg,vChgFlag,vCustFlag);
	if(ret != 0)
	{
		pubLog_DBErr(_FFL_,"", "%06d",return_msg,ret);
		return ret;
	}

	return retCode;
}


int InsertIDCorderinfo(char *vGrpIdNo,char *vProcid,char *vProcesstime,char *vSerialno,char *vCustid,char *vCustname,char *vAddress,
						char *vIsvip,char *vPhone,char *vEmail,char *vGroup_Mgr_Name,char *vGroup_Mgr_Sn,
						char *vCust_Manager_Id,char *vCust_Manager_Name,char *vCust_Manager_Sn,char *vPointid,
						char *vProductinfoidlist,char *vBusinesstype,char *vDomain,char *vDealflag,char *return_msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vSendValue[8+1];
		char vAttrValue[256+1];
	EXEC SQL END DECLARE SECTION;
	memset(vSendValue,0,sizeof(vSendValue));
	memset(vAttrValue,0,sizeof(vAttrValue));


	exec sql insert into  dgrpidcorderinfo (
							PROCID                    ,
							PROCESSTIME               ,
							SERIALNO                  ,
							CUSTID                    ,
							CUSTNAME                  ,
							ADDRESS                   ,
							ISVIP                     ,
							PHONE                     ,
							EMAIL                     ,
							GROUP_MGR_NAME            ,
							GROUP_MGR_SN              ,
							CUST_MANAGER_ID           ,
							CUST_MANAGER_NAME         ,
							CUST_MANAGER_SN           ,
							POINTID                   ,
							BUSINESSTYPE              ,
							DOMAIN                    ,
							RECORDTIME                ,
							DEALTIME                  ,
							DEALFLAG )
							select
							:vProcid,
							:vProcesstime,
							:vSerialno,
							:vCustid,
							:vCustname,
							:vAddress,
							:vIsvip,
							:vPhone,
							:vEmail,
							:vGroup_Mgr_Name,
							:vGroup_Mgr_Sn,
							:vCust_Manager_Id,
							:vCust_Manager_Name,
							:vCust_Manager_Sn,
							:vPointid,
							:vBusinesstype,
							:vDomain,
							sysdate,
							sysdate,
							:vDealflag
							from dual;
	if( 0 != SQLCODE)
	{
		sprintf(return_msg,"����� dgrpincorderinfo ʧ�� [%d]",SQLCODE);
		pubLog_Debug(_FFL_, "InsertIDCorderinfo", "", "%s", return_msg);
		return -1;
	}

	/*�����Ʒ�ӿڱ�*/
	EXEC SQL insert into DGRPIDCORDERPROD(SERIALNO,INFOCODE)
	                               values(:vSerialno,:vProductinfoidlist);
	if( 0 != SQLCODE)
	{
		sprintf(return_msg,"����DGRPIDCORDERPROD��ʧ�� [%d]",SQLCODE);
		pubLog_Debug(_FFL_, "InsertIDCorderinfo", "", "%s", return_msg);
		return -2;
	}

	/*�����Ʒ���Խӿڱ�*/
	EXEC SQL DECLARE attrCur CURSOR for
		 SELECT b.send_value,a.attr_value
		   FROM dgrpsrvmsgadd a, DGRPIDCORDERPRODCFG b
		  WHERE a.attr_code = b.product_attr
			AND a.id_No = to_number(:vGrpIdNo)
			AND SYSDATE BETWEEN begin_time AND end_time;
	EXEC SQL OPEN attrCur;
	for(;;)
	{
		EXEC SQL FETCH attrCur INTO :vSendValue,:vAttrValue;
		if(SQLCODE != 0)
		{
			if(1403 == SQLCODE) break;
			else
			{
				sprintf(return_msg,"��ȡ��Ʒ����ʧ��[%s][%d]",vGrpIdNo,SQLCODE);
				pubLog_Debug(_FFL_, "InsertIDCorderinfo", "", "%s", return_msg);
				EXEC SQL CLOSE attrCur;
				return -3;
			}
		}
		Trim(vSendValue);
		Trim(vAttrValue);

		/*��¼��Ʒ���Խӿڱ�*/
		EXEC SQL insert into DGRPIDCORDERPRODADD(SERIALNO,INFOCODE,PARAMKEY,PARAMVALUE)
	                     values(:vSerialno,:vProductinfoidlist,:vSendValue,:vAttrValue);
		if( 0 != SQLCODE)
		{
			sprintf(return_msg,"��¼��Ʒ���Խӿڱ�[%d]",SQLCODE);
			pubLog_Debug(_FFL_, "InsertIDCorderinfo", "", "%s", return_msg);
			EXEC SQL CLOSE attrCur;
			return -3;
		}
	}
	EXEC SQL CLOSE attrCur;
	return 0;

}
/************************************************************************
��������:fpubChgBbossMode
����ʱ��:2013/4/16
������Ա:chendx
��������:ȫ����ʡ֧������Ʒ�ײͱ��
�������:�����û�id
         ���ײ�
         ���ײ�
         ��������
         ��������
         ������ˮ��
         ������ע
�������:�������
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fpubChgBbossMode(char *grp_id,char *old_mode,char *new_mode,char *mode_flag,char *login_no, char *op_code, char *op_time,long lSysAccept, char *op_note)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vProductCode[8+1];		/*��Ʒ����*/
		char    vGrpId[14+1];          	/*�����û�ID*/
		char  	vOldModeCode[8+1];		/*���ײ�*/
		char  	vNewModeCode[8+1];		/*���ײ�*/
		char    vModeFlag[1+1];			/*�ײ���Ч��ʽ 0:������Ч 1:ԤԼ��Ч*/
		char    vGrpUserNo[15+1];      	/*�����û����,�ڲ�����*/
		char    vServiceType[1+1];	   	/*�Ż�����*/
		char    vSmCode[2+1];          	/*��ƷƷ��*/
		char    vRegionCode[2+1];      	/*��������*/
		char    vOpTime[17+1];         	/*����ʱ��*/
		char    nextMonthFirst[17+1];	/*����һ��*/
		char    vStartTime[17+1];		/*���ײ�ʧЧʱ��/���ײ���Чʱ��*/
		int		vFavOrder = 0;

		int		ret=0;
		
		long	lLoginAccept=0;
		char	vLoginNo[6+1];
		char	vOpCode[4+1];
		char	vOpNote[60+1];
		int 	vTotalDate = 0;
		char	vServiceCode[4+1];
		char	vMainFlag[1+1];
		char	vPriceCode[4+1];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		char v_update_sql[500+1];
    	TdBaseFavIndex vOldIndex;
	EXEC SQL END DECLARE SECTION;
	TdBaseFav tdBaseFav;

    init(vProductCode);
    init(vOldModeCode);
    init(vNewModeCode);
    init(vModeFlag);
    init(vGrpId);
    init(vGrpUserNo);
    init(vServiceType);
    init(vSmCode);
    init(vRegionCode);
    init(vOpTime);
    init(nextMonthFirst);
    init(vStartTime);
    init(vLoginNo);
    init(vOpCode);
    init(vOpNote);
    init(vServiceCode);
    init(vMainFlag);
    init(vPriceCode);
    init(v_parameter_array);
	init(v_update_sql);

    printf("++++++++ fpubChgBbossMode begin success ++++++++\n");

    lLoginAccept = lSysAccept;
    strcpy(vGrpId,grp_id);
    strcpy(vOldModeCode,old_mode);
    strcpy(vNewModeCode,new_mode);
    strcpy(vLoginNo,login_no);
    strcpy(vOpCode,op_code);
    strcpy(vOpNote,op_note);
    strcpy(vOpTime,op_time);
    strcpy(vModeFlag,mode_flag);

    EXEC SQL SELECT to_char(add_months(to_date(substr(:vOpTime,1,6),'yyyymm'), 1),'yyyymm')||'01 00:00:00',
                    to_number(substr(:vOpTime,1,8))
  		into :nextMonthFirst,:vTotalDate
  	from dual;
  	if(strcmp(vModeFlag,"0") == 0)
  	{
  		strcpy(vStartTime,vOpTime);
  	}
  	if(strcmp(vModeFlag,"1") == 0)
  	{
  		strcpy(vStartTime,nextMonthFirst);
  	}
  	pubLog_Debug(_FFL_,"fpubChgBbossMode","","vStartTime[%s]",vStartTime);
	
	/*ȡ�����ײ���Ϣ*/
	EXEC SQL DECLARE Old_Cur CURSOR for
    	select service_type, srv_code, 'Y', 0,price_code 
    	  from sproductspecsrvcfg 
    	 where mode_code = :vOldModeCode;
    EXEC SQL OPEN Old_Cur;
    while(1)
    {
    	Sinitn(vServiceType);
		Sinitn(vServiceCode);
		Sinitn(vMainFlag);
		Sinitn(vPriceCode);
		EXEC SQL FETCH Old_Cur INTO :vServiceType, :vServiceCode, :vMainFlag, :vFavOrder, :vPriceCode;
		if(SQLCODE != 0)
		{
			if(SQLCODE == 1403)
			{
				printf("û������\n");
				break;
			}
			pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ���ײͷ���������[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE Old_Cur;
			return -1;
		}
		Trim(vServiceType);
		Trim(vServiceCode);
		Trim(vMainFlag);
		Trim(vPriceCode);
		
		pubLog_Debug(_FFL_,"fpubChgBbossMode","","vServiceType[%s]",vServiceType);
		pubLog_Debug(_FFL_,"fpubChgBbossMode","","vServiceCode[%s]",vServiceCode);
		
		EXEC SQL INSERT INTO dGrpSrvMsgHis
		            (ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		             SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		             MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		             OP_TIME,   use_flag, 	  person_flag,  UNVAILD_TIME,
		             UPDATE_LOGIN,UPDATE_ACCEPT,UPDATE_DATE,UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE)
	          select ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		             SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		             MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		             OP_TIME,   use_flag, 	  person_flag,  UNVAILD_TIME,
		             :vLoginNo,:lLoginAccept,:vTotalDate,to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),:vOpCode,'D'
		        from dgrpsrvmsg 
		       where id_no = to_number(:vGrpId)
		       	 and service_code = :vServiceCode
		       	 and price_code = :vPriceCode
		       	 and end_time > sysdate;
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_,"fpubChgBbossMode","","��¼dgrpsrvmsghis����[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE Old_Cur;
			return -2;
		}
		
		EXEC SQL update dgrpsrvmsg set end_time = to_date(:vStartTime,'yyyymmdd hh24:mi:ss')
			      where id_no = to_number(:vGrpId)
		       	    and service_code = :vServiceCode
		       	 	and price_code = :vPriceCode
		       	 	and end_time > sysdate;
		if(SQLCODE!=0)
		{
        	pubLog_Debug(_FFL_,"fpubChgBbossMode","","�޸�dgrpsrvmsg����[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE Old_Cur;
			return -3;
		}
		
		/*������ڼƷ��Ż����ͣ��޸�dbasefav����Ϣ*/
		if(strcmp(vServiceType,"0") == 0)
		{
			printf("�޸�dBaseFav����Ϣ\n");
			
			 memset(&vOldIndex, 0, sizeof(vOldIndex));
    
		    EXEC SQL select user_no into :vOldIndex.sMsisdn
		    	       from dgrpusermsg
		    	      where id_no = to_number(:vGrpId);
		   	if(SQLCODE != 0)
		   	{
		   		pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ�����û��������[%d]",SQLCODE);
		   		EXEC SQL ROLLBACK;
				EXEC SQL CLOSE Old_Cur;
				return -1;
		   	}
		   	
		   	EXEC SQL insert into wBaseFavChg
						(
						MSISDN, REGION_CODE, FAV_BRAND, FAV_TYPE, FLAG_CODE, 
			            FAV_ORDER, FAV_DAY, START_TIME, END_TIME, FLAG, DEAL_TIME, 
			            SERVICE_ID, FAV_PERIOD, FREE_VALUE, ID_NO, GROUP_ID, PRODUCT_CODE
						)          
				 select
						MSISDN, REGION_CODE, FAV_BRAND, FAV_TYPE, FLAG_CODE, 
						FAV_ORDER, FAV_DAY, START_TIME, to_date(:vStartTime, 'YYYYMMDD HH24:MI:SS'), '2', to_date(:vOpTime, 'YYYYMMDD HH24:MI:SS'),
						SERVICE_ID, FAV_PERIOD, FREE_VALUE, ID_NO, GROUP_ID, PRODUCT_CODE
				   from dBaseFav
				  where msisdn = :vOldIndex.sMsisdn
				  	and fav_type = :vServiceCode
				  	and flag_code = :vServiceCode
					and end_time >= to_date(:vOpTime, 'yyyymmdd hh24:mi:ss');
   			if(SQLCODE != 0)
		   	{
		   		pubLog_Debug(_FFL_,"fpubChgBbossMode","","��¼�Ʒ��Ż�С�����[%d]",SQLCODE);
		   		EXEC SQL ROLLBACK;
				EXEC SQL CLOSE Old_Cur;
				return -2;
		   	}
		   	
			EXEC SQL select start_time into :vOldIndex.sStartTime
				       from dbasefav
				      where msisdn = :vOldIndex.sMsisdn
				        and fav_type = :vServiceCode
				        and flag_code = :vServiceCode
				        and end_time > to_date(:vOpTime,'yyyymmdd hh24:mi:ss');
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ�Ż���Ϣ����[%d]",SQLCODE);
				EXEC SQL ROLLBACK;
				EXEC SQL CLOSE Old_Cur;
				return -3;
			}
			
  			strcpy(vOldIndex.sFavType,vServiceCode);
  			strcpy(vOldIndex.sFlagCode,vServiceCode);
			
			init(v_update_sql);
			init(v_parameter_array);
			strcpy(v_update_sql,"end_time = to_date(:v1,'yyyymmdd hh24:mi:ss')");
			strcpy(v_parameter_array[0],vStartTime);			
			strcpy(v_parameter_array[1],vOldIndex.sMsisdn);
			strcpy(v_parameter_array[2],vOldIndex.sFavType);
			strcpy(v_parameter_array[3],vOldIndex.sFlagCode);
			strcpy(v_parameter_array[4],vOldIndex.sStartTime);
			ret = 0;
			ret = OrderUpdateBaseFav("2",vGrpId,100,vOpCode,lLoginAccept,vLoginNo,vOpNote,vOldIndex,vOldIndex,v_update_sql,"",v_parameter_array);
			if (ret < 0)
			{
				pubLog_Debug(_FFL_,"fpubChgBbossMode","","�޸ľ��Ż���Ϣ����[%d]",ret);
				EXEC SQL ROLLBACK;
				EXEC SQL CLOSE Old_Cur;
				return -3;
			}
		}
	}
    EXEC SQL CLOSE Old_Cur;
    
    /*��¼���ײ���Ϣ*/
    EXEC SQL DECLARE New_Cur CURSOR for
    	select service_type, srv_code, 'Y', 0,price_code 
    	  from sproductspecsrvcfg 
    	 where mode_code = :vNewModeCode;
    EXEC SQL OPEN New_Cur;
    while(1)
    {
    	Sinitn(vServiceType);
		Sinitn(vServiceCode);
		Sinitn(vMainFlag);
		Sinitn(vPriceCode);
		Sinitn(vGrpUserNo);
		Sinitn(vRegionCode);
		Sinitn(vSmCode);
		EXEC SQL FETCH New_Cur INTO :vServiceType, :vServiceCode, :vMainFlag, :vFavOrder, :vPriceCode;
		if(SQLCODE != 0)
		{
			if(SQLCODE == 1403)
			{
				printf("û������\n");
				break;
			}
			pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ���ײͷ���������[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE New_Cur;
			return -1;
		}
		Trim(vServiceType);
		Trim(vServiceCode);
		Trim(vMainFlag);
		Trim(vPriceCode);
		
		EXEC SQL select product_code into :vProductCode
		           from dgrpusermsg
		          where id_no = to_number(:vGrpId);
		if(SQLCODE!=0)
		{
        	pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ��Ʒ�������[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE New_Cur;
			return -2;
		}
		Trim(vProductCode);
		
		EXEC SQL INSERT INTO dGrpSrvMsgHis
		            (ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
		             SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
		             MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
		             OP_TIME,   UPDATE_LOGIN,  UPDATE_ACCEPT,UPDATE_DATE,
		             UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE,use_flag, person_flag, UNVAILD_TIME)
		            VALUES
		            (to_number(:vGrpId),    :vServiceType, :vServiceCode, :vPriceCode,
		             :vFavOrder, :vProductCode, to_date(:vStartTime,'yyyymmdd hh24:mi:ss'), To_Date('20500101', 'YYYYMMDD'),
		             :vMainFlag, :lLoginAccept,  :vLoginNo,  :vOpCode,
		             to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),:vLoginNo,:lLoginAccept,:vTotalDate,
		             to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),:vOpCode,'A', 'Y', 'N',To_Date('20500101', 'YYYYMMDD'));
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_,"fpubChgBbossMode","","��¼dgrpsrvmsghis����[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE New_Cur;
			return -3;
		}
		
		EXEC SQL INSERT INTO dGrpSrvMsg
			        (ID_NO,     SERVICE_TYPE, SERVICE_CODE, PRICE_CODE,
			         SRV_ORDER, PRODUCT_CODE, BEGIN_TIME,   END_TIME,
			         MAIN_FLAG, LOGIN_ACCEPT, LOGIN_NO,     OP_CODE,
			         OP_TIME, use_flag, person_flag, UNVAILD_TIME)
			     VALUES
			         (to_number(:vGrpId),    :vServiceType, :vServiceCode, :vPriceCode,
		             :vFavOrder, :vProductCode, to_date(:vStartTime,'yyyymmdd hh24:mi:ss'), To_Date('20500101', 'YYYYMMDD'),
		             :vMainFlag, :lLoginAccept,  :vLoginNo,  :vOpCode,
		             to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),'Y', 'N',To_Date('20500101', 'YYYYMMDD'));
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_,"fpubChgBbossMode","","��¼dgrpsrvmsg����[%d]",SQLCODE);
			EXEC SQL ROLLBACK;
			EXEC SQL CLOSE New_Cur;
			return -4;
		}
		
		/*������ڼƷ��Ż����ͣ��޸�dbasefav����Ϣ*/
		if(strcmp(vServiceType,"0") == 0)
		{
			printf("��¼dBaseFav����Ϣ\n");
			
			 memset(&vOldIndex, 0, sizeof(vOldIndex));
    
		    EXEC SQL select user_no,region_code,sm_code
		    		   into :vGrpUserNo,:vRegionCode,:vSmCode
		    	       from dgrpusermsg
		    	      where id_no = to_number(:vGrpId);
		   	if(SQLCODE != 0)
		   	{
		   		pubLog_Debug(_FFL_,"fpubChgBbossMode","","��ȡ�����û���Ϣ����[%d]",SQLCODE);
		   		EXEC SQL ROLLBACK;
				EXEC SQL CLOSE New_Cur;
				return -1;
		   	}
		   	Trim(vGrpUserNo);
		   	Trim(vRegionCode);
		   	Trim(vSmCode);
		   	
		   	EXEC SQL INSERT INTO wBaseFavChg
						(msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,fav_day,
						 start_time,end_time,flag,deal_time,id_no)
				  values
						(:vGrpUserNo,:vRegionCode,:vSmCode,:vServiceCode,:vServiceCode,to_number(:vFavOrder),'0',
						 to_date(:vStartTime,'YYYYMMDD HH24:MI:SS'),to_date('20500101 00:00:00','YYYYMMDD HH24:MI:SS'),
						 '1', to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),to_number(:vGrpId));
   			if(SQLCODE != 0)
		   	{
		   		pubLog_Debug(_FFL_,"fpubChgBbossMode","","��¼�Ʒ��Ż�С�����[%d]",SQLCODE);
		   		EXEC SQL ROLLBACK;
				EXEC SQL CLOSE New_Cur;
				return -2;
		   	}
			
  			memset(&tdBaseFav,0,sizeof(tdBaseFav));
  			
  			strcpy(tdBaseFav.sMsisdn,vGrpUserNo);
  			strcpy(tdBaseFav.sRegionCode,vRegionCode);
  			strcpy(tdBaseFav.sFavBrand,vSmCode);
  			strcpy(tdBaseFav.sFavType,vServiceCode);
  			strcpy(tdBaseFav.sFlagCode,vServiceCode);
  			strcpy(tdBaseFav.sFavOrder,"0");
  			strcpy(tdBaseFav.sFavDay,"0");
  			strcpy(tdBaseFav.sStartTime,vStartTime);
  			strcpy(tdBaseFav.sEndTime,"20500101 00:00:00");
  			strcpy(tdBaseFav.sServiceId,"");
  			strcpy(tdBaseFav.sFavPeriod,"0");
			strcpy(tdBaseFav.sFreeValue,"0");
  			strcpy(tdBaseFav.sIdNo,vGrpId);
  			strcpy(tdBaseFav.sGroupId,"0");
  			strcpy(tdBaseFav.sProductCode,vProductCode);

			ret = 0;
			ret=OrderInsertBaseFav(ORDERIDTYPE_USER,vGrpId,100,vOpCode,lLoginAccept,vLoginNo,vOpNote,tdBaseFav);
			if (ret < 0)
			{
				pubLog_Debug(_FFL_,"fpubChgBbossMode","","�������Ż���Ϣ����[%d]",ret);
				EXEC SQL ROLLBACK;
				EXEC SQL CLOSE New_Cur;
				return -4;
			}
		}
	}
    EXEC SQL CLOSE New_Cur;
			                
	printf("\n++++++++ fpubChgBbossMode end success ++++++++\n");
	
	return 0;
}
