#include "boss_srv.h"
#include "publicsrv.h"
EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

int getLoginAccept(char* vLoginAccept, char* vBipCode)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char vLoginTmp[20+1];
	
	EXEC SQL END DECLARE SECTION;
	
	memset(vLoginTmp,0x00,sizeof(vLoginTmp));
	
	EXEC SQL SELECT to_char(sysdate,'yyyymmddhh24miss')||trim(to_char(mod(smaxsysaccept.nextval,1000000),'000000'))
	           INTO vLoginTmp
	           FROM dual;
	          
	if (SQLCODE != 0)
	{
		return 1;
	}
	
	sprintf(vLoginAccept,"431%s%s", vBipCode, vLoginTmp);
	
	return 0;
}

int insert_req(char *iPhoneNo, char* iUsimPhone, char* iModeId, char* iOpCode, char* iLoginNo,char* iOrgCode, char* iLoginAccept)
{
	EXEC SQL insert into wnetcardchgreq(phone_no, usim_phoneno, login_accept, mode_id, op_code, login_no, org_code ,create_time)
	                values(:iPhoneNo, :iUsimPhone, :iLoginAccept, :iModeId, :iOpCode, :iLoginNo, :iOrgCode ,sysdate);
	                	
	if(SQLCODE != 0)
		return 1;
		
	return 0;
}

int insert_stop(char* vPhoneNo, char* vUsimPhone, char* vStopFlag)
{
	
	EXEC SQL INSERT INTO wnetcardstop(phone_no,usim_phoneno,create_time,stop_flag)
	               VALUES(:vPhoneNo, :vUsimPhone, sysdate, :vStopFlag);
	               	
	if(SQLCODE != 0)
		return 1;
		
	return 0;
}

int del_stop(char* vPhoneNo, char* vUsimPhone, char* vStopFlag)
{
	EXEC SQL DELETE FROM wnetcardstop 
	                WHERE phone_no = :vPhoneNo 
	                	AND usim_phoneno = :vUsimPhone 
	                	AND stop_flag = :vStopFlag;
		
	if(SQLCODE != 0)
		return 1;
		
	return 0;
}

#ifdef _DEPRECATED_
int del_grprawake(char* vPhoneNo)
{
	EXEC SQL insert into wgprsfunchis(
					phone_no, send_time, over_flux, op_time, op_flag, 
					login_accept, update_time, update_type,top_code)
			select phone_no, send_time, over_flux, op_time, op_flag, 
					login_accept, sysdate, 'd', top_code 
			from wgprsfunc 
			WHERE phone_no = :vPhoneNo 
	               AND top_code = 'GLAP';
	if(SQLCODE != 0)
		return 1;

	EXEC SQL DELETE FROM wgprsfunc 
	                WHERE phone_no = :vPhoneNo 
	                	AND top_code = 'GLAP';
		
	if(SQLCODE != 0)
		return 1;
		
	return 0;
}
#endif
