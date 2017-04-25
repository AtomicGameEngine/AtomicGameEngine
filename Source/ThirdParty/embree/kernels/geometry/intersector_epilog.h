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
#include "../common/context.h"
#include "filter.h"

namespace embree
{
  namespace isa
  {
    template<int M>
      struct UVIdentity {
        __forceinline void operator() (vfloat<M>& u, vfloat<M>& v) const {}
      };

    template<bool filter>
      struct Intersect1Epilog1
      {
        Ray& ray;
        IntersectContext* context;
        const int geomID;
        const int primID;
        
        __forceinline Intersect1Epilog1(Ray& ray,
                                        IntersectContext* context, 
                                        const int geomID, 
                                        const int primID)
          : ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask) == 0) return false;
#endif
          hit.finalize();
          int instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (unlikely(filter && geometry->hasIntersectionFilter1())) 
            return runIntersectionFilter1(geometry,ray,context,hit.u,hit.v,hit.t,hit.Ng,instID,primID);
#endif
          
          /* update hit information */
          ray.u = hit.u;
          ray.v = hit.v;
          ray.tfar = hit.t;
          ray.Ng = hit.Ng;
          ray.geomID = instID;
          ray.primID = primID;
          return true;
        }
      };

    template<bool filter>
      struct Occluded1Epilog1
      {
        Ray& ray;
        IntersectContext* context;
        const int geomID;
        const int primID;
        
        __forceinline Occluded1Epilog1(Ray& ray,
                                       IntersectContext* context, 
                                       const int geomID, 
                                       const int primID)
          : ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask) == 0) return false;
#endif
          hit.finalize();
          int instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (unlikely(filter && geometry->hasOcclusionFilter1())) 
            return runOcclusionFilter1(geometry,ray,context,hit.u,hit.v,hit.t,hit.Ng,instID,primID);
#endif
          return true;
        }
      };

    template<int K, bool filter>
      struct Intersect1KEpilog1
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Intersect1KEpilog1(RayK<K>& ray, size_t k,
                                         IntersectContext* context, 
                                         const unsigned int geomID, 
                                         const unsigned int primID)
          : ray(ray), k(k), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask[k]) == 0) 
            return false;
#endif
          hit.finalize();
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter && unlikely(geometry->hasIntersectionFilter<vfloat<K>>())) 
            return runIntersectionFilter(geometry,ray,k,context,hit.u,hit.v,hit.t,hit.Ng,geomID,primID);
#endif
          
          /* update hit information */
          ray.u[k] = hit.u;
          ray.v[k] = hit.v;
          ray.tfar[k] = hit.t;
          ray.Ng.x[k] = hit.Ng.x;
          ray.Ng.y[k] = hit.Ng.y;
          ray.Ng.z[k] = hit.Ng.z;
          ray.geomID[k] = geomID;
          ray.primID[k] = primID;
          return true;
        }
      };
    
    template<int K, bool filter>
      struct Occluded1KEpilog1
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Occluded1KEpilog1(RayK<K>& ray, size_t k,
                                        IntersectContext* context, 
                                        const unsigned int geomID, 
                                        const unsigned int primID)
          : ray(ray), k(k), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask[k]) == 0) 
            return false;
#endif

          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter && unlikely(geometry->hasOcclusionFilter<vfloat<K>>())) {
            hit.finalize();
            return runOcclusionFilter(geometry,ray,k,context,hit.u,hit.v,hit.t,hit.Ng,geomID,primID);
          }
