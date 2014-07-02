/***************************************
 *function  ����ȯ���������Ƶ���ؽӿ�
 *author    chendx
 *created   20130206
****************************************/

#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"


#include <sys/timeb.h>
#include <time.h>

#define ROWLEN 2048
#define _DEBUG_

EXEC SQL INCLUDE SQLCA;


/*************����ȯ���Žӿ�**************
  MCODE		������	    101810
  MID		ҵ����ˮ��	ÿ�ʽ��ױ���Ψһ
  DATE		��������	YYYYMMDD��������
  TIME		����ʱ��	hhmmss����ʱ��
  ACT_ID	���κ�	    ����������ȯ��Ļ��
  MBL_NO	�ֻ���	
  BON_AMT	����ȯ���	�Է�Ϊ��λ
  MLOG_NO	����ȯ������
  TCNL_TYP	ʡ����	    ��BOSS�Ļ�����BOSS
  OPTYP	    ������ʶ	0:������1:���� ȱʡֵΪ0
  TTXN_DT	����	    YYYYMMDD
  MERID	    �̻���	    BOSS
  SIGN	    ǩ��
  *****************************/
int pTicketSnd(long LoginAccept,char *RequestDate,char *RequestTime,char *ActId,
               char *PhoneNo,char *Cash,char *SendDate,char *SendTime,int SourceFlag,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		int iCount = 0;
		char vPhoneNo[15+1];
		long vLoginAccept = 0;
		char vActId[50+1];		/*Ӫ���id*/
		char vCash[50+1];		/*���,�Է�Ϊ��λ*/
		char vRequestDate[8+1];	/*����ȯ������������*/
		char vRequestTime[6+1];	/*����ȯ��������ʱ��*/
		char vSendDate[8+1];	/*����ʱ�� YYYYMMDD*/
		char vSendTime[6+1];	/*hh24miss*/
		
		char vTemp[512];
		char vSign[172+1];		/*ǩ����Ϣ*/
		char vSendMsg[1024];	/*������Ϣ*/
		char retemp[256]; 		/*������Ϣ*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
    	int  vStatus = 0;		/*����ȯ��ǰ״̬ 0δ���� 1�ѷ��� 2�ѳ���*/
	
		char vNewAddress[50];
		char vBunchNo[15+1];	/*���ӿ�����*/
		char vIdNo[14+1];
		long LoginAcceptTmp = 0;/*�����޸��������Ӿ����ˮ*/
		char update_sql[1000];
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vActId);
	init(vCash);
	init(vRequestDate);
	init(vRequestTime);
	init(vSendDate);
	init(vSendTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	init(vBunchNo);
	init(vIdNo);
	init(update_sql);
	memset(v_parameter_array, 0, DLMAXITEMS*DLINTERFACEDATA);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vCash,Cash);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	strcpy(vActId,ActId);
	strcpy(vSendDate,SendDate);
	strcpy(vSendTime,SendTime);	
	
	vLoginAccept = LoginAccept;
	
	Trim(vPhoneNo);
	Trim(vCash);
	Trim(vRequestDate);
	Trim(vRequestTime);
	Trim(vActId);
	Trim(vSendDate);
	Trim(vSendTime);

	/*�ж��û��Ƿ����ֻ�֧������
	EXEC SQL select count(*) into :iCount 
		       from ddsmpordermsg
		      where phone_no =:vPhoneNo  
		        and serv_code ='54' 
		        and spid ='698000' 
		        and bizcode ='00000001'
				and sysdate between eff_time and end_time ;
	if(iCount <= 0)
	{		
		strcpy(RspCode,"999999");
		strcpy(RspMsg,"���û�δ��ͨ�ֻ�֧���˻���������������ȯ���ҵ��");
		pubLog_Debug(_FFL_, "pTicketSnd", "","%s",ReternMsg);
	}*/
	
	/*����ն��˳����Ĳ������ȯ�ѳ����ģ�����ֱ��ɾ�����ٴ���
	if(1 == SourceFlag)
	{
		iCount = 0;
		EXEC  SQL  select count(*) into :iCount
		 	         from wCmpExcTicket a,dticketsend b
		            where a.login_accept = :vLoginAccept
		              and a.login_accept = b.login_accept
		              and b.status = '3';
		 if(iCount > 0)
		 {
		 	EXEC SQL delete from wCmpExcTicket where login_accept = :vLoginAccept;
		 	return 0 ;	
		 }
	}*/
	
	
	/*����ǩ���ļ����ֶ�*/
	sprintf(vTemp,"MCODE=101810&MID=%ld&DATE=%s&TIME=%s&ACT_ID=%s&"
	              "MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%ld&TCNL_TYP=BOSS&"
				  "OPTYP=1&TTXN_DT=%s&MERID=BOSS",
					vLoginAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					vLoginAccept,
					vSendDate);
	printf("\n��ǩ����Ϣ[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(RspCode,"999999");
		strcpy(RspMsg,"ǩ������ʧ��");
		pubLog_Debug(_FFL_, "pTicketSnd", "","%s",ReternMsg);
	}
	sprintf(vSendMsg,"MCODE=101810,MID=%ld,DATE=%s,TIME=%s,ACT_ID=%s,"
	              "MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%ld,TCNL_TYP=BOSS,"
				  "OPTYP=1,TTXN_DT=%s,MERID=BOSS,SIGN=%s",
					vLoginAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					vLoginAccept,
					vSendDate,
					vSign);
	Trim(vSendMsg);
	printf("\n���Žӿ���Ϣ\n[%s]\n",vSendMsg);
	
	/*�·��ӿ�*/
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"2",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC", RspMsg, sizeof(RspMsg));
		if(strcmp(RspCode,"000000") == 0 )
		{
			vStatus = 1;/*���ųɹ���״̬��Ϊ�ѷ���*/
		}
		else
		{
			vStatus = 2;/*����ʧ�ܣ�״̬��Ϊ����ʧ��*/
		}
	}
	else
	{
		strcpy(RspCode,"999999");
		sprintf(RspMsg,"����ȯ���Žӿڴ���[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
	}
	Trim(RspCode);
	Trim(RspMsg);
	
	/*�������������ʧ�������*/
	if(0 == SourceFlag || 2 == SourceFlag)
	{
		/*��¼�������������¼���ͽ��*/
		EXEC SQL insert into dticketsend(max_accept,login_accept,id_no,phone_no,
	                                ACT_ID,PACKAGE_ID,CASH,request_time,
	                                send_date,send_time,rsp_code,rsp_msg,status)
		                     select max_accept,login_accept,id_no,phone_no,
		                            ACT_ID,PACKAGE_ID,CASH,request_time,
		                            :vSendDate,:vSendTime,:RspCode,:RspMsg,:vStatus
		                       from wticketsend
		                      where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"��¼������������![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -1;
		}
		
		/*ֻ�������������ʱ���޸�һ�����ͼ�¼*/
		if(0 == SourceFlag)
		{
			/*�޸������ͼ�¼*/
			EXEC SQL select bunch_no,to_char(b.id_no) 
			           into :vBunchNo,:vIdNo
					   from dCustPresent a,wticketsend b
					  where a.login_accept = b.max_accept
					    and b.login_accept = :vLoginAccept
					    and rownum < 2;
			if(SQLCODE!=0)
			{
				sprintf(ReternMsg,"��¼������ʷ�����![%ld]",vLoginAccept);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -2;
			}
			Trim(vBunchNo);
			Trim(vIdNo);
			
			EXEC SQL select sMaxSysAccept.nextval into :LoginAcceptTmp from dual;
				     	
			EXEC SQL insert into dCustPresentHis(id_no,phone_no,belong_code,
					fav_code,bunch_no,prepay_fee,fav_money,month_fav,month_num,
					contract_no,begin_time,end_time,fav_fee,login_accept,login_no,
					open_time,total_date,op_time,op_code,update_accept,update_time,
					update_date,update_login,update_type,update_code,channel,group_id,fav_type,ACT_ID,PACKAGE_ID)
				select id_no,phone_no,belong_code,
					fav_code,bunch_no,prepay_fee,fav_money,month_fav,month_num,
					contract_no,begin_time,end_time,fav_fee,login_accept,login_no,
					open_time,total_date,op_time,op_code,:LoginAcceptTmp,
					to_date((:vSendDate||' '||:vSendTime),'yyyymmdd hh24:mi:ss'),
					:vSendDate,'system','u','7879',channel,group_id,fav_type,ACT_ID,PACKAGE_ID
				from dCustPresent
			   where bunch_no = :vBunchNo
			   	 and rownum < 2;
			if(SQLCODE!=0)
			{
				sprintf(ReternMsg,"��¼������ʷ�����![%d][%ld]",SQLCODE,vLoginAccept);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -3;
			}
				
			strcpy(v_parameter_array[0],vCash);
			sprintf(v_parameter_array[1],"%s%s",vSendDate,vSendTime);
			strncpy(v_parameter_array[2],vSendDate,6);
			strcpy(v_parameter_array[3], vBunchNo);
	
			strcpy(update_sql, "fav_fee=fav_fee+to_number(:v1)/100,op_time=to_date(:v2,'yyyymmdd hh24:mi:ss'),	pres_total_date=to_number(:v3)");
	
			ret = 0;
			ret = OrderUpdateCustPresent(ORDERIDTYPE_USER,vIdNo, 100,"7879", LoginAcceptTmp,
					"system", "pTicketSnd", vBunchNo, vBunchNo, update_sql, "", v_parameter_array);
			if (ret != 0)
			{
				sprintf(ReternMsg,"�޸����ͼ�¼�����![%s][%d]",vPhoneNo,ret);
				pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
				return -4;
			}
		}
	}
	/*�ն��˲������*/
	if(1 == SourceFlag)
	{
		/*�޸������ͼ�¼*/
		EXEC SQL update dticketsend 
					set send_date = :vSendDate,
						send_time = :vSendTime,
						rsp_code = :RspCode,
						rsp_msg	 = :RspMsg,
						status = :vStatus
				  where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"�޸ķ�����Ϣ����![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -5;
		}		
		
		/*�޸��ն��˲����¼��*/
		EXEC SQL update wCmpExcTicket
		            set deal_flag = :vStatus,
		            	rsp_code  = :RspCode,
		            	rsp_msg	 = :RspMsg
		          where login_accept = :vLoginAccept;
		if(0 != SQLCODE)
		{
			sprintf(ReternMsg,"�޸��ն��˲����¼�����ʶ����![%ld]",vLoginAccept);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -6;
		}	
	}
	
	
	return 0;
}

