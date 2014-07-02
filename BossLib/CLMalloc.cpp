#include "llayerStruct.h"

#ifndef DBUG_OFF
/**
 *     当前总的分配次数，在GlobalMalloc加一，在GlobalFree减一。
 */
ub8 CLMalloc::totalMallocTimes = 0;
#endif
