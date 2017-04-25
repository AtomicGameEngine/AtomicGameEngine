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

#include "../common/ray.h"

namespace embree
{
  namespace isa
  {
    struct HalfPlane
    {
      const Vec3fa P;  //!< plane origin
      const Vec3fa N;  //!< plane normal

      __forceinline HalfPlane(const Vec3fa& P, const Vec3fa& N) 
        : P(P), N(N) {}
      
      __forceinline BBox1f intersect(const Vec3fa& ray_org, const Vec3fa& ray_dir) const
      {
        Vec3fa O = Vec3fa(ray_org) - P;
        Vec3fa D = Vec3fa(ray_dir);
        float ON = dot(O,N);
        float DN = dot(D,N);
        float t = -ON*rcp(abs(DN) < min_rcp_input ? min_rcp_input : DN );
        float lower = select(DN < 0.0f, float(neg_inf), t);
        float upper = select(DN < 0.0f, t, float(pos_inf));
        return BBox1f(lower,upper);
      }
    };

    template<int M>
      struct HalfPlaneN
      {
        const Vec3<vfloat<M>> P;  //!< plane origin
        const Vec3<vfloat<M>> N;  //!< plane normal

        __forceinline HalfPlaneN(const Vec3<vfloat<M>>& P, const Vec3<vfloat<M>>& N)
          : P(P), N(N) {}

        __forceinline BBox<vfloat<M>> intersect(const Vec3fa& ray_org, const Vec3fa& ray_dir) const
        {
          Vec3<vfloat<M>> O = Vec3<vfloat<M>>(ray_org) - P;
          Vec3<vfloat<M>> D = Vec3<vfloat<M>>(ray_dir);
          vfloat<M> ON = dot(O,N);
          vfloat<M> DN = dot(D,N);
          vfloat<M> t = -ON*rcp(select(abs(DN) < min_rcp_input, min_rcp_input, DN) );
          vfloat<M> lower = select(DN < 0.0f, vfloat<M>(neg_inf), t);
          vfloat<M> upper = select(DN < 0.0f, t, vfloat<M>(pos_inf));
          return BBox<vfloat<M>>(lower,upper);
        }
      };
  }
}