#endif 
          return true;
        }
      };
    
    template<int M, int Mx, bool filter>
      struct Intersect1EpilogM
      {
        Ray& ray;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        
        __forceinline Intersect1EpilogM(Ray& ray,
                                        IntersectContext* context, 
                                        const vint<M>& geomIDs, 
                                        const vint<M>& primIDs)
          : ray(ray), context(context), geomIDs(geomIDs), primIDs(primIDs) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<Mx>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;
          vbool<Mx> valid = valid_i;          
          if (Mx > M) valid &= (1<<M)-1;
          hit.finalize();          
          size_t i = select_min(valid,hit.vt);
          int geomID = geomIDs[i];
          int instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER) || defined(EMBREE_RAY_MASK)
          bool foundhit = false;
          goto entry;
          while (true) 
          {
            if (unlikely(none(valid))) return foundhit;
            i = select_min(valid,hit.vt);

            geomID = geomIDs[i];
            instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
          entry:
            Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
            
#if defined(EMBREE_RAY_MASK)
            /* goto next hit if mask test fails */
            if ((geometry->mask & ray.mask) == 0) {
              clear(valid,i);
              continue;
            }
#endif
            
#if defined(EMBREE_INTERSECTION_FILTER) 
            /* call intersection filter function */
            if (filter) {
              if (unlikely(geometry->hasIntersectionFilter1())) {
                const Vec2f uv = hit.uv(i);
                foundhit |= runIntersectionFilter1(geometry,ray,context,uv.x,uv.y,hit.t(i),hit.Ng(i),instID,primIDs[i]);
                clear(valid,i);
                valid &= hit.vt < ray.tfar; // intersection filters may modify tfar value
                continue;
              }
            }
#endif
            break;
          }
#endif

          /* update hit information */
          const Vec2f uv = hit.uv(i);
          ray.u = uv.x;
          ray.v = uv.y;
          ray.tfar = hit.vt[i];
          ray.Ng.x = hit.vNg.x[i];
          ray.Ng.y = hit.vNg.y[i];
          ray.Ng.z = hit.vNg.z[i];
          ray.geomID = instID;
          ray.primID = primIDs[i];
          return true;

        }
      };

#if defined(__AVX512F__)
    template<int M, bool filter>
      struct Intersect1EpilogM<M,16,filter>
      {
        static const size_t Mx = 16;
        Ray& ray;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        
        __forceinline Intersect1EpilogM(Ray& ray,
                                        IntersectContext* context, 
                                        const vint<M>& geomIDs, 
                                        const vint<M>& primIDs)
          : ray(ray), context(context), geomIDs(geomIDs), primIDs(primIDs) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<Mx>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;
          vbool<Mx> valid = valid_i;
          if (Mx > M) valid &= (1<<M)-1;
          hit.finalize();          
          size_t i = select_min(valid,hit.vt);
          int geomID = geomIDs[i];
          int instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;

          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER) || defined(EMBREE_RAY_MASK)
          bool foundhit = false;
          goto entry;
          while (true) 
          {
            if (unlikely(none(valid))) return foundhit;
            i = select_min(valid,hit.vt);

            geomID = geomIDs[i];
            instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
          entry:
            Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
            
#if defined(EMBREE_RAY_MASK)
            /* goto next hit if mask test fails */
            if ((geometry->mask & ray.mask) == 0) {
              clear(valid,i);
              continue;
            }
#endif
            
#if defined(EMBREE_INTERSECTION_FILTER) 
            /* call intersection filter function */
            if (filter) {
              if (unlikely(geometry->hasIntersectionFilter1())) {
                const Vec2f uv = hit.uv(i);
                foundhit |= runIntersectionFilter1(geometry,ray,context,uv.x,uv.y,hit.t(i),hit.Ng(i),instID,primIDs[i]);
                clear(valid,i);
                valid &= hit.vt < ray.tfar; // intersection filters may modify tfar value
                continue;
              }
            }
#endif
            break;
          }
#endif

          vbool<Mx> finalMask(((unsigned int)1 << i));
          ray.update(finalMask,hit.vt,hit.vu,hit.vv,hit.vNg.x,hit.vNg.y,hit.vNg.z,instID,primIDs);
          return true;

        }
      };
