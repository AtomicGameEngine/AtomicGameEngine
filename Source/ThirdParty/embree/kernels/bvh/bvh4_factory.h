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
#include "../common/isa.h"
#include "../common/accel.h"
#include "../common/scene.h"

namespace embree
{
  /*! BVH4 instantiations */
  class BVH4Factory
  {
  public:
    enum class BuildVariant     { STATIC, DYNAMIC, HIGH_QUALITY };
    enum class IntersectVariant { FAST, ROBUST };

    BVH4Factory(int bfeatures, int ifeatures);

  public:
    Accel* BVH4Bezier1v(Scene* scene);
    Accel* BVH4Bezier1i(Scene* scene);
    Accel* BVH4Line4i(Scene* scene, BuildVariant bvariant = BuildVariant::STATIC);
    Accel* BVH4Line4iMB(Scene* scene);

    Accel* BVH4OBBBezier1v(Scene* scene);
    Accel* BVH4OBBBezier1i(Scene* scene);
    Accel* BVH4OBBBezier1iMB(Scene* scene);
    
    Accel* BVH4Triangle4   (Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);
    Accel* BVH4Triangle4v  (Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::ROBUST);
    Accel* BVH4Triangle4i  (Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);
    Accel* BVH4Triangle4vMB(Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);
    Accel* BVH4Triangle4iMB(Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);

    Accel* BVH4Quad4v  (Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);
    Accel* BVH4Quad4i  (Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);
    Accel* BVH4Quad4iMB(Scene* scene, BuildVariant bvariant = BuildVariant::STATIC, IntersectVariant ivariant = IntersectVariant::FAST);

    Accel* BVH4QuantizedTriangle4i(Scene* scene);
    Accel* BVH4QuantizedQuad4i(Scene* scene);
 
    Accel* BVH4SubdivPatch1Eager(Scene* scene);
    Accel* BVH4SubdivPatch1(Scene* scene, bool cached);
    Accel* BVH4SubdivPatch1MBlur(Scene* scene, bool cached);
    Accel* BVH4UserGeometry(Scene* scene, BuildVariant bvariant = BuildVariant::STATIC);
    Accel* BVH4UserGeometryMB(Scene* scene);
    Accel* BVH4InstancedBVH4Triangle4ObjectSplit(Scene* scene);

  private:
    void selectBuilders(int features);
    void selectIntersectors(int features);
    
  private:
    Accel::Intersectors BVH4Line4iIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4Line4iMBIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4Bezier1vIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4Bezier1iIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4Bezier1vIntersectors_OBB(BVH4* bvh);
    Accel::Intersectors BVH4Bezier1iIntersectors_OBB(BVH4* bvh);
    Accel::Intersectors BVH4Bezier1iMBIntersectors_OBB(BVH4* bvh);
    Accel::Intersectors BVH4Triangle4IntersectorsInstancing(BVH4* bvh);

    Accel::Intersectors BVH4Triangle4Intersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Triangle4vIntersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Triangle4iIntersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Triangle4vMBIntersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Triangle4iMBIntersectors(BVH4* bvh, IntersectVariant ivariant);

    Accel::Intersectors BVH4Quad4vIntersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Quad4iIntersectors(BVH4* bvh, IntersectVariant ivariant);
    Accel::Intersectors BVH4Quad4iMBIntersectors(BVH4* bvh, IntersectVariant ivariant);

    Accel::Intersectors QBVH4Quad4iIntersectors(BVH4* bvh);
    Accel::Intersectors QBVH4Triangle4iIntersectors(BVH4* bvh);

    Accel::Intersectors BVH4UserGeometryIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4UserGeometryMBIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4SubdivPatch1Intersectors(BVH4* bvh);
    Accel::Intersectors BVH4SubdivPatch1EagerIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4SubdivPatch1CachedIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4SubdivPatch1MBlurIntersectors(BVH4* bvh);
    Accel::Intersectors BVH4SubdivPatch1MBlurCachedIntersectors(BVH4* bvh);
    
    static void createLineSegmentsLine4i(LineSegments* mesh, AccelData*& accel, Builder*& builder);

