#ifndef __CLDriver_h__
#define __CLDriver_h__

//驱动类型定义
enum TDriverType
{
	//数据库驱动类型
	DRIVER_DATABASE		= 0,
	
	//文件驱动类型
	DRIVER_FILE			= 1,
	DRIVER_SHM			= 2
};

/*
 驱动器层的基类定义，为抽象类，必须定义子类，实现驱动的各种操作。
 */
class CLDriver :public CLLayer
{
public:
	virtual TLayerNo GetLayerNo() const;

	//驱动类型定义代码
	virtual TDriverType GetDriverType() const = 0;
};

#endif //__CLDriver_h__
