/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/**
 *	<B>积分透支冲销</B>
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

#define _DEBUG_

/*#define init(a) do { memset(a,0,sizeof(a)); } while(0)
*/
EXEC SQL INCLUDE SQLCA;
EXEC SQL BEGIN DECLARE SECTION;
	char tmp_buf[2048];
EXEC SQL END DECLARE SECTION;

void DecYm(int* ym)
{
	if((*ym)%100 == 1)
	{
		*ym = *ym - 89;
	}
	else
	{
		*ym = *ym - 1;
	}
}
void IncYm(int* ym)
{
	if((*ym)%100 == 12)
	{
		*ym = *ym + 89;
	}
	else
	{
		*ym = *ym + 1;
	}
}

void IncYmEx(int* ym,int months)
{
	int i;
	for(i=0; i < months; i ++)
	{
		IncYm(ym);
	}
}

void DecYmEx(int* ym,int months)
{
	int i;
	for(i=0; i < months; i ++)
	{
		DecYm(ym);
	}
}

/**
* 提取操作了流水
*
* @author	lab
*
* @version   %I%, %G%
*
* @since	 1.00
*
* @outparam  操作流水 long
*
* @return	0:成功，1:ORACLE错误，2:返回数据无效
*/
int getMaxSysAcc(long *accept)
{
		long loginAccept;

		EXEC SQL SELECT sMaxSysAccept.nextval
						INTO :loginAccept
						FROM DUAL;
		if (SQLCODE != 0)
		{
			*accept = 0;
			return -1;
		}
		else
		{
			*accept = loginAccept;
		}
		return 0;
}


/*透支积分冲销
* @inparam
 	contractNo			积分帐户
 	oweAccept			透支流水
 	opAccept			操作流水
 	loginNo				操作工号
 	sOpCode				操作代码
 	sOpTime				操作时间

* @outparam
 	retCode				返回代码
 	retMsg				返回信息
 */
int overdraftReduce(long contractNo, long oweAccept, long opAccept, char *loginNo, char *opcode, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long iContractNo, iOweAccept;
		char iLoginNo[6+1];
		long detailCode, loginAccept;
	  double contractPoint=0;		/*账户可用积分*/
	  double markTypePoint=0;		/*二级积分科目可用积分*/
	  double markTypePointYM=0;		/*二级积分科目某年月可用积分*/
	  double overdraftPoint=0;		/*账户需要冲销的透支积分总数*/
	  double reducePoint=0;			/*该次操作已经冲销的积分*/
	  double doPoint=0;				/*单次冲销积分数*/
	  double payedOweMark=0;		/*该次冲销前已经冲销透支积分*/
	  	char opTime[20+1], currentYM[6+1], minYM[6+1], oweYm[6+1];
	  	char finishFlag[1+1];
	  	char opCode[4+1];
	  	 int iSysYearMonth=0;
	  double fTotalCountRemain=0;
	    char sLoginAccept[14+1];
	    char sContractNo[14+1];
	EXEC SQL END DECLARE SECTION;
		/*ng解耦 by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];

	init(v_parameter_array);
	/*ng解耦 by dx end*/

	init(iLoginNo);
	init(retCode);
	init(retMsg);
	init(minYM);
	init(opTime);
	init(currentYM);
	init(finishFlag);
	init(opCode);
	init(sLoginAccept);
	init(sContractNo);
	init(oweYm);

	strcpy(iLoginNo, loginNo);
	iContractNo = contractNo;
	iOweAccept = oweAccept;
	loginAccept = opAccept;
	strncpy(opCode, opcode, sizeof(opCode)-1);
	sprintf(sLoginAccept, "%ld", loginAccept);
	sprintf(sContractNo, "%ld", iContractNo);

	strcpy(retCode, "000000");
	strcpy(retMsg, "透支积分冲销成功!");

/*获取系统时间*/
	EXEC SQL SELECT to_char(sysdate, 'yyyymmdd hh24:mi:ss') INTO :opTime FROM dual;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1001");
		sprintf(retMsg, "取系统时间失败！[%d]",SQLCODE);
		return -1;
	}
	Trim(opTime);

	EXEC SQL	SELECT to_char(sysdate, 'YYYYMM'), to_number(to_char(sysdate,'YYYYMM'))
				INTO :currentYM, :iSysYearMonth
				FROM dual;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1002");
		strcpy(retMsg, "取系统时间失败！");
		return -1;
	}
	Trim(currentYM);


/*取系统流水*/
	if (getMaxSysAcc(&loginAccept) != 0)
	{
		strcpy(retCode, "PM1003");
		strcpy(retMsg, "取流水失败！");
		return -1;
	}

/*获取该账户该流水下的透支积分数*/
	EXEC SQL select owe_mark, payed_owe_mark, Owe_ym
			   into :overdraftPoint, :payedOweMark, :oweYm
			   from dCustOweMark
			  where Owe_Accept=:iOweAccept
			  	and contract_no=:iContractNo;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1004");
		sprintf(retMsg, "取账户[%ld]流水[%ld]下的透支积分数失败！[SQLCODE:%d]", iContractNo, iOweAccept, SQLCODE);
		return -1;
	}

	overdraftPoint -= payedOweMark;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","overdraftPoint:%f",overdraftPoint);
#endif
	if (overdraftPoint == 0)
	{
		return 0;
	}

	if (MarkAwardReduce(sContractNo, overdraftPoint, sLoginAccept,
			iLoginNo, opcode,	opTime, iSysYearMonth,"3", &fTotalCountRemain, retCode, retMsg) != 0)
	{
		printf("FAILED,retcode:%s, retmsg:%s\n",retCode,retMsg);
		return -1;
	}
	reducePoint = overdraftPoint - fTotalCountRemain;

	if (fTotalCountRemain == 0)
	{
		finishFlag[0] = 'y';

		/*冲销完毕，将记录移到积分透支历史表*/
		init(tmp_buf);
		strcpy(tmp_buf, "insert into dCustOweMarkHis");
		strcat(tmp_buf, oweYm);
		strcat(tmp_buf, "(Owe_Accept, contract_no, Owe_ym, owe_mark, payed_owe_mark,");
		strcat(tmp_buf, "Finish_Flag, login_no, op_time, op_code, back_flag)");
		strcat(tmp_buf, "(select Owe_Accept, contract_no, Owe_ym, owe_mark, owe_mark, ");
		strcat(tmp_buf, "'1', :login_no, to_date(:opTime,'YYYYMMDD HH24:MI:SS'), op_code, '0'");
		strcat(tmp_buf, "from dCustOweMark where Owe_Accept=:iOweAccept and contract_no=:iContractNo)");

		EXEC SQL PREPARE pre_dCustOweMarkHisYM from :tmp_buf;
		EXEC SQL EXECUTE pre_dCustOweMarkHisYM using :iLoginNo, :opTime, :iOweAccept, :iContractNo;
		if (SQLCODE != 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","tmp_buf:%s--login_no:%s, opTime:%s, iOweAccept:%ld, iContractNo:%ld",tmp_buf, iLoginNo, opTime, iOweAccept, iContractNo);
#endif
			strcpy(retCode, "000011");
			sprintf(retMsg, "冲销完毕，将记录移到积分透支历史表失败！[SQLCODE:%d]", SQLCODE);
			return -1;
		}

		EXEC SQL delete from dCustOweMark where Owe_Accept=:iOweAccept and contract_no=:iContractNo;
		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1005");
			sprintf(retMsg, "冲销完毕，将记录移到积分透支历史表失败！[%d]",SQLCODE);
			return -1;
		}
	}
	else
	{
		finishFlag[0] = 'n';
		/*冲销没有完毕，修改积分透支表*/
		EXEC SQL update dCustOweMark
					set payed_owe_mark=payed_owe_mark+:reducePoint,
						login_no=:iLoginNo,
						op_time=to_date(:opTime,'YYYYMMDD HH24:MI:SS')
				  where Owe_Accept=:iOweAccept
				  	and contract_no=:iContractNo;
		if (SQLCODE != 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","reducePoint:%ld, iLoginNo:%s, opTime:%s, iOweAccept:%ld, iContractNo:%ld",
							reducePoint, iLoginNo, opTime, iOweAccept, iContractNo);
#endif
			strcpy(retCode, "PM1006");
			sprintf(retMsg, "冲销没有完毕，修改积分透支表失败！[SQLCODE:%d]", SQLCODE);
			PUBLOG_DBDEBUG("");
			return -1;
		}
	}

	/*插入积分透支冲销记录表(dMarkOweReduceYYYYMM)*/
	init(tmp_buf);
	strcpy(tmp_buf, "insert into dMarkOweReduce");
	strcat(tmp_buf, currentYM);
	strcat(tmp_buf, "(op_accept, contract_no, Owe_Accept, Before_Value, Paid_Value,");
	strcat(tmp_buf, "Finish_Flag, login_no, op_time, back_flag, op_code)");
	strcat(tmp_buf, " values(:loginAccept, :iContractNo, :iOweAccept, :payedOweMark, :reducePoint,");
	strcat(tmp_buf, ":finishFlag, :iLoginNo, to_date(:opTime,'YYYYMMDD HH24:MI:SS'), '0', :opCode)");

	EXEC SQL PREPARE pre_dMarkOweReduceYM from :tmp_buf;
	EXEC SQL EXECUTE pre_dMarkOweReduceYM using :loginAccept, :iContractNo, :iOweAccept, :payedOweMark, :reducePoint,
												:finishFlag, :iLoginNo, :opTime, :opCode;
	if (SQLCODE != 0)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","loginAccept:%ld, iContractNo:%ld, iOweAccept:%ld, payedOweMark:%f,\n"
			   "reducePoint:%f, finishFlag:%s, iLoginNo:%s, opTime:%s\n",
			   loginAccept, iContractNo, iOweAccept, payedOweMark,
			   	 reducePoint,finishFlag, iLoginNo, opTime);
#endif
		strcpy(retCode, "PM1007");
		sprintf(retMsg, "插入积分透支冲销记录表失败![SQLCODE:%d]", SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}

	/*修改账户积分表,此表的其他字段已在MarkAwardReduce函数中处理！*/
	/*
	EXEC SQL update dConMark
				set owe_point=owe_point-:reducePoint
			  where contract_no=:iContractNo;

	if (SQLCODE != 0)
	{
		sprintf(retCode, "PM1008");
		sprintf(retMsg, "修改账户积分表失败！");
		return -1;
	}*/
	/*ng解耦 by dx begin*/

    init(v_parameter_array);
		
	sprintf(v_parameter_array[0],"%f",reducePoint);
	strcpy(v_parameter_array[1],sContractNo);
    
	v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
						  	opCode,opAccept,iLoginNo,"overdraftReduce",
							sContractNo,
							"owe_point=owe_point-to_number(:reducePoint)","",v_parameter_array);
	printf("OrderUpdateConMark1 ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		strcpy(retCode, "PM1008");
		sprintf(retMsg, "修改账户积分表失败！[%d]",v_ret);
		return -1;
	}
    /*ng解耦 by dx end*/	

	return 0;
}


/*处理透支函数
* @inparam
 	lContractNo			积分帐户 对该帐户进行透支处理

 	loginAccept			操作流水
 	loginNo				操作工号
 	sOpTime				操作时间
 	sOpCode				操作代码

* @outparam
 	retCode				返回代码
 	retMsg				返回信息
*/
int oweMark(long lContractNo, long loginAccept, char *loginNo, char *opTime, char *sOpCode, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long iContractNo, oweAccept;
	EXEC SQL END DECLARE SECTION;

	iContractNo = lContractNo;

	EXEC SQL DECLARE cur_dCustOweMark cursor for
					  select Owe_Accept
						from dCustOweMark
					   where contract_no = :iContractNo;

	EXEC SQL OPEN cur_dCustOweMark;
	while (1)
	{
		EXEC SQL FETCH 	cur_dCustOweMark into :oweAccept;
		if (SQLCODE == NOTFOUND)
		{
			break;
		}
		else if (SQLCODE != 0)
		{
			strcpy(retCode, "OM0001");
			sprintf(retMsg, "查找透支积分大于０的账户失败！[SQLCODE:%d]", SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE cur_dCustOweMark;
			return -1;
		}

		overdraftReduce(iContractNo, oweAccept, loginAccept, loginNo, sOpCode, retCode, retMsg);
	}

	EXEC SQL CLOSE cur_dCustOweMark;
	return 0;
}


/*
 *按积分科目加积分函数
 *更新dMarkChangeDetailYYYYMM(帐户积分变更明细表)，
 *更新dConMarkPreYYYYMM(帐户积分科目明细表)
 *更新dConMarkDetail(帐户积分科目表)
 *更新dConMark(帐户积分表)
 * @inparam

 	lContractNo			积分帐户
 	sDetailCodeIn		积分二级科目
 	sChangeType			变更类型
 	totalPoint			加奖积分
 	billYearMonth		帐务年月
 	opAccept			操作流水
 	sLoginNo			操作工号
 	sOpCode				操作代码
 	sOpTime				操作时间


 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

 */
int markRedressal(long contractNo, long detailCode, double totalPoint, char *billYearMonth,
			char *changeType, long opAccept, char *loginNo, char *opTime, char *opcode, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char iOpTime[21+1], iBill_YM[6+1], iLoginNo[6+1], iChangeType[2+1], opCode[4+1],sysyearmonth[6+1];
		long iContractNo, iDetailCode, iOpAccept;
		int changeSeq;
		double iTotalPoint, preDetailValue, preTotalValue;
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
	iTotalPoint = totalPoint;
	iOpAccept = opAccept;
	strcpy(iOpTime, opTime);
	strcpy(iBill_YM, billYearMonth);
	strcpy(iLoginNo, loginNo);
	strcpy(iChangeType, changeType);
	strncpy(opCode, opcode, sizeof(opCode)-1);

	strcpy(retCode, "000000");

/******取帐户积分变更序号******/
	EXEC SQL select to_char(sysdate,'yyyymm') into :sysyearmonth from dual;
	if (SQLCODE != 0 )
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "获取系统时间错误[%d]", SQLCODE);
		return -1;
	}
	init(tmp_buf);
	strcpy(tmp_buf, "select nvl(max(Change_Seq), 0) from dMarkChangeDetail");
	strcat(tmp_buf, sysyearmonth);
	strcat(tmp_buf, " where op_accept=:iOpAccept");
	strcat(tmp_buf, " and contract_no=:iContractNo");
	strcat(tmp_buf, " and detail_code=:iDetailCode");
	strcat(tmp_buf, " and bill_ym=:iBill_YM");
	
	EXEC SQL PREPARE pro_markRedressal1 from :tmp_buf;
	EXEC SQL DECLARE cur_markRedressal1 CURSOR for pro_markRedressal1;
	EXEC SQL OPEN cur_markRedressal1 using :iOpAccept, :iContractNo, :iDetailCode, :iBill_YM;
	EXEC SQL FETCH cur_markRedressal1 INTO :changeSeq;

	if(SQLCODE==1403)
	{
		changeSeq=1;

		pubLog_Debug(_FFL_,"","","没有找到积分变更明细表变更序号,初始数值设置成[1]");
	}
	else if(SQLCODE==0)
	{
		changeSeq=changeSeq+1;

		pubLog_Debug(_FFL_,"","","设置积分变更明细表变更序号为%d",changeSeq);

	}
	else
	{
		strcpy(retCode, "PM1101");
		sprintf(retMsg, "获取帐户积分变更序号失败!流水:%ld, 账号:%ld, 积分科目:%ld, 帐务年月:%s,[SQLCODE:%d]",
						opAccept, contractNo, detailCode, billYearMonth, SQLCODE);
		EXEC SQL CLOSE cur_markRedressal1;
		return -1;
	}

	EXEC SQL CLOSE cur_markRedressal1;

