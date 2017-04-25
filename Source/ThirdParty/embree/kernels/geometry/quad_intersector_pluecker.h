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

#include "quad_intersector_moeller.h"

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
    struct PlueckerIntersectorTriangle1
    {
        template<int M, typename Epilog>
        static __forceinline bool intersect(Ray& ray, 
                                     const Vec3<vfloat<M>>& tri_v0, 
                                     const Vec3<vfloat<M>>& tri_v1, 
                                     const Vec3<vfloat<M>>& tri_v2, 
                                     const vbool<M>& flags,
                                     const Epilog& epilog)
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
          vbool<M> valid = (maxUVW <= 0.0f) | (minUVW >= 0.0f);
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
          QuadHitM<M> hit(valid,U,V,T,den,Ng,flags);
          return epilog(valid,hit);
        }  
      };

    /*! Intersects M quads with 1 ray */
    template<int M, bool filter>
      struct QuadMIntersector1Pluecker
    {
      __forceinline QuadMIntersector1Pluecker() {}

      __forceinline QuadMIntersector1Pluecker(const Ray& ray, const void* ptr) {}

      __forceinline void intersect(Ray& ray, IntersectContext* context,
                                   const Vec3<vfloat<M>>& v0, const Vec3<vfloat<M>>& v1, const Vec3<vfloat<M>>& v2, const Vec3<vfloat<M>>& v3, 
                                   const vint<M>& geomID, const vint<M>& primID) const
      {
        Intersect1EpilogM<M,M,filter> epilog(ray,context,geomID,primID);
        PlueckerIntersectorTriangle1::intersect(ray,v0,v1,v3,vbool<M>(false),epilog);
        PlueckerIntersectorTriangle1::intersect(ray,v2,v3,v1,vbool<M>(true),epilog);
      }
      
      __forceinline bool occluded(Ray& ray, IntersectContext* context,
                                  const Vec3<vfloat<M>>& v0, const Vec3<vfloat<M>>& v1, const Vec3<vfloat<M>>& v2, const Vec3<vfloat<M>>& v3, 
                                  const vint<M>& geomID, const vint<M>& primID) const
      {
        Occluded1EpilogM<M,M,filter> epilog(ray,context,geomID,primID);
        if (PlueckerIntersectorTriangle1::intersect(ray,v0,v1,v3,vbool<M>(false),epilog)) return true;
        if (PlueckerIntersectorTriangle1::intersect(ray,v2,v3,v1,vbool<M>(true ),epilog)) return true;
        return false;
      }
    };

#if defined(__AVX512F__)

    /*! Intersects 4 quads with 1 ray using AVX512 */
    template<bool filter>
      struct QuadMIntersector1Pluecker<4,filter>
    {
      __forceinline QuadMIntersector1Pluecker() {}

      __forceinline QuadMIntersector1Pluecker(const Ray& ray, const void* ptr) {}

      template<typename Epilog>
        __forceinline bool intersect(Ray& ray, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, const Epilog& epilog) const
      {
        const Vec3vf16 vtx0(select(0x0f0f,vfloat16(v0.x),vfloat16(v2.x)),
                            select(0x0f0f,vfloat16(v0.y),vfloat16(v2.y)),
                            select(0x0f0f,vfloat16(v0.z),vfloat16(v2.z)));
#if !defined(EMBREE_BACKFACE_CULLING)
        const Vec3vf16 vtx1(vfloat16(v1.x),vfloat16(v1.y),vfloat16(v1.z));
        const Vec3vf16 vtx2(vfloat16(v3.x),vfloat16(v3.y),vfloat16(v3.z));
#else
        const Vec3vf16 vtx1(select(0x0f0f,vfloat16(v1.x),vfloat16(v3.x)),
                            select(0x0f0f,vfloat16(v1.y),vfloat16(v3.y)),
                            select(0x0f0f,vfloat16(v1.z),vfloat16(v3.z)));
        const Vec3vf16 vtx2(select(0x0f0f,vfloat16(v3.x),vfloat16(v1.x)),
                            select(0x0f0f,vfloat16(v3.y),vfloat16(v1.y)),
                            select(0x0f0f,vfloat16(v3.z),vfloat16(v1.z)));
#endif
        const vbool16 flags(0xf0f0);
        return PlueckerIntersectorTriangle1::intersect(ray,vtx0,vtx1,vtx2,flags,epilog);
      }
      
      __forceinline bool intersect(Ray& ray, IntersectContext* context,
                                   const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                   const vint4& geomID, const vint4& primID) const
      {
        return intersect(ray,v0,v1,v2,v3,Intersect1EpilogM<8,16,filter>(ray,context,vint8(geomID),vint8(primID)));
      }
      
      __forceinline bool occluded(Ray& ray, IntersectContext* context,
                                  const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                  const vint4& geomID, const vint4& primID) const
      {
        return intersect(ray,v0,v1,v2,v3,Occluded1EpilogM<8,16,filter>(ray,context,vint8(geomID),vint8(primID)));
      }
    };

