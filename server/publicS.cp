#include "stdarg.h"
#include "publicsrv.h"
#include "boss_srv.h"
#include "boss_define.h"

extern int errno;
/* 字符串处理类 */
/************************************/
/* name:     rtrim					*/
/* writer:   zhaohao           		*/
/* modi:     zhaohao           		*/
/* date:     20020429       		*/
/* function: 去掉字符串右边的空格	*/
/************************************/
char *rtrim(char *src)
{
	int i=0;

	if (!strstr(src, " ")) return src;

	for(i=strlen(src); i>0; i--) {
		if (*(src+i-1) != ' ') {
			*(src+i) = '\0';
			break;
		}
	}
	if (i == 0) {
		*src = '\0';
	}

	return src;
}

/************************************/
/* name:     ltrim					*/
/* writer:   zhaohao           		*/
/* modi:     zhaohao           		*/
/* date:     20020429       		*/
/* function: 去掉字符串左边的空格	*/
/************************************/
char *ltrim(char *src)
{
	int i=0;

	if (!strstr(src, " ")) return src;

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) != ' ')
			return src+i;
	}

	if (i == strlen(src)) {
		*src = '\0';
	}

	return src;
}

/************************************/
/* name:     Trim					*/
/* writer:   zhaohao           		*/
/* modi:     zhaohao           		*/
/* date:     2001.11.1       		*/
/* function: 去掉字符串两边的空格	*/
/************************************/
char* Trim(char *S)
{
	int I = 0, i = 0, L = 0;

	if (!strstr(S, " ")) return S;

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

/********************************/
/* name:     Ssubstr			*/
/* writer:   zhaohao			*/
/* modi:     zhaohao			*/
/* date:     2001.11.1			*/
/* function: 截取src从begin开始	*/
/*           截取length位的字符	*/
/*           串返回				*/
/*           (字符串的第一位为1)*/
/********************************/
char *Ssubstr(char *src, int begin, int length, char *dest)
{
	int tmplen=0;

	tmplen = strlen(src);

	if (begin < 1 || length < 1 || begin > tmplen)
		return NULL;

	if(begin-1+length >= tmplen)
		strcpy(dest, src+begin-1);
	else
		strncpy(dest, src+begin-1, length);

	return dest;
}

extern char *substr(char *source,int begin,int num)
{
    int i;
    static char dest[2048+1]="\0";
    if(begin<1 || num<1) return NULL;
    for(i=begin-1;i<begin+num-1;i++){
        dest[i-begin+1]=source[i];
    }
    dest[i-begin+1]='\0';
    return dest;
}


/****************************************************/
/* name:     ChnStrSplit							*/
/* writer:   chendx         						*/
/* modi:                    						*/
/* date:     2011.05.15     						*/
/* function: 按中文字符进行字符串截取				*/
/* 输入参数: src原字符串，fieldLen最大截取字节长度	*/
/*			 outStr输出字符串，location截取位置		*/
/****************************************************/
int ChnStrSplit(char* src, int fieldLen, char *outStr, int *location)
{
	int length = 0;
	int i=0;
	char *p, *q;

	q = p = src;

	length=strlen(src);

	if(fieldLen > 1024 )
	{
		fieldLen = 1024;
	}

	if(fieldLen > length)
	{
		strcpy(outStr,src);
		*location = length;
	}
	else
	{
		while(i < fieldLen)
		{
			if((unsigned int)(*q) > 127)
			{
				q=q+2;
				i=i+2;
				if(i > fieldLen)
				{
					i=i-2;
					break;
				}
			}
			else
			{
				q++;
				i++;
			}
		}

		*location = i;
		strncpy(outStr,src,i);
		outStr[i] = '\0';
	}
	return 0;
}

/****************************************************/
/* name:     ChnStrLen								*/
/* writer:   sunzx									*/
/* modi:                    						*/
/* date:     2011.07.13     						*/
/* function: 计算字符长度，中文字符算一个字符		*/
/* 输入参数: src字符串								*/
/* 输出参数: 字符串长度								*/
/****************************************************/
int ChnStrLen(char* src)
{
	int length = 0;
	int charLen = 0;
	char *q;

	q = src;

	length=strlen(src);
	
	charLen = 0;
	while((q - src) < length)
	{
		if((unsigned int)(*q) > 127)
		{
			q=q+2;
		}
		else
		{
			q++;
		}
		charLen++;
	}

	return charLen;
}

/****************************************************/
/* name:     Slower									*/
/* writer:   zhaohao           						*/
/* modi:     zhaohao           						*/
/* date:     2001.11.1       						*/
/* function: 将大写字母转换为小写字母，其他字符不变	*/
/****************************************************/
char *Slower(char *src)
{
	int i=0;

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) >= 'A' && *(src+i) <= 'Z')
			*(src+i) += 32;
	}

	return src;
}

