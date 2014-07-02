#ifndef _DATA_ORDER_H_
#define _DATA_ORDER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	MAX_ITEM_SIZE		64

#define MAX_FIELD_LEN		512		/*�ֶ���󳤶�*/
#define MAX_FIELD_NUM		256		/*����ֶ���*/
#define MAX_VNAME_LEN		256		/*���󶨱�����*/
#define MAX_INAME_LEN		256		/*���ﶨ������*/
#define MAX_COLUMN_NUM		10240		/*��󻺴��ֶ���*/

#define	MAX_SQL_LEN		4096		/*���SQL����*/

#define	DATA_OBJECT_TYPE_MAIN		'0'	/*��������*/
#define	DATA_OBJECT_TYPE_CHANGE		'1'	/*���������*/
#define	DATA_OBJECT_TYPE_HISTORY	'2'	/*������ʷ��*/

#define	DATA_OPER_TYPE_INSERT		'1'	/*�������*/
#define	DATA_OPER_TYPE_DELETE		'2'	/*ɾ������*/
#define	DATA_OPER_TYPE_UPDATE		'3'	/*���²���*/
#define	DATA_OPER_TYPE_OTHER		'4'	/*��������*/

#define	DATA_DEAL_TYPE_NULL		0	/*������*/
#define	DATA_DEAL_TYPE_IGNORE		1	/*�����ֶ�*/
#define	DATA_DEAL_TYPE_SQLFUNC1		2	/*SQL����*/
#define	DATA_DEAL_TYPE_SQLFUNC2		3	/*ȡĬ��ֵ*/
#define DATA_DEAL_TYPE_ORDERDATA	4	/*ȡ���ݹ�������ֶ�ֵ*/
#define DATA_DEAL_TYPE_CONVDATA		5	/*ȡ���ݹ�����������ֶ�ֵ*/

#define	DATA_DEAL_FLAG_TRUE		'1'	/*��Ҫ������ر�*/
#define	DATA_DEAL_FLAG_FALSE		'0'	/*����Ҫ������ر�*/

#define	DATA_DEAL_ERROR_CHECK		1000	/*������Ϣ���δͨ��*/
#define	DATA_DEAL_ERROR_XML		2000	/*����XML��*/
#define	DATA_DEAL_ERROR_INSERT		3000	/*INSERT����*/
#define	DATA_DEAL_ERROR_DELETE		4000	/*DELETE����*/
#define	DATA_DEAL_ERROR_UPDATE		5000	/*UPDATE����*/
#define	DATA_DEAL_ERROR_NOTSUPP		6000	/*��֧�ֵĲ�������*/

#define	DATA_DEAL_ERROR_MAIN		100	/*�����������*/
#define	DATA_DEAL_ERROR_CHANGE		200	/*������������*/

#define	DATA_DATA_TYPE_STRING		0	/*�ַ���*/
#define	DATA_DATA_TYPE_NUMBER		1	/*����*/
#define DATA_DATA_TYPE_DATE		2	/*ʱ���ַ���*/

#define	DATA_ORDER_DATE_FORMAT		"YYYYMMDD HH24:MI:SS"		/*���ݹ�����ʱ���ַ�����ʽ*/

#define	CONFIG_RELOAD_INTERVAL		300	/*�����������³�ʼ����С���*/


/*���ݹ��� �����ֶ���Ϣ*/
typedef	struct	{
	char	strTableName[MAX_ITEM_SIZE];		/*���ݹ�����Я����TableName*/
	char	strObjectType[MAX_ITEM_SIZE];		/*���⴦���������|0������1�������2����ʷ��|*/
	char	strOperType[MAX_ITEM_SIZE];		/*�������ͣ���Ӧ�ӿڱ���OP_TYPE*/
	char	strColumnName[MAX_ITEM_SIZE];		/*��Ҫ���⴦����ֶ���*/
	int	iDealType;				/*���⴦������*/
	int	iDataFormat;				/*���ݸ�ʽ��ͬ�����Զ������ݸ�ʽ*/
	char	strFieldSql[MAX_ITEM_SIZE];		/*���⴦���ֶ�������SQL*/
	char	strDealParameter[MAX_ITEM_SIZE];	/*���⴦�������Ϣ*/
	
} SpecialColumnItemType;

typedef	struct	{
	SpecialColumnItemType	*pSpecialData;
	int	iCount;
} SpecialColumnListType;



/*���ݹ��� ��ӳ���ϵ��Ϣ*/
typedef	struct	{
	char	strTableName[MAX_ITEM_SIZE];		/*���ݹ�����Я����TableName*/
	
	char	strMainOwner[MAX_ITEM_SIZE];		/*��������*/
	char	strMainTableName[MAX_ITEM_SIZE];	/*�������*/
	
	char	strChgFlag[MAX_ITEM_SIZE];		/*�Ƿ�������*/
	
	char	strChgOwner[MAX_ITEM_SIZE];		/*���������*/
	char	strChgTableName[MAX_ITEM_SIZE];		/*��������*/
	
	char	strHisFlag[MAX_ITEM_SIZE];		/*�Ƿ�����ʷ��*/
	
	char	strHisOwner[MAX_ITEM_SIZE];		/*��ʷ������*/
	char	strHisTableName[MAX_ITEM_SIZE];		/*��ʷ�����*/
	
	int	iSpecialIdx;				/*���⴦������*/
	
	int	iColumnIdx[3];				/*������ֶ�����*/
	int	iColumnCnt[3];				/*������ֶ�����*/
} TableMapItemType;

typedef	struct	{
	TableMapItemType	*pTableMapData;
	int	iCount;
} TableMapListType;


/*�����ֶ���Ϣ*/
typedef struct	{
	char	strObjectType[MAX_ITEM_SIZE];		/*��������|0������1�������2����ʷ��|*/
	char	strOwner[MAX_ITEM_SIZE];		/*������*/
	char	strTableName[MAX_ITEM_SIZE];		/*����*/
	char	strColumnName[MAX_ITEM_SIZE];		/*�ֶ���*/
	char	strNullAble[MAX_ITEM_SIZE];		/*�Ƿ��Ϊ��*/
	char	chMainFlag;				/*�����Ƿ���ڴ��ֶα�־*/
} TableColumnItemType;

typedef	struct	{
	TableColumnItemType	*pColumnData;
	int	iCount;
} TableColumnListType;


/*���ݹ���������Ϣ*/
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

/*����SQL��������*/
typedef struct {
	char	strFieldSql[MAX_ITEM_SIZE];
	char	strFieldName[MAX_ITEM_SIZE];		/*�ֶ���*/
	char	strValue[MAX_FIELD_LEN];		/*�ֶ�ֵ���ַ�������*/
	long	lValue;					/*�ֶ�ֵ����������*/
	int	iDataType;				/*��������*/
	int	iDataFormat;				/*���ݸ�ʽ��ͬ�����Զ������ݸ�ʽ*/
	int	iLen;					/*�ֶγ���*/
} SqlFieldType;

typedef	struct {
	SqlFieldType	stFields[MAX_FIELD_NUM];	/*�ֶ�*/
	int		iCount;				/*�ֶ���*/
	char		strSql[MAX_SQL_LEN];		/*����SQL*/
} DataOrderSqlType;

int initDataOrderConfig();
int dealDataOrder(OrderTable *, MsgBodyType *, char *);


#ifdef __cplusplus
};
#endif

#endif
