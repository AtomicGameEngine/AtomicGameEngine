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

#include "../common/geometry.h"
#include "../common/ray.h"
#include "../common/hit.h"
#include "../common/context.h"

namespace embree
{
  namespace isa
  {
#if defined(__SSE__)
    typedef void (*ISPCFilterFunc4)(void* ptr, RTCRay4& ray, __m128i valid);
#endif
    
#if defined(__AVX__)
  typedef void (*ISPCFilterFunc8)(void* ptr, RTCRay8& ray, __m256i valid);
#endif

#if defined(__AVX512F__)
  typedef void (*ISPCFilterFunc16)(void* ptr, RTCRay16& ray, __m128i valid); // mask passed as 16 bytes
#endif

    __forceinline bool runIntersectionFilter1(const Geometry* const geometry, Ray& ray, IntersectContext* context,
                                              const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      if (likely(geometry->intersectionFilter1)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const float  ray_tfar = ray.tfar;
        const Vec3fa ray_Ng   = ray.Ng;
        const vfloat4   ray_uv_ids = *(vfloat4*)&ray.u;
#endif
        ray.u = u;
        ray.v = v;
        ray.tfar = t;
        ray.geomID = geomID;
        ray.primID = primID;
        ray.Ng = Ng;
        
        /* invoke filter function */
        AVX_ZERO_UPPER(); geometry->intersectionFilter1(geometry->userPtr,(RTCRay&)ray);

        /* restore hit if filter not passed */
        if (unlikely(ray.geomID == RTC_INVALID_GEOMETRY_ID)) 
        {
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
          ray.tfar = ray_tfar;
          ray.Ng = ray_Ng;
          *(vfloat4*)&ray.u = ray_uv_ids;
#endif
          return false;
        }
        return true;
      }
      else
      {
        assert(geometry->intersectionFilterN);
        int mask = -1; Hit hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN(&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,1);
        return mask != 0;
      }
    }
    
    __forceinline bool runOcclusionFilter1(const Geometry* const geometry, Ray& ray, IntersectContext* context,
                                           const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      if (likely(geometry->occlusionFilter1)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const float ray_tfar = ray.tfar;
        const int   ray_geomID = ray.geomID;
#endif
        ray.u = u;
        ray.v = v;
        ray.tfar = t;
        ray.geomID = geomID;
        ray.primID = primID;
        ray.Ng = Ng;
        
        /* invoke filter function */
        AVX_ZERO_UPPER(); geometry->occlusionFilter1(geometry->userPtr,(RTCRay&)ray);
      
        /* restore hit if filter not passed */
        if (unlikely(ray.geomID == RTC_INVALID_GEOMETRY_ID)) 
        {
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
          ray.tfar = ray_tfar;
          ray.geomID = ray_geomID;
#endif
          return false;
        }
        return true;
      }
      else 
      {
        assert(geometry->occlusionFilterN);
        int mask = -1; Hit hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN(&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,1);
        return mask != 0;
      }
    }

    __forceinline vbool4 runIntersectionFilter(const vbool4& valid, const Geometry* const geometry, Ray4& ray, IntersectContext* context,
                                               const vfloat4& u, const vfloat4& v, const vfloat4& t, const Vec3vf4& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc4  filter4 = geometry->intersectionFilter4;
      if (likely(filter4)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat4 ray_u = ray.u;           vfloat4::store(valid,&ray.u,u);
        const vfloat4 ray_v = ray.v;           vfloat4::store(valid,&ray.v,v);
        const vfloat4 ray_tfar = ray.tfar;     vfloat4::store(valid,&ray.tfar,t);
        const vint4 ray_geomID = ray.geomID;   vint4::store(valid,&ray.geomID,geomID);
        const vint4 ray_primID = ray.primID;   vint4::store(valid,&ray.primID,primID);
        const vfloat4 ray_Ng_x = ray.Ng.x;     vfloat4::store(valid,&ray.Ng.x,Ng.x);
        const vfloat4 ray_Ng_y = ray.Ng.y;     vfloat4::store(valid,&ray.Ng.y,Ng.y);
        const vfloat4 ray_Ng_z = ray.Ng.z;     vfloat4::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */
        AVX_ZERO_UPPER();
        if (geometry->hasISPCIntersectionFilter<vfloat4>()) ((ISPCFilterFunc4)filter4)(geometry->userPtr,(RTCRay4&)ray,valid.mask32());
        else { const vint4 mask = valid.mask32(); filter4(&mask,geometry->userPtr,(RTCRay4&)ray); }
      
        const vbool4 valid_failed = valid & (ray.geomID == vint4(-1));
        const vbool4 valid_passed = valid & (ray.geomID != vint4(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(any(valid_failed))) 
        {
          vfloat4::store(valid_failed,&ray.u,ray_u);
          vfloat4::store(valid_failed,&ray.v,ray_v);
          vfloat4::store(valid_failed,&ray.tfar,ray_tfar);
          vint4::store(valid_failed,&ray.geomID,ray_geomID);
          vint4::store(valid_failed,&ray.primID,ray_primID);
          vfloat4::store(valid_failed,&ray.Ng.x,ray_Ng_x);
          vfloat4::store(valid_failed,&ray.Ng.y,ray_Ng_y);
          vfloat4::store(valid_failed,&ray.Ng.z,ray_Ng_z);
        }
#endif
        return valid_passed;
      }
      else 
      {
        assert(geometry->intersectionFilterN);
        vint4 mask = valid.mask32(); Hit4 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4);
        return mask != vint4(0);
      }
  }
    
