/*********************************************************************
*
*	Copyright (C), 1995-2006, Si-Tech Information Technology Ltd.
*
**********************************************************************/

#include "ftp_conn.h"


int get_pwd_info(char *file_name,char *key_word,char *user_name,char *pwd_val)
{
	FILE *fp;
	char buff[256];
	int i,j,name_len=PUB_USERNAME_LEN,pwd_len=PUB_PASSWORD_LEN;
	int key_len;
	char err_affix[1000]={0};

	key_len=strlen(key_word);
	if(NULL==(fp=fopen(file_name,"rb")))
	{
		sprintf(err_affix,"associated with [%s]",file_name);
		return 0;
	}

	if((size_t)256!=fread(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			fclose(fp);
			return 0;
		}
		if(feof(fp))
		{
			fclose(fp);
			return 0;
		}
	}
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		user_name[i]=buff[i]^key_word[j];
		if(i==name_len)
		{
			user_name[i]=0;
			break;
		}
	}

	if((size_t)256!=fread(buff,1,256,fp))
	{
		if(ferror(fp))
		{
			fclose(fp);
			return 0;
		}
		if(feof(fp))
		{
			fclose(fp);
			return 0;
		}
	}
	for(i=0;i<256;i++)
	{
		j=i-i/key_len*key_len;
		pwd_val[i]=buff[i]^key_word[j];
		if(i==pwd_len)
		{
			pwd_val[i]=0;
			break;
		}
	}
	fclose(fp);
	return 1;
}

int main(int argc, char *argv[])
{
	char	user[PUB_USERNAME_LEN], passwd[PUB_PASSWORD_LEN], buf[PUB_LENGTH_OF_LINE_MAX*4];
	FILE	*pp;

	if (argc != 2) {
		printf("usage: %s filename\n", argv[0]);
		exit(0);
	}

	snprintf(g_src_file,PUB_DIR_LEN_MAX,"%s",argv[1]);

	if(!get_pwd_info(g_src_file,PUB_KEY_VALUE,user,passwd)) {
		perror("get_pwd_info");
		exit(1);
	}

	printf("user:[%s]\npasswd:[%s]\n", user, passwd);

}
