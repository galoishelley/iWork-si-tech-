/*
**    ³ÌÐòÃû£ºredo.c
**  ¹¦ÄÜÃèÊö£ºiBOSS²»¼ä¶ÏÔËÐÐ±£ÕÏÏµÍ³µÄÈÕÖ¾ÖØ×ö»Ö¸´
**  Ê¹ÓÃ·½·¨£ºredo startTime endTime
**        Èç£ºredo 20030115000000 20030115190000
**  ´´½¨ÈÕÆÚ£º2003.01.19
**  ³ÌÐò°æ±¾: V30.2003.01.20
**    ´´½¨ÈË£ºdangsl
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

/* ÏµÍ³ÖÐ´æÔÚFMLºÍFML32ÀàÐÍ±äÁ¿, ¶¼ÐèÒª¶¨Òå */
FBFR32 *obuf32;
FBFR   *obuf;

/* ½ÓÊÕBuffer */
char   *rcvbuf;

struct redoShm *shmp;

void QUIT(int signo)
{
	tpfree((char *)obuf32);
	tpfree((char *)obuf);
	tpfree((char *)rcvbuf);
	shmp->endFlag=2;
	DelRedoShm();
	userlog("³ÌÐò±»ÓÃ»§ÖÐ¶Ï!\n");
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


/* ´´½¨ÖØ×öÈÕÖ¾ÎÄ¼þµÄË÷ÒýÎÄ¼þ£¬¼Ó¿ìÖ´ÐÐËÙ¶È */
long creatIndexFile(char *startTime,char *endTime)
{
	FILE *cfgFP,*logFP,*idxFP;
	char cfmAccept[100],title[2048],svcName[20],phoneNo[20];
	char fileName[FILENAME+FILENAME],tempCMD[FILENAME+FILENAME];
	char tempIdxFile[FILENAME+FILENAME];
	char cfgFile[FILENAME+FILENAME],logFile[FILENAME+FILENAME];
	char idxFile[FILENAME+FILENAME];

	/* ¼ÇÂ¼×Ü¹²ÐèÒª´¦ÀíµÄ¼ÇÂ¼×ÜÊý */
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
		printf("»·¾³±äÁ¿$REDODIRµÄÖµÎªNULL!\n");
		userlog("»·¾³±äÁ¿$REDODIRµÄÖµÎªNULL!\n");
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
		userlog("´ò¿ªÅäÖÃÎÄ¼þ %s ´íÎó!\n",cfgFile);
		return -1;
	}

	if(!(idxFP=fopen(idxFile,"w+")))
	{
		userlog("´ò¿ªË÷ÒýÎÄ¼þ %s ´íÎó!\n",idxFile);
		return -2;
	}

	/* ¶ÁÅäÖÃÎÄ¼þ$REDODIR/cfg/svrLogFile.cfg */
	while(!feof(cfgFP))
	{
		memset(fileName,0,sizeof(fileName));

		/* ¸ù¾Ý$REDODIR/cfg/svrLogFile.cfgÅäÖÃÎÄ¼þ²éÕÒ·ûºÏÌõ¼þµÄ¼ÇÂ¼ */
		if(fscanf(cfgFP,"%s",fileName)<0)
		{
			if(!(feof(cfgFP)))
			{
				userlog("¶ÁÅäÖÃÎÄ¼þ %s ´íÎó!\n",cfgFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -3;
			}
			else
			{
				userlog("¶ÁÅäÖÃÎÄ¼þ %s ½áÊø!\n",cfgFile);
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


			/* ¸ù¾ÝÅäÖÃÎÄ¼þÖÐµÄÎÄ¼þÃû´ò¿ªÖØ×öÈÕÖ¾ÎÄ¼þ£¬
			   ²éÑ¯·ûºÏÖØ×öÌõ¼þµÄ¼ÇÂ¼ */
			if(!(logFP=fopen(logFile,"r")))
			{
				userlog("´ò¿ªÖØ×öÈÕÖ¾ÎÄ¼þ %s ´íÎó!\n",logFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -4;
			}
			else
			{
				/* ¶ÁÖØ×öÈÕÖ¾ÎÄ¼þ */
				while(!(feof(logFP)))
				{
					memset(title,0,sizeof(title));
					if(fscanf(logFP,"%s",title)<0)
					{
						if(!(feof(logFP)))
						{
							userlog("¶ÁÈ¡ÖØ×öÈÕÖ¾ÎÄ¼þ %s ´íÎó!\n",logFile);
							fclose(cfgFP);
							fclose(idxFP);
							fclose(logFP);
							return -5;
						}
						else
						{
							userlog("¶ÁÈ¡ÖØ×öÈÕÖ¾ÎÄ¼þ %s ½áÊø!\n",logFile);
							fclose(logFP);
							break;
						}
					}
					/* ÏÈÕÒµ½<TitleMessage>Õâ¸ö±êÖ¾ */
					else if(strcmp(title,"<TitleMessage>")==0) 
					{
						memset(cfmAccept,0,sizeof(cfmAccept));
						/* ¶ÁÈ¡ÄÚ´æÁ÷Ë® */
						if(fscanf(logFP,"%s",cfmAccept)<0)
						{
							if(!(feof(logFP)))
							{
								userlog("¶ÁÈ¡ÖØ×öÈÕÖ¾ÎÄ¼þ %s ´íÎó!\n",logFile);
								fclose(cfgFP);
								fclose(idxFP);
								fclose(logFP);
								return -6;
							}
							else
							{
								userlog("¶ÁÈ¡ÖØ×öÈÕÖ¾ÎÄ¼þ %s ½áÊø!\n",logFile);
								fclose(logFP);
								break;
							}
						}
						else 
						{
							idxPosition=ftell(logFP);
							/* ÔÚÊ±¼ä·¶Î§ÄÚµÄ¼ÇÂ¼ */
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
								/* ¶ÔÓÚ½»·ÑµÄÌØÊâ´¦Àí½áÊø */

								#ifdef _DEBUG_
								userlog("%s %ld %s %s %s\n",cfmAccept,ftell(logFP),logFile,svcName,phoneNo);
								#endif

								/* ½«¼ÇÂ¼Ð´ÈëË÷ÒýÎÄ¼þ */
								fprintf(idxFP,"%s %ld %s %s %s\n",
									cfmAccept,idxPosition,logFile,
									svcName,phoneNo);
								count++;
							}
							/* ÅÐ¶ÏÊÇ·ñ·ûºÏÌõ¼þµÄ¼ÇÂ¼´¦Àí½áÊø */
						}
						/* ¶ÁÄÚ´æÁ÷Ë®´¦Àí½áÊø */
					}
					/* ²éÕÒ<TitleMessage>±êÖ¾´¦Àí½áÊø */
				}
				/* ÅÐ¶Ï¶ÁÖØ×öÈÕÖ¾ÎÄ¼þÊÇ·ñ½áÊø */
			}
			/* ´ò¿ªÖØ×öÈÕÖ¾ÎÄ¼þ */
		}
		/* ¸ù¾Ý$REDODIR/cfg/svrLogFile.cfgÅäÖÃÎÄ¼þ²éÕÒ·ûºÏ
		   Ìõ¼þµÄ¼ÇÂ¼´¦Àí½áÊø */
	}
	/* Ñ­»·¶Á$REDODIR/cfg/svrLogFile.cfgÎÄ¼þ´¦Àí½áÊø */

	memset(tempCMD,0,sizeof(tempCMD));
	strcpy(tempCMD,"sort ");
	strcat(tempCMD,tempIdxFile);

	memset(idxFile,0,sizeof(idxFile));

	strcpy(idxFile,workDIR);
	strcat(idxFile,"/");
	strcat(idxFile,"log/IndexFile");

	strcat(tempCMD," -o ");
	strcat(tempCMD,idxFile);
	/* Éú³ÉÅÅÐòµÄÕýÊ½Ë÷ÒýÎÄ¼þ$REDODIR/log/IndexFile */
	system(tempCMD);

	printf("´´½¨Ë÷ÒýÎÄ¼þ[%s]³É¹¦Íê³É ...  \n",idxFile);
	userlog("´´½¨Ë÷ÒýÎÄ¼þ[%s]³É¹¦Íê³É ... \n",idxFile);

	/* ·µ»Ø·ûºÏÖØ×öÌõ¼þµÄ¼ÇÂ¼Êý */
	return count;
}

long redoServiceLog()
{
	/* Ë÷ÒýÎÄ¼þ, StdoutÎÄ¼þ */
	FILE *fpIndex, *fpErrlog,*fp;
	
	/* ·µ»ØÖµ, ½ÓÊÕBuffer³¤¶È */
	int ret,rcvlen;
	int i,j,k,l,m,blankPosition,pos;
	int pid[REDOPROCESSNUM+1],processNum,status;

	/* count¼ÇÂ¼³É¹¦Íê³ÉµÄ¼ÇÂ¼Êý */
	long count,completeCount;
	int  recordCount,usedCount;

	/* ¶ÁÈ¡Ë÷ÒýÎÄ¼þÄÚÈÝµÄ±äÁ¿ */
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

	/* redoPosition¼ÇÂ¼ÖØ×öÈÕÖ¾ÎÄ¼þÖÐµÄ·ûºÏÌõ¼þµÄ¼ÇÂ¼µÄÎ»ÖÃ */
	/* indexPosition¼ÇÂ¼Ë÷ÒýÎÄ¼þÖÐµÄÎÄ¼þÖ¸ÕëµÄÎ»ÖÃ */
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
		printf("»·¾³±äÁ¿$REDODIRµÄÖµÎªNULL!\n");
		userlog("»·¾³±äÁ¿$REDODIRµÄÖµÎªNULL!\n");
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
  		userlog("´ò¿ªË÷ÒýÎÄ¼þ %s ´íÎó!\n", indexFile);
		return -1;
  	}

	if((fpErrlog = fopen(errorFile,"w+")) == NULL)
  	{
  		userlog("´ò¿ª´íÎóÎÄ¼þ %s ´íÎó!\n", errorFile);
		fclose(fpIndex);
		return -2;
  	}
    
	/* ·ÖÅäÖÐ¼ä¼þÊäÈë»º³åÇøµÄ¿Õ¼ä FML/FML32 */
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
	
	/* ·ÖÅäÖÐ¼ä¼þÊä³ö»º³åÇøÊ¹ÓÃµÄ¿Õ¼ä */
	if((rcvbuf = (char *) tpalloc("STRING", NULL, 20)) == NULL) 
	{
		(void) userlog("Error allocating receive buffer\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		return -6;
	}
	
	/* ÉèÖÃÖØ×ö±êÖ¾£¬ËùÓÐµÄ·þÎñ½«²»ÔÙ¼ÇÂ¼ÖØ×öÈÕÖ¾,
	   ÓÐ³åÕýÒµÎñµÄÈ·ÈÏÒµÎñ½«°Ñ¾ÍµÄÄÚ´æÁ÷Ë®ºÍÖØ×öÊ±
	   ÐÂÉú³ÉµÄÒµÎñÁ÷Ë®¼ÇÂ¼µ½cfmAcceptFile.$OP_CODE
	   ÎÄ¼þÖÐ,¹©³åÕýÒµÎñÊ¹ÓÃ 
	*/
	strcpy(redoFlag,"redo");

	count = 0;
	processNum = 0;
	indexPosition = 0;

	signal(SIGTERM,QUIT);

	/* ´´½¨¹²ÏíÄÚ´æºÍÐÅºÅµÆ */
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

	/* ´ò¿ª¹²ÏíÄÚ´æ */
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
	
	/* ¿ªÊ¼¶ÁË÷ÒýÎÄ¼þ */
	while (1)
	{
		/* ²úÉú×Ó½ø³Ì */
		if(processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Fork child process,processNum=[%d]\n",processNum);
			#endif
			pid[processNum]=fork();
		}

		/* fork×Ó½ø³ÌÊ±·¢Éú´íÎó */
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
		/* ¸¸½ø³ÌÔËÐÐ,Èç¹ûfork×Ó½ø³ÌÍê±Ï,Í£Ö¹fork */ 
		else if(pid[processNum]>0)
		{ 
			if(processNum<REDOPROCESSNUM)
			{
				processNum++;
			}

			/* Èç¹û²úÉúµÄ×Ó½ø³ÌÊý×ã¹»,¸¸½ø³Ì¿ªÊ¼Ö´ÐÐÈÎÎñ·ÖÅä */
			if(processNum == REDOPROCESSNUM)
			{
				pid[processNum]=1;

				memset(memAccept,0,sizeof(memAccept));
				memset(redoFile,0,sizeof(redoFile));

				memset(idxRecord.redoFile,0,sizeof(idxRecord.redoFile));
				memset(idxRecord.memAccept,0,sizeof(idxRecord.memAccept));
				memset(idxRecord.svcName,0,sizeof(idxRecord.svcName));
				memset(idxRecord.phoneNo,0,sizeof(idxRecord.phoneNo));

				/* ½«¼ÇÂ¼×Ó½ø³Ì×´Ì¬µÄÊý×é³õÊ¼»¯ */
				for(i=0;i<REDOPROCESSNUM;i++)
				{
					idxRecord.redoPosition[i] = -1;
					idxRecord.processNo[i] = -1;
				}

				/* ½«±êÖ¾Êý×é³õÊ¼»¯ */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					blankFlag[i]=-1;

				redoPosition=0; i=0; recordCount=0;
			
				/* ¿ªÊ¼¶ÁË÷ÒýÎÄ¼þÖÐµÄ¼ÇÂ¼ */
				while(1)
				{
					/* ´ò¿ªË÷ÒýÎÄ¼þ */
					if((fpIndex = fopen(indexFile,"r")) == NULL)
					{
						userlog("´ò¿ªË÷ÒýÎÄ¼þ %s ´íÎó!\n", indexFile);
						fclose(fpErrlog);
						tpfree((char *)obuf32);
						tpfree((char *)obuf);
						tpfree((char *)rcvbuf);
						DelRedoShm();
						return -1;
					}

					/* ½«ÎÄ¼þÖ¸Õë¶¨Î»µ½ÉÏÒ»´Î¹Ø±ÕÎÄ¼þµÄÎ»ÖÃ */
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
					/* ¶ÁË÷ÒýÎÄ¼þÖÐµÄÊý¾Ý */
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

					/* ¼ÇÂ¼Ë÷ÒýÎÄ¼þµÄÎÄ¼þÖ¸ÕëÎ»ÖÃ */
					indexPosition = ftell(fpIndex);

					/* È¡¹»×ã¹»ÊýÄ¿µÄ¼ÇÂ¼ */
					if(feof(fpIndex)||i==REDOPROCESSNUM/2)
					{
						/* Óöµ½ÎÄ¼þ½áÊø¶Ôshmp->endFlagÉèÖÃ±êÖ¾,
						   1ÎªÖÐ¼ä±êÖ¾,µÈËùÓÐ´¦ÀíÍê³Éºó½«´Ë±êÖ¾
						   ÉèÖÃÎª2,×Ó½ø³Ì·¢ÏÖ´Ë±êÖ¾½«ÍË³ö 
						*/
						if(feof(fpIndex))shmp->endFlag=1;
						fclose(fpIndex);
						break;
					}
					fclose(fpIndex);
				}

				usedCount=0;
				/* ·ÖÅäÈÎÎñ¸ø×Ó½ø³Ì */
				while(1)
				{
					i=0;j=0;status=-1;
					/* ²éÕÒ¿ÕÏÐµÄ×Ó½ø³Ì */
					while(1)
					{
						/* Èç¹ûÓÐÃ»ÓÐÖ´ÐÐÍê³ÉµÄ½»·ÑÒµÎñ,¼ÌÐøµÈ´ý
						   shmp->phoneNo[i]="-"±íÊ¾·Ç½»·ÑÒµÎñ
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

						/* shmp->flag[i]=1±íÊ¾µÚi¸ö×Ó½ø³Ì¿ÕÏÐ */
						if(shmp->flag[i*REDOCOUNT]==1)
						{
							/* blankFlagÎª1±íÊ¾Õâ¸ö½ø³ÌÎª¿ÕÏÐ×´Ì¬ */
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

						/* µÈ´ýµ½ÓÐ×ã¹»¿ÕÏÐµÄ×Ó½ø³Ì */
						if(usedCount+j>=REDOPROCESSNUM/2)break;
						/* Èç¹ûÃ»ÓÐÕÒµ½×ã¹»µÄ¿ÕÏÐ½ø³Ì,µÈ´ý0.1Ãë */
						if(i==REDOPROCESSNUM&&usedCount+j<REDOPROCESSNUM/2)
						{
							i=0;
							sleep(0.05);
						}
					}
					/* ²éÕÒ¿ÕÏÐ×Ó½ø³ÌÑ­»·½áÊø */
					
					/* ²éÕÒ²¢·ÖÅäÓÐÏàÍ¬ºÅÂëµÄ´¦Àí */
					for(i=0;i<recordCount;i++)
					{
						l=0;
						blankPosition=0;
						/* Èç¹û´Ë½ø³ÌÎª²»¿ÕÏÐ×´Ì¬,½øÐÐÏÂÒ»´ÎÑ­»· */
						if(idxRecord.processNo[i]!=-1)continue;

						for(j=i+1;j<recordCount;j++)
						{
							/* Èç¹û´Ë½ø³ÌÎª²»¿ÕÏÐ×´Ì¬,½øÐÐÏÂÒ»´ÎÑ­»· */
							if(idxRecord.processNo[j]!=-1)
							{
								/* Èç¹ûÊôÓÚ×Ó½ø³ÌµÄÊý×éµÄµÚÒ»¸ö±êÖ¾Î»
								   ÔÚ±éÀúÍêËùÓÐµÄµç»°ºÅÂëºó»¹ÊÇ1,²¢ÇÒ
								   Ò»´ÎÈ¡³öµÄrecordCountÌõ¼ÇÂ¼ÖÐÓÐÏàÍ¬
								   µÄµç»°ºÅÂë,½«±êÖ¾ÉèÖÃÎªÕ¼ÓÃ×´Ì¬0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}

							/* ÕÒµ½µÄºÅÂë¶¼²»ÊÇ"-",¼´¶¼ÊÇ½»·ÑÏà¹ØµÄ²Ù×÷ */
							if(strcmp(idxRecord.phoneNo[i],"-")!=0&&strcmp(idxRecord.phoneNo[j],"-")!=0)
							{
								/* ÕÒµ½ÏàÍ¬µÄºÅÂë */
								if(strcmp(idxRecord.phoneNo[i],idxRecord.phoneNo[j])==0)
								{
									/* l¼ÇÂ¼ÓÐ¼¸¸öÏàÍ¬µÄºÅÂë¼ÇÂ¼ */
									/* pos¼ÇÂ¼±¾´Î±éÀúÕÒµ½µÄ¿ÕÏÐµÄ½ø³ÌºÅ */
									l++;pos=0;

									/* ±éÀúËùÓÐµÄ×Ó½ø³Ì,¸ø¿ÕÏÐ×Ó½ø³Ì·ÖÅäÈÎÎñ,
									   ¼ÇÂ¼µ½±êÖ¾Êý×éidxRecord.porcessNo
									*/
									for(k=0;k<REDOPROCESSNUM;k++)
									{
							#ifdef _DEBUGDETAIL1_
							userlog("START:i=%d,j=%d,k=%d,l=%d,blankPosition=%d,idxRecord.phoneNo[i]=%s,blankFlag[%d]=%d\n",
								i,j,k,l,blankPosition,idxRecord.phoneNo[i],k*REDOCOUNT,blankFlag[k*REDOCOUNT]);
							#endif
										/* blankPosition¼ÇÂ¼ÉÏÒ»´Î±éÀú
										   ÕÒµ½µÄ¿ÕµÄ×Ó½ø³ÌºÅ,Èç¹ûËüµÄ
										   Öµ´óÓÚ0,ÄÇÃ´k´ÓÕâ¸öÖµ¿ªÊ¼Ñ­»·Å
										*/
										if(blankPosition>0)
										{
											k=blankPosition;
										}

										/* µÚÒ»´ÎÕÒµ½¿ÕÏÐµÄ×Ó½ø³Ì */
										if(blankFlag[k*REDOCOUNT]==1&&l==1)
										{
											blankPosition=k;
											pos=k;
											/* ¼ÇÂ¼×Ó½ø³Ì±»·ÖÅäÈÎÎñµÄÎ»ÖÃ */
											idxRecord.processNo[i]=k*REDOCOUNT;
											idxRecord.processNo[j]=k*REDOCOUNT+l;
											/* ½«ÊôÓÚ´Ë×Ó½ø³ÌµÄÊý×éµÄ
											   µÚÒ»¸öÖµÉèÖÃÎªÕ¼ÓÃ×´Ì¬
											*/
											blankFlag[k*REDOCOUNT+l]=0;
							#ifdef _DEBUGDETAIL1_
							userlog("1:i=%d,j=%d,k=%d,l=%d,blankPosition=%d, idxRecord.processNo[i]=%d, idxRecord.processNo[j]=%d, blankFlag[%d]=0,idxRecord.phoneNo[i]=[%s]\n",
								i,j,k,l,blankPosition,
								idxRecord.processNo[i],
								idxRecord.processNo[j],
								k*REDOCOUNT+l,idxRecord.phoneNo[j]);
							#endif
											/* Èç¹ûÒÑ¾­ÕÒµ½¼ÇÂ¼µÄ×îºó
											   ½«ÊôÓÚ´Ë×Ó½ø³ÌµÄÊý×éµÚ
											   Ò»¸öÖµÉèÖÃÎªÕ¼ÓÃ×´Ì¬,
											   ÒÑ¾­·ÖÅäµÄÈÎÎñÊý¼Ó1
											*/
											if(j+1==recordCount)
											{
												blankFlag[k*REDOCOUNT]=0;
												usedCount++;
											}
											break;
										}
										/* µÚn´Î(n>1)ÕÒµ½¿ÕÏÐµÄ×Ó½ø³Ì */
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
										/* ÒÑ¾­µ½ÁËÊôÓÚ×Ó½ø³ÌÊý×éÊýÄ¿µÄÉÏÏÞ */
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
										/* ²éÕÒ¿ÕÏÐ×Ó½ø³ÌÅÐ¶Ï½áÊø */
									}
									/* ²éÕÒ¿ÕÏÐ×Ó½ø³ÌÑ­»·½áÊø */
								}
								/* ÕÒµ½ÏàÍ¬µÄºÅÂëÅÐ¶Ï½áÊø */
							}
							/* ÕÒµ½µÄºÅÂë¶¼²»ÊÇ"-"ÅÐ¶Ï½áÊø */
							/* ·Ç½»·Ñ²Ù×÷µÄ´¦Àí */
							else if(strcmp(idxRecord.phoneNo[j],"-")==0) 
							{
								/* Èç¹ûÊôÓÚ×Ó½ø³ÌµÄÊý×éµÄµÚÒ»¸ö±êÖ¾Î»
								   ÔÚ±éÀúÍêËùÓÐµÄµç»°ºÅÂëºó»¹ÊÇ1,²¢ÇÒ
								   Ò»´ÎÈ¡³öµÄrecordCountÌõ¼ÇÂ¼ÖÐÓÐÏàÍ¬
								   µÄµç»°ºÅÂë,½«±êÖ¾ÉèÖÃÎªÕ¼ÓÃ×´Ì¬0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}
							/* ·Ç½»·Ñ²Ù×÷µÄ´¦Àí */
							else if(strcmp(idxRecord.phoneNo[i],"-")==0)
								break;

							/* Èç¹ûÊôÓÚ×Ó½ø³ÌµÄÊý×éµÄµÚÒ»¸ö±êÖ¾Î»
							   ÔÚ±éÀúÍêËùÓÐµÄµç»°ºÅÂëºó»¹ÊÇ1,²¢ÇÒ
							   Ò»´ÎÈ¡³öµÄrecordCountÌõ¼ÇÂ¼ÖÐÓÐÏàÍ¬
							   µÄµç»°ºÅÂë,½«±êÖ¾ÉèÖÃÎªÕ¼ÓÃ×´Ì¬0
							*/
							if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
								j+1==recordCount)
							{
								blankFlag[pos*REDOCOUNT]=0;
								usedCount++;
							}

							/* ÒÑ¾­µ½ÁËÊôÓÚ×Ó½ø³ÌÊý×éÊýÄ¿µÄÉÏÏÞÔòÍË³ö */
							if(l==REDOCOUNT)break;
						}
					} /* ·ÖÅäÏàÍ¬ºÅÂëµÄÈÎÎñ */

					/* status¼ÇÂ¼ÓÐÃ»ÓÐ³É¹¦·ÖÅä,0Îª³É¹¦ */
					status=0;
					/* ¿ªÊ¼·ÖÅä²»Í¬ºÅÂëµÄÈÎÎñ */
					for(i=0;i<recordCount;i++)
					{
						/* Î´·ÖÅäµÄÈÎÎñ */
						if(idxRecord.processNo[i]==-1)
						{
							status=-1;
							for(k=0;k<REDOPROCESSNUM;k++)
							{
								/* ±ê¼ÇÊý×éÎª¿ÕÏÐ,1Îª¿ÕÏÐ,0ÎªÕ¼ÓÃ */
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
					} /* ·ÖÅäÆäËûµÄÈÎÎñ */

					/* ËùÓÐµÄÈÎÎñ¶¼ÒÑ¾­·ÖÅäÍê³É */
					if(status!=-1)break;
				}
				/* ·ÖÅäÈÎÎñ¸ø¿ÕÏÐ×Ó½ø³ÌÑ­»·½áÊø */

				#ifdef _DEBUGDETAIL2_
				for(i=0;i<recordCount;i++)
				{
					userlog("####i=[%d],idxRecord.processNo[%d]=%d\n",
						 i,i,idxRecord.processNo[i]);
				}
				#endif

				/* ¸ø·ÖÅäÍêÈÎÎñµÄ½ø³Ì¸³Öµ */
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

					/* ÐÞ¸Ä±êÖ¾ÎªÓÐÈÎÎñ×´Ì¬,ÏÈÐÞ¸ÄÊôÓÚ×Ó½ø³Ì·ÇµÚÒ»¸öÊý×éµÄ×´Ì¬ */
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
				/* ¸³Öµ¸ø¿ÕÏÐ×Ó½ø³ÌÑ­»·½áÊø */

				/* ×îºóÐÞ¸ÄÊôÓÚ×Ó½ø³ÌµÄµÚ1¸öÊý×éµÄ×´Ì¬ */
				for(i=0;i<recordCount;i++)
				{
					userlog("2-idxRecord.processNo[%d]=%d\n",i,idxRecord.processNo[i]);
					if((idxRecord.processNo[i]%REDOCOUNT)==0)
						shmp->flag[idxRecord.processNo[i]]=0;
				}
			}
			/* ¸¸½ø³Ì·ÖÅäÈÎÎñÅÐ¶Ï´¦Àí½áÊø */

			/* Èç¹û¶ÁË÷ÒýÎÄ¼þ½áÊø,ÍË³öÑ­»· */
			if(shmp->endFlag==1)
			{
				sleep(10);
				/* ½«±êÖ¾ÉèÖÃÎª2,Í¨Öª×Ó½ø³ÌÍË³ö */
				shmp->endFlag=2;

				/* Èç¹ûÓÐ×Ó½ø³ÌµÄÈÎÎñ»¹Ã»ÓÐÍê³É,¸¸½ø³ÌµÈ´ý */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					if(shmp->flag[i]!=1)
					{
						#ifdef _DEBUGDETAIL_
						userlog("Father process:shmp->flag[%d]=0!\n",i);
						#endif
						i--;sleep(1);
					}

				/* ËùÓÐ×Ó½ø³ÌÈÎÎñÖ´ÐÐÍê±Ï,¸¸½ø³ÌÍË³ö */
				break;
			}
		}
		/* ×Ó½ø³ÌµÄ´¦Àí */
		else if(pid[processNum]==0&&processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Child process start,pid[%d],processNum[%d]\n",getpid(),processNum);
			#endif

			/* ºÍ·þÎñ½¨Á¢Á¬½Ó */
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

			/* Ñ­»·²éÕÒ¸¸½ø³Ì·ÖÅäµÄÈÎÎñ */
			while(1)
			{
				count=0;status=0;
				/* ×Ó½ø³Ì±éÀúÊôÓÚ×Ô¼ºµÄÊý×é */
				for(i=0;i<REDOCOUNT;i++)
				{
					/* Èç¹û´Ë×Ó½ø³ÌµÄµÚ0¸öÊý×éÉÏ»¹Ã»ÓÐ·ÖÅäÈÎÎñÊ±
					   ×Ó½ø³ÌµÈ´ý¸¸½ø³Ì·ÖÅäÍêÈÎÎñÔÙ¼ÌÐøÖ´ÐÐ
					*/
					if(shmp->flag[processNum*REDOCOUNT]!=0)
					{
						
						#ifdef _DEBUGDETAIL2_
						userlog("pid[%d] i=[%d] processNum=[%d],shmp->flag[%d]=[%d],Child process sleep!\n",
							 getpid(),i,processNum,processNum*REDOCOUNT,shmp->flag[processNum*REDOCOUNT]);
						#endif
						i--;status++;
						/* Èç¹ûµÈ´ýÁË0.1*1000Ãë»¹Ã»ÓÐÈÎÎñ,ÍË³ö´ËÑ­»· */
						if(status==1000)break;
						sleep(0.05);
						continue;
					}	
					/* ¸¸½ø³Ì¸ø´Ë×Ó½ø³Ì·ÖÅäÍêÈÎÎñ,×Ó½ø³Ì¿ªÊ¼Ö´ÐÐ */
					else if(shmp->flag[processNum*REDOCOUNT+i]==0)
					{
						/* count¼ÇÂ¼Íê³ÉµÄÈÎÎñÊý */
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
						
						
						/* ´ò¿ªÖØ×öÈÕÖ¾ÎÄ¼þ */
						if((fp=fopen(redoFile, "r"))==NULL)
						{
							userlog("pid[%d] Open redolog file %s error !\n", 
								getpid(),redoFile);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							continue;
						}
						
						/* ½«ÎÄ¼þÖ¸ÕëÒÆ¶¯µ½Ö¸¶¨Î»ÖÃ */
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
						
						/* ¶Á·þÎñÃûºÍ»º³åÇøÀàÐÍ */
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


						/* Èç¹ûÊÇFML */
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
						
							/* ¶ÁÖØ×öÈÕÖ¾ÎÄ¼þÖÐ¾ßÌåµÄÄÚÈÝ */
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

							/* ½«¾ÉµÄÄÚ´æÁ÷Ë®ËÍ¸ø·þÎñ¹©³åÕýÒµÎñÊ¹ÓÃ */
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


						} /* FMLµ÷ÓÃ·þÎñ´¦Àí½áÊø */
						/* FML32´¦Àí */
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
						/* FML32µ÷ÓÃ·þÎñ´¦Àí½áÊø */
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
						/* ×Ó½ø³Ìµ÷ÓÃ·þÎñ´¦Àí½áÊø */
						
						fclose(fp);

						ftime(&finish);
						#ifdef _DEBUG_
						userlog("pid[%d] service [%s][%s] execute [%d] milseconds.\n",
							getpid(),memAccept,svcName,(finish.time-start.time)*1000+\
							(finish.millitm-start.millitm));
						#endif 
					}

				}/* ×Ó½ø³ÌÖ´ÐÐ±»·ÖÅäµÄÈÎÎñ½áÊø(for) */

				for(i=count-1;i>=0;i--)
				{
					/* ½«Ö´ÐÐÍê³ÉµÄÈÎÎñ±êÖ¾ÉèÖÃÎª¿ÉÓÃ,¸¸½ø³Ì
					   ½«¼ÌÐø¸ø×Ó½ø³Ì·ÖÅäÈÎÎñ,ÉèÖÃµÄË³ÐòÊÇÏÈ
					   ´óºóÐ¡ 
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

				/* ¸¸½ø³ÌµÄÈÎÎñÖ´ÐÐÍê³É */
				if(shmp->endFlag==2)
				{
					/* Èç¹û´æÔÚÓÐÃ»ÓÐÍê³ÉµÄÈÎÎñ,µ«¸¸½ø³ÌÒÑ¾­Ö´ÐÐÍê±Ï,
					   ×Ó½ø³Ì¼ÌÐøÖ´ÐÐÃ»ÓÐÍê³ÉµÄÈÎÎñ 
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

					/* Èç¹ûËùÓÐÈÎÎñ¶¼Ö´ÐÐÍê,×Ó½ø³ÌÍË³ö */
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
			/* Ñ­»·²éÕÒ¸¸½ø³Ì·ÖÅäµÄÈÎÎñ´¦Àí½áÊø */
			
			userlog("Child process end,pid[%d],process record:[%ld]\n",getpid(),completeCount);

			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			exit(0);
		} 
		/* ×Ó½ø³Ì´¦Àí½áÊø */
	} 
	/* ¶ÁË÷ÒýÎÄ¼þ´¦Àí½áÊø */
		
	userlog("Redo¹ý³ÌÒÑ¾­³É¹¦Íê³É ... \n");
	printf("Redo¹ý³ÌÒÑ¾­³É¹¦Íê³É ... \n");
	userlog("Redo¹ý³ÌÖÐµÄ´íÎóÐÅÏ¢ÒÑ¾­Ð´µ½ÎÄ¼þ:%s ... \n\n",errorFile);
	printf("Redo¹ý³ÌÖÐµÄ´íÎóÐÅÏ¢ÒÑ¾­Ð´µ½ÎÄ¼þ:%s ... \n\n",errorFile);

	#ifdef _DEBUGDETAIL2_
	/* ´òÓ¡¹²ÏíÄÚ´æµ±Ç°µÄÐÅÏ¢ */
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
		printf("»·¾³±äÁ¿$REDODIRµÄÖµÎªNULL!\n");
		exit(0);
	}
	memset(workDIR,0,sizeof(workDIR));
	strcpy(workDIR,envp);

	system("clear");
	printf("\n---------------------------------------------------------\n");
	printf("Program: redo\nVersion: V30.2003.01.20\nDeveloper: dangsl\n");
	printf("¹¦ÄÜÃèÊö£ºiBOSS²»¼ä¶ÏÔËÐÐ±£ÕÏÏµÍ³µÄÈÕÖ¾ÖØ×ö»Ö¸´\n");
	printf("---------------------------------------------------------\n\n");


	printf("\n---------------------------------------------------------\n");
	printf("              ÖØ×öÈÕÖ¾¹ý³Ì¿ªÊ¼                       ");
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
			printf("²ÎÊý³¤¶È´íÎó!\n");
			exit(0);
		}

		/* 1Ö¸»Ö¸´ÖØ×öErrorFileÖÐµÄÈÕÖ¾ */
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
			printf("²ÎÊý³¤¶È´íÎó!\n");
		}
		strcpy(startTime,argv[1]);
		strcpy(endTime,argv[2]);
	}
	else
	{
		printf("²ÎÊý´íÎó!\n");
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
	printf("ÇëÄú¼ì²é»Ö¸´»·¾³ÖÐÒÔÏÂÅäÖÃ:\n");
	printf("\t1.»·¾³±äÁ¿REDODIRºÍREDODIRÊÇ·ñÕýÈ·?\n");
	printf("\t2.ÇëÄúÈ·ÈÏ»Ö¸´»·¾³ÖÐ·þÎñÁ¬½ÓµÄÊý¾Ý¿âÊÇ·ñÕýÈ·?\n");
	printf("\t  ¼ì²éÅäÖÃÎÄ¼þ$REDODIR/cfg/sDBLogin.cfg\n");
	printf("\t3.$REDODIR/logÄ¿Â¼ÏÂµÄÎÄ¼þÊÇ·ñÎªÐèÒª»Ö¸´µÄ´óÇøµÄÖØ×öÈÕÖ¾?\n");
	printf("\t4.$REDODIR/logÄ¿Â¼ÏÂµÄcfmAcceptFile.*ÊÇ·ñÒÑ¾­É¾³ý?\n");
	printf("\t5.$REDODIR/logÄ¿Â¼ÏÂµÄcancelAcceptFile.*ÎÄ¼þµÄÄÚÈÝÊÇ²»ÊÇ\n");
	printf("\t  ÒÑ¾­½«Á½¸öÖÐ¼ä¼þ·þÎñÆ÷ÉÏµÄÎÄ¼þºÏ²¢ÎªÒ»¸ö?\n");
	printf("---------------------------------------------------------\n");

	printf("\n»·¾³±äÁ¿$REDODIRµÄÖµÎª:%s\n",workDIR);
	userlog("»·¾³±äÁ¿$REDODIRµÄÖµÎª:%s\n",workDIR);
	printf("¿ªÊ¼´¦Àí,µ±Ç°Ê±¼ä:%s\n",GetSysDatetime());

	if(redoType==0)
	{
		printf("ÄúÑ¡ÔñµÄÖØ×ö[%s]ºÍ[%s]Ê±¼ä¶ÎÄÚµÄ·þÎñ,ÊÇ·ñ¼ÌÐø?(y/n)\n",
			startTime,endTime);

		scanf("%c",&confirm);
		if(confirm!='y'&&confirm!='Y')
		{
			printf("ÄúÑ¡ÔñÁË²»Ö´ÐÐ£¬³ÌÐòÍË³ö¡£\n");
			userlog("ÄúÑ¡ÔñÁË²»Ö´ÐÐ£¬³ÌÐòÍË³ö¡£\n");
			exit(0);
		}
		getchar();

		printf("---------------------------------------------------------\n");
		printf("¿ªÊ¼´´½¨ÈÕÖ¾Ë÷ÒýÎÄ¼þ...\n");

		allRec = creatIndexFile(startTime,endTime);
		if( allRec< 0)
		{
			userlog("´´½¨Ë÷ÒýÎÄ¼þ´íÎó! ErrorNo:%ld\n",allRec);
			printf("´´½¨Ë÷ÒýÎÄ¼þ´íÎó! ErrorNo:%ld\n",allRec);
			exit(0);
		}

		userlog("Ë÷ÒýÎÄ¼þ´´½¨ÎÄ³É,¼ÇÂ¼ÊýÎª:%ld\n",allRec);
		printf("Ë÷ÒýÎÄ¼þ´´½¨ÎÄ³É,¼ÇÂ¼ÊýÎª:%ld\n",allRec);
		printf("´´½¨Ë÷ÒýÎÄ¼þ´¦ÀíÍê±Ï,µ±Ç°Ê±¼ä:%s\n",GetSysDatetime());
	}
	else
	{
		printf("ÏÂÃæ½«¿ªÊ¼»Ö¸´´íÎóÈÕÖ¾ÖÐµÄ·þÎñ,±¾´Î»Ö¸´µÄ´íÎóÈÕÖ¾½«¼ÇÂ¼ÔÚ$REDODIR/log/ErrorFile.newÖÐ.\n");
	}

	printf("ÏÂÃæ½«¸ù¾ÝË÷ÒýÎÄ¼þµÄÄÚÈÝµ÷ÓÃ·þÎñ,ÊÇ·ñ¼ÌÐø?(y/n)\n");
	scanf("%c",&confirm);
	if(confirm!='y'&&confirm!='Y')
	{
		printf("ÄúÑ¡ÔñÁË²»Ö´ÐÐ£¬³ÌÐòÍË³ö¡£\n");
		userlog("ÄúÑ¡ÔñÁË²»Ö´ÐÐ£¬³ÌÐòÍË³ö¡£\n");
		exit(0);
	}
	printf("¿ªÊ¼¸ù¾ÝË÷ÒýÎÄ¼þµÄÄÚÈÝµ÷ÓÃ·þÎñ...\n");
	succRec = redoServiceLog();
	if( succRec < 0)
	{
		userlog("Redo¹ý³ÌÊ§°Ü! ErrorNo:%ld\n",succRec);
		printf("Redo¹ý³ÌÊ§°Ü! ErrorNo:%ld\n",succRec);
		exit(0);
	}

	printf("µ÷ÓÃ·þÎñ´¦ÀíÍê±Ï,µ±Ç°Ê±¼ä:%s\n",GetSysDatetime());
	userlog("Redo¹ý³Ì´¦ÀíµÄ¼ÇÂ¼Êý:%d,³É¹¦Íê³ÉµÄ¼ÇÂ¼Êý:%d\n",
		allRec,succRec);
	printf("Redo¹ý³Ì´¦ÀíµÄ¼ÇÂ¼Êý:%d,³É¹¦Íê³ÉµÄ¼ÇÂ¼Êý:%d\n",
		allRec,succRec);
	printf("---------------------------------------------------------\n");
}
