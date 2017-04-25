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

#include "../builders/bvh_builder_hair.h"
#include "../builders/bvh_builder_hair_old.h"
#include "../builders/primrefgen.h"

#include "../geometry/bezier1v.h"
#include "../geometry/bezier1i.h"

#if defined(EMBREE_GEOMETRY_HAIR)

namespace embree
{
  namespace isa
  {
    template<int N, typename Primitive>
    struct BVHNHairBuilderSAH : public Builder
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      BVH* bvh;
      Scene* scene;
      mvector<PrimRef> prims;

      BVHNHairBuilderSAH (BVH* bvh, Scene* scene)
        : bvh(bvh), scene(scene), prims(scene->device) {}
      
      void build() 
      {
        /* fast path for empty BVH */
        const size_t numPrimitives = scene->getNumPrimitives<NativeCurves,false>();
        if (numPrimitives == 0) {
          prims.clear();
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }

        double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "BuilderHairSAH");

        //profile(1,5,numPrimitives,[&] (ProfileTimer& timer) {
        
        /* create primref array */
        prims.resize(numPrimitives);
        const PrimInfo pinfo = createPrimRefArray<NativeCurves,false>(scene,prims,scene->progressInterface);

        /* estimate acceleration structure size */
        const size_t node_bytes = pinfo.size()*sizeof(typename BVH::UnalignedNode)/(4*N);
        const size_t leaf_bytes = pinfo.size()*sizeof(Primitive);
        bvh->alloc.init_estimate(node_bytes+leaf_bytes);
        
        /* builder settings */
        BVHNBuilderHair::Settings settings;
        settings.branchingFactor = N;
        settings.maxDepth = BVH::maxBuildDepthLeaf;
        settings.logBlockSize = 0;
        settings.minLeafSize = 1;
        settings.maxLeafSize = BVH::maxLeafBlocks;

        /* creates a leaf node */
        auto createLeaf = [&] (size_t depth, const range<size_t>& set, FastAllocator::ThreadLocal2* alloc) -> NodeRef
          {
            size_t start = set.begin();
            size_t items = set.size();
            Primitive* accel = (Primitive*) alloc->alloc1->malloc(items*sizeof(Primitive),BVH::byteAlignment);
            for (size_t i=0; i<items; i++) {
              accel[i].fill(prims.data(),start,set.end(),bvh->scene);
            }
            return bvh->encodeLeaf((char*)accel,items);
          };
          
        /* build hierarchy */
        typename BVH::NodeRef root = BVHNBuilderHair::build<NodeRef>
          (typename BVH::CreateAlloc(bvh),
           typename BVH::AlignedNode::Create(),
           typename BVH::AlignedNode::Set(),
           typename BVH::UnalignedNode::Create(),
           typename BVH::UnalignedNode::Set(),
           createLeaf,scene->progressInterface,scene,prims.data(),pinfo,settings);
        
        bvh->set(root,LBBox3fa(pinfo.geomBounds),pinfo.size());
        
        //});
        
        /* clear temporary data for static geometry */
        if (scene->isStatic()) {
          prims.clear();
          bvh->shrink();
        }
        bvh->cleanup();
        bvh->postBuild(t0);
      }