#elif defined (__AVX__)

    /*! Intersects 4 quads with 1 ray using AVX */
    template<bool filter>
      struct QuadMIntersector1Pluecker<4,filter>
    {
      __forceinline QuadMIntersector1Pluecker() {}

      __forceinline QuadMIntersector1Pluecker(const Ray& ray, const void* ptr) {}
      
      template<typename Epilog>
        __forceinline bool intersect(Ray& ray, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, const Epilog& epilog) const
      {
        const Vec3vf8 vtx0(vfloat8(v0.x,v2.x),vfloat8(v0.y,v2.y),vfloat8(v0.z,v2.z));
#if !defined(EMBREE_BACKFACE_CULLING)
        const Vec3vf8 vtx1(vfloat8(v1.x),vfloat8(v1.y),vfloat8(v1.z));
        const Vec3vf8 vtx2(vfloat8(v3.x),vfloat8(v3.y),vfloat8(v3.z));
#else
        const Vec3vf8 vtx1(vfloat8(v1.x,v3.x),vfloat8(v1.y,v3.y),vfloat8(v1.z,v3.z));
        const Vec3vf8 vtx2(vfloat8(v3.x,v1.x),vfloat8(v3.y,v1.y),vfloat8(v3.z,v1.z));
#endif
        const vbool8 flags(0,0,0,0,1,1,1,1);
        return PlueckerIntersectorTriangle1::intersect(ray,vtx0,vtx1,vtx2,flags,epilog); 
      }
      
      __forceinline bool intersect(Ray& ray, IntersectContext* context, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                   const vint4& geomID, const vint4& primID) const
      {
        return intersect(ray,v0,v1,v2,v3,Intersect1EpilogM<8,8,filter>(ray,context,vint8(geomID),vint8(primID)));
      }
      
      __forceinline bool occluded(Ray& ray, IntersectContext* context, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                  const vint4& geomID, const vint4& primID) const
      {
        return intersect(ray,v0,v1,v2,v3,Occluded1EpilogM<8,8,filter>(ray,context,vint8(geomID),vint8(primID)));
      }
    };

