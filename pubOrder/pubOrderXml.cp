/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#include "order_xml.h"

/*全局变量*/
char *g_PUB_envp;
char *g_PUB_CFGPATH;
int  g_PUB_RcvId,*g_PUB_SndId;
int  g_PUB_NodeNum;
char g_PUB_LogPath[DLDIRNAME];
char g_PUB_Source[DLDATASERVER];
char g_PUB_Dept[DLDATASERVER];

PUB_PassType g_PUB_DbCfg;
OrderTaskShmType *g_PUB_TaskShm;
OrderMsgInfoType g_PUB_RcvBuf,*g_PUB_SndBuf;
PubNodeCfg g_PUB_NodeCfg[NODE_ARRAY];

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

SQLDA *bind_dp;

/*读取环境变量*/
int readEnvCfg(i_env_name)
	char *i_env_name;
{
	
	if((g_PUB_envp=getenv("HOME"))==NULL){
           	return(-1);
	}	
	
	if((g_PUB_CFGPATH=getenv(i_env_name))==NULL){
           	return(-1);
	}
	return 0;
}

void fReadWord(buffer,ibegin,iend,fp)
	char* buffer;
	long ibegin;
	long iend;
	FILE* fp;
{
	long i,j=0;
	long pos;

	pos=ftell(fp);
	fseek(fp,ibegin-1,SEEK_SET);
	for (i=ibegin;i<=iend;i++){
		buffer[j++]=getc(fp);
	}
	buffer[j]='\0';
	fseek(fp,pos,SEEK_SET);
}

/*把文件指针移到单词第一个字符上*/
int fGetWordBegin(fp,beginc)
	FILE* fp;
	int* beginc;
{
	int c;

	if (isspace(*beginc)){
		while(!feof(fp)){
			c=getc(fp);
			/*不是空格*/
			if (isspace(c)==0) break;
		}
	}
	return ftell(fp);
}

int isWordChar(c)
	int c;
{
	if (isspace(c) || c =='<' || c=='/' || c=='>' || c=='='){
		return 0;
	}
	else{
		return 1;
	}

}

/*把文件指针移到单词最后个字符上*/
int fGetWordEnd(fp,beginc)
	FILE* fp;
	int beginc;
{
	int c=0,oldc;

	/*把iend指向元素名称的最后个字符，注意单词中的空格和转义符*/
	while(!feof(fp)){
		oldc=c;
		c=getc(fp);
		/*以引号开始，则到结束引号中间的任何字符都认为单词*/
		if (beginc=='\"'){
			if (c=='\"' && oldc!='\\'){
				/*单词结束*/
				break;
			}
		}
		else{
			if (!isWordChar(c)){
				/*定位都最后一个单词词尾*/
				fseek(fp,-1,SEEK_CUR);
				break;
			}
		}
	}
	return ftell(fp);
}

int fGetLabelHead(i_label_name,fp)
	const char *i_label_name;
	FILE *fp;
{
	long	i=0,j=0;
	int		c=0,oldc=0;
	char	v_tmp_buf[DLCFGATTR];
	
	init(v_tmp_buf);

	while(!feof(fp)){
		oldc=c;
		c=getc(fp);

		if (c=='<'){
			oldc=c;
			c=getc(fp);
			if (c!='/'){
				i=fGetWordBegin(fp,&c);
				j=fGetWordEnd(fp,c);
				
				fReadWord(v_tmp_buf,i,j,fp);
				
				if (strcmp(i_label_name,v_tmp_buf)==0)	{
					return 0;
				}		
			}
		}
	}

	return -1;	
}

int fGetLabelTail(i_label_name,fp)
	const char *i_label_name;
	FILE *fp;
{
	long	i=0,j=0;
	int		c=0,oldc=0;
	char	v_tmp_buf[DLCFGATTR];
	
	init(v_tmp_buf);

	while(!feof(fp)){
		oldc=c;
		c=getc(fp);

		if (c=='<'){
			oldc=c;
			c=getc(fp);
			if (c=='/'){
				oldc=c;
				c=getc(fp);				
				i=fGetWordBegin(fp,&c);
				j=fGetWordEnd(fp,c);
				
				fReadWord(v_tmp_buf,i,j,fp);
				
				if (strcmp(i_label_name,v_tmp_buf)==0)	{
					return 0;
				}		
			}
		}
	}

	return -1;			
}	

int fGetCfgValue(i_cfg_name,o_cfg_msg)
	char *i_cfg_name;
	PUB_CfgMsg *o_cfg_msg;
{
	char	v_tmp_buf[DLCFGATTR],v_attr1[DLCFGATTR],v_attr2[DLCFGATTR];
	FILE 	*fp;
	long	i=0,j=0;
	int		c=0,oldc=0,k=0,v_ret=0;
	
	init(v_tmp_buf);		init(v_attr1);		init(v_attr2);
	
	sprintf(v_tmp_buf,"%s%s/%s",g_PUB_envp,g_PUB_CFGPATH,PUB_CFG_FILE);
	fp=fopen(v_tmp_buf,"r");
	
	v_ret=fGetLabelHead(i_cfg_name,fp);
	if (v_ret<0)
		return -1;	
	
	k=0;
	while(!feof(fp)){
		if (k>DLMAXCFGNUM)
			return -2;
	
		oldc=c;
		c=getc(fp);
		if (c=='>'){
			break;	
		}
		else{
			if (c=='=') continue;
			
			if (c=='\"'){
				i=fGetWordBegin(fp,&c);
				j=fGetWordEnd(fp,c);

				i++;j--;
				
				fReadWord(v_attr2,i,j,fp);
				strcpy(o_cfg_msg[k].cfgName,v_attr1);
				strcpy(o_cfg_msg[k].cfgValue,v_attr2);
				k++;
				continue;
			}
			i=fGetWordBegin(fp,&c);
			j=fGetWordEnd(fp,c);			
			fReadWord(v_attr1,i,j,fp);					
		}
	}
	
	v_ret=fGetLabelTail(i_cfg_name,fp);
	if (v_ret<0)
		return -3;	
		
	fclose(fp);
	return 0;
}

char *strupr(src)
	char *src;
{
	int i=0;

	for (i=0;i<strlen(src);i++){
		if (*(src+i)>=97 && *(src+i)<=122)
			*(src+i)=*(src+i)-32;
	}
	
	return src;
}
/*
char *Sltrim(char *src){
	int i=0;

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) != ' ')
			return src+i;
	}

	if (i == strlen(src)) {
		*src = '\0';
	}

	return src;
}

char *Srtrim(char *src){
	int i=0;

	for(i=strlen(src); i>0; i--) {
		if (*(src+i-1) != ' ') {
			*(src+i) = '\0';
			break;
		}
	}
	if (i == 0) {
		*src = '\0';
	}

	return src;
}
*/
char *strim(src)
	char *src;
{
	int i=0;
	char v_tmp_buf[100000];
	init(v_tmp_buf);
	strcpy(v_tmp_buf,src);

	for(i=strlen(v_tmp_buf); i>0; i--) {
		if (*(v_tmp_buf+i-1) != ' ') {
			*(v_tmp_buf+i) = '\0';
			break;
		}
	}

	if (i == 0) {
		*v_tmp_buf = '\0';
		strcpy(src,v_tmp_buf);
		return src;
	}

	for(i=0; i<strlen(v_tmp_buf); i++) {
		if (*(v_tmp_buf+i) != ' '){
			strcpy(src,v_tmp_buf+i);
			return src;
		}
	}
	
	if (i == strlen(v_tmp_buf)) {
		*v_tmp_buf = '\0';
		strcpy(src,v_tmp_buf+i);
		return src;
	}
}

