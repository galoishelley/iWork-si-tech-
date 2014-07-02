/************应用集成平台-通讯转发程序API函数头文件**************/
/************作者：赵东伟 2009-2-26******************************/
/************版本号：01******************************************/
#ifndef _UTIL_H
#define _UTIL_H

#define     MAXDLEN     	8000

#define     MSGBEGIN        0x00000001
#define     MSGCONTINUE     0x00000010
#define     MSGEND          0x00000100
#define     MSGCHECK        0x00000110
typedef struct lg_buffer
{
	char m_data[MAXDLEN+1];
}LG_Buffer;

 LG_Buffer* 			LG_AllocBuf();
 int 				LG_Connect_Server(const char *,int);
 int 				LG_FreeBuf(LG_Buffer*);
 int 				LG_RecvMsg_Client(int ,LG_Buffer *,int *,char *);
 int 				LG_SendMsg_Client(int,int,const char*,int,int,int,int,const char*);
 int 				LG_Service_Register(const char *,int ,int,int );
 void 			LG_Term(int);
 int 				LG_RecvMsg_Server(int,LG_Buffer*,int *,char *);
 int 				LG_SendMsg_Server(int,const char*,int,int);
 int 				parsepara(FILE *, char *, char *);
 int 				atoni(const char *,int );
 int 				stop_conn();
 int 				conn_server(char *,int,int);
 int 				send_msg_cli(int,int,char *,int ,int,int);
 int 				recv_msg_cli(int *,char *);
 int 				conn_agent(const char *,int,int,int,int);
 int 				recv_msg_srv(int *);
 int 				recv_ready();
 int 				send_msg_srv(int,int,int);

#endif
