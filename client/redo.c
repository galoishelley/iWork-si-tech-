/*
**    ��������redo.c
**  ����������iBOSS��������б���ϵͳ����־�����ָ�
**  ʹ�÷�����redo startTime endTime
**        �磺redo 20030115000000 20030115190000
**  �������ڣ�2003.01.19
**  ����汾: V30.2003.01.20
**    �����ˣ�dangsl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fml32.h>
#include <fml.h>
#include <sys/wait.h>
#include <sys/timeb.h>
#include "atmi.h"
#include "general.flds.h"
#include "general32.flds.h"
#include "publicsrv.h"

#define REDODIR "REDODIR"
#define FILENAME  100
#define ACCEPTLEN 100
#define _DEBUG_
#define _DEBUGDETAIL_
#define _DEBUGDETAIL1_

extern struct redoShm * redoOpenShm(key_t shmkey);

struct idxRecord
{
	char memAccept[REDOPROCESSNUM][ACCEPTLEN];
	long redoPosition[REDOPROCESSNUM];
	char redoFile[REDOPROCESSNUM][100];
	char svcName[REDOPROCESSNUM][20];
	char phoneNo[REDOPROCESSNUM][20];
	int  processNo[REDOPROCESSNUM];
};

char workDIR[50];
int  redoType=0;

/* ϵͳ�д���FML��FML32���ͱ���, ����Ҫ���� */
FBFR32 *obuf32;
FBFR   *obuf;

/* ����Buffer */
char   *rcvbuf;

struct redoShm *shmp;

void QUIT(int signo)
{
	tpfree((char *)obuf32);
	tpfree((char *)obuf);
	tpfree((char *)rcvbuf);
	shmp->endFlag=2;
	DelRedoShm();
	userlog("�����û��ж�!\n");
        exit(0);
}

int readTransINBuf(FBFR *obuf,FILE *fp)
{
	char bufId[20],bufVal[4096];
	char bufValChar;
	int  i,id,endFlag;
	long fldId,pos;
	
	while(1)
	{
		memset(bufId,0,sizeof(bufId));
		memset(bufVal,0,sizeof(bufVal));
		
		if(endFlag==1)break;
		if(fscanf(fp,"%s\t",bufId)==-1) return -1;

		if(strcmp(bufId,"SEND_PARMS_NUM")==0)id=51;
		else if(strcmp(bufId,"RECP_PARMS_NUM")==0)id=52;
		else id=atoi(strtok(bufId,"GPARM_"));

		if(strlen(bufId)==0) break;
		if(strcmp(strtok(bufId,"FLDID"),"(")==0)break; 
		if((bufValChar=fgetc(fp))==-1)return -2;
		i=0;
		while(bufValChar!='\n')
		{
			bufVal[i]=bufValChar;
			i++;
			if((bufValChar=fgetc(fp))==-1)return -3;
		}
		bufVal[i]='\0';

		pos=ftell(fp);

		bufValChar=fgetc(fp);
		if(bufValChar=='\n')
			endFlag=1;
		else 
			fseek(fp,pos,SEEK_SET);

		switch(id)
		{
			case 0:
				fldId=GPARM_0;
				break;
			case 1:
				fldId=GPARM_1;
				break;
			case 2:
				fldId=GPARM_2;
				break;
			case 3:
				fldId=GPARM_3;
				break;
			case 4:
				fldId=GPARM_4;
				break;
			case 5:
				fldId=GPARM_5;
				break;
			case 6:
				fldId=GPARM_6;
				break;
			case 7:
				fldId=GPARM_7;
				break;
			case 8:
				fldId=GPARM_8;
				break;
			case 9:
				fldId=GPARM_9;
				break;
			case 10:
				fldId=GPARM_10;
				break;
			case 11:
				fldId=GPARM_11;
				break;
			case 12:
				fldId=GPARM_12;
				break;
			case 13:
				fldId=GPARM_13;
				break;
			case 14:
				fldId=GPARM_14;
				break;
			case 15:
				fldId=GPARM_15;
				break;
			case 16:
				fldId=GPARM_16;
				break;
			case 17:
				fldId=GPARM_17;
				break;
			case 18:
				fldId=GPARM_18;
				break;
			case 19:
				fldId=GPARM_19;
				break;
			case 20:
				fldId=GPARM_20;
				break;
			case 21:
				fldId=GPARM_21;
				break;
			case 22:
				fldId=GPARM_22;
				break;
			case 23:
				fldId=GPARM_23;
				break;
			case 24:
				fldId=GPARM_24;
				break;
			case 25:
				fldId=GPARM_25;
				break;
			case 26:
				fldId=GPARM_26;
				break;
			case 27:
				fldId=GPARM_27;
				break;
			case 28:
				fldId=GPARM_28;
				break;
			case 29:
				fldId=GPARM_29;
				break;
			case 30:
				fldId=GPARM_30;
				break;
			case 31:
				fldId=GPARM_31;
				break;
			case 32:
				fldId=GPARM_32;
				break;
			case 33:
				fldId=GPARM_33;
				break;
			case 34:
				fldId=GPARM_34;
				break;
			case 35:
				fldId=GPARM_35;
				break;
			case 36:
				fldId=GPARM_36;
				break;
			case 37:
				fldId=GPARM_37;
				break;
			case 38:
				fldId=GPARM_38;
				break;
			case 39:
				fldId=GPARM_39;
				break;
			case 40:
				fldId=GPARM_40;
				break;
			case 41:
				fldId=GPARM_41;
				break;
			case 42:
				fldId=GPARM_42;
				break;
			case 43:
				fldId=GPARM_43;
				break;
			case 44:
				fldId=GPARM_44;
				break;
			case 45:
				fldId=GPARM_45;
				break;
			case 46:
				fldId=GPARM_46;
				break;
			case 47:
				fldId=GPARM_47;
				break;
			case 48:
				fldId=GPARM_48;
				break;
			case 49:
				fldId=GPARM_49;
				break;
			case 50:
				fldId=GPARM_50;
				break;
			case 51:
				fldId=SEND_PARMS_NUM;
				break;
			case 52:
				fldId=RECP_PARMS_NUM;
				break;
			default:
				fldId=-1;
				break;
		}
		if(fldId==-1)return -4;
		if(Fchg(obuf, fldId, 0, bufVal, 0)==-1) return -5;
		userlog("pid[%d] fldId[%d] bufVal[%s]\n",getpid(),fldId,bufVal);
	}
	return 0;
}