/* 去掉字符串中的空格，若字符串内容全部为空格，则直接返回原串 */
char *Coltrim(src)
	char *src;
{
	int i=0;
	char v_tmp_buf[100000];
	init(v_tmp_buf);
	strcpy(v_tmp_buf,src);

	for(i=strlen(v_tmp_buf); i>0; i--) {
		if (*(v_tmp_buf+i-1) != ' ') {
			*(v_tmp_buf+i) = '\0';
			break;
		}
	}

	if (i == 0) {
		return src;
	}

	for(i=0; i<strlen(v_tmp_buf); i++) {
		if (*(v_tmp_buf+i) != ' '){
			strcpy(src,v_tmp_buf+i);
			break;
		}
	}
/*
	if (strcmp(src,"null")==0){
		*v_tmp_buf = '\0';
		strcpy(src,v_tmp_buf);
		return src;
	}
	else
*/
	return src;
}


char *getSystemTime(){
	struct tm *p;
	time_t t;
	char datetime[DLDATETIME];

	init(datetime);
	t = time(NULL);
	p = localtime(&t);
	snprintf(datetime,DLDATETIME,"%d%02d%02d %02d:%02d:%02d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	datetime[DLDATETIME-1]= '\0';
	return datetime;	
}

int getCurDate(){
	struct tm *p;
	time_t t;
	int curDate;

	t = time(NULL);
	p = localtime(&t);
	curDate=(p->tm_year+1900)*10000+(p->tm_mon+1)*100+p->tm_mday;
	return curDate;	
}

int readProgCfg(i_cfg_name,o_cfg_msg)
	char *i_cfg_name;
	PUB_CfgMsg *o_cfg_msg;
{
	int  v_ret;
	PUB_CfgMsg v_chn_cfg_msg[DLMAXCFGNUM];
	memset(v_chn_cfg_msg,0,DLMAXCFGNUM*sizeof(PUB_CfgMsg));

	v_ret=fGetCfgValue(PUB_PROG_PARAMETER,v_chn_cfg_msg);
	if (v_ret<0){
		printf("Get Config Values Error[%d]!!\n",v_ret);
		return -1;	
	}
	strcpy(g_PUB_LogPath,v_chn_cfg_msg[0].cfgValue);

	v_ret=fGetCfgValue(i_cfg_name,o_cfg_msg);
	if (v_ret<0){
		printf("Get Config Values Error[%d]!!\n",v_ret);
		return -2;	
	}

	if (strcmp(o_cfg_msg[0].cfgName,PUB_LOGPATH_ATTRNAME)==0)
		strcpy(g_PUB_LogPath,o_cfg_msg[0].cfgValue);
	return 0;
}

int startDaemon(i_prog_label)
	char *i_prog_label;
{
	pid_t pidFirst,pidDaemon;

	pidFirst = fork();
	if (pidFirst<0){
		errLog(i_prog_label,"ERROR:ERRORCODE:[%d],ERRORINFO:[Fork error;Can't start daemon;Program exit!]\n\
			SUGGEST:[Check os;please Contact administrators]\n",FORKERROR);
		exit(1);
	}

	else if (pidFirst>0){
		DEBUG(i_prog_label,"first parent pid[%d] ppid=[%d] quit!\n",getpid(),getppid());
		exit(0);
	}

	setsid();
	signal(SIGHUP,SIG_IGN);
	
	pidDaemon = fork();
	if(pidDaemon<0){
		errLog(i_prog_label,"ERROR:ERRORCODE:[%d],ERRORINFO:[Fork error;Can't start daemon;Program exit!]\n\
			SUGGEST:[Check os,please Contact administrators!]\n",FORKERROR);
		exit(1);
	}
	else if(pidDaemon>0){
		DEBUG(i_prog_label,"daemon parent pid[%d] ppid=[%d] quit!\n",getpid(),getppid());
		exit(0);
	}

	return 0;
}

int readDBCfg(i_db_label)
	char *i_db_label;
{
	int  v_ret;
	PUB_CfgMsg v_db_msg[DLMAXCFGNUM];
	char v_src[PUB_MAXENCRYPTLEN],v_dest[PUB_MAXENCRYPTLEN];
	
	init(v_db_msg);		init(v_src);		init(v_dest);
	
	v_ret=fGetCfgValue(i_db_label,v_db_msg);
	if (v_ret<0){
		printf("Get Config Values Error!!\n");
		return -1;	
	}
	
	strcpy(g_PUB_DbCfg.srvName,v_db_msg[0].cfgValue);
	strcpy(g_PUB_DbCfg.dataBase,v_db_msg[1].cfgValue);
	strcpy(g_PUB_DbCfg.loginNo,v_db_msg[2].cfgValue);
	strcpy(v_src,v_db_msg[3].cfgValue);
	spublicEnDePasswd(v_src,v_dest,PUB_MAINKEY,0);
	strcpy(g_PUB_DbCfg.passWord,v_dest);
	return 0;
}

int connectDB(i_prog_label,i_label_flag,i_db_label)
	char *i_prog_label;
	int  i_label_flag;
	char *i_db_label;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char this_user[DLDATAUSER];
		char this_pwd[DLDATAPASSWD];
		char this_db[DLDATABASE];
		char this_srv[DLDATASERVER];
		char v_db_label[DLDATABASE];
    EXEC SQL END DECLARE SECTION;

	init(this_user);		init(this_pwd);		init(this_db);		init(this_srv);
 
	strcpy(this_srv,g_PUB_DbCfg.srvName);
	strcpy(this_db,g_PUB_DbCfg.dataBase);
	strcpy(this_user,g_PUB_DbCfg.loginNo);
	strcpy(this_pwd,g_PUB_DbCfg.passWord);
	
	if (i_label_flag==PUB_DB_LABEL_N){
		EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd USING :this_srv;
		if(SQLCODE!=SQLOK){
			errLog(i_prog_label,"SQLCODE[%d],SQLERR[%s]\n",SQLCODE,SQLERRMSG);
			return -1;
		}
	}
	else{
		init(v_db_label);
		strcpy(v_db_label,i_db_label);
		EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd AT :v_db_label USING :this_srv;
		if(SQLCODE!=SQLOK){
			errLog(i_prog_label,"SQLCODE[%d],SQLERR[%s]\n",SQLCODE,SQLERRMSG);
			return -2;
		}
	}
	
	return 0;
}

int PubReadNodeMsg(i_prog_label,i_process_num)
	char *i_prog_label;
{	
	int	i;

	memset(g_PUB_NodeCfg,0,NODE_ARRAY*PUBNODECFGSIZE);
	
	g_PUB_NodeNum=0;

	for (i=0;i<i_process_num;i++){
		if (i==i_process_num-1)
			strcpy(g_PUB_NodeCfg[g_PUB_NodeNum].tableLebel,"XX");
		else
			sprintf(g_PUB_NodeCfg[g_PUB_NodeNum].tableLebel,"%02d",i);
		
		g_PUB_NodeCfg[g_PUB_NodeNum].status=PUB_STATUS_N;
		g_PUB_NodeNum++;
		
	}

	return 0;	
}

