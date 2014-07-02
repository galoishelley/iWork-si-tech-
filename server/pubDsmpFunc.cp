/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubProcOrder.h"
#include "pubShortMsg.h"

#define   MAX_MSG_LENGTH    2000
#define  _DEBUG_

EXEC SQL INCLUDE SQLCA;
char* strSplit(char* str, char* dealim, int i);
int fDsmpProdRELATIONCheck(char *id_no, char *phone_no,char *prod_code, char *retMsg);
int fDsmpGetShortMsg(DSMPORDEROpr *pOrderData,char *send_msg,char *send_msg2,char *vRslt,char *retMsg);
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr);

extern int PrepareNewShortMsg(int iModelId);
extern int SetShortMsgParameter(int iModelId,const char *iKey, const char* iValue);
extern int GenerateShortMsg(int iModelId, const char *iRecvPhoneNo, const char *iLoginNo,const char *iOpCode, long iLoginAccept, const char *iOpTime);




long fGetVgopSeq()
{
	long lSelSeq = 0;
	long lInitSql = 0;

	EXEC SQL SELECT NVL(sequence,0)
			INTO :lInitSql
		FROM tVgopPkgSeq
		WHERE to_char(sysdate,'yyyymmdd') = cutoffday;
	if((SQLCODE != 0)&&(SQLCODE != 1403))
	{
		return 1;
	}

	if(lInitSql == 0)
	{
		EXEC SQL INSERT INTO tVgopPkgSeq(cutoffday, sequence)
			SELECT to_char(sysdate,'yyyymmdd'), vgop_stat_seq.nextval FROM dual;
		if(SQLCODE == 1)
		{
			EXEC SQL SELECT NVL(sequence,0)
					INTO :lInitSql
				FROM tVgopPkgSeq
				WHERE to_char(sysdate,'yyyymmdd') = cutoffday;
			if(SQLCODE != 0)
			{
				return 1;
			}
		}
		else if(SQLCODE != 0 && SQLCODE != 1)
		{
			return 1;
		}
	}

	EXEC SQL SELECT vgop_stat_seq.nextval INTO :lSelSeq FROM dual;
	if(SQLCODE != 0)
	{
		return 1;
	}

	while( lSelSeq <= lInitSql)
	{
		lInitSql = lSelSeq;
		EXEC SQL UPDATE tVgopPkgSeq SET sequence = :lInitSql WHERE to_char(sysdate,'yyyymmdd') = cutoffday;
		if(SQLCODE != 0)
		{
			return 1;
		}

		lSelSeq++;
	}

	return (lSelSeq - lInitSql);
}


/*
* ��Ʒ�ײͰ���֤
*/
int fspiBillModechk(char * spid,char *bizcode,char *oModeCode)
{

	EXEC SQL select mode_code
	         into :oModeCode
	         from SSPidBillMOde
	         where spid=:spid
	         	and bizcode=:bizcode;
#ifdef _DEBUG_
  	pubLog_Debug(_FFL_, "fspiBillModechk", "", "---��ʼִ�в�ѯ������ϵ����[fspiBillModechk]--------------");
    pubLog_Debug(_FFL_, "fspiBillModechk", "", "spid      =[%s]", spid  );
    pubLog_Debug(_FFL_, "fspiBillModechk", "", "bizcode   =[%ld]",bizcode);
    pubLog_Debug(_FFL_, "fspiBillModechk", "", "ModeCode  =[%ld]",oModeCode);
  	pubLog_Debug(_FFL_, "fspiBillModechk", "", "---����ִ�в�ѯ������ϵ����[fspiBillModechk]--------------");
#endif
	  if(SQLCODE == 1430)
	  {
	  	return 1430;
	  }
	  else if(SQLCODE!=0)
	  {
    	pubLog_DBErr(_FFL_, "fspiBillModechk", "111000", "��ѯ��Ʒ�ײͰ���֤ʧ�ܣ�");
      return -1;
    }
    return 0;
}

/*  ���ŷ�����Ϣ��ӿڱ�
* ���������
* ���������
*   �����룬
*/
int fSendShortMsg(char *InLoginAccept,char *InPhoneNo,char * InDxOpCode,char * InLoginNo,char *pShortMsg)
{
	/**** R_JLMob_liubo_CRM_CMI_2012_0163@����ͳһ�������ɡ����͡���ص��Ż����� by zhangxina @20121114
	** EXEC SQL BEGIN DECLARE SECTION;
	** 	char TmpSqlStr[1024+1];
	** EXEC SQL   END DECLARE SECTION;
	** Sinitn(TmpSqlStr);
	** sprintf(TmpSqlStr, "INSERT INTO wCommonShortMsgSp(COMMAND_ID,login_accept, msg_level, phone_no, msg, order_code, back_code, dx_op_code, login_no, op_time ) select SMSG_SEQUENCE.nextval, '0', 1, :v2, :v3, 0, 0, :v4, :v5, sysdate from dual ");
	** EXEC SQL EXECUTE
	** 	BEGIN
	** 		EXECUTE IMMEDIATE :TmpSqlStr using  :InPhoneNo, :pShortMsg,:InDxOpCode, :InLoginNo;
	** 	END;
	** END-EXEC;
	** if ( SQL_OK != SQLCODE )
	** {
	** 	#ifdef _DEBUG_
	** 		pubLog_Debug(_FFL_, "fSendShortMsg", "", "TmpSqlStr = [%s]",TmpSqlStr);
	** 	#endif
	** 	PUBLOG_DBDEBUG("fSendShortMsg");
	** 	pubLog_DBErr(_FFL_, "fSendShortMsg", "", "");
	** 	return -2 ;
	** }
	***/

	EXEC SQL BEGIN DECLARE SECTION;
		char vOpTime[17+1];
		int smModelId = 0;
		int ret=0;
	EXEC SQL END DECLARE SECTION;
	Sinitn(vOpTime);

	EXEC SQL SELECT to_char(sysdate,'yyyymmdd hh24:mi:ss')
				INTO :vOpTime
				FROM dual;
	if( SQL_OK != SQLCODE )
	{
		PUBLOG_DBDEBUG("fSendShortMsg");
		pubLog_DBErr(_FFL_, "fSendShortMsg", "", "");
		return -2 ;
	}
	Trim(vOpTime);

	Trim(pShortMsg);
	smModelId = 1230;
	PrepareNewShortMsg(smModelId);
	SetShortMsgParameter(smModelId, UPWARDACCEPT, "");
	SetShortMsgParameter(smModelId, "MSG_CONTENT", pShortMsg);
	ret = GenerateShortMsg(smModelId, InPhoneNo, InLoginNo, "SPDX", atol(InLoginAccept), vOpTime);
	if ( ret != 0 )
	{
		PUBLOG_DBDEBUG("fSendShortMsg");
		pubLog_DBErr(_FFL_, "","-3","�������Ѷ��Ŵ���[%d] ",ret);
		return -3;
	}

	return 0;
}

/*
* ���������
*   �����ַ�������"~"�ָ�
*      ����1������Ϊҵ�����ͣ���������Ϊ�ӿ�����������˳���ޣ�
* ���������
*   �����룬������Ϣ������������
*/
int fCallInterface(char *sParamStr,char *return_code,char *return_mess,char *second_return_code)
{
	FBFR32      *transIN_TP=NULL;   /*tpcall�м����뻺����*/
	FBFR32      *transOUT_TP=NULL;  /*tpcall�м����������*/
	FLDLEN32      len;                /*�������������Ŀռ�*/
	long      ilen=0;
	long      olen=0;
	int       retCode=0;
	int           flag=0;
	int           i=0;

	int inParamNum=0; /*��λ���������������*/
	int outParamNum=0;/*��λ���������������*/
	int secondLocation=0;/*�����������ȡFML32λ�ã�*/
	EXEC SQL BEGIN DECLARE SECTION;
	/* ��������������� */
	char  tmp_buf[200+1];
	char  tBusi_Type[2+1];
	char  dynStmt[1024];        /* ��̬��ѯ��� */
	char  return_code_tp[7];    /* ���ش��� */
	char  return_mess_tp[61];   /* ������Ϣ */
	char  serviceName[20+1];    /*һ��boss�ӿڷ�����*/
	char  vParamStr[512+1];     /*������Ӧ��ϵ*/
	char  vStrTmp[512+1];       /*��ʱ����*/
	char  vParamTmp[512+1];     /*��ʱ����*/
	char  vDefault[128+1];      /*Ĭ��ֵ*/
	char inParamNumString[10+1];
	EXEC SQL END DECLARE SECTION;

	init(tmp_buf);

	init(serviceName);
	init(tBusi_Type);
	init(vParamStr);
	init(vStrTmp);
	init(vDefault);


	init(return_code_tp );
	init(return_mess_tp );

	strcpy(return_code, "000000");
	strcpy(return_mess, "����ɹ�");
	strcpy(second_return_code,"00");
	pubLog_Debug(_FFL_, "fCallInterface", "", "sParamStr=[%s]",sParamStr);
	strcpy(tBusi_Type,strSplit(sParamStr,"~",0));/*��һ�������涨������ҵ�����*/
	Trim(tBusi_Type);
	pubLog_Debug(_FFL_, "fCallInterface", "", "tBusi_Type=[%s]",tBusi_Type);

	/*��ȡ���ýӿڷ�ʽ�����ýӿ��Լ�������Ӧ��ϵ**/
	EXEC SQL SELECT SERVICENAME,nvl(PARAMSTR,' ')
	INTO :serviceName,:vParamStr
	FROM SOBBIZTYPE_dsmp WHERE oprcode=:tBusi_Type;

	if(SQLCODE!=0){
	strcpy(return_code, "192001");
	sprintf(return_mess, "��ȡҵ�����ô���[%d]",SQLCODE);
		PUBLOG_DBDEBUG("fCallInterface");
		pubLog_DBErr(_FFL_, "fCallInterface", return_code, return_mess);
	flag=1;
	goto end_ProcOrderReq;
	}

	/* ����biztype��SOBBIZTYPE_dsmp�жϵ����Ǹ�һ��boss�ӿڷ��� */
	pubLog_Debug(_FFL_, "fCallInterface", "", "ͬ�����ô���");
	len = (FLDLEN32)(FIRST_OCCS32 * 100 * 50);
	pubLog_Debug(_FFL_, "fCallInterface", "", "�����ڴ�len=[%d]",len);
	transIN_TP=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(transIN_TP == (FBFR32 *)NULL)
	{
	strcpy(return_code, "192008");
	strcpy(return_mess, "������ʱ������ʧ��");
	pubLog_Debug(_FFL_, "fCallInterface", "", "������ʱ������ʧ��!");
	flag=1;
	goto end_ProcOrderReq;
	}
	transOUT_TP=(FBFR32 *)tpalloc(FMLTYPE32,NULL,len);
	if(transOUT_TP == (FBFR32 *)NULL)
	{
	strcpy(return_code, "192008");
	strcpy(return_mess, "������ʱ������ʧ��");
	pubLog_Debug(_FFL_, "fCallInterface", "", "������ʱ������ʧ��!");
	flag=1;
	goto end_ProcOrderReq;
	}

	ilen = Fsizeof32(transIN_TP);
	olen = Fsizeof32(transOUT_TP);

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fCallInterface", "", "-------------����%s begin!-------",serviceName);
	#endif

	/*���ݶ�Ӧ��ϵ������������*/
	/*��Ӧ��ϵ�����봮λ��=���񻺳���λ��~Ĭ��ֵ���磺1=2|aaaagh~2=4|spid*/
	Trim(serviceName);
	Trim(vParamStr);

	pubLog_Debug(_FFL_, "fCallInterface", "", "���÷�����=[%s]",serviceName);
	pubLog_Debug(_FFL_, "fCallInterface", "", "�������ù�ϵ=[%s]",vParamStr);
	memset(inParamNumString,0,sizeof(inParamNumString));
	strcpy(inParamNumString,strSplit(vParamStr,"~",0));
	Trim(inParamNumString);
	inParamNum=atoi(inParamNumString); /*��λ���������������*/
	outParamNum=atoi(strSplit(vParamStr,"~",1));/*��λ���������������*/
	secondLocation=atoi(strSplit(vParamStr,"~",2));/*�����������ȡFML32λ�ã�*/

	pubLog_Debug(_FFL_, "fCallInterface", "", "�����������=[%d],�����������=[%d],��������λ��=[%d]",inParamNum,outParamNum,secondLocation);

	init(tmp_buf);
	sprintf(tmp_buf,"%d",inParamNum);
	Fchg32(transIN_TP,SEND_PARMS_NUM32,0,tmp_buf,(FLDLEN32)0);          /*�����������*/
	init(tmp_buf);
	sprintf(tmp_buf,"%d",outParamNum);
	Fchg32(transIN_TP,RECP_PARMS_NUM32,0,tmp_buf,(FLDLEN32)0);          /*�����������*/

	for(i=3;i<inParamNum+3;i++)
	{/*ѭ������ÿ������*/
	init(vStrTmp);
	strcpy(vStrTmp,strSplit(vParamStr,"~",i));            /*ÿһ��~�ָ������Ϊ1�Բ�����Ӧ��ϵ����ʽ��1=2|aaaagh*/

	#ifdef _DEBUG_
	  pubLog_Debug(_FFL_, "fCallInterface", "", "��[%d]�Բ�����Ӧ��ϵΪ[%s]",i-2,vStrTmp);
	#endif

	if(strlen(vStrTmp)>0){
		int objSeq=atoi(strSplit(vStrTmp,"=",0));              /*��=��ǰ�ĵ�һ������ΪFML������λ��*/
		init(vParamTmp);
		strcpy(vParamTmp,strSplit(vStrTmp,"=",1));        /*��=���������Ϊ��������ڲ�����λ��|Ĭ��ֵ*/
		int origSeq=atoi(strSplit(vParamTmp,"|",0));              /*��|��ǰ������Ϊ��������ڲ�����λ��*/
		init(vDefault);
		strcpy(vDefault,strSplit(vParamTmp,"|",1));       /*��|��ǰ������Ϊ��ǰλ�õ�Ĭ��ֵ*/
		init(vParamTmp);
		strcpy(vParamTmp,strSplit(sParamStr,"~",origSeq));/*ȡ���봮��ָ��λ�õ�����*/

		#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fCallInterface", "", "����[%s]�ĵ�[%d]��λ��ѹ��ֵ�����봮[%s]�е�[%d]��λ�õ�ȡֵ[%s],Ĭ��ֵΪ[%s]",serviceName,objSeq,sParamStr,origSeq,vParamTmp,vDefault);
		#endif

		Trim(vParamTmp);
		if(strlen(vParamTmp)<=0){
		sprintf(vParamTmp,"%s",vDefault);                   /*�������Ϊ�գ�����дĬ��ֵ*/
		}
		Fchg32(transIN_TP,GPARM32_0+objSeq,0,vParamTmp,(FLDLEN32)0);/*��ȡ�õ��������ֵѹ���Ӧλ��*/
	}
	else{
	  strcpy(return_code, "192008");
	  strcpy(return_mess, "�������ô���");
	  pubLog_Debug(_FFL_, "fCallInterface", "", "�������ô���!");
	  flag=1;
	  goto end_ProcOrderReq;
	}
	}

	retCode = tpcall(serviceName,(char *)transIN_TP,(long) &ilen,(char **)&transOUT_TP,(long *)&olen,0);

	if (retCode != 0) {
	sprintf(return_code,"%06d", retCode);
	sprintf(return_mess, "����%s����!",serviceName);
	pubLog_Debug(_FFL_, "fCallInterface", "", "����%s����:tperrno=[%d]%s",serviceName,tperrno,tpstrerror(tperrno));
	pubLog_AppErr(_FFL_, "fCallInterface", return_code, return_mess);
	flag=1;
	goto end_ProcOrderReq;
	}

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fCallInterface", "", "-------------����%s end!-------",serviceName);
	pubLog_Debug(_FFL_, "fCallInterface", "", "tperrno=[%d]%s",tperrno,tpstrerror(tperrno));
	#endif

	Fget32(transOUT_TP,SVC_ERR_NO32,0,return_code_tp,(FLDLEN32)0);
	Fget32(transOUT_TP,SVC_ERR_MSG32,0,return_mess_tp,(FLDLEN32)0);
	if(secondLocation>=0){/*��Щҵ��û�ж���������*/
	Fget32(transOUT_TP,GPARM32_0+secondLocation,0,second_return_code,(FLDLEN32)0);
	}

	pubLog_Debug(_FFL_, "fCallInterface", "", "����[%s]�ķ�����=[%s],������Ϣ=[%s]",serviceName,return_code_tp,return_mess_tp);
	if (atol(return_code_tp) != 0)
	{
		Trim(return_code_tp);
		Trim(return_mess_tp);
		strcpy(return_code, return_code_tp);
		strcpy(return_mess, return_mess_tp);
		flag=1;
		Trim(return_code);
		Trim(return_mess);
		pubLog_AppErr(_FFL_, serviceName, return_code, return_mess);
		goto end_ProcOrderReq;
	}

end_ProcOrderReq:
	tpfree((char *)transIN_TP);
	tpfree((char *)transOUT_TP);
	return flag;
}


/**
 *  strSplit ʵ�ֶ��ַ������
 *
 */
char* strSplit(char* str, char* dealim, int i)
{
    char tmp[100][256];
    char tmpstr[1000];
    char tmpstr1[1000];
    char vdealim[10];
    char *p;
    char* q;
    int j;

    init(tmpstr);
    init(tmpstr1);
    init(vdealim);
    strcpy(vdealim,dealim);
    if ( i >= 100 ) return "";
    for( j=0;j<100;j++)
    {
            strcpy(tmp[j],"");
    }

    strcpy(tmpstr,str);
    /*printf("2tmpstr[%s]",tmpstr);*/
    j=0;
    /*printf("len[%d]",strlen(tmpstr));*/
    while(strlen(tmpstr)>0)
        {
    if(tmpstr[0]==vdealim[0])
    {
      strcpy(tmp[j],"");
      /*printf("ccc");*/
      strncpy(tmpstr,tmpstr+1,strlen(tmpstr));
      /*printf("dd tmpstr[%s]",tmpstr);*/
    }
    else
    {
      strcpy(tmpstr1,tmpstr);
      p=strtok(tmpstr1,vdealim);
      strcpy(tmp[j],p);
      /*printf("tmp[%d][%s],tmplen[%d]",i,tmpstr,strlen(tmp[j]));*/

      strncpy(tmpstr,tmpstr+strlen(tmp[j])+1,strlen(tmpstr));
      /*printf("ee tmpstr[%s]",tmpstr);*/
    }
    j++;
        }

        return tmp[i];
}

/** DSMP��Ʒ������Ϣ����**/
int fPubDsmpOrderMsg(DSMPORDEROpr *pOrderData,char *opr_flag,char *retMsg)
{
  int i = 0;
  int retCode = 0;
  EXEC SQL BEGIN DECLARE SECTION;
      char dynStmt[1024];
      long llogin_accept =0  ;
      char vthird_no[11 +1];
      char vopr_flag[1 +1 ];    /**1:����  0��ȡ���� ��ͣ���ָ�  **/
      char vid_no[14 +1];
      char vcontent[1000];
      char vcontent2[1000];

      int  vthirdno_num = 0;
      char temp_begin_time[17+1];
      char temp_end_time[17+1];
      char vprod_code[21+1];
      char orderpoint[1+1];
      char ordersync[1+1];
      char ordersynctable[30+1];
      char sTmpTime[5][14 + 1];
      int  iCount = 0;
      int  first_order_limit = 0;
      char temp_first_time[17+1];
      char temp_first_login[8+1];
      char temp_spId[20+1];
      char temp_spBizCode[21+1];
      char temp_op_accept[14+1];
      char vphone_no[15+1];
      char vthird_idno[14+1];
      char vspid[20+1];
      char vbizcode[21+1];
      char vserv_code[2+1];
      char feeFlag[1+1];
      char temp_reconfirm_flag[1+1];
      char temp_opr_code[2+1];
      char vGroupId[10+1];

      /*add by shijing begin*/
      int fvgop=0;
      char	vfeertype[2+1];
      int		oprseq=0;
      char	oprNumb[32+1];
      char  servtype[6+1];
      char insertsql[1024];
      /*add by shijing end*/
  EXEC SQL END DECLARE SECTION;

    Sinitn(vthird_no);
    Sinitn(vopr_flag);
    Sinitn(vid_no);
    Sinitn(temp_begin_time);
    Sinitn(temp_end_time);
    Sinitn(vprod_code);
    Sinitn(orderpoint);
    Sinitn(ordersync);
    Sinitn(ordersynctable);
    Sinitn(vcontent);
    Sinitn(vcontent2);
    memset(sTmpTime, 0, sizeof(sTmpTime));
    Sinitn(temp_first_time);
    Sinitn(temp_spId  );
    Sinitn(temp_spBizCode  );
    Sinitn(temp_op_accept);
    Sinitn(vphone_no);
    Sinitn(vthird_idno);
    Sinitn(vspid);
    Sinitn(vbizcode);
    Sinitn(vserv_code);
    Sinitn(feeFlag);
    Sinitn(temp_reconfirm_flag);
    Sinitn(temp_opr_code);
    Sinitn(vGroupId);
    Sinitn(vfeertype);
    Sinitn(oprNumb);
    Sinitn(insertsql);
    Sinitn(servtype);

    strcpy(vthird_no, pOrderData->third_phoneno);
    strcpy(vopr_flag, opr_flag);
    strcpy(vid_no,  pOrderData->id_no);
    strcpy(vphone_no, pOrderData->phone_no);
    strcpy(vthird_idno, pOrderData->third_id_no);

	/*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       int ret =0;
       memset(vGroupId, 0, sizeof(vGroupId));
       ret = sGetUserGroupid(vphone_no,vGroupId);
       if(ret <0)
       {
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��ȡ�û�group_idʧ��!");
					memset(vGroupId,0,sizeof(vGroupId));
					EXEC SQL select nvl(group_no,'0') into :vGroupId from (select * from dcustmsgdead where phone_No =:vphone_no order by open_time desc )
						where rownum<=1;
					if(strlen(vGroupId)==0)
					{
						strcpy(vGroupId,"0");
					}
       }
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/

#ifdef _DEBUG_
  	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "---��ʼִ�����ɶ�����ϵ����[fPubDsmpOrderMsg]--------------");
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "00000000000000000vid_no     =[%s]", vid_no  );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->order_id        =[%ld]", pOrderData->order_id       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->id_no           =[%s]", pOrderData->id_no           );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->phone_no        =[%s]", pOrderData->phone_no        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->serv_code       =[%s]", pOrderData->serv_code       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code        =[%s]", pOrderData->opr_code        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->third_id_no,    =[%s]", pOrderData->third_id_no     );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->third_phoneno   =[%s]", pOrderData->third_phoneno   );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->spid            =[%s]", pOrderData->spid            );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->bizcode         =[%s]", pOrderData->bizcode         );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->prod_code       =[%s]", pOrderData->prod_code       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->comb_flag       =[%s]", pOrderData->comb_flag       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->mode_code       =[%s]", pOrderData->mode_code       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chg_flag        =[%s]", pOrderData->chg_flag        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chrg_type       =[%s]", pOrderData->chrg_type       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->channel_id      =[%s]", pOrderData->channel_id      );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_source      =[%s]", pOrderData->opr_source      );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->first_order_date=[%s]", pOrderData->first_order_date);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->first_login_no  =[%s]", pOrderData->first_login_no  );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->last_order_date =[%s]", pOrderData->last_order_date );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_time        =[%s]", pOrderData->opr_time        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->eff_time        =[%s]", pOrderData->eff_time        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->end_time        =[%s]", pOrderData->end_time        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->local_code      =[%s]", pOrderData->local_code      );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->valid_flag      =[%s]", pOrderData->valid_flag      );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->belong_code     =[%s]", pOrderData->belong_code     );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->total_date      =[%s]", pOrderData->total_date      );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->login_accept    =[%s]", pOrderData->login_accept    );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->reserved        =[%s]", pOrderData->reserved        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->sm_code         =[%s]",  pOrderData->sm_code        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->password        =[%s]", pOrderData->password        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->sync_flag       =[%s]", pOrderData->sync_flag       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->msg_oprcode       =[%s]", pOrderData->msg_oprcode       );


#endif


  /** �ж��Ƿ���Ҫ���͵��ӿڱ���
     SOBBIZTYPE_dsmp.ORDERPOINT IS '������ϵ���ɵ㣬0��BOSS��1��ƽ̨��Ĭ��0';
     SOBBIZTYPE_dsmp.ORDERSYNC IS '������ϵͬ����־��0����ͬ����1��BOSS-��ƽ̨��2��ƽ̨-��BOSS��Ĭ��0';
     SOBBIZTYPE_dsmp.ORDERSYNCTABLE IS '��ordersync=1�������ΪBOSSͬ�����ݵ���ʱ��Ӫҵ����ͬ�����ݣ��ӿڸ�����';
     ���ORDERPOINT=0,�����ɶ�����ϵ������״̬Ϊ��Ч������״̬ΪԤ����
     ���ORDERSYNC=1,����Ҫ��������ϵ����ORDERSYNCTABLE�ֶ���ָ�ı��У�����ֶ������õ����б�ṹ��һ�����Ա�ӿڷ���ʱ���Է�ƽ̨����
  ***/
  EXEC SQL  SELECT ORDERPOINT,ORDERSYNC,nvl(ORDERSYNCTABLE,'  ' ) ,nvl(first_order_limit,0),
                   nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ')
              into  :orderpoint,:ordersync,:ordersynctable,:first_order_limit,
                    :temp_spId,:temp_spBizCode
            FROM  SOBBIZTYPE_dsmp
       WHERE  oprcode = :pOrderData->serv_code ;
    if  (SQLCODE != 0)
    {
    	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->serv_code =[%s]", pOrderData->serv_code);
      	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", pOrderData->serv_code,SQLCODE);
      	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
      	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-50", retMsg);
      	return -50;
    }
    Trim(temp_spId);
    Trim(temp_spBizCode);
    Trim(orderpoint);

    if (strlen(temp_spId) > 0 && strcmp(pOrderData->serv_code,"27")!=0)
    {
        strcpy(pOrderData->send_spid," ");
    }
    else
    {
        strcpy(pOrderData->send_spid,pOrderData->spid);
    }

    if (strlen(temp_spBizCode) > 0 )
    {
        strcpy(pOrderData->send_bizcode," ");
    }
     else
    {
        strcpy(pOrderData->send_bizcode,pOrderData->bizcode);
    }
	printf("\npOrderData->send_spid=[%s]\n",pOrderData->send_spid);
    Trim(ordersynctable);
    if  (( strlen(ordersynctable) <= 0 )  && ( strcmp(ordersync,"1") == 0 ))
    { /*�Ա��һ��Լ��*/
			pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "ordersynctable =[%s]", ordersynctable);
      sprintf(retMsg, "SOBBIZTYPE_dsmp�е�ordersynctable�ֶβ���Ϊ��[%s][%d]!", ordersynctable,SQLCODE);
      	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
      	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-51", retMsg);
      return -51;
      /**sprintf(ordersynctable,"TDSMPOPRECORDMSG"  ); ***/
    }

    /**����ƽ̨������Ч�Ķ�����ϵ����������ͬ������ chendh 2008.11.24**/
    if(strcmp(pOrderData->opr_source,"08")!=0 && strlen(pOrderData->opr_source)>0)
	{
    	EXEC SQL select decode(count(spid),0,'0','1') flag into :temp_reconfirm_flag
    				from SDSMPRECONFIRM
    				WHERE SPID  = :pOrderData->send_spid  AND BIZCODE   =:pOrderData->send_bizcode
                   AND OPR_SOURCE  =:pOrderData->opr_source;
    }
    Trim(temp_reconfirm_flag);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "orderpoint=[%s]",orderpoint);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "vopr_flag =[%s]",vopr_flag);
        /** Ĭ��0. ��Ч1. ֻ��ҵ������ʱ�����ܸ�ֵΪ��2�� ����Ԥ����   **/
        if (strncmp(pOrderData->sync_flag,"0",1)==0)
        { /** 1��ͬ��  0���첽����;Ĭ��ͬ��**/
            strcpy(pOrderData->valid_flag ,"3");
        }
        /*0��BOSS��1��ƽ̨��Ĭ��0'*/
        else if((strcmp(orderpoint,"2")==0)&&(strcmp(vopr_flag,"1")==0))
        		strcpy(pOrderData->valid_flag ,"1");
        else if((strcmp(orderpoint,"2")==0)&&(strcmp(vopr_flag,"0")==0))
						strcpy(pOrderData->valid_flag ,"0");
        else if((strcmp(temp_reconfirm_flag,"0")==0) && (strcmp(pOrderData->opr_source,"08")!=0))
				{
					/**������ϵ������Ч**/
					strcpy(pOrderData->valid_flag ,"1");
				}
        else
        	{
            	strcpy(pOrderData->valid_flag ,"2");
        	}
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->valid_flag == [%s]",pOrderData->valid_flag);

    strcpy(vspid, pOrderData->spid);
    strcpy(vbizcode,pOrderData->bizcode);
    strcpy(vserv_code,pOrderData->serv_code);


    /** ��ȡ�û������Ĳ�Ʒ��Ϣ **/
    if (((strncmp(pOrderData->opr_code,BIZ_OPR_DEST,2) == 0) && (strcmp(pOrderData->serv_code,"41")!=0)) /** WHL add for �ƶ�΢�� at 20111221 **/
          /*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
          || (strncmp(pOrderData->opr_code,BIZ_OPR_11,2) == 0&&(strcmp(pOrderData->serv_code,"02")!=0)&&(strcmp(pOrderData->serv_code,"93")!=0)&&(strcmp(pOrderData->serv_code,"92")!=0))
          /*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/
                || (strncmp(pOrderData->opr_code,BIZ_OPR_21,2) == 0)
                || (strncmp(pOrderData->opr_code,BIZ_OPR_REGIST,2) == 0)
                || (strncmp(pOrderData->opr_code,BIZ_OPR_13,2) == 0)
                || (strcmp(pOrderData->opr_code,"10")==0)
                || ((strcmp(pOrderData->opr_code,"12")==0)&&((strcmp(pOrderData->serv_code,"02")==0)||(strcmp(pOrderData->serv_code,"92")==0)||(strcmp(pOrderData->serv_code,"93")==0)))
                || ((strcmp(pOrderData->opr_code,"01")==0)&&(strcmp(pOrderData->serv_code,"36")==0))
                || (strcmp(pOrderData->opr_code,"26")==0))
    {   /**06-���� 11-���� 21-����ȷ��,01-ע�� 10-�ײͶ���**/

    /** ��ȡ�û������Ĳ�Ʒ��Ϣ  b.acid_flag = '0' ��ԭ�Ӳ�Ʒ**/
    EXEC SQL  SELECT a.prod_code,to_char(a.begin_time,'yyyymmdd hh24:mi:ss'),to_char(a.end_time,'yyyymmdd hh24:mi:ss')
                  INTO :vprod_code,:temp_begin_time,:temp_end_time
              FROM SDSMPPRODCODE a,SDSMPPRODBIZ b
         WHERE a.PROD_CODE = b.PROD_CODE
           AND b.SPID = :pOrderData->spid
           AND b.BIZCODE = :pOrderData->bizcode AND b.acid_flag = '0'
           AND sysdate between b.BEGIN_TIME  and b.END_TIME
           AND sysdate between a.BEGIN_TIME  and a.END_TIME;
      if ((SQLCODE != 0) &&(SQLCODE != 1403 ))
      {
	        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->spid            =[%s]", pOrderData->spid            );
	        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->bizcode         =[%s]", pOrderData->bizcode         );
	        sprintf(retMsg, "��ȡDSMP��Ʒ��Ϣ��SDSMPPRODCODE����[%s][%d]!", pOrderData->spid,SQLCODE);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-4", retMsg);
        return -4;
      }
      if (SQLCODE == 1403 )
      {
        	sprintf(retMsg, "SDSMPPRODCODE��û���ҵ�SPҵ���Ӧ�Ĳ�Ʒ��Ϣ[%s][%d]!", pOrderData->spid,SQLCODE);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-51", retMsg);
        	return -5;
      }

    Trim(vprod_code);
    Trim(temp_begin_time);
    Trim(temp_end_time);
		/*** R_JLMob_liubo_CRM_CMI_2012_0233�����ڶ��Ŷ���������ȷ�����״ζ������Ѷ��ŵ��Ż����� by zhangxina begin
		**** �ж��û��״ζ��� eff_time == first_order_date �˴��޸� eff_time Ҫ�ͺ����޸� first_order_date ����һ�� ***/

    strcpy(pOrderData->prod_code,vprod_code );
    if ( (strncmp(pOrderData->comb_flag,"1",1) != 0) && (strncmp(pOrderData->op_code,"1272",4) != 0)  )
      {
      	if((strcmp(pOrderData->opr_code,"12")==0)&&((strcmp(pOrderData->serv_code,"02")==0)||(strcmp(pOrderData->serv_code,"92")==0)||(strcmp(pOrderData->serv_code,"93")==0)))
      	{
      		;/**wlan�ײͱ��ԤԼ��Ч**/
      	}
      	else
      	{
      		strcpy(pOrderData->eff_time ,pOrderData->opr_time);
      	}
      	if(strcmp(pOrderData->spid,"WLAN01") != 0
      		|| strcmp(pOrderData->bizcode,"00014") != 0
      		|| strlen(pOrderData->end_time) == 0)
      	{
        	strcpy(pOrderData->end_time ,temp_end_time  );
        }
        if(strcmp(pOrderData->opr_code,"26")==0)
        {
        	strcpy(pOrderData->end_time ,pOrderData->opr_time);
        }
      }

    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_time        =[%s]", pOrderData->opr_time        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->eff_time        =[%s]", pOrderData->eff_time        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->end_time        =[%s]", pOrderData->end_time        );

    /** �û������Ĳ�Ʒ��ϵ��� --- ��Ʒ�������ж�;��Ʒ�������ж�**/
    if (strncmp(pOrderData->opr_code,BIZ_OPR_11,2) == 0)
    {
        iCount = 0;
        iCount = fDsmpProdRELATIONCheck( vthird_idno, vthird_no,vprod_code,  retMsg);
        if (iCount != 0)
        {
            return iCount;
        }
    }
    else
    {
        iCount = 0;
        iCount = fDsmpProdRELATIONCheck(vid_no ,vphone_no ,vprod_code,  retMsg);
        if (iCount != 0)
        {
            return iCount;
        }
    }

    /** ��ȡ�û���һ�ζ�����Ʒ��ʱ�� **/
    memset(dynStmt, 0, sizeof(dynStmt));
    /*
    sprintf(dynStmt, " select NVL(to_char(min(EFF_TIME),'yyyymmdd hh24:mi:ss'),' ')  "
              " from DDSMPORDERMSG%d   "
          " where id_no=to_number(:v1) and spid = :v2 and bizcode = :v3   "
        " and  EFF_TIME >= add_months(sysdate,:v4*(-1))  and third_id_no = 0 ",  \
            atol(pOrderData->id_no )%10L  );
    */

       strcpy(temp_opr_code,pOrderData->opr_code);
       pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��ȡ�û��״ζ���ʱ�䵱ǰ����: [%s] ",temp_opr_code);
       /** chendh 2009.02.03�޸��״ζ���ʱ����ȡ����,��opr_time����Ϊmax(first_order_date) **/
       sprintf(dynStmt, "select nvl(to_char(max(first_order_date),'yyyymmdd hh24:mi:ss'),' ') "
       				" from ddsmpordermsg%ld  "
       				" where id_no = to_number(:v1) and spid = :v2 and bizcode = :v3  "
       				" and  opr_time > to_date(to_char(sysdate,'yyyymm')||'01','yyyymmdd')",atol(pOrderData->id_no )%10L);

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��ȡ�״ζ���ʱ������:");
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "vid_no: [%s]",vid_no);
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "vspid: [%s]",vspid);
		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "vbizcode: [%s]",vbizcode);