    __forceinline vbool4 runOcclusionFilter(const vbool4& valid, const Geometry* const geometry, Ray4& ray, IntersectContext* context,
                                            const vfloat4& u, const vfloat4& v, const vfloat4& t, const Vec3vf4& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc4 filter4 = geometry->occlusionFilter4;
      if (likely(filter4)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const vfloat4 ray_tfar = ray.tfar; 
        const vint4 ray_geomID = ray.geomID;
#endif
        vfloat4::store(valid,&ray.u,u);
        vfloat4::store(valid,&ray.v,v);
        vfloat4::store(valid,&ray.tfar,t);
        vint4::store(valid,&ray.geomID,geomID);
        vint4::store(valid,&ray.primID,primID);
        vfloat4::store(valid,&ray.Ng.x,Ng.x);
        vfloat4::store(valid,&ray.Ng.y,Ng.y);
        vfloat4::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */
        AVX_ZERO_UPPER();
        if (geometry->hasISPCOcclusionFilter<vfloat4>()) ((ISPCFilterFunc4)filter4)(geometry->userPtr,(RTCRay4&)ray,valid.mask32());
        else { const vint4 mask = valid.mask32(); filter4(&mask,geometry->userPtr,(RTCRay4&)ray); }
      
        const vbool4 valid_failed = valid & (ray.geomID == vint4(-1));
        const vbool4 valid_passed = valid & (ray.geomID != vint4(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        vfloat4::store(valid_failed,&ray.tfar,ray_tfar);
        vint4::store(valid_failed,&ray.geomID,ray_geomID);
#endif
        return valid_passed;
      }
      else 
      {
        assert(geometry->occlusionFilterN);
        vint4 mask = valid.mask32(); Hit4 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4);
        return mask != vint4(0);
      }
    }
    
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray4& ray, const size_t k, IntersectContext* context,
                                             const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool4 valid(1 << k);
      RTCFilterFunc4  filter4 = geometry->intersectionFilter4;
      if (likely(filter4)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat4 ray_u = ray.u;           ray.u[k] = u;
        const vfloat4 ray_v = ray.v;           ray.v[k] = v;
        const vfloat4 ray_tfar = ray.tfar;     ray.tfar[k] = t;
        const vint4 ray_geomID = ray.geomID;   ray.geomID[k] = geomID;
        const vint4 ray_primID = ray.primID;   ray.primID[k] = primID;
        const vfloat4 ray_Ng_x = ray.Ng.x;     ray.Ng.x[k] = Ng.x;
        const vfloat4 ray_Ng_y = ray.Ng.y;     ray.Ng.y[k] = Ng.y;
        const vfloat4 ray_Ng_z = ray.Ng.z;     ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        AVX_ZERO_UPPER();
        if (geometry->hasISPCIntersectionFilter<vfloat4>()) ((ISPCFilterFunc4)filter4)(geometry->userPtr,(RTCRay4&)ray,valid.mask32());
        else { const vint4 mask = valid.mask32(); filter4(&mask,geometry->userPtr,(RTCRay4&)ray); }
      
        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat4::store(&ray.u,ray_u);
          vfloat4::store(&ray.v,ray_v);
          vfloat4::store(&ray.tfar,ray_tfar);
          vint4::store(&ray.geomID,ray_geomID);
          vint4::store(&ray.primID,ray_primID);
          vfloat4::store(&ray.Ng.x,ray_Ng_x);
          vfloat4::store(&ray.Ng.y,ray_Ng_y);
          vfloat4::store(&ray.Ng.z,ray_Ng_z);
        }
#endif
        return passed;
      }
      else 
      {
        assert(geometry->intersectionFilterN);
        vint4 mask = valid.mask32(); Hit4 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4);
        return mask[k] != 0;
      }
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray4& ray, const size_t k, IntersectContext* context,
                                          const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool4 valid(1 << k);
      RTCFilterFunc4  filter4 = geometry->occlusionFilter4;
      if (likely(filter4)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat4 ray_tfar = ray.tfar; 
        const vint4 ray_geomID = ray.geomID;
        ray.u[k] = u;
        ray.v[k] = v;
        ray.tfar[k] = t;
        ray.geomID[k] = geomID;
        ray.primID[k] = primID;
        ray.Ng.x[k] = Ng.x;
        ray.Ng.y[k] = Ng.y;
        ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        AVX_ZERO_UPPER();
        if (geometry->hasISPCOcclusionFilter<vfloat4>()) ((ISPCFilterFunc4)filter4)(geometry->userPtr,(RTCRay4&)ray,valid.mask32());
        else { const vint4 mask = valid.mask32(); filter4(&mask,geometry->userPtr,(RTCRay4&)ray); }
        
        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat4::store(&ray.tfar,ray_tfar);
          vint4::store(&ray.geomID,ray_geomID);
        }
#endif
        return passed;
      }
      else 
      {
        assert(geometry->occlusionFilterN);
        vint4 mask = valid.mask32(); Hit4 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4);
        return mask[k] != 0;
      }
    }
    
