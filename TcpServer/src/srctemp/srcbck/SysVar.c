/**********************************************************************\
 *
 * File: SysVar.cpp
 *
 *  NOTE!!! ʹ��VI�򿪴��ļ�ʱ, �뽫 tablespace����Ϊ4 (:set ts=4)
 *
\**********************************************************************/

char	gCfgFileServ[256];	// ���������ļ�����·����
char	gCfgFileWork[256];	// ҵ�������ļ�����·����
char	gServID[8];			// ����ID
int		gLogFlag = 1;		// PubFunc.cpp �й������� DBOSSLog ��ʹ�ô˱�־����ӡ��־, 1:��ӡ 0:����ӡ
char	gLogFile[256];		// ��־�ļ��ľ���·����

char	gIP[32];			// ����������̼�����IP
int		gPort=0;			// ����������̼����Ķ˿�
char	gIpcKeyPath[256];	// ���ء�ͬ�����첽�����Լ������ڴ桢�ź���ʹ�õļ�ֵ·��
char	gChannelID[16];		// ���׷����������ʶ
int		gTimeOut=0;			// select������ʱʱ��
int		gPollCount=0;		// ��ȡ����SOCKET���ݵ���ѯ������socketͨ�Ų��÷�����ģʽ��
int		gMaxProcNum=0;		// ���ͬ�����̸���

int		gQuit=0;			// ���Ƴ��������Ա�־����֤���������˳�

int		gMainQueue = 0;		// ���ض��� ��ʶID
int		gSynQueue = 0;		// ͬ������ ��ʶID
int		gASynQueue = 0;		// �첽���� ��ʶID

int		gSynResSHM = 0;		// ͬ��������Դ�����ڴ� ��ʶ ID
int		gSynCtlSHM = 0;		// ͬ�����̿��ƹ����ڴ� ��ʶ ID
int		gSynSEM = 0;		// ͬ�������ź��� ��ʶ ID
