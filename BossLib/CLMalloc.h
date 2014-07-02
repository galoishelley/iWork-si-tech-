#ifndef __CLMalloc_h__
#define __CLDbug_h__

class CLMalloc : public CLBase
{
public:
#ifndef DBUG_OFF
	/**
	 *     当前总的分配次数，在GlobalMalloc加一，在GlobalFree减一。
	 */
	static ub8 totalMallocTimes;

	/**
	 *     得到当前已经用Malloc分配的字节数
	 */
	static inline ub8 GetTotalMallocTimes()
	{
		return totalMallocTimes;
	}
#endif
	/**
	 *     malloc的封装函数，内部会记录调用malloc的调用次数。
	 */
	static void *GlobalMalloc (size_t size)
	{
		register char *new_malloc;

		if (!(new_malloc = (char*) malloc((size_t) size)))
		{
			return NULL;
		}
#ifndef DBUG_OFF
		//printf("CLMalloc.GlobalMalloc: mallocAddressId=[%ld], totalMallocTimes=%d\n", new_malloc,totalMallocTimes);
		totalMallocTimes ++;
#endif
		return (new_malloc);
	}

	/**
	 *     free的封装函数
	 */
	static void GlobalFree (void* ptr)
	{
#ifndef DBUG_OFF
		totalMallocTimes--;
		//printf("CLMalloc.GlobalMalloc: freeAddressId=[%ld],totalMallocTimes=%d\n", ptr,totalMallocTimes);
#endif
		free(ptr);
	}
};
#endif// __CLMalloc_h__
