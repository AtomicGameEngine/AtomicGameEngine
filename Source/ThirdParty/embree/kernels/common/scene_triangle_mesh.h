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

#include "geometry.h"
#include "buffer.h"

namespace embree
{
  /*! Triangle Mesh */
  struct TriangleMesh : public Geometry
  {
    /*! type of this geometry */
    static const Geometry::Type geom_type = Geometry::TRIANGLE_MESH;

    /*! triangle indices */
    struct Triangle 
    {
      uint32_t v[3];

      /*! outputs triangle indices */
      __forceinline friend std::ostream &operator<<(std::ostream& cout, const Triangle& t) {
        return cout << "Triangle { " << t.v[0] << ", " << t.v[1] << ", " << t.v[2] << " }";
      }
    };

    /*! triangle edge based on two indices */
    struct Edge 
    {
      uint64_t e;

      __forceinline Edge() {}

      __forceinline Edge(const uint32_t& v0, const uint32_t& v1) {
        e = v0 < v1 ? (((uint64_t)v1 << 32) | (uint64_t)v0) : (((uint64_t)v0 << 32) | (uint64_t)v1);
      }

      __forceinline friend bool operator==( const Edge& a, const Edge& b ) { 
        return a.e == b.e; 
      }
    };

    /* last edge of triangle 0 is shared */
    static __forceinline unsigned int pair_order(const unsigned int tri0_vtx_index0,
                                                 const unsigned int tri0_vtx_index1,
                                                 const unsigned int tri0_vtx_index2,
                                                 const unsigned int tri1_vtx_index)
    {
      return \
        (tri0_vtx_index0  <<  0) |
        (tri0_vtx_index1  <<  8) |
        (tri0_vtx_index2  << 16) |
        (tri1_vtx_index   << 24);
    }

    /*! tests if a shared exists between two triangles, returns -1 if no shared edge exists and the opposite vertex index of the second triangle if a shared edge exists */
    static __forceinline int sharedEdge(const Triangle &tri0,
                                        const Triangle &tri1)
    {
      const Edge tri0_edge0(tri0.v[0],tri0.v[1]);
      const Edge tri0_edge1(tri0.v[1],tri0.v[2]);
      const Edge tri0_edge2(tri0.v[2],tri0.v[0]);

      const Edge tri1_edge0(tri1.v[0],tri1.v[1]);
      const Edge tri1_edge1(tri1.v[1],tri1.v[2]);
      const Edge tri1_edge2(tri1.v[2],tri1.v[0]);

      /* rotate triangle 0 to force shared edge between the first and last vertex */

      if (unlikely(tri0_edge0 == tri1_edge0)) return pair_order(1,2,0, 2); 
      if (unlikely(tri0_edge1 == tri1_edge0)) return pair_order(2,0,1, 2); 
      if (unlikely(tri0_edge2 == tri1_edge0)) return pair_order(0,1,2, 2);

      if (unlikely(tri0_edge0 == tri1_edge1)) return pair_order(1,2,0, 0); 
      if (unlikely(tri0_edge1 == tri1_edge1)) return pair_order(2,0,1, 0); 
      if (unlikely(tri0_edge2 == tri1_edge1)) return pair_order(0,1,2, 0); 

      if (unlikely(tri0_edge0 == tri1_edge2)) return pair_order(1,2,0, 1); 
      if (unlikely(tri0_edge1 == tri1_edge2)) return pair_order(2,0,1, 1); 
      if (unlikely(tri0_edge2 == tri1_edge2)) return pair_order(0,1,2, 1); 

      return -1;
    }

  public:

    /*! triangle mesh construction */
    TriangleMesh (Scene* parent, RTCGeometryFlags flags, size_t numTriangles, size_t numVertices, size_t numTimeSteps); 

    /* geometry interface */
  public:
    void enabling();
    void disabling();
    void setMask (unsigned mask);
    void setBuffer(RTCBufferType type, void* ptr, size_t offset, size_t stride, size_t size);
    void* map(RTCBufferType type);
    void unmap(RTCBufferType type);
    void immutable ();
    bool verify ();
    void interpolate(unsigned primID, float u, float v, RTCBufferType buffer, float* P, float* dPdu, float* dPdv, float* ddPdudu, float* ddPdvdv, float* ddPdudv, size_t numFloats);
    // FIXME: implement interpolateN

  public:

    /*! returns number of triangles */
    __forceinline size_t size() const {
      return triangles.size();
    }

    /*! returns number of vertices */
    __forceinline size_t numVertices() const {
      return vertices[0].size();
    }
    
    /*! returns i'th triangle*/
    __forceinline const Triangle& triangle(size_t i) const {
      return triangles[i];
    }

    /*! returns i'th vertex of the first time step  */
    __forceinline const Vec3fa vertex(size_t i) const {
      return vertices0[i];
    }

    /*! returns i'th vertex of the first time step */
    __forceinline const char* vertexPtr(size_t i) const {
      return vertices0.getPtr(i);
    }

    /*! returns i'th vertex of itime'th timestep */
    __forceinline const Vec3fa vertex(size_t i, size_t itime) const {
      return vertices[itime][i];
    }

    /*! returns i'th vertex of itime'th timestep */
    __forceinline const char* vertexPtr(size_t i, size_t itime) const {
      return vertices[itime].getPtr(i);
    }

    /*! calculates the bounds of the i'th triangle */
    __forceinline BBox3fa bounds(size_t i) const 
    {
      const Triangle& tri = triangle(i);
      const Vec3fa v0 = vertex(tri.v[0]);
      const Vec3fa v1 = vertex(tri.v[1]);
      const Vec3fa v2 = vertex(tri.v[2]);
      return BBox3fa(min(v0,v1,v2),max(v0,v1,v2));
    }

