#include "publicsrv.h"

int main()
{
        if (spublicPutKeyShm() < 0) 
	{
                userlog("初始化共享内存失败!\n");
        }
	if (CreatMemAccept() != 0)
	{
                userlog("初始化内存流水失败!\n");
	}
}
