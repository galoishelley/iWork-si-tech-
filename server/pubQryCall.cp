#include "boss_srv.h"
#include <atmi.h>
#include <fml32.h>

#include "general32.flds.h"
#include "order_xml.h"
#include "pubOrder.h"

#include "commagent.h"
#include "util.h"

#define _DEBUG_
#define GL_MAX_LEN 5000

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

int commAgentCall(char *addr,int port,int appid, int flag, char *transcode, int input_par_num, int output_par_num, char *reqMsg, FBFR32 *recvbuf)
{

	/*******************这里假设需要调用BOSS的一个tuxedo服务，假设服务的名称是s1234**/
	/*******************假设服务s1234的输入参数个数是一个PhoneNumber，输出参数是4个
	分别是RETURN_CODE、ERROR_CODE、ERROR_MSG、PERSON_NAME.*****/

	int             Sockid 		= -1;
	int             ret         = 0;
	int             num         = 0;
	char            endflag     = 0X00;
	LG_Buffer       *pBuffer 	= NULL;
	char recvMsg[GL_MAX_LEN];
	init(recvMsg);

	pBuffer = LG_AllocBuf();
	if(NULL == pBuffer)
	{
		printf("分配通讯内存失败\n");
		return -1001;
	}

	if((Sockid=LG_Connect_Server(addr,port))<0)
	{
		printf("与BOSS建立连接失败\n");
		LG_FreeBuf(pBuffer);
		return -1002;
	}

	ret=LG_SendMsg_Client(Sockid,appid,reqMsg,strlen(reqMsg),flag,20,2,transcode);
	if(ret<0)
	{
		printf("发送请求失败\n");
		LG_FreeBuf(pBuffer);
		LG_Term(Sockid);
		return -1003;
	}

	ret=LG_RecvMsg_Client(Sockid,pBuffer,&num,&endflag);
	if(ret<=0)
	{
		printf("接受请求结果失败\n");
		LG_FreeBuf(pBuffer);
		LG_Term(Sockid);
		return -1004;
	}

	LG_FreeBuf(pBuffer);
	LG_Term(Sockid);

/*	strcpy(recvMsg,(char*)pBuffer);*/
	printf("BOSS 返回结果为： %s\n",(char*)pBuffer);
/* 我们约定应用集成平台返回的参数格式为下面样子。但测试程序没有严格按照约定返回，所以我们在此对其重新赋值 */
	strcpy(recvMsg,"SVC_ERR_NO32=000000,SVC_ERR_MSG32=SUCESS!,GPARM32_0=f1|f2|f3|,GPARM32_1=G1|aaaaa|G3|,GPARM32_2=测3|hh|3dsssssssssssss|");
	printf("重新赋值为： %s\n",recvMsg);

	ret=getRecvMsgtoFML(recvMsg,input_par_num,output_par_num,recvbuf);
	if(ret!=0)
	{
		printf("将输出字符串压入FML 缓冲区失败\n");
		LG_FreeBuf(pBuffer);
		LG_Term(Sockid);
		return -1005;
	}
	return 0;
}

/*
函数功能：将应用集成平台返回的结果串组装成TUXEDO的FML缓冲区信息
*/

int getRecvMsgtoFML(char *recvMsg,int input_par_num, int output_par_num, FBFR32 *recvbuf)
{

	int i=0,ret=0;
	char tempbuf[GL_MAX_LEN];
	char lable_name[16];

	init(tempbuf);
	init(lable_name);

    Fchg32(recvbuf,SEND_PARMS_NUM32,0,(char *)input_par_num,(FLDLEN32)0);
	Fchg32(recvbuf,RECP_PARMS_NUM32,0,(char *)output_par_num,(FLDLEN32)0);

    /*获取SVC_ERR_NO32*/
    init(tempbuf);
    ret=parseRecvMsg(recvMsg,"SVC_ERR_NO32",tempbuf);
    if(0!=ret)
    {
    	return -1002;
    }
    printf("tempbuf[SVC_ERR_NO32]=[%s]\n",tempbuf);
    Fchg32(recvbuf,SVC_ERR_NO32,0,tempbuf,(FLDLEN32)0);

    /*获取SVC_ERR_MSG32*/
    init(tempbuf);
    ret=parseRecvMsg(recvMsg,"SVC_ERR_MSG32",tempbuf);
    if(0!=ret)
    {
    	return -1003;
    }
    printf("tempbuf[SVC_ERR_MSG32]=[%s]\n",tempbuf);
    Fchg32(recvbuf,SVC_ERR_MSG32,0,tempbuf,(FLDLEN32)0);

    /*获取返回参数*/
    for(i=0;i<output_par_num;i++)
    {
    	sprintf(lable_name,"GPARM32_%d",i);
    	init(tempbuf);
    	ret=parseRecvMsg(recvMsg,lable_name,tempbuf);
    	if(0!=ret)
    	{
    		return -1004;
    	}

    	printf("tempbuf[%s]=[%s]\n",lable_name,tempbuf);

		/* 拆分参数，|分割 */
		int ii=0;
		char *p;
		p=strtok(tempbuf,"|");
        while(p!=NULL)
        {
			/*将值压入FML缓冲区*/
            Fchg32(recvbuf,GPARM32_0+i,ii,p,(FLDLEN32)0);
            p=strtok(NULL,"|");
            ii++;
        }
    }

	return 0;
}

