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

#include "bvh4_factory.h"
#include "../bvh/bvh.h"

#include "../geometry/bezier1v.h"
#include "../geometry/bezier1i.h"
#include "../geometry/linei.h"
#include "../geometry/triangle.h"
#include "../geometry/trianglev.h"
#include "../geometry/trianglei.h"
#include "../geometry/trianglev_mb.h"
#include "../geometry/trianglei_mb.h"
#include "../geometry/quadv.h"
#include "../geometry/quadi.h"
#include "../geometry/quadi_mb.h"
#include "../geometry/subdivpatch1cached.h"
#include "../geometry/object.h"
#include "../common/accelinstance.h"

namespace embree
{
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Line4iIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Line4iMBIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Bezier1vIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Bezier1vIntersector1_OBB);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iIntersector1_OBB);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Bezier1iMBIntersector1_OBB);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4XfmTriangle4Intersector1Moeller);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4Intersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vIntersector1Pluecker);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iIntersector1Pluecker);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vMBIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iMBIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4vMBIntersector1Pluecker);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Triangle4iMBIntersector1Pluecker);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4vIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4iIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4vIntersector1Pluecker);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4iIntersector1Pluecker);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4iMBIntersector1Moeller);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Quad4iMBIntersector1Pluecker);

  DECLARE_SYMBOL2(Accel::Intersector1,QBVH4Triangle4iIntersector1Pluecker);
  DECLARE_SYMBOL2(Accel::Intersector1,QBVH4Quad4iIntersector1Pluecker);

  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1Intersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1EagerIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1CachedIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1MBlurIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4Subdivpatch1MBlurCachedIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4VirtualIntersector1);
  DECLARE_SYMBOL2(Accel::Intersector1,BVH4VirtualMBIntersector1);
  
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Line4iIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Line4iMBIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Bezier1vIntersector4Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iIntersector4Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Bezier1vIntersector4Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iIntersector4Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Bezier1iMBIntersector4Hybrid_OBB);

  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vIntersector4HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iIntersector4HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vMBIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iMBIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4vMBIntersector4HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Triangle4iMBIntersector4HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4iIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4vIntersector4HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4iIntersector4HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4iMBIntersector4HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Quad4iMBIntersector4HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1Intersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1EagerIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1CachedIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1MBlurIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4Subdivpatch1MBlurCachedIntersector4);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4VirtualIntersector4Chunk);
  DECLARE_SYMBOL2(Accel::Intersector4,BVH4VirtualMBIntersector4Chunk);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Line4iIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Line4iMBIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Bezier1vIntersector8Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iIntersector8Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Bezier1vIntersector8Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iIntersector8Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Bezier1iMBIntersector8Hybrid_OBB);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vIntersector8HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iIntersector8HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vMBIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iMBIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4vMBIntersector8HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Triangle4iMBIntersector8HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4iIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4vIntersector8HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4iIntersector8HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4iMBIntersector8HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Quad4iMBIntersector8HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1Intersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1EagerIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1CachedIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1MBlurIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4Subdivpatch1MBlurCachedIntersector8);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4VirtualIntersector8Chunk);
  DECLARE_SYMBOL2(Accel::Intersector8,BVH4VirtualMBIntersector8Chunk);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Line4iIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Line4iMBIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Bezier1vIntersector16Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iIntersector16Hybrid);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Bezier1vIntersector16Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iIntersector16Hybrid_OBB);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Bezier1iMBIntersector16Hybrid_OBB);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4Intersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4Intersector16HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vIntersector16HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iIntersector16HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vMBIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iMBIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4vMBIntersector16HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Triangle4iMBIntersector16HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4iIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4vIntersector16HybridPluecker);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4iIntersector16HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4iMBIntersector16HybridMoeller);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Quad4iMBIntersector16HybridPluecker);

  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1Intersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1EagerIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1CachedIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1MBlurIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4Subdivpatch1MBlurCachedIntersector16);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4VirtualIntersector16Chunk);
  DECLARE_SYMBOL2(Accel::Intersector16,BVH4VirtualMBIntersector16Chunk);

  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Line4iIntersectorStream);
  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Line4iMBIntersectorStream);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1vIntersectorStream);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iIntersectorStream);
  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1vIntersectorStream_OBB);
  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iIntersectorStream_OBB);
  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Bezier1iMBIntersectorStream_OBB);

  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Triangle4IntersectorStreamMoeller);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Triangle4IntersectorStreamMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Triangle4iIntersectorStreamMoeller);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Triangle4vIntersectorStreamPluecker);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Triangle4iIntersectorStreamPluecker);

  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Quad4vIntersectorStreamMoeller);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Quad4vIntersectorStreamMoellerNoFilter);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Quad4iIntersectorStreamMoeller);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Quad4vIntersectorStreamPluecker);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Quad4iIntersectorStreamPluecker);

  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4Subdivpatch1CachedIntersectorStream);
  DECLARE_SYMBOL2(Accel::IntersectorN,BVH4VirtualIntersectorStream);
  //DECLARE_SYMBOL2(Accel::IntersectorN,BVH4VirtualMBIntersectorStream);
  
  DECLARE_BUILDER2(void,Scene,const createLineSegmentsAccelTy,BVH4BuilderTwoLevelLineSegmentsSAH);
  DECLARE_BUILDER2(void,Scene,const createTriangleMeshAccelTy,BVH4BuilderTwoLevelTriangleMeshSAH);
  DECLARE_BUILDER2(void,Scene,const createTriangleMeshAccelTy,BVH4BuilderInstancingTriangleMeshSAH);
  DECLARE_BUILDER2(void,Scene,const createQuadMeshAccelTy,BVH4BuilderTwoLevelQuadMeshSAH);
  DECLARE_BUILDER2(void,Scene,const createAccelSetAccelTy,BVH4BuilderTwoLevelVirtualSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Bezier1vBuilder_OBB_New);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Bezier1iBuilder_OBB_New);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Bezier1iMBBuilder_OBB_New);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4SceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4vSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4iSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4vMBSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4iMBSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4QuantizedTriangle4iSceneBuilderSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Quad4vSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Quad4iSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Quad4iMBSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4QuantizedQuad4iSceneBuilderSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4SceneBuilderFastSpatialSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4vSceneBuilderFastSpatialSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Triangle4iSceneBuilderFastSpatialSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Quad4vSceneBuilderFastSpatialSAH);

  DECLARE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMeshBuilderSAH);
  //DECLARE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMBMeshBuilderSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshBuilderSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshBuilderSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshBuilderSAH);
  DECLARE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4vMeshBuilderSAH);
  //DECLARE_BUILDER2(void,QuadMesh,size_t,BVH4Quad4iMeshBuilderSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4Bezier1vSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Bezier1iSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Line4iSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4Line4iMBSceneBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4VirtualSceneBuilderSAH);
  DECLARE_BUILDER2(void,AccelSet,size_t,BVH4VirtualMeshBuilderSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4VirtualMBSceneBuilderSAH);

  DECLARE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1EagerBuilderBinnedSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1CachedBuilderBinnedSAH);
  DECLARE_BUILDER2(void,Scene,size_t,BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH);

  DECLARE_BUILDER2(void,LineSegments,size_t,BVH4Line4iMeshRefitSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshRefitSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshRefitSAH);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshRefitSAH);
  DECLARE_BUILDER2(void,QuadMesh    ,size_t,BVH4Quad4vMeshRefitSAH);
  DECLARE_BUILDER2(void,AccelSet    ,size_t,BVH4VirtualMeshRefitSAH);

  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4MeshBuilderMortonGeneral);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4vMeshBuilderMortonGeneral);
  DECLARE_BUILDER2(void,TriangleMesh,size_t,BVH4Triangle4iMeshBuilderMortonGeneral);
  DECLARE_BUILDER2(void,QuadMesh    ,size_t,BVH4Quad4vMeshBuilderMortonGeneral);
  DECLARE_BUILDER2(void,AccelSet    ,size_t,BVH4VirtualMeshBuilderMortonGeneral);

  BVH4Factory::BVH4Factory(int bfeatures, int ifeatures)
  {
    selectBuilders(bfeatures);
    selectIntersectors(ifeatures);
  }

  void BVH4Factory::selectBuilders(int features)
  {
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderTwoLevelLineSegmentsSAH));
    IF_ENABLED_TRIS (SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderTwoLevelTriangleMeshSAH));
    IF_ENABLED_TRIS (SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderInstancingTriangleMeshSAH));
    IF_ENABLED_QUADS (SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderTwoLevelQuadMeshSAH));
    IF_ENABLED_USER (SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderTwoLevelVirtualSAH));
    //IF_ENABLED_QUADS (SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4BuilderInstancingQuadMeshSAH));

    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1vBuilder_OBB_New));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1iBuilder_OBB_New));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1iMBBuilder_OBB_New));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4SceneBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4vSceneBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4iSceneBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vMBSceneBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iMBSceneBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4QuantizedTriangle4iSceneBuilderSAH));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4vSceneBuilderSAH));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4iSceneBuilderSAH));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Quad4iMBSceneBuilderSAH));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4QuantizedQuad4iSceneBuilderSAH));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4SceneBuilderFastSpatialSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vSceneBuilderFastSpatialSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iSceneBuilderFastSpatialSAH));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Quad4vSceneBuilderFastSpatialSAH));

    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Line4iMeshBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4MeshBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4vMeshBuilderSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4iMeshBuilderSAH));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4vMeshBuilderSAH));
    //IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4iMeshBuilderSAH));
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Line4iSceneBuilderSAH));
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Line4iMBSceneBuilderSAH));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1vSceneBuilderSAH));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1iSceneBuilderSAH));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4VirtualSceneBuilderSAH));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4VirtualMeshBuilderSAH));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4VirtualMBSceneBuilderSAH));

    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4SubdivPatch1EagerBuilderBinnedSAH));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4SubdivPatch1CachedBuilderBinnedSAH));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH));

    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Line4iMeshRefitSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4MeshRefitSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4vMeshRefitSAH));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4iMeshRefitSAH));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4vMeshRefitSAH));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4VirtualMeshRefitSAH));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4MeshBuilderMortonGeneral));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4vMeshBuilderMortonGeneral));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Triangle4iMeshBuilderMortonGeneral));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4Quad4vMeshBuilderMortonGeneral));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_AVX_AVX512KNL_AVX512SKX(features,BVH4VirtualMeshBuilderMortonGeneral));
  }

  void BVH4Factory::selectIntersectors(int features)
  {
    /* select intersectors1 */
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Line4iIntersector1));
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Line4iMBIntersector1));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector1));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector1));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector1_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector1_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iMBIntersector1_OBB));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4XfmTriangle4Intersector1Moeller));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4Intersector1Moeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX512SKX(features,BVH4Triangle4iIntersector1Moeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX512SKX(features,BVH4Triangle4vIntersector1Pluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX512SKX(features,BVH4Triangle4iIntersector1Pluecker));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector1Moeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector1Moeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector1Pluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector1Pluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector1Moeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector1Moeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector1Pluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector1Pluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector1Pluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector1Moeller));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX512SKX(features,QBVH4Triangle4iIntersector1Pluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX512SKX(features,QBVH4Quad4iIntersector1Pluecker));

    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1Intersector1));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1EagerIntersector1));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1CachedIntersector1));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurIntersector1));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurCachedIntersector1));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4VirtualIntersector1));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4VirtualMBIntersector1));
    
