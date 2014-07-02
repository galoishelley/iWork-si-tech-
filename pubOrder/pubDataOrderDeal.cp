#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/errno.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>

#include "order_xml.h"
#include "dataOrder.h"

EXEC SQL INCLUDE sqlca;
EXEC SQL INCLUDE oraca;
EXEC SQL INCLUDE sqlcpr;
EXEC ORACLE OPTION (ORACA=YES);

EXEC SQL INCLUDE sqlda;
SQLDA *main_dp;
SQLDA *change_dp;
SQLDA *history_dp;

char strProcTag[64];

/****************************************************************** 
Function:	strtrim
Description:	将字符串source的头、尾空格去掉后放入字符串target中
Input:		char* target     存放处理后串的字符数组指针
                const char* src  需要去空格的字符串
Return: 	int 有效字符串的长度
Others:		target 必须是已经分配了足够存储空间的字符数组
********************************************************************/
int strtrim(char* target,const char* source)
{
	int len;
	char *p_buffer;
	
	if(source == NULL || *source == '\0')
		return 0;
	
	p_buffer = (char *)source;
	
	while(*p_buffer == ' ')
		p_buffer++;

	strcpy(target, p_buffer);

	len = strlen(target);

	while(target[len-1] == ' ')
		len--;

	target[len] = '\0';
	
	return strlen(target);
}

/****************************************************************** 
Function:	countTable
Description:	计算表中记录数
Input:		char *pTablename 要统计的表名
                char *pWhereCond 统计的Where条件
                int *pCount 表中记录数
Return: 	0：成功，1：错误
Others:	
********************************************************************/
int countTable(char *pTableName, char *pWhereCond, int *pCount)
{
	char	strSql[1024] = {0}, strTmp[32] = {0};
	
	*pCount = 0;
	
	sprintf(strSql, "select count(*) from %s %s ", pTableName, pWhereCond);
	
	EXEC SQL PREPARE count_tab_s FROM :strSql;
	EXEC SQL DECLARE count_tab_c CURSOR FOR count_tab_s;
	EXEC SQL OPEN count_tab_c;
	EXEC SQL FETCH count_tab_c INTO :strTmp;
		
	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		return 1;
	}

	*pCount = atoi(strTmp);

	return 0;
}

/****************************************************************** 
Function:	initSpecialColumn
Description:	初始化SpecialColumn表
Input:		
Return: 	0：成功，1：错误
Others:	
********************************************************************/
int initSpecialColumn()
{
	int	i;
	char	strSql[1024] = {0};
	SpecialColumnItemType tmpSC;
	SpecialColumnListType *pSpecialList = &(gDataOrderConfig.stSpecialList);
	
	sprintf(strSql, "select table_name, object_type, oper_type, column_name, deal_type, data_format, field_sql, "
		" nvl(deal_parameter, ' ') from %s order by table_name, object_type, oper_type ", gSpecialColumnTable);

	pSpecialList->iCount = 0;
	
	/*计算表记录数*/
	if(countTable(gSpecialColumnTable, NULL, &(pSpecialList->iCount)))
		return 1;

	if(pSpecialList->iCount < 0)
		return 1;

	if(pSpecialList->iCount == 0)
		return 0;

	/*内存分配*/
	if(pSpecialList->pSpecialData != NULL)
	{
		free(pSpecialList->pSpecialData);
		pSpecialList->pSpecialData = NULL;
	}

	pSpecialList->pSpecialData = (SpecialColumnItemType *)calloc(pSpecialList->iCount, sizeof(SpecialColumnItemType));
	
	if(pSpecialList->pSpecialData == NULL)
	{
		errLog(strProcTag, "initSepcalColumn()==>calloc() error:%d:%s.\n", errno, strerror(errno));
		return 1;
	}

	EXEC SQL PREPARE special_col_s FROM :strSql;
	EXEC SQL DECLARE special_col_c CURSOR FOR special_col_s;
	EXEC SQL OPEN special_col_c;

	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		return 1;
	}

	/*读取数据库记录*/
	for(i = 0; i < pSpecialList->iCount; i++)
	{
		memset(&tmpSC, 0, sizeof(tmpSC));
		
		EXEC SQL FETCH special_col_c INTO :tmpSC.strTableName, :tmpSC.strObjectType, :tmpSC.strOperType, :tmpSC.strColumnName,
			:tmpSC.iDealType, :tmpSC.iDataFormat, :tmpSC.strFieldSql, :tmpSC.strDealParameter;

		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			return 1;
		}

		strtrim((pSpecialList->pSpecialData)[i].strTableName, tmpSC.strTableName);
		strtrim((pSpecialList->pSpecialData)[i].strObjectType, tmpSC.strObjectType);
		strtrim((pSpecialList->pSpecialData)[i].strOperType, tmpSC.strOperType);
		strtrim((pSpecialList->pSpecialData)[i].strColumnName, tmpSC.strColumnName);
		(pSpecialList->pSpecialData)[i].iDealType = tmpSC.iDealType;
		(pSpecialList->pSpecialData)[i].iDataFormat = tmpSC.iDataFormat;
		strtrim((pSpecialList->pSpecialData)[i].strFieldSql, tmpSC.strFieldSql);
		strtrim((pSpecialList->pSpecialData)[i].strDealParameter, tmpSC.strDealParameter);
	}
	
	EXEC SQL CLOSE special_col_c;
	
	return 0;
}

/****************************************************************** 
Function:	initTableColumn
Description:	初始化TableMap和all_tab_cols关联信息
Input:		
Return: 	0：成功，1：错误
Others:	
********************************************************************/
int initTableColumn()
{
	int	i;
	char	strSql[1024] = {0};
	TableColumnItemType tmpTC;
	TableColumnListType *pColumnList = &(gDataOrderConfig.stColumnList);
	
	sprintf(strSql,
		"select object_type, owner, table_name, column_name, nullable "
		"from ( "
		"select '0' object_type, owner, table_name, column_name, nullable "
		"from all_tab_cols "
		"where (owner,table_name) in (select distinct main_owner,main_table_name from %s) "
		"and column_id is not null "
		"union "
		"select '1' object_type, owner, table_name, column_name, nullable "
		"from all_tab_cols "
		"where (owner,table_name) in (select distinct nvl(chg_owner,'NULL'),nvl(chg_table_name,'NULL') from %s) "
		"and column_id is not null "
		"union "
		"select '2' object_type, owner, table_name, column_name, nullable "
		"from all_tab_cols "
		"where (owner,table_name) in (select distinct nvl(his_owner,'NULL'),nvl(his_table_name,'NULL') from %s) "
		"and column_id is not null ) "
		"order by object_type, owner, table_name ", 
		gTableMapTable, gTableMapTable, gTableMapTable);

	pColumnList->iCount = 0;

	/*固定分配内存*/
	if(pColumnList->pColumnData != NULL)
	{
		free(pColumnList->pColumnData);
		pColumnList->pColumnData = NULL;
	}

	pColumnList->pColumnData = (TableColumnItemType *) calloc(MAX_COLUMN_NUM, sizeof(TableColumnItemType));

	if(pColumnList->pColumnData == NULL)
	{
		errLog(strProcTag, "initTableColumn()==>calloc() error:%d:%s.\n", errno, strerror(errno));
		return 1;
	}

	EXEC SQL PREPARE table_col_s FROM :strSql;
	EXEC SQL DECLARE table_col_c CURSOR FOR table_col_s;
	EXEC SQL OPEN table_col_c;

	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		return 1;
	}

	/*读取数据库记录*/
	for(i = 0; ; i++)
	{
		if(i >= MAX_COLUMN_NUM)
		{
			errLog(strProcTag, "the define MAX_COLUMN_NUM(%d) is too small\n", MAX_COLUMN_NUM);
			return 1;
		}
		
		memset(&tmpTC, 0, sizeof(tmpTC));
		
		EXEC SQL FETCH table_col_c INTO :tmpTC.strObjectType, :tmpTC.strOwner, :tmpTC.strTableName,
			:tmpTC.strColumnName, :tmpTC.strNullAble;

		if(SQLCODE == NOTFOUND)
			break;

		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			return 1;
		}

		strtrim((pColumnList->pColumnData)[i].strObjectType, tmpTC.strObjectType);
		strtrim((pColumnList->pColumnData)[i].strOwner, tmpTC.strOwner);
		strtrim((pColumnList->pColumnData)[i].strTableName, tmpTC.strTableName);
		strtrim((pColumnList->pColumnData)[i].strColumnName, tmpTC.strColumnName);
		strtrim((pColumnList->pColumnData)[i].strNullAble, tmpTC.strNullAble);
		(pColumnList->pColumnData)[i].chMainFlag = 0;
	}
	
	pColumnList->iCount = i;
	
	EXEC SQL CLOSE table_col_c;
	
	return 0;
}