#if defined(__AVX__)
    __forceinline vbool8 runIntersectionFilter(const vbool8& valid, const Geometry* const geometry, Ray8& ray, IntersectContext* context,
                                               const vfloat8& u, const vfloat8& v, const vfloat8& t, const Vec3vf8& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc8  filter8 = geometry->intersectionFilter8;    
      if (likely(filter8)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat8 ray_u = ray.u;           vfloat8::store(valid,&ray.u,u);
        const vfloat8 ray_v = ray.v;           vfloat8::store(valid,&ray.v,v);
        const vfloat8 ray_tfar = ray.tfar;     vfloat8::store(valid,&ray.tfar,t);
        const vint8 ray_geomID = ray.geomID;   vint8::store(valid,&ray.geomID,geomID);
        const vint8 ray_primID = ray.primID;   vint8::store(valid,&ray.primID,primID);
        const vfloat8 ray_Ng_x = ray.Ng.x;     vfloat8::store(valid,&ray.Ng.x,Ng.x);
        const vfloat8 ray_Ng_y = ray.Ng.y;     vfloat8::store(valid,&ray.Ng.y,Ng.y);
        const vfloat8 ray_Ng_z = ray.Ng.z;     vfloat8::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */    
        if (geometry->hasISPCIntersectionFilter<vfloat8>()) ((ISPCFilterFunc8)filter8)(geometry->userPtr,(RTCRay8&)ray,valid.mask32());
        else { const vint8 mask = valid.mask32(); filter8(&mask,geometry->userPtr,(RTCRay8&)ray); }
        
        const vbool8 valid_failed = valid & (ray.geomID == vint8(-1));
        const vbool8 valid_passed = valid & (ray.geomID != vint8(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(any(valid_failed))) 
        {
          vfloat8::store(valid_failed,&ray.u,ray_u);
          vfloat8::store(valid_failed,&ray.v,ray_v);
          vfloat8::store(valid_failed,&ray.tfar,ray_tfar);
          vint8::store(valid_failed,&ray.geomID,ray_geomID);
          vint8::store(valid_failed,&ray.primID,ray_primID);
          vfloat8::store(valid_failed,&ray.Ng.x,ray_Ng_x);
          vfloat8::store(valid_failed,&ray.Ng.y,ray_Ng_y);
          vfloat8::store(valid_failed,&ray.Ng.z,ray_Ng_z);
        }
#endif
        return valid_passed;
      }
      else 
      {
        assert(geometry->intersectionFilterN);
        vint8 mask = valid.mask32(); Hit8 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8);
        return mask != vint8(0);
      }
    }
    
    __forceinline vbool8 runOcclusionFilter(const vbool8& valid, const Geometry* const geometry, Ray8& ray, IntersectContext* context,
                                            const vfloat8& u, const vfloat8& v, const vfloat8& t, const Vec3vf8& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc8 filter8 = geometry->occlusionFilter8;
      if (likely(filter8)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const vfloat8 ray_tfar = ray.tfar; 
        const vint8 ray_geomID = ray.geomID;
#endif
        vfloat8::store(valid,&ray.u,u);
        vfloat8::store(valid,&ray.v,v);
        vfloat8::store(valid,&ray.tfar,t);
        vint8::store(valid,&ray.geomID,geomID);
        vint8::store(valid,&ray.primID,primID);
        vfloat8::store(valid,&ray.Ng.x,Ng.x);
        vfloat8::store(valid,&ray.Ng.y,Ng.y);
        vfloat8::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */
        if (geometry->hasISPCOcclusionFilter<vfloat8>()) ((ISPCFilterFunc8)filter8)(geometry->userPtr,(RTCRay8&)ray,valid.mask32());
        else { const vint8 mask = valid.mask32(); filter8(&mask,geometry->userPtr,(RTCRay8&)ray); }
        
        const vbool8 valid_failed = valid & (ray.geomID == vint8(-1));
        const vbool8 valid_passed = valid & (ray.geomID != vint8(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        vfloat8::store(valid_failed,&ray.tfar,ray_tfar);
        vint8::store(valid_failed,&ray.geomID,ray_geomID);
#endif
        return valid_passed;
      }
      else 
      {
        assert(geometry->occlusionFilterN);
        vint8 mask = valid.mask32(); Hit8 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8);
        return mask != vint8(0);
      }
    }
    
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray8& ray, const size_t k, IntersectContext* context,
                                             const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool8 valid(1 << k);
      RTCFilterFunc8  filter8 = geometry->intersectionFilter8;
      if (likely(filter8)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat8 ray_u = ray.u;           ray.u[k] = u;
        const vfloat8 ray_v = ray.v;           ray.v[k] = v;
        const vfloat8 ray_tfar = ray.tfar;     ray.tfar[k] = t;
        const vint8 ray_geomID = ray.geomID;   ray.geomID[k] = geomID;
        const vint8 ray_primID = ray.primID;   ray.primID[k] = primID;
        const vfloat8 ray_Ng_x = ray.Ng.x;     ray.Ng.x[k] = Ng.x;
        const vfloat8 ray_Ng_y = ray.Ng.y;     ray.Ng.y[k] = Ng.y;
        const vfloat8 ray_Ng_z = ray.Ng.z;     ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        if (geometry->hasISPCIntersectionFilter<vfloat8>()) ((ISPCFilterFunc8)filter8)(geometry->userPtr,(RTCRay8&)ray,valid.mask32());
        else { const vint8 mask = valid.mask32(); filter8(&mask,geometry->userPtr,(RTCRay8&)ray); }
        
        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat8::store(&ray.u,ray_u);
          vfloat8::store(&ray.v,ray_v);
          vfloat8::store(&ray.tfar,ray_tfar);
          vint8::store(&ray.geomID,ray_geomID);
          vint8::store(&ray.primID,ray_primID);
          vfloat8::store(&ray.Ng.x,ray_Ng_x);
          vfloat8::store(&ray.Ng.y,ray_Ng_y);
          vfloat8::store(&ray.Ng.z,ray_Ng_z);
        }
#endif
        return passed;
      }
      else 
      {
        assert(geometry->intersectionFilterN);
        vint8 mask = valid.mask32(); Hit8 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8);
        return mask[k] != 0;
      }
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray8& ray, const size_t k, IntersectContext* context,
                                          const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool8 valid(1 << k);
      RTCFilterFunc8 filter8 = geometry->occlusionFilter8;
      if (likely(filter8)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const vfloat8 ray_tfar = ray.tfar; 
        const vint8 ray_geomID = ray.geomID;
#endif
        ray.u[k] = u;
        ray.v[k] = v;
        ray.tfar[k] = t;
        ray.geomID[k] = geomID;
        ray.primID[k] = primID;
        ray.Ng.x[k] = Ng.x;
        ray.Ng.y[k] = Ng.y;
        ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        if (geometry->hasISPCOcclusionFilter<vfloat8>()) ((ISPCFilterFunc8)filter8)(geometry->userPtr,(RTCRay8&)ray,valid.mask32());
        else { const vint8 mask = valid.mask32(); filter8(&mask,geometry->userPtr,(RTCRay8&)ray); }

        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat8::store(&ray.tfar,ray_tfar);
          vint8::store(&ray.geomID,ray_geomID);
        }