int readTransINBuf32(FBFR32 *obuf32,FILE *fp)
{
	char bufId[20],bufVal[4096];
	char bufValChar;
	int  i,id,endFlag;
	long fldId,pos;
	
	while(1)
	{
		memset(bufId,0,sizeof(bufId));
		memset(bufVal,0,sizeof(bufVal));
		
		if(endFlag==1)break;
		if(fscanf(fp,"%s\t",bufId)==-1) return -1;

		if(strcmp(bufId,"SEND_PARMS_NUM32")==0)id=51;
		else if(strcmp(bufId,"RECP_PARMS_NUM32")==0)id=52;
		else id=atoi(strtok(bufId,"GPARM32_"));

		if(strlen(bufId)==0) break;
		if(strcmp(strtok(bufId,"FLDID"),"(")==0)break; 
		if((bufValChar=fgetc(fp))==-1)return -2;
		i=0;
		while(bufValChar!='\n')
		{
			bufVal[i]=bufValChar;
			i++;
			if((bufValChar=fgetc(fp))==-1)return -3;
		}
		bufVal[i]='\0';

		pos=ftell(fp);

		bufValChar=fgetc(fp);
		if(bufValChar=='\n')
			endFlag=1;
		else 
			fseek(fp,pos,SEEK_SET);

		switch(id)
		{
			case 0:
				fldId=GPARM32_0;
				break;
			case 1:
				fldId=GPARM32_1;
				break;
			case 2:
				fldId=GPARM32_2;
				break;
			case 3:
				fldId=GPARM32_3;
				break;
			case 4:
				fldId=GPARM32_4;
				break;
			case 5:
				fldId=GPARM32_5;
				break;
			case 6:
				fldId=GPARM32_6;
				break;
			case 7:
				fldId=GPARM32_7;
				break;
			case 8:
				fldId=GPARM32_8;
				break;
			case 9:
				fldId=GPARM32_9;
				break;
			case 10:
				fldId=GPARM32_10;
				break;
			case 11:
				fldId=GPARM32_11;
				break;
			case 12:
				fldId=GPARM32_12;
				break;
			case 13:
				fldId=GPARM32_13;
				break;
			case 14:
				fldId=GPARM32_14;
				break;
			case 15:
				fldId=GPARM32_15;
				break;
			case 16:
				fldId=GPARM32_16;
				break;
			case 17:
				fldId=GPARM32_17;
				break;
			case 18:
				fldId=GPARM32_18;
				break;
			case 19:
				fldId=GPARM32_19;
				break;
			case 20:
				fldId=GPARM32_20;
				break;
			case 21:
				fldId=GPARM32_21;
				break;
			case 22:
				fldId=GPARM32_22;
				break;
			case 23:
				fldId=GPARM32_23;
				break;
			case 24:
				fldId=GPARM32_24;
				break;
			case 25:
				fldId=GPARM32_25;
				break;
			case 26:
				fldId=GPARM32_26;
				break;
			case 27:
				fldId=GPARM32_27;
				break;
			case 28:
				fldId=GPARM32_28;
				break;
			case 29:
				fldId=GPARM32_29;
				break;
			case 30:
				fldId=GPARM32_30;
				break;
			case 31:
				fldId=GPARM32_31;
				break;
			case 32:
				fldId=GPARM32_32;
				break;
			case 33:
				fldId=GPARM32_33;
				break;
			case 34:
				fldId=GPARM32_34;
				break;
			case 35:
				fldId=GPARM32_35;
				break;
			case 36:
				fldId=GPARM32_36;
				break;
			case 37:
				fldId=GPARM32_37;
				break;
			case 38:
				fldId=GPARM32_38;
				break;
			case 39:
				fldId=GPARM32_39;
				break;
			case 40:
				fldId=GPARM32_40;
				break;
			case 41:
				fldId=GPARM32_41;
				break;
			case 42:
				fldId=GPARM32_42;
				break;
			case 43:
				fldId=GPARM32_43;
				break;
			case 44:
				fldId=GPARM32_44;
				break;
			case 45:
				fldId=GPARM32_45;
				break;
			case 46:
				fldId=GPARM32_46;
				break;
			case 47:
				fldId=GPARM32_47;
				break;
			case 48:
				fldId=GPARM32_48;
				break;
			case 49:
				fldId=GPARM32_49;
				break;
			case 50:
				fldId=GPARM32_50;
				break;
			case 51:
				fldId=SEND_PARMS_NUM32;
				break;
			case 52:
				fldId=RECP_PARMS_NUM32;
				break;
			default:
				fldId=-1;
				break;
		}
		if(fldId==-1)return -4;
		if(Fchg32(obuf32, fldId, 0, bufVal, 0)==-1) return -5;
	}
	return 0;
}