#endif

      EXEC SQL EXECUTE
      BEGIN
      	/*
        EXECUTE IMMEDIATE :dynStmt INTO :temp_first_time
                 using :vid_no ,:vspid ,:vbizcode,:first_order_limit;
         */
			EXECUTE IMMEDIATE :dynStmt INTO :temp_first_time  using :vid_no ,:vspid ,:vbizcode;
      END;
      END-EXEC;
      if ((SQLCODE != 0) &&(SQLCODE != 1403 ))
      	{
        	sprintf(retMsg, "��ȡ�û��״ζ���ʱ��ʧ��[%d]!",SQLCODE);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-45", retMsg);
        	return -45 ;
      }
			else if (SQLCODE == 1403 )
			{   /**ȱʡֵ**/
				if ( (strncmp(pOrderData->comb_flag,"1",1) != 0) && (strncmp(pOrderData->op_code,"1272",4) != 0)  )
				{
					strcpy(pOrderData->first_order_date, pOrderData->opr_time);
				}
			}
			else
			{
#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "�״ζ���ʱ�� temp_first_time=[%s]", temp_first_time);
#endif
				Trim(temp_first_time);
				if (strlen(temp_first_time) > 0)
				{
					strcpy(pOrderData->first_order_date ,temp_first_time  );
				}
				else if((strlen(temp_first_time) == 0)&&(strncmp(pOrderData->comb_flag,"1",1) != 0)&&(strncmp(pOrderData->op_code,"1272",4) != 0))
				{
					strcpy(pOrderData->first_order_date, pOrderData->opr_time);
				}
			}
			pubLog_Debug(_FFL_,"fPubDsmpOrderMsg","�����״ζ���ʱ��","pOrderData->first_order_date=[%s]",pOrderData->first_order_date);
			pubLog_Debug(_FFL_,"fPubDsmpOrderMsg","���ն�����Чʱ��","pOrderData->eff_time=[%s]",pOrderData->eff_time );
			/*** R_JLMob_liubo_CRM_CMI_2012_0233�����ڶ��Ŷ���������ȷ�����״ζ������Ѷ��ŵ��Ż����� by zhangxina end ***/

		}

  /** ֻ�ж��š�����ҵ����ܹ����� **/
  /*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
  if (strcmp(pOrderData->opr_code,BIZ_OPR_11) ==0&&(strcmp(pOrderData->serv_code,"19") !=0 )&&(strcmp(pOrderData->serv_code,"02")!=0)&&(strcmp(pOrderData->serv_code,"92")!=0)&&(strcmp(pOrderData->serv_code,"93")!=0) )
  /*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/
  	{
    if ((strcmp(pOrderData->serv_code,"04") !=0 ) && (strcmp(pOrderData->serv_code,"05") !=0 ))
    	{
      		sprintf(retMsg, "ֻ�ж��š����Ų��ܹ�ҵ������[%s]!", pOrderData->serv_code);
	      	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	        pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-6", retMsg);
     	 	return -6;
    	}

		/*��ʼ�ж����͵������Ƿ񳬹��涨*/
    EXEC SQL  SELECT THIRDNO_NUM  INTO :vthirdno_num
            FROM CDSMPTHIRDNONUM
          WHERE region_code = substr(:pOrderData->belong_code,1,2)
            AND biz_type = :pOrderData->serv_code ;
      if (SQLCODE != 0)
      {
       		sprintf(retMsg, "��ȡ��CDSMPTHIRDNONUM����[%d]!",SQLCODE);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-7", retMsg);
       		return -7;
      }

      Sinitn(dynStmt);
      i =0 ;
      sprintf(dynStmt, " select count(*) from DDSMPORDERMSG%ld  "
                     " where  id_no=to_number(:v1) and  SERV_CODE = :v2  "
                   " and sysdate between EFF_TIME and end_time and third_id_no > 0 ",  atol(pOrderData->id_no)%10L   );
    EXEC SQL EXECUTE
    BEGIN
      EXECUTE IMMEDIATE :dynStmt INTO :i using :vid_no,:vserv_code ;
    END;
    END-EXEC;
    if(SQLCODE != OK  )
    {
      sprintf(retMsg,"��ȡDDSMPORDERMSG��Ϣ����[%d]!",SQLCODE);
      	#ifdef _DEBUG_
    			pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
		#endif
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-8", retMsg);
        	return -8;
    }

    if (i > vthirdno_num)
    {
      		sprintf(retMsg, "�����˿���ҵ�����͵��û�����,���ܽ���ҵ������[%d]!", vthirdno_num);
	        pubLog_DBErr(_FFL_, "", "-9", retMsg);
      		return -9;
    }

  }

    /** 06-����   07-�˶�   11-����  14-�û�������ͣ   15-�û������ָ�    21-����ȷ��
        89-SPȫҵ���˶�     99-DSMPȫҵ���˶� **/
    /**chendh 2008.12.15 ����(����+������Ч)�����ݼƷ�**/
    if (strcmp(vopr_flag ,"1") == 0 )
    {    /** SPҵ������**/

    /** ��¼�û��Ķ�����Ϣ ****/
    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt, " INSERT INTO DDSMPORDERMSG%ld "
          "("
                      " order_id,id_no,phone_no,serv_code                 "
                      " ,opr_code,third_id_no,third_phoneno,spid          "
                      " ,bizcode,prod_code,comb_flag,mode_code            "
                      " ,chg_flag,chrg_type,channel_id,opr_source         "
                      " ,first_order_date,first_login_no,last_order_date  "
                      " ,opr_time,eff_time,end_time                       "
                      " ,local_code,valid_flag,belong_code "
                      " ,total_date,login_accept,reserved ,sm_code ,password,group_id"
          ")"
          "values"
          "("
                " :v1,to_number(:v2),:v3,:v4, "
                " :v5,to_number(:v6),NVL(:v7,' '), :v8 , "
                " :v9,:v10,:v11, NVL(:v12,' ' ), "
                " :v13,:v14,:v15, :v16 , "
              " to_date(:v17,'yyyymmdd hh24:mi:ss'),:v18,to_date(:v19,'yyyymmdd hh24:mi:ss'), "
              " to_date(:v20,'yyyymmdd hh24:mi:ss'), to_date(:v21,'yyyymmdd hh24:mi:ss'), "
              " to_date(:v22,'yyyymmdd hh24:mi:ss') ,  "
                " :v23, :v24, :v25 ,"
              " to_number(:v26) ,to_number(:v27) , nvl(:v28,' ' ),NVL(:v29,' ') ,nvl(:v30,' '),:v31 "
          ")",atol(vid_no)%10 );

      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                              :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,
                              :pOrderData->bizcode,:pOrderData->prod_code, :pOrderData->comb_flag,:pOrderData->mode_code,
                              :pOrderData->chg_flag,:pOrderData->chrg_type,:pOrderData->channel_id,:pOrderData->opr_source,
                              :pOrderData->first_order_date,:pOrderData->first_login_no,:pOrderData->last_order_date,
                              :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->end_time ,
                              :pOrderData->local_code,:pOrderData->valid_flag,:pOrderData->belong_code ,
                              :pOrderData->total_date,:pOrderData->login_accept,:pOrderData->reserved ,
                              :pOrderData->sm_code ,:pOrderData->password,:vGroupId;
    if (SQLCODE !=0)
    {
     	sprintf(retMsg, "�����DDSMPORDERMSG%ld����[%d]!", atol(vid_no)%10, SQLCODE);
    	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->order_id=[%ld],pOrderData->id_no=[%s]",pOrderData->order_id,pOrderData->id_no);
    	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->phone_no=[%s]pOrderData->serv_code=[%s]",pOrderData->phone_no,pOrderData->serv_code);
		#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
		#endif
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-20", retMsg);
      		return -20;
    }

    /** ��¼�û��Ķ�����ʷ��Ϣ ****/
    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt, "  INSERT INTO DDSMPORDERMSGHIS%ld  "
          "("
                      " order_id,id_no,phone_no,serv_code                 "
                      " ,opr_code,third_id_no,third_phoneno,spid          "
                      " ,bizcode,prod_code,comb_flag,mode_code            "
                      " ,chg_flag,chrg_type,channel_id,opr_source         "

                      " ,first_order_date,first_login_no,last_order_date  "
                      " ,opr_time,eff_time,end_time                       "
                      " ,local_code,valid_flag,belong_code                "
                      " ,total_date,login_accept,reserved                 "
                      " ,UPDATE_LOGIN,UPDATE_DATE,UPDATE_TIME             "
            " ,UPDATE_ACCEPT,UPDATE_CODE  ,UPDATE_TYPE ,sm_code,password  "
            ",GROUP_ID"

          ")"
          "values"
          "("
                " :v1,to_number(:v2),:v3,:v4, "
                " :v5,to_number(:v6),:v7, :v8 , "
                " :v9,:v10,:v11, :v12 , "
                " :v13,:v14,:v15, :v16 , "

              " to_date(:v17,'yyyymmdd hh24:mi:ss'),:v18,to_date(:v19,'yyyymmdd hh24:mi:ss'), "
              " to_date(:v20,'yyyymmdd hh24:mi:ss'), to_date(:v21,'yyyymmdd hh24:mi:ss'), "
              " to_date(:v22,'yyyymmdd hh24:mi:ss') ,  "
                " :v23, :v24, :v25 , "
              " to_number(:v26) ,to_number(:v27) , :v28,  "
              " :v29, to_number(:v30) , to_date(:v31,'yyyymmdd hh24:mi:ss'),  "
              "  to_number(:v32) , :v33 , 'a' ,:v34,:v35, "
              ":v36"
          ")",atol(vid_no)%10 );
      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                              :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,
                              :pOrderData->bizcode,:pOrderData->prod_code, :pOrderData->comb_flag,:pOrderData->mode_code,
                              :pOrderData->chg_flag,:pOrderData->chrg_type,:pOrderData->channel_id,:pOrderData->opr_source,
                              :pOrderData->first_order_date,:pOrderData->first_login_no,:pOrderData->last_order_date,
                              :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->end_time ,
                              :pOrderData->local_code,:pOrderData->valid_flag,:pOrderData->belong_code ,
                              :pOrderData->total_date,:pOrderData->login_accept,:pOrderData->reserved ,
                              :pOrderData->login_no  ,:pOrderData->total_date  ,:pOrderData->opr_time  ,
                              :pOrderData->login_accept, :pOrderData->op_code ,:pOrderData->sm_code,:pOrderData->password,
                              :vGroupId;
    if (SQLCODE !=0)
    {
      		sprintf(retMsg, "�����DDSMPORDERMSGHIS%ld����[%d]!", atol(vid_no)%10, SQLCODE);
		#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
		#endif
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-21", retMsg);
      		return -21;
    }

    	/**������������Ч+bill_flgΪ'1'+���¶��������������ݼƷ�,2008.12.15**/
    	Trim(pOrderData->chrg_type);
    	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��ʼ������Ķ�����Ϣ�����ݼƷ�pOrderData->valid_flag=[%s],pOrderData->chrg_type=[%s]",pOrderData->valid_flag,pOrderData->chrg_type);
    	/**chendh 2009.01.14�����ְ���**/
    	/*
    	if(strcmp(pOrderData->valid_flag,"1")==0)

			*/
				/**BILL_FLAG 0:�������ݼƷ� 1�������ݼƷ�**/
				EXEC SQL select BILL_FLAG into :feeFlag from sobbiztype_dsmp where oprcode=:pOrderData->serv_code;
      	if (SQLCODE !=0)
      	{
        	sprintf(retMsg, "��ѯ�Ʒѱ�Ǵ���[%d][%s]!", SQLCODE,pOrderData->serv_code);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-35", retMsg);
        	return -35;
      	}
      	Trim(feeFlag);
      	if(strcmp(feeFlag,"1")==0)
      	{
      		/*��ʼ�ͼƷ�*/
        	Trim(pOrderData->chrg_type);
        	memset(dynStmt,0,sizeof(dynStmt));
        	/*wugl add */
        	/*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
          memset(sTmpTime, 0, sizeof(sTmpTime));
          if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
          {
            sprintf(retMsg,":pOrderData->opr_time����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
            return -26;
          }
          pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", " �ͼƷѲ���ʱ�� = [%s] ",sTmpTime[0]);
					/* zhengbin 2010-12-10  update �ֻ����Ӱ������ײ͸��� BEGIN*/
					if(fspiBillModechk(pOrderData->spid,pOrderData->bizcode,pOrderData->mode_code)==0)
						{
							pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "�����ײͰ󶨴��� %s",pOrderData->mode_code);
						}
					/* zhengbin 2010-12-10  update �ֻ����Ӱ������ײ͸��� END */
						else
					 {
					sprintf(dynStmt,"insert into oboss.M_ORDER_INFO_TMP(op_no,msisdn,third_dn,serv_code,sp_code,oper_code,chrg_type,order_stat,opr_source,change_time,OP_TIME)"
                "values (dbcustadm.maxmsgaccept.NEXTVAL ,:v1,:v2,:v3,:v4,:v5,:v6,decode(:v7,'02','07','11','06','14','04','15','05','21','04','22','05','89','07','99','07',:v8),:v9,to_char(to_date(:v10,'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'),:v11)");

         	EXEC SQL PREPARE prepare2 From :dynStmt;
        	EXEC SQL EXECUTE prepare2 USING   :pOrderData->phone_no , :pOrderData->third_phoneno,:pOrderData->serv_code, :pOrderData->spid,
                                            :pOrderData->bizcode,:pOrderData->chrg_type,:pOrderData->opr_code,:pOrderData->opr_code,
                                            :pOrderData->opr_source,:pOrderData->eff_time,:sTmpTime[0] ;
        	if (SQLCODE !=0)
        	{
          		sprintf(retMsg, "����ƷѰ��±�������[%d]!", SQLCODE);
          		#ifdef _DEBUG_
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
				#endif
				PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
        		pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-34", retMsg);
          		return -34;
        	}
           }
        	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��ɽ�����Ķ�����Ϣ�����ݼƷ�pOrderData->valid_flag=[%s],pOrderData->chrg_type=[%s]pOrderData->phone_no=[%s]",pOrderData->valid_flag,pOrderData->chrg_type,pOrderData->phone_no);
      	}

      	/*add by shijing for ͬ��������ϵ��VGOP BEGIN*/

      	if((strcmp(pOrderData->serv_code,"23")==0)||(strcmp(pOrderData->serv_code,"60")==0)||(strcmp(pOrderData->serv_code,"16")==0))
      	{
      		printf("\n111111111\n");

          fvgop=fVGgopSync(pOrderData,"1",retMsg);
          if(fvgop!=0)
          {
          	printf("---insert TBOSSVGOPBIZPORTALMSGʧ��!\n");
          	return fvgop;
          }

      	}
      	/*add by shijing for ͬ��������ϵ��VGOP END*/

		}
    else
    {
    /*add by hejt begine 2008-5-9 10:34:56 ����ʡ�ڼƷ�ϵͳ*/
    /** chendh 2008-12-03 ���ӽ�������Чƽ̨������Ϣ�������ݼƷ�**/
    /* wugl delete
    	Trim(pOrderData->opr_code);
    */
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", " ������ϵ�ͼƷѷǶ����������� pOrderData->opr_code =[%s]", pOrderData->opr_code);
    /***
    if((strcmp(pOrderData->opr_code,"07")==0)||(strcmp(pOrderData->opr_code,"02")==0)||(strcmp(pOrderData->opr_code,"89")==0)||(strcmp(pOrderData->opr_code,"99")==0)||(strcmp(pOrderData->valid_flag,"1")==0 && strcmp(pOrderData->opr_source,"08")!=0))
    {
    ***/
      EXEC SQL select BILL_FLAG into :feeFlag from sobbiztype_dsmp where oprcode=:pOrderData->serv_code;
      if (SQLCODE !=0)
      {
        	sprintf(retMsg, "��ѯ�Ʒѱ�Ǵ���[%d][%s]!", SQLCODE,pOrderData->serv_code);
	     	PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	        pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-35", retMsg);
        	return -35;
      }
      Trim(feeFlag);
      Trim(pOrderData->chrg_type);
      /**chendh 2009.01.14 �����ְ���**/
      if(strcmp(feeFlag,"1")==0 )
      {
        Trim(pOrderData->chrg_type);
        memset(dynStmt,0,sizeof(dynStmt));
        /*wugl add */
        /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
        memset(sTmpTime, 0, sizeof(sTmpTime));
        if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
        {
          sprintf(retMsg,":pOrderData->opr_time����fPubFormatTimeʧ��! iRet=[%d]",i);
	        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
          return -26;
        }
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", " �ͼƷѲ���ʱ�� = [%s] ",sTmpTime[0]);
        /* wugl delete 2008.12.24
        sprintf(dynStmt,"insert into oboss.M_ORDER_INFO_TMP(op_no,msisdn,third_dn,serv_code,sp_code,oper_code,chrg_type,order_stat,opr_source,change_time,OP_TIME)"
                "values (dbcustadm.maxmsgaccept.NEXTVAL ,:v1,:v2,:v3,:v4,:v5,:v6,'07',:v7,to_char(to_date(:v8,'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'),TO_CHAR(SYSDATE,'yyyymmddhh24miss'))");
				*/
				/* zhengbin 2010-12-10  update */
				sprintf(dynStmt,"insert into oboss.M_ORDER_INFO_TMP(op_no,msisdn,third_dn,serv_code,sp_code,oper_code,chrg_type,order_stat,opr_source,change_time,OP_TIME)"
                "values (dbcustadm.maxmsgaccept.NEXTVAL ,:v1,:v2,:v3,:v4,:v5,:v6,decode(:v7,'02','07','11','06','14','04','15','05','21','04','22','05','89','07','99','07',:v8),:v9,to_char(to_date(:v10,'yyyymmdd hh24:mi:ss'),'yyyymmddhh24miss'),:v11)");
         EXEC SQL PREPARE prepare2 From :dynStmt;
         /*
         EXEC SQL EXECUTE prepare2 USING   :pOrderData->phone_no , :pOrderData->third_phoneno,:pOrderData->serv_code, :pOrderData->spid,
                                            :pOrderData->bizcode,:pOrderData->chrg_type,:pOrderData->opr_source,:pOrderData->eff_time ;
         */
				EXEC SQL EXECUTE prepare2 USING   :pOrderData->phone_no , :pOrderData->third_phoneno,:pOrderData->serv_code, :pOrderData->spid,
                                            :pOrderData->bizcode,:pOrderData->chrg_type,:pOrderData->opr_code,:pOrderData->opr_code,
                                            :pOrderData->opr_source,:pOrderData->eff_time,:sTmpTime[0];
        if (SQLCODE !=0)
        {
          		sprintf(retMsg, "����ƷѰ��±�������[%d]!", SQLCODE);
          		#ifdef _DEBUG_
						pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
				#endif
				PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-34", retMsg);
          		return -34;
        }
      }
/***
    }
***/

    /*add by hejt end */
    /** ��¼�û��Ķ�����Ϣ ****/
    printf("\npOrderData->end_time[%s]\n",pOrderData->end_time);
    strcpy(vid_no,  pOrderData->id_no);
    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt, "  UPDATE DDSMPORDERMSG%c  "
                     "     set  VALID_FLAG = :v1   , "
                     "   END_TIME =  to_date(:v2,'yyyymmdd hh24:mi:ss') , TOTAL_DATE = to_number(:v3) , "
                     "   LOGIN_ACCEPT = to_number(:v4) ,OPR_CODE = :v5 ,"
                     "   opr_time = to_date(:v6,'yyyymmdd hh24:mi:ss') ,eff_time = to_date(:v7,'yyyymmdd hh24:mi:ss') ,"
                     "   opr_source = :v8 ,password = :v9 , comb_flag= :v10 ,mode_code = :v11 "
                     "   WHERE   order_id = :v35  "  ,vid_no[strlen(vid_no)-1] );
#ifdef _DEBUG_

    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "22222222222222222222222 vid_no =[%s]", vid_no);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->order_id=[%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->eff_time=[%s]", pOrderData->eff_time);
#endif
      EXEC SQL PREPARE prepare2 From :dynStmt;
      EXEC SQL EXECUTE prepare2 USING   :pOrderData->valid_flag , :pOrderData->end_time, :pOrderData->total_date  ,
                              :pOrderData->login_accept, :pOrderData->opr_code,
                              :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->opr_source,:pOrderData->password,
                              :pOrderData->comb_flag,:pOrderData->mode_code ,
                              :pOrderData->order_id  ;
    if (SQLCODE !=0)
    {
      		sprintf(retMsg, "���±�DDSMPORDERMSG%ld����[%d]!", atol(vid_no)%10, SQLCODE);
			pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	        pubLog_DBErr(_FFL_, "", "-23", retMsg);
      		return -23;
    }

    /** ��¼�û��Ķ�����ʷ��Ϣ ****/
    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt, "  INSERT INTO DDSMPORDERMSGHIS%c "
          "("
                      " order_id,id_no,phone_no,serv_code                 "
                      " ,opr_code,third_id_no,third_phoneno,spid          "
                      " ,bizcode,prod_code,comb_flag,mode_code            "
                      " ,chg_flag,chrg_type,channel_id,opr_source         "

                      " ,first_order_date,first_login_no,last_order_date  "
                      " ,opr_time,eff_time,end_time                       "
                      " ,local_code,valid_flag,belong_code                "
                      " ,total_date,login_accept,reserved                 "
                      " ,UPDATE_LOGIN,UPDATE_DATE,UPDATE_TIME             "
            " ,UPDATE_ACCEPT,UPDATE_CODE  ,UPDATE_TYPE ,sm_code ,password, "
            "group_id"

          ")"
          " select "
                      " order_id,id_no,phone_no,serv_code ,                "
                      " opr_code,third_id_no,third_phoneno,spid ,         "
                      " bizcode,prod_code,comb_flag,mode_code ,           "
                      " chg_flag,chrg_type,channel_id,opr_source,        "

                      " first_order_date,first_login_no,last_order_date,  "
                      " opr_time,eff_time,end_time ,                      "
                      " local_code,valid_flag,belong_code ,               "
                      " total_date,login_accept,reserved ,                "
              " :v1, to_number(:v2) , to_date(:v3,'yyyymmdd hh24:mi:ss'),  "
              "  to_number(:v4) , :v5 , 'd' ,sm_code ,password,group_id"
              "  from  DDSMPORDERMSG%c "
              "  where  order_id = :v6  "
          ,vid_no[strlen(vid_no)-1] ,vid_no[strlen(vid_no)-1] );
#ifdef _DEBUG_

    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", " 11111111111111111111111vid_no =[%s]", vid_no);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->order_id=[%ld],pOrderData->login_no=[%s]", pOrderData->order_id,pOrderData->login_no);
#endif
      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING
                              :pOrderData->login_no  ,:pOrderData->total_date  ,:pOrderData->opr_time  ,
                              :pOrderData->login_accept, :pOrderData->op_code  ,
                              :pOrderData->order_id  ;
    if (SQLCODE !=0)
    {
      		sprintf(retMsg, "�����DDSMPORDERMSGHIS%ld����[%d]!", atol(vid_no)%10, SQLCODE);
	      	pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->login_no=[%s],pOrderData->total_date=[%s],pOrderData->opr_time=[%s],pOrderData->op_code=[%s],pOrderData->order_id=[%ld],pOrderData->login_accept=[%s]",pOrderData->login_no,pOrderData->total_date,pOrderData->opr_time,pOrderData->op_code,pOrderData->order_id,pOrderData->login_accept);
			pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	        pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-22", retMsg);
      		return -22;
    }

    /*add by shijing for ͬ��������ϵ��VGOP BEGIN*/
      	if((strcmp(pOrderData->serv_code,"23")==0)||(strcmp(pOrderData->serv_code,"60")==0)||(strcmp(pOrderData->serv_code,"16")==0))
      	{
      		printf("\n22222222\n");

          fvgop=0;
          fvgop=fVGgopSync(pOrderData,"1",retMsg);
          if (fvgop != 0)
          {
          	printf("other insert TBOSSVGOPBIZPORTALMSGʧ��!\n");
          	return fvgop;
          }

      	}
      	/*add by shijing for ͬ��������ϵ��VGOP END*/

	}

  /** ��¼�û��Ķ���������Ϣ ****/
  memset(dynStmt, 0, sizeof(dynStmt));
  sprintf(dynStmt, "  INSERT INTO WDSMPORDEROPR%d  "
        "("
                  " order_id,id_no,phone_no,serv_code                 "
                  " ,opr_code,third_id_no,third_phoneno,spid          "
                  " ,bizcode,prod_code,comb_flag,mode_code            "
                  " ,chg_flag,chrg_type,channel_id,opr_source         "
                  " ,first_order_date,first_login_no,last_order_date  "
                  " ,opr_time,eff_time,end_time                       "
                  " ,local_code,valid_flag,belong_code "
                  " ,total_date,login_accept,reserved                 "
                  " ,sys_note,op_note ,oprnumb   "
                  " ,login_no ,op_code ,sm_code,password, "
                  "group_id"
        ")"
        "values"
        "("
              " :v1,to_number(:v2),:v3,:v4, "
              " :v5,to_number(:v6),:v7, :v8 , "
              " :v9,:v10,:v11, :v12 , "
              " :v13,:v14,:v15, :v16 , "
            " to_date(:v17,'yyyymmdd hh24:mi:ss'),:v18,to_date(:v19,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v20,'yyyymmdd hh24:mi:ss'), to_date(:v21,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v22,'yyyymmdd hh24:mi:ss') ,  "
              " :v23, :v24,:v25, "
            " to_number(:v26) ,to_number(:v27) , :v28 , "
            " :v29, :v30, :v31,"
            "  :v32,:v33 ,:v34 ,:v35,"
            ":v36"
        ")",atoi(pOrderData->total_date)/100 );

  EXEC SQL PREPARE prepare1 From :dynStmt;
  EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                          :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,
                          :pOrderData->bizcode,:pOrderData->prod_code, :pOrderData->comb_flag,:pOrderData->mode_code,
                          :pOrderData->chg_flag,:pOrderData->chrg_type,:pOrderData->channel_id,:pOrderData->opr_source,
                          :pOrderData->first_order_date,:pOrderData->first_login_no,:pOrderData->last_order_date,
                          :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->end_time ,
                          :pOrderData->local_code,:pOrderData->valid_flag,:pOrderData->belong_code ,
                          :pOrderData->total_date,:pOrderData->login_accept,:pOrderData->reserved ,
                          :pOrderData->sys_note,:pOrderData->op_note ,:pOrderData->oprnumb ,
                          :pOrderData->login_no ,:pOrderData->op_code ,:pOrderData->sm_code,:pOrderData->password,
                          :vGroupId;
  if (SQLCODE !=0)
  {
		#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
		#endif

		#ifdef _DEBUG_
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "INSERT INTO WDSMPORDEROPR error =[%d]", SQLCODE);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->order_id=[%ld],pOrderData->id_no=[%s],pOrderData->phone_no=[%s],pOrderData->serv_code=[%s]",pOrderData->order_id,pOrderData->id_no,pOrderData->phone_no,pOrderData->serv_code);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code=[%s][%s][%s][%s]",pOrderData->opr_code,pOrderData->third_id_no, pOrderData->third_phoneno,pOrderData->spid);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->bizcode=[%s][%s][%s][%s]",pOrderData->bizcode,pOrderData->prod_code,pOrderData->comb_flag,pOrderData->mode_code);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chg_flag=[%s][%s][%s][%s]",pOrderData->chg_flag,pOrderData->chrg_type,pOrderData->channel_id,pOrderData->opr_source);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->first_order_date=[%s][%s][%s]",pOrderData->first_order_date,pOrderData->first_login_no,pOrderData->last_order_date);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_time=[%s][%s][%s]",pOrderData->opr_time,pOrderData->eff_time,pOrderData->end_time);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->local_code=[%s][%s][%s][%s][%s]",pOrderData->local_code,pOrderData->valid_flag,pOrderData->login_no,pOrderData->op_code,pOrderData->belong_code);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->total_date=[%s][%s][%s]",pOrderData->total_date,pOrderData->login_accept,pOrderData->reserved);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->sys_note=[%s][%s][%s]",pOrderData->sys_note,pOrderData->op_note ,pOrderData->oprnumb);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->login_no=[%s][%s][%s][%s]",pOrderData->login_no ,pOrderData->op_code ,pOrderData->sm_code,pOrderData->password);
		#endif
    sprintf(retMsg, "������ˮ��WDSMPORDEROPR%d ����[%d]!", atoi(pOrderData->total_date)/100, SQLCODE);
	    PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	    pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-24", retMsg);
    return -24;
  }

	#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->first_order_date=[%s]", pOrderData->first_order_date);
	#endif

  /** ������ϵͬ�����ӿڱ�
      1��ordersync������ϵͬ����־��0����ͬ����1��BOSS-��ƽ̨��2��ƽ̨-��BOSS��Ĭ��0';��Ҫͬ������
      2�����ڷ���ƽ̨��Ϣ����Ϊ�첽����ģ���ͬ�����ݣ�������ƽ̨����ʱ��ͬ������
        	(strncmp(pOrderData->sync_flag,"0",1)==0)   1��ͬ��  0���첽����;Ĭ��ͬ�� **/
  pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "ordersync=[%s]pOrderData->sync_flag=[%s]pOrderData->multi_order_flag=[%s]",ordersync,pOrderData->sync_flag,pOrderData->multi_order_flag);
  if ( ( (strcmp(ordersync,"1" ) == 0) &&  (strncmp(pOrderData->sync_flag,"0",1)!=0) )
      && (strncmp(pOrderData->multi_order_flag,"1",1) !=0 )  )
     {

      /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
        memset(sTmpTime, 0, sizeof(sTmpTime));
        if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
        {
            sprintf(retMsg,"opr_time����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[1],pOrderData->first_order_date)) != 0)
        {
            sprintf(retMsg,"first_order_date����fPubFormatTimeʧ��! iRet=[%d]",i);
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[2],pOrderData->eff_time)) != 0)
        {
            sprintf(retMsg,"eff_time����fPubFormatTimeʧ��! iRet=[%d]",i);
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[3],pOrderData->end_time)) != 0)
        {
            sprintf(retMsg,"end_time����fPubFormatTimeʧ��! iRet=[%d]",i);
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-26", retMsg);
        return -26;
        }

        if (strcmp(vopr_flag ,"1") != 0 )
        	{  /**��Чʱ���Ƕ�����ϵ���ʱ��**/
            strcpy(sTmpTime[2]  ,sTmpTime[0]  );
#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "����ʱ�� sTmpTime[0]=[%s]", sTmpTime[0]);
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "�״ζ���ʱ�� sTmpTime[1]=[%s]", sTmpTime[1]);
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "��Чʱ�� sTmpTime[2]=[%s]", sTmpTime[2]);
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "ʧЧʱ�� sTmpTime[3]=[%s]", sTmpTime[3]);
#endif
        }

        Sinitn(temp_op_accept);
        EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :temp_op_accept from dual;

      	memset(dynStmt, 0, sizeof(dynStmt));
      /**2008.11.24 ȡ����MISCƽ̨������Ч�Ķ�����ϵͬ����MISC**/
      if((strcmp(pOrderData->valid_flag,"1")==0 && strcmp(pOrderData->opr_source,"08")!=0))
      	{
      		pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "MISCƽ̨������ϵ������Ч������BOSS����������ͬ������[%s]",ordersynctable);
      	}
      	else
      	{
      		sprintf(dynStmt, "  INSERT INTO %s   "
        	"("
        	          " MAXACCEPT,LOCALCODE,Misisdn,TMSISDN ,    "
        	          " OprCode,EfftTime,Source,Channel ,        "
        	          " ChgFlg,OrderStatus,BillFlg,SPID , SPBIZCODE,MaxScan ,infocode,infovalue , "
        	          " BIZTYPE,PASSWORD ,REQTIME ,OPRTIME ,endtime ,orderid"
        	")"
        	"values"
        	"("
        	      " :v1,:v2,:v3,:v4, "
        	      " :v5, :v6,:v7, :v8 , "
        	      " :v9,:v10,:v11, :v12,:v13,'0' ,:v14,:v15 ,:v16,:v17,:v18,:v19,:v20,:v21 "
        	")" ,ordersynctable );

#ifdef _DEBUG_
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "temp_op_accept =[%s]", temp_op_accept );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->local_code   =[%s]", pOrderData->local_code   );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->phone_no     =[%s]", pOrderData->phone_no     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->third_phoneno=[%s]", pOrderData->third_phoneno);
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[2]              =[%s]", sTmpTime[2]              );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_source   =[%s]", pOrderData->opr_source   );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->channel_id   =[%s]", pOrderData->channel_id   );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chg_flag     =[%s]", pOrderData->chg_flag     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chrg_type    =[%s]", pOrderData->chrg_type    );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->send_spid    =[%s]", pOrderData->send_spid    );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->send_bizcode =[%s]", pOrderData->send_bizcode );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->infocode     =[%s]", pOrderData->infocode     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->infovalue    =[%s]", pOrderData->infovalue    );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->serv_code    =[%s]", pOrderData->serv_code    );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->password     =[%s]", pOrderData->password     );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[1]             =[%s]", sTmpTime[1]             );
					pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[0]             =[%s]", sTmpTime[0]             );
#endif

      		EXEC SQL PREPARE prepare1 From :dynStmt;
      		EXEC SQL EXECUTE prepare1 USING :temp_op_accept,:pOrderData->local_code,:pOrderData->phone_no,:pOrderData->third_phoneno,
      		                    :pOrderData->opr_code,:sTmpTime[2], :pOrderData->opr_source,:pOrderData->channel_id,
      		                    :pOrderData->chg_flag,:pOrderData->opr_code,:pOrderData->chrg_type,:pOrderData->send_spid,
      		                    :pOrderData->send_bizcode ,:pOrderData->infocode ,:pOrderData->infovalue ,
      		                    :pOrderData->serv_code ,:pOrderData->password ,:sTmpTime[1] ,:sTmpTime[0],:sTmpTime[3],:pOrderData->order_id ;
      		if (SQLCODE !=0)
      		{
      		  sprintf(retMsg, "����ͬ���ӿڱ�[%s] ����[%d]!", ordersynctable, SQLCODE);
				pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
				PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	    		pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-25", retMsg);
      		  return -25;
      		}
    		}
    }

		Trim(pOrderData->local_code);
    Trim(pOrderData->serv_code);
    /*ȫ�����������ܲ����ݼƷѱ��˶���ע����ȫҵ���˶�����misc������ȫ������ �˴��޸���ͬ���޸� SyncUserOrderNew.cp ***/
    /**chendh 2009.01.15�޸�,��ȫ��������ϵ�仯��¼ȫ���ӿڱ�**/
 	if( ( strcmp(pOrderData->serv_code,"03")==0
		||strcmp(pOrderData->serv_code,"04")==0
		||strcmp(pOrderData->serv_code,"05")==0
		||strcmp(pOrderData->serv_code,"07")==0
		||strcmp(pOrderData->serv_code,"08")==0
		||strcmp(pOrderData->serv_code,"09")==0
		||strcmp(pOrderData->serv_code,"10")==0
		||strcmp(pOrderData->serv_code,"13")==0
		||strcmp(pOrderData->serv_code,"14")==0
		||strcmp(pOrderData->serv_code,"20")==0
		||strcmp(pOrderData->serv_code,"27")==0
		||strcmp(pOrderData->serv_code,"36")==0
		||strcmp(pOrderData->serv_code,"39")==0
		||strcmp(pOrderData->serv_code,"41")==0
		||strcmp(pOrderData->serv_code,"42")==0
		||strcmp(pOrderData->serv_code,"52")==0
		||strcmp(pOrderData->serv_code,"53")==0
		||strcmp(pOrderData->serv_code,"56")==0
		||strcmp(pOrderData->serv_code,"57")==0
		||strcmp(pOrderData->serv_code,"60")==0
		||strcmp(pOrderData->serv_code,"65")==0
		||strcmp(pOrderData->serv_code,"70")==0
		||strcmp(pOrderData->serv_code,"72")==0
		||strcmp(pOrderData->serv_code,"73")==0) /*Liu. add for �ֻ�Ǯ�� at 2013.01.23 */
	&&(strcmp(pOrderData->local_code,"G")==0))
    {
      Sinitn(temp_op_accept);
        EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :temp_op_accept from dual;

      memset(dynStmt, 0, sizeof(dynStmt));
      sprintf(dynStmt, "  INSERT INTO oboss.tobsubinfomsg  "
        "("
                  " MAXACCEPT,LOCALCODE,Misisdn,TMSISDN ,    "
                  " OprCode,EfftTime,Source,Channel ,        "
                  " ChgFlg,OrderStatus,BillFlg,SPID , SPBIZCODE,MaxScan ,infocode,infovalue , "
                  " BIZTYPE,PASSWORD ,REQTIME ,OPRTIME ,endtime "
        ")"
        "values"
        "("
              " :v1,:v2,:v3,:v4, "
              " :v5,:v6,:v7, :v8 , "
              " :v9,:v10,:v11, :v12,:v13,'0' ,:v14,:v15 ,:v16,:v17,:v18,:v19,:v20 "
        ")"  );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->send_spid         =[%s]", pOrderData->send_spid         );
      #ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "temp_op_accept =[%s]", temp_op_accept );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->local_code   =[%s]", pOrderData->local_code   );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->phone_no     =[%s]", pOrderData->phone_no     );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->third_phoneno=[%s]", pOrderData->third_phoneno);
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[2]             =[%s]", sTmpTime[2]             );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_source   =[%s]", pOrderData->opr_source   );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->channel_id   =[%s]", pOrderData->channel_id   );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chg_flag     =[%s]", pOrderData->chg_flag     );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->chrg_type    =[%s]", pOrderData->chrg_type    );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->send_spid         =[%s]", pOrderData->send_spid         );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->send_bizcode      =[%s]", pOrderData->send_bizcode      );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->infocode      =[%s]", pOrderData->infocode      );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->infovalue      =[%s]", pOrderData->infovalue      );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->serv_code         =[%s]", pOrderData->serv_code         );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "pOrderData->password      =[%s]", pOrderData->password      );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[1]             =[%s]", sTmpTime[1]             );
        pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "sTmpTime[0]             =[%s]", sTmpTime[0]             );
      #endif
      /*��Чʱ��Ϊ��ʱ,Ĭ��Ϊ����ʱ��*/
      Trim(sTmpTime[2]);
			if(strlen(sTmpTime[2])==0)
			{
					strcpy(sTmpTime[2],sTmpTime[0]);
			}
      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :temp_op_accept,:pOrderData->local_code,:pOrderData->phone_no,:pOrderData->third_phoneno,
                          :pOrderData->opr_code,:sTmpTime[2], :pOrderData->opr_source,:pOrderData->channel_id,
                          :pOrderData->chg_flag,:pOrderData->opr_code,:pOrderData->chrg_type,:pOrderData->send_spid,
                          :pOrderData->send_bizcode ,:pOrderData->infocode ,:pOrderData->infovalue ,
                          :pOrderData->serv_code ,:pOrderData->password ,:sTmpTime[1] ,:sTmpTime[0],:sTmpTime[3] ;
      if (SQLCODE !=0)
      {
        	sprintf(retMsg, "�����ܲ����ݼƷ�ϵͳ�ӿڱ� ����[%d]!", SQLCODE);
			pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "dynStmt=[%s]", dynStmt);
			PUBLOG_DBDEBUG("fPubDsmpOrderMsg");
	    	pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "-24", retMsg);
        	return -25;
      }
    }

    /** �õ�����Ϣ��Ϣ **/
    retCode = fDsmpGetShortMsg(pOrderData,vcontent ,vcontent2 ,"00" ,retMsg);
    if (retCode != 0)
    {
	      pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "fDsmpGetShortMsg����ֵretCode=[%d][%s]",retCode,retMsg);
	      strcpy(vcontent  ,"" );
	      strcpy(vcontent2  ,"" );
	      pubLog_DBErr(_FFL_, "fPubDsmpOrderMsg", "%d", retMsg,retCode);
      /* return -26;*/
    } /*del by hejt ���� �ſ�*/

    strcpy(retMsg  ,"�����ɹ�" );
    Trim(vcontent);
    Trim(vcontent2 );
    retCode = 0;

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "fpubDsmpOrdermsg:vcontent=[%s]", vcontent);
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "fpubDsmpOrdermsg:vcontent2=[%s]", vcontent2);
#endif

    if (strlen(vcontent) > 0 )
   {
		retCode = fSendShortMsg(pOrderData->login_accept,pOrderData->phone_no,pOrderData->op_code,pOrderData->login_no,vcontent);
        if (retCode != 0)
        {
            strcpy(vcontent  ,"" );
          sprintf(retMsg, "������Žӿڱ����[%d]!",retCode);
          /** return -26; **/
        }
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "fpubDsmpOrdermsg:������Žӿڱ� retCode=[%d]", retCode);
#endif
    }

    if (strlen(vcontent2) > 0 )
    {
        retCode = fSendShortMsg(pOrderData->login_accept,pOrderData->third_phoneno,pOrderData->op_code,pOrderData->login_no,vcontent2);
        if (retCode != 0)
        {
          sprintf(retMsg, "������Žӿڱ����[%d]!",retCode);
          /** return -26; **/
          strcpy(vcontent2  ,"" );
        }
    }

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpOrderMsg", "", "-- end -- fPubDsmpOrderMsg   ");
#endif

  return 0 ;
}


