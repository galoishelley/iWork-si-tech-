#define 	BIZ_OPR_REGIST		 "01" 	/* �û�ע�� */
#define 	BIZ_OPR_EXIT		   "02"	/* �û�ע�� */
#define 	BIZ_OPR_CHGPWD		 "03"	/* �����޸� */
#define 	BIZ_OPR_SUSPEND		 "04"	/* ҵ����ͣ */
#define 	BIZ_OPR_RESUME		 "05"	/* ҵ��ָ� */
#define 	BIZ_OPR_DEST		   "06" 	/* ���񶨹� */
#define 	BIZ_OPR_UNDEST		 "07"	/* ���񶩹�ȡ�� */
#define 	BIZ_OPR_CHGDAT		 "08"	/* ���ϱ�� */
#define 	BIZ_OPR_PWDRESET   "09"	/* �������� */
#define		BIZ_OPR_PRECANCLE	 "17"	/* Ԥ�˶� */
#define 	BIZ_OPR_UNDESTALL	 "99"	/* ȫҵ���˶� */
#define   BOSS_OPRSOURCE     "08"    /* ��������BOSSĬ�ϱ���*/
#define   LOCALCODE      		 0   /* ���ط����־ */
#define   SYNCFLAG           1   /* ͬ�����ñ�־ */
#define   DISSYNCFLAG        0   /* �첽���ñ�־ */

#define 	BIZ_OPR_11		 "11"   /* 11-���� */
#define 	BIZ_OPR_12		 "12"   /* 12-��������ע�� */
#define 	BIZ_OPR_13		 "13"   /* 13-�������ֱ�� */
#define 	BIZ_OPR_14		 "14"   /* 14-�û�������ͣ */   
#define 	BIZ_OPR_15		 "15"   /* 15-�û������ָ� */ 
#define 	BIZ_OPR_21		 "21"   /* 21-����ȷ��,21-������ͣ,��ý�����ʹ��   */         
#define 	BIZ_OPR_22		 "22"   /* 22-�����ָ�  ,��ý����� */         
#define 	BIZ_OPR_89		 "89"   /* 89-SPȫҵ���˶�  */ 
#define 	BIZ_OPR_99		 "99"   /* 99-DSMPȫҵ���˶� */
#define 	BIZ_OPR_80		 "80"   /* 80-�û�ҵ�񿪹�  */ 
#define 	BIZ_OPR_90		 "90"   /* 90-�ƶ���Ϣ�����ܿ��� */
#define 	BIZ_OPR_25		 "25"   /* 28|��Ϸҵ��25 �C �㲥 */
#define 	COMB_SYNCFLAG    "1"    /* DSMP������ͬ�����첽���� 0���첽���� */
/*
�ṹ���ƣ�SORDERREQMSG
��дʱ�䣺2007/05/15
������������������ҵ�񶩹���Ϣ
*/
typedef struct SORDERREQMSG{
		long  lLoginAccept;         /* ������ˮ */
    char  vOp_Code[4+1];        /* �������� */
    char  vLogin_No[6+1];       /* ����Ա���� */
    char  vLogin_Pass[16+1];    /* ����Ա���� */
    char  vOrg_Code[9+1];       /* �������� */
    char  vPhoneType[2+1];      /* ��ʶ���� */
    char  vPhone_No[15+1];      /* ��ʶ���� */
    char  vBusi_Type[2+1];      /* ҵ������ */
    char  vOp_Type[2+1];        /* �������� */
    char  vPasswd[8+1];         /* ҵ������ */
    char  vSp_Code[12+1];       /* ��ҵ���� */
    char  vSp_Bizcode[21+1];    /* ҵ����� */
    char  vOp_Time[20+1];       /* ҵ����ʱ�� */
    char  vUserStatus[2+1];     /* �û�״̬ */
    char  vStatusChgTime[20+1]; /* �û�״̬���ʱ�� */
    char  vImsi[20+1];          /* BlackBerry�û�IMSI�� */
    char  vServType[2+1];       /* BlackBerryҵ�����ͣ�01- ���ſͻ�ҵ��02- ���ſͻ���չҵ��*/
    char  vProvinceCode[3+1];   /* ʡ�ݴ��� */
    char  vInfoCode[100+1];     /* ������Ϣ���� */
    char  vInfoValue[100+1];    /* ������Ϣֵ */
    char  vOp_Note[60+1];       /* ������ע */
    char  vTotal_Date[8+1];     /* �������� */
    char  vDateChar[6+1];       /* �������� */
    char  vSource[2+1];         /* ������Դ */
    char  vLocalCode[1+1];      /* ���ر�־,0:BOSS��������,1:ҵ��ƽ̨�������� */
    char  vSyncFlag[1+1];       /* ͬ����־,0:�첽���ã�1��ͬ������ */
    char  vRemark[512+1];       /* ������ע���������־��ֲ�������ֵ����Ա����~��ֵҵ�����~��Чʱ��~�û�Ʒ��~�Ƿ�����û� */
    char  vReturnCode[6+1];     /* ���ش��� 0���ɹ�������ʧ�� */
    char  vReturnMsg[128+1];    /* ������Ϣ */
}SORDERREQMSG;

/*
�ṹ���ƣ�DSMPORDEROpr
��дʱ�䣺2008/01/15
����������DSMP��Ʒ����������Ϣ
*/
typedef struct DSMPORDEROpr
{
	long   order_id;        
	char   id_no                [14+1];  
	char   phone_no             [15+1];  
	char   serv_code            [2+1];   
	char   opr_code             [2+1];   
	char   third_id_no          [14+1];  
	char   third_phoneno        [15+1];  
	char   spid                 [20+1];  
	char   bizcode              [21+1];  
	char   prod_code            [21+1];  
	char   comb_flag            [1+1];   
	char   mode_code            [8+1];   
	char   chg_flag             [1+1];   
	char   chrg_type            [1+1];   
	char   channel_id           [6+1];   
	char   opr_source           [2+1];   
	char   first_order_date     [17+1];  
	char   first_login_no       [6+1];   
	char   last_order_date      [17+1];  
	char   opr_time             [17+1];  
	char   eff_time             [17+1];  
	char   end_time             [17+1];  
	char   local_code           [1+1];   
	char   valid_flag           [1+1];   
	char   reserved             [100+1]; 
	char   oprnumb              [22+1];  
	char   login_no             [6+1];   
	char   op_code              [6+1];   
	char   belong_code          [8+1];   
	char   total_date           [8+1];   
	char   login_accept         [14+1];  
	char   sys_note             [100+1]; 
	char   op_note              [100+1]; 
    char   sm_code              [2+1];
    char   password             [32+1];
    char   infocode             [1001];
    char   infovalue            [1001];
    char   old_password         [32+1]; 
	char   send_spid            [20+1];  
	char   send_bizcode         [21+1]; 
	char   sync_flag            [1+1];   /* 1��ͬ��  0���첽���� */ 
	char   multi_order_flag     [1+1];   /** ���������Ĵ����־  1������  0����ʾ����������02��99��89�Ȳ�������ͬ���ӿڱ���Ϣ **/
	char   send_eff_time        [17+1];  /* ��������Чʱ�䣬���͵�ƽ̨��ʱ��*/
	char   msg_oprcode          [2+1];   /** ���Ų�����ʾ����� ����ȷ�ϣ�RC  **/
	/*add by shijing begin*/
	char   para_num          [12+1];
	char   para_name         [10+1];
	char   para_value        [256+1];
	/*add by shijing end*/
}DSMPORDEROpr ;

#ifdef _extern_
extern int procOrderReq(SORDERREQMSG *sOrderReqMsg);
#endif

