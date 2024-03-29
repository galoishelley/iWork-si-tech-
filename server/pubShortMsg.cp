#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#ifdef __cplusplus
}
#endif

#include "pubLog.h"
#include "pubShortMsg0.h"


EXEC SQL INCLUDE SQLCA;

/* the Lua interpreter */
static lua_State* L;
static char SysLabel[100];

/**短信应用脚本**/
#define SCRIPT_STRING \
"function Initial() \
	local parameters = {} \
	smparams = {} \
	smparams.data = parameters \
end \
function SetParameter(model_id, key, value) \
	if key == nil then \
		return \
	end \
	 \
	value = (value or '') \
	 \
	local params = smparams.data \
 \
	params[key] = value \
end \
function GetParameter(model_id,key) \
	if key == nil then \
		return 0 \
	end \
 \
	local params = smparams.data \
	if params[key] then \
		return 0,params[key] \
	else \
		return 1,'' \
	end \
 \
end \
function CheckParameter(model_id, key) \
	if key == nil then \
		return 0 \
	end \
	 \
	local params = smparams.data \
 \
	if params[key] then \
		return 0 \
	else \
		return 1 \
	end \
end \
function GetCurDate() \
	return os.date('%Y年%m月%d日') \
end \
function GetCurDateTime() \
	return os.date('%Y年%m月%d日%H时%M分%S秒') \
end "
/**短信应用脚本结束**/

/************************************************************
 *@ 函数名称：InitShortMsgLib
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：初始化短信组装环境
 *@
 *@ 输入参数：
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int InitShortMsgLib()
{
	int ret;
	char vScriptPath[200];
	Sinitn(vScriptPath);

#ifdef LOADSCRIPT_FROM_FILE
	ret = spublicGetCfgDir(SCRIPTFILENAME,vScriptPath);
	
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"-1","获取脚本文件全路径失败,ret = [%d]",ret);
		return -1;
	}
#endif

	memset(SysLabel,0,sizeof(SysLabel));
	if(strlen(getenv(SYSLABELENVNAME)) != 0)
	{
		strcpy(SysLabel,getenv(SYSLABELENVNAME));
	}

	/* initialize Lua */
	L = lua_open();
	/* load Lua base libraries */
	luaL_openlibs(L);


	/* load the script */
#ifdef LOADSCRIPT_FROM_FILE
	ret = luaL_dofile(L, vScriptPath);
#else
	ret = luaL_dostring(L, SCRIPT_STRING);
#endif

	return ret;
}

/************************************************************
 *@ 函数名称：InitShortMsgLabelLib
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：初始化短信组装环境
 *@
 *@ 输入参数：
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员： chendh 
 *@        修改日期： 2012.8.2
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：169 170 主机同时承载CRM  BOSS服务,为解决BOSS手机支付业务下发短信问题,增加InitShortMsgLabelLib 通过参数区分,sysname=CRM/BOSS;
 ************************************************************/
int InitShortMsgLabelLib(const char *sysname)
{
	int ret;
	char vScriptPath[200];
	Sinitn(vScriptPath);

#ifdef LOADSCRIPT_FROM_FILE
	ret = spublicGetCfgDir(SCRIPTFILENAME,vScriptPath);
	
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"-1","获取脚本文件全路径失败,ret = [%d]",ret);
		return -1;
	}
#endif
	Trim(sysname);
	memset(SysLabel,0,sizeof(SysLabel));
	if(strcmp(sysname,"BOSS")==0)
	{
		if(strlen(getenv(B_SYSLABELENVNAME)) != 0)
		{
			strcpy(SysLabel,getenv(B_SYSLABELENVNAME));
		}	
	}else{
		if(strlen(getenv(C_SYSLABELENVNAME)) != 0)
		{
			strcpy(SysLabel,getenv(C_SYSLABELENVNAME));
		}		
	}

	/* initialize Lua */
	L = lua_open();
	/* load Lua base libraries */
	luaL_openlibs(L);


	/* load the script */
#ifdef LOADSCRIPT_FROM_FILE
	ret = luaL_dofile(L, vScriptPath);
#else
	ret = luaL_dostring(L, SCRIPT_STRING);
#endif

	return ret;
}


/************************************************************
 *@ 函数名称：DestroyShortMsgLib
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：销毁短信组装环境
 *@
 *@ 输入参数：
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int DestroyShortMsgLib()
{
	/* cleanup Lua */
	if(L != NULL)
	{
		lua_close(L);
		L = NULL;
	}
	return 0;
}

