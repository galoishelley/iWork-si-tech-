#ifndef __custAuthen_h__
#define __custAuthen_h__

/*客户*/
#define USER_ID_TYPE_CUST		0

/*用户*/
#define USER_ID_TYPE_USER		1

/*帐户*/
#define USER_ID_TYPE_CONTRACT	2


/*客户认证方式*/
#define AUTH_AUTHENTICATION				0	/*认证方式          */
#define AUTH_PASSWORD					1	/*密码认证          */
#define AUTH_CERTIFICATE				2	/*证件认证          */
#define AUTH_RANDOM_PWD					3	/*随机短信密码认证          */
#define AUTH_SERVICE_PASSWORD			4	/*服务密码认证          */
#define AUTH_IDENTITY					5	/*身份认证          */
#define AUTH_CONSIGN					6	/*委托认证          */


/**
 * 客户身份认证随机密码流程
 * @author	
 * @version 
 * @since       1.00
 * @inparam		authen_code		认证方式
 * @inparam		cust_pwd		客户密码	
 * @inparam		cust_id_type	客户号类型
 * @inparam		cust_id			客户号 		
 * @outparam	retCode	错误代码
 * @outparam	retMsg	错误信息
 * @return 返回0 表示校验成功	1 表示校验失败，否则出错！
 */


                            
                                                            
#endif /*__custAuthen_h__*/
