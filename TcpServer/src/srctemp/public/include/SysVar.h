#ifndef __SYSVAR_H__
#define __SYSVAR_H__

// ����ע��ϵͳֻ�����ﶨ����ȫ�ֱ�������
// ��������ģ����Ա���һ��

// Ӧ��ʹ�õĺ궨��
#define _FL_ __FILE__, __LINE__

// ���̻������ļ��궨��
#define LOCK_OK 0
#define LOCK_ERR_EXIST 1
#define LOCK_ERR_FAIL -1
#define LCKHEAD "SynServ"

// SOCKET�������д�С
#define BACKLOG 50

// IPC RELATED ��ֵID
#define MAINID 111 		//���ض��еļ�ֵ ID
#define SYNID 112		//ͬ�����еļ�ֵ ID
#define ASYNID 113		//�첽���еļ�ֵ ID

#define SYNRESSHM 114	//ͬ��������Դ�����ڴ�ļ�ֵ ID
#define SYNCTLSHM 115	//ͬ�����̿��ƹ����ڴ�ļ�ֵ ID
#define SYNSEMSET 116	//ͬ�����̿����ź������ļ�ֵ ID
#define SYNSEMNUM 2		//ͬ�����̿����ź����ĸ���
#define SYNSEMAVAL 1	//ͬ�����̿����źſ��Ի��
#define SYNSEMUNAVAL 0	//ͬ�����̿����źŲ��ɻ��

// ���������첽��־
#define ASYNFLAG "ASYN=1"	//�첽��־

// ͬ�����̿����ź�����ʼ��ʹ�õĽṹ����
union semun {
	int				val;
	struct semid_ds	*buf;
	ushort			*array;
};

// ���շ��ͻ�������С
#define MAXBUFSIZE 64*1024

// ͬ������ʹ�õĹ�����Ϣʶ�������Ͷ���
typedef struct _SHM_MTYPE {
	int mtype;
} SHM_MTYPE;


/******************Add By Zhaodw******************/
//���湤���ļ��Ľṹ��
typedef struct _CfgFileStruct
{
	int   nLineCount;
	char  cBuffer[256][1024];
} CFGFILESTRUCT;
/************************************************/


// ȫ�ֱ�������
// ������ע��ϵͳֻ�� SysVar.cpp �ﶨ��ȫ�ֱ���
// ��������ģ����Ա���һ��
extern char		gServID[8];
extern char		gCfgFileServ[256];
extern char		gCfgFileWork[256];
extern int		gLogFlag;
extern char		gLogFile[256];

extern char		gIP[32];
extern int		gPort;
extern char		gIpcKeyPath[256];
extern char		gChannelID[16];
extern int		gTimeOut;
extern int		gPollCount;
extern int		gMaxProcNum;

extern int		gQuit;

extern int		gMainQueue;
extern int		gSynQueue;
extern int		gASynQueue;

extern int		gSynResSHM;
extern int		gSynCtlSHM;
extern int		gSynSEM;

#endif // __SYSVAR_H__


