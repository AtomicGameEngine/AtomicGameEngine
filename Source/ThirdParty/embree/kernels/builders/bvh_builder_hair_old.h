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

#include "../bvh/bvh.h"
#include "../geometry/primitive.h"
#include "../builders/bvh_builder_sah.h"
#include "../builders/heuristic_binning_array_aligned.h"
#include "../builders/heuristic_binning_array_unaligned.h"
#include "../builders/heuristic_strand_array.h"

namespace embree
{
  namespace isa
  {
    /*! Performs standard object binning */
    template<typename PrimRef, size_t BINS>
      struct UnalignedHeuristicArrayBinningSAHOld
      {
        typedef BinSplit<BINS> Split;
        typedef BinInfoT<BINS,PrimRef,BBox3fa> Binner;
        typedef range<size_t> Set;

         /*! computes bounding box of bezier curves for motion blur */
        struct PrimInfoMB 
        {
          PrimInfo pinfo;
          BBox3fa s0t0;
          BBox3fa s1t1;
        };

        __forceinline UnalignedHeuristicArrayBinningSAHOld ()
          : prims(nullptr) {}
        
        /*! remember prim array */
        __forceinline UnalignedHeuristicArrayBinningSAHOld (PrimRef* prims)
          : prims(prims) {}

        const LinearSpace3fa computeAlignedSpace(const range<size_t>& set)
        {
          /*! find first curve that defines valid direction */
          Vec3fa axis(0,0,1);
          for (size_t i=set.begin(); i<set.end(); i++)
          {
            const BezierPrim& prim = prims[i];
            const Curve3fa curve(prim.p0,prim.p1,prim.p2,prim.p3);
            const Vec3fa p0 = curve.begin();
            const Vec3fa p3 = curve.end();
            const Vec3fa axis1 = normalize(p3 - p0);
            if (sqr_length(p3 - p0) > 1E-18f) {
              axis = axis1;
              break;
            }
          }
          return frame(axis).transposed();
        }

        const AffineSpace3fa computeAlignedSpaceMB(Scene* scene, const range<size_t>& pinfo)
        {
          /*! find first curve that defines valid directions */
          Vec3fa axis0(0,0,1);
          Vec3fa axis1(0,0,1);

          for (size_t i=pinfo.begin(); i<pinfo.end(); i++)
          {
            const BezierPrim& prim = prims[i];
            const size_t geomID = prim.geomID();
            const size_t primID = prim.primID();
            const NativeCurves* curves = scene->get<NativeCurves>(geomID);
            const int curve = curves->curve(primID);
            
            const Vec3fa A3 = curves->vertex(curve+3,0);
            const Vec3fa A2 = curves->vertex(curve+2,0);
            const Vec3fa A1 = curves->vertex(curve+1,0);
            const Vec3fa A0 = curves->vertex(curve+0,0);
            const Curve3fa curveA(A0,A1,A2,A3);
            const Vec3fa a0 = curveA.begin();
            const Vec3fa a3 = curveA.end();
            
            const Vec3fa B3 = curves->vertex(curve+3,1);
            const Vec3fa B2 = curves->vertex(curve+2,1);
            const Vec3fa B1 = curves->vertex(curve+1,1);
            const Vec3fa B0 = curves->vertex(curve+0,1);
            const Curve3fa curveB(B0,B1,B2,B3);
            const Vec3fa b0 = curveB.begin();
            const Vec3fa b3 = curveB.end();
            
            if (sqr_length(a3 - a0) > 1E-18f && sqr_length(b3 - b0) > 1E-18f)
            {
              axis0 = normalize(a3 - a0);
              axis1 = normalize(b3 - b0);
              break;
            }
          }

          Vec3fa axis01 = axis0+axis1;
          if (sqr_length(axis01) < 1E-18f) axis01 = axis0;
          axis01 = normalize(axis01);
          return frame(axis01).transposed();
        }
        
