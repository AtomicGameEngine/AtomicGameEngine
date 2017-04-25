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

// FIXME: remove this file later
//#define Bezier1Intersector1 Hair1Intersector1
//#define Bezier1IntersectorK Hair1IntersectorK

namespace embree
{
  namespace isa
  {
    template<typename NativeCurve3fa, int M>
      struct HairHit
    {
      __forceinline HairHit() {}

      __forceinline HairHit(const vbool<M>& valid, const vfloat<M>& U, const vfloat<M>& V, const vfloat<M>& T, const int i, const int N,
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
    
    template<typename NativeCurve3fa>
      struct Hair1Intersector1
    {
      float depth_scale;
      LinearSpace3fa ray_space;

      __forceinline Hair1Intersector1() {}

      __forceinline Hair1Intersector1(const Ray& ray, const void* ptr) 
         : depth_scale(rsqrt(dot(ray.dir,ray.dir))), ray_space(frame(depth_scale*ray.dir).transposed()) {}

      template<typename Epilog>
      __forceinline bool intersect(Ray& ray,
                                   const Vec3fa& v0, const Vec3fa& v1, const Vec3fa& v2, const Vec3fa& v3, const int N,
                                   const Epilog& epilog) const
      {
        /* transform control points into ray space */
        Vec3fa w0 = xfmVector(ray_space,v0-ray.org); w0.w = v0.w;
        Vec3fa w1 = xfmVector(ray_space,v1-ray.org); w1.w = v1.w;
        Vec3fa w2 = xfmVector(ray_space,v2-ray.org); w2.w = v2.w;
        Vec3fa w3 = xfmVector(ray_space,v3-ray.org); w3.w = v3.w;
        NativeCurve3fa curve2D(w0,w1,w2,w3);
      
        /* evaluate the bezier curve */
        vboolx valid = vfloatx(step) < vfloatx(float(N));
        const Vec4vfx p0 = curve2D.template eval0<VSIZEX>(0,N);
        const Vec4vfx p1 = curve2D.template eval1<VSIZEX>(0,N);

        /* approximative intersection with cone */
        const Vec4vfx v = p1-p0;
        const Vec4vfx w = -p0;
        const vfloatx d0 = madd(w.x,v.x,w.y*v.y);
        const vfloatx d1 = madd(v.x,v.x,v.y*v.y);
        const vfloatx u = clamp(d0*rcp(d1),vfloatx(zero),vfloatx(one));
        const Vec4vfx p = madd(u,v,p0);
        const vfloatx t = p.z*depth_scale;
        const vfloatx d2 = madd(p.x,p.x,p.y*p.y); 
        const vfloatx r = p.w;
        const vfloatx r2 = r*r;
        valid &= (d2 <= r2) & (vfloatx(ray.tnear) < t) & (t < vfloatx(ray.tfar));

        /* update hit information */
        bool ishit = false;
        if (unlikely(any(valid))) {
          HairHit<NativeCurve3fa,VSIZEX> hit(valid,u,0.0f,t,0,N,v0,v1,v2,v3);
          ishit = ishit | epilog(valid,hit);
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
            
            /* approximative intersection with cone */
            const Vec4vfx v = p1-p0;
            const Vec4vfx w = -p0;
            const vfloatx d0 = madd(w.x,v.x,w.y*v.y);
            const vfloatx d1 = madd(v.x,v.x,v.y*v.y);
            const vfloatx u = clamp(d0*rcp(d1),vfloatx(zero),vfloatx(one));
            const Vec4vfx p = madd(u,v,p0);
            const vfloatx t = p.z*depth_scale;
            const vfloatx d2 = madd(p.x,p.x,p.y*p.y); 
            const vfloatx r = p.w;
            const vfloatx r2 = r*r;
            valid &= (d2 <= r2) & (vfloatx(ray.tnear) < t) & (t < vfloatx(ray.tfar));

             /* update hit information */
            if (unlikely(any(valid))) {
              HairHit<NativeCurve3fa,VSIZEX> hit(valid,u,0.0f,t,i,N,v0,v1,v2,v3);
              ishit = ishit | epilog(valid,hit);
            }
          }
        }
        return ishit;
      }
    };

    template<typename NativeCurve3fa, int K>
      struct Hair1IntersectorK
    {
      vfloat<K> depth_scale;
      LinearSpace3fa ray_space[K];

      __forceinline Hair1IntersectorK(const vbool<K>& valid, const RayK<K>& ray) 
      {
        size_t mask = movemask(valid);
        depth_scale = rsqrt(dot(ray.dir,ray.dir));
        while (mask) {
          size_t k = __bscf(mask);
          ray_space[k] = frame(depth_scale[k]*Vec3fa(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k])).transposed();
        }
      }

      __forceinline Hair1IntersectorK (const RayK<K>& ray, size_t k)
      {
        Vec3fa ray_dir = Vec3fa(ray.dir.x[k],ray.dir.y[k],ray.dir.z[k]);
        depth_scale[k] = rsqrt(dot(ray_dir,ray_dir));
        ray_space  [k] = frame(depth_scale[k]*ray_dir).transposed();
      }
      
      template<typename Epilog>
      __forceinline bool intersect(RayK<K>& ray, size_t k,
                                   const Vec3fa& v0, const Vec3fa& v1, const Vec3fa& v2, const Vec3fa& v3, const int N,
                                   const Epilog& epilog) const
      {
        /* load ray */
        const Vec3fa ray_org(ray.org.x[k],ray.org.y[k],ray.org.z[k]);
        const float ray_tnear = ray.tnear[k];
        const float ray_tfar  = ray.tfar [k];
        
        /* transform control points into ray space */
        Vec3fa w0 = xfmVector(ray_space[k],v0-ray_org); w0.w = v0.w;
        Vec3fa w1 = xfmVector(ray_space[k],v1-ray_org); w1.w = v1.w;
        Vec3fa w2 = xfmVector(ray_space[k],v2-ray_org); w2.w = v2.w;
        Vec3fa w3 = xfmVector(ray_space[k],v3-ray_org); w3.w = v3.w;
        NativeCurve3fa curve2D(w0,w1,w2,w3);
        
        /* process SIMD-size many segments per iteration */
        bool ishit = false;
        for (int i=0; i<N; i+=VSIZEX)
        {
          /* evaluate the bezier curve */
          vboolx valid = vintx(i)+vintx(step) < vintx(N);
          const Vec4vfx p0 = curve2D.template eval0<VSIZEX>(i,N);
          const Vec4vfx p1 = curve2D.template eval1<VSIZEX>(i,N);
          
          /* approximative intersection with cone */
          const Vec4vfx v = p1-p0;
          const Vec4vfx w = -p0;
          const vfloatx d0 = madd(w.x,v.x,w.y*v.y);
          const vfloatx d1 = madd(v.x,v.x,v.y*v.y);
          const vfloatx u = clamp(d0*rcp(d1),vfloatx(zero),vfloatx(one));
          const Vec4vfx p = madd(u,v,p0);
          const vfloatx t = p.z*depth_scale[k];
          const vfloatx d2 = madd(p.x,p.x,p.y*p.y); 
          const vfloatx r = p.w;
          const vfloatx r2 = r*r;
          valid &= (d2 <= r2) & (vfloatx(ray_tnear) < t) & (t < vfloatx(ray_tfar));
          if (likely(none(valid))) continue;
        
          /* update hit information */
          HairHit<NativeCurve3fa,VSIZEX> hit(valid,u,0.0f,t,i,N,v0,v1,v2,v3);
          ishit = ishit | epilog(valid,hit);
        }
        return ishit;
      }
    };  
  }
}
