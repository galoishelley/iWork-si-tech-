#include "llayerStruct.h"

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
int CLString::StrToFields(const char* src, char separateChar, ...)
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

/**
 *     把字符串中的值按照指定的分隔符，分别存放到后面的字符串指针数组变量中，这些变更的
 * 类型必须为“char*”。
 * @author lugz
 * @param src	表示将要拆分的源串，源串中的字符串用指定的单个字符separateChar分
 *              隔。如下所示，用竖线分隔的串：
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param ptrs		存放的数组。
 * @return 返回一源串src中包含域的数量，错误返回-1。
 */
int CLString::StrToPtrs(char *src,const char *separateStr,char **ptrs)
{
	int i=0;
	size_t separateStrLen=0;
	char *p;
	
	if(separateStr==NULL)
	{
		return -1;
	}

	separateStrLen = strlen(separateStr);
	while(src)
	{
		ptrs[i++]=src;
		p=strstr(src,separateStr);
		if(p != (char*)NULL)
		{
			*p=0x0;
			src=p+separateStrLen;
		}
		else
		{
			break;
		}
	}
	ptrs[i]=NULL;
	return i;
}

/**
 *     把字符串中的值按照指定的分隔符，分别存放到后面的字符串数组变量中，这些变更的
 * 类型必须为“char*”。
 * @author lugz
 * @param src	表示将要拆分的源串，源串中的字符串用指定的单个字符separateChar分
 *              隔。如下所示，用竖线分隔的串：
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param dest		存放的数组。
 * @param colLen	数组第二维的长度
 * @return 返回一源串src中包含域的数量。
 */
int CLString::StrToRows(char *src,char separateChar,char **dest, int colLen)
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

char* CLString::Trim(char *S)
{
	int I = 0, i = 0, L = 0;

	if (S == NULL)
	{
		return NULL;
	}
		
	L = (int)(strlen(S) - 1);
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

char* CLString::lTrim(char *S)
{
	int I = 0, i = 0, L = 0;

	if (S == NULL)
	{
		return NULL;
	}
		
	L = (int)(strlen(S) - 1);
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
		strncpy(S, S + I, L + 1);
		S[L + 1 - I] = '\0';
	}
	return S;
}

char* CLString::rTrim(char *S)
{
	int I = 0, i = 0, L = 0;

	if (S == NULL)
	{
		return NULL;
	}
		
	L = (int)(strlen(S) - 1);

	while ( (S[L] <= ' ')  && (S[L] > 0 ) )
	{
		L --;
	}
	strncpy(S, S + I, L + 1);
	S[L + 1 - I] = '\0';
	return S;
}
