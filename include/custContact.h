#ifndef __custContact_h__
#define __custContact_h__

/**
 * 表wLoginOprYYYYMM得表结构定义。
 * totalDate	帐务日期
 * loginAccept	流水
 * opCode		操作代码
 * payType		费用类型
 * payMoney		交费款				
 * smCode		业务代码
 * idNo			用户ID
 * phoneNo		手机号码
 * orgCode		工号归属代码
 * loginNo		工号
 * opNote		备注
 * ipAddr		IP地址
 * orgId
 */
typedef struct tagwLoginOpr
{
	ub4	totalDate;			/*格式：YYYYMMDD。默认值：sysdate*/
	ub8	loginAccept;
	sb1	opCode[4+1];
	sb1 payType[1+1];		/*默认值：'0'*/
	double payMoney;		/*默认值：0.00*/
	sb1 smCode[2+1];		/*默认值：NULL*/
	ub8 idNo;				/*默认值：NULL*/
	sb1 phoneNo[15+1];		/*默认值：NULL*/
	sb1 orgCode[9+1];		/*默认值：dLoginMsg.org_code*/
	sb1 loginNo[6+1];
	sb1 opTime[17+1];		/*格式：YYYYMMDD HH24:MI:SS。默认值：sysdate*/
	sb1 opNote[60+1];		/*默认值：NULL*/
	sb1 ipAddr[15+1];		/*格式：XXX.XXX.XXX.XXX。默认值：NULL*/
	sb1	orgId[10+1];		/*默认值：dLoginMsg.org_id*/
} TwLoginOpr;

typedef struct tagContactInfo
{
	 ub8 ContactId;                  /*客户接触标识       */
	 ub4 ContractType;               /*接触事件类型标识   */
	 ub4 CustIdType;                 /*客户标识类型       */
	 ub8 CustId;					 /*客户标识           */
	 ub8 IdNo;			    	     /*用户标识           */
	 ub8 ContractNo;				 /*帐户标识           */
	 sb1 GroupId[10+1];			     /*渠道标识           */
	 ub4 InterfaceCode;	             /*接触方式代码       */
	 sb1 ContactReason[100+1];		 /*接触原因           */
	 sb1 ContactContent[512+1];      /*接触内容           */
	 sb1 sBeginTime[17+1];			 /*接触开始时间       */
	 sb1 sEndTime[17+1];			 /*接触结束时间       */
	 ub4 ContactStatus;			     /*接触状态           */
	 sb1 LoginNo[6+1];			     /*受理人标识         */
	 ub4 ContractDirection;	         /*接触方向           */
	 ub8 ContactSessionId;	         /*客户接触会话标识,可空*/
	 sb1 sAgentPerson[60+1];		 /*代办人 可空*/
	 ub4 iTerminateInterface;	     /*接触终端界面类型*/
} TContactInfo;

#endif /*__custContact_h__*/
