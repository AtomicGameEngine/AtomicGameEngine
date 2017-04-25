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
#include "../common/primref.h"

namespace embree
{
  namespace isa
  {
    template<int N, typename Mesh>
    class BVHNBuilderTwoLevel : public Builder
    {
      ALIGNED_CLASS;

      typedef BVHN<N> BVH;
      typedef typename BVH::AlignedNode AlignedNode;
      typedef typename BVH::NodeRef NodeRef;

    public:

      typedef void (*createMeshAccelTy)(Mesh* mesh, AccelData*& accel, Builder*& builder);

      struct __aligned(16) BuildRef
      {
      public:
        __forceinline BuildRef () {}

        __forceinline BuildRef (const BBox3fa& bounds, NodeRef node)
          : lower(bounds.lower), upper(bounds.upper), node(node)
        {
          if (node.isLeaf())
            lower.w = 0.0f;
          else
            lower.w = area(this->bounds());
        }

        __forceinline BBox3fa bounds () const {
          return BBox3fa(lower,upper);
        }

        friend bool operator< (const BuildRef& a, const BuildRef& b) {
          return a.lower.w < b.lower.w;
        }

      public:
        Vec3fa lower;
        Vec3fa upper;
        NodeRef node;
      };
      
      /*! Constructor. */
      BVHNBuilderTwoLevel (BVH* bvh, Scene* scene, const createMeshAccelTy createMeshAcce, const size_t singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD);
      
      /*! Destructor */
      ~BVHNBuilderTwoLevel ();
      
      /*! builder entry point */
      void build();
      void deleteGeometry(size_t geomID);
      void clear();

      void open_sequential(size_t numPrimitives);
      void open_overlap(size_t numPrimitives);
      
    public:
      BVH* bvh;
      std::vector<BVH*>& objects;
      std::vector<Builder*> builders;
      
    public:
      Scene* scene;
      createMeshAccelTy createMeshAccel;
      
      mvector<BuildRef> refs;
      mvector<PrimRef> prims;
      std::atomic<int> nextRef;
      const size_t singleThreadThreshold;
    };
  }
}
