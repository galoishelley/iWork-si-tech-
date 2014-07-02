#include "llayerStruct.h"

ETYPE CLError::m_errorNo=0;
#ifndef WIN32
//最大错误数量
const ETYPE CLError::MAX_ERROR_NUM;

const ETYPE CLError::E_SUCCESS;
const ETYPE CLError::E_MAX_ERROR_NO;

//数据库类错误
const ETYPE CLError::E_DATABASE;
const ETYPE CLError::E_DB_SQL_NOCONNECT;
const ETYPE CLError::E_DB_SQL_PREPARE;
const ETYPE CLError::E_DB_SQL_DATA_TYPE;
const ETYPE CLError::E_DB_DISCONNFLAG;
const ETYPE CLError::E_DB_SQLALD_BIND;
const ETYPE CLError::E_DB_SQL_EXECUTE_ITERS;
const ETYPE CLError::E_DB_BIND_MAXIMUM;
const ETYPE CLError::E_DB_SELECT_MAXIMUM;
const ETYPE CLError::E_DB_SQL_EXECUTE;
const ETYPE CLError::E_DB_SQL_UNKNOWN;
const ETYPE CLError::E_DB_OCI_INIT_ERROR;
const ETYPE CLError::E_DB_OCI_INIT_ENV;
const ETYPE CLError::E_DB_OCI_HDL_ALLOC_SVC;
const ETYPE CLError::E_DB_OCI_HDL_ALLOC_ERR;
const ETYPE CLError::E_DB_OCI_HDL_ALLOC_SRV;
const ETYPE CLError::E_DB_OCI_HDL_ALLOC_AUTH;
const ETYPE CLError::E_DB_OCI_HDL_ALLOC_STMT;
const ETYPE CLError::E_DB_OCI_SRV_ATTACH;
const ETYPE CLError::E_DB_OCI_ATTR_GET;
const ETYPE CLError::E_DB_OCI_ATTR_SET;
const ETYPE CLError::E_DB_OCI_LOG_ON;
const ETYPE CLError::E_DB_COMMIT;
const ETYPE CLError::E_DB_ROLLBACK;
const ETYPE CLError::E_DB_SQL_FETCH;
const ETYPE CLError::E_DB_SQL_FETCH_NODATA;
const ETYPE CLError::E_DB_GET_ROW_COUNT;
const ETYPE CLError::E_DB_GET_FETCHED_COUNT;
const ETYPE CLError::E_DB_SQL_DEFINE;
const ETYPE CLError::E_DB_GET_COL_ATTR;
const ETYPE CLError::E_DB_GET_LABEL;

//共享内存类错误
const ETYPE CLError::E_SHM;
const ETYPE CLError::E_SHM_GETKEY;
const ETYPE CLError::E_SHM_GET_ID;
const ETYPE CLError::E_SHM_ASSOCIATED;
const ETYPE CLError::E_SHM_DETACH;

//共享内存类错误
const ETYPE CLError::E_FILE;
const ETYPE CLError::E_FILE_ENV;

//内存操作类错误
const ETYPE CLError::E_MEMORY;
const ETYPE CLError::E_MEMORY_LACK;

//TUXEDO操作类错误
const ETYPE CLError::E_TUX;
const ETYPE CLError::E_TUX_FGET;
const ETYPE CLError::E_TUX_FADD;

//XML操作类错误
const ETYPE CLError::E_XML;
const ETYPE CLError::E_XML_INIT;
const ETYPE CLError::E_XML_PARSE;
const ETYPE CLError::E_XML_TAG_ROOT;
const ETYPE CLError::E_XML_ELTAG_INPARAMS;
const ETYPE CLError::E_XML_ELTAG_PUBPARAMS;

//参数类错误
const ETYPE CLError::E_PARAM;
const ETYPE CLError::E_PARAM_GET;

////////////////////////////////////////////////////////////
//    以下为业务级别的错误
///////////////////////////////////////////////////////////

//业务权限类错误
const ETYPE CLError::EB_RIGHT;
const ETYPE CLError::EB_RIGHT_LOGIN;
const ETYPE CLError::EB_RIGHT_FUNC;
const ETYPE CLError::EB_RIGHT_FUNCNAME;

