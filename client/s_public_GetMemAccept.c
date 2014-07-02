#include "publicsrv.h"

int main()
{
	char accept[30];

	memset(accept,0,sizeof(accept));

	strcpy(accept,GetMemAccept());
	if (strlen(accept) == 0)
	{
                printf("取内存流水失败!\n");
	}
	printf("当前内存流水为:%s\n",accept);
}
