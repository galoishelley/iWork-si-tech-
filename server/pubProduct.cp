/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/*******已完成组织结构二期改造 edit by  zhaoqm 07/05/2009 ****/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "funcParam.h"
#include "pubProduct.h"
#include "province.h"

#define SQLCODE  sqlca.sqlcode

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

/*
 * getBusinessCode   根据模块代码提取业务规则代码
 *
 * 功能:根据模块代码提取业务规则代码
 *
 * input  参数1: *function_code，模块代码，最大长度5个字节；
 *
 * output 参数2: business_code， 业务规则代码，最大长度1个字节
 *
 *
 * 返回参数:            -1: 读取ORACLE数据失败
 *                       0: 成功;
 *                       1: 数据无效；
 *                       2: 记录重叠；
 *
 */
int getBusinessCode(char *function_code,char *business_code)
{
       char vFunction_Code[5+1];                /* 模块代码        */
       char vBusiness_Code[1+1];                /* 业务规则代码    */

       char vSQL_Text[1024+1];                  /* SQL语句         */
       char vTmp_String[1024+1];                /* 临时变量        */
       int  vNum=0;                             /* 计数器          */


       int  vret_code;                          /* 返回值          */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: Start Execute");
#endif

     memset(vFunction_Code,0, sizeof(vFunction_Code));
     memset(vBusiness_Code,0, sizeof(vBusiness_Code));

     memset(vSQL_Text,     0, sizeof(vSQL_Text));
     memset(vTmp_String,   0, sizeof(vTmp_String));
     memset(&vNum,         0, sizeof(vNum));
     vret_code           = 0;


     strncpy(vFunction_Code,   function_code,  5);
     vFunction_Code[5] = '\0';

     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Trim(Business_Code) FROM sFuncBusiness ",vTmp_String);

     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Function_Code='%s' ",vTmp_String,vFunction_Code);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: vSQL_Text=[%s]",vSQL_Text);
#endif


     memset(vSQL_Text,   0, sizeof(vSQL_Text));
     strcat(vSQL_Text," SELECT Trim(Business_Code) FROM sFuncBusiness ");
     strcat(vSQL_Text," WHERE Function_Code=:v1 ");


#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: vSQL_Text=[%s]",vSQL_Text);
#endif
	 pubLog_Debug(_FFL_, "getBusinessCode", "", "vFunction_Code = [%s]",vFunction_Code);

     EXEC SQL PREPARE Prepare_sFuncBusiness FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_sFuncBusiness CURSOR FOR Prepare_sFuncBusiness;
     EXEC SQL OPEN Cursor_sFuncBusiness using :vFunction_Code;
     for(;;)
     {
            EXEC SQL FETCH Cursor_sFuncBusiness INTO :vBusiness_Code;

            if (SQLCODE == NOTFOUND )
                  break;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: 根据被检索节点提取营业厅类型失败,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sFuncBusiness;
                  return -1;
            }

            vNum++;
     }
     EXEC SQL CLOSE Cursor_sFuncBusiness;

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: Business_Code=[%s]",vBusiness_Code);
#endif
	 pubLog_Debug(_FFL_, "getBusinessCode", "", "vNum = [%d]");
     if (vNum > 1)
        return 2;

	 pubLog_Debug(_FFL_, "getBusinessCode", "", "vBusiness_Code = [%s]",vBusiness_Code);
     if (strlen(rtrim(vBusiness_Code)) <= 0)
         return 1;
     else
     {
         if (business_code != NULL)
         {
             strncpy(business_code,rtrim(vBusiness_Code),strlen(rtrim(vBusiness_Code)));
             business_code[strlen(rtrim(vBusiness_Code))]='\0';
         }
     }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getBusinessCode", "", "getBusinessCode: End Execute");
#endif

     return 0;
}

/*
 * getAccPrepay   根据帐户Id提取该帐户的预存款
 *
 * 功能:根据帐户Id提取该帐户的预存款
 *
 * input  参数1: *account_no，        帐户Id，最大长度14个字节；
 *
 * output 参数2: float *prepay_fee，  预存款
 *
 *
 *
 *
 * 返回参数:            <0: 读取数据库信息报错;
 *                       0: 成功；
 *
 */

int getAccPrepay(char *account_no,float *prepay_fee)
{

       char  vAccount_No[14+1];                  /* 帐户Id          */
       float vPrepay_Fee;                        /* 用户预存款      */

       char  vSQL_Text[1024+1];                 /* SQL语句1        */

       char  vTmp_String[1024+1];                /* 临时变量        */
       int   vret_code=0;                        /* 返回值          */



#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: Start Execute");
#endif

     memset(vAccount_No,   0, sizeof(vAccount_No));
     memset(&vPrepay_Fee,  0, sizeof(vPrepay_Fee));
     memset(vSQL_Text,     0, sizeof(vSQL_Text));
     memset(vTmp_String,   0, sizeof(vTmp_String));
     memset(&vret_code,    0, sizeof(vret_code));



     strncpy(vAccount_No,   account_no,  strlen(account_no));
     vAccount_No[14] = '\0';


     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT NVL(Prepay_Fee,0.00) FROM dConMsgPre ",vTmp_String);

     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Contract_No = To_Number(Trim('%s')) ",vTmp_String,vAccount_No);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: vSQL_Text=[%s]",vSQL_Text);
#endif

     memset(vSQL_Text,  0, sizeof(vSQL_Text));
     strcat(vSQL_Text," SELECT NVL(Prepay_Fee,0.00) FROM dConMsgPre ");
     strcat(vSQL_Text," WHERE Contract_No = To_Number(Trim(:v1)) ");


#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: vSQL_Text=[%s]",vSQL_Text);
#endif

     EXEC SQL PREPARE Prepare_dConMsgPre FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_dConMsgPre CURSOR FOR Prepare_dConMsgPre;
     EXEC SQL OPEN Cursor_dConMsgPre using :vAccount_No;
     for(;;)
     {
            EXEC SQL FETCH Cursor_dConMsgPre INTO :vPrepay_Fee;

            if (SQLCODE == NOTFOUND )
                  break;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: 提取帐户[%s]的的预存款失败,SQLCODE=[%d]!",vAccount_No,SQLCODE);
                  EXEC SQL CLOSE Cursor_dConMsgPre;
                  return -1;
            }
     }
     EXEC SQL CLOSE Cursor_dConMsgPre;

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: vPrepay_Fee=[%10.2f]",vPrepay_Fee);
#endif

     if (prepay_fee != NULL)
     {
          *prepay_fee = vPrepay_Fee;
     }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAccPrepay", "", "getAccPrepay: End Execute");
#endif

     return 0;
}


/*
 * getUserPrepay   根据用户Id提取该用户的预存款
 *
 * 功能:根据用户Id提取该用户的预存款
 *
 * input  参数1: *id_no，             用户Id，最大长度14个字节；
 *
 * output 参数2: float *prepay_fee，  预存款
 *
 *
 *
 *
 * 返回参数:            <0: 读取数据库信息报错;
 *                       0: 成功；
 *                       1: 提取帐户的预存款失败
 *
 */

int getUserPrepay(char *id_no,float *prepay_fee)
{

       char  vId_No[14+1];                       /* 用户Id          */
       float vPrepay_Fee;                        /* 用户预存款      */

       char  vSQL_Text[1024+1];                 /* SQL语句1        */

       char  vTmp_String[1024+1];                /* 临时变量        */
       float vTmp_Prepay;                        /* 帐户预存款      */
       char  vAccount_No[14+1];                  /* 帐户            */
       int   vret_code=0;                        /* 返回值          */



#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: Start Execute");
#endif

     memset(vId_No,        0, sizeof(vId_No));
     memset(&vPrepay_Fee,  0, sizeof(vPrepay_Fee));
     memset(vSQL_Text,     0, sizeof(vSQL_Text));
     memset(vTmp_String,   0, sizeof(vTmp_String));
     memset(&vTmp_Prepay,  0, sizeof(vTmp_Prepay));
     memset(vAccount_No,   0, sizeof(vAccount_No));
     memset(&vret_code,    0, sizeof(vret_code));



     strncpy(vId_No,   id_no,  strlen(id_no));
     vId_No[14] = '\0';


     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT To_Char(Contract_No) FROM dConUserMsg ",vTmp_String);

     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Id_No = To_Number(Trim('%s')) ",vTmp_String,vId_No);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: vSQL_Text=[%s]",vSQL_Text);
#endif

     memset(vSQL_Text,   0, sizeof(vSQL_Text));
     strcpy(vSQL_Text," SELECT To_Char(Contract_No) FROM dConUserMsg ");
     strcat(vSQL_Text," WHERE Id_No = To_Number(Trim(:v1)) ");


#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: vSQL_Text=[%s]",vSQL_Text);
#endif

     EXEC SQL PREPARE Prepare_dConUserMsg FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_dConUserMsg CURSOR FOR Prepare_dConUserMsg;
     EXEC SQL OPEN Cursor_dConUserMsg using :vId_No;
     for(;;)
     {
            EXEC SQL FETCH Cursor_dConUserMsg INTO :vAccount_No;

            if (SQLCODE == NOTFOUND )
                  break;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: 提取用户[%s]的帐户失败,SQLCODE=[%d]!",vId_No,SQLCODE);
                  EXEC SQL CLOSE Cursor_dConUserMsg;
                  return -1;
            }

            memset(&vTmp_Prepay,   0,  sizeof(vTmp_Prepay));

            vret_code = getAccPrepay(vAccount_No,&vTmp_Prepay);

            if (vret_code != 0)
            {
                  pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: 提取帐户[%s]的预存款失败,SQLCODE=[%d]!",vAccount_No,SQLCODE);
                  pubLog_Debug(_FFL_, "getUserPrepay", "", "getAccPrepay:  return_code=[%d]!",vret_code);
                  EXEC SQL CLOSE Cursor_dConUserMsg;
                  return 1;
            }

            vPrepay_Fee += vTmp_Prepay;
     }
     EXEC SQL CLOSE Cursor_dConUserMsg;

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: vPrepay_Fee=[%10.2f]",vPrepay_Fee);
#endif

     if (prepay_fee != NULL)
     {
          *prepay_fee = vPrepay_Fee;
     }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserPrepay", "", "getUserPrepay: End Execute");
#endif

     return 0;
}

/*
 * getUserPrepay   根据用户Id提取该用户的预存款
 *
 * 功能:根据用户Id提取该用户的预存款
 *
 * input  参数1: *id_no，             用户Id，最大长度14个字节；
 *
 * output 参数2: float *prepay_fee，  预存款
 *
 *
 *
 *
 * 返回参数:            <0: 读取数据库信息报错;
 *                       0: 成功；
 *                       1: 用户在dMarkMsg表记录重叠；
 *zhoubd 20061221 屏蔽

int getUserMark(char *id_no,float *mark)
{

       char  vId_No[14+1];
       float vMark;

       char  vSQL_Text[1024+1];

       char  vTmp_String[1024+1];
       int   vret_code=0;
       int   vNum=0;



#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: Start Execute");
#endif

     memset(vId_No,        0, sizeof(vId_No));
     memset(&vMark,        0, sizeof(vMark));
     memset(vSQL_Text,     0, sizeof(vSQL_Text));
     memset(vTmp_String,   0, sizeof(vTmp_String));
     memset(&vret_code,    0, sizeof(vret_code));
     memset(&vNum,         0, sizeof(vNum));



     strncpy(vId_No,   id_no,  strlen(id_no));
     vId_No[14] = '\0';


     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Nvl(Current_Point,0.00) FROM dMarkMsg ",vTmp_String);

     strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Id_No = To_Number(Trim('%s')) ",vTmp_String,vId_No);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: vSQL_Text=[%s]",vSQL_Text);
#endif

     memset(vSQL_Text,   0, sizeof(vSQL_Text));
     strcpy(vSQL_Text," SELECT Nvl(Current_Point,0.00) FROM dMarkMsg ");
     strcat(vSQL_Text," WHERE Id_No = To_Number(Trim(:v1)) ");


#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: vSQL_Text=[%s]",vSQL_Text);
#endif

     EXEC SQL PREPARE Prepare_dCustMark FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_dCustMark CURSOR FOR Prepare_dCustMark;
     EXEC SQL OPEN Cursor_dCustMark using :vId_No;
     for(;;)
     {
            EXEC SQL FETCH Cursor_dCustMark INTO :vMark;

            if (SQLCODE == NOTFOUND )
                  break;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: 提取用户[%s]的当前积分失败,SQLCODE=[%d]!",vId_No,SQLCODE);
                  EXEC SQL CLOSE Cursor_dCustMark;
                  return -1;
            }

            vNum++;
     }
     EXEC SQL CLOSE Cursor_dCustMark;

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: vCust_Mark=[%10.2f]",vMark);
#endif


     if (vNum > 1)
        return 1;


     if (mark != NULL)
     {
          *mark = vMark;
     }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: End Execute");
#endif

     return 0;
}
 */

/*
 * getUserMark   根据用户Id提取该用户的积分
 *
 * 功能:根据用户Id提取该用户的预存款
 *
 * input  参数1: *id_no，             用户Id，最大长度14个字节；
 *
 * output 参数2: float *mark  积分
 *
 *
 *
 *
 * 返回参数:            <0: 读取数据库信息报错;
 *                       0: 成功；
 *zhoubd 20061221 修改开始
 */

int getUserMark(char *id_no, float *mark)
{
	char retCode[6+1];
	char retMsg[255+1];

	double currentPoint = 0;
	long contract_no = 0;
	
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: Start Execute");
#endif

	memset(retCode, 0, sizeof(retCode));
	memset(retMsg, 0, sizeof(retMsg));

	if (id_no == 0 || mark == 0)
	{
		pubLog_Debug(_FFL_, "getUserMark", "", "输入参数错误！");
		return -1;
	}

	if ( markGetConByIdno(*id_no, &contract_no, retCode, retMsg) < 0)
	{
		pubLog_Debug(_FFL_, "getUserMark", "", "获取用户账号错误！[id_no:%ld],[retCode:%s, retMsg:%s]", *id_no, retCode, retMsg);
		return -1;
	}

	EXEC SQL select current_point into :currentPoint from dconmark where contract_no = :contract_no;
	if (SQLCODE != 0)
	{
		pubLog_Debug(_FFL_, "getUserMark", "", "获取用户积分错误！[id_no:%ld],[SQLCODE:%d]", *id_no, SQLCODE);
		return -1;
	}

	*mark = currentPoint;
	
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getUserMark", "", "getUserMark: End Execute");
#endif

	return 0;
}
 /*zhoubd 20061221 修改结束*/



/*
 * getMaxLineOfProduct  计算产品结构已使用的最大行数；
 *
 * 功能:计算产品结构已使用的最大行数；
 *
 * input参数1: PRODUCT *product，产品属性结构体；
 *
 *
 * 返回参数:           <0，执行失败；
 *                    >=0，执行成功,返回行数;
 */

int getMaxLineOfProduct(PRODUCT *product)
{

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfProduct", "", "getMaxLineOfProduct: Start Execute");
#endif
          int i=0,max=0;

          for (i=0;i<MAX_ROW;i++)
          {
                 if (strlen((product+i)->vProduct_Code) > 0)
                      max++;
          }
	  
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfProduct", "", "getMaxLineOfProduct: End Execute");
#endif	  
    return max;
}

/*
 * getMaxLineOfService  计算服务结构已使用的最大行数；
 *
 * 功能:计算服务结构已使用的最大行数；
 *
 * input参数1: SERVICE *service，服务属性结构体；
 *
 *
 * 返回参数:           <0，执行失败；
 *                    >=0，执行成功,返回行数;
 */

int getMaxLineOfService(SERVICE *service)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfService", "", "getMaxLineOfService: Start Execute");
#endif

          int i=0,max=0;

          for (i=0;i<MAX_ROW;i++)
          {
                 if (strlen((service+i)->vProduct_Code) > 0)
                      max++;
          }	  
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfService", "", "getMaxLineOfService: End Execute");
#endif
    return max;
}

/*
 * getMaxLineOfSrvAttr  计算服务属性结构已使用的最大行数；
 *
 * 功能:计算服务属性结构已使用的最大行数；
 *
 * 参数1: SRVATTR *attr，服务属性值结构体；
 *
 *
 * 返回参数:           <0，执行失败；
 *                    >=0，执行成功,返回行数;
 */

int getMaxLineOfSrvAttr(SRVATTR *attr)
{

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfSrvAttr", "", "getMaxLineOfSrvAttr: Start Execute");
#endif
          int i=0,max=0;

          for (i=0;i<MAX_ROW;i++)
          {
                 if (strlen((attr+i)->vService_Code) > 0)
                      max++;
          }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfSrvAttr", "", "getMaxLineOfSrvAttr: End Execute");
#endif
    return max;
}


/*
 * getMaxLineOfProdAttr  计算产品属性结构已使用的最大行数；
 *
 * 功能:计算产品属性结构已使用的最大行数；
 *
 * 参数1: PRODATTR *attr，产品属性值结构体；
 *
 *
 * 返回参数:           <0，执行失败；
 *                    >=0，执行成功,返回行数;
 */

int getMaxLineOfProdAttr(PRODATTR *attr)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfProdAttr", "", "getMaxLineOfProdAttr: Start Execute");
#endif
         
	  int i=0,max=0;

          for (i=0;i<MAX_ROW;i++)
          {
                 if (strlen((attr+i)->vProduct_Attr) > 0)
                      max++;
          }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMaxLineOfProdAttr", "", "getMaxLineOfProdAttr: End Execute");
#endif
    return max;
}



/*
 * chkProductLevel  产品层次校验函数
 *
 * 功能:检查列选产品层次与选择的查品您层次是否一致
 *
 * input参数1: *product_code，产品代码，长度8个字节；
 *
 * input参数2: *product_level，可选产品层次，长度1个字节；
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功，校验一致;
 *                      1，执行成功，校验不一致；
 *
 *
 */

int chkProductLevel(char *product_code,char *product_level)
{
          char vProduct_Code[8+1];      /* 产品代码     */
          char vProduct_Level[1+1];     /* 可选产品层次 */
          char vProduct_Level_Tmp[1+1]; /* 列选产品层次 */
          char vSQL_Text[1024+1];       /* SQL语句      */
          char vTmp_String[1024+1];     /* 临时变量     */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductLevel", "", "chkProductLevel: Start Execute");
#endif


    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Level,      0, sizeof(vProduct_Level));
    memset(vProduct_Level_Tmp,  0, sizeof(vProduct_Level_Tmp));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vProduct_Code,       product_code, strlen(product_code));
    vProduct_Code[8] = '\0';

    strncpy(vProduct_Level,      product_level,strlen(product_level));
    vProduct_Level[1] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Product_Level FROM sProductCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "chkProductLevel", "", "chkProductLevel: vSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Product_Level FROM sProductCode ");
    strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "chkProductLevel", "", "chkProductLevel: vSQL_Text=[%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_sProductCode FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductCode CURSOR FOR Prepare_sProductCode;
    EXEC SQL OPEN Cursor_sProductCode using :vProduct_Code;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sProductCode INTO :vProduct_Level_Tmp;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "chkProductLevel", "", "读sProductCode产品字典表失败!");
                EXEC SQL CLOSE Cursor_sProductCode;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sProductCode;

    /*
     * 若用户输入的产品层次与被发布产品的层次不一致，则不在列选之列；
     *
     * Commented by lab 2004.11.01
     */



    if (strlen(vProduct_Level_Tmp) <=0 )
       return -2;
    else if (strncmp(vProduct_Level,vProduct_Level_Tmp,1) == 0)
       return 0;
    else
       return 1;

}


/*
 * chkSmProduct  产品归属品牌校验函数
 *
 * 功能:检查列选产品是否归属于选择品牌
 *
 * input参数1: *product_code，产品代码，长度8个字节；
 *
 * input参数2: *sm_code，归属品牌，长度2个字节；
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功，校验一致;
 *                      1，执行成功，校验不一致；
 *
 *
 */

int chkSmProduct(char *product_code,char *sm_code)
{
    char vProduct_Code[8+1];      /* 产品代码         */
    char vSm_Code[2+1];           /* 业务代码         */
    char vSm_Code_Tmp[2+1];       /* 业务代码临时变量 */
    char vSQL_Text[1024+1];       /* SQL语句          */
    char vTmp_String[1024+1];     /* 临时变量         */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkSmProduct", "", "chkSmProduct: Start Execute");
#endif

    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vSm_Code,            0, sizeof(vSm_Code));
    memset(vSm_Code_Tmp,        0, sizeof(vSm_Code_Tmp));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vProduct_Code,       product_code, strlen(product_code));
    vProduct_Code[8] = '\0';

    strncpy(vSm_Code,            sm_code,      strlen(sm_code));
    vSm_Code[2] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Sm_Code) FROM sSmProduct ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "chkSmProduct", "", "chkSmProduct: vSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Sm_Code) FROM sSmProduct ");
    strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "chkSmProduct", "", "chkSmProduct: vSQL_Text=[%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_sSmProduct FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sSmProduct CURSOR FOR Prepare_sSmProduct;
    EXEC SQL OPEN Cursor_sSmProduct using :vProduct_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sSmProduct INTO :vSm_Code_Tmp;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "chkSmProduct", "", "读sSmProduct产品品牌归属表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sSmProduct;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sSmProduct;


    /*
     * 若产品的归属品牌与该用户选择的品牌不一致，则不在列选之列；
     *
     * Commented by lab 2004.11.01
     */



    if (strlen(vSm_Code_Tmp) <=0 )
       return -2;
    else if (strncmp(vSm_Code,vSm_Code_Tmp,2) == 0){
    #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkSmProduct", "", "chkSmProduct: End Execute");
    #endif
       return 0;
       }
    else
       return 1;

}

/*
 * chkProductExclue  主产品与可选产品互斥关系函数
 *
 * 功能:检查可选产品与主产品是否存在互斥关系
 *
 * input参数1: *object_product，可选产品代码，长度8个字节；
 *
 * input参数2: *product_code，    主产品代码， 长度8个字节；
 *
 * input参数3: *business_code,    业务代码,   长度1个字节;
 *                                0: 开户
 *                                1: 添加
 *                                2: 变更
 *
 *
 * 返回参数:           <0，执行失败,ORACLE或者其他错误；
 *                      0，执行成功，不存在互斥关系;
 *                      1，执行成功，存在互斥关系；
 *                      2，已选产品与可选产品相同
 *
 *
 */

int chkProductExclude(char *object_product,char *product_code,char *business_code)
{
         char vObject_Product[8+1];        /* 可选产品代码     */
         char vProduct_Code[8+1];          /* 主产品代码       */
         char vBusiness_Code[1+1];         /* 业务代码         *
                                            * 0: 开户          *
                                            * 1: 添加          *
                                            * 2: 变更          */

         char vBusiness_Code_Tmp[20+1];    /* 业务代码Tmp      */
         char vRule_Status_Tmp[1+1];       /* 规则状态Tmp      */

         char vSQL_Text[1024+1];           /* SQL语句          */
         char vTmp_String[1024+1];         /* 临时变量         */
         int  vNum=0;                      /* 计数器           */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExclude", "", "chkProductExclude: Start Execute");
#endif

    memset(vObject_Product,    0, sizeof(vObject_Product));
    memset(vProduct_Code,      0, sizeof(vProduct_Code));
    memset(vBusiness_Code,     0, sizeof(vBusiness_Code));
    memset(vBusiness_Code_Tmp, 0, sizeof(vBusiness_Code_Tmp));
    memset(vRule_Status_Tmp,   0, sizeof(vRule_Status_Tmp));

    memset(vSQL_Text,          0, sizeof(vSQL_Text));
    memset(vTmp_String,        0, sizeof(vTmp_String));
    vNum                     = 0;

    strncpy(vObject_Product,     object_product, strlen(object_product));
    vObject_Product[8] = '\0';

    strncpy(vProduct_Code,       product_code,   strlen(product_code));
    vProduct_Code[8] = '\0';

    strncpy(vBusiness_Code,      business_code,  strlen(business_code));
    vBusiness_Code[1] = '\0';

    if (strncmp(vProduct_Code, vObject_Product, 8) == 0)
       return 2;

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status) FROM sProductExclude ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s AND   Object_Product = trim('%s') ",vTmp_String,vObject_Product);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExclude", "", "chkProductExclude:\tvSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status) FROM sProductExclude ");
    strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");
    strcat(vSQL_Text," AND   Object_Product = trim(:v2) ");


    EXEC SQL PREPARE Prepare_sProductExclude FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductExclude CURSOR FOR Prepare_sProductExclude;
    EXEC SQL OPEN Cursor_sProductExclude using :vProduct_Code,:vObject_Product;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sProductExclude INTO :vBusiness_Code_Tmp,vRule_Status_Tmp;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "chkProductExclude", "", "读sProductExclude产品互斥表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sProductExclude;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)  continue;
          if (strncmp(vRule_Status_Tmp,  "Y",         1) != 0)     continue;

          vNum++;
    }
    EXEC SQL CLOSE Cursor_sProductExclude;

    /*
     * 若vNum = 0, 则返回 0,若vNum > 0, 则返回 1;
     *
     * Commented by lab 2004.11.04
     */


    if (vNum < 0 )
       return -2;
    else if (vNum == 0 ){
    #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExclude", "", "chkProductExclude: End Execute");
    #endif
       return 0;
       }
    else if (vNum > 0)
       return 1;
    return 0;
}


/*
 * chkServiceExclude  服务与服务互斥关系函数
 *
 * 功能:检查已选服务与被选服务是否存在互斥关系
 *
 * 参数1: *object_service，可选服务代码，长度8个字节；
 *
 * 参数2: *service_code，  已选产品代码，长度8个字节；
 *
 * input参数3: *business_code,    业务代码,   长度1个字节;
 *                                0: 开户
 *                                1: 添加
 *                                2: 变更
 *
 * 返回参数:           <0，执行失败,ORACLE或者其他错误；
 *                      0，执行成功，不存在互斥关系;
 *                      1，执行成功，存在互斥关系；
 *                      2，已选服务与被选服务相同；
 *
 *
 */

int chkServiceExclude(char *object_service,char *service_code,char *business_code)
{
          char vObject_Service[4+1];        /* 可选服务代码     */
          char vService_Code[4+1];          /* 已选服务代码     */
          char vBusiness_Code[1+1];         /* 业务代码         *
                                             * 0: 开户          *
                                             * 1: 添加          *
                                             * 2: 变更          */

      char vBusiness_Code_Tmp[20+1];    /* 业务代码Tmp      */
      char vRule_Status_Tmp[1+1];       /* 规则状态Tmp      */

          char vSQL_Text[1024+1];           /* SQL语句          */
          char vTmp_String[1024+1];         /* 临时变量         */
          int  vNum=0;                      /* 计数器           */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceExclude", "", "chkServiceExclude: Start Execute");
#endif

    memset(vObject_Service,    0, sizeof(vObject_Service));
    memset(vService_Code,      0, sizeof(vService_Code));
    memset(vBusiness_Code,     0, sizeof(vBusiness_Code));
    memset(vBusiness_Code_Tmp, 0, sizeof(vBusiness_Code_Tmp));
    memset(vRule_Status_Tmp,   0, sizeof(vRule_Status_Tmp));

    memset(vSQL_Text,          0, sizeof(vSQL_Text));
    memset(vTmp_String,        0, sizeof(vTmp_String));
    vNum                     = 0;

    strncpy(vObject_Service,     object_service, strlen(object_service));
    vObject_Service[4] = '\0';

    strncpy(vService_Code,       service_code,   strlen(service_code));
    vService_Code[4] = '\0';

    strncpy(vBusiness_Code,      business_code,  strlen(business_code));
    vBusiness_Code[1] = '\0';

    if (strncmp(vService_Code, vObject_Service, 4) == 0)
       return 2;

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status) FROM sSrvExclude ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s AND   Object_Service = trim('%s') ",vTmp_String,vObject_Service);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceExclude", "", "chkServiceExclude:\tvSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status) FROM sSrvExclude ");
    strcat(vSQL_Text," WHERE Service_Code = trim(:v1) ");
    strcat(vSQL_Text," AND   Object_Service = trim(:v2) ");

    EXEC SQL PREPARE Prepare_sServiceExclude FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sServiceExclude CURSOR FOR Prepare_sServiceExclude;
    EXEC SQL OPEN Cursor_sServiceExclude using :vService_Code,:vObject_Service;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sServiceExclude INTO :vBusiness_Code_Tmp,:vRule_Status_Tmp;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "chkServiceExclude", "", "chkServiceExclude:读sServiceExclude服务互斥表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sServiceExclude;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)  continue;
          if (strncmp(vRule_Status_Tmp,  "Y",         1) != 0)     continue;

          vNum++;
    }
    EXEC SQL CLOSE Cursor_sServiceExclude;

    /*
     * 若vNum = 0, 则返回 0,若vNum > 0, 则返回 1;
     *
     * Commented by lab 2004.11.08
     */


    if (vNum < 0 )
       return -2;
    else if (vNum == 0 )
    {
    #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceExclude", "", "chkServiceExclude: End Execute");
    #endif
       return 0;
       }
    else if (vNum > 0)
       return 1;
    return 0;
}

/*
 * chkProductChg  主产品是否允许变更校验函数
 *
 * 功能:检查已选产品与新选择的主产品是否允许变更；
 *
 * 参数1: *product_code，  原产品代码，长度8个字节；
 *
 * 参数2: *object_product，新产品代码，长度8个字节；
 *
 * 参数3: *business_code,  业务代码,   长度1个字节;
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功，校验一致;
 *                     >0，执行成功，校验不一致；
 *
 *
 */

int chkProductChg(char *product_code,char *object_product,char *business_code)
{
          char vProduct_Code[8+1];      /* 原产品代码   */
          char vObject_Product[8+1];    /* 新产品代码   */
          char vBusiness_Code[1+1];     /* 业务代码     *
                                         * 0:开户       *
                                         * 1:变更       *
                                         * 2:其他       */

          char vBusiness_Code_Tmp[20+1]; /* 业务代码Tmp  */
          char vRule_Status_Tmp[1+1];   /* 业务规则     */

          char vSQL_Text[1024+1];       /* SQL语句      */
          char vTmp_String[1024+1];     /* 临时变量     */
          int  vNum=0;                  /* 临时计数器   */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductChg", "", "chkProductChg: Start Execute");
#endif

      memset(vProduct_Code,       0, sizeof(vProduct_Code));
      memset(vObject_Product,     0, sizeof(vObject_Product));
      memset(vBusiness_Code,      0, sizeof(vBusiness_Code));
      memset(vBusiness_Code_Tmp,  0, sizeof(vBusiness_Code_Tmp));
      memset(vRule_Status_Tmp,    0, sizeof(vRule_Status_Tmp));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&vNum,               0, sizeof(vNum));


      strncpy(vProduct_Code,       product_code, strlen(product_code));
      vProduct_Code[8]          = '\0';

      strncpy(vObject_Product,     object_product, strlen(object_product));
      vObject_Product[8]        = '\0';

      strncpy(vBusiness_Code,      business_code, strlen(business_code));
      vBusiness_Code[1]         = '\0';


      vNum = 0;

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status) FROM sProductChange ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s AND   Object_Product = trim('%s') ",vTmp_String,vObject_Product);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductChg", "", "chkProductChg:vSQL_Text=[%s]",vSQL_Text);
#endif

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status) FROM sProductChange ");
      strcat(vSQL_Text," WHERE Product_Code   = trim(:v1) ");
      strcat(vSQL_Text," AND   Object_Product = trim(:v2) ");



      EXEC SQL PREPARE Prepare_sProductChange FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_sProductChange CURSOR FOR Prepare_sProductChange;
      EXEC SQL OPEN Cursor_sProductChange using :vProduct_Code,:vObject_Product;
      for(;;)
      {
            EXEC SQL FETCH Cursor_sProductChange INTO :vBusiness_Code_Tmp,:vRule_Status_Tmp;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "chkProductChg", "", "读sProductChange产品变更表失败!,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sProductChange;
                  return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            vNum++;
      }
      EXEC SQL CLOSE Cursor_sProductChange;



      /*
       * 若无数据配置，则返回 1；
       *
       * Commented by lab 2004.11.23
       */



      if (vNum <= 0)
          return 1;

      /*
       * 若业务代码不归属于产品变更允许的业务代码，则返回 2；
       *
       * Commented by lab 2004.11.23
       */

      if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)
         return 2;

      /*
       * 若业务规则无效，则返回2；
       *
       * Commented by lab 2004.11.23
       */


      if (strncmp(vRule_Status_Tmp,"Y",1) != 0)
         return 3;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductChg", "", "chkProductChg: End Execute");
#endif
      return 0;
}


/*
 * chkProductAttr  产品属性校验函数
 *
 * 功能:检查该产品是否具有该属性代码
 *
 * 参数1: *product_code，产品代码，长度8个字节；
 *
 * 参数2: *attr_code，   属性代码，长度2个字节；
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功，该属性代码存在;
 *                      1，执行成功，该属性代码不存在；
 *
 *
 */

int chkProductAttr(char *product_code,char *attr_code)
{
      char vProduct_Code[8+1];         /* 产品代码         */
      char vAttr_Code[2+1];            /* 属性代码         */
      char vProduct_Attr[8+1];         /* 产品属性         */

      PRODATTR vProd_Attr[MAX_ROW];    /* 产品属性结构体   */
      PRODATTR *attr;                  /* 产品属性结构体   */

      int  vret_code=0;                /* 返回值           */
      int  i=0;                        /* 临时计数器       */
      int  vNum=0;                     /* 临时计数器       */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductAttr", "", "chkProductAttr: Start Execute");
#endif

      memset(vProduct_Code,       0, sizeof(vProduct_Code));
      memset(vAttr_Code,          0, sizeof(vAttr_Code));
      memset(vProduct_Attr,       0, sizeof(vProduct_Attr));
      memset(&vProd_Attr,         0, sizeof(vProd_Attr));

      attr = vProd_Attr;

      strncpy(vProduct_Code,      product_code, strlen(product_code));
      vProduct_Code[8]         =  '\0';

      strncpy(vAttr_Code,         attr_code,    strlen(attr_code));
      vAttr_Code[2]            =  '\0';


    /*
     * 根据产品代码提取产品属性；
     *
     * Commented by lab 2004.11.01
     */

      vret_code   = getProductAttr(vProduct_Code,vProduct_Attr);

      if (vret_code != 0)
      {
           pubLog_Debug(_FFL_, "chkProductAttr", "", "chkProductAttr:调用getProductAttr提取产品属性代码失败!",vret_code);
           pubLog_Debug(_FFL_, "chkProductAttr", "", "getProductAttr:vret_code=[%d]!",vret_code);
           return -1;
      }

    /*
     * 根据产品属性提取产品属性代码；
     *
     * Commented by lab 2004.11.01
     */

      vret_code   = getProductAttrCode(vProduct_Attr,attr);

      if (vret_code != 0)
      {
           pubLog_Debug(_FFL_, "chkProductAttr", "", "chkProductAttr:调用getProductAttr提取产品属性代码失败!",vret_code);
           pubLog_Debug(_FFL_, "chkProductAttr", "", "getProductAttrCode:vret_code=[%d]!",vret_code);
           return -1;
      }

    /*
     * 检查产品属性代码结构体中是否存在该属性代码
     *
     * Commented by lab 2004.11.01
     */

     vNum = 0;
     for (i=0;(i<MAX_ROW && strlen((attr+i) -> vProduct_Attr) > 0);i++)
     {
         if (strncmp((attr+i) -> vAttr_Code, vAttr_Code, 2) == 0)
         {
                 vNum++;
                 break;
         }
     }

    /*
     * 若vNum<=0,则返回1,否则返回0;
     *
     * Commented by lab 2004.11.01
     */

     if (vNum <= 0)
        return 1;
     else{
     #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductAttr", "", "chkProductAttr: End Execute");
     #endif
        return 0;
	}

}

/*
 *chkGrpProdExist 检测产品是否重复订购
 *
 *功能：检查集团产品是否已订购 (包括组合产品)
 *参数：客户ID
 *      产品代码
 *      品牌代码
 *      返回信息
 *返回：0-成功；-1-失败
 */
int chkGrpProdExist(char *iCustId, char *iGrpId, char *iProductCode, char *iSmCode, char *retMsg) {
  #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkGrpProdExist", "", "chkGrpProdExist: Start Execute");
#endif
  EXEC SQL BEGIN DECLARE SECTION;
    int  vCount = 0;
    char vBizCode[10+1];
    char vSqlStr[1024];
  EXEC SQL END DECLARE SECTION;

#if PROVINCE_RUN == PROVINCE_JILIN
  /* 只有专线业务允许客户重复申请 */ /*增加行业应用卡，允许重复定购  add by lixiaoxin 20121220*/
  if ((strcmp(iSmCode, "04") == 0) || (strcmp(iSmCode, "11") == 0) || (strcmp(iSmCode, "13") == 0) 
  	  || (strcmp(iSmCode, "hy") == 0 && strcmp(iProductCode, "HY000001") == 0))
  {
  #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkGrpProdExist", "", "chkGrpProdExist: End Execute");
     #endif
    return 0;
  }

	/* 对于ADC和MAS类业务，用业务代码限制重复订购（不使用产品代码判断） */
	if ((strcmp(iSmCode, "44") == 0) || (strcmp(iSmCode, "98") == 0)){
		EXEC SQL select field_value into :vBizCode 
			from dGrpUserMsgAdd 
			where id_no = to_number(:iGrpId) and user_type = 'code' and field_code = 'bcode';
		if (SQLCODE != 0){
    	sprintf(retMsg, "查询dGrpUserMsgAdd中业务代码出错![%d]", SQLCODE);
    	pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    	return -1;
		}
		
		EXEC SQL select count(*) into :vCount
			from dGrpUserMsg a, dGrpUserMsgAdd b
			where a.id_no = b.id_no and a.bill_date > last_day(sysdate) + 1
				and b.user_type = 'code' and b.field_code = 'bcode' and a.run_code = 'A'
				and a.cust_id = to_number(:iCustId) and trim(b.field_value) = :vBizCode;
		if (SQLCODE != 0){
    	sprintf(retMsg, "查询dGrpUserMsgAdd中业务代码出错![%d]", SQLCODE);
    	pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    	return -1;
		}
  	if (vCount > 0) {
    	sprintf(retMsg, "集团客户已经申请[%s]业务!",vBizCode);
    	pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s", __FILE__, retMsg);
    	return -1;
  	}
    #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkGrpProdExist", "", "chkGrpProdExist: End Execute");
     #endif
		return 0;
	}

	/* 新订购产品和用户已订购产品代码相同 */
  EXEC SQL select count(*) into :vCount
  	from dGrpUserMsg
    where cust_id = to_number(:iCustId) and product_code = :iProductCode
      and bill_date > last_day(sysdate) + 1;
  if (SQLCODE != 0) {
    sprintf(retMsg, "查询dGrpUserMsg表错误![%d]", SQLCODE);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    return -1;
  }
  if (vCount > 0) {
    sprintf(retMsg, "集团客户已经申请[%s]产品!",iProductCode);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s", __FILE__, retMsg);
    return -1;
  }

  /* 新订购产品和用户已订购组合产品重叠 */
  EXEC SQL select count(*) into :vCount
  	from dGrpUserMsg a, sProductCombInfo b
    where cust_id = to_number(:iCustId) and a.product_code = b.comb_id
    	and b.product_code = :iProductCode and a.bill_date > last_day(sysdate) + 1;
  if (SQLCODE != 0) {
    sprintf(retMsg, "查询sProductCombInfo表错误![%d]", SQLCODE);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    return -1;
  }
  if (vCount > 0) {
    sprintf(retMsg, "集团客户已经申请[%s]产品!",iProductCode);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s", __FILE__, retMsg);
    return -1;
  }

  /* 新订购组合产品和用户已订购产品重叠 */
  EXEC SQL select count(*) into :vCount
  	from dGrpUserMsg a, sProductCombInfo b
    where a.cust_id = to_number(:iCustId) and a.product_code = b.product_code
    	and b.comb_id = :iProductCode and a.bill_date > last_day(sysdate) + 1;
  if (SQLCODE != 0) {
    sprintf(retMsg, "查询sProductCombInfo表错误![%d]", SQLCODE);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    return -1;
  }
  if (vCount > 0) {
    sprintf(retMsg, "集团客户已经申请[%s]产品!",iProductCode);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s", __FILE__, retMsg);
    return -1;
  }
#endif

#if PROVINCE_RUN == PROVINCE_SHANXI
  EXEC SQL select count(*) into :vCount
    from dGrpUserMsg
    where cust_id = to_number(:iCustId) and sm_code = :iSmCode
    	and bill_date > last_day(sysdate) + 1;
  if (SQLCODE != 0) {
    sprintf(retMsg, "查询dGrpUserMsg表错误![%d]", SQLCODE);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s-- [%d][%s]", __FILE__, retMsg, SQLCODE, SQLERRMSG);
    return -1;
  }
  if (vCount > 0) {
    sprintf(retMsg, "集团客户已经申请[%s]业务!", iSmCode);
    pubLog_Debug(_FFL_, "chkGrpProdExist", "", "%s:%s", __FILE__, retMsg);
    return -1;
  }
#endif

 #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkGrpProdExist", "", "chkGrpProdExist: End Execute");
#endif
	return 0;
}


/*
 * chkProductExist  检查产品代码是否已定购
 *
 * 功能:            检查产品代码是否已定购
 *
 * input参数1: *id_no，          用户id，长度14个字节；
 *
 * input参数2: *product_code，   产品代码，长度8个字节；
 *
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，产品代码不存在;
 *                      1，产品代码存在主产品中；
 *                      2，产品代码存在可选产品中；
 *
 *
 */

int chkProductExist(char *id_no,  char *product_code)
{
      char vId_No[14+1];               /* 用户Id           */
      char vProduct_Code[8+1];         /* 产品代码         */

      char vProduct_Level[1+1];        /* 产品层次         */
      char vTmp_String[1024+1];        /* 临时变量         */
      char vSQL_Text[1024+1];          /* SQL语句          */

      char vBegin_Time_Tmp[17+1];      /* 开始时间Tmp      */
      char vEnd_Time_Tmp[17+1];        /* 结束时间Tmp      */
      char vProd_Code_Tmp[8+1];        /* 产品代码Tmp      */
      char vSysDate[17+1];             /* 系统时间Tmp      */

      char vId_Tail;                   /* ID号尾数         */


      int  vret_code=0;                /* 返回值           */
      int  i=0;                        /* 临时计数器       */
      int  vNum=0;                     /* 临时计数器       */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist: Start Execute");
#endif

      memset(vId_No,           0,  sizeof(vId_No));
      memset(vProduct_Code,    0,  sizeof(vProduct_Code));
      memset(vProduct_Level,   0,  sizeof(vProduct_Level));
      memset(vTmp_String,      0,  sizeof(vTmp_String));
      memset(vSQL_Text,        0,  sizeof(vSQL_Text));
      memset(vBegin_Time_Tmp,  0,  sizeof(vBegin_Time_Tmp));
      memset(vEnd_Time_Tmp,    0,  sizeof(vEnd_Time_Tmp));
      memset(vProd_Code_Tmp,   0,  sizeof(vProd_Code_Tmp));
      memset(vSysDate,         0,  sizeof(vSysDate));

      memset(&vId_Tail,        0,  sizeof(vId_Tail));

      strncpy(vId_No,          id_no,           strlen(id_no));
      vId_No[14]            =  '\0';

      strncpy(vProduct_Code,   product_code,    strlen(product_code));
      vProduct_Code[8]      =  '\0';

      rtrim(vId_No);
      vId_Tail   = vId_No[strlen(vId_No)-1];

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Product_Code),Trim(Product_Level), ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s Begin_Time,End_Time,To_Char(SysDate,'YYYYYMMDD HH24:MI:SS') ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s FROM dSrvMsg%c ",vTmp_String,vId_Tail);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Id_No = %s ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vSQL_Text=[%s]",vSQL_Text);
#endif

      EXEC SQL PREPARE Prepare_dSrvMsg6 FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_dSrvMsg6 CURSOR FOR Prepare_dSrvMsg6;
      EXEC SQL OPEN Cursor_dSrvMsg6;
      for(;;)
      {
              EXEC SQL FETCH Cursor_dSrvMsg6
                       INTO :vProd_Code_Tmp,     :vProduct_Level,
                            :vBegin_Time_Tmp,    :vEnd_Time_Tmp,
                            :vSysDate;

              if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
              {
                  pubLog_Debug(_FFL_, "chkProductExist", "", "读dSrvMsg%c产品定购表失败,SQLCODE=[%d]!",vId_Tail,SQLCODE);
                  EXEC SQL CLOSE Cursor_dSrvMsg6;
                  return -1;
              }

              if (SQLCODE == NOTFOUND)
                 break;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vProd_Code_Tmp=[%s]",vProd_Code_Tmp);
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vProduct_Code  =[%s]",vProduct_Code);
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vBegin_Time_Tmp =[%s]",vBegin_Time_Tmp);
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vEnd_Time_Tmp  =[%s]",vEnd_Time_Tmp);
#endif

              if (strncmp(vProd_Code_Tmp, vProduct_Code,  8) != 0) continue;
              if (strncmp(vBegin_Time_Tmp,vSysDate, 17)       > 0) continue;
              if (strncmp(vEnd_Time_Tmp,  vSysDate, 17)       < 0) continue;

              vNum++;

              if (vNum > 0) break;

      }
      EXEC SQL CLOSE Cursor_dSrvMsg6;


    /*
     * 若vNum<=0,则返回0,否则返回1,2,3;
     *
     * Commented by lab 2004.11.01
     */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductExist", "", "chkProductExist:vNum=[%d]",vNum);
#endif

     if (vNum <= 0)
        return 0;


    /*
     * 主产品返回1,可选产品返回2,品牌服务返回3
     *
     * Commented by lab 2004.11.01
     */

     if (strncmp(vProduct_Level, "1", 1) == 0)
        return 1;
     else
        return 2;

}




/*
 * chkServExist  检查服务代码是否存在
 *
 * 功能:检查服务代码是否存在
 *
 * input参数1: *id_no，          用户id，长度14个字节；
 *
 * input参数2: *service_code，   服务代码，长度4个字节；
 *
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，服务代码不存在;
 *                      1，服务代码存在主产品中；
 *                      2，服务代码存在可选产品中；
 *                      3，服务代码存在品牌服务中；
 *
 *
 */

int chkServExist(char *id_no,  char *service_code)
{
      char vId_No[14+1];               /* 用户Id           */
      char vService_Code[4+1];         /* 服务代码         */

      char vProduct_Level[1+1];        /* 产品层次         */
      char vTmp_String[1024+1];        /* 临时变量         */
      char vSQL_Text[1024+1];          /* SQL语句          */

      char vBegin_Time_Tmp[17+1];      /* 开始时间Tmp      */
      char vEnd_Time_Tmp[17+1];        /* 结束时间Tmp      */
      char vServ_Code_Tmp[4+1];        /* 服务代码Tmp      */
      char vSysDate[17+1];             /* 系统时间Tmp      */
      char vProduct_Code[8+1];         /* 产品代码         */

      char vId_Tail;                   /* ID号尾数         */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServExist", "", "chkServExist: Start Execute");
#endif
      int  vret_code=0;                /* 返回值           */
      int  i=0;                        /* 临时计数器       */
      int  vNum=0;                     /* 临时计数器       */

      memset(vId_No,           0,  sizeof(vId_No));
      memset(vService_Code,    0,  sizeof(vService_Code));
      memset(vProduct_Level,   0,  sizeof(vProduct_Level));
      memset(vTmp_String,      0,  sizeof(vTmp_String));
      memset(vSQL_Text,        0,  sizeof(vSQL_Text));
      memset(vBegin_Time_Tmp,  0,  sizeof(vBegin_Time_Tmp));
      memset(vEnd_Time_Tmp,    0,  sizeof(vEnd_Time_Tmp));
      memset(vServ_Code_Tmp,   0,  sizeof(vServ_Code_Tmp));
      memset(vSysDate,         0,  sizeof(vSysDate));
      memset(vProduct_Code,    0,  sizeof(vProduct_Code));

      memset(&vId_Tail,        0,  sizeof(vId_Tail));

      strncpy(vId_No,          id_no,           strlen(id_no));
      vId_No[14]            =  '\0';

      strncpy(vService_Code,   service_code,    strlen(service_code));
      vService_Code[4]            =  '\0';

      rtrim(vId_No);
      vId_Tail   = vId_No[strlen(vId_No)-1];

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Product_Code),Trim(Service_Code),Trim(Product_Level), ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s Begin_Time,End_Time,To_Char(SysDate,'YYYYYMMDD HH24:MI:SS') ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s FROM dSrvMsg%c ",vTmp_String,vId_Tail);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Id_No = %s ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServExist", "", "chkServExist:vSQL_Text=[%s]",vSQL_Text);
#endif

      EXEC SQL PREPARE Prepare_dSrvMsg3 FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_dSrvMsg3 CURSOR FOR Prepare_dSrvMsg3;
      EXEC SQL OPEN Cursor_dSrvMsg3;
      for(;;)
      {
              EXEC SQL FETCH Cursor_dSrvMsg3
                       INTO :vProduct_Code,      :vServ_Code_Tmp,
                            :vProduct_Level,     :vBegin_Time_Tmp,
                            :vEnd_Time_Tmp,      :vSysDate;

              if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
              {
                  pubLog_Debug(_FFL_, "chkServExist", "", "读dSrvMsg%c产品定购表失败,SQLCODE=[%d]!",vId_Tail,SQLCODE);
                  EXEC SQL CLOSE Cursor_dSrvMsg3;
                  return -1;
              }

              if (SQLCODE == NOTFOUND)
                 break;

              if (strncmp(vServ_Code_Tmp, vService_Code,  4) != 0) continue;
              if (strncmp(vBegin_Time_Tmp,vSysDate, 17)       > 0) continue;
              if (strncmp(vEnd_Time_Tmp,  vSysDate, 17)       < 0) continue;

              vNum++;

              if (vNum > 0) break;

      }
      EXEC SQL CLOSE Cursor_dSrvMsg3;


    /*
     * 若vNum<=0,则返回0,否则返回1,2,3;
     *
     * Commented by lab 2004.11.01
     */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServExist", "", "chkServExist:vNum=[%d]",vNum);
#endif

     if (vNum <= 0)
           return 0;


    /*
     * 主产品返回1,可选产品返回2,品牌服务返回3
     *
     * Commented by lab 2004.11.01
     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServExist", "", "chkServExist: End Execute");
#endif
     if (strncmp(vProduct_Level, "1", 1) == 0)
        return 1;
     else
     {
          if (strncmp(vProduct_Code, DEFAULT_PRODUCT_CODE, 8) != 0)
             return 2;
          else
             return 3;
     }

}

/*
 * chkSrvAttrOfNeed  检查服务代码是否必须输入服务属性
 *
 * 功能: 检查服务代码是否必须输入服务属性
 *
 * input参数1: *service_code，服务代码，长度4个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错;
 *                      0，无需输入服务属性;
 *                      1, 必须输入服务属性；
 *
 *
 */

int chkSrvAttrOfNeed(char *service_code)
{
      char     vService_Code[4+1];       /* 服务代码     */
      char     vNode_List[NODE_LIST+1];  /* 节点链表     */
      char     vOwner_Code[2+1];         /* 用户属性     */
      char     vClass_Code[2+1];         /* 营业厅类型   */
      int      vPower_Right;             /* 操作员权限   */

      SRVATTR  vServ[MAX_ROW],*attr;     /* 服务属性     */

      char     vSQL_Text[1024+1];        /* SQL语句      */
      char     vTmp_String[NODE_LIST+1]; /* 临时变量     */
      int      max;                      /* 临时变量     */
      int      i;                        /* 临时计数器   */
      int      vret_code;                /* 返回值       */
      int      vNum;                     /* 临时计数器   */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkSrvAttrOfNeed", "", "chkSrvAttrOfNeed: Start Execute");
#endif
      memset(vService_Code,       0, sizeof(vService_Code));
      memset(vNode_List,          0, sizeof(vNode_List));
      memset(vOwner_Code,         0, sizeof(vOwner_Code));
      memset(vClass_Code,         0, sizeof(vClass_Code));
      memset(&vPower_Right,       0, sizeof(vPower_Right));

      memset(&vServ,              0, sizeof(vServ));
      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&max,                0, sizeof(max));
      memset(&i,                  0, sizeof(i));
      vret_code                =  0;
      vNum                     =  0;

      strncpy(vService_Code,       service_code, strlen(service_code));
      vService_Code[4]           = '\0';

      attr  = vServ;

      /*
       * 调用getSrvAttr函数提取该服务的属性列表
       *
       * commented by lab 2004.11.19
       */

      vret_code = getSrvAttr(vService_Code,vNode_List,vOwner_Code,vClass_Code,vPower_Right,attr);

      for (i=0;i<MAX_ROW && (strlen((attr+i) -> vService_Code) > 0);i++)
      {
           if (strncmp((attr+i) -> vAllow_Flag,  "Y", 1) == 0)
              vNum++;
      }


      if (vNum > 0)
      	{
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "chkSrvAttrOfNeed", "", "chkSrvAttrOfNeed: End Execute");
	#endif
      		return 1;
      	}
      else
	{             
	#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "chkSrvAttrOfNeed", "", "chkSrvAttrOfNeed: End Execute");
	#endif	
		return 0;
	}
}



/*
 * chkVpmnUser  检查该用户是否为VPMN用户
 *
 * 功能:检查该用户是否为VPMN用户
 *
 * input参数1: *id_no，          用户id，长度14个字节；
 *
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，该用户不是VPMN用户;
 *                      1，服务代码存在主产品中；
 *
 */

int chkVpmnUser(char *id_no)
{
      char vId_No[14+1];               /* 用户Id           */
      char vSQL_Text[1024+1];          /* SQL语句          */
      char vBegin_Time[17+1];          /* 开始时间Tmp      */
      char vEnd_Time[17+1];            /* 结束时间Tmp      */
      char vSysDate[17+1];             /* 系统时间Tmp      */
      char vTmp_String[1024+1];        /* 临时变量         */

      int  vret_code=0;                /* 返回值           */
      int  vNum=0;                     /* 临时计数器       */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser: Start Execute");
#endif
      memset(vId_No,           0,  sizeof(vId_No));
      memset(vSQL_Text,        0,  sizeof(vSQL_Text));
      memset(vBegin_Time,      0,  sizeof(vBegin_Time));
      memset(vEnd_Time,        0,  sizeof(vEnd_Time));
      memset(vSysDate,         0,  sizeof(vSysDate));
      memset(vTmp_String,      0,  sizeof(vTmp_String));


      strncpy(vId_No,          id_no,           strlen(id_no));
      vId_No[14]            =  '\0';

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT To_Char(Start_Date,'YYYYYMMDD HH24:MI:SS'), ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s To_Char(End_Date,'YYYYYMMDD HH24:MI:SS'), ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s To_Char(SysDate,'YYYYYMMDD HH24:MI:SS') ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s FROM dVpmnUsrMsg ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Id_No = To_Number(Trim('%s')) ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser:vSQL_Text=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");
      strcpy(vSQL_Text," SELECT To_Char(Start_Date,'YYYYYMMDD HH24:MI:SS'), ");
      strcat(vSQL_Text," To_Char(End_Date,'YYYYYMMDD HH24:MI:SS'), ");
      strcat(vSQL_Text," To_Char(SysDate,'YYYYYMMDD HH24:MI:SS') ");
      strcat(vSQL_Text," FROM dVpmnUsrMsg ");
      strcat(vSQL_Text," WHERE Id_No = To_Number(Trim(:v1)) ");

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser:vSQL_Text=[%s]",vSQL_Text);
#endif

      EXEC SQL PREPARE Prepare_dVpmnUsrMsg FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_dVpmnUsrMsg CURSOR FOR Prepare_dVpmnUsrMsg;
      EXEC SQL OPEN Cursor_dVpmnUsrMsg using :vId_No;
      for(;;)
      {
              EXEC SQL FETCH Cursor_dVpmnUsrMsg INTO :vBegin_Time,:vEnd_Time,:vSysDate;

              if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
              {
                  pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser:读dVpmnUsrMsg表失败,SQLCODE=[%d]!",SQLCODE);
                  EXEC SQL CLOSE Cursor_dVpmnUsrMsg;
                  return -1;
              }

              if (SQLCODE == NOTFOUND)
                 break;


              if (strncmp(vBegin_Time,vSysDate, 17)       > 0) continue;
              if (strncmp(vEnd_Time,  vSysDate, 17)       < 0) continue;

              vNum++;
      }
      EXEC SQL CLOSE Cursor_dVpmnUsrMsg;


    /*
     * 若vNum<=0,则返回0,否则返回1,2,3;
     *
     * Commented by lab 2004.11.01
     */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser:vNum=[%d]",vNum);
#endif

     if (vNum <= 0)
	{
     #ifdef _DEBUG_
		pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser: End Execute");
     #endif
	        return 0;
	}
	else
	{
     #ifdef _DEBUG_
		pubLog_Debug(_FFL_, "chkVpmnUser", "", "chkVpmnUser: End Execute");
     #endif
		return 1;
	}
}


/*
 * getServAccept  提取服务的申请流水
 *
 * 功能: 根据用户Id提取主产品
 *
 * input参数1: *id_no，       用户Id，长度14个字节；
 *
 * input参数2: *product_type，产品类型，长度4个字节；
 *
 * input参数3: *service_code，服务代码，长度4个字节；
 *
 * output参数4: *login_accept，操作流水，长度14个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 无数据返回；
 *                      2, 主产品重叠
 *                      3, 产品代码为空；
 *
 */

int getServAccept(char *id_no,char *product_type, char *service_code, char *login_accept)
{
    char vId_No[14+1];            /* 用户Id       */
    char vProduct_Type[4+1];      /* 产品类型     *
                                   * main :主产品 *
                                   * sub:可选产品 *
                                   * sm:品牌服务  */

    char vId_Tail;                /* 用户ID尾数   */
    char vService_Code[4+1];      /* 服务代码     */

    char vProduct_Code[8+1];      /* 产品代码     */
    char vProduct_Level[1+1];     /* 产品层次     */
    char vServ_Code[4+1];         /* 服务代码Tmp  */
    char vLogin_Accept[14+1];     /* 操作流水Tmp  */
    char vMain_Flag[1+1];         /* 主体服务标志 */
    char vBegin_Time[17+1];       /* 开始时间     */
    char vEnd_Time[17+1];         /* 结束时间     */
    char vSysDate[17+1];          /* 系统时间     */
    char vSQL_Text[1024+1];       /* SQL语句      */
    char vTmp_String[1024+1];     /* 临时变量     */

    int  vNum=0;                  /* 临时计数器   */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServAccept", "", "getServAccept: Start Execute");
#endif
    memset(vId_No,              0, sizeof(vId_No));
    memset(vProduct_Type,       0, sizeof(vProduct_Type));
    memset(&vId_Tail,           0, sizeof(vId_Tail));
    memset(vService_Code,       0, sizeof(vService_Code));

    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Level,      0, sizeof(vProduct_Level));
    memset(vServ_Code,          0, sizeof(vServ_Code));
    memset(vLogin_Accept,       0, sizeof(vLogin_Accept));
    memset(vMain_Flag,          0, sizeof(vMain_Flag));
    memset(vBegin_Time,         0, sizeof(vBegin_Time));
    memset(vEnd_Time,           0, sizeof(vEnd_Time));
    memset(vSysDate,            0, sizeof(vSysDate));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    memset(&vNum,               0, sizeof(vNum));

    strncpy(vId_No,         id_no,   strlen(id_no));
    vId_No[14] = '\0';

    rtrim(vId_No);
    vId_Tail = vId_No[strlen(vId_No)-1];

    strncpy(vProduct_Type,   product_type,   strlen(product_type));
    vProduct_Type[4] = '\0';

    strncpy(vService_Code,   service_code,   strlen(service_code));
    vService_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Product_Code),Trim(Service_Code),Trim(Product_Level),Trim(Main_Flag), ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s Begin_Time,End_Time,To_Char(SysDate,'YYYYYMMDD HH24:MI:SS'), ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s Trim(To_Char(Login_Accept)) ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s FROM dSrvMsg%c ",vTmp_String,vId_Tail);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Id_No = %s ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServAccept", "", "getServAccept:vSQL_Text=[%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_dSrvMsg4 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_dSrvMsg4 CURSOR FOR Prepare_dSrvMsg4;
    EXEC SQL OPEN Cursor_dSrvMsg4;
    for(;;)
    {
            EXEC SQL FETCH Cursor_dSrvMsg4
                     INTO :vProduct_Code,       :vServ_Code,
                          :vProduct_Level,      :vMain_Flag,
                          :vBegin_Time,         :vEnd_Time,
                          :vSysDate,            :vLogin_Accept;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                pubLog_Debug(_FFL_, "getServAccept", "", "读dSrvMsg%c产品定购表失败,SQLCODE=[%d]!",vId_Tail,SQLCODE);
                EXEC SQL CLOSE Cursor_dSrvMsg4;
                return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            if (strncmp(vBegin_Time,    vSysDate, 17)          > 0) continue;
            if (strncmp(vEnd_Time,      vSysDate, 17)          < 0) continue;
            if (strncmp(vService_Code,  vServ_Code, 4)        != 0) continue;

            if   (strncmp(vProduct_Type, "main", 4) == 0)
            {
               if (strncmp(vProduct_Level, "1",   1) != 0)  continue;
            }
            else if (strncmp(vProduct_Type, "sub",  3) == 0)
            {
               if (strncmp(vProduct_Level, "1",   1) == 0)  continue;
            }
            else if (strncmp(vProduct_Type, "sm",   2) == 0)
            {
               if (strncmp(vProduct_Code, DEFAULT_PRODUCT_CODE,1) != 0)  continue;
            }
            else
               break;

            vNum++;

            if (vNum > 0)
                break;

    }
    EXEC SQL CLOSE Cursor_dSrvMsg4;

    /*
     * 若vNum非1，则返回错误信息;
     *
     * Commented by lab 2004.11.01
     */

    if (vNum <= 0)
       return 1;

    if (login_accept != NULL)
    {
         strncpy(login_accept,   vLogin_Accept,   strlen(vLogin_Accept));
         login_accept[strlen(vLogin_Accept)] = '\0';
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServAccept", "", "getServAccept: End Execute");
#endif
    return 0;
}



/*
 * getShortProdAccept  提取短号码产品的定制流水
 *
 * 功能: 提取短号码产品的定制流水
 *
 * input参数1:  *id_no，        用户Id，长度14个字节；
 *
 * output参数2: *product_code， 用户Id，长度8个字节；
 *
 * output参数3: *login_accept，操作流水，长度14个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 无数据返回；
 *                      2, 短号码产品重叠
 *                      3, 产品代码为空；
 *                      4, 操作流水为空；
 *
 */

int getShortProdAccept(char *id_no,char *product_code, char *login_accept)
{
    char vId_No[14+1];            /* 用户Id       */

    char vId_Tail;                /* 用户ID尾数   */
    char vProduct_Code[8+1];      /* 产品代码     */
    char vProduct_Level[1+1];     /* 产品层次     */
    char vLogin_Accept[14+1];     /* 操作流水Tmp  */

    char vBegin_Time[17+1];       /* 开始时间     */
    char vEnd_Time[17+1];         /* 结束时间     */
    char vSysDate[17+1];          /* 系统时间     */
    char vSQL_Text[1024+1];       /* SQL语句      */
    char vTmp_String[1024+1];     /* 临时变量     */

    int  vNum=0;                  /* 临时计数器   */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getShortProdAccept", "", "getShortProdAccept: Start Execute");
#endif
    memset(vId_No,              0, sizeof(vId_No));
    memset(&vId_Tail,           0, sizeof(vId_Tail));
    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Level,      0, sizeof(vProduct_Level));
    memset(vLogin_Accept,       0, sizeof(vLogin_Accept));
    memset(vBegin_Time,         0, sizeof(vBegin_Time));
    memset(vEnd_Time,           0, sizeof(vEnd_Time));
    memset(vSysDate,            0, sizeof(vSysDate));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    memset(&vNum,               0, sizeof(vNum));

    strncpy(vId_No,         id_no,   strlen(id_no));
    vId_No[14] = '\0';

    rtrim(vId_No);
    vId_Tail = vId_No[strlen(vId_No)-1];

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Product_Code),Trim(Product_Level), ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s Begin_Time,End_Time,To_Char(SysDate,'YYYYYMMDD HH24:MI:SS'), ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s Trim(To_Char(Login_Accept)) ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s FROM dSrvMsg%c ",vTmp_String,vId_Tail);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Id_No = %s ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getShortProdAccept", "", "getShortProdAccept:vSQL_Text=[%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_dSrvMsg5 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_dSrvMsg5 CURSOR FOR Prepare_dSrvMsg5;
    EXEC SQL OPEN Cursor_dSrvMsg5;
    for(;;)
    {
            EXEC SQL FETCH Cursor_dSrvMsg5
                     INTO :vProduct_Code,       :vProduct_Level,
                          :vBegin_Time,         :vEnd_Time,
                          :vSysDate,            :vLogin_Accept;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                pubLog_Debug(_FFL_, "getShortProdAccept", "", "读dSrvMsg%c产品定购表失败,SQLCODE=[%d]!",vId_Tail,SQLCODE);
                EXEC SQL CLOSE Cursor_dSrvMsg5;
                return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            if (strncmp(vBegin_Time,    vSysDate, 17)  > 0) continue;
            if (strncmp(vEnd_Time,      vSysDate, 17)  < 0) continue;
            if (strncmp(vProduct_Level, "1",       1) == 0) continue;
            if (strncmp(vProduct_Level, "0",       1) == 0) continue;

            if (chkProductAttr(vProduct_Code,"1b"))         continue;


            vNum++;

            if (vNum > 0)
                break;

    }
    EXEC SQL CLOSE Cursor_dSrvMsg5;

    /*
     * 若vNum非1，则返回错误信息;
     *
     * Commented by lab 2004.11.01
     */

    if (vNum <= 0)
       return 1;
    else if (vNum > 1)
       return 2;

    if (product_code != NULL)
    {
         Trim(vProduct_Code);
         if (strlen(vProduct_Code) <=0)  return 3;

         strncpy(product_code,   vProduct_Code,   strlen(vProduct_Code));
         product_code[strlen(vProduct_Code)] = '\0';
    }

    if (login_accept != NULL)
    {
         Trim(vLogin_Accept);
         if (strlen(vLogin_Accept) <=0)  return 4;

         strncpy(login_accept,   vLogin_Accept,   strlen(vLogin_Accept));
         login_accept[strlen(vLogin_Accept)] = '\0';
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getShortProdAccept", "", "getShortProdAccept: End Execute");
#endif
    return 0;
}



/*
 * getMainProduct  提取主产品函数
 *
 * 功能: 根据用户Id提取主产品
 *
 * input参数1: *id_no，用户Id，长度14个字节；
 *
 * output参数2: *main_product，主产品代码，长度8个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 无数据返回；
 *                      2, 主产品重叠
 *                      3, 产品代码为空；
 *
 */

int getMainProduct(char *id_no,char *main_product)
{
    char vId_No[14+1];            /* 用户Id       */
    char vMain_Product[8+1];      /* 主产品代码   */
    char vId_Tail;                /* 用户ID尾数   */

    char vProduct_Level[1+1];     /* 产品层次     */
    char vMain_Flag[1+1];         /* 主体服务标志 */
    char vBegin_Time[17+1];       /* 开始时间     */
    char vEnd_Time[17+1];         /* 结束时间     */
    char vSysDate[17+1];          /* 系统时间     */
    char vSQL_Text[1024+1];       /* SQL语句      */
    char vTmp_String[1024+1];     /* 临时变量     */

    int  vNum=0;                  /* 临时计数器   */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMainProduct", "", "getMainProduct: Start Execute");
#endif

    memset(vId_No,              0, sizeof(vId_No));
    memset(vMain_Product,       0, sizeof(vMain_Product));
    memset(&vId_Tail,           0, sizeof(vId_Tail));
    memset(vProduct_Level,      0, sizeof(vProduct_Level));
    memset(vMain_Flag,          0, sizeof(vMain_Flag));
    memset(vBegin_Time,         0, sizeof(vBegin_Time));
    memset(vEnd_Time,           0, sizeof(vEnd_Time));
    memset(vSysDate,            0, sizeof(vSysDate));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    memset(&vNum,               0, sizeof(vNum));

    strncpy(vId_No,         id_no,   strlen(id_no));
    vId_No[14] = '\0';

    rtrim(vId_No);
    vId_Tail = vId_No[strlen(vId_No)-1];

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Product_Code),Trim(Product_Level),NVL(Trim(Main_Flag),'N'), ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s Begin_Time,End_Time,To_Char(SysDate,'YYYYYMMDD HH24:MI:SS') ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s FROM dSrvMsg%c ",vTmp_String,vId_Tail);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Id_No = %s ",vTmp_String,vId_No);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMainProduct", "", "getMainProduct:vSQL_Text=[%s]",vSQL_Text);
#endif


    EXEC SQL PREPARE Prepare_dSrvMsg2 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_dSrvMsg2 CURSOR FOR Prepare_dSrvMsg2;
    EXEC SQL OPEN Cursor_dSrvMsg2;
    for(;;)
    {
            EXEC SQL FETCH Cursor_dSrvMsg2
                     INTO :vMain_Product,:vProduct_Level,
                          :vMain_Flag,   :vBegin_Time,
                          :vEnd_Time,    :vSysDate;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                pubLog_Debug(_FFL_, "getMainProduct", "", "读dSrvMsg%c产品定购表失败,SQLCODE=[%d],SQLMSG=[%s]!",vId_Tail,SQLCODE,SQLERRMSG);
                EXEC SQL CLOSE Cursor_dSrvMsg2;
                return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            if (strncmp(vMain_Product,DEFAULT_PRODUCT_CODE,8) == 0) continue;
            if (strncmp(vProduct_Level,"1",1)                 != 0) continue;
            if (strncmp(vMain_Flag,    "Y",1)                 != 0) continue;
            if (strncmp(vBegin_Time,    vSysDate, 17)          > 0) continue;
            if (strncmp(vEnd_Time,      vSysDate, 17)          < 0) continue;

            vNum++;

            if (vNum > 0) break;

    }
    EXEC SQL CLOSE Cursor_dSrvMsg2;

    /*
     * 若vNum非1，则返回错误信息;
     *
     * Commented by lab 2004.11.01
     */

    if (vNum <= 0)
       return 1;
    else if (vNum > 1)
       return 2;

    if (strlen(rtrim(vMain_Product)) <=0 )
       return 3;
    else
    {
         strncpy(main_product, vMain_Product, strlen(vMain_Product));
         main_product[strlen(vMain_Product)]='\0';
 #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMainProduct", "", "getMainProduct: End Execute");
#endif
         return 0;
    }

}


/*
 * getProductName  提取产品名称函数
 *
 * 功能: 根据产品代码提取产品名称
 *
 * input参数1: *product_code，产品代码，长度8个字节；
 *
 * output参数2: *product_name，产品名称，长度20个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *                      2, 无数据返回;
 *                      3, 数据重叠;
 *
 */

int getProductName(char *product_code,char *product_name)
{
          char vProduct_Code[8+1];      /* 产品代码     */
          char vProduct_Name[20+1];     /* 产品名称     */
          char vSQL_Text[1024+1];       /* SQL语句      */
          char vTmp_String[1024+1];     /* 临时变量     */
          int  vNum=0;                  /* 计数器       */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductName", "", "getProductName: Start Execute");
#endif
    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Name,       0, sizeof(vProduct_Name));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    memset(&vNum,               0, sizeof(vNum));

    strncpy(vProduct_Code,      product_code,   strlen(product_code));
    vProduct_Code[8] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Product_Name) FROM sProductCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductName", "", "getProductName:vSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Product_Name) FROM sProductCode ");
    strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductName", "", "getProductName:vSQL_Text=[%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_sProductCode1 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductCode1 CURSOR FOR Prepare_sProductCode1;
    EXEC SQL OPEN Cursor_sProductCode1 using :vProduct_Code;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sProductCode1 INTO :vProduct_Name;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getProductName", "", "读sProductCode产品字典表失败!");
                EXEC SQL CLOSE Cursor_sProductCode1;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          vNum++;
    }
    EXEC SQL CLOSE Cursor_sProductCode1;

    /*
     * 若vNum<=0 则返回2
     *
     * Commented by lab 2004.11.01
     */

     if (vNum <= 0)
        return 2;
     else if (vNum > 1)
        return 3;

    /*
     * 若产品名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */


    if (strlen(rtrim(vProduct_Name)) <=0 )
       return 1;
    else
    {
         strncpy(product_name, vProduct_Name, strlen(vProduct_Name));
         product_name[strlen(vProduct_Name)]='\0';
	 #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductName", "", "getProductName: End Execute");
	#endif
         return 0;
    }

}

/*
 * getSrvAttrValue  根据服务属性代码提取属性值函数
 *
 * 功能: 根据服务属性代码提取属性值
 *
 * input参数1:  *attr_code，   属性代码，长度2个字节；
 *
 * output参数2: *attr_type，   属性类型，长度1个字节；
 *
 * output参数3: *attr_name,    属性名称, 长度20字节;
 *
 * output参数4: *allow_flag,   允许输入属性值标志，长度1字节；
 *
 * output参数4: *value_format, 属性值格式, 长度2字节;
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错;
 *                      0，执行成功;
 *                      1, 无数据返回；
 *                      2, 返回记录重叠；
 *
 *
 */

int getSrvAttrValue(char *attr_code,char *attr_type, char *attr_name, char *allow_flag, char *value_format)
{
          char vAttr_Code[2+1];          /* 属性代码     */
          char vAttr_Type[1+1];          /* 属性类型     */
          char vAttr_Name[20+1];         /* 属性名称     */
          char vAllow_Flag[1+1];         /* 允许输入值   *
                                          * 标志         */
          char vValue_Format[2+1];       /* 属性值格式   */

          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
          int  max;                      /* 临时变量     */
          int  vret_code;                /* 返回值       */
          int  vNum;                     /* 临时计数器   */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttrValue", "", "getSrvAttrValue: Start Execute");
#endif
      memset(vAttr_Code,          0, sizeof(vAttr_Code));
      memset(vAttr_Type,          0, sizeof(vAttr_Type));
      memset(vAttr_Name,          0, sizeof(vAttr_Name));
      memset(vAllow_Flag,         0, sizeof(vAllow_Flag));
      memset(vValue_Format,       0, sizeof(vValue_Format));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&max,                0, sizeof(max));
      vret_code                =  0;
      vNum                     =  0;

      strncpy(vAttr_Code,        attr_code, strlen(attr_code));
      vAttr_Code[2]           = '\0';

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Attr_Type,Trim(Attr_Name) Attr_Name,Allow_Flag,Value_Format ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s FROM sSrvAttrCode ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Attr_Code = trim('%s') ",vTmp_String,vAttr_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttrValue", "", "debug msg:vSQL_Text=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");

      strcpy(vSQL_Text," SELECT Attr_Type,Trim(Attr_Name) Attr_Name,Allow_Flag,Value_Format ");
      strcat(vSQL_Text," FROM sSrvAttrCode WHERE  Attr_Code = trim(:v1) ");


      EXEC SQL PREPARE Prepare_sSrvAttrCode FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_sSrvAttrCode CURSOR FOR Prepare_sSrvAttrCode;
      EXEC SQL OPEN Cursor_sSrvAttrCode using :vAttr_Code;
      for(;;)
      {
            EXEC SQL FETCH Cursor_sSrvAttrCode
                     INTO :vAttr_Type,  :vAttr_Name,
                          :vAllow_Flag, :vValue_Format;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                    pubLog_Debug(_FFL_, "getSrvAttrValue", "", "读sSrvAttrCode服务属性值字典表失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_sSrvAttrCode;
                return -1;
            }

            if (SQLCODE == NOTFOUND)
                break;
            vNum++;
      }
      EXEC SQL CLOSE Cursor_sSrvAttrCode;

    if (vNum <= 0)
       return 1;
    else if (vNum > 1)
       return 2;
    else
    {
         if (attr_type != NULL)
         {
             strncpy(attr_type, vAttr_Type, strlen(vAttr_Type));
             attr_type[strlen(rtrim(vAttr_Type))]='\0';
         }

         if (attr_name != NULL)
         {
             strncpy(attr_name, vAttr_Name, strlen(vAttr_Name));
             attr_name[strlen(rtrim(vAttr_Name))]='\0';
         }

         if (allow_flag != NULL)
         {
             strncpy(allow_flag, vAllow_Flag, strlen(vAllow_Flag));
             allow_flag[strlen(rtrim(vAllow_Flag))]='\0';
         }

         if (value_format != NULL)
         {
             strncpy(value_format, vValue_Format, strlen(vValue_Format));
             value_format[strlen(rtrim(vValue_Format))]='\0';
         }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttrValue", "", "getSrvAttrValue: End Execute");
#endif
         return 0;
    }
}


/*
 * getSrvAttr  根据服务代码提取服务属性函数
 *
 * 功能: 根据服务代码提取服务属性
 *
 * input参数1: *service_code，服务代码，长度4个字节；
 *
 * input参数2: *node_list，   节点链表，长度1024个字节；
 *
 * input参数3: *owner_code,   用户属性, 长度2字节;
 *
 * input参数4: *class_code,   营业厅类型，长度2字节；
 *
 * input参数4: power_right,   营业员权限, 整型;
 *
 * output参数5:SRVATTR *attr, 服务属性结构体;
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错;
 *                      0，执行成功;
 *                      1, 无数据返回；
 *
 *
 */

int getSrvAttr(char *service_code,char *node_list, char *owner_code, char *class_code, int power_right,SRVATTR *attr)
{
          char vService_Code[4+1];       /* 服务代码     */
          char vService_Name[20+1];      /* 服务名称     */
          char vNode_List[NODE_LIST+1];  /* 节点链表     */
          char vOwner_Code[2+1];         /* 用户属性     */
          char vClass_Code[2+1];         /* 营业厅类型   */
          int  vPower_Right;             /* 操作员权限   */

          char vAttr_Code[2+1];          /* 属性代码     */
          char vAttr_Type[1+1];          /* 属性类型     */
          char vAttr_Name[20+1];         /* 属性名称     */
          char vAllow_Flag[1+1];         /* 允许输入值   *
                                          * 标志         */
          char vValue_Format[2+1];       /* 属性值格式   */

          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[NODE_LIST+1]; /* 临时变量     */
          int  max;                      /* 临时变量     */
          int  vret_code;                /* 返回值       */
          int  vNum;                     /* 临时计数器   */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: Start Execute");
#endif
      memset(vService_Code,       0, sizeof(vService_Code));
      memset(vService_Name,       0, sizeof(vService_Name));
      memset(vNode_List,          0, sizeof(vNode_List));
      memset(vOwner_Code,         0, sizeof(vOwner_Code));
      memset(vClass_Code,         0, sizeof(vClass_Code));
      memset(&vPower_Right,       0, sizeof(vPower_Right));

      memset(vAttr_Code,          0, sizeof(vAttr_Code));
      memset(vAttr_Type,          0, sizeof(vAttr_Type));
      memset(vAttr_Name,          0, sizeof(vAttr_Name));
      memset(vAllow_Flag,         0, sizeof(vAllow_Flag));
      memset(vValue_Format,       0, sizeof(vValue_Format));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&max,                0, sizeof(max));
      vret_code                =  0;
      vNum                     =  0;

      strncpy(vService_Code,       service_code, strlen(service_code));
      vService_Code[4]           = '\0';

      strncpy(vNode_List,          node_list,    strlen(node_list));
      vNode_List[NODE_LIST]      = '\0';

      strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
      vOwner_Code[2]             = '\0';

      strncpy(vClass_Code,         class_code,   strlen(class_code));
      vClass_Code[2]             = '\0';

      vPower_Right               =  power_right;


      /*
       * 集团服务属性值结构体已使用行数
       *
       * commented by lab 2004.11.19
       */

       max = getMaxLineOfSrvAttr(attr);

      if (max < 0)
      {
         pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: 提取服务属性结构体使用行数失败");
         pubLog_Debug(_FFL_, "getSrvAttr", "", "getMaxLineOfSrvAttr: vret_code＝[%d]",max);
         return -1;
      }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr:\tmax=[%d]",max);
#endif

      /*
       * 根据服务代码从服务属性配置表sSrvAttr中提取属性代码
       *
       * commented by lab 2004.11.19
       */

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Service_Attr FROM sSrvAttr ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvAttr", "", "debug msg:vSQL_Text=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");
      strcat(vSQL_Text," SELECT Service_Attr FROM sSrvAttr ");
      strcat(vSQL_Text," WHERE Service_Code = trim(:v1) ");


      EXEC SQL PREPARE Prepare_sSrvAttr FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_sSrvAttr CURSOR FOR Prepare_sSrvAttr;
      EXEC SQL OPEN Cursor_sSrvAttr using :vService_Code;
      for(;;)
      {
            EXEC SQL FETCH Cursor_sSrvAttr INTO :vAttr_Code;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                    pubLog_Debug(_FFL_, "getSrvAttr", "", "读sSrvAttr服务属性字典表失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_sSrvAttr;
                return -1;
            }

            if (SQLCODE == NOTFOUND)
                break;


            vret_code = getServiceName(vService_Code,vService_Name);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sSrvAttr;
                   pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: 提取服务名称失败，服务代码＝[%s]",vService_Code);
                   pubLog_Debug(_FFL_, "getSrvAttr", "", "getServiceName: vret_code＝[%d]",vret_code);
                   return -2;
            }

            vret_code = getSrvAttrValue(vAttr_Code,vAttr_Type,vAttr_Name,vAllow_Flag,vValue_Format);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sSrvAttr;
                   pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: 提取服务属性值失败，属性代码＝[%s]",vAttr_Code);
                   pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttrValue: vret_code＝[%d]",vret_code);
                   return -3;
            }

            strncpy((attr+max) -> vService_Code,   vService_Code,  strlen(vService_Code));
            strncpy((attr+max) -> vService_Name,   vService_Name,  strlen(vService_Name));
            strncpy((attr+max) -> vAttr_Type,      vAttr_Type,     strlen(vAttr_Type));
            strncpy((attr+max) -> vAttr_Code,      vAttr_Code,     strlen(vAttr_Code));
            strncpy((attr+max) -> vAttr_Name,      vAttr_Name,     strlen(vAttr_Name));
            strncpy((attr+max) -> vAllow_Flag,     vAllow_Flag,    strlen(vAllow_Flag));
            strncpy((attr+max) -> vValue_Format,   vValue_Format,  strlen(vValue_Format));

            max++;
            vNum++;
      }
      EXEC SQL CLOSE Cursor_sSrvAttr;

	if (vNum > 0)    
	{
	#ifdef _DEBUG_
	       pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: End Execute");
	#endif
		return 0;
	}
	else
	{
	#ifdef _DEBUG_
	       pubLog_Debug(_FFL_, "getSrvAttr", "", "getSrvAttr: End Execute");
	#endif
		return 1;
	}
}





/*
 * getServiceName  提取服务名称函数
 *
 * 功能: 根据服务代码提取服务名称
 *
 * input参数1: *service_code，服务代码，长度4个字节；
 *
 * output参数2: *service_name，服务名称，长度20个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *
 *
 */

int getServiceName(char *service_code,char *service_name)
{
          char vService_Code[4+1];       /* 服务代码     */
          char vService_Name[20+1];      /* 服务名称     */
          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServiceName", "", "getServiceName: Start Execute");
#endif
    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vService_Name,       0, sizeof(vService_Name));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vService_Code,       service_code, strlen(service_code));
    vService_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Service_Name) FROM sSrvCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Service_Name) FROM sSrvCode ");
    strcat(vSQL_Text," WHERE Service_Code = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sSrvCode1 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sSrvCode1 CURSOR FOR Prepare_sSrvCode1;
    EXEC SQL OPEN Cursor_sSrvCode1 using :vService_Code;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sSrvCode1 INTO :vService_Name;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getServiceName", "", "读sSrvCode服务字典表失败!");
                EXEC SQL CLOSE Cursor_sSrvCode1;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sSrvCode1;

    /*
     * 若服务名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vService_Name)) <=0 )
       return -2;
    else
    {
         strncpy(service_name, vService_Name, strlen(vService_Name));
         service_name[strlen(vService_Name)]='\0';
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServiceName", "", "getServiceName: End Execute");
#endif
       return 0;
    }

}


/*
 * getServiceType  提取服务类型函数
 *
 * 功能: 根据服务代码提取服务类型
 *
 * input参数1: *service_code，服务代码，长度4个字节；
 *
 * output参数2: *service_type，服务类型，长度1个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *
 *
 */

int getServiceType(char *service_code,char *service_type)
{
    char vService_Code[4+1];       /* 服务代码     */
    char vService_Type[1+1];       /* 服务类型     */
    char vSQL_Text[1024+1];        /* SQL语句      */
    char vTmp_String[1024+1];      /* 临时变量     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServiceType", "", "getServiceType: Start Execute");
#endif
    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vService_Type,       0, sizeof(vService_Type));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vService_Code,       service_code, strlen(service_code));
    vService_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Service_Type) FROM sSrvCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getServiceType", "", "getServiceType:vSQL_Text = [%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Service_Type) FROM sSrvCode ");
    strcat(vSQL_Text," WHERE Service_Code = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sSrvCode2 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sSrvCode2 CURSOR FOR Prepare_sSrvCode2;
    EXEC SQL OPEN Cursor_sSrvCode2 using :vService_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sSrvCode2 INTO :vService_Type;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getServiceType", "", "读sSrvCode服务字典表失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_sSrvCode2;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sSrvCode2;

    /*
     * 若服务类型为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vService_Type)) <=0 )
       return -2;
    else
    {
         strncpy(service_type, vService_Type, strlen(vService_Type));
         service_type[strlen(vService_Type)]='\0';
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServiceType", "", "getServiceType: End Execute");
#endif
         return 0;
    }
}


/*
 * getPriceName  提取价格名称函数
 *
 * 功能: 根据价格代码提取价格名称
 *
 * input参数1: *price_code，服务代码，长度4个字节；
 *
 * output参数2: *price_name，服务名称，长度20个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *
 *
 */

int getPriceName(char *price_code,char *price_name)
{
          char vPrice_Code[4+1];         /* 价格代码     */
          char vPrice_Name[20+1];        /* 价格名称     */
          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getPriceName", "", "getPriceName: Start Execute");
#endif
    memset(vPrice_Code,         0, sizeof(vPrice_Code));
    memset(vPrice_Name,         0, sizeof(vPrice_Name));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vPrice_Code,       price_code, strlen(price_code));
    vPrice_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Price_Name) FROM sPriceCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Price_Code = trim('%s') ",vTmp_String,vPrice_Code);

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Price_Name) FROM sPriceCode ");
    strcat(vSQL_Text," WHERE Price_Code = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sPriceCode FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sPriceCode CURSOR FOR Prepare_sPriceCode;
    EXEC SQL OPEN Cursor_sPriceCode using :vPrice_Code;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sPriceCode INTO :vPrice_Name;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getPriceName", "", "读sPriceCode价格字典表失败!");
                EXEC SQL CLOSE Cursor_sPriceCode;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sPriceCode;

    /*
     * 若价格名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vPrice_Name)) <=0 )
       return -2;
    else
    {
         if (price_name != NULL)
         {
            strncpy(price_name, vPrice_Name, strlen(vPrice_Name));
            price_name[strlen(vPrice_Name)]='\0';
         }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getPriceName", "", "getPriceName: End Execute");
#endif
       return 0;
    }

}

/*
 * getMeanName  提取因子名称函数
 *
 * 功能: 根据价格因子提取因子名称
 *
 * input参数1: *mean_code，因子代码，长度4个字节；
 *
 * output参数2: *mean_type，因子类型，长度1个字节；
 *
 * output参数3: *mean_name，因子名称，长度20个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *
 *
 */

int getMeanName(char *mean_code,char *mean_type,char *mean_name)
{
          char vMean_Code[4+1];          /* 因子代码     */
          char vMean_Type[1+1];          /* 因子类型     */
          char vMean_Name[20+1];         /* 因子名称     */
          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMeanName", "", "getMeanName: Start Execute");
#endif
    memset(vMean_Code,          0, sizeof(vMean_Code));
    memset(vMean_Type,          0, sizeof(vMean_Type));
    memset(vMean_Name,          0, sizeof(vMean_Name));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vMean_Code,         mean_code, strlen(mean_code));
    vMean_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Mean_Name),Trim(Mean_Type) FROM sPriceMean ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Price_Mean = trim('%s') ",vTmp_String,vMean_Code);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getMeanName", "", "getMeanName: vSql_String=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Mean_Name),Trim(Mean_Type) FROM sPriceMean ");
    strcat(vSQL_Text," WHERE Price_Mean = trim(:v1) ");


    EXEC SQL PREPARE Prepare_sPriceMean FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sPriceMean CURSOR FOR Prepare_sPriceMean;
    EXEC SQL OPEN Cursor_sPriceMean using :vMean_Code;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sPriceMean INTO :vMean_Name,:vMean_Type;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getMeanName", "", "读sPriceMean价格因子字典表失败!");
                EXEC SQL CLOSE Cursor_sPriceMean;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sPriceMean;

    /*
     * 若因子名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vMean_Name)) <=0 )
       return -2;
    else
    {
         if (mean_name != NULL)
         {
            strncpy(mean_name, vMean_Name, strlen(vMean_Name));
            mean_name[strlen(vMean_Name)]='\0';
         }
    }

    /*
     * 若因子类型为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vMean_Type)) <=0 )
       return -3;
    else
    {

         if (mean_type != NULL)
         {
             strncpy(mean_type, vMean_Type, strlen(vMean_Type));
             mean_type[strlen(vMean_Type)]='\0';
         }
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getMeanName", "", "getMeanName: End Execute");
#endif
    return 0;
}

/*
 * getPriceConfig  根据产品代码、服务代码、价格代码提取价格因子属性
 *
 * 功能:  根据产品代码、服务代码、价格代码提取价格因子属性
 *
 * input参数1:         *product_code，产品代码，8个字节；
 *
 * input参数2:         *service_code，服务代码，4个字节；
 *
 * input参数3:         *price_code，  价格代码, 4个字节；
 *
 * input参数4:         *node_list，   父节点链表, 最大1024个字节；
 *
 * input参数5:         *owner_code，  客户属性, 2个字节；
 *
 * input参数6:         power_right，  营业员权限, int；
 *
 * output参数7: PRICECFG  *config,    价格因子结构体；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1，无数据返回
 *
 */

int getPriceConfig(char *product_code, char *service_code, char *price_code,char *node_list,char *owner_code,int power_right,PRICECFG *config)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: Start Execute");
#endif
          /*
           * 传递变量 commented by lab 2004.11.02
           */

          char   vProduct_Code[8+1];            /* 产品代码     */
          char   vService_Code[4+1];            /* 服务代码     */
          char   vPrice_Code[4+1];              /* 价格代码     */
          char   vNode_List[NODE_LIST+1];       /* 父节点链表   */
          char   vOwner_Code[2+1];              /* 用户类型     */
          int    vPower_Right;                  /* 营业员权限   */

          /*
           *  内部变量 commented by lab 2004.11.02
           */

          char   vNode_Tmp[12+1];               /* 发布节点     */
          char   vProduct_Name[20+1];           /* 产品名称     */
          char   vService_Type[1+1];            /* 服务类型     */
          char   vService_Name[20+1];           /* 服务名称     */
          char   vPrice_Name[20+1];             /* 价格名称     */
          char   vPrice_Mean[4+1];              /* 价格因子     */
          char   vMean_Type[1+1];               /* 因子类型     */
          char   vMean_Name[20+1];              /* 因子名称     */
          char   vPrice_Type[1+1];              /* 取值模式     */
          float  vPrice_Value;                  /* 价格值       */
          int    vMean_Order;                   /* 因子顺序     */
          char   vTime_Flag[1+1];               /* 时间标志     */
          float  vMin_Prepay;                   /* 最小预存款   */
          float  vMax_Prepay;                   /* 最大预存款   */
          float  vMin_Deposit;                  /* 最小押金     */
          float  vMax_Deposit;                  /* 最大押金     */

          char   vSQL_Text[1024+1];             /* SQL语句      */
          char   vTmp_String[NODE_LIST+1];      /* 临时变量     */
          int    vret_code;                     /* 返回值       */
          int    i=0;                           /* 计数器       */
          int    j=0;                           /* 计数器       */
          int    vNum=0;                        /* 计数器       */

          /*
           *  init parameter of begin
           *
           *  commented by lab 2004.11.02
           */

    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vPrice_Code,         0, sizeof(vPrice_Code));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(&vPower_Right,       0, sizeof(vPower_Right));

    memset(vNode_Tmp,           0, sizeof(vNode_Tmp));
    memset(vProduct_Name,       0, sizeof(vProduct_Name));
    memset(vService_Type,       0, sizeof(vService_Type));
    memset(vService_Name,       0, sizeof(vService_Name));
    memset(vPrice_Name,         0, sizeof(vPrice_Name));
    memset(vPrice_Mean,         0, sizeof(vPrice_Mean));
    memset(vMean_Type,          0, sizeof(vMean_Type));
    memset(vMean_Name,          0, sizeof(vMean_Name));
    memset(vPrice_Type,         0, sizeof(vPrice_Type));
    memset(&vPrice_Value,       0, sizeof(vPrice_Value));
    memset(&vMean_Order,        0, sizeof(vMean_Order));
    memset(vTime_Flag,          0, sizeof(vTime_Flag));
    memset(&vMin_Prepay,        0, sizeof(vMin_Prepay));
    memset(&vMax_Prepay,        0, sizeof(vMax_Prepay));
    memset(&vMin_Deposit,       0, sizeof(vMin_Deposit));
    memset(&vMax_Deposit,       0, sizeof(vMax_Deposit));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;
    vNum                     =  0;

          /*
           *  init parameter of end
           *
           *  commented by lab 2004.11.02
           */

    /*
     *  将部分传递变量赋值到内部变量中；
     */

     strncpy(vProduct_Code,      product_code,   strlen(product_code));
     vProduct_Code[8] = '\0';

     strncpy(vService_Code,      service_code,   strlen(service_code));
     vService_Code[4] = '\0';

     strncpy(vPrice_Code,        price_code,     strlen(price_code));
     vPrice_Code[4] = '\0';

     strncpy(vNode_List,         node_list,      strlen(node_list));
     vNode_List[NODE_LIST] = '\0';

     strncpy(vOwner_Code,        owner_code,     strlen(owner_code));
     vOwner_Code[2] = '\0';

     vPower_Right             =  power_right;


    /*
     *  根据产品代码、服务代码、价格代码从价格因子表sPriceConfig表中提取价格因子属性
     */

     strcpy(vSQL_Text,"\0");

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Service_Type,Price_Mean,SvrMean_Order,Time_Flag,",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s Min_Prepay,Max_Prepay,Min_Deposit,Max_Deposit, ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s Price_Value,Price_Type,''''||Trim(Group_Id)||'''' ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s FROM sPriceConfig ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Service_Code = trim('%s') ",vTmp_String,vService_Code);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Price_Code = trim('%s') ",vTmp_String,vPrice_Code);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Owner_List LIKE '%%%s%%' ",vTmp_String,vOwner_Code);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Power_Right <= %d ",vTmp_String,vPower_Right);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Begin_Time <= SysDate ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND End_Time   >= SysDate ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s Order By SvrMean_Order ",vTmp_String);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif


     EXEC SQL PREPARE Prepare_sPriceConfig FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_sPriceConfig CURSOR FOR Prepare_sPriceConfig;
     EXEC SQL OPEN Cursor_sPriceConfig;
     for(;;)
     {
            EXEC SQL FETCH Cursor_sPriceConfig
                           INTO :vService_Type,:vPrice_Mean,:vMean_Order,
                                :vTime_Flag,   :vMin_Prepay,:vMax_Prepay,
                                :vMin_Deposit, :vMax_Deposit,
                                :vPrice_Value, :vPrice_Type,:vNode_Tmp;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: 读sPriceConfig价格因子代码表失败!,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sPriceConfig;
                  return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            /*
             * 过滤符合适用用户、网点类型、权限、有效时间、发布节点的产品
             *
             *
             *  commented by lab 2004.11.23
             */

            if (strstr(vNode_List,   rtrim(vNode_Tmp))  == NULL) continue;


            vret_code = getProductName(vProduct_Code,vProduct_Name);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sPriceConfig;
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: 提取产品名称失败，产品代码＝[%s]",vProduct_Code);
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getProductName: vret_code＝[%d]",vret_code);
                   return -2;
            }

            vret_code = getServiceName(vService_Code,vService_Name);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sPriceConfig;
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: 提取服务名称失败，服务代码＝[%s]",vService_Code);
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getServiceName: vret_code＝[%d]",vret_code);
                   return -3;
            }

            vret_code = getPriceName(vPrice_Code,vPrice_Name);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sPriceConfig;
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: 提取价格名称失败，价格代码＝[%s]",vPrice_Code);
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceName: vret_code＝[%d]",vret_code);
                   return -4;
            }

            vret_code = getMeanName(vPrice_Mean,vMean_Type,vMean_Name);

            if (vret_code != 0)
            {
                   EXEC SQL CLOSE Cursor_sPriceConfig;
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: 提取因子名称失败，因子代码＝[%s]",vPrice_Mean);
                   pubLog_Debug(_FFL_, "getPriceConfig", "", "getProductName: vret_code＝[%d]",vret_code);
                   return -5;
            }

            strncpy((config+i)->vProduct_Code,  vProduct_Code, strlen(vProduct_Code));
            strncpy((config+i)->vProduct_Name,  vProduct_Name, strlen(vProduct_Name));
            strncpy((config+i)->vService_Type,  vService_Type, strlen(vService_Type));
            strncpy((config+i)->vService_Code,  vService_Code, strlen(vService_Code));
            strncpy((config+i)->vService_Name,  vService_Name, strlen(vService_Name));
            strncpy((config+i)->vPrice_Code,    vPrice_Code,   strlen(vPrice_Code));
            strncpy((config+i)->vPrice_Name,    vPrice_Name,   strlen(vPrice_Name));
            strncpy((config+i)->vPrice_Mean,    vPrice_Mean,   strlen(vPrice_Mean));
            strncpy((config+i)->vMean_Type,     vMean_Type,    strlen(vMean_Type));
            strncpy((config+i)->vMean_Name,     vMean_Name,    strlen(vMean_Name));
            strncpy((config+i)->vPrice_Type,    vPrice_Type,   strlen(vPrice_Type));
            strncpy((config+i)->vTime_Flag,     vTime_Flag,    strlen(vTime_Flag));

            (config+i)->vPrice_Value          = vPrice_Value;
            (config+i)->vMean_Order           = vMean_Order;
            (config+i)->vMin_Prepay           = vMin_Prepay;
            (config+i)->vMax_Prepay           = vMax_Prepay;
            (config+i)->vMin_Deposit          = vMin_Deposit;
            (config+i)->vMax_Deposit          = vMax_Deposit;

            i++;
     }
     EXEC SQL CLOSE Cursor_sPriceConfig;

    /*
     * 若vNum <=0 ,则该产品无法找到合适的数据，返回 1;
     * 若出现其他异常，返回负值；
     *
     * Commented by lab 2004.11.09
     */

    vNum = i;

    if (vNum <= 0)
    	{
		#ifdef _DEBUG_
       		pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: End Execute");
		#endif
       		return 1;
    	}
    else{
		#ifdef _DEBUG_
       		pubLog_Debug(_FFL_, "getPriceConfig", "", "getPriceConfig: End Execute");
		#endif
       		return 0;
       }
}


/*
 * getProdAttrName  提取产品属性代码类型、名称函数
 *
 * 功能: 提取产品属性代码类型、名称函数
 *
 * input参数1:  *attr_code，属性代码，长度2个字节；
 *
 * output参数2: *attr_type，属性类型，长度1个字节；
 *
 * output参数3: *attr_name，属性名称，长度20个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 名称为空；
 *                      2, 类型为空；
 *
 *
 */

int getProdAttrName(char *attr_code,char *attr_type,char *attr_name)
{
          char vAttr_Code[2+1];          /* 属性代码     */
          char vAttr_Type[1+1];          /* 属性类型     */
          char vAttr_Name[20+1];         /* 属性名称     */
          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdAttrName", "", "getProdAttrName: Start Execute");
#endif
      memset(vAttr_Code,          0, sizeof(vAttr_Code));
      memset(vAttr_Type,          0, sizeof(vAttr_Type));
      memset(vAttr_Name,          0, sizeof(vAttr_Name));
      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));

      strncpy(vAttr_Code,         attr_code, strlen(attr_code));
      vAttr_Code[2] = '\0';

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Attr_Name),Trim(Attr_Type) FROM sProductAttrCode ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
#if PROVINCE_RUN != PROVINCE_JILIN
      sprintf(vSQL_Text,"%s WHERE Product_Attr = trim('%s') ",vTmp_String,vAttr_Code);
#else
      sprintf(vSQL_Text,"%s WHERE Product_Attr = '%s' ",vTmp_String,vAttr_Code);
#endif

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getProdAttrName", "", "getProdAttrName: vSql_String=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");
      strcat(vSQL_Text," SELECT Trim(Attr_Name),Trim(Attr_Type) FROM sProductAttrCode ");
#if PROVINCE_RUN != PROVINCE_JILIN
      strcat(vSQL_Text," WHERE Product_Attr = trim(:v1) ");
#else
      strcat(vSQL_Text," WHERE Product_Attr = :v1 ");
#endif


    EXEC SQL PREPARE Prepare_sProductAttrCode FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductAttrCode CURSOR FOR Prepare_sProductAttrCode;
    EXEC SQL OPEN Cursor_sProductAttrCode using :vAttr_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sProductAttrCode INTO :vAttr_Name,:vAttr_Type;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getProdAttrName", "", "读sProductAttrCode产品属性代码字典表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sProductAttrCode;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sProductAttrCode;

    /*
     * 若属性名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vAttr_Name)) <=0 )
       return 1;
    else
    {
         if (attr_name != NULL)
         {
            strncpy(attr_name, vAttr_Name, strlen(vAttr_Name));
            attr_name[strlen(vAttr_Name)]='\0';
         }
    }

    /*
     * 若属性类型为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vAttr_Type)) <=0 )
       return 2;
    else
    {
         if (attr_type != NULL)
         {
             strncpy(attr_type, vAttr_Type, strlen(vAttr_Type));
             attr_type[strlen(vAttr_Type)]='\0';
         }
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdAttrName", "", "getProdAttrName: End Execute");
#endif
    return 0;
}

/*
 * getProductAttr  提取产品属性代码串
 *
 * 功能: 提取产品属性代码串
 *
 * input参数1:  *product_code，产品代码，长度8个字节；
 *
 * output参数2: *product_attr，属性代码串，长度8个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 属性代码串无效；
 *
 *
 */

int getProductAttr(char *product_code,char *product_attr)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductAttr", "", "getProductAttr: Start Execute");
#endif
          char vProduct_Code[8+1];       /* 产品代码     */
          char vProduct_Attr[8+1];       /* 产品属性代码 */

          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */

      memset(vProduct_Code,       0, sizeof(vProduct_Code));
      memset(vProduct_Attr,       0, sizeof(vProduct_Attr));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));

      strncpy(vProduct_Code,      product_code, strlen(product_code));
      vProduct_Code[8] = '\0';

      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Product_Attr) FROM sProductCode ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getProductAttr", "", "getProdAttrName: vSql_String=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");
      strcat(vSQL_Text," SELECT Trim(Product_Attr) FROM sProductCode ");
      strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");


    EXEC SQL PREPARE Prepare_sProductCode2 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductCode2 CURSOR FOR Prepare_sProductCode2;
    EXEC SQL OPEN Cursor_sProductCode2 using :vProduct_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sProductCode2 INTO :vProduct_Attr;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getProductAttr", "", "读sProductCode产品代码字典表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sProductCode2;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sProductCode2;

    /*
     * 若属性名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vProduct_Attr)) <=0 )
       return -2;
    else
    {
         if (product_attr != NULL)
         {
            strncpy(product_attr,  vProduct_Attr, strlen(vProduct_Attr));
            product_attr[strlen(vProduct_Attr)]='\0';
         }
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductAttr", "", "getProductAttr: End Execute");
#endif
    return 0;
}


/*
 * getProdAttrCode  提取产品属性代码串提取产品属性代码组合
 *
 * 功能: 提取产品属性代码串
 *
 * input参数1:  *product_code，产品代码，长度8个字节；
 *
 * output参数2: *product_attr，属性代码串，长度8个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 属性代码串无效；
 *
 *
 */

int getProductAttrCode(char *product_attr,PRODATTR *attr)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode: Start Execute");
#endif
          char vProduct_Attr[8+1];       /* 产品属性代码 */
          char vAttr_Type[1+1];          /* 属性类型     */
          char vAttr_Code[2+1];          /* 属性代码     */
          char vAttr_Name[20+1];         /* 属性名称     */
          char vIs_Bill[1+1];            /* 集团是否计费 */

          char vSQL_Text[1024+1];        /* SQL语句      */
          char vTmp_String[1024+1];      /* 临时变量     */
          int  vret_code=0;              /* 返回值       */
          int  max=0;                    /* 临时变量     */
          int  vNum=0;                   /* 临时变量     */

      memset(vProduct_Attr,       0, sizeof(vProduct_Attr));
      memset(vAttr_Type,          0, sizeof(vAttr_Type));
      memset(vAttr_Code,          0, sizeof(vAttr_Code));
      memset(vAttr_Name,          0, sizeof(vAttr_Name));
      memset(vIs_Bill,            0, sizeof(vIs_Bill));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&max,                0, sizeof(max));

      strncpy(vProduct_Attr,      product_attr, strlen(product_attr));
      vProduct_Attr[8] = '\0';

      vret_code = 0;
      max       = 0;
      vNum      = 0;

      /*
       * 产品属性值结构体已使用行数
       *
       * commented by lab 2004.11.23
       */

       max = getMaxLineOfProdAttr(attr);

       if (max < 0)
       {
           pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode: 提取产品属性结构体使用行数失败");
           pubLog_Debug(_FFL_, "getProductAttrCode", "", "getMaxLineOfProdAttr: vret_code＝[%d]",max);
           return -3;
       }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode:\tmax=[%d]",max);
#endif


      strcpy(vSQL_Text,"\0");

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Attr_Type),Trim(Product_AttrCode),Trim(IsBill) ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s FROM sProductAttr ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Product_Attr = trim('%s') ",vTmp_String,vProduct_Attr);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode: vSql_String=[%s]",vSQL_Text);
#endif

      strcpy(vSQL_Text,"\0");
      strcat(vSQL_Text," SELECT Trim(Attr_Type),Trim(Product_AttrCode),Trim(IsBill) ");
      strcat(vSQL_Text," FROM sProductAttr ");
      strcat(vSQL_Text," WHERE Product_Attr = trim(:v1) ");


    EXEC SQL PREPARE Prepare_sProductAttr FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductAttr CURSOR FOR Prepare_sProductAttr;
    EXEC SQL OPEN Cursor_sProductAttr using :vProduct_Attr;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sProductAttr INTO :vAttr_Type,:vAttr_Code,:vIs_Bill;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getProductAttrCode", "", "读sProductAttr产品属性表失败,SQLCODE=[%d]!",SQLCODE);
                EXEC SQL CLOSE Cursor_sProductAttr;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          vret_code = getProdAttrName(vAttr_Code, NULL, vAttr_Name);

          if (vret_code != 0)
          {
                   EXEC SQL CLOSE Cursor_sProductAttr;
                   pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode: 提取产品属性代码名称失败，产品属性＝[%s]",vProduct_Attr);
                   pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProdAttrName: vret_code＝[%d]",vret_code);
                   return -2;
          }

          strncpy((attr+max) -> vProduct_Attr,  vProduct_Attr,  strlen(vProduct_Attr));
          strncpy((attr+max) -> vAttr_Type,     vAttr_Type,     strlen(vAttr_Type));
          strncpy((attr+max) -> vAttr_Code,     vAttr_Code,     strlen(vAttr_Code));
          strncpy((attr+max) -> vAttr_Name,     vAttr_Name,     strlen(vAttr_Name));
          strncpy((attr+max) -> vIs_Bill,       vIs_Bill,       strlen(vIs_Bill));

          vNum++;
          max++;
    }
    EXEC SQL CLOSE Cursor_sProductAttr;

    /*
     * 若属性代码无数据返回，则返回 1;
     *
     * Commented by lab 2004.11.01
     */

    if (vNum <= 0)
       return 1;
    else{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductAttrCode", "", "getProductAttrCode: End Execute");
#endif
       return 0;
       }
}


/*
 * getProdReleaseInfo  根据产品代码从产品发布表提取相关数据
 *
 * 功能:   根据产品代码从产品发布表提取相关数据
 *
 * input 参数1: *product_code，产品代码，长度8个字节；
 *
 * input 参数2: NODE *node,    全部节点结构体
 *
 * input 参数3: *node_list,    父节点组, 长度1024字节；
 *
 * input 参数4: *owner_code,   用户属性；2字节
 *
 * input 参数5: *class_code,   营业厅类型；2字节
 *
 * input 参数6: *credit_code,  信用约束；2字节
 *
 * input 参数7: power_right,   营业员权限，int
 *
 * output参数1: *prepay_least,  预存约束，float
 *
 * output参数2: *deposit_least, 押金约束，float
 *
 * output参数3: *mark_least,    积分约束，float
 *
 * output参数4: *product_note, 产品说明, 长度100字节；
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功;
 *                      1，返回记录为空行；
 *
 */

int getProdReleaseInfo(char *product_code,NODE *node, char *node_list, char *owner_code, char *class_code, char *credit_code, int power_right,float *prepay_least,float *deposit_least, float *mark_least, char *product_note)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo: Start Execute");
#endif
          char  vProduct_Code[8+1];            /* 主产品代码  (input)  */
          char  vNode_List[NODE_LIST+1];       /* 父节点组    (input)  */
          char  vOwner_Code[2+1];              /* 机主属性代码(input)  */
          char  vClass_Code[2+1];              /* 营业厅类型  (input)  */
          char  vCredit_Code[2+1];             /* 信用约束代码(input)  */
          int   vPower_Right;                  /* 营业员权限  (input)  */

          float vPrepay_Least;                 /* 预存约束    (output) */
          float vDeposit_Least;                /* 押金约束    (output) */
          float vMark_Least;                   /* 积分约束    (output) */
          char  vProduct_Note[100+1];          /* 产品说明    (output) */

          char  vOwner_List_Tmp[255+1];        /* 产品适用用户群       */
          char  vClass_List_Tmp[255+1];        /* 产品使用网点群       */
          char  vCredit_Least_Tmp[255+1];      /* 产品信用约束群       */
          int   vPower_Right_Tmp;              /* 产品权限             */
          char  vBegin_Time_Tmp[17+1];         /* 产品开始时间         */
          char  vEnd_Time_Tmp[17+1];           /* 产品结束时间         */

          float vPrepay_Least_Tmp;             /* 预存约束Tmp          */
          float vDeposit_Least_Tmp;            /* 押金约束Tmp          */
          float vMark_Least_Tmp;               /* 积分约束Tmp          */
          char  vProduct_Note_Tmp[100+1];      /* 产品说明Tmp          */

          char  vSysDate[17+1];                /* 系统时间             */
          char  vNode_Tmp[12+1];               /* 发布节点             */
          char  vNode_Tmp2[12+1];              /* 发布节点Tmp          */
          int   vNode_Level;                   /* 节点深度             */
          int   vNode_Level_Tmp;               /* 节点深度临时变量     */


          char vSQL_Text[1024+1];             /* SQL语句               */
          char vTmp_String[NODE_LIST+1];      /* 临时变量              */
          int  vret_code;                     /* 返回值                */
          int  vNum=0;                        /* 计数器                */


         /*
          * init parameter of begin
          *
          * commented by lab 2004.11.02
          */

    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vClass_Code,         0, sizeof(vClass_Code));
    memset(vCredit_Code,        0, sizeof(vCredit_Code));
    memset(&vPower_Right,       0, sizeof(vPower_Right));

    memset(&vPrepay_Least,      0, sizeof(vPrepay_Least));
    memset(&vDeposit_Least,     0, sizeof(vDeposit_Least));
    memset(&vMark_Least,        0, sizeof(vMark_Least));
    memset(vProduct_Note,       0, sizeof(vProduct_Note));

    memset(vOwner_List_Tmp,     0, sizeof(vOwner_List_Tmp));
    memset(vClass_List_Tmp,     0, sizeof(vClass_List_Tmp));
    memset(vCredit_Least_Tmp,   0, sizeof(vCredit_Least_Tmp));
    memset(&vPower_Right_Tmp,   0, sizeof(vPower_Right_Tmp));
    memset(vBegin_Time_Tmp,     0, sizeof(vBegin_Time_Tmp));
    memset(vEnd_Time_Tmp,       0, sizeof(vEnd_Time_Tmp));

    memset(&vPrepay_Least_Tmp,  0, sizeof(vPrepay_Least_Tmp));
    memset(&vDeposit_Least_Tmp, 0, sizeof(vDeposit_Least_Tmp));
    memset(&vMark_Least_Tmp,    0, sizeof(vMark_Least_Tmp));
    memset(vProduct_Note_Tmp,   0, sizeof(vProduct_Note_Tmp));

    memset(vSysDate,            0, sizeof(vSysDate));
    memset(vNode_Tmp,           0, sizeof(vNode_Tmp));
    memset(vNode_Tmp2,          0, sizeof(vNode_Tmp2));

    memset(&vNode_Level,        0, sizeof(vNode_Level));
    memset(&vNode_Level_Tmp,    0, sizeof(vNode_Level_Tmp));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;
    vNum                     =  0;


         /*
          * init parameter of end
          *
          * commented by lab 2004.11.02
          */

    /*
     *  将传递变量赋值到内部变量中；
     */

    strncpy(vProduct_Code,       product_code, strlen(product_code));
    vProduct_Code[8] = '\0';

    strncpy(vNode_List,          node_list,    strlen(node_list));
    vNode_List[NODE_LIST] = '\0';

    strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
    vOwner_Code[2] = '\0';

    strncpy(vClass_Code,         class_code,   strlen(class_code));
    vOwner_Code[2] = '\0';

    strncpy(vCredit_Code,        credit_code,  strlen(credit_code));
    vCredit_Code[2] = '\0';

    vPower_Right              =  power_right;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t产品代码=[%s]",vProduct_Code);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t父节点列表=[%s]",vNode_List);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t机主属性代码=[%s]",vOwner_Code);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t营业厅类型=[%s]",vClass_Code);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t信用约束代码=[%s]",vCredit_Code);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t营业员权限=[%d]",vPower_Right);
#endif

    /*
     *  根据产品代码从产品发布表中提取相关信息；
     *
     *  commented by lab 2004.11.02
     */

     strcpy(vSQL_Text,"\0");

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Note,Prepay_Least,Deposit_Least,Mark_Least, ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s Trim(Owner_List),Trim(Class_List),Trim(Credit_Least),Power_Right, ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(SysDate,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s ''''||Trim(Group_Id)||'''',Trim(Group_Id) ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s FROM sProductRelease ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Product_Code=trim('%s') ",vTmp_String,vProduct_Code);


#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "debug msg:\tDebug_Statment=[%s]",vSQL_Text);
#endif

     strcpy(vSQL_Text,"\0");
     strcat(vSQL_Text," SELECT Note,Prepay_Least,Deposit_Least,Mark_Least, ");
     strcat(vSQL_Text," Trim(Owner_List),Trim(Class_List),Trim(Credit_Least),Power_Right, ");
     strcat(vSQL_Text," To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(SysDate,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," ''''||Trim(Group_Id)||'''',Trim(Group_Id) ");
     strcat(vSQL_Text," FROM sProductRelease ");
     strcat(vSQL_Text," WHERE Product_Code=trim(:v1) ");

     EXEC SQL PREPARE Prepare_sProductRelease FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_sProductRelease CURSOR FOR Prepare_sProductRelease;
     EXEC SQL OPEN Cursor_sProductRelease using :vProduct_Code;
     for(;;)
     {
           EXEC SQL FETCH Cursor_sProductRelease
                    INTO :vProduct_Note_Tmp,  :vPrepay_Least_Tmp,
                         :vDeposit_Least_Tmp, :vMark_Least_Tmp,
                         :vOwner_List_Tmp,    :vClass_List_Tmp,
                         :vCredit_Least_Tmp,  :vPower_Right_Tmp,
                         :vBegin_Time_Tmp,    :vEnd_Time_Tmp,
                         :vSysDate,           :vNode_Tmp,
                         :vNode_Tmp2;

           if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
           {
                     pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:读sProductRelease产品发布表失败!,SQLCODE=[%d]",SQLCODE);
                     EXEC SQL CLOSE Cursor_sProductRelease;
                     return -1;
           }

           if (SQLCODE == NOTFOUND)
              break;

          /*
           * 过滤符合适用用户、网点类型、权限、有效时间、发布节点的产品
           *
           *  commented by lab 2004.11.23
           */

           if (strstr(vNode_List,   rtrim(vNode_Tmp))  == NULL) continue;
           if (strstr(vOwner_List_Tmp,   vOwner_Code)  == NULL) continue;
           if (strstr(vClass_List_Tmp,   vClass_Code)  == NULL) continue;
           if (strstr(vCredit_Least_Tmp, vCredit_Code) == NULL) continue;
           if (vPower_Right_Tmp        > vPower_Right)          continue;
           if (strncmp(vBegin_Time_Tmp,vSysDate,17) > 0)        continue;
           if (strncmp(vEnd_Time_Tmp,  vSysDate,17) < 0)        continue;

           vNode_Level_Tmp = getNodeLevel(rtrim(vNode_Tmp2), node);


           pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvNode=[%s],old_level=[%d],new_level=[%d]",vNode_Tmp2,vNode_Level,vNode_Level_Tmp);

           if    (vNode_Level_Tmp <= vNode_Level)               continue;
           else
           {

           pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvPrepay_Least=[%10.2f]",vPrepay_Least_Tmp);
           pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvDeposit_Least=[%10.2f]",vDeposit_Least_Tmp);
           pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvMark_Least=[%10.2f]",vMark_Least_Tmp);
           pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t产品描述=[%s]",vProduct_Note_Tmp);

                 vNode_Level  = vNode_Level_Tmp;

                 strncpy(vProduct_Note, vProduct_Note_Tmp, strlen(vProduct_Note_Tmp));
                 vPrepay_Least        = vPrepay_Least_Tmp;
                 vDeposit_Least       = vDeposit_Least_Tmp;
                 vMark_Least          = vMark_Least_Tmp;
           }

           vNum++;
     }
     EXEC SQL CLOSE Cursor_sProductRelease;


    /*
     * 若vNum <=0 ,则该产品无法找到合适的数据，返回 1;
     * 若vNum > 1; 则该产品找到合适的数据重叠，返回 2;
     * 若vNum = 1; 则该产品找到了核实的数据，  返回 0;
     * 若出现其他异常，返回负值；
     *
     * Commented by lab 2004.11.01
     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvPrepay_Least=[%10.2f]",vPrepay_Least);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvDeposit_Least=[%10.2f]",vDeposit_Least);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvMark_Least=[%10.2f]",vMark_Least);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\t产品描述=[%s]",vProduct_Note);
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo:\tvNum =[%d]",vNum);
#endif


    if (vNum <= 0)
       return 1;
    else
    {

         if (product_note != NULL)
         {
             strncpy(product_note, vProduct_Note, strlen(vProduct_Note));
             product_note[strlen(rtrim(vProduct_Note))]='\0';
         }

         if (prepay_least  != NULL)       *prepay_least  = vPrepay_Least;
         if (deposit_least != NULL)       *deposit_least = vDeposit_Least;
         if (mark_least    != NULL)       *mark_least    = vMark_Least;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProdReleaseInfo", "", "getProdReleaseInfo: End Execute");
#endif
         return 0;
    }
}


/*
 * getServReleaseInfo  根据服务代码从服务发布表提取相关数据
 *
 * 功能:   根据服务代码从服务发布表提取相关数据
 *
 * input 参数1: *service_code，服务代码，长度4个字节；
 *
 * input 参数2: NODE *node,    全部节点结构体
 *
 * input 参数3: *node_list,    父节点组, 长度1024字节；
 *
 * input 参数4: *owner_code,   用户属性  2个字节；
 *
 * input 参数5: *class_code,   营业厅类型, 2个字节；
 *
 * input 参数6: *credit_code,  信用约束,  2个字节；
 *
 * input 参数7: power_right,   营业员权限；
 *
 * output参数1: *prepay_least,  预存约束，float
 *
 * output参数2: *deposit_least, 押金约束，float
 *
 * output参数3: *mark_least,    积分约束，float
 *
 * output参数4: *service_note, 服务说明, 长度100字节；
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功;
 *                      1，返回记录为空行；
 *
 */

int getServReleaseInfo(char *service_code,NODE *node, char *node_list, char *owner_code, char *class_code, char *credit_code, int power_right,float *prepay_least,float *deposit_least, float *mark_least,char *price_code,char *service_note)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo: Start Execute");
#endif
          char  vService_Code[4+1];            /* 服务代码    (input)  */
          char  vNode_List[NODE_LIST+1];       /* 父节点组    (input)  */
          char  vOwner_Code[2+1];              /* 机主属性代码(input)  */
          char  vClass_Code[2+1];              /* 营业厅类型  (input)  */
          char  vCredit_Code[2+1];             /* 信用约束代码(input)  */
          int   vPower_Right;                  /* 营业员权限  (input)  */

          float vPrepay_Least;                 /* 预存约束    (output) */
          float vDeposit_Least;                /* 押金约束    (output) */
          float vMark_Least;                   /* 积分约束    (output) */
          char  vPrice_Code[4+1];              /* 价格代码    (output) */
          char  vService_Note[100+1];          /* 服务说明    (output) */

          char  vOwner_List_Tmp[255+1];        /* 产品适用用户群       */
          char  vClass_List_Tmp[255+1];        /* 产品使用网点群       */
          char  vCredit_Least_Tmp[255+1];      /* 产品信用约束群       */
          int   vPower_Right_Tmp;              /* 产品权限             */
          char  vBegin_Time_Tmp[17+1];         /* 产品开始时间         */
          char  vEnd_Time_Tmp[17+1];           /* 产品结束时间         */
          float vPrepay_Least_Tmp;             /* 预存约束Tmp          */
          float vDeposit_Least_Tmp;            /* 押金约束Tmp          */
          float vMark_Least_Tmp;               /* 积分约束Tmp          */
          char  vPrice_Code_Tmp[4+1];          /* 价格代码Tmp          */
          char  vService_Note_Tmp[100+1];      /* 服务说明Tmp          */

          char  vSysDate[17+1];                /* 系统时间             */
          char  vNode_Tmp[12+1];               /* 发布节点             */
          char  vNode_Tmp2[12+1];              /* 发布节点Tmp          */
          int   vNode_Level;                   /* 节点深度             */
          int   vNode_Level_Tmp;               /* 节点深度临时变量     */


          char vSQL_Text[1024+1];              /* SQL语句               */
          char vTmp_String[NODE_LIST+1];       /* 临时变量              */
          int  vret_code;                      /* 返回值                */
          int  vNum=0;                         /* 计数器                */


         /*
          * init parameter of begin
          *
          * commented by lab 2004.11.02
          */

    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vClass_Code,         0, sizeof(vClass_Code));
    memset(vCredit_Code,        0, sizeof(vCredit_Code));
    memset(&vPower_Right,       0, sizeof(vPower_Right));

    memset(&vPrepay_Least,      0, sizeof(vPrepay_Least));
    memset(&vDeposit_Least,     0, sizeof(vDeposit_Least));
    memset(&vMark_Least,        0, sizeof(vMark_Least));
    memset(vPrice_Code,         0, sizeof(vPrice_Code));
    memset(vService_Note,       0, sizeof(vService_Note));

    memset(vOwner_List_Tmp,     0, sizeof(vOwner_List_Tmp));
    memset(vClass_List_Tmp,     0, sizeof(vClass_List_Tmp));
    memset(vCredit_Least_Tmp,   0, sizeof(vCredit_Least_Tmp));
    memset(&vPower_Right_Tmp,   0, sizeof(vPower_Right_Tmp));
    memset(vBegin_Time_Tmp,     0, sizeof(vBegin_Time_Tmp));
    memset(vEnd_Time_Tmp,       0, sizeof(vEnd_Time_Tmp));
    memset(&vPrepay_Least_Tmp,  0, sizeof(vPrepay_Least_Tmp));
    memset(&vDeposit_Least_Tmp, 0, sizeof(vDeposit_Least_Tmp));
    memset(&vMark_Least_Tmp,    0, sizeof(vMark_Least_Tmp));
    memset(vPrice_Code_Tmp,     0, sizeof(vPrice_Code_Tmp));
    memset(vService_Note_Tmp,   0, sizeof(vService_Note_Tmp));

    memset(vSysDate,            0, sizeof(vSysDate));
    memset(vNode_Tmp,           0, sizeof(vNode_Tmp));
    memset(vNode_Tmp2,          0, sizeof(vNode_Tmp2));
    memset(&vNode_Level,        0, sizeof(vNode_Level));
    memset(&vNode_Level_Tmp,    0, sizeof(vNode_Level_Tmp));

    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;
    vNum                     =  0;


         /*
          * init parameter of end
          *
          * commented by lab 2004.11.02
          */

    /*
     *  将传递变量赋值到内部变量中；
     */

    strncpy(vService_Code,       service_code, strlen(service_code));
    vService_Code[4] = '\0';

    strncpy(vNode_List,          node_list,    strlen(node_list));
    vNode_List[NODE_LIST] = '\0';

    strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
    vOwner_Code[2] = '\0';

    strncpy(vClass_Code,         class_code,   strlen(class_code));
    vClass_Code[2] = '\0';

    strncpy(vCredit_Code,        credit_code,  strlen(credit_code));
    vCredit_Code[2] = '\0';

    vPower_Right              =  power_right;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t服务代码=[%s]",vService_Code);
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t父节点列表=[%s]",vNode_List);
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t机主属性代码=[%s]",vOwner_Code);
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t营业厅类型=[%s]",vClass_Code);
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t信用约束代码=[%s]",vCredit_Code);
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:\t营业员权限=[%d]",vPower_Right);
#endif

    /*
     *  根据服务代码从服务发布表中提取相关信息；
     *
     *  commented by lab 2004.11.02
     */

     strcpy(vSQL_Text,"\0");

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Trim(Note),Prepay_Least,Deposit_Least,Mark_Least,Trim(Price_Code), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s Trim(Owner_List),Trim(Class_List),Trim(Credit_Least),Power_Right, ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(SysDate,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s ''''||Trim(Group_Id)||'''',Trim(Group_Id) ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s FROM sSrvRelease ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Service_Code=trim('%s') ",vTmp_String,vService_Code);

#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getServReleaseInfo", "", "debug msg:\tDebug_Statment=[%s]",vSQL_Text);
#endif

     strcpy(vSQL_Text,"\0");
     strcat(vSQL_Text," SELECT Trim(Note),Prepay_Least,Deposit_Least,Mark_Least,Trim(Price_Code), ");
     strcat(vSQL_Text," Trim(Owner_List),Trim(Class_List),Trim(Credit_Least),Power_Right, ");
     strcat(vSQL_Text," To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(SysDate,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," ''''||Trim(Group_Id)||'''',Trim(Group_Id) ");
     strcat(vSQL_Text," FROM sSrvRelease ");
     strcat(vSQL_Text," WHERE Service_Code=trim(:v1) ");


     EXEC SQL PREPARE Prepare_sSrvRelease FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_sSrvRelease CURSOR FOR Prepare_sSrvRelease;
     EXEC SQL OPEN Cursor_sSrvRelease using :vService_Code;
     for(;;)
     {
           EXEC SQL FETCH Cursor_sSrvRelease
                    INTO :vService_Note_Tmp,  :vPrepay_Least_Tmp,
                         :vDeposit_Least_Tmp, :vMark_Least_Tmp,
                         :vPrice_Code_Tmp,
                         :vOwner_List_Tmp,    :vClass_List_Tmp,
                         :vCredit_Least_Tmp,  :vPower_Right_Tmp,
                         :vBegin_Time_Tmp,    :vEnd_Time_Tmp,
                         :vSysDate,           :vNode_Tmp,
                         :vNode_Tmp2;


           if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
           {
                     pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo:读sSrvRelease服务发布表失败!,SQLCODE=[%d]",SQLCODE);
                     EXEC SQL CLOSE Cursor_sSrvRelease;
                     return -1;
           }

           if (SQLCODE == NOTFOUND)
              break;

          /*
           * 过滤符合适用用户、网点类型、权限、有效时间的服务
           *
           *  commented by lab 2004.11.23
           */

           if (strstr(vNode_List,   rtrim(vNode_Tmp))  == NULL) continue;
           if (strstr(vOwner_List_Tmp,   vOwner_Code)  == NULL) continue;
           if (strstr(vClass_List_Tmp,   vClass_Code)  == NULL) continue;
           if (strstr(vCredit_Least_Tmp, vCredit_Code) == NULL) continue;
           if (vPower_Right_Tmp        > vPower_Right)          continue;
           if (strncmp(vBegin_Time_Tmp,vSysDate,17) > 0)        continue;
           if (strncmp(vEnd_Time_Tmp,  vSysDate,17) < 0)        continue;

           vNode_Level_Tmp = getNodeLevel(rtrim(vNode_Tmp2), node);

           if    (vNode_Level_Tmp <= vNode_Level)               continue;
           else
           {
                 vNode_Level  = vNode_Level_Tmp;

                 strncpy(vService_Note, vService_Note_Tmp, strlen(vService_Note_Tmp));
                 strncpy(vPrice_Code,   vPrice_Code_Tmp,   strlen(vPrice_Code_Tmp));
                 vPrepay_Least        = vPrepay_Least_Tmp;
                 vDeposit_Least       = vDeposit_Least_Tmp;
                 vMark_Least          = vMark_Least_Tmp;
           }

           vNum++;
     }
     EXEC SQL CLOSE Cursor_sSrvRelease;


    /*
     * 若vNum <=0 ,则该产品无法找到合适的数据，返回 1;
     * 若vNum > 1; 则该产品找到合适的数据重叠，返回 2;
     * 若vNum = 1; 则该产品找到了核实的数据，  返回 0;
     * 若出现其他异常，返回负值；
     *
     * Commented by lab 2004.11.01
     */

    if (vNum <= 0)
       return 1;
    else
    {
       if (service_note != NULL)
       {
          strncpy(service_note, vService_Note, strlen(vService_Note));
          service_note[strlen(rtrim(vService_Note))]='\0';
       }

       if (price_code != NULL)
       {
          strncpy(price_code,   vPrice_Code,   strlen(vPrice_Code));
          price_code[strlen(rtrim(vPrice_Code))]='\0';
       }

       if (prepay_least  != NULL)       *prepay_least  = vPrepay_Least;
       if (deposit_least != NULL)       *deposit_least = vDeposit_Least;
       if (mark_least    != NULL)       *mark_least    = vMark_Least;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServReleaseInfo", "", "getServReleaseInfo: End Execute");
#endif
       return 0;
    }
}



/*
 * getAttactProduct  根据主产品提取绑定可选产品函数；
 *
 * 功能:根据主产品提取绑定可选产品函数；
 *
 * input 参数1: *product_code，产品代码，长度8个字节；
 *
 * input 参数2: NODE *node,    全部节点结构体
 *
 * input 参数3: *node_list,    父节点组, 长度1024字节；
 *
 * input 参数4: *owner_code,   用户属性, 长度2个字节；
 *
 * input 参数5: *class_code,   营业厅类型,长度2个字节；
 *
 * input 参数6: *credit_code,  信用约束链表；
 *
 * input 参数7: *business_code, 业务代码
 *                              0:开户，1:添加,2:变更
 *
 * input 参数8: power_right,   营业员权限，int
 *
 *
 *
 * output参数1: PRODUCT *attach_product，产品结构体,维数MAX_ROW；
 *
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功;
 *                      1，结构体overflow,全部返回；
 *
 */

int getAttachProduct(char *product_code,NODE *node, char *node_list,char *owner_code,char *class_code, char *credit_code,char *business_code,int power_right,PRODUCT *attach_product)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: Start Execute");
#endif
     /*
      * 传递变量 commented by lab 2004.11.02
      */

     char vMain_Product[8+1];            /* 主产品代码       */
     char vNode_List[NODE_LIST+1];       /* 父节点链表       */
     char vOwner_Code[2+1];              /* 机主属性         */
     char vClass_Code[2+1];              /* 营业厅类型       */
     char vCredit_Code[2+1];             /* 信用约束链表     */
     int  vPower_Right;                  /* 营业员权限       */
     char vBusiness_Code[1+1];           /* 业务代码         */


     /*
      *  内部变量 commented by lab 2004.11.02
      */

     char vBusiness_Code_Tmp[20+1];       /* 业务代码Tmp      */
     char vRule_Status_Tmp[1+1];         /* 规则状态Tmp      */
     char vSQL_Text[1024+1];             /* SQL语句          */
     char vTmp_String[NODE_LIST+1];      /* 临时变量         */
     int  vret_code;                     /* 返回值           */
     int  i=0;                           /* 计数器           */
     int  max=0;                         /* PRODUCT结构体    *
                                          * 已使用最大行数   */
     /*
      *  产品结构体临时变量，担任传递作用  commented by lab 2004.11.02
      */

     char  vProduct_Code[8+1];           /* 产品代码     */
     char  vProduct_Name[20+1];          /* 产品代码     */
     float vPrepay_Least;                /* 预存约束     */
     float vDeposit_Least;               /* 押金约束     */
     float vMark_Least;                  /* 积分约束     */
     char  vProduct_Note[100+1];         /* 产品描述     */

     /*
      *  init parameter of begin
      *
      *  commented by lab 2004.11.02
      */

    memset(vMain_Product,       0, sizeof(vMain_Product));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vClass_Code,         0, sizeof(vClass_Code));
    memset(vCredit_Code,        0, sizeof(vCredit_Code));
    memset(&vPower_Right,       0, sizeof(vPower_Right));
    memset(vBusiness_Code,      0, sizeof(vBusiness_Code));

    memset(vBusiness_Code_Tmp,  0, sizeof(vBusiness_Code_Tmp));
    memset(vRule_Status_Tmp,    0, sizeof(vRule_Status_Tmp));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;

    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Name,       0, sizeof(vProduct_Name));
    memset(&vPrepay_Least,      0, sizeof(vPrepay_Least));
    memset(&vDeposit_Least,     0, sizeof(vDeposit_Least));
    memset(&vMark_Least,        0, sizeof(vMark_Least));
    memset(vProduct_Note,       0, sizeof(vProduct_Note));

    /*
     *  init parameter of end
     *
     *  commented by lab 2004.11.02
     */

    /*
     *  将主产品传递变量赋值到内部变量中；
     */

    strncpy(vMain_Product,       product_code, strlen(product_code));
    vMain_Product[8] = '\0';

    strncpy(vNode_List,          node_list,    strlen(node_list));
    vNode_List[NODE_LIST] = '\0';

    strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
    vOwner_Code[2] = '\0';

    strncpy(vClass_Code,         class_code,   strlen(class_code));
    vOwner_Code[2] = '\0';

    strncpy(vCredit_Code,        credit_code,  strlen(credit_code));
    vCredit_Code[2] = '\0';

    vPower_Right              =  power_right;

    strncpy(vBusiness_Code,      business_code,  strlen(business_code));
    vBusiness_Code[1] = '\0';


#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t主产产品代码=[%s]",vMain_Product);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t父节点列表=[%s]",vNode_List);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t机主属性代码=[%s]",vOwner_Code);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t营业厅类型=[%s]",vClass_Code);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t信用约束代码=[%s]",vCredit_Code);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t营业员权限=[%d]",vPower_Right);
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\t业务代码=[%s]",vBusiness_Code);
#endif

    /*
     *  检索出PRODUCT结构体当前已存储数据的行数，并返回
     *  及相关信息，然后追加到PRODUCT结构体中；
     */

     max = getMaxLineOfProduct(attach_product);

     if (max < 0)
     {
         pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: 提取产品结构体使用行数失败");
         pubLog_Debug(_FFL_, "getAttachProduct", "", "getMaxLineOfProduct: vret_code＝[%d]",max);
         return -1;
     }
     else
     {
           i = max;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct:\tmax=[%d]",max);
#endif
     }


    /*
     *  从主产品绑定可选产品表中sProductAttach表中提取可选产品
     *  及相关信息，然后存放到PRODUCT结构体中；
     */

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Object_Product),Trim(Business_Code),Trim(Rule_Status) ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s FROM sProductAttach ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vMain_Product);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Object_Product),Trim(Business_Code),Trim(Rule_Status) ");
    strcat(vSQL_Text," FROM sProductAttach ");
    strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sProductAttach FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sProductAttach CURSOR FOR Prepare_sProductAttach;
    EXEC SQL OPEN Cursor_sProductAttach using :vMain_Product;
    for(;;)
    {
            EXEC SQL FETCH Cursor_sProductAttach
                           INTO :vProduct_Code,:vBusiness_Code_Tmp,:vRule_Status_Tmp;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: 读sProductAttach主产品绑定表失败!");
                EXEC SQL CLOSE Cursor_sProductAttach;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)  continue;
          if (strncmp(vRule_Status_Tmp,  "Y",         1) != 0)     continue;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: debug msg:\t产品代码[%d]=[%s]",max,vProduct_Code);
#endif

          /*
           * 根据产品代码提取产品名称
           *
           * commented by lab 2004.11.02
           */

          vret_code = getProductName(vProduct_Code,vProduct_Name);

          if (vret_code != 0)
          {
                   EXEC SQL CLOSE Cursor_sProductAttach;
                   pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: 提取产品名称失败，产品代码＝[%s]",vProduct_Code);
                   pubLog_Debug(_FFL_, "getAttachProduct", "", "getProductName: vret_code＝[%d]",vret_code);
                   return -2;
          }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: debug msg:\t产品名称=[%s]",vProduct_Name);
#endif

          vret_code = getProdReleaseInfo(vProduct_Code,node,vNode_List,vOwner_Code,vClass_Code,
                                         vCredit_Code,vPower_Right,
                                         &vPrepay_Least,&vDeposit_Least,
                                         &vMark_Least,vProduct_Note);


          if (vret_code != 0 && vret_code != 1)
          {
                   EXEC SQL CLOSE Cursor_sProductAttach;
                   pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: 提取产品发布信息失败，产品代码＝[%s]",vProduct_Code);
                   pubLog_Debug(_FFL_, "getAttachProduct", "", "getProdReleaseInfo: vret_code＝[%d]",vret_code);
                   return -3;
          }

          /*
           * getProdReleaseInfo提取产品发布信息，当可选产品与主产品存在归属关系，
           * 但子产品没有发布时，getAttachProduct函数不报错，继续向下处理，并放弃该产品数据；
           *
           * commented by lab 2004.11.02
           */

          if (vret_code == 1)   continue;

          strncpy((attach_product+max)->vOld_Sel,        "n",           1);
          strncpy((attach_product+max)->vNew_Sel,        "Y",           1);
          strncpy((attach_product+max)->vProduct_Code,   vProduct_Code, strlen(vProduct_Code));
          strncpy((attach_product+max)->vProduct_Name,   vProduct_Name, strlen(vProduct_Name));
          strncpy((attach_product+max)->vProduct_Note,   vProduct_Note, strlen(vProduct_Note));

          (attach_product+max)->vPrepay_Least          = vPrepay_Least;
          (attach_product+max)->vDeposit_Least         = vDeposit_Least;
          (attach_product+max)->vMark_Least            = vMark_Least;

          max++;

          if (max > MAX_ROW)
          {
                   EXEC SQL CLOSE Cursor_sProductAttach;
                   pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: 产品结构体赋值overflow，MAX_ROW=[%d]",MAX_ROW);
                   return -4;
          }
    }
    EXEC SQL CLOSE Cursor_sProductAttach;

    /*
     * 若产品名称为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachProduct", "", "getAttachProduct: End Execute");
#endif
     return 0;
}


/*
 * getAttactSrvOfSm  根据品牌提取绑定服务函数；
 *
 * 功能:根据品牌提取绑定服务函数；
 *
 * input参数1: *sm_code，   品牌代码，长度2个字节；
 *
 * input参数2: NODE *node,    全部节点结构体
 *
 * input参数3: *node_list， 父节点列表, 长度1024个字节；
 *
 * input参数4: *owner_code, 用户约束,   2个字节；
 *
 * input参数5: *class_code, 营业厅类型, 2个字节；
 *
 * input参数6: *credit_code,信用约束,   2个字节；
 *
 * input参数7: power_right, 营业员权限, 整型
 *
 * output参数1: SERVICE *attach_service
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功;
 *                      1，结构体overflow,全部返回；
 *
 */

int getAttachSrvOfSm(char *sm_code,NODE *node, char *node_list,char *owner_code,char *class_code,char *credit_code,int power_right,SERVICE *attach_service)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: Start Execute");
#endif
          /*
           * 传递变量 commented by lab 2004.11.02
           */

          char vSm_Code[2+1];                 /* 品牌代码         */
          char vNode_List[NODE_LIST+1];       /* 父节点链表       */
          char vOwner_Code[2+1];              /* 机主属性         */
          char vClass_Code[2+1];              /* 营业厅类型       */
          char vCredit_Code[2+1];             /* 信用约束链表     */
          int  vPower_Right;                  /* 营业员权限       */


          /*
           *  内部变量 commented by lab 2004.11.0
           */

      char vChoice_Flag[1+1];             /* 选择标志         */
          char vSQL_Text[1024+1];             /* SQL语句          */
          char vTmp_String[NODE_LIST+1];      /* 临时变量         */
          int  vret_code;                     /* 返回值           */
          int  i=0;                           /* 计数器           */
          int  max=0;                         /* SERVICE结构体    *
                                               * 已使用最大行数   */
          /*
           *  服务结构体临时变量，担任传递作用
           *  commented by lab 2004.11.08
           */

    char   vProduct_Code[8+1];            /* 产品代码       */
    char   vProduct_Name[20+1];           /* 产品名称       */
    char   vService_Code[4+1];            /* 服务代码       */
    char   vService_Name[20+1];           /* 服务名称       */
    char   vPrice_Code[4+1];              /* 价格代码       */
    char   vPrice_Modify[1+1];            /* 个性定价       */
    float  vPrepay_Least;                 /* 预存约束       */
    float  vDeposit_Least;                /* 押金约束       */
    float  vMark_Least;                   /* 积分约束       */
    char   vService_Note[100+1];          /* 服务描述       */

          /*
           *  init parameter of begin
           *
           *  commented by lab 2004.11.02
           */

    memset(vSm_Code,            0, sizeof(vSm_Code));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vClass_Code,         0, sizeof(vClass_Code));
    memset(vCredit_Code,        0, sizeof(vCredit_Code));
    memset(&vPower_Right,       0, sizeof(vPower_Right));

    memset(vChoice_Flag,        0, sizeof(vChoice_Flag));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;


    memset(vProduct_Code,       0, sizeof(vProduct_Code));
    memset(vProduct_Name,       0, sizeof(vProduct_Name));
    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vService_Name,       0, sizeof(vService_Name));
    memset(vPrice_Code,         0, sizeof(vPrice_Code));
    memset(vPrice_Modify,       0, sizeof(vPrice_Modify));
    memset(&vPrepay_Least,      0, sizeof(vPrepay_Least));
    memset(&vDeposit_Least,     0, sizeof(vDeposit_Least));
    memset(&vMark_Least,        0, sizeof(vMark_Least));
    memset(vService_Note,       0, sizeof(vService_Note));

          /*
           *  init parameter of end
           *
           *  commented by lab 2004.11.02
           */

    /*
     *  将品牌传递变量赋值到内部变量中；
     */

    strncpy(vSm_Code,            sm_code,      strlen(sm_code));
    vSm_Code[2] = '\0';

    strncpy(vNode_List,          node_list,    strlen(node_list));
    vNode_List[NODE_LIST] = '\0';

    strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
    vOwner_Code[2] = '\0';

    strncpy(vClass_Code,         class_code,   strlen(class_code));
    vClass_Code[2] = '\0';

    strncpy(vCredit_Code,        credit_code,  strlen(credit_code));
    vCredit_Code[2] = '\0';

    vPower_Right              =  power_right;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t品牌代码=[%s]",vSm_Code);
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t父节点列表=[%s]",vNode_List);
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t机主属性代码=[%s]",vOwner_Code);
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t营业厅类型=[%s]",vClass_Code);
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t信用约束代码=[%s]",vCredit_Code);
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\t营业员权限=[%d]",vPower_Right);
#endif

    /*
     *  检索出SERVICE结构体当前已存储数据的行数，并返回
     *  及相关信息，然后追加到SERVICE结构体中；
     */

     max = getMaxLineOfService(attach_service);

     if (max < 0)
     {
         pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: 提取服务结构体使用行数失败");
         pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getMaxLineOfProduct: vret_code＝[%d]",max);
         return -1;
     }
     else
     {
           i = max;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm:\tmax=[%d]",max);
#endif
     }


    /*
     *  从品牌绑定服务表中sSmSrv表中提取绑定服务
     *  及相关信息，然后存放到SERVICE结构体中；
     */

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Service_Code),Trim(Choice_Flag) FROM sSmSrv ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Sm_Code = trim('%s') ",vTmp_String,vSm_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif


    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Service_Code),Trim(Choice_Flag) FROM sSmSrv ");
    strcat(vSQL_Text," WHERE Sm_Code      = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sSmSrv FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sSmSrv CURSOR FOR Prepare_sSmSrv;
    EXEC SQL OPEN Cursor_sSmSrv using :vSm_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sSmSrv INTO :vService_Code,:vChoice_Flag;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                    pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: 读sSmSrv服务关系表失败!");
                EXEC SQL CLOSE Cursor_sSmSrv;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;

          /*
           * sSmSrv.choice_flag='2'表示绑定；
           *
           * commented by lab 2004.11.02
           */

          if (strncmp(vChoice_Flag, "2", 1) != 0) continue;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: debug msg:\t服务代码[%d]=[%s]",max,vService_Code);
#endif

          /*
           * 根据服务代码提取服务名称
           *
           * commented by lab 2004.11.02
           */

          vret_code = getServiceName(vService_Code,vService_Name);

          if (vret_code != 0)
          {
                   EXEC SQL CLOSE Cursor_sSmSrv;
                   pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: 提取服务名称失败，服务代码＝[%s]",vService_Code);
                   pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getServiceName: vret_code＝[%d]",vret_code);
                   return -2;
          }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: debug msg:\t服务名称=[%s]",vService_Name);
#endif

          /*
           * getServReleaseInfo提取服务发布信息;
           *
           * commented by lab 2004.11.02
           */

          vret_code = getServReleaseInfo(vService_Code,node,vNode_List,vOwner_Code,vClass_Code,
                                         vCredit_Code,vPower_Right,
                                         &vPrepay_Least,&vDeposit_Least,
                                         &vMark_Least,vPrice_Code,vService_Note);


          if (vret_code != 0)
          {
                   EXEC SQL CLOSE Cursor_sSmSrv;
                   pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: 提取服务发布信息失败，服务代码＝[%s]",vService_Code);
                   pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getServReleaseInfo: vret_code＝[%d]",vret_code);
                   return -3;
          }


          strncpy((attach_service+max)->vOld_Sel,        "n",           1);
          strncpy((attach_service+max)->vNew_Sel,        "Y",           1);
          strncpy((attach_service+max)->vProduct_Code,   DEFAULT_PRODUCT_CODE,    8);
          strncpy((attach_service+max)->vProduct_Name,   "品牌服务",    8);
          strncpy((attach_service+max)->vService_Code,   vService_Code, strlen(vService_Code));
          strncpy((attach_service+max)->vService_Name,   vService_Name, strlen(vService_Name));
          strncpy((attach_service+max)->vPrice_Code,     vPrice_Code,   strlen(vPrice_Code));
          strncpy((attach_service+max)->vPrice_Modify,   "N",           1);
          strncpy((attach_service+max)->vService_Note,   vService_Note, strlen(vService_Note));

          (attach_service+max)->vPrepay_Least          = vPrepay_Least;
          (attach_service+max)->vDeposit_Least         = vDeposit_Least;
          (attach_service+max)->vMark_Least            = vMark_Least;

          max++;

          if (max > MAX_ROW)
          {
                   EXEC SQL CLOSE Cursor_sSmSrv;
                   pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: 服务结构体赋值overflow，MAX_ROW=[%d]",MAX_ROW);
                   return -4;
          }
    }
    EXEC SQL CLOSE Cursor_sSmSrv;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getAttachSrvOfSm", "", "getAttachSrvOfSm: End Execute");
#endif
    return 0;
}


/*
 * getProductPrice  根据产品代码、服务代码提取价格代码
 *
 * 功能:  根据产品代码、服务代码提取价格代码
 *
 * input参数1:         *product_code，产品代码，8个字节；
 *
 * input参数2:         *service_code，服务代码，4个字节；
 *
 * output参数3:        *price_code，  价格代码, 4个字节；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1，无数据返回
 *                      2, 返回数据重叠；
 *
 */

int getProductPrice(char *product_code, char *service_code, char *price_code)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductPrice", "", "getProductPrice: Start Execute");
#endif
          /*
           * 传递变量 commented by lab 2004.11.02
           */

          char   vProduct_Code[8+1];    /* 产品代码     */
          char   vService_Code[4+1];    /* 服务代码     */
          char   vPrice_Code[4+1];      /* 价格代码     */


          /*
           *  内部变量 commented by lab 2004.11.02
           */

      char vBegin_Time[17+1];             /* 开始时间Tmp      */
      char vEnd_Time[17+1];               /* 结束时间Tmp      */
      char vSysDate[17+1];                /* 系统时间Tmp      */
          char vSQL_Text[1024+1];             /* SQL语句          */
          char vTmp_String[1024+1];           /* 临时变量         */
          int  vret_code;                     /* 返回值           */
          int  i=0;                           /* 计数器           */
          int  j=0;                           /* 计数器           */
          int  vNum=0;                        /* 计数器           */

          /*
           *  init parameter of begin
           *
           *  commented by lab 2004.11.02
           */

        memset(vProduct_Code,       0, sizeof(vProduct_Code));
        memset(vService_Code,       0, sizeof(vService_Code));
        memset(vPrice_Code,         0, sizeof(vPrice_Code));

        memset(vBegin_Time,         0, sizeof(vBegin_Time));
        memset(vEnd_Time,           0, sizeof(vEnd_Time));
        memset(vSysDate,            0, sizeof(vSysDate));
        memset(vSQL_Text,           0, sizeof(vSQL_Text));
        memset(vTmp_String,         0, sizeof(vTmp_String));
        vret_code                =  0;
        vNum                     =  0;

          /*
           *  init parameter of end
           *
           *  commented by lab 2004.11.02
           */

    /*
     *  将部分传递变量赋值到内部变量中；
     */

     strncpy(vProduct_Code,      product_code,   strlen(product_code));
     vProduct_Code[8] = '\0';

     strncpy(vService_Code,      service_code,   strlen(service_code));
     vService_Code[4] = '\0';

     strncpy(vPrice_Code,        price_code,     strlen(price_code));
     vPrice_Code[4] = '\0';


    /*
     *  根据产品代码、服务代码从产品价格代码表sProductPrice表中提取价格代码
     */

     strcpy(vSQL_Text,"\0");

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s SELECT Trim(Actual_Price), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s To_Char(SysDate,'YYYYMMDD HH24:MI:SS') ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s FROM sProductPrice ",vTmp_String);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

     strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
     sprintf(vSQL_Text,"%s AND Service_Code = trim('%s') ",vTmp_String,vService_Code);


#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductPrice", "", "getProductPrice: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif

     strcpy(vSQL_Text,"\0");
     strcat(vSQL_Text," SELECT Trim(Actual_Price),");
     strcat(vSQL_Text," To_Char(Begin_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(End_Time,'YYYYMMDD HH24:MI:SS'), ");
     strcat(vSQL_Text," To_Char(SysDate,'YYYYMMDD HH24:MI:SS') ");
     strcat(vSQL_Text," FROM sProductPrice ");
     strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");
     strcat(vSQL_Text," AND Service_Code   = trim(:v2) ");


     EXEC SQL PREPARE Prepare_sProductPrice FROM :vSQL_Text;
     EXEC SQL DECLARE Cursor_sProductPrice CURSOR FOR Prepare_sProductPrice;
     EXEC SQL OPEN Cursor_sProductPrice using :vProduct_Code,:vService_Code;
     for(;;)
     {
            EXEC SQL FETCH Cursor_sProductPrice
                      INTO :vPrice_Code,:vBegin_Time,
                           :vEnd_Time,  :vSysDate;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "getProductPrice", "", "getProductPrice: 读sProductPrice产品代码表失败!,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sProductPrice;
                  return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            if (strncmp(vBegin_Time,vSysDate,17) > 0)        continue;
            if (strncmp(vEnd_Time,  vSysDate,17) < 0)        continue;

            vNum ++;
     }
     EXEC SQL CLOSE Cursor_sProductPrice;

    /*
     * 若vNum <=0 ,则该产品无法找到合适的数据，返回 1;
     * 若vNum > 1; 则该产品找到合适的数据重叠，返回 2;
     * 若vNum = 1; 则该产品找到了合适的数据，  返回 0;
     * 若出现其他异常，返回负值；
     *
     * Commented by lab 2004.11.09
     */

    if (vNum <= 0)
       return 1;
    else if (vNum > 1)
       return 2;
    else
    {

         if (price_code != NULL)
         {
             strncpy(price_code, vPrice_Code, strlen(vPrice_Code));
             price_code[strlen(rtrim(vPrice_Code))]='\0';
         }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProductPrice", "", "getProductPrice: End Execute");
#endif
       return 0;
    }
}


/*
 * getServOfProduct  根据主产品提取主产品服务清单及其属性；
 *
 * 功能:  根据主产品提取主产品服务清单及其属性；
 *
 * input参数1:         PRODUCT *product，产品结构体，维数必须为MAX_ROW；
 *
 * input参数2:         NODE *node,    全部节点结构体
 *
 * input参数3:         *node_list,父节点链表，最大1024个字节；
 *
 * input参数4:         *owner_code,用户约束， 2个字节；
 *
 * input参数5:         *class_code,营业厅类型,2个字节；
 *
 * input参数6:         *credit_code,信用约束，2个字节；
 *
 * input参数7:         power_right, 营业员权限；
 *
 * output参数1: SERVICE *service，服务结构体, 维数MAX_ROW；
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1，服务结构体overflow,无法返回；
 *                      2，产品中没有服务配置；
 *
 */

int getServOfProduct(PRODUCT *product, NODE *node, char *node_list, char *owner_code, char *class_code, char *credit_code, int power_right, SERVICE *service)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: Start Execute");
#endif
          /*
           * 传递变量 commented by lab 2004.11.02
           */

          char   vProduct_Code[8+1];      /* 产品代码     */
          char   vNode_List[NODE_LIST+1]; /* 父节点列表   */
          char   vOwner_Code[2+1];      /* 用户类型     */
          char   vClass_Code[2+1];      /* 营业厅类型   */
          char   vCredit_Code[2+1];     /* 信用约束     */
          int    vPower_Right;          /* 营业员权限   */

          char   vService_Code[4+1];    /* 服务代码     */
          char   vService_Name[20+1];   /* 服务名称     */
          char   vService_Note[100+1];  /* 服务说明     */
          char   vPrice_Code[4+1];      /* 价格代码     */
          char   vPrice_Modify[1+1];    /* 个性定价     */
          float  vPrepay_Least;         /* 预存约束     */
          float  vDeposit_Least;        /* 押金约束     */
          float  vMark_Least;           /* 积分约束     */


          /*
           *  内部变量 commented by lab 2004.11.02
           */

          char vSQL_Text[1024+1];             /* SQL语句          */
          char vTmp_String[NODE_LIST+1];      /* 临时变量         */
          int  vret_code;                     /* 返回值           */
          int  i=0;                           /* 计数器           */
          int  j=0;                           /* 计数器           */
          int  vNum=0;                        /* 临时计数器       */
          int  max=0;                         /* SERVICE结构体    *
                                               * 已使用最大行数   */



          /*
           *  init parameter of begin
           *
           *  commented by lab 2004.11.02
           */

       memset(vProduct_Code,       0, sizeof(vProduct_Code));

       memset(vNode_List,          0, sizeof(vNode_List));
       memset(vOwner_Code,         0, sizeof(vOwner_Code));
       memset(vClass_Code,         0, sizeof(vClass_Code));
       memset(vCredit_Code,        0, sizeof(vCredit_Code));
       vPower_Right             =  0;

       memset(vService_Code,       0, sizeof(vService_Code));
       memset(vService_Note,       0, sizeof(vService_Note));
       memset(vService_Name,       0, sizeof(vService_Name));
       memset(vPrice_Code,         0, sizeof(vPrice_Code));
       memset(vPrice_Modify,       0, sizeof(vPrice_Modify));

       memset(&vPrepay_Least,      0, sizeof(vPrepay_Least));
       memset(&vDeposit_Least,     0, sizeof(vPrepay_Least));
       memset(&vMark_Least,        0, sizeof(vPrepay_Least));

       memset(vSQL_Text,           0, sizeof(vSQL_Text));
       memset(vTmp_String,         0, sizeof(vTmp_String));
       vret_code                =  0;

          /*
           *  init parameter of end
           *
           *  commented by lab 2004.11.02
           */

    /*
     *  将部分传递变量赋值到内部变量中；
     */

     strncpy(vNode_List,         node_list,      strlen(node_list));
     vNode_List[NODE_LIST] = '\0';

     strncpy(vOwner_Code,        owner_code,     strlen(owner_code));
     vOwner_Code[2] = '\0';

     strncpy(vClass_Code,        class_code,     strlen(class_code));
     vClass_Code[2] = '\0';

     strncpy(vCredit_Code,       credit_code,    strlen(credit_code));
     vCredit_Code[2] = '\0';

     vPower_Right             =  power_right;

    /*
     *  检索出SERVICE结构体当前已存储数据的行数，并返回
     *  及相关信息，然后追加到SERVICE结构体中；
     */

     max = getMaxLineOfService(service);

     if (max < 0)
     {
         pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 提取服务结构体使用行数失败");
         pubLog_Debug(_FFL_, "getServOfProduct", "", "getMaxLineOfService: vret_code＝[%d]",max);
         return -1;
     }
     else
     {
           j = max;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct:\tmax=[%d]",max);
#endif
     }

    /*
     *  将主产品传递变量赋值到内部变量中；
     */

     for (i=0;(i<MAX_ROW && strlen((product+i) -> vProduct_Code) > 0);i++)
     {

          vNum = 0;
          strcpy(vProduct_Code, "\0");

          strncpy(vProduct_Code,   (product+i) -> vProduct_Code,  strlen((product+i) -> vProduct_Code));
          vProduct_Code[8] = '\0';

          strcpy(vSQL_Text,"\0");

          strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
          sprintf(vSQL_Text,"%s SELECT Trim(Service_Code),trim(modify_flag) FROM sProductSrv ",vTmp_String);

          strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
          sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif

          strcpy(vSQL_Text,"\0");
          strcat(vSQL_Text," SELECT Trim(Service_Code),trim(modify_flag) FROM sProductSrv ");
          strcat(vSQL_Text," WHERE Product_Code = trim(:v1) ");

          EXEC SQL PREPARE Prepare_sProductSrv FROM :vSQL_Text;
          EXEC SQL DECLARE Cursor_sProductSrv CURSOR FOR Prepare_sProductSrv;
          EXEC SQL OPEN Cursor_sProductSrv using :vProduct_Code;
          for(;;)
          {
                 EXEC SQL FETCH Cursor_sProductSrv INTO :vService_Code,:vPrice_Modify;

                 if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
                 {
                             pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 读sProductSrv产品服务关系表失败!");
                       EXEC SQL CLOSE Cursor_sProductSrv;
                       return -1;
                 }

                 if (SQLCODE == NOTFOUND)
                    break;

                 /*
                  * 根据服务代码提取服务名称
                  *
                  * commented by lab 2004.11.02
                  */

                 vret_code = getServiceName(vService_Code,vService_Name);

                 if (vret_code != 0)
                 {
                           EXEC SQL CLOSE Cursor_sProductSrv;
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 提取服务名称失败，服务代码＝[%s]",vService_Code);
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServiceName: vret_code＝[%d]",vret_code);
                           return -2;
                 }

                 /*
                  * 根据服务代码提取发布信息
                  *
                  * commented by lab 2004.11.02
                  */

                 vret_code = getServReleaseInfo(vService_Code,node,vNode_List,vOwner_Code,vClass_Code,
                                                vCredit_Code,vPower_Right,
                                                &vPrepay_Least,&vDeposit_Least,
                                                &vMark_Least,vPrice_Code,vService_Note);

                 if (vret_code != 0)
                 {
                           EXEC SQL CLOSE Cursor_sProductSrv;
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 提取服务发布属性失败，服务代码＝[%s]",vService_Code);
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServReleaseInfo: vret_code＝[%d]",vret_code);
                           return -3;
                 }

                 /*
                  * 根据产品代码提取价格代码
                  *
                  * commented by lab 2004.11.02
                  */

                 vret_code = getProductPrice((product+i) -> vProduct_Code,vService_Code,vPrice_Code);

                 if (vret_code != 0 && vret_code != 1)
                 {
                           EXEC SQL CLOSE Cursor_sProductSrv;
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 提取产品服务实际价格代码失败，产品代码＝[%s],服务代码=[%s]",(product+i) -> vProduct_Code,vService_Code);
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getProductPrice: vret_code＝[%d]",vret_code);
                           return -4;
                 }

                 /*
                  * 检查该服务属性是否存在必须调整性
                  *
                  * commented by lab 2004.11.02
                  */

                 vret_code = chkSrvAttrOfNeed(vService_Code);

                 if (vret_code != 0)  strncpy((service+j) -> vServ_Modify,  "Y",    1);
                 else                 strncpy((service+j) -> vServ_Modify,  "N",    1);

                 strncpy((service+j) -> vOld_Sel,        (product+i) -> vOld_Sel,      strlen((product+i) -> vOld_Sel));
                 strncpy((service+j) -> vNew_Sel,        (product+i) -> vNew_Sel,      strlen((product+i) -> vNew_Sel));
                 strncpy((service+j) -> vProduct_Code,   (product+i) -> vProduct_Code, strlen((product+i) -> vProduct_Code));
                 strncpy((service+j) -> vProduct_Name,   (product+i) -> vProduct_Name, strlen((product+i) -> vProduct_Name));
                 strncpy((service+j) -> vService_Code,   vService_Code,                strlen(vService_Code));
                 strncpy((service+j) -> vService_Name,   vService_Name,                strlen(vService_Name));
                 strncpy((service+j) -> vPrice_Code,     vPrice_Code,                  strlen(vPrice_Code));
                 strncpy((service+j) -> vPrice_Modify,   vPrice_Modify,                strlen(vPrice_Modify));
                 (service+j) -> vPrepay_Least          = vPrepay_Least;
                 (service+j) -> vDeposit_Least         = vDeposit_Least;
                 (service+j) -> vMark_Least            = vMark_Least;
                 strncpy((service+j) -> vService_Note,   vService_Note,                strlen(vService_Note));

                 j++;

                 if (j > MAX_ROW)
                 {
                           EXEC SQL CLOSE Cursor_sProductSrv;
                           pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 服务结构体赋值overflow，MAX_ROW=[%d]",MAX_ROW);
                           return -4;
                 }

                 vNum++;
          }
          EXEC SQL CLOSE Cursor_sProductSrv;

          if (vNum <= 0)
          {
              pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: 产品[%s]在sProductSrv中没有服务配置!",vProduct_Code);
              return 2;
          }

     }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getServOfProduct", "", "getServOfProduct: End Execute");
#endif
     return 0;
}

/**
 * 得到产品因子串
 */
int _getProductMeans(const char *productPrices,TProductPrices *tProductPrices)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getProductMeans", "", "_getProductMeans: Start Execute");
#endif
	const char *p, *q;
	int i;
	char meanRecord[127+1];

	i=0;
	p=q=productPrices;
	do
	{
		if ( (q=strchr(p, '|')) != (char*)NULL)
		{
			if ( p != q)
			{
				strncpy(meanRecord, p, q - p);
				meanRecord[q-p] = '\0';
				if ( strToFields( meanRecord, ',', tProductPrices[i].serviceCode,tProductPrices[i].priceCode,tProductPrices[i].priceMean,tProductPrices[i].meanOrder,tProductPrices[i].priceType,tProductPrices[i].priceValue) != 6 )
				{
					return -1;
				}
				Trim(tProductPrices[i].serviceCode);
				Trim(tProductPrices[i].priceCode);
				Trim(tProductPrices[i].priceMean);
				Trim(tProductPrices[i].meanOrder);
				Trim(tProductPrices[i].priceType);
				Trim(tProductPrices[i].priceValue);
				i++;
			}
		}
		p = q + 1;
	}
	while(q != NULL);
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getProductMeans", "", "_getProductMeans: End Execute");
#endif
	return i;
}

/**
 * 得到服务属性串
 */
int _getServiceAttr(const char *serviceAttr,TServiceAttr *tServiceAttr)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getServiceAttr", "", "_getServiceAttr: Start Execute");
#endif
	const char *p, *q;
	int i;
	char serviceAttrRecord[127+1];

	i=0;
	p=q=serviceAttr;
	do
	{
		if ( (q=strchr(p, '|')) != (char*)NULL)
		{
			if ( p != q)
			{
				strncpy(serviceAttrRecord, p, q - p);
				serviceAttrRecord[q-p] = '\0';
				if ( strToFields( serviceAttrRecord, ',', tServiceAttr[i].serviceCode,tServiceAttr[i].attrType,tServiceAttr[i].serviceAttr,tServiceAttr[i].attrValue) != 4 )
				{
					pubLog_Debug(_FFL_, "_getServiceAttr", "", "error");
					return -1;
				}
				Trim(tServiceAttr[i].serviceCode);
				Trim(tServiceAttr[i].attrType);
				Trim(tServiceAttr[i].serviceAttr);
				Trim(tServiceAttr[i].attrValue);
				i++;
			}
		}
		p = q + 1;
	}
	while(q != NULL);
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getServiceAttr", "", "_getServiceAttr: End Execute");
#endif
	return i;
}

/**
 * 得到用户业务品牌
 */
int _getProdSmCode(const char *idNo, const char* opTime, const char *srvMsgTableName,
	char *smCode)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getProdSmCode", "", "_getProdSmCode: Start Execute");
#endif
	EXEC SQL BEGIN DECLARE SECTION;
	     int  prod_num=0;
	EXEC SQL END DECLARE SECTION;
	char dynStmt[1024],mSmCode[2+1];

	memset(mSmCode, 0, sizeof(mSmCode));
	memset(dynStmt, 0, sizeof(dynStmt));
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "_getProdSmCode", "", "idNo=[%s], opTime=[%s],srvMsgTableName=[%s]", idNo, opTime, srvMsgTableName);
#endif
	if (strncmp(srvMsgTableName, "dGrp", 4) == 0)
	{
		EXEC SQL	SELECT sm_code
					  INTO :mSmCode
					  FROM dGrpUserMsg
					 WHERE id_no = to_number(:idNo);
	}
	else
	{
		EXEC SQL	SELECT sm_code
					  INTO :mSmCode
					  FROM dCustMsg
					 WHERE id_no = to_number(:idNo);
	}
	if(SQLCODE!=0 || mSmCode[0] == '\0')
	{
		pubLog_Debug(_FFL_, "_getProdSmCode", "", "20041208LrdynStmt=[%s]SQLCODE=[%d]",dynStmt,SQLCODE);
		return 1000;
	}
	strncpy(smCode, mSmCode,2);
	smCode[2]='\0';
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_getProdSmCode", "", "_getProdSmCode: End Execute");
#endif
	return 0;
}

/**
 * 生成产品有效期
 */
int _putProdExpire(const char *regionCode, const char *productCode, const char *idNo, const char *phoneNo,
		const char *effectTime, const char *opCode,	 const char *loginNo,const char *loginAccept,
		const char *opTime,	 const char *totalDate, const char *orgCode,	 const char *orgId, char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_putProdExpire", "", "_putProdExpire: Start Execute");
#endif
	/*用于用户有效期定义*/
	char timeCode[2+1];
	int days;
	tUserBase   userBase;
	float	   prepayFee=0;

	days=0;
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
	TdCustExpire tdCustExpire;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	char sExpireTime[17+1];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	memset(&tdCustExpire,0,sizeof(tdCustExpire));
	init(v_parameter_array);
	init(sTempSqlStr);
	init(sExpireTime);
	/*ng解耦 by yaoxc end*/
	
	memset(timeCode, 0, sizeof(timeCode));
	/*只有在表sPayTime存在的产品，才存在有效期的概念*/
	EXEC SQL	SELECT days,time_code
				  INTO :days, :timeCode
				  FROM sPayTime
				 WHERE region_code = :regionCode
				   AND bill_code = :productCode
				   AND open_flag = '0';
	if(SQLCODE == 0 && SQLROWS == 1 )
	{
		if((strncmp(opCode,"11",2)!=0) && strncmp(opCode,"8",1)!=0)
		{
			Sinitn((void*)(&userBase));
			/*调用公用函数*/
			if ( fGetUserBaseInfo(phoneNo,&userBase) != 0)
			{
				strcpy(retMsg,"查询用户信息错误!");
				return 1100;
			}
			prepayFee=userBase.user_owe.totalPrepay-userBase.user_owe.accountOwe;
			pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:prepayFee=[%f]",prepayFee);
			days=0;
			EXEC SQL	SELECT days
						  INTO :days
						  FROM sPayTime
						 WHERE region_code=:regionCode
						   AND bill_code=:productCode
						   AND open_flag='1'
						   AND begin_money<=:prepayFee
						   AND end_money>=:prepayFee;
			if(SQLCODE!=OK)
			{
				pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:sPayTime.regionCode=[%s][%d][%s]",
					regionCode,SQLCODE,SQLERRMSG);
				strcpy(retMsg,"用户预存款不足,不能申请预付费");
				return 1101;
			}
		}
		/*ng解耦
		EXEC SQL	INSERT INTO dCustExpire
					(
						ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME
					)
					VALUES
					(
						to_number(:userBase.id_no),to_date(:effectTime,'yyyymmdd hh24:mi:ss'),'N',
						to_date(:effectTime,'yyyymmdd hh24:mi:ss'),
						to_date(:effectTime,'yyyymmdd hh24:mi:ss'),
						to_date(:effectTime,'yyyymmdd hh24:mi:ss')+:days
					);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:dCustExpire.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户有效期dCustExpire错误!");
			return 1102;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	EXEC SQL SELECT to_char(to_date(:effectTime,'yyyymmdd hh24:mi:ss')+:days,'yyyymmdd hh24:mi:ss') into :sExpireTime  from dual;

    	strcpy(tdCustExpire.sIdNo    	, userBase.id_no);
		strcpy(tdCustExpire.sOpenTime 	, effectTime);
		strcpy(tdCustExpire.sBeginFlag 	, "N");
		strcpy(tdCustExpire.sBeginTime 	, effectTime);
		strcpy(tdCustExpire.sOldExpire 	, effectTime);
		strcpy(tdCustExpire.sExpireTime , sExpireTime);
		strcpy(tdCustExpire.sBakField	, "");
		v_ret=OrderInsertCustExpire(ORDERIDTYPE_USER,userBase.id_no,100,
								opCode,atol(loginAccept),loginNo,"_putProdExpire",
							  	tdCustExpire);
		printf("OrderInsertCustExpire ,ret=[%d]\n",v_ret); 
		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:dCustExpire.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户有效期dCustExpire错误!");
			return 1102;
		}   	
    	/*ng解耦 by yaoxc end*/	
		EXEC SQL	INSERT INTO dCustExpireHis
					(
						ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,
						UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE
					)
					VALUES
					(
						to_number(:userBase.id_no),to_date(:effectTime,'yyyymmdd hh24:mi:ss'),'N',
						to_date(:effectTime,'yyyymmdd hh24:mi:ss'),
						to_date(:effectTime,'yyyymmdd hh24:mi:ss'),
						to_date(:effectTime,'yyyymmdd hh24:mi:ss')+:days,
						to_number(:loginAccept),to_date(:opTime,'yyyymmdd hh24:mi:ss'), :loginNo, 'a',
						:opCode, to_number(:totalDate)
					);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:dCustExpireHis.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户有效期dCustExpireHis错误!");
			return 1103;
		}


		EXEC SQL	INSERT INTO wExpireList
					(
						ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,
						ORG_CODE,OP_TIME,OP_NOTE, ORG_ID
					)
					values
					(
						to_number(:idNo),to_number(:totalDate),to_number(:loginAccept),:opCode,
						:timeCode,:days,:loginNo,:orgCode,to_date(:opTime,'yyyymmdd hh24:mi:ss'),
						'申请产品'||:productCode||'初始化有效期信息', :orgId
					);
/***********组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009****
		EXEC SQL	INSERT INTO wExpireList
					(
						ID_NO,TOTAL_DATE,LOGIN_ACCEPT,OP_CODE,TIME_CODE,DAYS,LOGIN_NO,
						ORG_CODE,OP_TIME,OP_NOTE
					)
					values
					(
						to_number(:idNo),to_number(:totalDate),to_number(:loginAccept),:opCode,
						:timeCode,:days,:loginNo,:orgCode,to_date(:opTime,'yyyymmdd hh24:mi:ss'),
						'申请产品'||:productCode||'初始化有效期信息'
					);
***************/
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_putProdExpire", "", "beginProduct:dCustExpireHis.timeCode=[%s][%d][%d][%s]",
				timeCode,days,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户有效期wExpireList错误!");
			return 1104;
		}
	}
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_putProdExpire", "", "_putProdExpire: End Execute");
#endif
	return 0;
}

/**
 * 生成产品有效期
 */
int _delProdExpire(const char *regionCode, const char *oldProduct, const char *idNo,
		const char *opCode,	 const char *loginNo,const char *loginAccept,
		const char *opTime,	 const char *totalDate, char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_delProdExpire", "", "_delProdExpire: Start Execute");
#endif
	int i;
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	init(sTempSqlStr);
	/*ng解耦 by yaoxc end*/
	
	EXEC SQL	SELECT days
				  INTO :i
				  FROM sPayTime
				 WHERE region_code = :regionCode
				   AND bill_code = :oldProduct
				   AND open_flag = '0';
	if(SQLCODE == 0 && SQLROWS == 1 )
	{
		EXEC SQL	INSERT INTO dCustExpireHis
					(
						ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
						UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE
					)
					SELECT
						ID_NO,OPEN_TIME,BEGIN_FLAG,BEGIN_TIME,OLD_EXPIRE,EXPIRE_TIME,bak_field,
						to_number(:loginAccept),to_date(:opTime,'yyyymmdd hh24:mi:ss'), :loginNo, 'd',
						:opCode, to_number(:totalDate)
					  FROM dCustExpire
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_delProdExpire", "", "_delProdExpire:dCustExpireHis.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户有效期dCustExpireHis错误!");
			return 1200;
		}
		/*ng解耦
		EXEC SQL	DELETE FROM dCustExpire
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0 && SQLCODE != NOTFOUND)
		{
			pubLog_Debug(_FFL_, "_delProdExpire", "", "endProduct:dCustExpire.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"删除用户有效期dCustExpire错误!");
			return 1201;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	init(v_parameter_array);
			
		strcpy(v_parameter_array[0],idNo);
    	
		v_ret=OrderDeleteCustExpire(ORDERIDTYPE_USER,idNo,100,
							  	opCode,atol(loginAccept),loginNo,"_delProdExpire",
								idNo,"",v_parameter_array);
		printf("OrderDeleteCustExpire ,ret=[%d]\n",v_ret);
		if(0 > v_ret)
		{
			pubLog_Debug(_FFL_, "_delProdExpire", "", "endProduct:dCustExpire.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"删除用户有效期dCustExpire错误!");
			return 1201;
		}
    	/*ng解耦 by yaoxc end*/
	}
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_delProdExpire", "", "_delProdExpire: End Execute");
#endif
	return 0;
}

/**
 * 插入产品服务信息
 *
 *@inparam	  isProductService 1:是产品服务 0:是品牌服务
 */
int _putProdService(const char *phoneNo, const char *idNo, const char *belongCode, const char *smCode,
		const char *portCode, const char *simNo, const char *imsiNo,
		const char *regionCode, const char *productCode, const char *serviceType,
		TServiceAttr *tServiceAttr, int serviceAttrCount,
		TProductPrices *tProductPrices, int productPricesCount, int	 isProductService,
		const char *serviceCode, const char *mainFlag,  const char *timeFlag,   int favOrder,
		const char *productLevel, const char *modeTime, const char *favBrand,
		int timeCycle, int timeUnit,
		const char *srvMsgTableName,const char *srvMsgAddTableName,	 const char *priceTableName,
		const char *effectTime, const char *unValidTime, const char *loginNo,
		const char *loginAccept, const char *opTime, const char *opCode, const char *totalDate,
		const char *groupId, const char *orgCode, const char *orgId, char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_putProdService", "", "_putProdService: Start Execute");
#endif
	char dynStmt[1024];
	/*BOSS优惠集团部分处理*/
	char vpmnGroup[10+1];
	double mocRate, mtcRate;

	char prodGrpId[10+1];
	char flagCode[20+1];
	char beginTime[17+1], endTime[17+1];
	char actualPrice[4+1], personFlag[1+1];
	char dataNo[15+1];
	int i, hasBegin=0, hasEnd=0;
	char vEffectTime[20+1];
	char commandCode[2+1];
	char yearMonth[6+1];
	char rateCode[4+1];
	int  vCount=0;
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
	TdBaseFav tdBaseFav;
	TdBaseVpmn tdBaseVpmn;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sRegionCode[2+1];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	memset(&tdBaseFav,0,sizeof(tdBaseFav));
	memset(&tdBaseVpmn,0,sizeof(tdBaseVpmn));
	init(v_parameter_array);
	init(sRegionCode);
	init(rateCode);
	/*ng解耦 by yaoxc end*/

	memset(yearMonth, 0, sizeof(yearMonth));
	strncpy(yearMonth, totalDate, 6);
/*#*/
	pubLog_Debug(_FFL_, "_putProdService", "", "---effectTime=[%s]",effectTime);
	if (serviceType[0] != '0' && serviceType[0] != '1' && serviceType[0] != '2' &&
		serviceType[0] != '3' && serviceType[0] != '4' && serviceType[0] != '5' && serviceType[0] != '6')
	{
		pubLog_Debug(_FFL_, "_putProdService", "", "_putProdService serviceType=[%s][%d][%s]", serviceType, SQLCODE,SQLERRMSG);
		sprintf(retMsg,"服务类型有错[%s]!", serviceType);
		return 1301;
	}

	/*****************************************************************************
	 ** time_flag 0绝对时间/1相对结束或相对自然月结束/2相对开始 **
	 ** time_unit 0天/1月/2年				   **
	 ****************************************************************************/

	memset(beginTime, 0, sizeof(beginTime));
	memset(endTime, 0, sizeof(endTime));
	memset(vEffectTime, 0, sizeof(vEffectTime));
	strcpy(vEffectTime,effectTime);

	switch(timeFlag[0])
	{
		case '0':
		{
			strncpy(beginTime,vEffectTime, 17);
			strcpy(endTime,MAXENDTIME);
			pubLog_Debug(_FFL_, "_putProdService", "", "0---beginTime=[%s]",beginTime);
			break;
		}
		case '1':
		{
			strncpy(beginTime,vEffectTime,17);
			pubCompYMD(beginTime,timeCycle,timeUnit,endTime);
			pubLog_Debug(_FFL_, "_putProdService", "", "1---beginTime=[%s]",beginTime);
			break;
		}
		case '2':
		{
		pubCompYMD(vEffectTime,timeCycle,timeUnit,beginTime);
		strcpy(endTime,MAXENDTIME);
		pubLog_Debug(_FFL_, "_putProdService", "", "2---beginTime=[%s]",beginTime);
			break;
		}
		default:
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:timeFlag[%s][%d][%s]", timeFlag, SQLCODE,SQLERRMSG);
			strcpy(retMsg,"传入的时间标志错误!");
			return 1302;
		}
	}
	Trim(beginTime);
	Trim(endTime);
/*#*/
pubLog_Debug(_FFL_, "_putProdService", "", "---beginTime=[%s]",beginTime);
	for(i=0; i < serviceAttrCount; i ++)
	{
		if (strncmp(tServiceAttr[i].serviceCode, serviceCode, 4) == 0 &&
			strncmp(tServiceAttr[i].serviceAttr, SRV_ATTR_BEGIN_TIME, 2) == 0)
		{
			strncpy(beginTime, tServiceAttr[i].attrValue,17);
			beginTime[17] = '\0';
			hasBegin = 1;
		}
		else if (	   strncmp(tServiceAttr[i].serviceCode, serviceCode, 4) == 0 &&
					strncmp(tServiceAttr[i].serviceAttr, SRV_ATTR_END_TIME, 2) == 0)
		{
			strncpy(endTime, tServiceAttr[i].attrValue,17);
			endTime[17] = '\0';
			hasEnd = 1;
		}
	}

	/*二次批价*/
	if (serviceType[0] == '0')
	{
		/* 吉林集团产品计费优惠 20070901 */
#if PROVINCE_RUN == PROVINCE_JILIN
   /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
     /*ng解耦
    EXEC SQL	INSERT INTO dBaseFav
    (
      msisdn,region_code,fav_brand,fav_type,flag_code,fav_order, fav_day,
      start_time,end_time,service_id,id_no,group_id
    )
    values
    (
      :phoneNo,substr(:belongCode,1,2),:smCode,:serviceType,:rateCode,:flagCode,'0',
      to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),:phoneNo,to_number(:idNo),:groupId
    );
	 ng解耦*/
	/*ng解耦 by yaoxc begin*/
	EXEC SQL select substr(:belongCode,1,2) into :sRegionCode from dual;
	/*printf("**********sMsisdn*****=[%s]\n",phoneNo);
	printf("**********sRegionCode*****=[%s]\n",sRegionCode);
	printf("**********sFavBrand*****=[%s]\n",smCode);
	printf("**********sFavType*****=[%s]\n",serviceType);
	printf("**********sFlagCode*****=[%s]\n",rateCode);
	printf("**********sFavOrder*****=[%s]\n",flagCode);
	printf("**********sStartTime*****=[%s]\n",beginTime);
	printf("**********sEndTime*****=[%s]\n",endTime);
	printf("**********sServiceId*****=[%s]\n",phoneNo);
	printf("**********sIdNo*****=[%s]\n",idNo);
	printf("**********sGroupId*****=[%s]\n",groupId);*/	
	strcpy(tdBaseFav.sMsisdn    	, phoneNo);
	strcpy(tdBaseFav.sRegionCode	, sRegionCode);
	strcpy(tdBaseFav.sFavBrand 		, smCode);
	strcpy(tdBaseFav.sFavType  		, serviceType);
	strcpy(tdBaseFav.sFlagCode 		, rateCode);
	strcpy(tdBaseFav.sFavOrder 		, flagCode);
	strcpy(tdBaseFav.sFavDay	   	, "0");
	strcpy(tdBaseFav.sStartTime   	, beginTime);
	strcpy(tdBaseFav.sEndTime   	, endTime);
	strcpy(tdBaseFav.sServiceId 	, phoneNo);
	strcpy(tdBaseFav.sFavPeriod   	, "");
	strcpy(tdBaseFav.sFreeValue 	, ""); 
	strcpy(tdBaseFav.sIdNo 			, idNo);
	strcpy(tdBaseFav.sGroupId   	, groupId);
	strcpy(tdBaseFav.sProductCode 	, "");   
	
	v_ret=OrderInsertBaseFav(ORDERIDTYPE_USER,idNo,100,
							opCode,atol(loginAccept),loginNo,"_putProdService",
						   	tdBaseFav);
	printf("OrderInsertBaseFav1 ,ret=[%d]\n",v_ret);
    /*ng解耦 by yaoxc end*/	
    /* 和计费确认一下,这里"aa00"写死会不会有影响? */
#ifdef _CHGTABLE_
	EXEC SQL	INSERT INTO wBaseFavChg
		(
			msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
			start_time,end_time,flag,deal_time,service_id,id_no,group_id
		)
		values
		(
			:phoneNo,:regionCode,:smCode,:serviceType,'aa00',:flagCode,
			to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
			'1', to_date(:opTime,'yyyymmdd hh24:mi:ss'),:phoneNo,to_number(:idNo),:groupId
		);
#endif
#endif

		init(vpmnGroup);
		EXEC SQL	SELECT group_index,mocrate,mtcrate
					  INTO :vpmnGroup,:mocRate,:mtcRate
					  FROM sBillVpmnCond
					 WHERE region_code=:regionCode
					   AND mode_code=:productCode;
		if(SQLROWS==1)
		{
			Trim(vpmnGroup);
#if PROVINCE_RUN == PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO dBaseFav
			(
				msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_period, free_value,
				begin_date,end_date,id_no,group_id,product_id
			)
			values
			(
				:phoneNo,:smCode,:serviceType,'aa00',:vpmnGroup,:favOrder,'1',0,
				to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
				to_number(:idNo),'NULL', :productCode
			);
#elif PROVINCE_RUN == PROVINCE_JILIN
#elif PROVINCE_RUN == PROVINCE_SHANXI
      EXEC SQL	INSERT INTO dBaseFav
      (
      	msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_peroid, free_value,
        begin_time,end_time,id_no,group_id,product_code
      )
      values
      (
        :phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'0',0,
        to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
        to_number(:idNo),'000000000000000', :productCode
      );
#else
			EXEC SQL	INSERT INTO dBaseFav
			(
				msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_period, free_value,
				begin_date,end_date,id_no,group_id,product_id
			)
			values
			(
				:phoneNo,:smCode,:serviceType,'aa00',:vpmnGroup,:favOrder,'1',0,
				to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
				to_number(:idNo),'NULL', :productCode
			);
#endif

			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dBaseFav[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录计费表dBaseFav错误!");
					return 1303;
			}

#ifdef _CHGTABLE_
#if PROVINCE_RUN == PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO wBaseFavChg
						(
							msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
							begin_date,end_date,id_no,group_id,product_id,flag,deal_date
						)
						values
						(
							:phoneNo,:smCode,:serviceType,'aa00',:vpmnGroup,:favOrder,'1',0,
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
							to_number(:idNo),'NULL', :productCode, '1', to_date(:opTime,'yyyymmdd hh24:mi:ss')
						);
#elif PROVINCE_RUN == PROVINCE_JILIN
#else
			EXEC SQL	INSERT INTO wBaseFavChg
						(
							msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
							begin_date,end_date,id_no,group_id,product_id,flag,deal_date
						)
						values
						(
							:phoneNo,:smCode,:serviceType,'aa00',:vpmnGroup,:favOrder,'1',0,
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
							to_number(:idNo),'NULL', :productCode, '1', to_date(:opTime,'yyyymmdd hh24:mi:ss')
						);
#endif
			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wBaseFavChg[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录计费表wBaseFavChg错误!");
					return 1304;
			}
#endif
/*ng解耦 
			EXEC SQL	INSERT INTO dBaseVpmn
						(
							msisdn,groupid,mocrate,mtcrate,valid,invalid
						)
						values
						(
							:phoneNo,:vpmnGroup,:mocRate*1000,:mtcRate*1000,
							to_date(:beginTime,'yyyymmdd hh24:mi:ss'),to_date(:endTime,'YYYYMMDD HH24:MI:SS')
						);
			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dBaseVpmn[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录计费表wBaseFavChg错误!");
					return 1305;
			}
ng解耦*/
    	/*ng解耦 by yaoxc begin*/    		    	
    	strcpy(tdBaseVpmn.sMsisdn   , phoneNo);
		strcpy(tdBaseVpmn.sGroupId 	, vpmnGroup);
		strcpy(tdBaseVpmn.sValid 	, beginTime);
		strcpy(tdBaseVpmn.sInvalid 	, endTime);
		sprintf(tdBaseVpmn.sMocrate,"%f", mocRate*1000);
		sprintf(tdBaseVpmn.sMtcrate,"%f", mocRate*1000);
			
		v_ret=OrderInsertBaseVpmn(ORDERIDTYPE_USER,idNo,100,
								opCode,atol(loginAccept),loginNo,"_putProdExpire",
							  	tdBaseVpmn);
		printf("OrderInsertBaseVpmn ,ret=[%d]\n",v_ret); 
		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dBaseVpmn[%d][%s]",
						SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录计费表dBaseVpmn错误!");
			return 1305;
		}   	
    	/*ng解耦 by yaoxc end*/	
#ifdef _CHGTABLE_
		EXEC SQL	INSERT INTO wBaseVpmnChg
						(
							msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time
						)
						values
						(
							:phoneNo,:vpmnGroup,:mocRate*1000,:mtcRate*1000,
							to_date(:beginTime,'yyyymmdd hh24:mi:ss'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
							'1',to_date(:opTime,'yyyymmdd hh24:mi:ss')
						);
			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wBaseVpmnChg[%d][%s]",
						SQLCODE,SQLERRMSG);
					sprintf(retMsg,"处理wBaseVpmnChg默认集团%s时错误!", vpmnGroup);
					return 1306;
			}
#endif
		}

		memset(flagCode, 0, sizeof(flagCode));
		strcpy(flagCode, serviceCode);
		for (i=0; i < serviceAttrCount; i ++)
		{
			if (strncmp(tServiceAttr[i].serviceCode, serviceCode, 4)== 0)
			{
				strcpy(flagCode, tServiceAttr[i].attrValue);
				/*认为一个二次批价服务代码，只能有一个二次批价服务属性值*/
				break;
			}
		}

		memset(rateCode, 0, sizeof(rateCode));
		EXEC SQL	SELECT	rate_code
					  INTO :rateCode
					  FROM sSrvRateCode
					 WHERE service_code = :serviceCode;
		if (SQLCODE !=0)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:sSrvRateCode[%d][%s][%s]",
				SQLCODE,SQLERRMSG,serviceCode);
			strcpy(retMsg,"查询sSrvRateCode错误!");
			return 1307;
		}


#if PROVINCE_RUN == PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO dBaseFav
					(
						msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_period, free_value,
						begin_date,end_date,id_no,group_id,product_id
					)
					values
					(
						:phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'1',0,
						to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:idNo),'NULL', :productCode
					);
#elif PROVINCE_RUN == PROVINCE_SHANXI
                        EXEC SQL        INSERT INTO dBaseFav
                                        (
                                                msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_peroid, free_value,
                                                begin_time,end_time,id_no,group_id,product_code
                                        )
                                        values
                                        (
                                                :phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'0',0,
                                                to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
                                                to_number(:idNo),'000000000000000', :productCode
                                        );
#elif PROVINCE_RUN == PROVINCE_JILIN

/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
               /*ng解耦 EXEC SQL        INSERT INTO dBaseFav
                                        (
                                                msisdn,region_code,fav_brand,fav_type,flag_code, fav_order, fav_day,
                                                start_time,end_time,service_id,id_no,group_id,product_code
                                        )
                                        values
                                        (
                                                :phoneNo,substr(:belongCode,1,2),:smCode,:serviceType,:rateCode,:flagCode,'0',
                                                to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),:phoneNo,to_number(:idNo),:groupId,:productCode
                                        );
                                        ng解耦*/
						/*ng解耦 by yaoxc begin*/
						EXEC SQL select substr(:belongCode,1,2) into :sRegionCode from dual;
		
						strcpy(tdBaseFav.sMsisdn    	, phoneNo);
						strcpy(tdBaseFav.sRegionCode	, sRegionCode);
						strcpy(tdBaseFav.sFavBrand 		, smCode);
						strcpy(tdBaseFav.sFavType  		, serviceType);
						strcpy(tdBaseFav.sFlagCode 		, rateCode);
						strcpy(tdBaseFav.sFavOrder 		, flagCode);
						strcpy(tdBaseFav.sFavDay	   	, "0");
						strcpy(tdBaseFav.sStartTime   	, beginTime);
						strcpy(tdBaseFav.sEndTime   	, endTime);
						strcpy(tdBaseFav.sServiceId 	, phoneNo);
						strcpy(tdBaseFav.sFavPeriod   	, "");
						strcpy(tdBaseFav.sFreeValue 	, ""); 
						strcpy(tdBaseFav.sIdNo 			, idNo);
						strcpy(tdBaseFav.sGroupId   	, groupId);
						strcpy(tdBaseFav.sProductCode 	, productCode);   
	    	
						v_ret=OrderInsertBaseFav(ORDERIDTYPE_USER,idNo,100,
											opCode,atol(loginAccept),loginNo,"_putProdService",
											tdBaseFav);
						printf("OrderInsertBaseFav ,ret=[%d]\n",v_ret);
   						/*ng解耦 by yaoxc end*/	
	    									
#else       
			EXEC SQL	INSERT INTO dBaseFav
					(
						msisdn,fav_brand,fav_big_type,fav_type,flag_code, fav_order,fav_period, free_value,
						begin_date,end_date,id_no,group_id,product_id
					)       
					values      
					(
						:phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'1',0,
						to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:idNo),'NULL', :productCode
					);
#endif

		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dBaseFav[%d][%s][%s][%s][%s][%s]",
				SQLCODE,SQLERRMSG,phoneNo,serviceCode,beginTime,productCode);
			strcpy(retMsg,"记录计费表dBaseFav错误!");
			return 1307;
		}

#ifdef _CHGTABLE_
#if PROVINCE_RUN == PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO wBaseFavChg
					(
						msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
						begin_date,end_date,id_no,group_id,product_id,flag,deal_date
					)
					values
					(
						:phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'1',0,
						to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:idNo),'NULL', :productCode, '1', to_date(:opTime,'yyyymmdd hh24:mi:ss')
					);
#elif PROVINCE_RUN == PROVINCE_JILIN

/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL	INSERT INTO wBaseFavChg
						(
							msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
							start_time,end_time,flag,deal_time,service_id,id_no,group_id,product_code
						)
						values
						(
							:phoneNo,:regionCode,:smCode,:serviceType,'aa00',:flagCode,
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
							'1', to_date(:opTime,'yyyymmdd hh24:mi:ss'),:phoneNo,to_number(:idNo),:groupId,:productCode
						);
#else
			EXEC SQL	INSERT INTO wBaseFavChg
					(
						msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
						begin_date,end_date,id_no,group_id,product_id,flag,deal_date
					)
					values
					(
						:phoneNo,:smCode,:serviceType,:rateCode,:flagCode,:favOrder,'1',0,
						to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
						to_number(:idNo),'NULL', :productCode, '1', to_date(:opTime,'yyyymmdd hh24:mi:ss')
					);
#endif

		if(SQLCODE!=0)
		{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wBaseFavChg[%d][%s]",
					SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录计费表wBaseFavChg错误!");
				return 1308;
		}
#endif
	}/*二次批价*/
	else
	{
		/*处理短号码*/
		memset(dataNo, 0, sizeof(dataNo));
		for(i=0; i < serviceAttrCount; i ++)
		{
			if (strncmp(tServiceAttr[i].serviceCode, serviceCode, 4) == 0 &&
				(
					strncmp(tServiceAttr[i].serviceAttr, SRV_ATTR_DATA_NO, 2) == 0 ||
					strncmp(tServiceAttr[i].serviceAttr, SRV_ATTR_FAX_NO, 2) == 0 ||
					strncmp(tServiceAttr[i].serviceAttr, SRV_ATTR_SEEK_NO, 2) == 0
				)
			)
			{
				strncpy(dataNo, tServiceAttr[i].attrValue,15);
				dataNo[15] = '\0';
				Trim(dataNo);
			}
		}

		if (dataNo[0] != '\0')
		{
			init(dynStmt);
			sprintf(dynStmt,	"INSERT INTO wCustResOpr%s"
								"("
								"	LOGIN_ACCEPT,TOTAL_DATE,NO_TYPE,BEGIN_NO,"
								"	END_NO,PASSWORD,OTHER_ATTR,CHOICE_FEE,"
								"	LIMIT_PREPAY,BEGIN_TIME,OLD_GROUP,OLD_LOGIN,"
								"	NEW_GROUP,NEW_LOGIN,RESOURCE_CODE,OP_TIME,"
								"	OP_CODE,LOGIN_NO,ORG_GROUP,NOTE,BAK_FIELD"
								")"
								"SELECT"
								"	to_number(:LOGIN_ACCEPT),to_number(:TOTAL_DATE),NO_TYPE,PHONE_NO,"
								"	PHONE_NO,PASSWORD,OTHER_ATTR,CHOICE_FEE,"
								"	LIMIT_PREPAY,BEGIN_TIME,GROUP_ID,LOGIN_NO,"
								"	GROUP_ID,LOGIN_NO,'1',to_char(to_date(:OP_TIME, 'YYYYMMDD HH24:MI:SS'), 'YYYYMMDDHH24MISS'),"
								"	:OP_CODE,:LOGIN_NO,:ORG_GROUP,NULL,NULL"
								"  FROM dCustRes"
								" WHERE phone_no = :phoneNo"
								,yearMonth);

			EXEC SQL PREPARE ins_stmt From :dynStmt;
			EXEC SQL EXECUTE ins_stmt using :loginAccept ,:totalDate, :opTime,
					:opCode, :loginNo, :groupId, :phoneNo;

			if (SQLCODE != 0)
			{
				sprintf(retMsg, "插入wCustResOpr%s 错误",yearMonth);
				pubLog_Debug(_FFL_, "_putProdService", "", "插入wCustResOpr%s 错误 [%d]" ,yearMonth,SQLCODE);
				pubLog_Debug(_FFL_, "_putProdService", "", "SQLERRMSG = [%s]",SQLERRMSG);
				return 1309;
			}
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL	UPDATE dCustRes
						   SET resource_code = '1',
							   login_no  = :loginNo,
							   login_accept = to_number(:loginAccept),
							   op_time	= TO_DATE(:opTime,'YYYYMMDD HH24:MI:SS'),
							   total_date   = TO_NUMBER(:totalDate),
							   org_code  = :orgCode,
							   org_id = :orgId
						 WHERE phone_no = :dataNo;
			if (SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dCustRes[%d][%s]",SQLCODE,SQLERRMSG);
				strcpy(retMsg,"更新号码资源信息错误!");
				return 1309;
			}
			/***优良号码资源同步  20090224 add***/
			vCount=0;
			EXEC SQL SELECT  count(*)
					   into  :vCount
			           from  dgoodphoneres
			          where  phone_no=:dataNo;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dgoodphoneres[%d][%s]",SQLCODE,SQLERRMSG);
				strcpy(retMsg,"查询号码资源信息错误!");
				return 1309;
			}
			if(vCount>0)   
			{    
				pubLog_Debug(_FFL_, "_putProdService", "", "插入历史表phone_no=[%s]",dataNo);   
				EXEC SQL INSERT INTO dgoodphonereshis
			                 (  phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,
								begin_time,end_time,bak_field,district_code,resource_code ,
								update_login,update_code,update_date,update_time,update_type,update_accept )
						 select phone_no,goodsm_mode,bill_code,good_type,region_code,pre_do_flag,
								begin_time,end_time,bak_field,district_code,resource_code,
								:loginNo,:opCode,to_number(:totalDate),sysdate,'u',to_number(:loginAccept)
						  from  dgoodphoneres
						 where  phone_no=:dataNo;
				if(SQLCODE!=0)
				{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dgoodphonereshis[%d][%s]",SQLCODE,SQLERRMSG);
					strcpy(retMsg,"插入优良号码资源历史表信息错误!");
					return 1309;
				} 
				EXEC SQL UPDATE  dgoodphoneres
				           SET   resource_code='1'
				         where   phone_no=:dataNo;			
				if(SQLCODE!=0)
				{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:dgoodphoneres[%d][%s]",SQLCODE,SQLERRMSG);
					strcpy(retMsg,"更新优良号码资源信息错误!");
					return 1309;
				}
			}			
			/***优良号码资源同步  20090224 EDD***/
		}

		/*一般都是增值服务*/
		/*如果是立即生效，并且存在指令的服务要求发送开关机开通指令。如：增值服务*/
		if (strncmp(beginTime, opTime, 17) <= 0)
		{
			EXEC SQL	DECLARE cur321301	CURSOR FOR
				SELECT b.command_code
				  FROM sSrvCmd a, sStatCmdCode b
				 WHERE a.service_code = :serviceCode
				   AND a.cmd_code = b.cmd_code
				 ORDER BY b.command_order;
			EXEC SQL	OPEN cur321301;
			while(1)
			{
				EXEC SQL	FETCH cur321301	INTO :commandCode;
				if (SQLCODE != 0) break;

#if PROVINCE_RUN==PROVINCE_SICHUAN
				EXEC SQL	INSERT INTO wSndCmdDay
							(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,
								command_code,new_phone,imsi_no,new_imsi,other_char,op_code,total_date,
								op_time,login_no,org_code,login_accept,
								request_time,business_status,send_status,send_time,
								group_id,org_id
							)
							VALUES
							(
								sOffOn.nextval,:portCode,0,to_number(:idNo),:belongCode,:smCode,:phoneNo,
								:commandCode, :phoneNo,:imsiNo,:imsiNo,:simNo,:opCode,to_number(:totalDate),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:loginNo,:orgCode,to_number(:loginAccept),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),'1','0',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),
								:groupId, :orgId
							);
#elif PROVINCE_RUN==PROVINCE_JILIN
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				EXEC SQL	INSERT INTO wSndCmdDay
							(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,
								command_code,new_phone,imsi_no,new_imsi,other_char,op_code,total_date,
								op_time,login_no,org_code,login_accept,
								request_time,business_status,send_status,send_time,org_id,group_id
							)
							VALUES
							(
								sOffOn.nextval,:portCode,0,to_number(:idNo),:belongCode,:smCode,:phoneNo,
								:commandCode, :phoneNo,:imsiNo,:imsiNo,:simNo,:opCode,to_number(:totalDate),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:loginNo,:orgCode,to_number(:loginAccept),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),'1','0',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:orgId,:groupId
							);
#else

				EXEC SQL	INSERT INTO wSndCmdDay
							(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,
								command_code,new_phone,imsi_no,new_imsi,other_char,op_code,total_date,
								op_time,login_no,org_code,login_accept,
								request_time,business_status,send_status,send_time,
								group_id,org_id
							)
							VALUES
							(
								sOffOn.nextval,:portCode,0,to_number(:idNo),:belongCode,:smCode,:phoneNo,
								:commandCode, :phoneNo,:imsiNo,:imsiNo,:simNo,:opCode,to_number(:totalDate),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:loginNo,:orgCode,to_number(:loginAccept),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),'1','0',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),
								:groupId, :orgId
							);
#endif
				if (SQLCODE !=0)
				{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wSndCmdDay[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录帐务表wSndCmdDay错误!");
					return 1310;
				}
			}
			EXEC SQL	CLOSE cur321301;
		}

		if (hasBegin == 1)
		{
#if PROVINCE_RUN==PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note, org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'1',
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),'特服指令开通', :orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#elif PROVINCE_RUN==PROVINCE_JILIN
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note,org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'1',
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),'特服指令开通',:orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#else
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note, org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'1',
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),'特服指令开通', :orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#endif
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wCustExpire[%d][%s]",
					SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录到期指令表wCustExpire错误!");
				return 1311;
			}
		}
		if (hasEnd == 1)
		{
#if PROVINCE_RUN==PROVINCE_SICHUAN
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note, org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'0',
							to_date(:endTime,'YYYYMMDD HH24:MI:SS'),'特服指令关闭', :orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#elif PROVINCE_RUN==PROVINCE_JILIN
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note,org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'0',
							to_date(:endTime,'YYYYMMDD HH24:MI:SS'),'特服指令关闭',:orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#else
			EXEC SQL	INSERT INTO wCustExpire
						(
							command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
							total_date,op_code,op_time,function_code,business_status,
							expire_time,op_note, org_id
						)
						SELECT
							sOffOn.nextval,:smCode,to_number(:idNo),:phoneNo,:orgCode,:loginNo,to_number(:loginAccept),
							to_number(:totalDate),:opCode,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:serviceCode,'0',
							to_date(:endTime,'YYYYMMDD HH24:MI:SS'),'特服指令关闭', :orgId
						  FROM sSrvCmd
						 WHERE service_code = :serviceCode;
#endif
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wCustExpire[%d][%s]",
					SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录到期指令表wCustExpire错误!");
				return 1311;
			}
		}
	}

	/*增值服务*/
	if (serviceType[0] == '5')
	{
#if PROVINCE_RUN==PROVINCE_SICHUAN
		EXEC SQL	INSERT INTO wCustFuncDay
					(
						ID_NO,PHONE_NO,FUNCTION_TYPE,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
						OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT,BELONG_CODE,ORG_CODE
					)
					VALUES
					(
						to_number(:idNo),:phoneNo,:serviceType,:serviceCode,'Y',:totalDate,
						to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept),
						:belongCode,:orgCode
					);
#elif PROVINCE_RUN==PROVINCE_JILIN
		 EXEC SQL        INSERT INTO wCustFuncDay
                                        (
                                                ID_NO,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
                                                OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT
                                        )
                                        VALUES
                                        (
                                                to_number(:idNo),:serviceCode,'1',:totalDate,
                                                to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept)
                                        );
#else
		EXEC SQL	INSERT INTO wCustFuncDay
					(
						ID_NO,PHONE_NO,FUNCTION_TYPE,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
						OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT,BELONG_CODE,ORG_CODE
					)
					VALUES
					(
						to_number(:idNo),:phoneNo,:serviceType,:serviceCode,'Y',:totalDate,
						to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept),
						:belongCode,:orgCode
					);
#endif
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wCustFuncDay[%d][%s]",SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录到期增加服务变更表wCustFuncDay错误!");
			return 1312;
		}
	}

	/*记录所有的附属性*/
	for (i=0; i < serviceAttrCount; i ++)
	{
		if (strncmp(tServiceAttr[i].serviceCode, serviceCode, 4)== 0)
		{
			sprintf(dynStmt,	"INSERT INTO %s%c"
								"("
								"	   ID_NO, PRODUCT_CODE,SERVICE_TYPE, SERVICE_CODE, ATTR_TYPE, SERVICE_ATTR,"
								"	   ATTR_VALUE,BEGIN_TIME,END_TIME,LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
								")"
								"VALUES"
								"("
								"	   to_number(:idNo), :productCode,:serviceType, :serviceCode, :attrType,:serviceAttr,"
								"	   :attrValue,:beginTime,:endTime,:loginAccept,:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS')"
								")",
								srvMsgAddTableName,idNo[strlen(idNo)-1]);
			EXEC SQL	PREPARE predSrvMsgAdd FROM :dynStmt;
			EXEC SQL	EXECUTE predSrvMsgAdd USING :idNo, :productCode,:serviceType,:serviceCode,:tServiceAttr[i].attrType,
							:tServiceAttr[i].serviceAttr,:tServiceAttr[i].attrValue,:beginTime,:endTime, :loginAccept,
							:loginNo,:opCode,:opTime;
			if (SQLCODE !=0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:%s[%d][%s]",
					srvMsgAddTableName,SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录用户产品服务附加信息错误!");
				return 1313;
			}
			sprintf(dynStmt,	"INSERT INTO %s%cHis"
								"("
								"	   ID_NO, PRODUCT_CODE,SERVICE_TYPE, SERVICE_CODE, ATTR_TYPE, SERVICE_ATTR,"
								"	   ATTR_VALUE,BEGIN_TIME,END_TIME,LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
								"	   UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
								")"
								"VALUES"
								"("
								"	   to_number(:idNo), :productCode,:serviceType, :serviceCode, :attrType,:serviceAttr,"
								"	   :attrValue,:beginTime,:endTime,:loginAccept,:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS'),"
								"	   to_number(:updateAccept), to_date(:updateTime, 'YYYYMMDD HH24:MI:SS'), :updateLogin, 'a',:updateCode, to_number(:updateDate)"
								")", srvMsgAddTableName,idNo[strlen(idNo)-1]);
			EXEC SQL	PREPARE predSrvMsgAddHis1 FROM :dynStmt;
			EXEC SQL	EXECUTE predSrvMsgAddHis1 USING :idNo, :productCode,:serviceType,:serviceCode,:tServiceAttr[i].attrType,
							:tServiceAttr[i].serviceAttr,:tServiceAttr[i].attrValue,:beginTime,:endTime,:loginAccept,
							:loginNo,:opCode,:opTime,:loginAccept, :opTime, :loginNo,:opCode,:totalDate;
			if (SQLCODE !=0)
			{
				pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:%sHis[%d][%s]",
					srvMsgAddTableName,SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录用户产品服务附加信息历史错误!");
				return 1314;
			}
		}
	}

	if (isProductService)
	{
		sprintf(dynStmt,	"SELECT actual_price"
					"  FROM sProductPrice"
					" WHERE product_code = :productCode"
					"   AND service_code = :serviceCode");
	}
	else
	{
		memset(prodGrpId, 0, sizeof(prodGrpId));
		getLowestGroupId(groupId, serviceCode, prodGrpId);
		if (prodGrpId[0] == '\0')
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:[%s][%d][%s]",
				serviceCode, SQLCODE,SQLERRMSG);
			strcpy(retMsg,"得到发布服务的GROUP ID错误!");
			return 1315;
		}
		Trim(prodGrpId);

		sprintf(dynStmt,	"SELECT price_code"
					"  FROM sSrvRelease"
					" WHERE service_code = :serviceCode"
					"   AND trim(group_id) = :prodGrpId");
	}
	EXEC SQL	PREPARE prePrice from :dynStmt;
	EXEC SQL	DECLARE curProductPrice CURSOR FOR prePrice;

	if (isProductService)
	{
		EXEC SQL	OPEN curProductPrice USING :productCode, :serviceCode;
	}
	else
	{
		EXEC SQL	OPEN curProductPrice USING :serviceCode, :prodGrpId;
	}


	memset(personFlag, 0, sizeof(personFlag));

	/*第二层：和服务中的价格进一步有关的表*/
	for(;;)
	{
		EXEC SQL	FETCH   curProductPrice INTO :actualPrice;
		if (SQLCODE != 0) break;

		personFlag[0] = 'N';
		for(i=0; i < productPricesCount; i ++)
		{
			if (strncmp(serviceCode, tProductPrices[i].serviceCode, 4) == 0 &&
				strncmp(actualPrice, tProductPrices[i].priceCode, 4) == 0)
			{
				personFlag[0] = 'Y';
				break;
			}
		}

#if PROVINCE_RUN == PROVINCE_SICHUAN
		sprintf(dynStmt,	"INSERT INTO %s%c"
							"("
								"ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
								"SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
								"PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG,"
								"LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
							")"
							"VALUES"
							"("
								"to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
								"0, :favOrder, :beginTime,:endTime,:mainFlag,"
								":productLevel,:modeTime, 'Y',to_date(:unValidTime, 'yyyymmdd hh24:mi:ss'),:personFlag,"
								"to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS')"
							")",
							srvMsgTableName,idNo[strlen(idNo)-1]);
		EXEC SQL	PREPARE predSrvMsg FROM :dynStmt;
		EXEC SQL	EXECUTE predSrvMsg USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
						:favOrder, :beginTime,:endTime,:mainFlag,
						:productLevel, :modeTime,:unValidTime,:personFlag,
						:loginAccept,:loginNo, :opCode,:opTime;
		pubLog_Debug(_FFL_, "_putProdService", "", "INSERT INTO %s%c (ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE, \
					SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG, \
					PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG, \
					LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME) \
					VALUES( \
					to_number('%s'),'%s','%s','%s','%s', \
					0,%d,'%s','%s', \
					'%s','%s','Y','%s','%s', \
					%s,'%s','%s','%s');", 
					srvMsgTableName,idNo[strlen(idNo)-1],
					idNo, productCode, serviceType, serviceCode, actualPrice,
					favOrder, beginTime,endTime,mainFlag,
					productLevel,modeTime,unValidTime,personFlag,
					loginAccept,loginNo,opCode,opTime);
#elif PROVINCE_RUN == PROVINCE_SHANXI
		sprintf(dynStmt, "INSERT INTO %s%c"
                                 "("
                                 "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                 "SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                 "PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG,"
                                 "LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
                                 ")"
                                 " VALUES "
                                 "("
                                 "to_number('%s'), '%s', '%s', '%s', '%s',"
                                 "0, %d, '%s','%s','%s',"
                                 "'%s','%s', 'Y',to_date('%s', 'yyyymmdd hh24:mi:ss'),'%s',"
                                 "to_number('%s'),'%s','%s',to_date('%s', 'YYYYMMDD HH24:MI:SS')"
                                 ")",
                                 srvMsgTableName,idNo[strlen(idNo)-1],
                                 idNo, productCode, serviceType,serviceCode,actualPrice,
                                                favOrder,     beginTime, endTime,    mainFlag,
                                                productLevel, modeTime,  endTime,    personFlag,
                                                loginAccept,  loginNo,   opCode,     opTime);

                pubLog_Debug(_FFL_, "_putProdService", "", "SQL_text=[%s]",dynStmt);

                memset(dynStmt, 0, sizeof(dynStmt));

                sprintf(dynStmt,        "INSERT INTO %s%c"
                                                        "("
                                                                "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                                                "SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                                                "PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG,"
                                                                "LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
                                                        ")"
                                                        " VALUES "
                                                        "("
                                                                "to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
                                                                "0, :favOrder, :beginTime,:endTime,:mainFlag,"
                                                                ":productLevel,:modeTime, 'Y',to_date(:endTime,   'yyyymmdd hh24:mi:ss'),:personFlag,"
                                                                "to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS')"
                                                        ")",
                                                        srvMsgTableName,idNo[strlen(idNo)-1]);
                EXEC SQL        PREPARE predSrvMsg FROM :dynStmt;
                EXEC SQL        EXECUTE predSrvMsg USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
                                                :favOrder, :beginTime,:endTime,:mainFlag,
                                                :productLevel, :modeTime,:endTime,:personFlag,
                                                :loginAccept,:loginNo, :opCode,:opTime;
#else
	sprintf(dynStmt, "INSERT INTO dGrpSrvMsg"
                                 "("
                                 "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                 "SRV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                 "UPTO_TIME,USE_FLAG,UNVAILD_TIME,PERSON_FLAG,"
                                 "LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
                                 ")"
                                 " VALUES "
                                 "("
                                 "to_number('%ld'), '%s', '%s', '%s', '%s',"
                                 "0, to_date('%s','yyyymmdd hh24:mi:ss'),to_date('%s','yyyymmdd hh24:mi:ss'),'%s',"
                                 "'','Y',to_date('%s', 'yyyymmdd hh24:mi:ss'),'%s',"
                                 "to_number('%s'),'%s','%s',to_date('%s', 'YYYYMMDD HH24:MI:SS')"
                                 ")",
                                 idNo, productCode, serviceType,serviceCode,actualPrice,
                                                beginTime, endTime,    mainFlag,
                                                endTime,    personFlag,
                                                loginAccept,  loginNo,   opCode,     opTime);

                pubLog_Debug(_FFL_, "_putProdService", "", "SQL_text=[%s]",dynStmt);

                memset(dynStmt, 0, sizeof(dynStmt));

                sprintf(dynStmt,        "INSERT INTO dGrpSrvMsg"
                                                        "("
                                                                "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                 								"SRV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                 								"UPTO_TIME,USE_FLAG,UNVAILD_TIME,PERSON_FLAG,"
                                 								"LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
                                                        ")"
                                                        " VALUES "
                                                        "("
                                                                "to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
                                                                "0, to_date(:beginTime,'yyyymmdd hh24:mi:ss'),to_date(:endTime,'yyyymmdd hh24:mi:ss'),:mainFlag,"
                                                                "'', 'Y',to_date(:endTime,   'yyyymmdd hh24:mi:ss'),:personFlag,"
                                                                "to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS')"
                                                        ")");
                EXEC SQL        PREPARE predSrvMsg FROM :dynStmt;
                EXEC SQL        EXECUTE predSrvMsg USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
                                                :beginTime,:endTime,:mainFlag,
                                                :endTime,:personFlag,
                                                :loginAccept,:loginNo, :opCode,:opTime;
#endif
		if (SQLCODE !=0)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:%s[%d][%s][%06d]",
				srvMsgTableName,SQLCODE,SQLERRMSG,1316);
			strcpy(retMsg,"记录用户产品服务错误!");
			EXEC SQL	CLOSE curProductPrice;
			return 1316;
		}

#if PROVINCE_RUN == PROVINCE_SICHUAN
		sprintf(dynStmt,	"INSERT INTO %s%cHis"
							"("
								"ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
								"SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
								"PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG,"
								"LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
							"	   UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
							")"
							"VALUES"
							"("
								"to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
								"0, :favOrder, :beginTime,:endTime,:mainFlag,"
								":productLevel,:modeTime, 'Y',to_date(:unValidTime, 'yyyymmdd hh24:mi:ss'),:personFlag,"
								"to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS'),"
							"	   to_number(:updateAccept), to_date(:updateTime, 'YYYYMMDD HH24:MI:SS'), :updateLogin, 'a',:updateCode, to_number(:updateDate)"
							")", srvMsgTableName,idNo[strlen(idNo)-1]);
		EXEC SQL	PREPARE predSrvMsgHis1 FROM :dynStmt;
		EXEC SQL	EXECUTE predSrvMsgHis1 USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
						:favOrder, :beginTime,:endTime,:mainFlag,
						:productLevel, :modeTime,:unValidTime,:personFlag,
						:loginAccept,:loginNo, :opCode,:opTime,
						:loginAccept, :opTime, :loginNo,:opCode,:totalDate;
#elif PROVINCE_RUN == PROVINCE_SHANXI
		sprintf(dynStmt,        "INSERT INTO %s%cHis"
                                                        "("
                                                                "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                                                "SERAIL_ID,FAV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                                                "PRODUCT_LEVEL,MODE_TIME,USE_FLAG,UNVALID_TIME,PERSON_FLAG,"
                                                                "LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
                                                        "          UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
                                                        ")"
                                                        " VALUES "
                                                        "("
                                                                "to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
                                                                "0, :favOrder, :beginTime,:endTime,:mainFlag,"
                                                                ":productLevel,:modeTime, 'Y',to_date(:endTime,    'yyyymmdd hh24:mi:ss'),:personFlag,"
                                                                "to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS'),"
                                                        "          to_number(:updateAccept), to_date(:updateTime, 'YYYYMMDD HH24:MI:SS'), :updateLogin, 'a',:updateCode, to_number(:updateDate)"
                                                        ")", srvMsgTableName,idNo[strlen(idNo)-1]);
                EXEC SQL        PREPARE predSrvMsgHis1 FROM :dynStmt;
                EXEC SQL        EXECUTE predSrvMsgHis1 USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
                                                :favOrder, :beginTime,:endTime,:mainFlag,
                                                :productLevel, :modeTime,:endTime  ,:personFlag,
                                                :loginAccept,:loginNo, :opCode,:opTime,
                                                :loginAccept, :opTime, :loginNo,:opCode,:totalDate;
#else
		sprintf(dynStmt,        "INSERT INTO dGrpSrvMsgHis"
                                                        "("
                                                                "ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
                                 								"SRV_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
                                 								"UPTO_TIME,USE_FLAG,UNVAILD_TIME,PERSON_FLAG,"
                                 								"LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
                                                        "          UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
                                                        ")"
                                                        " VALUES "
                                                        "("
                                                                "to_number(:idNo), :productCode, :serviceType, :serviceCode, :actualPrice,"
                                                                "0, to_date(:beginTime,'yyyymmdd hh24:mi:ss'),to_date(:endTime,'yyyymmdd hh24:mi:ss'),:mainFlag,"
                                                                "'', 'Y',to_date(:endTime,    'yyyymmdd hh24:mi:ss'),:personFlag,"
                                                                "to_number(:loginAccept),:loginNo,:opCode,to_date(:opTime, 'YYYYMMDD HH24:MI:SS'),"
                                                        "          to_number(:updateAccept), to_date(:updateTime, 'YYYYMMDD HH24:MI:SS'), :updateLogin, 'a',:updateCode, to_number(:updateDate)"
                                                        ")");
                EXEC SQL        PREPARE predSrvMsgHis1 FROM :dynStmt;
                EXEC SQL        EXECUTE predSrvMsgHis1 USING :idNo, :productCode, :serviceType,:serviceCode,:actualPrice,
                                                :beginTime,:endTime,:mainFlag,
                                                :endTime  ,:personFlag,
                                                :loginAccept,:loginNo, :opCode,:opTime,
                                                :loginAccept, :opTime, :loginNo,:opCode,:totalDate;
#endif
		if (SQLCODE !=0)
		{
			pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:%sHis[%d][%s]",
				srvMsgTableName,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"记录用户产品服务历史错误!");
			EXEC SQL	CLOSE curProductPrice;
			return 1317;
		}

		/*非二次批价，二次批价信息不插入此表*/
		if (serviceType[0] != '0')
		{
			/*四川暂时不插入*/
#if PROVINCE_RUN==PROVINCE_SHANXI
			EXEC SQL        INSERT INTO wSrvMsgChg
                                    (
                                            op_no,op_type,op_time,id_no,product_code, service_type,service_code,
                                            price_code,FAV_ORDER, begin_time,end_time, person_flag,op_code,login_accept,phone_no
                                    )
                                    values
                                    (
                                            sMaxBillChg.nextval,'1',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),to_number(:idNo),
                                            :productCode, :serviceType,:serviceCode,:actualPrice, :favOrder,
                                            to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
                                            :personFlag,:opCode,to_number(trim(:loginAccept)),:phoneNo
                                    );
            if(SQLCODE!=0)
            {
                            pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wSrvMsgChg[%d][%s]",
                                    SQLCODE,SQLERRMSG);
                            strcpy(retMsg,"记录帐务表wSrvMsgChg错误!");
                            EXEC SQL        CLOSE curProductPrice;
                            return 1318;
            }
#endif
/*
			EXEC SQL	INSERT INTO wSrvMsgChg
						(
							op_no,op_type,op_time,id_no,product_code, service_type,service_code,
							price_code,FAV_ORDER, begin_time,end_time, person_flag
						)
						values
						(
							sMaxBillChg.nextval,'1',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),to_number(:idNo),
							:productCode, :serviceType,:serviceCode,:actualPrice, :favOrder,
							to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS'),
							:personFlag
						);
			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wSrvMsgChg[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录帐务表wSrvMsgChg错误!");
					EXEC SQL	CLOSE curProductPrice;
					return 1318;
			}
*/
		}/*非二次批价*/

		/*第三层：和服务中的价格的价格因子更进一步有关的表*/
		if (personFlag[0] == 'Y')
		{
			for(i=0; i < productPricesCount; i ++)
			{
				if (strncmp(serviceCode, tProductPrices[i].serviceCode, 4) == 0 &&
					strncmp(actualPrice, tProductPrices[i].priceCode, 4) == 0)
				{
					sprintf(dynStmt,	"INSERT INTO %s%c"
										"("
										"	   ID_NO,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,PRICE_MEAN,SVRMEAN_ORDER,"
										"	   PRICE_VALUE,PRICE_TYPE,PRODUCT_CODE,BEGIN_TIME,END_TIME,"
										"	   LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME"
										")"
										"VALUES"
										"("
											"to_number(:v1), :v2, :v3, :v4, :v5,to_number(:v6),"
											"to_number(:v7),:v8, :v9, :v10, :v11,"
											"to_number(:v12),:v13,:v14,to_date(:v15, 'YYYYMMDD HH24:MI:SS')"
										")",
										priceTableName,idNo[strlen(idNo)-1]);
					EXEC SQL	PREPARE predCustPrice1 FROM :dynStmt;
					EXEC SQL	EXECUTE predCustPrice1 USING :idNo, :serviceType, :serviceCode,:actualPrice, :tProductPrices[i].priceMean,
									:tProductPrices[i].meanOrder,:tProductPrices[i].priceValue,
									:tProductPrices[i].priceType, :productCode,:beginTime,:endTime,
									:loginAccept,:loginNo, :opCode,:opTime;
					if (SQLCODE !=0)
					{
						pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:%s[%d][%s]",
							priceTableName,SQLCODE,SQLERRMSG);
						strcpy(retMsg,"记录用户产品服务个性定价表错误!");
						EXEC SQL	CLOSE curProductPrice;
						return 1319;
					}

					/*非二次批价，二次批价信息不插入此表*/
					if (serviceType[0] != '0')
					{
						EXEC SQL	INSERT INTO wCustPriceChg
									(
										op_no,op_type,op_time,id_no,product_code, service_type,service_code,
										price_code,price_gene, price_value, price_type, begin_time,end_time
									)
									values
									(
										sMaxBillChg.nextval,'1',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),to_number(:idNo),
										:productCode, :serviceType,:serviceCode,:actualPrice, :tProductPrices[i].priceMean,:tProductPrices[i].priceValue,
										:tProductPrices[i].priceType,
										to_date(:beginTime,'YYYYMMDD HH24:MI:SS'),to_date(:endTime,'YYYYMMDD HH24:MI:SS')
									);
						if(SQLCODE!=0)
						{
								pubLog_Debug(_FFL_, "_putProdService", "", "beginProduct:wCustPriceChg[%d][%s]",
									SQLCODE,SQLERRMSG);
								strcpy(retMsg,"记录帐务表wCustPriceChg错误!");
								EXEC SQL	CLOSE curProductPrice;
								return 1320;
						}
					}/*非二次批价*/
				}
			}
		}
		/*第三层*/
	}
	/*第二层*/
	EXEC SQL	CLOSE curProductPrice;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_putProdService", "", "_putProdService: End Execute");
#endif
	return 0;
}

/**
 * 删除产品服务信息
 *
 *@inparam	  isProductService 1:是产品服务 0:是品牌服务
 */
int _delProdService(const char *phoneNo, const char *idNo, const char *belongCode, const char *smCode,
		const char *effectTime,
		const char *portCode, const char *simNo, const char *imsiNo,
		const char *regionCode, const char *oldProduct, const char *oldAccept,
		const char *serviceType, const char *serviceCode, const char *beginTime, const char *endTime,
		const char *srvMsgTableName,const char *srvMsgAddTableName,	 const char *priceTableName,
		const char *loginNo,const char *loginAccept, const char *opTime, const char *opCode,
		const char *totalDate, const char *groupId, const char *orgCode, const char *orgId, char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_delProdService", "", "_delProdService: Start Execute");
#endif
	/*产品服务定义*/
	int opType;
	
	/*ng解耦 by yaoxc begin*/
	int  icount=0;
	int  v_ret=0;
	TdBaseFavIndex tdBaseFavIndex;
	TdBaseVpmnIndex tdBaseVpmnIndex;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	char sTempSqlStr[1024];
	char sRegionCode[2+1];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	memset(&tdBaseFavIndex,0,sizeof(tdBaseFavIndex));
	memset(&tdBaseVpmnIndex,0,sizeof(tdBaseVpmnIndex));
	init(v_parameter_array);
	init(sTempSqlStr);
	init(sRegionCode);
	/*ng解耦 by yaoxc end*/

	/*BOSS优惠集团部分处理*/
	char vpmnGroup[10+1], dynStmt[1024], dataNo[15+1];

	char yearMonth[6+1];

	char rateCode[4+1];

	memset(yearMonth, 0, sizeof(yearMonth));
	strncpy(yearMonth, totalDate, 6);

	pubLog_Debug(_FFL_, "_delProdService", "", "serviceType=[%s]",serviceType);
	switch(serviceType[0])
	{
		case '0':
		{
			memset(rateCode, 0, sizeof(rateCode));
			EXEC SQL	SELECT	rate_code
						  INTO :rateCode
						  FROM sSrvRateCode
						 WHERE service_code = :serviceCode;
			if (SQLCODE !=0)
			{
				pubLog_Debug(_FFL_, "_delProdService", "", "_delProdService:sSrvRateCode[%d][%s][%s]",
					SQLCODE,SQLERRMSG,serviceCode);
				strcpy(retMsg,"查询sSrvRateCode错误!");
				return 1400;
			}

			if (strncmp(endTime,effectTime, 17) > 0)
			{
#if PROVINCE_RUN==PROVINCE_SICHUAN
				EXEC SQL	UPDATE dBaseFav
					SET end_date=to_date(:effectTime,'YYYYMMDD HH24:MI:SS')
					WHERE msisdn=:phoneNo
						AND fav_type=:rateCode
						AND begin_date=to_date(:beginTime,'YYYYMMDD HH24:MI:SS')
						AND product_id = :oldProduct;
#else
		/*ng解耦
		EXEC SQL	UPDATE dBaseFav
          			SET end_time=to_date(:effectTime,'YYYYMMDD HH24:MI:SS')
          			WHERE msisdn=:phoneNo
          			AND fav_type=:rateCode
            		AND end_time=To_Date(:endTime,'YYYYMMDD HH24:MI:SS');
        ng解耦*/
		/*ng解耦 by yaoxc begin*/

		EXEC SQL SELECT to_char(end_time,'YYYYMMDD HH24:MI:SS') into :endTime
			from dBaseFav
		where msisdn=:phoneNo and fav_type=:rateCode ;

		
/*
		EXEC SQL SELECT COUNT(*) INTO :v_ret  from  dBaseFav where msisdn=:phoneNo
			and fav_type=:rateCode 
			AND end_time=To_Date(:endTime,'YYYYMMDD HH24:MI:SS');
				
				AND to_char(end_time,'YYYYMMDD HH24:MI:SS')='21090818 00:00:00';

		init(sTempSqlStr);
		sprintf(sTempSqlStr,"select count(*) from dBaseFav "
		 					"where msisdn=:phoneNo");
		EXEC SQL EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :sTempSqlStr INTO :v_ret USING :phoneNo,:rateCode,:endTime;
			END;
		END-EXEC;	

printf("--------------[%d]\n",v_ret);	
*/	
		init(sTempSqlStr);
		sprintf(sTempSqlStr,"select msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
							"from dBaseFav "
		 					"where msisdn=rpad(:phoneNo,15,' ') and fav_type=:rateCode "
							"and end_time=To_Date(:endTime,'YYYYMMDD HH24:MI:SS')");

		EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
		EXEC SQL DECLARE ngcur01 cursor for sql_str;
		EXEC SQL OPEN ngcur01 using :phoneNo,:rateCode,:endTime;
		for(;;)
		{
			memset(&tdBaseFavIndex,0,sizeof(tdBaseFavIndex));			
			
			printf("sTempSqlStr=[%s]\n",sTempSqlStr);
			printf("1 SQLCODE=[%d]\n",SQLCODE);
			printf("1 tdBaseFavIndex.sMsisdn = [%s]\n",tdBaseFavIndex.sMsisdn);
			printf("1 tdBaseFavIndex.sFavType = [%s]\n",tdBaseFavIndex.sFavType);
			printf("1 tdBaseFavIndex.sFlagCode = [%s]\n",tdBaseFavIndex.sFlagCode);
			printf("1 tdBaseFavIndex.sStartTime = [%s]\n",tdBaseFavIndex.sStartTime);
			/*EXEC SQL FETCH ngcur01 INTO tdBaseFavIndex;*/
			
			EXEC SQL FETCH ngcur01 INTO :tdBaseFavIndex.sMsisdn,:tdBaseFavIndex.sFavType,:tdBaseFavIndex.sFlagCode,:tdBaseFavIndex.sStartTime;
			
			printf("2 SQLCODE=[%d]\n",SQLCODE);
			printf("2 tdBaseFavIndex.sMsisdn = [%s]\n",tdBaseFavIndex.sMsisdn);
			printf("2 tdBaseFavIndex.sFavType = [%s]\n",tdBaseFavIndex.sFavType);
			printf("2 tdBaseFavIndex.sFlagCode = [%s]\n",tdBaseFavIndex.sFlagCode);
			printf("2 tdBaseFavIndex.sStartTime = [%s]\n",tdBaseFavIndex.sStartTime);
			
			if((0!=SQLCODE) && (1403!=SQLCODE))
			{
				sprintf(retMsg, "查询dBaseFav错误\n");
				EXEC SQL CLOSE ngcur01;
				return -1;
			}
			if(1403==SQLCODE) break;
				
			strcpy(v_parameter_array[0],effectTime);
			strcpy(v_parameter_array[1],phoneNo);
			strcpy(v_parameter_array[2],rateCode);
			strcpy(v_parameter_array[3],tdBaseFavIndex.sFlagCode);
			strcpy(v_parameter_array[4],tdBaseFavIndex.sStartTime);
			
    	
			v_ret=OrderUpdateBaseFav(ORDERIDTYPE_USER,idNo,100,
						   			opCode,atol(loginAccept),loginNo,"_delProdService",
									tdBaseFavIndex,tdBaseFavIndex,
									" end_time=to_date(:effectTime,'YYYYMMDD HH24:MI:SS') ","",v_parameter_array);
			printf("OrderUpdateBaseFav ,ret=[%d]\n",v_ret);
			if(0!=v_ret)
			{
				pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s] serviceCode=[%s][%s]",
						phoneNo,SQLCODE,SQLERRMSG,serviceCode,beginTime);
				strcpy(retMsg,"更改dBaseFav错误!");
				EXEC SQL CLOSE ngcur01;
				return 1400;
			}
			icount++;
		}
		EXEC SQL CLOSE ngcur01;
		if(icount == 0)
    	{
    		pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s] serviceCode=[%s][%s]",
						phoneNo,SQLCODE,SQLERRMSG,serviceCode,beginTime);
			strcpy(retMsg,"更改dBaseFav错误!");
			return 1400;
    	}
#endif
/*				if(SQLCODE!=OK)
				{
					pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s] serviceCode=[%s][%s]",
						phoneNo,SQLCODE,SQLERRMSG,serviceCode,beginTime);
					strcpy(retMsg,"更改dBaseFav错误!");
					return 1400;
				}*/
/*ng解耦 by yaoxc end*/
#ifdef _CHGTABLE_
#if PROVINCE_RUN==PROVINCE_SICHUAN
				EXEC SQL	INSERT INTO wBaseFavChg
							(
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,flag,deal_date
							)
							select
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,'3',to_date(:opTime,'YYYYMMDD HH24:MI:SS')
							  from dBaseFav
							 where msisdn=:phoneNo
							   and fav_type=:rateCode
							   and begin_date=to_date(:beginTime,'YYYYMMDD HH24:MI:SS')
							   AND product_id = :oldProduct;
#elif PROVINCE_RUN==PROVINCE_JILIN
         /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				EXEC SQL	INSERT INTO wBaseFavChg
						(
							msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
							start_time,end_time,flag,deal_time,service_id,id_no,group_id,product_code
						)
						select
								msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
								start_time,end_time,'3',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),service_id,id_no,group_id,product_code
							  from dBaseFav
							 where msisdn=:phoneNo
							   and fav_type=:rateCode
							   and start_time=to_date(:beginTime,'YYYYMMDD HH24:MI:SS');
#else
				EXEC SQL	INSERT INTO wBaseFavChg
							(
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,flag,deal_date
							)
							select
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,'3',to_date(:opTime,'YYYYMMDD HH24:MI:SS')
							  from dBaseFav
							 where msisdn=:phoneNo
							   and fav_type=:rateCode
							   and begin_date=to_date(:beginTime,'YYYYMMDD HH24:MI:SS')
							   AND product_id = :oldProduct;
#endif
				if(SQLCODE!=OK||SQLROWS!=1)
				{
					pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:wBaseFavChg.phoneNo=[%s][%d][%s]",
						phoneNo,SQLCODE,SQLERRMSG);
					strcpy(retMsg,"插入wBaseFavChg错误!");
					return 1401;
				}
#endif

				/*删除预约未生效的记录*/
#if PROVINCE_RUN==PROVINCE_SICHUAN
				EXEC SQL	DELETE dBaseFav
							 where msisdn=:phoneNo
							   and fav_type=:rateCode
							   and begin_date>=end_date;
#elif PROVINCE_RUN==PROVINCE_JILIN
		/*ng解耦EXEC SQL    DELETE dBaseFav
                         where msisdn=:phoneNo
                           and fav_type=:rateCode
                           and start_time>=end_time;
                ng解耦*/
				/*ng解耦 by yaoxc begin*/
				init(sTempSqlStr);
				sprintf(sTempSqlStr,"select msisdn,fav_type,flag_code,to_char(start_time,'YYYYMMDD HH24:MI:SS') "
									"from dBaseFav "
				 					"where msisdn=:phoneNo and fav_type=:rateCode "
									"and begin_date>=end_date ");
				EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
				EXEC SQL DECLARE ngcur02 cursor for sql_str;
				EXEC SQL OPEN ngcur02 using :phoneNo,:rateCode;
				for(;;)
				{
					memset(&tdBaseFavIndex,0,sizeof(tdBaseFavIndex));
					EXEC SQL FETCH ngcur02 INTO :tdBaseFavIndex;
					if((0!=SQLCODE) && (1403!=SQLCODE))
					{
						sprintf(retMsg, "查询dBaseFav错误\n");
						EXEC SQL CLOSE ngcur02;
						return -1;
					}
					if(1403==SQLCODE) break;
						
					strcpy(v_parameter_array[0],tdBaseFavIndex.sMsisdn);
					strcpy(v_parameter_array[1],tdBaseFavIndex.sFavType);
					strcpy(v_parameter_array[2],tdBaseFavIndex.sFlagCode);
					strcpy(v_parameter_array[3],tdBaseFavIndex.sStartTime);
    			
					v_ret=OrderDeleteBaseFav(ORDERIDTYPE_USER,idNo,100,
								   			opCode,atol(loginAccept),loginNo,"_delProdService",
											tdBaseFavIndex,"",v_parameter_array);
					printf("OrderDeleteBaseFav ,ret=[%d]\n",v_ret);
					if(0!=v_ret)
					{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s]",
						phoneNo,SQLCODE,SQLERRMSG);
						strcpy(retMsg,"删除dBaseFav错误!");
						EXEC SQL CLOSE ngcur02;
						return 1404;
					}
				}
				EXEC SQL CLOSE ngcur02;
#else              /* 山西 */
				EXEC SQL        DELETE dBaseFav
                         where msisdn=:phoneNo
                           and fav_type=:rateCode
                           and begin_time>=end_time;
#endif
 /* 				if(SQLCODE!=OK&&SQLCODE!=NOTFOUND)
				{
					pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s]",
						phoneNo,SQLCODE,SQLERRMSG);
					strcpy(retMsg,"删除dBaseFav错误!");
					return 1404;
				} */
/*ng解耦 by yaoxc end*/
				init(vpmnGroup);
				EXEC SQL	SELECT group_index
							  INTO :vpmnGroup
							  FROM sBillVpmnCond
							 WHERE region_code=:regionCode
							   AND mode_code=:oldProduct;
				if(SQLROWS==1)
				{
					Trim(vpmnGroup);
#ifdef _CHGTABLE_
				EXEC SQL	INSERT INTO wBaseVpmnChg
								(
									msisdn,groupid,mocrate,mtcrate,valid,invalid,flag,deal_time
								)
								SELECT
									msisdn,groupid,mocrate,mtcrate,valid,
									to_date(:effectTime,'yyyymmdd hh24:mi:ss'),'2',to_date(:opTime,'yyyymmdd hh24:mi:ss')
								  FROM dBaseVpmn
								 WHERE msisdn = :phoneNo
								   AND groupid = :vpmnGroup
								   AND valid = to_date(:beginTime,'yyyymmdd hh24:mi:ss');
					if(SQLCODE!=0)
					{
							pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:wBaseVpmnChg[%d][%s]",
								SQLCODE,SQLERRMSG);
							sprintf(retMsg,"处理wBaseVpmnChg默认集团%s时错误!", vpmnGroup);
							return 1405;
					}
#endif
/*ng解耦
					EXEC SQL	UPDATE dBaseVpmn
								   SET invalid=to_date(:effectTime,'yyyymmdd hh24:mi:ss')
								 WHERE msisdn = :phoneNo
								   AND groupid = :vpmnGroup
								   AND valid = to_date(:beginTime,'yyyymmdd hh24:mi:ss');
					if(SQLCODE!=0)
					{
							pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseVpmn[%d][%s]",
								SQLCODE,SQLERRMSG);
							strcpy(retMsg,"更新计费表dBaseVpmn错误!");
							return 1406;
					}
ng解耦*/
		/*ng解耦 by yaoxc begin*/
		memset(&tdBaseVpmnIndex,0,sizeof(tdBaseVpmnIndex));
		init(v_parameter_array);
		
		strcpy(tdBaseVpmnIndex.sMsisdn,phoneNo);
		strcpy(tdBaseVpmnIndex.sGroupId,vpmnGroup);
		strcpy(tdBaseVpmnIndex.sValid,beginTime);
		
		strcpy(v_parameter_array[0],effectTime);
		strcpy(v_parameter_array[1],phoneNo);
		strcpy(v_parameter_array[2],vpmnGroup);
		strcpy(v_parameter_array[3],beginTime);
		
		v_ret=OrderUpdateBaseVpmn(ORDERIDTYPE_USER,idNo,100,
						   	opCode,atol(loginAccept),loginNo,"_delProdService",
							tdBaseVpmnIndex,tdBaseVpmnIndex,
							" invalid=to_date(:effectTime,'yyyymmdd hh24:mi:ss') ","",v_parameter_array);
		printf("OrderUpdateBaseVpmn ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseVpmn[%d][%s]",
								SQLCODE,SQLERRMSG);
			strcpy(retMsg,"更新计费表dBaseVpmn错误!");
			return 1406;
		}
		/*ng解耦 by yaoxc end*/
					/*删除预约未生效的记录*/
/*ng解耦
					EXEC SQL	DELETE dBaseVpmn
								 WHERE msisdn = :phoneNo
								   AND groupid = :vpmnGroup
								   AND valid >= invalid;
					if(SQLCODE!=0 && SQLCODE != NOTFOUND)
					{
							pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseVpmn[%d][%s]",
								SQLCODE,SQLERRMSG);
							strcpy(retMsg,"删除计费表dBaseVpmn错误!");
							return 1407;
					}
ng解耦*/
				/*ng解耦 by yaoxc begin*/
				init(sTempSqlStr);
				sprintf(sTempSqlStr,"select msisdn,groupid,to_char(valid,'yyyymmdd hh24:mi:ss') "
									"from dBaseVpmn "
				 					"where msisdn=:phoneNo and groupid = :vpmnGroup "
									"and valid >= invalid ");
				EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
				EXEC SQL DECLARE ngcur04 cursor for sql_str;
				EXEC SQL OPEN ngcur04 using :phoneNo,:vpmnGroup;
				for(;;)
				{
					memset(&tdBaseVpmnIndex,0,sizeof(tdBaseVpmnIndex));
					EXEC SQL FETCH ngcur04 INTO :tdBaseVpmnIndex;
					if((0!=SQLCODE) && (1403!=SQLCODE))
					{
						sprintf(retMsg, "查询dBaseVpmn错误\n");
						EXEC SQL CLOSE ngcur04;
						return -1;
					}
					if(1403==SQLCODE) break;
						
					strcpy(v_parameter_array[0],tdBaseVpmnIndex.sMsisdn);
					strcpy(v_parameter_array[1],tdBaseVpmnIndex.sGroupId);
					strcpy(v_parameter_array[2],tdBaseVpmnIndex.sValid);
    			
					v_ret=OrderDeleteBaseVpmn(ORDERIDTYPE_USER,idNo,100,
								   			opCode,atol(loginAccept),loginNo,"_delProdService",
											tdBaseVpmnIndex,"",v_parameter_array);
					printf("OrderDeleteBaseVpmn ,ret=[%d]\n",v_ret);
					if(0!=v_ret)
					{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav.phoneNo=[%s][%d][%s]",
						phoneNo,SQLCODE,SQLERRMSG);
						strcpy(retMsg,"删除dBaseFav错误!");
						EXEC SQL CLOSE ngcur04;
						return 1404;
					}
				}
				EXEC SQL CLOSE ngcur04;
				/*ng解耦 by yaoxc end*/
#if PROVINCE_RUN==PROVINCE_SICHUAN
					EXEC SQL	UPDATE dBaseFav
								   SET end_date=to_date(:effectTime,'YYYYMMDD HH24:MI:SS')
								 WHERE msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and begin_date=to_date(:beginTime,'YYYYMMDD HH24:MI:SS')
								   AND product_id = :oldProduct;
#elif  PROVINCE_RUN==PROVINCE_JILIN
		/*ng解耦EXEC SQL	UPDATE dBaseFav
								   SET end_time=to_date(:effectTime,'YYYYMMDD HH24:MI:SS')
								 WHERE msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and start_time=to_date(:beginTime,'YYYYMMDD HH24:MI:SS');
		ng解耦*/
		/*ng解耦 by yaoxc begin*/
		memset(&tdBaseFavIndex,0,sizeof(tdBaseFavIndex));
		init(v_parameter_array);
		
		strcpy(tdBaseFavIndex.sMsisdn,phoneNo);
		strcpy(tdBaseFavIndex.sFavType,"aa00");
		strcpy(tdBaseFavIndex.sFlagCode,vpmnGroup);
		strcpy(tdBaseFavIndex.sStartTime,beginTime);
		
		strcpy(v_parameter_array[0],effectTime);
		strcpy(v_parameter_array[1],phoneNo);
		strcpy(v_parameter_array[2],"aa00");
		strcpy(v_parameter_array[3],vpmnGroup);
		strcpy(v_parameter_array[4],beginTime);
		
		v_ret=OrderUpdateBaseFav(ORDERIDTYPE_USER,idNo,100,
						   	opCode,atol(loginAccept),loginNo,"_delProdService",
							tdBaseFavIndex,tdBaseFavIndex,
							" end_time=to_date(:effectTime,'YYYYMMDD HH24:MI:SS') ","",v_parameter_array);
		printf("OrderUpdateBaseFav ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav[%d][%s]",
						SQLCODE,SQLERRMSG);
			strcpy(retMsg,"更新计费表dBaseFav错误!");
			return 1408;
		}
#else	/* 山西 */
					EXEC SQL	UPDATE dBaseFav
								   SET end_time=to_date(:effectTime,'YYYYMMDD HH24:MI:SS')
								 WHERE msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and begin_time=to_date(:beginTime,'YYYYMMDD HH24:MI:SS');
#endif
/*					if(SQLCODE!=0)
					{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav[%d][%s]",
						SQLCODE,SQLERRMSG);
						strcpy(retMsg,"更新计费表dBaseFav错误!");
						return 1408;
					}*/
/*ng解耦 by yaoxc end*/
#ifdef _CHGTABLE_
#if PROVINCE_RUN==PROVINCE_SICHUAN
				EXEC SQL	INSERT INTO wBaseFavChg
								(
									msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
									begin_date,end_date,id_no,group_id,product_id,flag,deal_date
								)
								select
									msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
									begin_date,end_date,id_no,group_id,product_id,'2',to_date(:opTime, 'YYYYMMDD HH24:MI:SS')
								  from dBaseFav
								 where msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and begin_date=to_date(:beginTime,'YYYYMMDD HH24:MI:SS')
								   AND product_id = :oldProduct;
#elif PROVINCE_RUN==PROVINCE_JILIN
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				EXEC SQL	INSERT INTO wBaseFavChg
						(
							msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
							start_time,end_time,flag,deal_time,service_id,id_no,group_id,product_code
						)
						select
								msisdn,region_code,fav_brand,fav_type,flag_code,fav_order,
								start_time,end_time,'3',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),service_id,id_no,group_id,product_code
							  from dBaseFav
							 where msisdn=:phoneNo
							   and fav_type='aa00'
							   and flag_code=:vpmnGroup
							   and start_time=to_date(:beginTime,'YYYYMMDD HH24:MI:SS');
#else
				EXEC SQL	INSERT INTO wBaseFavChg
							(
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,flag,deal_date
							)
							select
								msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,
								begin_date,end_date,id_no,group_id,product_id,'3',to_date(:opTime,'YYYYMMDD HH24:MI:SS')
							  from dBaseFav
							 where msisdn=:phoneNo
							   and fav_type='aa00'
							   and flag_code=:vpmnGroup
							   and start_time=to_date(:beginTime,'YYYYMMDD HH24:MI:SS');
#endif


					if(SQLCODE!=0)
					{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:wBaseFavChg[%d][%s]",
						SQLCODE,SQLERRMSG);
						strcpy(retMsg,"更新计费表wBaseFavChg错误!");
						return 1409;
					}
#endif

#if PROVINCE_RUN==PROVINCE_SICHUAN
					EXEC SQL	DELETE dBaseFav
								 where msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and begin_date>=end_date;
#elif PROVINCE_RUN==PROVINCE_JILIN
/*ng解耦			EXEC SQL	DELETE dBaseFav
								 where msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and start_time>=end_time;
ng解耦*/
				/*ng解耦 by yaoxc begin*/
				init(sTempSqlStr);
				sprintf(sTempSqlStr,"select msisdn,fav_type,flag_code,to_char(start_time,'yyyymmdd hh24:mi:ss') "
									"from dBaseFav "
				 					"where msisdn=:phoneNo and fav_type='aa00' "
									"and flag_code=:vpmnGroup and start_time>=end_time ");
				EXEC SQL PREPARE sql_str FROM :sTempSqlStr;
				EXEC SQL DECLARE ngcur03 cursor for sql_str;
				EXEC SQL OPEN ngcur03 using :phoneNo,:vpmnGroup;
				for(;;)
				{
					memset(&tdBaseFavIndex,0,sizeof(tdBaseFavIndex));
					EXEC SQL FETCH ngcur03 INTO :tdBaseFavIndex;
					if((0!=SQLCODE) && (1403!=SQLCODE))
					{
						sprintf(retMsg, "查询dBaseFav错误\n");
						EXEC SQL CLOSE ngcur03;
						return -1;
					}
					if(1403==SQLCODE) break;
    			
    				strcpy(v_parameter_array[0],tdBaseFavIndex.sMsisdn);
					strcpy(v_parameter_array[1],tdBaseFavIndex.sFavType);
					strcpy(v_parameter_array[2],tdBaseFavIndex.sFlagCode);
					strcpy(v_parameter_array[3],tdBaseFavIndex.sStartTime);
    				
					v_ret=OrderDeleteBaseFav(ORDERIDTYPE_USER,idNo,100,
								   			opCode,atol(loginAccept),loginNo,"_delProdService",
											tdBaseFavIndex,"",v_parameter_array);
					printf("OrderDeleteBaseFav ,ret=[%d]\n",v_ret);
					if(0!=v_ret)
					{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav[%d][%s]",
								SQLCODE,SQLERRMSG);
						strcpy(retMsg,"删除计费表dBaseFav错误!");
						EXEC SQL CLOSE ngcur03;
						return 1410;
					}
				}
				EXEC SQL CLOSE ngcur03;
#else
					EXEC SQL	DELETE dBaseFav
								 where msisdn=:phoneNo
								   and fav_type='aa00'
								   and flag_code=:vpmnGroup
								   and begin_time>=end_time;
#endif
/*					if(SQLCODE!=0 && SQLCODE != NOTFOUND)
					{
							pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dBaseFav[%d][%s]",
								SQLCODE,SQLERRMSG);
							strcpy(retMsg,"删除计费表dBaseFav错误!");
							return 1410;
					}*/
/*ng解耦 by yaoxc end*/
				}
			}
			break;
		}
		case '1': case '2': case '3': case '4': case '5': case '6': case '9':
		{
			if (strncmp(effectTime, beginTime, 17) <=0)
			{
				opType=3;
			}
			else
			{
				opType=2;
			}
			/*四川暂时不用此表*/
#if PROVINCE_RUN==PROVINCE_SHANXI
			sprintf(dynStmt,        "INSERT INTO wSrvMsgChg"
                                                    "("
                                                    "          op_no,op_type,op_time,id_no,product_code, service_type,service_code,"
                                                    "          price_code,FAV_ORDER, begin_time,end_time, person_flag,"
                                                    "          op_code,   login_accept, phone_no "
                                                    ")"
                                                    "SELECT"
                                                    "          sMaxBillChg.nextval,:opType,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),id_no,"
                                                    "          product_code, service_type,service_code,price_code, FAV_ORDER,"
                                                    "          to_date(begin_time,'YYYYMMDD HH24:MI:SS'),to_date(:effectTime,'YYYYMMDD HH24:MI:SS'), person_flag,"
                                                    "          :opCode, to_number(trim(:login_accept)),:phoneNo "
                                                    "  FROM %s%c"
                                                    " WHERE id_no = to_number(:idNO)"
                                                    "   AND product_code = :oldProduct"
                                                    "   AND login_accept = to_number(:oldAccept)"
                                                    "   AND service_code = :serviceCode", srvMsgTableName,idNo[strlen(idNo)-1]);
            EXEC SQL PREPARE predCustPrice2 from :dynStmt;
            EXEC SQL EXECUTE predCustPrice2 using :opType, :opTime, :effectTime,:opCode,:loginAccept,:phoneNo,
                            :idNo, :oldProduct, :oldAccept, :serviceCode;
            if(SQLCODE!=0)
            {
                    pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wSrvMsgChg[%d][%s]",
                                    SQLCODE,SQLERRMSG);
                    strcpy(retMsg,"记录帐务表wSrvMsgChg错误!");
                    return 1411;
            }
#endif
/*
			sprintf(dynStmt,	"INSERT INTO wSrvMsgChg"
								"("
								"	   op_no,op_type,op_time,id_no,product_code, service_type,service_code,"
								"	   price_code,FAV_ORDER, begin_time,end_time, person_flag"
								")"
								"SELECT"
								"	   sMaxBillChg.nextval,:opType,to_date(:opTime,'YYYYMMDD HH24:MI:SS'),id_no,"
								"	   product_code, service_type,service_code,price_code, FAV_ORDER,"
								"	   to_date(begin_time,'YYYYMMDD HH24:MI:SS'),to_date(:effectTime,'YYYYMMDD HH24:MI:SS'), person_flag"
								"  FROM %s%c"
								" WHERE id_no = to_number(:idNO)"
								"   AND product_code = :oldProduct"
								"   AND login_accept = to_number(:oldAccept)"
								"   AND service_code = :serviceCode", srvMsgTableName,idNo[strlen(idNo)-1]);
			EXEC SQL PREPARE predCustPrice2 from :dynStmt;
			EXEC SQL EXECUTE predCustPrice2 using :opType, :opTime, :effectTime,
					:idNo, :oldProduct, :oldAccept, :serviceCode;
			if(SQLCODE!=0)
			{
				pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wCustPriceChg[%d][%s]",
						SQLCODE,SQLERRMSG);
				strcpy(retMsg,"记录帐务表wSrvMsgChg错误!");
				return 1411;
			}
*/

			/*luxc
			sprintf(dynStmt,	"INSERT INTO wCustPriceChg"
								"("
								"	   op_no,op_type,op_time,id_no,product_code, service_type,service_code,"
								"	   price_code,price_gene, price_value, price_type, begin_time,end_time"
								")"
								"SELECT"
								"	   sMaxBillChg.nextval,:v1,to_date(:v2,'YYYYMMDD HH24:MI:SS'),id_no,"
								"	   product_code, service_type,service_code,price_code, price_mean, price_value,"
								"	   price_type,"
								"	   to_date(:v3,'YYYYMMDD HH24:MI:SS'),to_date(:v4,'YYYYMMDD HH24:MI:SS')"
								"  FROM %s%c"
								" WHERE id_no = to_number(:v4)"
								"   AND product_code = :v5"
								"   AND login_accept = to_number(:v6)"
								"   AND service_code = :v7", priceTableName,idNo[strlen(idNo)-1]);

			pubLog_Debug(_FFL_, "_delProdService", "", "dynStmt:%s,opType:%s,opTime:%s",dynStmt,opType,opTime);

			EXEC SQL PREPARE predCustPrice2 from :dynStmt;
			EXEC SQL EXECUTE predCustPrice2 using :opType, :opTime, :beginTime, :effectTime,
					:idNo, :oldProduct, :oldAccept, :serviceCode;
			if(SQLCODE!=0)
			{
					pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wCustPriceChg[%d][%s]",
						SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录帐务表wCustPriceChg错误!");
					return 1412;
			}
			*/

			/*如果是立即生效，并且存在指令的服务要求发送开关机关闭指令。如：增值服务*/
			pubLog_Debug(_FFL_, "_delProdService", "", "pubProduct:beginTime=[%s],opTime=[%s]",beginTime,opTime);
			if (strncmp(beginTime, opTime, 17) <= 0)
			{
#if PROVINCE_RUN!=PROVINCE_JILIN
				EXEC SQL	INSERT INTO wSndCmdDay
							(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,
								command_code,new_phone,imsi_no,new_imsi,other_char,op_code,total_date,
								op_time,login_no,org_code,login_accept,
								request_time,business_status,send_status,send_time,
								group_id,org_id
							)
							SELECT
								sOffOn.nextval,:portCode,0,to_number(:idNo),:belongCode,:smCode,:phoneNo,
								b.command_code, :phoneNo,:imsiNo,:imsiNo,:simNo,:opCode,to_number(:totalDate),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:loginNo,:orgCode,to_number(:loginAccept),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),'0','0',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),
								:groupId, :orgId
							  FROM sSrvCmd a, sStatCmdCode b
							 WHERE a.service_code = :serviceCode
							   AND a.cmd_code = b.cmd_code
							 /*ORDER BY b.command_order*/;
#else
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				EXEC SQL	INSERT INTO wSndCmdDay
							(
								command_id,hlr_code,command_order,id_no,belong_code,sm_code,phone_no,
								command_code,new_phone,imsi_no,new_imsi,other_char,op_code,total_date,
								op_time,login_no,org_code,login_accept,
								request_time,business_status,send_status,send_time,group_id,org_id
							)
							SELECT
								sOffOn.nextval,:portCode,0,to_number(:idNo),:belongCode,:smCode,:phoneNo,
								b.command_code, :phoneNo,:imsiNo,:imsiNo,:simNo,:opCode,to_number(:totalDate),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:loginNo,:orgCode,to_number(:loginAccept),
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),'0','0',to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:groupId, :orgId
							  FROM sSrvCmd a, sStatCmdCode b
							 WHERE a.service_code = :serviceCode
							   AND a.cmd_code = b.cmd_code
							 /*ORDER BY b.command_order*/;
#endif
				if(SQLCODE!=0 && SQLCODE != NOTFOUND)
				{
						pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:wSndCmdDay[%d][%s]",
							SQLCODE,SQLERRMSG);
						strcpy(retMsg,"记录表wSndCmdDay错误!");
						return 1413;
				}
			}
			else
			{
#if PROVINCE_RUN!=PROVINCE_JILIN
				EXEC SQL	INSERT INTO wCustExpireHis
							(
								command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
								total_date,op_code,op_time,function_code,business_status,
								expire_time,op_note, org_id,
								UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE
							)
							SELECT
								command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
								total_date,op_code,op_time,function_code,business_status,
								expire_time,op_note, org_id,
								to_number(:loginAccept),to_date(:opTime,'yyyymmdd hh24:mi:ss'), :loginNo, 'd',
								:opCode, to_number(:totalDate)
							  FROM wCustExpire
							 WHERE id_no=to_number(:idNo)
							   and function_code=:serviceCode
					   and login_accept=to_number(:oldAccept);
#else
/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				EXEC SQL	INSERT INTO wCustExpireHis
							(
								command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
								total_date,op_code,op_time,function_code,business_status,
								expire_time,op_note,
								UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE,org_id
							)
							SELECT
								command_id,sm_code,id_no,phone_no,org_code,login_no,login_accept,
								total_date,op_code,op_time,function_code,business_status,
								expire_time,op_note,
								to_number(:loginAccept),to_date(:opTime,'yyyymmdd hh24:mi:ss'), :loginNo, 'd',
								:opCode, to_number(:totalDate),org_id
							  FROM wCustExpire
							 WHERE id_no=to_number(:idNo)
							   and function_code=:serviceCode
					   and login_accept=to_number(:oldAccept);
#endif
				if(SQLCODE!=0 && SQLCODE != NOTFOUND)
				{
						pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wCustExpireHis[%d][%s]",
							SQLCODE,SQLERRMSG);
						strcpy(retMsg,"记录到期指令表wCustExpireHis错误!");
						return 1414;
				}

				EXEC SQL	DELETE FROM wCustExpire
							 WHERE id_no=to_number(:idNo)
							   and function_code=:serviceCode
							   and login_accept=to_number(:oldAccept);
				if(SQLCODE!=0 && SQLCODE != NOTFOUND)
				{
						pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wCustExpire[%d][%s]",
							SQLCODE,SQLERRMSG);
						strcpy(retMsg,"删除到期指令表wCustExpire错误!");
						return 1415;
				}
			}

			/*处理短号码
			memset(dataNo, 0, sizeof(dataNo));
			sprintf(dynStmt,	"SELECT attr_value"
								"  FROM %s%c"
								" WHERE id_no = to_number(:idNo)"
								"   AND product_code = :productCode"
								"   AND service_type = :serviceType"
								"   AND service_code = :serviceCode"
								"   AND service_attr in ('%s', '%s', '%s')"
								"   AND begin_time <= :beginTime"
								"   AND end_time >= :endTime",
								srvMsgAddTableName, idNo[strlen(idNo)-1], SRV_ATTR_DATA_NO, SRV_ATTR_FAX_NO,
								SRV_ATTR_SEEK_NO);
#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "_delProdService", "", "[%s]", dynStmt);
#endif
			EXEC SQL	EXECUTE
			BEGIN
				EXECUTE IMMEDIATE :dynStmt INTO :dataNo USING :idNo, :oldProduct, :serviceType, :serviceCode,
						:opTime, :opTime;
			END;
			END-EXEC;
			if (dataNo[0] != '\0')
			{
				init(dynStmt);
				sprintf(dynStmt,	"INSERT INTO wCustResOpr%s"
									"("
									"	LOGIN_ACCEPT,TOTAL_DATE,NO_TYPE,BEGIN_NO,"
									"	END_NO,PASSWORD,OTHER_ATTR,CHOICE_FEE,"
									"	LIMIT_PREPAY,BEGIN_TIME,OLD_GROUP,OLD_LOGIN,"
									"	NEW_GROUP,NEW_LOGIN,RESOURCE_CODE,OP_TIME,"
									"	OP_CODE,LOGIN_NO,ORG_GROUP,NOTE,BAK_FIELD"
									")"
									"SELECT"
									"	to_number(:LOGIN_ACCEPT),to_number(:TOTAL_DATE),NO_TYPE,PHONE_NO,"
									"	PHONE_NO,PASSWORD,OTHER_ATTR,CHOICE_FEE,"
									"	LIMIT_PREPAY,BEGIN_TIME,GROUP_ID,LOGIN_NO,"
									"	GROUP_ID,LOGIN_NO,'2',to_char(to_date(:OP_TIME, 'YYYYMMDD HH24:MI:SS'), 'YYYYMMDDHH24MISS'),"
									"	:OP_CODE,:LOGIN_NO,:ORG_GROUP,NULL,NULL"
									"  FROM dCustRes"
									" WHERE phone_no = :phoneNo"
									,yearMonth);

				EXEC SQL PREPARE ins_stmt From :dynStmt;
				EXEC SQL EXECUTE ins_stmt using :loginAccept ,:totalDate, :opTime,
						:opCode, :loginNo, :groupId, :phoneNo;

				if (SQLCODE != 0)
				{
					sprintf(retMsg, "插入wCustResOpr%s 错误",yearMonth);
					pubLog_Debug(_FFL_, "_delProdService", "", "插入wCustResOpr%s 错误 [%d]" ,yearMonth,SQLCODE);
					pubLog_Debug(_FFL_, "_delProdService", "", "SQLERRMSG = [%s]",SQLERRMSG);
					return 1309;
				}

				EXEC SQL	UPDATE dCustRes
							   SET resource_code = '2',
								   login_no  = :loginNo,
								   login_accept = to_number(:loginAccept),
								   op_time	= TO_DATE(:opTime,'YYYYMMDD HH24:MI:SS'),
								   total_date   = TO_NUMBER(:totalDate),
								   org_code  = :orgCode
							 WHERE phone_no = :dataNo;
				if (SQLCODE != 0)
				{
					pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:dCustRes[%d][%s]",SQLCODE,SQLERRMSG);
					strcpy(retMsg,"更新号码资源信息错误!");
					return 1416;
				}
			}
			*/

			/*增值服务*/
			if (serviceType[0] == '5')
			{

#if PROVINCE_RUN==PROVINCE_SICHUAN
		EXEC SQL	INSERT INTO wCustFuncDay
							(
								ID_NO,PHONE_NO,FUNCTION_TYPE,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
								OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT,BELONG_CODE,ORG_CODE
							)
							VALUES
							(
								to_number(:idNo),:phoneNo,:serviceType,:serviceCode,'N',:totalDate,
								to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept),
								:belongCode,:orgCode
							);
#elif PROVINCE_RUN==PROVINCE_JILIN
		 EXEC SQL        INSERT INTO wCustFuncDay
                                        (
                                                ID_NO,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
                                                OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT
                                        )
                                        VALUES
                                        (
                                                to_number(:idNo),:serviceCode,'1',:totalDate,
                                                to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept)
                                        );
#else
		EXEC SQL	INSERT INTO wCustFuncDay
					(
						ID_NO,PHONE_NO,FUNCTION_TYPE,FUNCTION_CODE,ADD_FLAG,TOTAL_DATE,
						OP_TIME,OP_CODE,LOGIN_NO,LOGIN_ACCEPT,BELONG_CODE,ORG_CODE
					)
					VALUES
					(
						to_number(:idNo),:phoneNo,:serviceType,:serviceCode,'Y',:totalDate,
						to_date(:opTime,'YYYYMMDD HH24:MI:SS'),:opCode,:loginNo,to_number(:loginAccept),
						:belongCode,:orgCode
					);
#endif

				if(SQLCODE!=0)
				{
					pubLog_Debug(_FFL_, "_delProdService", "", "beginProduct:wCustFuncDay[%d][%s]",SQLCODE,SQLERRMSG);
					strcpy(retMsg,"记录到期增加服务变更表wCustFuncDay错误!");
					return 1417;
				}
			}
			break;
		}
		default:
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:serviceType[%s][%d][%s]", serviceType, SQLCODE,SQLERRMSG);
			sprintf(retMsg,"服务类型有错[%s]!", serviceType);
			return 1418;
		}
	}

	sprintf(dynStmt,	"INSERT INTO %s%cHis"
						"("
						"	   ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
						"	   Srv_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
						"	   USE_FLAG,UNVAilD_TIME,PERSON_FLAG,"
						"	   LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
						")"
						"SELECT"
						"	   ID_NO,PRODUCT_CODE,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,"
						"	   Srv_ORDER,BEGIN_TIME,END_TIME,MAIN_FLAG,"
						"	   USE_FLAG,UNVAilD_TIME,PERSON_FLAG,"
						"	   LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   to_number(:v1), to_date(:v2, 'YYYYMMDD HH24:MI:SS'), :v3, 'd',:v4, to_number(:v5)"
						"  FROM %s%c"
						" WHERE id_no = to_number(:v6)"
						"   AND product_code = :v7"
						"   AND login_accept= to_number(:v8)"
						"   AND service_code = :v9", srvMsgTableName,idNo[strlen(idNo)-1],srvMsgTableName, idNo[strlen(idNo)-1]);
	EXEC SQL	PREPARE predSrvMsgHis3 FROM :dynStmt;
	EXEC SQL	EXECUTE predSrvMsgHis3 USING :loginAccept, :opTime, :loginNo,:opCode,:totalDate,
					:idNo, :oldProduct,:oldAccept, :serviceCode;
	if (SQLCODE !=0)
	{
		pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dSrvMsg[%d][%s]",SQLCODE,SQLERRMSG);
		pubLog_Debug(_FFL_, "_delProdService", "", "dynStmt=[%s]",dynStmt);
		strcpy(retMsg,"记录用户产品服务历史表错误!");
		return 1419;
	}


pubLog_Debug(_FFL_, "_delProdService", "", "loginAccept=[%s],opTime=[%s],loginNo=[%s], opCode=[%s],totalDate=[%s], idNo=[%s], "
		"oldProduct=[%s], oldAccept=[%s],serviceCode=[%s]\n",loginAccept, opTime, loginNo,opCode,totalDate,
					idNo, oldProduct,oldAccept, serviceCode);
	/*	luxc 20080213
	sprintf(dynStmt,	"INSERT INTO %s%cHis"
						"("
						"	   ID_NO,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,PRICE_MEAN,SVRMEAN_ORDER,PRICE_VALUE,"
						"	   PRICE_TYPE,PRODUCT_CODE,BEGIN_TIME,END_TIME,"
						"	   LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
						")"
						"SELECT"
						"	   ID_NO,SERVICE_TYPE,SERVICE_CODE,PRICE_CODE,PRICE_MEAN,SVRMEAN_ORDER,PRICE_VALUE,"
						"	   PRICE_TYPE,PRODUCT_CODE,BEGIN_TIME,END_TIME,"
						"	   LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   to_number(:v1), to_date(:v2, 'YYYYMMDD HH24:MI:SS'), :v3, 'd',:v4, to_number(:v5)"
						"  FROM %s%c"
						" WHERE id_no = to_number(:v6)"
						"   AND product_code = :v7"
						"   AND login_accept= to_number(:v8)"
						"   AND service_code =:v9", priceTableName,idNo[strlen(idNo)-1], priceTableName,idNo[strlen(idNo)-1]);
	pubLog_Debug(_FFL_, "_delProdService", "", "pubProduct:dynStmt=[%s]",dynStmt);
	EXEC SQL	PREPARE predSrvMsgAddHis3 FROM :dynStmt;
	EXEC SQL	EXECUTE predSrvMsgAddHis3 USING :loginAccept, :opTime, :loginNo,:opCode,:totalDate,
					:idNo, :oldProduct,:oldAccept, :serviceCode;
	if (SQLCODE !=0)
	{
		pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dSrvMsg[%d][%s]",
			SQLCODE,SQLERRMSG);
		pubLog_Debug(_FFL_, "_delProdService", "", "dynStmt=[%s]",dynStmt);
		strcpy(retMsg,"记录用户产品服务价格历史表错误!");
		return 1420;
	}
	

	sprintf(dynStmt,	"INSERT INTO %s%cHis"
						"("
						"	   ID_NO, PRODUCT_CODE,SERVICE_TYPE, SERVICE_CODE, ATTR_TYPE, SERVICE_ATTR,"
						"	   ATTR_VALUE,BEGIN_TIME,END_TIME,LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   UPDATE_ACCEPT,UPDATE_TIME,UPDATE_LOGIN,UPDATE_TYPE,UPDATE_CODE,UPDATE_DATE"
						")"
						"SELECT"
						"	   ID_NO, PRODUCT_CODE,SERVICE_TYPE, SERVICE_CODE, ATTR_TYPE, SERVICE_ATTR,"
						"	   ATTR_VALUE,BEGIN_TIME,END_TIME,LOGIN_ACCEPT,LOGIN_NO,OP_CODE,OP_TIME,"
						"	   to_number(:updateAccept), to_date(:updateTime, 'YYYYMMDD HH24:MI:SS'), :updateLogin, 'a',:updateCode, to_number(:updateDate)"
						"  FROM %s%c"
						" WHERE id_no = to_number(:idNo)"
						"   AND product_code = :productCode"
						"   AND login_accept= to_number(:loginAccept)"
						"   AND service_code = :serviceCode", srvMsgAddTableName,idNo[strlen(idNo)-1],srvMsgAddTableName, idNo[strlen(idNo)-1]);
	EXEC SQL	PREPARE predSrvMsgAddHis2 FROM :dynStmt;
	EXEC SQL	EXECUTE predSrvMsgAddHis2 USING :loginAccept, :opTime, :loginNo,:opCode,:totalDate,
								:idNo,:oldProduct,:oldAccept, :serviceCode;
	if (SQLCODE !=0)
	{
		pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:%sHis[%d][%s]",
			srvMsgAddTableName,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"记录用户产品服务附加信息历史错误!");
		return 1421;
	}
	*/

  pubLog_Debug(_FFL_, "_delProdService", "", "pubProduct:effectTime=[%s],beginTime=[%s]",effectTime,beginTime);
	if (strncmp(effectTime, beginTime,17) <=0)
	{
		/*表示尚未生效的预约记录*/
		sprintf(dynStmt,	"DELETE FROM %s%c"
							" WHERE id_no = to_number(:v1)"
							"   AND product_code = :v2"
							"   AND login_accept= to_number(:v3)"
							"   AND service_code = :serviceCode", srvMsgTableName,idNo[strlen(idNo)-1]);
		EXEC SQL PREPARE predSrvMsg5 from :dynStmt;
		exec sql EXECUTE predSrvMsg5 using :idNo,:oldProduct,:oldAccept, :serviceCode;
		if (SQLCODE !=0)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dSrvMsg[%d][%s]",SQLCODE,SQLERRMSG);
			pubLog_Debug(_FFL_, "_delProdService", "", "dynStmt=[%s][%s][%s][%s][%s]",dynStmt,idNo,oldProduct,oldAccept,serviceCode);
			strcpy(retMsg,"删除用户产品服务表错误!");
			return 1422;
		}

		/*表示尚未生效的预约记录 luxc 20080213
		sprintf(dynStmt,	"DELETE FROM %s%c"
							" WHERE id_no = to_number(:v1)"
							"   AND product_code = :v2"
							"   AND login_accept= to_number(:v3)"
							"   AND service_code = :serviceCode", priceTableName,idNo[strlen(idNo)-1]);
		pubLog_Debug(_FFL_, "_delProdService", "", "pubProduct:dynStmt=[%s]",dynStmt);
		EXEC SQL PREPARE predCustPrice3 from :dynStmt;
		exec sql EXECUTE predCustPrice3 using :idNo,:oldProduct,:oldAccept, :serviceCode;
		if (SQLCODE !=0 && SQLCODE != NOTFOUND)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:%s[%d][%s]",
				priceTableName,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"删除用户产品服务价格表错误!");
			return 1423;
		}
		*/

		/*表示尚未生效的预约记录 luxc 20080213
		sprintf(dynStmt,	"DELETE FROM %s%c"
							" WHERE id_no = to_number(:idNo)"
							"   AND product_code = :productCode"
							"   AND login_accept= to_number(:loginAccept)"
							"   AND service_code = :serviceCode", srvMsgAddTableName,idNo[strlen(idNo)-1]);
		EXEC SQL PREPARE predSrvMsgAdd3 from :dynStmt;
		exec sql EXECUTE predSrvMsgAdd3 using :idNo,:oldProduct,:oldAccept, :serviceCode;
		if (SQLCODE !=0 && SQLCODE != NOTFOUND)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:%s[%d][%s]",
				srvMsgAddTableName,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"删除用户产品服务附加表错误!");
			return 1424;
		}
		*/
	}
	else
	{
		/*表示已经生效的记录*/

		sprintf(dynStmt,	"UPDATE %s%c"
							"   SET end_time=decode(sign(to_date(end_time,'YYYYMMDD HH24:MI:SS')-to_date(:v1,'YYYYMMDD HH24:MI:SS')),1,:v2,end_time),"
							"	   login_accept=to_number(:v3),"
							"	   op_code = :v4,"
							"	   op_time=to_date(:v6,'yyyymmdd hh24:mi:ss'),"
							"	   login_no=:v7"
							" WHERE id_no = to_number(:v8)"
							"   AND product_code = :v9"
							"   AND login_accept= to_number(:v10)"
							"   AND service_code = :serviceCode", srvMsgTableName,idNo[strlen(idNo)-1]);
		EXEC SQL PREPARE predSrvMsg6 from :dynStmt;
		exec sql EXECUTE predSrvMsg6 using :effectTime, :effectTime, :loginAccept, :opCode, :opTime,
					:loginNo, :idNo,:oldProduct,:oldAccept, :serviceCode;
		if (SQLCODE !=0)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct:dSrvMsg[%d][%s]",
				SQLCODE,SQLERRMSG);
			strcpy(retMsg,"更新用户产品服务表错误!");
			return 1425;
		}
		/*luxc 20080213
		sprintf(dynStmt,	"UPDATE %s%c"
							"   SET end_time=decode(sign(to_date(end_time,'YYYYMMDD HH24:MI:SS')-to_date(:v1,'YYYYMMDD HH24:MI:SS')),1,:v2,end_time),"
							"	   login_accept=to_number(:v3),"
							"	   op_code = :v4,"
							"	   op_time=to_date(:v5,'yyyymmdd hh24:mi:ss'),"
							"	   login_no=:v6"
							" WHERE id_no = to_number(:v7)"
							"   AND product_code = :v8"
							"   AND login_accept= to_number(:v9)"
							"   AND service_code = :v10", priceTableName,idNo[strlen(idNo)-1]);
		pubLog_Debug(_FFL_, "_delProdService", "", "pubProduct:dynStmt=[%s]",dynStmt);
		EXEC SQL PREPARE predCustPrice4 from :dynStmt;
		exec sql EXECUTE predCustPrice4 using :effectTime, :effectTime, :loginAccept, :opCode, :opTime,
					:loginNo, :idNo,:oldProduct,:oldAccept, :serviceCode;
		if (SQLCODE !=0 && SQLCODE != NOTFOUND)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct: [%d][%s]",SQLCODE,SQLERRMSG);
			strcpy(retMsg,"更新用户产品服务价格表错误!");
			return 1426;
		}
		*/
		/*luxc 20080213
		sprintf(dynStmt,	"UPDATE %s%c"
							"   SET end_time=decode(sign(to_date(end_time,'YYYYMMDD HH24:MI:SS')-to_date(:endTime,'YYYYMMDD HH24:MI:SS')),1,:endTime2,end_time),"
							"	   login_accept=to_number(:loginAccept),"
							"	   op_code = :opCode,"
							"	   op_time=to_date(:opTime,'yyyymmdd hh24:mi:ss'),"
							"	   login_no=:loginNo"
							" WHERE id_no = to_number(:idNo)"
							"   AND product_code = :productCode"
							"   AND login_accept= to_number(:oldLoginAccept)"
							"   AND service_code = :serviceCode", srvMsgAddTableName,idNo[strlen(idNo)-1]);
		EXEC SQL PREPARE predSrvMsgAdd4 from :dynStmt;
		exec sql EXECUTE predSrvMsgAdd4 using :effectTime, :effectTime, :loginAccept,:opCode, :opTime,
					:loginNo,:idNo,:oldProduct, :oldAccept, :serviceCode;
		if (SQLCODE !=0 && SQLCODE != NOTFOUND)
		{
			pubLog_Debug(_FFL_, "_delProdService", "", "endProduct: [%d][%s]",
				SQLCODE,SQLERRMSG);
			strcpy(retMsg,"更新用户产品服务附加表错误!");
			return 1427;
		}
		*/ 
	}
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "_delProdService", "", "_delProdService: End Execute");
#endif
	return 0;
}

/**
 *@function information
 *@name beginProduct
 *@description  功能描述
	为移动用户申请一个新的产品。
	用户申请的产品和服务放入表dSrvMsgX表，用户的个性价格信息插入表dCustPriceX表。
	计费侧的表插入dBaseFav和wBaseFavChg表。
	当申请的服务存在开关机命令时还要插入表wSndCmdDay中。
 *@author	   lugz
 *@created	  2004-8-13 13:46
 *@inparam
	o	   phoneNo 服务号码
		此参数可以为手机号码、集团用户编号、专线号码等移动公司服务号码，要求系统中同一个服务号码只能一个用户在用。
	o	   productCode	 产品代码
		此代码不能为默认的产品代码"ZZZZZZZZ"
	o	   productPrices   产品价格因子串
		只有当存在产品价格因子发生变化时，才用到此参数，否则传空值。
		此价格串的格式为:"服务代码1,价格代码1,价格因子1,因子顺序1,价格取值模式1,具体价格值1,|服务代码2,价格代码2,价格因子2,因子顺序2,价格取值模式2,具体价格值2,|..."
	o	   serviceAttr	 服务属性
		只有当存在服务属性类型时，才用到此参数，否则传空值。此时服务属性的类型一般为值"1"，表示服务归属。
		此服务属性串的格式为:"服务代码1,服务属性类型1,服务属性代码2,服务属性值2,|服务代码2,服务属性类型2,服务属性代码2,服务属性值2,|..."
	o	   effectTime	  生效时间
		为资费的总生效时间，时间格式'yyyymmdd hh24:mi:ss'
	o	   unValidTime	 失效时间
	o	   opCode  操作代码
	o	   loginNo 操作工号
	o	   loginAccept	 操作流水
	o	   opTime  操作时间
		时间格式'yyyymmdd hh24:mi:ss'
 *@outparam
	o	   retMsg  错误信息
		注意输入串定义的大小，不能小于60+1个字节。
 */
int beginProduct(
	const char *phoneNo,
	const char *productCode,
	const char *productPrices,
	const char *serviceAttr,
	const char *effectTime,
	const char *unValidTime,
	const char *opCode,
	const char *loginNo,
	const char *loginAccept,
	const char *opTime,
	char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct: Start Execute");
#endif
	EXEC SQL BEGIN DECLARE SECTION;
		/*用户信息定义*/
		char idNo[22+1],phoneServiceType[2+1], groupId[10+1],regionCode[2+1],
			 favBrand[2+1], smCode[2+1], belongCode[7+1],grpCode[7+1];
		char portCode[1+1],simNo[20+1], imsiNo[20+1];/*开机关代码，只对手机号码有效*/
		char iLoginNo[6+1];

		/*产品服务定义*/
		char serviceCode[4+1], mainFlag[1+1], timeFlag[1+1], serviceType[1+1],
			 productLevel[1+1],modeTime[1+1], productSmCode[2+1];
		int favOrder;

		TProductPrices tProductPrices[MAX_PRODUCT_PRICES];
		TServiceAttr tServiceAttr[MAX_SERVICE_ATTR_COUNT];
		int i, productPricesCount, serviceAttrCount;

		/*公用变量*/
		char totalDate[8+1], orgCode[9+1], orgId[10+1],dynStmt[1024];
		int timeCycle=0, timeUnit=0, ret=0;
		char srvMsgTableName[20+1];
		char priceTableName[20+1];
		char srvMsgAddTableName[20+1];
	EXEC SQL END   DECLARE SECTION;
	
	/*ng解耦 by yaoxc begin*/
	int  v_ret=0;
    char v_parameter_array[DLMAXITEMS][DLINTERFACEDATA];
	/*ng解耦 by yaoxc end*/
	
	/*ng解耦 by yaoxc begin*/
	init(v_parameter_array);
	/*ng解耦 by yaoxc end*/
	
	strcpy(retMsg, "确认成功!");
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input phoneNo=[%s]",phoneNo);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input productCode=[%s]",productCode);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input productPrices=[%s]",productPrices);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input serviceAttr=[%s]",serviceAttr);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input effectTime=[%s]",effectTime);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input opCode=[%s]",opCode);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input loginNo=[%s]",loginNo);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input loginAccept=[%s]",loginAccept);
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:input opTime=[%s]",opTime);
#endif

	memset(totalDate, 0, sizeof(totalDate));
	strncpy(totalDate, opTime, 8);

	productPricesCount = strlen(productPrices);

	memset(tProductPrices, 0, sizeof(tProductPrices));
	memset(tServiceAttr, 0, sizeof(tServiceAttr));
	memset(srvMsgTableName, 0, sizeof(srvMsgTableName));
	memset(priceTableName,  0, sizeof(priceTableName));

	if ((productPricesCount=_getProductMeans(productPrices, tProductPrices)) < 0)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:_getProductMeans 错误",SQLCODE,SQLERRMSG);
		sprintf(retMsg,"得到产品因子信息错误[%s]!", productCode);
		return 1500;
	}

	if ((serviceAttrCount=_getServiceAttr(serviceAttr, tServiceAttr)) < 0)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:_getServiceAttr 错误",SQLCODE,SQLERRMSG);
		sprintf(retMsg,"得到服务属性信息错误[%s]!", productCode);
		return 1501;
	}

	memset(idNo,0, sizeof(idNo));
	memset(belongCode, 0, sizeof(belongCode));
	memset(groupId, 0, sizeof(groupId));
	memset(phoneServiceType, 0, sizeof(phoneServiceType));


	if (strncmp(productCode, DEFAULT_PRODUCT_CODE, 8) == 0)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:if.productCode=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		sprintf(retMsg,"输入的产品代码不能为默认的产品代码[%s]!", productCode);
		return 1502;
	}

	EXEC SQL	SELECT to_char(id_no), group_id, '00', region_code||district_code||town_code
				  INTO :idNo, :groupId, :phoneServiceType, :belongCode
				  FROM dGrpUserMsg
				 WHERE user_no = :phoneNo
				   AND run_code < 'a';

	if(SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:dGrpUserMsgMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dGrpUserMsg错误!");
		return 1503;
	}

	strcpy(srvMsgTableName,"dGrpSrvMsg");
	strcpy(srvMsgAddTableName,"dGrpSrvMsgAdd");
	strcpy(priceTableName, "dGrpCustPrice");
	Trim(idNo);

	if (idNo[0] == '\0')
	{
		EXEC SQL	SELECT to_char(id_no), group_id, service_type, belong_code
					  INTO :idNo, :groupId, :phoneServiceType, :belongCode
					  FROM dCustMsg
					 WHERE phone_no = :phoneNo
					   AND substr(run_code,2,1) < 'a';

		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:dCustMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustMsg错误!");
			return 1504;
		}
		strcpy(srvMsgTableName,"dSrvMsg");
		strcpy(srvMsgAddTableName,"dSrvMsgAdd");
		strcpy(priceTableName, "dCustPrice");
	}
	Trim(idNo);

	/*读取用户的SM_CODE*/
	memset(smCode, 0, sizeof(smCode));
	_getProdSmCode(idNo, opTime, srvMsgTableName,smCode);

	memset(productSmCode, 0, sizeof(productSmCode));
	EXEC SQL	SELECT sm_code
			  INTO :productSmCode
			  FROM sSmProduct
			 WHERE product_code = :productCode;

	if (smCode[0] == '\0' && productSmCode[0] == '\0')
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:_getProdSmCode[%d][%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"取用户的品牌信息错误!");
		return 1505;
	}

	/*不是开户操作，且发生了品牌变更*/
	pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:smCode=[%s],productSmCode=[%s]",smCode,productSmCode);
	if (strncmp(smCode, productSmCode, 2) != 0 &&
		smCode[0] != '\0' &&
		productSmCode[0] != '\0')
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:idNo=[%s]",idNo);
		EXEC SQL	INSERT INTO dCustMsgHis
			(
				ID_NO,	CUST_ID,	CONTRACT_NO,	IDS,	PHONE_NO,SERVICE_TYPE,
				SM_CODE,	ATTR_CODE,	USER_PASSWD,	BELONG_CODE,	LIMIT_OWE,
				CREDIT_CODE,	CREDIT_VALUE,	RUN_CODE,	RUN_TIME,
				BILL_DATE,	BILL_TYPE,	ENCRYPT_PREPAY,	CMD_RIGHT,
				LAST_BILL_TYPE,	BAK_FIELD,OPEN_TIME,GROUP_ID,group_no,
				UPDATE_ACCEPT,	UPDATE_TIME,	UPDATE_LOGIN,	UPDATE_TYPE,	UPDATE_CODE, UPDATE_DATE
			)
			select
				ID_NO,	CUST_ID,	CONTRACT_NO,	IDS,	PHONE_NO,SERVICE_TYPE,
				SM_CODE,	ATTR_CODE,	USER_PASSWD,	BELONG_CODE,	LIMIT_OWE,
				CREDIT_CODE,	CREDIT_VALUE,	RUN_CODE,	RUN_TIME,
				BILL_DATE,	BILL_TYPE,	ENCRYPT_PREPAY,	CMD_RIGHT,
				LAST_BILL_TYPE,	BAK_FIELD,OPEN_TIME,GROUP_ID,group_no,
				to_number(:loginAccept),	to_date(:opTime, 'yyyymmdd hh24:mi:ss'),	:loginNo,	'u',	:opCode, to_number(:totalDate)
			  from dCustMsg
			 where id_no = to_number(:idNo);
		if (SQLCODE !=0)
		{
		pubLog_Debug(_FFL_, "beginProduct", "", "插入表dCustMsgHis::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
		sprintf(retMsg, "插入表dCustMsgHis::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
		return 1506;
		}
		/*ng解耦
		EXEC SQL	UPDATE dCustMsg
			   SET sm_code = :productSmCode
			 WHERE id_no = to_number(:idNo);
		if (SQLCODE !=0)
		{
		pubLog_Debug(_FFL_, "beginProduct", "", "更新表dCustMsg::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
		sprintf(retMsg, "更新表dCustMsg::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
		return 1507;
		}
		ng解耦*/
    	/*ng解耦 by yaoxc begin*/
    	init(v_parameter_array);
			
		strcpy(v_parameter_array[0],productSmCode);
		strcpy(v_parameter_array[1],idNo);
    	
		v_ret=OrderUpdateCustMsg(ORDERIDTYPE_USER,idNo,100,
								opCode,atol(loginAccept),loginNo,"beginProduct",
								idNo,
								" sm_code = :productSmCode ","",v_parameter_array);
		printf("OrderUpdateCustMsg ,ret=[%d]\n",v_ret);
		if(0!=v_ret)
		{
			pubLog_Debug(_FFL_, "beginProduct", "", "更新表dCustMsg::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
			sprintf(retMsg, "更新表dCustMsg::id_no=[%s]出错, SQLCODE=[%d]!", idNo, SQLCODE);
			return 1507;
		}
    	/*ng解耦 by yaoxc end*/
		strcpy(smCode, productSmCode);
	}
	else if (smCode[0] == '\0')
	{
		strcpy(smCode, productSmCode);
	}


	Trim(idNo);
	Trim(groupId);

#if PROVINCE_RUN == PROVINCE_SICHUAN
		if ( (getGroupCode(groupId, grpCode)) != 0)
		{
			pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dChnGroupMsg错误!");
			return 1508;
		}
		strncpy(regionCode,grpCode,2);
		regionCode[2] = '\0';
#elif PROVINCE_RUN == PROVINCE_SHANXI
        if ( (getGroupCode(groupId, grpCode)) != 0)
        {
                pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
                strcpy(retMsg,"查询dChnGroupMsg错误!");
                return 1508;
        }
        strncpy(regionCode,grpCode,2);
        regionCode[2] = '\0';
#else
		/* 吉林sRegionCode、sDisCode、sTownCode表都没有group_id,所以不能使用getGroupCode函数*/
		memset(iLoginNo,0,sizeof(iLoginNo));
		strncpy(iLoginNo,loginNo,6);
		iLoginNo[6]='\0';
		EXEC SQL SELECT substr(org_code,1,2) INTO :regionCode
				   FROM dLoginMsg
				  WHERE login_no=:iLoginNo;
#endif


	EXEC SQL	SELECT fav_brand
				  INTO :favBrand
				  FROM sSmCode
				 WHERE region_code = :regionCode
				   AND sm_code = :smCode;
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:sSmCode.smCode=[%s][%d][%s]",smCode,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询sSmCode错误!");
		return 1509;
	}

#if PROVINCE_RUN != PROVINCE_JILIN

	EXEC SQL	SELECT org_code,org_id
				  INTO :orgCode,:orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;		 	
#else

	EXEC SQL	SELECT org_code
				  INTO :orgCode
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
#endif
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:dLoginMsg.loginNo=[%s][%d][%s]",loginNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dLoginMsg错误!");
		return 1510;
	}
	
  /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
  ret =0;
  memset(orgId, 0, sizeof(orgId));
  ret = sGetLoginOrgid(loginNo,orgId);
  if(ret <0)
    	{
    	  pubLog_Debug(_FFL_, "beginProduct", "", "获取用户org_id失败!");
    	  return 200919; 
    	}
  Trim(orgId);
 /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

pubLog_Debug(_FFL_, "beginProduct", "", "phoneServiceType=[%s]",phoneServiceType);
	if (strncmp(phoneServiceType, "01", 2) == 0)
	{
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL	SELECT port_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#else
		EXEC SQL	SELECT area_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#endif
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:sHlrCode.regionCode=[%s]phoneNo=[%s][%d][%s]",
				regionCode,phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询sHlrCode错误!");
			return 1511;
		}

		EXEC SQL	SELECT switch_no,sim_no
					  INTO :imsiNo, :simNo
					  FROM dCustSim
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct:dCustSim.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustSim错误!");
			return 1512;
		}
	}

	if ((ret=_putProdExpire(regionCode, productCode, idNo, phoneNo,effectTime,	  opCode, loginNo,loginAccept,
				opTime, totalDate, orgCode, orgId, retMsg)) != 0)
	{
		return ret;
	}

	EXEC SQL	DECLARE curProductSrv CURSOR FOR
				SELECT a.service_code, a.main_flag, a.time_flag,a.fav_order, b.service_type,
					   c.product_level,a.mode_time,a.time_cycle, a.time_unit
				  FROM sProductSrv a, sSrvCode b, sProductCode c
				 WHERE a.service_code = b.service_code
				   AND a.product_code = c.product_code
				   AND a.product_code=:productCode;
	EXEC SQL	OPEN curProductSrv;
	/*第一层：服务有关的表*/
	for(;;)
	{
		EXEC SQL	FETCH   curProductSrv INTO :serviceCode, :mainFlag, :timeFlag,
				:favOrder, :serviceType, :productLevel, :modeTime, :timeCycle, :timeUnit;
		if (SQLCODE != 0) break;

		if ( (ret=_putProdService(phoneNo, idNo, belongCode, smCode,
									portCode, simNo, imsiNo,
									regionCode, productCode, serviceType,
									tServiceAttr, serviceAttrCount,
									tProductPrices, productPricesCount,	 1,
									serviceCode, mainFlag,  timeFlag,	   favOrder,
									productLevel, modeTime, favBrand,
									timeCycle, timeUnit,
									srvMsgTableName,srvMsgAddTableName,	 priceTableName,
									effectTime, unValidTime, loginNo,
									loginAccept, opTime, opCode, totalDate,
									groupId, orgCode, orgId, retMsg)) != 0)
		{
			EXEC SQL	CLOSE curProductSrv;
			return ret;
		}
	}
	/*第一层*/
	EXEC SQL	CLOSE curProductSrv;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "beginProduct", "", "beginProduct: End Execute");
#endif
	return 0;
}

/*
	待定问题列表：
		1、服务附加信息存放位置，暂时不在此函数方式中实现
 */
/*@function information
 *@name beginService
 *@description  功能描述
	为移动用户申请一个新的产品或服务。
	用户申请的产品和服务放入表dSrvMsgX表，用户的个性价格信息插入表dCustPriceX表。
	计费侧的表插入dBaseFav和wBaseFavChg表。
	当申请的服务存在开关机命令时还要插入表wSndCmdDay中。
 *@author	   lugz
 *@created	  2004-8-13 13:46
 *@input parameter information
 *@inparam
	o	   phoneNo 服务号码
		此参数可以为手机号码、集团用户编号、专线号码等移动公司服务号码
	o	   serviceCode	 服务代码
	o	   serviceAttr	 服务属性
		只有当存在服务属性类型时，才用到此参数，否则传空值。此时服务属性的类型一般为值"1"，表示服务归属。
		此服务属性串的格式为:"服务代码1,服务属性类型1,服务属性代码2,服务属性值2,|服务代码2,服务属性类型2,服务属性代码2,服务属性值2,|..."
	o	   effectTime	  生效时间
		为资费的总生效时间，时间格式'yyyymmdd hh24:mi:ss'
	o	   unValidTime	 失效时间
	o	   opCode  操作代码
	o	   loginNo 操作工号
	o	   loginAccept	 操作流水
	o	   opTime  操作时间
		时间格式'yyyymmdd hh24:mi:ss'
 *@outparam
	o	   retMsg  错误信息
		注意输入串定义的大小，不能小于60+1个字节。

 */
int beginService(
	const char *phoneNo,
	const char *serviceCode,
	const char *serviceAttr,
	const char *effectTime,
	const char *unValidTime,
	const char *opCode,
	const char *loginNo,
	const char *loginAccept,
	const char *opTime,
	char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "beginService", "", "beginService: Start Execute");
#endif
	char serviceAttrRecord[127+1];
	int ret;
	EXEC SQL BEGIN DECLARE SECTION;
		/*用户信息定义*/
		char idNo[22+1],phoneServiceType[2+1], groupId[10+1],regionCode[2+1], favBrand[2+1],
			 smCode[2+1], belongCode[7+1],grpCode[7+1];
		char portCode[1+1],simNo[20+1], imsiNo[20+1];/*开机关代码，只对手机号码有效*/
		char iLoginNo[6+1];

		/*产品服务定义*/
		char productCode[8+1], mainFlag[1+1], timeFlag[1+1], serviceType[1+1];
		int favOrder;

		TServiceAttr tServiceAttr[MAX_SERVICE_ATTR_COUNT];
		int i, serviceAttrCount;

		/*BOSS优惠集团部分处理*/
		char vpmnGroup[10+1];
		double mocRate, mtcRate;

		/*公用变量*/
		char totalDate[8+1], orgCode[9+1], orgId[10+1],dynStmt[1024];
		int timeCycle=0, timeUnit=0;
		char srvMsgTableName[20+1];
		char srvMsgAddTableName[20+1];
		char priceTableName[20+1];

	EXEC SQL END   DECLARE SECTION;

	strcpy(retMsg, "确认成功!");
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input phoneNo=[%s]",phoneNo);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input productCode=[%s]",productCode);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input serviceCode=[%s]",serviceCode);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input serviceAttr=[%s]",serviceAttr);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input effectTime=[%s]",effectTime);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input opCode=[%s]",opCode);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input loginNo=[%s]",loginNo);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input loginAccept=[%s]",loginAccept);
	pubLog_Debug(_FFL_, "beginService", "", "beginService:input opTime=[%s]",opTime);
#endif

	memset(totalDate, 0, sizeof(totalDate));
	strncpy(totalDate, opTime, 8);

	memset(idNo,0, sizeof(idNo));
	memset(belongCode, 0, sizeof(belongCode));
	memset(groupId, 0, sizeof(groupId));
	memset(phoneServiceType, 0, sizeof(phoneServiceType));
	memset(productCode, 0, sizeof(productCode));
	memset(srvMsgTableName, 0, sizeof(srvMsgTableName));
	memset(priceTableName,  0, sizeof(priceTableName));
	memset(tServiceAttr, 0, sizeof(tServiceAttr));

	if ((serviceAttrCount=_getServiceAttr(serviceAttr, tServiceAttr)) < 0)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginService:_getServiceAttr 错误",SQLCODE,SQLERRMSG);
		sprintf(retMsg,"得到服务属性信息错误!");
		return 1600;
	}

	strncpy(productCode, DEFAULT_PRODUCT_CODE, 8);

	memset(smCode, 0, sizeof(smCode));

	EXEC SQL	SELECT to_char(id_no), group_id, '00', region_code||district_code||town_code
				  INTO :idNo, :groupId, :phoneServiceType, :belongCode
				  FROM dGrpUserMsg
				 WHERE user_no = :phoneNo
				   AND run_code < 'a';

	if(SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginProduct:dGrpUserMsgMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dGrpUserMsg错误!");
		return 1601;
	}

	strcpy(srvMsgTableName,"dGrpSrvMsg");
	strcpy(srvMsgAddTableName,"dGrpSrvMsgAdd");
	strcpy(priceTableName, "dGrpCustPrice");
	Trim(idNo);

	if (idNo[0] == '\0')
	{
		EXEC SQL	SELECT to_char(id_no), group_id, service_type, belong_code
					  INTO :idNo, :groupId, :phoneServiceType, :belongCode
					  FROM dCustMsg
					 WHERE phone_no = :phoneNo
					   AND substr(run_code,2,1) < 'a';

		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginService", "", "beginService:dCustMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustMsg错误!");
			return 1602;
		}
		strcpy(srvMsgTableName,"dSrvMsg");
		strcpy(srvMsgAddTableName,"dSrvMsgAdd");
		strcpy(priceTableName, "dCustPrice");
	}

	Trim(idNo);

	if (_getProdSmCode(idNo, opTime, srvMsgTableName, smCode) != 0)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginService:_getProdSmCode错误[%d][%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"取用户的品牌信息错误!");
		return 1603;
	}


	Trim(idNo);
	Trim(groupId);

#if PROVINCE_RUN == PROVINCE_SICHUAN
		if ( (getGroupCode(groupId, grpCode)) != 0)
		{
			pubLog_Debug(_FFL_, "beginService", "", "beginService:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"得到应该归属的行政代码出错!");
			return 1604;
		}
		strncpy(regionCode,grpCode,2);
		regionCode[2] = '\0';
#elif PROVINCE_RUN == PROVINCE_SHANXI
        if ( (getGroupCode(groupId, grpCode)) != 0)
        {
                pubLog_Debug(_FFL_, "beginService", "", "beginService:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
                strcpy(retMsg,"得到应该归属的行政代码出错!");
                return 1604;
        }
        strncpy(regionCode,grpCode,2);
        regionCode[2] = '\0';
#else
		/* 吉林sRegionCode、sDisCode、sTownCode表都没有group_id,所以不能使用getGroupCode函数*/
		memset(iLoginNo,0,sizeof(iLoginNo));
		strncpy(iLoginNo,loginNo,6);
		iLoginNo[6]='\0';
		EXEC SQL SELECT substr(org_code,1,2) INTO :regionCode
				   FROM dLoginMsg
				  WHERE login_no=:iLoginNo;
#endif


	EXEC SQL	SELECT fav_brand
				  INTO :favBrand
				  FROM sSmCode
				 WHERE region_code = :regionCode
				   AND sm_code = :smCode;
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginService:sSmCode.smCode=[%s][%d][%s]",smCode,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询sSmCode错误!");
		return 1605;
	}

#if PROVINCE_RUN != PROVINCE_JILIN

	EXEC SQL	SELECT org_code,org_id
				  INTO :orgCode,:orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
#else
	/*---组织机构改造二期公共函数改造 给orgid赋值 edit by zhaoqm at 22/04/2009---*/
	/*EXEC SQL	SELECT org_code, 'unkonwn'
				  INTO :orgCode, :orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;*/
	/*--组织机构改造获取org_id edit by mengfy at 27/06/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(loginNo,orgId);
	if(ret <0)
	{
		strcpy(retMsg,"查询工号org_id失败!");
		pubLog_DBErr(_FFL_, "endProduct", "", "retMsg = [%s]", retMsg);
		return 1709;
	}
	Trim(orgId);
	/*--组织机构改造获取org_id edit by mengfy at 27/06/2009--end*/
	EXEC SQL	SELECT org_code
				  INTO :orgCode
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
#endif
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginService:dLoginMsg.loginNo=[%s][%d][%s]",loginNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dLoginMsg错误!");
		return 1607;
	}

	if (strncmp(phoneServiceType, "01", 2) == 0)
	{
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL	SELECT port_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#else
		EXEC SQL	SELECT area_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#endif
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginService", "", "beginService:sHlrCode.regionCode=[%s]phoneNo=[%s][%d][%s]",
				regionCode,phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询sHlrCode错误!");
			return 1608;
		}

		EXEC SQL	SELECT switch_no,sim_no
					  INTO :imsiNo, :simNo
					  FROM dCustSim
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "beginService", "", "beginService:dCustSim.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustSim错误!");
			return 1609;
		}
	}

	strcpy(mainFlag, "N");

	/*时间标志默认为绝对时间*/
	strcpy(timeFlag, "0");
	favOrder = 0;
	EXEC SQL	SELECT service_type
				  INTO :serviceType
				  FROM sSrvCode
				 WHERE service_code = :serviceCode;
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "beginService", "", "beginService:sSrvCode.service_code=[%s][%d][%s]",
			serviceCode,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询sSrvCode错误!");
		return 1610;
	}

	if ( (ret=_putProdService(phoneNo, idNo, belongCode, smCode,
								portCode, simNo, imsiNo,
								regionCode, productCode, serviceType,
								tServiceAttr, serviceAttrCount,
								NULL, 0,	0,
								serviceCode, mainFlag,  timeFlag,	   favOrder,
								"0", "N", favBrand,
								timeCycle, timeUnit,
								srvMsgTableName,srvMsgAddTableName,	 priceTableName,
								effectTime, unValidTime, loginNo,
								loginAccept, opTime, opCode, totalDate,
								groupId, orgCode, orgId, retMsg)) != 0)
	{
		return ret;
	}
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "beginService", "", "beginService: End Execute");
#endif
	return 0;
}

/*@function information
 *@name endProduct
 *@description  功能描述
	为取消用户的一个产品。
	如果是正在生效的产品，则更新它的结束时间；如果是预约尚未生效的产品则直接删除它。
 *@author	   lugz
 *@created	  2004-8-13 13:46
 *@input parameter information
 *@inparam
	o	   phoneNo 服务号码
		此参数可以为手机号码、集团用户编号、专线号码等移动公司服务号码，要求系统中同一个服务号码只能一个用户在用。
	o	   oldProduct	  原产品代码
		此代码不能为默认的产品代码"ZZZZZZZZ"
	o	   oldAccept	   原产品的操作流水
	o	   effectTime	  生效时间
		资费的结束时间，时间格式'yyyymmdd hh24:mi:ss'
	o	   opCode  操作代码
	o	   loginNo 操作工号
	o	   loginAccept	 操作流水
	o	   opTime  操作时间
		时间格式'yyyymmdd hh24:mi:ss'
 *@outparam
	o	   retMsg  错误信息
		注意输入串定义的大小，不能小于60+1个字节。

 *************************************************/

int endProduct(
	const char *phoneNo,
	const char *oldProduct,
	const char *oldAccept,
	const char *effectTime,
	const char *opCode,
	const char *loginNo,
	const char *loginAccept,
	const char *opTime,
	char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endProduct", "", "endProduct: Start Execute");
#endif
	int ret;
	EXEC SQL BEGIN DECLARE SECTION;
		/*用户信息定义*/
		char idNo[22+1],phoneServiceType[2+1], groupId[10+1],regionCode[2+1], favBrand[2+1],
			 smCode[2+1], belongCode[7+1],grpCode[7+1];
		char portCode[1+1],simNo[20+1], imsiNo[20+1];/*开机关代码，只对手机号码有效*/
		char iLoginNo[6+1];

		char serviceType[1+1], serviceCode[4+1], beginTime[17+1], endTime[17+1];
		int i;

		/*公用变量*/
		char totalDate[8+1], orgCode[9+1], orgId[10+1],dynStmt[1024];
		char srvMsgTableName[20+1];
		char priceTableName[20+1];
		char srvMsgAddTableName[20+1];
	EXEC SQL END   DECLARE SECTION;

	strcpy(retMsg, "确认成功!");
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input phoneNo=[%s]",phoneNo);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input oldProduct=[%s]",oldProduct);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input oldAccept=[%s]",oldAccept);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input effectTime=[%s]",effectTime);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input opCode=[%s]",opCode);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input loginNo=[%s]",loginNo);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input loginAccept=[%s]",loginAccept);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input opTime=[%s]",opTime);
#endif
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input phoneNo=[%s]",phoneNo);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input oldProduct=[%s]",oldProduct);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input oldAccept=[%s]",oldAccept);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input effectTime=[%s]",effectTime);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input opCode=[%s]",opCode);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input loginNo=[%s]",loginNo);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input loginAccept=[%s]",loginAccept);
	pubLog_Debug(_FFL_, "endProduct", "", "endProduct:input opTime=[%s]",opTime);

	memset(totalDate, 0, sizeof(totalDate));
	strncpy(totalDate, opTime, 8);


	memset(idNo,0, sizeof(idNo));
	memset(belongCode, 0, sizeof(belongCode));
	memset(groupId, 0, sizeof(groupId));
	memset(phoneServiceType, 0, sizeof(phoneServiceType));
	memset(srvMsgTableName, 0, sizeof(srvMsgTableName));
	memset(srvMsgAddTableName, 0, sizeof(srvMsgAddTableName));
	memset(priceTableName,  0, sizeof(priceTableName));



	if (strncmp(oldProduct, DEFAULT_PRODUCT_CODE, 8) == 0)
	{
		pubLog_Debug(_FFL_, "endProduct", "", "endProduct:if.productCode=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		sprintf(retMsg,"输入的产品代码不能为默认的产品代码[%s]!", oldProduct);
		return 1700;
	}

	EXEC SQL	SELECT to_char(id_no), group_id, '00', region_code||district_code||town_code
				  INTO :idNo, :groupId, :phoneServiceType, :belongCode
				  FROM dGrpUserMsg
				 WHERE user_no = :phoneNo
				   AND run_code < 'a';

	if(SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		pubLog_Debug(_FFL_, "endProduct", "", "beginProduct:dGrpUserMsgMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dGrpUserMsg错误!");
		return 1701;
	}

	strcpy(srvMsgTableName,"dGrpSrvMsg");
	strcpy(priceTableName, "dGrpCustPrice");
	strcpy(srvMsgAddTableName,"dGrpSrvMsgAdd");
	Trim(idNo);

	if (idNo[0] == '\0')
	{
		EXEC SQL	SELECT to_char(id_no), group_id, service_type, belong_code
					  INTO :idNo, :groupId, :phoneServiceType, :belongCode
					  FROM dCustMsg
					 WHERE phone_no = :phoneNo
					   AND substr(run_code,2,1) < 'a';

		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endProduct", "", "beginProduct:dCustMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustMsg错误!");
			return 1702;
		}
		strcpy(srvMsgTableName,"dSrvMsg");
		strcpy(priceTableName, "dCustPrice");
		strcpy(srvMsgAddTableName,"dSrvMsgAdd");
	}

	Trim(idNo);

	if (_getProdSmCode(idNo, opTime, srvMsgTableName,smCode) != 0)
	{
		pubLog_Debug(_FFL_, "endProduct", "", "endProduct:_getProdSmCode[%d][%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"取用户的品牌信息错误!");
		return 1703;
	}

	Trim(idNo);
	Trim(groupId);

#if PROVINCE_RUN == PROVINCE_SICHUAN
		if ( (getGroupCode(groupId, grpCode)) != 0)
		{
			pubLog_Debug(_FFL_, "endProduct", "", "endProduct:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"得到应该归属的行政代码出错!");
			return 1704;
		}
		strncpy(regionCode,grpCode,2);
		regionCode[2] = '\0';
#elif PROVINCE_RUN == PROVINCE_SHANXI
        if ( (getGroupCode(groupId, grpCode)) != 0)
        {
                pubLog_Debug(_FFL_, "endProduct", "", "beginService:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
                strcpy(retMsg,"得到应该归属的行政代码出错!");
                return 1604;
        }
        strncpy(regionCode,grpCode,2);
        regionCode[2] = '\0';
#else
		/* 吉林sRegionCode、sDisCode、sTownCode表都没有group_id,所以不能使用getGroupCode函数*/
		memset(iLoginNo,0,sizeof(iLoginNo));
		strncpy(iLoginNo,loginNo,6);
		iLoginNo[6]='\0';
		EXEC SQL SELECT substr(org_code,1,2) INTO :regionCode
				   FROM dLoginMsg
				  WHERE login_no=:iLoginNo;
#endif


	EXEC SQL	SELECT fav_brand
				  INTO :favBrand
				  FROM sSmCode
				 WHERE region_code = :regionCode
				   AND sm_code = :smCode;
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "endProduct", "", "endProduct:sSmCode.smCode=[%s][%d][%s]",smCode,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询sSmCode错误!");
		return 1705;
	}
/*--组织机构改造二期 org_id赋值 edit by zhaoqm at 22/04/2009--begin--*/
/*#if PROVINCE_RUN != PROVINCE_JILIN*/
	/*--组织机构改造获取org_id edit by mengfy at 27/06/2009--begin*/
	ret =0;
	ret = sGetLoginOrgid(loginNo,orgId);
	if(ret <0)
	{
		strcpy(retMsg,"查询工号org_id失败!");
		pubLog_DBErr(_FFL_, "endProduct", "", "retMsg = [%s]", retMsg);
		return 1709;
	}
	Trim(orgId);
	/*--组织机构改造获取org_id edit by mengfy at 27/06/2009--end*/

	EXEC SQL	SELECT org_code
				  INTO :orgCode
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
/*#else
	EXEC SQL	SELECT org_code, 'unkonwn'
				  INTO :orgCode, :orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
#endif*/
/*--组织机构改造二期 org_id赋值 edit by zhaoqm at 22/04/2009--end--*/
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "endProduct", "", "endProduct:dLoginMsg.loginNo=[%s][%d][%s]",loginNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dLoginMsg错误!");
		return 1706;
	}

	if (strncmp(phoneServiceType, "01", 2) == 0)
	{
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL	SELECT port_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#else
		EXEC SQL	SELECT area_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#endif
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endProduct", "", "endProduct:sHlrCode.regionCode=[%s]phoneNo=[%s][%d][%s]",
				regionCode,phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询sHlrCode错误!");
			return 1707;
		}

		EXEC SQL	SELECT switch_no,sim_no
					  INTO :imsiNo, :simNo
					  FROM dCustSim
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endProduct", "", "endProduct:dCustSim.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustSim错误!");
			return 1708;
		}
	}

	if ((ret=_delProdExpire(regionCode, oldProduct, idNo, opCode,   loginNo,loginAccept,
				opTime, totalDate, retMsg)) != 0)
	{
		return ret;
	}

	sprintf(dynStmt,	"SELECT distinct service_type, service_code, begin_time, end_time"
						"  FROM %s%c"
						" WHERE id_no=to_number(:v1)"
						"   AND product_code = :v2"
						"   AND login_accept = to_number(:v3)"
						" ORDER BY service_code", srvMsgTableName,idNo[strlen(idNo)-1]);
	pubLog_Debug(_FFL_, "endProduct", "", "dynStmt=[%s]",dynStmt);
	EXEC SQL PREPARE predSrvMsg4 from :dynStmt;
	EXEC SQL	DECLARE curdSrvMsg CURSOR for predSrvMsg4;
	EXEC SQL	OPEN curdSrvMsg USING :idNo, :oldProduct, :oldAccept;
	for(;;)
	{
		EXEC SQL	FETCH   curdSrvMsg INTO :serviceType, :serviceCode, :beginTime, :endTime;
		if (SQLCODE != 0) break;

    pubLog_Debug(_FFL_, "endProduct", "", "call _delProdService begin");
		if ((ret=_delProdService(phoneNo, idNo, belongCode, smCode, effectTime, portCode, simNo, imsiNo,
					regionCode, oldProduct, oldAccept, serviceType, serviceCode, beginTime, endTime,
					srvMsgTableName,srvMsgAddTableName,	 priceTableName,
					loginNo, loginAccept, opTime, opCode, totalDate,groupId, orgCode, orgId, retMsg)) != 0)
		{
			return ret;
		}
	}
	EXEC SQL CLoSE curdSrvMsg;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endProduct", "", "endProduct: End Execute");
#endif
	return 0;
}

/*@function information
 *@name endService
 *@description  功能描述
	为取消用户的一个产品。
	如果是正在生效的产品，则更新它的结束时间；如果是预约尚未生效的产品则直接删除它。
 *@author	   lugz
 *@created	  2004-8-13 13:46
 *@input parameter information
 *@inparam
	o	   phoneNo 服务号码
		此参数可以为手机号码、集团用户编号、专线号码等移动公司服务号码，要求系统中同一个服务号码只能一个用户在用。
	o	   oldService	  原服务代码
		此服务代码的产品代码必须为"ZZZZZZZZ"
	o	   oldAccept	   原产品的操作流水
	o	   effectTime	  生效时间
		资费的结束时间，时间格式'yyyymmdd hh24:mi:ss'
	o	   opCode  操作代码
	o	   loginNo 操作工号
	o	   loginAccept	 操作流水
	o	   opTime  操作时间
		时间格式'yyyymmdd hh24:mi:ss'
 *@outparam
	o	   retMsg  错误信息
		注意输入串定义的大小，不能小于60+1个字节。

 *************************************************/

int endService(
	const char *phoneNo,
	const char *oldService,
	const char *oldAccept,
	const char *effectTime,
	const char *opCode,
	const char *loginNo,
	const char *loginAccept,
	const char *opTime,
	char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endService", "", "endService: Start Execute");
#endif
	int ret;
	EXEC SQL BEGIN DECLARE SECTION;
		/*用户信息定义*/
		char idNo[22+1],phoneServiceType[2+1], groupId[10+1],regionCode[2+1], favBrand[2+1],
			 smCode[2+1], belongCode[7+1],grpCode[7+1];
		char portCode[1+1],simNo[20+1], imsiNo[20+1];/*开机关代码，只对手机号码有效*/
		char iLoginNo[6+1];

		/*产品服务定义*/
		char serviceType[1+1], oldProduct[8+1],beginTime[17+1], endTime[17+1];

		/*公用变量*/
		char totalDate[8+1], orgCode[9+1], orgId[10+1],dynStmt[1024];
		char srvMsgTableName[20+1];
		char priceTableName[20+1];
		char srvMsgAddTableName[20+1];
		char defaultProduct[8+1];
	EXEC SQL END   DECLARE SECTION;

	strcpy(retMsg, "确认成功!");
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "endService", "", "endService:input phoneNo=[%s]",phoneNo);
	pubLog_Debug(_FFL_, "endService", "", "endService:input oldService=[%s]",oldService);
	pubLog_Debug(_FFL_, "endService", "", "endService:input oldAccept=[%s]",oldAccept);
	pubLog_Debug(_FFL_, "endService", "", "endService:input effectTime=[%s]",effectTime);
	pubLog_Debug(_FFL_, "endService", "", "endService:input opCode=[%s]",opCode);
	pubLog_Debug(_FFL_, "endService", "", "endService:input loginNo=[%s]",loginNo);
	pubLog_Debug(_FFL_, "endService", "", "endService:input loginAccept=[%s]",loginAccept);
	pubLog_Debug(_FFL_, "endService", "", "endService:input opTime=[%s]",opTime);
#endif

	memset(totalDate, 0, sizeof(totalDate));
	strncpy(totalDate, opTime, 8);

	strcpy(defaultProduct,DEFAULT_PRODUCT_CODE);
	defaultProduct[8] = '\0';

	memset(idNo,0, sizeof(idNo));
	memset(belongCode, 0, sizeof(belongCode));
	memset(groupId, 0, sizeof(groupId));
	memset(phoneServiceType, 0, sizeof(phoneServiceType));

	memset(oldProduct, 0, sizeof(oldProduct));
	memset(srvMsgTableName, 0, sizeof(srvMsgTableName));
	memset(srvMsgAddTableName, 0, sizeof(srvMsgAddTableName));
	memset(priceTableName,  0, sizeof(priceTableName));


	EXEC SQL	SELECT to_char(id_no), group_id, '00', region_code||district_code||town_code
				  INTO :idNo, :groupId, :phoneServiceType, :belongCode
				  FROM dGrpUserMsg
				 WHERE user_no = :phoneNo
				   AND run_code < 'a';

	if(SQLCODE != 0 && SQLCODE != NOTFOUND)
	{
		pubLog_Debug(_FFL_, "endService", "", "beginProduct:dGrpUserMsgMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dGrpUserMsg错误!");
		return 1800;
	}

	strcpy(srvMsgTableName,"dGrpSrvMsg");
	strcpy(srvMsgAddTableName,"dGrpSrvMsgAdd");
	strcpy(priceTableName, "dGrpCustPrice");
	Trim(idNo);

	if (idNo[0] == '\0')
	{
		EXEC SQL	SELECT to_char(id_no), group_id, service_type, belong_code
					  INTO :idNo, :groupId, :phoneServiceType, :belongCode
					  FROM dCustMsg
					 WHERE phone_no = :phoneNo
					   AND substr(run_code,2,1) < 'a';

		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endService", "", "beginProduct:dCustMsg.phoneNo=[%s][%d][%s]",phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustMsg错误!");
			return 1801;
		}
		strcpy(srvMsgTableName,"dSrvMsg");
		strcpy(srvMsgAddTableName,"dSrvMsgAdd");
		strcpy(priceTableName, "dCustPrice");
	}

	Trim(idNo);

	/*读取用户的SM_CODE*/
	if (_getProdSmCode(idNo, opTime, srvMsgTableName,smCode) != 0)
	{
		pubLog_Debug(_FFL_, "endService", "", "endService:_getProdSmCode[%d][%s]",SQLCODE,SQLERRMSG);
		strcpy(retMsg,"取用户的品牌信息错误!");
		return 1802;
	}

	Trim(groupId);

#if PROVINCE_RUN == PROVINCE_SICHUAN
		if ( (getGroupCode(groupId, grpCode)) != 0)
		{
			pubLog_Debug(_FFL_, "endService", "", "endService:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"得到应该归属的行政代码出错!");
			return 1803;
		}
		strncpy(regionCode,grpCode,2);
		regionCode[2] = '\0';
#elif PROVINCE_RUN == PROVINCE_SHANXI
        if ( (getGroupCode(groupId, grpCode)) != 0)
        {
                pubLog_Debug(_FFL_, "endService", "", "endService:得到应该归属的行政代码出错.groupId=[%s][%d][%s]",groupId,SQLCODE,SQLERRMSG);
                strcpy(retMsg,"得到应该归属的行政代码出错!");
                return 1803;
        }
        strncpy(regionCode,grpCode,2);
        regionCode[2] = '\0';
#else
		/* 吉林sRegionCode、sDisCode、sTownCode表都没有group_id,所以不能使用getGroupCode函数*/
		memset(iLoginNo,0,sizeof(iLoginNo));
		strncpy(iLoginNo,loginNo,6);
		iLoginNo[6]='\0';
		EXEC SQL SELECT substr(org_code,1,2) INTO :regionCode
				   FROM dLoginMsg
				  WHERE login_no=:iLoginNo;
#endif


	EXEC SQL	SELECT fav_brand
				  INTO :favBrand
				  FROM sSmCode
				 WHERE region_code = :regionCode
				   AND sm_code = :smCode;
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "endService", "", "endService:sSmCode.smCode=[%s][%d][%s]",smCode,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询sSmCode错误!");
		return 1804;
	}

	Trim(idNo);
	Trim(groupId);
/*--组织机构改造二期 org_id 赋值 edit by zhaoqm at 22/04/2009--begin-*/
/*if PROVINCE_RUN != PROVINCE_JILIN*/
	EXEC SQL	SELECT org_code, org_id
				  INTO :orgCode, :orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
/*#else
	EXEC SQL	SELECT org_code, 'unkonwn'
				  INTO :orgCode, :orgId
				  FROM dLoginMsg
				 WHERE login_no= :loginNo;
#endif*/
/*--组织机构改造二期 org_id 赋值 edit by zhaoqm at 22/04/2009--begin-*/
	if(SQLCODE!=0)
	{
		pubLog_Debug(_FFL_, "endService", "", "endService:dLoginMsg.loginNo=[%s][%d][%s]",loginNo,SQLCODE,SQLERRMSG);
		strcpy(retMsg,"查询dLoginMsg错误!");
		return 1805;
	}

	if (strncmp(phoneServiceType, "01", 2) == 0)
	{
#if PROVINCE_RUN != PROVINCE_JILIN
		EXEC SQL	SELECT port_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#else
		EXEC SQL	SELECT area_code
					  INTO :portCode
					  FROM sHlrCode
					 WHERE phoneno_head=substr(:phoneNo,1,7);
#endif
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endService", "", "endService:sHlrCode.regionCode=[%s]phoneNo=[%s][%d][%s]",
				regionCode,phoneNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询sHlrCode错误!");
			return 1806;
		}

		EXEC SQL	SELECT switch_no,sim_no
					  INTO :imsiNo, :simNo
					  FROM dCustSim
					 WHERE id_no = to_number(:idNo);
		if(SQLCODE!=0)
		{
			pubLog_Debug(_FFL_, "endService", "", "endService:dCustSim.idNo=[%s][%d][%s]",
				idNo,SQLCODE,SQLERRMSG);
			strcpy(retMsg,"查询dCustSim错误!");
			return 1807;
		}
	}


	memset(serviceType, 0, sizeof(serviceType));
	memset(beginTime, 0, sizeof(beginTime));
	memset(endTime, 0, sizeof(endTime));
	memset(oldProduct, 0, sizeof(oldProduct));
	strcpy(oldProduct, DEFAULT_PRODUCT_CODE);
	sprintf(dynStmt,	"SELECT distinct service_type, begin_time, end_time"
						"  FROM %s%c"
						" WHERE id_no=to_number(:v1)"
						"   AND product_code = :v2"
						"   AND login_accept = to_number(:v3)"
						"   AND service_code = :oldService", srvMsgTableName,idNo[strlen(idNo)-1]);
	EXEC SQL EXECUTE
	BEGIN
	EXECUTE IMMEDIATE :dynStmt INTO :serviceType, :beginTime, :endTime
				USING :idNo, :oldProduct, :oldAccept, :oldService;
	END;
	END-EXEC;

	if (SQLCODE != 0)
	{
		  pubLog_Debug(_FFL_, "endService", "", "%s", dynStmt);
	pubLog_Debug(_FFL_, "endService", "", "endService:=[%s][%s][%d][%s]",
		idNo,serviceType,SQLCODE,SQLERRMSG);
	sprintf(retMsg,"查询%s错误!", srvMsgTableName);
	return 1808;
	}

  pubLog_Debug(_FFL_, "endService", "", "call  _delProdService begin");
	if ((ret=_delProdService(phoneNo, idNo, belongCode, smCode, effectTime, portCode, simNo, imsiNo,
				regionCode, oldProduct, oldAccept, serviceType, oldService, beginTime, endTime,
				srvMsgTableName,srvMsgAddTableName,	 priceTableName,
				loginNo, loginAccept, opTime, opCode, totalDate,groupId, orgCode, orgId, retMsg)) != 0)
	{
		return ret;
	}
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endService", "", "endService: End Execute");
#endif
	return 0;
}


/**
 *@function information
 *@name cancelAllBill
 *@description  功能描述
	取消一个用户的所有资费
 *@author	   lugz
 *@created	  2004-8-13 13:46
 */
int cancelAllBill(const char *idNo,  const char *userNo,
	 const char *opTime,  const char *opCode,
	 const char *loginNo,  const char *loginAccept, const char *srvMsgTableName,
	 char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "cancelAllBill", "", "cancelAllBill: Start Execute");
#endif
	EXEC SQL BEGIN DECLARE SECTION;
		char dynStmt[2048];		  /* sql字符串	 */
		char productCode[8+1];		/* 产品代码	  */
		char serviceCode[4+1];		/* 服务代码	  */
		char productLevel[1+1];       /* 主产品标志 */
		int  ret = 0 ;		  /* 函数返回值	*/
		char oldAccept[22+1];	   /* 要取消套餐的申请流水 */
	EXEC SQL END DECLARE SECTION;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "cancelAllBill", "", "idNo	=[%s]",	  idNo);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "userNo	  =[%s]",	  userNo);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "opTime	  =[%s]",	  opTime);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "opCode	  =[%s]",	  opCode);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "loginNo	 =[%s]",	  loginNo);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "loginAccept =[%s]",	  loginAccept);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "srvMsgTableName=[%s]",   srvMsgTableName);
#endif

	/* 取消服务 */
	sprintf(dynStmt,	"select distinct service_code,login_accept"
						"  from %s%c"
						" where id_no=to_number(:idNo)"
						"   and product_code = '%s'"
						"   and end_time > to_char(sysdate, 'YYYYMMDD HH24:MI:SS')",
						srvMsgTableName,idNo[strlen(idNo)-1], DEFAULT_PRODUCT_CODE);
	EXEC SQL PREPARE sel_dSrvMsg2 from :dynStmt;
	EXEC SQL declare cur_dSrvMsg2 cursor for sel_dSrvMsg2;
	EXEC SQL OPEN cur_dSrvMsg2 using :idNo;
	for(;;)
	{
		EXEC SQL	FETCH cur_dSrvMsg2 INTO :serviceCode, :oldAccept;
		if (SQLCODE != 0) break;
		Trim(oldAccept);
		ret = endService(userNo,serviceCode,oldAccept,opTime,
					opCode,loginNo,loginAccept,opTime,retMsg);
		if (ret !=0)
		{
			pubLog_Debug(_FFL_, "cancelAllBill", "", "call  endService ret = [%d]",ret);
			pubLog_Debug(_FFL_, "cancelAllBill", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
			EXEC SQL CLOSE cur_dSrvMsg2;
			return ret;
		}
	}
	EXEC SQL CLOSE cur_dSrvMsg2;

	/* 取消产品 */
	sprintf(dynStmt,	"select distinct product_code,login_accept,product_level"
						"  from %s%c"
						" where id_no=to_number(:idNo)"
						"   and product_code != '%s'"
						"   and end_time > to_char(sysdate, 'YYYYMMDD HH24:MI:SS')"
						"  order by product_level desc",
						srvMsgTableName,idNo[strlen(idNo)-1], DEFAULT_PRODUCT_CODE);
	pubLog_Debug(_FFL_, "cancelAllBill", "", "dynStmt=[%s]",dynStmt);
	EXEC SQL PREPARE sel_dSrvMsg1 from :dynStmt;
	EXEC SQL declare cur_dSrvMsg1 cursor for sel_dSrvMsg1;
	EXEC SQL OPEN cur_dSrvMsg1 using :idNo;
	for(;;)
	{
		EXEC SQL	FETCH cur_dSrvMsg1 INTO :productCode, :oldAccept, :productLevel;
		if (SQLCODE != 0) break;
		Trim(oldAccept);
		ret = endProduct(userNo,productCode,oldAccept,opTime,
					opCode,loginNo,loginAccept,opTime,retMsg);
		if (ret !=0)
		{
			pubLog_Debug(_FFL_, "cancelAllBill", "", "call  endProduct ret = [%d]",ret);
			pubLog_Debug(_FFL_, "cancelAllBill", "", "SQLCODE = [%d],SQLERRMSG = [%s]",SQLCODE,SQLERRMSG);
			EXEC SQL CLOSE cur_dSrvMsg1;
			return ret;
		}
	}
	EXEC SQL CLOSE cur_dSrvMsg1;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "cancelAllBill", "", "cancelAllBill: End Execute");
#endif
	return 0 ;

}

/**
 *@function information
 *@name getProduct
 *@description  功能描述
  取出用户所有已经生效和未生效的产品数据。当存在两个相同的品牌服务时，取最后的一个。
 *@author	   lugz
 *@created	  2004-12-6 10:49
 *@inparam
	o	   productPos 产品起始位置，取回三列
			顺序为：产品类型、产品代码和产品名称
	o	   servicePos 服务起始位置，取回六列
			顺序为：产品类型、产品代码、产品名称、服务代码、价格代码和服务名称
	o	   acceptPos 流水起始位置，取回三列
			顺序为：产品代码、服务代码和流水代码。
 */
int getProduct(FBFR32  *transIN, FBFR32 **transOUT, char *tuxServiceName,
	int output_par_num, char *dbLabel, char *connectType,
	const char *idNo, int productPos, int servicePos, int acceptPos, char *retMsg)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProduct", "", "getProduct: Start Execute");
#endif
	char dynStmt[1024];
	char preProductCode[8+1], productCode[8+1], productName[20+1], productLevel[1+1], serviceCode[4+1],
	priceCode[4+1], serviceName[20+1], loginAccept[14+1];
	int mainTimes;

	sprintf(dynStmt,	"SELECT a.product_code, b.product_name, a.product_level,"
						"	   a.service_code,a.price_code,c.service_name, a.login_accept"
						"  FROM dSrvMsg%c a, sProductCode b, sSrvCode c"
						" WHERE a.product_code = b.product_code"
						"   AND a.service_code = c.service_code"
						"   AND id_no = :v1"
						"   AND a.end_time >= '%s'"
						" ORDER BY a.product_code",
						idNo[strlen(idNo)-1], MAXENDTIME);
/*
	sprintf(dynStmt,	"SELECT a.product_code, b.product_name, a.product_level,"
						"       a.service_code,a.price_code,c.service_name, a.login_accept"
						"  FROM dSrvMsg%c a, sProductCode b, sSrvCode c"
						" WHERE a.product_code = b.product_code"
						"   AND a.service_code = c.service_code"
						"   AND id_no = :v1"
						"   AND a.end_time > to_char(sysdate, 'YYYYMMDD HH24:MI:SS') "
						"HAVING max(a.login_accept) = a.login_accept"
						" GROUP BY a.product_code, b.product_name, a.product_level,"
						"       a.service_code,a.price_code,c.service_name, a.login_accept"
						" ORDER BY a.product_code, a.login_accept",
						idNo[strlen(idNo)-1], MAXENDTIME);
*/
	EXEC SQL PREPARE predSrvMsg3240 from :dynStmt;
	EXEC SQL DECLARE curdSrvMsg3240 CURSOR FOR predSrvMsg3240;

	mainTimes=0;
	memset(preProductCode, 0, sizeof(preProductCode));
	EXEC SQL OPEN curdSrvMsg3240 USING :idNo;
	while(1)
	{
		EXEC SQL	FETCH curdSrvMsg3240	INTO :productCode, :productName, :productLevel,
				:serviceCode, :priceCode, :serviceName, :loginAccept;
		if (SQLCODE != 0 ) break;

		Trim(productLevel);
		Trim(productName);
		Trim(serviceName);
		Trim(loginAccept);

		/*取出产品代码*/
		if (strncmp(preProductCode, productCode, 8) != 0 && productLevel[0] != '0')
		{
			if (productLevel[0] == '1')
			{
				*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
					tuxServiceName,GPARM32_0+productPos,"主",dbLabel,connectType);
				mainTimes++;
			}
			else
			{
				*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
					tuxServiceName,GPARM32_0+productPos,"可",dbLabel,connectType);
			}

			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_1+productPos,productCode,dbLabel,connectType);
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_2+productPos,productName,dbLabel,connectType);

			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_0+acceptPos,productCode,dbLabel,connectType);
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_1+acceptPos,serviceCode,dbLabel,connectType);
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_2+acceptPos,loginAccept,dbLabel,connectType);

			strncpy(preProductCode, productCode, 8);
		}
		if (productLevel[0] == '0')
		{
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_0+servicePos,"服",dbLabel,connectType);

			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_0+acceptPos,productCode,dbLabel,connectType);
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_1+acceptPos,serviceCode,dbLabel,connectType);
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_2+acceptPos,loginAccept,dbLabel,connectType);
		}
		else if (productLevel[0] == '1')
		{
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_0+servicePos,"主",dbLabel,connectType);
		}
		else
		{
			*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_0+servicePos,"可",dbLabel,connectType);
		}
		*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_1+servicePos,productCode,dbLabel,connectType);
		*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_2+servicePos,productName,dbLabel,connectType);
		*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_3+servicePos,serviceCode,dbLabel,connectType);
		*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_4+servicePos,priceCode,dbLabel,connectType);
		*transOUT=add_value32(transIN,*transOUT,ONCE_ADD_LINES*output_par_num*50,
			tuxServiceName,GPARM32_5+servicePos,serviceName,dbLabel,connectType);
	}
	EXEC SQL CLOSE curdSrvMsg3240;
/*
	if (mainTimes != 1)
	{
		sprintf(retMsg, "取主产品出错!");
		return 1000;
	}
*/
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getProduct", "", "getProduct: End Execute");
#endif

	return 0;
}


/*
 * getCancelProduct  根据主产品提取绑定取消可选产品函数；
 *
 * 功能:根据主产品提取绑定取消可选产品函数；
 *
 * input 参数1: PRODUCT *main_product,被取消主产品代码结构体
 *
 * input 参数2: NODE *node,    全部节点结构体
 *
 * input 参数3: *node_list,    父节点组, 长度1024字节；
 *
 * input 参数4: *owner_code,   用户属性, 长度2个字节；
 *
 * input 参数5: *class_code,   营业厅类型,长度2个字节；
 *
 * input 参数6: *credit_code,  信用约束链表；
 *
 * input 参数7: *business_code, 业务代码
 *                              0:开户，1:添加,2:变更
 *
 * input 参数8: power_right,   营业员权限，int
 *
 *
 *
 * output参数1: PRODUCT *cancel_product，产品结构体,维数MAX_ROW；
 *
 *
 *
 * 返回参数:           <0，执行失败；
 *                      0，执行成功;
 *                      1，结构体overflow,全部返回；
 *
 */

int getCancelProduct(PRODUCT *main_product,NODE *node, char *node_list,char *owner_code,char *class_code, char *credit_code,char *business_code,int power_right,PRODUCT *cancel_product)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: Start Execute");
#endif

     /*
      * 传递变量 commented by lab 2004.11.02
      */

     char vMain_Product[8+1];            /* 主产品代码       */
     char vNode_List[NODE_LIST+1];       /* 父节点链表       */
     char vOwner_Code[2+1];              /* 机主属性         */
     char vClass_Code[2+1];              /* 营业厅类型       */
     char vCredit_Code[2+1];             /* 信用约束链表     */
     int  vPower_Right;                  /* 营业员权限       */
     char vBusiness_Code[1+1];           /* 业务代码         */


     /*
      *  内部变量 commented by lab 2004.11.02
      */

     char vBusiness_Code_Tmp[20+1];       /* 业务代码Tmp      */
     char vRule_Status_Tmp[1+1];         /* 规则状态Tmp      */
     int  vChoiced_Num_Tmp;              /* 取消规则Tmp      *
                                          * 0: 自由选择      *
                                          * 2: 绑定取消      */

     char vSend_Flag_Tmp[1+1];           /* 生效方式         *
                                          * 0: 实时生效      *
                                          * 1: 预约生效      */
     char vSQL_Text[1024+1];             /* SQL语句          */
     char vTmp_String[NODE_LIST+1];      /* 临时变量         */
     int  vret_code;                     /* 返回值           */
     int  i=0;                           /* 计数器           */
     int  max=0;                         /* PRODUCT结构体    *
                                          * 已使用最大行数   */
     /*
      *  产品结构体临时变量，担任传递作用  commented by lab 2004.11.02
      */

     char  vSub_Product[8+1];            /* 可选产品代码     */
     char  vExpire_Flag[1+1];            /* 主产品生效方式   *
                                          * 0: 实时生效      *
                                          * 1: 预约生效      */
     char  vNext_Month[17+1];            /* 下月1日零时      */

     /*
      *  init parameter of begin
      *
      *  commented by lab 2004.11.02
      */

    memset(vMain_Product,       0, sizeof(vMain_Product));
    memset(vNode_List,          0, sizeof(vNode_List));
    memset(vOwner_Code,         0, sizeof(vOwner_Code));
    memset(vClass_Code,         0, sizeof(vClass_Code));
    memset(vCredit_Code,        0, sizeof(vCredit_Code));
    memset(&vPower_Right,       0, sizeof(vPower_Right));
    memset(vBusiness_Code,      0, sizeof(vBusiness_Code));


    memset(vBusiness_Code_Tmp,  0, sizeof(vBusiness_Code_Tmp));
    memset(vRule_Status_Tmp,    0, sizeof(vRule_Status_Tmp));
    memset(&vChoiced_Num_Tmp,   0, sizeof(vChoiced_Num_Tmp));
    memset(vSend_Flag_Tmp,      0, sizeof(vSend_Flag_Tmp));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));
    vret_code                =  0;

    memset(vSub_Product,        0, sizeof(vSub_Product));
    memset(vExpire_Flag,        0, sizeof(vExpire_Flag));
    memset(vNext_Month,         0, sizeof(vNext_Month));

    /*
     *  init parameter of end
     *
     *  commented by lab 2004.11.02
     */

    /*
     *  将主产品传递变量赋值到内部变量中；
     */

    strncpy(vMain_Product,       main_product->vProduct_Code, strlen(main_product->vProduct_Code));
    vMain_Product[8] = '\0';

    strncpy(vNode_List,          node_list,    strlen(node_list));
    vNode_List[NODE_LIST] = '\0';

    strncpy(vOwner_Code,         owner_code,   strlen(owner_code));
    vOwner_Code[2] = '\0';

    strncpy(vClass_Code,         class_code,   strlen(class_code));
    vOwner_Code[2] = '\0';

    strncpy(vCredit_Code,        credit_code,  strlen(credit_code));
    vCredit_Code[2] = '\0';

    vPower_Right              =  power_right;

    strncpy(vBusiness_Code,      business_code,  strlen(business_code));
    vBusiness_Code[1] = '\0';


#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t主产品代码=[%s]",   vMain_Product);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t父节点列表=[%s]",   vNode_List);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t机主属性代码=[%s]", vOwner_Code);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t营业厅类型=[%s]",   vClass_Code);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t信用约束代码=[%s]", vCredit_Code);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t营业员权限=[%d]",   vPower_Right);
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\t业务代码=[%s]",     vBusiness_Code);
#endif

    /*
     *  提取下月1日零时
     *
     *  commented by lab 2005.5.18
     */

     vret_code = getNextMonth(vNext_Month);

     if (vret_code != 0)
     {
         pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:提取下月1日零时失败!");
         pubLog_Debug(_FFL_, "getCancelProduct", "", "getNextMonth: vret_code＝[%d]",vret_code);
         return -2;
     }

    /*
     *  检查新选择的主产品的生效时间是否预约生效
     *  若预约生效，则vExpire_Flag = '1',否则vExpire_Flag = '0';
     *
     *  commented by lab 2005.5.18
     */

     if   ((strncmp(main_product->vOld_Sel,    "Y",          1) == 0)
        && (strncmp(main_product->vNew_Sel,    "n",          1) == 0)
        && (strncmp(main_product->vEnd_Time,   vNext_Month, 17) == 0))
     {
         strncpy(vExpire_Flag, "1", 1);
         vExpire_Flag[1]     = '\0';
     }
     else
     {
         strncpy(vExpire_Flag, "0", 1);
         vExpire_Flag[1]     = '\0';
     }

    /*
     *  检索出PRODUCT结构体当前已存储数据的行数，并返回
     *  及相关信息，然后追加到PRODUCT结构体中；
     */

     max = getMaxLineOfProduct(cancel_product);

     if (max < 0)
     {
         pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: 提取产品结构体使用行数失败");
         pubLog_Debug(_FFL_, "getCancelProduct", "", "getMaxLineOfProduct: vret_code＝[%d]",max);
         return -1;
     }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct:\tmax=[%d]",max);
#endif


    /*
     *  从主产品绑定取消可选产品表中sProductCancel表中提取可选产品
     *  及相关信息，然后存放到PRODUCT结构体中；
     */

    for (i=0;((i<max) && (strncmp((cancel_product+i) -> vEnd_Time, MAXENDTIME, 17) == 0));i++)
    {
         memset(vSub_Product,    0, sizeof(vSub_Product));

         strncpy(vSub_Product,   (cancel_product+i) -> vProduct_Code, strlen((cancel_product+i) -> vProduct_Code));
         vSub_Product[strlen((cancel_product+i) -> vProduct_Code)]  = '\0';

         strcpy(vSQL_Text,"\0");

         strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
         sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num ",vTmp_String);

         strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
         sprintf(vSQL_Text,"%s FROM sProductCancel ",vTmp_String);

         strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
         sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vMain_Product);

         strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
         sprintf(vSQL_Text,"%s AND   Object_Product = trim('%s') ",vTmp_String,vSub_Product);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: debug msg:\tvSQL_Text=[%s]",vSQL_Text);
#endif

         strcpy(vSQL_Text,"\0");
         strcat(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num ");
         strcat(vSQL_Text," FROM sProductCancel ");
         strcat(vSQL_Text," WHERE Product_Code   = trim(:v1) ");
         strcat(vSQL_Text," AND   Object_Product = trim(:v2) ");

         EXEC SQL PREPARE Prepare_sProductCancel2 FROM :vSQL_Text;
         EXEC SQL DECLARE Cursor_sProductCancel2 CURSOR FOR Prepare_sProductCancel2;
         EXEC SQL OPEN Cursor_sProductCancel2 using :vMain_Product,:vSub_Product;
         for(;;)
         {
                 EXEC SQL FETCH Cursor_sProductCancel2
                          INTO :vBusiness_Code_Tmp,:vRule_Status_Tmp,
                               :vSend_Flag_Tmp,    :vChoiced_Num_Tmp;

               if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
               {
                     pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: 读sProductCancel可选产品取消表失败!");
                     EXEC SQL CLOSE Cursor_sProductCancel2;
                     return -1;
               }

               if (SQLCODE == NOTFOUND) break;

               if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)  continue;
               if (strncmp(vRule_Status_Tmp,  "Y",         1)    != 0)  continue;
               if (vChoiced_Num_Tmp != 2)                               continue;

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: debug msg:\t可选产品代码[%d]=[%s]",i,vSub_Product);
#endif

               strncpy((cancel_product+i) -> vOld_Sel,        main_product->vOld_Sel, strlen(main_product->vOld_Sel));

               if (strncmp(main_product->vNew_Sel,   "n",  1) == 0)
                   strncpy((cancel_product+i) -> vNew_Sel,        main_product->vNew_Sel, strlen(main_product->vNew_Sel));
               else
                   strncpy((cancel_product+i) -> vNew_Sel,        "n",                    1);

               if ((strncmp(vSend_Flag_Tmp,"0",1) == 0) && (strncmp(vExpire_Flag,"0",1) == 0))
               {
                    strncpy((cancel_product+i) -> vEnd_Time,   "sysdate",      7);
                    (cancel_product+i) -> vEnd_Time[7]           = '\0';
               }
               else
               {
                    strncpy((cancel_product+i)->vEnd_Time,      vNext_Month,   17);
                    (cancel_product+i)->vEnd_Time[17]           = '\0';
               }

         }
         EXEC SQL CLOSE Cursor_sProductCancel2;
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getCancelProduct", "", "getCancelProduct: End Execute");
#endif
    return 0;
}


/*
 * getSrvModeTime  提取服务相对时间的计算方法
 *
 * 功能: 提取服务相对时间的计算方法
 *
 * input参数1: *service_code，服务代码，长度4个字节；
 *
 * output参数2: *mode_time，  服务周期标志，长度1个字节；
 *
 * output参数3: *time_flag，  服务时间标志，长度1个字节；
 *
 * output参数4: *time_cycle， 时间周期数，  整型
 *
 * output参数5: *time_unit，  时间周期单位，整型；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 服务周期标标志为空；
 *                      2, 服务时间标标志为空；
 *                      3, 时间周期配置无效,应大于0；
 *                      4, 时间周期单位配置错误,非0,1,2；
 *
 *
 */

int getSrvModeTime(char *service_code,char *mode_time, char *time_flag, int *time_cycle, int *time_unit)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvModeTime", "", "getSrvModeTime: Start Execute");
#endif
    char vService_Code[4+1];       /* 服务代码               */
    char vMode_Time[1+1];          /* 服务周期标志           *
                                      * Y: 自动计算产品周期  *
                                      * N: 不自动计算        */
    char vTime_Flag[1+1];          /* 服务时间标志           *
                                    * 0: 绝对时间            *
                                    * 1: 相对自然月开始时间  *
                                    * 2: 相对自然月结束时间  *
                                    * 3: 相对帐务月结束时间  */
    int  vTime_Cycle;              /* 服务周期               */
    int  vTime_Unit;               /* 服务周期单位           *
                                    * 0: 天                  *
                                    * 1: 月                  *
                                    * 2: 年                  */
    char vSQL_Text[1024+1];        /* SQL语句                */
    char vTmp_String[1024+1];      /* 临时变量               */

    memset(vService_Code,       0, sizeof(vService_Code));
    memset(vMode_Time,          0, sizeof(vMode_Time));
    memset(vTime_Flag,          0, sizeof(vTime_Flag));
    memset(&vTime_Cycle,        0, sizeof(vTime_Cycle));
    memset(&vTime_Unit,         0, sizeof(vTime_Unit));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));
    memset(vTmp_String,         0, sizeof(vTmp_String));

    strncpy(vService_Code,       service_code, strlen(service_code));
    vService_Code[4] = '\0';

    strcpy(vSQL_Text,"\0");

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s SELECT Trim(Mode_Time), Trim(Time_Flag), Time_Cycle, Time_Unit FROM sSrvCode ",vTmp_String);

    strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
    sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getSrvModeTime", "", "getSrvModeTime:vSQL_Text = [%s]",vSQL_Text);
#endif

    strcpy(vSQL_Text,"\0");
    strcat(vSQL_Text," SELECT Trim(Mode_Time),Trim(Time_Flag), Time_Cycle, Time_Unit FROM sSrvCode ");
    strcat(vSQL_Text," WHERE Service_Code = trim(:v1) ");

    EXEC SQL PREPARE Prepare_sSrvCode3 FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_sSrvCode3 CURSOR FOR Prepare_sSrvCode3;
    EXEC SQL OPEN Cursor_sSrvCode3 using :vService_Code;
    for(;;)
    {
          EXEC SQL FETCH Cursor_sSrvCode3
                   INTO :vMode_Time,  :vTime_Flag,
                        :vTime_Cycle, :vTime_Unit;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getSrvModeTime", "", "getSrvModeTime:读sSrvCode服务字典表失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_sSrvCode3;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_sSrvCode3;

    /*
     * 若服务周期标志为空，则提示返回错误;
     *
     * Commented by lab 2005.05.21
     */

    if (strlen(rtrim(vMode_Time)) <=0 )
       return 1;
    else
    {
    	 if (mode_time != NULL)
    	 {
             strncpy(mode_time, vMode_Time, strlen(vMode_Time));
             mode_time[strlen(vMode_Time)]='\0';
         }
    }

    /*
     * 若服务时间标志为空，则提示返回错误;
     *
     * Commented by lab 2005.05.21
     */

    if (strlen(rtrim(vTime_Flag)) <=0 )
       return 2;
    else
    {
    	 if (time_flag != NULL)
    	 {
             strncpy(time_flag, vTime_Flag, strlen(vTime_Flag));
             time_flag[strlen(vTime_Flag)]='\0';
         }
    }

    /*
     * 若时间周期数为小于零，则提示返回错误;
     *
     * Commented by lab 2005.05.21
     */

    if (vTime_Cycle < 0 )
       return 3;
    else
    {
    	 if (time_cycle != NULL) *time_cycle  = vTime_Cycle;
    }

    /*
     * 若时间周期单位不是0,1,2，则提示返回错误;
     *
     * Commented by lab 2005.05.21
     */

    if (vTime_Unit < 0 || vTime_Unit > 2)
       return 4;
    else
    {
    	 if (time_unit != NULL) *time_unit  = vTime_Unit;
    }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSrvModeTime", "", "getSrvModeTime: End Execute");
#endif
    return 0;

}


/*
 * getNextMonth  提取下月1日零时
 *
 * 功能: 提取下月1日零时
 *
 * output参数1: *next_month,  长度17个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 数据为空;
 *
 *
 */

int getNextMonth(char *next_month)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getNextMonth", "", "getNextMonth: Start Execute");
#endif
    char vNext_Month[17+1];        /* 系统时间     */
    char vSQL_Text[1024+1];        /* SQL语句      */

    memset(vNext_Month,         0, sizeof(vNext_Month));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));

    strcpy(vSQL_Text,"\0");
    strcpy(vSQL_Text,"SELECT To_Char(Trunc(Last_Day(SysDate)+1),'YYYYMMDD HH24:MI:SS') FROM Dual");
#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getNextMonth", "", "getNextMonth:vSQL_Text = [%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_NextMonth FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_NextMonth CURSOR FOR Prepare_NextMonth;
    EXEC SQL OPEN Cursor_NextMonth;
    for(;;)
    {
          EXEC SQL FETCH Cursor_NextMonth INTO :vNext_Month;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getNextMonth", "", "提取下月1日零时失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_NextMonth;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_NextMonth;

    /*
     * 若系统时间为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vNext_Month)) <=0 )
       return 1;
    else
    {
         strncpy(next_month, vNext_Month, strlen(vNext_Month));
         next_month[strlen(vNext_Month)]='\0';
 #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getNextMonth", "", "getNextMonth: End Execute");
#endif
         return 0;
    }
}


/*
 * getNormalDate  根据输入时间计算相对时间
 *
 * 功能: 根据输入时间计算相对时间
 *
 * input参数1: *input_time,  输入时间，长度17个字节；
 *
 * input参数2: *time_flag,   时间标志，长度1个字节；
 *                           0 : 绝对时间
 *                           1:  相对自然月(年)开始时间：
 *                           2:  相对自然月(年)结束时间：
 *                           3:  相对帐务月结束时间
 *
 * input参数3: *time_cycle,  时间周期数，整型
 *
 * input参数4: *time_unit,   时间周期单位，整型
 *                           0 : 天
 *                           1 : 月
 *                           2 : 年
 * output参数5: *output_time,   相对时间   长度17字节
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 参数错误;
 *
 *
 */

int getNormalTime(char *input_time, char *time_flag, int time_cycle, int time_unit, char *output_time)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime: Start Execute");
#endif
    char vInput_Time[17+1];        /* 输入时间                      */
    char vOutput_Time[17+1];       /* 输出时间                      */
    char vTime_Flag[1+1];          /* 时间标志                      *
                                      1:  相对自然月时间            *
                                      2:  相对帐务月结束时间        */
    int  vTime_Cycle;              /* 时间周期数                    */
    int  vTime_Unit;               /* 时间周期单位                  */
    char vYear_Month[6+1];         /* 年月部分                      */


    char vSQL_Text[1024+1];        /* SQL语句      */

    memset(vInput_Time,         0, sizeof(vInput_Time));
    memset(vOutput_Time,        0, sizeof(vOutput_Time));
    memset(vTime_Flag,          0, sizeof(vTime_Flag));
    memset(&vTime_Cycle,        0, sizeof(vTime_Cycle));
    memset(&vTime_Unit,         0, sizeof(vTime_Unit));
    memset(vYear_Month,         0, sizeof(vYear_Month));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));

    if (input_time != NULL)
    {
        strncpy(vInput_Time,   input_time,   17);
        vInput_Time[17]      = '\0';
    }
    else return 1;


    if (time_flag != NULL)
    {
        strncpy(vTime_Flag,   time_flag,   1);
        vTime_Flag[1]      = '\0';

        if  ((strncmp(vTime_Flag,"1",1) != 0)
          && (strncmp(vTime_Flag,"2",1) != 0))
          return 1;
    }
    else return 1;

    vTime_Cycle  = time_cycle;
    vTime_Unit   = time_unit;

    if (vTime_Unit < 0 || vTime_Unit > 2) return 1;


    strncpy(vYear_Month,   vInput_Time,   6);
    vYear_Month[6]      = '\0';

#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vInput_Time=[%s]",vInput_Time);
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vTime_Flag =[%s]",vTime_Flag);
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vTime_Cycle=[%d]",vTime_Cycle);
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vTime_Unit =[%d]",vTime_Unit);
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vYear_Month=[%s]",vYear_Month);
#endif

    strcpy(vSQL_Text,"\0");
    if (strncmp(vTime_Flag, "1", 1) == 0)
    {
       if (vTime_Unit == 0)
          sprintf(vSQL_Text,"SELECT To_Char(To_Date('%s','YYYYMMDD HH24:MI:SS')+%d,'YYYYMMDD HH24:MI:SS') FROM Dual",vInput_Time,vTime_Cycle);
       else if (vTime_Unit == 1)
          sprintf(vSQL_Text,"SELECT To_Char(Add_Months(To_Date('%s','YYYYMMDD HH24:MI:SS'),%d),'YYYYMMDD HH24:MI:SS') FROM Dual",vInput_Time,vTime_Cycle);
       else if (vTime_Unit == 2)
          sprintf(vSQL_Text,"SELECT To_Char(Add_Months(To_Date('%s','YYYYMMDD HH24:MI:SS'),%d*12),'YYYYMMDD HH24:MI:SS') FROM Dual",vInput_Time,vTime_Cycle);
    }
    else
    {
       if (vTime_Unit == 0)
          sprintf(vSQL_Text,"SELECT To_Char(To_Date('%s01','YYYYMMDD')+%d,'YYYYMMDD HH24:MI:SS') FROM Dual",vYear_Month,vTime_Cycle);
       else if (vTime_Unit == 1)
          sprintf(vSQL_Text,"SELECT To_Char(Add_Months(To_Date('%s01','YYYYMMDD'),%d),'YYYYMMDD HH24:MI:SS') FROM Dual",vYear_Month,vTime_Cycle);
       else if (vTime_Unit == 2)
          sprintf(vSQL_Text,"SELECT To_Char(Add_Months(To_Date('%s01','YYYYMMDD'),%d*12),'YYYYMMDD HH24:MI:SS') FROM Dual",vYear_Month,vTime_Cycle);
    }

#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:vSQL_Text = [%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_NormalTime FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_NormalTime CURSOR FOR Prepare_NormalTime;
    EXEC SQL OPEN Cursor_NormalTime;
    for(;;)
    {
          EXEC SQL FETCH Cursor_NormalTime INTO :vOutput_Time;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime:提取相对时间失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_NormalTime;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_NormalTime;

    /*
     * 若相对时间为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vOutput_Time)) <=0 )
       return 1;
    else
    {
         strncpy(output_time, vOutput_Time, strlen(vOutput_Time));
         output_time[strlen(vOutput_Time)]='\0';
	 #ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getNormalTime", "", "getNormalTime: End Execute");
	#endif
         return 0;
    }
}


/*
 * chkServiceCancel  原子产品是否允许取消校验函数
 *
 * 功能:检查原子产品是否允许取消；
 *
 * input参数1:  *service_code，  原子产品代码，长度4个字节；
 *
 * input参数2:  *business_code,  业务代码,   长度1个字节;
 *
 * output参数3: *choiced_num,    取消方式    整型;
 *                               0 : 自由取消;
 *                               2 : 绑定取消;
 * output参数4: *send_flag,      生效方式,   长度1个字节;
 *                               0 : 实时,1 : 预约;
 *
 *
 * 返回参数:           <0, 执行失败；
 *                      0, 执行成功，校验一致;
 *                      1, 无配置数据
 *                      2, 不再该模块取消
 *                      3, 规则无效，不允许取消
 *                      4, 返回空数据
 *                      5, 取消方式数据错误
 *
 *
 */

int chkServiceCancel(char *service_code,char *business_code, int *choiced_num, char *send_flag)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceCancel", "", "chkServiceCancel: Start Execute");
#endif
          char vService_Code[4+1];      /* 服务代码     */
          char vBusiness_Code[1+1];     /* 业务代码     *
                                         * 0:开户       *
                                         * 1:变更       *
                                         * 2:其他       */

          char vBusiness_Code_Tmp[20+1];/* 业务代码Tmp  */
          char vRule_Status_Tmp[1+1];   /* 业务规则Tmp  */
          int  vChoiced_Num_Tmp;        /* 取消方式Tmp  */
          char vSend_Flag_Tmp[1+1];     /* 生效方式Tmp  */

          char vSQL_Text[1024+1];       /* SQL语句      */
          char vTmp_String[1024+1];     /* 临时变量     */
          int  vNum=0;                  /* 临时计数器   */

      memset(vService_Code,       0, sizeof(vService_Code));
      memset(vBusiness_Code,      0, sizeof(vBusiness_Code));
      memset(vBusiness_Code_Tmp,  0, sizeof(vBusiness_Code_Tmp));
      memset(vRule_Status_Tmp,    0, sizeof(vRule_Status_Tmp));
      memset(&vChoiced_Num_Tmp,   0, sizeof(vChoiced_Num_Tmp));
      memset(vSend_Flag_Tmp,      0, sizeof(vSend_Flag_Tmp));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&vNum,               0, sizeof(vNum));


      strncpy(vService_Code,       service_code, strlen(service_code));
      vService_Code[4]          = '\0';

      strncpy(vBusiness_Code,      business_code, strlen(business_code));
      vBusiness_Code[1]         = '\0';


      vNum = 0;
      vChoiced_Num_Tmp  = -1;

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num FROM sSrvCancel ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Service_Code = trim('%s') ",vTmp_String,vService_Code);

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceCancel", "", "chkServiceCancel:vSQL_Text=[%s]",vSQL_Text);
#endif

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num FROM sSrvCancel ");
      strcat(vSQL_Text," WHERE Service_Code   = trim(:v1) ");

      EXEC SQL PREPARE Prepare_sSrvCancel FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_sSrvCancel CURSOR FOR Prepare_sSrvCancel;
      EXEC SQL OPEN Cursor_sSrvCancel using :vService_Code;
      for(;;)
      {
            EXEC SQL FETCH Cursor_sSrvCancel
                     INTO :vBusiness_Code_Tmp,:vRule_Status_Tmp,
                          :vSend_Flag_Tmp,    :vChoiced_Num_Tmp;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "chkServiceCancel", "", "读sSrvCancel原子产品取消表失败!,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sSrvCancel;
                  return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            vNum++;
      }
      EXEC SQL CLOSE Cursor_sSrvCancel;



      /*
       * 若无数据配置，则返回 1；
       *
       * Commented by lab 2004.11.23
       */

      if (vNum <= 0)
          return 1;

      /*
       * 若业务代码不归属于产品变更允许的业务代码，则返回 2；
       *
       * Commented by lab 2004.11.23
       */

      if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)
         return 2;

      /*
       * 若业务规则无效，则返回2；
       *
       * Commented by lab 2004.11.23
       */

      if (strncmp(vRule_Status_Tmp,"Y",1) != 0)
         return 3;

      /*
       * 返回取消方式标志
       *
       * Commented by lab 2005.05.17
       */

      if (choiced_num != NULL)
      {
          if (vChoiced_Num_Tmp < 0)    return 5;
          else  *choiced_num = vChoiced_Num_Tmp;
      }

      /*
       * 返回生效标志
       *
       * Commented by lab 2005.05.17
       */

      if (send_flag != NULL)
      {
          if (strlen(rtrim(vSend_Flag_Tmp)) <=0 )
             return 4;
          else
          {
               strncpy(send_flag, vSend_Flag_Tmp, strlen(vSend_Flag_Tmp));
               send_flag[strlen(vSend_Flag_Tmp)]='\0';
          }
      }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkServiceCancel", "", "chkServiceCancel: End Execute");
#endif
      return 0;
}


/*
 * chkProductCancel  可选产品是否允许取消校验函数
 *
 * 功能:检查可选产品是否允许取消；
 *
 * input参数1:  *product_code，  原产品代码，长度8个字节；
 *
 * input参数2:  *object_product，新产品代码，长度8个字节；
 *
 * input参数3:  *business_code,  业务代码,   长度1个字节;
 *
 * output参数4: *choiced_num,    取消方式    整型;
 *                               0 : 自由取消;
 *                               2 : 绑定取消;
 * output参数5: *send_flag,      生效方式,   长度1个字节;
 *                               0 : 实时,1 : 预约;
 *
 *
 * 返回参数:           <0, 执行失败；
 *                      0, 执行成功，校验一致;
 *                      1, 无配置数据
 *                      2, 不再该模块取消
 *                      3, 规则无效，不允许取消
 *                      4, 返回空数据
 *                      5, 取消方式数据错误
 *
 *
 */

int chkProductCancel(char *product_code,char *object_product,char *business_code, int *choiced_num, char *send_flag)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductCancel", "", "chkProductCancel: Start Execute");
#endif
          char vProduct_Code[8+1];      /* 原产品代码   */
          char vObject_Product[8+1];    /* 新产品代码   */
          char vBusiness_Code[1+1];     /* 业务代码     *
                                         * 0:开户       *
                                         * 1:变更       *
                                         * 2:其他       */

          char vBusiness_Code_Tmp[20+1]; /* 业务代码Tmp  */
          char vRule_Status_Tmp[1+1];   /* 业务规则Tmp  */
          int  vChoiced_Num_Tmp;        /* 取消方式Tmp  */
          char vSend_Flag_Tmp[1+1];     /* 生效方式Tmp  */

          char vSQL_Text[1024+1];       /* SQL语句      */
          char vTmp_String[1024+1];     /* 临时变量     */
          int  vNum=0;                  /* 临时计数器   */

      memset(vProduct_Code,       0, sizeof(vProduct_Code));
      memset(vObject_Product,     0, sizeof(vObject_Product));
      memset(vBusiness_Code,      0, sizeof(vBusiness_Code));
      memset(vBusiness_Code_Tmp,  0, sizeof(vBusiness_Code_Tmp));
      memset(vRule_Status_Tmp,    0, sizeof(vRule_Status_Tmp));
      memset(&vChoiced_Num_Tmp,   0, sizeof(vChoiced_Num_Tmp));
      memset(vSend_Flag_Tmp,      0, sizeof(vSend_Flag_Tmp));

      memset(vSQL_Text,           0, sizeof(vSQL_Text));
      memset(vTmp_String,         0, sizeof(vTmp_String));
      memset(&vNum,               0, sizeof(vNum));


      strncpy(vProduct_Code,       product_code, strlen(product_code));
      vProduct_Code[8]          = '\0';

      strncpy(vObject_Product,     object_product, strlen(object_product));
      vObject_Product[8]        = '\0';

      strncpy(vBusiness_Code,      business_code, strlen(business_code));
      vBusiness_Code[1]         = '\0';


      vNum = 0;
      vChoiced_Num_Tmp  = -1;

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num FROM sProductCancel ",vTmp_String);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s WHERE Product_Code = trim('%s') ",vTmp_String,vProduct_Code);

      strcpy(vTmp_String,"\0");    sprintf(vTmp_String,"%s",vSQL_Text);
      sprintf(vSQL_Text,"%s AND   Object_Product = trim('%s') ",vTmp_String,vObject_Product);


#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductCancel", "", "chkProductCancel:vSQL_Text=[%s]",vSQL_Text);
#endif

      memset(vSQL_Text,           0, sizeof(vSQL_Text));

      strcpy(vSQL_Text," SELECT Trim(Business_Code),Trim(Rule_Status),Trim(Send_Flag),Choiced_Num FROM sProductCancel ");
      strcat(vSQL_Text," WHERE Product_Code   = trim(:v1) ");
      strcat(vSQL_Text," AND   Object_Product = trim(:v2) ");



      EXEC SQL PREPARE Prepare_sProductCancel FROM :vSQL_Text;
      EXEC SQL DECLARE Cursor_sProductCancel CURSOR FOR Prepare_sProductCancel;
      EXEC SQL OPEN Cursor_sProductCancel using :vProduct_Code,:vObject_Product;
      for(;;)
      {
            EXEC SQL FETCH Cursor_sProductCancel
                     INTO :vBusiness_Code_Tmp,:vRule_Status_Tmp,
                          :vSend_Flag_Tmp,    :vChoiced_Num_Tmp;

            if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
            {
                  pubLog_Debug(_FFL_, "chkProductCancel", "", "读sProductCancel可选产品取消表失败!,SQLCODE=[%d]",SQLCODE);
                  EXEC SQL CLOSE Cursor_sProductCancel;
                  return -1;
            }

            if (SQLCODE == NOTFOUND)
               break;

            vNum++;
      }
      EXEC SQL CLOSE Cursor_sProductCancel;



      /*
       * 若无数据配置，则返回 1；
       *
       * Commented by lab 2004.11.23
       */

      if (vNum <= 0)
          return 1;

      /*
       * 若业务代码不归属于产品变更允许的业务代码，则返回 2；
       *
       * Commented by lab 2004.11.23
       */

      if (strstr(vBusiness_Code_Tmp, vBusiness_Code) == NULL)
         return 2;

      /*
       * 若业务规则无效，则返回2；
       *
       * Commented by lab 2004.11.23
       */

      if (strncmp(vRule_Status_Tmp,"Y",1) != 0)
         return 3;

      /*
       * 返回取消方式标志
       *
       * Commented by lab 2005.05.17
       */

      if (choiced_num != NULL)
      {
          if (vChoiced_Num_Tmp < 0)    return 5;
          else  *choiced_num = vChoiced_Num_Tmp;
      }

      /*
       * 返回生效标志
       *
       * Commented by lab 2005.05.17
       */

      if (send_flag != NULL)
      {
          if (strlen(rtrim(vSend_Flag_Tmp)) <=0 )
             return 4;
          else
          {
               strncpy(send_flag, vSend_Flag_Tmp, strlen(vSend_Flag_Tmp));
               send_flag[strlen(vSend_Flag_Tmp)]='\0';
          }
      }
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "chkProductCancel", "", "chkProductCancel: End Execute");
#endif
       
      return 0;
}

/*
 * getSysDate  提取系统时间
 *
 * 功能: 提取系统时间
 *
 * output参数1: *sysdate,  系统时间，长度17个字节；
 *
 *
 * 返回参数:           <0，执行失败,ORACLE报错；
 *                      0，执行成功;
 *                      1, 数据为空;
 *
 *
 */

int getSysDate(char *sysdate)
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSysDate", "", "getSysDate: Start Execute");
#endif
       
    char vSysDate[17+1];           /* 系统时间     */
    char vSQL_Text[1024+1];        /* SQL语句      */

    memset(vSysDate,            0, sizeof(vSysDate));
    memset(vSQL_Text,           0, sizeof(vSQL_Text));

    strcpy(vSQL_Text,"\0");
    strcpy(vSQL_Text,"SELECT To_Char(SysDate,'YYYYMMDD HH24:MI:SS') FROM Dual");
#ifdef _DEBUG_
      pubLog_Debug(_FFL_, "getSysDate", "", "getSysDate:vSQL_Text = [%s]",vSQL_Text);
#endif

    EXEC SQL PREPARE Prepare_SysDate FROM :vSQL_Text;
    EXEC SQL DECLARE Cursor_SysDate CURSOR FOR Prepare_SysDate;
    EXEC SQL OPEN Cursor_SysDate;
    for(;;)
    {
          EXEC SQL FETCH Cursor_SysDate INTO :vSysDate;

          if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
          {
                pubLog_Debug(_FFL_, "getSysDate", "", "提取系统时间失败!,SQLCODE=[%d]",SQLCODE);
                EXEC SQL CLOSE Cursor_SysDate;
                return -1;
          }

          if (SQLCODE == NOTFOUND)
             break;
    }
    EXEC SQL CLOSE Cursor_SysDate;

    /*
     * 若系统时间为空，则提示返回错误;
     *
     * Commented by lab 2004.11.01
     */

    if (strlen(rtrim(vSysDate)) <=0 )
       return 1;
    else
    {
         strncpy(sysdate, vSysDate, strlen(vSysDate));
         sysdate[strlen(vSysDate)]='\0';
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getSysDate", "", "getSysDate: End Execute");
#endif
         return 0;
    }
}


/*
 * getClassCode   根据节点提取营业厅类型
 *
 * 功能:根据输入的节点值从sTownCode提取营业点类型
 *
 * input  参数1: *node，       节点值，最大长度10个字节；
 *
 * input  参数2: NODE *array， 全部子节点机构体；
 *
 * output 参数2: class_code，  营业点类型，最大长度2个字节
 *
 *
 * 返回参数:             0: 成功;
 *                      -1: 节点无效；
 *                      -2: 从数据库提取节点失败；
 *
 */
int getClassCode(char *node,NODE *array,char *class_code)
{

       char vNode[TREE_NODE_LENGTH+1];          /* 节点            */
       char vParent_Node[TREE_NODE_LENGTH+1];   /* 父节点          */
       char vClass_Code[2+1];                   /* 营业点类型      */
       char vSQL_String[1024+1];                /* SQL语句         */
       char vTmp_String[1024+1];                /* 临时变量        */
       int  vNode_Level;                        /* 节点深度        */
       int  vret_code;                          /* 返回值          */

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: Start Execute");
#endif

     memset(vNode,         0, sizeof(vNode));
     memset(vParent_Node,  0, sizeof(vParent_Node));
     memset(vClass_Code,   0, sizeof(vClass_Code));
     memset(vSQL_String,   0, sizeof(vSQL_String));
     memset(vTmp_String,   0, sizeof(vTmp_String));
     vNode_Level         = 0;
     vret_code           = 0;


     strncpy(vNode,   node,  TREE_NODE_LENGTH);
     vNode[TREE_NODE_LENGTH] = '\0';

     vNode_Level = getNodeLevel(vNode,array);

     if ((vNode_Level < TREE_COUNTRY_LEVEL) || (vNode_Level > TREE_LOGIN_LEVEL))
        return -1;


     if (vNode_Level == TREE_TOWN_LEVEL)
     {
        strncpy(vParent_Node,   vNode,  TREE_NODE_LENGTH);
        vParent_Node[TREE_NODE_LENGTH]  = '\0';
     }


     if (vNode_Level == TREE_LOGIN_LEVEL)
     {
         vret_code = getGroupParentNode(vNode,array,vParent_Node);

         if (vret_code != 0)
         {
              pubLog_Debug(_FFL_, "getClassCode", "", "getParentNode: getParentNode sub function ret_code=[%d]",vret_code);
              return -1;
         }

         vNode_Level = getNodeLevel(vParent_Node,array);

         if (vNode_Level <= 0)
            return -1;

     }

     if (vNode_Level < TREE_TOWN_LEVEL && vNode_Level > TREE_COUNTRY_LEVEL )
     {
           strcpy(vClass_Code,"99");
           strncpy(class_code,rtrim(vClass_Code),strlen(rtrim(vClass_Code)));
           class_code[strlen(rtrim(vClass_Code))]='\0';
           return 0;
     }

     if (vNode_Level == TREE_TOWN_LEVEL)
     {

         strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_String);
         sprintf(vSQL_String,"%s SELECT Trim(Innet_Type) FROM sTownCode ",vTmp_String);

         strcpy(vTmp_String,"\0");  strcpy(vTmp_String,vSQL_String);
         sprintf(vSQL_String,"%s WHERE Group_Id=trim('%s') ",vTmp_String,vParent_Node);

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: vSQL_String=[%s]",vSQL_String);
#endif
         memset(vSQL_String,   0, sizeof(vSQL_String));

         strcpy(vSQL_String," SELECT Trim(Innet_Type) FROM sTownCode ");
         strcat(vSQL_String," WHERE Group_Id=trim(:v1)");


#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: vSQL_String=[%s]",vSQL_String);
#endif

         EXEC SQL PREPARE P_Class_Code FROM :vSQL_String;
         EXEC SQL DECLARE C_Class_Code CURSOR FOR P_Class_Code;
         EXEC SQL OPEN C_Class_Code using :vParent_Node;
         for(;;)
         {
                EXEC SQL FETCH C_Class_Code INTO :vClass_Code;

                if (SQLCODE == NOTFOUND )
                      break;

                if ((SQLCODE != 0) && (SQLCODE != NOTFOUND))
                {
                      pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: 根据被检索节点提取营业厅类型失败,SQLCODE=[%d]",SQLCODE);
                      EXEC SQL CLOSE C_Class_Code;
                      return -2;
                }
         }
         EXEC SQL CLOSE C_Class_Code;
     }

#ifdef _DEBUG_
     pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: Class_Code=[%s]",vClass_Code);
#endif



     if (strlen(rtrim(vClass_Code)) <= 0)
         return -1;
     else
     {
         strncpy(class_code,rtrim(vClass_Code),strlen(rtrim(vClass_Code)));
         class_code[strlen(rtrim(vClass_Code))]='\0';
     }

#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "getClassCode", "", "getClassCode: End Execute");
#endif
     return 0;
}


/*****************************************
函数说明： sAppleGrantPreMsg
功    能： 缴费回馈/买手机赠话费申请（产品改造使用）
编写日期： 2005/10/20
*****************************************/

int sProductApplyFunc( tGrantData  vGrantMsg, char *retMsg )
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "sProductApplyFunc", "", "sProductApplyFunc: Start Execute");
#endif

	int			i=0,j=0,k=0,ret=0;
	char		temp_buf[1000];		/*临时使用的字符串数组*/

	EXEC SQL BEGIN DECLARE SECTION;
	struct wd_dcustmsg_area stdCustMsg;
	struct wd_dloginmsg_area stdLoginMsg;
	struct wd_sgrantcfg_area stsGrantCfg;
	struct wd_sgrantmarkcode_area stsGrantMarkCode;
	struct wd_sgrantpredetcode_area stsGrantPreDetCode;
	struct wd_sgrantcfg_area stsGtantCfg;
	struct wd_spresentcfg_area stsPresentCfg;
	struct wd_spayuseflag_area stsPayUseFlag;


		/* 输入参数 */
		char iOpCode[4+1];		/* 操作代码 */
		char iLoginNo[6+1];		/* 工号 */
		char iLoginPasswd[16+1];/* 工号密码 */
		char iOrgCode[9+1];		/* 工号机构编码 */
		char iIpAdd[15+1];		/* ip地址 */

		char iPhoneNo[15+1];	/* 手机号码 */
		char iContractNo[14+1];	/* 帐户 */
		char iPrepayFee[16+1];	/* 预存款 */
		char iModeCode[8+1];	/* 最底消费 */
		char iBigFlag[2+1];		/* 大客户等级 */
		char iCurrentPoint[10+1];/* 大客户等级 */
		char iOldSm[2+1];		/* 赠机后品牌 */
		char iNewSm[2+1];		/* 赠机后品牌 */
		char iSysNote[60+1];	/* 系统备注 */
		char iOpNote[60+1];		/* 用户备注 */
		char iGrantFlag[1+1];
		char iGrantCode[8+1];
		char iIdNo[14+1];
		char iMarkUsed[10];
		char iMarkAdd[10];
		char iSellCode[4+1];
		char iOpenTime[17+1];
		char iBunchNo[20+1];

		char vFPrepayFee[10];
		char vCFavMoney[10];
		char vFavFee[10];
		char vMonthFav[10];
		char vMonthNum[10];
		char vFavMoney[10];
		char vSendFlag[1+1];
		int   RetValue=0;
		char vLoginAccept[14+1];
		char vOldLoginAccept[14+1];
		char vOpTime[20+1];
		long vTotalDate=0;
		char vSmCode[7+1];
		char vBelongCode[7+1];
		long vCustId=0;
		char vSqlStr[1000];
		long vCurrentPoint=0;
		int  vFavourFee1=0;
		int  vFavourFee2=0;
		char vNowMode[8+1];
		char vOldAccept[14+1];
		char vEffectTime[17+1];
		char vPayType[1+1];
		char vRunCode[2+1];
		char  attr_code[8+1];
		char  credit_code[1+1];
		double limit_owe=0;
		char  region_code[2+1];
		float in_pay_money=0.00;
		float tmp_fee=0.00;
		double owe_fee2=0.00;
		double unbill_fee=0.00;
	    	double unbill_should=0.00;
	    	double unbill_favour=0.00;
	    	double noaccount_fee=0;
	    	long  in_id_no=0;
	    	char vphone_no[15+1];
	    	int vCount=0;
	    	char vRepFlag[1+1];
	    	char vPrepayFee[10];
        	char vPlanFee[10];
        	char vPlanMonths[10];
        	char vPlanFlag[1+1];
        	char vPlanAddFee[10];
        	char iLoginAccept[14+1];
	/* My defin begin */
	int		iRetCode = 0;
	char	grantDetType[1+1];
	char	detailCode[4+1];
	long	lMarkUsed = 0;
	char	vMarkType[1+1];
	double  vLimitFee = 0;  /* 起存金额，即最小配置金额beginMoney*/
	double  vSendFee = 0;   /* 送费金额 */
	double  vTransFee = 0;
	double  dCashPay = 0;
	char	vProdPayType[1+1];
	long	lPayAccept = 0;
	char	oPayMoney[16+1];
	char vOrgId[10+1];
	char vGroupId[10+1];

	EXEC SQL END DECLARE SECTION;

	/* 初始化变量 */
	init(iLoginAccept);
	init(vPlanAddFee);
	init(vPrepayFee);
	init(vPlanFee);
	init(vPlanMonths);
	init(vPlanFlag);
	init(vRepFlag);
	init(vphone_no);
	init(iOpCode);
	init(iLoginNo);
	init(iLoginPasswd);
	init(iOrgCode);
	init(iIpAdd);
	init(iPhoneNo);
	init(iContractNo);
	init(iPrepayFee);
	init(iModeCode);
	init(iBigFlag);
	init(iCurrentPoint);
	init(iOldSm);
	init(iNewSm);
	init(iSysNote);
	init(iOpNote);
	init(iGrantFlag);
	init(iGrantCode);
	init(iIdNo);
	init(iMarkUsed);
	init(iMarkAdd);
	init(iSellCode);
	init(iOpenTime);
	init(vFPrepayFee);
	init(vCFavMoney);
	init(vFavFee);
	init(vMonthFav);
	init(vMonthNum);
	init(vFavMoney);
	init(vSendFlag);
	init(vLoginAccept);
	init(vOldLoginAccept);
	init(vOpTime);
	init(vSmCode);
	init(vBelongCode);
	init(vSqlStr);
	init(vNowMode);
	init(vOldAccept);
	init(vEffectTime);
	init(vPayType);
	init(vRunCode);
	init(attr_code);
	init(credit_code);
	init(region_code);
	init(iBunchNo);
	init(vOrgId);
	init(vGroupId);

    /* 检查工号信息是否存在 */
    memset(&stdLoginMsg, 0, sizeof(stdLoginMsg));
    strcpy(stdLoginMsg.login_no, vGrantMsg.loginNo);
    if(DbsDLOGINMSG(DBS_FIND, &stdLoginMsg) != 0)
    {
        sprintf(retMsg, "工号[%s]不存在!", vGrantMsg.loginNo);
        return 800001;
    }

    /* 查询用户资料是否存在 */
    memset(&stdCustMsg, 0, sizeof(stdCustMsg));
    strcpy(stdCustMsg.phone_no, vGrantMsg.phoneNo);
    if(DbsDCUSTMSG2(DBS_FIND, &stdCustMsg) != 0)
    {
        sprintf(retMsg, "用户[%s]不存在!", vGrantMsg.phoneNo);
        return 800002;
    }
    
    /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret = sGetLoginOrgid(stdLoginMsg.login_no,vOrgId);
       if(ret <0)
         	{
         	  pubLog_Debug(_FFL_, "sProductApplyFunc", "", "获取用户org_id失败!");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/
      /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetUserGroupid(stdCustMsg.phone_no,vGroupId);
       if(ret <0)
         	{
         	  pubLog_Debug(_FFL_, "sProductApplyFunc", "", "获取集团用户group_id失败!");
         	  return 200919; 
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/

    
    if (vGrantMsg.applyType[0] == PROD_SENDFEE_CODE)
    {
        /* 检查缴费回馈代码配置是否正确 */
        memset(&stsGrantCfg, 0, sizeof(stsGtantCfg));
        strncpy(stsGrantCfg.region_code, stdLoginMsg.org_code, 2);
        strcpy(stsGrantCfg.det_mode_code, vGrantMsg.detModeCode);
        if (DbsSGRANTCFG(DBS_FIND, &stsGrantCfg) != 0)
        {
            sprintf(retMsg, "缴费回馈参数配置错误[%d]!", SQLCODE);
            return 800002;
        }
        strcpy(iGrantCode,      stsGrantCfg.grant_code);
        strcpy(iSellCode,       stsGrantCfg.sell_code);
		vLimitFee = stsGrantCfg.begin_money;

    } else if (vGrantMsg.applyType[0] == PROD_MACHFEE_CODE)
    {
        /* 检查买手机赠话费配置是否正确 */
        memset(&stsPresentCfg, 0, sizeof(stsPresentCfg));
        strncpy(stsPresentCfg.region_code, stdLoginMsg.org_code, 2);
        strcpy(stsPresentCfg.det_mode_code, vGrantMsg.detModeCode);
        if (DbsSPRESENTCFG(DBS_FIND, &stsPresentCfg) != 0)
        {
            sprintf(retMsg, "买手机赠话费参数配置错误[%d]!", SQLCODE);
            return 800002;
        }
        strcpy(iGrantCode,      stsPresentCfg.grant_code);
        strcpy(iSellCode,       "0000");
		vLimitFee = stsPresentCfg.begin_money;
    } else
    {
        sprintf(retMsg, "输入参数applyType=[%s]错误!", vGrantMsg.applyType);
        return 800009;
    }

	/* 确定最后一笔缴费金额 */
	if(vLimitFee > 0)
	{
		memset(oPayMoney, 0, sizeof(oPayMoney));
		getLastPayMoney(stdCustMsg.contract_no, vGrantMsg.totalDate, oPayMoney);
		if (atof(oPayMoney) < vLimitFee)
		{
			sprintf(retMsg, "用户最近一笔缴费金额[%s]小于回馈最低金额[%.2f]！",oPayMoney, vLimitFee);
			return 890001;
		}
	}

	strcpy(iOpCode,       	vGrantMsg.opCode);
	strcpy(iLoginNo,       	stdLoginMsg.login_no);
	strcpy(iOrgCode,       	stdLoginMsg.org_code);
	strcpy(iIpAdd,       	stdLoginMsg.ip_address);
	strcpy(iOpNote,       	vGrantMsg.opNote);
	sprintf(iIdNo,    "%ld",  	stdCustMsg.id_no);
	strcpy(iPhoneNo,       	vGrantMsg.phoneNo);
	sprintf(iContractNo,  "%ld",   stdCustMsg.contract_no);
	strcpy(iOldSm,       	stdCustMsg.sm_code);
	strncpy(iBigFlag,       	stdCustMsg.attr_code + 3, 2);
	strcpy(iOpenTime,       stdCustMsg.open_time);
	strcpy(iPrepayFee,      "0");

	strcpy(iNewSm,       	stdCustMsg.sm_code);
	strcpy(iLoginAccept,	vGrantMsg.loginAccept);
	strcpy(iBunchNo,    vGrantMsg.machineId);

	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginPasswd);
	Trim(iOrgCode);
	Trim(iIpAdd);
	Trim(iOpNote);
	Trim(iIdNo);
	Trim(iPhoneNo);
	Trim(iContractNo);
	Trim(iOldSm);
	Trim(iCurrentPoint);
	Trim(iBigFlag);
	Trim(iOpenTime);
	Trim(iPrepayFee);
	Trim(iGrantFlag);
	Trim(iGrantCode);
	Trim(iMarkUsed);
	Trim(iNewSm);
	Trim(iModeCode);
	Trim(iSellCode);
	Trim(iMarkAdd);


	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOpCode=[%s]",iOpCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iLoginNo=[%s]",iLoginNo);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vLoginAccept=[%s]",vLoginAccept);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOrgCode=[%s]",iOrgCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iIpAdd=[%s]",iIpAdd);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOpNote=[%s]",iOpNote);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iIdNo=[%s]",iIdNo);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iPhoneNo=[%s]",iPhoneNo);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iContractNo=[%s]",iContractNo);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOldSm=[%s]",iOldSm);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iCurrentPoint=[%s]",iCurrentPoint);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iBigFlag=[%s]",iBigFlag);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOpenTime=[%s]",iOpenTime);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iPrepayFee=[%s]",iPrepayFee);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iGrantFlag=[%s]",iGrantFlag);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iGrantCode=[%s]",iGrantCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iMarkUsed=[%s]",iMarkUsed);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iMarkAdd=[%s]",iMarkAdd);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iNewSm=[%s]",iNewSm);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iModeCode=[%s]",iModeCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iSellCode=[%s]",iSellCode);


	strcpy(vOpTime, vGrantMsg.opTime);
	vTotalDate = atol( vGrantMsg.totalDate);
	strcpy(vLoginAccept,vGrantMsg.loginAccept);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vLoginAccept=[%s]",vLoginAccept);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iLoginNo=[%s]",iLoginNo);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iOpCode=[%s]",iOpCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iSellCode=[%s]",iSellCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vOpTime=[%s]",vOpTime);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "iGrantCode=[%s]",iGrantCode);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vTotalDate=[%ld]",vTotalDate);
	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vOpTime=[%s]",vOpTime);
	if (vGrantMsg.applyType[0] == PROD_SENDFEE_CODE)
	{
	     /*记录缴费回馈资料表DCUSTGRANTMSG*/
	    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "记录缴费回馈资料表DCUSTGRANTMSG");
	    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
	    /*EXEC SQl insert into DCUSTGRANTMSG
	        (
	            ID_NO,PHONE_NO,CONTRACT_NO,BELONG_CODE,OLD_SM,CURRENT_POINT,
	    		BIG_FLAG,OPEN_TIME,PREPAY_FEE,GRANT_FLAG,GRANT_CODE,MARK_USED,
	    		NEW_SM,CHANNEL,LOGIN_ACCEPT,LOGIN_NO,OP_TIME,TOTAL_DATE,OP_CODE,MARK_ADD,GROUP_ID)
	    select
	    	    id_no, phone_no, contract_no, belong_code, sm_code, 0,
	    		substr(attr_code, 3, 2) , open_time, :vLimitFee, '0', :iGrantCode, 0,
	    		sm_code, :iSellCode,to_number(:vLoginAccept), :iLoginNo, to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
	    		:vTotalDate, :iOpCode, 0,GROUP_NO
	    from	dCustMsg
	    where	id_no = :stdCustMsg.id_no;
	    if(SQLCODE != 0)
	    {
	    	sprintf(retMsg,"记录缴费回馈资料表DCUSTGRANTMSG失败[%d]!",SQLCODE);
	    	return 132104;
	    }

	    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "更新缴费回馈目标用户资料表DCUSTCONDMSG,iIdNo[%s] sell_code[%s]",iIdNo,iSellCode);
	    EXEC SQL update DCUSTCONDMSG set flag=flag+1
	    	where id_no = :stdCustMsg.id_no and sell_code = :iSellCode;
	    if(SQLCODE!=0 && SQLCODE!=1403)
	    {
	    	sprintf(retMsg,"更新缴费回馈目标用户资料表DCUSTCONDMSG失败[%d]!",SQLCODE);
	    	return 132105;
	    }    */
	} else if (vGrantMsg.applyType[0] == PROD_MACHFEE_CODE)
	{
	    /* 买手机赠话费资源处理 */
	    EXEC SQL insert into dBunchResHis
	    (
	        region_code,district_code,town_code,
			fetch_no,mach_code,bunch_no,resource_code,op_time,login_no,org_code,
			login_accept,total_date,bunch_type,update_accept,update_time,
			update_date,update_login,update_type,update_code,channel,GROUP_ID, ORG_ID
		)
		select
		    region_code,district_code,town_code,
			fetch_no,mach_code,bunch_no,resource_code, op_time,login_no,org_code,
			login_accept,total_date,bunch_type,:vLoginAccept,
			to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
			:vTotalDate, :iLoginNo, 'u',:iOpCode, channel,group_id,org_id
		from dBunchRes
		/**组织机构改造二期 edit by zhaoqm at 22/04/2009 -begin-
		where (region_code = substr(:stdLoginMsg.org_code,1,2) or region_code='99')
			and (district_code=substr(:stdLoginMsg.org_code,3,2)
				or district_code='99')
			and (town_code=substr(:stdLoginMsg.org_code,5,3) or town_code='999')
		*组织机构改造二期 edit by zhaoqm at 22/04/2009 -end-**/	
			where group_id 
				in (select parent_group_id from dchngroupinfo where group_id =:vOrgId)
			and bunch_no = :iBunchNo and bunch_type = '1'
			and resource_code ='0';
	    if(SQLCODE != 0)
	    {
	    	sprintf(retMsg,"机串号[%s]资源不存在[%d]!", iBunchNo, SQLCODE);
	    	return 130801;
	    }
	 
	    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/ 
	    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "修改dBunchRes表");
	    EXEC SQL update dBunchRes set region_code = substr(:stdLoginMsg.org_code,1,2),
				district_code=substr(:stdLoginMsg.org_code,3,2),
				town_code=substr(:stdLoginMsg.org_code,5,3), resource_code='1',
				op_time=to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
				org_id = :vOrgId,
				login_no=:iLoginNo, org_code =:stdLoginMsg.org_code,
				login_accept = :vLoginAccept, total_date=:vTotalDate
		/**组织机构改造二期 edit by zhaoqm at 22/04/2009 -begin-
			where (region_code=substr(:stdLoginMsg.org_code,1,2) or region_code='99')
				and (district_code=substr(:stdLoginMsg.org_code,3,2)
					or district_code='99')
				and (town_code=substr(:stdLoginMsg.org_code,5,3) or town_code='999')
				*组织机构改造二期 edit by zhaoqm at 22/04/2009 -end-**/	
			where group_id 
				in (select parent_group_id from dchngroupinfo where group_id =:vOrgId)
				and bunch_no=:iBunchNo and bunch_type='1'
				and resource_code ='0';
	    if(SQLCODE != 0)
	    {
	    	sprintf(retMsg,"修改手机串号资源表失败[%d]",SQLCODE);
	    	return 130801;
	    }
	}

	/* 处理缴费回馈、买手机送费配置明细 */
	iRetCode = 0;
	init(grantDetType); init(detailCode);
	EXEC SQL declare curGrant CURSOR for
	select  add_type, detail_code  from sGrantCfgAdd
	where   region_code = substr(:stdLoginMsg.org_code, 1, 2)
	and     grant_code = :iGrantCode;
	EXEC SQL OPEN  curGrant;
	EXEC SQL FETCH curGrant into :grantDetType, :detailCode;
	if (SQLCODE != 0)
	{
	    sprintf(retMsg, "缴费回馈产品配置错误[%d]!", SQLCODE);
	    return 800004;
	}

	while(SQLCODE == 0)
	{
	    /**
	    * 根据不同的回馈明细配置进行处理,建议每种类型配置一条记录,
	    * 但程序支持同一种类型的多种回馈形式！
	    **/

	    if (grantDetType[0] == GRANT_DETTYPE_SENDXJ) /* 回馈现金 */
	    {
	        pubLog_Debug(_FFL_, "sProductApplyFunc", "", "回馈现金开始。。。");
	        /* 检查缴费送费配置信息是否存在 */
            memset(&stsGrantPreDetCode, 0, sizeof(stsGrantPreDetCode));
            strncpy(stsGrantPreDetCode.region_code, stdLoginMsg.org_code, 2);
            strcpy(stsGrantPreDetCode.detail_code, detailCode);
            if (DbsSGRANTPREDETCODE(DBS_FIND, &stsGrantPreDetCode) != 0)
            {
                sprintf(retMsg, "送费产品明细配置错误[%d]!", SQLCODE);
                iRetCode = 800002;
                break;
            }

			/* 确定送费金额 PROD_SEND_NOW ＝ '0' */
			if (stsGrantPreDetCode.send_flag[0] == PROD_SEND_NOW) /* 立即赠送 */
			{
				if (stsGrantPreDetCode.month_num <= 1)
				{
					vSendFee = stsGrantPreDetCode.fav_money; /* 全部送费金额 */
				}else {
					vSendFee = stsGrantPreDetCode.month_fav; /* 本次送费金额 */
				}

			} else {
				vSendFee = 0; /* 赠送金额下月送出 */
			}
       /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
            EXEC SQL insert into dCustPresent
            (
                id_no,phone_no,belong_code,fav_code,
				bunch_no,prepay_fee,fav_money,
				month_fav,month_num,
				contract_no,begin_time,
				end_time,fav_fee,login_accept,login_no,
				open_time,total_date,
				op_time,op_code,sell_code,group_id)
			select
				:stdCustMsg.id_no,:stdCustMsg.phone_no, :stdCustMsg.belong_code, :iGrantCode,
				:vLoginAccept, 0 ,:stsGrantPreDetCode.fav_money,
				:stsGrantPreDetCode.month_fav, :stsGrantPreDetCode.month_num,
				:stdCustMsg.contract_no, to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
				add_months(to_date(to_char(last_day(sysdate)+1,'yyyymmdd'),'yyyymmdd'),:stsGrantPreDetCode.month_num - decode(:stsGrantPreDetCode.send_flag,'0', 1, 0)),
				decode(:stsGrantPreDetCode.send_flag, '0', :stsGrantPreDetCode.fav_money, 0), :vLoginAccept,
				:iLoginNo,to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :vTotalDate,
				to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :iOpCode,:iSellCode, :vGroupId
			from dual;
			if(SQLCODE != 0)
			{
				sprintf(retMsg,"插入用户赠送表失败[%d]!",SQLCODE);
				return 132107;
				break;
			}

			/*记录dCustPresentHis表*/
			pubLog_Debug(_FFL_, "sProductApplyFunc", "", "	记录dCustPresentHis表");
			/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL insert into dCustPresentHis
			(
			    id_no,phone_no,belong_code,fav_code,
				bunch_no,prepay_fee,fav_money,
				month_fav,month_num,
				contract_no,begin_time,
				end_time,fav_fee,login_accept,login_no,
				open_time,total_date,
				op_time,op_code,update_accept,
				update_time,update_date,update_login,
				update_type,update_code,sell_code,group_id)
			select
			    id_no,phone_no,belong_code,fav_code,
				bunch_no,prepay_fee,fav_money,
				month_fav,month_num,
				contract_no,begin_time,
				end_time,fav_fee,login_accept,login_no,
				open_time,total_date,
				op_time,op_code, :vLoginAccept,
				to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :vTotalDate, :iLoginNo,
				'a', :iOpCode, :iSellCode, group_id
			from dCustPresent
			where   id_no = :stdCustMsg.id_no
			and     login_accept = to_number(:vLoginAccept);
			if(SQLCODE != 0)
			{
				sprintf(retMsg,"插入用户赠送历史表失败[%d]!",SQLCODE);
				iRetCode = 132108;
				break;
			}

			/* 获取产品包对应的专款项（如果没有配置默认为现金0） */
			memset(vProdPayType, 0, sizeof(vProdPayType));
			memset(&stsPayUseFlag, 0, sizeof(stsPayUseFlag));
			strncpy(stsPayUseFlag.region_code, stdLoginMsg.org_code, 2);
			strcpy(stsPayUseFlag.grant_code, iGrantCode);
			if( DbsSPAYUSEFLAG(DBS_FIND, &stsPayUseFlag) != 0)
			{
				strcpy(vProdPayType, "0");
			}else {
				strcpy(vProdPayType, stsPayUseFlag.pay_type);
			}

			/* 更新用户帐户信息 */
			EXEC SQL update dConMsg set
				prepay_fee = prepay_fee + :vSendFee,
				prepay_time = to_date(:vOpTime,'yyyymmdd hh24:mi:ss')
			where contract_no = :stdCustMsg.contract_no;
			if(SQLCODE != 0)
			{
				sprintf(retMsg,"用户默认帐户[%ld]资料不存在[%d]!",stdCustMsg.contract_no, SQLCODE);
				iRetCode = 132109;
				break;
			}

			/* 如果专款不是现金的话，从现金扣除用户专款 */
			if (vProdPayType[0] != '0')
			{
				/* 查询用户现金余额 */
				/*EXEC SQL SELECT  nvl(sum(prepay_fee), 0) into :dCashPay
				FROM	dConMsgPre
				WHERE	contract_no = :stdCustMsg.contract_no
				and pay_type = '0';
				if (SQLCODE != 0)
				{
					sprintf(retMsg,"该用户默认帐户非现金帐户，不能办理此业务[%d]!",SQLCODE);
					iRetCode = 132120;
					break;
				}      */

				if (vLimitFee > dCashPay)
				{
					sprintf(retMsg,"该用户默认帐户现金余额不足，请补交现金[%.2f]元!",vLimitFee - dCashPay);
					iRetCode = 132120;
					break;
				}

				/* 专款帐户非现金帐户 */
				/*EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
					prepay_fee = prepay_fee - :vLimitFee,
					add_prepay = 0 - :vLimitFee
				where contract_no = :stdCustMsg.contract_no
				and pay_type = '0';
				if(SQLCODE != 0)
				{
					sprintf(retMsg,"更新用户预存款失败[%d]!",SQLCODE);
					iRetCode = 132120;
					break;
				} */

			/*	EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
					prepay_fee = prepay_fee + :vLimitFee + :vSendFee,
					add_prepay = :vLimitFee + :vSendFee
				where contract_no = :stdCustMsg.contract_no
				and pay_type = :vProdPayType;
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(retMsg,"修改用户预存款明细失败[%d]!",SQLCODE);
					iRetCode = 132121;
					break;
				}

				if(SQLCODE == 1403)
				{
					EXEC SQL insert into dConMsgPre(contract_no,pay_type,prepay_fee,
						last_prepay,add_prepay,live_flag,allow_pay,begin_dt,end_dt)
					values( :stdCustMsg.contract_no, :vProdPayType,
						:vLimitFee + :vSendFee, 0,
						:vLimitFee + :vSendFee, '0', '0', :vTotalDate, 20200101);
					if(SQLCODE != 0)
					{
						sprintf(retMsg,"插入用户预存款明细失败[%d]!",SQLCODE);
						iRetCode = 132121;
						break;
					}
				}*/

				/* 现金款减少，专款项增加 */
				EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
				/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
					" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
					" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
					" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
					" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note,group_id,org_id) "
					" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
					" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, '0', 0-:v14, "
					" 0-:v15,0,0-:v16,0,0,0,0,0,0,'0',0,'00',:v17, :vGroupId, :vOrgId)",
					vTotalDate/100);
				EXEC SQL PREPARE sqlstr from :vSqlStr;
				EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
					:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
					:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vLimitFee, :vLimitFee, :vLimitFee,:iOpNote, :vGroupId, :vOrgId;
				if (SQLCODE != 0)
				{
					sprintf(retMsg, "记录用户缴费记录失败[%d]!", SQLCODE);
					iRetCode = 132122;
					break;
				}

				vTransFee = vLimitFee + vSendFee;

				EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
				/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
					" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
					" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
					" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
					" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note,group_id,org_id) "
					" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
					" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, :v14, :v15, "
					" :v16,0,:v17,0,0,0,0,0,0,'0',0,'00',:v18, :vGroupId, :vOrgId)",
					vTotalDate/100);
				EXEC SQL PREPARE sqlstr from :vSqlStr;
				EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
					:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
					:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vProdPayType, :vTransFee,:vTransFee, :vTransFee,:iOpNote, :vGroupId, :vOrgId;
				if (SQLCODE != 0)
				{
					sprintf(retMsg, "记录用户缴费记录失败[%d]!", SQLCODE);
					iRetCode = 132122;
					break;
				}

			} else
			{
				/* 专款帐户是现金帐户 */
			/*	EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
					prepay_fee = prepay_fee + :vSendFee,
					add_prepay = :vSendFee
				where contract_no = :stdCustMsg.contract_no
				and pay_type = :vProdPayType;
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(retMsg,"修改用户预存款明细失败[%d]!",SQLCODE);
					iRetCode = 132121;
					break;
				}

				if(SQLCODE == 1403)
				{
					EXEC SQL insert into dConMsgPre(contract_no,pay_type,prepay_fee,
						last_prepay,add_prepay,live_flag,allow_pay,begin_dt,end_dt)
					values( :stdCustMsg.contract_no, :vProdPayType,
						:vSendFee, 0,
						:vSendFee, '0', '0', :vTotalDate, 20200101);
					if(SQLCODE != 0)
					{
						sprintf(retMsg,"插入用户预存款明细失败[%d]!",SQLCODE);
						iRetCode = 132121;
						break;
					}
				}
          */

				vTransFee = vSendFee;  /* 现金帐户增加赠送金额 */
				EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
					/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
					" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
					" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
					" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
					" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note, group_id, org_id) "
					" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
					" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, :v14, :v15, "
					" :v16,0,:v17,0,0,0,0,0,0,'0',0,'00',:v18, :vGroupId, :vOrgId)",
					vTotalDate/100);
				EXEC SQL PREPARE sqlstr from :vSqlStr;
				EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
					:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
					:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vProdPayType, :vTransFee,:vTransFee, :vTransFee,:iOpNote, :vGroupId, :vOrgId;
				if (SQLCODE != 0)
				{
					sprintf(retMsg, "记录用户缴费记录失败[%d]!", SQLCODE);
					iRetCode = 132122;
					break;
				}
			}

        /* 回馈现金 finished */
	    } else if (grantDetType[0] == GRANT_DETTYPE_SENDSW) /* 回馈实物 */
	    {
	        pubLog_Debug(_FFL_, "sProductApplyFunc", "", "目前系统对实物不作记录！");

	    } else if (grantDetType[0] == GRANT_DETTYPE_SENDJF) /* 回馈积分(包括赠送和扣减)*/
	    {
/*****************2006-12-21 zhoubd屏蔽开始, 非本省业务**********
	        pubLog_Debug(_FFL_, "sProductApplyFunc", "", "回馈积分开始处理...");
            memset(&stsGrantMarkCode, 0, sizeof(stsGrantMarkCode));
            strncpy(stsGrantMarkCode.region_code, stdLoginMsg.org_code, 2);
            strcpy(stsGrantMarkCode.detail_code, detailCode);
            if (DbsSGRANTMARKCODE(DBS_FIND, &stsGrantMarkCode) != 0)
            {
                sprintf(retMsg, "回馈积分产品明细配置错误[%d]!", SQLCODE);
                iRetCode = 800002;
                break;
            }

            EXEC SQL insert into dMarkMsgHis
            (
                id_no,open_time,init_point,
				current_point,year_point,add_point,op_time,begin_time,
				total_used,total_prize,total_punish,lastyear_point,base_point,
				update_login,update_accept,update_date,
				update_time,update_code,update_type,sm_code
			)
			select
			    id_no,open_time,init_point,
				current_point,year_point, add_point, op_time,begin_time,
				total_used,total_prize,total_punish, lastyear_point, base_point,
				:iLoginNo,to_number(:vLoginAccept), :vTotalDate,
				to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :iOpCode, 'u', sm_code
			from dMarkMsg where id_no = :stdCustMsg.id_no;
		    if(SQLCODE != 0)
		    {
		    	sprintf(retMsg,"插入用户积分历史表失败[%d]",SQLCODE);
		    	iRetCode = 132111;
		    	break;
		    }

		    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "修改用户积分表 dMarkMsg ");
		    EXEC SQL update dMarkMsg
		    	set current_point = current_point + :stsGrantMarkCode.grant_mark,
		    		total_used = total_used + case when :stsGrantMarkCode.grant_mark > 0 then :stsGrantMarkCode.grant_mark else 0-:stsGrantMarkCode.grant_mark end,
		    		op_time = to_date(:vOpTime,'yyyymmdd hh24:mi:ss')
		    	where id_no = :stdCustMsg.id_no;
		    if(SQLCODE != 0 && SQLCODE != 1403)
		    {
		    	sprintf(retMsg,"修改用户积分表失败[%d]",SQLCODE);
		    	iRetCode = 132112;
		    	break;
		    } else if (SQLCODE == 1403 && stsGrantMarkCode.grant_mark >= 0)
		    {
				EXEC SQL insert into dMarkMsg
					(ID_NO, OPEN_TIME, INIT_POINT, CURRENT_POINT, YEAR_POINT,
					ADD_POINT, OP_TIME, BEGIN_TIME, TOTAL_USED, TOTAL_PRIZE,
					TOTAL_PUNISH, LASTYEAR_POINT, BASE_POINT, SM_CODE)
				values
					(
						:stdCustMsg.id_no, to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), 0, :stsGrantMarkCode.grant_mark, 0,
						0, to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
						case when :stsGrantMarkCode.grant_mark > 0 then :stsGrantMarkCode.grant_mark else 0-:stsGrantMarkCode.grant_mark end,
						0, 0, 0, 0, :stdCustMsg.sm_code
					);
				if (SQLCODE != 0)
				{
					sprintf(retMsg,"生成用户积分信息失败[%d]",SQLCODE);
		    		iRetCode = 132113;
		    		break;
				}
		    }

		    init(vMarkType);
		    strcpy(vMarkType, stsGrantMarkCode.mark_addtype);
		    lMarkUsed = stsGrantMarkCode.grant_mark;
		    if (stsGrantMarkCode.grant_mark < 0)
            {
                lMarkUsed = 0 - stsGrantMarkCode.grant_mark;
                strcpy(vMarkType, stsGrantMarkCode.mark_minustype);
            }

		    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vMarkType = [%s]", vMarkType);
		    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "lMarkUsed = [%ld]", lMarkUsed);
		    init(vSqlStr);
		    sprintf(vSqlStr,"update dCustMark%d%c set points = points+:v1 "
		    	 " where id_no=to_number(:v2) and mark_code = :v3 ",
		    	vTotalDate/100, iIdNo[strlen(iIdNo)-1]);
		    EXEC SQL PREPARE sqlstr from :vSqlStr;
		    EXEC SQL EXECUTE sqlstr using :lMarkUsed, :iIdNo, :vMarkType;
		    if(SQLCODE!=0 && SQLCODE!=1403)
		    {
		    	sprintf(retMsg,"修改dCustMark表失败[%d]!",SQLCODE);
		    	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vSqlStr = [%s]", vSqlStr);
		    	iRetCode = 132114;
		    	break;
		    }
		    else if(SQLCODE==1403)
		    {
		    	pubLog_Debug(_FFL_, "sProductApplyFunc", "", "插入dCustMark表");
		    	init(vSqlStr);
		    	sprintf(vSqlStr,"insert into dCustMark%d%c(id_no, "
		    			" year_month,mark_code,points,mark_status) "
		    		" values(to_number(:v1),floor(:v2/100), :v3 ,:v4,'0')",
		    		vTotalDate/100,iIdNo[strlen(iIdNo)-1]);
		    	EXEC SQL PREPARE sqlstr from :vSqlStr;
		    	EXEC SQL EXECUTE sqlstr using :iIdNo,:vTotalDate, :vMarkType, :lMarkUsed;
		    	if(SQLCODE!=0 && SQLCODE!=1403)
		    	{
		    	    pubLog_Debug(_FFL_, "sProductApplyFunc", "", "vSqlStr = [%s]", vSqlStr);
		    		sprintf(retMsg,"插入dCustMark表失败[%d]!",SQLCODE);
		    		iRetCode = 132115;
		    		break;
		    	}
		    }

            EXEC SQL insert into wMarkOpr(ID_NO , CUST_ID , LOGIN_ACCEPT , TOTAL_DATE ,
                 PHONE_NO , REGION_CODE , DISTRICT_CODE , SM_CODE ,FAVOUR_CODE ,
                 USE_POINT , FAVOUR_COUNT , OP_CODE ,
                 OP_TIME , LOGIN_NO , OP_NOTE )
            values(:stdCustMsg.id_no , :stdCustMsg.cust_id , to_number(:vLoginAccept) , :vTotalDate ,
                 :stdCustMsg.phone_no , substr(:stdCustMsg.belong_code,1,2) , substr(:stdCustMsg.belong_code,3,2),
                 :stdCustMsg.sm_code , :stsGrantMarkCode.mark_fav_code ,
                 :stsGrantMarkCode.grant_mark , 1 , :iOpCode ,
                 to_date(:vOpTime,'yyyymmdd hh24:mi:ss') , :iLoginNo, :iOpNote );
            if (SQLCODE != 0)
            {
                sprintf(retMsg,"插入wMarkOpr表出错[%d]!", SQLCODE);
                iRetCode =  125035 ;
                break;
            }
*****************2006-12-21 zhoubd屏蔽结束**********/

        /* 回馈积分finished */
	    } else if (grantDetType[0] == GRANT_DETTYPE_REQYC) /* 预存款处理 */
	    {
	        pubLog_Debug(_FFL_, "sProductApplyFunc", "", "暂时不处理预存款问题！");
	    }

	    EXEC SQL FETCH curGrant into :grantDetType, :detailCode;
	}
	EXEC SQL CLOSE curGrant;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "sProductApplyFunc", "", "sProductApplyFunc: End Execute");
#endif
	return iRetCode;

}


/*****************************************
函数说明： sProductCancelFunc
功    能： 缴费回馈/买手机赠话费取消（产品改造使用）
编写日期： 2005/10/20
****************************************/

int sProductCancelFunc( tGrantData  vGrantMsg, char *retMsg )
{
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "sProductCancelFunc", "", "sProductCancelFunc: Start Execute");
#endif
    int			i=0,j=0,k=0,ret=0;
	char		temp_buf[1000];		/*临时使用的字符串数组*/

	EXEC SQL BEGIN DECLARE SECTION;
	struct wd_dcustmsg_area stdCustMsg;
	struct wd_dloginmsg_area stdLoginMsg;
	struct wd_sgrantcfg_area stsGrantCfg;
	struct wd_sgrantmarkcode_area stsGrantMarkCode;
	struct wd_sgrantpredetcode_area stsGrantPreDetCode;
	struct wd_sgrantcfg_area stsGtantCfg;
	struct wd_spresentcfg_area stsPresentCfg;
	struct wd_spayuseflag_area stsPayUseFlag;
	struct wd_dmarkmsg_area stdMarkMsg;


		/* 输入参数 */
		char iOpCode[4+1];		/* 操作代码 */
		char iLoginNo[6+1];		/* 工号 */
		char iLoginPasswd[16+1];/* 工号密码 */
		char iOrgCode[9+1];		/* 工号机构编码 */
		char iIpAdd[15+1];		/* ip地址 */

		char iPhoneNo[15+1];	/* 手机号码 */
		char iContractNo[14+1];	/* 帐户 */
		char iPrepayFee[16+1];	/* 预存款 */
		char iModeCode[8+1];	/* 最底消费 */
		char iBigFlag[2+1];		/* 大客户等级 */
		char iCurrentPoint[10+1];/* 大客户等级 */
		char iOldSm[2+1];		/* 赠机后品牌 */
		char iNewSm[2+1];		/* 赠机后品牌 */
		char iSysNote[60+1];	/* 系统备注 */
		char iOpNote[60+1];		/* 用户备注 */
		char iGrantFlag[1+1];
		char iGrantCode[8+1];
		char iIdNo[14+1];
		char iMarkUsed[10];
		char iMarkAdd[10];
		char iSellCode[4+1];
		char iOpenTime[17+1];
		char iBunchNo[20+1];

		char vFPrepayFee[10];
		char vCFavMoney[10];
		char vFavFee[10];
		char vMonthFav[10];
		char vMonthNum[10];
		char vFavMoney[10];
		char vSendFlag[1+1];
		int   RetValue=0;
		char vLoginAccept[14+1];
		char vOldLoginAccept[14+1];
		char vOpTime[20+1];
		long vTotalDate=0;
		char vSmCode[7+1];
		char vBelongCode[7+1];
		long vCustId=0;
		char vSqlStr[1000];
		long vCurrentPoint=0;
		int  vFavourFee1=0;
		int  vFavourFee2=0;
		char vNowMode[8+1];
		char vOldAccept[14+1];
		char vEffectTime[17+1];
		char vPayType[1+1];
		char vRunCode[2+1];
		char  attr_code[8+1];
		char  credit_code[1+1];
		double limit_owe=0;
		char  region_code[2+1];
		float in_pay_money=0.00;
		float tmp_fee=0.00;
		double owe_fee2=0.00;
		double unbill_fee=0.00;
	    	double unbill_should=0.00;
	    	double unbill_favour=0.00;
	    	double noaccount_fee=0;
	    	long  in_id_no=0;
	    	char vphone_no[15+1];
	    	int vCount=0;
	    	char vRepFlag[1+1];
	    	char vPrepayFee[10];
        	char vPlanFee[10];
        	char vPlanMonths[10];
        	char vPlanFlag[1+1];
        	char vPlanAddFee[10];
        	char iLoginAccept[14+1];
	/* My defin begin */
	int		iRetCode = 0;
	char	grantDetType[1+1];
	char	detailCode[4+1];

	long	lMarkUsed = 0;
	char	vMarkType[1+1];
	double  vLimitFee = 0;  /* 起存金额，即最小配置金额beginMoney*/
	double  vSendFee = 0;   /* 送费金额 */
	double  vTransFee = 0;
	char	vProdPayType[1+1];
	long	lPayAccept = 0;
	double  vNowFee = 0;
	double  vProdNowFee = 0;
	char vOrgId[10+1];
	char vGroupId[10+1];


	EXEC SQL END DECLARE SECTION;

	/* 初始化变量 */
	init(iLoginAccept);
	init(vPlanAddFee);
	init(vPrepayFee);
	init(vPlanFee);
	init(vPlanMonths);
	init(vPlanFlag);
	init(vRepFlag);
	init(vphone_no);
	init(iOpCode);
	init(iLoginNo);
	init(iLoginPasswd);
	init(iOrgCode);
	init(iIpAdd);
	init(iPhoneNo);
	init(iContractNo);
	init(iPrepayFee);
	init(iModeCode);
	init(iBigFlag);
	init(iCurrentPoint);
	init(iOldSm);
	init(iNewSm);
	init(iSysNote);
	init(iOpNote);
	init(iGrantFlag);
	init(iGrantCode);
	init(iIdNo);
	init(iMarkUsed);
	init(iMarkAdd);
	init(iSellCode);
	init(iOpenTime);
	init(vFPrepayFee);
	init(vCFavMoney);
	init(vFavFee);
	init(vMonthFav);
	init(vMonthNum);
	init(vFavMoney);
	init(vSendFlag);
	init(vLoginAccept);
	init(vOldLoginAccept);
	init(vOpTime);
	init(vSmCode);
	init(vBelongCode);
	init(vSqlStr);
	init(vNowMode);
	init(vOldAccept);
	init(vEffectTime);
	init(vPayType);
	init(vRunCode);
	init(attr_code);
	init(credit_code);
	init(region_code);
	init(iBunchNo);
	init(vOrgId);
	init(vGroupId);

    /* 检查工号信息是否存在 */
    memset(&stdLoginMsg, 0, sizeof(stdLoginMsg));
    strcpy(stdLoginMsg.login_no, vGrantMsg.loginNo);
    if(DbsDLOGINMSG(DBS_FIND, &stdLoginMsg) != 0)
    {
        sprintf(retMsg, "工号[%s]不存在!", vGrantMsg.loginNo);
        return 800001;
    }

    /* 查询用户资料是否存在 */
    memset(&stdCustMsg, 0, sizeof(stdCustMsg));
    strcpy(stdCustMsg.phone_no, vGrantMsg.phoneNo);
    if(DbsDCUSTMSG2(DBS_FIND, &stdCustMsg) != 0)
    {
        sprintf(retMsg, "用户[%s]不存在!", vGrantMsg.phoneNo);
        return 800002;
    }

    if (vGrantMsg.applyType[0] == PROD_SENDFEE_CODE)
    {
        /* 检查缴费回馈代码配置是否正确 */
        memset(&stsGrantCfg, 0, sizeof(stsGtantCfg));
        strncpy(stsGrantCfg.region_code, stdLoginMsg.org_code, 2);
        strcpy(stsGrantCfg.det_mode_code, vGrantMsg.detModeCode);
        if (DbsSGRANTCFG(DBS_FIND, &stsGrantCfg) != 0)
        {
            sprintf(retMsg, "缴费回馈参数配置错误[%d]!", SQLCODE);
            return 800002;
        }
        strcpy(iGrantCode,      stsGrantCfg.grant_code);
        strcpy(iSellCode,       stsGrantCfg.sell_code);
		vLimitFee = stsGrantCfg.begin_money;

    } else if (vGrantMsg.applyType[0] == PROD_MACHFEE_CODE)
    {
        /* 检查买手机赠话费配置是否正确 */
        memset(&stsPresentCfg, 0, sizeof(stsPresentCfg));
        strncpy(stsPresentCfg.region_code, stdLoginMsg.org_code, 2);
        strcpy(stsPresentCfg.det_mode_code, vGrantMsg.detModeCode);
        if (DbsSPRESENTCFG(DBS_FIND, &stsPresentCfg) != 0)
        {
            sprintf(retMsg, "买手机赠话费参数配置错误[%d]!", SQLCODE);
            return 800002;
        }
        strcpy(iGrantCode,      stsPresentCfg.grant_code);
        strcpy(iSellCode,       "0000");
		vLimitFee = stsPresentCfg.begin_money;
    } else
    {
        sprintf(retMsg, "输入参数applyType=[%s]错误!", vGrantMsg.applyType);
        return 800009;
    }

	strcpy(iOpCode,       	vGrantMsg.opCode);
	strcpy(iLoginNo,       	stdLoginMsg.login_no);
	strcpy(iOrgCode,       	stdLoginMsg.org_code);
	strcpy(iIpAdd,       	stdLoginMsg.ip_address);
	strcpy(iOpNote,       	vGrantMsg.opNote);
	sprintf(iIdNo,    "%ld",  	stdCustMsg.id_no);
	strcpy(iPhoneNo,       	vGrantMsg.phoneNo);
	sprintf(iContractNo,  "%ld",   stdCustMsg.contract_no);
	strcpy(iOldSm,       	stdCustMsg.sm_code);
	strncpy(iBigFlag,       	stdCustMsg.attr_code + 3, 2);
	strcpy(iOpenTime,       stdCustMsg.open_time);
	strcpy(iPrepayFee,      "0");

	strcpy(iNewSm,       	stdCustMsg.sm_code);
	strcpy(iLoginAccept,	vGrantMsg.loginAccept);
	strcpy(iBunchNo,    vGrantMsg.machineId);

	Trim(iOpCode);
	Trim(iLoginNo);
	Trim(iLoginPasswd);
	Trim(iOrgCode);
	Trim(iIpAdd);
	Trim(iOpNote);
	Trim(iIdNo);
	Trim(iPhoneNo);
	Trim(iContractNo);
	Trim(iOldSm);
	Trim(iCurrentPoint);
	Trim(iBigFlag);
	Trim(iOpenTime);
	Trim(iPrepayFee);
	Trim(iGrantFlag);
	Trim(iGrantCode);
	Trim(iMarkUsed);
	Trim(iNewSm);
	Trim(iModeCode);
	Trim(iSellCode);
	Trim(iMarkAdd);

	strcpy(vOpTime, vGrantMsg.opTime);
	vTotalDate = atol( vGrantMsg.totalDate);
	strcpy(vLoginAccept,vGrantMsg.loginAccept);
	strcpy(vOldLoginAccept,vGrantMsg.oldLoginAccept);
	
	/*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/

       ret = sGetLoginOrgid(iLoginNo,vOrgId);
       if(ret <0)
         	{
         	  pubLog_Debug(_FFL_, "sProductCancelFunc", "", "获取用户org_id失败!");
         	  return 200919; 
         	}
       Trim(vOrgId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/
      /*--组织机构改造插入表字段edit by liuweib at 19/02/2009--begin*/
       ret =0;
       ret = sGetUserGroupid(iPhoneNo,vGroupId);
       if(ret <0)
         	{
         	  pubLog_Debug(_FFL_, "sProductCancelFunc", "", "获取集团用户group_id失败!");
         	  return 200919; 
         	}
       Trim(vGroupId);
      /*---组织机构改造插入表字段edit by liuweib at 19/02/2009---end*/
	

	if (vGrantMsg.applyType[0] == PROD_SENDFEE_CODE)
	{     
		    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
       /* EXEC SQL insert into dCustGrantMsgHis(ID_NO,PHONE_NO,CONTRACT_NO,BELONG_CODE,OLD_SM,
        	CURRENT_POINT,BIG_FLAG,OPEN_TIME,PREPAY_FEE,GRANT_FLAG,GRANT_CODE,
        	MARK_USED,NEW_SM,CHANNEL,LOGIN_ACCEPT,LOGIN_NO,OP_TIME,TOTAL_DATE,
        	OP_CODE,MARK_ADD,UPDATE_ACCEPT,UPDATE_TIME,UPDATE_DATE,UPDATE_LOGIN,
        	UPDATE_TYPE,UPDATE_CODE,group_id)
        select  ID_NO,PHONE_NO,CONTRACT_NO,BELONG_CODE,OLD_SM ,CURRENT_POINT,
        	BIG_FLAG,OPEN_TIME,PREPAY_FEE,GRANT_FLAG,GRANT_CODE,MARK_USED,
        	NEW_SM,CHANNEL,LOGIN_ACCEPT,LOGIN_NO,OP_TIME,TOTAL_DATE,OP_CODE,
        	MARK_ADD,:vLoginAccept,to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),:vTotalDate,:iLoginNo,
        	'd',:iOpCode,group_id
        from dCustGrantMsg
        where id_no = :stdCustMsg.id_no
        and login_accept = to_number(:vOldLoginAccept)
        and grant_code = :iGrantCode;
        if (SQLCODE != 0 && SQLCODE != 1403)
        {
		    sprintf(retMsg,"原始交易缴费回馈资料不存在[%d]!",SQLCODE);
	    	return 132104;
		}   */

		/* 删除缴费回馈资料失败 */
       /* EXEC SQL delete from dCustGrantMsg
        where id_no = :stdCustMsg.id_no
        and login_accept = to_number(:vOldLoginAccept)
        and grant_code = :iGrantCode;
       if (SQLCODE != 0 && SQLCODE != 1403)
        {
		    sprintf(retMsg,"原始交易缴费回馈资料不存在[%d]!",SQLCODE);
	    	return 132105;
		}    */

	} else if (vGrantMsg.applyType[0] == PROD_MACHFEE_CODE)
	{
	    /* 买手机赠话费资源处理 */
	    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
	    EXEC SQL insert into dBunchResHis
	    (
	        region_code,district_code,town_code,
			fetch_no,mach_code,bunch_no,resource_code,op_time,login_no,org_code,
			login_accept,total_date,bunch_type,update_accept,update_time,
			update_date,update_login,update_type,update_code,channel,group_id,org_id
		)
		select
		    region_code,district_code,town_code,
			fetch_no,mach_code,bunch_no,resource_code, op_time,login_no,org_code,
			login_accept,total_date,bunch_type,:vLoginAccept,
			to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
			:vTotalDate, :iLoginNo, 'u',:iOpCode, channel,group_Id,org_id
		from dBunchRes
		/**组织机构改造二期 edit by zhaoqm at 22/04/2009 -begin-
		where (region_code = substr(:stdLoginMsg.org_code,1,2) or region_code='99')
			and (district_code=substr(:stdLoginMsg.org_code,3,2)
				or district_code='99')
			and (town_code=substr(:stdLoginMsg.org_code,5,3) or town_code='999')
		*组织机构改造二期 edit by zhaoqm at 22/04/2009 -end-**/	
			where group_id 
				in (select parent_group_id from dchngroupinfo where group_id =:vOrgId)
			and bunch_no = :iBunchNo and bunch_type = '1'
			and resource_code ='0';
	    if(SQLCODE != 0)
	    {
	    	sprintf(retMsg,"机串号[%s]资源不存在[%d]!", iBunchNo, SQLCODE);
	    	return 130801;
	    }

	    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "修改dBunchRes表");
	    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
	    EXEC SQL update dBunchRes set region_code = substr(:stdLoginMsg.org_code,1,2),
				district_code=substr(:stdLoginMsg.org_code,3,2),
				town_code=substr(:stdLoginMsg.org_code,5,3), resource_code='1',
				op_time=to_date(:vOpTime,'yyyymmdd hh24:mi:ss'),
				login_no=:iLoginNo, org_code =:stdLoginMsg.org_code,
				org_id = :vOrgId,group_id = :vOrgId,
				login_accept = :vLoginAccept, total_date=:vTotalDate
			/**组织机构改造二期 edit by zhaoqm at 22/04/2009 -begin-		
			where (region_code=substr(:stdLoginMsg.org_code,1,2) or region_code='99')
				and (district_code=substr(:stdLoginMsg.org_code,3,2)
					or district_code='99')
				and (town_code=substr(:stdLoginMsg.org_code,5,3) or town_code='999')
			*组织机构改造二期 edit by zhaoqm at 22/04/2009 -end-**/	
			where group_id 
				in (select parent_group_id from dchngroupinfo where group_id =:vOrgId)
				and bunch_no=:iBunchNo and bunch_type='1'
				and resource_code ='0';
	    if(SQLCODE != 0)
	    {
	    	sprintf(retMsg,"修改手机串号资源表失败[%d]",SQLCODE);
	    	return 130801;
	    }
	}

	/* 获取原始交易的专款类型 */
	memset(vProdPayType, 0, sizeof(vProdPayType));
	memset(&stsPayUseFlag, 0, sizeof(stsPayUseFlag));
	strncpy(stsPayUseFlag.region_code, stdLoginMsg.org_code, 2);
	strcpy(stsPayUseFlag.grant_code, iGrantCode);
	if (DbsSPAYUSEFLAG(DBS_FIND, &stsPayUseFlag) != 0)
	{
	    strcpy(vProdPayType, "0");
	}else
	{
	    strcpy(vProdPayType, stsPayUseFlag.pay_type);
	}

	/* 处理缴费回馈、买手机送费配置明细 */
	iRetCode = 0;
	init(grantDetType); init(detailCode);
	EXEC SQL declare curCancelGrant CURSOR for
	select  add_type, detail_code  from sGrantCfgAdd
	where   region_code = substr(:stdLoginMsg.org_code, 1, 2)
	and     grant_code = :iGrantCode;
	EXEC SQL OPEN  curCancelGrant;
	EXEC SQL FETCH curCancelGrant into :grantDetType, :detailCode;
	if (SQLCODE != 0)
	{
	    sprintf(retMsg, "缴费回馈产品配置错误[%d]!", SQLCODE);
	    return 800004;
	}

	while(SQLCODE == 0)
	{
	    /**
	    * 根据不同的回馈明细配置进行处理,建议每种类型配置一条记录,
	    * 但程序支持同一种类型的多种回馈形式！
	    **/

	    if (grantDetType[0] == GRANT_DETTYPE_SENDXJ) /* 回馈现金 */
	    {
	        pubLog_Debug(_FFL_, "sProductCancelFunc", "", "回馈现金开始。。。");
	        /* 检查缴费送费配置信息是否存在 */
            memset(&stsGrantPreDetCode, 0, sizeof(stsGrantPreDetCode));
            strncpy(stsGrantPreDetCode.region_code, stdLoginMsg.org_code, 2);
            strcpy(stsGrantPreDetCode.detail_code, detailCode);
            if (DbsSGRANTPREDETCODE(DBS_FIND, &stsGrantPreDetCode) != 0)
            {
                sprintf(retMsg, "送费产品明细配置错误[%d]!", SQLCODE);
                iRetCode = 800002;
                break;
            }

            /* 确定已送费金额 */
            EXEC SQL select nvl(sum(fav_fee),0) into :vSendFee  from dCustPresent
            where   id_no = :stdCustMsg.id_no
            and     login_accept = to_number(:vOldLoginAccept);
            if (SQLCODE != 0 && SQLCODE != 1403)
            {
                sprintf(retMsg, "查询原始交易送费纪录失败[%d]!", SQLCODE);
                iRetCode = 800002;
                break;
            }
            pubLog_Debug(_FFL_, "sProductCancelFunc", "", "已送费金额 ＝ [%f]", vSendFee);
			/*记录dCustPresentHis表*/
			pubLog_Debug(_FFL_, "sProductCancelFunc", "", "记录dCustPresentHis表");
			/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
			EXEC SQL insert into dCustPresentHis
			(
			    id_no,phone_no,belong_code,fav_code,
				bunch_no,prepay_fee,fav_money,
				month_fav,month_num,
				contract_no,begin_time,
				end_time,fav_fee,login_accept,login_no,
				open_time,total_date,
				op_time,op_code,update_accept,
				update_time,update_date,update_login,
				update_type,update_code,sell_code,group_id)
			select
			    id_no,phone_no,belong_code,fav_code,
				bunch_no,prepay_fee,fav_money,
				month_fav,month_num,
				contract_no,begin_time,
				end_time,fav_fee,login_accept,login_no,
				open_time,total_date,
				op_time,op_code, :vLoginAccept,
				to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :vTotalDate, :iLoginNo,
				'd', :iOpCode, :iSellCode,group_id
			from dCustPresent
			where   id_no = :stdCustMsg.id_no
			and     login_accept = to_number(:vOldLoginAccept);
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				sprintf(retMsg,"插入用户赠送历史表失败[%d]!",SQLCODE);
				iRetCode = 132108;
				break;
			}

			EXEC SQL delete from    dCustPresent
			where   id_no = :stdCustMsg.id_no
			and     login_accept = to_number(:vOldLoginAccept);
			if(SQLCODE != 0 && SQLCODE != 1403)
			{
				sprintf(retMsg,"删除原始赠送交易失败[%d]!",SQLCODE);
				iRetCode = 132108;
				break;
			}

			/* 更新用户帐户信息 */
			EXEC SQL update dConMsg set
				prepay_fee = prepay_fee - :vSendFee,
				prepay_time = to_date(:vOpTime,'yyyymmdd hh24:mi:ss')
			where contract_no = :stdCustMsg.contract_no;
			if(SQLCODE != 0)
			{
				sprintf(retMsg,"用户默认帐户[%ld]资料不存在[%d]!",stdCustMsg.contract_no, SQLCODE);
				iRetCode = 132109;
				break;
			}

            /* 查询用户专款帐户余额 */
            vProdNowFee = 0;
         /*   EXEC SQL select prepay_fee into :vProdNowFee
            from    dConMsgPre
            where   contract_no = :stdCustMsg.contract_no
            and     pay_type = :vProdPayType;
            if (SQLCODE != 0)
            {
                sprintf(retMsg,"原始交易专款帐户不存在[%d]!",SQLCODE);
				iRetCode = 132109;
				break;
            }  */

			/* 如果专款不是现金的话，从现金扣除用户专款 */
			if (vProdPayType[0] != '0')
			{
			    if(vProdNowFee < vSendFee)
			    {
			        /* 查询现金帐户预存 */
			        /*EXEC SQL select prepay_fee into :vNowFee
                    from    dConMsgPre
                    where   contract_no = :stdCustMsg.contract_no
                    and     pay_type = '0';
                    if (SQLCODE != 0)
                    {
                        sprintf(retMsg,"用户没有现金帐户[%d]!",SQLCODE);
				        iRetCode = 132109;
				        break;
                    }  */

                    if (vNowFee < vSendFee - vProdNowFee)
                    {
                        sprintf(retMsg,"用户当前现金帐户余额不足，请补交赠送款[%.2f]元!",vSendFee - vProdNowFee-vNowFee);
				        iRetCode = 132109;
				        break;
                    }

                    /* 从用户现金帐户转款到专款帐户，专款金额为 vSendFee-vProdNowFee */
                    vTransFee = vSendFee-vProdNowFee;
				   /* EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
				    	prepay_fee = prepay_fee - :vTransFee,
				    	add_prepay = 0 - :vTransFee
				    where contract_no = :stdCustMsg.contract_no
				    and pay_type = '0';
				    if(SQLCODE!=0 && SQLCODE!=1403)
				    {
				    	sprintf(retMsg,"该用户没有现在帐户，修改用户预存款明细失败[%d]!",SQLCODE);
				    	iRetCode = 132120;
				    	break;
				    } */

				    /* 现金减少 */
				    EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
				    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				    sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
				    	" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
				    	" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
				    	" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
				    	" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note,group_id,org_id) "
				    	" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
				    	" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, '0', 0-:v14, "
				    	" 0-:v15,0,0-:v16,0,0,0,0,0,0,'0',0,'00',:v17,:vGroupId, :vOrgId)",
				    	vTotalDate/100);
				    EXEC SQL PREPARE sqlstr from :vSqlStr;
				    EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
				    	:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
				    	:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vTransFee, :vTransFee, :vTransFee,:iOpNote, :vGroupId, :vOrgId;
				    if (SQLCODE != 0)
				    {
				    	sprintf(retMsg, "记录用户缴费记录失败[%d]!", SQLCODE);
				    	iRetCode = 132122;
				    	break;
				    }

				    /* 专款帐户清零 */
				   /* EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
				    	prepay_fee = 0,
				    	add_prepay = 0 - prepay_fee
				    where contract_no = :stdCustMsg.contract_no
				    and pay_type = :vProdPayType;
				    if(SQLCODE!=0 && SQLCODE!=1403)
				    {
				    	sprintf(retMsg,"原始交易专款帐户不存在[%d]!",SQLCODE);
				    	iRetCode = 132120;
				    	break;
				    } */

				    EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
				    	/*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				    sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
				    	" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
				    	" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
				    	" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
				    	" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note, group_id, org_id) "
				    	" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
				    	" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, :v14, :v15, "
				    	" 0-:v16,0,0-:v17,0,0,0,0,0,0,'0',0,'00',0-:v18,:vGroupId, :vOrgId)",
				    	vTotalDate/100);
				    EXEC SQL PREPARE sqlstr from :vSqlStr;
				    EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
				    	:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
				    	:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vProdPayType, :vProdNowFee,:vProdNowFee, :vProdNowFee,:iOpNote, :vGroupId, :vOrgId;
				    if (SQLCODE != 0)
				    {
				    	sprintf(retMsg, "记录用户专款划拨缴费记录失败[%d]!", SQLCODE);
				    	iRetCode = 132122;
				    	break;
				    }

			    } else
			    {
			        /* 专款帐户金额大于赠送金额，直接扣除专款金额 */

			       /* EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
				    	prepay_fee = prepay_fee - :vSendFee,
				    	add_prepay = 0 - :vSendFee
				    where contract_no = :stdCustMsg.contract_no
				    and pay_type = :vProdPayType;
				    if(SQLCODE!=0 && SQLCODE!=1403)
				    {
				    	sprintf(retMsg,"原始交易专款帐户不存在[%d]!",SQLCODE);
				    	iRetCode = 132120;
				    	break;
				    }  */

				    EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
				    /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				    sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
				    	" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
				    	" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
				    	" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
				    	" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note,group_id, org_id) "
				    	" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
				    	" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, :v14, 0-:v15, "
				    	" 0-:v16,0,0-:v17,0,0,0,0,0,0,'0',0,'00',:v18, :vGroupId, :vOrgId)",
				    	vTotalDate/100);
				    EXEC SQL PREPARE sqlstr from :vSqlStr;
				    EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
				    	:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
				    	:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vProdPayType, :vSendFee,:vSendFee, :vSendFee,:iOpNote, :vGroupId, :vOrgId;
				    if (SQLCODE != 0)
				    {
				    	sprintf(retMsg, "记录用户专款划拨缴费记录失败[%d]!", SQLCODE);
				    	iRetCode = 132122;
				    	break;
				    }
			    }

			} else
			{
			    /* 查询现金帐户预存 */
			   /* EXEC SQL select prepay_fee into :vNowFee
                from    dConMsgPre
                where   contract_no = :stdCustMsg.contract_no
                and     pay_type = '0';
                if (SQLCODE != 0)
                {
                    sprintf(retMsg,"用户没有现金帐户[%d]!",SQLCODE);
				    iRetCode = 132109;
				    break;
                }
                if (vNowFee < vSendFee)
                {
                    sprintf(retMsg,"用户当前现金帐户金额小于送费金额，请补缴[%.2f]元!", vSendFee - vNowFee);
					iRetCode = 132121;
					break;
                }  */

				/* 专款帐户是现金帐户直接扣除赠送金额 */
				/*EXEC SQL update dConMsgPre set last_prepay = prepay_fee,
					prepay_fee = prepay_fee - :vSendFee,
					add_prepay = 0-:vSendFee
				where contract_no = :stdCustMsg.contract_no
				and pay_type = :vProdPayType;
				if(SQLCODE!=0 && SQLCODE!=1403)
				{
					sprintf(retMsg,"修改用户预存款明细失败[%d]!",SQLCODE);
					iRetCode = 132121;
					break;
				}  */

                EXEC SQL select sMaxSysAccept.nextVal into :lPayAccept from dual;
            /*组织机构改造插入表字段group_id和org_id  edit by liuweib at 19/02/2009*/
				    sprintf(vSqlStr,"insert into wPay%ld(contract_no,id_no,total_date, "
				    	" login_accept,con_cust_id,phone_no,belong_code,fetch_no,sm_code, "
				    	" login_no,org_code,op_time,op_code,pay_type,add_money,pay_money,"
				    	" out_prepay,prepay_fee,current_prepay,payed_owe,delay_fee, "
				    	" other_fee,dead_fee,bad_fee,back_flag,encrypt_fee,return_code, pay_note,group_id, org_id) "
				    	" values(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,"
				    	" to_date(:v12,'yyyymmdd hh24:mi:ss'),:v13, :v14, 0-:v15, "
				    	" 0-:v16,0,0-:v17,0,0,0,0,0,0,'0',0,'00',:v18, :vGroupId, :vOrgId)",
				    	vTotalDate/100);
				    EXEC SQL PREPARE sqlstr from :vSqlStr;
				    EXEC SQL EXECUTE sqlstr using :stdCustMsg.contract_no,:stdCustMsg.id_no,:vTotalDate,
				    	:lPayAccept,:stdCustMsg.cust_id,:stdCustMsg.phone_no,:stdCustMsg.belong_code, :iGrantCode,:stdCustMsg.sm_code,
				    	:iLoginNo,:iOrgCode,:vOpTime,:iOpCode, :vProdPayType, :vSendFee,:vSendFee, :vSendFee,:iOpNote, :vGroupId, :vOrgId;
				    if (SQLCODE != 0)
				    {
				    	sprintf(retMsg, "记录用户专款划拨缴费记录失败[%d]!", SQLCODE);
				    	iRetCode = 132122;
				    	break;
				    }
			}

        /* 回馈现金取消 finished */
	    } else if (grantDetType[0] == GRANT_DETTYPE_SENDSW) /* 回馈实物 */
	    {
	        pubLog_Debug(_FFL_, "sProductCancelFunc", "", "目前系统对实物不作记录！");

	    } else if (grantDetType[0] == GRANT_DETTYPE_SENDJF) /* 回馈积分(包括赠送和扣减)*/
	    {
/*****************2006-12-21 zhoubd屏蔽开始, 非本省业务**********
	        pubLog_Debug(_FFL_, "sProductCancelFunc", "", "回馈冲正积分开始处理...");
            memset(&stsGrantMarkCode, 0, sizeof(stsGrantMarkCode));
            strncpy(stsGrantMarkCode.region_code, stdLoginMsg.org_code, 2);
            strcpy(stsGrantMarkCode.detail_code, detailCode);
            if (DbsSGRANTMARKCODE(DBS_FIND, &stsGrantMarkCode) != 0)
            {
                sprintf(retMsg, "回馈积分产品明细配置错误[%d]!", SQLCODE);
                iRetCode = 800002;
                break;
            }

            if(stsGrantMarkCode.grant_mark > 0)
            {
                memset(&stdMarkMsg, 0, sizeof(stdMarkMsg));
                stdMarkMsg.id_no = stdCustMsg.id_no;
                iRetCode = DbsDMARKMSG(DBS_FIND, &stdMarkMsg);
                if(iRetCode != 0)
                {
                    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "用户积分信息不存在[%d]！", SQLCODE);
                    sprintf(retMsg, "用户积分信息不存在[%d]！", SQLCODE);
                    break;
                }

                if (stdMarkMsg.current_point < stsGrantMarkCode.grant_mark)
                {
                    sprintf(retMsg, "用户当前积分[%ld]小于赠送积分[%ld]!",stdMarkMsg.current_point,stsGrantMarkCode.grant_mark );
                    iRetCode = 600001;
                    break;
                }
            }

            EXEC SQL insert into dMarkMsgHis
            (
                id_no,open_time,init_point,
				current_point,year_point,add_point,op_time,begin_time,
				total_used,total_prize,total_punish,lastyear_point,base_point,
				update_login,update_accept,update_date,
				update_time,update_code,update_type,sm_code
			)
			select
			    id_no,open_time,init_point,
				current_point,year_point, add_point, op_time,begin_time,
				total_used,total_prize,total_punish, lastyear_point, base_point,
				:iLoginNo,to_number(:vLoginAccept), :vTotalDate,
				to_date(:vOpTime,'yyyymmdd hh24:mi:ss'), :iOpCode, 'u', sm_code
			from dMarkMsg where id_no = :stdCustMsg.id_no;
		    if(SQLCODE != 0)
		    {
		    	sprintf(retMsg,"插入用户积分历史表失败[%d]",SQLCODE);
		    	iRetCode = 132111;
		    	break;
		    }

		    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "修改用户积分表 dMarkMsg ");
		    EXEC SQL update dMarkMsg
		    	set current_point = current_point - :stsGrantMarkCode.grant_mark,
		    		total_used = total_used + case when :stsGrantMarkCode.grant_mark < 0 then :stsGrantMarkCode.grant_mark else 0-:stsGrantMarkCode.grant_mark end,
		    		op_time = to_date(:vOpTime,'yyyymmdd hh24:mi:ss')
		    	where id_no = :stdCustMsg.id_no;
		    if(SQLCODE != 0)
		    {
		    	sprintf(retMsg,"修改用户积分表失败[%d]",SQLCODE);
		    	iRetCode = 132112;
		    	break;
		    }

		    init(vMarkType);
		    strcpy(vMarkType, stsGrantMarkCode.mark_minustype);
		    lMarkUsed = stsGrantMarkCode.grant_mark;
		    if (stsGrantMarkCode.grant_mark < 0)
            {
                lMarkUsed = 0 - stsGrantMarkCode.grant_mark;
                strcpy(vMarkType, stsGrantMarkCode.mark_addtype);
            }

		    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "vMarkType = [%s]", vMarkType);
		    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "lMarkUsed = [%ld]", lMarkUsed);
		    init(vSqlStr);
		    sprintf(vSqlStr,"update dCustMark%d%c set points = points+:v1 "
		    	 " where id_no=to_number(:v2) and mark_code = :v3 ",
		    	vTotalDate/100, iIdNo[strlen(iIdNo)-1]);
		    EXEC SQL PREPARE sqlstr from :vSqlStr;
		    EXEC SQL EXECUTE sqlstr using :lMarkUsed, :iIdNo, :vMarkType;
		    if(SQLCODE!=0 && SQLCODE!=1403)
		    {
		    	sprintf(retMsg,"修改dCustMark表失败[%d]!",SQLCODE);
		    	pubLog_Debug(_FFL_, "sProductCancelFunc", "", "vSqlStr = [%s]", vSqlStr);
		    	iRetCode = 132114;
		    	break;
		    }
		    else if(SQLCODE==1403)
		    {
		    	pubLog_Debug(_FFL_, "sProductCancelFunc", "", "插入dCustMark表");
		    	init(vSqlStr);
		    	sprintf(vSqlStr,"insert into dCustMark%d%c(id_no, "
		    			" year_month,mark_code,points,mark_status) "
		    		" values(to_number(:v1),floor(:v2/100), :v3 ,:v4,'0')",
		    		vTotalDate/100,iIdNo[strlen(iIdNo)-1]);
		    	EXEC SQL PREPARE sqlstr from :vSqlStr;
		    	EXEC SQL EXECUTE sqlstr using :iIdNo,:vTotalDate, :vMarkType, :lMarkUsed;
		    	if(SQLCODE!=0 && SQLCODE!=1403)
		    	{
		    	    pubLog_Debug(_FFL_, "sProductCancelFunc", "", "vSqlStr = [%s]", vSqlStr);
		    		sprintf(retMsg,"插入dCustMark表失败[%d]!",SQLCODE);
		    		iRetCode = 132115;
		    		break;
		    	}
		    }

            EXEC SQL insert into wMarkOpr(ID_NO , CUST_ID , LOGIN_ACCEPT , TOTAL_DATE ,
                 PHONE_NO , REGION_CODE , DISTRICT_CODE , SM_CODE ,FAVOUR_CODE ,
                 USE_POINT , FAVOUR_COUNT , OP_CODE ,
                 OP_TIME , LOGIN_NO , OP_NOTE )
            values(:stdCustMsg.id_no , :stdCustMsg.cust_id , to_number(:vLoginAccept) , :vTotalDate ,
                 :stdCustMsg.phone_no , substr(:stdCustMsg.belong_code,1,2) , substr(:stdCustMsg.belong_code,3,2),
                 :stdCustMsg.sm_code , :stsGrantMarkCode.mark_fav_code ,
                 0-:stsGrantMarkCode.grant_mark , 1 , :iOpCode ,
                 to_date(:vOpTime,'yyyymmdd hh24:mi:ss') , :iLoginNo, :iOpNote );
            if (SQLCODE != 0)
            {
                sprintf(retMsg,"插入wMarkOpr表出错[%d]!", SQLCODE);
                iRetCode =  125035 ;
                break;
            }
*****************2006-12-21 zhoubd屏蔽结束**********/
        /* 回馈积分finished */
	    } else if (grantDetType[0] == GRANT_DETTYPE_REQYC) /* 预存款处理 */
	    {
	        pubLog_Debug(_FFL_, "sProductCancelFunc", "", "暂时不处理预存款问题！");
	    }

	    EXEC SQL FETCH curCancelGrant into :grantDetType, :detailCode;
	}
	EXEC SQL CLOSE curCancelGrant;
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "sProductCancelFunc", "", "sProductCancelFunc: End Execute");
#endif
	return iRetCode;
}
/*
 * getProdUserSeq  取产品序列号
 *
 * 功能:取产品序列号;
 *
 * input参数1: getType，     类型标识；
                       1 ,   取产品目录节点代码序列号
 *
 * 返回参数:            < 0，  函数执行失败;
 *                      > 0，  函数执行成功，返回序列值；
 *
 */
/*long getProdUserSeq(int getType)
{
#ifdef _DEBUG_
       printf("getProdUserSeq: Start Execute\n");
#endif
    long result = -1;
    long iTmpBuf;

    switch(getType)
    {
        case 1:*//* 取得产品目录节点代码,流水号*/
             /*{
                 EXEC SQL SELECT ProductDirSeq.NEXTVAL INTO :iTmpBuf FROM DUAL;
                 if (SQLCODE != 0)
                 {
                       printf("取产品目录节点代码流水号失败[%d]!\n",SQLCODE);
                 }
                 else
                 {
                       result = iTmpBuf;
                 }
             }
             break;

        defualt:
             printf("类型表示无法识别[%d]!\n",getType);
             break;
    }

#ifdef _DEBUG_
       printf("getProdUserSeq: End Execute\n");
#endif
    return (result);
}*/
int endGrpAllProductService(TDeleteGrpUser iDelGrpUserMsg, char *retMsg){
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endGrpAllProductService", "", "endGrpAllProductService: Start Execute");
#endif

	EXEC SQL BEGIN DECLARE SECTION;
        char    loginAccept[22+1];              /*流水*/
        char    opCode[4+1];                    /*功能代码*/
        char    loginNo[6+1];                   /*操作工号*/
        char    opTime[17+1];                   /*操作时间      'YYYYMMDD HH24:MI:SS'*/
        char    grpIdNo[22+1];                  /*集团用户ID*/
        char    productCode[8+1];               /*集团产品代码*/
        char    effectTime[20+1];               /*成号关系的生效时间：dGrpUserMebMsg:begin_time*/
	EXEC SQL END DECLARE SECTION;

	memset(loginAccept, 0, sizeof(loginAccept));
	memset(opCode, 0, sizeof(opCode));
	memset(loginNo, 0, sizeof(loginNo));
	memset(opTime, 0, sizeof(opTime));
	memset(grpIdNo, 0, sizeof(grpIdNo));
	memset(productCode, 0, sizeof(productCode));
	memset(effectTime, 0, sizeof(effectTime));

	strcpy(loginAccept, iDelGrpUserMsg.loginAccept);
	strcpy(opCode,      iDelGrpUserMsg.opCode);
	strcpy(loginNo,     iDelGrpUserMsg.loginNo);
	strcpy(opTime,      iDelGrpUserMsg.opTime);
	strcpy(grpIdNo,     iDelGrpUserMsg.grpIdNo);
	strcpy(productCode, iDelGrpUserMsg.productCode);
	strcpy(effectTime,  iDelGrpUserMsg.effectTime);

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.loginAccept = [%s]", iDelGrpUserMsg.loginAccept);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.grpIdNo = [%s]", iDelGrpUserMsg.grpIdNo);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.opCode = [%s]", iDelGrpUserMsg.opCode);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.opTime = [%s]", iDelGrpUserMsg.opTime);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.loginNo = [%s]", iDelGrpUserMsg.loginNo);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.productCode = [%s]", iDelGrpUserMsg.productCode);
	pubLog_Debug(_FFL_, "endGrpAllProductService", "", "iDelGrpUserMsg.effectTime = [%s]", iDelGrpUserMsg.effectTime);
#endif

	/* 结束用户的定购关系 */
	/*EXEC SQL insert into dGrpSrvMsgHis(
		ID_NO, SERVICE_TYPE, SERVICE_CODE, PRICE_CODE, SRV_ORDER, PRODUCT_CODE, BEGIN_TIME, END_TIME, MAIN_FLAG, UPTO_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG, LOGIN_ACCEPT, LOGIN_NO, OP_CODE, OP_TIME,
	  UPDATE_LOGIN, UPDATE_ACCEPT, UPDATE_DATE, UPDATE_TIME, UPDATE_CODE, UPDATE_TYPE
	)
	select
		ID_NO, SERVICE_TYPE, SERVICE_CODE, PRICE_CODE, SRV_ORDER, PRODUCT_CODE, BEGIN_TIME, END_TIME, MAIN_FLAG, UPTO_TIME, USE_FLAG, UNVAILD_TIME, PERSON_FLAG, LOGIN_ACCEPT, LOGIN_NO, OP_CODE, OP_TIME,
		:loginNo, to_number(:loginAccept),
		to_number(substr(:opTime,1,8)), to_date(:opTime,'yyyymmdd hh24:mi:ss'),
		:opCode, 'd'
	from dGrpSrvMsg
	where id_no = to_number(:grpIdNo)
	and	  end_time > sysdate;
	if(SQLCODE != 0){
		sprintf(retMsg, "Ins dGrpUserMsgHis Error: sqlcode =[%d]\n", SQLCODE);
		pubLog_Debug(_FFL_, "endGrpAllProductService", "", "Ins dGrpUserMsgHis Error: sqlcode =[%d]", SQLCODE);
		return 190090;
	}

	EXEC SQL update dGrpSrvMsg
	set	end_time = to_date(:effectTime,'yyyymmdd hh24:mi:ss'),
		LOGIN_ACCEPT = to_number(:loginAccept),
		LOGIN_NO = :loginNo,
		OP_CODE = :opCode,
		OP_TIME = to_date(:opTime,'yyyymmdd hh24:mi:ss')
	where id_no = to_number(:grpIdNo)
	and	  end_time > sysdate;
	if(SQLCODE != 0){
		sprintf(retMsg, "Update dGrpSrvMsg Error: sqlcode =[%d]\n", SQLCODE);
		pubLog_Debug(_FFL_, "endGrpAllProductService", "", "Update dGrpSrvMsg Error: sqlcode =[%d]", SQLCODE);
		return 190091;
	}*/
#ifdef _DEBUG_
       pubLog_Debug(_FFL_, "endGrpAllProductService", "", "endGrpAllProductService: End Execute");
#endif
	return 0;
}
