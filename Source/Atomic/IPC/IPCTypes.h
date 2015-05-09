
#pragma once

#ifdef ATOMIC_PLATFORM_WINDOWS

namespace Atomic
{

// avoid needing to include <windows.h>

// Windows handle type

//#define INVALID_HANDLE_VALUE -1

#define INVALID_IPCHANDLE_VALUE (void *)(-1)
typedef void* IPCHandle;

#else

typedef int IPCHandle;

#endif


}