/*************����������ȯ�ӿ�************
  MCODE		������	    101810
  MID		ҵ����ˮ��	ÿ�ʽ��ױ���Ψһ
  DATE		��������	YYYYMMDD��������
  TIME		����ʱ��	hhmmss����ʱ��
  ACT_ID	���κ�	    ����������ȯ��Ļ��
  MBL_NO	�ֻ���	
  BON_AMT	����ȯ���	�Է�Ϊ��λ
  MLOG_NO	����ȯ������
  TTXN_DT	����ȯ��������	YYYYMMDD
  MERID	    �̻���	    ����ʹ��888000730000009
  SIGN	    ǩ��
****************************************/	
int pTicketCancelSnd(long vTicketAccept,long OldLoginAccept,char *RequestDate,char *RequestTime,char *ActId,
               char *PhoneNo,char *Cash,char *OldSendDate,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		int iCount = 0;
		char vPhoneNo[15+1];
		char vActId[50+1];		/*Ӫ���id*/
		char vCash[50+1];		/*���,�Է�Ϊ��λ*/
		char vRequestDate[8+1];	/*����ȯ������������*/
		char vRequestTime[6+1];	/*����ȯ��������ʱ��*/
		char vOldSendDate[8+1];	/*����ʱ�� YYYYMMDD*/
		
		char vTemp[512];
		char vSign[172+1];		/*ǩ����Ϣ*/
		char vSendMsg[1024];	/*������Ϣ*/
		char retemp[256]; 		/*������Ϣ*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vActId);
	init(vCash);
	init(vRequestDate);
	init(vRequestTime);
	init(vOldSendDate);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	strcpy(ReternMsg,"\0");
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vCash,Cash);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	strcpy(vActId,ActId);
	strcpy(vOldSendDate,OldSendDate);
	
	Trim(vPhoneNo);
	Trim(vCash);
	Trim(vRequestDate);
	Trim(vRequestTime);
	Trim(vActId);
	
	/*
	EXEC SQL select count(*) into :iCount 
		       from ddsmpordermsg
		      where phone_no =:vPhoneNo  
		        and serv_code ='54' 
		        and spid ='698000' 
		        and bizcode ='00000001'
				and sysdate between eff_time and end_time ;
	if(iCount <= 0)
	{
		sprintf(ReternMsg,"���û�δ��ͨ�ֻ�֧���˻�������������ҵ��[%s]",vPhoneNo);
		printf("ReternMsg[%s]\n",ReternMsg);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -1;
	}*/

	sprintf(vTemp,"MCODE=101820&MID=%ld&DATE=%s&TIME=%s&ACT_ID=%s&"
	              "MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%ld&TTXN_DT=%s&MERID=888000730000009",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					OldLoginAccept,
					vOldSendDate);

	printf("\n��ǩ����Ϣ[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"ǩ������ʧ��");
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101820,MID=%ld,DATE=%s,TIME=%s,ACT_ID=%s,"
	              "MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%ld,TTXN_DT=%s,"
	              "MERID=888000730000009,SIGN=%s",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vActId,
					vPhoneNo,
					vCash,
					OldLoginAccept,
					vOldSendDate,
					vSign);
	Trim(vSendMsg);
	
	printf("\n�����ӿ���Ϣ\n[%s]\n",vSendMsg);
		
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"4",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC", RspMsg, sizeof(RspMsg));
		/*P31A34Ϊ�ѳ����������ظ�����,SC6001Ϊϵͳæ�����Ժ�����*/
		if(strcmp(RspCode,"000000") != 0 &&strcmp(RspCode,"SC6001") != 0 && strcmp(RspCode,"P31A34") != 0)
		/*if(strcmp(RspCode,"000000") != 0)*/
		{
			sprintf(ReternMsg,"����ȯ�����ӿڷ���ʧ��[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
			return -3;
		}
	}
	else
	{
		sprintf(ReternMsg,"������ȯ�����ӿڴ���[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketSnd", "", "%s",ReternMsg);
		return -4;
	}

	/*�޸�ԭ���׵���ȯ״: 0δ���� 1�ѷ��� 2����ʧ�� 3�ѳ���*/
	EXEC SQL update dticketsend set status = 3
	          where login_accept = :OldLoginAccept;
	if(0 != SQLCODE)
	{
		sprintf(ReternMsg,"�޸�����״̬����[%d][%ld]",SQLCODE,OldLoginAccept);
		EXEC SQL ROLLBACK;
		return -5;
	}
	          	
	/*��¼������¼�� ʵʱ�ӿ� ����ʱ��=����ʱ��*/
	EXEC SQL insert into wticketCancel(max_accept,oldlogin_accept,login_accept,phone_no,Cancel_time)
	                     select max_accept,login_accept,:vTicketAccept,phone_no,:vRequestDate||:vRequestTime
	                       from dticketsend
	                      where login_accept = :OldLoginAccept;
	if(0 != SQLCODE)
	{
		sprintf(ReternMsg,"��¼������¼�����[%ld][%d]",OldLoginAccept,SQLCODE);
		printf("��¼������¼�����![%ld][%d]\n",OldLoginAccept,SQLCODE);
		EXEC SQL ROLLBACK;
		return -6;
	}
	return 0;
}

