/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/******************************************
 *      Prg:  publicDb.cp
 *     Edit:  guo.yj
 * Modi/Add:  zhaohao  2001.10.31
 *     Date:  2001.02.02
 ******************************************/

#define DLMAXITEMS 150
#define DLINTERFACEDATA 4001

#include "boss_srv.h"
#include "publicsrv.h"
#include "publicEmer.h"

#define WORKDIR "WORKDIR"
extern int errno;
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;
/*局部变量定义*/
static SQLDA *bind_dp;
static char last_conn_name[40];


/*
 *  function of share memory  option
 *  Crea: Guo.yj
 *  Modi: zhaohao
 *  Date: 2001.11.06
 */
/*************数据库操作*****************/

/******************************
	登陆数据库
	支持切换应急数据库
	yinyx 2010-5-10 22:12:45
******************************/
int spublicDBLoginEmer(FBFR32 *transIN, FBFR32 *transOUT, char *srvName, 
		char *connect_name, sPublicDBCfg *dbCfg)
{
	/*如果传入dbCfg的值为空,或者明确标识非应急状态,则直接连接生长库*/
	if( (dbCfg == NULL) || (dbCfg->bExists == 1 && dbCfg->emFlag[0] != 'Y'))
	{
		return spublicDBLogin(transIN, transOUT, srvName, connect_name);
	}
	
	/*如果结构体为空则直接取共享内存中的的数据库连接*/
	if(1 != dbCfg->bExists)
	{
		/*取不到共享内存时直接连接生产*/
		if(0 != getDbCfg(srvName, dbCfg))
		{
			return spublicDBLogin(transIN, transOUT, srvName, connect_name);
		}
	}
	
	/*应急状态标识设置为Y,则连接应急数据库*/
	if(dbCfg->emFlag[0] == 'Y' || dbCfg->emFlag[0] == 'y')
	{
		return spublicDBLogin(transIN, transOUT, srvName, dbCfg->dbLabel);
	}
	else/*非应急状态，连接生产库*/
	{
		return spublicDBLogin(transIN, transOUT, srvName, connect_name);
	}
}

/***********************************
	检查数据库连接是否正常
	支持应急切换数据库 
	yinyx 2010-5-10 21:43:07
***********************************/
int sChkDBLoginEmer(FBFR32 *transIN, FBFR32 *transOUT, char *srvName, char *connect_name, sPublicDBCfg *dbCfg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int  val = 0;
		int  emerFlag = -1;/*连接应急标志,1应急,0生产,-1未知*/
	EXEC SQL END DECLARE SECTION;
	
	if(dbCfg == NULL)
	{
		emerFlag = 0;
	}
	
	/*如果结构体为空则直接取共享内存中的的数据库连接*/
	if((emerFlag == -1) && 1 != dbCfg->bExists)
	{
		/*取不到共享内存时直接连接生产*/
		if(0 != getDbCfg(srvName, dbCfg))
		{
			emerFlag = 0;
		}
	}
	
	/*取到共享内存，并且是应急状态*/
	if((emerFlag == -1) && (dbCfg->emFlag[0] == 'Y' || dbCfg->emFlag[0] == 'y'))
	{
		emerFlag = 1;
	}
	else  /*非应急状态连接生产*/
	{
		emerFlag = 0;
	}
	
	if(0 == emerFlag)/*生产连接校验*/
	{
		if(strcmp(last_conn_name,connect_name) != 0)
		{
			PUBLOG_APPRUN(__func__,"CHANGE DB CONNECTION","[%s]--[%s]\n",last_conn_name,connect_name);
			spublicDBClose(last_conn_name);
			return spublicDBLogin(transIN, transOUT, srvName, connect_name);
		}
		else
		{
			return sChkDBLogin(transIN, transOUT, srvName, connect_name);
		}
	}
	else if(1 == emerFlag)/*应急连接校验*/
	{
		if(strcmp(last_conn_name,dbCfg->dbLabel) != 0)
		{
			PUBLOG_APPRUN(__func__,"CHANGE DB CONNECTION","[%s]--[%s]\n",last_conn_name,dbCfg->dbLabel);
			spublicDBClose(last_conn_name);
			return spublicDBLogin(transIN, transOUT, srvName, dbCfg->dbLabel);
		}
		else
		{
			return sChkDBLogin(transIN, transOUT, srvName, dbCfg->dbLabel);
		}
	}
	else /*未判断出连接哪一数据库*/
	{
		return -1;
	}
}

/*******************************/
/* name:     spublicDBCloseEmer    */
/* writer:   cencm           */
/* date:     2010.10.12        */
/* function: 断开与数据库的连接 支持应急切换数据库*/
/*******************************/
int spublicDBCloseEmer(char *connect_name,sPublicDBCfg *dbCfg)
{
	/*如果结构体为空则直接取共享内存中的的数据库连接*/
	if(1 != dbCfg->bExists)
	{
		return spublicDBClose(connect_name);
	}
	
	/*不支持应急就直接断开生产连接*/
	if(dbCfg->emFlag[0] != 'Y')
	{
		return spublicDBClose(connect_name);
	}
	/*否则断开应急数据库连接*/
	return spublicDBClose(dbCfg->dbLabel);
}

