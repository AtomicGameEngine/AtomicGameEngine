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

#include "../common/default.h"
#include "../common/ray.h"
#include "../common/scene.h"

namespace embree
{
  namespace isa
  {
    class RayStream
    {
    public:

      static void filterAOS(Scene* scene, RTCRay*    rays, const size_t N, const size_t stride, IntersectContext* context, const bool intersect);
      static void filterAOP(Scene* scene, RTCRay**   rays, const size_t N, IntersectContext* context, const bool intersect);
      static void filterSOA(Scene* scene, char*      rays, const size_t N, const size_t streams, const size_t stream_offset, IntersectContext* context, const bool intersect);
      static void filterSOP(Scene* scene, const RTCRayNp& rays, const size_t N, IntersectContext* context, const bool intersect);
    };
  }
};
