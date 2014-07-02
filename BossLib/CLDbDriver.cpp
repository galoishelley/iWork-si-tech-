#include "llayerStruct.h"

//SQLLIB functions for V8 OCI interoperability
extern "C" {
#include "sql2oci.h"
}

#ifndef WIN32
const ub4 CLConnection::DB_CONN_TYPE_OCI;
const ub4 CLConnection::DB_CONN_TYPE_PROC;
#endif//WIN32

CLConnection::CLConnection()
{
	ConnectString = "";
	m_sDataSource = "";
	m_sUserId = "";
	m_sPasswd = "";
	m_Command = NULL;

	m_envhp = NULL;
	m_srvhp = NULL;
	m_svchp = NULL;
	m_authp = NULL;
	
	m_errhp = NULL;
	m_loggedOn = FALSE;
	m_ociMode = OCI_DEFAULT;
	m_connDbType = DB_CONN_TYPE_OCI;
}

CLConnection::~CLConnection()
{
	Destruction();
}

/* ----------------------------------------------------------------- */
/* Attach to server with a given mode.                               */
/* ----------------------------------------------------------------- */
ETYPE CLConnection::AttachServer(const char *tnsName)
{
	text *cstring = (text *)"";

	DBUG_ENTER("CLConnection::AttachServer");

	if (OCIServerAttach(m_srvhp, m_errhp, (text *) tnsName,
		(sb4) strlen((char *)tnsName), (ub4) m_ociMode))
	{
		DBUG_RETURN(CLError::E_DB_OCI_SRV_ATTACH);
	}

	/* Set the server handle in the service handle */
	if (OCIAttrSet((dvoid *) m_svchp, (ub4) OCI_HTYPE_SVCCTX,
		(dvoid *) m_srvhp, (ub4) 0, (ub4) OCI_ATTR_SERVER, m_errhp))
	{
		DBUG_RETURN(CLError::E_DB_OCI_ATTR_SET);
	}

	DBUG_RETURN(0);
}


// Logon to the database using given username, password & credentials
ETYPE CLConnection::LogOn(const char *uid, const char *pwd, const char* tnsName, ub4 credt)
{
	ETYPE iRet;

	DBUG_ENTER("CLConnection::LogOn");

	if ((iRet = AttachServer(tnsName)) != 0)
	{
		FreeHandles();
		DBUG_RETURN(iRet);
	}
	/* Set attributes in the authentication handle */
	if (OCIAttrSet((dvoid *) m_authp, (ub4) OCI_HTYPE_SESSION,
		(dvoid *) uid, (ub4) strlen((char *) uid),
		(ub4) OCI_ATTR_USERNAME, m_errhp))
	{
		DBUG_RETURN(CLError::E_DB_OCI_LOG_ON);
	}
	if (OCIAttrSet((dvoid *) m_authp, (ub4) OCI_HTYPE_SESSION,
		(dvoid *) pwd, (ub4) strlen((char *) pwd),
		(ub4) OCI_ATTR_PASSWORD, m_errhp))
	{
		DBUG_RETURN(CLError::E_DB_OCI_LOG_ON);
	}

	if (OCISessionBegin(m_svchp, m_errhp, m_authp, credt, m_ociMode))
	{
		DBUG_RETURN(CLError::E_DB_OCI_LOG_ON);
	}

	/* Set the authentication handle in the Service handle */
	if (OCIAttrSet((dvoid *) m_svchp, (ub4) OCI_HTYPE_SVCCTX,
		(dvoid *) m_authp, (ub4) 0, (ub4) OCI_ATTR_SESSION, m_errhp))
	{
		DBUG_RETURN(CLError::E_DB_OCI_LOG_ON);
	}

	m_loggedOn = TRUE;
	DBUG_RETURN(0);
}

void CLConnection::LogOutDetachServer()
{
	DBUG_ENTER("CLConnection::LogOutDetachServer");

	if (OCISessionEnd(m_svchp, m_errhp, m_authp, (ub4) 0))
	{
		//
	}


	if (OCIServerDetach(m_srvhp, m_errhp, m_ociMode))
	{
		//
	}
	DBUG_VOID_RETURN;
}

void CLConnection::Destruction()
{
	if (m_loggedOn)
	{
		DeleteCommond();
		if (m_connDbType == DB_CONN_TYPE_OCI)
		{
			LogOutDetachServer();
		}

		FreeHandles();

		if (m_connDbType == DB_CONN_TYPE_PROC)
		{
			ProcDBClose();
		}
	}

	m_loggedOn = FALSE;
}

