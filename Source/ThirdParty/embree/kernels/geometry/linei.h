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
  template <int M>
  struct LineMi
  {
    typedef Vec3<vfloat<M>> Vec3vfM;

    /* Virtual interface to query information about the line segment type */
    struct Type : public PrimitiveType
    {
      Type();
      size_t size(const char* This) const;
    };
    static Type type;

  public:

    /* Returns maximal number of stored line segments */
    static __forceinline size_t max_size() { return M; }

    /* Returns required number of primitive blocks for N line segments */
    static __forceinline size_t blocks(size_t N) { return (N+max_size()-1)/max_size(); }

  public:

    /* Default constructor */
    __forceinline LineMi() {  }

    /* Construction from vertices and IDs */
    __forceinline LineMi(const vint<M>& v0, const vint<M>& geomIDs, const vint<M>& primIDs)
      : v0(v0), geomIDs(geomIDs), primIDs(primIDs) {}

    /* Returns a mask that tells which line segments are valid */
    __forceinline vbool<M> valid() const { return primIDs != vint<M>(-1); }

      /* Returns a mask that tells which line segments are valid */
    template<int Mx>
    __forceinline vbool<Mx> valid() const { return vint<Mx>(primIDs) != vint<Mx>(-1); }

    /* Returns if the specified line segment is valid */
    __forceinline bool valid(const size_t i) const { assert(i<M); return primIDs[i] != -1; }

    /* Returns the number of stored line segments */
    __forceinline size_t size() const { return __bsf(~movemask(valid())); }

    /* Returns the geometry IDs */
    __forceinline vint<M> geomID() const { return geomIDs; }
    __forceinline int geomID(const size_t i) const { assert(i<M); return geomIDs[i]; }

    /* Returns the primitive IDs */
    __forceinline vint<M> primID() const { return primIDs; }
    __forceinline int primID(const size_t i) const { assert(i<M); return primIDs[i]; }

    /* gather the line segments */
    __forceinline void gather(Vec4<vfloat<M>>& p0,
                              Vec4<vfloat<M>>& p1,
                              const Scene* scene) const;

    __forceinline void gather(Vec4<vfloat<M>>& p0,
                              Vec4<vfloat<M>>& p1,
                              const LineSegments* geom0,
                              const LineSegments* geom1,
                              const LineSegments* geom2,
                              const LineSegments* geom3,
                              const vint<M>& itime) const;

    __forceinline void gather(Vec4<vfloat<M>>& p0,
                              Vec4<vfloat<M>>& p1,
                              const Scene* scene,
                              float time) const;

    /* Calculate the bounds of the line segments */
    __forceinline const BBox3fa bounds(const Scene* scene, size_t itime = 0) const
    {
      BBox3fa bounds = empty;
      for (size_t i=0; i<M && valid(i); i++)
      {
        const LineSegments* geom = scene->get<LineSegments>(geomID(i));
        const Vec3fa& p0 = geom->vertex(v0[i]+0,itime);
        const Vec3fa& p1 = geom->vertex(v0[i]+1,itime);
        BBox3fa b = merge(BBox3fa(p0),BBox3fa(p1));
        b = enlarge(b,Vec3fa(max(p0.w,p1.w)));
        bounds.extend(b);
      }
      return bounds;
    }

    /* Calculate the linear bounds of the primitive */
    __forceinline LBBox3fa linearBounds(const Scene* scene, size_t itime) {
      return LBBox3fa(bounds(scene,itime+0), bounds(scene,itime+1));
    }

    __forceinline LBBox3fa linearBounds(const Scene *const scene, size_t itime, size_t numTimeSteps) {
      LBBox3fa allBounds = empty;
      for (size_t i=0; i<M && valid(i); i++)
      {
        const LineSegments* geom = scene->get<LineSegments>(geomID(i));
        allBounds.extend(geom->linearBounds(primID(i), itime, numTimeSteps));
      }
      return allBounds;
    }

    /* Fill line segment from line segment list */
    __forceinline void fill(const PrimRef* prims, size_t& begin, size_t end, Scene* scene)
    {
      vint<M> geomID, primID;
      vint<M> v0;
      const PrimRef* prim = &prims[begin];

      for (size_t i=0; i<M; i++)
      {
        const LineSegments* geom = scene->get<LineSegments>(prim->geomID());
        if (begin<end) {
          geomID[i] = prim->geomID();
          primID[i] = prim->primID();
          v0[i] = geom->segment(prim->primID());
          begin++;
        } else {
          assert(i);
          if (i>0) {
            geomID[i] = geomID[i-1];
            primID[i] = -1;
            v0[i] = v0[i-1];
          }
        }
        if (begin<end) prim = &prims[begin];
      }

      new (this) LineMi(v0,geomID,primID); // FIXME: use non temporal store
    }

    /* Fill line segment from line segment list */
    __forceinline LBBox3fa fillMB(const PrimRef* prims, size_t& begin, size_t end, Scene* scene, size_t itime, size_t numTimeSteps)
    {
      fill(prims,begin,end,scene);
      return linearBounds(scene,itime,numTimeSteps);
    }

    /* Updates the primitive */
    __forceinline BBox3fa update(LineSegments* geom)
    {
      BBox3fa bounds = empty;
      for (size_t i=0; i<M && valid(i); i++)
      {
        const Vec3fa& p0 = geom->vertex(v0[i]+0);
        const Vec3fa& p1 = geom->vertex(v0[i]+1);
        BBox3fa b = merge(BBox3fa(p0),BBox3fa(p1));
        b = enlarge(b,Vec3fa(max(p0.w,p1.w)));
        bounds.extend(b);
      }
      return bounds;
    }

    /*! output operator */
    friend __forceinline std::ostream& operator<<(std::ostream& cout, const LineMi& line) {
      return cout << "Line" << M << "i {" << line.v0 << ", " << line.geomIDs << ", " << line.primIDs << "}";
    }
    
  public:
    vint<M> v0;      // index of start vertex
    vint<M> geomIDs; // geometry ID
    vint<M> primIDs; // primitive ID
  };

  template<>
    __forceinline void LineMi<4>::gather(Vec4vf4& p0,
                                         Vec4vf4& p1,
                                         const Scene* scene) const
  {
    const LineSegments* geom0 = scene->get<LineSegments>(geomIDs[0]);
    const LineSegments* geom1 = scene->get<LineSegments>(geomIDs[1]);
    const LineSegments* geom2 = scene->get<LineSegments>(geomIDs[2]);
    const LineSegments* geom3 = scene->get<LineSegments>(geomIDs[3]);

    const vfloat4 a0 = vfloat4::loadu(geom0->vertexPtr(v0[0]));
    const vfloat4 a1 = vfloat4::loadu(geom1->vertexPtr(v0[1]));
    const vfloat4 a2 = vfloat4::loadu(geom2->vertexPtr(v0[2]));
    const vfloat4 a3 = vfloat4::loadu(geom3->vertexPtr(v0[3]));

    transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z,p0.w);

    const vfloat4 b0 = vfloat4::loadu(geom0->vertexPtr(v0[0]+1));
    const vfloat4 b1 = vfloat4::loadu(geom1->vertexPtr(v0[1]+1));
    const vfloat4 b2 = vfloat4::loadu(geom2->vertexPtr(v0[2]+1));
    const vfloat4 b3 = vfloat4::loadu(geom3->vertexPtr(v0[3]+1));

    transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z,p1.w);
  }

  template<>
  __forceinline void LineMi<4>::gather(Vec4vf4& p0,
                                       Vec4vf4& p1,
                                       const LineSegments* geom0,
                                       const LineSegments* geom1,
                                       const LineSegments* geom2,
                                       const LineSegments* geom3,
                                       const vint4& itime) const
  {
    const vfloat4 a0 = vfloat4::loadu(geom0->vertexPtr(v0[0],itime[0]));
    const vfloat4 a1 = vfloat4::loadu(geom1->vertexPtr(v0[1],itime[1]));
    const vfloat4 a2 = vfloat4::loadu(geom2->vertexPtr(v0[2],itime[2]));
    const vfloat4 a3 = vfloat4::loadu(geom3->vertexPtr(v0[3],itime[3]));

    transpose(a0,a1,a2,a3,p0.x,p0.y,p0.z,p0.w);

    const vfloat4 b0 = vfloat4::loadu(geom0->vertexPtr(v0[0]+1,itime[0]));
    const vfloat4 b1 = vfloat4::loadu(geom1->vertexPtr(v0[1]+1,itime[1]));
    const vfloat4 b2 = vfloat4::loadu(geom2->vertexPtr(v0[2]+1,itime[2]));
    const vfloat4 b3 = vfloat4::loadu(geom3->vertexPtr(v0[3]+1,itime[3]));

    transpose(b0,b1,b2,b3,p1.x,p1.y,p1.z,p1.w);
  }

  template<>
    __forceinline void LineMi<4>::gather(Vec4vf4& p0,
                                         Vec4vf4& p1,
                                         const Scene* scene,
                                         float time) const
  {
    const LineSegments* geom0 = scene->get<LineSegments>(geomIDs[0]);
    const LineSegments* geom1 = scene->get<LineSegments>(geomIDs[1]);
    const LineSegments* geom2 = scene->get<LineSegments>(geomIDs[2]);
    const LineSegments* geom3 = scene->get<LineSegments>(geomIDs[3]);

    const vfloat4 numTimeSegments(geom0->fnumTimeSegments, geom1->fnumTimeSegments, geom2->fnumTimeSegments, geom3->fnumTimeSegments);
    vfloat4 ftime;
    const vint4 itime = getTimeSegment(vfloat4(time), numTimeSegments, ftime);

    Vec4vf4 a0,a1;
    gather(a0,a1,geom0,geom1,geom2,geom3,itime);
    Vec4vf4 b0,b1;
    gather(b0,b1,geom0,geom1,geom2,geom3,itime+1);
    p0 = lerp(a0,b0,ftime);
    p1 = lerp(a1,b1,ftime);
  }

  template<int M>
  typename LineMi<M>::Type LineMi<M>::type;

  typedef LineMi<4> Line4i;
}
