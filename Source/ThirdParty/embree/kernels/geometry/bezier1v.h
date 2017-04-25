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
#include "../subdiv/bezier_curve.h"
#include "../common/primref.h"

namespace embree
{
  struct BezierPrim // FIXME: rename to BezierRef, remove!!!
  {
  public:

    /*! Default constructor. */
    __forceinline BezierPrim () {}

    /*! Construction from vertices and IDs. */
    __forceinline BezierPrim (bool hair,
                              const Vec3fa& p0, const Vec3fa& p1, const Vec3fa& p2, const Vec3fa& p3, const int N,
                              const unsigned int geomID, const unsigned int primID)
      : p0(p0), p1(p1), p2(p2), p3(p3), N(N), geom(geomID), prim(primID), hair(hair) {}

    /*! access hidden members */
    __forceinline unsigned int primID() const { 
      return prim;
    }
    __forceinline unsigned int geomID() const { 
      return geom; 
    }

    /*! calculate the center of the curve */
    __forceinline const Vec3fa center2() const {
      return p0+p3;
    }

    /*! calculate the center of the curve */
    __forceinline const Vec3fa center2(const AffineSpace3fa& space) const {
      return xfmPoint(space,p0)+xfmPoint(space,p3);
    }

    /*! calculate the bounds of the curve */
    __forceinline const BBox3fa bounds() const 
    {
      const Curve3fa curve(p0,p1,p2,p3);
      if (likely(hair)) return curve.tessellatedBounds(N);
      else              return curve.accurateBounds();
    }

    /*! size for bin heuristic is 1 */
    __forceinline unsigned size() const { 
      return 1;
    }

    /*! returns bounds and centroid used for binning */
    __forceinline void binBoundsAndCenter(BBox3fa& bounds_o, Vec3fa& center_o) const 
    {
      bounds_o = bounds();
      center_o = embree::center2(bounds_o);
    }

    /*! calculate bounds in specified coordinate space */
    __forceinline const BBox3fa bounds(const AffineSpace3fa& space) const 
    {
      Vec3fa b0 = xfmPoint(space,p0); b0.w = p0.w;
      Vec3fa b1 = xfmPoint(space,p1); b1.w = p1.w;
      Vec3fa b2 = xfmPoint(space,p2); b2.w = p2.w;
      Vec3fa b3 = xfmPoint(space,p3); b3.w = p3.w;
      const Curve3fa curve(b0,b1,b2,b3);
      if (likely(hair)) return curve.tessellatedBounds(N);
      else              return curve.accurateBounds();
    }
    
    /*! returns bounds and centroid used for binning */
    __forceinline void binBoundsAndCenter(BBox3fa& bounds_o, Vec3fa& center_o, const AffineSpace3fa& space, void* user) const 
    {
      bounds_o = bounds(space);
      center_o = embree::center2(bounds_o);
    }

    __forceinline uint64_t id64() const {
      return (((uint64_t)prim) << 32) + (uint64_t)geom;
    }

    friend __forceinline bool operator<(const BezierPrim& p0, const BezierPrim& p1) {
      return p0.id64() < p1.id64();
    }

    friend std::ostream& operator<<(std::ostream& cout, const BezierPrim& b) 
    {
      return std::cout << "BezierPrim { " << std::endl << 
        " p0 = " << b.p0 << ", " << std::endl <<
        " p1 = " << b.p1 << ", " << std::endl <<
        " p2 = " << b.p2 << ", " << std::endl <<
        " p3 = " << b.p3 << ",  " << std::endl <<
        " N = " << b.N << ", " << std::endl <<
        " geomID = " << b.geomID() << ", primID = " << b.primID() << std::endl << 
      "}";
    }
    