      void clear() {
        prims.clear();
      }
    };


     template<int N, typename Primitive>
    struct BVHNHairMBBuilderSAH : public Builder
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::AlignedNodeMB AlignedNodeMB;
      typedef typename BVH::UnalignedNodeMB UnalignedNodeMB;
      typedef typename BVH::NodeRef NodeRef;
      typedef HeuristicArrayBinningSAH<BezierPrim,NUM_OBJECT_BINS> HeuristicBinningSAH;

      BVH* bvh;
      Scene* scene;
      mvector<BezierPrim> prims;

      BVHNHairMBBuilderSAH (BVH* bvh, Scene* scene)
        : bvh(bvh), scene(scene), prims(scene->device) {}
      
      const LBBox3fa computePrimInfoMB(size_t timeSegment, size_t numTimeSteps, Scene* scene, const PrimInfoRange& pinfo)
      {
        LBBox3fa allBounds = empty;
        for (size_t i=pinfo.begin(); i<pinfo.end(); i++) // FIXME: parallelize
        {
          BezierPrim& prim = prims[i];
          const size_t geomID = prim.geomID();
          const NativeCurves* curves = scene->get<NativeCurves>(geomID);
          allBounds.extend(curves->linearBounds(prim.primID(),timeSegment,numTimeSteps));
        }
        return allBounds;
      }

      void build() 
      {
        /* progress monitor */
        auto progress = [&] (size_t dn) { bvh->scene->progressMonitor(double(dn)); };
        auto virtualprogress = BuildProgressMonitorFromClosure(progress);

        /* fast path for empty BVH */
        const size_t numPrimitives = scene->getNumPrimitives<NativeCurves,true>();
        if (numPrimitives == 0) {
          prims.clear();
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }
        
        double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "BuilderMBHairSAH");

        //profile(1,5,numPrimitives,[&] (ProfileTimer& timer) {

        /* create primref array */
        bvh->numTimeSteps = scene->getNumTimeSteps<NativeCurves,true>();
        const size_t numTimeSegments = bvh->numTimeSteps-1; assert(bvh->numTimeSteps > 1);
        prims.resize(numPrimitives);
        bvh->alloc.init_estimate(numPrimitives*sizeof(Primitive)*numTimeSegments);
        NodeRef* roots = (NodeRef*) bvh->alloc.threadLocal()->malloc(sizeof(NodeRef)*numTimeSegments,BVH::byteNodeAlignment);

        /* build BVH for each timestep */
        avector<BBox3fa> bounds(bvh->numTimeSteps);
        size_t num_bvh_primitives = 0;
        for (size_t t=0; t<numTimeSegments; t++)
        {
          /* call BVH builder */
          const PrimInfo pinfo = createBezierRefArrayMBlur(t,bvh->numTimeSteps,scene,prims,virtualprogress);
          const LBBox3fa lbbox = computePrimInfoMB(t,bvh->numTimeSteps,scene,pinfo);
        
          NodeRef root = bvh_obb_builder_binned_sah<N>
          (
            [&] () { return bvh->alloc.threadLocal2(); },

            [&] (const PrimInfoRange* children, const size_t numChildren, HeuristicBinningSAH alignedHeuristic, FastAllocator::ThreadLocal2* alloc) -> AlignedNodeMB*
            {
              AlignedNodeMB* node = (AlignedNodeMB*) alloc->alloc0->malloc(sizeof(AlignedNodeMB),BVH::byteNodeAlignment); node->clear();
              for (size_t i=0; i<numChildren; i++) 
              {
                LBBox3fa bounds = computePrimInfoMB(t,bvh->numTimeSteps,scene,children[i]);
                node->set(i,bounds);
              }
              return node;
            },

            [&] (const PrimInfoRange* children, const size_t numChildren, UnalignedHeuristicArrayBinningSAHOld<BezierPrim,NUM_OBJECT_BINS> unalignedHeuristic, FastAllocator::ThreadLocal2* alloc) -> UnalignedNodeMB*
            {
              UnalignedNodeMB* node = (UnalignedNodeMB*) alloc->alloc0->malloc(sizeof(UnalignedNodeMB),BVH::byteNodeAlignment); node->clear();
              for (size_t i=0; i<numChildren; i++) 
              {
                const AffineSpace3fa space = unalignedHeuristic.computeAlignedSpaceMB(scene,children[i]); 
                UnalignedHeuristicArrayBinningSAHOld<BezierPrim,NUM_OBJECT_BINS>::PrimInfoMB pinfo = unalignedHeuristic.computePrimInfoMB(t,bvh->numTimeSteps,scene,children[i],space);
                node->set(i,space,pinfo.s0t0,pinfo.s1t1);
              }
              return node;
            },

            [&] (size_t depth, const PrimInfoRange& pinfo, FastAllocator::ThreadLocal2* alloc) -> NodeRef
            {
              size_t items = pinfo.size();
              size_t start = pinfo.begin();
              Primitive* accel = (Primitive*) alloc->alloc1->malloc(items*sizeof(Primitive),BVH::byteAlignment);
              NodeRef node = bvh->encodeLeaf((char*)accel,items);
              for (size_t i=0; i<items; i++) {
                accel[i].fill(prims.data(),start,pinfo.end(),bvh->scene);
              }
              return node;
            },
            progress,
            prims.data(),pinfo,N,BVH::maxBuildDepthLeaf,1,1,BVH::maxLeafBlocks);

          roots[t] = root;
          bounds[t+0] = lbbox.bounds0;
          bounds[t+1] = lbbox.bounds1;
          num_bvh_primitives = max(num_bvh_primitives,pinfo.size());
        }
        bvh->set(NodeRef((size_t)roots),LBBox3fa(bounds),num_bvh_primitives);
        bvh->msmblur = true;

        //});
        
        /* clear temporary data for static geometry */
        if (scene->isStatic()) {
          prims.clear();
          bvh->shrink();
        }
        bvh->cleanup();
        bvh->postBuild(t0);
      }

      void clear() {
        prims.clear();
      }
    };
    
    /*! entry functions for the builder */
    Builder* BVH4Bezier1vBuilder_OBB_New   (void* bvh, Scene* scene, size_t mode) { return new BVHNHairBuilderSAH<4,Bezier1v>((BVH4*)bvh,scene); }
    Builder* BVH4Bezier1iBuilder_OBB_New   (void* bvh, Scene* scene, size_t mode) { return new BVHNHairBuilderSAH<4,Bezier1i>((BVH4*)bvh,scene); }
    Builder* BVH4Bezier1iMBBuilder_OBB_New (void* bvh, Scene* scene, size_t mode) { return new BVHNHairMBBuilderSAH<4,Bezier1i>((BVH4*)bvh,scene); }

#if defined(__AVX__)
    Builder* BVH8Bezier1vBuilder_OBB_New   (void* bvh, Scene* scene, size_t mode) { return new BVHNHairBuilderSAH<8,Bezier1v>((BVH8*)bvh,scene); }
    Builder* BVH8Bezier1iBuilder_OBB_New   (void* bvh, Scene* scene, size_t mode) { return new BVHNHairBuilderSAH<8,Bezier1i>((BVH8*)bvh,scene); }
    Builder* BVH8Bezier1iMBBuilder_OBB_New (void* bvh, Scene* scene, size_t mode) { return new BVHNHairMBBuilderSAH<8,Bezier1i>((BVH8*)bvh,scene); }
#endif

  }
}
#endif
