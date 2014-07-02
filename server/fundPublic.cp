/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/**
 *	<B>积金透支冲销</B>
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
 *按积金科目加积金函数（可用于月结计算）'0'积金生成
 *更新dFundChangeDetailYYYYMM(帐户积金变更明细表)，
 *更新dConFundPreYYYYMM(帐户积金科目明细表)
 *更新dConFundDetail(帐户积金科目表)
 *更新dConFund(帐户积金表)
 * @inparam

 	lContractNo			积金帐户
 	sDetailCodeIn		积金二级科目
 	sChangeType			变更类型
 	totalFund			加奖积金
 	billYearMonth		帐务年月
 	opAccept			操作流水
 	sLoginNo			操作工号
 	sOpCode				操作代码
 	sOpTime				操作时间


 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

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

	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];

	init(v_parameter_array);
	init(sContractNo);
	/*ng解耦 by yaoxc end*/

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

/******取帐户积金变更序号******/
	EXEC SQL select to_char(sysdate,'yyyymm') into :sysyearmonth from dual;
	if (SQLCODE != 0 )
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "获取系统时间错误[%d]", SQLCODE);
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

		pubLog_Debug(_FFL_,"","","没有找到积金变更明细表变更序号,初始数值设置成[1]");
	}
	else if(SQLCODE==0)
	{
		changeSeq=changeSeq+1;

		pubLog_Debug(_FFL_,"","","设置积金变更明细表变更序号为%d",changeSeq);

	}
	else
	{
		strcpy(retCode, "PM1101");
		sprintf(retMsg, "获取帐户积金变更序号失败!流水:%ld, 账号:%ld, 积金科目:%ld, 帐务年月:%s,[SQLCODE:%d]\n",
						opAccept, contractNo, detailCode, billYearMonth, SQLCODE);
		EXEC SQL CLOSE cur_fundRedressal1;
		return -1;
	}

	EXEC SQL CLOSE cur_fundRedressal1;

/******取更新前详细积金******/
	EXEC SQL select nvl(Available_Fund, 0)
			   into :preDetailValue
			   from dConFundDetail
			  where contract_no=:contractNo
			  	and detail_code=:detailCode;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "获取更新前详细积金错误！账号:%ld, 积金科目:%ld[SQLCODE:%d]\n", contractNo, detailCode, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preDetailValue = 0;
	}

/******取更新前总积金******/
	EXEC SQL select nvl(current_fund, 0)
			   into :preTotalValue
			   from dConFund
			  where contract_no=:contractNo;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1103");
		sprintf(retMsg, "获取更新前总积金错误！账号:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preTotalValue = 0;
	}


/******插入dFundChangeDetailYYYYMM(帐户积金变更明细表)******/
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
		sprintf(retMsg, "插入帐户积金变更明细表(dFundChangeDetailYYYYMM)!流水:%ld, 账号:%ld, 积金科目:%ld, 帐务年月:%s, 序号:%d, iChangeType:%s [SQLCODE:%d]\n",
						opAccept, contractNo, detailCode, billYearMonth, changeSeq, iChangeType, SQLCODE);
		return -1;
	}

/******更新dConFundPreYYYYMM(帐户积金科目明细表)******/

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
			sprintf(retMsg, "插入帐户积金科目明细表(dConFundPreYYYYMM)失败！账号:%ld, 积金科目:%ld, 帐务年月:%s, [SQLCODE:%d]\n",
						contractNo, detailCode, billYearMonth, SQLCODE);
			return -1;
		}

	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1105");
		sprintf(retMsg, "更新帐户积金科目明细表(dConFundPreYYYYMM)失败！账号:%ld, 积金科目:%ld, 帐务年月:%s, [SQLCODE:%d]\n",
					contractNo, detailCode, billYearMonth, SQLCODE);
		return -1;
	}

/******更新dConFundDetail(帐户积金科目表)******/
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
			sprintf(retMsg, "插入帐户积金科目表(dConFundDetail)失败！账号:%ld, 积金科目:%ld, [SQLCODE:%d]\n",
						contractNo, detailCode, SQLCODE);
			return -1;
		}
	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1107");
		sprintf(retMsg, "更新帐户积金科目表(dConFundDetail)失败！账号:%ld, 积金科目:%ld, [SQLCODE:%d]\n",
						contractNo, detailCode, SQLCODE);
		return -1;
	}

