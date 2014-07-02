#ifndef __ltypedef_h__
#define __ltypedef_h__

#ifndef ORATYPES
#ifndef ub1
typedef unsigned char	ub1;
typedef signed char		sb1;
#endif

#ifndef ub2
typedef unsigned short	ub2;
typedef signed short	sb2;
#endif

#ifndef ub4
typedef unsigned int	ub4;
typedef signed int	sb4;
#endif

#ifndef ub8
#ifdef WIN32
typedef unsigned __int64	ub8;
typedef signed __int64	sb8;
#else
typedef unsigned long	ub8;
typedef signed long	sb8;
#endif
#endif

#endif

#ifdef WIN32
#define ETYPE	ub4
#else
#define ETYPE	ub4
#endif

#ifdef WIN32
#define key_t	ub8
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

//最大的加密长度。
#define MAXENDELEN	32

#endif//__ltypedef_h__
