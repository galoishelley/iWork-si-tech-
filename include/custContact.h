#ifndef __custContact_h__
#define __custContact_h__

/**
 * ��wLoginOprYYYYMM�ñ�ṹ���塣
 * totalDate	��������
 * loginAccept	��ˮ
 * opCode		��������
 * payType		��������
 * payMoney		���ѿ�				
 * smCode		ҵ�����
 * idNo			�û�ID
 * phoneNo		�ֻ�����
 * orgCode		���Ź�������
 * loginNo		����
 * opNote		��ע
 * ipAddr		IP��ַ
 * orgId
 */
typedef struct tagwLoginOpr
{
	ub4	totalDate;			/*��ʽ��YYYYMMDD��Ĭ��ֵ��sysdate*/
	ub8	loginAccept;
	sb1	opCode[4+1];
	sb1 payType[1+1];		/*Ĭ��ֵ��'0'*/
	double payMoney;		/*Ĭ��ֵ��0.00*/
	sb1 smCode[2+1];		/*Ĭ��ֵ��NULL*/
	ub8 idNo;				/*Ĭ��ֵ��NULL*/
	sb1 phoneNo[15+1];		/*Ĭ��ֵ��NULL*/
	sb1 orgCode[9+1];		/*Ĭ��ֵ��dLoginMsg.org_code*/
	sb1 loginNo[6+1];
	sb1 opTime[17+1];		/*��ʽ��YYYYMMDD HH24:MI:SS��Ĭ��ֵ��sysdate*/
	sb1 opNote[60+1];		/*Ĭ��ֵ��NULL*/
	sb1 ipAddr[15+1];		/*��ʽ��XXX.XXX.XXX.XXX��Ĭ��ֵ��NULL*/
	sb1	orgId[10+1];		/*Ĭ��ֵ��dLoginMsg.org_id*/
} TwLoginOpr;

typedef struct tagContactInfo
{
	 ub8 ContactId;                  /*�ͻ��Ӵ���ʶ       */
	 ub4 ContractType;               /*�Ӵ��¼����ͱ�ʶ   */
	 ub4 CustIdType;                 /*�ͻ���ʶ����       */
	 ub8 CustId;					 /*�ͻ���ʶ           */
	 ub8 IdNo;			    	     /*�û���ʶ           */
	 ub8 ContractNo;				 /*�ʻ���ʶ           */
	 sb1 GroupId[10+1];			     /*������ʶ           */
	 ub4 InterfaceCode;	             /*�Ӵ���ʽ����       */
	 sb1 ContactReason[100+1];		 /*�Ӵ�ԭ��           */
	 sb1 ContactContent[512+1];      /*�Ӵ�����           */
	 sb1 sBeginTime[17+1];			 /*�Ӵ���ʼʱ��       */
	 sb1 sEndTime[17+1];			 /*�Ӵ�����ʱ��       */
	 ub4 ContactStatus;			     /*�Ӵ�״̬           */
	 sb1 LoginNo[6+1];			     /*�����˱�ʶ         */
	 ub4 ContractDirection;	         /*�Ӵ�����           */
	 ub8 ContactSessionId;	         /*�ͻ��Ӵ��Ự��ʶ,�ɿ�*/
	 sb1 sAgentPerson[60+1];		 /*������ �ɿ�*/
	 ub4 iTerminateInterface;	     /*�Ӵ��ն˽�������*/
} TContactInfo;

#endif /*__custContact_h__*/
