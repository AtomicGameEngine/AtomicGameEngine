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

#include "bvh_builder.h"

namespace embree
{
  namespace isa
  {
    template<int N>
    typename BVHN<N>::NodeRef BVHNBuilderVirtual<N>::BVHNBuilderV::build(FastAllocator* allocator, BuildProgressMonitor& progressFunc, PrimRef* prims, const PrimInfo& pinfo, GeneralBVHBuilder::Settings settings)
    {
      auto createLeafFunc = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> NodeRef {
        return createLeaf(current,alloc);
      };
      
      settings.branchingFactor = N;
      settings.maxDepth = BVH::maxBuildDepthLeaf;
      return BVHBuilderBinnedSAH::build<NodeRef>
        (FastAllocator::CreateAlloc2(allocator),typename BVH::AlignedNode::Create2(),typename BVH::AlignedNode::Set3(allocator,prims),createLeafFunc,progressFunc,prims,pinfo,settings);
    }


    template<int N>
    typename BVHN<N>::NodeRef BVHNBuilderQuantizedVirtual<N>::BVHNBuilderV::build(FastAllocator* allocator, BuildProgressMonitor& progressFunc, PrimRef* prims, const PrimInfo& pinfo, GeneralBVHBuilder::Settings settings)
    {
      auto createLeafFunc = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> NodeRef {
        return createLeaf(current,alloc);
      };
            
      settings.branchingFactor = N;
      settings.maxDepth = BVH::maxBuildDepthLeaf;
      return BVHBuilderBinnedSAH::build<NodeRef>
        (FastAllocator::CreateAlloc2(allocator),typename BVH::QuantizedNode::Create2(),typename BVH::QuantizedNode::Set2(),createLeafFunc,progressFunc,prims,pinfo,settings);
    }

    template<int N>
    std::tuple<typename BVHN<N>::NodeRef,LBBox3fa> BVHNBuilderMblurVirtual<N>::BVHNBuilderV::build(FastAllocator* allocator, BuildProgressMonitor& progressFunc, PrimRef* prims, const PrimInfo& pinfo, GeneralBVHBuilder::Settings settings)
    {
      auto createLeafFunc = [&] (const BVHBuilderBinnedSAH::BuildRecord& current, Allocator* alloc) -> std::pair<NodeRef,LBBox3fa> {
        return createLeaf(current,alloc);
      };

      settings.branchingFactor = N;
      settings.maxDepth = BVH::maxBuildDepthLeaf;
      return BVHBuilderBinnedSAH::build<std::pair<NodeRef,LBBox3fa>>
        (FastAllocator::CreateAlloc2(allocator),typename BVH::AlignedNodeMB::Create2(),typename BVH::AlignedNodeMB::Set2(),createLeafFunc,progressFunc,prims,pinfo,settings);
    }

    template struct BVHNBuilderVirtual<4>;
    template struct BVHNBuilderQuantizedVirtual<4>;
    template struct BVHNBuilderMblurVirtual<4>;    

#if defined(__AVX__)
    template struct BVHNBuilderVirtual<8>;
    template struct BVHNBuilderQuantizedVirtual<8>;
    template struct BVHNBuilderMblurVirtual<8>;
#endif
  }
}