/*
函数功能：将FML缓冲区数据组装成应用集成平台所需的输入字符串
         不支持多维输入参数
*/
int setFMLtoRecvMsg(FBFR32 *sendbuf, char *recvMsg)
{
	int i=0, sendNum = 0;
	char tempbuf[256];
	char tempbuf2[256];
	char tempStr[1024];
	init(tempbuf);
	init(tempStr);
	/**********add by zhaodw*****************/
	char TempNumber[256];
	init(TempNumber);
	/****************************************/
	
	/* 获取输入参数个数 */
	init(tempbuf);
    Fget32(sendbuf,SEND_PARMS_NUM32,0,tempbuf,NULL);
    sendNum=atoi(tempbuf);
    /****************add by zhaodw*************/
    sprintf(TempNumber,"inp=%d,",sendNum);
    strcat(tempStr,TempNumber);
    
    init(tempbuf);
    Fget32(sendbuf,RECP_PARMS_NUM32,0,tempbuf,NULL);
    sendNum=atoi(tempbuf);
    init(TempNumber);
    sprintf(TempNumber,"outp=%d,",sendNum);
    strcat(tempStr,TempNumber);
    
    /******************************************/
    
    for(i=0;i<sendNum;i++)
    {
    	/* 获取第i个输入参数 */
    	init(tempbuf);
    	Fget32(sendbuf,GPARM32_0+i,0,tempbuf,NULL);
    	Trim(tempbuf);
    	/*************add by zhaodw*************/
    	LG_AddStr(tempbuf);
    	/***************************************/
    	printf("GPARM32_%d=%s\n",i,tempbuf);
    	
    	/* 将第i个参数拼到应用集成平台字符串末尾 */
    	/*************add by zhaodw**************/
    	sprintf(tempbuf2,"p%d=%s,",i,tempbuf);
    	/****************************************/
    	/*sprintf(tempbuf2,"GPARM32_%d=%s,",i,tempbuf);*/
    	strcat(tempStr,tempbuf2);
    	Trim(tempStr);
    }
    printf("tempStr=%s\n",tempStr);
    strcpy(recvMsg,tempStr);
    
	return 0;
}

/*
函数功能：解析应用集成平台返回结果串中标签的变量值
串形式 ：  SVC_ERR_NO32=000000,SVC_ERR_MSG32=SUCESS,GPARM32_0=p1|p2|p3|,GPARM32_1=p1|p2|p3|,
例如输入SVC_ERR_MSG32，返回SUCESS
*/
int parseRecvMsg(char *recvMsg, char *lable, char *value)
{
    char *tempstr;
    char *midval;
    char tempval[GL_MAX_LEN];
    strcpy(tempval,(char*)recvMsg);

    tempstr=strstr(tempval,lable);
    midval=strtok(tempstr,"=");
    midval=strtok(NULL,",");

    strcpy(value,(char *)midval);
	return 0;
}



