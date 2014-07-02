#include "boss_srv.h"
#include "publicsrv.h"

#define	MAXSERVERNUMBER 3000
#define CHECKTIME 30

typedef struct _CfgServerStruct
{
	char 	Server[MAXSERVERNUMBER][128];
	int     FindAcc[MAXSERVERNUMBER];
	int   	Max[MAXSERVERNUMBER];
	int 	Current[MAXSERVERNUMBER];
	char    Time[20];
	int     Count;
} CFGSERVERSTRUCT;

typedef struct _CopyServerStruct
{
	CFGSERVERSTRUCT CopyStruct[2];
} COPYSERVERSTRUCT;


int GetShm(key_t Key,int* ShmId)
{
	int ret = 0;

	ret = shmget(Key,0,0);
	if(ret == -1)
	{
		printf("GetShm error[%s]",strerror(errno));
		return -1;
	}

	*ShmId = ret;
	return 0;
}
int GetSem(key_t Key,int* SemId)
{
	int ret = 0;

	ret = semget(Key,0,0);
	if(ret == -1)
	{
		printf("GetSem error[%s]",strerror(errno));
		return -1;
	}

	*SemId = ret;
	return 1;
}

int hash_name(const char *servername)
{
	int index=0;
	long sum=0;
	int i=0;
	int p=0;
	if(strlen(servername)>8)
		p=8;
	else 
	{
		p=strlen(servername);
	}
	
	for(i=1;i<p;i++)
	{	
		if(servername[i]<'0'||servername[i]>'9')
		{
			sum=sum+servername[i];
		}
		else
		{
			sum=sum*10+(servername[i]-'0');
		}
	}
	return sum%(MAXSERVERNUMBER-1);
}
/***********************
�������ƣ�PubServerCheck
�������ܣ��жϷ���ĵȴ����и����Ƿ񳬹������ļ���������
	     ������ĵȴ�������Ϣ��¼�ڹ����ڴ��У�
��дʱ�䣺2010.04.10

����
	server_name 		������
	cfgkeypath  		�����ļ�·���������ļ���ȡ�����ڵ�ţ�   
	return_message 		���ش�����Ϣ
����ֵ
	0 ����
	-1 �����ѹ����������
************************/
int PubServerCheck(char *server_name,char *cfgfilepath, char *return_message)
{
	
	int						vMax=0,	vCurrent=0;
	int 					flag=0;
	int 					i=0;
	int                     index;
	int                     limit=0;
	time_t 					timep;
	key_t  					IPCkey = 0;
	COPYSERVERSTRUCT*		pCfgShareBuffer = NULL ;
	int 					cfgShmid = 0;
	int   					cfgSemid = 0;
	char 					cfgKeyPath[256];
	char                    vWorkDir[100];
	char                    vServName[100];
	
	clock_t 				start, finish; 
	double 					duration;
	
	long                    ii=100000;
	
	memset(cfgKeyPath,0,sizeof(cfgKeyPath));
	memset(vWorkDir,0,sizeof(vWorkDir));
	memset(vServName,0,sizeof(vServName));
	
	Trim(server_name);
	Trim(cfgfilepath);
	sprintf(cfgKeyPath,"%s",cfgfilepath);
#ifdef _LOG_
	printf("cfgKeyPath=[%s]\n",cfgKeyPath);
#endif
	IPCkey = ftok(cfgKeyPath,545);
	if(GetShm(IPCkey,&cfgShmid) == -1)
	{
		strcpy(return_message,"ȡ���Ѵ��ڹ����ڴ�IDʧ��");
		return 0;
	}
#ifdef _LOG_
	printf("cfgShmid=[%d]\n",cfgShmid);
#endif
	if(GetSem(IPCkey,&cfgSemid) == -1)
	{
		strcpy(return_message,"ȡ���Ѵ����ź���IDʧ��");
		return 0;
	}
#ifdef _LOG_
	printf("cfgSemid=[%d]\n",cfgSemid);
#endif
	pCfgShareBuffer = (COPYSERVERSTRUCT*)shmat(cfgShmid,0,0);
	if(pCfgShareBuffer == NULL)
	{
		strcpy(return_message,"���ӹ����ڴ�cfgShmidʧ��");
		shmctl(cfgShmid,IPC_RMID,0);
		return 0;
	}
	
	start = clock(); 
	index=hash_name(server_name);
    limit=0;
    while(pCfgShareBuffer->CopyStruct[1].Server[index][0]!='\0'&&
          strcmp(pCfgShareBuffer->CopyStruct[1].Server[index],server_name)!=0)
    {	
    	index=(index+1)%(MAXSERVERNUMBER-1);
    	limit++;
    	if(limit>MAXSERVERNUMBER)
    	{
    		break;
    	}
    } 
    if(strcmp(pCfgShareBuffer->CopyStruct[1].Server[index],server_name)==0)
    {	
    	vCurrent=pCfgShareBuffer->CopyStruct[1].Current[index];
    	vMax=pCfgShareBuffer->CopyStruct[1].Max[index];
    }
	else
	{
		strcpy(return_message,"û�����÷�����Ϣ");
		shmdt(pCfgShareBuffer);
		return 0;
	}

	finish = clock(); 
	duration = (double)(finish - start)/CLOCKS_PER_SEC; 
#ifdef _LOG_
	printf( "��ѯʱ��[%f seconds]\n", duration ); 
	
	printf("�����Ŷ�[%d]\n",vMax);
	printf("��ǰ�Ŷ�[%d]\n",vCurrent);
#endif
	time(&timep);
	printf("timep=[%ld],(pCfgShareBuffer->Time)=[%ld],[%ld]\n",timep,atol(pCfgShareBuffer->CopyStruct[1].Time),timep-atol(pCfgShareBuffer->CopyStruct[1].Time));
	if(vMax < vCurrent && timep-atol(pCfgShareBuffer->CopyStruct[1].Time)< CHECKTIME )
	{	
		strcpy(return_message,"ϵͳ��æ,���Ժ����ԣ�");
		shmdt(pCfgShareBuffer);
		return 1;
	}
	shmdt(pCfgShareBuffer);
	return 0;
}