void PubFreeSysRes(i_process_num,i_mainmsg_label,i_submsg_label)
	int i_process_num;
	int i_mainmsg_label;
	int i_submsg_label;
{
	int i;

	g_PUB_RcvId=msgget(i_mainmsg_label,0600|IPC_CREAT);
	if(g_PUB_RcvId<0)
		exit(1);

	g_PUB_SndId = (int *)malloc(i_process_num*sizeof(int));
	
	memset(g_PUB_SndId,0,i_process_num*sizeof(int));
	
	for (i=0;i<i_process_num;i++){
		g_PUB_SndId[i]=msgget(i_submsg_label+i,0600|IPC_CREAT);
		if(g_PUB_SndId[i]<0)
			exit(1);
	}

	/*删除消息队列*/
	msgctl(g_PUB_RcvId,IPC_RMID,NULL);

	for (i=0;i<i_process_num;i++){
		msgctl(g_PUB_SndId[i],IPC_RMID,NULL);
	}	

	free(g_PUB_SndBuf);
	munmap((void *)g_PUB_TaskShm,i_process_num*sizeof(OrderTaskShmType));
	
	return;	
}

int PubInitMain(i_prog_label,i_process_num,i_mainmsg_label,i_submsg_label)
	char *i_prog_label;
	int i_process_num;
	int i_mainmsg_label;
	int i_submsg_label;
{
	int tsize,i;
	
	/*初始化进程信息数组*/
	tsize=i_process_num*sizeof(OrderTaskShmType);
	g_PUB_TaskShm=(OrderTaskShmType *)mmap(NULL,tsize,PUB_MAPPARM,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(g_PUB_TaskShm == MAP_FAILED){
		errLog(i_prog_label,"PubInitMain Error[%s][%d]!\n",strerror(errno),errno);
		exit(1);
	}
	memset(g_PUB_TaskShm,0,tsize);	
	
	/*初始化消息队列*/
	g_PUB_RcvId=msgget(i_mainmsg_label,0600|IPC_CREAT);
	if(g_PUB_RcvId<0){
		errLog(i_prog_label,"PubInitMain Child:msgrcv() Error[%s][%d]!\n",strerror(errno),errno);
		exit(1);
	}
	memset(&g_PUB_RcvBuf,0x0,ORDERMSGINFOTYPESIZE);
	g_PUB_RcvBuf.mtype=1;
	g_PUB_RcvBuf.refresh=PUB_REFRESH_N;

	g_PUB_SndId = (int *)malloc(i_process_num*sizeof(int));
	g_PUB_SndBuf = (OrderMsgInfoType *)malloc(i_process_num*ORDERMSGINFOTYPESIZE);
	
	memset(g_PUB_SndId,0,i_process_num*sizeof(int));
	memset(g_PUB_SndBuf,0,i_process_num*ORDERMSGINFOTYPESIZE);
	
	for (i=0;i<i_process_num;i++){
		g_PUB_SndId[i]=msgget(i_submsg_label+i,0600|IPC_CREAT);
		if(g_PUB_SndId[i]<0)
			exit(1);
		g_PUB_SndBuf[i].mtype=1;
		g_PUB_SndBuf[i].msgPos=i;
	}
	
	return 0;	
}

int PubInitTaskListOneDB(i_prog_label,i_process_num,i_pubdeal_func)
	char *i_prog_label;
	int i_process_num;
	void (*i_pubdeal_func());
{
	int i;
	pid_t pid;
	
	for (i=0;i<i_process_num;i++){
		g_PUB_TaskShm[i].pos=i;

		if ((pid=fork())<0){
			errLog(i_prog_label,"PubInitTaskList SubProcess[i] Error[%s][%d]!\n",i,strerror(errno),errno);
			exit(1);	
		}
		else if (pid>0){
			g_PUB_TaskShm[i].pid=pid;	
			DEBUG(i_prog_label,"Parent[%ld] Assign Child[%ld]!\n",getpid(),pid);
		}	
		else if (pid==0){
			PubDealTaskOneDB(i_prog_label,i_process_num,i_pubdeal_func);
		}
	}
			
	return 0;
}

int PubInitTaskListTwoDB(i_prog_label,i_process_num,i_pubdeal_func)
	char *i_prog_label;
	int i_process_num;
	void (*i_pubdeal_func());
{
	int i;
	pid_t pid;
	
	for (i=0;i<i_process_num;i++){
		g_PUB_TaskShm[i].pos=i;

		if ((pid=fork())<0){
			errLog(i_prog_label,"PubInitTaskList SubProcess[i] Error[%s][%d]!\n",i,strerror(errno),errno);
			exit(1);	
		}
		else if (pid>0){
			g_PUB_TaskShm[i].pid=pid;	
			DEBUG(i_prog_label,"Parent[%ld] Assign Child[%ld]!\n",getpid(),pid);
		}	
		else if (pid==0){
			PubDealTaskTwoDB(i_prog_label,i_process_num,i_pubdeal_func);
		}
	}
			
	return 0;
}

int PubDealTaskOneDB(i_prog_label,i_process_num,i_pubdeal_func)
	char *i_prog_label;
	int i_process_num;
	void (*i_pubdeal_func());
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_db_name[DLDATASERVER];
    EXEC SQL END DECLARE SECTION;		
	pid_t pid;
	int i,v_ret,curMsgPos;

	pid=getpid();
	
	DEBUG(i_prog_label,"Start Child %ld!\n",pid);
	
	i=0;
	while(1){
		if(pid==g_PUB_TaskShm[i].pid){
			curMsgPos=g_PUB_TaskShm[i].pos;
			break;
		}
		i=(i+1)%i_process_num;	
	}
	
	DEBUG(i_prog_label,"Child Find Pos [%ld][%d]!\n",pid,curMsgPos);

	strcpy(v_db_name,g_PUB_Source);

	/*子进程连接数据库*/
	v_ret=readDBCfg(v_db_name);
	if (v_ret<0){
		errLog(i_prog_label,"Get DB Config Error!!\n");
		exit(-1);	
	}

	v_ret=connectDB(i_prog_label,PUB_DB_LABEL_N,v_db_name);
	if(v_ret!=0){
		errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500000,v_ret);
		exit(0);
	}

	strcpy(g_PUB_RcvBuf.tableLebel,"");
	g_PUB_RcvBuf.msgPos=curMsgPos;
	g_PUB_RcvBuf.refresh=PUB_REFRESH_I;

	while(1){
		if(msgsnd(g_PUB_RcvId,&g_PUB_RcvBuf,ORDERMSGINFOTYPESIZE-sizeof(long),0)<0){
			errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500002,errno);
			exit(0);
		}
		
		memset(&(g_PUB_SndBuf[curMsgPos]),0x0,ORDERMSGINFOTYPESIZE);
		
		if(msgrcv(g_PUB_SndId[curMsgPos],&(g_PUB_SndBuf[curMsgPos]),ORDERMSGINFOTYPESIZE-sizeof(long),0,MSG_NOERROR)<0){
			errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500003,errno);
			exit(0);
		}	
/*		
		DEBUG(i_prog_label,"Child  RCV msgPos=[%2d] tableLebel=[%s] refresh=[%c]\n",curMsgPos,
			g_PUB_SndBuf[curMsgPos].tableLebel,g_PUB_SndBuf[curMsgPos].refresh);	
*/
		g_PUB_RcvBuf.refresh=g_PUB_SndBuf[curMsgPos].refresh;
		strcpy(g_PUB_RcvBuf.tableLebel,g_PUB_SndBuf[curMsgPos].tableLebel);

		/*主进程没有分配任务,子进程空转*/
		if(g_PUB_SndBuf[curMsgPos].refresh==PUB_REFRESH_I){
			usleep(10);
			continue;
		}

		/*处理过程*/
		i_pubdeal_func(i_prog_label,g_PUB_RcvBuf.tableLebel);

		usleep(10);
	}
	exit(0);
}

