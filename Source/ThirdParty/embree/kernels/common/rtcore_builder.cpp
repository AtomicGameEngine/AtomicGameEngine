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

#ifdef _WIN32
#  define RTCORE_API extern "C" __declspec(dllexport)
#else
#  define RTCORE_API extern "C" __attribute__ ((visibility ("default")))
#endif

#include "default.h"
#include "device.h"
#include "scene.h"
#include "context.h"
#include "alloc.h"

#include "../builders/bvh_builder_sah.h"
#include "../builders/bvh_builder_morton.h"

namespace embree
{ 
  namespace isa // FIXME: support more ISAs for builders
  {
    struct BVH
    {
      BVH (Device* device)
        : device(device), isStatic(false), allocator(device,true), morton_src(device), morton_tmp(device) {}

    public:
      Device* device;
      bool isStatic;
      FastAllocator allocator;
      mvector<BVHBuilderMorton::BuildPrim> morton_src;
      mvector<BVHBuilderMorton::BuildPrim> morton_tmp;
    };

    RTCORE_API RTCBVH rtcNewBVH(RTCDevice device)
    {
      RTCORE_CATCH_BEGIN;
      RTCORE_TRACE(rtcNewAllocator);
      RTCORE_VERIFY_HANDLE(device);
      return (RTCBVH) new BVH((Device*)device);
      RTCORE_CATCH_END((Device*)device);
      return nullptr;
    }

    void* rtcBuildBVHMorton(BVH* bvh,
                            const RTCBuildSettings& settings,
                            RTCBuildPrimitive* prims_i,
                            size_t numPrimitives,
                            RTCCreateNodeFunc createNode,
                            RTCSetNodeChildrenFunc setNodeChildren,
                            RTCSetNodeBoundsFunc setNodeBounds,
                            RTCCreateLeafFunc createLeaf,
                            RTCBuildProgressFunc buildProgress,
                            void* userPtr)
    {
      /* initialize temporary arrays for morton builder */
      PrimRef* prims = (PrimRef*) prims_i;
      mvector<BVHBuilderMorton::BuildPrim>& morton_src = bvh->morton_src;
      mvector<BVHBuilderMorton::BuildPrim>& morton_tmp = bvh->morton_tmp;
      morton_src.resize(numPrimitives);
      morton_tmp.resize(numPrimitives);

      /* compute centroid bounds */
      const BBox3fa centBounds = parallel_reduce ( size_t(0), numPrimitives, BBox3fa(empty), [&](const range<size_t>& r) -> BBox3fa {

          BBox3fa bounds(empty);
          for (size_t i=r.begin(); i<r.end(); i++) 
            bounds.extend(prims[i].bounds().center2());
          return bounds;
        }, BBox3fa::merge);
      
      /* compute morton codes */
      BVHBuilderMorton::MortonCodeMapping mapping(centBounds);
      parallel_for ( size_t(0), numPrimitives, [&](const range<size_t>& r) {
          BVHBuilderMorton::MortonCodeGenerator generator(mapping,&morton_src[r.begin()]);
          for (size_t i=r.begin(); i<r.end(); i++) {
            generator(prims[i].bounds(),(unsigned) i);
          }
        });

      /* start morton build */
      std::pair<void*,BBox3fa> root = BVHBuilderMorton::build<std::pair<void*,BBox3fa>>(
        
        /* thread local allocator for fast allocations */
        [&] () -> FastAllocator::ThreadLocal* { 
          return bvh->allocator.threadLocal();
        },
        
        /* lambda function that allocates BVH nodes */
        [&] ( FastAllocator::ThreadLocal* alloc, size_t N ) -> void* {
          return createNode((RTCThreadLocalAllocator)alloc,N,userPtr);
        },
        
        /* lambda function that sets bounds */
        [&] (void* node, const std::pair<void*,BBox3fa>* children, size_t N) -> std::pair<void*,BBox3fa>
        {
          BBox3fa bounds = empty;
          void* childptrs[BVHBuilderMorton::MAX_BRANCHING_FACTOR];
          const RTCBounds* cbounds[BVHBuilderMorton::MAX_BRANCHING_FACTOR];
          for (size_t i=0; i<N; i++) {
            bounds.extend(children[i].second);
            childptrs[i] = children[i].first;
            cbounds[i] = (const RTCBounds*)&children[i].second;
          }
          setNodeBounds(node,cbounds,N,userPtr);
          setNodeChildren(node,childptrs,N,userPtr);
          return std::make_pair(node,bounds);
        },
        
        /* lambda function that creates BVH leaves */
        [&]( const range<unsigned>& current, FastAllocator::ThreadLocal* alloc) -> std::pair<void*,BBox3fa>
        {
          const size_t id = morton_src[current.begin()].index;
          const BBox3fa bounds = prims[id].bounds(); 
          void* node = createLeaf((RTCThreadLocalAllocator)alloc,prims_i+current.begin(),current.size(),userPtr);
          return std::make_pair(node,bounds);
        },
        
        /* lambda that calculates the bounds for some primitive */
        [&] (const BVHBuilderMorton::BuildPrim& morton) -> BBox3fa {
          return prims[morton.index].bounds();
        },
        
        /* progress monitor function */
        [&] (size_t dn) { 
          if (buildProgress) buildProgress(dn,userPtr);
        },
        
        morton_src.data(),morton_tmp.data(),numPrimitives,
        settings);

      bvh->allocator.cleanup();
      return root.first;
    }