#endif    
    
    template<int M, int Mx, bool filter>
      struct Occluded1EpilogM
      {
        Ray& ray;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        
        __forceinline Occluded1EpilogM(Ray& ray,
                                       IntersectContext* context, 
                                       const vint<M>& geomIDs, 
                                       const vint<M>& primIDs)
          : ray(ray), context(context), geomIDs(geomIDs), primIDs(primIDs) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<Mx>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;

          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER) || defined(EMBREE_RAY_MASK)
          if (unlikely(filter))
            hit.finalize(); /* called only once */

          vbool<Mx> valid = valid_i;
          if (Mx > M) valid &= (1<<M)-1;
          size_t m=movemask(valid);
          goto entry;
          while (true)
          {  
            if (unlikely(m == 0)) return false;
          entry:
            size_t i=__bsf(m);

            const int geomID = geomIDs[i];
            const int instID = context->geomID_to_instID ? context->geomID_to_instID[0] : geomID;
            Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
            
#if defined(EMBREE_RAY_MASK)
            /* goto next hit if mask test fails */
            if ((geometry->mask & ray.mask) == 0) {
              m=__btc(m,i);
              continue;
            }
#endif
            
#if defined(EMBREE_INTERSECTION_FILTER)
            /* if we have no filter then the test passed */
            if (filter) {
              if (unlikely(geometry->hasOcclusionFilter1())) 
              {
                //const Vec3fa Ngi = Vec3fa(Ng.x[i],Ng.y[i],Ng.z[i]);
                const Vec2f uv = hit.uv(i);
                if (runOcclusionFilter1(geometry,ray,context,uv.x,uv.y,hit.t(i),hit.Ng(i),instID,primIDs[i])) return true;
                m=__btc(m,i);
                continue;
              }
            }
#endif
            break;
          }
#endif
          
          return true;
        }
      };

    
    template<int M, bool filter>
      struct Intersect1EpilogMU
      {
        Ray& ray;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Intersect1EpilogMU(Ray& ray,
                                         IntersectContext* context, 
                                         const unsigned int geomID, 
                                         const unsigned int primID)
          : ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<M>& valid_i, Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask) == 0) return false;
#endif
          
          vbool<M> valid = valid_i;
          hit.finalize();
          
          size_t i = select_min(valid,hit.vt);
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (unlikely(geometry->hasIntersectionFilter1())) 
          {
            bool foundhit = false;
            while (true) 
            {
              /* call intersection filter function */
              Vec2f uv = hit.uv(i);
              foundhit |= runIntersectionFilter1(geometry,ray,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primID);
              clear(valid,i);
              valid &= hit.vt < ray.tfar; // intersection filters may modify tfar value
              if (unlikely(none(valid))) break;
              i = select_min(valid,hit.vt);
            }
            return foundhit;
          }
#endif
          
          /* update hit information */
          const Vec2f uv = hit.uv(i);
          ray.u = uv.x;
          ray.v = uv.y;
          ray.tfar = hit.vt[i];
          const Vec3fa Ng = hit.Ng(i);
          ray.Ng.x = Ng.x;
          ray.Ng.y = Ng.y;
          ray.Ng.z = Ng.z;
          ray.geomID = geomID;
          ray.primID = primID;
          return true;
        }
      };
    
    template<int M, bool filter>
      struct Occluded1EpilogMU
      {
        Ray& ray;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Occluded1EpilogMU(Ray& ray,
                                        IntersectContext* context, 
                                        const unsigned int geomID, 
                                        const unsigned int primID)
          : ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<M>& valid, Hit& hit) const
        {
          /* ray mask test */
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          if ((geometry->mask & ray.mask) == 0) return false;
#endif
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (unlikely(geometry->hasOcclusionFilter1())) 
          {
            hit.finalize();
            for (size_t m=movemask(valid), i=__bsf(m); m!=0; m=__btc(m,i), i=__bsf(m)) 
            {  
              const Vec2f uv = hit.uv(i);
              if (runOcclusionFilter1(geometry,ray,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primID)) return true;
            }
            return false;
          }
#endif
          return true;
        }
      };
        
    template<int M, int K, bool filter>
      struct IntersectKEpilogM
      {
        RayK<K>& ray;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        const size_t i;
        
        __forceinline IntersectKEpilogM(RayK<K>& ray,
                                        IntersectContext* context, 
                                       const vint<M>& geomIDs, 
                                       const vint<M>& primIDs, 
                                       size_t i)
          : ray(ray), context(context), geomIDs(geomIDs), primIDs(primIDs), i(i) {}
        
        template<typename Hit>
        __forceinline vbool<K> operator() (const vbool<K>& valid_i, const Hit& hit) const
        {
          Scene* scene = context->scene;

          vfloat<K> u, v, t; 
          Vec3<vfloat<K>> Ng;
          vbool<K> valid = valid_i;

          std::tie(u,v,t,Ng) = hit();
          
          const int geomID = geomIDs[i];
          const int primID = primIDs[i];
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
          
          /* ray masking test */
#if defined(EMBREE_RAY_MASK)
          valid &= (geometry->mask & ray.mask) != 0;
          if (unlikely(none(valid))) return false;
#endif
          
          /* occlusion filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasIntersectionFilter<vfloat<K>>())) {
              return runIntersectionFilter(valid,geometry,ray,context,u,v,t,Ng,geomID,primID);
            }
          }
#endif
          
          /* update hit information */
          vfloat<K>::store(valid,&ray.u,u);
          vfloat<K>::store(valid,&ray.v,v);
          vfloat<K>::store(valid,&ray.tfar,t);
          vint<K>::store(valid,&ray.geomID,geomID);
          vint<K>::store(valid,&ray.primID,primID);
          vfloat<K>::store(valid,&ray.Ng.x,Ng.x);
          vfloat<K>::store(valid,&ray.Ng.y,Ng.y);
          vfloat<K>::store(valid,&ray.Ng.z,Ng.z);
          return valid;
        }
      };
    
    template<int M, int K, bool filter>
      struct OccludedKEpilogM
      {
        vbool<K>& valid0;
        RayK<K>& ray;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        const size_t i;
        
        __forceinline OccludedKEpilogM(vbool<K>& valid0,
                                       RayK<K>& ray,
                                       IntersectContext* context, 
                                       const vint<M>& geomIDs, 
                                       const vint<M>& primIDs, 
                                       size_t i)
          : valid0(valid0), ray(ray), context(context), geomIDs(geomIDs), primIDs(primIDs), i(i) {}
        
        template<typename Hit>
        __forceinline vbool<K> operator() (const vbool<K>& valid_i, const Hit& hit) const
        {
          vbool<K> valid = valid_i;
          
          /* ray masking test */
          Scene* scene = context->scene;
          const int geomID = geomIDs[i];
          const int primID = primIDs[i];
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          valid &= (geometry->mask & ray.mask) != 0;
          if (unlikely(none(valid))) return valid;
#endif
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasOcclusionFilter<vfloat<K>>()))
            {
              vfloat<K> u, v, t; 
              Vec3<vfloat<K>> Ng;
              std::tie(u,v,t,Ng) = hit();
              valid = runOcclusionFilter(valid,geometry,ray,context,u,v,t,Ng,geomID,primID);
            }
          }
