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
#include "../common/scene_triangle_mesh.h"
#include "../common/primref.h"

namespace embree
{
  namespace isa
  {
    template<int N, typename Mesh>
    class BVHNBuilderInstancing : public Builder
    {
      ALIGNED_CLASS;

      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;
      typedef typename BVH::AlignedNode AlignedNode;
      typedef typename BVH::AlignedNodeMB AlignedNodeMB;
      typedef typename BVH::TransformNode TransformNode;

    public:

      typedef void (*createMeshAccelTy)(Mesh* mesh, AccelData*& accel, Builder*& builder);

      struct BuildRef
      {
      public:
        __forceinline BuildRef () {}

        __forceinline BuildRef (const AffineSpace3fa& local2world, const BBox3fa& localBounds_in, NodeRef node, unsigned mask, int instID, int xfmID, int type, int depth = 0)
          : local2world(local2world), localBounds(localBounds_in), node(node), mask(mask), instID(instID), xfmID(xfmID), type(type), depth(depth)
        {
          if (node.isAlignedNode()) {
          //if (node.isAlignedNode() || node.isAlignedNodeMB()) {
            const BBox3fa worldBounds = xfmBounds(local2world,localBounds);
            localBounds.lower.w = area(worldBounds);
          } else {
            localBounds.lower.w = 0.0f;
          }
        }

        __forceinline void clearArea() {
          localBounds.lower.w = 0.0f;
        }

        __forceinline BBox3fa worldBounds() const {
          return xfmBounds(local2world,localBounds);
        }

        friend bool operator< (const BuildRef& a, const BuildRef& b) {
          return a.localBounds.lower.w < b.localBounds.lower.w;
        }

      public:
        AffineSpace3fa local2world;
        BBox3fa localBounds;
        NodeRef node;
        unsigned mask;
        int instID;
        int xfmID;
        int type;
        int depth;
      };
      
      /*! Constructor. */
      BVHNBuilderInstancing (BVH* bvh, Scene* scene, const createMeshAccelTy createMeshAccel);
      
      /*! Destructor */
      ~BVHNBuilderInstancing ();
      
      /*! builder entry point */
      void build();
      void deleteGeometry(size_t geomID);
      void clear();

      void open(size_t numPrimitives);

      size_t numCollapsedTransformNodes;
      NodeRef collapse(NodeRef& node);
      
    public:
      BVH* bvh;
      std::vector<BVH*>& objects;
      std::vector<Builder*> builders;
      const createMeshAccelTy createMeshAccel;

    public:
      Scene* scene;
      
      mvector<BuildRef> refs;
      mvector<PrimRef> prims;
      std::atomic<size_t> nextRef;
    };
  }
}
