#ifndef __PUBLIC_SHM_ROUTE_H__
#define __PUBLIC_SHM_ROUTE_H__


/**
 *     共享内存结构
 */
typedef struct tagMemRoot
{
	/*共享内存的大小。在创建路由数据的共享内存时，生成此值*/
	int iMemSize;
	long lMemUsed;	/*已经使用的共享内存大小*/
	long lMemLeft;	/*还剩余的共享内存大小*/
	void* pMem;	/*共享内存指针头*/
}TMemRoot;

typedef struct tagHashBucket THashBucket, *LPHashBucket;
typedef struct tagHashTable THashTable, *LPHashTable;

struct tagHashBucket
{
    unsigned long h;
    unsigned int key_length;
    void *data;
    LPHashBucket next;
    LPHashBucket previous;
    LPHashBucket conflict_next;
    LPHashBucket conflict_previous;
    char key[1];
};

typedef unsigned long (*THashFunc)(char *, unsigned int);

struct tagHashTable
{
    unsigned int table_size;
    unsigned int size_index;
    unsigned int elements;
    LPHashBucket p;
    LPHashBucket head;
    LPHashBucket tail;
    LPHashBucket *buckets;
};

extern int HashCreate(LPHashTable pht, unsigned int size, THashFunc hash, TMemRoot *ptMemRoot);
extern int HashEntry(LPHashTable pht, char *key, unsigned int key_length, void *data, TMemRoot *ptMemRoot);
extern int HashFind(LPHashTable, char *, unsigned int, void **);
extern int HashDestroy(LPHashTable pht, TMemRoot *ptMemRoot);


/**
 *     配置文件名，在$HOME/run/cfg目录下存放。
 */
#define ROUTE_DATA_FILE	"RouteData.cfg"

/*
 *     本地的分区代码
 */
#define LOCAL_PARTITION_CODE "LOCAL_PARTITION_CODE"

/*
 *     是否使用共享内存分发服务。取值范围：0不分发服务，1分发服务
 */
#define IS_DISPATCH_SERVICE "IS_DISPATCH_SERVICE"

/*
 * 分区的数据库数量
 */
#define PARTITION_DB_NUM	2

#define TRANSACTION_FLAG_COMMIT		1
#define TRANSACTION_FLAG_ROLLBACK	2

/**
 *     
 */
#define SHM_ROUTE_KEY 8

#define ROUTE_KEY_VALUE_LENGTH	15

#define ROUTE_WEB_SUFFIX_LENGTH	2

/*路由分区代码。*/
#define ROUTE_PARTITION_CODE_10			10
#define ROUTE_PARTITION_CODE_11			11
#define ROUTE_PARTITION_CODE_20			20
#define ROUTE_PARTITION_CODE_21			21
#define ROUTE_PARTITION_CODE_INVALID	99999

#define ROUTE_WEB_SUFFIX_A			"A"

/**
 *     路由数据结构
 */
typedef struct tagRouteDataShm
{
	char sKeyValue[ROUTE_KEY_VALUE_LENGTH+1];	/*路由键值*/
	char sKeyValue2[ROUTE_KEY_VALUE_LENGTH+1];	/*路由键值2，非散列结构使用此值*/
	int	 iPartitionCode;		/*分区代码*/
	char sWebSuffix[ROUTE_WEB_SUFFIX_LENGTH + 1];			/*WEB服务后缀*/
}TRouteDataShm;

/*路由代码。取值：10手机号码， 11用户ID编码, 12账户ID编码, 13客户ID编码, 14工号编码, 15地区代码*/
#define ROUTE_ROUTE_CODE_PHONE		10
#define ROUTE_ROUTE_CODE_USERID		11
#define ROUTE_ROUTE_CODE_CONID		12
#define ROUTE_ROUTE_CODE_CUSTID		13
#define ROUTE_ROUTE_CODE_LOGINNO	14
#define ROUTE_ROUTE_CODE_REGION		15

/*路由规则代码。取值：10范围， 11散列*/
#define ROUTE_RULE_CODE_RANGE	10
#define ROUTE_RULE_CODE_HASH	11

/*路由数据类型。取值：10字符串，11整型值。*/
#define ROUTE_RULE_DATA_TYPE_STRING		10
#define ROUTE_RULE_DATA_TYPE_INTEGER	11

/**
 *     路由规则结构
 */
typedef struct tagRouteRuleShm
{
	int	iRouteCode;			/*路由规则代码*/
	int iRuleCode;			/*路由规则代码。取值：10范围， 11散列*/
	int iRuleDataType;		/*路由数据类型。取值：10字符串，11整型值。*/
	int iGroupStrLength;	/*分组字符串的长度*/
	int iExtendLength;		/*动态扩展的记录个数。作为新增路由值的扩展使用。程序中无用，暂时预留。*/

	int iRouteDataCount;			/*路由数据数量*/
	TRouteDataShm *ptRouteDataShm;	/*路由数据数组*/

	THashTable tHashTable;	/*指向的hash表*/
}TRouteRuleShm;

