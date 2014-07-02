/*
 *程序名称：sWebPrnCli
 *调用方法：sWebPrnCli
 *程序说明：
 *			1、要求必须传入两个参数：hDbLogin，hParams1；其中第三个参数hFontSize可选。
 *			2、调用服务sWebPrnSvr：传入两个参数：GPARM_0, 	cParams1 参数串1；GPARM_1, 	cLabelName	数据库登录名
 *
 *作者：lugz
 */
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <fml.h>
#include <stdarg.h>
#include <sys/types.h>
#include <math.h>
#include "atmi.h"
#include "general.flds.h"
#include "ferror.h"


#define MAX_PARMS_NUM 50
#define MAXINTLEN 11
#define MAXLOGINLEN 32
#define TEMPBUFLEN 256
#define hDbLogin "hDbLogin"
#define hParams1 "hParams1"
#define hFontSize "hFontSize"
#define RptServiceName "sWebPrnSvr"
#define MAXFIELDCOUNT 100
#define MAXSIGNCOUNT 100
#define MAXDATAREGIONCOUNT 2
#define MAXLOGINMAPCOUNT 4

#define MAXPAGEHEIGHT 43
#define SEPARATELINENUM 2

#define FieldSeparator '|'
#define XXSMALLFontWidth 6.5
#define XSMALLFontWidth 9.0

/*已有的属性标记*/
#define HAVEAllSIGNS	"CRLH"

/*字段的标号种类*/
typedef struct tagSignStruct
{
	char name;
	int  val;
} SignStruct;

/* Declare Field struct type*/
typedef struct tagFieldStruct  
{
	int		isExists;/*0存在，-1不存在*/
	int		iSignCount[MAXDATAREGIONCOUNT];
	SignStruct FieldSign[MAXDATAREGIONCOUNT][MAXSIGNCOUNT];
} FieldStruct;           

/*一条记录的结构*/
typedef struct tagRecordStruct  
{
	char RecType;
	char Format[MAXDATAREGIONCOUNT][256];
	char Data[MAXDATAREGIONCOUNT][256];
} RecordStruct;

/*记录从上一次以来列的输出情况*/
typedef struct tagRecordStatInfo
{
	char RecType;
	int iOldFieldCount[MAXDATAREGIONCOUNT];
	int iCurFieldCount[MAXDATAREGIONCOUNT];
	int RowCount[MAXFIELDCOUNT];
} RecordStatInfo;

/*程序的主要参数结构*/
typedef struct tagProgStruct
{
	char cParams1[256];
	char cDbLogin[256];
	char cFontSize[20 + 1];
	char HtmlFileName[256];
	char *recvflag;
	int  connect_flag;
	int  open_flag[MAXDATAREGIONCOUNT];
	int  occ;
	FILE *outFile;
	FILE *destFile[MAXDATAREGIONCOUNT];
	long recvlen;
	int TableLength[MAXDATAREGIONCOUNT][MAXFIELDCOUNT];/*记录正文数据的最大宽度*/
	FBFR *send_fb;
	FBFR *recv_fb;

	int IsOutTableHead[MAXDATAREGIONCOUNT];
	int IsOutTableTail[MAXDATAREGIONCOUNT];
	int IsOutPreHead[MAXDATAREGIONCOUNT];
	int IsOutPreTail[MAXDATAREGIONCOUNT];
	
	int HeadRecordCount;
	int TailRecordCount;
	int TotalRecordCount;/*不是指具体的记录数，而是指打印报表的行数*/

	FILE *ColHeadFile[MAXDATAREGIONCOUNT];					/*当前的列头*/
	int ColHeadRecNum;
	int RealRegionCount;
	
	int PrintType;				/*0:TABLE, 1:PRE*/
	int isMerged;				/*合并标志*/
} ProgStruct;

void getFileName(char *HtmlFileName);
void InitFieldStruct(FieldStruct *pFieldStruct);
int getRptType(char *line, ProgStruct *pCurProgStruct);
int getParams(char *line, ProgStruct *pCurProgStruct);
char x2c(char *what);
void unescape_url(char *url);