/*******************************/
/* name:     spublicDBLogin    */
/* writer:   zhaohao           */
/* date:     2001.02.09        */
/* function: 与数据库建立连接  */
/*******************************/
int spublicDBLogin(FBFR32 *transIN, FBFR32 *transOUT, char *srvName,char *connect_name)
{
	int  rtn_value;
	struct spublicdblogin sdblogin;
	
	EXEC SQL BEGIN DECLARE SECTION;
		char this_user[30];
		char this_pwd[MAXENDELEN+1];
		char this_db[30];
		char this_srv[30];
		char this_connect[30];
		char sqlstr[256];
	EXEC SQL END DECLARE SECTION;

	rtn_value = -1;

	/* 取得数据库的登陆参数 */
	sdblogin = spublicGetDBP(connect_name);
	if (sdblogin.label[0] == NULL)
	{
		pubLog_Debug(_FFL_,"spublicDBLogin"," ","Get db parameters fail!");
		rtn_value = -1;
		return rtn_value;
	}
	else
	{
		rtn_value = 0;
	}

	memset(sqlstr, 0, sizeof(sqlstr));
	memset(this_user, 0, sizeof(this_user));
	memset(this_pwd, 0, sizeof(this_pwd));
	memset(this_db, 0, sizeof(this_db));
	memset(this_srv, 0, sizeof(this_srv));
	memset(this_connect, 0, sizeof(this_connect));
	
	strcpy(this_user, sdblogin.username);
	strcpy(this_pwd,  sdblogin.password);
	strcpy(this_db,   sdblogin.database);
	strcpy(this_srv,  sdblogin.server);
	strcpy(this_connect,  connect_name);
	
	EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd using :this_srv;
	rtn_value = SQLCODE;

	if (rtn_value != 0)
	{
		rtn_value = -2;
		userlog("连接数据库参数错误!\n");
	}
	else
	{
		PUBLOG_APPRUN(__func__,"ASSIGN DB LABEL","[%s]\n",connect_name);
		memset(last_conn_name,0,sizeof(last_conn_name));
		strcpy(last_conn_name,connect_name);
		rtn_value=0;

	}

	return(rtn_value);
}

int sChkDBLogin(FBFR32 *transIN, FBFR32 *transOUT, char *srvName,char *connect_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int  val;
	EXEC SQL END DECLARE SECTION;
	EXEC SQL	SELECT 1
				  INTO :val
				  FROM dual;
	if (SQLCODE == 0)
	{
		return 0;
	}
	
	return spublicDBLogin(transIN, transOUT, srvName, connect_name);
}
 
/*******************************/
/* name:     spublicDBClose    */
/* writer:   zhaohao           */
/* date:     2001.02.09        */
/* function: 断开与数据库的连接*/
/*******************************/
int spublicDBClose(char *connect_name)
{
	int  rtn_value;

	/*清空数据库登录记录*/
	PUBLOG_APPRUN(__func__,"CLEAR DB LABEL","[%s]\n",last_conn_name);
	memset(last_conn_name,0,sizeof(last_conn_name));
	EXEC SQL COMMIT WORK RELEASE;

	rtn_value = SQLCODE;

	/***********/
	if (rtn_value < 0)
	{
		PUBLOG_DBERR(__func__,"ERR","断开数据库连接错误");
		userlog("断开数据库连接错误!\n");
	}
	
	return(rtn_value);
}
/*******************************/
/* name:     spublicDBLogin    */
/* writer:   zhaohao           */
/* date:     2001.02.09        */
/* function: 与数据库建立连接  */
/*******************************/
int spublicDBLogin2(FBFR32 *transIN, FBFR32 *transOUT, char *srvName,char *connect_name, char *atName)
{
	int  rtn_value;
	struct spublicdblogin sdblogin;
	
	EXEC SQL BEGIN DECLARE SECTION;
	char this_user[30];
	char this_pwd[MAXENDELEN+1];
	char this_db[30];
	char this_srv[30];
	char this_connect[30];
	char sqlstr[256];
	EXEC SQL END DECLARE SECTION;


	rtn_value = -1;

	/* 取得数据库的登陆参数 */
	sdblogin = spublicGetDBP(connect_name);
	if (sdblogin.label[0] == NULL)
	{
		pubLog_Debug(_FFL_,"spublicDBLogin"," ","Get db parameters fail!");
		rtn_value = -1;
	}
	else
	{
		rtn_value = 0;
	}

	memset(sqlstr, 0, sizeof(sqlstr));
	memset(this_user, 0, sizeof(this_user));
	memset(this_pwd, 0, sizeof(this_pwd));
	memset(this_db, 0, sizeof(this_db));
	memset(this_srv, 0, sizeof(this_srv));
	memset(this_connect, 0, sizeof(this_connect));
	
	strcpy(this_user, sdblogin.username);
	strcpy(this_pwd,  sdblogin.password);
	strcpy(this_db,   sdblogin.database);
	strcpy(this_srv,  sdblogin.server);
	strcpy(this_connect,  connect_name);



	EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd at :atName using :this_srv;
	rtn_value = SQLCODE;

	if (rtn_value < 0)
	{
		printf("连接数据库出错，SQLCODE=[%d],Conn=[%s],Server[%s], User=[%s], Db=[%s], atName[%s]\n",
					rtn_value,connect_name, this_srv, this_user, this_db,atName);
		rtn_value = -2;
		pubLog_DBErr(_FFL_,"spublicDBLogin","0001","链接数据库错误");
		userlog("连接数据库参数错误!\n");
	}
	else
	{
		rtn_value=0;

	}
	
	return(rtn_value);
}

