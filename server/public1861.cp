
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *atoh(unsigned char asc)
{
	static char hex[3];
	memset(hex, 0, sizeof(hex));
	sprintf(hex, "%+2X", asc);
	return hex;
}

int
gsm_encrypt(const char *src, char *dest)
{
	int		offset;
	int		keylen;
	int		srcpos;
	int		srcasc;
	int		keypos;
	char	key[39];
	int		len;
	char	strchec[39];
	char	rightstr[39];

	int		i;

	strcpy(key, "24");
	len=strlen(src);
	if (len != 18) {
		printf("明文应是18位的整数\n");
		return(1);
	}

	for (i=0; i<len; i++) {
		if (!isdigit(src[i])) {
			printf("明文应是18位的整数,不包括其他字符\n");
			return(2);
		}
	}

	offset=(atoi(&src[len-3]) * 10 % 255)+1;
	memset(dest, 0, sizeof(dest));
	sprintf(dest, "%+2X", offset);
	keylen=strlen(key);

	keypos=0;
	for (srcpos=1; srcpos<=len; srcpos++) {
		srcasc=(src[srcpos-1]+offset) % 255;
		if (keypos>(keylen-1))
			keypos=0;
		srcasc ^= key[keypos];
		strcat(dest, atoh(srcasc));
		offset=srcasc;
		keypos++;
	}
	return(0);
}

int
cbx_encrypt(const char *src, char *dest)
{
	int		i, len;
/*
	unsigned long long	passwd;
*/
	long	double	passwd;

	len=strlen(src);
	if (len != 18) {
		printf("明文应是18位的整数");
		return(1);
	}
	for (i=0; i<len; i++) {
		if (!isdigit(src[i])) {
			printf("明文应是18位的整数,不包括其他字符");
			return(2);
		}
	}
	passwd=0.00;

	sscanf(src, "%Lf", &passwd);
	passwd=passwd*22;
	sprintf(dest, "1%020.0Lf", passwd);

	return 0;
}
