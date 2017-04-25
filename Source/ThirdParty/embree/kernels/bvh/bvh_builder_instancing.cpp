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

#include "bvh_builder_instancing.h"
#include "bvh_statistics.h"
#include "../builders/bvh_builder_sah.h"
#include "../geometry/triangle.h"
#include "../geometry/trianglev_mb.h"
#include "../geometry/quadv.h"
#include "../geometry/quadi.h"
#include "../geometry/quadi_mb.h"

namespace embree
{
  namespace isa
  {
    template<int N, typename Mesh>
    BVHNBuilderInstancing<N,Mesh>::BVHNBuilderInstancing (BVH* bvh, Scene* scene, const createMeshAccelTy createMeshAccel)
      : bvh(bvh), objects(bvh->objects), createMeshAccel(createMeshAccel), scene(scene), refs(scene->device), prims(scene->device), nextRef(0) {}
    
    template<int N, typename Mesh>
    BVHNBuilderInstancing<N,Mesh>::~BVHNBuilderInstancing ()
    {
      for (size_t i=0; i<builders.size(); i++) 
	delete builders[i];
    }

    int hash(const Vec3fa& vec)
    {
      int h = 0;
      for (size_t i=0; i<3; i++)
        h ^= 0x42F276E1*i*((int*)&vec)[i];
      return h;
    }

    int hash(const AffineSpace3fa& space)
    {
      int h = 0;
      h ^= 0xF2FF7631*hash(space.l.vx);
      h ^= 0xF2FF7731*hash(space.l.vy);
      h ^= 0xF2FF7831*hash(space.l.vz);
      h ^= 0xF2FF7931*hash(space.p);
      return h;
    }

    /*int slot(int type, int numTimeSteps)
    {
      if (numTimeSteps == 1)
      {
        switch (type) {
        case Geometry::TRIANGLE_MESH: return 0;
        case Geometry::USER_GEOMETRY: break;
        case Geometry::BEZIER_CURVES: break;
        case Geometry::SUBDIV_MESH  : break;
        }
      } else {
        switch (type) {
        case Geometry::TRIANGLE_MESH: return 1;
        case Geometry::USER_GEOMETRY: break;
        case Geometry::BEZIER_CURVES: break;
        case Geometry::SUBDIV_MESH  : break;
        }
      }
      assert(false);
      return 0;
      }*/
    
    template<int N, typename Mesh>
    const BBox3fa xfmDeepBounds(const AffineSpace3fa& xfm, const BBox3fa& bounds, typename BVHN<N>::NodeRef ref, size_t depth)
    {
      if (ref == BVHN<N>::emptyNode) return empty;
      if (depth == 0 || !ref.isAlignedNode()) return xfmBounds(xfm,bounds);
      typename BVHN<N>::AlignedNode* node = ref.alignedNode();

      BBox3fa box = empty;
      for (size_t i=0; i<N; i++)
        box.extend(xfmDeepBounds<N>(xfm,node->bounds(i),node->child(i),depth-1));
      return box;
    }

    template<int N, typename Mesh>
    void BVHNBuilderInstancing<N,Mesh>::build()
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
      size_t numPrimitives = 0;
      //numPrimitives += scene->getNumPrimitives<TriangleMesh,false>();
      numPrimitives += scene->instanced.numTriangles;
      numPrimitives += scene->instancedMB.numTriangles;
      if (numPrimitives == 0) {
        prims.resize(0);
        bvh->set(BVH::emptyNode,empty,0);
        return;
      }
      
      double t0 = bvh->preBuild(TOSTRING(isa) "::BVH" + toString(N) + "BuilderInstancing");
      
      /* resize object array if scene got larger */
      if (objects.size()  < num) objects.resize(num);
      if (builders.size() < num) builders.resize(num);
      if (refs.size()     < num) refs.resize(num);
      nextRef.store(0);
      