/*****更新dConFund(帐户积金表)******/

	/*ng解耦
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
		sprintf(retMsg, "更新帐户积金表(dConFund)失败！账号:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	ng解耦*/
	/*ng解耦  begin*/
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
		sprintf(retMsg, "更新帐户积金表(dConFund)失败！账号:%ld, [SQLCODE:%d]\n", contractNo, SQLCODE);
		return -1;
	}
	/*ng解耦  end*/	

	return 0;
}





/**********插入PublicFundOprYYYYMM表函数，插入积金兑奖日志*********
* @inparam
	loginAccept			操作流水
	phoneNo				手机号码
	sgoodsType		    实物类别
	sidgoodsNo			资源代码   可为空
	volume				兑换数量
	consumeFund			消费积金
	favourFund			优惠积金
	billYearMonth		表年月
	loginNo				操作工号
	totalDate			帐务日期
	opTime				操作时间
	systemNote			系统备注
	opNote				操作备注
	orgId				工号ID
	backFlag			冲正标志
	opCode				操作代码
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

	/***从dcustmsg表里取指定号码的记录***/
    EXEC SQL SELECT id_no,sm_code,group_no
    		 INTO   :lIdNo,:sSmCode,:vGroupId
    		 FROM 	dCustMsg
    		 WHERE 	phone_no = :sPhoneNo
    		 AND 	substr(run_code,2,1) < 'a';

	if(SQLCODE != 0)
	{
		pubLog_DBErr(_FFL_,"","123401","fundPublic:PublicFundOpr查询dCustMsg出错:[%d][%s]",SQLCODE,SQLERRMSG);
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
		pubLog_DBErr(_FFL_,"","123401","fundPublic:PublicFundOpr查询home_pay_no出错:[%d][%s]",SQLCODE,SQLERRMSG);
		return 123401;
	}
	/***从dloginmsg表里取指定工号的记录***/
	pubLog_Debug(_FFL_,"","","sLoginNo=%s",sLoginNo);
	EXEC SQL SELECT org_code,ip_address
        	 INTO   :sOrgCode,:sIpAddr
        	 FROM 	dloginmsg
        	 WHERE 	login_no = :sLoginNo;

	if(SQLCODE != 0&&SQLCODE!=1403)
	{
		pubLog_DBErr(_FFL_,"","123402","fundPublic:PublicFundOpr查询dloginmsg出错:[%d][%s]",SQLCODE,SQLERRMSG);
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


    /***插入wFundExchangeOpr表***/
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
        pubLog_DBErr(_FFL_,"","123403","fundPublic:PublicFundOpr插入wFundExchangeOprYYYYMM出错:[%d][%s]",SQLCODE,SQLERRMSG);
        return 123403;
    }

	return 0;
}