/************************************************************************
函数名称:fPubCallSrv
编码时间:2009/7/7
编码人员:
功能描述:本函数集成tpcall和应用集成平台方式的服务调用
入口参数:
	call_type	调用类型（1:tpcall	2:应用集成平台）
	serv_name	服务名
	sendbuf		输入缓冲区
	recvbuf		输出缓冲区
	
返 回 值:0代表正确返回,其他错误
***********************************************************************/
int fPubCallSrv(int call_type,char *serv_name, FBFR32 *sendbuf, FBFR32 **recvbuf)
{
	int ret=0;
	int input_par_num=0;
	int output_par_num=0;
	char tempbuf[256];
	FLDLEN32  rcvlen;
	
	Ffree32(*recvbuf);
	
	/* tpcall方式调用服务 */
	if(call_type==1)
	{
		printf("serv_name=%s\n",serv_name);
		
		/* 获取输出参数个数 */
		init(tempbuf);
    	Fget32(sendbuf,RECP_PARMS_NUM32,0,tempbuf,NULL);
    	output_par_num=atoi(tempbuf);
    	printf("output_par_num=%d\n",output_par_num);
		/*初始化输出缓冲区*/
		/* 存在内存泄露，需注释掉
		rcvlen = (FLDLEN32)((output_par_num)*120);
		if((*recvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
		{
			return -1;
		}
		*/
		long sendlen=Fsizeof32(sendbuf);
     	long reqlen=Fsizeof32(*recvbuf);
        ret = tpcall(serv_name, (char *)sendbuf, 0, (char **)recvbuf, &reqlen, (long)0);
        if(ret != 0)
        {
        	printf("Can't send request to service [%s],Tperrno = [%d]\n",serv_name, tperrno);
        	return -1;
        }
        char get_parm[256];
    	
    	init(get_parm);
    	Fget32(sendbuf,GPARM32_0,0,get_parm,NULL);
    	printf("=========in GPARM32_0=[%s]\n",get_parm);
    	
    	init(get_parm);
    	Fget32(*recvbuf,GPARM32_0,0,get_parm,NULL);
    	printf("=========out GPARM32_0=[%s]\n",get_parm);
    	init(get_parm);
    	Fget32(*recvbuf,GPARM32_1,0,get_parm,NULL);
    	printf("=========out GPARM32_1=[%s]\n",get_parm);
    	init(get_parm);
    	Fget32(*recvbuf,GPARM32_2,0,get_parm,NULL);
    	printf("=========out GPARM32_2=[%s]\n",get_parm);
	}
	
	/* 应用集成平台调用服务 */
	else if(call_type==2)
	{
		int Sockid = -1;
		int num = 0;
		int port = 0;
		int appid = 0;
		char endflag = 0X00;
		LG_Buffer *pBuffer = NULL;
		
		char recvMsg[GL_MAX_LEN];
		char addr[15+1];
		char sendStr[8000];
		
		init(addr);
		init(sendStr);
		init(recvMsg);
		
		/* 获取输入参数个数 */
		init(tempbuf);
    	Fget32(sendbuf,SEND_PARMS_NUM32,0,tempbuf,NULL);
    	input_par_num=atoi(tempbuf);
    	/* 获取输出参数个数 */
		init(tempbuf);
    	Fget32(sendbuf,RECP_PARMS_NUM32,0,tempbuf,NULL);
    	output_par_num=atoi(tempbuf);
    	
		/* 需要将输入FML buffer拆解组装为应用集成平台识别的字符串 */
		ret = setFMLtoRecvMsg(sendbuf, sendStr);
		/*******************add by zhaodw**************/
		sprintf(sendStr,"route_code=13,route_key=10200010419523,service_name=%s,%s",serv_name,sendStr);
		/************************************************/
		/* 获取服务的appid，ip，port */
		EXEC SQL SELECT app_id,ip_addr,port
		           INTO :appid,:addr,:port
		           FROM sCommonAgentList
		          WHERE serv_name=:serv_name;
		if(0!=SQLCODE)
		{
			printf("获取服务信息失败\n");
			return -1001;
		}
		Trim(addr);

		pBuffer = LG_AllocBuf();
		if(NULL == pBuffer)
		{
			printf("分配通讯内存失败\n");
			return -1001;
		}
	
		if((Sockid=LG_Connect_Server(addr,port))<0)
		{
			printf("与BOSS建立连接失败\n");
			LG_FreeBuf(pBuffer);
			return -1002;
		}
	
		ret=LG_SendMsg_Client(Sockid,appid,sendStr,strlen(sendStr),MSGBEGIN|MSGEND,20,2,serv_name);
		if(ret<0)
		{
			printf("发送请求失败\n");
			LG_FreeBuf(pBuffer);
			LG_Term(Sockid);
			return -1003;
		}
	
		ret=LG_RecvMsg_Client(Sockid,pBuffer,&num,&endflag);
		if(ret<=0)
		{
			printf("接受请求结果失败\n");
			LG_FreeBuf(pBuffer);
			LG_Term(Sockid);
			return -1004;
		}
	
		LG_FreeBuf(pBuffer);
		LG_Term(Sockid);
	
		strcpy(recvMsg,(char*)pBuffer);
		printf("BOSS 返回结果为： %s\n",(char*)pBuffer);
	
		ret=getRecvMsgtoFML(recvMsg,input_par_num,output_par_num,*recvbuf);
		if(ret!=0)
		{
			printf("将输出字符串压入FML 缓冲区失败\n");
			LG_FreeBuf(pBuffer);
			LG_Term(Sockid);
			return -1005;
		}
		return 0;
	
	
	}
	
	else
	{
		printf("输入调用类型错误\n");
		return 1403;
	}
	return 0;
}