/************************************************************
 *@ 函数名称：GetShortMsgLibInterpreter
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：获取短信组装环境指针
 *@
 *@ 输入参数：
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        指针型
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
void* GetShortMsgLibInterpreter()
{
	return L;
}

/************************************************************
 *@ 函数名称：PrepareNewShortMsg
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：准备组装的新的短信
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int PrepareNewShortMsg(int iModelId)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vDefaultParam[40+1];
		char vDefaultValue[40+1];
		char vSqlStr[200+1];
	EXEC SQL END DECLARE SECTION;
	int ret;
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}
	/* the function name */
	lua_getglobal(L, "Initial");
	/* call the function */
	ret = lua_pcall(L, 0, 0, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"ERR","初始化模板参数失败,返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	Sinitn(vSqlStr);

	sprintf(vSqlStr,"Select smm_param_code,nvl(param_default_value,' ') "
					"From SSMMPARAMLIST a "
					"where bitand(a.param_flag,2) = 2 "
					"and sm_model_id = :v1");
	EXEC SQL PREPARE PRE_DEFAULT_PARAM From :vSqlStr;
	EXEC SQL DECLARE CUR_DEFAULT_PARAM cursor For PRE_DEFAULT_PARAM;
	EXEC SQL OPEN CUR_DEFAULT_PARAM using :iModelId;

	if (SQLCODE !=0)
	{
		PUBLOG_DBERR(__func__,"1","打开CUR_DEFUALT_PARAM游标失败");
		return 1;
	}

	for(;;)
	{
		Sinitn(vDefaultParam);
		Sinitn(vDefaultValue);
		EXEC SQL Fetch CUR_DEFAULT_PARAM
			Into :vDefaultParam,:vDefaultValue;
		if(SQLCODE == 1403)
		{
			break;
		}
		else if(SQLCODE != 0)
		{
			EXEC SQL CLOSE CUR_DEFAULT_PARAM;
			PUBLOG_DBERR(__func__,"2","获取游标CUR_DEFAULT_PARAM失败");
			return 2;
		}
		Trim(vDefaultParam);
		Trim(vDefaultValue);

		ret = SetShortMsgParameter(iModelId,vDefaultParam, vDefaultValue);
		if(ret != 0)
		{
			EXEC SQL CLOSE CUR_DEFAULT_PARAM;
			PUBLOG_APPERR(__func__,"3","设置模板默认值[%s] = [%s]失败,ret = %d!",vDefaultParam, vDefaultValue, ret);
			return 3;
		}
	}
	EXEC SQL CLOSE CUR_DEFAULT_PARAM;

	return 0;
}

/************************************************************
 *@ 函数名称：SetShortMsgParameter
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：设置短信模板相关参数
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iKey             模板参数KEY
 *@        iValue           模板参数值
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int SetShortMsgParameter(int iModelId,const char *iKey, const char* iValue)
{
	int ret;
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}
	/* the function name */
	lua_getglobal(L, "SetParameter");

	/* the first argument */
	lua_pushnumber(L, iModelId);
	/* the second argument */
	lua_pushstring(L, iKey);
	/* the third argument */
	lua_pushstring(L, iValue);
	/* call the function with model_id,key,value parameter*/
	ret = lua_pcall(L, 3, 0, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"ERR","设置短信模板参数失败，返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	return 0;
}

/************************************************************
 *@ 函数名称：SetShortMsgNumParameter
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：设置短信模板相关参数
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iKey             模板参数KEY
 *@        iValue           模板参数值
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int SetShortMsgNumParameter(int iModelId,const char *iKey, long iValue)
{
	int ret;
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}
	/* the function name */
	lua_getglobal(L, "SetParameter");

	/* the first argument */
	lua_pushnumber(L, iModelId);
	/* the second argument */
	lua_pushstring(L, iKey);
	/* the third argument */
	lua_pushnumber(L, iValue);
	/* call the function with model_id,key,value parameter*/
	ret = lua_pcall(L, 3, 0, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"ERR","设置短信模板参数失败，返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	return 0;
}

/************************************************************
 *@ 函数名称：CheckMandatoryParameters
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：验证短信模板必须的参数是否都存在
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int CheckMandatoryParameters(int iModelId)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vDefaultParam[40+1];
		char vSqlStr[200+1];
	EXEC SQL END DECLARE SECTION;
	int  ret;

#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"","Checking Parameters For model[%d]!",iModelId);
#endif

	Sinitn(vSqlStr);

	sprintf(vSqlStr,"Select smm_param_code "
					"From SSMMPARAMLIST a "
					"where bitand(a.param_flag,1) = 1 "
					"and sm_model_id = :v1");
	EXEC SQL PREPARE PRE_MANDATORY_PARAM From :vSqlStr;
	EXEC SQL DECLARE CUR_MANDATORY_PARAM cursor For PRE_MANDATORY_PARAM;
	EXEC SQL OPEN CUR_MANDATORY_PARAM using :iModelId;

	if (SQLCODE !=0)
	{
		PUBLOG_DBERR(__func__,"1","打开CUR_MANDATORY_PARAM游标失败");
		return 1;
	}

	for(;;)
	{
		Sinitn(vDefaultParam);
		EXEC SQL Fetch CUR_MANDATORY_PARAM
			Into :vDefaultParam;
		if(SQLCODE == 1403)
		{
			break;
		}
		else if(SQLCODE != 0)
		{
			EXEC SQL CLOSE CUR_MANDATORY_PARAM;
			PUBLOG_DBERR(__func__,"2","获取游标CUR_MANDATORY_PARAM失败");
			return 2;
		}
		Trim(vDefaultParam);

		ret = CheckMandatoryParameter(iModelId,vDefaultParam);
		if(ret != 0)
		{
			EXEC SQL CLOSE CUR_MANDATORY_PARAM;
			PUBLOG_APPERR(__func__,"3","模板参数[%s]必须存在,不可为空!",vDefaultParam);
			return 3;
		}
	}
	EXEC SQL CLOSE CUR_MANDATORY_PARAM;

	return 0;
}

/************************************************************
 *@ 函数名称：GetShortMsgParameter
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：取得模板参数当前值
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iKey             模板参数KEY
 *@
 *@ 返回参数：
 *@        iValue           模板参数值
 *@
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int GetShortMsgParameter(int iModelId,const char *iKey,char *iValue)
{
	int ret;
	int exist_flag;
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}

	/* the function name */
