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

#include "bvh_builder_twolevel.h"
#include "bvh_statistics.h"
#include "../builders/bvh_builder_sah.h"
#include "../common/scene_line_segments.h"
#include "../common/scene_triangle_mesh.h"
#include "../common/scene_quad_mesh.h"

#define PROFILE 0
#define MAX_OPEN_SIZE 10000
#define PROFILE_ITERATIONS 200

namespace embree
{
  namespace isa
  {
    template<int N, typename Mesh>
    BVHNBuilderTwoLevel<N,Mesh>::BVHNBuilderTwoLevel (BVH* bvh, Scene* scene, const createMeshAccelTy createMeshAccel, const size_t singleThreadThreshold)
      : bvh(bvh), objects(bvh->objects), scene(scene), createMeshAccel(createMeshAccel), refs(scene->device), prims(scene->device), singleThreadThreshold(singleThreadThreshold) {}
    
    template<int N, typename Mesh>
    BVHNBuilderTwoLevel<N,Mesh>::~BVHNBuilderTwoLevel ()
    {
      for (size_t i=0; i<builders.size(); i++) 
	delete builders[i];
    }

    template<int N, typename Mesh>
    void BVHNBuilderTwoLevel<N,Mesh>::build()
    {
      /* delete some objects */
      size_t num = scene->size();
      if (num < objects.size()) {
        parallel_for(num, objects.size(), [&] (const range<size_t>& r) {
            for (size_t i=r.begin(); i<r.end(); i++) {
              delete builders[i]; builders[i] = nullptr;
              delete objects[i]; objects[i] = nullptr;
            }
          });
      }

      /* reset memory allocator */
      bvh->alloc.reset();
      
      /* skip build for empty scene */
      const size_t numPrimitives = scene->getNumPrimitives<Mesh,false>();
      if (numPrimitives == 0) {
        prims.resize(0);
        bvh->set(BVH::emptyNode,empty,0);
        return;
      }

      double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "BuilderTwoLevel");

#if PROFILE
	profile(2,PROFILE_ITERATIONS,numPrimitives,[&] (ProfileTimer& timer)
        {
#endif

      /* resize object array if scene got larger */
      if (objects.size()  < num) objects.resize(num);
      if (builders.size() < num) builders.resize(num);
      if (refs.size()     < num) refs.resize(num);
      nextRef.store(0);


      /* create of acceleration structures */
      parallel_for(size_t(0), num, [&] (const range<size_t>& r)
      {
        for (size_t objectID=r.begin(); objectID<r.end(); objectID++)
        {
          Mesh* mesh = scene->getSafe<Mesh>(objectID);
          
          /* verify meshes got deleted properly */
          if (mesh == nullptr || mesh->numTimeSteps != 1) {
            assert(objectID < objects.size () && objects[objectID] == nullptr);
            assert(objectID < builders.size() && builders[objectID] == nullptr);
            continue;
          }
          
          /* create BVH and builder for new meshes */
          if (objects[objectID] == nullptr)
            createMeshAccel(mesh,(AccelData*&)objects[objectID],builders[objectID]);
        }
      });
      /* parallel build of acceleration structures */
      parallel_for(size_t(0), num, [&] (const range<size_t>& r)
      {
        for (size_t objectID=r.begin(); objectID<r.end(); objectID++)
        {
          /* ignore if no triangle mesh or not enabled */
          Mesh* mesh = scene->getSafe<Mesh>(objectID);
          if (mesh == nullptr || !mesh->isEnabled() || mesh->numTimeSteps != 1) 
            continue;
        
          BVH*     object  = objects [objectID]; assert(object);
          Builder* builder = builders[objectID]; assert(builder);
          
          /* build object if it got modified */
#if !PROFILE 
          if (mesh->isModified()) 
#endif
            builder->build();
          
          /* create build primitive */
          if (!object->getBounds().empty())
            refs[nextRef++] = BVHNBuilderTwoLevel::BuildRef(object->getBounds(),object->root);
        }
      });

      /* fast path for single geometry scenes */
      if (nextRef == 1) { 
        bvh->set(refs[0].node,LBBox3fa(refs[0].bounds()),numPrimitives);
      }

      else
      {
        /* open all large nodes */
        refs.resize(nextRef);
        open_sequential(numPrimitives); 
        //open_overlap(numPrimitives); 
        // PRINT(numPrimitives);
        // PRINT(refs.size());
        /* compute PrimRefs */
        prims.resize(refs.size());
        bvh->alloc.init_estimate(refs.size()*16);

#if defined(TASKING_TBB) && defined(__AVX512ER__) && USE_TASK_ARENA // KNL
        tbb::task_arena limited(min(32,(int)TaskScheduler::threadCount()));
        limited.execute([&]
#endif
        {
          const PrimInfo pinfo = parallel_reduce(size_t(0), refs.size(),  PrimInfo(empty), [&] (const range<size_t>& r) -> PrimInfo {

              PrimInfo pinfo(empty);
              for (size_t i=r.begin(); i<r.end(); i++) {
                pinfo.add(refs[i].bounds());
                prims[i] = PrimRef(refs[i].bounds(),(size_t)refs[i].node);
              }
              return pinfo;
            }, [] (const PrimInfo& a, const PrimInfo& b) { return PrimInfo::merge(a,b); });

          /* skip if all objects where empty */
          if (pinfo.size() == 0)
            bvh->set(BVH::emptyNode,empty,0);
        
          /* otherwise build toplevel hierarchy */
          else
          {
              /* settings for BVH build */
            GeneralBVHBuilder::Settings settings;
            settings.branchingFactor = N;
            settings.maxDepth = BVH::maxBuildDepthLeaf;
            settings.logBlockSize = __bsr(N);
            settings.minLeafSize = 1;
            settings.maxLeafSize = 1;
            settings.travCost = 1.0f;
            settings.intCost = 1.0f;
            settings.singleThreadThreshold = singleThreadThreshold;

            NodeRef root = BVHBuilderBinnedSAH::build<NodeRef>
              (
                typename BVH::CreateAlloc(bvh),
                typename BVH::AlignedNode::Create2(),
                typename BVH::AlignedNode::Set2(),
               
               [&] (const BVHBuilderBinnedSAH::BuildRecord& current, FastAllocator::ThreadLocal2* alloc) -> NodeRef
              {
                assert(current.prims.size() == 1);
                return (NodeRef) prims[current.prims.begin()].ID();
              },
               [&] (size_t dn) { bvh->scene->progressMonitor(0); },
                prims.data(),pinfo,settings);

            bvh->set(root,LBBox3fa(pinfo.geomBounds),numPrimitives);
          }
        }
#if defined(TASKING_TBB) && defined(__AVX512ER__) && USE_TASK_ARENA // KNL
          );
#endif

      }  
        
#if PROFILE
      }); 
#endif
      bvh->alloc.cleanup();
      bvh->postBuild(t0);
    }
    
