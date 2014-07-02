#include "llayerStruct.h"

TDriverType CLShmDriver::GetDriverType() const
{
	return DRIVER_SHM;
}

ETYPE CLShmDriver::GetDbShmKey(key_t *kid)
{
#ifdef WIN32
#else
	char filename[PATH_MAX];
	ETYPE iErr;
	
	memset(filename, 0, sizeof(filename));
	if ((iErr=CLFile::GetCfgDir(SHMKDIR, filename)) != 0)
	{
		return iErr;
	}
	*kid = ftok( filename, SHMKEY);
	if (*kid == (key_t)-1 )
	{
		return CLError::E_SHM_GETKEY;
	}
#endif
	return 0;
}

/* 从共享内存中取数据 */
ETYPE CLShmDriver::GetDbShm(TComShm **shmp)
{
#ifdef WIN32
#else
	int shmId;
	key_t shmKey;
	ETYPE iErr;

	if ((iErr=GetDbShmKey(&shmKey)) != 0)
	{
		return iErr;
	}
	
	//shmget - get shared memory segment identifier
	if (( shmId = shmget(shmKey,sizeof(TComShm), 0)) < 0 )
	{
		return CLError::E_SHM_GET_ID;
	}
	
	//The shmat() function  attaches  the  shared  memory  segment
	//associated  with  the  shared memory identifier specified by
	//shmid to the data segment of the calling process.
	if ((*shmp = (TComShm *) shmat (shmId, 0,0)) == (TComShm *) -1)
	{
		return CLError::E_SHM_ASSOCIATED;
	}
#endif
	return 0;
}

// 断开共享内存与数据结构的连接
ETYPE CLShmDriver::DetachShm(const void *shmp)
{
#ifdef WIN32
#else
	if (shmdt ((char*)shmp) < 0 )
	{
		return CLError::E_SHM_DETACH;
	}
#endif
	return 0;
}
