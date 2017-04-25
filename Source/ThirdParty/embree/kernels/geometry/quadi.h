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

#include "primitive.h"
#include "../common/scene.h"

namespace embree
{
  /* Stores M quads from an indexed face set */
  template <int M>
  struct QuadMi
  {
    typedef Vec3<vfloat<M>> Vec3vfM;

    /* Virtual interface to query information about the quad type */
    struct Type : public PrimitiveType
    {
      Type();
      size_t size(const char* This) const;
    };
    static Type type;

  public:

    /* Returns maximal number of stored quads */
    static __forceinline size_t max_size() { return M; }
    
    /* Returns required number of primitive blocks for N primitives */
    static __forceinline size_t blocks(size_t N) { return (N+max_size()-1)/max_size(); }
  
  public:

    /* Default constructor */
    __forceinline QuadMi() {  }

    /* Construction from vertices and IDs */
    __forceinline QuadMi(const vint<M>& v0, const vint<M>& v1, const vint<M>& v2, const vint<M>& v3, const vint<M>& geomIDs, const vint<M>& primIDs)
      : v0(v0),v1(v1), v2(v2), v3(v3), geomIDs(geomIDs), primIDs(primIDs) 
    {
    }

    /* Returns a mask that tells which quads are valid */
    __forceinline vbool<M> valid() const { return primIDs != vint<M>(-1); }
    
    /* Returns if the specified quad is valid */
    __forceinline bool valid(const size_t i) const { assert(i<M); return primIDs[i] != -1; }
    
    /* Returns the number of stored quads */
    __forceinline size_t size() const { return __bsf(~movemask(valid())); }
    
    /* Returns the geometry IDs */
    __forceinline vint<M> geomID() const { return geomIDs; }
    __forceinline int geomID(const size_t i) const { assert(i<M); assert(geomIDs[i] != -1); return geomIDs[i]; }
    
    /* Returns the primitive IDs */
    __forceinline vint<M> primID() const { return primIDs; }
    __forceinline int primID(const size_t i) const { assert(i<M); return primIDs[i]; }

    __forceinline Vec3fa& getVertex(const vint<M> &v, const size_t index, const Scene *const scene) const
    {
      const QuadMesh* mesh = scene->get<QuadMesh>(geomID(index));
      return *(Vec3fa*)mesh->vertexPtr(v[index]); // FIMXE: should use loadu
    }

    /* gather the quads */
    __forceinline void gather(Vec3<vfloat<M>>& p0, 
                              Vec3<vfloat<M>>& p1, 
                              Vec3<vfloat<M>>& p2, 
                              Vec3<vfloat<M>>& p3,
                              const Scene *const scene) const;       

#if defined(__AVX512F__)
    __forceinline void gather(Vec3vf16& p0, 
                              Vec3vf16& p1, 
                              Vec3vf16& p2, 
                              Vec3vf16& p3,
                              const Scene *const scene) const;       
#endif    

    /* Fill quad from quad list */
    __forceinline void fill(const PrimRef* prims, size_t& begin, size_t end, Scene* scene)
    {
      vint<M> geomID = -1, primID = -1;
      vint<M> v0 = zero, v1 = zero, v2 = zero, v3 = zero;
      const PrimRef* prim = &prims[begin];
      
      for (size_t i=0; i<M; i++)
      {
	const QuadMesh* mesh = scene->get<QuadMesh>(prim->geomID());
	const QuadMesh::Quad& q = mesh->quad(prim->primID());
	if (begin<end) {
	  geomID[i] = prim->geomID();
	  primID[i] = prim->primID();
	  v0[i] = q.v[0]; 
	  v1[i] = q.v[1]; 
	  v2[i] = q.v[2]; 
	  v3[i] = q.v[3]; 
	  begin++;
	} else {
	  assert(i);
	  geomID[i] = geomID[0]; // always valid geomIDs
	  primID[i] = -1;        // indicates invalid data
	  v0[i] = 0;
	  v1[i] = 0; 
	  v2[i] = 0;
	  v3[i] = 0;
	}
	if (begin<end) prim = &prims[begin];
      }
      
      new (this) QuadMi(v0,v1,v2,v3,geomID,primID); // FIXME: use non temporal store
    }
    
    /* Updates the primitive */
    __forceinline BBox3fa update(QuadMesh* mesh)
    {
      BBox3fa bounds = empty;
      for (size_t i=0; i<M; i++)
      {
        if (!valid(i)) break;
        const unsigned primId = primID(i);
        const QuadMesh::Quad& q = mesh->quad(primId);
        const Vec3fa p0 = mesh->vertex(q.v[0]);
        const Vec3fa p1 = mesh->vertex(q.v[1]);
        const Vec3fa p2 = mesh->vertex(q.v[2]);
        const Vec3fa p3 = mesh->vertex(q.v[3]);
        bounds.extend(merge(BBox3fa(p0),BBox3fa(p1),BBox3fa(p2),BBox3fa(p3)));
      }
      return bounds;
    }

    
  public:
    vint<M> v0;         // index of 1st vertex
    vint<M> v1;         // index of 2nd vertex
    vint<M> v2;         // index of 3rd vertex
    vint<M> v3;         // index of 4rd vertex
    vint<M> geomIDs;    // geometry ID of mesh
    vint<M> primIDs;    // primitive ID of primitive inside mesh
  };

