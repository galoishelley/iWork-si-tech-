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

#include "srv_field.h"
#define         SDEBUG 
/**#define		TESTPRINT **/
#define 	FILENAMELEN 	28
#define 	SEMKEY1 	((key_t) 39457L)
#define         PATHNAMELEN     50 
#define         CONSTANTSTR     "0000000000000000"
#define         MAX_TEXT_LEN    129

#define         PRIMARY_KEY     "movebank"
char MAC_key[17];
char MAC_code[17];
char exchange_key[9];
char cipher_key[9];
char ming_key[9];
char ming_text[MAX_TEXT_LEN];
char cipher_text[MAX_TEXT_LEN];


struct test1 {
	char nr[18];
	char bh;
	char bh1;
	struct test0 *sp;
	};

struct test0 {
	char xy[4];
	struct test0 *next;
	};

struct Payask {
	char op_code[SCHAR4];
	char phone_no[SCHAR11];
	char agt_code[SCHAR8];
	};
struct Payack {
	char ret_code[SCHAR4];
	char contract_no[SCHAR15];
	char region_code[SCHAR2];
	char district_code[SCHAR2];
	char owner_unit[SCHAR59];
	char owner_name[SCHAR12];
        char pay_code[SCHAR1];
        char run_code[SCHAR1];
	char current_owe[SMONEY12];
	char limit_owe[SMONEY12];
        char prepay_fee[SMONEY12];
	char predel_fee[SMONEY12];	
	char a_machines[SINT4];
	char d_machines[SINT4];
        struct Shouldpaylist *shouldpaylist;
};
