#ifndef __BOSS_SVR_H
#define __BOSS_SVR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <dlfcn.h>
#include <atmi.h>
#include <userlog.h>
#include <fml32.h>   
#ifdef _FML16_
#include <fml.h>   
#include "general.flds.h"
#endif
#include "general32.flds.h"
#include "funcParam.h"
#include "des.h"

/* modified by Chenyq 2002.4.28 */
/*#include "ferror.h"*/
#include "pubdef.h"
#include "wd_incl.h"
#include "DBS_DEFINE.h"
#define TIMELEN 30
#include "error_no.h"
#include "province.h"
#include "pubProduct.h"

#include "boss_define.h"
#include "pubLog.h"


#include "order_xml.h"
#include "pubOrder.h"


#if USE_SCCSID
static char Sccsid[] = {"@(#) boss_srv.h 1.1 12/27/2005"};
#endif /* USE_SCCSID */

#define _TIMETEST_

#define MAX_PARMS_NUM    60
#define MAX_PARMS_LENGTH 1000

/* ����־���������Ƿ����μƷ�С����� dx ���� */
#define _CHGTABLE_
/*#ifdef _CHGTABLE_
#undef _CHGTABLE_
#endif*/
/*
Define global variables used in all  programs
*/
#ifndef TRUE
#define TRUE 	1
#endif

#ifndef FALSE
#define FALSE 	0
#endif

#define FIRST_OCCS        100
#define FIRST_OCCS32      100
#define CNO_MORE_ROWS     1403
#define ONCE_ADD_LINES    10
#define SQL_OK            0

#define BINARY          1
#define BIT             2
#define CHAR            3
#define DATETIME        4
#define DATETIMN        5
#define DECIMAL         6
#define DECIMALN        7
#define FLOAT           8
#define FLOATN          9
#define IMAGE           10
#define INT             11
#define INTN            12
#define MONEY           13
#define MONEYN          14
#define NCHAR           15
#define NUMERIC         16
#define NUMERICN        17
#define NVARCHAR        18
#define REAL            19
#define SMALLDATETIME   20
#define SMALLINT        21
#define SMALLMONEY      22

#ifdef SYSNAME
#undef SYSNAME
#endif
#define SYSNAME         23

#define TEXT            24
#define TIMESTAMP       25
#define TINYINT         26
#define VARBINARY       27
#define VARCHAR         28

#define SQLCODE sqlca.sqlcode
#define SQLROWS sqlca.sqlerrd[2]
/**********************************/
/* lixg begin to define _SRV_TAR_ */
/**********************************/
#define LGZ_TEMPBUFLEN      128
#define MAXTINYINTLEN       3
#define MAXSMALLINTLEN      6
#define MAXINTLEN           11
#define MAXSMALLMONEYLEN    12
#define MAXMONEYLEN         21
#define MAXSMALLDATETIMELEN     17
#define CONNECT0            "0"
#define CONNECT1            "1"

#define LABELDBPAY          "dbpay"
#define LABELDBSYS          "dbsys"
/* lixg define end */

#define EXTERN