int sChkDBLogin2(FBFR32 *transIN, FBFR32 *transOUT, char *srvName,char *connect_name,char *atName)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int  val;
	EXEC SQL END DECLARE SECTION;
	EXEC SQL at :atName SELECT 1 INTO :val FROM dual;
	if (SQLCODE == 0)
	{
		return 0;
	}
	
	PUBLOG_DBDEBUG("sChkDBLogin");
	return spublicDBLogin2(transIN, transOUT, srvName, connect_name, atName);
}

int spublicDBClose2(char *connect_name, char *atName)
{
        int  rtn_value;
        struct spublicdblogin sdblogin;

        EXEC SQL BEGIN DECLARE SECTION;
                char this_connect[30];
        EXEC SQL END DECLARE SECTION;

        rtn_value =0;

        memset(this_connect, 0, sizeof(this_connect));
        strcpy(this_connect, connect_name);

        sdblogin = spublicGetDBP(connect_name);
        if (sdblogin.label[0] == NULL) {
			rtn_value = 0;
        }
        else
        {
			EXEC SQL at :atName COMMIT WORK RELEASE;
		}
                


        rtn_value = SQLCODE;

        /*** nyc ***/
        rtn_value = 0;
        /***********/
        if (rtn_value < 0)
                userlog("断开数据库连接错误!\n");

        return(rtn_value);
}
/*******************************/
/* name:     spublicGenMAC     */
/* writer:   zhaohao           */
/* date:     2001.02.21        */
/* function: 根据给定的字段    */
/*           产生MAC校验       */
/*******************************/
int spublicGenMAC(char *MAINKEY, struct MacStruc *macstruc, char *out_MAC)
{
        char buf[1024+1];
        char tmp_buf1[512+1];
        char tmp_buf2[512+1];
        char tmp_buf3[512+1];
        char tmp_int[20+1];
        char tmp_float[30+1];
        char tmp_char[20+1];
        char MAC_bcd[8+1];
        int  use_int, use_float, use_char;
        int  rtn_value;

        memset(buf, 0, sizeof(buf));
        memset(tmp_buf1, 0, sizeof(tmp_buf1));
        memset(tmp_buf2, 0, sizeof(tmp_buf2));
        memset(tmp_buf3, 0, sizeof(tmp_buf3));

        if (MAINKEY == NULL) {
                MAINKEY=(char *)(malloc(8+1));
                memset(MAINKEY, 0, 8+1);
                strcpy(MAINKEY, "bossboss");
        }

        if (macstruc->use_int < 0 || macstruc->use_float < 0 || macstruc->use_char < 0 ) {
                rtn_value = -1;
                return rtn_value;
        }

        /* 整型 */
        for(use_int=0; use_int<macstruc->use_int; use_int++) {
                memset(tmp_int, 0, sizeof(tmp_int));
                sprintf(tmp_int, "%d", macstruc->srcint[use_int]);

                if (use_int == 0) {
                        strcpy(tmp_buf1, tmp_int);
                }
                else {
                        strcat(tmp_buf1, tmp_int);
                }
        }

        /* 实型 */
        for(use_float=0; use_float<macstruc->use_float; use_float++) {
                memset(tmp_float, 0, sizeof(tmp_float));
                if (macstruc->srcfloat[use_float] < 0) {
                        macstruc->srcfloat[use_float] = macstruc->srcfloat[use_float] * -1;
                }
                sprintf(tmp_float, "%.0f", macstruc->srcfloat[use_float]*100);

                if (use_float == 0) {
                        strcpy(tmp_buf2, tmp_float);
                }
                else {
                        strcat(tmp_buf2, tmp_float);
                }
        }

        /* 字符串 */
        for(use_char=0; use_char<macstruc->use_char; use_char++) {
                if (use_char == 0) {
                        strcpy(tmp_buf3, Trim(macstruc->srcchar[use_char]));
                }
                else {
                        strcat(tmp_buf3, Trim(macstruc->srcchar[use_char]));
                }
        }

        strcpy(buf, tmp_buf1);
        strcat(buf, tmp_buf2);
        strcat(buf, tmp_buf3);

        memset(MAC_bcd, 0 ,sizeof(MAC_bcd));

	/*
        GenerateMAC(MAC_STAND, buf, strlen(buf), MAINKEY, MAC_bcd);
        BcdToAsc_(out_MAC,MAC_bcd,16);
	*/

        if (MAINKEY != NULL)
                free(MAINKEY);

        rtn_value = 0;
        return rtn_value;
}


               
int spublicRight(char *connect_name, char *login_no, char *login_passwd, char *function_code)
{
        int  rtn_value;

        EXEC SQL BEGIN DECLARE SECTION;
            int  iCount = 0;
            char vRelaType[1+1];
            char rtn_function_code[6+1];
            char rtn_login_no[8+1];
            char rtn_login_passwd[16+1];
            char rtn_power_code[30+1];
            char this_connect[30];
            char tmp_code[4+1];
        EXEC SQL END DECLARE SECTION;

        rtn_value = 0;
        SQLCODE = 0;

        memset(rtn_function_code, 0, sizeof(rtn_function_code));
        memset(rtn_login_no, 0, sizeof(rtn_login_no));
        memset(rtn_login_passwd, 0, sizeof(rtn_login_passwd));
        memset(rtn_power_code, 0, sizeof(rtn_power_code));
        memset(this_connect, 0, sizeof(this_connect));

        strcpy(this_connect, connect_name);
        strcpy(rtn_login_no, login_no);
        strcpy(rtn_function_code, function_code);
#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "spublicRight", "工号", "login_no=[%s]", rtn_login_no);
     pubLog_Debug(_FFL_, "spublicRight", "密码", "pass_word=[%s]", login_passwd);
     pubLog_Debug(_FFL_, "spublicRight", "op_code", "op_code=[%s]", rtn_function_code);
#endif
        EXEC SQL SELECT password,power_code
                 INTO   :rtn_login_passwd,:rtn_power_code
                 FROM   dLoginMsg
                 WHERE  login_no=:rtn_login_no;
        if (SQLCODE == 1403) 
        {
            /* 输入工号有误,从数据库取不到记录 */
            PUBLOG_DBDEBUG("spublicRight");
			pubLog_DBErr(_FFL_, "spublicRight", "1000", "没有该工号信息");    
			rtn_value = 1000;
        }
        else if (SQLCODE == SQL_OK) 
        {
                /* 将从库中取出的营业员密码解密 */
                Trim(rtn_power_code);
                Trim(rtn_login_passwd);
                Trim(rtn_function_code);
                if (strcmp( rtn_login_passwd, login_passwd ) == 0) 
                {
                	init(vRelaType);
					EXEC SQL select rela_type into :vRelaType
					from sLoginPdomRelation a, sPopedomCode b
					where a.POPEDOM_CODE = b.POPEDOM_CODE
					and   a.login_no =  :rtn_login_no
					and   b.PDT_CODE in ('03','05')
					and   a.end_date  > sysdate
					and   b.REFLECT_CODE = :rtn_function_code ;  
					if(SQLCODE == 1403)
					{
						/* 检查角色和权限的关系 */
						iCount = 0;
						EXEC SQL select count(*) into :iCount 
							from sRolePdomRelation a, sLoginRoalRelation b,  sPopedomCode c
  							where  a.role_code = b.role_code 
							and    a.POPEDOM_CODE = c.POPEDOM_CODE
  							and    b.login_no = :rtn_login_no
							and    c.REFLECT_CODE = :rtn_function_code
  							and    b.end_date > sysdate;
  							if(iCount == 0)
  							{ 	
  								PUBLOG_DBDEBUG("spublicRight");
  								pubLog_Debug(_FFL_, "spublicRight","","iCount=[%d]",iCount);
								pubLog_DBErr(_FFL_, "spublicRight", "1001", "工号没有该权限");
  								rtn_value = 1001;
  							}
					} 
					else if (SQLCODE == SQL_OK)
					{
						/* 关系类型为反项控制 */
						if(vRelaType[0] == '1') 
						{ 	
							pubLog_DBErr(_FFL_, "spublicRight", "1002", "工号被强制去掉该权限");
							rtn_value = 1002; 
						}
					} 
					else 
					{
						/* 数据库执行失败 */
						PUBLOG_DBDEBUG("spublicRight");
						pubLog_DBErr(_FFL_, "spublicRight", "2001", "数据库执行失败");
                        rtn_value = 2001;
					}
                }
                else 
                {
                       /* 工号正确，密码输入错误 */
					   pubLog_Debug(_FFL_, "spublicRight","正确密码", "pass_word=[%s]",rtn_login_passwd);
					   pubLog_Debug(_FFL_, "spublicRight","输入密码", "pass_word=[%s]",login_passwd);
					   pubLog_DBErr(_FFL_, "spublicRight", "1200", "工号密码错误");
                       rtn_value = 1200;
                }
        }
        else if (SQLCODE < 0) 
        {
                /* 数据库执行失败 */
				PUBLOG_DBDEBUG("spublicRight");
				pubLog_DBErr(_FFL_,"spublicRight","2002","数据库执行失败");
                rtn_value = 2002;
        }
		pubLog_Debug(_FFL_,"spublicRight","rtn_value=","[%d]",rtn_value);
        return rtn_value;
}


