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

#include "bvh_intersector1.h"
#include "bvh_intersector_node.h"
#include "bvh_traverser1.h"

#include "../geometry/intersector_iterators.h"
#include "../geometry/triangle_intersector.h"
#include "../geometry/trianglev_intersector.h"
#include "../geometry/trianglev_mb_intersector.h"
#include "../geometry/trianglei_intersector.h"
#include "../geometry/trianglei_mb_intersector.h"
#include "../geometry/quadv_intersector.h"
#include "../geometry/quadi_intersector.h"
#include "../geometry/quadi_mb_intersector.h"
#include "../geometry/bezier1v_intersector.h"
#include "../geometry/bezier1i_intersector.h"
#include "../geometry/linei_intersector.h"
#include "../geometry/subdivpatch1eager_intersector.h"
#include "../geometry/subdivpatch1cached_intersector.h"
#include "../geometry/object_intersector.h"

namespace embree
{ 
  namespace isa
  {
    template<int N, int types, bool robust, typename PrimitiveIntersector1>
    void BVHNIntersector1<N,types,robust,PrimitiveIntersector1>::intersect(const BVH* __restrict__ bvh, Ray& __restrict__ ray, IntersectContext* context)
    {
      /*! perform per ray precalculations required by the primitive intersector */
      Precalculations pre(ray,bvh,bvh->numTimeSteps);

      /*! stack state */
      StackItemT<NodeRef> stack[stackSize];           //!< stack of nodes 
      StackItemT<NodeRef>* stackPtr = stack+1;        //!< current stack pointer
      StackItemT<NodeRef>* stackEnd = stack+stackSize;
      stack[0].ptr  = bvh->getRoot(pre);
      stack[0].dist = neg_inf;
      /* filter out invalid rays */
#if defined(EMBREE_IGNORE_INVALID_RAYS)
      if (!ray.valid()) return;
#endif
      /* verify correct input */
      assert(ray.valid());
      assert(ray.tnear >= 0.0f);
      assert(!(types & BVH_MB) || (ray.time >= 0.0f && ray.time <= 1.0f));

      /*! load the ray into SIMD registers */
      size_t leafType = 0;
      context->geomID_to_instID = nullptr;
      TravRay<N,Nx> vray(ray.org,ray.dir);
      vfloat<Nx> ray_near = max(ray.tnear,0.0f);
      vfloat<Nx> ray_far  = max(ray.tfar ,0.0f);

      /*! initialize the node traverser */
      BVHNNodeTraverser1<N,Nx,types> nodeTraverser(vray);

      /* pop loop */
      while (true) pop:
      {
        /*! pop next node */
        if (unlikely(stackPtr == stack)) break;
        stackPtr--;
        NodeRef cur = NodeRef(stackPtr->ptr);

        /*! if popped node is too far, pop next one */
        if (unlikely(*(float*)&stackPtr->dist > ray.tfar))
          continue;

        /* downtraversal loop */
        while (true)
        {
          /* intersect node */
          size_t mask; vfloat<Nx> tNear;
          STAT3(normal.trav_nodes,1,1,1);
          bool nodeIntersected = BVHNNodeIntersector1<N,Nx,types,robust>::intersect(cur,vray,ray_near,ray_far,pre.ftime(),tNear,mask);
          if (unlikely(!nodeIntersected)) { STAT3(normal.trav_nodes,-1,-1,-1); break; }

          /*! if no child is hit, pop next node */
          if (unlikely(mask == 0))
            goto pop;

          /* select next child and push other children */
          nodeTraverser.traverseClosestHit(cur,mask,tNear,stackPtr,stackEnd);
        }

        /* ray transformation support */
        if (unlikely(nodeTraverser.traverseTransform(cur,ray,vray,leafType,context,stackPtr,stackEnd)))
          goto pop;
        
        /*! this is a leaf node */
        assert(cur != BVH::emptyNode);
        STAT3(normal.trav_leaves,1,1,1);
        size_t num; Primitive* prim = (Primitive*) cur.leaf(num);
        size_t lazy_node = 0;
        PrimitiveIntersector1::intersect(pre,ray,context,leafType,prim,num,lazy_node);
        ray_far = ray.tfar;

        /*! push lazy node onto stack */
        if (unlikely(lazy_node)) {
          stackPtr->ptr = lazy_node;
          stackPtr->dist = neg_inf;
          stackPtr++;
        }

        // perform stack compaction
        /*StackItemT<NodeRef>* left=stack;
        for (StackItemT<NodeRef>* right=stack; right<stackPtr; right++) 
        {
          if (*(float*)&right->dist >= ray.tfar) continue;
          *left = *right; left++;
        }
        stackPtr = left;*/
      }
      AVX_ZERO_UPPER();
    }
    