    void* rtcBuildBVHBinnedSAH(BVH* bvh,
                               const RTCBuildSettings& settings,
                               RTCBuildPrimitive* prims,
                               size_t numPrimitives,
                               RTCCreateNodeFunc createNode,
                               RTCSetNodeChildrenFunc setNodeChildren,
                               RTCSetNodeBoundsFunc setNodeBounds,
                               RTCCreateLeafFunc createLeaf,
                               RTCBuildProgressFunc buildProgress,
                               void* userPtr)
    {
      /* calculate priminfo */
      auto computeBounds = [&](const range<size_t>& r) -> CentGeomBBox3fa
        {
          CentGeomBBox3fa bounds(empty);
          for (size_t j=r.begin(); j<r.end(); j++)
            bounds.extend((BBox3fa&)prims[j]);
          return bounds;
        };
      const CentGeomBBox3fa bounds = 
        parallel_reduce(size_t(0),numPrimitives,size_t(1024),size_t(1024),CentGeomBBox3fa(empty), computeBounds, CentGeomBBox3fa::merge2);

      const PrimInfo pinfo(0,numPrimitives,bounds.geomBounds,bounds.centBounds);
      
      /* build BVH */
      void* root = BVHBuilderBinnedSAH::build<void*>(
        
        /* thread local allocator for fast allocations */
        [&] () -> FastAllocator::ThreadLocal* { 
          return bvh->allocator.threadLocal();
        },

        /* lambda function that creates BVH nodes */
        [&](BVHBuilderBinnedSAH::BuildRecord* children, const size_t N, FastAllocator::ThreadLocal* alloc) -> void*
        {
          void* node = createNode((RTCThreadLocalAllocator)alloc,N,userPtr);
          const RTCBounds* cbounds[GeneralBVHBuilder::MAX_BRANCHING_FACTOR];
          for (size_t i=0; i<N; i++) cbounds[i] = (const RTCBounds*) &children[i].prims.geomBounds;
          setNodeBounds(node,cbounds,N,userPtr);
          return node;
        },

        /* lambda function that updates BVH nodes */
        [&](const BVHBuilderBinnedSAH::BuildRecord& precord, const BVHBuilderBinnedSAH::BuildRecord* crecords, void* node, void** children, const size_t N) -> void* {
          setNodeChildren(node,children,N,userPtr);
          return node;
        },
        
        /* lambda function that creates BVH leaves */
        [&](const BVHBuilderBinnedSAH::BuildRecord& current, FastAllocator::ThreadLocal* alloc) -> void* {
          return createLeaf((RTCThreadLocalAllocator)alloc,prims+current.prims.begin(),current.prims.size(),userPtr);
        },
        
        /* progress monitor function */
        [&] (size_t dn) { 
          if (buildProgress) buildProgress(dn,userPtr);
        },
        
        (PrimRef*)prims,pinfo,settings);
        
      bvh->allocator.cleanup();
      return root;
    }

