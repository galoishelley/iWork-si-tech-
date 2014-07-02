#ifndef __ONEBOSS_DEFINE_H__
#define __ONEBOSS_DEFINE_H__

/* 报文头 */
struct MessagHead{
        int  MaxAccept;
        char OrigDomain[5];
        char HomeDomain[4];
        char BIPCode[8];
        char BIPVer[4];
        char ActivityCode[8];
        char ActionCode[1];
        char RouteType[2];
        char RouteValue[20];
        char ProcID[30];
        char TransIDO[30];
        char TransIDH[30];
        char ProcessTime[14];
        char RspType[1];
        char RspCode[4];
        char RspDesc[30];
        char TransIDC[60];
        char CutOffDay[8];
        char OSNDUNS[4];
        char HSNDUNS[4];
        char ConvID[60];
        char TestFlag[1];
        char MsgSender[4];
        char MsgReceiver[4];
        char SvcConVer[4];   
        char SvcCont[30];
        char OpTime[14];
};

/* 跨省操作记录 */
struct  SpanProvOprMsg { 
        char  op_type[2];      
        int   id_no;
        char  total_date[9]; 
        int   login_accept; 
        char  sm_code[3]; 
        char  belong_code[8];  
        char  org_code[10]; 
        char  login_no[7];     
        char  op_code[5];
        char  op_time[15]; 
        float pay_money; 
        float hand_fee; 
        float sim_fee; 
        float other_fee;
        char  back_flag[2];    
        char  system_note[61];  
        char  note[61]; 
};

/* 跨省交费记录 */ 
struct  SpanProvPayMsg { 
        int   id_no;
        char  phone_no[16];
        int   maxaccept;
        int   max_accept;
        char  op_code[5];
        char  login_no[7];     
        char  belong_code[8];  
        char  org_code[10]; 
        char  pay_type[2];
        char  pay_flag[2];
        char  back_flag[2];
        float pay_money; 
        float pay_prepay; 
        float payed_owe; 
        float delay_fee; 
        float dead_fee; 
        float bad_fee; 
        float other_fee;
        char  encrypt_fee[16];    
        char  pay_note[61];  
        char  op_time[15]; 
};
#endif /* __ONEBOSS_DEFINE_H__ */
