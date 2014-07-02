/**Created: zhengxg
 **Date:    2010.07.05*/
#include "publicEmer.h"


/*hash算法域begin*/
/*************************************************************
 * name:     HashKey                                         *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 第一种hash算法，采用不断累加机制，              *
 * 所谓的times33算法，不区分大小写                           *
 * 两种hash算法共同校验一个字符串                            *
 * input:    vStr            需要hash的字符串                *
 * return：  nHash           hash后的值                      *
 *************************************************************/
unsigned long HashKey(const char *vStr)
{
	unsigned long nHash = 0;
	while (*vStr)
	{
		nHash += (nHash<<5) + toupper(*vStr++);
	}
	return nHash;
}

/*************************************************************
 * name:     ELFhash                                         *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 第二种hash算法，采用校验累加方法，也不区分大小写*
 * 两种hash算法共同校验一个字符串                            *
 * input:    vStr            需要hash的字符串                *
 * return：  nHash           hash后的值                      *
 *************************************************************/
unsigned long ELFhash(const char *vStr)
{ 
	unsigned long nHash=0;
	while(*vStr)
	{
		nHash = (nHash << 4) + toupper(*vStr ++);
		unsigned long g = nHash & 0Xf0000000L;
		if(g) 
			nHash ^= g >> 24;
		nHash &= ~g;
	}
	return nHash;
}

/*************************************************************
 * name:     GetHashTablePos                                 *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 得到字符串在定长表中的位置                      *
 * input:    vStr            需要hash的字符串                *
 *           emTable         定长表结构                      *
 * return：  nHashPos        字符串在表中的位置              *
 *************************************************************/
