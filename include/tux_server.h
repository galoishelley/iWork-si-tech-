/**********************
 *Head file 
 * file name:tux_server.h
 * version 1.0
 * author:sunzx
 * description:
 *  Tuxedo��д����ģ��ͷ�ļ�
 * changes:
 *  1.����Serviceִ��ʱ������
 *  2.���ӳ�ʼ�������������չ���ܺ���TpSvrInit,TpSvrDone
 *  3.����Ĭ�����ݿ����ӷ�ʽʹ�ö�����
 *
 *********************/

#ifndef _TUX_SERVER_H_
#define _TUX_SERVER_H_
#include <sys/time.h>

/*Tuxedo Serviceǰ׺*/

#ifdef __cplusplus
#define _TUX_SVC_ extern "C" void
#else
#define _TUX_SVC_ void
#define SVCN(a) a
#endif

/*Ӧ��״̬��ʶ��������*/
#define GLOBAL_EMER "GLOBAL_EMER"
/*Ĭ�����ݿ��ǩ����*/
#define DEFAULT_DB_LABEL LABELDBCHANGE

#ifndef DB_LABEL
#define DB_LABEL DEFAULT_DB_LABEL
#endif

#define DEFAULT_OK_CODE "000000"
#define DEFAULT_OK_VALUE "�����ɹ�"

#ifdef Add_Ret_Value
#error �ظ�����Add_Ret_Value
#else
#define Add_Ret_Value(a,b) \
	add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
                (char*)__FUNCTION__,(a),(b),DB_LABEL,CONNECT0)
#endif

#ifdef SERVICE_FINALLY_DECLARE
#error �ظ�����SERVICE_FINALLY_DECLARE
#else
#define SERVICE_FINALLY_DECLARE() \
	strcpy(oRetCode,DEFAULT_OK_CODE); \
	strcpy(oRetMsg,DEFAULT_OK_VALUE); \
	finally_of_service: \
	if( (strcmp(oRetCode,DEFAULT_OK_CODE) == 0) && (__pResv1 != NULL) && ((strcmp(__pResv1,"ALL") == 0) || (strlen(strstr(__pResv1,__pOpCode)) != 0))) \
	{ \
		if(fSendOrderEmer(input_par_num, output_par_num, transIN, DB_LABEL, SVCN(__FUNCTION__), \
			__pIdType, __pIdNo, (*__pLoginAccept), __pOpCode, __pLoginNo, __pOpNote, \
			&__serviceDbCfg, oRetCode, oRetMsg) != 0) \
		{ \
			printf("��¼��������,ErrorCode = [%s],ErrorMsg = [%s]\n",oRetCode,oRetMsg); \
			printf("Service input parameter dump begin with timestamp[%ld.%ld]\n",__time_start.tv_sec, __time_start.tv_usec); \
			Fprint32(transIN); \
			printf("Service input parameter dump end with timestamp[%ld.%ld]\n",__time_start.tv_sec, __time_start.tv_usec); \
			strcpy(oRetCode,DEFAULT_OK_CODE); \
			strcpy(oRetMsg,DEFAULT_OK_VALUE); \
		} \
	} \
	
#endif

#ifdef RAISE_ERROR
#error �ظ�����RAISE_ERROR
#else
#define RAISE_ERROR(errCode,errMsg) \
	strcpy(oRetCode,(errCode)); \
	strcpy(oRetMsg,(errMsg)); \
	goto finally_of_service;
#endif

/*�Զ����ʼ����������������*/
#ifdef __cplusplus
extern "C"
{
#endif
extern int TpSvrInit(int argc, char *argv[]);
extern void TpSvrDone();
#ifdef __cplusplus
}
#endif

/*Tuxedo Server���ݿⳤ���ӿ�ʼ�����*/
#define TUX_SERVER_BEGIN_L() \
/**TUX_SERVER_BEGIN_L BEGIN**/ \
extern char input_parms[MAX_PARMS_NUM][1000];  \
extern char output_parms[MAX_PARMS_NUM][1000]; \
\
int tpsvrinit(int argc, char *argv[]) \
{\
	int ret; \
	sPublicDBCfg globalCfg; \
	memset(&globalCfg,0,sizeof(sPublicDBCfg)); \
	if(0 != (ret = getDbCfg(GLOBAL_EMER, &globalCfg))) \
	{ \
		PUBLOG_APPERR(GLOBAL_EMER,"-1","��ȡ�����ڴ��ж�Ӧ�������,�������[%d]!",ret); \
	} \
\
	ret = spublicDBLoginEmer(NULL,NULL,__FILE__,DB_LABEL, &globalCfg); \
	if(ret != 0) \
	{ \
		userlog("Server (%s) start error,connect database error code = %d\n",__FILE__,ret); \
		exit(-1); \
	} \
	TpSvrInit(argc,argv); \
	PUBLOG_APPRUN(__FUNCTION__, "APPRUN", "Server (%s) is running!",__FILE__); \
	return (0); \
} \
\
void tpsvrdone() \
{\
	spublicDBClose(DB_LABEL); \
	TpSvrDone(); \
	PUBLOG_APPRUN(__FUNCTION__, "APPRUN", "Server (%s) has closed!",__FILE__); \
}\
/**TUX_SERVER_BEGIN_L END**/ \
int __dummy()

