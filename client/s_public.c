#include "publicsrv.h"
#include "general.flds.h"
#include "general32.flds.h"

#define WORKDIR "WORKDIR"
#define         SPRI    "/"

extern int errno;
int 	sig_semid;
struct acceptshm *sig_shmp;

static struct sembuf op_lock={ 0,-1,0};
static struct sembuf op_unlock={ 0,1,0};
static struct sembuf op_endcreat={ 1,-1,SEM_UNDO };
static struct sembuf op_open={ 1,-1,0  };
static struct sembuf op_close={ 1,1,0  };


/* 根据环境变量的值取出全路径 */
char *
spublicGetEnvDir( char *env)
{
	char * envp;

	if ((envp = getenv(env)) == NULL) {
		userlog(" Get env err !!",__FILE__,__LINE__);
		return NULL;
	}
	return envp;
}


#define         CFGDIR  "/cfg"

/* 根据文件名取出/tbm/boss/run/cfg下的文件全路径 */
int
spublicGetCfgDir(const char *file, char *rtn_path)
{
	char *envp;

	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,CFGDIR);
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	return 0;
}

#define         BINDIR  "/bin"

/* 根据文件名取出/tbm/boss/run/bin下的文件全路径 */
int
spublicGetBinDir(const char *file, char *rtn_path)
{
	char *envp;
	
	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,BINDIR); 
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	
	return 0;
}


#define         INCLUDEDIR  "/include"

/* 根据文件名取出/tbm/boss/run/include下的文件全路径 */
int
spublicGetIncludeDir(const char *file, char *rtn_path)
{
	char *envp;
	
	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,INCLUDEDIR);
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	
	return 0;
}


#define         LIBDIR  "/lib"

/* 根据文件名取出/tbm/boss/run/lib下的文件全路径 */
int
spublicGetLibDir(const char *file, char *rtn_path)
{
	char *envp;
	
	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,LIBDIR);
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	
	return 0;
}


#define         PUBLICDIR  "/public"

/* 根据文件名取出/tbm/boss/run/public下的文件全路径 */
int
spublicGetPublicDir(const char *file, char *rtn_path)
{
	char *envp;
	
	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,PUBLICDIR);
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	
	return 0;
}

#define         TMPDIR  "/tmp"

/* 根据文件名取出/tbm/boss/run/tmp下的文件全路径 */
int
spublicGetTmpDir(const char *file, char *rtn_path)
{
	char *envp;

	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
		return -1;
	memset(rtn_path,0,LINE+1);
	
	strcpy(rtn_path,envp);
	strcat(rtn_path,TMPDIR);
	strcat(rtn_path,SPRI);
	strcat(rtn_path,file);
	
	return 0;
}

/* 根据文件名取出$REDOLOGDIR/log下的文件全路径 */
int
spublicGetRedoDir(char file[20], char *rtn_path)
{
        char *envp;

        memset(rtn_path,0,sizeof(rtn_path));

        envp=getenv(REDODIR);
        if (strlen(envp) == 0)
        {
                userlog(" Get env err !!",__FILE__,__LINE__);
                return -1;
        }

        strcpy(rtn_path,envp);
        strcat(rtn_path,SPRI);
        strcat(rtn_path,file);

        return 0;
}

/* 清除字符串前后的空格 */
char *Strim(char *src)
{
        int i=0;

        for(i=strlen(src); i>0; i--) {
                if (*(src+i-1) != ' ') {
                        *(src+i) = '\0';
                        break;
                }
        }
        if (i == 0) {
                *src = '\0';
                return src;
        }

        for(i=0; i<strlen(src); i++) {
                if (*(src+i) != ' ')
                        return src+i;
        }

        if (i == strlen(src)) {
                *src = '\0';
                return src;
        }
}

/* 取得共享内存键值 */
key_t
spublicGetShmKey()
{
	key_t kid;
	char filename[LINE+1];
	memset(filename, 0, sizeof(filename));
	spublicGetCfgDir(SHMKDIR, filename);
	kid = ftok( filename, SHMKEY);
	return kid;
}

/* 创建共享内存 */
struct comshm *
spublicCreatShm()
{
	int shmid;
	key_t shmkey,spublicGetShmKey();
	struct comshm *shmp;

	if (( shmkey = spublicGetShmKey()) < 0) {
		userlog(" get share memory key err!",__FILE__,__LINE__);
		return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct comshm),
		0666|IPC_CREAT)) < 0 ) {
		userlog(" get share memory id err!",__FILE__,__LINE__);
		return NULL;
	}
	
	if ((shmp = (struct comshm *) shmat (shmid, (char *) 0,0))
		== (struct comshm *) -1) {
		userlog(" share memory shmat err!",__FILE__,__LINE__);
		return NULL;
	}

	return shmp;
}

/* 从共享内存中取数据 */
struct comshm *
spublicGetShm()
{
	int shmid;
	key_t shmkey,spublicGetShmKey();
	struct comshm *shmp;

	if (( shmkey = spublicGetShmKey()) < 0) {
		userlog(" get share memory key err!",__FILE__,__LINE__);
	        return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct comshm), 0)) < 0 ) {
		userlog(" get share memory id err!",__FILE__,__LINE__);
		return NULL;
	}
	
	errno = 0;

	if ((shmp = (struct comshm *) shmat (shmid, 0,0)) == \
	(struct comshm *) -1) {
		userlog("shmat errno=%d\n",errno);
		userlog(" share memory shmat err!",__FILE__,__LINE__);
		return NULL;
	}

	return shmp;
}


/*******************************/
/* name:     spublicDelKeyShm  */
/* writer:   zhaohao           */
/* date:     2001.02.10        */
/* function: 删除共享内存中    */
/*           的数据            */
/*******************************/
int spublicDelKeyShm()
{
	int  shmid;
	int  rtn_value;
	key_t shmkey,spublicGetShmKey();

	rtn_value = 0;

	if (( shmkey = spublicGetShmKey()) < 0) {
		userlog(" get share memory key err!",__FILE__,__LINE__);
		userlog("取共享内存键值失败!\n");
		rtn_value = -1;
		return rtn_value;
	}

	if ((shmid= shmget(shmkey,sizeof(struct comshm),PERMS|IPC_CREAT))< 0 ) {
		userlog(" get share memory id err!",__FILE__,__LINE__);
		userlog("取共享内存ID失败!\n");
		rtn_value = -1;
		return rtn_value;
        }

        if (( shmctl (shmid, IPC_RMID, 0)) < 0) {
                userlog(" delete share memory  err!",__FILE__,__LINE__);
		userlog("删除共享内存失败!\n");
		rtn_value = -1;
		return rtn_value;
        }

        return rtn_value;
}

