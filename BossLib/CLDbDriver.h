#ifndef __CLDbDriver_h__
#define __CLDbDriver_h__

#define MAXLABELLEN	12
#define MAXLOGIN        100

#define MAX_COL_LENGTH 60

//数据库连接结构。
typedef struct tagConnect
{
	char connectName[MAXLABELLEN+1];
	char dbSrv[MAXLABELLEN+1];
	char dbName[MAXLABELLEN+1];
	char userName[MAXLABELLEN + 1];
	char userPwd[MAXENDELEN+1];
} TConnect;

typedef struct tagComShm
{
	ub4 connectNum;
	TConnect connects[MAXLOGIN];
}TComShm;


//连接数据库参数
typedef enum tagDbConnFlag
{
	DB_CONN_UNUSE_LABEL = 0,	//不使用标签作连接。
	DB_CONN_USE_LABEL = 1		//使用标签作连接。
}EDbConnFlag;

//断开数据库参数
typedef enum tagDbDisConnFlag
{
	DB_DISCONN_COMMIT = 1,		//在断开连接之前，提交数据库操作。
	DB_DISCONN_ROLLBACK = 2		//在断开连接之前，回滚数据库操作。
}EDbDisConnFlag;

//当执行ExecuteSql()函数时，如果查询操作，当从游标读取一行数据完成后会触发此事件。
//返回值：如果返回值不为零，则中断当前游标的执行，并返回此函数的错误代码。
//typedef ETYPE (*OnFetchCursor)();

class CLConnection;
#define CS_DATA_SOURCE			"Data Source"
#define CS_USER_ID				"User Id"
#define CS_PASSWORD				"Password"

class CLCommand;
class CLConnection : public CLDriver
{
public:
	CLConnection();
	~CLConnection();

	//驱动类型定义代码
	virtual TDriverType GetDriverType() const;
public:
	/*
	 Data Source:数据源
	 User Id:用户名
	 Password:口令
	 */
	string ConnectString;
	ETYPE Open(ub4 connDbType) throw();

	//通过数据库标签连接数据库
	inline ETYPE Open(const char* dbLabel) throw()
	{
		ETYPE iRet;
		if ( (iRet = GetDbP(dbLabel)) != 0)
		{
			return iRet;
		}
		return Open(DB_CONN_TYPE_PROC);
	}
	
	void Close() throw();

	//是否登录状态
	int IsLoggedOn() throw();

	OCIEnv *GetEnvhp() throw()
	{
		return m_envhp;
	}
	OCIServer *GetSrvhp() throw()
	{
		return m_srvhp;
	}
	OCISvcCtx *GetSvchp() throw()
	{
		return m_svchp;
	}
	OCISession *GetAuthp() throw()
	{
		return m_authp;
	}
	OCIError *GetErrhp() throw()
	{
		return m_errhp;
	}
	
	ub4 GetOciMode() throw()
	{
		return m_ociMode;
	}

	//连接的数据库类型
	//OCI模式
	static const ub4 DB_CONN_TYPE_OCI	= 0;

	//PRO*C模式
	static const ub4 DB_CONN_TYPE_PROC	= 1;

private:
	/*
		生成一个命令类，
	 */
	ETYPE CreateCommond() throw();

	/*
		删除一个命令类，
	 */
	void DeleteCommond() throw();

	ETYPE InitHandles();
	void FreeHandles();

	//初始化用户句柄
	ETYPE Initialize();

	// Logon to the database using given username, password & credentials
	ETYPE LogOn(const char *uid, const char *pwd, const char* tnsName, ub4 credt);

	ETYPE AttachServer(const char *tnsName);
	void LogOutDetachServer();
	void Destruction();

	//得到数据库标签的内容：数据库名、用户名和口令。
	ETYPE GetDbP(const char *connectName);
	
	OCIEnv *m_envhp;
	OCIServer *m_srvhp;
	OCISvcCtx *m_svchp;
	OCISession *m_authp;
	OCIError *m_errhp;

	int m_loggedOn;
	ub4 m_ociMode;
private:
	void GetLogInfo() throw();
	string m_sDataSource;
	string m_sUserId;
	string m_sPasswd;

	//连接的数据库类型
	ub4 m_connDbType;

	CLCommand *m_Command;
};

//全局数据库命令类指针
extern CLCommand *g_Command;

#endif //__CLDBDriver_h__
