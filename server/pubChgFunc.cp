/**********已完成增加group_id,org_id字段,开户公共函数改造及统一日志改造************/
/*
#include  "RBS_DEFINE.h"
#include  "wd_incl.h"
*/
/*******已完成组织结构二期改造 edit by  mengfy 07/05/2009  ****/
#include "pubLog.h"

#include "boss_srv.h"
#include "boss_define.h"
#include "publicsrv.h"
#include "funcParam.h"

#define SQLCODE  sqlca.sqlcode

#define _DEBUG_

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;


/* 提取业务限制代码 */
typedef struct {
    char  function_code[FUNCTION_CODE_LEN+1];
    int   param_num ;
    int   param_code[MAX_PARAMS+1];
    int   value_type[MAX_PARAMS+1];
    char  param_value[PARAM_VALUE_LEN+1][ PARAM_VALUE_LEN+1];
    char  retCode[6+1];
    char  retMsg[128+1];
}CFUNCCONDLIST;

int getFuncCondList(char *iFuncCode, CFUNCCONDLIST *retParamList)
{
    int i = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        char vFuncCode[FUNCTION_CODE_LEN+1];
        CFUNCCONDLIST vParamList;
    EXEC SQL END DECLARE SECTION;

    memset(vFuncCode, 0, sizeof(vFuncCode));
    strcpy(vFuncCode, iFuncCode);

    strcpy(retParamList->function_code,  iFuncCode);

    /* 提取业务功能配置参数列表 */
	pubLog_Debug(_FFL_, "getFuncCondList", "", "Func -> vFuncCode = [%s]", vFuncCode);
    memset(&vParamList, 0, sizeof(vParamList));
    EXEC SQL SELECT distinct PARAM_CODE, FUNCPARAM_TYPE
        into :vParamList.param_code, :vParamList.value_type
        FROM cFuncParamNeed
        WHERE  function_code = :vFuncCode;
    if(SQLCODE != 0 && SQLCODE != 1403)
    {
        /* return -1; chenxuan note 20070810 */      
        return 0;  
    }

    /* 业务功能配置参数个数 */
    vParamList.param_num = SQLROWS;
    retParamList->param_num = SQLROWS;

    for(i=0; i<vParamList.param_num; i++)
    {
#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "getFuncCondList", "", "param_code[%d] = [%d]", i, vParamList.param_code[i]);
#endif
        retParamList->value_type[i] = vParamList.value_type[i];
        retParamList->param_code[i] = vParamList.param_code[i];
    }

    pubLog_Debug(_FFL_, "getFuncCondList", "", " getFuncCondList End ....");
    return 0;

}

int getBillCondList(char *iCtrlCode, CFUNCCONDLIST *retParamList)
{
    int i = 0;
    EXEC SQL BEGIN DECLARE SECTION;
        char vFuncCode[FUNCTION_CODE_LEN+1];
        CFUNCCONDLIST vParamList;
    EXEC SQL END DECLARE SECTION;

    memset(vFuncCode, 0, sizeof(vFuncCode));
    strcpy(vFuncCode, iCtrlCode);

    strcpy(retParamList->function_code,  iCtrlCode);

    /* 提取业务功能配置参数列表 */
    memset(&vParamList, 0, sizeof(vParamList));
	pubLog_Debug(_FFL_, "getBillCondList", "", "Bill -> vFuncCode = [%s]", vFuncCode);
    EXEC SQL SELECT distinct PARAM_CODE, FUNCPARAM_TYPE
        into :vParamList.param_code, :vParamList.value_type
        FROM cBillParamNeed
        WHERE  ctrl_code = :vFuncCode;

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
	    pubLog_Debug(_FFL_, "getBillCondList", "", "param_code[%d] = [%d]", i, vParamList.param_code[i]);
#endif
        retParamList->value_type[i] = vParamList.value_type[i];
        retParamList->param_code[i] = vParamList.param_code[i];
    }

    pubLog_Debug(_FFL_, "getBillCondList", "", " getFuncCondList End ....");
    return 0;

}

#define  H_PARAM_SMCODE         0
#define  H_PARAM_MODECODE       1
#define  H_PARAM_REGIONCODE     2
#define  H_PARAM_PAYCODE        3
#define  H_PARAM_PAYTYPE        4
#define  H_PARAM_ACCOUNTTYPE    5
#define  H_PARAM_MACHINECODE    6
#define  H_PARAM_OWNERGRADE     7
#define  H_PARAM_IDNO           8
#define  H_PARAM_ATTRCODE       9
#define  H_PARAM_RUNCODE        10
#define  H_PARAM_ORGCODE        11
#define  H_PARAM_OPENTIME       12
#define  H_PARAM_SIMNO          13
#define  H_PARAM_PHONENO        14
#define  H_PARAM_CUSTID         15
#define  H_PARAM_CONTRACTNO     16
#define  H_PARAM_LOGINNO     	17
#define  H_PARAM_PREPAY         100
#define  H_PARAM_MARK           101