/******************************************************************************************
@lixg FUNC : fDsmpInfoOnOffCheck  �û���Ϣ���񿪹ؼ��
@input  params: phone_no -> �ֻ�����
@input  params: id_no ->    ID_NO
@input  params: biz_type -> SPҵ������
@input  params: return_mess -> ������Ϣ
@output params: 0 success else 0 fail
*******************************************************************************************
**	R_JLMob_liuyinga_CRM_CMI_2011_0089�������ռ����������ص��¶�����ֵҵ���Ʒʧ���û�������
**	�޸ķ������Ӷ�ʧ����Ϣ�ļ�¼��by zhangxina @ 2011.3.16
*******************************************************************************************
**	R_JLMob_liuyinga_CRM_CMI_2011_0158��������BOSSϵͳ���������������ش򿪶�������ҵ��ʧ�ܵ��û�������������
**	�޸ķ������Ӷ�������ҵ���롢ҵ����롣by zhangxina @ 2011.5.9
**	int fDsmpInfoOnOffCheck(char *id_no, char *phone_no,char *biz_type,char *belong_code,char *return_mess)
*******************************************************************************************
**	R_JLMob_liuyinga_CRM_CMI_2011_0107�����������Ż�����
**	�޸ķ�����������ҵ�����ñ��ڴ˱�����Ч������ҵ�������ֿ��ز�����ҵ������ܿ�����Ȼ������Ч
******************************************************************************************/
int fDsmpInfoOnOffCheck(char *id_no, char *phone_no, char *biz_type, char *spid, char *bizcode, char *belong_code, char *return_mess)
{
	EXEC SQL BEGIN DECLARE SECTION;
		int		icount = 0;
		int		iReturn = 0;
		long	lErrorAccept = 0;
		long	lPrice = 0;

		char	vid_no[14+1];
		char	vphone_no[11+1];
		char	vbiz_type[2+1];
		char	vbiz_type_temp[3+1];
		char	sbiztypetemp[4+1];
		char	sRetCode[4+1];
		char	sRetMsg[60+1];
		char	sSpid[20+1];
		char	sBizCode[21+1];
		char	sServName[64];
	EXEC SQL END DECLARE SECTION;

	Sinitn(vid_no	);
	Sinitn(vphone_no);
	Sinitn(vbiz_type);
	Sinitn(vbiz_type_temp);
	Sinitn(sbiztypetemp);
	Sinitn(sSpid	);
	Sinitn(sBizCode	);
	Sinitn(sRetCode	);
	Sinitn(sRetMsg	);
	Sinitn(sServName);

	strcpy(sRetCode, "0");
	strcpy(sRetMsg, "success");
	strcpy(vphone_no, phone_no );
	strncpy(vid_no, id_no, 14);
	strncpy(vbiz_type, biz_type, 2);
	strcpy(sSpid,		spid);
	strcpy(sBizCode,	bizcode);

	Trim(vphone_no);
	Trim(vid_no);
	Trim(vbiz_type);
	Trim(sSpid);
	Trim(sBizCode);

	pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "=========��ʼ����fDsmpInfoOnOffCheck������=========");

	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input vphone_no = [%s]", vphone_no);
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input vid_no = [%s]", vid_no);
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input vbiz_type = [%s]", vbiz_type);
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input sSpid = [%s]", sSpid);
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input sBizCode = [%s]", sBizCode);
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "input lPrice = [%ld]", lPrice);
	#endif

	/*����û� ��Ϣ���񿪹��Ƿ���������ҵ��*/
	icount = 0;
	EXEC SQL select count(*) into :icount
		from dmobileinfomsg
		where  msisdn = :vphone_no  and switch_type='0199' and SYSDATE BETWEEN begin_time AND end_time ;
	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "��ȡ��Ϣ�����ܿ���ʧ��, SQLCODE = [%d]", SQLCODE);
		sprintf(return_mess, "��ȡ��Ϣ�����ܿ���ʧ��![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fDsmpInfoOnOffCheck");
		pubLog_DBErr(_FFL_, "fDsmpInfoOnOffCheck", "-2", return_mess);
		iReturn = -2;
		sprintf(sRetMsg, "%s",return_mess);
		goto ONOFF_END;
	}
	if (icount > 0)
	{
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "�û���Ϣ�����ܿ����Ѿ��ر�,��������ҵ�� " );
		sprintf(return_mess, "�û���Ϣ�����ܿ����Ѿ��ر�,��������ҵ��!" );
		PUBLOG_DBDEBUG("fDsmpInfoOnOffCheck");
		pubLog_DBErr(_FFL_, "fDsmpInfoOnOffCheck", "-3", return_mess);
		iReturn = -3;
		sprintf(sRetMsg, "%s",return_mess);
		goto ONOFF_END;
	}

	icount = 0;
	memset(sbiztypetemp,0,sizeof(sbiztypetemp));
	sprintf(sbiztypetemp,"01%s",vbiz_type);
	Trim(sbiztypetemp);
	EXEC SQL SELECT count(*) into :icount FROM dmobileinfomsg
			WHERE  SYSDATE BETWEEN begin_time AND end_time
				AND  msisdn = :vphone_no
				AND  switch_type= :sbiztypetemp;
	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "��ȡ��Ϣ����ֿ���ʧ��, biztype=[%s],SQLCODE = [%d]", vbiz_type,SQLCODE);
		sprintf(return_mess, "��ȡ��Ϣ����ֿ���ʧ��![%d] ҵ������[%s]",SQLCODE ,vbiz_type);
		PUBLOG_DBDEBUG("fDsmpInfoOnOffCheck");
		pubLog_DBErr(_FFL_, "fDsmpInfoOnOffCheck", "-4", return_mess);
		iReturn = -4;
		sprintf(sRetMsg, "%s",return_mess);
		goto ONOFF_END;
	}
	if (icount > 0)
	{
		/* R_JLMob_liuyinga_CRM_CMI_2011_0107�����������Ż����� by zhangxina @ 20110328  BEGIN */
		icount = 0;
		EXEC SQL SELECT count(*)
				INTO :icount
				FROM sMobileSelfCfg
				WHERE biztype = :vbiz_type
					AND spid = :sSpid
					AND work_flag = '1';
		if (SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "��ȡ����ҵ��������Ϣʧ��, sSpid=[%s], SQLCODE = [%d]", sSpid, SQLCODE);
			sprintf(return_mess, "��ȡ����ҵ��������Ϣʧ��![%d] Spid = [%s]", SQLCODE, sSpid);
			PUBLOG_DBDEBUG("fDsmpInfoOnOffCheck");
			pubLog_DBErr(_FFL_, "fDsmpInfoOnOffCheck", "-6", return_mess);
			iReturn = -6;
			sprintf(sRetMsg, "%s",return_mess);
			goto ONOFF_END;
		}
		if( icount > 0)
		{
			/* ����ҵ�����ñ��д������ݱ�ʾΪ����ҵ�񣬷ֿ��ز�����ҵ����� */
			iReturn = 0;
		}
		else
		{
			/* ����ҵ�����ñ������ݰ���ԭ�����̰��� */
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "", "�û���Ϣ����ֿ����Ѿ��ر�,��������ҵ�� " );
			sprintf(return_mess, "�û���Ϣ����ֿ����Ѿ��ر�,��������ҵ��!" );
			PUBLOG_DBDEBUG("fDsmpInfoOnOffCheck");
			pubLog_DBErr(_FFL_, "fDsmpInfoOnOffCheck", "-5", return_mess);
		 	iReturn = -5;
		 	sprintf(sRetMsg, "%s",return_mess);
		 	goto ONOFF_END;
		}
	 	/* R_JLMob_liuyinga_CRM_CMI_2011_0107�����������Ż����� by zhangxina @ 20110328  END */
	}

ONOFF_END:
	if(iReturn == 0)
	{
		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "�û���Ϣ���񿪹�������", "-- end -- fDsmpInfoOnOffCheck   ");
		#endif

		return 0;
	}
	else
	{
		EXEC SQL ROLLBACK;

		EXEC SQL SELECT to_number(sMaxSysAccept.nextval), to_char(:iReturn), nvl(price, 0), nvl(servname, chr(0))
				INTO :lErrorAccept, :sRetCode, :lPrice, :sServName
				FROM dDsmpSpBizInfo
				WHERE spid = :sSpid
					AND bizcode = :sBizCode and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
		if (SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "��¼�������ص���ҵ��ʧ����Ϣ����", "��ѯ��ˮʧ��, biztype=[%s],SQLCODE = [%d]", vbiz_type,SQLCODE);
			return iReturn;
		}
		Trim(sRetCode);
		Trim(sRetMsg);
		Trim(sServName);
		pubLog_Debug(_FFL_,"��¼�������ص���ҵ��ʧ����Ϣ","lErrorAccept[%ld], sRetCode[%s]","price[%ld], ServName[%s]", lErrorAccept, sRetCode, lPrice, sServName);

		if( (iReturn == -2)||(iReturn == -3) )
		{
			EXEC SQL INSERT INTO dMobileOnOffError(ERROR_ACCEPT, MSISDN, SWITCH_TYPE, BIZTYPE, ID_NO, OP_TIME,
					RETURN_CODE, RETURN_MSG, SPID, BIZCODE, SERVNAME, PRICE)
				VALUES(:lErrorAccept, :vphone_no, '0199', :vbiz_type, to_number(:vid_no), sysdate,
					:sRetCode, :sRetMsg, :sSpid, :sBizCode, :sServName, :lPrice);
			if (SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "��¼�������ص���ҵ��ʧ����Ϣ����", "��¼dMobileOnOffErrorʧ��, SQLCODE = [%d]", SQLCODE);
				pubLog_Debug(_FFL_, "lErrorAccept[%d]", "vphone_no[%s],vbiz_type[%s], vid_no[%s]", "sRetCode[%s], sRetMsg{%s]", lErrorAccept,vphone_no, vbiz_type, vid_no,sRetCode,sRetMsg);
				EXEC SQL ROLLBACK;
				return iReturn;
			}
		}
		else if( (iReturn == -4)||(iReturn == -5) )
		{
			EXEC SQL INSERT INTO dMobileOnOffError(ERROR_ACCEPT, MSISDN, SWITCH_TYPE, BIZTYPE, ID_NO, OP_TIME,
					RETURN_CODE, RETURN_MSG, SPID, BIZCODE, SERVNAME, PRICE)
				VALUES(:lErrorAccept, :vphone_no, :sbiztypetemp, :vbiz_type, to_number(:vid_no), sysdate,
					:sRetCode, :sRetMsg, :sSpid, :sBizCode, :sServName, :lPrice);
			if (SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "��¼�������ص���ҵ��ʧ����Ϣ����", "��¼dMobileOnOffErrorʧ��, SQLCODE = [%d]", SQLCODE);
				pubLog_Debug(_FFL_, "lErrorAccept[%d]", "vphone_no[%s],sbiztypetemp[%s], vbiz_type[%s], vid_no[%s]", "sRetCode[%s], sRetMsg{%s]", lErrorAccept,vphone_no, sbiztypetemp, vbiz_type, vid_no,sRetCode,sRetMsg);
				EXEC SQL ROLLBACK;
				return iReturn;
			}
		}

		EXEC SQL INSERT INTO dMobileOnOffErrorDetail(ERROR_ACCEPT, MSISDN, SWITCH_TYPE, ID_NO, BEGIN_TIME, END_TIME, OP_TIME, OP_ACCEPT, LOGIN_NO )
			SELECT :lErrorAccept, MSISDN, SWITCH_TYPE, ID_NO, BEGIN_TIME, END_TIME, OP_TIME, LOGIN_ACCEPT, LOGIN_NO
			FROM dMobileInfoMsg
			WHERE SYSDATE BETWEEN begin_time AND end_time
				AND msisdn = :vphone_no;
		if (SQLCODE != 0)
		{
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "��¼�������ص���ҵ��ʧ����Ϣ����", "��¼dMobileOnOffErrorDetailʧ��, SQLCODE = [%d]", SQLCODE);
			EXEC SQL ROLLBACK;
			return iReturn;
		}

		#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "fDsmpInfoOnOffCheck", "�û���Ϣ���񿪹��Ѿ��رջ��ѯʧ��", "-- end -- fDsmpInfoOnOffCheck   ");
		#endif

		EXEC SQL COMMIT;
		return iReturn;
	}
}


/**********************************************
 @FUNC   fDsmpSPBizCheck  SP��ҵ��Ϣ�� SPҵ����Ϣ��Ч�Լ��
 @input  params: sp_id ->    SP��ҵ����
 @input  params: biz_code -> SPҵ�����
 @input  params: biz_type -> SPҵ������
 @input  params: check_type -> �������    A��ȫ����B��SPҵ��C��SP��Ϣ
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpSPBizCheck(char *sp_id, char *biz_code, char *biz_type,char *check_type,char *return_mess)
{
  EXEC SQL BEGIN DECLARE SECTION;
        char vsp_id[20+1];
    char vbiz_code[21+1];
    char vbiz_type[2+1];
    char vcheck_type[1+1];
    int  icount = 0;
    char vsp_status[1+1];
    char vbiz_status[1+1];
  EXEC SQL END DECLARE SECTION;

    Sinitn(vsp_id );
    Sinitn(vbiz_code );
    Sinitn(vbiz_type );
    Sinitn(vsp_status );
    Sinitn(vbiz_status );
    Sinitn(vcheck_type);

  strcpy(vsp_id, sp_id);
  strcpy(vbiz_code, biz_code);
  strcpy(vbiz_type, biz_type);
  strcpy(vcheck_type,check_type);
    Trim(vsp_id);
    Trim(vbiz_code);
    Trim(vbiz_type);

  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "=========��ʼ����fDsmpSPBizCheck������=========");

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "input vsp_id = [%s]", vsp_id);
  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "input biz_code = [%s]", biz_code);
  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "  vbiz_code = [%s]", vbiz_code);
  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "input vbiz_type = [%s]", vbiz_type);
#endif

  if ((strcmp(vcheck_type,"A")== 0 ) || (strcmp(vcheck_type,"C")== 0 ))
  	{
      /**�����ҵҵ��״̬**/
    EXEC SQL SELECT SPSTATUS into :vsp_status FROM DDsmpSPINFO
              WHERE SPID = :vsp_id and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    if (SQLCODE == 1403){
      pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "û���ҵ���SP��ҵ��Ϣ, SQLCODE = [%d],SPID[%s]", SQLCODE,vsp_id);
      sprintf(return_mess, "û���ҵ���SP��ҵ��Ϣ!SQLCODE[%d],SPID[%s]",SQLCODE,vsp_id);
		  	PUBLOG_DBDEBUG("fDsmpSPBizCheck");
	    	pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1301", return_mess);
      return  1301;
    }
    if (SQLCODE != 0)
    {
      	pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "��ȡSP��ҵ��Ϣʧ��, SQLCODE = [%d]", SQLCODE);
      	sprintf(return_mess, "��ȡSP��ҵ��Ϣʧ��![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fDsmpSPBizCheck");
	   	pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1301", return_mess);
      	return   1301;
    }
	if (strcmp(vsp_status, "N") == 0)
	{
      pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "��SP��ҵ״̬Ϊ��Ч,����ʹ�� SQLCODE = [%d],sp_status[%s]", SQLCODE,vsp_status);
      sprintf(return_mess, "��SP��ҵ״̬Ϊ��Ч,����ʹ��!SQLCODE[%d],sp_status[%s]",SQLCODE,vsp_status);
	  pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1301", return_mess);
      return 1301;
    }
  }

    /**���SPҵ����Ϣ **/
  if ((strcmp(vcheck_type,"A")== 0 ) || (strcmp(vcheck_type,"B")== 0 ))
  {
    EXEC SQL SELECT BIZSTATUS into :vbiz_status FROM DDsmpSPBIZINFO
              WHERE SPID = :vsp_id AND BIZCODE = :vbiz_code and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd');
    if (SQLCODE == 1403)
    {
      	pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "û���ҵ���SPҵ����Ϣ, SQLCODE = [%d],SPID[%s]", SQLCODE,vsp_id);
      	sprintf(return_mess, "û���ҵ���SPҵ����Ϣ!SQLCODE[%d],SPID[%s]",SQLCODE,vsp_id);
		PUBLOG_DBDEBUG("fDsmpSPBizCheck");
   		pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1302", return_mess);
      	return 1302;
    }
    if (SQLCODE != 0)
    {
      	pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "��ȡSPҵ����Ϣʧ��, SQLCODE = [%d]", SQLCODE);
      	sprintf(return_mess, "��ȡSPҵ����Ϣʧ��![%d]",SQLCODE);
		PUBLOG_DBDEBUG("fDsmpSPBizCheck");
    	pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1302", return_mess);
      	return 1302;
    }
    if (strcmp(vbiz_status, "N") == 0)
    {
      pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "��SPҵ��״̬Ϊ��Ч,����ʹ��, SQLCODE = [%d],sp_status[%s]", SQLCODE,vsp_status);
      sprintf(return_mess, "��SPҵ��״̬Ϊ��Ч,����ʹ��!SQLCODE[%d],sp_status[%s]",SQLCODE,vsp_status);
      pubLog_DBErr(_FFL_, "fDsmpSPBizCheck", "1302", return_mess);
      return 1302;
    }
  }

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpSPBizCheck", "", "-- end -- fDsmpSPBizCheck");
#endif

  return 0;
}



/**********************************************
 @FUNC   fDsmpProdOrderCheck  �û�������Ϣ�ظ��Լ��
 @input  params: sp_id ->    SP��ҵ����
 @input  params: biz_code -> SPҵ�����
 @input  params: biz_type -> SPҵ������
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpProdOrderCheck(char *id_no, char *third_id_no,char *sp_id, char *biz_code, char *return_mess)
{
  EXEC SQL BEGIN DECLARE SECTION;
      char temp_sql[1024];
        char vid_no[14+1];
        char vthird_id_no[14+1];
        char vsp_id[ 20 +1];
        char vbiz_code[21 +1];
    int  icount = 0;
    char vBizType[2+1];
    char vGprsType[1+1];
    char vGprsFlag[1+1];
    char vGprsMsg[100+1];
    long lid_no=0;
    long lthird_id_no=0;
  EXEC SQL END DECLARE SECTION;

    Sinitn(vid_no );
    Sinitn(vthird_id_no );
    Sinitn(vsp_id );
    Sinitn(vbiz_code );
    Sinitn(temp_sql );
    Sinitn(vBizType);
    Sinitn(vGprsType);
    Sinitn(vGprsFlag);
    Sinitn(vGprsMsg);

  	strcpy(vid_no, id_no);
  	strcpy(vthird_id_no, third_id_no);
  	strcpy(vsp_id, sp_id);
  	strcpy(vbiz_code, biz_code);
	/**add by zhangzhuo 20101022**/
	lid_no=atol(id_no);
	lthird_id_no=atol(vthird_id_no);
	/**add by zhangzhuo 20101022**/
    Trim(vid_no);
    Trim(vthird_id_no);
    Trim(vsp_id);
    Trim(vbiz_code);

  	pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "=========��ʼ����fDsmpProdOrderCheck������=========");

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "input vid_no = [%s]", vid_no);
  pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "input vthird_id_no = [%s]", vthird_id_no);
  pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "input vsp_id = [%s]", vsp_id);
  pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "input vbiz_code = [%s]", vbiz_code);
#endif

    if (  atol(third_id_no) > 0 )
    {
				/* �жϵ��������� �Ƿ������ҵ�� */
        Sinitn(temp_sql);
        icount = 0;
      	sprintf(temp_sql, " select count(*) from DDSMPORDERMSG%ld     "
                        " where id_no=:v1 and  spid = :v2 and  bizcode= :v3  "
                      " and sysdate between EFF_TIME and end_time and to_char(third_id_no) = '0' ",
              atol(vthird_id_no)%10L  );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "temp_sql = [%s]", temp_sql);
#endif
      EXEC SQL EXECUTE
      BEGIN
        EXECUTE IMMEDIATE :temp_sql INTO :icount using :lthird_id_no,:vsp_id ,:vbiz_code;
      END;
      END-EXEC;
      if(SQLCODE != OK  )
      {
        	sprintf(return_mess,"�жϵ��������� �Ƿ������ҵ��ʧ��[%d]!",SQLCODE);
        	pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "  [%s]", return_mess);
        	PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "-91", return_mess);
        	return -91;
      }
      if (icount > 0)
      {
        	sprintf(return_mess,"���������������ҵ��,�����ظ�����!" );
        	pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "return_mess =[%s]", return_mess);
        	pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vsp_id =[%s]", vsp_id);
        	pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vbiz_code =[%s]", vbiz_code);
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "1304", return_mess);
        	return 1304;
      }

            /* �жϵ��������� �Ƿ����͹���ҵ�� ��������ͬһ�������͹��� */
        Sinitn(temp_sql);
        icount = 0;
      sprintf(temp_sql, "select count(*) from DDSMPORDERMSG a "
     	"where  third_id_no= :v1 and  spid = :v2 and  bizcode= :v3 "
        "	and sysdate between EFF_TIME and end_time  " );
      EXEC SQL EXECUTE
      BEGIN
        EXECUTE IMMEDIATE :temp_sql INTO :icount using :lthird_id_no,:vsp_id ,:vbiz_code;
      END;
      END-EXEC;
      if(SQLCODE != OK  )
      {
        	sprintf(return_mess,"�жϵ��������� �����͹���ҵ��ʧ��[%d]!",SQLCODE);
        	PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "-93", return_mess);
        	return -93;
      }

      if (icount > 0 )
      {
        	sprintf(return_mess,"���������뱻���͹���ҵ��,�����ظ�������!" );
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "return_mess =[%s]", return_mess);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vsp_id =[%s]", vsp_id);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vbiz_code =[%s]", vbiz_code);
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "1304", return_mess);
        	return 1304;
      }
    }
    else
    	{
    		printf("�ж��û��Ƿ��������ҵ��\n");
        /** �ж��û��Ƿ��������ҵ��**/
        Sinitn(temp_sql);
        icount = 0;
      sprintf(temp_sql, "select count(*) from DDSMPORDERMSG%ld "
        "where  id_no=:v1 and  spid = :v2 and  bizcode= :v3 "
        "and sysdate between EFF_TIME and end_time and to_char(third_id_no) = '0' ",
              atol(vid_no)%10L  );
      EXEC SQL EXECUTE
      BEGIN
        EXECUTE IMMEDIATE :temp_sql INTO :icount using :lid_no,:vsp_id ,:vbiz_code;
      END;
      END-EXEC;
      if(SQLCODE != OK  )
      {
        	sprintf(return_mess,"�ж��û��Ƿ������ҵ��ʧ��[%d]!",SQLCODE);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "  [%s]", return_mess);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "temp_sql = [%s]",temp_sql);
	        PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "-95", return_mess);
        	return -95;
      }
      if (icount > 0 )
      {
      	if(strcmp(vsp_id,"WLAN01") != 0 || strcmp(vbiz_code,"00014") != 0)
      	{
      		printf("�û��������ҵ��,�����ظ�����!\n");
       		sprintf(return_mess,"�û��������ҵ��,�����ظ�����!" );
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "return_mess =[%s]", return_mess);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vsp_id =[%s]", vsp_id);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vbiz_code =[%s]", vbiz_code);
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "1304", return_mess);
        	return 1304;
        }
      }

        /** �ж��û��Ƿ� �����͹���ҵ��**/
        printf("�ж��û��Ƿ� �����͹���ҵ��vid_no=[%s]vsp_id=[%s]vbiz_code=[%s]\n",vid_no,vsp_id,vbiz_code);
        Sinitn(temp_sql);
        icount = 0;
      sprintf(temp_sql, " select count(*) from DDSMPORDERMSG%ld "
        "where third_id_no = :v1 and spid = :v2 and bizcode = :v3	"
        "and sysdate between EFF_TIME and end_time  ",atol(vid_no)%10L);
      printf("temp_sql=[%s]\n",temp_sql);
      EXEC SQL EXECUTE
      BEGIN
        EXECUTE IMMEDIATE :temp_sql INTO :icount using :lid_no,:vsp_id ,:vbiz_code;
      END;
      END-EXEC;

      if(SQLCODE != OK  )
      {
        	sprintf(return_mess,"�жϵ��������� �Ƿ������ҵ��ʧ��[%d]!",SQLCODE);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "temp_sql = [%s]",temp_sql);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "  [%s]", return_mess);
	        PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "-97", return_mess);
        	return -97;
      }

      if (icount > 0 )
      {
      		sprintf(return_mess,"�û������͹���ҵ��,�����ظ�����!" );
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "return_mess =[%s]", return_mess);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vsp_id =[%s]", vsp_id);
	        pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "vbiz_code =[%s]", vbiz_code);
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "1304", return_mess);
        	return 1304;
      }

    }
    printf("��Ҫ��ʾ��ͨGPRS\n");

    /** û�о���ʾ����ͨҵ����ʹ��GPRS����Ҫ��ʾ��ͨGPRS**/
    EXEC SQL SELECT NVL(a.GPRSTYPE,'0'),a.OPRCODE INTO :vGprsType ,:vBizType
               FROM SOBBIZTYPE_dsmp a,dDSmpSpInfo b
              WHERE a.OPRCODE= b.biztype
                AND b.spid = :vsp_id
                and sysdate between to_date(b.validdate,'yyyymmdd') and to_date(b.expiredate,'yyyymmdd');
    if (SQLCODE != 0)
    {
        strcpy(vGprsType,"0");
        PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    	pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "", vGprsType);
    }
    else if ((SQLCODE == 0) && (strcmp(vGprsType,"0") != 0))
    	{
        /*û�о���ʾ����ͨҵ����ʹ��GPRS�����û�û�п�ͨGPRS���뿪ͨ�� */
        EXEC SQL SELECT DECODE(COUNT(*),0,0,1) INTO :vGprsFlag
                   FROM DCUSTFUNC WHERE ID_NO= to_number(:vid_no) AND FUNCTION_CODE='55';
        if (SQLCODE != 0) {/*��ѯ����ʧ�ܻ������ݣ�����δ��ͨ����*/
            strcpy(vGprsFlag,"0");
            PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    			pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "", "");
        }
    }
		printf("��ҵ����������\n");
    if((atoi(vGprsType)==1) && (strcmp(vGprsFlag,"0") != 0))
    {  /*��ҵ����������*/
          EXEC SQL SELECT nvl(MSG,'��ͨ�ɹ�����ͨ��SPҵ��Ҫʹ�õ�GPRS���������û���ͨGPRS��') INTO :vGprsMsg
                     FROM sGPRSNOTIFYCFG WHERE BIZTYPE=:vBizType AND ROWNUM<2;
          if (SQLCODE == 0)
          {
                   sprintf(return_mess, "%s", vGprsMsg);
        	PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    	pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "", return_mess);
          }
    }

    if((atoi(vGprsType)==1) && (strcmp(vGprsFlag,"0") != 0))
    {  /*��SPID����*/
          EXEC SQL SELECT nvl(MSG,'��ͨ�ɹ�����ͨ��SPҵ��Ҫʹ�õ�GPRS���������û���ͨGPRS��') INTO :vGprsMsg
                     FROM sGPRSNOTIFYCFG WHERE SPID=:vsp_id AND ROWNUM<2;
          if (SQLCODE == 0)
          {
                sprintf(return_mess, "%s", vGprsMsg);
        		PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
	    		pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "", return_mess);
          }
    }
		printf("��ҵSPID��SPBIZCODE����\n");
    if((atoi(vGprsType)==3) && (strcmp(vGprsFlag,"0") != 0))
    {   /*��ҵSPID��SPBIZCODE����*/
          EXEC SQL SELECT nvl(MSG,'��ͨ�ɹ�����ͨ��SPҵ��Ҫʹ�õ�GPRS���������û���ͨGPRS��') INTO :vGprsMsg
                     FROM sGPRSNOTIFYCFG WHERE SPID=:vsp_id AND SPBIZCODE=:vbiz_code AND ROWNUM<2;
          if (SQLCODE == 0)
          {
				sprintf(return_mess, "%s", vGprsMsg);
	        	PUBLOG_DBDEBUG("fDsmpProdOrderCheck");
		    	pubLog_DBErr(_FFL_, "fDsmpProdOrderCheck", "", return_mess);
          }
    }

		#ifdef _DEBUG_
  			pubLog_Debug(_FFL_, "fDsmpProdOrderCheck", "", "-- end -- fDsmpProdOrderCheck");
		#endif

  return 0;
}

/**********************************************
 @FUNC   fDsmpGetOrderInfo     �û�������Ϣ��ѯ
 @input  params: pOrderData -> ��Ʒ������Ϣ
 @input  params: qry_code   -> ��ѯ��ʽ  00:����order_id��ѯ,01:����SPҵ�����,02 ԤԼ������03:��ѯ����ȷ����ʱ������Ϣ
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpGetOrderInfo(DSMPORDEROpr *pOrderData,char *qry_code,char *return_mess)
{
  EXEC SQL BEGIN DECLARE SECTION;
    char 	 TmpSqlStr[2048];
    long   order_id;
    char   serv_code[2+1];
    char   opr_code[2+1];
    char   third_id_no[14+1];
    char   third_phoneno[15+1];
    char   prod_code[21+1];
    char   comb_flag[1+1];
    char   mode_code[8+1];
    char   chg_flag[1+1];
    char   chrg_type[1+1];
    char   channel_id[6+1];
    char   opr_source[2+1];
    char   first_order_date[17+1];
    char   first_login_no[6+1];
    char   last_order_date[17+1];
    char   opr_time[17+1];
    char   eff_time[17+1];
    char   end_time[17+1];
    char   local_code[1+1];
    char   valid_flag[1+1];
    char   reserved[100+1];
    char   oprnumb[22+1];
    char   login_no[6+1];
    char   op_code[6+1];
    char   belong_code[8+1];
    char   total_date[8+1];
    char   login_accept[14+1];
    char   sys_note[100+1];
    char   op_note[100+1];

    char   id_no[14 +1];
    char   spid[20 +1];
    char   bizcode[21 +1];
    char   phone_no[15 +1];
    char   sm_code[2+1];
    char   password[32+1];

    char   temp_idno[14 +1];
    char   temp_phoneno[15 +1];
    char   temp_order_id[14+1];
    char   temp_qry_code[2+1];
    char   temp_spId[20+1];
    char   temp_spBizCode[21+1];
    char   temp_third_no[15 +1];

    char	 vphone_no[11+1];
    char 	 vthird_phoneno[11+1];
    char   vspid[20+1];
    char   vbizcode[21+1];
    char   vlogin_accept[14+1];
    char   vid_no[14+1];
  EXEC SQL   END DECLARE SECTION;

  #ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "-- begin -- fDsmpGetOrderInfo ");
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "qry_code    =[%s]",qry_code         );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id   =[%ld]", pOrderData->order_id   );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->id_no           =[%s]", pOrderData->id_no           );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->phone_no        =[%s]", pOrderData->phone_no        );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->serv_code       =[%s]", pOrderData->serv_code       );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->opr_code        =[%s]", pOrderData->opr_code        );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->third_id_no,    =[%s]", pOrderData->third_id_no    );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->third_phoneno   =[%s]", pOrderData->third_phoneno   );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->spid   =[%s]", pOrderData->spid   );
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->bizcode   =[%s]", pOrderData->bizcode   );
  #endif

  order_id =0            ;
  Sinitn(serv_code        );
  Sinitn(opr_code         );
  Sinitn(third_id_no      );
  Sinitn(third_phoneno    );
  Sinitn(prod_code        );
  Sinitn(comb_flag        );
  Sinitn(mode_code        );
  Sinitn(chg_flag         );
  Sinitn(chrg_type        );
  Sinitn(channel_id       );
  Sinitn(opr_source       );
  Sinitn(first_order_date );
  Sinitn(first_login_no   );
  Sinitn(last_order_date  );
  Sinitn(opr_time         );
  Sinitn(eff_time         );
  Sinitn(end_time         );
  Sinitn(local_code       );
  Sinitn(valid_flag       );
  Sinitn(reserved         );
  Sinitn(oprnumb          );
  Sinitn(login_no         );
  Sinitn(op_code          );
  Sinitn(belong_code      );
  Sinitn(total_date       );
  Sinitn(login_accept     );
  Sinitn(sys_note         );
  Sinitn(op_note          );
  Sinitn(temp_idno);
  Sinitn(temp_phoneno);
  Sinitn(id_no);
  Sinitn(spid);
  Sinitn(bizcode);
  Sinitn(phone_no);
  Sinitn(sm_code);
  Sinitn(password);
  Sinitn(temp_order_id);
  Sinitn(temp_qry_code);
  Sinitn(temp_spId);
  Sinitn(temp_spBizCode);
  Sinitn(temp_third_no);

  strcpy(temp_qry_code    ,qry_code );
  Sinitn(TmpSqlStr);
  Sinitn(vphone_no);
  Sinitn(vthird_phoneno);
  Sinitn(vspid);
  Sinitn(vbizcode);
  Sinitn(vlogin_accept);
  Sinitn(vid_no);

    /** ��Բ���û��SPID����SPBizCode��ҵ����д���,��ʼ��Ϊϵͳ�����ȱʡֵ **/
    EXEC SQL  SELECT nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ')
      into  :temp_spId,:temp_spBizCode
    FROM  SOBBIZTYPE_dsmp
   WHERE  oprcode = :pOrderData->serv_code ;
    if  ((SQLCODE != 0) &&  (SQLCODE != 1403) )
    {
      sprintf(return_mess, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", pOrderData->serv_code,SQLCODE);
	  	PUBLOG_DBDEBUG("fDsmpGetOrderInfo");
	  	pubLog_DBErr(_FFL_, "fDsmpGetOrderInfo", "-5", return_mess);
      return -5;
    }

    Trim(temp_spId);
    Trim(temp_spBizCode);
    Trim(pOrderData->spid);
    /**chendh 2009.01.09������ʱ�޸�Ϊ֧��16,14����**/
    if (strlen(temp_spId) > 0 && strlen(pOrderData->spid)==0 )
    {
      strcpy(pOrderData->spid,temp_spId);   /**��ʼ��Ϊϵͳ�����ȱʡֵ **/
      strcpy(pOrderData->send_spid," ");
    }
    else
    {
      strcpy(pOrderData->send_spid,pOrderData->spid);
    }

  if (strlen(temp_spBizCode) > 0 && strlen(pOrderData->bizcode)==0 )
  {
      strcpy(pOrderData->bizcode,temp_spBizCode);   /**��ʼ��Ϊϵͳ�����ȱʡֵ **/
      strcpy(pOrderData->send_bizcode," ");
  }
  else
  {
      strcpy(pOrderData->send_bizcode,pOrderData->bizcode);
  }

  strcpy(vphone_no, pOrderData->phone_no);
  strcpy(vthird_phoneno,  pOrderData->third_phoneno);
  strcpy(vspid, pOrderData->spid);
  strcpy(vbizcode,pOrderData->bizcode);
  strcpy(vlogin_accept,pOrderData->login_accept);
  strcpy(vid_no,pOrderData->id_no);

    if (strcmp(temp_qry_code,"00") == 0 )
    {		/*00:����order_id��ѯ*/
        sprintf(temp_order_id,"%ld",pOrderData->order_id);
		#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
    		pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "temp_order_id= [%s]", temp_order_id);
		#endif

    sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source,' ')  ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' '),NVL(substr(password,1,32),' ' )"
              " from DDSMPORDERMSG   "
          " where order_id =to_number(:v1)  "  );
  EXEC SQL EXECUTE
  BEGIN
    EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              :third_phoneno,:prod_code,:comb_flag,:mode_code,
              :chg_flag,:chrg_type,:channel_id,:opr_source  ,
              :first_order_date,:first_login_no,:last_order_date ,
              :opr_time,:eff_time,:end_time ,
              :local_code,:valid_flag,:reserved ,
              :belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
       				using :temp_order_id ;
  END;
  END-EXEC;
