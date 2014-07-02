#ifndef __db_h__
#define __db_h__

#define SQLCODE sqlca.sqlcode
#define NOTFOUND 1403

//默认函数的最大返回数量
#define DEFAULT_MAX_ROWCOUNT	100

ETYPE ProcDBLogin(const char* m_sDataSource, const char* m_sUserId, const char* m_sPasswd);


void ProcDBClose();

#endif /*__db_h__*/

