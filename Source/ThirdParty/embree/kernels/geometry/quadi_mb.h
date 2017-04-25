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
  /* Stores M motion blur quads from an indexed face set */
  template <int M>
  struct QuadMiMB
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
    __forceinline QuadMiMB() {  }

    /* Construction from vertices and IDs */
    __forceinline QuadMiMB(const vint<M>& v0, 
                           const vint<M>& v1, 
                           const vint<M>& v2, 
                           const vint<M>& v3, 
                           const vint<M>& geomIDs, 
                           const vint<M>& primIDs)
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
      return *(Vec3fa*)mesh->vertexPtr(v[index]);
    }

    template<typename T>
      __forceinline Vec3<T> getVertex(const vint<M> &v, const size_t index, const Scene *const scene, const size_t itime, const T& ftime) const
    {
      const QuadMesh* mesh = scene->get<QuadMesh>(geomID(index));
      const Vec3fa v0 = *(Vec3fa*)mesh->vertexPtr(v[index],itime+0);
      const Vec3fa v1 = *(Vec3fa*)mesh->vertexPtr(v[index],itime+1);
      const Vec3<T> p0(v0.x,v0.y,v0.z);
      const Vec3<T> p1(v1.x,v1.y,v1.z);
      return lerp(p0,p1,ftime);
    }

    /* gather the quads */
    template<int K>
      __forceinline void gather(const vbool<K>& valid,
                                Vec3<vfloat<K>>& p0,
                                Vec3<vfloat<K>>& p1,
                                Vec3<vfloat<K>>& p2,
                                Vec3<vfloat<K>>& p3,
                                const size_t index,
                                const Scene* const scene,
                                const vfloat<K>& time) const
    {
      const QuadMesh* mesh = scene->get<QuadMesh>(geomID(index));

      vfloat<K> ftime;
      const vint<K> itime = getTimeSegment(time, vfloat<K>(mesh->fnumTimeSegments), ftime);

      const size_t first = __bsf(movemask(valid)); // assume itime is uniform
      p0 = getVertex(v0, index, scene, itime[first], ftime);
      p1 = getVertex(v1, index, scene, itime[first], ftime);
      p2 = getVertex(v2, index, scene, itime[first], ftime);
      p3 = getVertex(v3, index, scene, itime[first], ftime);
    }

    __forceinline void gather(Vec3<vfloat<M>>& p0, 
                              Vec3<vfloat<M>>& p1, 
                              Vec3<vfloat<M>>& p2, 
                              Vec3<vfloat<M>>& p3,
                              const QuadMesh* mesh0,
                              const QuadMesh* mesh1,
                              const QuadMesh* mesh2,
                              const QuadMesh* mesh3,
                              const vint<M>& itime) const;

    __forceinline void gather(Vec3<vfloat<M>>& p0, 
                              Vec3<vfloat<M>>& p1, 
                              Vec3<vfloat<M>>& p2, 
                              Vec3<vfloat<M>>& p3,
                              const Scene *const scene,
                              const float time) const;
    
    /* Calculate the bounds of the quads */
    __forceinline const BBox3fa bounds(const Scene *const scene, const size_t itime=0) const
    {
      BBox3fa bounds = empty;
      for (size_t i=0; i<M && valid(i); i++)
      {
	const QuadMesh* mesh = scene->get<QuadMesh>(geomID(i));
        const Vec3fa &p0 = mesh->vertex(v0[i],itime);
        const Vec3fa &p1 = mesh->vertex(v1[i],itime);
        const Vec3fa &p2 = mesh->vertex(v2[i],itime);
        const Vec3fa &p3 = mesh->vertex(v3[i],itime);
	bounds.extend(p0);
	bounds.extend(p1);
	bounds.extend(p2);
	bounds.extend(p3);
      }
      return bounds;
    }
    
    /* Calculate the linear bounds of the primitive */
    __forceinline LBBox3fa linearBounds(const Scene* const scene, const size_t itime) {
      return LBBox3fa(bounds(scene,itime+0),bounds(scene,itime+1));
    }

    __forceinline LBBox3fa linearBounds(const Scene *const scene, size_t itime, size_t numTimeSteps) {
      LBBox3fa allBounds = empty;
      for (size_t i=0; i<M && valid(i); i++)
      {
        const QuadMesh* mesh = scene->get<QuadMesh>(geomID(i));
        allBounds.extend(mesh->linearBounds(primID(i), itime, numTimeSteps));
      }
      return allBounds;
    }
    
    /* Fill quad from quad list */
    __forceinline LBBox3fa fillMB(const PrimRef* prims, size_t& begin, size_t end, Scene* scene, size_t itime, size_t numTimeSteps)
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
      
      new (this) QuadMiMB(v0,v1,v2,v3,geomID,primID); // FIXME: use non temporal store
      return linearBounds(scene,itime,numTimeSteps);
    }

    friend std::ostream& operator<<(std::ostream& cout, const QuadMiMB& quad) {
      return cout << "QuadMiMB<" << M << ">( v0 = " << quad.v0 << ", v1 = " << quad.v1 << ", v2 = " << quad.v2 << ", v3 = " << quad.v3 << ", geomID = " << quad.geomIDs << ", primID = " << quad.primIDs << " )";
    }

  public:
    vint<M> v0;         // index of 1st vertex
    vint<M> v1;         // index of 2nd vertex
    vint<M> v2;         // index of 3rd vertex
    vint<M> v3;         // index of 4th vertex
    vint<M> geomIDs;    // geometry ID of mesh
    vint<M> primIDs;    // primitive ID of primitive inside mesh
  };

  template<>
    __forceinline void QuadMiMB<4>::gather(Vec3vf4& p0, 
                                           Vec3vf4& p1, 
                                           Vec3vf4& p2, 
                                           Vec3vf4& p3,
                                           const QuadMesh* mesh0,
                                           const QuadMesh* mesh1,
                                           const QuadMesh* mesh2,
                                           const QuadMesh* mesh3,
                                           const vint4& itime) const
  {
    const vfloat4 a0 = vfloat4::loadu(mesh0->vertexPtr(v0[0],itime[0]));
    const vfloat4 a1 = vfloat4::loadu(mesh1->vertexPtr(v0[1],itime[1]));
    const vfloat4 a2 = vfloat4::loadu(mesh2->vertexPtr(v0[2],itime[2]));
    const vfloat4 a3 = vfloat4::loadu(mesh3->vertexPtr(v0[3],itime[3]));

    transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z);

    const vfloat4 b0 = vfloat4::loadu(mesh0->vertexPtr(v1[0],itime[0]));
    const vfloat4 b1 = vfloat4::loadu(mesh1->vertexPtr(v1[1],itime[1]));
    const vfloat4 b2 = vfloat4::loadu(mesh2->vertexPtr(v1[2],itime[2]));
    const vfloat4 b3 = vfloat4::loadu(mesh3->vertexPtr(v1[3],itime[3]));

    transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z);

    const vfloat4 c0 = vfloat4::loadu(mesh0->vertexPtr(v2[0],itime[0]));
    const vfloat4 c1 = vfloat4::loadu(mesh1->vertexPtr(v2[1],itime[1]));
    const vfloat4 c2 = vfloat4::loadu(mesh2->vertexPtr(v2[2],itime[2]));
    const vfloat4 c3 = vfloat4::loadu(mesh3->vertexPtr(v2[3],itime[3]));

    transpose(c0,c1,c2,c3,p2.x,p2.y,p2.z);

    const vfloat4 d0 = vfloat4::loadu(mesh0->vertexPtr(v3[0],itime[0]));
    const vfloat4 d1 = vfloat4::loadu(mesh1->vertexPtr(v3[1],itime[1]));
    const vfloat4 d2 = vfloat4::loadu(mesh2->vertexPtr(v3[2],itime[2]));
    const vfloat4 d3 = vfloat4::loadu(mesh3->vertexPtr(v3[3],itime[3]));

    transpose(d0,d1,d2,d3,p3.x,p3.y,p3.z);
  }

  template<>
    __forceinline void QuadMiMB<4>::gather(Vec3vf4& p0, 
                                           Vec3vf4& p1, 
                                           Vec3vf4& p2, 
                                           Vec3vf4& p3,
                                           const Scene *const scene,
                                           const float time) const
  {
    const QuadMesh* mesh0 = scene->get<QuadMesh>(geomIDs[0]);
    const QuadMesh* mesh1 = scene->get<QuadMesh>(geomIDs[1]);
    const QuadMesh* mesh2 = scene->get<QuadMesh>(geomIDs[2]);
    const QuadMesh* mesh3 = scene->get<QuadMesh>(geomIDs[3]);

    const vfloat4 numTimeSegments(mesh0->fnumTimeSegments, mesh1->fnumTimeSegments, mesh2->fnumTimeSegments, mesh3->fnumTimeSegments);
    vfloat4 ftime;
    const vint4 itime = getTimeSegment(vfloat4(time), numTimeSegments, ftime);

    Vec3vf4 a0,a1,a2,a3;
    gather(a0,a1,a2,a3,mesh0,mesh1,mesh2,mesh3,itime);
    Vec3vf4 b0,b1,b2,b3;
    gather(b0,b1,b2,b3,mesh0,mesh1,mesh2,mesh3,itime+1);
    p0 = lerp(a0,b0,ftime);
    p1 = lerp(a1,b1,ftime);
    p2 = lerp(a2,b2,ftime);
    p3 = lerp(a3,b3,ftime);
  }

  template<int M>
  typename QuadMiMB<M>::Type QuadMiMB<M>::type;

  typedef QuadMiMB<4> Quad4iMB;
}
