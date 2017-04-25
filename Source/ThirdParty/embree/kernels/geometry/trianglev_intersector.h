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
#include "triangle_intersector_pluecker.h"
#include "triangle_intersector_moeller.h"

namespace embree
{
  namespace isa
  {
    /*! Intersects M triangles with 1 ray */
    template<int M, int Mx, bool filter>
      struct TriangleMvIntersector1Moeller
      {
        typedef TriangleMv<M> Primitive;
        typedef Intersector1Precalculations<MoellerTrumboreIntersector1<Mx>> Precalculations;
        
        /*! Intersect a ray with M triangles and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& tri)
        {
          STAT3(normal.trav_prims,1,1,1);
          pre.intersect(ray,tri.v0,tri.v1,tri.v2,/*UVIdentity<Mx>(),*/Intersect1EpilogM<M,Mx,filter>(ray,context,tri.geomIDs,tri.primIDs)); 
        }
        
        /*! Test if the ray is occluded by one of the M triangles. */
        static __forceinline bool occluded(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& tri)
        {
          STAT3(shadow.trav_prims,1,1,1);
          return pre.intersect(ray,tri.v0,tri.v1,tri.v2,/*UVIdentity<Mx>(),*/Occluded1EpilogM<M,Mx,filter>(ray,context,tri.geomIDs,tri.primIDs)); 
        }