int PubDealTaskTwoDB(i_prog_label,i_process_num,i_pubdeal_func)
	char *i_prog_label;
	int i_process_num;
	void (*i_pubdeal_func());
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_db_source[DLDATASERVER];
		char v_db_dest[DLDATASERVER];
    EXEC SQL END DECLARE SECTION;		
	pid_t pid;
	int i,v_ret,curMsgPos;

	pid=getpid();
	
	DEBUG(i_prog_label,"Start Child %ld!\n",pid);
	
	i=0;
	while(1){
		if(pid==g_PUB_TaskShm[i].pid){
			curMsgPos=g_PUB_TaskShm[i].pos;
			break;
		}
		i=(i+1)%i_process_num;	
	}
	
	DEBUG(i_prog_label,"Child Find Pos [%ld][%d]!\n",pid,curMsgPos);

	strcpy(v_db_source,g_PUB_Source);
	strcpy(v_db_dest,g_PUB_Dept);	

	/*子进程连接数据库*/
	v_ret=readDBCfg(v_db_source);
	if (v_ret<0){
		errLog(i_prog_label,"Get DB Config Error!!\n");
		exit(-1);	
	}

	v_ret=connectDB(i_prog_label,PUB_DB_LABEL_Y,v_db_source);
	if(v_ret!=0){
		errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500000,v_ret);
		exit(0);
	}
		
	v_ret=readDBCfg(v_db_dest);
	if (v_ret<0){
		errLog(i_prog_label,"Get DB Config Error!!\n");
		exit(-1);	
	}

	v_ret=connectDB(i_prog_label,PUB_DB_LABEL_Y,v_db_dest);
	if(v_ret!=0){
		errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500001,v_ret);
		exit(0);
	}	

	strcpy(g_PUB_RcvBuf.tableLebel,"");
	g_PUB_RcvBuf.msgPos=curMsgPos;
	g_PUB_RcvBuf.refresh=PUB_REFRESH_I;

	while(1){
		if(msgsnd(g_PUB_RcvId,&g_PUB_RcvBuf,ORDERMSGINFOTYPESIZE-sizeof(long),0)<0){
			errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500002,errno);
			exit(0);
		}
		
		memset(&(g_PUB_SndBuf[curMsgPos]),0x0,ORDERMSGINFOTYPESIZE);
		
		if(msgrcv(g_PUB_SndId[curMsgPos],&(g_PUB_SndBuf[curMsgPos]),ORDERMSGINFOTYPESIZE-sizeof(long),0,MSG_NOERROR)<0){
			errLog(i_prog_label,"SubProcess[%d][%d][%d]\n",curMsgPos,-500003,errno);
			exit(0);
		}	
/*		
		DEBUG(i_prog_label,"Child  RCV msgPos=[%2d] tableLebel=[%s] refresh=[%c]\n",curMsgPos,
			g_PUB_SndBuf[curMsgPos].tableLebel,g_PUB_SndBuf[curMsgPos].refresh);	
*/
		g_PUB_RcvBuf.refresh=g_PUB_SndBuf[curMsgPos].refresh;
		strcpy(g_PUB_RcvBuf.tableLebel,g_PUB_SndBuf[curMsgPos].tableLebel);

		/*主进程没有分配任务,子进程空转*/
		if(g_PUB_SndBuf[curMsgPos].refresh==PUB_REFRESH_I){
			usleep(10);
			continue;
		}

		/*处理过程*/
		i_pubdeal_func(i_prog_label,v_db_source,v_db_dest,g_PUB_RcvBuf.tableLebel);

		usleep(10);
	}
	exit(0);
}

int PubAssignTask(i_prog_label,i_process_num)
	char *i_prog_label;
	int i_process_num;
{
	int i=0,curMsgPos=0,nodePos=0;

	while(1){
		memset(&g_PUB_RcvBuf,0x0,ORDERMSGINFOTYPESIZE);
		if(msgrcv(g_PUB_RcvId,&g_PUB_RcvBuf,ORDERMSGINFOTYPESIZE-sizeof(long),0,0)<0){
			errLog(i_prog_label,"PubAssignTask Parent:msgrcv SubProcess[%d][%d][%d]\n",g_PUB_RcvBuf.msgPos,-510000,errno);
			exit(0);
		}			
/*
		DEBUG(i_prog_label,"Parent RCV msgPos=[%2d] tableLebel=[%s] refresh=[%c]\n",g_PUB_RcvBuf.msgPos,g_PUB_RcvBuf.tableLebel,g_PUB_RcvBuf.refresh);
*/
		/*从子进程反馈信息中获取子进程工作之后的状态信息*/
		if (g_PUB_RcvBuf.refresh==PUB_REFRESH_N){
			for(i=0;i<g_PUB_NodeNum;i++){
				if(strncmp(g_PUB_NodeCfg[i].tableLebel,g_PUB_RcvBuf.tableLebel,DLTABLELABEL)==0){
					g_PUB_NodeCfg[i].status=PUB_STATUS_N;
					break;
				}
			}			
		}
			
		/*循环查找没有占用的任务，如果没有查找到，则一直等待*/
		while(g_PUB_NodeCfg[nodePos].status==PUB_STATUS_Y){
			nodePos=(nodePos+1)%g_PUB_NodeNum;
		}
		
		/*如果分表已经分配，则继续做下一个分表
		if (g_PUB_NodeCfg[nodePos].status==PUB_STATUS_Y){
			nodePos=(nodePos+1)%g_PUB_NodeNum;
			curMsgPos=g_PUB_RcvBuf.msgPos;
			strcpy(g_PUB_SndBuf[curMsgPos].tableLebel,"");
			g_PUB_SndBuf[curMsgPos].refresh=PUB_REFRESH_I;
			if(msgsnd(g_PUB_SndId[curMsgPos],&(g_PUB_SndBuf[curMsgPos]),ORDERMSGINFOTYPESIZE-sizeof(long),0)<0){
				errLog("parent:idle msgsnd() error!\n");
				exit(0);
			}
			continue;		
		}
		*/
		
		/*从分表任务列表中提取分表任务信息发送给子进程*/
		g_PUB_NodeCfg[nodePos].status=PUB_STATUS_Y;
		curMsgPos=g_PUB_RcvBuf.msgPos;
		strncpy(g_PUB_SndBuf[curMsgPos].tableLebel,g_PUB_NodeCfg[nodePos].tableLebel,DLTABLELABEL);
		g_PUB_SndBuf[curMsgPos].msgPos=curMsgPos;
		g_PUB_SndBuf[curMsgPos].refresh=PUB_REFRESH_N;
		
		if(msgsnd(g_PUB_SndId[curMsgPos],&(g_PUB_SndBuf[curMsgPos]),ORDERMSGINFOTYPESIZE-sizeof(long),0)<0){
			errLog(i_prog_label,"PubAssignTask Parent:msgsnd SubProcess[%d][%d][%d]\n",g_PUB_RcvBuf.msgPos,-510001,errno);
			exit(0);
		}
		
		nodePos=(nodePos+1)%g_PUB_NodeNum;
		usleep(10);
	}

	return 0;	
}