/*Tuxedo Server���ݿ�����ӿ�ʼ�����*/
#define TUX_SERVER_BEGIN_S() \
/**TUX_SERVER_BEGIN_S BEGIN**/ \
\
extern char input_parms[MAX_PARMS_NUM][1000];  \
extern char output_parms[MAX_PARMS_NUM][1000]; \
\
int tpsvrinit(int argc, char *argv[]) \
{\
	TpSvrInit(argc,argv); \
	PUBLOG_APPRUN(__FUNCTION__, "APPRUN", "Server (%s) is running!",__FILE__); \
	return(0); \
} \
\
void tpsvrdone() \
{\
	TpSvrDone(); \
	PUBLOG_APPRUN(__FUNCTION__, "APPRUN", "Server (%s) has closed!",__FILE__); \
}\
/**TUX_SERVER_BEGIN_S END**/ \
int __dummy()

/*Tuxedo Service���ݿⳤ���ӿ�ʼ�����*/
#define TUX_SERVICE_BEGIN_L(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2) \
	/**TUX_SERVICE_BEGIN_L BEGIN**/ \
	FBFR32  *transIN = NULL; \
	FBFR32  *transOUT = NULL; \
	int     __i = 0; \
	int     __fml_len = 0; \
	int     __db_ret = 0; \
	struct timeval __time_start,__time_end; \
	char    oRetCode[6+1]; \
	char    oRetMsg[60+1]; \
	char    __tmpStr[1024+1]; \
	int     input_par_num = 0; \
	int     output_par_num = 0; \
	char *__pIdType = NULL;  /*�û���ʶ��������*/ \
	char *__pIdNo = NULL;    /*�û���ʶ����*/ \
	long *__pLoginAccept = NULL; /*������ˮ*/ \
	char *__pOpCode = NULL;  /*��������*/ \
	char *__pLoginNo = NULL; /*��������*/ \
	char *__pOpNote = NULL;  /*������ע*/ \
	char *__pResv1 = NULL;   /*��������1*/ \
	char *__pResv2 = NULL;   /*��������2*/ \
	sPublicDBCfg __serviceDbCfg; \
\
	memset(oRetCode,0,sizeof(oRetCode)); \
	memset(oRetMsg,0,sizeof(oRetMsg)); \
	memset(__tmpStr,0,sizeof(__tmpStr)); \
	memset(&__serviceDbCfg,0,sizeof(sPublicDBCfg)); \
	strcpy(oRetCode,DEFAULT_OK_CODE); \
	strcpy(oRetMsg,DEFAULT_OK_VALUE); \
	gettimeofday(&__time_start,NULL); \
	transIN = (FBFR32 *)transb->data; \
	get_input_parms32_2(transIN, &input_par_num, &output_par_num,DB_LABEL, CONNECT0); \
	__fml_len = (FLDLEN32)(FIRST_OCCS32 * output_par_num * 50); \
	transOUT = (FBFR32 *)tpalloc(FMLTYPE32,NULL,__fml_len); \
	if (transOUT == (FBFR32 *)NULL) \
	{ \
		sprintf(__tmpStr,"E200: alloc error in %s,size(%ld)",__FUNCTION__,(long)__fml_len); \
		PUBLOG_APPERR(__FUNCTION__,"ERR","�������������ʧ�ܣ������С(%ld)",(long)__fml_len); \
		error_handler32(transIN,transOUT,"200",__tmpStr,DB_LABEL,CONNECT0); \
	} \
\
	PUBLOG_APPRUN(__FUNCTION__,"ARGUMENTS DUMP","����%s��ʼִ��...",__FUNCTION__); \