#if defined (EMBREE_RAY_PACKETS)

    /* select intersectors4 */
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Line4iIntersector4));
    IF_ENABLED_LINES(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Line4iMBIntersector4));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector4Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector4Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector4Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector4Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_DEFAULT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iMBIntersector4Hybrid_OBB));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4Intersector4HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4Intersector4HybridMoellerNoFilter));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iIntersector4HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vIntersector4HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iIntersector4HybridPluecker));

    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector4HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector4HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector4HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector4HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector4HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector4HybridMoellerNoFilter));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector4HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector4HybridPluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector4HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector4HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector4HybridPluecker));

    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1Intersector4));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1EagerIntersector4));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1CachedIntersector4));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurIntersector4));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurCachedIntersector4));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4VirtualIntersector4Chunk));
    IF_ENABLED_USER(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4VirtualMBIntersector4Chunk));
    IF_ENABLED_QUADS(SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector4HybridMoeller));

    /* select intersectors8 */
    IF_ENABLED_LINES(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Line4iIntersector8));
    IF_ENABLED_LINES(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Line4iMBIntersector8));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector8Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector8Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1vIntersector8Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iIntersector8Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Bezier1iMBIntersector8Hybrid_OBB));

    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4Intersector8HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4Intersector8HybridMoellerNoFilter));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iIntersector8HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vIntersector8HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iIntersector8HybridPluecker));

    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector8HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector8HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4vMBIntersector8HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Triangle4iMBIntersector8HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector8HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector8HybridMoellerNoFilter));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector8HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4vIntersector8HybridPluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4iIntersector8HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector8HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Quad4iMBIntersector8HybridPluecker));

    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1Intersector8));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1EagerIntersector8));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1CachedIntersector8));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurIntersector8));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4Subdivpatch1MBlurCachedIntersector8));
    IF_ENABLED_USER(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4VirtualIntersector8Chunk));
    IF_ENABLED_USER(SELECT_SYMBOL_INIT_AVX_AVX2_AVX512SKX(features,BVH4VirtualMBIntersector8Chunk));

    /* select intersectors16 */
    IF_ENABLED_LINES(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Line4iIntersector16));
    IF_ENABLED_LINES(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Line4iMBIntersector16));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Bezier1vIntersector16Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Bezier1iIntersector16Hybrid));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Bezier1vIntersector16Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Bezier1iIntersector16Hybrid_OBB));
    IF_ENABLED_HAIR(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Bezier1iMBIntersector16Hybrid_OBB));

    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4Intersector16HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4Intersector16HybridMoellerNoFilter));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4iIntersector16HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4vIntersector16HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4iIntersector16HybridPluecker));

    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4vMBIntersector16HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4iMBIntersector16HybridMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4vMBIntersector16HybridPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Triangle4iMBIntersector16HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersector16HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersector16HybridMoellerNoFilter));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4iIntersector16HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersector16HybridPluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4iIntersector16HybridPluecker));

    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4iMBIntersector16HybridMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Quad4iMBIntersector16HybridPluecker));

    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1Intersector16));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1EagerIntersector16));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1CachedIntersector16));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1MBlurIntersector16));
    IF_ENABLED_SUBDIV(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1MBlurCachedIntersector16));
    IF_ENABLED_USER(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4VirtualIntersector16Chunk));
    IF_ENABLED_USER(SELECT_SYMBOL_INIT_AVX512KNL_AVX512SKX(features,BVH4VirtualMBIntersector16Chunk));

    /* select stream intersectors */
    IF_ENABLED_LINES(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX    (features,BVH4Line4iIntersectorStream));
    //IF_ENABLED_LINES(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2      (features,BVH4Line4iMBIntersectorStream));
    IF_ENABLED_HAIR(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX      (features,BVH4Bezier1vIntersectorStream));
    IF_ENABLED_HAIR(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX      (features,BVH4Bezier1iIntersectorStream));
    //IF_ENABLED_HAIR(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2      (features,BVH4Bezier1vIntersectorStream_OBB));
    //IF_ENABLED_HAIR(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2      (features,BVH4Bezier1iIntersectorStream_OBB));
    //IF_ENABLED_HAIR(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2      (features,BVH4Bezier1iMBIntersectorStream_OBB));

    IF_ENABLED_TRIS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4IntersectorStreamMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4IntersectorStreamMoellerNoFilter));
    IF_ENABLED_TRIS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4iIntersectorStreamMoeller));
    IF_ENABLED_TRIS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4vIntersectorStreamPluecker));
    IF_ENABLED_TRIS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Triangle4iIntersectorStreamPluecker));
        
    IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersectorStreamMoeller)); // FIXME: should support SSE2 in stream interface
    IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersectorStreamMoellerNoFilter));
    IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4iIntersectorStreamMoeller));
    IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4vIntersectorStreamPluecker));
    IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4iIntersectorStreamPluecker));

    //IF_ENABLED_SUBDIV(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Subdivpatch1CachedIntersectorStream));
    IF_ENABLED_USER(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4VirtualIntersectorStream));
    //IF_ENABLED_USER(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4VirtualStreamMBIntersector));
    //IF_ENABLED_QUADS(SELECT_SYMBOL_ZERO_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(features,BVH4Quad4iStreamMBIntersectorPluecker));

