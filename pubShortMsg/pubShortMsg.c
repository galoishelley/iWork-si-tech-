/*     PUBLIC LOGGING INJECTION BEGIN      */
#define PUBLOG(svcName,logCode,errNo,...)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,logCode,errNo,__VA_ARGS__)
#define PUBLOG_DEBUG(svcName,errNo,...)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DEBUG_LOG,errNo,__VA_ARGS__)
#define PUBLOG_APPRUN(svcName,errNo,...) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,APPRUN_LOG,errNo,__VA_ARGS__)
#define PUBLOG_APPERR(svcName,errNo,...) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,APPERR_LOG,errNo,__VA_ARGS__)
#define PUBLOG_OSERR(svcName,errNo,...) pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,OSERR_LOG,errNo,__VA_ARGS__)
#define PUBLOG_DBERR(svcName,errNo,...)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,DBERR_LOG,errNo,__VA_ARGS__)
#define PUBLOG_ASERR(svcName,errNo,...)  pubLog(svcName,__FILE__,__FUNCTION__,__LINE__,ASERR_LOG,errNo,__VA_ARGS__)
/*      PUBLIC LOGGING INJECTION END       */


/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[15];
};
static struct sqlcxp sqlfpn =
{
    14,
    "pubShortMsg.cp"
};


static unsigned int sqlctx = 1301398;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[14];
   unsigned long  sqhstl[14];
            int   sqhsts[14];
            short *sqindv[14];
            int   sqinds[14];
   unsigned long  sqharm[14];
   unsigned long  *sqharc[14];
   unsigned short  sqadto[14];
   unsigned short  sqtdso[14];
} sqlstm = {12,14};

