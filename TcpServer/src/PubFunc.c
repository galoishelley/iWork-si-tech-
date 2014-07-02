/**********************************************************************\
 *
 * File: PubFunc.cpp
 *
 *	NOTE!!! 使用VI打开此文件时, 请将 tablespace设置为4 (:set ts=4)
 *
\**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "SysVar.h"
#include "PubFunc.h"
#define _debug_
#define init(a)   memset(a,0,sizeof(a));/*modify miaoyl at 20110322*/

#define PASSWD_MAX_LENGTH	8
const char *pstr = "PLMOKNIJBUHYGVTFCRDXESZAQWqazwsxedcrfvtgbyhnujmikolp1234567890~`!@#$%^&*()-_+=|\\[]{}.,><?/;:";
/*@加密函数
 *@description				长度不能长于PASSWD_MAX_LENGTH个字符，加密前后的长度不变。
 *@param		src			明文原字串
 *@output		dest		加密后字串
 *@return					0成功；其它失败
*/
int Encrypt(const char* src, char *dest)
{
	int i, len;
	int tmp_int = 0;
	
	len = strlen(src);
	if (len > PASSWD_MAX_LENGTH)
	{
		return -1;
	}
	memset(dest, 0, PASSWD_MAX_LENGTH);
	
	for (i = 0; i < len; i++)
	{
		if (isdigit( (int)src[i] ) )
		{
			tmp_int = src[i] - '0';
			dest[i] =pstr[ (tmp_int + i + 1) *( len / 2 + 1)];
		}
		else
		{
			return -2;
		}
	}
	dest[i] = '\0';
	return 0;
}

/*@解密函数
 *@description				长度不能长于PASSWD_MAX_LENGTH个字符，解密前后的长度不变。
 *@param		src			密文原字串
 *@output		dest		解密后字串
 *@return					0成功；其它失败
*/
int unEncrypt(const char* src, char *dest)
{
	int i, j, len, len2;
	int tmp_int = 0;
	
	len = strlen(src);
	if (len > PASSWD_MAX_LENGTH)
	{
		return -1;
	}
	memset(dest, 0, PASSWD_MAX_LENGTH);
	len2= strlen(pstr);
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < len2; j++)
		{
			if (src[i] == pstr[j])
			{
				dest[i] = j / (len / 2 + 1) - i - 1 + '0';
				break;
			}
		}
		
	}
	dest[i] = '\0';

	return 0;
}

/*********************************************************************\
 * Function: RTrim
 * Action:	去掉字符串右面的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
char *
RTrim( char *in_str )
{
	int	i=0;
	int in_str_len=0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}
	
	in_str_len = strlen(in_str);
	for( i=in_str_len-1; i>=0; i-- )
	{
		if( in_str[i]==0x20 || in_str[i]==0x09 ||	in_str[i]==0x0a || in_str[i]==0x0d )
		{
			in_str[i]=0;
		}
		else
		{
			break;
		}
	}

	return( in_str );
}

/*********************************************************************\
 * Function: LTrim
 * Action:	去掉字符串左面的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
char *
LTrim( char *in_str )
{
	int	i=0;
	int	j=0;
	int in_str_len=0;

	if( in_str==NULL || strlen(in_str)==0 )
	{
		return( in_str );
	}
	
	in_str_len = strlen(in_str);
	for( i=0; i<in_str_len; i++ )
	{
		if( in_str[i]!=0x20 && in_str[i]!=0x09 &&
			in_str[i]!=0x0a && in_str[i]!=0x0d )
		{
			break;
		}
	}
	
	memmove( in_str, in_str+i, in_str_len-i );
	in_str[in_str_len-i]=0;
	return( in_str );
}

/*********************************************************************\
 * Function: Trim
 * Action:	去掉字符串左右两边的空格、TAB、回车
 *
 * Input:	char	*in_str
 * Output:			无
 * Return:	char	*in_str
\*********************************************************************/
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

/*********************************************************************\
 * Function:	ReadConfig
 * Action:		读取配置文件中的各个配置参数
 *
 * Input:		const char *i_cfgfile	-	参数所在文件名
 *							此文件名必须是相对于绝对路径文件名
 *							此文件内容说明：
 *								此文件中"参数类别"不能重复
 *								同一个"参数类别"下不能有相同的"参数名"
 *				const char *i_section	-	参数类别
 *				const char *i_key		-	参数名
 * Output:
 *				char *o_value			-	参数值输出缓冲地址
 *				char *o_value_len		-	参数值输出缓冲长度
 * Return:
 *			 0	- 成功
 *			-1	- 参数不合法
 *			-2	- 打开文件出错
 *			-3	- 找不到参数类别 section
 *			-4	- 找不到参数 key
\*********************************************************************/
int
ReadConfig( const char *i_cfgfile,
			const char *i_section,
			const char *i_key,
			char *o_value,
			int o_value_len )
{
	FILE	*fp = NULL;
	char	section_name[64];
	char	key_name[64];
	char	value[512];
	char	line_buf[512];
	char	ifindflag = 0;
	char	*p_assign = NULL;
	char	*p_comment = NULL;
	char	*p_left_bracket = NULL;
	char	*p_right_bracket = NULL;

	memset(section_name,0x00,sizeof(section_name));
	memset(key_name,0x00,sizeof(key_name));
	memset(value,0x00,sizeof(value));
	memset(line_buf,0x00,sizeof(line_buf));
	
	if( i_cfgfile==NULL ||i_section==NULL ||i_key==NULL ||o_value==NULL ||o_value_len<=0 )
	{
		return( -1 );
	}
	
	if( (fp=fopen(i_cfgfile,"r"))==NULL )
	{
		return( -2 );
	}
	
	snprintf( section_name, sizeof(section_name), "[%s]", i_section );

	/* -- 查找 section --*/
	while( fgets( line_buf, sizeof(line_buf), fp ) != NULL )
	{
		Trim(line_buf);
		if( line_buf[0]=='#' )
		{
			memset( line_buf, 0x00, sizeof(line_buf) );
			continue;
		}
		else
		{
			if( ( p_comment=strchr(line_buf,'#') ) != NULL )
			{
				*p_comment = 0;
				RTrim(line_buf);
			}
			if( strcmp(line_buf, section_name) != 0 )
			{
				memset( line_buf, 0x00, sizeof(line_buf) );
				continue;
			}
			ifindflag = 1;
			break;
		}
	}

	if( ifindflag==0 )
	{
		fclose( fp );
		return( -3 );
	}
	
	/* -- 查找 key --*/
	ifindflag = 0;
	memset( line_buf, 0x00, sizeof(line_buf) );
	snprintf( key_name, sizeof(key_name), "%s", i_key );

	while( fgets( line_buf, sizeof(line_buf), fp ) != NULL )
	{
		Trim(line_buf);
		if( line_buf[0]=='#' )
		{
			memset( line_buf, 0x00, sizeof(line_buf) );
			continue;
		}
		else
		{
			if( ( p_comment=strchr(line_buf,'#') ) != NULL )
			{
				*p_comment = 0;
				RTrim(line_buf);
			}
			if( ( ( p_left_bracket=strchr(line_buf,'[') ) != NULL ) &&
					( ( p_right_bracket=strchr(line_buf,']') ) != NULL ) &&
					( p_right_bracket > p_left_bracket)  )
			{
				break;
			}
			if( (p_assign=strchr(line_buf,'='))==NULL )
			{
				memset( line_buf, 0x00, sizeof(line_buf) );
				continue;
			}
			else
			{
				*p_assign = 0;
				RTrim(line_buf);
				LTrim(p_assign+1);
				if( strcmp(line_buf, key_name)!=0 )
				{
					memset( line_buf, 0x00, sizeof(line_buf) );
					continue;
				}
				else
				{
					snprintf( value, sizeof(value), p_assign+1 );
					ifindflag = 1;
				}
			}
		}
	}
	
	fclose( fp );

	if( ifindflag==1 )
	{
		snprintf( o_value, o_value_len, value );
		return( 0 );
	}
	return( -4 );
}

/*
 *功能:将第三方的查询参数，转化成BOSS调用tuxedo的输入形式
 *作者:王良 
 *日期:2008年4月18日
 *输入:
 *输出:
 */ 
int changeParaToTuxedoInput(struct str_PARA *pPara, int iQueryType,struct tagTuxedoCall *pTuxedoCall){
	int		iReturn = -1,i=0,ret=0;
	char 	*pchSplitData;
	char	*pPos; 
	int		iPos = 0;
	char	chItemValue[64+1];
	char	chI[32+1];
	char	chJ[32+1];
	
	for(i = 0; i < g_bossapp.count ; i++)
	{
		if(g_bossapp.case_code[i]==iQueryType)
		{
			pTuxedoCall->iInputNum=g_bossapp.input_num[i];
			pTuxedoCall->iOutputNum=g_bossapp.output_num[i];
			strcpy(pTuxedoCall->chServName,g_bossapp.server_name_english[i]);
			pchSplitData=g_bossapp.param_trans[i];
			ret=1;
			break;
		}
	}
	if(ret==0)
	{
		sprintf(loginfo,"changeParaToTuxedoInput error[%d]!\n",iQueryType);
		LogIt(loginfo,HEAD);
	}
	
	while(0 != strlen(pchSplitData))
	{
  	pPos = strchr(pchSplitData, ';');
    if (pPos != NULL)
    {
    	iPos = pPos - pchSplitData;
    	memset(chItemValue, 0, sizeof(chItemValue));
    	strncpy(chItemValue, pchSplitData, iPos);
    	pchSplitData = pPos + 1;
    	memset(chI, 0,sizeof(chI));
    	memset(chJ, 0,sizeof(chJ));
    
    	pPos = NULL;
    	iPos = 0;
    	pPos = strchr(chItemValue, ',');
    	iPos = pPos-chItemValue;
    	strncpy(chI,chItemValue,iPos);
    	strcpy(chJ,pPos+1);
			if (0==strncmp(chJ,"|",1))
				strcpy(pTuxedoCall->chInputValue[atoi(chI)],&chJ[1]);      
			else
				strcpy(pTuxedoCall->chInputValue[atoi(chI)],pPara->acParaName[atoi(chJ)]);      		
    } 
    else
    {
    	break;
    }
  }
}

int finderrcode(int case_code,char *src, char *dest)
{
	char tmp[32];
	int i=0,ret=0;
	memset(tmp,0x0,sizeof(tmp));
	sprintf(tmp,"%d%s",case_code,src);
	sprintf(loginfo,"tmp[%s]\n",tmp);
	LogIt(loginfo,HEAD);
	Trim(tmp);

	for(i = 0; i < g_Bosserrcode.count ; i++)
	{
		Trim(g_Bosserrcode.boss_errcode[i]);
		if(strcmp(g_Bosserrcode.boss_errcode[i],tmp)==0)
		{
			strcpy(dest,g_Bosserrcode.trans_errcode[i]);
			ret=1;
			break;
		}
	}
	if(ret==0)
		return -1;
	return 0;
}

/*
 *功能:调用tuxedo服务
 *作者:王良 
 *日期:2008年4月18日
 *输入:
 *输出:
 */