    static void createTriangleMeshTriangle4Morton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createTriangleMeshTriangle4vMorton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createTriangleMeshTriangle4iMorton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createTriangleMeshTriangle4(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createTriangleMeshTriangle4v(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createTriangleMeshTriangle4i(TriangleMesh* mesh, AccelData*& accel, Builder*& builder);

    static void createQuadMeshQuad4v(QuadMesh* mesh, AccelData*& accel, Builder*& builder);
    static void createQuadMeshQuad4vMorton(QuadMesh* mesh, AccelData*& accel, Builder*& builder);

    static void createAccelSetMesh(AccelSet* mesh, AccelData*& accel, Builder*& builder);
    
  private:
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Line4iIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Line4iMBIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Bezier1vIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Bezier1vIntersector1_OBB);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iIntersector1_OBB);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iMBIntersector1_OBB);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4XfmTriangle4Intersector1Moeller);

    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4Intersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vIntersector1Pluecker);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iIntersector1Pluecker);

    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vMBIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iMBIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vMBIntersector1Pluecker);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iMBIntersector1Pluecker);

    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4vIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4iIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4vIntersector1Pluecker);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4iIntersector1Pluecker);

    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4iMBIntersector1Moeller);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Quad4iMBIntersector1Pluecker);

    DEFINE_SYMBOL2(Accel::Intersector1,QBVH4Triangle4iIntersector1Pluecker);
    DEFINE_SYMBOL2(Accel::Intersector1,QBVH4Quad4iIntersector1Pluecker);

    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1Intersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1EagerIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1CachedIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1MBlurIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1MBlurCachedIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4VirtualIntersector1);
    DEFINE_SYMBOL2(Accel::Intersector1,BVH4VirtualMBIntersector1);
        
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Line4iIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Line4iMBIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Bezier1vIntersector4Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iIntersector4Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Bezier1vIntersector4Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iIntersector4Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iMBIntersector4Hybrid_OBB);

    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vIntersector4HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iIntersector4HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vMBIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iMBIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vMBIntersector4HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iMBIntersector4HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4iIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4iIntersector4HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4iMBIntersector4HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Quad4iMBIntersector4HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1Intersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1EagerIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1CachedIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1MBlurIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1MBlurCachedIntersector4);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4VirtualIntersector4Chunk);
    DEFINE_SYMBOL2(Accel::Intersector4,BVH4VirtualMBIntersector4Chunk);
    
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Line4iIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Line4iMBIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Bezier1vIntersector8Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iIntersector8Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Bezier1vIntersector8Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iIntersector8Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iMBIntersector8Hybrid_OBB);

    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vIntersector8HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iIntersector8HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vMBIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iMBIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vMBIntersector8HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iMBIntersector8HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4iIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4iIntersector8HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4iMBIntersector8HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Quad4iMBIntersector8HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1Intersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1EagerIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1CachedIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1MBlurIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1MBlurCachedIntersector8);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4VirtualIntersector8Chunk);
    DEFINE_SYMBOL2(Accel::Intersector8,BVH4VirtualMBIntersector8Chunk);
    
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Line4iIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Line4iMBIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Bezier1vIntersector16Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iIntersector16Hybrid);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Bezier1vIntersector16Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iIntersector16Hybrid_OBB);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iMBIntersector16Hybrid_OBB);

    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4Intersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4Intersector16HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vIntersector16HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iIntersector16HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vMBIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iMBIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vMBIntersector16HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iMBIntersector16HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4iIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridPluecker);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4iIntersector16HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4iMBIntersector16HybridMoeller);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Quad4iMBIntersector16HybridPluecker);

    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1Intersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1EagerIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1CachedIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1MBlurIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1MBlurCachedIntersector16);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4VirtualIntersector16Chunk);
    DEFINE_SYMBOL2(Accel::Intersector16,BVH4VirtualMBIntersector16Chunk);

    DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Line4iIntersectorStream);
    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Line4iMBIntersectorStream);
    DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1vIntersectorStream);
    DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iIntersectorStream);
    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1vIntersectorStream_OBB);
    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iIntersectorStream_OBB);
    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iMBIntersectorStream_OBB);

    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Triangle4IntersectorStreamMoeller);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Triangle4IntersectorStreamMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Triangle4iIntersectorStreamMoeller);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Triangle4vIntersectorStreamPluecker);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Triangle4iIntersectorStreamPluecker);

    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Quad4vIntersectorStreamMoeller);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Quad4vIntersectorStreamMoellerNoFilter);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Quad4iIntersectorStreamMoeller);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Quad4vIntersectorStreamPluecker);
    DEFINE_SYMBOL2(Accel::IntersectorN, BVH4Quad4iIntersectorStreamPluecker);

    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4Subdivpatch1CachedIntersectorStream);
    DEFINE_SYMBOL2(Accel::IntersectorN,BVH4VirtualIntersectorStream);
    //DEFINE_SYMBOL2(Accel::IntersectorN,BVH4VirtualMBIntersectorStream);
    //DEFINE_SYMBOL2(Accel::IntersectorN,QBVH4Triangle4IntersectorStreamMoeller);
    
    DEFINE_BUILDER2(void,Scene,const createLineSegmentsAccelTy,BVH4BuilderTwoLevelLineSegmentsSAH);
    DEFINE_BUILDER2(void,Scene,const createTriangleMeshAccelTy,BVH4BuilderTwoLevelTriangleMeshSAH);
    DEFINE_BUILDER2(void,Scene,const createTriangleMeshAccelTy,BVH4BuilderInstancingTriangleMeshSAH);
    DEFINE_BUILDER2(void,Scene,const createQuadMeshAccelTy,BVH4BuilderTwoLevelQuadMeshSAH);
    DEFINE_BUILDER2(void,Scene,const createAccelSetAccelTy,BVH4BuilderTwoLevelVirtualSAH);
    //DEFINE_BUILDER2(void,Scene,const createQuadMeshAccelTy,BVH4BuilderInstancingQuadMeshSAH);
    
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Bezier1vBuilder_OBB_New);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Bezier1iBuilder_OBB_New);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Bezier1iMBBuilder_OBB_New);
    
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4SceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4vSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4iSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4vMBSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4iMBSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Quad4vSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Quad4iSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Quad4iMBSceneBuilderSAH);
    
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4SceneBuilderFastSpatialSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4vSceneBuilderFastSpatialSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Triangle4iSceneBuilderFastSpatialSAH);

    DEFINE_BUILDER2(void,Scene,size_t,BVH4Quad4vSceneBuilderFastSpatialSAH);
    
    DEFINE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMeshBuilderSAH);
    //DEFINE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMBMeshBuilderSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshBuilderSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshBuilderSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshBuilderSAH);
    DEFINE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4vMeshBuilderSAH);
    //DEFINE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4iMeshBuilderSAH);

    DEFINE_BUILDER2(void,Scene,size_t,BVH4Bezier1vSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Bezier1iSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Line4iSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4Line4iMBSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4VirtualSceneBuilderSAH);
    DEFINE_BUILDER2(void,AccelSet,size_t,BVH4VirtualMeshBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4VirtualMBSceneBuilderSAH);
    
    DEFINE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1EagerBuilderBinnedSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1CachedBuilderBinnedSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH);
    
    DEFINE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMeshRefitSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshRefitSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshRefitSAH);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshRefitSAH);
    DEFINE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4vMeshRefitSAH);
    DEFINE_BUILDER2(void,AccelSet,size_t,BVH4VirtualMeshRefitSAH);
    
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshBuilderMortonGeneral);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshBuilderMortonGeneral);
    DEFINE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshBuilderMortonGeneral);
    DEFINE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4vMeshBuilderMortonGeneral);
    DEFINE_BUILDER2(void,AccelSet,size_t,BVH4VirtualMeshBuilderMortonGeneral);

    DEFINE_BUILDER2(void,Scene,size_t,BVH4QuantizedTriangle4iSceneBuilderSAH);
    DEFINE_BUILDER2(void,Scene,size_t,BVH4QuantizedQuad4iSceneBuilderSAH);
    
  };
}