long GetHashTablePos(const char *vStr, struct emshm *emTable)
{ 
	unsigned long nHashA = 0;
	unsigned long nHashB = 0;
	unsigned long lTableCnt = 0;
	unsigned long nHashStart = 0;
	unsigned long nHashPos = 0;

	lTableCnt = emTable->sPubDBCfgCnt;

	/*共享内存未初始化*/
	if(lTableCnt <= 0 )
	{
#ifdef _DEBUG_
		printf("Error: Service table count is [%ld] <= 0 @[%s],line:[%d]\n", lTableCnt, __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	
	nHashA = HashKey(vStr);
	nHashB = ELFhash(vStr);

	/*nHashStart = nHashA % lTableCnt;*/
	nHashStart = nHashA % MAXDBCFG;
	nHashPos = nHashStart;

	while(emTable->sPublicDBCfgs[nHashPos].bExists)
	{
		if(emTable->sPublicDBCfgs[nHashPos].lHashA == nHashA && emTable->sPublicDBCfgs[nHashPos].lHashB == nHashB) 
		{
			return nHashPos;
		}
		else 
		{
			nHashPos = (nHashPos + 1) % lTableCnt;
		}
		/*已经查询了一遍*/
		if(nHashPos == nHashStart)
		{
			break;
		}
	}
	return -1; /*没有找到*/
}

/*通过传入字符串，将相应的表项散列到索引表相应位置中去*/
/*************************************************************
 * name:     SetHashTable                                    *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 通过传入字符串，将相应的表项散列到索            *
 *           引表相应位置中去                                *
 * input:    vStr            需要hash的字符串                *
 *           emTable         定长表结构                      *
 *           sDbCfg          需要散列到表中的结构            *
 * return：  retValue        是否执行成功，0成功，-1不成功   *
 *************************************************************/
int SetHashTable(const char *vStr, struct emshm *emTable, sPublicDBCfg *sDbCfg)
{	
	unsigned long nHashA = 0;
	unsigned long nHashB = 0;
	unsigned long lTableCnt = 0;
	unsigned long nHashStart = 0;
	unsigned long nHashPos = 0;

	lTableCnt = emTable->sPubDBCfgCnt;

	/*共享内存未初始化*/
	if(lTableCnt <= 0 )
	{
#ifdef _DEBUG_
		printf("Error: Service table count is [%ld] <= 0 @[%s],line:[%d]\n", lTableCnt, __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	
	nHashA = HashKey(vStr);
	nHashB = ELFhash(vStr);

	/*nHashStart = nHashA % lTableCnt;*/
	nHashStart = nHashA % MAXDBCFG;
	nHashPos = nHashStart;
	
	while(emTable->sPublicDBCfgs[nHashPos].bExists)
	{ 
		nHashPos = (nHashPos + 1) % lTableCnt;
		if(nHashPos == nHashStart) 
		{
			return -1; 
		}
	}
	memcpy(&(emTable->sPublicDBCfgs[nHashPos]), sDbCfg, sizeof(sPublicDBCfg));
	emTable->sPublicDBCfgs[nHashPos].bExists = 1;
	emTable->sPublicDBCfgs[nHashPos].lHashA = nHashA;
	emTable->sPublicDBCfgs[nHashPos].lHashB = nHashB;
	return 0;
}
/*hash算法域end*/

/*共享内存域begin*/
/*************************************************************
 * name:     sPubGetCfgFile                                  *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 获取配置文件所在路径                            *
 * input:    file            文件名，形式为：/config.cfg     *
 *           rtn_path        需要返回的文件全路径            *
 * return：  retValue        是否执行成功，0成功，-1不成功   *
 *************************************************************/
int sPubGetCfgFile(const char *file, char *rtn_path)
{
	char * envp;
	if((envp = getenv(EMDIR)) == NULL)
	{
		printf("Error: get EMDIR env variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	memset(rtn_path, 0, sizeof(rtn_path));
	strcpy(rtn_path, envp);
	strcat(rtn_path, "/cfg");
	strcat(rtn_path, file);
	return 0;	
}

/*************************************************************
 * name:     sPubGetShmKey                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 取得共享内存键值                                *
 * input:    NULL                                            *
 * return：  kid        获取到系统分配的ID                   *
 *************************************************************/
key_t sPubGetShmKey()
{
	char fileName[200 + 1];
	key_t kid;
	sPubGetCfgFile(EMCFGFILE, fileName);
#ifdef _DEBUG_
	printf("local path[%s]\n", fileName);
#endif
	kid = ftok(fileName, EMKEY);
	return kid;
}

/*************************************************************
 * name:     sPubCreateShm                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 创建共享内存，返回NULL表示创建失败              *
 * input:    NULL                                            *
 * return：  emshmTmp        创建内存后分配的结构体          *
 *************************************************************/
struct emshm *sPubCreateShm()
{
	int shmId = 0;
	key_t shmKey;
	struct emshm *emshmTmp;
	
	if((shmKey = sPubGetShmKey()) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmKey variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*分配共享内存*/
	if((shmId = shmget(shmKey, sizeof(struct emshm),IPC_CREAT|0666)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*获取共享内存中的内容*/
	if((emshmTmp = (struct emshm *)shmat(shmId, (char *) 0, 0)) == (struct emshm *) -1)
	{
#ifdef _DEBUG_
		printf("Error: get emshmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		/*获取失败，则删除创建的共享内存*/
		shmctl(shmId, IPC_RMID, 0);
		return NULL;
	}
	return emshmTmp;
}

/*************************************************************
 * name:     sPubGetShm                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 获取共享内存中内容，取不到返回NULL              *
 * input:    NULL                                            *
 * return：  emshmTmp        获取到共享内存中的结构体        *
 *************************************************************/
struct emshm *sPubGetShm()
{
	int shmId = 0;
	key_t shmKey;
	struct emshm *emshmTmp;
	if((shmKey = sPubGetShmKey()) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmKey variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*获取共享内存ID*/
	if((shmId = shmget(shmKey, sizeof(struct emshm), 0)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*获取共享内存中的内容*/
	if((emshmTmp = (struct emshm *)shmat(shmId, (char *) 0, 0)) == (struct emshm *) -1)
	{
#ifdef _DEBUG_
		printf("Error: get emshmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	return emshmTmp;
}

/*************************************************************
 * name:     sPubDisconShm                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 断开数据结构和内存的连接                        *
 * input:    shmt            共享内存中的结构体              *
 * return：  retValue        成功0，失败-1                   *
 *************************************************************/
int sPubDisconShm(struct emshm *shmt)
{
	if(shmdt((char *)shmt) < 0)
	{
		printf("断开数据结构与内存连接错误，错误函数：[%s],错误行：[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/*************************************************************
 * name:     sPubDelShm                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 删除共享内存                                    *
 * input:    NULL                                            *
 * return：  retValue        成功0，失败-1                   *
 *************************************************************/
int sPubDelShm()
{
	int shmId = 0;
	key_t shmKey;
	int ret_value = 0;
	if((shmKey = sPubGetShmKey()) < 0)
	{
		printf("Error: get shmKey variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	/*分配共享内存*/
	if((shmId = shmget(shmKey, sizeof(struct emshm),IPC_CREAT|0666)) < 0)
	{
		printf("Error: get shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(shmctl(shmId, IPC_RMID, 0) < 0)
	{
		printf("Error: delete shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}
/*共享内存域end*/
/*信号量使用域begin*/
/*************************************************************
 * name:     sPubGetSemKey                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 取得信号量键值                                  *
 * input:    NULL                                            *
 * return：  kid        获取到系统分配的ID                   *
 *************************************************************/
key_t sPubGetSemKey()
{
	key_t kid;
	char fileName[200 + 1];
	memset(fileName, 0, sizeof(fileName));
	char * envp;
	if((envp = getenv(EMDIR)) == NULL)
	{
		printf("Error: get EMDIR env variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	strcpy(fileName, envp);
	strcat(fileName, "/bin");
	
	kid = ftok(fileName, QUEKEY);
	return kid;
}

/*************************************************************
 * name:     sPubCreateSem                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 创建消息队列                                    *
 * input:    NULL                                            *
 * return：  queueId    获取到系统中队列的ID号               *
 *************************************************************/
int sPubCreateSem()
{
	key_t key;
	int iQueId = 0;
	if((key = sPubGetSemKey()) < 0)
	{
		printf("Error: get key variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if((iQueId = msgget(key, IPC_CREAT)) < 0)
	{
		return msgget(key, IPC_CREAT|0660);
	}
	msgctl(iQueId, IPC_RMID, NULL);
	return msgget(key, IPC_CREAT|0660);
	
}
/*************************************************************
 * name:     sPubGetSemQueId                                 *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 取得消息队列                                    *
 * input:    NULL                                            *
 * return：  queueId    获取到系统中队列的ID号               *
 *************************************************************/
int sPubGetSemQueId()
{
	key_t key;
	if((key = sPubGetSemKey()) < 0)
	{
		printf("Error: get key variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return(msgget(key, IPC_CREAT));
}
/*************************************************************
 * name:     sPubSndMsg                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 发送消息到队列中                                *
 * input:    iQueId     系统中队列的ID号                     *
 *           sMsg       需要发送的系统消息                   *
 * return：  iRetValue  是否执行成功，0成功，-1失败          *
 *************************************************************/
int sPubSndMsg(const int iQueId, const char *sMsg)
{
	int length = 0;
	struct sPubQueueMsg sSndMsg;
	memset(&sSndMsg, 0, sizeof(struct sPubQueueMsg));
	sSndMsg.msgType = MSGTYPE;
	strncpy(sSndMsg.quitFlag, sMsg, 1);
	length = sizeof(struct sPubQueueMsg) - sizeof(long);
	return msgsnd(iQueId, &sSndMsg, length, 0);
}
/*************************************************************
 * name:     sPubRcvMsg                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 接收队列中的消息                                *
 * input:    iQueId     系统中队列的ID号                     *
 *           sMsg       返回的系统消息                       *
 * return：  iRetValue  是否执行成功，0成功，-1失败          *
 *************************************************************/
int sPubRcvMsg(const int iQueId, char *sMsg)
{
	int length = 0;
	struct sPubQueueMsg sRcvMsg;
	memset(&sRcvMsg, 0, sizeof(struct sPubQueueMsg));
	length = sizeof(struct sPubQueueMsg) - sizeof(long);
	memset(sMsg, 0, sizeof(sMsg));
	if(msgrcv(iQueId, &sRcvMsg, length, MSGTYPE, IPC_NOWAIT) < 0)
	{
		return -1;
	}
	strcpy(sMsg, sRcvMsg.quitFlag);
	return 0;
}
/*************************************************************
 * name:     sPubQuitQue                                     *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 删除消息队列                                    *
 * input:    iQueId     系统中队列的ID号                     *
 * return：  iRetValue  是否执行成功，0成功，-1失败          *
 *************************************************************/
int sPubQuitQue(const int iQueId)
{
	return msgctl(iQueId, IPC_RMID, NULL);
}
/*信号量使用域end*/
/*公共函数使用域begin*/
/*************************************************************
 * name:     getDbCfg                                        *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 获取服务对应在共享内存中的结构体                *
 * input:    serviceName     服务名                          *
 *           dbCfg           返回的结构体                    *
 * return：  retValue        成功0，失败-1                   *
 *************************************************************/
int getDbCfg(const char *serviceName, sPublicDBCfg *dbCfg)
{
	long lPos = 0;
	struct emshm shm;
	struct emshm *shmTmp;
	
	if(serviceName == NULL || dbCfg == NULL || (strlen(serviceName) == 0))
	{
#ifdef _DEBUG_
		printf("Error: don't need do anything@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	
	/*读取共享内存中内容*/
	if((shmTmp = sPubGetShm()) == NULL)
	{
#ifdef _DEBUG_
		printf("Error: read shmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	
	memset(&shm, 0, sizeof(struct emshm));
	memcpy(&shm, shmTmp, sizeof(struct emshm));
	/*断开与共享内存连接*/
	if((sPubDisconShm(shmTmp)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	/*获取服务名在共享内存中的位置*/
	lPos = GetHashTablePos(serviceName, &shm);
	if(lPos < 0)
	{
#ifdef _DEBUG_
		printf("Error: get lPos variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	memset(dbCfg, 0, sizeof(sPublicDBCfg));
	memcpy(dbCfg, &(shm.sPublicDBCfgs[lPos]), sizeof(sPublicDBCfg));
	return 0;
}
/*************************************************************
 * name:     fEmerCheck                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 查看服务是否支持应急                            *
 * input:    serviceName     服务名                          *
 *           oDbCfg          返回的数据库结构体              *
 *           oRetMsg         校验错误代码                    *
 *           oRetMsg         校验返回信息                    *
 * return：  retValue        成功0，失败-1                   *
 *************************************************************/
int fEmerCheck(const char *serviceName, sPublicDBCfg *oDbCfg, char *oRetCode, char *oRetMsg)
{
	sPublicDBCfg dbCfg;
	/*获取服务对应在共享内存中的结构体*/
	if(0 != getDbCfg(serviceName, &dbCfg))
	{
		printf("Error: get service dbCfg variable@[%s]\n", __FUNCTION__);
		return 0;
	}
	/*判断系统是否在应急状态，应急标志是否生效*/
	if(dbCfg.emsupportFlag[0] == 'N' 
		&& dbCfg.emFlag[0] == 'Y')
	{
		strcpy(oRetCode, "999998");
		sprintf(oRetMsg, "应急系统启用期间，服务[%s]暂不支持应急！", serviceName);
		return -1;
	}
	memset(oDbCfg, 0, sizeof(sPublicDBCfg));
	memcpy(oDbCfg, &dbCfg, sizeof(sPublicDBCfg));
	return 0;
}
/*************************************************************
 * name:     getEmerDbLabel                                  *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 获取数据库标签                                  *
 * input:    serviceName     服务名                          *
 *           dbLabel         返回的数据库标签                *
 * return：  retValue        成功0，失败-1                   *
 *************************************************************/
int getEmerDbLabel(const char *serviceName, char *dbLabel)
{
	sPublicDBCfg dbCfg;
	/*获取服务对应在共享内存中的结构体*/
	if(0 != getDbCfg(serviceName, &dbCfg))
	{
		printf("获取共享内存中对应服务错误，错误函数[%s],错误行[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	/*判断应急是否启用*/
	if(dbCfg.emFlag[0] != 'Y')
	{
		/*printf("该服务应急暂未启用，错误函数[%s],错误行[%d]\n", __FUNCTION__, __LINE__);*/
		return -1;
	}
	memset(dbLabel, 0, sizeof(dbLabel));
	strcpy(dbLabel, dbCfg.dbLabel);
	return 0;
}
/*公共函数使用域end*/
/*其他公共域begin*/
/*************************************************************
 * name:     doPrint                                         *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 打印共享内存中的内容                            *
 * input:    sPubDbCfg       需要打印的结构体                *
 * return：  NULL                                            *
 *************************************************************/
void doPrint(sPublicDBCfg *sPubDbCfg)
{
	printf("[%-20s][%-4s][%-10s][%-1s][%-1s][%-1s][%-17s][%-4s][%-4s][%-60s]\n", 
	sPubDbCfg->serviceName,
	sPubDbCfg->opCode,
	sPubDbCfg->dbLabel,
	sPubDbCfg->emFlag,
	sPubDbCfg->orderCreateFlag,
	sPubDbCfg->emsupportFlag,
	sPubDbCfg->opTime,
	sPubDbCfg->relaOpCode,
	sPubDbCfg->machineCode,
	sPubDbCfg->opNote
	);	
}

/*************************************************************
 * name:     getConfigKey                                    *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: 获取配置行的相关信息，并将数据库密码解码        *
 * input:    cfgLine         配置行信息                      *
 *           machineCode     主机代码                        *
 *           machineIP       主机IP                          *
 *           Db              数据库名                        *
 *           userName        数据库用户名                    *
 *           passWd          解密后的数据库密码              *
 *           refreshSpanTmp  程序刷新频率                    *
 * return：  vRetValue       返回值，成功0                   *
 *************************************************************/
int getConfigKey(const char *cfgLine, char *machineCode, char *machineIP, char *Db, char *userName, char *passWd, char *refreshSpan)
{
	char machineCodeTmp[4 + 1];
	char machineIpTmp[20 + 1];
	char dbTmp[12 + 1];
	char userNameTmp[12 + 1];
	char passwdTmp[32 + 1];
	char passwdDecode[32 + 1];
	char refreshSpanTmp[10 + 1];
	init(machineCodeTmp);
	init(machineIpTmp);
	init(dbTmp);
	init(userNameTmp);
	init(passwdTmp);
	init(passwdDecode);
	init(refreshSpanTmp);
	sscanf(cfgLine, "%s%s%s%s%s%s", machineCodeTmp, machineIpTmp, dbTmp, userNameTmp, passwdTmp, refreshSpanTmp);
#ifdef _DEBUG_
	printf("machineCode=[%s]\nmachineIp=[%s]\ndb=[%s]\nuserName=[%s]\npasswd=[%s]\nrefreshSpanTmp=[%s]\n", machineCodeTmp, machineIpTmp, dbTmp, userNameTmp, passwdTmp, refreshSpanTmp);
#endif
	/*解密数据库密码*/
	spublicEnDePasswd(passwdTmp, passwdDecode, MASTERKEY, DECRYPT);
	init(machineCode);
	init(machineIP);
	init(Db);
	init(userName);
	init(passWd);
	init(refreshSpan);
	strncpy(machineCode, machineCodeTmp, 4);
	strncpy(machineIP, machineIpTmp, 20);
	strncpy(Db, dbTmp, 12);
	strncpy(userName, userNameTmp, 12);
	strncpy(passWd, passwdDecode, 32);
	strncpy(refreshSpan, refreshSpanTmp, 10);
	return 0;
}

/*其他公共域end*/
