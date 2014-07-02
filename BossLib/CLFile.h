#ifndef __CLFile_h__
#define __CLFile_h__

#define CFGDIR			"/cfg"

//BOSS动态库
#define BOSS_SYNLIB_DIR	"/public/synlib"


#define FN_LIBCHAR		'/'
#define FN_LIBSTR		"/"
#define WORKDIR			"WORKDIR"

class CLFile :public CLDriver
{
public:
	CLFile()
	{
		
	}
	//驱动类型定义代码
	virtual TDriverType GetDriverType() const;
	
	//得到环境变量的内容
	static ETYPE GetEnvContext(const char *envLabel, char** envContext);

	//根据文件名取出$WORKDIR/cfg下的文件全路径
	static ETYPE GetCfgDir(const char *shortFile, char *fullFile);

	//根据文件名取出$WORKDIR/public/synlib下的文件全路径
	static ETYPE GetSynlibDir(const char *shortFile, char *fullFile);
	
private:
};

#endif //__CLFile_h__
