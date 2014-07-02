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

/**����Ӧ�ýű�**/
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
	return os.date('%Y��%m��%d��') \
end \
function GetCurDateTime() \
	return os.date('%Y��%m��%d��%Hʱ%M��%S��') \
end "
/**����Ӧ�ýű�����**/

/************************************************************
 *@ �������ƣ�InitShortMsgLib
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������ʼ��������װ����
 *@
 *@ ���������
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_APPERR(__func__,"-1","��ȡ�ű��ļ�ȫ·��ʧ��,ret = [%d]",ret);
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
 *@ �������ƣ�InitShortMsgLabelLib
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������ʼ��������װ����
 *@
 *@ ���������
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա�� chendh 
 *@        �޸����ڣ� 2012.8.2
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��169 170 ����ͬʱ����CRM  BOSS����,Ϊ���BOSS�ֻ�֧��ҵ���·���������,����InitShortMsgLabelLib ͨ����������,sysname=CRM/BOSS;
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
		PUBLOG_APPERR(__func__,"-1","��ȡ�ű��ļ�ȫ·��ʧ��,ret = [%d]",ret);
		return -1;
	}
#endif
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
 *@ �������ƣ�DestroyShortMsgLib
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ �������������ٶ�����װ����
 *@
 *@ ���������
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
 *@ �������ƣ�GetShortMsgLibInterpreter
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������ȡ������װ����ָ��
 *@
 *@ ���������
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ָ����
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
 ************************************************************/
void* GetShortMsgLibInterpreter()
{
	return L;
}

/************************************************************
 *@ �������ƣ�PrepareNewShortMsg
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ����������׼����װ���µĶ���
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_APPERR(__func__,"ERR","��ʼ��ģ�����ʧ��,����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
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
		PUBLOG_DBERR(__func__,"1","��CUR_DEFUALT_PARAM�α�ʧ��");
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
			PUBLOG_DBERR(__func__,"2","��ȡ�α�CUR_DEFAULT_PARAMʧ��");
			return 2;
		}
		Trim(vDefaultParam);
		Trim(vDefaultValue);

		ret = SetShortMsgParameter(iModelId,vDefaultParam, vDefaultValue);
		if(ret != 0)
		{
			EXEC SQL CLOSE CUR_DEFAULT_PARAM;
			PUBLOG_APPERR(__func__,"3","����ģ��Ĭ��ֵ[%s] = [%s]ʧ��,ret = %d!",vDefaultParam, vDefaultValue, ret);
			return 3;
		}
	}
	EXEC SQL CLOSE CUR_DEFAULT_PARAM;

	return 0;
}

/************************************************************
 *@ �������ƣ�SetShortMsgParameter
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ �������������ö���ģ����ز���
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iKey             ģ�����KEY
 *@        iValue           ģ�����ֵ
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_APPERR(__func__,"ERR","���ö���ģ�����ʧ�ܣ�����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	return 0;
}

/************************************************************
 *@ �������ƣ�SetShortMsgNumParameter
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ �������������ö���ģ����ز���
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iKey             ģ�����KEY
 *@        iValue           ģ�����ֵ
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_APPERR(__func__,"ERR","���ö���ģ�����ʧ�ܣ�����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	return 0;
}

/************************************************************
 *@ �������ƣ�CheckMandatoryParameters
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������֤����ģ�����Ĳ����Ƿ񶼴���
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
 ************************************************************/
int CheckMandatoryParameters(int iModelId)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vDefaultParam[40+1];
		char vSqlStr[200+1];
	EXEC SQL END DECLARE SECTION;
	int  ret;

#ifdef DEBUG
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
		PUBLOG_DBERR(__func__,"1","��CUR_MANDATORY_PARAM�α�ʧ��");
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
			PUBLOG_DBERR(__func__,"2","��ȡ�α�CUR_MANDATORY_PARAMʧ��");
			return 2;
		}
		Trim(vDefaultParam);

		ret = CheckMandatoryParameter(iModelId,vDefaultParam);
		if(ret != 0)
		{
			EXEC SQL CLOSE CUR_MANDATORY_PARAM;
			PUBLOG_APPERR(__func__,"3","ģ�����[%s]�������,����Ϊ��!",vDefaultParam);
			return 3;
		}
	}
	EXEC SQL CLOSE CUR_MANDATORY_PARAM;

	return 0;
}

