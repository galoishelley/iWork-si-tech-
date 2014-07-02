#include "order_xml.h"

int main (argc,argv)
	int argc;
	char **argv;
{
	int		i,v_ret=0;	
	char	v_tmp_buf[500];
	char	v_prog_label[50+1];
	char	v_prog_name[50+1];
	PUB_CfgMsg v_chn_cfg_msg[DLMAXCFGNUM];
	int		v_process_num=0,v_mainmsg_label=0,v_submsg_label=0;
	
	init(v_tmp_buf);		init(v_prog_label);		init(v_prog_name);
	memset(v_chn_cfg_msg,0,DLMAXCFGNUM*sizeof(PUB_CfgMsg));

	if (argc!=2){
		printf("Usage: orderStop [FLAG]\n");	
	}

	strcpy(v_prog_name,argv[1]);
	strcpy(v_prog_label,v_prog_name);
	strupr(v_prog_label);

	v_ret=readEnvCfg(PUB_HOME_LABEL);
	if (v_ret<0){
		printf("Get Environment Variable Error!!\n");
		exit(-1);	
	}
	
	v_ret=readProgCfg(v_prog_label,v_chn_cfg_msg);
	if (v_ret<0){
		printf("Get Config Values Error!!\n");
		exit(-1);	
	}
	
	for (i=0;i<DLMAXCFGNUM;i++){
		if (strcmp(v_chn_cfg_msg[i].cfgName,"PROCESS_NUM")==0)
			v_process_num=atoi(v_chn_cfg_msg[i].cfgValue);
			
		if (strcmp(v_chn_cfg_msg[i].cfgName,"MAINMSG_LABEL")==0)
			v_mainmsg_label=atoi(v_chn_cfg_msg[i].cfgValue);

		if (strcmp(v_chn_cfg_msg[i].cfgName,"SUBMSG_LABEL")==0)
			v_submsg_label=atoi(v_chn_cfg_msg[i].cfgValue);
	}
	
	/*É±µô³ÌÐò*/
	sprintf(v_tmp_buf,"ps -ef|grep %s|grep -v grep|grep -v vi|awk '{print $2}'|xargs kill -9",v_prog_name);
	system(v_tmp_buf);
	sleep(1);
	
	PubFreeSysRes(v_process_num,v_mainmsg_label,v_submsg_label);

	exit(0);	
}

