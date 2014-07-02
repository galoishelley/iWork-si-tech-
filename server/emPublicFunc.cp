/**Created: zhengxg
 **Date:    2010.07.05*/
#include "publicEmer.h"


/*hash�㷨��begin*/
/*************************************************************
 * name:     HashKey                                         *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ��һ��hash�㷨�����ò����ۼӻ��ƣ�              *
 * ��ν��times33�㷨�������ִ�Сд                           *
 * ����hash�㷨��ͬУ��һ���ַ���                            *
 * input:    vStr            ��Ҫhash���ַ���                *
 * return��  nHash           hash���ֵ                      *
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
 * function: �ڶ���hash�㷨������У���ۼӷ�����Ҳ�����ִ�Сд*
 * ����hash�㷨��ͬУ��һ���ַ���                            *
 * input:    vStr            ��Ҫhash���ַ���                *
 * return��  nHash           hash���ֵ                      *
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
 * function: �õ��ַ����ڶ������е�λ��                      *
 * input:    vStr            ��Ҫhash���ַ���                *
 *           emTable         ������ṹ                      *
 * return��  nHashPos        �ַ����ڱ��е�λ��              *
 *************************************************************/
long GetHashTablePos(const char *vStr, struct emshm *emTable)
{ 
	unsigned long nHashA = 0;
	unsigned long nHashB = 0;
	unsigned long lTableCnt = 0;
	unsigned long nHashStart = 0;
	unsigned long nHashPos = 0;

	lTableCnt = emTable->sPubDBCfgCnt;

	/*�����ڴ�δ��ʼ��*/
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
		/*�Ѿ���ѯ��һ��*/
		if(nHashPos == nHashStart)
		{
			break;
		}
	}
	return -1; /*û���ҵ�*/
}

/*ͨ�������ַ���������Ӧ�ı���ɢ�е���������Ӧλ����ȥ*/
/*************************************************************
 * name:     SetHashTable                                    *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ͨ�������ַ���������Ӧ�ı���ɢ�е���            *
 *           ������Ӧλ����ȥ                                *
 * input:    vStr            ��Ҫhash���ַ���                *
 *           emTable         ������ṹ                      *
 *           sDbCfg          ��Ҫɢ�е����еĽṹ            *
 * return��  retValue        �Ƿ�ִ�гɹ���0�ɹ���-1���ɹ�   *
 *************************************************************/
