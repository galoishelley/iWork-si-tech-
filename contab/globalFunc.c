#include "global.h"
#include "debug.h"

#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *文件存在返回1，不存在返回0
 */
int fileExists(const char *filename)
{
	return access(filename,F_OK) == 0?1:0;
}

/*
 *返回文件的大小，-1表示出错。
 */
long getFileSize(const char *filename)
{
	struct stat fstat;

	if(access(filename,F_OK) != 0)
	{
		return(-1);
	}
	if(stat(filename,&fstat) < 0)
	{
		return(-1);
	}
	return((long)fstat.st_size);
}

/*
 *返回是否有正确的文件状态
 */
int checkFileStat(const char *pathname)
{
	struct stat fstat;
	
	if(access(pathname,F_OK) < 0){
		return(-1);
	}
	
	if(stat(pathname,&fstat) < 0){
		return(-1);
	}
	
    return(1);
}

/*
 *是目录返回1，其它返回0
 */
int isDir(const char *dirname)
{
	struct stat bufstat;
	char filemode[10];
	
	if(stat(dirname,&bufstat) != 0)
		return(0);
	
	if ((bufstat.st_mode|_S_IFDIR)!=0)
		return(1);
	else
		return(0);
}

int initXml()
{
	uword ecode;
	const char *xmlfile;
	DBUG_ENTER("initXml");
	if ( (xmlfile= getenv(XML_CFG_FILE)) == NULL)
	{
		fprintf(stderr, "getenv(\"%s\") error\n", XML_CFG_FILE);
		return -1;
	}
	if (!fileExists(xmlfile))
	{
		fprintf(stderr, "cfg file[%s] not exists\n", xmlfile);
		return -1;
	}
	if (!(xmlCtx = xmlinit(&ecode, (const oratext *) 0,
				(void (*)(void *, const oratext *, uword)) 0,
				(void *) 0, (const xmlsaxcb *) 0, (void *) 0,
				(const xmlmemcb *) 0, (void *) 0,
				(const oratext *) 0)))
	{
		fprintf(stderr, "Failed to initialze XML parser, error %u\n", (unsigned) ecode);
		return -1;
	}
	if (ecode = xmlparse(xmlCtx, (oratext *) xmlfile, (oratext *) 0,
							XML_FLAG_DISCARD_WHITESPACE))
	{
		fprintf(stderr, "Parse failed, error %u\n", (unsigned) ecode);
		return -1;
	}
	xmlRootNode = getDocumentElement(xmlCtx);
	DBUG_LEAVE("initXml");
	return 0;
}

void destroyXml()
{
	DBUG_ENTER("destroyXml");
	if (xmlCtx != NULL)
	{
		xmlterm(xmlCtx);
	}
	xmlCtx = NULL;
	xmlRootNode = NULL;
	DBUG_LEAVE("destroyXml");
}

int getParameters(TParameters *parameters, xmlnode *inParamsNode)
{
	const oratext *name, *attr;
	xmlnodes    *nodes;
	int  i, n_nodes;
	xmlnode	*node;
	
	
	DBUG_ENTER("getParameters");
	parameters->parameterNum = 0;
	if (hasChildNodes(inParamsNode))
	{
		nodes = getChildNodes(inParamsNode);
		n_nodes = numChildNodes(nodes);
		for (i = 0; i < n_nodes; i++)
		{
			node=getChildNode(nodes, i);
			if (strncmp((char*)getNodeName(node), "parameter", 9) == 0)
			{
				if ( (attr=getAttribute(node,(oratext *)"paramName")) != (oratext *)NULL)
				{
					strcpy(parameters->parameter[parameters->parameterNum].paramName, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get parameter-->paramName error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"paramValue")) != (oratext *)NULL)
				{
					strcpy(parameters->parameter[parameters->parameterNum].paramValue, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get parameter-->paramValue error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"paramType")) != (oratext *)NULL)
				{
					strcpy(parameters->parameter[parameters->parameterNum].paramType, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get parameter-->paramType error\n");
					return -1;
				}
				
				parameters->parameterNum ++;
				if (parameters->parameterNum == MAX_PARAMETER_NUM) break;
			}
		}
	}
	DBUG_LEAVE("getParameters");
	return 0;
}

