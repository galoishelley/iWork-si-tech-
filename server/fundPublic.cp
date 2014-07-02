/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
/**
 *	<B>����͸֧����</B>
 *@author				zhoubd
 *@since				2.0
 *@date					2006-08-08
 */
/*
#include "global.h"
#include "debug.h"
#include "pubdb.h"
*/
#include "boss_srv.h"
#include "publicsrv.h"
#include "contact.h"
#include "pubLog.h"

#define DBEXIST			-1

/*#define init(a) do { memset(a,0,sizeof(a)); } while(0)
*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL BEGIN DECLARE SECTION;
	char tmp_buf[2048];
EXEC SQL END DECLARE SECTION;


/*
 *�������Ŀ�ӻ��������������½���㣩'0'��������
 *����dFundChangeDetailYYYYMM(�ʻ���������ϸ��)��
 *����dConFundPreYYYYMM(�ʻ������Ŀ��ϸ��)
 *����dConFundDetail(�ʻ������Ŀ��)
 *����dConFund(�ʻ������)
 * @inparam

 	lContractNo			�����ʻ�
 	sDetailCodeIn		���������Ŀ
 	sChangeType			�������
 	totalFund			�ӽ�����
 	billYearMonth		��������
 	opAccept			������ˮ
 	sLoginNo			��������
 	sOpCode				��������
 	sOpTime				����ʱ��


 * @outparam
 	retCode				���ش���
 	retMsg				������Ϣ

 */
int fundRedressal(long contractNo, long detailCode, double totalFund, char *billYearMonth,
			char *changeType, long opAccept, char *loginNo, char *opTime, char *opcode, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION; 
		char iOpTime[21+1], iBill_YM[6+1], iLoginNo[6+1], iChangeType[2+1], opCode[4+1],sysyearmonth[6+1];
		long iContractNo, iDetailCode, iOpAccept;
		int changeSeq;
		double iTotalFund, preDetailValue, preTotalValue;
	EXEC SQL END DECLARE SECTION;

	/*ng���� by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];

	init(v_parameter_array);
	init(sContractNo);
	/*ng���� by yaoxc end*/

	init(iOpTime);
	init(iBill_YM);
	init(iLoginNo);
	init(iChangeType);
	init(opCode);

	iContractNo = contractNo;
	iDetailCode = detailCode;
	iTotalFund = totalFund;
	iOpAccept = opAccept;
	strcpy(iOpTime, opTime);
	strcpy(iBill_YM, billYearMonth);
	strcpy(iLoginNo, loginNo);
	strcpy(iChangeType, changeType);
	strncpy(opCode, opcode, sizeof(opCode)-1);

	strcpy(retCode, "000000");

/******ȡ�ʻ����������******/
	EXEC SQL select to_char(sysdate,'yyyymm') into :sysyearmonth from dual;
	if (SQLCODE != 0 )
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "��ȡϵͳʱ�����[%d]", SQLCODE);
		return -1;
	}
	init(tmp_buf);
	strcpy(tmp_buf, "select nvl(max(Change_Seq), 0) from dFundChangeDetail");
	strcat(tmp_buf, sysyearmonth);
	strcat(tmp_buf, " where op_accept=:iOpAccept");
	strcat(tmp_buf, " and contract_no=:iContractNo");
	strcat(tmp_buf, " and detail_code=:iDetailCode");
	strcat(tmp_buf, " and bill_ym=:iBill_YM");
	
	EXEC SQL PREPARE pro_fundRedressal1 from :tmp_buf;
	EXEC SQL DECLARE cur_fundRedressal1 CURSOR for pro_fundRedressal1;
	EXEC SQL OPEN cur_fundRedressal1 using :iOpAccept, :iContractNo, :iDetailCode, :iBill_YM;
	EXEC SQL FETCH cur_fundRedressal1 INTO :changeSeq;

	if(SQLCODE==1403)
	{
		changeSeq=1;

		pubLog_Debug(_FFL_,"","","û���ҵ���������ϸ�������,��ʼ��ֵ���ó�[1]");
	}
	else if(SQLCODE==0)
	{
		changeSeq=changeSeq+1;

		pubLog_Debug(_FFL_,"","","���û�������ϸ�������Ϊ%d",changeSeq);

	}
	else
	{
		strcpy(retCode, "PM1101");
		sprintf(retMsg, "��ȡ�ʻ����������ʧ��!��ˮ:%ld, �˺�:%ld, �����Ŀ:%ld, ��������:%s,[SQLCODE:%d]\n",
						opAccept, contractNo, detailCode, billYearMonth, SQLCODE);
		EXEC SQL CLOSE cur_fundRedressal1;
		return -1;
	}

	EXEC SQL CLOSE cur_fundRedressal1;

/******ȡ����ǰ��ϸ����******/
	EXEC SQL select nvl(Available_Fund, 0)
			   into :preDetailValue
			   from dConFundDetail
			  where contract_no=:contractNo
			  	and detail_code=:detailCode;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "��ȡ����ǰ��ϸ��������˺�:%ld, �����Ŀ:%ld[SQLCODE:%d]\n", contractNo, detailCode, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preDetailValue = 0;
	}

/******ȡ����ǰ�ܻ���******/
	EXEC SQL select nvl(current_fund, 0)
			   into :preTotalValue
			   from dConFund
			  where contract_no=:contractNo;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1103");
		sprintf(retMsg, "��ȡ����ǰ�ܻ�������˺�:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preTotalValue = 0;
	}


/******����dFundChangeDetailYYYYMM(�ʻ���������ϸ��)******/
	init(tmp_buf);
	strcpy(tmp_buf, "insert into dFundChangeDetail");
	strcat(tmp_buf, sysyearmonth);
	strcat(tmp_buf, " (op_accept, contract_no, detail_code, bill_ym, Change_Seq, Change_Type, Pre_Detail_Value, Pre_Total_Value");
	strcat(tmp_buf, ",Change_Value, After_Detail_Value, After_Total_Value, login_no, op_time, back_flag, op_code) ");
	strcat(tmp_buf, " values(:opAccept, :contractNo, :detailCode, to_number(:billYearMonth), :changeSeq, :iChangeType, :preDetailValue, :preTotalValue");
	strcat(tmp_buf, ", :totalFund1, :preDetailValue+:totalFund2, :preTotalValue+:totalFund3, :iLoginNo, to_date(:opTime,'YYYYMMDD HH24:MI:SS'), 0, :opCode)");

	EXEC SQL PREPARE pro_updateConFund2 from :tmp_buf;
	EXEC SQL EXECUTE pro_updateConFund2 using :opAccept,:contractNo,:detailCode,:billYearMonth,:changeSeq, :iChangeType, :preDetailValue,:preTotalValue,
											  :totalFund,:preDetailValue,:totalFund,:preTotalValue,:totalFund,:iLoginNo,:opTime,:opCode;

	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_,"","","sql:%s\n optime :%s\n", tmp_buf, opTime);
		pubLog_Debug(_FFL_,"","","op_accept:%ld, contract_no:%ld, detail_code:%ld, bill_ym:%s, \n"
			   "Change_Seq:%d, ChangeType:%s, Pre_Detail_Value:%d, Pre_Total_Value:%d \n"
			   "Change_Value:%d, After_Detail_Value:%d, After_Total_Value:%d, login_no:%s, \n"
			   "op_time:%s, back_flag:%s\n",
			   opAccept, contractNo, detailCode, billYearMonth,
			   changeSeq, iChangeType, preDetailValue, preTotalValue,
			   totalFund, preDetailValue+totalFund, preTotalValue+totalFund,iLoginNo,
			     opTime, "0");
		strcpy(retCode, "PM1104");
		sprintf(retMsg, "�����ʻ���������ϸ��(dFundChangeDetailYYYYMM)!��ˮ:%ld, �˺�:%ld, �����Ŀ:%ld, ��������:%s, ���:%d, iChangeType:%s [SQLCODE:%d]\n",
						opAccept, contractNo, detailCode, billYearMonth, changeSeq, iChangeType, SQLCODE);
		return -1;
	}

