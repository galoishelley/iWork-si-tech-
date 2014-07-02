#ifndef __PUBLIC_SHM_ROUTE_H__
#define __PUBLIC_SHM_ROUTE_H__


/**
 *     �����ڴ�ṹ
 */
typedef struct tagMemRoot
{
	/*�����ڴ�Ĵ�С���ڴ���·�����ݵĹ����ڴ�ʱ�����ɴ�ֵ*/
	int iMemSize;
	long lMemUsed;	/*�Ѿ�ʹ�õĹ����ڴ��С*/
	long lMemLeft;	/*��ʣ��Ĺ����ڴ��С*/
	void* pMem;	/*�����ڴ�ָ��ͷ*/
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
 *     �����ļ�������$HOME/run/cfgĿ¼�´�š�
 */
#define ROUTE_DATA_FILE	"RouteData.cfg"

/*
 *     ���صķ�������
 */
#define LOCAL_PARTITION_CODE "LOCAL_PARTITION_CODE"

/*
 *     �Ƿ�ʹ�ù����ڴ�ַ�����ȡֵ��Χ��0���ַ�����1�ַ�����
 */
#define IS_DISPATCH_SERVICE "IS_DISPATCH_SERVICE"

/*
 * ���������ݿ�����
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

/*·�ɷ������롣*/
#define ROUTE_PARTITION_CODE_10			10
#define ROUTE_PARTITION_CODE_11			11
#define ROUTE_PARTITION_CODE_20			20
#define ROUTE_PARTITION_CODE_21			21
#define ROUTE_PARTITION_CODE_INVALID	99999

#define ROUTE_WEB_SUFFIX_A			"A"

/**
 *     ·�����ݽṹ
 */
typedef struct tagRouteDataShm
{
	char sKeyValue[ROUTE_KEY_VALUE_LENGTH+1];	/*·�ɼ�ֵ*/
	char sKeyValue2[ROUTE_KEY_VALUE_LENGTH+1];	/*·�ɼ�ֵ2����ɢ�нṹʹ�ô�ֵ*/
	int	 iPartitionCode;		/*��������*/
	char sWebSuffix[ROUTE_WEB_SUFFIX_LENGTH + 1];			/*WEB�����׺*/
}TRouteDataShm;

/*·�ɴ��롣ȡֵ��10�ֻ����룬 11�û�ID����, 12�˻�ID����, 13�ͻ�ID����, 14���ű���, 15��������*/
#define ROUTE_ROUTE_CODE_PHONE		10
#define ROUTE_ROUTE_CODE_USERID		11
#define ROUTE_ROUTE_CODE_CONID		12
#define ROUTE_ROUTE_CODE_CUSTID		13
#define ROUTE_ROUTE_CODE_LOGINNO	14
#define ROUTE_ROUTE_CODE_REGION		15

/*·�ɹ�����롣ȡֵ��10��Χ�� 11ɢ��*/
#define ROUTE_RULE_CODE_RANGE	10
#define ROUTE_RULE_CODE_HASH	11

/*·���������͡�ȡֵ��10�ַ�����11����ֵ��*/
#define ROUTE_RULE_DATA_TYPE_STRING		10
#define ROUTE_RULE_DATA_TYPE_INTEGER	11

/**
 *     ·�ɹ���ṹ
 */
typedef struct tagRouteRuleShm
{
	int	iRouteCode;			/*·�ɹ������*/
	int iRuleCode;			/*·�ɹ�����롣ȡֵ��10��Χ�� 11ɢ��*/
	int iRuleDataType;		/*·���������͡�ȡֵ��10�ַ�����11����ֵ��*/
	int iGroupStrLength;	/*�����ַ����ĳ���*/
	int iExtendLength;		/*��̬��չ�ļ�¼��������Ϊ����·��ֵ����չʹ�á����������ã���ʱԤ����*/

	int iRouteDataCount;			/*·����������*/
	TRouteDataShm *ptRouteDataShm;	/*·����������*/

	THashTable tHashTable;	/*ָ���hash��*/
}TRouteRuleShm;

/**
 *     ·�����ݹ����ڴ�ӿ�
 */
typedef struct tagRouteShm
{
	TMemRoot tMemRoot;
	
	/*���صķ������룬����������ļ�ROUTE_DATA_FILE��LOCAL_PARTITION_CODE�ֶ���*/
	int iLocalPartitionCode;

	/*�Ƿ�ʹ�ù����ڴ�ַ�����ȡֵ��Χ��0���ַ�����1�ַ�����*/
	int iIsDispatchService;

	int iRouteRuleCount;			/*·�ɹ�������*/
	TRouteRuleShm *ptRouteRuleShm;	/*·�ɹ�������*/
}TRouteShm;

/**
 *     ���е�·�����ݹ����ڴ�ӿڣ��������ݽӿں����ýӿڡ�
 */
typedef struct tagRouteShmAll
{
	int iRouteShmIndex;		/*·�����ݹ����ڴ�ӿڵ���������ʾ���õĽӿ�*/
	int iShmSize;			/*�����ڴ��С*/
	time_t tUpdateTime;		/*�����ڴ����ʱ��*/
	TRouteShm *ptRouteShms[2];/*·�����ݹ����ڴ�ָ�룬�������ڴ滥Ϊ���ݡ�*/
}TRouteShmAll;

/**
 *    ���ݿ������ǳ����ӡ�
 */
#define ROUTE_DB_CONNECT_LONG 1

/**
 *    ���ݿ������Ƕ����ӡ�
 */
#define ROUTE_DB_CONNECT_SHORT 0

/**
 *    ·�ɺ�������������塣��ֵ���޷��ų����ͣ�unsigned long����
 */
#define ROUTE_FLAGS_NONE 0x0
#define ROUTE_FLAGS_DYNAMIC_VALUE 0x1

/*�ص��������壬��tpforward֮ǰ���á�*/
typedef void (*TFuncCallBackHandler)(void*param);

/**
 * <summary>
 * ·�ɺ궨��ӿڣ��ڴ˺���ʵ��·�ɷ���ĵ��ȡ����ڷ����ڵ���tpforward����
 * ֮�󣬱��ط����������أ���ִֹͣ���������䣬һ����˵��DISPATCH_PARTITION��
 * Ӧ�÷��ڱ�������֮�󣬱������ڴ�ȳ�ʼ��֮ǰ��������ݿ��ǳ����ӣ�
 * DISPATCH_PARTITIONӦ���Ƿ���ĵ�һ��ִ����䡣������ݿ��Ƕ����ӣ�
 * DISPATCH_PARTITIONӦ���ǽ������ݿ����Ӻ���֮��
 * </summary>
 *<param name="psServiceName"> 	���÷�������ơ�</param>
 *<param name="iRouteCode"> 	·�ɴ��롣
 *	ROUTE_ROUTE_CODE_PHONE		10�ֻ�����
 *	ROUTE_ROUTE_CODE_USERID		11�û�ID����
 *	ROUTE_ROUTE_CODE_CONID		12�˻�ID����
 *	ROUTE_ROUTE_CODE_CUSTID		13�ͻ�ID����
 *	ROUTE_ROUTE_CODE_LOGINNO	14���ű���
 *  ROUTE_ROUTE_CODE_REGION		15��������
 *</param>
 *<param name="psRouteValuePos">
 *  ��iRouteFlags��ֵ����ROUTE_FLAGS_DYNAMIC_VALUEʱ��
 *     psRouteValuePos��ʾ·�ɵ�ֵ������Ϊ�ַ���(const char*)��
 *  ��iRouteFlags��ֵ������ROUTE_FLAGS_DYNAMIC_VALUEʱ��
 *     psRouteValuePos��ʾ·�������ڷ����в�����λ�á���GPARM32_0��GPARM32_49֮�䡣
 *</param>
 *<param name="transb"> 	���÷�������ṹ�壨TPSVCINFO*����</param>
 *<param name="psDbLabel"> 	���ݿ����ӵ����ơ��磺LABELDBCHANGE��</param>
 *<param name="iIsDbConnect">
 *  ���ݿ������Ƿ��ǳ����ӡ�ROUTE_DB_CONNECT_LONG��ROUTE_DB_CONNECT_SHORT��
 *</param>
 *<param name="iRouteFlags">
 *  ·�ɺ�������������壬��ֵ���޷��ų����ͣ�����ȡֵ��Χ���£�
 *  	ROUTE_FLAGS_NONE���޶��塣
 *  	ROUTE_FLAGS_DYNAMIC_VALUE����ֵ̬��·��ֵ�ɵ����߶�ָ̬����
 *</param>
 *<param name="ptFuncCallBackHandler">�ص��������壬��tpforward֮ǰ���ã��������޷���ֵ��</param>
 *<param name="FuncCallBackParam">�ص����������������</param>
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
			sprintf(sBuff, "���������[%s]��Ч!", psRouteValuePos);\
		}\
		else\
		{\
			sprintf(sBuff, "�����������Ч,����λ��[%d]!", (int)(psRouteValuePos-GPARM32_0));\
		}\
		Fchg32(ptTransOut,SVC_ERR_NO32,0,"1000",(FLDLEN32)0); \
		Fchg32(ptTransOut,SVC_ERR_MSG32,0,sBuff,(FLDLEN32)0); \
		tpreturn(TPSUCCESS,0,(char *)ptTransOut,0L,0); \
	} 
extern int GetRouteIdNo(const char* psPhoneNo, char* psRouteIdNo, int *piPartitionCode);

/* ����������Ϣ*/

/*������dWipOrder XML�趨�Ĳ�����ʽ��ǩ����*/
#define PARAM_ROOT_TAG				"params"
#define PARAM_TAG					"param"
#define PARAM_ORDER_TAG				"order"
#define PARAM_VALUE_TAG				"value"
#define PARAM_VALUE_ORDER_TAG		"order"

#define WIP_ORDER_PRECEDURE_TYPE_TUXEDO		0	/*������������.0:tuxedo*/
#define WIP_ORDER_PRECEDURE_TYPE_FUNCTION	1	/*������������.1:function*/

#define WIP_ORDER_PARAM_FORMAT_XML			1	/*�������̲�����ʽ��1:XML��ʽ;*/
#define WIP_ORDER_PARAM_FORMAT_SEPARATOR	2	/*�������̲�����ʽ��2:�����ָ��ʽ,�ֶ���'\r'�ָ�,��¼��'\n'�ָ*/

#define WIP_ORDER_PARAM_SEPARATOR_RECORD	'\r'	/*�������̲�����ʽ��2:�����ָ��ʽ,��¼��'\r'�ָ*/
#define WIP_ORDER_PARAM_SEPARATOR_FIELD		'\n'	/*�������̲�����ʽ��2:�����ָ��ʽ,�ֶ���'\n'�ָ�*/

typedef struct tagParamValue
{
	char *psValue;
	int iValueLen;	/*���������ַ�'\0'*/
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
	long lBusiOrder;			/*ҵ�񶩵���ʶ*/
	long lWipOrderId;			/*ҵ�񹤵���ʶ*/
	long lSequenceId;			/*ִ��˳��*/
	int  iPartitionCode;		/*��������*/
	int  iWipType;				/*�������͡�1��ͨ��*/
	char sLimitTime[17+1];		/*����ʱ�ޣ���ʽ'YYYYMMDD HH24:MI:SS'*/
	char sCreateTime[17+1];		/*��������ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'*/
	char sCompleteTime[17+1];	/*�������ʱ�䣬��ʽ'YYYYMMDD HH24:MI:SS'*/
	int  iWipStatus;			/*����״̬��0������,1���, 2�˵���*/
	char sOpCode[4+1];			/*��������*/
	int  iPrecedureType;		/*������������.0:tuxedo,1:function*/
	char sPrecedureName[31+1];	/*������������*/
	int  iParamFormat;			/*�������̲�����ʽ��1:XML��ʽ;2:�����ָ��ʽ,�ֶ���'\r'�ָ�,��¼��'\n'�ָ*/
	char sParam1[WIP_ORDER_PARAM_LEN+1];		/*�������̲���1*/
	char sParam2[WIP_ORDER_PARAM_LEN+1];		/*�������̲���2*/
} TWipOrder;
int InsWipOrder(TWipOrder *ptWipOrder, TTuxInParams *ptTuxInParams,
		char* psRetCode, char* psRetMsg);
#endif /*__PUBLIC_SHM_ROUTE_H__*/