     void* rtcBuildBVHSpatialSAH(BVH* bvh,
                                 const RTCBuildSettings& settings,
                                 RTCBuildPrimitive* prims,
                                 size_t numPrimitives,
                                 RTCCreateNodeFunc createNode,
                                 RTCSetNodeChildrenFunc setNodeChildren,
                                 RTCSetNodeBoundsFunc setNodeBounds,
                                 RTCCreateLeafFunc createLeaf,
                                 RTCSplitPrimitiveFunc splitPrimitive,
                                 RTCBuildProgressFunc buildProgress,
                                 void* userPtr)
    {
      /* calculate priminfo */
      auto computeBounds = [&](const range<size_t>& r) -> CentGeomBBox3fa
        {
          CentGeomBBox3fa bounds(empty);
          for (size_t j=r.begin(); j<r.end(); j++)
            bounds.extend((BBox3fa&)prims[j]);
          return bounds;
        };
      const CentGeomBBox3fa bounds = 
        parallel_reduce(size_t(0),numPrimitives,size_t(1024),size_t(1024),CentGeomBBox3fa(empty), computeBounds, CentGeomBBox3fa::merge2);

      const PrimInfo pinfo(0,numPrimitives,bounds.geomBounds,bounds.centBounds);

      /* function that splits a build primitive */
      struct Splitter
      {
        Splitter (RTCSplitPrimitiveFunc splitPrimitive, unsigned geomID, unsigned primID, void* userPtr)
          : splitPrimitive(splitPrimitive), geomID(geomID), primID(primID), userPtr(userPtr) {}
        
        __forceinline void operator() (PrimRef& prim, const size_t dim, const float pos, PrimRef& left_o, PrimRef& right_o) const 
        {
          prim.geomID() &= BVHBuilderBinnedFastSpatialSAH::GEOMID_MASK;
          splitPrimitive((RTCBuildPrimitive&)prim,dim,pos,(RTCBounds&)left_o,(RTCBounds&)right_o,userPtr);
          left_o.geomID()  = geomID; left_o.primID()  = primID;
          right_o.geomID() = geomID; right_o.primID() = primID;
        }

        __forceinline void operator() (const BBox3fa& box, const size_t dim, const float pos, BBox3fa& left_o, BBox3fa& right_o) const 
        {
          PrimRef prim(box,geomID & BVHBuilderBinnedFastSpatialSAH::GEOMID_MASK,primID);
          splitPrimitive((RTCBuildPrimitive&)prim,dim,pos,(RTCBounds&)left_o,(RTCBounds&)right_o,userPtr);
        }
   
        RTCSplitPrimitiveFunc splitPrimitive;
        unsigned geomID;
        unsigned primID;
        void* userPtr;
      };

      /* build BVH */
      void* root = BVHBuilderBinnedFastSpatialSAH::build<void*>(
        
        /* thread local allocator for fast allocations */
        [&] () -> FastAllocator::ThreadLocal* { 
          return bvh->allocator.threadLocal();
        },

        /* lambda function that creates BVH nodes */
        [&] (BVHBuilderBinnedFastSpatialSAH::BuildRecord* children, const size_t N, FastAllocator::ThreadLocal* alloc) -> void*
        {
          void* node = createNode((RTCThreadLocalAllocator)alloc,N,userPtr);
          const RTCBounds* cbounds[GeneralBVHBuilder::MAX_BRANCHING_FACTOR];
          for (size_t i=0; i<N; i++) cbounds[i] = (const RTCBounds*) &children[i].prims.geomBounds;
          setNodeBounds(node,cbounds,N,userPtr);
          return node;
        },

        /* lambda function that updates BVH nodes */
        [&] (const BVHBuilderBinnedFastSpatialSAH::BuildRecord& precord, const BVHBuilderBinnedFastSpatialSAH::BuildRecord* crecords, void* node, void** children, const size_t N) -> void* {
          setNodeChildren(node,children,N,userPtr);
          return node;
        },
        
        /* lambda function that creates BVH leaves */
        [&] (const BVHBuilderBinnedFastSpatialSAH::BuildRecord& current, FastAllocator::ThreadLocal* alloc) -> void* {
          return createLeaf((RTCThreadLocalAllocator)alloc,prims+current.prims.begin(),current.prims.size(),userPtr);
        },
        
        /* returns the splitter */
        [&] ( const PrimRef& prim ) -> Splitter {
          return Splitter(splitPrimitive,prim.geomID(),prim.primID(),userPtr);
        },

        /* progress monitor function */
        [&] (size_t dn) { 
          if (buildProgress) buildProgress(dn,userPtr);
        },
        
        (PrimRef*)prims,
        pinfo.size()+settings.extraSpace,
        pinfo,settings);
        
      bvh->allocator.cleanup();
      return root;
    }

