#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fml32.h>             
#include <atmi.h>      
#include "general32.flds.h"
#include "ferror32.h"

#if defined(__STDC__) || defined(__cplusplus)
main(int argc, char *argv[])
#else

main(argc, argv)
int argc;
char *argv[];
#endif
{
	FBFR32*		sendbuf;
	FBFR32*		rcvbuf;
	FLDLEN32	sendlen;
	FLDLEN32	rcvlen;
	int		i;
	int		j;
	int		occs;
	int		sendNum;
	int		recvNum;
	int		ret;
	long		reqlen;
	char		serverName[32];
	char		buffer[256];

	if(argc < 4)
	{
		(void) fprintf(stderr, "Usage: cl32 serverName, SEND_PARMS_NUM RECP_PARMS_NUM, arg1, arg2,...\n");
		exit(1);
	}

	memset(serverName, 0, sizeof(serverName));

	strcpy(serverName, argv[1]);
	sendNum = atoi(argv[2]);
	recvNum = atoi(argv[3]);

	/* Attach to System/T as a Client Process */
	if (tpinit((TPINIT *) NULL) == -1)
	{
		(void) fprintf(stderr, "Tpinit failed\n");
		exit(1);
	}

	sendlen = (FLDLEN32)((sendNum)*100);

	if((sendbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, sendlen)) == NULL)
	{
		(void) fprintf(stderr,"Error allocating send buffer\n");
		tpterm();
		exit(1);
	}

	Fchg32(sendbuf,SEND_PARMS_NUM32,0,argv[2],(FLDLEN32)0); 
	Fchg32(sendbuf,RECP_PARMS_NUM32,0,argv[3],(FLDLEN32)0);

	for(i = 0; i < sendNum; i ++)
	{
		printf("SEND GPARM32_[%d]=[%s]\n", i, argv[4+i]);

		SplitSendChg(sendbuf, argv[4], argv[5 + i], i);
/**WHL note out for support array style of cl32 input params 
**		Fchg32(sendbuf,GPARM32_0 + i,0,argv[4+i],(FLDLEN32)0);
**/
	}
        
	rcvlen = (FLDLEN32)((recvNum)*100);
	if((rcvbuf = (FBFR32 *) tpalloc(FMLTYPE32, NULL, rcvlen)) == NULL)
	{
		(void) fprintf(stderr,"Error allocating receive buffer\n");
		tpfree((char *)sendbuf);
		tpterm();
		exit(1);
	}

	reqlen=Fsizeof32(rcvbuf);
	ret = tpcall(serverName, (char *)sendbuf, 0, (char **)&rcvbuf, &reqlen, (long)0);
	if(ret == -1)
	{
		(void) fprintf(stderr, "Can't send request to service TOUPPER\n");
		(void) fprintf(stderr, "Tperrno = %d\n", tperrno);
		tpfree((char *)sendbuf);
		tpfree((char *)rcvbuf);
		tpterm();
		exit(1);
	}

	printf("++++ tpcall succ !!! ++++\n");
	printf("++++++++++++++++++++++++++++++\n");
	memset(buffer,0,sizeof(buffer));
	Fget32(rcvbuf,SVC_ERR_NO32,0,buffer,NULL);
	printf("++ SVC_ERR_NO32  = [%s] \n", buffer);
	memset(buffer,0,sizeof(buffer));
	Fget32(rcvbuf,SVC_ERR_MSG32,0,buffer,NULL);
	printf("++ SVC_ERR_MSG32 = [%s] \n", buffer);
	memset(buffer,0,sizeof(buffer));
	Fget32(rcvbuf,SVC_ACCEPT32,0,buffer,NULL);
	printf("++ SVC_ACCEPT32  = [%s] \n", buffer);
	printf("++++++++++++++++++++++++++++++\n");

	memset(buffer, '\0', sizeof(buffer));
	for(i = 0; i < recvNum; i ++)
	{
		occs = 0;
		occs = Foccur32(rcvbuf, GPARM32_0 + i);
		if(occs > 1)
		{
			for(j = 0; j < occs; j++)
			{
				Fget32(rcvbuf,GPARM32_0+i,j,buffer,NULL);
				printf("+RECV GPARM32_[%d][%d]=[%s]\n", i, j, buffer);
			}
		}
		else
		{
			Fget32(rcvbuf,GPARM32_0+i,0,buffer,NULL);
			printf("RECV GPARM32_[%d]=[%s]\n", i, buffer);
			memset(buffer, '\0', sizeof(buffer));
		}
	}

	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	tpterm();
	return(0);
}

int SplitSendChg(FBFR32* sendbuf, char spilt, char SrcBuff[], int loop)
{
	int		j = 0;
	char		tmpbuf[10 * 1024 + 1];
	char		tmpItem[10][1024 + 1];
	char		cnt = 0;

	memset(tmpbuf, 0, sizeof(tmpbuf));

	strcpy(tmpbuf, SrcBuff);

	SplitBuff2Array(tmpbuf, &tmpItem[0][0], sizeof(tmpItem[10]), spilt, &cnt);
	
	if(cnt != 1)
	{
		for(j = 0; j < cnt; j++)
		{
			Fchg32(sendbuf,GPARM32_0 + loop, j, tmpItem[j], (FLDLEN32)0);
		}
	}
	else
	{
		Fchg32(sendbuf,GPARM32_0 + loop, 0, SrcBuff, (FLDLEN32)0);
	}
}

static int SplitBuff2Array(char SrcBuff[], char* DescArray, int offset, char Split, int* pCnt)
{
	int		loop = 0;
	int		loop_2 = 0;
	int		count = 0;
	char*		pb = SrcBuff;
	char*		pe = NULL;
	char		tmp[1023 + 1];

	for(loop = 0; loop < strlen(SrcBuff); loop++)
	{
		if(SrcBuff[loop] == Split)
		{
			pe = &SrcBuff[loop];
			strncpy(DescArray + offset * count, pb, pe - pb);

			for(loop_2 = loop + 1; loop_2 < strlen(SrcBuff); loop_2++)
			{
				if(SrcBuff[loop_2] != ' ')
				{
					pb = &SrcBuff[loop_2];
					break;
				}
			}
			count++;
		}
	}

	pe = &SrcBuff[loop];
	strncpy(DescArray + offset * count, pb, pe - pb);
	count++;

	*pCnt = count;

	return 0;
}