        const PrimInfo computePrimInfo(const range<size_t>& set, const LinearSpace3fa& space)
        {
          auto computeBounds = [&](const range<size_t>& r) -> CentGeomBBox3fa
            {
              CentGeomBBox3fa bounds(empty);
              for (size_t i=r.begin(); i<r.end(); i++)
                bounds.extend(prims[i].bounds(space));
              return bounds;
            };
          
          const CentGeomBBox3fa bounds = parallel_reduce(set.begin(), set.end(), size_t(1024), size_t(4096), 
                                                         CentGeomBBox3fa(empty), computeBounds, CentGeomBBox3fa::merge2);

          return PrimInfo(set.begin(),set.end(),bounds.geomBounds,bounds.centBounds);
        }
        
        const PrimInfoMB computePrimInfoMB(size_t timeSegment, size_t numTimeSteps, Scene* scene, const range<size_t>& pinfo, const AffineSpace3fa& space)
        {
          size_t N = 0;
          BBox3fa centBounds = empty;
          BBox3fa geomBounds = empty;
          BBox3fa s0t0 = empty, s1t1 = empty;
          for (size_t i=pinfo.begin(); i<pinfo.end(); i++)  // FIXME: parallelize
          {
            const BezierPrim& prim = prims[i];
            const size_t geomID = prim.geomID();
            const size_t primID = prim.primID();

            N++;
            const BBox3fa bounds = prim.bounds(space);
            geomBounds.extend(bounds);
            centBounds.extend(center2(bounds));

            const NativeCurves* curves = scene->get<NativeCurves>(geomID);
            const LBBox3fa linearBounds = curves->linearBounds(space,primID,timeSegment,numTimeSteps);
            s0t0.extend(linearBounds.bounds0);
            s1t1.extend(linearBounds.bounds1);
          }
          
          PrimInfoMB ret;
          ret.pinfo = PrimInfo(N,geomBounds,centBounds);
          ret.s0t0 = s0t0;
          ret.s1t1 = s1t1;
          return ret;
        }
        
        /*! finds the best split */
        __forceinline const Split find(const PrimInfoRange& pinfo, const size_t logBlockSize, const LinearSpace3fa& space)
        {
          if (likely(pinfo.size() < 10000))
            return find_template<false>(pinfo,logBlockSize,space);
          else
            return find_template<true>(pinfo,logBlockSize,space);
        }

        /*! finds the best split */
        template<bool parallel>
        const Split find_template(const PrimInfoRange& set, const size_t logBlockSize, const LinearSpace3fa& space)
        {
          Binner binner(empty);
          const BinMapping<BINS> mapping(set);
          bin_serial_or_parallel<parallel>(binner,prims,set.begin(),set.end(),size_t(4096),mapping,space);
          return binner.best(mapping,logBlockSize);
        }
        
        /*! array partitioning */
        __forceinline void split(const Split& split, const LinearSpace3fa& space, const Set& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          if (likely(set.size() < 10000))
            split_template<false>(split,space,set,lset,rset);
          else
            split_template<true>(split,space,set,lset,rset);
        }

