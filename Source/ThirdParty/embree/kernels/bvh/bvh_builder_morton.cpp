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

#include "bvh.h"
#include "bvh_statistics.h"
#include "bvh_rotate.h"
#include "../common/profile.h"
#include "../../common/algorithms/parallel_prefix_sum.h"

#include "../builders/primrefgen.h"
#include "../builders/bvh_builder_morton.h"

#include "../geometry/triangle.h"
#include "../geometry/trianglev.h"
#include "../geometry/trianglei.h"
#include "../geometry/quadv.h"
#include "../geometry/quadi.h"
#include "../geometry/object.h"

#define ROTATE_TREE 1 // specifies number of tree rotation rounds to perform

namespace embree 
{
  namespace isa
  {
    template<int N>
    struct SetBVHNBounds
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;
      typedef typename BVH::AlignedNode AlignedNode;

      BVH* bvh;
      __forceinline SetBVHNBounds (BVH* bvh) : bvh(bvh) {}

      __forceinline std::pair<NodeRef,BBox3fa> operator() (NodeRef ref, const std::pair<NodeRef,BBox3fa>* children, size_t num)
      {
        AlignedNode* node = ref.alignedNode();

        BBox3fa res = empty;
        for (size_t i=0; i<num; i++) {
          const BBox3fa b = children[i].second;
          res.extend(b);
          node->set(i,children[i].first);
          node->set(i,b);
        }

#if ROTATE_TREE
        if (N == 4)
        {
          size_t n = 0;
          for (size_t i=0; i<num; i++)
            n += children[i].second.lower.a;

          if (n >= 4096) {
            for (size_t i=0; i<num; i++) {
              if (children[i].second.lower.a < 4096) {
                for (int j=0; j<ROTATE_TREE; j++)
                  BVHNRotate<N>::rotate(node->child(i));
                node->child(i).setBarrier();
              }
            }
          }
          res.lower.a = unsigned(n);
        }
#endif

        return std::make_pair(ref,res);
      }
    };

    template<int N, typename Primitive>
    struct CreateMortonLeaf;

    template<int N>
    struct CreateMortonLeaf<N,Triangle4>
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      __forceinline CreateMortonLeaf (TriangleMesh* mesh, BVHBuilderMorton::BuildPrim* morton)
        : mesh(mesh), morton(morton) {}