/******取更新前详细积分******/
	EXEC SQL select nvl(Available_Point, 0)
			   into :preDetailValue
			   from dConMarkDetail
			  where contract_no=:contractNo
			  	and detail_code=:detailCode;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1102");
		sprintf(retMsg, "获取更新前详细积分错误！账号:%ld, 积分科目:%ld[SQLCODE:%d]", contractNo, detailCode, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preDetailValue = 0;
	}

/******取更新前总积分******/
	EXEC SQL select nvl(current_point, 0)
			   into :preTotalValue
			   from dConMark
			  where contract_no=:contractNo;
	if (SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		strcpy(retCode, "PM1103");
		sprintf(retMsg, "获取更新前总积分错误！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
		return -1;
	}
	else if (SQLCODE == NOTFOUND)
	{
		preTotalValue = 0;
	}


/******插入dMarkChangeDetailYYYYMM(帐户积分变更明细表)******/
	init(tmp_buf);
	strcpy(tmp_buf, "insert into dMarkChangeDetail");
	strcat(tmp_buf, sysyearmonth);
	strcat(tmp_buf, " (op_accept, contract_no, detail_code, bill_ym, Change_Seq, ChangeType, Pre_Detail_Value, Pre_Total_Value");
	strcat(tmp_buf, ",Change_Value, After_Detail_Value, After_Total_Value, login_no, op_time, back_flag, op_code) ");
	strcat(tmp_buf, " values(:opAccept, :contractNo, :detailCode, to_number(:billYearMonth), :changeSeq, :iChangeType, :preDetailValue, :preTotalValue");
	strcat(tmp_buf, ", :totalPoint1, :preDetailValue+:totalPoint2, :preTotalValue+:totalPoint3, :iLoginNo, to_date(:opTime,'YYYYMMDD HH24:MI:SS'), 0, :opCode)");

	EXEC SQL PREPARE pro_updateConMark2 from :tmp_buf;
	EXEC SQL EXECUTE pro_updateConMark2 using :opAccept,:contractNo,:detailCode,:billYearMonth,:changeSeq, :iChangeType, :preDetailValue,:preTotalValue,
											  :totalPoint,:preDetailValue,:totalPoint,:preTotalValue,:totalPoint,:iLoginNo,:opTime,:opCode;

	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_,"","","sql:%s\n optime :%s\n", tmp_buf, opTime);
		pubLog_Debug(_FFL_,"","","op_accept:%ld, contract_no:%ld, detail_code:%ld, bill_ym:%s, \n"
			   "Change_Seq:%d, ChangeType:%s, Pre_Detail_Value:%d, Pre_Total_Value:%d \n"
			   "Change_Value:%d, After_Detail_Value:%d, After_Total_Value:%d, login_no:%s, \n"
			   "op_time:%s, back_flag:%s\n",
			   opAccept, contractNo, detailCode, billYearMonth,
			   changeSeq, iChangeType, preDetailValue, preTotalValue,
			   totalPoint, preDetailValue+totalPoint, preTotalValue+totalPoint,iLoginNo,
			     opTime, "0");
		strcpy(retCode, "PM1104");
		sprintf(retMsg, "插入帐户积分变更明细表(dMarkChangeDetailYYYYMM)!流水:%ld, 账号:%ld, 积分科目:%ld, 帐务年月:%s, 序号:%d, iChangeType:%s [SQLCODE:%d]",
						opAccept, contractNo, detailCode, billYearMonth, changeSeq, iChangeType, SQLCODE);
		return -1;
	}

/******更新dConMarkPreYYYYMM(帐户积分科目明细表)******/

	init(tmp_buf);
	strcpy(tmp_buf, "update dConMarkPre");
	strcat(tmp_buf, billYearMonth);
	strcat(tmp_buf, " set Total_Mark=Total_Mark+:totalPoint, Available_Mark=Available_Mark+:totalPoint,");
	strcat(tmp_buf, " op_time=to_date(:opTime, 'YYYYMMDD HH24:MI:SS') where contract_no=:contractNo and detail_code=:detailCode");

	EXEC SQL PREPARE pro_markRedressal4 from :tmp_buf;
	EXEC SQL EXECUTE pro_markRedressal4 using :totalPoint,:totalPoint,:opTime,:contractNo,:detailCode;

	if (SQLCODE == NOTFOUND)
	{
		init(tmp_buf);
		strcpy(tmp_buf, "insert into dConMarkPre");
		strcat(tmp_buf, billYearMonth);
		strcat(tmp_buf, "(contract_no,detail_code,bill_ym,Total_Mark,Available_Mark,op_time)");
		strcat(tmp_buf, " values(:contractNo,:detailCode,:billYearMonth,:totalPoint,:totalPoint,to_date(:opTime,'YYYYMMDD HH24:MI:SS'))");

		EXEC SQL PREPARE pro_markRedressal3 from :tmp_buf;
		EXEC SQL EXECUTE pro_markRedressal3 using :contractNo,:detailCode,:billYearMonth,:totalPoint,:totalPoint,:opTime;

		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1105");
			sprintf(retMsg, "插入帐户积分科目明细表(dConMarkPreYYYYMM)失败！账号:%ld, 积分科目:%ld, 帐务年月:%s, [SQLCODE:%d]",
						contractNo, detailCode, billYearMonth, SQLCODE);
			return -1;
		}

	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1105");
		sprintf(retMsg, "更新帐户积分科目明细表(dConMarkPreYYYYMM)失败！账号:%ld, 积分科目:%ld, 帐务年月:%s, [SQLCODE:%d]",
					contractNo, detailCode, billYearMonth, SQLCODE);
		return -1;
	}

/******更新dConMarkDetail(帐户积分科目表)******/
	EXEC SQL update dConMarkDetail
				set Total_Point=Total_Point+:iTotalPoint, Available_Point=Available_Point+:iTotalPoint,
					max_ym=:iBill_YM, op_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
			  where contract_no = :iContractNo
			  	and detail_code = :iDetailCode;

	if (SQLCODE == NOTFOUND)
	{
		EXEC SQL insert into dConMarkDetail(contract_no, detail_code, min_ym, max_ym,
											Total_Point, Available_Point, op_time)
					  				 values(:iContractNo, :iDetailCode, :iBill_YM, :iBill_YM,
					  				 		:iTotalPoint, :iTotalPoint, to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'));
		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1106");
			sprintf(retMsg, "插入帐户积分科目表(dConMarkDetail)失败！账号:%ld, 积分科目:%ld, [SQLCODE:%d]",
						contractNo, detailCode, SQLCODE);
			return -1;
		}
	}
	else if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1107");
		sprintf(retMsg, "更新帐户积分科目表(dConMarkDetail)失败！账号:%ld, 积分科目:%ld, [SQLCODE:%d]",
						contractNo, detailCode, SQLCODE);
		return -1;
	}

/*****更新dConMark(帐户积分表)******/
	if(iDetailCode==3)/*如果为转赠积分科目是不加入月积分和年积分和累记积分*/
	{
		if(strcmp(opCode,"2419")==0)
		{
			/*ng解耦
			EXEC SQL update dConMark
						set current_point=current_point+:iTotalPoint,
							add_point=add_point+:iTotalPoint,
							current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
					  where contract_no=:iContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			} 
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
    		sprintf(sContractNo,"%ld",iContractNo);

    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",iTotalPoint);
			sprintf(v_parameter_array[1],"%f",iTotalPoint);
			strcpy(v_parameter_array[2],iOpTime);
			strcpy(v_parameter_array[3],sContractNo);
    		
			v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
								  	opCode,iOpAccept,iLoginNo,"markRedressal",
									sContractNo,
									"current_point=current_point+ to_number(:v0),add_point=add_point+to_number(:v1),current_time=to_date(:v2,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark1 ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	 
		}
		else
		{ 
			/*ng解耦
			EXEC SQL update dConMark
						set current_point=current_point+:iTotalPoint,
							current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
					  where contract_no=:iContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
    		sprintf(sContractNo,"%ld",iContractNo);
    		
    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",iTotalPoint);
			strcpy(v_parameter_array[1],iOpTime);
			strcpy(v_parameter_array[2],sContractNo);
    		
			v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
								  	opCode,iOpAccept,iLoginNo,"markRedressal",
									sContractNo,
									"current_point=current_point+to_number(:iTotalPoint),current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark2 ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	
		}
	}
	else if( iDetailCode==2 && strcmp(opCode,"3744")==0 )
	{
		/*ng解耦
		EXEC SQL update dConMark
					set current_point=current_point+:iTotalPoint,
						current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
				  where contract_no=:iContractNo;
		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1108");
			sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	sprintf(sContractNo,"%ld",iContractNo);
    	
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",iTotalPoint);
		strcpy(v_parameter_array[1],iOpTime);
		strcpy(v_parameter_array[2],sContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
							  	opCode,iOpAccept,iLoginNo,"markRedressal",
								sContractNo,
								"current_point=current_point+to_number(:iTotalPoint),current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark3 ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode, "PM1108");
			sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/	
	}
	else
	{
		if(strcmp(opCode,"2418")==0)
		{
			/*ng解耦
			EXEC SQL update dConMark
						set current_point=current_point+:iTotalPoint,
					ng解耦*//*Month_point=Month_point+:iTotalPoint,
							year_point=year_point+:iTotalPoint, 
							add_point=add_point+:iTotalPoint,*/
					  /*解耦current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
					  where contract_no=:iContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
    		sprintf(sContractNo,"%ld",iContractNo);
    		
    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",iTotalPoint);
			strcpy(v_parameter_array[1],iOpTime);
			strcpy(v_parameter_array[2],sContractNo);
    		
			v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
								  	opCode,iOpAccept,iLoginNo,"markRedressal",
									sContractNo,
									"current_point=current_point+to_number(:iTotalPoint),current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark4,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	
		}
		else
		{
			/*ng解耦
			EXEC SQL update dConMark
						set current_point=current_point+:iTotalPoint,
							Month_point=Month_point+:iTotalPoint,
							year_point=year_point+:iTotalPoint, 
							add_point=add_point+:iTotalPoint,
							current_time=to_date(:iOpTime,'YYYYMMDD HH24:MI:SS')
					  where contract_no=:iContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
    		sprintf(sContractNo,"%ld",iContractNo);
    		
    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",iTotalPoint);
			sprintf(v_parameter_array[1],"%f",iTotalPoint);
			sprintf(v_parameter_array[2],"%f",iTotalPoint);
			sprintf(v_parameter_array[3],"%f",iTotalPoint);
			strcpy(v_parameter_array[4],iOpTime);
			strcpy(v_parameter_array[5],sContractNo);
    		
			v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
								  	opCode,iOpAccept,iLoginNo,"markRedressal",
									sContractNo,
									"current_point=current_point+to_number(:v0),Month_point=Month_point+to_number(:v1),year_point=year_point+to_number(:v2),add_point=add_point+to_number(:v3), current_time=to_date(:v4,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark5,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", contractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	
		}
	}
	return 0;
}



/*查询标识类资源代码存储的年表函数
输入：
	goodsType
	groupId
输出：
	idgoosNo
	tablename
返回
   -1 表示查询数据为空
   0  成功返回数据 
   >0 表示函数出错
   */
int getMarkExchRes( char * goodsType,  char * idgoodsNo, char * tableName,char * groupId,char *noYear )
{
	EXEC SQL BEGIN DECLARE SECTION;
	char snoYear[4+1];
	char stableName[16+1];
	char sSqlStr[3000];
	char sidgoodsNo[16+1];
	long lnoSequence=0;
	EXEC SQL END DECLARE SECTION;
	
	init(snoYear);
	init(stableName);
	init(sSqlStr);
	init(sidgoodsNo);
	

	/***从资源表dresidgoodsbatch取出存放资源号段的表名**/
	EXEC SQL DECLARE cursor1 CURSOR for select distinct no_year
	          from dresidgoodsbatch
	         where goods_type=:goodsType
	           and record_status='1'
	         order by no_year;
	EXEC SQL OPEN cursor1;
	while(1)
	{
		EXEC SQL FETCH cursor1 into :snoYear;
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_,"","","markPublic:getMarkExchRes查询为空:[%d][%s]",SQLCODE,SQLERRMSG);
			PUBLOG_DBDEBUG("");
			break;
		}
		EXEC SQL DECLARE cursor2 CURSOR for select distinct dresidgoods_tablename ,to_number(substr(dresidgoods_tablename,13,4)) as aa
		                                      from dresidgoodsbatch
		                                     where goods_type=:goodsType
	                                           and no_year=:snoYear
	                                           and record_status='1'
	                                         order by aa;
	    EXEC SQL OPEN cursor2;
	    
	    while(1)
	    {
	    	EXEC SQL FETCH cursor2 into :stableName;
	    	if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","markPublic:getMarkExchRes查询表名为空:[%d][%s]",SQLCODE,SQLERRMSG);
				PUBLOG_DBDEBUG("");
				break;
			}
			sprintf(sSqlStr,"select nvl(min(no_sequence),-1)"
			                "  from %s"
			                " where goods_type = :v1"
			                "   and idgoods_status= '2'    "
			                "   and current_group_id=:v2      "
			                "   and no_year=:v3         "
                            , stableName);
			
			Trim(groupId);
			Trim(snoYear);
			Trim(goodsType);
			pubLog_Debug(_FFL_,"","","stableName=%s\n,sgoodsType=%s\n,sgroup_Id=%s\n,no_year=%s\n",stableName,goodsType,groupId,snoYear);
    		
    		EXEC SQL PREPARE pre1 From :sSqlStr;
    		EXEC SQL DECLARE cursor3 CURSOR for pre1;
    		EXEC SQL open cursor3 using :goodsType, :groupId,:snoYear;
    		pubLog_Debug(_FFL_,"","","sSqlStr=%s",sSqlStr);
    		EXEC SQL FETCH cursor3 into :lnoSequence;
  
    		if(SQLCODE!=0&&SQLCODE!=1403)
    		{
    			return 1104;
    		}
    		pubLog_Debug(_FFL_,"","","lnoSequence=%ld",lnoSequence);
			if(SQLCODE==1403||lnoSequence==-1)
			{
				pubLog_Debug(_FFL_,"","","当前表中不存在该groupid的资源代码，取下张表[%d]",SQLCODE);
				PUBLOG_DBDEBUG("");
				EXEC SQL CLOSE cursor3;
				continue;
			}
			else
			{	
				sprintf(sSqlStr,"select idgoods_no "
			                "  from %s "
			                " where goods_type = :v1"
			                "   and idgoods_status= '2'    "
			                "   and current_group_id=:v2      "
			                "   and no_year=:v3         "
			                "   and no_sequence=:lnoSequence "
                            , stableName);
				
    			EXEC SQL PREPARE pre2 From :sSqlStr;
    			EXEC SQL DECLARE cursor4 CURSOR for pre2;
    			EXEC SQL open cursor4 using :goodsType, :groupId,:snoYear,:lnoSequence;
    			EXEC SQL FETCH cursor4 into :sidgoodsNo;
    			if(SQLCODE!=0)
    			{
    				pubLog_DBErr(_FFL_,"","1105","查询最小序列错误");
    				PUBLOG_DBDEBUG("");
    				return 1105;
    			}
    			strcpy(idgoodsNo,sidgoodsNo);
				strcpy(tableName,stableName);
				strcpy(noYear,snoYear);
				EXEC SQL CLOSE cursor1;
				EXEC SQL CLOSE cursor2;
				EXEC SQL CLOSE cursor3;
				EXEC SQL CLOSE cursor4;
				goto end;
			}
			
				
	    }
		EXEC SQL CLOSE cursor2;
	}
	EXEC SQL CLOSE cursor1;
	strcpy(sidgoodsNo,"");