        /*! array partitioning */
        template<bool parallel>
        __forceinline void split_template(const Split& split, const LinearSpace3fa& space, const Set& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          if (!split.valid()) {
            deterministic_order(set);
            return splitFallback(set,lset,rset);
          }
          
          const size_t begin = set.begin();
          const size_t end   = set.end();
          CentGeomBBox3fa local_left(empty);
          CentGeomBBox3fa local_right(empty);
          const int splitPos = split.pos;
          const int splitDim = split.dim;

          size_t center = 0;
          if (likely(set.size() < 10000))
            center = serial_partitioning(prims,begin,end,local_left,local_right,
                                         [&] (const PrimRef& ref) { return split.mapping.bin_unsafe(center2(ref.bounds(space)))[splitDim] < splitPos; },
                                         [] (CentGeomBBox3fa& pinfo,const PrimRef& ref) { pinfo.extend(ref.bounds()); });
          else
            center = parallel_partitioning(prims,begin,end,EmptyTy(),local_left,local_right,
                                           [&] (const PrimRef& ref) { return split.mapping.bin_unsafe(center2(ref.bounds(space)))[splitDim] < splitPos; },
                                           [] (CentGeomBBox3fa& pinfo,const PrimRef& ref) { pinfo.extend(ref.bounds()); },
                                           [] (CentGeomBBox3fa& pinfo0,const CentGeomBBox3fa& pinfo1) { pinfo0.merge(pinfo1); },
                                           128);
          
          new (&lset) PrimInfoRange(begin,center,local_left.geomBounds,local_left.centBounds);
          new (&rset) PrimInfoRange(center,end,local_right.geomBounds,local_right.centBounds);
          assert(area(lset.geomBounds) >= 0.0f);
          assert(area(rset.geomBounds) >= 0.0f);
        }
        
        void deterministic_order(const range<size_t>& set) 
        {
          /* required as parallel partition destroys original primitive order */
          std::sort(&prims[set.begin()],&prims[set.end()]);
        }
        
        void splitFallback(const range<size_t>& set, PrimInfoRange& lset, PrimInfoRange& rset)
        {
          const size_t begin = set.begin();
          const size_t end   = set.end();
          const size_t center = (begin + end)/2;
          
          CentGeomBBox3fa left; left.reset();
          for (size_t i=begin; i<center; i++)
            left.extend(prims[i].bounds());
          new (&lset) PrimInfoRange(begin,center,left.geomBounds,left.centBounds);
          
          CentGeomBBox3fa right; right.reset();
          for (size_t i=center; i<end; i++)
            right.extend(prims[i].bounds());	
          new (&rset) PrimInfoRange(center,end,right.geomBounds,right.centBounds);
        }
        
      private:
        PrimRef* const prims;
      };

    template<int N,
             typename CreateAllocFunc,
             typename CreateAlignedNodeFunc, 
             typename CreateUnalignedNodeFunc, 
             typename CreateLeafFunc, 
             typename ProgressMonitor>

      class BVHNBuilderHairOld
    {
      ALIGNED_CLASS;

      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;
      typedef FastAllocator::ThreadLocal2* Allocator;
      typedef HeuristicArrayBinningSAH<BezierPrim,NUM_OBJECT_BINS> HeuristicBinningSAH;
      typedef UnalignedHeuristicArrayBinningSAHOld<BezierPrim,NUM_OBJECT_BINS> UnalignedHeuristicBinningSAH;
      //typedef HeuristicStrandSplit HeuristicStrandSplitSAH;

      static const size_t MAX_BRANCHING_FACTOR =  8;         //!< maximal supported BVH branching factor
      static const size_t MIN_LARGE_LEAF_LEVELS = 8;         //!< create balanced tree if we are that many levels before the maximal tree depth
      static const size_t SINGLE_THREADED_THRESHOLD = 4096;  //!< threshold to switch to single threaded build

      static const size_t travCostAligned = 1;
      static const size_t travCostUnaligned = 5;
      static const size_t intCost = 6;

    public:
      
      BVHNBuilderHairOld (BezierPrim* prims,
                       const CreateAllocFunc& createAlloc, 
                       const CreateAlignedNodeFunc& createAlignedNode, 
                       const CreateUnalignedNodeFunc& createUnalignedNode, 
                       const CreateLeafFunc& createLeaf,
                       const ProgressMonitor& progressMonitor,
                       const size_t branchingFactor, const size_t maxDepth, const size_t logBlockSize, 
                       const size_t minLeafSize, const size_t maxLeafSize )
        : createAlloc(createAlloc), 
        createAlignedNode(createAlignedNode), 
        createUnalignedNode(createUnalignedNode), 
        createLeaf(createLeaf),
        progressMonitor(progressMonitor),
        prims(prims), 
        branchingFactor(branchingFactor), maxDepth(maxDepth), logBlockSize(logBlockSize), 
        minLeafSize(minLeafSize), maxLeafSize(maxLeafSize),
        alignedHeuristic(prims), unalignedHeuristic(prims)/*, strandHeuristic(prims)*/ {}
       
