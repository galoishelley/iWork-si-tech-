#include "llayerStruct.h"

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
	while(q != NULL); /*����ѵ������һ����ͽ���*/
	va_end(marker);

	return count;
}

/**
 *     ���ַ����е�ֵ����ָ���ķָ������ֱ��ŵ�������ַ���ָ����������У���Щ�����
 * ���ͱ���Ϊ��char*����
 * @author lugz
 * @param src	��ʾ��Ҫ��ֵ�Դ����Դ���е��ַ�����ָ���ĵ����ַ�separateChar��
 *              ����������ʾ�������߷ָ��Ĵ���
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param ptrs		��ŵ����顣
 * @return ����һԴ��src�а���������������󷵻�-1��
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
 *     ���ַ����е�ֵ����ָ���ķָ������ֱ��ŵ�������ַ�����������У���Щ�����
 * ���ͱ���Ϊ��char*����
 * @author lugz
 * @param src	��ʾ��Ҫ��ֵ�Դ����Դ���е��ַ�����ָ���ĵ����ַ�separateChar��
 *              ����������ʾ�������߷ָ��Ĵ���
 *                  "field1|field2|field3| ... fieldn|"
 * @param separateChar
 * @param dest		��ŵ����顣
 * @param colLen	����ڶ�ά�ĳ���
 * @return ����һԴ��src�а������������
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