/*******************************/
/* name:     spublicGetAccept  */
/* writer:   zhaohao           */
/* date:     2001.02.22        */
/* function: 取最大流水        */
/*******************************/
int spublicGetAccept(char *connect_name,int flag)
{
        EXEC SQL BEGIN DECLARE SECTION;
                int  in_flag; 
                static int maxaccept;
                char this_connect[30];
        EXEC SQL END DECLARE SECTION;

        memset(this_connect, 0, sizeof(this_connect));

        in_flag = flag;
        strcpy(this_connect, connect_name);

/*
        EXEC SQL AT :this_connect SET CHAINED OFF;
        EXEC SQL AT :this_connect EXEC  :maxaccept=prc_pub_maxaccept :in_flag;
*/
        switch (SQLCODE) {
                case 0:
                  break;
                case 1403:
                  maxaccept = -1;
                  break;
                default:
                  maxaccept = -1;
                  break;
        }

        return maxaccept;
}


/*******************************/
/* name:     spublicGetAccept  */
/* writer:   zhaohao           */
/* date:     2001.02.22        */
/* function: 取n个最大流水     */
/*******************************/
int spublicGetnAccept(char *connect_name,int flag, int n)
{
        EXEC SQL BEGIN DECLARE SECTION;
                int  in_flag;
                int  in_n;
                static int maxaccept;
                char this_connect[30];
        EXEC SQL END DECLARE SECTION;

        memset(this_connect, 0, sizeof(this_connect));

        in_flag = flag;
        in_n = n;
        strcpy(this_connect, connect_name);

/*
        EXEC SQL AT :this_connect SET CHAINED OFF;
        EXEC SQL AT :this_connect EXEC  :maxaccept=prc_pub_maxaccept :in_flag,:in_n;
*/
        switch (SQLCODE) {
                case 0:
                  break;
                case 100:
                  maxaccept = -1;
                  break;
                default:
                  maxaccept = -1;
                  break;
        }

        return maxaccept;
}