#endif
          
          /* update occlusion */
          valid0 = valid0 & !valid;
          return valid;
        }
      };
    
    template<int M, int K, bool filter>
      struct IntersectKEpilogMU
      {
        RayK<K>& ray;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline IntersectKEpilogMU(RayK<K>& ray,
                                         IntersectContext* context, 
                                         const unsigned int geomID, 
                                         const unsigned int primID)
          : ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline vbool<K> operator() (const vbool<K>& valid_org, const Hit& hit) const
        {
          vbool<K> valid = valid_org;
          vfloat<K> u, v, t; 
          Vec3<vfloat<K>> Ng;
          std::tie(u,v,t,Ng) = hit();
         
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
          
          /* ray masking test */
#if defined(EMBREE_RAY_MASK)
          valid &= (geometry->mask & ray.mask) != 0;
          if (unlikely(none(valid))) return false;
#endif
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasIntersectionFilter<vfloat<K>>())) {
              return runIntersectionFilter(valid,geometry,ray,context,u,v,t,Ng,geomID,primID);
            }
          }
#endif
          
          /* update hit information */
          vfloat<K>::store(valid,&ray.u,u);
          vfloat<K>::store(valid,&ray.v,v);
          vfloat<K>::store(valid,&ray.tfar,t);
          vint<K>::store(valid,&ray.geomID,geomID);
          vint<K>::store(valid,&ray.primID,primID);
          vfloat<K>::store(valid,&ray.Ng.x,Ng.x);
          vfloat<K>::store(valid,&ray.Ng.y,Ng.y);
          vfloat<K>::store(valid,&ray.Ng.z,Ng.z);
          return valid;
        }
      };
    
    template<int M, int K, bool filter>
      struct OccludedKEpilogMU
      {
        vbool<K>& valid0;
        RayK<K>& ray;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline OccludedKEpilogMU(vbool<K>& valid0,
                                        RayK<K>& ray,
                                        IntersectContext* context, 
                                        const unsigned int geomID, 
                                        const unsigned int primID)
          : valid0(valid0), ray(ray), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline vbool<K> operator() (const vbool<K>& valid_i, const Hit& hit) const
        {
          vbool<K> valid = valid_i;
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
          
#if defined(EMBREE_RAY_MASK)
          valid &= (geometry->mask & ray.mask) != 0;
          if (unlikely(none(valid))) return false;
#endif
          
          /* occlusion filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasOcclusionFilter<vfloat<K>>()))
            {
              vfloat<K> u, v, t; 
              Vec3<vfloat<K>> Ng;
              std::tie(u,v,t,Ng) = hit();
              valid = runOcclusionFilter(valid,geometry,ray,context,u,v,t,Ng,geomID,primID);
            }
          }
#endif
          
          /* update occlusion */
          valid0 = valid0 & !valid;
          return valid;
        }
      };
    
    
    
    template<int M, int Mx, int K, bool filter>
      struct Intersect1KEpilogM
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        
        __forceinline Intersect1KEpilogM(RayK<K>& ray, size_t k,
                                         IntersectContext* context, 
                                         const vint<M>& geomIDs, 
                                         const vint<M>& primIDs)
          : ray(ray), k(k), context(context), geomIDs(geomIDs), primIDs(primIDs) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<Mx>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;
          vbool<Mx> valid = valid_i;
          hit.finalize();
          if (Mx > M) valid &= (1<<M)-1;
          size_t i = select_min(valid,hit.vt);
          assert(i<M);
          int geomID = geomIDs[i];
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER) || defined(EMBREE_RAY_MASK)
          bool foundhit = false;
          goto entry;
          while (true) 
          {
            if (unlikely(none(valid))) return foundhit;
            i = select_min(valid,hit.vt);
            assert(i<M);            
            geomID = geomIDs[i];
          entry:
            Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
            
#if defined(EMBREE_RAY_MASK)
            /* goto next hit if mask test fails */
            if ((geometry->mask & ray.mask[k]) == 0) {
              clear(valid,i);
              continue;
            }
#endif
            
#if defined(EMBREE_INTERSECTION_FILTER) 
            /* call intersection filter function */
            if (filter) {
              if (unlikely(geometry->hasIntersectionFilter<vfloat<K>>())) {
                assert(i<M);
                const Vec2f uv = hit.uv(i);
                foundhit = foundhit | runIntersectionFilter(geometry,ray,k,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primIDs[i]);
                clear(valid,i);
                valid &= hit.vt < ray.tfar[k]; // intersection filters may modify tfar value
                continue;
              }
            }
#endif
            break;
          }
