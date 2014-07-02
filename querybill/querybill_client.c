#include "querybill.h"
#include "rtPay.h"

int TimeOut;
int comm_sockid = -2;
char serv_ip[32];
int serv_port;

int isphone(phoneno)
char *phoneno;
{
  int val;

  if(strlen(phoneno)!=11)
    return -1;

  if(phoneno[0]!='1' || phoneno[1]!='3')
    return -1;

  val=atoi(phoneno+2);
  if(val<500000000 || val>999999999)
    return -1;

  return 0;
}

void catchalm(sig)
int sig;
{
  signal(sig,SIG_IGN);
  TimeOut=1;

  return;
}


int querybill_info(type, contract_no, id_no, phoneno, ip, port, timeout, db, owe_fee, should_pay, favour_fee)
int type;
long contract_no;
long id_no;
char *phoneno;
char *ip;
int port;
int timeout;
char *db;
double *owe_fee;
double *should_pay;
double *favour_fee;
{
  struct sockaddr_in srv_addr;
  char buf[MSGLEN],Pay[64],should[64],favour[64];
  struct querybill    *datainfo;
  int ret_no;

  ret_no=0;
  TimeOut=0;
  signal(SIGALRM,catchalm);
  alarm(timeout);

printf("timeout=======%d\n",timeout);
printf("comm_sockid===%d\n",comm_sockid);

  if(strcmp(serv_ip,ip) || comm_sockid<0)
  {
    if(comm_sockid>=0)
      close(comm_sockid);

printf("stat 1 \n");
    memset(&srv_addr,0x0,sizeof(srv_addr));
    srv_addr.sin_family=AF_INET;
    srv_addr.sin_addr.s_addr=inet_addr(ip);
    srv_addr.sin_port=htons(port);

    comm_sockid=socket(AF_INET,SOCK_STREAM,0);
    if(comm_sockid<0)
    {
      return -1000;
    }

    if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
    {
      if(comm_sockid>=0)
        close(comm_sockid);

      comm_sockid=-1;

      if(TimeOut)
      {
        return -1001;
      }
      else
      {
        srv_addr.sin_port=htons(port+100);

        if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
        {
          if(comm_sockid>=0)
            close(comm_sockid);

          comm_sockid=-1;

          if(TimeOut)
          {
            return -1001;
          }
          else
          {
            return -1002;
          }
        }
      }
    }
  }

  strcpy(serv_ip,ip);
  serv_port=port;

  while(1)
  {
printf("stat 2\n");
    memset(buf,0x0,sizeof(buf));
    sprintf(buf,"%d %ld %ld %s %s",type,contract_no,id_no,phoneno,db);

    if(write(comm_sockid,buf,MSGLEN)!=MSGLEN)
    {
printf("stat 3 [errno==%d]\n",errno);
      if(comm_sockid>=0)
        close(comm_sockid);

      comm_sockid=-1;

      if(TimeOut)
      {
        ret_no = -1011;
        break;
      }

      memset(&srv_addr,0x0,sizeof(srv_addr));
      srv_addr.sin_family=AF_INET;
      srv_addr.sin_addr.s_addr=inet_addr(ip);
      srv_addr.sin_port=htons(port);

      comm_sockid=socket(AF_INET,SOCK_STREAM,0);
      if(comm_sockid<0)
      {
        ret_no = -1000;
        break;
      }

      if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
      {
        if(comm_sockid>=0)
          close(comm_sockid);

        comm_sockid=-1;

        if(TimeOut)
        {
          ret_no = -1001;
          break;
        }
        else
        {
          srv_addr.sin_port=htons(port+100);

          if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
          {
            if(comm_sockid>=0)
              close(comm_sockid);

            comm_sockid=-1;

            if(TimeOut)
            {
              ret_no = -1001;
              break;
            }
            else
            {
              ret_no = -1002;
              break;
            }
          }
        }
      }

      continue;
    }

    memset(Pay,0x0,sizeof(Pay));
    memset(should,0x0,sizeof(should));
    memset(favour,0x0,sizeof(favour));

    memset(buf,0x0,sizeof(buf));
    if(read(comm_sockid,buf,MSGLEN)!=MSGLEN)
    {
printf("stat 3 [errno==%d]\n",errno);

      if(comm_sockid>=0)
        close(comm_sockid);

      comm_sockid=-1;

      if(TimeOut)
      {
        ret_no = -1021;
        break;
      }

      memset(&srv_addr,0x0,sizeof(srv_addr));
      srv_addr.sin_family=AF_INET;
      srv_addr.sin_addr.s_addr=inet_addr(ip);
      srv_addr.sin_port=htons(port);

      comm_sockid=socket(AF_INET,SOCK_STREAM,0);
      if(comm_sockid<0)
      {
        ret_no = -1000;
        break;
      }

      if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
      {
        if(comm_sockid>=0)
          close(comm_sockid);

        comm_sockid=-1;

        if(TimeOut)
        {
          ret_no = -1001;
          break;
        }
        else
        {
          srv_addr.sin_port=htons(port+100);

          if(connect(comm_sockid,(struct sockaddr *)&srv_addr,sizeof(srv_addr))<0)
          {
            if(comm_sockid>=0)
              close(comm_sockid);

            comm_sockid=-1;

            if(TimeOut)
            {
              ret_no = -1001;
              break;
            }
            else
            {
              ret_no = -1002;
              break;
            }
          }
        }
      }

      continue;
    }

    break;
  }

  alarm(0);
  signal(SIGALRM,SIG_IGN);

  if(ret_no<0)
    return ret_no;

  sscanf(buf,"%d%s%s%s",&ret_no,Pay,should,favour);
  if(owe_fee)
    *owe_fee=atof(Pay);
  if(should_pay)
    *should_pay=atof(should);
  if(favour_fee)
    *favour_fee=atof(favour);

  return ret_no;
}