/****ÿ�ŵ���ȯ��������ѯ�ӿ�*****
	MCODE	������	101811
	MID		ҵ����ˮ��	ÿ�ʽ��ױ���Ψһ
	DATE	��������	YYYYMMDD
	TIME	����ʱ��	HHMMSS
	MERID	�̻����	�̻�����ʱ��ʡƽ̨�·� ����ʹ��888000730000009
	ACT_ID	����	
	MBL_NO	�ֻ�����	
	BON_AMT	����ȯ���	��λ����
	MLOG_NO	���Ŷ�����	16λ	
	TTXN_DT	��������	
	SIGN	ǩ��	172λ
******************************/	
int pTicketQrySnd(char *ActId,char *PhoneNo,char *Cash,char *OldLoginAccept,char *SendDate,char *TicketName,char *Balance,char *EffDate,char *ExpDate,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		long vTicketAccept = 0;	/*������ˮ*/
		char vPhoneNo[15+1];	/*�ֻ�����*/
		char vActId[50+1];		/*�id*/
		char vCash[50+1];		/*��ֵ*/
		char vOldLoginAccept[14+1];/*ԭ����ȯ������*/
		char vSendDate[8+1];	/*ԭ��������*/
		
		char vDate[8+1];		/*�������� yyyymmdd*/
		char vTime[6+1];		/*����ʱ�� hh24miss*/
		char vTemp[512];
		char vSign[172+1];
		char vSendMsg[1024];	/*������Ϣ*/
		char retemp[256]; 		/*������Ϣ*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vOldLoginAccept);
	init(vActId);
	init(vCash);
	init(vSendDate);
	init(vDate);
	init(vTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vOldLoginAccept,OldLoginAccept);
	strcpy(vActId,ActId);
	strcpy(vCash,Cash);
	strcpy(vSendDate,SendDate);
	
	Trim(vPhoneNo);
	Trim(vActId);
	Trim(vCash);
	Trim(vSendDate);
	
	EXEC SQL SELECT STICKETSYSACCEPT.nextVal INTO :vTicketAccept from dual;
	if(SQLCODE!=0 || vTicketAccept<=0)
	{
		strcpy(ReternMsg,"ȡ����ȯ������ˮʧ��");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -1;
	}
	
	EXEC SQL SELECT to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss')
			   INTO :vDate,:vTime
		       from dual;
	if(SQLCODE!=0)
	{
		strcpy(ReternMsg,"��ȡϵͳʱ�����");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -2;
	}
	
	sprintf(vTemp,"MCODE=101811&MID=%ld&DATE=%s&TIME=%s&MERID=888000730000009&"
				  "ACT_ID=%s&MBL_NO=%s&BON_AMT=%s&MLOG_NO=00%s&TTXN_DT=%s", 
					vTicketAccept,
					vDate,
					vTime,
					vActId,	
					vPhoneNo,
					vCash,
					vOldLoginAccept,
					vSendDate);

	printf("\n��ǩ����Ϣ[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"ǩ������ʧ��");
		pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101811,MID=%ld,DATE=%s,TIME=%s,MERID=888000730000009,"
				  "ACT_ID=%s,MBL_NO=%s,BON_AMT=%s,MLOG_NO=00%s,TTXN_DT=%s,SIGN=%s", 
					vTicketAccept,
					vDate,
					vTime,
					vActId,	
					vPhoneNo,
					vCash,
					vOldLoginAccept,
					vSendDate,
					vSign);
	Trim(vSendMsg);
	
	printf("\n������ѯ���ӿ���Ϣ\n[%s]\n",vSendMsg);
	
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"5",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, 	sizeof(RspCode));
		mml_getvalue(retemp,"DESC",  RspMsg, 	sizeof(RspMsg));
		mml_getvalue(retemp,"BON_NM",TicketName,sizeof(TicketName));/*����ȯ����*/		
		mml_getvalue(retemp,"CUR_AC_BAL",Balance,sizeof(Balance));	/*����ȯ���*/		
		mml_getvalue(retemp,"EFF_DT",EffDate, 	sizeof(EffDate));	/*����ȯ��Чʱ��*/		
		mml_getvalue(retemp,"EXP_DT",ExpDate, 	sizeof(ExpDate));	/*����ȯʧЧʱ��*/		
		if(strcmp(RspCode,"000000") != 0)
		{
			sprintf(ReternMsg,"����ȯ���ӿڷ��ش���[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketQrySnd", "", "%s",ReternMsg);
			return -3;	
		}
		Trim(TicketName);
		Trim(Balance);
		Trim(EffDate);
		Trim(ExpDate);
		printf("\nTicketName[%s],Balance[%s],EffDate[%s],ExpDate[%s]\n\n",TicketName,Balance,EffDate,ExpDate);
	}
	else
	{
		sprintf(ReternMsg,"��ƽ̨�ӿڴ���[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketQrySnd",  "","%s",ReternMsg);
		return -4;	
	}
	
	return 0;
}
/****��ѯ�ֻ�������õ���ȯ�����*****
  MCODE		������	    101760
  MID		ҵ����ˮ��	ÿ�ʽ��ױ���Ψһ
  DATE		��������	YYYYMMDD��������
  TIME		����ʱ��	hhmmss����ʱ��
  MBL_NO	�ֻ���	
  MERID	    �̻���	    BOSS
  SIGN	    ǩ��
******************************/	
int pTicketQryAllSnd(char *RequestDate,char *RequestTime,char *PhoneNo,char *Balance,char *ReternMsg)
{
	int ret = 0;
	EXEC SQL BEGIN DECLARE SECTION;
		long vTicketAccept = 0;	/*������ˮ*/
		char vPhoneNo[15+1];
		char vRequestDate[8+1];	/*����ȯ������������*/
		char vRequestTime[6+1];	/*����ȯ��������ʱ��*/
		
		char vTemp[512];
		char vSign[172+1];
		char vSendMsg[1024];	/*������Ϣ*/
		char retemp[256]; 		/*������Ϣ*/
		
		char RspCode[6+1];
    	char RspMsg[50+1];
	
		char vNewAddress[50];
	EXEC SQL END DECLARE SECTION;
	init(vPhoneNo);
	init(vRequestDate);
	init(vRequestTime);
	init(vTemp);
	init(vSign);
	init(vSendMsg);
	init(retemp);
	init(vNewAddress);
	init(RspCode);
	init(RspMsg);
	
	sprintf(vNewAddress,"ADDRESS=%s",getenv("ADDRESS"));

	strcpy(vPhoneNo,PhoneNo);
	strcpy(vRequestDate,RequestDate);
	strcpy(vRequestTime,RequestTime);
	
	Trim(vPhoneNo);
	Trim(vRequestDate);
	Trim(vRequestTime);
	
	EXEC SQL SELECT STICKETSYSACCEPT.nextVal INTO :vTicketAccept from dual;
	if(SQLCODE!=0 || vTicketAccept<=0)
	{
		strcpy(ReternMsg,"ȡ����ȯ������ˮʧ��");
		pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
		return -1;
	}
	sprintf(vTemp,"MCODE=101760&MID=%ld&DATE=%s&TIME=%s&MERID=888000730000009&MOBILEID=%s", 
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vPhoneNo);

	printf("\n��ǩ����Ϣ[%s]\n",vTemp);
	ret = 0;
	ret = func_sign(vTemp,vSign);
	if(ret < 0)
	{
		strcpy(ReternMsg,"ǩ������ʧ��");
		pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
		return -2;
	}
	sprintf(vSendMsg,"MCODE=101760,MID=%ld,DATE=%s,TIME=%s,"
	               "MERID=888000730000009,MOBILEID=%s,SIGN=%s",
					vTicketAccept,
					vRequestDate,
					vRequestTime,
					vPhoneNo,
					vSign);
	Trim(vSendMsg);
	
	printf("\n�����ֻ������ѯ�ܶ�ӿ���Ϣ\n[%s]\n",vSendMsg);
	
	ret = 0;    
	ret = LG_Getinter(vNewAddress,600018,1,"3",vSendMsg,retemp);
	printf("\n\n");
	if(ret == 0)
	{
		mml_getvalue(retemp,"RCODE", RspCode, sizeof(RspCode));
		mml_getvalue(retemp,"DESC",  RspMsg, sizeof(RspMsg));
		mml_getvalue(retemp,"MER_RED_BAL",Balance, sizeof(Balance));
		if(strcmp(RspCode,"000000") != 0)
		{
			sprintf(ReternMsg,"����ȯ���ӿڷ��ش���[%s][%s]",RspCode,RspMsg);
			pubLog_Debug(_FFL_, "pTicketQryAllSnd", "", "%s",ReternMsg);
			return -3;	
		}
		Trim(Balance);
	}
	else
	{
		sprintf(ReternMsg,"��ƽ̨�ӿڴ���[%d]",ret);
		pubLog_Debug(_FFL_, "pTicketQryAllSnd",  "","%s",ReternMsg);
		return -4;	
	}
	
	return 0;
}


