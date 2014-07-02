#include "publicsrv.h"
#include <funcParam.h>
/**
#include <sqlcpr.h>
#include <oraca.h>
**/

EXEC SQL INCLUDE SQLCA;

int paramCodeComp(const void *v1,const void *v2)
{
	return(((CFUNCPARAM *)v1)->param_code-((CFUNCPARAM *)v2)->param_code);
}

int op_in(char *in,char *set)
{
	char *p;

	p=strtok(set,PARAM_VALUE_SEP);
	while(p)
	{
		if(strcmp(in,p)==0) 
			return 1;
		p=strtok(NULL,PARAM_VALUE_SEP);
	}
	return 0;
}


int op_sql(char *value,char *dynstmt)
{
	int ret;
	EXEC SQL WHENEVER SQLERROR GOTO OP_END;
	EXEC SQL WHENEVER NOT FOUND GOTO OP_END;
	EXEC SQL PREPARE S FROM :dynstmt;
	EXEC SQL DECLARE C CURSOR FOR S;
	EXEC SQL OPEN C USING :value;
	EXEC SQL FETCH C INTO :ret;
	EXEC SQL CLOSE C;
	return ret;
OP_END:
	printf("OP_SQL_ERROR: \n%.*s\n", sqlca.sqlerrm.sqlerrml, sqlca.sqlerrm.sqlerrmc);
	EXEC SQL WHENEVER SQLERROR continue;
	EXEC SQL WHENEVER NOT FOUND continue;
	EXEC SQL CLOSE C;
	return 0;	
}

int funcCondMatch(CFUNCCONDDETAIL *cFuncCondDetail,int conds,CFUNCCONDENTRY *cFuncCondEntry)
{
	int i,j;

    printf("conds = [%d] [%d]\n", conds, cFuncCondEntry->paramsIn);
	if(conds>cFuncCondEntry->paramsIn)
		return 0;

	for(i=0,j=0 ; i<conds && j<cFuncCondEntry->paramsIn ; i++,j++){

        printf("cFuncCondDetail[i].param_code [%d] [%d]\n", cFuncCondDetail[i].param_code,cFuncCondEntry->cFuncParamIn[j].param_code);
		if(cFuncCondDetail[i].param_code<cFuncCondEntry->cFuncParamIn[j].param_code){
			return 0;
		}
		else if(cFuncCondDetail[i].param_code>cFuncCondEntry->cFuncParamIn[j].param_code){
			i--;
			continue;
		}
		printf("cFuncCondDetail[%d].operator = [%d] cFuncCondDetail[i].value_type=[%d]\n", i, cFuncCondDetail[i].operator1,cFuncCondDetail[i].value_type);
		switch(cFuncCondDetail[i].operator1){
		case 0:	/*--- == ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
			case 1: /*--- int ---*/
			case 2: /*--- double ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)!=0)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 1: /*--- != ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
			case 1: /*--- int ---*/
			case 2: /*--- double ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)==0)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 2: /*--- > ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)<=0)
					return 0;
				break;
			case 1: /*--- int ---*/
				if(atoi(cFuncCondEntry->cFuncParamIn[j].value)<=atoi(cFuncCondDetail[i].value1))
					return 0;
				break;
			case 2: /*--- double ---*/
				if(atof(cFuncCondEntry->cFuncParamIn[j].value)<=atof(cFuncCondDetail[i].value1))
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 3: /*--- < ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)>=0)
					return 0;
				break;
			case 1: /*--- int ---*/
				if(atoi(cFuncCondEntry->cFuncParamIn[j].value)>=atoi(cFuncCondDetail[i].value1))
					return 0;
				break;
			case 2: /*--- double ---*/
				if(atof(cFuncCondEntry->cFuncParamIn[j].value)>=atof(cFuncCondDetail[i].value1))
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 4: /*--- >= ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)<0)
					return 0;
				break;
			case 1: /*--- int ---*/
				if(atoi(cFuncCondEntry->cFuncParamIn[j].value)<atoi(cFuncCondDetail[i].value1))
					return 0;
				break;
			case 2: /*--- double ---*/
				if(atof(cFuncCondEntry->cFuncParamIn[j].value)<atof(cFuncCondDetail[i].value1))
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 5: /*--- <= ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)>0)
					return 0;
				break;
			case 1: /*--- int ---*/
				if(atoi(cFuncCondEntry->cFuncParamIn[j].value)>atoi(cFuncCondDetail[i].value1))
					return 0;
				break;
			case 2: /*--- double ---*/
				if(atof(cFuncCondEntry->cFuncParamIn[j].value)>atof(cFuncCondDetail[i].value1))
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 6: /*--- [) ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)<0 || strcmp(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value2)>0)
					return 0;
				break;
			case 1: /*--- int ---*/
				if(atoi(cFuncCondEntry->cFuncParamIn[j].value)<atoi(cFuncCondDetail[i].value1) || atoi(cFuncCondEntry->cFuncParamIn[j].value)>atoi(cFuncCondDetail[i].value2))
					return 0;
				break;
			case 2: /*--- double ---*/
				if(atof(cFuncCondEntry->cFuncParamIn[j].value)<atof(cFuncCondDetail[i].value1) || atof(cFuncCondEntry->cFuncParamIn[j].value)>atof(cFuncCondDetail[i].value2))
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 7: /*--- like ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
				if(strstr(cFuncCondDetail[i].value1,cFuncCondEntry->cFuncParamIn[j].value)!=cFuncCondDetail[i].value1)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 8:	/*--- in ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
			case 1: /*--- int ---*/
			case 2: /*--- double ---*/
				if(op_in(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)!=1)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 9:	/*--- not in ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
			case 1: /*--- int ---*/
			case 2: /*--- double ---*/
				if(op_in(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)==1)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		case 10:	/*--- sql ---*/
			switch(cFuncCondDetail[i].value_type){
			case 0: /*--- char ---*/
			case 1: /*--- int ---*/
			case 2: /*--- double ---*/
				if(op_sql(cFuncCondEntry->cFuncParamIn[j].value,cFuncCondDetail[i].value1)<=0)
					return 0;
				break;
			default:
				return 0;
			}
			break;
		default:
			return 0;

		}
	}
	
	return 1;
	
}

