/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#include "order_xml.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;



int Insert()
{
	int v_ret,i,j,v_array_num=0;
	MsgBodyType	vMsgBodyType;
	char vXmlArray[DLINTERFACEARRAY][DLINTERFACEDATA];
	char vXmlString[DLINTERFACEARRAY*DLINTERFACEDATA];
	char v_tmp_buf[100];
	OrderTable v_order_data;
	
	for (i=0;i<100;i++){
		init(vXmlArray);

		OrderInitMsgBody(&vMsgBodyType,ORDERTYPEBUSI);

		v_ret=OrderSetServiceInfo(&vMsgBodyType,"sZKTest",2,0);

		init(v_tmp_buf);
		sprintf(v_tmp_buf,"%d",i);
		v_ret=OrderSetParameter(&vMsgBodyType,"id_no",v_tmp_buf);
		init(v_tmp_buf);
		sprintf(v_tmp_buf,"%d",i*5);		
		v_ret=OrderSetParameter(&vMsgBodyType,"value",v_tmp_buf);

		v_ret=OrderGenXml(&vMsgBodyType,vXmlString);
		if (v_ret<0)
			return -500001;

		OrderDestroyMsgBody(&vMsgBodyType);

		OrderInitStructOrder(&v_order_data);

		strcpy(v_order_data.order_type,ORDERTYPEBUSI);
		strcpy(v_order_data.id_type,ORDERIDTYPE_USER);
		init(v_tmp_buf);
		sprintf(v_tmp_buf,"%d",i);
		strcpy(v_order_data.id_no,v_tmp_buf);
		strcpy(v_order_data.busi_code,"sZKTest");
		strcpy(v_order_data.op_code,"1104");
		v_order_data.order_right=100;
		v_order_data.op_accept=12345678;
		strcpy(v_order_data.op_login,"system");
		strcpy(v_order_data.data_format,ORDERDATAFORMAT_XML);

		v_ret=OrderInserSend(&v_order_data,vXmlString);
		if (v_ret!=0){
			return -500002;
		}
			
		if (i%100000==0){
			EXEC SQL COMMIT;
			printf("[%d]\n",i/100000);
		}
	}

	return 0;
}

void test1(){
	
	char v_xml_array[DLINTERFACEARRAY][DLINTERFACEDATA];
	int v_array_num,i;
	char v_order_string[20];
	char v_tmp_buf1[20];
	char v_tmp_buf2[20];
	char v_tmp_buf3[20];
	
	strcpy(v_order_string,"张坤CRM事业部");
	
	strim(v_order_string);


/*	
	for (i=0;i<v_array_num;i++)
		printf("[%s]\n",);
*/
}

void test(){
/*
	struct tm *p;
	time_t t;
	int curDate;

	t = time(NULL);
	p = localtime(&t);
	
	printf("[%ld]\n",t);
*/

struct timeval t1,t2;


  gettimeofday(&t1,NULL);

  gettimeofday(&t2,NULL);

  printf("BEG TIME:%d.%06d\n",t1.tv_sec,t1.tv_usec);
  printf("END TIME:%d.%06d\n",t2.tv_sec,t2.tv_usec);
}



struct StructClob{
		char a[5];
		char b[64*1024];
};
typedef struct StructClob Clob;
#define CLOBSIZE sizeof(Clob)

int InClob(){
	EXEC SQL BEGIN DECLARE SECTION;
		Clob	v_clob;
		char	v_a[5];
		char	v_b[4001];
		char	v_clob_a[5];
		char	v_clob_b[65536];
	EXEC SQL END DECLARE SECTION;
	struct timeval t1,t2;
	int i=0,j;
/*	
	strcpy(v_clob.a,"22");
	strcpy(v_clob.b,"张坤张磊");
	
	EXEC SQL INSERT INTO zkzk values(:v_clob.a,:v_clob.b);
	printf("[%d][%s]\n",SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;
	
	EXEC SQL SELECT * INTO :v_clob FROM zkzk WHERE rownum=1;
	printf("[%d][%s]\n",SQLCODE,SQLERRMSG);
	printf("[%s][%s]\n",v_clob.a,v_clob.b);
*/
/*
	gettimeofday(&t1,NULL);
	printf("BEG TIME:%d.%06d\n",t1.tv_sec,t1.tv_usec);

	EXEC SQL DECLARE NodeCur1 CURSOR FOR 
		SELECT a,b from zk1;
	EXEC SQL OPEN NodeCur1;
	for(;;){
		EXEC SQL FETCH NodeCur1 INTO :v_a,:v_b;

		if (SQLCODE==NOTFOUND)  break;
		
		if(SQLCODE<0){
			EXEC SQL CLOSE NodeCur1;
			return -280000;       
		}
		
		strim(v_b);
		EXEC SQL INSERT INTO zk2 values(:v_a,:v_b);
		
		i++;
		if (i>50000) break;
	}
	EXEC SQL CLOSE NodeCur1;	
	EXEC SQL COMMIT;
	gettimeofday(&t1,NULL);
	printf("END TIME:%d.%06d\n",t1.tv_sec,t1.tv_usec);

	i=0;
	gettimeofday(&t2,NULL);
	printf("BEG TIME:%d.%06d\n",t2.tv_sec,t2.tv_usec);
	EXEC SQL DECLARE NodeCur CURSOR FOR 
		SELECT a,b FROM zkzk1;
	EXEC SQL OPEN NodeCur;
	for(;;){
		EXEC SQL FETCH NodeCur INTO :v_clob;

		if (SQLCODE==NOTFOUND)  break;
		
		if(SQLCODE<0){
			EXEC SQL CLOSE NodeCur;
			return -280000;       
		}
		
		strim(v_clob.b);
		EXEC SQL INSERT INTO zkzk2 values(:v_clob.a,:v_clob.b);
		
		i++;
		if (i>50000) break;
	}
	EXEC SQL CLOSE NodeCur;	
	EXEC SQL COMMIT;
	
	gettimeofday(&t2,NULL);
	printf("END TIME:%d.%06d\n",t2.tv_sec,t2.tv_usec);
*/
}


int parseRecvMsg(char *recvMsg, char *lable, char *value)
{
/*
    char *tempstr;
    tempstr=strstr(recvMsg,lable);
    value=strtok(tempstr,"=");   
    printf("value=%s\n",value);
    value=strtok(NULL,",");
    printf("value=%s\n",value);
*/
	strcpy(value,"1111");
	return 0;
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


/*			
	EXEC SQL CONNECT :this_user IDENTIFIED BY :this_pwd USING :this_srv;

	parseRecvMsg(this_user,this_pwd,this_srv);
	
	printf("[%s]\n",this_srv);
*/
/*

	InClob();
*/	
	
/*	
	v_ret=Insert();		
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;	
*/


/*
	v_ret=OrderInsertCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789","1111","V","22222","0","33333333","测试测试");
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;
*/

/*
	strcpy(v_parameter_array[0],"123456");
	strcpy(v_parameter_array[1],"123456789");
	v_ret=OrderUpdateCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789",
			"field_value=:v2","",v_parameter_array);
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;
*/
/*
	strcpy(v_parameter_array[0],"123456789");
	v_ret=OrderDeleteCustMsgAdd("2","123456789",100,
			"1104",998899990,"system","",
			"123456789",
			"",v_parameter_array);
	printf("[%d][%d][%s]\n",v_ret,SQLCODE,SQLERRMSG);
	EXEC SQL COMMIT;
*/	

	test1();
	exit(0);
}