/***************  数据的合法性检验 ****************/

/*       1 合法      0  不合法     */

check_syn( str , type , sz1 )
char    *str;
int     type, sz1;
{
        int     i = 0 , j = 0;
        int     flag = 0;
        static char     temp[51];
        
        strcpy(temp,str);
        if (temp[0] != 0) {
                switch ( type ) {
                        case CHAR:
                        case VARCHAR:
                                /* 只进行长度的合法性检查 */
                                flag = check_char( sz1 , temp);
                                break;
                        case BIT:
                        case TINYINT:
                        case INT:
                        case SMALLINT:
                                flag = check_int( type, temp);
                                break;
                        case SMALLMONEY:
                        case MONEY:
                        case NUMERIC:
                        case FLOAT:
                        case REAL:
                        case DECIMAL:
                                flag = check_money( type, temp );
                                break;
                        case SMALLDATETIME:
                        case DATETIME:
                                flag = check_datetime( type, temp);
                                break;
                        case SYSNAME:
                        case DATETIMN:
                        case TEXT:
                        case TIMESTAMP:
                        case BINARY:
                        case VARBINARY:
                        case NCHAR:
                        case NVARCHAR:
                        case INTN:
                        case MONEYN:
                        case NUMERICN:
                        case FLOATN:
                        case DECIMALN:
                                /* 系统暂时不支持此数据类型 */
                                flag = 0;
                                break;
                        default:
                                /* 数据库中有非法数据类型 */
                                flag = 0;
                                break;
                }
        }
        return flag;
}

/* 检查字符型的合法性 */
check_char( wid , cond )
unsigned int  wid;
char    *cond;
{ 
        if ( strlen(cond) > wid ) {
                /* 长度超宽 */
                return ( 0 );
        }
        return ( 1 );
}


/* 检查整形的合法性 */
int check_int( type , cond )
unsigned int type;
char *cond;
{
        int     i,len,count,o_i;
        int     flag;
        int     flag1=1;
        char    ch;
        char    dat1[51] , dat2[51];

        if (cond[0] != '-' && !isdigit(cond[0])) {
                flag = 0;
                return flag;
        }
        else {
                if (cond[0] == '-') {
                        flag1 = -1;
                        memset(dat1, 0, sizeof(dat1));
                        strncpy(dat1, cond+1, strlen(cond)-1);
                        memset(cond, 0, sizeof(cond));
                        strcpy(cond, dat1);
                }
        }

        i = 0;  count = 0;
        ch = cond[i];
        for (i=0; i<strlen(cond); i++) {
                if ( !isdigit(cond[i]) ) {
                        break;
                }
        }
        if ( i<strlen(cond) ) {
                /* 字符串中有非数字的字符 */
                flag = 0;
                return flag;
        }
        
        if (type == BIT) {
                if ( (atoi(cond)*flag1 != 0) || (atoi(cond)*flag1 != 1) )
                        flag = 0;
                else
                        flag = 1;
        }
        else if (type == TINYINT) {
                if ( (atoi(cond)*flag1 < 0) || (atoi(cond)*flag1 > 255) )
                        flag = 0;
                else
                        flag = 1;
        }
        else if (type == SMALLINT) {
                if ( (atoi(cond)*flag1 < -32768) || (atoi(cond)*flag1 > 32767) )
                        flag = 0;
                else
                        flag = 1;
        }
        else if (type == INT) {
                if ( (atoi(cond)*flag1 < -214748363) || (atoi(cond)*flag1 > 2147483646) ) {
                        flag = 0;
                }
                else {
                        flag = 1;
                }
        }

        return flag;

}