#ifndef   EXTERN
	void err_hand();
	/*  16λ  */
	void FML_errchk(char *msg);
	void error_handler(FBFR *transIN,FBFR *transOUT,char *errno,char *errmsg,char *dbconnect_name,char *connect_type);
	void warning_handler();
	void get_input_parms(FBFR *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	void get_input_parms1(FBFR *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	FBFR *add_value(FBFR *transIN,FBFR *transOUT,long realloc_size, char *service_name, FLDID param_name, char *param_value, char *dbconnect_name,char *connect_type);
	/*  16λ  */
	/*  32λ  */
	void error_handler32(FBFR32 *transIN,FBFR32 *transOUT,char *errno,char *errmsg,char *dbconnect_name,char *connect_type);
	void warning_handler32();
	void get_input_parms32(FBFR32 *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	FBFR32 *add_value32(FBFR32  *transIN, FBFR32 *transOUT, long  realloc_size, char *service_name,FLDID32 param_name, char *param_value, char *dbconnect_name,char *connect_type);
	/*  32λ  */
    int spublicDBLogin(FBFR32 *transIN,FBFR32 *transOUT,char *srvName,char *connect_name);
    int spublicDBClose(char *connect_name);
	int PublicInsertShortMsg(int InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney);
	char *alltrim(char *str);
	char *rtrim(char *str);
	char *ltrim(char *str);

	int funcChk(CFUNCCONDENTRY *cFuncCondEntry);

#else
	extern int funcChk(CFUNCCONDENTRY *cFuncCondEntry);
	extern void err_hand();
	/*  16λ  */
#ifdef _FML16_
	extern void FML_errchk(char *msg);
	extern void error_handler(FBFR *transIN,FBFR *transOUT,char *errno,char *errmsg,char *dbconnect_name,char *connect_type);
	extern void warning_handler();
	extern void get_input_parms(FBFR *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	extern void get_input_parms1(FBFR *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	extern FBFR *add_value(FBFR *transIN,FBFR *transOUT,long realloc_size, char *service_name, FLDID param_name, char *param_value, char *dbconnect_name,char *connect_type);

#endif
	/*  32λ  */
	extern void error_handler32(FBFR32 *transIN,FBFR32 *transOUT,char *errno,char *errmsg,char *dbconnect_name,char *connect_type);
	extern void warning_handler32();
	extern void get_input_parms32(FBFR32 *transIN, int *input_par_num, int *output_par_num,char *dbconnect_name,char *connect_type);
	extern FBFR32 *add_value32(FBFR32  *transIN, FBFR32 *transOUT, long  realloc_size, char *service_name,FLDID32 param_name, char *param_value, char *dbconnect_name,char *connect_type);

	/*extern int spublicDBLogin(FBFR32 *transIN,FBFR32 *transOUT,char *srvName,char *connect_name);*/
	extern int spublicDBClose(char *connect_name);
	extern int PublicInsertShortMsg(int InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,double InPayMoney);

	extern int GenerateInvoiceData(STRUCT_MONTHLYFEE InArrMonthlyFee[],int InCurrentPage,
	char * InCustUnit,char * InPhoneNo,int InContractNo,int InHighFeeFlag,
	float InCurPay,float InCurPrepay,float InLimitOwe,float InUnBillFee,int InCurPoint,float InRemonthFee,
	char * InWorkNo,int InPrintAccept,
	float * InOutPrepayRemain,char OutDataArray[100][61]);
	extern int GenerateInvoiceInt(STRUCT_MONTHLYFEE InArrMonthlyFee[],int InCurrentPage,
	char * InCustUnit,char * InPhoneNo,int InContractNo,int InHighFeeFlag,
	float InCurPay,float InCurPrepay,float InLimitOwe,float InUnBillFee,int InCurPoint,float InRemonthFee,
	char * InWorkNo,int InPrintAccept,
	float * InOutPrepayRemain,char OutDataArray[100][61]);

	extern char *alltrim(char *str);
	extern char *rtrim(char *str);
	extern char *ltrim(char *str);
	extern char* Trim(char *S);
	extern char *getSysTime(int type);

	extern int checkBillBindFunc(const char* allFuncCode, 
	const char billBindFunc[][2+1],
	int billBindFuncNum, char *funcCode);

	/*ȡ���ط� */
	extern int fDelFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/*�޸���Ч���ط�*/
	extern int fUpdUsedFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/*�޸�δ��Ч���ط�*/
	extern int fUpdUnUsedFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/*����������Ч���ط�*/
	extern int fAddUsedFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/*����ԤԼ��Ч���ط�*/
	extern int fAddUnUsedFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/*���Ӵ��̺ŵ��ط�*/
	extern int fAddAddFuncs(tFuncEnv *funcEnv, char *funcs);
	
	/* ����ҵ���������� */
	extern int fSndBizProcReq(tBizProcReq pReq, char *pSrvName);
	
	extern int sGetAllGroupName(const char* group_id,char* group_name,char* retmsg);
	
	/*�����û��������ú���*/
	int fpubCreateGrpUser1(tGrpUserMsg *grpUserMsg, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note);
	
	/*��ֹ�û����Ѽƻ�*/
	int fpubDisConUserMsg(char *inId_No, char *srvNo, char *inContract_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message);
	/*�����û�ɾ�����ú���*/
	int fpubDelGrpUser(char *inGrpId_No, char *login_no, char *op_code, char *op_time, long lSysAccept, char *op_note, char *return_message);
	
	/*�����û��������ú���*/
	int fpubCreateGrpUser(FBFR32 *transIN, int ProdIndx, int SrvIndx, tGrpUserMsg *grpUserMsg, 
			char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *feeList, char *systemNote);
	/*��ֹ�û����Ѽƻ�*/
	int fpubDisConUserMsg1(char *inId_No, char *inContract_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message);
	/*�����û�ɾ�����ú���*/
	int fpubDelGrpUser1(char *inGrpId_No, char *login_no, char *op_code, char *total_date, long lSysAccept, char *op_note, char *return_message);
	/*�����û��������ݵ��� dgrpidcorderinfo ��������*/
	int InsertIDCorderinfo(char *vGrpIdNo,char *vProcid,char *vProcesstime,char *vSerialno,char *vCustid,char *Custname,char *vAddress,
						char *vIsvip,char *vPhone,char *vEmail,char *vGroup_Mgr_Name,char *vGroup_Mgr_Sn,
						char *vCust_Manager_Id,char *vCust_Manager_Name,char *vCust_Manager_Sn,char *vPointid,
						char *vProductinfoidlist,char *vBusinesstype,char *vDomain,char *vDealflag,char *return_msg);
#endif

#endif /* __BOSS_SVR_H */