/* 断开共享内存与数据结构的连接 */
int
spublicDtcShm(struct comshm *shmp)
{
        if (shmdt ((char *) shmp) < 0 ) {
                userlog(" detach share memory err ! ",__FILE__,__LINE__);
                return FALSE;
        }
        return TRUE;
}


/* 2001.02.11 */
/* 根据提供的标签返回相应的服务器名、数据库名、用户和密码 */
struct spublicdblogin spublicGetDBP(char *connect_name) 
{
	struct comshm *shmp, *spublicGetShm();
	struct spublicdblogin sdblogin;
	int  count, i;

	shmp = spublicGetShm();
	if (shmp == NULL ) {
		userlog("get share memory err!\n");
		strcpy(sdblogin.label, NULL);
		strcpy(sdblogin.server, NULL);
		strcpy(sdblogin.database, NULL);
		strcpy(sdblogin.username, NULL);
		strcpy(sdblogin.password, NULL);
		return sdblogin;
	}

	count = shmp->sdbloginno;
	for ( i=0; i<count; i++)
        {
		if (strcmp(shmp->sdblogin[i].label, connect_name) == 0) {
			sdblogin = shmp->sdblogin[i];
			break;	
		}
	}

	if (count == 0) {
		strcpy(sdblogin.label, NULL);
		strcpy(sdblogin.server, NULL);
		strcpy(sdblogin.database, NULL);
		strcpy(sdblogin.username, NULL);
		strcpy(sdblogin.password, NULL);
	}

	spublicDtcShm( shmp );	
	
	return sdblogin;
}


/*******************************/
/* name:     spublicPutKeyShm  */
/* writer:   zhaohao           */
/* date:     2001.02.10        */
/* function: 将配置文件读入    */
/*           共享内存          */
/*******************************/
int spublicPutKeyShm()
{
	int  count;
	char tmp[LINE+1];
	char lbl[MAXLABELLEN+1];
	char srv[MAXLABELLEN+1];
	char db[MAXLABELLEN+1];
	char user[MAXLABELLEN+1];
	char passwd[MAXENDELEN+1];
	char temp_passwd[MAXENDELEN+1];
	char tmp_pass1[MAXENDELEN+1];
	char tmp_pass2[MAXENDELEN+1];

	char filename[LINE];
	FILE *fp;
	struct comshm *comshm;

	memset(tmp,0,LINE+1);
	memset(lbl,0,MAXLABELLEN+1);
	memset(srv,0,MAXLABELLEN+1);
	memset(db,0,MAXLABELLEN+1);
	memset(user,0,MAXLABELLEN+1);
	memset(passwd,0,MAXENDELEN+1);
	memset(temp_passwd,0,MAXENDELEN+1);

	spublicGetCfgDir ( TRANSSDBLOGIN, filename );
	if ( filename == NULL ) {
		userlog("get pathname err!",__FILE__,__LINE__);
		userlog("取配置文件失败!\n",__FILE__,__LINE__);
		return(-1);
	}

	if (( fp = fopen(filename,"r")) == NULL ) {
		userlog("open file err!",__FILE__,__LINE__);
		userlog("读配置文件失败!\n",__FILE__,__LINE__);
		return(-1);
	}

	/* 创建共享内存 */
	if (( comshm = spublicCreatShm()) == NULL ) {
		userlog("Creat share memory err!",__FILE__,__LINE__);
		userlog("创建共享内存失败!\n",__FILE__,__LINE__);
		return(-1);
	}
        memset(comshm,0,sizeof(struct comshm));

	count=0;
	while (fgets(tmp,LINE,fp) != NULL)  
	{
        	memset(tmp_pass1, 0, sizeof(tmp_pass1));
        	memset(tmp_pass2, 0, sizeof(tmp_pass2));

		if (tmp[0] != COMMENT ) 
		{
			sscanf (tmp,"%s%s%s%s%s",lbl,srv,db,user,passwd);
			strcpy(comshm->sdblogin[count].label,lbl);
			strcpy(comshm->sdblogin[count].server,srv);
			strcpy(comshm->sdblogin[count].database,db);
			strcpy(comshm->sdblogin[count].username,user);
	
			/* 将密码解密后放入共享内存 */
			spublicEnDePasswd(passwd, tmp_pass2, MASTERKEY, DECRYPT);
			strcpy(comshm->sdblogin[count].password, tmp_pass2);

			count++;
			memset(lbl,0,MAXLABELLEN+1);
			memset(srv,0,MAXLABELLEN+1);
			memset(db,0,MAXLABELLEN+1);
			memset(user,0,MAXLABELLEN+1);
			memset(passwd,0,MAXENDELEN+1);
		}
		memset(tmp,0,LINE+1);
	}
	comshm->sdbloginno = count;
	fclose(fp);

	spublicDtcShm(comshm);

	userlog("写入共享内存成功!");

	return(0);
}

/*******************************/
/* name:     spublicGenMAC     */
/* writer:   zhaohao	       */
/* date:     2001.02.15        */
/* function: 根据给定的字段    */
/*           产生MAC校验       */
/*******************************/
/*
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

	for(use_int=0; use_int<macstruc->use_int; use_int++) {
		memset(tmp_int, 0, sizeof(tmp_int));
		sprintf(tmp_int, "%d", macstruc->srcint[use_int]);

		if (use_int = 0) {
			strcpy(tmp_buf1, tmp_int);
		}
		else {
			strcat(tmp_buf1, tmp_int);
		}
	}

	for(use_float=0; use_float<macstruc->use_float; use_float++) {
		memset(tmp_float, 0, sizeof(tmp_float));
                if (macstruc->srcfloat[use_float] < 0) {
                        macstruc->srcfloat[use_float] = macstruc->srcfloat[use_float] * -1;
                }
		sprintf(tmp_float, "%.0f", macstruc->srcfloat[use_float]*100);

		if (use_float = 0) {
			strcpy(tmp_buf2, tmp_float);
		}
		else {
			strcat(tmp_buf2, tmp_float);
		}
	}

	for(use_char=0; use_char<macstruc->use_char; use_char++) {
		if (use_char = 0) {
			strcpy(tmp_buf3, alltrim(macstruc->srcchar[use_char]));
		}
		else {
			strcat(tmp_buf3, alltrim(macstruc->srcchar[use_char]));
		}
	}

	strcpy(buf, tmp_buf1);
	strcat(buf, tmp_buf2);
	strcat(buf, tmp_buf3);

	memset(MAC_bcd, 0 ,sizeof(MAC_bcd));

        GenerateMAC(MAC_STAND, buf, strlen(buf), MAINKEY, MAC_bcd);
        BcdToAsc_(out_MAC,MAC_bcd,16);

	if (MAINKEY != NULL)
		free(MAINKEY);

	rtn_value = 0;
	return rtn_value;
}
*/

