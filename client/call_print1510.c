#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include "atmi.h"
#include <time.h>
#include <fml.h>
#include <stdarg.h>
#include <sys/types.h>
#include "resultfld.h"
#include "requfld.h"
#define ALL     0
#define IP		1
#define LINE_NUM 50
#define E_SERVNAME "semobile"	  /**ÃÎÍø**/
#define G_SERVNAME "sgeneral"	  /**ÆÕÍ¨**/
#define GPRS_SERVNAME "sgprs"	  /**GPRS**/
#define S_SERVNAME "sshort_info"  /**¶ÌÐÅ***/
#define SUNION_SERVNAME "sushort_info"/***¶ÌÐÅ»¥Í¨***/
#define SG_SERVNAME "sgoforw"     /****ºô×ª*****/
#define SVPMN "svpmn"     /****vpmn»°µ¥*****/
#define SMMS "smms"        /****²ÊÐÅ»°µ¥**/
#define USER_ID_LEN 		 16
#define START_DATETIME_LEN 	 24

char      login_no[6+1];
char      user_id[15+1];
char      bill_type[2];
char      begin_time[16];
char      end_time[16];
char      login_accept[20];
char      today[21];
int query_general(float * gen_bas_shouldpay,float *gen_toll_shouldpay);/***ÆÕÍ¨»°µ¥**£¨°üÀ¨¸÷ÖÖÂþÓÎ£©**/
/**ºô×ª**/
int query_goforw(float * go_bas_shouldpay,float *go_toll_shouldpay1,float *go_toll_shouldpay2);
/**¶ÌÐÅ***/
int query_short_info(float * short_bas_shouldpay);
/**ÒÆ¶¯ÃÎÍø***/
int query_emobile(float * emobile_msg_fee);
/***gprs***/
int query_gprs(float *gprs_cfee);
/*****¶ÌÐÅ»¥Í¨*******/
int query_ushort_info(float * short_union_cfee);
/*********VPMN***********/
int query_vpmn(float * vpmn_cfee,float *vpmn_lfee);
/***²ÊÐÅ»°µ¥***/

int query_mms(float * mms_fee);
void printYearMonth(char * today,char *btime,char *pyear);


void   initsheet_print();    /* ½øÈëÍ¸Ã÷´òÓ¡·½Ê½ */
void   cancel_print();
void   endsheet_print();     /* ÍË³öÍ¸Ã÷´òÓ¡·½Ê½ */
/****close by wangrq 02-5-29 19:54
void   release_mem();
void   close_file();
**/

char* Trim(char *S);
main(int argc, char *argv[])
{	
	char call_type[2];
	float gen_cfees=0,gen_lfees=0;
	float go_cfees=0,go_lfee1s=0,go_lfee2s=0;
	float short_cfees=0;
	float short_union_cfee=0;
	float gprs_cfees=0;
	float emobile_cfees=0;
	float vpmn_cfee=0;
	float vpmn_lfee=0;
	float mms_fee=0;
	char  message_str[1025];
	memset(login_no,0,sizeof(login_no));      
	memset(user_id,0,sizeof(user_id));      
	memset(bill_type,0,sizeof(bill_type));
	memset(begin_time,0,sizeof(begin_time));
	memset(end_time,0,sizeof(end_time));
	memset(login_accept,0,sizeof(login_accept));
	memset(today,0,sizeof(today));
    
	strcpy(login_no,argv[1]);
	strcpy(user_id,argv[2]);
	strcpy(bill_type,argv[3]);
	strcpy(begin_time,argv[4]);
	strcpy(end_time,argv[5]);
	strcpy(login_accept,argv[6]);
	strcpy(today,argv[7]);
	initsheet_print();
	
	strcpy(call_type,bill_type);
	
	switch(atoi(call_type))
	{
		case ALL:
				if(query_general(&gen_cfees,&gen_lfees)<0)
				exit(1);
				if(query_goforw(&go_cfees,&go_lfee1s,&go_lfee2s)<0)
				exit(1);
				if(query_short_info(&short_cfees)<0)
				exit(1);
				if(query_gprs(&gprs_cfees)<1)
				exit(1);
				if(query_emobile(&emobile_cfees)<0)
				exit(1);
				if(query_ushort_info(&short_union_cfee)<0)
				exit(1);
				if(query_vpmn(&vpmn_cfee,&vpmn_lfee)<0)
				exit(1);
				if(query_mms(&mms_fee)<0)
				exit(1);
				break;
				
		case GL:
		case GR:
	    case GO:
	    		if(query_general(&gen_cfees,&gen_lfees)<0)
				exit(1);
				break;
		case FORW:
				if(query_goforw(&go_cfees,&go_lfee1s,&go_lfee2s)<0)
				exit(1);
				break;
		case SMSG:
				if(query_short_info(&short_cfees)<0)
				exit(1);
				break;
		case GPRS:
				if(query_gprs(&gprs_cfees)<1)
				exit(1);
				break;
		case EMOBI:
				if(query_emobile(&emobile_cfees)<0)
				exit(1);
				break;
		case UNION:
				if(query_ushort_info(&short_union_cfee)<0)
				exit(1);
				break;									
	 	case VPMN:
				if(query_vpmn(&vpmn_cfee,&vpmn_lfee)<0)
				exit(1);
				break;									
	 	case MMS:
				if(query_mms(&mms_fee)<0)
				exit(1);
				break;									
	 	default:
	 			break;
	}
	strcpy(message_str,"");
	sprintf(message_str,"È«²¿»°µ¥ÊµÊÕ»ù±¾·ÑºÏ¼Æ£º%.2f    È«²¿»°µ¥ÊµÊÕ³¤Í¾·ÑºÏ¼Æ£º%.2f    È«²¿»°µ¥ÊµÊÕ·ÑÓÃ×ÜºÏ¼Æ£º%.2f\n",gen_cfees+go_cfees+short_cfees+gprs_cfees+emobile_cfees+short_union_cfee+vpmn_cfee+mms_fee,gen_lfees+go_lfee1s+go_lfee2s+vpmn_lfee,gen_lfees+go_lfee1s+go_lfee2s+gen_cfees+go_cfees+short_cfees+gprs_cfees+emobile_cfees+short_union_cfee+vpmn_cfee+vpmn_lfee+mms_fee);
	printf("%s\n",message_str);
	
	cancel_print();
	endsheet_print();     
	exit(0);
}


void   initsheet_print()/* ½øÈëÍ¸Ã÷´òÓ¡·½Ê½ */
{
       putchar(27);
       putchar('[');
       putchar('5');
       putchar('i');
       putchar(27);
       putchar('@');
       
}

void   cancel_print()
{
       putchar(12);
}
char* Trim(char *S)
{
    int I = 0, i = 0, L = 0;
    L = strlen(S) - 1;
    I = 0;
    while ( (I <= L) && (S[I] <= ' ') && (S[I] > 0) )
    {
        I ++;
    }
    if (I > L)
    {
        S[0] = '\0';
    }
    else
    {
        while ( (S[L] <= ' ')  && (S[L] > 0 ) )
        {
            L --;
        }
        strncpy(S, S + I, L + 1);
        S[L + 1 - I] = '\0';
    }
    return S;
}


