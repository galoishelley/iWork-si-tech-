/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#include "order_xml.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/*
CREATE TABLE zk_tmp
(
  SERV_TYPE      VARCHAR2(20 BYTE)              NOT NULL,
  SERV_LOCATION  VARCHAR2(40 BYTE)              NOT NULL,
  SERV_NAME      VARCHAR2(40 BYTE)              NOT NULL,
  OBJECT_TYPE    CHAR(1 BYTE)                   NOT NULL,
  OBJECT_NAME    VARCHAR2(80 BYTE)               NULL,
  OBJECT_DESC    VARCHAR2(100 BYTE)              null,
  OPER_S         CHAR(1 BYTE)                    NULL,
  OPER_I         CHAR(1 BYTE)                    NULL,
  OPER_U         CHAR(1 BYTE)                    NULL,
  OPER_D         CHAR(1 BYTE)                    NULL,
  OPER_F         CHAR(1 BYTE)                    NULL,
  type_detail 	varchar2(30) 					null,
  num             number(4)                        not null
)
TABLESPACE CUSTMSG03;
CREATE INDEX zk_tmp_PK ON zk_tmp
(SERV_TYPE, SERV_LOCATION, SERV_NAME)
TABLESPACE CUSTMSG03_IDX;

CREATE TABLE zkzk
(
  SERV_TYPE      VARCHAR2(20 BYTE)              NOT NULL,
  SERV_LOCATION  VARCHAR2(40 BYTE)              NOT NULL,
  SERV_NAME      VARCHAR2(40 BYTE)              NOT NULL,
  table_NAME    VARCHAR2(80 BYTE)               NULL,
  read         number(4)                   NULL,
  write         number(4)                   NULL
)
TABLESPACE CUSTMSG03;

CREATE TABLE zk_func_tree
(
  SERV_TYPE      VARCHAR2(20 BYTE)              NOT NULL,
  SERV_LOCATION  VARCHAR2(40 BYTE)              NOT NULL,
  SERV_NAME      VARCHAR2(40 BYTE)              NOT NULL,
  object_NAME    VARCHAR2(80 BYTE)               NULL,
  parent_object	 VARCHAR2(80 BYTE)               NULL,
  num             number(4)                        not null
)
TABLESPACE CUSTMSG03;
*/

/*
int test2(v_num)
int	 v_num;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_tmp_buf[1000];
		char v_serv_location[100];
		char v_serv_name[100];
	EXEC SQL END DECLARE SECTION;	

	if (v_num>20){
		EXEC SQL COMMIT;
		printf("--[%d]\n",v_num);
		exit(0);		
	}

	exec sql insert into zk_tmp 
		select a.*,:v_num+1
			from ng_dServDetail_get a,zk_tmp b
			where a.serv_type in ('cfile','server') 
			  and a.serv_type=b.serv_type and a.serv_name=b.object_name
			  and b.num=:v_num;
	if (SQLCODE==NOTFOUND || SQLNUM==0){
		return 0;
	}
				
	test2(v_num+1);
}

int test1(){

	EXEC SQL BEGIN DECLARE SECTION;
		char v_tmp_buf[1000];
		char v_serv_location[100];
		char v_serv_name[100];
	EXEC SQL END DECLARE SECTION;	
	
	exec sql delete zkzk;
	
	EXEC SQL DECLARE CondCodeCur CURSOR FOR 
		select distinct a.serv_location,a.serv_name from ng_dServDetail_get a,temp_all_serv b 
			where a.serv_location=b.srv_name and a.serv_name=b.svc_name;
	EXEC SQL OPEN CondCodeCur;
	for (;;){
		EXEC SQL FETCH CondCodeCur INTO :v_serv_location,:v_serv_name;

		if(SQLCODE==NOTFOUND) break;
        
        if(SQLCODE<0){
             EXEC SQL CLOSE CondCodeCur;
             return -200000;            
        }

		strim(v_serv_location);	strim(v_serv_name);
		
		exec sql delete zk_tmp;
		exec sql insert into zk_tmp            
			select distinct a.*,0 from ng_dServDetail_get a 
				where serv_type='cfile' and serv_location=:v_serv_location and serv_name=:v_serv_name;
		test2(0);
		
		exec sql insert into zkzk
			select distinct 'cfile',:v_serv_location,:v_serv_name,object_name,
				to_number(oper_s),sign(to_number(oper_i)+to_number(oper_u)+to_number(oper_d)) 
				from zk_tmp where object_type='T';

		exec sql commit;
	}
	EXEC SQL CLOSE CondCodeCur; 	
}
*/

