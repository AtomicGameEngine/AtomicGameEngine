// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_CORE_MEMORY_H
#define NV_CORE_MEMORY_H

#include "nvcore.h"

#include <stdlib.h> // malloc(), realloc() and free()
//#include <stddef.h> // size_t

//#include <new>	// new and delete


#if NV_CC_GNUC
#   define NV_ALIGN_16 __attribute__ ((__aligned__ (16)))
#else
#   define NV_ALIGN_16 __declspec(align(16))
#endif


#define NV_OVERRIDE_ALLOC 0

#if NV_OVERRIDE_ALLOC

// Custom memory allocator
extern "C" {
    NVCORE_API void * malloc(size_t size);
    NVCORE_API void * debug_malloc(size_t size, const char * file, int line);
    NVCORE_API void free(void * ptr);
    NVCORE_API void * realloc(void * ptr, size_t size);
}

/*
#ifdef _DEBUG
#define new new(__FILE__, __LINE__)
#define malloc(i) debug_malloc(i, __FILE__, __LINE__)
#endif
*/

#endif

namespace nv {

    // C++ helpers.
    template <typename T> NV_FORCEINLINE T * malloc(size_t count) {
        return (T *)::malloc(sizeof(T) * count);
    }

    template <typename T> NV_FORCEINLINE T * realloc(T * ptr, size_t count) {
        return (T *)::realloc(ptr, sizeof(T) * count);
    }

    template <typename T> NV_FORCEINLINE void free(const T * ptr) {
        ::free((void *)ptr);
    }

} // nv namespace

#endif // NV_CORE_MEMORY_H