int OrderInitMsgBody(vMsgBodyType,vOrderType)
MsgBodyType	*vMsgBodyType;
char *vOrderType;
{
	DataOrderType *vDataOrder;
	BusiOrderType *vBusiOrder;
	
	if (vMsgBodyType==NULL)
		return -1;

	vMsgBodyType->DataOrder=NULL;	
	vMsgBodyType->BusiOrder=NULL;

	if (strcmp(vOrderType,ORDERTYPEDATA)==0){
		vDataOrder=(DataOrderType *)malloc(DATAORDERTYPESIZE);
		memset(vDataOrder,0,DATAORDERTYPESIZE);	
		vMsgBodyType->DataOrder=vDataOrder;
	}
	else if (strcmp(vOrderType,ORDERTYPEBUSI)==0){
		vBusiOrder=(BusiOrderType *)malloc(BUSIORDERTYPESIZE);
		memset(vBusiOrder,0,BUSIORDERTYPESIZE);	
		vMsgBodyType->BusiOrder=vBusiOrder;
	}
	else
		return -2;
		
	return 0;
}

int OrderDestroyMsgBody(vMsgBodyType)
MsgBodyType	*vMsgBodyType;
{
	DataItemListType	*pDataItemListType;
	ParameterListType	*pParameterListType;

	if (vMsgBodyType==NULL)
		return 0;

	if (vMsgBodyType->DataOrder!=NULL){
		while (vMsgBodyType->DataOrder->PrimaryKeyList!=NULL){
			pDataItemListType=vMsgBodyType->DataOrder->PrimaryKeyList->next;
			free(vMsgBodyType->DataOrder->PrimaryKeyList->DataItem);
			free(vMsgBodyType->DataOrder->PrimaryKeyList);
			vMsgBodyType->DataOrder->PrimaryKeyList=pDataItemListType;
		}
		
		while (vMsgBodyType->DataOrder->DataItemList!=NULL){		
			pDataItemListType=vMsgBodyType->DataOrder->DataItemList->next;	
			free(vMsgBodyType->DataOrder->DataItemList->DataItem);
			free(vMsgBodyType->DataOrder->DataItemList);
			vMsgBodyType->DataOrder->DataItemList=pDataItemListType;	
		}

		free(vMsgBodyType->DataOrder);
		vMsgBodyType->DataOrder=NULL;
	
		return 0;
	}

	if (vMsgBodyType->BusiOrder!=NULL){
		while (vMsgBodyType->BusiOrder->ParameterList!=NULL){
			pParameterListType=vMsgBodyType->BusiOrder->ParameterList->next;
			free(vMsgBodyType->BusiOrder->ParameterList->ParameterItem);
			free(vMsgBodyType->BusiOrder->ParameterList);
			vMsgBodyType->BusiOrder->ParameterList=pParameterListType;
		}

		free(vMsgBodyType->BusiOrder);
		vMsgBodyType->BusiOrder=NULL;
		return 0;
	}	

	return 0;
}

int OrderSetTableName(vMsgBodyType,vTableName)
MsgBodyType	*vMsgBodyType;
char *vTableName;
{
	if (vMsgBodyType->DataOrder==NULL)
		return -1;
	
	strcpy(vMsgBodyType->DataOrder->TableName,vTableName);
	
	return 0;
}

int OrderSetPrimaryKey(vMsgBodyType,vDataName,vDataFormat,vDataValue)
MsgBodyType	*vMsgBodyType;
char *vDataName;
int  vDataFormat;
char *vDataValue;
{
	DataItemType *vDataItemType;
	DataItemListType *pDataItemListType,*vDataItemListType;
	
	if (vMsgBodyType->DataOrder==NULL)
		return -1;
	
	vDataItemType=(DataItemType *)malloc(DATAITEMTYPESIZE);
	memset(vDataItemType,0,DATAITEMTYPESIZE);		

	strcpy(vDataItemType->DataName,vDataName);
	vDataItemType->DataFormat=vDataFormat;
	strcpy(vDataItemType->DataValue,vDataValue);

	vDataItemListType=(DataItemListType *)malloc(DATAITEMLISTTYPESIZE);
	memset(vDataItemListType,0,DATAITEMLISTTYPESIZE);
	
	vDataItemListType->DataItem=vDataItemType;
	
	if (vMsgBodyType->DataOrder->PrimaryKeyList==NULL){
		vMsgBodyType->DataOrder->PrimaryKeyList=vDataItemListType;
	}
	else{
		pDataItemListType=vMsgBodyType->DataOrder->PrimaryKeyList;
		while(pDataItemListType->next!=NULL){
			pDataItemListType=pDataItemListType->next;
		}

		pDataItemListType->next=vDataItemListType;		
	}
	
	return 0;
}

int OrderSetDataItem(vMsgBodyType,vDataName,vDataFormat,vDataValue)
MsgBodyType	*vMsgBodyType;
char *vDataName;
int  vDataFormat;
char *vDataValue;
{
	DataItemType *vDataItemType;
	DataItemListType *pDataItemListType,*vDataItemListType;
	
	if (vMsgBodyType->DataOrder==NULL)
		return -1;
		
	vDataItemType=(DataItemType *)malloc(DATAITEMTYPESIZE);
	memset(vDataItemType,0,DATAITEMTYPESIZE);		

	strcpy(vDataItemType->DataName,vDataName);
	vDataItemType->DataFormat=vDataFormat;
	strcpy(vDataItemType->DataValue,vDataValue);

	vDataItemListType=(DataItemListType *)malloc(DATAITEMLISTTYPESIZE);
	memset(vDataItemListType,0,DATAITEMLISTTYPESIZE);
	
	vDataItemListType->DataItem=vDataItemType;
	
	if (vMsgBodyType->DataOrder->DataItemList==NULL){
		vMsgBodyType->DataOrder->DataItemList=vDataItemListType;
	}
	else{
		pDataItemListType=vMsgBodyType->DataOrder->DataItemList;
		while(pDataItemListType->next!=NULL){
			pDataItemListType=pDataItemListType->next;
		}

		pDataItemListType->next=vDataItemListType;		
	}

	return 0;
}

int OrderSetServiceInfo(vMsgBodyType,vCallServiceName,vSendParaNum,vRecpParaNum)
MsgBodyType	*vMsgBodyType;
char *vCallServiceName;
int  vSendParaNum;
int  vRecpParaNum;
{
	if (vMsgBodyType->BusiOrder==NULL)
		return -1;
	
	strcpy(vMsgBodyType->BusiOrder->CallServiceName,vCallServiceName);
	vMsgBodyType->BusiOrder->SendParaNum=vSendParaNum;
	vMsgBodyType->BusiOrder->RecpParaNum=vRecpParaNum;
	return 0;
}

int OrderSetParameter(vMsgBodyType,vParameterName,vParameterValue)
MsgBodyType	*vMsgBodyType;
char *vParameterName;
char *vParameterValue;
{
	ParameterItemType *vParameterItemType;
	ParameterListType *pParameterListType,*vParameterListType;
	
	if (vMsgBodyType->BusiOrder==NULL)
		return -1;

	vParameterItemType=(ParameterItemType *)malloc(PARAMETERITEMTYPESIZE);
	memset(vParameterItemType,0,PARAMETERITEMTYPESIZE);		

	strcpy(vParameterItemType->ParameterName,vParameterName);
	strcpy(vParameterItemType->ParameterValue,vParameterValue);

	vParameterListType=(ParameterListType *)malloc(PARAMETERLISTTYPESIZE);
	memset(vParameterListType,0,PARAMETERLISTTYPESIZE);
	
	vParameterListType->ParameterItem=vParameterItemType;

	if (vMsgBodyType->BusiOrder->ParameterList==NULL){
		vMsgBodyType->BusiOrder->ParameterList=vParameterListType;
	}
	else{
		pParameterListType=vMsgBodyType->BusiOrder->ParameterList;
		while(pParameterListType->next!=NULL){
			pParameterListType=pParameterListType->next;
		}

		pParameterListType->next=vParameterListType;		
	}

	return 0;
}