/* SQLLIB Prototypes */
extern sqlcxt (/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlcx2t(/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlbuft(/*_ void **, char * _*/);
extern sqlgs2t(/*_ void **, char * _*/);
extern sqlorat(/*_ void **, unsigned int *, void * _*/);

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem(/*_ char *, int * _*/);

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4130,852,8,0,
5,0,0,1,0,0,17,305,0,0,1,1,0,1,0,1,97,0,0,
24,0,0,1,0,0,45,307,0,0,1,1,0,1,0,1,3,0,0,
43,0,0,1,0,0,13,319,0,0,2,0,0,1,0,2,97,0,0,2,97,0,0,
66,0,0,1,0,0,15,327,0,0,0,0,0,1,0,
81,0,0,1,0,0,15,337,0,0,0,0,0,1,0,
96,0,0,1,0,0,15,342,0,0,0,0,0,1,0,
111,0,0,2,0,0,17,490,0,0,1,1,0,1,0,1,97,0,0,
130,0,0,2,0,0,45,492,0,0,1,1,0,1,0,1,3,0,0,
149,0,0,2,0,0,13,503,0,0,1,0,0,1,0,2,97,0,0,
168,0,0,2,0,0,15,511,0,0,0,0,0,1,0,
183,0,0,2,0,0,15,520,0,0,0,0,0,1,0,
198,0,0,2,0,0,15,525,0,0,0,0,0,1,0,
213,0,0,3,87,0,4,758,0,0,2,1,0,1,0,2,97,0,0,1,3,0,0,
236,0,0,4,245,0,4,931,0,0,12,1,0,1,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,
0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,1,3,0,0,
299,0,0,5,53,0,4,1128,0,0,2,1,0,1,0,1,97,0,0,2,97,0,0,
322,0,0,6,90,0,4,1295,0,0,2,1,0,1,0,1,3,0,0,2,97,0,0,
345,0,0,7,104,0,4,1300,0,0,3,2,0,1,0,1,97,0,0,1,3,0,0,2,97,0,0,
372,0,0,8,109,0,4,1305,0,0,3,2,0,1,0,1,97,0,0,1,3,0,0,2,97,0,0,
399,0,0,9,140,0,4,1361,0,0,3,2,0,1,0,2,3,0,0,1,97,0,0,1,97,0,0,
426,0,0,10,84,0,4,1375,0,0,3,2,0,1,0,2,3,0,0,1,97,0,0,1,97,0,0,
453,0,0,11,184,0,4,1399,0,0,6,5,0,1,0,2,97,0,0,1,97,0,0,1,97,0,0,1,3,0,0,1,97,
0,0,1,97,0,0,
492,0,0,12,149,0,4,1420,0,0,3,2,0,1,0,2,97,0,0,1,97,0,0,1,97,0,0,
519,0,0,13,196,0,4,1443,0,0,4,3,0,1,0,2,97,0,0,1,97,0,0,1,97,0,0,1,3,0,0,
550,0,0,14,0,0,17,1505,0,0,1,1,0,1,0,1,97,0,0,
569,0,0,14,0,0,21,1511,0,0,3,3,0,1,0,1,3,0,0,1,97,0,0,1,97,0,0,
596,0,0,15,0,0,17,1598,0,0,1,1,0,1,0,1,97,0,0,
615,0,0,15,0,0,21,1604,0,0,14,14,0,1,0,1,3,0,0,1,3,0,0,1,3,0,0,1,97,0,0,1,97,0,
0,1,97,0,0,1,97,0,0,1,97,0,0,1,3,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,0,0,1,97,
0,0,
};


#line 1 "pubShortMsg.cp"
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

/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/ora10g/app/oracle/product/10.2.0/precomp/public/SQLCA.H"
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */

#line 20 "pubShortMsg.cp"

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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 277 "pubShortMsg.cp"

		char vDefaultParam[40+1];
		char vDefaultValue[40+1];
		char vSqlStr[200+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 281 "pubShortMsg.cp"

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
	/* EXEC SQL PREPARE PRE_DEFAULT_PARAM From :vSqlStr; */ 
#line 305 "pubShortMsg.cp"

{
#line 305 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 305 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 305 "pubShortMsg.cp"
 sqlstm.arrsiz = 1;
#line 305 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 305 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 305 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 305 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 305 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )5;
#line 305 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 305 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 305 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vSqlStr;
#line 305 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )201;
#line 305 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 305 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 305 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 305 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 305 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 305 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 305 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 305 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 305 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 305 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 305 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 305 "pubShortMsg.cp"
}

#line 305 "pubShortMsg.cp"

	/* EXEC SQL DECLARE CUR_DEFAULT_PARAM cursor For PRE_DEFAULT_PARAM; */ 
#line 306 "pubShortMsg.cp"

	/* EXEC SQL OPEN CUR_DEFAULT_PARAM using :iModelId; */ 
#line 307 "pubShortMsg.cp"

{
#line 307 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 307 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 307 "pubShortMsg.cp"
 sqlstm.arrsiz = 1;
#line 307 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 307 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 307 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 307 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 307 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )24;
#line 307 "pubShortMsg.cp"
 sqlstm.selerr = (unsigned short)1;
#line 307 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 307 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 307 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqcmod = (unsigned int )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)&iModelId;
#line 307 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 307 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 307 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 307 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 307 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 307 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 307 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 307 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 307 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 307 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 307 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 307 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 307 "pubShortMsg.cp"
}

#line 307 "pubShortMsg.cp"


	if (SQLCODE !=0)
	{
		PUBLOG_DBERR(__func__,"1","��CUR_DEFUALT_PARAM�α�ʧ��");
		return 1;
	}

	for(;;)
	{
		Sinitn(vDefaultParam);
		Sinitn(vDefaultValue);
		/* EXEC SQL Fetch CUR_DEFAULT_PARAM
			Into :vDefaultParam,:vDefaultValue; */ 
#line 320 "pubShortMsg.cp"

{
#line 319 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 319 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 319 "pubShortMsg.cp"
  sqlstm.arrsiz = 2;
#line 319 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 319 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 319 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 319 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )43;
#line 319 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 319 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 319 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 319 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqfoff = (         int )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqfmod = (unsigned int )2;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)vDefaultParam;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )41;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhstv[1] = (unsigned char  *)vDefaultValue;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhstl[1] = (unsigned long )41;
#line 319 "pubShortMsg.cp"
  sqlstm.sqhsts[1] = (         int  )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqindv[1] = (         short *)0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqinds[1] = (         int  )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 319 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 319 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 319 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 319 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 319 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 319 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 319 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 319 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 319 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 319 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 319 "pubShortMsg.cp"
}

#line 320 "pubShortMsg.cp"

		if(SQLCODE == 1403)
		{
			break;
		}
		else if(SQLCODE != 0)
		{
			/* EXEC SQL CLOSE CUR_DEFAULT_PARAM; */ 
#line 327 "pubShortMsg.cp"

{
#line 327 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 327 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 327 "pubShortMsg.cp"
   sqlstm.arrsiz = 2;
#line 327 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 327 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 327 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 327 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )66;
#line 327 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 327 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 327 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 327 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 327 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 327 "pubShortMsg.cp"
}

#line 327 "pubShortMsg.cp"

			PUBLOG_DBERR(__func__,"2","��ȡ�α�CUR_DEFAULT_PARAMʧ��");
			return 2;
		}
		Trim(vDefaultParam);
		Trim(vDefaultValue);

		ret = SetShortMsgParameter(iModelId,vDefaultParam, vDefaultValue);
		if(ret != 0)
		{
			/* EXEC SQL CLOSE CUR_DEFAULT_PARAM; */ 
#line 337 "pubShortMsg.cp"

{
#line 337 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 337 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 337 "pubShortMsg.cp"
   sqlstm.arrsiz = 2;
#line 337 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 337 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 337 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 337 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )81;
#line 337 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 337 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 337 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 337 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 337 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 337 "pubShortMsg.cp"
}

#line 337 "pubShortMsg.cp"

			PUBLOG_APPERR(__func__,"3","����ģ��Ĭ��ֵ[%s] = [%s]ʧ��,ret = %d!",vDefaultParam, vDefaultValue, ret);
			return 3;
		}
	}
	/* EXEC SQL CLOSE CUR_DEFAULT_PARAM; */ 
#line 342 "pubShortMsg.cp"

{
#line 342 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 342 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 342 "pubShortMsg.cp"
 sqlstm.arrsiz = 2;
#line 342 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 342 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 342 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 342 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )96;
#line 342 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 342 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 342 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 342 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 342 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 342 "pubShortMsg.cp"
}

#line 342 "pubShortMsg.cp"


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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 474 "pubShortMsg.cp"

		char vDefaultParam[40+1];
		char vSqlStr[200+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 477 "pubShortMsg.cp"

	int  ret;

#ifdef DEBUG
	PUBLOG_DEBUG(__func__,"","Checking Parameters For model[%d]!",iModelId);
#endif

	Sinitn(vSqlStr);

	sprintf(vSqlStr,"Select smm_param_code "
					"From SSMMPARAMLIST a "
					"where bitand(a.param_flag,1) = 1 "
					"and sm_model_id = :v1");
	/* EXEC SQL PREPARE PRE_MANDATORY_PARAM From :vSqlStr; */ 
#line 490 "pubShortMsg.cp"

{
#line 490 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 490 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 490 "pubShortMsg.cp"
 sqlstm.arrsiz = 2;
#line 490 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 490 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 490 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 490 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 490 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )111;
#line 490 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 490 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 490 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vSqlStr;
#line 490 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )201;
#line 490 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 490 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 490 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 490 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 490 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 490 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 490 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 490 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 490 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 490 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 490 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 490 "pubShortMsg.cp"
}

#line 490 "pubShortMsg.cp"

	/* EXEC SQL DECLARE CUR_MANDATORY_PARAM cursor For PRE_MANDATORY_PARAM; */ 
#line 491 "pubShortMsg.cp"

	/* EXEC SQL OPEN CUR_MANDATORY_PARAM using :iModelId; */ 
#line 492 "pubShortMsg.cp"

{
#line 492 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 492 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 492 "pubShortMsg.cp"
 sqlstm.arrsiz = 2;
#line 492 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 492 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 492 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 492 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 492 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )130;
#line 492 "pubShortMsg.cp"
 sqlstm.selerr = (unsigned short)1;
#line 492 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 492 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 492 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqcmod = (unsigned int )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)&iModelId;
#line 492 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 492 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 492 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 492 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 492 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 492 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 492 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 492 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 492 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 492 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 492 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 492 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 492 "pubShortMsg.cp"
}

#line 492 "pubShortMsg.cp"


	if (SQLCODE !=0)
	{
		PUBLOG_DBERR(__func__,"1","��CUR_MANDATORY_PARAM�α�ʧ��");
		return 1;
	}

	for(;;)
	{
		Sinitn(vDefaultParam);
		/* EXEC SQL Fetch CUR_MANDATORY_PARAM
			Into :vDefaultParam; */ 
#line 504 "pubShortMsg.cp"

{
#line 503 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 503 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 503 "pubShortMsg.cp"
  sqlstm.arrsiz = 2;
#line 503 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 503 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 503 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 503 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )149;
#line 503 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 503 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 503 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 503 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqfoff = (         int )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqfmod = (unsigned int )2;
#line 503 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)vDefaultParam;
#line 503 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )41;
#line 503 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 503 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 503 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 503 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 503 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 503 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 503 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 503 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 503 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 503 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 503 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 503 "pubShortMsg.cp"
}

#line 504 "pubShortMsg.cp"

		if(SQLCODE == 1403)
		{
			break;
		}
		else if(SQLCODE != 0)
		{
			/* EXEC SQL CLOSE CUR_MANDATORY_PARAM; */ 
#line 511 "pubShortMsg.cp"

{
#line 511 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 511 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 511 "pubShortMsg.cp"
   sqlstm.arrsiz = 2;
#line 511 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 511 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 511 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 511 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )168;
#line 511 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 511 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 511 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 511 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 511 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 511 "pubShortMsg.cp"
}

#line 511 "pubShortMsg.cp"

			PUBLOG_DBERR(__func__,"2","��ȡ�α�CUR_MANDATORY_PARAMʧ��");
			return 2;
		}
		Trim(vDefaultParam);

		ret = CheckMandatoryParameter(iModelId,vDefaultParam);
		if(ret != 0)
		{
			/* EXEC SQL CLOSE CUR_MANDATORY_PARAM; */ 
#line 520 "pubShortMsg.cp"

{
#line 520 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 520 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 520 "pubShortMsg.cp"
   sqlstm.arrsiz = 2;
#line 520 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 520 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 520 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 520 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )183;
#line 520 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 520 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 520 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 520 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 520 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 520 "pubShortMsg.cp"
}

#line 520 "pubShortMsg.cp"

			PUBLOG_APPERR(__func__,"3","ģ�����[%s]�������,����Ϊ��!",vDefaultParam);
			return 3;
		}
	}
	/* EXEC SQL CLOSE CUR_MANDATORY_PARAM; */ 
#line 525 "pubShortMsg.cp"

{
#line 525 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 525 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 525 "pubShortMsg.cp"
 sqlstm.arrsiz = 2;
#line 525 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 525 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 525 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 525 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )198;
#line 525 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 525 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 525 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 525 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 525 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 525 "pubShortMsg.cp"
}

#line 525 "pubShortMsg.cp"


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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 741 "pubShortMsg.cp"

		char vModelPattern[1024+1];
		char vSendTime[17+1];
		char vUpwardAccept[14+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 745 "pubShortMsg.cp"

	int ret;

	if(L == NULL)
	{
		PUBLOG_APPERR(__func__,"-1","ShortMsg Interpreter is not constructed!");
		return -1;
	}

	Sinitn(vModelPattern);
	Sinitn(vSendTime);
	Sinitn(vUpwardAccept);

	/* EXEC SQL Select replace(model_pattern,'||','..')
		Into :vModelPattern
		From sSMModel a
		Where sm_model_id = :iModelId; */ 
#line 761 "pubShortMsg.cp"

{
#line 758 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 758 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 758 "pubShortMsg.cp"
 sqlstm.arrsiz = 2;
#line 758 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 758 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 758 "pubShortMsg.cp"
 sqlstm.stmt = "select replace(model_pattern,'||','..') into :b0  from sSMM\
odel a where sm_model_id=:b1";
#line 758 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 758 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )213;
#line 758 "pubShortMsg.cp"
 sqlstm.selerr = (unsigned short)1;
#line 758 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 758 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 758 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vModelPattern;
#line 758 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )1025;
#line 758 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqhstv[1] = (unsigned char  *)&iModelId;
#line 758 "pubShortMsg.cp"
 sqlstm.sqhstl[1] = (unsigned long )sizeof(int);
#line 758 "pubShortMsg.cp"
 sqlstm.sqhsts[1] = (         int  )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqindv[1] = (         short *)0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqinds[1] = (         int  )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 758 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 758 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 758 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 758 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 758 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 758 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 758 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 758 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 758 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 758 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 758 "pubShortMsg.cp"
}

#line 761 "pubShortMsg.cp"


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
 *@ �������������ݶ���ģ��ͨ�����ü����Ի����������������ݲ��뵽���Žӿڱ�
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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 881 "pubShortMsg.cp"

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
	/* EXEC SQL END DECLARE SECTION; */ 
#line 909 "pubShortMsg.cp"


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

	/* EXEC SQL Select default_sm_port, default_sm_type, msg_level,
			seq_name, fold_policy, exp_policy, duration,
			req_upward, send_time_policy, send_time_offset,
			work_group
		Into :vDefaultSmPort, :vDefaultSmType, :vMsgLevel,
			:vSeqName, :vFoldPolicy, :vExpPolicy, :vDuration,
			:vReqUpward, :vSendTimePolicy, :vSendTimeOffset,
			:vWorkGroup
		From sSMModel a
		Where sm_model_id = :iModelId; */ 
#line 940 "pubShortMsg.cp"

{
#line 931 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 931 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 931 "pubShortMsg.cp"
 sqlstm.arrsiz = 12;
#line 931 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 931 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 931 "pubShortMsg.cp"
 sqlstm.stmt = "select default_sm_port ,default_sm_type ,msg_level ,seq_nam\
e ,fold_policy ,exp_policy ,duration ,req_upward ,send_time_policy ,send_time\
_offset ,work_group into :b0,:b1,:b2,:b3,:b4,:b5,:b6,:b7,:b8,:b9,:b10  from s\
SMModel a where sm_model_id=:b11";
#line 931 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 931 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )236;
#line 931 "pubShortMsg.cp"
 sqlstm.selerr = (unsigned short)1;
#line 931 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 931 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 931 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vDefaultSmPort;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )11;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[1] = (unsigned char  *)vDefaultSmType;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[1] = (unsigned long )6;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[1] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[1] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[1] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[2] = (unsigned char  *)&vMsgLevel;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[2] = (unsigned long )sizeof(int);
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[2] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[2] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[2] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[3] = (unsigned char  *)vSeqName;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[3] = (unsigned long )71;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[3] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[3] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[3] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[3] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[3] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[3] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[4] = (unsigned char  *)vFoldPolicy;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[4] = (unsigned long )2;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[4] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[4] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[4] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[4] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[4] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[4] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[5] = (unsigned char  *)vExpPolicy;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[5] = (unsigned long )2;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[5] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[5] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[5] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[5] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[5] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[5] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[6] = (unsigned char  *)&vDuration;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[6] = (unsigned long )sizeof(long);
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[6] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[6] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[6] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[6] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[6] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[6] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[7] = (unsigned char  *)vReqUpward;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[7] = (unsigned long )2;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[7] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[7] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[7] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[7] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[7] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[7] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[8] = (unsigned char  *)vSendTimePolicy;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[8] = (unsigned long )2;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[8] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[8] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[8] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[8] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[8] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[8] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[9] = (unsigned char  *)&vSendTimeOffset;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[9] = (unsigned long )sizeof(long);
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[9] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[9] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[9] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[9] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[9] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[9] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[10] = (unsigned char  *)vWorkGroup;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[10] = (unsigned long )21;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[10] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[10] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[10] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[10] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[10] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[10] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstv[11] = (unsigned char  *)&iModelId;
#line 931 "pubShortMsg.cp"
 sqlstm.sqhstl[11] = (unsigned long )sizeof(int);
#line 931 "pubShortMsg.cp"
 sqlstm.sqhsts[11] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqindv[11] = (         short *)0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqinds[11] = (         int  )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqharm[11] = (unsigned long )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqadto[11] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqtdso[11] = (unsigned short )0;
#line 931 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 931 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 931 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 931 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 931 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 931 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 931 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 931 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 931 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 931 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 931 "pubShortMsg.cp"
}

#line 940 "pubShortMsg.cp"


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

	/*�������ʹ������������۵�����*/
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
				
				/* EXEC SQL Select nvl(substr(:vShortMsgTmp,1,70),' ') 
					Into :vShortMsg from dual; */ 
#line 1129 "pubShortMsg.cp"

{
#line 1128 "pubShortMsg.cp"
    struct sqlexd sqlstm;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqlvsn = 12;
#line 1128 "pubShortMsg.cp"
    sqlstm.arrsiz = 12;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqladtp = &sqladt;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqltdsp = &sqltds;
#line 1128 "pubShortMsg.cp"
    sqlstm.stmt = "select nvl(substr(:b0,1,70),' ') into :b1  from dual ";
#line 1128 "pubShortMsg.cp"
    sqlstm.iters = (unsigned int  )1;
#line 1128 "pubShortMsg.cp"
    sqlstm.offset = (unsigned int  )299;
#line 1128 "pubShortMsg.cp"
    sqlstm.selerr = (unsigned short)1;
#line 1128 "pubShortMsg.cp"
    sqlstm.cud = sqlcud0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqlety = (unsigned short)256;
#line 1128 "pubShortMsg.cp"
    sqlstm.occurs = (unsigned int  )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhstv[0] = (unsigned char  *)vShortMsgTmp;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhstl[0] = (unsigned long )1025;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhsts[0] = (         int  )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqindv[0] = (         short *)0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqinds[0] = (         int  )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqharm[0] = (unsigned long )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqadto[0] = (unsigned short )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqtdso[0] = (unsigned short )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhstv[1] = (unsigned char  *)vShortMsg;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhstl[1] = (unsigned long )1001;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqhsts[1] = (         int  )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqindv[1] = (         short *)0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqinds[1] = (         int  )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqharm[1] = (unsigned long )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqadto[1] = (unsigned short )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqtdso[1] = (unsigned short )0;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqphsv = sqlstm.sqhstv;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqphsl = sqlstm.sqhstl;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqphss = sqlstm.sqhsts;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqpind = sqlstm.sqindv;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqpins = sqlstm.sqinds;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqparm = sqlstm.sqharm;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqparc = sqlstm.sqharc;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqpadto = sqlstm.sqadto;
#line 1128 "pubShortMsg.cp"
    sqlstm.sqptdso = sqlstm.sqtdso;
#line 1128 "pubShortMsg.cp"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1128 "pubShortMsg.cp"
}

#line 1129 "pubShortMsg.cp"

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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 1283 "pubShortMsg.cp"

		char vTimeStr[17+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 1285 "pubShortMsg.cp"

	int ret;
	Sinitn(vTimeStr);
#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"POLICY","��opTime=[%s],POLICY=[%c],offset=[%ld]����ʱ��",iOpTime,policy,offset);
#endif
	switch(policy)
	{
		/*�̶�ʱ��*/
		case '2':
			/* EXEC SQL select to_char(to_date(:offset,'YYYYMMDDHH24MISS'),'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual; */ 
#line 1297 "pubShortMsg.cp"

{
#line 1295 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 1295 "pubShortMsg.cp"
   sqlstm.arrsiz = 12;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 1295 "pubShortMsg.cp"
   sqlstm.stmt = "select to_char(to_date(:b0,'YYYYMMDDHH24MISS'),'YYYYMMDD \
HH24:MI:SS') into :b1  from dual ";
#line 1295 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 1295 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )322;
#line 1295 "pubShortMsg.cp"
   sqlstm.selerr = (unsigned short)1;
#line 1295 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 1295 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhstv[0] = (unsigned char  *)&offset;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhsts[0] = (         int  )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqindv[0] = (         short *)0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqinds[0] = (         int  )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhstv[1] = (unsigned char  *)vTimeStr;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhstl[1] = (unsigned long )18;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqhsts[1] = (         int  )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqindv[1] = (         short *)0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqinds[1] = (         int  )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqpind = sqlstm.sqindv;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqpins = sqlstm.sqinds;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqparm = sqlstm.sqharm;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqparc = sqlstm.sqharc;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 1295 "pubShortMsg.cp"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 1295 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1295 "pubShortMsg.cp"
}

#line 1297 "pubShortMsg.cp"

			break;
		case '3':
			/* EXEC SQL select to_char(to_date(:iOpTime,'YYYYMMDD HH24:MI:SS') + (:offset/24), 'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual; */ 
#line 1302 "pubShortMsg.cp"

{
#line 1300 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 1300 "pubShortMsg.cp"
   sqlstm.arrsiz = 12;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 1300 "pubShortMsg.cp"
   sqlstm.stmt = "select to_char((to_date(:b0,'YYYYMMDD HH24:MI:SS')+(:b1/2\
4)),'YYYYMMDD HH24:MI:SS') into :b2  from dual ";
#line 1300 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 1300 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )345;
#line 1300 "pubShortMsg.cp"
   sqlstm.selerr = (unsigned short)1;
#line 1300 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 1300 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstv[0] = (unsigned char  *)iOpTime;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstl[0] = (unsigned long )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhsts[0] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqindv[0] = (         short *)0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqinds[0] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstv[1] = (unsigned char  *)&offset;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstl[1] = (unsigned long )sizeof(long);
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhsts[1] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqindv[1] = (         short *)0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqinds[1] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstv[2] = (unsigned char  *)vTimeStr;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhstl[2] = (unsigned long )18;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqhsts[2] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqindv[2] = (         short *)0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqinds[2] = (         int  )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqpind = sqlstm.sqindv;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqpins = sqlstm.sqinds;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqparm = sqlstm.sqharm;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqparc = sqlstm.sqharc;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 1300 "pubShortMsg.cp"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 1300 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1300 "pubShortMsg.cp"
}

#line 1302 "pubShortMsg.cp"

			break;
		case '4':
			/* EXEC SQL select to_char(add_months(to_date(:iOpTime,'YYYYMMDD HH24:MI:SS'),:offset), 'YYYYMMDD HH24:MI:SS') 
				Into :vTimeStr
				From dual; */ 
#line 1307 "pubShortMsg.cp"

{
#line 1305 "pubShortMsg.cp"
   struct sqlexd sqlstm;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqlvsn = 12;
#line 1305 "pubShortMsg.cp"
   sqlstm.arrsiz = 12;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqladtp = &sqladt;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqltdsp = &sqltds;
#line 1305 "pubShortMsg.cp"
   sqlstm.stmt = "select to_char(add_months(to_date(:b0,'YYYYMMDD HH24:MI:S\
S'),:b1),'YYYYMMDD HH24:MI:SS') into :b2  from dual ";
#line 1305 "pubShortMsg.cp"
   sqlstm.iters = (unsigned int  )1;
#line 1305 "pubShortMsg.cp"
   sqlstm.offset = (unsigned int  )372;
#line 1305 "pubShortMsg.cp"
   sqlstm.selerr = (unsigned short)1;
#line 1305 "pubShortMsg.cp"
   sqlstm.cud = sqlcud0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqlety = (unsigned short)256;
#line 1305 "pubShortMsg.cp"
   sqlstm.occurs = (unsigned int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstv[0] = (unsigned char  *)iOpTime;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstl[0] = (unsigned long )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhsts[0] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqindv[0] = (         short *)0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqinds[0] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstv[1] = (unsigned char  *)&offset;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstl[1] = (unsigned long )sizeof(long);
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhsts[1] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqindv[1] = (         short *)0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqinds[1] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstv[2] = (unsigned char  *)vTimeStr;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhstl[2] = (unsigned long )18;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqhsts[2] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqindv[2] = (         short *)0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqinds[2] = (         int  )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqpind = sqlstm.sqindv;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqpins = sqlstm.sqinds;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqparm = sqlstm.sqharm;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqparc = sqlstm.sqharc;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 1305 "pubShortMsg.cp"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 1305 "pubShortMsg.cp"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1305 "pubShortMsg.cp"
}

#line 1307 "pubShortMsg.cp"

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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 1348 "pubShortMsg.cp"

		char vChnTableName[70+1];  /**����ͨ����Ϣ��**/
		int vRandomRow = 0;
		int vCount = 0;
	/* EXEC SQL END DECLARE SECTION; */ 
#line 1352 "pubShortMsg.cp"



#ifdef	DEBUG
	PUBLOG_DEBUG(__func__,"DEBUG","BUSI_CODE = [%s]",iOpCode);
	PUBLOG_DEBUG(__func__,"DEBUG","SYS_LABEL = [%s]",SysLabel);
	PUBLOG_DEBUG(__func__,"DEBUG","iWorkGroup= [%s]",iWorkGroup);
#endif

	/* EXEC SQL Select count(*)
		Into :vCount
		From SSMCHANNEL a, SBUSISMRELATION b
		Where a.channel_id = b.channel_id
			And a.belong_sys = :SysLabel
			AND b.busi_code = :iOpCode; */ 
#line 1366 "pubShortMsg.cp"

{
#line 1361 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 1361 "pubShortMsg.cp"
 sqlstm.arrsiz = 12;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 1361 "pubShortMsg.cp"
 sqlstm.stmt = "select count(*)  into :b0  from SSMCHANNEL a ,SBUSISMRELATI\
ON b where ((a.channel_id=b.channel_id and a.belong_sys=:b1) and b.busi_code=\
:b2)";
#line 1361 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 1361 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )399;
#line 1361 "pubShortMsg.cp"
 sqlstm.selerr = (unsigned short)1;
#line 1361 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 1361 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)&vCount;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstv[1] = (unsigned char  *)SysLabel;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstl[1] = (unsigned long )100;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhsts[1] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqindv[1] = (         short *)0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqinds[1] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstv[2] = (unsigned char  *)iOpCode;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhstl[2] = (unsigned long )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqhsts[2] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqindv[2] = (         short *)0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqinds[2] = (         int  )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 1361 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 1361 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1361 "pubShortMsg.cp"
}

#line 1366 "pubShortMsg.cp"

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","��ѯ����ͨ������ʧ��");
		return -1;
	}
	
	if(vCount == 0)  /*û��ָ��ר��ͨ��*/
	{
		/* EXEC SQL select count (*)
			Into :vCount
			FROM ssmchannel
			Where belong_sys = :SysLabel
				And work_group = :iWorkGroup; */ 
#line 1379 "pubShortMsg.cp"

{
#line 1375 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 1375 "pubShortMsg.cp"
  sqlstm.arrsiz = 12;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 1375 "pubShortMsg.cp"
  sqlstm.stmt = "select count(*)  into :b0  from ssmchannel where (belong_s\
ys=:b1 and work_group=:b2)";
#line 1375 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 1375 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )426;
#line 1375 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 1375 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 1375 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)&vCount;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstv[1] = (unsigned char  *)SysLabel;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstl[1] = (unsigned long )100;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhsts[1] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqindv[1] = (         short *)0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqinds[1] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstv[2] = (unsigned char  *)iWorkGroup;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhstl[2] = (unsigned long )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqhsts[2] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqindv[2] = (         short *)0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqinds[2] = (         int  )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 1375 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 1375 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1375 "pubShortMsg.cp"
}

#line 1379 "pubShortMsg.cp"

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

		/* EXEC SQL SELECT channel_tab_name
			Into :vChnTableName
			From (SELECT ROWNUM rnm, a.* From ssmchannel a WHERE belong_sys = :SysLabel And work_group = :iWorkGroup) b
			WHERE b.rnm = :vRandomRow
				And belong_sys = :SysLabel
				And work_group = :iWorkGroup; */ 
#line 1404 "pubShortMsg.cp"

{
#line 1399 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 1399 "pubShortMsg.cp"
  sqlstm.arrsiz = 12;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 1399 "pubShortMsg.cp"
  sqlstm.stmt = "select channel_tab_name into :b0  from (select ROWNUM rnm \
,a.*  from ssmchannel a where (belong_sys=:b1 and work_group=:b2)) b where ((\
b.rnm=:b3 and belong_sys=:b1) and work_group=:b2)";
#line 1399 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 1399 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )453;
#line 1399 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 1399 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 1399 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)vChnTableName;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )71;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[1] = (unsigned char  *)SysLabel;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[1] = (unsigned long )100;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[1] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[1] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[1] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[2] = (unsigned char  *)iWorkGroup;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[2] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[2] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[2] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[2] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[3] = (unsigned char  *)&vRandomRow;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[3] = (unsigned long )sizeof(int);
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[3] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[3] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[3] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[3] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[3] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[3] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[4] = (unsigned char  *)SysLabel;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[4] = (unsigned long )100;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[4] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[4] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[4] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[4] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[4] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[4] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstv[5] = (unsigned char  *)iWorkGroup;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhstl[5] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqhsts[5] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqindv[5] = (         short *)0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqinds[5] = (         int  )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqharm[5] = (unsigned long )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqadto[5] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqtdso[5] = (unsigned short )0;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 1399 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 1399 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1399 "pubShortMsg.cp"
}

#line 1404 "pubShortMsg.cp"

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
		/* EXEC SQL Select a.channel_tab_name
			Into :vChnTableName
			From SSMCHANNEL a, SBUSISMRELATION b
			Where a.channel_id = b.channel_id
				And a.belong_sys = :SysLabel
				AND b.busi_code = :iOpCode; */ 
#line 1425 "pubShortMsg.cp"

{
#line 1420 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 1420 "pubShortMsg.cp"
  sqlstm.arrsiz = 12;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 1420 "pubShortMsg.cp"
  sqlstm.stmt = "select a.channel_tab_name into :b0  from SSMCHANNEL a ,SBU\
SISMRELATION b where ((a.channel_id=b.channel_id and a.belong_sys=:b1) and b.\
busi_code=:b2)";
#line 1420 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 1420 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )492;
#line 1420 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 1420 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 1420 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)vChnTableName;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )71;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstv[1] = (unsigned char  *)SysLabel;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstl[1] = (unsigned long )100;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhsts[1] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqindv[1] = (         short *)0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqinds[1] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstv[2] = (unsigned char  *)iOpCode;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhstl[2] = (unsigned long )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqhsts[2] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqindv[2] = (         short *)0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqinds[2] = (         int  )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 1420 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 1420 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1420 "pubShortMsg.cp"
}

#line 1425 "pubShortMsg.cp"

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

		/* EXEC SQL SELECT channel_tab_name
			Into :vChnTableName
			From (Select ROWNUM rnm, a.*
				From ssmchannel a, sbusismrelation b
				Where a.channel_id = b.channel_id
					And b.busi_code = :iOpCode
					And a.belong_sys = :SysLabel
				) c
			Where c.rnm = :vRandomRow; */ 
#line 1451 "pubShortMsg.cp"

{
#line 1443 "pubShortMsg.cp"
  struct sqlexd sqlstm;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqlvsn = 12;
#line 1443 "pubShortMsg.cp"
  sqlstm.arrsiz = 12;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqladtp = &sqladt;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqltdsp = &sqltds;
#line 1443 "pubShortMsg.cp"
  sqlstm.stmt = "select channel_tab_name into :b0  from (select ROWNUM rnm \
,a.*  from ssmchannel a ,sbusismrelation b where ((a.channel_id=b.channel_id \
and b.busi_code=:b1) and a.belong_sys=:b2)) c where c.rnm=:b3";
#line 1443 "pubShortMsg.cp"
  sqlstm.iters = (unsigned int  )1;
#line 1443 "pubShortMsg.cp"
  sqlstm.offset = (unsigned int  )519;
#line 1443 "pubShortMsg.cp"
  sqlstm.selerr = (unsigned short)1;
#line 1443 "pubShortMsg.cp"
  sqlstm.cud = sqlcud0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqlety = (unsigned short)256;
#line 1443 "pubShortMsg.cp"
  sqlstm.occurs = (unsigned int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstv[0] = (unsigned char  *)vChnTableName;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstl[0] = (unsigned long )71;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhsts[0] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqindv[0] = (         short *)0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqinds[0] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstv[1] = (unsigned char  *)iOpCode;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstl[1] = (unsigned long )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhsts[1] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqindv[1] = (         short *)0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqinds[1] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstv[2] = (unsigned char  *)SysLabel;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstl[2] = (unsigned long )100;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhsts[2] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqindv[2] = (         short *)0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqinds[2] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstv[3] = (unsigned char  *)&vRandomRow;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhstl[3] = (unsigned long )sizeof(int);
#line 1443 "pubShortMsg.cp"
  sqlstm.sqhsts[3] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqindv[3] = (         short *)0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqinds[3] = (         int  )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqharm[3] = (unsigned long )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqadto[3] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqtdso[3] = (unsigned short )0;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqpind = sqlstm.sqindv;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqpins = sqlstm.sqinds;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqparm = sqlstm.sqharm;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqparc = sqlstm.sqharc;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 1443 "pubShortMsg.cp"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 1443 "pubShortMsg.cp"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1443 "pubShortMsg.cp"
}

#line 1451 "pubShortMsg.cp"

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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 1495 "pubShortMsg.cp"

		char vSqlStr[511+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 1497 "pubShortMsg.cp"

	Sinitn(vSqlStr);
	
	sprintf(vSqlStr,"Delete From %s "
					"Where sm_model_id = :v1 "
					" and phone_no = :v2 "
					" and busi_code = :v3 ",
					iChnTableName);
	/* EXEC SQL PREPARE Pre_SM_Del FROM :vSqlStr; */ 
#line 1505 "pubShortMsg.cp"

{
#line 1505 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 1505 "pubShortMsg.cp"
 sqlstm.arrsiz = 12;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 1505 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 1505 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 1505 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )550;
#line 1505 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 1505 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vSqlStr;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )512;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 1505 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 1505 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1505 "pubShortMsg.cp"
}

#line 1505 "pubShortMsg.cp"

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-3","ɾ��ͬ�����ʧ��!");
		return -3;
	}
	/* EXEC SQL EXECUTE Pre_SM_Del using :iModelId,:iRecvPhoneNo,:iSendTime; */ 
#line 1511 "pubShortMsg.cp"

{
#line 1511 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 1511 "pubShortMsg.cp"
 sqlstm.arrsiz = 12;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 1511 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 1511 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 1511 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )569;
#line 1511 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 1511 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)&iModelId;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstv[1] = (unsigned char  *)iRecvPhoneNo;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstl[1] = (unsigned long )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhsts[1] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqindv[1] = (         short *)0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqinds[1] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstv[2] = (unsigned char  *)iSendTime;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhstl[2] = (unsigned long )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqhsts[2] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqindv[2] = (         short *)0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqinds[2] = (         int  )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 1511 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 1511 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1511 "pubShortMsg.cp"
}

#line 1511 "pubShortMsg.cp"

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
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 1567 "pubShortMsg.cp"

		char vSqlStr[511+1];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 1569 "pubShortMsg.cp"


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

	/* EXEC SQL PREPARE Pre_SM_Send FROM :vSqlStr; */ 
#line 1598 "pubShortMsg.cp"

{
#line 1598 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 1598 "pubShortMsg.cp"
 sqlstm.arrsiz = 12;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 1598 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 1598 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 1598 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )596;
#line 1598 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 1598 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)vSqlStr;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )512;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 1598 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 1598 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1598 "pubShortMsg.cp"
}

#line 1598 "pubShortMsg.cp"

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-1","�������ʵ���ӿڱ�ʧ��!");
		return -1;
	}
	/* EXEC SQL EXECUTE Pre_SM_Send using :iModelId,:iLoginAccept,:iMsgLevel,
					:iSmPort,:iSmType,:iRecvPhoneNo,:iShortMsg,
					:iShortMsgAdd,:iOrderCode,:iLoginNo,:iOpCode,:iOpTime,
					:iSendTime,:iExpTime; */ 
#line 1607 "pubShortMsg.cp"

{
#line 1604 "pubShortMsg.cp"
 struct sqlexd sqlstm;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqlvsn = 12;
#line 1604 "pubShortMsg.cp"
 sqlstm.arrsiz = 14;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqladtp = &sqladt;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqltdsp = &sqltds;
#line 1604 "pubShortMsg.cp"
 sqlstm.stmt = "";
#line 1604 "pubShortMsg.cp"
 sqlstm.iters = (unsigned int  )1;
#line 1604 "pubShortMsg.cp"
 sqlstm.offset = (unsigned int  )615;
#line 1604 "pubShortMsg.cp"
 sqlstm.cud = sqlcud0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqlety = (unsigned short)256;
#line 1604 "pubShortMsg.cp"
 sqlstm.occurs = (unsigned int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[0] = (unsigned char  *)&iModelId;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[0] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[0] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[0] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[1] = (unsigned char  *)&iLoginAccept;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[1] = (unsigned long )sizeof(long);
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[1] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[1] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[1] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[2] = (unsigned char  *)&iMsgLevel;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[2] = (unsigned long )sizeof(int);
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[2] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[2] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[2] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[3] = (unsigned char  *)iSmPort;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[3] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[3] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[3] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[3] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[3] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[3] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[3] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[4] = (unsigned char  *)iSmType;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[4] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[4] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[4] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[4] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[4] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[4] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[4] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[5] = (unsigned char  *)iRecvPhoneNo;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[5] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[5] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[5] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[5] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[5] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[5] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[5] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[6] = (unsigned char  *)iShortMsg;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[6] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[6] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[6] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[6] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[6] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[6] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[6] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[7] = (unsigned char  *)iShortMsgAdd;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[7] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[7] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[7] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[7] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[7] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[7] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[7] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[8] = (unsigned char  *)&iOrderCode;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[8] = (unsigned long )sizeof(int);
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[8] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[8] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[8] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[8] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[8] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[8] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[9] = (unsigned char  *)iLoginNo;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[9] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[9] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[9] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[9] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[9] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[9] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[9] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[10] = (unsigned char  *)iOpCode;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[10] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[10] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[10] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[10] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[10] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[10] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[10] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[11] = (unsigned char  *)iOpTime;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[11] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[11] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[11] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[11] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[11] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[11] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[11] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[12] = (unsigned char  *)iSendTime;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[12] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[12] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[12] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[12] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[12] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[12] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[12] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstv[13] = (unsigned char  *)iExpTime;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhstl[13] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqhsts[13] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqindv[13] = (         short *)0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqinds[13] = (         int  )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqharm[13] = (unsigned long )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqadto[13] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqtdso[13] = (unsigned short )0;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqpind = sqlstm.sqindv;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqpins = sqlstm.sqinds;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqparm = sqlstm.sqharm;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqparc = sqlstm.sqharc;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 1604 "pubShortMsg.cp"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 1604 "pubShortMsg.cp"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 1604 "pubShortMsg.cp"
}

#line 1607 "pubShortMsg.cp"

	if(SQLCODE != 0)
	{
		PUBLOG_DBERR(__func__,"-2","�������ʵ���ӿڱ�ʧ��!");
		return -2;
	}
	return 0;
}