/*
void spublicGetKeyShm(char *connect_name)
{
	int  rtn_value;

        if ( (rtn_value = sDBLogin(connect_name)) < 0 ) {
                userlog("The label name is wrong!\n");
                exit(0);
        }
        else {
                userlog("The label is right!\n");
        }
}


*/



/************* 公用套打格式函数 ****************/

/* 从固定位置取字符串 */
substring(char * dest,const char * src,int start,int n)
{
    int i,j,n1,s1;
    j=strlen(src);
    if (start>j)
    {
             dest[0]=0;
        return -1;
    }
    if (start+n>j+1)
    {
             n1=j+1-start;
    }
    else
    {
             n1=n;
    }
    for (i=0;i<n1;i++)
    {
             dest[i]=src[start-1+i];
    }
    dest[i]=0;
}

void   spublicinit_print()/***进入透明打印方式***/
{
       putchar(27);
       putchar('[');
       putchar('5');
       putchar('i');
       putchar(27);
       putchar('@');

}

void   spubliccancel_print()
{
	putchar(12);
}


void   spublicend_print()   /***退出透明打印方式***/
{
	putchar(27);
	putchar('[');
        putchar('4');
        putchar('i');
}



/*******************************/
/* name:     spublicPrnFormat  */
/* writer:   zhaohao           */
/* date:     2001.02.10        */
/* function: 根据配置文件打印  */
/*           发票              */
/*******************************/
/*
int spublicPrnFormat(char in_array[][30+1])
{
	int  count, len;
	int  i, j, tmp_line;
	int  flag;
	char tmp[LINE+1];
	char tmp_dt[2+1];
	char yy[4+1];
	char mm[2+1];
	char dd[2+1];
	char hh[2+1];
	char ii[2+1];
	char ss[2+1];
	char prn_buf[1000+1];
	char prn_buf1[1000+1];
	char prn_buf2[1000+1];

	PRNFORMAT prnformat;

	char filename[LINE];
	FILE *fp;

	memset(tmp,0,LINE+1);
	memset(prn_buf,0,1000+1);
	memset(prn_buf1,0,1000+1);
	memset(prn_buf2,0,1000+1);
	memset(yy, 0, sizeof(yy));
	memset(mm, 0, sizeof(mm));
	memset(dd, 0, sizeof(dd));
	memset(hh, 0, sizeof(hh));
	memset(ii, 0, sizeof(ii));
	memset(ss, 0, sizeof(ss));

	strncpy(yy,GetSysDate(),4);
	strncpy(mm,GetSysDate()+4,2);
	strncpy(dd,GetSysDate()+6,2);
	strncpy(hh,GetSysTime(),2);
	strncpy(ii,GetSysTime()+2,2);
	strncpy(ss,GetSysTime()+4,2);

	spublicGetCfgDir( TRANSSPRNFORMAT, filename );
	if ( filename == NULL ) {
		userlog("get pathname err!",__FILE__,__LINE__);
		return(-1);
	}

	if (( fp = fopen(filename,"r")) == NULL ) {
		userlog("open file err!",__FILE__,__LINE__);
		return(-1);
	}

	spublicinit_print(); 

	count = 1;
	tmp_line = 1;
	flag = 0;

	while (fgets(tmp,LINE,fp) != NULL)  
	{
        	memset(&prnformat, 0 ,sizeof(PRNFORMAT));
		strcpy(prnformat.line_no, "0");
		
		if (tmp[0] != COMMENT ) 
		{
			len=strlen(tmp);
			tmp[len-1] = '\0';

			strncpy(prnformat.line_no, tmp, 4);
			strncpy(prnformat.col_width, tmp+4, 4);
			strncpy(prnformat.order_type, tmp+8, 2);
			strncpy(prnformat.format_type, tmp+10, 2);
			strncpy(prnformat.all_var, tmp+12,36);
			
			if (tmp_line == atoi(prnformat.line_no)) {
				memset(prn_buf, 0, sizeof(prn_buf));
				memset(prn_buf2,0,sizeof(prn_buf2));

				if (strcmp(alltrim(prnformat.format_type), "s") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					sprintf(prn_buf2, prn_buf, alltrim((char *)(in_array+atoi(prnformat.all_var))));
				}
				else if (strcmp(alltrim(prnformat.format_type), "x") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					sprintf(prn_buf2, prn_buf, alltrim(prnformat.all_var));
				}
				else if (strcmp(alltrim(prnformat.format_type), "t") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					memset(tmp_dt, 0, sizeof(tmp_dt));
					strcpy(tmp_dt, alltrim(prnformat.all_var));
					switch(tmp_dt[0]) {
						case 'y':
							sprintf(prn_buf2, prn_buf, yy);
							break;
						case 'm':
							sprintf(prn_buf2, prn_buf, mm);
							break;
						case 'd':
							sprintf(prn_buf2, prn_buf, dd);
							break;
						case 'h':
							sprintf(prn_buf2, prn_buf, hh);
							break;
						case 'i':
							sprintf(prn_buf2, prn_buf, ii);
							break;
						case 's':
							sprintf(prn_buf2, prn_buf, ss);
							break;
						default:
							break;
					}
				}
				else if (strcmp(alltrim(prnformat.format_type), "n") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					if (prnformat.all_var[0] == 0) {
						printf("\n");
					}
					else {
						sprintf(prn_buf2, prn_buf, alltrim(prnformat.all_var));
						printf("\n");
					}
				}

				strcat(prn_buf1, prn_buf2);
				flag = 0;
			}
			else {
				printf("%s\n",prn_buf1);
				memset(prn_buf1, 0, sizeof(prn_buf1));

				memset(prn_buf, 0, sizeof(prn_buf));
				memset(prn_buf2,0,sizeof(prn_buf2));

				if (strcmp(alltrim(prnformat.format_type), "s") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					sprintf(prn_buf2, prn_buf, alltrim((char *)(in_array+atoi(prnformat.all_var))));
				}
				else if (strcmp(alltrim(prnformat.format_type), "x") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					sprintf(prn_buf2, prn_buf, alltrim(prnformat.all_var));
				}
				else if (strcmp(alltrim(prnformat.format_type), "t") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					memset(tmp_dt, 0, sizeof(tmp_dt));
					strcpy(tmp_dt, alltrim(prnformat.all_var));
					switch(tmp_dt[0]) {
						case 'y':
							sprintf(prn_buf2, prn_buf, yy);
							break;
						case 'm':
							sprintf(prn_buf2, prn_buf, mm);
							break;
						case 'd':
							sprintf(prn_buf2, prn_buf, dd);
							break;
						case 'h':
							sprintf(prn_buf2, prn_buf, hh);
							break;
						case 'i':
							sprintf(prn_buf2, prn_buf, ii);
							break;
						case 's':
							sprintf(prn_buf2, prn_buf, ss);
							break;
						default:
							break;
					}
				}
				else if (strcmp(alltrim(prnformat.format_type), "n") == 0) {
					sprintf(prn_buf, "%%%c%ds", 
						prnformat.order_type[0],
						atoi(prnformat.col_width));
					if (prnformat.all_var[0] == 0) {
						printf("\n");
					}
					else {
						sprintf(prn_buf2, prn_buf, alltrim(prnformat.all_var));
						printf("\n");
					}
				}

				strcpy(prn_buf1, prn_buf2);	

				flag = 1;

				tmp_line++;
			}

			memset(prn_buf, 0, sizeof(prn_buf));
			memset(prn_buf2, 0, sizeof(prn_buf2));
			memset(tmp,0,LINE+1);
			count++;
		}
	}
	fclose(fp);


	spublicend_print(); 

	return(0);
}
*/