/**
 *     路由数据共享内存接口
 */
typedef struct tagRouteShm
{
	TMemRoot tMemRoot;
	
	/*本地的分区代码，存放在配置文件ROUTE_DATA_FILE的LOCAL_PARTITION_CODE字段中*/
	int iLocalPartitionCode;

	/*是否使用共享内存分发服务。取值范围：0不分发服务，1分发服务*/
	int iIsDispatchService;

	int iRouteRuleCount;			/*路由规则数量*/
	TRouteRuleShm *ptRouteRuleShm;	/*路由规则数组*/
}TRouteShm;

/**
 *     所有的路由数据共享内存接口，包括备份接口和在用接口。
 */
typedef struct tagRouteShmAll
{
	int iRouteShmIndex;		/*路由数据共享内存接口的索引，表示在用的接口*/
	int iShmSize;			/*共享内存大小*/
	time_t tUpdateTime;		/*共享内存更新时间*/
	TRouteShm *ptRouteShms[2];/*路由数据共享内存指针，这两个内存互为备份。*/
}TRouteShmAll;

/**
 *    数据库连接是常连接。
 */
#define ROUTE_DB_CONNECT_LONG 1

/**
 *    数据库连接是短连接。
 */
#define ROUTE_DB_CONNECT_SHORT 0

/**
 *    路由宏的其他参数定义。此值是无符号长整型（unsigned long）。
 */
#define ROUTE_FLAGS_NONE 0x0
#define ROUTE_FLAGS_DYNAMIC_VALUE 0x1

/*回调函数定义，在tpforward之前调用。*/
typedef void (*TFuncCallBackHandler)(void*param);

/**
 * <summary>
 * 路由宏定义接口，在此宏中实现路由服务的调度。由于服务在调用tpforward函数
 * 之后，本地服务立即返回，并停止执行下面的语句，一般来说，DISPATCH_PARTITION宏
 * 应该放在变量定义之后，变量、内存等初始化之前。如果数据库是常连接，
 * DISPATCH_PARTITION应该是服务的第一条执行语句。如果数据库是短连接，
 * DISPATCH_PARTITION应该是紧跟数据库连接函数之后。
 * </summary>
 *<param name="psServiceName"> 	调用服务的名称。</param>
 *<param name="iRouteCode"> 	路由代码。
 *	ROUTE_ROUTE_CODE_PHONE		10手机号码
 *	ROUTE_ROUTE_CODE_USERID		11用户ID编码
 *	ROUTE_ROUTE_CODE_CONID		12账户ID编码
 *	ROUTE_ROUTE_CODE_CUSTID		13客户ID编码
 *	ROUTE_ROUTE_CODE_LOGINNO	14工号编码
 *  ROUTE_ROUTE_CODE_REGION		15地区代码
 *</param>
 *<param name="psRouteValuePos">
 *  当iRouteFlags的值包含ROUTE_FLAGS_DYNAMIC_VALUE时：
 *     psRouteValuePos表示路由的值，必须为字符串(const char*)。
 *  当iRouteFlags的值包不含ROUTE_FLAGS_DYNAMIC_VALUE时：
 *     psRouteValuePos表示路由数据在服务中参数的位置。在GPARM32_0和GPARM32_49之间。
 *</param>
 *<param name="transb"> 	调用服务参数结构体（TPSVCINFO*）。</param>
 *<param name="psDbLabel"> 	数据库连接的名称。如：LABELDBCHANGE。</param>
 *<param name="iIsDbConnect">
 *  数据库连接是否是常连接。ROUTE_DB_CONNECT_LONG和ROUTE_DB_CONNECT_SHORT。
 *</param>
 *<param name="iRouteFlags">
 *  路由宏的其他参数定义，此值是无符号长整型，参数取值范围如下：
 *  	ROUTE_FLAGS_NONE：无定义。
 *  	ROUTE_FLAGS_DYNAMIC_VALUE：动态值，路由值由调用者动态指定。
 *</param>
 *<param name="ptFuncCallBackHandler">回调函数定义，在tpforward之前调用，本函数无返回值。</param>
 *<param name="FuncCallBackParam">回调函数的输入参数。</param>
 */
int DispatchPartition(const char *psServiceName, int iRouteCode, const char* psRouteValuePos,
	TPSVCINFO *transb,const char* psDbLabel,int iIsDbConnect, unsigned long iRouteFlags,
	TFuncCallBackHandler ptFuncCallBackHandler, void*FuncCallBackParam);
