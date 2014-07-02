/* FileName: UniSafe.h */
/***************************************************************************************
 * 
 * Information Security Development Toolkit UniSafe V1.0
 * 
 * Author:
 *		Lizemin (lizemin@unismmw.com) 
 * 
 * Date: 
 *		Oct, 10, 1999 
 * 
 * Copyright(C) 1999-2000 Beijing Tsinghua Unishunf Info Sec Co., Ltd 
 * All rights reserved. 
 * 
 * 
 ****************************************************************************************/


#ifndef		__UNISAFE_HEADER_
#define		__UNISAFE_HEADER_


/*******************************************************/
#ifndef FALSE
#define FALSE             (0x00)
#endif

#ifndef TRUE
#define TRUE              (0x01)
#endif

#define CK_PTR *
typedef unsigned char     CK_BYTE;
typedef CK_BYTE           CK_CHAR;
typedef unsigned long int CK_ULONG;
typedef long int          CK_LONG;
typedef unsigned short CK_USHORT;
typedef short	CK_SHORT;
typedef CK_SHORT CK_PTR CK_SHORT_PTR;
typedef CK_BYTE     CK_PTR   CK_BYTE_PTR;
typedef CK_CHAR     CK_PTR   CK_CHAR_PTR;
typedef CK_ULONG    CK_PTR   CK_ULONG_PTR;
typedef CK_USHORT	CK_PTR	 CK_USHORT_PTR;
typedef void        CK_PTR   CK_VOID_PTR;
typedef CK_VOID_PTR CK_PTR CK_VOID_PTR_PTR;

/* CK_RV is a value that identifies the return value of a UniSafe function */
typedef CK_ULONG          CK_RV;

#define CKR_OK                                0x00000000
#define CKR_HOST_MEMORY                       0x00000002
#define CKR_GENERAL_ERROR                     0x00000005
#define CKR_FUNCTION_FAILED                   0x00000006
#define CKR_ARGUMENTS_BAD                     0x00000007
#define CKR_CANT_LOCK                         0x0000000A
#define CKR_DEVICE_ERROR                      0x00000030
#define CKR_DEVICE_REMOVED                    0x00000032
#define CKR_ENCRYPTED_DATA_INVALID            0x00000040
#define CKR_ENCRYPTED_DATA_LEN_RANGE          0x00000041
#define CKR_DEVICE_MEMORY                     0x00000031
#define CKR_MUTEX_BAD                         0x000001A0
#define CKR_MUTEX_NOT_LOCKED                  0x000001A1
#define CKR_COMMUNICATION_FAIL		      0x000005A0


/**************************** Function ProtoType ****************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Function: C_GetRandomFromSN()
 *
 * Description:
 *         Get random number from given Card's Serial number.
 *
 * Parameter:
 *         pInSN        --- Input, Card Serial number, '0'-'9' total 13
 *         pOutRandom   --- Output, generated random number, '0'-'9' total 8
 *
 * Return:
 *         0  ---- Success
 *        !0  ---- Fail
 */
int C_GetRandomFromSN(unsigned char *pInSN, unsigned char *pOutRandom);

/* Function: C_GetCipherFromRandom()
 *
 * Description:
 *         Get cipher (password) from given Card's random number.
 *
 * Parameter:
 *         pInRandom    --- Input, Card random number, '0'-'9' total 8
 *         pOutCipher   --- Output, Card password, '0'-'9' total 9
 *
 * Return:
 *         0  ---- Success
 *        !0  ---- Fail
 */
int C_GetCipherFromRandom(unsigned char *pInRandom, unsigned char *pOutCipher);

/* Function: C_GetRandomFromCipher()
 *
 * Description:
 *         Get card Random from given Card's cipher(password).
 *
 * Parameter:
 *         pInCipher    --- Input, Card password, '0'-'9' total 9
 *         pOutRandom   --- Output, Card random number, '0'-'9' total 8
 *
 * Return:
 *         0  ---- Success
 *        !0  ---- Fail
 */
int C_GetRandomFromCipher(unsigned char *pInCipher, unsigned char *pOutRandom);


#ifdef __cplusplus
}
#endif

#endif
