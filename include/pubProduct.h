#ifndef __PUB_PRODUCT_H__
#define __PUB_PRODUCT_H__

/* 地区行政管理树中各节点深度的定义 */
#define TREE_COUNTRY_LEVEL	1	/* 国家，深度为 1                */
#define TREE_PROVINCE_LEVEL	2	/* 省、自治区、直辖市，深度为 2  */
#define TREE_REGION_LEVEL	3	/* 地区、市级市，深度为 3        */
#define TREE_DISTRICT_LEVEL	4	/* 区县、县级市，深度为 4        */
#define TREE_TOWN_LEVEL		5	/* 网点、代销点，深度为 5        */
#define TREE_LOGIN_LEVEL	6	/* 营业员，深度为 6              */
#define TREE_NODE_LENGTH	10	/* 树节点的长度                  */

#define NO_QUOTATION		0	/* 不含任何修饰返回节点列表      */
#define SINGLE_QUOTATION	1	/* 节点单引后返回节点列表        */
#define DOUBLE_QUOTATION	2	/* 节点双引后返回节点列表        */

#define MAX_ROW			1024		/* 产品/服务最大行数             */
#define MAX_COL			100		/* 产品/服务最大列数             */
#define NODE_LIST		8912	/* 节点返回数组长度              */
#define NODE_NUM        	30000   /* 全部节点个数                  */

#define MAXENDTIME			"20500101 00:00:00"
#define MAX_PRODUCT_PRICES		500
#define MAX_SERVICE_ATTR_COUNT	50
#define DEFAULT_PRODUCT_CODE		"ZZZZZZZZ"
#define DEFAULT_GRPTOPERSON_CODE	"af000200"


struct product
{
   char  vProduct_Code[8+1];    	/* 产品代码        */
   char  vProduct_Name[20+1];   	/* 产品名称        */
   float vPrepay_Least;         	/* 预存约束        */
   float vDeposit_Least;        	/* 押金约束        */
   float vMark_Least;           	/* 积分约束        */
   char  vProduct_Note[100+1];  	/* 产品描述        */
   char  vOld_Sel[1+1];                 /* 原选择标志      */
   char  vNew_Sel[1+1];                 /* 新选择标志      */
   char  vMean_List[1024+1];            /* 因子属性        */
   char  vProduct_Attr[2048+1];         /* 产品属性        */
   char  vLogin_Accept[14+1];           /* 操作流水        */
   char  vBegin_Time[17+1];             /* 开始时间        */
   char  vEnd_Time[17+1];               /* 结束时间        */
   char  vBak_Field[10+1];              /* 保留字段        */
};

typedef struct product PRODUCT;

struct service
{

    char   vProduct_Code[8+1];		/* 产品代码     */
    char   vProduct_Name[20+1];		/* 产品名称     */
    char   vService_Code[4+1];		/* 服务代码     */
    char   vService_Name[20+1];		/* 服务名称     */
    char   vPrice_Code[4+1];      	/* 价格代码     */
    char   vPrice_Modify[1+1];   	/* 个性定价     */
    float  vPrepay_Least;         	/* 预存约束     */
    float  vDeposit_Least;        	/* 押金约束     */
    float  vMark_Least;           	/* 积分约束     */
    char   vService_Note[100+1];  	/* 服务说明     */
    char   vOld_Sel[1+1];               /* 原选择标志   */
    char   vNew_Sel[1+1];               /* 新选择标志   */
    char   vServ_Attr[2048+1];          /* 产品属性     */
    char   vAdd_No[2048+1];             /* 附加号码属性 */
    char   vServ_Modify[1+1];           /* 服务属性     *
                                         * 必须输入标志 */
    char   vLogin_Accept[14+1];         /* 操作流水     */
    char   vBegin_Time[17+1];           /* 开始时间     */
    char   vEnd_Time[17+1];             /* 结束时间     */
    char   vBak_Field[10+1];            /* 保留字段     */
};

typedef struct service SERVICE;

struct pricecfg
{

    char   vProduct_Code[8+1];		/* 产品代码     */
    char   vProduct_Name[20+1];		/* 产品名称     */
    char   vService_Type[1+1];    	/* 服务类型     */
    char   vService_Code[4+1];		/* 服务代码     */
    char   vService_Name[20+1];		/* 服务名称     */
    char   vPrice_Code[4+1];      	/* 价格代码     */
    char   vPrice_Name[20+1];     	/* 价格名称     */
    char   vPrice_Mean[4+1];      	/* 价格因子     */
    char   vMean_Type[1+1];       	/* 因子类型     */
    char   vMean_Name[20+1];      	/* 因子名称     */
    char   vPrice_Type[1+1];      	/* 取值模式     */
    float  vPrice_Value;          	/* 价格值       */
    int    vMean_Order;           	/* 因子顺序     */
    char   vTime_Flag[1+1];       	/* 时间标志     */
    float  vMin_Prepay;           	/* 最小预存款   */
    float  vMax_Prepay;           	/* 最大预存款   */
    float  vMin_Deposit;          	/* 最小押金     */
    float  vMax_Deposit;          	/* 最大押金     */
};

typedef struct pricecfg PRICECFG;

struct srvattr
{

    char   vService_Code[4+1];		/* 服务代码     */
    char   vService_Name[20+1];		/* 服务名称     */
    char   vAttr_Type[1+1];   	 	/* 属性类型     */
    char   vAttr_Code[2+1];      	/* 属性代码     */
    char   vAttr_Name[20+1];     	/* 属性名称     */
    char   vAllow_Flag[1+1];      	/* 是否允许     *
    								 * 输入属性值   */
    char   vValue_Format[2+1];		/* 属性值格式   */
};

typedef struct srvattr SRVATTR;

struct prodattr
{

    char   vProduct_Attr[8+1];		/* 产品属性     */
    char   vAttr_Type[1+1];   	 	/* 属性类型     */
    char   vAttr_Code[2+1];      	/* 属性代码     */
    char   vAttr_Name[20+1];     	/* 属性名称     */
    char   vIs_Bill[1+1];      	    /* 集团本身     *
    								 * 是否计费     */
};

typedef struct prodattr PRODATTR;

struct node
{

    char   vNode[10+1];				/* 子节点     */
    char   vParent_Node[10+1];   	/* 父节点     */
    int    vNode_Level;             /* 节点深度   */
};

typedef struct node NODE;

typedef struct tagProductPrices
{
	char	serviceCode[4+1];
	char	priceCode[4+1];
	char	priceMean[4+1];
	char	meanOrder[11+1];
	char	priceType[1+1];
	char	priceValue[11+1];

} TProductPrices;

#define SRV_ATTR_BEGIN_TIME		"10"
#define SRV_ATTR_END_TIME		"11"
#define SRV_ATTR_DATA_NO		"13"
#define SRV_ATTR_FAX_NO			"14"
#define SRV_ATTR_SEEK_NO		"15"
#define SRV_ATTR_AREA_NO		"04"
#define SRV_ATTR_GROUP_NO		"02"

typedef struct tagServiceAttr
{
	char	serviceCode[4+1];
	char	attrType[1+1];
	char	serviceAttr[2+1];
	char	attrValue[20+1];
} TServiceAttr;

#endif /*__PUB_PRODUCT_H__*/