#define DISPATCH_PARTITION(psServiceName, iRouteCode, psRouteValuePos, transb, psDbLabel,iIsDbConnect, iRouteFlags) \
	if ((DispatchPartition(psServiceName, iRouteCode, (const char*)psRouteValuePos, transb,\
			psDbLabel,iIsDbConnect, iRouteFlags, NULL, NULL)) != 0) \
	{ \
		FBFR32 *ptTransOut = NULL; \
		char sBuff[128];\
		\
		memset(sBuff, 0, sizeof(sBuff));\
		if (iIsDbConnect==ROUTE_DB_CONNECT_SHORT)\
		{\
			spublicDBClose(psDbLabel);\
		}\
		MemoryAllocate32(1,3,psServiceName, (FBFR32 *)transb->data,&ptTransOut,psDbLabel); \
		if(iRouteFlags == ROUTE_FLAGS_DYNAMIC_VALUE) \
		{\
			sprintf(sBuff, "传入的数据[%s]无效!", psRouteValuePos);\
		}\
		else\
		{\
			sprintf(sBuff, "传入的数据无效,参数位置[%d]!", (int)(psRouteValuePos-GPARM32_0));\
		}\
		Fchg32(ptTransOut,SVC_ERR_NO32,0,"1000",(FLDLEN32)0); \
		Fchg32(ptTransOut,SVC_ERR_MSG32,0,sBuff,(FLDLEN32)0); \
		tpreturn(TPSUCCESS,0,(char *)ptTransOut,0L,0); \
	} 
extern int GetRouteIdNo(const char* psPhoneNo, char* psRouteIdNo, int *piPartitionCode);

/* 工单参数信息*/

/*工单表dWipOrder XML设定的参数格式标签定义*/
#define PARAM_ROOT_TAG				"params"
#define PARAM_TAG					"param"
#define PARAM_ORDER_TAG				"order"
#define PARAM_VALUE_TAG				"value"
#define PARAM_VALUE_ORDER_TAG		"order"

#define WIP_ORDER_PRECEDURE_TYPE_TUXEDO		0	/*调用例程类型.0:tuxedo*/
#define WIP_ORDER_PRECEDURE_TYPE_FUNCTION	1	/*调用例程类型.1:function*/

#define WIP_ORDER_PARAM_FORMAT_XML			1	/*调用例程参数格式。1:XML格式;*/
#define WIP_ORDER_PARAM_FORMAT_SEPARATOR	2	/*调用例程参数格式。2:参数分割格式,字段用'\r'分割,记录用'\n'分割。*/

#define WIP_ORDER_PARAM_SEPARATOR_RECORD	'\r'	/*调用例程参数格式。2:参数分割格式,记录用'\r'分割。*/
#define WIP_ORDER_PARAM_SEPARATOR_FIELD		'\n'	/*调用例程参数格式。2:参数分割格式,字段用'\n'分割*/

typedef struct tagParamValue
{
	char *psValue;
	int iValueLen;	/*不包含空字符'\0'*/
	struct tagParamValue *ptNext;
}TParamValue;

typedef struct tagTuxInParam
{
	TParamValue *ptParamValueHead;
	unsigned int iParamOrder;
	struct tagTuxInParam *ptNext;
}TTuxInParam;

typedef struct tagTuxInParams
{
	struct tagTuxInParam *ptTuxInParaHead;
}TTuxInParams;

int AddTuxParam(TTuxInParams *ptTuxInParams, char* psValue, unsigned int iParamOrder);
int AddTuxParams(TTuxInParams *ptTuxInParams, char* psValueFirst, ...);
void ClearTuxParam(TTuxInParams *ptTuxInParams, int iIsPrintValue);
int GetDbNoByRouteValue(int iRouteCode, char *psRouteValue,int *piDbNo);
int GetPartitionCodeByRouteValue(int iRouteCode, char *psRouteValue,int *piPartitionCode);

#define WIP_ORDER_PARAM_LEN	4000

typedef struct tagWipOrder
{
	long lBusiOrder;			/*业务订单标识*/
	long lWipOrderId;			/*业务工单标识*/
	long lSequenceId;			/*执行顺序*/
	int  iPartitionCode;		/*分区代码*/
	int  iWipType;				/*工单类型。1普通。*/
	char sLimitTime[17+1];		/*工单时限，格式'YYYYMMDD HH24:MI:SS'*/
	char sCreateTime[17+1];		/*工单生成时间，格式'YYYYMMDD HH24:MI:SS'*/
	char sCompleteTime[17+1];	/*工单完成时间，格式'YYYYMMDD HH24:MI:SS'*/
	int  iWipStatus;			/*工单状态。0待处理,1完成, 2退单。*/
	char sOpCode[4+1];			/*操作代码*/
	int  iPrecedureType;		/*调用例程类型.0:tuxedo,1:function*/
	char sPrecedureName[31+1];	/*调用例程名称*/
	int  iParamFormat;			/*调用例程参数格式。1:XML格式;2:参数分割格式,字段用'\r'分割,记录用'\n'分割。*/
	char sParam1[WIP_ORDER_PARAM_LEN+1];		/*调用例程参数1*/
	char sParam2[WIP_ORDER_PARAM_LEN+1];		/*调用例程参数2*/
} TWipOrder;
int InsWipOrder(TWipOrder *ptWipOrder, TTuxInParams *ptTuxInParams,
		char* psRetCode, char* psRetMsg);
#endif /*__PUBLIC_SHM_ROUTE_H__*/
