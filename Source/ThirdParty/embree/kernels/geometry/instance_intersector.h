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

#include "../common/scene_instance.h"
#include "../common/ray.h"

namespace embree
{
  namespace isa
  {
    template<int K>
    struct FastInstanceIntersectorK
    {
      static void intersect(vint<K>* valid, const Instance* instance, RayK<K>& ray, size_t item);
      static void occluded (vint<K>* valid, const Instance* instance, RayK<K>& ray, size_t item);
    };

    typedef FastInstanceIntersectorK<4>  FastInstanceIntersector4;
    typedef FastInstanceIntersectorK<8>  FastInstanceIntersector8;
    typedef FastInstanceIntersectorK<16> FastInstanceIntersector16;
  }
}
