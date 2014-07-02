#include "stdarg.h"
#include "publicsrv.h"
#include "boss_srv.h"
#include "boss_define.h"

extern int errno;
/* �ַ��������� */
/************************************/
/* name:     rtrim					*/
/* writer:   zhaohao           		*/
/* modi:     zhaohao           		*/
/* date:     20020429       		*/
/* function: ȥ���ַ����ұߵĿո�	*/
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
/* function: ȥ���ַ�����ߵĿո�	*/
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
/* function: ȥ���ַ������ߵĿո�	*/
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
/* function: ��ȡsrc��begin��ʼ	*/
/*           ��ȡlengthλ���ַ�	*/
/*           ������				*/
/*           (�ַ����ĵ�һλΪ1)*/
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
/* function: �������ַ������ַ�����ȡ				*/
/* �������: srcԭ�ַ�����fieldLen����ȡ�ֽڳ���	*/
/*			 outStr����ַ�����location��ȡλ��		*/
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
/* function: �����ַ����ȣ������ַ���һ���ַ�		*/
/* �������: src�ַ���								*/
/* �������: �ַ�������								*/
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
/* function: ����д��ĸת��ΪСд��ĸ�������ַ�����	*/
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
/* function: ��Сд��ĸת��Ϊ��д��ĸ�������ַ�����	*/
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
*    desc:    �⿪�ַ���
*
*    inparm1: ��Ҫ�⿪���ַ���
*    inparm2: �ָ���
*    inparm3: �������
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
 @wt FUNC : ���մ������ҵ�˳�򣬽�Father
 @wt      : ����CHΪ�ָ������Ӵ�
 @wt      : ȡ��Son�У�ʣ�µĴ�������Uncle�С�
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

#define ROWLEN 2048     /* �����ַ���Ĭ�ϳ��� */
/**************************************************
  * Auth: PeiJH
  * Name: StrListToArray
  * Func: �������ַ����������ַ�������
  * Input:�ַ���,�ָ���
  * Output:�ַ�������,�ַ����������ַ�������
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
  * Func: �޸ĺ���StrListToArrayδ���շ������顣
  * Input:�ַ���,�ָ���
  * Output:�ַ�������,�ַ����������ַ�������
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
 ��������  strToFilter
 ��������  2004/12/15
 ������Ա  yangzh
 ���ù��̣�
 �������������Էָ���Ϊ��־���ַ�����ת��Ϊ���ݿ�In�������õ��ַ���
 ���������
     ԭ�ַ���
     �ָ���
     ���ַ���
 ���������
 ����ʾ����
          ����ǰ 01, 0101, 0102, 0103
          ���ú� '01', '0101', '0102', '0103'
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

/*��ȡ�ַ���src�к���ָ���ַ��ĸ��� added by chendx*/
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
/*��ȡ�ַ���src�е�i�͵�i+1��ָ���ַ�separateChar����ַ��� added by chendx*/
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
 *     ���ַ����е�ֵ����ָ���ķָ������ֱ��ŵ�����Ķ�̬�����У���Щ�����
 * ���ͱ���Ϊ��char*����
 *     һ����˵��Դ�ַ������ֶ���ĸ�����Ӧ���붯̬�����ĸ�����ͬ�����Դ�ַ�
 * ����ĸ������ڶ�̬�����ĸ�����������-1�����Դ�ַ�����ĸ���С�ڶ�̬����
 * �ĸ���������Ĳ��ֶ�̬������ܽ��ղ���ֵ��������Ϊ�������������ѵõ�ֵ
 * ��ĸ��������Դ�ַ�����ĸ������ڶ�̬���������������ѵõ�ֵ��ĸ�����
 * @author lugz
 * @param src	��ʾ��Ҫ��ֵ�Դ����Դ���е��ַ�����ָ���ĵ����ַ�separateChar��
 *              ����������ʾ�������߷ָ��Ĵ���
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param ...	��������ͱ���Ϊ��char*�����ں����ж���Щ��������Ͳ���У�졣
 * @return ����һ������ֵ�����Դ�ַ�����ĸ������ڶ�̬�����ĸ�����������-1��
 *         ���Դ�ַ�����ĸ���С�ڶ�̬�����ĸ���������Ĳ��ֶ�̬������ܽ���
 *         ����ֵ��������Ϊ�������������ѵõ�ֵ��ĸ��������Դ�ַ������
 *         �������ڶ�̬���������������ѵõ�ֵ��ĸ�����
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
	while(q != NULL); /*����ѵ������һ����ͽ���*/
	va_end(marker);

	return count;
}

/* �ַ��������� */
/**
 * ���ַ����е�ֵ����ָ���ķָ������ֱ��ŵ�����Ķ�̬�����У���Щ������
 * ���ͱ���Ϊ��char*����
 * һ����˵��Դ�ַ������ֶ���ĸ�����Ӧ���붯̬�����ĸ�����ͬ�����Դ�ַ�
 * ����ĸ������ڶ�̬�����ĸ�����������-1�����Դ�ַ�����ĸ���С�ڶ�̬����
 * �ĸ���������Ĳ��ֶ�̬������ܽ��ղ���ֵ��������Ϊ�������������ѵõ�ֵ
 * ��ĸ��������Դ�ַ�����ĸ������ڶ�̬���������������ѵõ�ֵ��ĸ�����
 * @author sunzx
 * @inparam src	��ʾ��Ҫ��ֵ�Դ����Դ���е��ַ�����ָ���ĵ����ַ�separateChar��
 *			  ����������ʾ�������߷ָ��Ĵ���
 *			 "field1|field2|field3| ... fieldn|"
 * @inparam separateCharʵ����Ϊ'|'������������ASCII�ַ���
 * @outparam ...���������ͱ���Ϊ��char*�����ں����ж���Щ���������Ͳ���У�졣
 * @outparam ...�����Ŀ��������ݵ�������������0ʱ����ʾ��������������
 * @return ����һ������ֵ�����Դ�ַ�����ĸ������ڶ�̬�����ĸ����������ض�̬�����ĸ�����
 *		 ���Դ�ַ�����ĸ���С�ڶ�̬�����ĸ���������Ĳ��ֶ�̬������ܽ���
 *		 ����ֵ��������Ϊ�������������ѵõ�ֵ��ĸ��������Դ�ַ������
 *		 �������ڶ�̬���������������ѵõ�ֵ��ĸ�����
 */
 /* �ַ��������� 2010 �������޸ģ�������⺺��ʹ�ø��ֽ�ASCII���� */
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
	}while((*q) != '\0'); /*����ѵ������һ����ͽ���*/

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
/* ��_DEPRECATED_ע�Ͱ�Χ�ĺ�����ʾ�Ѿ�������ͬ���ܺ�����������ʹ�� */
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
 @wt FUNC : ȡFather���Ӵ���λ���÷�ͬoracle
 @wt      : �е�substr��Son�д����Ӵ���ֵ��
 @wt 0 ����        Father
 @wt 1 ��ʼλ��    Begin
 @wt 2 ��ȡ����    Len
 @wt 3 �Ӵ�        Son -- OUT
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
/* function: �ʵ��������ý���		*/
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