#endif
  }

  Accel::Intersectors BVH4Factory::BVH4Bezier1vIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Bezier1vIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Bezier1vIntersector4Hybrid();
    intersectors.intersector8  = BVH4Bezier1vIntersector8Hybrid();
    intersectors.intersector16 = BVH4Bezier1vIntersector16Hybrid();
    intersectors.intersectorN  = BVH4Bezier1vIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Bezier1iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Bezier1iIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Bezier1iIntersector4Hybrid();
    intersectors.intersector8  = BVH4Bezier1iIntersector8Hybrid();
    intersectors.intersector16 = BVH4Bezier1iIntersector16Hybrid();
    intersectors.intersectorN  = BVH4Bezier1iIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Line4iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Line4iIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Line4iIntersector4();
    intersectors.intersector8  = BVH4Line4iIntersector8();
    intersectors.intersector16 = BVH4Line4iIntersector16();
    intersectors.intersectorN  = BVH4Line4iIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Line4iMBIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Line4iMBIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Line4iMBIntersector4();
    intersectors.intersector8  = BVH4Line4iMBIntersector8();
    intersectors.intersector16 = BVH4Line4iMBIntersector16();
    //intersectors.intersectorN  = BVH4Line4iMBIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Bezier1vIntersectors_OBB(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Bezier1vIntersector1_OBB();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Bezier1vIntersector4Hybrid_OBB();
    intersectors.intersector8  = BVH4Bezier1vIntersector8Hybrid_OBB();
    intersectors.intersector16 = BVH4Bezier1vIntersector16Hybrid_OBB();
    //intersectors.intersectorN  = BVH4Bezier1vIntersectorStream_OBB();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Bezier1iIntersectors_OBB(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Bezier1iIntersector1_OBB();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Bezier1iIntersector4Hybrid_OBB();
    intersectors.intersector8  = BVH4Bezier1iIntersector8Hybrid_OBB();
    intersectors.intersector16 = BVH4Bezier1iIntersector16Hybrid_OBB();
    //intersectors.intersectorN  = BVH4Bezier1iIntersectorStream_OBB();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Bezier1iMBIntersectors_OBB(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Bezier1iMBIntersector1_OBB();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Bezier1iMBIntersector4Hybrid_OBB();
    intersectors.intersector8  = BVH4Bezier1iMBIntersector8Hybrid_OBB();
    intersectors.intersector16 = BVH4Bezier1iMBIntersector16Hybrid_OBB();
    //intersectors.intersectorN  = BVH4Bezier1iMBIntersectorStream_OBB();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4IntersectorsInstancing(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4XfmTriangle4Intersector1Moeller();
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4Intersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    assert(ivariant == IntersectVariant::FAST);
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1           = BVH4Triangle4Intersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4_filter    = BVH4Triangle4Intersector4HybridMoeller();
    intersectors.intersector4_nofilter  = BVH4Triangle4Intersector4HybridMoellerNoFilter();
    intersectors.intersector8_filter    = BVH4Triangle4Intersector8HybridMoeller();
    intersectors.intersector8_nofilter  = BVH4Triangle4Intersector8HybridMoellerNoFilter();
    intersectors.intersector16_filter   = BVH4Triangle4Intersector16HybridMoeller();
    intersectors.intersector16_nofilter = BVH4Triangle4Intersector16HybridMoellerNoFilter();
    intersectors.intersectorN_filter    = BVH4Triangle4IntersectorStreamMoeller();
    intersectors.intersectorN_nofilter  = BVH4Triangle4IntersectorStreamMoellerNoFilter();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4vIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    assert(ivariant == IntersectVariant::ROBUST);
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Triangle4vIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Triangle4vIntersector4HybridPluecker();
    intersectors.intersector8  = BVH4Triangle4vIntersector8HybridPluecker();
    intersectors.intersector16 = BVH4Triangle4vIntersector16HybridPluecker();
    intersectors.intersectorN  = BVH4Triangle4vIntersectorStreamPluecker();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4iIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4iIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4iIntersector4HybridMoeller();
      intersectors.intersector8  = BVH4Triangle4iIntersector8HybridMoeller();
      intersectors.intersector16 = BVH4Triangle4iIntersector16HybridMoeller();
      intersectors.intersectorN  = BVH4Triangle4iIntersectorStreamMoeller();
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4iIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4iIntersector4HybridPluecker();
      intersectors.intersector8  = BVH4Triangle4iIntersector8HybridPluecker();
      intersectors.intersector16 = BVH4Triangle4iIntersector16HybridPluecker();
      intersectors.intersectorN  = BVH4Triangle4iIntersectorStreamPluecker();
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4vMBIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4vMBIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4vMBIntersector4HybridMoeller();
      intersectors.intersector8  = BVH4Triangle4vMBIntersector8HybridMoeller();
      intersectors.intersector16 = BVH4Triangle4vMBIntersector16HybridMoeller();
      //intersectors.intersectorN  = BVH4Triangle4vMBIntersectorStreamMoeller();
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4vMBIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4vMBIntersector4HybridPluecker();
      intersectors.intersector8  = BVH4Triangle4vMBIntersector8HybridPluecker();
      intersectors.intersector16 = BVH4Triangle4vMBIntersector16HybridPluecker();
      //intersectors.intersectorN  = BVH4Triangle4vMBIntersectorStreamPluecker();
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::BVH4Triangle4iMBIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4iMBIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4iMBIntersector4HybridMoeller();
      intersectors.intersector8  = BVH4Triangle4iMBIntersector8HybridMoeller();
      intersectors.intersector16 = BVH4Triangle4iMBIntersector16HybridMoeller();
      //intersectors.intersectorN  = BVH4Triangle4iMBIntersectorStreamMoeller();
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Triangle4iMBIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Triangle4iMBIntersector4HybridPluecker();
      intersectors.intersector8  = BVH4Triangle4iMBIntersector8HybridPluecker();
      intersectors.intersector16 = BVH4Triangle4iMBIntersector16HybridPluecker();
      //intersectors.intersectorN  = BVH4Triangle4iMBIntersectorStreamPluecker();
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::BVH4Quad4vIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1           = BVH4Quad4vIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4_filter    = BVH4Quad4vIntersector4HybridMoeller();
      intersectors.intersector4_nofilter  = BVH4Quad4vIntersector4HybridMoellerNoFilter();
      intersectors.intersector8_filter    = BVH4Quad4vIntersector8HybridMoeller();
      intersectors.intersector8_nofilter  = BVH4Quad4vIntersector8HybridMoellerNoFilter();
      intersectors.intersector16_filter   = BVH4Quad4vIntersector16HybridMoeller();
      intersectors.intersector16_nofilter = BVH4Quad4vIntersector16HybridMoellerNoFilter();
      intersectors.intersectorN_filter    = BVH4Quad4vIntersectorStreamMoeller();
      intersectors.intersectorN_nofilter  = BVH4Quad4vIntersectorStreamMoellerNoFilter();
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1  = BVH4Quad4vIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4  = BVH4Quad4vIntersector4HybridPluecker();
      intersectors.intersector8  = BVH4Quad4vIntersector8HybridPluecker();
      intersectors.intersector16 = BVH4Quad4vIntersector16HybridPluecker();
      intersectors.intersectorN  = BVH4Quad4vIntersectorStreamPluecker();
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::BVH4Quad4iIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1 = BVH4Quad4iIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4 = BVH4Quad4iIntersector4HybridMoeller();
      intersectors.intersector8 = BVH4Quad4iIntersector8HybridMoeller();
      intersectors.intersector16= BVH4Quad4iIntersector16HybridMoeller();
      intersectors.intersectorN = BVH4Quad4iIntersectorStreamMoeller(); 
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1 = BVH4Quad4iIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4 = BVH4Quad4iIntersector4HybridPluecker();
      intersectors.intersector8 = BVH4Quad4iIntersector8HybridPluecker();
      intersectors.intersector16= BVH4Quad4iIntersector16HybridPluecker();
      intersectors.intersectorN = BVH4Quad4iIntersectorStreamPluecker(); 
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::BVH4Quad4iMBIntersectors(BVH4* bvh, IntersectVariant ivariant)
  {
    switch (ivariant) {
    case IntersectVariant::FAST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1 = BVH4Quad4iMBIntersector1Moeller();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4 = BVH4Quad4iMBIntersector4HybridMoeller();
      intersectors.intersector8 = BVH4Quad4iMBIntersector8HybridMoeller();
      intersectors.intersector16= BVH4Quad4iMBIntersector16HybridMoeller();
      //intersectors.intersectorN = BVH4Quad4iMBIntersectorStreamMoeller();
#endif
      return intersectors;
    }
    case IntersectVariant::ROBUST: 
    {
      Accel::Intersectors intersectors;
      intersectors.ptr = bvh;
      intersectors.intersector1 = BVH4Quad4iMBIntersector1Pluecker();
#if defined (EMBREE_RAY_PACKETS)
      intersectors.intersector4 = BVH4Quad4iMBIntersector4HybridPluecker();
      intersectors.intersector8 = BVH4Quad4iMBIntersector8HybridPluecker();
      intersectors.intersector16= BVH4Quad4iMBIntersector16HybridPluecker();
      //intersectors.intersectorN = BVH4Quad4iMBIntersectorStreamPluecker();
#endif
      return intersectors;
    }
    }
    return Accel::Intersectors();
  }

  Accel::Intersectors BVH4Factory::QBVH4Triangle4iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = QBVH4Triangle4iIntersector1Pluecker();
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::QBVH4Quad4iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = QBVH4Quad4iIntersector1Pluecker();
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4UserGeometryIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4VirtualIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4VirtualIntersector4Chunk();
    intersectors.intersector8  = BVH4VirtualIntersector8Chunk();
    intersectors.intersector16 = BVH4VirtualIntersector16Chunk();
    intersectors.intersectorN  = BVH4VirtualIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4UserGeometryMBIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4VirtualMBIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4VirtualMBIntersector4Chunk();
    intersectors.intersector8  = BVH4VirtualMBIntersector8Chunk();
    intersectors.intersector16 = BVH4VirtualMBIntersector16Chunk();
    //intersectors.intersectorN  = BVH4VirtualMBIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4SubdivPatch1Intersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Subdivpatch1Intersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Subdivpatch1Intersector4();
    intersectors.intersector8  = BVH4Subdivpatch1Intersector8();
    intersectors.intersector16 = BVH4Subdivpatch1Intersector16();
    //intersectors.intersectorN  = BVH4Subdivpatch1IntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4SubdivPatch1EagerIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Subdivpatch1EagerIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Subdivpatch1EagerIntersector4();
    intersectors.intersector8  = BVH4Subdivpatch1EagerIntersector8();
    intersectors.intersector16 = BVH4Subdivpatch1EagerIntersector16();
    //intersectors.intersectorN  = BVH4Subdivpatch1EagerIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4SubdivPatch1CachedIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Subdivpatch1CachedIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Subdivpatch1CachedIntersector4();
    intersectors.intersector8  = BVH4Subdivpatch1CachedIntersector8();
    intersectors.intersector16 = BVH4Subdivpatch1CachedIntersector16();
    //intersectors.intersectorN  = BVH4Subdivpatch1CachedIntersectorStream();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4SubdivPatch1MBlurIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Subdivpatch1MBlurIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Subdivpatch1MBlurIntersector4();
    intersectors.intersector8  = BVH4Subdivpatch1MBlurIntersector8();
    intersectors.intersector16 = BVH4Subdivpatch1MBlurIntersector16();
    //intersectors.intersectorN  = BVH4Subdivpatch1MBlurStreamIntersector();
