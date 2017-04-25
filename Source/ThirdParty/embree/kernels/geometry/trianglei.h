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

namespace embree
{
  /* Stores M triangles from an indexed face set */
  template <int M>
  struct TriangleMi
  {
    typedef Vec3<vfloat<M>> Vec3vfM;

    /* Virtual interface to query information about the triangle type */
    struct Type : public PrimitiveType
    {
      Type();
      size_t size(const char* This) const;
    };
    static Type type;

  public:

    /* Returns maximal number of stored triangles */
    static __forceinline size_t max_size() { return M; }
    
    /* Returns required number of primitive blocks for N primitives */
    static __forceinline size_t blocks(size_t N) { return (N+max_size()-1)/max_size(); }
  
  public:

    /* Default constructor */
    __forceinline TriangleMi() {  }

    /* Construction from vertices and IDs */
    __forceinline TriangleMi(const vint<M>& v0, const vint<M>& v1, const vint<M>& v2, const vint<M>& geomIDs, const vint<M>& primIDs)
      : v0(v0), v1(v1), v2(v2), geomIDs(geomIDs), primIDs(primIDs) {}
    
    /* Returns a mask that tells which triangles are valid */
    __forceinline vbool<M> valid() const { return primIDs != vint<M>(-1); }
    
    /* Returns if the specified triangle is valid */
    __forceinline bool valid(const size_t i) const { assert(i<M); return primIDs[i] != -1; }
    
    /* Returns the number of stored triangles */
    __forceinline size_t size() const { return __bsf(~movemask(valid())); }
    
    /* Returns the geometry IDs */
    __forceinline vint<M> geomID() const { return geomIDs; }
    __forceinline int geomID(const size_t i) const { assert(i<M); return geomIDs[i]; }
    
    /* Returns the primitive IDs */
    __forceinline vint<M> primID() const { return primIDs; }
    __forceinline int primID(const size_t i) const { assert(i<M); return primIDs[i]; }

    /* gather the triangles */
    __forceinline void gather(Vec3<vfloat<M>>& p0, Vec3<vfloat<M>>& p1, Vec3<vfloat<M>>& p2, const Scene* const scene) const;
    
     /* Non temporal store */
    __forceinline static void store_nt(TriangleMi* dst, const TriangleMi& src)
    {
      vint<M>::store_nt(&dst->v0,src.v0);
      vint<M>::store_nt(&dst->v1,src.v1);
      vint<M>::store_nt(&dst->v2,src.v2);
      vint<M>::store_nt(&dst->geomIDs,src.geomIDs);
      vint<M>::store_nt(&dst->primIDs,src.primIDs);
    }
    
    /* Fill triangle from triangle list */
    __forceinline void fill(const PrimRef* prims, size_t& begin, size_t end, Scene* scene)
    {
      vint<M> geomID = -1, primID = -1;
      vint<M> v0 = zero, v1 = zero, v2 = zero;
      const PrimRef* prim = &prims[begin];
      
      for (size_t i=0; i<M; i++)
      {
	const TriangleMesh* mesh = scene->get<TriangleMesh>(prim->geomID());
	const TriangleMesh::Triangle& tri = mesh->triangle(prim->primID());
	if (begin<end) {
	  geomID[i] = prim->geomID();
	  primID[i] = prim->primID();
          int* base = (int*) mesh->vertexPtr(tri.v[0]);
	  v0[i] = tri.v[0]; 
	  v1[i] = int(size_t((int*)mesh->vertexPtr(tri.v[1])-base)); 
	  v2[i] = int(size_t((int*)mesh->vertexPtr(tri.v[2])-base)); 
	  begin++;
	} else {
	  assert(i);
          if (i > 0) {
            geomID[i] = geomID[0];
            primID[i] = -1;
            v0[i] = 0;
            v1[i] = 0; 
            v2[i] = 0;
          }
	}
	if (begin<end) prim = &prims[begin];
      }
      
      new (this) TriangleMi(v0,v1,v2,geomID,primID); // FIXME: use non temporal store
    }
    