\
	__db_ret = fEmerCheck(SVCN(__FUNCTION__), &__serviceDbCfg, oRetCode, oRetMsg); \
	if(0 != __db_ret) \
	{ \
		PUBLOG_APPERR(__FUNCTION__, oRetCode, "���Ӧ����־ʧ��,retMsg=[%s]!", oRetMsg); \
		RAISE_ERROR(oRetCode,oRetMsg); \
	} \
	PUBLOG_DEBUG(__FUNCTION__,"DB CONNECT DUMP","������ó����ӷ�ʽ�������ݿ�"); \
	if ( (__db_ret = sChkDBLoginEmer(transIN,transOUT,__FUNCTION__,DB_LABEL,&__serviceDbCfg)) != 0) \
	{ \
		sprintf(__tmpStr,"E201: connect database error in %s,ret =(%d)",__FUNCTION__,__db_ret); \
		PUBLOG_APPERR(__FUNCTION__,"ERR","�������ݿ�����ʧ�ܣ�����ֵ(%d)",__db_ret); \
		error_handler32(transIN,transOUT,"201",__tmpStr,DB_LABEL,CONNECT0); \
	} \
	for( __i = 0; __i < input_par_num; __i++) \
	{ \
		PUBLOG_DEBUG(__FUNCTION__,"ARGUMENTS DUMP","+ input_parms[%d] = [%s] +++ ",__i, input_parms[__i]); \
	} \
\
	__pLoginAccept = &inLoginAccept; \
	__pOpCode = inOpCode; \
	__pLoginNo = inLoginNo; \
	__pOpNote = inOpNote; \
	__pIdType = inIdType; \
	__pIdNo = inIdNo; \
	__pResv1 = inResv1; \
	__pResv2 = inResv2; \
	/**TUX_SERVICE_BEGIN_L END**/ \

/*Tuxedo Service���ݿ�����ӿ�ʼ�����*/
#define TUX_SERVICE_BEGIN_S(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2) \
	/**TUX_SERVICE_BEGIN_S BEGIN**/ \
	FBFR32  *transIN = NULL; \
	FBFR32  *transOUT = NULL; \
	int     __i = 0; \
	int     __fml_len = 0; \
	int     __db_ret = 0; \
	struct timeval __time_start,__time_end; \
	char    oRetCode[6+1]; \
	char    oRetMsg[60+1]; \
	char    __tmpStr[1024+1]; \
	int     input_par_num = 0; \
	int     output_par_num = 0; \
	char *__pIdType = NULL;  /*�û���ʶ��������*/ \
	char *__pIdNo = NULL;    /*�û���ʶ����*/ \
	long *__pLoginAccept = NULL; /*������ˮ*/ \
	char *__pOpCode = NULL;  /*��������*/ \
	char *__pLoginNo = NULL; /*��������*/ \
	char *__pOpNote = NULL;  /*������ע*/ \
	char *__pResv1 = NULL;   /*��������1*/ \
	char *__pResv2 = NULL;   /*��������2*/ \
	sPublicDBCfg __serviceDbCfg; \
\
	memset(oRetCode,0,sizeof(oRetCode)); \
	memset(oRetMsg,0,sizeof(oRetMsg)); \
	memset(__tmpStr,0,sizeof(__tmpStr)); \
	memset(&__serviceDbCfg,0,sizeof(sPublicDBCfg)); \
	strcpy(oRetCode,DEFAULT_OK_CODE); \
	strcpy(oRetMsg,DEFAULT_OK_VALUE); \
	gettimeofday(&__time_start,NULL); \
	transIN = (FBFR32 *)transb->data; \
	get_input_parms32_2(transIN, &input_par_num, &output_par_num,DB_LABEL, CONNECT0); \
	__fml_len = (FLDLEN32)(FIRST_OCCS32 * output_par_num * 50); \
	transOUT = (FBFR32 *)tpalloc(FMLTYPE32,NULL,__fml_len); \
	if (transOUT == (FBFR32 *)NULL) \
	{ \
		sprintf(__tmpStr,"E200: alloc error in %s,size(%ld)",__FUNCTION__,(long)__fml_len); \
		PUBLOG_APPERR(__FUNCTION__,"ERR","�������������ʧ�ܣ������С(%ld)",(long)__fml_len); \
		error_handler32(transIN,transOUT,"200",__tmpStr,DB_LABEL,CONNECT0); \
	} \
\
	PUBLOG_DEBUG(__FUNCTION__,"ARGUMENTS DUMP","����%s��ʼִ��...",__FUNCTION__); \