int getValueByName(char *line, char* value, char *name, 
					ProgStruct *pCurProgStruct, int iHasDefault);
void MyInit( ProgStruct *pProgStruct );
void MyDistroy( ProgStruct *pProgStruct);
void MyErrorExit( ProgStruct *pProgStruct, int ErrNo, const char *ErrMsg);
void CreateHtmlHead( ProgStruct *pCurProgStruct );
void CreateHtmlTail( ProgStruct *pCurProgStruct );
int CreateHtmlBody(RecordStruct *pCurRecordStruct, ProgStruct *pCurProgStruct,
			   FieldStruct *pCurFields, RecordStatInfo *CurStatInfo);
int MergeFile( ProgStruct *pCurProgStruct);
int WriteOtherBuf(RecordStruct *pCurRecordStruct, ProgStruct *pCurProgStruct,
			   FieldStruct *pCurFields, RecordStatInfo *CurStatInfo);
int WriteEFBuf(RecordStruct *pCurRecordStruct, ProgStruct *pCurProgStruct,
			   FieldStruct *pCurFields, RecordStatInfo *CurStatInfo);
int WriteTBuf(RecordStruct *pCurRecordStruct, ProgStruct *pCurProgStruct,
			   FieldStruct *pCurFields, RecordStatInfo *CurStatInfo);
int ParseRecordToField(RecordStruct *pCurRecordStruct, FieldStruct *pCurFields,
						RecordStatInfo *CurStatInfo, ProgStruct *pCurProgStruct);
char* lTrim(char *S);
char* rTrim(char *S);
char* Trim(char *S);

/*把空格转化为“&nbsp;”*/
void SpaceToNBSP(char *src, char *dest);

int FillSigns(char *FormatWord, SignStruct *pSignStruct, int *iSignCount,
			  ProgStruct *pCurProgStruct );
char* getInput(char **input, ProgStruct *pCurProgStruct);


