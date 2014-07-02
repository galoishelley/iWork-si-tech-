#ifndef _SM_UTIL_H_
#define _SM_UTIL_H_

#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * 初始化短信组装环境
 **/
int InitShortMsgLib();

/**
 * 销毁短信组装环境
 **/
int DestroyShortMsgLib();

/**
 * 获取短信组装环境指针
 **/
void* GetShortMsgLibInterpreter();

/**
 * 准备组装的新的短信
 **/
int PrepareNewShortMsg(int model_id);

/**
 * 设置短信模板相关参数
 **/
int SetShortMsgParameter(int model,const char *key, const char* value);

/**
 * 设置短信模板相关参数
 **/
int SetShortMsgNumParameter(int model,const char *key, long value);

/**
 * 验证单个模板参数是否存在
 **/
int CheckMandatoryParameter(int model_id,const char *key);

/**
 * 验证短信模板必须的参数是否都存在
 **/
int CheckMandatoryParameters(int model_id);

/**
 * 编译短信模板
 **/
int CompileShortMsgModel(int model_id,const char* model_pattern);

/**
 * 组装短信实例
 **/
int GenerateShortMsg(int iModelId, const char *iRecvPhoneNo, const char *iLoginNo,
                     const char *iOpCode, long iLoginAccept, const char *iOpTime);

#ifdef __cplusplus
}
#endif

/*系统最大结束时间*/
#define SYS_DOOMSDAY "20500101 00:00:00"
#define SQLCODE sqlca.sqlcode
#define SQLERRMSG sqlca.sqlerrm.sqlerrmc
char* Trim(char *S);
#define SYSLABELENVNAME "APP_SYS_LABEL"
#define SCRIPTFILENAME "SM_Generater.script"
#define SHORTMSGSENDTIME "_SEND_TIME"
#define UPWARDACCEPT "_UPWARD_ACCPET"


#endif /*END OF TEST DEFINE _SM_UTIL_H_*/
