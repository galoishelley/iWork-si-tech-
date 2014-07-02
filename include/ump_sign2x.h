/* Copyright (C) 1997-2012 Liusheng (liusheng@umpay.com)
 * All rights reserved.
 * @author Liusheng (liusheng@umpay.com)
 * $ openssl pkcs8 -inform DER -outform PEM -in testMer.key.p8 -out testMer.key.p8.pem -nocrypt
 * $ cc -lcrypto -o a2.out ump_sign2x.c ump_test.c
 * $ ./a2.out
 **/
/*************************************************************/
#ifndef UMP_SIGN2X_H
/*************************************************************/
#define UMP_SIGN2X_H
/*************************************************************/
#define UMP_LOAD_DIR "UMP_LOAD_DIR"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
	
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
//#include <openssl/bio.h>
#include <openssl/buffer.h>
/*************************************************************/	
#ifdef __cplusplus
extern "C" {
#endif
/*************************************************************/
EVP_PKEY* 	init_pkey(char* keyfile);
EVP_PKEY* 	init_cert(char* certfile);
void 		free_pkey(EVP_PKEY* pkey);
int umpay_sign(EVP_PKEY* prikey, unsigned char* src, unsigned int srcLen, unsigned char* buf, unsigned int bufSize);
int umpay_verify(EVP_PKEY* pubkey, unsigned char* src, unsigned int srcLen, char* sig, unsigned int sigLen);
void printBytes(unsigned char *buf, int len, char* info);
/*************************************************************/
int func_sign(char *data, char *dest);
int func_sign_verify(char *data, char *dest);
#ifdef __cplusplus
}
#endif
/*************************************************************/
#endif	/*UMP_SIGN2X_H*/
/*************************************************************/

