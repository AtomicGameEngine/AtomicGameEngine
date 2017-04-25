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

namespace embree
{
  /* Hit structure for K hits */
  template<int K>
  struct HitK
  {
    /* Default construction does nothing */
    __forceinline HitK() {}

    /* Constructs a hit */
    __forceinline HitK(const vint<K>& instID, const vint<K>& geomID, const vint<K>& primID, const vfloat<K>& u, const vfloat<K>& v, const vfloat<K>& t, const Vec3<vfloat<K>>& Ng)
      : Ng(Ng), instID(instID), geomID(geomID), primID(primID), u(u), v(v), t(t) {}

    /* Returns the size of the hit */
    static __forceinline size_t size() { return K; }

  public:
    Vec3<vfloat<K>> Ng;  // geometry normal
    vint<K> instID;      // instance ID
    vint<K> geomID;      // geometry ID
    vint<K> primID;      // primitive ID
    vfloat<K> u;         // barycentric u coordinate of hit
    vfloat<K> v;         // barycentric v coordinate of hit
    vfloat<K> t;         // hit distance
  };

  /* Specialization for a single hit */
  template<>
  struct HitK<1>
  {
     /* Default construction does nothing */
    __forceinline HitK() {}

    /* Constructs a hit */
    __forceinline HitK(int instID, int geomID, int primID, float u, float v, float t, const Vec3fa& Ng)
      : Ng(Ng.x,Ng.y,Ng.z), instID(instID), geomID(geomID), primID(primID), u(u), v(v), t(t) {}

    /* Returns the size of the hit */
    static __forceinline size_t size() { return 1; }

  public:
    Vec3<float> Ng;  // geometry normal
    int instID;      // instance ID
    int geomID;      // geometry ID
    int primID;      // primitive ID
    float u;         // barycentric u coordinate of hit
    float v;         // barycentric v coordinate of hit
    float t;         // hit distance
  };

  /* Shortcuts */
  typedef HitK<1>  Hit;
  typedef HitK<4>  Hit4;
  typedef HitK<8>  Hit8;
  typedef HitK<16> Hit16;

  /* Outputs hit to stream */
  template<int K>
  inline std::ostream& operator<<(std::ostream& cout, const HitK<K>& ray)
  {
    return cout << "{ " << std::endl
                << "  instID = " << ray.instID << std::endl
                << "  geomID = " << ray.geomID << std::endl
                << "  primID = " << ray.primID <<  std::endl
                << "  u = " << ray.u <<  std::endl
                << "  v = " << ray.v << std::endl
                << "  t = " << ray.v << std::endl
                << "  Ng = " << ray.Ng
                << "}";
  }
}