//统一视图类错误
const ETYPE CLError::EB_UNIFY;
const ETYPE CLError::EB_UNIFY_RULE_TYPE;
const ETYPE CLError::EB_UNIFY_GET_FUNC_ADDR;
const ETYPE CLError::EB_UNIFY_UNSET_PARAM;
const ETYPE CLError::EB_UNIFY_NO_IN_PARAM;
const ETYPE CLError::EB_UNIFY_NOSUPPORT_INDATATYPE;
const ETYPE CLError::EB_UNIFY_NOSUPPORT_OUTDATATYPE;
const ETYPE CLError::EB_UNIFY_NOSUPPORT_OUTDATATYPE2;
const ETYPE CLError::EB_UNIFY_NO_FUNC_RULE;
const ETYPE CLError::EB_UNIFY_NO_RULE_INFO;
const ETYPE CLError::EB_UNIFY_NO_RULE_IN;
const ETYPE CLError::EB_UNIFY_NO_RULE_OUT;
const ETYPE CLError::EB_UNIFY_NO_MAIN_CONDITION;
const ETYPE CLError::EB_UNIFY_NOSUPPORT_DISPDATATYPE;
const ETYPE CLError::EB_UNIFY_NOSUPPORT_DISPLAYTYPE;
const ETYPE CLError::EB_UNIFY_ARRAY_STRUCT;
const ETYPE CLError::EB_UNIFY_SELECT_VALUE;
const ETYPE CLError::EB_UNIFY_SELECT_SQL;
const ETYPE CLError::EB_UNIFY_RULE_NO_DATA;
const ETYPE CLError::EB_UNIFY_RULE_MAX_DATA;

#endif

