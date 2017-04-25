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

#include "quadi_mb.h"
#include "quad_intersector_moeller.h"
#include "quad_intersector_pluecker.h"

namespace embree
{
  namespace isa
  {
    /*! Intersects M motion blur quads with 1 ray */
    template<int M, bool filter>
      struct QuadMiMBIntersector1Moeller
    {
      typedef QuadMiMB<M> Primitive;
      typedef Intersector1PrecalculationsMB<QuadMIntersector1MoellerTrumbore<M,filter>> Precalculations;
        
      /*! Intersect a ray with the M quads and updates the hit. */
      static __forceinline void intersect(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& quad)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time);
        pre.intersect(ray,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
      }
        
      /*! Test if the ray is occluded by one of M quads. */
      static __forceinline bool occluded(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& quad)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time);
        return pre.occluded(ray,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
      }
    };
    
    /*! Intersects M motion blur quads with K rays. */
    template<int M, int K, bool filter>
      struct QuadMiMBIntersectorKMoeller
      {
        typedef QuadMiMB<M> Primitive;
        typedef IntersectorKPrecalculationsMB<K,QuadMIntersectorKMoellerTrumbore<M,K,filter>> Precalculations;
        
        /*! Intersects K rays with M quads. */
        static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          for (size_t i=0; i<QuadMiMB<M>::max_size(); i++)
          {
            if (!quad.valid(i)) break;
            STAT3(normal.trav_prims,1,popcnt(valid_i),K);
            Vec3<vfloat<K>> v0,v1,v2,v3; quad.gather(valid_i,v0,v1,v2,v3,i,context->scene,ray.time);
            pre.intersectK(valid_i,ray,v0,v1,v2,v3,IntersectKEpilogM<M,K,filter>(ray,context,quad.geomIDs,quad.primIDs,i));
          }
        }
        
        /*! Test for K rays if they are occluded by any of the M quads. */
        static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          vbool<K> valid0 = valid_i;
          for (size_t i=0; i<QuadMiMB<M>::max_size(); i++)
          {
            if (!quad.valid(i)) break;
            STAT3(shadow.trav_prims,1,popcnt(valid0),K);
            Vec3<vfloat<K>> v0,v1,v2,v3; quad.gather(valid_i,v0,v1,v2,v3,i,context->scene,ray.time);
            if (pre.intersectK(valid0,ray,v0,v1,v2,v3,OccludedKEpilogM<M,K,filter>(valid0,ray,context,quad.geomIDs,quad.primIDs,i)))
              break;
          }
          return !valid0;
        }
        
        /*! Intersect a ray with M quads and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          STAT3(normal.trav_prims,1,1,1);
          Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time[k]);
          pre.intersect1(ray,k,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
        }
        
        /*! Test if the ray is occluded by one of the M quads. */
        static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          STAT3(shadow.trav_prims,1,1,1);
          Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time[k]);
          return pre.occluded1(ray,k,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
        }
      };

    /*! Intersects M motion blur quads with 1 ray */
    template<int M, bool filter>
      struct QuadMiMBIntersector1Pluecker
    {
      typedef QuadMiMB<M> Primitive;
      typedef Intersector1PrecalculationsMB<QuadMIntersector1Pluecker<M,filter>> Precalculations;
        
      /*! Intersect a ray with the M quads and updates the hit. */
      static __forceinline void intersect(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& quad)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time);
        pre.intersect(ray,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
      }
        
      /*! Test if the ray is occluded by one of M quads. */
      static __forceinline bool occluded(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& quad)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time);
        return pre.occluded(ray,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
      }
    };
    
    /*! Intersects M motion blur quads with K rays. */
    template<int M, int K, bool filter>
      struct QuadMiMBIntersectorKPluecker
      {
        typedef QuadMiMB<M> Primitive;
        typedef IntersectorKPrecalculationsMB<K,QuadMIntersectorKPluecker<M,K,filter>> Precalculations;
        
        /*! Intersects K rays with M quads. */
        static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          for (size_t i=0; i<QuadMiMB<M>::max_size(); i++)
          {
            if (!quad.valid(i)) break;
            STAT3(normal.trav_prims,1,popcnt(valid_i),K);
            Vec3<vfloat<K>> v0,v1,v2,v3; quad.gather(valid_i,v0,v1,v2,v3,i,context->scene,ray.time);
            pre.intersectK(valid_i,ray,v0,v1,v2,v3,IntersectKEpilogM<M,K,filter>(ray,context,quad.geomIDs,quad.primIDs,i));
          }
        }
        
        /*! Test for K rays if they are occluded by any of the M quads. */
        static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          vbool<K> valid0 = valid_i;
          for (size_t i=0; i<QuadMiMB<M>::max_size(); i++)
          {
            if (!quad.valid(i)) break;
            STAT3(shadow.trav_prims,1,popcnt(valid0),K);
            Vec3<vfloat<K>> v0,v1,v2,v3; quad.gather(valid_i,v0,v1,v2,v3,i,context->scene,ray.time);
            if (pre.intersectK(valid0,ray,v0,v1,v2,v3,OccludedKEpilogM<M,K,filter>(valid0,ray,context,quad.geomIDs,quad.primIDs,i)))
              break;
          }
          return !valid0;
        }
        
        /*! Intersect a ray with M quads and updates the hit. */
        static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          STAT3(normal.trav_prims,1,1,1);
          Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time[k]);
          pre.intersect1(ray,k,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
        }
        
        /*! Test if the ray is occluded by one of the M quads. */
        static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const QuadMiMB<M>& quad)
        {
          STAT3(shadow.trav_prims,1,1,1);
          Vec3<vfloat<M>> v0,v1,v2,v3; quad.gather(v0,v1,v2,v3,context->scene,ray.time[k]);
          return pre.occluded1(ray,k,context,v0,v1,v2,v3,quad.geomIDs,quad.primIDs); 
        }
      };
  }
}

