#include "publicsrv.h"

main(int argc, char *argv[])
{
        char dest[32+1];
        memset(dest, 0, sizeof(dest));

	if (argc != 2) {
		printf("usage: please info\n");
		exit(0);
	}

	if (atoi(argv[1]) == 0 ) {
		spublicEnDePasswd(argv[1], dest, MASTERKEY, 0);
		printf("[%s] [%s]\n", argv[2], dest);
	}
	else {
		printf(" ‰»Î¥ÌŒÛ!\n");
	}
}