const TErrorNoMsg CLError::errorNoMsg[MAX_ERROR_NUM]=
{
	{E_SUCCESS,					"正确"},
	{E_MAX_ERROR_NO,			"最大错误号"},

	{E_DATABASE,				"数据库类错误"},
	{E_DB_SQL_NOCONNECT,		"数据库没有连接"},
	{E_DB_SQL_PREPARE,			"预编译SQL语句错误"},
	{E_DB_SQL_DATA_TYPE,		"错误绑定的数据类型, 参考EDbDataType"},
	{E_DB_DISCONNFLAG,			"数据库断开数据库连接参数错误"},
	{E_DB_SQLALD_BIND,			"分配绑定变量内存失败"},
	{E_DB_SQL_EXECUTE_ITERS,	"执行的迭代次数不对"},
	{E_DB_BIND_MAXIMUM,			"绑定变量太多，超过限制"},
	{E_DB_SELECT_MAXIMUM,		"选择的列数太多，超过限制"},
	{E_DB_SQL_EXECUTE,			"执行SQL语句错误"},
	{E_DB_SQL_UNKNOWN,			"不支持的SQL语句"},
	{E_DB_OCI_INIT_ERROR,		"ORACLE 初始化失败"},
	{E_DB_OCI_INIT_ENV,			"ORACLE 环境初始化失败"},
	{E_DB_OCI_HDL_ALLOC_SVC,	"ORACLE 初始化服务句柄失败"},
	{E_DB_OCI_HDL_ALLOC_ERR,	"ORACLE 初始化错误句柄失败"},
	{E_DB_OCI_HDL_ALLOC_SRV,	"ORACLE 初始化服务器句柄失败"},
	{E_DB_OCI_HDL_ALLOC_AUTH,	"ORACLE 初始化验证句柄失败"},
	{E_DB_OCI_HDL_ALLOC_STMT,	"ORACLE 初始化语句句柄失败"},
	{E_DB_OCI_SRV_ATTACH,		"FAILED: OCIServerAttach()"},
	{E_DB_OCI_ATTR_GET,			"FAILED: OCIAttrGet() server attribute"},
	{E_DB_OCI_ATTR_SET,			"FAILED: OCIAttrSet() server attribute"},
	{E_DB_OCI_LOG_ON,			"登陆失败"},
	{E_DB_COMMIT,				"事务提交失败"},
	{E_DB_ROLLBACK,				"事务回滚失败"},
	{E_DB_SQL_FETCH,			"取出游标的数据出错"},
	{E_DB_SQL_FETCH_NODATA,		"游标中无数据"},
	{E_DB_GET_ROW_COUNT,		"得到游标中的记录数出错"},
	{E_DB_GET_FETCHED_COUNT,	"得到本次返回的记录数出错"},
	{E_DB_SQL_DEFINE,			"定义输出绑定变量出错"},
	{E_DB_GET_COL_ATTR,			"得到SELECT的列属性出错"},
	{E_DB_GET_LABEL,			"得到数据库标签出错"},

	{E_SHM,						"共享内存类错误"},
	{E_SHM_GETKEY,				"得到共享内存键值错误"},
	{E_SHM_GET_ID,				"得到共享内存标识错误"},
	{E_SHM_DETACH,				"断开共享内存错误"},

	{E_FILE,					"文件类错误"},
	{E_FILE_ENV,				"取环境变量错误"},

	{E_MEMORY,					"内存错误"},
	{E_MEMORY_LACK,				"内存不足"},

	{E_TUX,						"TUXEDO错误"},
	{E_TUX_FGET,				"Fget读取参数失败"},
	{E_TUX_FADD,				"Fadd传出参数失败"},

	{E_XML,						"XML操作错误"},
	{E_XML_INIT,				"初始化XML环境失败"},
	{E_XML_PARSE,				"解析XML文件错误"},
	{E_XML_TAG_ROOT,			"得到XML根标签错误"},
	{E_XML_ELTAG_INPARAMS,		"得到XML XML_ELTAG_INPARAMS标签错误"},
	{E_XML_ELTAG_PUBPARAMS,		"得到XML XML_ELTAG_PUBPARAMS标签错误"},

	{E_PARAM,					"参数类操作错误"},
	{E_PARAM_GET,				"读取参数错误"},

	////////////////////////////////////////////////////////////
	//    以下为业务级别的错误
	///////////////////////////////////////////////////////////

	//业务权限类错误
	{EB_RIGHT,					"业务权限类错误"},
	{EB_RIGHT_LOGIN,			"输入工号或密码错误"},
	{EB_RIGHT_FUNC,				"无此功能权限"},
	{EB_RIGHT_FUNCNAME,			"无此功能代码"},

	//统一视图类错误
	{EB_UNIFY,						"统一视图类错误"},
	{EB_UNIFY_RULE_TYPE,			"不支持的规则类型[%d]！"},
	{EB_UNIFY_GET_FUNC_ADDR,		"从动态链接库中读取函数[%s]地址出错"},
	{EB_UNIFY_UNSET_PARAM,			"函数[%s]返回的参数位置[%d]，在代码表没有设置！"},
	{EB_UNIFY_NO_IN_PARAM,			"函数[%s]无输入参数，也没有默认值，参数位置[%d]！"},
	{EB_UNIFY_NOSUPPORT_INDATATYPE,	"不支持的输入函数[%s]数据类型，参数位置[%d]！"},
	{EB_UNIFY_NOSUPPORT_OUTDATATYPE,"不支持的输出函数[%s]数据类型，参数位置[%d]！"},
	{EB_UNIFY_NOSUPPORT_OUTDATATYPE2,"函数[%s]嵌套超过[%d]级！"},
	{EB_UNIFY_NO_FUNC_RULE,			"功能操作[%s]没有任何规则配置！"},
	{EB_UNIFY_NO_RULE_INFO,			"规则[%d]在规则信息表中没有配置信息！"},
	{EB_UNIFY_NO_RULE_IN,			"功能操作[%s]在规则输入表中没有配置规则[%d]！"},
	{EB_UNIFY_NO_RULE_OUT,			"功能操作[%s]在规则输出表中没有配置规则[%d]！"},
	{EB_UNIFY_NO_MAIN_CONDITION,	"功能操作[%s]在功能规则[%d]输入表中，没有输入主查询条件！"},
	{EB_UNIFY_NOSUPPORT_DISPDATATYPE,"不支持的显示数据类型"},
	{EB_UNIFY_NOSUPPORT_DISPLAYTYPE,"不支持的显示控件类型"},
	{EB_UNIFY_ARRAY_STRUCT,			"数组的元素只能是单值型的，函数[%s]参数位置[%d]。"},
	{EB_UNIFY_SELECT_VALUE,			"功能操作[%s], 规则代码[%d], 多选值规则代码配置错误，格式：记录之间用“,”号分开，记录内用“~”分开。"},
	{EB_UNIFY_SELECT_SQL,			"提取规则代码[%d]的SQL语句错误！"},
	{EB_UNIFY_RULE_NO_DATA,		"查询无符合条件的数据！函数[%s]！"},
	{EB_UNIFY_RULE_MAX_DATA,		"函数[%s]返回的数据量超过最大限制[%d]，只显示最大限制的数量！"}
};

ETYPE CLError::GetLastError() 
{
	return m_errorNo;
}

const char*  CLError::GetErrorMsg(ETYPE errNo) 
{
	register int i;
	for (i = 0; i < MAX_ERROR_NUM; i ++)
	{
		if (errorNoMsg[i].errorNo == errNo)
		{
			return errorNoMsg[i].errorMsg;
		}
	}
	return NULL;
}
