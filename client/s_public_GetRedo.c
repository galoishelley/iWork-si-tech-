#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/timeb.h>
#include "publicsrv.h"

main()
{
	int i;
	char tempstr[LINE];
	char shmfile[LINE];
	struct redoShm *shmp;
    
	spublicGetCfgDir(REDOSHMKEYFILE, tempstr);
	strcpy(shmfile,tempstr);

	/* 打开共享内存 */
	shmp = redoOpenShm( ftok( shmfile,1) );
	if( shmp== (struct redoShm *)NULL)
	{
		printf("Open redo shm fail!\n");
		exit(-1);
	}
	/* 给分配完任务的进程赋值 */
	for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
	{
		printf("--------------------------------------------\n");
		printf("shmp->memAccept[%d]=[%s],shmp->redoPosition[%d]=[%d],\nshmp->redoFile[%d]=[%s], shmp->svcName[%d]=[%s],\nshmp->phoneNo[%d]=[%s], shmp->flag[%d]=[%d], \nshmp->endFlag=[%d]\n",
			 i,shmp->memAccept[i],
			 i,shmp->redoPosition[i],
			 i,shmp->redoFile[i],
			 i,shmp->svcName[i],
			 i,shmp->phoneNo[i],
			 i,shmp->flag[i],
			 shmp->endFlag);
	}
}