/************* 公用套打格式函数 ****************/


/***********************************************/
/* 功能: 实现内存流水                          */
/* 时间: 2002.06.12/dangsl                     */
/* 版本: 1.0                                   */
/***********************************************/

/* 创建单值信号灯 */
int NCreatOneSem(const key_t semkey)
{
	int semid,semval;
        union semun {
        	int val;
                struct semid_ds *buf;
                unsigned short *array;
                } semctl_arg;


	semid=semget(semkey,1,IPC_CREAT|PERMS);  
        if(semid<0){
                userlog("semaphoreid get error!\n");
       		return -1;
	}

        if((semval=semctl(semid,0,GETVAL,0))<0)
                userlog("GETVAL error!\n");
        else
	        if(semval==0){
                	semctl_arg.val=1;
                        if(semctl(semid,0,SETVAL,semctl_arg)<0)
                      		userlog("SETVAL error\n");
                }   
          

       return semid;
}

/* 根据semid删除信号灯 */
int NSemDel(int semid)
{
 	if((semctl(semid,0,IPC_RMID,0))<0) {
 		userlog("delete semaphore error!\n");
 		return -1;
 	}

       	return 0;
}



/* 打开单值信号灯 */
int NOpenOneSem(semkey)
const key_t semkey;
{
        int semid;

	semid=semget(semkey,1,0);  
        if(semid<0){
        	userlog("semaphoreid get error!\n");
                return -1;
        }
	return semid;
}


/* 加单值锁 */
int Nsm_sem_lock(semid)
const int semid;
{
	while((semop(semid,&op_lock,1))<0){
        	if(errno==EINTR){
			usleep(1000);
			continue;
		}
		userlog("sem op_lock error!\n");
                return -1;
        }
        return 0;
}


/* 释放单值锁 */
int  Nsm_sem_unlock(semid)
const int semid;
{
 
        while((semop(semid,&op_unlock,1))<0){
        	if(errno==EINTR){
			usleep(1000);
			continue;
		}
		userlog("sem op_unlock error!\n");
                return -1;
        } 
        return 0;
}



/* 对非阻塞信号灯加单值锁 */
int Nsem_lock_nowait(semid)
const int semid;
{
struct sembuf op_no_lock={ 0,-1,IPC_NOWAIT};

        while(( semop(semid,&op_no_lock,1) ) < 0 ){
                if( errno==EINTR ) {
                        usleep(1000);
                        continue;
                }
                else if(errno== EAGAIN||errno==EFBIG){
                        userlog("sem_lock_nowait ret\n");
                        return -1;
                }
                userlog("sem op_lock error!\n");
                return -1;
        }
        return 0;
}


/* 对非阻塞信号灯解单值锁 */
int Nsem_unlock_nowait(semid)
const int semid;
{
struct sembuf op_no_unlock={ 0,1,IPC_NOWAIT};

 	while(( semop(semid,&op_no_unlock,1) ) < 0 ){
 		if( errno==EINTR ) {
 			usleep(1000);
 			continue;
 		}
 		else if(errno== EAGAIN||errno==EFBIG){
    			userlog("sem_unlock_nowait ret\n");
 			return -1;
 		}
 		userlog("sem op_unlock error!\n");
 		return -1;
 	}
 	return 0;
}

/*--------创建共享内存--------*/
struct acceptshm * 
NCreatShm(key_t shmkey, int length, int *shmid)
{
	struct acceptshm *shmp;

	if((*shmid=shmget(shmkey,length, PERMS|IPC_CREAT)) < 0){
                userlog(" get share memory id err!\n");
                return NULL;
        }

	if((shmp = (struct acceptshm *)shmat(*shmid, (struct acceptshm  *)0, 0)) == (struct acceptshm  *) -1) {
                userlog(" share memory shmat err!\n");
                return NULL;
        }

        return shmp;
}

/*--------创建共享内存--------*/
struct redoShm * 
redoCreatShm(key_t shmkey, int length, int *shmid)
{
	struct redoShm *shmp;

	if((*shmid=shmget(shmkey,length, PERMS|IPC_CREAT)) < 0){
                userlog(" get share memory id err!\n");
                return NULL;
        }

	if((shmp = (struct redoShm *)shmat(*shmid, (struct redoShm  *)0, 0)) == (struct redoShm  *) -1) {
                userlog(" share memory shmat err!\n");
                return NULL;
        }

        return shmp;
}


/*----------使用已经创建的共享内存---------------*/
struct acceptshm * 
NOpenShm(key_t shmkey)
{
	int	shmid;
	struct acceptshm *shmp;

	if((shmid=shmget(shmkey, 0, 0)) < 0){
                userlog(" open get share memory id err!\n");
                return NULL;
        }

	if((shmp = (struct acceptshm  *)shmat(shmid, (struct acceptshm  *)0, 0)) == (struct acceptshm  *) -1) {
                userlog(" open share memory shmat err!\n");
                return NULL;
        }

        return shmp;
}

/*----------使用已经创建的共享内存---------------*/
struct redoShm * redoOpenShm(key_t shmkey)
{
	int	shmid;
	struct redoShm *shmp;

