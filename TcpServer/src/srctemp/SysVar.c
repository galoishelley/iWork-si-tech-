/**********************************************************************\
 *
 * File: SysVar.cpp
 *
 *  NOTE!!! 使用VI打开此文件时, 请将 tablespace设置为4 (:set ts=4)
 *
\**********************************************************************/

char	gCfgFileServ[256];	// 服务配置文件绝对路径名
char	gCfgFileWork[256];	// 业务配置文件绝对路径名
char	gServID[8];			// 服务ID
int		gLogFlag = 1;		// PubFunc.cpp 中公共函数 DBOSSLog 会使用此标志来打印日志, 1:打印 0:不打印
char	gLogFile[256];		// 日志文件的绝对路径名

char	gIP[32];			// 监听服务进程监听的IP
int		gPort=0;			// 监听服务进程监听的端口
char	gIpcKeyPath[256];	// 主控、同步、异步队列以及共享内存、信号量使用的键值路径
char	gChannelID[16];		// 交易发起的渠道标识
int		gTimeOut=0;			// select函数超时时间
int		gPollCount=0;		// 读取或发送SOCKET数据的轮询次数（socket通信采用非阻塞模式）
int		gMaxProcNum=0;		// 最大同步进程个数

int		gQuit=0;			// 控制程序完整性标志，保证程序正常退出

int		gMainQueue = 0;		// 主控队列 标识ID
int		gSynQueue = 0;		// 同步队列 标识ID
int		gASynQueue = 0;		// 异步队列 标识ID

int		gSynResSHM = 0;		// 同步进程资源共享内存 标识 ID
int		gSynCtlSHM = 0;		// 同步进程控制共享内存 标识 ID
int		gSynSEM = 0;		// 同步进程信号量 标识 ID
