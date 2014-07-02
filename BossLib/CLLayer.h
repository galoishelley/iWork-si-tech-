#ifndef __CLLayer_h__
#define __CLLayer_h__

//层定义
enum TLayerNo
{
	//驱动协议层
	LAYER_DRIVER		= 0,
	
	//原子数据协议层
	LAYER_ATOM			= 1,

	//逻辑数据协议层
	LAYER_LOGICDATA		= 2,

	//业务逻辑协议层
	LAYER_BUSINESS		= 3,

	//应用逻辑协议层
	LAYER_APPLICATION	= 4
};

class CLLayer :public CLBase
{
public:
	//得到公司名称
	virtual TLayerNo GetLayerNo() const = 0;
};

#endif //__CLLayer_h__