    template<int N, typename Mesh>
    void BVHNBuilderTwoLevel<N,Mesh>::deleteGeometry(size_t geomID)
    {
      if (geomID >= objects.size()) return;
      delete builders[geomID]; builders[geomID] = nullptr;
      delete objects [geomID]; objects [geomID] = nullptr;
    }

    template<int N, typename Mesh>
    void BVHNBuilderTwoLevel<N,Mesh>::clear()
    {
      for (size_t i=0; i<objects.size(); i++) 
        if (objects[i]) objects[i]->clear();

      for (size_t i=0; i<builders.size(); i++) 
	if (builders[i]) builders[i]->clear();

      refs.clear();
    }

    template<int N, typename Mesh>
    void BVHNBuilderTwoLevel<N,Mesh>::open_sequential(size_t numPrimitives)
    {
      if (refs.size() == 0)
	return;

      size_t num = min(numPrimitives/400,size_t(MAX_OPEN_SIZE));
      refs.reserve(num);

#if 1
      for (size_t i=0;i<refs.size();i++)
      {
        NodeRef ref = refs.back().node;
        if (ref.isAlignedNode())
          ref.prefetch();
      }
#endif

      std::make_heap(refs.begin(),refs.end());
      while (refs.size()+3 <= num)
      {
        std::pop_heap (refs.begin(),refs.end()); 
        NodeRef ref = refs.back().node;
        if (ref.isLeaf()) break;
        refs.pop_back();    
        
        AlignedNode* node = ref.alignedNode();
        for (size_t i=0; i<N; i++) {
          if (node->child(i) == BVH::emptyNode) continue;
          refs.push_back(BuildRef(node->bounds(i),node->child(i)));
         
#if 1
          NodeRef ref_pre = node->child(i);
          if (ref_pre.isAlignedNode())
            ref_pre.prefetch();
#endif
          std::push_heap (refs.begin(),refs.end()); 
        }
      }
    }