/************************************************************
 *@ �������ƣ�GetShortMsgParameter
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ����������ȡ��ģ�������ǰֵ
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iKey             ģ�����KEY
 *@
 *@ ���ز�����
 *@        iValue           ģ�����ֵ
 *@
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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

#ifdef	DEBUG
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
		PUBLOG_APPERR(__func__,"ERR","��ȡ����ģ�����ʧ�ܣ�����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
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
 *@ �������ƣ�CheckMandatoryParameter
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������֤����ģ������Ƿ����
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iKey             ģ�����KEY
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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

#ifdef	DEBUG
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
		PUBLOG_APPERR(__func__,"ERR","��֤����ģ�����ʧ�ܣ�����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
		lua_pop(L,1);
		return ret;
	}

	exist_flag = 0;
	exist_flag = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return exist_flag;
}

/************************************************************
 *@ �������ƣ�CompileShortMsgModel
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ �����������������ģ��
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        model_pattern    ����ģ������
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
	print(\"��������������\") \n"
#endif
	"return %s \n\
	end",model_pattern);


#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"DEBUG","script function [%s]",func_buf);
#endif

	ret = luaL_dostring(L,func_buf);
	return ret;
}

/************************************************************
 *@ �������ƣ�GenerateShortMsg
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ������������װ����ʵ��
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iRecvPhoneNo     �����ֻ�����
 *@        iLoginNo         ��������
 *@        iOpCode          ��������
 *@        iLoginAccept     ������ˮ
 *@        iOpTime          ����ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_DBERR(__func__,"1","��ѯ����ģ��ʧ��!");
		ret = 1;
		goto END_FUNC;
	}

	Trim(vModelPattern);

	if((ret = CompileShortMsgModel(iModelId,vModelPattern)) != 0)
	{
		PUBLOG_APPERR(__func__,"2","�������ģ��ʧ��,ret = %d!",ret);
		ret = 2;
		goto END_FUNC;
	}

	if((ret = CheckMandatoryParameters(iModelId)) != 0)
	{
		PUBLOG_APPERR(__func__,"3","������ģ�����ʧ��,ret = %d!",ret);
		ret = 3;
		goto END_FUNC;
	}

	/* the function name */
	lua_getglobal(L, "GenerateSM");

	/* call the function return 1 string*/
	ret = lua_pcall(L, 0, 1, 0);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"4","���ú���[GenerateSM]����,����ֵ=[%d],������Ϣ=[%s]!",ret,lua_tostring(L, -1));
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


	PUBLOG_DEBUG(__func__,"DEBUG","���ֻ�[%s]���Ͷ��ţ�ҵ����ˮ[%ld]��ҵ�����[%s]!",iRecvPhoneNo,iLoginAccept,iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","��������Ϊ��[%s]",shortMsg);


	/*��ѯ���Ͷ���ʱ�����*/
	GetShortMsgParameter(iModelId,SHORTMSGSENDTIME,vSendTime);
	GetShortMsgParameter(iModelId,UPWARDACCEPT,vUpwardAccept);
	
	ret = DispatchShortMsg(iModelId, iRecvPhoneNo, iLoginNo, iOpCode, iOpTime, iLoginAccept, vUpwardAccept, shortMsg, vSendTime);
	if(ret != 0)
	{
		PUBLOG_APPERR(__func__,"5","��ѯ���ŵ�����ͨ��ʧ��!ret=[%d]",ret);
		free(shortMsg);
		goto END_FUNC;
	}

	free(shortMsg);

	ret = 0;

END_FUNC:


	PUBLOG_DEBUG(__func__,"DEBUG","before collect mem size:%d!",lua_getgccount(L));


	/**�����ڴ�����**/
	lua_gc(L,LUA_GCCOLLECT,0);


	PUBLOG_DEBUG(__func__,"DEBUG","after collect mem size:%d!",lua_getgccount(L));

	return ret;
}