      __noinline std::pair<NodeRef,BBox3fa> operator() (const range<unsigned>& current, FastAllocator::ThreadLocal2* alloc)
      {
        vfloat4 lower(pos_inf);
        vfloat4 upper(neg_inf);
        size_t items = current.size();
        size_t start = current.begin();
        assert(items<=4);
        
        /* allocate leaf node */
        Triangle4* accel = (Triangle4*) alloc->alloc1->malloc(sizeof(Triangle4),BVH::byteAlignment);
        NodeRef ref = BVH::encodeLeaf((char*)accel,1);
        vint4 vgeomID = -1, vprimID = -1;
        Vec3vf4 v0 = zero, v1 = zero, v2 = zero;
        const unsigned geomID = this->mesh->id;
        const TriangleMesh* __restrict__ const mesh = this->mesh;

        for (size_t i=0; i<items; i++)
        {
          const unsigned primID = morton[start+i].index;
          const TriangleMesh::Triangle& tri = mesh->triangle(primID);
          const Vec3fa& p0 = mesh->vertex(tri.v[0]);
          const Vec3fa& p1 = mesh->vertex(tri.v[1]);
          const Vec3fa& p2 = mesh->vertex(tri.v[2]);
          lower = min(lower,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          upper = max(upper,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          vgeomID [i] = geomID;
          vprimID [i] = primID;
          v0.x[i] = p0.x; v0.y[i] = p0.y; v0.z[i] = p0.z;
          v1.x[i] = p1.x; v1.y[i] = p1.y; v1.z[i] = p1.z;
          v2.x[i] = p2.x; v2.y[i] = p2.y; v2.z[i] = p2.z;
        }

        Triangle4::store_nt(accel,Triangle4(v0,v1,v2,vgeomID,vprimID));
        BBox3fa box_o = BBox3fa((Vec3fa)lower,(Vec3fa)upper);
#if ROTATE_TREE
        if (N == 4)
          box_o.lower.a = unsigned(current.size());
#endif
        return std::make_pair(ref,box_o);
      }
    
    private:
      TriangleMesh* mesh;
      BVHBuilderMorton::BuildPrim* morton;
    };
    
    template<int N>
    struct CreateMortonLeaf<N,Triangle4v>
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      __forceinline CreateMortonLeaf (TriangleMesh* mesh, BVHBuilderMorton::BuildPrim* morton)
        : mesh(mesh), morton(morton) {}
      
      __noinline std::pair<NodeRef,BBox3fa> operator() (const range<unsigned>& current, FastAllocator::ThreadLocal2* alloc)
      {
        vfloat4 lower(pos_inf);
        vfloat4 upper(neg_inf);
        size_t items = current.size();
        size_t start = current.begin();
        assert(items<=4);
        
        /* allocate leaf node */
        Triangle4v* accel = (Triangle4v*) alloc->alloc1->malloc(sizeof(Triangle4v),BVH::byteAlignment);
        NodeRef ref = BVH::encodeLeaf((char*)accel,1);       
        vint4 vgeomID = -1, vprimID = -1;
        Vec3vf4 v0 = zero, v1 = zero, v2 = zero;
        const unsigned geomID = this->mesh->id;
        const TriangleMesh* __restrict__ mesh = this->mesh;

        for (size_t i=0; i<items; i++)
        {
          const unsigned primID = morton[start+i].index;
          const TriangleMesh::Triangle& tri = mesh->triangle(primID);
          const Vec3fa& p0 = mesh->vertex(tri.v[0]);
          const Vec3fa& p1 = mesh->vertex(tri.v[1]);
          const Vec3fa& p2 = mesh->vertex(tri.v[2]);
          lower = min(lower,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          upper = max(upper,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          vgeomID [i] = geomID;
          vprimID [i] = primID;
          v0.x[i] = p0.x; v0.y[i] = p0.y; v0.z[i] = p0.z;
          v1.x[i] = p1.x; v1.y[i] = p1.y; v1.z[i] = p1.z;
          v2.x[i] = p2.x; v2.y[i] = p2.y; v2.z[i] = p2.z;
        }
        Triangle4v::store_nt(accel,Triangle4v(v0,v1,v2,vgeomID,vprimID));
        BBox3fa box_o = BBox3fa((Vec3fa)lower,(Vec3fa)upper);
#if ROTATE_TREE
        if (N == 4)
          box_o.lower.a = current.size();
#endif
        return std::make_pair(ref,box_o);
      }
    private:
      TriangleMesh* mesh;
      BVHBuilderMorton::BuildPrim* morton;
    };

    template<int N>
    struct CreateMortonLeaf<N,Triangle4i>
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      __forceinline CreateMortonLeaf (TriangleMesh* mesh, BVHBuilderMorton::BuildPrim* morton)
        : mesh(mesh), morton(morton) {}
      
      __noinline std::pair<NodeRef,BBox3fa> operator() (const range<unsigned>& current, FastAllocator::ThreadLocal2* alloc)
      {
        vfloat4 lower(pos_inf);
        vfloat4 upper(neg_inf);
        size_t items = current.size();
        size_t start = current.begin();
        assert(items<=4);
        
        /* allocate leaf node */
        Triangle4i* accel = (Triangle4i*) alloc->alloc1->malloc(sizeof(Triangle4i),BVH::byteAlignment);
        NodeRef ref = BVH::encodeLeaf((char*)accel,1);
        
        vint4 vgeomID = -1, vprimID = -1;
        vint4 v0 = zero, v1 = zero, v2 = zero;
        const unsigned geomID = this->mesh->id;
        const TriangleMesh* __restrict__ const mesh = this->mesh;
        
        for (size_t i=0; i<items; i++)
        {
          const unsigned primID = morton[start+i].index;
          const TriangleMesh::Triangle& tri = mesh->triangle(primID);
          const Vec3fa& p0 = mesh->vertex(tri.v[0]);
          const Vec3fa& p1 = mesh->vertex(tri.v[1]);
          const Vec3fa& p2 = mesh->vertex(tri.v[2]);
          lower = min(lower,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          upper = max(upper,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2);
          vgeomID[i] = geomID;
          vprimID[i] = primID;
          int* base = (int*) mesh->vertexPtr(tri.v[0]);
          v0[i] = tri.v[0];
          v1[i] = int(ssize_t((int*)mesh->vertexPtr(tri.v[1])-base)); 
          v2[i] = int(ssize_t((int*)mesh->vertexPtr(tri.v[2])-base)); 
        }
        
        for (size_t i=items; i<4; i++)
        {
          vgeomID[i] = vgeomID[0];
          vprimID[i] = -1;
          v0[i] = 0;
          v1[i] = 0; 
          v2[i] = 0;
        }
        
        Triangle4i::store_nt(accel,Triangle4i(v0,v1,v2,vgeomID,vprimID));
        BBox3fa box_o = BBox3fa((Vec3fa)lower,(Vec3fa)upper);
#if ROTATE_TREE
        if (N == 4)
          box_o.lower.a = current.size();
#endif
        return std::make_pair(ref,box_o);
      }
    private:
      TriangleMesh* mesh;
      BVHBuilderMorton::BuildPrim* morton;
    };

    template<int N>
    struct CreateMortonLeaf<N,Quad4v>
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      __forceinline CreateMortonLeaf (QuadMesh* mesh, BVHBuilderMorton::BuildPrim* morton)
        : mesh(mesh), morton(morton) {}
      
      __noinline std::pair<NodeRef,BBox3fa> operator() (const range<unsigned>& current, FastAllocator::ThreadLocal2* alloc)
      {
        vfloat4 lower(pos_inf);
        vfloat4 upper(neg_inf);
        size_t items = current.size();
        size_t start = current.begin();
        assert(items<=4);
        
        /* allocate leaf node */
        Quad4v* accel = (Quad4v*) alloc->alloc1->malloc(sizeof(Quad4v),BVH::byteAlignment);
        NodeRef ref = BVH::encodeLeaf((char*)accel,1);
        
        vint4 vgeomID = -1, vprimID = -1;
        Vec3vf4 v0 = zero, v1 = zero, v2 = zero, v3 = zero;
        const unsigned geomID = this->mesh->id;
        const QuadMesh* __restrict__ mesh = this->mesh;

        for (size_t i=0; i<items; i++)
        {
          const unsigned primID = morton[start+i].index;
          const QuadMesh::Quad& tri = mesh->quad(primID);
          const Vec3fa& p0 = mesh->vertex(tri.v[0]);
          const Vec3fa& p1 = mesh->vertex(tri.v[1]);
          const Vec3fa& p2 = mesh->vertex(tri.v[2]);
          const Vec3fa& p3 = mesh->vertex(tri.v[3]);
          lower = min(lower,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2,(vfloat4)p3);
          upper = max(upper,(vfloat4)p0,(vfloat4)p1,(vfloat4)p2,(vfloat4)p3);
          vgeomID [i] = geomID;
          vprimID [i] = primID;
          v0.x[i] = p0.x; v0.y[i] = p0.y; v0.z[i] = p0.z;
          v1.x[i] = p1.x; v1.y[i] = p1.y; v1.z[i] = p1.z;
          v2.x[i] = p2.x; v2.y[i] = p2.y; v2.z[i] = p2.z;
          v3.x[i] = p3.x; v3.y[i] = p3.y; v3.z[i] = p3.z;
        }
        Quad4v::store_nt(accel,Quad4v(v0,v1,v2,v3,vgeomID,vprimID));
        BBox3fa box_o = BBox3fa((Vec3fa)lower,(Vec3fa)upper);
#if ROTATE_TREE
        if (N == 4)
          box_o.lower.a = current.size();
#endif
        return std::make_pair(ref,box_o);
      }
    private:
      QuadMesh* mesh;
      BVHBuilderMorton::BuildPrim* morton;
    };

    template<int N>
    struct CreateMortonLeaf<N,Object>
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::NodeRef NodeRef;

      __forceinline CreateMortonLeaf (AccelSet* mesh, BVHBuilderMorton::BuildPrim* morton)
        : mesh(mesh), morton(morton) {}
      
      __noinline std::pair<NodeRef,BBox3fa> operator() (const range<unsigned>& current, FastAllocator::ThreadLocal2* alloc)
      {
        vfloat4 lower(pos_inf);
        vfloat4 upper(neg_inf);
        size_t items = current.size();
        size_t start = current.begin();
        
        /* allocate leaf node */
        Object* accel = (Object*) alloc->alloc1->malloc(items*sizeof(Object),BVH::byteAlignment);
        NodeRef ref = BVH::encodeLeaf((char*)accel,items);

        const unsigned geomID = this->mesh->id;
        const AccelSet* mesh = this->mesh;
        
        BBox3fa bounds = empty;
        for (size_t i=0; i<items; i++)
        {
          const unsigned index = morton[start+i].index;
          const unsigned primID = index; 
          bounds.extend(mesh->bounds(primID));
          new (&accel[i]) Object(geomID,primID);
        }
        BBox3fa box_o = bounds;
#if ROTATE_TREE
        if (N == 4)
          box_o.lower.a = current.size();
#endif
        return std::make_pair(ref,box_o);
      }
    private:
      AccelSet* mesh;
      BVHBuilderMorton::BuildPrim* morton;
    };