/****************************************************************** 
Function:	initTableMap
Description:	初始化TableMap表
Input:		
Return: 	0：成功，1：错误
Others:	
********************************************************************/
int initTableMap()
{
	int	i;
	char	strSql[1024] = {0};
	TableMapItemType tmpTM;
	TableMapListType *pTableMapList = &(gDataOrderConfig.stTableMapList);

	sprintf(strSql, "select table_name, main_owner, main_table_name, nvl(chg_flag, '0'), nvl(chg_owner, ' '), nvl(chg_table_name, ' '),"
		" nvl(his_flag, '0'), nvl(his_owner, ' '), nvl(his_table_name, ' ') from %s order by table_name ", gTableMapTable);

	pTableMapList->iCount = 0;
	
	/*统计表记录数*/
	if(countTable(gTableMapTable, NULL, &(pTableMapList->iCount)))
		return 1;

	if(pTableMapList->iCount < 0)
		return 1;

	if(pTableMapList->iCount == 0)
		return 0;

	/*分配内存*/
	if(pTableMapList->pTableMapData != NULL)
	{
		free(pTableMapList->pTableMapData);
		pTableMapList->pTableMapData = NULL;
	}

	pTableMapList->pTableMapData = (TableMapItemType *)calloc(pTableMapList->iCount, sizeof(TableMapItemType));
	
	if(pTableMapList->pTableMapData == NULL)
	{
		errLog(strProcTag, "initTableMap()==>calloc() error:%d:%s.\n", errno, strerror(errno));
		return 1;
	}

	EXEC SQL PREPARE table_map_s FROM :strSql;
	EXEC SQL DECLARE table_map_c CURSOR FOR table_map_s;
	EXEC SQL OPEN table_map_c;

	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		return 1;
	}

	/*读取数据库记录*/
	for(i = 0; i < pTableMapList->iCount; i++)
	{
		memset(&tmpTM, 0, sizeof(tmpTM));
		
		EXEC SQL FETCH table_map_c INTO :tmpTM.strTableName, :tmpTM.strMainOwner, :tmpTM.strMainTableName,
			:tmpTM.strChgFlag, :tmpTM.strChgOwner, :tmpTM.strChgTableName,
			:tmpTM.strHisFlag, :tmpTM.strHisOwner, :tmpTM.strHisTableName;

		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			return 1;
		}

		strtrim((pTableMapList->pTableMapData)[i].strTableName, tmpTM.strTableName);
		strtrim((pTableMapList->pTableMapData)[i].strMainOwner, tmpTM.strMainOwner);
		strtrim((pTableMapList->pTableMapData)[i].strMainTableName, tmpTM.strMainTableName);
		
		strtrim((pTableMapList->pTableMapData)[i].strChgFlag, tmpTM.strChgFlag);
		strtrim((pTableMapList->pTableMapData)[i].strChgOwner, tmpTM.strChgOwner);
		strtrim((pTableMapList->pTableMapData)[i].strChgTableName, tmpTM.strChgTableName);
		
		strtrim((pTableMapList->pTableMapData)[i].strHisFlag, tmpTM.strHisFlag);
		strtrim((pTableMapList->pTableMapData)[i].strHisOwner, tmpTM.strHisOwner);
		strtrim((pTableMapList->pTableMapData)[i].strHisTableName, tmpTM.strHisTableName);
	}
	
	EXEC SQL CLOSE table_col_c;
	
	return 0;
}

/****************************************************************** 
Function:	relateSpecialIndex
Description:	计算TableMap和SpecialColumn关联索引
Input:		
Return: 
Others:	
********************************************************************/
void relateSpecialIndex()
{
	int i, j, iFound;
	SpecialColumnListType *pSpecialList = &(gDataOrderConfig.stSpecialList);
	TableMapListType *pTableMapList = &(gDataOrderConfig.stTableMapList);
	
	if(pSpecialList->iCount == 0 || pTableMapList->iCount == 0)
		return;

	for(i = 0; i < pTableMapList->iCount; i++)
	{
		for(j = 0, iFound = 0; j < pSpecialList->iCount; j++)
		{
			if(strcasecmp((pTableMapList->pTableMapData)[i].strTableName, 
				(pSpecialList->pSpecialData)[j].strTableName) == 0)
			{
				iFound = 1;
				break;
			}
		}
		
		/*记录数据工单TableName对应的SpecialColumn配置开始索引*/
		if(iFound)
			(pTableMapList->pTableMapData)[i].iSpecialIdx = j;
		else
			(pTableMapList->pTableMapData)[i].iSpecialIdx = -1;
	}
	
	return;
}

/****************************************************************** 
Function:	relateColumnIndex
Description:	计算TableMap和TableColumn关联索引
Input:		
Return: 
Others:	
********************************************************************/
void relateColumnIndex()
{
	int i, j, k, iFound;
	TableColumnListType *pColumnList = &(gDataOrderConfig.stColumnList);
	TableMapListType *pTableMapList = &(gDataOrderConfig.stTableMapList);
	
	if(pColumnList->iCount == 0 || pTableMapList->iCount == 0)
		return;

	for(i = 0; i < pTableMapList->iCount; i++)
	{
		/*定位主表在TableColumn信息中的索引和字段数*/
		for(j = 0, iFound = 0; j < pColumnList->iCount; j++)
		{
			if(strcasecmp((pTableMapList->pTableMapData)[i].strMainTableName, (pColumnList->pColumnData)[j].strTableName) == 0
				&& strcasecmp((pTableMapList->pTableMapData)[i].strMainOwner, (pColumnList->pColumnData)[j].strOwner) == 0
				&& (pColumnList->pColumnData)[j].strObjectType[0] == DATA_OBJECT_TYPE_MAIN)
			{
				if(iFound == 0)
				{
					k = j;
					iFound = 1;
				}
			}
			else
			{
				if(iFound)
					break;
			}
		}
		
		if(iFound)
		{
			(pTableMapList->pTableMapData)[i].iColumnIdx[0] = k;
			(pTableMapList->pTableMapData)[i].iColumnCnt[0] = j - k;
		}
		else
		{
			(pTableMapList->pTableMapData)[i].iColumnIdx[0] = -1;
		}

		/*定位变更表在TableColumn信息中的索引和字段数*/
		if((pTableMapList->pTableMapData)[i].strChgFlag[0] == DATA_DEAL_FLAG_TRUE)
		{
			for(j = 0, iFound = 0; j < pColumnList->iCount; j++)
			{
				if(strcasecmp((pTableMapList->pTableMapData)[i].strChgTableName, (pColumnList->pColumnData)[j].strTableName) == 0
					&& strcasecmp((pTableMapList->pTableMapData)[i].strChgOwner, (pColumnList->pColumnData)[j].strOwner) == 0
					&& (pColumnList->pColumnData)[j].strObjectType[0] == DATA_OBJECT_TYPE_CHANGE)
				{
					if(iFound == 0)
					{
						k = j;
						iFound = 1;
					}
				}
				else
				{
					if(iFound == 1)
						break;
				}
			}
			
			if(iFound)
			{
				(pTableMapList->pTableMapData)[i].iColumnIdx[1] = k;
				(pTableMapList->pTableMapData)[i].iColumnCnt[1] = j - k;
			}
			else
			{
				(pTableMapList->pTableMapData)[i].iColumnIdx[1] = -1;
			}
		}

		/*定位历史表在TableColumn信息中的索引和字段数*/
		if((pTableMapList->pTableMapData)[i].strHisFlag[0] == DATA_DEAL_FLAG_TRUE)
		{
			for(j = 0, iFound = 0; j < pColumnList->iCount; j++)
			{
				if(strcasecmp((pTableMapList->pTableMapData)[i].strHisTableName, (pColumnList->pColumnData)[j].strTableName) == 0
					&& strcasecmp((pTableMapList->pTableMapData)[i].strHisOwner, (pColumnList->pColumnData)[j].strOwner) == 0
					&& (pColumnList->pColumnData)[j].strObjectType[0] == DATA_OBJECT_TYPE_HISTORY)
				{
					if(iFound == 0)
					{
						k = j;
						iFound = 1;
					}
				}
				else
				{
					if(iFound == 1)
						break;
				}
			}
			
			if(iFound)
			{
				(pTableMapList->pTableMapData)[i].iColumnIdx[2] = k;
				(pTableMapList->pTableMapData)[i].iColumnCnt[2] = j - k;
			}
			else
			{
				(pTableMapList->pTableMapData)[i].iColumnIdx[2] = -1;
			}
		}
	}
	
	
	for(i = 0; i < pTableMapList->iCount; i++)
	{
		/*关联增量表和主表字段关系,chMainFlag非零表示增量表中字段在主表中能找到对应字段*/
		if((pTableMapList->pTableMapData)[i].strChgFlag[0] == DATA_DEAL_FLAG_TRUE)
		{

			for(j = (pTableMapList->pTableMapData)[i].iColumnIdx[1]; j < MAX_COLUMN_NUM; j++)
			{
				if(strcasecmp((pColumnList->pColumnData)[j].strTableName, (pTableMapList->pTableMapData)[i].strChgTableName))
					break;

				iFound = 0;
				
				for(k = (pTableMapList->pTableMapData)[i].iColumnIdx[0]; k < MAX_COLUMN_NUM; k++)
				{
					if(strcasecmp((pColumnList->pColumnData)[k].strTableName, 
						(pTableMapList->pTableMapData)[i].strMainTableName))
						break;
					
					if(strcasecmp((pColumnList->pColumnData)[k].strColumnName, 
						(pColumnList->pColumnData)[j].strColumnName) == 0)
					{
						iFound = 1;
						break;
					}
				}
				
				if(iFound)
					(pColumnList->pColumnData)[j].chMainFlag = 1;
				else
					(pColumnList->pColumnData)[j].chMainFlag = 0;
			}
		}
		
		/*关联历史表和主表字段关系,chMainFlag非零表示历史表中字段在主表中能找到对应字段*/
		if((pTableMapList->pTableMapData)[i].strHisFlag[0] == DATA_DEAL_FLAG_TRUE)
		{
			for(j = (pTableMapList->pTableMapData)[i].iColumnIdx[2]; j < MAX_COLUMN_NUM; j++)
			{
				if(strcasecmp((pColumnList->pColumnData)[j].strTableName, (pTableMapList->pTableMapData)[i].strHisTableName))
					break;

				iFound = 0;
				
				for(k = (pTableMapList->pTableMapData)[i].iColumnIdx[0]; k < MAX_COLUMN_NUM; k++)
				{
					if(strcasecmp((pColumnList->pColumnData)[k].strTableName, 
						(pTableMapList->pTableMapData)[i].strMainTableName))
						break;
					
					if(strcasecmp((pColumnList->pColumnData)[k].strColumnName, 
						(pColumnList->pColumnData)[j].strColumnName) == 0)
					{
						iFound = 1;
						break;
					}
				}
				
				if(iFound)
					(pColumnList->pColumnData)[j].chMainFlag = 1;
				else
					(pColumnList->pColumnData)[j].chMainFlag = 0;
			}
		}
	}

	return;
}

