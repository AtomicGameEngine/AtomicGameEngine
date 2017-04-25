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

#include "linei.h"
#include "line_intersector.h"
#include "intersector_epilog.h"

namespace embree
{
  namespace isa
  {
    template<int M, int Mx, bool filter>
    struct LineMiIntersector1
    {
      typedef LineMi<M> Primitive;
      typedef Intersector1Precalculations<typename LineIntersector1<Mx>::Precalculations> Precalculations;

      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& line)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene);
        const vbool<Mx> valid = line.template valid<Mx>();
        LineIntersector1<Mx>::intersect(valid,ray,pre,v0,v1,Intersect1EpilogM<M,Mx,filter>(ray,context,line.geomIDs,line.primIDs));
      }

      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& line)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene);
        const vbool<Mx> valid = line.template valid<Mx>();
        return LineIntersector1<Mx>::intersect(valid,ray,pre,v0,v1,Occluded1EpilogM<M,Mx,filter>(ray,context,line.geomIDs,line.primIDs));
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

    template<int M, int Mx, bool filter>
    struct LineMiMBIntersector1
    {
      typedef LineMi<M> Primitive;
      typedef Intersector1PrecalculationsMB<typename LineIntersector1<Mx>::Precalculations> Precalculations;

      static __forceinline void intersect(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& line)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene,ray.time);
        const vbool<Mx> valid = line.template valid<Mx>();
        LineIntersector1<Mx>::intersect(valid,ray,pre,v0,v1,Intersect1EpilogM<M,Mx,filter>(ray,context,line.geomIDs,line.primIDs));
      }

      static __forceinline bool occluded(Precalculations& pre, Ray& ray, IntersectContext* context, const Primitive& line)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene,ray.time);
        const vbool<Mx> valid = line.template valid<Mx>();
        return LineIntersector1<Mx>::intersect(valid,ray,pre,v0,v1,Occluded1EpilogM<M,Mx,filter>(ray,context,line.geomIDs,line.primIDs));
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

    template<int M, int Mx, int K, bool filter>
    struct LineMiIntersectorK
    {
      typedef LineMi<M> Primitive;
      typedef IntersectorKPrecalculations<K, typename LineIntersectorK<Mx,K>::Precalculations> Precalculations;

      static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& line)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene);
        const vbool<Mx> valid = line.template valid<Mx>();
        LineIntersectorK<Mx,K>::intersect(valid,ray,k,pre,v0,v1,Intersect1KEpilogM<M,Mx,K,filter>(ray,k,context,line.geomIDs,line.primIDs));
      }

      static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        int mask = movemask(valid_i);
        while (mask) intersect(pre,ray,__bscf(mask),context,prim);
      }
      
      static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& line)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene);
        const vbool<Mx> valid = line.template valid<Mx>();
        return LineIntersectorK<Mx,K>::intersect(valid,ray,k,pre,v0,v1,Occluded1KEpilogM<M,Mx,K,filter>(ray,k,context,line.geomIDs,line.primIDs));
      }

      static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        vbool<K> valid_o = false;
        int mask = movemask(valid_i);
        while (mask) {
          size_t k = __bscf(mask);
          if (occluded(pre,ray,k,context,prim))
            set(valid_o, k);
        }
        return valid_o;
      }
    };

    template<int M, int Mx, int K, bool filter>
    struct LineMiMBIntersectorK
    {
      typedef LineMi<M> Primitive;
      typedef IntersectorKPrecalculationsMB<K, typename LineIntersectorK<Mx,K>::Precalculations> Precalculations;

      static __forceinline void intersect(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context,  const Primitive& line)
      {
        STAT3(normal.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene,ray.time[k]);
        const vbool<Mx> valid = line.template valid<Mx>();
        LineIntersectorK<Mx,K>::intersect(valid,ray,k,pre,v0,v1,Intersect1KEpilogM<M,Mx,K,filter>(ray,k,context,line.geomIDs,line.primIDs));
      }

      static __forceinline void intersect(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        int mask = movemask(valid_i);
        while (mask) intersect(pre,ray,__bscf(mask),context,prim);
      }

      static __forceinline bool occluded(Precalculations& pre, RayK<K>& ray, size_t k, IntersectContext* context, const Primitive& line)
      {
        STAT3(shadow.trav_prims,1,1,1);
        Vec4<vfloat<M>> v0,v1; line.gather(v0,v1,context->scene,ray.time[k]);
        const vbool<Mx> valid = line.template valid<Mx>();
        return LineIntersectorK<Mx,K>::intersect(valid,ray,k,pre,v0,v1,Occluded1KEpilogM<M,Mx,K,filter>(ray,k,context,line.geomIDs,line.primIDs));
      }
      
      static __forceinline vbool<K> occluded(const vbool<K>& valid_i, Precalculations& pre, RayK<K>& ray, IntersectContext* context, const Primitive& prim)
      {
        vbool<K> valid_o = false;
        int mask = movemask(valid_i);
        while (mask) {
          size_t k = __bscf(mask);
          if (occluded(pre,ray,k,context,prim))
            set(valid_o, k);
        }
        return valid_o;
      }
    };
  }
}
