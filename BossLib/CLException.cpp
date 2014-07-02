#include "llayerStruct.h"

ETYPE CLException::GetErrorNo()
{
	return m_errorCode;
}

const char* CLException::GetErrorMsg()
{
	return CLError::GetErrorMsg(m_errorCode);
}