/************************************************************
 *@ �������ƣ�DispatchShortMsg
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ �������������ݶ���ģ��ͨ�����ü����Ի����������������ݲ��뵽���Žӿڱ���
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iRecvPhoneNo     �����ֻ�����
 *@        iLoginNo         ��������
 *@        iOpCode          ��������
 *@        iOpTime          ����ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@        iLoginAccept     ������ˮ
 *@        iUpwardAccept    ����ҵ���ʶ��ˮ
 *@        iShortMsg        ��������
 *@        iSendTime        �Զ��巢��ʱ��
 *@
 *@ ���ز�����
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		char vChnTableName[70+1];  /**����ͨ����Ϣ��**/
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
		int  vMsgByteLen;  /*�������ݵ��ֽڳ���*/
		int  vMsgCharLen;  /*�������ݵ��ַ�����*/
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
		PUBLOG_DBERR(__func__,"-1","��ѯ����ģ����Ϣʧ��");
		return -1;
	}
	
	Trim(vWorkGroup);
	if( (ret = QryShortMsgChannel(iOpCode, vWorkGroup, vChnTableName)) != 0)
	{
		PUBLOG_APPERR(__func__,"-2","��ȡ����ͨ��ʧ��!");
		return -2;
	}

#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"DEBUG","BUSI_CODE[%s]��ȡ����ͨ����[%s]",iOpCode, vChnTableName);
#endif

	Trim(vDefaultSmPort);
	Trim(vDefaultSmType);
	Trim(vSeqName);
	Trim(vFoldPolicy);
	Trim(vExpPolicy);
	Trim(vReqUpward);
	Trim(vSendTimePolicy);
	
	/*�������Ҫ���ж��ţ��ڶ˿ںź�ƴװ��ˮ��*/
	strcpy(vSmPort, vDefaultSmPort);
	if(vReqUpward[0] == '1' && strlen(iUpwardAccept) > 0)
	{
		sprintf(vSmPort,"%s%s",vSmPort, iUpwardAccept);
	}
	
	/*������ŷ���ʱ�䣬����ʹ���ⲿ���뷢��ʱ�䣬δ����ʹ��ģ�巢��ʱ���������*/

