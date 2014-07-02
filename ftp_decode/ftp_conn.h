/*********************************************************************
*
*	Copyright (C), 1995-2006, Si-Tech Information Technology Ltd.
*
**********************************************************************/

#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#ifndef FTP_CONN_H
#define FTP_CONN_H


#define PUB_DIR_LEN_MAX		1000		/*�ļ�����󳤶ȣ���Ŀ¼��*/
#define PUB_PREFIX_LEN_MAX	100		/*���ǰ׺�ַ���󳤶�*/
#define PUB_POSTFIX_LEN_MAX	100		/*��Ӻ�׺�ַ���󳤶�*/
#define PUB_TRANS_DIR_LEN_MAX	10		/*�������䷽����ִ���󳤶�*/
#define PUB_LENGTH_OF_LINE_MAX	1000		/*�ı��ļ�һ�����ַ������Ŀ*/
#define PUB_IP_ADD_LEN		16		/*IP��ַ�ִ���󳤶�*/
#define	PUB_KEY_VALUE		"HAHAHACAIBUZHAO"
#define	PUB_USERNAME_LEN	256
#define	PUB_PASSWORD_LEN	256

/******************************************
*	�Դ������͵Ķ���
*/
#define PUB_ERR_FOPEN		"EcaipbSY1"	/*���ļ�����*/
#define PUB_ERR_MALLOC		"EcaipbSY2"	/*�ڴ�������*/
#define PUB_ERR_SEG_FORMAT	"EcaipbSY3"	/*�������ô���*/
#define PUB_ERR_PARAM_FORMAT	"EcaipbSY4"	/*�������ô���*/
#define PUB_ERR_PWDFILE		"EcaipbSY5"	/*�����ļ����ô���*/
#define PUB_ERR_PWDFILE_IO	"EcaipbSY6"	/*�������ļ�IO����*/

/******************************************
*	�Ե��Կ��صĶ���
*/
#define DEBUG_PUB_GET_PWD_USER


/******************************************
*	�õ��û����Ϳ���
*/
extern int get_pwd_info
(
	char *,				/*�����ļ���*/
	char *,				/*������Կ*/
	char *,				/*���ܺ�õ����û���*/
	char *				/*���ܺ�õ����û�����*/
);

char	g_src_file[PUB_DIR_LEN_MAX];

#endif


