
#include "pubLog.h"
#include "boss_srv.h"
#include "publicsrv.h"
#include "publicEmer.h"

/** ��ʹ�ó����ӷ�ʽ�������ݿ⣬��ſ� DB_LONG_CONN �� **/
/**
#define DB_LONG_CONN
**/

/** ��ʹ���������ݱ�ǩ�����岢�ſ� DB_LABEL �� **/
/**
#define DB_LABEL XXXXX
**/

#include "tux_server.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

TUX_SERVER_BEGIN();

int TpSvInit(int argc, char* argv[])
{
	printf("�Զ����ʼ��\n");
	
	return 0;
}

void TpSvDone(void)
{
	printf("�Զ�������\n");
	
	return;
}




/*@service information
 *@name 					sWebDelivery
 *@description				��¼��վ����ԤԼ���š��ն˵�������Ϣ,��bossǰ̨չʾ
 *@author					shijing
 *@created					20120910
 *@input parameter information
 *@inparam					iDeliveryType		[��������]--0Ϊ���Ϳ���1Ϊ�����ն�
 *@inparam					iOrderTime			[ԤԼʱ��]--��ʽΪyyyyMMddHHmiss
 *@inparam					iOrderChannel		[ԤԼ;��]--��վ:www000
 *@inparam					iCustName				[�û�����]
 *@inparam					iIdentityCard		[���֤��]
 *@inparam					iAdress					[���͵�ַ]
 *@inparam					iDeliveryTime		[����ʱ��]
 *@inparam					iInvoice				[�Ƿ���Ҫ��Ʊ]--0Ϊ��1Ϊ��
 *@inparam					iOrderPhoneNo		[ԤԼ����]--�����������=0ʱΪԤԼ����ĺ��룬��������=1ʱΪ�ն˰󶨵ĺ���
 *@inparam					iModeCode				[ѡ���ײ�]
 *@inparam					iReserve				[Ԥ�滰��]
 *@inparam					iBuyPhone				[�Ƿ����ֻ�]--0Ϊ��1Ϊ��
 *@inparam					iModel					[�ֻ��ͺ�]
 *@inparam					iPrice					[Ӫ�����۸�]
 *@inparam					iBindCycle			[������]
 *@inparam					iLowestConsume	[�������]
 *@inparam					iOrderId				[ԤԼid]--��ʽ:yyyyMMddHHmiss+3λ�����
 *@inparam					iPhoneNo				[�û��ֻ�����]--��������ʱ����ϵ��ʽ
 *@inparam					iChooseModeCode [��ѡ�ʷ�]
 *@inparam					iGrantAccept 		[Ӫ������ˮ]
 *@inparam					iContactOrgId		[Ӫҵ������]
 *@inparam					iPhoneType			[��������] 0 ��ͨ���� 1 ��������(������ͨ����,188����)
 *@inparam					iOrderStatus 		[����״̬] 010 δ���� 011 ����ȡ�� 012 ��ʱ 020�Ѹ���  030 �ɹ�
 *@inparam					vChnFlag				[�ն����۷�ʽ] 0:��ȡ 1:���� (�����������=0ʱ ����2,=1ʱ����0:��ȡ 1:����)
 
 *@output parameter information     
 *@outparam					oRetCode		[���ش���]
 *@outparam					oRetMsg			[������Ϣ]
 
 cl32 sWebDelivery 24 2 0 20130122145201 www000 ���޼� 22010419820506061A ���� "2013012201|2013012220" 0 13944007555 hn0Dm005 100 0 null null 0 0 20130122093012126 13894896992 "gn220003|" 0 "355" 0 2 010
 cl32 sWebDelivery 24 2 1 20130122145201 www000 ���� 22010419820506061C ��ü "2013012201|2013012220" 0 13844800675 hn0Dm005 100 1 "TCL-U699" 1300 0 0 20130122093012127 13894896992 "gn220003|" 0 "355" 2 1 020
 cl32 sWebDelivery 24 2 1 20130122145201 www000 ���� 22010419820506061C ��ü "2013012201|2013012220" 0 13844800675 null 100 1 "TCL-U699" 1300 0 "0" 20130122093012131 13894896992 null 11062157711 "355" 2 1 020
 */


