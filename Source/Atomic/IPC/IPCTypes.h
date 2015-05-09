
#pragma once

namespace Atomic
{

#ifdef ATOMIC_PLATFORM_WINDOWS

// avoid needing to include <windows.h>

//#define INVALID_HANDLE_VALUE -1

// Windows handle type
#define INVALID_IPCHANDLE_VALUE (void *)(-1)
typedef void* IPCHandle;

#else

typedef int IPCHandle;

#endif


}