#endif
          assert(i<M);
          /* update hit information */
#if defined(__AVX512F__)
          ray.updateK(i,k,hit.vt,hit.vu,hit.vv,vfloat<Mx>(hit.vNg.x),vfloat<Mx>(hit.vNg.y),vfloat<Mx>(hit.vNg.z),geomID,vint<Mx>(primIDs));
#else
          const Vec2f uv = hit.uv(i);
          ray.u[k] = uv.x;
          ray.v[k] = uv.y;
          ray.tfar[k] = hit.vt[i];
          ray.Ng.x[k] = hit.vNg.x[i];
          ray.Ng.y[k] = hit.vNg.y[i];
          ray.Ng.z[k] = hit.vNg.z[i];
          ray.geomID[k] = geomID;
          ray.primID[k] = primIDs[i];
#endif
          return true;
        }
      };
    
    template<int M, int Mx, int K, bool filter>
      struct Occluded1KEpilogM
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const vint<M>& geomIDs;
        const vint<M>& primIDs;
        
        __forceinline Occluded1KEpilogM(RayK<K>& ray, size_t k,
                                        IntersectContext* context, 
                                        const vint<M>& geomIDs, 
                                        const vint<M>& primIDs)
          : ray(ray), k(k), context(context), geomIDs(geomIDs), primIDs(primIDs) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<Mx>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;

          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER) || defined(EMBREE_RAY_MASK)
          if (unlikely(filter))
            hit.finalize(); /* called only once */
          
          vbool<Mx> valid = valid_i;
          if (Mx > M) valid &= (1<<M)-1;
          size_t m=movemask(valid);
          goto entry;
          while (true)
          {  
            if (unlikely(m == 0)) return false;
          entry:
            size_t i=__bsf(m);

            const int geomID = geomIDs[i];
            Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
            
#if defined(EMBREE_RAY_MASK)
            /* goto next hit if mask test fails */
            if ((geometry->mask & ray.mask[k]) == 0) {
              m=__btc(m,i);
              continue;
            }
#endif
            
#if defined(EMBREE_INTERSECTION_FILTER)
            /* execute occlusion filer */
            if (filter) {
              if (unlikely(geometry->hasOcclusionFilter<vfloat<K>>())) 
              {
                const Vec2f uv = hit.uv(i);
                if (runOcclusionFilter(geometry,ray,k,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primIDs[i])) return true;
                m=__btc(m,i);
                continue;
              }
            }
#endif
            break;
          }
