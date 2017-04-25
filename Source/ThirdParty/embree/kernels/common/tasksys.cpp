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

#include "../../common/algorithms/parallel_for.h" 

namespace embree
{
  /* Signature of ispc-generated 'task' functions */
  typedef void (*ISPCTaskFunc)(void* data, int threadIndex, int threadCount, int taskIndex, int taskCount);
  
  extern "C" __dllexport void* ISPCAlloc(void** taskPtr, int64_t size, int32_t alignment) 
  {
    if (*taskPtr == nullptr) *taskPtr = new std::vector<void*>;
    std::vector<void*>* lst = (std::vector<void*>*)(*taskPtr);
    void* ptr = alignedMalloc((size_t)size,alignment);
    lst->push_back(ptr);
    return ptr;
  }
  
  extern "C" __dllexport void ISPCSync(void* task) 
  {
    std::vector<void*>* lst = (std::vector<void*>*)task;
    for (size_t i=0; i<lst->size(); i++) alignedFree((*lst)[i]);
    delete lst;
  }
  
  extern "C" __dllexport void ISPCLaunch(void** taskPtr, void* func, void* data, int count) 
  {      
    parallel_for(0, count,[&] (const range<int>& r) {
        const int threadIndex = (int) TaskScheduler::threadIndex();
        const int threadCount = (int) TaskScheduler::threadCount();
        for (int i=r.begin(); i<r.end(); i++) 
          ((ISPCTaskFunc)func)(data,threadIndex,threadCount,i,count);
      });
  }
}
