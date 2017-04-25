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

#include "bvh.h"
#include "../common/ray.h"
#include "../common/stack_item.h"

namespace embree
{
  namespace isa 
  {
    /*! BVH hybrid packet intersector. Switches between packet and single ray traversal (optional). */
    template<int N, int K, int types, bool robust, typename PrimitiveIntersectorK, bool single = true>
    class BVHNIntersectorKHybrid
    {
      /* shortcuts for frequently used types */
      typedef typename PrimitiveIntersectorK::Precalculations Precalculations;
      typedef typename PrimitiveIntersectorK::Primitive Primitive;
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;
      typedef typename BVH::BaseNode BaseNode;
      typedef typename BVH::AlignedNode AlignedNode;
      typedef typename BVH::AlignedNodeMB AlignedNodeMB;
      typedef Vec3<vfloat<K>> Vec3vfK;
      typedef Vec3<vint<K>> Vec3viK;

      static const size_t stackSizeChunk = 1+(N-1)*BVH::maxDepth;

      static const size_t switchThreshold = (K==4)  ? 3 :
                                            (K==8)  ? ((N==4) ? 5 : 7) :
                                            (K==16) ? 7 :
                                                      0;

    private:
      static void intersectSingle(const vbool<K>& valid, BVH* bvh, Precalculations& pre, RayK<K>& ray, IntersectContext* context);
      static void occludedSingle (const vbool<K>& valid, BVH* bvh, Precalculations& pre, RayK<K>& ray, IntersectContext* context);

    public:
      static void intersect(vint<K>* valid, BVH* bvh, RayK<K>& ray, IntersectContext* context);
      static void occluded (vint<K>* valid, BVH* bvh, RayK<K>& ray, IntersectContext* context);
    };

    /*! BVH packet intersector. */
    template<int N, int K, int types, bool robust, typename PrimitiveIntersectorK>
      class BVHNIntersectorKChunk : public BVHNIntersectorKHybrid<N,K,types,robust,PrimitiveIntersectorK,false> {};
  }
}