/****************************************************************** 
Function:	initDataOrderConfig
Description:	初始化数据工单的配置信息
Input:		
Return: 
Others:	
********************************************************************/
int initDataOrderConfig()
{
	strcpy(gSpecialColumnTable, "tb_special_column");
	strcpy(gTableMapTable, "tb_table_map");
	
	pDataOrderConfig = &gDataOrderConfig;
	
	memset(pDataOrderConfig, 0, sizeof(DataOrderConfigType));
	
	if(initSpecialColumn())
		return 1;
		
	if(initTableColumn())
		return 1;

	if(initTableMap())
		return 1;
		
	relateSpecialIndex();
	
	relateColumnIndex();
	
	gDataOrderConfig.load_time = time(NULL);
	gDataOrderConfig.chInitFlag = 1;
	
	return 0;
}

/****************************************************************** 
Function:	judgeDealFlag
Description:	判断是否需要插入变更表
Input:		char *pTableName 判断的表名
Output:		char *result 判断结果
		int *idx 表名在TableMap中的位置
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int judgeDealFlag(char *pTableName, char *result, int *idx)
{
	int	i;
	TableMapListType *pTableMapList = &(gDataOrderConfig.stTableMapList);
		
	/*默认仅处理主表*/
	*result = *(result + 1) = DATA_DEAL_FLAG_FALSE;
	*idx = -1;

	for(i = 0; i < pTableMapList->iCount; i++)
	{
		if(strcasecmp((pTableMapList->pTableMapData)[i].strTableName, pTableName) == 0)
		{
			*idx = i;
			
			if((pTableMapList->pTableMapData)[i].strChgFlag[0] == DATA_DEAL_FLAG_TRUE
				&& strlen((pTableMapList->pTableMapData)[i].strChgOwner) > 0
				&& strlen((pTableMapList->pTableMapData)[i].strChgTableName) > 0)
			{
				*result = DATA_DEAL_FLAG_TRUE;
			}
				
			if((pTableMapList->pTableMapData)[i].strHisFlag[0] == DATA_DEAL_FLAG_TRUE
				&& strlen((pTableMapList->pTableMapData)[i].strHisOwner) > 0
				&& strlen((pTableMapList->pTableMapData)[i].strHisTableName) > 0)
			{
				*(result + 1) = DATA_DEAL_FLAG_TRUE;
			}

			return 0;
		}
	}
	
	return 1;
}

/****************************************************************** 
Function:	getDataType
Description:	自定义数据类型转换为Oracle数据类型
Input:		int iDataFormat 自定义数据类型
Output:		int iDataType Oracle数据类型
Return: 	0：正常；1：错误
Others:		
********************************************************************/
int getDataType(const int iDataFormat, int *iDataType)
{
	if(iDataFormat == DATA_DATA_TYPE_STRING)
		*iDataType = 96;
	else if(iDataFormat == DATA_DATA_TYPE_NUMBER)
		*iDataType = 96;
	else if(iDataFormat == DATA_DATA_TYPE_DATE)
		*iDataType = 96;
	else
		return 1;

	return 0;
}

/****************************************************************** 
Function:	getDealType
Description:	查找字段处理类型
Input:		int iIndex 表在TableMap数据中的索引
		char chObjectType 操作对象类型
		char chOperType 操作类型
		char *pColumnName 字段名
Output:		int iDealType SpecialColumn中记录的字段处理类型
Return: 	负数：错误，非负数：正常
Others:		
********************************************************************/
int getDealType(const int iIndex, const char chObjectType, const char chOperType, const char *pColumnName, int *iDealType)
{
	int	i;
	TableMapListType	*tm = &(gDataOrderConfig.stTableMapList);
	SpecialColumnListType	*sc = &(gDataOrderConfig.stSpecialList);
	
	*iDealType = DATA_DEAL_TYPE_NULL;
	
	if(iIndex < 0 || iIndex >= tm->iCount)
		return -1;
	
	i = tm->pTableMapData[iIndex].iSpecialIdx;
	
	if(i < 0)
		return 0;

	for( ; i < sc->iCount; i++)
	{
		if(strcasecmp((tm->pTableMapData)[iIndex].strTableName, (sc->pSpecialData)[i].strTableName))
			break;
		
		if(strcasecmp(pColumnName, (sc->pSpecialData)[i].strColumnName) \
			|| (sc->pSpecialData)[i].strObjectType[0] != chObjectType \
			|| (sc->pSpecialData)[i].strOperType[0] != chOperType)
			continue;
		
		*iDealType = (sc->pSpecialData)[i].iDealType;
		
		return i;
	}

	return 0;
}

/****************************************************************** 
Function:	getOrderTable
Description:	取工单表字段数据
Input:		const OrderTable *pOrderTable 数据工单接口表数据
		const char *pName 字段名
Output:		char *pValue 字段值
Return: 	0：成功，非0：失败
Others:		
********************************************************************/
int getOrderTable(char *pValue, const OrderTable *pOrderTable, const char *pName)
{
	if(strcasecmp(pName, "order_accept") == 0)
		sprintf(pValue, "%ld", pOrderTable->order_accept);
	else if(strcasecmp(pName, "order_type") == 0)
		sprintf(pValue, "%s", pOrderTable->order_type);
	else if(strcasecmp(pName, "id_type") == 0)
		sprintf(pValue, "%s", pOrderTable->id_type);
	else if(strcasecmp(pName, "id_no") == 0)
		sprintf(pValue, "%s", pOrderTable->id_no);
	else if(strcasecmp(pName, "interface_type") == 0)
		sprintf(pValue, "%s", pOrderTable->interface_type);
	else if(strcasecmp(pName, "busi_type") == 0)
		sprintf(pValue, "%s", pOrderTable->busi_type);
	else if(strcasecmp(pName, "busi_code") == 0)
		sprintf(pValue, "%s", pOrderTable->busi_code);
	else if(strcasecmp(pName, "order_right") == 0)
		sprintf(pValue, "%d", pOrderTable->order_right);
	else if(strcasecmp(pName, "op_type") == 0)
		sprintf(pValue, "%s", pOrderTable->op_type);
	else if(strcasecmp(pName, "op_code") == 0)
		sprintf(pValue, "%s", pOrderTable->op_code);
	else if(strcasecmp(pName, "op_accept") == 0)
		sprintf(pValue, "%ld", pOrderTable->op_accept);
	else if(strcasecmp(pName, "op_login") == 0)
		sprintf(pValue, "%s", pOrderTable->op_login);
	else if(strcasecmp(pName, "total_date") == 0)
		sprintf(pValue, "%d", pOrderTable->total_date);
	else if(strcasecmp(pName, "op_time") == 0)
		sprintf(pValue, "%s", pOrderTable->op_time);
	else if(strcasecmp(pName, "op_note") == 0)
		sprintf(pValue, "%s", pOrderTable->op_note);
	else if(strcasecmp(pName, "data_format") == 0)
		sprintf(pValue, "%s", pOrderTable->data_format);
	else if(strcasecmp(pName, "det_flag") == 0)
		sprintf(pValue, "%s", pOrderTable->det_flag);
	else if(strcasecmp(pName, "commuinfo_flag") == 0)
		sprintf(pValue, "%s", pOrderTable->commuinfo_flag);
	else if(strcasecmp(pName, "send_num") == 0)
		sprintf(pValue, "%d", pOrderTable->send_num);
	else if(strcasecmp(pName, "send_status") == 0)
		sprintf(pValue, "%s", pOrderTable->send_status);
	else if(strcasecmp(pName, "send_time") == 0)
		sprintf(pValue, "%s", pOrderTable->send_time);
	else if(strcasecmp(pName, "resp_code") == 0)
		sprintf(pValue, "%s", pOrderTable->resp_code);
	else if(strcasecmp(pName, "resp_time") == 0)
		sprintf(pValue, "%s", pOrderTable->resp_time);
	else
		return 1;

	return 0;
}