#endif
    return intersectors;
  }

  Accel::Intersectors BVH4Factory::BVH4SubdivPatch1MBlurCachedIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1  = BVH4Subdivpatch1MBlurCachedIntersector1();
#if defined (EMBREE_RAY_PACKETS)
    intersectors.intersector4  = BVH4Subdivpatch1MBlurCachedIntersector4();
    intersectors.intersector8  = BVH4Subdivpatch1MBlurCachedIntersector8();
    intersectors.intersector16 = BVH4Subdivpatch1MBlurCachedIntersector16();
    //intersectors.intersectorN  = BVH4Subdivpatch1MBlurCachedStreamIntersector();
#endif
    return intersectors;
  }

  void BVH4Factory::createLineSegmentsLine4i(LineSegments* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Line4i::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4Line4iMeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4Line4iMeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4Line4iMeshBuilderSAH(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  void BVH4Factory::createTriangleMeshTriangle4Morton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4::type,mesh->parent);
    builder = factory->BVH4Triangle4MeshBuilderMortonGeneral(accel,mesh,0);
  }

  void BVH4Factory::createTriangleMeshTriangle4vMorton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4v::type,mesh->parent);
    builder = factory->BVH4Triangle4vMeshBuilderMortonGeneral(accel,mesh,0);
  }

  void BVH4Factory::createTriangleMeshTriangle4iMorton(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4i::type,mesh->parent);
    builder = factory->BVH4Triangle4iMeshBuilderMortonGeneral(accel,mesh,0); 
  }

  void BVH4Factory::createQuadMeshQuad4vMorton(QuadMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Quad4v::type,mesh->parent);
    builder = factory->BVH4Quad4vMeshBuilderMortonGeneral(accel,mesh,0);
  }

  void BVH4Factory::createTriangleMeshTriangle4(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4Triangle4MeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4Triangle4MeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4Triangle4MeshBuilderMortonGeneral(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  void BVH4Factory::createTriangleMeshTriangle4v(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4v::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4Triangle4vMeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4Triangle4vMeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4Triangle4vMeshBuilderMortonGeneral(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  void BVH4Factory::createTriangleMeshTriangle4i(TriangleMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Triangle4i::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4Triangle4iMeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4Triangle4iMeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4Triangle4iMeshBuilderMortonGeneral(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  void BVH4Factory::createQuadMeshQuad4v(QuadMesh* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Quad4v::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4Quad4vMeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4Quad4vMeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4Quad4vMeshBuilderMortonGeneral(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  void BVH4Factory::createAccelSetMesh(AccelSet* mesh, AccelData*& accel, Builder*& builder)
  {
    BVH4Factory* factory = mesh->parent->device->bvh4_factory.get();
    accel = new BVH4(Object::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = factory->BVH4VirtualMeshBuilderSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = factory->BVH4VirtualMeshRefitSAH(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = factory->BVH4VirtualMeshBuilderMortonGeneral(accel,mesh,0); break;
    default: throw_RTCError(RTC_UNKNOWN_ERROR,"invalid geometry flag");
    }
  }

  Accel* BVH4Factory::BVH4Bezier1v(Scene* scene)
  {
    BVH4* accel = new BVH4(Bezier1v::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1vIntersectors(accel);

    Builder* builder = nullptr;
    if      (scene->device->hair_builder == "default"     ) builder = BVH4Bezier1vSceneBuilderSAH(accel,scene,0);
    else if (scene->device->hair_builder == "sah"         ) builder = BVH4Bezier1vSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->hair_builder+" for BVH4<Bezier1v>");

    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Bezier1i(Scene* scene)
  {
    BVH4* accel = new BVH4(Bezier1i::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1iIntersectors(accel);

    Builder* builder = nullptr;
    if      (scene->device->hair_builder == "default"     ) builder = BVH4Bezier1iSceneBuilderSAH(accel,scene,0);
    else if (scene->device->hair_builder == "sah"         ) builder = BVH4Bezier1iSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->hair_builder+" for BVH4<Bezier1i>");

    scene->needBezierVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Line4i(Scene* scene, BuildVariant bvariant)
  {
    BVH4* accel = new BVH4(Line4i::type,scene);
    Accel::Intersectors intersectors = BVH4Line4iIntersectors(accel);

    Builder* builder = nullptr;
    if (scene->device->line_builder == "default"     ) {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Line4iSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelLineSegmentsSAH(accel,scene,&createLineSegmentsLine4i); break;
      case BuildVariant::HIGH_QUALITY: assert(false); break;
      }
    }
    else if (scene->device->line_builder == "sah"         ) builder = BVH4Line4iSceneBuilderSAH(accel,scene,0);
    else if (scene->device->line_builder == "dynamic"     ) builder = BVH4BuilderTwoLevelLineSegmentsSAH(accel,scene,&createLineSegmentsLine4i);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->line_builder+" for BVH4<Line4i>");

    scene->needLineVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Line4iMB(Scene* scene)
  {
    BVH4* accel = new BVH4(Line4i::type,scene);
    Accel::Intersectors intersectors = BVH4Line4iMBIntersectors(accel);

    Builder* builder = nullptr;
    if      (scene->device->line_builder_mb == "default"     ) builder = BVH4Line4iMBSceneBuilderSAH(accel,scene,0);
    else if (scene->device->line_builder_mb == "sah"         ) builder = BVH4Line4iMBSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->line_builder_mb+" for BVH4MB<Line4iMB>");

    scene->needLineVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4OBBBezier1v(Scene* scene)
  {
    BVH4* accel = new BVH4(Bezier1v::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1vIntersectors_OBB(accel);

    Builder* builder = nullptr;
    if      (scene->device->hair_builder == "default"     ) builder = BVH4Bezier1vBuilder_OBB_New(accel,scene,0);
    else if (scene->device->hair_builder == "sah"         ) builder = BVH4Bezier1vBuilder_OBB_New(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->hair_builder+" for BVH4OBB<Bezier1v>");
    
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4OBBBezier1i(Scene* scene)
  {
    BVH4* accel = new BVH4(Bezier1i::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1iIntersectors_OBB(accel);

    Builder* builder = nullptr;
    if      (scene->device->hair_builder == "default"     ) builder = BVH4Bezier1iBuilder_OBB_New(accel,scene,0);
    else if (scene->device->hair_builder == "sah"         ) builder = BVH4Bezier1iBuilder_OBB_New(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->hair_builder+" for BVH4OBB<Bezier1i>");
    
    scene->needBezierVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4OBBBezier1iMB(Scene* scene)
  {
    BVH4* accel = new BVH4(Bezier1i::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1iMBIntersectors_OBB(accel);

    Builder* builder = nullptr;
    if      (scene->device->hair_builder_mb == "default"     ) builder = BVH4Bezier1iMBBuilder_OBB_New(accel,scene,0);
    else if (scene->device->hair_builder_mb == "sah"         ) builder = BVH4Bezier1iMBBuilder_OBB_New(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->hair_builder_mb+" for BVH4MBOBB<Bezier1iMB>");   

    scene->needBezierVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4InstancedBVH4Triangle4ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(Triangle4::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsInstancing(accel);
    Builder* builder = BVH4BuilderInstancingTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Triangle4(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Triangle4::type,scene);

    Accel::Intersectors intersectors;
    if      (scene->device->tri_traverser == "default") intersectors = BVH4Triangle4Intersectors(accel,ivariant);
    else if (scene->device->tri_traverser == "fast"   ) intersectors = BVH4Triangle4Intersectors(accel,IntersectVariant::FAST);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown traverser "+scene->device->tri_traverser+" for BVH4<Triangle4>");

    Builder* builder = nullptr;
    if (scene->device->tri_builder == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Triangle4SceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4); break;
      case BuildVariant::HIGH_QUALITY: builder = BVH4Triangle4SceneBuilderFastSpatialSAH(accel,scene,0); break;
      }
    }
    else if (scene->device->tri_builder == "sah"         ) builder = BVH4Triangle4SceneBuilderSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_fast_spatial" ) builder = BVH4Triangle4SceneBuilderFastSpatialSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_presplit") builder = BVH4Triangle4SceneBuilderSAH(accel,scene,MODE_HIGH_QUALITY);
    else if (scene->device->tri_builder == "dynamic"     ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4);
    else if (scene->device->tri_builder == "morton"      ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4Morton);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->tri_builder+" for BVH4<Triangle4>");

    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Triangle4v(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Triangle4v::type,scene);

    Accel::Intersectors intersectors;
    if      (scene->device->tri_traverser == "default") intersectors = BVH4Triangle4vIntersectors(accel,ivariant);
    else if (scene->device->tri_traverser == "fast"   ) intersectors = BVH4Triangle4vIntersectors(accel,IntersectVariant::FAST);
    else if (scene->device->tri_traverser == "robust" ) intersectors = BVH4Triangle4vIntersectors(accel,IntersectVariant::ROBUST);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown traverser "+scene->device->tri_traverser+" for BVH4<Triangle4>");

    Builder* builder = nullptr;
    if (scene->device->tri_builder == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Triangle4vSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4v); break;
      case BuildVariant::HIGH_QUALITY: builder = BVH4Triangle4vSceneBuilderFastSpatialSAH(accel,scene,0); break;
      }
    }
    else if (scene->device->tri_builder == "sah"         ) builder = BVH4Triangle4vSceneBuilderSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_fast_spatial" ) builder = BVH4Triangle4vSceneBuilderFastSpatialSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_presplit") builder = BVH4Triangle4vSceneBuilderSAH(accel,scene,MODE_HIGH_QUALITY);
    else if (scene->device->tri_builder == "dynamic"     ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4v);
    else if (scene->device->tri_builder == "morton"      ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4vMorton);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->tri_builder+" for BVH4<Triangle4v>");

    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Triangle4i(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Triangle4i::type,scene);
    
    Accel::Intersectors intersectors;
    if      (scene->device->tri_traverser == "default") intersectors = BVH4Triangle4iIntersectors(accel,ivariant);
    else if (scene->device->tri_traverser == "fast"   ) intersectors = BVH4Triangle4iIntersectors(accel,IntersectVariant::FAST);
    else if (scene->device->tri_traverser == "robust" ) intersectors = BVH4Triangle4iIntersectors(accel,IntersectVariant::ROBUST);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown traverser "+scene->device->tri_traverser+" for BVH4<Triangle4i>");

    Builder* builder = nullptr;
    if (scene->device->tri_builder == "default"     ) {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Triangle4iSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4i); break;
      case BuildVariant::HIGH_QUALITY: builder = BVH4Triangle4iSceneBuilderFastSpatialSAH(accel,scene,0); break;
      }
    }
    else if (scene->device->tri_builder == "sah"         ) builder = BVH4Triangle4iSceneBuilderSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_fast_spatial" ) builder = BVH4Triangle4iSceneBuilderFastSpatialSAH(accel,scene,0);
    else if (scene->device->tri_builder == "sah_presplit") builder = BVH4Triangle4iSceneBuilderSAH(accel,scene,MODE_HIGH_QUALITY);
    else if (scene->device->tri_builder == "dynamic"     ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4i);
    else if (scene->device->tri_builder == "morton"      ) builder = BVH4BuilderTwoLevelTriangleMeshSAH(accel,scene,&createTriangleMeshTriangle4iMorton);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->tri_builder+" for BVH4<Triangle4i>");

    scene->needTriangleVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

   Accel* BVH4Factory::BVH4Triangle4vMB(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Triangle4vMB::type,scene);

    Accel::Intersectors intersectors;
    if      (scene->device->tri_traverser_mb == "default") intersectors = BVH4Triangle4vMBIntersectors(accel,ivariant);
    else if (scene->device->tri_traverser_mb == "fast"   ) intersectors = BVH4Triangle4vMBIntersectors(accel,IntersectVariant::FAST);
    else if (scene->device->tri_traverser_mb == "robust" ) intersectors = BVH4Triangle4vMBIntersectors(accel,IntersectVariant::ROBUST);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown traverser "+scene->device->tri_traverser_mb+" for BVH4MB<Triangle4vMB>");

    Builder* builder = nullptr;
    if (scene->device->tri_builder_mb == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Triangle4vMBSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : assert(false); break; // FIXME: implement
      case BuildVariant::HIGH_QUALITY: assert(false); break;
      }
    }
    else  if (scene->device->tri_builder_mb == "sah") builder = BVH4Triangle4vMBSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->tri_builder_mb+" for BVH4<Triangle4vMB>");
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Triangle4iMB(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Triangle4iMB::type,scene);

    Accel::Intersectors intersectors;
    if      (scene->device->tri_traverser_mb == "default") intersectors = BVH4Triangle4iMBIntersectors(accel,ivariant);
    else if (scene->device->tri_traverser_mb == "fast"   ) intersectors = BVH4Triangle4iMBIntersectors(accel,IntersectVariant::FAST);
    else if (scene->device->tri_traverser_mb == "robust" ) intersectors = BVH4Triangle4iMBIntersectors(accel,IntersectVariant::ROBUST);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown traverser "+scene->device->tri_traverser_mb+" for BVH4MB<Triangle4iMB>");

    Builder* builder = nullptr;
    if (scene->device->tri_builder_mb == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Triangle4iMBSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : assert(false); break; // FIXME: implement
      case BuildVariant::HIGH_QUALITY: assert(false); break;
      }
    }
    else  if (scene->device->tri_builder_mb == "sah") builder = BVH4Triangle4iMBSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->tri_builder_mb+" for BVH4MB<Triangle4iMB>");

    scene->needTriangleVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }
  
  Accel* BVH4Factory::BVH4Quad4v(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Quad4v::type,scene);
    Accel::Intersectors intersectors = BVH4Quad4vIntersectors(accel,ivariant);
   
    Builder* builder = nullptr;
    if (scene->device->quad_builder == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Quad4vSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelQuadMeshSAH(accel,scene,&createQuadMeshQuad4v); break;
      case BuildVariant::HIGH_QUALITY: builder = BVH4Quad4vSceneBuilderFastSpatialSAH(accel,scene,0); break;
      }
    }
    else if (scene->device->quad_builder == "sah"              ) builder = BVH4Quad4vSceneBuilderSAH(accel,scene,0);
    else if (scene->device->quad_builder == "sah_fast_spatial" ) builder = BVH4Quad4vSceneBuilderFastSpatialSAH(accel,scene,0);
    else if (scene->device->quad_builder == "dynamic"          ) builder = BVH4BuilderTwoLevelQuadMeshSAH(accel,scene,&createQuadMeshQuad4v);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->quad_builder+" for BVH4<Quad4v>");
        
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Quad4i(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Quad4i::type,scene);
    Accel::Intersectors intersectors = BVH4Quad4iIntersectors(accel,ivariant);

    Builder* builder = nullptr;
    if (scene->device->quad_builder == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Quad4iSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : assert(false); break; // FIXME: implement
      case BuildVariant::HIGH_QUALITY: assert(false); break; // FIXME: implement
      }
    }
    else if (scene->device->quad_builder == "sah") builder = BVH4Quad4iSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->quad_builder+" for BVH4<Quad4i>");
    
    scene->needQuadVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4Quad4iMB(Scene* scene, BuildVariant bvariant, IntersectVariant ivariant)
  {
    BVH4* accel = new BVH4(Quad4iMB::type,scene);
    Accel::Intersectors intersectors = BVH4Quad4iMBIntersectors(accel,ivariant);

    Builder* builder = nullptr;
    if (scene->device->quad_builder_mb == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4Quad4iMBSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : assert(false); break; // FIXME: implement
      case BuildVariant::HIGH_QUALITY: assert(false); break;
      }
    }
    else if (scene->device->quad_builder_mb == "sah") builder = BVH4Quad4iMBSceneBuilderSAH(accel,scene,0);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->quad_builder_mb+" for BVH4MB<Quad4iMB>");
    
    scene->needQuadVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4QuantizedQuad4i(Scene* scene)
  {
    BVH4* accel = new BVH4(Quad4i::type,scene);
    Builder* builder = BVH4QuantizedQuad4iSceneBuilderSAH(accel,scene,0);
    Accel::Intersectors intersectors = QBVH4Quad4iIntersectors(accel);
    scene->needQuadVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4QuantizedTriangle4i(Scene* scene)
  {
    BVH4* accel = new BVH4(Triangle4i::type,scene);
    Builder* builder = BVH4QuantizedTriangle4iSceneBuilderSAH(accel,scene,0);
    Accel::Intersectors intersectors = QBVH4Triangle4iIntersectors(accel);
    scene->needTriangleVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4SubdivPatch1(Scene* scene, bool cached)
  {
    if (cached)
    {
      BVH4* accel = new BVH4(SubdivPatch1Cached::type_cached,scene);
      Accel::Intersectors intersectors = BVH4SubdivPatch1CachedIntersectors(accel);
      Builder* builder = BVH4SubdivPatch1CachedBuilderBinnedSAH(accel,scene,1);
      scene->needSubdivVertices = true;
      return new AccelInstance(accel,builder,intersectors);
    }
    else 
    {
      BVH4* accel = new BVH4(SubdivPatch1Cached::type,scene);
      Accel::Intersectors intersectors = BVH4SubdivPatch1Intersectors(accel);
      Builder* builder = BVH4SubdivPatch1CachedBuilderBinnedSAH(accel,scene,0);
      scene->needSubdivVertices = true;
      return new AccelInstance(accel,builder,intersectors);
    }
  }

  Accel* BVH4Factory::BVH4SubdivPatch1Eager(Scene* scene)
  {
    BVH4* accel = new BVH4(SubdivPatch1Cached::type,scene); // FIXME: use different type
    Accel::Intersectors intersectors = BVH4SubdivPatch1EagerIntersectors(accel);
    Builder* builder = BVH4SubdivPatch1EagerBuilderBinnedSAH(accel,scene,0);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4SubdivPatch1MBlur(Scene* scene, bool cached)
  {
    if (cached)
    {
      BVH4* accel = new BVH4(SubdivPatch1Cached::type_cached,scene);
      Accel::Intersectors intersectors = BVH4SubdivPatch1MBlurCachedIntersectors(accel);
      Builder* builder = BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH(accel,scene,1);
      scene->needSubdivVertices = true;
      return new AccelInstance(accel,builder,intersectors);
    }
    else
    {
      BVH4* accel = new BVH4(SubdivPatch1Cached::type,scene);
      Accel::Intersectors intersectors = BVH4SubdivPatch1MBlurIntersectors(accel);
      Builder* builder = BVH4SubdivPatch1MBlurCachedBuilderBinnedSAH(accel,scene,0);
      scene->needSubdivVertices = true;
      return new AccelInstance(accel,builder,intersectors);
    }
  }

  Accel* BVH4Factory::BVH4UserGeometry(Scene* scene, BuildVariant bvariant)
  {
    BVH4* accel = new BVH4(Object::type,scene);
    Accel::Intersectors intersectors = BVH4UserGeometryIntersectors(accel);

    Builder* builder = nullptr;
    if (scene->device->object_builder == "default") {
      switch (bvariant) {
      case BuildVariant::STATIC      : builder = BVH4VirtualSceneBuilderSAH(accel,scene,0); break;
      case BuildVariant::DYNAMIC     : builder = BVH4BuilderTwoLevelVirtualSAH(accel,scene,&createAccelSetMesh); break;
      case BuildVariant::HIGH_QUALITY: assert(false); break;
      }
    } 
    else if (scene->device->object_builder == "sah") builder = BVH4VirtualSceneBuilderSAH(accel,scene,0);
    else if (scene->device->object_builder == "dynamic") builder = BVH4BuilderTwoLevelVirtualSAH(accel,scene,&createAccelSetMesh);
    else throw_RTCError(RTC_INVALID_ARGUMENT,"unknown builder "+scene->device->object_builder+" for BVH4<Object>");
    
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4Factory::BVH4UserGeometryMB(Scene* scene)
  {
    BVH4* accel = new BVH4(Object::type,scene);
    Accel::Intersectors intersectors = BVH4UserGeometryMBIntersectors(accel);
    Builder* builder = BVH4VirtualMBSceneBuilderSAH(accel,scene,0);
    return new AccelInstance(accel,builder,intersectors);
  }
}