int OrderGenXml(vMsgBodyType,vXmlString)
MsgBodyType	*vMsgBodyType;
char *vXmlString;
{
	char vTmpBuf[DLINTERFACEARRAY*DLINTERFACEDATA];
	DataItemListType *pDataItemListType;
	ParameterListType *pParameterListType;
	int  vFindFlag,vXmlStringLength,i;

	if (vMsgBodyType==NULL)
		return -1;	
	
	init(vXmlString);
	
	if (vMsgBodyType->DataOrder!=NULL){
		init(vTmpBuf);
		vFindFlag=0;
		pDataItemListType=vMsgBodyType->DataOrder->PrimaryKeyList;
		while(pDataItemListType!=NULL){
			sprintf(vTmpBuf,"%s\t\t\t\t<DataItem>\n\t\t\t\t\t<DataName>%s</DataName>\n\t\t\t\t\t<DataFormat>%d</DataFormat>\n\t\t\t\t\t<DataValue>%s</DataValue>\n\t\t\t\t</DataItem>\n",
				vTmpBuf,
				pDataItemListType->DataItem->DataName,
				pDataItemListType->DataItem->DataFormat,
				pDataItemListType->DataItem->DataValue);
			pDataItemListType=pDataItemListType->next;	
			vFindFlag=1;
		}

		if (vFindFlag)
			sprintf(vXmlString,"%s\t\t\t<PrimaryKeyList>\n%s\t\t\t</PrimaryKeyList>\n",vXmlString,vTmpBuf);
			
		init(vTmpBuf);
		vFindFlag=0;
		pDataItemListType=vMsgBodyType->DataOrder->DataItemList;
		while(pDataItemListType!=NULL){
			sprintf(vTmpBuf,"%s\t\t\t\t<DataItem>\n\t\t\t\t\t<DataName>%s</DataName>\n\t\t\t\t\t<DataFormat>%d</DataFormat>\n\t\t\t\t\t<DataValue>%s</DataValue>\n\t\t\t\t</DataItem>\n",
				vTmpBuf,
				pDataItemListType->DataItem->DataName,
				pDataItemListType->DataItem->DataFormat,
				pDataItemListType->DataItem->DataValue);
			pDataItemListType=pDataItemListType->next;	
			vFindFlag=1;
		}

		if (vFindFlag)
			sprintf(vXmlString,"%s\t\t\t<DataItemList>\n%s\t\t\t</DataItemList>\n",vXmlString,vTmpBuf);

		strcpy(vTmpBuf,vXmlString);
		sprintf(vXmlString,"\t\t<DataOrder>\n\t\t\t<TableName>%s</TableName>\n%s\t\t</DataOrder>\n",
			vMsgBodyType->DataOrder->TableName,vTmpBuf);
	}
	else if (vMsgBodyType->BusiOrder!=NULL){
		init(vTmpBuf);
		vFindFlag=0;
		pParameterListType=vMsgBodyType->BusiOrder->ParameterList;
		while(pParameterListType!=NULL){
			sprintf(vTmpBuf,"%s\t\t\t\t<ParameterItem>\n\t\t\t\t\t<ParameterName>%s</ParameterName>\n\t\t\t\t\t<ParameterValue>%s</ParameterValue>\n\t\t\t\t</ParameterItem>\n",
				vTmpBuf,
				pParameterListType->ParameterItem->ParameterName,
				pParameterListType->ParameterItem->ParameterValue);
			pParameterListType=pParameterListType->next;	
			vFindFlag=1;
		}

		if (vFindFlag)
			sprintf(vXmlString,"%s\t\t\t<ParameterList>\n%s\t\t\t</ParameterList>\n",vXmlString,vTmpBuf);

		strcpy(vTmpBuf,vXmlString);
		sprintf(vXmlString,"\t\t<BusiOrder>\n\t\t\t<CallServiceName>%s</CallServiceName>\n\t\t\t<SendParaNum>%d</SendParaNum>\n\t\t\t<RecpParaNum>%d</RecpParaNum>\n%s\t\t</BusiOrder>\n",
			vMsgBodyType->BusiOrder->CallServiceName,
			vMsgBodyType->BusiOrder->SendParaNum,
			vMsgBodyType->BusiOrder->RecpParaNum,
			vTmpBuf);
		
	}
	else
		return -2;
	
	strcpy(vTmpBuf,vXmlString);
	init(vXmlString);
	sprintf(vXmlString,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n<OrderMsg>\n\t<MsgBody>\n%s\t</MsgBody>\n</OrderMsg>",vTmpBuf);
	
	vXmlStringLength=strlen(vXmlString);
	vXmlStringLength=vXmlStringLength+vXmlStringLength/(DLINTERFACEDATA-1);

	if (vXmlStringLength>DLINTERFACEARRAY*DLINTERFACEDATA)
		return -3;/*数据过大，目前程序暂不支持*/

	return 0;
}

int OrderSpilitArray(vXmlString,vXmlArray)
char *vXmlString;
char vXmlArray[][DLINTERFACEARRAY][DLINTERFACEDATA];

{
	int vArrayNum,i,vXmlStringLength;

	vXmlStringLength=strlen(vXmlString);
	vXmlStringLength=vXmlStringLength+vXmlStringLength/(DLINTERFACEDATA-1);
	if (vXmlStringLength>DLINTERFACEARRAY*DLINTERFACEDATA)
		return 0;

	init(vXmlArray);
	for (i=0;i<DLINTERFACEARRAY;i++){
		if (vXmlStringLength<=(i+1)*DLINTERFACEDATA){
			memcpy(vXmlArray[0][i],vXmlString+i*DLINTERFACEDATA-i,vXmlStringLength-i*DLINTERFACEDATA);
			break;
		}
		else{
			memcpy(vXmlArray[0][i],vXmlString+i*DLINTERFACEDATA-i,DLINTERFACEDATA-1);
			vXmlArray[0][i][DLINTERFACEDATA-1]='\0';
		}
		
	}
	
	vArrayNum=i+1;
	
	return vArrayNum;
}

int OrderGetXmlData(pXmlString,vLabelName,vXmlData)
char **pXmlString;
char *vLabelName;
char *vXmlData;
{
	char *p,*q;
	char vLabelBegin[DLLABELNAME],vLabelEnd[DLLABELNAME];

	init(vLabelBegin);	init(vLabelEnd);	init(vXmlData);
	
	sprintf(vLabelBegin,"<%s>",vLabelName);
	sprintf(vLabelEnd,"</%s>",vLabelName);

	p=strstr(*pXmlString,vLabelBegin);
	q=strstr(*pXmlString,vLabelEnd);
	
	if (p==NULL || q==NULL)
		return 1;
	
	p=p+strlen(vLabelBegin);
	
	if (p>q)
		return -1;
	
	*pXmlString=q+strlen(vLabelEnd);

	memcpy(vXmlData,p,q-p);
	vXmlData[q-p]='\0';
	
	return 0;
}

int OrderParseXml(vMsgBodyType,vXmlString,vOrderType)
MsgBodyType	*vMsgBodyType;
char *vXmlString;
char *vOrderType;
{
	char *pXmlString,*pXmlTmp;
	char vXmlData[DLINTERFACEARRAY*DLINTERFACEDATA],vXmlTmp[DLINTERFACEARRAY*DLINTERFACEDATA];
	int  v_ret,v_first_flag;
	DataOrderType *vDataOrder;
	BusiOrderType *vBusiOrder;
	DataItemType *vDataItemType;
	DataItemListType *pDataItemListType,*vDataItemListType;
	ParameterItemType *vParameterItemType;
	ParameterListType	*pParameterListType,*vParameterListType;
	
	init(vXmlData);		init(vXmlTmp);

	pXmlString=vXmlString;

	if (strcmp(vOrderType,ORDERTYPEDATA)==0){
		vDataOrder=(DataOrderType *)malloc(DATAORDERTYPESIZE);
		memset(vDataOrder,0,DATAORDERTYPESIZE);	
		vMsgBodyType->DataOrder=vDataOrder;
		
		v_ret=OrderGetXmlData(&pXmlString,LABELTABLENAME,vXmlData);
		if (v_ret<0)
			return -1;
		strcpy(vMsgBodyType->DataOrder->TableName,vXmlData);
		
		v_ret=OrderGetXmlData(&pXmlString,LABELPRIMARYKEYLIST,vXmlData);
		if (v_ret<0)
			return -1;
			
		if (v_ret==0){
			strcpy(vXmlTmp,vXmlData);
			pXmlTmp=vXmlTmp;
			v_first_flag=1;

			while(OrderGetXmlData(&pXmlTmp,LABELDATANAME,vXmlData)==0){

				vDataItemListType=(DataItemListType *)malloc(DATAITEMLISTTYPESIZE);
				memset(vDataItemListType,0,DATAITEMLISTTYPESIZE);
				vDataItemType=(DataItemType *)malloc(DATAITEMTYPESIZE);
				memset(vDataItemType,0,DATAITEMTYPESIZE);
				vDataItemListType->DataItem=vDataItemType;
				
				if (v_first_flag){
					pDataItemListType=vDataItemListType;
					vMsgBodyType->DataOrder->PrimaryKeyList=vDataItemListType;
					v_first_flag=0;
				}
				else{
					pDataItemListType->next=vDataItemListType;
					pDataItemListType=vDataItemListType;
				}
				
				strcpy(vDataItemListType->DataItem->DataName,vXmlData);
				
				v_ret=OrderGetXmlData(&pXmlTmp,LABELDATAFORMAT,vXmlData);
				if (v_ret<0)
					return -1;
				vDataItemListType->DataItem->DataFormat=atoi(vXmlData);
	
				v_ret=OrderGetXmlData(&pXmlTmp,LABELDATAVALUE,vXmlData);
				if (v_ret<0)
					return -1;
				strcpy(vDataItemListType->DataItem->DataValue,vXmlData);	
			}

		}

		v_ret=OrderGetXmlData(&pXmlString,LABELDATAITEMLIST,vXmlData);
		if (v_ret<0)
			return -1;
			
		if (v_ret==0){
			strcpy(vXmlTmp,vXmlData);
			pXmlTmp=vXmlTmp;
			v_first_flag=1;

			while(OrderGetXmlData(&pXmlTmp,LABELDATANAME,vXmlData)==0){

				vDataItemListType=(DataItemListType *)malloc(DATAITEMLISTTYPESIZE);
				memset(vDataItemListType,0,DATAITEMLISTTYPESIZE);
				vDataItemType=(DataItemType *)malloc(DATAITEMTYPESIZE);
				memset(vDataItemType,0,DATAITEMTYPESIZE);
				vDataItemListType->DataItem=vDataItemType;
				
				if (v_first_flag){
					pDataItemListType=vDataItemListType;
					vMsgBodyType->DataOrder->DataItemList=vDataItemListType;
					v_first_flag=0;
				}
				else{
					pDataItemListType->next=vDataItemListType;
					pDataItemListType=vDataItemListType;
				}
				
				strcpy(vDataItemListType->DataItem->DataName,vXmlData);
				
				v_ret=OrderGetXmlData(&pXmlTmp,LABELDATAFORMAT,vXmlData);
				if (v_ret<0)
					return -1;
				vDataItemListType->DataItem->DataFormat=atoi(vXmlData);
	
				v_ret=OrderGetXmlData(&pXmlTmp,LABELDATAVALUE,vXmlData);
				if (v_ret<0)
					return -1;
				strcpy(vDataItemListType->DataItem->DataValue,vXmlData);	
			}

		}	
	}
	else if (strcmp(vOrderType,ORDERTYPEBUSI)==0){
		vBusiOrder=(BusiOrderType *)malloc(BUSIORDERTYPESIZE);
		memset(vBusiOrder,0,BUSIORDERTYPESIZE);	
		vMsgBodyType->BusiOrder=vBusiOrder;
		
		v_ret=OrderGetXmlData(&pXmlString,LABELSERVICENAME,vXmlData);
		if (v_ret<0)
			return -1;
		strcpy(vMsgBodyType->BusiOrder->CallServiceName,vXmlData);

		v_ret=OrderGetXmlData(&pXmlString,LABELSENDPARANUM,vXmlData);
		if (v_ret<0)
			return -1;
		vMsgBodyType->BusiOrder->SendParaNum=atoi(vXmlData);
			
		v_ret=OrderGetXmlData(&pXmlString,LABELRECPPARANUM,vXmlData);
		if (v_ret<0)
			return -1;
		vMsgBodyType->BusiOrder->RecpParaNum=atoi(vXmlData);
		
		v_first_flag=1;

		while(OrderGetXmlData(&pXmlString,LABELPARAMETERNAME,vXmlData)==0){
			vParameterListType=(ParameterListType *)malloc(PARAMETERLISTTYPESIZE);
			memset(vParameterListType,0,PARAMETERLISTTYPESIZE);
			vParameterItemType=(ParameterItemType *)malloc(PARAMETERITEMTYPESIZE);
			memset(vParameterItemType,0,PARAMETERITEMTYPESIZE);

			strcpy(vParameterItemType->ParameterName,vXmlData);
			v_ret=OrderGetXmlData(&pXmlString,LABELPARAMETERVALUE,vXmlData);
			if (v_ret<0)
				return -1;
			strcpy(vParameterItemType->ParameterValue,vXmlData);

			vParameterListType->ParameterItem=vParameterItemType;	
			
			if (v_first_flag){
				pParameterListType=vParameterListType;
				vMsgBodyType->BusiOrder->ParameterList=vParameterListType;
				v_first_flag=0;
			}
			else{
				pParameterListType->next=vParameterListType;
				pParameterListType=vParameterListType;
			}
		}	
	}
	else
		return -2;

	return 0;	

}

void OrderInitStructOrder(v_order_data)
OrderTable *v_order_data;
{
	v_order_data->order_accept=0;
	init(v_order_data->order_type);
	init(v_order_data->id_type);
	init(v_order_data->id_no);
	init(v_order_data->interface_type);
	init(v_order_data->busi_type);
	init(v_order_data->busi_code);
	v_order_data->order_right=0;
	init(v_order_data->op_type);
	init(v_order_data->op_code);
	v_order_data->op_accept=0;
	init(v_order_data->op_login);
	v_order_data->total_date=0;
	init(v_order_data->op_time);
	init(v_order_data->op_note);
	init(v_order_data->data_format);
	init(v_order_data->det_flag);
	init(v_order_data->interface_data);
	init(v_order_data->commuinfo_flag);
	v_order_data->send_num=0;
	init(v_order_data->send_status);
	init(v_order_data->send_time);
	init(v_order_data->resp_code);
	init(v_order_data->resp_time);
}

int OrderInserSend(v_table_owner,v_order_data,v_order_string)
char *v_table_owner;
OrderTable *v_order_data;
char *v_order_string;
{
	EXEC SQL BEGIN DECLARE SECTION;
		long v_order_accept;
		int i;
		OrderDetTable v_orderdet_data[DLINTERFACEARRAY];
		
		char v_tmp_buf[DLDBSQL];
		char v_data_type[DLCOMMTYPE];
		char v_data_msg[DLINTERFACEDATA];
		
		char v_order_type[DLCOMMTYPE];
		char v_id_type[DLCOMMTYPE];
		char v_id_no[31];
		char v_interface_type[3];
		char v_busi_type[21];
		char v_busi_code[31];
		int	 v_order_right;
		char v_op_type[DLCOMMTYPE];
		char v_op_code[7];
		long v_op_accept;
		char v_op_login[7];
		char v_op_note[101];
		char v_data_format[DLCOMMTYPE];
		char v_det_flag[DLCOMMTYPE];
		char v_interface_data[DLINTERFACEDATA];
		char v_commuinfo_flag[DLCOMMTYPE];
    EXEC SQL END DECLARE SECTION;
    char v_xml_array[DLINTERFACEARRAY][DLINTERFACEDATA];
	int v_array_num;

	init(v_xml_array);
	v_array_num=OrderSpilitArray(v_order_string,v_xml_array);
	
	if (v_array_num==1){
		strcpy(v_order_data->det_flag,ORDERDETFLAG_N);
		strcpy(v_order_data->interface_data,v_xml_array[0]);
	}
	else{
		strcpy(v_order_data->det_flag,ORDERDETFLAG_Y);
		for (i=0;i<v_array_num;i++){
			strcpy(v_orderdet_data[i].data_type,"0");	
			strcpy(v_orderdet_data[i].data_msg,v_xml_array[i]);
		}
	}	
	
	init(v_tmp_buf);
	sprintf(v_tmp_buf,"SELECT %s.seq_ordersend_accept.nextval FROM dual",v_table_owner);
	EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :v_tmp_buf INTO :v_order_accept;
		END;
	END-EXEC;
	if (SQLCODE!=SQLOK)
		return -1;	

	strcpy(v_order_type    ,v_order_data->order_type    );
	strcpy(v_id_type       ,v_order_data->id_type       );
	strcpy(v_id_no         ,v_order_data->id_no         );
	strcpy(v_interface_type,v_order_data->interface_type);
	strcpy(v_busi_type     ,v_order_data->busi_type     );
	strcpy(v_busi_code     ,v_order_data->busi_code     );
	v_order_right=          v_order_data->order_right    ;
	strcpy(v_op_type       ,v_order_data->op_type       );
	strcpy(v_op_code       ,v_order_data->op_code       );
	v_op_accept=            v_order_data->op_accept      ;
	strcpy(v_op_login      ,v_order_data->op_login      );
	strcpy(v_op_note       ,v_order_data->op_note       );
	strcpy(v_data_format   ,v_order_data->data_format   );
	strcpy(v_det_flag      ,v_order_data->det_flag      );
	strcpy(v_interface_data,v_order_data->interface_data);
	strcpy(v_commuinfo_flag,v_order_data->commuinfo_flag);
			
	init(v_tmp_buf);
	sprintf(v_tmp_buf,"INSERT INTO %s.wOrderSend(order_accept,order_type,id_type,id_no,interface_type,\
					busi_type,busi_code,partition_column,order_right,op_type,op_code,\
					op_accept,op_login,total_date,op_time,op_note,\
					data_format,det_flag,interface_data,commuinfo_flag,send_num,\
					send_status,send_time,resp_code,resp_time) \
			VALUES(:v1,:v2,:v3,:v4,:v5,\
					:v6,:v7,mod(to_number(to_char(sysdate,'YYYYMMDD')),10),:v8,:v9,:v10,\
					:v11,:v12,to_number(to_char(sysdate,'YYYYMMDD')),sysdate,:v13,\
					:v14,:v15,:v16,:v17,0,'0',null,null,null)",v_table_owner);

	EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :v_tmp_buf USING :v_order_accept,:v_order_type,:v_id_type,:v_id_no,:v_interface_type,
				:v_busi_type,:v_busi_code,:v_order_right,:v_op_type,:v_op_code,
				:v_op_accept,:v_op_login,:v_op_note,
				:v_data_format,:v_det_flag,:v_interface_data,:v_commuinfo_flag;
		END;
	END-EXEC;				
	if (SQLCODE!=SQLOK){
		return -2;
	}

	if (v_array_num>1){
		for (i=0;i<v_array_num;i++){
			strcpy(v_data_type,v_orderdet_data[i].data_type);
			strcpy(v_data_msg,v_orderdet_data[i].data_msg);
			
			init(v_tmp_buf);
			sprintf(v_tmp_buf,"INSERT INTO %s.wOrderSendDataDet(order_accept,data_type,order_no,data_msg) \
					VALUES(:v1,:v2,:v3,:v4)",v_table_owner);
			EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :v_tmp_buf USING :v_order_accept,:v_data_type,:i,:v_data_msg;
				END;
			END-EXEC;
			if (SQLCODE!=SQLOK)
				return -3;			
		}
	}

	return 0;
}

int OrderInserSendCrm(v_order_data,v_order_string)
OrderTable *v_order_data;
char *v_order_string;
{
	int v_ret;
	v_ret=OrderInserSend(ORDEROWNER_CRM,v_order_data,v_order_string);
	
	return v_ret;
}

int OrderInserSendBOSS(v_order_data,v_order_string)
OrderTable *v_order_data;
char *v_order_string;
{
	int v_ret;
	v_ret=OrderInserSend(ORDEROWNER_BOSS,v_order_data,v_order_string);
	
	return v_ret;
}

void OrderMultiParaFree(){
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

/*
返回值:
	0:正常
	1:NOTFOUND
	负值:错误
	
*/
int OrderMultiParaDML(vSqlBuf,vParameterArray)
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
		OrderMultiParaFree();
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

/*
		if ( (strncmp(bind_dp->V[i], "NULL", 4) == 0) || (strncmp(bind_dp->V[i], "null", 4) == 0)) {
			* bind_dp->I[i] = -1;
		}
		else {
			* bind_dp->I[i] = 0;
		}
*/
		* bind_dp->I[i] = 0;
		
		bind_dp->T[i] = 96;
	}
	EXEC SQL OPEN CC USING DESCRIPTOR bind_dp;
	if(SQLCODE!=SQLOK && SQLCODE!=NOTFOUND){
		EXEC SQL CLOSE CC;
		OrderMultiParaFree();
		return -4;
	}

	if (SQLCODE==NOTFOUND){
		EXEC SQL CLOSE CC;
		OrderMultiParaFree();
		return 1;
	}
		
	EXEC SQL CLOSE CC;
	if(SQLCODE!=SQLOK){
		OrderMultiParaFree();
		return -5;
	}
	
	OrderMultiParaFree();
	return 0;
}