end:
	
	if(sidgoodsNo[0]==0)
	{
		pubLog_Debug(_FFL_,"","","函数调用循环查询完毕，没有发现该groupid下的资源\n");
		return -1;
	}
	else
	{
		return 0;
	}
}


/**********插入PublicMarkOprYYYYMM表函数*********
* @inparam
	loginAccept			操作流水
	phoneNo				手机号码
	sgoodsType		    实物类别
	sidgoodsNo			资源代码   可为空
	volume				兑换数量
	consumeMark			消费积分
	favourMark			优惠积分
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
int PublicMarkOpr(long loginAccept, const char *phoneNo,const char * goodsType, const char * idgoodsNo, long volume, double consumeMark,  double favourMark,
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
	double	dConsumeMark;
	double	dFavourMark;
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
	dConsumeMark 	= consumeMark;
	dFavourMark 	= favourMark;
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
    EXEC SQL SELECT id_no,contract_no,sm_code,group_no
    		 INTO   :lIdNo,:lContractNo,:sSmCode,:vGroupId
    		 FROM 	dCustMsg
    		 WHERE 	phone_no = :sPhoneNo
    		 AND 	substr(run_code,2,1) < 'a';

	if(SQLCODE != 0)
	{
		pubLog_DBErr(_FFL_,"","123401","markPublic:PublicMarkOpr查询dCustMsg出错:[%d][%s]",SQLCODE,SQLERRMSG);
		return 123401;
	}
	Trim(vGroupId);
	/***从dloginmsg表里取指定工号的记录***/
	pubLog_Debug(_FFL_,"","","sLoginNo=%s",sLoginNo);
	EXEC SQL SELECT org_code,nvl(ip_address,'0.0.0.0')
        	 INTO   :sOrgCode,:sIpAddr
        	 FROM 	dloginmsg
        	 WHERE 	login_no = :sLoginNo;

	if(SQLCODE != 0&&SQLCODE!=1403)
	{
		pubLog_DBErr(_FFL_,"","123402","markPublic:PublicMarkOpr查询dloginmsg出错:[%d][%s]",SQLCODE,SQLERRMSG);
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

    /*printf("lLoginAccept=%ld\n",lLoginAccept);
	printf("lContractNo=%ld\n",lContractNo);
	printf("sgoodsType=%s\n",sgoodsType);
	printf("lVolume=%ld\n",lVolume);
	printf("dConsumeMark=%.2f\n",dConsumeMark);

	printf("dFavourMark=%.2f\n",dFavourMark);
	printf("lIdNo=%ld\n",lIdNo);
	printf("sSmCode=%s\n",sSmCode);
	printf("sPhoneNo=%s\n",sPhoneNo);
	printf("sOrgCode=%s\n",sOrgCode);

	printf("sLoginNo=%s\n",sLoginNo);
	printf("iTotalDate=%d\n",iTotalDate);
	printf("sOpTime=%s\n",sOpTime);
	printf("sIpAddr=%s\n",sIpAddr);
	printf("opCode=%s\n",opCode);

	printf("systemNote=%s\n",systemNote);
	printf("opNote=%s\n",opNote);
	printf("orgId=%s\n",orgId);
	printf("backFlag=%s\n",backFlag);
	printf("opCode=%s\n",opCode);*/

    /***插入wMarkExchangeOpr表***/
   /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
     /*09积分系统升级需求  ADD group_id  edit by majha at 20091016*/
    Trim(sOrgId);
    sprintf(sSqlStr,"insert into wMarkExchangeOpr%d(LOGIN_ACCEPT,CONTRACT_NO,goods_type,idgoods_no,VOLUME,"
                      " CONSUME_MARK,FAVOUR_MARK,ID_NO,SM_CODE,PHONE_NO,ORG_CODE,LOGIN_NO,TOTAL_DATE,"
                      " OP_TIME,SYSTEM_NOTE,OP_NOTE,IP_ADDR,ORG_ID,BACK_FLAG,OP_CODE,GROUP_ID) "
                      " values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13, "
                     "  to_date(:v14,'YYYYMMDD HH24:MI:SS'),:v15,:v16,:v17,:v18,:v19,:v20,:v21) ", iBillYearMonth);
	pubLog_Debug(_FFL_,"","","sSqlStr=[%s]",sSqlStr);

    EXEC SQL EXECUTE
    BEGIN
        EXECUTE IMMEDIATE :sSqlStr using :lLoginAccept,:lContractNo,:sgoodsType,:sidgoodsNo,:lVolume,
                                           :dConsumeMark,:dFavourMark,:lIdNo,:sSmCode,:sPhoneNo,
                                           :sOrgCode,:sLoginNo,:iTotalDate,:sOpTime,:sSystemNote,
                                           :sOpNote,:sIpAddr,:sOrgId,:sBackFlag,:sOpCode,:vGroupId;

    END;
    END-EXEC;

    if (SQLCODE != 0)
    {
        pubLog_DBErr(_FFL_,"","123403","markPublic:PublicMarkOpr插入wMarkExchangeOprYYYYMM出错:[%d]",SQLCODE);
        return 123403;
    }

	return 0;
}



/**********插入wmarktransopr积分转赠表函数*********
* @inparam
	from_contractno		转出帐户
	to_contractno		转入帐户
	point		    	转出积分
	loginNo				操作工号
	opTime				操作时间
	backFlag			冲正标志
	login_accept		操作流水
	opNote				操作备注

	
*/
int InsertMarkTrans(long from_contractno,long to_contractno, double point,
                    const char *loginNo,const char *opTime,const char *backFlag, long login_accept, const char *opNote,const char *SmCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	long 	lloginAccept=0;
	long	lfromContractNo=0;
	long	ltoContractNo=0;
	double	fpoint=0;
	
	
	char	sLoginNo[6+1];
	char	sOpTime[20+1];
	char	sBackFlag[1+1];
	char	sOpNote[60+1];
	char	sSmCode[2+1];
	
	
	EXEC SQL END DECLARE SECTION;
	
	
	init(sLoginNo);
	init(sOpTime);
	
	init(sOpNote);
	
	init(sBackFlag);
	init(sSmCode);
	
	

	lloginAccept 		= login_accept;
	lfromContractNo 	= from_contractno;
	ltoContractNo 		= to_contractno;
	fpoint 	            = point;
	
	strcpy(sLoginNo,loginNo);
	strcpy(sOpTime,opTime);
	strcpy(sSmCode,SmCode);
	Trim(sLoginNo);
	Trim(sOpTime);
	Trim(sSmCode);
	
	strcpy(sBackFlag,backFlag);
	strcpy(sOpNote,opNote);
	Trim(sBackFlag);
	Trim(sOpNote);
	pubLog_Debug(_FFL_,"","","lfromContractNo=%ld##\n",lfromContractNo);
	pubLog_Debug(_FFL_,"","","ltoContractNo=%ld##\n",ltoContractNo);
	pubLog_Debug(_FFL_,"","","fpoint=%ld##\n",(long)fpoint);
	pubLog_Debug(_FFL_,"","","sLoginNo=%s##\n",sLoginNo);
	pubLog_Debug(_FFL_,"","","sOpTime=%s##\n",sOpTime);
	pubLog_Debug(_FFL_,"","","sBackFlag=%s##\n",sBackFlag);
	pubLog_Debug(_FFL_,"","","lloginAccept=%ld##\n",lloginAccept);
	pubLog_Debug(_FFL_,"","","sOpNote=%s##\n",sOpNote);
	pubLog_Debug(_FFL_,"","","sOpNote=%s##\n",sSmCode);
	Trim(sBackFlag);
	Trim(sOpNote);
    /***插入wMarkTransOpr表***/
    EXEC SQL insert into wmarktransopr  
                      ( FROM_CONTRACTNO,  TO_CONTRACTNO, POINTS,
                        LOGIN_NO,         OP_TIME,       BACK_FLAG, 
                        LOGIN_ACCEPT,     OP_NOTE,sm_code)                  
                values( :lfromContractNo, :ltoContractNo,round(:fpoint),
                	      :sLoginNo,to_date(:sOpTime,'YYYYMMDD HH24:MI:SS'),:sBackFlag,
                	      :lloginAccept,    :sOpNote,:sSmCode);                    
                 
    
	if (SQLCODE != 0)
    {
        pubLog_DBErr(_FFL_,"","121103","markPublic:PublicMarkOpr插入wmarktransopr出错:[%d][%s]",SQLCODE,SQLERRMSG);
        return 121103;
    }

	return 0;
}


/**
 *     积分扣减函数
 说明：积分冲销扣减，积分兑奖扣减
                         按照帐户进行扣减
 输入：
	sContractNo 		 兑奖帐号
 	fTotalCount 		 兑奖积分
 	sLoginAccept		 操作流水
 	sLoginNo			   操作工号
 	sOpCode				   操作代码
 	sOpTime				   操作时间
 	iSysYearMonth		 当前时间
 	sChangeType 		 变更类型

 返回
 	pfTotalCountRemain   剩余的兑换积分   >0 表示兑奖时出现透支 （但函数中未进行插入透支操作）
 	retCode
 	retMsg

 */
int MarkAwardReduce(const char *sContractNo, double fTotalCount, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *sChangeType, double *pfTotalCountRemain, char *retCode, char *retMsg)
{
	double  fCurrentPoint=0.00;
	double  fOweLimit=0.00;
	double  fOwePoint=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainMark=0.00;
	char 	sDetailCode[10+1];       /*number10*/
	long    change_seq=0;
	int    	iCurrentYearMonth=0;
	long    iMinYearMonth=0;
	double  fPreMark=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];
	
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

	/*根据帐户查找当前积分， 透支积分额度，已透支积分，已经消费积分，帐户状态,最早消费年月*/
	EXEC SQL	select current_point, owe_limit, owe_point, total_used, to_char(contract_status), min_ym
	              into :fCurrentPoint,:fOweLimit,:fOwePoint,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConMark
	             where contract_no = to_number(:sContractNo);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125067");
		sprintf(retMsg,"查询帐户积分信息错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fCurrentPoint=%lf\n",fCurrentPoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOweLimit=%lf\n",fOweLimit);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOwePoint=%lf\n",fOwePoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息iMinYearMonth=%d\n",iMinYearMonth);
#endif
		fPreTotalValue=fCurrentPoint;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"125068");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
#endif
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}

	if(fOwePoint>0.00)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","该账户已经发生透支，请确认");
#endif
	}

	if(fCurrentPoint+fOweLimit-fOwePoint < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","用户可用的积分不够该操作，要求积分=%f,透支积分=%f,用户积分=%f\n",fTotalCount,fOwePoint,fCurrentPoint);
		pubLog_Debug(_FFL_,"","","用户消耗后积分=%f,fOweLimit=%f,fOwePoint=%f\n",fCurrentPoint+fOweLimit-fOwePoint,fOweLimit,fOwePoint);
#endif
		sprintf(retMsg,"用户可用的积分不够该操作");
		strcpy(retCode,"125069");
		return -1;
	}


