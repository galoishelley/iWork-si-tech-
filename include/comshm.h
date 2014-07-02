#ifndef _COMSHM_H
#define _COMSHM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHMTEST		19999
#define SHMKDIR 	"HandleShm"
#define SHMKEY		1
#define SHMKDIR1 	"HandShm1"
#define SHMKEY1		2
#define SHMKDIR2 	"HandShm2"
#define SHMKEY2		3
#define PERMS		0660

#define MAX_BIND 	80
#define COMPORT 	4900 /*export*/
#define COMPORT1 	4901  /*input */
#define MAXADDNO 	80
#define MAXCODE 	50
#define LINE 		80
#define COMMENT 	'#'
#define IPADDLEN 	15
#define JGBMLEN  	9
#define CODELEN 	6
#define PRGNAMELEN 	20

#define TRANSCONF 	"TransConf"
#define LOCAL 		0
#define CENTER 		1
#define FRONT 		2

#define TRANSTIME 	"TransTime"
#define DEFTIME 	50

#define TRANSPRG 	"TransPrg"
#define TRANSCZ 	"TransCz"
#define TRANSSTRU  	"TransStru"
#define TRANSSEMA 	"TransSema"

#define SEMKEY1         ((key_t) 39458L)
#define SEMKEY2         ((key_t) 39468L)
#define SEMKEY3         ((key_t) 39478L)
#define BIGCOUNT        200

struct addshm {
	char          jgbm[JGBMLEN+1];
	unsigned long srvadd;
	u_short	      flag; /* 0 local 1 center 2 front */
	unsigned int  sndport;
	unsigned int  rcvport;
};  


struct trans_time {
	char 		Code[CODELEN+1];
	char 		ECHO;
	unsigned int 	EcTm;
	char 		ACK;
	unsigned int 	AckTm;
	char 		FWD;
	unsigned int 	FwdTm;
};


struct trans_prg {
	char 		tcode[CODELEN+2];
	char 		tpname[PRGNAMELEN+1];
};

struct trans_stru {
	char 		stru[LINE+1];
};

struct trans_cz {
	char 		code[CODELEN+1];
	char 		czcode[CODELEN+1];
	char 		sltime[8];
};
	
struct comshm {
	unsigned short 		addno;
	struct addshm   add[MAXADDNO];
	unsigned int 	ttimeno;
	struct trans_time ttime[MAXCODE];
	unsigned int 	tprgno;
	struct trans_prg  tprg[MAXCODE*2];
	unsigned int 	tstruno;
	struct trans_stru tstru[MAXCODE*2];
	unsigned int 	tczno;
	struct trans_cz   tcz[MAXCODE];
	unsigned int    maxuproc;
};


typedef struct {
	int jgbm;
	int rcvport;
	} BINDPORT;

typedef struct {
	BINDPORT bindport[MAX_BIND];
	} BINDNO;
#endif /* _COMSHM_H */
