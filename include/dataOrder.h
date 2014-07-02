#ifndef _DATA_ORDER_H_
#define _DATA_ORDER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	MAX_ITEM_SIZE		64

#define MAX_FIELD_LEN		512		/*字段最大长度*/
#define MAX_FIELD_NUM		256		/*最大字段数*/
#define MAX_VNAME_LEN		256		/*最大绑定变量数*/
#define MAX_INAME_LEN		256		/*最大帮定变量数*/
#define MAX_COLUMN_NUM		10240		/*最大缓存字段数*/

#define	MAX_SQL_LEN		4096		/*最大SQL长度*/

#define	DATA_OBJECT_TYPE_MAIN		'0'	/*操作主表*/
#define	DATA_OBJECT_TYPE_CHANGE		'1'	/*操作变更表*/
#define	DATA_OBJECT_TYPE_HISTORY	'2'	/*操作历史表*/

#define	DATA_OPER_TYPE_INSERT		'1'	/*插入操作*/
#define	DATA_OPER_TYPE_DELETE		'2'	/*删除操作*/
#define	DATA_OPER_TYPE_UPDATE		'3'	/*更新操作*/
#define	DATA_OPER_TYPE_OTHER		'4'	/*其他操作*/

#define	DATA_DEAL_TYPE_NULL		0	/*不处理*/
#define	DATA_DEAL_TYPE_IGNORE		1	/*忽略字段*/
#define	DATA_DEAL_TYPE_SQLFUNC1		2	/*SQL函数*/
#define	DATA_DEAL_TYPE_SQLFUNC2		3	/*取默认值*/
#define DATA_DEAL_TYPE_ORDERDATA	4	/*取数据工单相关字段值*/
#define DATA_DEAL_TYPE_CONVDATA		5	/*取数据工单内容相关字段值*/

#define	DATA_DEAL_FLAG_TRUE		'1'	/*需要操作相关表*/
#define	DATA_DEAL_FLAG_FALSE		'0'	/*不需要操作相关表*/

#define	DATA_DEAL_ERROR_CHECK		1000	/*基础信息检查未通过*/
#define	DATA_DEAL_ERROR_XML		2000	/*解析XML错*/
#define	DATA_DEAL_ERROR_INSERT		3000	/*INSERT报错*/
#define	DATA_DEAL_ERROR_DELETE		4000	/*DELETE报错*/
#define	DATA_DEAL_ERROR_UPDATE		5000	/*UPDATE报错*/
#define	DATA_DEAL_ERROR_NOTSUPP		6000	/*不支持的操作类型*/

#define	DATA_DEAL_ERROR_MAIN		100	/*主表操作报错*/
#define	DATA_DEAL_ERROR_CHANGE		200	/*变更表操作报错*/

#define	DATA_DATA_TYPE_STRING		0	/*字符串*/
#define	DATA_DATA_TYPE_NUMBER		1	/*数字*/
#define DATA_DATA_TYPE_DATE		2	/*时间字符串*/

#define	DATA_ORDER_DATE_FORMAT		"YYYYMMDD HH24:MI:SS"		/*数据工单中时间字符串样式*/

#define	CONFIG_RELOAD_INTERVAL		300	/*配置数据重新初始化最小间隔*/


/*数据工单 特殊字段信息*/
typedef	struct	{
	char	strTableName[MAX_ITEM_SIZE];		/*数据工单中携带的TableName*/
	char	strObjectType[MAX_ITEM_SIZE];		/*特殊处理对象类型|0：主表；1：变更表；2：历史表|*/
	char	strOperType[MAX_ITEM_SIZE];		/*处理类型，对应接口表中OP_TYPE*/
	char	strColumnName[MAX_ITEM_SIZE];		/*需要特殊处理的字段名*/
	int	iDealType;				/*特殊处理类型*/
	int	iDataFormat;				/*数据格式，同工单自定义数据格式*/
	char	strFieldSql[MAX_ITEM_SIZE];		/*特殊处理字段名所需SQL*/
	char	strDealParameter[MAX_ITEM_SIZE];	/*特殊处理参数信息*/
	
} SpecialColumnItemType;

typedef	struct	{
	SpecialColumnItemType	*pSpecialData;
	int	iCount;
} SpecialColumnListType;



/*数据工单 表映射关系信息*/
typedef	struct	{
	char	strTableName[MAX_ITEM_SIZE];		/*数据工单中携带的TableName*/
	
	char	strMainOwner[MAX_ITEM_SIZE];		/*主表属主*/
	char	strMainTableName[MAX_ITEM_SIZE];	/*主表表名*/
	
	char	strChgFlag[MAX_ITEM_SIZE];		/*是否处理变更表*/
	
	char	strChgOwner[MAX_ITEM_SIZE];		/*变更表属主*/
	char	strChgTableName[MAX_ITEM_SIZE];		/*变更表表名*/
	
	char	strHisFlag[MAX_ITEM_SIZE];		/*是否处理历史表*/
	
	char	strHisOwner[MAX_ITEM_SIZE];		/*历史表属主*/
	char	strHisTableName[MAX_ITEM_SIZE];		/*历史表表名*/
	
	int	iSpecialIdx;				/*特殊处理索引*/
	
	int	iColumnIdx[3];				/*缓存表字段索引*/
	int	iColumnCnt[3];				/*缓存表字段数量*/
} TableMapItemType;

typedef	struct	{
	TableMapItemType	*pTableMapData;
	int	iCount;
} TableMapListType;


/*缓存字段信息*/
typedef struct	{
	char	strObjectType[MAX_ITEM_SIZE];		/*对象类型|0：主表；1：变更表；2：历史表|*/
	char	strOwner[MAX_ITEM_SIZE];		/*表属主*/
	char	strTableName[MAX_ITEM_SIZE];		/*表名*/
	char	strColumnName[MAX_ITEM_SIZE];		/*字段名*/
	char	strNullAble[MAX_ITEM_SIZE];		/*是否可为空*/
	char	chMainFlag;				/*主表是否存在此字段标志*/
} TableColumnItemType;

typedef	struct	{
	TableColumnItemType	*pColumnData;
	int	iCount;
} TableColumnListType;


/*数据工单配置信息*/
typedef	struct	{
	SpecialColumnListType	stSpecialList;
	TableMapListType	stTableMapList;
	TableColumnListType	stColumnList;
	
	time_t	load_time;

	char	chInitFlag;
} DataOrderConfigType;

DataOrderConfigType	gDataOrderConfig;
DataOrderConfigType	*pDataOrderConfig;

char	gSpecialColumnTable[MAX_ITEM_SIZE];
char	gTableMapTable[MAX_ITEM_SIZE];

/*操作SQL及数据区*/
typedef struct {
	char	strFieldSql[MAX_ITEM_SIZE];
	char	strFieldName[MAX_ITEM_SIZE];		/*字段名*/
	char	strValue[MAX_FIELD_LEN];		/*字段值：字符串类型*/
	long	lValue;					/*字段值：数字类型*/
	int	iDataType;				/*数据类型*/
	int	iDataFormat;				/*数据格式，同工单自定义数据格式*/
	int	iLen;					/*字段长度*/
} SqlFieldType;

typedef	struct {
	SqlFieldType	stFields[MAX_FIELD_NUM];	/*字段*/
	int		iCount;				/*字段数*/
	char		strSql[MAX_SQL_LEN];		/*操作SQL*/
} DataOrderSqlType;

int initDataOrderConfig();
int dealDataOrder(OrderTable *, MsgBodyType *, char *);


#ifdef __cplusplus
};
#endif

#endif
