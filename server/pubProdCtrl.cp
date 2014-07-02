/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/**********已完成开户公共函数改造及统一日志改造 edit by quanzl at 07/05/2009**********/
#include "pubLog.h"
#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "prodParam.h"


#define SQLCODE  sqlca.sqlcode

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

int getProdCondList(char *iCtrlCode, CPRODCONDLIST *retParamList)
{
    int i = 0;
    char service_name [15+1] ; 
    EXEC SQL BEGIN DECLARE SECTION;
        char vCtrlCode[CTRL_CODE_LENGTH+1];
        CPRODCONDLIST vParamList;
    EXEC SQL END DECLARE SECTION;

    memset(vCtrlCode, 0, sizeof(vCtrlCode));
    memset(service_name, 0, sizeof(service_name));
    strcpy(vCtrlCode, iCtrlCode);
    strcpy(retParamList->function_code,  iCtrlCode);
    
    strcpy(service_name,"getProdCondList");

    /* 提取业务功能配置参数列表 */
    pubLog_Debug(_FFL_,service_name, "","get cBillParamNeed ctrlCode = [%s]", vCtrlCode);
    EXEC SQL SELECT distinct PARAM_CODE, FUNCPARAM_TYPE
        into :vParamList.param_code, :vParamList.value_type
        FROM cBillParamNeed
        WHERE  ctrl_code = :vCtrlCode;

    if(SQLCODE != 0 && SQLCODE != 1403)
    {
        return -1;
    }

    /* 业务功能配置参数个数 */
    vParamList.param_num = SQLROWS;
    retParamList->param_num = SQLROWS;

    for(i=0; i<vParamList.param_num; i++)
    {
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,service_name, "","param_code[%d] = [%d]", i, vParamList.param_code[i]);
#endif
        retParamList->value_type[i] = vParamList.value_type[i];
        retParamList->param_code[i] = vParamList.param_code[i];
    }
	
	pubLog_Debug(_FFL_,service_name, "","getFuncCondList End ....");
    return 0;

}