      /*! entry point into builder */
      NodeRef operator() (const PrimInfoRange& pinfo) {
        NodeRef root = recurse(1,pinfo,nullptr,true);
        _mm_mfence(); // to allow non-temporal stores during build
        return root;
      }
      
    private:

       void deterministic_order(const PrimInfoRange& pinfo)
       {
         /* required as parallel partition destroys original primitive order */
         std::sort(&prims[pinfo.begin()],&prims[pinfo.end()]);
       }

       void splitFallback(const PrimInfoRange& pinfo, PrimInfoRange& linfo, PrimInfoRange& rinfo)
       {
         const size_t begin = pinfo.begin();
         const size_t end   = pinfo.end();
         const size_t center = (begin + end)/2;
         
         CentGeomBBox3fa left; left.reset();
         for (size_t i=begin; i<center; i++)
           left.extend(prims[i].bounds());
         new (&linfo) PrimInfoRange(begin,center,left.geomBounds,left.centBounds);
         
         CentGeomBBox3fa right; right.reset();
         for (size_t i=center; i<end; i++)
           right.extend(prims[i].bounds());
         new (&rinfo) PrimInfoRange(center,end,right.geomBounds,right.centBounds);
       }
       
      /*! creates a large leaf that could be larger than supported by the BVH */
      NodeRef createLargeLeaf(size_t depth, const PrimInfoRange& pinfo, Allocator alloc)
      {
        /* this should never occur but is a fatal error */
        if (depth > maxDepth) 
          throw_RTCError(RTC_UNKNOWN_ERROR,"depth limit reached");
        
        /* create leaf for few primitives */
        if (pinfo.size() <= maxLeafSize)
          return createLeaf(depth,pinfo,alloc);
        
        /* fill all children by always splitting the largest one */
        PrimInfoRange children[MAX_BRANCHING_FACTOR];
        unsigned numChildren = 1;
        children[0] = pinfo;
        
        do {
          
          /* find best child with largest bounding box area */
          int bestChild = -1;
          size_t bestSize = 0;
          for (unsigned i=0; i<numChildren; i++)
          {
            /* ignore leaves as they cannot get split */
            if (children[i].size() <= maxLeafSize)
              continue;
            
            /* remember child with largest size */
            if (children[i].size() > bestSize) { 
              bestSize = children[i].size();
              bestChild = i;
            }
          }
          if (bestChild == -1) break;
          
          /*! split best child into left and right child */
          __aligned(64) PrimInfoRange left, right;
          splitFallback(children[bestChild],left,right);
          
          /* add new children left and right */
          children[bestChild] = children[numChildren-1];
          children[numChildren-1] = left;
          children[numChildren+0] = right;
          numChildren++;
          
        } while (numChildren < branchingFactor);
        
        /* create node */
        auto node = createAlignedNode(children,numChildren,alignedHeuristic,alloc);
        return BVH::encodeNode(node);
      }
            
