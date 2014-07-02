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
    char    msgflag[4];         /* 起始标志，"`DC`" */
    char    version[2];         /* 版本号 "01" */
    char    finished;           /* 是否是最后一包 */
    char    msgtype;            /* 消息类型 */
    char    msglength[4];       /* 消息长度，不足4位左补零，最大8000 */
    char    appid[6];           /* 事务类型 */
    char    transid[10];        /* 事务标志，唯一的序号，不足10位左补零*/
    char    transnum[6];        /* 事务内分组序号（必须连续）,不足6位左补零*/
    char    codetime[6];        /* 生存期,单位秒(或应答标识),不足6位左补零*/
    char    clientid[8];        /* 任务请求客户标识 */
    char    userid[8];          /* 通信代理客户标识 */
    char    streamid[8];        /* 通信描述符序号 */
    char    msgbody[MAXDLEN];   /* 消息体 */
    int     sockid;             /* 发送消息客户标识 */
    long    time;               /* 接收消息时间 */
    struct CommCtrl  *next;     /* 后续节点指针 */
};

struct appregister
{
    int                 appid;      /* 事务类型 */
    int                 sockid;     /* 业务处理系统套接字 */
    int                 streamid;   /* 描述符序号 */
    int                 status;     /* 业务处理系统状态：闲、忙、等待 */
    int                 transid;    /* 最新处理的事务标识 */
    long                time;       /* 业务系统超时限制 */
    long                last;       /* 业务系统最新使用时间 */
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