/**2009.8.3ע��**/
/**
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "TmpSqlStr = [%s]", TmpSqlStr);
#endif
***/
    }
    else if (strcmp(temp_qry_code,"02") == 0 )
    { 	/*02 ԤԼ����*/
        sprintf(temp_order_id,"%ld",pOrderData->order_id);
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "temp_order_id= [%s]", temp_order_id);
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->login_accept= [%s]", pOrderData->login_accept);
#endif

    sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source,' ')  ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' '),NVL(substr(password,1,32),' ' )"
              " from PDSMPORDERPENDING   "
          " where order_id =to_number(:v1) and login_accept  =to_number(:v2)"  );
  EXEC SQL EXECUTE
  BEGIN
    EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              :third_phoneno,:prod_code,:comb_flag,:mode_code,
              :chg_flag,:chrg_type,:channel_id,:opr_source  ,
              :first_order_date,:first_login_no,:last_order_date ,
              :opr_time,:eff_time,:end_time ,
              :local_code,:valid_flag,:reserved ,
              :belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
       using :temp_order_id ,:vlogin_accept ;
  END;
  END-EXEC;
/**2009.8.3ע��**/
/**
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "TmpSqlStr = [%s]", TmpSqlStr);
#endif
***/

    }
    else if (strcmp(temp_qry_code,"01") == 0 )
    	{		/*01:����SPҵ�����*/
        sprintf(temp_third_no,"%s",pOrderData->third_phoneno);
        Trim(temp_third_no);
        Trim(pOrderData->third_phoneno);
        pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "����������temp_third_no=[%s],pOrderData->third_phoneno=[%s]",temp_third_no,pOrderData->third_phoneno);
        memset(TmpSqlStr,0,sizeof(TmpSqlStr));
        if (strlen(temp_third_no) > 1)
				{
        	sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source,' ')  ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' '),NVL(substr(password,1,32),' ' ) "
              " from DDSMPORDERMSG%ld   "
          " where phone_no = :v1 AND third_phoneno = :v2 and third_id_no > 0 "
          " and spid = :v3 and bizcode = :v4   "
        " and sysdate between EFF_TIME and end_time  ",
            atol(pOrderData->id_no )%10L  );

  			EXEC SQL EXECUTE
  				BEGIN
    					EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              	:third_phoneno,:prod_code,:comb_flag,:mode_code,
              	:chg_flag,:chrg_type,:channel_id,:opr_source  ,
              	:first_order_date,:first_login_no,:last_order_date ,
              	:opr_time,:eff_time,:end_time ,
              	:local_code,:valid_flag,:reserved ,
              	:belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
       				using :vphone_no ,:vthird_phoneno ,:vspid ,:vbizcode ;
  				END;
  			END-EXEC;
			}else
			{
					/**chendh 2009.3.5���޸Ĳ�ѯ���� and third_id_no = 0---->to_char(third_id_no)='0' **/
					pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "vspid=[%s],vbizcode=[%s]",vspid,vbizcode);
					sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source,' ')  ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' '),NVL(substr(password,1,32),' ' ) "
              " from DDSMPORDERMSG%ld   "
          " where id_no=to_number(:v1) "
          " and spid = :v2 and bizcode = :v3  "
        	" and sysdate between EFF_TIME and end_time and opr_code not in ('02','07','89','99') ",atol(pOrderData->id_no)%10L );
  				EXEC SQL EXECUTE
  					BEGIN
    					EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              	:third_phoneno,:prod_code,:comb_flag,:mode_code,
              	:chg_flag,:chrg_type,:channel_id,:opr_source  ,
              	:first_order_date,:first_login_no,:last_order_date ,
              	:opr_time,:eff_time,:end_time ,
              	:local_code,:valid_flag,:reserved ,
              	:belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
       				using :vid_no ,:vspid ,:vbizcode ;
  					END;
  				END-EXEC;
  				/**cdh 2009.8.3 ��12580Ͷ��ע��**/
  				/*pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "��ѯ���TmpSqlStr=[%s],SQLCODE=[%d],vid_no=[%s],vspid=[%s],vbizcode=[%s]",TmpSqlStr,SQLCODE,vid_no,vspid,vbizcode);*/

			}

    }
    else if (strncmp(temp_qry_code,"03",2) == 0 )
    {		/*03:��ѯ����ȷ����ʱ������Ϣ*/
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "TmpSqlStr = [%s]", TmpSqlStr);
#endif
        sprintf(temp_order_id,"%ld",pOrderData->order_id);

    sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source,' ') ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' '),NVL(substr(password,1,32),' ' )"
              " from PDSMPRECONFIRMINFO   "
          " where order_id = to_number(:v1)  "
        " and sysdate between EFF_TIME and end_time   " );
  EXEC SQL EXECUTE
  BEGIN
    EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              :third_phoneno,:prod_code,:comb_flag,:mode_code,
              :chg_flag,:chrg_type,:channel_id,:opr_source  ,
              :first_order_date,:first_login_no,:last_order_date ,
              :opr_time,:eff_time,:end_time ,
              :local_code,:valid_flag,:reserved ,
              :belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
        using :temp_order_id ;
  END;
  END-EXEC;
    }

  if ((SQLCODE != OK  ) && (SQLCODE != 1403  ))
  {
    	sprintf(return_mess,"��ȡ�û���Ʒ������Ϣʧ��[%d]!",SQLCODE);
	    PUBLOG_DBDEBUG("fDsmpGetOrderInfo");
	  	pubLog_DBErr(_FFL_, "fDsmpGetOrderInfo", "-1", return_mess);
    	return -1 ;
  }
  else if ((SQLCODE == 1403  ) && (strcmp(temp_qry_code,"01") == 0 ) )
  	{
    Sinitn(TmpSqlStr);
      sprintf(TmpSqlStr, " select order_id,serv_code,opr_code,NVL(third_id_no,0 ),  "
            " NVL(third_phoneno,' '),NVL(prod_code,'  '),comb_flag,NVL(mode_code,' ' ) ,  "
            " chg_flag,chrg_type,NVL(channel_id,'99'),nvl(opr_source ,' ') ,    "
            " to_char(first_order_date,'yyyymmdd hh24:mi:ss'),NVL(first_login_no,' '), "
            " to_char(last_order_date,'yyyymmdd hh24:mi:ss') , "
            " to_char(opr_time,'yyyymmdd hh24:mi:ss'),to_char(eff_time,'yyyymmdd hh24:mi:ss'), "
            " to_char(end_time,'yyyymmdd hh24:mi:ss') ,  "
            " local_code,valid_flag,NVL(reserved, ' ' ) ,   "
            " belong_code,total_date,login_accept ,id_no,spid, bizcode ,phone_no,NVL(sm_code,' ') ,NVL(substr(password,1,32),' ' )"
              " from DDSMPORDERMSG    "
          " where ( third_id_no=to_number(:v1)  OR ( phone_no = :v2 AND third_phoneno = :v3 ) ) "
          " and spid = :v4 and bizcode = :v5   "
        " and sysdate between EFF_TIME and end_time and third_id_no > 0 and rownum  <2 " );

    EXEC SQL EXECUTE
    BEGIN
      EXECUTE IMMEDIATE :TmpSqlStr INTO :order_id,:serv_code,:opr_code,:third_id_no ,
              :third_phoneno,:prod_code,:comb_flag,:mode_code,
              :chg_flag,:chrg_type,:channel_id,:opr_source  ,
              :first_order_date,:first_login_no,:last_order_date ,
              :opr_time,:eff_time,:end_time ,
              :local_code,:valid_flag,:reserved ,
              :belong_code,:total_date,:login_accept ,:id_no,:spid, :bizcode,:phone_no,:sm_code,:password
          using :vid_no,:vphone_no ,:vthird_phoneno , :vspid ,:vbizcode ;
    END;
    END-EXEC;

    if(SQLCODE != OK && SQLCODE != 1403)
    {
      		sprintf(return_mess,"��ȡ�û���Ʒ������Ϣʧ��[%d]!",SQLCODE);
      		PUBLOG_DBDEBUG("fDsmpGetOrderInfo");
	  		pubLog_DBErr(_FFL_, "fDsmpGetOrderInfo", "-2", return_mess);
      		return -2 ;
    }
     else if(SQLCODE == 1403  )
    {
      	sprintf(return_mess,"û���ҵ��û��Ĳ�Ʒ������Ϣ[%d]!",SQLCODE);
      	PUBLOG_DBDEBUG("fDsmpGetOrderInfo");
	  	pubLog_DBErr(_FFL_, "fDsmpGetOrderInfo", "-3", return_mess);
      	return -3;
    }
    /***
    Trim(third_phoneno);
    EXEC SQL  SELECT to_char(id_no)
                  INTO :temp_idno
              FROM dCustMsg
         WHERE phone_no = :third_phoneno
           AND substr(run_code,2,1) < 'a';
      if  (SQLCODE != 0)
      {
        sprintf(return_mess, "��ȡ�������û��ı�dCustMsg����[%s][%d]!", temp_phoneno,SQLCODE);
        return -4;
      }
      Trim(temp_idno);

    strcpy(pOrderData->third_id_no    ,pOrderData->id_no  );
    strcpy(pOrderData->third_phoneno  ,pOrderData->phone_no  );
      strcpy(pOrderData->id_no          ,temp_idno  );
    strcpy(pOrderData->phone_no       ,temp_phoneno  );    ***/
  }
  /**else {
    strcpy(pOrderData->third_id_no    ,"0"  );
    strcpy(pOrderData->third_phoneno  ,"0"  );
  }**/
  else if ((SQLCODE == 1403  ) && (strcmp(temp_qry_code,"01") != 0 ) )
  {
		sprintf(return_mess,"û���ҵ��û��Ĳ�Ʒ������Ϣ[%d]!",SQLCODE);
		pubLog_DBErr(_FFL_, "fDsmpGetOrderInfo", "-6", return_mess);
		return -6 ;
  }

  Trim(serv_code        );
  Trim(opr_code         );
  Trim(prod_code        );
  Trim(comb_flag        );
  Trim(mode_code        );
  Trim(chg_flag         );
  Trim(chrg_type        );
  Trim(channel_id       );
  Trim(opr_source       );
  Trim(first_order_date );
  Trim(first_login_no   );
  Trim(last_order_date  );
  Trim(opr_time         );
  Trim(eff_time         );
  Trim(end_time         );
  Trim(local_code       );
  Trim(valid_flag       );
  Trim(reserved         );
  Trim(oprnumb          );
  Trim(login_no         );
  Trim(op_code          );
  Trim(belong_code      );
  Trim(total_date       );
  Trim(login_accept     );
  Trim(sys_note         );
  Trim(op_note          );

  Trim(id_no );
  Trim(spid);
  Trim(bizcode );
  Trim(phone_no);
  Trim(sm_code);
  Trim(password);
  Trim(third_id_no);
    Trim(third_phoneno);
    Trim(belong_code);

  pOrderData->order_id = order_id            ;
  strcpy(pOrderData->serv_code        ,serv_code        );
  strcpy(pOrderData->opr_code         ,opr_code         );
  strcpy(pOrderData->prod_code        ,prod_code        );
  strcpy(pOrderData->comb_flag        ,comb_flag        );
  strcpy(pOrderData->mode_code        ,mode_code        );
  strcpy(pOrderData->chg_flag         ,chg_flag         );
  strcpy(pOrderData->chrg_type        ,chrg_type        );
  strcpy(pOrderData->channel_id       ,channel_id       );
/** strcpy(pOrderData->opr_source       ,opr_source       ); **/
  strcpy(pOrderData->first_order_date ,first_order_date );
  strcpy(pOrderData->first_login_no   ,first_login_no   );
  strcpy(pOrderData->last_order_date  ,last_order_date  );
/** strcpy(pOrderData->opr_time         ,opr_time         );  **/
  strcpy(pOrderData->eff_time         ,eff_time         );
  strcpy(pOrderData->end_time         ,end_time         );
  strcpy(pOrderData->local_code       ,local_code       );
  strcpy(pOrderData->valid_flag       ,valid_flag       );
  strcpy(pOrderData->reserved         ,reserved         );
/** strcpy(pOrderData->oprnumb          ,oprnumb          );
  strcpy(pOrderData->login_no         ,login_no         );
  strcpy(pOrderData->op_code          ,op_code          );
  strcpy(pOrderData->belong_code      ,belong_code      );
  strcpy(pOrderData->total_date       ,total_date       );
  strcpy(pOrderData->login_accept     ,login_accept     );
  strcpy(pOrderData->sys_note         ,sys_note         );
  strcpy(pOrderData->op_note          ,op_note          ); **/
  strcpy(pOrderData->id_no        ,id_no   );
  strcpy(pOrderData->spid         ,spid    );
  strcpy(pOrderData->bizcode      ,bizcode );
  strcpy(pOrderData->phone_no     ,phone_no );
  strcpy(pOrderData->sm_code      ,sm_code );
  strcpy(pOrderData->password     ,password );
  strcpy(pOrderData->third_phoneno     ,third_phoneno );
  strcpy(pOrderData->third_id_no     ,third_id_no );
  strcpy(pOrderData->belong_code     ,belong_code );

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "pOrderData->order_id = [%ld]", pOrderData->order_id);
  pubLog_Debug(_FFL_, "fDsmpGetOrderInfo", "", "-- end -- fDsmpGetOrderInfo ");
#endif

  return 0;
}

/**********************************************
 @FUNC   fPubDsmpReConfirm     �û�����ȷ����Ϣ����
 @input  params: pOrderData -> ��Ʒ������Ϣ
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fPubDsmpReConfirm(DSMPORDEROpr *pOrderData, char *retMsg)
{
  int i = 0;
  int retCode = 0;
  EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[1024];
    long llogin_accept =0  ;
    char vthird_no[11 +1];
    char vopr_flag[1 +1 ];
    char vid_no[14 +1];
    char orderpoint[1+1];
    char ordersync[1+1];
    char ordersynctable[30+1];
    char sTmpTime[5][14 + 1];

    char temp_spId[20+1];
    char temp_spBizCode[21+1];

    int  vthirdno_num = 0;
    char temp_begin_time[17+1];
    char temp_end_time[17+1];
    char vprod_code[20+1];
    char vback_code[512 +1];
    long vtime_limit =0;
    char vrsp_accept[14+1];
    char vcontent[1000];
    char vcontent2[1000];
    char vsrc_no[22 +1];
    char temp_first_time[17+1];
    int first_order_limit  = 0 ;
    int count_index;
    char vphone_no[15+1];
    char vthird_phoneno[11+1];
    char vspid[20+1];
    char vbizcode[21+1];
    char vserv_code[2+1];
    char seq[6+1];
    char monthdate[6+1];
    char temp_opr_code[2+1];
    int smModelId = 0;
  EXEC SQL END DECLARE SECTION;

  	Sinitn(temp_first_time);
    Sinitn(vthird_no);
    Sinitn(vopr_flag);
    Sinitn(vid_no);
    Sinitn(temp_begin_time);
    Sinitn(temp_end_time);
    Sinitn(vprod_code);
    Sinitn(vback_code);
    Sinitn(vrsp_accept);
    Sinitn(vcontent);
    Sinitn(vcontent2);
    Sinitn(vsrc_no);
    Sinitn(temp_spId  );
    Sinitn(temp_spBizCode  );
    Sinitn(orderpoint);
    Sinitn(ordersync);
    Sinitn(ordersynctable);
    memset(sTmpTime, 0, sizeof(sTmpTime));
  	Sinitn(vphone_no);
  	Sinitn(vthird_phoneno);
  	Sinitn(vspid);
  	Sinitn(vbizcode);
  	Sinitn(vserv_code);
  	Sinitn(temp_opr_code);
  	strcpy(vthird_no,pOrderData->third_phoneno);
  	strcpy(vid_no,pOrderData->id_no);
  	/*add by hejt ���Ӵӱ���ȡ����ˮ������ֻ֧����λ��ˮ*/
  	Sinitn(vphone_no);
  	Sinitn(monthdate);
  	Sinitn(seq);
  	strcpy(vphone_no,pOrderData->phone_no);
  	Trim(vphone_no);
  	EXEC SQL select to_char(sysdate,'yyyymm') into :monthdate from dual;
  	Trim(monthdate);

  	 /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       int ret =0;
       EXEC SQL BEGIN DECLARE SECTION;
    	   char vGroupId[10+1];
       EXEC SQL END DECLARE SECTION;
       init(vGroupId);
       ret = sGetUserGroupid(vphone_no,vGroupId);
       if(ret <0)
       {
         	  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "��ȡ�����û�group_idʧ��!");
         	  pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "200919","ret=[%d]",ret);
         	  return 200919;
       }
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/

  EXEC SQL select seq into :seq from oboss.dreconfirmseq where phone_no=:vphone_no;
  if((SQLCODE != 0)&&(SQLCODE !=1403))
  {
    sprintf(retMsg, "��ȡ��dreconfirmseq����[%s][%d]!", vphone_no,SQLCODE);
    	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
    	pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-14", retMsg);
    return -14;
  }
  else if(SQLCODE ==1403)
  {
    Sinitn(seq);
    strcpy(seq,"200");
    EXEC SQL insert into oboss.dreconfirmseq(phone_no,seq) values (:vphone_no,'201');
    if(SQLCODE != 0)
    {
      	sprintf(retMsg, "����dreconfirmseq����[%s][%d]!", vphone_no,SQLCODE);
      	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
      	pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-14", retMsg);
      	return -14;
    }
  }
  else
  {
    Trim(seq);
    if(strcmp(seq,"299")==0)
    {
      EXEC SQL update oboss.dreconfirmseq set seq='200' where phone_no=:vphone_no;
      if(SQLCODE != 0)
      {
      		sprintf(retMsg, "����dreconfirmseqΪ0����[%s][%d]!", vphone_no,SQLCODE);
	      	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
      		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-14", retMsg);
        	return -14;
      }
    }
    else
    {
      EXEC SQL update oboss.dreconfirmseq set seq=seq+1 where phone_no=:vphone_no;
      if(SQLCODE != 0)
      {
	        sprintf(retMsg, "����dreconfirmseq��1����[%s][%d]!", vphone_no,SQLCODE);
	        pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-14", retMsg);
	        return -14;
      }
    }
  }
  /**chendh 2008.11.29�޸Ķ�������**/
  /*
    if(strlen(seq)==1)
      sprintf(vrsp_accept,"1008620%s",seq);
    else
      sprintf(vrsp_accept,"10086%s",seq);
	*/
		sprintf(vrsp_accept,"%06d", rand()%900000+100000);
    memset(dynStmt,0,sizeof(dynStmt));
    count_index=0;
    sprintf(dynStmt, " select count(1) from WDSMPRECONFIRM%s  where ((phone_no=:v1 and THIRD_ID_NO = 0) or THIRD_PHONENO = :v2 ) "
                  " and rsp_accept = to_number(:v3) and CONFIRM_FLAG  = '0' ",  monthdate );
    EXEC SQL EXECUTE
      BEGIN
        EXECUTE IMMEDIATE :dynStmt INTO :count_index  using :vphone_no, :vphone_no,:vrsp_accept;
      END;
    END-EXEC;
    if(SQLCODE != 0  )
    {
	      sprintf(retMsg, "��ȡ����ȷ����ˮ�����WDSMPRECONFIRM����[%s][%d]!", vphone_no,SQLCODE);
	      pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-1", retMsg);
	      return -1;
    }
    if(count_index!=0)
    {
       memset(dynStmt,0,sizeof(dynStmt));
       sprintf(dynStmt, " insert into WDSMPRECONFIRM_INVALID(ORDER_ID,ID_NO,PHONE_NO,SERV_CODE,OPR_CODE,THIRD_ID_NO,THIRD_PHONENO,SPID,BIZCODE,OPR_SOURCE,OPR_TIME,BACK_CODE,"
       "TIME_LIMIT,SMS_CONT,EXTEND_CODE,RSP_ACCEPT,OVER_TIME_DUR,RSP_TIME,RSP_SMS_CONT,RESERVED,LOGIN_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,OP_NOTE,CONFIRM_FLAG,DEALTIME)"
       "select ORDER_ID,ID_NO,PHONE_NO,SERV_CODE,OPR_CODE,THIRD_ID_NO,THIRD_PHONENO,SPID,BIZCODE,OPR_SOURCE,OPR_TIME,BACK_CODE, "
       "TIME_LIMIT,SMS_CONT,EXTEND_CODE,RSP_ACCEPT,OVER_TIME_DUR,RSP_TIME,RSP_SMS_CONT,RESERVED,LOGIN_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,OP_NOTE,CONFIRM_FLAG,sysdate "
       "from WDSMPRECONFIRM%s  where ((phone_no=:v1 and THIRD_ID_NO = 0) or THIRD_PHONENO = :v2 ) "
                  " and rsp_accept = to_number(:v3) and CONFIRM_FLAG  = '0' ",  monthdate );
      EXEC SQL EXECUTE
        BEGIN
          EXECUTE IMMEDIATE :dynStmt  using :vphone_no, :vphone_no,:vrsp_accept;
        END;
      END-EXEC;
      if(SQLCODE != 0  )
      {
	        sprintf(retMsg, "�������ȷ����Ч�����WDSMPRECONFIRM����[%s][%d]!", vphone_no,SQLCODE);
			PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-1", retMsg);
	        return -1;
      }
       memset(dynStmt,0,sizeof(dynStmt));
        sprintf(dynStmt, " delete from WDSMPRECONFIRM%s  where ((phone_no=:v1 and THIRD_ID_NO = 0) or THIRD_PHONENO = :v2 ) "
                  " and rsp_accept = to_number(:v3) and CONFIRM_FLAG  = '0' ",  monthdate );
        EXEC SQL EXECUTE
          BEGIN
            EXECUTE IMMEDIATE :dynStmt  using :vphone_no, :vphone_no,:vrsp_accept;
          END;
        END-EXEC;
      if(SQLCODE != 0  )
      {
	        sprintf(retMsg, "ɾ������ȷ����ˮ�����WDSMPRECONFIRM[%s]����[%s][%d]!", monthdate,vphone_no,SQLCODE);
	    	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-1", retMsg);
	        return -1;
      }
    }
  /*add by hejt end*/
    /**int a=rand()%900000+100000; **/
#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "-- begin -- fPubDsmpReConfirm ");
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "vrsp_accept     =[%s]", vrsp_accept        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "rand     =[%d]", rand()%900000+100000  );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->order_id        =[%ld]", pOrderData->order_id        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->id_no           =[%s]", pOrderData->id_no           );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->phone_no        =[%s]", pOrderData->phone_no        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->serv_code       =[%s]", pOrderData->serv_code       );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->opr_code        =[%s]", pOrderData->opr_code        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->third_id_no,    =[%s]", pOrderData->third_id_no    );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->third_phoneno   =[%s]", pOrderData->third_phoneno   );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->spid            =[%s]", pOrderData->spid            );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->bizcode         =[%s]", pOrderData->bizcode         );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->prod_code       =[%s]", pOrderData->prod_code       );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->comb_flag       =[%s]", pOrderData->comb_flag       );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->mode_code       =[%s]", pOrderData->mode_code       );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->chg_flag        =[%s]", pOrderData->chg_flag        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->chrg_type       =[%s]", pOrderData->chrg_type       );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->channel_id      =[%s]", pOrderData->channel_id      );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->opr_source      =[%s]", pOrderData->opr_source      );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->first_order_date=[%s]", pOrderData->first_order_date);
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->first_login_no  =[%s]", pOrderData->first_login_no  );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->last_order_date =[%s]", pOrderData->last_order_date );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->opr_time        =[%s]", pOrderData->opr_time        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->eff_time        =[%s]", pOrderData->eff_time        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->end_time        =[%s]", pOrderData->end_time        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->local_code      =[%s]", pOrderData->local_code      );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->valid_flag      =[%s]", pOrderData->valid_flag      );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->belong_code     =[%s]", pOrderData->belong_code     );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->total_date      =[%s]", pOrderData->total_date      );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->login_accept    =[%s]", pOrderData->login_accept    );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->reserved        =[%s]", pOrderData->reserved        );
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->op_code	        =[%s]", pOrderData->op_code);
#endif
  /** ��ȡ����ȷ�ϴ�����Ϣ**/
  EXEC SQL  SELECT  back_code,time_limit  into :vback_code,:vtime_limit
              FROM SDSMPRECONFIRM
         WHERE SPID  = :pOrderData->spid
             AND BIZCODE   =:pOrderData->bizcode
                   AND OPR_SOURCE  =:pOrderData->opr_source
                   AND OP_CODE = :pOrderData->op_code ;
    if (SQLCODE != 0)
    {
      	sprintf(retMsg, "��ȡ����ȷ�ϴ����SDSMPRECONFIRM����[%s][%d]!", pOrderData->spid,SQLCODE);
      	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-1", retMsg);
      	return -1;
    }
  Trim(vback_code);

  EXEC SQL  SELECT SMSPORT ,ORDERPOINT,ORDERSYNC,nvl(ORDERSYNCTABLE,'  ' ) ,
                     nvl(DEFAULT_SPID,' '),nvl(DEFAULT_BIZCODE,' ') ,nvl(FIRST_ORDER_LIMIT , 0)
                into :vsrc_no ,:orderpoint,:ordersync,:ordersynctable,
                     :temp_spId,:temp_spBizCode ,:first_order_limit
              FROM SOBBIZTYPE_dsmp
         WHERE OPRCODE  =:pOrderData->serv_code  ;
  if (SQLCODE != 0)
  {
    	sprintf(retMsg, "��ȡ��SOBBIZTYPE_dsmp����[%s][%d]!", pOrderData->spid,SQLCODE);
	  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-14", retMsg);
    	return -14;
  }

  Trim(vsrc_no);
  Trim(temp_spId);
  Trim(temp_spBizCode);

  if (strlen(temp_spId) > 0 )
  {
      strcpy(pOrderData->send_spid," ");
  }
  else
  {
      strcpy(pOrderData->send_spid,pOrderData->spid);
  }

  if (strlen(temp_spBizCode) > 0 )
  {
      strcpy(pOrderData->send_bizcode," ");
  }
  else
  {
      strcpy(pOrderData->send_bizcode,pOrderData->bizcode);
  }

  Trim(ordersynctable);
  if  (( strlen(ordersynctable) <= 0 )  && ( strcmp(ordersync,"1") == 0 ))
  {

  	pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "ordersynctable =[%s]", ordersynctable);
    sprintf(retMsg, "SOBBIZTYPE_dsmp�е�ordersynctable�ֶβ���Ϊ��[%s][%d]!", ordersynctable,SQLCODE);
	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
	pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-51", retMsg);
    return -51;

  }
  strcpy(vspid, pOrderData->spid);
  strcpy(vbizcode,pOrderData->bizcode);
  strcpy(vserv_code,pOrderData->serv_code);
  strcpy(vthird_phoneno,pOrderData->third_phoneno);

  /** ��ȡ�û������Ĳ�Ʒ��Ϣ
  2011/10/24  �ֻ�����ҵ����� zhengbin ��� 36 01����
  **/
  if ((strncmp(pOrderData->opr_code,"06",2) == 0)
      || (strncmp(pOrderData->opr_code,"11",2) == 0)
      || (strncmp(pOrderData->opr_code,"21",2) == 0)
      || (strncmp(pOrderData->opr_code,"10",2) == 0)
      || (strncmp(pOrderData->opr_code,"12",2) == 0)
      || ((strncmp(pOrderData->opr_code,"01",2) == 0)&& strcmp(pOrderData->serv_code,"31")==0)
      || ((strncmp(pOrderData->opr_code,"01",2) == 0)&& strcmp(pOrderData->serv_code,"02")==0)
      || ((strncmp(pOrderData->opr_code,"01",2) == 0)&& strcmp(pOrderData->serv_code,"36")==0)
      || ((strncmp(pOrderData->opr_code,"01",2) == 0)&& strcmp(pOrderData->serv_code,"41")==0)
      || ( (strncmp(pOrderData->opr_code, "01", 2) == 0)) && (strncmp(pOrderData->serv_code, "21", 2) == 0) )
    {   /**06-���� 11-���� 21-����ȷ�� (10-�ײͶ��� 12-�ײͱ��)WLAN-02**/

  		/** ��ȡ�û��Ĳ�Ʒ��Ϣ **/
  		EXEC SQL  SELECT a.prod_code,to_char(a.begin_time,'yyyymmdd hh24:mi:ss'),to_char(a.end_time,'yyyymmdd hh24:mi:ss')
  		              INTO :vprod_code,:temp_begin_time,:temp_end_time
  		          FROM SDSMPPRODCODE a,SDSMPPRODBIZ b
  		     WHERE a.PROD_CODE = b.PROD_CODE
  		       AND b.SPID = :pOrderData->spid
  		       AND b.BIZCODE = :pOrderData->bizcode
  		       AND sysdate between b.BEGIN_TIME  and b.END_TIME
  		       AND sysdate between a.BEGIN_TIME  and a.END_TIME;
  		if (SQLCODE != 0)
  		{
  		  sprintf(retMsg, "��ȡSPҵ����Ϣ��SDSMPPRODBIZ����[%s][%d]!", pOrderData->spid,SQLCODE);
		  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-5", retMsg);
  		  return -5;
  		}
  		if (SQLCODE == 1403 )
  		{
  		  	sprintf(retMsg, "SPҵ����Ϣ��SDSMPPRODBIZ������[%s][%d]!", pOrderData->spid,SQLCODE);
		  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-6", retMsg);
  		  	return -6;
  		}
  		Trim(vprod_code);
  		Trim(temp_begin_time);
  		Trim(temp_end_time);

  		if (strlen(temp_end_time) < 17 )
  		{
  		    strcpy(temp_end_time ,"20500101 00:00:00" );
  		}

  		strcpy(pOrderData->prod_code,vprod_code );
  		strcpy(pOrderData->eff_time ,pOrderData->opr_time  );
  		strcpy(pOrderData->end_time ,temp_end_time  );

  }
  printf("\npOrderData->eff_time=[%s],pOrderData->end_time=[%s]\n",pOrderData->eff_time,pOrderData->end_time);

  /** ֻ�ж��š�����ҵ����ܹ����� **/
  if (strcmp(pOrderData->opr_code,"11") ==0 )
  {
    	if ((strcmp(pOrderData->serv_code,"04") !=0 ) && (strcmp(pOrderData->serv_code,"05") !=0 ))
    	 {
    	  	sprintf(retMsg, "ֻ�ж��š����Ų��ܹ�ҵ������[%s]!", pOrderData->serv_code);
	  	  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-7", retMsg);
    	  	return -7;
    	}
    	EXEC SQL  SELECT THIRDNO_NUM  INTO :vthirdno_num
    	        FROM CDSMPTHIRDNONUM
    	      WHERE region_code = substr(:pOrderData->belong_code,1,2)
    	        AND biz_type = :pOrderData->serv_code ;
    	if (SQLCODE != 0)
    	{
    	  	sprintf(retMsg, "��ȡ��CDSMPTHIRDNONUM����[%d]!",SQLCODE);
	  	  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-8", retMsg);
    	 	 return -8;
    	}
    	Sinitn(dynStmt);
    	sprintf(dynStmt, "select count(*) from DDSMPORDERMSG%ld a   "
    			"where a.id_no=to_number(:v1) and a.SERV_CODE = :v2    "
    			"and sysdate between EFF_TIME and end_time and third_id_no > 0 ", atol(pOrderData->id_no)%10L  );

    	EXEC SQL EXECUTE
    	BEGIN
    	  EXECUTE IMMEDIATE :dynStmt INTO :i using :vid_no,:vserv_code  ;
    	END;
    	END-EXEC;

    	if(SQLCODE != OK  ){
    	  	sprintf(retMsg,"��ȡ�û���ҵ��������Ϣ����[%d]!",SQLCODE);
	  	    PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-9", retMsg);
    	    return -9;
    	}

    	if (i > vthirdno_num)
    	{
    	  	sprintf(retMsg, "�����˿���ҵ�����͵�����,���ܽ���ҵ������[%d]!", vthirdno_num);
	  	  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-10", retMsg);
    	  	return -10;
    	}

    	/** ��ȡ�û���һ�ζ�����Ʒ��ʱ�� **/
    	memset(dynStmt, 0, sizeof(dynStmt));
    	/*
    	sprintf(dynStmt, " select NVL(to_char(min(EFF_TIME),'yyyymmdd hh24:mi:ss'),' ')  "
    	          " from DDSMPORDERMSG%d   "
    	      " where id_no=to_number(:v1) and spid = :v2 and bizcode = :v3   "
    	    " and  EFF_TIME >= add_months(sysdate,(-1)*:v4)  and third_id_no = 0 ",  \
    	        atol(pOrderData->id_no )%10L  );
			*/
       strcpy(temp_opr_code,pOrderData->opr_code);
       pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "��ǰ�û��Ĳ���: %s",temp_opr_code);
       sprintf(dynStmt, "select nvl(to_char(min(opr_time),'yyyymmdd hh24:mi:ss'),' ') "
       				" from ddsmpordermsghis%ld  "
       				" where id_no = to_number(:v1) and spid = :v2 and bizcode = :v3  "
       				" and opr_code =:v4 and opr_time > to_date(to_char(sysdate,'yyyymm')||'01','yyyymmdd')",atol(pOrderData->id_no )%10L);

    	EXEC SQL EXECUTE
    	BEGIN
    		/*
    	  EXECUTE IMMEDIATE :dynStmt INTO :temp_first_time  using :vid_no,:vspid ,:vbizcode,:first_order_limit;
    	  */
    	  EXECUTE IMMEDIATE :dynStmt INTO :temp_first_time  using :vid_no,:vspid ,:vbizcode,:temp_opr_code;
    	END;
    	END-EXEC;

    	if ((SQLCODE != 0) &&(SQLCODE != 1403 ))
		{
    	  	sprintf(retMsg, "����ȷ�϶�ȡ�û��״ζ���ʱ��ʧ��[%d]!",SQLCODE);
	  	  	pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "dynStmt=[%s]", dynStmt);
	  	  	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-45", retMsg);
    	  	return -45 ;
    	}
    	else if (SQLCODE == 1403 )
    	{   /**ȱʡֵ**/

    	} else
    	{
    		pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "temp_first_time=[%s]", temp_first_time);
			Trim(temp_first_time);
    	   	if (strlen(temp_first_time) > 0)
    	   	{
    	        strcpy(pOrderData->first_order_date ,temp_first_time);
    	   	}
    	}

  }

    /** �õ�����Ϣ��Ϣ 2010.6.13����***/
    Trim(pOrderData->msg_oprcode);
    if(strcmp(pOrderData->msg_oprcode,"")==0)
    {
			strcpy(pOrderData->msg_oprcode ,"RC"  );
    }
    retCode = fDsmpGetShortMsg(pOrderData,vcontent ,vcontent2 ,"00" ,retMsg);
    if (retCode != 0)
    {
		sprintf(retMsg, "�õ����Žӿڱ����[%d][%s]!",retCode ,retMsg );
		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-26", retMsg);
      	return -26;
    }
    Trim(vcontent);
    Trim(vcontent2 );
	 printf("\npOrderData->eff_time=[%s],pOrderData->end_time=[%s]\n",pOrderData->eff_time,pOrderData->end_time);
  /** ��¼�û��Ķ���ȷ�ϲ�����Ϣ����ʱ�� ****/
  memset(dynStmt, 0, sizeof(dynStmt));
  sprintf(dynStmt, "  INSERT INTO PDSMPRECONFIRMINFO  "
        "("
                  " order_id,id_no,phone_no,serv_code                 "
                  " ,opr_code,third_id_no,third_phoneno,spid          "
                  " ,bizcode,prod_code,comb_flag,mode_code            "
                  " ,chg_flag,chrg_type,channel_id,opr_source         "

                  " ,first_order_date,first_login_no,last_order_date  "
                  " ,opr_time,eff_time,end_time                       "
                  " ,local_code,valid_flag,belong_code                "
                  " ,total_date,login_accept,reserved                 "
                  " ,sys_note,op_note ,oprnumb   "
                  " ,login_no ,op_code,group_id,password "
        ")"
        "values"
        "("
              " :v1,to_number(:v2),:v3,:v4, "
              " :v5,to_number(:v6),:v7, :v8 , "
              " :v9,:v10,:v11, :v12 , "
              " :v13,:v14,:v15, :v16 , "

            " to_date(:v17,'yyyymmdd hh24:mi:ss'),:v18,to_date(:v19,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v20,'yyyymmdd hh24:mi:ss'), to_date(:v21,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v22,'yyyymmdd hh24:mi:ss') ,  "
              " :v23, :v24, :v25 , "
            " to_number(:v26) ,to_number(:v27) , :v28 , "
            " :v29, :v30, :v31,"
            "  :v32,:v33,:v34, :v35 "
        ")"  );
  EXEC SQL PREPARE prepare1 From :dynStmt;
  EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                          :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,
                          :pOrderData->bizcode,:pOrderData->prod_code, :pOrderData->comb_flag,:pOrderData->mode_code,
                          :pOrderData->chg_flag,:pOrderData->chrg_type,:pOrderData->channel_id,:pOrderData->opr_source,
                          :pOrderData->first_order_date,:pOrderData->first_login_no,:pOrderData->last_order_date,
                          :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->end_time ,
                          :pOrderData->local_code,:pOrderData->valid_flag,:pOrderData->belong_code ,
                          :pOrderData->total_date,:pOrderData->login_accept,:pOrderData->reserved ,
                          :pOrderData->sys_note,:pOrderData->op_note ,:pOrderData->oprnumb ,
                          :pOrderData->login_no ,:pOrderData->op_code ,:vGroupId,:pOrderData->password;
  if (SQLCODE !=0)
  {
    	sprintf(retMsg, "�������ȷ����ʱ��PDSMPRECONFIRMINFO����[%d]!",  SQLCODE);
    	#ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "dynStmt=[%s]", dynStmt);
		#endif
		PUBLOG_DBDEBUG("fPubDsmpReConfirm");
		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-11", retMsg);
    	return -11;
  }

  /** ��¼�û��Ķ���ȷ�ϲ�����ˮ�� ****/
  memset(dynStmt, 0, sizeof(dynStmt));
  sprintf(dynStmt, "  INSERT INTO WDSMPRECONFIRM%d  "
        "("
                  " order_id,id_no,phone_no,serv_code "
                  " ,opr_code,third_id_no,third_phoneno,spid ,bizcode "
                  " ,opr_source ,opr_time ,back_code  "
                  " ,time_limit,sms_cont,extend_code,rsp_accept   "
                  " ,over_time_dur,rsp_time,rsp_sms_cont,reserved "
                  " ,login_no,total_date,login_accept,op_code     "
                  " ,op_note,confirm_flag   "
        ")"
        "values"
        "("
              " :v1,to_number(:v2),:v3,:v4, "
              " :v5,to_number(:v6),nvl(:v7,'0'), :v8 , :v9, "
              " :v10,to_date(:v11,'yyyymmdd hh24:mi:ss'), :v12 , "
              " :v13,:v14,' ', to_number(:15) , "
              " 0, NULL,NULL, '  ', "
              " :v16, to_number(:v17) ,to_number(:v18) ,:v19, "
              " :v20,'0' "
        ")" ,atoi(pOrderData->total_date)/100 );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->total_date      =[%d]", atoi(pOrderData->total_date)/100 );
