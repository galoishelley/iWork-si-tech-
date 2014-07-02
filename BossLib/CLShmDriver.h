#ifndef __CLShmDriver_h__
#define __CLShmDriver_h__

#define	SHMKDIR		"sDBLogin.cfg"
#define SHMKEY		1
 
class CLShmDriver :public CLDriver
{
public:
	CLShmDriver()
	{
		
	}
	//驱动类型定义代码
	virtual TDriverType GetDriverType() const;
	
	
	static ETYPE	GetDbShmKey(key_t *kid);
	
	// 从共享内存中取数据 
	static ETYPE GetDbShm(TComShm **shmp);

	// 断开共享内存与数据结构的连接
	static ETYPE DetachShm(const void *shmp);
private:
};

#endif //__CLShmDriver_h__
