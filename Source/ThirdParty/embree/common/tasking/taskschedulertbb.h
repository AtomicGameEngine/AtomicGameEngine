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

#include "../sys/platform.h"
#include "../sys/alloc.h"
#include "../sys/barrier.h"
#include "../sys/thread.h"
#include "../sys/mutex.h"
#include "../sys/condition.h"
#include "../sys/ref.h"

#if defined(__WIN32__)
#  define NOMINMAX
#endif

#include "tbb/tbb.h"
#include "tbb/parallel_sort.h"

namespace embree
{  
  struct TaskScheduler
  {
    /*! initializes the task scheduler */
    static void create(size_t numThreads, bool set_affinity, bool start_threads);

    /*! destroys the task scheduler again */
    static void destroy();
    
    /* returns the index of the current thread */
    static __forceinline size_t threadIndex()
    {
#if TBB_INTERFACE_VERSION >= 9100
      return tbb::this_task_arena::current_thread_index();
#elif TBB_INTERFACE_VERSION >= 9000
      return tbb::task_arena::current_thread_index();
#else
      return 0;
#endif
    }
  
    /* returns the total number of threads */
    static __forceinline size_t threadCount() {
#if TBB_INTERFACE_VERSION >= 9100
      return tbb::this_task_arena::max_concurrency();
#else
      return tbb::task_scheduler_init::default_num_threads();
#endif
    }
  };
};