/* 检查钱币型和高精度型数值的合法性 */
check_money( type , cond )
unsigned int type;
char *cond;
{
        int     i,j,len,o_i;
        int     flag, dot_num;
        int     flag1=1;
        char    dat1[51] , dat2[51];

        memset(dat1, 0, sizeof(dat1));
        memset(dat2, 0, sizeof(dat2));

        if (cond[0] != '-' && !isdigit(cond[0])) {
                flag = 0;
                return flag;
        }
        else {
                if (cond[0] == '-') {
                        flag1 = -1;
                        strncpy(dat1, cond+1, strlen(cond)-1);
                        memset(cond, 0, sizeof(cond));
                        strcpy(cond, dat1);
                }
        }

        i = 0;  dot_num = 0;
        for (i=0; i<strlen(cond); i++) {
                if ( isdigit(cond[i]) || (cond[i] == '.') ) {
                        if (cond[i] == '.') dot_num++;
                        continue;
                }
                else {  
                        break;
                }
        }

        if ( i<strlen(cond) || dot_num > 1) {
                /* 字符串中有非数字的字符 */
                flag = 0;
                return flag;
        }

        for(j=0; j<strlen(cond); j++) {
                if (cond[j] == '.' ) {
                        if (j == strlen(cond)-1)
                                strcat(cond, "00");
                        else if (j == strlen(cond)-2)
                                strcat(cond, "0");
                        cond[j+3] = '\0';
                        break;
                }
        }
        if (dot_num == 0)
                strcat(cond, ".00");
        
        if (type == SMALLMONEY) {
                if ( (atoi(cond)*flag1 < -214748.3648) || (atoi(cond)*flag1 > 214748.3648) )
                        flag = 0;
                else
                        flag = 1;
        }
        else if (type == MONEY) {
                if (flag1 == -1) {
                        sprintf(dat2, "-%s", cond);
                        /* strlen("-922337203685477.58") == 19 */
                        if (strlen(dat2) > 19)
                                flag = 0;
                        else 
                                if (strcmp(dat2, "-922337203685477.58") > 0)
                                        flag = 0;
                                else
                                        flag = 1;
                }
                else {
                        strcpy(dat2, cond);
                        if (strlen(dat2) > 18)
                                flag = 0;
                        else
                                if (strcmp(dat2, "922337203685477.58") > 0)
                                        flag = 0;
                                else
                                        flag = 1;
                }
        }
        else if ( (type == NUMERIC) || (type == FLOAT) || (type == REAL) || (type == DECIMAL) ) {
                if (flag1 == -1) {
                        sprintf(dat2, "-%s", cond);
                        if (strlen(dat2) > 34)
                                flag = 0;
                        else
                                if (strcmp(dat2, "-100000000000000000000000000000.00") > 0)
                                        flag = 0;
                                else
                                        flag = 1;
                }
                else {
                        strcpy(dat2, cond);
                        if (strlen(dat2) > 33)
                                flag = 0;
                        else
                                if (strcmp(dat2, "100000000000000000000000000000.00") > 0)
                                        flag = 0;
                                else
                                        flag = 1;
                }
        }

        return flag;

}

/* 检查日期型数值的合法性 */
check_datetime( type , cond )
unsigned int type;
char *cond;
{
        int     i,len,o_i;
        int     flag;
        int     flag1,flag2;
        char    date_tmp[8+1];
        char    time_tmp[8+1];
        char    dat1[51] , dat2[51], dat3[51];
        
        memset(date_tmp, 0, sizeof(date_tmp));
        memset(time_tmp, 0, sizeof(time_tmp));
        memset(dat1, 0, sizeof(dat1));
        memset(dat2, 0, sizeof(dat2));
        memset(dat3, 0, sizeof(dat3));

        i = 0;
        for (i=0; i<strlen(cond); i++) {
                if ( isdigit(cond[i]) || (cond[i] == ':') || (cond[i] == ' ') ) {
                        continue;
                }
                else {  
                        break;
                }
        }
        if ( i<strlen(cond) ) {
                /* 字符串中有非数字的字符 */
                flag = 0;
                return flag;
        }

        for (i=0; i<strlen(cond); i++) {
                if (cond[i] == ' ') break;
        }       
        strncpy(dat1, cond, i);
        strncpy(dat2, cond+i, strlen(cond)-i);

        for (i=0; i<strlen(dat2); i++) {
                if (dat2[i] != ' ') break;
        }
        strncpy(dat3, dat2+i, strlen(dat2)-i);

        if (strlen(dat1) == 8 && (strlen(dat3) == 8 || dat3[0] == 0)) {
                if ( (test_date(dat1, type) == 0) || (flag2 = test_time(dat3) == 0) )
                        return( 0 );
        }
        else
                return( 0 );

        return( 1 );
}


int test_time(time)
char    *time;
{
        char    str[5];
        int     hour,minute,second;
        int     flag;

        if (time[0] == 0) return( 1 );
        strncpy(str, time,   2);   str[2]=0;  hour   = atoi(str);
        strncpy(str, time+3, 2);   str[2]=0;  minute = atoi(str);
        strncpy(str, time+6, 2);   str[2]=0;  second = atoi(str);

        if (time[2] != ':' || time[5] != ':')
                return( 0 );
        
        if ((hour < 0 || hour > 24) || (minute < 0 || minute > 60) 
                || ( second < 0 || second > 60))
                return( 0 );

        return( 1 );
}

int test_date(date, type)
char    *date;
int     type;
{
        char    str[5];
        int     year,month,day;
        int     flag;
        strncpy(str, date,   4);   str[4]=0;   year  = atoi(str);
        strncpy(str, date+4, 2);   str[2]=0;   month = atoi(str);
        strncpy(str, date+6, 2);   str[2]=0;   day   = atoi(str);

        if ( day < 1 ) {
                return( 0 );
        }

        switch(month) {
                case 1: case 3: case 5: case 7:
                case 8: case 10: case 12:
                        if ( day > 31 )
                                return( 0 );
                        break;
                case 4: case 6: case 9: case 11:
                        if ( day > 30 )
                                return( 0 );
                        break;

                case 2:
                        if ( day > 29 )
                                return 0;
                        else if (  day == 29 &&
                                year % 400 != 0 && (year % 4 != 0
                                || year % 100 == 0))
                                return 0;
                        break;
                default:
                        return 0;
                        break;
        }

        if (type == SMALLDATETIME) {
                if (year < 1900 || year > 2079)
                        return 0;
        }
        else if (type == DATETIME) {
                if (year < 1753 || year > 9999)
                        return 0;
        }

        return( 1 );
}


