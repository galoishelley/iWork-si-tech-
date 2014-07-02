
/***************************
        kill shm, msg, sema 
 	Date: Jul 30,1999
**************************** */

#include "publicsrv.h"

void main()
{
	if((spublicDelKeyShm())<0)
		userlog("É¾³ý¹²ÏíÄÚ´æÊ§°Ü!",__FILE__,__LINE__);
	if(DelMemAccept() != 0)
		userlog("É¾³ýÄÚ´æÁ÷Ë®Ê§°Ü!",__FILE__,__LINE__);
}