/* ����������־�ļ��������ļ����ӿ�ִ���ٶ� */
long creatIndexFile(char *startTime,char *endTime)
{
	FILE *cfgFP,*logFP,*idxFP;
	char cfmAccept[100],title[2048],svcName[20],phoneNo[20];
	char fileName[FILENAME+FILENAME],tempCMD[FILENAME+FILENAME];
	char tempIdxFile[FILENAME+FILENAME];
	char cfgFile[FILENAME+FILENAME],logFile[FILENAME+FILENAME];
	char idxFile[FILENAME+FILENAME];

	/* ��¼�ܹ���Ҫ����ļ�¼���� */
	long count=0,idxPosition;

	memset(cfmAccept,0,sizeof(cfmAccept));
	memset(title,0,sizeof(title));
	memset(svcName,0,sizeof(svcName));
	memset(phoneNo,0,sizeof(phoneNo));
	memset(fileName,0,sizeof(fileName));
	memset(cfgFile,0,sizeof(cfgFile));
	memset(idxFile,0,sizeof(idxFile));
	memset(logFile,0,sizeof(logFile));
	memset(tempCMD,0,sizeof(tempCMD));
	memset(tempIdxFile,0,sizeof(tempIdxFile));

	if(strlen(workDIR)==0)
	{
		printf("��������$REDODIR��ֵΪNULL!\n");
		userlog("��������$REDODIR��ֵΪNULL!\n");
		return -1;
	}

	strcpy(cfgFile,workDIR);
	strcat(cfgFile,"/");
	strcat(cfgFile,"cfg/svrLogFile.cfg");

	strcpy(idxFile,workDIR);
	strcat(idxFile,"/");
	strcat(idxFile,"log/IndexFile.tmp");

	strcpy(tempIdxFile,idxFile);

	if(!(cfgFP=fopen(cfgFile,"r")))
	{
		userlog("�������ļ� %s ����!\n",cfgFile);
		return -1;
	}

	if(!(idxFP=fopen(idxFile,"w+")))
	{
		userlog("�������ļ� %s ����!\n",idxFile);
		return -2;
	}

	/* �������ļ�$REDODIR/cfg/svrLogFile.cfg */
	while(!feof(cfgFP))
	{
		memset(fileName,0,sizeof(fileName));

		/* ����$REDODIR/cfg/svrLogFile.cfg�����ļ����ҷ��������ļ�¼ */
		if(fscanf(cfgFP,"%s",fileName)<0)
		{
			if(!(feof(cfgFP)))
			{
				userlog("�������ļ� %s ����!\n",cfgFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -3;
			}
			else
			{
				userlog("�������ļ� %s ����!\n",cfgFile);
				fclose(cfgFP);
				fclose(idxFP);
				break;
			}
		}
		else 
		{
			memset(logFile,0,sizeof(logFile));
			memset(tempCMD,0,sizeof(tempCMD));

			strcpy(tempCMD,"log/");
			strcat(tempCMD,fileName);

			strcpy(logFile,workDIR);
			strcat(logFile,"/");
			strcat(logFile,tempCMD);


			/* ���������ļ��е��ļ�����������־�ļ���
			   ��ѯ�������������ļ�¼ */
			if(!(logFP=fopen(logFile,"r")))
			{
				userlog("��������־�ļ� %s ����!\n",logFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -4;
			}
			else
			{
				/* ��������־�ļ� */
				while(!(feof(logFP)))
				{
					memset(title,0,sizeof(title));
					if(fscanf(logFP,"%s",title)<0)
					{
						if(!(feof(logFP)))
						{
							userlog("��ȡ������־�ļ� %s ����!\n",logFile);
							fclose(cfgFP);
							fclose(idxFP);
							fclose(logFP);
							return -5;
						}
						else
						{
							userlog("��ȡ������־�ļ� %s ����!\n",logFile);
							fclose(logFP);
							break;
						}
					}
					/* ���ҵ�<TitleMessage>�����־ */
					else if(strcmp(title,"<TitleMessage>")==0) 
					{
						memset(cfmAccept,0,sizeof(cfmAccept));
						/* ��ȡ�ڴ���ˮ */
						if(fscanf(logFP,"%s",cfmAccept)<0)
						{
							if(!(feof(logFP)))
							{
								userlog("��ȡ������־�ļ� %s ����!\n",logFile);
								fclose(cfgFP);
								fclose(idxFP);
								fclose(logFP);
								return -6;
							}
							else
							{
								userlog("��ȡ������־�ļ� %s ����!\n",logFile);
								fclose(logFP);
								break;
							}
						}
						else 
						{
							idxPosition=ftell(logFP);
							/* ��ʱ�䷶Χ�ڵļ�¼ */
							if((strcmp(cfmAccept,startTime)>=0)&&
								(strcmp(cfmAccept,endTime)<=0))
							{
								if(fscanf(logFP,"%s",svcName)>0)
								{
									if(strcmp(svcName,"s1300Cfm")==0)
									{
										while(!feof(logFP))
										{
											memset(phoneNo,0,sizeof(phoneNo));
											fscanf(logFP,"%s",phoneNo);
											if(strcmp(phoneNo,"GPARM_5")==0){memset(phoneNo,0,sizeof(phoneNo));fscanf(logFP,"%s",phoneNo);break;}
										}
									}
									else if(strcmp(svcName,"s1300_Valid1")==0||strcmp(svcName,"s1300BaseMsg")==0)
									{
										while(!feof(logFP))
										{
											memset(phoneNo,0,sizeof(phoneNo));
											fscanf(logFP,"%s",phoneNo);
											if(strcmp(phoneNo,"GPARM_0")==0){memset(phoneNo,0,sizeof(phoneNo));fscanf(logFP,"%s",phoneNo);break;}
										}
									}
									else
									{
										memset(phoneNo,0,sizeof(phoneNo));
										strcpy(phoneNo,"-");
									}
								}
								/* ���ڽ��ѵ����⴦����� */

								#ifdef _DEBUG_
								userlog("%s %ld %s %s %s\n",cfmAccept,ftell(logFP),logFile,svcName,phoneNo);
								#endif

								/* ����¼д�������ļ� */
								fprintf(idxFP,"%s %ld %s %s %s\n",
									cfmAccept,idxPosition,logFile,
									svcName,phoneNo);
								count++;
							}
							/* �ж��Ƿ���������ļ�¼������� */
						}
						/* ���ڴ���ˮ������� */
					}
					/* ����<TitleMessage>��־������� */
				}
				/* �ж϶�������־�ļ��Ƿ���� */
			}
			/* ��������־�ļ� */
		}
		/* ����$REDODIR/cfg/svrLogFile.cfg�����ļ����ҷ���
		   �����ļ�¼������� */
	}
	/* ѭ����$REDODIR/cfg/svrLogFile.cfg�ļ�������� */

	memset(tempCMD,0,sizeof(tempCMD));
	strcpy(tempCMD,"sort ");
	strcat(tempCMD,tempIdxFile);

	memset(idxFile,0,sizeof(idxFile));

	strcpy(idxFile,workDIR);
	strcat(idxFile,"/");
	strcat(idxFile,"log/IndexFile");

	strcat(tempCMD," -o ");
	strcat(tempCMD,idxFile);
	/* �����������ʽ�����ļ�$REDODIR/log/IndexFile */
	system(tempCMD);

	printf("���������ļ�[%s]�ɹ���� ...  \n",idxFile);
	userlog("���������ļ�[%s]�ɹ���� ... \n",idxFile);

	/* ���ط������������ļ�¼�� */
	return count;
}

long redoServiceLog()
{
	/* �����ļ�, Stdout�ļ� */
	FILE *fpIndex, *fpErrlog,*fp;
	
	/* ����ֵ, ����Buffer���� */
	int ret,rcvlen;
	int i,j,k,l,m,blankPosition,pos;
	int pid[REDOPROCESSNUM+1],processNum,status;

	/* count��¼�ɹ���ɵļ�¼�� */
	long count,completeCount;
	int  recordCount,usedCount;

	/* ��ȡ�����ļ����ݵı��� */
	long indexPosition;
	int  blankFlag[REDOPROCESSNUM*REDOCOUNT];

	char redoBufferType[6];
	char redoFlag[5];
	char tempStr[LINE];
	char shmFile[LINE];
	char fileName[LINE];
    
	char memAccept[ACCEPTLEN];
	char redoFile[FILENAME];
	long redoPosition;
	char svcName[20];
	char phoneNo[20];

	/* redoPosition��¼������־�ļ��еķ��������ļ�¼��λ�� */
	/* indexPosition��¼�����ļ��е��ļ�ָ���λ�� */
	char indexFile[FILENAME];
	char errorFile[FILENAME];

	struct timeb start,finish;
	struct idxRecord idxRecord;

	memset(memAccept,0,sizeof(memAccept));
	memset(redoFile,0,sizeof(redoFile));
	memset(svcName,0,sizeof(svcName));
	memset(phoneNo,0,sizeof(phoneNo));

	memset(indexFile,0,sizeof(indexFile));
	memset(errorFile,0,sizeof(errorFile));
	memset(redoBufferType,0,sizeof(redoBufferType));
	memset(redoFlag,0,sizeof(redoFlag));
	memset(tempStr,0,sizeof(tempStr));
	memset(shmFile,0,sizeof(shmFile));
	memset(fileName,0,sizeof(fileName));

	memset(idxRecord.redoFile,0,sizeof(idxRecord.redoFile));
	memset(idxRecord.memAccept,0,sizeof(idxRecord.memAccept));
	memset(idxRecord.svcName,0,sizeof(idxRecord.svcName));
	memset(idxRecord.phoneNo,0,sizeof(idxRecord.phoneNo));

	for(i=0;i<REDOPROCESSNUM;i++)
	{
		idxRecord.redoPosition[i] = -1;
		idxRecord.processNo[i] = -1;
	}

	if(strlen(workDIR)==0)
	{
		printf("��������$REDODIR��ֵΪNULL!\n");
		userlog("��������$REDODIR��ֵΪNULL!\n");
		return -1;
	}

	if(redoType==0)
	{
		strcpy(indexFile,workDIR);
		strcat(indexFile,"/");
		strcat(indexFile,"log/IndexFile");

		strcpy(errorFile,workDIR);
		strcat(errorFile,"/");
		strcat(errorFile,"log/ErrorFile");
	}
	else
	{
		strcpy(indexFile,workDIR);
		strcat(indexFile,"/");
		strcat(indexFile,"log/ErrorFile");

		strcpy(errorFile,workDIR);
		strcat(errorFile,"/");
		strcat(errorFile,"log/ErrorFile.new");
	}

	if((fpIndex = fopen(indexFile,"r")) == NULL)
  	{
  		userlog("�������ļ� %s ����!\n", indexFile);
		return -1;
  	}

	if((fpErrlog = fopen(errorFile,"w+")) == NULL)
  	{
  		userlog("�򿪴����ļ� %s ����!\n", errorFile);
		fclose(fpIndex);
		return -2;
  	}
    
	/* �����м�����뻺�����Ŀռ� FML/FML32 */
	if ((obuf32 = (FBFR32 *)tpalloc("FML32",NULL,Fneeded32(1, 50*1024+50)))
		==(FBFR32 *)NULL) 
	{
		userlog("tpalloc failed in open_acct\n");
		fclose(fpIndex);
		fclose(fpErrlog);
		return -4;
	}
	
	if ((obuf = (FBFR *)tpalloc("FML",NULL,Fneeded(1, 50*1024+50)))
		==(FBFR *)NULL) 
	{
		userlog("tpalloc failed in open_acct\n");
		tpfree((char *)obuf32);
		fclose(fpIndex);
		fclose(fpErrlog);
		return -5;
	}
	
	/* �����м�����������ʹ�õĿռ� */
	if((rcvbuf = (char *) tpalloc("STRING", NULL, 20)) == NULL) 
	{
		(void) userlog("Error allocating receive buffer\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		return -6;
	}
	
	/* ����������־�����еķ��񽫲��ټ�¼������־,
	   �г���ҵ���ȷ��ҵ�񽫰Ѿ͵��ڴ���ˮ������ʱ
	   �����ɵ�ҵ����ˮ��¼��cfmAcceptFile.$OP_CODE
	   �ļ���,������ҵ��ʹ�� 
	*/
	strcpy(redoFlag,"redo");

	count = 0;
	processNum = 0;
	indexPosition = 0;

	signal(SIGTERM,QUIT);

	/* ���������ڴ���źŵ� */
	if(CreateRedoShm()!=0)
	{
		(void) userlog("Error allocating receive buffer\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		return -7;
	}

	spublicGetCfgDir(REDOSHMKEYFILE, tempStr);
	strcpy(shmFile,tempStr);

	/* �򿪹����ڴ� */
	if((shmp = redoOpenShm(ftok(shmFile,1)))==(struct redoShm *)NULL)
	{
		(void) userlog("Error open redo shm!\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		DelRedoShm();
		return -9;
	}
	
	/* ��ʼ�������ļ� */
	while (1)
	{
		/* �����ӽ��� */
		if(processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Fork child process,processNum=[%d]\n",processNum);
			#endif
			pid[processNum]=fork();
		}

		/* fork�ӽ���ʱ�������� */
		if(pid[processNum]<0&&processNum<REDOPROCESSNUM)
		{
			printf("Fork child process error!\n");
			userlog("Fork child process error!\n");
			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			fclose(fpErrlog);
			DelRedoShm();
			exit(-1);
		}
		/* ����������,���fork�ӽ������,ֹͣfork */ 
		else if(pid[processNum]>0)
		{ 
			if(processNum<REDOPROCESSNUM)
			{
				processNum++;
			}

			/* ����������ӽ������㹻,�����̿�ʼִ��������� */
			if(processNum == REDOPROCESSNUM)
			{
				pid[processNum]=1;

				memset(memAccept,0,sizeof(memAccept));
				memset(redoFile,0,sizeof(redoFile));

				memset(idxRecord.redoFile,0,sizeof(idxRecord.redoFile));
				memset(idxRecord.memAccept,0,sizeof(idxRecord.memAccept));
				memset(idxRecord.svcName,0,sizeof(idxRecord.svcName));
				memset(idxRecord.phoneNo,0,sizeof(idxRecord.phoneNo));

				/* ����¼�ӽ���״̬�������ʼ�� */
				for(i=0;i<REDOPROCESSNUM;i++)
				{
					idxRecord.redoPosition[i] = -1;
					idxRecord.processNo[i] = -1;
				}

				/* ����־�����ʼ�� */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					blankFlag[i]=-1;

				redoPosition=0; i=0; recordCount=0;
			
				/* ��ʼ�������ļ��еļ�¼ */
				while(1)
				{
					/* �������ļ� */
					if((fpIndex = fopen(indexFile,"r")) == NULL)
					{
						userlog("�������ļ� %s ����!\n", indexFile);
						fclose(fpErrlog);
						tpfree((char *)obuf32);
						tpfree((char *)obuf);
						tpfree((char *)rcvbuf);
						DelRedoShm();
						return -1;
					}

					/* ���ļ�ָ�붨λ����һ�ιر��ļ���λ�� */
					if (-1 == fseek(fpIndex, indexPosition, SEEK_SET))
					{
						userlog("feek error, file:%s, position:%ld\n",
							redoFile, redoPosition);
						fclose(fpIndex);
						fclose(fpErrlog);
						tpfree((char *)obuf32);
						tpfree((char *)obuf);
						tpfree((char *)rcvbuf);
						DelRedoShm();
						return -7;
					}
					/* �������ļ��е����� */
					fscanf(fpIndex, "%s %ld %s %s %s\n", 
						idxRecord.memAccept[i],
						&idxRecord.redoPosition[i],
						idxRecord.redoFile[i],
						idxRecord.svcName[i],
						idxRecord.phoneNo[i]);


					userlog("Read IndexFile:[%s][%ld][%s][%s][%s]\n",
						idxRecord.memAccept[i],
						idxRecord.redoPosition[i],
						idxRecord.redoFile[i],
						idxRecord.svcName[i],
						idxRecord.phoneNo[i]);
					
					count++;
					i++;
					recordCount++;

					/* ��¼�����ļ����ļ�ָ��λ�� */
					indexPosition = ftell(fpIndex);

					/* ȡ���㹻��Ŀ�ļ�¼ */
					if(feof(fpIndex)||i==REDOPROCESSNUM/2)
					{
						/* �����ļ�������shmp->endFlag���ñ�־,
						   1Ϊ�м��־,�����д�����ɺ󽫴˱�־
						   ����Ϊ2,�ӽ��̷��ִ˱�־���˳� 
						*/
						if(feof(fpIndex))shmp->endFlag=1;
						fclose(fpIndex);
						break;
					}
					fclose(fpIndex);
				}

				usedCount=0;
				/* ����������ӽ��� */
				while(1)
				{
					i=0;j=0;status=-1;
					/* ���ҿ��е��ӽ��� */
					while(1)
					{
						/* �����û��ִ����ɵĽ���ҵ��,�����ȴ�
						   shmp->phoneNo[i]="-"��ʾ�ǽ���ҵ��
						*/
						if(status==-1)
						{
							for(l=0;l<REDOPROCESSNUM;l++)
							{
								if(shmp->flag[l*REDOCOUNT]==0&&
								   strcmp(shmp->phoneNo[l*REDOCOUNT],"-")!=0)
								{
									sleep(0.05);
									l--;
								}
							}
							status=0;
						}

						/* shmp->flag[i]=1��ʾ��i���ӽ��̿��� */
						if(shmp->flag[i*REDOCOUNT]==1)
						{
							/* blankFlagΪ1��ʾ�������Ϊ����״̬ */
							if(blankFlag[i*REDOCOUNT]==-1)
							{
								blankFlag[i*REDOCOUNT]=1;
								j++;

								#ifdef _DEBUGDETAIL_
								userlog("shmp->flag[%d]=1,blankFlag[%d]=1\n",
									i*REDOCOUNT,i*REDOCOUNT);
								#endif
							}
						}

						i++;

						/* �ȴ������㹻���е��ӽ��� */
						if(usedCount+j>=REDOPROCESSNUM/2)break;
						/* ���û���ҵ��㹻�Ŀ��н���,�ȴ�0.1�� */
						if(i==REDOPROCESSNUM&&usedCount+j<REDOPROCESSNUM/2)
						{
							i=0;
							sleep(0.05);
						}
					}
					/* ���ҿ����ӽ���ѭ������ */
					
					/* ���Ҳ���������ͬ����Ĵ��� */
					for(i=0;i<recordCount;i++)
					{
						l=0;
						blankPosition=0;
						/* ����˽���Ϊ������״̬,������һ��ѭ�� */
						if(idxRecord.processNo[i]!=-1)continue;

						for(j=i+1;j<recordCount;j++)
						{
							/* ����˽���Ϊ������״̬,������һ��ѭ�� */
							if(idxRecord.processNo[j]!=-1)
							{
								/* ��������ӽ��̵�����ĵ�һ����־λ
								   �ڱ��������еĵ绰�������1,����
								   һ��ȡ����recordCount����¼������ͬ
								   �ĵ绰����,����־����Ϊռ��״̬0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}

							/* �ҵ��ĺ��붼����"-",�����ǽ�����صĲ��� */
							if(strcmp(idxRecord.phoneNo[i],"-")!=0&&strcmp(idxRecord.phoneNo[j],"-")!=0)
							{
								/* �ҵ���ͬ�ĺ��� */
								if(strcmp(idxRecord.phoneNo[i],idxRecord.phoneNo[j])==0)
								{
									/* l��¼�м�����ͬ�ĺ����¼ */
									/* pos��¼���α����ҵ��Ŀ��еĽ��̺� */
									l++;pos=0;

									/* �������е��ӽ���,�������ӽ��̷�������,
									   ��¼����־����idxRecord.porcessNo
									*/
									for(k=0;k<REDOPROCESSNUM;k++)
									{
							#ifdef _DEBUGDETAIL1_
							userlog("START:i=%d,j=%d,k=%d,l=%d,blankPosition=%d,idxRecord.phoneNo[i]=%s,blankFlag[%d]=%d\n",
								i,j,k,l,blankPosition,idxRecord.phoneNo[i],k*REDOCOUNT,blankFlag[k*REDOCOUNT]);
							#endif
										/* blankPosition��¼��һ�α���
										   �ҵ��Ŀյ��ӽ��̺�,�������
										   ֵ����0,��ôk�����ֵ��ʼѭ���
										*/
										if(blankPosition>0)
										{
											k=blankPosition;
										}

										/* ��һ���ҵ����е��ӽ��� */
										if(blankFlag[k*REDOCOUNT]==1&&l==1)
										{
											blankPosition=k;
											pos=k;
											/* ��¼�ӽ��̱����������λ�� */
											idxRecord.processNo[i]=k*REDOCOUNT;
											idxRecord.processNo[j]=k*REDOCOUNT+l;
											/* �����ڴ��ӽ��̵������
											   ��һ��ֵ����Ϊռ��״̬
											*/
											blankFlag[k*REDOCOUNT+l]=0;
							#ifdef _DEBUGDETAIL1_
							userlog("1:i=%d,j=%d,k=%d,l=%d,blankPosition=%d, idxRecord.processNo[i]=%d, idxRecord.processNo[j]=%d, blankFlag[%d]=0,idxRecord.phoneNo[i]=[%s]\n",
								i,j,k,l,blankPosition,
								idxRecord.processNo[i],
								idxRecord.processNo[j],
								k*REDOCOUNT+l,idxRecord.phoneNo[j]);
							#endif
											/* ����Ѿ��ҵ���¼�����
											   �����ڴ��ӽ��̵������
											   һ��ֵ����Ϊռ��״̬,
											   �Ѿ��������������1
											*/
											if(j+1==recordCount)
											{
												blankFlag[k*REDOCOUNT]=0;
												usedCount++;
											}
											break;
										}
										/* ��n��(n>1)�ҵ����е��ӽ��� */
										else if(blankFlag[k*REDOCOUNT]==1&&l<REDOCOUNT&&l>1)
										{
											k=blankPosition;
											pos=k;
											idxRecord.processNo[i]=k*REDOCOUNT;
											idxRecord.processNo[j]=k*REDOCOUNT+l;
											blankFlag[k*REDOCOUNT+l]=0;
							#ifdef _DEBUGDETAIL1_
							userlog("2:i=%d,j=%d,k=%d,l=%d,blankPosition=%d, idxRecord.processNo[i]=%d, idxRecord.processNo[j]=%d, blankFlag[%d]=0\n",
								i,j,k,l,blankPosition,
								idxRecord.processNo[i],
								idxRecord.processNo[j],
								k*REDOCOUNT+l);
							#endif
											if(j+1==recordCount)
											{
												blankFlag[k*REDOCOUNT]=0;
												usedCount++;
											}
											break;
										}
										/* �Ѿ����������ӽ���������Ŀ������ */
										else if(blankFlag[k*REDOCOUNT]==1&&l==REDOCOUNT)
										{
											pos=k;
											blankPosition=0;
											blankFlag[k*REDOCOUNT]=0;
											usedCount++;
							#ifdef _DEBUGDETAIL1_
							userlog("3:i=%d,j=%d,k=%d,l=%d,blankPosition=%d, idxRecord.processNo[i]=%d, idxRecord.processNo[j]=%d, blankFlag[%d]=0\n",
								i,j,k,l,blankPosition,
								idxRecord.processNo[i],
								idxRecord.processNo[j],
								k*REDOCOUNT);
							#endif
											break;
										}
										/* ���ҿ����ӽ����жϽ��� */
									}
									/* ���ҿ����ӽ���ѭ������ */
								}
								/* �ҵ���ͬ�ĺ����жϽ��� */
							}
							/* �ҵ��ĺ��붼����"-"�жϽ��� */
							/* �ǽ��Ѳ����Ĵ��� */
							else if(strcmp(idxRecord.phoneNo[j],"-")==0) 
							{
								/* ��������ӽ��̵�����ĵ�һ����־λ
								   �ڱ��������еĵ绰�������1,����
								   һ��ȡ����recordCount����¼������ͬ
								   �ĵ绰����,����־����Ϊռ��״̬0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}
							/* �ǽ��Ѳ����Ĵ��� */
							else if(strcmp(idxRecord.phoneNo[i],"-")==0)
								break;

							/* ��������ӽ��̵�����ĵ�һ����־λ
							   �ڱ��������еĵ绰�������1,����
							   һ��ȡ����recordCount����¼������ͬ
							   �ĵ绰����,����־����Ϊռ��״̬0
							*/
							if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
								j+1==recordCount)
							{
								blankFlag[pos*REDOCOUNT]=0;
								usedCount++;
							}

							/* �Ѿ����������ӽ���������Ŀ���������˳� */
							if(l==REDOCOUNT)break;
						}
					} /* ������ͬ��������� */

					/* status��¼��û�гɹ�����,0Ϊ�ɹ� */
					status=0;
					/* ��ʼ���䲻ͬ��������� */
					for(i=0;i<recordCount;i++)
					{
						/* δ��������� */
						if(idxRecord.processNo[i]==-1)
						{
							status=-1;
							for(k=0;k<REDOPROCESSNUM;k++)
							{
								/* �������Ϊ����,1Ϊ����,0Ϊռ�� */
								if(blankFlag[k*REDOCOUNT]==1)
								{
									idxRecord.processNo[i]=k*REDOCOUNT;
									blankFlag[k*REDOCOUNT]=0;
									usedCount++;
									status=0;
							#ifdef _DEBUGDETAIL1_
							userlog("5:i=%d,k=%d,idxRecord.processNo[i]=%d, blankFlag[%d]=0\n",
								i,k,
								idxRecord.processNo[i],
								k*REDOCOUNT);
							#endif
									break;
								}
							}
						}
					} /* �������������� */

					/* ���е������Ѿ�������� */
					if(status!=-1)break;
				}
				/* ��������������ӽ���ѭ������ */

				#ifdef _DEBUGDETAIL2_
				for(i=0;i<recordCount;i++)
				{
					userlog("####i=[%d],idxRecord.processNo[%d]=%d\n",
						 i,i,idxRecord.processNo[i]);
				}
				#endif

				/* ������������Ľ��̸�ֵ */
				for(i=0;i<recordCount;i++)
				{

					memset(shmp->memAccept[idxRecord.processNo[i]],0,
						sizeof(shmp->memAccept[idxRecord.processNo[i]]));
					memset(shmp->redoFile[idxRecord.processNo[i]],0,
						sizeof(shmp->redoFile[idxRecord.processNo[i]]));
					memset(shmp->svcName[idxRecord.processNo[i]],0,
						sizeof(shmp->svcName[idxRecord.processNo[i]]));
					memset(shmp->phoneNo[idxRecord.processNo[i]],0,
						sizeof(shmp->phoneNo[idxRecord.processNo[i]]));

					strcpy(shmp->memAccept[idxRecord.processNo[i]],idxRecord.memAccept[i]);
					shmp->redoPosition[idxRecord.processNo[i]]=idxRecord.redoPosition[i];
					strcpy(shmp->redoFile[idxRecord.processNo[i]],idxRecord.redoFile[i]);
					strcpy(shmp->svcName[idxRecord.processNo[i]],idxRecord.svcName[i]);
					strcpy(shmp->phoneNo[idxRecord.processNo[i]],idxRecord.phoneNo[i]);

					/* �޸ı�־Ϊ������״̬,���޸������ӽ��̷ǵ�һ�������״̬ */
					if((idxRecord.processNo[i]%REDOCOUNT)!=0)
					{
						userlog("1-idxRecord.processNo[%d]=%d\n",i,idxRecord.processNo[i]);
						shmp->flag[idxRecord.processNo[i]]=0;
					}

					#ifdef _DEBUGDETAIL_
					userlog("i=[%d],idxRecord.processNo[%d]=%d\n",
						 i,i,idxRecord.processNo[i]);
					userlog("shmp->memAccept[%d]=[%s], shmp->redoPosition[%d]=%d, shmp->redoFile[%d]=%s, shmp->svcName[%d]=%s, shmp->phoneNo[%d]=%s, shmp->flag[%d]=%d, shmp->endFlag=%d\n",
						 idxRecord.processNo[i],
						 shmp->memAccept[idxRecord.processNo[i]],
						 idxRecord.processNo[i],
						 shmp->redoPosition[idxRecord.processNo[i]],
						 idxRecord.processNo[i],
						 shmp->redoFile[idxRecord.processNo[i]],
						 idxRecord.processNo[i],
						 shmp->svcName[idxRecord.processNo[i]],
						 idxRecord.processNo[i],
						 shmp->phoneNo[idxRecord.processNo[i]],
						 idxRecord.processNo[i],
						 shmp->flag[idxRecord.processNo[i]],
						 shmp->endFlag);
					#endif 
				}
				/* ��ֵ�������ӽ���ѭ������ */

				/* ����޸������ӽ��̵ĵ�1�������״̬ */
				for(i=0;i<recordCount;i++)
				{
					userlog("2-idxRecord.processNo[%d]=%d\n",i,idxRecord.processNo[i]);
					if((idxRecord.processNo[i]%REDOCOUNT)==0)
						shmp->flag[idxRecord.processNo[i]]=0;
				}
			}
			/* �����̷��������жϴ������ */

			/* ����������ļ�����,�˳�ѭ�� */
			if(shmp->endFlag==1)
			{
				sleep(10);
				/* ����־����Ϊ2,֪ͨ�ӽ����˳� */
				shmp->endFlag=2;

				/* ������ӽ��̵�����û�����,�����̵ȴ� */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					if(shmp->flag[i]!=1)
					{
						#ifdef _DEBUGDETAIL_
						userlog("Father process:shmp->flag[%d]=0!\n",i);
						#endif
						i--;sleep(1);
					}

				/* �����ӽ�������ִ�����,�������˳� */
				break;
			}
		}
		/* �ӽ��̵Ĵ��� */
		else if(pid[processNum]==0&&processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Child process start,pid[%d],processNum[%d]\n",getpid(),processNum);
			#endif

			/* �ͷ��������� */
			if (tpinit((TPINIT *) NULL) == -1) 
			{
				userlog("pid[%d] tpinit error:tperrno=%d,tpstrerror=%s\n",
					getpid(),tperrno,tpstrerror(tperrno));
				userlog("Can't create connect to service,child process end,pid[%d]\n",getpid());
				fprintf(fpErrlog, "%s %ld %s\n",  \
					memAccept,redoPosition,   \
					redoFile);
				tpfree((char *)obuf32);
				tpfree((char *)obuf);
				tpfree((char *)rcvbuf);
				fclose(fp);
				exit(0);
			}

			completeCount=0;

			/* ѭ�����Ҹ����̷�������� */
			while(1)
			{
				count=0;status=0;
				/* �ӽ��̱��������Լ������� */
				for(i=0;i<REDOCOUNT;i++)
				{
					/* ������ӽ��̵ĵ�0�������ϻ�û�з�������ʱ
					   �ӽ��̵ȴ������̷����������ټ���ִ��
					*/
					if(shmp->flag[processNum*REDOCOUNT]!=0)
					{
						
						#ifdef _DEBUGDETAIL2_
						userlog("pid[%d] i=[%d] processNum=[%d],shmp->flag[%d]=[%d],Child process sleep!\n",
							 getpid(),i,processNum,processNum*REDOCOUNT,shmp->flag[processNum*REDOCOUNT]);
						#endif
						i--;status++;
						/* ����ȴ���0.1*1000�뻹û������,�˳���ѭ�� */
						if(status==1000)break;
						sleep(0.05);
						continue;
					}	
					/* �����̸����ӽ��̷���������,�ӽ��̿�ʼִ�� */
					else if(shmp->flag[processNum*REDOCOUNT+i]==0)
					{
						/* count��¼��ɵ������� */
						count++;completeCount++;
						memset(memAccept,0,sizeof(memAccept));
						memset(redoFile,0,sizeof(redoFile));

						strcpy(memAccept,shmp->memAccept[processNum*REDOCOUNT+i]);
						redoPosition = shmp->redoPosition[processNum*REDOCOUNT+i];
						strcpy(redoFile,shmp->redoFile[processNum*REDOCOUNT+i]);

						#ifdef _DEBUGDETAIL_
						userlog("pid[%d],CHILD process start run!shmp->memAccept[%d]=%s, shmp->redoPosition[%d]=%d, shmp->redoFile[%d]=%s,shmp->phoneNo[%d]=%s,shmp->flag[%d]=%d\n",
							 getpid(),processNum*REDOCOUNT+i,
							 memAccept,processNum*REDOCOUNT+i,
							 redoPosition,processNum*REDOCOUNT+i,
							 redoFile,processNum*REDOCOUNT+i,
							 shmp->phoneNo[processNum*REDOCOUNT+i],
							 processNum*REDOCOUNT+i,shmp->flag[processNum*REDOCOUNT+i]);
						#endif 
						
						ftime(&start);
						
						
						/* ��������־�ļ� */
						if((fp=fopen(redoFile, "r"))==NULL)
						{
							userlog("pid[%d] Open redolog file %s error !\n", 
								getpid(),redoFile);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							continue;
						}
						
						/* ���ļ�ָ���ƶ���ָ��λ�� */
						if (-1 == fseek(fp, redoPosition, SEEK_SET))
						{
							userlog("pid[%d] feek error, file:%s, position:%ld\n",
								getpid(),redoFile, redoPosition);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							fclose(fp);
							continue;
						}

						memset(svcName,0,sizeof(svcName));
						memset(redoBufferType,0,sizeof(redoBufferType));
						
						/* ���������ͻ��������� */
						if (-1 == fscanf(fp, "%s %s\n", svcName, redoBufferType))
						{
							userlog("pid[%d] fscanf file <%s> error!\n", 
								getpid(),redoFile);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							fclose(fp);
							continue;
						}
						
						#ifdef _DEBUG_
						userlog("pid[%d] Start tpcall,svcName=[%s], redoBufferType=[%s]\n",
							getpid(),svcName, redoBufferType);	
						#endif


						/* �����FML */
						if (strcmp(redoBufferType, "fml") == 0)
						{
							ret=Finit(obuf, Fneeded(1, 50*1024+20));
							if(ret==-1)
							{
								userlog("pid[%d] Error occur on Finit:tperrno=%d, \
									tpstrerror=%s",getpid(),Ferror,   \
									tpstrerror(tperrno));
								fclose(fp);
								continue;
							}
						
							/* ��������־�ļ��о�������� */
							ret = Fextread(obuf,fp);
/*
							if(ret != 0)
							{
								userlog("pid[%d] Error occur on readTransINBuf,errno=%d\n",
									getpid(),fp);
								fclose(fp);
								continue;
							}
*/

							if (-1 == Fchg(obuf, RedoFlag, 0, redoFlag, 0))
							{
								userlog("pid[%d] Error occur on FChg of RedoFlag\n",getpid());
								fprintf(fpErrlog, "%s %ld %s\n",  \
									memAccept,redoPosition,   \
									redoFile);
								fclose(fp);
								continue;
							}

							/* ���ɵ��ڴ���ˮ�͸����񹩳���ҵ��ʹ�� */
							if (-1 == Fchg(obuf, DPARM_9, 0, memAccept, 0))
							{
								userlog("pid[%d] Error occur on FChg of  \
									DPARM_9\n",getpid());
								fprintf(fpErrlog, "%s %ld %s\n",  \
									memAccept,redoPosition,   \
									redoFile);
								fclose(fp);
								continue;
							}

							ret = tpcall(svcName, (char *)obuf, (long)0, \
								(char **)&rcvbuf, (long*)&rcvlen, (long)0);
							if(ret == -1) 
							{
								userlog("pid[%d] tpcall error:tperrno=%d, \
									tpstrerror=%s",getpid(),tperrno,   \
									tpstrerror(tperrno));
								fprintf(fpErrlog, "%s %ld %s\n",  \
									memAccept,redoPosition,   \
									redoFile);
								fclose(fp);
								continue;
							}


						} /* FML���÷�������� */
						/* FML32���� */
						else if (strcmp(redoBufferType, "fml32") == 0)
						{
							ret=Finit32(obuf32, Fneeded32(1, 50*1024+20));
							if(ret==-1)
							{
								userlog("pid[%d] Error occur on Finit:tperrno=%d, \
									tpstrerror=%s",getpid(),Ferror,   \
									tpstrerror(tperrno));
								fclose(fp);
								continue;
							}
						
							ret = Fextread32(obuf32,fp);
/*
							if(ret != 0)
							{
								userlog("pid[%d] Error occur on readTransINBuf32,errno=%d\n",
									getpid(),fp);
								fclose(fp);
								continue;
							}
*/

							if (-1 == Fchg32(obuf32, RedoFlag32, 0,   \
								redoFlag, 0))
							{
								userlog("pid[%d] Error occur on FChg32 of \
									svcName\n",getpid());
								fprintf(fpErrlog, "%s %ld %s\n",  \
									memAccept,redoPosition,   \
									redoFile);
								fclose(fp);
								continue;
							}
							if (-1 == Fchg32(obuf32, DPARM32_9, 0,      \
								memAccept, 0))
							{
								userlog("pid[%d] Error occur on FChg32 of \
									DPARM32_9\n",getpid());
								fprintf(fpErrlog, "%s %ld %s\n",  \
									memAccept,redoPosition,   \
									redoFile);
								fclose(fp);
								continue;
							}


							ret = tpcall(svcName, (char *)obuf32, (long)0,\
								(char **)&rcvbuf, (long*)&rcvlen, (long)0);
							if(ret == -1) 
							{
								userlog("pid[%d] tpcall error:tperrno=%d,  \
									tpstrerror=%s",getpid(),tperrno,   \
									tpstrerror(tperrno));
								fprintf(fpErrlog, "%s %ld %s\n",   \
									memAccept,redoPosition,    \
									redoFile);
								fclose(fp);
								continue;
							}

							#ifdef _DEBUG_
							userlog("pid[%d] tpcall:memAccept=%s, svcName=%s\n", 
								getpid(),memAccept, svcName);
							#endif
						} 
						/* FML32���÷�������� */
						else
						{
							userlog("pid[%d] Buffer Type Error: %s\n", 
								getpid(),redoBufferType);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							fclose(fp);
							continue;
						} 
						/* �ӽ��̵��÷�������� */
						
						fclose(fp);

						ftime(&finish);
						#ifdef _DEBUG_
						userlog("pid[%d] service [%s][%s] execute [%d] milseconds.\n",
							getpid(),memAccept,svcName,(finish.time-start.time)*1000+\
							(finish.millitm-start.millitm));
						#endif 
					}

				}/* �ӽ���ִ�б�������������(for) */

				for(i=count-1;i>=0;i--)
				{
					/* ��ִ����ɵ������־����Ϊ����,������
					   ���������ӽ��̷�������,���õ�˳������
					   ���С 
					*/
					if(shmp->flag[processNum*REDOCOUNT+i] == 0)
					{
						#ifdef _DEBUGDETAIL_
						userlog("pid[%d] Reset the shmp->flag[%d] to 1!\n",
							 getpid(),processNum*REDOCOUNT+i);
						#endif
						shmp->flag[processNum*REDOCOUNT+i]=1;
					}
				}

				/* �����̵�����ִ����� */
				if(shmp->endFlag==2)
				{
					/* ���������û����ɵ�����,���������Ѿ�ִ�����,
					   �ӽ��̼���ִ��û����ɵ����� 
					*/
					for(i=0;i<REDOCOUNT;i++)
						if(shmp->flag[processNum*REDOCOUNT+i]==0)
						{
							#ifdef _DEBUGDETAIL_
							userlog("pid[%d] shmp->endFlag=2,the shmp->flag[%d]=0!Continue run!\n",
								 getpid(),processNum*REDOCOUNT+i);
							#endif
							i=0;break;
						}

					/* �����������ִ����,�ӽ����˳� */
					if(i==REDOCOUNT&&shmp->flag[processNum*REDOCOUNT]==1)
					{
						#ifdef _DEBUGDETAIL_
						userlog("pid[%d] shmp->endFlag=2,all shmp->flag[%d]=1!Child process exit!\n",
							 getpid(),processNum*REDOCOUNT+i);
						#endif
						break;
					}
				}
			}
			/* ѭ�����Ҹ����̷������������� */
			
			userlog("Child process end,pid[%d],process record:[%ld]\n",getpid(),completeCount);

			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			exit(0);
		} 
		/* �ӽ��̴������ */
	} 
	/* �������ļ�������� */
		
	userlog("Redo�����Ѿ��ɹ���� ... \n");
	printf("Redo�����Ѿ��ɹ���� ... \n");
	userlog("Redo�����еĴ�����Ϣ�Ѿ�д���ļ�:%s ... \n\n",errorFile);
	printf("Redo�����еĴ�����Ϣ�Ѿ�д���ļ�:%s ... \n\n",errorFile);

	#ifdef _DEBUGDETAIL2_
	/* ��ӡ�����ڴ浱ǰ����Ϣ */
 	for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
        {
		userlog("--------------------------------------------\n");
		userlog("shmp->memAccept[%d]=[%s],shmp->redoPosition[%d]=[%d],\nshmp->redoFile[%d]=[%s], shmp->svcName[%d]=[%s],\nshmp->phoneNo[%d]=[%s], shmp->flag[%d]=[%d], \nshmp->endFlag=[%d]\n",
			i,shmp->memAccept[i],
			i,shmp->redoPosition[i],
			i,shmp->redoFile[i],
			i,shmp->svcName[i],
			i,shmp->phoneNo[i],
			i,shmp->flag[i],
			shmp->endFlag);
	}
	#endif
	
	fclose(fpIndex);
	fclose(fpErrlog);
	
	tpfree((char *)obuf32);
	tpfree((char *)obuf);
	tpfree((char *)rcvbuf);
	DelRedoShm();

	return count;
}

void main(int argc, char *argv[])
{
	char startTime[15],endTime[15];
	char *envp,confirm;
	long allRec,succRec;

	envp = spublicGetEnvDir(REDODIR);
	if ( envp == NULL )
	{
		printf("��������$REDODIR��ֵΪNULL!\n");
		exit(0);
	}
	memset(workDIR,0,sizeof(workDIR));
	strcpy(workDIR,envp);

	system("clear");
	printf("\n---------------------------------------------------------\n");
	printf("Program: redo\nVersion: V30.2003.01.20\nDeveloper: dangsl\n");
	printf("����������iBOSS��������б���ϵͳ����־�����ָ�\n");
	printf("---------------------------------------------------------\n\n");


	printf("\n---------------------------------------------------------\n");
	printf("              ������־���̿�ʼ                       ");
	printf("\n---------------------------------------------------------\n");

	if(argc==1)
	{
		strcpy(startTime,"00000000000000");
		strcpy(endTime,"99999999999999");
	}
	else if(argc==2)
	{
		if(strlen(argv[1])<14&&strcmp(argv[1],"error")!=0)
		{
			printf("�������ȴ���!\n");
			exit(0);
		}

		/* 1ָ�ָ�����ErrorFile�е���־ */
		if(strcmp(argv[1],"error")==0)
		{
			redoType=1;
		}
		else
		{
			strcpy(startTime,argv[1]);
			strcpy(endTime,"99999999999999");
		}
	}
	else if(argc==3)
	{
		if((strlen(argv[1])<14)||(strlen(argv[2])<14))
		{
			printf("�������ȴ���!\n");
		}
		strcpy(startTime,argv[1]);
		strcpy(endTime,argv[2]);
	}
	else
	{
		printf("��������!\n");
		printf("Usage:\n");
		printf("\tredo [StartDatetime] [EndDatetime]\n");
		printf("Or:\n");
		printf("\tredo [yyyymmddhhmmss] [yyyymmddhhmmss]\n");
		printf("Example:\n");
		printf("\tredo 20020101010101 20021231235959 \n");
		exit(0);
	}

	userlog("startTime:[%s],endTime:[%s],%d,%d\n",
		startTime,endTime,strlen(startTime),strlen(endTime));

	printf("\n---------------------------------------------------------\n");
	printf("�������ָ���������������:\n");
	printf("\t1.��������REDODIR��REDODIR�Ƿ���ȷ?\n");
	printf("\t2.����ȷ�ϻָ������з������ӵ����ݿ��Ƿ���ȷ?\n");
	printf("\t  ��������ļ�$REDODIR/cfg/sDBLogin.cfg\n");
	printf("\t3.$REDODIR/logĿ¼�µ��ļ��Ƿ�Ϊ��Ҫ�ָ��Ĵ�����������־?\n");
	printf("\t4.$REDODIR/logĿ¼�µ�cfmAcceptFile.*�Ƿ��Ѿ�ɾ��?\n");
	printf("\t5.$REDODIR/logĿ¼�µ�cancelAcceptFile.*�ļ��������ǲ���\n");
	printf("\t  �Ѿ��������м���������ϵ��ļ��ϲ�Ϊһ��?\n");
	printf("---------------------------------------------------------\n");

	printf("\n��������$REDODIR��ֵΪ:%s\n",workDIR);
	userlog("��������$REDODIR��ֵΪ:%s\n",workDIR);
	printf("��ʼ����,��ǰʱ��:%s\n",GetSysDatetime());

	if(redoType==0)
	{
		printf("��ѡ�������[%s]��[%s]ʱ����ڵķ���,�Ƿ����?(y/n)\n",
			startTime,endTime);

		scanf("%c",&confirm);
		if(confirm!='y'&&confirm!='Y')
		{
			printf("��ѡ���˲�ִ�У������˳���\n");
			userlog("��ѡ���˲�ִ�У������˳���\n");
			exit(0);
		}
		getchar();

		printf("---------------------------------------------------------\n");
		printf("��ʼ������־�����ļ�...\n");

		allRec = creatIndexFile(startTime,endTime);
		if( allRec< 0)
		{
			userlog("���������ļ�����! ErrorNo:%ld\n",allRec);
			printf("���������ļ�����! ErrorNo:%ld\n",allRec);
			exit(0);
		}

		userlog("�����ļ������ĳ�,��¼��Ϊ:%ld\n",allRec);
		printf("�����ļ������ĳ�,��¼��Ϊ:%ld\n",allRec);
		printf("���������ļ��������,��ǰʱ��:%s\n",GetSysDatetime());
	}
	else
	{
		printf("���潫��ʼ�ָ�������־�еķ���,���λָ��Ĵ�����־����¼��$REDODIR/log/ErrorFile.new��.\n");
	}

	printf("���潫���������ļ������ݵ��÷���,�Ƿ����?(y/n)\n");
	scanf("%c",&confirm);
	if(confirm!='y'&&confirm!='Y')
	{
		printf("��ѡ���˲�ִ�У������˳���\n");
		userlog("��ѡ���˲�ִ�У������˳���\n");
		exit(0);
	}
	printf("��ʼ���������ļ������ݵ��÷���...\n");
	succRec = redoServiceLog();
	if( succRec < 0)
	{
		userlog("Redo����ʧ��! ErrorNo:%ld\n",succRec);
		printf("Redo����ʧ��! ErrorNo:%ld\n",succRec);
		exit(0);
	}

	printf("���÷��������,��ǰʱ��:%s\n",GetSysDatetime());
	userlog("Redo���̴���ļ�¼��:%d,�ɹ���ɵļ�¼��:%d\n",
		allRec,succRec);
	printf("Redo���̴���ļ�¼��:%d,�ɹ���ɵļ�¼��:%d\n",
		allRec,succRec);
	printf("---------------------------------------------------------\n");
}