        /*! Intersect an array of rays with an array of M primitives. */
        static __forceinline size_t intersect(Precalculations* pre, size_t valid, Ray** rays, IntersectContext* context,  size_t ty, const Primitive* prim, size_t num)
        {
          size_t valid_isec = 0;
          do {
            const size_t i = __bscf(valid);
            const float old_far = rays[i]->tfar;
            for (size_t n=0; n<num; n++)
              intersect(pre[i],*rays[i],context,prim[n]);
            valid_isec |= (rays[i]->tfar < old_far) ? ((size_t)1 << i) : 0;            
          } while(unlikely(valid));
          return valid_isec;
        }
      };

    /*! Intersects M triangles with K rays */
    template<int M, int Mx, int K, bool filter>
      struct TriangleMvIntersectorKMoeller
      {
        typedef TriangleMv<M> Primitive;
        typedef IntersectorKPrecalculations<K,MoellerTrumboreIntersectorK<Mx,K>> Precalculations;
        
        /*! Intersects K rays with M triangles. */
        static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& tri)
        {
          for (size_t i=0; i<M; i++)
          {
            if (!tri.valid(i)) break;
            STAT3(normal.trav_prims,1,popcnt(valid_i),K);
            const Vec3<vfloat<K>> v0 = broadcast<vfloat<K>>(tri.v0,i);
            const Vec3<vfloat<K>> v1 = broadcast<vfloat<K>>(tri.v1,i);
            const Vec3<vfloat<K>> v2 = broadcast<vfloat<K>>(tri.v2,i);
            pre.intersectK(valid_i,ray,v0,v1,v2,/*UVIdentity<K>(),*/IntersectKEpilogM<M,K,filter>(ray,context,tri.geomIDs,tri.primIDs,i));
          }
        }
        
        /*! Test for K rays if they are occluded by any of the M triangles. */
        static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& tri)
        {
          vbool<K> valid0 = valid_i;
          
          for (size_t i=0; i<M; i++)
          {
            if (!tri.valid(i)) break;
            STAT3(shadow.trav_prims,1,popcnt(valid_i),K);
            const Vec3<vfloat<K>> v0 = broadcast<vfloat<K>>(tri.v0,i);
            const Vec3<vfloat<K>> v1 = broadcast<vfloat<K>>(tri.v1,i);
            const Vec3<vfloat<K>> v2 = broadcast<vfloat<K>>(tri.v2,i);
            pre.intersectK(valid0,ray,v0,v1,v2,/*UVIdentity<K>(),*/OccludedKEpilogM<M,K,filter>(valid0,ray,context,tri.geomIDs,tri.primIDs,i));
            if (none(valid0)) break;
          }
          return !valid0;
        }
        
        /*! Intersect a ray with M triangles and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& tri)
        {
          STAT3(normal.trav_prims,1,1,1);
          pre.intersect(ray,k,tri.v0,tri.v1,tri.v2,/*UVIdentity<Mx>(),*/Intersect1KEpilogM<M,Mx,K,filter>(ray,k,context,tri.geomIDs,tri.primIDs)); //FIXME: M,Mx
        }
        
        /*! Test if the ray is occluded by one of the M triangles. */
        static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& tri)
        {
          STAT3(shadow.trav_prims,1,1,1);
          return pre.intersect(ray,k,tri.v0,tri.v1,tri.v2,/*UVIdentity<Mx>(),*/Occluded1KEpilogM<M,Mx,K,filter>(ray,k,context,tri.geomIDs,tri.primIDs)); //FIXME: M,Mx
        }
      };

    /*! Intersects M triangles with 1 ray */
    template<int M, int Mx, bool filter>
      struct TriangleMvIntersector1Pluecker
      {
        typedef TriangleMv<M> Primitive;
        typedef Intersector1Precalculations<PlueckerIntersector1<Mx>> Precalculations;
        
        /*! Intersect a ray with M triangles and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& tri)
        {
          STAT3(normal.trav_prims,1,1,1);
          pre.intersect(ray,tri.v0,tri.v1,tri.v2,UVIdentity<Mx>(),Intersect1EpilogM<M,Mx,filter>(ray,context,tri.geomIDs,tri.primIDs)); 
        }
        
        /*! Test if the ray is occluded by one of the M triangles. */
        static __forceinline bool occluded(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& tri)
        {
          STAT3(shadow.trav_prims,1,1,1);
          return pre.intersect(ray,tri.v0,tri.v1,tri.v2,UVIdentity<Mx>(),Occluded1EpilogM<M,Mx,filter>(ray,context,tri.geomIDs,tri.primIDs)); 
        }

        /*! Intersect an array of rays with an array of M primitives. */
        static __forceinline size_t intersect(Precalculations* pre, size_t valid, Ray** rays, IntersectContext* context,  size_t ty, const Primitive* prim, size_t num)
        {
          size_t valid_isec = 0;
          do {
            const size_t i = __bscf(valid);
            const float old_far = rays[i]->tfar;
            for (size_t n=0; n<num; n++)
              intersect(pre[i],*rays[i],context,prim[n]);
            valid_isec |= (rays[i]->tfar < old_far) ? ((size_t)1 << i) : 0;            
          } while(unlikely(valid));
          return valid_isec;
        }
      };

    /*! Intersects M triangles with K rays */
    template<int M, int Mx, int K, bool filter>
      struct TriangleMvIntersectorKPluecker
      {
        typedef TriangleMv<M> Primitive;
        typedef IntersectorKPrecalculations<K,PlueckerIntersectorK<Mx,K>> Precalculations;
        
        /*! Intersects K rays with M triangles. */
        static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& tri)
        {
          for (size_t i=0; i<M; i++)
          {
            if (!tri.valid(i)) break;
            STAT3(normal.trav_prims,1,popcnt(valid_i),K);
            const Vec3<vfloat<K>> v0 = broadcast<vfloat<K>>(tri.v0,i);
            const Vec3<vfloat<K>> v1 = broadcast<vfloat<K>>(tri.v1,i);
            const Vec3<vfloat<K>> v2 = broadcast<vfloat<K>>(tri.v2,i);
            pre.intersectK(valid_i,ray,v0,v1,v2,UVIdentity<K>(),IntersectKEpilogM<M,K,filter>(ray,context,tri.geomIDs,tri.primIDs,i));
          }
        }
        
        /*! Test for K rays if they are occluded by any of the M triangles. */
        static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& tri)
        {
          vbool<K> valid0 = valid_i;
          
          for (size_t i=0; i<M; i++)
          {
            if (!tri.valid(i)) break;
            STAT3(shadow.trav_prims,1,popcnt(valid_i),K);
            const Vec3<vfloat<K>> v0 = broadcast<vfloat<K>>(tri.v0,i);
            const Vec3<vfloat<K>> v1 = broadcast<vfloat<K>>(tri.v1,i);
            const Vec3<vfloat<K>> v2 = broadcast<vfloat<K>>(tri.v2,i);
            pre.intersectK(valid0,ray,v0,v1,v2,UVIdentity<K>(),OccludedKEpilogM<M,K,filter>(valid0,ray,context,tri.geomIDs,tri.primIDs,i));
            if (none(valid0)) break;
          }
          return !valid0;
        }
        
        /*! Intersect a ray with M triangles and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& tri)
        {
          STAT3(normal.trav_prims,1,1,1);
          pre.intersect(ray,k,tri.v0,tri.v1,tri.v2,UVIdentity<Mx>(),Intersect1KEpilogM<M,Mx,K,filter>(ray,k,context,tri.geomIDs,tri.primIDs)); //FIXME: M,Mx
        }
        
        /*! Test if the ray is occluded by one of the M triangles. */
        static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& tri)
        {
          STAT3(shadow.trav_prims,1,1,1);
          return pre.intersect(ray,k,tri.v0,tri.v1,tri.v2,UVIdentity<Mx>(),Occluded1KEpilogM<M,Mx,K,filter>(ray,k,context,tri.geomIDs,tri.primIDs)); //FIXME: M,Mx
        }
      };
  }
}
