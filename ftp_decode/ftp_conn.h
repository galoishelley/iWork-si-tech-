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


#define PUB_DIR_LEN_MAX		1000		/*文件名最大长度（含目录）*/
#define PUB_PREFIX_LEN_MAX	100		/*添加前缀字符最大长度*/
#define PUB_POSTFIX_LEN_MAX	100		/*添加后缀字符最大长度*/
#define PUB_TRANS_DIR_LEN_MAX	10		/*描述传输方向的字串最大长度*/
#define PUB_LENGTH_OF_LINE_MAX	1000		/*文本文件一行中字符最大数目*/
#define PUB_IP_ADD_LEN		16		/*IP地址字串最大长度*/
#define	PUB_KEY_VALUE		"HAHAHACAIBUZHAO"
#define	PUB_USERNAME_LEN	256
#define	PUB_PASSWORD_LEN	256

/******************************************
*	对错误类型的定义
*/
#define PUB_ERR_FOPEN		"EcaipbSY1"	/*打开文件错误*/
#define PUB_ERR_MALLOC		"EcaipbSY2"	/*内存分配错误*/
#define PUB_ERR_SEG_FORMAT	"EcaipbSY3"	/*段名配置错误*/
#define PUB_ERR_PARAM_FORMAT	"EcaipbSY4"	/*参数配置错误*/
#define PUB_ERR_PWDFILE		"EcaipbSY5"	/*口令文件配置错误*/
#define PUB_ERR_PWDFILE_IO	"EcaipbSY6"	/*读口令文件IO错误*/

/******************************************
*	对调试开关的定义
*/
#define DEBUG_PUB_GET_PWD_USER


/******************************************
*	得到用户名和口令
*/
extern int get_pwd_info
(
	char *,				/*口令文件名*/
	char *,				/*解密密钥*/
	char *,				/*解密后得到的用户名*/
	char *				/*解密后得到的用户口令*/
);

char	g_src_file[PUB_DIR_LEN_MAX];

#endif