/* 根据类型得到数值 */
int ttov(char *type)
{
        if (strcmp(type, "BINARY") == 0)
                return 1;
        if (strcmp(type, "BIT") == 0)
                return 2;
        if (strcmp(type, "CHAR") == 0)
                return 3;
        if (strcmp(type, "DATETIME") == 0)
                return 4;
        if (strcmp(type, "DATETIMN") == 0)
                return 5;
        if (strcmp(type, "DECIMAL") == 0)
                return 6;
        if (strcmp(type, "DECIMALN") == 0)
                return 7;
        if (strcmp(type, "FLOAT") == 0)
                return 8;
        if (strcmp(type, "FLOATN") == 0)
                return 9;
        if (strcmp(type, "IMAGE") == 0)
                return 10;
        if (strcmp(type, "INT") == 0)
                return 11;
        if (strcmp(type, "INTN") == 0)
                return 12;
        if (strcmp(type, "MONEY") == 0)
                return 13;
        if (strcmp(type, "MONEYN") == 0)
                return 14;
        if (strcmp(type, "NCHAR") == 0)
                return 15;
        if (strcmp(type, "NUMERIC") == 0)
                return 16;
        if (strcmp(type, "NUMERICN") == 0)
                return 17;
        if (strcmp(type, "NVARCHAR") == 0)
                return 18;
        if (strcmp(type, "REAL") == 0)
                return 19;
        if (strcmp(type, "SMALLDATETIME") == 0)
                return 20;
        if (strcmp(type, "SMALLINT") == 0)
                return 21;
        if (strcmp(type, "SMALLMONEY") == 0)
                return 22;
        if (strcmp(type, "SYSNAME") == 0)
                return 23;
        if (strcmp(type, "TEXT") == 0)
                return 24;
        if (strcmp(type, "TIMESTAMP") == 0)
                return 25;
        if (strcmp(type, "TINYINT") == 0)
                return 26;
        if (strcmp(type, "VARBINARY") == 0)
                return 27;
        if (strcmp(type, "VARCHAR") == 0)
                return 28;
        return 0;
}


int ifand(char *str, char *strb, char *stre)
{
        int  i=0;
        int  sl, ifflag;
        char tmp_strb[40+1];
        char tmp_stre[40+1];

        if (str[0] == 0) return( 0 );

        memset(tmp_strb, 0, sizeof(tmp_strb));
        memset(tmp_stre, 0, sizeof(tmp_stre));

        sl = strlen(str);

        for (i=0; i<sl; i++)
                if ((i < sl-1) && (str[i] == '&') && (str[i+1] == '&')) {
                        strncpy(tmp_strb, str, i);
                        strncpy(tmp_stre, str+i+2, sl-i-2);
                        ifflag = 1;
                        break;
                }
                else
                        ifflag = 0;

        if (ifflag == 1) {
                strcpy(strb, Trim(tmp_strb));
                strcpy(stre, Trim(tmp_stre));
        }
        else
                strcpy(strb, Trim(str));

        return( 1 );
}

char * getMaxID(char *region_code,int idType,char *ID)
{
	EXEC SQL BEGIN DECLARE SECTION;
        char pRegionCode[3];
        char sqlVst[256];
		static char retMaxID[23];
	EXEC SQL END DECLARE SECTION;

	memset(pRegionCode, '\0', sizeof(pRegionCode));
	memset(sqlVst, '\0', sizeof(sqlVst));
	memset(retMaxID, '\0', sizeof(retMaxID));
	memset(ID, '\0', sizeof(ID));
	memcpy(pRegionCode,region_code,sizeof(pRegionCode)-1);
	
	if(idType != 0 && idType != 1 && idType != 2 && idType != 3)
	{
		userlog("IN [%s] [%d] input idType=[%d] invalid \n",__FILE__,__LINE__,idType);
		return NULL;
	}
		
	if(idType == 0)
	{
		sprintf(sqlVst, "select to_char(sMaxCustId_%s.nextVal) from dual", pRegionCode);
	}
	else if(idType == 1)
	{
		sprintf(sqlVst, "select to_char(sMaxOpenId_%s.nextVal) from dual",pRegionCode);
	}
	else if(idType == 2)
	{
		sprintf(sqlVst, "select to_char(sMaxLineNo%s.nextVal) from dual",pRegionCode);
	}
	else 
	{
		strcpy(sqlVst, "select to_char(sMaxLineAccept.nextVal) from dual");
	}

	EXEC SQL PREPARE sqlStr FROM :sqlVst;
	EXEC SQL DECLARE S CURSOR FOR sqlStr;
	EXEC SQL OPEN S;
	EXEC SQL FETCH S INTO :retMaxID;
	EXEC SQL CLOSE S;
	if(sqlca.sqlcode != 0)
		return NULL;

	strcpy(ID, retMaxID);
	return retMaxID;
	
}