/*******************************************************************
Function:	getOrderData
Description:	取工单字段数据
Input:		const DataOrderType *pDataOrder 已解析的数据工单数据
		const char *pName 字段名
Output:		char *pValue 字段值
		int *iDataFormat 字段类型
Return: 	0：成功，非0：失败
Others:		
********************************************************************/
int getOrderData(char *pValue, int *iDataFormat, const DataOrderType *pDataOrder, const char *pName)
{
	DataItemListType	*pItem = pDataOrder->DataItemList;
	DataItemListType	*pPrimary = pDataOrder->PrimaryKeyList;

	while(pItem != NULL)
	{
		if(strcasecmp(pName, pItem->DataItem->DataName) == 0)
		{
			sprintf(pValue, pItem->DataItem->DataValue);

			*iDataFormat = pItem->DataItem->DataFormat;

			return 0;
		}
		
		pItem = pItem->next;
	}
	
	while(pPrimary != NULL)
	{
		if(strcasecmp(pName, pPrimary->DataItem->DataName) == 0)
		{
			strcpy(pValue, pPrimary->DataItem->DataValue);

			*iDataFormat = pItem->DataItem->DataFormat;

			return 0;
		}
		
		pPrimary = pPrimary->next;
	}
	
	return 1;
}


/*******************************************************************
Function:	dealSpecialColumn
Description:	处理特殊字段
Input:		const OrderTable *pOrderTable 数据工单接口表数据
		const DataOrderType *pDataOrder 已解析的数据工单数据
		const int iTableIdx TableMap索引
		const int iSpecialIdx SpecialColumn索引
		const int iCount 绑定变量索引号
Output:		SqlFieldType *pField 生成的Sql数据
Return: 	0：成功，非0：失败
Others:		
********************************************************************/
int dealSpecialColumn(SqlFieldType *pField, const OrderTable *pOrderTable, const DataOrderType *pDataOrder, const char chObjectType, const char chOperType, const int iTableidx, const int iSpecialIdx, const int iCount)
{
	TableMapListType	*tm = &(gDataOrderConfig.stTableMapList);
	SpecialColumnListType	*sc = &(gDataOrderConfig.stSpecialList);
	TableColumnListType	*tc = &(gDataOrderConfig.stColumnList);
	
	SpecialColumnItemType	*pSpecial;

	if(iSpecialIdx < 0 || iSpecialIdx >= sc->iCount)
		return 1;

	pSpecial = &((sc->pSpecialData)[iSpecialIdx]);

	switch(pSpecial->iDealType)
	{
		case	DATA_DEAL_TYPE_NULL:
		{
			return 0;
		}
		case	DATA_DEAL_TYPE_IGNORE:
		{
			return 0;
		}
		case	DATA_DEAL_TYPE_SQLFUNC1:
		{
			strcpy(pField->strFieldName, pSpecial->strColumnName);
			strtrim(pField->strFieldSql, pSpecial->strFieldSql);
			
			return 0;
		}
		case	DATA_DEAL_TYPE_SQLFUNC2:
		{
			strcpy(pField->strFieldName, pSpecial->strColumnName);
			sprintf(pField->strFieldSql, pSpecial->strFieldSql, iCount + 1);
			strcpy(pField->strValue, pSpecial->strDealParameter);
			
			pField->iDataFormat = pSpecial->iDataFormat;

			pField->iDataType = 96;
			
			return 0;
		}
		case	DATA_DEAL_TYPE_ORDERDATA:
		{
			strcpy(pField->strFieldName, pSpecial->strColumnName);
			sprintf(pField->strFieldSql, pSpecial->strFieldSql, iCount + 1);
			getOrderTable(pField->strValue, pOrderTable, pSpecial->strDealParameter);
			
			pField->iDataFormat = pSpecial->iDataFormat;

			pField->iDataType = 96;
			
			return 0;
		}
		case	DATA_DEAL_TYPE_CONVDATA:
		{
			strcpy(pField->strFieldName, pSpecial->strColumnName);
			sprintf(pField->strFieldSql, pSpecial->strFieldSql, iCount + 1);
			getOrderData(pField->strValue, &(pField->iDataFormat), pDataOrder, pSpecial->strDealParameter);
			
			pField->iDataType = 96;
			
			return 0;
		}
		default:
			break;
	}

	return 9;
}

