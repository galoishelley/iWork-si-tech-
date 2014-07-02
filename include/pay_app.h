#ifndef _PAY_APP_H
#define _PAY_APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/********
#include <sys/shm.h>
********/

#define         CARD_LEN	13
#define         PASSWD_LEN	9
#define         PHONE_LEN	11
#define         FILENAMELEN     28
#define         SEMKEY1         ((key_t) 23456L)
#define         PATHNAMELEN     50
#define         CONSTANTSTR     "0000000000000000"
#define         MAX_TEXT_LEN    129

#define         SDEBUG

typedef struct _c_payfee {
	char 	card_no[CARD_LEN+1];
	char	card_passwd[PASSWD_LEN+1];
	char 	phone_no[PHONE_LEN+1];
} C_PAYFEE; /* from local_city to center */

typedef struct _s_payfee {
	char ret[3];
} S_PAYFEE; /* from center to local_city */

#endif
