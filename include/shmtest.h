#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>

#define SDEBUG 
#define FILENAMELEN 	28
#define SEMKEY1 	((key_t) 39458L)
#define PATHNAMELEN     50 
#define CONSTANTSTR     "0000000000000000"
#define MAX_TEXT_LEN    129
#define SRV_DADDR_CODE       "10000000F"
#define TRADE_UNBILL_TEST      "999999"
#define TRADE_UNBILL_ZECX      "100000"
#define TRADE_UNBILL_XXCX      "100001"
#define TRADE_UNBILL_PLCX      "999999"

struct test1 {
	char nr[20];
	};

struct test0 {
	char xy[4];
	};

struct ShmTestAsk {
	char phone_no[15];
	};
struct ShmTestAck {
	char ret_code[4];
	char should_pay[10];
};