    template<int N, typename Mesh>
    void BVHNBuilderTwoLevel<N,Mesh>::open_overlap(size_t numPrimitives)
    {
      if (refs.size() == 0)
	return;

      size_t num = min(numPrimitives/400,size_t(MAX_OPEN_SIZE));
      refs.reserve(num);

#if 1
      for (size_t i=0;i<refs.size();i++)
      {
        NodeRef ref = refs.back().node;
        if (ref.isAlignedNode())
          ref.prefetch();
      }
#endif

      for (size_t i=0;i<refs.size();i++)
        for (size_t j=i+1;j<refs.size();j++)
        {
          //std::cout << "i " << i << " j " << j << " -> " << disjoint(refs[i].bounds(),refs[j].bounds()) << std::endl;
        }


      std::make_heap(refs.begin(),refs.end());
      while (refs.size()+3 <= num)
      {
        std::pop_heap (refs.begin(),refs.end()); 
        NodeRef ref = refs.back().node;
        if (ref.isLeaf()) break;
        refs.pop_back();    
        
        AlignedNode* node = ref.alignedNode();
        for (size_t i=0; i<N; i++) {
          if (node->child(i) == BVH::emptyNode) continue;
          refs.push_back(BuildRef(node->bounds(i),node->child(i)));
         
#if 1
          NodeRef ref_pre = node->child(i);
          if (ref_pre.isAlignedNode())
            ref_pre.prefetch();
#endif
          std::push_heap (refs.begin(),refs.end()); 
        }
      }
    }

#if defined(EMBREE_GEOMETRY_LINES)    
    Builder* BVH4BuilderTwoLevelLineSegmentsSAH (void* bvh, Scene* scene, const createLineSegmentsAccelTy createMeshAccel) {
      return new BVHNBuilderTwoLevel<4,LineSegments>((BVH4*)bvh,scene,createMeshAccel);
    }
#endif

#if defined(EMBREE_GEOMETRY_TRIANGLES)
    Builder* BVH4BuilderTwoLevelTriangleMeshSAH (void* bvh, Scene* scene, const createTriangleMeshAccelTy createMeshAccel) {
      return new BVHNBuilderTwoLevel<4,TriangleMesh>((BVH4*)bvh,scene,createMeshAccel);
    }
#endif

#if defined(EMBREE_GEOMETRY_QUADS)
    Builder* BVH4BuilderTwoLevelQuadMeshSAH (void* bvh, Scene* scene, const createQuadMeshAccelTy createMeshAccel) {
    return new BVHNBuilderTwoLevel<4,QuadMesh>((BVH4*)bvh,scene,createMeshAccel);
    }
#endif

#if defined(EMBREE_GEOMETRY_USER)
    Builder* BVH4BuilderTwoLevelVirtualSAH (void* bvh, Scene* scene, const createAccelSetAccelTy createMeshAccel) {
    return new BVHNBuilderTwoLevel<4,AccelSet>((BVH4*)bvh,scene,createMeshAccel);
    }
#endif


#if defined(__AVX__)
#if defined(EMBREE_GEOMETRY_TRIANGLES)
    Builder* BVH8BuilderTwoLevelTriangleMeshSAH (void* bvh, Scene* scene, const createTriangleMeshAccelTy createMeshAccel) {
      return new BVHNBuilderTwoLevel<8,TriangleMesh>((BVH8*)bvh,scene,createMeshAccel);
    }
#endif

#if defined(EMBREE_GEOMETRY_QUADS)
    Builder* BVH8BuilderTwoLevelQuadMeshSAH (void* bvh, Scene* scene, const createQuadMeshAccelTy createMeshAccel) {
      return new BVHNBuilderTwoLevel<8,QuadMesh>((BVH8*)bvh,scene,createMeshAccel);
    }
#endif

#if defined(EMBREE_GEOMETRY_USER)
    Builder* BVH8BuilderTwoLevelVirtualSAH (void* bvh, Scene* scene, const createAccelSetAccelTy createMeshAccel) {
      return new BVHNBuilderTwoLevel<8,AccelSet>((BVH8*)bvh,scene,createMeshAccel);
    }
#endif
#endif
  }
}
