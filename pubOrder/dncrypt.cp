#include "order_xml.h"

EXEC SQL INCLUDE SQLCA;
EXEC SQL INCLUDE SQLDA;

int main (argc,argv)
	int argc;
	char **argv;
{
	char v_src[PUB_MAXENCRYPTLEN],v_dest[PUB_MAXENCRYPTLEN];	
	int eflag=0;
	
	init(v_src);		init(v_dest);
	
	if (argc!=2){
		printf("Usage:dncrypt srouce_string !\n");
		exit(0);	
	}

	strcpy(v_src,argv[1]);

	spublicEnDePasswd(v_src,v_dest,PUB_MAINKEY,1);
	printf("v_src=[%s], v_dest=[%s]\n", v_src, v_dest);

	exit(0);
} 