  template<>
  __forceinline void QuadMi<4>::gather(Vec3vf4& p0, 
                                       Vec3vf4& p1, 
                                       Vec3vf4& p2, 
                                       Vec3vf4& p3,
                                       const Scene *const scene) const
  {
    prefetchL1(((char*)this)+0*64);
    prefetchL1(((char*)this)+1*64);

    const QuadMesh* mesh0 = scene->get<QuadMesh>(geomIDs[0]);
    const QuadMesh* mesh1 = scene->get<QuadMesh>(geomIDs[1]);
    const QuadMesh* mesh2 = scene->get<QuadMesh>(geomIDs[2]);
    const QuadMesh* mesh3 = scene->get<QuadMesh>(geomIDs[3]);

    const vfloat4 a0 = vfloat4::loadu(mesh0->vertexPtr(v0[0]));
    const vfloat4 a1 = vfloat4::loadu(mesh1->vertexPtr(v0[1]));
    const vfloat4 a2 = vfloat4::loadu(mesh2->vertexPtr(v0[2]));
    const vfloat4 a3 = vfloat4::loadu(mesh3->vertexPtr(v0[3]));

    transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z);

    const vfloat4 b0 = vfloat4::loadu(mesh0->vertexPtr(v1[0]));
    const vfloat4 b1 = vfloat4::loadu(mesh1->vertexPtr(v1[1]));
    const vfloat4 b2 = vfloat4::loadu(mesh2->vertexPtr(v1[2]));
    const vfloat4 b3 = vfloat4::loadu(mesh3->vertexPtr(v1[3]));

    transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z);

    const vfloat4 c0 = vfloat4::loadu(mesh0->vertexPtr(v2[0]));
    const vfloat4 c1 = vfloat4::loadu(mesh1->vertexPtr(v2[1]));
    const vfloat4 c2 = vfloat4::loadu(mesh2->vertexPtr(v2[2]));
    const vfloat4 c3 = vfloat4::loadu(mesh3->vertexPtr(v2[3]));

    transpose(c0,c1,c2,c3,p2.x,p2.y,p2.z);

    const vfloat4 d0 = vfloat4::loadu(mesh0->vertexPtr(v3[0]));
    const vfloat4 d1 = vfloat4::loadu(mesh1->vertexPtr(v3[1]));
    const vfloat4 d2 = vfloat4::loadu(mesh2->vertexPtr(v3[2]));
    const vfloat4 d3 = vfloat4::loadu(mesh3->vertexPtr(v3[3]));

    transpose(d0,d1,d2,d3,p3.x,p3.y,p3.z);

  }


#if defined(__AVX512F__)
  template<>
  __forceinline void QuadMi<4>::gather(Vec3vf16& p0, 
                                       Vec3vf16& p1, 
                                       Vec3vf16& p2, 
                                       Vec3vf16& p3,
                                       const Scene *const scene) const
  {
    const QuadMesh* mesh0 = scene->get<QuadMesh>(geomIDs[0]);
    const QuadMesh* mesh1 = scene->get<QuadMesh>(geomIDs[1]);
    const QuadMesh* mesh2 = scene->get<QuadMesh>(geomIDs[2]);
    const QuadMesh* mesh3 = scene->get<QuadMesh>(geomIDs[3]);

    const vint16 perm(0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15);
    const vfloat4 &a0 = *(vfloat4*)(mesh0->vertexPtr(v0[0]));
    const vfloat4 &a1 = *(vfloat4*)(mesh1->vertexPtr(v0[1]));
    const vfloat4 &a2 = *(vfloat4*)(mesh2->vertexPtr(v0[2]));
    const vfloat4 &a3 = *(vfloat4*)(mesh3->vertexPtr(v0[3]));

    const vfloat16 _p0(permute(vfloat16(a0,a1,a2,a3),perm));

    const vfloat4 &b0 = *(vfloat4*)(mesh0->vertexPtr(v1[0]));
    const vfloat4 &b1 = *(vfloat4*)(mesh1->vertexPtr(v1[1]));
    const vfloat4 &b2 = *(vfloat4*)(mesh2->vertexPtr(v1[2]));
    const vfloat4 &b3 = *(vfloat4*)(mesh3->vertexPtr(v1[3]));

    const vfloat16 _p1(permute(vfloat16(b0,b1,b2,b3),perm));

    const vfloat4 &c0 = *(vfloat4*)(mesh0->vertexPtr(v2[0]));
    const vfloat4 &c1 = *(vfloat4*)(mesh1->vertexPtr(v2[1]));
    const vfloat4 &c2 = *(vfloat4*)(mesh2->vertexPtr(v2[2]));
    const vfloat4 &c3 = *(vfloat4*)(mesh3->vertexPtr(v2[3]));

    const vfloat16 _p2(permute(vfloat16(c0,c1,c2,c3),perm));

    const vfloat4 &d0 = *(vfloat4*)(mesh0->vertexPtr(v3[0]));
    const vfloat4 &d1 = *(vfloat4*)(mesh1->vertexPtr(v3[1]));
    const vfloat4 &d2 = *(vfloat4*)(mesh2->vertexPtr(v3[2]));
    const vfloat4 &d3 = *(vfloat4*)(mesh3->vertexPtr(v3[3]));

    const vfloat16 _p3(permute(vfloat16(d0,d1,d2,d3),perm));

    p0.x = shuffle4<0>(_p0);
    p0.y = shuffle4<1>(_p0);
    p0.z = shuffle4<2>(_p0);

    p1.x = shuffle4<0>(_p1);
    p1.y = shuffle4<1>(_p1);
    p1.z = shuffle4<2>(_p1);

    p2.x = shuffle4<0>(_p2);
    p2.y = shuffle4<1>(_p2);
    p2.z = shuffle4<2>(_p2);

    p3.x = shuffle4<0>(_p3);
    p3.y = shuffle4<1>(_p3);
    p3.z = shuffle4<2>(_p3);

  }

#endif

  template<int M>
  typename QuadMi<M>::Type QuadMi<M>::type;

  typedef QuadMi<4> Quad4i;
}