int getFuncParam(CFUNCCONDENTRY *cFuncCondEntry,int funcparamType)
{
	char funcCode[FUNCTION_CODE_LEN+1];
	int entrySerial;
	CFUNCCONDDETAIL cFuncCondDetail[MAX_PARAMS];
	CFUNCPARAM funcParam;
	int conds,i;
	int match;
	
	EXEC SQL WHENEVER SQLERROR GOTO DB_ERROR;

	/**	oraca.orastxtf = ORASTFERR; **/
		
	/*--- 初始化 ---*/
	/*--- 输入参数排序 ---*/
	
	qsort(cFuncCondEntry->cFuncParamIn, cFuncCondEntry->paramsIn, sizeof(CFUNCPARAM), paramCodeComp);

	/*--- 匹配条件 ---*/
	strcpy(funcCode,cFuncCondEntry->function_code);

	EXEC SQL DECLARE curFuncCondEntry CURSOR FOR select serial from cFuncCondEntry where function_code=:funcCode and funcparam_type=:funcparamType order by serial;

	EXEC SQL DECLARE curFuncCondDetail CURSOR FOR select a.param_code,value_type,value_length,operator,value1,nvl(value2,'0') from sParamCode a,cFuncCondDetail b where a.param_code=b.param_code  and function_code=:funcCode and serial=:entrySerial order by a.param_code;

	EXEC SQL DECLARE curFuncParam CURSOR FOR select a.param_code,order_code,value_type,value_length,value from sParamCode a,cFuncParam b where a.param_code=b.param_code and function_code=:funcCode and serial=:entrySerial order by a.param_code,order_code;

	EXEC SQL WHENEVER NOT FOUND DO break;

	EXEC SQL OPEN curFuncCondEntry;
	match=0;
	for(;;){
		EXEC SQL FETCH curFuncCondEntry into :entrySerial;
		EXEC SQL OPEN curFuncCondDetail;
		for(conds=0;;conds++){
			EXEC SQL FETCH curFuncCondDetail into :cFuncCondDetail[conds];
			rtrim(cFuncCondDetail[conds].value1);
			rtrim(cFuncCondDetail[conds].value2);

		}
		EXEC SQL CLOSE curFuncCondDetail;
		/*--- 条件判断 ---*/
		if(match=funcCondMatch(cFuncCondDetail,conds,cFuncCondEntry))
			break;
	}
	EXEC SQL CLOSE curFuncCondEntry;
	if(match)
	{
		/*--- 查询业务参数 ---*/
		EXEC SQL OPEN curFuncParam;
		for(i=0;;i++){
			EXEC SQL FETCH curFuncParam into :funcParam;
			rtrim(funcParam.value);
			memcpy(&(cFuncCondEntry->cFuncParamOut[i]),&funcParam,sizeof(CFUNCPARAM));
		}
		EXEC SQL CLOSE curFuncParam;
		cFuncCondEntry->paramsOut=i;
	}
	return match;

DB_ERROR:
	printf("SQLERROR: \n%.*s\n", sqlca.sqlerrm.sqlerrml, sqlca.sqlerrm.sqlerrmc);
	EXEC SQL WHENEVER SQLERROR CONTINUE;
	EXEC SQL CLOSE curFuncCondDetail;
	EXEC SQL CLOSE curFuncCondEntry;
	EXEC SQL CLOSE curFuncParam;
	return -1;
}