    template<int N, int types, bool robust, typename PrimitiveIntersector1>
    void BVHNIntersector1<N,types,robust,PrimitiveIntersector1>::occluded(const BVH* __restrict__ bvh, Ray& __restrict__ ray, IntersectContext* context)
    {
      /*! early out for already occluded rays */
      if (unlikely(ray.geomID == 0))
        return;

      /*! perform per ray precalculations required by the primitive intersector */
      Precalculations pre(ray,bvh,bvh->numTimeSteps);

      /*! stack state */
      NodeRef stack[stackSize];  //!< stack of nodes that still need to get traversed
      NodeRef* stackPtr = stack+1;        //!< current stack pointer
      NodeRef* stackEnd = stack+stackSize;
      stack[0] = bvh->getRoot(pre);
      
      /* filter out invalid rays */
#if defined(EMBREE_IGNORE_INVALID_RAYS)
      if (!ray.valid()) return;
#endif

      /* verify correct input */
      assert(ray.valid());
      assert(ray.tnear >= 0.0f);
      assert(!(types & BVH_MB) || (ray.time >= 0.0f && ray.time <= 1.0f));

      /*! load the ray into SIMD registers */
      size_t leafType = 0;
      context->geomID_to_instID = nullptr;
      TravRay<N,Nx> vray(ray.org,ray.dir);
      vfloat<Nx> ray_near = max(ray.tnear,0.0f);
      vfloat<Nx> ray_far  = max(ray.tfar ,0.0f);

      /*! initialize the node traverser */
      BVHNNodeTraverser1<N,Nx,types> nodeTraverser(vray);

      /* pop loop */
      while (true) pop:
      {
        /*! pop next node */
        if (unlikely(stackPtr == stack)) break;
        stackPtr--;
        NodeRef cur = (NodeRef) *stackPtr;
        
        /* downtraversal loop */
        while (true)
        {
          /* intersect node */
          size_t mask; vfloat<Nx> tNear;
          STAT3(shadow.trav_nodes,1,1,1);
          bool nodeIntersected = BVHNNodeIntersector1<N,Nx,types,robust>::intersect(cur,vray,ray_near,ray_far,pre.ftime(),tNear,mask);
          if (unlikely(!nodeIntersected)) { STAT3(shadow.trav_nodes,-1,-1,-1); break; }

          /*! if no child is hit, pop next node */
          if (unlikely(mask == 0))
            goto pop;

          /* select next child and push other children */
          nodeTraverser.traverseAnyHit(cur,mask,tNear,stackPtr,stackEnd);
        }
        
        /* ray transformation support */
        if (unlikely(nodeTraverser.traverseTransform(cur,ray,vray,leafType,context,stackPtr,stackEnd)))
          goto pop;

        /*! this is a leaf node */
        assert(cur != BVH::emptyNode);
        STAT3(shadow.trav_leaves,1,1,1);
        size_t num; Primitive* prim = (Primitive*) cur.leaf(num);
        size_t lazy_node = 0;
        if (PrimitiveIntersector1::occluded(pre,ray,context,leafType,prim,num,lazy_node)) {
          ray.geomID = 0;
          break;
        }
        
        /*! push lazy node onto stack */
        if (unlikely(lazy_node)) {
          *stackPtr = (NodeRef)lazy_node;
          stackPtr++;
        }
      }
      AVX_ZERO_UPPER();
    }
  }
}
