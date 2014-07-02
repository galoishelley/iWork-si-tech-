
/*
** exutils.h
**
** Header file which contains the defines and prototypes for the utility
** functions in exutils.c
** 
*/

/* Sccsid %Z% %M% %I% %G% */

/*****************************************************************************
** 
** defines and typedefs used 
** 
*****************************************************************************/

#ifndef MAX
#define MAX(X,Y)	(((X) > (Y)) ? (X) : (Y))
#endif

#ifndef MIN
#define MIN(X,Y)	(((X) < (Y)) ? (X) : (Y))
#endif

/*
** Maximum character buffer for displaying a column
*/
#define MAX_CHAR_BUF	255 /*1024*/
#define MAX_RECORDS     120
#define MAX_FRECORDS    120
#define MAX_COLUMNS     32 
#define MAX_PARAMETER   10
#define MAX_BCPROWS     100
#define MAX_RECORD_LEN  1024
#define         TRUE     1
#define         FALSE    0
/*
void s_debug(va_alist)
va_dcl
*/
/***
int
errlog( const char *errmsg, const char *file, const int line);
***/

#define         RETURN_OK            "0"
#define         LOGIN_ON             "0"
#define         LOGIN_OFF            "1"
#define         ENABLE               "0"
#define         DISABLE              "1"

#define         B_SPACE               '1'
#define         F_SPACE               '2'
#define         F_ZERO                '3'

int
format_data(char * dec,char * src,int len,char sign);

