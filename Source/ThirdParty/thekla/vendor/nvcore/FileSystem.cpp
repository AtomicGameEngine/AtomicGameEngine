// This code is in the public domain -- castano@gmail.com

#include "FileSystem.h"

#if NV_OS_WIN32
#define _CRT_NONSTDC_NO_WARNINGS // _chdir is defined deprecated, but that's a bug, chdir is deprecated, _chdir is *not*.
//#include <shlwapi.h> // PathFileExists
#include <windows.h> // GetFileAttributes
#include <direct.h> // _mkdir
#elif NV_OS_XBOX
#include <Xtl.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <stdio.h> // remove, unlink

using namespace nv;


bool FileSystem::exists(const char * path)
{
#if NV_OS_UNIX
	return access(path, F_OK|R_OK) == 0;
	//struct stat buf;
	//return stat(path, &buf) == 0;
#elif NV_OS_WIN32 || NV_OS_XBOX
    // PathFileExists requires linking to shlwapi.lib
    //return PathFileExists(path) != 0;
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#elif NV_OS_ORBIS
    // SBtodoORBIS use Fios2??
    const int BUFFER_SIZE = 2048;
    char file_fullpath[BUFFER_SIZE];
    snprintf(file_fullpath, BUFFER_SIZE, "/app0/%s", path);
    if (FILE * fp = fopen(file_fullpath, "r"))
	{
		fclose(fp);
		return true;
	}
	return false;
#else
	if (FILE * fp = fopen(path, "r"))
	{
		fclose(fp);
		return true;
	}
	return false;
#endif
}

bool FileSystem::createDirectory(const char * path)
{
#if NV_OS_WIN32 || NV_OS_XBOX
    return CreateDirectoryA(path, NULL) != 0;
#else
    return mkdir(path, 0777) != -1;
#endif
}

bool FileSystem::changeDirectory(const char * path)
{
#if NV_OS_WIN32
    return _chdir(path) != -1;
#elif NV_OS_XBOX
	// Xbox doesn't support Current Working Directory!
	return false;
#elif NV_OS_ORBIS
    // Orbis doesn't support Current Working Directory!
	return false;
#else
    return chdir(path) != -1;
#endif
}

bool FileSystem::removeFile(const char * path)
{
    // @@ Use unlink or remove?
    return remove(path) == 0;
}
