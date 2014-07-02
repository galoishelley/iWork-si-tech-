#ifndef __SYSVAR_H__
#define __SYSVAR_H__

// ！！注意系统只在这里定义宏和全局变量声明
// 这样各个模块可以保持一致

// 应用使用的宏定义
#define _FL_ __FILE__, __LINE__

// 进程互斥锁文件宏定义
#define LOCK_OK 0
#define LOCK_ERR_EXIST 1
#define LOCK_ERR_FAIL -1
#define LCKHEAD "SynServ"

// SOCKET监听队列大小
#define BACKLOG 50

// IPC RELATED 键值ID
#define MAINID 111 		//主控队列的键值 ID
#define SYNID 112		//同步队列的键值 ID
#define ASYNID 113		//异步队列的键值 ID

#define SYNRESSHM 114	//同步进程资源共享内存的键值 ID
#define SYNCTLSHM 115	//同步进程控制共享内存的键值 ID
#define SYNSEMSET 116	//同步进程控制信号量集的键值 ID
#define SYNSEMNUM 2		//同步进程控制信号量的个数
#define SYNSEMAVAL 1	//同步进程控制信号可以获得
#define SYNSEMUNAVAL 0	//同步进程控制信号不可获得

// 工单请求异步标志
#define ASYNFLAG "ASYN=1"	//异步标志

// 同步进程控制信号量初始化使用的结构定义
union semun {
	int				val;
	struct semid_ds	*buf;
	ushort			*array;
};

// 接收发送缓冲区大小
#define MAXBUFSIZE 64*1024

// 同步进程使用的共享消息识别码类型定义
typedef struct _SHM_MTYPE {
	int mtype;
} SHM_MTYPE;


/******************Add By Zhaodw******************/
//保存工单文件的结构体
typedef struct _CfgFileStruct
{
	int   nLineCount;
	char  cBuffer[256][1024];
} CFGFILESTRUCT;
/************************************************/


// 全局变量声明
// ！！！注意系统只在 SysVar.cpp 里定义全局变量
// 这样各个模块可以保持一致
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


