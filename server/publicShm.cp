/******************************************
 *      Prg:  publicShm.cp
 *     Edit:  dangsl
 *     Date:  2002.12.26
 ******************************************/

#ifndef __PUBLICBACK_H
#define __PUBLICBACK_H

#include "boss_srv.h"
#include "publicsrv.h"
#include "general.flds.h"
#include "general32.flds.h"
#include "fml.h"
#include "fml32.h"
#include "atmi.h"

#define WORKDIR "WORKDIR"

extern int errno;

int    sig_semid;
struct acceptshm *sig_shmp;

static struct sembuf op_lock={ 0,-1,0};
static struct sembuf op_unlock={ 0,1,0};
static struct sembuf op_endcreat={ 1,-1,SEM_UNDO };
static struct sembuf op_open={ 1,-1,0  };
static struct sembuf op_close={ 1,1,0  };

/* 取得共享内存键值 */
key_t
spublicGetShmKey()
{
	key_t kid;
	char filename[LINE];
	memset(filename, 0, sizeof(filename));
	userlog("[%s] SHMKDIR=[%s]\n",__FILE__,SHMKDIR);
	spublicGetCfgDir(SHMKDIR, filename);
	userlog("[%s] SHMKDIR_FILENAME=[%s]\n",__FILE__,filename);
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
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
		return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct comshm),
		PERMS|IPC_CREAT)) < 0 ) {
		userlog(" [%s][%s]get share memory id err!",__FILE__,__LINE__);
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
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
	        return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct comshm), 0)) < 0 ) {
		userlog(" [%s][%d]get share memory id err!",__FILE__,__LINE__);
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
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
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

    userlog("----- [%s]spublicGetDBP is starting ......\n",__FILE__);
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

	spublicGetCfgDir( TRANSSDBLOGIN, filename );
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

/*
void spublicGetKeyShm(char *connect_name)
{
	int  rtn_value;

        if ( (rtn_value = spublicDBLogin(connect_name)) < 0 ) {
                userlog("The label name is wrong!\n");
                return NULL;
        }
        else {
                userlog("The label is right!\n");
        }
}
*/


/* 取得日志共享内存键值 */
key_t
spublicLogGetShmKey()
{
	key_t kid;
	char filename[LINE];
	memset(filename, 0, sizeof(filename));
	userlog("---- before ShmKey file [%s]\n",filename);
	spublicGetCfgDir(SHMKLOGDIR, filename);
	userlog("---- ShmKey file [%s]\n",filename);
	kid = ftok( filename, SHMKEY);
	return kid;
}

/* 创建共享内存 */
struct logshm *
spublicLogCreatShm()
{
	int shmid;
	key_t shmkey,spublicLogGetShmKey();
	struct logshm *shmp;

	if (( shmkey = spublicLogGetShmKey()) < 0) {
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
		return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct logshm),
		PERMS|IPC_CREAT)) < 0 ) {
		userlog(" get share memory id err!",__FILE__,__LINE__);
		return NULL;
	}
	
	if ((shmp = (struct logshm *) shmat (shmid, (char *) 0,0))
		== (struct logshm *) -1) {
		userlog(" share memory shmat err!",__FILE__,__LINE__);
		return NULL;
	}

	return shmp;
}

/* 从共享内存中取数据 */
struct logshm *
spublicLogGetShm()
{
	int shmid;
	key_t shmkey,spublicLogGetShmKey();
	struct logshm *shmp;

	if (( shmkey = spublicLogGetShmKey()) < 0) {
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
	        return NULL;
	}
	
	if (( shmid = shmget(shmkey,sizeof(struct logshm), 0)) < 0 ) {
		userlog(" get share memory id err!",__FILE__,__LINE__);
		return NULL;
	}
	
	errno = 0;

	if ((shmp = (struct logshm *) shmat (shmid, 0,0)) == \
	(struct logshm *) -1) {
		userlog("shmat errno=%d\n",errno);
		userlog(" share memory shmat err!",__FILE__,__LINE__);
		return NULL;
	}

	return shmp;
}