/**
 *     积金扣减函数
 说明：积金冲销扣减，积金兑奖扣减
                         按照帐户进行扣减
 输入：
	sContractNo 		 兑奖帐号
 	fTotalCount 		 兑奖积金
 	sLoginAccept		 操作流水
 	sLoginNo			   操作工号
 	sOpCode				   操作代码
 	sOpTime				   操作时间
 	iSysYearMonth		 当前时间
 	sChangeType 		 变更类型

 返回
 	pfTotalCountRemain   剩余的兑换积金   >0 表示兑奖时出现透支 ,报错
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
	int    	iCurrentYearMonth=0;	/*初始值为Min_ym，循环增加*/
	long    iMinYearMonth=0;
	double  fPreFund=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];
	char	vOpTime[17+1];
	
	/*ng解耦 by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char set_strp[1024];

	init(v_parameter_array);
	init(set_strp);
	/*ng解耦 by dx end*/

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
		sprintf(retMsg,"取系统时间错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	/*根据帐户查找当前积金， 已经消费积金，帐户状态,最小年月*/
	EXEC SQL	select current_fund, total_used, to_char(contract_status), min_ym
	              into :fCurrentFund,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConFund
	             where contract_no = to_number(:sContractNo);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639367");
		sprintf(retMsg,"查询帐户积金信息错误[%d]",SQLCODE);
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
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
		strcpy(retCode,"639368");
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}
	/*当前积金不足，不允许兑换*/
	if(fCurrentFund < fTotalCount)
	{
		pubLog_Debug(_FFL_,"","","要求积金=%f,用户积金=%f\n",fTotalCount,fCurrentFund);
		sprintf(retMsg,"用户可用的积金不够该操作");
		strcpy(retCode,"639369");
		return -1;
	}

	pubLog_Debug(_FFL_,"","","用户当前积金=%lf,iMinYearMonth=%d\n",fCurrentFund,iMinYearMonth);

	pubLog_Debug(_FFL_,"","","*****************积金兑换开始**********************");

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

		pubLog_Debug(_FFL_,"","","当前操作表%s,系统时间为[%d] sSqlStr[%s]",
			sTableName,iSysYearMonth, sSqlStr);

		EXEC SQL PREPARE s3760_first01 from :sSqlStr;
		EXEC SQL DECLARE s3760_cur1 CURSOR for s3760_first01;
		EXEC SQL OPEN s3760_cur1 using :sContractNo;
		while(fTotalCountRemain > 0)/*游标选取正确*/
		{
			EXEC SQL fetch s3760_cur1 into :fRemainFund,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","取动态表格%s错误,由于当前表被取空，错误代码是%d\n",sTableName,SQLCODE);
				break;
			}
			
			Trim(sDetailCode);
			if(fRemainFund==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","从%s中取得当月有效积金fRemainFund=%lf sDetailCode=%s\n",sTableName,fRemainFund,sDetailCode);
#endif
			fPreFund=fRemainFund;                /*记录此次游标位置的积金*/
			if(fRemainFund>=fTotalCountRemain)         /*游标积金大于当前交易需要的积金*/
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
				sprintf(retMsg,"更新帐户积金科目明细表时发生错误[%d]",SQLCODE);
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

				pubLog_Debug(_FFL_,"","","没有找到积金变更明细表变更序号,初始数值设置成[1]\n");

			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;

				pubLog_Debug(_FFL_,"","","设置积金变更明细表变更序号为%d\n",change_seq);

			}
			else
			{
				strcpy(retCode,"639371");
				sprintf(retMsg,"查询最大积金变更序号错误[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				EXEC SQL CLOSE curFundDeduct3;
				return -1;
			}
			EXEC SQL CLOSE curFundDeduct3;
			pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积金变更明细表中dFundChangeDetail\n");
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
				sprintf(retMsg,"向积金变更明细表中插入操作时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;

				pubLog_Debug(_FFL_,"","","向积金变更明细表中插入数据成功\n");

			}


			pubLog_Debug(_FFL_,"","","更新帐户积金科目表\n");

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
				sprintf(retMsg,"出错[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}

			pubLog_Debug(_FFL_,"","","游标大,交易结束更新帐户积金科目标成功\n");
			pubLog_Debug(_FFL_,"","","交易结束,直接跳转到关闭游标\n");

		}
		EXEC SQL CLOSE s3760_cur1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","单表循环结束，取下一张表格的年月为%d\n",iCurrentYearMonth);
		}
	}


	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}



	/*当前积金分表总和 < 总表积金时*/
	if(fTotalCountRemain > 0.005)
	{
		fTotalCount=fCurrentFund;
		strcpy(retCode,"639311");
		sprintf(retMsg,"分表积金不够扣减，发生错误[%d]",SQLCODE);
	}
	/*ng解耦dx
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
		sprintf(retMsg,"正常结束后更新帐户积金表数据出错[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}*/
	/*ng解耦 by dx begin*/

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
		sprintf(retMsg,"正常结束后更新帐户积金表数据出错[%d]",v_ret);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
    /*ng解耦 by dx end*/	


	pubLog_Debug(_FFL_,"","","正常结束后更新帐户积金数据成功\n");


	/*如果 fTotalCountRemain大于0，表示发生积金透支*/
	*pfTotalCountRemain = fTotalCountRemain;
	return 0;
}


/**
 *     积金兑奖函数
              说明：按照帐户进行兑奖，包括允许透支下的插入透支操作
  输入：
	sContractNo 		 兑奖帐号
 	fTotalCount 		 兑奖积金
 	sLoginAccept		 操作流水
 	sLoginNo			 操作工号
 	sOpCode				 操作代码
 	sOpTime				 操作时间
 	iSysYearMonth		 当前时间


 返回
 	retCode              返回代码
 	retMsg               返回信息
 */
int FundAward(const char *sContractNo, double fTotalCount, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	/*ng解耦 by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char set_strp[1024];

	init(v_parameter_array);
	init(set_strp);
	/*ng解耦 by dx end*/
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
		pubLog_Debug(_FFL_,"","","积金账户内积金不足\n");
		return -1;
	}
}


