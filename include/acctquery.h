#ifndef _ACCTQUERY_H_
#define _ACCTQUERY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <varargs.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/socket.h>
#include <math.h>



typedef struct _CUSTOWE_NODE
{
	char Phone_No[16];
	int  Year_Month;
	char Status_Flag; /*排序标识,'Z'欠费,'A'非欠费*/
	char Status_Name[20];
	char Bill_Begin[21];
	char Bill_End[21]; 
	double Owe_Fee;
	double Delay_Fee;
	double Should_Pay;
	double Favour_Fee;
	double Payed_Prepay;
	double Payed_Later;
}OutCustOweType; 
#define OUTCUSTOWE_SIZE sizeof(OutCustOweType)

typedef struct _CUSTOWEDET_NODE
{
	char Phone_No[16];
	int  Year_Month;
	char Status_Flag; /*排序标识,'Z'欠费,'A'非欠费*/
	char Status_Name[20];
	char Fee_Code[6];
	char Fee_Name[64];
	char Detail_Code[8];
	char Detail_Name[64];
	double Owe_Fee;
	double Should_Pay;
	double Favour_Fee;
	double Payed_Prepay;
	double Payed_Later;
}OutCustOweDetType; 
#define OUTCUSTOWEDET_SIZE sizeof(OutCustOweDetType)      

#define ISSPACE(x) ((x)==' '||(x)=='\r'||(x)=='\n'||(x)=='\f'||(x)=='\b'||(x)=='\t')

int Query_Det_Bill(char *IPhoneNo,long IIdNo,long IContractNo,char *BillBegin,char *BillEnd,
		 int QueryType,OutCustOweDetType **vDetOweArr,int *iDetOweNum);
		 
int Query_Tot_Bill(char *IPhoneNo,long IIdNo,long IContractNo,char *IBeginYM,char *IEndYM,
					OutCustOweType **vTotOweArr,int *iTotOweNum);
int Query_Con_Pre( long IContractNo, char *IPayType, double *OConPreFee);

int Query_Con_Owe(long IContractNo,double *OConOweFee);

void Print_Det_Bill(OutCustOweDetType **vDetOweArr,int *iDetOweNum,int QueryType);

void Print_Tot_Bill(OutCustOweType **vTotOweArr,int *iTotOweNum);

int Connect_DB(char * dbpass);
int Close_DB();
					
#endif
					