	if((shmid=shmget(shmkey, 0, 0)) < 0){
                userlog(" open get share memory id err!\n");
                return (struct redoShm *)NULL;
        }
	if((shmp = (struct redoShm  *)shmat(shmid, (struct redoShm  *)0, 0)) == (struct redoShm  *) -1) {
                userlog(" open share memory shmat err!\n");
                return (struct redoShm *)NULL;
        }
        return shmp;
}

/*---------关闭已经打开的共享内存-------------------*/
int NCloseShm(const struct acceptshm *shmp){
	int	lret;
	lret=NDtcShm(shmp);
	return lret;
}
int 
NDtcShm(const struct acceptshm *shmp)
{
	if((shmdt(shmp)) < 0 ) {
		userlog(" detach share memory err ! \n");
		return -1;
	}
	return 0;
}


/*----------删除已经创建的共享内存------------*/
int
NDelShm(key_t shmkey)
{
int 	shmid;

	if ((shmid= shmget(shmkey, 0, 0)) < 0 ) {
                userlog(" get share memory id err!\n");
                return -1;
        }

	if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *)0)) < 0) {
		userlog(" delete share memory  err!\n");
		return -2;
	}

	return 0;
}


/*--------可根据返回值判断OpenShm Err的原因--------*/
int
NOpenShmErr(key_t shmkey)
{
	int	shmid;
	struct acceptshm *shmp;

	if((shmid=shmget(shmkey, 0, 0)) < 0){
                userlog(" open get share memory id err!\n");
                return -1;
        }

	if((shmp = (struct acceptshm *)shmat(shmid, (struct acceptshm *)0, 0)) == (struct acceptshm *) -1) {
                userlog(" open share memory shmat err!\n");
                return -2;
        }

        return 0;
}

char *GetSysDatetime()
{
	struct tm *p;
        time_t t;
	char   tempsec[3],tempmin[3],temphour[3];
	char   tempday[3],tempmon[3],tempyear[5];
	char   temptime[30];

        t = time(NULL);
        p = localtime(&t);

	memset(tempsec,0,sizeof(tempsec));
	memset(tempmin,0,sizeof(tempmin));
	memset(temphour,0,sizeof(temphour));
	memset(tempday,0,sizeof(tempday));
	memset(tempmon,0,sizeof(tempmon));
	memset(tempyear,0,sizeof(tempyear));

	sprintf(tempsec,"%d",p->tm_sec);
	sprintf(tempmin,"%d",p->tm_min);
	sprintf(temphour,"%d",p->tm_hour);
	sprintf(tempday,"%d",p->tm_mday);
	sprintf(tempmon,"%d",p->tm_mon+1);
	sprintf(tempyear,"%d",p->tm_year+1900);
	if(p->tm_sec<10)
	{
		memset(tempsec,0,sizeof(tempsec));
		sprintf(tempsec,"0%d",p->tm_sec); 
	}
	if(p->tm_min<10)  
	{
		memset(tempmin,0,sizeof(tempmin));
		sprintf(tempmin,"0%d",p->tm_min); 
	}
	if(p->tm_hour<10)
	{
		memset(temphour,0,sizeof(temphour));
		sprintf(temphour,"0%d",p->tm_hour); 
	}
	if(p->tm_mday<10)
	{
		memset(tempday,0,sizeof(tempday));
		sprintf(tempday,"0%d",p->tm_mday); 
	}
	if(p->tm_mon+1<10)
	{ 
		memset(tempmon,0,sizeof(tempmon));
		sprintf(tempmon,"0%d",p->tm_mon+1);
	}
	memset(temptime,0,sizeof(temptime));
	strcat(temptime,tempyear);
	strcat(temptime,tempmon);
	strcat(temptime,tempday);
	strcat(temptime,temphour);
	strcat(temptime,tempmin);
	strcat(temptime,tempsec);

	return(temptime);
}

void Quit(int signo)
{
	Nsm_sem_unlock(sig_semid);
	NCloseShm(sig_shmp);
	exit(0);
}


int CreatMemAccept()
{
        int semid,shmid;
        struct acceptshm *shmp;
	char semfile[LINE],shmfile[LINE],destdate[10],srcdate[20],tempdate[30];
	char tempcmd[LINE+LINE],tempstr[LINE];

	memset(tempdate, 0, sizeof(tempdate));
	memset(shmfile, 0, sizeof(shmfile));
	memset(semfile, 0, sizeof(semfile));
	memset(destdate, 0, sizeof(destdate));
	memset(srcdate, 0, sizeof(srcdate));
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));

	spublicGetCfgDir(ACCSHMKEYFILE, tempstr);
	strcpy(shmfile,tempstr);

	spublicGetCfgDir(ACCSEMKEYFILE, tempstr);
	strcpy(semfile,tempstr);

	memset(tempcmd, 0, sizeof(tempcmd));
	strcpy(tempcmd,"touch ");
	strcat(tempcmd,shmfile);
	system(tempcmd);

	memset(tempcmd, 0, sizeof(tempcmd));
	strcpy(tempcmd,"touch ");
	strcat(tempcmd,semfile);
	system(tempcmd);


        if((semid = NCreatOneSem(ftok(semfile,1)))==NULL)
	{
		userlog("Create Accept Semaphore Error!\n");
		return -1;
	}
	printf("Create Accept Semaphore:\n");
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));
	strcpy(tempcmd,"ipcs -s|grep ");
	sprintf(tempstr,"%d",semid);
	strcat(tempcmd,tempstr);
	system(tempcmd);

        if((shmp = NCreatShm(ftok(shmfile,1),sizeof(struct acceptshm),&shmid))==NULL)
	{
		userlog("Create Accept Shared Memory Error!\n");
		return -2;
	}

	memset(srcdate, 0, sizeof(srcdate));
	memset(destdate, 0, sizeof(destdate));

	strcpy(tempdate,GetSysDatetime());
	strcpy(srcdate,tempdate);
	substring(destdate,srcdate,1,8);
	strcpy(shmp->sysdate,destdate);

	printf("Create Accept Shared Memory:\n");
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));
	strcpy(tempcmd,"ipcs -m|grep ");
	sprintf(tempstr,"%d",shmid);
	strcat(tempcmd,tempstr);
	system(tempcmd);
	
	return 0;
}	

