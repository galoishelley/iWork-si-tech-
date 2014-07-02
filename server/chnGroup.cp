/**********���������group_id,org_id�ֶ�,���������������켰ͳһ��־����************/
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
 *     �õ������ڵ�֮��Ĺ�ϵ.
 *     �����������û���dChnGroupInfo��������ͬ���dChnGroupInfoChn������Դ����
 * ���е�dChnGroupInfo�����ֿ���
 * @author	lugz
 * @inparam	nodeSrc			Դ�ڽڵ�
 * @inparam	nodeDest		Ŀ��ڵ�
 * @return 0����ʾ���ڵ�֮��û�й�ϵ
 *         1: ��ʾԴ�ڵ�͸��ڵ���ͬһ���ڵ�
 *         2: ��ʾԴ�ڵ���Ŀ��ڵ�ĸ��ڵ�
 *         3����ʾԴ�ڵ���Ŀ��ڵ���ӽڵ�
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
 * getGroupCode   ���ݽڵ���ȡ��������
 *
 * ����:��������Ľڵ�ֵ�����д�sRegionCode,sDisCode,sTownCode����
 *      ��ȡ��������
 *
 * input  ����1: *node��       �ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * output ����2: group_code��  �������룬��󳤶�7���ֽ�
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �ڵ���Ч��
 *                      -2: �����ݿ���ȡ�ڵ�ʧ�ܣ�
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
 * chkNodeRelation    ������ڵ�֮���Ƿ���ڸ��ӹ�ϵ����
 *
 * ����:������ڵ�֮���Ƿ���ڸ��ӹ�ϵ����
 *
 * input����1: *node1��      �ڵ�1����󳤶�10���ֽڣ�
 *
 * input����2: *node2��      �ڵ�2����󳤶�10���ֽڣ�
 *
 *
 * ���ز���:             0: ������;
 *                       1: ������
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
 * chkNodeAgentRelation    ������ڵ�֮���Ƿ���ں�����ϵ����
 *
 * ����:������ڵ�֮���Ƿ���ں�����ϵ����
 *
 * input����1: *loginGrpId�� ���Ź����ڵ�     �ڵ�1����󳤶�10���ֽڣ�
 *
 * input����2: *phoneGrpId�� ��������ڵ�     �ڵ�2����󳤶�10���ֽڣ�
 *
 *
 * ���ز���:             0: ������;
 *                       1: ������
 *                      <0: ORACLE����
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
 * chkNodeAgentType    ��鹤�ŵĹ����ڵ��Ƿ����Ӫҵ��
 *
 * ����:��鹤�ŵĹ����ڵ��Ƿ����Ӫҵ��
 *
 * input����1: *node1�� ���Ź����ڵ�     �ڵ�1����󳤶�10���ֽڣ�
 * input����2: *array ��ʱ����
 *
 * ���ز���:             0: �԰�Ӫҵ��;
 *                       1: ����Ӫҵ����
 *                       2: ��ȡ�ڵ����ʧ��;
 *                      <0: ORACLE����
 *
 */ 
 /***********************  ����sTownCode��û��group_id����ʱ���øú���
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
 * getGroupName   ���ݽڵ���ȡ�ڵ���߸��ڵ�����
 *
 * ����:��������Ľڵ�ֵ�������жϽڵ���ȣ���������Ӧ�Ľڵ�����
 *
 * input  ����1: *node��       �ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input  ����2: level         ��ʱ����;
 *
 * input  ����3: NODE *        ��ʱ����;
 *
 * output ����4: group_name��  �ڵ�����
 *
 *
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �ڵ���Ч��
 *                      -2: �����ݿ���ȡ�ڵ�ʧ�ܣ�
 *
 */

