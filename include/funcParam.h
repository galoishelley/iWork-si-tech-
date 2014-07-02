#ifndef _FUNCPARAM
#define _FUNCPARAM

#define FUNCTION_CODE_LEN 5
#define PARAM_VALUE_LEN 255
#define MAX_PARAMS 40
#define PARAM_VALUE_SEP " "

typedef struct {
	int param_code;
	int value_type;
	int value_length;
	int operator1;
	char value1[PARAM_VALUE_LEN+1];
	char value2[PARAM_VALUE_LEN+1];
	} CFUNCCONDDETAIL;

typedef struct {
	int param_code;
	int order_code;
	int value_type;
	int value_length;
	char value[PARAM_VALUE_LEN+1];
	} CFUNCPARAM;
	
typedef struct {
	char function_code[FUNCTION_CODE_LEN+1];
	int serial;
	int paramsIn;
	CFUNCPARAM cFuncParamIn[MAX_PARAMS];
	int paramsOut;
	CFUNCPARAM cFuncParamOut[MAX_PARAMS];
	} CFUNCCONDENTRY;

int getFuncParam(CFUNCCONDENTRY *cFuncCondEntry,int funcparamType);
int funcChk(CFUNCCONDENTRY *cFuncCondEntry);
int funcDesc(CFUNCCONDENTRY *cFuncCondEntry);


#endif