int test2(v_num)
int	 v_num;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char v_tmp_buf[1000];
		char v_serv_location[100];
		char v_serv_name[100];
	EXEC SQL END DECLARE SECTION;	

	if (v_num>20){
		EXEC SQL COMMIT;
		printf("--[%d]\n",v_num);
		exit(0);		
	}

	exec sql insert into zk_tmp 
		select distinct a.*,:v_num+1
			from ng_dServDetail_get a,zk_tmp b
			where ((a.type_detail='cfile' and a.serv_location=b.serv_location and a.serv_name=b.object_name)
			  or (a.type_detail in ('cfile_func','server') and a.serv_name=b.object_name))
			  and a.object_type='F' and b.num=:v_num;
/*
	exec sql insert into zk_tmp 
		select distinct a.*,:v_num+1
			from ng_dServDetail_get a,zk_tmp b
			where a.serv_type in ('cfile','server')  and a.serv_name=b.object_name
			  and a.object_type='F' and b.num=:v_num;
*/
	if (SQLCODE==NOTFOUND || SQLNUM==0){
		return 0;
	}
				
	test2(v_num+1);
}

int test1(){

	EXEC SQL BEGIN DECLARE SECTION;
		char v_tmp_buf[1000];
		char v_serv_location[100];
		char v_serv_name[100];
	EXEC SQL END DECLARE SECTION;	
	
	exec sql delete zk_func_tree;
	
	EXEC SQL DECLARE CondCodeCur CURSOR FOR 
		select distinct a.serv_location,a.serv_name from ng_dServDetail_get a
			where a.type_detail='1029';
	EXEC SQL OPEN CondCodeCur;
	for (;;){
		EXEC SQL FETCH CondCodeCur INTO :v_serv_location,:v_serv_name;

		if(SQLCODE==NOTFOUND) break;
        
        if(SQLCODE<0){
             EXEC SQL CLOSE CondCodeCur;
             return -200000;            
        }

		strim(v_serv_location);	strim(v_serv_name);
		
		exec sql delete zk_tmp;
		exec sql insert into zk_tmp            
			select distinct a.*,0 from ng_dServDetail_get a 
				where serv_type='cfile' and serv_location=:v_serv_location and serv_name=:v_serv_name;
		test2(0);
		
		exec sql insert into zk_func_tree
			select distinct 'cfile',:v_serv_location,:v_serv_name,object_name,serv_name,num
				from zk_tmp 
				where object_type='F';
		exec sql commit;
	}
	EXEC SQL CLOSE CondCodeCur; 	
}


int main (argc,argv)
	int argc;
	char **argv;
{
	EXEC SQL BEGIN DECLARE SECTION;
		char this_user[30];
		char this_pwd[30];
		char this_srv[30];
	EXEC SQL END DECLARE SECTION;	
	char v_id_no[30];
	char v_update_sql[1000];
	char v_where_sql[1000];
	char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	int v_ret=0;

	strcpy(this_user,"dbcustadm");
	strcpy(this_pwd,"dbcustadm123");
	strcpy(this_srv,"THCUST2");
		
	EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd USING :this_srv;

	printf("%d\n",7%3);
	
	test1();

	exit(0);
}
