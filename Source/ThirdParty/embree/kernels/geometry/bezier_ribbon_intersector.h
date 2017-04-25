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
#include "quad_intersector.h"

#define Bezier1Intersector1 Ribbon1Intersector1
#define Bezier1IntersectorK Ribbon1IntersectorK

namespace embree
{
  namespace isa
  {
    template<typename NativeCurve3fa, int M>
      struct RibbonHit
    {
      __forceinline RibbonHit() {}

      __forceinline RibbonHit(const vbool<M>& valid, const vfloat<M>& U, const vfloat<M>& V, const vfloat<M>& T, const int i, const int N,
                              const Vec3fa& p0, const Vec3fa& p1, const Vec3fa& p2, const Vec3fa& p3)
        : U(U), V(V), T(T), i(i), N(N), p0(p0), p1(p1), p2(p2), p3(p3), valid(valid) {}
      
      __forceinline void finalize() 
      {
        vu = (vfloat<M>(step)+U+vfloat<M>(float(i)))*(1.0f/float(N));
        vv = V;
        vt = T;
      }
      
      __forceinline Vec2f uv (const size_t i) const { return Vec2f(vu[i],vv[i]); }
      __forceinline float t  (const size_t i) const { return vt[i]; }
      __forceinline Vec3fa Ng(const size_t i) const 
      { 
        Vec3fa T = NativeCurve3fa(p0,p1,p2,p3).eval_du(vu[i]);
        return T == Vec3fa(zero) ? Vec3fa(one) : T; 
      }
      
    public:
      vfloat<M> U;
      vfloat<M> V;
      vfloat<M> T;
      int i, N;
      Vec3fa p0,p1,p2,p3;
      
    public:
      vbool<M> valid;
      vfloat<M> vu;
      vfloat<M> vv;
      vfloat<M> vt;
    };

    /* calculate squared distance of point p0 to line p1->p2 */
    __forceinline std::pair<vfloatx,vfloatx> sqr_point_line_distance(const Vec2vfx& p0, const Vec2vfx& p1, const Vec2vfx& p2)
    {
      const vfloatx num = det(p2-p1,p1-p0);
      const vfloatx den2 = dot(p2-p1,p2-p1);
      return std::make_pair(num*num,den2);
    }
    
    /* performs culling against a cylinder */
    __forceinline vboolx cylinder_culling_test(const Vec2vfx& p0, const Vec2vfx& p1, const Vec2vfx& p2, const vfloatx& r)
    {
      const std::pair<vfloatx,vfloatx> d = sqr_point_line_distance(p0,p1,p2);
      return d.first <= r*r*d.second;
    }