      /* creation of acceleration structures */
      parallel_for(size_t(0), num, [&] (const range<size_t>& r) {
          for (size_t objectID=r.begin(); objectID<r.end(); objectID++)
          {
            Mesh* mesh = scene->getSafe<Mesh>(objectID);
            
            /* verify if geometry got deleted properly */
            if (mesh == nullptr) {
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
      parallel_for(size_t(0), num, [&] (const range<size_t>& r) {
          for (size_t objectID=r.begin(); objectID<r.end(); objectID++)
          {
            Geometry* geom = scene->get(objectID);
            if (geom == nullptr) continue;
            Builder* builder = builders[objectID]; 
            if (builder == nullptr) continue;
            if (geom->isModified() && geom->isInstanced()) 
              builder->build();
          }
        });

      /* creates all instances */
      parallel_for(size_t(0), num, [&] (const range<size_t>& r) {
          for (size_t objectID=r.begin(); objectID<r.end(); objectID++)
          {
            Geometry* geom = scene->get(objectID);
            if (geom == nullptr) continue;
            if (!(geom->getType() & Geometry::INSTANCE)) continue;
            GeometryInstance* instance = (GeometryInstance*) geom;
            if (!instance->isEnabled()) continue;
            BVH* object = objects[instance->geom->id];
            if (object == nullptr) continue;
            if (object->getBounds().empty()) continue;
            int s = 0; //slot(geom->getType() & ~Geometry::INSTANCE, geom->numTimeSteps);
            refs[nextRef++] = BVHNBuilderInstancing::BuildRef(instance->local2world,object->getBounds(),object->root,instance->mask,unsigned(objectID),hash(instance->local2world),s);
          }
        });
      refs.resize(nextRef);

#if 0
      /* compute transform IDs */
      std::sort(refs.begin(),refs.end(), [] (const BuildRef& ref0, const BuildRef& ref1) { return ref0.xfmID < ref1.xfmID; });
      
      int lastXfmID = 0;
      AffineSpace3fa lastXfm = one;
      for (size_t i=0; i<refs.size(); i++) {
        if (refs[i].local2world != lastXfm) {
          lastXfmID++;
          lastXfm = refs[i].local2world;
        }
        refs[i].xfmID = lastXfmID;
      }
#endif   
      
      /* fast path for single geometry scenes */
      /*if (nextRef == 1) { 
        bvh->set(refs[0].node,refs[0].bounds(),numPrimitives);
        return;
        }*/
      
      /* open all large nodes */  
      open(numPrimitives); 

      /* fast path for small geometries */
      /*if (refs.size() == 1) { 
        bvh->set(refs[0].node,refs[0].bounds(),numPrimitives);
        return;
        }*/

      bvh->alloc.init_estimate(refs.size()*16);

      /* compute PrimRefs */
      prims.resize(refs.size());
      const PrimInfo pinfo = parallel_reduce(size_t(0), refs.size(), size_t(1024), PrimInfo(empty), [&] (const range<size_t>& r) -> PrimInfo {
          PrimInfo pinfo(empty);
          for (size_t i=r.begin(); i<r.end(); i++) 
          {
            const BBox3fa bounds = refs[i].worldBounds();
            pinfo.add(bounds);
            prims[i] = PrimRef(bounds,(size_t)&refs[i]);
          }
          return pinfo;
        }, [] (const PrimInfo& a, const PrimInfo& b) { return PrimInfo::merge(a,b); });
      
      /* skip if all objects where empty */
      if (pinfo.size() == 0)
        bvh->set(BVH::emptyNode,empty,0);
      
#if 0 // this codepath is buggy, have to create transform node here too
      else if (pinfo.size() == 1) {
        BuildRef* ref = (BuildRef*) prims[0].ID();
        //const BBox3fa bounds = xfmBounds(ref->local2world,ref->localBounds);
        const BBox3fa bounds = xfmDeepBounds<N>(ref->local2world,ref->localBounds,ref->node,2);
        bvh->set(ref->node,LBBox3fa(bounds),numPrimitives);
      }
#endif
 
      /* otherwise build toplevel hierarchy */
      else
      {
        /* settings for BVH build */
        GeneralBVHBuilder::Settings settings;
        settings.branchingFactor = N;
        settings.maxDepth = BVH::maxBuildDepthLeaf;
        settings.logBlockSize = __bsr(4);
        settings.minLeafSize = 1;
        settings.maxLeafSize = 1;
        settings.travCost = 1.0f;
        settings.intCost = 1.0f;
        settings.singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD;

        NodeRef root = BVHBuilderBinnedSAH::build<NodeRef>
          (
            typename BVH::CreateAlloc(bvh),
            typename BVH::AlignedNode::Create2(),
            typename BVH::AlignedNode::Set2(),

           [&] (const BVHBuilderBinnedSAH::BuildRecord& current, FastAllocator::ThreadLocal2* alloc) -> NodeRef
          {
            assert(current.prims.size() == 1);
            BuildRef* ref = (BuildRef*) prims[current.prims.begin()].ID();
            TransformNode* node = (TransformNode*) alloc->alloc0->malloc(sizeof(TransformNode),BVH::byteAlignment);
            new (node) TransformNode(ref->local2world,ref->localBounds,ref->node,ref->mask,ref->instID,ref->xfmID,ref->type); // FIXME: rcp should be precalculated somewhere
            NodeRef noderef = BVH::encodeNode(node);
            noderef.setBarrier();
            return noderef;
          },
           [&] (size_t dn) { bvh->scene->progressMonitor(0); },
            prims.data(),pinfo,settings);
        
        bvh->set(root,LBBox3fa(pinfo.geomBounds),numPrimitives);
        numCollapsedTransformNodes = refs.size();
        bvh->root = collapse(bvh->root);
        if (scene->device->verbosity(1))
          std::cout << "collapsing from " << refs.size() << " to " << numCollapsedTransformNodes << " minimally possible " << nextRef << std::endl;
      }
            
      bvh->alloc.cleanup();
      bvh->postBuild(t0);
    }
    
    template<int N, typename Mesh>
    void BVHNBuilderInstancing<N,Mesh>::deleteGeometry(size_t geomID)
    {
      if (geomID >= objects.size()) return;
      delete builders[geomID]; builders[geomID] = nullptr;
      delete objects [geomID]; objects [geomID] = nullptr;
    }

    template<int N, typename Mesh>
    void BVHNBuilderInstancing<N,Mesh>::clear()
    {
      for (size_t i=0; i<objects.size(); i++) 
        if (objects[i]) objects[i]->clear();
      
      for (size_t i=0; i<builders.size(); i++) 
	if (builders[i]) builders[i]->clear();
      
      refs.clear();
    }
    
    template<int N, typename Mesh>
    void BVHNBuilderInstancing<N,Mesh>::open(size_t numInstancedPrimitives)
    {
      if (refs.size() == 0)
	return;
     
      if (scene->device->benchmark) { std::cout << "BENCHMARK_INSTANCES " << refs.size() << std::endl; }
      if (scene->device->benchmark) { std::cout << "BENCHMARK_INSTANCED_PRIMITIVES " << numInstancedPrimitives << std::endl; }
      
      /* calculate opening size */
      size_t num = 0;
      if      (scene->device->instancing_block_size ) num = numInstancedPrimitives/scene->device->instancing_block_size;
      else if (scene->device->instancing_open_factor != 0.0f) num = scene->device->instancing_open_factor*refs.size();
      num = max(num,scene->device->instancing_open_min);
      num = min(num,scene->device->instancing_open_max);
      refs.reserve(num);

      std::make_heap(refs.begin(),refs.end());
      while (refs.size()+N-1 <= num)
      {
        std::pop_heap (refs.begin(),refs.end()); 
        BuildRef ref = refs.back();
        refs.pop_back();    

        if (ref.depth >= scene->device->instancing_open_max_depth) {
          ref.clearArea();
          refs.push_back(ref);
          std::push_heap (refs.begin(),refs.end()); 
          continue;
        }
        
        if (ref.node.isAlignedNode()) 
        {
          AlignedNode* node = ref.node.alignedNode();
          for (size_t i=0; i<N; i++) {
            if (node->child(i) == BVH::emptyNode) continue;
            refs.push_back(BuildRef(ref.local2world,node->bounds(i),node->child(i),ref.mask,ref.instID,ref.xfmID,ref.type,ref.depth+1));
            std::push_heap (refs.begin(),refs.end()); 
          }
        } 
        /*else if (ref.node.isAlignedNodeMB()) 
        {
          AlignedNodeMB* node = ref.node.alignedNodeMB();
          for (size_t i=0; i<N; i++) {
            if (node->child(i) == BVH::emptyNode) continue;
            refs.push_back(BuildRef(ref.local2world,node->bounds(i),node->child(i),ref.mask,ref.instID,ref.xfmID,ref.type,ref.depth+1));
            std::push_heap (refs.begin(),refs.end()); 
          }
        }*/
        else {
          refs.push_back(ref);
          break;
        }
      }

      if (scene->device->benchmark) { std::cout << "BENCHMARK_OPENED_INSTANCES " << refs.size() << std::endl; }
    }
    
    template<int N, typename Mesh>
    typename BVHNBuilderInstancing<N,Mesh>::NodeRef BVHNBuilderInstancing<N,Mesh>::collapse(NodeRef& node)
    {
      if (node.isBarrier()) {
        node.clearBarrier();
        return node;
      }
      
      assert(node.isAlignedNode());
      AlignedNode* n = node.alignedNode();
      TransformNode* first = nullptr;
      for (size_t c=0; c<N; c++) {
        if (n->child(c) == BVH::emptyNode) continue;
        NodeRef child = n->child(c) = collapse(n->child(c));
        if (child.isTransformNode()) first = child.transformNode();
      }
      
      bool allEqual = true;
      for (size_t c=0; c<N; c++)
      {
        NodeRef child = n->child(c);
        if (child == BVH::emptyNode) continue;
        
        if (!child.isTransformNode()) {
          allEqual = false;
          break;
        }
        
        if (child.transformNode()->world2local != first->world2local) {
          allEqual = false;
          break;
        }
        
        if (child.transformNode()->instID != first->instID) {
          allEqual = false;
          break;
        }
      }
      
      if (!allEqual) 
        return node;
      
      BBox3fa bounds = empty;
      for (size_t c=0; c<N; c++) {
        if (n->child(c) == BVH::emptyNode) continue;
        TransformNode* child = n->child(c).transformNode();
        numCollapsedTransformNodes--;
        const BBox3fa cbounds = child->localBounds;
        n->set(c,cbounds,child->child);
        bounds.extend(cbounds);
      }
      numCollapsedTransformNodes++;
      first->localBounds = bounds;
      first->child = node;
      return BVH::encodeNode(first);
    }
    
    Builder* BVH4BuilderInstancingTriangleMeshSAH (void* bvh, Scene* scene, const createTriangleMeshAccelTy createTriangleMeshAccel) {
      return new BVHNBuilderInstancing<4,TriangleMesh>((BVH4*)bvh,scene,createTriangleMeshAccel);
    }

    Builder* BVH4BuilderInstancingQuadMeshSAH (void* bvh, Scene* scene, const createQuadMeshAccelTy createQuadMeshAccel) {
      return new BVHNBuilderInstancing<4,QuadMesh>((BVH4*)bvh,scene,createQuadMeshAccel);
    }
  }
}
