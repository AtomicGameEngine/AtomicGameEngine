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

#include "default.h"
#include "geometry.h"
#include "buffer.h"

namespace embree
{
  /*! represents an array of line segments */
  struct LineSegments : public Geometry
  {
    /*! type of this geometry */
    static const Geometry::Type geom_type = Geometry::LINE_SEGMENTS;

  public:

    /*! line segments construction */
    LineSegments (Scene* parent, RTCGeometryFlags flags, size_t numPrimitives, size_t numVertices, size_t numTimeSteps);

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

    /*! returns number of line segments */
    __forceinline size_t size() const {
      return segments.size();
    }

    /*! returns the number of vertices */
    __forceinline size_t numVertices() const {
      return vertices[0].size();
    }

    /*! returns the i'th segment */
    __forceinline const unsigned int& segment(size_t i) const {
      return segments[i];
    }

     /*! returns i'th vertex of the first time step */
    __forceinline Vec3fa vertex(size_t i) const {
      return vertices0[i];
    }

    /*! returns i'th vertex of the first time step */
    __forceinline const char* vertexPtr(size_t i) const {
      return vertices0.getPtr(i);
    }

    /*! returns i'th radius of the first time step */
    __forceinline float radius(size_t i) const {
      return vertices0[i].w;
    }

    /*! returns i'th vertex of itime'th timestep */
    __forceinline Vec3fa vertex(size_t i, size_t itime) const {
      return vertices[itime][i];
    }

    /*! returns i'th vertex of itime'th timestep */
    __forceinline const char* vertexPtr(size_t i, size_t itime) const {
      return vertices[itime].getPtr(i);
    }

    /*! returns i'th radius of itime'th timestep */
    __forceinline float radius(size_t i, size_t itime) const {
      return vertices[itime][i].w;
    }

    /*! calculates bounding box of i'th line segment */
    __forceinline BBox3fa bounds(size_t i) const
    {
      const unsigned int index = segment(i);
      const float r0 = radius(index+0);
      const float r1 = radius(index+1);
      const Vec3fa v0 = vertex(index+0);
      const Vec3fa v1 = vertex(index+1);
      const BBox3fa b = merge(BBox3fa(v0),BBox3fa(v1));
      return enlarge(b,Vec3fa(max(r0,r1)));
    }

    /*! calculates bounding box of i'th line segment for the itime'th time step */
    __forceinline BBox3fa bounds(size_t i, size_t itime) const
    {
      const unsigned int index = segment(i);
      const float r0 = radius(index+0,itime);
      const float r1 = radius(index+1,itime);
      const Vec3fa v0 = vertex(index+0,itime);
      const Vec3fa v1 = vertex(index+1,itime);
      const BBox3fa b = merge(BBox3fa(v0),BBox3fa(v1));
      return enlarge(b,Vec3fa(max(r0,r1)));
    }

    /*! check if the i'th primitive is valid at the itime'th timestep */
    __forceinline bool valid(size_t i, size_t itime) const {
      return valid(i, make_range(itime, itime));
    }

    /*! check if the i'th primitive is valid between the specified time range */
    __forceinline bool valid(size_t i, const range<size_t>& itime_range) const
    {
      const unsigned int index = segment(i);
      if (index+1 >= numVertices()) return false;
      
      for (size_t itime = itime_range.begin(); itime <= itime_range.end(); itime++)
      {
        const Vec3fa v0 = vertex(index+0,itime); if (unlikely(!isvalid((vfloat4)v0))) return false;
        const Vec3fa v1 = vertex(index+1,itime); if (unlikely(!isvalid((vfloat4)v1))) return false;
        if (min(v0.w,v1.w) < 0.0f) return false;
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
    __forceinline bool buildBounds(size_t i, BBox3fa* bbox) const
    {
      if (!valid(i,0)) return false;
      *bbox = bounds(i); 
      return true;
    }

    /*! calculates the build bounds of the i'th primitive at the itime'th time segment, if it's valid */
    __forceinline bool buildBounds(size_t i, size_t itime, BBox3fa& bbox) const
    {
      if (!valid(i,itime+0) || !valid(i,itime+1)) return false;
      bbox = bounds(i,itime);  // use bounds of first time step in builder
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
    APIBuffer<unsigned int> segments;                 //!< array of line segment indices
    BufferRefT<Vec3fa> vertices0;                     //!< fast access to first vertex buffer
    vector<APIBuffer<Vec3fa>> vertices;               //!< vertex array for each timestep
    vector<APIBuffer<char>> userbuffers;              //!< user buffers
  };
}
