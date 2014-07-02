#include "llayerStruct.h"

TDriverType CLFile::GetDriverType() const
{
	return DRIVER_FILE;
}

//得到环境变量的内容
ETYPE CLFile::GetEnvContext(const char *envLabel, char** envContext)
{
	if ((*envContext = getenv(envLabel)) == NULL)
	{
		return CLError::E_FILE_ENV;
	}
	return 0;
}

//根据文件名取出$WORKDIR/cfg下的文件全路径
ETYPE CLFile::GetCfgDir(const char *shortFile, char *fullFile)
{
	char* envContext=NULL;
	ETYPE iErrNo;
	
	if ((iErrNo=GetEnvContext(WORKDIR, &envContext)) != 0)
	{
		return iErrNo;
	}
	
	strcpy(fullFile,envContext);
	strcat(fullFile,CFGDIR);
	strcat(fullFile,FN_LIBSTR);
	strcat(fullFile,shortFile);
	
	return 0;
}

//根据文件名取出$WORKDIR/public/synlib下的文件全路径
ETYPE CLFile::GetSynlibDir(const char *shortFile, char *fullFile)
{
	char* envContext=NULL;
	ETYPE iErrNo;
	
	if ((iErrNo=GetEnvContext(WORKDIR, &envContext)) != 0)
	{
		return iErrNo;
	}
	
	strcpy(fullFile,envContext);
	strcat(fullFile,BOSS_SYNLIB_DIR);
	strcat(fullFile,FN_LIBSTR);
	strcat(fullFile,shortFile);
	
	return 0;
}