//得到数据库标签的内容：数据库名、用户名和口令。
ETYPE CLConnection::GetDbP(const char *connectName)
{
#ifdef WIN32
	string m_sDataSource;
	string m_sUserId;
	string m_sPasswd;
	
	m_sDataSource = "orcl";
	m_sUserId = "dbcustadm";
	m_sPasswd = "111111";
#else
	TComShm *shmp;
	register int  i;
	ETYPE iErr;

	if ((iErr = CLShmDriver::GetDbShm(&shmp)) != 0)
	{
		return iErr;
	}
	
	for ( i=0; i<shmp->connectNum; i++)
	{
		if (strcmp(shmp->connects[i].connectName, connectName) == 0)
		{
			m_sDataSource = shmp->connects[i].dbSrv;
			m_sUserId = shmp->connects[i].userName;
			m_sPasswd = shmp->connects[i].userPwd;
			break;
		}
	}
	if (i == shmp->connectNum)
	{
		CLShmDriver::DetachShm(shmp);
		return CLError::E_DB_GET_LABEL;
	}
	if ((iErr = CLShmDriver::DetachShm(shmp)) != 0)
	{
		return iErr;
	}
#endif	
	return 0;
}

void CLConnection::GetLogInfo() throw()
{
	basic_string <char>::size_type beginPos = 0;
	basic_string <char>::size_type endPos = 0;
	basic_string <char>::size_type curPos = 0;

	if ((beginPos = ConnectString.find(CS_DATA_SOURCE)) != string::npos)
	{
		if ((endPos = ConnectString.find_first_of(';', beginPos)) != string::npos)
		{
			m_sDataSource=ConnectString.substr(beginPos + 11+1,endPos - (beginPos + 11+1));
		}
		if (m_sDataSource.size() > 0)
		{
			if (m_sDataSource.c_str()[0] == '\'')
			{
				m_sDataSource.erase(0, 1);
			}
		}
		if (m_sDataSource.size() > 0)
		{
			if (m_sDataSource.c_str()[m_sDataSource.size()-1] == '\'')
			{
				m_sDataSource.erase(m_sDataSource.size()-1, 1);
			}

		}
	}
	if ((beginPos = ConnectString.find(CS_USER_ID)) != string::npos)
	{
		if ((endPos = ConnectString.find_first_of(';', beginPos)) != string::npos)
		{
			m_sUserId=ConnectString.substr(beginPos + 7+1,endPos - (beginPos + 7+1));
		}
		if (m_sUserId.size() > 0)
		{
			if (m_sUserId.c_str()[0] == '\'')
			{
				m_sUserId.erase(0, 1);
			}
		}
		if (m_sUserId.size() > 0)
		{
			if (m_sUserId.c_str()[m_sUserId.size()-1] == '\'')
			{
				m_sUserId.erase(m_sUserId.size()-1, 1);
			}
		}
	}
	if ((beginPos = ConnectString.find(CS_PASSWORD)) != string::npos )
	{
		if ((endPos = ConnectString.find_first_of(';', beginPos)) != string::npos)
		{
			m_sPasswd=ConnectString.substr(beginPos + 8+1,endPos - (beginPos + 8+1));
		}
		if (m_sPasswd.size() > 0)
		{
			if (m_sPasswd.c_str()[0] == '\'')
			{
				m_sPasswd.erase(0, 1);
			}
		}
		if (m_sPasswd.size() > 0)
		{
			if (m_sPasswd.c_str()[m_sPasswd.size()-1] == '\'')
			{
				m_sPasswd.erase(m_sPasswd.size()-1, 1);
			}
		}
	}
}

ETYPE CLConnection::Open(ub4 connDbType)  throw()
{
	ETYPE iRet=0;

	DBUG_ENTER("CLConnection::Open");

	if (m_loggedOn)
	{
		DBUG_RETURN(0);
	}

	m_ociMode = OCI_DEFAULT;

	m_connDbType = connDbType;

	GetLogInfo();

	if (connDbType == DB_CONN_TYPE_PROC)
	{
		if (iRet = ::ProcDBLogin(m_sDataSource.c_str(), m_sUserId.c_str(), m_sPasswd.c_str()))
		{
			DBUG_RETURN(iRet);
		}
		m_loggedOn = TRUE;
	}
	
	if ((iRet = Initialize()) != 0)
	{
		DBUG_RETURN(iRet);
	}

	if (connDbType == DB_CONN_TYPE_OCI)
	{
		// Logon to the server and begin a session
		if ((iRet=LogOn(m_sUserId.c_str(), m_sPasswd.c_str(), m_sDataSource.c_str(),(ub4) OCI_CRED_RDBMS)) != 0)
		{
			DBUG_RETURN(iRet);
		}
	}

	if (( iRet = CreateCommond()) != 0)
	{
		DBUG_RETURN(iRet);
	}
	DBUG_RETURN(iRet);
}

void CLConnection::Close() throw()
{
	DBUG_ENTER("CLConnection::Close");
	Destruction();
	DBUG_VOID_RETURN;
}

TDriverType CLConnection::GetDriverType() const
{
	return DRIVER_DATABASE;
}