void   endsheet_print()   /* ÍË³öÍ¸Ã÷´òÓ¡·½Ê½ */
{
	putchar(27);
	putchar('[');
	putchar('4');
	putchar('i');
}


/** add by wangrq 02-5-29 19:53
void release_mem()
{
	printf("Tuxedo error!!!\n");
	tpdiscon(cd);
	tpfree((char *)fbfr);
	tpfree((char *)send_fb);
	tpfree(messbuf);
	tpterm();

	cancel_print();
	endsheet_print();     
	exit(1);
}
**/
/**
void close_file()
{
	if(fclose(fp) == -1)
	userlog("Close file %s fail!\n",filename);
	strcpy(temp_buf,"rm -f ");
	strcat(temp_buf,filename);
	system(temp_buf);
}
***/
/**ÆÕÍ¨»°µ¥°üÀ¨¸÷ÖÖÂþÓÎ**/
int query_general(float * gen_bas_shouldpay,float *gen_toll_shouldpay)
{
	FBFR *sendbuf,*fbfr;
	FLDLEN len;
	long  revent,len1;
	int occ;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[25],start_datetime[21];
	char  visit_area_code[5],city_name[21],fee_type[7];
	int  call_duration;
	int s_duration=0,n_duration=0,t_duration=0;
	float    discount_cfee,discount_lfee,fee_count;
	float cfee_shd,lfee_shd,fee_shd;
	float info_fee,cfee,cfeeadd,lfee,lfeeadd;
	float sum_discount_cfee=0,sum_discount_lfee=0,sum_cfee=0;
	float sum_lfee=0,sum_info_fee=0,sum_wap_discount=0,sum_wap_fee=0;
	float sum_lfee1=0,sum_lfee2=0;
	float roam_fee=0;
	int line_no=0;
	char  record_str[1025],chat_type[3];
	int err_count=0;
	char nyear[5];
	char talk_type[5];
	char phone_no[25];
	int minu;
	int secon;
	char stime[12];
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÆÕÍ¨»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÆÕÍ¨»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
   	printf("%s",record_str);
	strcpy(record_str,"");
	/*sprintf(record_str,"%s%-15s%-15s%-15s%-5s%-6s%-7s%-5s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ");*/
	/*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ"Ã","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ"); */
	/*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","Í¨»°Ê±¼ä","Ê±³¤","ÇøºÅ", "»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ","Í¨»°µØµã","ÀàÐÍ","»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ");*/
	sprintf(record_str,"%s%-s%\r\n",record_str,"    Í¨»°ÆðÊ¼Ê±¼ä	      Ê±³¤   Í¨»°ÀàÐÍ   ¶Ô·½ºÅÂë 	     Í¨»°µØµã  ³¤Í¾ÀàÐÍ  »ù±¾Í¨»°·Ñ  ³¤Í¾·Ñ");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,G_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	/*printf("enter while(1)\n");**/
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(call_duration);
		if ( Fget(fbfr,F_DURATION,i,(char *)&call_duration,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(visit_area_code);
        if ( Fget(fbfr,F_R_BUREAU,i,(char *)&visit_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		len =sizeof(discount_cfee);
        if ( Fget(fbfr,F_CFEE,i,(char *)&discount_cfee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(discount_lfee);
        if ( Fget(fbfr,F_LFEE,i,(char *)&discount_lfee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(fee_count);
        if ( Fget(fbfr,F_LFEEADD,i,(char *)&fee_count,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(city_name);
        if ( Fget(fbfr,F_CITY_NAME,i,(char *)&city_name,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(fee_type);
        if ( Fget(fbfr,F_ROAM_TYPE,i,(char *)&fee_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(cfee_shd);
        if ( Fget(fbfr,F_CFEE_SHD,i,(char *)&cfee_shd,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(lfee_shd);
        if ( Fget(fbfr,F_lFEE_SHD,i,(char *)&lfee_shd,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(fee_shd);
        if ( Fget(fbfr,F_FEECOUNT_SHD,i,(char *)&fee_shd,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(chat_type);
        if ( Fget(fbfr,F_CHAT_TYPE,i,(char *)&chat_type,&len) < 0 ) {
		err_count++;
		continue;
		}
		len = sizeof(info_fee);
        if ( Fget(fbfr,F_INFO_FEE,i,(char *)&info_fee,&len) < 0 ) {
        err_count++;
	    continue;
		}
		
		sum_discount_cfee=sum_discount_cfee+discount_cfee;
		sum_discount_lfee=sum_discount_lfee+discount_lfee;
		if(strcmp(fee_type,"±¾µØ")!=0)roam_fee+=cfee_shd;
		else sum_cfee=sum_cfee+cfee_shd;
		sum_lfee=sum_lfee+lfee_shd;
		sum_info_fee=sum_info_fee+info_fee;
		t_duration+=call_duration/60;
		n_duration=call_duration%60;
		if(n_duration>0)
		t_duration=t_duration+1;
		minu=call_duration/60;
		secon=call_duration%60;
		memset(stime,0,sizeof(stime));
		sprintf(stime,"%d·Ö%dÃë",minu,secon);
		
		if (atoi(chat_type)==7||atoi(chat_type)==28||atoi(chat_type)==29)
		{
			sum_wap_discount=sum_wap_discount+discount_cfee+discount_lfee;
			sum_wap_fee=sum_wap_fee+cfee_shd+lfee_shd;
		}
		if(strcmp(user_id,msisdn)!=0)
		{
		  strcpy(phone_no,msisdn);
          strcpy(msisdn,other_party);
          strcpy(other_party,phone_no);	
          strcpy(talk_type,"±»½Ð");
		}
		else 
			  strcpy(talk_type,"Ö÷½Ð");
		
		strcpy(record_str,"");
		sprintf(record_str,"%-22s%-12s%-8s%-16s%-14s%-10s%-10.2f%-10.2f%\r\n",start_datetime,stime,talk_type,other_party,city_name,fee_type,cfee_shd,lfee_shd);
		/*sprintf(record_str,"%-22s%-8d%-8s%-16s%-14s%-10s%-10.2f%-10.2f%\r\n",start_datetime,call_duration,talk_type,other_party,city_name,fee_type,cfee_shd,lfee_shd);*/
		/*sprintf(record_str,"%s%-5d%-5s%-7.2f%-7.2f%-7.2f",record_str,call_duration,visit_area_code,discount_cfee,discount_lfee,fee_count);*/
		/*sprintf(record_str,"%s%-9s%-5s%-7.2f%-7.2f%-7.2f%\r\n",record_str,city_name,fee_type,cfee_shd,lfee_shd,cfee_shd+lfee_shd);	*/
        printf("%s",record_str);
         line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
         if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÆÕÍ¨»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
    	 else
   		 sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÆÕÍ¨»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
		 printf("%s",record_str);
		 strcpy(record_str,"");
		 /*sprintf(record_str,"%s%-15s%-15s%-15s%-5s%-6s%-7s%-5s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ");*/
		 /*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ"); */
		                        
		 /*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","Í¨»°Ê±¼ä","Ê±³¤","ÇøºÅ", "»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ","Í¨»°µØµã","ÀàÐÍ","»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ");*/
		 sprintf(record_str,"%s%-s%\r\n",record_str,"    Í¨»°ÆðÊ¼Ê±¼ä	      Ê±³¤    Í¨»°ÀàÐÍ   ¶Ô·½ºÅÂë 	     Í¨»°µØµã  ³¤Í¾ÀàÐÍ  »ù±¾Í¨»°·Ñ  ³¤Í¾·Ñ");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		/*sprintf(record_str,"%15s%10.2f%20s%10.2f%20s%10.2f%20s%10.2f\r\n","»ù±¾·ÑºÏ¼Æ=",sum_discount_cfee,"³¤Í¾·ÑºÏ¼Æ=",sum_discount_lfee,"WAPÍ¨»°·ÑºÏ¼Æ=",sum_wap_discount,"×Ü·ÑÓÃºÏ¼Æ=",sum_discount_cfee+sum_discount_lfee+sum_info_fee); */
	    strcpy(record_str,"");
	    printf("Í¨»°Ê±³¤=%d(·ÖÖÓ)\n",t_duration);
	    sprintf(record_str,"%s%15s%10.2f%20s%10.2f%20s%10.2f%20s%10.2f%20s%10.2f\r\n",record_str,"ÊµÊÕ»ù±¾·ÑºÏ¼Æ=",sum_cfee,"ÊµÊÕ³¤Í¾·ÑºÏ¼Æ=",sum_lfee,"ÊµÊÕWAPÍ¨»°·ÑºÏ¼Æ=",sum_wap_fee,"ÊµÊÕÂþÓÎ·ÑºÏ¼Æ=",roam_fee,"ÊµÊÕ×Ü·ÑÓÃºÏ¼Æ=",sum_cfee+sum_lfee+sum_info_fee); 
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*gen_bas_shouldpay=sum_cfee+roam_fee;
		*gen_toll_shouldpay=sum_lfee;
		
		
	    return 1;
	}
/**ºô×ª²éÑ¯************************/	
 int query_goforw(float * go_bas_shouldpay,float *go_toll_shouldpay1,float *go_toll_shouldpay2)
{	
	FBFR *sendbuf,*fbfr;
	FLDLEN len;
	long  revent,len1;
	char *messbuf;	
	int occ,err_count=0;
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[16],start_datetime[21],a_number[24];
	char  visit_area_code[5],home_area_code[5],city_name[21],fee_type[7];
	int   call_duration;
	int s_duration=0,n_duration=0,t_duration=0;
	float discount_cfee,discount_lfee,fee_count;
	float cfee,cfee_shd,lfee_shd,info_fee;
	float sum_discount_cfee=0,sum_discount_lfee=0,sum_cfee=0,sum_cfee_gf=0;
	float sum_lfee=0,sum_info_fee=0,sum_wap_discount=0,sum_wap_fee=0;
	float sum_lfee1=0,sum_lfee2=0;
	float lfee1,lfee2;
	int line_no=0;
	char  record_str[1025],chat_type[3];
	char nyear[5];
	int minu;
	int secon;
	char stime[12];


	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ºô½Ð×ªÒÆ)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ºô½Ð×ªÒÆ)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
   	printf("%s",record_str);
	strcpy(record_str,"");
	sprintf(record_str,"%s%66sºô½Ð  ±»·Ã ¹éÊô\r\n",record_str,"");
	sprintf(record_str,"%s%-15s%-15s%-15s%-19s%-8s",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","±»½Ðºô×ªºÅ","Í¨»°Ê±¼ä","  Ê±³¤");
	sprintf(record_str,"%s%-5s%-8s%-8s%-8s%-8s%-8s\r\n",record_str," ¾Ö","¾Ö"," »ù±¾·Ñ","   ³¤Í¾·Ñ1","  ³¤Í¾·Ñ2"," Í¨»°ÀàÐÍ");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,SG_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	 
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	/*printf("enter while(1)\n");**/
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = sizeof(a_number);;
		if ( Fget(fbfr,F_A_NUMBER,i,(char *)&a_number,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
	
		len = sizeof(call_duration);
		if ( Fget(fbfr,F_DURATION,i,(char *)&call_duration,&len) < 0 )  {
			err_count++;
			continue;
		}
	
		len = sizeof(visit_area_code);
        if ( Fget(fbfr,F_R_BUREAU,i,(char *)&visit_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
	
		len = sizeof(home_area_code);
        if ( Fget(fbfr,F_H_BUREAU,i,(char *)&home_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
	
        len =sizeof(cfee);
        if ( Fget(fbfr,F_CFEE,i,(char *)&cfee,&len) < 0 ) {
			err_count++;
			continue;
		}
	
		len = sizeof(lfee1);
        if ( Fget(fbfr,F_LFEE,i,(char *)&lfee1,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len = sizeof(lfee2);
        if ( Fget(fbfr,F_LFEEADD,i,(char *)&lfee2,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(fee_type);
        if ( Fget(fbfr,F_FEE_TYPE,i,(char *)&fee_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		Trim(a_number);Trim(other_party);
	    sum_cfee_gf=sum_cfee_gf+cfee;
		sum_lfee1=sum_lfee1+lfee1;
		sum_lfee2=sum_lfee2+lfee2;
		t_duration+=call_duration/60;
		n_duration=call_duration%60;
		if(n_duration>0)
		t_duration=t_duration+1;
		
		minu=call_duration/60;
		secon=call_duration%60;
		memset(stime,0,sizeof(stime));
		sprintf(stime,"%d·Ö%dÃë",minu,secon);

		/*s_duration=s_duration+call_duration;*/
		strcpy(record_str,"");
		sprintf(record_str,"%s%-15s%-15s%-15s%20s%12s   %4s",record_str,a_number,user_id,other_party,start_datetime,stime,visit_area_code);
        sprintf(record_str,"%s%4s%10.2f%10.2f%10.2f%6s\r\n",record_str,home_area_code,cfee,lfee1,lfee2,fee_type); 
		printf("%s",record_str);
        line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
        if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ºô×ª»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ºô½Ð×ªÒÆ)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
		 sprintf(record_str,"%s%-15s%-15s%-15s%-19s%-8s",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","±»½Ðºô×ªºÅ","Í¨»°Ê±¼ä","  Ê±³¤");
		 sprintf(record_str,"%s%-5s%-8s%-8s%-8s%-8s%-8s\r\n",record_str," ¾Ö","¾Ö"," »ù±¾·Ñ","   ³¤Í¾·Ñ1","  ³¤Í¾·Ñ2"," Í¨»°ÀàÐÍ");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		/**
		t_duration=s_duration/60;
		n_duration=s_duration%60;
		if(n_duration>0)
		t_duration=t_duration+1;
		*/
		printf("Í¨»°Ê±³¤=%d(·ÖÖÓ)\n",t_duration);
		strcpy(record_str,"");
		sprintf(record_str,"\r\n%s%10.2f%18s%10.2f%18s%10.2f\r\n","ºô×ª»ù±¾×Ê·ÑºÏ¼Æ=",sum_cfee_gf,"ºô×ª³¤Í¾·Ñ1ºÏ¼Æ=",sum_lfee1,"ºô×ª³¤Í¾·Ñ2ºÏ¼Æ=",sum_lfee2);			
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);			
		printf("%s",record_str);
		*go_bas_shouldpay=sum_cfee_gf;
		*go_toll_shouldpay1=sum_lfee1;
		*go_toll_shouldpay2=sum_lfee2;
		
	    return 1;
	}

	
int query_short_info(float * short_bas_shouldpay)
{	
	FLDLEN len;
	long  revent,len1;
	int occ,err_count=0;
	FBFR *sendbuf,*fbfr;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[16],start_datetime[21];
	char  visit_area_code[5],city_name[21],fee_type[7];
	float fee;
	char source_type[7],type[9];
	float sum_cfee=0,discount_lfee,fee_count,sum_fee=0;
	int line_no=0;
	char  record_str[1025];
	char destination[10];
	char roamtype[20];
	char nyear[5];
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
    sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
    printf("%s",record_str);
    strcpy(record_str,"");
	strcat(record_str,"Ö÷½ÐºÅ                  ±»½ÐºÅ                  Í¨»°Ê±¼ä     »ù±¾·Ñ");
	sprintf(record_str,"%s%-8s%-10s%-6s%-8s\r\n",record_str,"  Ô´ÐÅÏ¢"," ÐÅÏ¢ÀàÐÍ","À´Ô´","·¢ËÍ×´Ì¬");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,S_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		
		len = sizeof(fee);
		if ( Fget(fbfr,F_CFEE,i,(char *)&fee,&len) < 0 )  {
			err_count++;
			continue;
		}
		
        len = sizeof(source_type);
        if ( Fget(fbfr,F_SOURCE_TYPE,i,(char *)&source_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
        len =sizeof(type);
        if ( Fget(fbfr,F_TYPE,i,(char *)&type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len = sizeof(roamtype);
        if ( Fget(fbfr,F_FINISH_STATE,i,(char *)&roamtype,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		sum_fee=sum_fee+fee;
		if (atoi(type)==0) 
		strcpy(destination,"ÊÖ»ú");
		else 
		strcpy(destination,"ÆäËû");
		strcpy(record_str,"");
		sprintf(record_str,"%-15s%-15s%-22s%-.2f    %-6s  %-8s%-6s%-8s\r\n",msisdn,other_party,start_datetime,fee,source_type,type,destination,roamtype);
		printf("%s",record_str);
         line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
         if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
   		 else
   	     sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
     	 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
     	 printf("%s",record_str);
    	 strcpy(record_str,"");
		 strcat(record_str,"Ö÷½ÐºÅ                  ±»½ÐºÅ                  Í¨»°Ê±¼ä    »ù±¾·Ñ");
		 sprintf(record_str,"%s%-8s%-10s%-6s%-8s\r\n",record_str,"  Ô´ÐÅÏ¢"," ÐÅÏ¢ÀàÐÍ","À´Ô´","·¢ËÍ×´Ì¬");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		
		sprintf(record_str,"\r\n¶ÌÐÅ»ù±¾·ÑºÏ¼Æ=%.2f\r\n",sum_fee);						
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*short_bas_shouldpay=sum_fee;
		
	    return 1;
	}
/******emobile******************/
int query_emobile(float * emobile_msg_fee)
{	
	FLDLEN len;
	long  revent,len1;
	int occ,err_count=0;
	FBFR *sendbuf,*fbfr;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[16],start_datetime[21],end_datetime[21],bill_type[5];
	char serv_code[7];
	float sum_fee_mdn=0;
	int line_no=0;
	float	msg_fee=0,month_fee=0;
	char sp_name[61];
	char  record_str[1025],chat_type[3];
	char nyear[5];
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÒÆ¶¯ÃÎÍø)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÒÆ¶¯ÃÎÍø)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	printf("%s",record_str);
	strcpy(record_str,"");
	sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
    strcat(record_str,"¼Æ·ÑºÅÂë     ·þÎñ´úÂë µÚÈý·½ºÅÂë               ÏûÏ¢·¢³öÊ±¼ä         ÏûÏ¢½ÓÊÜÊ±¼ä         ÀàÐÍ  ·ÑÓÃ  SPÃû³Æ\r\n");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,E_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(serv_code);
		if ( Fget(fbfr,F_SERV_CODE,i,(char *)&serv_code,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		
		len = sizeof(end_datetime);
		if ( Fget(fbfr,F_END_TIME,i,(char *)&end_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		
        len = sizeof(bill_type);
        if ( Fget(fbfr,F_BILL_TYPE,i,(char *)&bill_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		msg_fee=0;
        len =sizeof(msg_fee);
        if ( Fget(fbfr,F_MSG_FEE,i,(char *)&msg_fee,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len = sizeof(sp_name);
        if ( Fget(fbfr,F_SP_NAME,i,(char *)&sp_name,&len) < 0 ) {
			err_count++;
			continue;
		}
	/*	strcpy(record_str,"°´Ìõ"); */
		
    	strcpy(record_str,user_id);
    	strcat(record_str,"  ");
    	strcat(record_str,serv_code);
    	strcat(record_str,"  ");
    	strcat(record_str,other_party);
    	strcat(record_str,"  ");
    	strcat(record_str,start_datetime);
    	strcat(record_str,"  ");
    	strcat(record_str,end_datetime);
    	strcat(record_str,"  ");
    	strcat(record_str,bill_type);
    	sprintf(record_str,"%s%s%.2f  %s\r\n",record_str,"  ",msg_fee,sp_name);
    	sum_fee_mdn=sum_fee_mdn+msg_fee;
		printf("%s",record_str);
        line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
        if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÒÆ¶¯ÃÎÍø)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
    	else
   		sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÒÆ¶¯ÃÎÍø)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 printf("%s",record_str);
         strcpy(record_str,"");
		 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
         strcat(record_str,"¼Æ·ÑºÅÂë     ·þÎñ´úÂë µÚÈý·½ºÅÂë               ÏûÏ¢·¢³öÊ±¼ä         ÏûÏ¢½ÓÊÜÊ±¼ä         ÀàÐÍ  ·ÑÓÃ  SPÃû³Æ\r\n");
         sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		
		sprintf(record_str,"\r\nÐÅÏ¢·ÑºÏ¼Æ=%.2f \r\n\r\n",sum_fee_mdn);
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*emobile_msg_fee=sum_fee_mdn;
		
	    return 1;
	}
	
 /*********GPRS*********/
 int query_gprs(float *gprs_cfee)
{	
	FLDLEN len;
	long  revent,len1;
	int occ,err_count=0;
	FBFR *sendbuf,*fbfr;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[16],start_datetime[21],a_number[24];
	char  visit_area_code[5],home_area_code[5],city_name[21],fee_type[7];
	int   call_duration;
	float discount_cfee,discount_lfee,fee_count;
	float cfee=0;
	float sum_discount_cfee=0,sum_discount_lfee=0,sum_cfee=0,sum_cfee_gf=0;
	float sum_lfee=0,sum_info_fee=0,sum_wap_discount=0,sum_wap_fee=0;
	float sum_lfee1=0,sum_lfee2=0;
	float lfee1,lfee2;
	int line_no=0;
	char  record_str[1025],chat_type[3];
	/**add gprs 02-5-28 14:34 wrq **/
	float dataflow=0,dataflow_sum=0;
    float flowsum=0;
    int gprs_duration;
    char record_type[2+1];
    char apncode[20+1];
    char roam_type[1+1];
    char roam_name[8+1];
    float sum_fee_gprs=0;
    char nyear[5];
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(GPRS)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(GPRS)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	printf("%s",record_str);
	strcpy(record_str,"");
	sprintf(record_str,"%s%8s %18s %-20s %18s %22s %12s %8s %6s %6s %6s\r\n",record_str,"ºô½ÐÀàÐÍ","Ö÷½ÐºÅÂë","APN´úÂë","Í¨ÐÅÈÕÆÚ","Á÷Á¿(k)","Ê±³¤","ÂþÓÎÀàÐÍ","ÂþÓÎµØ","¹éÊôµØ","·ÑÓÃ");
	sprintf(record_str,"%s--------------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}

	strcpy(ServiceName,GPRS_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = sizeof(record_type);;
		if ( Fget(fbfr,F_RECORD_TYPE,i,(char *)&record_type,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(apncode);
		if ( Fget(fbfr,F_APN_CODE,i,(char *)&apncode,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		
		len = sizeof(dataflow);
		if ( Fget(fbfr,F_DATAFLOW,i,(char *)&dataflow,&len) < 0 )  {
			err_count++;
			continue;
		}
		
        len = sizeof(gprs_duration);
        if ( Fget(fbfr,F_DURATION,i,(char *)&gprs_duration,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(roam_name);
        if ( Fget(fbfr,F_ROAM_TYPE,i,(char *)&roam_name,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		
        len =sizeof(visit_area_code);
        if ( Fget(fbfr,F_R_BUREAU,i,(char *)&visit_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len = sizeof(home_area_code);
        if ( Fget(fbfr,F_H_BUREAU,i,(char *)&home_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(cfee);
		if ( Fget(fbfr,F_CFEE,i,(char *)&cfee,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		dataflow_sum=dataflow_sum+dataflow;
		sum_cfee=sum_cfee+cfee;
		strcpy(record_str,"");
		sprintf(record_str,"%8s %16s %-20s %17s %19.3f %10d %8s %7s %5s%.2f \r\n",record_type,user_id,apncode,start_datetime,dataflow,gprs_duration,roam_name,visit_area_code,home_area_code,cfee);
		printf("%s",record_str);
        line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
        if ( strcmp(end_time,"month") == 0 ) 
		 sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(GPRS)          µç»°ºÅÂë£º%-15s%6s½áÕÊÔÂ·Ý%2s\r\n"," ",user_id," ",begin_time);
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(GPRS)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 printf("%s",record_str);
         strcpy(record_str,"");
         sprintf(record_str,"%s%8s %18s %-20s %18s %22s %12s %8s %6s %6s %6s\r\n",record_str,"ºô½ÐÀàÐÍ","Ö÷½ÐºÅÂë","APN´úÂë","Í¨ÐÅÈÕÆÚ","Á÷Á¿","Ê±³¤","ÂþÓÎÀàÐÍ","ÂþÓÎµØ","¹éÊôµØ","·ÑÓÃ");
		 sprintf(record_str,"%s--------------------------------------------------------------------------------------------------------------------------------\r\n",record_str);

		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		strcpy(record_str,"");
        /**if ( strcmp(end_time,"month") == 0 ) 
        {
        	flowsum=dataflow_sum/1024;
	      	if(flowsum > 0)
			 {
			  if(flowsum <=1)
			  cfee=flowsum*0.02;
			  else if(flowsum >1 && flowsum <=4)
			  {
			  flowsum=flowsum-1;
			  cfee=(1-0)*0.02+ flowsum*0.01;
			  }
			  else if(flowsum >4 && flowsum <=30)
			  {
			  flowsum =flowsum-4;
			  cfee=(1-0)*0.02+(4-1)*0.01+flowsum*0.005;
			  }
			  else if(flowsum>30)
			  {
			  cfee=180;
			  }
			 }
			 sum_fee_gprs=cfee;
		*/
		sprintf(record_str,"GPRSÁ÷Á¿ºÏ¼Æ=%.3f k£¬»°·ÑºÏ¼Æ=%.2f\r\n",dataflow_sum,sum_cfee);
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
		*gprs_cfee=sum_cfee;
	    return 1;
	}
int query_ushort_info(float *short_union_cfee)
{	
	FLDLEN len;
	long  revent,len1;
	int occ,err_count=0;
	FBFR *sendbuf,*fbfr;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[25],msisdn[16],start_datetime[21];
	char  visit_area_code[5],city_name[21],fee_type[7];
	float fee;
	char source_type[7],type[9];
	float sum_cfee=0,discount_lfee,fee_count,sum_fee=0;
	int line_no=0;
	char  record_str[1025];
	char destination[10];
	char roamtype[20];
	char nyear[5];
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ»¥Í¨)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ»¥Í¨)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
    sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
    printf("%s",record_str);
    strcpy(record_str,"");
	strcat(record_str,"Ö÷½ÐºÅ                  ±»½ÐºÅ                  Í¨»°Ê±¼ä     »ù±¾·Ñ");
	sprintf(record_str,"%s%-8s%-10s%-6s%-8s\r\n",record_str,"  Ô´ÐÅÏ¢"," ÐÅÏ¢ÀàÐÍ","À´Ô´","·¢ËÍ×´Ì¬");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,SUNION_SERVNAME);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = USER_ID_LEN;
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		
		
		len = sizeof(fee);
		if ( Fget(fbfr,F_CFEE,i,(char *)&fee,&len) < 0 )  {
			err_count++;
			continue;
		}
		
        len = sizeof(source_type);
        if ( Fget(fbfr,F_SOURCE_TYPE,i,(char *)&source_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
        len =sizeof(type);
        if ( Fget(fbfr,F_TYPE,i,(char *)&type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len = sizeof(roamtype);
        if ( Fget(fbfr,F_FINISH_STATE,i,(char *)&roamtype,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		sum_fee=sum_fee+fee;
		if (atoi(type)==0) 
		strcpy(destination,"ÊÖ»ú");
		else 
		strcpy(destination,"ÆäËû");
		strcpy(record_str,"");
		sprintf(record_str,"%-15s%-15s%-22s%-.2f    %-6s  %-8s%-6s%-8s\r\n",msisdn,other_party,start_datetime,fee,source_type,type,destination,roamtype);
		printf("%s",record_str);
         line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
         if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ»¥Í¨)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
   		 else
   	     sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(¶ÌÐÅ»¥Í¨)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
     	 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
     	 printf("%s",record_str);
    	 strcpy(record_str,"");
		 strcat(record_str,"Ö÷½ÐºÅ                  ±»½ÐºÅ                  Í¨»°Ê±¼ä    »ù±¾·Ñ");
		 sprintf(record_str,"%s%-8s%-10s%-6s%-8s\r\n",record_str,"  Ô´ÐÅÏ¢"," ÐÅÏ¢ÀàÐÍ","À´Ô´","·¢ËÍ×´Ì¬");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		
		sprintf(record_str,"\r\n»¥Áª¶ÌÐÅ»ù±¾·ÑºÏ¼Æ=%.2f\r\n",sum_fee);						
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*short_union_cfee=sum_fee;
		
	    return 1;
	}	
/**add by wangrenqiang **/
int query_vpmn(float * vpmn_cfee,float *vpmn_lfee)
{
	FBFR *sendbuf,*fbfr;
	FLDLEN len;
	long  revent,len1;
	int occ;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[17],msisdn[17],start_datetime[21];
	char  visit_area_code[5],city_name[21],fee_type[2];
	int  call_duration;
	int s_duration=0,n_duration=0,t_duration=0;
	float    discount_cfee,discount_lfee,fee_count;
	float cfee_shd,lfee_shd,fee_shd;
	float info_fee,cfee,cfeeadd,lfee,lfeeadd;
	float sum_cfee=0;
	float sum_lfee=0,sum_addfee=0;
	float sum_lfee1=0,sum_lfee2=0;
	float add_fee=0;
	int line_no=0;
	char  record_str[1025],chat_type[3];
	int err_count=0;
	char nyear[5];
	char talk_type[10];
	char phone_no[17];
	char call_type[3];
	char group_code[11];
	char fee_flag[2];
	char roam_type[12];
	char pay_type[5];
	char calling_group[7];
	
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÖÇÄÜÒµÎñ»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÖÇÄÜÒµÎñ»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
   	printf("%s",record_str);
	strcpy(record_str,"");
	/*sprintf(record_str,"%s%-15s%-15s%-15s%-5s%-6s%-7s%-5s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ");*/
	/*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ"Ã","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ"); */
	/*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","Í¨»°Ê±¼ä","Ê±³¤","ÇøºÅ", "»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ","Í¨»°µØµã","ÀàÐÍ","»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ");*/
	sprintf(record_str,"%s%-s%\r\n",record_str,"    Í¨»°ÆðÊ¼Ê±¼ä	   Ê±³¤(Ãë)   Í¨»°ÀàÐÍ       ¶Ô·½ºÅÂë 	   ÄÚ²¿±àºÅ   Í¨»°µØµã   ³¤Í¾ÀàÐÍ   »ù±¾Í¨»°·Ñ   ³¤Í¾·Ñ  ¼¯ÍÅ´úÂë");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,SVPMN);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	/*printf("enter while(1)\n");**/
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = sizeof(call_type);
		if ( Fget(fbfr,F_TYPE,i,(char *)&call_type,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(msisdn);
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&other_party,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(start_datetime);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&start_datetime,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(call_duration);
		if ( Fget(fbfr,F_DURATION,i,(char *)&call_duration,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(visit_area_code);
        if ( Fget(fbfr,F_R_BUREAU,i,(char *)&visit_area_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		len =sizeof(cfee);
        if ( Fget(fbfr,F_CFEE,i,(char *)&cfee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(lfee);
        if ( Fget(fbfr,F_LFEE,i,(char *)&lfee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(add_fee);
        if ( Fget(fbfr,F_LFEEADD,i,(char *)&add_fee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(city_name);
        if ( Fget(fbfr,F_CITY_NAME,i,(char *)&city_name,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(fee_type);
        if ( Fget(fbfr,F_ROAM_TYPE,i,(char *)&fee_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(group_code);
        if ( Fget(fbfr,F_APN_CODE,i,(char *)&group_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		len = sizeof(calling_group);
        if ( Fget(fbfr,F_SERV_CODE,i,(char *)&calling_group,&len) < 0 ) {
			err_count++;
			continue;
		}
	/*	len = sizeof(fee_flag);
        if ( Fget(fbfr,F_BILL_TYPE,i,(char *)&fee_flag,&len) < 0 ) {
		err_count++;
		continue;
	}
	*/
		sum_addfee+=add_fee;
		sum_cfee+=cfee;
		sum_lfee+=lfee;
		t_duration+=call_duration/60;
		n_duration=call_duration%60;
		if(n_duration>0)
		t_duration=t_duration+1;
		if(call_type[0]!='5')
		{
		if(strcmp(fee_type,"0")==0)
		strcpy(roam_type,"±¾µØ");
		else if(!strcmp(fee_type,"1"))
		strcpy(roam_type,"Ê¡ÄÚ");
		else if(!strcmp(fee_type,"2"))
		strcpy(roam_type,"Ê¡¼Ê");
		else if(!strcmp(fee_type,"3"))
		strcpy(roam_type,"¸Û°ÄÌ¨");
		else if(!strcmp(fee_type,"4"))
		strcpy(roam_type,"¹ú¼Ê");
		else if(!strcmp(fee_type,"3"))
		strcpy(roam_type,"ÆäËû");
		}
		if(!strcmp(call_type,"13"))
		strcpy(talk_type,"VPMNÖ÷½Ð");
		else if(!strcmp(call_type,"23"))
		strcpy(talk_type,"VPMN±»½Ð");
		else if(call_type[0]=='5')
		strcpy(talk_type,"Ç×ÇéºÅÂë");
		if(call_type[0]=='5')
		{
		if(strcmp(fee_type,"0")==0)
		strcpy(roam_type,"¹ú¼Ê");
		else if(!strcmp(fee_type,"1"))
		strcpy(roam_type,"Ê¡¼Ê");
		else if(!strcmp(fee_type,"2"))
		strcpy(roam_type,"Ê¡ÄÚ");
		else if(!strcmp(fee_type,"3"))
		strcpy(roam_type,"±¾µØ");
		else if(!strcmp(fee_type,"4"))
		strcpy(roam_type,"¸Û°ÄÌ¨");
		else if(!strcmp(fee_type,"6"))
		strcpy(roam_type,"°´´ÎÊÕ·Ñ");
		else if(!strcmp(fee_type,"7"))
		strcpy(roam_type,"²¥´ò¹ÜÀí");
		else if(!strcmp(fee_type,"8"))
		strcpy(roam_type,"ºô½Ð»°ÎñÔ±");
			
			}
		/*if(strcmp(user_id,msisdn)!=0)
		{
		  strcpy(phone_no,msisdn);
          strcpy(msisdn,other_party);
          strcpy(other_party,phone_no);	
          strcpy(talk_type,"±»½Ð");
		}
		else 
			  strcpy(talk_type,"Ö÷½Ð");
		*/		  
	
		/*if(!strcmp(fee_flag,"0"))
		 strcpy(pay_type,"¼¯ÍÅ");
		else strcpy(pay_type,"¸öÈË");
		*/
		strcpy(record_str,"");
		sprintf(record_str,"%-22s%-8d%-12s%-16s%-12s%-12s%-10s%-10.2f%-10.2f%10s%\r\n",start_datetime,call_duration,talk_type,other_party,calling_group,city_name,roam_type,cfee,lfee,group_code);	
		/*sprintf(record_str,"%s%-5d%-5s%-7.2f%-7.2f%-7.2f",record_str,call_duration,visit_area_code,discount_cfee,discount_lfee,fee_count);*/
		/*sprintf(record_str,"%s%-9s%-5s%-7.2f%-7.2f%-7.2f%\r\n",record_str,city_name,fee_type,cfee_shd,lfee_shd,cfee_shd+lfee_shd);	*/
        printf("%s",record_str);
         line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
         if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÖÇÄÜÒµÎñ»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
    	 else
   		 sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(ÖÇÄÜÒµÎñ»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
		 printf("%s",record_str);
		 strcpy(record_str,"");
		 /*sprintf(record_str,"%s%-15s%-15s%-15s%-5s%-6s%-7s%-5s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ");*/
		 /*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"","","", "ºô½Ð","Í¨»°µØ","","","·ÑÓÃ","      ","Í¨»°","ÊµÊÕ","ÊµÊÕ","ÊµÊÕ"); */
		                        
		 /*sprintf(record_str,"%s%-15s%-15s%-20s%-5s%-5s%-7s%-7s%-7s%-9s%-5s%-7s%-7s%-7s\r\n",record_str,"Ö÷½ÐºÅ","±»½ÐºÅ","Í¨»°Ê±¼ä","Ê±³¤","ÇøºÅ", "»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ","Í¨»°µØµã","ÀàÐÍ","»ù±¾·Ñ","³¤Í¾·Ñ","ºÏ¼Æ");*/
		 sprintf(record_str,"%s%-s%\r\n",record_str,"    Í¨»°ÆðÊ¼Ê±¼ä	   Ê±³¤(Ãë)   Í¨»°ÀàÐÍ       ¶Ô·½ºÅÂë 	   ÄÚ²¿±àºÅ   Í¨»°µØµã   ³¤Í¾ÀàÐÍ   »ù±¾Í¨»°·Ñ   ³¤Í¾·Ñ  ¼¯ÍÅ´úÂë");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */

		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		/*sprintf(record_str,"%15s%10.2f%20s%10.2f%20s%10.2f%20s%10.2f\r\n","»ù±¾·ÑºÏ¼Æ=",sum_discount_cfee,"³¤Í¾·ÑºÏ¼Æ=",sum_discount_lfee,"WAPÍ¨»°·ÑºÏ¼Æ=",sum_wap_discount,"×Ü·ÑÓÃºÏ¼Æ=",sum_discount_cfee+sum_discount_lfee+sum_info_fee); */
	    strcpy(record_str,"");
	    printf("Í¨»°Ê±³¤=%d(·ÖÖÓ)\n",t_duration);
	    sprintf(record_str,"%s%15s%10.2f%20s%10.2f%20s%10.2f%20s%10.2f\r\n",record_str,"»ù±¾Í¨»°·ÑºÏ¼Æ=",sum_cfee,"³¤Í¾Í¨»°·ÑºÏ¼Æ=",sum_lfee,"¸½¼ÓÍ¨»°·ÑºÏ¼Æ=",sum_addfee,"ÖÇÄÜÒµÎñ×Ü·ÑÓÃºÏ¼Æ=",sum_cfee+sum_lfee+sum_addfee); 
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*vpmn_cfee=sum_cfee+sum_addfee;
		*vpmn_lfee=sum_lfee;
		
		
	    return 1;
	}

int query_mms(float * mms_fee)
{
	FBFR *sendbuf,*fbfr;
	FLDLEN len;
	long  revent,len1;
	int occ;
	char *messbuf;	
	int i;
	char ServiceName[20];
	int cd;
	char other_party[31],msisdn[16],start_datetime[21];
	char  visit_area_code[5],city_name[21],fee_type[5];
	int  call_duration;
	int s_duration=0,n_duration=0,t_duration=0;
	float    discount_cfee,discount_lfee,fee_count;
	float cfee_shd,lfee_shd,fee_shd;
	float mms_info_fee=0,cfee,cfeeadd,lfee,lfeeadd;
	
	float sum_lfee=0,sum_addfee=0;
	float sum_lfee1=0,sum_lfee2=0;
	float sum_cfee=0,sum_basic_fee=0,sum_info_fee=0;
	
	float add_fee=0;
	int line_no=0;
	char  record_str[1025],chat_type[3];
	int err_count=0;
	char nyear[5];
	char talk_type[10];
	char phone_no[17];
	char call_type[20];
	char group_code[11];
	char fee_flag[2];
	char roam_type[12];
	char pay_type[5];
	char calling_group[7];
	
	char mm_type[3];
	char send_address[31];
	char receive_address[31];
	char send_time[20];
	char app_type[2];
	char serv_code[7];
	char oper_code[12];
	char charge_type[3];
	char sp_name[61];
	char sp_code[7];
	float mms_basic_fee=0;;
		
	if ( strcmp(end_time,"month") == 0 )
	{
	printYearMonth(today,begin_time,nyear); 
	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(²ÊÐÅ»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    }
    else
   	sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(²ÊÐÅ»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
   	sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
   	printf("%s",record_str);
	strcpy(record_str,"");
	sprintf(record_str,"%s%-s%\r\n",record_str,"	ÀàÐÍ  	¶Ô¶Ë			   Ê¹ÓÃÊ±¼ä       Í¨ÐÅ·Ñ	ÐÅÏ¢·Ñ   ÐÅÏ¢·ÑÀàÐÍ ÒµÎñ´úÂë 	SP´úÂëÃû³Æ	                 ");
	sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
	printf("%s",record_str);
	if ( (fbfr= (FBFR *)tpalloc("FML","",4096)) == NULL ) 
	{
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
	
	if (tpinit((TPINIT *) NULL) == -1) {
		printf("Tpinit failed\n");
         return -1;	}
	
	if ( ( messbuf=(char *)tpalloc("STRING","",12)) == NULL ) {
		printf("alloc messbuf err\n");
		tpterm();
         return -1;
	}

	if((sendbuf = (FBFR *) tpalloc("FML", "", 1024)) == NULL) {
		printf("Error allocating send buffer\n");
		tpfree(messbuf);
		tpterm();
         return -1;
	}
	if ( Fchg(sendbuf,F_REQ_TABLE_TYPE,0,bill_type,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_USER_ID,0,user_id,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( Fchg(sendbuf,F_REQ_BEGIN_TIME,0,begin_time,(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	if ( strlen(end_time) == 0 ) 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,"month",(FLDLEN)0) < 0 ) 
	   printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	else 
	{
	   if ( Fchg(sendbuf,F_REQ_END_TIME,0,end_time,(FLDLEN)0) < 0 ) 
	       printf("Fchg %s(%i)\n",tpstrerror(tperrno),tperrno);
	}
	strcpy(ServiceName,SMMS);
	if ( (cd =tpconnect(ServiceName,(char *)sendbuf,0L,TPRECVONLY)) == -1 )
	  {
			tpfree(messbuf);
			tpfree((char *)sendbuf);
			tpterm();
	       	printf(" Á¬½Ó²éÑ¯·þÎñ(%s)´íÎó \n",ServiceName);
             return -1;
	  }
	
	
	
	while (1) 
	{

	Finit(fbfr,(FLDLEN)Fsizeof(fbfr));
	/*printf("enter while(1)\n");**/
	if ( tprecv(cd,(char **)&fbfr,&len1,TPNOCHANGE,&revent) != -1 )
	 {
		printf("(%i) tprecv err:%s(%i) %i\n",tpurcode,tpstrerror(tperrno),tperrno,__LINE__);
		
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		printf(" ½ÓÊÕ½á¹û´íÎó£¡\n");
         return -1;
	}

	if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
	{   printf("server failed\n");
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
	}
		
	occ = Foccur(fbfr,F_USER_ID);
	if ( occ == 0 ) break; 	
	for (i=0; i<occ ;i++) 
	 {
		
		len = sizeof(msisdn);
		if ( Fget(fbfr,F_USER_ID,i,(char *)&msisdn,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(mm_type);
		if ( Fget(fbfr,F_TYPE,i,(char *)&mm_type,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(app_type);
		if ( Fget(fbfr,F_FEE_TYPE,i,(char *)&app_type,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(other_party);
		if ( Fget(fbfr,F_OTHER_PARTY,i,(char *)&receive_address,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(send_address);
		if ( Fget(fbfr,F_USER_ID2,i,(char *)&send_address,&len) < 0 )  {
			err_count++;
			continue;
		}
		len=sizeof(send_time);
		if ( Fget(fbfr,F_START_TIME,i,(char *)&send_time,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(mms_basic_fee);
		if ( Fget(fbfr,F_CFEE,i,(char *)&mms_basic_fee,&len) < 0 )  {
			err_count++;
			continue;
		}
		len = sizeof(mms_info_fee);
        if ( Fget(fbfr,F_LFEE,i,(char *)&mms_info_fee,&len) < 0 ) {
			err_count++;
			continue;
		}
		len =sizeof(charge_type);
        if ( Fget(fbfr,F_SOURCE_TYPE,i,(char *)&charge_type,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		len =sizeof(sp_name);
        if ( Fget(fbfr,F_ROAM_TYPE,i,(char *)&sp_name,&len) < 0 ) {
			err_count++;
			continue;
		}
		
		
		len = sizeof(oper_code);
        if ( Fget(fbfr,F_APN_CODE,i,(char *)&oper_code,&len) < 0 ) {
			err_count++;
			continue;
		}
		
	/*	len = sizeof(fee_flag);
        if ( Fget(fbfr,F_BILL_TYPE,i,(char *)&fee_flag,&len) < 0 ) {
		err_count++;
		continue;
	}
	*/
		
		sum_basic_fee+=mms_basic_fee;
		sum_info_fee+=mms_info_fee;
		
		if(!strcmp(mm_type,"00"))strcpy(call_type,"Ê¹ÓÃÖÕ¶ËÖ÷½Ð");
		else if(!strcmp(mm_type,"01"))
		strcpy(call_type,"Ê¹ÓÃÓÊÏäÖ÷½Ð");
		else if(!strcmp(mm_type,"02"))
		strcpy(call_type,"SPÖ÷½Ð");
		else if(!strcmp(mm_type,"03"))
		strcpy(call_type,"±»½Ð");
		else strcpy(call_type,"Ö÷½Ð");
		
		if(!strcmp(mm_type,"00")||!strcmp(mm_type,"01")||!strcmp(mm_type,"02"))
		strcpy(other_party,receive_address);
		
		else if(!strcmp(mm_type,"03"))
		strcpy(other_party,send_address);
		
		else strcpy(other_party,receive_address);
		
		if(!strcmp(charge_type,"00"))
		strcpy(fee_type,"Ãâ·Ñ");
		
		else if(!strcmp(charge_type,"01")||!strcmp(charge_type,"03"))
		strcpy(fee_type,"°´Ìõ");
		
		else if(!strcmp(charge_type,"02"))
		strcpy(fee_type,"°üÔÂ");
		
		else strcpy(fee_type,"°´Ìõ");
		
		
		
		
		strcpy(record_str,"");
		
		sprintf(record_str,"%-15s%-25s%-20s%-14.2f%-6.2f%4s%12s%30s\r\n",call_type,other_party,send_time,mms_basic_fee,mms_info_fee,fee_type,oper_code,sp_name);	
		/*sprintf(record_str,"%s%-5d%-5s%-7.2f%-7.2f%-7.2f",record_str,call_duration,visit_area_code,discount_cfee,discount_lfee,fee_count);*/
		/*sprintf(record_str,"%s%-9s%-5s%-7.2f%-7.2f%-7.2f%\r\n",record_str,city_name,fee_type,cfee_shd,lfee_shd,cfee_shd+lfee_shd);	*/
        printf("%s",record_str);
         line_no++;
        if(line_no%LINE_NUM==0&&line_no >0)
        {
         if ( strcmp(end_time,"month") == 0 )
		{
		printYearMonth(today,begin_time,nyear); 
		sprintf(record_str,"\r\n%10sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(²ÊÐÅ»°µ¥)          µç»°ºÅÂë£º%-15s%6s½áÕÊÄê%sÔÂ·Ý%2s\r\n"," ",user_id," ",nyear,begin_time);
    	}
    	 else
   		 sprintf(record_str,"\r\n%20sËÄ´¨Ê¡ÒÆ¶¯Í¨ÐÅ¹«Ë¾Ïêµ¥(²ÊÐÅ»°µ¥)          µç»°ºÅÂë£º%-15s%6sÍ¨»°Ê±¼ä%s---%s\r\n"," ",user_id," ",begin_time,end_time);
		 sprintf(record_str,"%s¹¤ºÅ:%s       Á÷Ë®ºÅ:%s     ´òÓ¡Ê±¼ä£º%s\r\n\r\n",record_str,login_no,login_accept,today);
		 printf("%s",record_str);
		 strcpy(record_str,"");
		 sprintf(record_str,"%s%-s%\r\n",record_str,"	ÀàÐÍ  	¶Ô¶Ë			   Ê¹ÓÃÊ±¼ä       Í¨ÐÅ·Ñ	ÐÅÏ¢·Ñ   ÐÅÏ¢·ÑÀàÐÍ ÒµÎñ´úÂë 	SP´úÂëÃû³Æ	                 ");
		 sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		 printf("%s",record_str);
        }	
         
	     
	}/****for Ñ­»·½áÊø*******/
      
	  strcpy(messbuf,"RecvOK"); 
      
   
	if ( tpsend(cd, (char *)messbuf, strlen(messbuf),TPRECVONLY, &revent ) == -1 ) 
	{
	    printf(" send RecvOK error\n");
         return -1;
	}
	if ( err_count > 1000 ) { err_count = 0 ; break;}


  }  /* end while (1) */
	    sum_cfee=sum_basic_fee+sum_info_fee;
		tpfree((char *)fbfr);
		tpfree((char *)sendbuf);
		tpfree(messbuf);
		tpterm();
		strcpy(record_str,"");
	    sprintf(record_str,"%s²ÊÐÅÒµÎñ·ÑºÏ¼Æ=%10.2f,Í¨»°·ÑºÏ¼Æ:%10.2f ÐÅÏ¢·ÑºÏ¼Æ: %10.2f\r\n",record_str,sum_cfee,sum_basic_fee,sum_info_fee); 
		sprintf(record_str,"%s------------------------------------------------------------------------------------------------------------------------\r\n",record_str);
		printf("%s",record_str);
		*mms_fee=sum_cfee;
		
	    return 1;
	}


void printYearMonth(char * today,char *btime,char *pyear)
{
	char year[5];
	char month[3];
	int iyear,nyear;
	strncpy(year,today,4);
	iyear=atoi(year);
	strncpy(month,today+4,2);
	month[3]='\0';
	if(strcmp(month,btime)<0)
	iyear=iyear-1;
	sprintf(pyear,"%d",iyear);
}