/**积金按帐户或帐户下积金科目扣减函数
 * @inparam iDeductType 调整类型： 未定
 
 *插入dFundChangeDetailYYYYMM(帐户积金变更明细表)，
 *更新dConFundPreYYYYMM(帐户积金科目明细表)
 *更新dConFundDetail(帐户积金科目表)
 *更新dConFund(帐户积金表)

 	lContractNo			积金帐户
 	sDetailCodeIn		积金二级科目  按科目扣减时不能为NULL
 	sChangeType			变更类型
 	fTotalCount			扣减积金
 	lLoginAccept		操作流水
 	sLoginNo			操作工号
 	sOpCode			    操作代码
 	sOpTime				操作时间
 	iSysYearMonth		系统时间

 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

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
	double  fPreFund=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];
	char	vOpTime[17+1];
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];
    
	init(v_parameter_array);
	init(sContractNo);
	/*ng解耦 by yaoxc end*/

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
		sprintf(retMsg,"取系统时间错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}

	/*根据帐户查找当前积金， 透支积金额度，已透支积金，已经消费积金，帐户状态,最早消费年月*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_fund,total_used, to_char(contract_status), min_ym
	              into :fCurrentFund,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConFund
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"639358");
		sprintf(retMsg,"查询帐户积金信息错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fCurrentFund=%lf\n",fCurrentFund);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息iMinYearMonth=%d\n",iMinYearMonth);
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
			sprintf(retMsg,"查询帐户积金科目积金错误[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		if (fCurrentFund < fTotalCount)
		{
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","帐户积金科目的当前积金[%lf]不足于扣除积金[%lf]",fCurrentFund,fTotalCount);
	#endif
			strcpy(retCode,"1000");
			sprintf(retMsg,"帐户积金科目的当前积金[%lf]不足于扣除积金[%lf]",fCurrentFund,fTotalCount);
			return -1;
		}
	}
	else if (fCurrentFund < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户积金的当前积金[%lf]不足于扣除积金[%lf]",fCurrentFund,fTotalCount);
#endif
		strcpy(retCode,"1000");
		sprintf(retMsg,"帐户积金的当前积金[%lf]不足于扣除积金[%lf]",fCurrentFund,fTotalCount);
		return -1;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"639360");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
#endif
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}


#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************积金兑换开始**********************\n");
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
			pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]\n",
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
			pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]\n",
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
		while(fTotalCountRemain > 0)/*游标选取正确*/
		{
			EXEC SQL fetch curFundDeduct1 into :fRemainFund,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","取动态表格%s错误,由于当前表被取空，错误代码是%d\n",sTableName,SQLCODE);
				break;
			}
			Trim(sDetailCode);
			if(fRemainFund==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","从%s中取得的fRemainFund=%lf sDetailCode=%s\n",sTableName,fRemainFund,sDetailCode);
#endif
			fPreFund=fRemainFund;                /*记录此次游标位置的积金*/
			if(fRemainFund>=fTotalCountRemain)         /*游标积金大于当前交易需要的积金*/
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
				sprintf(retMsg,"更新帐户积金科目明细表时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标积金更新成功\n");
			pubLog_Debug(_FFL_,"","","当前游标位置剩余的积金数值为%lf\n",fRemainFund);
			pubLog_Debug(_FFL_,"","","该游标消费的积金数量为%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积金变更明细表中最大的积金变更序号\n",iSysYearMonth);
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
				pubLog_Debug(_FFL_,"","","没有找到积金变更明细表变更序号,初始数值设置成[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","设置积金变更明细表变更之后序号为%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"639362");
				sprintf(retMsg,"查询最大积金变更序号错误[%d]\n",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct2;
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
			EXEC SQL CLOSE curFundDeduct2;
			pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积金变更明细表中dFundChangeDetail\n");
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
				sprintf(retMsg,"向积金变更明细表中插入操作时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","向积金变更明细表中插入数据成功");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","更新帐户积金科目表");
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
				sprintf(retMsg,"更新帐户积金科目表失败[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标大,交易结束更新帐户积金科目标成功");
			pubLog_Debug(_FFL_,"","","交易结束,直接跳转到关闭游标");
#endif
		}
		EXEC SQL CLOSE curFundDeduct1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","单表循环结束，取下一张表格的年月为%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while操作正常结束１.没有透支积金2.发生透支积金");
	pubLog_Debug(_FFL_,"","","当前年月为%d",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","当前年月为%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","更新帐户表前的检查,total_fund=%f\n",fTotalCount);
#endif

	/*更新dconfund表扣积金时，不扣减月积金和年积金和累记积金。只记录使用积金*/
		
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
		sprintf(retMsg,"正常结束后更新帐户积金表数据出错[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}


	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","正常结束后更新帐户积金数据成功fTotalCountRemain=%f",fTotalCountRemain);
#endif

	if(fTotalCountRemain > 0.005)/*发生积金透支*/
	{
		strcpy(retCode,"639366");
		sprintf(retMsg,"发生积金透支错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	return 0;
}

/**
 *     积金冲正函数，按照帐户进行对奖
* @inparam
 	sBackLoginAccept	冲正流水
 	iBackYearMonth		冲正年月
 	sLoginAccept		操作流水
 	sLoginNo			操作工号
 	sOpCode				操作代码
 	sOpTime				操作时间
 	iSysYearMonth		系统年月

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
	/*积分变换类型为1代表此类型是积分兑换*/
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
			sprintf(retMsg,"取帐户积分信息出错[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		Trim(sContractNo);
		pubLog_Debug(_FFL_,"","","获得的该细则产生的积分变化为%lf",fChangeValue);
		pubLog_Debug(_FFL_,"","","当前帐户为%s,备份帐户为[%s]",sContractNo,sBakeContractNo);

		if (strcmp(sBakeContractNo, sContractNo) != 0)
		{
			if(sBakeContractNo[0] != '\0')
			{
				pubLog_Debug(_FFL_,"","","多帐户兑奖冲正出错\n");
				strcpy(retCode,"376915");
				sprintf(retMsg,"多帐户兑奖冲正出错");
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
				sprintf(retMsg,"从帐户积分表中查询帐户总积分出错[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curFundAwardBack2;
				return -1;
			}

			iCurrMinYm=iMinYm;
			init(sBakeContractNo);
			strcpy(sBakeContractNo,sContractNo);
			fAccCount=0;               /*临时帐户清空*/
			fCurrentTotalValue=fCurrentFund;
		}


		/*查询当前年月帐户积分变更明细表中，同操作流水，帐户id,二级积分科目，积分帐户年月下最大的帐户积分变更序号*/
		pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积分变更明细表中最大的积分变更序号",iSysYearMonth);
		Trim(sContractNo);
		Trim(sDetailCode);

		/*判断最小年月*/
		if (iMinYm > iBillYm)
		{
			iCurrMinYm = iBillYm;
		}

		iMinYm=iCurrMinYm;

		init(table_name);
		sprintf(table_name,"dFundChangeDetail%6d",iSysYearMonth);
		pubLog_Debug(_FFL_,"","","数据为[%s],[%s],[%s],[%d]\n",sLoginAccept,sContractNo,sDetailCode,iBillYm);
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
			pubLog_Debug(_FFL_,"","","没有找到该条件下变更序号,初始数值设置成[1]");
			EXEC SQL CLOSE curFundAwardBack3;
		}
		else if(SQLCODE==0)
		{
			iChangeSeq=iChangeSeq+1;
			EXEC SQL CLOSE curFundAwardBack3;
			pubLog_Debug(_FFL_,"","","当前设置的变更序号为%d",iChangeSeq);
		}
		else
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","查找变更序号时，发生错误[%d]",SQLCODE);
#endif
			sprintf(retMsg,"查询最大积分变更序号[%d]",SQLCODE);
			EXEC SQL CLOSE curFundAwardBack3;
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","向当前系统时间下的积分变更明细表中插入当前操作的结果");
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
			sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}
		fCurrentTotalValue=temp2;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","向积分变更明细表中插入数据成功\n");
#endif
		fAccCount+=fChangeValue;

		pubLog_Debug(_FFL_,"","","contract_no=%s;sDetailCode=%s\n",sContractNo,sDetailCode);
		pubLog_Debug(_FFL_,"","","更新%d年月的帐户积分科目明细表\n",iBillYm);
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
			pubLog_Debug(_FFL_,"","","成功对帐户科目明细表更新数值\n");
		}
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"更新帐户科目明细表可用积分失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		pubLog_Debug(_FFL_,"","","选择积分科目表下该二级科目下的最小年月\n");
		EXEC SQL	select min_ym
		              into :iDetailMinYm
		              from dConFundDetail
		             where contract_no = to_number(:sContractNo)
		               and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"选择积分科目表下该二级科目下的最小年月失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}

		if( iDetailMinYm>iBillYm )
		{
			iDetailMinYm = iBillYm;
		}

		pubLog_Debug(_FFL_,"","","更新积分科目表\n");
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
			sprintf(retMsg,"更新积分科目表失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curFundAwardBack2;
			return -1;
		}
	}
	EXEC SQL CLOSE curFundAwardBack2;
	
	/*游标关闭，将最后的帐户下的数据写到帐户积分表中*/

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
			sprintf(retMsg,"更新帐户积分表出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","将累积计算的积分%lf加到该帐户%s的积分表中，成功\n",fAccCount,sBakeContractNo);
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
			sprintf(retMsg,"更新帐户科目明细表可用积分失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}
	return 0;
}
