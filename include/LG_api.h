/************应用集成平台-通讯转发程序API函数头文件**************/
/************作者：赵东伟 2009-2-26******************************/
/************版本号：01******************************************/
#ifndef _LG_API_H
#define _LG_API_H

#define     MAXDLEN     	8000
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
void 				LG_Term(int);
int 				LG_RecvMsg_Server(int,LG_Buffer*,int *,char *);
int 				LG_SendMsg_Server(int,const char*,int,int);
int 				getConfpath(char *);
int 				convertpath(char *);
int 				LG_Connect_Buss(const char*);
char* 				ALLTrim(char *);
/* added by liupengc 2009-10-23 10:57:27 */
int LG_Getinter(char *newaddress,int appid,int priority,char *transcode,char *inmsg,char *outmsg);
/* Note that Getinter or LG_Getinter */
#endif
