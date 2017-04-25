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

namespace embree
{
  namespace isa
  {
    /*! Intersects a ray with a quad with backface culling
     *  enabled. The quad v0,v1,v2,v3 is split into two triangles
     *  v0,v1,v3 and v2,v3,v1. The edge v1,v2 decides which of the two
     *  triangles gets intersected. */
    template<int N>
      __forceinline vbool<N> intersect_quad_backface_culling(const vbool<N>& valid0,
                                                             const Vec3fa& ray_org,
                                                             const Vec3fa& ray_dir,
                                                             const float ray_tnear,
                                                             const float ray_tfar,
                                                             const Vec3<vfloat<N>>& quad_v0, 
                                                             const Vec3<vfloat<N>>& quad_v1, 
                                                             const Vec3<vfloat<N>>& quad_v2, 
                                                             const Vec3<vfloat<N>>& quad_v3, 
                                                             vfloat<N>& u_o, 
                                                             vfloat<N>& v_o,
                                                             vfloat<N>& t_o)
      {
        /* calculate vertices relative to ray origin */
        vbool<N> valid = valid0;
        typedef Vec3<vfloat<N>> Vec3vfN;
        const Vec3vfN O = Vec3vfN(ray_org);
        const Vec3vfN D = Vec3vfN(ray_dir);
        const Vec3vfN va = quad_v0-O;
        const Vec3vfN vb = quad_v1-O;
        const Vec3vfN vc = quad_v2-O;
        const Vec3vfN vd = quad_v3-O;
        
        const Vec3vfN edb = vb-vd;
        const vfloat<N> WW = dot(cross(vd,edb),D);
        const Vec3vfN v0 = select(WW <= 0.0f,va,vc);
        const Vec3vfN v1 = select(WW <= 0.0f,vb,vd);
        const Vec3vfN v2 = select(WW <= 0.0f,vd,vb);

        /* calculate edges */
        const Vec3vfN e0 = v2-v0;
        const Vec3vfN e1 = v0-v1;
                
        /* perform edge tests */
        const vfloat<N> U = dot(cross(v0,e0),D);
        const vfloat<N> V = dot(cross(v1,e1),D);  
        valid &= max(U,V) <= 0.0f;
        if (unlikely(none(valid))) return false;
        
        /* calculate geometry normal and denominator */
        const Vec3vfN Ng = cross(e1,e0);
        const vfloat<N> den = dot(Ng,D);
        const vfloat<N> absDen = abs(den);
        const vfloat<N> sgnDen = signmsk(den);
        
        /* perform depth test */
        const vfloat<N> T = dot(v0,Ng);
        valid &= ((T^sgnDen) >= absDen*vfloat<N>(ray_tnear));
        valid &=(absDen*vfloat<N>(ray_tfar) >= (T^sgnDen));
        if (unlikely(none(valid))) return false;
        
        /* avoid division by 0 */
        valid &= den != vfloat<N>(zero);
        if (unlikely(none(valid))) return false;
        
        /* update hit information */
        const vfloat<N> rcpDen = rcp(den);
        t_o = T * rcpDen;
        u_o = U * rcpDen;
        v_o = V * rcpDen;
        u_o = select(WW <= 0.0f,u_o,1.0f-u_o);
        v_o = select(WW <= 0.0f,v_o,1.0f-v_o);
        return valid;
      }
  }
}