/****************************************************/
/* name:     Supper									*/
/* writer:   zhaohao           						*/
/* modi:     zhaohao           						*/
/* date:     2001.11.1       						*/
/* function: 将小写字母转换为大写字母，其他字符不变	*/
/****************************************************/
char *Supper(char *src)
{
	int i=0;

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) >= 'a' && *(src+i) <= 'z')
			*(src+i) -= 32;
	}

	return src;
}

/*******************************************
*    desc:    解开字符串
*
*    inparm1: 需要解开的字符串
*    inparm2: 分隔符
*    inparm3: 结果数组
*
********************************************/
int gettok(char *str,char *tok,char **tokstr)
{
	int i=0;
	char *p;

	if(tok==NULL)
		return -1;

	while(str)
	{
		tokstr[i++]=str;
		p=strstr(str,tok);
		if(p)
		{
			*p=0x0;
			str=p+strlen(tok);
		}
		else
		{
			break;
		}
	}

	tokstr[i]=NULL;

	return 0;
}

/**********************************************
 @wt PRG  : pubApartStr
 @wt FUNC : 按照从左向右的顺序，将Father
 @wt      : 中以CH为分隔符的子串
 @wt      : 取到Son中，剩下的串存贮在Uncle中。
 @wt 0 Father
 @wt 1 CH
 @wt 2 Uncle  -- OUT
 @wt 3 Son    -- OUT
 *********************************************/
void pubApartStr(char *Father,char CH,char *Uncle,char *Son)
{
	int i,j;
	for(i=0,j=0;;i++,j++)
	{
		if(Father[i]==0||Father[i]==CH)
		{
			break;
		}
		Son[j]=Father[i];
	}
	i++;
	Son[j]=0;
	for(j=0;;i++,j++)
	{
		if(Father[i]==0)
		{
			break;
		}
		Uncle[j]=Father[i];
	}
	Uncle[j]=0;
}

#define ROWLEN 2048     /* 输入字符串默认长度 */
/**************************************************
  * Auth: PeiJH
  * Name: StrListToArray
  * Func: 将输入字符串解析成字符串数组
  * Input:字符串,分隔符
  * Output:字符串数组,字符串数组中字符串个数
  * Date: 2003/10/14
****************************************************/
void StrListToArray(char *InStrList, char ch,char Array[][ROWLEN])
{
	int  tmplen=0,begin=0,end=0,num =0,i =0 ,k=0;
	char vTmp_List[ROWLEN];
	
	memset(vTmp_List,0x00,sizeof(vTmp_List));
	
	strcpy(vTmp_List,InStrList);
	if(strlen(ltrim(rtrim(vTmp_List)))!=0)
	{
		tmplen=strlen(vTmp_List);
		begin = 1; end = 1; k=0;
		
		for(i=0;i<=tmplen;i++)
		{
			if(vTmp_List[i]==ch)
			{
				end = i;
				num = end - begin + 1;

				/*yangzh add 2004/11/24 for '|||'*/
				if (num <= 0) 
				{
					begin = end+ 2; 
					continue;
				}
				/*yangzh add 2004/11/24 for '|||'*/

				vTmp_List[i]='\0';
				strcpy(Array[k],substr(vTmp_List,begin,num));
				begin = end +2;
				k++;
			}
		}
	}
}

/**************************************************
  * Auth: PeiJH
  * Name: StrListToArrayByToken
  * Func: 修改函数StrListToArray未将空放入数组。
  * Input:字符串,分隔符
  * Output:字符串数组,字符串数组中字符串个数
  * Date: 2011/11/22
****************************************************/
void StrListToArrayByToken(char *InStrList, char ch,char Array[][ROWLEN])
{
	int  tmplen=0,begin=0,end=0,num =0,i =0 ,k=0;
	char vTmp_List[ROWLEN];

	memset(vTmp_List,0x00,sizeof(vTmp_List));

	strcpy(vTmp_List,InStrList);
	if(strlen(ltrim(rtrim(vTmp_List)))!=0)
	{
		tmplen=strlen(vTmp_List);
		begin = 1; end = 1; k=0;

		for(i=0;i<=tmplen;i++)
		{
			if(vTmp_List[i]==ch)
			{
				end = i;
				num = end - begin + 1;

				if (num <= 0)
				{

					strcpy(Array[k],"\0");
					k++;
					begin = end + 2;

					continue;
				}				

				vTmp_List[i]='\0';
				strcpy(Array[k],substr(vTmp_List,begin,num));
				begin = end + 2;
				k++;
			}
		}
	}
}

