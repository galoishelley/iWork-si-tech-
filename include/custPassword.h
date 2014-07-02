#ifndef __custPassword_h__
#define __custPassword_h__
/**
 *    提取系统操作流水
 */
int GetSysMaxAccept(ub8 *lLoginAccept, char *retCode, char *retMsg);

/**
 * 插入客户随机密码表
 * @author	
 * @version 
 * @since       1.00
 * @inparam	authen_code         认证方式
 * @inparam	cust_detail_id      客户明细标识
 * @inparam	random_password     随机密码
 * @inparam	op_time             生成时间      
 * @inparam	begin_time         	开始时间
 * @outparam	retCode	错误代码
 * @outparam	retMsg	错误信息
 * @return 返回0表示正确，否则出错！
 */
int CreateCustRandPwd(int custIdType, long custDetialId, int pwdLength, int authenCode, const char* loginNo,
	char *randPwd, ub4 effectDays, int genLog, const char *opTime, char *retCode, char *retMsg);

#endif /*__custPassword_h__*/