int getBillParam(CFUNCCONDENTRY *cFuncCondEntry,int funcparamType)
{
	char funcCode[FUNCTION_CODE_LEN+1];
	int entrySerial;
	CFUNCCONDDETAIL cFuncCondDetail[MAX_PARAMS];
	CFUNCPARAM funcParam;
	int conds,i;
	int match;
	
	
	EXEC SQL WHENEVER SQLERROR GOTO DB_ERROR;

	/**	oraca.orastxtf = ORASTFERR; **/
		
	/*--- 初始化 ---*/
	/*--- 输入参数排序 ---*/
	
	qsort(cFuncCondEntry->cFuncParamIn, cFuncCondEntry->paramsIn, sizeof(CFUNCPARAM), paramCodeComp);

	/*--- 匹配条件 ---*/
	strcpy(funcCode,cFuncCondEntry->function_code);

	EXEC SQL DECLARE curBillCondEntry CURSOR FOR 
	select serial from cBillCondEntry 
	where ctrl_code=:funcCode 
	and funcparam_type=:funcparamType order by serial;

	EXEC SQL DECLARE curBillCondDetail CURSOR FOR 
	select a.param_code,value_type,value_length,operator,value1,nvl(value2,'0') 
	from sParamCode a,cBillCondDetail b 
	where a.param_code=b.param_code  
	and ctrl_code=:funcCode 
	and serial=:entrySerial 
	order by a.param_code;

	EXEC SQL DECLARE curBillParam CURSOR FOR 
	select a.param_code,order_code,value_type,value_length,value 
	from sParamCode a,cBillParam b 
	where a.param_code=b.param_code 
	and ctrl_code=:funcCode and serial=:entrySerial 
	order by a.param_code,order_code;

	EXEC SQL WHENEVER NOT FOUND DO break;

	EXEC SQL OPEN curBillCondEntry;
	printf("0000 [%d]\n", sqlca.sqlcode);
	match=0;
	for(;;){
		EXEC SQL FETCH curBillCondEntry into :entrySerial;
		printf("111111 [%d]\n", sqlca.sqlcode);
		EXEC SQL OPEN curBillCondDetail;
		printf("22222 [%d]\n", sqlca.sqlcode);
		for(conds=0;;conds++){
			EXEC SQL FETCH curBillCondDetail into :cFuncCondDetail[conds];
			printf("3333 [%d]\n", sqlca.sqlcode);
			rtrim(cFuncCondDetail[conds].value1);
			rtrim(cFuncCondDetail[conds].value2);

		}
		EXEC SQL CLOSE curBillCondDetail;
		/*--- 条件判断 ---*/
		if(match=funcCondMatch(cFuncCondDetail,conds,cFuncCondEntry))
			break;
	}
	EXEC SQL CLOSE curBillCondEntry;
	if(match)
	{
		/*--- 查询业务参数 ---*/
		EXEC SQL OPEN curBillParam;
		for(i=0;;i++){
			EXEC SQL FETCH curBillParam into :funcParam;
			printf("555555 [%d] [%s]\n", sqlca.sqlcode, funcParam.value);
			rtrim(funcParam.value);
			memcpy(&(cFuncCondEntry->cFuncParamOut[i]),&funcParam,sizeof(CFUNCPARAM));
		}
		EXEC SQL CLOSE curBillParam;
		cFuncCondEntry->paramsOut=i;
	}
	return match;

DB_ERROR:
	printf("SQLERROR: \n%.*s\n", sqlca.sqlerrm.sqlerrml, sqlca.sqlerrm.sqlerrmc);
	EXEC SQL WHENEVER SQLERROR CONTINUE;
	EXEC SQL CLOSE curFuncCondDetail;
	EXEC SQL CLOSE curFuncCondEntry;
	EXEC SQL CLOSE curFuncParam;
	return -1;
}

int funcChk(CFUNCCONDENTRY *cFuncCondEntry)
{
	return getFuncParam(cFuncCondEntry,0);
}

int funcDesc(CFUNCCONDENTRY *cFuncCondEntry)
{
	return getFuncParam(cFuncCondEntry,1);
}

int prodBillChk(CFUNCCONDENTRY *cFuncCondEntry)
{
	return getBillParam(cFuncCondEntry,0);
}

int prodBillDesc(CFUNCCONDENTRY *cFuncCondEntry)
{
	return getBillParam(cFuncCondEntry,1);
}