_TUX_SVC_ sWebDelivery(TPSVCINFO *transb)
{	
	EXEC SQL BEGIN DECLARE SECTION;
		char	service_name[20 + 1];
		char	iDeliveryType[1+1];
		char	iOrderTime[15 + 1];
		char	iOrderChannel[6 + 1];
		char	iCustName[40 + 1];
		char	iIdentityCard[18 + 1];
		
		char	iAdress[100 + 1];
		char	iDeliveryTime[21 + 1];
		char	iInvoice[1 + 1];
		char	iOrderPhoneNo[11 + 1];
		
		char	iModeCode[8 + 1];
		char	iReserve[14 + 1];
		char	iBuyPhone[1 + 1];
		char	iModel[20 + 1];
		
		char	iPrice[14 + 1];
		char	iBindCycle[4 + 1];
		char	iLowestConsume[14 + 1];
		char	iOrderId[20 + 1];
		char	iOrgId[10+1];
		char	iOpCode[4 + 1];
		char	iPhoneNo[11 + 1];
		
		
		long	lLoginAccept = 0;
		
		int		iCount=0;
		char	vPhoneNo[15+1];
		char	iChooseModeCode[256+1];
		char	iGrantAccept[14+1];
		char	iContactOrgId[10+1];
		char	iPhoneType[1+1];
		char	iOrderStatus[3+1];
		int		vCount=0;
		char	vTotalDate[8+1];
		char	vLoginAccept[14+1];
		char	vRegion_code[2+1];
		char	vIdNo[14+1];
		char	vChnFlag[1+1];
		char	Region_Code[2+1];
		char	District_Code[2+1];
		char	vGroupId[10+1];
		char	vDisGroupId[10+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(service_name);
	Sinitn(iOrderTime);
	Sinitn(iOrderChannel);
	Sinitn(iCustName);
	Sinitn(iIdentityCard);
	Sinitn(iAdress);
	Sinitn(iDeliveryTime);
	Sinitn(iInvoice);
	Sinitn(iOrderPhoneNo);
	Sinitn(iModeCode);
	Sinitn(iReserve);
	Sinitn(iBuyPhone);
	Sinitn(iModel);
	Sinitn(iPrice);
	Sinitn(iBindCycle);
	Sinitn(iLowestConsume);
	Sinitn(iOrderId);
	Sinitn(iOpCode);
	Sinitn(iOrgId);
	Sinitn(iDeliveryType);
	Sinitn(iPhoneNo);
	
	Sinitn(vPhoneNo);
	Sinitn(iChooseModeCode);
	Sinitn(iGrantAccept);
	Sinitn(iContactOrgId);
	Sinitn(iPhoneType);
	Sinitn(iOrderStatus);
	Sinitn(vTotalDate);
	Sinitn(vLoginAccept);
	Sinitn(vRegion_code);
	Sinitn(vIdNo);
	Sinitn(vChnFlag);
	Sinitn(Region_Code);
	Sinitn(District_Code);
	Sinitn(vGroupId);
	Sinitn(vDisGroupId);
	
	strcpy(service_name, "sWebDelivery");
	PUBLOG_APPRUN(service_name, "", "����%s��ʼִ��", service_name);
	
	TUX_SERVICE_BEGIN(lLoginAccept, iOpCode, iOrderChannel, "ԤԼ������Ϣ��¼","0", iPhoneNo, NULL, NULL);

	strcpy(iDeliveryType,	 input_parms[0]);
	strcpy(iOrderTime,		 input_parms[1]);
	strcpy(iOrderChannel,	 input_parms[2]);
	strcpy(iCustName,			 input_parms[3]);
	strcpy(iIdentityCard,	 input_parms[4]);
	strcpy(iAdress, 			 input_parms[5]);
	strcpy(iDeliveryTime,	 input_parms[6]);
	strcpy(iInvoice,			 input_parms[7]);
	strcpy(iOrderPhoneNo,	 input_parms[8]);
	strcpy(iModeCode,			 input_parms[9]);
	strcpy(iReserve,			 input_parms[10]);
	strcpy(iBuyPhone,			 input_parms[11]);
	strcpy(iModel,				 input_parms[12]);
	strcpy(iPrice,				 input_parms[13]);
	strcpy(iBindCycle,		 input_parms[14]);
	strcpy(iLowestConsume, input_parms[15]);
	strcpy(iOrderId,			 input_parms[16]);
	strcpy(iPhoneNo,			 input_parms[17]);
	strcpy(iChooseModeCode,input_parms[18]);
	strcpy(iGrantAccept,	 input_parms[19]);
	strcpy(iContactOrgId,	 input_parms[20]);
	strcpy(iPhoneType,		 input_parms[21]);
	strcpy(iOrderStatus,	 input_parms[22]);
	strcpy(vChnFlag,			 input_parms[23]);
	
	Trim(iOrderTime);
	Trim(iOrderChannel);
	Trim(iCustName);
	Trim(iIdentityCard);
	Trim(iAdress);
	Trim(iDeliveryTime);
	Trim(iInvoice);
	Trim(iOrderPhoneNo);
	Trim(iModeCode);
	Trim(iReserve);
	Trim(iBuyPhone);
	Trim(iModel);
	Trim(iPrice);
	Trim(iBindCycle);
	Trim(iLowestConsume);
	Trim(iOrderId);
	Trim(iDeliveryType);
	Trim(iPhoneNo);
	Trim(iChooseModeCode);
	Trim(iGrantAccept);
	Trim(iContactOrgId);
	Trim(iPhoneType);
	Trim(iOrderStatus);
	Trim(vChnFlag);
	
	if((strcmp(iDeliveryType,"0")!=0)&&(strcmp(iDeliveryType,"1")!=0))
	{
		sprintf(oRetCode, "000001");
		sprintf(oRetMsg, "�������ͱ�������0��1[%s]",iDeliveryType);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	if(strlen(iOrderTime)!= 14)
	{
		sprintf(oRetCode, "000002");
		sprintf(oRetMsg, "ԤԼʱ���ʽ����[%s]", iOrderTime);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	EXEC SQL select ORG_ID into :iOrgId
				from dloginmsg 
				where login_no = :iOrderChannel;
	if((SQLCODE !=0)&&(SQLCODE !=1403))
	{
		sprintf(oRetCode, "000003");
		sprintf(oRetMsg, "��ѯԤԼ;��ʧ��[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	if(1403 == SQLCODE)
	{
		sprintf(oRetCode, "000004");
		sprintf(oRetMsg, "ԤԼ;������[%s]",iOrderChannel);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	PUBLOG_DEBUG(service_name, "", "iOrgId[%s]", iOrgId);
	
	if((strlen(iIdentityCard)!=15)&&(strlen(iIdentityCard)!=18))
	{
		sprintf(oRetCode, "000005");
		sprintf(oRetMsg, "���֤��λ������[%s]",iIdentityCard);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	if(strlen(iDeliveryTime)!=21)
	{
		sprintf(oRetCode, "000006");
		sprintf(oRetMsg, "����ʱ�θ�ʽ����[%s]",iDeliveryTime);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}

	if((strcmp(iInvoice,"0")!=0)&&(strcmp(iInvoice,"1")!=0))
	{
		sprintf(oRetCode, "000007");
		sprintf(oRetMsg, "�Ƿ���Ҫ��Ʊ��ʶ��������0��1[%s]",iInvoice);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}

	if(strlen(iOrderPhoneNo)!=11)
	{
		sprintf(oRetCode, "000008");
		sprintf(oRetMsg, "�û�ԤԼ����λ������[%s]",iOrderPhoneNo);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}

/*	if(strlen(iOrderId)!=17)
	{
		sprintf(oRetCode, "000009");
		sprintf(oRetMsg, "ԤԼID����[%s]",iOrderId);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}*/
	if(strlen(iCustName)==0||strcmp(iCustName,"null")==0)
	{
		sprintf(oRetCode, "000035");
		sprintf(oRetMsg, "�ͻ�����������Ϊ��[%s]",iCustName);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	/*����Ӫҵ��*/
	if( strlen(iContactOrgId) == 0 )
	{
		sprintf(oRetCode, "000010");
		sprintf(oRetMsg, "���������Ӫҵ������[%s]",iContactOrgId);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}	
	
	/*У�鹺���͹��ն�����*/
	if( strcmp(iDeliveryType,"0") ==0 )
	{
		
		/*���򿨵�ʱ��һ�������Թ����ն�*/
		if( strcmp(iBuyPhone,"0") !=0 )
		{
			sprintf(oRetCode, "000011");
			sprintf(oRetMsg, "�����ն˲�����ͬʱ����[%s]",iBuyPhone);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*��������*/
		if((strcmp(iPhoneType,"0")!=0)&&(strcmp(iPhoneType,"1")!=0))
		{
			sprintf(oRetCode, "000012");
			sprintf(oRetMsg, "�������ֱ�������0��1[%s]",iPhoneType);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*�ն����۷�ʽ*/
		if( (strcmp(vChnFlag,"2")!=0) )
		{
			sprintf(oRetCode, "000013");
			sprintf(oRetMsg, "�ն����۷�ʽ��������2[%s]",vChnFlag);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		if(strcmp(iModel,"null")==0) /*�ֻ��ͺ�*/
		{
			Sinitn(iModel);
		}
		if(strcmp(iPrice,"null")==0) /*Ӫ�����۸�*/
		{
			Sinitn(iPrice);
		}
		init(vChnFlag);
	}else if( strcmp(iDeliveryType,"1") == 0  )/*�ն�����У��*/
	{
		/*�����ն˵�ʱ�����Ϊ1*/
		if( strcmp(iBuyPhone,"1") !=0 )
		{
			sprintf(oRetCode, "000014");
			sprintf(oRetMsg, "�����ն�ʱ,iBuyPhone[%s]�������,ֵ����Ϊ1",iBuyPhone);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*Ӫ������ˮ*/
		if( strlen(iGrantAccept) == 0 )
		{
			sprintf(oRetCode, "000015");
			sprintf(oRetMsg, "Ӫ������ˮ��������[%s]",iGrantAccept);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*��������*/
		if( (strcmp(iPhoneType,"2")!=0) )
		{
			sprintf(oRetCode, "000016");
			sprintf(oRetMsg, "����������ִ���[%s],ֵ����Ϊ2",iPhoneType);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		init(iPhoneType);
		
		/*�ն����۷�ʽ*/
		if( (strcmp(vChnFlag,"1")!=0) && (strcmp(vChnFlag,"0")!=0) )
		{
			sprintf(oRetCode, "000017");
			sprintf(oRetMsg, "�����ն����۷�ʽ����[%s]",vChnFlag);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*�����ն˵�ʱ�� ����ɷѳɹ��Ķ���*/
		if( strcmp(iOrderStatus,"020") !=0 )
		{
			sprintf(oRetCode, "000018");
			sprintf(oRetMsg, "�ն˹��򣬱����Ƚɷ�[%s]",iOrderStatus);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}
	/*���ʷ�*/
	if( strcmp(iModeCode,"null") == 0 )
	{
		Sinitn(iModeCode);
	}
	
	/*��ѡ�ʷ�*/
	if( strcmp(iChooseModeCode,"null") == 0 )
	{
		Sinitn(iChooseModeCode);
	}
	
	vCount=0;
	EXEC SQL SELECT count(*)
				into :vCount
				from DWEBDELIVERY
				where order_id=rpad(:iOrderId,18);
	if(SQLCODE !=0)
	{
		sprintf(oRetCode, "000019");
		sprintf(oRetMsg, "��ѯ��DBCUSTADM.DWEBDELIVERY����[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	if(vCount>0)
	{
		sprintf(oRetCode, "000020");
		sprintf(oRetMsg, "�����Ѵ���[%d]",vCount);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	/*ȡ��ˮ*/
	EXEC SQL SELECT to_char(sMaxSysAccept.nextVal), to_char(sysdate,'yyyymmdd')
			INTO :vLoginAccept,:vTotalDate
			FROM dual;
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"000021");
		sprintf(oRetMsg,"ȡϵͳʱ��ʧ��!,sqlcode[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vLoginAccept);
	Trim(vTotalDate);
	
	/*���ɶ���*/
	EXEC SQL insert into DWEBDELIVERY
			(delivery_type, order_time, order_channel, cust_name, 
			identity_card,adress, delivery_time, invoice, 
			order_phoneno, mode_code, reserve, buy_phone, model, 
			price, bind_cycle, lowest_consume, order_id, org_id, 
			total_date, op_time, phoneno, choosemodecode, 
			grant_accept, order_status,contact_orgid,phone_type,chn_flag)
		values
		(:iDeliveryType,to_date(:iOrderTime,'yyyymmdd hh24:mi:ss'),:iOrderChannel,:iCustName,
		:iIdentityCard,:iAdress,:iDeliveryTime,:iInvoice,
		:iOrderPhoneNo,:iModeCode,to_number(:iReserve),:iBuyPhone,:iModel,
		 to_number(:iPrice),:iBindCycle,to_number(:iLowestConsume),:iOrderId,:iOrgId,
		 to_number(:vTotalDate),sysdate,:iPhoneNo,:iChooseModeCode,
		 :iGrantAccept,:iOrderStatus,:iContactOrgId,:iPhoneType,:vChnFlag);
	if(SQLCODE !=0)
	{
		sprintf(oRetCode, "000022");
		sprintf(oRetMsg, "�����DBCUSTADM.DWEBDELIVERY����[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	/*��¼��ʷ����*/
	EXEC SQL insert into DWEBDELIVERYHIS
		(delivery_type, order_time, order_channel, cust_name, 
		identity_card,adress, delivery_time, invoice, 
		order_phoneno, mode_code, reserve, buy_phone, model, 
		price, bind_cycle, lowest_consume, order_id, org_id, 
		total_date, op_time, phoneno, choosemodecode, 
		grant_accept, order_status,contact_orgid,phone_type,chn_flag,
		Update_Accept,  Update_Time,Update_Login,Update_Type,
		Update_Code,Update_Date,update_note)
	select delivery_type, order_time, order_channel, cust_name, 
		identity_card,adress, delivery_time, invoice, 
		order_phoneno, mode_code, reserve, buy_phone, model, 
		price, bind_cycle, lowest_consume, order_id, org_id, 
		total_date, op_time, phoneno, choosemodecode, 
		grant_accept, order_status,contact_orgid,phone_type,chn_flag,
		to_number(:vLoginAccept),sysdate,'www000','U',
		'g097',to_number(:vTotalDate),'���ɶ�����Ϣ'
	from DWEBDELIVERY
	where order_id=rpad(:iOrderId,18);
	if(SQLCODE !=0)
	{
		sprintf(oRetCode, "000023");
		sprintf(oRetMsg, "�����DBCUSTADM.DWEBDELIVERYHIS����[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}		

	/*0Ϊ���Ϳ� ������Դ*/
	if( strcmp(iDeliveryType,"0") == 0 )
	{
		EXEC SQL select region_code,district_code,group_id
			into :Region_Code,:District_Code,:vGroupId
			from dcustres
			where phone_no=:iOrderPhoneNo;
		if(SQLCODE!=0)
		{
			sprintf(oRetCode, "000032");
			sprintf(oRetMsg, "��ѯdcustres����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		Trim(vGroupId);
		EXEC SQL select a.group_id 
				into :vDisGroupId
				from dchngroupmsg a
				where substr(boss_org_code,1,4)=:Region_Code||:District_Code
				and a.class_code='12';
		if(SQLCODE!=0)
		{
			sprintf(oRetCode, "000033");
			sprintf(oRetMsg, "��ѯgroup_id����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		Trim(vDisGroupId);
		if(strcmp(vGroupId,vDisGroupId)!=0)
		{
			sprintf(oRetCode, "000034");
			sprintf(oRetMsg, "�˺���ǵ��м��ڵ㣬������ԤԼ[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*��¼His*/
		EXEC SQL INSERT INTO dCustResHis
			(phone_no, no_type, password, other_attr, resource_code, choice_fee, region_code, 
			district_code, town_code, begin_time, op_time, login_no, org_code, login_accept, 
			total_date, assign_accept, limit_prepay, update_accept, update_time, update_login, 
			update_type, update_code, update_date,group_id,org_id)
		SELECT phone_no, no_type, password, other_attr, resource_code, choice_fee,region_code, 
			district_code, town_code, begin_time, op_time, login_no, org_code, login_accept, 
			total_date, assign_accept, limit_prepay,sMaxSysAccept.NEXTVAL,SYSDATE,:iOrderChannel,
			'u','g097',to_number(to_char(SYSDATE,'yyyymmdd')),group_id,org_id
		FROM dCustRes
		WHERE Phone_No=:iOrderPhoneNo
		and resource_code in('0','s');
		if( SQLCODE != 0 )
		{
			sprintf(oRetCode, "000024");
			sprintf(oRetMsg, "��¼��Դ��ʷ�����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*������Դ�� dCustRes������ͨ�������������*/
		EXEC SQL UPDATE	dcustres
				set resource_code='p' /*��������Ԥռ*/
				where phone_no=:iOrderPhoneNo
				and resource_code in('0','s');
		if(SQLCODE!=0)
		{
			sprintf(oRetCode, "000025");
			sprintf(oRetMsg, "����Դ�ѱ�Ԥռ[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		if( strcmp(iPhoneType,"1") == 0 )
		{	
			/*��¼��������His*/
			EXEC SQL INSERT INTO dgoodphonereshis
				(phone_no, goodsm_mode, bill_code, good_type, region_code, pre_do_flag, begin_time, 
				end_time, bak_field, update_login, update_code, update_date, update_time, update_type, 
				update_accept, district_code, resource_code,group_id)
			SELECT phone_no, goodsm_mode, bill_code, good_type, region_code,pre_do_flag, begin_time,
				end_time,'','system','g097',to_number(to_char(sysdate,'yyyymmdd')),sysdate,'u',
				smaxsysaccept.nextval,district_code, resource_code,''
			FROM dgoodphoneres
			WHERE Phone_No=:iOrderPhoneNo
			and	resource_code in('0','s');
			if( SQLCODE != 0 )
			{
				sprintf(oRetCode, "000026");
				sprintf(oRetMsg, "��¼����������Դ��ʷ�����[%d]",SQLCODE);
				PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}			
			
			/*��������������Դ*/
			EXEC SQL UPDATE	dGoodPhoneRes
					set	resource_code='p'
					where	phone_no=:iOrderPhoneNo
					and	resource_code in('0','s');
			if( SQLCODE != 0 )
			{
				sprintf(oRetCode, "000027");
				sprintf(oRetMsg, "������������Դ�ѱ�Ԥռ[%d]",SQLCODE);
				PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}
		}	
	}
	/*�ն����� �ͱ�dtermsellbookmsg*/
	if( strcmp(iDeliveryType,"1") == 0 )
	{
		/*�û�������Ϣ*/
		EXEC SQL select to_char(id_no),substr(belong_code,1,2) into :vIdNo,:vRegion_code
							 from	dCustMsg
							where	phone_no=rpad(:iOrderPhoneNo,15);
		if( SQLCODE != 0 && SQLCODE != 1403 )
		{
			sprintf(oRetCode, "000028");
			sprintf(oRetMsg, "��ѯ������Ϣ����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}								
		Trim(vRegion_code);
		Trim(vIdNo);
		
		iCount=0;
		EXEC SQL select count(*) into :iCount from stermsellcfg
							where login_accept=to_number(:iGrantAccept)
								and use_flag='y'
								and region_code=:vRegion_code;
		if( SQLCODE != 0 )
		{
			sprintf(oRetCode, "000029");
			sprintf(oRetMsg, "��ѯӪ��������[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		if( iCount <= 0 )
		{
			sprintf(oRetCode, "000030");
			sprintf(oRetMsg, "����[%s]��Ӫ������ˮ[%s]����",vRegion_code,iGrantAccept);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
				
		EXEC SQL INSERT INTO dtermsellbookmsg
				(phone_no, id_no, mach_code, product_fee, busin_prepay, custplan_fee, custplan_months, 
				fav_money, plan_fee, plan_months, grant_accept, imeibind_months, repeatlimit_months, login_accept, 
				sell_flag, op_time, login_no, sell_time, sell_login, chn_flag)
			SELECT :iOrderPhoneNo,:vIdNo,mach_code,product_fee,prepay_fee,custplan_fee,custplan_months,
			fav_money, plan_fee, plan_months,:iGrantAccept, imeibind_months, repeat_limit, login_accept,
			'N',SYSDATE,'www000','','',:vChnFlag
			FROM stermsellcfg
			where login_accept=to_number(:iGrantAccept)
			and use_flag='y'
			and region_code=:vRegion_code;
		if( SQLCODE != 0 )
		{
			sprintf(oRetCode, "000031");
			sprintf(oRetMsg, "�����ն���վԤԼ��Ϣʧ��[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}

SERVICE_FINALLY_DECLARE();

	if(strcmp(oRetCode, "000000") == 0)
	{
		EXEC SQL COMMIT;
		PUBLOG_APPRUN(service_name, "", "����%sִ�гɹ�", service_name);
	}
	else
	{
		EXEC SQL ROLLBACK;
		PUBLOG_APPRUN(service_name, "", "����%sִ��ʧ��", service_name);
	}
	
	transOUT = Add_Ret_Value(GPARM32_0, oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1, oRetMsg);
	
	PUBLOG_APPRUN(service_name, "", "����%sִ�н���", service_name);
	TUX_SERVICE_END();
}

/*@service information
 *@name 						sWebDelivQry
 *@description			���Ͷ�����ѯ
 *@author						miaoyl
 *@created					20130118
 *@input parameter information
 *@inparam					iLoginNo				[��������]
 *@inparam					vOrderID				[����ID]
 *@inparam					vOprType 				[�ֻ�����]
 *@inparam					vIpAddr					[֤������] 
 
 *@output parameter information     
 *@outparam					oRetCode			 [���ش���]
 *@outparam					oRetMsg				 [������Ϣ]
 *@outparam					vdelivery_type [��������    ] 0Ϊ���Ϳ���1Ϊ�����ն�
 *@outparam					vorder_time    [ԤԼʱ��    ]
 *@outparam					vorder_channel [ԤԼ;��    ]
 *@outparam					vcust_name     [�û�����    ]
 *@outparam					videntity_card [���֤����  ]
 *@outparam					vadress        [���͵�ַ    ]
 *@outparam					vdelivery_time [����ʱ��    ] yyyyMMddHH|yyyyMMddHH
 *@outparam					vinvoice       [�Ƿ���Ҫ��Ʊ] 0Ϊ��1Ϊ��
 *@outparam					vorder_phoneno [�û�ԤԼ����] �����������=0ʱΪԤԼ����ĺ��룬��������=1ʱΪ�ն˰󶨵ĺ���
 *@outparam					vmode_code     [���ʷ�      ]
 *@outparam					vreserve       [Ԥ�滰��    ]
 *@outparam					vbuy_phone     [�Ƿ����ֻ�] 0Ϊ��1Ϊ��
 *@outparam					vmodel         [�ֻ��ͺ�    ]
 *@outparam					vprice         [Ӫ�����۸�  ]
 *@outparam					vbind_cycle    [��������    ]
 *@outparam					vlowest_consume[�������    ]
 *@outparam					vorder_id      [ԤԼID      ]	yyyyMMddHHmiss+3λ����� �������
 *@outparam					op_time        [��������ʱ��]
 *@outparam					vphoneno       [�û��ֻ�����]
 *@outparam					vchoosemodecode[��ѡ�ʷ�    ] ��ѡ�ʷѸ�ʽ XXXXXXXX| XXXXXXXX|
 *@outparam					vgrant_accept  [Ӫ������ˮ  ]
 *@outparam					vcontact_orgid [Ӫҵ������  ]
 *@outparam					vphone_type  	 [��������    ]
 *@outparam					vorder_status  [����״̬    ] 010 δ���� 011 ����ȡ�� 012 ʧЧ 020�Ѹ���  030 �ɹ�
 *@outparam					vchnflag  		 [�ն����۷�ʽ] 0:��ȡ 1:����
 *@outparam
 @test case
	cl32 sWebDelivQry 4 27 www000 20120912133012001 13503321234 220100380421124123
	cl32 sWebDelivQry 4 27 www000 20130130105200003 null null
	cl32 sWebDelivQry 4 27 www000 null 13844800675 null
	cl32 sWebDelivQry 4 27 www000 null null 220104198203171319
	cl32 sWebDelivQry 4 28 null 20130424110850044723 null null
*/

_TUX_SVC_ sWebDelivQry(TPSVCINFO *transb)
{
	char	service_name[20+1];
	int		i=0,j=0;
	int		iFlag=0;
	EXEC SQL BEGIN DECLARE SECTION;
			long	lLoginAccept = 0;
			char	iOpCode[4 + 1];
			char	iPhoneNo[11 + 1];
			char	iLoginNo[6+1];
			char	vCardID[18+1];
			char	vOrderID[20+1];
			
			char	vdelivery_type[1+1];
			char	vdelivery_name[10+1];
			char	vorder_time[19+1];
			char	vorder_channel[20+1];
			char	vcust_name[40+1];
			char	videntity_card[18+1];
			char	vadress[100+1];
			char	vdelivery_time[21+1];
			char	vinvoice[2+1];
			char	vorder_phoneno[15+1];
			char	vmode_code[8+1];
			long	vreserve=0.0;
			char	vbuy_phone[2+1];
			char	vmodel[20+1];
			long	vprice=0.0;
			char	vbind_cycle[4+1]; 
			long	vlowest_consume=0.0;
			char	vorder_id[18+1];
			char	vorg_id[10+1];
			char	vtotal_date[8+1];
			char	vop_time[19+1];
			char	vphoneno[15+1];
			char	vchoosemodecode[256+1];
			char	vchoosemodename[30+1];
			char	vCModeName[1024];
			char	v_mode_str[8+1];
			char	vgrant_accept[14+1];
			char	vgroup_name[100+1];
			char	vphone_type[8+1];
			char	vstatus_name[10+1];
			char	vchnflag[4+1];
			char	vStrTmp[1204+1];
			char	vModeName[30+1];
			char	vRegionCode[2+1];
			char	vEmsNo[25+1];
	EXEC SQL END DECLARE SECTION;
	
	char vStrBuff[20+1];
	
	/*input_params*/
	init(iOpCode);
	init(iPhoneNo);
	init(iLoginNo);
	init(vCardID);
	init(vOrderID);
	/*��ѯ���*/	
	init(vdelivery_type);
	init(vdelivery_name);
	init(vorder_time);
	init(vorder_channel);
	init(vcust_name);
	init(videntity_card);
	init(vadress);
	init(vdelivery_time);
	init(vinvoice);
	init(vorder_phoneno);
	init(vmode_code);
	init(vbuy_phone);
	init(vmodel);
	init(vbind_cycle);
	init(vorder_id);
	init(vorg_id);
	init(vtotal_date);
	init(vop_time);
	init(vphoneno);
	init(vchoosemodecode);
	init(vchoosemodename);
	init(vCModeName);
	init(v_mode_str);
	init(vgrant_accept);
	init(vgroup_name);
	init(vphone_type);
	init(vstatus_name);
	init(vchnflag);
	init(vStrTmp);
	init(vModeName);
	init(vRegionCode);
	init(vEmsNo);
	
	init(vStrBuff);
	
	init(service_name);
	strcpy(service_name, "sWebDelivQry");
	PUBLOG_APPRUN(service_name, "", "����%s��ʼִ��", service_name);
	
	TUX_SERVICE_BEGIN(lLoginAccept, iOpCode, iLoginNo, "������Ϣ��ѯ","0", iPhoneNo, NULL, NULL);

	strcpy(iLoginNo,		input_parms[0]);
	strcpy(vOrderID,		input_parms[1]); /*����ID*/
	strcpy(iPhoneNo,		input_parms[2]); /*�ֻ�����*/
	strcpy(vCardID, 		input_parms[3]); /*֤������*/
	
	Trim(iLoginNo);
	Trim(vOrderID);
	Trim(iPhoneNo);
	Trim(vCardID);
	
	i=0;
	i=sizeof("��");
	printf("test =%d\n",i);
	
	/*��������һ����ѯ����*/
	if( strcmp(vOrderID,"null") == 0 && strcmp(iPhoneNo,"null") == 0 && strcmp(vCardID,"null") == 0 )
	{
		sprintf(oRetCode, "g09700");
		sprintf(oRetMsg, "��������һ����ѯ����");
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	/*У����ζ������*/
	if( strlen(vOrderID) != 0 && strcmp(vOrderID,"null") != 0  )
	{	
		/*if( strlen(vOrderID) !=17 )
		{
			sprintf(oRetCode, "g09705");
			sprintf(oRetMsg, "����Ķ�����ų�������[%s]",vOrderID);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}*/
	}	
	
	/*У�����ԤԼ�ֻ�����*/
	if( strlen(iPhoneNo) !=0 && strcmp(iPhoneNo,"null") != 0 )
	{	
		if( strlen(iPhoneNo) != 11 )
		{
			sprintf(oRetCode, "g09706");
			sprintf(oRetMsg, "ԤԼ���볤������[%s]",iPhoneNo);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}	
	/*У�����֤������*/
	if( strlen(vCardID) != 0 && strcmp(vCardID,"null") != 0 )
	{	
		if( strlen(vCardID) < 15 )
		{
			sprintf(oRetCode, "g09707");
			sprintf(oRetMsg, "�����֤�����볤������[%s]",vCardID);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}
	
	init(vStrTmp);
	sprintf(vStrTmp,"select delivery_type,decode(delivery_type,0,'����','1','���ն�',delivery_type),"
			"order_time,nvl(decode(trim(order_channel),'1000','�����̳�','www000','�����ƶ���վ',order_channel),' '),"
			"nvl(cust_name,' '),nvl(identity_card,' '),adress,decode(trim(delivery_time),'1','���ϰ�ʱ���ͻ�','2','��ĩ�ͻ�','3','��һ�����ն������ͻ�',' '), "
			"decode(invoice,'0','��','1','��'),order_phoneno,nvl(mode_code,' '),nvl(reserve,0),decode(buy_phone,'0','��','1','��',buy_phone),nvl(model,' '),nvl(price,0),nvl(bind_cycle,' '),nvl(lowest_consume,0), "
			"order_id,op_time,phoneno,nvl(choosemodecode,' '),nvl(grant_accept,' '),nvl(b.status_name,' '),nvl(c.group_name,' '), "
			"decode(phone_type,'0','��ͨ����','1','��������',nvl(phone_type,' ')),decode(chn_flag,'0','��ȡ','1','����',nvl(chn_flag,' ')),nvl(ems_no,' ') "
			"from dWebDelivery a, sDeliveryCfg b,dchngroupmsg c "
			"where a.order_status=b.order_status and b.valid_flag='1' "
			"and a.contact_orgid=c.group_id " );
	/*��ѯ*/
	if( strcmp(vOrderID,"null") != 0 )
	{
		iFlag |= 1;
		strcat(vStrTmp,"and order_id=:vOrderID ");
	}
	if( strcmp(iPhoneNo,"null") != 0 )
	{
		iFlag |= 2;
		strcat(vStrTmp,"and order_phoneno=:iPhoneNo ");
	}
	if( strcmp(vCardID,"null") != 0  )
	{
		iFlag |= 4;
		strcat(vStrTmp,"and identity_card=:vCardID ");
	}
	
	printf("iPhoneNo=%s\n",iPhoneNo);
	printf("vCardID=%s\n",vCardID);
	printf("vOrderID=%s\n",vOrderID);
	printf("vStrTmp=[%s]\n",vStrTmp);
	printf("iFlag=[%d]\n",iFlag);
	i=0;
	EXEC SQL PREPARE vSql_Prepare from :vStrTmp;
	EXEC SQL DECLARE CurSor02 CURSOR for vSql_Prepare;
	if( iFlag == 1 )
	{
		EXEC SQL OPEN CurSor02 USING :vOrderID;
	}
	else if( iFlag == 2 )
	{
		EXEC SQL OPEN CurSor02 USING :iPhoneNo;
	}
	else if( iFlag == 4 )
	{
		EXEC SQL OPEN CurSor02 USING :vCardID;
	}
	else if( iFlag == 3 )
	{
		EXEC SQL OPEN CurSor02 USING :vOrderID,:iPhoneNo;
	}
	else if( iFlag == 5 )
	{
		EXEC SQL OPEN CurSor02 USING :vOrderID,:vCardID;
	}
	else if( iFlag == 6 )
	{
		EXEC SQL OPEN CurSor02 USING :iPhoneNo,:vCardID;
	}
	else if( iFlag == 7 )
	{
		EXEC SQL OPEN CurSor02 USING :vOrderID,:iPhoneNo,:vCardID;
	}		
	if( 0 != SQLCODE )
	{
		sprintf(oRetCode, "g09708");
		sprintf(oRetMsg, "open cursor02 err[%d]iFlag[%d]",SQLCODE,iFlag);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	for(;;)
	{
		init(vdelivery_type);
		init(vdelivery_name);
		init(vorder_time);
		init(vorder_channel);
		init(vcust_name);
		init(videntity_card);
		init(vadress);
		init(vdelivery_time);
		init(vinvoice);
		init(vorder_phoneno);
		init(vmode_code);
		init(vbuy_phone);
		init(vmodel);
		init(vbind_cycle);
		init(vorder_id);
		init(vop_time);
		init(vphoneno);
		init(vchoosemodecode);
		init(vgrant_accept);
		init(vgroup_name);
		init(vphone_type);
		init(vstatus_name);
		init(vchnflag);
		init(vEmsNo);
		
		EXEC SQL FETCH CurSor02 into :vdelivery_type,:vdelivery_name,:vorder_time,:vorder_channel,:vcust_name,:videntity_card,:vadress,
							:vdelivery_time,:vinvoice,:vorder_phoneno,:vmode_code,:vreserve,:vbuy_phone,:vmodel,:vprice,:vbind_cycle,
							:vlowest_consume,:vorder_id,:vop_time,:vphoneno,:vchoosemodecode,:vgrant_accept,:vstatus_name,:vgroup_name,
							:vphone_type,:vchnflag,:vEmsNo;
		if( 1403 == SQLCODE ) break;
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09709");
			sprintf(oRetMsg, "select dWebDelivery err [%d]",SQLCODE);
			EXEC SQL CLOSE CurSor02;
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		Trim(vdelivery_type);
		Trim(vdelivery_name);
		Trim(vorder_time);
		Trim(vorder_channel);
		Trim(vcust_name);
		Trim(videntity_card);
		Trim(vadress);
		Trim(vdelivery_time);
		Trim(vinvoice);
		Trim(vorder_phoneno);
		Trim(vmode_code);
		Trim(vbuy_phone);
		Trim(vmodel);
		Trim(vbind_cycle);
		Trim(vorder_id);
		Trim(vop_time);
		Trim(vphoneno);
		Trim(vchoosemodecode);
		Trim(vgrant_accept);
		Trim(vgroup_name);
		Trim(vphone_type);		
		Trim(vstatus_name);
		Trim(vchnflag);
		Trim(vEmsNo);
		i++;
		
		init(vModeName);
		init(vRegionCode);
		init(vchoosemodename);
		init(v_mode_str);
		init(vCModeName);
		if( strcmp(vdelivery_type,"0") == 0 ) /*����*/
		{	
			EXEC SQL SELECT region_code
					into :vRegionCode
					from dcustres 
					where phone_no=:vorder_phoneno;
					/*and resource_code='p';*/
			if(SQLCODE!=0)
			{
				sprintf(oRetCode, "g09711");
				sprintf(oRetMsg, "����վԤԼ���� [%d]",SQLCODE);
				EXEC SQL CLOSE CurSor02;
				PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}
			Trim(vRegionCode);
			
			/*���ʷ����Ʋ�ѯ*/
			EXEC SQL SELECT mode_name
				into :vModeName
				from sbillmodecode
				where region_code=:vRegionCode
				and mode_code=:vmode_code;
			if(SQLCODE!=0)
			{
				strcpy(oRetCode,"g09712");
				sprintf(oRetMsg, "ԤԼ�����ʷ��ײͲ�����[%d]",SQLCODE);
				PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode,oRetMsg);
			}
			Trim(vModeName);
			
			printf("vchoosemodecode=%s\n",vchoosemodecode);
			/*��ѡ�ʷ����Ʋ�ѯ*/
			j=0;
			while(getValueByIndex(vchoosemodecode, j, '|', v_mode_str)>=0)
			{
				EXEC SQL SELECT mode_name
					into :vchoosemodename
					from sbillmodecode
					where region_code=:vRegionCode
					and mode_code=:v_mode_str;
				if(SQLCODE!=0)
				{
					strcpy(oRetCode,"g09713");
					sprintf(oRetMsg, "ԤԼ�Ŀ�ѡ�ײͲ�����[%d]",SQLCODE);
					PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_, service_name, "%s", "%s", oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode,oRetMsg);
				}
				Trim(vchoosemodename);
				
				sprintf(vchoosemodename,"%s|",vchoosemodename);
				strcat(vCModeName,vchoosemodename);
				j++;
			}
			Trim(vCModeName);
		}
		
		transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
		transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
		transOUT = Add_Ret_Value(GPARM32_2,vdelivery_name);
		transOUT = Add_Ret_Value(GPARM32_3,vorder_time);
		transOUT = Add_Ret_Value(GPARM32_4,vorder_channel);
		transOUT = Add_Ret_Value(GPARM32_5,vcust_name);
		transOUT = Add_Ret_Value(GPARM32_6,videntity_card);
		transOUT = Add_Ret_Value(GPARM32_7,vadress);
		transOUT = Add_Ret_Value(GPARM32_8,vdelivery_time);
		transOUT = Add_Ret_Value(GPARM32_9,vinvoice);
		transOUT = Add_Ret_Value(GPARM32_10,vorder_phoneno);
		transOUT = Add_Ret_Value(GPARM32_11,vModeName);
		init(vStrBuff);
		sprintf(vStrBuff,"%ld",vreserve);
		transOUT = Add_Ret_Value(GPARM32_12,vStrBuff);
		transOUT = Add_Ret_Value(GPARM32_13,vbuy_phone);
		transOUT = Add_Ret_Value(GPARM32_14,vmodel);
		init(vStrBuff);
		sprintf(vStrBuff,"%ld",vprice);		
		transOUT = Add_Ret_Value(GPARM32_15,vStrBuff);
		transOUT = Add_Ret_Value(GPARM32_16,vbind_cycle);
		init(vStrBuff);
		sprintf(vStrBuff,"%ld",vlowest_consume);			
		transOUT = Add_Ret_Value(GPARM32_17,vStrBuff);
		transOUT = Add_Ret_Value(GPARM32_18,vorder_id);
		transOUT = Add_Ret_Value(GPARM32_19,vop_time);
		transOUT = Add_Ret_Value(GPARM32_20,vphoneno);
		transOUT = Add_Ret_Value(GPARM32_21,vCModeName);
		transOUT = Add_Ret_Value(GPARM32_22,vgrant_accept);
		transOUT = Add_Ret_Value(GPARM32_23,vgroup_name);
		transOUT = Add_Ret_Value(GPARM32_24,vphone_type);
		transOUT = Add_Ret_Value(GPARM32_25,vstatus_name);
		transOUT = Add_Ret_Value(GPARM32_26,vchnflag);	
		transOUT = Add_Ret_Value(GPARM32_27,vEmsNo);		
	}
	EXEC SQL CLOSE CurSor02;
	if( i == 0 )
	{
		sprintf(oRetCode, "g09710");
		sprintf(oRetMsg, "��ѯ������");
		EXEC SQL CLOSE CurSor02;
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}	
	
SERVICE_FINALLY_DECLARE();
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetCode=[%s]\n",oRetCode);
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetMsg =[%s]\n",oRetMsg);

	if(atol(oRetCode) == 0)
	{
		EXEC SQL COMMIT;
	}
	else
	{
		EXEC SQL ROLLBACK;
		transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
		transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	}
	
	/*TODO:����������������������Ľ��*/
	
	/*
	+Service������������+
	*/
TUX_SERVICE_END();	
	
}

/*@service information
 *@name 						sWebDelivChk
 *@description			���Ͷ���У��
 *@author						miaoyl
 *@created					20130128
 *@input parameter information
 *@inparam					iLoginNo				[��������]
 *@inparam					vDeliveryType		[��������] 0Ϊ���Ϳ���1Ϊ�����ն� 
 *@inparam					iPhoneNo				[�ֻ�����] 	 
 *@inparam					iIdentityCard 	[֤������]
 *@inparam					vCustName				[֤������] 
 *@inparam					vPhoneType			[��������] 0 ��ͨ���� 1 ��������
 
 *@output parameter information     
 *@outparam					oRetCode		[���ش���]
 *@outparam					oRetMsg			[������Ϣ]
 
 @test case
 cl32 sWebDelivChk 6 2 www000 0 13844800675 22018119781011041x "���޼�" 1
*/
_TUX_SVC_ sWebDelivChk(TPSVCINFO *transb)
{
	char	service_name[20+1];
	int		iCount=0,iDCount=0;
	EXEC SQL BEGIN DECLARE SECTION;
			long	lLoginAccept = 0;
			char	iPhoneNo[11 + 1];
			char	iLoginNo[6+1];
			char	iIdentityCard[18+1];
			char	vCardType[2+1];
			char	vDeliveryType[1+1];
			char	vCustName[60+1];
			char	vPhoneNo[15+1];
			char	oAllowFlag[1+1];
			char	vPhoneType[1+1]; /*����״̬ 0 ��ͨ���� 1��������*/
			char	vValidFlag[1+1];
			tUserBase	userBase;
	EXEC SQL END DECLARE SECTION;
	
	/*input_params*/
	init(iPhoneNo);
	init(iLoginNo);
	init(iIdentityCard);
	init(vCardType);
	init(vDeliveryType);
	init(vCustName);
	init(oAllowFlag);
	init(vPhoneType);
	init(vValidFlag);
	init((void*)(&userBase));
	
	init(service_name);
	strcpy(service_name, "sWebDelivQry");
	PUBLOG_APPRUN(service_name, "", "����%s��ʼִ��", service_name);
	
	TUX_SERVICE_BEGIN(lLoginAccept, "", iLoginNo, "������ϢУ��","0", iPhoneNo, NULL, NULL);

	strcpy(iLoginNo,			input_parms[0]);
	strcpy(vDeliveryType,	input_parms[1]);
	strcpy(iPhoneNo,			input_parms[2]);
	strcpy(iIdentityCard, input_parms[3]);
	strcpy(vCustName, 		input_parms[4]);
	strcpy(vPhoneType,		input_parms[5]);
	
	Trim(iLoginNo);
	Trim(vDeliveryType);
	Trim(iPhoneNo);
	Trim(iIdentityCard);
	Trim(vDeliveryType);
	
	/*�����ʸ�У��*/
	/*a.����������������һ�ʶ����𣨼��������ʱ�䣩,�����֤������30���ڲ����ٴ�����ѡ�Ŷ���*/
	/*��ͨ���������*/
	EXEC SQL SELECT CASE WHEN sysdate-MAX(op_time)<30 THEN '0' ELSE '1' END  into :vValidFlag
							 FROM dphonereserveblack 
							WHERE id_iccid=:iIdentityCard;
	if( 0 != SQLCODE )
	{
		sprintf(oRetCode, "g09721");
		sprintf(oRetMsg, "��ѯ��ͨ�������������[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vValidFlag);
	if( strcmp(vValidFlag,"0") == 0 )
	{
		sprintf(oRetCode, "g09722");
		sprintf(oRetMsg, "������ͨ�����������,�����֤������30���ڲ����ٴ�����ѡ�Ŷ���");
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	/*�������������*/
	init(vValidFlag);
	EXEC SQL SELECT CASE WHEN sysdate-MAX(op_time)<30 THEN '0' ELSE '1' END  into :vValidFlag
						 FROM dgoodphoneblacklist 
						WHERE id_iccid=:iIdentityCard;
	if( 0 != SQLCODE )
	{
		sprintf(oRetCode, "g09723");
		sprintf(oRetMsg, "��ѯ�����������������[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vValidFlag);
	if( strcmp(vValidFlag,"0") == 0 )
	{
		sprintf(oRetCode, "g09724");
		sprintf(oRetMsg, "�������������������,�����֤������30���ڲ����ٴ�����ѡ�Ŷ���");
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
		
	/*0Ϊ���Ϳ���1Ϊ�����ն�*/
	if( strcmp(vDeliveryType,"0")==0 )
	{	
		/*b.ͬһ���֤���ƶ����뻧��������5��=�ɹ���������+�������Ѿ����ڵ� 010 δ���� 020 �Ѹ���*/
		/*�ɹ���������*/
		iCount=0;
		EXEC SQL SELECT COUNT(*) into :iCount FROM dCustMsg a WHERE 
							EXISTS (SELECT 1 FROM dCustDoc b WHERE
							b.id_iccid=:iIdentityCard AND a.Cust_Id=b.Cust_id )
							and substr(a.run_code,2,1) not in ('a','b','c');
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09725");
			sprintf(oRetMsg, "��ѯ�û�������Ϣ����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		pubLog_Debug(_FFL_,service_name,"","iCount=[%d]",iCount);
		
		/*�������Ѿ����ڵ� 010 δ���� 020 �Ѹ��� ��Ч��������־*/
		iDCount=0;
		EXEC SQL select COUNT(*) into :iDCount from dwebdelivery
							where identity_card=:iIdentityCard
								and	order_status=any('010','020');
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09726");
			sprintf(oRetMsg, "��ѯ�û�������Ϣ����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		if( iCount+iDCount >= 5 )
		{
			sprintf(oRetCode, "g09727");
			sprintf(oRetMsg, "ͬһ���֤[%s]���ƶ����뻧���Ѵ��ڵ���5��[%d]",iIdentityCard,iCount+iDCount);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*c.����30����,ͬһ���֤��������ѡ�Ŷ�������δ���������Ĵ����ۼƴﵽ5�Σ����¼������*/
		iCount=0;
		EXEC SQL SELECT COUNT(*) into :iCount FROM dwebdelivery 
							WHERE identity_card=:iIdentityCard 
							  AND op_time>=SYSDATE-30
							  AND order_status=ANY('010','012');
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09730");
			sprintf(oRetMsg, "��ѯ������Ϣ����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		if( iCount >= 5 )
		{
			/*��¼������*/
			if( strcmp(vPhoneType,"0") == 0 )/*��ͨ����*/
			{
				EXEC SQL INSERT INTO dphonereserveblack(id_type,id_iccid,cust_name,op_time,query_chn)
									VALUES(:vDeliveryType,:iIdentityCard,:vCustName,SYSDATE,'0');
				if( 0 != SQLCODE )
				{
					sprintf(oRetCode, "g09733");
					sprintf(oRetMsg, "��¼��ͨ�������������[%d]",SQLCODE);
					PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode, oRetMsg);
				}
			}	
			else if( strcmp(vPhoneType,"1") == 0 )/*��������*/
			{	
				EXEC SQL INSERT into dgoodphoneblacklist(id_type,id_iccid,cust_name,op_time)
									VALUES(:vDeliveryType,:iIdentityCard,:vCustName,SYSDATE);
				if( 0 != SQLCODE )
				{
					sprintf(oRetCode, "g09734");
					sprintf(oRetMsg, "��¼�����������������[%d]",SQLCODE);
					PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
					RAISE_ERROR(oRetCode, oRetMsg);
				}
			}
			
			EXEC SQL COMMIT;
			
			sprintf(oRetCode, "g09735");
			sprintf(oRetMsg, "����30��,֤�����ɶ�����δ�����ۼƴ�5������,�������");
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
		/*d.ͬһ���֤���������ĺ��벻����Ƿ�ѣ�����������Ƚ���*/
		EXEC SQL DECLARE CurSor01 CURSOR for
						SELECT a.phone_no FROM dCustMsg a WHERE 
												EXISTS (SELECT 1 FROM dCustDoc b WHERE
												b.id_iccid=:iIdentityCard AND a.Cust_Id=b.Cust_id )
												and substr(a.run_code,2,1) not in ('a','b','c');
		EXEC SQL OPEN CurSor01;
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09728");
			sprintf(oRetMsg, "open cursor01 err[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		for(;;)
		{
			init(vPhoneNo);
			EXEC SQL FETCH CurSor01 into :vPhoneNo;
			if( 1403 == SQLCODE ) break;
			if( 0 != SQLCODE )
			{
				sprintf(oRetCode, "g09729");
				sprintf(oRetMsg, "fetch cursor01 err[%d]",SQLCODE);
				EXEC SQL CLOSE CurSor01;
				PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}
			Trim(vPhoneNo);
			
			pubLog_Debug(_FFL_,service_name,"","\nvPhoneNo=[%s]",vPhoneNo);
			
			init(oAllowFlag);
			/*У��Ƿ��*/
			if(QueryRemainFee("bs_QryRemainFee",vPhoneNo,"xxxx","1","0",oRetCode,oRetMsg,oAllowFlag)!=0)
			{
				EXEC SQL CLOSE CurSor01;
				PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}
			pubLog_Debug(_FFL_,service_name,"","oAllowFlag=[%s]",oAllowFlag); /* ��������Ĵ�ӡ*/
			if(strcmp(oAllowFlag,"0")==0)
			{
				sprintf(oRetCode, "g09720");
				sprintf(oRetMsg, "�û�%s�ʻ�����Ƿ��,����������ҵ��!",vPhoneNo);	
				EXEC SQL CLOSE CurSor01;
				PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
				RAISE_ERROR(oRetCode, oRetMsg);
			}
		}
		EXEC SQL CLOSE CurSor01;
	}
	else if( strcmp(vDeliveryType,"1")==0 )
	{
		/*b.�Ƿ���ն� �ڷ���s2841Init���Ѿ�ʵ��*/
		/*c.�û�״̬�Ƿ����� ״̬ΪF I J���������ɶ���*/
		sprintf(oRetCode, "%06d", fGetUserBaseInfo(iPhoneNo,&userBase)); 
		if( strcmp(oRetCode, "000000") != 0 )
		{
			strcpy(oRetCode,"g09731");
			strcpy(oRetMsg,"��ѯ�û���Ϣʧ��");
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
		PUBLOG_DBERR(service_name,"","userBase.run_code[%s]",userBase.run_code);
		if(userBase.run_code[0]=='F' || userBase.run_code[0]=='I' || userBase.run_code[0]=='J')
		{
			strcpy(oRetCode,"g09732");
			strcpy(oRetMsg,"�û�����״̬���������");
			PUBLOG_DBERR(service_name,"%s","%s",oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode,oRetMsg);
		}
	}
	
SERVICE_FINALLY_DECLARE();
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetCode=[%s]\n",oRetCode);
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetMsg =[%s]\n",oRetMsg);

	if( strcmp(oRetCode,"000000") == 0 )
	{
		EXEC SQL COMMIT;
	}
	else
	{
		EXEC SQL ROLLBACK;
	}
	
	/*TODO:����������������������Ľ��*/
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	
	/*
	+Service������������+
	*/
TUX_SERVICE_END();	
	
}

/*@service information
 *@name 						sWebDelivUpd
 *@description			���Ͷ����޸�
 *@author						miaoyl
 *@created					20130118
 *@input parameter information
 *@inparam					iLoginNo				[��������]
 *@inparam					iOrgCode				[���Ź���] 
 *@inparam					iLoginPwd				[��������]
 *@inparam					vOrderID				[����ID]
 *@inparam					vOprType 				[��������] u �޸� �û���������ȡ������
 *@inparam					vIpAddr					[����IP] 
 
 *@output parameter information     
 *@outparam					oRetCode		[���ش���]
 *@outparam					oRetMsg			[������Ϣ]
 
 @test case
	cl32 sWebDelivUpd 6 2 www000 0000006in CODCDOJHGECIFONG 20130130105200004 u ��0.0.0.0��
*/
_TUX_SVC_ sWebDelivUpd(TPSVCINFO *transb)
{
	char	service_name[20+1];
	
	EXEC SQL BEGIN DECLARE SECTION;
			long	lLoginAccept = 0;
			char	iOpCode[4 + 1];
			char	iPhoneNo[11 + 1];
			char	iLoginNo[6+1];
			char	iLoginPwd[16+1];
			char	iOrgCode[9+1];	
			char	vIpAddr[20+1];
			
			char	vCardID[18+1];
			char	vOrderID[20+1];
			char	vOprType[1+1];
			
			int		iCount=0;
			char	vOrderStatus[10+1];
			long	tLoginAccept=0;
			char	vTotalDate[8+1];
			char  vReservationTime[14+1];
	EXEC SQL END DECLARE SECTION;
	
	char vStrBuff[20+1];
	
	/*input_params*/
	init(iOpCode);
	init(iPhoneNo);
	init(iLoginNo);
	init(iLoginPwd);
	init(iOrgCode);
	init(vIpAddr);
	
	init(vCardID);
	init(vOrderID);
	init(vOprType);
	
	init(vOrderStatus);
	init(vTotalDate);
	init(vReservationTime);
	
	init(service_name);
	strcpy(service_name, "sWebDelivUpd");
	PUBLOG_APPRUN(service_name, "", "����%s��ʼִ��", service_name);
	
	TUX_SERVICE_BEGIN(lLoginAccept, iOpCode, iLoginNo, "������Ϣ�޸�","0", iPhoneNo, NULL, NULL);

	strcpy(iLoginNo,				input_parms[0]);
	strcpy(iOrgCode,				input_parms[1]);
	strcpy(iLoginPwd,				input_parms[2]);
	strcpy(vOrderID,				input_parms[3]);
	strcpy(vOprType,				input_parms[4]);
	strcpy(vIpAddr,					input_parms[5]);
	
	Trim(iLoginNo);
	Trim(iLoginPwd);
	Trim(iOrgCode);
	Trim(vOrderID);
	Trim(vOprType);
	Trim(vIpAddr);
	
	/*У�鶩��״̬*/
	iCount=0;
	EXEC SQL select decode(order_status,'011','ȡ��','012','ʧЧ','020','�Ѹ���','030','�ɹ�',order_status)
						 into :vOrderStatus
						 from DWEBDELIVERY 
						 where order_id=:vOrderID;
						/*where order_id=rpad(:vOrderID,18);*/
	if( 0 != SQLCODE )
	{
		sprintf(oRetCode, "g09701");
		sprintf(oRetMsg, "select DWEBDELIVERY err[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vOrderStatus);
	
	if( strcmp(vOrderStatus,"010") != 0 )
	{
		sprintf(oRetCode, "g09702");
		sprintf(oRetMsg, "����״̬Ϊ[%s],�޷��޸Ĵ˶���",vOrderStatus);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	/*�޸Ķ���*/
	if( strcmp(vOprType,"u") == 0 )
	{
		/*��¼��ʷ����*/
		EXEC SQL insert into DWEBDELIVERYHIS
			(delivery_type, order_time, order_channel, cust_name, 
			identity_card,adress, delivery_time, invoice, 
			order_phoneno, mode_code, reserve, buy_phone, model, 
			price, bind_cycle, lowest_consume, order_id, org_id, 
			total_date, op_time, phoneno, choosemodecode, 
			grant_accept, order_status,contact_orgid,phone_type,chn_flag,
			Update_Accept,  Update_Time,Update_Login,Update_Type,
			Update_Code,Update_Date,update_note)
		select delivery_type, order_time, order_channel, cust_name, 
			identity_card,adress, delivery_time, invoice, 
			order_phoneno, mode_code, reserve, buy_phone, model, 
			price, bind_cycle, lowest_consume, order_id, org_id, 
			total_date, op_time, phoneno, choosemodecode, 
			grant_accept, order_status,contact_orgid,phone_type,chn_flag,
			smaxsysaccept.nextval,sysdate,:iLoginNo,'U',
			'g097',to_number(to_char(sysdate,'yyyymmdd')),'��������ȡ������'
		from DWEBDELIVERY
		where order_id=:vOrderID;
		if(SQLCODE !=0)
		{
			sprintf(oRetCode, "g09705");
			sprintf(oRetMsg, "�����DBCUSTADM.DWEBDELIVERYHIS����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}	
		/*�޸�Ϊ����ȡ����־*/
		EXEC SQL update dwebdelivery set order_status='011' where order_id=:vOrderID;
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09703");
			sprintf(oRetMsg, "�޸Ķ���״̬����[%d]",SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}
	else
	{
		sprintf(oRetCode, "g09704");
		sprintf(oRetMsg, "����������ʹ���[%s]",vOprType);
		PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}		
	
SERVICE_FINALLY_DECLARE();
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetCode=[%s]\n",oRetCode);
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetMsg =[%s]\n",oRetMsg);

	if( strcmp(oRetCode,"000000") == 0 )
	{
		EXEC SQL COMMIT;
	}
	else
	{
		EXEC SQL ROLLBACK;
	}
	
	/*TODO:����������������������Ľ��*/
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	
	/*
	+Service������������+
	*/
TUX_SERVICE_END();	
}

/*@service information
 *@name 					sg097Upd
 *@description				����״̬�޸�(g097ģ��)
 *@author					majha
 *@created					20130424
 *@input parameter information
 *@inparam0					iLoginNo				[��������]
 *@inparam1					iOpCode					[��������] 
 *@inparam2					iOrgCode				[���Ź���] 
 *@inparam3					iLoginPwd				[��������]
 *@inparam4					iOrderID				[����ID]
 *@inparam5					iIpAddr					[����IP] 
 *@inparam6					iOrderStatus 			[����״̬]
 *@inparam7					iEmsNo 					[�˵���]
 *@inparam8					iOpNote 				[��ע��Ϣ]

	 
 *@output parameter information     
 *@outparam					oRetCode		[���ش���]
 *@outparam					oRetMsg			[������Ϣ]
 
 @test case
	cl32 sg097Upd 6 2 www000 0000006in CODCDOJHGECIFONG 20130130105200004 u ��0.0.0.0��
*/
_TUX_SVC_ sg097Upd(TPSVCINFO *transb)
{
	char	service_name[20+1];
	
	EXEC SQL BEGIN DECLARE SECTION;
			long	lLoginAccept = 0;
			char	iOpCode[4 + 1];
			char	iPhoneNo[11 + 1];
			char	iLoginNo[6+1];
			char	iLoginPwd[16+1];
			char	iOrgCode[9+1];	
			char	iOrderID[20+1];
			char	iOrderStatus[3+1];
			char	iEmsNo[25+1];
			char	iIpAddr[20+1];
			char	iOpNote[60+1];
			int		iCount=0;		
			long	tLoginAccept=0;
			char	vTotalDate[8+1];
			char	vOrgId[10+1];
			char	vOpTime[20+1];
			tLoginOpr vLog;
			char	vChgFlag[1+1];
			char	vCustFlag[1+1];
			int		ret=0;
	EXEC SQL END DECLARE SECTION;
	
	char vStrBuff[20+1];
	
	/*input_params*/
	init(iOpCode);
	init(iPhoneNo);
	init(iLoginNo);
	init(iLoginPwd);
	init(iOrgCode);
	init(iIpAddr);
	init(iOrderID);
	init(iOrderStatus);
	init(iEmsNo);
	init(vTotalDate);
	init(iOpNote);
	init(vOrgId);
	init(vOpTime);
	init(vChgFlag);
	init(vCustFlag);
	memset(&vLog,0,sizeof(vLog));
	init(service_name);
	strcpy(service_name, "sg097Upd");
	PUBLOG_APPRUN(service_name, "", "����%s��ʼִ��", service_name);
	
	TUX_SERVICE_BEGIN(lLoginAccept, iOpCode, iLoginNo, iOpNote,"0", iPhoneNo, NULL, NULL);

	strcpy(iLoginNo,				input_parms[0]);
	strcpy(iOpCode,					input_parms[1]);
	strcpy(iOrgCode,				input_parms[2]);
	strcpy(iLoginPwd,				input_parms[3]);
	strcpy(iOrderID,				input_parms[4]);	
	strcpy(iIpAddr,					input_parms[5]);
	strcpy(iOrderStatus,			input_parms[6]);
	strcpy(iEmsNo,					input_parms[7]);
	strcpy(iOpNote,					input_parms[8]);
	
	Trim(iLoginNo);
	Trim(iOpCode);
	Trim(iOrgCode);
	Trim(iLoginPwd);
	Trim(iOrderID);
	Trim(iIpAddr);
	Trim(iOrderStatus);
	Trim(iEmsNo);
	Trim(iOpNote);
	
	/*ȡ��ˮ*/
	EXEC SQL SELECT sMaxSysAccept.nextVal, to_char(sysdate,'yyyymmdd'),
					to_char(sysdate,'yyyymmdd hh24:mi:ss')
			INTO :lLoginAccept,:vTotalDate,:vOpTime
			FROM dual;
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"000021");
		sprintf(oRetMsg,"ȡϵͳʱ��ʧ��!,sqlcode[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vTotalDate);
	Trim(vOpTime);
	
	EXEC SQL select org_id
				into :vOrgId
				from dloginmsg
				where login_no=:iLoginNo;
	if(SQLCODE!=0)
	{
		strcpy(oRetCode,"000021");
		sprintf(oRetMsg,"ȡϵͳʱ��ʧ��!,sqlcode[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	Trim(vOrgId);
	
	/*��¼��ʷ����*/
	EXEC SQL insert into DWEBDELIVERYHIS
		(delivery_type, order_time, order_channel, cust_name, 
		identity_card,adress, delivery_time, invoice, 
		order_phoneno, mode_code, reserve, buy_phone, model, 
		price, bind_cycle, lowest_consume, order_id, org_id, 
		total_date, op_time, phoneno, choosemodecode, 
		grant_accept, order_status,contact_orgid,phone_type,chn_flag,
		Update_Accept,  Update_Time,Update_Login,Update_Type,
		Update_Code,Update_Date,update_note,order_source,ems_no)
	select delivery_type, order_time, order_channel, cust_name, 
		identity_card,adress, delivery_time, invoice, 
		order_phoneno, mode_code, reserve, buy_phone, model, 
		price, bind_cycle, lowest_consume, order_id, org_id, 
		total_date, op_time, phoneno, choosemodecode, 
		grant_accept, order_status,contact_orgid,phone_type,chn_flag,
		smaxsysaccept.nextval,sysdate,:iLoginNo,'U',
		:iOpCode,to_number(to_char(sysdate,'yyyymmdd')),:iOpNote,order_source,ems_no
	from DWEBDELIVERY
	where order_id=:iOrderID;
	if(SQLCODE !=0)
	{
		sprintf(oRetCode, "g09755");
		sprintf(oRetMsg, "�����DBCUSTADM.DWEBDELIVERYHIS����[%d]",SQLCODE);
		PUBLOG_APPERR(__FUNCTION__, oRetCode, oRetMsg);
		RAISE_ERROR(oRetCode, oRetMsg);
	}
	
	/*У�鶩��״̬*/
	if(strcmp(iOrderStatus,"040")==0)
	{
		EXEC SQL update dwebdelivery 
			set order_status=:iOrderStatus,
				ems_no=:iEmsNo,
				op_time=sysdate
			 where order_id=:iOrderID;
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09756");
			sprintf(oRetMsg, "�޸Ķ���״̬����[%s][%d]",iOrderID,SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
		
	}
	else
	{
		EXEC SQL update dwebdelivery 
			set order_status=:iOrderStatus,
				op_time=sysdate
			 where order_id=:iOrderID;
		if( 0 != SQLCODE )
		{
			sprintf(oRetCode, "g09757");
			sprintf(oRetMsg, "�޸Ķ���״̬����[%s][%d]",iOrderID,SQLCODE);
			PUBLOG_APPERR(__FUNCTION__, oRetCode,oRetMsg);
			RAISE_ERROR(oRetCode, oRetMsg);
		}
	}
	
	vLog.vId_No                            =  0;
	vLog.vTotal_Date                       =  atoi(vTotalDate);
	vLog.vLogin_Accept                     =  lLoginAccept;
	
	strncpy(vLog.vSm_Code,       "xx",      2);
	strncpy(vLog.vBelong_Code,   iOrgCode,     7);
	strncpy(vLog.vPhone_No,      "0",       15);
	strncpy(vLog.vOrg_Code,      iOrgCode,         9);
	strncpy(vLog.vLogin_No,      iLoginNo,        6);
	strncpy(vLog.vOp_Code,       iOpCode,          4);
	strncpy(vLog.vOp_Time,       vOpTime,        17);
	strncpy(vLog.vMachine_Code,     "000",         3);
	strncpy(vLog.vBack_Flag,         "0",          1);
	strncpy(vLog.vEncrypt_Fee,       "0",         16);
	strncpy(vLog.vSystem_Note,     iOpNote,   	60);
	strncpy(vLog.vOp_Note,          iOpNote,       60);
	strncpy(vLog.vGroup_Id,       "",        10);
	strncpy(vLog.vOrg_Id,         vOrgId,          10);
	strncpy(vLog.vPay_Type,       "0",              1);
	strncpy(vLog.vIp_Addr,       "0.0.0.0",         15);
	vLog.vPay_Money                     		 = 0;
	vLog.vCash_Pay                       		= 0;
	vLog.vCheck_Pay      					  = 0;
	vLog.vSim_Fee            				  = 0;
	vLog.vMachine_Fee        				  = 0;
	vLog.vInnet_Fee          				  = 0;
	vLog.vChoice_Fee         				  = 0;
	vLog.vOther_Fee           				  = 0;
	vLog.vHand_Fee           				  = 0;
	vLog.vDeposit            				  = 0;
	
	ret = 0;
	strncpy(vChgFlag,        "N",                   1);         
	strncpy(vCustFlag,       "Y",                   1);        
	ret = recordOprLog(&vLog,oRetMsg,vChgFlag,vCustFlag);
	if(ret != 0)
	{
		sprintf(oRetCode,"%06d",ret);
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_,service_name, oRetCode,oRetMsg);
		RAISE_ERROR(oRetCode,oRetMsg);
	}
	
		
	
SERVICE_FINALLY_DECLARE();
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetCode=[%s]\n",oRetCode);
	PUBLOG_DEBUG(__FUNCTION__, service_name,"oRetMsg =[%s]\n",oRetMsg);

	if( strcmp(oRetCode,"000000") == 0 )
	{
		EXEC SQL COMMIT;
	}
	else
	{
		EXEC SQL ROLLBACK;
	}
	
	/*TODO:����������������������Ľ��*/
	transOUT = Add_Ret_Value(GPARM32_0,oRetCode);
	transOUT = Add_Ret_Value(GPARM32_1,oRetMsg);
	
	/*
	+Service������������+
	*/
TUX_SERVICE_END();	
}
