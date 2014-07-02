/* 
 * Edited by dangsl
 * Date : 2002.12.26
 */

#include "publicsrv.h"

main(int argc, char *argv[])
{
	struct spublicdblogin sdblogin;
	int  rtn_value;

	rtn_value = -1;

	/* 取得数据库的登陆参数 */
	sdblogin = spublicGetDBP(argv[1]);
	if (sdblogin.label[0] == NULL) {
		printf("Get db parameters fail!\n");
		rtn_value = -1;
		return rtn_value;
	}
	else
                rtn_value = 0;

        printf("[%s]\n",sdblogin.username);
        printf("[%s]\n",sdblogin.password);
        printf("[%s]\n",sdblogin.database);
        printf("[%s]\n",sdblogin.server);
}