#endif
  EXEC SQL PREPARE prepare2 From :dynStmt;
  EXEC SQL EXECUTE prepare2 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no ,:pOrderData->serv_code,
                            :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,:pOrderData->bizcode,
                            :pOrderData->opr_source,:pOrderData->opr_time ,:vback_code,
                            :vtime_limit,:vcontent,:vrsp_accept,
                            :pOrderData->login_no ,:pOrderData->total_date,:pOrderData->login_accept,
                            :pOrderData->op_code , :pOrderData->op_note;
  if (SQLCODE !=0)
  {
	    sprintf(retMsg, "�����WDSMPRECONFIRM%d����[%d]!", atoi(pOrderData->total_date)/100, SQLCODE);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->order_id=[%ld],pOrderData->id_no=[%s],pOrderData->phone_no=[%s],pOrderData->serv_code=[%s]",pOrderData->order_id,pOrderData->id_no,pOrderData->phone_no,pOrderData->serv_code);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->opr_code=[%s],pOrderData->third_id_no=[%s],pOrderData->third_phoneno=[%s],pOrderData->spid=[%s]",pOrderData->opr_code,pOrderData->third_id_no,pOrderData->third_phoneno,pOrderData->spid);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->bizcode=[%s],pOrderData->opr_source=[%s],pOrderData->opr_time=[%s],vback_code=[%s]",pOrderData->bizcode,pOrderData->opr_source,pOrderData->opr_time,vback_code);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "vtime_limit=[%ld],vcontent=[%s],vrsp_accept=[%s],pOrderData->login_no=[%s],pOrderData->total_date=[%s],pOrderData->login_accept=[%s]",vtime_limit,vcontent,vrsp_accept,pOrderData->login_no,pOrderData->total_date,pOrderData->login_accept);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "pOrderData->op_code=[%s],pOrderData->op_note=[%s]",pOrderData->op_code,pOrderData->op_note);
	    pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "dynStmt=[%s]", dynStmt);
	    return -12;
  }

	/*** R_JLMob_liubo_CRM_CMI_2012_0163@����ͳһ�������ɡ����͡���ص��Ż����� by zhangxina @20121114
	** memset(dynStmt,0,sizeof(dynStmt));
	** sprintf(dynStmt,"INSERT INTO wCommonShortMsgSp(COMMAND_ID,login_accept, msg_level, phone_no, msg, order_code, back_code, dx_op_code, login_no, op_time ) select SMSG_SEQUENCE.nextval, :v1, 1, :v2, :v3, 0, 0, :v4, :v5, sysdate from dual ");
	** EXEC SQL PREPARE prepare3 From :dynStmt;
	** EXEC SQL EXECUTE prepare3 USING  :vrsp_accept,:pOrderData->phone_no,:vcontent,:pOrderData->op_code,:pOrderData->login_no;
	** if ( SQL_OK != SQLCODE )
	** {
	** 	#ifdef _DEBUG_
	** 		pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "dynStmt = [%s][%d]",dynStmt,SQLCODE);
	** 	#endif
	** 	pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "vrsp_accept=[%ld],pOrderData->phone_no=[%s],vcontent=[%s],pOrderData->op_code=[%s],pOrderData->login_no=[%s]",vrsp_accept,pOrderData->phone_no,vcontent,pOrderData->op_code,pOrderData->login_no);
	** 	PUBLOG_DBDEBUG("fPubDsmpReConfirm");
	** 	pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-13", retMsg);
	** 	return -13 ;
	** }
	***/

	if(strlen(vcontent) > 0)
	{
		smModelId = 1230;
		PrepareNewShortMsg(smModelId);
		SetShortMsgParameter(smModelId, UPWARDACCEPT, vrsp_accept);
		SetShortMsgParameter(smModelId, "MSG_CONTENT", vcontent);
		ret = GenerateShortMsg(smModelId, pOrderData->phone_no, pOrderData->login_no, "SPDX", atol(pOrderData->login_accept), pOrderData->opr_time);
		if ( ret != 0 )
		{
			PUBLOG_DBDEBUG("fSendShortMsg");
			pubLog_DBErr(_FFL_, "","-3","�������Ѷ��Ŵ���[%d] ",ret);
			return -13;
		}
	}

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpReConfirm", "", "-- end -- fPubDsmpReConfirm ");
#endif
  return 0 ;
}



/*���ṩ�ĸ�ʽȡϵͳʱ�� */
int fPubFormatTime(char *timeDest, char *timeSrc)
{
    char sOutDate[20+1];
    char sTmpTimeSrc[17 + 1];
    char sTmpBuff[100];
    memset(sOutDate, 0, sizeof(sOutDate));
    memset(sTmpTimeSrc, 0, sizeof(sTmpTimeSrc));
    memset(sTmpBuff, 0, sizeof(sTmpBuff));

    if(NULL==timeDest || NULL==timeSrc)
    {
#ifdef _DEBUG_
        pubLog_Debug(_FFL_, "fPubFormatTime", "", "call fPubFormatTime:�����������ΪNULL");
#endif
		pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-1", "");
        return -1;
    }
    strcpy(sTmpTimeSrc,timeSrc);
    Trim(sTmpTimeSrc);

    sprintf(sTmpBuff,"select to_char(to_date(:v1, 'yyyymmdd hh24:mi:ss'), 'yyyymmddhh24miss') from dual");
    EXEC SQL PREPARE preGetSysdate from :sTmpBuff;
    EXEC SQL DECLARE curGetSysdate CURSOR FOR preGetSysdate;
    EXEC SQL OPEN curGetSysdate using :sTmpTimeSrc;
    while(1)
    {
        EXEC SQL FETCH curGetSysdate INTO  :sOutDate;
        if(SQLCODE == 1403) break;
        if(SQLCODE != 0)
        {
#ifdef _DEBUG_
            pubLog_Debug(_FFL_, "fPubFormatTime", "", "��ѯϵͳʱ��ʧ��[%d]!",SQLCODE);
#endif
            EXEC SQL CLOSE curGetSysdate ;
            PUBLOG_DBDEBUG("fPubDsmpReConfirm");
			pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-3", "");
            return -3;
        }
        if (SQLCODE != 0 ) break;
    }
    EXEC SQL CLOSE curGetSysdate;

    Trim(sOutDate);
    if(strlen(sOutDate) == 0)
    {
    	pubLog_DBErr(_FFL_, "fPubDsmpReConfirm", "-4", "");
        return -4;
    }
    strcpy(timeDest, sOutDate);
    return 0;
}

/**********************************************
 @lixg FUNC : fDsmpOprCodeCheck  DSMP�������ͼ��
 @input  params: biz_type -> SPҵ������
 @input  params: OPR_CODE -> ԭ����״̬
 @input  params: OPR_CODE_NEW -> �²���״̬
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success ,1 no operate else 0,1 fail
 *********************************************/
int fDsmpOprCodeCheck(char *biz_type, char *opr_code,char *opr_code_new,char *return_mess)
{
  EXEC SQL BEGIN DECLARE SECTION;
    char vid_no[14+1];
    char vphone_no[11+1];
    int  icount = 0;
    char vbiz_type[2+1];
    char vopr_code[2+1];
    char vopr_code_new[2+1];
  EXEC SQL END DECLARE SECTION;

    Sinitn(vopr_code );
    Sinitn(vopr_code_new );
    Sinitn(vbiz_type );

  strcpy(vopr_code, opr_code );
  strcpy(vbiz_type, biz_type);
  strcpy(vopr_code_new, opr_code_new);

    Trim(opr_code);
    Trim(opr_code_new);
    Trim(vbiz_type);

  pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "=========��ʼ����fDsmpOprCodeCheck������=========");

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "input vopr_code = [%s]", vopr_code);
  pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "input vopr_code_new = [%s]", vopr_code_new);
  pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "input vbiz_type = [%s]", vbiz_type);
#endif

  /* ����״̬�ļ��
  if (strcmp(opr_code,vopr_code_new) == 0) {
    pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "����״̬��ԭ״̬һ��,�������ý���");
    sprintf(return_mess, "����״̬��ԭ״̬һ��,�������ý���!" );
    return -1;
  }*/

/***
04-ҵ����ͣ
05-ҵ��ָ�
14-�û�������ͣ
15-�û������ָ�
09-�û�������ͣ���ֻ�����ר�á�������ҵ��Ĳ�������OPRCODE�г�ͻ��

1�����ֲ���Ӧ�óɶԽ��У�
2��04 ����ͨ��15����
3��14 ֻ��ͨ��15����   ****/

    /** ��ͣ״̬�����ٽ�����ͣ ����**/
    if ( (strcmp(vopr_code_new,BIZ_OPR_SUSPEND) == 0)
       || (strcmp(vopr_code_new,BIZ_OPR_14) == 0)
       || ((strcmp(vopr_code_new,"09") == 0) && (strcmp(biz_type,"16") == 0))  )
       {
           if ( (strcmp(opr_code,BIZ_OPR_SUSPEND) == 0)
                    || (strcmp(opr_code,BIZ_OPR_14) == 0)
                    || ((strcmp(opr_code,"09") == 0) && (strcmp(biz_type,"16") == 0)) )
            {
        		pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "ԭ״̬Ϊ��ͣ,���ܽ���ҵ����ͣ����!" );
				sprintf(return_mess, "ԭ״̬Ϊ��ͣ,���ܽ���ҵ����ͣ����!"  );
           		pubLog_DBErr(_FFL_, "fDsmpOprCodeCheck", "1310", return_mess);
                return 1310;
          	}
    }

    /** ԭ״̬Ϊ��ͣ[04],���ܽ���ҵ��ָ�[05]���� **/
  if ((strcmp(vopr_code_new,BIZ_OPR_RESUME) == 0) && (strcmp(opr_code,BIZ_OPR_SUSPEND) != 0))
  {
  		pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "ԭ״̬Ϊ��ͣ[04],���ܽ���ҵ��ָ�[05]����!" );
		sprintf(return_mess, "ԭ״̬Ϊ��ͣ[04],���ܽ���ҵ��ָ�[05]����!"  );
		pubLog_DBErr(_FFL_, "fDsmpOprCodeCheck", "1310", return_mess);
		return 1310;
  }

  /** ԭ״̬Ϊ��ͣ[04��14��09],���ܽ����û������ָ�[15]���� **/
    if (strcmp(vopr_code_new,BIZ_OPR_15) == 0)
    {
             if ( (strcmp(opr_code,BIZ_OPR_SUSPEND) == 0)
                    || (strcmp(opr_code,BIZ_OPR_14) == 0)
                    || ((strcmp(opr_code,"09") == 0) && (strcmp(biz_type,"16") == 0)) )
             {
                        return 0;

             }
             else
             {
                    pubLog_Debug(_FFL_, "fDsmpOprCodeCheck", "", "ԭ״̬Ϊ��ͣ,���ܽ����û������ָ�[15]����!" );
                    sprintf(return_mess, "ԭ״̬Ϊ��ͣ,���ܽ����û������ָ�[15]����!"  );
					pubLog_DBErr(_FFL_, "fDsmpOprCodeCheck", "1311", return_mess);
                    return 1311;
          	}
    }

  return 0 ;
}


/** DSMP��Ʒ���Ӷ�����Ϣ����**/
int fPubDsmpOrderMsgAdd(DSMPORDEROpr *pOrderData,char *opr_flag,char *temp_orderadd_flag,char *retMsg)
{
  int  i = 0;
  int  retCode = 0;
  char *p, *q;
  int  rowNum=0;
  char ch ='|';

  EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[1024];
    char vopr_flag[1 +1 ];    /**1:����  0��ȡ���� ��ͣ���ָ�  **/
    char vcontent[1000];
    char orderpoint[1+1];
    char ordersync[1+1];
    char ordersynctable[30+1];
    char sTmpTime[5][14 + 1];
    char vinfocode[1001];
    char vinfovalue[1001];
    char infocodeArr[30][20+1];
    char infovalueArr[30][20+1];
  EXEC SQL END DECLARE SECTION;

    Sinitn(vopr_flag);
    Sinitn(orderpoint);
    Sinitn(ordersync);
    Sinitn(ordersynctable);
    Sinitn(vcontent);
    Sinitn(vinfocode);
    Sinitn(vinfovalue);
    memset(sTmpTime, 0, sizeof(sTmpTime));

    strcpy(vopr_flag, opr_flag);
    strcpy(vinfocode, pOrderData->infocode);
    strcpy(vinfovalue,  pOrderData->infovalue);

    /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       int ret =0;
       EXEC SQL BEGIN DECLARE SECTION;
    	   char vGroupId[10+1];
       EXEC SQL END DECLARE SECTION;
       memset(vGroupId, 0, sizeof(vGroupId));
       ret = sGetUserGroupid(pOrderData->phone_no,vGroupId);
       if(ret <0)
       	{
			PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
			pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "200919", "��ȡ�����û�group_idʧ��!");
         	return 200919;
        }
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "-- begin -- fPubDsmpOrderMsgAdd   ");
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->order_id        =[%ld]", pOrderData->order_id       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->id_no           =[%s]", pOrderData->id_no           );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->phone_no        =[%s]", pOrderData->phone_no        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->serv_code       =[%s]", pOrderData->serv_code       );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->opr_code        =[%s]", pOrderData->opr_code        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->infocode        =[%s]", pOrderData->infocode        );
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->infovalue       =[%s]", pOrderData->infovalue       );
#endif

    if ( (rowNum =strToRows(vinfovalue, ch, infovalueArr, 21)) == 0)
    {
        sprintf(retMsg, "����ĸ�����Ϣ���������Ϊ��!");
		pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-19", retMsg);
        return -19;
    }
  if ( (rowNum =strToRows(vinfocode, ch, infocodeArr, 21)) == 0)
    {
        sprintf(retMsg, "����ĸ�����Ϣֵ������Ϊ��!");
        pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-18", retMsg);
        return -18;
    }
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "rowNum =[%d]", rowNum    );
#endif

    if (strcmp(vopr_flag ,"1") == 0 )
    {
        for (i=0; i<rowNum ; i++)
        {
            memset(dynStmt, 0, sizeof(dynStmt));
       		sprintf(dynStmt, "  INSERT INTO DDSMPORDERMSGADD  "
                         "  (ORDER_ID,INFO_CODE,INFO_VALUE) "
                         "  values (:v1, :v2,:v3 ) "  );
          EXEC SQL PREPARE prepare1 From :dynStmt;
          EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,
                          :infocodeArr[i] ,:infovalueArr[i] ;
          if (SQLCODE !=0)
          {
	            sprintf(retMsg, "������ˮ��DDSMPORDERMSGADD[%s]����[%d]!", infocodeArr[i], SQLCODE);
	            PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
		        pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-16", retMsg);
	            return -16;
          }
      }
    }  else {
    /** ���������Ϣ ****/
        for (i=0; i<rowNum ; i++)
        {
		#ifdef _DEBUG_
		    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "infocodeArr[%d] =[%s]", i,infocodeArr[i]  );
		    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "infovalueArr[%d] =[%s]", i,infovalueArr[i]   );
		#endif
            memset(dynStmt, 0, sizeof(dynStmt));
        sprintf(dynStmt, "  UPDATE DDSMPORDERMSGADD  "
                         "     SET INFO_VALUE = :v1  "
                         "   WHERE  order_id = :v2 AND info_code =:v3 "  );
          EXEC SQL PREPARE prepare1 From :dynStmt;
          EXEC SQL EXECUTE prepare1 USING :infovalueArr[i],:pOrderData->order_id , :infocodeArr[i];
          if (SQLCODE == 1403)
          	{
                memset(dynStmt, 0, sizeof(dynStmt));
           		sprintf(dynStmt, "  INSERT INTO DDSMPORDERMSGADD  "
                             "  (ORDER_ID,INFO_CODE,INFO_VALUE) "
                             "  values (:v1, :v2,:v3 ) "  );
              EXEC SQL PREPARE prepare1 From :dynStmt;
              EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,
                              :infocodeArr[i] ,:infovalueArr[i] ;
              if (SQLCODE !=0)
              {
                sprintf(retMsg, "������ˮ��DDSMPORDERMSGADD[%s]����[%d]!", infocodeArr[i], SQLCODE);
                PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
	            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-15", retMsg);
                return -15;
              }
          }
          else if (SQLCODE !=0)
          {
            	sprintf(retMsg, "������ˮ��DDSMPORDERMSGADD[%s]����[%d]!", infocodeArr[i], SQLCODE);
	            PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
	            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-14", retMsg);
            	return -14;
          }
      }
    }

    if (strcmp(temp_orderadd_flag,"1") == 0   )
    {
  /** ��¼�û��Ķ���������Ϣ ****/
  memset(dynStmt, 0, sizeof(dynStmt));
  sprintf(dynStmt, "  INSERT INTO WDSMPORDEROPR%d  "
        "("
                  " order_id,id_no,phone_no,serv_code                 "
                  " ,opr_code,third_id_no,third_phoneno,spid          "
                  " ,bizcode,prod_code,comb_flag,mode_code            "
                  " ,chg_flag,chrg_type,channel_id,opr_source         "

                  " ,first_order_date,first_login_no,last_order_date  "
                  " ,opr_time,eff_time,end_time                       "
                  " ,local_code,valid_flag,belong_code                "
                  " ,total_date,login_accept,reserved                 "
                  " ,sys_note,op_note ,oprnumb   "
                  " ,login_no ,op_code ,sm_code,password,group_id "
        ")"
        "values"
        "("
              " :v1,to_number(:v2),:v3,:v4, "
              " :v5,to_number(:v6),:v7, :v8 , "
              " :v9,:v10,:v11, :v12 , "
              " :v13,:v14,:v15, :v16 , "

            " to_date(:v17,'yyyymmdd hh24:mi:ss'),:v18,to_date(:v19,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v20,'yyyymmdd hh24:mi:ss'), to_date(:v21,'yyyymmdd hh24:mi:ss'), "
            " to_date(:v22,'yyyymmdd hh24:mi:ss') ,  "
              " :v23, :v24, :v25 , "
            " to_number(:v26) ,to_number(:v27) , :v28 , "
            " :v29, :v30, :v31,"
            "  :v32,:v33 ,:v34 ,:v35,:v36"
        ")",atoi(pOrderData->total_date)/100 );

  EXEC SQL PREPARE prepare1 From :dynStmt;
  EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                          :pOrderData->opr_code,:pOrderData->third_id_no, :pOrderData->third_phoneno,:pOrderData->spid,
                          :pOrderData->bizcode,:pOrderData->prod_code, :pOrderData->comb_flag,:pOrderData->mode_code,
                          :pOrderData->chg_flag,:pOrderData->chrg_type,:pOrderData->channel_id,:pOrderData->opr_source,
                          :pOrderData->first_order_date,:pOrderData->first_login_no,:pOrderData->last_order_date,
                          :pOrderData->opr_time,:pOrderData->eff_time,:pOrderData->end_time ,
                          :pOrderData->local_code,:pOrderData->valid_flag,:pOrderData->belong_code ,
                          :pOrderData->total_date,:pOrderData->login_accept,:pOrderData->reserved ,
                          :pOrderData->sys_note,:pOrderData->op_note ,:pOrderData->oprnumb ,
                          :pOrderData->login_no ,:pOrderData->op_code ,:pOrderData->sm_code,:pOrderData->password,:vGroupId;
  if (SQLCODE !=0)
  {
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "INSERT INTO WDSMPORDEROPR error =[%d]", SQLCODE);
#endif
    sprintf(retMsg, "������ˮ��WDSMPORDEROPR%d ����[%d]!", atoi(pOrderData->total_date)/100, SQLCODE);
    #ifdef _DEBUG_
    		pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "dynStmt=[%s]", dynStmt);
		#endif
			PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-24", retMsg);
    		return -24;
  }
    }

  /** ������ϵͬ�����ӿڱ� **/
  if ((strcmp(ordersync,"1" ) == 0) && (strcmp(temp_orderadd_flag,"1") == 0   ) ) {

      /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
        memset(sTmpTime, 0, sizeof(sTmpTime));
        if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
        {
            sprintf(retMsg,"op_time����fPubFormatTimeʧ��! iRet=[%d]", i);
        	PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-26", retMsg);
       		return -26;
        }

      memset(dynStmt, 0, sizeof(dynStmt));
      sprintf(dynStmt, "  INSERT INTO %s   "
        "("
                  " MAXACCEPT,LOCALCODE,Misisdn,TMSISDN ,    "
                  " OprCode,EfftTime,Source,Channel ,        "
                  " ChgFlg,OrderStatus,BillFlg,SPID , SPBIZCODE,MaxScan "
        ")"
        "values"
        "("
              " :v1,:v2,:v3,:v4, "
              " :v5, :v6,:v7, :v8 , "
              " :v9,:v10,:v11, :v12,:v13,'0'  "
        ")" ,ordersynctable );
#ifdef _DEBUG_
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->login_accept =[%s]", pOrderData->login_accept );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->local_code   =[%s]", pOrderData->local_code   );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->phone_no     =[%s]", pOrderData->phone_no     );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->third_phoneno=[%s]", pOrderData->third_phoneno);
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "sTmpTime[0]             =[%s]", sTmpTime[0]             );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->opr_source   =[%s]", pOrderData->opr_source   );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->channel_id   =[%s]", pOrderData->channel_id   );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->chg_flag     =[%s]", pOrderData->chg_flag     );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->chrg_type    =[%s]", pOrderData->chrg_type    );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->spid         =[%s]", pOrderData->spid         );
pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "pOrderData->bizcode      =[%s]", pOrderData->bizcode      );

#endif

      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :pOrderData->login_accept ,:pOrderData->local_code,:pOrderData->phone_no,:pOrderData->third_phoneno,
                          :pOrderData->opr_code,:sTmpTime[0], :pOrderData->opr_source,:pOrderData->channel_id,
                          :pOrderData->chg_flag,:pOrderData->opr_code,:pOrderData->chrg_type,:pOrderData->spid,
                          :pOrderData->bizcode ;
      if (SQLCODE !=0)
      {
        	sprintf(retMsg, "����ͬ���ӿڱ�[%s] ����[%d]!", ordersynctable, SQLCODE);
			pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "dynStmt=[%s]", dynStmt);
			PUBLOG_DBDEBUG("fPubDsmpOrderMsgAdd");
            pubLog_DBErr(_FFL_, "fPubDsmpOrderMsgAdd", "-25", retMsg);
        	return -25;
      }
    }

    /** ���Ͷ���Ϣ���ӿڱ�
  EXEC SQL  SELECT  a.CONTENT INTO :vcontent
          FROM SDSMPSMSMSGTYPE    a, SDSMPSMSMSGCODE b,SDSMPSMSSENDCODE c
        WHERE a.MSGTYPEID = b.MSGTYPEID
            AND a.MSGTYPEID = c.MSGTYPEID
            AND b.spid =  :pOrderData->spid
            AND b.bizcode = :pOrderData->bizcode
            AND c.SERV_CODE  = :pOrderData->serv_code
            AND c.OPR_SOURCE = :pOrderData->opr_source
            AND c.CHRG_TYPE  = :pOrderData->chg_flag
            AND rownum <2  ;
    if (SQLCODE != 0)
    {
      sprintf(retMsg, "��ȡ��SDSMPSMSMSGTYPE������Ϣ����[%d]!",SQLCODE);
      return -30;
    }
    Trim(vcontent);

    retCode = fSendShortMsg(pOrderData->login_accept,pOrderData->phone_no,pOrderData->op_code,pOrderData->login_no,vcontent);
    if (retCode != 0)
    {
      sprintf(retMsg, "������Žӿڱ����[%d]!",retCode);
      return -26;
    } **/

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpOrderMsgAdd", "", "-- end -- fPubDsmpOrderMsgAdd   ");
#endif

  return 0 ;
}




/** DSMP��Ʒ�����ӿڴ�����Ϣ����**/
int fPubDsmpInterErr(DSMPORDEROpr *pOrderData,char *retCode,char *retMsg)
{
  int i = 0;
  EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[1024];
	char vid_no[14 +1];
	char vorder_id[14 +1];
	int  iCount = 0;
  EXEC SQL END DECLARE SECTION;

  Sinitn(dynStmt);
  Sinitn(vid_no);
  Sinitn(vorder_id);

  strcpy(vid_no,  pOrderData->id_no);

  #ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "-- begin -- fPubDsmpInterErr   ");
      pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "00000000000000000vid_no           =[%s]", vid_no  );
    pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "pOrderData->order_id        =[%ld]", pOrderData->order_id );
    #endif

    if ((strcmp(pOrderData->opr_code,BIZ_OPR_EXIT) != 0 )
       ||  (strcmp(pOrderData->opr_code,BIZ_OPR_UNDEST) != 0 )||  (strcmp(pOrderData->opr_code,BIZ_OPR_12) != 0 )  )
    {
         	 return atoi(retCode) ;
    }

  /** ��¼�û��Ķ�����ʷ��Ϣ ****/
  memset(dynStmt, 0, sizeof(dynStmt));
  sprintf(dynStmt, "  UPDATE DDSMPORDERMSG%c  "
                     "     set  opr_code = '17'  "
                     "   WHERE   order_id = :v35  "  ,vid_no[strlen(vid_no)-1] );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "22222222222222222222222 vid_no =[%s]", vid_no);
    pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "pOrderData->order_id=[%ld]", pOrderData->order_id);
    pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "dynStmt=[%s]", dynStmt);
#endif
  EXEC SQL PREPARE prepare2 From :dynStmt;
  EXEC SQL EXECUTE prepare2 USING   :pOrderData->order_id  ;
  if (SQLCODE !=0)
  {
    	sprintf(retMsg, "fPubDsmpInterErr�����±�DDSMPORDERMSG%ld����[%d]!", atol(vid_no)%10, SQLCODE);
    	PUBLOG_DBDEBUG("fPubDsmpInterErr");
    	pubLog_DBErr(_FFL_, "fPubDsmpInterErr", "-11", retMsg);
   		return -11;
  }

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpInterErr", "", "-- end -- fPubDsmpInterErr   ");
#endif

    strcpy(retCode,"000000");
    strcpy(retMsg,"OK!OK!!!");
  return 0 ;
}


/**********************************************
 @lixg FUNC : fDsmpInterSend  DSMP�ӿ���Ϣ���ʹ���
 @input  params: pOrderData -> ��Ʒ������Ϣ
 @input  params: ordersync_flag ������ϵͬ�����ӿڱ����־ 1���������򲻴���
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpInterfaceSend(DSMPORDEROpr *pOrderData,char *sParamStr,char *ordersync_flag,char *retMsg)
{
    int  iRet =0 ;
    int  i =0;
  EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[1024];

    char orderpoint[1+1];
    char ordersync[1+1];
    char ordersynctable[30+1];
    char sTmpTime[5][14 + 1];
    char canclesyn[1+1];
    char retCode[6+1];
    char second_retCode[6+1];
    char temp_portalcode[4+1];
  EXEC SQL END DECLARE SECTION;

    Sinitn(dynStmt );
    Sinitn(orderpoint);
    Sinitn(ordersync);
    Sinitn(ordersynctable);
    Sinitn(canclesyn);
    Sinitn(retCode);
    Sinitn(second_retCode);
    memset(sTmpTime, 0, sizeof(sTmpTime));
    Sinitn(temp_portalcode);

  pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "=========��ʼ����fDsmpInterfaceSend������=========");

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "input ordersync_flag = [%s]", ordersync_flag);
  pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "input sParamStr = [%s]", sParamStr);
#endif

    /** CANCLESYN IS '0���˶���ͬ�����ã�1���˶����첽����' **/
  EXEC SQL  SELECT ORDERPOINT,ORDERSYNC,nvl(ORDERSYNCTABLE,'  ' ) ,CANCLESYN ,PORTALCODE
              into  :orderpoint,:ordersync,:ordersynctable,:canclesyn ,:temp_portalcode
            FROM  SOBBIZTYPE_dsmp
       WHERE  oprcode = :pOrderData->serv_code ;
    if  (SQLCODE != 0)
    {
    	pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->serv_code =[%s]", pOrderData->serv_code);
     	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", pOrderData->serv_code,SQLCODE);
      	PUBLOG_DBDEBUG("fDsmpInterfaceSend");
      	pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-104", retMsg);
     	return -104;
    }
    Trim(temp_portalcode);
    if (strcmp(temp_portalcode,"DSMP") == 0)
    {  /**DSMP������BOSSΪ׼�����÷��ͽӿڣ�ֱ��ͬ�����ݼ��� ***/
        return 0;
    }

    if ( (strncmp(pOrderData->sync_flag,"0",1)==0))
    {  /** 1��ͬ��  0���첽����;Ĭ��ͬ��**/
        /** ���������Ʒ������COMB_SYNCFLAG �ж�*/
      memset(dynStmt, 0, sizeof(dynStmt));
      sprintf(dynStmt, "  INSERT INTO dDSMPInterSend  "
        "("
                  " order_id,id_no,phone_no,serv_code         "
                  " ,opr_code,total_date,login_accept, OPR_TIME "
                  " ,Param_Str,deal_flag  ,deal_date   "
                  " ,ret_code ,ret_msg ,ORDERSYNC_FLAG,TRYTIMES "
        ")"
        "values"
        "("
              " :v1,to_number(:v2),:v3,:v4, "
              " :v5,to_number(:v6),to_number(:v7) ,  to_date(:v8,'yyyymmdd hh24:mi:ss'), "
              " :v9,'0',NULL, ' ' , ' ' ,'0' , 0 "
        ")"  );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "dynStmt=[%s]", dynStmt);
