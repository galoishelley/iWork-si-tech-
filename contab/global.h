#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <oratypes.h>
#include <oraxml.h>
#include <oraxsd.h>
#include <varargs.h>
#include <sqlcpr.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "des.h"

#define MASTERKEY "bossboss"
#define XML_CFG_FILE		"XML_CFG_FILE"

#define MAX_DATETIME_LEN 17
#define MAX_FILEPATH_LEN 100
#define MAX_FILENAME_LEN 100
#define MAX_NAME_LEN 10
#define MAX_PASS_LEN 100

#define MAX_PARAMETER_NUM		50
typedef struct tagParameter
{
	char paramName[64+1];
	char paramValue[256+1];
	char paramType[32+1];
}TParameter;

typedef struct tagDbParameters
{
	TParameter parameter[MAX_PARAMETER_NUM];
	int		parameterNum;
}TParameters;


#define MAX_FILE_NUM		10
typedef struct tagFile
{
	char fileID[255+1];
	char fileName[1023+1];
	char sendPath[1023+1];
	char sendBakPath[1023+1];
	char receivePath[1023+1];
	char isUseRootPath[5+1];
	char fileType[32+1];
	char remoteHostIp[32+1];
	char remoteUser[32+1];
	char remotePwd[64+1];
	char remoteTmpPath[1023+1];
	char remotePath[1023+1];
}TFile;

typedef struct tagFiles
{
	TFile file[MAX_FILE_NUM];
	char rootPath[1023+1];
	int		fileNum;
}TFiles;

#define MAX_DB_CONN_NUM		10
typedef struct tagDbConn
{
	char connName[32+1];
	char dbName[32+1];
	char dbUser[32+1];
	char dbPwd[32+1];
}TDbConn;
typedef struct tagDbConns
{
	TDbConn dbConn[MAX_DB_CONN_NUM];
	int		dbConnNum;
}TDbConns;

typedef struct tagDbConnNames
{
	TDbConn	dbConn[MAX_DB_CONN_NUM];
	int		useDbConnNum;
}TDbConnNames;

typedef struct tagComment
{
	char	comment[1023+1];
}TComment;

typedef struct tagProgram
{
	TParameters 	parameters;
	TFiles			files;
	TDbConnNames	dbConnNames;
	TComment		comment;
}TProgram;

/*edit by liuweib 20081224*/
/*
typedef struct tagFuncEnv
{
        char  *temp_buf;
        const char *totalDate;
        const char *opTime;
        const char *opCode;
        const char *loginNo;
        const char *orgCode;
        const char *loginAccept;
        const char *idNo;
        const char *phoneNo;
        const char *smCode;
        const char *belongCode;
        const char *cmdRight;
        const char *imsiNo;
        const char *simNo;
        const char *hlrCode;
        const char *offonAccept;
} tFuncEnv;
*/

#ifdef	__cplusplus
extern "C" {
#endif

/*
 *文件存在返回1，不存在返回0
 */
int fileExists(const char *filename);

/*
 *返回文件的大小，-1表示出错。
 */
long getFileSize(const char *filename);

/*
 *返回是否有正确的文件状态
 */
int checkFileStat(const char *pathname);

/*
 *是目录返回1，其它返回0
 */
int isDir(const char *dirname);


const char* getTime();
const char* getDateTime(int flag);
const char* getDate();

int initXml();
void destroyXml();
xmlnode	*xmlRootNode;
xmlctx *xmlCtx;

char programName[256];
char logFileName[1024];
void writelog();
void getHostIp(char *ip);
char* Trim(char *S);

int getDbConns(TDbConns *dbConns);
int getProgram(TProgram *program, const char *progName);


#ifdef	__cplusplus
}
#endif/*__cplusplus*/
#endif/*_GLOBAL_H*/