#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"","Checking Parameter [%s] For model[%d]!", iKey, iModelId);
#endif

	lua_getglobal(L, "GetParameter");

	/* the first argument */
	lua_pushnumber(L, iModelId);
	/* the second argument */
	lua_pushstring(L, iKey);
	/* call the function with model_id,key parameter, return the value*/
	ret = lua_pcall(L, 2, 2, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"ERR","获取短信模板参数失败，返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	exist_flag = 0;
	exist_flag = (int)lua_tonumber(L, -2);
	
	if(exist_flag == 0)
	{
		strcpy(iValue,lua_tostring(L, -1));
	}
	lua_pop(L, 2);

	return exist_flag;
}

/************************************************************
 *@ 函数名称：CheckMandatoryParameter
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：验证单个模板参数是否存在
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iKey             模板参数KEY
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int CheckMandatoryParameter(int iModelId,const char *iKey)
{
	int ret;
	int exist_flag;
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}

	/* the function name */
#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"","Checking Parameter [%s] For model[%d]!", iKey, iModelId);
#endif

	lua_getglobal(L, "CheckParameter");

	/* the first argument */
	lua_pushnumber(L, iModelId);
	/* the second argument */
	lua_pushstring(L, iKey);
	/* call the function with model_id,key parameter, return the key is nil or not*/
	ret = lua_pcall(L, 2, 1, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"ERR","验证短信模板参数失败，返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	exist_flag = 0;
	exist_flag = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return exist_flag;
}

/************************************************************
 *@ 函数名称：CompileShortMsgModel
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：编译短信模板
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        model_pattern    短信模版内容
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int CompileShortMsgModel(int iModelId, const char *model_pattern)
{
	int ret;
	char func_buf[1024];
	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}

	Sinitn(func_buf);

	sprintf(func_buf,
	"function GenerateSM() \n\
	local params = smparams.data \n"
#ifdef _DEBUG_
	"for k,v in pairs(params) do \n\
		print(\"[\"..k..\"]=[\"..v..\"]\") \n\
	end \n\
	print(\"参数集合输出完成\") \n"
#endif
	"return %s \n\
	end",model_pattern);

#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"DEBUG","script function [%s]",func_buf);
#endif

	ret = luaL_dostring(L,func_buf);
	return ret;
}

/************************************************************
 *@ 函数名称：GenerateShortMsg
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：组装短信实例
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iRecvPhoneNo     接收手机号码
 *@        iLoginNo         操作工号
 *@        iOpCode          操作代码
 *@        iLoginAccept     操作流水
 *@        iOpTime          操作时间，格式'YYYYMMDD HH24:MI:SS'
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int GenerateShortMsg(int iModelId, const char *iRecvPhoneNo, const char *iLoginNo,
                     const char *iOpCode, long iLoginAccept, const char *iOpTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vModelPattern[1024+1];
		char vSendTime[17+1];
		char vUpwardAccept[14+1];
	EXEC SQL END DECLARE SECTION;
	int ret;

	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}

	Sinitn(vModelPattern);
	Sinitn(vSendTime);
	Sinitn(vUpwardAccept);

	EXEC SQL Select replace(model_pattern,'||','..')
		Into :vModelPattern
		From sSMModel a
		Where sm_model_id = :iModelId;

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"1","查询短信模板失败!");
		ret = 1;
		goto END_FUNC;
	}

	Trim(vModelPattern);

	if((ret = CompileShortMsgModel(iModelId,vModelPattern)) != 0)
	{
		PUBLOG_APPERR(__func__,"2","编译短信模板失败,ret = %d!",ret);
		ret = 2;
		goto END_FUNC;
	}

	if((ret = CheckMandatoryParameters(iModelId)) != 0)
	{
		PUBLOG_APPERR(__func__,"3","检查短信模板参数失败,ret = %d!",ret);
		ret = 3;
		goto END_FUNC;
	}

	/* the function name */
	lua_getglobal(L, "GenerateSM");

	/* call the function return 1 string*/
	ret = lua_pcall(L, 0, 1, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"4","调用函数[GenerateSM]出错,返回值=[%d],错误信息=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		ret = 4;
		goto END_FUNC;
	}

	/* get the shortMsg */
	char *shortMsg;
	int shortMsgLen;
	shortMsgLen = strlen(lua_tostring(L, -1)) + 1;
	shortMsg = (char *)malloc(shortMsgLen);
	memset(shortMsg, 0, shortMsgLen);
	sprintf(shortMsg,"%s",lua_tostring(L, -1));
	lua_pop(L, 1);

	PUBLOG_DEBUG(__func__,"DEBUG","向手机[%s]发送短信，业务流水[%ld]，业务代码[%s]!",iRecvPhoneNo,iLoginAccept,iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","短信内容为：[%s]",shortMsg);
	
	/*查询发送短信时间参数*/
	GetShortMsgParameter(iModelId,SHORTMSGSENDTIME,vSendTime);
	GetShortMsgParameter(iModelId,UPWARDACCEPT,vUpwardAccept);
	
	ret = DispatchShortMsg(iModelId, iRecvPhoneNo, iLoginNo, iOpCode, iOpTime, iLoginAccept, vUpwardAccept, shortMsg, vSendTime);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"5","查询短信到短信通道失败!ret=[%d]",ret);
		free(shortMsg);
		goto END_FUNC;
	}

	free(shortMsg);

	ret = 0;

