#ifndef _PRODPARAM
#define _PRODPARAM

#define CTRL_CODE_LENGTH 5
#define PARAM_VALUE_LEN 255
#define MAX_PARAMS 40
#define PARAM_VALUE_SEP " "

#define  D_PARAM_SMCODE         0
#define  D_PARAM_MODECODE       1
#define  D_PARAM_REGIONCODE     2
#define  D_PARAM_PAYCODE        3
#define  D_PARAM_PAYTYPE        4
#define  D_PARAM_ACCOUNTTYPE    5
#define  D_PARAM_MACHINECODE    6
#define  D_PARAM_OWNERGRADE     7
#define  D_PARAM_IDNO           8
#define  D_PARAM_ATTRCODE       9
#define  D_PARAM_RUNCODE        10
#define  D_PARAM_LOGINNO        11
#define  D_PARAM_OPENTIME       12
#define  D_PARAM_SIMNO          13
#define  D_PARAM_PHONENO        14
#define  D_PARAM_CUSTID         15
#define  D_PARAM_CONTRACTNO     16
#define  D_PARAM_OPCODE         20
#define  D_PARAM_PREPAY         100
#define  D_PARAM_MARK           101

typedef struct {
	int param_code;
	int value_type;
	int value_length;
	int operation;
	char value1[PARAM_VALUE_LEN+1];
	char value2[PARAM_VALUE_LEN+1];
} CPRODCONDDETAIL;

typedef struct {
	int param_code;
	int order_code;
	int value_type;
	int value_length;
	char value[PARAM_VALUE_LEN+1];
} CPRODPARAM;
	
typedef struct {
	char function_code[CTRL_CODE_LENGTH+1];
	int serial;
	int paramsIn;
	CPRODPARAM cFuncParamIn[MAX_PARAMS];
	int paramsOut;
	CPRODPARAM cFuncParamOut[MAX_PARAMS];
	} CPRODCONDENTRY;

typedef struct {
    char  function_code[CTRL_CODE_LENGTH+1];
    int   param_num ;
    int   param_code[MAX_PARAMS+1];
    int   value_type[MAX_PARAMS+1];
    char  param_value[PARAM_VALUE_LEN+1][ PARAM_VALUE_LEN+1];
    char  retCode[6+1];
    char  retMsg[128+1];
}CPRODCONDLIST;

int getProdParam(CPRODCONDENTRY *cProdCondEntry,int prodparamType);
int prodChk(CPRODCONDENTRY *cProdCondEntry);
int prodDesc(CPRODCONDENTRY *cProdCondEntry);


#endif