#endif



    /* ----------------------------- */
    /* -- ray packet intersectors -- */
    /* ----------------------------- */

    struct PlueckerIntersector1KTriangleM
    {
      /*! Intersect k'th ray from ray packet of size K with M triangles. */
      template<int M, int K, typename Epilog>
       static  __forceinline bool intersect1(RayK<K>& ray, 
                                     size_t k,
                                     const Vec3<vfloat<M>>& tri_v0, 
                                     const Vec3<vfloat<M>>& tri_v1, 
                                     const Vec3<vfloat<M>>& tri_v2, 
                                     const vbool<M>& flags,                                     
                                     const Epilog& epilog)
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
          const vfloat<M> minUVW MAYBE_UNUSED = min(U,V,W);
          const vfloat<M> maxUVW = max(U,V,W);
#if defined(EMBREE_BACKFACE_CULLING)
          vbool<M> valid = maxUVW <= 0.0f;
#else
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
          QuadHitM<M> hit(valid,U,V,T,den,Ng,flags);
          return epilog(valid,hit);
      }
    };

    template<int M, int K, bool filter>
    struct QuadMIntersectorKPlueckerBase
    {
      __forceinline QuadMIntersectorKPlueckerBase(const vbool<K>& valid, const RayK<K>& ray) {}
            
      /*! Intersects K rays with one of M triangles. */
      template<typename Epilog>
        __forceinline vbool<K> intersectK(const vbool<K>& valid0, 
                                          RayK<K>& ray, 
                                          const Vec3<vfloat<K>>& tri_v0, 
                                          const Vec3<vfloat<K>>& tri_v1, 
                                          const Vec3<vfloat<K>>& tri_v2, 
                                          const vbool<K>& flags,
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
          valid &= (maxUVW <= 0.0f) | (minUVW >= 0.0f);
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
          QuadHitK<K> hit(U,V,T,den,Ng,flags);
          return epilog(valid,hit);
      }
      
      /*! Intersects K rays with one of M quads. */
      template<typename Epilog>
      __forceinline bool intersectK(const vbool<K>& valid0, 
                                    RayK<K>& ray, 
                                    const Vec3<vfloat<K>>& v0, 
                                    const Vec3<vfloat<K>>& v1, 
                                    const Vec3<vfloat<K>>& v2, 
                                    const Vec3<vfloat<K>>& v3, 
                                    const Epilog& epilog) const
      {
        intersectK(valid0,ray,v0,v1,v3,vbool<K>(false),epilog);
        if (none(valid0)) return true;
        intersectK(valid0,ray,v2,v3,v1,vbool<K>(true ),epilog);
        return none(valid0);
      }
    };

    template<int M, int K, bool filter>
      struct QuadMIntersectorKPluecker : public QuadMIntersectorKPlueckerBase<M,K,filter>
    {
      __forceinline QuadMIntersectorKPluecker(const vbool<K>& valid, const RayK<K>& ray)
        : QuadMIntersectorKPlueckerBase<M,K,filter>(valid,ray) {}

      __forceinline void intersect1(RayK<K>& ray, size_t k, IntersectContext* context,
                                    const Vec3<vfloat<M>>& v0, const Vec3<vfloat<M>>& v1, const Vec3<vfloat<M>>& v2, const Vec3<vfloat<M>>& v3, 
                                    const vint<M>& geomID, const vint<M>& primID) const
      {
        Intersect1KEpilogM<M,M,K,filter> epilog(ray,k,context,geomID,primID);
        PlueckerIntersector1KTriangleM::intersect1(ray,k,v0,v1,v3,vbool<M>(false),epilog);
        PlueckerIntersector1KTriangleM::intersect1(ray,k,v2,v3,v1,vbool<M>(true ),epilog);
      }
      
      __forceinline bool occluded1(RayK<K>& ray, size_t k, IntersectContext* context,
                                   const Vec3<vfloat<M>>& v0, const Vec3<vfloat<M>>& v1, const Vec3<vfloat<M>>& v2, const Vec3<vfloat<M>>& v3, 
                                   const vint<M>& geomID, const vint<M>& primID) const
      {
        Occluded1KEpilogM<M,M,K,filter> epilog(ray,k,context,geomID,primID);
        if (PlueckerIntersector1KTriangleM::intersect1(ray,k,v0,v1,v3,vbool<M>(false),epilog)) return true;
        if (PlueckerIntersector1KTriangleM::intersect1(ray,k,v2,v3,v1,vbool<M>(true ),epilog)) return true;
        return false;
      }
    };