END_FUNC:

#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"DEBUG","before collect mem size:%d!",lua_getgccount(L));
#endif

	/**回收内存垃圾**/
	lua_gc(L,LUA_GCCOLLECT,0);

#ifdef _DEBUG_
	PUBLOG_DEBUG(__func__,"DEBUG","after collect mem size:%d!",lua_getgccount(L));
#endif
	return ret;
}

/************************************************************
 *@ 函数名称：DispatchShortMsg
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：根据短信模板通道配置及个性化参数，将短信内容插入到短信接口表。
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iRecvPhoneNo     接收手机号码
 *@        iLoginNo         操作工号
 *@        iOpCode          操作代码
 *@        iOpTime          操作时间，格式'YYYYMMDD HH24:MI:SS'
 *@        iLoginAccept     操作流水
 *@        iUpwardAccept    上行业务标识流水
 *@        iShortMsg        短信内容
 *@        iSendTime        自定义发送时间
 *@
 *@ 返回参数：
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int DispatchShortMsg(int iModelId, const char *iRecvPhoneNo, 
                     const char *iLoginNo, const char *iOpCode, 
                     const char *iOpTime, long iLoginAccept, 
                     const char *iUpwardAccept,
                     char *iShortMsg, char *iSendTime)
{
	int ret;
	char *p;
	EXEC SQL BEGIN DECLARE SECTION;
		char vChnTableName[70+1];  /**短信通道信息表**/
		char vDefaultSmPort[10+1];
		char vSmPort[30+1];
		char vDefaultSmType[5+1];
		char vSmType[5+1];
		int  vMsgLevel;
		char vSeqName[70+1];
		char vFoldPolicy[1+1];
		char vExpPolicy[1+1];
		long vDuration = 0;
		char vReqUpward[1+1];
		char vSendTimePolicy[1+1];
		long vSendTimeOffset = 0;
		char vShortMsgTmp[1024+1];
		char vShortMsg[1000+1];
		char vShortMsgAdd[1000+1];
		int  vOrderCode;
		char vSendTime[17+1];
		char vExpTime[17+1];
		char vWorkGroup[20+1];
		int  vMsgByteLen;  /*短信内容的字节长度*/
		int  vMsgCharLen;  /*短信内容的字符长度*/
		int  vUrlLen;
		int  vMsgLeftLen;
		int  vNormMsgMaxLen;
		int  vPos;
		int  vStartPos;
	EXEC SQL END DECLARE SECTION;

	Sinitn(vChnTableName);
	Sinitn(vDefaultSmPort);
	Sinitn(vSmPort);
	Sinitn(vDefaultSmType);
	Sinitn(vSmType);
	Sinitn(vSeqName);
	Sinitn(vExpPolicy);
	Sinitn(vFoldPolicy);
	Sinitn(vReqUpward);
	Sinitn(vSendTimePolicy);
	Sinitn(vShortMsg);
	Sinitn(vShortMsgAdd);
	Sinitn(vSendTime);
	Sinitn(vExpTime);
	Sinitn(vWorkGroup);
	vMsgLevel = 0;
	vDuration = 0;
	vOrderCode = 0;
	vSendTimeOffset = 0;

	EXEC SQL Select default_sm_port, default_sm_type, msg_level,
			seq_name, fold_policy, exp_policy, duration,
			req_upward, send_time_policy, send_time_offset,
			work_group
		Into :vDefaultSmPort, :vDefaultSmType, :vMsgLevel,
			:vSeqName, :vFoldPolicy, :vExpPolicy, :vDuration,
			:vReqUpward, :vSendTimePolicy, :vSendTimeOffset,
			:vWorkGroup
		From sSMModel a
		Where sm_model_id = :iModelId;

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","查询短信模板信息失败");
		return -1;
	}
	
	Trim(vWorkGroup);
	if( (ret = QryShortMsgChannel(iOpCode, vWorkGroup, vChnTableName)) != 0)
	{
		PUBLOG_APPERR(__func__,"-2","获取短信通道失败!");
		return -2;
	}
	
	PUBLOG_DEBUG(__func__,"DEBUG","BUSI_CODE[%s]获取短信通道表[%s]",iOpCode, vChnTableName);

	Trim(vDefaultSmPort);
	Trim(vDefaultSmType);
	Trim(vSeqName);
	Trim(vFoldPolicy);
	Trim(vExpPolicy);
	Trim(vReqUpward);
	Trim(vSendTimePolicy);
	
	/*如果是需要上行短信，在端口号后拼装流水号*/
	strcpy(vSmPort, vDefaultSmPort);
	if(vReqUpward[0] == '1' && strlen(iUpwardAccept) > 0)
	{
		sprintf(vSmPort,"%s%s",vSmPort, iUpwardAccept);
	}
	
	/*计算短信发送时间，优先使用外部传入发送时间，未传入使用模板发送时间规则配置*/
	PUBLOG_DEBUG(__func__,"DEBUG","OPTIME=[%s]\n",iOpTime);
	if(strlen(iSendTime) == 0)
	{
		/*立即发送*/
		if(vSendTimePolicy[0] == '0')
		{
			strcpy(vSendTime, iOpTime);
		}
		else
		{
			if((ret = ComputePolicyTime(vSendTimePolicy[0], iOpTime, vSendTimeOffset,vSendTime)) != 0)
			{
				PUBLOG_APPERR(__func__,"-3","计算短信发送时间失败,ret=[%d]\n",ret);
				return -3;
			}
		}
	}
	else
	{
		strcpy(vSendTime,iSendTime);
	}

	/*计算短信超时时间*/
	if(vExpPolicy[0] == '0')
	{
		strcpy(vExpTime,SYS_DOOMSDAY);
	}
	else if(vExpPolicy[0] == '1')
	{
		/*根据发送时间与短信池中同类短信比较，删除同类短信*/
		ret = DeleteSameTypeMsg(iModelId,vChnTableName,
								iRecvPhoneNo, iOpCode, iSendTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__,"-17","删除同类短信失败,ret=[%d]\n",ret);
			return -17;
		}
	}
	else
	{
		ret = ComputePolicyTime(vExpPolicy[0], iOpTime, vDuration, vExpTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__,"-4","计算短信超时时间失败,ret=[%d]\n",ret);
			return -4;
		}
	}

	/*短信类型处理，超长短信折叠处理*/
	strcpy(vSmType, vDefaultSmType);
	
	vMsgByteLen = strlen(iShortMsg);
	vMsgCharLen = ChnStrLen(iShortMsg);
	
	/*普通短信长度限制70个字符*/
	#define NORM_SHORTMSG_LEN 70
	/*PUSH短信中URL长度限制，总长度(130) - PUSH_HEADER(20) - TEXT_HEADER(4) - TEXT_END(4) = 100*/
	#define PUSH_SHORTMSG_URL_LEN 100
	vNormMsgMaxLen = NORM_SHORTMSG_LEN;
	
	if(strcmp(vSmType,"push") == 0)  /**PUSH短信**/
	{
		/*短信长度超出短信范围*/
		if(vMsgByteLen > 1000)
		{
			PUBLOG_APPERR(__func__, "-5", "PUSH短信过长[%s]!",iShortMsg);
			return -5;
		}
		
		p = (char *)strchr(iShortMsg,(int)('|'));
		if ( p == NULL)
		{
			PUBLOG_APPERR(__func__, "-6", "PUSH短信中必须使用'|'分割信息内容与URL");
			return -6;
		}
		
		vUrlLen = strlen(p);
			
		/*短信中URL长度超出范围*/
		if(vUrlLen >= PUSH_SHORTMSG_URL_LEN)
		{
			PUBLOG_APPERR(__func__, "-7", "PUSH短信中URL过长[%s]!",p);
			return -7;
		}
		
		ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
							vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
							iOpCode, iOpTime, iLoginAccept, iShortMsg, 
							NULL, 0, vSendTime, vExpTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__, "-8", "插入短信接口表失败,ret=[%d]\n",ret);
			return -8;
		}
	}
	else if(strcmp(vSmType,"long") == 0) /*长短信*/
	{
		vMsgLeftLen = vMsgByteLen;
		Sinitn(vShortMsgTmp);
		if(vMsgLeftLen > 1024) /*PROC输入缓冲区最大长度1024*/
		{
			strncpy(vShortMsgTmp,iShortMsg,1024);
		}
		else
		{
			strcpy(vShortMsgTmp,iShortMsg);
		}
		
		ChnStrSplit(vShortMsgTmp, 1000, vShortMsg, &vPos);
		
		Trim(vShortMsg);
		
		vMsgLeftLen = vMsgLeftLen - vPos;
		Sinitn(vShortMsgTmp);
		if(vMsgLeftLen > 1024)   /*大于1000剩余字符截断*/
		{
			strncpy(vShortMsgTmp,iShortMsg+vPos,1024);
		}
		else
		{
			strcpy(vShortMsgTmp,iShortMsg+vPos);
		}
		
		ChnStrSplit(vShortMsgTmp, 1000, vShortMsgAdd, &vPos);
		
		Trim(vShortMsgAdd);
		
		ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
						vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
						iOpCode, iOpTime, iLoginAccept, vShortMsg, 
						vShortMsgAdd, 0, vSendTime, vExpTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__, "-10", "插入短信接口表失败,ret=[%d]\n",ret);
			return -10;
		}
	}
	else /*普通短信*/
	{
		/*短信长度超出普通短信范围*/
		if(vMsgCharLen > vNormMsgMaxLen)
		{
			/*超长处理原则*/
			if(vFoldPolicy[0] == '0') /*截断*/
			{
				Sinitn(vShortMsgTmp);
				strncpy(vShortMsgTmp,iShortMsg,(vNormMsgMaxLen*2)+3);  /*多取三个字节，便于截断函数处理*/
				
				EXEC SQL Select nvl(substr(:vShortMsgTmp,1,70),' ') 
					Into :vShortMsg from dual;
				if(SQLCODE != 0)
				{
					PUBLOG_DBERR(__func__,"-11","截断短信内容失败!");
					return -11;
				}
				
				Trim(vShortMsg);

				ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
								iOpCode, iOpTime, iLoginAccept, vShortMsg, 
								NULL, 0, vSendTime, vExpTime);
				if(ret != 0)
				{
					PUBLOG_APPERR(__func__, "-12", "插入短信接口表失败,ret=[%d]\n",ret);
					return -12;
				}
			}
			else if(vFoldPolicy[0] == '1') /*变成多条普通短信发送*/
			{
				vMsgLeftLen = vMsgByteLen;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024) /*PROC输入缓冲区最大长度1024*/
				{
					strncpy(vShortMsgTmp,iShortMsg,1024);
				}
				else
				{
					strcpy(vShortMsgTmp,iShortMsg);
				}
				
				ChnStrSplit(vShortMsgTmp, 1000, vShortMsg, &vPos);
				
				Trim(vShortMsg);
				
				vMsgLeftLen = vMsgLeftLen - vPos;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024)   /*大于1024剩余字符截断*/
				{
					strncpy(vShortMsgTmp,iShortMsg+vPos,1024);
				}
				else
				{
					strcpy(vShortMsgTmp,iShortMsg+vPos);
				}
				
				ChnStrSplit(vShortMsgTmp, 1000, vShortMsgAdd, &vPos);
				
				Trim(vShortMsgAdd);
				
				ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo,
								iOpCode, iOpTime, iLoginAccept, vShortMsg, 
								vShortMsgAdd, 0, vSendTime, vExpTime);
				if(ret != 0)
				{
					PUBLOG_APPERR(__func__, "-13", "插入短信接口表失败,ret=[%d]\n",ret);
					return -13;
				}
			}
			else if(vFoldPolicy[0] == '2')  /*转换成长短信发送*/
			{
				/*改变短信类型为长短信类型*/
				strcpy(vSmType,"long");
				vMsgLeftLen = vMsgByteLen;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024) /*PROC输入缓冲区最大长度1024*/
				{
					strncpy(vShortMsgTmp,iShortMsg,1024);
				}
				else
				{
					strcpy(vShortMsgTmp,iShortMsg);
				}
				
				ChnStrSplit(vShortMsgTmp, 1000, vShortMsg, &vPos);
				
				Trim(vShortMsg);
				
				vMsgLeftLen = vMsgLeftLen - vPos;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024)   /*大于1024剩余字符截断*/
				{
					strncpy(vShortMsgTmp,iShortMsg+vPos,1024);
				}
				else
				{
					strcpy(vShortMsgTmp,iShortMsg+vPos);
				}
				
				ChnStrSplit(vShortMsgTmp, 1000, vShortMsgAdd, &vPos);
				
				Trim(vShortMsgAdd);
				
				ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo,
								iOpCode, iOpTime, iLoginAccept, vShortMsg, 
								vShortMsgAdd, 0, vSendTime, vExpTime);
				if(ret != 0)
				{
					PUBLOG_APPERR(__func__, "-14", "插入短信接口表失败,ret=[%d]\n",ret);
					return -14;
				}
			}
			else
			{
				PUBLOG_APPERR(__func__,"-14","超长普通短信处理原则配置错误，Policy=[%s]\n",vFoldPolicy);
				return -15;
			}
		}
		else /*发送普通短信*/
		{
			ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
								iOpCode, iOpTime, iLoginAccept, iShortMsg, 
								NULL, 0, vSendTime, vExpTime);
			if(ret != 0)
			{
				PUBLOG_APPERR(__func__, "-16", "插入短信接口表失败,ret=[%d]\n",ret);
				return -16;
			}
		}
	}

	return 0;
}

