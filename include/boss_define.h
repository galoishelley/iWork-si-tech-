#ifndef __BOSS_DEFINE_H__
#define __BOSS_DEFINE_H__

#define SM_LIST_FILE "sm_list.cfg"

#define DBOPEN 		"dbopen"
#define DBCHANGE 	"dbchange"
#define DBOFFON 	"offonrpt"
#define BOSSBILL 	"bossbill"
#define DBQUERY 	"dbquery"
#define DBSIMC	 	"simc"
#define DIFFLABEL "dblocal"
#define CONNECT0 	"0"
#define NOTFOUND	1403
#define init(a) memset(a, 0, sizeof(a))
#define MAX_COLUMNS             50
#define MAX_FIELD_LEN           120
#define OK		0
#define MAXFUNCTIONNUM		50
#define SQLERRD		sqlca.sqlerrd[2]
#define SQLERRMSG	sqlca.sqlerrm.sqlerrmc
#define ERRNOLEN	4
#define ERRMSGLEN	60
#define MAXFUNC		100	
#define FUNCLISTLEN	1000
#define MAXNUMOFFAVOURTYPE	20

#define TEMPLEN          255
#define MAXTINYINTLEN		3
#define MAXSMALLINTLEN		6
#define MAXINTLEN			11
#define MAXSMALLMONEYLEN	12
#define MAXMONEYLEN			21
#define MAXDATETIMELEN		40
#define MAXSMALLDATETIMELEN  17 
#define CONNECT0         "0" 
#define CONNECT1         "1" 
#define InitVar(a) memset(a, 0, sizeof(a))
#define ROWNUM  1000
#define COLNUM  100


/*费用代码优化*/
#define FEE_CODE_MONTH			"0a"
#define FEE_CODE_FUNCTION		"0b"
#define FEE_CODE_LOCAL_NET		"0c"
#define FEE_CODE_LOCAL_TOLL		"0d"
#define FEE_CODE_INLAND_TOLL	"0e"
#define FEE_CODE_INTER_TOLL		"0f"
#define FEE_CODE_ROAM_TOLL		"0g"
#define FEE_CODE_LOCAL_ROAM		"0h"
#define FEE_CODE_INLAND_ROAM	"0i"
#define FEE_CODE_INTER_ROAM		"0j"
#define FEE_CODE_HMT_TOLL		"0k"
#define FEE_CODE_IP				"0l"
#define FEE_CODE_COUNTRY		"0m"
#define FEE_CODE_MESSAGE		"0n"
#define FEE_CODE_OCC			"0o"
#define FEE_CODE_FQ_NET			"0p"
#define FEE_CODE_LAST_SHOULD	"0q"
#define FEE_CODE_RENT			"0r"
#define FEE_CODE_GPRS			"0s"
#define FEE_CODE_CREDIT                 "0t"
#define FEE_CODE_HMT			"0u"
#endif /*__BOSS_DEFINE_H__*/