    template<typename Mesh>
    struct CalculateMeshBounds
    {
      __forceinline CalculateMeshBounds (Mesh* mesh)
        : mesh(mesh) {}
      
      __forceinline const BBox3fa operator() (const BVHBuilderMorton::BuildPrim& morton) {
        return mesh->bounds(morton.index);
      }
      
    private:
      Mesh* mesh;
    };        

    template<int N, typename Mesh, typename Primitive>
    class BVHNMeshBuilderMorton : public Builder
    {
      typedef BVHN<N> BVH;
      typedef typename BVH::AlignedNode AlignedNode;
      typedef typename BVH::NodeRef NodeRef;

    public:
      
      BVHNMeshBuilderMorton (BVH* bvh, Mesh* mesh, const size_t minLeafSize, const size_t maxLeafSize, const size_t singleThreadThreshold = DEFAULT_SINGLE_THREAD_THRESHOLD)
        : bvh(bvh), mesh(mesh), morton(bvh->device), settings(N,BVH::maxBuildDepth,minLeafSize,maxLeafSize,singleThreadThreshold) {}
      
      /* build function */
      void build() 
      {
        /* we reset the allocator when the mesh size changed */
        if (mesh->numPrimitivesChanged) {
          bvh->alloc.clear();
          morton.clear();
          mesh->numPrimitivesChanged = false;
        }
        size_t numPrimitives = mesh->size();
        
        /* skip build for empty scene */
        if (numPrimitives == 0) {
          bvh->set(BVH::emptyNode,empty,0);
          return;
        }
        
        /* preallocate arrays */
        morton.resize(numPrimitives);
        size_t bytesAllocated = numPrimitives*sizeof(AlignedNode)/(4*N) + size_t(1.2f*Primitive::blocks(numPrimitives)*sizeof(Primitive));
        size_t bytesMortonCodes = numPrimitives*sizeof(BVHBuilderMorton::BuildPrim);
        bytesAllocated = max(bytesAllocated,bytesMortonCodes); // the first allocation block is reused to sort the morton codes
        bvh->alloc.init(bytesAllocated,2*bytesAllocated);

        /* create morton code array */
        BVHBuilderMorton::BuildPrim* dest = (BVHBuilderMorton::BuildPrim*) bvh->alloc.specialAlloc(bytesMortonCodes);
        size_t numPrimitivesGen = createMortonCodeArray<Mesh>(mesh,morton,bvh->scene->progressInterface);

        /* create BVH */
        SetBVHNBounds<N> setBounds(bvh);
        CreateMortonLeaf<N,Primitive> createLeaf(mesh,morton.data());
        CalculateMeshBounds<Mesh> calculateBounds(mesh);
        auto root = BVHBuilderMorton::build<std::pair<NodeRef,BBox3fa>>(
          typename BVH::CreateAlloc(bvh), 
          typename BVH::AlignedNode::Create(),
          setBounds,createLeaf,calculateBounds,bvh->scene->progressInterface,
          morton.data(),dest,numPrimitivesGen,settings);
        
        bvh->set(root.first,LBBox3fa(root.second),numPrimitives);
        
#if ROTATE_TREE
        if (N == 4)
        {
          for (int i=0; i<ROTATE_TREE; i++)
            BVHNRotate<N>::rotate(bvh->root);
          bvh->clearBarrier(bvh->root);
        }
#endif

        /* clear temporary data for static geometry */
        if (mesh->isStatic()) 
        {
          morton.clear();
          bvh->shrink();
        }
        bvh->cleanup();
      }
      
