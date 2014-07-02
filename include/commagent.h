#define     COMM_PERMS           0660
#define     SA              struct sockaddr
#define     MAXCONN         1024

#define     CMDSUCCESS      0

#define     MSGBEGIN        0x00000001
#define     MSGCONTINUE     0x00000010
#define     MSGEND          0x00000100
#define     MSGCHECK        0x00000110

#define     SYSNULL         0
#define     SYSFREE         1
#define     SYSBUSY         2
#define     SYSWAIT         3

#define     ISTRUE          '1'
#define     NOTTRUE         '0'

#define     BREAKHEART      '0'
#define     SRVREGISTER     '1'
#define     MSGREQUEST      '3'
#define     MSGRESPONSE     '4'
#define     MSGGETNEXT      '5'
#define		MSGREADY		'6'

#define     maxnum(a,b)  (a>b) ? a : b

#define     MAXDLEN     8000
#define     HEADLEN      64
#define     MSGFLAG      "`DC`"
#define     VER01        "01"

struct CommCtrl
{
    char    msgflag[4];         /* ��ʼ��־��"`DC`" */
    char    version[2];         /* �汾�� "01" */
    char    finished;           /* �Ƿ������һ�� */
    char    msgtype;            /* ��Ϣ���� */
    char    msglength[4];       /* ��Ϣ���ȣ�����4λ���㣬���8000 */
    char    appid[6];           /* �������� */
    char    transid[10];        /* �����־��Ψһ����ţ�����10λ����*/
    char    transnum[6];        /* �����ڷ�����ţ�����������,����6λ����*/
    char    codetime[6];        /* ������,��λ��(��Ӧ���ʶ),����6λ����*/
    char    clientid[8];        /* ��������ͻ���ʶ */
    char    userid[8];          /* ͨ�Ŵ���ͻ���ʶ */
    char    streamid[8];        /* ͨ����������� */
    char    msgbody[MAXDLEN];   /* ��Ϣ�� */
    int     sockid;             /* ������Ϣ�ͻ���ʶ */
    long    time;               /* ������Ϣʱ�� */
    struct CommCtrl  *next;     /* �����ڵ�ָ�� */
};

struct appregister
{
    int                 appid;      /* �������� */
    int                 sockid;     /* ҵ����ϵͳ�׽��� */
    int                 streamid;   /* ��������� */
    int                 status;     /* ҵ����ϵͳ״̬���С�æ���ȴ� */
    int                 transid;    /* ���´���������ʶ */
    long                time;       /* ҵ��ϵͳ��ʱ���� */
    long                last;       /* ҵ��ϵͳ����ʹ��ʱ�� */
};
#define    APPREGSIZE       sizeof(struct appregister)

struct sysvar
{
    int                 streamid;
    int                 serverid;
    int                 listenid;
    int                 connectid;
    int                 clilen;
    int                 msgid;
    int                 fdret,maxfd1;
    int                 service;
    int                 ip_port;
    int                 max_connect;
    int                 msg_key;
    int                 hb_time;
    int                 net_timeout;
    int                 appid;
    long                time;
    long                servertime;
    char                server_ip[16];
    int                 server_port;
    fd_set              rset;
    struct timeval      tval;
    struct sockaddr_in  srv_addr,cli_addr,serverconn;
};
