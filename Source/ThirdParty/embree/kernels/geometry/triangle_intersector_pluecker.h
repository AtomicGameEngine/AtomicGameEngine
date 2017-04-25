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

#include "triangle.h"
#include "trianglev.h"
#include "trianglev_mb.h"
#include "intersector_epilog.h"

/*! Modified Pluecker ray/triangle intersector. The test first shifts
 *  the ray origin into the origin of the coordinate system and then
 *  uses Pluecker coordinates for the intersection. Due to the shift,
 *  the Pluecker coordinate calculation simplifies and the tests get
 *  numerically stable. The edge equations are watertight along the
 *  edge for neighboring triangles. */

namespace embree
{
  namespace isa
  {
    template<int M, typename UVMapper>
      struct PlueckerHitM
    {
      __forceinline PlueckerHitM(const vfloat<M>& U, const vfloat<M>& V, const vfloat<M>& T, const vfloat<M>& den, const Vec3<vfloat<M>>& Ng, const UVMapper& mapUV)
        : U(U), V(V), T(T), den(den), mapUV(mapUV), vNg(Ng) {}
      
      __forceinline void finalize() 
      {
        const vfloat<M> rcpDen = rcp(den);
        vt = T * rcpDen;
        vu = U * rcpDen;
        vv = V * rcpDen;
        mapUV(vu,vv);
      }
      
      __forceinline Vec2f uv (const size_t i) const { return Vec2f(vu[i],vv[i]); }
      __forceinline float t  (const size_t i) const { return vt[i]; }
      __forceinline Vec3fa Ng(const size_t i) const { return Vec3fa(vNg.x[i],vNg.y[i],vNg.z[i]); }
      
    private:
      const vfloat<M> U;
      const vfloat<M> V;
      const vfloat<M> T;
      const vfloat<M> den;
      const UVMapper& mapUV;
      
    public:
      vfloat<M> vu;
      vfloat<M> vv;
      vfloat<M> vt;
      Vec3<vfloat<M>> vNg;
    };

    template<int M>
      struct PlueckerIntersector1
      {
        __forceinline PlueckerIntersector1() {}

        __forceinline PlueckerIntersector1(const Ray& ray, const void* ptr) {}
        
        template<typename UVMapper, typename Epilog>
          __forceinline bool intersect(Ray& ray, 
                                       const Vec3<vfloat<M>>& tri_v0, 
                                       const Vec3<vfloat<M>>& tri_v1, 
                                       const Vec3<vfloat<M>>& tri_v2,  
                                       const UVMapper& mapUV,
                                       const Epilog& epilog) const
        {
          /* calculate vertices relative to ray origin */
          typedef Vec3<vfloat<M>> Vec3vfM;
          const Vec3vfM O = Vec3vfM(ray.org);
          const Vec3vfM D = Vec3vfM(ray.dir);
          const Vec3vfM v0 = tri_v0-O;
          const Vec3vfM v1 = tri_v1-O;
          const Vec3vfM v2 = tri_v2-O;
          
          /* calculate triangle edges */
          const Vec3vfM e0 = v2-v0;
          const Vec3vfM e1 = v0-v1;
          const Vec3vfM e2 = v1-v2;
          
          /* perform edge tests */
          const vfloat<M> U = dot(cross(v2+v0,e0),D);
          const vfloat<M> V = dot(cross(v0+v1,e1),D);
          const vfloat<M> W = dot(cross(v1+v2,e2),D);
#if defined(EMBREE_BACKFACE_CULLING)
          const vfloat<M> maxUVW = max(U,V,W);
          vbool<M> valid = maxUVW <= 0.0f;
#else
          const vfloat<M> minUVW = min(U,V,W);
          const vfloat<M> maxUVW = max(U,V,W);
          vbool<M> valid = (minUVW >= 0.0f) | (maxUVW <= 0.0f);
#endif
          if (unlikely(none(valid))) return false;
          
          /* calculate geometry normal and denominator */
          const Vec3vfM Ng = stable_triangle_normal(e2,e1,e0);
          const vfloat<M> den = twice(dot(Ng,D));
          const vfloat<M> absDen = abs(den);
          const vfloat<M> sgnDen = signmsk(den);
          
          /* perform depth test */
          const vfloat<M> T = twice(dot(v0,Ng));
          valid &= ((T^sgnDen) >= absDen*vfloat<M>(ray.tnear));
          valid &=(absDen*vfloat<M>(ray.tfar) >= (T^sgnDen));
          if (unlikely(none(valid))) return false;
          
          /* avoid division by 0 */
          valid &= den != vfloat<M>(zero);
          if (unlikely(none(valid))) return false;
          
          /* update hit information */
          PlueckerHitM<M,UVMapper> hit(U,V,T,den,Ng,mapUV);
          return epilog(valid,hit);
        }
      };

