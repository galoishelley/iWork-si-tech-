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

#define 	FILENAMELEN 	28

struct test1 {
	char nr[20];
	};

struct test0 {
	char xy[4];
	};
struct test2ask{
        char text[35];
        };
struct test2ack{              
        char retcode[5];
        char fieldsname[120];
        char fieldslen[54];
        char fieldsvalue[182];
        }; 
struct test3ack{
        char retcode[5];
        char fieldsname[28];
        char fieldslen[12];
        char fieldsvalue[44];
        }; 
struct test4ask{
        char text[70];
        };
struct test4ack{
        char retcode[5];
        };
        