/****************************************************************** 
Function:	buildSqlData
Description:	生成数据工单操作SQL及数据
Input:		const char chOperType 操作类型：插入、更新、删除
		const char chObjectType 对象类型：主表、变更表、历史表
		const int iIndex 此表在TableMap信息中的索引值
		DataOrderType *pDataOrder 已解析的数据工单数据
		OrderTable *pOrderTable 数据工单接口表数据
Output:		DataOrderSqlType *pSqlData 生成的SQL语句及数据
Return: 	0：成功，非0：失败
Others:		pSql需在调用函数中分配足够内存
********************************************************************/
int buildSqlData(const char chOperType, const char chObjectType, const int iIndex, DataOrderSqlType *pSqlData, const DataOrderType *pDataOrder, const OrderTable *pOrderTable)
{
	int	i, j, iCount, iWhereCount, iDealType, iRet, iBegin, iEnd, iDataFormat;
	char	strOwner[MAX_ITEM_SIZE], strTableName[MAX_ITEM_SIZE], strTmp1[MAX_SQL_LEN], strTmp2[MAX_SQL_LEN], strTmp3[MAX_SQL_LEN];

	TableMapListType	*tm = &(gDataOrderConfig.stTableMapList);
	SpecialColumnListType	*sc = &(gDataOrderConfig.stSpecialList);
	TableColumnListType	*tc = &(gDataOrderConfig.stColumnList);
	
	DataItemListType	*pItem = pDataOrder->DataItemList;
	DataItemListType	*pPrimary = pDataOrder->PrimaryKeyList;
	
	SqlFieldType		stField;

	memset(strOwner, 0, sizeof(strOwner));
	memset(strTableName, 0, sizeof(strTableName));
	
	if(iIndex < 0 || iIndex >= tm->iCount)
		return 1;

	/*生成表名*/
	if(chObjectType == DATA_OBJECT_TYPE_MAIN)
	{
		strcpy(strOwner, tm->pTableMapData[iIndex].strMainOwner);
		strcpy(strTableName, tm->pTableMapData[iIndex].strMainTableName);
	}
	else if(chObjectType == DATA_OBJECT_TYPE_CHANGE)
	{
		strcpy(strOwner, tm->pTableMapData[iIndex].strChgOwner);
		strcpy(strTableName, tm->pTableMapData[iIndex].strChgTableName);
	}
	else if(chObjectType == DATA_OBJECT_TYPE_HISTORY)
	{
		strcpy(strOwner, tm->pTableMapData[iIndex].strHisOwner);
		strcpy(strTableName, tm->pTableMapData[iIndex].strHisTableName);
	}
	else
		return 2;
	
	if(strlen(strOwner) < 1 || strlen(strTableName) < 1)
		return 3;

	memset(strTmp1, 0, sizeof(strTmp1));
	
	if(chOperType == DATA_OPER_TYPE_INSERT)
		sprintf(strTmp1, "INSERT INTO %s.%s ", strOwner, strTableName);
	else if(chOperType == DATA_OPER_TYPE_DELETE)
	{
		if(chObjectType == DATA_OBJECT_TYPE_MAIN)
			sprintf(strTmp1, "DELETE FROM %s.%s WHERE ", strOwner, strTableName);
		else
			sprintf(strTmp1, "INSERT INTO %s.%s ", strOwner, strTableName);
	}
	else if(chOperType == DATA_OPER_TYPE_UPDATE)
	{
		if(chObjectType == DATA_OBJECT_TYPE_MAIN)
			sprintf(strTmp1, "UPDATE %s.%s SET ", strOwner, strTableName);
		else
			sprintf(strTmp1, "INSERT INTO %s.%s ", strOwner, strTableName);
	}
	else
		return 4;

	strcat(pSqlData->strSql, strTmp1);

	memset(strTmp1, 0, sizeof(strTmp1));
	memset(strTmp2, 0, sizeof(strTmp2));
	
	/*SQL语句主干*/
	
	iCount = 0;
	iWhereCount = 0;

	/*插入操作*/
	if(chOperType == DATA_OPER_TYPE_INSERT && chObjectType == DATA_OBJECT_TYPE_MAIN)
	{
		/*字段名区域*/
		strcpy(strTmp1, " (");
	
		/*绑定变量区域*/
		strcpy(strTmp2, " VALUES(");

		while(pItem != NULL)
		{
			memset(strTmp3, 0, sizeof(strTmp3));
			
			iRet = getDealType(iIndex, chObjectType, chOperType, pItem->DataItem->DataName, &iDealType);
			
			if(iDealType != DATA_DEAL_TYPE_NULL)
			{
				pItem = pItem->next;
				continue;
			}

			if(iCount > 0)
				sprintf(strTmp3, ", %s", pItem->DataItem->DataName);
			else
				sprintf(strTmp3, "%s", pItem->DataItem->DataName);
			
			if(strlen(strTmp3) + strlen(strTmp1) >= MAX_SQL_LEN)
				return 5;
			
			strcat(strTmp1, strTmp3);

			snprintf((pSqlData->stFields)[iCount].strFieldName, MAX_ITEM_SIZE - 1, "%s", pItem->DataItem->DataName);
			snprintf((pSqlData->stFields)[iCount].strValue, MAX_FIELD_LEN - 1, "%s", pItem->DataItem->DataValue);
		
			if(getDataType(pItem->DataItem->DataFormat, &((pSqlData->stFields)[iCount].iDataType)))
				return 6;

			(pSqlData->stFields)[iCount].iDataFormat = pItem->DataItem->DataFormat;
			
			memset(strTmp3, 0, sizeof(strTmp3));
			
			if((pSqlData->stFields)[iCount].iDataFormat == DATA_DATA_TYPE_DATE)
				sprintf(strTmp3, "%sTO_DATE(:v%d, '%s')", (iCount > 0) ? ", " : NULL, iCount + 1, DATA_ORDER_DATE_FORMAT);
			else
				sprintf(strTmp3, "%s:v%d", (iCount > 0) ? ", " : NULL, iCount + 1);

			if(strlen(strTmp3) + strlen(strTmp2) >= MAX_SQL_LEN)
				return 7;
				
			strcat(strTmp2, strTmp3);
			
			iCount++;

			pItem = pItem->next;
		}
		
		if((i = (tm->pTableMapData)[iIndex].iSpecialIdx) >= 0)
		{
			for( ; i < sc->iCount; i++)
			{
				if(strcasecmp((tm->pTableMapData)[iIndex].strTableName, (sc->pSpecialData)[i].strTableName))
					break;

				if((sc->pSpecialData)[i].strObjectType[0] != chObjectType \
					|| (sc->pSpecialData)[i].strOperType[0] != chOperType)
					continue;

				if((sc->pSpecialData)[i].iDealType == DATA_DEAL_TYPE_NULL \
					|| (sc->pSpecialData)[i].iDealType == DATA_DEAL_TYPE_IGNORE)
					continue;
				
				memset(&stField, 0, sizeof(stField));
				
				if(dealSpecialColumn(&stField, pOrderTable, pDataOrder, chObjectType, chOperType, iIndex, i, iCount))
					return 8;

				memset(strTmp3, 0, sizeof(strTmp3));
				
				sprintf(strTmp3, "%s%s", (iCount > 0) ? ", " : NULL, (sc->pSpecialData)[i].strColumnName);
				
				if(strlen(strTmp3) + strlen(strTmp1) > MAX_SQL_LEN)
					return 7;
				
				strcat(strTmp1, strTmp3);
					
					
				memset(strTmp3, 0, sizeof(strTmp3));
				
				sprintf(strTmp3, "%s%s", (iCount > 0) ? ", " : NULL, stField.strFieldSql);
				
				if(strlen(strTmp3) + strlen(strTmp2) > MAX_SQL_LEN)
					return 7;

				strcat(strTmp2, strTmp3);
				
				if(strstr(strTmp3, ":v") != NULL)
				{
					memcpy(&(pSqlData->stFields)[iCount], &stField, sizeof(stField));
					iCount++;
				}
			}
		}
		
		strcat(strTmp1, ")");
		strcat(strTmp2, ")");
		
		if(strlen(strTmp1) + strlen(strTmp2) >= MAX_SQL_LEN)
			return 9;
		
		strcat(strTmp1, strTmp2);
		
		pSqlData->iCount = iCount;
	}
	/*删除主表操作*/
	else if(chOperType == DATA_OPER_TYPE_DELETE && chObjectType == DATA_OBJECT_TYPE_MAIN)
	{
		while(pPrimary != NULL)
		{
			memset(strTmp3, 0, sizeof(strTmp3));

			snprintf((pSqlData->stFields)[iWhereCount].strFieldName, MAX_ITEM_SIZE - 1, "%s", pPrimary->DataItem->DataName);
			snprintf((pSqlData->stFields)[iWhereCount].strValue, MAX_FIELD_LEN - 1, "%s", pPrimary->DataItem->DataValue);

			if(getDataType(pPrimary->DataItem->DataFormat, &((pSqlData->stFields)[iWhereCount].iDataType)))
				return 6;

			(pSqlData->stFields)[iWhereCount].iDataFormat = pPrimary->DataItem->DataFormat;

			if((pSqlData->stFields)[iWhereCount].iDataFormat == DATA_DATA_TYPE_DATE)
				sprintf(strTmp3, "%s%s=TO_DATE(:v%d, '%s') ", ((iWhereCount > 0) ? "AND " : NULL), pPrimary->DataItem->DataName, \
					iWhereCount + 1, DATA_ORDER_DATE_FORMAT);
			else
				sprintf(strTmp3, "%s%s=:v%d ", ((iWhereCount > 0) ? "AND " : NULL), pPrimary->DataItem->DataName, iWhereCount + 1);

			if(strlen(strTmp1) + strlen(strTmp3) >= MAX_SQL_LEN)
				return 7;

			strcat(strTmp1, strTmp3);

			iWhereCount++;

			pPrimary = pPrimary->next;
		}

		if(iWhereCount < 1)
			return 8;

		pSqlData->iCount = iWhereCount;
	}
	/*更新主表操作*/
	else if(chOperType == DATA_OPER_TYPE_UPDATE && chObjectType == DATA_OBJECT_TYPE_MAIN)
	{
		while(pItem != NULL)
		{
			memset(strTmp3, 0, sizeof(strTmp3));
			
			iRet = getDealType(iIndex, chObjectType, chOperType, pItem->DataItem->DataName, &iDealType);
			
			if(iDealType == DATA_DEAL_TYPE_IGNORE)
			{
				pItem = pItem->next;
				continue;
			}

			snprintf((pSqlData->stFields)[iCount].strFieldName, MAX_ITEM_SIZE - 1, "%s", pItem->DataItem->DataName);
			snprintf((pSqlData->stFields)[iCount].strValue, MAX_FIELD_LEN - 1, "%s", pItem->DataItem->DataValue);
			
			if(getDataType(pItem->DataItem->DataFormat, &((pSqlData->stFields)[iCount].iDataType)))
				return 6;

			(pSqlData->stFields)[iCount].iDataFormat = pItem->DataItem->DataFormat;

			if((pSqlData->stFields)[iCount].iDataFormat == DATA_DATA_TYPE_DATE)
				sprintf(strTmp3, "%s%s=TO_DATE(:v%d, '%s')", ((iCount > 0) ? ", " : NULL), pItem->DataItem->DataName, \
					iCount + 1, DATA_ORDER_DATE_FORMAT);
			else
				sprintf(strTmp3, "%s%s=:v%d", ((iCount > 0) ? ", " : NULL), pItem->DataItem->DataName, iCount + 1);
			
			if(strlen(strTmp3) + strlen(strTmp1) >= MAX_SQL_LEN)
				return 7;

			strcat(strTmp1, strTmp3);

			iCount++;

			pItem = pItem->next;
		}
		
		iWhereCount = iCount;
		
		while(pPrimary != NULL)
		{
			memset(strTmp3, 0, sizeof(strTmp3));

			snprintf((pSqlData->stFields)[iWhereCount].strFieldName, MAX_ITEM_SIZE - 1, "%s", pPrimary->DataItem->DataName);
			snprintf((pSqlData->stFields)[iWhereCount].strValue, MAX_FIELD_LEN - 1, "%s", pPrimary->DataItem->DataValue);

			if(getDataType(pPrimary->DataItem->DataFormat, &((pSqlData->stFields)[iWhereCount].iDataType)))
				return 6;

			(pSqlData->stFields)[iWhereCount].iDataFormat = pPrimary->DataItem->DataFormat;

			if((pSqlData->stFields)[iWhereCount].iDataFormat == DATA_DATA_TYPE_DATE)
				sprintf(strTmp3, "%s%s=TO_DATE(:v%d, '%s') ", ((iWhereCount - iCount > 0) ? "AND " : " WHERE "), \
					pPrimary->DataItem->DataName, iWhereCount + 1, DATA_ORDER_DATE_FORMAT);
			else
				sprintf(strTmp3, "%s%s=:v%d ", ((iWhereCount - iCount > 0) ? "AND " : " WHERE "), \
					pPrimary->DataItem->DataName, iWhereCount + 1);
			
			if(strlen(strTmp1) + strlen(strTmp3) >= MAX_SQL_LEN)
				return 7;
			
			strcat(strTmp1, strTmp3);
			
			iWhereCount++;
			
			pPrimary = pPrimary->next;
		}
		
		iWhereCount -= iCount;
		
		if(iWhereCount < 1)
			return 8;
		
		pSqlData->iCount = iCount + iWhereCount;
	}
	/*删除触发插入变更、历史表*/
	else if(chOperType == DATA_OPER_TYPE_DELETE)
	{
		if(chObjectType == DATA_OBJECT_TYPE_CHANGE)
		{
			iBegin = (tm->pTableMapData)[iIndex].iColumnIdx[1];
			iEnd = (tm->pTableMapData)[iIndex].iColumnIdx[1] + (tm->pTableMapData)[iIndex].iColumnCnt[1];
		}
		else
		{
			iBegin = (tm->pTableMapData)[iIndex].iColumnIdx[2];
			iEnd = (tm->pTableMapData)[iIndex].iColumnIdx[2] + (tm->pTableMapData)[iIndex].iColumnCnt[2];
		}
		
		if(iBegin >= 0)
		{
			for(i = iBegin ; i < iEnd; i++)
			{
				memset(strTmp3, 0, sizeof(strTmp3));
				
				if(i == iBegin)
					sprintf(strTmp3, " (%s", (tc->pColumnData)[i].strColumnName);
				else
					sprintf(strTmp3, ", %s", (tc->pColumnData)[i].strColumnName);

				if(strlen(strTmp1) + strlen(strTmp3) >= MAX_SQL_LEN)
					return 7;
					
				strcat(strTmp1, strTmp3);

				memset(strTmp3, 0, sizeof(strTmp3));
				
				if(i == iBegin)
					strcpy(strTmp3, " SELECT ");
				else
					strcpy(strTmp3, ", ");

				iRet = getDealType(iIndex, chObjectType, chOperType, (tc->pColumnData)[i].strColumnName, &iDealType);

				if(iDealType == DATA_DEAL_TYPE_NULL)
				{
					if((tc->pColumnData)[i].chMainFlag){
						strcat(strTmp3, (tc->pColumnData)[i].strColumnName);
					
					}
					else if((tc->pColumnData)[i].strNullAble[0] == 'Y')
						strcat(strTmp3, "NULL");
					else
						return 6;
				}
				else if(iDealType == DATA_DEAL_TYPE_IGNORE)
				{
					if((tc->pColumnData)[i].strNullAble[0] == 'Y')
						strcat(strTmp3, "NULL");
					else
						return 6;
				}
				else
				{
					memset(&stField, 0, sizeof(stField));

					if(dealSpecialColumn(&stField, pOrderTable, pDataOrder, chObjectType, chOperType, iIndex, iRet, iCount))
						return 8;
			
					strcat(strTmp3, stField.strFieldSql);

					if(strstr(strTmp3, ":v") != NULL)
					{
						memcpy(&(pSqlData->stFields)[iCount], &stField, sizeof(stField));
						iCount++;
					}
				}

				if(strlen(strTmp2) + strlen(strTmp3) >= MAX_SQL_LEN)
					return 7;

				strcat(strTmp2, strTmp3);
			}
		}
		
		strcat(strTmp1, ")");
		
		if(strlen(strTmp1) + strlen(strTmp2) > MAX_SQL_LEN)
			return 7;
		
		strcat(strTmp1, strTmp2);

		memset(strTmp3, 0, sizeof(strTmp3));
		sprintf(strTmp3, " FROM %s.%s ", (tm->pTableMapData)[iIndex].strMainOwner, (tm->pTableMapData)[iIndex].strMainTableName);

		strcat(strTmp1, strTmp3);

		iWhereCount = iCount;

		while(pPrimary != NULL)
		{
			memset(strTmp3, 0, sizeof(strTmp3));
	
			snprintf((pSqlData->stFields)[iWhereCount].strFieldName, MAX_ITEM_SIZE - 1, "%s", pPrimary->DataItem->DataName);
			if(getOrderData((pSqlData->stFields)[iWhereCount].strValue, &iDataFormat, pDataOrder, pPrimary->DataItem->DataName))
				return 5;
	
			if(getDataType(pPrimary->DataItem->DataFormat, &((pSqlData->stFields)[iWhereCount].iDataType)))
				return 6;
	
			(pSqlData->stFields)[iWhereCount].iDataFormat = pPrimary->DataItem->DataFormat;
	
			if((pSqlData->stFields)[iWhereCount].iDataFormat == DATA_DATA_TYPE_DATE)
				sprintf(strTmp3, "%s%s=TO_DATE(:v%d, '%s') ", ((iWhereCount - iCount > 0) ? " AND " : " WHERE "), pPrimary->DataItem->DataName, \
					iWhereCount + 1, DATA_ORDER_DATE_FORMAT);
			else
				sprintf(strTmp3, "%s%s=:v%d ", ((iWhereCount - iCount > 0) ? " AND " : " WHERE "), pPrimary->DataItem->DataName, \
					iWhereCount + 1);
			
			if(strlen(strTmp1) + strlen(strTmp3) >= MAX_SQL_LEN)
				return 7;
				
			strcat(strTmp1, strTmp3);
	
			iWhereCount++;

			pPrimary = pPrimary->next;
		}

		pSqlData->iCount = iWhereCount;

		iWhereCount -= iCount;

		if(iWhereCount < 1)
			return 9;
	}
	/*变更触发插入变更、历史表*/
	else
	{
		if(chObjectType == DATA_OBJECT_TYPE_CHANGE)
		{
			iBegin = (tm->pTableMapData)[iIndex].iColumnIdx[1];
			iEnd = (tm->pTableMapData)[iIndex].iColumnIdx[1] + (tm->pTableMapData)[iIndex].iColumnCnt[1];
		}
		else
		{
			iBegin = (tm->pTableMapData)[iIndex].iColumnIdx[2];
			iEnd = (tm->pTableMapData)[iIndex].iColumnIdx[2] + (tm->pTableMapData)[iIndex].iColumnCnt[2];
		}
		
		if(iBegin >= 0)
		{
			for(i = iBegin; i < iEnd; i++)
			{
				/*字段名区域*/
				memset(strTmp3, 0, sizeof(strTmp3));
				
				if(i == iBegin)
					sprintf(strTmp3, " (%s", (tc->pColumnData)[i].strColumnName);
				else
					sprintf(strTmp3, " ,%s", (tc->pColumnData)[i].strColumnName);

				if(strlen(strTmp1) + strlen(strTmp3) >= MAX_SQL_LEN)
					return 7;

				strcat(strTmp1, strTmp3);
				
				/*绑定变量区域*/
				memset(strTmp3, 0, sizeof(strTmp3));
				
				if(i == iBegin)
					strcpy(strTmp3, " VALUES(");
				else
					strcpy(strTmp3, ", ");

				iRet = getDealType(iIndex, chObjectType, chOperType, (tc->pColumnData)[i].strColumnName, &iDealType);

				if(iDealType == DATA_DEAL_TYPE_NULL)
				{
					memset(&stField, 0, sizeof(stField));
					
					strcpy(stField.strFieldName, (tc->pColumnData)[i].strColumnName);

					if(getOrderData(stField.strValue, &(stField.iDataFormat), pDataOrder, stField.strFieldName))
						return 5;

					if(stField.iDataFormat == DATA_DATA_TYPE_DATE)
						sprintf(stField.strFieldSql, "TO_DATE(:v%d, '%s')", iCount + 1, DATA_ORDER_DATE_FORMAT);
					else
						sprintf(stField.strFieldSql, ":v%d", iCount + 1);
					
					strcat(strTmp3, stField.strFieldSql);
					
					stField.iDataType = 96;
					
					memcpy(&(pSqlData->stFields)[iCount], &stField, sizeof(stField));
					
					iCount++;
				}
				else if(iDealType == DATA_DEAL_TYPE_IGNORE)
				{
					if((tc->pColumnData)[i].strNullAble[0] == 'Y')
						strcat(strTmp3, "NULL");
					else
						return 6;
				}
				else
				{
					memset(&stField, 0, sizeof(stField));

					if(dealSpecialColumn(&stField, pOrderTable, pDataOrder, chObjectType, chOperType, iIndex, iRet, iCount))
						return 8;
			
					strcat(strTmp3, stField.strFieldSql);

					if(strstr(strTmp3, ":v") != NULL)
					{
						memcpy(&(pSqlData->stFields)[iCount], &stField, sizeof(stField));
						iCount++;
					}
				}

				if(strlen(strTmp2) + strlen(strTmp3) >= MAX_SQL_LEN)
					return 7;

				strcat(strTmp2, strTmp3);
			}
		}
		
		strcat(strTmp1, ")");
		strcat(strTmp2, ")");
		
		if(strlen(strTmp1) + strlen(strTmp2) >= MAX_SQL_LEN)
			return 9;
		
		strcat(strTmp1, strTmp2);
		
		pSqlData->iCount = iCount;
	}
	
	if(strlen(pSqlData->strSql) + strlen(strTmp1) >= MAX_SQL_LEN)
		return 9;
	
	strcat(pSqlData->strSql, strTmp1);
	
	return 0;
}

