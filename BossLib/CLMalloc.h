#ifndef __CLMalloc_h__
#define __CLDbug_h__

class CLMalloc : public CLBase
{
public:
#ifndef DBUG_OFF
	/**
	 *     ��ǰ�ܵķ����������GlobalMalloc��һ����GlobalFree��һ��
	 */
	static ub8 totalMallocTimes;

	/**
	 *     �õ���ǰ�Ѿ���Malloc������ֽ���
	 */
	static inline ub8 GetTotalMallocTimes()
	{
		return totalMallocTimes;
	}
#endif
	/**
	 *     malloc�ķ�װ�������ڲ����¼����malloc�ĵ��ô�����
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
	 *     free�ķ�װ����
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
