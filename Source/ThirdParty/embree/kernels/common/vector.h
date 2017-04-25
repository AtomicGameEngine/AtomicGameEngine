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

#include "default.h"

namespace embree
{
  /*! invokes the memory monitor callback */
  struct MemoryMonitorInterface {
    virtual void memoryMonitor(ssize_t bytes, bool post) = 0;
  };

  /*! allocator that performs aligned monitored allocations */
  template<typename T, size_t alignment = 64>
    struct aligned_monitored_allocator
    {
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;
      
      __forceinline aligned_monitored_allocator(MemoryMonitorInterface* device) 
        : device(device) {}

      __forceinline pointer allocate( size_type n ) 
      {
        if (n) {
          assert(device);
          device->memoryMonitor(n*sizeof(T),false);
        }
#if defined(__LINUX__) && defined(__AVX512ER__) // KNL
        if (n*sizeof(value_type) >= 14 * PAGE_SIZE_2M)
        {
          pointer p =  (pointer) os_malloc(n*sizeof(value_type));
          assert(p);
          return p;
        }
#endif
        return (pointer) alignedMalloc(n*sizeof(value_type),alignment);
      }

      __forceinline void deallocate( pointer p, size_type n ) 
      {
        if (p)
        {
#if defined(__LINUX__) && defined(__AVX512ER__) // KNL
          if (n*sizeof(value_type) >= 14 * PAGE_SIZE_2M)
            os_free(p,n*sizeof(value_type)); 
          else
            alignedFree(p);
#else
          alignedFree(p);
#endif
        }
        else assert(n == 0);

        if (n) {
          assert(device);
          device->memoryMonitor(-ssize_t(n)*sizeof(T),true);
        }
      }

      __forceinline void construct( pointer p, const_reference val ) {
        new (p) T(val);
      }

      __forceinline void destroy( pointer p ) {
        p->~T();
      }

    private:
      MemoryMonitorInterface* device;
    };
}

/*! instantiate vector using monitored aligned allocations */
#define VECTOR_INIT_ALLOCATOR
#define vector_t mvector
#define allocator_t aligned_monitored_allocator<T,std::alignment_of<T>::value>
#include "../common/sys/vector_t.h"
#undef vector_t
#undef allocator_t
#undef VECTOR_INIT_ALLOCATOR

namespace embree
{
  /*! monitored vector */
  //template<typename T> // FIXME: unfortunately not supported in VS2012
  //using mvector = vector_t<T,aligned_monitored_allocator<T,std::alignment_of<T>::value> >;
}