#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"DEBUG","OPTIME=[%s]\n",iOpTime);
#endif


	if(strlen(iSendTime) == 0)
	{
		/*��������*/
		if(vSendTimePolicy[0] == '0')
		{
			strcpy(vSendTime, iOpTime);
		}
		else
		{
			if((ret = ComputePolicyTime(vSendTimePolicy[0], iOpTime, vSendTimeOffset,vSendTime)) != 0)
			{
				PUBLOG_APPERR(__func__,"-3","������ŷ���ʱ��ʧ��,ret=[%d]\n",ret);
				return -3;
			}
		}
	}
	else
	{
		strcpy(vSendTime,iSendTime);
	}

	/*������ų�ʱʱ��*/
	if(vExpPolicy[0] == '0')
	{
		strcpy(vExpTime,SYS_DOOMSDAY);
	}
	else if(vExpPolicy[0] == '1')
	{
		/*���ݷ���ʱ������ų���ͬ����űȽϣ�ɾ��ͬ�����*/
		ret = DeleteSameTypeMsg(iModelId,vChnTableName,
								iRecvPhoneNo, iOpCode, iSendTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__,"-17","ɾ��ͬ�����ʧ��,ret=[%d]\n",ret);
			return -17;
		}
	}
	else
	{
		ret = ComputePolicyTime(vExpPolicy[0], iOpTime, vDuration, vExpTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__,"-4","������ų�ʱʱ��ʧ��,ret=[%d]\n",ret);
			return -4;
		}
	}

	/*�������ʹ��������������۵�����*/
	strcpy(vSmType, vDefaultSmType);
	
	vMsgByteLen = strlen(iShortMsg);
	vMsgCharLen = ChnStrLen(iShortMsg);
	
	/*��ͨ���ų�������70���ַ�*/
	#define NORM_SHORTMSG_LEN 70
	/*PUSH������URL�������ƣ��ܳ���(130) - PUSH_HEADER(20) - TEXT_HEADER(4) - TEXT_END(4) = 100*/
	#define PUSH_SHORTMSG_URL_LEN 100
	vNormMsgMaxLen = NORM_SHORTMSG_LEN;
	
	if(strcmp(vSmType,"push") == 0)  /**PUSH����**/
	{
		/*���ų��ȳ������ŷ�Χ*/
		if(vMsgByteLen > 1000)
		{
			PUBLOG_APPERR(__func__, "-5", "PUSH���Ź���[%s]!",iShortMsg);
			return -5;
		}
		
		p = (char *)strchr(iShortMsg,(int)('|'));
		if ( p == NULL)
		{
			PUBLOG_APPERR(__func__, "-6", "PUSH�����б���ʹ��'|'�ָ���Ϣ������URL");
			return -6;
		}
		
		vUrlLen = strlen(p);
			
		/*������URL���ȳ�����Χ*/
		if(vUrlLen >= PUSH_SHORTMSG_URL_LEN)
		{
			PUBLOG_APPERR(__func__, "-7", "PUSH������URL����[%s]!",p);
			return -7;
		}
		
		ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
							vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
							iOpCode, iOpTime, iLoginAccept, iShortMsg, 
							NULL, 0, vSendTime, vExpTime);
		if(ret != 0)
		{
			PUBLOG_APPERR(__func__, "-8", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
			return -8;
		}
	}
	else if(strcmp(vSmType,"long") == 0) /*������*/
	{
		vMsgLeftLen = vMsgByteLen;
		Sinitn(vShortMsgTmp);
		if(vMsgLeftLen > 1024) /*PROC���뻺������󳤶�1024*/
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
		if(vMsgLeftLen > 1024)   /*����1000ʣ���ַ��ض�*/
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
			PUBLOG_APPERR(__func__, "-10", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
			return -10;
		}
	}
	else /*��ͨ����*/
	{
		/*���ų��ȳ�����ͨ���ŷ�Χ*/
		if(vMsgCharLen > vNormMsgMaxLen)
		{
			/*��������ԭ��*/
			if(vFoldPolicy[0] == '0') /*�ض�*/
			{
				Sinitn(vShortMsgTmp);
				strncpy(vShortMsgTmp,iShortMsg,(vNormMsgMaxLen*2)+3);  /*��ȡ�����ֽڣ����ڽضϺ�������*/
				
				EXEC SQL Select nvl(substr(:vShortMsgTmp,1,70),' ') 
					Into :vShortMsg from dual;
				if(SQLCODE != 0)
				{
					PUBLOG_DBERR(__func__,"-11","�ض϶�������ʧ��!");
					return -11;
				}
				
				Trim(vShortMsg);

				ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
								iOpCode, iOpTime, iLoginAccept, vShortMsg, 
								NULL, 0, vSendTime, vExpTime);
				if(ret != 0)
				{
					PUBLOG_APPERR(__func__, "-12", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
					return -12;
				}
			}
			else if(vFoldPolicy[0] == '1') /*��ɶ�����ͨ���ŷ���*/
			{
				vMsgLeftLen = vMsgByteLen;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024) /*PROC���뻺������󳤶�1024*/
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
				if(vMsgLeftLen > 1024)   /*����1024ʣ���ַ��ض�*/
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
					PUBLOG_APPERR(__func__, "-13", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
					return -13;
				}
			}
			else if(vFoldPolicy[0] == '2')  /*ת���ɳ����ŷ���*/
			{
				/*�ı��������Ϊ����������*/
				strcpy(vSmType,"long");
				vMsgLeftLen = vMsgByteLen;
				Sinitn(vShortMsgTmp);
				if(vMsgLeftLen > 1024) /*PROC���뻺������󳤶�1024*/
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
				if(vMsgLeftLen > 1024)   /*����1024ʣ���ַ��ض�*/
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
					PUBLOG_APPERR(__func__, "-14", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
					return -14;
				}
			}
			else
			{
				PUBLOG_APPERR(__func__,"-14","������ͨ���Ŵ���ԭ�����ô���Policy=[%s]\n",vFoldPolicy);
				return -15;
			}
		}
		else /*������ͨ����*/
		{
			ret = InsertShortMsg(iModelId,vChnTableName,vSeqName,vMsgLevel, 
								vSmPort, vSmType, iRecvPhoneNo, iLoginNo, 
								iOpCode, iOpTime, iLoginAccept, iShortMsg, 
								NULL, 0, vSendTime, vExpTime);
			if(ret != 0)
			{
				PUBLOG_APPERR(__func__, "-16", "������Žӿڱ�ʧ��,ret=[%d]\n",ret);
				return -16;
			}
		}
	}

	return 0;
}

