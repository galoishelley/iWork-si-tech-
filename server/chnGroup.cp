/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
#include "boss_srv.h"
#include "boss_define.h"
#include "pubProduct.h"
#include "pubLog.h"

#ifdef _DEBUG_
#undef _DEBUG_
#endif

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;

/**
 *     得到两到节点之间的关系.
 *     对渠道属主用户表dChnGroupInfo建立公有同义词dChnGroupInfoChn，与资源和三
 * 户中的dChnGroupInfo表区分开。
 * @author	lugz
 * @inparam	nodeSrc			源节节点
 * @inparam	nodeDest		目标节点
 * @return 0：表示两节点之间没有关系
 *         1: 表示源节点和父节点是同一个节点
 *         2: 表示源节点是目标节点的父节点
 *         3：表示源节点是目标节点的子节点
 */
int GetNodeRelation(const char* nodeSrc, const char* nodeDest)
{
	int count = 0;
	int retVal = 0;
	
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","GetNodeRelation in[%s][%s]",nodeSrc, nodeDest);
#endif
	if (strcmp(nodeSrc, nodeDest) == 0)
	{
		retVal = 1;
	}
	else
	{
		EXEC SQL	SELECT COUNT(*)
					  INTO :count
					  FROM dChnGroupInfo
					 WHERE group_id = :nodeSrc
					   AND parent_group_id = :nodeDest;
		if (count > 0)
		{
			retVal = 3;
		}
		else
		{
			EXEC SQL	SELECT COUNT(*)
						  INTO :count
						  FROM dChnGroupInfo
						 WHERE group_id = :nodeDest
						   AND parent_group_id = :nodeSrc;
			if (count > 0)
			{
				retVal = 2;
			}
		}
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","GetNodeRelation leave[%s][%s][%d]",nodeSrc, nodeDest,retVal);
#endif
	return retVal;
}

/*
 * getGroupCode   根据节点提取归属代码
 *
 * 功能:根据输入的节点值在树中从sRegionCode,sDisCode,sTownCode表中
 *      提取归属代码
 *
 * input  参数1: *node，       节点值，最大长度10个字节；
 *
 * output 参数2: group_code，  归属代码，最大长度7个字节
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *                      -2: 从数据库提取节点失败；
 *
 */
int getGroupCode(char *node,char *group_code)
{
	char groupCode[30+1];
	
	memset(groupCode, 0, sizeof(groupCode));
	EXEC SQL	SELECT BOSS_ORG_CODE
				  INTO :groupCode
				  FROM dChnGroupMsg
				 WHERE group_id = :node;
	if (SQLCODE != 0)
	{
		return -1;
	}
	Trim(groupCode);
	strncpy(group_code, groupCode, 7);
	group_code[7] = '\0';
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","getGroupCode:groupCode=[%s]",groupCode);
#endif
    return 0;
}


/*
 * chkNodeRelation    检查两节点之间是否存在父子关系函数
 *
 * 功能:检查两节点之间是否存在父子关系函数
 *
 * input参数1: *node1，      节点1，最大长度10个字节；
 *
 * input参数2: *node2，      节点2，最大长度10个字节；
 *
 *
 * 返回参数:             0: 不成立;
 *                       1: 成立；
 *
 */
int chkNodeRelation(char *node1, char *node2)
{
    int rela=0;
	rela=GetNodeRelation(node1, node2);
	
	if (rela == 2 || rela == 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/*
 * chkNodeAgentRelation    检查两节点之间是否存在合作关系函数
 *
 * 功能:检查两节点之间是否存在合作关系函数
 *
 * input参数1: *loginGrpId， 工号归属节点     节点1，最大长度10个字节；
 *
 * input参数2: *phoneGrpId， 号码归属节点     节点2，最大长度10个字节；
 *
 *
 * 返回参数:             0: 不成立;
 *                       1: 成立；
 *                      <0: ORACLE错误
 *
 */
int chkNodeAgentRelation(char *loginGrpId, char *phoneGrpId)
{
	int  vNum=0;

	EXEC SQL	SELECT NVL(Count(*),0) INTO :vNum
				  FROM wTownTown
				 WHERE Group_Id      = :loginGrpId
				   AND Allow_GroupId = :phoneGrpId;
               
	if ((SQLCODE != OK) && (SQLCODE != NOTFOUND))
	{
		return -1;
	}
	else 
	{
		if (vNum > 0) 
		{
			return 1;
		}
		else 
		{
			return 0;
		}
	}
}



/*
 * chkNodeAgentType    检查工号的归属节点是否代办营业厅
 *
 * 功能:检查工号的归属节点是否代办营业厅
 *
 * input参数1: *node1， 工号归属节点     节点1，最大长度10个字节；
 * input参数2: *array 暂时不用
 *
 * 返回参数:             0: 自办营业厅;
 *                       1: 代办营业厅；
 *                       2: 提取节点深度失败;
 *                      <0: ORACLE错误
 *
 */ 
 /***********************  吉林sTownCode表没有group_id，暂时不用该函数
int chkNodeAgentType(char *node,NODE *array)
{
	char groupCode[30+1];
	char townType[2+1];
	char agentFlag[1+1];
	
	memset(groupCode, 0, sizeof(groupCode));
	if (getGroupCode(node, groupCode) != 0)
	{
		return 2;
	}

	if (strncmp(groupCode + 4, "999", 3) == 0)
	{
		return 0;
	}

	memset(townType, 0, sizeof(townType));
	
	EXEC SQL	SELECT NVL(Innet_Type,'99')
				  INTO :townType
				  FROM sTownCode
				 WHERE Group_Id = :node;
	if (SQLCODE != OK)
	{
		return -2;
	}

	memset(agentFlag, 0, sizeof(agentFlag));
	EXEC SQL	SELECT NVL(Agent_Flag,'N')
				  INTO :agentFlag
				  FROM sAgType
				 WHERE Region_Code = Substr(:groupCode,1,2)
				   AND Agent_Type  = :townType;
	if (SQLCODE != OK)
	{
		return -3;
	}

	if (strncmp(agentFlag,"Y",1) == 0) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
**************************/



/*
 * getGroupName   根据节点提取节点或者父节点名称
 *
 * 功能:根据输入的节点值在树中判断节点深度，并返回相应的节点名称
 *
 * input  参数1: *node，       节点值，最大长度10个字节；
 *
 * input  参数2: level         暂时不用;
 *
 * input  参数3: NODE *        暂时不用;
 *
 * output 参数4: group_name，  节点名称
 *
 *
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *                      -2: 从数据库提取节点失败；
 *
 */

int getGroupName(char *node,int level,NODE *array, char *group_name)
{
	char groupName[60+1];                  /* 节点名称        */
	char groupCode[30+1];
	char townType[2+1];
	char agentFlag[1+1];
	
	memset(groupCode, 0, sizeof(groupCode));
	
	if (getGroupCode(node, groupCode) != 0)
	{
		return -1;
	}

	if (strncmp(groupCode+4, "999", 3) == 0)
	{
		EXEC SQL	SELECT town_name
					  INTO :groupName
					  FROM sTownCode
					 WHERE region_code = substr(:groupCode,1,2)
					   AND district_code = substr(:groupCode,3,2)
					   AND town_code = substr(:groupCode,5,3);
	}
	else if (strncmp(groupCode+2, "99", 2) == 0)
	{
		EXEC SQL	SELECT district_name
					  INTO :groupName
					  FROM sDisCode
					 WHERE region_code = substr(:groupCode,1,2)
					   AND district_code = substr(:groupCode,3,2);
	}
	else
	{
		EXEC SQL	SELECT region_name
					  INTO :groupName
					  FROM sRegionCode
					 WHERE region_code = substr(:groupCode,1,2);
	}
	
	if (SQLCODE != 0)
	{
		return -2;
	}
	return 0;
}


/**
 *
 * 得到开户GRP ID
 *
 */
int getOpenGrpId(char *phoneGrpId, char *loginGrpId, char *retOpenGrpId)
{
	int rela;
	rela=GetNodeRelation(phoneGrpId, loginGrpId);

	switch(rela)
	{
		case 0:
		{
			if (chkNodeAgentRelation(loginGrpId, phoneGrpId)== 1)
			{
				strcpy(retOpenGrpId, phoneGrpId);
			}
			else
			{
				return -1;
			}
			break;
		}
		case 1:
		{
			strcpy(retOpenGrpId, phoneGrpId);
			break;
		}
		case 2:
		{
			strcpy(retOpenGrpId, loginGrpId);
			break;
		}
		case 3:
		{
			strcpy(retOpenGrpId, phoneGrpId);
			break;
		}
		default:
		{
			break;
		}
	}
	
	return 0;
}

/**
 *     得到产品服务在最小的group_id
 */
int getLowestGroupId(const char* groupId, const char* serviceCode, char* lowestGroupId)
{
	char tmpLowestGroupId[10+1];
	
	memset(tmpLowestGroupId, 0, sizeof(tmpLowestGroupId));
	
	EXEC SQL	select group_id
				  into :lowestGroupId
				  from dChnGroupInfo
				 where group_id in (select group_id
				                      from sSrvRelease
				                     where service_code = :serviceCode
				                       and (  group_id in (select parent_group_id
				                                             from dChnGroupInfo
				                                            where group_id = :groupId
				                                           )
				                            or group_id = :groupId
				                           )
				                   )
				having denorm_level=max(denorm_level)
				 group by group_id,denorm_level;
	if (SQLCODE != 0)
	{
		return -1;
	}
	strcpy(lowestGroupId, tmpLowestGroupId);
	return 0;
}

/*
 * getGroupChildNode   行政树子节点下单次检索函数
 *
 * 功能:根据输入的节点值在树中单次检索子节点
 *
 * input参数1:   *parent_node， 父节电节点值，最大长度10个字节；
 *
 * input参数2:   NODE *array,   暂时无用，可为NULL
 *
 * output参数3:  child_node[][TREE_NODE_LENGTH+1]
 *                            字节电数组；
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *                      -2: 从数据库提取节点失败；
 *
 */
int getGroupChildNode(char *parent_node,NODE *array, char child_node[][TREE_NODE_LENGTH+1])
{
	char node[10+1];
	int i=0;
	
	EXEC SQL	DECLARE curgetChildNode CURSOR FOR 
				SELECT group_id
				  FROM dChnGroupInfo
				 WHERE parent_group_id = :parent_node;
     
	EXEC SQL	OPEN curgetChildNode;
	while(1)
	{
		memset(node, 0, sizeof(node));
		EXEC SQL	FETCH curgetChildNode INTO :node;
		if (SQLCODE != 0) break;
		Trim(node);
		strcpy(child_node[i], node);
	}
	EXEC SQL	CLOSE curgetChildNode;
	if (i == 0)
	{
		return -1;
	}
	return 0;
}


/*
 * getChildNodeList  行政树节点下检索函数
 *
 * 功能:根据输入的节点值在树中可控的深度内检索子节点,并列表返回;
 *      该函数返回的列表中需要单独取出第一个字符,其为','字符；
 *
 * input参数1: *node，       驱动节点值，最大长度10个字节；
 *
 * input参数2: NODE *array,  暂时不用，为NULL
 *
 * input参数3: max_level，   检索的深度，即检索到树中的哪层深度为止；，暂时不用
 *
 * input参数4: self，        该参数保留，整型；
 *
 * input参数5: flag，        0:节点返回前不增加任何修饰；
 *                           1:每个节点返回前左右分别增加单引号；
 *                           2:每个节点返回前左右分别增加双引号；
 *
 * input参数6: ch，          列表节点分割符；
 *
 * output参数7: *node_list,   返回节点列表；
 *
 * 返回参数:          !=0，   函数执行失败;
 *                      0，   函数执行成功,返回节点列表；
 *
 */
int getChildNodeList(char *node,NODE *array,int max_level,int self, int flag,char ch,char *node_list)
{
	char       vChild_Node[MAX_ROW][TREE_NODE_LENGTH+1]; /* 子节点数组       */
	int iRet = 0,j;
	
	memset(vChild_Node, 0, sizeof(vChild_Node));
	
	iRet = getGroupChildNode(node,array,vChild_Node);
	if (iRet != 0)
	{
		pubLog_Debug(_FFL_,"","","getChildNodeList: getGroupChildNode sub function return [%d]",iRet);
		return 3;
	}

	strcpy(node_list, "");
	for(j=0;(j<MAX_ROW && strlen(rtrim(vChild_Node[j])) > 0);j++)
	{
		if (flag == 0)
		{
			sprintf(node_list,"%s%c%s",node_list,ch,vChild_Node[j]);
		}
		else if (flag == 1)
		{
			sprintf(node_list,"%s%c\'%s\'",node_list,ch,vChild_Node[j]);
		}
		else if (flag == 2)
		{
			sprintf(node_list,"%s%c\"%s\"",node_list,ch,vChild_Node[j]);
		}
		else
		{
			sprintf(node_list,"%s%c%s",node_list,ch,vChild_Node[j]);
		}
	}

	return 0;
}



/*
 * getNodeList  行政树节点双向遍历检索函数
 *
 * 功能:根据输入的节点值在树中可控的深度内检索子节点,并列表返回;
 *
 * input参数1: *node，       驱动节点值，最大长度10个字节；
 *
 * input参数2: NODE *array,  全部节点结构体；暂时不用，为NULL

 * input参数3: min_level，   向上检索的深度；
 *
 * input参数4: max_level，   向下检索的深度；
 *
 * input参数5: self，        0:表示返回的列表中不含自身节点
 *                           1:表示返回的列表中含自身节点
 *
 * input参数6: flag，        0:节点返回前不增加任何修饰；
 *                           1:每个节点返回前左右分别增加单引号；
 *                           2:每个节点返回前左右分别增加双引号；
 *
 * input参数7: ch，          列表节点分割符；
 *
 * output参数8: *node_list， 返回节点列表；
 *
 * 返回参数:            !=0，  函数执行失败;
 *                        0，  函数执行成功，返回节点列表；
 *
 */
int getNodeList(char *node,NODE *array,int min_level,int max_level,int self,int flag,char ch,char *node_list)
{
	int  return_code;                      /* 返回值       */
	char vParent_Node_List[NODE_LIST+1];   /* 父节点列表   */
	char vChild_Node_List[NODE_LIST+1];    /* 子节点列表   */

	memset(vParent_Node_List,   0,  sizeof(vParent_Node_List));
	memset(vChild_Node_List ,   0,  sizeof(vChild_Node_List));

	return_code = getParentNodeList(node,array,min_level,self,flag,ch,vParent_Node_List);

	if (return_code != 0)
	{
		return 1;
	}

	return_code = getChildNodeList(node,array,max_level,self,flag,ch,vChild_Node_List);
	if (return_code != 0 && return_code != 1)
	{
		return 2;
	}
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,"","","vParent_Node_List=[%s]",vParent_Node_List);
	pubLog_Debug(_FFL_,"","","vChild_Node_List=[%s]",vChild_Node_List);
#endif

	if (node_list != NULL)
	{
		sprintf(node_list,"%s%s",rtrim(vParent_Node_List),rtrim(vChild_Node_List));
	}
     return 0;
}




/*
 * getClassCode   根据节点提取营业厅类型
 *
 * 功能:根据输入的节点值从sTownCode提取营业点类型
 *
 * input  参数1: *node，       节点值，最大长度10个字节；
 *
 * input  参数2: NODE *array， 暂时无用，为NULL
 *
 * output 参数2: class_code，  营业点类型，最大长度2个字节
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *
 */
/************************ 吉林dLoginMsg表没有 org_id，暂时不使用该函数
int getClassCode(char *node,NODE *array,char *class_code)
{
	char groupCode[30+1];
	char innetType[2+1];
	char groupId[10+1];
	
	memset(groupId, 0, sizeof(groupId));
	EXEC SQL	SELECT group_id
				  INTO :groupId
				  FROM dLoginMsg
				 WHERE org_id = :node;
	Trim(groupId);
	if (groupId[0] == '\0')
	{
		strcpy(groupId, node);
	}

	memset(groupCode, 0, sizeof(groupCode));
	getGroupCode(groupId, groupCode);
	Trim(groupCode);
	if (groupCode[0] != '\0')
	{
		if (strncmp(groupCode+4, "999", 3) == 0)
		{
			strcpy(class_code, "99");
			return 0; 
		}
		else
		{
			memset(innetType, 0, sizeof(innetType));
			EXEC SQL	SELECT innet_type
						  INTO :innetType
						  FROM sTownCode
						 WHERE region_code = substr(:groupCode, 1,2)
						   AND district_code = substr(:groupCode,3,2)
						   AND town_code = substr(:groupCode,5,3);
			Trim(innetType);
			if (innetType[0] == '\0')
			{
				return -1;
			}
			return 0;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
****************************************/

/*
 * getGroupParentNode   行政树父节点上单次检索函数
 *
 * 功能:根据输入的节点值在树中单次检索父节点
 *
 * input参数1:  *child_node， 子节电节点值，最大长度10个字节；
 *
 * input参数2:  NODE *array,   全部节点机构体；暂时不用
 *
 * output参数3: parent_node， 父节电节点值，最大长度10个字节；
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *                      -2: 从数据库提取节点失败；
 *
 */
int getGroupParentNode(char *child_node,NODE *array, char *parentNode)
{
	char tmpParentNode[10+1];
	int i=0;
	
	memset(tmpParentNode, 0, sizeof(tmpParentNode));
	
	EXEC SQL	SELECT parent_group_id
				  INTO :tmpParentNode
				  FROM dChnGroupInfo
				 WHERE group_id = :child_node
				   AND DENORM_LEVEL = 1;
	Trim(tmpParentNode);
	
	if (tmpParentNode[0] == '\0')
	{
		return -1;
	}
	strcpy(parentNode, tmpParentNode);
	return 0;
}



/*
 * getParentNodeList  行政树节点上检索函数
 *
 * 功能:根据输入的节点值在树中可控的深度内检索父节点,并列表返回;
 *
 * input参数1:  *node，        驱动节点值，最大长度10个字节；
 *
 * input参数2:  NODE *array,   全部节点机构体；暂时不用，为NULL
 *
 * input参数3:  min_level，    检索的深度，即检索到树中的哪层深度为止；暂时不用
 *
 * input参数4:  self，         0:表示返回的列表中不含自身节点
 *                             1:表示返回的列表中含自身节点
 *
 * input参数5:  flag，         0:节点返回前不增加任何修饰；
 *                             1:每个节点返回前左右分别增加单引号；
 *                             2:每个节点返回前左右分别增加双引号；
 *
 * input参数6:  ch，           列表节点分割符；
 *
 * output参数7: *node_list,    节点返回列表；
 *
 * 返回参数:            null，函数执行失败;
 *                      非null，返回节点列表；
 *
 */
int getParentNodeList(char *node,NODE *array,int min_level,int self,int flag,char ch,char *node_list)
{
	char groupId[10+1];
	int i=0;
	
	EXEC SQL	DECLARE curgetParentNodeList CURSOR FOR 
				SELECT parent_group_id
				  FROM dChnGroupInfo
				 WHERE group_id = :node;
     
	EXEC SQL	OPEN curgetParentNodeList;
	if (self == 1)
	{
		if (flag == 0)
		{
			sprintf(node_list,"%s%c%s",node_list,ch,node);
		}
		else if (flag == 1)
		{
			sprintf(node_list,"%s%c\'%s\'",node_list,ch,node);
		}
		else if (flag == 2)
		{
			sprintf(node_list,"%s%c\"%s\"",node_list,ch,node);
		}
		else
		{
			sprintf(node_list,"%s%c%s",node_list,ch,node);
		}
		i ++;
	}
	else
	{
		strcpy(node_list, "");
	}
	while(1)
	{
		memset(groupId, 0, sizeof(groupId));
		EXEC SQL	FETCH curgetParentNodeList INTO :groupId;
		if (SQLCODE != 0) break;
		Trim(groupId);

		if (flag == 0)
		{
			sprintf(node_list,"%s%c%s",node_list,ch,groupId);
		}
		else if (flag == 1)
		{
			sprintf(node_list,"%s%c\'%s\'",node_list,ch,groupId);
		}
		else if (flag == 2)
		{
			sprintf(node_list,"%s%c\"%s\"",node_list,ch,groupId);
		}
		else
		{
			sprintf(node_list,"%s%c%s",node_list,ch,groupId);
		}
		i ++;
	}
	EXEC SQL	CLOSE curgetParentNodeList;
	if (i == 0)
	{
		return -1;
	}
	return 0;
}


/*
 * getNodeLevel   行政树节点上深度函数
 *
 * 功能:根据输入的节点值在树中检索出该节点的深度
 *
 * 参数1: *node，         节电节点值，最大长度10个字节；
 *
 * 参数2: NODE *array     全部节点结构体；
 *
 * 返回参数:            >0: 深度值;
 *                       0: 根;
 *                      -1: 节点深度无效；
 *                      -2: 从数据库提取节点深度失败;
 *
 */
int getNodeLevel(char *node,NODE *array)
{
	int iNodeLevel = -1;
	char groupCode[30+1];
	char groupId[10+1];
	
	if (strcmp(node, "0") == 0)
	{
		return TREE_COUNTRY_LEVEL;
	}
	else if (strcmp(node, "1") == 0)
	{
		return TREE_PROVINCE_LEVEL;
	}
	
	memset(groupCode, 0, sizeof(groupCode));
	getGroupCode(node, groupCode);
	Trim(groupCode);
	
	if (groupCode[0] != '\0')
	{
		if (strncmp(groupCode+4, "999", 3) != 0)
		{
			return TREE_TOWN_LEVEL;
		}
		else if (strncmp(groupCode+2, "99", 2) != 0)
		{
			return TREE_DISTRICT_LEVEL;
		}
		else if (strncmp(groupCode, "99", 2) != 0)
		{
			return TREE_REGION_LEVEL;
		}
	}
	else
	{
#if PROVINCE_RUN!=PROVINCE_JILIN
		memset(groupId, 0, sizeof(groupId));
		EXEC SQL	SELECT group_id
					  INTO :groupId
					  FROM dLoginMsg
					 WHERE org_id = :node;
		Trim(groupId);
		/********* 吉林dLoginMsg表没有org_id，默认就是工号级别 */
#else
		strcpy(groupId,"unknown");
#endif
		if (groupId[0] == '\0')
		{
#ifdef _DEBUG_			
			pubLog_Debug(_FFL_,"","","getNodeLevel:groupCode=[%s][%s]\n", groupCode, groupId);
#endif
			return -1;
		}
		else
		{
			return TREE_LOGIN_LEVEL;
		}
	}
#ifdef _DEBUG_			
		pubLog_Debug(_FFL_,"","","getNodeLevel:2groupCode=[%s][%s]\n", groupCode, groupId);
#endif
	return -1;
}


/*
 * loadNode   将行政树的全部节点进行加载
 *
 * 功能: 将行政树的全部节点进行加载
 *
 * output参数:  NODE *node， 节点描述结构体；
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 无数据返回；
 *                      -2: 从数据库提取节点失败；
 *
 */
int loadNode(NODE *node)
{
         return 0;
}