#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************积分兑换开始**********************");
#endif
	while((iCurrentYearMonth<=iSysYearMonth) && (fTotalCountRemain > 0))
	{
		init(sTableName);
		sprintf(sTableName,"dConMarkPre%6d\n",iCurrentYearMonth);
		sprintf(sSqlStr,
			"select     a.available_mark,to_char(b.detail_code) "
			" from      %s a,sMarkCodeDetail b "
			" where     a.contract_no=to_number(:v1) "
			" and       a.detail_code=b.detail_code "
			" order by  b.DEDUCT_ORDER ",
			sTableName);
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]",
			sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
#endif
		EXEC SQL PREPARE s3760_first01 from :sSqlStr;
		EXEC SQL DECLARE s3760_cur1 CURSOR for s3760_first01;
		EXEC SQL OPEN s3760_cur1 using :sContractNo;
		while(fTotalCountRemain > 0)/*游标选取正确*/
		{
			EXEC SQL fetch s3760_cur1 into :fRemainMark,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","取动态表格%s错误,由于当前表被取空，错误代码是%d\n",sTableName,SQLCODE);
				break;
			}
			
			Trim(sDetailCode);
			if(fRemainMark==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","从%s中取得的fRemainMark=%lf sDetailCode=%s\n",sTableName,fRemainMark,sDetailCode);
#endif
			fPreMark=fRemainMark;                /*记录此次游标位置的积分*/
			if(fRemainMark>=fTotalCountRemain)         /*游标积分大于当前交易需要的积分*/
			{
				fRemainMark=fRemainMark-fTotalCountRemain;
				fTotalCountRemain = 0;
			}
			else
			{
				fTotalCountRemain = fTotalCountRemain - fRemainMark;
				fRemainMark=0;
			}
			fChangeValue=fPreMark-fRemainMark;
			fAfterTotalValue=fPreTotalValue-fChangeValue;

			init(sTableName);
			sprintf(sTableName,"dConMarkPre%6d",iCurrentYearMonth);
			sprintf(sSqlStr, "update %s"
			                 "   set available_mark=:v1,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=to_number(:v2) "
			                 "   and detail_code=to_number(:v3)",sTableName);
			EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
			EXEC SQL EXECUTE s3760_first02 using  :fRemainMark,:sOpTime, :sContractNo,:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125070");
				pubLog_DBErr(_FFL_,"","125070",retMsg);
				sprintf(retMsg,"更新帐户积分科目明细表时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"","125070",retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标积分更新成功\n");
			pubLog_Debug(_FFL_,"","","当前游标位置剩余的积分数值为%lf\n",fRemainMark);
			pubLog_Debug(_FFL_,"","","该游标消费的积分数量为%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积分变更明细表中最大的积分变更序号\n",iSysYearMonth);
#endif

			init(sTableName);
			sprintf(sTableName,"dMarkChangeDetail%6d",iSysYearMonth);
			sprintf(sSqlStr,
				"select nvl(max(change_seq),0) "
				"  from %s"
				" where op_accept=to_number(:v1) "
				"   and   contract_no =to_number(:v2) "
				"   and   detail_code =to_number(:v3) "
				"   and   bill_ym     = :v4 ",
				sTableName);
			/*EXEC SQL PREPARE s3760_first03 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first03 into :change_seq
				using :sLoginAccept,:sContractNo,:sDetailCode,:iCurrentYearMonth;*/
			EXEC SQL PREPARE s3799_first05 from :sSqlStr;
			EXEC SQL DECLARE curMarkDeduct3 CURSOR for s3799_first05;
			EXEC SQL OPEN curMarkDeduct3 using :sLoginAccept, :sContractNo, :sDetailCode, :iCurrentYearMonth;
			EXEC SQL FETCH curMarkDeduct3 INTO :change_seq;

			if(SQLCODE==1403)
			{
				change_seq=1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","没有找到积分变更明细表变更序号,初始数值设置成[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","设置积分变更明细表变更序号为%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"125071");
				sprintf(retMsg,"查询最大积分变更序号错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				EXEC SQL CLOSE curMarkDeduct3;
				return -1;
			}
			EXEC SQL CLOSE curMarkDeduct3;
			pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积分变更明细表中dMarkChangeDetail\n");
			sprintf(sSqlStr,
				"insert into %s  "
				"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
				"ChangeType,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
				"After_Detail_Value,After_Total_Value,"
				"login_no        ,op_time,back_flag,op_code)"
				"values(to_number(:v1),to_number(:v2),to_number(:v3),:v4,:v5,"
				":sChangeType,:v6,:v7,-1*:v8,"
				":v9,:v10,"
				"nvl(:v11,'test'),sysdate,'0',:op_code)",
				sTableName);

			EXEC SQL PREPARE s3760_first04 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first04 using
				:sLoginAccept    ,:sContractNo,:sDetailCode,:iCurrentYearMonth,:change_seq,
				:sChangeType, :fPreMark        ,:fPreTotalValue         ,:fChangeValue,
				:fRemainMark     ,:fAfterTotalValue         ,:sLoginNo, :sOpCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125072");
				sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","向积分变更明细表中插入数据成功\n");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","更新帐户积分科目表\n");
#endif
			EXEC SQL	update dConMarkDetail
			               set available_point=available_point-:fChangeValue,
			                   min_ym=:iCurrentYearMonth,
			                   max_ym=:iSysYearMonth,
			                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
			             where contract_no=:sContractNo
			               and detail_code=:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125073");
				sprintf(retMsg,"出错[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE s3760_cur1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标大,交易结束更新帐户积分科目标成功\n");
			pubLog_Debug(_FFL_,"","","交易结束,直接跳转到关闭游标\n");
#endif
		}
		EXEC SQL CLOSE s3760_cur1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","单表循环结束，取下一张表格的年月为%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while操作正常结束１.没有透支积分2.发生透支积分\n");
	pubLog_Debug(_FFL_,"","","当前年月为%d\n",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","当前年月为%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","更新帐户表前的检查,total_point=%f\n",fTotalCount);
#endif

/*当当前积分不足以扣减时，把当前积分更新成0*/
	if(fTotalCountRemain > 0.005)
	{
		fTotalCount=fCurrentPoint;
	}
	/*ng解耦dx
	EXEC SQL	update dConMark
	               set current_point   =   round(current_point-:fTotalCount,2),
	                   min_ym          =   :iCurrentYearMonth,
	                   month_used      =   month_used+:fTotalCount,
	                   total_used      =   total_used+:fTotalCount,
	                   year_used       = year_used+:fTotalCount,
	                   current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	             where contract_no=to_number(:sContractNo);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125074");
		sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
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
	sprintf(set_strp,"current_point   =   round(current_point-to_number(:v0),2),\
	min_ym          =   to_number(:v1),\
	month_used      =   month_used+to_number(:v2),\
	total_used      =   total_used+to_number(:v3),\
	year_used       = year_used+to_number(:v4),\
	current_time    = to_date(:v5, 'YYYYMMDD HH24:MI:SS')");
    
	v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
						  	sOpCode,atol(sLoginAccept),sLoginNo,"MarkAwardReduce",
							sContractNo,set_strp,"",v_parameter_array);
	printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
	if(0!=v_ret)
	{
		strcpy(retCode,"125074");
		sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",v_ret);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
    /*ng解耦 by dx end*/	

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","正常结束后更新帐户积分数据成功\n");
#endif

	/*如果 fTotalCountRemain大于0，表示发生积分透支*/
	*pfTotalCountRemain = fTotalCountRemain;
	return 0;
}


/**
 *     积分兑奖函数
              说明：按照帐户进行兑奖，不允许透支操作
  输入：
	sContractNo 		 兑奖帐号
 	fTotalCount 		 兑奖积分
 	sLoginAccept		 操作流水
 	sLoginNo			 操作工号
 	sOpCode				 操作代码
 	sOpTime				 操作时间
 	iSysYearMonth		 当前时间


 返回
 	retCode              返回代码
 	retMsg               返回信息
 */
int MarkAward(const char *sContractNo, double fTotalCount, const char *sLoginAccept,
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

	if (MarkAwardReduce(sContractNo, fTotalCount, sLoginAccept,
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
		pubLog_Debug(_FFL_,"","","当前发生了积分透支,透支数量为%f\n",fTotalCountRemain);
		/*更新帐户积分表*/

		pubLog_Debug(_FFL_,"","","更新积分透支表\n");
		EXEC SQL	insert into  dCustOweMark
		            (
		                contract_no, owe_ym, owe_mark, payed_owe_mark, login_no, op_time,
		                owe_accept, op_code, back_flag
		            )
		            values
		            (
		                to_number(:sContractNo),  :iSysYearMonth ,round(:fTotalCountRemain,2),
		                0,:sLoginNo,to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
		                to_number(:sLoginAccept) ,:sOpCode,    '0'
		            );
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125075");
			sprintf(retMsg,"插入积分透支表失败[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","更新帐户透支表成功\n");
#endif
	}
	/*更新透支*/
	/*ng解耦dx
		EXEC SQL	update dConMark
		               set owe_point = owe_point + round(:fTotalCountRemain,2)
		             where contract_no=to_number(:sContractNo);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125057");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}*/
		/*ng解耦 by dx begin*/

    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCountRemain);
		strcpy(v_parameter_array[1],sContractNo);
		sprintf(set_strp,"owe_point = owe_point + round(to_number(:fTotalCountRemain),2)");
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
							  	sOpCode,atol(sLoginAccept),sLoginNo,"MarkAward",
								sContractNo,set_strp,"",v_parameter_array);
		printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125057");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",v_ret);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by dx end*/	
	return 0;
}



/**积分按帐户或帐户下积分科目扣减函数
 * @inparam iDeductType 调整类型： '4'积分调整、'5'转赠或'7'积分扣除

 	lContractNo			积分帐户
 	sDetailCodeIn		积分二级科目  按科目扣减时不能为NULL
 	sChangeType			变更类型
 	fTotalCount			扣减积分
 	lLoginAccept		操作流水
 	sLoginNo			  操作工号
 	sOpCode				  操作代码
 	sOpTime				  操作时间
 	iSysYearMonth		系统时间

 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

 */
