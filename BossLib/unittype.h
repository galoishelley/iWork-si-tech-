#ifndef __unittype_h__
#define __unittype_h__

#ifndef NULL
#define NULL 0
#endif

#define U_EMPTY 0
#define U_CHAR 1
#define U_INT 2
#define U_LONG 3
#define U_DOUBLE 4
#define U_STRING 5
#define U_ARRAY 6
#define U_STRUCT 7

#define U_ERR_NOMEM 1
#define U_ERR_NOOCC 2
#define U_ERR_WRONGTYPE 3
#define U_ERR_EMPTY 4

extern int u_errno;

typedef struct tagutype utype;
typedef struct tague{
	int type;
	union {
		char v_char;	
		int v_int;		
		long v_long;	
		double v_double;	
		char * p_char;	
		utype * p_utype;	
	} value;
} ue;

struct tagutype{
	int cnt;
	int capacity;
	ue **ppe;
};

typedef struct tagCtrlInfo
{
	//������룬������Ϣ
	char	retCode[20+1];
	char	retMsg[255+1];

	//��Ϣ���룬��Ϣ����
	char	infoCode[20+1];
	char	infoMsg[256];

	int  cursorCtrl;
} TCtrlInfo;

/**
 * �����ɹ�ִ�з���0
 */
typedef unsigned long  (*TFuncBusiness)(utype *uInParam,utype **uOutParam,TCtrlInfo *ctrlInfo);

/*���ɹ�����NULL*/
utype* uInit(int capacity);	/*����ͳһ���ݽṹ�� capacity ��ʼԪ�ظ��� */

/*���ɹ�����NULL*/
utype * uSetInt(utype *pu,int occ,int iv);	/*��intֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

int uGetInt(utype *pu,int occ);	/*��intֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�����intֵ*/

/*���ɹ�����NULL*/
utype * uSetLong(utype *pu,int occ,long iv);	/*��longֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

long uGetLong(utype *pu,int occ);	/*��longֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�����longֵ*/

/*���ɹ�����NULL*/
utype * uSetDouble(utype *pu,int occ,double iv);	/*��doubleֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

double uGetDouble(utype *pu,int occ);	/*��longֵ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�����longֵ*/

/*���ɹ�����NULL*/
utype * uSetStr(utype *pu,int occ,const char * iv);	/*���ַ�����pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

char * uGetStr(utype *pu,int occ);	/*���ַ�����pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã������ַ���*/

/*���ɹ�����NULL*/
utype * uSetArray(utype *pu,int occ,utype * iv);	/*�����飬pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

/*���ɹ�����NULL*/
utype * uGetArray(utype *pu,int occ);	/*�����飬pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã��������飨ͳһ���ݽṹָ�룩*/

/*���ɹ�����NULL*/
utype * uSetStruct(utype *pu,int occ,utype * iv);	/*��ṹ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã�iv ����ֵ*/

utype* uGetStruct(utype *pu,int occ);	/*���ṹ��pu ͳһ���ݽṹָ�룬occ Ԫ��λ�ã����ؽṹ��ͳһ���ݽṹָ�룩*/

/*���ɹ�����NULL*/
utype* uAlloc(utype *pu,int occ);
void uFree(utype *pu);
void ueFree(ue *pue);

#endif/*__unittype_h__*/