    /*! calculates the bounds of the i'th triangle at the itime'th timestep */
    __forceinline BBox3fa bounds(size_t i, size_t itime) const
    {
      const Triangle& tri = triangle(i);
      const Vec3fa v0 = vertex(tri.v[0],itime);
      const Vec3fa v1 = vertex(tri.v[1],itime);
      const Vec3fa v2 = vertex(tri.v[2],itime);
      return BBox3fa(min(v0,v1,v2),max(v0,v1,v2));
    }

    /*! calculates the interpolated bounds of the i'th triangle at the specified time */
    __forceinline BBox3fa bounds(size_t i, float time) const
    {
      float ftime; size_t itime = getTimeSegment(time, fnumTimeSegments, ftime);
      const BBox3fa b0 = bounds(i, itime+0);
      const BBox3fa b1 = bounds(i, itime+1);
      return lerp(b0, b1, ftime);
    }

    /*! check if the i'th primitive is valid at the itime'th timestep */
    __forceinline bool valid(size_t i, size_t itime) const {
      return valid(i, make_range(itime, itime));
    }

    /*! check if the i'th primitive is valid between the specified time range */
    __forceinline bool valid(size_t i, const range<size_t>& itime_range) const
    {
      const Triangle& tri = triangle(i);
      if (unlikely(tri.v[0] >= numVertices())) return false;
      if (unlikely(tri.v[1] >= numVertices())) return false;
      if (unlikely(tri.v[2] >= numVertices())) return false;

      for (size_t itime = itime_range.begin(); itime <= itime_range.end(); itime++)
      {
        if (!isvalid(vertex(tri.v[0],itime))) return false;
        if (!isvalid(vertex(tri.v[1],itime))) return false;
        if (!isvalid(vertex(tri.v[2],itime))) return false;
      }

      return true;
    }

    /*! calculates the linear bounds of the i'th primitive at the itimeGlobal'th time segment */
    __forceinline LBBox3fa linearBounds(size_t i, size_t itimeGlobal, size_t numTimeStepsGlobal) const
    {
      return Geometry::linearBounds([&] (size_t itime) { return bounds(i, itime); },
                                    itimeGlobal, numTimeStepsGlobal, numTimeSteps);
    }

    /*! calculates the build bounds of the i'th primitive, if it's valid */
    __forceinline bool buildBounds(size_t i, BBox3fa* bbox = nullptr) const
    {
      const Triangle& tri = triangle(i);
      if (unlikely(tri.v[0] >= numVertices())) return false;
      if (unlikely(tri.v[1] >= numVertices())) return false;
      if (unlikely(tri.v[2] >= numVertices())) return false;

      for (size_t t=0; t<numTimeSteps; t++)
      {
        const Vec3fa v0 = vertex(tri.v[0],t);
        const Vec3fa v1 = vertex(tri.v[1],t);
        const Vec3fa v2 = vertex(tri.v[2],t);
        if (unlikely(!isvalid(v0) || !isvalid(v1) || !isvalid(v2)))
          return false;
      }

      if (likely(bbox)) 
        *bbox = bounds(i);

      return true;
    }

    /*! calculates the build bounds of the i'th primitive at the itime'th time segment, if it's valid */
    __forceinline bool buildBounds(size_t i, size_t itime, BBox3fa& bbox) const
    {
      const Triangle& tri = triangle(i);
      if (unlikely(tri.v[0] >= numVertices())) return false;
      if (unlikely(tri.v[1] >= numVertices())) return false;
      if (unlikely(tri.v[2] >= numVertices())) return false;

      assert(itime+1 < numTimeSteps);
      const Vec3fa a0 = vertex(tri.v[0],itime+0); if (unlikely(!isvalid(a0))) return false;
      const Vec3fa a1 = vertex(tri.v[1],itime+0); if (unlikely(!isvalid(a1))) return false;
      const Vec3fa a2 = vertex(tri.v[2],itime+0); if (unlikely(!isvalid(a2))) return false;
      const Vec3fa b0 = vertex(tri.v[0],itime+1); if (unlikely(!isvalid(b0))) return false;
      const Vec3fa b1 = vertex(tri.v[1],itime+1); if (unlikely(!isvalid(b1))) return false;
      const Vec3fa b2 = vertex(tri.v[2],itime+1); if (unlikely(!isvalid(b2))) return false;
      
      /* use bounds of first time step in builder */
      bbox = BBox3fa(min(a0,a1,a2),max(a0,a1,a2));
      return true;
    }

    /*! calculates the build bounds of the i'th primitive at the itimeGlobal'th time segment, if it's valid */
    __forceinline bool buildBounds(size_t i, size_t itimeGlobal, size_t numTimeStepsGlobal, BBox3fa& bbox) const
    {
      return Geometry::buildBounds([&] (size_t itime, BBox3fa& bbox) -> bool
                                   {
                                     if (unlikely(!valid(i, itime))) return false;
                                     bbox = bounds(i, itime);
                                     return true;
                                   },
                                   itimeGlobal, numTimeStepsGlobal, numTimeSteps, bbox);
    }
    
  public:
    APIBuffer<Triangle> triangles;                    //!< array of triangles
    BufferRefT<Vec3fa> vertices0;                     //!< fast access to first vertex buffer
    vector<APIBuffer<Vec3fa>> vertices;               //!< vertex array for each timestep
    vector<APIBuffer<char>> userbuffers;         //!< user buffers
  };
}
