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
  /*! Quad Mesh */
  struct QuadMesh : public Geometry
  {
    /*! type of this geometry */
    static const Geometry::Type geom_type = Geometry::QUAD_MESH;
    
    /*! triangle indices */
    struct Quad
    {
      uint32_t v[4];

      /*! outputs triangle indices */
      __forceinline friend std::ostream &operator<<(std::ostream& cout, const Quad& q) {
        return cout << "Quad {" << q.v[0] << ", " << q.v[1] << ", " << q.v[2] << ", " << q.v[3] << " }";
      }
    };

  public:

    /*! quad mesh construction */
    QuadMesh (Scene* parent, RTCGeometryFlags flags, size_t numQuads, size_t numVertices, size_t numTimeSteps); 
  
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

    /*! returns number of quads */
    __forceinline size_t size() const {
      return quads.size();
    }

    /*! returns number of vertices */
    __forceinline size_t numVertices() const {
      return vertices[0].size();
    }
    
    /*! returns i'th quad */
    __forceinline const Quad& quad(size_t i) const {
      return quads[i];
    }

    /*! returns i'th vertex of itime'th timestep */
    __forceinline const Vec3fa vertex(size_t i) const {
      return vertices0[i];
    }

    /*! returns i'th vertex of itime'th timestep */
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

    /*! calculates the bounds of the i'th quad */
    __forceinline BBox3fa bounds(size_t i) const 
    {
      const Quad& q = quad(i);
      const Vec3fa v0 = vertex(q.v[0]);
      const Vec3fa v1 = vertex(q.v[1]);
      const Vec3fa v2 = vertex(q.v[2]);
      const Vec3fa v3 = vertex(q.v[3]);
      return BBox3fa(min(v0,v1,v2,v3),max(v0,v1,v2,v3));
    }

    /*! calculates the bounds of the i'th quad at the itime'th timestep */
    __forceinline BBox3fa bounds(size_t i, size_t itime) const
    {
      const Quad& q = quad(i);
      const Vec3fa v0 = vertex(q.v[0],itime);
      const Vec3fa v1 = vertex(q.v[1],itime);
      const Vec3fa v2 = vertex(q.v[2],itime);
      const Vec3fa v3 = vertex(q.v[3],itime);
      return BBox3fa(min(v0,v1,v2,v3),max(v0,v1,v2,v3));
    }

    /*! check if the i'th primitive is valid at the itime'th timestep */
    __forceinline bool valid(size_t i, size_t itime) const {
      return valid(i, make_range(itime, itime));
    }

    /*! check if the i'th primitive is valid between the specified time range */
    __forceinline bool valid(size_t i, const range<size_t>& itime_range) const
    {
      const Quad& q = quad(i);
      if (unlikely(q.v[0] >= numVertices())) return false;
      if (unlikely(q.v[1] >= numVertices())) return false;
      if (unlikely(q.v[2] >= numVertices())) return false;
      if (unlikely(q.v[3] >= numVertices())) return false;

      for (size_t itime = itime_range.begin(); itime <= itime_range.end(); itime++)
      {
        if (!isvalid(vertex(q.v[0],itime))) return false;
        if (!isvalid(vertex(q.v[1],itime))) return false;
        if (!isvalid(vertex(q.v[2],itime))) return false;
        if (!isvalid(vertex(q.v[3],itime))) return false;
      }

      return true;
    }

    /*! calculates the linear bounds of the i'th quad at the itimeGlobal'th time segment */
    __forceinline LBBox3fa linearBounds(size_t i, size_t itimeGlobal, size_t numTimeStepsGlobal) const {
      return Geometry::linearBounds([&] (size_t itime) { return bounds(i, itime); }, itimeGlobal, numTimeStepsGlobal, numTimeSteps);
    }

    /*! calculates the build bounds of the i'th primitive, if it's valid */
    __forceinline bool buildBounds(size_t i, BBox3fa* bbox = nullptr) const
    {
      const Quad& q = quad(i);
      if (q.v[0] >= numVertices()) return false;
      if (q.v[1] >= numVertices()) return false;
      if (q.v[2] >= numVertices()) return false;
      if (q.v[3] >= numVertices()) return false;

      for (size_t t=0; t<numTimeSteps; t++)
      {
        const Vec3fa v0 = vertex(q.v[0],t);
        const Vec3fa v1 = vertex(q.v[1],t);
        const Vec3fa v2 = vertex(q.v[2],t);
        const Vec3fa v3 = vertex(q.v[3],t);

        if (unlikely(!isvalid(v0) || !isvalid(v1) || !isvalid(v2) || !isvalid(v3)))
          return false;
      }

      if (bbox) 
        *bbox = bounds(i);

      return true;
    }

    /*! calculates the build bounds of the i'th primitive at the itime'th time segment, if it's valid */
    __forceinline bool buildBounds(size_t i, size_t itime, BBox3fa& bbox) const
    {
      const Quad& q = quad(i);
      if (unlikely(q.v[0] >= numVertices())) return false;
      if (unlikely(q.v[1] >= numVertices())) return false;
      if (unlikely(q.v[2] >= numVertices())) return false;
      if (unlikely(q.v[3] >= numVertices())) return false;

      assert(itime+1 < numTimeSteps);
      const Vec3fa a0 = vertex(q.v[0],itime+0); if (unlikely(!isvalid(a0))) return false;
      const Vec3fa a1 = vertex(q.v[1],itime+0); if (unlikely(!isvalid(a1))) return false;
      const Vec3fa a2 = vertex(q.v[2],itime+0); if (unlikely(!isvalid(a2))) return false;
      const Vec3fa a3 = vertex(q.v[3],itime+0); if (unlikely(!isvalid(a3))) return false;
      const Vec3fa b0 = vertex(q.v[0],itime+1); if (unlikely(!isvalid(b0))) return false;
      const Vec3fa b1 = vertex(q.v[1],itime+1); if (unlikely(!isvalid(b1))) return false;
      const Vec3fa b2 = vertex(q.v[2],itime+1); if (unlikely(!isvalid(b2))) return false;
      const Vec3fa b3 = vertex(q.v[3],itime+1); if (unlikely(!isvalid(b3))) return false;
      
      /* use bounds of first time step in builder */
      bbox = BBox3fa(min(a0,a1,a2,a3),max(a0,a1,a2,a3));
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
    APIBuffer<Quad> quads;                            //!< array of quads
    BufferRefT<Vec3fa> vertices0;                     //!< fast access to first vertex buffer
    vector<APIBuffer<Vec3fa>> vertices;               //!< vertex array for each timestep
    vector<APIBuffer<char>> userbuffers;              //!< user buffers
  };
}