/******����dConFundPreYYYYMM(�ʻ������Ŀ��ϸ��)******/

	init(tmp_buf);
	strcpy(tmp_buf, "update dConFundPre");
	strcat(tmp_buf, billYearMonth);
	strcat(tmp_buf, " set Total_Fund=Total_Fund+:totalFund, Available_Fund=Available_Fund+:totalFund,");
	strcat(tmp_buf, " op_time=to_date(:opTime, 'YYYYMMDD HH24:MI:SS') where contract_no=:contractNo and detail_code=:detailCode");

	EXEC SQL PREPARE pro_fundRedressal4 from :tmp_buf;
	EXEC SQL EXECUTE pro_fundRedressal4 using :totalFund,:totalFund,:opTime,:contractNo,:detailCode;

	if (SQLCODE == NOTFOUND)
	{
		init(tmp_buf);
		strcpy(tmp_buf, "insert into dConFundPre");
		strcat(tmp_buf, billYearMonth);
		strcat(tmp_buf, "(contract_no,detail_code,bill_ym,Total_Fund,Available_Fund,op_time)");
		strcat(tmp_buf, " values(:contractNo,:detailCode,:billYearMonth,:totalFund,:totalFund,to_date(:opTime,'YYYYMMDD HH24:MI:SS'))");

		EXEC SQL PREPARE pro_fundRedressal3 from :tmp_buf;
		EXEC SQL EXECUTE pro_fundRedressal3 using :contractNo,:detailCode,:billYearMonth,:totalFund,:totalFund,:opTime;

		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1105");
			sprintf(retMsg, "�����ʻ������Ŀ��ϸ��(dConFundPreYYYYMM)ʧ�ܣ��˺�:%ld, �����Ŀ:%ld, ��������:%s, [SQLCODE:%d]\n",
						contractNo, detailCode, billYearMonth, SQLCODE);
			return -1;
		}

	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1105");
		sprintf(retMsg, "�����ʻ������Ŀ��ϸ��(dConFundPreYYYYMM)ʧ�ܣ��˺�:%ld, �����Ŀ:%ld, ��������:%s, [SQLCODE:%d]\n",
					contractNo, detailCode, billYearMonth, SQLCODE);
		return -1;
	}

/******����dConFundDetail(�ʻ������Ŀ��)******/
	EXEC SQL update dConFundDetail
				set Total_Fund=Total_Fund+:iTotalFund, Available_Fund=Available_Fund+:iTotalFund,
					max_ym=:iBill_YM, op_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
			  where contract_no = :iContractNo
			  	and detail_code = :iDetailCode;

	if (SQLCODE == NOTFOUND)
	{
		EXEC SQL insert into dConFundDetail(contract_no, detail_code, min_ym, max_ym,
											Total_Fund, Available_Fund, op_time)
					  				 values(:iContractNo, :iDetailCode, :iBill_YM, :iBill_YM,
					  				 		:iTotalFund, :iTotalFund, to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1106");
			sprintf(retMsg, "�����ʻ������Ŀ��(dConFundDetail)ʧ�ܣ��˺�:%ld, �����Ŀ:%ld, [SQLCODE:%d]\n",
						contractNo, detailCode, SQLCODE);
			return -1;
		}
	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1107");
		sprintf(retMsg, "�����ʻ������Ŀ��(dConFundDetail)ʧ�ܣ��˺�:%ld, �����Ŀ:%ld, [SQLCODE:%d]\n",
						contractNo, detailCode, SQLCODE);
		return -1;
	}

