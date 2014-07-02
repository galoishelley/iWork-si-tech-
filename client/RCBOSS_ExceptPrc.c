#include "general32.flds.h"

#define MAXMODULENUM 60 
#define  TMPLEN          256
#define  WORKNOLEN       6
#define  OPCODELEN       4
#define  ORGCODELEN      9
#define  TOTALDATELEN    8
#define  RETURNCODELEN   4
#define  RETURNMSGLEN    200
#define  RET_OK          0
#define  RET_TPALLOC_ERR  1105
#define RET_TPCALL_ERR    3900
EXEC SQL INCLUDE SQLCA;

#if defined(__STDC__) || defined(__cplusplus)
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{

    int    input_par_num=0,output_par_num=0;
    FLDLEN32 len,rcvLen;
    int    i=0,num=0;
    char   temp_buf[100];
    FBFR32 *send_fb=NULL,*rcv_fb=NULL;
    
    EXEC SQL BEGIN DECLARE SECTION;
        char TmpSqlStr[TMPLEN + 1];
        char vPhoneNo[11+1];
        char vDate[8+1];
        char vProvCode[4+1];
    EXEC SQL END DECLARE SECTION;

    int  RetValue = RET_OK;
    long RetCall = 0;
    char RetCode[RETURNCODELEN+10+1];
    char RetMsg[RETURNMSGLEN+1];
    char InWorkNo[6+1];
    char InOrgCode[9+1];
    char InOpCode[4+1];
    char svcName[20+1];
    int  InNum=0;
    
    memset(InWorkNo,0x00,sizeof(InWorkNo));
    memset(InOrgCode,0x00,sizeof(InOrgCode));
    memset(InOpCode,0x00,sizeof(InOpCode));
    memset(TmpSqlStr,0x00,sizeof(TmpSqlStr));
    memset(vPhoneNo,0x00,sizeof(vPhoneNo));
    memset(vDate,0x00,sizeof(vDate));
    memset(vProvCode,0x00,sizeof(vProvCode));
    memset(svcName,0x00,sizeof(svcName));
    

    if(argc < 6) {
        (void) fprintf(stderr, "Usage: RCBOSSExceptPro aaaa01 010100101 20040527 451 50\n");
        exit(1);
    }
    
    strncpy(InWorkNo,argv[1],6);
    strncpy(InOrgCode,argv[2],9);
    strncpy(vDate,argv[3],8);
    strncpy(vProvCode,argv[4],3);
    InNum=atoi(argv[5]);
printf("InWorkNo=[%s],InOrgCode=[%s],ProvCode=[%s],Date=[%s],Num=[%d]\n",InWorkNo,InOrgCode,vProvCode,vDate,InNum);
    
    strncpy(svcName,"s4133Snd",8);
    strncpy(InOpCode,"4133",4);

	spublicDBLogin(transIN, transOUT, "Exception",LABELCBOSS);

    Sinitn(TmpSqlStr);
    sprintf(TmpSqlStr,"select msisdn from in_coming_user where start_date='%s'and home_prov_code='%s'",vDate,vProvCode);

    printf("TmpSqlStr=[%s]\n",TmpSqlStr);
    EXEC SQL PREPARE selstate FROM :TmpSqlStr;
    EXEC SQL DECLARE sel_cur CURSOR FOR selstate;
    EXEC SQL OPEN sel_cur;
    EXEC SQL FETCH sel_cur INTO :vPhoneNo;
    if(SQLCODE !=0){
        if(SQLCODE == 1403 ){
            printf("漫游号码数据不存在请检查!\n");
            return -1;
        }
        printf("FETCH ERROR SQLCODE=[%d][%s]\n",SQLCODE,sqlca.sqlerrm.sqlerrmc);
        return -1;
    }
    num=0;
    printf("\n");
    printf("--------------------------------------------------------------------- \n");
    printf("---------------     一级BOSS异常处理程序     ------------------------ \n");
    printf("------------------------------------------------------------------- - \n"); 
    printf("\n");

    printf("对【%s】执行【%d】笔交易--------------------------------------------- \n",vProvCode,InNum);
    while(SQLCODE==0)
    {
        /* 达到设置的交易总数就 BREAK */
        if(num == InNum)  break;
        num = num + 1;

        input_par_num = 6;
        output_par_num = 3;
        len = 40 +  WORKNOLEN + ORGCODELEN + OPCODELEN + 9*MAXMODULENUM + 3*MAXMODULENUM + 9*MAXMODULENUM + 3*MAXMODULENUM + 1 +1;
        send_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(len));
        if(send_fb==(FBFR32 *)NULL) 
        {
            RetValue = RET_TPALLOC_ERR;
            goto end_credit;
        }
        len = (FLDLEN32)((output_par_num)*100);
        rcv_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(len));
        if(rcv_fb==(FBFR32 *)NULL) 
        {
            RetValue = RET_TPALLOC_ERR;
            goto end_credit;
        }
        
        /*input parms*/
        sprintf(temp_buf,"%d",input_par_num);
        Fchg32(send_fb, SEND_PARMS_NUM32, 0, temp_buf, (FLDLEN32)0);
        sprintf(temp_buf,"%d",output_par_num);
        Fchg32(send_fb, RECP_PARMS_NUM32, 0, temp_buf, (FLDLEN32)0);
        
        memset(temp_buf,0x00,sizeof(temp_buf));
        sprintf(temp_buf,"%s",InWorkNo);
        Fchg32(send_fb, GPARM32_0, 0, temp_buf, (FLDLEN32)0);

        memset(temp_buf,0x00,sizeof(temp_buf));
        sprintf(temp_buf,"%s",InOrgCode);
        Fchg32(send_fb, GPARM32_1, 0, temp_buf, (FLDLEN32)0);

        memset(temp_buf,0x00,sizeof(temp_buf));
        sprintf(temp_buf,"%s",InOpCode);
        Fchg32(send_fb, GPARM32_2, 0, temp_buf, (FLDLEN32)0);

        Fchg32(send_fb, GPARM32_3, 0, vDate, (FLDLEN32)0);
        Fchg32(send_fb, GPARM32_4, 0, "01", (FLDLEN32)0);
        Fchg32(send_fb, GPARM32_5, 0, vPhoneNo, (FLDLEN32)0);
        printf("第【%d】笔------------------------------------------------------------ \n",num);
        for(i = 0; i < input_par_num; i ++)
        {
            Fget32(send_fb,GPARM32_0+i,0,temp_buf,NULL);
            printf("\t服务【%s】输入参数[%d]=[%s]\n", svcName,i, temp_buf);
        }
        
         /* call service*/
         rcvLen = Fsizeof32(rcv_fb);
         RetCall = tpcall(svcName, (char *)send_fb, (long)0L, (char **)&rcv_fb, &rcvLen, (long)0L);
 
        if(RetCall == -1){
             printf("\tCan't send request to service %s\n",svcName);
             printf("\tTperrno = %d\n", tperrno);
             RetValue = RET_TPCALL_ERR;
             goto end_credit;
        }
        for(i = 0; i < output_par_num; i ++)
        {
             Fget32(rcv_fb,GPARM32_0+i,0,temp_buf,NULL);
             printf("\t服务【%s】返回参数[%d]=[%s]\n", svcName,i, temp_buf);
        }
        sleep(3);
        
        printf("\n");
        /* get the return msg*/
        Sinitn(RetCode);
        Fget32(rcv_fb,GPARM32_0,0,RetCode,NULL);
        Sinitn(RetMsg);
        Fget32(rcv_fb,GPARM32_1,0,RetMsg,NULL);
end_credit:
        if (NULL != send_fb)
            Ffree32(send_fb);
        if (NULL != rcv_fb)
            Ffree32(rcv_fb);
        EXEC SQL FETCH sel_cur INTO :vPhoneNo;
        if(SQLCODE == 1403){
            if(num < InNum)
            printf("交易数量不足【%d】,还需要补做【%d】笔----------------------------------\n",InNum,InNum - num);
        }
    }
    EXEC SQL CLOSE sel_cur;

    EXEC SQL COMMIT WORK RELEASE;

}
