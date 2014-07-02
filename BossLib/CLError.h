#ifndef __CLError_h__
#define __CLError_h__

/*
	错误分类：
		一、系统级别错误
			1、数据库驱动错误
			2、文件错误
			3、共享内存错误
		二、业务级别错误
 */
//错误处理类
typedef struct tagErrorNoMsg
{
	ETYPE	errorNo;
	const	char* errorMsg;
} TErrorNoMsg;

class CLError :public CLBase
{
public:
	//最大错误数量
	static const ETYPE MAX_ERROR_NUM					= 10000;
	
	static const ETYPE E_SUCCESS						= 0x00000000;
	static const ETYPE E_MAX_ERROR_NO					= 0xFFFFFFFF;
	
	//数据库类错误
	static const ETYPE E_DATABASE						= 0x00001000;
	static const ETYPE E_DB_SQL_NOCONNECT				= 0x00001001;
	static const ETYPE E_DB_SQL_PREPARE 				= 0x00001002;
	static const ETYPE E_DB_SQL_DATA_TYPE				= 0x00001003;
	static const ETYPE E_DB_DISCONNFLAG					= 0x00001004;
	static const ETYPE E_DB_SQLALD_BIND					= 0x00001005;
	static const ETYPE E_DB_SQL_EXECUTE_ITERS			= 0x00001006;
	static const ETYPE E_DB_BIND_MAXIMUM				= 0x00001007;
	static const ETYPE E_DB_SELECT_MAXIMUM				= 0x00001008;
	static const ETYPE E_DB_SQL_EXECUTE					= 0x00001009;
	static const ETYPE E_DB_SQL_UNKNOWN					= 0x0000100A;
	static const ETYPE E_DB_OCI_INIT_ERROR				= 0x0000100B;
	static const ETYPE E_DB_OCI_INIT_ENV				= 0x0000100C;
	static const ETYPE E_DB_OCI_HDL_ALLOC_SVC			= 0x0000100D;
	static const ETYPE E_DB_OCI_HDL_ALLOC_ERR			= 0x0000100E;
	static const ETYPE E_DB_OCI_HDL_ALLOC_SRV			= 0x0000100F;
	static const ETYPE E_DB_OCI_HDL_ALLOC_AUTH			= 0x00001010;
	static const ETYPE E_DB_OCI_HDL_ALLOC_STMT			= 0x00001011;
	static const ETYPE E_DB_OCI_SRV_ATTACH				= 0x00001012;
	static const ETYPE E_DB_OCI_ATTR_GET				= 0x00001013;
	static const ETYPE E_DB_OCI_ATTR_SET				= 0x00001014;
	static const ETYPE E_DB_OCI_LOG_ON					= 0x00001015;
	static const ETYPE E_DB_COMMIT						= 0x00001016;
	static const ETYPE E_DB_ROLLBACK					= 0x00001017;
	static const ETYPE E_DB_SQL_FETCH					= 0x00001018;
	static const ETYPE E_DB_SQL_FETCH_NODATA			= 0x00001019;
	static const ETYPE E_DB_GET_ROW_COUNT				= 0x00001020;
	static const ETYPE E_DB_GET_FETCHED_COUNT			= 0x00001021;
	static const ETYPE E_DB_SQL_DEFINE					= 0x00001022;
	static const ETYPE E_DB_GET_COL_ATTR				= 0x00001023;
	static const ETYPE E_DB_GET_LABEL					= 0x00001024;

	//共享内存类错误
	static const ETYPE E_SHM							= 0x00002000;
	static const ETYPE E_SHM_GETKEY						= 0x00002001;
	static const ETYPE E_SHM_GET_ID						= 0x00002002;
	static const ETYPE E_SHM_ASSOCIATED					= 0x00002003;
	static const ETYPE E_SHM_DETACH						= 0x00002004;

	//共享内存类错误
	static const ETYPE E_FILE							= 0x00003000;
	static const ETYPE E_FILE_ENV						= 0x00003001;

	//内存操作类错误
	static const ETYPE E_MEMORY							= 0x00004000;
	static const ETYPE E_MEMORY_LACK					= 0x00004001;

	//TUXEDO操作类错误
	static const ETYPE E_TUX							= 0x00005000;
	static const ETYPE E_TUX_FGET						= 0x00005001;
	static const ETYPE E_TUX_FADD						= 0x00005002;

	//XML操作类错误
	static const ETYPE E_XML							= 0x00006000;
	static const ETYPE E_XML_INIT						= 0x00006001;
	static const ETYPE E_XML_PARSE						= 0x00006002;
	static const ETYPE E_XML_TAG_ROOT					= 0x00006003;
	static const ETYPE E_XML_ELTAG_INPARAMS				= 0x00006004;
	static const ETYPE E_XML_ELTAG_PUBPARAMS			= 0x00006005;

	//参数类错误
	static const ETYPE E_PARAM							= 0x00007000;
	static const ETYPE E_PARAM_GET						= 0x00007001;

	////////////////////////////////////////////////////////
	//    以下为业务级别的错误
	////////////////////////////////////////////////////////

	//业务权限类错误
	static const ETYPE EB_RIGHT							= 0x00100000;
	static const ETYPE EB_RIGHT_LOGIN					= 0x00100001;
	static const ETYPE EB_RIGHT_FUNC					= 0x00100002;
	static const ETYPE EB_RIGHT_FUNCNAME				= 0x00100003;

	//统一视图类错误
	static const ETYPE EB_UNIFY							= 0x00200000;
	static const ETYPE EB_UNIFY_RULE_TYPE				= 0x00200001;
	static const ETYPE EB_UNIFY_GET_FUNC_ADDR			= 0x00200002;
	static const ETYPE EB_UNIFY_UNSET_PARAM				= 0x00200003;
	static const ETYPE EB_UNIFY_NO_IN_PARAM				= 0x00200004;
	static const ETYPE EB_UNIFY_NOSUPPORT_INDATATYPE	= 0x00200005;
	static const ETYPE EB_UNIFY_NOSUPPORT_OUTDATATYPE	= 0x00200006;
	static const ETYPE EB_UNIFY_NOSUPPORT_OUTDATATYPE2	= 0x00200007;
	static const ETYPE EB_UNIFY_NO_FUNC_RULE			= 0x00200008;
	static const ETYPE EB_UNIFY_NO_RULE_INFO			= 0x00200009;
	static const ETYPE EB_UNIFY_NO_RULE_IN				= 0x00200010;
	static const ETYPE EB_UNIFY_NO_RULE_OUT				= 0x00200011;
	static const ETYPE EB_UNIFY_NO_MAIN_CONDITION		= 0x00200012;
	static const ETYPE EB_UNIFY_NOSUPPORT_DISPDATATYPE	= 0x00200013;
	static const ETYPE EB_UNIFY_NOSUPPORT_DISPLAYTYPE	= 0x00200014;
	static const ETYPE EB_UNIFY_ARRAY_STRUCT			= 0x00200015;
	static const ETYPE EB_UNIFY_SELECT_VALUE			= 0x00200016;
	static const ETYPE EB_UNIFY_SELECT_SQL				= 0x00200017;
	static const ETYPE EB_UNIFY_RULE_NO_DATA			= 0x00200018;
	static const ETYPE EB_UNIFY_RULE_MAX_DATA			= 0x00200019;

public:
	static ETYPE GetLastError();
	static const char* GetErrorMsg(ETYPE errNo);
private:
	static ETYPE m_errorNo;
	static const TErrorNoMsg errorNoMsg[MAX_ERROR_NUM];
};

#endif //__CLError_h__