/************************************************************
 *@ 函数名称：ComputePolicyTime
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：根据时间偏移策略及偏移量计算未来时间
 *@
 *@ 输入参数：
 *@        policy           时间策略类型，'2':偏移到固定时间；'3':以小时为单位偏移；'4':以月为单位偏移
 *@        iOpTime          当前操作时间，格式'YYYYMMDD HH24:MI:SS'
 *@        offset           时间偏移量
 *@
 *@ 返回参数：
 *@        OutputTime       输出时间
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int ComputePolicyTime(char policy, char *iOpTime, long offset, char *OutputTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vTimeStr[17+1];
	EXEC SQL END DECLARE SECTION;
	int ret;
	Sinitn(vTimeStr);
	PUBLOG_DEBUG(__func__,"POLICY","以opTime=[%s],POLICY=[%c],offset=[%ld]计算时间",iOpTime,policy,offset);
	switch(policy)
	{
		/*固定时间*/
		case '2':
			EXEC SQL select to_char(to_date(:offset,'YYYYMMDDHH24MISS'),'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual;
			break;
		case '3':
			EXEC SQL select to_char(to_date(:iOpTime,'YYYYMMDD HH24:MI:SS') + (:offset/24), 'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual;
			break;
		case '4':
			EXEC SQL select to_char(add_months(to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:offset), 'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual;
			break;
		default:
			return -1;
	}
	
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-2","计算偏移量时间出错!");
		return -2;
	}
	strcpy(OutputTime,vTimeStr);
	
	return 0;
}