      /*! performs split */
      __noinline bool split(const PrimInfoRange& pinfo, PrimInfoRange& linfo, PrimInfoRange& rinfo) // FIXME: not inlined as ICC otherwise uses much stack
      {
        /* variable to track the SAH of the best splitting approach */
        float bestSAH = inf;
        const float leafSAH = intCost*float(pinfo.size())*halfArea(pinfo.geomBounds);
        
        /* perform standard binning in aligned space */
        float alignedObjectSAH = inf;
        HeuristicBinningSAH::Split alignedObjectSplit;
        alignedObjectSplit = alignedHeuristic.find(pinfo,0);
        alignedObjectSAH = travCostAligned*halfArea(pinfo.geomBounds) + intCost*alignedObjectSplit.splitSAH();
        bestSAH = min(alignedObjectSAH,bestSAH);
        
        /* perform standard binning in unaligned space */
        UnalignedHeuristicBinningSAH::Split unalignedObjectSplit;
        LinearSpace3fa uspace;
        float unalignedObjectSAH = inf;
        if (alignedObjectSAH > 0.7f*leafSAH) {
          uspace = unalignedHeuristic.computeAlignedSpace(pinfo); 
          const PrimInfoRange       sinfo = unalignedHeuristic.computePrimInfo(pinfo,uspace);
          unalignedObjectSplit = unalignedHeuristic.find(sinfo,0,uspace);    	
          unalignedObjectSAH = travCostUnaligned*halfArea(pinfo.geomBounds) + intCost*unalignedObjectSplit.splitSAH();
          bestSAH = min(unalignedObjectSAH,bestSAH);
        }

#if 0
        /* perform splitting into two strands */
        HeuristicStrandSplitSAH::Split strandSplit;
        float strandSAH = inf;
        if (alignedObjectSAH > 0.6f*leafSAH) {
          strandSplit = strandHeuristic.find(pinfo);
          strandSAH = travCostUnaligned*halfArea(pinfo.geomBounds) + intCost*strandSplit.splitSAH();
          bestSAH = min(strandSAH,bestSAH);
        }
#endif   
        /* perform aligned split if this is best */
        if (bestSAH == alignedObjectSAH) {
          alignedHeuristic.split(alignedObjectSplit,pinfo,linfo,rinfo);
          return true;
        }
        /* perform unaligned split if this is best */
        else if (bestSAH == unalignedObjectSAH) {
          unalignedHeuristic.split(unalignedObjectSplit,uspace,pinfo,linfo,rinfo);
          return false;
        }
#if 0
        /* perform strand split if this is best */
        else if (bestSAH == strandSAH) {
          strandHeuristic.split(strandSplit,pinfo,linfo,rinfo);
          return false;
        }
#endif
        /* otherwise perform fallback split */
        else {
          deterministic_order(pinfo);
          splitFallback(pinfo,linfo,rinfo);
          return true;
        }
      }
      
