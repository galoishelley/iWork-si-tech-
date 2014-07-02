
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
           char  filnam[6];
};
static const struct sqlcxp sqlfpn =
{
    5,
    "db.pc"
};


static unsigned int sqlctx = 2027;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
      const short *cud;
   unsigned char  *sqlest;
      const char  *stmt;
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
   unsigned char  *sqhstv[4];
   unsigned long  sqhstl[4];
            int   sqhsts[4];
            short *sqindv[4];
            int   sqinds[4];
   unsigned long  sqharm[4];
   unsigned long  *sqharc[4];
   unsigned short  sqadto[4];
   unsigned short  sqtdso[4];
} sqlstm = {12,4};

// Prototypes
extern "C" {
  void sqlcxt (void **, unsigned int *,
               struct sqlexd *, const struct sqlcxp *);
  void sqlcx2t(void **, unsigned int *,
               struct sqlexd *, const struct sqlcxp *);
  void sqlbuft(void **, char *);
  void sqlgs2t(void **, char *);
  void sqlorat(void **, unsigned int *, void *);
}

// Forms Interface
static const int IAPSUCC = 0;
static const int IAPFAIL = 1403;
static const int IAPFTL  = 535;
extern "C" { void sqliem(char *, int *); }

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{12,4130,852,8,0,
5,0,0,1,0,0,27,34,0,0,4,4,0,1,0,1,97,0,0,1,97,0,0,1,97,0,0,1,10,0,0,
36,0,0,2,0,0,30,45,0,0,0,0,0,1,0,
};


#line 1 "db.pc"
#include <stdio.h>
#include <string.h>
#include "oci.h"
#include "ltypedef.h"
#include "db.h"

#ifdef SQLCA_STORAGE_CLASS
#undef SQLCA_STORAGE_CLASS
#endif

/**
 * 在此处生成SQLCA的实体。
 */
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

#line 15 "db.pc"

#define SQLCA_STORAGE_CLASS extern

ETYPE ProcDBLogin(const char* m_sDataSource, const char* m_sUserId, const char* m_sPasswd)
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 20 "db.pc"

	char this_user[30];
	char this_pwd[MAXENDELEN+1];
	char this_srv[30];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 24 "db.pc"

	
	memset(this_user, 0, sizeof(this_user));
	memset(this_pwd, 0, sizeof(this_pwd));
	memset(this_srv, 0, sizeof(this_srv));
	
	strcpy(this_user, m_sUserId);
	strcpy(this_pwd, m_sPasswd);
	strcpy(this_srv, m_sDataSource);

	/* EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd using :this_srv; */ 
#line 34 "db.pc"

{
#line 34 "db.pc"
 struct sqlexd sqlstm;
#line 34 "db.pc"
 sqlstm.sqlvsn = 12;
#line 34 "db.pc"
 sqlstm.arrsiz = 4;
#line 34 "db.pc"
 sqlstm.sqladtp = &sqladt;
#line 34 "db.pc"
 sqlstm.sqltdsp = &sqltds;
#line 34 "db.pc"
 sqlstm.iters = (unsigned int  )10;
#line 34 "db.pc"
 sqlstm.offset = (unsigned int  )5;
#line 34 "db.pc"
 sqlstm.cud = sqlcud0;
#line 34 "db.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 34 "db.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 34 "db.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 34 "db.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)this_user;
#line 34 "db.pc"
 sqlstm.sqhstl[0] = (unsigned long )30;
#line 34 "db.pc"
 sqlstm.sqhsts[0] = (         int  )30;
#line 34 "db.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 34 "db.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 34 "db.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 34 "db.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqhstv[1] = (unsigned char  *)this_pwd;
#line 34 "db.pc"
 sqlstm.sqhstl[1] = (unsigned long )33;
#line 34 "db.pc"
 sqlstm.sqhsts[1] = (         int  )33;
#line 34 "db.pc"
 sqlstm.sqindv[1] = (         short *)0;
#line 34 "db.pc"
 sqlstm.sqinds[1] = (         int  )0;
#line 34 "db.pc"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 34 "db.pc"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqhstv[2] = (unsigned char  *)this_srv;
#line 34 "db.pc"
 sqlstm.sqhstl[2] = (unsigned long )30;
#line 34 "db.pc"
 sqlstm.sqhsts[2] = (         int  )30;
#line 34 "db.pc"
 sqlstm.sqindv[2] = (         short *)0;
#line 34 "db.pc"
 sqlstm.sqinds[2] = (         int  )0;
#line 34 "db.pc"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 34 "db.pc"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 34 "db.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 34 "db.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 34 "db.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 34 "db.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 34 "db.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 34 "db.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 34 "db.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 34 "db.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 34 "db.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 34 "db.pc"
 sqlstm.sqlcmax = (unsigned int )100;
#line 34 "db.pc"
 sqlstm.sqlcmin = (unsigned int )2;
#line 34 "db.pc"
 sqlstm.sqlcincr = (unsigned int )1;
#line 34 "db.pc"
 sqlstm.sqlctimeout = (unsigned int )0;
#line 34 "db.pc"
 sqlstm.sqlcnowait = (unsigned int )0;
#line 34 "db.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 34 "db.pc"
}

#line 34 "db.pc"

	if (SQLCODE != 0)
	{
		return 0x00001015;
	}
	return 0;
}


void ProcDBClose()
{
	/* EXEC SQL COMMIT WORK RELEASE; */ 
#line 45 "db.pc"

{
#line 45 "db.pc"
 struct sqlexd sqlstm;
#line 45 "db.pc"
 sqlstm.sqlvsn = 12;
#line 45 "db.pc"
 sqlstm.arrsiz = 4;
#line 45 "db.pc"
 sqlstm.sqladtp = &sqladt;
#line 45 "db.pc"
 sqlstm.sqltdsp = &sqltds;
#line 45 "db.pc"
 sqlstm.iters = (unsigned int  )1;
#line 45 "db.pc"
 sqlstm.offset = (unsigned int  )36;
#line 45 "db.pc"
 sqlstm.cud = sqlcud0;
#line 45 "db.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 45 "db.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 45 "db.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 45 "db.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 45 "db.pc"
}

#line 45 "db.pc"

}
