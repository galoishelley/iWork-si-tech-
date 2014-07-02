#ifndef __CLString_h__
#define __CLString_h__

class CLString : public CLBase
{
public:
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
	static int StrToFields(const char* src, char separateChar, ...);

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
	static int StrToPtrs(char *src,const char *separateStr,char **ptrs);

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
	static int StrToRows(char *src,char separateChar,char **dest, int colLen);

	//ȥ�ո�
	static char* Trim(char *S);
	
	//��ȥ�ո�
	static char* lTrim(char *S);
	
	//��ȥ�ո�
	static char* rTrim(char *S);
};
#endif//__CLString_h__