int getGroupName(char *node,int level,NODE *array, char *group_name)
{
	char groupName[60+1];                  /* �ڵ�����        */
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
 * �õ�����GRP ID
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
 *     �õ���Ʒ��������С��group_id
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
 * getGroupChildNode   �������ӽڵ��µ��μ�������
 *
 * ����:��������Ľڵ�ֵ�����е��μ����ӽڵ�
 *
 * input����1:   *parent_node�� ���ڵ�ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input����2:   NODE *array,   ��ʱ���ã���ΪNULL
 *
 * output����3:  child_node[][TREE_NODE_LENGTH+1]
 *                            �ֽڵ����飻
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �ڵ���Ч��
 *                      -2: �����ݿ���ȡ�ڵ�ʧ�ܣ�
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
 * getChildNodeList  �������ڵ��¼�������
 *
 * ����:��������Ľڵ�ֵ�����пɿص�����ڼ����ӽڵ�,���б���;
 *      �ú������ص��б�����Ҫ����ȡ����һ���ַ�,��Ϊ','�ַ���
 *
 * input����1: *node��       �����ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input����2: NODE *array,  ��ʱ���ã�ΪNULL
 *
 * input����3: max_level��   ��������ȣ������������е��Ĳ����Ϊֹ������ʱ����
 *
 * input����4: self��        �ò������������ͣ�
 *
 * input����5: flag��        0:�ڵ㷵��ǰ�������κ����Σ�
 *                           1:ÿ���ڵ㷵��ǰ���ҷֱ����ӵ����ţ�
 *                           2:ÿ���ڵ㷵��ǰ���ҷֱ�����˫���ţ�
 *
 * input����6: ch��          �б�ڵ�ָ����
 *
 * output����7: *node_list,   ���ؽڵ��б�
 *
 * ���ز���:          !=0��   ����ִ��ʧ��;
 *                      0��   ����ִ�гɹ�,���ؽڵ��б�
 *
 */
int getChildNodeList(char *node,NODE *array,int max_level,int self, int flag,char ch,char *node_list)
{
	char       vChild_Node[MAX_ROW][TREE_NODE_LENGTH+1]; /* �ӽڵ�����       */
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
 * getNodeList  �������ڵ�˫�������������
 *
 * ����:��������Ľڵ�ֵ�����пɿص�����ڼ����ӽڵ�,���б���;
 *
 * input����1: *node��       �����ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input����2: NODE *array,  ȫ���ڵ�ṹ�壻��ʱ���ã�ΪNULL

 * input����3: min_level��   ���ϼ�������ȣ�
 *
 * input����4: max_level��   ���¼�������ȣ�
 *
 * input����5: self��        0:��ʾ���ص��б��в�������ڵ�
 *                           1:��ʾ���ص��б��к�����ڵ�
 *
 * input����6: flag��        0:�ڵ㷵��ǰ�������κ����Σ�
 *                           1:ÿ���ڵ㷵��ǰ���ҷֱ����ӵ����ţ�
 *                           2:ÿ���ڵ㷵��ǰ���ҷֱ�����˫���ţ�
 *
 * input����7: ch��          �б�ڵ�ָ����
 *
 * output����8: *node_list�� ���ؽڵ��б�
 *
 * ���ز���:            !=0��  ����ִ��ʧ��;
 *                        0��  ����ִ�гɹ������ؽڵ��б�
 *
 */
int getNodeList(char *node,NODE *array,int min_level,int max_level,int self,int flag,char ch,char *node_list)
{
	int  return_code;                      /* ����ֵ       */
	char vParent_Node_List[NODE_LIST+1];   /* ���ڵ��б�   */
	char vChild_Node_List[NODE_LIST+1];    /* �ӽڵ��б�   */

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
 * getClassCode   ���ݽڵ���ȡӪҵ������
 *
 * ����:��������Ľڵ�ֵ��sTownCode��ȡӪҵ������
 *
 * input  ����1: *node��       �ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input  ����2: NODE *array�� ��ʱ���ã�ΪNULL
 *
 * output ����2: class_code��  Ӫҵ�����ͣ���󳤶�2���ֽ�
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �ڵ���Ч��
 *
 */
/************************ ����dLoginMsg��û�� org_id����ʱ��ʹ�øú���
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
 * getGroupParentNode   ���������ڵ��ϵ��μ�������
 *
 * ����:��������Ľڵ�ֵ�����е��μ������ڵ�
 *
 * input����1:  *child_node�� �ӽڵ�ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input����2:  NODE *array,   ȫ���ڵ�����壻��ʱ����
 *
 * output����3: parent_node�� ���ڵ�ڵ�ֵ����󳤶�10���ֽڣ�
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �ڵ���Ч��
 *                      -2: �����ݿ���ȡ�ڵ�ʧ�ܣ�
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
 * getParentNodeList  �������ڵ��ϼ�������
 *
 * ����:��������Ľڵ�ֵ�����пɿص�����ڼ������ڵ�,���б���;
 *
 * input����1:  *node��        �����ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * input����2:  NODE *array,   ȫ���ڵ�����壻��ʱ���ã�ΪNULL
 *
 * input����3:  min_level��    ��������ȣ������������е��Ĳ����Ϊֹ����ʱ����
 *
 * input����4:  self��         0:��ʾ���ص��б��в�������ڵ�
 *                             1:��ʾ���ص��б��к�����ڵ�
 *
 * input����5:  flag��         0:�ڵ㷵��ǰ�������κ����Σ�
 *                             1:ÿ���ڵ㷵��ǰ���ҷֱ����ӵ����ţ�
 *                             2:ÿ���ڵ㷵��ǰ���ҷֱ�����˫���ţ�
 *
 * input����6:  ch��           �б�ڵ�ָ����
 *
 * output����7: *node_list,    �ڵ㷵���б�
 *
 * ���ز���:            null������ִ��ʧ��;
 *                      ��null�����ؽڵ��б�
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
 * getNodeLevel   �������ڵ�����Ⱥ���
 *
 * ����:��������Ľڵ�ֵ�����м������ýڵ�����
 *
 * ����1: *node��         �ڵ�ڵ�ֵ����󳤶�10���ֽڣ�
 *
 * ����2: NODE *array     ȫ���ڵ�ṹ�壻
 *
 * ���ز���:            >0: ���ֵ;
 *                       0: ��;
 *                      -1: �ڵ������Ч��
 *                      -2: �����ݿ���ȡ�ڵ����ʧ��;
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
		/********* ����dLoginMsg��û��org_id��Ĭ�Ͼ��ǹ��ż��� */
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
 * loadNode   ����������ȫ���ڵ���м���
 *
 * ����: ����������ȫ���ڵ���м���
 *
 * output����:  NODE *node�� �ڵ������ṹ�壻
 *
 *
 * ���ز���:             0: �ɹ�;
 *                      -1: �����ݷ��أ�
 *                      -2: �����ݿ���ȡ�ڵ�ʧ�ܣ�
 *
 */
int loadNode(NODE *node)
{
         return 0;
}
