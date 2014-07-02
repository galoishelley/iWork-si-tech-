#ifndef _COMMSG_H
#define _COMMSG_H

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
/* for sco5.0.5 */
#include <arpa/inet.h>

#define 	DEBUG  
#define 	PERMS 		0660
#define 	MSGINDIR   	"HanInMsg"
#define 	MSGINKEY   	4
#define 	MSGOUTDIR	"HanOutMsg"
#define 	MSGOUTKEY	2
#define 	MSGOUT1DIR	"HanOut1Msg"
#define 	MSGOUT1KEY      3	
#define 	STEPTIME 	5
#define         WAITTIME        15 
#define 	TIMEOUT 	-999

#define		MAXDATASIZ	216
#define 	MAX_LINK 	3
#define 	CODESIZ		6
#define 	ORDERSIZ  	9
#define 	FNAMELEN 	28
#define 	CODEFIELD 	8
#define 	STRUFIELD 	7
#define		MAXBUF   	500
#define		MAX_DATA	256

#define 	SPRI 		'+'
#define 	SSPRI 		"+"

#define 	STRUREQ 	'1'
#define 	STRUACK 	'2'
#define 	FILEREQ 	'3'
#define 	FILEACK 	'4'

#define 	SNDERR		-1
#define 	RCVERR		-2

#define 	SET 		'1'
#define 	NOSET 		'0'

struct CtlMsg {
		unsigned char  morepkt;
		unsigned char  pktype;
		unsigned char  unitend;
		unsigned char  save;
		unsigned short sequence;
		unsigned short length;
		unsigned long  msgtype;
	};


#define 	CTLMSGLEN	sizeof(struct CtlMsg)

struct header {
		char code[CODESIZ];
		char d_add[ORDERSIZ];
		char l_add[ORDERSIZ];
	};

#define 	HEADLEN		sizeof(struct header)

struct pktstru {
		struct 	CtlMsg	ctlmsg;
		struct  header  header;
		char    datatrans[MAXDATASIZ];
	};

#define 	PKTSTRULEN 		sizeof(struct pktstru)

struct PktTrans {
		struct  pktstru pktstru;
	};


#define 	PKTLEN 		sizeof(struct PktTrans)

struct packet {
		char data[PKTLEN];
		struct packet *next;
	};

#define PACKETSIZ 	sizeof(struct packet)

struct msg_buf {
		unsigned long msgtyp;
		char   mdata[PKTLEN];
	};

#define MSGBUFLEN 	sizeof(struct msg_buf)
/*********************************
struct msg_buf {
		unsigned long msgtyp;
		char   mdata[100];
	};

#define MSGBUFLEN 	sizeof(struct msg_buf)
 ******************************************/     

struct node {
       		char data[MSGBUFLEN];
		struct node *next;
	};

#define NODELEN  sizeof(struct node)


struct struexp {
	int total;
	int realen;
	int l_num;
	struct {
		int size;
		int count;
	} exp[MAX_LINK];
};

#define STRUEXPLEN 	sizeof(struct struexp )
#endif  /* _COMMSG_H */