/***************************************
 函数名称  strToFilter
 编码日期  2004/12/15
 编码人员  yangzh
 调用过程：
 功能描述：将以分隔符为标志的字符串，转换为数据库In操作可用的字符串
 输入参数：
     原字符串
     分隔符
     新字符串
 输出参数：
 调用示例：
          调用前 01, 0101, 0102, 0103
          调用后 '01', '0101', '0102', '0103'
****************************************/
int strToFilter(char *InStr, char *strFilter, int strlen1, int strlen2)
{
	int len1=0, len2=1;
	
	strcat(strFilter, "'");
	for( ;(len1<=strlen1) && (len2<=strlen2); )
	{
		if (InStr[len1] == ' ') 
		{
			len1++;
		}
		else if (InStr[len1] == ',') 
		{
			len1++;
			strcat(strFilter, "', '");
			len2+=4;
		} 
		else 
		{
			strFilter[len2++] = InStr[len1++];
		}
	}
	strcat(strFilter, "'");
	return 0;
}

int strToRows(const char *src,char separateChar,char **dest, int colLen)
{
	const char *p, *q;
	int i;
	char *s;

	if (src == NULL)
	{
		return 0;
	}

	s=(char*)dest;
	q = p = src;
	i=0;
	while ( (q=strchr(p, (int)separateChar)) != (char *)NULL)
	{
		strncpy( s + i * colLen, p,  q - p);
		s[i * colLen + q - p] = '\0';
		p = q + 1;
		i++;
	}
	return i;
}

/*获取字符串src中含有指定字符的个数 added by chendx*/
int strToNum(const char *src,char separateChar)
{
	const char *p, *q;
	int i;
	char *s;

	if (src == NULL)
	{
		return 0;
	}

	q = p = src;
	i=0;
	while ( (q=strchr(p, (int)separateChar)) != (char *)NULL)
	{

		p = q + 1;
		i++;
	}
	return i;
}
/*获取字符串src中第i和第i+1个指定字符separateChar间的字符串 added by chendx*/
char *getSpStr(const char *str, char separateChar, int i)
{
	char tmp[40][256];
	char tmpstr[1000];
	char tmpstr1[1000];
	char vdealim[1];
	char *p;
	char* q;
	int j;

	memset(tmpstr,0,sizeof(tmpstr));
	memset(tmpstr1,0,sizeof(tmpstr1));
	memset(vdealim,0,sizeof(vdealim));
	vdealim[0]= separateChar;
	if ( i >= 40 ) return "";
	for( j=0;j<40;j++)
	{
		strcpy(tmp[j],"");
	}
	strcpy(tmpstr,str);

	j=0;

	while(strlen(tmpstr)>0)
	{
		if(tmpstr[0]==vdealim[0])
		{
			strcpy(tmp[j],"");
			strncpy(tmpstr,tmpstr+1,strlen(tmpstr));
		}
		else
		{
			strcpy(tmpstr1,tmpstr);
			p=strtok(tmpstr1,vdealim);
			strcpy(tmp[j],p);
			strncpy(tmpstr,tmpstr+strlen(tmp[j])+1,strlen(tmpstr));
		}
		j++;
	}
	return tmp[i];
}

/**
 *     把字符串中的值按照指定的分隔符，分别存放到后面的动态变量中，这些变更的
 * 类型必须为“char*”。
 *     一般来说，源字符串中字段域的个数，应该与动态变量的个数相同。如果源字符
 * 串域的个数大于动态变量的个数，出错返回-1；如果源字符串域的个数小于动态变量
 * 的个数，后面的部分动态变更可能接收不到值，程序不认为出错，正常返回已得到值
 * 域的个数；如果源字符串域的个数等于动态变量，正常返回已得到值域的个数。
 * @author lugz
 * @param src	表示将要拆分的源串，源串中的字符串用指定的单个字符separateChar分
 *              隔。如下所示，用竖线分隔的串：
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param ...	变更的类型必须为“char*”，在函数中对这些变更的类型不做校检。
 * @return 返回一个整形值。如果源字符串域的个数大于动态变量的个数，出错返回-1；
 *         如果源字符串域的个数小于动态变量的个数，后面的部分动态变更可能接收
 *         不到值，程序不认为出错，正常返回已得到值域的个数；如果源字符串域的
 *         个数等于动态变量，正常返回已得到值域的个数。
 */