int CreateRedoShm()
{
        int shmid,i;
        struct redoShm *shmp;
	char shmfile[LINE];
	char tempcmd[LINE+LINE],tempstr[LINE];

	memset(shmfile, 0, sizeof(shmfile));
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));

	spublicGetCfgDir(REDOSHMKEYFILE, tempstr);
	strcpy(shmfile,tempstr);

	memset(tempcmd, 0, sizeof(tempcmd));
	strcpy(tempcmd,"touch ");
	strcat(tempcmd,shmfile);
	system(tempcmd);

        if((shmp = redoCreatShm(ftok(shmfile,1),sizeof(struct redoShm),&shmid))==NULL)
	{
		userlog("Create Redo Shared Memory Error!\n");
		return -2;
	}

	memset(shmp->memAccept,0,sizeof(shmp->memAccept));
	memset(shmp->redoFile,0,sizeof(shmp->redoFile));
	memset(shmp->svcName,0,sizeof(shmp->svcName));
	memset(shmp->phoneNo,0,sizeof(shmp->phoneNo));

	for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
	{
		shmp->redoPosition[i]=0;
		shmp->flag[i]=1;
		/* 0 is busy */
	}
	shmp->endFlag=0;

	printf("Create Redo Shared Memory:%d\n",shmid);
	
	return 0;
}

char *GetMemAccept()
{
	int semid;
	struct acceptshm *shmp;
	char accept[LINE+LINE],temp[70],temptime[30];
	char semfile[LINE],shmfile[LINE];
	char tempcmd[LINE+LINE],tempstr[LINE];

	memset(temp, 0, sizeof(temp));
	memset(temptime, 0, sizeof(temptime));
	memset(accept, 0, sizeof(accept));
	memset(shmfile, 0, sizeof(shmfile));
	memset(semfile, 0, sizeof(semfile));
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));

	spublicGetCfgDir(ACCSHMKEYFILE, tempstr);
	strcpy(shmfile,tempstr);

	spublicGetCfgDir(ACCSEMKEYFILE, tempstr);
	strcpy(semfile,tempstr);

	/* 打开信号灯 */
	if((semid = NOpenOneSem(ftok(semfile,1)))<0)return NULL;
	/* 打开共享内存 */
	if((shmp = NOpenShm(ftok(shmfile,1)))==NULL)return NULL;

	/* 全局变量用于QUIT函数释放资源 */
	sig_semid=semid;
	sig_shmp=shmp;

	/* 设置进程中断信号 */
	signal(SIGTERM,Quit);

	/* 信号灯加锁 */
	if(Nsm_sem_lock(semid)!=0)return NULL;

	/* 共享内存递增 */
	shmp->count++;


	sprintf(temp,"%d",shmp->count);

	/* 信号灯解锁 */
	if(Nsm_sem_unlock(semid)!=0)return NULL;

	/* 共享内存关闭 */
	if(NCloseShm(shmp)!=0)return NULL;


	strcpy(temptime,GetSysDatetime());

	strcpy(accept,temptime);
	strcat(accept,temp);


	return accept;
}

char *GetMemDate()
{
	int semid;
	struct acceptshm *shmp;
	char accept[LINE],temp[LINE];
	char semfile[LINE],shmfile[LINE],destdate[10],srcdate[20],olddate[10],tempdate[30];
	char tempcmd[LINE+LINE],tempstr[LINE];

	memset(accept, 0, sizeof(accept));
	memset(temp, 0, sizeof(temp));
	memset(shmfile, 0, sizeof(shmfile));
	memset(semfile, 0, sizeof(semfile));
	memset(destdate, 0, sizeof(destdate));
	memset(srcdate, 0, sizeof(srcdate));
	memset(olddate, 0, sizeof(olddate));
	memset(tempcmd, 0, sizeof(tempcmd));
	memset(tempstr, 0, sizeof(tempstr));

	spublicGetCfgDir(ACCSHMKEYFILE, tempstr);
	strcpy(shmfile,tempstr);

	spublicGetCfgDir(ACCSEMKEYFILE, tempstr);
	strcpy(semfile,tempstr);

	/* 打开信号灯 */
	if((semid = NOpenOneSem(ftok(semfile,1)))<0)return NULL;
	/* 打开共享内存 */
	if((shmp = NOpenShm(ftok(shmfile,1)))==NULL)return NULL;

	/* 全局变量用于QUIT函数释放资源 */
	sig_semid=semid;
	sig_shmp=shmp;

	memset(srcdate, 0, sizeof(srcdate));
	memset(destdate, 0, sizeof(destdate));

	strcpy(tempdate,GetSysDatetime());
	strcpy(srcdate,tempdate);
	substring(destdate,srcdate,1,8);

	if(strcmp(shmp->sysdate,destdate)==0) 
	{
		return NULL;
	}
	else
	{
		/* 设置进程中断信号 */
		signal(SIGTERM,Quit);

		memset(olddate,0,sizeof(olddate));
		strcpy(olddate,shmp->sysdate);

		/* 信号灯加锁 */
		if(Nsm_sem_lock(semid)!=0)return NULL;

		strcpy(shmp->sysdate,destdate);

		/* 信号灯解锁 */
		if(Nsm_sem_unlock(semid)!=0)return NULL;

		/* 共享内存关闭 */
		if(NCloseShm(shmp)!=0)return NULL;

		return olddate;
	}
}


int DelMemAccept()
{
	int semid;
	char semfile[LINE],shmfile[LINE];

	memset(shmfile, 0, sizeof(shmfile));
	spublicGetCfgDir(ACCSHMKEYFILE, shmfile);
	if(NDelShm(ftok(shmfile,1))!=0)return -1;

	memset(semfile, 0, sizeof(semfile));
	spublicGetCfgDir(ACCSEMKEYFILE, semfile);
	if((semid = NOpenOneSem(ftok(semfile,1)))<0)return -2;
	if(NSemDel(semid)!=0)return -3;

	userlog("Accept Shared Memory and Semaphore was removed!\n");
	printf("Accept Shared Memory and Semaphore was removed!\n");

	return 0;
}

int DelRedoShm()
{
	char shmfile[LINE],tempcmd[LINE],tempstr[LINE];

	memset(shmfile, 0, sizeof(shmfile));
	spublicGetCfgDir(REDOSHMKEYFILE, shmfile);
	if(NDelShm(ftok(shmfile,1))!=0)return -1;

	userlog("Redo's Shared Memory was removed!\n");
	printf("Redo's Shared Memory was removed!\n");

	return 0;
}


