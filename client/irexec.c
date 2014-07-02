/*------------------------------------------------------------------------------
       compile:cc -o irexec irexec.c -lsocket
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>                                             
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

main(int argc,char *argv[])
{
        struct servent *ser;
        int  i,fd2p=0,s;
        char com[128],*rhost[]={"                       "},user[50],password[50];
        char rtnStr[255]="\0";
        FILE *fp;


        if(argc<4)
        {
                printf("LocalHost Message:\n	Usage:./irexec hostname user password command\n");
                exit(1);
        }

        umask(000);

        memset(user,0,sizeof(user));
        memset(password,0,sizeof(password));

        strcpy(*rhost,argv[1]);
        strcpy(user,argv[2]);
        strcpy(password,argv[3]);

        if(argc>199) argc=199;
        for(i=4;i<argc;i++)
        {
                strcat(com,argv[i]);
                strcat(com," ");
        }

        ser=getservbyname("exec","tcp");
        s=rexec(rhost,ser->s_port,user,password,com,&fd2p);
        if(s<0) 
        {
                printf("rexec excute failed!\n");
                exit(1);
        }

        fp=fdopen(s,"r");
        while(!feof(fp))
        {
                while(fgets(rtnStr,sizeof(rtnStr),fp) != NULL)
                {
                        printf("%s",rtnStr);
                }
        }
        fclose(fp);
}
