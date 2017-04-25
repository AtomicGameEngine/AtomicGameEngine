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

#pragma once

#include "platform.h"
#include <vector>

namespace embree
{
#define ALIGNED_STRUCT                                           \
  void* operator new(size_t size) { return alignedMalloc(size); }       \
  void operator delete(void* ptr) { alignedFree(ptr); }      \
  void* operator new[](size_t size) { return alignedMalloc(size); }  \
  void operator delete[](void* ptr) { alignedFree(ptr); }

#define ALIGNED_STRUCT_(align)                                           \
  void* operator new(size_t size) { return alignedMalloc(size,align); } \
  void operator delete(void* ptr) { alignedFree(ptr); }                 \
  void* operator new[](size_t size) { return alignedMalloc(size,align); } \
  void operator delete[](void* ptr) { alignedFree(ptr); }

#define ALIGNED_CLASS                                                \
  public:                                                            \
    ALIGNED_STRUCT                                                  \
  private:

#define ALIGNED_CLASS_(align)                                           \
 public:                                                               \
    ALIGNED_STRUCT_(align)                                              \
 private:
  
  /*! aligned allocation */
  void* alignedMalloc(size_t size, size_t align = 64);
  void alignedFree(void* ptr);
  
  /*! allocator that performs aligned allocations */
  template<typename T, size_t alignment = 64>
    struct aligned_allocator
    {
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;

      __forceinline pointer allocate( size_type n ) {
        return (pointer) alignedMalloc(n*sizeof(value_type),alignment);
      }

      __forceinline void deallocate( pointer p, size_type n ) {
        return alignedFree(p);
      }

      __forceinline void construct( pointer p, const_reference val ) {
        new (p) T(val);
      }

      __forceinline void destroy( pointer p ) {
        p->~T();
      }
    };

  /*! allocates pages directly from OS */
  void* os_malloc (size_t bytes);
  void* os_reserve(size_t bytes);
  void  os_commit (void* ptr, size_t bytes);
  size_t os_shrink (void* ptr, size_t bytesNew, size_t bytesOld);
  void  os_free   (void* ptr, size_t bytes);
  void  os_advise (void* ptr, size_t bytes);

  /*! allocator that performs OS allocations */
  template<typename T>
    struct os_allocator
    {
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;

      __forceinline pointer allocate( size_type n ) {
        return (pointer) os_malloc(n*sizeof(value_type));
      }

      __forceinline void deallocate( pointer p, size_type n ) {
        return os_free(p,n*sizeof(value_type));
      }

      __forceinline void construct( pointer p, const_reference val ) {
        new (p) T(val);
      }

      __forceinline void destroy( pointer p ) {
        p->~T();
      }
    };

  /*! allocator for IDs */
  template<typename T>
    struct IDPool
    {
      typedef T value_type;

      IDPool ()
      : nextID(0) {}

      __forceinline T allocate() 
      {
        /* return ID from list */
        if (IDs.size()) 
        {
          T id = IDs.back();
          IDs.pop_back();
          return id;
        } 

        /* allocate new ID */
        else {
          return nextID++;
        }
      }

      __forceinline void deallocate( T id ) 
      {
        assert(id < nextID);
        IDs.push_back(id);
      }

      __forceinline size_t size() const {
        return nextID;
      }

    private:
      std::vector<T> IDs;   //!< stores deallocated IDs to be reused
      size_t nextID;        //!< next ID to use when IDs vector is empty
    };
}

