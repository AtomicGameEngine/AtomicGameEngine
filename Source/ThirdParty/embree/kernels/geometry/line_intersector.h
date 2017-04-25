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
#include "filter.h"

namespace embree
{
  namespace isa
  {
    template<int M>
      struct LineIntersectorHitM
      {
        __forceinline LineIntersectorHitM() {}

        __forceinline LineIntersectorHitM(const vfloat<M>& u, const vfloat<M>& v, const vfloat<M>& t, const Vec3<vfloat<M>>& Ng)
          : vu(u), vv(v), vt(t), vNg(Ng) {}
        
        __forceinline void finalize() {}
        
        __forceinline Vec2f uv (const size_t i) const { return Vec2f(vu[i],vv[i]); }
        __forceinline float t  (const size_t i) const { return vt[i]; }
        __forceinline Vec3fa Ng(const size_t i) const { return Vec3fa(vNg.x[i],vNg.y[i],vNg.z[i]); }
        
      public:
        vfloat<M> vu;
        vfloat<M> vv;
        vfloat<M> vt;
        Vec3<vfloat<M>> vNg;
      };
    
    template<int M>
      struct LineIntersector1
      {
        typedef Vec3<vfloat<M>> Vec3vfM;
        typedef Vec4<vfloat<M>> Vec4vfM;
        
        struct Precalculations
        {
          __forceinline Precalculations() {}

          __forceinline Precalculations(const Ray& ray, const void* ptr)
          {
            const float s = rsqrt(dot(ray.dir,ray.dir));
            depth_scale = s;
            ray_space = frame(s*ray.dir).transposed();
          }
          
          vfloat<M> depth_scale;
          LinearSpace3<Vec3vfM> ray_space;
        };
        
        template<typename Epilog>
        static __forceinline bool intersect(const vbool<M>& valid_i,
                                            Ray& ray, const Precalculations& pre,
                                            const Vec4vfM& v0, const Vec4vfM& v1,
                                            const Epilog& epilog)
        {
          /* transform end points into ray space */
          vbool<M> valid = valid_i;
          Vec4vfM p0(xfmVector(pre.ray_space,v0.xyz()-Vec3vfM(ray.org)), v0.w);
          Vec4vfM p1(xfmVector(pre.ray_space,v1.xyz()-Vec3vfM(ray.org)), v1.w);
          
          /* approximative intersection with cone */
          const Vec4vfM v = p1-p0;
          const Vec4vfM w = -p0;
          const vfloat<M> d0 = madd(w.x,v.x,w.y*v.y);
          const vfloat<M> d1 = madd(v.x,v.x,v.y*v.y);
          const vfloat<M> u = clamp(d0*rcp(d1),vfloat<M>(zero),vfloat<M>(one));
          const Vec4vfM p = madd(u,v,p0);
          const vfloat<M> t = p.z*pre.depth_scale;
          const vfloat<M> d2 = madd(p.x,p.x,p.y*p.y);
          const vfloat<M> r = p.w;
          const vfloat<M> r2 = r*r;
          valid &= (d2 <= r2) & (vfloat<M>(ray.tnear) < t) & (t < vfloat<M>(ray.tfar));
          if (unlikely(none(valid))) return false;
          
          /* ignore denormalized segments */
          const Vec3vfM T = v1.xyz()-v0.xyz();
          valid &= (T.x != vfloat<M>(zero)) | (T.y != vfloat<M>(zero)) | (T.z != vfloat<M>(zero));
          if (unlikely(none(valid))) return false;
          
          /* update hit information */
          LineIntersectorHitM<M> hit(u,zero,t,T);
          return epilog(valid,hit);
        }
      };
    
    template<int M, int K>
      struct LineIntersectorK
      {
        typedef Vec3<vfloat<M>> Vec3vfM;
        typedef Vec4<vfloat<M>> Vec4vfM;
        
        struct Precalculations 
        {
          __forceinline Precalculations (const vbool<K>& valid, const RayK<K>& ray)
          {
            size_t mask = movemask(valid);
            depth_scale = rsqrt(dot(ray.dir,ray.dir));
            while (mask) {
              size_t k = __bscf(mask);
              ray_space[k] = frame(depth_scale[k]*Vec3fa(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k])).transposed();
            }
          }
          
          vfloat<K> depth_scale;
          LinearSpace3<Vec3vfM> ray_space[K];
        };
        
        template<typename Epilog>
        static __forceinline bool intersect(const vbool<M>& valid_i,
                                            RayK<K>& ray, size_t k, const Precalculations& pre,
                                            const Vec4vfM& v0, const Vec4vfM& v1,
                                            const Epilog& epilog)
        {
          /* transform end points into ray space */
          vbool<M> valid = valid_i;
          const Vec3vfM ray_org(ray.org.x[k],ray.org.y[k],ray.org.z[k]);
          const Vec3vfM ray_dir(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k]);
          Vec4vfM p0(xfmVector(pre.ray_space[k],v0.xyz()-ray_org), v0.w);
          Vec4vfM p1(xfmVector(pre.ray_space[k],v1.xyz()-ray_org), v1.w);
          
          /* approximative intersection with cone */
          const Vec4vfM v = p1-p0;
          const Vec4vfM w = -p0;
          const vfloat<M> d0 = madd(w.x,v.x,w.y*v.y);
          const vfloat<M> d1 = madd(v.x,v.x,v.y*v.y);
          const vfloat<M> u = clamp(d0*rcp(d1),vfloat<M>(zero),vfloat<M>(one));
          const Vec4vfM p = madd(u,v,p0);
          const vfloat<M> t = p.z*pre.depth_scale[k];
          const vfloat<M> d2 = madd(p.x,p.x,p.y*p.y);
          const vfloat<M> r = p.w;
          const vfloat<M> r2 = r*r;
          valid &= (d2 <= r2) & (vfloat<M>(ray.tnear[k]) < t) & (t < vfloat<M>(ray.tfar[k]));
          if (unlikely(none(valid))) return false;
          
          /* ignore denormalized segments */
          const Vec3vfM T = v1.xyz()-v0.xyz();
          valid &= (T.x != vfloat<M>(zero)) | (T.y != vfloat<M>(zero)) | (T.z != vfloat<M>(zero));
          if (unlikely(none(valid))) return false;
          
          /* update hit information */
          LineIntersectorHitM<M> hit(u,zero,t,T);
          return epilog(valid,hit);
        }
      };
  }
}
