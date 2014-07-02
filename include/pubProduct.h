#ifndef __PUB_PRODUCT_H__
#define __PUB_PRODUCT_H__

/* ���������������и��ڵ���ȵĶ��� */
#define TREE_COUNTRY_LEVEL	1	/* ���ң����Ϊ 1                */
#define TREE_PROVINCE_LEVEL	2	/* ʡ����������ֱϽ�У����Ϊ 2  */
#define TREE_REGION_LEVEL	3	/* �������м��У����Ϊ 3        */
#define TREE_DISTRICT_LEVEL	4	/* ���ء��ؼ��У����Ϊ 4        */
#define TREE_TOWN_LEVEL		5	/* ���㡢�����㣬���Ϊ 5        */
#define TREE_LOGIN_LEVEL	6	/* ӪҵԱ�����Ϊ 6              */
#define TREE_NODE_LENGTH	10	/* ���ڵ�ĳ���                  */

#define NO_QUOTATION		0	/* �����κ����η��ؽڵ��б�      */
#define SINGLE_QUOTATION	1	/* �ڵ㵥���󷵻ؽڵ��б�        */
#define DOUBLE_QUOTATION	2	/* �ڵ�˫���󷵻ؽڵ��б�        */

#define MAX_ROW			1024		/* ��Ʒ/�����������             */
#define MAX_COL			100		/* ��Ʒ/�����������             */
#define NODE_LIST		8912	/* �ڵ㷵�����鳤��              */
#define NODE_NUM        	30000   /* ȫ���ڵ����                  */

#define MAXENDTIME			"20500101 00:00:00"
#define MAX_PRODUCT_PRICES		500
#define MAX_SERVICE_ATTR_COUNT	50
#define DEFAULT_PRODUCT_CODE		"ZZZZZZZZ"
#define DEFAULT_GRPTOPERSON_CODE	"af000200"


struct product
{
   char  vProduct_Code[8+1];    	/* ��Ʒ����        */
   char  vProduct_Name[20+1];   	/* ��Ʒ����        */
   float vPrepay_Least;         	/* Ԥ��Լ��        */
   float vDeposit_Least;        	/* Ѻ��Լ��        */
   float vMark_Least;           	/* ����Լ��        */
   char  vProduct_Note[100+1];  	/* ��Ʒ����        */
   char  vOld_Sel[1+1];                 /* ԭѡ���־      */
   char  vNew_Sel[1+1];                 /* ��ѡ���־      */
   char  vMean_List[1024+1];            /* ��������        */
   char  vProduct_Attr[2048+1];         /* ��Ʒ����        */
   char  vLogin_Accept[14+1];           /* ������ˮ        */
   char  vBegin_Time[17+1];             /* ��ʼʱ��        */
   char  vEnd_Time[17+1];               /* ����ʱ��        */
   char  vBak_Field[10+1];              /* �����ֶ�        */
};

typedef struct product PRODUCT;

struct service
{

    char   vProduct_Code[8+1];		/* ��Ʒ����     */
    char   vProduct_Name[20+1];		/* ��Ʒ����     */
    char   vService_Code[4+1];		/* �������     */
    char   vService_Name[20+1];		/* ��������     */
    char   vPrice_Code[4+1];      	/* �۸����     */
    char   vPrice_Modify[1+1];   	/* ���Զ���     */
    float  vPrepay_Least;         	/* Ԥ��Լ��     */
    float  vDeposit_Least;        	/* Ѻ��Լ��     */
    float  vMark_Least;           	/* ����Լ��     */
    char   vService_Note[100+1];  	/* ����˵��     */
    char   vOld_Sel[1+1];               /* ԭѡ���־   */
    char   vNew_Sel[1+1];               /* ��ѡ���־   */
    char   vServ_Attr[2048+1];          /* ��Ʒ����     */
    char   vAdd_No[2048+1];             /* ���Ӻ������� */
    char   vServ_Modify[1+1];           /* ��������     *
                                         * ���������־ */
    char   vLogin_Accept[14+1];         /* ������ˮ     */
    char   vBegin_Time[17+1];           /* ��ʼʱ��     */
    char   vEnd_Time[17+1];             /* ����ʱ��     */
    char   vBak_Field[10+1];            /* �����ֶ�     */
};

typedef struct service SERVICE;

struct pricecfg
{

    char   vProduct_Code[8+1];		/* ��Ʒ����     */
    char   vProduct_Name[20+1];		/* ��Ʒ����     */
    char   vService_Type[1+1];    	/* ��������     */
    char   vService_Code[4+1];		/* �������     */
    char   vService_Name[20+1];		/* ��������     */
    char   vPrice_Code[4+1];      	/* �۸����     */
    char   vPrice_Name[20+1];     	/* �۸�����     */
    char   vPrice_Mean[4+1];      	/* �۸�����     */
    char   vMean_Type[1+1];       	/* ��������     */
    char   vMean_Name[20+1];      	/* ��������     */
    char   vPrice_Type[1+1];      	/* ȡֵģʽ     */
    float  vPrice_Value;          	/* �۸�ֵ       */
    int    vMean_Order;           	/* ����˳��     */
    char   vTime_Flag[1+1];       	/* ʱ���־     */
    float  vMin_Prepay;           	/* ��СԤ���   */
    float  vMax_Prepay;           	/* ���Ԥ���   */
    float  vMin_Deposit;          	/* ��СѺ��     */
    float  vMax_Deposit;          	/* ���Ѻ��     */
};

typedef struct pricecfg PRICECFG;

struct srvattr
{

    char   vService_Code[4+1];		/* �������     */
    char   vService_Name[20+1];		/* ��������     */
    char   vAttr_Type[1+1];   	 	/* ��������     */
    char   vAttr_Code[2+1];      	/* ���Դ���     */
    char   vAttr_Name[20+1];     	/* ��������     */
    char   vAllow_Flag[1+1];      	/* �Ƿ�����     *
    								 * ��������ֵ   */
    char   vValue_Format[2+1];		/* ����ֵ��ʽ   */
};

typedef struct srvattr SRVATTR;

struct prodattr
{

    char   vProduct_Attr[8+1];		/* ��Ʒ����     */
    char   vAttr_Type[1+1];   	 	/* ��������     */
    char   vAttr_Code[2+1];      	/* ���Դ���     */
    char   vAttr_Name[20+1];     	/* ��������     */
    char   vIs_Bill[1+1];      	    /* ���ű���     *
    								 * �Ƿ�Ʒ�     */
};

typedef struct prodattr PRODATTR;

struct node
{

    char   vNode[10+1];				/* �ӽڵ�     */
    char   vParent_Node[10+1];   	/* ���ڵ�     */
    int    vNode_Level;             /* �ڵ����   */
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
