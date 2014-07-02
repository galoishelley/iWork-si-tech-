#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fml32.h>
#include <fml.h>
#include "atmi.h"
#include "general.flds.h"
#include "general32.flds.h"

#define WORKDIR "WORKDIR"
#define FILENAME  100
#define ACCEPTLEN 100
#define _DEBUG_

char workdir[50];

char *spublicGetEnvDir( char *env)
{
	char *envp;
	if ((envp = getenv(env)) == NULL) 
	{
		userlog("取环境变量$WORKDIR错误!");
		return NULL;
	}
	return envp;
}

int spublicGetFileName(const char *file, char rtn_path[FILENAME])
{

	memset(rtn_path,0,FILENAME+1);
	strcpy(rtn_path,workdir);
	strcat(rtn_path,"/");
	strcat(rtn_path,file);

	return 0;
}


long CreatIndexFile(char *starttime,char *endtime)
{
	FILE *cfgfp,*logfp,*idxfp;
	char accept[100],title[20];
	char filename[FILENAME],tempcmd[FILENAME],tempidxfile[FILENAME];
	char cfgfile[FILENAME],logfile[FILENAME],idxfile[FILENAME];

	/* 记录总共需要处理的记录总数 */
	long count;

	memset(cfgfile,0,sizeof(cfgfile));
	memset(idxfile,0,sizeof(idxfile));
	memset(logfile,0,sizeof(logfile));
	memset(tempcmd,0,sizeof(tempcmd));
	memset(tempidxfile,0,sizeof(tempidxfile));

	if(spublicGetFileName("cfg/svrLogFile.cfg",cfgfile)!=0)
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		userlog("环境变量$WORKDIR的值为NULL!\n");
		return -1;
	}
	if(spublicGetFileName("log/IndexFile.tmp",idxfile)!=0)
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		userlog("环境变量$WORKDIR的值为NULL!\n");
		return -1;
	}

	strcpy(tempidxfile,idxfile);

	if(!(cfgfp=fopen(cfgfile,"r")))
	{
		userlog("打开配置文件 %s 错误!\n",cfgfile);
		return -1;
	}

	if(!(idxfp=fopen(idxfile,"w+")))
	{
		userlog("打开索引文件 %s 错误!\n",idxfile);
		return -2;
	}

	while(!feof(cfgfp))
	{
		memset(filename,0,sizeof(filename));
		if(fscanf(cfgfp,"%s",filename)<0)
		{
			if(!(feof(cfgfp)))
			{
				userlog("读配置文件 %s 错误!\n",cfgfile);
				fclose(cfgfp);
				fclose(idxfp);
				return -3;
			}
			else
			{
				userlog("读配置文件 %s 结束!\n",cfgfile);
				fclose(cfgfp);
				fclose(idxfp);
				break;
			}
		}
		else 
		{
			memset(logfile,0,sizeof(logfile));
			memset(tempcmd,0,sizeof(tempcmd));
			strcpy(tempcmd,"log/");
			strcat(tempcmd,filename);

			if(spublicGetFileName(tempcmd,logfile)!=0)
			{
				printf("环境变量$WORKDIR的值为NULL!\n");
				userlog("环境变量$WORKDIR的值为NULL!\n");
				return -1;
			}

			if(!(logfp=fopen(logfile,"r")))
			{
				userlog("打开重做日志文件 %s 错误!\n",logfile);
				fclose(cfgfp);
				fclose(idxfp);
				return -4;
			}
			else
			{
				while(!(feof(logfp)))
				{
					memset(title,0,sizeof(title));
					if(fscanf(logfp,"%s",title)<0)
					{
						if(!(feof(logfp)))
						{
							userlog("读取重做日志文件 %s 错误!\n",logfile);
							fclose(cfgfp);
							fclose(idxfp);
							fclose(logfp);
							return -5;
						}
						else
						{
							userlog("读取重做日志文件 %s 结束!\n",logfile);
							fclose(logfp);
							break;
						}
					}
					else if(strcmp(title,"<TitleMessage>")==0) 
					{
						memset(accept,0,sizeof(accept));
						if(fscanf(logfp,"%s",accept)<0)
						{
							if(!(feof(logfp)))
							{
								userlog("读取重做日志文件 %s 错误!\n",logfile);
								fclose(cfgfp);
								fclose(idxfp);
								fclose(logfp);
								return -6;
							}
							else
							{
								userlog("读取重做日志文件 %s 结束!\n",logfile);
								fclose(logfp);
								break;
							}
						}
						else 
						{
#ifdef _DEBUG_
							userlog("%s %ld %s\n",accept,ftell(logfp),logfile);
#endif
							/* 在时间范围内的记录 */
							if((strcmp(accept,starttime)>=0)&&(strcmp(accept,endtime)<=0))
							{
								fprintf(idxfp,"%s %ld %s\n",accept,ftell(logfp),logfile);
								count++;
							}
						}
					}
				}
			}
		}
	}

	memset(tempcmd,0,sizeof(tempcmd));
	strcpy(tempcmd,"sort ");
	strcat(tempcmd,tempidxfile);

	memset(idxfile,0,sizeof(idxfile));
	if(spublicGetFileName("log/IndexFile",idxfile)!=0)
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		userlog("环境变量$WORKDIR的值为NULL!\n");
		return -1;
	}

	strcat(tempcmd," -o ");
	strcat(tempcmd,idxfile);
	system(tempcmd);

	printf("\n创建索引文件 %s 成功完成 ...  \n\n",idxfile);
	userlog("\n创建索引文件 %s 成功完成 ... \n\n",idxfile);

	return count;
}