\
	__db_ret = fEmerCheck(SVCN(__FUNCTION__), &__serviceDbCfg, oRetCode, oRetMsg); \
	if(0 != __db_ret) \
	{ \
		PUBLOG_APPERR(__FUNCTION__, oRetCode, "���Ӧ����־ʧ��,retMsg=[%s]!", oRetMsg); \
		RAISE_ERROR(oRetCode,oRetMsg); \
	} \
	PUBLOG_DEBUG(__FUNCTION__,"DB CONNECT DUMP","������ö����ӷ�ʽ�������ݿ�"); \
	if ( (__db_ret = spublicDBLoginEmer(transIN,transOUT,__FILE__,DB_LABEL,&__serviceDbCfg)) != 0) \
	{ \
		sprintf(__tmpStr,"E201: connect database error in %s,ret =(%d)",__FUNCTION__,__db_ret); \
		PUBLOG_APPERR(__FUNCTION__,"ERR","�������ݿ�����ʧ�ܣ�����ֵ(%d)",__db_ret); \
		error_handler32(transIN,transOUT,"201",__tmpStr,DB_LABEL,CONNECT0); \
	} \
	for( __i = 0; __i < input_par_num; __i++) \
	{ \
		PUBLOG_DEBUG(__FUNCTION__,"ARGUMENTS DUMP","+ input_parms[%d] = [%s] +++ ",__i, input_parms[__i]); \
	} \
\
	__pLoginAccept = &inLoginAccept; \
	__pOpCode = inOpCode; \
	__pLoginNo = inLoginNo; \
	__pOpNote = inOpNote; \
	__pIdType = inIdType; \
	__pIdNo = inIdNo; \
	__pResv1 = inResv1; \
	__pResv2 = inResv2; \
	/**TUX_SERVICE_BEGIN_S END**/ \

/*Tuxedo Service���ݿⳤ���ӽ��������*/
#define TUX_SERVICE_END_L() \
	/**TUX_SERVICE_END_L BEGIN**/ \
	transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
                (char*)__FUNCTION__,SVC_ERR_NO32,oRetCode,DB_LABEL,CONNECT0); \
	transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
                (char*)__FUNCTION__,SVC_ERR_MSG32,oRetMsg,DB_LABEL,CONNECT0); \
	if(strcmp(oRetCode,"000000") == 0 && __pLoginAccept!=(long *)"" && __pLoginAccept!=(long *)" ")\
	{\
		printf("������ˮ[%ld]\n",*__pLoginAccept);\
		transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
	                (char*)__FUNCTION__,SVC_ACCEPT32,ltoa(*__pLoginAccept),DB_LABEL,CONNECT0); \
	}\
	gettimeofday(&__time_end,NULL); \
	PUBLOG_APPRUN(__FUNCTION__,"PERFORM STATISTICS","Service (%s) elapse %.3lf(s)", \
					__FUNCTION__, \
					(double)(1000000 * (__time_end.tv_sec - __time_start.tv_sec) + (__time_end.tv_usec - __time_start.tv_usec))/1000000); \
\
	tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0); \
	/**TUX_SERVICE_END_L END**/ \

/*Tuxedo Service���ݿ�����ӽ��������*/
#define TUX_SERVICE_END_S() \
	/**TUX_SERVICE_END_S BEGIN**/ \
	transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
                (char*)__FUNCTION__,SVC_ERR_NO32,oRetCode,DB_LABEL,CONNECT0); \
	transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
                (char*)__FUNCTION__,SVC_ERR_MSG32,oRetMsg,DB_LABEL,CONNECT0); \
	if(strcmp(oRetCode,"000000") == 0 && __pLoginAccept!=(long *)"" && __pLoginAccept!=(long *)" ")\
	{\
		printf("������ˮ[%ld]\n",*__pLoginAccept);\
		transOUT = add_value32(transIN,transOUT,ONCE_ADD_LINES*output_par_num*50, \
	                (char*)__FUNCTION__,SVC_ACCEPT32,ltoa(*__pLoginAccept),DB_LABEL,CONNECT0); \
	}\
	spublicDBClose(DB_LABEL); \
	gettimeofday(&__time_end,NULL); \
	PUBLOG_DEBUG(__FUNCTION__,"PERFORM STATISTICS","Service (%s) elapse %.3lf(s)", \
					__FUNCTION__, \
					(double)(1000000 * (__time_end.tv_sec - __time_start.tv_sec) + (__time_end.tv_usec - __time_start.tv_usec))/1000000); \
	tpreturn(TPSUCCESS,0,(char *)transOUT,0L,0); \
	/**TUX_SERVICE_END_S END**/ \

#ifndef DB_LONG_CONN
#define TUX_SERVER_BEGIN() TUX_SERVER_BEGIN_S()
#define TUX_SERVICE_BEGIN(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2) TUX_SERVICE_BEGIN_S(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2)
#define TUX_SERVICE_END()   TUX_SERVICE_END_S()
#else
#define TUX_SERVER_BEGIN()  TUX_SERVER_BEGIN_L()
#define TUX_SERVICE_BEGIN(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2) TUX_SERVICE_BEGIN_L(inLoginAccept,inOpCode,inLoginNo,inOpNote,inIdType,inIdNo,inResv1,inResv2)
#define TUX_SERVICE_END()   TUX_SERVICE_END_L()
#endif

#endif