      /*! recursive build */
      NodeRef recurse(size_t depth, const PrimInfoRange& pinfo, Allocator alloc, bool toplevel)
      {
        if (alloc == nullptr) 
          alloc = createAlloc();

        /* call memory monitor function to signal progress */
        if (toplevel && pinfo.size() <= SINGLE_THREADED_THRESHOLD)
          progressMonitor(pinfo.size());
	
        PrimInfoRange children[MAX_BRANCHING_FACTOR];
        
        /* create leaf node */
        if (depth+MIN_LARGE_LEAF_LEVELS >= maxDepth || pinfo.size() <= minLeafSize) {
          deterministic_order(pinfo);
          return createLargeLeaf(depth,pinfo,alloc);
        }
        
        /* fill all children by always splitting the one with the largest surface area */
        size_t numChildren = 1;
        children[0] = pinfo;
        bool aligned = true;
        
        do {
          
          /* find best child with largest bounding box area */
          ssize_t bestChild = -1;
          float bestArea = neg_inf;
          for (size_t i=0; i<numChildren; i++)
          {
            /* ignore leaves as they cannot get split */
            if (children[i].size() <= minLeafSize)
              continue;
            
            /* remember child with largest area */
            if (area(children[i].geomBounds) > bestArea) { 
              bestArea = area(children[i].geomBounds);
              bestChild = i;
            }
          }
          if (bestChild == -1) break;
          
          /*! split best child into left and right child */
          PrimInfoRange left, right;
          aligned &= split(children[bestChild],left,right);
          
          /* add new children left and right */
          children[bestChild] = children[numChildren-1];
          children[numChildren-1] = left;
          children[numChildren+0] = right;
          numChildren++;
          
        } while (numChildren < branchingFactor); 
        assert(numChildren > 1);
	
        /* create aligned node */
        if (aligned) 
        {
          auto node = createAlignedNode(children,numChildren,alignedHeuristic,alloc);

          /* spawn tasks or ... */
          if (pinfo.size() > SINGLE_THREADED_THRESHOLD)
          {
            parallel_for(size_t(0), numChildren, [&] (const range<size_t>& r) {
                for (size_t i=r.begin(); i<r.end(); i++) {
                  node->child(i) = recurse(depth+1,children[i],nullptr,true); 
                  _mm_mfence(); // to allow non-temporal stores during build
                }                
              });
          }
          /* ... continue sequential */
          else {
            for (size_t i=0; i<numChildren; i++) 
              node->child(i) = recurse(depth+1,children[i],alloc,false);
          }
          return BVH::encodeNode(node);
        }
        
        /* create unaligned node */
        else 
        {
          auto node = createUnalignedNode(children,numChildren,unalignedHeuristic,alloc);
          
          /* spawn tasks or ... */
          if (pinfo.size() > SINGLE_THREADED_THRESHOLD)
          {

            parallel_for(size_t(0), numChildren, [&] (const range<size_t>& r) {
                for (size_t i=r.begin(); i<r.end(); i++) {
                    node->child(i) = recurse(depth+1,children[i],nullptr,true);
                    _mm_mfence(); // to allow non-temporal stores during build
                }                
              });
          }
          /* ... continue sequentially */
          else
          {
            for (size_t i=0; i<numChildren; i++) 
              node->child(i) = recurse(depth+1,children[i],alloc,false);
          }
          return BVH::encodeNode(node);
        }
      }
    
    private:      
      const CreateAllocFunc& createAlloc;
      const CreateAlignedNodeFunc& createAlignedNode;
      const CreateUnalignedNodeFunc& createUnalignedNode;
      const CreateLeafFunc& createLeaf;
      const ProgressMonitor& progressMonitor;

    private:
      BezierPrim* prims;
      const size_t branchingFactor;
      const size_t maxDepth;
      const size_t logBlockSize;
      const size_t minLeafSize;
      const size_t maxLeafSize;
  
    private:
      HeuristicBinningSAH alignedHeuristic;
      UnalignedHeuristicBinningSAH unalignedHeuristic;
      //HeuristicStrandSplitSAH strandHeuristic;
    };

    template<int N,
             typename CreateAllocFunc,
             typename CreateAlignedNodeFunc, 
             typename CreateUnalignedNodeFunc, 
             typename CreateLeafFunc, 
             typename ProgressMonitor>

      typename BVHN<N>::NodeRef bvh_obb_builder_binned_sah (const CreateAllocFunc& createAlloc,
                                                const CreateAlignedNodeFunc& createAlignedNode, 
                                                const CreateUnalignedNodeFunc& createUnalignedNode, 
                                                const CreateLeafFunc& createLeaf, 
                                                const ProgressMonitor& progressMonitor,
                                                BezierPrim* prims, 
                                                const PrimInfo& pinfo,
                                                const size_t branchingFactor, const size_t maxDepth, const size_t logBlockSize, 
                                                const size_t minLeafSize, const size_t maxLeafSize) 
    {
      typedef BVHNBuilderHairOld<N,CreateAllocFunc,CreateAlignedNodeFunc,CreateUnalignedNodeFunc,CreateLeafFunc,ProgressMonitor> Builder;
      Builder builder(prims,createAlloc,createAlignedNode,createUnalignedNode,createLeaf,progressMonitor,
                      branchingFactor,maxDepth,logBlockSize,minLeafSize,maxLeafSize);
      return builder(pinfo);
    }
  }
}