/*****����dConFund(�ʻ������)******/

	/*ng����
	EXEC SQL update dConFund
				set current_fund=current_fund+:iTotalFund,
					Month_fund=Month_fund+:iTotalFund,
					year_fund=year_fund+:iTotalFund, 
					add_fund=add_fund+:iTotalFund,
					current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
			  where contract_no=:iContractNo;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1108");
		sprintf(retMsg, "�����ʻ������(dConFund)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	ng����*/
	/*ng����  begin*/
	sprintf(sContractNo,"%ld",iContractNo);
	
	init(v_parameter_array);
		
	sprintf(v_parameter_array[0],"%f",iTotalFund);
	sprintf(v_parameter_array[1],"%f",iTotalFund);
	sprintf(v_parameter_array[2],"%f",iTotalFund);
	sprintf(v_parameter_array[3],"%f",iTotalFund);
	strcpy(v_parameter_array[4],iOpTime);
	strcpy(v_parameter_array[5],sContractNo);
	
	v_ret=OrderUpdateConFund(ORDERIDTYPE_CNTR,sContractNo,100,
						  	opCode,iOpAccept,iLoginNo,"fundRedressal",
							sContractNo,
							"current_fund=current_fund+to_number(:v0),Month_fund=Month_fund+to_number(:v1),year_fund=year_fund+to_number(:v2),add_fund=add_fund+to_number(:v3), current_time=to_date(:v4,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
	printf("OrderUpdateConFund5,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		strcpy(retCode, "PM1108");
		sprintf(retMsg, "�����ʻ������(dConFund)ʧ�ܣ��˺�:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	/*ng����  end*/	

	return 0;
}





/**********����PublicFundOprYYYYMM�������������ҽ���־*********
* @inparam
	loginAccept			������ˮ
	phoneNo				�ֻ�����
	sgoodsType		    ʵ�����
	sidgoodsNo			��Դ����   ��Ϊ��
	volume				�һ�����
	consumeFund			���ѻ���
	favourFund			�Żݻ���
	billYearMonth		������
	loginNo				��������
	totalDate			��������
	opTime				����ʱ��
	systemNote			ϵͳ��ע
	opNote				������ע
	orgId				����ID
	backFlag			������־
	opCode				��������
*/
int PublicFundOpr(long loginAccept, const char *phoneNo,const char * goodsType, const char * idgoodsNo, long volume, double consumeFund,  double favourFund,
		                       int billYearMonth,const char *loginNo, int totalDate, const char *opTime, const char *systemNote, const char *opNote, const char *orgId, const char *backFlag, const char *opCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	long 	lloginAccept=0;
	char 	sgoodsType[4+1];
	char 	sidgoodsNo[20+1];
	
	int		iTotalDate;
	int		iBillYearMonth;
	long	lLoginAccept;
	long	lVolume;
	long	lContractNo;
	long	lIdNo;
	char	sPhoneNo[15+1];
	
	char	sLoginNo[6+1];
	char	sOpTime[20+1];
	char	sSystemNote[60+1];
	char	sOpNote[60+1];
	char	sOrgId[10+1];
	char	sBackFlag[1+1];
	char	sOpCode[4+1];
	char	sIpAddr[15+1];
	char	sSmCode[2+1];
	char	sOrgCode[9+1];
	char    sSqlStr[1000];
	double	dConsumeFund;
	double	dFavourFund;
	char    vGroupId[10+1];
	EXEC SQL END DECLARE SECTION;
	
	init(sgoodsType);
	init(sidgoodsNo);
	init(sPhoneNo);
	
	init(sLoginNo);
	init(sOpTime);
	init(sSystemNote);
	init(sOpNote);
	init(sOrgId);
	init(sBackFlag);
	init(sOpCode);
	init(sIpAddr);
	init(sSmCode);
	init(sOrgCode);
	init(sSqlStr);
	init(vGroupId);

	iTotalDate 		= totalDate;
	iBillYearMonth 	= billYearMonth;
	lVolume 		= volume;
	lLoginAccept 	= loginAccept;
	dConsumeFund 	= consumeFund;
	dFavourFund 	= favourFund;
	strcpy(sPhoneNo,phoneNo);
	strcpy(sgoodsType,goodsType);
	strcpy(sidgoodsNo,idgoodsNo);
	strcpy(sLoginNo,loginNo);
	strcpy(sOpTime,opTime);
	strcpy(sSystemNote,systemNote);
	strcpy(sOpNote,opNote);
	strcpy(sOrgId,orgId);
	strcpy(sOpCode,opCode);
	strcpy(sBackFlag,backFlag);

	/***��dcustmsg����ȡָ������ļ�¼***/
    EXEC SQL SELECT id_no,sm_code,group_no
    		 INTO   :lIdNo,:sSmCode,:vGroupId
    		 FROM 	dCustMsg
    		 WHERE 	phone_no = :sPhoneNo
    		 AND 	substr(run_code,2,1) < 'a';

	if(SQLCODE != 0)
	{
		pubLog_DBErr(_FFL_,"","123401","fundPublic:PublicFundOpr��ѯdCustMsg����:[%d][%s]",SQLCODE,SQLERRMSG);
		return 123401;
	}
	Trim(vGroupId);
	
	EXEC SQL select a.home_pay_no 
			  into :lContractNo
			  from dfamilyinfo a,dfamilypromember b
			 where b.phone_no=:sPhoneNo
			and a.family_id=b.family_id
			and sysdate between b.eff_time and b.exp_time;
	if(SQLCODE != 0)
	{
		pubLog_DBErr(_FFL_,"","123401","fundPublic:PublicFundOpr��ѯhome_pay_no����:[%d][%s]",SQLCODE,SQLERRMSG);
		return 123401;
	}
	/***��dloginmsg����ȡָ�����ŵļ�¼***/
	pubLog_Debug(_FFL_,"","","sLoginNo=%s",sLoginNo);
	EXEC SQL SELECT org_code,ip_address
        	 INTO   :sOrgCode,:sIpAddr
        	 FROM 	dloginmsg
        	 WHERE 	login_no = :sLoginNo;

	if(SQLCODE != 0&&SQLCODE!=1403)
	{
		pubLog_DBErr(_FFL_,"","123402","fundPublic:PublicFundOpr��ѯdloginmsg����:[%d][%s]",SQLCODE,SQLERRMSG);
		return 123402;
	}
	else if(SQLCODE==1403)
	{
		strcpy(sOrgCode,sLoginNo);
		strcpy(sIpAddr,"0.0.0.0");
	}
	else
	{
	}


    /***����wFundExchangeOpr��***/
    Trim(sOrgId);
    sprintf(sSqlStr,"insert into wFundExchangeOpr%d(LOGIN_ACCEPT,CONTRACT_NO,goods_type,idgoods_no,VOLUME,"
                      " CONSUME_FUND,FAVOUR_FUND,ID_NO,SM_CODE,PHONE_NO,ORG_CODE,LOGIN_NO,TOTAL_DATE,"
                      " OP_TIME,SYSTEM_NOTE,OP_NOTE,IP_ADDR,ORG_ID,BACK_FLAG,OP_CODE,GROUP_ID) "
                      " values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13, "
                     "  to_date(:v14,'YYYYMMDD HH24:MI:SS'),:v15,:v16,:v17,:v18,:v19,:v20,:v21) ", iBillYearMonth);
	pubLog_Debug(_FFL_,"","","sSqlStr=[%s]",sSqlStr);

    EXEC SQL EXECUTE
    BEGIN
        EXECUTE IMMEDIATE :sSqlStr using :lLoginAccept,:lContractNo,:sgoodsType,:sidgoodsNo,:lVolume,
                                           :dConsumeFund,:dFavourFund,:lIdNo,:sSmCode,:sPhoneNo,
                                           :sOrgCode,:sLoginNo,:iTotalDate,:sOpTime,:sSystemNote,
                                           :sOpNote,:sIpAddr,:sOrgId,:sBackFlag,:sOpCode,:vGroupId;

    END;
    END-EXEC;

    if (SQLCODE != 0)
    {
        pubLog_DBErr(_FFL_,"","123403","fundPublic:PublicFundOpr����wFundExchangeOprYYYYMM����:[%d][%s]",SQLCODE,SQLERRMSG);
        return 123403;
    }

	return 0;
}






/**
 *     ����ۼ�����
 ˵������������ۼ�������ҽ��ۼ�
                         �����ʻ����пۼ�
 ���룺
	sContractNo 		 �ҽ��ʺ�
 	fTotalCount 		 �ҽ�����
 	sLoginAccept		 ������ˮ
 	sLoginNo			   ��������
 	sOpCode				   ��������
 	sOpTime				   ����ʱ��
 	iSysYearMonth		 ��ǰʱ��
 	sChangeType 		 �������

 ����
 	pfTotalCountRemain   ʣ��Ķһ�����   >0 ��ʾ�ҽ�ʱ����͸֧ ,����
 	retCode
 	retMsg

 */
int FundAwardReduce(const char *sContractNo, double fTotalCount, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *sChangeType, double *pfTotalCountRemain, char *retCode, char *retMsg)
{
	double  fCurrentFund=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainFund=0.00;
	char 	sDetailCode[10+1];       /*number10*/
	long    change_seq=0;
	int    	iCurrentYearMonth=0;	/*��ʼֵΪMin_ym��ѭ������*/
	long    iMinYearMonth=0;
	double  fPreFund=0.00;                 /*ÿ�δ��α�ʱ�����ֵ*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];
	char	vOpTime[17+1];
	
	/*ng���� by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char set_strp[1024];

	init(v_parameter_array);
	init(set_strp);
	/*ng���� by dx end*/

	init(sContractStatus);
	init(sDetailCode);
	init(sTableName);
	init(vOpTime);
	
	EXEC SQL SELECT to_char(sysdate, 'yyyymmdd hh24:mi:ss')
				INTO :vOpTime
				from dual;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639334");
		sprintf(retMsg,"ȡϵͳʱ�����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	/*�����ʻ����ҵ�ǰ���� �Ѿ����ѻ����ʻ�״̬,��С����*/
	EXEC SQL	select current_fund, total_used, to_char(contract_status), min_ym
	              into :fCurrentFund,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConFund
	             where contract_no = to_number(:sContractNo);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639367");
		sprintf(retMsg,"��ѯ�ʻ�������Ϣ����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
		fPreTotalValue=fCurrentFund;
	}
	
	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		pubLog_Debug(_FFL_,"","","�ʻ�����,����ִ�в���");
		strcpy(retCode,"639368");
		sprintf(retMsg,"�ʻ�����,����ִ�в���");
		return -1;
	}
	/*��ǰ�����㣬������һ�*/
	if(fCurrentFund < fTotalCount)
	{
		pubLog_Debug(_FFL_,"","","Ҫ�����=%f,�û�����=%f\n",fTotalCount,fCurrentFund);
		sprintf(retMsg,"�û����õĻ��𲻹��ò���");
		strcpy(retCode,"639369");
		return -1;
	}

	pubLog_Debug(_FFL_,"","","�û���ǰ����=%lf,iMinYearMonth=%d\n",fCurrentFund,iMinYearMonth);

	pubLog_Debug(_FFL_,"","","*****************����һ���ʼ**********************");

	while((iCurrentYearMonth<=iSysYearMonth) && (fTotalCountRemain > 0))
	{
		init(sTableName);
		sprintf(sTableName,"dConFundPre%6d\n",iCurrentYearMonth);
		sprintf(sSqlStr,
			"select     a.available_fund,to_char(b.detail_code) "
			" from      %s a,sFundCode b "
			" where     a.contract_no=to_number(:v1) "
			" and       a.detail_code=b.detail_code "
			" order by  b.fund_code ",
			sTableName);

		pubLog_Debug(_FFL_,"","","��ǰ������%s,ϵͳʱ��Ϊ[%d] sSqlStr[%s]",
			sTableName,iSysYearMonth, sSqlStr);

		EXEC SQL PREPARE s3760_first01 from :sSqlStr;
		EXEC SQL DECLARE s3760_cur1 CURSOR for s3760_first01;
		EXEC SQL OPEN s3760_cur1 using :sContractNo;
		while(fTotalCountRemain > 0)/*�α�ѡȡ��ȷ*/
		{
			EXEC SQL fetch s3760_cur1 into :fRemainFund,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","ȡ��̬���%s����,���ڵ�ǰ��ȡ�գ����������%d\n",sTableName,SQLCODE);
				break;
			}
			
			Trim(sDetailCode);
			if(fRemainFund==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��%s��ȡ�õ�����Ч����fRemainFund=%lf sDetailCode=%s\n",sTableName,fRemainFund,sDetailCode);
#endif
			fPreFund=fRemainFund;                /*��¼�˴��α�λ�õĻ���*/
			if(fRemainFund>=fTotalCountRemain)         /*�α������ڵ�ǰ������Ҫ�Ļ���*/
			{
				fRemainFund=fRemainFund-fTotalCountRemain;
				fTotalCountRemain = 0;
			}
			else
			{
				fTotalCountRemain = fTotalCountRemain - fRemainFund;
				fRemainFund=0;
			}
			fChangeValue=fPreFund-fRemainFund;
			fAfterTotalValue=fPreTotalValue-fChangeValue;

			init(sTableName);
			sprintf(sTableName,"dConFundPre%6d",iCurrentYearMonth);
			sprintf(sSqlStr, "update %s"
			                 "   set available_fund=:v1,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=to_number(:v2) "
			                 "   and detail_code=to_number(:v3)",sTableName);
			EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
			EXEC SQL EXECUTE s3760_first02 using  :fRemainFund,:sOpTime, :sContractNo,:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639370");
				pubLog_DBErr(_FFL_,"","639370",retMsg);
				sprintf(retMsg,"�����ʻ������Ŀ��ϸ��ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"","639370",retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}


			init(sTableName);
			sprintf(sTableName,"dFundChangeDetail%6d",iSysYearMonth);
			sprintf(sSqlStr,
				"select nvl(max(change_seq),0) "
				"  from %s"
				" where op_accept=to_number(:v1) "
				"   and   contract_no =to_number(:v2) "
				"   and   detail_code =to_number(:v3) "
				"   and   bill_ym     = :v4 ",
				sTableName);
			EXEC SQL PREPARE s3799_first05 from :sSqlStr;
			EXEC SQL DECLARE curFundDeduct3 CURSOR for s3799_first05;
			EXEC SQL OPEN curFundDeduct3 using :sLoginAccept, :sContractNo, :sDetailCode, :iCurrentYearMonth;
			EXEC SQL FETCH curFundDeduct3 INTO :change_seq;

			if(SQLCODE==1403)
			{
				change_seq=1;

				pubLog_Debug(_FFL_,"","","û���ҵ���������ϸ�������,��ʼ��ֵ���ó�[1]\n");

			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;

				pubLog_Debug(_FFL_,"","","���û�������ϸ�������Ϊ%d\n",change_seq);

			}
			else
			{
				strcpy(retCode,"639371");
				sprintf(retMsg,"��ѯ����������Ŵ���[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				EXEC SQL CLOSE curFundDeduct3;
				return -1;
			}
			EXEC SQL CLOSE curFundDeduct3;
			pubLog_Debug(_FFL_,"","","��������Ϣ���뵽�ʻ���������ϸ����dFundChangeDetail\n");
			sprintf(sSqlStr,
				"insert into %s  "
				"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
				"Change_Type,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
				"After_Detail_Value,After_Total_Value,"
				"login_no        ,op_time,back_flag,op_code)"
				"values(to_number(:v1),to_number(:v2),to_number(:v3),:v4,:v5,"
				":sChangeType,:v6,:v7,-1*:v8,"
				":v9,:v10,"
				"nvl(:v11,'test'),:v12,'0',:op_code)",
				sTableName);

			EXEC SQL PREPARE s3760_first04 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first04 using
				:sLoginAccept    ,:sContractNo,:sDetailCode,:iCurrentYearMonth,:change_seq,
				:sChangeType, :fPreFund        ,:fPreTotalValue         ,:fChangeValue,
				:fRemainFund     ,:fAfterTotalValue         ,:sLoginNo,:vOpTime, :sOpCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639372");
				sprintf(retMsg,"���������ϸ���в������ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;

				pubLog_Debug(_FFL_,"","","���������ϸ���в������ݳɹ�\n");

			}


			pubLog_Debug(_FFL_,"","","�����ʻ������Ŀ��\n");

			EXEC SQL	update dConFundDetail
			               set available_fund=available_fund-:fChangeValue,
			                   min_ym=:iCurrentYearMonth,
			                   max_ym=:iSysYearMonth,
			                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
			             where contract_no=:sContractNo
			               and detail_code=:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639373");
				sprintf(retMsg,"����[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}

			pubLog_Debug(_FFL_,"","","�α��,���׽��������ʻ������Ŀ��ɹ�\n");
			pubLog_Debug(_FFL_,"","","���׽���,ֱ����ת���ر��α�\n");

		}
		EXEC SQL CLOSE s3760_cur1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","����ѭ��������ȡ��һ�ű�������Ϊ%d\n",iCurrentYearMonth);
		}
	}


	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}



	/*��ǰ����ֱ��ܺ� < �ܱ����ʱ*/
	if(fTotalCountRemain > 0.005)
	{
		fTotalCount=fCurrentFund;
		strcpy(retCode,"639311");
		sprintf(retMsg,"�ֱ���𲻹��ۼ�����������[%d]",SQLCODE);
	}
	/*ng����dx
	EXEC SQL	update dConFund
	               set current_fund   =   round(current_fund-:fTotalCount,2),
	                   min_ym          =   :iCurrentYearMonth,
	                   month_used      =   month_used+:fTotalCount,
	                   total_used      =   total_used+:fTotalCount,
	                   year_used       = year_used+:fTotalCount,
	                   current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	             where contract_no=to_number(:sContractNo);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639374");
		sprintf(retMsg,"��������������ʻ���������ݳ���[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}*/
	/*ng���� by dx begin*/

    init(v_parameter_array);
		
	sprintf(v_parameter_array[0],"%f",fTotalCount);
	sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
	sprintf(v_parameter_array[2],"%f",fTotalCount);
	sprintf(v_parameter_array[3],"%f",fTotalCount);
	sprintf(v_parameter_array[4],"%f",fTotalCount);
	strcpy(v_parameter_array[5],sOpTime);
	strcpy(v_parameter_array[6],sContractNo);
	sprintf(set_strp,"current_fund   =   round(current_fund-to_number(:v0),2),\
	min_ym          =   to_number(:v1),\
	month_used      =   month_used+to_number(:v2),\
	total_used      =   total_used+to_number(:v3),\
	year_used       = year_used+to_number(:v4),\
	current_time    = to_date(:v5, 'YYYYMMDD HH24:MI:SS')");
    
	v_ret=OrderUpdateConFund(ORDERIDTYPE_CNTR,sContractNo,100,
						  	sOpCode,atol(sLoginAccept),sLoginNo,"FundAwardReduce",
							sContractNo,set_strp,"",v_parameter_array);
	printf("OrderUpdateConFund ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		strcpy(retCode,"639374");
		sprintf(retMsg,"��������������ʻ���������ݳ���[%d]",v_ret);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
    /*ng���� by dx end*/	


	pubLog_Debug(_FFL_,"","","��������������ʻ��������ݳɹ�\n");


	/*��� fTotalCountRemain����0����ʾ��������͸֧*/
	*pfTotalCountRemain = fTotalCountRemain;
	return 0;
}


/**
 *     ����ҽ�����
              ˵���������ʻ����жҽ�����������͸֧�µĲ���͸֧����
  ���룺
	sContractNo 		 �ҽ��ʺ�
 	fTotalCount 		 �ҽ�����
 	sLoginAccept		 ������ˮ
 	sLoginNo			 ��������
 	sOpCode				 ��������
 	sOpTime				 ����ʱ��
 	iSysYearMonth		 ��ǰʱ��


 ����
 	retCode              ���ش���
 	retMsg               ������Ϣ
 */
int FundAward(const char *sContractNo, double fTotalCount, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	/*ng���� by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char set_strp[1024];

	init(v_parameter_array);
	init(set_strp);
	/*ng���� by dx end*/
	double  fTotalCountRemain=0;

	if (FundAwardReduce(sContractNo, fTotalCount, sLoginAccept,
			sLoginNo, sOpCode,	sOpTime, iSysYearMonth,"1", &fTotalCountRemain, retCode, retMsg) != 0)
	{
		return -1;
	}

	if (fTotalCountRemain < 0.005)
	{
		return 0;
	}
	else
	{
		pubLog_Debug(_FFL_,"","","�����˻��ڻ�����\n");
		return -1;
	}
}


/**�����ʻ����ʻ��»����Ŀ�ۼ�����
 * @inparam iDeductType �������ͣ� δ��
 
 *����dFundChangeDetailYYYYMM(�ʻ���������ϸ��)��
 *����dConFundPreYYYYMM(�ʻ������Ŀ��ϸ��)
 *����dConFundDetail(�ʻ������Ŀ��)
 *����dConFund(�ʻ������)

 	lContractNo			�����ʻ�
 	sDetailCodeIn		���������Ŀ  ����Ŀ�ۼ�ʱ����ΪNULL
 	sChangeType			�������
 	fTotalCount			�ۼ�����
 	lLoginAccept		������ˮ
 	sLoginNo			��������
 	sOpCode			    ��������
 	sOpTime				����ʱ��
 	iSysYearMonth		ϵͳʱ��

 * @outparam
 	retCode				���ش���
 	retMsg				������Ϣ

 */
int FundDeduct_ng(char *send_type,char *send_id,long lContractNo, const char* sDetailCodeIn, const char* sChangeType,
		double fTotalCount, long lLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	double  fCurrentFund=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainFund=0.00;
	char 	sDetailCode[10+1];       /*number10*/
	long    change_seq=0;
	int    	iCurrentYearMonth=0;
	long    iMinYearMonth=0;
	double  fPreFund=0.00;                 /*ÿ�δ��α�ʱ�����ֵ*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];
	char	vOpTime[17+1];
	/*ng���� by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];
    
	init(v_parameter_array);
	init(sContractNo);
	/*ng���� by yaoxc end*/

	init(sContractStatus);
	init(sDetailCode);
	init(sTableName);
	init(vOpTime);
	
	EXEC SQL SELECT to_char(sysdate, 'yyyymmdd hh24:mi:ss')
				INTO :vOpTime
				from dual;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639334");
		sprintf(retMsg,"ȡϵͳʱ�����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}

	/*�����ʻ����ҵ�ǰ���� ͸֧�����ȣ���͸֧�����Ѿ����ѻ����ʻ�״̬,������������*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_fund,total_used, to_char(contract_status), min_ym
	              into :fCurrentFund,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConFund
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639358");
		sprintf(retMsg,"��ѯ�ʻ�������Ϣ����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfCurrentFund=%lf\n",fCurrentFund);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢfTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","��ѯ�����û���ǰ��ϢiMinYearMonth=%d\n",iMinYearMonth);
#endif
		fPreTotalValue=fCurrentFund;
	}

	if (sDetailCodeIn != NULL)
	{
		EXEC SQL	select Available_Fund, min_ym
		              into :fCurrentFund,:iMinYearMonth
		              from dConFundDetail
		             where contract_no = :lContractNo
		               and detail_code=to_number(:sDetailCodeIn);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"639359");
			sprintf(retMsg,"��ѯ�ʻ������Ŀ�������[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		if (fCurrentFund < fTotalCount)
		{
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�ʻ������Ŀ�ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentFund,fTotalCount);
	#endif
			strcpy(retCode,"1000");
			sprintf(retMsg,"�ʻ������Ŀ�ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentFund,fTotalCount);
			return -1;
		}
	}
	else if (fCurrentFund < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�ʻ�����ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentFund,fTotalCount);
#endif
		strcpy(retCode,"1000");
		sprintf(retMsg,"�ʻ�����ĵ�ǰ����[%lf]�����ڿ۳�����[%lf]",fCurrentFund,fTotalCount);
		return -1;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"639360");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","�ʻ�����,����ִ�в���");
#endif
		sprintf(retMsg,"�ʻ�����,����ִ�в���");
		return -1;
	}


#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************����һ���ʼ**********************\n");
#endif
	while((iCurrentYearMonth<=iSysYearMonth) && (fTotalCountRemain > 0))
	{
		init(sTableName);
		sprintf(sTableName,"dConFundPre%6d\n",iCurrentYearMonth);
		if (sDetailCodeIn == NULL)
		{
			sprintf(sSqlStr,
				"select     a.available_fund,to_char(b.detail_code) "
				" from      %s a,sFundCode b "
				" where     a.contract_no=:v1 "
				" and       a.detail_code=b.detail_code "
				" order by  b.DEDUCT_ORDER ",
				sTableName);
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��ǰ������%s,��ʱ��%d,ϵͳʱ��Ϊ[%d] sSqlStr[%s]\n",
				sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
	#endif
		}
		else
		{
			sprintf(sSqlStr,
				"select available_fund,detail_code "
				"  from %s  "
				" where contract_no=:v1 "
				"   and detail_code=:sDetailCodeIn " ,
				sTableName);
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��ǰ������%s,��ʱ��%d,ϵͳʱ��Ϊ[%d] sSqlStr[%s]\n",
				sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
	#endif
		}
		EXEC SQL PREPARE preFundDeduct1 from :sSqlStr;
		EXEC SQL DECLARE curFundDeduct1 CURSOR for preFundDeduct1;
		if (sDetailCodeIn == NULL)
		{
			pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
			EXEC SQL OPEN curFundDeduct1 using :lContractNo;
		}
		else
		{
			EXEC SQL OPEN curFundDeduct1 using :lContractNo,:sDetailCodeIn;
		}
		while(fTotalCountRemain > 0)/*�α�ѡȡ��ȷ*/
		{
			EXEC SQL fetch curFundDeduct1 into :fRemainFund,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","ȡ��̬���%s����,���ڵ�ǰ��ȡ�գ����������%d\n",sTableName,SQLCODE);
				break;
			}
			Trim(sDetailCode);
			if(fRemainFund==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","��%s��ȡ�õ�fRemainFund=%lf sDetailCode=%s\n",sTableName,fRemainFund,sDetailCode);
#endif
			fPreFund=fRemainFund;                /*��¼�˴��α�λ�õĻ���*/
			if(fRemainFund>=fTotalCountRemain)         /*�α������ڵ�ǰ������Ҫ�Ļ���*/
			{
				fRemainFund=fRemainFund-fTotalCountRemain;
				fTotalCountRemain = 0;
			}
			else
			{
				fTotalCountRemain = fTotalCountRemain - fRemainFund;
				fRemainFund=0;
			}
			fChangeValue=fPreFund-fRemainFund;
			fAfterTotalValue=fPreTotalValue-fChangeValue;

			init(sTableName);
			sprintf(sTableName,"dConFundPre%6d",iCurrentYearMonth);
			sprintf(sSqlStr, "update %s"
			                 "   set available_fund=:v1,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=:v2"
			                 "   and detail_code=to_number(:v3)",sTableName);
			EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
			EXEC SQL EXECUTE s3760_first02 using  :fRemainFund,:sOpTime, :lContractNo,:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639361");
				sprintf(retMsg,"�����ʻ������Ŀ��ϸ��ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�α������³ɹ�\n");
			pubLog_Debug(_FFL_,"","","��ǰ�α�λ��ʣ��Ļ�����ֵΪ%lf\n",fRemainFund);
			pubLog_Debug(_FFL_,"","","���α����ѵĻ�������Ϊ%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","��ѯ��ǰ����%d�ʻ���������ϸ�������Ļ��������\n",iSysYearMonth);
#endif

			init(sTableName);
			sprintf(sTableName,"dFundChangeDetail%6d",iSysYearMonth);
			sprintf(sSqlStr,
				"select nvl(max(change_seq),0) "
				"  from %s"
				" where op_accept=:v1"
				"   and   contract_no =:v2 "
				"   and   detail_code =to_number(:v3) "
				"   and   bill_ym     = :v4 ",
				sTableName);
			EXEC SQL PREPARE s3799_first04 from :sSqlStr;
			EXEC SQL DECLARE curFundDeduct2 CURSOR for s3799_first04;
			EXEC SQL OPEN curFundDeduct2 using :lLoginAccept, :lContractNo, :sDetailCode, :iCurrentYearMonth;
			EXEC SQL FETCH curFundDeduct2 INTO :change_seq;
	
			if(SQLCODE==1403)
			{
				change_seq=1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","û���ҵ���������ϸ�������,��ʼ��ֵ���ó�[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","���û�������ϸ����֮�����Ϊ%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"639362");
				sprintf(retMsg,"��ѯ����������Ŵ���[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct2;
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
			EXEC SQL CLOSE curFundDeduct2;
			pubLog_Debug(_FFL_,"","","��������Ϣ���뵽�ʻ���������ϸ����dFundChangeDetail\n");
			sprintf(sSqlStr,
				"insert into %s  "
				"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
				"Change_Type,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
				"After_Detail_Value,After_Total_Value,"
				"login_no        ,op_time,back_flag,op_code)"
				"values(:v1,:v2,to_number(:v3),:v4,:v5,"
				":ChangeType,:v6,:v7,-1*:v8,"
				":v9,:v10,"
				"nvl(:v11,'test'),:v12,'0',:op_code)",
				sTableName);

			EXEC SQL PREPARE s3760_first04 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first04 using
				:lLoginAccept,    :lContractNo,:sDetailCode,:iCurrentYearMonth,:change_seq,
				:sChangeType,     :fPreFund        ,:fPreTotalValue         ,:fChangeValue,
				:fRemainFund,     :fAfterTotalValue         ,:sLoginNo, :vOpTime,:sOpCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639363");
				sprintf(retMsg,"���������ϸ���в������ʱ��������[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","���������ϸ���в������ݳɹ�");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�����ʻ������Ŀ��");
#endif
			EXEC SQL	update dConFundDetail
			               set available_fund=available_fund-:fChangeValue,
			                   min_ym=:iCurrentYearMonth,
			                   max_ym=:iSysYearMonth,
			                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
			             where contract_no=:lContractNo
			               and detail_code=to_number(:sDetailCode);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"639364");
				sprintf(retMsg,"�����ʻ������Ŀ��ʧ��[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","�α��,���׽��������ʻ������Ŀ��ɹ�");
			pubLog_Debug(_FFL_,"","","���׽���,ֱ����ת���ر��α�");
#endif
		}
		EXEC SQL CLOSE curFundDeduct1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","����ѭ��������ȡ��һ�ű�������Ϊ%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while��������������.û��͸֧����2.����͸֧����");
	pubLog_Debug(_FFL_,"","","��ǰ����Ϊ%d",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","��ǰ����Ϊ%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","�����ʻ���ǰ�ļ��,total_fund=%f\n",fTotalCount);
#endif

	/*����dconfund��ۻ���ʱ�����ۼ��»�����������ۼǻ���ֻ��¼ʹ�û���*/
		
	sprintf(sContractNo,"%ld",lContractNo);
	Trim(sContractNo);
	init(v_parameter_array);
		
	sprintf(v_parameter_array[0],"%f",fTotalCount);
	sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
	strcpy(v_parameter_array[2],sOpTime);
	strcpy(v_parameter_array[3],sContractNo);
	
	v_ret=OrderUpdateConFund(send_type,send_id,100,
						  	sOpCode,lLoginAccept,sLoginNo,"FundDeduct",
							sContractNo,
							" current_fund = round(current_fund-:fTotalCount,2),min_ym = to_number(:v1),current_time = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
	printf("OrderUpdateConFund5 ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		strcpy(retCode,"639365");
		sprintf(retMsg,"��������������ʻ���������ݳ���[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}


	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","��������������ʻ��������ݳɹ�fTotalCountRemain=%f",fTotalCountRemain);
#endif

	if(fTotalCountRemain > 0.005)/*��������͸֧*/
	{
		strcpy(retCode,"639366");
		sprintf(retMsg,"��������͸֧����[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	return 0;
}

/**
 *     ������������������ʻ����жԽ�
* @inparam
 	sBackLoginAccept	������ˮ
 	iBackYearMonth		��������
 	sLoginAccept		������ˮ
 	sLoginNo			��������
 	sOpCode				��������
 	sOpTime				����ʱ��
 	iSysYearMonth		ϵͳ����

* @outparam
 	retCode
 	retMsg
 */
int FundAwardBack(const char *sBackLoginAccept, int iBackYearMonth, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	char    sSqlStr[2000+1];
	char 	table_name[40+1];

	double 	fAccCount=0.00;
	double  fChangeValue=0.00;
	double  fOweFund=0.00;
	double  fCurrentFund=0.00;
	double  fAfterDetailValue=0.00;
	double  fCurrentTotalValue=0.00;
	double 	temp1=0.00;
	double 	temp2=0.00;
	int    	iBillYm=0;
	int    	iMinYm=0;
	char    sContractNo[14+1];
	char    sDetailCode[10+1];
	int    	iChangeSeq=0;
	long    iCurrMinYm=0;
	int    	iDetailMinYm=0;
	char    sBakeContractNo[14+1];
	
	int     v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];

	init(v_parameter_array);
	init(sContractNo);
	init(sDetailCode);
	init(sBakeContractNo);

	init(sSqlStr);
	sprintf(sSqlStr, "select to_char(contract_no), to_char(detail_code), bill_ym,"
					 "       change_seq,abs(change_value),after_detail_value"
					 "  from dFundChangeDetail%d"
					 " where op_accept = to_number(:sBackLoginAccept)"
					 "   and change_type = '1'"
					 "   and back_flag = '0'"
					 " order by bill_ym desc",
					 iBackYearMonth);
	printf("sSqlStr[%s]\n",sSqlStr);
	/*���ֱ任����Ϊ1����������ǻ��ֶһ�*/
	EXEC SQL PREPARE preFundAwardBack2 from :sSqlStr;
	EXEC SQL DECLARE curFundAwardBack2 CURSOR FOR preFundAwardBack2;
	EXEC SQL OPEN    curFundAwardBack2 using :sBackLoginAccept;


	pubLog_Debug(_FFL_,"","","OPEN SQLCODE :%d\n", SQLCODE);

	for(;;)
	{
		EXEC SQL FETCH   curFundAwardBack2 INTO :sContractNo, :sDetailCode, :iBillYm,
												:iChangeSeq,:fChangeValue,:fAfterDetailValue;
		if (SQLCODE == 1403)
		{
			break;
		}
		else if (SQLCODE != 0)
		{
			strcpy(retCode,"376935");
			sprintf(retMsg,"ȡ�ʻ�������Ϣ����[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		Trim(sContractNo);
		pubLog_Debug(_FFL_,"","","��õĸ�ϸ������Ļ��ֱ仯Ϊ%lf",fChangeValue);
		pubLog_Debug(_FFL_,"","","��ǰ�ʻ�Ϊ%s,�����ʻ�Ϊ[%s]",sContractNo,sBakeContractNo);

		if (strcmp(sBakeContractNo, sContractNo) != 0)
		{
			if(sBakeContractNo[0] != '\0')
			{
				pubLog_Debug(_FFL_,"","","���ʻ��ҽ���������\n");
				strcpy(retCode,"376915");
				sprintf(retMsg,"���ʻ��ҽ���������");
				EXEC SQL CLOSE curFundAwardBack2;
				return -1;
			}

			EXEC SQL	select current_fund,min_ym
			              into :fCurrentFund,:iMinYm
			              from dconFund
			             where contract_no=to_number(:sContractNo);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"376916");
				sprintf(retMsg,"���ʻ����ֱ��в�ѯ�ʻ��ܻ��ֳ���[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundAwardBack2;
				return -1;
			}

			iCurrMinYm=iMinYm;
			init(sBakeContractNo);
			strcpy(sBakeContractNo,sContractNo);
			fAccCount=0;               /*��ʱ�ʻ����*/
			fCurrentTotalValue=fCurrentFund;
		}


		/*��ѯ��ǰ�����ʻ����ֱ����ϸ���У�ͬ������ˮ���ʻ�id,�������ֿ�Ŀ�������ʻ������������ʻ����ֱ�����*/
		pubLog_Debug(_FFL_,"","","��ѯ��ǰ����%d�ʻ����ֱ����ϸ�������Ļ��ֱ�����",iSysYearMonth);
		Trim(sContractNo);
		Trim(sDetailCode);

		/*�ж���С����*/
		if (iMinYm > iBillYm)
		{
			iCurrMinYm = iBillYm;
		}

		iMinYm=iCurrMinYm;

		init(table_name);
		sprintf(table_name,"dFundChangeDetail%6d",iSysYearMonth);
		pubLog_Debug(_FFL_,"","","����Ϊ[%s],[%s],[%s],[%d]\n",sLoginAccept,sContractNo,sDetailCode,iBillYm);
		sprintf(sSqlStr,
			" select nvl(max(change_seq),0) "
			" from %s  where op_accept=to_number(:v1) "
			" and   contract_no =to_number(:v2) "
			" and   Detail_Code =to_number(:v3) "
			" and   bill_ym     = :v4 ",
			table_name);
		EXEC SQL PREPARE preFundAwardBack3 from :sSqlStr;
		EXEC SQL DECLARE curFundAwardBack3 CURSOR for preFundAwardBack3;
		EXEC SQL OPEN    curFundAwardBack3 using :sLoginAccept,:sContractNo,:sDetailCode,:iBillYm;;
		EXEC SQL fetch curFundAwardBack3 into :iChangeSeq;

		if(SQLCODE==1403)
		{
			iChangeSeq=1;
			pubLog_Debug(_FFL_,"","","û���ҵ��������±�����,��ʼ��ֵ���ó�[1]");
			EXEC SQL CLOSE curFundAwardBack3;
		}
		else if(SQLCODE==0)
		{
			iChangeSeq=iChangeSeq+1;
			EXEC SQL CLOSE curFundAwardBack3;
			pubLog_Debug(_FFL_,"","","��ǰ���õı�����Ϊ%d",iChangeSeq);
		}
		else
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","���ұ�����ʱ����������[%d]",SQLCODE);
#endif
			sprintf(retMsg,"��ѯ�����ֱ�����[%d]",SQLCODE);
			EXEC SQL CLOSE curFundAwardBack3;
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","��ǰϵͳʱ���µĻ��ֱ����ϸ���в��뵱ǰ�����Ľ��");
#endif

		temp1=fAfterDetailValue+fChangeValue;
		temp2=fCurrentTotalValue+fChangeValue;

		init(table_name);
		sprintf(table_name,"dFundChangeDetail%6d",iSysYearMonth);
		sprintf(sSqlStr,
			"insert into %s  "
			"(op_accept,contract_no,Detail_Code,Bill_Ym,change_seq,"
			"Change_Type,pre_detail_value,pre_total_value,change_value,"
			"after_detail_value,after_total_value,"
			"login_no,op_time,back_flag,op_code)"
			"values(to_number(:v1),to_number(:v2),to_number(:v3),:v4,:v5,"
			"'1',:v6,:v7,:v8,"
			":v9,:v10,"
			":v11,to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),'1',:sOpCode)",
			table_name);
		EXEC SQL PREPARE preFundAwardBack4 from :sSqlStr;
		EXEC SQL EXECUTE preFundAwardBack4 using
			:sLoginAccept,:sContractNo,:sDetailCode,:iBillYm,:iChangeSeq,
			:fAfterDetailValue,:fCurrentTotalValue,:fChangeValue,
			:temp1,:temp2,:sLoginNo, :sOpTime, :sOpCode;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"����ֱ����ϸ���в������ʱ��������[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}
		fCurrentTotalValue=temp2;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","����ֱ����ϸ���в������ݳɹ�\n");
#endif
		fAccCount+=fChangeValue;

		pubLog_Debug(_FFL_,"","","contract_no=%s;sDetailCode=%s\n",sContractNo,sDetailCode);
		pubLog_Debug(_FFL_,"","","����%d���µ��ʻ����ֿ�Ŀ��ϸ��\n",iBillYm);
		init(table_name);
		sprintf(table_name,"dConFundPre%6d",iBillYm);
		sprintf(sSqlStr,
			" update %s "
			" set available_Fund=available_Fund+:v1, "
			" op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS') "
			" where  contract_no=to_number(:v2)   "
			" and    detail_code=to_number(:v3) ",
			table_name);
		EXEC SQL PREPARE preFundAwardBack5 from :sSqlStr;
		EXEC SQL EXECUTE preFundAwardBack5 using :fChangeValue,:sOpTime,:sContractNo,:sDetailCode;
		if(SQLCODE==0)
		{
			pubLog_Debug(_FFL_,"","","�ɹ����ʻ���Ŀ��ϸ�������ֵ\n");
		}
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"�����ʻ���Ŀ��ϸ����û���ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		pubLog_Debug(_FFL_,"","","ѡ����ֿ�Ŀ���¸ö�����Ŀ�µ���С����\n");
		EXEC SQL	select min_ym
		              into :iDetailMinYm
		              from dConFundDetail
		             where contract_no = to_number(:sContractNo)
		               and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"ѡ����ֿ�Ŀ���¸ö�����Ŀ�µ���С����ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		if( iDetailMinYm>iBillYm )
		{
			iDetailMinYm = iBillYm;
		}

		pubLog_Debug(_FFL_,"","","���»��ֿ�Ŀ��\n");
		EXEC SQL	update dConFundDetail
		               set min_ym=:iDetailMinYm,
		                   max_ym=:iSysYearMonth,
		                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
		                   available_fund=available_fund+:fChangeValue
		             where contract_no = to_number(:sContractNo)
			           and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"���»��ֿ�Ŀ��ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}
	}
	EXEC SQL CLOSE curFundAwardBack2;
	
	/*�α�رգ��������ʻ��µ�����д���ʻ����ֱ���*/

	if (strlen(sBakeContractNo) != 0)
	{
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fAccCount);
		sprintf(v_parameter_array[1],"%ld",iCurrMinYm);
		sprintf(v_parameter_array[2],"%f",fAccCount);
		sprintf(v_parameter_array[3],"%f",fAccCount);
		sprintf(v_parameter_array[4],"%f",fAccCount);
		strcpy(v_parameter_array[5],sOpTime);
		strcpy(v_parameter_array[6],sBakeContractNo);
    	printf("[%f][%ld][%f][%s]\n",fAccCount,iCurrMinYm,fAccCount,sBakeContractNo);
		v_ret=OrderUpdateConFund(ORDERIDTYPE_CNTR,sBakeContractNo,100,
							  	sOpCode,atol(sLoginAccept),sLoginNo,"FundAwardBack",
								sBakeContractNo,
								" current_fund = current_fund+to_number(:v0),min_ym = to_number(:v1),month_used = month_used-to_number(:v2),year_used = year_used-to_number(:v3),total_used = total_used-to_number(:v4),current_time = to_date(:v5, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConFund ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"376917");
			sprintf(retMsg,"�����ʻ����ֱ����[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","���ۻ�����Ļ���%lf�ӵ����ʻ�%s�Ļ��ֱ��У��ɹ�\n",fAccCount,sBakeContractNo);
#endif

		sprintf(sSqlStr,"update dFundChangeDetail%6d "
						"   set back_flag='1'"
						" where op_accept=to_number(:op_accept)   ",
						iBackYearMonth);
		EXEC SQL PREPARE preFundAwardBack6 from :sSqlStr;
		EXEC SQL EXECUTE preFundAwardBack6 using :sBackLoginAccept;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376918");
			sprintf(retMsg,"�����ʻ���Ŀ��ϸ����û���ʧ��[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}
	return 0;
}
