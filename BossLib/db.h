#ifndef __db_h__
#define __db_h__

#define SQLCODE sqlca.sqlcode
#define NOTFOUND 1403

//Ĭ�Ϻ�������󷵻�����
#define DEFAULT_MAX_ROWCOUNT	100

ETYPE ProcDBLogin(const char* m_sDataSource, const char* m_sUserId, const char* m_sPasswd);


void ProcDBClose();

#endif /*__db_h__*/