      void clear() {
        morton.clear();
      }
      
    private:
      BVH* bvh;
      Mesh* mesh;
      mvector<BVHBuilderMorton::BuildPrim> morton;
      BVHBuilderMorton::Settings settings;
    };

#if defined(EMBREE_GEOMETRY_TRIANGLES)
    Builder* BVH4Triangle4MeshBuilderMortonGeneral  (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<4,TriangleMesh,Triangle4> ((BVH4*)bvh,mesh,4,4); }
    Builder* BVH4Triangle4vMeshBuilderMortonGeneral (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<4,TriangleMesh,Triangle4v>((BVH4*)bvh,mesh,4,4); }
    Builder* BVH4Triangle4iMeshBuilderMortonGeneral (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<4,TriangleMesh,Triangle4i>((BVH4*)bvh,mesh,4,4); }
#if defined(__AVX__)
    Builder* BVH8Triangle4MeshBuilderMortonGeneral  (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<8,TriangleMesh,Triangle4> ((BVH8*)bvh,mesh,4,4); }
    Builder* BVH8Triangle4vMeshBuilderMortonGeneral (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<8,TriangleMesh,Triangle4v>((BVH8*)bvh,mesh,4,4); }
    Builder* BVH8Triangle4iMeshBuilderMortonGeneral (void* bvh, TriangleMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<8,TriangleMesh,Triangle4i>((BVH8*)bvh,mesh,4,4); }
#endif
#endif

#if defined(EMBREE_GEOMETRY_QUADS)
    Builder* BVH4Quad4vMeshBuilderMortonGeneral (void* bvh, QuadMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<4,QuadMesh,Quad4v>((BVH4*)bvh,mesh,4,4); }
#if defined(__AVX__)
    Builder* BVH8Quad4vMeshBuilderMortonGeneral (void* bvh, QuadMesh* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<8,QuadMesh,Quad4v>((BVH8*)bvh,mesh,4,4); }
#endif
#endif

#if defined(EMBREE_GEOMETRY_USER)
    Builder* BVH4VirtualMeshBuilderMortonGeneral (void* bvh, AccelSet* mesh, size_t mode) { return new class BVHNMeshBuilderMorton<4,AccelSet,Object>((BVH4*)bvh,mesh,1,BVH4::maxLeafBlocks); }
#endif

  }
}
