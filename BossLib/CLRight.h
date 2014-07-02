#ifndef __CLRight_h__
#define __CLRight_h__
/**
 *  权限管理类
 */
extern "C" ETYPE CheckFuncRight(const char *loginNo, const char *opCode);

//检查列级权限
//    有权限返回零，否则返回非零
extern "C" ETYPE CheckColumnRight(const char *loginNo, ub8 columnId);

class CLRight : public CLBase
{
public:
	//检查功能权限
	//loginPasswd加密后的密码
	static ETYPE CheckFuncPwdRight(const char *loginNo,
		const char *loginPasswd, const char *opCode) throw();

	//检查列级权限
	//    有权限返回零，否则返回非零
	static inline ETYPE CheckColumnRight(const char *loginNo, ub8 columnId)
	{
		return ::CheckColumnRight(loginNo, columnId);
	}
};

#endif //__CLRight_h__