  public:
    Vec3fa p0;            //!< 1st control point (x,y,z,r)
    Vec3fa p1;            //!< 2nd control point (x,y,z,r)
    Vec3fa p2;            //!< 3rd control point (x,y,z,r)
    Vec3fa p3;            //!< 4th control point (x,y,z,r)
    int N;                //!< tessellation rate
    unsigned geom;        //!< geometry ID
    unsigned prim;        //!< primitive ID
    int hair;             //!< 0=surface, 1=hair
  };

  struct Bezier1v
  {
    struct Type : public PrimitiveType 
    {
      Type ();
      size_t size(const char* This) const;
    };
    static Type type;

  public:

    /* Returns maximal number of stored primitives */
    static __forceinline size_t max_size() { return 1; }

    /* Returns required number of primitive blocks for N primitives */
    static __forceinline size_t blocks(size_t N) { return N; }

  public:

    /*! Default constructor. */
    __forceinline Bezier1v () {}

    /*! Construction from vertices and IDs. */
    __forceinline Bezier1v (const Vec3fa& p0, const Vec3fa& p1, const Vec3fa& p2, const Vec3fa& p3, const unsigned int geomID, const unsigned int primID)
      : p0(p0), p1(p1), p2(p2), p3(p3), geom(geomID), prim(primID) {}

    /*! return primitive ID */
    __forceinline unsigned int primID() const { 
      return prim;
    }

    /*! return geometry ID */
    __forceinline unsigned int geomID() const { 
      return geom; 
    }

    /*! fill triangle from triangle list */
    __forceinline void fill(const PrimRef* prims, size_t& i, size_t end, Scene* scene)
    {
      const PrimRef& prim = prims[i];
      i++;
      const unsigned geomID = prim.geomID();
      const unsigned primID = prim.primID();
      const NativeCurves* curves = scene->get<NativeCurves>(geomID);
      const unsigned id = curves->curve(primID);
      const Vec3fa& p0 = curves->vertex(id+0);
      const Vec3fa& p1 = curves->vertex(id+1);
      const Vec3fa& p2 = curves->vertex(id+2);
      const Vec3fa& p3 = curves->vertex(id+3);
      new (this) Bezier1v(p0,p1,p2,p3,geomID,primID);
    }

    /*! fill triangle from triangle list */
    __forceinline void fill(const BezierPrim* prims, size_t& i, size_t end, Scene* scene) 
    {
      new (this) Bezier1v(prims[i].p0,prims[i].p1,prims[i].p2,prims[i].p3,prims[i].geom,prims[i].prim);
      i++;
    }

    /*! calculate the center of the curve */
    __forceinline const Vec3fa center2() const {
      return p0+p3;
    }

    /*! calculate the center of the curve */
    __forceinline const Vec3fa center2(const AffineSpace3fa& space) const {
      return xfmPoint(space,p0)+xfmPoint(space,p3);
    }

    __forceinline uint64_t id64() const {
      return (((uint64_t)prim) << 32) + (uint64_t)geom;
    }

    friend __forceinline bool operator<(const Bezier1v& p0, const Bezier1v& p1) {
      return p0.id64() < p1.id64();
    }

    friend std::ostream& operator<<(std::ostream& cout, const Bezier1v& b) 
    {
      return std::cout << "Bezier1v { " << std::endl << 
        " p0 = " << b.p0 << ", " << std::endl <<
        " p1 = " << b.p1 << ", " << std::endl <<
        " p2 = " << b.p2 << ", " << std::endl <<
        " p3 = " << b.p3 << ",  " << std::endl <<
        " geomID = " << b.geomID() << ", primID = " << b.primID() << std::endl << 
      "}";
    }
    
  public:
    Vec3fa p0;            //!< 1st control point (x,y,z,r)
    Vec3fa p1;            //!< 2nd control point (x,y,z,r)
    Vec3fa p2;            //!< 3rd control point (x,y,z,r)
    Vec3fa p3;            //!< 4th control point (x,y,z,r)
    unsigned geom;      //!< geometry ID
    unsigned prim;      //!< primitive ID
  };
}