void doProcess(struct str_PARA *pStrucPara, int iQueryType, char* pchBuff)
{
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[10240];
	char	tchBuff[10*1024];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0,ret=0;
	char   chWorkName[50+1];
	int    flag = 0;
	char  chQueryType[6+1];
	char *casep1=NULL;
	char *casep2=NULL;
	char para1[5+1];
	int  para2=0;
	int  para3=0;
	char errstr[16+1];
	
	init(errstr);	
  memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));  
  memset(chWorkName, 0, sizeof(chWorkName));
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
 	sprintf(chQueryType,"[%d]",iQueryType);
	putenv(chWsnAddr);  	 	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);	
	memset(&m_tuxedoreturn, 0, sizeof(struct tagTuxedoReturn));
	callTuxedoServer(&m_tuxedocall,&m_tuxedoreturn);
			
	#ifdef _debug_	
		for(i=0;i<m_tuxedoreturn.iArraySum;i++)
		{
			for(j=0;j<m_tuxedoreturn.iOutputNum;j++)
			{
				sprintf(loginfo,"m_tuxedoreturn[%d][%d]=[%s]\n",i,j,m_tuxedoreturn.chOutputValue[i][j]);
	  			LogIt(loginfo,HEAD);
			}
		}	
	#endif	

	memset(chBuff, 0, sizeof(chBuff));
	sprintf(loginfo,"case :%d.ReturnCode=[%s],Returnmess[%s]\n",iQueryType,m_tuxedoreturn.return_code,m_tuxedoreturn.return_msg);
  LogIt(loginfo,HEAD);
  Trim(m_tuxedoreturn.return_code);
  if(strlen(m_tuxedoreturn.return_code)!=6)
  {
  	init(m_tuxedoreturn.return_code);
  	strncpy(m_tuxedoreturn.return_code,m_tuxedoreturn.chOutputValue[0][0],6);
  }
  Trim(m_tuxedoreturn.return_code);
  init(chBuff);
  init(tchBuff);
  
	sprintf(errstr,"%d%s",iQueryType,m_tuxedoreturn.return_code);
	Trim(errstr);
	sprintf(loginfo,"errstr[%s]\n",errstr);
	LogIt(loginfo,HEAD);
	for(i = 0; i < g_Bosserrcode.count ; i++)
	{
		Trim(g_Bosserrcode.boss_errcode[i]);
			
		if(strcmp(g_Bosserrcode.boss_errcode[i],errstr)==0)
		{
			strcpy(chBuff,g_Bosserrcode.trans_errcode[i]);
			ret=1;
			break;
		}
	}
	if(ret==0&&(strcmp(m_tuxedoreturn.return_code,"000000")==0))
	{
		sprintf(loginfo,"deal returnmsg error,notify\n");
	  	LogIt(loginfo,HEAD);
	}
	if(ret==0)
		sprintf(chBuff,"0\t0\t-1\n");
  else if(ret==1)
  {
  	Trim(chBuff);
  	
  	sprintf(loginfo,"test2 chBuff [%s]\n",chBuff);
  	LogIt(loginfo,HEAD);
  	
  	casep1=strstr(chBuff,"\t");
  	memcpy(para1,chBuff,casep1-chBuff);
  	Trim(para1);
  	para2=atoi(casep1+1);
  	if(para2>0)
  	{	
  		casep2=strstr(chBuff,"|");
  		para3=atoi(casep2+1);
  	}
  	if(strcmp(para1,"LINES")==0)
  	{
  		chBuff[casep2-chBuff]='\0';
			sprintf(chBuff,"%d%s",m_tuxedoreturn.iArraySum,casep1);
		
			sprintf(loginfo,"test3 chBuff[%s]\n",chBuff);
			LogIt(loginfo,HEAD);
		
			sprintf(loginfo,"test4 m_tuxedoreturn.iArraySum[%d]\n",m_tuxedoreturn.iArraySum);
			LogIt(loginfo,HEAD);
		
			for(i=0;i<m_tuxedoreturn.iArraySum;i++)
			{
				for(j=para3;j<para2+para3;j++)
				{
						/**
						sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[i][j]);
						sprintf(loginfo,"test5 chBuff[%s]\n",chBuff);
						**/ 
						memset(tchBuff,0,sizeof(tchBuff));
						sprintf(tchBuff,"%s",m_tuxedoreturn.chOutputValue[i][j]);
						sprintf(loginfo,"test5 tchBuff[%s]\n",tchBuff);
						LogIt(loginfo,HEAD);
						Trim(tchBuff);
						Trim(chBuff);
						printf("chBuff=[%s],tchBuff=[%s]\n",chBuff,tchBuff);
						strcat(chBuff,tchBuff);
				}
				printf("Test chBuff=[%s]\n",chBuff);
				sprintf(chBuff,"%s\n",chBuff);
				sprintf(loginfo,"test6 chBuff[%s]\n",chBuff); 
				LogIt(loginfo,HEAD);
			}
  	}
  	else if(para2>0)
  	{
  		chBuff[casep2-chBuff]='\0';
  		for(j=para3;j<para2+para3;j++)
			{sprintf(chBuff,"%s\t%s",chBuff,m_tuxedoreturn.chOutputValue[0][j]);
			
			sprintf(loginfo,"test7 chBuff[%s]\n",chBuff); 
			LogIt(loginfo,HEAD);}
			
			sprintf(chBuff,"%s\n",chBuff);
			
			sprintf(loginfo,"test8 chBuff[%s]\n",chBuff); 
			LogIt(loginfo,HEAD);
  	}
  	else
  		sprintf(chBuff,"%s\n",chBuff);
  }

	#ifdef _debug_
		sprintf(loginfo,"chBuff[%s]\n",chBuff);
	  LogIt(loginfo,HEAD);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		sprintf(loginfo,"call callTuxedoServer end...\n");
	  LogIt(loginfo,HEAD);
	#endif
}

int callTuxedoServer(struct tagTuxedoCall *pTuxedoCall,struct tagTuxedoReturn *pTuxedoReturn){
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int			iInputNum = 0;
	char		chTmp[32];		/*临时变量*/
	int			i = 0;
	long 		len = 0;
	int  		ret = 0;
	char		chServerName[64];
	char 		buffer[400];
	int 		ret_line = 0; 
	char 		temp_buf[100];
	int 		j=0;	
	char		chReturnCode[6+1];		/*返回代码*/
	char		chReturnMsg[128+1];		/*返回代码描述信息*/
	char		chSegment[1024];				/*返回值片段*/
	int			iHasRecord = 1;				/*返回字段标示*/
	
	#ifdef _debug_
		sprintf(loginfo,"pTuxedoCall->chServName=[%s]\n",pTuxedoCall->chServName);
	  LogIt(loginfo,HEAD);
	#endif		
	iInputNum = pTuxedoCall->iInputNum;
	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"Tpinit failed!\n");
    LogIt(loginfo,HEAD);
    tpterm();
	  return -1;
	}	

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"FBFR32 send_fb tpalloc failed\n");
    LogIt(loginfo,HEAD);
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */
	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			sprintf(loginfo,"pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
    	LogIt(loginfo,HEAD);
    #endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"FBFR32 receive_fb tpalloc failed\n");
    LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }       
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno));
    LogIt(loginfo,HEAD);   		
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    sprintf(loginfo,"buffer=[%s]\n",buffer);
    LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);

	strcpy(pTuxedoReturn->return_code,chReturnCode); 	
	strcpy(pTuxedoReturn->return_msg,chReturnMsg);
	pTuxedoReturn->iOutputNum = pTuxedoCall->iOutputNum; 	
	for(i=0; 1 == iHasRecord ; i++)
	{
		iHasRecord = 0;
		for(j=0; j<pTuxedoCall->iOutputNum; j++) 
		{
			memset(chSegment, 0, sizeof(chSegment));			
			Fgets32(receive_fb, GPARM32_0+j , i, chSegment);
			strcpy(pTuxedoReturn->chOutputValue[i][j],chSegment);
			if (0 != strlen(Trim(chSegment)))
			{				
				iHasRecord = 1;
			}			
		}
	}   
	pTuxedoReturn->iArraySum = i-1;
	tpfree((char *)send_fb);
  tpfree((char *)receive_fb);
  tpterm();	
	return ret_line;	
}

/*
 *功能:读取配制文件中,参数
 *作者:王良
 *日期:2008年3月19日
 */
int GetProfile(char* pchSection, char* pchSegName, char* pchValue,int iflag)
{
	char	chFileName[128];
	char	chBuff[1024];
	char	chSection[8];
	char	chSegName[60];
	int		iSection = 0;
	int		flag=0;
	FILE 	*fp;
	memset(chFileName, 0, sizeof(chFileName));
	memset(chSegName, 0, sizeof(chSegName));
	memset(chSection, 0, sizeof(chSection));
	if(iflag==1)
		sprintf(chFileName,"%s",bossappcfg);
	else if(iflag==2)
		sprintf(chFileName,"%s",errorcodecfg);
	strcpy(chSection,pchSection);
	strcpy(chSegName,pchSegName);
	
	fp = fopen(chFileName,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"Open bossinter.cfg error");
		LogIt(loginfo,HEAD);
		#ifdef _debug_
			sprintf(loginfo,"打开配制文件出错![%s]\n",chFileName);
    	LogIt(loginfo,HEAD);	
		#endif
		return -1;
	}
	while(!feof(fp))
	{
		memset(chBuff, 0, sizeof(chBuff));
		freadln(chBuff,fp);
		if (0 == strncmp(chBuff,chSection,6) && iSection == 0)
		{
			iSection = 1;
			continue;
		}
		else if (0 != strncmp(chBuff,chSection,6) && iSection == 0)
		{
			continue;
		}

		if (0 ==strncmp(chBuff,chSegName,strlen(chSegName)))
		{
			strcpy(pchValue, &chBuff[strlen(chSegName)+1]);
			flag=1;
			break;		
		} 

		if(0==strncmp(chBuff,"[",1) && iSection==1)
		{
			break;
		}	
	}
	fclose(fp);
	if(flag==0)
		return -1;
	return 0;
} 

void freadln(char *szBuf, FILE *fp)
{
	char szCh;
  if (fp!=NULL)
  {
  	while (!feof(fp))
  	{
    	szCh =  fgetc(fp);
      if (szCh == ' ')
      {
      	continue;
      }      
      if (szCh == 0x0D)
      {
      	continue;
      }          
      if (szCh == 0x0A)
      {
      	break;
      }   		
   		*szBuf++ = szCh;
    	}
  }
}

/*
 *功能:详单查询
 *作者:王良
 *日期:2008年3月20日
 *
 */