#endif
      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :pOrderData->order_id ,:pOrderData->id_no,:pOrderData->phone_no,:pOrderData->serv_code,
                          :pOrderData->opr_code,:pOrderData->total_date,:pOrderData->login_accept,
                          :pOrderData->opr_time,:sParamStr ;
      if (SQLCODE !=0)
      {
        pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "��������pOrderData->order_id=[%ld]pOrderData->id_no=[%s]pOrderData->phone_no=[%s]pOrderData->serv_code=[%s]pOrderData->opr_code=[%s]pOrderData->total_date=[%s]pOrderData->login_accept=[%s]pOrderData->opr_time=[%s]sParamStr=[%s]",pOrderData->order_id,pOrderData->id_no,pOrderData->phone_no,pOrderData->serv_code,pOrderData->opr_code,pOrderData->total_date,pOrderData->login_accept,pOrderData->opr_time,sParamStr);
        sprintf(retMsg, "����ӿڷ��ͱ�dDSMPInterSend����[%d]!",  SQLCODE);
        PUBLOG_DBDEBUG("fDsmpInterfaceSend");
      	pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-105", retMsg);
        return -105;
      }
    }
    else
   	{  /** ͬ������  **/
        #ifdef _DEBUG_
        	pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "......���ͽӿ����ݵ�DSMPƽ̨");
        #endif

      iRet = fCallInterface(sParamStr,retCode ,retMsg ,second_retCode);
      pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "���ͽӿ����ݵ�DSMPƽ̨��Ϣ����retCode=[%s]" ,retCode);
      if (iRet != 0)
      {
        	pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fCallInterface:���ýӿں�������second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
            strcpy(retCode,"791720");
            sprintf(retMsg, "fCallInterface:���ýӿں�������second_retCode=[%s]", second_retCode );
            iRet = fPubDsmpInterErr(pOrderData,retCode,retMsg);
            if (iRet != 0 )
            {
                pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fPubDsmpInterErr:retCode=[%s]  retMsg=[%s]iRet=[%d] ", retCode,retMsg,iRet );
     			pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-110", retMsg);
                return -110 ;
            }
      }

      if  (strncmp(second_retCode, "05",2) == 0)
      {  /**05:ҵ��ƽ̨�Ѵ��ڶ�����ϵ **/
        if (strncmp(pOrderData->opr_code, BIZ_OPR_DEST,2) == 0||strncmp(pOrderData->opr_code, BIZ_OPR_13,2) == 0||(strncmp(pOrderData->opr_code, BIZ_OPR_11,2) == 0&&strncmp(pOrderData->serv_code, "19",2) == 0)) {
          /** ���� BOSSϵͳ �� ҵ��ƽ̨ ���ݲ�һ������ ,�����������**/
      }
      else
      {
          		pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fCallInterface:ƽ̨���ش���second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
                strcpy(retCode,"791721");
                sprintf(retMsg, "fCallInterface:ƽ̨���ش���second_retCode=[%s] ", second_retCode);
                iRet = fPubDsmpInterErr(pOrderData,retCode,retMsg);
                if (iRet != 0 )
                {
                     pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fPubDsmpInterErr:retCode=[%s]  retMsg=[%s]iRet=[%d] ", retCode,retMsg,iRet );
                     pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-111", retMsg);
                     return -111 ;
                }
      }
      }
      else if (strncmp(second_retCode, "00",2) != 0)
		{
        pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fCallInterface:ƽ̨���ش���second_retCode=[%s]  retMsg=[%s] ", second_retCode,retMsg );
            strcpy(retCode,"791722");
            sprintf(retMsg, "fCallInterface:ƽ̨���ش���second_retCode=[%s]  ", second_retCode );
            iRet = fPubDsmpInterErr(pOrderData,retCode,retMsg);
			if (iRet != 0 )
			{
			     pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fPubDsmpInterErr:retCode=[%s]  retMsg=[%s]iRet=[%d] ", retCode,retMsg,iRet );
			     pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-112", retMsg);
                     return -112 ;
            }
      	}

      if (strncmp(retCode, "000000",6) != 0)
      {
        	pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fCallInterface:����ƽִ̨�к������� retCode=[%s]  retMsg=[%s] ", retCode,retMsg );
            strcpy(retCode,"791723");
            sprintf(retMsg, "fCallInterface:����ƽִ̨�к������� retCode=[%s]", retCode );
            iRet = fPubDsmpInterErr(pOrderData,retCode,retMsg);
            if (iRet != 0 )
            {
                pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "fPubDsmpInterErr:retCode=[%s]  retMsg=[%s]iRet=[%d] ", retCode,retMsg,iRet );
                pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-113", retMsg);
                     return -113 ;
			}
      }

    }

    if (strcmp(ordersync_flag,"1") == 0)
    {
        Trim(ordersynctable);
        if  (( strlen(ordersynctable) <= 0 )  && ( strcmp(ordersync,"1") == 0 ))
        {
        	pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "ordersynctable =[%s]", ordersynctable);
          	sprintf(retMsg, "SOBBIZTYPE_dsmp�е�ordersynctable�ֶβ���Ϊ��[%s][%d]!", ordersynctable,SQLCODE);
          	pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-101", retMsg);
          	return -101;
        }
    }

    if ((strcmp(ordersync_flag,"1") == 0) && (strcmp(ordersync,"1" ) == 0))
    { /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
        memset(sTmpTime, 0, sizeof(sTmpTime));
        if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
        {
            sprintf(retMsg,"op_time����fPubFormatTimeʧ��! iRet=[%d]", i);
            pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-102", retMsg);
        	return -102;
        }

      memset(dynStmt, 0, sizeof(dynStmt));
      sprintf(dynStmt, "  INSERT INTO %s   "
        "("
                  " MAXACCEPT,LOCALCODE,Misisdn,TMSISDN ,    "
                  " OprCode,EfftTime,Source,Channel ,        "
                  " ChgFlg,OrderStatus,BillFlg,SPID , SPBIZCODE,MaxScan ,infocode,infovalue"
        ")"
        "values"
        "("
              " :v1,:v2,:v3,:v4, "
              " :v5, :v6,:v7, :v8 , "
              " :v9,:v10,:v11, :v12,:v13,'0' ,:v14,:v15 "
        ")" ,ordersynctable );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "dynStmt=[%s]", dynStmt);
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->login_accept =[%s]", pOrderData->login_accept );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->local_code   =[%s]", pOrderData->local_code   );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->phone_no     =[%s]", pOrderData->phone_no     );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->third_phoneno=[%s]", pOrderData->third_phoneno);
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "sTmpTime[0]             =[%s]", sTmpTime[0]             );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->opr_source   =[%s]", pOrderData->opr_source   );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->channel_id   =[%s]", pOrderData->channel_id   );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->chg_flag     =[%s]", pOrderData->chg_flag     );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->chrg_type    =[%s]", pOrderData->chrg_type    );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->spid         =[%s]", pOrderData->spid         );
    pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "pOrderData->bizcode      =[%s]", pOrderData->bizcode      );
#endif

      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :pOrderData->login_accept ,:pOrderData->local_code,:pOrderData->phone_no,:pOrderData->third_phoneno,
                          :pOrderData->opr_code,:sTmpTime[0], :pOrderData->opr_source,:pOrderData->channel_id,
                          :pOrderData->chg_flag,:pOrderData->opr_code,:pOrderData->chrg_type,:pOrderData->spid,
                          :pOrderData->bizcode ,:pOrderData->infocode ,:pOrderData->infovalue ;
      if (SQLCODE !=0)
      {
        	sprintf(retMsg, "����ͬ���ӿڱ�[%s] ����[%d]!", ordersynctable, SQLCODE);
			PUBLOG_DBDEBUG("fDsmpInterfaceSend");
			pubLog_DBErr(_FFL_, "fDsmpInterfaceSend", "-103", retMsg);
        	return -103;
      }
    }

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpInterfaceSend", "", "-- end -- fDsmpInterfaceSend   ");
#endif

  return 0;
}

/**********************************************
 @FUNC   fDsmpProdRELATIONCheck  �û������Ĳ�Ʒ��ϵ���
 @input  params: id_no ->       �û�ID
 @input  params: phone_no ->    �û�����
 @input  params: prod_code ->   �����Ĳ�Ʒ����
 @input  params: return_mess -> ������Ϣ
 @output params: 0 success else 0 fail
 *********************************************/
int fDsmpProdRELATIONCheck(char *id_no, char *phone_no,char *prod_code, char *retMsg)
{
  EXEC SQL BEGIN DECLARE SECTION;
      char temp_sql[1024];
      char vid_no[14+1];
      char vphone_no[14+1];
      char vprod_code[22+1];
      int  icount = 0;
      int  icount2 = 0;
  EXEC SQL END DECLARE SECTION;

    Sinitn(vid_no );
    Sinitn(vphone_no );
    Sinitn(vprod_code );

    strcpy(vid_no, id_no);
    strcpy(vphone_no, phone_no);
    strcpy(vprod_code, prod_code);

    Trim(vid_no);
    Trim(vphone_no);
    Trim(vprod_code);

  pubLog_Debug(_FFL_, "fDsmpProdRELATIONCheck", "", "=========��ʼ����fDsmpProdRELATIONCheck������=========");

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpProdRELATIONCheck", "", "input vid_no = [%s]", vid_no);
  pubLog_Debug(_FFL_, "fDsmpProdRELATIONCheck", "", "input vphone_no = [%s]", vphone_no);
  pubLog_Debug(_FFL_, "fDsmpProdRELATIONCheck", "", "input vprod_code = [%s]", vprod_code);
#endif

/**��Ʒ�������ж�  ��a0��:�����ϵ;Ϊ�˱������Ժ��Ų�Ʒ�����Ĳ�Ʒ��ͻ ���Զ���Ϊ��a0�� **/
    icount = 0;
    EXEC SQL select count(*)  into  :icount
               FROM SDSMPPRODRELATION
              WHERE (sysdate between  begin_time  and  end_time)
                AND RELA_CODE = 'a0'
                AND prod_code = :vprod_code;
    if ( SQLCODE != 0 )
    {
		sprintf(retMsg, "��ȡSDSMPPRODRELATION��Ϣʧ��[%d]!",SQLCODE );
		PUBLOG_DBDEBUG("fDsmpProdRELATIONCheck");
		pubLog_DBErr(_FFL_, "fDsmpProdRELATIONCheck", "-41", retMsg);
		return -41;
    }
    else if (( SQLCODE == 0 ) && (icount > 0))
    	{
        /** �����ж��Ѿ������Ĳ�Ʒ���¶����Ĳ�Ʒ�Ƿ��г�ͻ **/
       icount2 =0;
       EXEC SQL select count(*)  into  :icount2
               FROM DDSMPORDERMSG  a ,SDSMPPRODRELATION b
              WHERE a.prod_code = b.RELA_PROD_CODE
                AND (sysdate between  b.begin_time  and  b.end_time)
                AND a.valid_flag  = '1'
                AND b.RELA_CODE = 'a0'
                AND b.prod_code = :vprod_code
                AND a.id_no = to_number(:vid_no) AND a.THIRD_ID_NO = 0 ;
       if  (icount2 > 0 )
       {
           sprintf(retMsg, "���ڲ�Ʒ����,ͬ���Ʒ�����ظ�����!" );
           pubLog_DBErr(_FFL_, "fDsmpProdRELATIONCheck", "-40", retMsg);
           return -40;
       }

        /** ����ж��Ѿ������Ĳ�Ʒ���¶����Ĳ�Ʒ�Ƿ��г�ͻ **/
       icount2 =0;
       EXEC SQL select count(*)  into  :icount2
               FROM DDSMPORDERMSG  a ,SDSMPPRODRELATION b
              WHERE a.prod_code = b.RELA_PROD_CODE
                AND (sysdate between  b.begin_time  and  b.end_time)
                AND a.valid_flag  = '1'
                AND b.RELA_CODE = 'a0'
                AND b.prod_code = :vprod_code
                AND a.THIRD_ID_NO = to_number(:vid_no)  ;
       if  (icount2 > 0 )
       {
           sprintf(retMsg, "�����͸��û�[%s]�Ĳ�Ʒ���ڻ����ϵ,ͬ���Ʒ�����ظ�����!",vphone_no );
           pubLog_DBErr(_FFL_, "fDsmpProdRELATIONCheck", "-45", retMsg);
           return -45;
       }
    }

/**��Ʒ�������ж�**/
    icount = 0;
    EXEC SQL select count(*)  into  :icount
                  FROM SDSMPPRODRELATION
                 WHERE (sysdate between  begin_time  and  end_time)
                   AND RELA_CODE = 'a1'
                   AND prod_code = :vprod_code;
    if ( SQLCODE != 0 )
   {
        sprintf(retMsg, "��ȡSDSMPPRODRELATION��Ϣʧ��[%d]!",SQLCODE );
		PUBLOG_DBDEBUG("fDsmpProdRELATIONCheck");
		pubLog_DBErr(_FFL_, "fDsmpProdRELATIONCheck", "-41", retMsg);
        return -41;
    }
    else if (( SQLCODE == 0 ) && (icount > 0))
    {
        /** �����ж� �û��Ƿ񶨹��� ��Ҫ�����Ĳ�Ʒ**/
        icount2 =0;
        EXEC SQL select count(*)  into  :icount2
                FROM DDSMPORDERMSG  a ,SDSMPPRODRELATION b
               WHERE a.prod_code = b.RELA_PROD_CODE
                 AND (sysdate between  b.begin_time  and  b.end_time)
                 AND a.valid_flag  = '1'
                 AND b.RELA_CODE = 'a1'
                 AND b.prod_code = :vprod_code
                 AND a.id_no = to_number(:vid_no) AND a.THIRD_ID_NO = 0 ;
        if  (icount2 < 1 )
        {
            /** Ȼ���ж� �û��Ƿ����͹� ��Ҫ�����Ĳ�Ʒ**/
            icount2 =0;
            EXEC SQL select count(*)  into  :icount2
                FROM DDSMPORDERMSG  a ,SDSMPPRODRELATION b
               WHERE a.prod_code = b.RELA_PROD_CODE
                 AND (sysdate between  b.begin_time  and  b.end_time)
                 AND a.valid_flag  = '1'
                 AND b.RELA_CODE = 'a1'
                 AND b.prod_code = :vprod_code
                 AND a.THIRD_ID_NO = to_number(:vid_no)  ;
            if  (icount2 < 1)
           	{
                sprintf(retMsg, "û�ж�������ҵ���޷�������չҵ��[%s]!",vprod_code );
                PUBLOG_DBDEBUG("fDsmpProdRELATIONCheck");
           		pubLog_DBErr(_FFL_, "fDsmpProdRELATIONCheck", "1303", retMsg);
                return 1303;
            }
        }

    }

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fDsmpProdRELATIONCheck", "", "-- end -- fDsmpProdRELATIONCheck");
#endif

  return 0;
}


/** DSMP��Ʒ�ӿ���Ϣ����**/
int fPubDsmpSubInfo(DSMPORDEROpr *pOrderData,char *retMsg)
{
  int i = 0;
  int retCode = 0;
  EXEC SQL BEGIN DECLARE SECTION;
    char dynStmt[1024];
    char temp_op_accept[14+1];

    char orderpoint[1+1];
    char ordersync[1+1];
    char ordersynctable[30+1];
    char sTmpTime[5][14 + 1];
    int  iCount = 0;
  EXEC SQL END DECLARE SECTION;
	Sinitn(orderpoint);
    Sinitn(ordersync);
	Sinitn(ordersynctable);
	Sinitn(temp_op_accept);
    memset(sTmpTime, 0, sizeof(sTmpTime));

#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "-- begin -- fPubDsmpSubInfo   ");
#endif
    Sinitn(temp_op_accept);
    EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :temp_op_accept from dual;

  EXEC SQL  SELECT ORDERPOINT,ORDERSYNC,nvl(ORDERSYNCTABLE,'  ' )
              into  :orderpoint,:ordersync,:ordersynctable
            FROM  SOBBIZTYPE_dsmp
       WHERE  oprcode = :pOrderData->serv_code ;
    if  (SQLCODE != 0)
    {
    	pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->serv_code =[%s]", pOrderData->serv_code);
      	sprintf(retMsg, "��ȡSOBBIZTYPE_dsmp����[%s][%d]!", pOrderData->serv_code,SQLCODE);
      	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-50", retMsg);
      	return -50;
    }

    Trim(ordersynctable);
    if  (( strlen(ordersynctable) <= 0 )  && ( strcmp(ordersync,"1") == 0 ))
    {
    	pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "ordersynctable =[%s]", ordersynctable);
     	sprintf(retMsg, "SOBBIZTYPE_dsmp�е�ordersynctable�ֶβ���Ϊ��[%s][%d]!", ordersynctable,SQLCODE);
      	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-51", retMsg);
      	return -51;
    }


  /** ������ϵͬ�����ӿڱ�
      1��ordersync������ϵͬ����־��0����ͬ����1��BOSS-��ƽ̨��2��ƽ̨-��BOSS��Ĭ��0';��Ҫͬ������
      2�����ڷ���ƽ̨��Ϣ����Ϊ�첽����ģ���ͬ�����ݣ�������ƽ̨����ʱ��ͬ������
                    1��ͬ��  0���첽����;Ĭ��ͬ�� **/
  if ( ( (strcmp(ordersync,"1" ) == 0) && (strncmp(pOrderData->sync_flag,"0",1)!=0) )
      && (strncmp(pOrderData->multi_order_flag,"1",1) ==0 )  )
     {

      /*ת��ʱ���ʽ 'YYYYMMDD HH24:MI:SS'->'YYYYMMDDHHMISS'*/
        memset(sTmpTime, 0, sizeof(sTmpTime));
        if((i = fPubFormatTime(sTmpTime[0],pOrderData->opr_time)) != 0)
        {
            sprintf(retMsg,"opr_time����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[1],pOrderData->opr_time)) != 0)
        {
            sprintf(retMsg,"first_order_date����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[2],pOrderData->eff_time)) != 0)
        {
            sprintf(retMsg,"eff_time����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-26", retMsg);
        return -26;
        }
        if((i = fPubFormatTime(sTmpTime[3],pOrderData->end_time)) != 0)
        {
            sprintf(retMsg,"end_time����fPubFormatTimeʧ��! iRet=[%d]",i);
        	pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-26", retMsg);
        return -26;
        }

        Sinitn(temp_op_accept);
        EXEC SQL SELECT  sMaxSysAccept.nextVal INTO :temp_op_accept from dual;

      memset(dynStmt, 0, sizeof(dynStmt));
      sprintf(dynStmt, "  INSERT INTO %s   "
        "("
                  " MAXACCEPT,LOCALCODE,Misisdn,TMSISDN ,    "
                  " OprCode,EfftTime,Source,Channel ,        "
                  " ChgFlg,OrderStatus,BillFlg,SPID , SPBIZCODE,MaxScan ,infocode,infovalue , "
                  " BIZTYPE,PASSWORD ,REQTIME ,OPRTIME ,endtime "
        ")"
        "values"
        "("
              " :v1,:v2,:v3,:v4, "
              " :v5, :v6,:v7, :v8 , "
              " :v9,:v10,:v11, :v12,:v13,'0' ,:v14,:v15 ,:v16,:v17,:v18,:v19,:v20 "
        ")" ,ordersynctable );
#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "dynStmt=[%s]", dynStmt);
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "temp_op_accept =[%s]", temp_op_accept );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->local_code   =[%s]", pOrderData->local_code   );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->phone_no     =[%s]", pOrderData->phone_no     );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->third_phoneno=[%s]", pOrderData->third_phoneno);
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "sTmpTime[2]             =[%s]", sTmpTime[2]             );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->opr_source   =[%s]", pOrderData->opr_source   );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->channel_id   =[%s]", pOrderData->channel_id   );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->chg_flag     =[%s]", pOrderData->chg_flag     );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->opr_code     =[%s]", pOrderData->opr_code     );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->chrg_type    =[%s]", pOrderData->chrg_type    );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->send_spid         =[%s]", pOrderData->send_spid         );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->send_bizcode      =[%s]", pOrderData->send_bizcode      );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->infocode      =[%s]", pOrderData->infocode      );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->infovalue      =[%s]", pOrderData->infovalue      );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->serv_code         =[%s]", pOrderData->serv_code         );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "pOrderData->password      =[%s]", pOrderData->password      );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "sTmpTime[1]             =[%s]", sTmpTime[1]             );
    pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "sTmpTime[0]             =[%s]", sTmpTime[0]             );
#endif

      EXEC SQL PREPARE prepare1 From :dynStmt;
      EXEC SQL EXECUTE prepare1 USING :temp_op_accept,:pOrderData->local_code,:pOrderData->phone_no,:pOrderData->third_phoneno,
                          :pOrderData->opr_code,:sTmpTime[2], :pOrderData->opr_source,:pOrderData->channel_id,
                          :pOrderData->chg_flag,:pOrderData->opr_code,:pOrderData->chrg_type,:pOrderData->send_spid,
                          :pOrderData->send_bizcode ,:pOrderData->infocode ,:pOrderData->infovalue ,
                          :pOrderData->serv_code ,:pOrderData->password ,:sTmpTime[1] ,:sTmpTime[0],:sTmpTime[3] ;
      if (SQLCODE !=0)
      {
        sprintf(retMsg, "����ͬ���ӿڱ�[%s] ����[%d]!", ordersynctable, SQLCODE);
        pubLog_DBErr(_FFL_, "fPubDsmpSubInfo", "-25", retMsg);
        return -25;
      }
    }


#ifdef _DEBUG_
  pubLog_Debug(_FFL_, "fPubDsmpSubInfo", "", "-- end -- fPubDsmpSubInfo   ");
#endif

  return 0 ;
}

int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
        int  StringLen;
        char caNewString[MAX_MSG_LENGTH];

        char *FindPos = strstr(sSrc, sMatchStr);
        if( (!FindPos) || (!sMatchStr) )
		{
			pubLog_DBErr(_FFL_, "ReplaceStr", "-1", "");
	        return -1;
		}
        while( FindPos )
        {
                memset(caNewString, 0, sizeof(caNewString));
                StringLen = FindPos - sSrc;
                strncpy(caNewString, sSrc, StringLen);
                strcat(caNewString, sReplaceStr);
                strcat(caNewString, FindPos + strlen(sMatchStr));
                strcpy(sSrc, caNewString);

                FindPos = strstr(sSrc, sMatchStr);
        }

        return 0;
}


/** DSMP��Ʒ���� ������ʾ�ﴦ��**/
int fDsmpGetShortMsg(DSMPORDEROpr *pOrderData,char *send_msg,char *send_msg2,char *vRslt,char *retMsg)
{
	int i = 0;
	EXEC SQL BEGIN DECLARE SECTION;
			char dynStmt[2000];
   		char vid_no[14 +1];
   		char vphone_no[15 +1];
			char vorder_id[22 +1];
	    int  iCount = 0;
	    char vbillingtype[2+1];   /** �Ʒ�����**/
	    char vold_billtype[2+1];  /** ���֮ǰ�ļƷ����� **/
	    char vsend_msg[2000];
	    char vsend_msg_tmp[2000];
	    char vparam_ab[30 +1];
	    char vparam_value[30+1];
	    char vtemp_msg[2000];
	    char vparam_code[6+1];
	    char vtemp_value[1024];
	    char vportalcode[8+1];
	    char vtemp_spname[60+1];
	    char vtemp_servname[60+1];
	    char vtemp_price[10+1];
	    char vtemp_param_l[300+1];
	    char vmsg_oprcode[2 +1];
	    char vMemLevel[1+1];				/**��Ա����**/

	    char param_c[32    +1];
	    char param_s[64    +1];
	    char param_d[32    +1];
	    char param_m[32   +1];
	    char param_f[20    +1];
	    char param_F[20    +1];
	    char param_t[2    +1];
	    char param_k[30    +1];
	    char param_l[1024 +1];
	    char param_r[6    +1];
	    char param_w[6    +1];
	    char param_n[11   +1];
	    char param_a[8    +1];
	    char param_v[1024 +1];
	    char param_p[6    +1];
	    char param_q[4    +1];
	    char param_u[2    +1];
	    char param_e[2    +1];
	    char param_y[2    +1];
	    char param_x[100  +1];
	    char param_z[50   +1];
			/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
	    char param_H[2    +1];
	    char param_M[2    +1];
	    char param_S[2    +1];
			/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/
			/*add by shijing at 20121113 begin*/
			char	param_b[17    +1];
			char	param_g[18    +1];
			/*add by shijing at 20121113 end*/
	    char vspid[20+1];
	    char vbizcode[21+1];
	    char vserv_code[2+1];
	    char vopr_source[2+1];
	    char vop_code[6+1];
	    char vopr_code[2+1];

			char vserv_property[2+1];
			char vfirst_order_flag[1+1];   /**�״ζ�����־**/
			char vbefore_21_flag[1+1];     /**�Ƿ�21��֮ǰ����**/
			char vthird_opr_flag[1+1];     /**�����ͷ�������־**/
			char vmsgtype_id[14+1];
			char vcfm_order_flag[1+1];     /** �ظ����Ƿ��Ʊ�־;����ʷѱ������Ķ���ȷ�� **/
			char vnocfm_order_flag[1+1];   /** �޻ظ��Ƿ��Ʊ�־;����ʷѱ������Ķ���ȷ�� **/
			char vDeDuctMode[1+1];
			char vDayConvert[1+1];
			char vSpType[1+1];

			char temp_first_order_date[18];
			long temp_time = 0;
			char temp_msgtyped_id[3+1];
			char vModeFlag[1+1];
	EXEC SQL END DECLARE SECTION;

    Sinitn(vcfm_order_flag);
    Sinitn(vnocfm_order_flag);
    Sinitn(vold_billtype);
    Sinitn(vmsgtype_id);
    Sinitn(vserv_property);
    Sinitn(vfirst_order_flag);
    Sinitn(vbefore_21_flag);
    Sinitn(vthird_opr_flag);
    Sinitn(vDeDuctMode);
    Sinitn(vDayConvert);
    Sinitn(vSpType);
		Sinitn(vspid);
		Sinitn(vbizcode);
		Sinitn(vserv_code);
    Sinitn(vopr_source);
		Sinitn(vop_code);
		Sinitn(vopr_code);

		Sinitn(vid_no);
		Sinitn(vorder_id);
		Sinitn(vphone_no);
		Sinitn(vbillingtype);
		Sinitn(vsend_msg);
		Sinitn(vsend_msg_tmp);
		Sinitn(vparam_ab);
		Sinitn(vparam_value);
		Sinitn(vtemp_msg);
		Sinitn(vtemp_spname);
		Sinitn(vtemp_servname);
		Sinitn(vtemp_price);
		Sinitn(vtemp_param_l);

    Sinitn(param_c	);
    Sinitn(param_s	);
    Sinitn(param_d	);
    Sinitn(param_m	);
    Sinitn(param_f  );
    Sinitn(param_F  );
    Sinitn(param_t  );
    Sinitn(param_k  );
    Sinitn(param_l  );
    Sinitn(param_r  );
    Sinitn(param_w  );
    Sinitn(param_n  );
    Sinitn(param_a  );
    Sinitn(param_v  );
    Sinitn(param_p  );
    Sinitn(param_q  );
    Sinitn(param_u  );
    Sinitn(param_e  );
    Sinitn(param_y  );
    Sinitn(param_x  );
    Sinitn(param_z  );
    /*add by shijing at 20121113 begin*/
    Sinitn(param_b);
    Sinitn(param_g);
    /*add by shijing at 20121113 end*/
		/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
		Sinitn(param_H);
		Sinitn(param_M);
		Sinitn(param_S);
		/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/
    Sinitn(vparam_code);
    Sinitn(vtemp_value);
    Sinitn(vmsg_oprcode);
    Sinitn(vModeFlag);
    Sinitn(temp_first_order_date);
    Sinitn(temp_msgtyped_id);

    strcpy(vid_no,	pOrderData->id_no);
		strcpy(vphone_no,	pOrderData->phone_no);
		strcpy(vmsg_oprcode,pOrderData->msg_oprcode);  /** ���Ų�����ʾ��������� **/
		strcpy(vspid,	pOrderData->spid);
		strcpy(vbizcode,pOrderData->bizcode);
		strcpy(vserv_code,pOrderData->serv_code);
		strcpy(vopr_source,pOrderData->opr_source);
		strcpy(vop_code,pOrderData->op_code);
		strcpy(vopr_code,pOrderData->opr_code);

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "--  begin -- fDsmpGetShortMsg   ");
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vid _no              =[%s]", vid_no  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "pOrderData->order_id =[%ld]", pOrderData->order_id );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vRslt 							 =[%s]",vRslt);
#endif


		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "��ʼ�жϲ��������ȷ�ϲ�������!");
    /** ���ڶ���ȷ�ϵȲ���û�ж����Ĳ������� **/
    Trim(vmsg_oprcode);
    if (strlen(vmsg_oprcode) <1 )
    {
        strcpy(vmsg_oprcode,pOrderData->opr_code);
    }

		/** SPҵ������	%s  ,�ʷ�(�����) %F**/
    memset(dynStmt,0,sizeof(dynStmt));
    sprintf(dynStmt," select nvl(SERVNAME,' ') ,nvl(billingtype,' '),to_char(nvl(PRICE,0) ),nvl(SERV_PROPERTY,'Z'),nvl(deduct_mode,' '),nvl(day_convert,' ') from dDsmpSPBizinfo  "
                    "  where SPID  =:v1 AND BIZCODE   =:v2 and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd')"   );
    EXEC SQL EXECUTE
    BEGIN
        EXECUTE IMMEDIATE :dynStmt into  :param_s ,:vbillingtype ,:param_F ,:vserv_property,:vDeDuctMode,:vDayConvert
                 using :vspid,:vbizcode;
    END;
    END-EXEC;
		if ( (SQLCODE !=0) && (SQLCODE !=1403 ))
		{
			sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��dDsmpSPBizinfo����[%d]!", SQLCODE);
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-204", retMsg);
			return -204;
		}
		Trim(param_s);
		Trim(vbillingtype);
		Trim(param_F);
		Trim(vserv_property);
		Trim(vDeDuctMode);
		Trim(vDayConvert);
		/**BIZ_OPR_DEST-06(����);BIZ_OPR_11-11(����);BIZ_OPR_21-21(����ȷ��);BIZ_OPR_REGIST-01(ע��);BIZ_OPR_13-13(�������ֱ��)**/
		if  ( (strncmp(pOrderData->opr_code,BIZ_OPR_DEST,2) == 0)
    			|| (strncmp(pOrderData->opr_code,BIZ_OPR_11,2) == 0)
          || (strncmp(pOrderData->opr_code,BIZ_OPR_21,2) == 0)
          || (strncmp(pOrderData->opr_code,BIZ_OPR_REGIST,2) == 0)
          || (strncmp(pOrderData->opr_code,BIZ_OPR_13,2) == 0) )
    {
				/*�ж��Ƿ��ǵ�һ�ζ���*/
        pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "pOrderData->first_order_date,%s",pOrderData->first_order_date);
        pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "pOrderData->eff_time,%s",pOrderData->eff_time);
        if  (strcmp(pOrderData->first_order_date ,pOrderData->eff_time  ) == 0 )
        {
            strcpy(vfirst_order_flag,"1");
        }
        else
        {
            strcpy(vfirst_order_flag,"0");
        }
				memcpy(param_e,pOrderData->total_date +6,2);
				/*�Զ������ڽ��д���(21��֮ǰ����֮��)*/
				pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "param_e: %s",param_e);
        if (atol(param_e) < 21)
        {
            strcpy(vbefore_21_flag,"1");
        }
        else
        {
            strcpy(vbefore_21_flag,"0");
        }
    }
    else
    {
        strcpy(vfirst_order_flag,"Z");
        strcpy(vbefore_21_flag,"Z");
    }

    if ( (strncmp(vserv_code,"04",2) != 0)  && (strncmp(vserv_code,"05",2) != 0) )
    {
        strcpy(vserv_property,"Z");     /** ϵͳĬ��ֵ**/
    }

		/** �ʷ�	%f :     9118 �ʷ���Ϣ�������Ķ�����ϵ������� **/
		if (strncmp(pOrderData->op_code ,"9118",4) ==0)
		{
        memset(dynStmt,0,sizeof(dynStmt));
        memset(vold_billtype,0,sizeof(vold_billtype));
        sprintf(dynStmt," select nvl(billingtype,' '),to_char(nvl(PRICE,0) ) from  dDsmpSPBizinfoHis  "
           " where SPID  =:v1 AND BIZCODE   =:v2  "
           " AND update_accept =(select max(update_accept) from dDsmpSPBizinfoHis where SPID =:v3 AND BIZCODE =:v4 ) " );
        EXEC SQL EXECUTE
        BEGIN
            EXECUTE IMMEDIATE :dynStmt into  :vold_billtype ,:param_f using :vspid,:vbizcode , :vspid,:vbizcode;
        END;
        END-EXEC;
	    	if ( (SQLCODE !=0) && (SQLCODE !=1403 ))
	    	{
	    		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��dDsmpSPBizinfo����[%d]!", SQLCODE);
	    		PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-50", retMsg);
	    		return -204;
	    	}
	    	Trim(vold_billtype);
	    	Trim(param_f);

	    	if ((strncmp(vold_billtype ,"2",1) ==0) )
	    	{
	    		/**0:���,1:����,2:����**/
	        sprintf(param_f,"%sԪ/��",param_f);
	    	}
	    	else if  (strncmp(vold_billtype ,"1",1) ==0)
	    	{
	        sprintf(param_f,"%sԪ/��",param_f);
	    	}else if  (strncmp(vold_billtype ,"0",1) ==0)
	    	{
	        sprintf(param_f,"%sԪ/��",param_f);
	    	}

#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vold_billtype=[%s]",vold_billtype );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "param_f=[%s]",param_f );
#endif

				if ( (strcmp(vbillingtype,"1") == 0) && (strcmp(vold_billtype,"1") != 0) )
        {
            /** ���¡������շѱ���Ѷ���ȷ����ʾ�� vRslt **/
            strcpy(vRslt , "88");
        }
		}

    /****ʵ�ַ�ʽ�޸�ע�͵� 20101117*/
    if(strncmp(pOrderData->serv_code ,"02",2)==0&&strcmp(vopr_code,"10")==0)
		{
				strcpy(vserv_property,"A");
		}
		if(strncmp(pOrderData->serv_code ,"02",2)==0&&strcmp(vopr_code,"12")==0)
		{
				strcpy(vserv_property,"B");
		}



	/**---------- ��ȡ������ʾ���ʽ ----------  **/
	/* 20081105 yangzc �޸Ķ�����ʾ�� ��������,���ع�*/
	memset(dynStmt,0,sizeof(dynStmt));
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "yangzc test vRslt: %s,vmsg_oprcode=[%s]",vRslt,vmsg_oprcode);
	/**2012-11-05 ��ֵҵ����������������  chendh shijing deduct_mode��day_convert �ֶ�δ����ʱ**/
	if(strlen(vDeDuctMode)==0 && strlen(vDayConvert)==0)
	{
		printf("\nvDeDuctMode=0\n");
		sprintf(dynStmt," select Trim(CONTENT),to_char(a.MSGTYPEID),nvl(cfm_order_flag,' '),nvl(nocfm_order_flag,' ') "
	         " from  SDSMPSMSMSGTYPE  a, SDSMPSMSSENDCODE  b  "
	         "  where a.MSGTYPEID =b.MSGTYPEID  AND b.OPR_CODE = decode(:v1,'89','07','99','07',:v2) AND b.SERV_CODE = :v3 "
	         "  AND b.OPR_SOURCE =:v4 AND b.CHRG_TYPE =:v5 and b.RSLT_CODE =:v6 "
	         "  and (b.serv_property =:v7 or b.serv_property ='Z') "
	         "  and (b.first_order_flag =:v8 or b.first_order_flag ='Z')   "
	         "  and (b.before_21_flag =:v9 or b.before_21_flag ='Z')   "
	         "  and (b.deduct_mode is null and  b.day_convert is null)   "
	         "  AND  b.third_opr_flag = '0'  and rownum < 2 " );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);
		EXEC SQL EXECUTE
		BEGIN
	    EXECUTE IMMEDIATE :dynStmt into  :vsend_msg ,:vmsgtype_id,:vcfm_order_flag ,:vnocfm_order_flag
	            using :vmsg_oprcode ,:vmsg_oprcode,
	         :vserv_code,  :vopr_source,:vbillingtype,:vRslt ,:vserv_property , :vfirst_order_flag ,:vbefore_21_flag ;
		END;
		END-EXEC;
	}else{
		printf("\n vDeDuctMode=%s,vDayConvert=%s\n",vDeDuctMode,vDayConvert);
		sprintf(dynStmt," select Trim(CONTENT),to_char(a.MSGTYPEID),nvl(cfm_order_flag,' '),nvl(nocfm_order_flag,' ') "
	         " from  SDSMPSMSMSGTYPE  a, SDSMPSMSSENDCODE  b  "
	         "  where a.MSGTYPEID =b.MSGTYPEID  AND b.OPR_CODE = decode(:v1,'89','07','99','07',:v2) AND b.SERV_CODE = :v3 "
	         "  AND b.OPR_SOURCE =:v4 AND b.CHRG_TYPE =:v5 and b.RSLT_CODE =:v6 "
	         "  and (b.serv_property =:v7 or b.serv_property ='Z') "
	         "  and (b.first_order_flag =:v8 or b.first_order_flag ='Z')   "
	         "  and b.deduct_mode =:v9  and b.day_convert =:v10"
	         "  AND  b.third_opr_flag = '0'  and rownum < 2 " );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);
		EXEC SQL EXECUTE
		BEGIN
	    EXECUTE IMMEDIATE :dynStmt into  :vsend_msg ,:vmsgtype_id,:vcfm_order_flag ,:vnocfm_order_flag
	            using :vmsg_oprcode ,:vmsg_oprcode,
	         :vserv_code,  :vopr_source,:vbillingtype,:vRslt ,:vserv_property , :vfirst_order_flag,:vDeDuctMode,:vDayConvert;
		END;
		END-EXEC;
	}

	if (SQLCODE != 0 && SQLCODE != 1403)
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vmsg_oprcode     =[%s]", vmsg_oprcode  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vserv_code       =[%s]", vserv_code  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vopr_source      =[%s]", vopr_source );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbillingtype     =[%s]", vbillingtype  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vRslt            =[%s]", vRslt  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vserv_property   =[%s]", vserv_property  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vfirst_order_flag=[%s]", vfirst_order_flag  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbefore_21_flag  =[%s]", vbefore_21_flag  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vDeDuctMode  		=[%s]", vDeDuctMode  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vDayConvert  		=[%s]", vDayConvert  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "SQLCODE=[%d]",SQLCODE );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt=[%s]",dynStmt );
#endif
		PUBLOG_DBDEBUG("fDsmpGetShortMsg");
		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-220", retMsg);
		return -220;
	}
	else
	{
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "��ѯ�����������б�����\n");
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vmsg_oprcode     =[%s]", vmsg_oprcode  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vserv_code       =[%s]", vserv_code  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vopr_source      =[%s]", vopr_source );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbillingtype     =[%s]", vbillingtype  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vRslt            =[%s]", vRslt  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vserv_property   =[%s]", vserv_property  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vfirst_order_flag=[%s]", vfirst_order_flag  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbefore_21_flag  =[%s]", vbefore_21_flag  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vDeDuctMode  		=[%s]", vDeDuctMode  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vDayConvert  		=[%s]", vDayConvert  );
#endif

		/** ����ܹ��ҵ�msgtype_id�Ļ�,��ȥSDSMPSMSMSGCODE�Ҹ��Ի���ʾ��**/
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "��ѯ�������ѯ���\n");
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vmsgtype_id: %s",vmsgtype_id);
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "spid: %s",vspid);
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbizcode: %s",vbizcode);
		memset(vsend_msg_tmp,0,sizeof(vsend_msg_tmp));
		memset(dynStmt,0,sizeof(dynStmt));
		sprintf(dynStmt," select  a.CONTENT from  SDSMPSMSMSGCODE  a   "
		           "  where  a.SPID = :v1  AND a.BIZCODE =:v2 and MSGTYPEID =:v3"   );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);
		EXEC SQL EXECUTE
		BEGIN
		    EXECUTE IMMEDIATE :dynStmt into  :vsend_msg_tmp using :vspid,:vbizcode,:vmsgtype_id;
		END;
		END-EXEC;
		if ((SQLCODE !=0) &&  (SQLCODE !=1403) )
		{
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
			pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-205", retMsg);
			return -205;
		}
		else if (SQLCODE == 0 )
		{
			/** ��Ĭ��ֵ���������Żظ����������ظ�ȡ��**/
			strcpy(vcfm_order_flag,	"1");
			strcpy(vnocfm_order_flag,	"0");
		}
		/**���̶�ģ�岻Ϊ��ʱʹ�ù̶�ģ���滻**/
		Trim(vsend_msg_tmp);
		if(strlen(vsend_msg_tmp)!=0)
		{
			memset(vsend_msg,0,sizeof(vsend_msg));
			strcpy(vsend_msg,vsend_msg_tmp);
		}

	}
	/* 20081105 yangzc �޸Ľ���*/
	/* yangzc 20081108�޸Ķ�����ʾ��ģ��*/
	/*����û��������Ƕ��Ż��߲��ŵĻ�,���⴦�������ʾ��*/
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vserv_code333:%s",vserv_code);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid333:%s",vspid);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbizcode333:%s",vbizcode);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vfirst_order_flag333:%s",vfirst_order_flag);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vbillingtype333:%s",vbillingtype);
	if(strcmp(vserv_code,"03") == 0)
	{
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "yangzc test 11111111111111111");
		if(strcmp(vspid,"900137") == 0 && strcmp(vbizcode,"13000023") == 0 && strcmp(vfirst_order_flag,"0") == 0 && strcmp(vbillingtype,"2") == 0)
		{
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "yangzc test 22222222222222222222");
			strcpy(temp_msgtyped_id,"106");
			memset(dynStmt,0,sizeof(dynStmt));
			sprintf(dynStmt," select Trim(CONTENT) from  SDSMPSMSMSGTYPE a where a.MSGTYPEID = :v9");
			EXEC SQL EXECUTE
		  	BEGIN
				EXECUTE IMMEDIATE :dynStmt into  :vsend_msg  using :temp_msgtyped_id ;
		  END;
		  END-EXEC;
			if (SQLCODE !=0)
			{
				PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-226", retMsg);
				return -226;
			}
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "�ֻ�֤��Ķ���:%s",vsend_msg);
		}
	}

	if((strcmp(vserv_code,"04") == 0 || strcmp(vserv_code,"05") == 0)&&(strlen(vDeDuctMode)==0 && strlen(vDayConvert)==0))
	{
		/* ����û��Ƿ��״ζ���,���Ҵ���ʱ�����ڵ��µ�21��֮��,���ǰ���ҵ��*/
		if(strcmp(vfirst_order_flag,"0") == 0 && strcmp(vbefore_21_flag,"0") == 0 && strcmp(vbillingtype,"2") == 0)
		{
			strcpy(temp_first_order_date,pOrderData->first_order_date);
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "�û��״ζ���ʱ��: %s",temp_first_order_date);
			EXEC SQL select to_number(to_char((to_date(:temp_first_order_date,'yyyymmdd hh24:mi:ss')),'yyyymmddhh24miss')) - to_number(to_char(sysdate,'yyyymm')||'21000000') into :temp_time from dual;
			if (SQLCODE !=0){
				sprintf(retMsg, "fDsmpGetShortMsg:��ȡ�û��״ζ���ʱ�����[%d]!", SQLCODE);
				PUBLOG_DBDEBUG("fDsmpGetShortMsg");
				pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
				return -231;
			}
			if(temp_time >= 0)
			{
				if(strcmp(pOrderData->opr_code,"11") != 0)
				{
					strcpy(temp_msgtyped_id,"103");
				}else{
					strcpy(temp_msgtyped_id,"903");
				}
			}else{
				if(strcmp(pOrderData->opr_code,"11") != 0)
				{
					strcpy(temp_msgtyped_id,"106");
				}else{
					strcpy(temp_msgtyped_id,"906");
				}
			}
			memset(dynStmt,0,sizeof(dynStmt));
			sprintf(dynStmt," select Trim(CONTENT) from  SDSMPSMSMSGTYPE a where a.MSGTYPEID = :v9");
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);
		  EXEC SQL EXECUTE
		  BEGIN
				EXECUTE IMMEDIATE :dynStmt into  :vsend_msg using :temp_msgtyped_id;
		  END;
		  END-EXEC;
			if (SQLCODE !=0)
			{
				PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-225", retMsg);
				return -225;
			}
		}
	}
	/* yangzc 20081108�޸Ķ�����ʾ��ģ��end*/
	Trim(vsend_msg);
	Trim(vmsgtype_id);
	Trim(vcfm_order_flag);
	Trim(vnocfm_order_flag);
	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vsend_msg=[%s]",vsend_msg );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vmsgtype_id=[%s]",vmsgtype_id );
    #endif

