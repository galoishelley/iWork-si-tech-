#ifndef __unittype_h__
#define __unittype_h__

#ifndef NULL
#define NULL 0
#endif

#define U_EMPTY 0
#define U_CHAR 1
#define U_INT 2
#define U_LONG 3
#define U_DOUBLE 4
#define U_STRING 5
#define U_ARRAY 6
#define U_STRUCT 7

#define U_ERR_NOMEM 1
#define U_ERR_NOOCC 2
#define U_ERR_WRONGTYPE 3
#define U_ERR_EMPTY 4

extern int u_errno;

typedef struct tagutype utype;
typedef struct tague{
	int type;
	union {
		char v_char;	
		int v_int;		
		long v_long;	
		double v_double;	
		char * p_char;	
		utype * p_utype;	
	} value;
} ue;

struct tagutype{
	int cnt;
	int capacity;
	ue **ppe;
};

typedef struct tagCtrlInfo
{
	//错误代码，错误信息
	char	retCode[20+1];
	char	retMsg[255+1];

	//信息代码，信息内容
	char	infoCode[20+1];
	char	infoMsg[256];

	int  cursorCtrl;
} TCtrlInfo;

/**
 * 函数成功执行返回0
 */
typedef unsigned long  (*TFuncBusiness)(utype *uInParam,utype **uOutParam,TCtrlInfo *ctrlInfo);

/*不成功返回NULL*/
utype* uInit(int capacity);	/*创建统一数据结构， capacity 初始元素个数 */

/*不成功返回NULL*/
utype * uSetInt(utype *pu,int occ,int iv);	/*存int值，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

int uGetInt(utype *pu,int occ);	/*读int值，pu 统一数据结构指针，occ 元素位置，返回int值*/

/*不成功返回NULL*/
utype * uSetLong(utype *pu,int occ,long iv);	/*存long值，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

long uGetLong(utype *pu,int occ);	/*读long值，pu 统一数据结构指针，occ 元素位置，返回long值*/

/*不成功返回NULL*/
utype * uSetDouble(utype *pu,int occ,double iv);	/*存double值，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

double uGetDouble(utype *pu,int occ);	/*读long值，pu 统一数据结构指针，occ 元素位置，返回long值*/

/*不成功返回NULL*/
utype * uSetStr(utype *pu,int occ,const char * iv);	/*存字符串，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

char * uGetStr(utype *pu,int occ);	/*读字符串，pu 统一数据结构指针，occ 元素位置，返回字符串*/

/*不成功返回NULL*/
utype * uSetArray(utype *pu,int occ,utype * iv);	/*存数组，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

/*不成功返回NULL*/
utype * uGetArray(utype *pu,int occ);	/*读数组，pu 统一数据结构指针，occ 元素位置，返回数组（统一数据结构指针）*/

/*不成功返回NULL*/
utype * uSetStruct(utype *pu,int occ,utype * iv);	/*存结构，pu 统一数据结构指针，occ 元素位置，iv 输入值*/

utype* uGetStruct(utype *pu,int occ);	/*读结构，pu 统一数据结构指针，occ 元素位置，返回结构（统一数据结构指针）*/

/*不成功返回NULL*/
utype* uAlloc(utype *pu,int occ);
void uFree(utype *pu);
void ueFree(ue *pue);

#endif/*__unittype_h__*/

