#include "pay_app.h"
#include "pay_utils.h"

struct Para{
	    int   para_type;
            char  para_name[40];
            char  para_value[258];
            int   int_value;
            float float_value;
            int   para_length;
};

/*
** define structure for tuxedo function
*/
typedef struct _bc_column_data
{
        char         value[60+1];
} BC_COLUMN_DATA;
BC_COLUMN_DATA *output_para[MAX_RECORDS];



#define INPUTPARA_NUM_1 3
#define INPUTPARA_NUM_2 14
#define TUXRETURN_OK    "0000"
#define TUXPAY_FALSE    "3591"
#define TUXPAYED_OK     "3617"
#define HHMMSS          "000000"

#define ORDERSIZ 9
#define LINE    80

#if !lint
static char Sccsid[] = {"@(#) call_wlan.c 1.0 11/24/2003"};
#endif /* !lint */


char * log_time(void);

int main(int argc, char *argv[])
{
    FILE *fMessage;
    struct 	Para para_set[14];
    int 	para_count;
    int 	i, j,  k;
    int		ret_code;
    int     	output_num;
    char        fopfile[60];

    printf("--tax_4000_wlan--start!\n");

    if (argc != 2)
    {
    	printf("请输入PATH  AND FILE!\n");
    	exit(1);
    }
    memset(fopfile,'\0',sizeof(fopfile));
    strcpy(fopfile, argv[1]);
    printf("fopfile=%s!\n",fopfile);
    strcpy(para_set[0].para_name, "fopfile");
    memset(para_set[0].para_value, ' ',
    sizeof(para_set[0].para_value));
    strncpy(para_set[0].para_value, fopfile, 60);
    para_set[0].para_length = 60;

    para_count = 1;
    output_num = 2;
    ret_code = -1;

    ret_code = snd_rcv_parm("s12w0Ins", para_count,para_set, output_num);
    if (ret_code == -1) 
    {
     	printf("调用后台服务s12w0Ins错误! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][0].value);
      	exit(1);
    } 
    else if ((strcmp(output_para[0][0].value, 
		TUXRETURN_OK) == 0) || 
		(strcmp(output_para[0][0].value, TUXPAYED_OK) == 0)) 
    {
    	printf("调用后台服务s12w0Ins成功! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][0].value );
        printf("调用后台服务s12w0Ins成功! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][1].value );
    } 
    else if (ret_code == 1) 
    {
    	printf("调用后台服务s12w0Ins参数不符合条件! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][0].value);
        printf("调用后台服务s12w0Ins参数不符合条件! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][1].value);
    } 
    else 
    { 
     	printf("调用后台服务s12w0Ins失败! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][0].value);
        printf("调用后台服务s12w0Ins失败! tux_ret_code = [%d], srv_ret_code = [%s]\n", ret_code, output_para[0][1].value);
      	exit(1);
    }
   exit(0);
}
