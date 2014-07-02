#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
/* for sco5.0.5 */
#include <arpa/inet.h>
#include <setjmp.h>
#include <termio.h>

#include "commagent.h"

#define MSGLEN	256
#define LOG printf

#define   BILL_PAY		1
#define   BILL_QUERY   		0
#define   MSG_BEG		9600
#define   PERMS		0666

typedef struct msgbuf1
{
        long mtype;
        char mtext[256];
}MSGBUF;