int doDetailQuery(struct str_PARA *pStrucPara, int iSocket, int pchQueryType,char *outbuff)
{
	char	chPhoneNo[11+1];
	char	chBeginDate[20+1];
	char	chEndDate[20+1];
	char	chServerName[20+1];
	FBFR32  *send_fb;
	FBFR32  *receive_fb;
	TPINIT	*tpinitbuf;
	int		input_par_num = 10;
	int		cd = 0;
	long	revent = 0;
	long 	rcv_len =0;
	char	buffer[128];
	char	return_code[6+1];
	char	tmpbuf[40];	
	int  	counter=0;
	int		ret_lines = 0;
	int 	iRecordSum = 0;
	int 	output_par_num =0;
	int		i, j, k, flag = 0;
	int		TuxedoRcvAllLines=0;
	int		TuxedoOutFields=0;
	int 	err_counter = 0;
	char	chRecord[1024*10000];
	/*char	chBuff[1024*31];*/
	char	chLoginNo[6+1];
	char	chQueryType[4+1];
	char   	chWorkName[50+1];
	int    	iiflag = 0;	
	char	chPwdWord[20+1];
	sprintf(chQueryType,"%d",pchQueryType);
	
	memset(chWorkName, 0, sizeof(chWorkName));
	init(chRecord);
	#ifdef _debug_
		sprintf(loginfo,"doDetailQuery begin...\n");
    LogIt(loginfo,HEAD);	
	#endif  
  memset(chLoginNo, 0, sizeof(chLoginNo));
  strncpy(chLoginNo,pStrucPara->acParaName[0],6);
  
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
	putenv(chWsnAddr);
		
	memset(chPhoneNo, 0, sizeof(chPhoneNo));
	memset(chBeginDate, 0, sizeof(chBeginDate));
	memset(chEndDate, 0, sizeof(chEndDate));
	memset(chServerName, 0, sizeof(chServerName));	
	memset(chPwdWord, 0, sizeof(chPwdWord));
	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
	strncpy(chBeginDate,pStrucPara->acParaName[5],20);
	strncpy(chEndDate,pStrucPara->acParaName[6],20);
	strncpy(chPwdWord,pStrucPara->acParaName[8],20);

	#ifdef _debug_
		sprintf(loginfo,"tpinit\nchBeginDate=[%s]\nchEndDate=[%s]\n",chBeginDate,chEndDate);
    LogIt(loginfo,HEAD);
	#endif	

	tpinitbuf=(TPINIT *)tpalloc("TPINIT", NULL, TPINITNEED(0));  
	tpinitbuf->flags = TPMULTICONTEXTS;   	
	if (tpinit((TPINIT *) tpinitbuf) == -1)
	{
		sprintf(loginfo,"DetailQuery tpinit error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	
	/*为输入变量分配FML缓冲区*/ 
	send_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(input_par_num*50));   
	if(send_fb==(FBFR32 *)NULL)
	{  	
		sprintf(loginfo,"DetailQuery sendbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();    
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;                                             
	}

	#ifdef _debug_
		sprintf(loginfo,"Fchg32 send_fb\nchLoginNo=[%s]\nchQueryType=[%s]\nchPhoneNo=[%s]\nchBeginDate=[%s]\nchEndDate=[%s]\n",
		chLoginNo,chQueryType,chPhoneNo,chBeginDate,chEndDate);
    LogIt(loginfo,HEAD);
	#endif	

	Fchg32(send_fb, GPARM32_0+0, 0, chLoginNo, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+1, 0, chQueryType, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+2, 0, chPhoneNo, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+3, 0, chBeginDate, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+4, 0, chEndDate, (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+5, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+6, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+7, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+8, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+9, 0, "0", (FLDLEN32)0);
	Fchg32(send_fb, GPARM32_0+10,0, chPwdWord, (FLDLEN32)0);
          
	/*为输出变量分配FML缓冲区*/
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32, NULL, (FLDLEN32)4096);
	if(receive_fb==(FBFR32 *)NULL)
	{ 
		sprintf(loginfo,"DetailQuery recvbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);  		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
	  tpterm();
	  sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		sprintf(loginfo,"Fchg32 tpconnect\n");
    LogIt(loginfo,HEAD);
	#endif

	strcpy(chServerName,"s1861UnitQry");
	if ( (cd =tpconnect(chServerName,(char *)NULL,0L,TPSENDONLY)) == -1 )
	{	
		sprintf(loginfo,"*** DetailQuery tpconnect(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
    return -1;
	}		
	#ifdef _debug_
		sprintf(loginfo,"cd=[%d]\ntpsend\n",cd);
    LogIt(loginfo,HEAD);	
	#endif
	
	if ( tpsend(cd, (char *)send_fb, 0L,TPRECVONLY, &revent ) == -1 )
	{	
		sprintf(loginfo,"DetailQuery tpsend error\n" );
    LogIt(loginfo,HEAD);	
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		sprintf(loginfo,"tprecv\n");
    LogIt(loginfo,HEAD);
	#endif
	Finit32(receive_fb, Fsizeof32(receive_fb));
	if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
	{
		sprintf(loginfo,"DetailQuery tprecv error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
	{   		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	Fgets32(receive_fb, GPARM32_0, 0, buffer);

	#ifdef _debug_
		sprintf(loginfo,"buffer=[%s]\n",buffer);
    LogIt(loginfo,HEAD);
	#endif
	sprintf(loginfo,"DetailQuery Tuxedo returncode =[%s]\n", buffer);
  LogIt(loginfo,HEAD);
	if(!strcmp(buffer,"186100"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t0\n");
		return 0;
	}
	else if(!strcmp(buffer,"186105"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-2\n");
		return 0;
	}
	else if(!strcmp(buffer,"000000"))
	{

		strncpy(return_code, buffer, sizeof(return_code)-1);
		if( 0 != atoi(buffer) )
		{
			tpfree((char *)tpinitbuf);
			tpfree((char *)send_fb);
			tpfree((char *)receive_fb);
			tpterm();
			sprintf(outbuff,"0\t0\t-1\n");
			return -13;
		}		
		#ifdef _debug_
			sprintf(loginfo,"while\n");
    	LogIt(loginfo,HEAD);
		#endif
	
		counter = 1;
		/*memset(chBuff, 0, sizeof(chBuff));*/
		memset(chRecord, 0, sizeof(chRecord));
		iRecordSum = 0;
				
		while( 1 )
		{
			#ifdef _debug_
				sprintf(loginfo,"while counter=[%d]\n",counter);
    		LogIt(loginfo,HEAD);
			#endif					
			memset(tmpbuf, 0, sizeof(tmpbuf));
			sprintf(tmpbuf, "%d", counter);		
			Fchg32(send_fb, GPARM32_0, 0, tmpbuf , (FLDLEN32)0);
	
			revent = 0;
			if ( tpsend(cd, (char *)send_fb, 0L, TPRECVONLY, &revent ) == -1 ) 
			{
				break;
			}					
			Finit32(receive_fb, Fsizeof32(receive_fb));
			if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
			{
				sprintf(loginfo,"DetailQuery tprecv error\n");
    		LogIt(loginfo,HEAD);
				tpfree((char *)tpinitbuf);
				tpfree((char *)send_fb);
				tpfree((char *)receive_fb);
				tpterm();
				sprintf(outbuff,"0\t0\t-1\n");
				return -1;			
			}
			
			if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
			{	
				break;
			}		
			memset(buffer, 0, sizeof(buffer));
	
			ret_lines = Foccur32(receive_fb, GPARM32_0);
			Fgets32(receive_fb, GPARM32_0, 0, buffer);
			output_par_num = atoi( buffer );
	
			
			#ifdef _debug_
				sprintf(loginfo,"while counter=[%d] ret_lines=[%d]\n",counter,ret_lines);
    		LogIt(loginfo,HEAD);
			#endif		
			counter++;			
			iRecordSum = iRecordSum+ret_lines;			
			flag = 0;
			for(i=0; i<ret_lines; i++)
			{			
				memset(buffer, 0, sizeof(buffer));
				if( Fgets32(receive_fb, GPARM32_0, i, buffer)>=0 )
				{				
					if ( atoi( buffer ) > 0 )
						TuxedoRcvAllLines++;
					else 
					{
						flag=1;
						break;
					}
				}
			
				for(k=0; k<output_par_num; k++)
				{
					TuxedoOutFields = output_par_num;
					memset(buffer, 0, sizeof(buffer));
					if ( Fgets32(receive_fb, GPARM32_1+k, i, buffer) < 0 )
					{
						err_counter++;					
						continue;
					}
					sprintf(chRecord,"%s%s\t",chRecord,buffer);
				}
				sprintf(chRecord,"%s\n",chRecord);
			} // for end		
			if (flag == 1)
				break;
		} // while( 1 ) end
		#ifdef _debug_
			sprintf(loginfo,"iRecordSum=[%d]\noutput_par_num=[%d]\n",iRecordSum,output_par_num);
    	LogIt(loginfo,HEAD);
		#endif		
		sprintf(outbuff,"%d\t%d\t1\t%s",iRecordSum,TuxedoOutFields,chRecord);
		#ifdef _debug_
			sprintf(loginfo,"chBuff=[%s]\n",outbuff);
    	LogIt(loginfo,HEAD);
		#endif	
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
	}
	else
	{
		sprintf(outbuff,"0\t0\t-1\n");
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
		return 0;
	}
	#ifdef _debug_
		sprintf(loginfo,"doDetailQuery end...\n");
    LogIt(loginfo,HEAD);
	#endif
	return 0;
}
int doSPQuery(struct str_PARA *pStrucPara,char *outbuff)
{
	char	chPhoneNo[11+1];	
	char	bizetype[2+1];
	char	chServerName[20+1];
	FBFR32  *send_fb;
	FBFR32  *receive_fb;
	TPINIT	*tpinitbuf;
	int		input_par_num = 10;
	int		cd = 0;
	long	revent = 0;
	long 	rcv_len =0;
	char	buffer[128];
	char	return_code[6+1];
	char	tmpbuf[40];	
	int  	counter=0;
	int		ret_lines = 0;
	int 	iRecordSum = 0;
	int 	output_par_num =0;
	int		i, j, k, flag = 0;
	int		TuxedoRcvAllLines=0;
	int		TuxedoOutFields=0;
	int 	err_counter = 0;
	char	chRecord[1024*30];
	/*char	chBuff[1024*31];*/
	char	begin_time[14+1];
	char  end_time[14+1];
	char    chWorkName[50+1];
	int     kkflag = 0;
	
	#ifdef _debug_
		sprintf(loginfo,"doSPQuery begin...\n");
    LogIt(loginfo,HEAD);	
	#endif	
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  memset(begin_time, 0, sizeof(begin_time));
  memset(end_time, 0, sizeof(end_time));
  memset(bizetype, 0, sizeof(bizetype));
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  Trim(chWorkName);
  if(strcmp(chWorkName,"QUERY_CUSTOM_ORDER")==0)
  {
  	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  	strncpy(bizetype,pStrucPara->acParaName[5],2);
  }
  else if(strcmp(chWorkName,"QUERY_CUSTOMHIS_ORDER")==0)
  {
  	strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  	strncpy(begin_time,pStrucPara->acParaName[5],8);
  	strncpy(end_time,pStrucPara->acParaName[6],8);
  }
	putenv(chWsnAddr);
		
	memset(chServerName, 0, sizeof(chServerName));
	
	#ifdef _debug_
		sprintf(loginfo,"tpinit\nbizetype=[%s]\n",bizetype);
    LogIt(loginfo,HEAD);
	#endif	

	tpinitbuf=(TPINIT *)tpalloc("TPINIT", NULL, TPINITNEED(0));  
	tpinitbuf->flags = TPMULTICONTEXTS;   	
	if (tpinit((TPINIT *) tpinitbuf) == -1)
	{
		sprintf(loginfo,"doSOQuery tpinit error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}
	
	/*为输入变量分配FML缓冲区*/ 
	send_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(input_par_num*50));   
	if(send_fb==(FBFR32 *)NULL)
	{  	
		sprintf(loginfo,"doSOQuery sendbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);
		tpfree((char *)tpinitbuf);
		tpterm(); 
		sprintf(outbuff,"0\t0\t-1\n");   
		return -1;                                             
	}
	if(strcmp(chWorkName,"QUERY_CUSTOM_ORDER")==0)
	{
		Fchg32(send_fb, GPARM32_0+0, 0, chPhoneNo, (FLDLEN32)0);
		Fchg32(send_fb, GPARM32_0+1, 0, bizetype, (FLDLEN32)0);
	}
	else if(strcmp(chWorkName,"QUERY_CUSTOMHIS_ORDER")==0)
	{
		Fchg32(send_fb, GPARM32_0+0, 0, chPhoneNo, (FLDLEN32)0);
		Fchg32(send_fb, GPARM32_0+1, 0, begin_time, (FLDLEN32)0);
		Fchg32(send_fb, GPARM32_0+2, 0, end_time, (FLDLEN32)0);
	}
	/*为输出变量分配FML缓冲区*/
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32, NULL, (FLDLEN32)4096);
	if(receive_fb==(FBFR32 *)NULL)
	{   
		sprintf(loginfo,"doSOQuery recvbuffer tpalloc error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
	  tpterm();
	  sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		sprintf(loginfo,"Fchg32 tpconnect\n");
    LogIt(loginfo,HEAD);	
	#endif
	
	if(strcmp(chWorkName,"QUERY_CUSTOM_ORDER")==0)
	{
		strcpy(chServerName,"sOrderQryDlg");
	}
	else if(strcmp(chWorkName,"QUERY_CUSTOMHIS_ORDER")==0)
	{
		strcpy(chServerName,"sOrderHisQryDlg");
	}
	
	if ( (cd =tpconnect(chServerName,(char *)NULL,0L,TPSENDONLY)) == -1 )
	{
		sprintf(loginfo,"*** doSPQuery tpconnect(%s) Error *** \nTperrno:  %d\nMessage:\t%s\n",chServerName,tperrno,tpstrerror(tperrno) );
    LogIt(loginfo,HEAD);	
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
    return -1;
	}		
	#ifdef _debug_
		sprintf(loginfo,"cd=[%d]\ntpsend\n",cd);
    LogIt(loginfo,HEAD);	
	#endif
	
	if ( tpsend(cd, (char *)send_fb, 0L,TPRECVONLY, &revent ) == -1 )
	{
		sprintf(loginfo,"doSOQuery tpsend error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	#ifdef _debug_
		sprintf(loginfo,"tprecv\n");
    LogIt(loginfo,HEAD);
	#endif
	Finit32(receive_fb, Fsizeof32(receive_fb));
	if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
	{
		sprintf(loginfo,"doSOQuery tprecv error\n" );
    LogIt(loginfo,HEAD);		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
	{   		
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return -1;
	}

	Fgets32(receive_fb, GPARM32_0, 0, buffer);

	#ifdef _debug_
		sprintf(loginfo,"buffer=[%s]\n",buffer);
    LogIt(loginfo,HEAD);
	#endif
	sprintf(loginfo,"doSOQuery Tuxedo returnvalue =[%s]\n",buffer );
  LogIt(loginfo,HEAD);
	if(!strcmp(buffer,"190000"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t0\n");
		return 0;
	}
	else if(!strcmp(buffer,"190002"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-2\n");
		return 0;
	}
	else if(!strcmp(buffer,"191001"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-2\n");
		return 0;
	}
	else if(!strcmp(buffer,"192003"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-3\n");
		return 0;
	}
	else if(!strcmp(buffer,"192005"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-4\n");
		return 0;
	}
	else if(!strcmp(buffer,"192006"))
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-5\n");
		return 0;
	}
	else if(!strcmp(buffer,"000000"))
	{
		strncpy(return_code, buffer, sizeof(return_code)-1);
		if( 0 != atoi(buffer) )
		{
			tpfree((char *)tpinitbuf);
			tpfree((char *)send_fb);
			tpfree((char *)receive_fb);
			tpterm();
			sprintf(outbuff,"0\t0\t-1\n");
			return -13;
		}	
	
		#ifdef _debug_
			sprintf(loginfo,"while\n");
    	LogIt(loginfo,HEAD);
    #endif
	
		counter = 1;
		/*memset(chBuff, 0, sizeof(chBuff));*/
		memset(chRecord, 0, sizeof(chRecord));
		iRecordSum = 0;
				
		while( 1 )
		{
			#ifdef _debug_
				sprintf(loginfo,"while counter=[%d]\n",counter);
    		LogIt(loginfo,HEAD);
			#endif					
			memset(tmpbuf, 0, sizeof(tmpbuf));
			sprintf(tmpbuf, "%d", counter);		
			Fchg32(send_fb, GPARM32_0, 0, tmpbuf , (FLDLEN32)0);	
			revent = 0;
			if ( tpsend(cd, (char *)send_fb, 0L, TPRECVONLY, &revent ) == -1 ) 
			{
					break;
			}		
			Finit32(receive_fb, Fsizeof32(receive_fb));
			if ( tprecv(cd, (char **)&receive_fb, &rcv_len, TPNOCHANGE, &revent) != -1 )
			{
				sprintf(loginfo,"doSOQuery tprecv error\n" );
    		LogIt(loginfo,HEAD);
				tpfree((char *)tpinitbuf);
				tpfree((char *)send_fb);
				tpfree((char *)receive_fb);
				tpterm();
				sprintf(outbuff,"0\t0\t-1\n");
				return -1;			
			}
	
			if ( (tperrno != TPEEVENT) || ((revent !=TPEV_SENDONLY) && (revent != TPEV_SVCSUCC)) )
			{	
				break;
			}

			memset(buffer, 0, sizeof(buffer));	
			ret_lines = Foccur32(receive_fb, GPARM32_0);
			Fgets32(receive_fb, GPARM32_0, 0, buffer);
			output_par_num = atoi( buffer );			
			#ifdef _debug_
				sprintf(loginfo,"while counter=[%d] ret_lines=[%d] output_par_num=[%d]\n",counter,ret_lines,output_par_num);
				LogIt(loginfo,HEAD);
			#endif	
	  	
			counter++;		
			iRecordSum = iRecordSum+ret_lines;		
			flag = 0;
			for(i=0; i<ret_lines; i++)
			{			
				memset(buffer, 0, sizeof(buffer));
				if( Fgets32(receive_fb, GPARM32_0, i, buffer)>=0 )
				{	
					if ( atoi( buffer ) > 0 )
						TuxedoRcvAllLines++;
					else 
					{
							flag=1;
							break;
					}
				}
				for(k=0; k<output_par_num; k++)
				{
					TuxedoOutFields = output_par_num;
					memset(buffer, 0, sizeof(buffer));
					if ( Fgets32(receive_fb, GPARM32_1+k, i, buffer) < 0 )
					{
						err_counter++;					
						continue;
					}
					sprintf(chRecord,"%s%s\t",chRecord,buffer);
				}
				sprintf(chRecord,"%s\n",chRecord);
			} // for end		
			if (flag == 1)
				break;
		} // while( 1 ) end
		sprintf(outbuff,"%d\t21\t1\t%s",iRecordSum,chRecord);
		#ifdef _debug_
			sprintf(loginfo,"iRecordSum=[%d]\noutput_par_num=[%d]\nchBuff=[%s]\n",
			iRecordSum,output_par_num,outbuff);
			LogIt(loginfo,HEAD);
		#endif		
		tpfree((char *)tpinitbuf);
		tpfree((char *)receive_fb);
		tpfree((char *)send_fb);	
		tpterm();
	}
	else 
	{
		tpfree((char *)tpinitbuf);
		tpfree((char *)send_fb);
		tpfree((char *)receive_fb);
		tpterm();
		sprintf(outbuff,"0\t0\t-1\n");
		return 0;
	}
	#ifdef _debug_
		sprintf(loginfo,"doSFQuery end...\n");
		LogIt(loginfo,HEAD);
	#endif
	return 0;
}

int getWorkToCase()
{
	FILE 	*fp;
	char	chBuff[128];
	char  *ptemp=NULL;
	char  *ptemp1=NULL;
	char  *ptemp2=NULL;
	char  *ptemp3=NULL;
	char  *ptemp4=NULL;
	char  *ptemp5=NULL;

	memset(chBuff, 0, sizeof(chBuff));
	fp = fopen(worktocasecfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!worktocasecfg[%s]\n",worktocasecfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_worktocase.count=0;
	while(fgets(chBuff,128,fp))
	{
		if (0==strncmp(chBuff,"#",1))
			continue;
		Trim(chBuff);
		if(0 == strlen(chBuff))
			continue;
		ptemp=NULL;
		ptemp1=NULL;
		ptemp2=NULL;
		ptemp3=NULL;
		ptemp4=NULL;
		ptemp5=NULL;
		ptemp=strtok(chBuff,"|");	
		ptemp1=strtok(NULL,"|");	
		ptemp2=strtok(NULL,"|");
		ptemp3=strtok(NULL,"|");	
		strcpy(g_worktocase.work[(g_worktocase.count)++],ptemp);
		g_worktocase.case_code[(g_worktocase.count)-1]=atoi(ptemp1);
		g_worktocase.call_flag[(g_worktocase.count)-1]=atoi(ptemp2);
		ptemp4=strtok(ptemp3,":");
		ptemp5=strtok(NULL,":");
		g_worktocase.para[(g_worktocase.count)-1]=atoi(ptemp4);	
		strcpy(g_worktocase.para_value[(g_worktocase.count)-1],ptemp5);
		memset(chBuff, 0, sizeof(chBuff));
	}
	fclose(fp);
	sprintf(loginfo,"g_worktocase.count[%d]\n",g_worktocase.count);
	LogIt(loginfo,HEAD);
	return 0;
}

int getBosserrcode()
{
	FILE 	*fp;
	char	chBuff[128];
	char KeyValue[50];
	char  *ptemp=NULL;
	char value1[100],value2[100];
	int i=0,ret=0;
	
	memset(chBuff, 0, sizeof(chBuff));
	memset(KeyValue, 0, sizeof(KeyValue));
	memset(value1, 0, sizeof(value1));
	memset(value2, 0, sizeof(value2));
	memset(chBuff, 0, sizeof(chBuff));
	fp = fopen(errorcodecfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!errorcodecfg[%s]\n",errorcodecfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_Bosserrcode.count=0;
	for(;fgets(chBuff,sizeof(chBuff),fp);)
  {
  	memset(value1,0,sizeof(value1));
		memset(value2,0,sizeof(value2));
  	if(chBuff[0]=='#')
    	continue;
    if(strstr(chBuff,"=")==NULL)
     	continue;
    strcpy(KeyValue,"%[^=]=%[^=]");
    ret=sscanf(chBuff,KeyValue,value1,value2);
    if(ret!=2)
    {	
    	printf("chBuff=[%s],KeyValue=[%s]\n",chBuff,KeyValue);
    	fclose(fp);
    	return -3;
    }
    ALLTrim(value1);
    ALLTrim(value2);
    	
 	strcpy(g_Bosserrcode.boss_errcode[(g_Bosserrcode.count)++],value1);
	strcpy(g_Bosserrcode.trans_errcode[(g_Bosserrcode.count)-1],value2);

	memset(chBuff, 0, sizeof(chBuff));
	
  }
	fclose(fp);
	sprintf(loginfo,"getBosserrcode.count[%d]\n",g_Bosserrcode.count);
	LogIt(loginfo,HEAD);
	
	return 0;
}

int getBossapp()
{
	FILE 	*fp;
	char	chBuff[256];
	char  *ptemp=NULL;
	char  *ptemp1=NULL;
	char  *ptemp2=NULL;
	char  ptemp_name[32];
	int		length=5,i=0;

	memset(chBuff, 0, sizeof(chBuff));
	memset(ptemp_name, 0, sizeof(ptemp_name));
	fp = fopen(bossappcfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!bossappcfg[%s]\n",bossappcfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_bossapp.count=0;
	while(fgets(chBuff,256,fp))
	{
		i++;
		ptemp=NULL;
		ptemp1=NULL;
		ptemp2=NULL;
		memset(ptemp_name, 0, sizeof(ptemp_name));
		if (0==strncmp(chBuff,"#",1))
			continue;
		Trim(chBuff);
		if(0 == strlen(chBuff))
			continue;
		if((ptemp=strstr(chBuff,"["))!=NULL&&(ptemp1=strstr(chBuff,"]"))!=NULL)
		{
			g_bossapp.case_code[(g_bossapp.count)++]=atoi(ptemp+1);
			if(length!=5)
			{
				fclose(fp);
				sprintf(loginfo,"1配制文件有问题,请核查![%d]行chBuff[%s]\n",i,chBuff);
				LogIt(loginfo,HEAD);
				return -1;
			}
			length=1;
			memset(chBuff, 0, sizeof(chBuff));
			continue;
		}
		ptemp2=strstr(chBuff,"=");
		if(ptemp2==NULL)
		{
			fclose(fp);
			sprintf(loginfo,"2配制文件有问题,请核查![%d]行,chBuff[%s]\n",i,chBuff);
			LogIt(loginfo,HEAD);
			return -1;
		}
		else
		{
			memcpy(ptemp_name,chBuff,ptemp2-chBuff);
			if(strcmp(ptemp_name,"SERVER_NAME_ENGLISH")==0)
				strcpy(g_bossapp.server_name_english[(g_bossapp.count)-1],ptemp2+1);
			else if(strcmp(ptemp_name,"INPUT_NUM")==0)
				g_bossapp.input_num[(g_bossapp.count)-1]=atoi(ptemp2+1);
			else if(strcmp(ptemp_name,"OUTPUT_NUM")==0)
				g_bossapp.output_num[(g_bossapp.count)-1]=atoi(ptemp2+1);
			else if(strcmp(ptemp_name,"PARAM_TRANS")==0)
				strcpy(g_bossapp.param_trans[(g_bossapp.count)-1],ptemp2+1);
			else
			{
				fclose(fp);
				sprintf(loginfo,"3配制文件有问题,请核查![%d]行,chBuff[%s]\n",i,chBuff);
				LogIt(loginfo,HEAD);
				return -1;
			}
			length++;
		}
		memset(chBuff, 0, sizeof(chBuff));
	}
	fclose(fp);
	sprintf(loginfo,"g_bossapp.count[%d]\n",g_bossapp.count);
	LogIt(loginfo,HEAD);
	return 0;
}


int GetPacketHead(char * recbuff,char *head)
{
	int i = 0;
	if(recbuff == NULL || head == NULL)
		return -1;
	for(i = 0; i< PACKETHEAD; i ++)
	  head[i] = recbuff[i];
  head[PACKETHEAD] = '\0';
	return 0;
}  

int GenarateSendbuffer(char * head,char * body,char * sendbuff)
{
     char TransDate[14+1];
     char sendbuffLen[10];
     char temp[11];
     int   nlen = 0;
     int	sendbufflen=0;
     int   i =0;
     int   lentemp = 0;
     struct tm *ptm;
		 time_t now;
     if(head == NULL || body == NULL || sendbuff == NULL)
     {
     		return -1;
     }
     time(&now);
		 ptm = localtime(&now);
		 sprintf(TransDate, "%d%02d%02d%02d%02d%02d", 1900 + ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday, \
		 ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	 
     nlen = strlen(head) + strlen(body);
     sprintf(temp,"%d",nlen);
     lentemp = strlen(temp);
     for(i = 0; i < 10-lentemp ; i++)
     {
           sendbuffLen[i] = '0';  	
     }
     for(i = 10-lentemp; i<10;i++)
     {
     	    sendbuffLen[i] = temp[i-10+lentemp];
     }
     for(i = 4;i<14;i++)
     {
          head[i] = sendbuffLen[i-4];
     }
	 
     head[42] = '1';
     for(i = 75 ; i < 89;i++)
     {
          head[i] = TransDate[i-75];
     }
     sprintf(sendbuff,"%s%s",head,body);
     sendbufflen=strlen(sendbuff);
     sprintf(loginfo,"strlen(sendbuff)=%d\n",sendbufflen);
		 LogIt(loginfo,HEAD);
     
     return 0;
}

void dototalfee(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff)
{
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[40960];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0;
	char    chWorkName[50+1];
	int     kkflag = 0;
	#ifdef _debug_
		sprintf(loginfo,"call callTuxedoServer begin...\n");
	  LogIt(loginfo,HEAD);	
	#endif	
  memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
	putenv(chWsnAddr);
  	
  	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);
	memset(&m_tuxedoreturn, 0, sizeof(m_tuxedoreturn));
	memset(chBuff, 0, sizeof(chBuff));
	callTuxedoTotalfee(&m_tuxedocall,chBuff);
	#ifdef _debug_
		sprintf(loginfo,"chBuff[%s]\n",chBuff);
	  LogIt(loginfo,HEAD);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		sprintf(loginfo,"call callTuxedoServer end...\n");
	  LogIt(loginfo,HEAD);
	#endif
}

void doInvoicePrint(struct str_PARA *pStrucPara,int iQueryType,char* pchBuff)
{
	struct 	tagTuxedoCall m_tuxedocall;
	struct 	tagTuxedoReturn m_tuxedoreturn;
	char 	chBuff[40960];
	char	chTmpReturn[1024];
	char	chPhoneNo[11+1];
	int 	i = 0;
	int 	j = 0;
	char    chWorkName[50+1];
	int     kkflag = 0;
	#ifdef _debug_
		sprintf(loginfo,"call callTuxedoServer begin...\n");
	  LogIt(loginfo,HEAD);	
	#endif	
  memset(&m_tuxedocall, 0, sizeof(struct tagTuxedoCall));
  memset(chWorkName, 0, sizeof(chWorkName));
  memset(chPhoneNo, 0, sizeof(chPhoneNo));
  strncpy(chWorkName,pStrucPara->acParaName[3],50);
  strncpy(chPhoneNo,pStrucPara->acParaName[4],11);
  Trim(chWorkName);
  Trim(chPhoneNo);
  sprintf(loginfo,"chWorkName=[%s],chPhoneNo=[%s]\n",chWorkName,chPhoneNo);
  LogIt(loginfo,HEAD);
	putenv(chWsnAddr);
  	
  	
	changeParaToTuxedoInput(pStrucPara,iQueryType,&m_tuxedocall);
	
	memset(&m_tuxedoreturn, 0, sizeof(m_tuxedoreturn));
	/*callTuxedoServer(&m_tuxedocall,&m_tuxedoreturn);*/
	memset(chBuff, 0, sizeof(chBuff));
	callTuxedoInvoicePrint(&m_tuxedocall,iQueryType,chBuff);
	#ifdef _debug_
		sprintf(loginfo,"chBuff[%s]\n",chBuff);
	  LogIt(loginfo,HEAD);
	#endif
	strcpy(pchBuff,chBuff);
	#ifdef _debug_
		sprintf(loginfo,"call callTuxedoServer end...\n");
	  LogIt(loginfo,HEAD);
	#endif
}
int callTuxedoInvoicePrint(struct tagTuxedoCall *pTuxedoCall,int type,char* pTuxedoReturn)
{
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int		iInputNum = 0;
	char	chTmp[32];		/*临时变量*/
	char 	temp[40960];
	int		i = 0,m=0;
	long 	len = 0;
	int  	ret = 0;
	char	chServerName[64];
	char 	buffer[400];
	int 	ret_line = 0; 
	char 	temp_buf[100];
	int 	j=0,n;
	int     iParaNumber = 0;
	char    cParaNumber[128];
	char    retCode[20];
	
	char	chReturnCode[6+1];		/*返回代码*/
	char	chReturnMsg[128+1];		/*返回代码描述信息*/
	char	chSegment[1024];				/*返回值片段*/
	int		iHasRecord = 1;				/*返回字段标示*/
	
	memset(cParaNumber,0,sizeof(cParaNumber));
	memset(retCode,0,sizeof(retCode));
	sprintf(loginfo,"chServName=[%s]\niInputNum=[%d]\niOutputNum=[%d]\nchInputValue0=[%s]\nchInputValue1=[%s]\n",
	pTuxedoCall->chServName,pTuxedoCall->iInputNum,pTuxedoCall->iOutputNum,pTuxedoCall->chInputValue[0],pTuxedoCall->chInputValue[1]);
	LogIt(loginfo,HEAD);	
	
	iInputNum = pTuxedoCall->iInputNum;

	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"doInvoicePrint tpinit error\n" );
	  LogIt(loginfo,HEAD);
    tpterm();
	  return -1;
	}

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"doInvoicePrint sendbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			sprintf(loginfo,"pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
	  	LogIt(loginfo,HEAD);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"doInvoicePrint recvbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }
    
   
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
	#ifdef _debug_
		sprintf(loginfo,"chServerName=[%s]\n",chServerName );
	  LogIt(loginfo,HEAD);
	#endif
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** doInvoicePrint tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
	  LogIt(loginfo,HEAD);
		
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    sprintf(loginfo,"buffer[%s]\n",buffer);
	  LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);
  n = pTuxedoCall->iOutputNum; 

	#ifdef _debug_
		sprintf(loginfo,"SVC_ERR_NO32=[%s]\nSVC_ERR_MSG32[%s]\n",chReturnCode,chReturnMsg);
		LogIt(loginfo,HEAD);
	#endif	
	Fget32(receive_fb,GPARM32_0,0,retCode,NULL);
	#ifdef _debug_
		sprintf(loginfo,"retCode = [%s]\n",retCode );
		LogIt(loginfo,HEAD);
	#endif
	sprintf(loginfo,"doInvoicePrint Tuxedo returnvalue=[%s]\n",retCode );
	LogIt(loginfo,HEAD);
	if(type==1034)
	{
		if(!strcmp(retCode,"186100"))
		{
			sprintf(pTuxedoReturn,"0\t0\t0\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		else if(!strcmp(retCode,"000000"))
		{
			Fget32(receive_fb,GPARM32_0+15,0,cParaNumber,NULL);
			iParaNumber = atoi(cParaNumber);
			#ifdef _debug_
				sprintf(loginfo,"iParaNumber = [%d]\n",iParaNumber);
	  		LogIt(loginfo,HEAD);
			#endif
			sprintf(pTuxedoReturn,"1\t17\t1");
			for(i = 1 ; i < 15 ;i++)
			{
				Fget32(receive_fb,GPARM32_0+i,0,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
			}
			sprintf(loginfo,"pTuxedoReturn=[%s]\n",pTuxedoReturn);
			LogIt(loginfo,HEAD);
			sprintf(pTuxedoReturn,"%s\t%d\t",pTuxedoReturn,iParaNumber);
		
			for(i=0; i<iParaNumber; i++)
			{
				Fget32(receive_fb,GPARM32_0+16,i,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
				Fget32(receive_fb,GPARM32_0+17,i,chSegment,NULL);
				sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
				sprintf(pTuxedoReturn,"%s\t",pTuxedoReturn);
			}	   
			sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);
			tpfree((char *)send_fb);
	  	tpfree((char *)receive_fb);
	  	tpterm();		
			return 0;
		}	
		else
		{
			sprintf(pTuxedoReturn,"0\t0\t-1\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
	}
	else if(type==1041||type==1042)
	{
		if(!strcmp(retCode,"190701")&&type==1041)
		{
			sprintf(pTuxedoReturn,"0\t0\t-2\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		if(!strcmp(retCode,"190702")&&type==1042)
		{
			sprintf(pTuxedoReturn,"0\t0\t-2\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
		else if(!strcmp(retCode,"000000"))
		{
			for(i=0; 1 == iHasRecord ; i++)
			{
				iHasRecord = 0;
				for(j=6; j<n-1; j++) 
				{
					memset(chSegment, 0, sizeof(chSegment));			
					Fgets32(receive_fb, GPARM32_0+j , i, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{
						sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);
						iHasRecord = 1;
					}			
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);
			}
			#ifdef _debug_
				sprintf(loginfo,"pTuxedoReturn = [%s]\n",pTuxedoReturn);
	  		LogIt(loginfo,HEAD);
			#endif
			m=n-6-1;
			sprintf(temp,"%d\t%d\t1%s\n",i-1,m,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
			#ifdef _debug_
				sprintf(loginfo,"pTuxedoReturn = [%s]\n",pTuxedoReturn);
	  		LogIt(loginfo,HEAD);
			#endif   
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();	
			return 0;	
		}	
		else
		{
			sprintf(pTuxedoReturn,"0\t0\t-1\n");
			tpfree((char *)send_fb);
  		tpfree((char *)receive_fb);
  		tpterm();
			return 0;
		}
	}
	return 1;
}

int callTuxedoTotalfee(struct tagTuxedoCall *pTuxedoCall,char* pTuxedoReturn)
{
	static 	FBFR32  *send_fb;
	static 	FBFR32  *receive_fb;
	int		iInputNum = 0;
	char	chTmp[32];		/*临时变量*/
	int		i = 0,m=0;
	long 	len = 0;
	int  	ret = 0;
	char	chServerName[64];
	char 	buffer[400];
	char 	temp[40960];
	int 	ret_line = 0; 
	char 	temp_buf[100];
	int 	j=0,n=0;
	int     iParaNumber = 0;
	char    cParaNumber[128];
	char    retCode[20];
	
	char	chReturnCode[6+1];		/*返回代码*/
	char	chReturnMsg[128+1];		/*返回代码描述信息*/
	char	chSegment[1024];				/*返回值片段*/
	int		iHasRecord = 1;				/*返回字段标示*/
	
	memset(cParaNumber,0,sizeof(cParaNumber));
	memset(retCode,0,sizeof(retCode));	
	#ifdef _debug_
		sprintf(loginfo,"pTuxedoCall-> chServName=[%s]\npTuxedoCall-> iInputNum=[%d]\npTuxedoCall-> iOutputNum=[%d]\npTuxedoCall-> chInputValue0=[%s]\n"
		"pTuxedoCall-> chInputValue1=[%s]\n",pTuxedoCall->chServName,pTuxedoCall->iInputNum,pTuxedoCall->iOutputNum,pTuxedoCall->chInputValue[0],
		pTuxedoCall->chInputValue[1]);
	  LogIt(loginfo,HEAD);	
	#endif	
	
	iInputNum = pTuxedoCall->iInputNum;


	if (tpinit((TPINIT *) NULL) == -1) 
	{
		sprintf(loginfo,"doInvoicePrint tpinit error\n" );
	  LogIt(loginfo,HEAD);
    tpterm();
	  return -1;
	}
	

	send_fb=(FBFR32*)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(iInputNum*50));
  if(send_fb==(FBFR32 *)NULL) 
  {
  	sprintf(loginfo,"doInvoicePrint sendbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
    tpterm();
    return -2;
 	}

	/*设置输入输出参数个数 */
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iInputNum);
  Fchg32(send_fb, SEND_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);
	memset(chTmp, 0, sizeof(chTmp));
  sprintf(chTmp,"%d",pTuxedoCall->iOutputNum);
  Fchg32(send_fb, RECP_PARMS_NUM32, 0, chTmp, (FLDLEN32)0);

	/* 输入参数的具体值 */

	for (i=0; i<pTuxedoCall->iInputNum; i++) 
	{
		#ifdef _debug_
			sprintf(loginfo,"pTuxedoCall->chInputValue[%d]=[%s]\n",i,pTuxedoCall->chInputValue[i]);
	  	LogIt(loginfo,HEAD);
		#endif
		Fchg32(send_fb, GPARM32_0+i, 0, pTuxedoCall->chInputValue[i], (FLDLEN32)0);
	}

	/* 为输出变量分配FML缓冲区 */
	receive_fb=(FBFR32 *)tpalloc(FMLTYPE32,NULL,(FLDLEN32)(10*pTuxedoCall->iOutputNum*50));
	if(receive_fb==(FBFR32 *)NULL) 
	{
		sprintf(loginfo,"doInvoicePrint recvbuffer tpalloc error\n" );
	  LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpterm();
    return -3;
  }
    
   
	len=Fsizeof32(receive_fb);

	/*调用TUXEDO服务*/
	memset(chServerName, 0, sizeof(chServerName));
	strcpy(chServerName,pTuxedoCall->chServName);
	#ifdef _debug_
		sprintf(loginfo,"chServerName=[%s]\n",chServerName);
	  LogIt(loginfo,HEAD);
	#endif
  ret=tpcall(chServerName, (char *)send_fb, 0L, (char **)&receive_fb, &len, TPNOCHANGE);
  if(ret == -1) 
  {
  	sprintf(loginfo,"*** doInvoicePrint tpcall(%s) Error *** \nTperrno:  %d\nMessage:\t%s",chServerName,tperrno,tpstrerror(tperrno) );
	  LogIt(loginfo,HEAD);
		
		Fget32( receive_fb, SVC_ERR_NO32, 0, temp_buf, 0 );
    sprintf(buffer,"***Tuxedo Service \"%s\" Error***\nErroNo:\t%s",chServerName,temp_buf);
    Fget32( receive_fb, SVC_ERR_MSG32, 0, temp_buf, 0);
    strcat(buffer,"\nErrorMsg:\t");
    strcat(buffer,temp_buf);
    sprintf(loginfo,"buffer=[%s]\n",buffer);
	  LogIt(loginfo,HEAD);
    tpfree((char *)send_fb);
    tpfree((char *)receive_fb);
    tpterm();
    return -4;
	}
	memset(chReturnCode,0,sizeof(chReturnCode));
	memset(chReturnMsg,0,sizeof(chReturnMsg));
	Fget32(receive_fb,SVC_ERR_NO32,0,chReturnCode,NULL);
  Fget32(receive_fb,SVC_ERR_MSG32,0,chReturnMsg,NULL);

	#ifdef _debug_
		sprintf(loginfo,"++SVC_ERR_NO32=[%s]\n++SVC_ERR_MSG32=[%s]\n",chReturnCode,chReturnMsg);
		LogIt(loginfo,HEAD);
	#endif	
	Fget32(receive_fb,GPARM32_0,0,retCode,NULL);
	sprintf(loginfo,"dototalfee Tuxedo returnvalue=[%s]\n",retCode );
	LogIt(loginfo,HEAD);
	if(!strcmp(retCode,"186100"))
	{
		sprintf(pTuxedoReturn,"0\t0\t0\n");
		tpfree((char *)send_fb);
  	tpfree((char *)receive_fb);
  	tpterm();
		return 0;
	}
	else if(!strcmp(retCode,"000000"))
	{
		for(i=0; 1 == iHasRecord ; i++)
		{
			iHasRecord = 0;
			for(j=1; j<5; j++) 
			{
				memset(chSegment, 0, sizeof(chSegment));			
				Fgets32(receive_fb, GPARM32_0+j, i, chSegment);		
				if (0 != strlen(Trim(chSegment)))
				{			
					sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
					iHasRecord = 1;
				}
			}
			if(iHasRecord==0) break;
			if(i==0&&iHasRecord==1)
			{
				for(m=0;m<56;m++)
				{
					memset(chSegment, 0, sizeof(chSegment));
					Fgets32(receive_fb, GPARM32_5 , m, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{
						sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
						iHasRecord = 1;
					}
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);	
			}
			else if(i==1&&iHasRecord==1)
			{
				for(m=56;m<112;m++)
				{
					memset(chSegment, 0, sizeof(chSegment));
					Fgets32(receive_fb, GPARM32_5 , m, chSegment);
					if (0 != strlen(Trim(chSegment)))
					{			
							sprintf(pTuxedoReturn,"%s\t%s",pTuxedoReturn,chSegment);				
							iHasRecord = 1;
					}
				}
				sprintf(pTuxedoReturn,"%s\n",pTuxedoReturn);	
			}		
		}
		n=i-1;
		if(n==0)
		{
			memset(chSegment, 0, sizeof(chSegment));
			sprintf(chSegment,"1\t60\t1");
			sprintf(temp,"%s%s",chSegment,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
		}
		else if(n==1)
		{
			memset(chSegment, 0, sizeof(chSegment));
			sprintf(chSegment,"2\t120\t1");
			sprintf(temp,"%s%s",chSegment,pTuxedoReturn);
			strcpy(pTuxedoReturn,temp);
		}   	   	
		tpfree((char *)send_fb);
	  tpfree((char *)receive_fb);
	  tpterm();		
		return 0;
	}	
	else
	{
		sprintf(pTuxedoReturn,"0\t0\t-1\n");
		tpfree((char *)send_fb);
  	tpfree((char *)receive_fb);
  	tpterm();
		return 0;
	}

}

int getWorkAddressHead()
{
	FILE 	*fp;
	char	chBuff[128];
	char KeyValue[50];
	char  *ptemp=NULL;
	char value1[100],value2[100],value3[100];
	int i=0,ret=0;
	
	memset(KeyValue, 0, sizeof(KeyValue));
	memset(value1, 0, sizeof(value1));
	memset(value2, 0, sizeof(value2));
	memset(value3, 0, sizeof(value3));
	memset(chBuff, 0, sizeof(chBuff));
	fp = fopen(workaddresscfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!workaddresscfg[%s]\n",workaddresscfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_WorkAddress.count=0;
	for(;fgets(chBuff,sizeof(chBuff),fp);)
  {
  	memset(value1,0,sizeof(value1));
		memset(value2,0,sizeof(value2));
		memset(value3, 0, sizeof(value3));
  	if(chBuff[0]=='#')
  	{
  		memset(chBuff, 0, sizeof(chBuff));
    	continue;
    }
    if(strstr(chBuff,"|")==NULL)
    {
    	memset(chBuff, 0, sizeof(chBuff)); 
    	continue;
    }
    strcpy(KeyValue,"%[^|]|%[^|]|%[^|]");
    ret=sscanf(chBuff,KeyValue,value1,value2,value3);
    if(ret!=3)
    {
    	printf("chBuff=[%s],KeyValue=[%s]\n",chBuff,KeyValue);
    	fclose(fp);
    	return -3;
    }
    ALLTrim(value1);
    ALLTrim(value2);
    ALLTrim(value3);
	 	strcpy(g_WorkAddress.work[(g_WorkAddress.count)++],value1);
		strcpy(g_WorkAddress.wsn_addr[(g_WorkAddress.count)-1],value3);
		memset(chBuff, 0, sizeof(chBuff));
  }
	fclose(fp);
	sprintf(loginfo,"g_WorkAddress.count[%d]\n",g_WorkAddress.count);
	LogIt(loginfo,HEAD);
	return 0;
}

int getPhoneHead()
{
	FILE 	*fp=NULL;
	char	chBuff[32];
	char  *ptemp=NULL;
	
	memset(chBuff, 0, sizeof(chBuff));	
	fp = fopen(phoneheadcfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!phoneheadcfg[%s]\n",phoneheadcfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_phonehead.count=0;
	while(fgets(chBuff,32,fp))
	{
		Trim(chBuff);
		if (0==strncmp(chBuff,"#",1))
			continue;
		if(0 == strlen(chBuff))
			continue;
		if((ptemp=strstr(chBuff,"|"))==NULL)
			continue;	
		memcpy(g_phonehead.phone_head[(g_phonehead.count)++],chBuff,ptemp-chBuff);
		strcpy(g_phonehead.wsn_addr[(g_phonehead.count)-1],ptemp+1);
		memset(chBuff, 0, sizeof(chBuff));
		ptemp=NULL;
	}
	fclose(fp);
	sprintf(loginfo,"g_phonehead.count[%d]\n",g_phonehead.count);
	LogIt(loginfo,HEAD);
}

int getBossorCrm()
{
	FILE 	*fp=NULL;
	char	chBuff[64];
	char  *ptemp=NULL;
	
	memset(chBuff, 0, sizeof(chBuff));	
	fp = fopen(bossorcrmcfg,"r");
	if (NULL==fp)
	{
		sprintf(loginfo,"打开配制文件出错!bossorcrmcfg[%s]\n",bossorcrmcfg);
		LogIt(loginfo,HEAD);
		return -1;
	}
	g_BossorCrm.count=0;
	while(fgets(chBuff,64,fp))
	{
		Trim(chBuff);
		if (0==strncmp(chBuff,"#",1))
			continue;
		if(0 == strlen(chBuff))
			continue;
		if((ptemp=strstr(chBuff,"|"))==NULL)
			continue;	
		memcpy(g_BossorCrm.belong_code[(g_BossorCrm.count)++],chBuff,ptemp-chBuff);
		strcpy(g_BossorCrm.wsn_addr[(g_BossorCrm.count)-1],ptemp+1);
		memset(chBuff, 0, sizeof(chBuff));
		ptemp=NULL;
	}
	fclose(fp);
	sprintf(loginfo,"g_BossorCrm.count[%d]\n",g_BossorCrm.count);
	LogIt(loginfo,HEAD);
}

int getPhoneAddr(char *pchPhoneNo, char* pchWsnAddr)
{
	char chPhoneHead[7+1];
	int	iLow = 0;
	int iHigh = 0;
	int iMid = 0;
	int iResult = -1;
	init(chPhoneHead);
	
	memcpy(chPhoneHead,pchPhoneNo,7);
	Trim(chPhoneHead);
	iHigh = g_phonehead.count; 
	while(iLow<=iHigh)
	{
  	iMid=(iLow+iHigh)/2;
  	Trim(g_phonehead.phone_head[iMid]);
    if (0 == strcmp(g_phonehead.phone_head[iMid],chPhoneHead))
    {
    	iResult = iMid;
    	break;
    }
    if(strcmp(g_phonehead.phone_head[iMid],chPhoneHead)>0)
    	iHigh=iMid-1;
    else
    	iLow=iMid+1; 
  }  
  if(iResult==-1)
  	return -1;
  strcpy(pchWsnAddr,g_phonehead.wsn_addr[iResult]);
	return 0;
}
int getPhoneWsnAddr(char *pchPhoneNo, int belongcode,char* pchWsnAddr)
{
	char phoneHead[7+1];
	char address[10];
	char address1[15];
	char tempWsnAddr[50];
	int i=0,ret=0;;
	init(address);
	init(address1);
	init(phoneHead);
	init(tempWsnAddr);
	memcpy(phoneHead,pchPhoneNo,7);
	Trim(address);
	if(getPhoneAddr(phoneHead,address)<0)
	{
		return -1;
	}
	sprintf(address1,"%s%d",address,belongcode);
	Trim(address1);
	for(i = 0; i < g_BossorCrm.count ; i++)
	{
		Trim(g_BossorCrm.belong_code[i]);  		
 						
		if(strcmp(g_BossorCrm.belong_code[i],address1)==0)
		{
			sprintf(tempWsnAddr,"%s",g_BossorCrm.wsn_addr[i]);
			/*LogIt(tempWsnAddr,HEAD);*/
			ret=1;
			break;
		}
	}
	if(ret==0)
		return -1;
	sprintf(pchWsnAddr,"WSNADDR=%s",tempWsnAddr);
	return 0;
}

int getWorkWsnAddr(char *pchWork, char* pchWsnAddr)
{
	char pwork[50];
	char address[10];
	char address1[15];
	char tempWsnAddr[50];
	int  i=0,ret=0;
	memset(address,0,sizeof(address));
	memset(address1,0,sizeof(address1));
	memset(tempWsnAddr,0,sizeof(tempWsnAddr));
	memset(pwork,0,sizeof(pwork));
	sprintf(pwork,"%s",pchWork);
	Trim(pwork);
	for(i = 0; i < g_WorkAddress.count ; i++)
	{
		Trim(g_WorkAddress.work[i]);
		if(strcmp(g_WorkAddress.work[i],pwork)==0)
		{
			sprintf(tempWsnAddr,"%s",g_WorkAddress.wsn_addr[i]);
			ret=1;
			break;
		}
	}
	if(ret!=1)
		return -100;
	sprintf(pchWsnAddr,"WSNADDR=%s",tempWsnAddr);
	return atoi(tempWsnAddr);
}

int LogIt( char *sBuff,char *head)                                                                                                                                              
{
	FILE *logFile=NULL;                                                   
	char fileName[255];
	int Pid=0;
	struct timeval tpstart;
                                                             
	Pid=getpid();
	memset(fileName,0,sizeof(fileName));
                                                            
	time_t nowTime;                                                
	struct tm LocalTime;

	nowTime=time(NULL);                                                
	memcpy(&LocalTime,localtime(&nowTime),sizeof(struct tm));             
	sprintf(fileName,"%s%04d%02d%02d.log",head,LocalTime.tm_year+1900,LocalTime.tm_mon+1,LocalTime.tm_mday);
	                                                                                                                                 
	logFile=fopen(fileName,"ab+");
	if(logFile==NULL)
	{
	  return -1;
	}
	gettimeofday(&tpstart,NULL);
	fprintf(logFile,"%02d.%02d.%02d.%06ld:%d:%s",LocalTime.tm_hour,LocalTime.tm_min,LocalTime.tm_sec,tpstart.tv_usec,Pid,sBuff);
	fflush(logFile);
	fclose(logFile);
	return 0;
} 

int getConfpath(char *path)                        
{
	char tmp[255];                                                                                                     
	memset(tmp,0,sizeof(tmp));                              	                                                
	sprintf(tmp,"%s",getenv("INTERAPPSER"));                            
	strcpy(path,tmp);
	return 0;
} 

int convertpath(char *path)                                                                      
{                                                    
	char temp[255],tmp[255],out[255];                
	char *p1=NULL,*p2=NULL;                          
	int len;                                         
                                                     
	memset(temp,0,sizeof(temp));                     
	memset(tmp,0,sizeof(tmp));                       
	memset(out,0,sizeof(out));                       
	p1=strchr(path,'$');                             
	if(p1==NULL)                                     
	{                                                
	 return 0;                                       
	}                                                
	p2=strchr(path,'/');                             
	if(p2==NULL)                                     
	{                                                
	  sprintf(temp,"%s",p1+1);                       
	  memset(path,0,sizeof(path));                   
	  if(getenv(temp)==NULL)                         
      {                                              
        return -1;                                   
      }                                              
      else                                           
      {                                              
       sprintf(path,"%s",getenv(temp));              
       if(temp[0]==0)                                
       return -2;                                    
       else                                          
       return 0;                                     
      }                                              
    }                                                
    else                                             
    {                                                
    	strncpy(temp,p1+1,p2-p1-1);                  
	   if(getenv(temp)==NULL)                        
       {                                             
        return -1;                                   
       }                                             
       else                                          
      {                                              
       snprintf(out,sizeof(out)-1,"%s",getenv(temp));
       if(temp[0]==0)                                
       return -2;                                    
       len = strlen(out);                            
       if(out[len-1]=='/')                           
       out[len-1]='\0';                              
       snprintf(tmp,sizeof(tmp)-1,"%s",p2);          
       strcat(out,tmp);                              
       sprintf(path,"%s",out);                       
       return 0;                                     
      }                                              
    }  
}

int readcfg(char *cfgname,char *section,char *key,char *value)                                              
{                                                             
	FILE *cfgfile;                                            
	char buff[1024];                                          
	char section_ext[1024];                                   
	char key_value[1024];                                     
	int find=0;                                               
	char value_ext[1024];                                     
                                                              
	if((cfgfile=fopen(cfgname,"r"))==NULL)                    
	return -1;                                                
                                                              
	/*查找section*/                                           
	sprintf(section_ext,"[%s]",section);                      
                                                              
	for(;fgets(buff,sizeof(buff),cfgfile);)                   
	{                                                         
		if(buff[0]=='#')                                      
			continue;                                         
		if(buff[0]!='[')                                      
			continue;                                         
		if(strstr(buff,section_ext)==NULL)                    
			continue;                                         
		else                                                  
		{                                                     
			find=1;                                           
			break;                                            
		}                                                     
	}                                                         
                                                              
	if(find==0)                                               
	{                                                         
		fclose(cfgfile);                                      
		return -2;                                            
	}                                                         
                                                              
	/*查找key=value*/                                         
	value_ext[0]=0;                                           
	find=0;                                                   
                                                              
	for(;fgets(buff,sizeof(buff),cfgfile);)                   
	{                                                         
		if(buff[0]=='#')                                      
			continue;                                         
		if(strstr(buff,key)==NULL)                            
			continue;                                         
		else                                                  
		{                                                     
			sprintf(key_value,"%s=%%s\n",key);                
			sscanf(buff,key_value,value_ext);                 
			find=1;                                           
			break;                                            
		}                                                     
	}                                                         
                                                              
	fclose(cfgfile);                                          
	if(find==0)                                               
		return -3;                                            
                                                              
	if(value_ext[0]==0)                                       
		return -4;                                            
	else                                                      
	{                                                         
		strcpy(value,value_ext);                              
		return 1;                                             
	}                                                                                                         
}

char * 
ALLTrim(char *sBuf)      /*modify by miaoyl at 20110322*/                                                                                                              
{                                                                               
	int i, iFirstChar, iEndPos, iFirstPos;                                      
                                                                                
        iEndPos = iFirstChar = iFirstPos = 0;                                   
                                                                                
        for (i = 0; sBuf[i] != '\0'; i++)                                       
        {                                                                       
                if (sBuf[i] == ' '||sBuf[i]=='\n'||sBuf[i]=='\r'||sBuf[i]=='\t')
                {                                                               
                        if (iFirstChar == 0)                                    
                                iFirstPos++;                                    
                }                                                               
                else                                                            
                {                                                               
                        iEndPos = i;                                            
                        iFirstChar = 1;                                         
                }                                                               
        }                                                                       
                                                                                
        for (i = iFirstPos; i <= iEndPos; i++)                                  
                sBuf[i-iFirstPos] = sBuf[i];                                    
                                                                                
        sBuf[i-iFirstPos]='\0';                                                                                                            
} 

int LockFile(char* filename)
{
    FILE *fd=NULL;
    char pidstr[10];
    pid_t pid;
    int mayLock = -1;
    fd = fopen(filename,"r");		/*打开一个已经存在的文件*/
    if(fd != NULL)      			/*标志文件存在*/
    {
        fscanf(fd,"%s",pidstr);		/*将FD指向的文件中的PID读到PIDSTR中*/
        fclose(fd);
        pid = atoi(pidstr);
        if (pid < 2)        		/*不是有效的用户进程ID*/
            mayLock = 1;
        else
        {
            if (kill(pid,0) < 0)    /*检测进程有效性*/
            {
                if (errno == ESRCH)	/*进程失效*/
                    mayLock = 1;
                else            	/*执行错误*/
                    mayLock = -1;
            }
            else                    /*进程处于活动中*/
                mayLock = 0;
        }
    }
    else
    {
        if (errno == ENOENT || errno == EBADF)      /*标志文件不存在，没有活动进程*/
            mayLock = 1;
        else
            mayLock = -1;
    }   /* fd != NULL */
    if (mayLock == 1)
    {
        fd = fopen(filename,"w");
        if (fd == NULL)
        {
            mayLock = -1;
        }
        fprintf(fd,"%d\n",getpid());
    		fclose(fd);
    }
    return mayLock;
}

int	InitServerSocket(int *socketfd,	char *ip, char *port)
{
	int	handleofsocket;
	int	reuseaddr =	1, keepalive = 1;

	/*//建立连接*/
	struct sockaddr_in addr;
	handleofsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (handleofsocket == -1)
	{
		*socketfd =	errno;
		return -1;
	}
	if (setsockopt(handleofsocket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	if (setsockopt(handleofsocket, SOL_SOCKET, SO_REUSEPORT, &reuseaddr, sizeof(reuseaddr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	memset((char	*) &addr,0, sizeof(addr));
	addr.sin_family	= AF_INET;
	if(ip==NULL||strlen(ip)==0||strcmp(ip,"INADDR_ANY")==0)
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	else
	{
		addr.sin_addr.s_addr = inet_addr(ip);
	}

	addr.sin_port =	htons((unsigned	short)atoi(port));
	if ( bind(handleofsocket,(struct sockaddr *)(&addr),sizeof(addr))	== -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}

	if (listen(handleofsocket,BACKLOG) == -1)
	{
		*socketfd =	errno;
		close(handleofsocket);
		return -1;
	}
	*socketfd =	handleofsocket;
	return 0;
}

int Checkheartbeat(char* recvbuff)
{
	if(recvbuff == NULL)
	{
		return -2;
	}
	char TransCode[13];
	
	memset(TransCode,0,sizeof(TransCode));
	
	strncpy(TransCode,&recvbuff[30],12);
	TransCode[12] = '\0';
	Trim(TransCode);
	if(!strcmp(TransCode,"keep"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
int GetStringNumber(char* string_buf, char** arrString)
{
	int 	i,ilen;
	char   	result_buf[MAX_LEGALIP_COUNT*20];
	char * 	string_flag;	
	char *tempString[MAX_LEGALIP_COUNT];
	
	i=0;
	
	memset(result_buf,'\0',strlen(string_buf));
	memcpy(result_buf,string_buf,strlen(string_buf));
	
	/*根据 ‘|’ 依次循环取出string*/
	while((string_flag=strchr(string_buf,'|')) != NULL)
	{
		ilen = string_flag-string_buf;
		result_buf[ilen]='\0';		
		strcpy(arrString[i],result_buf);		
		/*指针下移一位，去掉'|'*/
		string_flag=string_flag + 1;
		memcpy(result_buf,string_flag,strlen(string_flag));
		result_buf[strlen(string_flag)]='\0';
		memcpy(string_buf,result_buf,strlen(result_buf));	
		string_buf[strlen(result_buf)]='\0';
		i+=1;		
	}	
	strcpy(arrString[i],string_buf);
	i+=1;
	
	return i;
}
void sig_chld(int signal_type)
{
	pid_t pid;
	int	status;
	/* WNOHANG								  */
	/*	 means to return immediately if	no child is	there to be	waited for.	*/
	/*	 pid is	the	child process which	exit				  */
	while (	(pid = waitpid(-1,&status, WNOHANG)) > 0)
	{
		sprintf(loginfo,"child process <%d>	exit!\n",pid);
		LogIt(loginfo,HEAD);
	}
}

void sig_chld1(int signal_type)
{
	pid_t pid;
	int	status;
	#ifdef _debug_
	sprintf(loginfo,"sonchild exit\n");
	LogIt(loginfo,HEAD);
	#endif
	while (	(pid = waitpid(-1,&status, WNOHANG)) > 0)
	{
		sprintf(loginfo,"sonchild process <%d>	exit!",pid);
		LogIt(loginfo,HEAD);
	}
}

int RecvResultOnHP(int socketfd, char *resultstr,int length,int limit)
{
  char chRecvBuff[1024];
  int numbytes=0,i=0;    
	int iRecvSucFlag = 0;
	int ret;
	fd_set readfds;

	if(length<=0)
		return -1;

  memset(chRecvBuff,0,sizeof(chRecvBuff));
  memset(resultstr,0,length);
	numbytes=recv(socketfd,chRecvBuff,length,0);

	if (numbytes == 0)
	{
		if (errno == EAGAIN )
			return -9;		
		if (errno == EBADF || errno == ESRCH || errno == ENOTCONN || errno == ENOTSOCK || errno == EPIPE)
			return -6;
		if (errno == EINTR)
			return -7;
		else
		{
			#ifdef _debug_
				sprintf(loginfo,"Recv failed[%d][%s]!\n",errno,strerror(errno));
				LogIt(loginfo,HEAD);
			#endif
			return -8;
		}
	}
	else if (numbytes < 0)
	{
		return -1;
	}
    
  for(i=0;i<numbytes;i++)
	{		
		if (chRecvBuff[i] == 0)
			;
    else
    	iRecvSucFlag++;   	
	}
	
	if(iRecvSucFlag <= 0)
	{
		#ifdef _debug_
			sprintf(loginfo,"client send error\n");
			LogIt(loginfo,HEAD);
		#endif
		return -9;
	}	
	if (numbytes >= length)
   {
		memcpy(resultstr,chRecvBuff,length-1);
		resultstr[length-1]='\0';
	}
	else
	{
		memcpy(resultstr,chRecvBuff,numbytes);
		resultstr[numbytes]='\0';
	}
  return numbytes;
}

int getPara(const char* buffer,char*pFlag,STRPARA* pPara)
{
	char temp_buf[RECV_SIZE + 1];
	char *temp_flag = NULL;
	int  i = 0;
	char paraValue[1024];
	int  ifind = 0;
	char *find_flag = NULL;
	int  iret = 0;
	char *HCFlag = NULL;
	
	memset(paraValue,0,sizeof(paraValue));	
	memset(temp_buf,0,sizeof(temp_buf));	
	strcpy(temp_buf, buffer);
	
	temp_flag = temp_buf;	
	i = 0;
	do
	{
		memset(paraValue,0,sizeof(paraValue));
		find_flag = strstr(temp_flag, pFlag);
		ifind = find_flag-temp_flag;
		
		if(find_flag == NULL)
		{
			strcpy(paraValue,temp_flag);
			iret = 1;
		}
		else
		{
			strcpy(paraValue,temp_flag);
			paraValue[ifind] = '\0';
		}
		strcpy(pPara->acParaName[i],paraValue);
		
		if(find_flag!=NULL)
			temp_flag = find_flag + strlen(pFlag);		
		i++;		
		if(iret) break;
	}while(1);
	pPara->nParaCount = i;
	
	HCFlag = strstr(pPara->acParaName[i-1],"\n");
	if(HCFlag != NULL)
	{
		HCFlag[0] = '\0';
	}
	return i;
}
int Create_share(key_t Key,int nSize)
{
	int ret        = 0;
	int tempSemId = 0;

	tempSemId = shmget(Key,nSize,IPC_CREAT|0666);
	if(tempSemId == -1)
	{
		#ifdef _debug_
			sprintf(loginfo,"CreateCfgshm error[%s][%d]!,",strerror(errno),errno);
			LogIt(loginfo,HEAD);
		#endif
		return -1;
	}
	return tempSemId;
}
int creat_semaphore(key_t keyval,int numsems)
{
	int sid;
	if (!numsems)
		return(-1);
	if ((sid=semget(keyval,numsems,IPC_CREAT|0660))==-1)
	{
		return -1;
	}
	return(sid);
}
int init_semaphore(int sid,int semnum,int initval)
{
	int ret;
	union semun semopts;
	semopts.val=initval;
	ret=semctl(sid,semnum,SETVAL,semopts);
	return (ret);
}

int CreateSEM(key_t Key)
{
	int SID=0;
	SID=creat_semaphore(Key,1);
	if(SID<0)
	{
		sprintf(loginfo,"Create semaphore error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	if(init_semaphore(SID,0,1)<0)
	{
		sprintf(loginfo,"Init semaphore error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	return SID;
}

int IncreaseChildPid(int shm,int sem,pid_t pid)
{
	SHAREBUFFER* pbuffer = NULL;
	int i = 0;
	int ret=0;
	
	pbuffer=(SHAREBUFFER *)shmat(shm,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	ret=locksem(sem,0);
	sprintf(loginfo,"locksem[%d]\n",ret);
	LogIt(loginfo,HEAD);
	for(i = 0; i < MAXPROCNUMBER ; i++)
	{
		if(pbuffer->ProcPid[i] == -1)
		{
			pbuffer->ProcPid[i] = pid;
			break;
		}
	}
	(pbuffer->nProcCount)++;
	ret=unlocksem(sem,0);
	sprintf(loginfo,"unlocksem[%d]\n",ret);
	LogIt(loginfo,HEAD);
	shmdt(pbuffer);
	return 0;
	
}
int locksem(int sid,int member)
{
	int ret;
	struct sembuf sem_lock;/*={member,-1,SEM_UNDO}	*/
	memset(&sem_lock,0,sizeof(struct sembuf));
	//if (member<0||member>MAXSEMNUM)
	if (member<0)
		return (-2);
	sem_lock.sem_num=member;
	sem_lock.sem_op=-1;
	sem_lock.sem_flg=SEM_UNDO;
	ret=semop(sid,&sem_lock,1);
	return (ret);
}
int unlocksem(int sid, int member)
{
	int ret;/*semval*/
	struct sembuf sem_unlock;/*={member,1,SEM_UNDO};*/
	memset(&sem_unlock,0,sizeof(struct sembuf));
	//if (member <0||member>MAXSEMNUM)
	if (member <0)
		return (-2);
	sem_unlock.sem_num=member;
	sem_unlock.sem_op=1;
	sem_unlock.sem_flg=SEM_UNDO;
	ret=semop(sid,&sem_unlock,1);
	return (ret);
}
void TimeOutAlarm(int sig)
{
	sprintf(loginfo,"--系统处理超时--");
	LogIt(loginfo,HEAD);
}

int CheckProcNumber(int semid, int shmid,int MaxProcNumber)
{
	SHAREBUFFER* pbuffer = NULL;
	pbuffer=(SHAREBUFFER *)shmat(shmid,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	locksem(semid,0);
	if(pbuffer->nProcCount>=MaxProcNumber)
	{
		unlocksem(semid,0);
		shmdt(pbuffer);
		return -1;
	}
	else
	{
		unlocksem(semid,0);
		shmdt(pbuffer);
		return 0;
	}
} 

void Killsonpid(int shm,int sem)
{
	int i=0;
	int ret1=1,ret2=1;
	
	pid_t *Child=NULL;
	SHAREBUFFER *Addr=NULL;

	Addr=(SHAREBUFFER *)shmat(shm,0,0);
	if(Addr==NULL)
	{
		sprintf(loginfo,"in Killsonpid,get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return ;
	}

	locksem(sem,0);
	for(i=0;i<MAXPROCNUMBER;i++)
	{
		if(Addr->ProcPid[i]>0)
		{
			kill(Addr->ProcPid[i],SIGTERM);
			sleep(1);
		}
	}
	unlocksem(sem,0);
	while(1)
	{
		ret1=1;
		ret2=1;
		locksem(sem,0);
		for(i=0;i<MAXPROCNUMBER;i++)
		{
			if(Addr->ProcPid[i]>0)
			{
				#ifdef _debug_
					sprintf(loginfo,"Addr->ProcPid[%d]=[%d]\n",i,Addr->ProcPid[i]);
   				LogIt(loginfo,HEAD);
				#endif
				ret2=kill(Addr->ProcPid[i],0);
				ret1=ret1*ret2;
			}
		}
		unlocksem(sem,0);
		if(ret1!=0)
		{
			break;
		}
		else
		{
			sleep(1);
			#ifdef _debug_
				sprintf(loginfo,"In killSonPid...\n");
   			LogIt(loginfo,HEAD);
			#endif
			continue;
		}
	}
	shmdt(Addr);
} 

int decreaseChildPid(int shm,int sem,pid_t pid)
{
	SHAREBUFFER* pbuffer = NULL;
	int i = 0;
	int ret=0;
	
	pbuffer=(SHAREBUFFER *)shmat(shm,0,0);
	if(pbuffer==NULL)
	{
		sprintf(loginfo,"get shm error[%s]",strerror(errno));
		LogIt(loginfo,HEAD);
		return -1;
	}
	ret=locksem(sem,0);
	sprintf(loginfo,"locksem[%d]\n",ret);
	LogIt(loginfo,HEAD);
	for(i = 0; i < MAXPROCNUMBER ; i++)
	{
		if(pbuffer->ProcPid[i] == pid)
		{
			pbuffer->ProcPid[i] = -1;
			break;
		}
	}
	(pbuffer->nProcCount)--;
	ret=unlocksem(sem,0);
	sprintf(loginfo,"unlocksem[%d]\n",ret);
	LogIt(loginfo,HEAD);
	shmdt(pbuffer);
	return 0;
}