int redoLog(char *strService, FBFR *transIN)
{	
	char strFlag[5],accept[30]; /* 重做标志, 'redo'--重做; 否则,正常 */
	int  rtnCode,findAcceptFlag,findOpCodeFlag;
	char serviceType[8],opCode[5],phoneNo[16];
	char workNo[7],totalDate[9],cfmAccept[12];
	char phoneNoTmp[16],workNoTmp[7],totalDateTmp[9],cfmAcceptTmp[12];
	char cfmMemAccept[30];      /* 存储内存流水的变量 */
	char cfmAcceptFile[100],cancelAcceptFile[100],cancelOpCodeFile[100];
	char cancelOpCode[5],cfmOpCode[5];
	FILE *cfmFP,*cancelFP,*opcodeFP;

	memset(strFlag,0,sizeof(strFlag));
	memset(accept,0,sizeof(accept));
	memset(serviceType,0,sizeof(serviceType));
	memset(opCode,0,sizeof(opCode));
	memset(phoneNo,0,sizeof(phoneNo));
	memset(workNo,0,sizeof(workNo));
	memset(totalDate,0,sizeof(totalDate));
	memset(cfmAccept,0,sizeof(cfmAccept));
	memset(phoneNoTmp,0,sizeof(phoneNoTmp));
	memset(workNoTmp,0,sizeof(workNoTmp));
	memset(totalDateTmp,0,sizeof(totalDateTmp));
	memset(cfmAcceptTmp,0,sizeof(cfmAcceptTmp));
	memset(cfmMemAccept,0,sizeof(cfmMemAccept));
	memset(cfmAcceptFile,0,sizeof(cfmAcceptFile));
	memset(cancelAcceptFile,0,sizeof(cancelAcceptFile));
	memset(cancelOpCodeFile,0,sizeof(cancelOpCodeFile));
	memset(cancelOpCode,0,sizeof(cancelOpCode));
	memset(cfmOpCode,0,sizeof(cfmOpCode));
	
	if (-1 == Fget(transIN, RedoFlag, 0, strFlag, 0))
	{
        	strcpy(strFlag, "");
	}

	Fget(transIN, DPARM_0, 0, serviceType, 0);
	Fget(transIN, DPARM_1, 0, opCode, 0);
	Fget(transIN, DPARM_2, 0, phoneNo, 0);
	Fget(transIN, DPARM_3, 0, workNo, 0);
	Fget(transIN, DPARM_4, 0, totalDate, 0);
	Fget(transIN, DPARM_5, 0, cfmAccept, 0);

	userlog("KBROS:serviceType[%s],opCode=[%s],phoneNo[%s],workNo[%s],totalDate[%s],cfmAccept[%s]\n",
		serviceType,opCode,phoneNo,workNo,totalDate,cfmAccept);

	strcpy(accept,GetMemAccept());
	if (strlen(accept) == 0)
	{
		fprintf(stderr, "取内存流水错误!\n");
		return -1;
	}

	/* 冲正确认服务的处理 */
	if(strcmp(serviceType,"cancel") == 0)
	{
		if (strcmp(strFlag,"redo") == 0) 
		{
			return 0;
		}
		spublicGetCfgDir(SOPCODEFILE, cancelOpCodeFile);
		if(!(opcodeFP=fopen(cancelOpCodeFile,"r")))
		{
			userlog("KBROS:打开操作代码对应配置文件错误!\n");
		}
		else
		{
			findOpCodeFlag=0;
			while(!feof(opcodeFP))
			{
				fscanf(opcodeFP,"%s %s\n",cancelOpCode,cfmOpCode);
				if(strcmp(cancelOpCode,opCode)==0)
				{
					findOpCodeFlag=1;
					break;
				}
			}
			if(findOpCodeFlag == 0)
			{
				userlog("KBROS:OPCODE在sCancelOpCode.cfg配置文件中不存在!\n");
			}
			fclose(opcodeFP);
		}

		spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
		strcat(cfmAcceptFile,cfmOpCode);

		spublicGetRedoDir(CANCELACCEPTFILE, cancelAcceptFile);
		strcat(cancelAcceptFile,cancelOpCode);


		if(!(cancelFP=fopen(cancelAcceptFile,"a+")))
		{
			userlog("KBROS:打开冲正操作流水对应文件错误!\n");
		}
		else
		{
			if(!(cfmFP=fopen(cfmAcceptFile,"r")))
			{
				userlog("KBROS:打开确认操作流水对应文件错误!\n");
			}
			else
			{
				findAcceptFlag=0;
				while(!feof(cfmFP))
				{
					/* 取出确认操作的相关数据，确认冲正业务对应的确认业务 */
					fscanf(cfmFP,"%s %s %s\n",
						cfmMemAccept,totalDateTmp,cfmAcceptTmp); 

					if(strcmp(totalDate,totalDateTmp)==0&&
						strcmp(cfmAccept,cfmAcceptTmp)==0)
					{
						fprintf(cancelFP,"%s %s %s %s\n",
							accept,cfmMemAccept,cfmAccept,totalDate); 
						findAcceptFlag=1;
						break;
					}
				}
				if(findAcceptFlag == 0)
				{
					userlog("KBROS:找不到冲正操作对应的确认操作!\n");
				}
				fclose(cfmFP);
				fclose(cancelFP);
			}
		}
	}    /* 冲正处理结束 */
	else if(strcmp(serviceType,"confirm") == 0)
	     /* 确认服务处理 */
	{
		spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
		strcat(cfmAcceptFile,opCode);
		
		if(!(cfmFP=fopen(cfmAcceptFile,"a+")))
		{
			userlog("KBROS:打开确认操作流水对应文件错误!\n");
		}
		else
		{
			fprintf(cfmFP,"%s %s %s\n",
				accept,totalDate,cfmAccept); 
			fclose(cfmFP);
		}
	}
	
	if (strcmp(strFlag,"redo") == 0) 
	{
		return 0;
	}
	
	fprintf(stdout,"<TitleMessage> %s %s %s\n",accept,strService,"fml");
	userlog("<TitleMessage> %s %s %s\n",accept,strService,"fml");
	
	if (Fprint(transIN) == -1)
	{
		fprintf(stderr,"redoLog-Fprint errror:Ferror=%s\n",Fstrerror(Ferror));
		return -1;
	}
	
	return 0;
}