/**********************************/
/* name:     spublicLogDelKeyShm  */
/* writer:   zhaohao              */
/* date:     2001.12.21           */
/* function: 删除共享内存中       */
/*           的数据               */
/**********************************/
int spublicLogDelKeyShm()
{
	int  shmid;
	int  rtn_value;
	key_t shmkey,spublicLogGetShmKey();

	rtn_value = 0;

	if (( shmkey = spublicLogGetShmKey()) < 0) {
		userlog(" [%s][%d]get share memory key err!",__FILE__,__LINE__);
		userlog("取共享内存键值失败!\n");
		rtn_value = -1;
		return rtn_value;
	}

	if ((shmid= shmget(shmkey,sizeof(struct logshm),PERMS|IPC_CREAT))< 0 ) {
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
spublicLogDtcShm(struct logshm *shmp)
{
        if (shmdt ((char *) shmp) < 0 ) {
                userlog(" detach share memory err ! ",__FILE__,__LINE__);
                return FALSE;
        }
        return TRUE;
}

/* 2001.12.21 */
/* 根据提供的操作码返回是否输出日志 */
int spublicLogGetFlag(char *codelevel) 
{
	struct logshm *shmp, *spublicLogGetShm();
	struct spublicloglevel sloglevel;
	int  count, i;

	shmp = spublicLogGetShm();
	if (shmp == NULL ) {
		userlog("get share memory err!\n");
		return(TRUE);
	}

	count = shmp->sloglevelno;
	for ( i=0; i<count; i++)
        {
		if (strcmp(shmp->sloglevel[i].codelevel, codelevel) == 0) {
			return(shmp->sloglevel[i].flag);
		}
	}

	if (i == count || count == 0) {
		return(TRUE);
	}

	spublicLogDtcShm( shmp );	
	
	return(TRUE);
}


/* 2001.12.21 */
/* 根据提供的操作码返回是否输出日志结构 */
struct spublicloglevel spublicLogGetDBP(char *codelevel) 
{
	struct logshm *shmp, *spublicLogGetShm();
	struct spublicloglevel sloglevel;
	int  count, i;

	shmp = spublicLogGetShm();
	if (shmp == NULL ) {
		userlog("get share memory err!\n");
		strcpy(sloglevel.codelevel, "00000");
		sloglevel.flag  = 1;
		return sloglevel;
	}

	count = shmp->sloglevelno;
	for ( i=0; i<count; i++)
        {
		if (strcmp(shmp->sloglevel[i].codelevel, codelevel) == 0) {
			sloglevel = shmp->sloglevel[i];

			return sloglevel;
		}
	}

	if (i == count || i == 0) {
		strcpy(sloglevel.codelevel, "00000");
		sloglevel.flag = 1;
	}

	spublicLogDtcShm( shmp );	
	
	return sloglevel;
}

/**********************************/
/* name:     spublicLogPutKeyShm  */
/* writer:   zhaohao              */
/* date:     2001.12.21           */
/* function: 将配置文件读入       */
/*           共享内存             */
/**********************************/
int spublicLogPutKeyShm()
{
	int  count;
	char tmp[LINE+1];

	char filename[LINE];
	FILE *fp;
	struct logshm *logshm;

	memset(tmp,0,LINE+1);

	spublicGetCfgDir( TRANSSLOGLEVEL, filename );
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
	if (( logshm = spublicLogCreatShm()) == NULL ) {
		userlog("Creat share memory err!",__FILE__,__LINE__);
		userlog("创建共享内存失败!\n",__FILE__,__LINE__);
		return(-1);
	}

        memset(logshm,0,sizeof(struct logshm));

	count=0;
	while (fgets(tmp,LINE,fp) != NULL)  
	{
		if (tmp[0] != COMMENT ) 
		{
			strncpy(logshm->sloglevel[count].codelevel, tmp, 5);
			logshm->sloglevel[count].flag = atoi(tmp+5);
printf("codelelve=[%s][%d]\n", logshm->sloglevel[count].codelevel,logshm->sloglevel[count].flag);
	
			count++;
		}
		memset(tmp,0,LINE+1);
	}
	logshm->sloglevelno = count;
	fclose(fp);

	spublicLogDtcShm(logshm);

	userlog("写入共享内存成功!");

	return(0);
}

#endif /* __PUBLICSHM_CP */



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

/*---------关闭已经打开的共享内存-------------------*/
int NCloseShm(const struct acceptshm *shmp){
	int	lret;
	lret=NDtcShm(shmp);
	return lret;
}
int 
NDtcShm(const struct acceptshm *shmp)
{
	if((shmdt((char *)shmp)) < 0 ) {
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
	static char temptime[30];

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

char *GetMemAccept()
{
	int semid;
	struct acceptshm *shmp;
	char temp[70],temptime[30];
	char semfile[LINE],shmfile[LINE];
	char tempcmd[LINE+LINE],tempstr[LINE];
	static char accept[LINE+LINE];

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
	sprintf(temp,"%ld",shmp->count);

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
	char semfile[LINE],shmfile[LINE],destdate[10],srcdate[20],tempdate[30];
	char tempcmd[LINE+LINE],tempstr[LINE];
	static char olddate[10];

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
	struct acceptshm *shmp;
	char accept[LINE];
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

	Trim(serviceType);
	Trim(opCode);
	Trim(phoneNo);
	Trim(workNo);
	Trim(totalDate);
	Trim(cfmAccept);

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
			if (strcmp(strFlag,"redo") == 0) 
			{
				memset(accept,0,sizeof(accept));
				Fget(transIN, DPARM_9, 0, accept, 0);
				fprintf(cfmFP,"%s %s %s\n",
					accept,totalDate,cfmAccept); 
			}
			else
			{
				userlog("KBORS:accept:[%s]\n",accept);
				fprintf(cfmFP,"%s %s %s\n",
					accept,totalDate,cfmAccept); 
			}
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

	Trim(serviceType);
	Trim(opCode);
	Trim(phoneNo);
	Trim(workNo);
	Trim(totalDate);
	Trim(cfmAccept);

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
			if (strcmp(strFlag,"redo") == 0) 
			{
				memset(accept,0,sizeof(accept));
				Fget32(transIN, DPARM32_9, 0, accept, 0);
				fprintf(cfmFP,"%s %s %s\n",
					accept,totalDate,cfmAccept); 
			}
			else
			{
				fprintf(cfmFP,"%s %s %s\n",
					accept,totalDate,cfmAccept); 
			}
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

char *getRedoAccept(char inPayAccept[12],char inTotalDate[9],
		char inOpCode[5],char inNowTotalDate[9],FBFR *transIN)
{	
	int  findAcceptFlag,findOpCodeFlag;
	char totalDate[9],cfmAccept[12],strFlag[5];
	char totalDateTmp[9];
	char cfmMemAccept[30],cancelMemAccept[30],cfmMemAcceptTmp[30];
	char cfmAcceptFile[100],cancelAcceptFile[100],cancelOpCodeFile[100];
	char cancelOpCode[5],cfmOpCode[5];
	static char cfmAcceptTmp[12];
	FILE *cfmFP,*cancelFP,*opcodeFP;

	memset(totalDate,0,sizeof(totalDate));
	memset(cfmAccept,0,sizeof(cfmAccept));
	memset(totalDateTmp,0,sizeof(totalDateTmp));
	memset(cfmAcceptTmp,0,sizeof(cfmAcceptTmp));
	memset(cfmMemAccept,0,sizeof(cfmMemAccept));
	memset(cancelMemAccept,0,sizeof(cancelMemAccept));
	memset(cfmMemAcceptTmp,0,sizeof(cfmMemAcceptTmp));
	memset(cfmAcceptFile,0,sizeof(cfmAcceptFile));
	memset(cancelAcceptFile,0,sizeof(cancelAcceptFile));
	memset(cancelOpCodeFile,0,sizeof(cancelOpCodeFile));
	memset(cancelOpCode,0,sizeof(cancelOpCode));
	memset(cfmOpCode,0,sizeof(cfmOpCode));
	
	userlog("KBROS:inPayAccept[%s],inTotalDate=[%s]\n",
		inPayAccept,inTotalDate);

	if (-1 == Fget(transIN, RedoFlag, 0, strFlag, 0))
	{
        	strcpy(strFlag, "");
	}
	/* 不是重做日志，或者不同的total_date不修改流水号 */
	if ((strcmp(strFlag,"redo") != 0) || 
		strcmp(inTotalDate,inNowTotalDate) != 0) 
	{
		return inPayAccept;
	}

	spublicGetCfgDir(SOPCODEFILE, cancelOpCodeFile);
	if(!(opcodeFP=fopen(cancelOpCodeFile,"r")))
	{
		userlog("KBROS:打开操作代码对应配置文件错误!\n");
		return inPayAccept;
	}
	else
	{
		findOpCodeFlag=0;
		while(!feof(opcodeFP))
		{
			fscanf(opcodeFP,"%s %s\n",cancelOpCode,cfmOpCode);
			if(strcmp(cancelOpCode,inOpCode)==0)
			{
				findOpCodeFlag=1;
				break;
			}
		}
		if(findOpCodeFlag == 0)
		{
			userlog("KBROS:OPCODE在sCancelOpCode.cfg配置文件中不存在!\n");
			fclose(opcodeFP);
			return inPayAccept;
		}
	}

	spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
	strcat(cfmAcceptFile,cfmOpCode);
	spublicGetRedoDir(CANCELACCEPTFILE, cancelAcceptFile);
	strcat(cancelAcceptFile,cancelOpCode);

	if(!(cancelFP=fopen(cancelAcceptFile,"r")))
	{
		userlog("KBROS:打开冲正操作流水对应文件错误!\n");
		return inPayAccept;
	}
	else
	{
		if(!(cfmFP=fopen(cfmAcceptFile,"r")))
		{
			userlog("KBROS:打开确认操作流水对应文件错误!\n");
			fclose(cancelFP);
			return inPayAccept;
		}

		findAcceptFlag=0;
		while(!feof(cancelFP))
		{
			fscanf(cancelFP,"%s %s %s %s\n",
				cancelMemAccept,cfmMemAccept,cfmAccept,totalDate); 

			if(strcmp(inTotalDate,totalDate)==0&&
				strcmp(inPayAccept,cfmAccept)==0)
			{
				findAcceptFlag=1;
				while(!feof(cfmFP))
				{
					fscanf(cfmFP,"%s %s %s\n",
						cfmMemAcceptTmp,totalDateTmp,cfmAcceptTmp); 
					if(strcmp(cfmMemAccept,cfmMemAcceptTmp) ==0)
					{
						fclose(cfmFP);
						fclose(cancelFP);
						return cfmAcceptTmp;
					}
				}
			}
		}
		if(findAcceptFlag == 1)
		{
			userlog("KBROS:找不到冲正操作对应的确认操作!\n");
			fclose(cfmFP);
			fclose(cancelFP);
			return inPayAccept;
		}
		else 
		{
			userlog("KBROS:找不到冲正操作对应的确认操作!\n");
			fclose(cancelFP);
			return inPayAccept;
		}
	}
}

char *getRedoAccept32(char inPayAccept[12],char inTotalDate[9],
		char inOpCode[5],char inNowTotalDate[9],FBFR32 *transIN)
{	
	int  findAcceptFlag,findOpCodeFlag;
	char totalDate[9],cfmAccept[12],strFlag[5];
	char totalDateTmp[9];
	char cfmMemAccept[30],cancelMemAccept[30],cfmMemAcceptTmp[30];
	char cfmAcceptFile[100],cancelAcceptFile[100],cancelOpCodeFile[100];
	char cancelOpCode[5],cfmOpCode[5];
	static char cfmAcceptTmp[12];
	FILE *cfmFP,*cancelFP,*opcodeFP;

	memset(totalDate,0,sizeof(totalDate));
	memset(cfmAccept,0,sizeof(cfmAccept));
	memset(totalDateTmp,0,sizeof(totalDateTmp));
	memset(cfmAcceptTmp,0,sizeof(cfmAcceptTmp));
	memset(cfmMemAccept,0,sizeof(cfmMemAccept));
	memset(cancelMemAccept,0,sizeof(cancelMemAccept));
	memset(cfmMemAcceptTmp,0,sizeof(cfmMemAcceptTmp));
	memset(cfmAcceptFile,0,sizeof(cfmAcceptFile));
	memset(cancelAcceptFile,0,sizeof(cancelAcceptFile));
	memset(cancelOpCodeFile,0,sizeof(cancelOpCodeFile));
	memset(cancelOpCode,0,sizeof(cancelOpCode));
	memset(cfmOpCode,0,sizeof(cfmOpCode));
	
	userlog("KBROS:inPayAccept[%s],inTotalDate=[%s]\n",
		inPayAccept,inTotalDate);

	if (-1 == Fget32(transIN, RedoFlag, 0, strFlag, 0))
	{
        	strcpy(strFlag, "");
	}
	/* 不是重做日志，或者不同的total_date不修改流水号 */
	if ((strcmp(strFlag,"redo") != 0) || 
		strcmp(inTotalDate,inNowTotalDate) != 0) 
	{
		return inPayAccept;
	}

	spublicGetCfgDir(SOPCODEFILE, cancelOpCodeFile);
	if(!(opcodeFP=fopen(cancelOpCodeFile,"r")))
	{
		userlog("KBROS:打开操作代码对应配置文件错误!\n");
		return inPayAccept;
	}
	else
	{
		findOpCodeFlag=0;
		while(!feof(opcodeFP))
		{
			fscanf(opcodeFP,"%s %s\n",cancelOpCode,cfmOpCode);
			if(strcmp(cancelOpCode,inOpCode)==0)
			{
				findOpCodeFlag=1;
				break;
			}
		}
		if(findOpCodeFlag == 0)
		{
			userlog("KBROS:OPCODE在sCancelOpCode.cfg配置文件中不存在!\n");
			fclose(opcodeFP);
			return inPayAccept;
		}
	}

	spublicGetRedoDir(CFMACCEPTFILE, cfmAcceptFile);
	strcat(cfmAcceptFile,cfmOpCode);
	spublicGetRedoDir(CANCELACCEPTFILE, cancelAcceptFile);
	strcat(cancelAcceptFile,cancelOpCode);

	if(!(cancelFP=fopen(cancelAcceptFile,"r")))
	{
		userlog("KBROS:打开冲正操作流水对应文件错误!\n");
		return inPayAccept;
	}
	else
	{
		if(!(cfmFP=fopen(cfmAcceptFile,"r")))
		{
			userlog("KBROS:打开确认操作流水对应文件错误!\n");
			fclose(cancelFP);
			return inPayAccept;
		}

		findAcceptFlag=0;
		while(!feof(cancelFP))
		{
			fscanf(cancelFP,"%s %s %s %s\n",
				cancelMemAccept,cfmMemAccept,cfmAccept,totalDate); 

			if(strcmp(inTotalDate,totalDate)==0&&
				strcmp(inPayAccept,cfmAccept)==0)
			{
				findAcceptFlag=1;
				while(!feof(cfmFP))
				{
					fscanf(cfmFP,"%s %s %s\n",
						cfmMemAcceptTmp,totalDateTmp,cfmAcceptTmp); 
					if(strcmp(cfmMemAccept,cfmMemAcceptTmp) ==0)
					{
						fclose(cfmFP);
						fclose(cancelFP);
						return cfmAcceptTmp;
					}
				}
			}
		}
		if(findAcceptFlag == 1)
		{
			userlog("KBROS:找不到冲正操作对应的确认操作!\n");
			fclose(cfmFP);
			fclose(cancelFP);
			return inPayAccept;
		}
		else 
		{
			userlog("KBROS:找不到冲正操作对应的确认操作!\n");
			fclose(cancelFP);
			return inPayAccept;
		}
	}
}