#if defined(__AVX512F__)

    /*! Intersects 4 quads with 1 ray using AVX512 */
    template<int K, bool filter>
      struct QuadMIntersectorKPluecker<4,K,filter> : public QuadMIntersectorKPlueckerBase<4,K,filter>
    {
      __forceinline QuadMIntersectorKPluecker(const vbool<K>& valid, const RayK<K>& ray)
        : QuadMIntersectorKPlueckerBase<4,K,filter>(valid,ray) {}

      template<typename Epilog>
        __forceinline bool intersect1(RayK<K>& ray, size_t k, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, const Epilog& epilog) const
      {
        const Vec3vf16 vtx0(select(0x0f0f,vfloat16(v0.x),vfloat16(v2.x)),
                            select(0x0f0f,vfloat16(v0.y),vfloat16(v2.y)),
                            select(0x0f0f,vfloat16(v0.z),vfloat16(v2.z)));
#if !defined(EMBREE_BACKFACE_CULLING)
        const Vec3vf16 vtx1(vfloat16(v1.x),vfloat16(v1.y),vfloat16(v1.z));
        const Vec3vf16 vtx2(vfloat16(v3.x),vfloat16(v3.y),vfloat16(v3.z));
#else
        const Vec3vf16 vtx1(select(0x0f0f,vfloat16(v1.x),vfloat16(v3.x)),
                            select(0x0f0f,vfloat16(v1.y),vfloat16(v3.y)),
                            select(0x0f0f,vfloat16(v1.z),vfloat16(v3.z)));
        const Vec3vf16 vtx2(select(0x0f0f,vfloat16(v3.x),vfloat16(v1.x)),
                            select(0x0f0f,vfloat16(v3.y),vfloat16(v1.y)),
                            select(0x0f0f,vfloat16(v3.z),vfloat16(v1.z)));
#endif

        const vbool16 flags(0xf0f0);
        return PlueckerIntersector1KTriangleM::intersect1(ray,k,vtx0,vtx1,vtx2,flags,epilog);
      }
      
      __forceinline bool intersect1(RayK<K>& ray, size_t k, IntersectContext* context, 
                                    const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                    const vint4& geomID, const vint4& primID) const
      {
        return intersect1(ray,k,v0,v1,v2,v3,Intersect1KEpilogM<8,16,K,filter>(ray,k,context,vint8(geomID),vint8(primID)));
      }
      
      __forceinline bool occluded1(RayK<K>& ray, size_t k, IntersectContext* context,
                                   const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                   const vint4& geomID, const vint4& primID) const
      {
        return intersect1(ray,k,v0,v1,v2,v3,Occluded1KEpilogM<8,16,K,filter>(ray,k,context,vint8(geomID),vint8(primID)));
      }
    };

#elif defined (__AVX__)

    /*! Intersects 4 quads with 1 ray using AVX */
     template<int K, bool filter>
       struct QuadMIntersectorKPluecker<4,K,filter> : public QuadMIntersectorKPlueckerBase<4,K,filter>
    {
      __forceinline QuadMIntersectorKPluecker(const vbool<K>& valid, const RayK<K>& ray)
        : QuadMIntersectorKPlueckerBase<4,K,filter>(valid,ray) {}
      
      template<typename Epilog>
        __forceinline bool intersect1(RayK<K>& ray, size_t k, const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, const Epilog& epilog) const
      {
        const Vec3vf8 vtx0(vfloat8(v0.x,v2.x),vfloat8(v0.y,v2.y),vfloat8(v0.z,v2.z));
        const vbool8 flags(0,0,0,0,1,1,1,1);
#if !defined(EMBREE_BACKFACE_CULLING)
        const Vec3vf8 vtx1(vfloat8(v1.x),vfloat8(v1.y),vfloat8(v1.z));
        const Vec3vf8 vtx2(vfloat8(v3.x),vfloat8(v3.y),vfloat8(v3.z));
#else
        const Vec3vf8 vtx1(vfloat8(v1.x,v3.x),vfloat8(v1.y,v3.y),vfloat8(v1.z,v3.z));
        const Vec3vf8 vtx2(vfloat8(v3.x,v1.x),vfloat8(v3.y,v1.y),vfloat8(v3.z,v1.z));
#endif
        return PlueckerIntersector1KTriangleM::intersect1(ray,k,vtx0,vtx1,vtx2,flags,epilog); 
      }
      
      __forceinline bool intersect1(RayK<K>& ray, size_t k, IntersectContext* context,
                                    const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                    const vint4& geomID, const vint4& primID) const
      {
        return intersect1(ray,k,v0,v1,v2,v3,Intersect1KEpilogM<8,8,K,filter>(ray,k,context,vint8(geomID),vint8(primID)));
      }
      
      __forceinline bool occluded1(RayK<K>& ray, size_t k, IntersectContext* context,
                                   const Vec3vf4& v0, const Vec3vf4& v1, const Vec3vf4& v2, const Vec3vf4& v3, 
                                   const vint4& geomID, const vint4& primID) const
      {
        return intersect1(ray,k,v0,v1,v2,v3,Occluded1KEpilogM<8,8,K,filter>(ray,k,context,vint8(geomID),vint8(primID)));
      }
    };

#endif
  }
}
