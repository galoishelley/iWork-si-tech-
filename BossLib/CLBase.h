#ifndef __CLBase_h__
#define __CLBase_h__

#define DEFAULT_COMPANY_NAME	"sitech"

#define DEFAULT_MIN_VERSION		0
#define DEFAULT_MAX_VERSION		1
#define DEFAULT_VERSION		"1.0"


class CLBase
{
public:
	//得到公司名称
	virtual const char* GetCompanyName()
	{
		return DEFAULT_COMPANY_NAME;
	}

	//得到小版本号
	virtual int GetMinVersion()
	{
		return DEFAULT_MIN_VERSION;
	}

	//得到大版本号
	virtual int GetMaxVersion()
	{
		return DEFAULT_MAX_VERSION;
	}

	//得到版本号串
	virtual const char *GetVersion()
	{
		return DEFAULT_VERSION;
	}
};

#endif/*__CLBase_h__*/