/************************************************************
 *@ �������ƣ�ComputePolicyTime
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ��������������ʱ��ƫ�Ʋ��Լ�ƫ��������δ��ʱ��
 *@
 *@ ���������
 *@        policy           ʱ��������ͣ�'2':ƫ�Ƶ��̶�ʱ�䣻'3':��СʱΪ��λƫ�ƣ�'4':����Ϊ��λƫ��
 *@        iOpTime          ��ǰ����ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@        offset           ʱ��ƫ����
 *@
 *@ ���ز�����
 *@        OutputTime       ���ʱ��
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
 ************************************************************/
int ComputePolicyTime(char policy, char *iOpTime, long offset, char *OutputTime)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char vTimeStr[17+1];
	EXEC SQL END DECLARE SECTION;
	int ret;
	Sinitn(vTimeStr);
#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"POLICY","��opTime=[%s],POLICY=[%c],offset=[%ld]����ʱ��",iOpTime,policy,offset);
#endif
	switch(policy)
	{
		/*�̶�ʱ��*/
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
		PUBLOG_DBERR(__func__,"-2","����ƫ����ʱ�����!");
		return -2;
	}
	strcpy(OutputTime,vTimeStr);
	
	return 0;
}

/************************************************************
 *@ �������ƣ�QryShortMsgChannel
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ��������������ҵ����������ѯ����ͨ��������δ���ù�ϵ�����ѡȡͨ��
 *@
 *@ ���������
 *@        iOpCode          ҵ���������
 *@
 *@ ���ز�����
 *@        oTableName       ����ͨ������
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
 ************************************************************/
int QryShortMsgChannel(const char *iOpCode, char *iWorkGroup, char *oTableName)
{
	int ret;
	EXEC SQL BEGIN DECLARE SECTION;
		char vChnTableName[70+1];  /**����ͨ����Ϣ��**/
		int vRandomRow = 0;
		int vCount = 0;
	EXEC SQL END DECLARE SECTION;


#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"DEBUG","BUSI_CODE = [%s]",iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","SYS_LABEL = [%s]",SysLabel);
	PUBLOG_DEBUG(__func__,"DEBUG","iWorkGroup= [%s]",iWorkGroup);
#endif

	EXEC SQL Select count(*)
		Into :vCount
		From SSMCHANNEL a, SBUSISMRELATION b
		Where a.channel_id = b.channel_id
			And a.belong_sys = :SysLabel
			AND b.busi_code = :iOpCode;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","��ѯ����ͨ������ʧ��");
		return -1;
	}
	
	if(vCount == 0)  /*û��ָ��ר��ͨ��*/
	{
		EXEC SQL select count (*)
			Into :vCount
			FROM ssmchannel
			Where belong_sys = :SysLabel
				And work_group = :iWorkGroup;
		if(SQLCODE != 0)
		{
			PUBLOG_DBERR(__func__,"-2","��ѯ����ͨ������ʧ��");
			return -2;
		}

		if(vCount <= 0)
		{
			PUBLOG_APPERR(__func__,"-3","�޿��ö���ͨ��");
			return -3;
		}

		srand((unsigned)time(0) + getpid());
		vRandomRow = rand()%vCount + 1;

#ifdef	DEBUG
		PUBLOG_DEBUG(__func__,"DEBUG","Random Row = [%d]",vRandomRow);
#endif

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
			PUBLOG_DBERR(__func__,"-4","��ѯ����ͨ����ʧ��");
			return -4;
		}
	}
	else if(vCount == 1)  /*ֻ��һ��ͨ��*/
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
			PUBLOG_DBERR(__func__,"-5","��ѯ����ͨ����ʧ��");
			return -5;
		}
	}
	else /*ָ���˶��ר��ͨ��*/
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
			PUBLOG_DBERR(__func__,"-6","��ѯ����ͨ����ʧ��");
			return -6;
		}
	}
}