/*************
Bam 业务活动控制
*************/
int spublicLimit(char *login_no,char *function_code,char *limit_cause,char *deal_type)
{
        EXEC SQL BEGIN DECLARE SECTION;
        
        char iLoginNo[6+1];
        char iFunctionCode[4+1];
        char vOrgCode[9+1];
        char vLimitCause[256+1];
        char vDealType[1+1]; /*0-	前台提示（扩展预留）1-	业务禁止*/

        
        EXEC SQL END DECLARE SECTION;
		init(iLoginNo);
		init(iFunctionCode);
		init(vOrgCode);
		init(vLimitCause);
		init(vDealType);
    	/*memset(iLoginNo, 0, sizeof(iLoginNo));
        memset(iFunctionCode, 0, sizeof(iFunctionCode));
        memset(vOrgCode, 0, sizeof(vOrgCode));
        memset(vLimitCause, 0, sizeof(vLimitCause));
        memset(vDealType, 0, sizeof(vDealType));*/
		
		pubLog_Debug(_FFL_, "s3354Cfm", "", "login_no[%s]iLoginNo[%s]",login_no,iLoginNo);
		
		strcpy(iLoginNo,"majha");
		printf("login_no[%s]function_code[%s]\n",login_no,function_code);
		strncpy(iLoginNo,login_no,6);
       /* strcpy(iLoginNo, login_no);*/
        strcpy(iFunctionCode, function_code);
		
		printf( "iLoginNo[%s]iFunctionCode[%s]\n",iLoginNo,iFunctionCode);
		
        EXEC SQL SELECT org_code
                 INTO   :vOrgCode
                 FROM   dLoginMsg
                 WHERE  login_no=:iLoginNo;
        if (SQLCODE == 1403) 
        {
        	sprintf(limit_cause,"输入的工号[%s]有误[%d]",iLoginNo,SQLCODE);
        	return 9001;
        }
		/*0-	按营业厅控制*/
		EXEC SQL select control_cause,deal_type
					into :vLimitCause,:vDealType
					from dopercontrlimit
					where control_type='0'
					and control_chna=substr(:vOrgCode,1,7)
					and control_opcode=:iFunctionCode
					and begine_time<sysdate
					and end_time>sysdate;
		if( SQLCODE!=0 && SQLCODE!=1403)
		{
			PUBLOG_DBDEBUG("spublicLimit");
			pubLog_DBErr(_FFL_,"spublicLimit","9002","查询dopercontrlimit0有误");
			sprintf(limit_cause,"查询dopercontrlimit0有误[%d]",SQLCODE);
        	return 9002;
		}
		else if(SQLCODE==0)
		{
			sprintf(limit_cause,"%s",vLimitCause);
			sprintf(deal_type,"%s",vDealType);
        	return 9003;
		}
		
		/*1-	按工号控制*/
		EXEC SQL select control_cause,deal_type
					into :vLimitCause,:vDealType
					from dopercontrlimit
					where control_type='1'
					and control_loginno=:iLoginNo
					and control_opcode=:iFunctionCode
					and begine_time<sysdate
					and end_time>sysdate;
		if( SQLCODE!=0 && SQLCODE!=1403)
		{
			sprintf(limit_cause,"查询dopercontrlimit0有误[%d]",SQLCODE);
        	return 9004;
		}
		else if(SQLCODE==0)
		{
			sprintf(limit_cause,"%s",vLimitCause);
			sprintf(deal_type,"%s",vDealType);
        	return 9005;
		}
		
        return 0;
}

void SelfMultiParaFree(){
	int i;
	for (i = 0; i < DLMAXITEMS; i++){
		if (bind_dp->V[i] != (char * ) 0){
			free(bind_dp->V[i]);
		}
		free(bind_dp->I[i]);
	}
	sqlclu(bind_dp);
	EXEC SQL WHENEVER SQLERROR CONTINUE;
}


int SelfMultiParaDML(vSqlBuf,vParameterArray)
char *vSqlBuf;
char vParameterArray[][DLMAXITEMS][DLINTERFACEDATA];
{
	int i,n;
	char vParameter[DLINTERFACEDATA];

	EXEC SQL PREPARE SS FROM:vSqlBuf;
	if(SQLCODE!=SQLOK){
		printf("sqlcode=%d\n",SQLCODE);
		return -1;
		}

	EXEC SQL DECLARE CC CURSOR FOR SS;
	if(SQLCODE!=SQLOK)
		return -2;

	bind_dp =(SQLDA *)sqlald(DLMAXITEMS, DLMAXVNAMELEN, DLMAXINAMELEN);
	if ( bind_dp == (SQLDA * ) 0){
		EXEC SQL CLOSE CC;
		SelfMultiParaFree();
		return -3;
	}

	for (i = 0; i < DLMAXITEMS; i++) {
		bind_dp->I[i] = (short *) malloc(sizeof (short));
		bind_dp->V[i] = (char *) malloc(sizeof(char)+1);
	}

	bind_dp->N = DLMAXITEMS;

	EXEC SQL DESCRIBE BIND VARIABLES FOR SS INTO bind_dp;
	
	bind_dp->N = bind_dp->F;

	for (i = 0; i < bind_dp->F; i++) {
		init(vParameter);

		strcpy(vParameter,vParameterArray[0][i]);
	
		n = strlen(vParameter);
		bind_dp->L[i] = n;

		bind_dp->V[i] = (char * ) realloc(bind_dp->V[i],(bind_dp->L[i] + 1));

		memset(bind_dp->V[i],'\0',n+1);
		strncpy(bind_dp->V[i], vParameter, n);

		* bind_dp->I[i] = 0;
		
		bind_dp->T[i] = 96;
	}
	EXEC SQL OPEN CC USING DESCRIPTOR bind_dp;
	if(SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
		EXEC SQL CLOSE CC;
		SelfMultiParaFree();
		return -4;
	}
	if (SQLCODE==NOTFOUND){
		EXEC SQL CLOSE CC;
		SelfMultiParaFree();
		return 1;
	}
	
	EXEC SQL CLOSE CC;
	if(SQLCODE!=SQLOK){
		SelfMultiParaFree();
		return -5;
	}
printf("10\n"); 	
	SelfMultiParaFree();
	return 0;
}