int setParamCode(char *iPhoneNo, char *loginNo, CFUNCCONDLIST *paramList)
{
    int i=0, iDbsRet = 0;
    double   dPrePay = 0.00;
    tUserMode userMode;
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
	/*zhoubd 2006-12-21 屏蔽, 定义变量current_point
    struct wd_dmarkmsg_area stdMarkMsg;
	*/
	long current_point = 0;
    struct wd_dcustsim_area stdCustSim;
    struct wd_dloginmsg_area stdLoginMsg;
    EXEC SQL END DECLARE SECTION;

    memset(vPhoneNo, 0, sizeof(vPhoneNo));
    memset(vRegionCode, 0, sizeof(vRegionCode));
    memset(vIdNo, 0, sizeof(vIdNo));
    memset(vModeCode, 0, sizeof(vModeCode));
    memset(vNextModeCode, 0, sizeof(vNextModeCode));
    memset(vAccept, 0, sizeof(vAccept));

    /* 提取用户资料 */
    memset(&stdCustMsg, 0, sizeof(stdCustMsg));
    strcpy(stdCustMsg.phone_no, iPhoneNo);
    iDbsRet = DbsDCUSTMSG2(DBS_FIND, &stdCustMsg);
    if(iDbsRet != 0 && iDbsRet != 1403)
    {
        sprintf(paramList->retCode, "%06d", iDbsRet);
        sprintf(paramList->retMsg, "用户[%s]资料不存在!", iPhoneNo);
        return iDbsRet;
    } else if (iDbsRet == 1403)
    {
        pubLog_Debug(_FFL_, "setParamCode", "", "用户[%s]没有入网！",iPhoneNo);
        iDbsRet = 0;
        sprintf(paramList->retCode, "%06d", 0);
        sprintf(paramList->retMsg, "用户[%s]资料不存在!", iPhoneNo);
    }

    pubLog_Debug(_FFL_, "setParamCode", "", "aaaaaaaaaaaaaa [%d] ", paramList->param_num);
    for (i=0; i<paramList->param_num; i++)
    {
        pubLog_Debug(_FFL_, "setParamCode", "", "in setParamCode:paramList->param_code[%d] = [%d]", i, paramList->param_code[i]);
        switch(paramList->param_code[i])
        {
            case H_PARAM_PHONENO :
                strcpy(paramList->param_value[i], stdCustMsg.phone_no);
                break;
            case H_PARAM_LOGINNO :
                strcpy(paramList->param_value[i], loginNo);
                break;
            case H_PARAM_ORGCODE :
                memset(&stdLoginMsg, 0, sizeof(stdLoginMsg));
		        strcpy(stdLoginMsg.login_no, loginNo);
		        DbsDLOGINMSG(DBS_FIND, &stdLoginMsg);
                strcpy(paramList->param_value[i], stdLoginMsg.org_code);
                break;
            case H_PARAM_CUSTID :
                sprintf(paramList->param_value[i], "%ld",stdCustMsg.cust_id);
                break;
            case H_PARAM_CONTRACTNO :
                sprintf(paramList->param_value[i],"%ld",stdCustMsg.contract_no);
                break;
            case H_PARAM_SMCODE :
                strcpy(paramList->param_value[i], stdCustMsg.sm_code);
                break;
            case H_PARAM_SIMNO:
                memset(&stdCustSim, 0, sizeof(stdCustSim));
                stdCustSim.id_no = stdCustMsg.id_no;
                iDbsRet = DbsDCUSTSIM(DBS_FIND,&stdCustSim);
                if(iDbsRet != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    return iDbsRet;
                }
                strcpy(paramList->param_value[i], stdCustSim.sim_no);
                break;
            case H_PARAM_MODECODE :
                memset(&userMode, 0, sizeof(userMode));
                if(0 != pubGetUserMode(iPhoneNo, &userMode))
                {
                    sprintf(paramList->retCode, "%06d", 1002);
                    return 1001;
                }
                strcpy(paramList->param_value[i], userMode.mode_code[0]);
                break;
            case H_PARAM_REGIONCODE :
                strncpy(paramList->param_value[i], stdCustMsg.belong_code, 2);
                break;
            case H_PARAM_PAYCODE:
                memset(&stdConMsg, 0, sizeof(stdConMsg));
                stdConMsg.contract_no = stdCustMsg.contract_no;
                if( (iDbsRet = DbsDCONMSG(DBS_FIND, &stdConMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取帐户资料失败[%ld]", stdCustMsg.contract_no);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdConMsg.pay_code);
                break;
            case H_PARAM_ACCOUNTTYPE:
                memset(&stdConMsg, 0, sizeof(stdConMsg));
                stdConMsg.contract_no = stdCustMsg.contract_no;
                if( (iDbsRet = DbsDCONMSG(DBS_FIND, &stdConMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取帐户资料失败[%ld]", stdCustMsg.contract_no);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdConMsg.account_type);
                break;

            case  H_PARAM_OWNERGRADE:
                memset(&stdCustDoc, 0, sizeof(stdCustDoc));
                stdCustDoc.cust_id = stdCustMsg.cust_id;
                if( (iDbsRet = DbsDCUSTDOC(DBS_FIND, &stdCustDoc)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取客户资料失败[%ld]", stdCustMsg.cust_id);
                    return 1002;
                }
                strcpy(paramList->param_value[i], stdCustDoc.owner_grade);

                break;
            case  H_PARAM_MARK:
            	/*zhoubd 2006-12-21 屏蔽*/
            	/*
                memset(&stdMarkMsg, 0, sizeof(stdMarkMsg));
                stdMarkMsg.id_no = stdCustMsg.id_no;
                if( (iDbsRet = DbsDMARKMSG(DBS_FIND, &stdMarkMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取用户积分资料失败[%ld]", stdMarkMsg.id_no);
                    return 1002;
                }
                sprintf(paramList->param_value[i], "%ld", stdMarkMsg.current_point);
                */

                /*zhoubd 2006-12-21 修改开始*/
                if ((iDbsRet = markGetCurrentPoint(1, ltoa(stdCustMsg.id_no), &current_point)) != 0)
                {
                	sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取用户积分资料失败[%ld]", stdCustMsg.id_no);
                    return 1002;
				}

				sprintf(paramList->param_value[i], "%ld", current_point);
				/*zhoubd 2006-12-21 修改结束*/
                break;

            case H_PARAM_PREPAY:
                memset(&stdConMsg, 0, sizeof(stdConMsg));
                stdConMsg.contract_no = stdCustMsg.contract_no;
                if( (iDbsRet = DbsDCONMSG(DBS_FIND, &stdConMsg)) != 0)
                {
                    sprintf(paramList->retCode, "%06d", iDbsRet);
                    sprintf(paramList->retMsg, "读取帐户资料失败[%ld]", stdCustMsg.contract_no);
                    return 1002;
                }
                sprintf(paramList->param_value[i], "%.2f", stdConMsg.prepay_fee);
                break;
            case H_PARAM_IDNO:
                sprintf(paramList->param_value[i], "%ld", stdCustMsg.id_no);
                break;
            case H_PARAM_ATTRCODE:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.attr_code);
                break;
            case H_PARAM_RUNCODE:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.run_code+1);
                break;
            case H_PARAM_OPENTIME:
                sprintf(paramList->param_value[i], "%s", stdCustMsg.open_time);
                break;
            default:
                sprintf(paramList->retCode, "%06d", 999999);
                sprintf(paramList->retMsg, "参数表配置错误!");
                break;
        }
    }  /* end for */

#ifdef _DEBUG_
	for (i=0; i<paramList->param_num; i++)
	{
	    pubLog_Debug(_FFL_, "setParamCode", "", "paramList[%d] = [%s]", paramList->param_code[i],paramList->param_value[i]);
	}
#endif

    pubLog_Debug(_FFL_, "setParamCode", "", "setParamCode End ..... ");
    return iDbsRet;

}
/*  取结构体数组 retEntry 中 与paramCode 对应的 value 值   --- mfy ---*/
int getValueByParamCode(CFUNCCONDENTRY *retEntry, int paramCode, char *value)
{
    int i = 0,  existFlag = 0;
    for(i=0; i < retEntry->paramsOut; i++){
         pubLog_Debug(_FFL_, "getValueByParamCode", "", "out_param: [%d] = [%s]",retEntry->cFuncParamOut[i].param_code,retEntry->cFuncParamOut[i].value);
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

/*从结构体 retEntry 为结构体 paramList 变量param_code、value_type、value 赋值*/
void setCheckParam(CFUNCCONDLIST paramList, CFUNCCONDENTRY *retEntry)
{
    int  i = 0;

    pubLog_Debug(_FFL_, "setCheckParam", "", "setCheckParam begin 0 .....");

	if(paramList.param_num > 0)
	{
    	strcpy(retEntry->function_code, paramList.function_code);
    	retEntry->paramsIn = paramList.param_num;
    	for (i=0; i < paramList.param_num; i++)
    	{
#ifdef _DEBUG_
			pubLog_Debug(_FFL_, "setCheckParam", "", "in setCheckParam: [%d] [%s]", paramList.param_code[i],paramList.param_value[i]);
#endif
        	retEntry->cFuncParamIn[i].param_code = paramList.param_code[i];
        	retEntry->cFuncParamIn[i].value_type = paramList.value_type[i];
        	strcpy(retEntry->cFuncParamIn[i].value, paramList.param_value[i]);
    	}
	}

    pubLog_Debug(_FFL_, "setCheckParam", "", "setCheckParam End ..... ");
}

/**
* Function: To contral user to change by phoneNo + opCode + loginNo
**/

int fCheckFuncAllow(char *phoneNo, char *loginNo, char *opCode, char *retMsg, CFUNCCONDENTRY *retEntry)
{
    int i=0, retCode = 0;
    CFUNCCONDLIST paramList;
    
	/*----组织结构二期改造 参数变量初始化 by mengfy 2009/05/08 begin----*/
	memset(&paramList,0,sizeof(paramList));
	/*----组织结构二期改造 参数变量初始化 by mengfy 2009/05/08 end ----*/

    retCode = getFuncCondList(opCode, &paramList);
    if(retCode != 0)
    {
        strcpy(retMsg, paramList.retMsg);
        pubLog_DBErr(_FFL_, "fCheckFuncAllow", "", "getFuncCondList : Error..... ");
        return retCode;
    }

    retCode = setParamCode(phoneNo, loginNo, &paramList);
    if(retCode != 0)
    {
        pubLog_DBErr(_FFL_, "fCheckFuncAllow", "", "setParamCode : Error..... ");
        strcpy(retMsg, paramList.retMsg);
        return retCode;
    }

	if(paramList.param_num > 0)
	{
		setCheckParam(paramList, retEntry);
		if( funcChk( retEntry ) != 0 )
		{
			return -1;
		}
	}

    pubLog_Debug(_FFL_, "fCheckFuncAllow", "", "fCheckFuncAllow End ..... ");

    return retCode;

}

/**
* FuncTion: contral user to change mode_code
**/
int fBillControl(char *phoneNo, char *loginNo, char *opCode, char *retMsg)
{
    CFUNCCONDENTRY retEntry;
    int i=0, retCode = 0;
    char tmpRetCode[6+1];
    CFUNCCONDLIST paramList;

	/*----组织结构二期改造 参数变量初始化 by mengfy 2009/05/08 begin----*/
	memset(&paramList,0,sizeof(paramList));
	/*----组织结构二期改造 参数变量初始化 by mengfy 2009/05/08 end ----*/

    retCode = getBillCondList(opCode, &paramList);
    if(retCode != 0)
    {
        strcpy(retMsg, paramList.retMsg);
        pubLog_DBErr(_FFL_, "fBillControl", "", "getFuncCondList : Error..... ");
        return retCode;
    }

    retCode = setParamCode(phoneNo, loginNo, &paramList);
    if(retCode != 0)
    {
		strcpy(retMsg, paramList.retMsg);
		pubLog_DBErr(_FFL_, "fBillControl", "", "setParamCode : Error..... [%d][%s]",retCode,retMsg);
		return retCode;
    }

    memset(&retEntry, 0, sizeof(retEntry));
    setCheckParam(paramList, &retEntry);

    if( prodBillChk( &retEntry ) != 0 )
    {
        memset(tmpRetCode, 0, sizeof(tmpRetCode));
        getValueByParamCode(&retEntry, 1001, retMsg);
        getValueByParamCode(&retEntry, 1000, tmpRetCode);
        retCode = atoi(tmpRetCode);
    }

    pubLog_Debug(_FFL_, "fBillControl", "", "fBillControl End ..... [%d][%s]", retCode, retMsg);

    return retCode;
}

int fCheckFuncOpr(char *phoneNo, char *loginNo, char *opCode, char *retMsg)
{
    CFUNCCONDENTRY retEntry;
    int i=0, retCode = 0;
    char tmpRetCode[6+1];
    CFUNCCONDLIST paramList;
    
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/
    memset(&paramList,0,sizeof(paramList));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

    retCode = getFuncCondList(opCode, &paramList);
    if(retCode != 0)
    {
        strcpy(retMsg, paramList.retMsg);
        pubLog_DBErr(_FFL_, "fCheckFuncOpr", "", "getFuncCondList: Error..... [%d][%s]",retCode,retMsg);
        return retCode;
    }

    retCode = setParamCode(phoneNo, loginNo, &paramList);
    if(retCode != 0)
    {
        strcpy(retMsg, paramList.retMsg);
        pubLog_DBErr(_FFL_, "fCheckFuncOpr", "", "fCheckFuncOpr->setParamCode : Error..... [%d][%s]",retCode,retMsg);
        return retCode;
    }

    memset(&retEntry, 0, sizeof(retEntry));
    setCheckParam(paramList, &retEntry);

    if( funcChk( &retEntry ) != 0 )
    {
        memset(tmpRetCode, 0, sizeof(tmpRetCode));
        getValueByParamCode(&retEntry, 1001, retMsg);
        getValueByParamCode(&retEntry, 1000, tmpRetCode);
        retCode = atoi(tmpRetCode);
    }

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "fCheckFuncOpr", "", "fCheckFuncOpr End ..... [%d][%s]", retCode, retMsg);
#endif

    return retCode;
}

int fGetOprContion(char *phoneNo, char *loginNo, char *opCode, char *retMsg, CFUNCCONDENTRY *retEntry)
{
    int i=0, retCode = 0;
    CFUNCCONDLIST paramList;
    
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/    
    memset(&paramList,0,sizeof(paramList));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 end ----*/

    retCode = getFuncCondList(opCode, &paramList);
    if(retCode != 0)
    {
		strcpy(retMsg, paramList.retMsg);
		pubLog_DBErr(_FFL_, "fGetOprContion", "", "getFuncCondList : Error..... [%d][%s]", retCode, retMsg);
        return retCode;
    }

    retCode = setParamCode(phoneNo, loginNo, &paramList);
    if(retCode != 0)
    {
        strcpy(retMsg, paramList.retMsg);
        pubLog_DBErr(_FFL_, "fGetOprContion", "", "setParamCode : Error..... [%d][%s]", retCode, retMsg);
        return retCode;
    }

    setCheckParam(paramList, retEntry);

    if( funcChk( retEntry ) != 0 )
    {
        return -1;
    }

    pubLog_Debug(_FFL_, "fGetOprContion", "", "fCheckFuncAllow End ..... [%d][%s]", retCode, retMsg);

    return retCode;

}



int pubGetUserMode(char *phone_no, tUserMode *userMode)
{
    EXEC SQL BEGIN DECLARE SECTION;
		int		iCount = 0;
        int     modeIndex = 0;
        int     iModeDetIndex = 0;
        long    idNo = 0;
        char    phoneNo[15+1];
        char    regionCode[2+1];
		char	smCode[2+1];
        char    mode_code[8+1];
        char    mode_name[60+1];
        char    mode_note[512+1];
        char    detail_code[4+1];
        char    detail_name[60+1];
        char    detail_type[1+1];
        char    detail_type_name[60+1];
        char    begin_time[17+1];
        char    end_time[17+1];
        char    login_accept[14+1];
        char    login_no[6+1];
		char	op_code[4+1]; 

        char    dynStmt[1024+1];
    EXEC SQL END DECLARE SECTION;

    memset(phoneNo , 0, sizeof(phoneNo));
    memset(regionCode, 0, sizeof(regionCode));
	memset(smCode , 0, sizeof(smCode));
    memset(mode_code , 0, sizeof(mode_code));
    memset(mode_name , 0, sizeof(mode_name));
    memset(mode_note , 0, sizeof(mode_note));
    memset(detail_code , 0, sizeof(detail_code));
    memset(detail_type , 0, sizeof(detail_type));
    memset(detail_name , 0, sizeof(detail_name));
    memset(detail_type_name , 0, sizeof(detail_type_name));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/    
    memset(begin_time , 0, sizeof(begin_time));
    memset(end_time , 0, sizeof(end_time));
	/*----组织机构二期改造 增加参数变量初始化 by mengfy @2009/05/07 begin ----*/    
    memset(login_accept , 0, sizeof(login_accept));
    memset(login_no , 0, sizeof(login_no));
	memset(op_code , 0, sizeof(op_code));

    strcpy(phoneNo, phone_no);
    EXEC SQL select id_no, substr(belong_code, 1, 2) into :idNo, :regionCode 
		    FROM    dCustMsg
		    WHERE   phone_no = :phoneNo
		    AND     substr(run_code, 2, 1) < 'a';
    if(SQLCODE != 0)
    {
		PUBLOG_DBDEBUG("pubGetUserMode");
        pubLog_DBErr(_FFL_, "pubGetUserMode", "", "in pubGetUserMode: sel dCustMsg Error [%d]", SQLCODE);
        return -1002;
    }

    /* 提取用户主资费信息 */
    memset(dynStmt, 0, sizeof(dynStmt));
    sprintf(dynStmt, "select a.mode_code, b.sm_code, b.mode_name, to_char(a.begin_time,'yyyymmdd hh24:mi:ss'), "
        " to_char(a.end_time,'yyyymmdd hh24:mi:ss'), to_char(login_accept), a.login_no, a.op_code, b.note "
        " from dBillCustDetail%ld a, sBillModeCode b "
        " where a.mode_code = b.mode_code  and   b.region_code = :v1 "
        " and   a.id_no = :v2 and   a.mode_time = 'Y' and a.mode_flag ='0' "
        " and   a.end_time > sysdate order by a.begin_time desc ", idNo%10);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "pubGetUserMode", "", "dynStmt = [%s]", dynStmt);
#endif
	EXEC SQL PREPARE sql_str FROM :dynStmt;
	EXEC SQL DECLARE modeCur CURSOR for sql_str;
	EXEC SQL OPEN modeCur using :regionCode, :idNo;
	modeIndex = 0;
	while (SQLCODE == 0)
	{
        EXEC SQL FETCH  modeCur into :mode_code, :smCode, :mode_name, :begin_time, :end_time, :login_accept, :login_no, :op_code, :mode_note;

        if(SQLCODE != 0)
		{
			PUBLOG_DBDEBUG("pubGetUserMode");
			pubLog_DBErr(_FFL_, "pubGetUserMode", "", "SQLCODE == [%d]", SQLCODE);
			break;
		}

        Trim(mode_name); Trim(mode_note); Trim(login_accept);
        sprintf(userMode->modeIdx[modeIndex], "%d", modeIndex);
		strcpy(userMode->sm_code[modeIndex], smCode);
        strcpy(userMode->mode_code[modeIndex], mode_code);
        strcpy(userMode->mode_name[modeIndex], mode_name);
        strcpy(userMode->begin_time[modeIndex], begin_time);
        strcpy(userMode->end_time[modeIndex], end_time);
        strcpy(userMode->login_accept[modeIndex], login_accept);
        strcpy(userMode->login_no[modeIndex], login_no);
		strcpy(userMode->op_code[modeIndex], op_code);
        strcpy(userMode->mode_note[modeIndex], mode_note);

        /* 提取用户资费明细信息 */
        memset(dynStmt, 0, sizeof(dynStmt));
        sprintf(dynStmt, "select a.mode_code, b.mode_name, a.detail_code, c.note, d.detail_type,d.TYPE_NAME, "
           " to_char(a.begin_time,'yyyymmdd hh24:mi:ss'),to_char(a.end_time,'yyyymmdd hh24:mi:ss'),to_char(login_accept), a.login_no "
           "  from dBillCustDetail%ld a, sBillModeCode b, sBillModeDetail c, sBillDetName d "
           "  where a.mode_code = b.mode_code and a.mode_code = c.mode_code "
           "  and   c.region_code = b.region_code and a.detail_code = c.detail_code "
           "  and   c.detail_type = d.detail_type  and   b.region_code = :v1 "
           "  and   a.id_no = :v2 and   a.begin_time >= to_date(:v3,'yyyymmdd hh24:mi:ss')  "
           "  and a.end_time <= to_date(:v4,'yyyymmdd hh24:mi:ss') "
           " order by a.begin_time ", idNo%10);
#ifdef _DEBUG_
	    pubLog_Debug(_FFL_, "pubGetUserMode", "", "dynStmt 2 = [%s]", dynStmt);
#endif
        EXEC SQL PREPARE sql_str2 FROM :dynStmt;
        EXEC SQL DECLARE modeCur2 CURSOR for sql_str2;
        EXEC SQL OPEN modeCur2 using :regionCode, :idNo, :begin_time, :end_time;

        iModeDetIndex = 0;
        while (SQLCODE == 0)
        {
           EXEC SQL FETCH  modeCur2 into :mode_code, :mode_name, :detail_code, :detail_name,
               :detail_type, :detail_type_name, :begin_time, :end_time, :login_accept, :login_no;

           if(SQLCODE != 0) break;

           Trim(mode_name);
           Trim(detail_name);
           Trim(detail_type_name);
           Trim(login_accept);

           strcpy(userMode->tDetail[modeIndex].mode_code[iModeDetIndex], mode_code);
           strcpy(userMode->tDetail[modeIndex].mode_name[iModeDetIndex], mode_name);
           strcpy(userMode->tDetail[modeIndex].detail_code[iModeDetIndex], detail_code);
           strcpy(userMode->tDetail[modeIndex].detail_name[iModeDetIndex], detail_name);
           strcpy(userMode->tDetail[modeIndex].detail_type[iModeDetIndex], detail_type);
           strcpy(userMode->tDetail[modeIndex].detail_type_name[iModeDetIndex], detail_type_name);
           strcpy(userMode->tDetail[modeIndex].begin_time[iModeDetIndex], begin_time);
           strcpy(userMode->tDetail[modeIndex].end_time[iModeDetIndex], end_time);
           strcpy(userMode->tDetail[modeIndex].login_accept[iModeDetIndex], login_accept);
           strcpy(userMode->tDetail[modeIndex].login_no[iModeDetIndex], login_no);

           iModeDetIndex++;
        }
        sprintf(userMode->tDetail[modeIndex].modeDetIndex[modeIndex], "%d",  iModeDetIndex);
        modeIndex++;

        EXEC SQL CLOSE modeCur2;

     }

     EXEC SQL CLOSE modeCur;

     sprintf(userMode->modeNum, "%d", modeIndex);

     if(modeIndex == 0) return -1; /* 取用户主资费信息失败 */

    return 0;
}

int pubGetUserModeDet(char *inIdNo, tUserMode *userMode)
{
    EXEC SQL BEGIN DECLARE SECTION;
		int     areaFlag = 0;
		int     iDeadFlag = 0;
		int     modeIndex = 0;
		int     iModeDetIndex = 0;
		long    idNo = atol(inIdNo);
		char    regionCode[2+1];
		char	smCode[2+1];
		char    mode_code[8+1];
		char    mode_name[60+1];
		char    mode_note[120+1];
		char    detail_code[4+1];
		char    detail_name[60+1];
		char    detail_type[1+1];
		char    detail_type_name[60+1];
		char    begin_time[17+1];
		char    end_time[17+1];
		char    login_accept[14+1];
		char    login_no[6+1];
		char    op_code[4+1];
		char	flag_code[10+1];
		char	flag_name[256+1];
		
		char    dynStmt[1024+1];
    EXEC SQL END DECLARE SECTION;

    memset(regionCode, 0, sizeof(regionCode));
	memset(smCode, 0, sizeof(smCode));
    memset(mode_code , 0, sizeof(mode_code));
    memset(mode_name , 0, sizeof(mode_name));
    memset(mode_note , 0, sizeof(mode_note));
    memset(detail_code , 0, sizeof(detail_code));
    memset(detail_type , 0, sizeof(detail_type));
    memset(detail_name , 0, sizeof(detail_name));
    memset(detail_type_name , 0, sizeof(detail_type_name));
    memset(begin_time , 0, sizeof(end_time));
    memset(login_accept , 0, sizeof(login_accept));
    memset(login_no , 0, sizeof(login_no));
	memset(op_code , 0, sizeof(op_code));
	memset(flag_code , 0, sizeof(flag_code));
	memset(flag_name , 0, sizeof(flag_name));


    EXEC SQL select substr(belong_code, 1, 2) into :regionCode
    FROM    dCustMsg
    WHERE   id_no = :idNo;
    if(SQLCODE != 0 && SQLCODE != 1403)
    {
		PUBLOG_DBDEBUG("pubGetUserModeDet");
        pubLog_DBErr(_FFL_, "pubGetUserModeDet", "", "in pubGetUserMode: sel dCustMsg Error [%d]", SQLCODE);
        return -2001;
    }else if(SQLCODE == 1403)
    {
        iDeadFlag = 1;
        EXEC SQL select substr(belong_code, 1, 2) into :regionCode
        FROM    dCustMsgDead
        WHERE   id_no = :idNo;
    }

    /* 提取用户主资费信息 */
    if(iDeadFlag == 0)
    {
        EXEC SQL insert into dCustModeDetTmp
        (
         ID_NO,DETAIL_TYPE,DETAIL_CODE,BEGIN_TIME,END_TIME,
         FAV_ORDER,MODE_CODE,MODE_FLAG,MODE_TIME,MODE_STATUS,
         LOGIN_ACCEPT,OP_CODE,TOTAL_DATE,OP_TIME,LOGIN_NO,REGION_CODE
        )
        select ID_NO,DETAIL_TYPE,DETAIL_CODE,BEGIN_TIME,END_TIME,
         FAV_ORDER,MODE_CODE,MODE_FLAG,MODE_TIME,MODE_STATUS,
         LOGIN_ACCEPT,OP_CODE,TOTAL_DATE,OP_TIME,LOGIN_NO,REGION_CODE
        from dBillCustDetail
        where  id_no = :idNo;
    }
    else
    {
        EXEC SQL insert into dCustModeDetTmp
        (
         ID_NO,DETAIL_TYPE,DETAIL_CODE,BEGIN_TIME,END_TIME,
         FAV_ORDER,MODE_CODE,MODE_FLAG,MODE_TIME,MODE_STATUS,
         LOGIN_ACCEPT,OP_CODE,TOTAL_DATE,OP_TIME,LOGIN_NO,REGION_CODE
        )
        select ID_NO,DETAIL_TYPE,DETAIL_CODE,BEGIN_TIME,END_TIME,
         FAV_ORDER,MODE_CODE,MODE_FLAG,MODE_TIME,MODE_STATUS,
         LOGIN_ACCEPT,OP_CODE,TOTAL_DATE,OP_TIME,LOGIN_NO,REGION_CODE
        from dBillCustDetailDead
        where  id_no = :idNo;
    }

#ifdef _DEBUG_
    pubLog_Debug(_FFL_, "pubGetUserModeDet", "", "Ins dCustModeDetTmp sqlcode DeadFlag [%d]= [%d] [%d] ", iDeadFlag, SQLCODE, SQLROWS);
#endif

	EXEC SQL DECLARE modeCurDet CURSOR for
        select a.mode_code, b.sm_code, b.mode_name, to_char(a.begin_time,'yyyymmdd hh24:mi:ss'),
        to_char(a.end_time,'yyyymmdd hh24:mi:ss'), to_char(login_accept), a.login_no, a.op_code, b.note
        from dCustModeDetTmp a, sBillModeCode b
        where a.mode_code = b.mode_code
        and   b.region_code = :regionCode
        and   a.mode_time = 'Y' and a.mode_flag ='0'
        order by a.begin_time desc;

#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "pubGetUserModeDet", "", "dynStmt = [%s]", dynStmt);
#endif

     EXEC SQL OPEN modeCurDet;
     modeIndex = 0;
     while (SQLCODE == 0)
     {
        EXEC SQL FETCH  modeCurDet into :mode_code, :smCode, :mode_name, :begin_time, :end_time, :login_accept, :login_no, :op_code, :mode_note;

        if(SQLCODE != 0 || modeIndex > USER_MODE_NUM) break;

        Trim(mode_name); Trim(mode_note); Trim(login_accept);
        sprintf(userMode->modeIdx[modeIndex], "%d", modeIndex);
		strcpy(userMode->sm_code[modeIndex], smCode);
        strcpy(userMode->mode_code[modeIndex], mode_code);
        strcpy(userMode->mode_name[modeIndex], mode_name);
        strcpy(userMode->begin_time[modeIndex], begin_time);
        strcpy(userMode->end_time[modeIndex], end_time);
        strcpy(userMode->login_accept[modeIndex], login_accept);
        strcpy(userMode->login_no[modeIndex], login_no);
		strcpy(userMode->op_code[modeIndex], op_code);
        strcpy(userMode->mode_note[modeIndex], mode_note);



        /* 提取用户资费明细信息 */
		/*if(modeIndex == 0) strcpy(begin_time, "19000101 00:00:00"); */
        EXEC SQL DECLARE modeCurDet2 CURSOR for
            select a.mode_code, b.mode_name, a.detail_code, c.note, d.detail_type,d.TYPE_NAME,
            to_char(a.begin_time,'yyyymmdd hh24:mi:ss'),to_char(a.end_time,'yyyymmdd hh24:mi:ss'),to_char(login_accept), a.login_no
            from dCustModeDetTmp a, sBillModeCode b, sBillModeDetail c, sBillDetName d
            where a.mode_code = b.mode_code and a.mode_code = c.mode_code
            and   c.region_code = b.region_code and a.detail_code = c.detail_code
            and   c.detail_type = d.detail_type  and   b.region_code = :regionCode
            and   a.begin_time >= to_date(:begin_time,'yyyymmdd hh24:mi:ss')
            and a.end_time <= to_date(:end_time,'yyyymmdd hh24:mi:ss')
			order by a.begin_time;
#ifdef _DEBUG_
		pubLog_Debug(_FFL_, "pubGetUserModeDet", "", "dynStmt 2 = [%s]", dynStmt);
#endif

        EXEC SQL OPEN modeCurDet2;

        iModeDetIndex = 0;
		areaFlag = 0;
        while (1)
        {
           EXEC SQL FETCH  modeCurDet2 into :mode_code, :mode_name, :detail_code, :detail_name,
               :detail_type, :detail_type_name, :begin_time, :end_time, :login_accept, :login_no;

           if(SQLCODE != 0) break;

           Trim(mode_name);
           Trim(detail_name);
           Trim(detail_type_name);
           Trim(login_accept);

           strcpy(userMode->tDetail[modeIndex].mode_code[iModeDetIndex], mode_code);
           strcpy(userMode->tDetail[modeIndex].mode_name[iModeDetIndex], mode_name);
           strcpy(userMode->tDetail[modeIndex].detail_code[iModeDetIndex], detail_code);
           strcpy(userMode->tDetail[modeIndex].detail_name[iModeDetIndex], detail_name);
           strcpy(userMode->tDetail[modeIndex].detail_type[iModeDetIndex], detail_type);
           strcpy(userMode->tDetail[modeIndex].detail_type_name[iModeDetIndex], detail_type_name);
           strcpy(userMode->tDetail[modeIndex].begin_time[iModeDetIndex], begin_time);
           strcpy(userMode->tDetail[modeIndex].end_time[iModeDetIndex], end_time);
           strcpy(userMode->tDetail[modeIndex].login_accept[iModeDetIndex], login_accept);
           strcpy(userMode->tDetail[modeIndex].login_no[iModeDetIndex], login_no);

			/* 提取小区代码
			memset(flag_code, 0, sizeof(flag_code));
			memset(flag_name, 0, sizeof(flag_name));
			EXEC SQL select b.flag_code, b.flag_code_name
				into :flag_code, :flag_name
			from dBillCustFlag a, sRateFlagCode b
			where a.id_no = :idNo
			and   a.region_code = b.region_code
			and   a.flag_code = b.flag_code
			and   a.detail_code = b.rate_code
			and   a.mode_code = :mode_code
			and   a.login_accept = to_number(:login_accept)
			and   a.begin_time = to_date(:begin_time, 'yyyymmdd hh24:mi:ss');
			*/
			Trim(flag_code);
			Trim(flag_name);
			if(SQLCODE != 0)
			{
				pubLog_Debug(_FFL_, "pubGetUserModeDet", "", "SqlCode = [%d] idNo = [%ld] mode_code [%s]", SQLCODE, idNo,mode_code);
				strcpy(flag_code, "无");
				strcpy(flag_name, "无");
			}
			strcpy(userMode->tDetail[modeIndex].flag_code[iModeDetIndex], flag_code);
			strcpy(userMode->tDetail[modeIndex].flag_name[iModeDetIndex], flag_name);

			iModeDetIndex++;
        }
        sprintf(userMode->tDetail[modeIndex].modeDetIndex[modeIndex], "%d",  iModeDetIndex);

        EXEC SQL CLOSE modeCurDet2;

		modeIndex++;
	}

	EXEC SQL CLOSE modeCurDet;

	sprintf(userMode->modeNum, "%d", modeIndex);

	if(modeIndex == 0) return -1; /* 取用户主资费信息失败 */

	return 0;
}

/**/
int getProdBillDet(char *iRegCode, char *iModeCode, char *oPrepay, char *oMachType)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char    detType[1+1];
	char    vBillDetCode[4+1];
	double  prepayFee = 0;
	double  maxPrepayFee = 0;
	char    machType[20+1];
	EXEC SQL END DECLARE SECTION;
	
	memset(vBillDetCode, 0, sizeof(vBillDetCode));
	memset(detType, 0, sizeof(detType));
	memset(machType, 0, sizeof(machType));
	
	EXEC SQL DECLARE curGetProdDetail CURSOR FOR
			select detail_type, detail_code from sBillModeDetail
			where  region_code = :iRegCode
			and	   mode_code = :iModeCode
			and    detail_type in ( 'C', 'E');

	/* C：交费送费 E: 买手机送话费； 由sBillDetName解释 */
	EXEC SQL OPEN curGetProdDetail;
	EXEC SQL fetch curGetProdDetail into :detType, :vBillDetCode;
	while (SQLCODE == 0)
	{
		prepayFee = 0;
		if (detType[0] == PROD_SENDFEE_CODE) /* 交费回馈 */
		{
			EXEC SQL select begin_money into :prepayFee
			from  sGrantCfg
			where region_code = :iRegCode
			and   det_mode_code = :vBillDetCode;
			pubLog_Debug(_FFL_, "getProdBillDet", "", "sel sGrantCfg = [%d]", SQLCODE);
		}

		if (detType[0] == PROD_MACHFEE_CODE) /* 买手机送话费 */
		{
		    EXEC SQL select begin_money, nvl(fav_code,' ') into :prepayFee, :machType
			from  sPresentCfg
			where region_code = :iRegCode
			and   det_mode_code = :vBillDetCode;
			pubLog_Debug(_FFL_, "getProdBillDet", "", "sel sPresentCfg  = [%d]", SQLCODE);
			Trim(machType);
		}

		/*
		if (maxPrepayFee < prepayFee )
		{
		maxPrepayFee = prepayFee;
		}
		*/

		maxPrepayFee = maxPrepayFee + prepayFee;

		init(detType); init(vBillDetCode);
		EXEC SQL fetch curGetProdDetail into :detType, :vBillDetCode;
   }

   EXEC SQL CLOSE curGetProdDetail;
   sprintf(oPrepay, "%.2f", maxPrepayFee);
   strcpy(oMachType, machType);

#ifdef _DEBUG_
   pubLog_Debug(_FFL_, "getProdBillDet", "", "in getProdBillDet [%d][%s][%s] [%s][%s]", SQLCODE, iRegCode, iModeCode, oPrepay, oMachType);
#endif

   return  0;
}

#define _PRODDEBUG_

void getErrMsg(const char *iECode, char *oEMsg)
{
#ifdef _PRODDEBUG_
    pubLog_Debug(_FFL_, "getErrMsg", "", "in getErrMsg retCode = [%s][%s]", iECode, oEMsg);
#endif
	struct	wd_ssyserrmsg_area	stErrMsg;

	memset(&stErrMsg, 0, sizeof(stErrMsg));

	strncpy(stErrMsg.errcode, iECode, 6);
	if (DbsSSYSERRMSG(DBS_FIND, &stErrMsg) == 0)
	{
		Trim(stErrMsg.errmsg);
		strcpy(oEMsg, stErrMsg.errmsg);
	}
}

/**/
int getLastPayMoney(long lContractNo, char const *cTotalDate, char *oPayMoney)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char	dynStmt[1024+1];
	float	dPayMoney = 0;
	char	vTotalDate[6+1];
	EXEC SQL END   DECLARE SECTION;

 	memset(vTotalDate, 0, sizeof(vTotalDate));
	memset(dynStmt, 0, sizeof(dynStmt));

	strcpy(vTotalDate,cTotalDate);
	Trim(vTotalDate);
	
	sprintf(dynStmt, "select nvl(sum(pay_money), 0) from wPay%s "
		" where contract_no = %ld  and total_date = %s "
		" and   pay_type in ('0')  and op_code in ('1302', '1300')" ,
		vTotalDate, lContractNo, vTotalDate);
#ifdef _DEBUG_
	pubLog_Debug(_FFL_, "getLastPayMoney", "", "in getLastPayMoney: dynStmt = [%s]", dynStmt);
#endif
	EXEC  SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :dynStmt into :dPayMoney;
	END;
	END-EXEC;

	sprintf(oPayMoney, "%.2f", dPayMoney);

	return 0;
}

/*查询dBillCustDetail 表中是否有 id_no = inIdNo 的数据 ; 如果有，返回0 ; 否则，返回 -1;*/
int pubCheckBillMsg(const char *inIdNo)
{
	EXEC SQL BEGIN DECLARE SECTION;
	char	vIdNo[22+1];
	char	vSqlStmt[256+1];
	int		iCount = 0;
	EXEC SQL END   DECLARE SECTION;

	memset(vIdNo, 0, sizeof(vIdNo));
	memset(vSqlStmt, 0, sizeof(vSqlStmt));

	strncpy(vIdNo, inIdNo, sizeof(vIdNo)-1);

	sprintf(vSqlStmt, "select count(*) from dBillCustDetail%ld "
		" where id_no = to_number(:v1) ", atol(vIdNo)%10);

	EXEC SQL EXECUTE
	BEGIN
		EXECUTE IMMEDIATE :vSqlStmt into :iCount USING :vIdNo;
	END;
    END-EXEC;

	if(iCount == 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}

}