int SetHashTable(const char *vStr, struct emshm *emTable, sPublicDBCfg *sDbCfg)
{	
	unsigned long nHashA = 0;
	unsigned long nHashB = 0;
	unsigned long lTableCnt = 0;
	unsigned long nHashStart = 0;
	unsigned long nHashPos = 0;

	lTableCnt = emTable->sPubDBCfgCnt;

	/*�����ڴ�δ��ʼ��*/
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
/*hash�㷨��end*/

/*�����ڴ���begin*/
/*************************************************************
 * name:     sPubGetCfgFile                                  *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ��ȡ�����ļ�����·��                            *
 * input:    file            �ļ�������ʽΪ��/config.cfg     *
 *           rtn_path        ��Ҫ���ص��ļ�ȫ·��            *
 * return��  retValue        �Ƿ�ִ�гɹ���0�ɹ���-1���ɹ�   *
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
 * function: ȡ�ù����ڴ��ֵ                                *
 * input:    NULL                                            *
 * return��  kid        ��ȡ��ϵͳ�����ID                   *
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
 * function: ���������ڴ棬����NULL��ʾ����ʧ��              *
 * input:    NULL                                            *
 * return��  emshmTmp        �����ڴ�����Ľṹ��          *
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
	/*���乲���ڴ�*/
	if((shmId = shmget(shmKey, sizeof(struct emshm),IPC_CREAT|0666)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*��ȡ�����ڴ��е�����*/
	if((emshmTmp = (struct emshm *)shmat(shmId, (char *) 0, 0)) == (struct emshm *) -1)
	{
#ifdef _DEBUG_
		printf("Error: get emshmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		/*��ȡʧ�ܣ���ɾ�������Ĺ����ڴ�*/
		shmctl(shmId, IPC_RMID, 0);
		return NULL;
	}
	return emshmTmp;
}

/*************************************************************
 * name:     sPubGetShm                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ��ȡ�����ڴ������ݣ�ȡ��������NULL              *
 * input:    NULL                                            *
 * return��  emshmTmp        ��ȡ�������ڴ��еĽṹ��        *
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
	/*��ȡ�����ڴ�ID*/
	if((shmId = shmget(shmKey, sizeof(struct emshm), 0)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmId variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return NULL;
	}
	/*��ȡ�����ڴ��е�����*/
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
 * function: �Ͽ����ݽṹ���ڴ������                        *
 * input:    shmt            �����ڴ��еĽṹ��              *
 * return��  retValue        �ɹ�0��ʧ��-1                   *
 *************************************************************/
int sPubDisconShm(struct emshm *shmt)
{
	if(shmdt((char *)shmt) < 0)
	{
		printf("�Ͽ����ݽṹ���ڴ����Ӵ��󣬴�������[%s],�����У�[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/*************************************************************
 * name:     sPubDelShm                                      *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ɾ�������ڴ�                                    *
 * input:    NULL                                            *
 * return��  retValue        �ɹ�0��ʧ��-1                   *
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
	/*���乲���ڴ�*/
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
/*�����ڴ���end*/
/*�ź���ʹ����begin*/
/*************************************************************
 * name:     sPubGetSemKey                                   *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ȡ���ź�����ֵ                                  *
 * input:    NULL                                            *
 * return��  kid        ��ȡ��ϵͳ�����ID                   *
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
 * function: ������Ϣ����                                    *
 * input:    NULL                                            *
 * return��  queueId    ��ȡ��ϵͳ�ж��е�ID��               *
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
 * function: ȡ����Ϣ����                                    *
 * input:    NULL                                            *
 * return��  queueId    ��ȡ��ϵͳ�ж��е�ID��               *
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
 * function: ������Ϣ��������                                *
 * input:    iQueId     ϵͳ�ж��е�ID��                     *
 *           sMsg       ��Ҫ���͵�ϵͳ��Ϣ                   *
 * return��  iRetValue  �Ƿ�ִ�гɹ���0�ɹ���-1ʧ��          *
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
 * function: ���ն����е���Ϣ                                *
 * input:    iQueId     ϵͳ�ж��е�ID��                     *
 *           sMsg       ���ص�ϵͳ��Ϣ                       *
 * return��  iRetValue  �Ƿ�ִ�гɹ���0�ɹ���-1ʧ��          *
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
 * function: ɾ����Ϣ����                                    *
 * input:    iQueId     ϵͳ�ж��е�ID��                     *
 * return��  iRetValue  �Ƿ�ִ�гɹ���0�ɹ���-1ʧ��          *
 *************************************************************/
int sPubQuitQue(const int iQueId)
{
	return msgctl(iQueId, IPC_RMID, NULL);
}
/*�ź���ʹ����end*/
/*��������ʹ����begin*/
/*************************************************************
 * name:     getDbCfg                                        *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ��ȡ�����Ӧ�ڹ����ڴ��еĽṹ��                *
 * input:    serviceName     ������                          *
 *           dbCfg           ���صĽṹ��                    *
 * return��  retValue        �ɹ�0��ʧ��-1                   *
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
	
	/*��ȡ�����ڴ�������*/
	if((shmTmp = sPubGetShm()) == NULL)
	{
#ifdef _DEBUG_
		printf("Error: read shmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	
	memset(&shm, 0, sizeof(struct emshm));
	memcpy(&shm, shmTmp, sizeof(struct emshm));
	/*�Ͽ��빲���ڴ�����*/
	if((sPubDisconShm(shmTmp)) < 0)
	{
#ifdef _DEBUG_
		printf("Error: get shmTmp variable@[%s],line:[%d]\n", __FUNCTION__, __LINE__);
#endif
		return -1;
	}
	/*��ȡ�������ڹ����ڴ��е�λ��*/
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
 * function: �鿴�����Ƿ�֧��Ӧ��                            *
 * input:    serviceName     ������                          *
 *           oDbCfg          ���ص����ݿ�ṹ��              *
 *           oRetMsg         У��������                    *
 *           oRetMsg         У�鷵����Ϣ                    *
 * return��  retValue        �ɹ�0��ʧ��-1                   *
 *************************************************************/
int fEmerCheck(const char *serviceName, sPublicDBCfg *oDbCfg, char *oRetCode, char *oRetMsg)
{
	sPublicDBCfg dbCfg;
	/*��ȡ�����Ӧ�ڹ����ڴ��еĽṹ��*/
	if(0 != getDbCfg(serviceName, &dbCfg))
	{
		printf("Error: get service dbCfg variable@[%s]\n", __FUNCTION__);
		return 0;
	}
	/*�ж�ϵͳ�Ƿ���Ӧ��״̬��Ӧ����־�Ƿ���Ч*/
	if(dbCfg.emsupportFlag[0] == 'N' 
		&& dbCfg.emFlag[0] == 'Y')
	{
		strcpy(oRetCode, "999998");
		sprintf(oRetMsg, "Ӧ��ϵͳ�����ڼ䣬����[%s]�ݲ�֧��Ӧ����", serviceName);
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
 * function: ��ȡ���ݿ��ǩ                                  *
 * input:    serviceName     ������                          *
 *           dbLabel         ���ص����ݿ��ǩ                *
 * return��  retValue        �ɹ�0��ʧ��-1                   *
 *************************************************************/
int getEmerDbLabel(const char *serviceName, char *dbLabel)
{
	sPublicDBCfg dbCfg;
	/*��ȡ�����Ӧ�ڹ����ڴ��еĽṹ��*/
	if(0 != getDbCfg(serviceName, &dbCfg))
	{
		printf("��ȡ�����ڴ��ж�Ӧ������󣬴�����[%s],������[%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	/*�ж�Ӧ���Ƿ�����*/
	if(dbCfg.emFlag[0] != 'Y')
	{
		/*printf("�÷���Ӧ����δ���ã�������[%s],������[%d]\n", __FUNCTION__, __LINE__);*/
		return -1;
	}
	memset(dbLabel, 0, sizeof(dbLabel));
	strcpy(dbLabel, dbCfg.dbLabel);
	return 0;
}
/*��������ʹ����end*/
/*����������begin*/
/*************************************************************
 * name:     doPrint                                         *
 * writer:   zhengxg                                         *
 * date:     2010.07.05                                      *
 * function: ��ӡ�����ڴ��е�����                            *
 * input:    sPubDbCfg       ��Ҫ��ӡ�Ľṹ��                *
 * return��  NULL                                            *
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
 * function: ��ȡ�����е������Ϣ���������ݿ��������        *
 * input:    cfgLine         ��������Ϣ                      *
 *           machineCode     ��������                        *
 *           machineIP       ����IP                          *
 *           Db              ���ݿ���                        *
 *           userName        ���ݿ��û���                    *
 *           passWd          ���ܺ�����ݿ�����              *
 *           refreshSpanTmp  ����ˢ��Ƶ��                    *
 * return��  vRetValue       ����ֵ���ɹ�0                   *
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
	/*�������ݿ�����*/
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

/*����������end*/
