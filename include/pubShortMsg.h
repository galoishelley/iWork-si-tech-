#ifndef _SM_UTIL_H_
#define _SM_UTIL_H_

#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * ��ʼ��������װ����
 **/
int InitShortMsgLib();

/**
 * ���ٶ�����װ����
 **/
int DestroyShortMsgLib();

/**
 * ��ȡ������װ����ָ��
 **/
void* GetShortMsgLibInterpreter();

/**
 * ׼����װ���µĶ���
 **/
int PrepareNewShortMsg(int model_id);

/**
 * ���ö���ģ����ز���
 **/
int SetShortMsgParameter(int model,const char *key, const char* value);

/**
 * ���ö���ģ����ز���
 **/
int SetShortMsgNumParameter(int model,const char *key, long value);

/**
 * ��֤����ģ������Ƿ����
 **/
int CheckMandatoryParameter(int model_id,const char *key);

/**
 * ��֤����ģ�����Ĳ����Ƿ񶼴���
 **/
int CheckMandatoryParameters(int model_id);

/**
 * �������ģ��
 **/
int CompileShortMsgModel(int model_id,const char* model_pattern);

/**
 * ��װ����ʵ��
 **/
int GenerateShortMsg(int iModelId, const char *iRecvPhoneNo, const char *iLoginNo,
                     const char *iOpCode, long iLoginAccept, const char *iOpTime);

#ifdef __cplusplus
}
#endif

/*ϵͳ������ʱ��*/
#define SYS_DOOMSDAY "20500101 00:00:00"
#define SQLCODE sqlca.sqlcode
#define SQLERRMSG sqlca.sqlerrm.sqlerrmc
char* Trim(char *S);
#define SYSLABELENVNAME "APP_SYS_LABEL"
#define SCRIPTFILENAME "SM_Generater.script"
#define SHORTMSGSENDTIME "_SEND_TIME"
#define UPWARDACCEPT "_UPWARD_ACCPET"


#endif /*END OF TEST DEFINE _SM_UTIL_H_*/