long RedoServiceLog()
{
	/* 系统中存在FML和FML32类型变量, 都需要定义 */
	FBFR32 *obuf32;
	FBFR   *obuf;
	
	/* 接收Buffer */
	char *rcvbuf;
	
	/* 索引文件, Stdout文件 */
	FILE *fpIndex, *fpErrlog,*fp;
	
	/* 返回值, 接收Buffer长度 */
	int ret,rcvlen;

	/* count记录成功完成的记录数 */
	long count;
	
	/* 读取Stdout文件内容的变量 */
	char			strService[30];
	char			strBufferType[6];
	char			strFlag[5];
    
	/* 读取索引文件内容的变量 */
	char			strSerial[ACCEPTLEN];
	long			lPosition;
	char			strRedoFile[50];

	char 			indexfile[FILENAME];
	char 			errorfile[FILENAME];

    
	memset(indexfile,0,sizeof(indexfile));
	memset(errorfile,0,sizeof(errorfile));

	if(spublicGetFileName("log/IndexFile",indexfile)!=0)
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		userlog("环境变量$WORKDIR的值为NULL!\n");
		return -1;
	}
	if(spublicGetFileName("log/ErrorFile",errorfile)!=0)
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		userlog("环境变量$WORKDIR的值为NULL!\n");
		return -1;
	}

	if((fpIndex = fopen(indexfile,"r")) == NULL)
  	{
  		userlog("打开索引文件 %s 错误!\n", indexfile);
		return -1;
  	}

	if((fpErrlog = fopen(errorfile,"w+")) == NULL)
  	{
  		userlog("打开错误文件 %s 错误!\n", errorfile);
		fclose(fpIndex);
		return -2;
  	}
    
	/* Attach to System/T as a Client Process */
	if (tpinit((TPINIT *) NULL) == -1) 
	{
		userlog("tpinit error:tperrno=%d,tpstrerror=%s\n",tperrno,tpstrerror(tperrno));
		fclose(fpIndex);
		fclose(fpErrlog);
		return -3;
	}
	
	if ((obuf32 = (FBFR32 *)tpalloc("FML32",NULL,Fneeded32(1, 50*1024+50)))==(FBFR32 *)NULL) 
	{
		userlog("tpalloc failed in open_acct\n");
		fclose(fpIndex);
		fclose(fpErrlog);
		tpterm();
		return -4;
	}
	
	if ((obuf = (FBFR *)tpalloc("FML",NULL,Fneeded(1, 50*1024+50)))==(FBFR *)NULL) 
	{
		userlog("tpalloc failed in open_acct\n");
		tpfree((char *)obuf32);
		fclose(fpIndex);
		fclose(fpErrlog);
		tpterm();
		return -5;
	}
	
	if((rcvbuf = (char *) tpalloc("STRING", NULL, 20)) == NULL) 
	{
		(void) userlog("Error allocating receive buffer\n");
		tpfree((char *)obuf32);
		tpfree((char *)obuf);
		fclose(fpIndex);
		fclose(fpErrlog);
		tpterm();
		return -6;
	}
	
	strcpy(strFlag,"redo");

	count = 0;
	while (!feof(fpIndex))
	{
	

		fscanf(fpIndex, "%s %ld %s\n", strSerial,&lPosition,strRedoFile);

#ifdef _DEBUG_
		userlog("%s %ld %s\n",strSerial, lPosition, strRedoFile);	
#endif

		if((fp=fopen(strRedoFile, "r"))==NULL)
  		{
  			userlog("Open stdout file %s error !\n", strRedoFile);
			fclose(fpIndex);
			fclose(fpErrlog);
			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			return -7;
  		}
  		
  		if (-1 == fseek(fp, lPosition, SEEK_SET))
  		{
  			userlog("feek error, file:%s, position:%ld\n",strRedoFile, lPosition);
			fclose(fp);
			fclose(fpIndex);
			fclose(fpErrlog);
			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			return -8;
  		}
#ifdef _DEBUG_
  		userlog("file <%s> position is: %ld\n", strRedoFile, lPosition);
#endif
  		
  		if (-1 == fscanf(fp, "%s %s\n", strService, strBufferType))
  		{
  			userlog("fscanf file <%s> error!\n", strRedoFile);
			fclose(fp);
			fclose(fpIndex);
			fclose(fpErrlog);
			tpfree((char *)obuf32);
			tpfree((char *)obuf);
			tpfree((char *)rcvbuf);
			tpterm();
			return -9;
  		}
		
#ifdef _DEBUG_
		userlog("strService=%s, strBufferType=%s\n",strService, strBufferType);	
#endif

		if (strcmp(strBufferType, "fml") == 0)
		{
			Finit(obuf, Fneeded(1, 50*1024+20));
		
			ret = Fextread(obuf,fp);
			if (-1 == Fchg(obuf, RedoFlag, 0, strFlag, 0))
    			{
	    			userlog("Error occur on FChg of RedoFlag\n");
				fclose(fp);
				fclose(fpIndex);
				fclose(fpErrlog);
				tpfree((char *)obuf32);
				tpfree((char *)obuf);
				tpfree((char *)rcvbuf);
				tpterm();
				return -10;
    			}

        		ret = tpcall(strService, (char *)obuf, 0, (char **)&rcvbuf, &rcvlen, (long)0);
			if(ret == -1) 
			{
				userlog("tpcall error:tperrno=%d,tpstrerror=%s",tperrno,tpstrerror(tperrno));
				fprintf(fpErrlog, "%s %ld %s\n", strSerial,lPosition,strRedoFile);
				count--;
			}
		}
		else if (strcmp(strBufferType, "fml32") == 0)
		{
			Finit32(obuf32, Fneeded32(1, 50*1024+20));
		
			ret = Fextread32(obuf32,fp);
			if (-1 == Fchg32(obuf32, RedoFlag32, 0, strFlag, 0))
    			{
	    			userlog("Error occur on FChg32 of strService\n");
				fclose(fp);
				fclose(fpIndex);
				fclose(fpErrlog);
				tpfree((char *)obuf32);
				tpfree((char *)obuf);
				tpfree((char *)rcvbuf);
				tpterm();
				return -11;
    			}
        		ret = tpcall(strService, (char *)obuf32, 0, (char **)&rcvbuf, &rcvlen, (long)0);
			if(ret == -1) 
			{
				userlog("tpcall error:tperrno=%d,tpstrerror=%s",tperrno,tpstrerror(tperrno));
				fprintf(fpErrlog, "%s %ld %s\n", strSerial,lPosition,strRedoFile);
				count--;
			}
		}
		else
		{
			userlog("Buffer Type Error: %s\n", strBufferType);
			count--;
		}
		fclose(fp);

		count++;

#ifdef _DEBUG_
		userlog("tpcall ok: strSerial=%s, strService=%s\n", strSerial, strService);
#endif
	}
		
	userlog("Redo过程已经成功完成 ... \n");
	printf("Redo过程已经成功完成 ... \n");
	userlog("Redo过程中的错误信息已经写到文件:%s ... \n\n",errorfile);
	printf("Redo过程中的错误信息已经写到文件:%s ... \n\n",errorfile);
	
	fclose(fp);
	fclose(fpIndex);
	fclose(fpErrlog);
	
	tpfree((char *)obuf32);
	tpfree((char *)obuf);
	tpfree((char *)rcvbuf);
	tpterm();
	
	return count;
}

