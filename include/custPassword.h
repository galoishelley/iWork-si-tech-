#ifndef __custPassword_h__
#define __custPassword_h__
/**
 *    ��ȡϵͳ������ˮ
 */
int GetSysMaxAccept(ub8 *lLoginAccept, char *retCode, char *retMsg);

/**
 * ����ͻ���������
 * @author	
 * @version 
 * @since       1.00
 * @inparam	authen_code         ��֤��ʽ
 * @inparam	cust_detail_id      �ͻ���ϸ��ʶ
 * @inparam	random_password     �������
 * @inparam	op_time             ����ʱ��      
 * @inparam	begin_time         	��ʼʱ��
 * @outparam	retCode	�������
 * @outparam	retMsg	������Ϣ
 * @return ����0��ʾ��ȷ���������
 */
int CreateCustRandPwd(int custIdType, long custDetialId, int pwdLength, int authenCode, const char* loginNo,
	char *randPwd, ub4 effectDays, int genLog, const char *opTime, char *retCode, char *retMsg);

#endif /*__custPassword_h__*/