#endif
          
          return true;
        }
      };
    
    template<int M, int K, bool filter>
      struct Intersect1KEpilogMU
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Intersect1KEpilogMU(RayK<K>& ray, size_t k,
                                          IntersectContext* context, 
                                          const unsigned int geomID, 
                                          const unsigned int primID)
          : ray(ray), k(k), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<M>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          /* ray mask test */
          if ((geometry->mask & ray.mask[k]) == 0) 
            return false;
#endif

          /* finalize hit calculation */
          vbool<M> valid = valid_i;
          hit.finalize();
          size_t i = select_min(valid,hit.vt);
          
          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasIntersectionFilter<vfloat<K>>())) 
            {
              bool foundhit = false;
              while (true) 
              {
                const Vec2f uv = hit.uv(i);
                foundhit = foundhit | runIntersectionFilter(geometry,ray,k,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primID);
                clear(valid,i);
                valid &= hit.vt < ray.tfar[k]; // intersection filters may modify tfar value
                if (unlikely(none(valid))) break;
                i = select_min(valid,hit.vt);
              }
              return foundhit;
            }
          }
#endif
          
          /* update hit information */
#if defined(__AVX512F__)
          const Vec3fa Ng = hit.Ng(i);
          ray.updateK(i,k,hit.vt,hit.vu,hit.vv,vfloat<M>(Ng.x),vfloat<M>(Ng.y),vfloat<M>(Ng.z),geomID,vint<M>(primID));
#else
          const Vec2f uv = hit.uv(i);
          ray.u[k] = uv.x;
          ray.v[k] = uv.y;
          ray.tfar[k] = hit.vt[i];
          const Vec3fa Ng = hit.Ng(i);
          ray.Ng.x[k] = Ng.x;
          ray.Ng.y[k] = Ng.y;
          ray.Ng.z[k] = Ng.z;
          ray.geomID[k] = geomID;
          ray.primID[k] = primID;
#endif
          return true;
        }
      };
    
    template<int M, int K, bool filter>
      struct Occluded1KEpilogMU
      {
        RayK<K>& ray;
        size_t k;
        IntersectContext* context;
        const unsigned int geomID;
        const unsigned int primID;
        
        __forceinline Occluded1KEpilogMU(RayK<K>& ray, size_t k,
                                         IntersectContext* context, 
                                         const unsigned int geomID, 
                                         const unsigned int primID)
          : ray(ray), k(k), context(context), geomID(geomID), primID(primID) {}
        
        template<typename Hit>
        __forceinline bool operator() (const vbool<M>& valid_i, Hit& hit) const
        {
          Scene* scene = context->scene;
          Geometry* geometry MAYBE_UNUSED = scene->get(geomID);
#if defined(EMBREE_RAY_MASK)
          /* ray mask test */
          if ((geometry->mask & ray.mask[k]) == 0) 
            return false;
#endif

          /* intersection filter test */
#if defined(EMBREE_INTERSECTION_FILTER)
          if (filter) {
            if (unlikely(geometry->hasOcclusionFilter<vfloat<K>>())) 
            {
              hit.finalize();
              for (size_t m=movemask(valid_i), i=__bsf(m); m!=0; m=__btc(m,i), i=__bsf(m))
              {  
                const Vec2f uv = hit.uv(i);
                if (runOcclusionFilter(geometry,ray,k,context,uv.x,uv.y,hit.t(i),hit.Ng(i),geomID,primID)) return true;
              }
              return false;
            }
          }
#endif 
          return true;
        }
      };
  }
}