    /* Updates the primitive */
    __forceinline BBox3fa update(TriangleMesh* mesh)
    {
      BBox3fa bounds = empty;      
      for (size_t i=0; i<M; i++)
      {
        if (primID(i) == -1) break;
        const unsigned primId = primID(i);
        const TriangleMesh::Triangle& tri = mesh->triangle(primId);
        const Vec3fa p0 = mesh->vertex(tri.v[0]);
        const Vec3fa p1 = mesh->vertex(tri.v[1]);
        const Vec3fa p2 = mesh->vertex(tri.v[2]);
        bounds.extend(merge(BBox3fa(p0),BBox3fa(p1),BBox3fa(p2)));
      }
      return bounds;
    }
    
  public:
    vint<M> v0;         // index to 1st vertex
    vint<M> v1;         // offset to 2nd vertex
    vint<M> v2;         // offset to 3rd vertex
    vint<M> geomIDs;    // geometry ID of mesh
    vint<M> primIDs;    // primitive ID of primitive inside mesh
  };

  template<>
    __forceinline void TriangleMi<4>::gather(Vec3vf4& p0, Vec3vf4& p1, Vec3vf4& p2, const Scene* const scene) const
  {
    const bool samegeom = all(geomIDs == vint4(geomIDs[0]));
    if (likely(samegeom))
    {
      const TriangleMesh* mesh = scene->get<TriangleMesh>(geomIDs[0]);
      const int* base0 = (const int*) mesh->vertexPtr(v0[0]);
      const int* base1 = (const int*) mesh->vertexPtr(v0[1]);
      const int* base2 = (const int*) mesh->vertexPtr(v0[2]);
      const int* base3 = (const int*) mesh->vertexPtr(v0[3]);
      const vfloat4 a0 = vfloat4::loadu(base0      ), a1 = vfloat4::loadu(base1      ), a2 = vfloat4::loadu(base2      ), a3 = vfloat4::loadu(base3      );
      const vfloat4 b0 = vfloat4::loadu(base0+v1[0]), b1 = vfloat4::loadu(base1+v1[1]), b2 = vfloat4::loadu(base2+v1[2]), b3 = vfloat4::loadu(base3+v1[3]);
      const vfloat4 c0 = vfloat4::loadu(base0+v2[0]), c1 = vfloat4::loadu(base1+v2[1]), c2 = vfloat4::loadu(base2+v2[2]), c3 = vfloat4::loadu(base3+v2[3]);
      transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z);
      transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z);
      transpose(c0,c1,c2,c3,p2.x,p2.y,p2.z);
    }
    else
    {
      const int* base0 = (const int*) scene->get<TriangleMesh>(geomIDs[0])->vertexPtr(v0[0]);
      const int* base1 = (const int*) scene->get<TriangleMesh>(geomIDs[1])->vertexPtr(v0[1]);
      const int* base2 = (const int*) scene->get<TriangleMesh>(geomIDs[2])->vertexPtr(v0[2]);
      const int* base3 = (const int*) scene->get<TriangleMesh>(geomIDs[3])->vertexPtr(v0[3]);
      const vfloat4 a0 = vfloat4::loadu(base0      ), a1 = vfloat4::loadu(base1      ), a2 = vfloat4::loadu(base2      ), a3 = vfloat4::loadu(base3      );
      const vfloat4 b0 = vfloat4::loadu(base0+v1[0]), b1 = vfloat4::loadu(base1+v1[1]), b2 = vfloat4::loadu(base2+v1[2]), b3 = vfloat4::loadu(base3+v1[3]);
      const vfloat4 c0 = vfloat4::loadu(base0+v2[0]), c1 = vfloat4::loadu(base1+v2[1]), c2 = vfloat4::loadu(base2+v2[2]), c3 = vfloat4::loadu(base3+v2[3]);
      transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z);
      transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z);
      transpose(c0,c1,c2,c3,p2.x,p2.y,p2.z);
    }
  }

  template<int M>
  typename TriangleMi<M>::Type TriangleMi<M>::type;

  typedef TriangleMi<4> Triangle4i;
}