int redoLog32(char *strService, FBFR32 *transIN)
{	
	char strFlag[5],accept[30]; /* 重做标志, 'redo'--重做; 否则,正常 */
	int  rtnCode,findAcceptFlag,findOpCodeFlag;
	char serviceType[8],opCode[5],phoneNo[16];
	char workNo[7],totalDate[9],cfmAccept[12];
	char phoneNoTmp[16],workNoTmp[7],totalDateTmp[9],cfmAcceptTmp[12];
	char cfmMemAccept[30];      /* 存储内存流水的变量 */
	char cfmAcceptFile[100],cancelAcceptFile[100],cancelOpCodeFile[100];
	char cancelOpCode[5],cfmOpCode[5];
	FILE *cfmFP,*cancelFP,*opcodeFP;

	memset(strFlag,0,sizeof(strFlag));
	memset(accept,0,sizeof(accept));
	memset(serviceType,0,sizeof(serviceType));
	memset(opCode,0,sizeof(opCode));
	memset(phoneNo,0,sizeof(phoneNo));
	memset(workNo,0,sizeof(workNo));
	memset(totalDate,0,sizeof(totalDate));
	memset(cfmAccept,0,sizeof(cfmAccept));
	memset(phoneNoTmp,0,sizeof(phoneNoTmp));
	memset(workNoTmp,0,sizeof(workNoTmp));
	memset(totalDateTmp,0,sizeof(totalDateTmp));
	memset(cfmAcceptTmp,0,sizeof(cfmAcceptTmp));
	memset(cfmMemAccept,0,sizeof(cfmMemAccept));
	memset(cfmAcceptFile,0,sizeof(cfmAcceptFile));
	memset(cancelAcceptFile,0,sizeof(cancelAcceptFile));
	memset(cancelOpCodeFile,0,sizeof(cancelOpCodeFile));
	memset(cancelOpCode,0,sizeof(cancelOpCode));
	memset(cfmOpCode,0,sizeof(cfmOpCode));
	
	if (-1 == Fget32(transIN, RedoFlag, 0, strFlag, 0))
	{
        	strcpy(strFlag, "");
	}

	Fget32(transIN, DPARM32_0, 0, serviceType, 0);
	Fget32(transIN, DPARM32_1, 0, opCode, 0);
	Fget32(transIN, DPARM32_2, 0, phoneNo, 0);
	Fget32(transIN, DPARM32_3, 0, workNo, 0);
	Fget32(transIN, DPARM32_4, 0, totalDate, 0);
	Fget32(transIN, DPARM32_5, 0, cfmAccept, 0);

	userlog("KBROS:serviceType[%s],opCode=[%s],phoneNo[%s],workNo[%s],totalDate[%s],cfmAccept[%s]\n",
		serviceType,opCode,phoneNo,workNo,totalDate,cfmAccept);

	strcpy(accept,GetMemAccept());
	if (strlen(accept) == 0)
	{
		fprintf(stderr, "取内存流水错误!\n");
		return -1;
	}

	/* 冲正确认服务的处理 */
	if(strcmp(serviceType,"cancel") == 0)
	{
		if (strcmp(strFlag,"redo") == 0) 
		{
			return 0;
		}
	
		spublicGetCfgDir(SOPCODEFILE, cancelOpCodeFile);
		if(!(opcodeFP=fopen(cancelOpCodeFile,"r")))
		{
			userlog("KBROS:打开操作代码对应配置文件错误!\n");
		}
		else
		{
			findOpCodeFlag=0;
			while(!feof(opcodeFP))
			{
				fscanf(opcodeFP,"%s %s\n",cancelOpCode,cfmOpCode);
				if(strcmp(cancelOpCode,opCode)==0)
				{
					findOpCodeFlag=1;
					break;
				}
			}
			if(findOpCodeFlag == 0)
			{
				userlog("KBROS:OPCODE在sCancelOpCode.cfg配置文件中不存在!\n");
			}
			fclose(opcodeFP);
		}

		spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
		strcat(cfmAcceptFile,cfmOpCode);
		spublicGetRedoDir(CANCELACCEPTFILE, cancelAcceptFile);
		strcat(cancelAcceptFile,cancelOpCode);


		if(!(cancelFP=fopen(cancelAcceptFile,"a+")))
		{
			userlog("KBROS:打开冲正操作流水对应文件错误!\n");
		}
		else
		{
			if(!(cfmFP=fopen(cfmAcceptFile,"r")))
			{
				userlog("KBROS:打开确认操作流水对应文件错误!\n");
			}
			else
			{
				findAcceptFlag=0;
				while(!feof(cfmFP))
				{
					/* 取出确认操作的相关数据，确认冲正业务对应的确认业务 */
					fscanf(cfmFP,"%s %s %s\n",
						cfmMemAccept,totalDateTmp,cfmAcceptTmp); 

					if(strcmp(totalDate,totalDateTmp)==0&&
						strcmp(cfmAccept,cfmAcceptTmp)==0)
					{
						fprintf(cancelFP,"%s %s %s %s\n",
							accept,cfmMemAccept,cfmAccept,totalDate); 
						findAcceptFlag=1;
						break;
					}
				}
				if(findAcceptFlag == 0)
				{
					userlog("KBROS:找不到冲正操作对应的确认操作!\n");
				}
				fclose(cfmFP);
				fclose(cancelFP);
			}
		}
	}    /* 冲正处理结束 */
	else if(strcmp(serviceType,"confirm") == 0)
	     /* 确认服务处理 */
	{
		spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
		strcat(cfmAcceptFile,opCode);

		if(!(cfmFP=fopen(cfmAcceptFile,"a+")))
		{
			userlog("KBROS:打开确认操作流水对应文件错误!\n");
		}
		else
		{
			fprintf(cfmFP,"%s %s %s\n",
				accept,totalDate,cfmAccept); 
			fclose(cfmFP);
		}
	}

	if (strcmp(strFlag,"redo") == 0) 
	{
		return 0;
	}

	
	fprintf(stdout,"<TitleMessage> %s %s %s\n",accept,strService,"fml32");
	userlog("<TitleMessage> %s %s %s\n",accept,strService,"fml32");
	
	if (Fprint32(transIN) == -1)
	{
		fprintf(stderr,"redoLog-Fprint errror:Ferror=%s\n",Fstrerror(Ferror));
		return -1;
	}
	
	return 0;
}

int changeFile(char *strStdout)
{
	char strDate[10],tempDate[30]; /* 串形日期 */
	char strNewStdout[60];
	char strCmd[100];

	memset(strDate,0,sizeof(strDate));
	memset(tempDate,0,sizeof(tempDate));
	memset(strNewStdout,0,sizeof(strNewStdout));
	memset(strCmd,0,sizeof(strCmd));

	strcpy(tempDate,GetMemDate());
	
	if(strlen(tempDate)!=0)
    	{
		/* 切换文件 */
		strcpy(strDate,tempDate);

		sprintf(strNewStdout,"%s.%s", strStdout, strDate);
		fprintf(stderr,"%s\n", strNewStdout);
		sprintf(strCmd, "cp %s %s", strStdout, strNewStdout);
		fprintf(stderr,"%s\n",strCmd);
		system(strCmd);
	
		userlog("strCmd:%s\n",strCmd);
		
		/* 清空stdout文件 */
		if (-1 == truncate(strStdout,0))
		{
			fprintf(stderr,"changeFile error\n");
			return -1;
		}
    	}
    	return 0;
}