/************************************************************
 *@ 函数名称：QryShortMsgChannel
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：根据业务操作代码查询短信通道表名，未配置关系则随机选取通道
 *@
 *@ 输入参数：
 *@        iOpCode          业务操作代码
 *@
 *@ 返回参数：
 *@        oTableName       短信通道表名
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int QryShortMsgChannel(const char *iOpCode, char *iWorkGroup, char *oTableName)
{
	int ret;
	EXEC SQL BEGIN DECLARE SECTION;
		char vChnTableName[70+1];  /**短信通道信息表**/
		int vRandomRow = 0;
		int vCount = 0;
	EXEC SQL END DECLARE SECTION;

	PUBLOG_DEBUG(__func__,"DEBUG","BUSI_CODE = [%s]",iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","SYS_LABEL = [%s]",SysLabel);
	PUBLOG_DEBUG(__func__,"DEBUG","iWorkGroup= [%s]",iWorkGroup);

	EXEC SQL Select count(*)
		Into :vCount
		From SSMCHANNEL a, SBUSISMRELATION b
		Where a.channel_id = b.channel_id
			And a.belong_sys = :SysLabel
			AND b.busi_code = :iOpCode;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","查询短信通道个数失败");
		return -1;
	}
	
	if(vCount == 0)  /*没有指定专用通道*/
	{
		EXEC SQL select count (*)
			Into :vCount
			FROM ssmchannel
			Where belong_sys = :SysLabel
				And work_group = :iWorkGroup;
		if(SQLCODE != 0)
		{
			PUBLOG_DBERR(__func__,"-2","查询短信通道个数失败");
			return -2;
		}

		if(vCount <= 0)
		{
			PUBLOG_APPERR(__func__,"-3","无可用短信通道");
			return -3;
		}

		srand((unsigned)time(0) + getpid());
		vRandomRow = rand()%vCount + 1;

		PUBLOG_DEBUG(__func__,"DEBUG","Random Row = [%d]",vRandomRow);

		EXEC SQL SELECT channel_tab_name
			Into :vChnTableName
			From (SELECT ROWNUM rnm, a.* From ssmchannel a WHERE belong_sys = :SysLabel And work_group = :iWorkGroup) b
			WHERE b.rnm = :vRandomRow
				And belong_sys = :SysLabel
				And work_group = :iWorkGroup;
		if(SQLCODE == 0)
		{
			Trim(vChnTableName);
			strcpy(oTableName, vChnTableName);
			return 0;
		}
		else
		{
			PUBLOG_DBERR(__func__,"-4","查询短信通道表失败");
			return -4;
		}
	}
	else if(vCount == 1)  /*只有一个通道*/
	{
		Sinitn(vChnTableName);
		EXEC SQL Select a.channel_tab_name
			Into :vChnTableName
			From SSMCHANNEL a, SBUSISMRELATION b
			Where a.channel_id = b.channel_id
				And a.belong_sys = :SysLabel
				AND b.busi_code = :iOpCode;
		if(SQLCODE == 0)
		{
			Trim(vChnTableName);
			strcpy(oTableName, vChnTableName);
			return 0;
		}
		else
		{
			PUBLOG_DBERR(__func__,"-5","查询短信通道表失败");
			return -5;
		}
	}
	else /*指定了多个专用通道*/
	{
		srand((unsigned)time(0) + getpid());
		vRandomRow = rand()%vCount + 1;

		EXEC SQL SELECT channel_tab_name
			Into :vChnTableName
			From (Select ROWNUM rnm, a.*
				From ssmchannel a, sbusismrelation b
				Where a.channel_id = b.channel_id
					And b.busi_code = :iOpCode
					And a.belong_sys = :SysLabel
				) c
			Where c.rnm = :vRandomRow;
		if(SQLCODE == 0)
		{
			Trim(vChnTableName);
			strcpy(oTableName, vChnTableName);
			return 0;
		}
		else
		{
			PUBLOG_DBERR(__func__,"-6","查询短信通道表失败");
			return -6;
		}
	}
}

