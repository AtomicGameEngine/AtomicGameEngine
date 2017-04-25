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

#include "bvh_intersector1.cpp"

namespace embree
{
  namespace isa
  {
    int getISA() {
      return VerifyMultiTargetLinking::getISA();
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// BVH4Intersector1 Definitions
    ////////////////////////////////////////////////////////////////////////////////

    IF_ENABLED_LINES(DEFINE_INTERSECTOR1(BVH4Line4iIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<LineMiIntersector1<SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_LINES(DEFINE_INTERSECTOR1(BVH4Line4iMBIntersector1,BVHNIntersector1<4 COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<LineMiMBIntersector1<SIMD_MODE(4) COMMA true> > >));

    IF_ENABLED_HAIR(DEFINE_INTERSECTOR1(BVH4Bezier1vIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<Bezier1vIntersector1> >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR1(BVH4Bezier1iIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<Bezier1iIntersector1> >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR1(BVH4Bezier1vIntersector1_OBB,BVHNIntersector1<4 COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersector1<Bezier1vIntersector1> >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR1(BVH4Bezier1iIntersector1_OBB,BVHNIntersector1<4 COMMA BVH_AN1_UN1 COMMA false COMMA ArrayIntersector1<Bezier1iIntersector1> >));
    IF_ENABLED_HAIR(DEFINE_INTERSECTOR1(BVH4Bezier1iMBIntersector1_OBB,BVHNIntersector1<4 COMMA BVH_AN2_UN2 COMMA false COMMA ArrayIntersector1<Bezier1iIntersector1MB> >));

  #if 0
    typedef Select2Intersector1<
      TriangleMIntersector1Moeller<4 COMMA 4 COMMA true>,
      TriangleMvMBIntersector1Moeller<4 COMMA 4 COMMA true> > Intersector1_Triangle4Moeller_Triangle4vMBMoeller;
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4XfmTriangle4Intersector1Moeller,BVHNIntersector1<4 COMMA BVH_TN_AN1_AN2 COMMA false COMMA Intersector1_Triangle4Moeller_Triangle4vMBMoeller>));
  #else
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4XfmTriangle4Intersector1Moeller,BVHNIntersector1<4 COMMA BVH_TN_AN1 COMMA false COMMA ArrayIntersector1<TriangleMIntersector1Moeller<4 COMMA 4 COMMA true> > >));
  #endif

    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4Intersector1Moeller,  BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<TriangleMIntersector1Moeller  <SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4iIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<TriangleMiIntersector1Moeller <SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4vIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersector1<TriangleMvIntersector1Pluecker<SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4iIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersector1<TriangleMiIntersector1Pluecker<SIMD_MODE(4) COMMA true> > >));

    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4vMBIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<TriangleMvMBIntersector1Moeller <SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4iMBIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<TriangleMiMBIntersector1Moeller <SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4vMBIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersector1<TriangleMvMBIntersector1Pluecker<SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(BVH4Triangle4iMBIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersector1<TriangleMiMBIntersector1Pluecker<SIMD_MODE(4) COMMA true> > >));

    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4vIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<QuadMvIntersector1Moeller <4 COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4iIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<QuadMiIntersector1Moeller <4 COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4vIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersector1<QuadMvIntersector1Pluecker<4 COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4iIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true  COMMA ArrayIntersector1<QuadMiIntersector1Pluecker<4 COMMA true> > >));

    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4iMBIntersector1Moeller, BVHNIntersector1<4 COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<QuadMiMBIntersector1Moeller <4 COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(BVH4Quad4iMBIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_AN2 COMMA true  COMMA ArrayIntersector1<QuadMiMBIntersector1Pluecker<4 COMMA true> > >));

    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR1(BVH4Subdivpatch1Intersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1CachedIntersector1<false>>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR1(BVH4Subdivpatch1EagerIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1EagerIntersector1>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR1(BVH4Subdivpatch1CachedIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA true COMMA SubdivPatch1CachedIntersector1<true>>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR1(BVH4Subdivpatch1MBlurIntersector1,BVHNIntersector1<4 COMMA BVH_AN2 COMMA true COMMA SubdivPatch1MBlurCachedIntersector1<false>>));
    IF_ENABLED_SUBDIV(DEFINE_INTERSECTOR1(BVH4Subdivpatch1MBlurCachedIntersector1,BVHNIntersector1<4 COMMA BVH_AN2 COMMA true COMMA SubdivPatch1MBlurCachedIntersector1<true>>));

    IF_ENABLED_USER(DEFINE_INTERSECTOR1(BVH4VirtualIntersector1,BVHNIntersector1<4 COMMA BVH_AN1 COMMA false COMMA ArrayIntersector1<ObjectIntersector1<false>> >));
    IF_ENABLED_USER(DEFINE_INTERSECTOR1(BVH4VirtualMBIntersector1,BVHNIntersector1<4 COMMA BVH_AN2 COMMA false COMMA ArrayIntersector1<ObjectIntersector1<true>> >));

    IF_ENABLED_TRIS(DEFINE_INTERSECTOR1(QBVH4Triangle4iIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_QN1 COMMA false COMMA ArrayIntersector1<TriangleMiIntersector1Pluecker<SIMD_MODE(4) COMMA true> > >));
    IF_ENABLED_QUADS(DEFINE_INTERSECTOR1(QBVH4Quad4iIntersector1Pluecker,BVHNIntersector1<4 COMMA BVH_QN1 COMMA false COMMA ArrayIntersector1<QuadMiIntersector1Pluecker<4 COMMA true> > >));
  }
}
