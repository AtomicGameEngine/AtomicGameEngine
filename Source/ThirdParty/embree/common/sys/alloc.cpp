// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "config.h"
#include "alloc.h"
#include "intrinsics.h"
#include "sysinfo.h"

////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>

namespace embree
{
  void* os_malloc(size_t bytes) 
  {
    int flags = MEM_COMMIT | MEM_RESERVE;
    char* ptr = (char*) VirtualAlloc(nullptr,bytes,flags,PAGE_READWRITE);
    if (ptr == nullptr) throw std::bad_alloc();
    return ptr;
  }

  void* os_reserve(size_t bytes)
  {
    char* ptr = (char*) VirtualAlloc(nullptr,bytes,MEM_RESERVE,PAGE_READWRITE);
    if (ptr == nullptr) throw std::bad_alloc();
    return ptr;
  }

  void os_commit (void* ptr, size_t bytes) {
    VirtualAlloc(ptr,bytes,MEM_COMMIT,PAGE_READWRITE);
  }

  size_t os_shrink(void* ptr, size_t bytesNew, size_t bytesOld) 
  {
    size_t pageSize = 4096;
    bytesNew = (bytesNew+pageSize-1) & ~(pageSize-1);
    assert(bytesNew <= bytesOld);
    if (bytesNew < bytesOld)
      VirtualFree((char*)ptr+bytesNew,bytesOld-bytesNew,MEM_DECOMMIT);
    return bytesNew;
  }

  void os_free(void* ptr, size_t bytes) {
    if (bytes == 0) return;
    if (!VirtualFree(ptr,0,MEM_RELEASE))
      /*throw std::bad_alloc()*/ return;  // we on purpose do not throw an exception when an error occurs, to avoid throwing an exception during error handling
  }

  void os_advise(void *ptr, size_t bytes)
  {
  }

}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(__UNIX__)

#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define UPGRADE_TO_2M_PAGE_LIMIT (256*1024) 

namespace embree
{
  __forceinline bool isHugePageCandidate(const size_t bytes) 
  {
    /* try to use huge pages for large allocations */
    if (bytes >= PAGE_SIZE_2M)
    {
      /* multiple of page size */
      if ((bytes % PAGE_SIZE_2M) == 0) 
        return true;
      else if (bytes >= 64 * PAGE_SIZE_2M) /* will only introduce a 3% overhead */
        return true;
    }
    return false;
  }

#if !defined(__MACOSX__)
  static bool tryDirectHugePageAllocation = true;
#endif

  /* hint for transparent huge pages (THP) */
  void os_advise(void *pptr, size_t bytes)
  {
#if defined(MADV_HUGEPAGE)
    if (isHugePageCandidate(bytes)) 
      madvise(pptr,bytes,MADV_HUGEPAGE); 
#endif
  }
  
  void* os_malloc(size_t bytes)
  {
    int flags = MAP_PRIVATE | MAP_ANON;
        
    if (isHugePageCandidate(bytes)) 
    {
      bytes = (bytes+PAGE_SIZE_2M-1)&ssize_t(-PAGE_SIZE_2M);
#if !defined(__MACOSX__)
      /* try direct huge page allocation first */
      if (tryDirectHugePageAllocation)
      {
        int huge_flags = flags;
#ifdef MAP_HUGETLB
        huge_flags |= MAP_HUGETLB;
#endif
#ifdef MAP_ALIGNED_SUPER
        huge_flags |= MAP_ALIGNED_SUPER;
#endif
        void* ptr = mmap(0, bytes, PROT_READ | PROT_WRITE, huge_flags, -1, 0);
        
        if (ptr == nullptr || ptr == MAP_FAILED)
        {
          /* direct huge page allocation failed, disable it for the future */
          tryDirectHugePageAllocation = false;     
        }
        else
          return ptr;
      }
#endif
    } 
    else
      bytes = (bytes+PAGE_SIZE_4K-1)&ssize_t(-PAGE_SIZE_4K);

    /* standard mmap call */
    void* ptr = (char*) mmap(0, bytes, PROT_READ | PROT_WRITE, flags, -1, 0);
    assert( ptr != MAP_FAILED );
    if (ptr == nullptr || ptr == MAP_FAILED) throw std::bad_alloc();

    /* advise huge page hint for THP */
    os_advise(ptr,bytes);
    return ptr;
  }

  void* os_reserve(size_t bytes) 
  {
    /* linux always allocates pages on demand, thus just call allocate */
    return os_malloc(bytes);
  }

  void os_commit (void* ptr, size_t bytes) {
  }

  size_t os_shrink(void* ptr, size_t bytesNew, size_t bytesOld) 
  {
    /* first try with 4KB pages */
    bytesNew = (bytesNew+PAGE_SIZE_4K-1) & ~(PAGE_SIZE_4K-1);
    assert(bytesNew <= bytesOld);
    if (bytesNew >= bytesOld)
      return bytesOld;

    if (munmap((char*)ptr+bytesNew,bytesOld-bytesNew) != -1)
      return bytesNew;

    /* now try with 2MB pages */
    bytesNew = (bytesNew+PAGE_SIZE_2M-1) & ~(PAGE_SIZE_2M-1);
    assert(bytesNew <= bytesOld);
    if (bytesNew >= bytesOld)
      return bytesOld;

    if (munmap((char*)ptr+bytesNew,bytesOld-bytesNew) != -1)
      return bytesNew; // this may be too small in case we really used 2MB pages

    throw std::bad_alloc();
  }

  void os_free(void* ptr, size_t bytes) 
  {
    if (bytes == 0)
      return;

    size_t pageSize = PAGE_SIZE_4K;
    if (isHugePageCandidate(bytes)) 
      pageSize = PAGE_SIZE_2M;

    bytes = (bytes+pageSize-1)&ssize_t(-pageSize);
    if (munmap(ptr,bytes) == -1)
      /*throw std::bad_alloc()*/ return;  // we on purpose do not throw an exception when an error occurs, to avoid throwing an exception during error handling
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
/// All Platforms
////////////////////////////////////////////////////////////////////////////////
  
namespace embree
{
  void* alignedMalloc(size_t size, size_t align) 
  {
    assert((align & (align-1)) == 0);
    void* ptr = _mm_malloc(size,align);

    if (size != 0 && ptr == nullptr) 
      throw std::bad_alloc();
    
    return ptr;
  }
  
  void alignedFree(void* ptr) {
    _mm_free(ptr);
  }
}
