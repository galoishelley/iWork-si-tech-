#ifndef __ltypedef_h__
#define __ltypedef_h__

#ifndef NULL
#define NULL	0
#endif

#define UINT	unsigned int
/*
#define uint	unsigned int
*/

#define ULONG	unsigned long
/*
#ifndef DOUBLE
#define  DOUBLE	double
#endif
*/
#ifndef ORATYPES
#ifndef ub1
#define ub1 unsigned char
#define sb1 char
#endif

#ifndef ub2
#define ub2 unsigned short
#define sb2 short
#endif

#ifndef ub4
#define ub4 unsigned int
#define sb4 int
#endif

#ifndef ub8
#ifdef WIN32
#define ub8 unsigned __int64
#define sb8 __int64
#else
#define ub8 unsigned long
#define sb8 long
#endif
#endif

#endif

#ifdef WIN32
#define ETYPE	__int64
#else
#define ETYPE	ULONG
#endif

#ifdef WIN32
#define key_t	__int64
#else
#endif

#ifndef PATH_MAX
#define PATH_MAX	1023
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#endif/*__ltypedef_h__*/