ETYPE CLConnection::InitHandles()
{
	DBUG_ENTER("CLConnection::InitHandles");

	/* Initialize the OCI Process */
	if (OCIInitialize(m_ociMode, (dvoid *)0,
		(dvoid * (*)(dvoid *, size_t)) 0,
		(dvoid * (*)(dvoid *, dvoid *, size_t))0,
		(void (*)(dvoid *, dvoid *)) 0 ))
	{
		return CLError::E_DB_OCI_INIT_ERROR;
		DBUG_RETURN(CLError::E_DB_OCI_SRV_ATTACH);
	}

	if (m_connDbType == DB_CONN_TYPE_OCI)
	{
		/* Inititialize the OCI Environment */
		if (OCIEnvInit((OCIEnv **) &m_envhp, (ub4) m_ociMode,
			(size_t) 0, (dvoid **) 0 ))
		{
			DBUG_RETURN(CLError::E_DB_OCI_INIT_ENV);
		}

		/* Allocate a service handle */
		if (OCIHandleAlloc((dvoid *) m_envhp, (dvoid **) &m_svchp,
			(ub4) OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
		{
			DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_SVC);
		}

		/* Allocate a server handle */
		if (OCIHandleAlloc((dvoid *) m_envhp, (dvoid **) &m_srvhp,
			(ub4) OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0))
		{
			DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_SRV);
		}

		/* Allocate a authentication handle */
		if (OCIHandleAlloc((dvoid *) m_envhp, (dvoid **) &m_authp,
			(ub4) OCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0))
		{
			DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_AUTH);
		}
	}
	else if (m_connDbType == DB_CONN_TYPE_PROC)
	{
		// get an OCI formatted environment handle from the primary connection
		if (SQLEnvGet(0,&m_envhp) != 0)
		{
			DBUG_RETURN(CLError::E_DB_OCI_INIT_ENV);
		}

		// get an OCI formatted service context handle from the primary connection
		if (SQLSvcCtxGet(0,0,-1,&m_svchp) != 0)
		{
			DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_SVC);
		}
	}

	/* Allocate an error handle */
	if (OCIHandleAlloc((dvoid *) m_envhp, (dvoid **) &m_errhp,
		(ub4) OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0))
	{
		DBUG_RETURN(CLError::E_DB_OCI_HDL_ALLOC_ERR);
	}


	DBUG_RETURN(0);
}

/* ----------------------------------------------------------------- */
/*  Free the specified handles                                       */
/* ----------------------------------------------------------------- */
void CLConnection::FreeHandles()
{
	DBUG_ENTER("CLConnection::FreeHandles");

	if (m_connDbType == DB_CONN_TYPE_OCI)
	{
		if (m_srvhp)
		{
			(void) OCIHandleFree((dvoid *) m_srvhp, (ub4) OCI_HTYPE_SERVER);
			m_srvhp = NULL;
		}
		if (m_svchp)
		{
			(void) OCIHandleFree((dvoid *) m_svchp, (ub4) OCI_HTYPE_SVCCTX);
			m_svchp = NULL;
		}
		if (m_envhp)
		{
			(void) OCIHandleFree((dvoid *) m_envhp, (ub4) OCI_HTYPE_ENV);
			m_envhp = NULL;
		}
		if (m_authp)
		{
			(void) OCIHandleFree((dvoid *) m_authp, (ub4) OCI_HTYPE_SESSION);
			m_authp = NULL;
		}
	}
	else if (m_connDbType == DB_CONN_TYPE_PROC)
	{
		//SQLSvcCtxGet
		m_svchp = NULL;

		//SQLEnvGet
		m_envhp = NULL;
	}

	if (m_errhp)
	{
		(void) OCIHandleFree((dvoid *) m_errhp, (ub4) OCI_HTYPE_ERROR);
		m_errhp = NULL;
	}
	DBUG_VOID_RETURN;
}

ETYPE CLConnection::Initialize()
{
	ETYPE iRet=0;

	DBUG_ENTER("CLConnection::Initialize");
	if ((iRet = InitHandles()) != 0)
	{
		FreeHandles();
		DBUG_RETURN(iRet);
	}
	DBUG_RETURN(iRet);
}

ETYPE CLConnection::CreateCommond() throw()
{
	ETYPE iRet = 0;

	m_Command = new CLCommand();
	if (!m_Command)
	{
		return CLError::E_MEMORY_LACK;
	}
	m_Command->ActiveConnection = this;
	m_Command->BindToRecordset();
	if ( (iRet = m_Command->InitParameters(0)) != 0)
	{
		delete m_Command;
		m_Command = NULL;
	}
	g_Command = m_Command;
	return iRet;
}

/*
	删除一个命令类，
 */
void CLConnection::DeleteCommond() throw()
{
	if (m_Command)
	{
		delete m_Command;
		m_Command = NULL;
	}
}

int CLConnection::IsLoggedOn() throw()
{
	return m_loggedOn;
}

CLCommand *g_Command = NULL;