    RTCORE_API void* rtcBuildBVH(RTCBVH hbvh,
                                 const RTCBuildSettings& settings,
                                 RTCBuildPrimitive* prims,
                                 size_t numPrimitives,
                                 RTCCreateNodeFunc createNode,
                                 RTCSetNodeChildrenFunc setNodeChildren,
                                 RTCSetNodeBoundsFunc setNodeBounds,
                                 RTCCreateLeafFunc createLeaf,
                                 RTCSplitPrimitiveFunc splitPrimitive,
                                 RTCBuildProgressFunc buildProgress,
                                 void* userPtr)
    {
      BVH* bvh = (BVH*) hbvh;
      RTCORE_CATCH_BEGIN;
      RTCORE_TRACE(rtcBuildBVH);
      RTCORE_VERIFY_HANDLE(hbvh);
      RTCORE_VERIFY_HANDLE(createNode);
      RTCORE_VERIFY_HANDLE(setNodeChildren);
      RTCORE_VERIFY_HANDLE(setNodeBounds);
      RTCORE_VERIFY_HANDLE(createLeaf);

      /* if we made this BVH static, we can not re-build it anymore  */
      if (bvh->isStatic)
        throw_RTCError(RTC_INVALID_OPERATION,"static BVH cannot get rebuild");

      /* initialize the allocator */
      bvh->allocator.init_estimate(numPrimitives*sizeof(BBox3fa));
      bvh->allocator.reset();

      /* switch between differnet builders based on quality level */
      if (settings.quality == RTC_BUILD_QUALITY_LOW)
        return rtcBuildBVHMorton   (bvh,settings,prims,numPrimitives,createNode,setNodeChildren,setNodeBounds,createLeaf,buildProgress,userPtr);
      else if (settings.quality == RTC_BUILD_QUALITY_NORMAL)
        return rtcBuildBVHBinnedSAH(bvh,settings,prims,numPrimitives,createNode,setNodeChildren,setNodeBounds,createLeaf,buildProgress,userPtr);
      else if (settings.quality == RTC_BUILD_QUALITY_HIGH) {
        if (splitPrimitive == nullptr || settings.extraSpace == 0)
          return rtcBuildBVHBinnedSAH(bvh,settings,prims,numPrimitives,createNode,setNodeChildren,setNodeBounds,createLeaf,buildProgress,userPtr);
        else
          return rtcBuildBVHSpatialSAH(bvh,settings,prims,numPrimitives,createNode,setNodeChildren,setNodeBounds,createLeaf,splitPrimitive,buildProgress,userPtr);  
      }
      else
        throw_RTCError(RTC_INVALID_OPERATION,"invalid build quality");

      RTCORE_CATCH_END(bvh->device);
      return nullptr;
    }

    RTCORE_API void* rtcThreadLocalAlloc(RTCThreadLocalAllocator localAllocator, size_t bytes, size_t align)
    {
      RTCORE_CATCH_BEGIN;
      RTCORE_TRACE(rtcThreadLocalAlloc);
      FastAllocator::ThreadLocal* alloc = (FastAllocator::ThreadLocal*) localAllocator;
      return alloc->malloc(bytes,align);
      RTCORE_CATCH_END(((FastAllocator::ThreadLocal*) localAllocator)->alloc->getDevice());
      return nullptr;
    }

    RTCORE_API void rtcMakeStaticBVH(RTCBVH hbvh)
    {
      BVH* bvh = (BVH*) hbvh;
      RTCORE_CATCH_BEGIN;
      RTCORE_TRACE(rtcStaticBVH);
      RTCORE_VERIFY_HANDLE(hbvh);
      bvh->allocator.shrink();
      bvh->morton_src.clear();
      bvh->morton_tmp.clear();
      bvh->isStatic = true;
      RTCORE_CATCH_END(bvh->device);
    }

    RTCORE_API void rtcDeleteBVH(RTCBVH hbvh)
    {
      BVH* bvh = (BVH*) hbvh;
      Device* device = bvh ? bvh->device : nullptr;
      RTCORE_CATCH_BEGIN;
      RTCORE_TRACE(rtcDeleteAllocator);
      RTCORE_VERIFY_HANDLE(hbvh);
      delete bvh;
      RTCORE_CATCH_END(device);
    }
  }
}

