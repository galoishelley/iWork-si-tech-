/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "pubLog.h"
#include <sys/timeb.h>
#include <time.h>


#define _DEBUG_
/*#undef _DEBUG_*/

#undef MAXUSERCONNUM		/*һ���û���Ӧ�������ʻ�����*/
#define MAXUSERCONNUM 10

#ifndef MAXENDTIME
#define MAXENDTIME "20500101"
#endif

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/************************************************************************
��������:fIsAllowChg
����ʱ��:2003/10/19
������Ա:liwd
��������:�ж��Ƿ�ĳ�����Ƿ��������
�������:�ƶ�����
��������
��������
�û�Ƿ����Ϣ
�û���Ƿ��
�û���Ԥ��
�û�δ���ʻ���
��һ���ʺ�
��һ���ʺ�Ƿ��
�������:

�� �� ֵ:0������ȷ����,��������
************************************************************************/
/**
* lixg->ע�⣺����������Ҫ�޸ĺ�ʹ�ã� ʹ������ĺ��� fCheckChgAllow��
**/
int fIsAllowChg(const char* phone_no, const char* op_code, const char* work_no, tUserOwe user_owe, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char		vcontract_no[23];
	char        vst[1024];
	char		vregion_char[100+1];
	char        vregion_code[3];
	char        vdistrict_code[3];
	char        vrun_code[2];
	char        vrun_time[9];
	char        vbelong_code[8];
	char        vpay_code[2];
	char		vsm_code[2+1];
	double      vprepay_fee=0;
	double		vunbill=0;
	char        vunbill_fee[MAXMONEYLEN+1];
	char        vunbill_month_fee[MAXMONEYLEN+1];
	char        vid_no[23];
	char        vallow_flag[2];
	char        vreturn_code[5];
	char        vreturn_msg[61];
	char        vowe_flag[2];
	char		phoneNo[15+1];
	int         vNum=0;
	int         vcurtime=0;
	int         vpre_month=0;
	int         vpredel_month=0;
	char        vorg_code[10];
	char		vContractNo[22+1];
	double		vowe_fee;
	short int   in=0;

	char 		sOpCode[4 + 1];
	char 		sWorkNo[6 + 1];
	char 		sCheckType[1 + 1]; 

	int			i=0,vCount=0;
	int			vret=0,iret=0 ,ret = 0;
	/*��֯�ṹ������� editer =wanfh  date = 20090509*/
	char vOrgId[10+1];
	char vGroupId[10+1];
	char Vflag[1+1];
	EXEC SQL END DECLARE SECTION;

	init(vcontract_no);
	init(vContractNo);
	init(vid_no);
	init(vregion_code);
	init(vdistrict_code);
	init(vrun_code);
	init(vbelong_code);
	init(vrun_time);
	init(vpay_code);
	init(vsm_code);        
	init(vrun_time);        
	init(vunbill_fee);
	init(vunbill_month_fee);
	init(vorg_code);
	init(sCheckType);
	init(vOrgId);
	init(vGroupId);
	init(Vflag);
	memset(vregion_char, 0, sizeof(vregion_char));
	vprepay_fee=0;

	strcpy(sOpCode, op_code);
	strcpy(sWorkNo, work_no);
	strcpy(phoneNo, phone_no);
	Trim(phoneNo);
	phoneNo[11]= '\0';

	/**ȡӪҵԱ�Ļ�������**/
	EXEC SQL	SELECT org_code, region_char
				  INTO :vorg_code, :vregion_char
	  			  FROM dLoginMsg 
	  			 WHERE login_no=:sWorkNo;
	if (SQLCODE != 0)
	{
#ifdef _DEBUG_
		/*printf("SQLCODE=[%d], SQLERRMS=[%s] sWorkNo=[%s]\n", SQLCODE, SQLERRMSG, sWorkNo);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1200","SQLCODE=[%d]sWorkNo=[%s]", SQLCODE, sWorkNo);
#endif
        strcpy(retMsg,"ȡӪҵԱ�Ļ����������");
       
		return 1200;
	}
	/*--��֯�������������ֶ�edit by wanfh at 14/05/2009--begin*/
       ret = sGetLoginOrgid(sWorkNo,vOrgId);
       if(ret <0)
         	{
		         strcpy(retMsg,"��ѯ����org_idʧ��!");
		         pubLog_DBErr(_FFL_, "", "", "%s", retMsg);
		         return 1201 ;
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by wanfh at 14/05/2009---end*/
	/**ȡ�û�id **//*��֯�ṹ���ڸ���   ��ȡgroup_id editer=wanfh  date=20090508*/
	sprintf(vst,    "select to_char(id_no),substr(run_code,2,1),\
					to_char(contract_no), sm_code,\
					to_char(run_time,'yyyymmdd'),substr(belong_code,1,2),\
					substr(belong_code,3,2),belong_code ,nvl(group_no,'')\
					from dCustMsg \
					where phone_no=:v1 \
					and substr(run_code,2,1)<'a'");

	exec sql execute 
	begin
		execute immediate :vst into :vid_no indicator :in,:vrun_code indicator :in,
		:vContractNo indicator :in,:vsm_code indicator :in,
		:vrun_time indicator :in,:vregion_code indicator :in,
		:vdistrict_code indicator :in,:vbelong_code,:vGroupId indicator :in USING :phone_no;  
	end;
	end-exec;
	if(vid_no==NULL)
	{
		strcpy(retMsg,"ȡ�û���Ϣ����");
		pubLog_DBErr(_FFL_,"","1201","ȡ�û���Ϣ����");
		return 1201;
	}
	Trim(vGroupId);
	EXEC SQL    select count(*)
                 into :vret 
                 from dConMsg a, dConUserMsg b
                where a.contract_no = b.contract_no
                  and b.serial_no = 0
                  and b.id_no = :vid_no
                  and a.pay_code in('4','5','9');
	if (SQLCODE != 0)
	{
		strcpy(retMsg,"ȡ�˻���Ϣ����");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1009","ȡ�˻���Ϣ����");
		return 1009;
	}
	if( vret > 0 )
	{
		vpay_code[0]='4';
	}
	else
	{
		EXEC SQL	select pay_code
					into :vpay_code
				  	from dConMsg 
				 	where contract_no  = to_number(:vContractNo);
	}

	/*�жϺ���״̬�Ƿ���������    */
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
	  			  FROM cChgRun
	  			 WHERE op_code=:sOpCode and run_code=:vrun_code 
	  			   AND region_code=:vregion_code;
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
		strcpy(retMsg,"��ǰ״̬��������");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1202","��ǰ״̬��������");
		return 1202;
	}

	/*�жϺ���״̬�Ƿ��������������*/
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508*/
	/*��֯�ṹ����if (strncmp(vorg_code,vbelong_code,2)!=0 && strncmp(vorg_code,"00",2)!=0)��֯�ṹ����*/
	EXEC SQL select count(*) 
			  into  :vCount
	          from  dchngroupmsg a,dchngroupinfo b
             where  a.group_id =b.parent_group_id
               and  b.group_id=:vOrgId
               and  a.group_id='2';
	if(vCount==0)
	{
		int ret=0;
		ret =sCheckGroupId(vGroupId,vOrgId,"11",Vflag,retMsg);
		if (ret != 0)
		{
			sprintf(retMsg,"����sCheckGroupId ��������[%s]",vOrgId);
			PUBLOG_DBDEBUG("");
			pubLog_Debug(_FFL_,"","001298",retMsg);
			return 1298;
		}
		Trim(Vflag);
		if(strcmp(Vflag,"Y") != 0 )
		{
			EXEC SQL	SELECT allow_flag
						  INTO :vallow_flag
						  FROM cChgReg
						 WHERE op_code=:sOpCode 
						   AND region_code= :vregion_code;
			if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
			{
			    sprintf(retMsg, "��ҵ��������������!");
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","001215",retMsg);
				return 1215;
			}
				
			/*���ܿ����������*/
			if (vregion_char[0] != 'Y')
			{
			    sprintf(retMsg, "����[%s]û�п��������Ȩ��!", sWorkNo);
			    PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","001216",retMsg);
				return 1216;
			}
		}
	}

	/*�жϺ���״̬�Ƿ���������������*/	
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508*/
	/*��֯�ṹif (strncmp(vorg_code,vbelong_code,4)!=0)��֯�ṹ���ڸ�*/
	init(Vflag);
	if(sCheckGroupId(vGroupId,vOrgId,"12",Vflag,retMsg) != 0)
	{
		pubLog_Debug(_FFL_,"","","����sCheckGroupId ��������");
		pubLog_DBErr(_FFL_,"","001299",retMsg);
		return 1299;
	}
	Trim(Vflag);
	if(strcmp(Vflag,"Y") != 0)
	{
		/*�����ӪҵԱ������ͬ*/
		EXEC SQL	SELECT allow_flag
					  INTO :vallow_flag
					  FROM cChgDis
	  				 WHERE op_code=:sOpCode 
	  				   AND region_code=:vregion_code;
		if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
		{
			strcpy(retMsg,"�ù��ܲ��������������");
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1203","");
			return 1203;
		}
	}

	/*�ж�ҵ�������Ƿ���������*/
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
				  FROM cChgSm
				 WHERE region_code=:vregion_code 
	  			   AND op_code=:sOpCode
				   AND sm_code=:vsm_code;
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
		strcpy(retMsg,"��ǰҵ�����Ͳ����������ҵ��");
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1204","");
		return 1204;
	}
	
	/*�ж��Ƿ�����Ƿ������    */
	strcpy(vallow_flag,"0");
	strcpy(vowe_flag,"0");        
	EXEC SQL	SELECT allow_flag,owe_flag
				  INTO :vallow_flag indicator :in,  :vowe_flag indicator :in 
				  FROM cChgOwe
				 WHERE op_code=:sOpCode 
				   AND pay_code=decode(:vpay_code,'4','4','5','5','0')
				   AND region_code=:vregion_code;
	/*printf("vallow_flag=[%s], vowe_flag=[%s], user_owe.accountNo=[%s], user_owe.accountOwe=[%lf]\n",
		vallow_flag, vowe_flag, user_owe.accountNo, user_owe.accountOwe);
    */
    pubLog_Debug(_FFL_,"","","vallow_flag=[%s], vowe_flag=[%s], user_owe.accountNo=[%s], user_owe.accountOwe=[%lf]",
		vallow_flag, vowe_flag, user_owe.accountNo, user_owe.accountOwe);
		
/*Ԥ��(1216)�򸶷Ѽƻ����(1212)Ƿ��ʱ��������*/
/*
	if( strncmp(sOpCode ,"1216",4)==0 || strncmp(sOpCode ,"1212",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
				return 1007;
			}
	}
*/
	if( strncmp(sOpCode ,"1216",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
				strcpy(retMsg,"���û���Ƿ���˺Ų���������ҵ��");
				pubLog_DBErr(_FFL_,"","1007","");
				return 1007;
			}
	}

	if (atoi(vallow_flag)==1 && atoi(vowe_flag)==0)
	{	
		/*��Ƿ�Ѵ������ʱ��������*/	
		if ( user_owe.totalOwe - user_owe.totalPrepay > 0.005)
		{
			strcpy(retMsg,"���Ƚ���Ƿ���ٰ����ҵ��");
			pubLog_DBErr(_FFL_,"","1008",retMsg);
			return 1008;
		}
	}
/****************************
@@@��ѯ�û��Ƿ��ں�������,���û��������ֻ��Ƿ�Ƿ��@@@
	strcpy(sCheckType,"A");
	if ( ( iret = fChkBlackOwe(phoneNo,sCheckType)) != 0 )
		return iret;	

@@@@��ѯʹ�ø��û����֤�����������û��Ƿ�Ƿ��,����û���һ���ʻ�Ϊ�����ʻ�,����Ϊ�����û���Ƿ��@@@
	iret = fGetIccOwe(phoneNo);
	if( iret > 0 )
		return 1017;
	else if ( iret < 0 )
		return 1018;
*****************************/
	strcpy(retMsg,"У��ɹ�");
	return 0;
}

/************************************************************************
��������: fCheckChgAllow()
����ʱ��: 2005/11/20
������Ա:liwd
��������:�ж��Ƿ�ĳ�����Ƿ��������
�������:�ƶ�����,��������,��������,
�û�Ƿ����Ϣ
{
    �û���Ƿ��
    �û���Ԥ��
    �û�δ���ʻ���
    ��һ���ʺ�
    ��һ���ʺ�Ƿ��
}.
�������: oErrMsg
�� �� ֵ: 0 ������ȷ����, ��������!
ע�⣺�ڸú��������������ҵ�����Ƶ����ù��ܣ���������Ƶ���������
      ��ҵ�����ģ�Ϳ�����
************************************************************************/
int fCheckChgAllow(const char* phone_no, const char* op_code, const char* work_no, tUserOwe user_owe, char *oErrMsg)
{
    CFUNCCONDENTRY fe;
    char tmpRetCode[6+1];
    char vServiceName[20+1];
    
	EXEC SQL BEGIN DECLARE SECTION;
	char        vRunName[20+1];
	char        vSmName[20+1];
	char		vcontract_no[23];
	char        vst[1024];
	char		vregion_char[100+1];
	char        vregion_code[3];
	char        vdistrict_code[3];
	char        vrun_code[2];
	char        vrun_time[9];
	char        vbelong_code[8];
	char        vpay_code[2];
	char		vsm_code[2+1];
	double      vprepay_fee=0;
	double		vunbill=0;
	char        vunbill_fee[MAXMONEYLEN+1];
	char        vunbill_month_fee[MAXMONEYLEN+1];
	char        vid_no[23];
	char        vallow_flag[2];
	char        vreturn_code[5];
	char        vreturn_msg[61];
	char        vowe_flag[2];
	char		phoneNo[15+1];
	int         vNum=0;
	int         vcurtime=0;
	int         vpre_month=0;
	int         vpredel_month=0;
	char        vorg_code[10];
	char		vContractNo[22+1];
	double		vowe_fee;
	short int   in=0;

	char 		sOpCode[4 + 1];
	char 		sWorkNo[6 + 1];
	char 		sCheckType[1 + 1]; 

	int			i=0;
	int			vret=0,iret=0,ret = 0;
	/*��֯�ṹ������� editer =wanfh  date = 20090509*/
	char vOrgId[10+1];
	char vGroupId[10+1];
	char Vflag[1+1];
	int  vCount=0;
	EXEC SQL END DECLARE SECTION;

	init(vcontract_no);
	init(vContractNo);
	init(vid_no);
	init(vregion_code);
	init(vdistrict_code);
	init(vrun_code);
	init(vbelong_code);
	init(vrun_time);
	init(vpay_code);
	init(vsm_code);        
	init(vrun_time);        
	init(vunbill_fee);
	init(vunbill_month_fee);
	init(vorg_code);
	init(sCheckType);
	init(vServiceName);
	init(vOrgId);
	init(vGroupId);
	init(Vflag);
	memset(vregion_char, 0, sizeof(vregion_char));
	vprepay_fee=0;

	strcpy(sOpCode, op_code);
	strcpy(sWorkNo, work_no);
	strcpy(phoneNo, phone_no);
	Trim(phoneNo);
	phoneNo[11]= '\0';

	strcpy(vServiceName,"fCheckChgAllow");

	/**ȡӪҵԱ�Ļ�������**/
	EXEC SQL	SELECT org_code, region_char
				  INTO :vorg_code, :vregion_char
	  			  FROM dLoginMsg 
	  			 WHERE login_no=:sWorkNo;
	if (SQLCODE != 0)
	{
		sprintf(oErrMsg, "����[%s]��Ϣ������!", sWorkNo);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001200",oErrMsg);
#endif

		return 1200;
	}
	/*--��֯�������������ֶ�edit by wanfh at 14/05/2009--begin*/
       ret = sGetLoginOrgid(sWorkNo,vOrgId);
       if(ret <0)
         	{
		         strcpy(oErrMsg,"��ѯ����org_idʧ��!");
		         pubLog_DBErr(_FFL_, "", "", "%s", oErrMsg);
		         return 1201 ;
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by wanfh at 14/05/2009---end*/
	/**ȡ�û�id **//*��֯�ṹ���ڸ���   ��ȡgroup_id editer=wanfh  date=20090508*/
	sprintf(vst,    "select to_char(id_no),substr(run_code,2,1),\
					to_char(contract_no), sm_code,\
					to_char(run_time,'yyyymmdd'),substr(belong_code,1,2),\
					substr(belong_code,3,2),belong_code,nvl(group_no,'') \
					from dCustMsg \
					where phone_no=:v1 \
					and substr(run_code,2,1)<'a'");

	exec sql execute 
	begin
		execute immediate :vst into :vid_no indicator :in,:vrun_code indicator :in,
		:vContractNo indicator :in,:vsm_code indicator :in,
		:vrun_time indicator :in,:vregion_code indicator :in,
		:vdistrict_code indicator :in,:vbelong_code,:vGroupId indicator :in USING :phone_no;  
	end;
	end-exec;
	if(vid_no==NULL)
	{
	    sprintf(oErrMsg, "�û�[%s]��Ϣ������!", phone_no);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001201",oErrMsg);
#endif
		return 1201;
	}

    Trim(vGroupId);
	EXEC SQL    select count(*)
                 into :vret 
                 from dConMsg a, dConUserMsg b
                where a.contract_no = b.contract_no
                  and b.serial_no = 0
                  and b.id_no = :vid_no
                  and a.pay_code in('4','5','9');
	if (SQLCODE != 0)
	{
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001009","������ʾδ����");
#endif
		return 1009;
	}
	if( vret > 0 )
	{
		vpay_code[0]='4';
	}
	else
	{
		EXEC SQL	select pay_code
					into :vpay_code
				  	from dConMsg 
				 	where contract_no  = to_number(:vContractNo);
	}

	/*�жϺ���״̬�Ƿ���������    */
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
	  			  FROM cChgRun
	  			 WHERE op_code=:sOpCode and run_code=:vrun_code 
	  			   AND region_code=:vregion_code;
	printf("--------sqlcode=[%d],sOpCode=[%s],vallow_flag=[%s],vregion_code=[%s]\n",SQLCODE,sOpCode,vallow_flag,vregion_code);
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    init(vRunName);
	    EXEC SQL select run_name into :vRunName from sRunCode
	    where  region_code = :vregion_code 
	    and    run_code = :vrun_code;
	    Trim(vRunName);
	    printf("--------552sqlcode=[%d],vrun_code=[%s]\n",SQLCODE,vrun_code);
	    sprintf(oErrMsg, "�û�[%s]��ǰ״̬Ϊ[%s],��ֹҵ����!", phone_no,vRunName);
#ifdef _DEBUG_
		printf("--------555sqlcode=[%d],vRunName=[%s]\n",SQLCODE,vRunName);
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001202",oErrMsg);
#endif
		return 1202;
	}

	/*�жϺ���״̬�Ƿ��������������*/	
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508,majha�޸�BUG*/
	/*��֯�ṹ����if (strncmp(vorg_code,vbelong_code,2)!=0 && strncmp(vorg_code,"00",2)!=0)��֯�ṹ����*/
	EXEC SQL select count(*) 
			  into  :vCount
	          from  dchngroupmsg a,dchngroupinfo b
             where  a.group_id =b.parent_group_id
               and  b.group_id=:vOrgId
               and  a.group_id='2';
	if(vCount==0)
	{
		int ret=0;
		ret =sCheckGroupId(vGroupId,vOrgId,"11",Vflag,oErrMsg);
		if (ret != 0)
		{
			sprintf(oErrMsg,"����sCheckGroupId ��������[%s]",vOrgId);
			PUBLOG_DBDEBUG(vServiceName);
			pubLog_Debug(_FFL_,vServiceName,"001298",oErrMsg);
			return 1298;
		}
		Trim(Vflag);
		if(strcmp(Vflag,"Y") != 0 )
		{
			EXEC SQL	SELECT allow_flag
						  INTO :vallow_flag
						  FROM cChgReg
						 WHERE op_code=:sOpCode 
						   AND region_code= :vregion_code;
			if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
			{
			    sprintf(oErrMsg, "��ҵ��������������!");
				PUBLOG_DBDEBUG(vServiceName);
				pubLog_DBErr(_FFL_,vServiceName,"001215",oErrMsg);
				return 1215;
			}
				
			/*���ܿ����������*/
			if (vregion_char[0] != 'Y')
			{
			    sprintf(oErrMsg, "����[%s]û�п��������Ȩ��!", sWorkNo);
			    PUBLOG_DBDEBUG(vServiceName);
				pubLog_DBErr(_FFL_,vServiceName,"001216",oErrMsg);
				return 1216;
			}
		}
	}

	/*�жϺ���״̬�Ƿ���������������*/	
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508*/
	/*��֯�ṹif (strncmp(vorg_code,vbelong_code,4)!=0)��֯�ṹ���ڸ�*/
	init(Vflag);
	if(sCheckGroupId(vGroupId,vOrgId,"12",Vflag,oErrMsg) != 0)
	{
		pubLog_Debug(_FFL_,"","","����sCheckGroupId ��������");
		pubLog_DBErr(_FFL_,"","001299",oErrMsg);
		return 1299;
	}
	Trim(Vflag);
	if(strcmp(Vflag,"Y") != 0)
	{
		/*�����ӪҵԱ������ͬ*/
		EXEC SQL	SELECT allow_flag
					  INTO :vallow_flag
					  FROM cChgDis
	  				 WHERE op_code=:sOpCode 
	  				   AND region_code=:vregion_code;
		if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
		{
		    sprintf(oErrMsg, "��ҵ���������������!");
			PUBLOG_DBDEBUG(vServiceName);
			pubLog_DBErr(_FFL_,vServiceName,"001203",oErrMsg);
			return 1203;
		}
	}

	/*�ж�ҵ�������Ƿ���������*/
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
				  FROM cChgSm
				 WHERE region_code=:vregion_code 
	  			   AND op_code=:sOpCode
				   AND sm_code=:vsm_code;
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    init(vSmName);
	    EXEC SQL select sm_name into :vSmName from sSmCode 
	    where region_code = :vregion_code
	    and   sm_code=:vsm_code;
	    Trim(vSmName);
	    sprintf(oErrMsg, "�û���ǰƷ��[%s]�����������ҵ��!", vSmName);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001204",oErrMsg);
#endif
		return 1204;
	}
	
	/*�ж��Ƿ�����Ƿ������    */
	strcpy(vallow_flag,"0");
	strcpy(vowe_flag,"0");        
	EXEC SQL	SELECT allow_flag,owe_flag
				  INTO :vallow_flag indicator :in,  :vowe_flag indicator :in 
				  FROM cChgOwe
				 WHERE op_code=:sOpCode 
				   AND pay_code=decode(:vpay_code,'4','4','5','5','0')
				   AND region_code=:vregion_code;
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,vServiceName,"vallow_flag=[%s]","vowe_flag=[%s]","user_owe.accountNo=[%s]","user_owe.accountOwe=[%lf]",vallow_flag, vowe_flag, user_owe.accountNo, user_owe.accountOwe); /* ��������Ĵ�ӡ*/
#endif

/*Ԥ��(1216)�򸶷Ѽƻ����(1212)Ƿ��ʱ��������*/
/*
	if( strncmp(sOpCode ,"1216",4)==0 || strncmp(sOpCode ,"1212",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
				return 1007;
			}
	}
*/
	if( strncmp(sOpCode ,"1216",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
			    sprintf(oErrMsg, "�û������ʻ�[%s]����Ƿ��,����������ҵ��!", user_owe.accountNo);
			    pubLog_DBErr(_FFL_,vServiceName,"001007",oErrMsg);
				return 1007;
			}
	}

	if (atoi(vallow_flag)==1 && atoi(vowe_flag)==0)
	{	
		/*��Ƿ�Ѵ������ʱ��������*/	
		if ( user_owe.totalOwe - user_owe.totalPrepay > 0.005)
		{
		    sprintf(oErrMsg, "�û��ʻ�����Ƿ��,����������ҵ��!");
		    pubLog_DBErr(_FFL_,vServiceName,"001008",oErrMsg);
			return 1008;
		}
	}
	
/****************************
@@@��ѯ�û��Ƿ��ں�������,���û��������ֻ��Ƿ�Ƿ��@@@
	strcpy(sCheckType,"A");
	if ( ( iret = fChkBlackOwe(phoneNo,sCheckType)) != 0 )
		return iret;	

@@@@��ѯʹ�ø��û����֤�����������û��Ƿ�Ƿ��,����û���һ���ʻ�Ϊ�����ʻ�,����Ϊ�����û���Ƿ��@@@
	iret = fGetIccOwe(phoneNo);
	if( iret > 0 )
		return 1017;
	else if ( iret < 0 )
		return 1018;
*****************************/

    /* lixg Add������ҵ������������ */   
    if(fCheckFuncAllow(phoneNo, sWorkNo, sOpCode, oErrMsg, &fe) != 0)
    {
        init(tmpRetCode);
        getValueByParamCode(&fe, 1001, oErrMsg);
        getValueByParamCode(&fe, 1000, tmpRetCode);
        pubLog_DBErr(_FFL_,vServiceName,tmpRetCode,oErrMsg);
        return atoi(tmpRetCode);        
    }
    /*majha add at 20121215 ����ҵ����������*/
    if(fBizOprLimitCtl(phoneNo,vid_no,sOpCode,oErrMsg)!=0)
    {
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001203",oErrMsg);
		printf("\n oErrMsg=%s\n",oErrMsg);
		return 1246;
	}
    
	return 0;
}

/************************************************************************
��������: fNewCheckChgAllow()
����ʱ��: 2011/03/05
������Ա:liwd
��������:�ж��Ƿ�ĳ�����Ƿ��������
�������:�ƶ�����,��������,��������,
�������: oErrMsg
�� �� ֵ: 0 ������ȷ����, ��������!
ע�⣺�ڸú��������������ҵ�����Ƶ����ù��ܣ���������Ƶ���������
      ��ҵ�����ģ�Ϳ�����
      servicetype ��Ϊ��չʹ�ã������ñ���SSTILLOWE��Ӧ
      NeedFee     ����ҵ��������С���
��ע���޸�fCheckChgAllow������ȥ��Ƿ�ѹ����жϲ��֣�Ƿ�ѹ��򲿷��Ƶ�BOSS�࣬���ú���QueryRemainFee
************************************************************************/
int fNewCheckChgAllow(const char* phone_no, const char* op_code, const char* work_no,const char* servicetype,const char* NeedFee, char *oErrMsg)
{
    CFUNCCONDENTRY fe;
    char tmpRetCode[6+1];
    char vServiceName[20+1];
    char oAllowFlag[1+1];
    
	EXEC SQL BEGIN DECLARE SECTION;
	char        vRunName[20+1];
	char        vSmName[20+1];
	char		vcontract_no[23];
	char        vst[1024];
	char		vregion_char[100+1];
	char        vregion_code[3];
	char        vdistrict_code[3];
	char        vrun_code[2];
	char        vrun_time[9];
	char        vbelong_code[8];
	char        vpay_code[2];
	char		vsm_code[2+1];
	double      vprepay_fee=0;
	double		vunbill=0;
	char        vunbill_fee[MAXMONEYLEN+1];
	char        vunbill_month_fee[MAXMONEYLEN+1];
	char        vid_no[23];
	char        vallow_flag[2];
	char        vreturn_code[5];
	char        vreturn_msg[61];
	char        vowe_flag[2];
	char		phoneNo[15+1];
	int         vNum=0;
	int         vcurtime=0;
	int         vpre_month=0;
	int         vpredel_month=0;
	char        vorg_code[10];
	char		vContractNo[22+1];
	double		vowe_fee;
	short int   in=0;

	char 		sOpCode[4 + 1];
	char 		sWorkNo[6 + 1];
	char 		sCheckType[1 + 1]; 

	int			i=0;
	int			vret=0,iret=0,ret = 0;
	/*��֯�ṹ������� editer =wanfh  date = 20090509*/
	char vOrgId[10+1];
	char vGroupId[10+1];
	char Vflag[1+1];
	int  vCount=0;
	EXEC SQL END DECLARE SECTION;

	init(vcontract_no);
	init(vContractNo);
	init(vid_no);
	init(vregion_code);
	init(vdistrict_code);
	init(vrun_code);
	init(vbelong_code);
	init(vrun_time);
	init(vpay_code);
	init(vsm_code);        
	init(vrun_time);        
	init(vunbill_fee);
	init(vunbill_month_fee);
	init(vorg_code);
	init(sCheckType);
	init(tmpRetCode);
	init(vServiceName);
	init(oAllowFlag);
	init(vOrgId);
	init(vGroupId);
	init(Vflag);
	memset(vregion_char, 0, sizeof(vregion_char));
	vprepay_fee=0;

	strcpy(sOpCode, op_code);
	strcpy(sWorkNo, work_no);
	strcpy(phoneNo, phone_no);
	Trim(phoneNo);
	phoneNo[11]= '\0';

	strcpy(vServiceName,"fNewCheckChgAllow");

	/**ȡӪҵԱ�Ļ�������**/
	EXEC SQL	SELECT org_code, region_char
				  INTO :vorg_code, :vregion_char
	  			  FROM dLoginMsg 
	  			 WHERE login_no=:sWorkNo;
	if (SQLCODE != 0)
	{
		sprintf(oErrMsg, "����[%s]��Ϣ������!", sWorkNo);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001200",oErrMsg);
#endif

		return 1200;
	}
	/*--��֯�������������ֶ�edit by wanfh at 14/05/2009--begin*/
       ret = sGetLoginOrgid(sWorkNo,vOrgId);
       if(ret <0)
         	{
		         strcpy(oErrMsg,"��ѯ����org_idʧ��!");
		         pubLog_DBErr(_FFL_, "", "", "%s", oErrMsg);
		         return 1201 ;
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by wanfh at 14/05/2009---end*/
	/**ȡ�û�id **//*��֯�ṹ���ڸ���   ��ȡgroup_id editer=wanfh  date=20090508*/
	sprintf(vst,    "select to_char(id_no),substr(run_code,2,1),\
					to_char(contract_no), sm_code,\
					to_char(run_time,'yyyymmdd'),substr(belong_code,1,2),\
					substr(belong_code,3,2),belong_code,nvl(group_no,'') \
					from dCustMsg \
					where phone_no=:v1 \
					and substr(run_code,2,1)<'a'");

	exec sql execute 
	begin
		execute immediate :vst into :vid_no indicator :in,:vrun_code indicator :in,
		:vContractNo indicator :in,:vsm_code indicator :in,
		:vrun_time indicator :in,:vregion_code indicator :in,
		:vdistrict_code indicator :in,:vbelong_code,:vGroupId indicator :in USING :phone_no;  
	end;
	end-exec;
	if(vid_no==NULL)
	{
	    sprintf(oErrMsg, "�û�[%s]��Ϣ������!", phone_no);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001201",oErrMsg);
#endif
		return 1201;
	}
	Trim(vid_no);

    Trim(vGroupId);
	EXEC SQL    select count(*)
                 into :vret 
                 from dConMsg a, dConUserMsg b
                where a.contract_no = b.contract_no
                  and b.serial_no = 0
                  and b.id_no = :vid_no
                  and a.pay_code in('4','5','9');
	if (SQLCODE != 0)
	{
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001009","������ʾδ����");
#endif
		return 1009;
	}
	if( vret > 0 )
	{
		vpay_code[0]='4';
	}
	else
	{
		EXEC SQL	select pay_code
					into :vpay_code
				  	from dConMsg 
				 	where contract_no  = to_number(:vContractNo);
	}

	/*�жϺ���״̬�Ƿ���������    */
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
	  			  FROM cChgRun
	  			 WHERE op_code=:sOpCode and run_code=:vrun_code 
	  			   AND region_code=:vregion_code;
	printf("--------sqlcode=[%d],sOpCode=[%s],vallow_flag=[%s],vregion_code=[%s]\n",SQLCODE,sOpCode,vallow_flag,vregion_code);
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    init(vRunName);
	    EXEC SQL select run_name into :vRunName from sRunCode
	    where  region_code = :vregion_code 
	    and    run_code = :vrun_code;
	    Trim(vRunName);
	    printf("--------552sqlcode=[%d],vrun_code=[%s]\n",SQLCODE,vrun_code);
	    sprintf(oErrMsg, "�û�[%s]��ǰ״̬Ϊ[%s],��ֹҵ����!", phone_no,vRunName);
#ifdef _DEBUG_
		printf("--------555sqlcode=[%d],vRunName=[%s]\n",SQLCODE,vRunName);
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001202",oErrMsg);
#endif
		return 1202;
	}

	/*�жϺ���״̬�Ƿ��������������*/	
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508,majha�޸�BUG*/
	/*��֯�ṹ����if (strncmp(vorg_code,vbelong_code,2)!=0 && strncmp(vorg_code,"00",2)!=0)��֯�ṹ����*/
	EXEC SQL select count(*) 
			  into  :vCount
	          from  dchngroupmsg a,dchngroupinfo b
             where  a.group_id =b.parent_group_id
               and  b.group_id=:vOrgId
               and  a.group_id='2';
	if(vCount==0)
	{
		int ret=0;
		ret =sCheckGroupId(vGroupId,vOrgId,"11",Vflag,oErrMsg);
		if (ret != 0)
		{
			sprintf(oErrMsg,"����sCheckGroupId ��������[%s]",vOrgId);
			PUBLOG_DBDEBUG(vServiceName);
			pubLog_Debug(_FFL_,vServiceName,"001298",oErrMsg);
			return 1298;
		}
		Trim(Vflag);
		if(strcmp(Vflag,"Y") != 0 )
		{
			EXEC SQL	SELECT allow_flag
						  INTO :vallow_flag
						  FROM cChgReg
						 WHERE op_code=:sOpCode 
						   AND region_code= :vregion_code;
			if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
			{
			    sprintf(oErrMsg, "��ҵ��������������!");
				PUBLOG_DBDEBUG(vServiceName);
				pubLog_DBErr(_FFL_,vServiceName,"001215",oErrMsg);
				return 1215;
			}
				
			/*���ܿ����������*/
			if (vregion_char[0] != 'Y')
			{
			    sprintf(oErrMsg, "����[%s]û�п��������Ȩ��!", sWorkNo);
			    PUBLOG_DBDEBUG(vServiceName);
				pubLog_DBErr(_FFL_,vServiceName,"001216",oErrMsg);
				return 1216;
			}
		}
	}

	/*�жϺ���״̬�Ƿ���������������*/	
	strcpy(vallow_flag,"0");
	/*��֯�ṹ���ڸ���  editer=wanfh  date=20090508*/
	/*��֯�ṹif (strncmp(vorg_code,vbelong_code,4)!=0)��֯�ṹ���ڸ�*/
	init(Vflag);
	if(sCheckGroupId(vGroupId,vOrgId,"12",Vflag,oErrMsg) != 0)
	{
		pubLog_Debug(_FFL_,"","","����sCheckGroupId ��������");
		pubLog_DBErr(_FFL_,"","001299",oErrMsg);
		return 1299;
	}
	Trim(Vflag);
	if(strcmp(Vflag,"Y") != 0)
	{
		/*�����ӪҵԱ������ͬ*/
		EXEC SQL	SELECT allow_flag
					  INTO :vallow_flag
					  FROM cChgDis
	  				 WHERE op_code=:sOpCode 
	  				   AND region_code=:vregion_code;
		if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
		{
		    sprintf(oErrMsg, "��ҵ���������������!");
			PUBLOG_DBDEBUG(vServiceName);
			pubLog_DBErr(_FFL_,vServiceName,"001203",oErrMsg);
			return 1203;
		}
	}

	/*�ж�ҵ�������Ƿ���������*/
	strcpy(vallow_flag,"0");
	EXEC SQL	SELECT allow_flag
				  INTO :vallow_flag
				  FROM cChgSm
				 WHERE region_code=:vregion_code 
	  			   AND op_code=:sOpCode
				   AND sm_code=:vsm_code;
	if ((SQLCODE != 0 && SQLCODE != 1403) || atoi(vallow_flag)==1)
	{
	    init(vSmName);
	    EXEC SQL select sm_name into :vSmName from sSmCode 
	    where region_code = :vregion_code
	    and   sm_code=:vsm_code;
	    Trim(vSmName);
	    sprintf(oErrMsg, "�û���ǰƷ��[%s]�����������ҵ��!", vSmName);
#ifdef _DEBUG_
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001204",oErrMsg);
#endif
		return 1204;
	}
	/*�ж��Ƿ�����Ƿ������    */
	strcpy(vallow_flag,"0");
	strcpy(vowe_flag,"0");        
	EXEC SQL	SELECT allow_flag,owe_flag
				  INTO :vallow_flag indicator :in,  :vowe_flag indicator :in 
				  FROM cChgOwe
				 WHERE op_code=:sOpCode 
				   AND pay_code=decode(:vpay_code,'4','4','5','5','0')
				   AND region_code=:vregion_code;
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,vServiceName,"","vallow_flag=[%s]vowe_flag=[%s]",vallow_flag, vowe_flag); /* ��������Ĵ�ӡ*/
#endif

/*Ԥ��(1216)�򸶷Ѽƻ����(1212)Ƿ��ʱ��������*/
/*
	if( strncmp(sOpCode ,"1216",4)==0 || strncmp(sOpCode ,"1212",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
				return 1007;
			}
	}

	if( strncmp(sOpCode ,"1216",4)==0 )
	{
		if (user_owe.accountNo[0] != '\0')
			{
			    sprintf(oErrMsg, "�û������ʻ�[%s]����Ƿ��,����������ҵ��!", user_owe.accountNo);
			    pubLog_DBErr(_FFL_,vServiceName,"001007",oErrMsg);
				return 1007;
			}
	}
*/
	if (atoi(vallow_flag)==1 && atoi(vowe_flag)==0)
	{	
		if(QueryRemainFee("bs_QryRemainFee",phoneNo,sOpCode,servicetype,NeedFee,tmpRetCode,oErrMsg,oAllowFlag)!=0)
		{
		    sprintf(oErrMsg, "��ѯ����Ƿ���������ҵ��ʧ��!");
		    pubLog_DBErr(_FFL_,vServiceName,tmpRetCode,oErrMsg);
			return 1008;
		}
	#ifdef _DEBUG_
	pubLog_Debug(_FFL_,vServiceName,"","oAllowFlag=[%s]",oAllowFlag); /* ��������Ĵ�ӡ*/
	#endif
		if(strcmp(oAllowFlag,"0")==0)
		{
		    sprintf(oErrMsg, "�û��ʻ�����Ƿ��,����������ҵ��!");
		    pubLog_DBErr(_FFL_,vServiceName,"001008",oErrMsg);
			return 1008;
		}
	}


    /* lixg Add������ҵ������������ */   
    if(fCheckFuncAllow(phoneNo, sWorkNo, sOpCode, oErrMsg, &fe) != 0)
    {
        init(tmpRetCode);
        getValueByParamCode(&fe, 1001, oErrMsg);
        getValueByParamCode(&fe, 1000, tmpRetCode);
        pubLog_DBErr(_FFL_,vServiceName,tmpRetCode,oErrMsg);
        return atoi(tmpRetCode);        
    }
	
	/*majha add at 20121215 ����ҵ����������*/
    if(fBizOprLimitCtl(phoneNo,vid_no,sOpCode,oErrMsg)!=0)
    {
		PUBLOG_DBDEBUG(vServiceName);
		pubLog_DBErr(_FFL_,vServiceName,"001203",oErrMsg);
		return 1246;
	}
    
	return 0;
}

/************************************************************************
��������:fGetConOwe
����ʱ��:2003/10/19
������Ա:liwd
��������:�����ʻ�IDȡ�ʻ���Ƿ��,Ԥ���
�������:�ʺ�
�������:Ƿ��
Ԥ���
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fGetConOwe(const char *contract_no,double *owe_fee,double *prepay_fee)
{
	EXEC SQL BEGIN DECLARE SECTION;
	/***�������õ���������****/		
	char	vcontract_no[23];

	double	vowe_fee=0;
	double	vprepay_fee;

	int		vret=0;
	EXEC SQL END DECLARE SECTION;

	memset(vcontract_no,0,sizeof(vcontract_no));
	if(contract_no!=NULL)
	{
		strcpy(vcontract_no,contract_no);
	}
	
	EXEC SQL	SELECT nvl(owe_fee,0),nvl(prepay_fee,0)
				  INTO :vowe_fee,:vprepay_fee
				  FROM dConMsg
				 WHERE contract_no=to_number(:vcontract_no);
	if(vret!=0)
	{	
		pubLog_Debug(_FFL_,"","%d","select dconmsg ����",vret);
		return vret;
	}
	if(owe_fee != NULL)
	{
		*owe_fee = vowe_fee;
	}

	if(prepay_fee != NULL)
	{
		*prepay_fee = vprepay_fee;
	}
	return 0;
}
/************************************************************************
��������:fGetUserOwe
����ʱ��:2003/10/19
������Ա:liwd
��������:�����û�IDȡ�����ʻ���Ƿ����Ϣ
�������:�û�ID
�������:�û�Ƿ����Ϣ,���а�����������
�û���Ƿ��
�û���Ԥ��
��һ���ʺ�
��һ���ʺ�Ƿ��
�� �� ֵ:0������ȷ����,��������
************************************************************************/

int fGetUserOwe(const char *id_no, tUserOwe *user_owe)
{
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 begin --*/
	/*EXEC SQL BEGIN DECLARE SECTION;
	char	vid_no[22+1];
	char	vcontract_no[22+1];
	char	vst[512];
	double	vowe_fee=0;
	double	vprepay_fee=0;
	int		vtmp_num=0;
	int     ids=0;
	int		vret=0;
	char    phone_no[15+1];
	double  v_unbill=0;
	long    contract_no=0;
	char	billStatus[1+1];
	EXEC SQL END DECLARE SECTION;        

	memset(vid_no,0,sizeof(vid_no));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vst,0,sizeof(vst));
	memset(billStatus,0,sizeof(billStatus));
	init(phone_no);

	if(id_no!=NULL)
	{
		strcpy(vid_no,id_no);
	}
	
	EXEC SQL	select ids,contract_no,phone_no into :ids,:contract_no,:phone_no
				  from dCustMsg 
				 where id_no=:vid_no;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","ȡ�û���Ϣ����");
		return -1;
	}
	
	EXEC SQL	select bill_status into :billStatus
				  from cbillstatus;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","ȡcbillstatus����");
		return -1;
	}

	memset(user_owe, 0, sizeof(tUserOwe));
	
	if(strcmp(billStatus,"0") == 0)
	{
		if(ids==1)
		{
			vret=Query_Con_Pre(contract_no,"*",&vprepay_fee);
			if(vret!=0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Pre��Ϣ����");
				return -1;
			}
			
			vret=Query_Con_Owe(contract_no,&vowe_fee);
			if(vret!=0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Owe��Ϣ����");
				return -1;
			}
			
			vret=PublicGetUnbillChg(contract_no,atol(vid_no),0,phone_no,4,&v_unbill);
			if(vret!=0)
			{
				pubLog_Debug(_FFL_,"","ret=%d","ȡδ���ʻ��ѳ���",vret);
				return -1;
			}
		
			user_owe->totalPrepay = vprepay_fee;
			user_owe->totalOwe = vowe_fee+v_unbill; 
			user_owe->unbillFee =v_unbill ;
			if (vowe_fee+v_unbill>0.005)
			{
				sprintf(user_owe->accountNo,"%ld", contract_no);
				if(vowe_fee+v_unbill-vprepay_fee>0.005)
				{
					sprintf(vcontract_no,"%ld",contract_no);
					user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
				}
			}
		}
		else
		{	    
			vtmp_num = 0;
			sprintf(vst,"select contract_no from dConUserMsg\
						where id_no=to_number(:v1)\
						and serial_no=0 ");
			EXEC SQL PREPARE stmt_sGetUserOwe1 FROM :vst;
			EXEC SQL DECLARE c_sGetUserOwe1 CURSOR FOR stmt_sGetUserOwe1;
			EXEC SQL OPEN c_sGetUserOwe1 using :vid_no;
			for(;;)
			{
				memset(vcontract_no,0,sizeof(vcontract_no));
				vowe_fee=0;
				v_unbill=0;
				contract_no=0;
				EXEC SQL FETCH c_sGetUserOwe1 INTO :contract_no;
				if(SQLCODE==NOTFOUND || SQLCODE!=0) break;
	
				vret=GetConUserOweChg(contract_no,atol(vid_no),&vowe_fee);
				if(vret!=0)
				{
					pubLog_Debug(_FFL_,"","-1","ȡǷ�ѳ���");
					EXEC SQL CLOSE c_sGetUserOwe1;
					return -1;
				}
				vret=PublicGetUnbillChg(contract_no,atol(vid_no),0,phone_no,4,&v_unbill);
				if(vret!=0)
				{
					pubLog_Debug(_FFL_,"","-1","ȡδ���ʻ��ѳ���");
					EXEC SQL CLOSE c_sGetUserOwe1;
					return -1;
				}         
				user_owe->unbillFee =user_owe->unbillFee +v_unbill ;    	
				vtmp_num++;
	
				vprepay_fee = 0.00;
				vret=Query_Con_Pre(contract_no,"*",&vprepay_fee);
				if(vret!=0)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Pre��Ϣ����");
					return -1;
				}
				
				user_owe->totalPrepay += vprepay_fee;
				user_owe->totalOwe += vowe_fee+v_unbill; 
				user_owe->unbillFee += v_unbill ;
				if (vowe_fee+v_unbill>0.005 && user_owe->accountOwe < 0.005)
				{
					sprintf(user_owe->accountNo,"%ld", contract_no);
					if(vowe_fee+v_unbill-vprepay_fee>0.005)
					{
						sprintf(vcontract_no,"%ld",contract_no);
						user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
					}
				}
			}
			EXEC SQL CLOSE c_sGetUserOwe1;
		}
	}
	else
	{
		if(ids==1)
		{
			vret=Query_Con_Pre(contract_no,"*",&vprepay_fee);
			if(vret!=0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Pre��Ϣ����");
				return -1;
			}
			
			vret=Query_Con_Owe(contract_no,&vowe_fee);
			if(vret!=0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Owe��Ϣ����");
				return -1;
			}
			
			v_unbill=0;
		
			user_owe->totalPrepay = vprepay_fee;
			user_owe->totalOwe = vowe_fee+v_unbill; 
			user_owe->unbillFee =v_unbill ;
			if (vowe_fee+v_unbill>0.005)
			{
				sprintf(user_owe->accountNo,"%ld", contract_no);
				if(vowe_fee+v_unbill-vprepay_fee>0.005)
				{
					sprintf(vcontract_no,"%ld",contract_no);
					user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
				}
			}
		}
		else
		{	    
			vtmp_num = 0;
			sprintf(vst,"select contract_no from dConUserMsg where id_no=to_number(:v1) and serial_no=0 ");
			EXEC SQL PREPARE stmt_sGetUserOwe3 FROM :vst;
			EXEC SQL DECLARE c_sGetUserOwe3 CURSOR FOR stmt_sGetUserOwe3;
			EXEC SQL OPEN c_sGetUserOwe3 using :vid_no;
			for(;;)
			{
				memset(vcontract_no,0,sizeof(vcontract_no));
				vowe_fee=0;
				v_unbill=0;
				contract_no=0;
				EXEC SQL FETCH c_sGetUserOwe3 INTO :contract_no;
				if(SQLCODE==NOTFOUND || SQLCODE!=0) break;
	
				vret=GetConUserOweChg(contract_no,atol(vid_no),&vowe_fee);
				if(vret!=0)
				{
					pubLog_Debug(_FFL_,"","-1","ȡǷ�ѳ���");
					EXEC SQL CLOSE c_sGetUserOwe3;
					return -1;
				}
				v_unbill=0;      
				user_owe->unbillFee =user_owe->unbillFee +v_unbill ;    	
				vtmp_num++;
	
				vprepay_fee = 0.00;
				vret=Query_Con_Pre(contract_no,"*",&vprepay_fee);
				if(vret!=0)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","-1","ȡQuery_Con_Pre��Ϣ����");
					return -1;
				}
				
				user_owe->totalPrepay += vprepay_fee;
				user_owe->totalOwe += vowe_fee+v_unbill; 
				user_owe->unbillFee += v_unbill ;
				if (vowe_fee+v_unbill>0.005 && user_owe->accountOwe < 0.005)
				{
					sprintf(user_owe->accountNo,"%ld", contract_no);
					if(vowe_fee+v_unbill-vprepay_fee>0.005)
					{
						sprintf(vcontract_no,"%ld",contract_no);
						user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
					}
				}
			}
			EXEC SQL CLOSE c_sGetUserOwe3;
		}
	}*/
	
	EXEC SQL BEGIN DECLARE SECTION;
	FBFR32    *sendbuf, *rcvbuf;
    FLDLEN32  sendlen,rcvlen;
    int 	  sendNum=0;
    int 	  recvNum=0;
    int	      v_ret = 0;
    char 	  buffer1[256];
    char	  buffer2[256];
	char	  vIdNo[14 + 1];
	char 	  vContractNo[14 + 1];
	char	  vTotalPrepay[14 + 1];
	char	  vTotalOwe[14 + 1];
	char	  vUnbillFee[14 + 1];
	char	  vAccountOwe[14 + 1];
	char	  vRunName[30+1];
	EXEC SQL END DECLARE SECTION;
	
	init(vIdNo);
	init(vContractNo);
	init(vTotalPrepay);
	init(vTotalOwe);
	init(vUnbillFee);
	init(vAccountOwe);
	init(vRunName);
	
	strcpy(vIdNo, id_no);
	
	pubLog_Debug(_FFL_, "", "", "����BOSS����bs_getUserOwe��ʼ!");
	
	/*�����������*/
	sendNum = 1;
			
	/*�����������*/
	recvNum = 8;
	
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","��ʼ�����������ʧ��");
		return -1;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"8",(FLDLEN32)0);							
			
	Fchg32(sendbuf,GPARM32_0,0,vIdNo,(FLDLEN32)0);
			   
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);
			
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","��ʼ�����������ʧ��");
		return -1;
	}

	long reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	v_ret = 0;
	v_ret = fPubCallSrv(CALLSERV_TP,"bs_getUserOwe",sendbuf,&rcvbuf);
	if(v_ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","����BOSS����bs_getUserOwe����");
		pubLog_Debug(_FFL_, "", "", "v_ret=[%d]!",v_ret);
		return -77;
	}
	
	memset(buffer1, '\0', sizeof(buffer1));
	memset(buffer2, '\0', sizeof(buffer2));

	Fget32(rcvbuf,GPARM32_0,0,buffer1,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_1,0,buffer2,(FLDLEN32)0);
  	
  	Trim(buffer1);
	Trim(buffer2);					
	printf("buffer1[%s]\n",buffer1);
	printf("buffer2[%s]\n",buffer2);
	
	if(strcmp(buffer1,"000000")!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"",buffer1,buffer2);
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		return -1;
	}
	
	
	Fget32(rcvbuf,GPARM32_2,0,vTotalOwe,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_3,0,vTotalPrepay,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_4,0,vUnbillFee,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_5,0,vContractNo,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_6,0,vAccountOwe,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_7,0,vRunName,(FLDLEN32)0);
	
	Trim(vTotalOwe);
	Trim(vTotalPrepay);
	Trim(vUnbillFee);
	Trim(vContractNo);
	Trim(vAccountOwe);
	Trim(vRunName);
	
	printf("vTotalOwe:[%s]\n",vTotalOwe);
	printf("vTotalPrepay:[%s]\n",vTotalPrepay);
	printf("vUnbillFee:[%s]\n",vUnbillFee);
	printf("vContractNo:[%s]\n",vContractNo);
	printf("vAccountOwe:[%s]\n",vAccountOwe);
	printf("vRunName:[%s]\n",vRunName);
	
	user_owe->totalOwe = atof(vTotalOwe);
	user_owe->totalPrepay = atof(vTotalPrepay);
	user_owe->unbillFee = atof(vUnbillFee);
	strcpy(user_owe->accountNo, vContractNo); 
	user_owe->accountOwe = atof(vAccountOwe);
	strcpy(user_owe->creditname , vRunName);
	

    
	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 end --*/
	return 0;
}
/************************************************************************
��������:fGetUserBaseInfo
����ʱ��:2003/10/19
������Ա:liwd
��������:�����ƶ�����ȡ�û��Ļ�����Ϣ
�������:�ƶ�����
�������:�û�������Ϣ
�û�id      
�ͻ�id      
belong_code
ҵ�����ʹ���
ҵ����������
�ͻ�����    
�û�����    
״̬����    
״̬����    
�ȼ�����    
�ȼ�����    
�û����ʹ���
�û���������
�ͻ�סַ    
֤������    
֤������    
֤������    
�ͻ�������
�ͻ�������
���ſͻ���־
���ſͻ�����
bak_field
�û�Ƿ��(tUserOwe)	  
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fGetUserBaseInfo(const char *phone_no,tUserBase *user_base)
{           
	EXEC SQL BEGIN DECLARE SECTION;
	/***�������õ���������****/		
	char		vphone_no[15+1];
	char		vid_no[23];		
	char		vcust_id[23];
	char		vsm_code[2+1];
	char		vsm_name[20+1];
	char		vcust_name[60+1];
	char		vuser_passwd[8+1];
	char		vrun_code[1+1];
	char		vrun_name[30+1];
	char		oRunName[30+1];
	char		vQueryFlag[1+1];
	char		vowner_grade[2+1];
	/*char		vgrade_name[20+1];*/
	char		vgrade_name[30+1];
	char		vowner_type[2+1];
	char		vtype_name[20+1];
	char		vcust_address[60+1];
	char		vid_type[1+1];
	char		vid_name[12+1];
	char		vid_iccid[20+1];
	char 		vcard_type[2+1];
	char 		vcard_name[20+1];
	char 		vgrpbig_flag[1+1];
	char		vMT_flag[1+1];	/*��ͣ��־��N����ͣ�û����������ƽ�����ľ�����������ͣ*/
	char 		vgrpbig_name[20+1];
	char		vcontract_no[23];
	double		vowe_fee=0;
	double		vprepay_fee=0;
	char		vregion_code[2+1];
	char        vBelongCode[8];
	char		vst[512];
	int			vtmp_num=0;
	int			vret=0;
	int			vcycle_begin_day=0;
	EXEC SQL END DECLARE SECTION;     

	memset(vphone_no,0,sizeof(vphone_no));
	memset(user_base->id_no,0,sizeof(user_base->id_no));
	memset(user_base->cust_id,0,sizeof(user_base->cust_id));
	memset(user_base->belong_code,0,sizeof(user_base->belong_code));
	memset(user_base->sm_code,0,sizeof(user_base->sm_code));
	memset(user_base->sm_name,0,sizeof(user_base->sm_name));
	memset(user_base->cust_name,0,sizeof(user_base->cust_name));
	memset(user_base->user_passwd,0,sizeof(user_base->user_passwd));
	memset(user_base->run_code,0,sizeof(user_base->run_code));
	memset(user_base->run_name,0,sizeof(user_base->run_name));
	memset(user_base->owner_grade,0,sizeof(user_base->owner_grade));
	memset(user_base->grade_name,0,sizeof(user_base->grade_name));
	memset(user_base->owner_type,0,sizeof(user_base->owner_type));
	memset(user_base->type_name,0,sizeof(user_base->type_name));
	memset(user_base->cust_address,0,sizeof(user_base->cust_address));
	memset(user_base->id_type,0,sizeof(user_base->id_type));
	memset(user_base->id_name,0,sizeof(user_base->id_name));
	memset(user_base->id_iccid,0,sizeof(user_base->id_iccid));
	memset(user_base->card_type,0,sizeof(user_base->card_type));
	memset(user_base->card_name,0,sizeof(user_base->card_name));
	memset(user_base->grpbig_flag,0,sizeof(user_base->grpbig_flag));
	memset(user_base->grpbig_name,0,sizeof(user_base->grpbig_name));
	memset(user_base->bak_field,0,sizeof(user_base->bak_field));
	memset(user_base->user_owe.accountNo,0,sizeof(user_base->user_owe.accountNo));
	user_base->user_owe.totalOwe=0;
	user_base->user_owe.totalPrepay=0;
	user_base->user_owe.accountOwe=0;
	user_base->user_owe.unbillFee=0;
	user_base->cycle_begin_day=0;

	memset(vid_no,0,sizeof(vid_no));
	memset(vcust_id,0,sizeof(vcust_id));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vBelongCode,0,sizeof(vBelongCode));
	memset(vsm_code,0,sizeof(vsm_code));
	memset(vsm_name,0,sizeof(vsm_name));
	memset(vcust_name,0,sizeof(vcust_name));
	memset(vuser_passwd,0,sizeof(vuser_passwd));
	memset(vrun_code,0,sizeof(vrun_code));
	memset(vrun_name,0,sizeof(vrun_name));
	memset(oRunName,0,sizeof(oRunName));
	memset(vQueryFlag,0,sizeof(vQueryFlag));
	memset(vowner_grade,0,sizeof(vowner_grade));
	memset(vgrade_name,0,sizeof(vgrade_name));
	memset(vowner_type,0,sizeof(vowner_type));
	memset(vtype_name,0,sizeof(vtype_name));
	memset(vcust_address,0,sizeof(vcust_address));
	memset(vid_type,0,sizeof(vid_type));
	memset(vid_name,0,sizeof(vid_name));
	memset(vid_iccid,0,sizeof(vid_iccid));

	memset(vcard_type,0,sizeof(vcard_type));
	memset(vcard_name,0,sizeof(vcard_name));
	memset(vgrpbig_flag,0,sizeof(vgrpbig_flag));
	memset(vMT_flag,0,sizeof(vMT_flag));
	memset(vgrpbig_name,0,sizeof(vgrpbig_name));

	memset(vregion_code,0,sizeof(vregion_code));
	memset(vst,0,sizeof(vst));

	memcpy(vphone_no,phone_no,sizeof(vphone_no)-1);

	/*��ѯdCustMsg��*/
	Sinitn(vid_no);
	Sinitn(vcust_id);
	Sinitn(vuser_passwd);
	Sinitn(vrun_code);
	Sinitn(vBelongCode);
	EXEC SQL SELECT to_char(id_no),to_char(cust_id),sm_code,
		user_passwd,substr(run_code,2,1),
		substr(attr_code,3,2),substr(attr_code,5,1),substr(attr_code,6,1),
		belong_code,substr(belong_code,1,2)
		INTO :vid_no,:vcust_id,:vsm_code,
	  :vuser_passwd,:vrun_code,
	  :vcard_type, :vgrpbig_flag,:vMT_flag,:vBelongCode,:vregion_code
	  FROM dCustMsg
	  WHERE phone_no=:vphone_no AND substr(run_code,2,1)<'a';
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","������ʾδ����");
		return 1207;
	}

	/*��ѯdCustDoc��*/
	EXEC SQL	SELECT cust_name,owner_grade,owner_type,cust_address,
						id_type,id_iccid
				  INTO :vcust_name,:vowner_grade,:vowner_type,:vcust_address,
						  :vid_type,:vid_iccid
				  FROM dCustDoc
				 WHERE cust_id=to_number(:vcust_id);
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","������ʾδ����");
		return 1208;
	}

	/*��ѯsSmCode��*/
	EXEC SQL	SELECT sm_name
				  INTO :vsm_name
				  FROM sSmCode
				 WHERE region_code=:vregion_code
				   AND sm_code=:vsm_code;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","������ʾδ����");
		return 1209;
	}

	printf("vregion_code[%s]vowner_grade[%s]",vregion_code,vowner_grade);	
	/*��ѯsCustGradeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vgrade_name
	  			  FROM sCustGradeCode
	  			 WHERE region_code=:vregion_code
	  			   and owner_code=:vowner_grade;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1210","������ʾδ����");
		return 1210;
	}
	
	/*��ѯsCustTypeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vtype_name
				  FROM sCustTypeCode
				 WHERE type_code=:vowner_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1211","������ʾδ����");
		return 1211;
	}

	/*��ѯsIdTtype��*/
	EXEC SQL	SELECT id_name
				  INTO :vid_name
				  FROM sIdType
				 WHERE id_type=:vid_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1212","������ʾδ����");
		return 1212;
	}

	/* ��ѯ sBigCardCode */
	EXEC SQL select card_name into :vcard_name
		from  sBigCardCode
		where card_type = :vcard_type;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1213","������ʾδ����");
		return 1213;
	}

	/* ��ѯ sGrpBigFlag */
	EXEC SQL select grp_name into :vgrpbig_name
		from  sGrpBigFlag
		where grp_flag = :vgrpbig_flag;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1214","������ʾδ����");
		return 1214;
	}

	/*�ʻ���Ƿ�Ѻ�Ԥ��*/	

	vret=fGetUserOwe(vid_no,&user_base->user_owe);
	if(vret!=0)
	{	
		pubLog_DBErr(_FFL_,"","-1","ȡ�û�Ƿ����Ϣ����");	 
		return -1;
	}   	

	if(strlen(user_base->user_owe.creditname)==0)
	{
		EXEC SQL	SELECT run_name
			  INTO :vrun_name
			  FROM sRunCode
			 WHERE region_code=:vregion_code
			   AND run_code=:vrun_code;
	}
	else
	{
		strcpy(vrun_name,user_base->user_owe.creditname);
	}

	/*��ȡ������Ϣ*/
	EXEC SQL SELECT Cycle_Begin_day INTO :vcycle_begin_day 
		FROM dCustAcctCycle 
	WHERE id_no = :vid_no 
		AND phone_no = :vphone_no 
		AND begin_time < to_char(sysdate,'YYYYMMDDHH24MISS')
		AND end_time > to_char(sysdate,'YYYYMMDDHH24MISS');
	if(SQLCODE != 0 && SQLCODE !=1403)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","121500","��ȡ������Ϣʧ��");
		return 121500;
	}
	else if(SQLCODE == 1403)
	{
		vcycle_begin_day=1;
	}


	/*��ֵ*/
	Trim(vid_no); strcpy(user_base->id_no,vid_no);
	Trim(vcust_id); strcpy(user_base->cust_id,vcust_id);
	Trim(vBelongCode); strcpy(user_base->belong_code,vBelongCode);
	Trim(vsm_code); strcpy(user_base->sm_code,vsm_code);
	Trim(vsm_name); strcpy(user_base->sm_name,vsm_name);
	Trim(vuser_passwd); strcpy(user_base->cust_name,vcust_name);
	Trim(vuser_passwd); strcpy(user_base->user_passwd,vuser_passwd);
	Trim(vrun_code); strcpy(user_base->run_code,vrun_code);
	Trim(vrun_name); strcpy(user_base->run_name,vrun_name);
	Trim(vowner_grade); strcpy(user_base->owner_grade,vowner_grade);
	Trim(vgrade_name); strcpy(user_base->grade_name,vgrade_name);

	Trim(vowner_type); strcpy(user_base->owner_type,vowner_type);
	Trim(vtype_name); strcpy(user_base->type_name,vtype_name);

	Trim(vcust_address); strcpy(user_base->cust_address,vcust_address);
	Trim(vid_type); strcpy(user_base->id_type,vid_type);
	Trim(vid_name); strcpy(user_base->id_name,vid_name);
	Trim(vid_iccid); strcpy(user_base->id_iccid,vid_iccid);

	Trim(vcard_type); strcpy(user_base->card_type,vcard_type);
	Trim(vcard_name); strcpy(user_base->card_name,vcard_name);
	Trim(vgrpbig_flag); strcpy(user_base->grpbig_flag,vgrpbig_flag);
	Trim(vgrpbig_name); strcpy(user_base->grpbig_name,vgrpbig_name);

	user_base->cycle_begin_day = vcycle_begin_day;
	return 0;
}

int fGetGrpUserOwe(const char *id_no, tUserOwe *user_owe)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char	vid_no[22+1];
	char	vcontract_no[22+1];
	char	vst[512];
	double	vowe_fee=0;
	double	vprepay_fee=0;
	int		vtmp_num=0;
	int     ids=0;
	int		vret=0;
	char    phone_no[15+1];
	double  v_unbill=0;
	long    contract_no=0;
	EXEC SQL END DECLARE SECTION;        

	memset(vid_no,0,sizeof(vid_no));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vst,0,sizeof(vst));
	init(phone_no);

	if(id_no!=NULL)
	{
		strcpy(vid_no,id_no);
	}
	
	EXEC SQL	select ids,contract_no,phone_no into :ids,:contract_no,:phone_no
				  from dCustMsg 
				 where id_no=:vid_no;
	if(SQLCODE!=0)
	{
#ifdef _DEBUG_
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","ȡ�û���Ϣ����");
#endif
		return -1;
	}

	memset(user_owe, 0, sizeof(tUserOwe));
	if(ids==1)
	{
		EXEC SQL SELECT nvl(owe_fee,0),nvl(prepay_fee,0) INTO :vowe_fee,:vprepay_fee
			FROM dConMsg
			WHERE contract_no=:contract_no;
		if(SQLCODE!=0)
		{
#ifdef _DEBUG_
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","ȡ�˻���Ϣ����");
#endif
			return -1;
		}
		vret=PublicGetUnbillChg(contract_no,atol(vid_no),0,phone_no,4,&v_unbill);
		if(vret!=0)
		{
#ifdef _DEBUG_
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","ȡδ���ʻ�������");
#endif    
			return -1;
		}
	
		user_owe->totalPrepay = vprepay_fee;
		user_owe->totalOwe = vowe_fee+v_unbill; 
		user_owe->unbillFee =v_unbill ;
		if (vowe_fee+v_unbill>0.005)
		{
			sprintf(user_owe->accountNo,"%ld", contract_no);
			if(vowe_fee+v_unbill-vprepay_fee>0.005)
			{
				sprintf(vcontract_no,"%ld",contract_no);
				user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
			}
		}
	}
	else
	{	    
		vtmp_num = 0;
		sprintf(vst,"select contract_no from dConUserMsg\
					where id_no=to_number(:v1)\
					and serial_no=0 ");
		EXEC SQL PREPARE stmt_sGetUserOwe2 FROM :vst;
		EXEC SQL DECLARE c_sGetUserOwe2 CURSOR FOR stmt_sGetUserOwe2;
		EXEC SQL OPEN c_sGetUserOwe2 using :vid_no;
		for(;;)
		{
			memset(vcontract_no,0,sizeof(vcontract_no));
			vowe_fee=0;
			v_unbill=0;
			contract_no=0;
			EXEC SQL FETCH c_sGetUserOwe2 INTO :contract_no;			
			if(SQLCODE==NOTFOUND || SQLCODE!=0) break;
			if(PubGrpAccount(contract_no)==1) continue;
			vret=GetConUserOweChg(contract_no,atol(vid_no),&vowe_fee);
			if(vret!=0)
			{
				pubLog_DBErr(_FFL_,"","-1","ȡǷ�ѳ���");
				EXEC SQL CLOSE c_sGetUserOwe2;
				return -1;
			}
			vret=PublicGetUnbillChg(contract_no,atol(vid_no),0,phone_no,4,&v_unbill);
			if(vret!=0)
			{
				pubLog_DBErr(_FFL_,"","-1","ȡδ���ʷ�����Ϣ����");
				EXEC SQL CLOSE c_sGetUserOwe2;
				return -1;
			}         
			user_owe->totalOwe = user_owe->totalOwe + vowe_fee;   	    	
			/*δ���ʻ���*/
			user_owe->unbillFee =user_owe->unbillFee +v_unbill ;    	
			vtmp_num++;

			vprepay_fee = 0.00;
			EXEC SQL	SELECT nvl(prepay_fee,0)
						  INTO :vprepay_fee
						  FROM dConMsg
						 WHERE contract_no=:contract_no;
			user_owe->totalPrepay += vprepay_fee;
			user_owe->totalOwe += vowe_fee+v_unbill; 
			user_owe->unbillFee += v_unbill ;
			if (vowe_fee+v_unbill>0.005 && user_owe->accountOwe < 0.005)
			{
				sprintf(user_owe->accountNo,"%ld", contract_no);
				if(vowe_fee+v_unbill-vprepay_fee>0.005)
				{
					sprintf(vcontract_no,"%ld",contract_no);
					user_owe->accountOwe = vowe_fee+v_unbill-vprepay_fee;
				}
			}
		}
	}
	EXEC SQL CLOSE c_sGetUserOwe2;

	return 0;
}

int  PubGrpAccount(long  in_contract_no)
{
    EXEC SQL BEGIN DECLARE SECTION;
    long  contract_no=0;
    int   flag=0;
    EXEC SQL   END DECLARE SECTION;
    contract_no=in_contract_no;
    EXEC SQL select  count(*) into :flag from dGrpUserMsg where account_id=:contract_no and bill_date>sysdate;
    if(SQLCODE!=0)
    {
        PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ�����û������");
        return -1;
    }
    if(flag==0) 
    { 
    	return 0;
    }
    else 
    { 	
    	pubLog_DBErr(_FFL_,"","1","������ʾδ����");
		return 1;
	}
}

int fGetUserGrpBaseInfo(const char *phone_no,tUserBase *user_base)
{           
	EXEC SQL BEGIN DECLARE SECTION;
	/***�������õ���������****/		
	char		vphone_no[15+1];
	char		vid_no[23];		
	char		vcust_id[23];
	char		vsm_code[2+1];
	char		vsm_name[20+1];
	char		vcust_name[60+1];
	char		vuser_passwd[8+1];
	char		vrun_code[1+1];
	char		vrun_name[20+1];
	char		vowner_grade[2+1];
	char		vgrade_name[20+1];
	char		vowner_type[2+1];
	char		vtype_name[20+1];
	char		vcust_address[60+1];
	char		vid_type[1+1];
	char		vid_name[12+1];
	char		vid_iccid[20+1];
	char 		vcard_type[2+1];
	char 		vcard_name[20+1];
	char 		vgrpbig_flag[1+1];
	char 		vgrpbig_name[20+1];
	char		vcontract_no[23];
	double		vowe_fee=0;
	double		vprepay_fee=0;
	char		vregion_code[2+1];
	char        vBelongCode[8];
	char		vst[512];
	int			vtmp_num=0;
	int			vret=0;
	EXEC SQL END DECLARE SECTION;     

	memset(vphone_no,0,sizeof(vphone_no));
	memset(user_base->id_no,0,sizeof(user_base->id_no));
	memset(user_base->cust_id,0,sizeof(user_base->cust_id));
	memset(user_base->belong_code,0,sizeof(user_base->belong_code));
	memset(user_base->sm_code,0,sizeof(user_base->sm_code));
	memset(user_base->sm_name,0,sizeof(user_base->sm_name));
	memset(user_base->cust_name,0,sizeof(user_base->cust_name));
	memset(user_base->user_passwd,0,sizeof(user_base->user_passwd));
	memset(user_base->run_code,0,sizeof(user_base->run_code));
	memset(user_base->run_name,0,sizeof(user_base->run_name));
	memset(user_base->owner_grade,0,sizeof(user_base->owner_grade));
	memset(user_base->grade_name,0,sizeof(user_base->grade_name));
	memset(user_base->owner_type,0,sizeof(user_base->owner_type));
	memset(user_base->type_name,0,sizeof(user_base->type_name));
	memset(user_base->cust_address,0,sizeof(user_base->cust_address));
	memset(user_base->id_type,0,sizeof(user_base->id_type));
	memset(user_base->id_name,0,sizeof(user_base->id_name));
	memset(user_base->id_iccid,0,sizeof(user_base->id_iccid));
	memset(user_base->card_type,0,sizeof(user_base->card_type));
	memset(user_base->card_name,0,sizeof(user_base->card_name));
	memset(user_base->grpbig_flag,0,sizeof(user_base->grpbig_flag));
	memset(user_base->grpbig_name,0,sizeof(user_base->grpbig_name));
	memset(user_base->bak_field,0,sizeof(user_base->bak_field));
	memset(user_base->user_owe.accountNo,0,sizeof(user_base->user_owe.accountNo));
	user_base->user_owe.totalOwe=0;
	user_base->user_owe.totalPrepay=0;
	user_base->user_owe.accountOwe=0;
	user_base->user_owe.unbillFee=0;

	memset(vid_no,0,sizeof(vid_no));
	memset(vcust_id,0,sizeof(vcust_id));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vBelongCode,0,sizeof(vBelongCode));
	memset(vsm_code,0,sizeof(vsm_code));
	memset(vsm_name,0,sizeof(vsm_name));
	memset(vcust_name,0,sizeof(vcust_name));
	memset(vuser_passwd,0,sizeof(vuser_passwd));
	memset(vrun_code,0,sizeof(vrun_code));
	memset(vrun_name,0,sizeof(vrun_name));
	memset(vowner_grade,0,sizeof(vowner_grade));
	memset(vgrade_name,0,sizeof(vgrade_name));
	memset(vowner_type,0,sizeof(vowner_type));
	memset(vtype_name,0,sizeof(vtype_name));
	memset(vcust_address,0,sizeof(vcust_address));
	memset(vid_type,0,sizeof(vid_type));
	memset(vid_name,0,sizeof(vid_name));
	memset(vid_iccid,0,sizeof(vid_iccid));

	memset(vcard_type,0,sizeof(vcard_type));
	memset(vcard_name,0,sizeof(vcard_name));
	memset(vgrpbig_flag,0,sizeof(vgrpbig_flag));
	memset(vgrpbig_name,0,sizeof(vgrpbig_name));

	memset(vregion_code,0,sizeof(vregion_code));
	memset(vst,0,sizeof(vst));

	memcpy(vphone_no,phone_no,sizeof(vphone_no)-1);

	/*��ѯdCustMsg��*/
	Sinitn(vid_no);
	Sinitn(vcust_id);
	Sinitn(vuser_passwd);
	Sinitn(vrun_code);
	Sinitn(vBelongCode);
	EXEC SQL SELECT to_char(id_no),to_char(cust_id),sm_code,
		user_passwd,substr(run_code,2,1),
		substr(attr_code,3,2),substr(attr_code,5,1),
		belong_code,substr(belong_code,1,2)
		INTO :vid_no,:vcust_id,:vsm_code,
	  :vuser_passwd,:vrun_code,
	  :vcard_type, :vgrpbig_flag,:vBelongCode,:vregion_code
	  FROM dCustMsg
	  WHERE phone_no=:vphone_no AND substr(run_code,2,1)<'a';
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","������ʾδ����");
		return 1207;
	}
	
	/*��ѯdCustDoc��*/
	EXEC SQL	SELECT cust_name,owner_grade,owner_type,cust_address,
						id_type,id_iccid
				  INTO :vcust_name,:vowner_grade,:vowner_type,:vcust_address,
						  :vid_type,:vid_iccid
				  FROM dCustDoc
				 WHERE cust_id=to_number(:vcust_id);
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","������ʾδ����");
		return 1208;
	}

	/*��ѯsSmCode��*/
	EXEC SQL	SELECT sm_name
				  INTO :vsm_name
				  FROM sSmCode
				 WHERE region_code=:vregion_code
				   AND sm_code=:vsm_code;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1209","������ʾδ����");
		return 1209;
	}
	
	/*��ѯsRunCode��*/
	EXEC SQL	SELECT run_name
				  INTO :vrun_name
				  FROM sRunCode
				 WHERE region_code=:vregion_code
				   AND run_code=:vrun_code;

	/*��ѯsCustGradeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vgrade_name
	  			  FROM sCustGradeCode
	  			 WHERE region_code=:vregion_code
	  			   and owner_code=:vowner_grade;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1210","������ʾδ����");
		return 1210;
	}
	
	/*��ѯsCustTypeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vtype_name
				  FROM sCustTypeCode
				 WHERE type_code=:vowner_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1211","������ʾδ����");
		return 1211;
	}

	/*��ѯsIdTtype��*/
	EXEC SQL	SELECT id_name
				  INTO :vid_name
				  FROM sIdType
				 WHERE id_type=:vid_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1212","������ʾδ����");
		return 1212;
	}

	/* ��ѯ sBigCardCode */
	EXEC SQL select card_name into :vcard_name
		from  sBigCardCode
		where card_type = :vcard_type;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1213","������ʾδ����");
		return 1213;
	}

	/* ��ѯ sGrpBigFlag */
	EXEC SQL select grp_name into :vgrpbig_name
		from  sGrpBigFlag
		where grp_flag = :vgrpbig_flag;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1214","������ʾδ����");
		return 1214;
	}

	/*�ʻ���Ƿ�Ѻ�Ԥ��*/
	vret=fGetGrpUserOwe(vid_no,&user_base->user_owe);
	if(vret!=0)
	{	
		pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
		return -1;
	}    

	/*��ֵ*/
	Trim(vid_no); strcpy(user_base->id_no,vid_no);
	Trim(vcust_id); strcpy(user_base->cust_id,vcust_id);
	Trim(vBelongCode); strcpy(user_base->belong_code,vBelongCode);
	Trim(vsm_code); strcpy(user_base->sm_code,vsm_code);
	Trim(vsm_name); strcpy(user_base->sm_name,vsm_name);
	Trim(vuser_passwd); strcpy(user_base->cust_name,vcust_name);
	Trim(vuser_passwd); strcpy(user_base->user_passwd,vuser_passwd);
	Trim(vrun_code); strcpy(user_base->run_code,vrun_code);
	Trim(vrun_name); strcpy(user_base->run_name,vrun_name);
	Trim(vowner_grade); strcpy(user_base->owner_grade,vowner_grade);
	Trim(vgrade_name); strcpy(user_base->grade_name,vgrade_name);

	Trim(vowner_type); strcpy(user_base->owner_type,vowner_type);
	Trim(vtype_name); strcpy(user_base->type_name,vtype_name);

	Trim(vcust_address); strcpy(user_base->cust_address,vcust_address);
	Trim(vid_type); strcpy(user_base->id_type,vid_type);
	Trim(vid_name); strcpy(user_base->id_name,vid_name);
	Trim(vid_iccid); strcpy(user_base->id_iccid,vid_iccid);

	Trim(vcard_type); strcpy(user_base->card_type,vcard_type);
	Trim(vcard_name); strcpy(user_base->card_name,vcard_name);
	Trim(vgrpbig_flag); strcpy(user_base->grpbig_flag,vgrpbig_flag);
	Trim(vgrpbig_name); strcpy(user_base->grpbig_name,vgrpbig_name);

	return 0;
}

/************************************************************************
��������:fGetIccidOwe
����ʱ��:2003/10/19
������Ա:lugz
��������:�����û�֤������ȡ�����û���Ƿ����Ϣ
�������:�û�֤������
�������:�û�Ƿ����Ϣ�б�
�� �� ֵ:0������ȷ����,��������
************************************************************************/

int fGetIccidOwe(const char *iccid, tUserOwe *user_owe)
{
	return -1;
}

/*
*���ѡ�����ط��Ƿ��룬�ײͰ󶨵��ط���ͻ
*/
int checkBillBindFunc(const char* allFuncCode, const char billBindFunc[][2+1],
					  int billBindFuncNum, char *funcCode)
{
	const char *p, *q;
	int i;

	p = allFuncCode;
	while ((q = strchr(p, '|')) != (char *) NULL)
	{
		for(i = 0; i < billBindFuncNum; i ++)
		{       
			if ( strncmp(p, billBindFunc[i], 2) == 0 )
			{
				memcpy(funcCode,billBindFunc[i],2);
				pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
				return -1;
			}
		}
		p = q + 1;
	}
	return 0;
}

/*ȡ���ط�
*��ʽ:��'AA|' + 'AA|' + 'AA|' + ������
*      "AA"��ʾ�ط����룬"|"���ָ�����
*/
int fDelFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	int	 iRetCode = 0;
	EXEC SQL BEGIN DECLARE SECTION;
	char function_code[2+1];
	char tmpFuncCode[3+1];
	char add_no[15+1];
	char	dynStmt[1024];
	int		iCount = 0;

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	long  vOldAccept=0;
	
	TdCustFuncIndex oldIndex;
	TdCustFuncAddIndex oldIndex1;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char vFunctionType[20+1];
	int ret=0;
	char  TmpSqlStr[2048];
	TdCustFuncAddHis	sTdCustFuncAddHis;
	TdCustFuncHis	sTdCustFuncHis;
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	

	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, q - p);
		function_code[2] = '\0';
		p = q + 1;

		iRetCode = checkFuncCode(funcEnv->idNo,'d',funcEnv->belongCode,function_code,funcEnv->smCode);
		if(iRetCode != 0) return iRetCode;

		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'N',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute 
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
				:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121902","������ʾδ����");
			pubLog_Debug(_FFL_,"","idNo","idNo=%d",idNo);
			return 121902;
		}
    /*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'd'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121903","������ʾδ����");
			return 121903;
		}
    */
    Trim(idNo);
    EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept,:totalDate, :opTime, :opCode, 'd'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	  if(SQLCODE != 0 && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121903","������ʾδ����");
			return 121903;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 			
		    ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fDelFuncs",sTdCustFuncHis);
			if (ret < 0)
		    {    				  
		      PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121903","������ʾδ����");
				return 121903;
		    }
		}
		/*sprintf(dynStmt,
			"DELETE FROM dCustFunc"
			" WHERE id_no = to_number(:v1)"
			"	AND function_code = :v2");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121903","������ʾδ����");
			return 121903;
		}*/
		init(TmpSqlStr);
		sprintf(TmpSqlStr,"select FUNCTION_TYPE from dCustFunc WHERE id_no = to_number(:v1) AND function_code = :v2 ");
		EXEC SQL PREPARE sql_str FROM :TmpSqlStr;
  	EXEC SQL declare ngcur04 cursor for sql_str;
  	EXEC SQL open ngcur04 using :idNo, :function_code;    
  	for(;;)
  	{ 
  		init(vFunctionType);	
  		init(v_parameter_array);
  		memset(&oldIndex,0,sizeof(oldIndex));
  		EXEC SQL FETCH ngcur04 into :vFunctionType;
  		if((0!=SQLCODE)&&(1403!=SQLCODE))
  		{					
  			PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-4","��ѯdCustFuncr����!");
				EXEC SQL CLOSE ngcur04;
				return 121903;
  		}
  		if(1403==SQLCODE) break;
  		
  		Trim(vFunctionType);
  		 
    	strcpy(v_parameter_array[0],idNo);     
    	strcpy(v_parameter_array[1],vFunctionType);
    	strcpy(v_parameter_array[2],function_code);
    	
    	strcpy(oldIndex.sIdNo , idNo);                
    	strcpy(oldIndex.sFunctionType , vFunctionType);  
    	strcpy(oldIndex.sFunctionCode , function_code);  
			        				
    	ret=OrderDeleteCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fDelFuncs",oldIndex,"",v_parameter_array);
			if (ret < 0)
    	{    				  
    	  PUBLOG_DBDEBUG("");
			  pubLog_DBErr(_FFL_,"","121903","������ʾδ����");
			  EXEC SQL CLOSE ngcur04;
			  return 121903;
    	}
    }
    EXEC SQL CLOSE ngcur04;   	
    
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'd'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if ((SQLCODE != 0) && (SQLCODE != 1403))
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121904","������ʾδ����");
			return 121904;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE, FUNCTION_CODE,	nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		    nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),nvl(OTHER_ATTR,chr(0)),
		    :loginNo, :loginAccept,	:totalDate ,:opTime, :opCode, 'd'
		    into :sTdCustFuncAddHis from dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	   if((SQLCODE != 0) && (SQLCODE != 1403))
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121904","������ʾδ����");
			return 121904;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 		
		    ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fDelFuncs",sTdCustFuncAddHis);
				if (ret < 0)
		    {    				  
		      PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121904","������ʾδ����");
					return 121904;
		    }
		}
    /*ng���� by zhaohx at 04/08/2009 end*/
		sprintf(dynStmt,
			"UPDATE dCustRes"
			"   SET resource_code = '0'"
			" WHERE phone_no = (select add_no from dCustFuncAdd where id_no = to_number(:v1) and function_code = :v2)"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121905","������ʾδ����");
			return 121905;
		}
		
		/***����������Դͬ��  20090224 add***/
		init(dynStmt);
		sprintf(dynStmt,
			" INSERT INTO dgoodphonereshis "
            "    (  phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,"
			"		begin_time,end_time,bak_field,district_code,resource_code ,"
			"		update_login,update_code,update_date,update_time,update_type,update_accept )"
			" select phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,"
			"		begin_time,end_time,bak_field,district_code,resource_code ,"
			"		:v1,:v2,to_number(:v3),sysdate,'u',to_number(:v4)"
			"  from  dgoodphoneres"
			" where  phone_no = (select add_no from dCustFuncAdd where id_no = to_number(:v5) and function_code = :v6)"
			
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,:opCode,:totalDate,:loginAccept,
											:idNo,	:function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121911","������ʾδ����");
			return 121911;
		}
		
		init(dynStmt);
		sprintf(dynStmt,
			"UPDATE dgoodphoneres"
			"   SET resource_code = '0'"
			" WHERE phone_no = (select add_no from dCustFuncAdd where id_no = to_number(:v1) and function_code = :v2)"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121912","������ʾδ����");
			return 121912;
		}
		/***����������Դͬ��  20090224 EDD***/
    /*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"DELETE FROM dCustFuncAdd"
			" WHERE id_no = to_number(:v1)"
			"	AND function_code = :v2"
			,	idNo, function_code);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:function_code;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121906","������ʾδ����");
			return 121906;
		}*/
		Trim(idNo);		
		init(TmpSqlStr);
		sprintf(TmpSqlStr,"select FUNCTION_TYPE from dCustFuncAdd WHERE id_no = to_number(:v1) AND function_code = :v2 ");
		EXEC SQL PREPARE sql_str FROM :TmpSqlStr;
  	EXEC SQL declare ngcur06 cursor for sql_str;
  	EXEC SQL open ngcur06 using :idNo, :function_code;    
  	for(;;)
  	{ 
  		init(vFunctionType);	
  		init(v_parameter_array);
  		memset(&oldIndex1,0,sizeof(oldIndex1));
  		EXEC SQL FETCH ngcur06 into :vFunctionType;
  		if((0!=SQLCODE)&&(1403!=SQLCODE))
  		{					
  			PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121906","��ѯdCustFuncAdd����!");
				EXEC SQL CLOSE ngcur06;
				return 121906;
  		}
  		if(1403==SQLCODE) break;
  		
  		Trim(vFunctionType);
 		 
    	strcpy(v_parameter_array[0],idNo);     
    	strcpy(v_parameter_array[1],function_code);
    	strcpy(v_parameter_array[2],vFunctionType);
    	
   		strcpy(oldIndex1.sIdNo, idNo);                
    	strcpy(oldIndex1.sFunctionCode , function_code);  
    	strcpy(oldIndex1.sFunctionType , vFunctionType);  
			        				
    	ret=OrderDeleteCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fDelFuncs",oldIndex1,"",v_parameter_array);
			if (ret < 0)
    	{    				  
    	  PUBLOG_DBDEBUG("");
			  pubLog_DBErr(_FFL_,"","121906","������ʾδ����, ret===[%d]", ret);
			  EXEC SQL CLOSE ngcur06;
			  return 121906;
    	}
    }
    EXEC SQL CLOSE ngcur06;  
		/*ng���� by zhaohx at 04/08/2009 end*/ 
		vOldAccept=0;
		if(strcmp(opCode,"1219")==0)
		{
			EXEC SQL select b.login_accept 
					into :vOldAccept
				from dbillcustdetail a, wCustExpire b
				where a.id_no=to_number(:idNo)
				and a.id_no=b.id_no
				and a.login_accept=b.login_accept
				and b.function_code=:function_code
				and b.business_status='1'
				and b.EXPIRE_TIME>sysdate
				and a.end_time>sysdate
				and b.op_code<>'1219'
				and a.detail_type='a'
				and rownum<2;
			if(SQLCODE!=0 && SQLCODE!=1403)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121906","������ʾδ����");
				return 121906;
			}
		}
 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
		sprintf(dynStmt,
			"INSERT INTO wCustExpireHis"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,org_id,LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'd'"
			"  FROM wCustExpire"
			" WHERE id_no = to_number(:v6)"
			"   and function_code = :v7 and login_accept<>:v8");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code,:vOldAccept;
		end;
		end-exec;
		if((SQLCODE != 0) && (SQLCODE != 1403))
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121907","������ʾδ����");
			return 121907;
		}

		sprintf(dynStmt,
			"DELETE FROM wCustExpire"
			" WHERE id_no = to_number(:v1)"
			"	AND function_code = :v2 and login_accept<>:v3");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code,:vOldAccept;
		end;
		end-exec;
		if ((SQLCODE != 0) && (SQLCODE != 1403))
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121908","������ʾδ����");
			return 121908;
		}

		if( strcmp(function_code, "05") == 0) /* ȡ������ת��ͬʱȡ���������� */
		{
			iCount = 0;
			EXEC SQL select count(*) into :iCount 
				from 	dCustFunc
				where  id_no = to_number(:idNo)
				and 	function_code = '47';
			if(iCount != 0 && (strstr(funcs, "47") == NULL))
			{
				iCount = 0;
				Sinitn(tmpFuncCode);
				sprintf(tmpFuncCode, "%s|", "47");
				if((iCount = fDelFuncs(funcEnv, tmpFuncCode)) != 0)
				{	
					pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iCount);
					return iCount;
				}

			}
		} 
		/* �����ػ����� */
		iRetCode = fSendCmd(funcEnv, '0', "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}

		/* �����ػ��������� */
		iRetCode = fSendCmdFuncAdd(funcEnv, "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}
		/* ȡ���������� del by lixg + sFuncListAdd */
		/*if(strcmp(function_code, "17") == 0)
		{
		iRetCode = fSendCmd(funcEnv, '0', "1219", "16");
		if(iRetCode != 0) 
		{
		return iRetCode;
		}
		}
		*/

	}
	return 0;
}

/*�޸���Ч���ط�
*��ʽ:��'AAYYYYMMDD|' + 'AAYYYYMMDD|' + 'AAYYYYMMDD|' + �� ����
*      "AA"��ʾ�ط����룬"YYYYMMDD"��ʾ�������ڣ�"|"���ָ�����
*/
int fUpdUsedFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	int hasExpire = 0;
	int iRetCode = 0;
	char end_date[8+1];
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];

	char function_code[2+1];
	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);

	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p == 10)
		{
			strncpy(end_date, p+2, 8);
			end_date[8] = '\0';
			hasExpire = 1;
		}
		else if (q - p == 2)
		{
			hasExpire = 0;
		}
		else
		{	
			pubLog_DBErr(_FFL_,"","121920","������ʾδ����");
			return 121920;
		}
		p = q + 1;

		iRetCode = checkFuncCode(idNo,'u',belongCode,function_code,smCode);
		if(iRetCode != 0) 
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}
		if(hasExpire == 1)
		{
  /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
			sprintf(dynStmt,
				"INSERT INTO wCustExpireHis"
				"("
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,ORG_ID,LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
				"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
				")"
				"SELECT"
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,ORG_ID,LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
				"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'u' "
				"  FROM wCustExpire"
				" WHERE id_no = to_number(:v6)"
				"   and function_code = :v7"
				"   AND business_status = '0'"
				);
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
					:idNo, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{	
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121909","������ʾδ����");
				return 121909;
			}

			/*business_status = '0':��ʾԤԼ�ر�*/
			sprintf(dynStmt,
				"UPDATE wCustExpire"
				"   SET EXPIRE_TIME=to_date(:v1, 'yyyymmdd')"
				" WHERE id_no = to_number(:v2)"
				"	AND function_code = :v3"
				"   AND business_status = '0'"
				);
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :end_date,	:idNo, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{	
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121910","������ʾδ����");
				return 121910;
			}
		}
	}
	return 0;
}

/*�޸�δ��Ч���ط�
*��ʽ:'AAYYYYMMDDYYYYMMDD|' + 'AAYYYYMMDDYYYYMMDD |'  +  'AAYYYYMMDDYYYYMMDD |' + ������
*      "AA"��ʾ�ط����룬"YYYYMMDDYYYYMMDD"��ʾ��ʼ���ں͵������ڣ�"|"��ʾ��ָ�����
*/
int fUpdUnUsedFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char begin_date[8+1];
	char end_date[8+1];
	int  iRetCode = 0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);

	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		strncpy(begin_date, p+2, 8);
		begin_date[8] = '\0';
		strncpy(end_date, p+10, 8);
		end_date[8] = '\0';
		p = q + 1;

		iRetCode = checkFuncCode(idNo,'u',belongCode,function_code,smCode);
		if(iRetCode != 0) 
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}
 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
		sprintf(dynStmt,
			"INSERT INTO wCustExpireHis"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'u' "
			"  FROM wCustExpire"
			" WHERE id_no = to_number(:v6)"
			"   and function_code = :v7"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
								:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121911","������ʾδ����");
			return 121911;
		}

		/*business_status = '1':��ʾԤԼ��ͨ*/
		sprintf(dynStmt,
			"UPDATE wCustExpire"
			"   SET EXPIRE_TIME=to_date(:v1, 'yyyymmdd')"
			" WHERE id_no = to_number(:v2)"
			"	AND function_code = :v3"
			"   AND business_status = '1'"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :begin_date,	:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121912","������ʾδ����");
			return 121912;
		}

		/*business_status = '0':��ʾԤԼ�ر�*/
		sprintf(dynStmt,
			"UPDATE wCustExpire"
			"   SET EXPIRE_TIME=to_date(:v1, 'yyyymmdd')"
			" WHERE id_no = to_number(:v2)"
			"	AND function_code = :v3"
			"   AND business_status = '0'");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :end_date,	:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121913","������ʾδ����");
			return 121913;
		}
	}
	return 0;
}

/*����������Ч���ط�
*��ʽ:��'AAYYYYMMDD|' + 'AAYYYYMMDD|' + 'AAYYYYMMDD|' + ������ 
*     "AA"��ʾ�ط����룬"YYYYMMDD"��ʾ�������ڣ�"|"��ʾ��ָ����������û�е��ڣ���գ���
*/
int fAddUsedFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char end_date[8+1];
	int  hasExpire = 0; /*�Ƿ��е���ʱ��*/
	int  iRetCode = 0;
	int ret =0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
  char vOrgId[10+1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
  TdCustFunc	sTdCustFunc;
  TdCustFuncAdd	sTdCustFuncAdd;
  TdCustFuncAddHis	sTdCustFuncAddHis;
  TdCustFuncHis	sTdCustFuncHis;
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(vOrgId);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(orgCode);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);

	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p == 10)
		{
			strncpy(end_date, p+2, 8);
			end_date[8] = '\0';
			hasExpire = 1;
		}
		else if (q - p == 2)
		{
			hasExpire = 0;
		}
		else
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121920","������ʾδ����");
			return 121920;
		}

		p = q + 1;
        printf("function_code = [%s]\n", function_code);
		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) 
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}
			
	  /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
			  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/		
	
		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
			:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121914","������ʾδ����");
			return 121914;
		}
    /*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFunc"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v3,1,2)"
			"   AND sm_code = :v4"
			"   AND function_code = :v5"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;
		}*/     
		Trim(idNo); 
	  EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE, :opTime  into :sTdCustFunc 
	  	FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code ;
	  if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;		
		}
		ret = 0; 		 		
    ret=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedFuncs",sTdCustFunc);
		if (ret!=0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;
    }
		
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","������ʾδ����");
			return 121916;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE, to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept,:totalDate, :opTime, :opCode,'a'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	   if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","������ʾδ����");
			return 121916;		
		}
		ret = 0; 		 				
    ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedFuncs",sTdCustFuncHis);
		if (ret != 0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","������ʾδ����");
			return 121916;
    }
    
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAdd"
			"("
			"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
			"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
			"add_months(to_date(:v3, 'yyyymmdd hh24:mi:ss'), month_limit), MONTH_LIMIT, NULL, NULL"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v4,1,2)"
			"   AND sm_code = :v5"
			"   AND function_code = :v6"
			"   AND month_limit > 0");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:belongCode,
			:smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121917","������ʾδ����");
			return 121917;
		}*/
			 
		Trim(idNo);
		Trim(opTime);
		Trim(belongCode);
		Trim(smCode);
		Trim(function_code);	 
		
		EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE,:opTime,
			 to_char(add_months(to_date(:opTime,'yyyymmdd hh24:mi:ss'), month_limit),'yyyymmdd hh24:mi:ss'), to_char(MONTH_LIMIT), chr(0), chr(0) into :sTdCustFuncAdd
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code
			 AND month_limit > 0 ;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 	
			ret=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedFuncs",sTdCustFuncAdd);
			if (ret < 0)
	    {    				  
	      PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
				return 121915;
	    }
    	}
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
						:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","������ʾδ����");
			return 121918;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,FUNCTION_CODE,nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		   nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),nvl(OTHER_ATTR,chr(0)),
			 :loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a' 
			 into :sTdCustFuncAddHis  FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code;
		 if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","������ʾδ����");
			return 121918;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 		 	
			ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedFuncs",sTdCustFuncAddHis);
			if (ret < 0)
		    {    				  
		      	PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121918","������ʾδ����");
					return 121918;
		    }
		}
    /*ng���� by zhaohx at 04/08/2009 end*/
		if (hasExpire)
		{
		 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
			sprintf(dynStmt,
				"INSERT INTO wCustExpire"
				"("
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,org_id,LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
				")"
				" SELECT"
				"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
				"	:v4,:vOrgId,:v5,	to_number(:v6),"
				"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
				"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��'"
				"  FROM sFuncList"
				" where region_code=substr(:v12, 1,2)"
				"   and sm_code = :v13"
				"   and function_code = :v14");
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
								:orgCode,:vOrgId,:loginNo,	:loginAccept,
								:totalDate, :opCode,	:opTime, :function_code,
								:end_date, 
								:belongCode, :smCode, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121919","������ʾδ����");
				return 121919;
			}

		}
		iRetCode = fSendCmd(funcEnv, '1', "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}

	}
	return 0;
}

/*����ԤԼ��Ч���ط�
*��ʽ:����ʽ:'AAYYYYMMDDYYYYMMDD|' + 'AAYYYYMMDDYYYYMMDD|'  +  'AAYYYYMMDDYYYYMMDD|' + ������
*      "AA"��ʾ�ط����룬"YYYYMMDDYYYYMMDD"��ʾ��ʼ���ں͵������ڣ�"|"���ָ�����
*/
int fAddUnUsedFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char begin_date[8+1];
	char end_date[8+1];
	char iRetCode = 0;
	int ret =0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
	char vOrgId[10+1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(orgCode);
	Sinitn(vOrgId);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);


	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	pubLog_Debug(_FFL_,"","funcs","funcs = %s",funcs); 
	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		strncpy(begin_date, p+2, 8);
		begin_date[8] = '\0';
		strncpy(end_date, p+10, 8);
		end_date[8] = '\0';
		p = q + 1;

		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) 
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}
			
	  /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
			  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
	  
 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
		sprintf(dynStmt,
			"INSERT INTO wCustExpire"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
			")"
			"SELECT"
			"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
			"	:v4,:vOrgId,	:v5,	to_number(:v6),"
			"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
			"	'1',	to_date(:v11, 'yyyymmdd'),	'�ط����ڿ���' "
			"  FROM sFuncList"
			" where region_code=substr(:v12, 1,2)"
			"   and sm_code = :v13"
			"   and function_code = :v14");

		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
							:orgCode,:vOrgId,:loginNo,	:loginAccept,
							:totalDate, :opCode,	:opTime, :function_code,
							:begin_date, 
							:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121920","������ʾδ����");
			return 121920;
		}
 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/

		sprintf(dynStmt,
			"INSERT INTO wCustExpire"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
			")"
			"SELECT"
			"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
			"	:v4,:vOrgId,	:v5,	to_number(:v6),"
			"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
			"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��' "
			"  FROM sFuncList"
			" where region_code=substr(:v12, 1,2)"
			"   and sm_code = :v13"
			"   and function_code = :v14");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
							:orgCode,:vOrgId,:loginNo,	:loginAccept,
							:totalDate, :opCode,	:opTime, :function_code,
							:end_date, 
							:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121921","������ʾδ����");
			return 121921;
		}
	}
	return 0;
}

/*���Ӵ��̺ŵ��ط����������ܹ�����ԤԼ
* ��ʽ:����ʽ:'AAXXXXXXXX|' + 'AAXXXXXXXX|'  +  'AAXXXXXXXX|' + ������
*      "AA"��ʾ�ط����룬"XXXXXXXX"��ʾ��ʼ���ں͵������ڣ�"|"���ָ�����
*/
int fAddAddFuncs(tFuncEnv *funcEnv, char *funcs)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char add_no[15+1];
	int	 iRetCode = 0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	int  iCount=0;
	int  iCountFav=0;
    char	outtime[17+1];
    long	rspno =0;
    long	tLoginAccept=0;
  int iCountBind=0;
  TdCustFunc	sTdCustFunc;
  TdCustFuncAdd	sTdCustFuncAdd;
  TdCustFuncAddHis	sTdCustFuncAddHis;
  TdCustFuncHis	sTdCustFuncHis;
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(orgCode);
	Sinitn(outtime);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
    EXEC SQL select to_char(sysdate+70/(24*60),'yyyymmdd HH24:MI:SS') into :outtime from dual;
	
	srand(time(NULL));
	
	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p != 0)
		{
			strncpy(add_no, p+2, q - p-2);
			add_no[q - p - 2] = '\0';
			add_no[15] = '\0';
		}
		else
		{	
			pubLog_DBErr(_FFL_,"","121922","������ʾδ����");
			return 121922;
		}
		p = q + 1;

		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) return iRetCode;
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 begin*/
		if(strcmp(opCode,"1219") == 0)
		{ 
		/*	srand(time(NULL));  */
			
			init(dynStmt);
			sprintf(dynStmt,	"SELECT count(*) "
								"  FROM dBillCustDetail%c a, sBillFuncBind b"
								" WHERE a.DETAIL_CODE = b.FUNCTION_BIND"
								"   AND a.DETAIL_TYPE = 'a'"
								"   AND b.function_code = :v0 "
								"   AND b.region_code = substr(:v1, 1,2)"
								"   AND begin_time <= sysdate"
								"   AND end_time > sysdate"
								"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
								,idNo[strlen(idNo)-1]);
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :iCountBind using :function_code,:belongCode,:idNo;
			END;
			END-EXEC;
			if(SQLCODE != 0 && SQLCODE != 1403)    
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ�ײͰ󶨵��ط�ʧ��!SQLCODE=%d",SQLCODE);
				return 121921;
			}
				
			if ( iCountBind > 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�������б��!");
				return 121921;
			
			}                            
			                           
      EXEC SQL select count(*) into :iCount from SFUNCSENDMSG where region_code=substr(:belongCode,1,2) 
        		AND sm_code = :smCode  and function_code=:function_code;	
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ SFUNCSENDMSG ʧ��!SQLCODE=%d",SQLCODE);
				return 121921;
			}
			printf("iCount=%d\n",iCount);
			if(iCount>0)
			{   
				rspno=0;
				rspno=rand()%900000+100000;
				Sinitn(dynStmt);     
				sprintf(dynStmt,	"SELECT  count(*) "
						"  FROM dBillCustDetail%c a, sBillFuncFav b"
						" WHERE a.DETAIL_CODE = b.FUNCTION_BILL"
						"   AND a.DETAIL_TYPE = '4'"
						"   AND b.function_code = :v0 "
						"   AND b.region_code = substr(:v1, 1,2)"
						"   AND begin_time <= sysdate"
						"   AND end_time > sysdate"
						"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
						,idNo[strlen(idNo)-1]);
				EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :dynStmt INTO :iCountFav using :function_code,:belongCode,:idNo;
				END;
				END-EXEC;
				if(SQLCODE != 0 && SQLCODE != 1403)    
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121922","ȡ�ط��Żݴ���!SQLCODE=%d",SQLCODE);
					return 121922;
				}
				
				if(iCountFav==0 )
				{  
					/*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� begin*/
					tLoginAccept=0;
			        EXEC SQL select sMaxSysAccept.nextval
			        		   into :tLoginAccept
			        		   from dual;
				    if(SQLCODE!=0)
				    {
				        printf("\nȡ��ˮ[%s][%d]",SQLERRMSG,SQLCODE);
				        return -3;
				    }
				    /*majha  edit  at 20100930 �������ȡ��ˮ��һ���޷��շ����� end*/
				    
				   	Sinitn(dynStmt);     
					sprintf(dynStmt,
						"INSERT INTO dCrmRemindMsg "
						"("
						"	MAXACCEPT, ID_NO , PHONE_NO, BIZTYPE,	SPID,BIZCODE,BIZNAME,"
						"	RSP_NO , OP_TIME, SERV_PROPERTY,TOTAL_DATE,SEND_TIME,OUT_TIME,CONFIRM_FLAG "
						")"
						"select"
						"  :v1,	to_number(:v2),	:v3, '00',"
						"  FUNCTION_TYPE,:v4,FUNCTION_NAME,:v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),'3', :v7,"
						"  to_date(:v8, 'yyyymmdd hh24:mi:ss'),to_date(:v9, 'yyyymmdd hh24:mi:ss'),'0'"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v10,1,2)"
						"   AND sm_code = :v11"
						"   AND function_code = :v4"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :tLoginAccept,:idNo, :phoneNo,:function_code,:rspno,
						 :opTime,:totalDate,:opTime,:outtime,:belongCode,:smCode;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121924","�����dCrmRemindMsgʧ��!SQLCODE=%d",SQLCODE);
						return 121924;
					}
					
					#ifdef _CHGTABLE_
					Sinitn(dynStmt);     
					sprintf(dynStmt,
						"INSERT INTO wfuncremindchg "
						"("
						"	OP_NO,		ID_NO,		PHONE_NO,	MAXACCEPT, OP_TYPE, "
						"	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME, FEE_FLAG,FLAG_TIME "
						")"
						"select"
						"   sMaxSysAccept.nextval,	to_number(:v2),	:v3, :v4,'i', "
						"	FUNCTION_TYPE, :v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'), 0,to_date(:v7, 'yyyymmdd hh24:mi:ss')"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v8,1,2)"
						"   AND sm_code = :v9"
						"   AND function_code = :v10"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :idNo, :phoneNo,:tLoginAccept,
						:function_code, :opTime,:opTime,:belongCode, :smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121925","�����wfuncremindchgʧ��!SQLCODE=%d",SQLCODE);
						return 121925;
					}
					#endif
				}
			}
			
		}
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 end*/
		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
						:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121923","������ʾδ����");
			return 121923;
		}
    /*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFunc"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v3,1,2)"
			"   AND sm_code = :v4"
			"   AND function_code = :v5");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121924","������ʾδ����");
			return 121924;
		}*/
		Trim(idNo);
		EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE, :opTime into :sTdCustFunc 
	  	FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code ;
	  if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121924","������ʾδ����");
			return 121924;		
		}
		iRetCode = 0; 		 		
    iRetCode=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddAddFuncs",sTdCustFunc);
		if (iRetCode!=0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121924","������ʾδ����");
			return 121924;
    }

		/*memset(dynStmt, 0 ,sizeof(dynStmt));

		sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121925","������ʾδ����[%s]",idNo);
			return 121925;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME, 'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept,:totalDate, :opTime, :opCode, 'a'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	  if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121925","������ʾδ����");
			return 121925;		
		}
		iRetCode = 0; 					
    iRetCode=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddAddFuncs",sTdCustFuncHis);
		if (iRetCode != 0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121925","������ʾδ����[%s]",idNo);
			return 121925;
    }
    
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAdd"
			"("
			"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
			"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
			"to_date(:v3, 'yyyymmdd hh24:mi:ss'), MONTH_LIMIT, :v4, NULL"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v5,1,2)"
			"   AND sm_code = :v6"
			"   AND function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:add_no, :belongCode,
					:smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121926","������ʾδ����");
			return 121926;
		}*/
		Trim(idNo);
		EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE,	:opTime,
			 :opTime, to_char(MONTH_LIMIT), :add_no, chr(0) into :sTdCustFuncAdd
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code
			 AND month_limit > 0 ;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;		
		}
		iRetCode = 0; 			 	
		iRetCode=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddAddFuncs",sTdCustFuncAdd);
		if (iRetCode != 0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;
    }
    
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
					:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
			 nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),nvl(OTHER_ATTR,chr(0)),
			 :loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a'
			 into :sTdCustFuncAddHis FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;		
		}
		iRetCode = 0; 				 	
		iRetCode=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddAddFuncs",sTdCustFuncAddHis);
		if (iRetCode != 0)
    {    				  
      PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;
    }
    /*ng���� by zhaohx at 04/08/2009 end*/
		sprintf(dynStmt,
			"UPDATE dCustRes"
			"   SET resource_code = '1'"
			" WHERE phone_no = :v");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :add_no;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121928","������ʾδ����");
			return 121928;
		}
		/***����������Դͬ��  20090224 add***/
		init(dynStmt);
		sprintf(dynStmt,
			" INSERT INTO dgoodphonereshis "
            "    (  phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,"
			"		begin_time,end_time,bak_field,district_code,resource_code ,"
			"		update_login,update_code,update_date,update_time,update_type,update_accept )"
			" select phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,"
			"		begin_time,end_time,bak_field,district_code,resource_code ,"
			"		:v1,:v2,to_number(:v3),sysdate,'u',to_number(:v4)"
			"  from  dgoodphoneres"
			" where  phone_no = :v5"
			
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,:opCode,:totalDate,:loginAccept,
											:add_no;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121911","������ʾδ����");
			return 121911;
		}
		
		init(dynStmt);
		sprintf(dynStmt,
			"UPDATE dgoodphoneres"
			"   SET resource_code = '1'"
			" WHERE phone_no = :v1"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :add_no;
		end;
		end-exec;
		if (SQLCODE != 0 && SQLCODE != 1403)
		{
			return 121912;
		}
		/***����������Դͬ��  20090224 EDD***/
		
		iRetCode = fSendCmd(funcEnv, '1', "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","������ʾδ����",iRetCode);
			return iRetCode;
		}

	}
	return 0;
}

/* ���Ӻ���ת���ط� (lixg Add 20031208)*/
int fAddVertFuncs(tFuncEnv *funcEnv, char *func_code, char *phone_no)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	EXEC SQL BEGIN DECLARE SECTION;
	int		iCount=0;
	int ret =0;
	char	dynStmt[1024];
	char 	funcCode[2+1];

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char vGroupId[10+1];
	char	opCode[4+1];
	char vOrgId[10+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	tsphoneNo[15+1];
	char	orgCode[9+1];
	char	hlrCode[1+1];
	char	imsiNo[20+1];
	char	simNo[20+1];
	
	TdCustFunc	sTdCustFunc;
	TdCustFuncAdd	sTdCustFuncAdd;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	TdCustFuncAddIndex oldIndex;
	TdCustFuncAddIndex newIndex;
	TdCustFuncAddHis	sTdCustFuncAddHis;
	TdCustFuncHis	sTdCustFuncHis;
	char vFunctionType[20+1];
	int count=0;
	EXEC SQL END DECLARE SECTION;

	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(tsphoneNo);
	Sinitn(orgCode);
	Sinitn(funcCode);
	Sinitn(hlrCode);
	Sinitn(imsiNo);
	Sinitn(simNo);
	Sinitn(vOrgId);
	Sinitn(vGroupId);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	strcpy(hlrCode, funcEnv->hlrCode);
	strcpy(imsiNo, funcEnv->imsiNo);
	strcpy(simNo, funcEnv->simNo);


	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	strcpy(tsphoneNo, phone_no); Trim(tsphoneNo);
	memcpy(funcCode, func_code, sizeof(funcCode)-1);
	
	
	 /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
			  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
      /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret =0;

       ret = sGetUserGroupid(phoneNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_DBErr(_FFL_,"","200920","��ȡ�û�group_idʧ��");
         	  return 200920; 
         	}
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/

	memset(dynStmt, '\0', sizeof(dynStmt));
	sprintf(dynStmt,
		"INSERT INTO wCustFuncDay"
		"("
		"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
		"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
		")"
		"VALUES"
		"("
		"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
		"	:v5,	:v6,		to_number(:v7)"
		")");
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :idNo, :funcCode, :totalDate, :opTime,
			:opCode, :loginNo,	:loginAccept;
	end;
	end-exec;
	if (SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124040","������ʾδ����idNo[%s]",idNo);
		return 124040;
	}

	iCount = 0;
	EXEC SQL select count(*) into :iCount  from dCustFunc where
			 id_no = to_char(:idNo) and function_code = :funcCode;
	if(iCount == 0)
	{
	/*ng���� by zhaohx at 04/08/2009 begin*/	
	/*memset(dynStmt, '\0', sizeof(dynStmt));
	sprintf(dynStmt,
		"INSERT INTO dCustFunc"
		"("
		"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
		")"
		"SELECT"
		"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
		"  FROM sFuncList"
		" WHERE region_code=substr(:v3,1,2)"
		"   AND sm_code = :v4"
		"   AND function_code = :v5");
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :idNo, :opTime, :belongCode, :smCode, :funcCode;
	end;
	end-exec;
	if (SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124041","������ʾδ����idNo[%s]",idNo);
		return 124041;
	}*/
	Trim(idNo);
	EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE, :opTime  into :sTdCustFunc 
	 	  FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :funcCode ;
	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","124041","������ʾδ����");
			return 124041;		
		}
	ret = 0; 		 		
  ret=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",sTdCustFunc);
	if (ret!=0)
  {    				  
    PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124041","������ʾδ����idNo[%s]",idNo);
		return 124041;
  }
  
	/*memset(dynStmt, '\0', sizeof(dynStmt));
	sprintf(dynStmt,
		"INSERT INTO dCustFuncHis"
		"("
		"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
		"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
		")"
		"SELECT"
		"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
		"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
		"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7"
		);
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
		:idNo, :funcCode;
	end;
	end-exec;
	if (SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124042","������ʾδ����");
		return 124042;
	}*/
	Trim(idNo);
	EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept ,:totalDate, :opTime, :opCode,'a'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :funcCode ;
	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","124042","������ʾδ����");
			return 124042;		
		}
	ret = 0; 				
  ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",sTdCustFuncHis);
	if (ret != 0)
  {    				  
    PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124042","������ʾδ����");
		return 124042;
  }
	/*ng���� by zhaohx at 04/08/2009 end*/ 
	}
  
	iCount = 0;
	EXEC SQL select count(*) into :iCount  from dCustFuncAdd where
			 id_no = to_char(:idNo) and function_code = :funcCode;

	if(iCount == 0)
	{
  /*ng���� by zhaohx at 04/08/2009 begin*/
	/*memset(dynStmt, '\0', sizeof(dynStmt));
	sprintf(dynStmt,
		"INSERT INTO dCustFuncAdd"
		"("
		"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
		"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
		")"
		"SELECT"
		"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
		"to_date(:v3, 'yyyymmdd hh24:mi:ss'), MONTH_LIMIT, 'Z'||:v4, NULL"
		"  FROM sFuncList"
		" WHERE region_code=substr(:v5,1,2)"
		"   AND sm_code = :v6"
		"   AND function_code = :v7"
		);
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:tsphoneNo, :belongCode,
					:smCode, :funcCode;
	end;
	end-exec;
	if (SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124043","������ʾδ����");
		return 124043;
	}*/
	Trim(idNo);
	EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE, :opTime,
			 :opTime, to_char(MONTH_LIMIT), 'Z'||:tsphoneNo, chr(0) into :sTdCustFuncAdd
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :funcCode;
	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","124043","������ʾδ����");
			return 124043;		
		}
	ret = 0; 						 	
		ret=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",sTdCustFuncAdd);
		if (ret != 0)
    {    				  
      PUBLOG_DBDEBUG("");
		  pubLog_DBErr(_FFL_,"","124043","������ʾδ����");
		  return 124043;
    }
  
	/*memset(dynStmt, '\0', sizeof(dynStmt));
	sprintf(dynStmt,
		"INSERT INTO dCustFuncAddHis"
		"("
		"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
		"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
		")"
		"SELECT"
		"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
		"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
		"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :funcCode;
	end;
	end-exec;
	if (SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
		return 121927;
	}*/
	Trim(idNo);
	EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
	     nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),
			 nvl(OTHER_ATTR,chr(0)),:loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a'
			 into :sTdCustFuncAddHis FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :funcCode;
	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;		
		}
	ret = 0; 						 	
	ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",sTdCustFuncAddHis);
	if (ret != 0)
  {    				  
    PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
		return 121927;
  }
	/*ng���� by zhaohx at 04/08/2009 end*/ 
	}
	else
	{
		memset(dynStmt, '\0', sizeof(dynStmt));
		/*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt, "update dCustFuncAdd set add_no = 'Z'||:v1  "
					" where id_no = to_number(:v2) and function_code = :v3");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :tsphoneNo, :idNo,	:funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","124094","������ʾδ����");
			return 124094;
		}*/
		Trim(idNo);
		Trim(tsphoneNo);
		count=0;
		sprintf(dynStmt,"select FUNCTION_TYPE from dCustFuncAdd WHERE id_no = to_number(:v1) AND function_code = :v2 ");
		EXEC SQL PREPARE sql_str FROM :dynStmt;
  	EXEC SQL declare ngcur07 cursor for sql_str;
  	EXEC SQL open ngcur07 using :idNo, :funcCode;    
  	for(;;)
  	{ 
  		
  		init(vFunctionType);	
  		init(v_parameter_array);
  		memset(&oldIndex,0,sizeof(oldIndex));
  		memset(&newIndex,0,sizeof(newIndex));
  		EXEC SQL FETCH ngcur07 into :vFunctionType;
  		if((0!=SQLCODE)&&(1403!=SQLCODE))
  		{					
  			PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","124094","��ѯdCustFuncAdd����!");
				EXEC SQL CLOSE ngcur07;
				return 124094;
  		}
  		if(1403==SQLCODE) break;
  		
  		Trim(vFunctionType);
  		 
  		strcpy(v_parameter_array[0],tsphoneNo);
    	strcpy(v_parameter_array[1],idNo);     
    	strcpy(v_parameter_array[2],vFunctionType);
    	strcpy(v_parameter_array[3],funcCode);
    	
    	strcpy(oldIndex.sIdNo, idNo);                
    	strcpy(oldIndex.sFunctionType , vFunctionType);  
    	strcpy(oldIndex.sFunctionCode , funcCode); 
    	
    	strcpy(newIndex.sIdNo, idNo);                
    	strcpy(newIndex.sFunctionType , vFunctionType);  
    	strcpy(newIndex.sFunctionCode , funcCode);  
			        				
    	ret=OrderUpdateCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",oldIndex,newIndex,
    	    " add_no = 'Z'||:tsphoneNo ","",v_parameter_array);
			if (ret != 0)
    	{    				  
    	  PUBLOG_DBDEBUG("");
			  pubLog_DBErr(_FFL_,"","124094","������ʾδ����");
			  EXEC SQL CLOSE ngcur07;
			  return 124094;
    	}
    	count++;
    }
    EXEC SQL CLOSE ngcur07;  
    if (count==0)
    {  
    	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","124094","������ʾδ����");
			return 124094;
    }
    
		
		
		/*memset(dynStmt, '\0', sizeof(dynStmt));
		sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'u'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
				:idNo, :funcCode;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121995","������ʾδ����");
			return 121995;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE, nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		   nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),
			 nvl(OTHER_ATTR,chr(0)),:loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a'
			 into :sTdCustFuncAddHis FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :funcCode;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;		
		}
	  ret = 0; 					 	
		ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddVertFuncs",sTdCustFuncAddHis);
		if (ret != 0)
  	{    				  
  	  PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121927","������ʾδ����");
			return 121927;
  	}
		/*ng���� by zhaohx at 04/08/2009 end*/
	}

	memset(dynStmt, '\0', sizeof(dynStmt));
	/*��֯�����������ӱ��ֶ�edit by wanfh at 09/02/2009*/
	sprintf(dynStmt,"insert into wSndCmdDay "
		"("
		"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,GROUP_ID,"
		"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
		"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE,ORG_ID,LOGIN_ACCEPT,"
		"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE"
		")"
		" select "
		" sOffOn.nextval, :v1, 0, to_number(:v2), :v3, :vGroupId,"
		" :v4, :v5, :v6, :v7, :v8, :v9,"
		" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14,:vOrgId, to_number(:v15), "
		" to_date(:v16, 'yyyymmdd hh24:mi:ss'), '1', '0', to_date(:v17, 'yyyymmdd hh24:mi:ss'),:v18 "
		" from dual ");
	exec sql execute
	begin
		EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,:vGroupId,
			:smCode, :phoneNo, :tsphoneNo, :imsiNo,:imsiNo, :simNo,
			:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
			:opTime, :opTime, :funcCode;
	end;
	end-exec;
	if(SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","124044","������ʾδ����");
		return 124044;
	}

	return 0;
}

/* FuncName: checkFuncCode
* @pBelongCode: ��������
* @pFuncCode:   �ط�����
* @pSmCode:     SIM ����
*
* Return: 0: Sucess, 121900: Fail
*/
int checkFuncCode(char *pIdNo,char opType, char *pBelongCode,char *pFuncCode, char *pSmCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char regionCode[2 + 1];
	char funcCode	[2 + 1];
	char smCode		[2 + 1];
	char idNo		[22 + 1];
	int  iCount = 0;
	int  iHasExpire = 0;
	EXEC SQL END DECLARE SECTION;

	memset(regionCode, '\0', sizeof(regionCode));
	memset(funcCode, '\0', sizeof(funcCode));
	memset(smCode, '\0', sizeof(smCode));
	memset(idNo, '\0', sizeof(idNo));

	memcpy(regionCode,pBelongCode,sizeof(regionCode)-1);
	memcpy(smCode,pSmCode,sizeof(smCode)-1);
	memcpy(funcCode,pFuncCode,sizeof(funcCode)-1);
	strcpy(idNo, pIdNo);

	EXEC SQL select count(*) into :iCount 
		from  sFuncList 
		where  region_code = :regionCode
		and    sm_code = :smCode
		and    function_code = :funcCode;

	if(iCount == 0) 
	{
		pubLog_DBErr(_FFL_,"","121910","������ʾδ����regionCode[%s]smcode[%s]funccode[%s]",regionCode,smCode,funcCode);
		return 121910;
	}

	iCount = 0;
	
	EXEC SQL select count(*) INTO :iCount from dCustFunc
		where  id_no = to_number(:idNo)
		and    function_code = :funcCode;
 
	switch(opType)
	{
	case 'a':
		if(iCount != 0)
		{
			pubLog_DBErr(_FFL_,"","121900","������ʾδ����idNo[%s]funccode[%s]",idNo,funcCode);
			return 121900;
		}
		break;
	case 'u':
	case 'd':
		if(iCount == 0)
		{
			pubLog_Debug(_FFL_,"","���Դ�ӡ","idNo = %s,funccode =%s",idNo,funcCode);
			EXEC SQL select count(*) into :iHasExpire from wCustExpire
					 where  id_no = to_number(:idNo) 
					 and    function_code = :funcCode;
			if (iHasExpire == 0)
			{	
				pubLog_DBErr(_FFL_,"","121900","������ʾδ����");
				return 121901;
			}
		}
		break;
	default:
		{
		pubLog_DBErr(_FFL_,"","121902","������ʾδ����idNo[%s]funccode[%s]",idNo,funcCode);
		return 121900;
		}	
	}
	return 0;
}

int fSendCmd(tFuncEnv *funcEnv, char bStat, char *opCode, char *funcCode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;

	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char	dynStmtnew[1024];
	char	idNo[22 + 1];
	int   ret =0;
	char	totalDate[8+1];
	char	opTime[17+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char vOrgId[10+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char vGroupId[10+1];
	char    phoneNo1[15+1];
	char	orgCode[9+1];
	char	hlrCode[1+1];
	char	imsiNo[20+1];
	char	simNo[20+1];
	char	funcCode1[2+1];
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	int iCount = 0;
	char vNewCmdStr[15 + 1];
	/*Modify at 2012.03.19 end*/

	char	businessStat[1+1];
	int		vcount=0,retValue=0;

	EXEC SQL END DECLARE SECTION;
	
	Sinitn(phoneNo1);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(orgCode);
	Sinitn(hlrCode);
	Sinitn(imsiNo);
	Sinitn(simNo);
	Sinitn(funcCode1);
	Sinitn(vGroupId);
	Sinitn(vOrgId);
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.04.17 end*/
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	strcpy(hlrCode, funcEnv->hlrCode);
	strcpy(imsiNo, funcEnv->imsiNo);
	strcpy(simNo, funcEnv->simNo);
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify for at 2012.04.17 end*/
	transIN = funcEnv->transIN;
	transOUT = funcEnv->transOUT;

	sprintf(businessStat, "%c",bStat);
	businessStat[1] = '\0';
	
	 /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       printf("funcEnv->phoneNo=[%s]\n",funcEnv->phoneNo);
       ret = sGetUserGroupid(phoneNo,vGroupId);
       if(ret <0)
         	{
         	  PUBLOG_DBDEBUG("");
         	  printf("phoneNo=[%s]\n",phoneNo);
         	  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�group_idʧ��");
         	  return 200919; 
         	}
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
     
     /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
         	  pubLog_DBErr(_FFL_,"","200920","��ȡ����org_idʧ��");
         	  return 200920; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/

	memset(dynStmt, '\0', sizeof(dynStmt));
	memset(dynStmtnew, '\0', sizeof(dynStmtnew));
	/*��֯�����������ӱ��ֶ�edit by wanfh at 09/02/2009*/
	/*20090609mengfanyuҪ�������������ط������Ӵ��ж�*/
	
	sprintf(dynStmtnew,"insert into wSndCmdDay "
		"("
		"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,GROUP_ID,"
		"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
		"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE,ORG_ID, LOGIN_ACCEPT,"
		"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE"
		")"
		" select "
		" sOffOn.nextval, :v1, 0, to_number(:v2), :v3,:vGroupId, "
		" :v4, :v5, :v6, :v7, '20500101', :v8||substr(:v9,1,2),"
		" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14,:vOrgId,to_number(:v15), "
		" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code "
		" from sFuncList "
		" where region_code = substr(:v19, 1,2)"
		"   and sm_code = :v20"
		"   and function_code = :v21"
		"   and command_Code <> 'zz'");
		
	sprintf(dynStmt,"insert into wSndCmdDay "
		"("
		"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,GROUP_ID,"
		"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
		"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE,ORG_ID, LOGIN_ACCEPT,"
		"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE"
		")"
		" select "
		" sOffOn.nextval, :v1, 0, to_number(:v2), :v3,:vGroupId, "
		" :v4, :v5, :v6, :v7, :v8, :v9,"
		" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14,:vOrgId,to_number(:v15), "
		" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),command_code "
		" from sFuncList "
		" where region_code = substr(:v19, 1,2)"
		"   and sm_code = :v20"
		"   and function_code = :v21"
		"   and command_Code <> 'zz'");
	if(strstr(funcCode,"41"))
	{
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmtnew USING :hlrCode, :idNo, :belongCode,:vGroupId,
				:smCode, :phoneNo, :phoneNo, :totalDate, :totalDate,:belongCode,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121999","������ʾδ����idNo[%s]",idNo);
			return 121999;
		}
	}
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	else if(strstr(funcCode,"02"))
	{
		EXEC SQL SELECT COUNT(1)
							into : iCount
							FROM dcustfunc a
								WHERE a.id_no = :idNo
									AND a.function_code = '87';
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121993","������ʾδ����idNo[%s]",idNo);
			return 121993;
		}	
		if(iCount == 0)
		{
			if(strcmp(businessStat,"1")==0)
			{
				strcpy(vNewCmdStr,"BOS1");
			}
			else
			{
				strcpy(vNewCmdStr,"N");
			}
		}
		else
		{
			if(strcmp(businessStat,"1")==0)
			{
				strcpy(vNewCmdStr,"BOS1&BOS3");
			}
			else
			{
				strcpy(vNewCmdStr,"BOS3");
			}
		}
		
		exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,:vGroupId,
					:smCode, :phoneNo, :phoneNo, :imsiNo,:vNewCmdStr, :simNo,
					:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
					:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121999","������ʾδ����idNo[%s]",idNo);
			return 121999;
		}							
	}
	else if(strstr(funcCode,"87"))
	{
		EXEC SQL SELECT COUNT(1)
							into : iCount
							FROM dcustfunc a
								WHERE id_no = :idNo
									AND function_code = '02';
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121993","������ʾδ����idNo[%s]",idNo);
			return 121993;
		}	
		if(iCount == 0)
		{
			if(strcmp(businessStat,"1")==0)
			{
				strcpy(vNewCmdStr,"BOS3");
			}
			else
			{
				strcpy(vNewCmdStr,"N");
			}
		}
		else
		{
			if(strcmp(businessStat,"1")==0)
			{
				strcpy(vNewCmdStr,"BOS1&BOS3");
			}
			else
			{
				strcpy(vNewCmdStr,"BOS1");
			}
		}
		
		exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,:vGroupId,
					:smCode, :phoneNo, :phoneNo, :imsiNo,:vNewCmdStr, :simNo,
					:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
					:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121999","������ʾδ����idNo[%s]",idNo);
			return 121999;
		}							
	}
	/*Modify for at 2012.04.17 end*/
	else 
	{
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,:vGroupId,
				:smCode, :phoneNo, :phoneNo, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121999","������ʾδ����idNo[%s]",idNo);
			return 121999;
		}
	}
	
	if((bStat=='1') && strstr(funcCode,"24"))
	{
		strcpy(funcCode1,"61");
		sprintf(phoneNo1,"43%c12580",idNo[0]);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode, :vGroupId,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode, :vOrgId, :loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;
	}
	if((bStat=='1') && strstr(funcCode,"48"))
	{
		strcpy(funcCode1,"61");
		strcpy(phoneNo1,"13800431166");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode, :vGroupId,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode, :vOrgId,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;
	}
	if((bStat=='1') && (strstr(funcCode,"95")||strstr(funcCode,"41")))
	{
		Sinitn(funcCode1);
		strcpy(funcCode1,"05");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode, :vGroupId,
				:smCode, :phoneNo, :phoneNo, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode, :vOrgId, :loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;
		
		Sinitn(funcCode1);
		if(strstr(funcCode,"95"))
		{
			strcpy(phoneNo1,"13800431309");
			strcpy(funcCode1,"61");		
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode, :vGroupId,
					:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
					:opCode, :totalDate, :opTime,:loginNo,:orgCode, :vOrgId, :loginAccept,
					:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
			end;
			end-exec;
		}
		/*
		Sinitn(funcCode1);
		strcpy(funcCode1,"62");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;
		*/
		/*Sinitn(funcCode1);
		strcpy(funcCode1,"63");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;*/
	}
	if((bStat=='0') && strstr(funcCode,"95"))
	{
		strcpy(phoneNo1,"13800431309");
		Sinitn(funcCode1);
		strcpy(funcCode1,"61");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode, :vGroupId,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode, :vOrgId, :loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;
		
		/*Sinitn(funcCode1);
		strcpy(funcCode1,"63");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,
				:smCode, :phoneNo, :phoneNo1, :imsiNo,:imsiNo, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:loginAccept,
				:opTime, :businessStat,:opTime, :belongCode, :smCode, :funcCode1;
		end;
		end-exec;*/
	}
	if((bStat=='1') && strstr(funcCode,"03"))
	{	
		vcount=0;
		EXEC SQL SELECT COUNT(*)
	         INTO : vcount
	         FROM  DCustFaze
	         WHERE phone_no=:phoneNo  and  faze_type='2';
	    if(SQLCODE != 0 && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121976","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121976;
		}
		if(vcount>0)
		{
			pubLog_DBErr(_FFL_,"","121977","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121977;
		}
		vcount=0;
		EXEC SQL SELECT COUNT(*)
	         INTO : vcount
	         FROM  dFTPControlShortMsg
	         WHERE phone_no=:phoneNo  and  flag='1';
	   if(SQLCODE != 0 && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121979","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121979;
		}
	    if(vcount>0)
		{
			pubLog_DBErr(_FFL_,"","121980","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121980;
		}
	}
	/*Add for ��������@2012_0070 by liujj at 2012.09.11 begin*/
	if((bStat=='1') && strstr(funcCode,"55"))
	{	
		vcount=0;
		EXEC SQL SELECT COUNT(*)
	         INTO : vcount
	         FROM  DCustFaze
	         WHERE phone_no=:phoneNo  and  faze_type='4';
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121982","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121982;
		}
		if(vcount>0)
		{
			pubLog_DBErr(_FFL_,"","121983","������ʾδ����DCustFaze bStat[%s]funcCode[%s]",businessStat,funcCode);
			return 121983;
		}
		/*Add for ��������@2012_0070 by liujj at 2012.09.11 end*/
	}
	return 0;
}

/****
funcname:DealOPtUser
****/
int DealOPtUser(char *vidNo,char *vopType, char *vbelongCode,char *vphoneNo,char *vloginAccept)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	phoneNo[15+1];
	long	idNo=0;
	char	regionCode[2+1];
	char	opType[1+1];
	char	optCode[1+1];
	char	optList[36+1];
	int		icount=0;
	char	loginAccept[22+1];
	char	newoptList[36+1];
	int		k=0,n=0,ret=0;
	char	optTmp[1+1];
	char	opTime[14+1];
	
	TdIngwUserIndex oldIndex;
  TdIngwUserIndex newIndex;
	char  TmpSqlStr[2048];
	char  vMsisdn[15+1];
	char  vBeginDate[20+1];
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	TdIngwUser tdIngwUser;
	int count=0;
	EXEC SQL END DECLARE SECTION;
	
	init(phoneNo);
	init(regionCode);
	init(opType);
	init(optCode);
	init(optList);
	init(loginAccept);
	init(newoptList);
	init(optTmp);
	init(opTime);
	
	strcpy(phoneNo,vphoneNo);
	idNo=atol(vidNo);
	strcpy(opType,vopType);
	strcpy(loginAccept,vloginAccept);
	strncpy(regionCode,vbelongCode,2);
	
	EXEC SQL SELECT opt_code,to_char(sysdate,'yyyymmddhh24miss') INTO :optCode,:opTime FROM Singwctrcode WHERE ctr_code='INGW';
	if(SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
		return -1;
	}
	
	if(strcmp(opType,"1") == 0)
	{
		EXEC SQL SELECT count(*) INTO :icount FROM dingwuser WHERE id_no=:idNo AND end_date>sysdate;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-2","������ʾδ����");
			return -2;
		}
		
		if(icount > 0)
		{
			/*chendx ��ס �����������������߳����������ڹ�������Ż������� 20111103
      #ifdef _CHGTABLE_
			EXEC SQL INSERT INTO wingwuserchg(msisdn,opt_list,begin_date,end_date,flag,deal_time)
					 SELECT msisdn,opt_list,begin_date,to_date(:opTime,'yyyy-mm-dd hh24:mi:ss'),'2',sysdate
					 FROM dingwuser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-3","������ʾδ����phoneNo[%s]",phoneNo);
				return -3;
			}
      #endif*/
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL UPDATE dingwuser SET opt_list=trim(opt_list)||:optCode,begin_date=to_date(:opTime,'yyyy-mm-dd hh24:mi:ss')
					 WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-4","������ʾδ����phoneNo[%s]",phoneNo);
				return -4;
			}*/
			count=0;
			Trim(vidNo);
			init(TmpSqlStr);
			sprintf(TmpSqlStr,"select msisdn, begin_date from dingwuser where id_no=:v1 AND end_date>sysdate ");
			EXEC SQL PREPARE sql_str FROM :TmpSqlStr;
  		EXEC SQL declare ngcur01 cursor for sql_str;
  		EXEC SQL open ngcur01 using :idNo;
  		for(;;)
  		{ 
  			init(vMsisdn);	
    		init(vBeginDate); 
  			init(v_parameter_array);
  			memset(&oldIndex,0,sizeof(oldIndex));
    	  memset(&newIndex,0,sizeof(newIndex));
  			EXEC SQL FETCH ngcur01 into :vMsisdn,:vBeginDate;
  			if((0!=SQLCODE)&&(1403!=SQLCODE))
  			{					
  				PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","-4","��ѯdingwuser����!");
					EXEC SQL CLOSE ngcur01;
					return -4;
  			}
  			if(1403==SQLCODE) break;
  			
  			Trim(vMsisdn);
  			Trim(vBeginDate);
  			 
    		strcpy(v_parameter_array[0],optCode);     
    		strcpy(v_parameter_array[1],opTime);
    		strcpy(v_parameter_array[2],vMsisdn);
    		strcpy(v_parameter_array[3],vBeginDate);
    		strcpy(v_parameter_array[4],vidNo);
    		
    		strcpy(oldIndex.sMsisdn , vMsisdn);                
    		strcpy(oldIndex.sBeginDate , vBeginDate);  
    		
    		strcpy(newIndex.sMsisdn , vMsisdn);                
    		strcpy(newIndex.sBeginDate , opTime);        
				        				
    		ret=OrderUpdateIngwUser(ORDERIDTYPE_USER,vidNo,100,"",atol(vloginAccept),"","DealOPtUser",oldIndex,newIndex,
    		    " opt_list=trim(opt_list)||:optCode,begin_date=to_date(:opTime,'yyyy-mm-dd hh24:mi:ss') ",
    		    " and id_no=:vidNo AND end_date>sysdate ",v_parameter_array);
				if (ret != 0)
    		{    				  
    		  PUBLOG_DBDEBUG("");
				  pubLog_DBErr(_FFL_,"","-4","������ʾδ����phoneNo[%s]",phoneNo);
				  EXEC SQL CLOSE ngcur01;
				  return -4;
    		}
    		count++;
    	}
    	EXEC SQL CLOSE ngcur01;   	
    	if (count==0)
    	{
    		PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-4","������ʾδ����phoneNo[%s]",phoneNo);
				return -4;
    	}
			/*ng���� by zhaohx at 04/08/2009 end*/ 
			/*chendx ��ס �����������������߳����������ڹ�������Ż������� 20111103
			#ifdef _CHGTABLE_
			EXEC SQL INSERT INTO wingwuserchg(msisdn,opt_list,begin_date,end_date,flag,deal_time)
					 SELECT msisdn,opt_list,begin_date,end_date,'1',sysdate
					 FROM dingwuser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-5","������ʾδ����phoneNo[%s]",phoneNo);
				return -5;
			}
			#endif*/
			EXEC SQL INSERT INTO dIngwUserHis(ID_no,Msisdn,Region_code,Opt_list,Begin_date,
									End_date,Prepay_fee,Update_time,Update_accept,
									Update_login,Update_code)
						 SELECT ID_no,Msisdn,Region_code,Opt_list,Begin_date,
								End_date,Prepay_fee,sysdate,to_number(:loginAccept),'system','u'
						 FROM dIngwUser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-4","������ʾδ����phoneNo[%s]",phoneNo);
				return -6;
			}
		}
		else
		{
			strcpy(optList,optCode);
			 /*��ṹδ��*/
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL INSERT INTO dingwuser
						(ID_no,Msisdn,Region_code,Opt_list,Begin_date,End_date,Prepay_fee)
					VALUES
						(:idNo,:phoneNo,:regionCode,:optList,to_date(:opTime,'yyyy-mm-dd hh24:mi:ss'),to_date('20500101','yyyy-mm-dd'),0);
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-7","������ʾδ����phoneNo[%s]",phoneNo);
				return -7;
			}*/
			memset(&tdIngwUser,0,sizeof(tdIngwUser));
			Trim(vidNo);
			EXEC SQL select :vidNo,:phoneNo,:regionCode,:opTime,'20500101',0,:optList into :tdIngwUser from dual;
			if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-7","������ʾδ����");
			return -7;		
		}
	    ret = 0; 					   	
		  ret=OrderInsertIngwUser(ORDERIDTYPE_USER,vidNo,100,"",atol(vloginAccept),"","DealOPtUser",tdIngwUser);
			if (ret != 0)
    	{    				  
    	  PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-7","������ʾδ����phoneNo[%s]",phoneNo);
				return -7;
    	}
			/*ng���� by zhaohx at 04/08/2009 end*/
			EXEC SQL INSERT INTO dIngwUserHis(ID_no,Msisdn,Region_code,Opt_list,Begin_date,
									End_date,Prepay_fee,Update_time,Update_accept,
									Update_login,Update_code)
						 SELECT ID_no,Msisdn,Region_code,Opt_list,Begin_date,
								End_date,Prepay_fee,sysdate,to_number(:loginAccept),'system','a'
						 FROM dingwuser WHERE id_no=:idNo;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-8","������ʾδ����phoneNo[%s]",phoneNo);
				return -8;
			}
			/*chendx ��ס �����������������߳����������ڹ�������Ż������� 20111103
			#ifdef _CHGTABLE_
			EXEC SQL INSERT INTO wingwuserchg
						(msisdn,opt_list,begin_date,end_date,flag,deal_time)
					VALUES
						(:phoneNo,:optList,to_date(:opTime,'yyyy-mm-dd hh24:mi:ss'),to_date('20500101','yyyy-mm-dd'),'1',sysdate);
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-9","������ʾδ����phoneNo[%s]",phoneNo);
				return -9;
			}
			#endif*/
		}
	}
	else
	{
		EXEC SQL SELECT opt_list INTO :optList FROM dIngwUser WHERE id_no=:idNo;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-10","������ʾδ����phoneNo[%s]",phoneNo);
			return -10;
		}
		/*chendx ��ס �����������������߳����������ڹ�������Ż������� 20111103
		#ifdef _CHGTABLE_
		EXEC SQL INSERT INTO wingwuserchg(msisdn,opt_list,begin_date,end_date,flag,deal_time)
					 SELECT msisdn,opt_list,begin_date,to_date(:opTime,'yyyy-mm-dd hh24:mi:ss'),'2',sysdate
					 FROM dingwuser WHERE id_no=:idNo AND end_date>sysdate;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-11","������ʾδ����phoneNo[%s]",phoneNo);
			return -11;
		}
		#endif*/
		for(k=0;k<36;k++)
		{
			init(optTmp);
			strncpy(optTmp,optList+k,1);
			if(strlen(optTmp) == 0) break;
			if(strncmp(optTmp,optCode,1) != 0)
			{
				sprintf(newoptList,"%s%s",newoptList,optTmp);
			}
		}
		
		Trim(newoptList);
		if(strlen(newoptList) == 0)
		{
			EXEC SQL INSERT INTO dIngwUserHis(ID_no,Msisdn,Region_code,Opt_list,Begin_date,
									End_date,Prepay_fee,Update_time,Update_accept,
									Update_login,Update_code)
						 SELECT ID_no,Msisdn,Region_code,Opt_list,Begin_date,
								sysdate,Prepay_fee,sysdate,to_number(:loginAccept),'system','d'
						 FROM dIngwUser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-12","������ʾδ����phoneNo[%s]",phoneNo);
				return -12;
			}
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL delete dIngwUser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-12","������ʾδ����phoneNo[%s]",phoneNo);
				return -12;
			}*/
			count=0;
			init(TmpSqlStr);
			Trim(vidNo);
			sprintf(TmpSqlStr,"select msisdn, begin_date from dingwuser where id_no=:v1 AND end_date>sysdate ");
			EXEC SQL PREPARE sql_str FROM :TmpSqlStr;
  		EXEC SQL declare ngcur02 cursor for sql_str;
  		EXEC SQL open ngcur02 using :idNo;
  		for(;;)
  		{ 
  			init(vMsisdn);	
    		init(vBeginDate); 
  			init(v_parameter_array);
  			memset(&oldIndex,0,sizeof(oldIndex));
  			EXEC SQL FETCH ngcur02 into :vMsisdn,:vBeginDate;
  			if((0!=SQLCODE)&&(1403!=SQLCODE))
  			{					
  				PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","-4","��ѯdingwuser����!");
					EXEC SQL CLOSE ngcur02;
					return -12;
  			}
  			if(1403==SQLCODE) break;
  			
  			Trim(vMsisdn);
  			Trim(vBeginDate);

    		strcpy(v_parameter_array[0],vMsisdn);
    		strcpy(v_parameter_array[1],vBeginDate);
    		strcpy(v_parameter_array[2],vidNo);
    		
    		strcpy(oldIndex.sMsisdn , vMsisdn);                
    		strcpy(oldIndex.sBeginDate , vBeginDate);       
				        				
    		ret=OrderDeleteIngwUser(ORDERIDTYPE_USER,vidNo,100,"",atol(vloginAccept),"","DealOPtUser",oldIndex,
    		    " and id_no=:vidNo AND end_date>sysdate ",v_parameter_array);
				if (ret != 0)
    		{    				  
    		  PUBLOG_DBDEBUG("");
				  pubLog_DBErr(_FFL_,"","-12","������ʾδ����phoneNo[%s]",phoneNo);
				  EXEC SQL CLOSE ngcur02;
				  return -12;
    		}
    		count++;
    	}
    	EXEC SQL CLOSE ngcur02;   
    	if (count==0)
    	{
    		PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-12","������ʾδ����phoneNo[%s]",phoneNo);
				return -12;
    	}	
			/*ng���� by zhaohx at 04/08/2009 end*/
		}
		else
		{
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL UPDATE dingwuser SET opt_list=:newoptList,begin_date=to_date(:opTime,'yyyy-mm-dd hh24:mi:ss')
					 WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-13","������ʾδ����phoneNo[%s]",phoneNo);
				return -13;
			}*/
			count=0;
			init(TmpSqlStr);
			Trim(vidNo);
			sprintf(TmpSqlStr,"select msisdn, begin_date from dingwuser where id_no=:v1 AND end_date>sysdate ");
			EXEC SQL PREPARE sql_str FROM :TmpSqlStr;
  		EXEC SQL declare ngcur03 cursor for sql_str;
  		EXEC SQL open ngcur03 using :idNo;
  		for(;;)
  		{ 
  			init(vMsisdn);	
    		init(vBeginDate); 
  			init(v_parameter_array);
  			memset(&oldIndex,0,sizeof(oldIndex));
    	  memset(&newIndex,0,sizeof(newIndex));
  			EXEC SQL FETCH ngcur03 into :vMsisdn,:vBeginDate;
  			if((0!=SQLCODE)&&(1403!=SQLCODE))
  			{					
  				PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","-4","��ѯdingwuser����!");
					EXEC SQL CLOSE ngcur03;
					return -13;
  			}
  			if(1403==SQLCODE) break;
  			
  			Trim(vMsisdn);
  			Trim(vBeginDate);
  			 
    		strcpy(v_parameter_array[0],newoptList);     
    		strcpy(v_parameter_array[1],opTime);
    		strcpy(v_parameter_array[2],vMsisdn);
    		strcpy(v_parameter_array[3],vBeginDate);
    		strcpy(v_parameter_array[4],vidNo);
    		
    		strcpy(oldIndex.sMsisdn , vMsisdn);                
    		strcpy(oldIndex.sBeginDate , vBeginDate);  
    		
    		strcpy(newIndex.sMsisdn , vMsisdn);                
    		strcpy(newIndex.sBeginDate , opTime);        
				        				
    		ret=OrderUpdateIngwUser(ORDERIDTYPE_USER,vidNo,100,"",atol(vloginAccept),"","DealOPtUser",oldIndex,newIndex,
    		    " opt_list=:newoptList,begin_date=to_date(:opTime,'yyyy-mm-dd hh24:mi:ss') ",
    		    " and id_no=:vidNo AND end_date>sysdate ",v_parameter_array);
				if (ret != 0)
    		{    				  
    			  PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","-13","������ʾδ����phoneNo[%s]",phoneNo);
						EXEC SQL CLOSE ngcur03;
						return -13;
    		}
    	  count++;
    	}
    	EXEC SQL CLOSE ngcur03; 
    	if(count==0)
    	{
    		PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-13","������ʾδ����phoneNo[%s]",phoneNo);
				return -13;
    	}  	
			/*ng���� by zhaohx at 04/08/2009 end*/ 
			/*chendx ��ס �����������������߳����������ڹ�������Ż������� 20111103
			#ifdef _CHGTABLE_
			EXEC SQL INSERT INTO wingwuserchg(msisdn,opt_list,begin_date,end_date,flag,deal_time)
					 SELECT msisdn,opt_list,begin_date,end_date,'1',sysdate
					 FROM dingwuser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-14","������ʾδ����phoneNo[%s]",phoneNo);
				return -14;
			}
			#endif*/
			EXEC SQL INSERT INTO dIngwUserHis(ID_no,Msisdn,Region_code,Opt_list,Begin_date,
									End_date,Prepay_fee,Update_time,Update_accept,
									Update_login,Update_code)
						 SELECT ID_no,Msisdn,Region_code,Opt_list,Begin_date,
								end_date,Prepay_fee,sysdate,to_number(:loginAccept),'system','u'
						 FROM dIngwUser WHERE id_no=:idNo AND end_date>sysdate;
			if(SQLCODE != 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","-15","������ʾδ����phoneNo[%s]",phoneNo);
				return -15;
			}
		}
	}
	
	return 0;
}

/* lixg add for delFuncAdd */
int fSendCmdFuncAdd(tFuncEnv *funcEnv, char *opCode, char *funcCode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	int	 iRetCode = 0;

	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char 	idNo[22+1];
	char 	opTime[17+1];

	char	modeCode[8+1];
	char	smCode[2+1];
	char    regionCode[2+1];
	char    tmpFuncCode[2+1];
	char    tmpAddFuncCode[2+1];
	EXEC SQL END DECLARE SECTION;

	transIN = funcEnv->transIN;
	transOUT = funcEnv->transOUT;

	Sinitn(idNo);
	Sinitn(opTime);
	Sinitn(smCode);
	Sinitn(modeCode);
	Sinitn(regionCode);
	Sinitn(tmpFuncCode);
	Sinitn(tmpAddFuncCode);

	memset(dynStmt, '\0', sizeof(dynStmt));
	strcpy(idNo, funcEnv->idNo);
	strcpy(opTime, funcEnv->opTime);
	Trim(idNo);

	sprintf(dynStmt, "select mode_code from dBillCustDetail%c "
		" where  id_no = to_number(:v1) "
		" and mode_flag = '0' and mode_time ='Y' "
		" and begin_time <= to_date(:v2,'yyyymmdd hh24:mi:ss') "
		" and end_time > to_date(:v3,'yyyymmdd hh24:mi:ss') ",
		idNo[strlen(idNo)-1]);

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :dynStmt into :modeCode USING :idNo, :opTime, :opTime;
	END;
	END-EXEC;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","121990","������ʾδ����");
		return 121990;
	}

	memcpy(regionCode,funcEnv->belongCode,sizeof(regionCode)-1);
	memcpy(smCode,funcEnv->smCode,sizeof(smCode)-1);
	memcpy(tmpFuncCode,funcCode,sizeof(tmpFuncCode)-1);

	EXEC SQL DECLARE CUR_LISTADD CURSOR FOR
		select  add_funccode from sFuncListAdd
		where  region_code = :regionCode
		and    sm_code = :smCode
		and    mode_code = :modeCode
		and    function_code = :tmpFuncCode;
	EXEC SQL OPEN CUR_LISTADD;
	Sinitn(tmpAddFuncCode);
	EXEC SQL FETCH CUR_LISTADD into :tmpAddFuncCode;
	while(SQLCODE == 0)
	{
		iRetCode = fSendCmd(funcEnv, '0', opCode, tmpAddFuncCode);
		if(iRetCode != 0)
		{
			break;
		}
		Sinitn(tmpAddFuncCode);
		EXEC SQL FETCH CUR_LISTADD into :tmpAddFuncCode;
	}
	EXEC SQL OPEN CUR_LISTADD;

	return iRetCode;
}

int PublicGetUnbillChg(long in_contract_no,long in_id_no,int in_type,
					   char *in_phone_no,int in_timeout,double *should_pay)
{
	EXEC SQL BEGIN DECLARE SECTION;
	long contract_no = 0;
	long id_no = 0;
	int  type = 0;
	int  timeout = 0;
	int  Port = 0;
	char PhoneNo[PHONENOLEN+1];
	char IP[15+1];
	char DB[32+1];
	char PhoneHead[7+1];
	double out_should_pay = 0.00;
	double out_owe_pay=0.00;
	double out_fav_pay=0.00;
	EXEC SQL END DECLARE SECTION;
	int ret = 0;
	init(PhoneNo);
	init(DB);
	init(PhoneHead);

	pubLog_Debug(_FFL_,"","PublicGetUnbillChg","...begin..."); /*���Դ�ӡ���*/
	contract_no = in_contract_no;
	id_no = in_id_no;
	type = in_type;
	timeout = in_timeout;
	strcpy(PhoneNo,in_phone_no);	
	strncpy(PhoneHead,PhoneNo,7);
	
	if (strncmp(in_phone_no, "10", 2) == 0)
	{
		/*��ʾ��ר��*/
		if(type==1)
		{
			EXEC SQL	SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
						  INTO   :IP,:Port,:DB
						  FROM   sPhoneComm
						 WHERE  PHONE_HEAD = '9001'
						   and    query_type='01';
		}	 
		if(type==0)
		{
			EXEC SQL	SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
						  INTO   :IP,:Port,:DB
						  FROM   sPhoneComm
						 WHERE  PHONE_HEAD = '9001'
						   and    query_type='02';
		}	 
		if (SQLCODE !=0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","130023","������ʾδ����");
			return 130023;
		} 
	}
	else
	{
		if(type==1)
		{
			EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
			INTO   :IP,:Port,:DB
			FROM   sPhoneComm
			WHERE  PHONE_HEAD = :PhoneHead
			and    query_type='01';
		}	 
		if(type==0)
		{
			EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
			INTO   :IP,:Port,:DB
			FROM   sPhoneComm
			WHERE  PHONE_HEAD = :PhoneHead
			and    query_type='02';
		}	 
		if (SQLCODE !=0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","130024","������ʾδ����");
			return 130024;
		}
	}
	
	
	/*if ((strncmp(in_phone_no, "13", 2) == 0)||(strncmp(in_phone_no, "15", 2) == 0)||(strncmp(in_phone_no, "18", 2) == 0))
	{
		if(type==1)
		{
			EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
			INTO   :IP,:Port,:DB
			FROM   sPhoneComm
			WHERE  PHONE_HEAD = :PhoneHead
			and    query_type='01';
		}	 
		if(type==0)
		{
			EXEC SQL SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
			INTO   :IP,:Port,:DB
			FROM   sPhoneComm
			WHERE  PHONE_HEAD = :PhoneHead
			and    query_type='02';
		}	 
		if (SQLCODE !=0)
		{
			return 130023;
		}       
	}
	else
	{
		if(type==1)
		{
			EXEC SQL	SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
						  INTO   :IP,:Port,:DB
						  FROM   sPhoneComm
						 WHERE  PHONE_HEAD = '9001'
						   and    query_type='01';
		}	 
		if(type==0)
		{
			EXEC SQL	SELECT IP_ADDR,COMM_PORT,SERVER_NAME 
						  INTO   :IP,:Port,:DB
						  FROM   sPhoneComm
						 WHERE  PHONE_HEAD = '9001'
						   and    query_type='02';
		}	 
		if (SQLCODE !=0)
		{
			return 130023;
		}       
	}*/
	
	ret = querybill_info(type,contract_no,id_no,PhoneNo,IP,Port,
		timeout,DB,&out_should_pay,&out_owe_pay,&out_fav_pay);
	if (ret < 0)
	{	
		pubLog_DBErr(_FFL_,"","%d","ȡδ���ʻ�������",ret);
		return ret;
	}
	*should_pay = out_should_pay;
	return 0;
}
int GetConUserOweChg(long in_contract_no,long in_id_no,double *out_owe_fee)
{
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 begin --*/      
	/*EXEC SQL BEGIN DECLARE SECTION;
	long   IdNo=0;
	char  TmpSqlStr[TMPLEN+1];
	long  ContractNo=0;
	int   MinYm;
	char  OweTableName[100+1];
	char  PayCode[1+1];
	char  BelongCode[7+1];
	int   delay_flag=0;
	double delay_rate=0.00;
	int    delay_begin=0;
	double ShouldPay=0.00;
	double FavourFee=0.00;
	double PayedPrepay=0.00;
	double PayedLater=0.00;
	double StillOwe=0.00;
	double RemontFee=0.00;
	double DelayFee=0.00;

	double fShouldPay=0.00;
	double fFavourFee=0.00;
	double fPayedPrepay=0.00;
	double fPayedLater=0.00;
	double fStillOwe=0.00;
	double fRemontFee=0.00;
	double fDelayFee=0.00;    
	int   CurYm=0; 
	int   MinMinYm=0;
	int   RetCode=0; 
	float  should_pay=0.00;
	char total_date[8+1];
	int	unBillYm=0;
	int	runFlag=0;
	EXEC SQL   END DECLARE SECTION;
	init(vContractNo);
	init(vMiniYM);
	ContractNo=in_contract_no;
	IdNo=in_id_no;
	pubLog_Debug(_FFL_, "", "", "ContractNo=[%ld]",ContractNo);
	pubLog_Debug(_FFL_, "", "", "in_id_no=[%ld]",in_id_no);
	init(total_date);
	sprintf(total_date,"%.8s",getSysTime(108));
	CurYm=atoi(total_date)/100;
	
	EXEC SQL select min_ym,pay_code,belong_code 
		into  :MinYm,:PayCode,:BelongCode
	   from dConMsg
	   where contract_no=:ContractNo;
	if(SQLCODE!=SQL_OK)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","9999","������ʾδ����");
		return 9999;
	}
	
	delay_flag=0;
	delay_rate=0;
	delay_begin=0;*/
	/***����ȡ�û����ɽ����***/
	/*EXEC SQL SELECT  to_number(delay_flag),delay_rate,
		delay_begin
		INTO :delay_flag,:delay_rate,:delay_begin
	  FROM sPayCode
	  WHERE pay_code=:PayCode
	  AND region_code=substr(:BelongCode,1,2);
	if (SQLCODE != SQL_OK) {
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
		return -1;
	}*/
	
	/***************************
 	ȡ�������ñ����Ϣ 
 	***************************/
	/*EXEC SQL SELECT  to_number(run_flag),to_number(unBill_Ym)
 	     INTO    :runFlag,:unBillYm
 	     FROM    cBillCond
 	     WHERE rownum<2;
	if (SQLCODE != SQL_OK) {
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
		return -1;
	}
	
	for(;;)
	{
		if(MinYm>CurYm)  break;
		init(OweTableName);
		
		sprintf(OweTableName,"dCustOweTotal%7d",MinYm*10+ContractNo%10);
		
		ShouldPay=0.00;  
		FavourFee=0.00;  
		PayedPrepay=0.00;
		PayedLater=0.00; 
		StillOwe=0.00;   
		RemontFee=0.00;     
		DelayFee=0.00;  
		
		init(TmpSqlStr);
		if( ( MinYm == unBillYm ) && ( 1 == runFlag ) )
		{                 
			sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0), \
							  nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0),\
							  nvl(sum(should_pay),0),nvl(sum(favour_fee),0),\
							  nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
							  from %s where id_no=:v5  and contract_no=:v6  AND ( bill_day<3600 or bill_day>9000 ) and payed_status=any('0','9','a') ",OweTableName);
		}
		else
		{
			sprintf(TmpSqlStr,"select nvl(sum(should_pay-favour_fee-payed_prepay-payed_later),0), \
							  nvl(sum(:v1*round((sign(sign(floor(sysdate-add_months(to_date(year_month*100+1+:v2,'YYYYMMDD'),2)))-1)+1)*floor(sysdate-add_months(to_date(year_month*100+1+:v3,'YYYYMMDD'),2))*:v4*(should_pay-favour_fee-payed_prepay-payed_later),2)),0),\
							  nvl(sum(should_pay),0),nvl(sum(favour_fee),0),\
							  nvl(sum(payed_prepay),0),nvl(sum(payed_later),0) \
							  from %s where id_no=:v5  and contract_no=:v6  and payed_status=any('0','9','a') ",OweTableName);
		}
		pubLog_Debug(_FFL_,"","GetPhoneOwe","TmpSqlStr=[%s]",TmpSqlStr);
	              
	
		EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :TmpSqlStr INTO :StillOwe,:DelayFee,:ShouldPay,:FavourFee,:PayedPrepay,:PayedLater using :delay_flag,:delay_begin,:delay_begin,:delay_rate,:in_id_no,:ContractNo;
		END;
		END-EXEC;

		if (SQLCODE != SQL_OK) {
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
			return -1;
		}
		
		fShouldPay  =fShouldPay  +ShouldPay   ;   
		fFavourFee  =fFavourFee  +FavourFee   ;   
		fPayedPrepay=fPayedPrepay+PayedPrepay ; 
		fPayedLater =fPayedLater +PayedLater  ;  
		fStillOwe   =fStillOwe   +StillOwe    ;    
		fRemontFee  =fRemontFee  +RemontFee   ;   
		fDelayFee   =fDelayFee   +DelayFee    ;    
		if (MinYm % 100 == 12)
			MinYm = (MinYm / 100 + 1) * 100 + 1;
		else
			MinYm++;
	}
	*out_owe_fee=fStillOwe+fDelayFee;*/
	
	/*-- ���������ʱ���� edit by huangtao at 25/08/2009 --*/
	EXEC SQL BEGIN DECLARE SECTION;
	FBFR32    *sendbuf, *rcvbuf;
    FLDLEN32  sendlen,rcvlen;
    int 	  sendNum=0;
    int 	  recvNum=0;
    int	      v_ret = 0;
    char 	  buffer1[256];
    char	  buffer2[256];
	char	  vContractNo[14 + 1];
	char	  vIdNo[14 + 1];
	char	  vOweFee[14 + 1];
	EXEC SQL END DECLARE SECTION;
	
	init(vContractNo);
	init(vIdNo);
	init(vOweFee);
	
	sprintf(vContractNo,"%ld",in_contract_no);
	sprintf(vIdNo,"%ld",in_id_no);
	
	pubLog_Debug(_FFL_, "", "", "����BOSS����bs_qryUserOwe��ʼ!");
	/*�����������*/
	sendNum = 2;
			
	/*�����������*/
	recvNum = 9;
	
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","��ʼ�����������ʧ��");
		return -1;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"2",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"9",(FLDLEN32)0);							
			
	Fchg32(sendbuf,GPARM32_0,0,vIdNo,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_1,0,vContractNo,(FLDLEN32)0);
			   
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);
			
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","��ʼ�����������ʧ��");
		return -1;
	}

	long reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	v_ret = 0;
	v_ret = fPubCallSrv(CALLSERV_TP,"bs_qryUserOwe",sendbuf,&rcvbuf);
	if(v_ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","","����BOSS����bs_qryUserOwe����");
		pubLog_Debug(_FFL_, "", "", "v_ret=[%d]!",v_ret);
		return -77;
	}
	
	memset(buffer1, '\0', sizeof(buffer1));
	memset(buffer2, '\0', sizeof(buffer2));

	Fget32(rcvbuf,GPARM32_0,0,buffer1,(FLDLEN32)0);
    	Fget32(rcvbuf,GPARM32_1,0,buffer2,(FLDLEN32)0);
  	
  	Trim(buffer1);
	Trim(buffer2);					
	printf("buffer1[%s]\n",buffer1);
	printf("buffer2[%s]\n",buffer2);
	
	if(strcmp(buffer1,"000000")!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"",buffer1,buffer2);
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		return -1;
	}
	
    Fget32(rcvbuf,GPARM32_8,0,vOweFee,(FLDLEN32)0);
    
    Trim(vOweFee);
    
    *out_owe_fee = atof(vOweFee);
    
    printf("out_owe_fee:[%lf]\n",*out_owe_fee);
    
	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 end --*/
	return 0;
}
int GetAccountMinYmChg(long InContractNo, int  *OutMinYm,
					   char *OutPayCode,  char *OutBelongCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int     MinYm=0;
	long    ContractNo=0;
	char    PayCode[1+1];
	char    BelongCode[7+1];
	EXEC SQL END DECLARE SECTION;
	pubLog_Debug(_FFL_,"","ContractNo","ContractNo=[%ld]",ContractNo); /*���Դ�ӡ���*/
	ContractNo = InContractNo;
	init(PayCode);
	init(BelongCode);
	pubLog_Debug(_FFL_,"","ContractNo","ContractNo=[%ld]",ContractNo); /*���Դ�ӡ���*/
	EXEC SQL SELECT  min_ym,pay_code,belong_code  
INTO    :MinYm,:PayCode,:BelongCode
		 FROM    dConMsg  
		 WHERE   contract_no=:ContractNo;
	if (sqlca.sqlcode != 0) 
	{
		if (sqlca.sqlcode == 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
			return -1;
		}
		else
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","������ʾδ����");
			return  -1;
		}
	}   
	*OutMinYm = MinYm;
	strcpy(OutPayCode, PayCode);
	strcpy(OutBelongCode, BelongCode);
	return 0;
}

int PublicGetInterest(long in_contract_no,double *out_interest_fee)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int RetValue=0;
	char  TmpSqlStr[2000];
	double tmp_prepay_fee=0.00;
	char  op_time[8+1];
	char  sysdate[8+1];
	double interest_fee=0.00;
	double total_interest=0.00;
	int   begin_ym=0;
	int   end_ym=0;
	long contractNo;
	EXEC SQL END DECLARE SECTION;
	
	contractNo = in_contract_no;
	EXEC SQL select  to_char(sysdate,'YYYYMMDD') into :sysdate from dual;
        if(SQLCODE!=0)
        {
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","130001","ȡϵͳʱ�����");
			return 130001;
        }
    /*ÿ��7��1�ս�Ϣ�����ֻͳ�ƴ����Ϣ����Ϣ��*/
    if ((atoi(sysdate)/100)%100>=7)
    {
    		begin_ym=atoi(sysdate)/100;
    		end_ym=(atoi(sysdate)/10000)*100+7;
    }
    else 
    {
    		begin_ym=atoi(sysdate)/100;
    		end_ym=(atoi(sysdate)/10000-1)*100+7;
    }
	for(;;)
        {
         if(begin_ym<end_ym)  break;
		/*wPayYYYYMM*/
		init(TmpSqlStr);
    	sprintf(TmpSqlStr,"select prepay_fee,to_char(op_time,'YYYYMMDD') from wPay%6d \
    		where contract_no=:v1 and op_code=any('1300','1302','1306','1354') \
    		and back_flag='0' and prepay_fee<>0 and pay_type in (select pay_type from spaytype \
    		where refund_flag='1')",
    						begin_ym);
    	 EXEC SQL PREPARE GetInterestStr  FROM :TmpSqlStr;
    	 EXEC SQL DECLARE GetInterestCur1  CURSOR FOR GetInterestStr;
    	 EXEC SQL OPEN GetInterestCur1 USING :contractNo;
    	 for(;;)
    	 {
    	   tmp_prepay_fee=0;
    	   init(op_time);
    	   interest_fee=0;
    	   EXEC SQL  FETCH   GetInterestCur1 INTO :tmp_prepay_fee,:op_time;
    	   if(SQLCODE==NOTFOUND) break;
		   if(SQLCODE<0)  
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","136298","ȡ�α����");
				EXEC SQL CLOSE GetInterestCur1;
				return 136298;
			}
    	   /***ȡ����**/	
    	   RetValue=ComputeInterest(tmp_prepay_fee,op_time,&interest_fee);
			if (RetValue != 0)
			{
				pubLog_DBErr(_FFL_,"","136298","������Ϣ����[%06d]",RetValue);
				EXEC SQL CLOSE GetInterestCur1;
				return 136298;
			}
			total_interest=total_interest+interest_fee;
    	 }
    	 EXEC SQL CLOSE GetInterestCur1;
    	
    	/*dCustPayOweYYYYMM0*/ 
		init(TmpSqlStr);
    	sprintf(TmpSqlStr,"select payed_prepay+delay_owe+bad_fee+dead_fee+payed_owe,\
    	to_char(op_time,'YYYYMMDD') from dCustPayOwe%7ld where contract_no=:v1 and \
    	payed_prepay+delay_owe+bad_fee+dead_fee+payed_owe<>0 and pay_type in \
    	(select pay_type from spaytype where refund_flag='1')",
    						begin_ym*10+in_contract_no%10);
    	 EXEC SQL PREPARE GetInterestStr  FROM :TmpSqlStr;
    	 EXEC SQL DECLARE GetInterestCur2  CURSOR FOR GetInterestStr;
    	 EXEC SQL OPEN GetInterestCur2 USING :contractNo;
    	 for(;;)
    	 {
    	   tmp_prepay_fee=0;
    	   init(op_time);
    	   interest_fee=0;
    	   EXEC SQL  FETCH   GetInterestCur2 INTO :tmp_prepay_fee,:op_time;
    	   if(SQLCODE==NOTFOUND) break;
		   if(SQLCODE<0)  
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","136297","ȡ�α����");
				EXEC SQL CLOSE GetInterestCur2;
				return 136298;
			}
/***
    	   if(SQLCODE==NOTFOUND||SQLCODE!=0)  break;
***/
    	   /***ȡ����**/
			printf("\n-");
    	   RetValue=ComputeInterest(tmp_prepay_fee,op_time,&interest_fee);
			if (RetValue != 0)
			{
				pubLog_DBErr(_FFL_,"","%06d","ȡ�α����",RetValue);
				EXEC SQL CLOSE GetInterestCur2;
				return RetValue;
			}
			total_interest=total_interest-interest_fee;
    	 }
    	 EXEC SQL CLOSE GetInterestCur2;
    	 if(begin_ym%100==1)
            {
                begin_ym=begin_ym-89;
            }
            else
            {
                begin_ym--;
            }
        }
		if(total_interest<0)
			total_interest=0.00;
		*out_interest_fee=total_interest;
		return 0;
}
int ComputeInterest(double in_fee,char *in_op_time,double *out_fee)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int temp_days=0;
	double temp_fee=0.00;
	int   in_day=0;
	int   in_month=0;
	int   in_year=0;
	int   this_year=0;
	char  op_time[8+1];
	EXEC SQL   END DECLARE SECTION;
	init(op_time);
	strcpy(op_time,in_op_time);
	temp_fee=in_fee;
	
		EXEC SQL SELECT to_number(to_char(sysdate,'yyyy')),to_number(substr(:op_time,1,4)),to_number(substr(:op_time,5,2)),to_number(substr(:op_time,7,2))
				 INTO :this_year,:in_year,:in_month,:in_day
				 FROM dual;
    	if(SQLCODE!=0)
    	{	
            pubLog_Debug(_FFL_,"","op_time","op_time = [%s]",op_time);
            PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","136299","������ʾδ����");
    	    return 136299;
    	}
	if (temp_fee == 0)
	{
		*out_fee=0;
		return 0;
	}
	if (in_month >= 7){
				EXEC SQL SELECT mod(sysdate-to_date(:op_time,'yyyymmdd'),365)
				 INTO :temp_days
				 FROM dual;
	}
	else {
		if (this_year=in_year) 
			{
				EXEC SQL SELECT sysdate-to_date(:op_time,'yyyymmdd')
				 INTO :temp_days
				 FROM dual;
			}
		else 
			{
				EXEC SQL SELECT mod(sysdate-to_date(substr(:op_time,1,4)||'0701','yyyymmdd'),365)
				 INTO :temp_days
				 FROM dual;
			}
	}
	/**out_fee=temp_fee * temp_days * 0.8 / 10000;
	printf("\nout_fee:[%12.6f]=[%12.2f * %d * 0.8 / 10000]",*out_fee,temp_fee,temp_days);*/
	*out_fee=temp_fee * temp_days * 0.0072 * 0.8 / 365;
	 pubLog_Debug(_FFL_,"","out_fee","out_fee:[%12.6f]=[%12.2f * %d * 0.0072 * 0.8 / 365]",*out_fee,temp_fee,temp_days);
	return 0;
}
/***********************************************/
/* lixg add for oneboss procession 2004.02.03  */
/*---------------------------------------------*/
/*@Return: 0: sucessful else failure           */
/***********************************************/
int fSndBizProcReq(tBizProcReq pReq, char *pSrvName)
{
     FBFR32 	*sendbuf, *rcvbuf;
     FLDLEN32 	sendlen,  rcvlen;
     int sendNum = 19, recvNum = 2, ret;
     long reqlen;
     char serverName[32],buffer[256];

     /* Attach to System/T as a Client Process */
     /*if (tpinit((TPINIT *) NULL) == -1) { printf("111\n"); return -1; }*/

     sendlen = (FLDLEN32)((sendNum)*100);
     if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
     {
          tpterm();
          pubLog_DBErr(_FFL_,"","-1","�ڴ�������");
          return -1;
     }

     Fchg32(sendbuf,SEND_PARMS_NUM32,0,"19",(FLDLEN32)0); 
     Fchg32(sendbuf,RECP_PARMS_NUM32,0,"2",(FLDLEN32)0);

	 /* put struct pReq data into sendbuf */
	 Fchg32(sendbuf,GPARM32_0,0,pReq.loginNo,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_1,0,pReq.orgCode,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_2,0,pReq.opCode,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_3,0,pReq.totalDate,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_4,0,pReq.idType,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_5,0,pReq.idValue,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_6,0,pReq.oprCode,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_7,0,pReq.bizType,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_8,0,pReq.newPaswd,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_9,0,pReq.oprTime,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_10,0,pReq.spid,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_11,0,pReq.spBizCode,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_12,0,pReq.infoCode,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_13,0,pReq.infoValue,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_14,0,pReq.userStatus,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_15,0,pReq.chgTime,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_16,0,pReq.otherStatus,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_17,0,pReq.homeProv,(FLDLEN32)0);
	 Fchg32(sendbuf,GPARM32_18,0,pReq.opNote,(FLDLEN32)0);
      
     rcvlen = (FLDLEN32)((recvNum)*100);
     if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
     {
          tpfree((char *)sendbuf);
          tpterm();
          pubLog_DBErr(_FFL_,"","-1","�ڴ�������");
          return -1;
     }

     memset(serverName, 0, sizeof(serverName));
     strcpy(serverName, pSrvName);
     reqlen=Fsizeof32(rcvbuf);
     ret = tpcall(serverName, (char *)sendbuf, 0, (char **)&rcvbuf, &reqlen, (long)0);
     if(ret == -1)
     {
		#ifdef _DEBUG  
		    pubLog_AppErr(_FFL_,"","-1","����[%s]ʧ��",serverName);
		#endif
          tpfree((char *)sendbuf);
          tpfree((char *)rcvbuf);
          tpterm();
          return -1;
     }
  
     memset(buffer, '\0', sizeof(buffer));
     Fget32(rcvbuf,GPARM32_0,0,buffer,NULL);
	 if(strcmp(buffer, "0000") != 0) { return(atoi(buffer));}

     tpfree((char *)sendbuf);
     tpfree((char *)rcvbuf);
     tpterm();

     return(0);
}

/**
 *  isAdd: 1����, 0ɾ��
 */
int fInsdCustType(const char *id_no, const char* main_code, const char* main_note, int is_add)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char		idNo[21+1];
	char		mainCode[2+1];
	char		mainNote[60+1];
	int			iCount=0;
	TdCustType	sTdCustType;
	char 	v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	TdCustTypeIndex oldIndex;
	int ret=0;
	EXEC SQL END DECLARE SECTION;
	
	memset(idNo, 0, sizeof(idNo));
	memset(mainCode, 0, sizeof(mainCode));
	memset(mainNote, 0, sizeof(mainNote));
	
	strcpy(idNo, id_no);
	strcpy(mainCode, main_code);
	strcpy(mainNote, main_note);

	if (is_add == 1)
	{
		EXEC SQL	SELECT count(*)
					  INTO :iCount
					  FROM dCustType
					 WHERE id_no = to_number(:idNo)
					   AND main_code = :mainCode;
		if (iCount != 0)
		{	
			pubLog_DBErr(_FFL_,"","1300","������ʾδ����");
			return 1300;
		}
		else
		{
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL	INSERT INTO dCustType
						(
							id_no, main_code, main_note
						)
						VALUES
						(
							to_number(:idNo), :mainCode, :mainNote
						);
			if (SQLCODE != 0)
			{	
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","1301","������ʾδ����");
				return 1301;
			}*/
			Trim(idNo);
			EXEC SQL select :idNo, :mainCode, :mainNote into :sTdCustType from dual ;
	  	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1301","������ʾδ����");
			return 1301;		
		}
	    ret = 0; 						
    	ret=OrderInsertCustType(ORDERIDTYPE_USER,idNo,100,"",0,"","fInsdCustType",sTdCustType);
			if (ret != 0)
    	{    				  
    	    PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","1301","������ʾδ����");
					return 1301;
    	}
			/*ng���� by zhaohx at 04/08/2009 end*/
		}
	}
	else if (is_add == 0)
	{
		EXEC SQL	SELECT count(*)
					  INTO :iCount
					  FROM dCustType
					 WHERE id_no = to_number(:idNo)
					   AND main_code = :mainCode;
		if (iCount == 0)
		{	
			pubLog_DBErr(_FFL_,"","1302","������ʾδ����");
			return 1302;
		}
		else
		{
			/*ng���� by zhaohx at 04/08/2009 begin*/
			/*EXEC SQL	DELETE FROM dCustType
						 WHERE id_no = to_number(:idNo)
						   AND main_code = :mainCode;
			if (SQLCODE != 0)
			{	
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","1303","������ʾδ����");
				return 1303;
			}*/
			Trim(idNo);
		  init(v_parameter_array);
  		memset(&oldIndex,0,sizeof(oldIndex));
  		
  		strcpy(oldIndex.sIdNo , idNo);                
    	strcpy(oldIndex.sMainCode , mainCode);
    	
    	strcpy(v_parameter_array[0],idNo);
    	strcpy(v_parameter_array[1],mainCode);  
    	
			ret=OrderDeleteCustType(ORDERIDTYPE_USER,idNo,100,"",0,"","",oldIndex,"",v_parameter_array);
			if (ret != 0)
    	{    				  
    	    PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","1303","������ʾδ����");
					return 1303;
    	}
			/*ng���� by zhaohx at 04/08/2009 end*/ 
		}
	}
	else
	{	
		pubLog_DBErr(_FFL_,"","1304","������ʾδ����");
		return 1304;
	}
	return 0;
}


/************************* 
   add by yangxh 20040228
   ���ӵ�����Ϣ��¼
**************************/
int fAddCustAssuer(struct sCustAssure *tmpCustAssure, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vassureRowId[20+1];
		char	dynStmt[1024];
		struct  sCustAssure strCustAssure;
	EXEC SQL END DECLARE SECTION;

	memset(vassureRowId,0,sizeof(vassureRowId));
	memset(&strCustAssure, 0, sizeof(struct sCustAssure));
	
	memcpy(&strCustAssure, tmpCustAssure, sizeof(struct sCustAssure));
	
	
	printf("strCustAssure.idNo 			[%s]\n",strCustAssure.idNo 			);
	printf("strCustAssure.opCode 		[%s]\n",strCustAssure.opCode 		);
	printf("strCustAssure.custName		[%s]\n",strCustAssure.custName		);
	printf("strCustAssure.idType		[%s]\n",strCustAssure.idType		);
	printf("strCustAssure.idAddress		[%s]\n",strCustAssure.idAddress		);
	printf("strCustAssure.idIccid		[%s]\n",strCustAssure.idIccid		);
	printf("strCustAssure.contactPhone	[%s]\n",strCustAssure.contactPhone	);
	printf("strCustAssure.contactAddress[%s]\n",strCustAssure.contactAddress);
	printf("strCustAssure.loginNo		[%s]\n",strCustAssure.loginNo		);
	printf("strCustAssure.opTime		[%s]\n",strCustAssure.opTime		);
	printf("strCustAssure.notes			[%s]\n",strCustAssure.notes			);
	EXEC SQL SELECT rowid
           	INTO    :vassureRowId
          	FROM    dCustAssure
           	WHERE   ID_NO   = to_number(:strCustAssure.idNo)
           	  AND   OP_CODE = :strCustAssure.opCode;

	/*��ѯ������Ϣ�����*/
	if(SQLCODE != 0 && SQLCODE != 1403 && SQLCODE != 100)
    {
        sprintf(retMsg,"��ѯ�ͻ�������Ϣ��dCustAssureʧ��! SQLCODE:[%d]",SQLCODE);
        PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1217",retMsg);
        return 1217;
    }
	/*��ѯ������Ϣ������ؼ�¼*/
    else if ( SQLCODE == 1403 || SQLCODE == 100)
    {
    	memset(dynStmt, 0, sizeof(dynStmt));
		sprintf(dynStmt, " INSERT INTO dCustAssure "
						 " ( id_no,op_code,cust_name, "
						 " id_type,id_address,id_iccid, "
						 " contact_phone,contact_address, "
						 " login_no,op_time,notes )"
						 " Values "
		                 " (:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,to_date(:v10,'yyyymmdd hh24:mi:ss'),:v11)");
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","dynStmt","dynStmt=[%s]", dynStmt);
#endif

		EXEC SQL PREPARE prepare1 From :dynStmt;
		EXEC SQL EXECUTE prepare1 using
								 	:strCustAssure.idNo 			,
								    :strCustAssure.opCode 		,
									:strCustAssure.custName		,
								    :strCustAssure.idType			,
								    :strCustAssure.idAddress		,
									:strCustAssure.idIccid		,
									:strCustAssure.contactPhone	,
									:strCustAssure.contactAddress	,
									:strCustAssure.loginNo		,
									:strCustAssure.opTime			,
									:strCustAssure.notes			;
    	if (SQLCODE !=0)
    	{
        	sprintf(retMsg,"����ͻ�������Ϣ��dCustAssure����!SQLCODE:[%d]",SQLCODE);
        	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1218",retMsg);
           	return 1218 ;
    	}
    }
    else
    {
	    memset(dynStmt, 0, sizeof(dynStmt));
	    
  		sprintf(dynStmt, "UPDATE dCustAssure SET "
							"CUST_NAME		= :v1,"
	  				 		"ID_TYPE		= :v2,"
	  				 		"ID_ADDRESS		= :v4,"
							"ID_ICCID		= :v3,"
				 			"CONTACT_PHONE	= :v5,"
					 		"CONTACT_ADDRESS= :v6,"
					 		"LOGIN_NO		= :v7,"
					 		"OP_TIME= to_date(:v8,'YYYYMMDD HH24:MI:SS'),"
                   	 		"NOTES			= :v9 "
                   	 	 "WHERE  rowid		= rtrim(:v10)" );
#ifdef _DEBUG_
			pubLog_Debug(_FFL_,"","dynStmt","dynStmt=[%s]", dynStmt);
#endif
		EXEC SQL PREPARE prepare2 From :dynStmt;
		EXEC SQL EXECUTE prepare2 using
								:strCustAssure.custName		,
								:strCustAssure.idType			,
								:strCustAssure.idAddress		,
								:strCustAssure.idIccid		,
								:strCustAssure.contactPhone	,
								:strCustAssure.contactAddress	,
								:strCustAssure.loginNo		,
								:strCustAssure.opTime			,
								:strCustAssure.notes			,
								:vassureRowId					;
    	if (SQLCODE !=0)
    	{
     	   	sprintf(retMsg,"�޸Ŀͻ�������Ϣ��dCustAssure����!SQLCODE:[%d]",SQLCODE);
     	   	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1219",retMsg);
            return 1219 ;
    	}
    }
    
    memcpy(tmpCustAssure, &strCustAssure, sizeof(struct sCustAssure));
    return 0 ;
}

/************************* 
   add by yangxh 20040228
   ��ѯ������Ϣ��¼
**************************/
int fQueryCustAssuer(struct sCustAssure *tmpCustAssure, char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		struct  sCustAssure strCustAssure;
	EXEC SQL END DECLARE SECTION;
	
	memset(&strCustAssure, 0, sizeof(struct sCustAssure));
	
	memcpy(&strCustAssure, tmpCustAssure, sizeof(struct sCustAssure));

    EXEC SQL SELECT  NVL(CUST_NAME,' '),
					 NVL(ID_TYPE,' '),
                     NVL(ID_ICCID,' '),
                     NVL(ID_ADDRESS,' '),
                     NVL(CONTACT_PHONE,' '),
                     NVL(CONTACT_ADDRESS,' '),
                     LOGIN_NO,
                     to_char(OP_TIME,'YYYYMMDD HH24:MI:SS'),
                     NVL(NOTES,' ')
             INTO    :strCustAssure.custName,
					 :strCustAssure.idType,
                     :strCustAssure.idIccid,
                     :strCustAssure.idAddress,
                     :strCustAssure.contactPhone,
                     :strCustAssure.contactAddress,
                     :strCustAssure.loginNo,
                     :strCustAssure.opTime,
                     :strCustAssure.notes
             FROM    dCustAssure
             WHERE   ID_NO   = to_number(:strCustAssure.idNo)
               AND	 OP_CODE = :strCustAssure.opCode;
	
	/*��ѯ������Ϣ�����*/
	if(SQLCODE != 0 && SQLCODE != 1403)
    {
        sprintf(retMsg,"ȡ�ͻ�������Ϣʧ��! [%d]",SQLCODE);
        PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1217",retMsg);
		return 1217;
    }
	/*��ѯ������Ϣ������ؼ�¼*/
    else if ( SQLCODE == 1403 || SQLCODE == 100)
    {	
		SQLCODE = 0;
    }
 
    memcpy(tmpCustAssure, &strCustAssure, sizeof(struct sCustAssure));

    return 0;
}

int fTimeMark(char *file, int line, char timeMsg[1024+1])
{
	struct timeb tm;
	struct tm    *time;
	memset(&tm,0,sizeof(struct timeb));
	
	ftime(&tm);
	
	time=localtime(&tm.time);
	pubLog_Debug(_FFL_,"","","In File [%s] Line {%d]",file,line);
	pubLog_Debug(_FFL_,"","Time","Time is : [%02d:%02d:%02d::%03d] ",time->tm_hour,time->tm_min,time->tm_sec,tm.millitm);
	pubLog_Debug(_FFL_,"","msg","Msg=[%s]",timeMsg);
	return 0;
}

/***************************************
 �������ƣ�fChkBlackOwe
 �������ڣ�2004/02/19
 ���ù��̣���֤֤���Ƿ���������Ƿ�Ƿ��
 ����������
 ���������
	  �ֻ�����
      ��֤����   B����֤��������O����֤Ƿ�ѣ�A����֤��������Ƿ��
      
 ��������� 
      �������
      ������Ϣ
      
�޸ļ�¼:
		�޸���Ա��
        �޸����ڣ�
		�޸�ǰ�汾�ţ�
		�޸ĺ�汾��:
		�޸�ԭ��
                                  
****************************************/
int fChkBlackOwe( const char * phone_no, const char *check_type )
{
    char        temp_buf[60+1];	      /*��ʱʹ�õ��ַ�������*/
    int         i=0,k=0,iRet=0;
    long        j=0;
	char        ret_code[6+1];        /* ������� */
    char        ret_msg[40+1];        /* ������Ϣ */

    EXEC SQL BEGIN DECLARE SECTION;
        char    sIn_Id_type[1+1];     /* ֤������ */
        char    sIn_Id_iccid[20+1];   /* ֤������ */
        char    sIn_Check_type[1+1];  /* ��֤���� */

        int     v_Num=0;
        char    v_Assure_no[14+1];    /* ������   */
        long    v_Id_no=0;
        long    v_Min_ym=0;           /* ��СǷ������ */
        char    v_Contract_no[14+1];
        char    vSqltemp[1000+1];
        int     v_Bill_year=0;
        int     v_Bill_month=0;
        long    v_Bill_ym=0;
        char    v_Phone_no[15+1];
        char	vcust_id[23];		
    EXEC SQL END DECLARE SECTION;
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","fChkBlackOwe","begin fChkBlackOwe");
    #endif
    
    init(ret_code);
    init(ret_msg);
    init(temp_buf);
    
    init(sIn_Id_type);
    init(sIn_Id_iccid);
    init(sIn_Check_type);
    
    init(v_Assure_no);
    init(v_Contract_no);
    init(vSqltemp);
    init(v_Phone_no);
    
    strcpy(v_Phone_no, phone_no);
    strcpy(sIn_Check_type, check_type);
    
    #ifdef _DEBUG_
        pubLog_Debug(_FFL_,"","v_Phone_no","v_Phone_no= [%s]",v_Phone_no);
        pubLog_Debug(_FFL_,"","sIn_Check_type","sIn_Check_type= [%s]",sIn_Check_type);
    #endif
    
    strcpy(ret_code,"000000");
    strcpy(ret_msg,"���Ǻ������û�/û��Ƿ��");
    
   	Sinitn(vcust_id );
	
	EXEC SQL SELECT to_char(cust_id)
	  INTO :vcust_id
	  FROM dCustMsg
	  WHERE phone_no=:v_Phone_no AND substr(run_code,2,1)<'a';
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","������ʾδ����");
		return 1207;
	}
	
	/*��ѯdCustDoc��*/
	EXEC SQL	SELECT id_type,id_iccid
				  INTO :sIn_Id_type,:sIn_Id_iccid
				  FROM dCustDoc
				 WHERE cust_id=to_number(:vcust_id);
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","������ʾδ����");
		return 1208;
	}
	
	Trim(sIn_Id_iccid);
	
	/*��������֤��֤�����Ȳ�Ϊ15��18����Ϊ���֤������,ֹͣ���;���֤�����볤��С��5λ��ʹ�ò���������Ӱ��Ч�ʣ�Ҳֹͣ���*/
	if((sIn_Id_type[0]=='0' && ( strlen(sIn_Id_iccid)!=15 && strlen(sIn_Id_iccid)!=18 )) || ( strlen(sIn_Id_iccid) < 5 ))
	{
		pubLog_Debug(_FFL_,"","","֤������[%s],֤����:[%s],���֤�������֤�����볤��С��5λ",sIn_Id_type,sIn_Id_iccid);
    	pubLog_Debug(_FFL_,"","fChkBlackOwe","end  fChkBlackOwe");
    	return 0;
    }
	/*��������֤,����֤�����г��������ж��Ƿ�Ϊ�Ϸ�֤����*/
	if(sIn_Id_type[0]=='0')
	{
		char vIccIdDate[8 + 1];
		init(vIccIdDate);
		if(strlen(sIn_Id_iccid)==15)
		{
			strcpy(vIccIdDate,"19");
			strncpy(vIccIdDate+2,sIn_Id_iccid+6,6);
		}
		else
		{
			strncpy(vIccIdDate+6,sIn_Id_iccid,8);
		}
		vIccIdDate[8]=0;
		if(!test_date(vIccIdDate, SMALLDATETIME))
		{
			pubLog_Debug(_FFL_,"","fChkBlackOwe","֤������[%s],֤����:[%s],���֤������",sIn_Id_type,sIn_Id_iccid);
			return 0;
		}
	}

    k=1;
    if(strcmp(sIn_Check_type,"A")==0)
        k=2;
    pubLog_Debug(_FFL_,"","k","k=[%d]",k);
    
    for(i=0;i<k;i++)
    {
        if( (strcmp(sIn_Check_type,"A")==0 && i==0) || strcmp(sIn_Check_type,"B")==0)
        {
            /* ��֤������ */
            EXEC SQL select count(*) into :v_Num 
                     from wLocalBlackList where black_no=:sIn_Id_iccid and black_type=:sIn_Id_type;
            if(SQLCODE!=0)
            {
                strcpy(ret_code,"100000");
                strcpy(ret_msg,"��ѯwLocalBlackList��ʧ��");
                iRet = 1010;
                PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"",ret_code,ret_msg);
                goto end_valid;
            }
            else
            {
                if(v_Num>0)
                {
                    strcpy(ret_code,"100001");
                    strcpy(ret_msg,"��֤�������ں����������ܰ���ҵ��");
                    iRet = 1011;
                    PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"",ret_code,ret_msg);
                    goto end_valid;
                }
                else
                {
                    strcpy(ret_code,"000000");
                    strcpy(ret_msg,"���Ǻ������û�");
                    PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"",ret_code,ret_msg);
                }
            }
        }
        else if( (strcmp(sIn_Check_type,"A")==0 && i==1) || strcmp(sIn_Check_type,"O")==0)
        {
            /* ��֤Ƿ�� */
            EXEC SQL declare mycur00 cursor for
            		select assure_no
                     from dAssuMsg where assure_id=:sIn_Id_iccid and id_type=:sIn_Id_type;
            EXEC SQL open mycur00;
            for(;;)
            {
            	EXEC SQL fetch mycur00 into :v_Assure_no;
                if(SQLCODE!=0)
                       break;
                
                Trim(v_Assure_no);
                pubLog_Debug(_FFL_,"","v_Assure_no","v_Assure_no=[%s]\n",v_Assure_no);
                EXEC SQL declare mycur01 cursor for
                    select id_no,to_number(to_char(sysdate,'yyyymm')) from dCustInnet where assure_no=:v_Assure_no;
                EXEC SQL open mycur01;
                for(;;)
                {
                    EXEC SQL fetch mycur01 into :v_Id_no,:v_Bill_ym;
                    if(SQLCODE!=0)
                    {
                        break;
                    }
                    pubLog_Debug(_FFL_,"","","v_Id_no=[%ld][%ld]\n",v_Id_no,v_Bill_ym);
                    EXEC SQL declare mycur02 cursor for
                        select to_char(contract_no) from dConUserMsg where id_no=:v_Id_no;
                    EXEC SQL open mycur02;
                    for(;;)
                    {
                        EXEC SQL fetch mycur02 into :v_Contract_no;
                        if(SQLCODE!=0)
                        {
                            break;
                        }
                        pubLog_Debug(_FFL_,"","v_Contract_no","v_Contract_no=[%s]\n",v_Contract_no);
                        Trim(v_Contract_no);
                        EXEC SQL select min_ym into :v_Min_ym from dConMsg where contract_no=to_number(:v_Contract_no);
                        if(SQLCODE!=0)
                        {
                            strcpy(ret_code,"100006");
                            strcpy(ret_msg,"��ѯdConMsg��ʧ��");
                            PUBLOG_DBDEBUG("");
							pubLog_DBErr(_FFL_,"",ret_code,ret_msg);
                            iRet = 1016;
                            goto end_valid;
                        }
                        pubLog_Debug(_FFL_,"","v_Min_ym","v_Min_ym=[%ld]\n",v_Min_ym);
                        v_Bill_year=v_Min_ym/100;
                        v_Bill_month=v_Min_ym%100;
                        for(j=v_Min_ym;j<=v_Bill_ym;)
                        {
                            init(v_Phone_no);
                            sprintf(vSqltemp,"select phone_no from dCustOweTotal%ld%.1s \
                                              where contract_no=to_number(:v_Contract_no) and id_no=:v_Id_no \
                                                  and (payed_status='0' or payed_status='1' or payed_status='4') \
                                                  and rownum<2",j,&v_Contract_no[strlen(v_Contract_no)-1]);
                            pubLog_Debug(_FFL_,"","vSqltemp","vSqltemp=[%s]\n",vSqltemp);
      
    	                    EXEC SQL EXECUTE
    	                    BEGIN
    	                        EXECUTE IMMEDIATE :vSqltemp INTO :v_Phone_no using :v_Contract_no,:v_Id_no;
    	                    END;
    	                    END-EXEC;
                            if(SQLCODE!=0 && SQLCODE!=1403)
                            {
                                /*printf("SQLCODE=[%d]\n",SQLCODE);
                                */
                                strcpy(ret_code,"100005");
                                strcpy(ret_msg,"��ѯdCustOweTotal��ʧ��");
                                PUBLOG_DBDEBUG("");
								pubLog_DBErr(_FFL_,"","1015","SQLCODE=[%d],ret_code=[%S],ret_msg=[%S]",SQLCODE,ret_code,ret_msg);
                                iRet = 1015;
                                goto end_valid;
                            }
                            Trim(v_Phone_no);
                            printf("v_Phone_no=[%s]\n",v_Phone_no);
                            if(strlen(v_Phone_no)>0)
                            {
                                strcpy(ret_code,"100003");
                                sprintf(ret_msg,"���û������ĺ���[%s]��Ƿ��/����/����",v_Phone_no);                          
                                pubLog_DBErr(_FFL_,"","1013","ret_code=[%S],ret_msg=[%S]",ret_code,ret_msg);
                                iRet = 1013;
                                goto end_valid;
                            }
                            v_Bill_month++;
                            if(v_Bill_month==13)
                            {
                                v_Bill_month=1;
                                v_Bill_year++;
                            }
                            j=v_Bill_year*100+v_Bill_month;
                        }
                    }
                    EXEC SQL close mycur02;
                }
                EXEC SQL close mycur01;
               
                strcpy(ret_code,"000000");
                strcpy(ret_msg,"û��Ƿ��");
            }
            EXEC SQL close mycur00;
        }
        else
        {
            strcpy(ret_code,"100004");
            strcpy(ret_msg,"��֤�Ƿ���������Ƿ�Ƿ�ѵ���֤���Ͳ�����");
            pubLog_DBErr(_FFL_,"","ret_code=[%S],ret_msg=[%S]",ret_code,ret_msg);
            iRet = 1014;
            goto end_valid;
        }
    }
        
end_valid:
    EXEC SQL close mycur02;
    EXEC SQL close mycur01;
    /*
    printf("��֤���:[%s]\n",ret_msg);
    printf("==========end  fChkBlackOwe ==============\n"); 
    */
	pubLog_Debug(_FFL_,"","","��֤���:[%s]",ret_msg);
	pubLog_Debug(_FFL_,"","","==========end  fChkBlackOwe ==============");
    return iRet;
        
}

int fGetIccOwe( const char * phone_no )
{
    char        temp_buf[60+1];	      /*��ʱʹ�õ��ַ�������*/
    int         i=0,k=0,iRet=0;
    long        j=0;
	char        ret_code[6+1];        /* ������� */
    char        ret_msg[255+1];        /* ������Ϣ */
    int			proc_stat = 0;		  /* ִ�в��� */
	float       totalPrepay=0.0;      /* �û��ۼ�Ԥ��� */
	float		totalOwe=0.0;         /* �û��ۼ�Ƿ��+�ۼ�δ����*/

    EXEC SQL BEGIN DECLARE SECTION;
        char    sIn_Id_type[1+1];     /* ֤������ */
        char    sIn_Id_iccid[20+1];   /* ֤������ */

        int     v_Num=0;
        long    v_Id_no=0;
        char    vSqltemp[1000+1];
        char	vcust_id[23];
        char    v_Phone_no[15+1];
    EXEC SQL END DECLARE SECTION;
    
    #ifdef _DEBUG_    
       /* printf("==========begin fGetIccOwe ==============");
       */
     pubLog_Debug(_FFL_,"","","==========begin fGetIccOwe ==============");
    #endif
    
    init(ret_code);
    init(ret_msg);
    init(temp_buf);
    
    init(sIn_Id_type);
    init(sIn_Id_iccid);
    
    init(vSqltemp);
    init(v_Phone_no);
    
    strcpy(v_Phone_no, phone_no);
    
   	Sinitn(vcust_id );
	
	EXEC SQL SELECT to_char(cust_id)
	  INTO :vcust_id
	  FROM dCustMsg
	  WHERE phone_no=:v_Phone_no AND substr(run_code,2,1)<'a';
	if(SQLCODE!=0)
	{
		/*printf("In fGetIccOwe ��ѯdCustMsgʧ��,phone_no=[%s],SQLCODE=[%d]\n",v_Phone_no,SQLCODE);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","In fGetIccOwe ��ѯdCustMsgʧ��,phone_no=[%s],SQLCODE=[%d]",v_Phone_no,SQLCODE);
		return -1;
	}
	
	/*��ѯdCustDoc��*/
	EXEC SQL	SELECT id_type,id_iccid
				  INTO :sIn_Id_type,:sIn_Id_iccid
				  FROM dCustDoc
				 WHERE cust_id=to_number(:vcust_id);
	if(SQLCODE!=0)
	{
		/*printf("In fGetIccOwe ��ѯdCustDocʧ��,cust_id=[%s],SQLCODE=[%d]\n",vcust_id,SQLCODE);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","In fGetIccOwe ��ѯdCustDocʧ��,cust_id=[%s],SQLCODE=[%d]",vcust_id,SQLCODE);
		return -1;
	}
    
    #ifdef _DEBUG_
		/*printf("sIn_Id_iccid= [%s]\n", sIn_Id_iccid);
          printf("sIn_Id_type= [%s] \n", sIn_Id_type);
        */
        pubLog_Debug(_FFL_,"","","sIn_Id_iccid= [%s]", sIn_Id_iccid);
         pubLog_Debug(_FFL_,"","","sIn_Id_type= [%s]", sIn_Id_type);
    #endif
    
	
	Trim(sIn_Id_iccid);
	
	/*��������֤��֤�����Ȳ�Ϊ15��18����Ϊ���֤������,ֹͣ���;���֤�����볤��С��5λ��ʹ�ò���������Ӱ��Ч�ʣ�Ҳֹͣ���*/
	if((sIn_Id_type[0]=='0' && ( strlen(sIn_Id_iccid)!=15 && strlen(sIn_Id_iccid)!=18 )) || ( strlen(sIn_Id_iccid) < 5 ))
	{
		/*printf("֤������[%s],֤����:[%s],���֤�������֤�����볤��С��5λ\n",sIn_Id_type,sIn_Id_iccid);
    	printf("==========end  fChkBlackOwe ==============\n");
    	*/
		pubLog_Debug(_FFL_,"","","֤������[%s],֤����:[%s],���֤�������֤�����볤��С��5λ",sIn_Id_type,sIn_Id_iccid);
		pubLog_Debug(_FFL_,"","","==========end  fChkBlackOwe ==============");
    	return 0;
    }
	/*��������֤,����֤�����г��������ж��Ƿ�Ϊ�Ϸ�֤����*/
	if(sIn_Id_type[0]=='0')
	{
		char vIccIdDate[8 + 1];
		init(vIccIdDate);
		if(strlen(sIn_Id_iccid)==15)
		{
			strcpy(vIccIdDate,"19");
			strncpy(vIccIdDate+2,sIn_Id_iccid+6,6);
		}
		else
		{
			strncpy(vIccIdDate+6,sIn_Id_iccid,8);
		}
		vIccIdDate[8]=0;
		if(!test_date(vIccIdDate, SMALLDATETIME))
		{
			pubLog_Debug(_FFL_,"","","֤������[%s],֤����:[%s],���֤������",sIn_Id_type,sIn_Id_iccid);
			return 0;
		}
	}
    
    EXEC SQL declare Iccidcur00 cursor for
    	select cust_id
            from dCustDoc where id_iccid=:sIn_Id_iccid;
    EXEC SQL open Iccidcur00;
    for(;;)
    {
       	EXEC SQL fetch Iccidcur00 into :vcust_id;
       	
        if(SQLCODE==NOTFOUND) 
        {
        	
        	PUBLOG_DBDEBUG("");
        	pubLog_DBErr(_FFL_,"","","In fGetIccOwe id_iccid=[%s]",sIn_Id_iccid);
        	proc_stat = 10;
        	break;
        }
        else if(SQLCODE!=0)
        {    
        	PUBLOG_DBDEBUG("");
        	pubLog_DBErr(_FFL_,"","","In fGetIccOwe ��ѯdCustDocʧ��,id_iccid=[%s],SQLCODE=[%d]",sIn_Id_iccid,SQLCODE);
        	iRet=-1;
        	proc_stat = 20;
        	goto end_valid;
        }
              
        Trim(vcust_id);
                
    	EXEC SQL declare Iccidcur01 cursor for
    		select id_no
            	from dCustUserMsg where cust_id = to_number(:vcust_id);
        EXEC SQL open Iccidcur01;
        for(;;)
    	{
       		EXEC SQL fetch Iccidcur01 into :v_Id_no;
       		
        	if(SQLCODE==NOTFOUND)
        	{	
        		proc_stat = 30;
        		break;
        	}
        	else if(SQLCODE!=0)
        	{
        		/*printf("In fGetIccOwe ��ѯdCustUserMsgʧ��,cust_id=[%s],SQLCODE=[%d]\n",vcust_id,SQLCODE);
        		*/
        		PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","","In fGetIccOwe ��ѯdCustUserMsgʧ��,cust_id=[%s],SQLCODE=[%d]",vcust_id,SQLCODE);
        		iRet=-1;
        		proc_stat = 40;
        		goto end_valid;
        	}

    		totalPrepay=0.0;
    		totalOwe=0.0;
                  	       	
        	{
        		EXEC SQL BEGIN DECLARE SECTION;
				char	vst[512];
				double	vowe_fee=0;
				double	vprepay_fee=0;
				int		vtmp_num=0;
				int     ids=0;
				int		vret=0;
				char    phone_no[15+1];
				double  v_unbill=0;
				long    contract_no=0;
				char	vpay_code[1+1];
				EXEC SQL END DECLARE SECTION;        
    			
				memset(vst,0,sizeof(vst));
				init(phone_no);
				init(vpay_code);

				EXEC SQL	select ids,contract_no,phone_no into :ids,:contract_no,:phone_no
				 		from dCustMsg 
						where id_no=:v_Id_no;
        		if(SQLCODE==NOTFOUND)
        		{	
        			PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
        			proc_stat = 45;
					goto end_userId;
        		}
				else if(SQLCODE!=0)
				{
#ifdef _DEBUG_
					/*printf("\n In fGetIccOwe ȡ�û���Ϣ����,[%d] id_no:[%ld]",SQLCODE,v_Id_no);
					*/
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"",""," In fGetIccOwe ȡ�û���Ϣ����,[%d] id_no:[%ld]",SQLCODE,v_Id_no);
#endif
                    sprintf(ret_msg,"ȡ�û���Ϣ����[%d],[%ld]",SQLCODE,v_Id_no);
                    iRet = -1;
                    proc_stat = 50;
                    goto end_valid;
				}

				if(ids==1)
				{
					EXEC SQL SELECT nvl(owe_fee,0),nvl(prepay_fee,0),pay_code INTO :vowe_fee,:vprepay_fee,:vpay_code
						FROM dConMsg
						WHERE contract_no=:contract_no;
					if(SQLCODE!=0)
					{
#ifdef _DEBUG_
						/*printf("\n In fGetIccOwe ȡdCOnMsg��Ϣ����,[%d],contract_no:[%ld]",SQLCODE,contract_no);
						*/
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"",""," In fGetIccOwe ȡdCOnMsg��Ϣ����,[%d],contract_no:[%ld]",SQLCODE,contract_no);
#endif
                        sprintf(ret_msg,"ȡdCOnMsg��Ϣ����,[%d],contract_no:[%ld]",SQLCODE,contract_no);
                        iRet = -1;
                        goto end_valid;
					}
					/*���û����������ʻ�,�������û�Ƿ��״̬*/
					if ( vpay_code[0] == '4' )
					{
						sprintf(ret_msg,"�ͻ�ֻ��һ���û�,���û����������ʻ�[%ld],�������û�Ƿ��״̬",contract_no);
						/*printf("�ͻ�ֻ��һ���û�,���û����������ʻ�[%ld],�������û�Ƿ��״̬\n",contract_no);
						*/
						pubLog_DBErr(_FFL_,"","","�ͻ�ֻ��һ���û�,���û����������ʻ�[%ld],�������û�Ƿ��״̬",contract_no);
						proc_stat = 60;
						goto end_userId;
					}

					vret=PublicGetUnbillChg(contract_no,v_Id_no,0,phone_no,4,&v_unbill);
					/*
					printf("vowe_fee=[%f],v_unbill=[%f]\n",vowe_fee,v_unbill);
					printf("vprepay_fee=[%f]\n",vprepay_fee);
					*/
					 pubLog_Debug(_FFL_,"","","vowe_fee=[%f],v_unbill=[%f]",vowe_fee,v_unbill);
					 pubLog_Debug(_FFL_,"","","vprepay_fee=[%f]",vprepay_fee);	
					if(vret!=0)
					{
#ifdef _DEBUG_
						/*printf("\n In fGetIccOwe ȡδ���ʻ��ѣ�[%d],[%d]",vret,SQLCODE);
						*/
						pubLog_DBErr(_FFL_,"",""," In fGetIccOwe ȡδ���ʻ��ѣ�[%d],[%d]",vret,SQLCODE);
#endif    
                        sprintf(ret_msg,"ȡδ���ʻ��ѣ�[%d],[%d]",vret,SQLCODE);
                        iRet = -1;
                        proc_stat = 70;
                        goto end_valid;
					}

					totalPrepay = vprepay_fee;
					totalOwe = vowe_fee+v_unbill; 
				}                
				else
				{	    
					vtmp_num = 0;
					sprintf(vst,"select contract_no from dConUserMsg\
							where id_no=to_number(:v1)\
							and serial_no=0 ");
					EXEC SQL PREPARE tmp_sGetUserOwe1 FROM :vst;
					EXEC SQL DECLARE C_GetUserOwe1 CURSOR FOR tmp_sGetUserOwe1;
					EXEC SQL OPEN C_GetUserOwe1 using :v_Id_no;
					for(;;)
					{
						vowe_fee=0;
						v_unbill=0;
						contract_no=0;
						EXEC SQL FETCH C_GetUserOwe1 INTO :contract_no;
						if(SQLCODE==NOTFOUND)
        				{	
        					PUBLOG_DBDEBUG("");
							pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
        					proc_stat = 90;
        					break;
        				}
        				else if(SQLCODE!=0)
        				{
        					/*printf("In fGetIccOwe ��ѯdConUserMsgʧ��,id_no=[%ld],contract_no=[%ld]\n",v_Id_no,contract_no);
        					*/
        					PUBLOG_DBDEBUG("");
							pubLog_DBErr(_FFL_,"","","In fGetIccOwe ��ѯdConUserMsgʧ��,id_no=[%ld],contract_no=[%ld]",v_Id_no,contract_no);
        					iRet=-1;
        					proc_stat = 100;
        					EXEC SQL CLOSE C_GetUserOwe1;
        					goto end_valid;
        				}
        										
						EXEC SQL SELECT PAY_CODE INTO :vpay_code
							FROM dConMsg 
							WHERE contract_no=:contract_no;
						if(SQLCODE!=0)
						{
							/*printf("\n In fGetIccOwe ��ѯdConMsg����,SQLCODE=[%d],�û�[%ld],�ʻ�[%ld]\n",SQLCODE,v_Id_no,contract_no);
							*/
							PUBLOG_DBDEBUG("");
							pubLog_DBErr(_FFL_,"","","In fGetIccOwe ��ѯdConMsg����,SQLCODE=[%d],�û�[%ld],�ʻ�[%ld]",SQLCODE,v_Id_no,contract_no);
							proc_stat = 110;
							EXEC SQL CLOSE C_GetUserOwe1;
							sprintf(ret_msg,"��ѯdConMsg�����û�[%ld],�ʻ�[%ld]Ƿ�ѳ���",v_Id_no,contract_no);
                            iRet = -1;
                            goto end_valid;
						}
						/*���û����������ʻ�,�������û�Ƿ��״̬*/
						if ( vpay_code[0] == '4' )
						{
							sprintf(ret_msg,"���û����������ʻ�[%ld],�������û�Ƿ��״̬",contract_no);
							/*printf("���û�[%ld]���������ʻ�[%ld],�������û�Ƿ��״̬\n",v_Id_no,contract_no);
							*/						
							pubLog_DBErr(_FFL_,"","","���û�[%ld]���������ʻ�[%ld],�������û�Ƿ��״̬",v_Id_no,contract_no);
							proc_stat = 120;
							EXEC SQL CLOSE C_GetUserOwe1;
							goto end_userId;
						}
						
						vret=GetConUserOweChg(contract_no,v_Id_no,&vowe_fee);
						if(vret!=0)
						{
							/*printf("\n In fGetIccOwe ȡǷ�ѳ���");
							*/
							pubLog_DBErr(_FFL_,"","","In fGetIccOwe ȡǷ�ѳ���");
							EXEC SQL CLOSE C_GetUserOwe1;
                            sprintf(ret_msg,"ȡ�û�[%ld],�ʻ�[%ld]Ƿ�ѳ���",v_Id_no,contract_no);
                            iRet = -1;
                            proc_stat = 130;
                            goto end_valid;
						}
						
						vret=PublicGetUnbillChg(contract_no,v_Id_no,0,phone_no,4,&v_unbill);
						if(vret!=0)
						{
							/*printf("\n In fGetIccOwe  ȡδ���ʻ��ѳ���");
							*/
							pubLog_DBErr(_FFL_,"","","In fGetIccOwe  ȡδ���ʻ��ѳ���");
							EXEC SQL CLOSE C_GetUserOwe1;
                            sprintf(ret_msg,"ȡ�û�[%ld],�ʻ�[%ld]δ���ʻ��ѳ���",v_Id_no,contract_no);
                            iRet = -1;
                            proc_stat = 140;
                            goto end_valid;
						}         
						vtmp_num++;

						vprepay_fee = 0.00;
						EXEC SQL	SELECT nvl(prepay_fee,0)
					  		INTO :vprepay_fee
					  		FROM dConMsg
							WHERE contract_no=:contract_no;
							
						/*printf("vowe_fee=[%f],v_unbill=[%f]\n",vowe_fee,v_unbill);
						printf("vprepay_fee=[%f]\n",vprepay_fee);
						*/
						pubLog_Debug(_FFL_,"","","vowe_fee=[%f],v_unbill=[%f]",vowe_fee,v_unbill);
						pubLog_Debug(_FFL_,"","","vprepay_fee=[%f]",vprepay_fee);

						totalPrepay += vprepay_fee;
						totalOwe += vowe_fee+v_unbill; 
					}
					EXEC SQL CLOSE C_GetUserOwe1;
        		}
			}

			if( totalOwe-totalPrepay > 0.005 )
			{
		   		sprintf( ret_msg, "�û�[%ld]���ʻ��ۼ�Ƿ�����ۼ�δ����֮�ʹ����ۼ�Ԥ���", v_Id_no );
           		iRet = 1;
               	proc_stat = 150;
               	pubLog_DBErr(_FFL_,"","",ret_msg);
				goto end_valid;
			}
end_userId:
			;
    	}
    	EXEC SQL close Iccidcur01;
        
    }
    EXEC SQL close Iccidcur00;
    /*
    printf("��ѯǷ�ѽ��:[%d],proc_stat[%d]\n",iRet,proc_stat);
    printf("��ѯ��Ϣ:[%s]\n",ret_msg);
    printf("==========end  fGetIccOwe ==============\n"); 
	*/
	 pubLog_Debug(_FFL_,"","","��ѯǷ�ѽ��:[%d],proc_stat[%d]",iRet,proc_stat);
	 pubLog_Debug(_FFL_,"","","��ѯ��Ϣ:[%s]",ret_msg);
	 pubLog_Debug(_FFL_,"","","==========end  fGetIccOwe =============="); 
	return iRet;

end_valid:
	
	EXEC SQL close Iccidcur01;
	EXEC SQL close Iccidcur00;
	/*
	printf("��ѯǷ�ѽ��:[%d],proc_stat[%d]\n",iRet,proc_stat);
	printf("��ѯ��Ϣ:[%s]\n",ret_msg);
	printf("==========end  fGetIccOwe ==============\n"); 
	*/
	 pubLog_Debug(_FFL_,"","","��ѯǷ�ѽ��:[%d],proc_stat[%d]",iRet,proc_stat);
	 pubLog_Debug(_FFL_,"","","��ѯ��Ϣ:[%s]",ret_msg);
	 pubLog_Debug(_FFL_,"","","==========end  fGetIccOwe =============="); 
	return iRet;

}
/*����������Ч���ط� ��Ԥ������
*��ʽ:��'AAYYYYMMDD|' + 'AAYYYYMMDD|' + 'AAYYYYMMDD|' + ������ 
*     "AA"��ʾ�ط����룬"YYYYMMDD"��ʾ�������ڣ�"|"��ʾ��ָ����������û�е��ڣ���գ���
*/
int fAddUsedLimitFuncs(tFuncEnv *funcEnv, char *funcs,double iRemainFee,double *olimitFee,char *funcode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char end_date[8+1];
	int  hasExpire = 0; /*�Ƿ��е���ʱ��*/
	int  iRetCode = 0;
	int  num=0;
	int  iCount=0;
	int  iCountFav=0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];    
	char function_code[2+1];

	char	idNo[22 + 1];
	int   ret =0;
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char vOrgId[10+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	double  vLimitPay=0;
	double  vRemainFee=0;
	char	vLimiName[20+1];
	char	sqlStrfunc[512+1];
	char	delFuncStr[3+1];
	char	retCode[6+1];
	char	outtime[17+1];
	long	rspno =0;
	long	tLoginAccept=0;
	int iCountBind=0;
	TdCustFunc	sTdCustFunc;
	TdCustFuncAdd	sTdCustFuncAdd;
	TdCustFuncAddHis	sTdCustFuncAddHis;
	TdCustFuncHis	sTdCustFuncHis;
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vLimiName);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(vOrgId);
	Sinitn(orgCode);
	Sinitn(delFuncStr);
	Sinitn(sqlStrfunc);
	Sinitn(retCode);
	Sinitn(outtime);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	
	vRemainFee=iRemainFee;
	
	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;
	
	srand(time(NULL));
	
	EXEC SQL select to_char(sysdate+70/(24*60),'yyyymmdd HH24:MI:SS') into :outtime from dual;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p == 10)
		{
			strncpy(end_date, p+2, 8);
			end_date[8] = '\0';
			hasExpire = 1;
		}
		else if (q - p == 2)
		{
			hasExpire = 0;
		}
		else
		{	
			pubLog_DBErr(_FFL_,"","121920","");
			return 121920;
		}

		p = q + 1;
		/*printf("function_code = [%s]\n", function_code);
		*/
		pubLog_Debug(_FFL_,"","","function_code = [%s]", function_code);
		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) return iRetCode;
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 begin*/	
		if(strcmp(opCode,"1219") == 0)
		{   
			 
			printf("function_code=%s\n",function_code);     
			/*delete by liujj for �����û���ͨ,ȡ���ײͰ󶨵��ط� at 2011-04-06*/
			/*init(dynStmt);
			sprintf(dynStmt,	"SELECT count(*) "
								"  FROM dBillCustDetail%c a, sBillFuncBind b"
								" WHERE a.DETAIL_CODE = b.FUNCTION_BIND"
								"   AND a.DETAIL_TYPE = 'a'"
								"   AND b.function_code = :v0 "
								"   AND b.region_code = substr(:v1, 1,2)"
								"   AND begin_time <= sysdate"
								"   AND end_time > sysdate"
								"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
								,idNo[strlen(idNo)-1]);
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :iCountBind using :function_code,:belongCode,:idNo;
			END;
			END-EXEC;
			if(SQLCODE != 0 && SQLCODE != 1403)    
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ�ײͰ󶨵��ط�ʧ��!SQLCODE=%d",SQLCODE);
				return 121931;
			}*/
			 /* 2011-1-26  ���55 �ط� �ײͰ󶨺���Խ����ط�55 ��ͨ���� */	
			/*delete by liujj for �����û���ͨ,ȡ���ײͰ󶨵��ط� at 2011-04-06
			if ( (iCountBind > 0 ) && (strcmp(function_code,"55")!=0))
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�������б��!");
				return 121932;
			
			}   */                                                  
        	EXEC SQL select count(*) into :iCount from SFUNCSENDMSG where region_code=substr(:belongCode,1,2) 
        		AND sm_code = :smCode and function_code=:function_code;	
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ SFUNCSENDMSG ʧ��!SQLCODE=%d",SQLCODE);
				return 121933;
			}
			printf("iCount=%d\n",iCount);
			if(iCount>0)
			{   
				rspno=0;
				rspno=rand()%900000+100000;
				
				Sinitn(dynStmt);     
				sprintf(dynStmt,	"SELECT  count(*) "
						"  FROM dBillCustDetail%c a, sBillFuncFav b"
						" WHERE a.DETAIL_CODE = b.FUNCTION_BILL"
						"   AND a.DETAIL_TYPE = '4'"
						"   AND b.function_code = :v0 "
						"   AND b.region_code = substr(:v1, 1,2)"
						"   AND begin_time <= sysdate"
						"   AND end_time > sysdate"
						"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
						,idNo[strlen(idNo)-1]);
				EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :dynStmt INTO :iCountFav using :function_code,:belongCode,:idNo;
				END;
				END-EXEC;
				if(SQLCODE != 0 && SQLCODE != 1403)    
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121922","ȡ�ط��Żݴ���!SQLCODE=%d",SQLCODE);
					return 121922;
				}
				printf("---6915��-rspno[%ld][%s]---------\n",rspno,phoneNo);
				if(iCountFav==0)
				{  
					/*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� begin*/
					tLoginAccept=0;
			        EXEC SQL select sMaxSysAccept.nextval
			        		   into :tLoginAccept
			        		   from dual;
				    if(SQLCODE!=0)
				    {
				        printf("\nȡ��ˮ[%s][%d]",SQLERRMSG,SQLCODE);
				        return -3;
				    }
				    printf("----rspno[%ld][%ld][%s]---------\n",rspno,tLoginAccept,phoneNo);
				    /*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� end*/
				   	Sinitn(dynStmt);     
					sprintf(dynStmt,
						"INSERT INTO dCrmRemindMsg "
						"("
						"	MAXACCEPT, ID_NO , PHONE_NO, BIZTYPE,	SPID,BIZCODE,BIZNAME,"
						"	RSP_NO , OP_TIME, SERV_PROPERTY,TOTAL_DATE,SEND_TIME,OUT_TIME,CONFIRM_FLAG "
						")"
						"select"
						"  :v1,	to_number(:v2),	:v3, '00',"
						"  FUNCTION_TYPE,:v4,FUNCTION_NAME,:v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),'3', :v7,"
						"  to_date(:v8, 'yyyymmdd hh24:mi:ss'),to_date(:v9, 'yyyymmdd hh24:mi:ss'),'0'"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v10,1,2)"
						"   AND sm_code = :v11"
						"   AND function_code = :v12"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :tLoginAccept,:idNo, :phoneNo,:function_code,:rspno,
						 :opTime,:totalDate,:opTime,:outtime,:belongCode,:smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121924","�����dCrmRemindMsgʧ��[%d]!SQLCODE=%d",rspno,SQLCODE);
						return 121924;
					}
					
					#ifdef _CHGTABLE_
					Sinitn(dynStmt);     
					sprintf(dynStmt, 
						"INSERT INTO wfuncremindchg "
						"("
						"	OP_NO,		ID_NO,		PHONE_NO,	MAXACCEPT, OP_TYPE,"
						"	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME, FEE_FLAG,FLAG_TIME "
						")"
						"select"
						"  sMaxSysAccept.nextval,	to_number(:v2),	:v3, :v4, 'i',"
						"	FUNCTION_TYPE, :v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),0,to_date(:v7, 'yyyymmdd hh24:mi:ss')"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v8,1,2)"
						"   AND sm_code = :v9"
						"   AND function_code = :v10"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :idNo, :phoneNo,:tLoginAccept,
						:function_code, :opTime,:opTime,:belongCode, :smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121925","�����wfuncremindchgʧ��!SQLCODE=%d",SQLCODE);
						return 121925;
					}
					#endif
				}
			}
			
		}
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 end*/	
		
		/*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
		
		ret = sGetLoginOrgid(loginNo,vOrgId);
		if(ret <0)
		{
			pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��!");
			return 200919; 
		}
		Trim(vOrgId);
		/*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
		/*add by zwm ��������*/
		if((strcmp(function_code,"15") == 0) || (strcmp(function_code,"16") == 0) || (strcmp(function_code,"17") == 0) || (strcmp(function_code,"23") == 0) || (strcmp(function_code,"29") == 0))
		{
			Trim(idNo);
			Sinitn(sqlStrfunc);
			sprintf(sqlStrfunc,"select function_code||'|' FROM dcustfunc WHERE id_no=to_number(:v1) AND function_code in ('15','16','17','23','29')");
			EXEC SQL PREPARE SqlStmting FROM :sqlStrfunc;
			EXEC SQL DECLARE func_curing CURSOR FOR SqlStmting;
			EXEC SQL OPEN func_curing using :idNo;
			for(;;)
			{
				Sinitn(delFuncStr);
				EXEC SQL FETCH func_curing INTO :delFuncStr;
				if(SQLCODE == 1403) break;
				if(SQLCODE != 0 && SQLCODE != 1403)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","201005","SELECT dcustfunc ERROR!");
					return 201005;
				}
				
				sprintf(retCode, "%06d", fDelFuncs(funcEnv, delFuncStr));
				if ( strcmp(retCode, "000000") != 0)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","201006","ȡ���ط�����!");
					return 201006;
				}
			}
			EXEC SQL CLOSE func_curing;
		}
		Sinitn(dynStmt);
		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
			:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121914","��ȡ�û���Ϣʧ��!");
			return 121914;
		}
	
		EXEC SQL select count(*) into :num from dconmsg 
				where contract_no in(select contract_no from  dconusermsg where id_no=:idNo and serial_no=0)
						and pay_code='4';
	/*�����������հ��� �Ż�by cencm at 2011-3-29 10:57:28--begin---*/
	/*3077 ������ط� �����յ���Ҫ������� ��R3077��������,�ڴ˶�3077�����������*/
	/*if(num>0&&(strcmp(function_code,"00")==0||strcmp(function_code,"17")==0))*/
	if((num>0&&(strcmp(function_code,"00")==0||strcmp(function_code,"17")==0)) || strcmp(opCode,"3077") == 0)
	/*�����������հ��� �Ż�by cencm at 2011-3-29 10:57:28--end---*/
	/*printf("This is a specular user,vRemainFee=[%f]\n",vRemainFee);
	*/
	pubLog_Debug(_FFL_,"","","This is a specular user,vRemainFee=[%f]",vRemainFee);
	else
	{
		Sinitn(vLimiName);
		EXEC SQL select nvl(limit_prefee,0),function_name
			into :vLimitPay,:vLimiName
			from sFuncList
			 WHERE region_code=substr(:belongCode,1,2)
			   AND sm_code = :smCode
			   AND function_code = :function_code;
		if(SQLCODE!=0)
		{
			/*printf("121957 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121957", "SqlCode =[%d]", SQLCODE);
			return 121957;
		}
		if(vLimitPay>vRemainFee)
		{
			/*printf("121958 vLimitPay  =[%f]\n", vLimitPay);
			printf("121958 vRemainFee =[%f]\n", vRemainFee);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121958", "121958 vLimitPay  =[%f],vRemainFee =[%f]",vLimitPay,vRemainFee);
			sprintf(funcode,"[%s]%s",function_code,vLimiName);
			*olimitFee=vLimitPay;
			return 121958;
		}
	}
		/*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFunc"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v3,1,2)"
			"   AND sm_code = :v4"
			"   AND function_code = :v5"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
		}*/
		Trim(idNo);
		
		EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE,	:opTime into :sTdCustFunc 
	 	  FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code ;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121974","��ѯ�ط���Ϣ����");
			return 121974;		
		}
		ret = 0; 		
	  ret=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedLimitFuncs",sTdCustFunc);
		if (ret!=0)
	  {    				  
	  	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
	  }

		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
		}
    */
    Trim(idNo);
    EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept, :totalDate, :opTime, :opCode, 'a'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	  if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;		
		}
		ret = 0; 					
  	ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedLimitFuncs",sTdCustFuncHis);
		if (ret != 0)
  	{    				  
  	  PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
  	}
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAdd"
			"("
			"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
			"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
			"add_months(to_date(:v3, 'yyyymmdd hh24:mi:ss'), month_limit), MONTH_LIMIT, NULL, NULL"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v4,1,2)"
			"   AND sm_code = :v5"
			"   AND function_code = :v6"
			"   AND month_limit > 0");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:belongCode,
			:smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
			return 121917;
		}*/
		
		Trim(idNo);
		Trim(opTime);
		Trim(belongCode);
		Trim(smCode);
		Trim(function_code);
		EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE,	:opTime,
			 to_char(add_months(to_date(:opTime, 'yyyymmdd hh24:mi:ss'), month_limit),'yyyymmdd hh24:mi:ss'), to_char(MONTH_LIMIT), chr(0), chr(0) into :sTdCustFuncAdd 
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code 
			 AND month_limit > 0;
		if(SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121978","��ѯ�ط���Ϣ����");
			return 121978;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 	
			ret=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedLimitFuncs",sTdCustFuncAdd);
			if (ret < 0)
	    	{    				  
	      	PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
					return 121917;
	    	}
	    }
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
						:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			//printf("121918 SqlCode = [%d]\n", SQLCODE);
			
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
			return 121918;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE, nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		   nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),
			 nvl(ADD_NO,chr(0)),nvl(OTHER_ATTR,chr(0)),:loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a'
			 into :sTdCustFuncAddHis FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
			return 121918;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 		 	
			ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedLimitFuncs",sTdCustFuncAddHis);
			if (ret < 0)
	  	{    			
	  	  PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
				return 121918;
	  	}
	    }
    	/*ng���� by zhaohx at 04/08/2009 end*/
		if (hasExpire)
		{ /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
            Sinitn(dynStmt);        
			sprintf(dynStmt,
				"INSERT INTO wCustExpire"
				"("
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
				")"
				" SELECT"
				"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
				"	:v4,:vOrgId,	:v5,	to_number(:v6),"
				"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
				"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��'"
				"  FROM sFuncList"
				" where region_code=substr(:v12, 1,2)"
				"   and sm_code = :v13"
				"   and function_code = :v14");
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
								:orgCode,:vOrgId,	:loginNo,	:loginAccept,
								:totalDate, :opCode,	:opTime, :function_code,
								:end_date, 
								:belongCode, :smCode, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{
				/*printf("121919 SqlCode = [%d]\n", SQLCODE);
				*/
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121919","121919 SqlCode = [%d]", SQLCODE);
				return 121919;
			}

		}
		iRetCode = fSendCmd(funcEnv, '1', "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","fsencmdʧ��",iRetCode);
			return iRetCode;
		}
/*��������Ͷ�ߵ����� �ڿ�ͨ�������ε�ʱ�� ��Ҫ���͹���ֱ����ָ�� �Է�ֹ������ƽ̨���ݲ�һ�� add by cencm---begin*/
		if(strcmp(function_code,"17")==0)
		{
			iRetCode=0;
			iRetCode = fSendCmd(funcEnv, '1', "1219", "00");
			if(iRetCode != 0)
			{	
				pubLog_DBErr(_FFL_,"","%d","fsencmdʧ��",iRetCode);
				return iRetCode;
			}
		}
/*��������Ͷ�ߵ����� �ڿ�ͨ�������ε�ʱ�� ��Ҫ���͹���ֱ����ָ�� �Է�ֹ������ƽ̨���ݲ�һ�� add by cencm---end*/
	}
	return 0;
}

/*����ԤԼ��Ч���ط� ��Ԥ������
*��ʽ:����ʽ:'AAYYYYMMDDYYYYMMDD|' + 'AAYYYYMMDDYYYYMMDD|'  +  'AAYYYYMMDDYYYYMMDD|' + ������
*      "AA"��ʾ�ط����룬"YYYYMMDDYYYYMMDD"��ʾ��ʼ���ں͵������ڣ�"|"���ָ�����
*/
int fAddUnUsedLimitFuncs(tFuncEnv *funcEnv, char *funcs,double iRemainFee,double *olimitFee,char *funcode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char begin_date[8+1];
	char end_date[8+1];
	char iRetCode = 0;
	int ret =0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char 	vOrgId[10+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	char	vLimiName[20+1];
	double  vRemainFee=0;
	double  vLimitPay=0;
	int  	iCount=0;
	int  	iCountFav=0;
	char	outtime[17+1];
	long	rspno =0;
	long	tLoginAccept=0;
	int iCountBind=0;
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vLimiName);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(vOrgId);
	Sinitn(orgCode);
	Sinitn(outtime);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	
	vRemainFee=iRemainFee;

	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;
	
	
	EXEC SQL select to_char(sysdate+70/(24*60),'yyyymmdd HH24:MI:SS') into :outtime from dual;

	/*printf(" ==== funcs = [%s]\n",funcs);
	*/
	
	srand(time(NULL));
	
	pubLog_Debug(_FFL_,"",""," ==== funcs = [%s]",funcs);
	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		strncpy(begin_date, p+2, 8);
		begin_date[8] = '\0';
		strncpy(end_date, p+10, 8);
		end_date[8] = '\0';
		p = q + 1;
		Sinitn(vLimiName);
		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) return iRetCode;
		
		EXEC SQL select nvl(limit_prefee,0),function_name
			into :vLimitPay,:vLimiName
			from sFuncList
			 WHERE region_code=substr(:belongCode,1,2)
			   AND sm_code = :smCode
			   AND function_code = :function_code;
		if(SQLCODE!=0)
		{
			/*printf("121955 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121955","121955 SqlCode =[%d]", SQLCODE);
			return 121955;
		}
		if(vLimitPay>vRemainFee)
		{
			/*printf("121956 vLimitPay  =[%f]\n", vLimitPay);
			printf("121956 vRemainFee =[%f]\n", vRemainFee);
			*/
			pubLog_DBErr(_FFL_,"","121956","121956 vLimitPay  =[%f],vRemainFee =[%f]", vLimitPay,vRemainFee);
			sprintf(funcode,"[%s]%s",function_code,vLimiName);
			*olimitFee=vLimitPay;
			return 121956;
		}
		 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
		 /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/

       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
         	  /*printf("��ȡ�û�org_idʧ��!\n");
         	  */
         	  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��!");			
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
		
	  /* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 begin*/	
		if(strcmp(opCode,"1219") == 0)
		{    
			/*srand(time(NULL)); */
			printf("function_code=%s\n",function_code);  
			
			init(dynStmt);
			sprintf(dynStmt,	"SELECT count(*) "
								"  FROM dBillCustDetail%c a, sBillFuncBind b"
								" WHERE a.DETAIL_CODE = b.FUNCTION_BIND"
								"   AND a.DETAIL_TYPE = 'a'"
								"   AND b.function_code = :v0 "
								"   AND b.region_code = substr(:v1, 1,2)"
								"   AND begin_time <= sysdate"
								"   AND end_time > sysdate"
								"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
								,idNo[strlen(idNo)-1]);
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :iCountBind using :function_code,:belongCode,:idNo;
			END;
			END-EXEC;
			if(SQLCODE != 0 && SQLCODE != 1403)    
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ�ײͰ󶨵��ط�ʧ��!SQLCODE=%d",SQLCODE);
				return 121921;
			}
				
			if ( iCountBind > 0)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�������б��!");
				return 121921;
			
			}                            
			                                   
    		EXEC SQL select count(*) into :iCount from SFUNCSENDMSG where region_code=substr(:belongCode,1,2) 
    		AND sm_code = :smCode and function_code=:function_code;	
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121957","��ѯ SFUNCSENDMSG ʧ��!SQLCODE=%d",SQLCODE);
				return 121957;
			}
			printf("iCount=%d\n",iCount);
			if(iCount>0)
			{   
				rspno=0;
				rspno=rand()%900000+100000;
				Sinitn(dynStmt);     
				sprintf(dynStmt,	"SELECT  count(*) "
						"  FROM dBillCustDetail%c a, sBillFuncFav b"
						" WHERE a.DETAIL_CODE = b.FUNCTION_BILL"
						"   AND a.DETAIL_TYPE = '4'"
						"   AND b.function_code = :v0 "
						"   AND b.region_code = substr(:v1, 1,2)"
						"   AND begin_time <= sysdate"
						"   AND end_time > sysdate"
						"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
						,idNo[strlen(idNo)-1]);
				EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :dynStmt INTO :iCountFav using :function_code,:belongCode,:idNo;
				END;
				END-EXEC;
				if(SQLCODE != 0 && SQLCODE != 1403)    
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121958","ȡ�ط��Żݴ���!SQLCODE=%d",SQLCODE);
					return 121958;
				}
				
				if(iCountFav==0)
				{  
					
					/*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� begin*/
					tLoginAccept=0;
			        EXEC SQL select sMaxSysAccept.nextval
			        		   into :tLoginAccept
			        		   from dual;
				    if(SQLCODE!=0)
				    {
				        printf("\nȡ��ˮ[%s][%d]",SQLERRMSG,SQLCODE);
				        return -3;
				    }
				    /*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� end*/
				   	Sinitn(dynStmt);     
					sprintf(dynStmt,
						"INSERT INTO dCrmRemindMsg "
						"("
						"	MAXACCEPT, ID_NO , PHONE_NO, BIZTYPE,	SPID,BIZCODE,BIZNAME,"
						"	RSP_NO , OP_TIME, SERV_PROPERTY,TOTAL_DATE,SEND_TIME,OUT_TIME,CONFIRM_FLAG "
						")"
						"select"
						"  :v1,	to_number(:v2),	:v3, '00',"
						"  FUNCTION_TYPE,:v4,FUNCTION_NAME,:v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),'3', :v7,"
						"  to_date(:v8, 'yyyymmdd hh24:mi:ss'),to_date(:v9, 'yyyymmdd hh24:mi:ss'),'0'"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v10,1,2)"
						"   AND sm_code = :v11"
						"   AND function_code = :v12"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :tLoginAccept,:idNo, :phoneNo,:function_code,:rspno,
						 :opTime,:totalDate,:opTime,:outtime,:belongCode,:smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121959","�����dCrmRemindMsgʧ��!SQLCODE=%d",SQLCODE);
						return 121959;
					}
					
					#ifdef _CHGTABLE_
					Sinitn(dynStmt);     
					sprintf(dynStmt, 
						"INSERT INTO wfuncremindchg "
						"("
						"	OP_NO,		ID_NO,		PHONE_NO,	MAXACCEPT, OP_TYPE,"
						"	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME, FEE_FLAG,FLAG_TIME "
						")"
						"select"
						"   sMaxSysAccept.nextval,	to_number(:v2),	:v3, :v4, 'i',"
						"	FUNCTION_TYPE, :v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),0 ,to_date(:v7, 'yyyymmdd hh24:mi:ss')"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v8,1,2)"
						"   AND sm_code = :v9"
						"   AND function_code = :v10"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :idNo, :phoneNo, :tLoginAccept,
						:function_code, :opTime, :opTime,:belongCode, :smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121960","�����wfuncremindchgʧ��!SQLCODE=%d",SQLCODE);
						return 121960;
					}
					#endif
				}
			}
			
		}
	  /* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 end*/	
		sprintf(dynStmt,
			"INSERT INTO wCustExpire"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
			")"
			"SELECT"
			"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
			"	:v4,:vOrgId,	:v5,	to_number(:v6),"
			"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
			"	'1',	to_date(:v11, 'yyyymmdd'),	'�ط����ڿ���' "
			"  FROM sFuncList"
			" where region_code=substr(:v12, 1,2)"
			"   and sm_code = :v13"
			"   and function_code = :v14");

		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
							:orgCode,:vOrgId,	:loginNo,	:loginAccept,
							:totalDate, :opCode,	:opTime, :function_code,
							:begin_date, 
							:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			/*printf("121920 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121920","121920 SqlCode =[%d]", SQLCODE);
			return 121920;
		}
 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/

		sprintf(dynStmt,
			"INSERT INTO wCustExpire"
			"("
			"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
			"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
			"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
			"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
			")"
			"SELECT"
			"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
			"	:v4,:vOrgId,	:v5,	to_number(:v6),"
			"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
			"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��' "
			"  FROM sFuncList"
			" where region_code=substr(:v12, 1,2)"
			"   and sm_code = :v13"
			"   and function_code = :v14");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
							:orgCode,:vOrgId,	:loginNo,	:loginAccept,
							:totalDate, :opCode,	:opTime, :function_code,
							:end_date, 
							:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			/*printf("121921 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121921","121921 SqlCode =[%d]", SQLCODE);
			return 121921;
		}
	}
	return 0;
}

/************************************************************************
��������:fGetGrpBaseInfo
����ʱ��:2004/08/25
������Ա:liab
��������:���ݼ��ű�Ų�ѯ���ŵĻ�����Ϣ
�������:���ű��
�������:���Ż�����Ϣ
�û�id      
�ͻ�id      
belong_code
ҵ�����ʹ���  (�����û�����Ϊ��)
ҵ����������  (�����û�����Ϊ��)
�ͻ�����    
�û�����      
״̬����    
״̬����    
�ȼ�����    
�ȼ�����    
�û����ʹ���
�û���������
�ͻ�סַ    
֤������    
֤������    
֤������    
�ͻ�������
�ͻ�������
���ſͻ���־
���ſͻ�����
bak_field
�û�Ƿ��(tUserOwe)	
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fGetGrpBaseInfo(const char *group_no,tUserBase *user_base)
{	   
	EXEC SQL BEGIN DECLARE SECTION;
	/***�������õ���������****/	     
	char	    vgroup_no[20+1];	     /* ���ű��      */
	char	    vid_no[23];		  /* �û�ID	*/
	char	    vcust_id[23];		/* �ͻ�ID	*/
	char	    vsm_code[2+1];	       /* ҵ�����      */
	char	    vsm_name[20+1];	      /* ҵ������      */
	char	    vcust_name[60+1];	    /* ��������      */
	char	    vuser_passwd[8+1];	   /* ҵ����������  */
	char	    vrun_code[1+1];	      /* ����״̬      */
	char	    vrun_name[20+1];	     /* ����״̬����  */
	char	    vowner_grade[2+1];	   /* �ȼ�����      */
	char	    vgrade_name[20+1];	   /* �ȼ�����      */
	char	    vowner_type[2+1];	    /* �û����ʹ���  */
	char	    vtype_name[20+1];	    /* �û���������  */
	char	    vcust_address[60+1];	 /* �ͻ�סַ      */
	char	    vid_type[1+1];	       /* ֤������      */
	char	    vid_name[12+1];	      /* ֤������      */
	char	    vid_iccid[20+1];	     /* ֤������      */
	char	    vcard_type[2+1];	     /* �ͻ�������    */
	char	    vcard_name[20+1];	    /* �ͻ�������    */
	char	    vvip_no[20+1];	       /* VIP����       */
	char	    vgrpbig_flag[1+1];	   /* ���ſͻ���־  */
	char	    vgrpbig_name[20+1];	  /* ���ſͻ�����  */
	char	    vcontract_no[23];	    /* Ĭ���ʺ�      */
	double	  vowe_fee=0;		  /* Ƿ��	  */
	double	  vprepay_fee=0;	       /* Ԥ���	*/
	char	    vregion_code[2+1];	   /* ��������      */
	char	    vBelongCode[8];	      /* ��������      */
	char	    vst[512];		    /* ��ʱ����      */
	char	    vgroup_id[10+1];
	int	     vtmp_num=0;
	int	     vret=0;
	char	    vcredit_code[1+1];
	EXEC SQL END DECLARE SECTION;     

	memset(vgroup_no,0,sizeof(vgroup_no));
	memset(vcredit_code,0,sizeof(vcredit_code));
	memset(user_base->id_no,0,sizeof(user_base->id_no));
	memset(user_base->cust_id,0,sizeof(user_base->cust_id));
	memset(user_base->contract_no,0,sizeof(user_base->contract_no));
	memset(user_base->belong_code,0,sizeof(user_base->belong_code));
	memset(user_base->sm_code,0,sizeof(user_base->sm_code));
	memset(user_base->sm_name,0,sizeof(user_base->sm_name));
	memset(user_base->cust_name,0,sizeof(user_base->cust_name));
	memset(user_base->user_passwd,0,sizeof(user_base->user_passwd));
	memset(user_base->run_code,0,sizeof(user_base->run_code));
	memset(user_base->run_name,0,sizeof(user_base->run_name));
	memset(user_base->owner_grade,0,sizeof(user_base->owner_grade));
	memset(user_base->grade_name,0,sizeof(user_base->grade_name));
	memset(user_base->owner_type,0,sizeof(user_base->owner_type));
	memset(user_base->type_name,0,sizeof(user_base->type_name));
	memset(user_base->cust_address,0,sizeof(user_base->cust_address));
	memset(user_base->id_type,0,sizeof(user_base->id_type));
	memset(user_base->id_name,0,sizeof(user_base->id_name));
	memset(user_base->id_iccid,0,sizeof(user_base->id_iccid));
	memset(user_base->card_type,0,sizeof(user_base->card_type));
	memset(user_base->card_name,0,sizeof(user_base->card_name));
	memset(user_base->grpbig_flag,0,sizeof(user_base->grpbig_flag));
	memset(user_base->group_id,0,sizeof(user_base->group_id));
	memset(user_base->grpbig_name,0,sizeof(user_base->grpbig_name));
	memset(user_base->bak_field,0,sizeof(user_base->bak_field));
	memset(user_base->user_owe.accountNo,0,sizeof(user_base->user_owe.accountNo));
	user_base->user_owe.totalOwe=0;
	user_base->user_owe.totalPrepay=0;
	user_base->user_owe.accountOwe=0;
	user_base->user_owe.unbillFee=0;

	memset(vid_no,0,sizeof(vid_no));
	memset(vcust_id,0,sizeof(vcust_id));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vBelongCode,0,sizeof(vBelongCode));
	memset(vsm_code,0,sizeof(vsm_code));
	memset(vsm_name,0,sizeof(vsm_name));
	memset(vcust_name,0,sizeof(vcust_name));
	memset(vuser_passwd,0,sizeof(vuser_passwd));
	memset(vrun_code,0,sizeof(vrun_code));
	memset(vrun_name,0,sizeof(vrun_name));
	memset(vowner_grade,0,sizeof(vowner_grade));
	memset(vgrade_name,0,sizeof(vgrade_name));
	memset(vowner_type,0,sizeof(vowner_type));
	memset(vtype_name,0,sizeof(vtype_name));
	memset(vcust_address,0,sizeof(vcust_address));
	memset(vid_type,0,sizeof(vid_type));
	memset(vid_name,0,sizeof(vid_name));
	memset(vid_iccid,0,sizeof(vid_iccid));
		memset(vgroup_id,0,sizeof(vgroup_id));
	memset(vcard_type,0,sizeof(vcard_type));
	memset(vcard_name,0,sizeof(vcard_name));
	memset(vvip_no,0,sizeof(vvip_no));
	memset(vgrpbig_flag,0,sizeof(vgrpbig_flag));
	memset(vgrpbig_name,0,sizeof(vgrpbig_name));

	memset(vregion_code,0,sizeof(vregion_code));
	memset(vst,0,sizeof(vst));
	vtmp_num = 0;

	memcpy(vgroup_no,group_no,sizeof(vgroup_no)-1);

	/*��ѯdCustMsg��*/
	memset(vid_no,       0, sizeof(vid_no));
	memset(vcust_id,     0, sizeof(vcust_id));
	memset(vuser_passwd, 0, sizeof(vuser_passwd));
	memset(vrun_code,    0, sizeof(vrun_code));
	memset(vBelongCode,  0, sizeof(vBelongCode));

#ifdef _DEBUG_
     /* printf("debug msg: String sub function!\n");
        printf("debug msg: vgroup_no=[%s]!\n",vgroup_no);
     */
      pubLog_Debug(_FFL_,"","","debug msg: String sub function!");  
       pubLog_Debug(_FFL_,"","","debug msg: vgroup_no=[%s]!",vgroup_no); 
#endif	 

	
	strcpy(vgrpbig_flag,"1");	 /* Ĭ�ϼ�����ͨ�ͻ� */


	EXEC SQL SELECT to_char(id_no),	 to_char(cust_id),
			trim(user_name),	user_passwd,
			run_code,	       region_code||district_code||town_code,
			region_code,
			to_char(account_id),group_id, credit_code
		INTO   :vid_no,		:vcust_id,
		       :vcust_name,	    :vuser_passwd,
		       :vrun_code,	     :vBelongCode,
		       :vregion_code,
		       :vcontract_no,:vgroup_id,:vcredit_code
	  FROM dGrpUserMsg
	  WHERE user_no=:vgroup_no AND run_code<'a';
	  
	if(SQLCODE!=0)
	{
			/*printf("SQLCODE=[%d][%s]!\n",SQLCODE,SQLERRMSG);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1240","SQLCODE=[%d][%s]!",SQLCODE,SQLERRMSG);
			return 1240;
	}

#ifdef _DEBUG_
     /* printf("debug msg: end of dGrpUserMsg !\n");
     */
     pubLog_Debug(_FFL_,"","","debug msg: end of dGrpUserMsg !");
#endif	 
	
	EXEC SQL SELECT sm_code,     substr(attr_code,3,2)
		  INTO :vsm_code,   :vcard_type
	  FROM dCustMsg
	  WHERE id_no=:vid_no AND substr(run_code,2,1)<'a';
	  
	if (SQLCODE == 0)
	{
	     vtmp_num ++;
	}
	  
	if(SQLCODE!=0 && SQLCODE!=1403)
	{
		/*printf ("1207 333 %d, %s\n", SQLCODE, SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","1207 333 %d, %s", SQLCODE, SQLERRMSG);
		return 1207;
	}
	

#ifdef _DEBUG_
      /*printf("debug msg: end of dCustMsg !\n");
      */
      pubLog_Debug(_FFL_,"","","debug msg: end of dCustMsg !");
#endif	 

	/*��ѯdCustDoc��*/
	EXEC SQL SELECT owner_grade,	  owner_type,	 cust_address,
			id_type,	      id_iccid
		  INTO :vowner_grade,	:vowner_type,       :vcust_address,
		       :vid_type,	    :vid_iccid
		  FROM dCustDoc
		  WHERE cust_id=to_number(:vcust_id);
		  
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","ȡ�û���Ϣ����");
		return 1208;
	}

#ifdef _DEBUG_
      /*printf("debug msg: end of dCustDoc !\n");
      */
      pubLog_Debug(_FFL_,"","","debug msg: end of dCustDoc !");
#endif	 

	/*��ѯsSmCode��*/
	EXEC SQL  SELECT sm_name INTO :vsm_name
			FROM sSmCode
		  WHERE region_code=:vregion_code
		  AND sm_code=:vsm_code;
		  
	if(SQLCODE!=0 && SQLCODE != 1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1209","ȡ�û���Ϣ����");
		return 1209;
	}

#ifdef _DEBUG_
     /* printf("debug msg: end of sSmCode !\n");
     */
      pubLog_Debug(_FFL_,"","","debug msg: end of sSmCode !");
#endif	 


	/*��ѯsRunCode��*/
	EXEC SQL  SELECT run_name INTO :vrun_name
			 FROM sRunCode
		  WHERE region_code=:vregion_code
		  AND run_code=:vrun_code;
	if(SQLCODE!=0 && SQLCODE != 1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1210","ȡ�û���Ϣ����");
		return 1210;
	}		
#ifdef _DEBUG_
     /* printf("debug msg: end of sRunCode !\n");
     */
      pubLog_Debug(_FFL_,"","","debug msg: end of sRunCode !");
#endif	 


	/*��ѯsCustGradeCode��*/
	/*
	EXEC SQL  SELECT type_name INTO :vgrade_name
			 FROM sCustGradeCode
		  WHERE region_code=:vregion_code
		  and owner_code=:vowner_grade;
		  
	if(SQLCODE!=0)
	{
		return 1210;
	}
	*/
	strcpy(vgrade_name, "");
#ifdef _DEBUG_
      /*printf("debug msg: end of sCustGradeCode !\n");
      */
       pubLog_Debug(_FFL_,"","","debug msg: end of sCustGradeCode !");
#endif	 

	/*��ѯsCustTypeCode��*/
	EXEC SQL  SELECT type_name INTO :vtype_name
			 FROM sCustTypeCode
		  WHERE type_code=:vowner_type;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1211","ȡ�û���Ϣ����");
		return 1211;
	}
	
#ifdef _DEBUG_
      /*printf("debug msg: end of sCustTypeCode !\n");
      */
      pubLog_Debug(_FFL_,"","","debug msg: end of sCustTypeCode !");
#endif	 

	/*��ѯsIdTtype��*/
	EXEC SQL SELECT id_name INTO :vid_name
			FROM sIdType
		 WHERE id_type=:vid_type;
		 
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1212","ȡ�û���Ϣ����");
		return 1212;
	}

#ifdef _DEBUG_
     /* printf("debug msg: end of sIdType !\n");
     */
     pubLog_Debug(_FFL_,"","","debug msg: end of sIdType !");
#endif	 

	/* ��ѯ sBigCardCode */
	EXEC SQL select card_name into :vcard_name
		from  sBigCardCode
		where card_type = :vcard_type;
		
	if(SQLCODE != 0 && SQLCODE != 1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1213","ȡ�û���Ϣ����");
		return 1213;
	}

#ifdef _DEBUG_
      /*printf("debug msg: end of sBigCardCode !\n");
      */
      pubLog_Debug(_FFL_,"","","debug msg: end of sBigCardCode !");
#endif	 

	/* ��ѯ sGrpBigFlag */
	EXEC SQL select grp_name into :vgrpbig_name
		from  sGrpBigFlag
		where grp_flag = :vgrpbig_flag;
	if(SQLCODE != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1214","ȡ�û���Ϣ����");
		return 1214;
	}

#ifdef _DEBUG_
      /*printf("debug msg: end of sGrpBigFlag !\n");
      */
       pubLog_Debug(_FFL_,"","","debug msg: end of sGrpBigFlag !");
#endif	 

	/*
	 * �ʻ���Ƿ�Ѻ�Ԥ��
	 *
	 * ע��:�����û�����dCustMsg��������Ϣ���ѯǷ��,
	 *      ���򲻲�ѯǷ��
	 */
	if (vtmp_num > 0)
	{
	    vret=fGetUserOwe(vid_no,&user_base->user_owe);
	    if(vret!=0)
	    {
	    pubLog_DBErr(_FFL_,"","-1","��ȡ�û�������Ϣʧ��");	
		return -1;
	    }    
	}
	
#ifdef _DEBUG_
      /*printf("debug msg: end of fGetUserOwe !\n");
      */
    pubLog_Debug(_FFL_,"","","debug msg: end of fGetUserOwe !");  
#endif	 

	/*��ֵ*/
	Trim(vid_no);	strcpy(user_base->id_no,vid_no);
	Trim(vcust_id);      strcpy(user_base->cust_id,vcust_id);
	Trim(vcontract_no);  strcpy(user_base->contract_no,vcontract_no);
	Trim(vBelongCode);   strcpy(user_base->belong_code,vBelongCode);
	Trim(vsm_code);      strcpy(user_base->sm_code,vsm_code);
	Trim(vsm_name);      strcpy(user_base->sm_name,vsm_name);
	Trim(vuser_passwd);  strcpy(user_base->cust_name,vcust_name);
	Trim(vuser_passwd);  strcpy(user_base->user_passwd,vuser_passwd);
	Trim(vrun_code);     strcpy(user_base->run_code,vrun_code);
	Trim(vrun_name);     strcpy(user_base->run_name,vrun_name);
	Trim(vowner_grade);  strcpy(user_base->owner_grade,vowner_grade);
	Trim(vgrade_name);   strcpy(user_base->grade_name,vgrade_name);

	Trim(vowner_type);   strcpy(user_base->owner_type,vowner_type);
	Trim(vtype_name);    strcpy(user_base->type_name,vtype_name);

	Trim(vcust_address); strcpy(user_base->cust_address,vcust_address);
	Trim(vid_type);      strcpy(user_base->id_type,vid_type);
	Trim(vid_name);      strcpy(user_base->id_name,vid_name);
	Trim(vid_iccid);     strcpy(user_base->id_iccid,vid_iccid);

	Trim(vcard_type);    strcpy(user_base->card_type,vcard_type);
	Trim(vcard_name);    strcpy(user_base->card_name,vcard_name);
	Trim(vgrpbig_flag);  strcpy(user_base->grpbig_flag,vgrpbig_flag);
	Trim(vgrpbig_name);  strcpy(user_base->grpbig_name,vgrpbig_name);
	Trim(vgroup_id);     strcpy(user_base->group_id,vgroup_id);
	Trim(vcredit_code);  strcpy(user_base->credit_code, vcredit_code);

#ifdef _DEBUG_
     /* printf("debug msg: end of sub function !\n");
     */
     pubLog_Debug(_FFL_,"","","debug msg: end of sub function !");  
#endif	 

	return 0;
}
/* Author: lixg
   Date:   20070410
   Func:   get Work all Function Right
   Input:  LoginNo, pdtCode
   Out:    retCode
*/

int getWorkNoRightList(char *loginNo){
	EXEC SQL BEGIN DECLARE SECTION;
	char	vLoginNo[6+1];
	EXEC SQL END DECLARE SECTION;
	
	/* ������ʼ�� */
	init(vLoginNo);
	strncpy(vLoginNo, loginNo, 6);
	
#ifdef _DEBUG_
	/*printf("getWorkNoRightList: loginNo = [%s]\n", vLoginNo);
	*/
	 pubLog_Debug(_FFL_,"","","getWorkNoRightList: loginNo = [%s]", vLoginNo); 
#endif   

	/* ��ȡ���ŵ�����Ȩ�޵���ʱ��sMidLoginPdomCode */	      
	EXEC SQL 	insert into sMidLoginPdomCode(login_no, popedom_code)
		(select a.login_no login_no , b.popedom_code popedom_code
		 from sLoginRoalRelation a, sPopeDomCode b, sRolePdomRelation c
		where  a.login_no =  :vLoginNo
		and    a.role_code = c.role_code
		and    b.popedom_code = c.popedom_code
		and    b.use_flag = 'Y'
		union
		select login_no login_no , b.par_domcode  popedom_code from sLoginPdomRelation a, sPopeDomInfo b
		where login_no = :vLoginNo
		and   a.popedom_code = b.popedom_code
		and   rela_type = '0'
		and   end_date > sysdate
		)
		minus
		(
		select login_no login_no, popedom_code popedom_code from sLoginPdomRelation
		where login_no = :vLoginNo
		and   rela_type = '1'
		and   end_date > sysdate
		); 
		
	if(SQLCODE != 0 || SQLROWS == 0){
#ifdef _DEBUG_
	/*printf("getWorkNoRightList: loginNo = [%s] sqlcode=[%d]\n", vLoginNo, SQLCODE);
	*/
	PUBLOG_DBDEBUG("");
	pubLog_DBErr(_FFL_,"","100011","getWorkNoRightList: loginNo = [%s] sqlcode=[%d]", vLoginNo, SQLCODE);
#endif 		
	  return 100011;
	}
	
	return 0;
}

char	*strchange ( int nfirst, char *from, char *to )
{

int	tolen = (to==NULL?0:strlen(to)),	nshift = abs(tolen-nfirst);	
	

/* -------------------------------------------------------------------------
shift from left or right to accommodate replacement of its nfirst chars by to
-------------------------------------------------------------------------- */
if ( tolen < nfirst )
  strcpy(from,from+nshift);
if ( tolen > nfirst )
  { char *pfrom = from+strlen(from);
    for ( ; pfrom>=from; pfrom-- )
      *(pfrom+nshift) = *pfrom; 
  }
  
/* -------------------------------------------------------------------------
from has exactly the right number of free leading chars, so just put to there
-------------------------------------------------------------------------- */
if ( tolen != 0 )			      /* make sure to not empty or null */
  memcpy(from,to,tolen);		/* chars moved into place */
  return ( from );			      /* changed string back to caller */
} 

int	strreplace ( char *string, char *from, char *to, int nreplace )
{
/* -------------------------------------------------------------------------
Allocations and Declarations
-------------------------------------------------------------------------- */
int	fromlen = (from==NULL?0:strlen(from)),   /* #chars to be replaced */
	tolen = (to==NULL?0:strlen(to));           /* #chars in replacement string */
char	*pfrom = (char *)NULL,		   /*ptr to 1st char of from in string*/
	*pstring = string;		           /*ptr past previously replaced from*/
char	*strchange ( int nfirst, char *from, char *to );   /* change 'from' to 'to' */
  int	nreps = 0;			             /* #replacements returned to caller*/
  
/* -------------------------------------------------------------------------
repace occurrences of 'from' in string to 'to'
-------------------------------------------------------------------------- */

if ( string == (char *)NULL		   /* no input string */
||   (fromlen<1 && nreplace<=0) )	/* replacing empty string forever */
  nreps = (-1);				 /* so signal error */
else					         /* args okay */
  while (nreplace<1 || nreps<nreplace)	/* up to #replacements requested */
    {
    if ( fromlen > 0 )			        /* have 'from' string */
      pfrom = strstr(pstring,from);	/*ptr to 1st char of from in string*/
    else  pfrom = pstring;		      /*or empty from at start of string*/
    if ( pfrom == (char *)NULL ) break;	/*no more from's, so back to caller*/
    if ( strchange(fromlen,pfrom,to)	  /* leading 'from' changed to 'to' */
    ==   (char *)NULL ) { nreps=(-1); break; } /* signal error to caller */
    nreps++;				         /* count another replacement */
    pstring = pfrom+tolen;		/* pick up search after 'to' */
    if ( *pstring == '\000' ) break;	/* but quit at end of string */
    } /* --- end-of-while() --- */
    
	return ( nreps );			/* #replacements back to caller */
}

int getGrpFeeList(FBFR32 *transIN, int index, tGrpUserMsg *grpUserMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char service_code[4+1];
		char price_code[4+1];
		char charge_flag[1+1];
		float price_fee = 0;
		char feeCode[2+1];
		char detailCode[2+1];
	EXEC SQL END DECLARE SECTION;
	
	int i = 0;
  int iFeeCount = 0;
  float vyShouldFee = 0;   /* ӪҵӦ�� */
  float vyRealFee = 0;     /* Ӫҵʵ�� */
  float vzShouldFee = 0;   /* ����Ӧ�� */
  float vzRealFee = 0;     /* ����ʵ�� */
  
  iFeeCount = Foccur32(transIN, GPARM32_0+index); 
#ifdef _DEBUG_

	pubLog_Debug(_FFL_,"","","iFeeCount = [%d]", iFeeCount);
#endif  
  for(i = 0; i < iFeeCount; i++){
  	Fget32(transIN, GPARM32_0+index, i, grpUserMsg->feeList.service_code[i], NULL);

  	if(strlen(grpUserMsg->feeList.service_code[i]) == 0 && i == 0) {
  		iFeeCount = 0;
  		break;
  	}
  		
  	if(strlen(grpUserMsg->feeList.service_code[i]) == 0) continue;

		memset(service_code, 0, sizeof(service_code));
		memset(price_code, 0, sizeof(price_code));
		memset(charge_flag, 0, sizeof(charge_flag));
		memset(feeCode, 0, sizeof(feeCode));
		memset(detailCode, 0, sizeof(detailCode));
		price_fee = 0;
			
  	Fget32(transIN, GPARM32_0+index+1, i, grpUserMsg->feeList.price_code[i], NULL);
  	Fget32(transIN, GPARM32_0+index+2, i, grpUserMsg->feeList.action_fee[i], NULL);
  	Fget32(transIN, GPARM32_0+index+3, i, charge_flag, NULL);

#ifdef _DEBUG_

	pubLog_Debug(_FFL_,"","","[%s][%s][%s]",grpUserMsg->feeList.service_code[i], 
		grpUserMsg->feeList.price_code[i], grpUserMsg->feeList.action_fee[i]);	
#endif
		
		strcpy(price_code, grpUserMsg->feeList.price_code[i]);
		strcpy(service_code, grpUserMsg->feeList.service_code[i]);
		
		EXEC SQL select b.price_rate*b.price_fee, nvl(b.fee_code, '00'), nvl(b.detail_code, '00')
			into :price_fee, :feeCode, :detailCode
	  from sSrvPrice a, spriceCode b, sSrvCode c
		where  a.price_code = b.price_code 
			and a.service_code = c.service_code
			and a.service_code = :service_code
			and b.price_code = :price_code
			and c.service_type = '5';    /* service_type = 5 ��ʾһ���Է��� */

		if(SQLCODE != 0){
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
			return -1002;
		}
		
		strcpy(grpUserMsg->feeList.charge_flag[i], charge_flag);
		sprintf(grpUserMsg->feeList.price_fee[i], "%.2f", price_fee);
		strcpy(grpUserMsg->feeList.fee_code[i], feeCode);
		strcpy(grpUserMsg->feeList.detail_code[i], detailCode);
		
		if(charge_flag[0] == '1'){ /* ǰ̨��ȡ */
			vyShouldFee = vyShouldFee + price_fee;
			vyRealFee = vyRealFee + atof(grpUserMsg->feeList.action_fee[i]);
		}else { /* ������ȡ */
			vzShouldFee = vzShouldFee + price_fee;
			vzRealFee = vzRealFee + atof(grpUserMsg->feeList.action_fee[i]);
		}		
  }

#ifdef _DEBUG_
	/*printf("feeList = [%.2f] [%.2f] [%.2f] [%.2f]\n", vyShouldFee, vyRealFee, vzShouldFee, vzRealFee);
	*/
	pubLog_Debug(_FFL_,"","","feeList = [%.2f] [%.2f] [%.2f] [%.2f]", vyShouldFee, vyRealFee, vzShouldFee, vzRealFee);
#endif

  sprintf(grpUserMsg->feeList.vyShouldFee, "%.2f", vyShouldFee);
  sprintf(grpUserMsg->feeList.vyRealFee, "%.2f", vyRealFee);
  sprintf(grpUserMsg->feeList.vzShouldFee, "%.2f", vzShouldFee);
  sprintf(grpUserMsg->feeList.vzRealFee, "%.2f", vzRealFee);
  sprintf(grpUserMsg->feeList.iFeeCount, "%d", iFeeCount);
  
	return 0;
}

/*ng_����_ҵ�񹤵� add by mengfy@09/28/2009*/
/*int pubInsGrpOutSideFee(char *grpIdNo, tGrpUserMsg *grpUserMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char  vGrpIdNo[14+1];
		long  contractNo = 0;
		char  userNo[15+1];
		float real_fee = 0;
		char  feeCode[2+1];
		char  detailCode[2+1];
		char  billYM[6+1];
		int   i = 0;
	EXEC SQL END DECLARE SECTION;
	
#ifdef _DEBUG_
	printf("[pubInsGrpOutSideFee begin...[%d].]\n",atoi(grpUserMsg->feeList.iFeeCount));
	
	pubLog_Debug(_FFL_,"","","[pubInsGrpOutSideFee begin...[%d].]",atoi(grpUserMsg->feeList.iFeeCount));
#endif

	memset(vGrpIdNo, 0, sizeof(vGrpIdNo));
	memset(userNo, 0, sizeof(userNo));
	memset(billYM, 0, sizeof(billYM));
	
	strcpy(vGrpIdNo, grpIdNo);
	
	EXEC SQL select account_id, user_no, to_char(run_time,'yyyymm')
	into   :contractNo, :userNo, :billYM
	from dGrpUserMsg 
	where id_no = to_number(:vGrpIdNo);
	if(SQLCODE != 0){
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
			return -1003;
	}
	
	for(i=0; i< atoi(grpUserMsg->feeList.iFeeCount); i++){
		 printf("grpUserMsg->feeList.charge_flag[%d] == [%s]\n", i, grpUserMsg->feeList.charge_flag[i]);
		 if(grpUserMsg->feeList.charge_flag[i][0] == '2'){
		 			memset(feeCode, 0, sizeof(feeCode));
					memset(detailCode, 0, sizeof(detailCode));
					strcpy(feeCode, grpUserMsg->feeList.fee_code[i]);
					strcpy(detailCode, grpUserMsg->feeList.detail_code[i]);
					real_fee = atof(grpUserMsg->feeList.action_fee[i]);
					
					EXEC SQL insert into wGrpOutSideFee 
					(CONTRACT_NO, USER_NO, FEE_CODE, DETAIL_CODE, SHOULD_PAY, BILL_YM)
					values(
					:contractNo, :userNo, :feeCode, :detailCode, :real_fee, :billYM);
			    if(SQLCODE != 0){
			    	PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","","ȡ�û���Ϣ����");
			    	return -2001;
			    }
		 }
	}
#ifdef _DEBUG_
	printf("[pubInsGrpOutSideFee end....]\n");
	
	pubLog_Debug(_FFL_,"","","[pubInsGrpOutSideFee end....]");
#endif	
	return 0;
}*/
/*ng_����_ҵ�񹤵� add by mengfy@09/28/2009*/
/**
 *     �õ������û��ⲿ���롣
 * @author      lugz
 * @version %I%, %G%
 * @since 1.00
 * @inparam     regionCode      ���д���
 * @inparam     smCode  ҵ����� ��ʱ���ã�����
 * @outparam grpUserNo  �����û�����
 * @return ����0��ʾ��ȷ���������
 */
 
int GetInnerUserNo(const char *regionCode, const char *smCode, char* grpUserNo, char *retMsg)
{
        char            tempBuf[100];

        short in;
        char dynStmt[1024];

        memset(dynStmt, 0, sizeof(dynStmt));
#if PROVINCE_RUN==PROVINCE_JILIN
	/*printf("+GetInnerUserNo================1\n");
	*/
	pubLog_Debug(_FFL_,"","","+GetInnerUserNo================1");
        sprintf(dynStmt,        "SELECT SMAXLINENO%s.nextval"
                                                "  FROM dual",
                                                regionCode);
#else
	/*printf("+GetInnerUserNo================1\n");
	*/
	pubLog_Debug(_FFL_,"","","+GetInnerUserNo================1");
        sprintf(dynStmt,        "SELECT sGrpUserNo%s.nextval"
                                                "  FROM dual",
                                                regionCode);
#endif

        memset(tempBuf, 0, sizeof(tempBuf));
        EXEC SQL EXECUTE
        BEGIN
                EXECUTE IMMEDIATE :dynStmt INTO :tempBuf;
        END;
        END-EXEC;
        Trim(tempBuf);
        if (SQLCODE !=0 || tempBuf[0] == '\0')
        {
#ifdef _DEBUG_
                /*printf("dynStmt=[%s]\n", dynStmt);
                */
               PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1001","dynStmt=[%s]", dynStmt);
#endif
                strcpy(retMsg, "�õ��ڲ�����ֵ����");
                return 1001;
        }
        strcpy(grpUserNo, tempBuf);
        return 0;
}

int GetInnerUserNoNormal(const char *regionCode, const char *smCode, char* grpUserNo, char *retMsg)
{
        char            tempBuf[100];

        short in;
        char dynStmt[1024];

        memset(dynStmt, 0, sizeof(dynStmt));
		
		/*printf("+GetInnerUserNo================1\n");
		*/
		pubLog_Debug(_FFL_,"","","+GetInnerUserNo================1");
        sprintf(dynStmt,        "SELECT sGrpUserNo%s.nextval"
                                                "  FROM dual",
                                                regionCode);

        memset(tempBuf, 0, sizeof(tempBuf));
        EXEC SQL EXECUTE
        BEGIN
                EXECUTE IMMEDIATE :dynStmt INTO :tempBuf;
        END;
        END-EXEC;
        Trim(tempBuf);
        if (SQLCODE !=0 || tempBuf[0] == '\0')
        {
#ifdef _DEBUG_
               /* printf("dynStmt=[%s]\n", dynStmt);
               */
                PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","1001","dynStmt=[%s]", dynStmt);
#endif
                strcpy(retMsg, "�õ��ڲ�����ֵ����");
                return 1001;
        }
        strcpy(grpUserNo, tempBuf);
        return 0;

}

/* ���Ӻ�������¼ */
int PublicRecordBlackList(DCUSTFAZE_TYPE *dCustFaze,long login_accept,char *login_no,char *op_code,char *op_time,char *op_note)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sPhoneNo[15+1];
	char sFazeType[1+1];
	char sDisposeway[2+1];
	char sBelongType[1+1];                       
	char sGradeCode[1+1];                       
	char sBeginTime[17+1];
	char sLoginNo[6+1];
	char sOpCode[4+1];
	char sOpTime[17+1];
	char sOpNote[128+1];
	long lLoginAccept = 0;                       
	EXEC SQL END DECLARE SECTION;
	
	init(sPhoneNo);
	init(sFazeType);
	init(sDisposeway);
	init(sBelongType);
	init(sGradeCode);
	init(sBeginTime);
	
	init(sLoginNo);
	init(sOpCode);
	init(sOpTime);
	init(sOpNote);
	
	strcpy(sPhoneNo,dCustFaze->phone_no);    
	strcpy(sFazeType,dCustFaze->faze_type);   
	strcpy(sDisposeway,dCustFaze->disposeway); 
	strcpy(sBelongType,dCustFaze->belong_type); 
	strcpy(sGradeCode,dCustFaze->grade_code);  
	strcpy(sBeginTime,dCustFaze->begin_time);  	
	strcpy(sLoginNo,login_no); 
	strcpy(sOpCode,op_code); 
	strcpy(sOpTime,op_time);  
	strcpy(sOpNote,op_note); 
	lLoginAccept = login_accept;
	
	/*�����������*/
	EXEC SQL INSERT INTO dCustFaze(phone_no,faze_type,disposeway,belong_type,grade_code,begin_time,CREATE_RESION,LOGIN_NO,OP_TIME )
	         VALUES(:sPhoneNo, :sFazeType, :sDisposeway,:sBelongType,
	         	    :sGradeCode, to_date(:sBeginTime,'YYYYMMDD HH24:MI:SS'),:sOpNote,:sLoginNo,to_date(:sOpTime,'YYYYMMDD HH24:MI:SS'));
	if(SQLCODE!=0)
	{
		/*
		printf("��������������,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��������������,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	
	/*���������¼��*/
	EXEC SQL INSERT INTO dCustFazeHis(login_accept,phone_no,faze_type,disposeway,
									belong_type,grade_code,begin_time,login_no,op_code,op_time,op_note)
	         VALUES(:lLoginAccept,:sPhoneNo,:sFazeType, :sDisposeway,:sBelongType,
	         	    	:sGradeCode, to_date(:sBeginTime,'YYYYMMDD HH24:MI:SS'),
	         	    	:sLoginNo,:sOpCode,:sOpTime,:sOpNote);
	         	    		
	if(SQLCODE!=0)
	{
		/*printf("�����������ʷ�����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-2","�����������ʷ�����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -2;
	}
	
	return 0;
}

/* ɾ����������¼ */
int PublicDelBlackList(long login_accept,char *phone_no, char *login_no,char *op_code,
								 char *op_time,char *op_note)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sPhoneNo[15+1];                                          
	char sDisposedNum[21+1];                      
	char sLoginNo[6+1];                      
	char sOpCode[4+1];                      
	char sOpTime[17+1];                      
	char sOpNote[128+1];
	long lLoginAccept = 0;	                      
	EXEC SQL END DECLARE SECTION;                      
	
	init(sPhoneNo);      
 
	init(sDisposedNum);  
	init(sLoginNo);       
	init(sOpCode);        
	init(sOpTime);       
	init(sOpNote);	     
	
	strcpy(sPhoneNo,phone_no); 
	strcpy(sLoginNo,login_no); 
	strcpy(sOpCode,op_code); 
	strcpy(sOpTime,op_time);  
	strcpy(sOpNote,op_note); 
	lLoginAccept = login_accept;
	
	/*�����������ʷ��*/
	EXEC SQL INSERT INTO dCustFazeHis(login_accept,phone_no,faze_type,disposeway,
									belong_type,grade_code,begin_time,login_no,op_code,op_time,op_note)
	         SELECT :lLoginAccept, phone_no, faze_type, disposeway,
	         	    belong_type, grade_code, begin_time,
	         	    :sLoginNo, :sOpCode, :sOpTime, :sOpNote
	           FROM dCustFaze
	          WHERE phone_no=:sPhoneNo;
	if(SQLCODE!=0)
	{
		/*printf("�����������ʷ�����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","�����������ʷ�����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	
	/*ɾ����������¼*/
	EXEC SQL delete FROM dCustFaze
	               WHERE phone_no=:sPhoneNo;
	if(SQLCODE!=0)
	{
		/*printf("ɾ����������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-2","ɾ����������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -2;
	}
	
	return 0;
}

/*��ѯ�����Ƿ��ں�����(�������)
����ֵ��
   0������
   1����
*/
int PublicChkBlackList(char *phone_no,char *faze_type)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int  iCount = 0;
	char sPhoneNo[15+1];
	char sFazeType[1+1];                      
	EXEC SQL END DECLARE SECTION; 
	
	init(sPhoneNo);
	init(sFazeType);
	
	strcpy(sPhoneNo,phone_no);
	strcpy(sFazeType,faze_type);
	
	Trim(sPhoneNo);
	Trim(sFazeType);
	
	pubLog_DBErr(_FFL_,"","","��ѯ����[%s][%s]",phone_no,faze_type);
	
	if(strlen(sFazeType) == 1 && strcmp(sFazeType,"x") != 0)
	{
		EXEC SQL SELECT count(*) INTO :iCount
	           FROM dCustFaze
	          WHERE phone_no=:sPhoneNo and faze_type in (:sFazeType,'0');
	}
	else
	{
		EXEC SQL SELECT count(*) INTO :iCount
	           FROM dCustFaze
	          WHERE phone_no=:sPhoneNo;
	}
	
	if((SQLCODE!=0)&&(SQLCODE!=1403))
	{
		/*printf("��ѯ��������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ��������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	
	if(iCount==0)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

/*�޸����ƺ�������״̬*/
int PublicUpdateRubBish(char *phone_no, char *status1, char * login_accept1)
{
	EXEC SQL BEGIN DECLARE SECTION;
	int  iCount = 0;
	char sPhoneNo[15+1]; 
	char status[1+1];  
	char sPhoneAccept[14+1];                   
	EXEC SQL END DECLARE SECTION; 
	
	init(sPhoneNo);
	init(status);
	init(sPhoneAccept);
	
	strcpy(sPhoneNo,phone_no);
	strcpy(status,status1);
	strcpy(sPhoneAccept,login_accept1);
	
	EXEC SQL update dRubBishMsg set status=:status 
		      WHERE msisdn=:sPhoneNo and login_accept = to_number(:sPhoneAccept);
	
	if((SQLCODE!=0)&&(SQLCODE!=1403))
	{
		/*printf("�������ƺ�����״̬����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","�������ƺ�������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
	
		return -1;
	}

	return 0;
	
}

/*ȡɧ������*/
int PublicGetFazeName(char *faze_type, char *faze_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sFazeType[1+1];
	char sFazeName[128+1];                      
	EXEC SQL END DECLARE SECTION; 
	
	init(sFazeType);
	init(sFazeName);
	
	strcpy(sFazeType,faze_type);
	
	EXEC SQL select faze_name 
						into :sFazeName 
					from sfazetype 
					WHERE faze_type=:sFazeType;
	
	if(SQLCODE != 0)
	{
		/*printf("��ѯ��������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ��������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	Trim(sFazeName);

	strcpy(faze_name,sFazeName);
	
	return 0;
}

/*ȡ��������*/
int PublicGetBelongName(char *belong_type, char *belong_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sBelongType[1+1];
	char sBelongName[128+1];                      
	EXEC SQL END DECLARE SECTION; 
	
	init(sBelongType);
	init(sBelongName);
	
	strcpy(sBelongType,belong_type);
	
	EXEC SQL select belong_name 
						into :sBelongName 
					from sbelongtype 
					WHERE belong_type=:sBelongType;
	
	if(SQLCODE != 0)
	{
		/*printf("��ѯ��������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ��������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	Trim(sBelongName);

	strcpy(belong_name,sBelongName);
	
	return 0;
}

/*ȡ���ͷ�ʽ����*/
int PublicGetDisposeName(char *disposeway, char *dispose_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sDisposeWay[2+1];
	char sDisposeName[128+1];                      
	EXEC SQL END DECLARE SECTION; 
	
	init(sDisposeWay);
	init(sDisposeName);
	
	strcpy(sDisposeWay,disposeway);
	
	EXEC SQL select dispose_name 
						into :sDisposeName 
					from sDisposeWay 
					WHERE disposeway=:sDisposeWay;
	
	if(SQLCODE != 0)
	{
		/*printf("��ѯ��������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ��������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	Trim(sDisposeName);

	strcpy(dispose_name,sDisposeName);
	
	return 0;
}

/*ȡ�û���������*/
int PublicGetGradeName(char *grade_code, char *grade_name)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char sGradeCode[1+1];
	char sGradeName[128+1];                      
	EXEC SQL END DECLARE SECTION; 
	
	init(sGradeCode);
	init(sGradeName);
	
	strcpy(sGradeCode,grade_code);
	
	EXEC SQL select grade_name 
						into :sGradeName 
					from sGradeCode 
					WHERE grade_code=:sGradeCode;
	
	if(SQLCODE != 0)
	{
		/*printf("��ѯ��������¼����,SQLCODE=[%d][%s]\n",SQLCODE,SQLERRMSG);
		*/
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","��ѯ��������¼����,SQLCODE=[%d][%s]",SQLCODE,SQLERRMSG);
		return -1;
	}
	Trim(sGradeName);

	strcpy(grade_name,sGradeName);
	
	return 0;
}


/************************************************************************
��������:fGetGrpunbillowe
����ʱ��:2008/01/03
������Ա:zwm
��������:�����ʻ�IDȡ�ʻ���Ƿ��,Ԥ���
�������:�ʺ�
�������:Ƿ��
Ԥ���
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fGetGrpunbillowe(const char *contract_no,double *owe_fee,double *prepay_fee)
{
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 begin --*/
	/*EXEC SQL BEGIN DECLARE SECTION;	
	char	vcontract_no[23];
	char	sqlStr[512];
	double	vowe_fee=0;
	double	vprepay_fee=0;
	double	v_unbill=0.00;
	long	grpcontractNo=0;
	long	idNo=0;
	int		vret=0;
	char	phoneNo[15+1];
	EXEC SQL END DECLARE SECTION;

	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(sqlStr,0,sizeof(sqlStr));
	memset(phoneNo,0,sizeof(phoneNo));
	
	Trim(vcontract_no);
	if(contract_no!=NULL)
	{
		strcpy(vcontract_no,contract_no);
	}
	
	grpcontractNo=atol(contract_no);
	
	vret=0;
	vret=Query_Con_Pre(grpcontractNo,"*",&vprepay_fee);
	if(vret != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","ȡ���ݿ�Ƿ�ѳ���");
		return -1;
	}
	vret=0;
	vret=Query_Con_Owe(grpcontractNo,&vowe_fee);
	if(vret != 0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","ȡ���ݿ�Ƿ�ѳ���");
		return -1;
	}
	sprintf(sqlStr,"select a.id_no,b.phone_no from dConUserMsg a,dcustmsg b\
					where a.contract_no=:v1 \
				and a.serial_no=0 and a.id_no=b.id_no ");
	EXEC SQL PREPARE stmt_sGetUserOwe2 FROM :sqlStr;
	EXEC SQL DECLARE getgrpunbill_cur CURSOR FOR stmt_sGetUserOwe2;
	EXEC SQL OPEN getgrpunbill_cur using :grpcontractNo;
	for(;;)
	{
		v_unbill=0.00;
		idNo=0;
		memset(phoneNo,0,sizeof(phoneNo));
		EXEC SQL FETCH getgrpunbill_cur INTO :idNo,:phoneNo;			
		if(SQLCODE == NOTFOUND) break;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-2","select dConUserMsg SQLCODE=[%d]",SQLCODE);
			EXEC SQL CLOSE getgrpunbill_cur;
			return -2;
		}
		vret=PublicGetUnbillChg(grpcontractNo,idNo,0,phoneNo,4,&v_unbill);
		if(vret!=0)
		{
			pubLog_DBErr(_FFL_,"","","ȡδ���ʻ��ѳ���phoneNo=[%s]",phoneNo);
			continue;
		} 	    	
		vowe_fee =vowe_fee +v_unbill ;
	}
	EXEC SQL CLOSE getgrpunbill_cur;*/
	
	/*-- ���������ʱ���� edit by huangtao at 25/08/2009 --*/
	EXEC SQL BEGIN DECLARE SECTION;
		FBFR32    *sendbuf, *rcvbuf;
	    FLDLEN32  sendlen,rcvlen;	
	    int 	  sendNum=0;
	    int 	  recvNum=0;
	    int	      v_ret = 0;
	    char 	  buffer1[256];
	    char	  buffer2[256];
	    char	  vPrePayFee[14 + 1];
	    char	  vOweFee[14 + 1];
	    long 	  reqlen = 0;
	    char	  vcontract_no[23];
	    double	  vowe_fee=0;
		double	  vprepay_fee=0;
	EXEC SQL END DECLARE SECTION;
	
	init(vcontract_no);
	strcpy(vcontract_no, contract_no);
	
	pubLog_Debug(_FFL_, "", "", "����BOSS����bs_qryLastOwe��ʼ!");
	/*�����������*/
	sendNum = 1;
			
	/*�����������*/
	recvNum = 4;
	
	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		pubLog_DBErr(_FFL_, "", "","��ʼ�����������ʧ��!");
		return -1;
	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"4",(FLDLEN32)0);							
			
	Fchg32(sendbuf,GPARM32_0,0,vcontract_no,(FLDLEN32)0);
			   
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);
			
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		pubLog_DBErr(_FFL_, "", "","��ʼ�����������ʧ��!");
		return -1;
	}

	reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	v_ret = 0;
	v_ret = fPubCallSrv(CALLSERV_TP,"bs_qryLastOwe",sendbuf,&rcvbuf);
	if(v_ret != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		PUBLOG_DBDEBUG("");
		pubLog_Debug(_FFL_, "", "", "v_ret=[%d]!",v_ret);
		pubLog_DBErr(_FFL_, "", "","����BOSS����bs_qryLastOwe����!");
		return -777;
	}
	
	memset(buffer1, '\0', sizeof(buffer1));
	memset(buffer2, '\0', sizeof(buffer2));

	Fget32(rcvbuf,GPARM32_0,0,buffer1,(FLDLEN32)0);
    	Fget32(rcvbuf,GPARM32_1,0,buffer2,(FLDLEN32)0);
  	
  	Trim(buffer1);
	Trim(buffer2);					
	printf("buffer1[%s]\n",buffer1);
	printf("buffer2[%s]\n",buffer2);
	
	if(strcmp(buffer1,"000000")!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"",buffer1,buffer2);
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		return -1;
	}
	
	Fget32(rcvbuf,GPARM32_2,0,vPrePayFee,(FLDLEN32)0);
    Fget32(rcvbuf,GPARM32_3,0,vOweFee,(FLDLEN32)0);
    
    Trim(vPrePayFee);
    Trim(vOweFee);
    
	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	vprepay_fee = atof(vPrePayFee);
	vowe_fee = atof(vOweFee);
	
	printf("vprepay_fee=[%lf]\n",vprepay_fee);
	printf("vowe_fee=[%lf]\n",vowe_fee);
	
	/*-- ����֮������ø��� edit by huangtao at 25/08/2009 end --*/
	*owe_fee = vowe_fee;
	*prepay_fee = vprepay_fee;
	
	return 0;
}

/****
  ϵͳ���ͻ���ʱ���Ͷ���֪ͨ����
  ��������:SystemSnedPreMsg
  �������:
  		�û�����
 		  ������ˮ
  		֪ͨ��������
  		��������
  		����
  �������:
  		0 --����
  		��0 --����
  ****/
  int SystemSnedPreMsg(char *phone_no,long login_accept,char *send_msg,char *op_code,char *login_no)
  {
  	EXEC SQL BEGIN DECLARE SECTION;
  	
  	char	phoneNo[15+1];
  	long	loginAccept=0;
  	char	sendMsg[255+1];
  	char	opCode[4+1];
  	char	loginNo[6+1];
  	char	sqlStr[1024];
  	
  	EXEC SQL END DECLARE SECTION;
  	
  	memset(phoneNo,0,sizeof(phoneNo));
  	memset(sendMsg,0,sizeof(sendMsg));
  	memset(opCode,0,sizeof(opCode));
  	memset(loginNo,0,sizeof(loginNo));
  	memset(sqlStr,0,sizeof(sqlStr));
  	
  	strcpy(phoneNo,phone_no);
  	loginAccept=login_accept;
  	strcpy(sendMsg,send_msg);
  	strcpy(opCode,op_code);
  	strcpy(loginNo,login_no);
  	
  	init(sqlStr);
  	sprintf(sqlStr," insert into wcommonshortmsgts (COMMAND_ID,LOGIN_ACCEPT,MSG_LEVEL,PHONE_NO,MSG,ORDER_CODE, "
  				   " BACK_CODE,DX_OP_CODE,LOGIN_NO,OP_TIME ) "
  				   " values(SMSG_SEQUENCE.nextval,:v1,100,:v2,:v3,0,0,:v4,:v5,sysdate)");
  	EXEC SQL PREPARE sql_stmt FROM :sqlStr;
  	EXEC SQL EXECUTE sql_stmt using :loginAccept,:phoneNo,:sendMsg,:opCode,:loginNo;
  	if(SQLCODE != 0)
  	{
  		/*printf("insert wcommonshortmsgts error[%d][%s]\n",SQLCODE,phoneNo);
  		*/
  		pubLog_DBErr(_FFL_,"","-1","insert wcommonshortmsgts error[%d][%s]",SQLCODE,phoneNo);
  		return -1;
  	}
  	
  	return 0;	
  }

/*****
�������ƣ�sNewfuncList
�����������ϲ��ɵĿ��ػ�ָ��Ϊ�µĿ��ػ�ָ��
���������
		funcList	�ɵ��ط����ܴ��봮
		voffenFunc	�ɵĿ��ػ����봮
		vhlrCode	
���������
		newFuncList	�ϲ�����µ�������봮
		oldFuncList	���ܺϲ��ľɵ��ط����봮
		newFuncStr	���Ժϲ��ľɵ��ط����봮
*****/

int sNewfuncList(char *vfuncList,char *voffenFunc,char *vnewFuncList,char *voldFuncList,char *vnewFuncStr,char *vhlrCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	funcList[128+1];
	char	funcList1[128+1];
	char	newfuncList[128+1];
	char	newfuncStr[128+1];
	char	funclistTemp[128+1];
	char	funclistTemp1[128+1];
	int		i=0,j=0,k=0,x=0,y=0;
	int		funcNum=0,matchFlag=0,icount=0;
	char	funcArr[32][2+1];
	char	funcArr1[32][2+1];
	char	funcArr2[32][2+1];
	char	funcArr3[32][10+1];
	char	*p,*q,*rest,*opph;
	char	sqlStr[256+1];
	char	newFunc[2+1];
	char	offenFunclist[32+1];
	char	tempstr[2+1];
	char	newfuncOpen[2+1];
	
	EXEC SQL END DECLARE SECTION;
	
	init(funcList);
	init(funcList1);
	init(newfuncList);
	init(newfuncStr);
	init(sqlStr);
	init(funclistTemp);
	init(funclistTemp1);
	init(newFunc);
	init(offenFunclist);
	init(tempstr);
	init(newfuncOpen);
	
	for(i=0;i<32;i++)
	{
		init(funcArr[i]);
		init(funcArr1[i]);
		init(funcArr2[i]);
		init(funcArr3[i]);
	}
	
	strcpy(funcList,vfuncList);
	strcpy(offenFunclist,voffenFunc);
	
	Trim(funcList);
	sprintf(funcList,"%s%s",funcList,offenFunclist);
	
	/*printf("sNewfuncList BEGIN\n");
	*/
	pubLog_Debug(_FFL_,"","","sNewfuncList BEGIN");
	
	strcpy(funcList1,funcList);
	p=strtok(funcList1,"|");
	while(p != NULL)
	{
		strcpy(funcArr3[icount++],p);
		p=strtok(NULL,"|");
	}
	
	init(funcList);
	for(y=0;y<icount;y++)
	{
		init(tempstr);
		strncpy(tempstr,funcArr3[y],2);
		sprintf(funcList,"%s%s|",funcList,tempstr);
	}
	
	Trim(funcList);
	/*printf("funcList=[%s]\n",funcList);
	*/
	pubLog_Debug(_FFL_,"","","funcList=[%s]",funcList);
	funcNum=strlen(funcList)/3;
	/*printf("funcNum=[%d]\n",funcNum);
	*/
	pubLog_Debug(_FFL_,"","","funcNum=[%d]",funcNum);
	
	init(sqlStr);
	sprintf(sqlStr," select new_function,old_funclist from snewfunction where hlr_code=:v1 "
				   " and function_num <=:v2 order by function_num desc");
	EXEC SQL PREPARE SqlStmt FROM :sqlStr;
	EXEC SQL DECLARE my_cur CURSOR FOR SqlStmt;
	EXEC SQL OPEN my_cur using :vhlrCode,:funcNum;
	for(;;)
	{
		init(newFunc);
		init(funclistTemp);
		init(funclistTemp1);
		init(funcList1);
		k=0;
		i=0;
		matchFlag=0;
		EXEC SQL FETCH my_cur INTO :newFunc,:funclistTemp;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			/*printf("select snewfunction error[%d]\n",SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","select snewfunction error[%d]",SQLCODE);
			EXEC SQL CLOSE my_cur;
			return -1;
		}
		if(SQLCODE == 1403)
		{
			/*printf("select snewfunction no more data\n");*/
			
			break;
		}
		
		strcpy(funclistTemp1,funclistTemp);
		strcpy(funcList1,funcList);
		Trim(funclistTemp);
		Trim(funclistTemp1);
		Trim(funcList);
		Trim(funcList1);
		
		if(strlen(funcList) < 4)
		{
			break;
		}
		
		for(x=0;x<32;x++)
		{
			init(funcArr[x]);
			init(funcArr1[x]);
		}
		
		/*������������봮*/
		p=strtok(funcList1,"|");
		while(p != NULL)
		{
			strcpy(funcArr[i++],p);
			p=strtok(NULL,"|");
		}
		
		/*������������봮*/
		q=strtok(funclistTemp1,"|");
		while(q != NULL)
		{
			strcpy(funcArr1[k++],q);
			q=strtok(NULL,"|");
		}
		
		/*��֤�µĴ��봮�����ھɴ����Ƿ�ȫƥ��*/
		for(j=0;j<k;j++)
		{
			rest=strstr(funcList,funcArr1[j]);
			if(rest == NULL)
			{
				matchFlag=1;
				break;
			}
		}
		
		/*�ɴ��봮�����Ӵ����Ժϳ��µ�����*/
		if(matchFlag == 0)
		{
			sprintf(newfuncList,"%s%s|",newfuncList,newFunc);
			sprintf(newfuncStr,"%s%s",newfuncStr,funclistTemp);
			
			init(funcList);
			for(j=0;j<i;j++)
			{
				rest=strstr(funclistTemp,funcArr[j]);
				if(rest == NULL)
				{
					sprintf(funcList,"%s%s|",funcList,funcArr[j]);
				}
			}
			
			/*�Ѻ��п�������64���������ǰ��*/
			opph=strstr(funclistTemp,"64");
			if(opph != NULL)
			{
				strcpy(newfuncOpen,newFunc);
			}
			
			opph=strstr(funclistTemp,"74");
			if(opph != NULL)
			{
				strcpy(newfuncOpen,newFunc);
			}
		}
		
		/*printf("newfuncList=[%s]\n",newfuncList);
		printf("newfuncStr=[%s]\n",newfuncStr);
		printf("funcList=[%s]\n",funcList);*/
	}
	EXEC SQL CLOSE my_cur;
	
	/*�Ѻϲ���ʣ��ľɴ��봮�еĿ��ػ�����Ų���´��봮��*/
	k=0;
	q=strtok(funcList,"|");
	while(q != NULL)
	{
		strcpy(funcArr2[k++],q);
		q=strtok(NULL,"|");
	}
	
	init(funcList);
	for(y=0;y<k;y++)
	{
		rest=strstr(offenFunclist,funcArr2[y]);
		if(rest == NULL)
		{
			sprintf(funcList,"%s%s|",funcList,funcArr2[y]);
		}
		else
		{
			sprintf(newfuncList,"%s%s|",newfuncList,funcArr2[y]);
		}
	}
	
	/*�ѿ��Ժϲ��ľɴ����еĿ��ػ�����ȥ��*/
	for(i=0;i<32;i++)
	{
		init(funcArr2[i]);
	}
	
	k=0;
	q=strtok(newfuncStr,"|");
	while(q != NULL)
	{
		strcpy(funcArr2[k++],q);
		q=strtok(NULL,"|");
	}
	
	init(newfuncStr);
	for(y=0;y<k;y++)
	{
		rest=strstr(offenFunclist,funcArr2[y]);
		if(rest == NULL)
		{
			sprintf(newfuncStr,"%s%s|",newfuncStr,funcArr2[y]);
		}
	}
	
	/*�ѹػ�����66���ڵ�һλ*/
	for(i=0;i<32;i++)
	{
		init(funcArr2[i]);
	}
	
	k=0;
	q=strtok(newfuncList,"|");
	while(q != NULL)
	{
		strcpy(funcArr2[k++],q);
		q=strtok(NULL,"|");
	}
	
	init(newfuncList);
	for(y=0;y<k;y++)
	{
		if(strcmp(funcArr2[y],"66") == 0)
		{
			sprintf(newfuncList,"%s%s|",newfuncList,funcArr2[y]);
			init(funcArr2[y]);
		}
	}
	
	/*�ѿ�������64���ڵ�2λ*/
	for(y=0;y<k;y++)
	{
		if((strcmp(funcArr2[y],"64") == 0) || (strcmp(funcArr2[y],"74") == 0))
		{
			sprintf(newfuncList,"%s%s|",newfuncList,funcArr2[y]);
			init(funcArr2[y]);
		}
	}
	
	/*�Ѻ��п�������64���ڵ�3λ*/
	for(y=0;y<k;y++)
	{
		if((strcmp(funcArr2[y],newfuncOpen) == 0) && (strlen(newfuncOpen) > 0))
		{
			sprintf(newfuncList,"%s%s|",newfuncList,funcArr2[y]);
			init(funcArr2[y]);
		}
	}
	
	for(y=0;y<k;y++)
	{
		if(strlen(funcArr2[y]) > 0)
		{
			sprintf(newfuncList,"%s%s|",newfuncList,funcArr2[y]);
		}
	}
	
	/*���ط�ԤԼ������ʱ�丽�ӻ�ȥ*/
	for(i=0;i<32;i++)
	{
		init(funcArr1[i]);
		init(funcArr2[i]);
	}
	
	k=0;
	q=strtok(funcList,"|");
	while(q != NULL)
	{
		strcpy(funcArr1[k++],q);
		q=strtok(NULL,"|");
	}
	
	i=0;
	q=strtok(newfuncStr,"|");
	while(q != NULL)
	{
		strcpy(funcArr2[i++],q);
		q=strtok(NULL,"|");
	}
	
	init(funcList);
	init(newfuncStr);
	
	for(y=0;y<k;y++)
	{
		for(j=0;j<icount;j++)
		{
			if(strncmp(funcArr1[y],funcArr3[j],2) == 0)
			{
				sprintf(funcList,"%s%s|",funcList,funcArr3[j]);
				break;
			}
		}
	}
	
	for(y=0;y<i;y++)
	{
		for(j=0;j<icount;j++)
		{
			if(strncmp(funcArr2[y],funcArr3[j],2) == 0)
			{
				sprintf(newfuncStr,"%s%s|",newfuncStr,funcArr3[j]);
				break;
			}
		}
	}
	
	/*�������㣬������*/
	strcpy(vnewFuncList,newfuncList);
	strcpy(voldFuncList,funcList);
	strcpy(vnewFuncStr,newfuncStr);
	
	/*printf("---newfuncList=[%s]---\n",newfuncList);
	printf("---newfuncStr=[%s]---\n",newfuncStr);
	printf("---voldFuncList=[%s]---\n",voldFuncList);	
	printf("sNewfuncList END\n");
	*/
	pubLog_Debug(_FFL_,"","","---newfuncList=[%s]---",newfuncList);
	pubLog_Debug(_FFL_,"","","---newfuncStr=[%s]---",newfuncStr);
	pubLog_Debug(_FFL_,"","","---voldFuncList=[%s]---",voldFuncList);
	pubLog_Debug(_FFL_,"","","sNewfuncList END");
	
	return 0;
}

/*****
�������ƣ�soffencommand
����������ȡ���ػ����
���������
		regionCode	��������
		opCode		��������
		smCode		Ʒ��
		runCode		״̬
		phoneNo		�û�����
���������
		offenFunclist	�ϲ�����µ�������봮
*****/

int soffencommand(char *InIdNo,char *vregionCode,char *vopCode,char *vsmCode,char *vrunCode,char *voffenfuncList)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	char	offenfuncList[32+1];
	char	sqlStr[512+1];
	char	commandCode[2+1];
	char	smCode[2+1];
	char	opCode[4+1];
	char    vNetType[2+1];
	char	vInIdNo[14+1];
	int vCount=0;
	
	EXEC SQL END DECLARE SECTION;
	
	init(offenfuncList);
	init(sqlStr);
	init(commandCode);
	init(smCode);
	init(opCode);
	init(vNetType);
	
	strcpy(vInIdNo,InIdNo);
	
	if(strcmp(vsmCode,"z0") == 0)
	{
		strcpy(smCode,"cb");
	}
	else
	{
		strcpy(smCode,vsmCode);
	}
	
	if(strcmp(vopCode,"1108") == 0)
	{
		strcpy(opCode,"1106");
	}
	else if(strcmp(vopCode,"1118") == 0)
	{
		strcpy(opCode,"1116");
	}
	else
	{
		strcpy(opCode,vopCode);
	}
	vCount=0;
	EXEC SQL SELECT count(*) 
	       		INTO :vCount
				FROM susimrescode 
				WHERE res_code IN
				(
				 	  SELECT a.sim_type 
					  	FROM dSimRes a,dcustsim b
						WHERE b.id_no=to_number(:vInIdNo)
						AND a.sim_no=b.sim_no
				);
	if(SQLCODE!=0)
	{
			/*printf("select susimrescode error[%d]\n",SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","select susimrescode error[%d]",SQLCODE);
			return -1;
	}
	if(vCount>0)
	{
		strcpy(vNetType,"TD");
	}
	else
	{
		strcpy(vNetType,"2G");
	}
	
	init(sqlStr);
	sprintf(sqlStr," select b.command_code from cstatcode a,sstatcmdcode b where  a.region_code= :v1 "
				   " and a.op_code= :v2 and a.sm_code=:v3 and a.run_code= substr(:v4,1,1) "
				   " and a.new_run= substr(:v5,2,1) and a.cmd_code=b.cmd_code and a.net_type=:v6 ");
	EXEC SQL PREPARE SqlStmt1 FROM :sqlStr;
	EXEC SQL DECLARE offencommand_cur CURSOR FOR SqlStmt1;
	EXEC SQL OPEN offencommand_cur using :vregionCode,:opCode,:smCode,:vrunCode,:vrunCode,:vNetType;
	for(;;)
	{
		init(commandCode);
		EXEC SQL FETCH offencommand_cur INTO :commandCode;
		if(SQLCODE != 0 && SQLCODE != 1403)
		{
			/*printf("select command_code error[%d]\n",SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","-1","select command_code error[%d]",SQLCODE);
			EXEC SQL CLOSE offencommand_cur;
			return -1;
		}
		if(SQLCODE == 1403)
		{
			break;
		}
		
		sprintf(offenfuncList,"%s%s|",offenfuncList,commandCode);
	}
	EXEC SQL CLOSE offencommand_cur;
	
	/*printf("---offenfuncList=[%s]---\n",offenfuncList);
	*/
	pubLog_Debug(_FFL_,"","","---offenfuncList=[%s]---",offenfuncList);
	strcpy(voffenfuncList,offenfuncList);
	
	return 0;
}

/*****
�������ƣ�NewpubOpenAcctSendCmd
�����������û��������ػ�����
���������
		funcEnv		�û���Ϣ
		bStat		��������
		opCode		��������
		funcCodeList	״̬
���������

*****/

int NewpubOpenAcctSendCmd(tFuncEnv *funcEnv, char bStat, char *opCode, char *vfuncCodeList)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;

	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char	funcodeList[128+1];
	char	idNo[22 + 1];
	char	totalDate[8+1];
	char	opTime[17+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char    phoneNo1[15+1];
	char	orgCode[9+1];
	char	hlrCode[1+1];
	char	imsiNo[20+1];
	char	simNo[20+1];
	char	funcCode1[32][2+1];
	char	*p;
	char	businessStat[1+1];
	int		vcount=0;
	int		i=0,j=0;
	int ret =0;
	char vOrgId[10+1];
	char vGroupId[10+1];
	char	funcCode[2+1];
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	char vNewCmdStr[15 + 1];
	/*Modify at 2012.03.19 end*/
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(phoneNo1);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(orgCode);
	Sinitn(hlrCode);
	Sinitn(imsiNo);
	Sinitn(simNo);
	Sinitn(vOrgId);
	Sinitn(vGroupId);
	Sinitn(funcodeList);
	Sinitn(funcCode);
	/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
	Sinitn(vNewCmdStr);
	/*Modify at 2012.03.19 end*/	
	for(i=0;i<32;i++)
	{
		Sinitn(funcCode1[i]);
	}
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	strcpy(hlrCode, funcEnv->hlrCode);
	strcpy(imsiNo, funcEnv->imsiNo);
	strcpy(simNo, funcEnv->simNo);
	strcpy(funcodeList,vfuncCodeList);

	transIN = funcEnv->transIN;
	transOUT = funcEnv->transOUT;

	sprintf(businessStat, "%c",bStat);
	businessStat[1] = '\0';
	
	p=strtok(funcodeList,"|");
	while(p != NULL)
	{
		strcpy(funcCode1[j++],p);
		p=strtok(NULL,"|");
	}
	
	if(j == 0)
	{
		return 0;
	}
	
	/*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
         	  /*printf("��ȡ�û�org_idʧ��!\n");
         	  */
         	  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��!");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
      /*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetUserGroupid(phoneNo,vGroupId);
       if(ret <0)
         	{
         	  /*printf("��ȡ�û�group_idʧ��!\n");
         	  */
         	  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�group_idʧ��!");
         	  return 200919; 
         	}
       Trim(vGroupId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
	
	memset(dynStmt, '\0', sizeof(dynStmt));
	 /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/

	sprintf(dynStmt,"insert into wSndCmdDay "
		"("
		"COMMAND_ID, HLR_CODE, COMMAND_ORDER, ID_NO, BELONG_CODE,GROUP_ID,"
		"SM_CODE, PHONE_NO, NEW_PHONE, IMSI_NO, NEW_IMSI, OTHER_CHAR,"
		"OP_CODE, TOTAL_DATE, OP_TIME, LOGIN_NO, ORG_CODE,ORG_ID, LOGIN_ACCEPT,"
		"REQUEST_TIME, BUSINESS_STATUS, SEND_STATUS, SEND_TIME, COMMAND_CODE"
		")"
		" values "
		" (sOffOn.nextval, :v1, 0, to_number(:v2), :v3,:vGroupId,"
		" :v4, :v5, :v6, :v7, :v8, :v9,"
		" :v10, to_number(:v11), to_date(:v12, 'yyyymmdd hh24:mi:ss'), :v13, :v14,:vOrgId, to_number(:v15), "
		" to_date(:v16, 'yyyymmdd hh24:mi:ss'), :v17, '0', to_date(:v18, 'yyyymmdd hh24:mi:ss'),:v19) ");
	
	for(i=0;i<j;i++)
	{
		Sinitn(funcCode);
		strcpy(funcCode,funcCode1[i]);
		
		/*Modify for 87,02ָ��������� at 2012.04.17 begin*/
		Trim(funcCode);
		if(strcmp(funcCode,"02") == 0)
		{
			if(strcmp(businessStat,"1")==0)
			{
				strcpy(vNewCmdStr,"BOS1&BOS3");
			}
			else
			{
				strcpy(vNewCmdStr,"BOS3");
			}						
		}
		else if(strcmp(funcCode,"87") == 0)
		{
			if(strstr(funcodeList,"02"))
			{
				if(strcmp(businessStat,"1")==0)
				{
					strcpy(vNewCmdStr,"BOS1&BOS3");
				}
				else
				{
					strcpy(vNewCmdStr,"BOS1");
				}
				
			}
			else
			{
				if(strcmp(businessStat,"1")==0)
				{
					strcpy(vNewCmdStr,"BOS3");
				}
				else
				{
					strcpy(vNewCmdStr,"N");
				}
			}						
		}
		else
		{
			strcpy(vNewCmdStr,imsiNo);
		}
		Trim(vNewCmdStr);
		/*Modify for at 2012.04.17 end*/

		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :hlrCode, :idNo, :belongCode,:vGroupId,
				:smCode, :phoneNo, :phoneNo, :imsiNo,:vNewCmdStr, :simNo,
				:opCode, :totalDate, :opTime,:loginNo,:orgCode,:vOrgId,:loginAccept,
				:opTime, :businessStat,:opTime,:funcCode;
		end;
		end-exec;
		if(SQLCODE != 0)
		{
			/*printf("idNo=[%s], USERERROR[%d] [%d],[%s]",idNo, 121999, SQLCODE,SQLERRMSG);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121999","idNo=[%s], USERERROR[%d] [%d],[%s]",idNo, 121999, SQLCODE,SQLERRMSG);
			return 121999;
		}
	}
	
	
	return 0;
}

/*****
�������ƣ�NewfAddUsedFuncs
�����������û�������Ч�ط�����
���������
		funcEnv		�û���Ϣ
		bStat		��������
		opCode		��������
		funcCodeList	״̬
���������

*****/
int NewfAddUsedFuncs(tFuncEnv *funcEnv, char *funcs,double iRemainFee,double *olimitFee,char *funcode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char end_date[8+1];
	int  hasExpire = 0; /*�Ƿ��е���ʱ��*/
	int  iRetCode = 0;
	int  num=0;
	int ret =0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];
	char function_code[2+1];

	char	idNo[22 + 1];
	char	totalDate[8+1];
	char vOrgId[10+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	double  vLimitPay=0;
	double  vRemainFee=0;
	char	vLimiName[20+1];
	
	TdCustFunc	sTdCustFunc;
	TdCustFuncAdd	sTdCustFuncAdd;
	TdCustFuncAddHis	sTdCustFuncAddHis;
	TdCustFuncHis	sTdCustFuncHis;
	
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vLimiName);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(vOrgId);
	Sinitn(orgCode);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	
	vRemainFee=iRemainFee;
	
	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p == 10)
		{
			strncpy(end_date, p+2, 8);
			end_date[8] = '\0';
			hasExpire = 1;
		}
		else if (q - p == 2)
		{
			hasExpire = 0;
		}
		else
		{	
			pubLog_DBErr(_FFL_,"","121920","");
			return 121920;
		}

		p = q + 1;
        /*printf("function_code = [%s]\n", function_code);
        */
        pubLog_Debug(_FFL_,"","","function_code = [%s]", function_code);
		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) return iRetCode;
			
		/*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
         	  /*printf("��ȡ�û�org_idʧ��!\n");
         	  */
         	  pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��!");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/	
			
		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
			:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121914","ȡ�û���Ϣ����");
			return 121914;
		}
		
		EXEC SQL select count(*) into :num from dconmsg 
						where contract_no in(select contract_no from  dconusermsg where id_no=:idNo and serial_no=0)
	   				and pay_code='4';
	  if(num>0&&(strcmp(function_code,"00")==0||strcmp(function_code,"17")==0))
	  	/*printf("This is a specular user,vRemainFee=[%f]\n",vRemainFee);
	  	*/
	  	pubLog_Debug(_FFL_,"","","This is a specular user,vRemainFee=[%f]",vRemainFee);
	  else
	  {
		Sinitn(vLimiName);
		EXEC SQL select nvl(limit_prefee,0),function_name
			into :vLimitPay,:vLimiName
			from sFuncList
			 WHERE region_code=substr(:belongCode,1,2)
			   AND sm_code = :smCode
			   AND function_code = :function_code;
		if(SQLCODE!=0)
		{
			/*printf("121957 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121957","121957 SqlCode =[%d]", SQLCODE);
			return 121957;
		}
		if(vLimitPay>vRemainFee)
		{
			/*printf("121958 vLimitPay  =[%f]\n", vLimitPay);
			  printf("121958 vRemainFee =[%f]\n", vRemainFee);
			*/
			pubLog_DBErr(_FFL_,"","121958","121958 vLimitPay  =[%f]vRemainFee =[%f]", vLimitPay,vRemainFee);
			sprintf(funcode,"[%s]%s",function_code,vLimiName);
			*olimitFee=vLimitPay;
			return 121958;
		}
	}
		/*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFunc"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v3,1,2)"
			"   AND sm_code = :v4"
			"   AND function_code = :v5"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
		}*/
		Trim(idNo);
		EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE,	:opTime into :sTdCustFunc 
	 	  FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code ;
	 	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","������ʾδ����");
			return 121915;		
		}
	  ret = 0; 			
  	ret=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"NewfAddUsedFuncs",sTdCustFunc);
		if (ret!=0)
  	{    				  
  	 	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
  	}
    /*
		sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
		}
    	*/
    	Trim(idNo);
	    EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
			    :loginNo,	:loginAccept, :totalDate, :opTime, :opCode, 'a'
			    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	  	if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;		
		}
		ret = 0; 					
	  	ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"NewfAddUsedFuncs",sTdCustFuncHis);
		if (ret != 0)
	  	{    				  
	  	  	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
	  	}
    
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAdd"
			"("
			"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
			"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
			"add_months(to_date(:v3, 'yyyymmdd hh24:mi:ss'), month_limit), MONTH_LIMIT, NULL, NULL"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v4,1,2)"
			"   AND sm_code = :v5"
			"   AND function_code = :v6"
			"   AND month_limit > 0");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:belongCode,
			:smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
			return 121917;
		}*/
		Trim(idNo);
		EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE,	:opTime,
			 to_char(add_months(to_date(:opTime, 'yyyymmdd hh24:mi:ss'), month_limit),'yyyymmdd hh24:mi:ss'), to_char(MONTH_LIMIT), chr(0), chr(0) into :sTdCustFuncAdd
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code
			 AND month_limit > 0;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121917","������ʾδ����");
			return 121917;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 			 	
			ret=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"NewfAddUsedFuncs",sTdCustFuncAdd);
			if (ret < 0)
			{    				  
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
				return 121917;
			}
		}
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
						:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			//printf("121918 SqlCode = [%d]\n", SQLCODE);
			
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
			return 121918;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE, FUNCTION_CODE, nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		   nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),nvl(ADD_NO,chr(0)),
			 nvl(OTHER_ATTR,chr(0)),:loginNo,:loginAccept,:totalDate, :opTime,:opCode, 'a'
			 into :sTdCustFuncAddHis  FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","������ʾδ����");
			return 121918;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 			 	 	
			ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"NewfAddUsedFuncs",sTdCustFuncAddHis);
			if (ret < 0)
			{    				  
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
				return 121918;
			}
		}
    	/*ng���� by zhaohx at 04/08/2009 end*/ 
		if (hasExpire)
		{ /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/

			sprintf(dynStmt,
				"INSERT INTO wCustExpire"
				"("
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
				")"
				" SELECT"
				"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
				"	:v4,:vOrgId,	:v5,	to_number(:v6),"
				"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
				"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��'"
				"  FROM sFuncList"
				" where region_code=substr(:v12, 1,2)"
				"   and sm_code = :v13"
				"   and function_code = :v14");
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
								:orgCode,:vOrgId,	:loginNo,	:loginAccept,
								:totalDate, :opCode,	:opTime, :function_code,
								:end_date, 
								:belongCode, :smCode, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{
				/*printf("121919 SqlCode = [%d]\n", SQLCODE);
				*/
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121919","121919 SqlCode = [%d]", SQLCODE);
				return 121919;
			}
		}
	}
	return 0;
}

/**
function name :newpubDefaultFunc
function desc :��ͨĬ���ط�ƴ��
**/
int newpubDefaultFunc(
      char sIn_User_idA[23] , char sIn_Login_noA[7] ,
      char sIn_Belong_CodeA[8] , char sIn_Sm_CodeA[3] ,
      char sIn_Total_dateA[21] , char sIn_Sys_dateA[21] ,
      char sIn_Login_acceptA[39] , char sIn_Op_codeA[5] ,
      char * sErrMessA , char sIn_Mode_codeA[9] ,
      char *defaultList)
{
    EXEC SQL BEGIN DECLARE SECTION;    
        char    sIn_User_id[22+1];            /* �û�ID       */
        char    sIn_Login_no[6+1];            /* ��������     */
        char    sIn_Belong_Code[7+1];         /* ��������     */
        char    sIn_Sm_Code[2+1];             /* ҵ������     */
        char    sIn_Total_date[20+1];         /* ��������     */
        char    sIn_Sys_date[20+1];           /* ϵͳ����     */
        char    sIn_Login_accept[38+1];       /* ������ˮ     */
        char    sIn_Op_code[4+1];             /* ��������     */
        char    sIn_Mode_code[9];             /* ���ʷѴ���   */
        
        char    sV_Region_code[2+1];          /* ��������     */
        char    sV_District_code[2+1];        /* ���ش���     */
        char    sV_Town_code[3+1];            /* �������     */
        char    sV_Function_type[1+1];        /* �ط�����     */
        char    sV_Function_code[2+1];        /* �ط�����     */
        char    sV_Add_flag[1+1];             /* ���ӱ�־     */
        int     sV_flag=0;
        char	vdefaultList[64+1];
		
    EXEC SQL END DECLARE SECTION;
    
    int ret=0;
    
    printf("--------------- begin function newpubDefaultFunc ----------\n");
    
    /* ���Ի����� */
    init(sIn_User_id);
    init(sIn_Login_no);
    init(sIn_Belong_Code);
    init(sIn_Sm_Code);
    init(sIn_Total_date);
    init(sIn_Sys_date);
    init(sIn_Login_accept);
    init(sIn_Op_code);
    init(sIn_Mode_code);
    
    init(sV_Region_code);
    init(sV_District_code);
    init(sV_Town_code);
    init(sV_Function_type);
    init(sV_Function_code);
    init(sV_Add_flag);
    init(vdefaultList);
    
    /* ȡ��������� */
    strcpy(sIn_User_id,sIn_User_idA);
    strcpy(sIn_Login_no,sIn_Login_noA);
    strcpy(sIn_Belong_Code,sIn_Belong_CodeA);
    strcpy(sIn_Sm_Code,sIn_Sm_CodeA);
    strcpy(sIn_Total_date,sIn_Total_dateA);
    strcpy(sIn_Sys_date,sIn_Sys_dateA);
    strcpy(sIn_Login_accept,sIn_Login_acceptA);
    strcpy(sIn_Op_code,sIn_Op_codeA);
    strcpy(sIn_Mode_code,sIn_Mode_codeA);
    
    #ifdef _DEBUG_
       /*printf("sIn_User_id      =[%s]\n",      sIn_User_id);
        printf("sIn_Login_no      =[%s]\n",      sIn_Login_no);
        printf("sIn_Belong_Code      =[%s]\n",      sIn_Belong_Code);
        printf("sIn_Sm_Code      =[%s]\n",      sIn_Sm_Code);
        printf("sIn_Total_date      =[%s]\n",      sIn_Total_date);
        printf("sIn_Sys_date      =[%s]\n",      sIn_Sys_date);
        printf("sIn_Login_accept      =[%s]\n",      sIn_Login_accept);
        printf("sIn_Op_code      =[%s]\n",      sIn_Op_code);
        */
        pubLog_Debug(_FFL_,"","","sIn_User_id      =[%s]",      sIn_User_id);
        pubLog_Debug(_FFL_,"","","sIn_Login_no      =[%s]",      sIn_Login_no);
        pubLog_Debug(_FFL_,"","","sIn_Belong_Code      =[%s]",      sIn_Belong_Code);
        pubLog_Debug(_FFL_,"","","sIn_Sm_Code      =[%s]",      sIn_Sm_Code);
        pubLog_Debug(_FFL_,"","","sIn_Total_date      =[%s]",      sIn_Total_date);
        pubLog_Debug(_FFL_,"","","sIn_Sys_date      =[%s]",      sIn_Sys_date);
        pubLog_Debug(_FFL_,"","","sIn_Login_accept      =[%s]",      sIn_Login_accept);
        pubLog_Debug(_FFL_,"","","sIn_Op_code      =[%s]",      sIn_Op_code);
    #endif
    
    /* ���  �������� */
    EXEC SQL SELECT substr(:sIn_Belong_Code,1,2),substr(:sIn_Belong_Code,3,2),substr(:sIn_Belong_Code,5,3)
             INTO :sV_Region_code,:sV_District_code,:sV_Town_code FROM dual;
    if(SQLCODE!=0){
        sprintf(sErrMessA,"%s","��ֹ�������ʧ��!");
        /*printf("SQLCODE = [%d],SQLERRMSG = [%s]\n",SQLCODE,SQLERRMSG);
        */
        PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-1","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -1;
    } 
    #ifdef _DEBUG_
       /* printf("sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]\n",
        sV_Region_code,sV_District_code,sV_Town_code); 
        */
        pubLog_Debug(_FFL_,"","","sV_Region_code=[%s],sV_District_code=[%s],sV_Town_code=[%s]",
        sV_Region_code,sV_District_code,sV_Town_code); 
    #endif
    
    /* ��ѯĬ�� */
    EXEC SQL DECLARE c102 CURSOR for 
        select FUNCTION_TYPE,FUNCTION_CODE , ADD_FLAG 
        from sFuncList 
        where REGION_CODE = :sV_Region_code
            and SM_CODE = :sIn_Sm_Code
            and DEFAULT_ADD_FLAG = '1'
            and show_flag='Y'
            and function_code not in(
        select b.function_code from sBillModeDetail a,sBillFuncBind b
        where a.region_code = :sV_Region_code and a.mode_code = :sIn_Mode_code
            and a.detail_code=b.FUNCTION_BIND
            and a.detail_type='a' and a.region_code=b.region_code);

    EXEC SQL OPEN c102 ;
    EXEC SQL FETCH c102 INTO :sV_Function_type , :sV_Function_code , :sV_Add_flag;
    if (SQLCODE==1403)
    {
    	strcpy(sErrMessA,"SUCCESSED!");
    	/*printf("SQLCODE = [%d],SQLERRMSG = [%s]\n",SQLCODE,SQLERRMSG);
    	printf("û��Ĭ���ط�\n");
    	*/
    	pubLog_Debug(_FFL_,"","","SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);	
    	pubLog_Debug(_FFL_,"","","û��Ĭ���ط�");
    	return 0;
    }
        
    if ((SQLCODE!=0)&&(SQLCODE!=1403))
    {
    	sprintf(sErrMessA,"%s","��ѯsFuncList��ʧ��!");
    	/*printf("SQLCODE = [%d]  ,SQLERRMSG = [%s]\n",SQLCODE,SQLERRMSG);
    	*/
    	PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-2","SQLCODE = [%d]  ,SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
        return -2;
    }
    
    while(SQLCODE == 0)
    {
        Trim(sV_Function_type);
        Trim(sV_Function_code);
        Trim(sV_Add_flag);
        
        #ifdef _DEBUG_
           /* printf("sV_Function_type      =[%s]\n",      sV_Function_type);
            printf("sV_Function_code      =[%s]\n",      sV_Function_code);
            printf("sV_Add_flag      =[%s]\n",      sV_Add_flag);
            */
           pubLog_Debug(_FFL_,"","","sV_Function_type      =[%s]",      sV_Function_type);
           pubLog_Debug(_FFL_,"","","sV_Function_code      =[%s]",      sV_Function_code);
           pubLog_Debug(_FFL_,"","", "sV_Add_flag      =[%s]",      sV_Add_flag);
        #endif
        
        EXEC SQL select count(*) into :sV_flag from dCustFunc where id_no=to_number(:sIn_User_id) and function_code=:sV_Function_code;
        if(sV_flag>0)
        {
            /*printf("aaaaaaaaaaaasV_Function_code=[%s]\n",sV_Function_code);
            */
            PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","","aaaaaaaaaaaasV_Function_code=[%s]",sV_Function_code);
        	continue;
        }
        else
        {
        	sprintf(vdefaultList,"%s%s|",vdefaultList,sV_Function_code);
        }

        EXEC SQL FETCH c102 INTO :sV_Function_type , :sV_Function_code , :sV_Add_flag;
    }
    EXEC SQL CLOSE c102 ;
    
    Trim(vdefaultList);
    strcpy(defaultList,vdefaultList);
    
    strcpy(sErrMessA,"SUCCESSED!");
    
    /*printf("--------------- end function newpubDefaultFunc ----------\n");
    */
     pubLog_Debug(_FFL_,"","","--------------- end function newpubDefaultFunc ----------");
    return 0 ;
	
}

/***************************************
 *function  ��dcustmsg��ȡgroup_id
 *author    liuweib
 *created   2009-02-19 15:43
****************************************/

int sGetUserGroupid(char * phone_no,char * group_id)
{
	EXEC SQL BEGIN DECLARE SECTION;
   char vPhoneNo[15+1];
   char vGroupId[10+1];
   EXEC SQL END DECLARE SECTION;
   
   init(vPhoneNo);
   init(vGroupId);
   
   strcpy(vPhoneNo,phone_no);
   Trim(vPhoneNo);
   
   /*printf("==%s==\n",vPhoneNo);
   */
    pubLog_Debug(_FFL_,"","","==%s==",vPhoneNo);
   EXEC SQL select nvl(group_no,' ')
               into :vGroupId
             from dcustmsg 
             where phone_no = :vPhoneNo;
   if(SQLCODE !=0)
   	{ 
   		/*printf("-%s-\n",vGroupId);
   		  printf("��ѯ�û�group_idʧ��SQLCODE=[%d]\n",SQLCODE);
   		*/
   		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100","-%s-,��ѯ�û�group_idʧ��SQLCODE=[%d]",vGroupId,SQLCODE);
   		return -100;
   	}
   
  /*printf("-%s-\n",vGroupId);
  */
   pubLog_Debug(_FFL_,"","","-%s-",vGroupId);
   	
   Trim(vGroupId);
   strcpy(group_id,vGroupId);
   
   return 0;
}

/***************************************
 *function  ��dcustdoc��ȡgroup_id(�����û�)
 *author    liuweib
 *created   2009-02-19 15:43
****************************************/

int sGetDocGroupid(long cust_id, char * group_id)
{	
	EXEC SQL BEGIN DECLARE SECTION;
   long iCustId =0;
   char vGroupId[10+1];
   EXEC SQL END DECLARE SECTION;
   
   init(vGroupId);
   
   iCustId =cust_id;
   
   EXEC SQL select nvl(group_id,' ')
              into :vGroupId
             from dcustdoc
             where cust_id = :iCustId;
   if(SQLCODE !=0)
   	{
   		/*printf("��ѯ�ͻ�group_idʧ��,SQLCODE[%d]\n",SQLCODE);
   		*/
   		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100","��ѯ�û�group_idʧ��SQLCODE=[%d]",SQLCODE);
   		return -100;
   	}
   	
   Trim(vGroupId);
   strcpy(group_id,vGroupId);
   
   return 0;
}

/***************************************
 *function  ��dloginmsg��ȡorg_id
 *author    liuweib
 *created   2009-02-19 15:43
****************************************/
int sGetLoginOrgid( char * login_no ,char * org_id)
{
	EXEC SQL BEGIN DECLARE SECTION;
   char vLoginNo[10+1];
   char vOrgId[10+1];
   EXEC SQL END DECLARE SECTION;
   
   init(vLoginNo);
   init(vOrgId);
   
   strcpy(vLoginNo,login_no);
   Trim(vLoginNo);
   
   EXEC SQL select nvl(org_id,' ')
            into :vOrgId
            	from dloginmsg
            	where login_no = :vLoginNo;
   if(SQLCODE != 0)
   	{ 
   		/*printf("��ȡ����org_idʧ��,SQLCODE[%d]\n",SQLCODE);
   		*/
   		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100","��ȡ����org_idʧ��,SQLCODE[%d]",SQLCODE);
   		return -100;
   	}
   Trim(vOrgId);
   strcpy(org_id,vOrgId);
   
   return 0;
}

/***************************************
 *function  ��dchngroupmsg��ȡboss_org_code
 *author    liuweib
 *created   2009-02-19 15:43
****************************************/
int sGetOrgCode( char *group_id  ,char * boss_org_code)
{
	EXEC SQL BEGIN DECLARE SECTION;
   char vGroupId[10+1];
   char vBossOrgCode[30+1];
   EXEC SQL END DECLARE SECTION;
   
   init(vGroupId);
   init(vBossOrgCode);
   
   strcpy(vGroupId,group_id);
   Trim(vGroupId);
   
   EXEC SQL select boss_org_code
            into :vBossOrgCode
            	from dchngroupmsg
            	where group_id = :vGroupId;
   if(SQLCODE != 0)
   	{
   		/*printf("��ȡboss_org_codeʧ��,SQLCODE[%d]\n",SQLCODE);
   		*/
   	    PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100","��ȡboss_org_codeʧ��,SQLCODE[%d]",SQLCODE);
   		return -100;
   	}
   Trim(vBossOrgCode);
   strcpy(boss_org_code,vBossOrgCode);
   
   return 0;
}

/***************************************
 *function  ��dchngroupmsg��ȡgroup_id()
 *author    liuweib
 *created   2009-02-19 15:43
****************************************/
int sGetGroupClass( char *group_id  ,tGroupClass * Group_Class)
{
	EXEC SQL BEGIN DECLARE SECTION;
    char vGroupId[10+1];
   	char	vClassKind[1+1];	/*��������*/
    char	vKindName[40+1];	/*��������*/
    char    vIsAccount[1+1];   
    char    vIsTown[1+1];		/*�Ƿ���Ӫҵ��Y,N*/
    char	vIsAgent[1+1];		/*�Ƿ��Ǵ����Y,N*/
    char	vDocType[1+1];
   EXEC SQL END DECLARE SECTION;
   
   init(vGroupId);
   init(vClassKind);
   init(vKindName);
   init(vIsAccount);
   init(vIsTown);
   init(vIsAgent);
   init(vDocType);
   
   
   strcpy(vGroupId,group_id);
   Trim(vGroupId);
   
   EXEC SQL select a.class_kind,a.kind_name,a.is_account,a.is_town,a.is_agent,a.doc_type
                   INTO :vClassKind,:vKindName,:vIsAccount,:vIsTown,:vIsAgent,:vDocType
                   from dbqdadm.schnclasskind a,dbqdadm.schnclassmsg b,dchngroupmsg c
                   where a.class_kind = b.class_kind
                   and   b.class_code = c.CLASS_CODE
                   and   c.group_id = :vGroupId;
   if(SQLCODE != 0)
   	{
   		/*printf("��ȡboss_org_codeʧ��,SQLCODE[%d]\n",SQLCODE);
   		*/
   		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100","��ȡboss_org_codeʧ��,SQLCODE[%d]",SQLCODE);
   		return -100;
   	}
   	
   Trim(vClassKind);
   Trim(vKindName);
   Trim(vIsAccount);
   Trim(vIsTown);
   Trim(vIsAgent);
   Trim(vDocType);
   
   strcpy(Group_Class->vClass_Kind,vClassKind);
   strcpy(Group_Class->vKind_Name,vKindName);
   strcpy(Group_Class->vIs_Account,vIsAccount);
   strcpy(Group_Class->vIs_Town,vIsTown);
   strcpy(Group_Class->vIs_Agent,vIsAgent);
   strcpy(Group_Class->vDoc_Type,vDocType);
   
   return 0;
}
/***************************************
 *function  ��dconmsg��ȡgroup_id
 *author    zhangxha
 *created   2009-03-03 11:07
****************************************/

int sGetConGroupid(long contract_no,char * group_id)
{
	EXEC SQL BEGIN DECLARE SECTION;
	long iContractNo =0;
	char vGroupId[10+1];
	EXEC SQL END DECLARE SECTION;
	
	init(vGroupId);
	
	iContractNo = contract_no;
	
	EXEC SQL select nvl(group_id,' ')
			into :vGroupId
			from dconmsg 
			where contract_no = :iContractNo;
	if(SQLCODE !=0)
	{ 
	  /*printf("-%s-\n",vGroupId);
		printf("��ѯ�ʻ�group_idʧ��SQLCODE=[%d]\n",SQLCODE);
	  */	
	  	PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","-100",vGroupId,SQLCODE);
		return -100;
	}
	
	/*printf("vGroupId=[%s]\n",vGroupId);
	*/
	pubLog_Debug(_FFL_,"","","vGroupId=[%s]",vGroupId);
	Trim(vGroupId);
	strcpy(group_id,vGroupId);
	
	return 0;
}

/***************************************
 *function  �Ƚ�����group_id��ĳ�������group_id�Ƿ����
 *author    liuwei
 *created   2009-04-29 11:07
****************************************/

int sCheckGroupId(char *group_id1, char *group_id2,char * class_code,char * flag,char* retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vGroupId1[10+1];
	char vGroupId2[10+1];
	char vPGroupId1[10+1];
	char vPGroupId2[10+1];
	char vClassCode[5][4+1];
	char vClassTmp[60+1];
	int i = 0,iCount  =0;
	EXEC SQL END DECLARE SECTION;
	
	init(vGroupId1);
	init(vGroupId2);
	init(vPGroupId1);
	init(vPGroupId2);
	memset(&vClassCode,0,sizeof(vClassCode));
	
	strcpy(vGroupId1,group_id1);
	strcpy(vGroupId2,group_id2);
	strcpy(vClassTmp,class_code);
	
	Trim(vGroupId1);
	Trim(vGroupId2);
	Trim(vClassTmp);
	
	strcat(vClassTmp,"|");
	pubLog_Debug(_FFL_,"sCheckGroupId","vClassTmp","[%s]",vClassTmp);
	
	strToRows(vClassTmp,'|',vClassCode, 5);
	
		
	for(i=0;i<=4;i++)
	{
		pubLog_Debug(_FFL_,"","vClassCode[%d]","[%s]",i,vClassCode[i]);
	}
	printf("vGroupId1[%s]\n",vGroupId1);
	EXEC SQL SELECT B.PARENT_GROUP_ID
		INTO :vPGroupId1
		FROM DCHNGROUPMSG A,DCHNGROUPINFO B
		WHERE B.GROUP_ID = :vGroupId1
		AND B.PARENT_GROUP_ID = A.GROUP_ID
		AND A.CLASS_CODE IN(:vClassCode[0],:vClassCode[1],:vClassCode[2],:vClassCode[3],:vClassCode[4]);
	if(SQLCODE != 0)
	{	
		sprintf(retMsg,"��ȡ���ڵ�ʧ��[%s]",vGroupId1);
		PUBLOG_DBDEBUG("sCheckGroupId");
		pubLog_DBErr(_FFL_,"sCheckGroupId","100001","��ȡ���ڵ�ʧ��[%s][%s]",vGroupId2,vGroupId1);
		return 100001;
	}
	printf("vGroupId2[%s]\n",vGroupId2);
	EXEC SQL SELECT B.PARENT_GROUP_ID
		INTO :vPGroupId2
		FROM DCHNGROUPMSG A,DCHNGROUPINFO B
		WHERE B.GROUP_ID = :vGroupId2
		AND B.PARENT_GROUP_ID = A.GROUP_ID
		AND A.CLASS_CODE IN(:vClassCode[0],:vClassCode[1],:vClassCode[2],:vClassCode[3],:vClassCode[4]);
	if(SQLCODE != 0)
	{	
		sprintf(retMsg,"��ȡ���ڵ�ʧ��[%s]",vGroupId2);
		PUBLOG_DBDEBUG("sCheckGroupId");
		pubLog_DBErr(_FFL_,"sCheckGroupId","100001","��ȡ���ڵ�ʧ��[%s][%s]",vGroupId2,vGroupId1);
		return 100001;
	}		
	Trim(vPGroupId1);
	Trim(vPGroupId2);		
	pubLog_Debug(_FFL_,"","vPGroupId1","[%s]",vPGroupId1);
	pubLog_Debug(_FFL_,"","vPGroupId2","[%s]",vPGroupId2);
	
	if(strcmp(vPGroupId1,vPGroupId2)==0)
	{
		strcpy(flag,"Y");
	}	
	else
	{
		strcpy(flag,"N");
	}
	
	pubLog_Debug(_FFL_,"","flag","[%s]",flag);
	return 0;
}

/***************************************
 *function  ��ȡgroup_id��ĳ�������ϵĸ��ڵ�
 *author    liuweib
 *created   2009-04-29 11:07
****************************************/
int sGetGroupId(char * group_id,char * p_group_id,char * class_code,char * retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vGroupId[10+1];
	char vPGroupId[10+1];
	char vClassCode[5][4+1];
	char vClassTmp[60];
	EXEC SQL END DECLARE SECTION;
	
	init(vGroupId);
	init(vPGroupId);
	init(vClassCode);
	init(vClassTmp);
	
	strcpy(vGroupId,group_id);
	strcpy(vClassTmp,class_code);
	Trim(vGroupId);
	Trim(vClassTmp);
	strcat(vClassTmp,"|");
	pubLog_Debug(_FFL_,"sCheckGroupId","vClassTmp","[%s]",vClassTmp);
	
	strToRows(vClassTmp,'|',vClassCode, 5);

	EXEC SQL SELECT A.GROUP_ID
			INTO :vPGroupId
			FROM dChnGroupMsg A, dChnGroupInfo B
			WHERE A.GROUP_ID = B.PARENT_GROUP_ID
			AND B.GROUP_ID = :vGroupId
			AND A.CLASS_CODE in(:vClassCode[0],:vClassCode[1],:vClassCode[2],:vClassCode[3],:vClassCode[4]);
	if(SQLCODE != 0)
	{	
		sprintf(retMsg,"��ȡ���ڵ�ʧ��[%s][%s][%s][%s][%s]",vClassCode[0],vClassCode[1],vClassCode[2],vClassCode[3],vClassCode[4]);
		PUBLOG_DBDEBUG("sGetGroupId");
		pubLog_DBErr(_FFL_,"sGetGroupId","100001","��ȡ���ڵ�ʧ��[%s][%s][%s][%s][%s]",vClassCode[0],vClassCode[1],vClassCode[2],vClassCode[3],vClassCode[4]);
		return 100001;
	}
	
	Trim(vPGroupId);
	
	strcpy(p_group_id,vPGroupId);
	
	return 0;
}

/***************************************
 *function  ��֤ĳ��group_id�ĵȼ�
 *author    liuweib
 *created   2009-04-29 11:07
****************************************/
int sCheckGroupLevel(char * group_id,char * class_code,char * is_town,char *retMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vGroupId[10+1];
	char vClassCode[4+1];
	char vIsTown[1+1];
	EXEC SQL END DECLARE SECTION;
	
	init(vGroupId);
	init(vIsTown);
	init(vClassCode);
	
	strcpy(vGroupId,group_id);
	Trim(vGroupId);
	
	EXEC SQL SELECT CLASS_CODE
		INTO :vClassCode
		FROM dChnGroupMsg
		WHERE GROUP_ID = :vGroupId;
	if(SQLCODE != 0)
	{	
		sprintf(retMsg,"��ȡclass_codeʧ��[%s]",vGroupId);
		PUBLOG_DBDEBUG("sCheckGroupLevel");
		pubLog_DBErr(_FFL_,"sCheckGroupLevel","100001","��ȡclass_codeʧ��[%s]",vGroupId);
		return 100001;
	}
	
	/*CLAS_CODE= '1058'���Ǵ�����,����Ӫҵ��,��is_town��ֵΪY.group_id�㼶ȷ��ʱ,Ҫ���ⲿ��������ΪN*/
	EXEC SQL SELECT c.IS_TOWN
 		INTO :vIsTown
 		FROM dChnGroupMsg a,dbqdadm.sChnClassMsg b,dbqdadm.sChnClassKind c 
 		WHERE a.CLASS_CODE = b.CLASS_CODE
 		AND b.CLASS_KIND = c.CLASS_KIND
 		AND a.GROUP_ID = :vGroupId;
 	if(SQLCODE != 0)
	{	
		sprintf(retMsg,"��ȡclass_codeʧ��[%s]",vGroupId);
		PUBLOG_DBDEBUG("sCheckGroupLevel");
		pubLog_DBErr(_FFL_,"sCheckGroupLevel","100001","��ȡclass_codeʧ��[%s]",vGroupId);
		return 100001;
	}
	
	Trim(vClassCode);
	Trim(vIsTown);
	strcpy(class_code,vClassCode);
	strcpy(is_town,vIsTown);
	
	return 0;
	
}

/***********************************************************
*�������ƣ���ȡ��֯������������
*�������ڣ�20090514
* Copyright (c) 1995,2009 ������������˼������Ϣ�����ɷ����޹�˾ . All rights reserved.
*������Ա��zhaoqm
*�������������ݳ�group_id��ȡ��group_id������group_name���� ĳĳ����-ĳĳ��--ĳĳӪҵ��
*���������
*  		��֯�������	group_id
*���������
*	  	��֯��������	group_name
*	 	������Ϣ		retmsg
***********************************************************/
int sGetAllGroupName(const char* group_id,char* group_name,char* retmsg)
{	
	
	EXEC SQL BEGIN DECLARE SECTION;
		char	group_nametemp[200+1];
		char	Vgroup_name[200+1];
		char	group_idtemp[10+1];
		char	synsql[200];
		int		size=0;
	EXEC SQL END DECLARE SECTION;
	
#ifdef _DEBUG_
       pubLog_Debug(_FFL_,"","sGetAllGroupName","START[%s]",group_id);
#endif	
	memset(group_nametemp,'\0',sizeof(group_nametemp));
	memset(Vgroup_name,'\0',sizeof(Vgroup_name));
	memset(group_idtemp,0,sizeof(group_idtemp));
	
	strcpy(group_idtemp,group_id);

	Trim(group_idtemp);
	/*��ѯgroup_name*/
	EXEC SQL DECLARE SEARMSG CURSOR FOR	
 		SELECT a.group_name
		FROM dChnGroupMsg a,dChnGroupInfo b
		WHERE b.parent_group_id = a.group_id
			AND b.group_id = :group_idtemp
			AND a.class_code <> '10'
			ORDER by b.denorm_level DESC;	
	
	EXEC SQL OPEN SEARMSG ;
	if(SQLCODE != 0)
	{
		sprintf(retmsg,"��ѯgroup_nameʧ��[%d]",SQLCODE);
		EXEC SQL CLOSE SEARMSG ;
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","300001","��ȡGROUP_NAMEʧ��[%s]",group_idtemp);
		return 300001;
	}
	while(1)
	{
		EXEC SQL FETCH SEARMSG INTO:group_nametemp;
		Trim(group_nametemp);						
		if(SQLCODE == 0)							/*ƴ��ʽ��ĳĳ����-ĳĳ�е��ַ���*/
		{
			strcat(Vgroup_name,group_nametemp);
			strcat(Vgroup_name,"-");
			Trim(Vgroup_name);
		}
		else if(1403 == SQLCODE&&0==SQLROWS)		/*��ѯ���Ϊ��*/
		{
			sprintf(retmsg,"��ѯʧ��[%d]",SQLCODE);
			EXEC SQL CLOSE SEARMSG ;
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","300002","��ȡGROUP_NAMEʧ��[%s]",group_idtemp);
			return 300002;
		}
		else if(1403 == SQLCODE)					/*ȡ�α����*/
		{
			Trim(Vgroup_name);
			size=strlen(Vgroup_name);
			Vgroup_name[size-1]='\0';				/*ȥ��ĩβ��"-"*/
			EXEC SQL CLOSE SEARMSG ;
			pubLog_Debug(_FFL_,"","sGetAllGroupName","�α��ѿ�[%s]",group_idtemp);	
			break;
		}
		else
		{
			sprintf(retmsg,"��ѯʧ��[%d]",SQLCODE);
			EXEC SQL CLOSE SEARMSG ;
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","300003","��ȡGROUP_NAMEʧ��[%s]",group_idtemp);
			return 300003;
		}
		
	}
	strcpy(group_name,Vgroup_name);
	
	pubLog_Debug(_FFL_,"","group_name","[%s]",group_name);
	
#ifdef _DEBUG_
    pubLog_Debug(_FFL_,"","sGetAllGroupName","END");
#endif

	return 0;
} 


/***************************************
 *��������	��ȡgroup_id��Ӧ��group_name
 *Copyright (c) 1995,2009 ������������˼������Ϣ�����ɷ����޹�˾ . All rights reserved.
 *�������
 *			group_id	��֯�������
 *�������
 *			group_Name	��ǰ��ѡ�������
 *			ret_Msg		������Ϣ
 *���ز���
 *			�Ƿ�ִ�гɹ���0���ɹ� ���������ɹ�
 *������Ա	zhengxg
 *��������	2009-05-14
****************************************/
int sGetGroupName(const char *group_id, char *group_Name, char *ret_Msg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vGroupId[10 + 1];
		char	Vgroup_Name[60 + 1];
	EXEC SQL END DECLARE SECTION;

	init(vGroupId);
	init(Vgroup_Name);
	
	strcpy(vGroupId,group_id);
	Trim(vGroupId);
	
#ifdef _DEBUG_
       pubLog_Debug(_FFL_,"","sGetGroupName","START[%s]",group_id);
#endif	
	/*��ȡӪҵ��������*/
	EXEC SQL SELECT	nvl(GROUP_NAME,' ') 
		INTO :Vgroup_Name
		FROM dChnGroupMsg
		WHERE GROUP_ID=:vGroupId;
	if(0 != SQLCODE)
	{
		sprintf(ret_Msg,"SQLCODE=[%d]", SQLCODE);
		PUBLOG_DBDEBUG("sGetGroupName");
		pubLog_DBErr(_FFL_,"sGetGroupName", "200001", "��ȡgroup_nameʧ��");
		return 200001;
	}
	Trim(Vgroup_Name);
	strcpy(group_Name,Vgroup_Name);
#ifdef _DEBUG_
      pubLog_Debug(_FFL_,"","sGetGroupName","END[%s]",group_id);
#endif
	return 0;	
}
/******************************************************************************\
*	�� �� ����BindPhoneHeadMode
*	��    �ܣ�У������Ŷΰ��ײʹ��뺯�� 
*	���������char* i_RegionCode		��������
*			  char* i_SmCode			Ʒ�ƴ���
*			  char* i_PhoneHead			�����
*			  char* i_ModeCode			���ײʹ���
*
*	���������char* o_Flag				�ײ��жϱ�ʶ
*			  char* o_RetCode			���󷵻���
*			  char* o_RetMsg			���󷵻���Ϣ
*
*	�� �� ֵ��0 -- �ɹ�		��0 -- ʧ��
*	�� �� �ˣ�ZHCY
*	��    �ڣ�2009-06-01
*	��    ע��dphoneheadattr
\******************************************************************************/
int BindPhoneHeadMode(char* i_RegionCode, char* i_SmCode, char* i_PhoneHead, char* i_ModeCode, char* o_Flag, char* o_RetCode, char* o_RetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char l_sRegionCode[2+1];
		char l_sSmCode[2+1];
		char l_sPhoneHead[7+1];
		char l_sModeCode[8+1];
		
		int l_nCount = 0;
	EXEC SQL END DECLARE SECTION;
	
	init(l_sRegionCode);
	init(l_sSmCode);
	init(l_sPhoneHead);
	init(l_sModeCode);
	
	pubLog_Debug(_FFL_,"","","BindPhoneHeadMode BEGIN");
	
	strcpy(l_sRegionCode, i_RegionCode);
	strcpy(l_sSmCode, i_SmCode);
	strcpy(l_sPhoneHead, i_PhoneHead);
	strcpy(l_sModeCode, i_ModeCode);
	strcpy(o_Flag, "N");
	
	pubLog_Debug(_FFL_,"","","l_sRegionCode = [%s]", l_sRegionCode);
	pubLog_Debug(_FFL_,"","","l_sSmCode = [%s]", l_sSmCode);
	pubLog_Debug(_FFL_,"","","l_sPhoneHead = [%s]", l_sPhoneHead);
	pubLog_Debug(_FFL_,"","","l_sModeCode = [%s]", l_sModeCode);
	
	l_nCount = 0;
	EXEC SQL select count(*) into :l_nCount
				from dphoneheadattr 
				where phone_head = :l_sPhoneHead 
						and sm_code = :l_sSmCode 
						and region_code = :l_sRegionCode
						and mode_code = :l_sModeCode;
	if ( SQLCODE != 0 )
	{
		sprintf(o_RetCode, "%06d", SQLCODE);
		sprintf(o_RetMsg, "��ȡ�Ŷ�[%s]���ײ�ʧ��! SQLCODE = [%d]",
				l_sPhoneHead, SQLCODE);	
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return -1;
	}
	
	if ( l_nCount == 0 )
	{
		strcpy(o_RetCode, "140300");
		sprintf(o_RetMsg, "�ײ�[%s]�������ںŶ�[%s]!", l_sModeCode, l_sPhoneHead);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return -2;
	}
	
	strcpy(o_Flag, "Y");
	pubLog_Debug(_FFL_,"","","BindPhoneHeadMode END");
	
	return 0;
}/* END of BindPhoneHeadMode() */





/******************************************************************************\
*	�� �� ����CheckPhoneHeadSM
*	��    �ܣ�����Ŷ�Ʒ��ת��У�麯��
*	���������char* i_RegionCode		��������
*			  char* i_PhoneHead			�����
*			  char* i_OldSM				��Ʒ�ƴ���
*			  char* i_NewSM				��Ʒ�ƴ���
*
*	���������char* o_Flag				�����ʶ
*			  char* o_RetCode			���󷵻���
*			  char* o_RetMsg			���󷵻���Ϣ
*
*	�� �� ֵ��0 -- �ɹ�		��0 -- ʧ��
*	�� �� �ˣ�ZHCY
*	��    �ڣ�2009-06-01
*	��    ע��dphoneheadattr
\******************************************************************************/
int CheckPhoneHeadSM(char* i_RegionCode, char* i_PhoneHead, char* i_OldSM, char* i_NewSM, char* o_Flag, char* o_RetCode, char* o_RetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char l_sRegionCode[2+1];
		char l_sPhoneHead[7+1];
		char l_sOldSM[2+1];
		char l_sNewSM[2+1];
		
		int l_nCount = 0;
	EXEC SQL END DECLARE SECTION;
	
	init(l_sRegionCode);
	init(l_sPhoneHead);
	init(l_sOldSM);
	init(l_sNewSM);
	strcpy(o_Flag, "N");
	
	pubLog_Debug(_FFL_,"","","CheckPhoneHeadSM BEGIN");
	
	strcpy(l_sRegionCode, i_RegionCode);
	strcpy(l_sPhoneHead, i_PhoneHead);
	strcpy(l_sOldSM, i_OldSM);
	strcpy(l_sNewSM, i_NewSM);
	
	pubLog_Debug(_FFL_,"","","l_sRegionCode = [%s]", l_sRegionCode);
	pubLog_Debug(_FFL_,"","","l_sPhoneHead = [%s]", l_sPhoneHead);
	pubLog_Debug(_FFL_,"","","l_sOldSM = [%s]", l_sOldSM);
	pubLog_Debug(_FFL_,"","","l_sNewSM = [%s]", l_sNewSM);
	
	l_nCount = 0;
	EXEC SQL select count(*) into :l_nCount
				from dphoneheadattr 
				where phone_head = :l_sPhoneHead 
						and sm_code = :l_sOldSM 
						and region_code = :l_sRegionCode;
	if ( SQLCODE != 0 )
	{
		sprintf(o_RetCode, "%06d", SQLCODE);
		sprintf(o_RetMsg, "У��Ŷ�[%s]����Ʒ��[[%s]ʧ��! SQLCODE = [%d]", \
				l_sPhoneHead, l_sOldSM, SQLCODE);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);		
		return -1;
	}
	if ( l_nCount == 0 )
	{
		strcpy(o_RetCode, "140300");
		sprintf(o_RetMsg, "�Ŷ�[%s] Ʒ��[%s] ����[%s] ������!", \
				l_sPhoneHead, l_sOldSM, l_sRegionCode);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return 0;
	}
	
	l_nCount = 0;
	EXEC SQL select count(*) into :l_nCount
				from dphoneheadattr 
				where phone_head = :l_sPhoneHead 
						and sm_code = :l_sNewSM 
						and region_code = :l_sRegionCode;
	if ( SQLCODE != 0 )
	{
		sprintf(o_RetCode, "%06d", SQLCODE);
		sprintf(o_RetMsg, "У��Ŷ�[%s]��Ʒ��[[%s]ʧ��! SQLCODE = [%d]", \
				l_sPhoneHead, l_sNewSM, SQLCODE);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);		
		return -1;
	}
	if ( l_nCount == 0 )
	{
		strcpy(o_RetCode, "140300");
		sprintf(o_RetMsg, "�Ŷ�[%s] Ʒ��[%s] ����[%s] ������!", \
				l_sPhoneHead, l_sNewSM, l_sRegionCode);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return 0;
	}
	
	strcpy(o_Flag, "Y");
	
	pubLog_Debug(_FFL_,"","","CheckPhoneHeadSM END");
	
	return 0;
}/* END of CheckPhoneHeadSM() */





/******************************************************************************\
*	�� �� ����CheckPhoneHeadMode
*	��    �ܣ�����Ŷ��ײͱ�����麯��
*	���������char* i_RegionCode		��������
*			  char* i_PhoneHead			�����
*			  char* i_OldModeCode		���ײʹ���
*			  char* i_NewModeCode		���ײʹ���
*
*	���������char* o_Flag				�����ʶ
*			  char* o_RetCode			���󷵻���
*			  char* o_RetMsg			���󷵻���Ϣ
*
*	�� �� ֵ��0 -- �ɹ�		��0 -- ʧ��
*	�� �� �ˣ�ZHCY
*	��    �ڣ�2009-06-01
*	��    ע��dphoneheadattr
\******************************************************************************/
int CheckPhoneHeadMode(char* i_RegionCode, char* i_PhoneHead, char* i_OldModeCode, char* i_NewModeCode, char* o_Flag, char* o_RetCode, char* o_RetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char l_sRegionCode[2+1];
		char l_sPhoneHead[7+1];
		char l_sOldModeCode[8+1];
		char l_sNewModeCode[8+1];
		
		int l_nCount = 0;
	EXEC SQL END DECLARE SECTION;
	
	init(l_sRegionCode);
	init(l_sPhoneHead);
	init(l_sOldModeCode);
	init(l_sNewModeCode);
	strcpy(o_Flag, "N");
	
	pubLog_Debug(_FFL_,"","","CheckPhoneHeadMode BEGIN");
	
	strcpy(l_sRegionCode, i_RegionCode);
	strcpy(l_sPhoneHead, i_PhoneHead);
	strcpy(l_sOldModeCode, i_OldModeCode);
	strcpy(l_sNewModeCode, i_NewModeCode);
	
	pubLog_Debug(_FFL_,"","","l_sRegionCode = [%s]", l_sRegionCode);
	pubLog_Debug(_FFL_,"","","l_sPhoneHead = [%s]", l_sPhoneHead);
	pubLog_Debug(_FFL_,"","","l_sOldModeCode = [%s]", l_sOldModeCode);
	pubLog_Debug(_FFL_,"","","l_sNewModeCode = [%s]", l_sNewModeCode);
	
	l_nCount = 0;
	EXEC SQL select count(*) into :l_nCount
				from dphoneheadattr 
				where phone_head = :l_sPhoneHead 
						and mode_code = :l_sOldModeCode 
						and region_code = :l_sRegionCode;
	if ( SQLCODE != 0 )
	{
		sprintf(o_RetCode, "%06d", SQLCODE);
		sprintf(o_RetMsg, "У��Ŷ�[%s]�����ײ�[%s]ʧ��! SQLCODE = [%d]", \
				l_sPhoneHead, l_sOldModeCode, SQLCODE);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);		
		return -1;
	}
	if ( l_nCount == 0 )
	{
		strcpy(o_RetCode, "140300");
		sprintf(o_RetMsg, "�Ŷ�[%s] �ײ�[%s] ����[%s] ������!", \
				l_sPhoneHead, l_sOldModeCode, l_sRegionCode);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return 0;
	}
	
	l_nCount = 0;
	EXEC SQL select count(*) into :l_nCount
				from dphoneheadattr 
				where phone_head = :l_sPhoneHead 
						and mode_code = :l_sNewModeCode 
						and region_code = :l_sRegionCode;
	if ( SQLCODE != 0 )
	{
		sprintf(o_RetCode, "%06d", SQLCODE);
		sprintf(o_RetMsg, "У��Ŷ�[%s]���ײ�[%s]ʧ��! SQLCODE = [%d]", \
				l_sPhoneHead, l_sNewModeCode, SQLCODE);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);		
		return -1;
	}
	if ( l_nCount == 0 )
	{
		strcpy(o_RetCode, "140300");
		sprintf(o_RetMsg, "�Ŷ�[%s] �ײ�[%s] ����[%s] ������!", \
				l_sPhoneHead, l_sNewModeCode, l_sRegionCode);
		pubLog_DBErr(_FFL_,"", o_RetCode, o_RetMsg);
		return 0;
	}
	
	strcpy(o_Flag, "Y");
	
	pubLog_Debug(_FFL_,"","","CheckPhoneHeadMode END");
	
	return 0;
}/* END of CheckPhoneHeadMode() */


/************************************************************************
��������:fGetUserBaseInfo_ng
����ʱ��:2009/09/22
������Ա:zhangzhe
��������:�����ƶ�����ȡ�û��Ļ�����Ϣ
�������:�ƶ�����
�������:�û�������Ϣ
�û�id      
�ͻ�id      
belong_code
ҵ�����ʹ���
ҵ����������
�ͻ�����    
�û�����    
״̬����    
״̬����    
�ȼ�����    
�ȼ�����    
�û����ʹ���
�û���������
�ͻ�סַ    
֤������    
֤������    
֤������    
�ͻ�������
�ͻ�������
���ſͻ���־
���ſͻ�����
bak_field
�û�Ƿ��(tUserOwe)	  
�� �� ֵ:0������ȷ����,��������
************************************************************************/
int fGetUserBaseInfo_ng(const char *phone_no,const char *op_code,tUserBase *user_base)
{           
	EXEC SQL BEGIN DECLARE SECTION;
	/***�������õ���������****/		
	char		vphone_no[15+1];
	char		vid_no[23];	
	char		vcontract_no[23];	
	char		vcust_id[23];
	char		vsm_code[2+1];
	char		vsm_name[20+1];
	char		vcust_name[60+1];
	char		vuser_passwd[8+1];
	char		vrun_code[1+1];
	char		vrun_name[20+1];
	char		vowner_grade[2+1];
	char		vgrade_name[20+1];
	char		vowner_type[2+1];
	char		vtype_name[20+1];
	char		vcust_address[60+1];
	/*char		vid_type[1+1];*/
	char		vid_type[2+1];
	char		vid_name[12+1];
	char		vid_iccid[20+1];
	char 		vcard_type[2+1];
	char 		vcard_name[20+1];
	char 		vgrpbig_flag[1+1];
	char 		vgrpbig_name[20+1];
	double		vowe_fee=0;
	double		vprepay_fee=0;
	char		vregion_code[2+1];
	char        vBelongCode[8];
	char		vst[512];
	char		vop_code[4+1];
	char		sothervalue[255+1];
	int			vtmp_num=0;
	int			vret=0;
	char		retMsg[255+1];
	float		vTotalPrepay=0;
	EXEC SQL END DECLARE SECTION;     
	
	memset(sothervalue,0,sizeof(sothervalue));
	memset(vop_code,0,sizeof(vop_code));
	memset(vphone_no,0,sizeof(vphone_no));
	memset(user_base->id_no,0,sizeof(user_base->id_no));
	memset(user_base->cust_id,0,sizeof(user_base->cust_id));
	memset(user_base->belong_code,0,sizeof(user_base->belong_code));
	memset(user_base->sm_code,0,sizeof(user_base->sm_code));
	memset(user_base->sm_name,0,sizeof(user_base->sm_name));
	memset(user_base->cust_name,0,sizeof(user_base->cust_name));
	memset(user_base->user_passwd,0,sizeof(user_base->user_passwd));
	memset(user_base->run_code,0,sizeof(user_base->run_code));
	memset(user_base->run_name,0,sizeof(user_base->run_name));
	memset(user_base->owner_grade,0,sizeof(user_base->owner_grade));
	memset(user_base->grade_name,0,sizeof(user_base->grade_name));
	memset(user_base->owner_type,0,sizeof(user_base->owner_type));
	memset(user_base->type_name,0,sizeof(user_base->type_name));
	memset(user_base->cust_address,0,sizeof(user_base->cust_address));
	memset(user_base->id_type,0,sizeof(user_base->id_type));
	memset(user_base->id_name,0,sizeof(user_base->id_name));
	memset(user_base->id_iccid,0,sizeof(user_base->id_iccid));
	memset(user_base->card_type,0,sizeof(user_base->card_type));
	memset(user_base->card_name,0,sizeof(user_base->card_name));
	memset(user_base->grpbig_flag,0,sizeof(user_base->grpbig_flag));
	memset(user_base->grpbig_name,0,sizeof(user_base->grpbig_name));
	memset(user_base->bak_field,0,sizeof(user_base->bak_field));
	memset(user_base->user_owe.accountNo,0,sizeof(user_base->user_owe.accountNo));
	user_base->user_owe.totalOwe=0;
	user_base->user_owe.totalPrepay=0;
	user_base->user_owe.accountOwe=0;
	user_base->user_owe.unbillFee=0;

	memset(vid_no,0,sizeof(vid_no));
	
	memset(vcust_id,0,sizeof(vcust_id));
	memset(vcontract_no,0,sizeof(vcontract_no));
	memset(vBelongCode,0,sizeof(vBelongCode));
	memset(vsm_code,0,sizeof(vsm_code));
	memset(vsm_name,0,sizeof(vsm_name));
	memset(vcust_name,0,sizeof(vcust_name));
	memset(vuser_passwd,0,sizeof(vuser_passwd));
	memset(vrun_code,0,sizeof(vrun_code));
	memset(vrun_name,0,sizeof(vrun_name));
	memset(vowner_grade,0,sizeof(vowner_grade));
	memset(vgrade_name,0,sizeof(vgrade_name));
	memset(vowner_type,0,sizeof(vowner_type));
	memset(vtype_name,0,sizeof(vtype_name));
	memset(vcust_address,0,sizeof(vcust_address));
	memset(vid_type,0,sizeof(vid_type));
	memset(vid_name,0,sizeof(vid_name));
	memset(vid_iccid,0,sizeof(vid_iccid));

	memset(vcard_type,0,sizeof(vcard_type));
	memset(vcard_name,0,sizeof(vcard_name));
	memset(vgrpbig_flag,0,sizeof(vgrpbig_flag));
	memset(vgrpbig_name,0,sizeof(vgrpbig_name));

	memset(vregion_code,0,sizeof(vregion_code));
	memset(vst,0,sizeof(vst));

	memcpy(vphone_no,phone_no,sizeof(vphone_no)-1);
	strcpy(vop_code,op_code);
	
	/*��ѯdCustMsg��*/
	Sinitn(vid_no);
	Sinitn(vcust_id);
	Sinitn(vuser_passwd);
	Sinitn(vrun_code);
	Sinitn(vBelongCode);
	EXEC SQL SELECT to_char(id_no),to_char(contract_no),to_char(cust_id),sm_code,
		user_passwd,substr(run_code,2,1),
		substr(attr_code,3,2),substr(attr_code,5,1),
		belong_code,substr(belong_code,1,2)
		INTO :vid_no,:vcontract_no,:vcust_id,:vsm_code,
	  :vuser_passwd,:vrun_code,
	  :vcard_type, :vgrpbig_flag,:vBelongCode,:vregion_code
	  FROM dCustMsg
	  WHERE phone_no=:vphone_no AND substr(run_code,2,1)<'a';
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1207","������ʾδ����");
		return 1207;
	}
	
	/*��ѯdCustDoc��*/
	EXEC SQL	SELECT cust_name,owner_grade,owner_type,cust_address,
						id_type,id_iccid
				  INTO :vcust_name,:vowner_grade,:vowner_type,:vcust_address,
						  :vid_type,:vid_iccid
				  FROM dCustDoc
				 WHERE cust_id=to_number(:vcust_id);
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","������ʾδ����");
		return 1208;
	}

	/*��ѯsSmCode��*/
	EXEC SQL	SELECT sm_name
				  INTO :vsm_name
				  FROM sSmCode
				 WHERE region_code=:vregion_code
				   AND sm_code=:vsm_code;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1208","������ʾδ����");
		return 1209;
	}
	
	/*��ѯsRunCode��*/
	EXEC SQL	SELECT run_name
				  INTO :vrun_name
				  FROM sRunCode
				 WHERE region_code=:vregion_code
				   AND run_code=:vrun_code;

	/*��ѯsCustGradeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vgrade_name
	  			  FROM sCustGradeCode
	  			 WHERE region_code=:vregion_code
	  			   and owner_code=:vowner_grade;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1210","������ʾδ����");
		return 1210;
	}
	
	/*��ѯsCustTypeCode��*/
	EXEC SQL	SELECT type_name
				  INTO :vtype_name
				  FROM sCustTypeCode
				 WHERE type_code=:vowner_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1211","������ʾδ����");
		return 1211;
	}

	/*��ѯsIdTtype��*/
	EXEC SQL	SELECT id_name
				  INTO :vid_name
				  FROM sIdType
				 WHERE id_type=:vid_type;
	if(SQLCODE!=0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1212","������ʾδ����");
		return 1212;
	}

	/* ��ѯ sBigCardCode */
	EXEC SQL select card_name into :vcard_name
		from  sBigCardCode
		where card_type = :vcard_type;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1213","������ʾδ����");
		return 1213;
	}

	/* ��ѯ sGrpBigFlag */
	EXEC SQL select grp_name into :vgrpbig_name
		from  sGrpBigFlag
		where grp_flag = :vgrpbig_flag;
	if(SQLCODE != 0)
	{	
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","1214","������ʾδ����");
		return 1214;
	}

	/*�ж��Ƿ���Ҫ���⴦��*/
	vret = fCheckExtraRule("0001","op_code", vop_code, sothervalue,  retMsg);
	if(vret!=0)
	{
		vret=0;
		/*�ʻ���Ƿ�Ѻ�Ԥ��*/
		vret=fGetUserOwe(vid_no,&user_base->user_owe);
		if(vret!=0)
		{	
			pubLog_DBErr(_FFL_,"","-1","ȡ�û�Ƿ����Ϣ����");
			return -1;
		}    
	}
	else
	{
		EXEC SQL 	select  prepay_fee
					into :vTotalPrepay
				  	from dConMsg 
					where Contract_No=to_number(:vcontract_no);
		if(SQLCODE!=0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","1215","��ѯԤ�����");
			return 1215;
		}
		
		printf("vTotalPrepay=[%f],vcontract_no=[%s]\n",vTotalPrepay,vcontract_no);
		user_base->user_owe.totalOwe = 0;
		user_base->user_owe.totalPrepay = vTotalPrepay;
		user_base->user_owe.unbillFee = 0;
		strcpy(user_base->user_owe.accountNo, vcontract_no); 
		user_base->user_owe.accountOwe = 0;
	}
	/*��ֵ*/
	Trim(vid_no); strcpy(user_base->id_no,vid_no);
	Trim(vcust_id); strcpy(user_base->cust_id,vcust_id);
	Trim(vBelongCode); strcpy(user_base->belong_code,vBelongCode);
	Trim(vsm_code); strcpy(user_base->sm_code,vsm_code);
	Trim(vsm_name); strcpy(user_base->sm_name,vsm_name);
	Trim(vuser_passwd); strcpy(user_base->cust_name,vcust_name);
	Trim(vuser_passwd); strcpy(user_base->user_passwd,vuser_passwd);
	Trim(vrun_code); strcpy(user_base->run_code,vrun_code);
	Trim(vrun_name); strcpy(user_base->run_name,vrun_name);
	Trim(vowner_grade); strcpy(user_base->owner_grade,vowner_grade);
	Trim(vgrade_name); strcpy(user_base->grade_name,vgrade_name);

	Trim(vowner_type); strcpy(user_base->owner_type,vowner_type);
	Trim(vtype_name); strcpy(user_base->type_name,vtype_name);

	Trim(vcust_address); strcpy(user_base->cust_address,vcust_address);
	Trim(vid_type); strcpy(user_base->id_type,vid_type);
	Trim(vid_name); strcpy(user_base->id_name,vid_name);
	Trim(vid_iccid); strcpy(user_base->id_iccid,vid_iccid);

	Trim(vcard_type); strcpy(user_base->card_type,vcard_type);
	Trim(vcard_name); strcpy(user_base->card_name,vcard_name);
	Trim(vgrpbig_flag); strcpy(user_base->grpbig_flag,vgrpbig_flag);
	Trim(vgrpbig_name); strcpy(user_base->grpbig_name,vgrpbig_name);

	return 0;
}

/************************************************************************
��������: fCheckExtraRule()
����ʱ��: 2009/09/21
������Ա: zhangzhe
��������: �ж�ҵ����������Ƿ����
�������: ��������,�����ֶ�,�������
�������: ��������ֶ�,oErrMsg
�� �� ֵ: 0 ������ȷ����, ��������!
ע�⣺�������͵�˵����sRuleType
************************************************************************/
int fCheckExtraRule(const char* ruletype, const char* rulecode, const char* rulevalue, char *othervalue, char *oErrMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char sruletype[4+1];
		char srulecode[40+1];
		char srulevalue[40+1];
		char sothervalue[255+1];
		int	 iRet=0;
	EXEC SQL END DECLARE SECTION;
	
	init(sruletype);
	init(srulecode);
	init(srulevalue);
	init(sothervalue);
	
	strcpy(sruletype,ruletype);
	strcpy(srulecode,rulecode);
	strcpy(srulevalue,rulevalue);
	
	pubLog_Debug(_FFL_,"","","ruletype = [%s]", Trim(sruletype));
	pubLog_Debug(_FFL_,"","","rulecode = [%s]", Trim(srulecode));
	pubLog_Debug(_FFL_,"","","rulevalue = [%s]", Trim(srulevalue));
	
	EXEC SQL 	SELECT 	OTHER_VALUE 
				INTO	:sothervalue
				FROM   	cExtraRule 
				WHERE 	RULE_TYPE=:sruletype
				AND		RULE_CODE=:srulecode
				AND		RULE_VALUE=:srulevalue;
	if ( SQLCODE != 0 &&  SQLCODE != 1403)
	{
		sprintf(oErrMsg, "��������ҵ������ʧ��! SQLCODE = [%d]",SQLCODE);	
		return -1;
	}
	else if( SQLCODE == 1403)
	{
		return 1403;
	}
	strcpy(othervalue,sothervalue);
	return 0;	 
}

/************************************************************************
��������: fGetDBConnect()
����ʱ��: 2009/11/10
������Ա: zhengxg
��������: �жϵ�ǰ���ӵ����ݿ��ǩ
�������: ����Ӧ�����ݿ��ǩ,�����������ݿ��ǩ
�������: ���ص����ݿ��ǩ
�� �� ֵ: 0 ������ȷ����, ��������!
************************************************************************/
int fGetDBConnect(const char* strConEMDBLabel, const char* strConRunDBLabel, char* dbconnect_name)
{
	char cTmp[1 + 1];
	char vConEMDBLabel[40 + 1];
	char vConRunDBLabel[40 + 1];
	init(cTmp);
	init(vConEMDBLabel);
	init(vConRunDBLabel);
	init(dbconnect_name);
	
	strcpy(vConEMDBLabel, strConEMDBLabel);
	strcpy(vConRunDBLabel, strConRunDBLabel);
	strcpy(dbconnect_name, vConRunDBLabel);
    if(0 == spublicDBLogin(NULL,NULL,"",vConEMDBLabel))
   	{
   		EXEC SQL SELECT LOGINDB_FLAG into :cTmp FROM CSETDBCHANGE WHERE rownum < 2;
   		if(0 == SQLCODE && 0 == strncmp(cTmp, "1", 1))
   		{
   			strcpy(dbconnect_name, vConEMDBLabel);
   		}
   	}
   	Trim(dbconnect_name);
   	if(0 != strcmp(dbconnect_name, vConEMDBLabel))
   	{
	   	spublicDBClose(vConEMDBLabel);
	   	if(0 != spublicDBLogin(NULL,NULL,"",dbconnect_name))
	   	{
	   		return -1;
	   	}
   	}
    return 0;
}

/************************************************************************
��������: unencrypt()
����ʱ��: 2010/06/29
������Ա: liuweib
��������: �û����˻����ͻ�����ӽ���
�������: 
�������: 
�� �� ֵ: 0 �������, 1 ����
************************************************************************/
int unencrypt(char * vPassWd,char * vEnPassWd, char * key, char *flag )
{
	char vKey[93+1];
	char vFlag[1+1];
	char vPassWord[8+1];
	char vEnPassWord[8+1];
	char *p=NULL;
	char *q=NULL;
	char i=0,j=0,k=0,len=0,size=0;
	
	init(vKey);
	init(vFlag);
	init(vPassWord);
	init(vEnPassWord);
	strcpy(vKey,key);
	strcpy(vPassWord,vPassWd);
	strcpy(vFlag,flag);
	Trim(vFlag);
	Trim(vPassWord);
	Trim(vKey);
	
	p=vKey;
	len = strlen(vPassWord);
	printf("p=[%s]\nvKey=[%s]\nflag=[%s]\nlen=[%d]\n",p,vKey,vFlag,len);
	printf("vPassWord=[%s]\n",vPassWord);
	if(len <= 0)
	{
		return -2;
	}
	
	if(strcmp(vFlag,"0")==0)
	{
		for(i=0;i<len;i++)
		{	
			size =((int)vPassWord[i]-48+i+1)*(len/2+1);
			vEnPassWord[i] = vKey[size];
			printf("vEnPassWord[%d]=[%c]\n",i,vEnPassWord[i]);
		}
		vEnPassWord[len]='\0';
		strcpy(vEnPassWd,vEnPassWord);
		Trim(vEnPassWd);
		printf("vEnPassWord=[%s]\n",vEnPassWord);
	}
	else if(strcmp(vFlag,"1")==0)
	{
		for(i=0;i<len;i++)
		{	
			q=strchr(p,vPassWord[i]);
			if(q)
			{
				k=q-p;
			}
			else if(!q)
			{
				return -3;
			}
			vEnPassWord[i]=k/(len/2+1)-i-1+48;
			printf("vEnPassWord[%d]=[%c]\n",i,vEnPassWord[i]);
		}
		vEnPassWord[len]='\0';
		strcpy(vEnPassWd,vEnPassWord);
		Trim(vEnPassWd);
		printf("vEnPassWord=[%s]\n",vEnPassWord);
	}
	else
	{
		return -4;
	}
	
	return 0;
}

int QueryRemainFee(char *service_name ,char *iPhoneNo ,char *iOpCode,char *iFlag,char *iNeedPrePay,char *return_code,char *return_msg,char *vAllowFlag)
{
	int			input_par_num = 0;	/*�����������*/
	int			output_par_num = 0;	/*�����������*/
	int			i=0,j=0,k=0,ret=0;
	FBFR32		*transIN = NULL;	/*���뻺����*/
	FBFR32		*transOUT = NULL;	/*���������*/
	FLDLEN32	len;				/*�������������Ŀռ�*/
	char		temp_buf[1000];		/*��ʱʹ�õ��ַ�������*/

	/**��ȡǷ����Ϣ**/
	FBFR32    *sendbuf = NULL;
	FBFR32 	  *rcvbuf = NULL;
	FLDLEN32  sendlen,rcvlen;
	long      ilen=0, olen=0;
	int       RetValue=0;
	int 	  sendNum=0;
	int 	  recvNum=0;	

	EXEC SQL	BEGIN DECLARE SECTION;
	/* ������� */
	char 	  buffer1[256];
	char	  buffer2[256];
	char	  vChgFlag[1+1];
	/**���ʷ���Ϣ**/
	char        vModeName[30+1];
	int 		iCount=0;
	char        vRegionCode[2+1];

	EXEC SQL    END DECLARE SECTION;

	/**��ʼ���û�������Ϣ�ṹ��**/
	init(buffer1);
	init(buffer2);
	init(vChgFlag);
	init(vModeName);
	init(vRegionCode);


	/**��ȡ�û�ʣ����**/
	sendNum = 4;
	recvNum = 3;

	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		strcpy(return_code,"873016");
		strcpy(return_msg,"��ʼ�����������ʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_msg);

	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"4",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);

	Fchg32(sendbuf,GPARM32_0,0,iPhoneNo,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_1,0,iOpCode,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_2,0,iFlag,(FLDLEN32)0);
	Fchg32(sendbuf,GPARM32_3,0,iNeedPrePay,(FLDLEN32)0);
	
	
	printf("in GPARM32_0=[%s]\n",iPhoneNo);
	printf("in GPARM32_1=[%s]\n",iOpCode);
	printf("in GPARM32_2=[%s]\n",iFlag);
	printf("in GPARM32_3=[%s]\n",iNeedPrePay);
	
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);

	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		strcpy(return_code,"873017");
		strcpy(return_msg,"��ʼ�����������ʧ��!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_msg);
		return  1;
	}

	long reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	RetValue = 0;
	RetValue = fPubCallSrv(CALLSERV_TP,service_name,sendbuf,&rcvbuf);
	if(RetValue != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		strcpy(return_code,"873018");
		strcpy(return_msg,"call bs_QryRemainFee failed!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_, service_name, "", "v_ret=[%d]!",RetValue);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_msg);
		return  1;
	}

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));

		Fget32(rcvbuf,SVC_ERR_NO32,0,buffer1,(FLDLEN32)0);
	 	Fget32(rcvbuf,SVC_ERR_MSG32,0,buffer2,(FLDLEN32)0);

	  	Trim(buffer1);
		Trim(buffer2);
		pubLog_Debug(_FFL_,service_name,"","buffer1=[%s]",buffer1);
		pubLog_Debug(_FFL_,service_name,"","buffer2=[%s]",buffer2);
	if(strcmp(buffer1,"000000") != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		strcpy(return_code,"873019");
		strcpy(return_msg,"call bs_QryRemainFee failed!");
		PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_, service_name, "", "buffer1=[%s]!",buffer1);
		pubLog_DBErr(_FFL_, service_name, "%s", "%s", return_code, return_msg);
		return  1;
	}

  	Fget32(rcvbuf,GPARM32_2,0,vChgFlag,(FLDLEN32)0);

  	Trim(vChgFlag);
  	
  	sprintf(vAllowFlag,"%s",vChgFlag);

  	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);		

    pubLog_Debug(_FFL_, service_name, "", "����ִ�н���");


	return  0;
}
int fAddUsedDataFuncs(tFuncEnv *funcEnv, char *funcs,double iRemainFee,double *olimitFee,char *funcode)
{
	FBFR32 *transIN = NULL, *transOUT = NULL;
	char * const temp_buf=funcEnv->temp_buf;

	char *p, *q;
	char end_date[8+1];
	int  hasExpire = 0; /*�Ƿ��е���ʱ��*/
	int  iRetCode = 0;
	int  num=0;
	int  iCount=0;
	int  iCountFav=0;
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024];    
	char function_code[2+1];

	char	idNo[22 + 1];
	int   ret =0;
	char	totalDate[8+1];
	char	opTime[17+1];
	char	opCode[4+1];
	char	loginNo[6+1];
	char	loginAccept[22+1];
	char	belongCode[7+1];
	char	smCode[2+1];
	char vOrgId[10+1];
	char	phoneNo[15+1];
	char	orgCode[9+1];
	double  vLimitPay=0;
	double  vRemainFee=0;
	char	vLimiName[20+1];
	char	sqlStrfunc[512+1];
	char	delFuncStr[3+1];
	char	retCode[6+1];
	char	outtime[17+1];
	long	rspno =0;
	long	tLoginAccept=0;
	int iCountBind=0;
	TdCustFunc	sTdCustFunc;
	TdCustFuncAdd	sTdCustFuncAdd;
	TdCustFuncAddHis	sTdCustFuncAddHis;
	TdCustFuncHis	sTdCustFuncHis;
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vLimiName);
	Sinitn(totalDate);
	Sinitn(opTime);
	Sinitn(opCode);
	Sinitn(loginNo);
	Sinitn(loginAccept);
	Sinitn(belongCode);
	Sinitn(smCode);
	Sinitn(phoneNo);
	Sinitn(vOrgId);
	Sinitn(orgCode);
	Sinitn(delFuncStr);
	Sinitn(sqlStrfunc);
	Sinitn(retCode);
	Sinitn(outtime);
	
	strcpy(idNo, funcEnv->idNo);
	strcpy(totalDate, funcEnv->totalDate);
	strcpy(opTime, funcEnv->opTime);
	strcpy(opCode, funcEnv->opCode);
	strcpy(loginNo, funcEnv->loginNo);
	strcpy(loginAccept, funcEnv->loginAccept);
	strcpy(belongCode, funcEnv->belongCode);
	strcpy(smCode, funcEnv->smCode);
	strcpy(phoneNo, funcEnv->phoneNo);
	strcpy(orgCode, funcEnv->orgCode);
	
	vRemainFee=iRemainFee;
	
	transIN=funcEnv->transIN;
	transOUT=funcEnv->transOUT;
	
	srand(time(NULL));
	
	EXEC SQL select to_char(sysdate+70/(24*60),'yyyymmdd HH24:MI:SS') into :outtime from dual;

	p = funcs;
	while (p != 0)
	{
		q = (char *)strchr(p, (int)('|'));
		if (q == (char *)0) break;
		strncpy(function_code, p, 2);
		function_code[2] = '\0';
		if (q - p == 10)
		{
			strncpy(end_date, p+2, 8);
			end_date[8] = '\0';
			hasExpire = 1;
		}
		else if (q - p == 2)
		{
			hasExpire = 0;
		}
		else
		{	
			pubLog_DBErr(_FFL_,"","121920","");
			return 121920;
		}

		p = q + 1;
        /*printf("function_code = [%s]\n", function_code);
        */
        pubLog_Debug(_FFL_,"","","function_code = [%s]", function_code);
		iRetCode = checkFuncCode(idNo,'a',belongCode,function_code,smCode);
		if(iRetCode != 0) return iRetCode;
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 begin*/	
		if(strcmp(opCode,"1219") == 0)
		{   
			 
			printf("function_code=%s\n",function_code);     
			
			init(dynStmt);
			sprintf(dynStmt,	"SELECT count(*) "
								"  FROM dBillCustDetail%c a, sBillFuncBind b"
								" WHERE a.DETAIL_CODE = b.FUNCTION_BIND"
								"   AND a.DETAIL_TYPE = 'a'"
								"   AND b.function_code = :v0 "
								"   AND b.region_code = substr(:v1, 1,2)"
								"   AND begin_time <= sysdate"
								"   AND end_time > sysdate"
								"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
								,idNo[strlen(idNo)-1]);
			EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :iCountBind using :function_code,:belongCode,:idNo;
			END;
			END-EXEC;
			if(SQLCODE != 0 && SQLCODE != 1403)    
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ�ײͰ󶨵��ط�ʧ��!SQLCODE=%d",SQLCODE);
				return 121931;
			}
			 /* 2011-3-2  �޸��ײͰ󶨺���Զ������ط����п�ͨ*/	
		/*	if ( (iCountBind > 0 ) && (strcmp(function_code,"55")!=0))
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","���ײͰ󶨵��ط���ͻ�����ܽ��ж��ط�������б��!");
				return 121932;
			
			}    */                                                 
        	EXEC SQL select count(*) into :iCount from SFUNCSENDMSG where region_code=substr(:belongCode,1,2) 
        		AND sm_code = :smCode and function_code=:function_code;	
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121921","��ѯ SFUNCSENDMSG ʧ��!SQLCODE=%d",SQLCODE);
				return 121933;
			}
			printf("iCount=%d\n",iCount);
			if((iCountBind==0)&&(iCount>0))
			{   
				rspno=0;
				rspno=rand()%900000+100000;
				
				Sinitn(dynStmt);     
				sprintf(dynStmt,	"SELECT  count(*) "
						"  FROM dBillCustDetail%c a, sBillFuncFav b"
						" WHERE a.DETAIL_CODE = b.FUNCTION_BILL"
						"   AND a.DETAIL_TYPE = '4'"
						"   AND b.function_code = :v0 "
						"   AND b.region_code = substr(:v1, 1,2)"
						"   AND begin_time <= sysdate"
						"   AND end_time > sysdate"
						"   AND a.id_no = to_number(:v2) and a.mode_status='Y'"
						,idNo[strlen(idNo)-1]);
				EXEC SQL EXECUTE
				BEGIN
					EXECUTE IMMEDIATE :dynStmt INTO :iCountFav using :function_code,:belongCode,:idNo;
				END;
				END-EXEC;
				if(SQLCODE != 0 && SQLCODE != 1403)    
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121922","ȡ�ط��Żݴ���!SQLCODE=%d",SQLCODE);
					return 121922;
				}
				printf("---6915��-rspno[%ld][%s]---------\n",rspno,phoneNo);
				if(iCountFav==0)
				{  
					/*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� begin*/
					tLoginAccept=0;
			        EXEC SQL select sMaxSysAccept.nextval
			        		   into :tLoginAccept
			        		   from dual;
				    if(SQLCODE!=0)
				    {
				        printf("\nȡ��ˮ[%s][%d]",SQLERRMSG,SQLCODE);
				        return -3;
				    }
				    printf("----rspno[%ld][%ld][%s]---------\n",rspno,tLoginAccept,phoneNo);
				    /*majha edit at 20100930 �������ȡ��ˮ��һ���޷��շ����� end*/
				   	Sinitn(dynStmt);     
					sprintf(dynStmt,
						"INSERT INTO dCrmRemindMsg "
						"("
						"	MAXACCEPT, ID_NO , PHONE_NO, BIZTYPE,	SPID,BIZCODE,BIZNAME,"
						"	RSP_NO , OP_TIME, SERV_PROPERTY,TOTAL_DATE,SEND_TIME,OUT_TIME,CONFIRM_FLAG "
						")"
						"select"
						"  :v1,	to_number(:v2),	:v3, '00',"
						"  FUNCTION_TYPE,:v4,FUNCTION_NAME,:v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),'3', :v7,"
						"  to_date(:v8, 'yyyymmdd hh24:mi:ss'),to_date(:v9, 'yyyymmdd hh24:mi:ss'),'0'"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v10,1,2)"
						"   AND sm_code = :v11"
						"   AND function_code = :v12"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :tLoginAccept,:idNo, :phoneNo,:function_code,:rspno,
						 :opTime,:totalDate,:opTime,:outtime,:belongCode,:smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121924","�����dCrmRemindMsgʧ��[%d]!SQLCODE=%d",rspno,SQLCODE);
						return 121924;
					}
					
					#ifdef _CHGTABLE_
					Sinitn(dynStmt);     
					sprintf(dynStmt, 
						"INSERT INTO wfuncremindchg "
						"("
						"	OP_NO,		ID_NO,		PHONE_NO,	MAXACCEPT, OP_TYPE,"
						"	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME, FEE_FLAG,FLAG_TIME "
						")"
						"select"
						"  sMaxSysAccept.nextval,	to_number(:v2),	:v3, :v4, 'i',"
						"	FUNCTION_TYPE, :v5,	to_date(:v6, 'yyyymmdd hh24:mi:ss'),0,to_date(:v7, 'yyyymmdd hh24:mi:ss')"
						"  FROM SFUNCSENDMSG "
						" WHERE region_code=substr(:v8,1,2)"
						"   AND sm_code = :v9"
						"   AND function_code = :v10"
						);
					exec sql execute
					begin
						EXECUTE IMMEDIATE :dynStmt USING :idNo, :phoneNo,:tLoginAccept,
						:function_code, :opTime,:opTime,:belongCode, :smCode,:function_code;
					end;
					end-exec;
					if (SQLCODE != 0)
					{	
						PUBLOG_DBDEBUG("");
						pubLog_DBErr(_FFL_,"","121925","�����wfuncremindchgʧ��!SQLCODE=%d",SQLCODE);
						return 121925;
					}
					#endif
				}
			}
			
		}
		/* ����ȷ�Ϲ��� add by zhaohx 2020/06/15 end*/	
		
		/*--��֯�������������ֶ�edit by liuweib at 19/02/2009--begin*/
       
       ret = sGetLoginOrgid(loginNo,vOrgId);
       if(ret <0)
         	{
         	  /*printf("��ȡ�û�org_idʧ��!\n");
         	  */
				pubLog_DBErr(_FFL_,"","200919","��ȡ�û�org_idʧ��!");
         	  	return 200919; 
         	}
       Trim(vOrgId);
      /*---��֯�������������ֶ�edit by liuweib at 19/02/2009---end*/
		/*add by zwm ��������*/
		if((strcmp(function_code,"15") == 0) || (strcmp(function_code,"16") == 0) || (strcmp(function_code,"17") == 0) || (strcmp(function_code,"23") == 0) || (strcmp(function_code,"29") == 0))
		{
			Trim(idNo);
			Sinitn(sqlStrfunc);
			sprintf(sqlStrfunc,"select function_code||'|' FROM dcustfunc WHERE id_no=to_number(:v1) AND function_code in ('15','16','17','23','29')");
			EXEC SQL PREPARE SqlStmting FROM :sqlStrfunc;
			EXEC SQL DECLARE func_curing1 CURSOR FOR SqlStmting;
			EXEC SQL OPEN func_curing1 using :idNo;
			for(;;)
			{
				Sinitn(delFuncStr);
				EXEC SQL FETCH func_curing1 INTO :delFuncStr;
				if(SQLCODE == 1403) break;
				if(SQLCODE != 0 && SQLCODE != 1403)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","201005","SELECT dcustfunc ERROR!");
					return 201005;
				}
				
				sprintf(retCode, "%06d", fDelFuncs(funcEnv, delFuncStr));
				if ( strcmp(retCode, "000000") != 0)
				{
					PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","201006","ȡ���ط�����!");
					return 201006;
				}
			}
			EXEC SQL CLOSE func_curing1;
		}
		Sinitn(dynStmt);     
		sprintf(dynStmt,
			"INSERT INTO wCustFuncDay"
			"("
			"	ID_NO,		FUNCTION_CODE,		ADD_FLAG,	TOTAL_DATE,	OP_TIME,"
			"	OP_CODE,	LOGIN_NO,			LOGIN_ACCEPT"
			")"
			"VALUES"
			"("
			"to_number(:v1),	:v2,	'Y',	to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'),"
			"	:v5,	:v6,		to_number(:v7)"
			")");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo, :function_code, :totalDate, :opTime,
			:opCode, :loginNo,	:loginAccept;
		end;
		end-exec;
		if (SQLCODE != 0)
		{	
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121914","��ȡ�û���Ϣʧ��!");
			return 121914;
		}
		
		EXEC SQL select count(*) into :num from dconmsg 
						where contract_no in(select contract_no from  dconusermsg where id_no=:idNo and serial_no=0)
	   				and pay_code='4';
	  if(num>0&&(strcmp(function_code,"00")==0||strcmp(function_code,"17")==0))
	  	/*printf("This is a specular user,vRemainFee=[%f]\n",vRemainFee);
	  	*/
	  	pubLog_Debug(_FFL_,"","","This is a specular user,vRemainFee=[%f]",vRemainFee);
	  else
	  {
		Sinitn(vLimiName);
		EXEC SQL select nvl(limit_prefee,0),function_name
			into :vLimitPay,:vLimiName
			from sFuncList
			 WHERE region_code=substr(:belongCode,1,2)
			   AND sm_code = :smCode
			   AND function_code = :function_code;
		if(SQLCODE!=0)
		{
			/*printf("121957 SqlCode =[%d]\n", SQLCODE);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121957", "SqlCode =[%d]", SQLCODE);
			return 121957;
		}
		if(vLimitPay>vRemainFee)
		{
			/*printf("121958 vLimitPay  =[%f]\n", vLimitPay);
			printf("121958 vRemainFee =[%f]\n", vRemainFee);
			*/
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121958", "121958 vLimitPay  =[%f],vRemainFee =[%f]",vLimitPay,vRemainFee);
			sprintf(funcode,"[%s]%s",function_code,vLimiName);
			*olimitFee=vLimitPay;
			return 121958;
		}
	}
		/*ng���� by zhaohx at 04/08/2009 begin*/
		/*sprintf(dynStmt,
			"INSERT INTO dCustFunc"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_TYPE,	FUNCTION_CODE,	to_date(:v2, 'yyyymmdd hh24:mi:ss')"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v3,1,2)"
			"   AND sm_code = :v4"
			"   AND function_code = :v5"
			);
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:belongCode, :smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
		}*/
		Trim(idNo);
		
		EXEC SQL select :idNo,	FUNCTION_TYPE,	FUNCTION_CODE,	:opTime into :sTdCustFunc 
	 	  FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code ;
		if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121974","��ѯ�ط���Ϣ����");
			return 121974;		
		}
		ret = 0; 		
	  ret=OrderInsertCustFunc(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedDataFuncs",sTdCustFunc);
		if (ret!=0)
	  {    				  
	  	PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121915","ȡ�û���Ϣ����");
			return 121915;
	  }

		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	OP_TIME,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFunc WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
			:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
		}
    */
    Trim(idNo);
    EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE,	to_char(OP_TIME,'yyyymmdd hh24:mi:ss'),
		    :loginNo,	:loginAccept, :totalDate, :opTime, :opCode, 'a'
		    into :sTdCustFuncHis from dCustFunc WHERE id_no = to_number(:idNo) and function_code = :function_code ;
	  if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;		
		}
		ret = 0; 					
  	ret=OrderInsertCustFuncHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedDataFuncs",sTdCustFuncHis);
		if (ret != 0)
  	{    				  
  	  PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121916","ȡ�û���Ϣ����");
			return 121916;
  	}
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAdd"
			"("
			"	ID_NO,	FUNCTION_CODE,	FUNCTION_TYPE,	BEGIN_TIME,"
			"	BILL_TIME,	FAVOUR_MONTH,	ADD_NO,	OTHER_ATTR"
			")"
			"SELECT"
			"	to_number(:v1),	FUNCTION_CODE,	FUNCTION_TYPE,	to_date(:v2, 'yyyymmdd hh24:mi:ss'), "
			"add_months(to_date(:v3, 'yyyymmdd hh24:mi:ss'), month_limit), MONTH_LIMIT, NULL, NULL"
			"  FROM sFuncList"
			" WHERE region_code=substr(:v4,1,2)"
			"   AND sm_code = :v5"
			"   AND function_code = :v6"
			"   AND month_limit > 0");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :idNo,	:opTime,	:opTime,	:belongCode,
			:smCode, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
			return 121917;
		}*/
		
		Trim(idNo);
		Trim(opTime);
		Trim(belongCode);
		Trim(smCode);
		Trim(function_code);
		EXEC SQL select :idNo,	FUNCTION_CODE,	FUNCTION_TYPE,	:opTime,
			 to_char(add_months(to_date(:opTime, 'yyyymmdd hh24:mi:ss'), month_limit),'yyyymmdd hh24:mi:ss'), to_char(MONTH_LIMIT), chr(0), chr(0) into :sTdCustFuncAdd 
			 FROM sFuncList WHERE region_code=substr(:belongCode,1,2) AND sm_code = :smCode AND function_code = :function_code 
			 AND month_limit > 0;
		if(SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121978","��ѯ�ط���Ϣ����");
			return 121978;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 	
			ret=OrderInsertCustFuncAdd(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedDataFuncs",sTdCustFuncAdd);
			if (ret < 0)
	    	{    				  
	      	PUBLOG_DBDEBUG("");
					pubLog_DBErr(_FFL_,"","121917","ȡ�û���Ϣ����");
					return 121917;
	    	}
	    }
		/*sprintf(dynStmt,
			"INSERT INTO dCustFuncAddHis"
			"("
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	UPDATE_LOGIN,	UPDATE_ACCEPT,	UPDATE_DATE,	UPDATE_TIME,	UPDATE_CODE,	UPDATE_TYPE"
			")"
			"SELECT"
			"	ID_NO,	FUNCTION_TYPE,	FUNCTION_CODE,	BEGIN_TIME,BILL_TIME,FAVOUR_MONTH,ADD_NO,OTHER_ATTR,"
			"	:v1,	to_number(:v2),		to_number(:v3), to_date(:v4, 'yyyymmdd hh24:mi:ss'), :v5, 'a'"
			"  FROM dCustFuncAdd WHERE id_no = to_number(:v6) and function_code = :v7");
		exec sql execute
		begin
			EXECUTE IMMEDIATE :dynStmt USING :loginNo,	:loginAccept,	:totalDate, :opTime,	:opCode,
						:idNo, :function_code;
		end;
		end-exec;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			//printf("121918 SqlCode = [%d]\n", SQLCODE);
			
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
			return 121918;
		}*/
		Trim(idNo);
		EXEC SQL select to_char(ID_NO),	FUNCTION_TYPE,	FUNCTION_CODE, nvl(to_char(BEGIN_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),
		   nvl(to_char(BILL_TIME,'yyyymmdd hh24:mi:ss'),chr(0)),nvl(to_char(FAVOUR_MONTH),chr(0)),
			 nvl(ADD_NO,chr(0)),nvl(OTHER_ATTR,chr(0)),:loginNo,:loginAccept,:totalDate, :opTime, :opCode, 'a'
			 into :sTdCustFuncAddHis FROM dCustFuncAdd WHERE id_no = to_number(:idNo) and function_code = :function_code;
		if (SQLCODE != 0  && SQLCODE != 1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
			return 121918;		
		}
		if (SQLCODE != 1403)
		{
			ret = 0; 		 		 	
			ret=OrderInsertCustFuncAddHis(ORDERIDTYPE_USER,idNo,100,opCode,atol(loginAccept),loginNo,"fAddUsedDataFuncs",sTdCustFuncAddHis);
			if (ret < 0)
	  	{    			
	  	  PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121918","121918 SqlCode = [%d]", SQLCODE);
				return 121918;
	  	}
	    }
    	/*ng���� by zhaohx at 04/08/2009 end*/
		if (hasExpire)
		{ /*��֯�������������ֶ�edit by wanfh at 19��02��2009*/
            Sinitn(dynStmt);        
			sprintf(dynStmt,
				"INSERT INTO wCustExpire"
				"("
				"	COMMAND_ID,	SM_CODE,	ID_NO,	PHONE_NO,"
				"	ORG_CODE,ORG_ID,	LOGIN_NO,	LOGIN_ACCEPT,"
				"	TOTAL_DATE,	OP_CODE,	OP_TIME,	FUNCTION_CODE,	COMMAND_CODE,"
				"	BUSINESS_STATUS,	EXPIRE_TIME,	OP_NOTE"
				")"
				" SELECT"
				"	sOffon.nextval,	:v1,	to_number(:v2),	:v3,"
				"	:v4,:vOrgId,	:v5,	to_number(:v6),"
				"	to_number(:v7),	:v8,	to_date(:v9, 'yyyymmdd hh24:mi:ss'),	:v10,	COMMAND_CODE,"
				"	'0',	to_date(:v11, 'yyyymmdd'),	'�ط�����ͣ��'"
				"  FROM sFuncList"
				" where region_code=substr(:v12, 1,2)"
				"   and sm_code = :v13"
				"   and function_code = :v14");
			exec sql execute
			begin
				EXECUTE IMMEDIATE :dynStmt USING :smCode,	:idNo,	:phoneNo,
								:orgCode,:vOrgId,	:loginNo,	:loginAccept,
								:totalDate, :opCode,	:opTime, :function_code,
								:end_date, 
								:belongCode, :smCode, :function_code;
			end;
			end-exec;
			if (SQLCODE != 0)
			{
				/*printf("121919 SqlCode = [%d]\n", SQLCODE);
				*/
				PUBLOG_DBDEBUG("");
				pubLog_DBErr(_FFL_,"","121919","121919 SqlCode = [%d]", SQLCODE);
				return 121919;
			}

		}
		iRetCode = fSendCmd(funcEnv, '1', "1219", function_code);
		if(iRetCode != 0)
		{	
			pubLog_DBErr(_FFL_,"","%d","fsencmdʧ��",iRetCode);
			return iRetCode;
		}
/*��������Ͷ�ߵ����� �ڿ�ͨ�������ε�ʱ�� ��Ҫ���͹���ֱ����ָ�� �Է�ֹ������ƽ̨���ݲ�һ�� add by cencm---begin*/
		if(strcmp(function_code,"17")==0)
		{
			iRetCode=0;
			iRetCode = fSendCmd(funcEnv, '1', "1219", "00");
			if(iRetCode != 0)
			{	
				pubLog_DBErr(_FFL_,"","%d","fsencmdʧ��",iRetCode);
				return iRetCode;
			}
		}
/*��������Ͷ�ߵ����� �ڿ�ͨ�������ε�ʱ�� ��Ҫ���͹���ֱ����ָ�� �Է�ֹ������ƽ̨���ݲ�һ�� add by cencm---end*/
	}
	return 0;
}
/* �ַ����滻���� */
char  *replace(char  *source,  char  *sub,  char  *rep) 
{ 
	char  *result; 
	/*pc1  �Ǹ��Ƶ����result��ɨ��ָ��*/
	/*pc2  ��ɨ�� source �ĸ���ָ��*/
	/*pc3  Ѱ���Ӵ�ʱ,Ϊ���仯�е�source�Ƿ����Ӵ����,��ָ��sub��ɨ��ָ�� */
	/*�ҵ�ƥ���,Ϊ�˸��Ƶ������,��ָ��rep��ɨ��ָ��*/
	char  *pc1,  *pc2,  *pc3;     
	int  isource,  isub,  irep; 
	isub  = strlen(sub);   /*�Ա��ַ����ĳ���*/
	irep  = strlen(rep);   /*�滻�ַ����ĳ���*/
	isource= strlen(source); /*Դ�ַ����ĳ���*/ 
	if(NULL == *sub)  
	return strdup(source); 
	/*����������Ҫ�Ŀռ�*/
	result  = (char *)malloc(( (irep > isub) ? (float)strlen(source) / isub* irep+ 1:isource ) * sizeof(char));
	pc1  =  result; /*Ϊpc1���θ��ƽ������ÿ���ֽ���׼��*/
	while(*source  !=  NULL) 
	{ 
		/*Ϊ���source��sub�Ƿ������׼��,Ϊpc2,pc3 ����ֵ*/
		pc2  =  source; 
		pc3  =  sub;
		/* ��ѭ���ģ���һ�������ǣ� 
		*  *pc2 ������ *pc3 �����Ӵ�����ȣ� 
		*  pc2  ��Դ����β 
		*  pc3  ��Դ����β ����ʱ,�����ȫ���Ӵ�,source����sub��ȣ� 
		*****************************************************/
		while(*pc2  ==  *pc3  &&  *pc3  !=  NULL  &&  *pc2  !=  NULL) 
			pc2++,  pc3++; 
		/* ����ҵ����Ӵ�,�������´�����*/
		if(NULL  ==  *pc3) 
		{ 
			pc3  =  rep; 
			/*�������׷�ӵ������*/
			while(*pc3  !=  NULL) 
			*pc1++  =  *pc3++; 
			pc2--; 
			source  =  pc2; 
			/*   ��� source��sub��ȵ�ѭ�������� 
			* pc2 ��Ӧ��λ������ sub �д���������������Դ������һ��λ�á� 
			* �� source ָ����ǰ��һ���ַ��� 
			***************************************************/
		} 
		else /*���û�ҵ��Ӵ�,���渴��source��ָ���ֽڵ������*/ 
			*pc1++ = *source; 
		source++; /* ��source�����һ���ַ�*/ 
	}     
	*pc1  =  NULL;    
	return  result;
}

/*
��������: pubRetMsg  �������ش���ӳ�䷵����Ϣ����

��ر�:
���ش�����Ϣ���ñ�             SRET_MSG_CFG
oracle ���ش���ӳ���������Ϣ  SORACLE_ERROR
BOSSϵͳ������Ϣ��             SSYSTEM_ERROR
������Ϣ��ʽ���ñ�             SRET_MSG_FORMAT
	BOSSϵͳ������Ϣ  %R 
	Oracle������Ϣ    %S 
	���������Ϣ      %I 
	�������          %C 
	Oracle�������Ϣ  %O 


�������
	Func_code ���ܴ���
	Server_name  ������ 
	RetCode ���ش���
	SQLCODE SQL���ش���
	InputDate ����չʾ��Ϣ
	RetMsg ����������Ϣ
*/
int pubRetMsg(char * Func_code,char *Server_name,char *callPubSever ,char * RetCode,int SqlCode,char *InputDate ,char * RetMsg ) 
{
	
	char RetCodeThis[6+1];
	char RetMsgThis[256+1];
	char SqlMsg[6+1];
	char SqlCodeMsg[90+1];
	char SqlCodeStr[6+1];
	char Format[256+1];
	char Format_type[6+1];
	char *PR=NULL;
	char *PS=NULL;
	char *PI=NULL;
	char *PC=NULL;
	char *PO=NULL;
	init(Format_type);
	init(Format);
	init(SqlCodeMsg);
	init(RetCodeThis);
	init(RetMsgThis);
	init(SqlCodeStr);
	
	Trim(Func_code);
	Trim(Server_name);
	Trim(RetCode);
	Trim(InputDate);
	pubLog_DBErr(_FFL_,"pubRetMsg","","��ʼ���÷��� ");
	printf("  Func_code     [%s] \n",Func_code);
	printf("  Server_name   [%s] \n",Server_name);
	printf("  callPubSever  [%s] \n",callPubSever);
	printf("  RetCode       [%s] \n",RetCode);
	printf("  SqlCode       [%d] \n",SqlCode);
	printf("  InputDate     [%s] \n",InputDate);
	printf("  RetMsg        [%s] \n",RetMsg);
	
	strcmp(RetCodeThis,"000000");
	
	if(strcmp(Func_code ,"") == 0)
	{
		strcmp(RetCodeThis,"000001");
		strcmp(RetMsg,"���ܴ����������,");
	}
	if(strcmp(Server_name ,"") == 0)
	{
		strcmp(RetCodeThis,"000001");
		strcmp(RetMsg,"���ܷ������Ʊ�������,");
	}
	if(strcmp(RetCode ,"") == 0)
	{
		strcmp(RetCodeThis,"000001");
		strcmp(RetMsg,"��������������,");
	}
	if(RetMsg == NULL)
	{
		strcmp(RetCodeThis,"000001");
		strcmp(RetMsg,"������ϢΪ�ձ�������,");
	}
	
	EXEC SQL select  b.retmsg ,a.Format_type
			into :RetMsgThis,:Format_type
			from SRET_MSG_CFG a ,SSYSTEM_ERROR b 
			where b.retcode=a.MsgRetcode
			and a.func_code in (:Func_code ,'0000')
			and a.service_name in (:Server_name ,:callPubSever)
			and a.ret_code=:RetCode;
	if(SQLCODE==NOTFOUND) 
	{
		strcmp(RetCode,"000001");
		pubLog_DBErr(_FFL_,"pubRetMsg","","û�����÷���������Ϣ SERRORINFO " );
		return 0;
	}
	if(SQLCODE!=OK&&SQLCODE!=NOTFOUND)
	{
		pubLog_DBErr(_FFL_,"pubRetMsg"," %d","û�����÷���������Ϣ SERRORINFO " ,SQLCODE);
		strcmp(RetCode,"000001");
	}
	Trim(RetMsgThis);
	Trim(Format_type);
	pubLog_DBErr(_FFL_,"pubRetMsg","%s","" ,RetMsgThis);
	/* ȡSQL������Ϣ */
	if(SqlCode != -1 )
	{
		sprintf(SqlCodeStr,"%05d",SqlCode);
		Trim(SqlCodeStr);
		EXEC SQL  select SQLMSG
			into :SqlCodeMsg
			from SORACLE_ERROR
			where sqlcode =:SqlCodeStr;
		if( SQLCODE != OK && SQLCODE != NOTFOUND)
		{
			strcmp(RetCode,"000001");
			pubLog_DBErr(_FFL_,"pubRetMsg","%s","SORACLE_ERROR",SqlCodeMsg ,SQLCODE);
		}
		printf("Oracle �������  [%s]  ������Ϣ  [%s \n",SqlCodeStr,SqlCodeMsg);
		
	}
	/* ������Ϣ��ʽ�� */

	Trim(RetMsgThis); /* BOSSϵͳ������Ϣ  %R */
	Trim(SqlCodeMsg); /* Oracle������Ϣ    %S */
	Trim(InputDate);  /* ���������Ϣ      %I */
	Trim(RetCode);    /* �������          %C */
	Trim(SqlCodeStr);  /* Oracle�������Ϣ %O */
	init(RetMsg);	
	EXEC SQL select format
			into :Format
			from SRET_MSG_FORMAT
			where format_type=:Format_type;
	if( SQLCODE != OK && SQLCODE != NOTFOUND)
	{
		strcmp(RetCode,"000001");
		pubLog_DBErr(_FFL_,"pubRetMsg","%s","SORACLE_ERROR",SqlCodeMsg ,SQLCODE);
	}
	Trim(Format);  
	printf("Format  [%s] \n",Format);
	PR=strstr(Format,"%R" );
	if(strcmp(RetMsgThis ,"")  !=0   &&  PR != NULL) 
	{
		strcpy(RetMsg,replace(Format,"%R",RetMsgThis));
		strcpy(Format,RetMsg);
		printf( " BOSSϵͳ������Ϣ =====  %s  \n ",RetMsg);
	}
	PS=strstr(Format,"%S" );
	if(strcmp(SqlCodeMsg ,"")  !=0 && PS!=NULL ) 
	{
		strcpy(RetMsg,replace(Format,"%S",SqlCodeMsg));
		strcpy(Format,RetMsg);
		printf( " Oracle������Ϣ =====  %s  \n ",RetMsg);
		
	}
	PI=strstr(Format,"%I" );
	if(strcmp(InputDate ,"")  !=0 && PI!=NULL )  
	{
		strcpy(RetMsg,replace(Format,"%I",InputDate));
		strcpy(Format,RetMsg);
		printf( " ���������Ϣ=====  %s  \n ",RetMsg);
	}
	PC=strstr(Format,"%C" );
	if(strcmp(RetCode ,"")  !=0 && PC!=NULL)  
	{
		strcpy(RetMsg,replace(Format,"%C",RetCode));
		strcpy(Format,RetMsg);
		printf( " ������� =====  %s  \n ",RetMsg);
	}
	PO=strstr(Format,"%O" );
	if(strcmp(SqlCodeStr ,"")  !=0  && PO!=NULL) 
	{
		strcpy(RetMsg,replace(Format,"%O",SqlCodeStr));
		strcpy(Format,RetMsg);
		printf( " Oracle�������Ϣ=====  %s  \n ",RetMsg);
	}
	printf("  RetMsg  [%s] \n",RetMsg);
	pubLog_DBErr(_FFL_,"pubRetMsg","","�������÷��� "); 
	return 0;
}


/*
2011-6-2 
�ǳ�Ĭ�û��û���Ϣ 
�û��޸ģ����ã�У���������������»����޸ĸñ����ݡ�
������û��Ƿǳ�Ĭ�û��û� 
*/
int pubUserWakeUP(char * iId_No,char *iOp_Time,char *iLogin_No) 
{
	char runSQL[1024];
	EXEC SQL BEGIN DECLARE SECTION;
		char vId_No[14+1];
		char vOp_Time[17+1];
		char vLogin_No[6+1];
		int vCount=0;
		char totalDate[8+1];
	EXEC SQL END DECLARE SECTION;
	init(vId_No);
	init(vOp_Time);
	init(vLogin_No);
	init(runSQL);
	init(totalDate);
	Trim(iId_No);
	Trim(iOp_Time);
	Trim(iLogin_No);
	strcpy(vId_No,iId_No);
	strcpy(vOp_Time,iOp_Time);
	strcpy(vLogin_No,iLogin_No);
	
	PUBLOG_APPRUN("pubUserWakeUP"," ","��ʼ���÷��� ");
	PUBLOG_APPRUN("pubUserWakeUP"," ","iId_No [%s]  iOp_Time[%s]  iLogin_No[%s]",iId_No,iOp_Time,iLogin_No);
	EXEC SQL select to_char(sysdate,'yyyymm')
			into :totalDate
			from dual;
	if( SQLCODE != OK && SQLCODE != NOTFOUND)
	{
		PUBLOG_DBERR("pubUserWakeUP"," %d","����ǳ�˯�û��� SERRORINFO " ,SQLCODE);
		return -2;
	}

		sprintf(runSQL,		" Insert into  DCUSTMSGWAKEUP (id_no , phone_no , op_time,login_no,total_date ) "
		" select to_number(a.id_no) ,to_char(a.phone_no),to_date(:vOp_Time,'yyyymmdd hh24:mi:ss'), "
		" to_char(:vLogin_No),to_char(to_date(:vOp_Time),'YYYYMMDD') "
		" from dcustmsg a  "
		" where  a.id_no =to_number(:vId_No) "); 
		PUBLOG_APPRUN("pubUserWakeUP"," ","  erunSQL [%s] ",runSQL);
		EXEC SQL EXECUTE
		BEGIN
			EXECUTE IMMEDIATE :runSQL   using :vOp_Time,:vLogin_No,:vOp_Time,:vId_No;
		END;
		END-EXEC;
		if(SQLCODE!=0&&SQLCODE!=1043)
		{
			PUBLOG_DBERR("pubUserWakeUP"," %d","����ǳ�˯�û��� DCUSTMSGWAKEUP " ,SQLCODE);
			return -3;
		}

	PUBLOG_APPRUN("pubUserWakeUP","","�������÷��� "); 
	return 0;
}
/*
add by cencm at 2011-10-20 9:51:10
Ӧ�ü���ƽָ̨��ӿڻ�ȡ��Ϣ����
*/
char *mml_getvalue(const char *buf, const char *key, char *value, int maxlen)
{
        char *ptr0, *ptr1, *ptr2, *ptre;
        int len, slen;

        value[0] = 0;
        ptre = (char *)(buf + strlen(buf));
        ptr0 = (char *)buf;
        while( (ptr1 = (char *)strstr(ptr0, key)) >= ptr0)
        {
                ptr0 = ptr1 + strlen(key);
                if(ptr0 >= ptre)
                {
                        break;
                }
                if(ptr0[0] != '=')
                {
                        continue;
                }
                ptr0 ++;

                while(*ptr0 == ' ' || *ptr0 =='\t') ptr0 ++;
                if(*ptr0 == ',' || *ptr0 == 0)
                {
                        value[0] = 0;
                        return value;
                }
                if(*ptr0 == '\"')
                {
                        ptr0 ++;
                        ptr1 = strchr(ptr0, '\"');
                        if(ptr1 == NULL)
                        {
                                strncpy(value, ptr0, maxlen);
                                return 0;
                        }
                        value[ptr1 - ptr0] = 0;
                        memcpy(value, ptr0, ptr1-ptr0);
                        return value;
                }
                ptr1 = strchr(ptr0, ',');
                if(ptr1 == NULL)
                {
                        strncpy(value, ptr0, maxlen);
                        return value;
                }

                if(maxlen > ptr1-ptr0)
                {
                        maxlen = ptr1-ptr0;
                }
                value[maxlen] = 0;
                memcpy(value, ptr0, maxlen);
                return value;
        }
        return 0;
}

int QueryRunName(char *service_name ,char *iIdNo,char *vRunNameStr)
{
	int			input_par_num = 0;	/*�����������*/
	int			output_par_num = 0;	/*�����������*/
	int			i=0,j=0,k=0,ret=0;
	FBFR32		*transIN = NULL;	/*���뻺����*/
	FBFR32		*transOUT = NULL;	/*���������*/
	FLDLEN32	len;				/*�������������Ŀռ�*/
	char		temp_buf[1000];		/*��ʱʹ�õ��ַ�������*/

	/**��ȡǷ����Ϣ**/
	FBFR32    *sendbuf = NULL;
	FBFR32 	  *rcvbuf = NULL;
	FLDLEN32  sendlen,rcvlen;
	long      ilen=0, olen=0;
	int       RetValue=0;
	int 	  sendNum=0;
	int 	  recvNum=0;	

	EXEC SQL	BEGIN DECLARE SECTION;
	/* ������� */
	char 	  buffer1[256];
	char	  buffer2[256];
	char	  vRunName[30+1];
	EXEC SQL    END DECLARE SECTION;

	/**��ʼ���û�������Ϣ�ṹ��**/
	init(buffer1);
	init(buffer2);
	init(vRunName);

	/**��ȡ�û�ʣ����**/
	sendNum = 1;
	recvNum = 3;

	sendlen = (FLDLEN32)((sendNum)*120);
	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "5500", "%s", "��ʼ�����������ʧ��");
		return 5500;

	}
	Fchg32(sendbuf,SEND_PARMS_NUM32,0,"1",(FLDLEN32)0);
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,"3",(FLDLEN32)0);

	Fchg32(sendbuf,GPARM32_0,0,iIdNo,(FLDLEN32)0);
	
	
	printf("in GPARM32_0=[%s]\n",iIdNo);
	
	/*��ʼ�����������*/
	rcvlen = (FLDLEN32)((recvNum)*120);

	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		tpfree((char *)sendbuf);
		PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_, service_name, "", "%s", "��ʼ�����������ʧ��");
		return  1;
	}

	long reqlen=Fsizeof32(rcvbuf);
	/*���÷���*/
	/*������÷�ʽ 1:tp_call;	2:Ӧ�ü���ƽ̨  order_xml.h�ж���
	#define CALLSERV_TP	1
	#define CALLSERV_CA	2
	*/
	RetValue = 0;
	RetValue = fPubCallSrv(CALLSERV_TP,service_name,sendbuf,&rcvbuf);
	if(RetValue != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_, service_name, "", "v_ret=[%d]!",RetValue);
		pubLog_DBErr(_FFL_, service_name, "", "%s", "call sGetRunName failed!");
		return  1;
	}

		memset(buffer1, '\0', sizeof(buffer1));
		memset(buffer2, '\0', sizeof(buffer2));

		Fget32(rcvbuf,SVC_ERR_NO32,0,buffer1,(FLDLEN32)0);
	 	Fget32(rcvbuf,SVC_ERR_MSG32,0,buffer2,(FLDLEN32)0);

	  	Trim(buffer1);
		Trim(buffer2);
		pubLog_Debug(_FFL_,service_name,"","buffer1=[%s]",buffer1);
		pubLog_Debug(_FFL_,service_name,"","buffer2=[%s]",buffer2);
	if(strcmp(buffer1,"000000") != 0)
	{
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		PUBLOG_DBDEBUG(service_name);
		pubLog_Debug(_FFL_, service_name, "", "buffer1=[%s]!",buffer1);
		pubLog_DBErr(_FFL_, service_name, "", "%s", "call sGetRunName failed!");
		return  1;
	}

  	Fget32(rcvbuf,GPARM32_2,0,vRunName,(FLDLEN32)0);

  	Trim(vRunName);
  	
  	strcpy(vRunNameStr,vRunName);

  	/*�ͷŷ�����ڴ�*/
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);		

    pubLog_Debug(_FFL_, service_name, "", "����ִ�н���");


	return  0;
}



int pubCustAcctCycle(long vcontractNo,long vidNo,char *vphoneNo,int vcycleDay,long vloginAccept,char *vloginNo,char *vopNote,char *vregionCode,char *vopCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	
	long	contractNo=0;
	long	idNo=0;
	char	phoneNo[15+1];
	int		cycleDay=0,sysDay=0,v_ret=0;
	long	loginAccept=0;
	char	loginNo[6+1];
	char	opNote[60+1]; 
	char	opTime[14+1];
	char	regionCode[2+1];
	char	opCode[4+1];
	char	beginTime[14+1];
	char	endTime[14+1];
	char	ncycleDay[2+1];
	char  vSysDate[17+1];            /*   ϵͳʱ��       */
	TdCustAcctCycle tdCustAcctCycle;
	
	EXEC SQL END DECLARE SECTION;
	
	init(phoneNo);
	init(loginNo);
	init(opNote);
	init(opTime);
	init(regionCode);
	init(opCode);
	init(beginTime);
	init(endTime);
	init(ncycleDay);
	init(vSysDate);
	memset(&tdCustAcctCycle,0,sizeof(tdCustAcctCycle));
	
	PUBLOG_APPRUN("pubCustAcctCycle"," ","��ʼ����  pubCustAcctCycle");
	PUBLOG_APPRUN("pubCustAcctCycle"," ","vcontractNo [%ld]  vidNo[%ld]  vphoneNo[%s]   vcycleDay[%d]   ",vcontractNo,vidNo,vphoneNo,vcycleDay);
	
	contractNo=vcontractNo;
	idNo=vidNo;
	strcpy(phoneNo,vphoneNo);
	cycleDay=vcycleDay;
	loginAccept=vloginAccept;
	strcpy(loginNo,vloginNo);
	strcpy(opNote,vopNote);
	strcpy(regionCode,vregionCode);
	strcpy(opCode,vopCode);
	sprintf(ncycleDay,"%02d",cycleDay);
	
	
	EXEC SQL SELECT to_number(to_char(sysdate,'dd')),to_char(sysdate,'YYYYMMDD HH24:MI:SS'),to_char(sysdate,'yyyymmdd')||'000000' INTO :sysDay,:vSysDate,:opTime FROM dual;
	strncpy(beginTime,opTime,6);
	
	strcat(beginTime,ncycleDay);
	strcat(beginTime,"000000");
	
	PUBLOG_APPRUN("pubCustAcctCycle"," ","vSysDate [%s] ",vSysDate);
	PUBLOG_APPRUN("pubCustAcctCycle"," ","ncycleDay [%s] ",ncycleDay);
	PUBLOG_APPRUN("pubCustAcctCycle"," ","beginTime [%s] ",beginTime);
	PUBLOG_APPRUN("pubCustAcctCycle"," ","sysDay [%d] ",sysDay);
	PUBLOG_APPRUN("pubCustAcctCycle"," ","opTime [%s] ",opTime);
	PUBLOG_APPRUN("pubCustAcctCycle"," ","cycleDay [%d] ",cycleDay);
		
	if(cycleDay == sysDay)
	{
		sprintf(tdCustAcctCycle.sIdNo,"%ld",idNo);
		sprintf(tdCustAcctCycle.sContractNo,"%ld",	contractNo);
		strcpy(tdCustAcctCycle.sPhoneNo,		phoneNo);
		strcpy(tdCustAcctCycle.sBeginTime,		beginTime);
		strcpy(tdCustAcctCycle.sEndTime,		"21000101000000");
		strcpy(tdCustAcctCycle.sAcctCycleType,	"0");
		sprintf(tdCustAcctCycle.sCycleBeginDay,"%d",	cycleDay);
		strcpy(tdCustAcctCycle.sRegionCode,		regionCode);
		sprintf(tdCustAcctCycle.sOpNo,"%ld",loginAccept);
		strcpy(tdCustAcctCycle.sOpTime,			vSysDate);
			
		v_ret=OrderInsertDcustacctcycle(ORDERIDTYPE_USER,tdCustAcctCycle.sIdNo,100,opCode,loginAccept,loginNo,opNote,tdCustAcctCycle);
		if(v_ret != 0)
		{
			PUBLOG_APPRUN("pubCustAcctCycle"," ","OrderInsertDcustacctcycle error [%ld][%d]",idNo,v_ret);
			return -1;
		}
	}
	else
	{
		EXEC SQL SELECT to_char(add_months(sysdate,1),'yyyymm')||:ncycleDay||'000000' INTO :endTime FROM dual;
		
		sprintf(tdCustAcctCycle.sIdNo,"%ld",idNo);
		sprintf(tdCustAcctCycle.sContractNo,"%ld",	contractNo);
		strcpy(tdCustAcctCycle.sPhoneNo,		phoneNo);
		strcpy(tdCustAcctCycle.sBeginTime,		opTime);
		strcpy(tdCustAcctCycle.sEndTime,		endTime);
		strcpy(tdCustAcctCycle.sAcctCycleType,	"2");
		sprintf(tdCustAcctCycle.sCycleBeginDay,"%d",	cycleDay);
		strcpy(tdCustAcctCycle.sRegionCode,		regionCode);
		sprintf(tdCustAcctCycle.sOpNo,"%ld",loginAccept);
		strcpy(tdCustAcctCycle.sOpTime,			vSysDate);
			
		v_ret=OrderInsertDcustacctcycle(ORDERIDTYPE_USER,tdCustAcctCycle.sIdNo,100,opCode,loginAccept,loginNo,opNote,tdCustAcctCycle);
		if(v_ret != 0)
		{
			printf("OrderInsertDcustacctcycle error [%ld][%d]",idNo,v_ret);
			return -2;
		}
		
		
		strcpy(tdCustAcctCycle.sBeginTime,		endTime);
		strcpy(tdCustAcctCycle.sEndTime,		"21000101000000");
		strcpy(tdCustAcctCycle.sAcctCycleType,	"0");
			
		v_ret=OrderInsertDcustacctcycle(ORDERIDTYPE_USER,tdCustAcctCycle.sIdNo,100,opCode,loginAccept,loginNo,opNote,tdCustAcctCycle);
		if(v_ret != 0)
		{
			printf("OrderInsertDcustacctcycle error [%ld][%d]",idNo,v_ret);
			return -3;
		}
	}
	
	EXEC SQL INSERT INTO  dcustacctcyclehis
					(
					 ID_NO,CONTRACT_NO,PHONE_NO,BEGIN_TIME,END_TIME,ACCT_CYCLE_TYPE,
					 CYCLE_BEGIN_DAY,REGION_CODE,OP_NO,OP_TIME,UPDATE_LOGIN,UPDATE_ACCEPT,UPDATE_TIME,OP_NOTE,OP_CODE
					)
				SELECT
					 ID_NO,CONTRACT_NO,PHONE_NO,BEGIN_TIME,END_TIME,ACCT_CYCLE_TYPE,CYCLE_BEGIN_DAY,REGION_CODE,OP_NO,OP_TIME,
					 :loginNo,:loginAccept,:opTime,:opNote,:opCode
				FROM dcustacctcycle 
				WHERE id_no=:idNo AND op_no=:loginAccept;
	if(SQLCODE != 0)
	{
		PUBLOG_APPRUN("pubCustAcctCycle"," ","pubCustAcctCycle insert into dcustacctcyclehis error [%ld][%d]",idNo,SQLCODE);
		return -4;
	}
	PUBLOG_APPRUN("pubCustAcctCycle"," ","�������� ");
	return 0;
}


/**********************************************
 @wt  PRG  : pubGetCustAcctCyCle
 @wt  FUNC : ��ȡ�û�ָ�������ڿ�ʼ/��������
 @wt  author: ᯴�÷
 @wt 1 iIdNo					�û�IdNo
 @wt 2 iTime					��ȡ���ڵĻ�׼ʱ��
 @wt33 iPresentMonth			ָ���ĵڼ�������
 @wt44 iModeMonth				�ײͿ�ͨ��������  0��������ײͿ�ͨ����ʱ��Ϊ20500101	�������
 @wt55 oCustAcctCycleBeginTime	�û�ָ�����ڿ�ʼ����									�������
 @wt66 oCustAcctCycleEndTime	�û�ָ�����ڽ�������									�������
 @wt17 oErrMsg    				������Ϣ												�������
 *********************************************/
int pubGetCustAcctCyCle(char *iIdNo,char *iTime,int iPresentMonth,int iModeMonth,char *oCustAcctCycleBeginTime,char *oCustAcctCycleEndTime,char *oErrMsg )
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	vIdNo[15+1];
		int		sCount=0;
		char	vContractNo[14+1];
		char	vDay[2+1];
		char	vCustAcctCycleBeginTime[8+1];
		char	vCustAcctCycleEndTime[8+1];
		char	vMaxEndTime[17+1];
		char	vAcctCyCleType[1+1];
		char	vCycleBeginDay[2+1];
		char	vCycleType[1+1];
		int		vPresentMonth=0,vModeMonth=0;
		char	vTime[17+1];
	EXEC SQL END DECLARE SECTION;
	Sinitn(vIdNo);
	Sinitn(vDay);
	Sinitn(vCustAcctCycleBeginTime);
	Sinitn(vCustAcctCycleEndTime);
	Sinitn(vAcctCyCleType);
	Sinitn(vCycleBeginDay);
	Sinitn(vCycleType);
	Sinitn(vMaxEndTime);
	Sinitn(vTime);
	
	strcpy(vMaxEndTime,MAXENDTIME);
	
	strcpy(vIdNo,iIdNo);
	strcpy(vTime,iTime);
	vPresentMonth=iPresentMonth;
	vModeMonth=iModeMonth;
	
	Trim(vIdNo);
	/*�û���Ч���ڷ�Ϊ����acct_cycle_typeֵ������:(1) 0(��Ȼ���˻�)  (2) 1(��ɢ�����û����������ڱ�����ڹ������ڵ��û�) (3)2 ��ǰ�����ڷ������ڱ�����û�*/
	 EXEC SQL SELECT count(*) into :sCount
				FROM DCUSTACCTCYCLE
				WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(END_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
	if(SQLCODE != 0)
	{
		Sinitn(oErrMsg);
		sprintf(oErrMsg,"��ѯ�û���Ч���ڸ�������![%d]",SQLCODE);
		PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
		pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
		return -1;
	}
	switch(sCount)
	{
		case 0:/*��Ȼ�����û�*/
			EXEC SQL SELECT to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth),'yyyymm')||'01',
						decode(:vModeMonth,0,:vMaxEndTime,to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth+:vModeMonth),'yyyymm')||'01')
						INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
					FROM DUAL;
			if(SQLCODE != 0)
			{
				Sinitn(oErrMsg);
				sprintf(oErrMsg,"��ѯ��Ȼ�������û�ʱ�䱨��![%d]",SQLCODE);
				PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
				pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
				return -2;
			}
			strcpy(vCycleType,"0");
			break;
		case 1:/*��ɢ�����û� �����û��������ڱ�� Ŀǰ���ڹ������ڵ��û�*/
			EXEC SQL SELECT ACCT_CYCLE_TYPE,lpad(to_char(CYCLE_BEGIN_DAY),2,'0'),to_char(to_date(:vTime,'yyyymmdd hh24:mi:ss'),'dd')
					INTO :vAcctCyCleType,:vCycleBeginDay,:vDay
					FROM DCUSTACCTCYCLE
					WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(END_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
			if(SQLCODE != 0)
			{
				Sinitn(oErrMsg);
				sprintf(oErrMsg,"��ѯ��ɢ�����û��������ͱ���![%d]",SQLCODE);
				PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
				pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
				return -3;
			}
			if(strcmp(vAcctCyCleType,"0") == 0)/*��ɢ�����û�*/
			{
				if(atol(vDay)>=atol(vCycleBeginDay))
				{
					EXEC SQL SELECT to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth),'yyyymm')||to_char(lpad(CYCLE_BEGIN_DAY,2,'0')),
					decode(:vModeMonth,0,:vMaxEndTime,to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth+:vModeMonth),'yyyymm')||to_char(lpad(CYCLE_BEGIN_DAY,2,'0')))
					INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
					FROM DCUSTACCTCYCLE
					WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(END_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
				}
				else
				{
					EXEC SQL SELECT to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth-1),'yyyymm')||to_char(lpad(CYCLE_BEGIN_DAY,2,'0')),
					decode(:vModeMonth,0,:vMaxEndTime,to_char(add_months(to_date(:vTime,'yyyymmdd hh24:mi:ss'),:vPresentMonth-1+:vModeMonth),'yyyymm')||to_char(lpad(CYCLE_BEGIN_DAY,2,'0')))
					INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
					FROM DCUSTACCTCYCLE
					WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(END_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
				}
				if(SQLCODE != 0)
				{
					Sinitn(oErrMsg);
					sprintf(oErrMsg,"��ѯ��ɢ�����û�ʱ�䱨��![%d]",SQLCODE);
					PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
					pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
					return -4;
				}
				strcpy(vCycleType,"1");
			}
			else/*�û���ǰ����Ϊ��������*/
			{
				EXEC SQL SELECT TO_CHAR(add_months(to_date(END_TIME,'YYYYMMDD HH24:MI:SS'),:vPresentMonth-1),'YYYYMMDD'),
				decode(:vModeMonth,0,:vMaxEndTime,TO_CHAR(add_months(to_date(END_TIME,'YYYYMMDD HH24:MI:SS'),:vPresentMonth+:vModeMonth-1),'YYYYMMDD'))
				INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
				FROM DCUSTACCTCYCLE
				WHERE ID_NO= TO_NUMBER(:vIdNo)
				AND to_date(END_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
				if(SQLCODE != 0)
				{
					Sinitn(oErrMsg);
					sprintf(oErrMsg,"��ѯ��ɢ�����û�ʱ�䱨��![%d][%d][%d]",vPresentMonth,vModeMonth,SQLCODE);
					PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
					pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
					return -5;
				}
				strcpy(vCycleType,"1");
			}
			break;
		case 2:/*�û��ڵ�ǰ�����ڷ��������ڱ��*/
			if(vPresentMonth == 1)/*ȡ�û����¸����� ����������*/
			{
				EXEC SQL SELECT TO_CHAR(to_date(BEGIN_TIME,'YYYYMMDD HH24:MI:SS'),'YYYYMMDD'),
						to_char(add_months(to_date(end_time,'yyyymmdd hh24:mi:ss'),:vModeMonth-1),'yyyymmdd')
					INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
					FROM DCUSTACCTCYCLE
					WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(BEGIN_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
			}
			else
			{
				EXEC SQL SELECT to_char(add_months(TO_DATE(END_TIME,'YYYYMMDD HH24:MI:SS'),:vPresentMonth-2),'yyyymmdd'),
						to_char(add_months(to_date(END_TIME,'YYYYMMDD HH24:MI:SS'),:vPresentMonth+:vModeMonth-2),'yyyymmdd')
					INTO :vCustAcctCycleBeginTime,:vCustAcctCycleEndTime
					FROM DCUSTACCTCYCLE
					WHERE ID_NO= TO_NUMBER(:vIdNo)
					AND to_date(BEGIN_TIME,'yyyymmdd hh24:mi:ss')>to_date(:vTime,'yyyymmdd hh24:mi:ss');
			}
			if(SQLCODE != 0)
			{
				Sinitn(oErrMsg);
				sprintf(oErrMsg,"��ѯ��ɢ�����û�ʱ�䱨��![%d][%d][%d]",vPresentMonth,vModeMonth,SQLCODE);
				PUBLOG_DBDEBUG("pubGetCustAcctCyCle");
				pubLog_DBErr(_FFL_, "", "", "pubGetCustAcctCyCle :select DCUSTACCTCYCLE error[%d]",SQLCODE);
				return -7;
			}
			strcpy(vCycleType,"1");
			break;
		default : 
			return -8;
	}
	
	strcpy(oCustAcctCycleBeginTime,vCustAcctCycleBeginTime);
	strcpy(oCustAcctCycleEndTime,vCustAcctCycleEndTime);
	
	pubLog_Debug(_FFL_,"","","�û�[%s]��ǰ����������[%s]0:�������� 1:С���� 2:������\n",vIdNo, vAcctCyCleType);
	pubLog_Debug(_FFL_,"","","�û�[%s]����������[%s]\n",vIdNo, vCycleBeginDay);
	pubLog_Debug(_FFL_,"","","�û�[%s]��[%s]0 :��Ȼ�������û� 1����ɢ�����û�\n",vIdNo, vCycleType);
	pubLog_Debug(_FFL_,"","","�û�[%s]ԤԼ��[%d]�����ڵĿ�ʼʱ����[%s],��Ч[%d]������(0:ȡϵͳ������ʱ��20500101)����ʱ����[%s]\n",vIdNo, vPresentMonth,vCustAcctCycleBeginTime,vModeMonth,vCustAcctCycleEndTime);

	return 0;
}


/*********************
**Name��	pubAcctCycleChg
**create��	wanghao
**
*********************/
int pubAcctCycleChg(char *iPhoneNo ,char *iOldCycle ,char *iNewCycle ,char *iOpDate)
{

	EXEC SQL BEGIN DECLARE SECTION;
		char	phoneCycle[8+1];
		char	opMonth[6+1];
		char	phoneNewCycle[8+1];
		int		cycleDay=0;
		char	cycleType[1+1];
		char	tempStr[2+1];
		char	phoneNo[15+1];
		char	oldCycle[2+1];
		char	newCycle[2+1];
		char	opDate[8+1];

		long idNo = 0;
		long contractNo = 0;
		char phone_no[15+1];
		char vBeginTime[14+1];
		char vEndTime[14+1];
		char regionCode[2+1];
		long loginAccept=0;
		char opTime[14+1];
		char vToDay[2+1];

		TdCustAcctCycle tdCustAcctCycle;
		TdCustAcctCycleIndex oldIndex;
		TdCustAcctCycleIndex newIndex;
		char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
		int vRet = 0;

	EXEC SQL END DECLARE SECTION;
	Sinitn(phoneCycle);
	Sinitn(opMonth);
	Sinitn(phoneNewCycle);
	Sinitn(cycleType);
	Sinitn(tempStr);
	Sinitn(phoneNo);
	Sinitn(oldCycle);
	Sinitn(newCycle);
	Sinitn(opDate);

	Sinitn(phone_no);
	Sinitn(vBeginTime);
	Sinitn(vEndTime);
	Sinitn(regionCode);
	Sinitn(opTime);
	memset(&tdCustAcctCycle,0,sizeof(tdCustAcctCycle));
	memset(&oldIndex,0,sizeof(oldIndex));
	memset(&newIndex,0,sizeof(newIndex));
	Sinitn(v_parameter_array);

	strcpy(phoneNo,iPhoneNo);
	strcpy(oldCycle,iOldCycle);
	strcpy(newCycle,iNewCycle);
	strcpy(opDate,iOpDate);
	
	printf("phoneNo=[%s]\n"  ,   phoneNo );
	printf("oldCycle=[%s]\n" ,   oldCycle);
	printf("newCycle=[%s]\n" ,   newCycle);
	printf("opDate=[%s]\n"   ,   opDate  );
		
	printf("oldCycle=[%s]\n" ,   oldCycle);
	printf("strlen(oldCycle)=[%ld] \n",strlen(oldCycle) );
	printf("newCycleAAAA=[%s]\n" ,   newCycle);
	printf("strlen(newCycle)=[%ld] \n",strlen(newCycle) );
	if(strlen(oldCycle) == 1)
	{
		Sinitn(tempStr);
		sprintf(tempStr,"0%s",oldCycle);
		Sinitn(oldCycle);
		strcpy(oldCycle,tempStr);
		printf("strlen(oldCycle)=[%ld] \n",strlen(oldCycle) );
		printf("sizeof(oldCycle)=[%ld] \n",sizeof(oldCycle) );
		printf("oldCycle=[%s]\n" ,   oldCycle);
		printf("newCycleAAAA=[%s]\n" ,   newCycle);
		printf("strlen(newCycle)=[%ld] \n",strlen(newCycle) );
	}
	printf("newCycleAAAA=[%s]\n" ,   newCycle);
	printf("strlen(newCycle)=[%ld] \n",strlen(newCycle) );
	if(strlen(newCycle) == 1)
	{
		Sinitn(tempStr);
		sprintf(tempStr,"0%s",newCycle);
		Sinitn(newCycle);
		strcpy(newCycle,tempStr);
		printf("newCycle=[%s]\n" ,   newCycle);
	}
	printf("newCycleBBB=[%s]\n" ,   newCycle);
	if(strlen(opDate) == 0)
	{
		EXEC SQL SELECT to_char(sysdate,'yyyymmdd') INTO :opDate FROM dual;
	}
	Sinitn(vToDay);
	EXEC SQL SELECT substr(:opDate,1,6) ,to_char(sysdate,'dd') INTO :opMonth , :vToDay FROM dual;

	if(atol(oldCycle)<=atol(vToDay))
	{
		EXEC SQL SELECT to_char(add_months(to_date(:opMonth,'YYYYMM'),1),'YYYYMM') INTO :opMonth  FROM dual;
	}
	
	sprintf(phoneCycle,"%s%s",opMonth,oldCycle);
	
	sprintf(phoneNewCycle,"%s%s",opMonth,newCycle);
	printf("phoneCycle=[%s]\n" ,   phoneCycle);
	printf("phoneNewCycle=[%s]\n" ,   phoneNewCycle);
	if(atol(phoneNewCycle) < atol(phoneCycle)) 
	{
		Sinitn(phoneNewCycle);
		EXEC SQL SELECT to_char(add_months(to_date(:opMonth,'yyyymm'),1),'yyyymm')||:newCycle INTO :phoneNewCycle FROM dual;
	}
	/*
	**EXEC SQL SELECT to_date(:phoneNewCycle,'yyyymmdd')-to_date(:phoneCycle,'yyyymmdd') INTO :cycleDay FROM dual;
	**	
	**if(cycleDay < 25 )
	**{
	**	EXEC SQL SELECT to_char(add_months(to_date(substr(:phoneNewCycle,1,6),'yyyymm'),1),'yyyymm')||:newCycle INTO :phoneNewCycle FROM dual;
	**	strcpy(cycleType,"1");
	**}
	**else if (cycleDay >=25 && cycleDay <=31)
	**{
	**	strcpy(cycleType,"2");
	**}
	**else
	**{
	**	strcpy(cycleType,"1");
	**}
	*/
	
	/*�û����ڴ���25�յİ��մ�����ִ��,С��25�հ���С����ִ�С����� ϵͳ�����ڱ������ԤԼ��Ч��ֻ���ɶ����ڱ�Ϊ��Ȼ���� newCycle����Ϊ��01��*/
	if(atol(oldCycle) <= 25 )
	{
		EXEC SQL SELECT  to_char(add_months(to_date(:opMonth,'YYYYMM'),1),'YYYYMM')||:newCycle INTO :phoneNewCycle FROM dual;
		strcpy(cycleType,"1");
	}
	else if (atol(oldCycle) >25 && atol(oldCycle) <=31)
	{
		EXEC SQL SELECT to_char(add_months(sysdate,2),'yyyymm')||:newCycle INTO :phoneNewCycle FROM dual;
		strcpy(cycleType,"2");
	}
	
	printf("������ȡֵ�� cycleType=[%s]\n" ,   cycleType);
	printf("������ȡֵ�� phoneNewCycle=[%s]\n" ,   phoneNewCycle);

	/***
	**	EXEC SQL UPDATE dcustacctcycle SET end_time=:phoneCycle||'000000',op_time=sysdate
	**			WHERE phone_no=:phoneNo
	**			AND to_date(end_time,'yyyymmdd hh24miss')>sysdate;
	**	printf("SQLCODE=[%d]\n" ,   SQLCODE);
	**	if(SQLCODE != 0)
	**	{
	**		PUBLOG_DBERR("", "", "pubAcctCycleChg :UPDATE DCUSTACCTCYCLE error[%d]",SQLCODE);
	**		return -1;
	**	}
	**
	**	EXEC SQL INSERT INTO dcustacctcycle
	**	(
	**		ID_NO,CONTRACT_NO,PHONE_NO,BEGIN_TIME,END_TIME,
	**		ACCT_CYCLE_TYPE,CYCLE_BEGIN_DAY,REGION_CODE,OP_NO,OP_TIME
	**	)
	**	SELECT id_no,contract_no,phone_no,:phoneCycle||'000000',:phoneNewCycle||'000000',:cycleType,:newCycle,substr(belong_code,1,2),sMaxSysAccept.nextVal,sysdate
	**	FROM dcustmsg WHERE PHONE_NO=:phoneNo;
	**	printf("SQLCODE=[%d]\n" ,   SQLCODE);
	**	if(SQLCODE != 0)
	**	{
	**		PUBLOG_DBERR("", "", "pubAcctCycleChg :INSERT DCUSTACCTCYCLE error[%d]",SQLCODE);
	**		return -2;
	**	}
	*****/


	exec sql SELECT  sMaxSysAccept.nextVal 
		into  :loginAccept 
	FROM dual;
	if ( SQLCODE != 0 )
	{
		PUBLOG_DBERR("", "", "pubAcctCycleChg :GET loginAccept error[%d]",SQLCODE);
		return -1;
	}
	PUBLOG_DBERR("", "", "wanghao");
	
	int i=0;
	EXEC SQL DECLARE curacctcycle_1 CURSOR for SELECT ID_NO, PHONE_NO, BEGIN_TIME
	from dcustacctcycle  WHERE phone_no=:phoneNo
	AND to_date(end_time,'yyyymmdd hh24miss')>sysdate;
	EXEC SQL OPEN curacctcycle_1;
	for (;;){

		EXEC SQL FETCH curacctcycle_1 INTO :idNo, :phoneNo, :vBeginTime ;
		if ((SQLCODE != 0) && (SQLCODE != 1403) )
		{
			EXEC SQL CLOSE curacctcycle_1;
			PUBLOG_DBERR("", "", "pubAcctCycleChg :UPDATE DCUSTACCTCYCLE error[%d]",SQLCODE);
			return -1;
		}
		if (SQLCODE == 1403) break;
			
		strcpy(v_parameter_array[0],phoneCycle);
		sprintf(v_parameter_array[1] ,"%ld", idNo);
		strcpy(v_parameter_array[2],phoneNo);
		strcpy(v_parameter_array[3],vBeginTime);
		
		sprintf(oldIndex.sIdNo ,"%ld", idNo);
		strcpy(oldIndex.sPhoneNo , phoneNo);
		strcpy(oldIndex.sBeginTime , vBeginTime);
		
		sprintf(newIndex.sIdNo ,"%ld", idNo);
		strcpy(newIndex.sPhoneNo , phoneNo);
		strcpy(newIndex.sBeginTime , vBeginTime);
		
		PUBLOG_DBERR("", "", "wanghao01");
		
	/**
		int OrderUpdateDcustacctcycle(i_sendid_type,i_sendid_no,i_order_right,
							   i_op_code,i_op_accept,i_op_login,i_op_note,
							   oldIndex,newIndex,
							   i_update_sql,i_where_sql,i_parameter_array
							   )

	**/	

		for(i=0;i<4;i++)
			printf("v_parameter_array[%d]=[%s]\n",i,v_parameter_array[i]);
		vRet = OrderUpdateDcustacctcycle(ORDERIDTYPE_USER, oldIndex.sIdNo ,100, 
		"", loginAccept, "", "OrderUpdate",
		oldIndex, newIndex,
		" end_time=:phoneCycle||'000000',op_time=sysdate ",
		" AND to_date(end_time,'yyyymmdd hh24miss')>sysdate ",v_parameter_array);
		if( vRet != 0)
		{
			PUBLOG_DBERR("", "", "pubAcctCycleChg :UPDATE DCUSTACCTCYCLE error[%d]",SQLCODE);
			EXEC SQL CLOSE curacctcycle_1;
			return -2;
		}

	}

	PUBLOG_DBERR("", "", "wanghao02");
	EXEC SQL CLOSE curacctcycle_1;

	PUBLOG_DBERR("", "", "wanghao03");
	
	exec sql SELECT id_no, contract_no, phone_no, :phoneCycle||'000000', :phoneNewCycle||'000000' ,
		 substr(belong_code,1,2),  to_char(sysdate,'yyyymmddhh24miss')
	into :idNo, :contractNo, :phoneNo, :vBeginTime, :vEndTime, :regionCode,  :opTime
	FROM dcustmsg WHERE PHONE_NO=:phoneNo;
	if ( SQLCODE != 0 )
	{
		PUBLOG_DBERR("", "", "pubAcctCycleChg :GET loginAccept error[%d]",SQLCODE);
		return -1;
	}
	
	PUBLOG_DBERR("", "", "wanghao03");
	sprintf(tdCustAcctCycle.sIdNo,      "%ld",idNo);
	sprintf(tdCustAcctCycle.sContractNo,"%ld",	contractNo);
	strcpy(tdCustAcctCycle.sPhoneNo,		phoneNo);
	strcpy(tdCustAcctCycle.sBeginTime,		vBeginTime);
	strcpy(tdCustAcctCycle.sEndTime,		vEndTime);
	strcpy(tdCustAcctCycle.sAcctCycleType,	cycleType);
	strcpy(tdCustAcctCycle.sCycleBeginDay,	newCycle);
	strcpy(tdCustAcctCycle.sRegionCode,		regionCode);
	sprintf(tdCustAcctCycle.sOpNo,"%ld",loginAccept);
	strcpy(tdCustAcctCycle.sOpTime,			opTime);

	vRet = OrderInsertDcustacctcycle(ORDERIDTYPE_USER,tdCustAcctCycle.sIdNo,100, "",
		loginAccept,"","pubAcctCycleChg",tdCustAcctCycle);
	if( vRet != 0)
	{
		PUBLOG_DBERR("", "", "pubAcctCycleChg :INSERT DCUSTACCTCYCLE error[%d]",SQLCODE);
		return -3;
	}


/*
	EXEC SQL INSERT INTO dcustacctcycle
	(
		ID_NO,CONTRACT_NO,PHONE_NO,BEGIN_TIME,END_TIME,
		ACCT_CYCLE_TYPE,CYCLE_BEGIN_DAY,REGION_CODE,OP_NO,OP_TIME
	)
	SELECT id_no,contract_no,phone_no,:phoneNewCycle||'000000','21000101000000','0',:newCycle,substr(belong_code,1,2),sMaxSysAccept.nextVal,sysdate
	FROM dcustmsg WHERE PHONE_NO=:phoneNo;
	printf("SQLCODE=[%d]\n" ,   SQLCODE);
	if(SQLCODE != 0)
	{
		PUBLOG_DBERR("", "", "pubAcctCycleChg :INSERT DCUSTACCTCYCLE error[%d]",SQLCODE);
		return -3;
	}
	*/
	printf("phoneNo=[%s]\n"  ,   phoneNo );
	printf("oldCycle=[%s]\n" ,   oldCycle);
	printf("newCycle=[%s]\n" ,   newCycle);
	printf("opDate=[%s]\n"   ,   opDate  );
	
	
	return 0;
}
/**********************************************
 @majha FUNC : fBizOprLimitCtl  ҵ�����Ȩ�޿���
 Note: (Mifi, ʡ��Я�ŵ�,�Ժ���������ҵ��,�����ñ�����SQL����)
 @input  params: phoneNO -> �������
 @input  params: idNo -> �û�ID
 @input  params: opCode -> ��������
 @input  params: return_mess -> ������Ϣ
 @output params: 
 *********************************************/
int fBizOprLimitCtl(char *phoneNo, char *idNo,char *opCode,char *oRetMsg)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char	vSqlStr[1024+1];
	char	vIdNo[14+1];
	char	vPhoneNo[15+1];
	char	vOpCode[4+1];
	int		icount = 0,vCount=0;
	char	vSrvNo[4+1];
	char	vSrvName[128+1];
	char	vRetCode[6+1];
	char	vRetMsg[512+1];
	char	vDealSql[1024+1];
	char	vParamType[1+1];
	char	vContractNo[14+1];
	char	vIdIccid[20+1];
	char	vContent[60+1];		/*������Ϣ*/
	EXEC SQL END DECLARE SECTION;
	
	Sinitn(vIdNo);
	Sinitn(vPhoneNo);
	Sinitn(vOpCode );
	Sinitn(vSrvNo );
	Sinitn(vSrvName );
	Sinitn(vRetCode );
	Sinitn(vRetMsg );
	Sinitn(vDealSql );
	Sinitn(vParamType );
	Sinitn(vContractNo);
	Sinitn(vIdIccid);
	Sinitn(vContent);
	
	
	strcpy(vPhoneNo, 	phoneNo);
	strcpy(vIdNo, 		idNo);
	strcpy(vOpCode, 	opCode);

	Trim(vPhoneNo);
	Trim(vIdNo);
	Trim(vOpCode);
	
	EXEC SQL SELECT to_char(b.contract_no),c.id_iccid
				into :vContractNo,:vIdIccid
				from dcustmsg b,dcustdoc c
				where id_no=to_number(:vIdNo)
				and b.cust_id=c.cust_id;
	if(SQLCODE!=0&&SQLCODE!=1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","001001","select dcustmsg error !SQLCODE=%d",SQLCODE);
		return 001001;
	}
	if(SQLCODE==1403)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","001002","���벻����!SQLCODE=%d",SQLCODE);
		return 001002;
	}
	Trim(vContractNo);
	Trim(vIdIccid);
	
	EXEC SQL DECLARE mycur0 CURSOR for
				select b.srvno,b.srv_name,b.ret_code,b.ret_msg,b.param_type,b.deal_sql
				from spubLogincheck b,sMifiCONTROL c
				where b.srvno=c.srvno
				and c.op_code=:vOpCode
				and c.use_flag='Y'
				order by b.srvno asc;
	EXEC SQL open mycur0 ;
	if(SQLCODE!=0)
	{
		PUBLOG_DBDEBUG("");
		pubLog_DBErr(_FFL_,"","001003","select spubLogincheck,sMifiCONTROL error !SQLCODE=%d",SQLCODE);
		return 001003;
	}
	for(;;)
	{
		init(vSrvNo);
		init(vSrvName);
		init(vRetCode);
		init(vRetMsg);
		init(vParamType);
		init(vDealSql);
		init(vContent);

		EXEC SQL fetch mycur0 into :vSrvNo,:vSrvName,:vRetCode,
							:vRetMsg,:vParamType,:vDealSql;
		if(SQLCODE!=0&&SQLCODE!=1403)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","001002","���α�mycur0ʧ��!SQLCODE=%d",SQLCODE);
			return 001004;
		}
		if(SQLCODE==1403)
		{
			break;
		}
		Trim(vSrvNo);
		Trim(vSrvName);
		Trim(vRetCode);
		Trim(vRetMsg);
		Trim(vParamType);
		Trim(vDealSql);	
		
		if(strncmp(vParamType,"0",1)==0)		/* ������� */
		{
			strcpy(vContent,vPhoneNo);
		}
		else if (strncmp(vParamType,"1",1)==0)	/* �ʻ��� */
		{
			strcpy(vContent,vContractNo);
		}
		else if (strncmp(vParamType,"2",1)==0)	/* �û� */
		{
			strcpy(vContent,vIdNo);
		}
		else if (strncmp(vParamType,"3",1)==0)	/* ֤������ */
		{
			strcpy(vContent,vIdIccid);
		}
		else
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","001006","��ѯ���벻����!SQLCODE=%d",SQLCODE);
			return 001006;
		}
		printf("vContent=[%s]\n",vContent);
		vCount=0;
		EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :vDealSql
				INTO :vCount
				USING :vContent;
			END;
		END-EXEC;

		printf("vCount=[%d]\n",vCount);
		
		if(vCount>0)
		{
			PUBLOG_DBDEBUG("");
			pubLog_DBErr(_FFL_,"","001007","����ҵ������!SQLCODE=%d",SQLCODE);
			strcpy(oRetMsg,vRetMsg);
			return 001007;
		}
	}
	return 0;		
}