    template<typename NativeCurve3fa, typename Epilog>
      __forceinline bool intersect_ribbon(const Vec3fa& ray_org, const Vec3fa& ray_dir, const float ray_tnear, const float ray_tfar,
                                          const float depth_scale, const LinearSpace3fa& ray_space,
                                          const Vec3fa& v0, const Vec3fa& v1, const Vec3fa& v2, const Vec3fa& v3, const int N,
                                          const Epilog& epilog)
    {  
      /* transform control points into ray space */
      Vec3fa w0 = xfmVector(ray_space,v0-ray_org); w0.w = v0.w;
      Vec3fa w1 = xfmVector(ray_space,v1-ray_org); w1.w = v1.w;
      Vec3fa w2 = xfmVector(ray_space,v2-ray_org); w2.w = v2.w;
      Vec3fa w3 = xfmVector(ray_space,v3-ray_org); w3.w = v3.w;
      NativeCurve3fa curve2D(w0,w1,w2,w3);
      
      /* evaluate the bezier curve */
      bool ishit = false;
      vboolx valid = vfloatx(step) < vfloatx(float(N));
      const Vec4vfx p0 = curve2D.template eval0<VSIZEX>(0,N);
      const Vec4vfx p1 = curve2D.template eval1<VSIZEX>(0,N);
      valid &= cylinder_culling_test(zero,Vec2vfx(p0.x,p0.y),Vec2vfx(p1.x,p1.y),max(p0.w,p1.w));
      
      if (any(valid)) 
      {
        const Vec3vfx dp0dt = curve2D.template derivative0<VSIZEX>(0,N);
        const Vec3vfx dp1dt = curve2D.template derivative1<VSIZEX>(0,N);
        const Vec3vfx n0(dp0dt.y,-dp0dt.x,0.0f);
        const Vec3vfx n1(dp1dt.y,-dp1dt.x,0.0f);
        const Vec3vfx nn0 = normalize(n0);
        const Vec3vfx nn1 = normalize(n1);
        const Vec3vfx lp0 =  madd(p0.w,nn0,Vec3vfx(p0));
        const Vec3vfx lp1 =  madd(p1.w,nn1,Vec3vfx(p1));
        const Vec3vfx up0 = nmadd(p0.w,nn0,Vec3vfx(p0));
        const Vec3vfx up1 = nmadd(p1.w,nn1,Vec3vfx(p1));
        
        vfloatx vu,vv,vt;
        vboolx valid0 = intersect_quad_backface_culling(valid,zero,Vec3fa(0,0,1),ray_tnear*depth_scale,ray_tfar*depth_scale,lp0,lp1,up1,up0,vu,vv,vt);
        if (any(valid0))
        {
          vv = madd(2.0f,vv,vfloatx(-1.0f));
          RibbonHit<NativeCurve3fa,VSIZEX> bhit(valid0,vu,vv,depth_scale*vt,0,N,v0,v1,v2,v3);
          ishit |= epilog(bhit.valid,bhit);
        }
      }
      
      if (unlikely(VSIZEX < N)) 
      {
        /* process SIMD-size many segments per iteration */
        for (int i=VSIZEX; i<N; i+=VSIZEX)
        {
          /* evaluate the bezier curve */
          vboolx valid = vintx(i)+vintx(step) < vintx(N);
          const Vec4vfx p0 = curve2D.template eval0<VSIZEX>(i,N);
          const Vec4vfx p1 = curve2D.template eval1<VSIZEX>(i,N);
          valid &= cylinder_culling_test(zero,Vec2vfx(p0.x,p0.y),Vec2vfx(p1.x,p1.y),max(p0.w,p1.w));
          if (none(valid)) continue;
          
          const Vec3vfx dp0dt = curve2D.template derivative0<VSIZEX>(i,N);
          const Vec3vfx dp1dt = curve2D.template derivative1<VSIZEX>(i,N);
          const Vec3vfx n0(dp0dt.y,-dp0dt.x,0.0f);
          const Vec3vfx n1(dp1dt.y,-dp1dt.x,0.0f);
          const Vec3vfx nn0 = normalize(n0);
          const Vec3vfx nn1 = normalize(n1);
          const Vec3vfx lp0 =  madd(p0.w,nn0,Vec3vfx(p0));
          const Vec3vfx lp1 =  madd(p1.w,nn1,Vec3vfx(p1));
          const Vec3vfx up0 = nmadd(p0.w,nn0,Vec3vfx(p0));
          const Vec3vfx up1 = nmadd(p1.w,nn1,Vec3vfx(p1));
          
          vfloatx vu,vv,vt;
          vboolx valid0 = intersect_quad_backface_culling(valid,zero,Vec3fa(0,0,1),ray_tnear*depth_scale,ray_tfar*depth_scale,lp0,lp1,up1,up0,vu,vv,vt);
          if (any(valid0))
          {
            vv = madd(2.0f,vv,vfloatx(-1.0f));
            RibbonHit<NativeCurve3fa,VSIZEX> bhit(valid0,vu,vv,depth_scale*vt,i,N,v0,v1,v2,v3);
            ishit |= epilog(bhit.valid,bhit);
          }
        }
      }
      return ishit;
    }
    
    template<typename NativeCurve3fa>
      struct Ribbon1Intersector1
    {
      float depth_scale;
      LinearSpace3fa ray_space;

      __forceinline Ribbon1Intersector1() {}

      __forceinline Ribbon1Intersector1(const Ray& ray, const void* ptr) 
         : depth_scale(rsqrt(dot(ray.dir,ray.dir))), ray_space(frame(depth_scale*ray.dir).transposed()) {}

      template<typename Epilog>
      __forceinline bool intersect(Ray& ray,
                                   const Vec3fa& v0, const Vec3fa& v1, const Vec3fa& v2, const Vec3fa& v3, const int N,
                                   const Epilog& epilog) const
      {
        return intersect_ribbon<NativeCurve3fa>(ray.org,ray.dir,ray.tnear,ray.tfar,
                                                depth_scale,ray_space,
                                                v0,v1,v2,v3,N,
                                                epilog);
      }
    };
    
    template<typename NativeCurve3fa, int K>
    struct Ribbon1IntersectorK
    {
      vfloat<K> depth_scale;
      LinearSpace3fa ray_space[K];

      __forceinline Ribbon1IntersectorK(const vbool<K>& valid, const RayK<K>& ray)
      {
        size_t mask = movemask(valid);
        depth_scale = rsqrt(dot(ray.dir,ray.dir));
        while (mask) {
          size_t k = __bscf(mask);
          ray_space[k] = frame(depth_scale[k]*Vec3fa(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k])).transposed();
        }
      }

      template<typename Epilog>
      __forceinline bool intersect(RayK<K>& ray, size_t k,
                                   const Vec3fa& v0, const Vec3fa& v1, const Vec3fa& v2, const Vec3fa& v3, const int N,
                                   const Epilog& epilog) const
      {
        const Vec3fa ray_org(ray.org.x[k],ray.org.y[k],ray.org.z[k]);
        const Vec3fa ray_dir(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k]);
        const float ray_tnear = ray.tnear[k];
        const float ray_tfar  = ray.tfar [k];
        return intersect_ribbon<NativeCurve3fa>(ray_org,ray_dir,ray_tnear,ray_tfar,
                                                depth_scale[k],ray_space[k],
                                                v0,v1,v2,v3,N,
                                                epilog);
      }
    };
  }
}