int setProdParamCode(char *iPhoneNo, char *iLoginNo, char *iOpCode, CPRODCONDLIST *paramList)
{
    int i=0, iDbsRet = 0;
    double   dPrePay = 0.00;
    tUserOwe stUserOwe;
    char service_name [15+1] ; 
    EXEC SQL BEGIN DECLARE SECTION;
    char   vIdNo[14+1];
    char   vPhoneNo[15+1];
    char   vModeCode[8+1];
    char   vNextModeCode[8+1];
    char   vModeName[60+1];
    char   vAccept[22+1];
    char   vRegionCode[2+1];

    struct wd_dcustmsg_area stdCustMsg;
    struct wd_dconmsg_area stdConMsg;
    struct wd_dcustdoc_area stdCustDoc;
	struct  wd_dmarkmsg_area stdMarkMsg;
	struct wd_dcustsim_area stdCustSim;
    EXEC SQL END DECLARE SECTION;
    
	memset(service_name, 0, sizeof(service_name));
    memset(vPhoneNo, 0, sizeof(vPhoneNo));
    memset(vRegionCode, 0, sizeof(vRegionCode));
    memset(vIdNo, 0, sizeof(vIdNo));
    memset(vModeCode, 0, sizeof(vModeCode));
    memset(vNextModeCode, 0, sizeof(vNextModeCode));
    memset(vAccept, 0, sizeof(vAccept));


    /* 提取用户资料 */
    memset(&stdCustMsg, 0, sizeof(stdCustMsg));
    strcpy(stdCustMsg.phone_no, iPhoneNo);
    strcpy(service_name,"setProdParamCode");
    iDbsRet = DbsDCUSTMSG2(DBS_FIND, &stdCustMsg);

    if(iDbsRet != 0)
    {
        sprintf(paramList->retCode, "%06d", iDbsRet);
        sprintf(paramList->retMsg, "用户[%s]资料不存在!", iPhoneNo);
        PUBLOG_DBDEBUG(service_name);
		pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
        return iDbsRet;
    }

    for (i=0; i<paramList->param_num; i++)
    {
    	pubLog_Debug(_FFL_,service_name, "","in setParamCode:paramList->param_code[%d] = [%d]", i, paramList->param_code[i]);
        switch(paramList->param_code[i])
        {
            case D_PARAM_PHONENO :
                strcpy(paramList->param_value[i], stdCustMsg.phone_no);
                break;
            case D_PARAM_LOGINNO :
                strcpy(paramList->param_value[i], iLoginNo);
                break;
            case D_PARAM_OPCODE :
                strcpy(paramList->param_value[i], iOpCode);
                break;
            case D_PARAM_CUSTID :
                sprintf(paramList->param_value[i], "%ld",stdCustMsg.cust_id);
                break;
            case D_PARAM_CONTRACTNO :
                sprintf(paramList->param_value[i],"%ld",stdCustMsg.contract_no);
                break;
            case D_PARAM_SMCODE :
                strcpy(paramList->param_value[i], stdCustMsg.sm_code);
                break;
            case D_PARAM_SIMNO:
                memset(&stdCustSim, 0, sizeof(stdCustSim));
                stdCustSim.id_no = stdCustMsg.id_no;
                iDbsRet = DbsDCUSTSIM(DBS_FIND,&stdCustSim);
                if(iDbsRet != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return iDbsRet;
                }
                strcpy(paramList->param_value[i], stdCustSim.sim_no);
                break;
            case D_PARAM_MODECODE :
                sprintf(vIdNo, "%ld", stdCustMsg.id_no);
#if PROVINCE_RUN == PROVINCE_HEILONGJIANG
                if(0 != pubModeCheck(vIdNo, paramList->param_value[i], vAccept, vNextModeCode, vAccept, paramList->retMsg))
                {
                    sprintf(paramList->retCode, "%06d", 1002);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1001;
                }
#else
                if(0 != pubBillCheck(vIdNo, paramList->param_value[i], vAccept, vNextModeCode, vAccept, paramList->retMsg))
                {
                    sprintf(paramList->retCode, "%06d", 1002);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1001;
                }
#endif
                break;
            case D_PARAM_REGIONCODE :
                strncpy(paramList->param_value[i], stdCustMsg.belong_code, 2);
                break;
            case D_PARAM_PAYCODE:
                memset(&stdConMsg, 0, sizeof(stdConMsg));
                stdConMsg.contract_no = stdCustMsg.contract_no;
                if( (iDbsRet = DbsDCONMSG(DBS_FIND, &stdConMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取帐户资料失败[%ld]", stdCustMsg.contract_no);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdConMsg.pay_code);
                break;
            case D_PARAM_ACCOUNTTYPE:
                memset(&stdConMsg, 0, sizeof(stdConMsg));
                stdConMsg.contract_no = stdCustMsg.contract_no;
                if( (iDbsRet = DbsDCONMSG(DBS_FIND, &stdConMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取帐户资料失败[%ld]", stdCustMsg.contract_no);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdConMsg.account_type);
                break;

            case  D_PARAM_OWNERGRADE: 
                memset(&stdCustDoc, 0, sizeof(stdCustDoc));
                stdCustDoc.cust_id = stdCustMsg.cust_id;
                if( (iDbsRet = DbsDCUSTDOC(DBS_FIND, &stdCustDoc)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取客户资料失败[%ld]", stdCustMsg.cust_id);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdCustDoc.owner_grade);
                
                break; 
 		    case  D_PARAM_MARK:
                memset(&stdMarkMsg, 0, sizeof(stdMarkMsg));
                stdMarkMsg.id_no=stdCustMsg.id_no;
          
                if( (iDbsRet = DbsDMARKMSG(DBS_FIND, &stdMarkMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取用户积分资料失败[%ld]", stdMarkMsg.id_no);
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
                }
                sprintf(paramList->param_value[i], "%ld", stdMarkMsg.current_point);

                break;

            case D_PARAM_PREPAY:
#if PROVINCE_RUN == PROVINCE_HEILONGJIANG            	
                if(PublicQueryLast(stdCustMsg.contract_no, &dPrePay) != 0)
                {
                    sprintf(paramList->retCode, "%06d", 1002);
                    sprintf(paramList->retMsg, "查询用户欠费失败!");
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
                }
                sprintf(paramList->param_value[i], "%.2f", dPrePay);
#else
				sprintf(vIdNo, "%ld", stdCustMsg.id_no);
				memset(&stUserOwe, 0, sizeof(stUserOwe));
				if(fGetUserOwe(vIdNo,&stUserOwe) != 0)
				{
					sprintf(paramList->retCode, "%06d", 1002);
                    sprintf(paramList->retMsg, "查询用户欠费失败!");
                    PUBLOG_DBDEBUG(service_name);
					pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                    return 1002;
				}
				sprintf(paramList->param_value[i], "%.2f", stUserOwe.totalPrepay - stUserOwe.totalOwe);
#endif                
                
                break;
            case D_PARAM_IDNO:
                sprintf(paramList->param_value[i], "%ld", stdCustMsg.id_no);
                break;
            case D_PARAM_ATTRCODE:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.attr_code);
                break;
            case D_PARAM_RUNCODE:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.run_code+1);
                break;
            case D_PARAM_OPENTIME:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.open_time);
                break;
            default:
                sprintf(paramList->retCode, "%06d", 999999);
                sprintf(paramList->retMsg, "参数表配置错误!");
                PUBLOG_DBDEBUG(service_name);
				pubLog_DBErr(_FFL_,service_name,paramList->retCode,paramList->retMsg);
                break;
        }
    }  /* end for */

#ifdef _DEBUG_
for (i=0; i<paramList->param_num; i++)
{
	pubLog_Debug(_FFL_,service_name, "","paramList[%d] = [%s]", paramList->param_code[i],paramList->param_value[i]);
}
#endif

	pubLog_Debug(_FFL_,service_name, "","setParamCode End ..... ");
    return iDbsRet;

}

int getParamCodeValue(CPRODCONDENTRY *retEntry, int paramCode, char *value)
{
    int i = 0,  existFlag = 0;
    char service_name [15+1] ; 
    memset(service_name, 0, sizeof(service_name));
    strcpy(service_name,"getParamCodeValue");
    for(i=0; i < retEntry->paramsOut; i++){
    	 pubLog_Debug(_FFL_,service_name, "","out_param: [%d] = [%s]",retEntry->cFuncParamOut[i].param_code,retEntry->cFuncParamOut[i].value);
         if(retEntry->cFuncParamOut[i].param_code == paramCode)
         {
                strcpy(value, retEntry->cFuncParamOut[i].value);
                existFlag = 1;
                break;
         }
    }

    if (existFlag == 0) return -1;

    return 0;
}

void setProdParam(CPRODCONDLIST paramList, CPRODCONDENTRY *retEntry)
{
    int  i = 0;
	char service_name [15+1] ; 
	memset(service_name, 0, sizeof(service_name));
	pubLog_Debug(_FFL_,service_name, "","setCheckParam begin .....");
    
    strcpy(retEntry->function_code, paramList.function_code);
    strcpy(service_name,"setProdParam");
    retEntry->paramsIn = paramList.param_num;
    for (i=0; i < paramList.param_num; i++)
    {
#ifdef _DEBUG_
	pubLog_Debug(_FFL_,service_name, "","in setCheckParam: [%d] [%s]", paramList.param_code[i],paramList.param_value[i]);
#endif
        retEntry->cFuncParamIn[i].param_code = paramList.param_code[i];
        retEntry->cFuncParamIn[i].value_type = paramList.value_type[i];
        strcpy(retEntry->cFuncParamIn[i].value, paramList.param_value[i]);
    }
    pubLog_Debug(_FFL_,service_name, "","setCheckParam End .....");
}

int fProdCheckOpr(char *phoneNo, char *iCtrlCode, char *opCode, char *loginNo, CPRODCONDENTRY *retEntry)
{

    int i=0, retCode = 0;
    char service_name [15+1] ; 
    CPRODCONDLIST paramList;
    EXEC SQL BEGIN DECLARE SECTION;
        char tCtrlCode[CTRL_CODE_LENGTH+1];
    EXEC SQL END DECLARE SECTION;
    memset(service_name, 0, sizeof(service_name));
    strcpy(service_name,"fProdCheckOpr");
#ifdef _DEBUG_
		pubLog_Debug(_FFL_,service_name, "","+fProdCheckOpr+ phoneNo = [%s]", phoneNo);
		pubLog_Debug(_FFL_,service_name, "","+fProdCheckOpr+ iCtrlCode = [%s]", iCtrlCode);
		pubLog_Debug(_FFL_,service_name, "","+fProdCheckOpr+ opCode = [%s]", opCode);
		pubLog_Debug(_FFL_,service_name, "","+fProdCheckOpr+ loginNo = [%s]", loginNo);
#endif
    EXEC SQL SELECT distinct ctrl_code into :tCtrlCode 
    	FROM cBillParamNeed WHERE ctrl_code = :iCtrlCode;
    if (SQLCODE != 0) {
      pubLog_Debug(_FFL_,service_name, "","+fProdCheckOpr+ NOT MATCH CtrlCode = [%s]", iCtrlCode);
      return 0;
    }
            	
    retCode = getProdCondList(iCtrlCode, &paramList);
    if(retCode != 0)
    {
      pubLog_Debug(_FFL_,service_name, "","getFuncCondList : Error.....[%d]", retCode);
      return retCode;
    }

    retCode = setProdParamCode(phoneNo, loginNo, opCode, &paramList);
    if(retCode != 0)
    {
      pubLog_Debug(_FFL_,service_name, "","setParamCode : Error.....[%d]", retCode);
      return 0;
    }

    setProdParam(paramList, retEntry);
    if( prodChk( retEntry ) != 0 )
    {
      return -1;
    }
    
	pubLog_Debug(_FFL_,service_name, "","fProdCheckOpr End .....");
    return retCode;
}
