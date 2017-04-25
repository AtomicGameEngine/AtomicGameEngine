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

#include "object.h"
#include "../common/ray.h"

namespace embree
{
  namespace isa
  {
    template<bool mblur>
      struct ObjectIntersector1
    {
      typedef Object Primitive;
     
      static const bool validIntersectorK = false;

      struct PrecalculationsBase {
        __forceinline PrecalculationsBase() {}
        __forceinline PrecalculationsBase (const Ray& ray, const void *ptr) {}
      };

      typedef typename std::conditional<mblur, 
        Intersector1PrecalculationsMB<PrecalculationsBase>,
        Intersector1Precalculations<PrecalculationsBase>>::type Precalculations;
      
      static __forceinline void intersect(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& prim) 
      {
        AVX_ZERO_UPPER();
        AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);

        /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
        if ((ray.mask & accel->mask) == 0) 
          return;
#endif

        accel->intersect(ray,prim.primID,context);
      }
      
      static __forceinline bool occluded(const Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& prim) 
      {
        AVX_ZERO_UPPER();
        AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);

        /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
        if ((ray.mask & accel->mask) == 0) 
          return false;
#endif

        accel->occluded(ray,prim.primID,context);
        return ray.geomID == 0;
      }
      
      //template<typename Context>
      static __forceinline size_t intersect(Precalculations* pre, size_t valid_in, Ray** rays, IntersectContext* context, size_t ty, const Primitive* prims, size_t num /*, size_t& lazy_node */)
      {
        AVX_ZERO_UPPER();
        
        /* intersect all primitives */
        for (size_t i=0; i<num; i++)
        {
          const Primitive& prim = prims[i];
          AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);

          size_t N = 0, valid = valid_in;
          Ray* rays_filtered[64];
          while (unlikely(valid)) 
          {
            const size_t i = __bscf(valid);
            Ray* ray = rays[i];

            /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
            if ((ray->mask & accel->mask) == 0) 
              continue;
#endif
            rays_filtered[N++] = ray;
          }
          if (unlikely(N == 0)) continue;

          /* call user stream intersection function */
          accel->intersect1M(rays_filtered,N,prim.primID,context);
        }

        /* /\* update all contexts *\/ */
        /* size_t valid = valid_in; */
        /* while (unlikely(valid)) { */
        /*   const size_t i = __bscf(valid); */
        /*   ctx[i].update(rays[i]); */
        /* } */
        return valid_in;
      }

      static __forceinline size_t occluded(Precalculations* pre, size_t valid_in, Ray** rays, IntersectContext* context, size_t ty, const Primitive* prims, size_t num /*, size_t& lazy_node */)
      {
        AVX_ZERO_UPPER();
        size_t hit = 0;
        
        /* intersect all primitives */
        for (size_t i=0; i<num; i++)
        {
          const Primitive& prim = prims[i];
          AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);

          size_t N = 0, valid = valid_in;
          Ray* rays_filtered[64];
          size_t index_filtered[64];
          while (unlikely(valid)) 
          {
            const size_t i = __bscf(valid);
            Ray* ray = rays[i];

            /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
            if ((ray->mask & accel->mask) == 0) 
              continue;
#endif
            rays_filtered[N] = ray;
            index_filtered[N] = i;
            N++;
          }
          if (unlikely(N == 0)) continue;

          /* call user stream occluded function */
          accel->occluded1M(rays_filtered,N,prim.primID,context);

          /* mark occluded rays */
          for (size_t i=0; i<N; i++)
          {
            if (rays_filtered[i]->geomID == 0) {
              hit |= (size_t)1 << index_filtered[i];
            }
          }
          valid_in &= ~hit;
        }
        return hit;
      }

      template<int K>
      static __forceinline void intersectK(const vbool<K>& valid, /* PrecalculationsK& pre, */ RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t num, size_t& lazy_node)
      {
      }

      template<int K>
      static __forceinline vbool<K> occludedK(const vbool<K>& valid, /* PrecalculationsK& pre, */ RayK<K>& ray, IntersectContext* context, const Primitive* prim, size_t num, size_t& lazy_node)
      {
        return valid;
      }
    };

    template<int K, bool mblur>
      struct ObjectIntersectorK
    {
      typedef Object Primitive;
      
      struct PrecalculationsBase {
        __forceinline PrecalculationsBase (const vbool<K>& valid, const RayK<K>& ray) {}
      };

      typedef typename std::conditional<mblur, 
        IntersectorKPrecalculationsMB<K,PrecalculationsBase>,
        IntersectorKPrecalculations<K,PrecalculationsBase>>::type Precalculations;
      
      static __forceinline void intersect(const vbool<K>& valid_i, const Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        AVX_ZERO_UPPER();
        vbool<K> valid = valid_i;
        AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);
        
        /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
        valid &= (ray.mask & accel->mask) != 0;
        if (none(valid)) return;
#endif
        accel->intersect(valid,ray,prim.primID,context);
      }

      static __forceinline vbool<K> occluded(const vbool<K>& valid_i, const Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        vbool<K> valid = valid_i;
        AccelSet* accel = (AccelSet*) context->scene->get(prim.geomID);
        
        /* perform ray mask test */
#if defined(EMBREE_RAY_MASK)
        valid &= (ray.mask & accel->mask) != 0;
        if (none(valid)) return false;
#endif
        accel->occluded(valid,ray,prim.primID,context);
        return ray.geomID == 0;
      }

      static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& prim) {
        intersect(vbool<K>(1<<int(k)),pre,ray,context,prim);
      }
      
      static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& prim) {
        occluded(vbool<K>(1<<int(k)),pre,ray,context,prim);
        return ray.geomID[k] == 0; 
      }
    };

    typedef ObjectIntersectorK<4,false>  ObjectIntersector4;
    typedef ObjectIntersectorK<8,false>  ObjectIntersector8;
    typedef ObjectIntersectorK<16,false> ObjectIntersector16;

    typedef ObjectIntersectorK<4,true>  ObjectIntersector4MB;
    typedef ObjectIntersectorK<8,true>  ObjectIntersector8MB;
    typedef ObjectIntersectorK<16,true> ObjectIntersector16MB;
  }
}