int strToFields(const char* src, char separateChar, ...)
{
	int count = 0;
	const char *p, *q;
	char *field=NULL;
	va_list marker;

	q = p = src;
	va_start( marker, separateChar);
	do
	{
		if ( (q=strchr(p, (int)separateChar)) != (char*)NULL)
		{
			field = va_arg( marker, char*);
			if (field != NULL)
			{
				if ( q - p != 0 )
				{
					strncpy(field, p, q - p);
				}
				field[q - p] = '\0';
			}
			else
			{
				return -1;
			}
			p = q + 1;
			count ++;
		}
	}
	while(q != NULL); /*如果已到达最后一个域就结束*/
	va_end(marker);

	return count;
}

/* 字符串处理类 */
/**
 * 把字符串中的值按照指定的分隔符，分别存放到后面的动态变量中，这些变量的
 * 类型必须为“char*”。
 * 一般来说，源字符串中字段域的个数，应该与动态变量的个数相同。如果源字符
 * 串域的个数大于动态变量的个数，出错返回-1；如果源字符串域的个数小于动态变量
 * 的个数，后面的部分动态变更可能接收不到值，程序不认为出错，正常返回已得到值
 * 域的个数；如果源字符串域的个数等于动态变量，正常返回已得到值域的个数。
 * @author sunzx
 * @inparam src	表示将要拆分的源串，源串中的字符串用指定的单个字符separateChar分
 *			  隔。如下所示，用竖线分隔的串：
 *			 "field1|field2|field3| ... fieldn|"
 * @inparam separateChar实例中为'|'，可以是其他ASCII字符。
 * @outparam ...变量的类型必须为“char*”，在函数中对这些变量的类型不做校检。
 * @outparam ...变量的可容纳数据的容量，当赋予0时，表示忽略溢出的情况。
 * @return 返回一个整形值。如果源字符串域的个数大于动态变量的个数，出错返回动态变量的个数；
 *		 如果源字符串域的个数小于动态变量的个数，后面的部分动态变更可能接收
 *		 不到值，程序不认为出错，正常返回已得到值域的个数；如果源字符串域的
 *		 个数等于动态变量，正常返回已得到值域的个数。
 */
 /* 字符串处理类 2010 吕觉新修改，解决特殊汉字使用高字节ASCII问题 */
int strToFieldsEx(const char* src, char separateChar, ...)
{
	int count = 0;
	const char *p, *q;
	char *field=NULL;
	int field_len = 0;
	int real_len = 0;
	va_list marker;

	q = p = src;
	va_start( marker, separateChar);
	do
	{
		field = va_arg(marker, char*);
		if (field != NULL)
		{
			field_len = va_arg(marker, int);
			if(field_len == 0)
			{
				field_len = 10240;  /*default 10M*/
			}
		}
		else
		{
			break;
		}

		while(((*q)!='\0')&&(*q!=separateChar))
		{
			if((unsigned int)(*q)>127)
			{
				if( (q - p + 1) == field_len )
				{
					field_len--;
				}
				q=q+2;
			}
			else
			{
				q++;
			}
		}

		real_len = (field_len < q - p)?field_len:(q - p);

	   	if ( real_len > 0 )
		{
			strncpy(field, p, real_len);
			field[real_len] = '\0';
		}
		if((*q) != '\0')
		{
			p = q + 1;
			q=p;
		}
		count ++;
	}while((*q) != '\0'); /*如果已到达最后一个域就结束*/

	va_end(marker);

	return count;
}

int StrToFloatArray(float Array[ROWNUM],const char *String,char ch)
{
    const char *p,*q;
	char ss[ROWNUM];
	int  k;

	if (strlen(String)==0)
	   return 1;

	k=0;

	p = String;
	while(*p)
	{
		if((q=strchr(p, ch)) != NULL)
		{
			if (p!= q)
			{
				    strncpy(ss, p, q-p);
					ss[q-p]=0;
			        Array[k]=atof(ss);
			}
			else
			{
				Array[k]=0.00;
			}
			k ++;
		}
		else
			break;
		p=q+1;
	}
	return 0;
}

