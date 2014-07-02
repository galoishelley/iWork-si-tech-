#ifndef __CLException_h__
#define __CLException_h__
class CLException : public CLBase
{
public:
	CLException(ETYPE errorCode)
	{
		m_errorCode = errorCode;
	}
public:
	ETYPE GetErrorNo();
	const char* GetErrorMsg();
private:
	ETYPE m_errorCode;
};



#endif//__CLException_h__
