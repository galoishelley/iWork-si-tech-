/**
 *  ���ļ��ĺ����嶨���ڶ�̬����ʵ�֡�
 */

#ifndef __CLLibInit_h__
#define __CLLibInit_h__

typedef struct tagLibInitInfo
{
	//���Կⲿ��
	FILE *_db_fp_;	/* Output stream, default stderr */
	const char *_db_process_; /* Pointer to process name; argv[0] */
	FILE *_db_pfp_;	/* Profile stream, 'dbugmon.out' */
	int _db_pon_;	/* TRUE if profile currently on */
	int _db_on_;		/* TRUE if debugging currently on */
	int _no_db_;		/* TRUE if no debugging at all */
} TLibInitInfo;

typedef ETYPE  (*TLibSynchronize)(TLibInitInfo *ptLibInitInfo);
/**
 *     ����ȫ�ֱ�����ͬ��������
 */
extern "C" ETYPE LibSynchronize(TLibInitInfo *ptLibInitInfo);

#endif/*__CLLibInit_h__*/