int getFiles(TFiles *files, xmlnode *filesNode)
{
	const oratext *name, *attr;
	xmlnodes    *nodes;
	int  i, n_nodes;
	xmlnode	*node;
	
	
	DBUG_ENTER("getFiles");
	files->fileNum = 0;
	if ( (attr=getAttribute(filesNode,(oratext *)"rootPath")) != (oratext *)NULL)
	{
		strcpy(files->rootPath, (char *)attr);
	}
	if (hasChildNodes(filesNode))
	{
		nodes = getChildNodes(filesNode);
		n_nodes = numChildNodes(nodes);
		for (i = 0; i < n_nodes; i++)
		{
			node=getChildNode(nodes, i);
			if (strncmp((char*)getNodeName(node), "file", 9) == 0)
			{
				if ( (attr=getAttribute(node,(oratext *)"fileID")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].fileID, (char *)attr);
				}

				if ( (attr=getAttribute(node,(oratext *)"fileName")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].fileName, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get file-->fileName error\n");
					return -1;
				}

				if ( (attr=getAttribute(node,(oratext *)"sendPath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].sendPath, (char *)attr);
				}

				if ( (attr=getAttribute(node,(oratext *)"sendBakPath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].sendBakPath, (char *)attr);
				}

				if ( (attr=getAttribute(node,(oratext *)"receivePath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].receivePath, (char *)attr);
				}

				if ( (attr=getAttribute(node,(oratext *)"isUseRootPath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].isUseRootPath, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get file-->isUseRootPath error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"fileType")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].fileType, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get file-->isUseRootPath error\n");
					return -1;
				}
				/*以下属性可选*/
				if ( (attr=getAttribute(node,(oratext *)"remoteHostIp")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].remoteHostIp, (char *)attr);
				}
				if ( (attr=getAttribute(node,(oratext *)"remoteUser")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].remoteUser, (char *)attr);
				}
				if ( (attr=getAttribute(node,(oratext *)"remotePwd")) != (oratext *)NULL)
				{
					spublicEnDePasswd((const char *)attr, files->file[files->fileNum].remotePwd,
						MASTERKEY, DES_DECRYPT);
				}
				if ( (attr=getAttribute(node,(oratext *)"remoteTmpPath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].remoteTmpPath, (char *)attr);
				}
				
				if ( (attr=getAttribute(node,(oratext *)"remotePath")) != (oratext *)NULL)
				{
					strcpy(files->file[files->fileNum].remotePath, (char *)attr);
				}
				files->fileNum ++;
				if (files->fileNum == MAX_FILE_NUM) break;
			}
		}
	}
	DBUG_LEAVE("getFiles");
	return 0;
}

int getDbConnNames(TDbConnNames *dbConnNames, xmlnode *dbConnNamesNode)
{
	const oratext *name, *attr;
	xmlnodes    *nodes;
	int  i,j, n_nodes, isFoundConn=0;
	xmlnode	*node;
	TDbConns dbConns;
	
	DBUG_ENTER("getDbConnNames");
	
	if (getDbConns(&dbConns) != 0)		/*得到所有的数据库连接*/
	{
		return -1;
	}

	dbConnNames->useDbConnNum = 0;
	if (hasChildNodes(dbConnNamesNode))
	{
		nodes = getChildNodes(dbConnNamesNode);
		n_nodes = numChildNodes(nodes);
		for (i = 0; i < n_nodes; i++)
		{
			node=getChildNode(nodes, i);
			if (strncmp((char*)getNodeName(node), "dbConnName", 9) == 0)
			{
				if ( (attr=getAttribute(node,(oratext *)"connName")) != (oratext *)NULL)
				{
					isFoundConn = 0;
					for( j = 0; j < dbConns.dbConnNum; j ++)
					{
						if (strcmp(dbConns.dbConn[j].connName, (char*)attr) == 0)
						{
							isFoundConn = 1;
							memcpy(&dbConnNames->dbConn[dbConnNames->useDbConnNum],
								(const void *)&dbConns.dbConn[j], sizeof(TDbConn));
						}
						if (isFoundConn == 1)
						{
							dbConnNames->useDbConnNum ++;
							break;
						}
						else
						{

							continue;
							
						}
					}
          if(isFoundConn == 0)
          	fprintf(stderr, "The database connect name is error:[%s] \n", dbConns.dbConn[j].connName);
					if (dbConnNames->useDbConnNum == MAX_DB_CONN_NUM) break;
				}
				else
				{
					fprintf(stderr, "get connName-->connName error\n");
					return -1;
				}
				
			}
		}
	}
	DBUG_LEAVE("getDbConnNames");
	return 0;
}

int getComment(TComment *comment, xmlnode *commentNode)
{
	const oratext *name, *attr;
	xmlnodes    *nodes;
	int  i, n_nodes;
	xmlnode	*node;
	
	DBUG_ENTER("getComment");
	if ( (attr=getNodeValue(commentNode)) != (oratext *)NULL)
	{
		strcpy(comment->comment, (char *)attr);
	}
	DBUG_LEAVE("getComment");
	return 0;
}

int getProgram(TProgram *program, const char *progName)
{
	const oratext *attr, *name;
	xmlnodes    *nodes;
	int  i, j, n_nodes;
	int	isProgramFound=0;
	xmlnode	*node;
	
	memset(program, 0, sizeof(TProgram));
	
	DBUG_ENTER("getProgram");
	if (hasChildNodes(xmlRootNode))
	{
		nodes = getChildNodes(xmlRootNode);
		n_nodes = numChildNodes(nodes);
		for (i = 0; i < n_nodes; i++)
		{
			node=getChildNode(nodes, i);
			name=getNodeName(node);
			if (strncmp((char*)name, "program", 7) == 0)
			{
				if ( (attr=getAttribute(node,(oratext *)"progName")) != (oratext *)NULL)
				{
					if (strcmp((char*)attr, progName) == 0)
					{
						isProgramFound = 1;
						nodes = getChildNodes(node);
						n_nodes = numChildNodes(nodes);
						for(j = 0; j < n_nodes; j ++)
						{
							node=getChildNode(nodes, j);
							name=getNodeName(node);
							if (strncmp((char *)name, "inParams", 8) == 0)
							{
								if (getParameters(&(program->parameters), node) != 0)
								{
									return -1;
								}
							}
							else if (strncmp((char *)name, "files", 8) == 0)
							{
								if (getFiles(&(program->files), node) != 0)
								{
									return -1;
								}
							}
							else if (strncmp((char *)name, "dbConnNames", 11) == 0)
							{
								if (getDbConnNames(&(program->dbConnNames), node) != 0)
								{
									return -1;
								}
							}
							else if (strncmp((char *)name, "comment", 7) == 0)
							{
								if (getComment(&(program->comment), node) != 0)
								{
									return -1;
								}
							}
							else
							{
								fprintf(stderr, "get program-->progName error\n");
								return -1;
							}
						}
						break;
					}
				}
				else
				{
					fprintf(stderr, "get program-->progName error\n");
					return -1;
				}
			}
		}
	}
	else
	{
		return -1;
	}

	/*get log file name*/
	memset(logFileName, 0, sizeof(logFileName));
	strcpy(logFileName, programName);
	for (i = 0; i < program->files.fileNum; i ++)
	{
		if (strcmp(program->files.file[i].fileType, "log") == 0)
		{
			if (strcmp(program->files.file[i].isUseRootPath, "true") == 0)
			{
				strcpy(logFileName, program->files.rootPath);
				strcat(logFileName, "/");
				strcat(logFileName, program->files.file[i].fileName);
			}
			else
			{			
				strcpy(logFileName, program->files.file[i].fileName);
			}
			break;
		}
	}
	
	DBUG_LEAVE("getProgram");
	return isProgramFound?0:-1;
}

int getDbConns(TDbConns *dbConns)
{
	const oratext *name, *attr;
	xmlnodes    *nodes;
	int  i, n_nodes;
	xmlnode	*node;
	
	
	DBUG_ENTER("getDbConns");
	dbConns->dbConnNum = 0;
	if (hasChildNodes(xmlRootNode))
	{
		nodes = getChildNodes(xmlRootNode);
		n_nodes = numChildNodes(nodes);
		for (i = 0; i < n_nodes; i++)
		{
			node=getChildNode(nodes, i);
			if (strncmp((char*)getNodeName(node), "dbConnect", 9) == 0)
			{
				if ( (attr=getAttribute(node,(oratext *)"connName")) != (oratext *)NULL)
				{
					strcpy(dbConns->dbConn[dbConns->dbConnNum].connName, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get dbConnect-->connName error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"dbName")) != (oratext *)NULL)
				{
					strcpy(dbConns->dbConn[dbConns->dbConnNum].dbName, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get dbConnect-->dbName error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"dbUser")) != (oratext *)NULL)
				{
					strcpy(dbConns->dbConn[dbConns->dbConnNum].dbUser, (char *)attr);
				}
				else
				{
					fprintf(stderr, "get dbConnect-->dbUser error\n");
					return -1;
				}
				if ( (attr=getAttribute(node,(oratext *)"dbPwd")) != (oratext *)NULL)
				{
					spublicEnDePasswd((const char *)attr, dbConns->dbConn[dbConns->dbConnNum].dbPwd,
						MASTERKEY, DES_DECRYPT);
				}
				else
				{
					fprintf(stderr, "get dbConnect-->dbPwd error\n");
					return -1;
				}
				
				dbConns->dbConnNum ++;
				if (dbConns->dbConnNum == MAX_DB_CONN_NUM) break;
			}
		}
	}
	else
	{
		return -1;
	}
	DBUG_LEAVE("getDbConns");
	return 0;
}

void writelog(va_alist)
	va_dcl
{
	const char *fmt;
	va_list ap;
	FILE *fp;
	
	fp = fopen(logFileName, "a+");
	if (fp == NULL) return;

	va_start(ap);
	fmt = va_arg(ap, char*);
	vfprintf(fp, fmt, ap);
	va_end(ap);
	
	if (fp) fclose(fp);
}

const char* getTime()
{
	time_t tm;
	static char curtime[MAX_DATETIME_LEN+1];
	
	tm = time(NULL);
	memset(curtime, 0, sizeof(curtime));
	strftime(curtime, 17+1, "%Y%m%d %H:%M:%S", localtime(&tm));
	return curtime;
}

/*
 *不同的flag:对应返回的日期格式
 *	1	:		YYYYMMDD HH:MI:SS
 *	2	:		YYYYMMDDHHMISS
 *	4	:		YYYYMMDD
 */
const char* getDateTime(int flag)
{
	time_t tm;
	static char curtime[MAX_DATETIME_LEN+1];
	
	tm = time(NULL);
	memset(curtime, 0, sizeof(curtime));
	switch(flag)
	{
		case 1:
			strftime(curtime, 17+1, "%Y%m%d %H:%M:%S", localtime(&tm));
			curtime[17] = '\0';
			break;
		case 2:
			strftime(curtime, 14+1, "%Y%m%d03/19/2004M%S", localtime(&tm));
			curtime[14] = '\0';
			break;
		case 4:
			strftime(curtime, 8+1, "%Y%m%d", localtime(&tm));
			curtime[8] = '\0';
			break;
		default:
			return NULL;
	}
	return curtime;
}

const char* getDate()
{
	time_t tm;
	static char curdate[8+1];
	
	tm = time(NULL);
	memset(curdate, 0, sizeof(curdate));
	strftime(curdate, 8+1, "%Y%m%d", localtime(&tm));
	return curdate;
}

void getHostIp(char *ip)
{
	struct utsname        myname;
	struct hostent        *host;
	struct in_addr        in;
	
	memset(ip, 0, sizeof(ip));
	if(uname(&myname)<0)
	{
		return;
	}
	
	host=gethostbyname(myname.nodename);
	if(host==NULL)
	{
		return;
	}
	
	memcpy(&in.s_addr,*host->h_addr_list,sizeof(in.s_addr));
	sprintf(ip,"%s",(char *)inet_ntoa(in));
	return;
}
/*
char* Trim(char *S)
{
	int I = 0, i = 0, L = 0;

	L = strlen(S) - 1;
	I = 0;
	while ( (I <= L) && (S[I] <= ' ') && (S[I] > 0) )
	{
		I ++;
	}
	if (I > L)
	{
		S[0] = '\0';
	}
	else
	{
		while ( (S[L] <= ' ')  && (S[L] > 0 ) )
		{
			L --;
		}
		strncpy(S, S + I, L + 1);
		S[L + 1 - I] = '\0';
	}
	return S;
}

*/
