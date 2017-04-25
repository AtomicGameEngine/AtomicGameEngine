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

#include "alloc.h"

/*! instantiate vector using default allocator */
#define vector_t vector
#define allocator_t std::allocator<T>
#include "vector_t.h"
#undef vector_t
#undef allocator_t

/*! instantiate vector using aligned malloc */
#define vector_t avector
#define allocator_t aligned_allocator<T,std::alignment_of<T>::value>
#include "vector_t.h"
#undef vector_t
#undef allocator_t

/*! instantiate vector using os_malloc */
#define vector_t ovector
#define allocator_t os_allocator<T>
#include "vector_t.h"
#undef vector_t
#undef allocator_t

namespace embree
{
  /*! vector class that performs aligned allocations */
  //template<typename T> // FIXME: unfortunately not supported in VS2012
  //  using avector = vector_t<T,aligned_allocator<T,std::alignment_of<T>::value> >;
  
  /*! vector class that performs OS allocations */
  //template<typename T> // FIXME: unfortunately not supported in VS2012
  //  using ovector = vector_t<T,os_allocator<T> >;
}