/**---------- ������ʾ�������ֵ----------  **/
    /**���%q/�·�	%u/��%e/����%y **/
    memcpy(param_q,pOrderData->total_date +0,4);
    memcpy(param_u,pOrderData->total_date +4,2);
    memcpy(param_e,pOrderData->total_date +6,2);
    memcpy(param_y,pOrderData->total_date +4,2);
    if (strncpy(param_y,"0",1) == 0){
        memcpy(param_y,param_y +1,1);
    }

/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
		memcpy(param_H,pOrderData->opr_time + 9,2);
		memcpy(param_M,pOrderData->opr_time + 12,2);
		memcpy(param_S,pOrderData->opr_time + 15,2);
/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/



    /** �ӳټƷѵ�Сʱ��	%p �״ζ�������21��֮ǰ ��72Сʱ�ڼƷ� **/
    if ( (atol(param_e) < 21)  &&  (strcmp(pOrderData->first_order_date ,pOrderData->eff_time  ) == 0 ))
    {
        strcpy(param_p,"72");
    }

    /**���ͷ�/�����ͷ��ֻ�����	%n , �������	%a**/
    strcpy(param_n,	pOrderData->third_phoneno);
    strcpy(param_a,	pOrderData->password);
    Trim(param_n);
    Trim(param_a);

    /** ҵ������	%k **/
    /*
    memset(dynStmt,0,sizeof(dynStmt));
    sprintf(dynStmt," select substr(OPRNAME,1,30),portalcode from sobbiztype_dsmp WHERE OPRCODE  =:v1 " );
    EXEC SQL EXECUTE
    BEGIN
         EXECUTE IMMEDIATE :dynStmt into :param_k ,:vportalcode  using :vserv_code;
    END;
    END-EXEC;
    */
    pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "select substr(OPRNAME,1,30),portalcode into :param_k ,:vportalcode from sobbiztype_dsmp WHERE OPRCODE  = %s ", vserv_code);
    EXEC SQL select substr(OPRNAME,1,30),portalcode into :param_k ,:vportalcode
    				 from sobbiztype_dsmp
    				 WHERE OPRCODE  =:vserv_code ;


	if (SQLCODE !=0)
	{
		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��sobbiztype_dsmp����[%d]!", SQLCODE);
		PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-201", retMsg);
		return -201;
	}
	Trim(param_k);
	Trim(vportalcode);

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "ҵ������	k " );
  #endif

    /**����ȷ�ϻظ�ȷ����	%x **/
    memset(dynStmt,0,sizeof(dynStmt));
    sprintf(dynStmt," select nvl(substr(BACK_CODE,1,100),' ')  from SDSMPRECONFIRM   "
                    " where SPID  =:v1 AND BIZCODE   =:v2 "
                    "   AND OPR_SOURCE  =:v3  AND OP_CODE  =:v4 ");
    EXEC SQL EXECUTE
    BEGIN
                       EXECUTE IMMEDIATE :dynStmt into :param_x using :vspid,:vbizcode,
                       :vopr_source,:vop_code ;
    END;
    END-EXEC;
	if ( (SQLCODE !=0) && (SQLCODE !=1403 ))
	{
		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��SDSMPRECONFIRM����[%d]!", SQLCODE);
		PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-202", retMsg);
		return -202;
	}
	Trim(param_x);
	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "����ȷ�ϻظ�ȷ���� [%s]",param_x );
    #endif

	/** �Ƿ��з������	%z,SP��˾���	%c,SP�ͷ��绰	%m ,SP�������	%d
	if ( (strncmp(pOrderData->serv_code ,"04",2) ==0) ||(strncmp(pOrderData->serv_code ,"05",2) ==0) ) {    **/

    memset(dynStmt,0,sizeof(dynStmt));
    sprintf(dynStmt," select nvl(SPSVCID,' '),nvl(SPSHORTNAME,' ') spshortname,nvl(CSRTEL,' '),sptype  from dDsmpSPinfo where SPID =:v1 and sysdate between to_date(validdate,'yyyymmdd') and to_date(expiredate,'yyyymmdd')"  );
    EXEC SQL EXECUTE
    BEGIN
                EXECUTE IMMEDIATE :dynStmt into :param_d,:param_c,:param_m,:vSpType using :vspid ;
    END;
    END-EXEC;
	if ( (SQLCODE !=0) && (SQLCODE !=1403 ))
	{
		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��dDsmpSPinfo����[%d]!", SQLCODE);
		PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-203", retMsg);
		return -203;
	}
	Trim(param_d);
	Trim(param_c);
	Trim(param_m);
	if (strlen(param_d) > 0)
	{
	    strcpy(param_z,	",�������");
	}

	/** 1862��ѯ����	%r  ����:186201��WAP:186202������:186203
	    �˶���ʽ	%w     �˶����ţ�CX0000,������ 0000
	    ��Ϊ���Ų���ʱ��%z��ȡֵΪ����������롱����ΪWapʱ��%zΪ��**/
	if (strncmp(pOrderData->serv_code ,"03",2) ==0)
	{
	    strcpy(param_z,	" ");
	    strcpy(param_r,	"186202");
	    strcpy(param_w,	"0000");
	}
	else if (strncmp(pOrderData->serv_code ,"04",2) ==0)
	{  /**SMS**/
	    strcpy(param_r,	"0000");
	    strcpy(param_w,	"0000");
	}
	else if (strncmp(pOrderData->serv_code ,"05",2) ==0)
	{  /**MMS**/
	    strcpy(param_r,	"186203");
	    strcpy(param_w,	"CX0000");
	}
	else
	{
	    strcpy(param_w,	"0000");
	}

	#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "1862��ѯ���� �˶���ʽ " );
    #endif

	/**���ڰ��¼Ʒ�ҵ���ʷѵ�λΪԪ/�£����ڰ����Ʒ�ҵ���ʷѵ�λΪԪ/��
	   0:���,1:����,2:���� **/
	if (strncmp(vbillingtype ,"2",1) ==0)
	{
	        sprintf(param_F,"%sԪ/��",param_F);
	}
	else if  (strncmp(vbillingtype ,"1",1) ==0)
	{
					sprintf(param_F,"%sԪ/��",param_F);
	}
	else if  (strncmp(vbillingtype ,"0",1) ==0)
	{
					sprintf(param_F,"%sԪ/��",param_F);
	}
    if (strncmp(pOrderData->op_code ,"9118",4) !=0)
    {  /**���ʷѱ��ʱ,�ʷ�(�����)��ֵ��Ϊ��ǰ�ʷ�**/
        strcpy(param_f,param_F);
    }

    /**�Ѷ�������ҵ���嵥	����Ӫҵ�� %l **/
    if ( (strncmp(vportalcode ,"DSMP",4) ==0) &&  (strncmp(pOrderData->opr_source ,"32",2) ==0) )
    {
    	memset(dynStmt,0,sizeof(dynStmt));
    	memset(param_l,0,sizeof(param_l));
		sprintf(dynStmt, " select nvl(b.spshortname,' ') spshortname,c.SERVNAME,c.billingtype,to_char(nvl(PRICE,0) ) "
		                  " from DDSMPORDERMSG   a, dDsmpSpInfo b,dDsmpSPBizInfo c   "
		                  " WHERE a.spid = b.spid and b.spid = c.spid and a.bizcode = c.bizcode "
		  				  "	AND ( (a.id_no=to_number(:v1) and a.THIRD_ID_NO = 0 ) OR a.THIRD_ID_NO = to_number(:v2) ) "
						  "	and sysdate between a.EFF_TIME  and a.end_time  AND  a.serv_code =:v3  and sysdate between to_date(b.validdate,'yyyymmdd') and to_date(b.expiredate,'yyyymmdd') and sysdate between to_date(c.validdate,'yyyymmdd') and to_date(c.expiredate,'yyyymmdd')"  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);

    EXEC SQL PREPARE preStmt FROM :dynStmt;
	  EXEC SQL DECLARE vCurOrder CURSOR for preStmt;
		EXEC SQL OPEN vCurOrder using :vid_no,:vid_no ,:pOrderData->serv_code  ;
		EXEC SQL FETCH vCurOrder into :vtemp_spname ,:vtemp_servname ,:vbillingtype ,:vtemp_price ;

		if (SQLCODE == 1403)
		{
			sprintf(retMsg, "û���ҵ���ҵ��Ķ�����Ϣ[%d]!",  SQLCODE);
			EXEC SQL CLOSE vCurOrder;
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-250", retMsg);
			return -250;
		}
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "yangzc test1111 vRslt: %s",vRslt);

        while(SQLCODE == 0)
        {
            Trim(vtemp_spname);
            Trim(vtemp_servname);
            Trim(vtemp_price);

	        if (strncmp(vbillingtype ,"2",1) ==0)
	        {
	            sprintf(vtemp_param_l,"%s,%s,%sԪ/��",vtemp_spname ,vtemp_servname ,vtemp_price);
	        }
	         else if  (strncmp(vbillingtype ,"1",1) ==0)
	        {
	            sprintf(vtemp_param_l,"%s,%s,%sԪ/��",vtemp_spname ,vtemp_servname ,vtemp_price);
	        }
	        else if (strncmp(vbillingtype ,"7",1) ==0)
	        {
	            sprintf(vtemp_param_l,"%s,%s,%sԪ/��",vtemp_spname ,vtemp_servname ,vtemp_price);
	        }

            Trim(param_l);
            if (strlen(param_l) > 0)
            {
                sprintf(param_l,"%s;%s",param_l,vtemp_param_l );
            }
            else
            {
                sprintf(param_l,"%s",vtemp_param_l );
            }

       	    EXEC SQL FETCH vCurOrder into :vtemp_spname ,:vtemp_servname ,:vbillingtype ,:vtemp_price ;
       	}
       	EXEC SQL CLOSE vCurOrder;

    }

    /**ĳһSP����ҵ���嵥 ����Ӫҵ�� %k **/
    if ( (strncmp(vportalcode ,"DSMP",4) ==0) &&  (strncmp(pOrderData->opr_source ,"32",2) ==0) )
    	{
    	memset(dynStmt,0,sizeof(dynStmt));
    	memset(param_k,0,sizeof(param_k));
    	memset(vtemp_servname,0,sizeof(vtemp_servname));
    	memset(vbillingtype,0,sizeof(vbillingtype));
    	memset(vtemp_price,0,sizeof(vtemp_price));
    	memset(vtemp_param_l,0,sizeof(vtemp_param_l));

		sprintf(dynStmt, " select c.SERVNAME,c.billingtype,to_char(nvl(PRICE,0) ) "
		                  " from DDSMPORDERMSG   a, dDsmpSpInfo b,dDsmpSPBizInfo c   "
		                  " WHERE a.spid = b.spid and b.spid = c.spid and a.bizcode = c.bizcode "
		  				  "	AND ( (a.id_no=to_number(:v1) and a.THIRD_ID_NO = 0 ) OR a.THIRD_ID_NO = to_number(:v2) ) "
						  "	and sysdate between a.EFF_TIME  and a.end_time  AND  a.spid =:v3  and sysdate between to_date(b.validdate,'yyyymmdd') and to_date(b.expiredate,'yyyymmdd') and sysdate between to_date(c.validdate,'yyyymmdd') and to_date(c.expiredate,'yyyymmdd')"  );
		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);

    EXEC SQL PREPARE preStmt FROM :dynStmt;
	  EXEC SQL DECLARE vCurOrder2 CURSOR for preStmt;
		EXEC SQL OPEN vCurOrder2 using :vid_no,:vid_no ,:pOrderData->spid  ;
		EXEC SQL FETCH vCurOrder2 into :vtemp_servname ,:vbillingtype ,:vtemp_price ;

		if (SQLCODE == 1403)
		{
			sprintf(retMsg, "û���ҵ���ҵ��Ķ�����Ϣ[%d]!",  SQLCODE);
			EXEC SQL CLOSE vCurOrder;
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-260", retMsg);
			return -260;
		}

        while(SQLCODE == 0)
        {
            Trim(vtemp_servname);
            Trim(vtemp_price);

	        if (strncmp(vbillingtype ,"2",1) ==0)
	        {
	            sprintf(vtemp_param_l,"%s,%sԪ/��", vtemp_servname ,vtemp_price);
	        } else if  (strncmp(vbillingtype ,"1",1) ==0)  {
	            sprintf(vtemp_param_l,"%s,%sԪ/��", vtemp_servname ,vtemp_price);
	        }else if (strncmp(vbillingtype ,"7",1) ==0) {
	            sprintf(vtemp_param_l,"%s,%sԪ/��", vtemp_servname ,vtemp_price);
	        }

            Trim(param_k);
            if (strlen(param_k) > 0)
            {
                sprintf(param_k,"%s;%s",param_k,vtemp_param_l );
            } else {
                sprintf(param_k,"%s",vtemp_param_l );
            }

       	    EXEC SQL FETCH vCurOrder2 into :vtemp_servname ,:vbillingtype ,:vtemp_price ;
       	}
       	EXEC SQL CLOSE vCurOrder2;
    }
	/**wlan������**/
	if(strncmp(pOrderData->serv_code ,"02",2) ==0 && strcmp(vopr_code,"01")!=0 )
	{
		if(strcmp(vopr_code,"10")==0||strcmp(vopr_code,"12")==0)
		{
			EXEC SQL select time_hour, mode_money, mode_flag
					   into :param_p , :param_F, :vModeFlag
					   from oboss.sobmodecode
					  where wlan_mode=:vbizcode;
			if(SQLCODE!=0&&SQLCODE!=1403)
			{
				sprintf(retMsg, "û���ҵ���ҵ��Ķ�����Ϣ[%d]!",  SQLCODE);
				PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-302", retMsg);
				return -302;
			}
			Trim(param_p);
			Trim(param_F);
			if(strcmp(vbizcode,"00012") ==0||strcmp(vbizcode,"00013") ==0)
			{
				strcat(param_p,"G");
			}
			else if(strcmp(vbizcode,"00011") ==0||strcmp(vbizcode,"00014") ==0)
			{
				strcat(param_p,"M");
			}
                         if(strcmp(vbizcode,"00014") == 0)
			{
				EXEC SQL SELECT b.time_hour, b.mode_money
									into :param_p , :param_F
							  FROM ddsmpordermsg a,sobmodecode b
							 WHERE id_no = :vid_no
							   AND serv_code = '02'
							   and a.bizcode =trim(b.wlan_mode)
							   and b.mode_group = '0'
							   AND opr_code not IN ('02', '07','04')
							   AND (sysdate between eff_time and end_time);
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(retMsg, "û���ҵ����û�����Ϣ[%d]!",  SQLCODE);
					PUBLOG_DBDEBUG("fDsmpGetShortMsg");
		  			pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-303", retMsg);
					return -303;
				}


			}

		}
		if(strcmp(vopr_code,"10")==0)
		{
			if(strcmp(vModeFlag,"0")==0)
			{
				EXEC SQL select phone_no ,password
						   into :param_n,:param_a
						   from ddsmpordermsg
						  where id_no=:vid_no
						  	and serv_code='02'
        				  	and opr_code in('01','03','05','09','15')
        				  	and end_time>sysdate;
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(retMsg, "û���ҵ����û�����Ϣ[%d]!",  SQLCODE);
					PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    			pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-303", retMsg);
					return -303;
				}
				if(SQLCODE==1403)
				{
					strcpy(param_n,pOrderData->phone_no);
					strcpy(param_a,"111111");
				}
			}
			else
			{
				strcpy(param_n,pOrderData->phone_no);
			}
			Trim(param_n);
			Trim(param_a);
		}

	}
	/*add by shijing for �ֻ���Ϸ��ֵ�ɹ��·����� at 20120807 BEGIN*/
	if(strncmp(pOrderData->serv_code ,"28",2) ==0 && strcmp(vopr_code,"21")==0 )
	{
		EXEC SQL SELECT POINTS,POINTS
							into :param_F,:param_f
								FROM
									(SELECT * FROM dmgppoint
									 WHERE id_NO =:vid_no
									 and oper_type=:vserv_code
									 and oper_code=:vopr_code
									 and sp_code=:vspid
									 and sp_service=:vbizcode
									 order by op_time desc) where rownum <2;
		if(SQLCODE!=0)
		{
			sprintf(retMsg, "��ѯ��ֵ��Ϣ����[%d]!",  SQLCODE);
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	  		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-303", retMsg);
			return -303;
		}
		Trim(param_F);
		Trim(param_f);
		sprintf(param_F,"%.2fԪ�����",atof(param_F)/100);
	}
	/*add by shijing for �ֻ���Ϸ��ֵ�ɹ��·����� at 20120807 END*/

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- pOrderData->opr_code =[%s]  ",pOrderData->opr_code);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- pOrderData->serv_code=[%s]   ",pOrderData->serv_code);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- pOrderData->opr_source =[%s] ",pOrderData->opr_source);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- vbillingtype =[%s]  ",vbillingtype);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- vportalcode =[%s]  ",vportalcode);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- vRslt =[%s]  ",vRslt);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- param_f =[%s]  ",param_f);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", " -- param_F =[%s]  ",param_F);
#endif
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "yangzc test1111 vRslt: %s",vRslt);
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vDeDuctMode=%s,vDayConvert=%s,vSpType=%s",vDeDuctMode,vDayConvert,vSpType);

	/**2012.11.08 chendh shijing�µĶ���ģ�忼���Ƿ�����/������1-���У�0-����**/
  Trim(vDeDuctMode);
  Trim(vDayConvert);

  printf("\n---vDeDuctMode=[%s],vDayConvert=[%s]\n",vDeDuctMode,vDayConvert);
  if((strlen(vDeDuctMode)>0) || (strlen(vDayConvert)>0))
	{
		/*Sinitn(vSpType);*/
		Trim(vSpType);
		/*add by shijing at 20121113 begin*/
		Sinitn(param_b);
		Sinitn(param_g);
		/*add by shijing at 20121113 end*/
  	if(strcmp(vSpType,"1")==0)
  	{
  		Sinitn(param_c);/*add by shijing at 20121113*/
  		strcpy(param_c,"�й��ƶ�");

  	}else{
  		strcat(param_c,"��˾�ṩ��");
  		/*add by shijing at 20121113 begin*/
  		strcpy(param_b,"���й��ƶ����շ�,");
  		strcpy(param_g,"(���й��ƶ����շ�)");
  		/*add by shijing at 20121113 end*/
  	}
  	printf("\n---param_c=[%s],param_b=[%s],param_g=[%s]\n",param_c,param_b,param_g);
	}



/**---------- �滻���ɶ�����ʾ�� ---------- **/
    Trim(param_f);
    Trim(param_F);
    Sinitn(dynStmt);
	sprintf(dynStmt, "select PARAM_AB, BAK_FIELD,param_code from SDSMPSMSMSGPARAM where valid_flag = '1' "  );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);

  EXEC SQL PREPARE preStmt FROM :dynStmt;
	EXEC SQL DECLARE vCurCONT CURSOR for preStmt;
	EXEC SQL OPEN vCurCONT   ;
	EXEC SQL FETCH vCurCONT into :vparam_ab ,:vparam_value ,:vparam_code;

	if (SQLCODE == 1403)
	{
		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��SDSMPSMSMSGPARAMû�м�¼[%d]!", SQLCODE);
	  	EXEC SQL CLOSE vCurCONT;
	  	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-206", retMsg);
	  	return -206;
	}

    while(SQLCODE == 0)
    {
        Trim(vparam_ab);
        Trim(vparam_value);
        Trim(vparam_code);

        if (strcmp(vparam_value,":param_c") == 0) {  strcpy(vtemp_value, param_c );   }
        else if (strcmp(vparam_value,":param_s") == 0) {  strcpy(vtemp_value, param_s );   }
        else if (strcmp(vparam_value,":param_d") == 0) {  strcpy(vtemp_value, param_d );   }
        else if (strcmp(vparam_value,":param_m") == 0) {  strcpy(vtemp_value, param_m );   }
        else if (strcmp(vparam_value,":param_f") == 0) {  strcpy(vtemp_value, param_f );   }
        else if (strcmp(vparam_value,":param_F") == 0) {  strcpy(vtemp_value, param_F );   }
        else if (strcmp(vparam_value,":param_t") == 0) {  strcpy(vtemp_value, param_t );   }
        else if (strcmp(vparam_value,":param_k") == 0) {  strcpy(vtemp_value, param_k );   }
        else if (strcmp(vparam_value,":param_l") == 0) {  strcpy(vtemp_value, param_l );   }
        else if (strcmp(vparam_value,":param_r") == 0) {  strcpy(vtemp_value, param_r );   }
        else if (strcmp(vparam_value,":param_w") == 0) {  strcpy(vtemp_value, param_w );   }
        else if (strcmp(vparam_value,":param_n") == 0) {  strcpy(vtemp_value, param_n );   }
        else if (strcmp(vparam_value,":param_a") == 0) {  strcpy(vtemp_value, param_a );   }
        else if (strcmp(vparam_value,":param_v") == 0) {  strcpy(vtemp_value, param_v );   }
        else if (strcmp(vparam_value,":param_p") == 0) {  strcpy(vtemp_value, param_p );   }
        else if (strcmp(vparam_value,":param_q") == 0) {  strcpy(vtemp_value, param_q );   }
        else if (strcmp(vparam_value,":param_u") == 0) {  strcpy(vtemp_value, param_u );   }
        else if (strcmp(vparam_value,":param_e") == 0) {  strcpy(vtemp_value, param_e );   }
        else if (strcmp(vparam_value,":param_y") == 0) {  strcpy(vtemp_value, param_y );   }
        else if (strcmp(vparam_value,":param_x") == 0) {  strcpy(vtemp_value, param_x );   }
        else if (strcmp(vparam_value,":param_z") == 0) {  strcpy(vtemp_value, param_z );   }
/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
 				else if (strcmp(vparam_value,":param_H") == 0) {  strcpy(vtemp_value, param_H );   }
        else if (strcmp(vparam_value,":param_M") == 0) {  strcpy(vtemp_value, param_M );   }
        else if (strcmp(vparam_value,":param_S") == 0) {  strcpy(vtemp_value, param_S );   }
/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/
/*add by shijing at 20121113 begin*/
				else if (strcmp(vparam_value,":param_b") == 0) {  strcpy(vtemp_value, param_b );   }
				else if (strcmp(vparam_value,":param_g") == 0) {  strcpy(vtemp_value, param_g );   }
/*add by shijing at 20121113 end*/
        iCount = 0;
        Trim(vtemp_value);

        memset(dynStmt,0,sizeof(dynStmt));
        sprintf(dynStmt," SELECT replace(:v1,PARAM_AB,:v2)  "
                        " from SDSMPSMSMSGPARAM where param_code =:v3  "    );
        EXEC SQL EXECUTE
        BEGIN
                EXECUTE IMMEDIATE :dynStmt into  :vtemp_msg using :vsend_msg ,:vtemp_value,:vparam_code;
        END;
        END-EXEC;
	    if (SQLCODE == 1403)
	    {
		    sprintf(retMsg, "fDsmpGetShortMsg:����ַ�������ִ�д���[%d]!", iCount);
	    	EXEC SQL CLOSE vCurCONT;
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-230", retMsg);
	    	return -230;
	    }
        Trim(vtemp_msg);
	    	strcpy(vsend_msg,vtemp_msg );
        EXEC SQL FETCH vCurCONT into  :vparam_ab ,:vparam_value ,:vparam_code;
    }
    EXEC SQL CLOSE vCurCONT;

    strcpy(send_msg,vsend_msg );
    Trim(vmsg_oprcode);

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "----send_msg=[%s]   ",send_msg );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "----pOrderData->third_id_no=[%s]  ",pOrderData->third_id_no);
#endif
    if (atol(pOrderData->third_id_no) > 0 && strcmp(vmsg_oprcode,"RC")!=0)
    {

        memset(dynStmt,0,sizeof(dynStmt));
        memset(vsend_msg,0,sizeof(vsend_msg));
        memset(vmsgtype_id,0,sizeof(vmsgtype_id));
        memset(vtemp_msg,0,sizeof(vtemp_msg));

        sprintf(dynStmt," select  Trim(CONTENT) ,to_char(a.MSGTYPEID)  "
             " from  SDSMPSMSMSGTYPE  a, SDSMPSMSSENDCODE  b  "
             "  where a.MSGTYPEID =b.MSGTYPEID  AND b.OPR_CODE = decode(:v1,'89','07','99','07',:v2) AND b.SERV_CODE = :v3 "
             "  AND b.OPR_SOURCE =:v4 AND b.CHRG_TYPE =:v5 and b.RSLT_CODE =:v6 "
             "  and (b.serv_property =:v7 or b.serv_property ='Z') "
             "  and (b.first_order_flag =:v8 or b.first_order_flag ='Z')   "
             "  and (b.before_21_flag =:v9 or b.before_21_flag ='Z')   "
             "  AND  b.third_opr_flag = '1'  and rownum < 2 " );
	    pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);
        EXEC SQL EXECUTE
        BEGIN
                 EXECUTE IMMEDIATE :dynStmt into  :vsend_msg ,:vmsgtype_id  using :vmsg_oprcode ,:vmsg_oprcode,
                 :vserv_code,  :vopr_source,:vbillingtype,:vRslt ,:vserv_property , :vfirst_order_flag ,:vbefore_21_flag ;
        END;
        END-EXEC;
	    if (SQLCODE !=0)
	    {
			#ifdef _DEBUG_
		   		pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "SQLCODE=[%d]",SQLCODE );
				pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt=[%s]",dynStmt );
		    #endif
			PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-220", retMsg);
	    	return -220;
	    }
	    Trim(vsend_msg);

	#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vmsgtype_id=[%s]",vmsgtype_id );
    pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vsend_msg=[%s]",vsend_msg );
    #endif

        Sinitn(dynStmt);
    	sprintf(dynStmt, "select PARAM_AB, BAK_FIELD,param_code from SDSMPSMSMSGPARAM where valid_flag = '1'  "  );
    	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "dynStmt = [%s]", dynStmt);

        EXEC SQL PREPARE preStmt FROM :dynStmt;
    	EXEC SQL DECLARE vCurCONT2 CURSOR for preStmt;
    	EXEC SQL OPEN vCurCONT2   ;
    	EXEC SQL FETCH vCurCONT2 into :vparam_ab ,:vparam_value ,:vparam_code;

    	if (SQLCODE == 1403)
    	{
    		sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��SDSMPSMSMSGPARAMû�м�¼[%d]!", SQLCODE);
    	  	EXEC SQL CLOSE vCurCONT2;
    	  	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-206", retMsg);
    	  	return -206;
    	}

        while(SQLCODE == 0)
        {
            Trim(vparam_ab);
            Trim(vparam_value);
            Trim(vparam_code);

            if (strcmp(vparam_value,":param_c") == 0) {  strcpy(vtemp_value, param_c );   }
            else if (strcmp(vparam_value,":param_s") == 0) {  strcpy(vtemp_value, param_s );   }
            else if (strcmp(vparam_value,":param_d") == 0) {  strcpy(vtemp_value, param_d );   }
            else if (strcmp(vparam_value,":param_m") == 0) {  strcpy(vtemp_value, param_m );   }
            else if (strcmp(vparam_value,":param_f") == 0) {  strcpy(vtemp_value, param_f );   }
            else if (strcmp(vparam_value,":param_F") == 0) {  strcpy(vtemp_value, param_F );   }
            else if (strcmp(vparam_value,":param_t") == 0) {  strcpy(vtemp_value, param_t );   }
            else if (strcmp(vparam_value,":param_k") == 0) {  strcpy(vtemp_value, param_k );   }
            else if (strcmp(vparam_value,":param_l") == 0) {  strcpy(vtemp_value, param_l );   }
            else if (strcmp(vparam_value,":param_r") == 0) {  strcpy(vtemp_value, param_r );   }
            else if (strcmp(vparam_value,":param_w") == 0) {  strcpy(vtemp_value, param_w );   }
            else if (strcmp(vparam_value,":param_n") == 0) {  strcpy(vtemp_value, param_n );   }
            else if (strcmp(vparam_value,":param_a") == 0) {  strcpy(vtemp_value, param_a );   }
            else if (strcmp(vparam_value,":param_v") == 0) {  strcpy(vtemp_value, param_v );   }
            else if (strcmp(vparam_value,":param_p") == 0) {  strcpy(vtemp_value, param_p );   }
            else if (strcmp(vparam_value,":param_q") == 0) {  strcpy(vtemp_value, param_q );   }
            else if (strcmp(vparam_value,":param_u") == 0) {  strcpy(vtemp_value, param_u );   }
            else if (strcmp(vparam_value,":param_e") == 0) {  strcpy(vtemp_value, param_e );   }
            else if (strcmp(vparam_value,":param_y") == 0) {  strcpy(vtemp_value, param_y );   }
            else if (strcmp(vparam_value,":param_x") == 0) {  strcpy(vtemp_value, param_x );   }
            else if (strcmp(vparam_value,":param_z") == 0) {  strcpy(vtemp_value, param_z );   }
/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 begin*/
 					 	else if (strcmp(vparam_value,":param_H") == 0) {  strcpy(vtemp_value, param_H );   }
 						else if (strcmp(vparam_value,":param_M") == 0) {  strcpy(vtemp_value, param_M );   }
 						else if (strcmp(vparam_value,":param_S") == 0) {  strcpy(vtemp_value, param_S );   }
/*Modify for wlan ҵ��淶 v1.6.5 at 2012.01.06 end*/

            iCount = 0;
            Trim(vtemp_value);

            memset(dynStmt,0,sizeof(dynStmt));
            sprintf(dynStmt," SELECT replace(:v1,PARAM_AB,:v2)  "
                            " from SDSMPSMSMSGPARAM where param_code =:v3  "    );
            EXEC SQL EXECUTE
            BEGIN
                    EXECUTE IMMEDIATE :dynStmt into  :vtemp_msg using :vsend_msg ,:vtemp_value,:vparam_code;
            END;
            END-EXEC;
    	    if (SQLCODE == 1403)
    	    {
    		    sprintf(retMsg, "fDsmpGetShortMsg:����ַ�������ִ�д���[%d]!", iCount);
    	    	EXEC SQL CLOSE vCurCONT2;
    	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    		pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-230", retMsg);
    	    	return -230;
    	    }
            Trim(vtemp_msg);
    	    strcpy(vsend_msg,vtemp_msg );

            EXEC SQL FETCH vCurCONT2 into  :vparam_ab ,:vparam_value ,:vparam_code;
        }
        EXEC SQL CLOSE vCurCONT2;

         strcpy(send_msg2,vsend_msg );
    }

    /**chendh 20090831�����ֻ�����ƽ̨��ȡ**/
    Trim(send_msg);
    if(strlen(send_msg)==0 && strcmp(vserv_code,"23")==0)
	{
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid=[%s],vbizcode=[%s],vopr_source=[%s],vopr_code=[%s],vRslt=[%s]",vspid,vbizcode,vopr_source,vopr_code,vRslt);
    	EXEC SQL select content into :vsend_msg from SCBOSSSHORTMSGTX
    				where spid=:vspid and bizcode=:vbizcode and opr_source=:vopr_source
    					and opr_code=:vopr_code and rslt_code=:vRslt and op_code=:vop_code;
      if(SQLCODE!=0 && SQLCODE!=1403)
			{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ���Զ�����ʾ��ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	/**return -230; **/
			}
			strcpy(send_msg,vsend_msg);
    }
    else if(strlen(send_msg)==0 && strcmp(vserv_code,"25")==0)
    {
    	/**�������ֻ�Ա**/
    	memset(vMemLevel,0,sizeof(vMemLevel));
    	EXEC SQL select member_level into :vMemLevel
    		from dWMusicUser
    		where id_no=to_number(:vid_no);
    	if(SQLCODE!=0&&SQLCODE!=1403)
    	{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ��Ա����ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	strcpy(vMemLevel,"0");
    	}
    	if(SQLCODE==1403)
    	{
    		/**Ĭ�Ϸǻ�Ա**/
    		strcpy(vMemLevel,"0");
    	}

    	/**���ǻ�Ա,��ȡ���ֶ�������**/
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid=[%s],vbizcode=[%s],vopr_source=[%s],vopr_code=[%s],vRslt=[%s]",vspid,vbizcode,vopr_source,vopr_code,vRslt);
    	EXEC SQL select content into :vsend_msg from SCBOSSSHORTMSGTX
    				where spid=:vspid and bizcode=:vbizcode and opr_source=:vopr_source
    					and opr_code=:vopr_code and rslt_code=:vRslt and op_code=:vop_code
    				  and memlevel=:vMemLevel;
      if(SQLCODE!=0 && SQLCODE!=1403)
			{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ���Զ�����ʾ��ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	/**return -230; **/
			}
			strcpy(send_msg,vsend_msg);

    }
    else if(strlen(send_msg)==0 && strcmp(vserv_code,"53")==0 )
    {
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid=[%s],vbizcode=[%s],vopr_source=[%s],vopr_code=[%s],vRslt=[%s]",vspid,vbizcode,vopr_source,vopr_code,vRslt);
    	EXEC SQL select content into :vsend_msg from SCBOSSSHORTMSGTX
    				where spid=:vspid and bizcode=:vbizcode and opr_source=:vopr_source
    					and opr_code=:vopr_code and rslt_code=:vRslt
    					and op_code=:vop_code;
      if(SQLCODE!=0 && SQLCODE!=1403)
			{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ���Զ�����ʾ��ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	/**return -230; **/
			}
			strcpy(send_msg,vsend_msg);
    }
    else if(strlen(send_msg)==0 && strcmp(vserv_code,"31")==0 )
    {
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid=[%s],vbizcode=[%s],vopr_source=[%s],vopr_code=[%s],vRslt=[%s]",vspid,vbizcode,vopr_source,vopr_code,vRslt);
    	EXEC SQL select content into :vsend_msg from SCBOSSSHORTMSGTX
    				where spid=:vspid and bizcode=:vbizcode and opr_source=:vopr_source
    					and opr_code=:vopr_code and rslt_code=:vRslt
    					and op_code=:vop_code;
      if(SQLCODE!=0 && SQLCODE!=1403)
			{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ���Զ�����ʾ��ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	/**return -230; **/
			}
			strcpy(send_msg,vsend_msg);
    }
    else if(strlen(send_msg)==0)
    {
			pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vspid=[%s],vbizcode=[%s],vopr_source=[%s],vopr_code=[%s],vRslt=[%s]",vspid,vbizcode,vopr_source,vopr_code,vRslt);
    	EXEC SQL select content into :vsend_msg from SCBOSSSHORTMSGTX
    				where spid=:vspid and bizcode=:vbizcode and opr_source=:vopr_source
    					and opr_code=:vopr_code and rslt_code=:vRslt;
      if(SQLCODE!=0 && SQLCODE!=1403)
			{
		    sprintf(retMsg, "fDsmpGetShortMsg:��ȡ���Զ�����ʾ��ʧ��[%d]!", SQLCODE);
	    	PUBLOG_DBDEBUG("fDsmpGetShortMsg");
	    	pubLog_DBErr(_FFL_, "fDsmpGetShortMsg", "-231", retMsg);
	    	/**return -230; **/
			}
			strcpy(send_msg,vsend_msg);
    }

    /*modify by baidq
    memset(vRslt,0,sizeof(vRslt));
    sprintf(vRslt,"%s%s",vcfm_order_flag , vnocfm_order_flag);*/

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "-- end -- fDsmpGetShortMsg   ");
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "send_msg=[%s]",send_msg );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "send_msg2=[%s]",send_msg2 );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vRslt=[%s]",vRslt );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vcfm_order_flag=[%s]",vcfm_order_flag );
	pubLog_Debug(_FFL_, "fDsmpGetShortMsg", "", "vnocfm_order_flag=[%s]",vnocfm_order_flag );
