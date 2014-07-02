#ifndef __CLString_h__
#define __CLString_h__

class CLString : public CLBase
{
public:
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
	static int StrToFields(const char* src, char separateChar, ...);

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
	static int StrToPtrs(char *src,const char *separateStr,char **ptrs);

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
	static int StrToRows(char *src,char separateChar,char **dest, int colLen);

	//去空格
	static char* Trim(char *S);
	
	//左去空格
	static char* lTrim(char *S);
	
	//右去空格
	static char* rTrim(char *S);
};
#endif//__CLString_h__