int MarkDeduct(long lContractNo, const char* sDetailCodeIn, const char* sChangeType,
		double fTotalCount, long lLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	double  fCurrentPoint=0.00;
	double  fOweLimit=0.00;
	double  fOwePoint=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainMark=0.00;
	char 	sDetailCode[10+1];       /*number10*/
	long    change_seq=0;
	int    	iCurrentYearMonth=0;
	long    iMinYearMonth=0;
	double  fPreMark=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fChangeValue=0.00;
	double  fTotalCountRemain=fTotalCount;
	char 	sTableName[40+1];

	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
    char sContractNo[14+1];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	init(sContractNo);
	/*ng解耦 by yaoxc end*/

	init(sContractStatus);
	init(sDetailCode);
	init(sTableName);

	/*根据帐户查找当前积分， 透支积分额度，已透支积分，已经消费积分，帐户状态,最早消费年月*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_point, owe_limit, owe_point, total_used, to_char(contract_status), min_ym
	              into :fCurrentPoint,:fOweLimit,:fOwePoint,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConMark
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125058");
		sprintf(retMsg,"查询帐户积分信息错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fCurrentPoint=%lf\n",fCurrentPoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOweLimit=%lf\n",fOweLimit);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOwePoint=%lf\n",fOwePoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fTotalUsed=%lf\n",fTotalUsed);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息iMinYearMonth=%d\n",iMinYearMonth);
#endif
		fPreTotalValue=fCurrentPoint;
	}

	if (sDetailCodeIn != NULL)
	{
		EXEC SQL	select Available_Point, min_ym
		              into :fCurrentPoint,:iMinYearMonth
		              from dConMarkDetail
		             where contract_no = :lContractNo
		               and detail_code=to_number(:sDetailCodeIn);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125059");
			sprintf(retMsg,"查询帐户积分科目积分错误[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		if (fCurrentPoint < fTotalCount)
		{
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","帐户积分科目的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
	#endif
			strcpy(retCode,"1000");
			sprintf(retMsg,"帐户积分科目的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
			return 1000;
		}
	}
	else if (fCurrentPoint < fTotalCount)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户积分的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
#endif
		strcpy(retCode,"1000");
		sprintf(retMsg,"帐户积分的当前积分[%lf]不足于扣除积分[%lf]",fCurrentPoint,fTotalCount);
		return 1000;
	}

	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"125060");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
#endif
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}

	if(fOwePoint>0.00)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","该账户已经发生透支，请确认");
#endif
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************积分兑换开始**********************\n");
#endif
	while((iCurrentYearMonth<=iSysYearMonth) && (fTotalCountRemain > 0))
	{
		init(sTableName);
		sprintf(sTableName,"dConMarkPre%6d\n",iCurrentYearMonth);
		if (sDetailCodeIn == NULL)
		{
			sprintf(sSqlStr,
				"select     a.available_mark,to_char(b.detail_code) "
				" from      %s a,sMarkCodeDetail b "
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
				"select available_mark,detail_code "
				"  from %s  "
				" where contract_no=:v1 "
				"   and detail_code=:sDetailCodeIn " ,
				sTableName);
	#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","当前操作表%s,表时间%d,系统时间为[%d] sSqlStr[%s]\n",
				sTableName,iCurrentYearMonth,iSysYearMonth, sSqlStr);
	#endif
		}
		EXEC SQL PREPARE preMarkDeduct1 from :sSqlStr;
		EXEC SQL DECLARE curMarkDeduct1 CURSOR for preMarkDeduct1;
		if (sDetailCodeIn == NULL)
		{
			pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
			EXEC SQL OPEN curMarkDeduct1 using :lContractNo;
		}
		else
		{
			EXEC SQL OPEN curMarkDeduct1 using :lContractNo,:sDetailCodeIn;
		}
		while(fTotalCountRemain > 0)/*游标选取正确*/
		{
			EXEC SQL fetch curMarkDeduct1 into :fRemainMark,:sDetailCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_,"","","取动态表格%s错误,由于当前表被取空，错误代码是%d\n",sTableName,SQLCODE);
				break;
			}
			Trim(sDetailCode);
			if(fRemainMark==0)
			{
				continue;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","从%s中取得的fRemainMark=%lf sDetailCode=%s\n",sTableName,fRemainMark,sDetailCode);
#endif
			fPreMark=fRemainMark;                /*记录此次游标位置的积分*/
			if(fRemainMark>=fTotalCountRemain)         /*游标积分大于当前交易需要的积分*/
			{
				fRemainMark=fRemainMark-fTotalCountRemain;
				fTotalCountRemain = 0;
			}
			else
			{
				fTotalCountRemain = fTotalCountRemain - fRemainMark;
				fRemainMark=0;
			}
			fChangeValue=fPreMark-fRemainMark;
			fAfterTotalValue=fPreTotalValue-fChangeValue;

			init(sTableName);
			sprintf(sTableName,"dConMarkPre%6d",iCurrentYearMonth);
			sprintf(sSqlStr, "update %s"
			                 "   set available_mark=:v1,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=:v2"
			                 "   and detail_code=to_number(:v3)",sTableName);
			EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
			EXEC SQL EXECUTE s3760_first02 using  :fRemainMark,:sOpTime, :lContractNo,:sDetailCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125061");
				sprintf(retMsg,"更新帐户积分科目明细表时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标积分更新成功\n");
			pubLog_Debug(_FFL_,"","","当前游标位置剩余的积分数值为%lf\n",fRemainMark);
			pubLog_Debug(_FFL_,"","","该游标消费的积分数量为%lf\n",fChangeValue);
			pubLog_Debug(_FFL_,"","","查询当前年月%d帐户积分变更明细表中最大的积分变更序号\n",iSysYearMonth);
#endif

			init(sTableName);
			sprintf(sTableName,"dMarkChangeDetail%6d",iSysYearMonth);
			sprintf(sSqlStr,
				"select nvl(max(change_seq),0) "
				"  from %s"
				" where op_accept=:v1"
				"   and   contract_no =:v2 "
				"   and   detail_code =to_number(:v3) "
				"   and   bill_ym     = :v4 ",
				sTableName);
		/*	EXEC SQL PREPARE s3760_first03 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first03 into :change_seq
				using :lLoginAccept,:lContractNo,:sDetailCode,:iCurrentYearMonth;*/
			EXEC SQL PREPARE s3799_first04 from :sSqlStr;
			EXEC SQL DECLARE curMarkDeduct2 CURSOR for s3799_first04;
			EXEC SQL OPEN curMarkDeduct2 using :lLoginAccept, :lContractNo, :sDetailCode, :iCurrentYearMonth;
			EXEC SQL FETCH curMarkDeduct2 INTO :change_seq;
	
			if(SQLCODE==1403)
			{
				change_seq=1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","没有找到积分变更明细表变更序号,初始数值设置成[1]\n");
#endif
			}
			else if(SQLCODE==0)
			{
				change_seq=change_seq+1;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","设置积分变更明细表变更之后序号为%d\n",change_seq);
#endif
			}
			else
			{
				strcpy(retCode,"125062");
				sprintf(retMsg,"查询最大积分变更序号错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct2;
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			EXEC SQL CLOSE curMarkDeduct2;
			pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积分变更明细表中dMarkChangeDetail\n");
			sprintf(sSqlStr,
				"insert into %s  "
				"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
				"ChangeType,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
				"After_Detail_Value,After_Total_Value,"
				"login_no        ,op_time,back_flag,op_code)"
				"values(:v1,:v2,to_number(:v3),:v4,:v5,"
				":ChangeType,:v6,:v7,-1*:v8,"
				":v9,:v10,"
				"nvl(:v11,'test'),sysdate,'0',:op_code)",
				sTableName);

			EXEC SQL PREPARE s3760_first04 from :sSqlStr;
			EXEC SQL EXECUTE s3760_first04 using
				:lLoginAccept,    :lContractNo,:sDetailCode,:iCurrentYearMonth,:change_seq,
				:sChangeType,     :fPreMark        ,:fPreTotalValue         ,:fChangeValue,
				:fRemainMark,     :fAfterTotalValue         ,:sLoginNo, :sOpCode;
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125063");
				sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
			else
			{
				fPreTotalValue=fAfterTotalValue;
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","向积分变更明细表中插入数据成功");
#endif
			}

#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","更新帐户积分科目表");
#endif
			EXEC SQL	update dConMarkDetail
			               set available_point=available_point-:fChangeValue,
			                   min_ym=:iCurrentYearMonth,
			                   max_ym=:iSysYearMonth,
			                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
			             where contract_no=:lContractNo
			               and detail_code=to_number(:sDetailCode);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"125064");
				sprintf(retMsg,"更新帐户积分科目表失败[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkDeduct1;
				return -1;
			}
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","游标大,交易结束更新帐户积分科目标成功");
			pubLog_Debug(_FFL_,"","","交易结束,直接跳转到关闭游标");
#endif
		}
		EXEC SQL CLOSE curMarkDeduct1;
		if(fTotalCountRemain > 0)
		{
			IncYm(&iCurrentYearMonth);
			pubLog_Debug(_FFL_,"","","单表循环结束，取下一张表格的年月为%d\n",iCurrentYearMonth);
		}
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","while操作正常结束１.没有透支积分2.发生透支积分");
	pubLog_Debug(_FFL_,"","","当前年月为%d",iCurrentYearMonth);
#endif
	if (iCurrentYearMonth > iSysYearMonth)
	{
		iCurrentYearMonth = iSysYearMonth;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","当前年月为%d\n",iCurrentYearMonth);
	pubLog_Debug(_FFL_,"","","更新帐户表前的检查,total_point=%f\n",fTotalCount);
#endif
	if(sChangeType[0]=='9')/*如果“积分转赠”积分扣减时，不扣减月积分和年积分和累记积分*/
	{
		if(strcmp(sOpCode,"2418")==0)
		{	/*ng解耦
			EXEC SQL  update dConMark
	    	             set current_point   = current_point-:fTotalCount,
	    	             	 Month_point=Month_point-:fTotalCount,
							 year_point=year_point-:fTotalCount, 
							 add_point=add_point-:fTotalCount,*/
	    	                 /*min_ym          = :iCurrentYearMonth,*/
	    	                /* current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	    	           where contract_no=:lContractNo;
			if (SQLCODE != 0)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
				return -1;
			}
			ng解耦*/
    		/*ng解耦 by yaoxc begin*/
    		sprintf(sContractNo,"%ld",lContractNo);

    		init(v_parameter_array);
				
			sprintf(v_parameter_array[0],"%f",fTotalCount);
			sprintf(v_parameter_array[1],"%f",fTotalCount);
			sprintf(v_parameter_array[2],"%f",fTotalCount);
			sprintf(v_parameter_array[3],"%f",fTotalCount);
			strcpy(v_parameter_array[4],sOpTime);
			strcpy(v_parameter_array[5],sContractNo);
    		
			v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
								  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
									sContractNo,
									"current_point   = current_point-to_number(:v0),Month_point=Month_point-to_number(:v1),year_point=year_point-to_number(:v2),add_point=add_point-to_number(:v3),current_time = to_date(:v4, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
			printf("OrderUpdateConMark1 ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				strcpy(retCode, "PM1108");
				sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
				return -1;
			}
    		/*ng解耦 by yaoxc end*/	
	 	}
	 	else
	 	{
			if(strcmp(sOpCode,"2419")==0)
			{	/*ng解耦
				EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
								 add_point=add_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
					return -1;
				}
				ng解耦*/
    			/*ng解耦 by yaoxc begin*/
    			sprintf(sContractNo,"%ld",lContractNo);
    			init(v_parameter_array);
					
				sprintf(v_parameter_array[0],"%f",fTotalCount);
				sprintf(v_parameter_array[1],"%f",fTotalCount);
				sprintf(v_parameter_array[2],"%d",iCurrentYearMonth);
				strcpy(v_parameter_array[3],sOpTime);
				strcpy(v_parameter_array[4],sContractNo);
    			
				v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
									  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
										sContractNo,
										" current_point = current_point-to_number(:v0),add_point=add_point-to_number(:v1),min_ym = to_number(:v2),current_time = to_date(:v3, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
				printf("OrderUpdateConMark2 ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng解耦 by yaoxc end*/
		 	}	 
		 	else{
		 		/*ng解耦		
		 		EXEC SQL  update dConMark
		    	             set current_point   = current_point-:fTotalCount,
		    	                 min_ym          = :iCurrentYearMonth,
		    	                 current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		    	           where contract_no=:lContractNo;
				if (SQLCODE != 0)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
					return -1;
				}
				ng解耦*/
    			/*ng解耦 by yaoxc begin*/
    			sprintf(sContractNo,"%ld",lContractNo);
    			init(v_parameter_array);
					
				sprintf(v_parameter_array[0],"%f",fTotalCount);
				sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
				strcpy(v_parameter_array[2],sOpTime);
				strcpy(v_parameter_array[3],sContractNo);
    			
				v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
									  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
										sContractNo,
										" current_point = current_point-to_number(:v0),min_ym = to_number(:v1),current_time = to_date(:v2, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
				printf("OrderUpdateConMark3 ,ret=[%d]\n",v_ret);
				if(0!=v_ret)
				{
					strcpy(retCode, "PM1108");
					sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
					return -1;
				}
    			/*ng解耦 by yaoxc end*/
			}
	 	}
	}
	else if(sChangeType[0]=='8')/*单独的做积分调整时，扣减积分变更时扣减月积分和年积分和累记积分*/
	{	/*ng解耦
		EXEC SQL  update dConMark
	                 set current_point   = current_point-:fTotalCount,
	                     min_ym          = :iCurrentYearMonth,
	                     Month_point     = Month_point-:fTotalCount,
	                     year_point      = year_point-:fTotalCount,
	                     add_point       = add_point-:fTotalCount,
	                     current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	               where contract_no=:lContractNo;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	sprintf(sContractNo,"%ld",lContractNo);
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
		sprintf(v_parameter_array[2],"%f",fTotalCount);
		sprintf(v_parameter_array[3],"%f",fTotalCount);
		sprintf(v_parameter_array[4],"%f",fTotalCount);
		strcpy(v_parameter_array[5],sOpTime);
		strcpy(v_parameter_array[6],sContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
								sContractNo,
								" current_point = current_point-to_number(:v0),min_ym = to_number(:v1),Month_point = Month_point-to_number(:v2),year_point = year_point-to_number(:v3),add_point = add_point-to_number(:v4),current_time = to_date(:v5, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark4 ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/

	}
	else/*其他服务扣积分时，不扣减月积分和年积分和累记积分。记录使用积分*/
	{	/*ng解耦
		EXEC SQL	update dConMark
	               set current_point   =   round(current_point-:fTotalCount,2),
	                   min_ym          =   :iCurrentYearMonth,*/
	                  /* month_used      =   month_used+:fTotalCount,
	                   total_used      =   total_used+:fTotalCount,
	                   year_used       = year_used+:fTotalCount,*/
	                  /* current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	             where contract_no=:lContractNo;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	sprintf(sContractNo,"%ld",lContractNo);
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
		strcpy(v_parameter_array[2],sOpTime);
		strcpy(v_parameter_array[3],sContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sContractNo,100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeduct",
								sContractNo,
								" current_point = round(current_point-to_number(:fTotalCount),2),min_ym = to_number(:iCurrentYearMonth),current_time = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark5 ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/
	}
	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","正常结束后更新帐户积分数据成功fTotalCountRemain=%f",fTotalCountRemain);
#endif

	if(fTotalCountRemain > 0.005)/*发生积分透支*/
	{
		strcpy(retCode,"125066");
		sprintf(retMsg,"发生积分透支错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	return 0;
}


/**积分按帐户年月和积分科目下扣减函数
 * @inparam iDeductType 调整类型： '4'积分调整、'5'转赠或'7'积分扣除

 	lContractNo			积分帐户
 	sDetailCodeIn		积分二级科目  不能为空
 	sBillYearMon        积分帐户年月
 	sChangeType			变更类型
 	fTotalCount			扣减积分
 	lLoginAccept		操作流水
 	sLoginNo			  操作工号
 	sOpCode				  操作代码
 	sOpTime				  操作时间
 	iSysYearMonth		系统时间

 * @outparam
 	retCode				返回代码
 	retMsg				返回信息

 */
int MarkDeductBillYM(long lContractNo, const char* sDetailCodeIn, char *sBillYearMon, const char* sChangeType,
		double fTotalCount, long lLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	double  fCurrentPoint=0.00;
	double  fOweLimit=0.00;
	double  fOwePoint=0.00;
	double  fTotalUsed=0.00;
	char 	sContractStatus[2+1];
	char  	sSqlStr[1000+1];
	double 	fRemainMark=0.00;
	long    change_seq=0;
	int    	iCurrentYearMonth=0;
	long    iMinYearMonth=0;
	double  fPreMark=0.00;                 /*每次打开游标时候的数值*/
	double  fPreTotalValue=0.00;
	double  fAfterTotalValue=0.00;
	double	TocurrentPoint=0.00;
	char 	sTableName[40+1];
	
	/*ng解耦 by dx begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	/*ng解耦 by dx end*/
	
	/*ng解耦 by dx begin*/
	init(v_parameter_array);
	/*ng解耦 by dx end*/

	init(sContractStatus);
	init(sTableName);

	/*根据帐户查找当前积分， 透支积分额度，已透支积分，已经消费积分，帐户状态,最早消费年月*/
	pubLog_Debug(_FFL_,"","","lContractNo=[%ld]\n",lContractNo);
	EXEC SQL	select current_point, owe_limit, owe_point, total_used, to_char(contract_status), min_ym
	              into :fCurrentPoint,:fOweLimit,:fOwePoint,:fTotalUsed,:sContractStatus,:iMinYearMonth
	              from dConMark
	             where contract_no = :lContractNo;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125058");
		sprintf(retMsg,"查询帐户积分信息错误[%d][%s]",SQLCODE,SQLERRMSG);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fCurrentPoint=%lf",fCurrentPoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOweLimit=%lf",fOweLimit);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息fOwePoint=%lf",fOwePoint);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息iMinYearMonth=%d",iMinYearMonth);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息sBillYearMon=%s",sBillYearMon);
		pubLog_Debug(_FFL_,"","","查询到的用户当前信息sDetailCodeIn=%s",sDetailCodeIn);
#endif
		fPreTotalValue=fCurrentPoint;
	}
	init(sSqlStr);
	sprintf(sSqlStr,"select nvl(available_mark ,0) "
	                "  from dconmarkpre%s    "
	                " where contract_no=:lContractNo "
	                "   and detail_code=to_number(:sDetailCodeIn)",sBillYearMon);
	EXEC SQL PREPARE preMarkDeduct from :sSqlStr;
	EXEC SQL DECLARE preDeductCursor CURSOR FOR preMarkDeduct;
	EXEC SQL OPEN    preDeductCursor using :lContractNo,:sDetailCodeIn;
	EXEC SQL FETCH preDeductCursor into :TocurrentPoint; 
	EXEC SQL CLOSE preDeductCursor;
	if(SQLCODE != 0)
	{
	    strcpy(retCode,"111103");
	    sprintf(retMsg,"查询回退帐户当前积分错误[%d]!", SQLCODE);
	    return -1;
	}
	pubLog_Debug(_FFL_,"","","查询到的用户当前信息TocurrentPoint=%f",TocurrentPoint);
	pubLog_Debug(_FFL_,"","","查询到的用户当前信息fTotalCount=%f",fTotalCount);
	if(TocurrentPoint<fTotalCount)
	{
		strcpy(retCode,"111104");
	    sprintf(retMsg,"该年月下科目下积分小于扣减积分[%d]!", SQLCODE);
	    return -1;
	}
	iCurrentYearMonth=iMinYearMonth;

	if(sContractStatus[0] == '1')
	{
		strcpy(retCode,"125060");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","帐户冻结,不能执行操作");
#endif
		sprintf(retMsg,"帐户冻结,不能执行操作");
		return -1;
	}

	if(fOwePoint>0.00)
	{
#ifdef _DEBUG_
		printf("该账户已经发生透支，请确认");
#endif
	}

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","*****************积分扣减开始**********************");
#endif
	
	init(sTableName);
	sprintf(sTableName,"dConMarkPre%s\n",sBillYearMon);
	sprintf(sSqlStr, "update %s"
			                 "   set available_mark=available_mark-:fTotalCount,"
			                 "       op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')"
			                 " where contract_no=:v2"
			                 "   and detail_code=to_number(:v3)",sTableName);
	EXEC SQL PREPARE s3760_first02 from   :sSqlStr;
	EXEC SQL EXECUTE s3760_first02 using  :fTotalCount,:sOpTime, :lContractNo,:sDetailCodeIn;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125061");
		sprintf(retMsg,"更新帐户积分科目明细表时发生错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}

	init(sTableName);
	sprintf(sTableName,"dMarkChangeDetail%6d",iSysYearMonth);
	sprintf(sSqlStr,
		"select nvl(max(change_seq),0) "
		"  from %s"
		" where op_accept=:v1"
		"   and   contract_no =:v2 "
		"   and   detail_code =to_number(:v3) "
		"   and   bill_ym     = to_number(:v4) ",
		sTableName);
	EXEC SQL PREPARE Deductfirst04 from :sSqlStr;
	EXEC SQL DECLARE curDeduct CURSOR for Deductfirst04;
	EXEC SQL OPEN curDeduct using :lLoginAccept, :lContractNo, :sDetailCodeIn, :sBillYearMon;
	EXEC SQL FETCH curDeduct INTO :change_seq;
	
	if(SQLCODE==1403)
	{
		change_seq=1;
	}
	else if(SQLCODE==0)
	{
		change_seq=change_seq+1;
	}
	else
	{
		strcpy(retCode,"125062");
		sprintf(retMsg,"查询最大积分变更序号错误[%d]\n",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		EXEC SQL CLOSE curDeduct;
		return -1;
	}
	EXEC SQL CLOSE curDeduct;
	fAfterTotalValue=fPreTotalValue-fTotalCount;
	fRemainMark=TocurrentPoint-fTotalCount;
	pubLog_Debug(_FFL_,"","","将数据信息插入到帐户积分变更明细表中dMarkChangeDetail");
	sprintf(sSqlStr,
		"insert into %s  "
		"(op_accept,contract_no,detail_code,bill_ym,Change_Seq,"
		"ChangeType,Pre_Detail_Value,Pre_Total_Value,Change_Value,"
		"After_Detail_Value,After_Total_Value,"
		"login_no        ,op_time,back_flag,op_code)"
		"values(:v1,:v2,to_number(:v3),to_number(:v4),:v5,"
		":ChangeType,:v6,:v7,-1*:v8,"
		":v9,:v10,"
		"nvl(:v11,'test'),sysdate,'0',:op_code)",
		sTableName);

	EXEC SQL PREPARE s3760_first04 from :sSqlStr;
	EXEC SQL EXECUTE s3760_first04 using
		:lLoginAccept,    :lContractNo,:sDetailCodeIn,:sBillYearMon,:change_seq,
		:sChangeType,     :TocurrentPoint        ,:fPreTotalValue         ,:fTotalCount,
		:fRemainMark,     :fAfterTotalValue         ,:sLoginNo, :sOpCode;
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125063");
		sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	
	EXEC SQL	update dConMarkDetail
	               set available_point=available_point-:fTotalCount,
	                   min_ym=:iCurrentYearMonth,
	                   max_ym=:iSysYearMonth,
	                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	             where contract_no=:lContractNo
	               and detail_code=to_number(:sDetailCodeIn);
	if(SQLCODE!=0)
	{
		strcpy(retCode,"125064");
		sprintf(retMsg,"更新帐户积分科目表失败[%d]\n",SQLCODE);
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	if(sChangeType[0]=='9')
	{
		/*ng解耦dx
		EXEC SQL update dConMark
					set current_point=current_point-:fTotalCount,
						current_time=to_date(:sOpTime,'YYYYMMDD HH24:MI:SS')
				  where contract_no=:lContractNo;
		if (SQLCODE != 0)
		{
			strcpy(retCode, "PM1108");
			sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld, [SQLCODE:%d]", lContractNo, SQLCODE);
			return -1;
		}*/
		init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%s",sOpTime);
		sprintf(v_parameter_array[2],"%ld",lContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,ltoa(lContractNo),100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeductBillYM",
								ltoa(lContractNo),
								" current_point=current_point-to_number(:fTotalCount), current_time=to_date(:sOpTime,'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode, "PM1108");
			sprintf(retMsg, "更新帐户积分表(dConMark)失败！账号:%ld", lContractNo);
			return -1;
		}
	}
	else 
	{
		/*ng解耦dx
		EXEC SQL  update dConMark
	                 set current_point   = current_point-:fTotalCount,
	                     min_ym          = :iCurrentYearMonth,
	                     Month_point     = Month_point-:fTotalCount,
	                     year_point      = year_point-:fTotalCount,
	                     add_point       = add_point-:fTotalCount,
	                     current_time    = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
	               where contract_no=:lContractNo;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}*/
		init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fTotalCount);
		sprintf(v_parameter_array[1],"%d",iCurrentYearMonth);
		sprintf(v_parameter_array[2],"%f",fTotalCount);
		sprintf(v_parameter_array[3],"%f",fTotalCount);
		sprintf(v_parameter_array[4],"%f",fTotalCount);
		sprintf(v_parameter_array[5],"%s",sOpTime);
		sprintf(v_parameter_array[6],"%ld",lContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,ltoa(lContractNo),100,
							  	sOpCode,lLoginAccept,sLoginNo,"MarkDeductBillYM",
								ltoa(lContractNo),
								" current_point = current_point-to_number(:v0),  min_ym = to_number(:v1), Month_point = Month_point-to_number(:v2), year_point= year_point-to_number(:v3),add_point= add_point-to_number(:v4),current_time = to_date(:v5, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"125065");
			sprintf(retMsg,"正常结束后更新帐户积分表数据出错[%d]",v_ret);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

	}
	
	
	return 0;
}


/**
 *     积分冲正函数，按照帐户进行对奖
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
int MarkAwardBack(const char *sBackLoginAccept, int iBackYearMonth, const char *sLoginAccept,
		const char *sLoginNo, const char *sOpCode,	const char *sOpTime, int iSysYearMonth,
		char *retCode, char *retMsg)
{
	char    sSqlStr[2000+1];
	char 	table_name[40+1];

	double 	fAccCount=0.00;
	double  fChangeValue=0.00;
	double  available_mark=0.00;
	double  fOweMark=0.00;
	double  fPayedOweMark=0.00;
	double  fCurrentPoint=0.00;
	double  fPreDetailValue=0.00;
	double  fPreTotalValue=0.00;
	double  fAfterDetailValue=0.00;
	double  fAfterTotalValue=0.00;
	double  fCurrentTotalValue=0.00;
	double 	temp1=0.00;
	double 	temp2=0.00;
	int    	iBillYm=0;
	int    	iOweYm=0;
	int    	iMinYm=0;
	char    sContractNo[14+1];
	char    sDetailCode[10+1];
	int    	iChangeSeq=0;
	long    iCurrMinYm=0;
	int    	iDetailMinYm=0;
	char    sBakeContractNo[14+1];
	int 	iFindFlag=0;        /*表示是否找到积分兑换记录*/
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	/*ng解耦 by yaoxc end*/

	init(sContractNo);
	init(sDetailCode);
	init(sBakeContractNo);

	/*******进行透支的冲正,可能没有变更明细只有透支********/
	/***********一个流水只有一个透支记录，所以不用判断输入年月*************/
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","***********检查透支有没有被冲销，如果被冲销过则不能做冲正！************");
#endif

	/*根据输入流水号查找积分透支表*/
	EXEC SQL	select owe_mark   ,owe_ym  ,to_char(contract_no),  payed_owe_mark
	              into :fOweMark, :iOweYm,:sContractNo, :fPayedOweMark
	              from dCustOweMark
	             where owe_accept=to_number(:sBackLoginAccept);

	if(SQLCODE==1403)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","需要到积分透支历史表中查询[%d]",SQLCODE);
#endif

		sprintf(sSqlStr, "select owe_mark   ,  owe_ym  , to_char(contract_no),  payed_owe_mark"
		                 "  FROM dCustOweMarkHis%06d"
		                 " WHERE owe_accept=to_number(:v1) and back_flag='0' and rownum<2", iBackYearMonth);
		EXEC SQL	PREPARE preMarkAwardBack1 from :sSqlStr;
		EXEC SQL	DECLARE preMarkAwardBackCur cursor for preMarkAwardBack1;
		EXEC SQL	OPEN	preMarkAwardBackCur USING :sBackLoginAccept;
			
		printf("11111111[%d][%s]\n",SQLCODE, sSqlStr);
		EXEC SQL    fetch 	preMarkAwardBackCur INTO :fOweMark, :iOweYm,:sContractNo, :fPayedOweMark;
		
		if (SQLCODE == 1403)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","积分透支历史表中无透支信息[%d]", SQLCODE);
#endif
		}
		else if(SQLCODE == 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","积分透支历史表中有透支信息[%d]!",SQLCODE);
#endif
		}
		else
		{
			strcpy(retCode,"376926");
			sprintf(retMsg,"查询积分透支历史表时发生错误[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE preMarkAwardBackCur;
			return -1;
		}
		EXEC SQL CLOSE preMarkAwardBackCur;
	}
	else if(SQLCODE!=0)
	{
		strcpy(retCode,"376919");
		strcpy(retMsg,"查询积分透支表时发生错误");
		pubLog_DBErr(_FFL_,"",retCode,retMsg);
		return -1;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","在积分透支表中找到记录\n");
#endif
	}

    Trim(sContractNo);

	if (fPayedOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","用户透支的积分已经被做过冲销，不容许再冲正!\n");
#endif
		strcpy(retCode,"376919");
		strcpy(retMsg,"用户透支的积分已经被做过冲销，不容许再冲正!");
		return -1;
	}
	else if (fOweMark > 0.005)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","修改积分透支表!\n");
#endif
		/*冲销完毕，将记录移到积分透支历史表*/
		init(tmp_buf);
		strcpy(tmp_buf, "insert into dCustOweMarkHis");
		sprintf(tmp_buf,"%s%d",tmp_buf,iOweYm);
		strcat(tmp_buf, "(Owe_Accept, contract_no, Owe_ym, owe_mark, payed_owe_mark,");
		strcat(tmp_buf, "Finish_Flag, login_no, op_time, op_code, back_flag) ");
		strcat(tmp_buf, "select Owe_Accept, contract_no, Owe_ym, owe_mark, owe_mark, ");
		strcat(tmp_buf, "'0', login_no, op_time, op_code, '1' ");
		strcat(tmp_buf, "from dCustOweMark where Owe_Accept=to_number(:iOweAccept)");

		EXEC SQL PREPARE pre_dCustOweMarkHisYM from :tmp_buf;
		EXEC SQL EXECUTE pre_dCustOweMarkHisYM using :sBackLoginAccept;
		if (SQLCODE != 0)
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","tmp_buf:%s\n sBackLoginAccept:%s\n", tmp_buf, sBackLoginAccept);
#endif
			sprintf(retCode, "000011");
			sprintf(retMsg, "冲销完毕，将记录移到积分透支历史表失败！[SQLCODE:%d]", SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}

		EXEC SQL delete from dCustOweMark where Owe_Accept=to_number(:sBackLoginAccept);
		if (SQLCODE != 0)
		{
			strcpy(retCode,"3769395");
			sprintf(retMsg,"删除积分透支表记录出错[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
	}

	init(sSqlStr);
	sprintf(sSqlStr, "select to_char(contract_no), to_char(detail_code), bill_ym,"
					 "       change_seq, pre_detail_value, pre_total_value,"
					 "       abs(change_value), after_detail_value, after_total_value"
					 "  from dMarkChangeDetail%d"
					 " where op_accept = to_number(:sBackLoginAccept)"
					 "   and changetype = '1'"
					 "   and back_flag = '0'"
					 " order by bill_ym desc",
					 iBackYearMonth);

	/*积分变换类型为1代表此类型是积分兑换*/
	EXEC SQL PREPARE preMarkAwardBack2 from :sSqlStr;
	EXEC SQL DECLARE curMarkAwardBack2 CURSOR FOR preMarkAwardBack2;
	EXEC SQL OPEN    curMarkAwardBack2 using :sBackLoginAccept;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","OPEN SQLCODE :%d\n", SQLCODE);
#endif
	for(;;)
	{
		EXEC SQL FETCH   curMarkAwardBack2 INTO :sContractNo, :sDetailCode, :iBillYm,
												:iChangeSeq, :fPreDetailValue, :fPreTotalValue,
												:fChangeValue, :fAfterDetailValue, :fAfterTotalValue;
		if (SQLCODE == 1403)
		{
			break;
		}
		else if (SQLCODE != 0)
		{
			strcpy(retCode,"376935");
			sprintf(retMsg,"取帐户积分信息出错[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		iFindFlag = 1;

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","获得的该细则产生的积分变化为%lf",fChangeValue);
		pubLog_Debug(_FFL_,"","","当前帐户为%s,备份帐户为[%s]",sContractNo,sBakeContractNo);
#endif
		if (strcmp(sBakeContractNo, sContractNo) != 0)
		{
			if(sBakeContractNo[0] != '\0')
			{
				pubLog_Debug(_FFL_,"","","多帐户兑奖冲正出错\n");
				strcpy(retCode,"376915");
				sprintf(retMsg,"多帐户兑奖冲正出错");
				EXEC SQL CLOSE curMarkAwardBack2;
				return -1;
			}

			EXEC SQL	select current_point,min_ym
			              into :fCurrentPoint,:iMinYm
			              from dconmark
			             where contract_no=to_number(:sContractNo);
			if(SQLCODE!=0)
			{
				strcpy(retCode,"376916");
				sprintf(retMsg,"从帐户积分表中查询帐户总积分出错[%d]",SQLCODE);
				pubLog_DBErr(_FFL_,"",retCode,retMsg);
				EXEC SQL CLOSE curMarkAwardBack2;
				return -1;
			}
			else
			{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_,"","","查询到新帐户总积分为%lf,最早积分变更日期%d",fCurrentPoint,iMinYm);
#endif
			}

			iCurrMinYm=iMinYm;
			init(sBakeContractNo);
			strcpy(sBakeContractNo,sContractNo);
			fAccCount=0;               /*临时帐户清空*/
			fCurrentTotalValue=fCurrentPoint;
		}
		else
		{
			/*表示contract_no和sBakeContractNo相同，已经处理过了。*/
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
		/***lula增加*******/
		iMinYm=iCurrMinYm;
		/******lula增加****/
		init(table_name);
		sprintf(table_name,"dMarkChangeDetail%6d",iSysYearMonth);
		pubLog_Debug(_FFL_,"","","数据为[%s],[%s],[%s],[%d]\n",sLoginAccept,sContractNo,sDetailCode,iBillYm);
		sprintf(sSqlStr,
			" select nvl(max(change_seq),0) "
			" from %s  where op_accept=to_number(:v1) "
			" and   contract_no =to_number(:v2) "
			" and   Detail_Code =to_number(:v3) "
			" and   bill_ym     = :v4 ",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack3 from :sSqlStr;
		EXEC SQL DECLARE curMarkAwardBack3 CURSOR for preMarkAwardBack3;
		EXEC SQL OPEN    curMarkAwardBack3 using :sLoginAccept,:sContractNo,:sDetailCode,:iBillYm;;
		EXEC SQL fetch curMarkAwardBack3 into :iChangeSeq;

		if(SQLCODE==1403)
		{
			iChangeSeq=1;
			pubLog_Debug(_FFL_,"","","没有找到该条件下变更序号,初始数值设置成[1]");
			EXEC SQL CLOSE curMarkAwardBack3;
		}
		else if(SQLCODE==0)
		{
			iChangeSeq=iChangeSeq+1;
			EXEC SQL CLOSE curMarkAwardBack3;
			pubLog_Debug(_FFL_,"","","当前设置的变更序号为%d",iChangeSeq);
		}
		else
		{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","","查找变更序号时，发生错误[%d]",SQLCODE);
#endif
			sprintf(retMsg,"查询最大积分变更序号[%d]",SQLCODE);
			EXEC SQL CLOSE curMarkAwardBack3;
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","向当前系统时间下的积分变更明细表中插入当前操作的结果");
#endif
		/*******lula修改********/
		temp1=fAfterDetailValue+fChangeValue;
		temp2=fCurrentTotalValue+fChangeValue;
		/*******lula修改********/
		/*fCurrentTotalValue=temp2;*/
		init(table_name);
		sprintf(table_name,"dMarkChangeDetail%6d",iSysYearMonth);
		sprintf(sSqlStr,
			"insert into %s  "
			"(op_accept,contract_no,Detail_Code,Bill_Ym,change_seq,"
			"ChangeType,pre_detail_value,pre_total_value,change_value,"
			"after_detail_value,after_total_value,"
			"login_no,op_time,back_flag,op_code)"
			"values(to_number(:v1),to_number(:v2),to_number(:v3),:v4,:v5,"
			"'1',:v6,:v7,:v8,"
			":v9,:v10,"
			":v11,to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),'1',:sOpCode)",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack4 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack4 using
			:sLoginAccept,:sContractNo,:sDetailCode,:iBillYm,:iChangeSeq,
			:fAfterDetailValue,:fCurrentTotalValue,:fChangeValue,
			:temp1,:temp2,:sLoginNo, :sOpTime, :sOpCode;
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"向积分变更明细表中插入操作时发生错误[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
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
		sprintf(table_name,"dConMarkPre%6d",iBillYm);
		sprintf(sSqlStr,
			" update %s "
			" set available_mark=available_mark+:v1, "
			" op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS') "
			" where  contract_no=to_number(:v2)   "
			" and    detail_code=to_number(:v3) ",
			table_name);
		EXEC SQL PREPARE preMarkAwardBack5 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack5 using :fChangeValue,:sOpTime,:sContractNo,:sDetailCode;
		if(SQLCODE==0)
		{
			pubLog_Debug(_FFL_,"","","成功对帐户科目明细表更新数值\n");
		}
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"更新帐户科目明细表可用积分失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		pubLog_Debug(_FFL_,"","","选择积分科目表下该二级科目下的最小年月\n");
		EXEC SQL	select min_ym
		              into :iDetailMinYm
		              from dConMarkDetail
		             where contract_no = to_number(:sContractNo)
		               and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"选择积分科目表下该二级科目下的最小年月失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}

		if( iDetailMinYm>iBillYm )
		{
			iDetailMinYm = iBillYm;
		}

		pubLog_Debug(_FFL_,"","","更新积分科目表\n");
		EXEC SQL	update dConMarkDetail
		               set min_ym=:iDetailMinYm,
		                   max_ym=:iSysYearMonth,
		                   op_time=to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS'),
		                   available_point=available_point+:fChangeValue
		             where contract_no = to_number(:sContractNo)
			           and detail_code = to_number(:sDetailCode);
		if(SQLCODE!=0)
		{
			strcpy(retCode,"376916");
			sprintf(retMsg,"更新积分科目表失败[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			EXEC SQL CLOSE curMarkAwardBack2;
			return -1;
		}
	}
	EXEC SQL CLOSE curMarkAwardBack2;
	/*游标关闭，将最后的帐户下的数据写到帐户积分表中*/

	if (sBakeContractNo[0] == '\0')
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","在帐户积分变更明细表中没有找到当前流水\n");
#endif
		/*
		strcpy(retCode,"376918");
		strcpy(retMsg,"帐户积分变更明细表中没有找到当前流水");
		return -1;
		*/
	}
	else
	{
/*****
fAccCount是从dMarkChangeDetail表取的，不包含透支积分，fOweMark为透支积分
*****/	/*ng解耦
		EXEC SQL	update dConMark
		               set current_point = current_point+:fAccCount,
		                   min_ym        = :iCurrMinYm,
		                   month_used    = month_used-:fAccCount,
		                   year_used     = year_used-:fAccCount,
		                   total_used    = total_used-:fAccCount,
		                   owe_point     = owe_point - :fOweMark,
		                   current_time  = to_date(:sOpTime, 'YYYYMMDD HH24:MI:SS')
		             where contract_no   = :sBakeContractNo;

		if(SQLCODE!=0)
		{
			strcpy(retCode,"376917");
			sprintf(retMsg,"更新帐户积分表出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	init(v_parameter_array);
			
		sprintf(v_parameter_array[0],"%f",fAccCount);
		sprintf(v_parameter_array[1],"%ld",iCurrMinYm);
		sprintf(v_parameter_array[2],"%f",fAccCount);
		sprintf(v_parameter_array[3],"%f",fAccCount);
		sprintf(v_parameter_array[4],"%f",fAccCount);
		sprintf(v_parameter_array[5],"%f",fOweMark);
		strcpy(v_parameter_array[6],sOpTime);
		strcpy(v_parameter_array[7],sBakeContractNo);
    	
		v_ret=OrderUpdateConMark(ORDERIDTYPE_CNTR,sBakeContractNo,100,
							  	sOpCode,atol(sLoginAccept),sLoginNo,"MarkAwardBack",
								sBakeContractNo,
								" current_point = current_point+to_number(:v0),min_ym = to_number(:v1),month_used = month_used-to_number(:v2),year_used = year_used-to_number(:v3),total_used = total_used-to_number(:v4),owe_point = owe_point - to_number(:v5),current_time = to_date(:v6, 'YYYYMMDD HH24:MI:SS')","",v_parameter_array);
		printf("OrderUpdateConMark ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			strcpy(retCode,"376917");
			sprintf(retMsg,"更新帐户积分表出错[%d]",SQLCODE);
			pubLog_DBErr(_FFL_,"",retCode,retMsg);
			return -1;
		}
    	/*ng解耦 by yaoxc end*/
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","将累积计算的积分%lf加到该帐户%s的积分表中，成功\n",fAccCount,sBakeContractNo);
#endif

		sprintf(sSqlStr,"update dMarkChangeDetail%6d "
						"   set back_flag='1'"
						" where op_accept=to_number(:op_accept)   ",
						iBackYearMonth);
		EXEC SQL PREPARE preMarkAwardBack6 from :sSqlStr;
		EXEC SQL EXECUTE preMarkAwardBack6 using :sBackLoginAccept;
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


/*功能：根据用户id查询账号
 *输入: 用户id，
 *输出: 账号，
 *如果错误返回-1,retCode返回错误码，retMsg返回错误信息
 */
int markGetConByIdno(long idNo, long *conTractNo, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long icontractNo;
	EXEC SQL END DECLARE SECTION;

	if (retCode == NULL || retMsg == NULL)
	{
		pubLog_Debug(_FFL_,"","","输入参数错！指针retCode或retMsg指向空");
		return -1;
	}

	EXEC SQL select contract_no
			   into :icontractNo
			   from dcustmsg
			  where id_no = :idNo;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "GC0001");
		sprintf(retMsg, "通过用户ID(idNo:%ld)查询账户错误![SQLCODE:%d]", idNo, SQLCODE);
		return -1;
	}

	*conTractNo = icontractNo;
	return 0;
}


/*功能：根据用户电话号码查询账号
 *输入: 用户id，
 *输出: 账号，
 *如果错误返回-1,retCode返回错误码，retMsg返回错误信息
 */
int markGetConByPhone(char *Phone, long *conTractNo, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		long icontractNo;
		char phoneNo[15+1];
	EXEC SQL END DECLARE SECTION;

	init(phoneNo);

	strncpy(phoneNo, Phone, sizeof(phoneNo)-1);

	if (retCode == NULL || retMsg == NULL)
	{
		pubLog_Debug(_FFL_,"","","输入参数错！指针retCode或retMsg指向空");
		return -1;
	}

	EXEC SQL select contract_no
			   into :icontractNo
			   from dcustmsg
			  where phone_no = :phoneNo;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "GC0002");
		sprintf(retMsg, "通过电话号码(phoneNo:%s)查询账户错误![SQLCODE:%d]", phoneNo, SQLCODE);
		return -1;
	}

	*conTractNo = icontractNo;

	return 0;
}

/*功能：根据奖品代码giftCode、regionCode和smCode查找一条奖品规则，尽可能不要押金和预存，如果查询到多条就返回其中的一条
 *输入:
 		奖品代码giftCode，
 		地区代码regionCode，
 		品牌代码smCode，
 *输出: 规则代码ruleCode，
 *如果错误返回-1,retCode返回错误码，retMsg返回错误信息
 */
/*int markGetRuleCodeBygift(char *giftCode, char *regionCode, char *smCode, char *ruleCode, char *retCode, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vGiftCode[8+1];
		char vRegionCode[2+1];
		char vSmCode[2+1];
		char vRuleCode[8+1];
	EXEC SQL END DECLARE SECTION;

	init(vGiftCode);
	init(vRegionCode);
	init(vSmCode);
	init(vRuleCode);

	strncpy(vGiftCode, 		giftCode, 	sizeof(vGiftCode)-1);
	strncpy(vRegionCode, 	regionCode, sizeof(vRegionCode)-1);
	strncpy(vSmCode, 		smCode, 	sizeof(vSmCode)-1);

	EXEC SQL DECLARE cur_getgiftrule cursor for
				   select c.GIFT_RULE_CODE
					 from sGiftConfig a, sGiftRuleRel b,sGiftRule c, sMarkFavRuleRelease d
					where a.GIFT_CODE = b.GIFT_CODE
					  and b.Gift_Rule_Code = c.GIFT_RULE_CODE
					  and c.GIFT_RULE_CODE = d.Gift_Rule_Code
					  and Trim(d.region_code) = :vRegionCode
					  and Trim(a.GIFT_CODE) = :vGiftCode
					  and Trim(c.SM_CODE) = :vSmCode
					  order by c.prepay_least desc, c.deposit_least desc;
	EXEC SQL OPEN cur_getgiftrule;

	EXEC SQL FETCH cur_getgiftrule into :vRuleCode;
	if (SQLCODE != 0)
	{
		if (SQLCODE == 1403)
		{
			EXEC SQL CLOSE cur_getgiftrule;
			return 1;
		}
		strcpy(retCode, "PM1401");
		sprintf(retMsg, "根据奖品查找回馈规则错误");
		EXEC SQL CLOSE cur_getgiftrule;
		return -1;
	}
	else
	{
		EXEC SQL CLOSE cur_getgiftrule;
		Trim(vRuleCode);
		strcpy(ruleCode, vRuleCode);
		return 0;
	}

	return 0;
}
*/

/*功能：获取用户的当前可用积分
 *输入:
 		输入种类：falg=0	表示电话号码phone_no
 				  falg=1	表示用户ID	id_no
 				  falg=2	账号		contract_no
 		输入值：  value

 *输出: 用户当前可用积分	current_Point
 *成功返回0，否则返回非0。
 */
int markGetCurrentPoint(int flag, char *Value, long *current_Point)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vValue[15+1];
		long iContractNo = 0;
	  double vCurrentPoint = 0;
	EXEC SQL END DECLARE SECTION;

	init(vValue);

	if (Value == NULL || current_Point == NULL)
	{
		return -1;
	}

	strncpy(vValue, Value, sizeof(vValue)-1);
	Trim(vValue);
	
	if (flag == 0)
	{
		EXEC SQL select contract_no
				   into :iContractNo
				   from dcustmsg
				  where Trim(phone_no) = :vValue
				    and rownum < 2;
		if (SQLCODE != 0)
		{
			return -1001;
		}
	}
	else if (flag == 1)
	{
		EXEC SQL select contract_no
				   into :iContractNo
				   from dcustmsg
				  where id_no = to_number(:vValue)
				    and rownum < 2;
		if (SQLCODE != 0)
		{
			return -1002;
		}
	}
	else if (flag == 2)
	{
		iContractNo = atol(vValue);
	}

	EXEC SQL select current_point
			   into :vCurrentPoint
			   from dconMark
			  where contract_no = :iContractNo;
	if (SQLCODE != 0)
	{
		return -1003;
	}
	
	*current_Point = (long)iContractNo;

	return 0;
}

/*积分兑换，旧业务修改用，新开发的积分兑换不建议使用*/
/*int pubMarkConsume(
char	*iOpCode,
char	*iLoginNo,
char 	*iLoginAccept,
char	*iTotalDate,
char	*iOpTime,
char	*iUsePoint,
char	*iIdNo,
char	*iFavCode,
char	*iFavCount,
char	*iOpNote,
char	*iGoodId,
char	*oRetMsg
)
{
	char	buf[255];
	char	retCode[6+1];
	char	retMsg[255+1];
	int		ret = 0;

	EXEC SQL BEGIN DECLARE SECTION;
		char	vOpCode[4+1];
		char	vLoginNo[6+1];
		char 	vLoginAccept[14+1];
		char	vTotalDate[8+1];
		char	vOpTime[18+1];
		long	vUsePoint=0;
		char	vIdNo[14+1];
		char	vFavourCode[6+1];
		char	vFavourCount[6+1];
		char	vOpNote[60+1];
		char	vGoodId[20+1];
		char	vRegionCode[2+1];

		char	giftRuleCode[8+1];
		char	orgCode[10+1];
		char 	contractNo[15+1];
		char	phoneNo[15+1];
		char	smCode[2+1];

		int		sysYearMonth=0;
		int		totalDate=0;
	EXEC SQL END DECLARE SECTION;

	init(vOpCode);
	init(vLoginNo);
	init(vLoginAccept);
	init(vOpTime);
	init(vIdNo);
	init(vFavourCode);
	init(vFavourCount);
	init(vOpNote);
	init(vGoodId);
	init(vTotalDate);

	init(giftRuleCode);
	init(smCode);
	init(contractNo);
	init(phoneNo);
	init(retCode);
	init(retMsg);

	init(vRegionCode);

	strcpy(vOpCode, iOpCode);
	strcpy(vLoginNo, iLoginNo);
	strcpy(vLoginAccept, iLoginAccept);
	strcpy(vTotalDate, iTotalDate);
	strcpy(vOpTime, iOpTime);
	vUsePoint=atol(iUsePoint);
	strcpy(vIdNo, iIdNo);
	strcpy(vFavourCode, iFavCode);
	strcpy(vFavourCount, iFavCount);
	strcpy(vOpNote, iOpNote);
	strcpy(vGoodId, iGoodId);

	strcpy(buf, vOpTime);
	buf[6] = 0;
	sysYearMonth = atoi(buf);

	strcpy(buf, vOpTime);
	buf[8] = 0;
	totalDate = atoi(buf);

	EXEC SQL select to_char(contract_no), phone_no, sm_Code
			   into :contractNo, :phoneNo, :smCode
			   from dcustmsg
			  where id_no = to_number(:vIdNo);
	if (SQLCODE != 0)
	{
		strncpy(retCode, "PM1201", sizeof(retCode)-1);
		sprintf(retMsg, "通过用户id查询账号和电话号码错误！id_no:%s, SQLCODE:%d", vIdNo, SQLCODE);
		ret = 1;
		goto end;
	}

	Trim(contractNo);
	Trim(phoneNo);
	Trim(smCode);

	EXEC SQL SELECT Trim(substr(org_code,1,2)), org_code
			   INTO :vRegionCode, :orgCode
			   FROM dLoginMsg
			  WHERE login_no=:vLoginNo;
	if (SQLCODE != 0)
	{
		strncpy(retCode, "PM1202", sizeof(retCode)-1);
		sprintf(retMsg, "通过loginNo查询地区代码错误！login_no:%s, SQLCODE:%d", vLoginNo, SQLCODE);
		ret = 2;
		goto end;
	}

	if ((ret = markGetRuleCodeBygift(vFavourCode, vRegionCode, smCode, giftRuleCode, retCode, retMsg)) < 0)
	{
		ret = 3;
		goto end;
	}
	else if (ret == 1)
	{
		strcpy(giftRuleCode, "000000");
		ret = 0;
	}

	if (MarkAward(contractNo, vUsePoint, vLoginAccept, vLoginNo, vOpCode, vOpTime, sysYearMonth, retCode, retMsg) < 0)
	{
		ret = 4;
		goto end;
	}
	
	if (PublicMarkOpr(atol(vLoginAccept), 	phoneNo, 	giftRuleCode,	NULL,atol(vFavourCount),	vUsePoint, 
			0, 	sysYearMonth,     vLoginNo, totalDate,	vOpTime, 	vOpNote, vOpNote, 	orgCode, "0", vOpCode) <0)
	{
		ret = 5;
		goto end;
	}

end:
	if ( ret != 0)
	{
		printf("pubMarkConsume 错误！[retCode:%s, retMsg:%s], ret = %d\n",retCode, retMsg, ret);
		sprintf(oRetMsg, "retCode:%s, retMsg:%s", retCode, retMsg);
	}

	return ret;
}
*/


/*积分兑换冲正，旧业务修改用，新开发的积分兑换不建议使用*/
int pubMarkConsumeBack(
char	*iOpCode,
char	*iLoginNo,
char 	*iLoginAccept,
char	*iOldAccept,
char	*iOldYearMonth,
char	*iTotalDate,
char	*iOpTime,
char	*iOpNote,
char	*oRetMsg
)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vOpCode[4+1];
		char	vLoginNo[6+1];
		char 	vLoginAccept[14+1];
		char	vOldAccept[14+1];
		char	vOldYearMonth[6+1];
		char	vTotalDate[8+1];
		char	vOpTime[18+1];
		char	vOpNote[60+1];

		int 	sBackYearMonth;
		int 	sSysYearMonth;

		char	vRegionCode[2+1];
		char	vOrgCode[10+1];
		char  sOrgId[10+1];
	EXEC SQL END DECLARE SECTION;

	char 	tmp_buf[1024];
	char 	retCode[6+1];
	char 	retMsg[255+1];
	int 	ret = 0;

	init(vOpCode);
	init(vLoginNo);
	init(vLoginAccept);
	init(vOldAccept);
	init(vOldYearMonth);
	init(vTotalDate);
	init(vOpTime);
	init(vOpNote);
	init(sOrgId);

	init(vRegionCode);
	init(vOrgCode);

	init(retCode);
	init(retMsg);


	strncpy(vOpCode, 		iOpCode, 		sizeof(vOpCode)-1);
	strncpy(vLoginNo, 		iLoginNo, 		sizeof(vLoginNo)-1);
	strncpy(vLoginAccept, 	iLoginAccept, 	sizeof(vLoginAccept)-1);
	strncpy(vOldAccept, 	iOldAccept, 	sizeof(vOldAccept)-1);
	strncpy(vOldYearMonth, 	iOldYearMonth, 	sizeof(vOldYearMonth)-1);
	strncpy(vTotalDate, 	iTotalDate, 	sizeof(vTotalDate)-1);
	strncpy(vOpTime, 		iOpTime, 		sizeof(vOpTime)-1);
	strncpy(vOpNote, 		iOpNote, 		sizeof(vOpNote)-1);

	Trim(vOpCode);
	Trim(vLoginNo);
	Trim(vLoginAccept);
	Trim(vOldAccept);
	Trim(vOldYearMonth);
	Trim(vTotalDate);
	Trim(vOpTime);
	Trim(vOpNote);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpCode:[%s]", vOpCode);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vLoginNo:[%s]", vLoginNo);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vLoginAccept:[%s]", vLoginAccept);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOldAccept:[%s]", vOldAccept);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOldYearMonth:[%s]", vOldYearMonth);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vTotalDate:[%s]", vTotalDate);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpTime:[%s]", vOpTime);
	pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpNote:[%s]", vOpNote);
#endif


	/*判断输入的冲销年月是否为空*/
	if (strcmp(vOldYearMonth, "") == 0)
	{
		strcpy(retCode, "PM1301");
		sprintf(retMsg,"输入的冲正年月为空!");
		ret = 1;
		goto end;
	}

	EXEC SQL SELECT org_code
			   INTO :vOrgCode
			   FROM dLoginMsg
			  WHERE login_no=:vLoginNo;
	if (SQLCODE != 0)
	{
		strncpy(retCode, "PM1302", sizeof(retCode)-1);
		sprintf(retMsg, "通过loginNo查询地区代码错误！login_no:%s, SQLCODE:%d", vLoginNo, SQLCODE);
		ret = 2;
		goto end;
	}
	Trim(vOrgCode);
	
	/*--组织机构改造插入表字段edit by liuweib at 23/02/2009--begin*/
  ret = sGetLoginOrgid(vLoginNo,sOrgId);
  if(ret <0)
   	{
      printf("获取用户org_id失败!\n");
      return 200919; 
    }
  Trim(sOrgId);

	init(tmp_buf);
	strncpy(tmp_buf, vOpTime, sizeof(tmp_buf)-1);
	tmp_buf[6] = 0;
	sSysYearMonth = atoi(tmp_buf);

	if (MarkAwardBack(vOldAccept, atoi(vOldYearMonth), vLoginAccept, vLoginNo,
					vOpCode, vOpTime, sSysYearMonth, retCode, retMsg) < 0)
	{
		ret = 3;
		goto end;
	}

	init(tmp_buf);
	sprintf(tmp_buf,"update wMarkExchangeOpr%s "
					"   set back_flag='1'"
					" where login_accept=to_number(:vOldAccept)   ",
					vOldYearMonth);
	EXEC SQL PREPARE pubMkConsumeBack1 from :tmp_buf;
	EXEC SQL EXECUTE pubMkConsumeBack1 using :vOldAccept;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1303");
		sprintf(retMsg, "更新积分兑换日志表失败[SQLCODE:%d]", SQLCODE);
		ret = 4;
		goto end;
	}
	
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 23/02/2009*/
	init(tmp_buf);
	sprintf(tmp_buf,"insert into wMarkExchangeOpr%d"
	                "("
	                "    LOGIN_ACCEPT,CONTRACT_NO,GIFT_RULE_CODE,VOLUME,"
                    "    CONSUME_MARK,FAVOUR_MARK,ID_NO,SM_CODE,PHONE_NO,ORG_CODE,LOGIN_NO,TOTAL_DATE,"
                    "    OP_TIME,SYSTEM_NOTE,OP_NOTE,IP_ADDR,ORG_ID,BACK_FLAG,OP_CODE"
                    ")"
                    "select"
	                "    to_number(:LOGIN_ACCEPT),CONTRACT_NO,GIFT_RULE_CODE,-1*VOLUME,"
                    "    -1*CONSUME_MARK,-1*FAVOUR_MARK,ID_NO,SM_CODE,PHONE_NO,:ORG_CODE,:LOGIN_NO,to_number(:TOTAL_DATE),"
                    "    to_date(:OP_TIME, 'YYYYMMDD HH24:MI:SS'),:SYSTEM_NOTE,:OP_NOTE,'0.0.0.0',ORG_ID,'1',:OP_CODE"
                    "  FROM wMarkExchangeOpr%s "
                    " WHERE login_accept=to_number(:sBackLoginAccept)"
                    , sSysYearMonth, vOldYearMonth);
	EXEC SQL PREPARE pubMkConsumeBack2 from :tmp_buf;
	EXEC SQL EXECUTE pubMkConsumeBack2 using :vLoginAccept, :vOrgCode, :vLoginNo, :vTotalDate,
			:vOpTime, :vOpNote, :vOpNote, :sOrgId, :vOpCode, :vOldAccept;
	if (SQLCODE != 0)
	{
		strcpy(retCode, "PM1304");
		sprintf(retMsg, "更新帐户积分变更明细表的冲正标志失败[SQLCODE:%d]", SQLCODE);
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,"","","tm_buf:%s\n", tmp_buf);
		pubLog_Debug(_FFL_,"","","vOrgCode:%s\n", vOrgCode);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpCode:[%s]\n", vOpCode);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vLoginNo:[%s]\n", vLoginNo);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vLoginAccept:[%s]\n", vLoginAccept);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOldAccept:[%s]\n", vOldAccept);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOldYearMonth:[%s]\n", vOldYearMonth);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vTotalDate:[%s]\n", vTotalDate);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpTime:[%s]\n", vOpTime);
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack inparam vOpNote:[%s]\n", vOpNote);
#endif
		ret = 5;
		goto end;
	}

end:
	if ( ret != 0)
	{
		pubLog_Debug(_FFL_,"","","pubMarkConsumeBack错误！[retCode:%s, retMsg:%s]\n",retCode, retMsg);
		sprintf(oRetMsg, "retCode:%s, retMsg:%s", retCode, retMsg);
	}
	return ret;
}