#endif

	return 0 ;
}


/*add by shijing for R2098.cp(�ֻ�����)����ʧ�� ��20111024�汾�н������������� at 20120705 begin*/
int fspiBillModeGet(DSMPORDEROpr *pOrderData,char *ModeCode)
{
	 char vModeCode[8+1];
	 char vFuncCode[4+1];
	 char vEnd_mm[17+1];
	 char vEff_mm[17+1];
	 char dynStmt[1024+1];

	 char id_no[14+1];
	 char spid[20+1];
	 int Scount=0;
	 init(dynStmt);
	 init(vModeCode);
	 init(vFuncCode);
	 init(vEnd_mm);
	 init(vEff_mm);
	 init(id_no);
	 init(spid);
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "=============��ʼ===============");

	printf("pOrderData->spid %s \n",pOrderData->spid);
	printf("pOrderData->bizcode %s \n",pOrderData->bizcode);
	printf("pOrderData->opr_source %s \n",pOrderData->opr_source);
	 /* �״ζ�������ʱ�� */
	 EXEC SQL select mode_code, to_char(add_months(sysdate,opr_mm), 'yyyymmdd hh24:mi:ss'), to_char(ADD_MONTHS(sysdate,1),'YYYYMM')||'01' || to_char(sysdate ,' hh24:mi:ss')
	          into :vModeCode,:vEnd_mm,:vEff_mm
	          from SSPidBillMOde
	          where spid=:pOrderData->spid
	          and  bizcode=:pOrderData->bizcode;
	if ( SQLCODE != 0 )
	{
		pubLog_Debug(_FFL_, "fspiBillModeGet", "", "��ȡ�ײ� spid = %s \nbizcode = %s\nModeCode =%s  \n pOrderData->opr_source =%s SQLCODE [%d]\n",pOrderData->spid,pOrderData->bizcode,vModeCode,pOrderData->opr_source,SQLCODE);
		return -1;
	}

        strcpy(pOrderData->end_time,vEnd_mm); /* ����ʱ�� */
	strcpy(ModeCode,vModeCode);
        printf("vEnd_mm %s \n",vEnd_mm);
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "spid = %s \nbizcode = %s\nModeCode =%s \n end_time = %s\n",pOrderData->spid,pOrderData->bizcode,ModeCode,pOrderData->end_time);

	/** chendh 2009.02.03�޸��״ζ���ʱ����ȡ����,��opr_time����Ϊmax(first_order_date) **/
  sprintf(dynStmt, "select to_number(count(*)) "
   				" from ddsmpordermsg%ld  "
   				" where id_no = to_number(:v1) and spid = :v2  "
   				" and opr_code='06' and valid_flag ='1'",atol(pOrderData->id_no )%10L);

  strcpy(id_no,pOrderData->id_no);
  strcpy(spid,pOrderData->spid);
  Trim(id_no);
  Trim(spid);
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "��ȡ�״ζ���ʱ������:");
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "dynStmt=[%s]",dynStmt);
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "vid_no: [%s]",id_no);
	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "vspid : [%s]",spid );


      EXEC SQL EXECUTE
      BEGIN
			EXECUTE IMMEDIATE :dynStmt INTO :Scount  using :id_no ,:spid ;
      END;
      END-EXEC;
      if ((SQLCODE != 0) &&(SQLCODE != 1403 ))
      	{
      		pubLog_Debug(_FFL_, "fspiBillModeGet", "", "��ȡ�û��״ζ���ʱ��ʧ�� [%d]\n",SQLCODE);
        	return -45 ;
        }
        printf("\n Scount %d  \n",Scount);
        /* ��ʼʱ�� */
       if (Scount==0)
      	{  /** ȱʡֵ  �״ζ���  ������Ч**/
      		 strcpy(pOrderData->eff_time ,pOrderData->opr_time);
      	}
      else
      	{ /*   ������������Ʒ����һ����Ч */
          strcpy(pOrderData->eff_time ,vEff_mm );
      	}
  	pubLog_Debug(_FFL_, "fspiBillModeGet", "", "pOrderData->eff_time [%s] \n  [%d]\n",pOrderData->eff_time,SQLCODE);
        pubLog_Debug(_FFL_, "fspiBillModeGet", "", "================����==================");
	return 0;

}


int  fDsmpProdOrderModeCodeCheck(char * Id_No ,char * spid ,char *bizcode,char *retMsg)
{

   char vModeCode[8+1];
	 char vFuncCode[4+1];
	 char sql_str[1000+1];
	 char MM[4+1];
	 char service_name[30+1];

	 char timelitmmit_flag[2+1];
	 int vnCount=0;

	 init(service_name);
	 init(MM);
	 init(vFuncCode);
	 init(sql_str);
	 init(vModeCode);
	 init(timelitmmit_flag);

	Sinitn(sql_str);
	sprintf(sql_str, "select distinct  ceil(months_between(end_time,begin_time) ),b.timelimmit_flag \
										from dbillcustdetail%c a , SSPidBillMOde b          \
										where a.id_no = :sIDNo         \
										and a.mode_code = b.mode_code  \
										and a.op_code= b.op_code       \
										and mode_status = 'Y'          \
										and mode_time = 'Y'", Id_No[strlen(Id_No)-1]);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "sql_str = [%s]  Id_No =[%s]", sql_str,Id_No);
#endif
 	EXEC SQL prepare GetModeMM     from   :sql_str;
	EXEC SQL declare GetModeMM_Cur cursor for GetModeMM;
	EXEC SQL open    GetModeMM_Cur using  :Id_No;
	while(1)
	{
		EXEC SQL fetch GetModeMM_Cur into :MM,:timelitmmit_flag;
		if ( (SQLCODE != 0) && (SQLCODE != 1403) )
		{
			sprintf(retMsg,"��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����֤! SQLCODE = [%d]",Id_No, vModeCode, SQLCODE);
			pubLog_Debug(_FFL_, service_name, "", "[%s]",retMsg);
			EXEC SQL close GetModeMM_Cur;
			break;
		}
		if ( SQLCODE == 1403 )
		{
			break;
		}
		Trim(MM);
#ifdef _DEBUG_
		   pubLog_Debug(_FFL_, service_name, "", "MM = [%s]", MM);
#endif
		vnCount++;
		if ( vnCount > 1 )
		{
			sprintf(retMsg,"��ȡ�û�[%s]��ԭ�ײ�[%s]��֤ʧ��! SQLCODE = [%d]",Id_No, bizcode, SQLCODE);
			pubLog_Debug(_FFL_, service_name, "", "[%s]",retMsg);
			EXEC SQL close GetModeMM_Cur;
			break;
		}
	}
	EXEC SQL close   GetModeMM_Cur;
	Trim(timelitmmit_flag);
        printf("vnCount =[%d]  MM =[%s] timelitmmit_flag=[%s] \n",vnCount,MM,timelitmmit_flag);
		/*if(vnCount==1)*/ /* �Զ��� һ�����ܽ�������ͬ���ײͶ��� */
		{

		  if((atoi(MM) >1) && (strcmp(timelitmmit_flag,"Y")==0))
		 	{
		 		sprintf(retMsg,"ҵ���ܽ��а���[%s]��ʼ����ʱ�䲻�ܴ���1��[%s]��֤ʧ��! SQLCODE = [%d]", bizcode, MM,SQLCODE);
		 		pubLog_Debug(_FFL_, service_name, "", "[%s]\n", retMsg);
		                return 1111;
		 	}
	        }

		return 0;
}


int fspiBillMode(DSMPORDEROpr *pOrderData,char *iMode_Code,char *iFunCode,char *retMsg)
{
	/* �����м���� */
	DSMPORDEROpr tBizData;

	int  RetCode=0;
	int  vnCount=0;
	char tempbuf[1000];
	char iEffectTime[17+1];
	char iOpCode[6+1];
	char iLoginNo[8+1];
	char sql_str[1024+1];
	char Phone_No[15+1];
	char Mode_Code[8+1];
	char sModeAccept[14+1];
	char Id_No[14+1];
	char service_name[20+1];

	char opr_code[2+1];
	char opType[1+1];/*"1" ԤԼ 0 ���� */

	char sModeCode[8+1];
	char retCode[6+1];
	/* ��ʼ���м���� */
	Sinitn(opr_code);
	Sinitn(service_name);
	Sinitn(Phone_No);
	Sinitn(Mode_Code);
	Sinitn(sql_str);
	Sinitn(iLoginNo);
	Sinitn(iOpCode);
	Sinitn(tempbuf);
	Sinitn(iEffectTime);
	Sinitn(sModeAccept);
	Sinitn(opType);
	Sinitn(retCode);
	Sinitn(sModeCode);
	/* ������ֵ */
	strcpy(opType,"1");/* Ĭ��ԤԼ*/
  strcpy(iLoginNo,pOrderData->login_no);
  strcpy(iOpCode,iFunCode);
  strcpy(Id_No, pOrderData->id_no);
  strcpy(Phone_No,pOrderData->phone_no );
	strcpy(Mode_Code,iMode_Code);
	strcpy(opr_code,pOrderData->opr_code);
	strcpy(service_name,"fPubDsmpOrderMsg");

	memset(&tBizData,0,sizeof(DSMPORDEROpr));

    printf( "---��ʼִ�����ɶ�����ϵ����[fPubDsmpOrderMsg]--------------\n");

    printf( "pOrderData->order_id       =[%ld]\n", pOrderData->order_id       );
    printf( "pOrderData->id_no           =[%s]\n", pOrderData->id_no           );
    printf( "pOrderData->phone_no        =[%s]\n", pOrderData->phone_no        );
    printf( "pOrderData->serv_code       =[%s]\n", pOrderData->serv_code       );
    printf( "pOrderData->opr_code        =[%s]\n", pOrderData->opr_code        );
    printf( "pOrderData->third_id_no,    =[%s]\n", pOrderData->third_id_no     );
    printf( "pOrderData->third_phoneno   =[%s]\n", pOrderData->third_phoneno   );
    printf( "pOrderData->spid            =[%s]\n", pOrderData->spid            );
    printf( "pOrderData->bizcode         =[%s]\n", pOrderData->bizcode         );
    printf( "pOrderData->prod_code       =[%s]\n", pOrderData->prod_code       );
    printf( "pOrderData->comb_flag       =[%s]\n", pOrderData->comb_flag       );
    printf( "pOrderData->mode_code       =[%s]\n", pOrderData->mode_code       );
    printf( "pOrderData->chg_flag        =[%s]\n", pOrderData->chg_flag        );
    printf( "pOrderData->chrg_type       =[%s]\n", pOrderData->chrg_type       );
    printf( "pOrderData->channel_id      =[%s]\n", pOrderData->channel_id      );
    printf( "pOrderData->opr_source      =[%s]\n", pOrderData->opr_source      );
    printf( "pOrderData->first_order_date=[%s]\n", pOrderData->first_order_date);
    printf( "pOrderData->first_login_no  =[%s]\n", pOrderData->first_login_no  );
    printf( "pOrderData->last_order_date =[%s]\n", pOrderData->last_order_date );
    printf( "pOrderData->opr_time        =[%s]\n", pOrderData->opr_time        );
    printf( "pOrderData->eff_time        =[%s]\n", pOrderData->eff_time        );
    printf( "pOrderData->end_time        =[%s]\n", pOrderData->end_time        );
    printf( "pOrderData->local_code      =[%s]\n", pOrderData->local_code      );
    printf( "pOrderData->valid_flag      =[%s]\n", pOrderData->valid_flag      );
    printf( "pOrderData->belong_code     =[%s]\n", pOrderData->belong_code     );
    printf( "pOrderData->total_date      =[%s]\n", pOrderData->total_date      );
    printf( "pOrderData->login_accept    =[%s]\n", pOrderData->login_accept    );
    printf( "pOrderData->reserved        =[%s]\n", pOrderData->reserved        );
    printf( "pOrderData->sm_code         =[%s]\n",  pOrderData->sm_code        );
    printf( "pOrderData->password        =[%s]\n", pOrderData->password        );


    /*  ���� */
 if(strcmp(opr_code,"06")==0)
 	{
printf("\n ԤԼ��ͨ�ֻ������ײͿ�ʼ=================================\n");

	/* 3.6. ԤԼ���뱾ʡ���ײ� pubGetEffectTime() pubBillBeginMode() */
	RetCode = pubGetEffectTime(pOrderData->opr_time , "1",Id_No , iEffectTime);
	if ( RetCode != 0 )
	{
		pubLog_Debug(_FFL_, service_name, "", "��ȡ�ײ�[%s]ȡ��ʱ��ʧ��!", iEffectTime);
		return -1;
	}

	if(strcmp(pOrderData->eff_time,iEffectTime)!=0)
	{
		strcpy(iEffectTime,pOrderData->eff_time);
	}
	pubLog_Debug(_FFL_, service_name, "", "pOrderData->eff_time [%s] iEffectTime [%s] opType [%s]!", pOrderData->eff_time,iEffectTime ,opType);
	init(tempbuf);
	RetCode = pubBillBeginMode(Phone_No,    \
														 Mode_Code,   \
														 iEffectTime, \
													 	 iOpCode,     \
													   iLoginNo,    \
													   pOrderData->login_accept, \
														 pOrderData->opr_time, "0","", "", tempbuf);
	if (RetCode != 0)
	{
	   pubLog_Debug(_FFL_, service_name, "", "��ͨ�ײ�[%s]ʧ��! RetCode = [%d]", Mode_Code, RetCode);
     return -2;
	}
printf("\n ԤԼ��ͨ�ֻ������ײͽ���=================================\n");
 	}
  else if((strcmp(opr_code,"07")==0)|| (strcmp(opr_code,"99")==0) || (strcmp(opr_code,"89")==0))/*�˶�*/
  {
printf("\n �����˶��ֻ������ײͿ�ʼ=================================\n");
  init(Mode_Code);
	/* 2.2. ��ȡȡ����ʡԭ�ײ�ҵ����ˮ */
	vnCount = 0;
	Sinitn(sql_str);
	sprintf(sql_str, "select to_char(a.login_accept),a.Mode_code     \
										from dbillcustdetail%c a, SSPidBillMOde b    \
										where a.id_no = :sIDNo           \
										and a.mode_code = b.mode_code    \
										and a.op_code= b.op_code         \
										and a.mode_status = 'Y'            \
										and a.mode_time = 'Y'", Id_No[strlen(Id_No)-1]);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "sql_str = [%s]", sql_str);
#endif
	EXEC SQL prepare GetModeAccept     from   :sql_str;
	EXEC SQL declare GetModeAccept_Cur cursor for GetModeAccept;
	EXEC SQL open GetModeAccept_Cur    using  :Id_No;
	while(1)
	{
		EXEC SQL fetch GetModeAccept_Cur into :sModeAccept,:Mode_Code;
		if ( (SQLCODE != 0) && (SQLCODE != 1403) )
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! SQLCODE = [%d]",Phone_No, Mode_Code, SQLCODE);
			EXEC SQL close GetModeAccept_Cur;
			break;
		}
		if ( SQLCODE == 1403 )
		{
			break;
		}
		Trim(sModeAccept);
#ifdef _DEBUG_
		   pubLog_Debug(_FFL_, service_name, "", "sModeAccept = [%s]", sModeAccept);
#endif
		vnCount++;
		if ( vnCount > 1 )
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! l_nCount = [%d]",Phone_No, Mode_Code, vnCount);
			sprintf(retMsg, "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! l_nCount = [%d]",Phone_No, Mode_Code, vnCount);
			EXEC SQL close GetModeAccept_Cur;
			break;
		}
	}
	EXEC SQL close GetModeAccept_Cur;

  if(vnCount==0)
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡԭ�ײ�[%s]��ˮʧ��!", Mode_Code);
      return 1001;
		}
  		/* 2.3. ԤԼȡ����ʡԭ�ײ� pubGetEffectTime() pubBillEndMode() */
	init(iEffectTime);
	RetCode = pubGetEffectTime(pOrderData->opr_time ,\
	                           "1",    \
	                           Id_No , \
	                           iEffectTime);
	if ( RetCode != 0 )
	{
		pubLog_Debug(_FFL_, service_name, "", "��ȡ�ײ�[%s]ȡ��ʱ��ʧ��!", Mode_Code);
    return 1000;
	}

	init(tempbuf);
	RetCode = pubBillEndMode(Phone_No,                \
													 Mode_Code,               \
													 sModeAccept,             \
													 iEffectTime,             \
													 iOpCode,                 \
													 iLoginNo,                \
													 pOrderData->login_accept,\
													 pOrderData->opr_time,    \
													 tempbuf);
	if( RetCode != 0 )
	{
		pubLog_Debug(_FFL_, service_name, "", "ȡ���ײ�[%s]ʧ��! RetCode = [%d]", Mode_Code, RetCode);
	  return 222;
	}
printf("\n �����˶��ֻ������ײͽ���=================================\n");

  }
  else if(strcmp(opr_code,"13")==0)/*���*/
  {

printf("\n ����˶��ֻ������ײͿ�ʼ=================================\n");

	/* 2.2. ��ȡȡ����ʡԭ�ײ�ҵ����ˮ */
	vnCount = 0;
	Sinitn(sql_str);
	sprintf(sql_str, "select to_char(a.login_accept),a.Mode_code     \
										from dbillcustdetail%c a, SSPidBillMOde b    \
										where a.id_no = :sIDNo           \
										and a.mode_code = b.mode_code    \
										and a.op_code= b.op_code         \
										and a.mode_status = 'Y'          \
										and a.mode_time = 'Y'", Id_No[strlen(Id_No)-1]);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, service_name, "", "sql_str = [%s]", sql_str);
#endif
	EXEC SQL prepare GetModeAcceptChange     from   :sql_str;
	EXEC SQL declare GetModeAcceptChange_Cur cursor for GetModeAcceptChange;
	EXEC SQL open    GetModeAcceptChange_Cur using  :Id_No;
	while(1)
	{
		EXEC SQL fetch GetModeAcceptChange_Cur into :sModeAccept,:sModeCode;
		if ( (SQLCODE != 0) && (SQLCODE != 1403) )
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! SQLCODE = [%d]",Phone_No, sModeCode, SQLCODE);
			EXEC SQL close GetModeAcceptChange_Cur;
			break;
		}
		if ( SQLCODE == 1403 )
		{
			break;
		}
		Trim(sModeAccept);
#ifdef _DEBUG_
		   pubLog_Debug(_FFL_, service_name, "", "sModeAccept = [%s]", sModeAccept);
#endif
		vnCount++;
		if ( vnCount > 1 )
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! l_nCount = [%d]",Phone_No, Mode_Code, vnCount);
			sprintf(retMsg, "��ȡ�û�[%s]��ԭ�ײ�[%s]ҵ����ˮʧ��! l_nCount = [%d]",Phone_No, Mode_Code, vnCount);
			EXEC SQL close GetModeAcceptChange_Cur;
			break;
		}
	}
	EXEC SQL close GetModeAcceptChange_Cur;

  if(vnCount==0)
		{
			pubLog_Debug(_FFL_, service_name, "", "��ȡԭ�ײ�[%s]��ˮʧ��!", Mode_Code);
      return 1001;
		}
  /* 2.3. ԤԼȡ����ʡԭ�ײ� pubGetEffectTime() pubBillEndMode() */
	init(iEffectTime);
	RetCode = pubGetEffectTime(pOrderData->opr_time ,\
	                           "1",   \
	                           Id_No, \
	                           iEffectTime);
	if ( RetCode != 0 )
	{
		pubLog_Debug(_FFL_, service_name, "", "��ȡ�ײ�[%s]ȡ��ʱ��ʧ��!", Mode_Code);
    return 1000;
	}

	init(tempbuf);
	RetCode = pubBillEndMode(Phone_No,                \
													 sModeCode,               \
													 sModeAccept,             \
													 iEffectTime,             \
													 iOpCode,                 \
													 iLoginNo,                \
													 pOrderData->login_accept,\
													 pOrderData->opr_time,    \
													 tempbuf);
	if( RetCode != 0 )
	{
		pubLog_Debug(_FFL_, service_name, "", "ȡ���ײ�[%s]ʧ��! RetCode = [%d]", Mode_Code, RetCode);
	  return 222;
	}
printf("\n �����˶��ֻ������ײͽ���=================================\n");

printf("\n ԤԼ��ͨ�ֻ������ײͿ�ʼ=================================\n");

	/* 3.6. ԤԼ���뱾ʡ���ײ� pubGetEffectTime() pubBillBeginMode() */
	RetCode = pubGetEffectTime(pOrderData->opr_time , "1", Id_No, iEffectTime);
	if ( RetCode != 0 )
	{
		printf("��ȡ�ײ�[%s]��Чʱ��ʧ��!", iEffectTime);
		return -1;
	}
	init(tempbuf);
	RetCode = pubBillBeginMode(Phone_No,                 \
														 Mode_Code,                \
														 iEffectTime, iOpCode,     \
														 iLoginNo,                 \
														 pOrderData->login_accept, \
														 pOrderData->opr_time,     \
														 "2","", "", tempbuf);
	if (RetCode != 0)
	{
	   printf( "��ͨ�ײ�[%s]ʧ��!  tempbuf = [%s] ",Mode_Code,tempbuf);
     return -2;
	}
printf("\n ԤԼ��ͨ�ֻ������ײͽ���=================================\n");
  }
		return 0;

}
/*add by shijing for R2098.cp(�ֻ�����)����ʧ�� ��20111024�汾�н������������� at 20120705 end*/
/**********************************************
 @chendh FUNC : fDsmpOprLimitCtl  ����ҵ�����Ȩ�޿���--MIFI��������
 @input  params: phoneNO -> �������
 @input  params: idNo -> �û�ID
 @input  params: oprCode -> ��������
 @input  params: return_mess -> ������Ϣ
 @output params:
 *********************************************/
int fDsmpOprLimitCtl(char *phoneNo, char *idNo,char *oprCode,char *return_mess)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char	vIdNo[14+1];
	char	vPhoneNo[15+1];
	char	vOprCode[2+1];
	int		icount = 0,vCount=0;
	char	vSrvNo[4+1];
	char	vSrvName[128+1];
	char	vRetCode[6+1];
	char	vRetMsg[512+1];
	char	vDealSql[1024+1];
	char	vParamType[1+1];

	EXEC SQL END DECLARE SECTION;

	Sinitn(vIdNo);
	Sinitn(vPhoneNo);
	Sinitn(vOprCode );
	Sinitn(vSrvNo );
	Sinitn(vSrvName );
	Sinitn(vRetCode );
	Sinitn(vRetMsg );
	Sinitn(vDealSql );
	Sinitn(vParamType );

	strcpy(vPhoneNo, 	phoneNo);
	strcpy(vIdNo, 		idNo);
	strcpy(vOprCode, 	oprCode);

	Trim(vPhoneNo);
	Trim(vIdNo);
	Trim(vOprCode);

	pubLog_Debug(_FFL_, "fDsmpOprLimitCtl", "", "=========��ʼ����fDsmpOprLimitCtl������=========");



	EXEC SQL select srvno,srv_name,ret_code,ret_msg,param_type,deal_sql
			into :vSrvNo,:vSrvName,:vRetCode,:vRetMsg,:vParamType,:vDealSql
			from spubLogincheck
			where srvno ='0001';
	if(SQLCODE!=0)
	{
			pubLog_Debug(_FFL_, "fDsmpOprLimitCtl", "", "MIFI�û����ư���ȫ��ҵ��!" );
			sprintf(return_mess, "MIFI�û����ư���ȫ��ҵ��!"  );
			pubLog_DBErr(_FFL_, "fDsmpOprLimitCtl", "1310", return_mess);
			return 1310;
	}
	Trim(vDealSql);
	pubLog_Debug(_FFL_, "fDsmpOprLimitCtl", "", "vDealSql=[%s]!" ,vDealSql);
	vCount=0;
	EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :vDealSql
			INTO :vCount
			USING :vIdNo;
		END;
	END-EXEC;
	if(vCount>0)
	{
			pubLog_Debug(_FFL_, "fDsmpOprLimitCtl", "", "MIFI�û����ư���ȫ��ҵ��!" );
			sprintf(return_mess, "MIFI�û����ư���ȫ��ҵ��!"  );
			pubLog_DBErr(_FFL_, "fDsmpOprLimitCtl", "1310", return_mess);
			return 1310;
	}

	return 0 ;
}

/*add by shijing*/
int fVGgopSync(DSMPORDEROpr *pOrderData,char *opr_flag,char *retMsg)
{
	  EXEC SQL BEGIN DECLARE SECTION;
      char	vfeertype[2+1];
      char	oprNumb[32+1];
      char  servtype[6+1];
      char 	insertsql[1024];
      char		subServType[20+1];
      char	vflag[1+1];
      char	thirdNo[11+1];
      char	voprcode[2+1];
      long		oprseq=0;
    EXEC SQL END DECLARE SECTION;
    
    Sinitn(vfeertype);
    Sinitn(oprNumb);
    Sinitn(insertsql);
    Sinitn(servtype);
    Sinitn(vflag);
    Sinitn(subServType);
    Sinitn(thirdNo);
    Sinitn(voprcode);
    
    strcpy(vflag,opr_flag);
    Trim(vflag);
    
   printf("pOrderData->serv_code=%s,vflag=%s",pOrderData->serv_code,vflag);
   if(strcmp(pOrderData->serv_code,"16")==0)
   {
   		strcpy(servtype,"210006");
   }
   else if(strcmp(pOrderData->serv_code,"23")==0)
   {
   		strcpy(servtype,"210001");
   }
   else if(strcmp(pOrderData->serv_code,"60")==0)
   {
   		strcpy(servtype,"210009");
   }
   
   Trim(servtype);
   
   if(strcmp(pOrderData->opr_code,"99")==0)
   {
   		strcpy(voprcode,"89");
   		printf("\n----1voprcode=%s\n",voprcode);
   }
   else
   {
   		strcpy(voprcode,pOrderData->opr_code);
   		printf("\n----2voprcode=%s\n",voprcode);
   }
   Trim(voprcode);
   
   EXEC SQL SELECT billingtype into :vfeertype
   				FROM ddsmpspbizinfo WHERE spid=:pOrderData->spid and bizcode=:pOrderData->bizcode;
   if(SQLCODE!=0)
   {
   	printf("��ѯddsmpspbizinfoʧ��[%d]!\n",SQLCODE);
     sprintf(retMsg, "��ѯddsmpspbizinfoʧ��[%d]!" ,SQLCODE);
     return -36;
   	
   }
   Trim(vfeertype);
   printf("\n----servtype=%s\n",servtype);
    /*������ˮ��*/
   oprseq=0;
   oprseq=fGetVgopSeq();
   memset(oprNumb,0,sizeof(oprNumb));	
   EXEC SQL select :servtype||'BIP2B515'||to_char(sysdate,'yymmddhh24miss')||lpad(:oprseq,6,0)  into :oprNumb from dual;
   if(SQLCODE!=0)
   {
   		printf("��ѯ��ˮʧ��[%d]!\n",SQLCODE);
     sprintf(retMsg, "��ѯ��ˮʧ��[%d]!" ,SQLCODE);
     return -42;
   }    	
   Trim(oprNumb);
   
    /*printf("\n---OPRNUMB=%s OPRCODE=%s FEEUSER_ID=%s,ACTIONREASONID=%s,SERVTYPE=%s,SUBSERVTYPE=%s\n",
      oprNumb,pOrderData->opr_code,pOrderData->phone_no,vflag,servtype,pOrderData->bizcode);
      
    printf("\n---SPID=%s,BIZCODE=%s,OPRSOURCE=%s,PARAMNUM=%s,PARAMNAME=%s\n",
    pOrderData->spid,pOrderData->bizcode,pOrderData->opr_source,pOrderData->para_num,pOrderData->para_name);
    
    printf("\n---PARAMVALUE=%s,vfeertype=%s,BIZTYPE=%s\n",
    pOrderData->para_value,vfeertype,pOrderData->serv_code);*/
   if(strcmp(pOrderData->serv_code,"16")==0)/*�ֻ�����*/
   {
   		printf("\n@@@@@@@�ֻ�����\n");
   			sprintf(insertsql,"INSERT INTO OBOSS.TBOSSVGOPBIZPORTALMSG "
   							" (MAXACCEPT,OPRNUMB,OPTIME,OPRCODE,EFFTT,EXPIRETIME,FEEUSER_ID,DESTUSER_ID,ACTIONREASONID, "
   							" SERVTYPE,SUBSERVTYPE,SPID,BIZCODE,OPRSOURCE,PARAMNUM,PARAMNAME,PARAMVALUE,FEETYPE,BIZTYPE)"
   							" VALUES(oboss.seq_resendorder.NEXTVAL,:v1,to_char(sysdate,'yyyymmddhh24miss'),:v2,to_char(sysdate,'yyyymmddhh24miss'),'30000101000000',:v3, "
   					    " :v4,:v5,:v6,:v7,:v8,:v9,:v10,'1','Pack',:v11,:v12,:v13)");
   			EXEC SQL PREPARE prepare4 From :insertsql;
   			EXEC SQL EXECUTE prepare4 USING   :oprNumb,:voprcode,:pOrderData->phone_no,
   																			 :pOrderData->phone_no,:vflag,:servtype,:pOrderData->bizcode,:pOrderData->spid,:pOrderData->bizcode,:pOrderData->opr_source,:pOrderData->bizcode,:vfeertype,:pOrderData->serv_code;
   			if(SQLCODE!=0)
   			{
   				printf("insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!\n",SQLCODE);
   		 	 	sprintf(retMsg, "insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!" ,SQLCODE);
   		 	 	return -50;
   			
   			}
   }
   else if(strcmp(pOrderData->serv_code,"60")==0)/*�Ķ�*/
   {
   	printf("\n@@@@@@@�Ķ�\n");
   	if(strcmp(pOrderData->phone_no,pOrderData->third_phoneno)==0)
		{
			strcpy(thirdNo,pOrderData->phone_no);
			strcpy(subServType,pOrderData->bizcode);
		}
		else
		{	
			if(strlen(pOrderData->third_phoneno)!=0)
			{
				strcpy(thirdNo,pOrderData->third_phoneno);
				strcpy(subServType,"CMRBinding");
			}
			else
			{
				strcpy(thirdNo,pOrderData->phone_no);
				strcpy(subServType,pOrderData->bizcode);
			}
		}
   		
   		sprintf(insertsql,"INSERT INTO OBOSS.TBOSSVGOPBIZPORTALMSG "
   						" (MAXACCEPT,OPRNUMB,OPTIME,OPRCODE,EFFTT,EXPIRETIME,FEEUSER_ID,DESTUSER_ID,ACTIONREASONID, "
   						" SERVTYPE,SUBSERVTYPE,SPID,BIZCODE,OPRSOURCE,PARAMNUM,PARAMNAME,PARAMVALUE,FEETYPE,BIZTYPE)"
   						" VALUES(oboss.seq_resendorder.NEXTVAL,:v1,to_char(sysdate,'yyyymmddhh24miss'),:v2,to_char(sysdate,'yyyymmddhh24miss'),'30000101000000',:v3, "
   				    " :v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,:v15)");
   		EXEC SQL PREPARE prepare4 From :insertsql;
   		EXEC SQL EXECUTE prepare4 USING   :oprNumb,:voprcode,:pOrderData->phone_no,
   																			 :thirdNo,:vflag,:servtype,:subServType,:pOrderData->spid,:pOrderData->bizcode,:pOrderData->opr_source,:pOrderData->para_num,:pOrderData->para_name,:pOrderData->para_value,:vfeertype,:pOrderData->serv_code;
   		
   		if(SQLCODE!=0)
   		{
   			printf("insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!\n",SQLCODE);
   		  sprintf(retMsg, "insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!" ,SQLCODE);
   		  return -37;
   			
   		}
 		}
 		else if(strcmp(pOrderData->serv_code,"23")==0)/*����*/
   {
   	printf("\n@@@@@@@����\n");
   		sprintf(insertsql,"INSERT INTO OBOSS.TBOSSVGOPBIZPORTALMSG "
   						" (MAXACCEPT,OPRNUMB,OPTIME,OPRCODE,EFFTT,EXPIRETIME,FEEUSER_ID,DESTUSER_ID,ACTIONREASONID, "
   						" SERVTYPE,SUBSERVTYPE,SPID,BIZCODE,OPRSOURCE,PARAMNUM,PARAMNAME,PARAMVALUE,FEETYPE,BIZTYPE)"
   						" VALUES(oboss.seq_resendorder.NEXTVAL,:v1,to_char(sysdate,'yyyymmddhh24miss'),:v2,to_char(sysdate,'yyyymmddhh24miss'),'30000101000000',:v3, "
   				    " :v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,:v15)");
   		EXEC SQL PREPARE prepare4 From :insertsql;
   		EXEC SQL EXECUTE prepare4 USING   :oprNumb,:voprcode,:pOrderData->phone_no,
   																			 :pOrderData->phone_no,:vflag,:servtype,:pOrderData->bizcode,:pOrderData->spid,:pOrderData->bizcode,:pOrderData->opr_source,:pOrderData->para_num,:pOrderData->para_name,:pOrderData->para_value,:vfeertype,:pOrderData->serv_code;
   		
   		if(SQLCODE!=0)
   		{
   			printf("insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!\n",SQLCODE);
   		  sprintf(retMsg, "insert TBOSSVGOPBIZPORTALMSGʧ��[%d]!" ,SQLCODE);
   		  return -38;
   			
   		}
 		}
   return 0;
          
}
