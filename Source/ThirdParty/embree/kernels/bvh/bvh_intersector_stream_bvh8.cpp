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

#include "bvh_intersector_stream.cpp"

namespace embree
{
  namespace isa
  {
    ////////////////////////////////////////////////////////////////////////////////
    /// BVH8IntersectorStream Definitions
    ////////////////////////////////////////////////////////////////////////////////

    // disabled for now
    //IF_ENABLED_LINES(DEFINE_INTERSECTORN(BVH8Line4iIntersectorStream,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<LineMiIntersector1<SIMD_MODE(4) COMMA true> > >));

    IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4IntersectorStreamMoeller,         BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Triangle4IntersectorStreamMoeller>));
    IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4IntersectorStreamMoellerNoFilter, BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Triangle4IntersectorStreamMoellerNoFilter>));
    IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4iIntersectorStreamMoeller,        BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Triangle4iIntersectorStreamMoeller>));
    IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4vIntersectorStreamPluecker,       BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA true  COMMA Triangle4vIntersectorStreamPluecker>));
    IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4iIntersectorStreamPluecker,       BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA true  COMMA Triangle4iIntersectorStreamPluecker>));

    IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4vIntersectorStreamMoeller,         BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Quad4vIntersectorStreamMoeller>));
    IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4vIntersectorStreamMoellerNoFilter, BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Quad4vIntersectorStreamMoellerNoFilter>));
    IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4iIntersectorStreamMoeller,         BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA Quad4iIntersectorStreamMoeller>));
    IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4vIntersectorStreamPluecker,        BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA true  COMMA Quad4vIntersectorStreamPluecker>));
    IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4iIntersectorStreamPluecker,        BVHNIntersectorStream<SIMD_MODE(8) COMMA VSIZEX COMMA BVH_AN1 COMMA true  COMMA Quad4iIntersectorStreamPluecker>));

    //IF_ENABLED_USER(DEFINE_INTERSECTORN(BVH4VirtualIntersectorStream,BVHNIntersectorStream<SIMD_MODE(4) COMMA VSIZEX COMMA BVH_AN1 COMMA false COMMA ObjectIntersector1<false>>));

    //IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4vMBIntersectorStreamMoeller,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<TriangleMvMBIntersector1Moeller<SIMD_MODE(4) COMMA true> > >));

    //IF_ENABLED_TRIS(DEFINE_INTERSECTORN(BVH8Triangle4IntersectorStreamMoellerNoFilter, BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<TriangleMIntersector1Moeller<SIMD_MODE(4) COMMA false> > >));

    //IF_ENABLED_LINES(DEFINE_INTERSECTORN(BVH8Line4iMBIntersectorStream,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<LineMiMBIntersector1<SIMD_MODE(4) COMMA true> > >));

    //IF_ENABLED_HAIR(DEFINE_INTERSECTORN(BVH8Bezier1vIntersectorStream_OBB,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersector1<Bezier1vIntersector1> >));
    //IF_ENABLED_HAIR(DEFINE_INTERSECTORN(BVH8Bezier1iIntersectorStream_OBB,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersector1<Bezier1iIntersector1> >));
    //IF_ENABLED_HAIR(DEFINE_INTERSECTORN(BVH8Bezier1iMBIntersectorStream_OBB,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN2_UN2 COMMA false COMMA ArrayIntersector1<Bezier1iIntersector1MB> >));

    //IF_ENABLED_QUADS(DEFINE_INTERSECTORN(BVH8Quad4iMBIntersectorStreamPluecker,BVHNIntersectorStream<SIMD_MODE(8) COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<QuadMiMBIntersector1Pluecker<4 COMMA true> > >));
  }
}