    template<int K, typename UVMapper>
      struct PlueckerHitK
    {
      __forceinline PlueckerHitK(const vfloat<K>& U, const vfloat<K>& V, const vfloat<K>& T, const vfloat<K>& den, const Vec3<vfloat<K>>& Ng, const UVMapper& mapUV)
        : U(U), V(V), T(T), den(den), Ng(Ng), mapUV(mapUV) {}
      
      __forceinline std::tuple<vfloat<K>,vfloat<K>,vfloat<K>,Vec3<vfloat<K>>> operator() () const
      {
        const vfloat<K> rcpDen = rcp(den);
        const vfloat<K> t = T * rcpDen;
        vfloat<K> u = U * rcpDen;
        vfloat<K> v = V * rcpDen;
        mapUV(u,v);
        return std::make_tuple(u,v,t,Ng);
      }
      
    private:
      const vfloat<K> U;
      const vfloat<K> V;
      const vfloat<K> T;
      const vfloat<K> den;
      const Vec3<vfloat<K>> Ng;
      const UVMapper& mapUV;
    };
    
    template<int M, int K>
      struct PlueckerIntersectorK
      {
        __forceinline PlueckerIntersectorK(const vbool<K>& valid, const RayK<K>& ray) {}
        
        /*! Intersects K rays with one of M triangles. */
        template<typename UVMapper, typename Epilog>
          __forceinline vbool<K> intersectK(const vbool<K>& valid0, 
                                            RayK<K>& ray, 
                                            const Vec3<vfloat<K>>& tri_v0, 
                                            const Vec3<vfloat<K>>& tri_v1, 
                                            const Vec3<vfloat<K>>& tri_v2, 
                                            const UVMapper& mapUV,
                                            const Epilog& epilog) const
        {
          /* calculate vertices relative to ray origin */
          typedef Vec3<vfloat<K>> Vec3vfK;
          vbool<K> valid = valid0;
          const Vec3vfK O = ray.org;
          const Vec3vfK D = ray.dir;
          const Vec3vfK v0 = tri_v0-O;
          const Vec3vfK v1 = tri_v1-O;
          const Vec3vfK v2 = tri_v2-O;
          
          /* calculate triangle edges */
          const Vec3vfK e0 = v2-v0;
          const Vec3vfK e1 = v0-v1;
          const Vec3vfK e2 = v1-v2;
           
          /* perform edge tests */
          const vfloat<K> U = dot(Vec3vfK(cross(v2+v0,e0)),D);
          const vfloat<K> V = dot(Vec3vfK(cross(v0+v1,e1)),D);
          const vfloat<K> W = dot(Vec3vfK(cross(v1+v2,e2)),D);
#if defined(EMBREE_BACKFACE_CULLING)
          const vfloat<K> maxUVW = max(U,V,W);
          valid &= maxUVW <= 0.0f;
#else
          const vfloat<K> minUVW = min(U,V,W);
          const vfloat<K> maxUVW = max(U,V,W);
          valid &= (minUVW >= 0.0f) | (maxUVW <= 0.0f);
#endif
          if (unlikely(none(valid))) return false;
          
           /* calculate geometry normal and denominator */
          const Vec3vfK Ng = stable_triangle_normal(e2,e1,e0);
          const vfloat<K> den = twice(dot(Vec3vfK(Ng),D));
          const vfloat<K> absDen = abs(den);
          const vfloat<K> sgnDen = signmsk(den);

          /* perform depth test */
          const vfloat<K> T = twice(dot(v0,Vec3vfK(Ng)));
          valid &= ((T^sgnDen) >= absDen*ray.tnear);
          valid &= (absDen*ray.tfar >= (T^sgnDen));
          if (unlikely(none(valid))) return false;
          
          /* avoid division by 0 */
          valid &= den != vfloat<K>(zero);
          if (unlikely(none(valid))) return false;
          
          /* calculate hit information */
          PlueckerHitK<K,UVMapper> hit(U,V,T,den,Ng,mapUV);
          return epilog(valid,hit);
        }
        