int StrToCharArray(char Array[ROWNUM][COLNUM],const char *String,char ch)
{
    const char *p,*q;
	char ss[ROWNUM];
	int  k;

	if (strlen(String)==0)
	   return 1;

	k=0;

	p = String;
	while(*p)
	{
		if((q=strchr(p, ch)) != NULL)
		{
			if (p!= q)
			{
				    strncpy(ss, p, q-p);
					ss[q-p]=0;
			        strcpy(Array[k],ss);
			}
			else
			{
				strcpy(Array[k],"0");
			}
			k ++;
		}
		else
			break;
		p=q+1;
	}
	return 0;
}

int StrToIntArray(int Array[ROWNUM],const char *String,char ch)
{
	const char *p,*q;
	char ss[ROWNUM];
	int  k;

	if (strlen(String)==0)
	   return 1;

	k=0;

	p = String;
	while(*p)
	{
		if((q=strchr(p, ch)) != NULL)
		{
			if (p!= q)
			{
				strncpy(ss, p, q-p);
				if (q-p < 12)
				{
					ss[q-p]=0;
					Array[k]=atoi(ss);
				}
				else
				{
					Array[k]=0;
				}
			}
			else
			{
				Array[k]=0;
			}
			k ++;
		}
		else
			break;
		p=q+1;
	}
	return 0;
}

/********************************************************************/
/* 有_DEPRECATED_注释包围的函数表示已经存在相同功能函数，不建议使用 */
/********************************************************************/

#ifdef _DEPRECATED_
char *Srtrim(char *src)
{
	int i=0;

	if (!strstr(src, " ")) return src;

	for(i=strlen(src); i>0; i--) {
		if (*(src+i-1) != ' ') {
			*(src+i) = '\0';
			break;
		}
	}
	if (i == 0) {
		*src = '\0';
	}

	return src;
}
#endif

#ifdef _DEPRECATED_
char *Sltrim(char *src)
{
	int i=0;

	if (!strstr(src, " ")) return src;

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) != ' ')
			return src+i;
	}

	if (i == strlen(src)) {
		*src = '\0';
	}

	return src;
}
#endif

#ifdef _DEPRECATED_
char *Strim(char *src)
{
	int i=0;

	if (!strstr(src, " ")) return src;

	for(i=strlen(src); i>0; i--) {
		if (*(src+i-1) != ' ') {
			*(src+i) = '\0';
			break;
		}
	}
	if (i == 0) {
		*src = '\0';
		return src;
	}

	for(i=0; i<strlen(src); i++) {
		if (*(src+i) != ' ')
			return src+i;
	}

	if (i == strlen(src)) {
		*src = '\0';
		return src;
	}
	return src;
}
#endif

#ifdef _DEPRECATED_
/**********************************************
 @wt PRG  : pubSubStr
 @wt FUNC : 取Father的子串，位置用法同oracle
 @wt      : 中的substr。Son中存贮子串的值。
 @wt 0 父串        Father
 @wt 1 开始位置    Begin
 @wt 2 截取长度    Len
 @wt 3 子串        Son -- OUT
 *********************************************/
void pubSubStr(char *Father,int Begin,int Len,char *Son){
    int     i=0,j=0;
    Sinitn(Son);
    for(i=Begin-1,j=0;i<Begin+Len-1;i++,j++){
        Son[j]=Father[i];
        if(Father[i]==0) break;
    }
    Son[j]=0;
}
#endif

#ifdef _DEPRECATED_
/************************************/
/* name:     get_billfee			*/
/* writer:   hanying           		*/
/* date:     2002.05.22      		*/
/* function: 帐单调整费用解析		*/
/************************************/
float get_billfee(char *in_str)
{
	char *p,*q,str[20],str1[255];
	int i=0,j=0;
	p=in_str;
	q=strchr(in_str,'|');

	if(q!=NULL){
		for(i=0;i<q-p;i++)str[i]=in_str[i];
		str[i++]='\0';
		for(j=0;i<strlen(in_str);j++)str1[j]=in_str[i++];
		str1[j]='\0';
		strcpy(in_str,str1);
	}
	else
		str[0]='\0';

	return atof(str);
}
#endif