/************************************************************
 *@ �������ƣ�DeleteSameTypeMsg
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ����������ɾ��ͬ�����ʵ����ɾ��������ͬһ�û���ͬһ�������롢���η���ʱ�� >= ��ǰ�Ķ��ŷ���ʱ��
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iChnTableName    ����ͨ������
 *@        iRecvPhoneNo     �����ֻ�����
 *@        iOpCode          ��������
 *@        iSendTime        Ԥ�Ʒ���ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@
 *@ ���ز�����
 *@
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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
		PUBLOG_DBERR(__func__,"-3","ɾ��ͬ�����ʧ��!");
		return -3;
	}
	EXEC SQL EXECUTE Pre_SM_Del using :iModelId,:iRecvPhoneNo,:iSendTime;
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		PUBLOG_DBERR(__func__,"-4","ɾ��ͬ�����ʧ��!");
		return -4;
	}
	
	return 0;
}

/************************************************************
 *@ �������ƣ�InsertShortMsg
 *@ �������ڣ�2011/03/20
 *@ ����汾: Ver1.0
 *@ ������Ա��sunzx
 *@ ����������������ŵ�ͨ��
 *@
 *@ ���������
 *@        iModelId         ģ��ID
 *@        iChnTableName    ����ͨ������
 *@        iSeqName         ��������
 *@        iMsgLevel        �������ȼ�
 *@        iSmPort          �����·��˿�
 *@        iSmType          ��������
 *@        iRecvPhoneNo     �����ֻ�����
 *@        iLoginNo         ��������
 *@        iOpCode          ��������
 *@        iOpTime          ����ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@        iLoginAccept     ����ҵ����ˮ
 *@        iShortMsg        ����Ϣ����
 *@        iShortMsgAdd     ����Ϣ׷������
 *@        iOrderCode       ����Ϣ�����
 *@        iSendTime        ����ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@        iExpTime         ����ʧЧʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'
 *@
 *@ ���ز�����
 *@
 *@ ����ֵ��
 *@        ������   0:�ɹ�  ����:ʧ��
 *@
 *@ �޸ļ�¼:
 *@        �޸���Ա��
 *@        �޸����ڣ�
 *@        �޸�ǰ�汾�ţ�
 *@        �޸ĺ�汾��:
 *@        �޸�ԭ��
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

#ifdef	DEBUG
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
#endif

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
		PUBLOG_DBERR(__func__,"-1","�������ʵ���ӿڱ�ʧ��!");
		return -1;
	}
	EXEC SQL EXECUTE Pre_SM_Send using :iModelId,:iLoginAccept,:iMsgLevel,
					:iSmPort,:iSmType,:iRecvPhoneNo,:iShortMsg,
					:iShortMsgAdd,:iOrderCode,:iLoginNo,:iOpCode,:iOpTime,
					:iSendTime,:iExpTime;
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-2","�������ʵ���ӿڱ�ʧ��!");
		return -2;
	}
	return 0;
}