        /*! Intersect k'th ray from ray packet of size K with M triangles. */
        template<typename UVMapper, typename Epilog>
          __forceinline bool intersect(RayK<K>& ray, size_t k,
                                       const Vec3<vfloat<M>>& tri_v0, 
                                       const Vec3<vfloat<M>>& tri_v1, 
                                       const Vec3<vfloat<M>>& tri_v2, 
                                       const UVMapper& mapUV,
                                       const Epilog& epilog) const
        {
          /* calculate vertices relative to ray origin */
          typedef Vec3<vfloat<M>> Vec3vfM;
          const Vec3vfM O = broadcast<vfloat<M>>(ray.org,k);
          const Vec3vfM D = broadcast<vfloat<M>>(ray.dir,k);
          const Vec3vfM v0 = tri_v0-O;
          const Vec3vfM v1 = tri_v1-O;
          const Vec3vfM v2 = tri_v2-O;
          
          /* calculate triangle edges */
          const Vec3vfM e0 = v2-v0;
          const Vec3vfM e1 = v0-v1;
          const Vec3vfM e2 = v1-v2;
          
          /* perform edge tests */
          const vfloat<M> U = dot(cross(v2+v0,e0),D);
          const vfloat<M> V = dot(cross(v0+v1,e1),D);
          const vfloat<M> W = dot(cross(v1+v2,e2),D);
#if defined(EMBREE_BACKFACE_CULLING)
          const vfloat<M> maxUVW = max(U,V,W);
          vbool<M> valid = maxUVW <= 0.0f;
#else
          const vfloat<M> minUVW = min(U,V,W);
          const vfloat<M> maxUVW = max(U,V,W);
          vbool<M> valid = (minUVW >= 0.0f) | (maxUVW <= 0.0f);
#endif
          if (unlikely(none(valid))) return false;
          
          /* calculate geometry normal and denominator */
          const Vec3vfM Ng = stable_triangle_normal(e2,e1,e0);
          const vfloat<M> den = twice(dot(Ng,D));
          const vfloat<M> absDen = abs(den);
          const vfloat<M> sgnDen = signmsk(den);

          /* perform depth test */
          const vfloat<M> T = twice(dot(v0,Ng));
          valid &= ((T^sgnDen) >= absDen*vfloat<M>(ray.tnear[k]));
          valid &= (absDen*vfloat<M>(ray.tfar[k]) >= (T^sgnDen));
          if (unlikely(none(valid))) return false;
          
          /* avoid division by 0 */
          valid &= den != vfloat<M>(zero);
          if (unlikely(none(valid))) return false;
          
          /* calculate hit information */
          PlueckerHitM<M,UVMapper> hit(U,V,T,den,Ng,mapUV);
          return epilog(valid,hit);
        }
      };
  }
}
