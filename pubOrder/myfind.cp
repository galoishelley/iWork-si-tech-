/*
**Created: Tophill(zhangkun)
**Date:    2009.03.18
*/

#include "order_xml.h"
#include <iconv.h>


void grepKey(i_fine_name)
char *i_fine_name;
{
	FILE *fp;
	fp=fopen(i_fine_name,"r");
	char v_tmp_buf[10000];

	char *p;
	
	int v_ret=0;

	while(fscanf(fp,"%s",v_tmp_buf)>0){
		strupr(v_tmp_buf);
		p=strstr(v_tmp_buf,"SELECT");
		if (p!=NULL)
			printf("[%s][%s]\n",v_tmp_buf,p+6);
	}

	fclose(fp);
}

void test(i_path_name)
char *i_path_name;
{
	DIR *v_dir;
	struct dirent *v_struct_dir;
	
	struct stat fstat;
	
	char v_tmp_buf[10000];
/*	
	v_dir = (DIR *)malloc(sizeof(DIR));
	v_struct_dir = (struct dirent *)malloc(sizeof(struct dirent));
printf("[%p][%p]\n",v_dir,v_struct_dir)	;

	memset(v_dir,0,sizeof(DIR));
	memset(v_struct_dir,0,sizeof(struct dirent));
printf("[%p][%p]\n",v_dir,v_struct_dir)	;
*/	
	v_dir=opendir(i_path_name);
	v_struct_dir=v_tmp_buf;
	while(v_struct_dir!=NULL){
		v_struct_dir=readdir(v_dir);
		/*printf("[%d][%d][%d][%d][%s]\n",v_struct_dir->d_ino,v_struct_dir->d_ino,
			v_struct_dir->d_reclen,v_struct_dir->d_namlen,v_struct_dir->d_name);*/
		
		if (v_struct_dir->d_namlen==0 || strcmp(v_struct_dir->d_name,".")==0 || strcmp(v_struct_dir->d_name,"..")==0)
			continue;
		
		init(v_tmp_buf);
		sprintf(v_tmp_buf,"%s/%s",i_path_name,v_struct_dir->d_name);
		stat(v_tmp_buf,&fstat);
		
		if (S_ISDIR(fstat.st_mode)){
			test(v_tmp_buf);
		}
		else{
			grepKey(v_tmp_buf);
			printf("ÎÄ¼þ\n");	
		}
	}
	
	closedir(v_dir);
/*
	free(v_dir);
	free(v_struct_dir);
*/
}

void clob(){
    FILE *fin, *fout;
    char *encFrom, *encTo;
    char bufin[1024], bufout[1024], *sin, *sout;
    int mode, lenin, lenout, ret, nline;
    iconv_t c_pt;
    encFrom = "utf-8";
    encTo   = "gbk";
    if ((c_pt = iconv_open(encTo, encFrom)) == (iconv_t)-1) {
        printf("iconv_open false: %s ==> %s\n", encFrom, encTo);
        return -1;
    }
    iconv(c_pt, NULL, NULL, NULL, NULL);
    strcpy(bufin,"æ­£å¸¸");
    lenin  = strlen(bufin) + 1;
    lenout = 1024;
    sin    = bufin;
    sout   = bufout;
    iconv(c_pt,
                &sin,
                (size_t*)&lenin,
                &sout,
                (size_t*)&lenout);
    printf("%s -> %s: ret=%d, len_in=%d, len_out=%d\n",
            encFrom, encTo, ret, lenin, lenout);
    iconv_close(c_pt);
    printf("out= %s\n",&sout);
}

int main (argc,argv)
	int argc;
	char **argv;
{

	test("/iboss1/work/zk/tmp");
	/*grepKey("/iboss1/work/zk/tmp/aaa");*/
	exit(0);
}


