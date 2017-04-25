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

#include "bbox.h"

namespace embree
{
  template<typename T>
    __forceinline std::pair<T,T> globalLinear(const std::pair<T,T> v, const BBox1f& dt)
  {
    const float rcp_dt_size = float(1.0f)/dt.size();
    const T g0 = lerp(v.first,v.second,-dt.lower*rcp_dt_size);
    const T g1 = lerp(v.first,v.second,(1.0f-dt.lower)*rcp_dt_size);
    return std::make_pair(g0,g1);
  }

  template<typename T>
  struct LBBox
  {
  public:
    __forceinline LBBox () {}

    __forceinline LBBox ( const LBBox& other ) { 
      bounds0 = other.bounds0; bounds1 = other.bounds1; 
    }
    __forceinline LBBox& operator= ( const LBBox& other ) { 
      bounds0 = other.bounds0; bounds1 = other.bounds1; return *this; 
    }

    __forceinline LBBox (EmptyTy) 
      : bounds0(EmptyTy()), bounds1(EmptyTy()) {}
    
    __forceinline explicit LBBox ( const BBox<T>& bounds) 
      : bounds0(bounds), bounds1(bounds) { }
    
    __forceinline LBBox ( const BBox<T>& bounds0, const BBox<T>& bounds1) 
      : bounds0(bounds0), bounds1(bounds1) { }

    LBBox ( const avector<BBox<T>>& bounds ) 
    {
      assert(bounds.size());
      BBox<T> b0 = bounds.front();
      BBox<T> b1 = bounds.back();
      for (size_t i=1; i<bounds.size()-1; i++) {
        const float f = float(i)/float(bounds.size()-1);
        const BBox<T> bt = lerp(b0,b1,f);
        const T dlower = min(bounds[i].lower-bt.lower,T(zero));
        const T dupper = max(bounds[i].upper-bt.upper,T(zero));
        b0.lower += dlower; b1.lower += dlower;
        b0.upper += dupper; b1.upper += dupper;
      }
      bounds0 = b0;
      bounds1 = b1;
    }

  public:

    __forceinline bool empty() const {
      return bounds().empty();
    }

    __forceinline BBox<T> bounds () const {
      return merge(bounds0,bounds1);
    }

    __forceinline BBox<T> interpolate( const float t ) const {
      return lerp(bounds0,bounds1,t);
    }

    __forceinline LBBox<T> interpolate( const BBox1f& dt ) const {
      return LBBox<T>(interpolate(dt.lower),interpolate(dt.upper));
    }

    __forceinline void extend( const LBBox& other ) {
      bounds0.extend(other.bounds0);
      bounds1.extend(other.bounds1);
    }

    __forceinline float expectedHalfArea() const;

    __forceinline float expectedHalfArea(const BBox1f& dt) const {
      return interpolate(dt).expectedHalfArea();
    }

    __forceinline float expectedApproxHalfArea() const {
      return 0.5f*(halfArea(bounds0) + halfArea(bounds1));
    }

    /* calculates bounds for [0,1] time range from bounds in dt time range */
    __forceinline LBBox global(const BBox1f& dt) const 
    {
      const float rcp_dt_size = 1.0f/dt.size();
      const BBox<T> b0 = interpolate(-dt.lower*rcp_dt_size);
      const BBox<T> b1 = interpolate((1.0f-dt.lower)*rcp_dt_size);
      return LBBox(b0,b1);
    }

    /*! Comparison Operators */
    template<typename TT> friend __forceinline bool operator==( const LBBox<TT>& a, const LBBox<TT>& b ) { return a.bounds0 == b.bounds0 && a.bounds1 == b.bounds1; }
    template<typename TT> friend __forceinline bool operator!=( const LBBox<TT>& a, const LBBox<TT>& b ) { return a.bounds0 != b.bounds0 || a.bounds1 != b.bounds1; }

    /*! output operator */
    friend __forceinline std::ostream& operator<<(std::ostream& cout, const LBBox& box) {
      return cout << "LBBox { " << box.bounds0 << "; " << box.bounds1 << " }";
    }

  public:
    BBox<T> bounds0, bounds1;
  };

  template<typename T>
    __forceinline T expectedArea(const T& a0, const T& a1, const T& b0, const T& b1)
  {
    const T da = a1-a0;
    const T db = b1-b0;
    return a0*b0+(a0*db+da*b0)*T(0.5f) + da*db*T(1.0f/3.0f);
  }
  
  template<> __forceinline float LBBox<Vec3fa>::expectedHalfArea() const 
  {
    const Vec3fa d0 = bounds0.size();
    const Vec3fa d1 = bounds1.size();
    return reduce_add(expectedArea(Vec3fa(d0.x,d0.y,d0.z),
                                   Vec3fa(d1.x,d1.y,d1.z),
                                   Vec3fa(d0.y,d0.z,d0.x),
                                   Vec3fa(d1.y,d1.z,d1.x)));
  }

  template<typename T>
  __forceinline float expectedApproxHalfArea(const LBBox<T>& box) {
    return box.expectedApproxHalfArea(); 
  }

  template<typename T>
  __forceinline LBBox<T> merge(const LBBox<T>& a, const LBBox<T>& b) {
    return LBBox<T>(merge(a.bounds0, b.bounds0), merge(a.bounds1, b.bounds1));
  }

  /*! default template instantiations */
  typedef LBBox<float> LBBox1f;
  typedef LBBox<Vec2f> LBBox2f;
  typedef LBBox<Vec3f> LBBox3f;
  typedef LBBox<Vec3fa> LBBox3fa;
}