/************************************************************
 *@ 函数名称：DeleteSameTypeMsg
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：删除同类短信实例。删除条件：同一用户、同一操作代码、本次发送时间 >= 以前的短信发送时间
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iChnTableName    短信通道表名
 *@        iRecvPhoneNo     接收手机号码
 *@        iOpCode          操作代码
 *@        iSendTime        预计发送时间，格式'YYYYMMDD HH24:MI:SS'
 *@
 *@ 返回参数：
 *@
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int DeleteSameTypeMsg(const int iModelId,const char *iChnTableName,
                   const char *iRecvPhoneNo, const char *iOpCode, const char *iSendTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vSqlStr[511+1];
	EXEC SQL END DECLARE SECTION;
	Sinitn(vSqlStr);
	
	sprintf(vSqlStr,"Delete From %s "
					"Where sm_model_id = :v1 "
					" and phone_no = :v2 "
					" and busi_code = :v3 ",
					iChnTableName);
	EXEC SQL PREPARE Pre_SM_Del FROM :vSqlStr;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-3","删除同类短信失败!");
		return -3;
	}
	EXEC SQL EXECUTE Pre_SM_Del using :iModelId,:iRecvPhoneNo,:iSendTime;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		PUBLOG_DBERR(__func__,"-4","删除同类短信失败!");
		return -4;
	}
	
	return 0;
}

/************************************************************
 *@ 函数名称：InsertShortMsg
 *@ 编码日期：2011/03/20
 *@ 服务版本: Ver1.0
 *@ 编码人员：sunzx
 *@ 功能描述：插入短信到通道
 *@
 *@ 输入参数：
 *@        iModelId         模板ID
 *@        iChnTableName    短信通道表名
 *@        iSeqName         序列名称
 *@        iMsgLevel        短信优先级
 *@        iSmPort          短信下发端口
 *@        iSmType          短信类型
 *@        iRecvPhoneNo     接收手机号码
 *@        iLoginNo         操作工号
 *@        iOpCode          操作代码
 *@        iOpTime          操作时间，格式'YYYYMMDD HH24:MI:SS'
 *@        iLoginAccept     操作业务流水
 *@        iShortMsg        短消息内容
 *@        iShortMsgAdd     短消息追加内容
 *@        iOrderCode       短消息子序号
 *@        iSendTime        发送时间，格式'YYYYMMDD HH24:MI:SS'
 *@        iExpTime         短信失效时间，格式'YYYYMMDD HH24:MI:SS'
 *@
 *@ 返回参数：
 *@
 *@ 返回值：
 *@        整数型   0:成功  其他:失败
 *@
 *@ 修改记录:
 *@        修改人员：
 *@        修改日期：
 *@        修改前版本号：
 *@        修改后版本号:
 *@        修改原因：
 ************************************************************/
