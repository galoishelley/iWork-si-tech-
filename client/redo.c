/*
**    程序名：redo.c
**  功能描述：iBOSS不间断运行保障系统的日志重做恢复
**  使用方法：redo startTime endTime
**        如：redo 20030115000000 20030115190000
**  创建日期：2003.01.19
**  程序版本: V30.2003.01.20
**    创建人：dangsl
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

/* 系统中存在FML和FML32类型变量, 都需要定义 */
FBFR32 *obuf32;
FBFR   *obuf;

/* 接收Buffer */
char   *rcvbuf;

struct redoShm *shmp;

void QUIT(int signo)
{
	tpfree((char *)obuf32);
	tpfree((char *)obuf);
	tpfree((char *)rcvbuf);
	shmp->endFlag=2;
	DelRedoShm();
	userlog("程序被用户中断!\n");
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


/* 创建重做日志文件的索引文件，加快执行速度 */
long creatIndexFile(char *startTime,char *endTime)
{
	FILE *cfgFP,*logFP,*idxFP;
	char cfmAccept[100],title[2048],svcName[20],phoneNo[20];
	char fileName[FILENAME+FILENAME],tempCMD[FILENAME+FILENAME];
	char tempIdxFile[FILENAME+FILENAME];
	char cfgFile[FILENAME+FILENAME],logFile[FILENAME+FILENAME];
	char idxFile[FILENAME+FILENAME];

	/* 记录总共需要处理的记录总数 */
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
		printf("环境变量$REDODIR的值为NULL!\n");
		userlog("环境变量$REDODIR的值为NULL!\n");
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
		userlog("打开配置文件 %s 错误!\n",cfgFile);
		return -1;
	}

	if(!(idxFP=fopen(idxFile,"w+")))
	{
		userlog("打开索引文件 %s 错误!\n",idxFile);
		return -2;
	}

	/* 读配置文件$REDODIR/cfg/svrLogFile.cfg */
	while(!feof(cfgFP))
	{
		memset(fileName,0,sizeof(fileName));

		/* 根据$REDODIR/cfg/svrLogFile.cfg配置文件查找符合条件的记录 */
		if(fscanf(cfgFP,"%s",fileName)<0)
		{
			if(!(feof(cfgFP)))
			{
				userlog("读配置文件 %s 错误!\n",cfgFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -3;
			}
			else
			{
				userlog("读配置文件 %s 结束!\n",cfgFile);
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


			/* 根据配置文件中的文件名打开重做日志文件，
			   查询符合重做条件的记录 */
			if(!(logFP=fopen(logFile,"r")))
			{
				userlog("打开重做日志文件 %s 错误!\n",logFile);
				fclose(cfgFP);
				fclose(idxFP);
				return -4;
			}
			else
			{
				/* 读重做日志文件 */
				while(!(feof(logFP)))
				{
					memset(title,0,sizeof(title));
					if(fscanf(logFP,"%s",title)<0)
					{
						if(!(feof(logFP)))
						{
							userlog("读取重做日志文件 %s 错误!\n",logFile);
							fclose(cfgFP);
							fclose(idxFP);
							fclose(logFP);
							return -5;
						}
						else
						{
							userlog("读取重做日志文件 %s 结束!\n",logFile);
							fclose(logFP);
							break;
						}
					}
					/* 先找到<TitleMessage>这个标志 */
					else if(strcmp(title,"<TitleMessage>")==0) 
					{
						memset(cfmAccept,0,sizeof(cfmAccept));
						/* 读取内存流水 */
						if(fscanf(logFP,"%s",cfmAccept)<0)
						{
							if(!(feof(logFP)))
							{
								userlog("读取重做日志文件 %s 错误!\n",logFile);
								fclose(cfgFP);
								fclose(idxFP);
								fclose(logFP);
								return -6;
							}
							else
							{
								userlog("读取重做日志文件 %s 结束!\n",logFile);
								fclose(logFP);
								break;
							}
						}
						else 
						{
							idxPosition=ftell(logFP);
							/* 在时间范围内的记录 */
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
								/* 对于交费的特殊处理结束 */

								#ifdef _DEBUG_
								userlog("%s %ld %s %s %s\n",cfmAccept,ftell(logFP),logFile,svcName,phoneNo);
								#endif

								/* 将记录写入索引文件 */
								fprintf(idxFP,"%s %ld %s %s %s\n",
									cfmAccept,idxPosition,logFile,
									svcName,phoneNo);
								count++;
							}
							/* 判断是否符合条件的记录处理结束 */
						}
						/* 读内存流水处理结束 */
					}
					/* 查找<TitleMessage>标志处理结束 */
				}
				/* 判断读重做日志文件是否结束 */
			}
			/* 打开重做日志文件 */
		}
		/* 根据$REDODIR/cfg/svrLogFile.cfg配置文件查找符合
		   条件的记录处理结束 */
	}
	/* 循环读$REDODIR/cfg/svrLogFile.cfg文件处理结束 */

	memset(tempCMD,0,sizeof(tempCMD));
	strcpy(tempCMD,"sort ");
	strcat(tempCMD,tempIdxFile);

	memset(idxFile,0,sizeof(idxFile));

	strcpy(idxFile,workDIR);
	strcat(idxFile,"/");
	strcat(idxFile,"log/IndexFile");

	strcat(tempCMD," -o ");
	strcat(tempCMD,idxFile);
	/* 生成排序的正式索引文件$REDODIR/log/IndexFile */
	system(tempCMD);

	printf("创建索引文件[%s]成功完成 ...  \n",idxFile);
	userlog("创建索引文件[%s]成功完成 ... \n",idxFile);

	/* 返回符合重做条件的记录数 */
	return count;
}

long redoServiceLog()
{
	/* 索引文件, Stdout文件 */
	FILE *fpIndex, *fpErrlog,*fp;
	
	/* 返回值, 接收Buffer长度 */
	int ret,rcvlen;
	int i,j,k,l,m,blankPosition,pos;
	int pid[REDOPROCESSNUM+1],processNum,status;

	/* count记录成功完成的记录数 */
	long count,completeCount;
	int  recordCount,usedCount;

	/* 读取索引文件内容的变量 */
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

	/* redoPosition记录重做日志文件中的符合条件的记录的位置 */
	/* indexPosition记录索引文件中的文件指针的位置 */
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
		printf("环境变量$REDODIR的值为NULL!\n");
		userlog("环境变量$REDODIR的值为NULL!\n");
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
  		userlog("打开索引文件 %s 错误!\n", indexFile);
		return -1;
  	}

	if((fpErrlog = fopen(errorFile,"w+")) == NULL)
  	{
  		userlog("打开错误文件 %s 错误!\n", errorFile);
		fclose(fpIndex);
		return -2;
  	}
    
	/* 分配中间件输入缓冲区的空间 FML/FML32 */
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
	
	/* 分配中间件输出缓冲区使用的空间 */
	if((rcvbuf = (char *) tpalloc("STRING", NULL, 20)) == NULL) 
	{
		(void) userlog("Error allocating receive buffer\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		return -6;
	}
	
	/* 设置重做标志，所有的服务将不再记录重做日志,
	   有冲正业务的确认业务将把就的内存流水和重做时
	   新生成的业务流水记录到cfmAcceptFile.$OP_CODE
	   文件中,供冲正业务使用 
	*/
	strcpy(redoFlag,"redo");

	count = 0;
	processNum = 0;
	indexPosition = 0;

	signal(SIGTERM,QUIT);

	/* 创建共享内存和信号灯 */
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

	/* 打开共享内存 */
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
	
	/* 开始读索引文件 */
	while (1)
	{
		/* 产生子进程 */
		if(processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Fork child process,processNum=[%d]\n",processNum);
			#endif
			pid[processNum]=fork();
		}

		/* fork子进程时发生错误 */
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
		/* 父进程运行,如果fork子进程完毕,停止fork */ 
		else if(pid[processNum]>0)
		{ 
			if(processNum<REDOPROCESSNUM)
			{
				processNum++;
			}

			/* 如果产生的子进程数足够,父进程开始执行任务分配 */
			if(processNum == REDOPROCESSNUM)
			{
				pid[processNum]=1;

				memset(memAccept,0,sizeof(memAccept));
				memset(redoFile,0,sizeof(redoFile));

				memset(idxRecord.redoFile,0,sizeof(idxRecord.redoFile));
				memset(idxRecord.memAccept,0,sizeof(idxRecord.memAccept));
				memset(idxRecord.svcName,0,sizeof(idxRecord.svcName));
				memset(idxRecord.phoneNo,0,sizeof(idxRecord.phoneNo));

				/* 将记录子进程状态的数组初始化 */
				for(i=0;i<REDOPROCESSNUM;i++)
				{
					idxRecord.redoPosition[i] = -1;
					idxRecord.processNo[i] = -1;
				}

				/* 将标志数组初始化 */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					blankFlag[i]=-1;

				redoPosition=0; i=0; recordCount=0;
			
				/* 开始读索引文件中的记录 */
				while(1)
				{
					/* 打开索引文件 */
					if((fpIndex = fopen(indexFile,"r")) == NULL)
					{
						userlog("打开索引文件 %s 错误!\n", indexFile);
						fclose(fpErrlog);
						tpfree((char *)obuf32);
						tpfree((char *)obuf);
						tpfree((char *)rcvbuf);
						DelRedoShm();
						return -1;
					}

					/* 将文件指针定位到上一次关闭文件的位置 */
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
					/* 读索引文件中的数据 */
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

					/* 记录索引文件的文件指针位置 */
					indexPosition = ftell(fpIndex);

					/* 取够足够数目的记录 */
					if(feof(fpIndex)||i==REDOPROCESSNUM/2)
					{
						/* 遇到文件结束对shmp->endFlag设置标志,
						   1为中间标志,等所有处理完成后将此标志
						   设置为2,子进程发现此标志将退出 
						*/
						if(feof(fpIndex))shmp->endFlag=1;
						fclose(fpIndex);
						break;
					}
					fclose(fpIndex);
				}

				usedCount=0;
				/* 分配任务给子进程 */
				while(1)
				{
					i=0;j=0;status=-1;
					/* 查找空闲的子进程 */
					while(1)
					{
						/* 如果有没有执行完成的交费业务,继续等待
						   shmp->phoneNo[i]="-"表示非交费业务
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

						/* shmp->flag[i]=1表示第i个子进程空闲 */
						if(shmp->flag[i*REDOCOUNT]==1)
						{
							/* blankFlag为1表示这个进程为空闲状态 */
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

						/* 等待到有足够空闲的子进程 */
						if(usedCount+j>=REDOPROCESSNUM/2)break;
						/* 如果没有找到足够的空闲进程,等待0.1秒 */
						if(i==REDOPROCESSNUM&&usedCount+j<REDOPROCESSNUM/2)
						{
							i=0;
							sleep(0.05);
						}
					}
					/* 查找空闲子进程循环结束 */
					
					/* 查找并分配有相同号码的处理 */
					for(i=0;i<recordCount;i++)
					{
						l=0;
						blankPosition=0;
						/* 如果此进程为不空闲状态,进行下一次循环 */
						if(idxRecord.processNo[i]!=-1)continue;

						for(j=i+1;j<recordCount;j++)
						{
							/* 如果此进程为不空闲状态,进行下一次循环 */
							if(idxRecord.processNo[j]!=-1)
							{
								/* 如果属于子进程的数组的第一个标志位
								   在遍历完所有的电话号码后还是1,并且
								   一次取出的recordCount条记录中有相同
								   的电话号码,将标志设置为占用状态0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}

							/* 找到的号码都不是"-",即都是交费相关的操作 */
							if(strcmp(idxRecord.phoneNo[i],"-")!=0&&strcmp(idxRecord.phoneNo[j],"-")!=0)
							{
								/* 找到相同的号码 */
								if(strcmp(idxRecord.phoneNo[i],idxRecord.phoneNo[j])==0)
								{
									/* l记录有几个相同的号码记录 */
									/* pos记录本次遍历找到的空闲的进程号 */
									l++;pos=0;

									/* 遍历所有的子进程,给空闲子进程分配任务,
									   记录到标志数组idxRecord.porcessNo
									*/
									for(k=0;k<REDOPROCESSNUM;k++)
									{
							#ifdef _DEBUGDETAIL1_
							userlog("START:i=%d,j=%d,k=%d,l=%d,blankPosition=%d,idxRecord.phoneNo[i]=%s,blankFlag[%d]=%d\n",
								i,j,k,l,blankPosition,idxRecord.phoneNo[i],k*REDOCOUNT,blankFlag[k*REDOCOUNT]);
							#endif
										/* blankPosition记录上一次遍历
										   找到的空的子进程号,如果它的
										   值大于0,那么k从这个值开始循环�
										*/
										if(blankPosition>0)
										{
											k=blankPosition;
										}

										/* 第一次找到空闲的子进程 */
										if(blankFlag[k*REDOCOUNT]==1&&l==1)
										{
											blankPosition=k;
											pos=k;
											/* 记录子进程被分配任务的位置 */
											idxRecord.processNo[i]=k*REDOCOUNT;
											idxRecord.processNo[j]=k*REDOCOUNT+l;
											/* 将属于此子进程的数组的
											   第一个值设置为占用状态
											*/
											blankFlag[k*REDOCOUNT+l]=0;
							#ifdef _DEBUGDETAIL1_
							userlog("1:i=%d,j=%d,k=%d,l=%d,blankPosition=%d, idxRecord.processNo[i]=%d, idxRecord.processNo[j]=%d, blankFlag[%d]=0,idxRecord.phoneNo[i]=[%s]\n",
								i,j,k,l,blankPosition,
								idxRecord.processNo[i],
								idxRecord.processNo[j],
								k*REDOCOUNT+l,idxRecord.phoneNo[j]);
							#endif
											/* 如果已经找到记录的最后
											   将属于此子进程的数组第
											   一个值设置为占用状态,
											   已经分配的任务数加1
											*/
											if(j+1==recordCount)
											{
												blankFlag[k*REDOCOUNT]=0;
												usedCount++;
											}
											break;
										}
										/* 第n次(n>1)找到空闲的子进程 */
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
										/* 已经到了属于子进程数组数目的上限 */
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
										/* 查找空闲子进程判断结束 */
									}
									/* 查找空闲子进程循环结束 */
								}
								/* 找到相同的号码判断结束 */
							}
							/* 找到的号码都不是"-"判断结束 */
							/* 非交费操作的处理 */
							else if(strcmp(idxRecord.phoneNo[j],"-")==0) 
							{
								/* 如果属于子进程的数组的第一个标志位
								   在遍历完所有的电话号码后还是1,并且
								   一次取出的recordCount条记录中有相同
								   的电话号码,将标志设置为占用状态0
								*/
								if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
									j+1==recordCount)
								{
									blankFlag[pos*REDOCOUNT]=0;
									usedCount++;
								}
								continue;
							}
							/* 非交费操作的处理 */
							else if(strcmp(idxRecord.phoneNo[i],"-")==0)
								break;

							/* 如果属于子进程的数组的第一个标志位
							   在遍历完所有的电话号码后还是1,并且
							   一次取出的recordCount条记录中有相同
							   的电话号码,将标志设置为占用状态0
							*/
							if(l>0&&blankFlag[pos*REDOCOUNT]==1&&
								j+1==recordCount)
							{
								blankFlag[pos*REDOCOUNT]=0;
								usedCount++;
							}

							/* 已经到了属于子进程数组数目的上限则退出 */
							if(l==REDOCOUNT)break;
						}
					} /* 分配相同号码的任务 */

					/* status记录有没有成功分配,0为成功 */
					status=0;
					/* 开始分配不同号码的任务 */
					for(i=0;i<recordCount;i++)
					{
						/* 未分配的任务 */
						if(idxRecord.processNo[i]==-1)
						{
							status=-1;
							for(k=0;k<REDOPROCESSNUM;k++)
							{
								/* 标记数组为空闲,1为空闲,0为占用 */
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
					} /* 分配其他的任务 */

					/* 所有的任务都已经分配完成 */
					if(status!=-1)break;
				}
				/* 分配任务给空闲子进程循环结束 */

				#ifdef _DEBUGDETAIL2_
				for(i=0;i<recordCount;i++)
				{
					userlog("####i=[%d],idxRecord.processNo[%d]=%d\n",
						 i,i,idxRecord.processNo[i]);
				}
				#endif

				/* 给分配完任务的进程赋值 */
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

					/* 修改标志为有任务状态,先修改属于子进程非第一个数组的状态 */
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
				/* 赋值给空闲子进程循环结束 */

				/* 最后修改属于子进程的第1个数组的状态 */
				for(i=0;i<recordCount;i++)
				{
					userlog("2-idxRecord.processNo[%d]=%d\n",i,idxRecord.processNo[i]);
					if((idxRecord.processNo[i]%REDOCOUNT)==0)
						shmp->flag[idxRecord.processNo[i]]=0;
				}
			}
			/* 父进程分配任务判断处理结束 */

			/* 如果读索引文件结束,退出循环 */
			if(shmp->endFlag==1)
			{
				sleep(10);
				/* 将标志设置为2,通知子进程退出 */
				shmp->endFlag=2;

				/* 如果有子进程的任务还没有完成,父进程等待 */
				for(i=0;i<REDOPROCESSNUM*REDOCOUNT;i++)
					if(shmp->flag[i]!=1)
					{
						#ifdef _DEBUGDETAIL_
						userlog("Father process:shmp->flag[%d]=0!\n",i);
						#endif
						i--;sleep(1);
					}

				/* 所有子进程任务执行完毕,父进程退出 */
				break;
			}
		}
		/* 子进程的处理 */
		else if(pid[processNum]==0&&processNum<REDOPROCESSNUM)
		{
			#ifdef _DEBUG_
			userlog("Child process start,pid[%d],processNum[%d]\n",getpid(),processNum);
			#endif

			/* 和服务建立连接 */
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

			/* 循环查找父进程分配的任务 */
			while(1)
			{
				count=0;status=0;
				/* 子进程遍历属于自己的数组 */
				for(i=0;i<REDOCOUNT;i++)
				{
					/* 如果此子进程的第0个数组上还没有分配任务时
					   子进程等待父进程分配完任务再继续执行
					*/
					if(shmp->flag[processNum*REDOCOUNT]!=0)
					{
						
						#ifdef _DEBUGDETAIL2_
						userlog("pid[%d] i=[%d] processNum=[%d],shmp->flag[%d]=[%d],Child process sleep!\n",
							 getpid(),i,processNum,processNum*REDOCOUNT,shmp->flag[processNum*REDOCOUNT]);
						#endif
						i--;status++;
						/* 如果等待了0.1*1000秒还没有任务,退出此循环 */
						if(status==1000)break;
						sleep(0.05);
						continue;
					}	
					/* 父进程给此子进程分配完任务,子进程开始执行 */
					else if(shmp->flag[processNum*REDOCOUNT+i]==0)
					{
						/* count记录完成的任务数 */
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
						
						
						/* 打开重做日志文件 */
						if((fp=fopen(redoFile, "r"))==NULL)
						{
							userlog("pid[%d] Open redolog file %s error !\n", 
								getpid(),redoFile);
							fprintf(fpErrlog, "%s %ld %s\n",  \
								memAccept,redoPosition,   \
								redoFile);
							continue;
						}
						
						/* 将文件指针移动到指定位置 */
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
						
						/* 读服务名和缓冲区类型 */
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


						/* 如果是FML */
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
						
							/* 读重做日志文件中具体的内容 */
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

							/* 将旧的内存流水送给服务供冲正业务使用 */
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


						} /* FML调用服务处理结束 */
						/* FML32处理 */
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
						/* FML32调用服务处理结束 */
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
						/* 子进程调用服务处理结束 */
						
						fclose(fp);

						ftime(&finish);
						#ifdef _DEBUG_
						userlog("pid[%d] service [%s][%s] execute [%d] milseconds.\n",
							getpid(),memAccept,svcName,(finish.time-start.time)*1000+\
							(finish.millitm-start.millitm));
						#endif 
					}

				}/* 子进程执行被分配的任务结束(for) */

				for(i=count-1;i>=0;i--)
				{
					/* 将执行完成的任务标志设置为可用,父进程
					   将继续给子进程分配任务,设置的顺序是先
					   大后小 
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

				/* 父进程的任务执行完成 */
				if(shmp->endFlag==2)
				{
					/* 如果存在有没有完成的任务,但父进程已经执行完毕,
					   子进程继续执行没有完成的任务 
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

					/* 如果所有任务都执行完,子进程退出 */
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
			/* 循环查找父进程分配的任务处理结束 */
			
			userlog("Child process end,pid[%d],process record:[%ld]\n",getpid(),completeCount);

			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			exit(0);
		} 
		/* 子进程处理结束 */
	} 
	/* 读索引文件处理结束 */
		
	userlog("Redo过程已经成功完成 ... \n");
	printf("Redo过程已经成功完成 ... \n");
	userlog("Redo过程中的错误信息已经写到文件:%s ... \n\n",errorFile);
	printf("Redo过程中的错误信息已经写到文件:%s ... \n\n",errorFile);

	#ifdef _DEBUGDETAIL2_
	/* 打印共享内存当前的信息 */
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
		printf("环境变量$REDODIR的值为NULL!\n");
		exit(0);
	}
	memset(workDIR,0,sizeof(workDIR));
	strcpy(workDIR,envp);

	system("clear");
	printf("\n---------------------------------------------------------\n");
	printf("Program: redo\nVersion: V30.2003.01.20\nDeveloper: dangsl\n");
	printf("功能描述：iBOSS不间断运行保障系统的日志重做恢复\n");
	printf("---------------------------------------------------------\n\n");


	printf("\n---------------------------------------------------------\n");
	printf("              重做日志过程开始                       ");
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
			printf("参数长度错误!\n");
			exit(0);
		}

		/* 1指恢复重做ErrorFile中的日志 */
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
			printf("参数长度错误!\n");
		}
		strcpy(startTime,argv[1]);
		strcpy(endTime,argv[2]);
	}
	else
	{
		printf("参数错误!\n");
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
	printf("请您检查恢复环境中以下配置:\n");
	printf("\t1.环境变量REDODIR和REDODIR是否正确?\n");
	printf("\t2.请您确认恢复环境中服务连接的数据库是否正确?\n");
	printf("\t  检查配置文件$REDODIR/cfg/sDBLogin.cfg\n");
	printf("\t3.$REDODIR/log目录下的文件是否为需要恢复的大区的重做日志?\n");
	printf("\t4.$REDODIR/log目录下的cfmAcceptFile.*是否已经删除?\n");
	printf("\t5.$REDODIR/log目录下的cancelAcceptFile.*文件的内容是不是\n");
	printf("\t  已经将两个中间件服务器上的文件合并为一个?\n");
	printf("---------------------------------------------------------\n");

	printf("\n环境变量$REDODIR的值为:%s\n",workDIR);
	userlog("环境变量$REDODIR的值为:%s\n",workDIR);
	printf("开始处理,当前时间:%s\n",GetSysDatetime());

	if(redoType==0)
	{
		printf("您选择的重做[%s]和[%s]时间段内的服务,是否继续?(y/n)\n",
			startTime,endTime);

		scanf("%c",&confirm);
		if(confirm!='y'&&confirm!='Y')
		{
			printf("您选择了不执行，程序退出。\n");
			userlog("您选择了不执行，程序退出。\n");
			exit(0);
		}
		getchar();

		printf("---------------------------------------------------------\n");
		printf("开始创建日志索引文件...\n");

		allRec = creatIndexFile(startTime,endTime);
		if( allRec< 0)
		{
			userlog("创建索引文件错误! ErrorNo:%ld\n",allRec);
			printf("创建索引文件错误! ErrorNo:%ld\n",allRec);
			exit(0);
		}

		userlog("索引文件创建文成,记录数为:%ld\n",allRec);
		printf("索引文件创建文成,记录数为:%ld\n",allRec);
		printf("创建索引文件处理完毕,当前时间:%s\n",GetSysDatetime());
	}
	else
	{
		printf("下面将开始恢复错误日志中的服务,本次恢复的错误日志将记录在$REDODIR/log/ErrorFile.new中.\n");
	}

	printf("下面将根据索引文件的内容调用服务,是否继续?(y/n)\n");
	scanf("%c",&confirm);
	if(confirm!='y'&&confirm!='Y')
	{
		printf("您选择了不执行，程序退出。\n");
		userlog("您选择了不执行，程序退出。\n");
		exit(0);
	}
	printf("开始根据索引文件的内容调用服务...\n");
	succRec = redoServiceLog();
	if( succRec < 0)
	{
		userlog("Redo过程失败! ErrorNo:%ld\n",succRec);
		printf("Redo过程失败! ErrorNo:%ld\n",succRec);
		exit(0);
	}

	printf("调用服务处理完毕,当前时间:%s\n",GetSysDatetime());
	userlog("Redo过程处理的记录数:%d,成功完成的记录数:%d\n",
		allRec,succRec);
	printf("Redo过程处理的记录数:%d,成功完成的记录数:%d\n",
		allRec,succRec);
	printf("---------------------------------------------------------\n");
}