#endif
        return passed;
      }
      else 
      {
        assert(geometry->occlusionFilterN);
        vint8 mask = valid.mask32(); Hit8 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8);
        return mask[k] != 0;
      }
    }
    
#endif


#if defined(__AVX512F__)
    __forceinline vbool16 runIntersectionFilter(const vbool16& valid, const Geometry* const geometry, Ray16& ray, IntersectContext* context,
                                                const vfloat16& u, const vfloat16& v, const vfloat16& t, const Vec3vf16& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc16  filter16 = geometry->intersectionFilter16;
      if (likely(filter16)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat16 ray_u = ray.u;           vfloat16::store(valid,&ray.u,u);
        const vfloat16 ray_v = ray.v;           vfloat16::store(valid,&ray.v,v);
        const vfloat16 ray_tfar = ray.tfar;     vfloat16::store(valid,&ray.tfar,t);
        const vint16 ray_geomID = ray.geomID;   vint16::store(valid,&ray.geomID,geomID);
        const vint16 ray_primID = ray.primID;   vint16::store(valid,&ray.primID,primID);
        const vfloat16 ray_Ng_x = ray.Ng.x;     vfloat16::store(valid,&ray.Ng.x,Ng.x);
        const vfloat16 ray_Ng_y = ray.Ng.y;     vfloat16::store(valid,&ray.Ng.y,Ng.y);
        const vfloat16 ray_Ng_z = ray.Ng.z;     vfloat16::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */
        if (geometry->hasISPCIntersectionFilter<vfloat16>()) ((ISPCFilterFunc16)filter16)(geometry->userPtr,(RTCRay16&)ray,valid.mask8());
        else { const vint16 mask = valid.mask32(); filter16(&mask,geometry->userPtr,(RTCRay16&)ray); }

        const vbool16 valid_failed = valid & (ray.geomID == vint16(-1));
        const vbool16 valid_passed = valid & (ray.geomID != vint16(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(any(valid_failed))) 
        {
          vfloat16::store(valid_failed,&ray.u,ray_u);
          vfloat16::store(valid_failed,&ray.v,ray_v);
          vfloat16::store(valid_failed,&ray.tfar,ray_tfar);
          vint16::store(valid_failed,&ray.geomID,ray_geomID);
          vint16::store(valid_failed,&ray.primID,ray_primID);
          vfloat16::store(valid_failed,&ray.Ng.x,ray_Ng_x);
          vfloat16::store(valid_failed,&ray.Ng.y,ray_Ng_y);
          vfloat16::store(valid_failed,&ray.Ng.z,ray_Ng_z);
        }
#endif
        return valid_passed;
      }
      else 
      {
        assert(geometry->intersectionFilterN);
        vint16 mask = valid.mask32(); Hit16 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16);
        return mask != vint16(0);
      }
    }
    
    __forceinline vbool16 runOcclusionFilter(const vbool16& valid, const Geometry* const geometry, Ray16& ray, IntersectContext* context,
                                             const vfloat16& u, const vfloat16& v, const vfloat16& t, const Vec3vf16& Ng, const int geomID, const int primID)
    {
      RTCFilterFunc16 filter16 = geometry->occlusionFilter16;
      if (likely(filter16)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const vfloat16 ray_tfar = ray.tfar; 
        const vint16 ray_geomID = ray.geomID;
#endif
        vfloat16::store(valid,&ray.u,u);
        vfloat16::store(valid,&ray.v,v);
        vfloat16::store(valid,&ray.tfar,t);
        vint16::store(valid,&ray.geomID,geomID);
        vint16::store(valid,&ray.primID,primID);
        vfloat16::store(valid,&ray.Ng.x,Ng.x);
        vfloat16::store(valid,&ray.Ng.y,Ng.y);
        vfloat16::store(valid,&ray.Ng.z,Ng.z);
        
        /* invoke filter function */
        if (geometry->hasISPCOcclusionFilter<vfloat16>()) ((ISPCFilterFunc16)filter16)(geometry->userPtr,(RTCRay16&)ray,valid.mask8());
        else { const vint16 mask = valid.mask32(); filter16(&mask,geometry->userPtr,(RTCRay16&)ray); }

        const vbool16 valid_failed = valid & (ray.geomID == vint16(-1));
        const vbool16 valid_passed = valid & (ray.geomID != vint16(-1));
        
        /* restore hit if filter not passed */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        vfloat16::store(valid_failed,&ray.tfar,ray_tfar);
        vint16::store(valid_failed,&ray.geomID,ray_geomID);
#endif
        return valid_passed;
      }
      else
      {
        assert(geometry->occlusionFilterN);
        vint16 mask = valid.mask32(); Hit16 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16);
        return mask != vint16(0);
      }
    }
      
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray16& ray, const size_t k, IntersectContext* context,
                                             const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool16 valid(1 << k);
      RTCFilterFunc16  filter16 = geometry->intersectionFilter16;
      if (likely(filter16)) // old code for compatibility
      {
        /* temporarily update hit information */
        const vfloat16 ray_u = ray.u;           ray.u[k] = u;
        const vfloat16 ray_v = ray.v;           ray.v[k] = v;
        const vfloat16 ray_tfar = ray.tfar;     ray.tfar[k] = t;
        const vint16 ray_geomID = ray.geomID;   ray.geomID[k] = geomID;
        const vint16 ray_primID = ray.primID;   ray.primID[k] = primID;
        const vfloat16 ray_Ng_x = ray.Ng.x;     ray.Ng.x[k] = Ng.x;
        const vfloat16 ray_Ng_y = ray.Ng.y;     ray.Ng.y[k] = Ng.y;
        const vfloat16 ray_Ng_z = ray.Ng.z;     ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        if (geometry->hasISPCIntersectionFilter<vfloat16>()) ((ISPCFilterFunc16)filter16)(geometry->userPtr,(RTCRay16&)ray,valid.mask8());
        else { const vint16 mask = valid.mask32(); filter16(&mask,geometry->userPtr,(RTCRay16&)ray); }
        
        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat16::store(&ray.u,ray_u);
          vfloat16::store(&ray.v,ray_v);
          vfloat16::store(&ray.tfar,ray_tfar);
          vint16::store(&ray.geomID,ray_geomID);
          vint16::store(&ray.primID,ray_primID);
          vfloat16::store(&ray.Ng.x,ray_Ng_x);
          vfloat16::store(&ray.Ng.y,ray_Ng_y);
          vfloat16::store(&ray.Ng.z,ray_Ng_z);
        }