/******************************************************************
Function:	relateSqlData
Description:	数据工单数据和绑定数据描述区关联
Input:		DataOrderSqlType *pSqlData 工单数据信息
Output:		SQLDA *pBindDa 绑定数据描述区
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int relateSqlData(SQLDA *pBindDa, DataOrderSqlType *pSqlData)
{
	int	i, iErrFlag = 0;
	for(i = 0; i < pBindDa->F; i++)
	{
		if(pSqlData->stFields[i].iDataType != 96)
		{
			iErrFlag = 1;
			break;
		}

		pSqlData->stFields[i].lValue = 0;
		pBindDa->V[i] = pSqlData->stFields[i].strValue;
		pBindDa->L[i] = strlen(pSqlData->stFields[i].strValue);
		
		pBindDa->T[i] = pSqlData->stFields[i].iDataType;
		pBindDa->I[i] = 0;
	}
	
	return iErrFlag;
}

/******************************************************************
Function:	dumpSqlData
Description:	输出SqlData信息
Input:		DataOrderSqlType *pSqlData 工单数据信息
Output:
Return:
Others:	
********************************************************************/
void dumpSqlData(DataOrderSqlType *pSqlData, char *pNote)
{
	return;

	int	i;
	
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Note : %s\n", pNote);
	
	printf("Sql : %s\n", pSqlData->strSql);
	printf("Count : %d\n", pSqlData->iCount);
	
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	for(i = 0; i < pSqlData->iCount; i++)
	{
		printf("[%02d]FieldName : %s, DataType : %d, DataFormat : %d, Value : %s\n",
			i, pSqlData->stFields[i].strFieldName, pSqlData->stFields[i].iDataType,
			pSqlData->stFields[i].iDataFormat, pSqlData->stFields[i].strValue);
	}
	
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/******************************************************************
Function:	dealDataOrderInsert
Description:	数据工单插入函数
Input:		DataOrderType *pDataOrder 已解析的数据工单数据
		OrderTable *pOrderTable 工单接口信息
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int dealDataOrderInsert(DataOrderType *pDataOrder, OrderTable *pOrderTable)
{
	char	chDealFlag[2];
	int	i, iIndex, iRet, iErrFlag;
	DataOrderSqlType	stSqlDataMain, stSqlDataChg;
	
	iRet = judgeDealFlag(pDataOrder->TableName, chDealFlag, &iIndex);
	
	if(iRet < 0)
		return -1;
	
	if(iRet > 0)
		return 10;

	memset(&stSqlDataMain, 0, sizeof(stSqlDataMain));
	memset(&stSqlDataChg, 0, sizeof(stSqlDataChg));
	
	if(iRet = buildSqlData(DATA_OPER_TYPE_INSERT, DATA_OBJECT_TYPE_MAIN, iIndex, &stSqlDataMain, pDataOrder, pOrderTable))
		return 20 + iRet;
	
	dumpSqlData(&stSqlDataMain, "Insert Main");
	
	main_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
	
	main_dp->N = MAX_FIELD_NUM;

	EXEC SQL PREPARE INSERT_MAIN_S FROM  :stSqlDataMain.strSql;
	EXEC SQL DESCRIBE BIND VARIABLES FOR INSERT_MAIN_S INTO main_dp;
	
	if(main_dp->F < 0)
	{
		sqlclu(main_dp);
		return 30;
	}
	
	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 31;
	}

	main_dp->N = main_dp->F;
	
	
	iErrFlag = relateSqlData(main_dp, &stSqlDataMain);
	
	if(iErrFlag)
	{
		sqlclu(main_dp);
		return 32;
	}
	
	EXEC SQL EXECUTE INSERT_MAIN_S USING DESCRIPTOR main_dp;
	
	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 33;
	}
	
	sqlclu(main_dp);

	if(chDealFlag[0] == DATA_DEAL_FLAG_TRUE)
	{
		if(iRet = buildSqlData(DATA_OPER_TYPE_INSERT, DATA_OBJECT_TYPE_CHANGE, iIndex, &stSqlDataChg, pDataOrder, pOrderTable))
		{
			return 50 + iRet;
		}
		
		dumpSqlData(&stSqlDataChg, "Insert Change");
	
		change_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
		
		EXEC SQL PREPARE INSERT_CHANGE_S FROM :stSqlDataChg.strSql;
		EXEC SQL DESCRIBE BIND VARIABLES FOR INSERT_CHANGE_S INTO change_dp;
		
		if(change_dp->F < 0)
		{
			sqlclu(change_dp);
			return 60;
		}
			
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 61;
		}
			
		change_dp->N = change_dp->F;
		
		iErrFlag = relateSqlData(change_dp, &stSqlDataChg);

		if(iErrFlag)
		{
			sqlclu(change_dp);
			return 62;
		}
		
		EXEC SQL EXECUTE INSERT_CHANGE_S USING DESCRIPTOR change_dp;
		
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 63;
		}
		
		sqlclu(change_dp);
	}
	
	return 0;
}

/****************************************************************** 
Function:	dealDataOrderDelete
Description:	数据工单删除函数
Input:		DataOrderType *pDataOrder 已解析的数据工单数据
		OrderTable *pOrderTable 工单接口信息
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int dealDataOrderDelete(DataOrderType *pDataOrder, OrderTable *pOrderTable)
{
	char	chDealFlag[2];
	int	i, iIndex, iRet, iErrFlag;
	DataOrderSqlType	stSqlDataMain, stSqlDataChg, stSqlDataHis;

	iRet = judgeDealFlag(pDataOrder->TableName, chDealFlag, &iIndex);
	
	if(iRet < 0)
		return -1;
	
	if(iRet > 0)
		return 10;

	memset(&stSqlDataMain, 0, sizeof(stSqlDataMain));
	memset(&stSqlDataChg, 0, sizeof(stSqlDataChg));
	memset(&stSqlDataHis, 0, sizeof(stSqlDataHis));


	if(pOrderTable->op_type[0] != '4' && chDealFlag[0] == DATA_DEAL_FLAG_TRUE)
	{
		if(iRet = buildSqlData(DATA_OPER_TYPE_DELETE, DATA_OBJECT_TYPE_CHANGE, iIndex, &stSqlDataChg, pDataOrder, pOrderTable))
		{
			return 50 + iRet;
		}
	
		dumpSqlData(&stSqlDataChg, "Delete Change");
	
		change_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
		
		EXEC SQL PREPARE DELETE_CHANGE_S FROM :stSqlDataChg.strSql;
		EXEC SQL DESCRIBE BIND VARIABLES FOR DELETE_CHANGE_S INTO change_dp;
		
		if(change_dp->F < 0)
		{
			sqlclu(change_dp);
			return 60;
		}
			
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 61;
		}
			
		change_dp->N = change_dp->F;
		
		iErrFlag = relateSqlData(change_dp, &stSqlDataChg);
		
		if(iErrFlag)
		{
			sqlclu(change_dp);
			return 62;
		}
		
		EXEC SQL EXECUTE DELETE_CHANGE_S USING DESCRIPTOR change_dp;
		
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 63;
		}
		
		sqlclu(change_dp);
	}
	
	if(pOrderTable->op_type[0] != '4' && chDealFlag[1] == DATA_DEAL_FLAG_TRUE)
	{
		if(iRet = buildSqlData(DATA_OPER_TYPE_DELETE, DATA_OBJECT_TYPE_HISTORY, iIndex, &stSqlDataHis, pDataOrder, pOrderTable))
		{
			return 70 + iRet;
		}
		
		dumpSqlData(&stSqlDataHis, "Delete History");
	
		history_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
		
		EXEC SQL PREPARE DELETE_HISTORY_S FROM :stSqlDataHis.strSql;
		EXEC SQL DESCRIBE BIND VARIABLES FOR DELETE_HISTORY_S INTO history_dp;
		
		if(history_dp->F < 0)
		{
			sqlclu(history_dp);
			return 80;
		}
			
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(history_dp);
			return 81;
		}
			
		history_dp->N = history_dp->F;
		
		iErrFlag = relateSqlData(history_dp, &stSqlDataHis);
		
		if(iErrFlag)
		{
			sqlclu(history_dp);
			return 82;
		}
		
		EXEC SQL EXECUTE DELETE_HISTORY_S USING DESCRIPTOR history_dp;
		
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(history_dp);
			return 83;
		}
		
		sqlclu(history_dp);
	}

	if(iRet = buildSqlData(DATA_OPER_TYPE_DELETE, DATA_OBJECT_TYPE_MAIN, iIndex, &stSqlDataMain, pDataOrder, pOrderTable))
	{
		return 20 + iRet;
	}
	
	dumpSqlData(&stSqlDataMain, "Delete Main");

	main_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
	
	main_dp->N = MAX_FIELD_NUM;

	EXEC SQL PREPARE DELETE_MAIN_S FROM  :stSqlDataMain.strSql;
	EXEC SQL DESCRIBE BIND VARIABLES FOR DELETE_MAIN_S INTO main_dp;
	
	if(main_dp->F < 0)
	{
		sqlclu(main_dp);
		return 30;
	}
	
	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 31;
	}

	main_dp->N = main_dp->F;
	
	iErrFlag = relateSqlData(main_dp, &stSqlDataMain);
	
	if(iErrFlag)
	{
		sqlclu(main_dp);
		return 32;
	}
	
	EXEC SQL EXECUTE DELETE_MAIN_S USING DESCRIPTOR main_dp;
	
	if(SQLCODE != SQLOK && SQLCODE != NOTFOUND)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 33;
	}
	
	sqlclu(main_dp);

	return 0;
}

/****************************************************************** 
Function:	dealDataOrderUpdate
Description:	数据工单更新函数
Input:		DataOrderType *pDataOrder 已解析的数据工单数据
		OrderTable *pOrderTable 工单接口信息
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int dealDataOrderUpdate(DataOrderType *pDataOrder, OrderTable *pOrderTable)
{
	char	chDealFlag[2];
	int	i, iIndex, iRet, iErrFlag;
	DataOrderSqlType	stSqlDataMain, stSqlDataChg;
	
	iRet = judgeDealFlag(pDataOrder->TableName, chDealFlag, &iIndex);
	
	if(iRet < 0)
		return -1;
	
	if(iRet > 0)
		return 10;

	memset(&stSqlDataMain, 0, sizeof(stSqlDataMain));
	memset(&stSqlDataChg, 0, sizeof(stSqlDataChg));

	if(iRet = buildSqlData(DATA_OPER_TYPE_UPDATE, DATA_OBJECT_TYPE_MAIN, iIndex, &stSqlDataMain, pDataOrder, pOrderTable))
		return 20 + iRet;
		
	dumpSqlData(&stSqlDataMain, "Update Main");

	main_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
	
	main_dp->N = MAX_FIELD_NUM;

	EXEC SQL PREPARE UPDATE_MAIN_S FROM  :stSqlDataMain.strSql;
	EXEC SQL DESCRIBE BIND VARIABLES FOR UPDATE_MAIN_S INTO main_dp;
	
	if(main_dp->F < 0)
	{
		sqlclu(main_dp);
		return 30;
	}
	
	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 31;
	}

	main_dp->N = main_dp->F;
	
	iErrFlag = relateSqlData(main_dp, &stSqlDataMain);
	
	if(iErrFlag)
	{
		sqlclu(main_dp);
		return 32;
	}
	
	EXEC SQL EXECUTE UPDATE_MAIN_S USING DESCRIPTOR main_dp;

	if(SQLCODE != SQLOK)
	{
		errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
		sqlclu(main_dp);
		return 33;
	}

	sqlclu(main_dp);

	if(chDealFlag[0] == DATA_DEAL_FLAG_TRUE)
	{
		if(iRet = buildSqlData(DATA_OPER_TYPE_UPDATE, DATA_OBJECT_TYPE_CHANGE, iIndex, &stSqlDataChg, pDataOrder, pOrderTable))
		{
			return 50 + iRet;
		}
		
		dumpSqlData(&stSqlDataChg, "Update Change");
	
		change_dp = sqlald(MAX_FIELD_NUM, MAX_VNAME_LEN, MAX_INAME_LEN);
		
		EXEC SQL PREPARE UPDATE_CHANGE_S FROM :stSqlDataChg.strSql;
		EXEC SQL DESCRIBE BIND VARIABLES FOR UPDATE_CHANGE_S INTO change_dp;
		
		if(change_dp->F < 0)
		{
			sqlclu(change_dp);
			return 60;
		}
			
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 61;
		}

		change_dp->N = change_dp->F;
		
		iErrFlag = relateSqlData(change_dp, &stSqlDataChg);

		if(iErrFlag)
		{
			sqlclu(change_dp);
			return 62;
		}
		
		EXEC SQL EXECUTE UPDATE_CHANGE_S USING DESCRIPTOR change_dp;
		
		if(SQLCODE != SQLOK)
		{
			errLog(strProcTag, "LINE[%d],SQLCODE[%d],SQLERR[%s]\n", __LINE__, SQLCODE, SQLERRMSG);
			sqlclu(change_dp);
			return 63;
		}
		
		sqlclu(change_dp);
	}
	
	return 0;
}


/****************************************************************** 
Function:	dealDataOrder
Description:	处理数据工单函数
Input:		OrderTable *pOrderTable 工单接口信息
		char *pXmlData	工单XML数据
Return: 	0：成功，非0：失败
Others:	
********************************************************************/
int dealDataOrder(OrderTable *pOrderTable, MsgBodyType *stMsgBody, char *pProcLabel)
{
	int	iRet;

	memset(strProcTag, 0, sizeof(strProcTag));
	
	strncpy(strProcTag, pProcLabel, 63);

	/*仅处理XML数据*/
	if(pOrderTable->data_format[0] != '1')
		return DATA_DEAL_ERROR_CHECK;

	if(gDataOrderConfig.chInitFlag == 0 && initDataOrderConfig())
		return 1;

	if(pOrderTable->op_type[0] == DATA_OPER_TYPE_INSERT)
	{
		iRet = dealDataOrderInsert(stMsgBody->DataOrder, pOrderTable);
		
		if(iRet < 0)
			return -1;
			
		if(iRet > 0)
		{
			errLog(strProcTag, "ErrorCode : %d\n", DATA_DEAL_ERROR_INSERT + iRet);
			return DATA_DEAL_ERROR_INSERT + iRet;
		}
	}
	else if(pOrderTable->op_type[0] == DATA_OPER_TYPE_DELETE || pOrderTable->op_type[0] == '4')
	{
		iRet = dealDataOrderDelete(stMsgBody->DataOrder, pOrderTable);
		
		if(iRet < 0)
			return -1;
			
		if(iRet > 0)
		{
			errLog(strProcTag, "ErrorCode : %d\n", DATA_DEAL_ERROR_DELETE + iRet);
			return DATA_DEAL_ERROR_DELETE + iRet;
		}
	}
	else if(pOrderTable->op_type[0] == DATA_OPER_TYPE_UPDATE)
	{
		iRet = dealDataOrderUpdate(stMsgBody->DataOrder, pOrderTable);
		
		if(iRet < 0)
			return -1;
			
		if(iRet > 0)
		{
			errLog(strProcTag, "ErrorCode : %d\n", DATA_DEAL_ERROR_UPDATE + iRet);
			return DATA_DEAL_ERROR_UPDATE + iRet;
		}
	}
	else
		return DATA_DEAL_ERROR_NOTSUPP;

	return 0;
}