main(int argc, char *argv[])
{
	int i = 0;
	unsigned int size= 0;
        long recvlen,revent,ret;
	ProgStruct CurProgStruct;
	RecordStruct CurRecordStruct;
	FieldStruct CurFields[MAXFIELDCOUNT];
	RecordStatInfo CurStatInfo;
	char tmpBuf[256];
	CurStatInfo.RecType='\0'; 
	for(i = 0; i < MAXDATAREGIONCOUNT; i ++)
	{
		CurStatInfo.iOldFieldCount[i] = 0;
		CurStatInfo.iCurFieldCount[i] = 0;
	}
	for(i = 0; i < MAXFIELDCOUNT; i ++ )
	{
		InitFieldStruct(&CurFields[i]);
		CurStatInfo.RowCount[i] = 0;
	}
  
	MyInit( &CurProgStruct );

	while(1)
	{
		/* Init the receive buffer */
		if(Finit(CurProgStruct.recv_fb,(FLDLEN)Fsizeof(CurProgStruct.recv_fb)) == -1)
		{
    			MyErrorExit( &CurProgStruct, 2000, "Finit recv_fb error!\n");
		}
	
		/* Receive date from service */
		ret = tprecv(CurProgStruct.connect_flag,(char **)&CurProgStruct.recv_fb,
			&CurProgStruct.recvlen,TPNOCHANGE,&revent);
		
		if (( ret == -1 ) && (revent != TPEV_SVCSUCC) && revent != TPEV_SENDONLY )
		{
    			MyErrorExit( &CurProgStruct, 2001, "tprecv recv_fb error!\n");
		}
		
		if ( (revent==TPEV_SVCERR) || ( revent==TPEV_SVCFAIL ) )
		{
    			MyErrorExit( &CurProgStruct, 2002, "返回TPEV_SVCERR，或TPEV_SVCFAIL!\n");
		}
	
		/* Get receive record count */
		CurProgStruct.occ = Foccur(CurProgStruct.recv_fb,GPARM_2);
		if(CurProgStruct.occ == -1)
		{
    			MyErrorExit( &CurProgStruct, 2003, "Foccur recv_fb error!\n");
			
		}
		
		if ( CurProgStruct.occ == 0 ) break;
		for (i = 0; i < CurProgStruct.occ; i++)
		{
			/* Get parms value */
			if ( Fget(CurProgStruct.recv_fb, GPARM_2, i, (char *)tmpBuf, NULL) < 0 )
			{
    				MyErrorExit( &CurProgStruct, 2004, "Fget recv_fb GPARM_2 error!\n");
			}
			Trim(tmpBuf);
			CurRecordStruct.RecType = tmpBuf[0];
			if ( Fget(CurProgStruct.recv_fb, GPARM_4, i, (char *)CurRecordStruct.Format[0], NULL) < 0 )
			{
    				MyErrorExit( &CurProgStruct, 2006, "Fget recv_fb GPARM_4 error!\n");
			}
			if ( Fget(CurProgStruct.recv_fb, GPARM_5, i, (char *)CurRecordStruct.Format[1], NULL) < 0 )
			{
    				MyErrorExit( &CurProgStruct, 2007, "Fget recv_fb GPARM_5 error!\n");
			}
			if ( Fget(CurProgStruct.recv_fb, GPARM_6, i, (char *)CurRecordStruct.Data[0], NULL) < 0 )
			{
    				MyErrorExit( &CurProgStruct, 2008, "Fget recv_fb GPARM_6 error!\n");
			}
			if ( Fget(CurProgStruct.recv_fb, GPARM_7, i, (char *)CurRecordStruct.Data[1], NULL) < 0 )
			{
    				MyErrorExit( &CurProgStruct, 2009, "Fget recv_fb GPARM_7 error!\n");
			}
			CreateHtmlBody(&CurRecordStruct, &CurProgStruct, CurFields, &CurStatInfo );
		} /* End for occ */


		strcpy(CurProgStruct.recvflag, "RecvOK");
		if( revent == TPEV_SENDONLY )
		{
			/* Send receive status to service */
			if (tpsend(CurProgStruct.connect_flag,(char *)CurProgStruct.recvflag,
				   strlen(CurProgStruct.recvflag),TPRECVONLY,&revent) ==-1   )
			{
    				MyErrorExit( &CurProgStruct, 2010, "tpsend recvflag error!\n");
			}
		}
	}
	if (CurProgStruct.isMerged != 1)
	{
		MergeFile(&CurProgStruct);
	}
	CreateHtmlTail( &CurProgStruct );
	MyDistroy( &CurProgStruct );
}

void MyInit( ProgStruct *pProgStruct )
{
	char tmpBuf[256], *InputStr=NULL;
	int i = 0, j = 0;

	pProgStruct->connect_flag = -1;
	pProgStruct->occ = -1;
	pProgStruct->recvlen = 4096;
	pProgStruct->send_fb = NULL;
	pProgStruct->recv_fb = NULL;
	pProgStruct->recvflag = NULL;
	pProgStruct->TotalRecordCount = 0;
	pProgStruct->HeadRecordCount = 0;
	pProgStruct->TailRecordCount = 0;
	pProgStruct->RealRegionCount = 0;
	pProgStruct->isMerged = 0;
	pProgStruct->PrintType = 0;
	for (i = 0; i < MAXDATAREGIONCOUNT; i ++)
	{
		for(j = 0; j < MAXFIELDCOUNT; j ++)
		{
			pProgStruct->TableLength[i][j] = -1;
		}
		pProgStruct->open_flag[i] = -1;
		pProgStruct->IsOutTableHead[i] = -1;
		pProgStruct->IsOutTableTail[i] = -1;
		pProgStruct->IsOutPreHead[i] = -1;
		pProgStruct->IsOutPreTail[i] = -1;
	}

	memset(pProgStruct->cParams1, 0, sizeof(pProgStruct->cParams1) );

	getFileName(pProgStruct->HtmlFileName);