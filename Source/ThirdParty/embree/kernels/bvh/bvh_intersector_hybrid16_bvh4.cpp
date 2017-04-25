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

#include "bvh_intersector_hybrid.cpp"

namespace embree
{
  namespace isa
  {
    ////////////////////////////////////////////////////////////////////////////////
    /// BVH4Intersector16 Definitions
    ////////////////////////////////////////////////////////////////////////////////

    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4Intersector16HybridMoeller,         BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA TriangleMIntersectorKMoeller  <SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4Intersector16HybridMoellerNoFilter, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA TriangleMIntersectorKMoeller  <SIMD_MODE(4) COMMA 16 COMMA false> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4iIntersector16HybridMoeller,        BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA TriangleMiIntersectorKMoeller <SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4vIntersector16HybridPluecker,       BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA TriangleMvIntersectorKPluecker<SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4iIntersector16HybridPluecker,       BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA TriangleMiIntersectorKPluecker<SIMD_MODE(4) COMMA 16 COMMA true> > >));

    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4vMBIntersector16HybridMoeller,  BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA TriangleMvMBIntersectorKMoeller <SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4iMBIntersector16HybridMoeller,  BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA TriangleMiMBIntersectorKMoeller <SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4vMBIntersector16HybridPluecker, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA TriangleMvMBIntersectorKPluecker<SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR16(BVH4Triangle4iMBIntersector16HybridPluecker, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA TriangleMiMBIntersectorKPluecker<SIMD_MODE(4) COMMA 16 COMMA true> > >));

    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4vIntersector16HybridMoeller,        BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA QuadMvIntersectorKMoeller <4 COMMA 16 COMMA true > > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4vIntersector16HybridMoellerNoFilter,BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA QuadMvIntersectorKMoeller <4 COMMA 16 COMMA false> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4iIntersector16HybridMoeller,        BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA QuadMiIntersectorKMoeller <4 COMMA 16 COMMA true > > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4vIntersector16HybridPluecker,       BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA QuadMvIntersectorKPluecker<4 COMMA 16 COMMA true > > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4iIntersector16HybridPluecker,       BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA QuadMiIntersectorKPluecker<4 COMMA 16 COMMA true > > >));

    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4iMBIntersector16HybridMoeller, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA QuadMiMBIntersectorKMoeller <4 COMMA 16 COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR16(BVH4Quad4iMBIntersector16HybridPluecker,BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersectorK_1<16 COMMA QuadMiMBIntersectorKPluecker<4 COMMA 16 COMMA true> > >));

    IF_ENABLED_LINES(DEFINE_INTERSECTOR16(BVH4Line4iIntersector16,  BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA LineMiIntersectorK  <SIMD_MODE(4) COMMA 16 COMMA true> > >));
    IF_ENABLED_LINES(DEFINE_INTERSECTOR16(BVH4Line4iMBIntersector16,BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA LineMiMBIntersectorK<SIMD_MODE(4) COMMA 16 COMMA true> > >));

    IF_ENABLED_HAIR(DEFINE_INTERSECTOR16(BVH4Bezier1vIntersector16Hybrid, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA Bezier1vIntersectorK<16> > >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR16(BVH4Bezier1iIntersector16Hybrid, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA Bezier1iIntersectorK<16> > >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR16(BVH4Bezier1vIntersector16Hybrid_OBB, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA Bezier1vIntersectorK<16> > >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR16(BVH4Bezier1iIntersector16Hybrid_OBB, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA Bezier1iIntersectorK<16> > >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR16(BVH4Bezier1iMBIntersector16Hybrid_OBB,BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN2_UN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA Bezier1iIntersectorKMB<16> > >));

    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR16(BVH4Subdivpatch1Intersector16, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1Intersector16>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR16(BVH4Subdivpatch1EagerIntersector16, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1EagerIntersector16>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR16(BVH4Subdivpatch1CachedIntersector16, BVHNIntersectorKHybrid<4 COMMA 16 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1CachedIntersector16>));

    IF_ENABLED_USER(DEFINE_INTERSECTOR16(BVH4VirtualIntersector16Chunk, BVHNIntersectorKChunk<4 COMMA 16 COMMA BVH_AN1 COMMA false COMMA ArrayIntersectorK_1<16 COMMA ObjectIntersector16> >));
    IF_ENABLED_USER(DEFINE_INTERSECTOR16(BVH4VirtualMBIntersector16Chunk, BVHNIntersectorKChunk<4 COMMA 16 COMMA BVH_AN2 COMMA false COMMA ArrayIntersectorK_1<16 COMMA ObjectIntersector16MB> >));
  }
}