int InsertShortMsg(const int iModelId,
                   const char *iChnTableName,const char *iSeqName,
                   const int iMsgLevel, const char *iSmPort, const char *iSmType,
                   const char *iRecvPhoneNo, const char *iLoginNo, 
                   const char *iOpCode, const char *iOpTime, 
                   const long iLoginAccept, const char *iShortMsg, 
                   const char *iShortMsgAdd, const int iOrderCode, 
                   const char *iSendTime, const char *iExpTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vSqlStr[511+1];
	EXEC SQL END DECLARE SECTION;

	PUBLOG_DEBUG(__func__,"DEBUG","iOrderCode = [%d]\n",iOrderCode);
	PUBLOG_DEBUG(__func__,"DEBUG","iMsgLevel = [%d]\n",iMsgLevel);
	PUBLOG_DEBUG(__func__,"DEBUG","iLoginAccept = [%ld]\n",iLoginAccept);
	PUBLOG_DEBUG(__func__,"DEBUG","iSmPort = [%s]\n",iSmPort);
	PUBLOG_DEBUG(__func__,"DEBUG","iSmType = [%s]\n",iSmType);
	PUBLOG_DEBUG(__func__,"DEBUG","iRecvPhoneNo = [%s]\n",iRecvPhoneNo);
	PUBLOG_DEBUG(__func__,"DEBUG","iOpCode = [%s]\n",iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","iShortMsg = [%s]\n",iShortMsg);
	PUBLOG_DEBUG(__func__,"DEBUG","iShortMsgAdd = [%s]\n",iShortMsgAdd);
	PUBLOG_DEBUG(__func__,"DEBUG","iOpTime = [%s]\n",iOpTime);
	PUBLOG_DEBUG(__func__,"DEBUG","iSendTime = [%s]\n",iSendTime);
	PUBLOG_DEBUG(__func__,"DEBUG","iExpTime = [%s]\n",iExpTime);

	Sinitn(vSqlStr);
	sprintf(vSqlStr,"Insert Into %s(command_id, sm_model_id, login_accept, "
					"msg_level, gateway_port,sm_type, phone_no, msg, "
					"append_msg, order_code, login_no, busi_code, "
					"op_time, send_time, expire_time) "
					"Values"
					"(%s.nextval,:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
					"to_date(:v12,'YYYYMMDD HH24:MI:SS'),"
					"to_date(:v13,'YYYYMMDD HH24:MI:SS'),"
					"to_date(:v14,'YYYYMMDD HH24:MI:SS'))",
					iChnTableName,iSeqName);

	EXEC SQL PREPARE Pre_SM_Send FROM :vSqlStr;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","插入短信实例接口表失败!");
		return -1;
	}
	EXEC SQL EXECUTE Pre_SM_Send using :iModelId,:iLoginAccept,:iMsgLevel,
					:iSmPort,:iSmType,:iRecvPhoneNo,:iShortMsg,
					:iShortMsgAdd,:iOrderCode,:iLoginNo,:iOpCode,:iOpTime,
					:iSendTime,:iExpTime;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-2","插入短信实例接口表失败!");
		return -2;
	}
	return 0;
}