void main(int argc, char *argv[])
{
	char starttime[15],endtime[15];
	char *envp;
	long allrec,sussrec;

	envp = spublicGetEnvDir(WORKDIR);
	if ( envp == NULL )
	{
		printf("环境变量$WORKDIR的值为NULL!\n");
		exit(0);
	}
	memset(workdir,0,sizeof(workdir));
	strcpy(workdir,envp);

	printf("\n ------------------------------------------------- \n\n");
	printf("              重做日志过程开始                       ");
	printf("\n\n ------------------------------------------------- \n");

	printf("\n环境变量$WORKDIR的值为:%s\n",workdir);
	userlog("\n环境变量$WORKDIR的值为:%s\n",workdir);

	printf("\n当前系统时间:%s\n",system("date"));

	if(argc==1)
	{
		strcpy(starttime,"00000000000000");
		strcpy(endtime,"99999999999999");
	}
	else if(argc==2)
	{
		if(strlen(argv[1])<14)
		{
			printf("参数长度错误!\n");
		}
		strcpy(starttime,argv[1]);
		strcpy(endtime,"99999999999999");
	}
	else if(argc==3)
	{
		if((strlen(argv[1])<14)||(strlen(argv[2])<14))
		{
			printf("参数长度错误!\n");
		}
		strcpy(starttime,argv[1]);
		strcpy(endtime,argv[2]);
	}
	else
	{
		printf("参数错误!\n");
		printf("Usage:\n");
		printf("\tredo [StartDateTime] [EndDateTime]\n");
		printf("Or:\n");
		printf("\tredo [yyyymmddhhmmss] [yyyymmddhhmmss]\n");
		printf("Example:\n");
		printf("\tredo 20020101010101 20021231235959 \n");
		exit(0);
	}

	allrec = CreatIndexFile(starttime,endtime);
	if( allrec< 0)
	{
		userlog("创建索引文件错误! ErrorNo:%ld\n",allrec);
		printf("创建索引文件错误! ErrorNo:%ld\n",allrec);
		exit(0);
	}
	
	sussrec = RedoServiceLog();
	if( sussrec < 0)
	{
		userlog("Redo过程失败! ErrorNo:%ld\n",sussrec);
		printf("Redo过程失败! ErrorNo:%ld\n",sussrec);
		exit(0);
	}
	

	printf("\n当前系统时间:%s\n",system("date"));
	userlog("Redo过程处理的记录数:%d,成功完成的记录数:%d\n\n",allrec,sussrec);
	printf("Redo过程处理的记录数:%d,成功完成的记录数:%d\n\n",allrec,sussrec);
}