#endif
        return passed;
      }
      else
      {
        assert(geometry->intersectionFilterN);
        vint16 mask = valid.mask32(); Hit16 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16);
        return mask[k] != 0;
      }
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray16& ray, const size_t k, IntersectContext* context,
                                          const float& u, const float& v, const float& t, const Vec3fa& Ng, const int geomID, const int primID)
    {
      const vbool16 valid(1 << k);
      RTCFilterFunc16 filter16 = geometry->occlusionFilter16;
      if (likely(filter16)) // old code for compatibility
      {
        /* temporarily update hit information */
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        const vfloat16 ray_tfar = ray.tfar; 
        const vint16 ray_geomID = ray.geomID;
#endif
        ray.u[k] = u;
        ray.v[k] = v;
        ray.tfar[k] = t;
        ray.geomID[k] = geomID;
        ray.primID[k] = primID;
        ray.Ng.x[k] = Ng.x;
        ray.Ng.y[k] = Ng.y;
        ray.Ng.z[k] = Ng.z;
        
        /* invoke filter function */
        if (geometry->hasISPCOcclusionFilter<vfloat16>()) ((ISPCFilterFunc16)filter16)(geometry->userPtr,(RTCRay16&)ray,valid.mask8());
        else { const vint16 mask = valid.mask32(); filter16(&mask,geometry->userPtr,(RTCRay16&)ray); }
        
        /* restore hit if filter not passed */
        const bool passed = ray.geomID[k] != -1;
#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
        if (unlikely(!passed)) {
          vfloat16::store(&ray.tfar,ray_tfar);
          vint16::store(&ray.geomID,ray_geomID);
        }
#endif
        return passed;
      }
      else
      {
        assert(geometry->occlusionFilterN);
        vint16 mask = valid.mask32(); Hit16 hit(ray.instID,geomID,primID,u,v,t,Ng);
        AVX_ZERO_UPPER(); geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16);
        return mask[k] != 0;
      }
    }    
#endif

  }
}
