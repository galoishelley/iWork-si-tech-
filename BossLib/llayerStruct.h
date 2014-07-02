#ifndef __llayerstruct_h__
#define __llayerstruct_h__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#ifdef WIN32
#include <process.h>
#else
#include <ipc.h>
#endif

#include <sys/types.h>

#ifdef WIN32
//忽略 C++ 异常规范，但指示函数不是 __declspec(nothrow)
#pragma warning( disable : 4290 )
#else
#include <sys/shm.h>
#include <unistd.h>
#endif

#include <stdarg.h>

#define HP 1000
/**
 机器类型定义
 PLATFORM_TYPE
	HP
 */
#if PLATFORM_TYPE == HP
#include <iostream.h>
#else
#include <iostream>
#endif

#include <string>
#include <list>
#include <vector>
/*
#include <xstring>
*/

//#include <sqlda.h>
//#include <sqlcpr.h>
#include "oci.h"

//  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
//  will be defined to have this storage class.
#define SQLCA_STORAGE_CLASS extern

#if PLATFORM_TYPE == HP
#else
using namespace std; 
#endif

#include "ltypedef.h"

#include "province.h"
#include "CLBase.h"
#include "CLString.h"
#include "CLError.h"
#include "CLException.h"
#include "CLDbug.h"
#include "CLMalloc.h"
#include "CLLayer.h"
#include "CLDriver.h"
#include "db.h"
#include "CLDbDriver.h"
#include "CLCommand.h"
#include "CLRecordSet.h"
#include "CLShmDriver.h"
#include "CLFile.h"
#include "CLRule.h"
#include "CLDomain.h"
#include "CLRight.h"
#include "unittype.h"
#endif //
