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

#include "default.h"
#include "builder.h"
#include "geometry.h"
#include "ray.h"

namespace embree
{
  /*! Base class for set of acceleration structures. */
  class AccelSet : public Geometry
  {
    ALIGNED_CLASS;
  public:

    /*! type of this geometry */
    static const Geometry::Type geom_type = Geometry::USER_GEOMETRY;

    typedef RTCIntersectFunc IntersectFunc;
    typedef RTCIntersectFunc4 IntersectFunc4;
    typedef RTCIntersectFunc8 IntersectFunc8;
    typedef RTCIntersectFunc16 IntersectFunc16;
    typedef RTCIntersectFunc1Mp IntersectFunc1M;
    typedef RTCIntersectFuncN IntersectFuncN;
    
    typedef RTCOccludedFunc OccludedFunc;
    typedef RTCOccludedFunc4 OccludedFunc4;
    typedef RTCOccludedFunc8 OccludedFunc8;
    typedef RTCOccludedFunc16 OccludedFunc16;
    typedef RTCOccludedFunc1Mp OccludedFunc1M;
    typedef RTCOccludedFuncN OccludedFuncN;

#if defined(__SSE__)
    typedef void (*ISPCIntersectFunc4)(void* ptr, RTCRay4& ray, size_t item, __m128i valid);
    typedef void (*ISPCOccludedFunc4 )(void* ptr, RTCRay4& ray, size_t item, __m128i valid);
#endif

#if defined(__AVX__)
    typedef void (*ISPCIntersectFunc8)(void* ptr, RTCRay8& ray, size_t item, __m256i valid);
    typedef void (*ISPCOccludedFunc8 )(void* ptr, RTCRay8& ray, size_t item, __m256i valid);
#endif

#if defined(__AVX512F__)
    typedef void (*ISPCIntersectFunc16)(void* ptr, RTCRay16& ray, size_t item, __m128i valid); // mask gets passed as 16 bytes
    typedef void (*ISPCOccludedFunc16 )(void* ptr, RTCRay16& ray, size_t item, __m128i valid); // mask gets passed as 16 bytes
#endif

    typedef void (*ErrorFunc) ();

    struct Intersector1
    {
      Intersector1 (ErrorFunc error = nullptr);
      Intersector1 (IntersectFunc intersect, OccludedFunc occluded, const char* name);

      operator bool() const { return name; }
        
      public:
        static const char* type;
        IntersectFunc intersect;
        OccludedFunc occluded;  
        const char* name;
      };
      
      struct Intersector4 
      {
        Intersector4 (ErrorFunc error = nullptr);
        Intersector4 (void* intersect, void* occluded, const char* name, bool ispc);
	
        operator bool() const { return name; }
        
      public:
        static const char* type;
        void* intersect;
        void* occluded;
        const char* name;
	bool ispc;
      };
      
      struct Intersector8 
      {
        Intersector8 (ErrorFunc error = nullptr);
        Intersector8 (void* intersect, void* occluded, const char* name, bool ispc);
        
        operator bool() const { return name; }
        
      public:
        static const char* type;
        void* intersect;
        void* occluded;
        const char* name;
	bool ispc;
      };
      
      struct Intersector16 
      {
        Intersector16 (ErrorFunc error = nullptr);
        Intersector16 (void* intersect, void* occluded, const char* name, bool ispc);
        
        operator bool() const { return name; }
        
      public:
        static const char* type;
        void* intersect;
        void* occluded;
        const char* name;
	bool ispc;
      };

      struct Intersector1M
      {
        Intersector1M (ErrorFunc error = nullptr);
        Intersector1M (IntersectFunc1M intersect, OccludedFunc1M occluded, const char* name);
        
        operator bool() const { return name; }
        
      public:
        static const char* type;
        IntersectFunc1M intersect;
        OccludedFunc1M occluded;
        const char* name;
      };

      struct IntersectorN
      {
        IntersectorN (ErrorFunc error = nullptr) ;
        IntersectorN (IntersectFuncN intersect, OccludedFuncN occluded, const char* name);
        
        operator bool() const { return name; }
        
      public:
        static const char* type;
        IntersectFuncN intersect;
        OccludedFuncN occluded; 
        const char* name;
      };
      
    public:
      
      /*! construction */
      AccelSet (Scene* parent, RTCGeometryFlags gflags, size_t items, size_t numTimeSteps);
      
      /*! makes the acceleration structure immutable */
      virtual void immutable () {}
      
      /*! build accel */
      virtual void build () = 0;

      /*! Calculates the bounds of an item */
      __forceinline BBox3fa bounds(size_t i, size_t itime = 0) const
      {
        BBox3fa box[2]; // have to always use 2 boxes as the geometry might have motion blur
        assert(i < size());
        if      (likely(boundsFunc3)) boundsFunc3(boundsFuncUserPtr,intersectors.ptr,i,itime,(RTCBounds&)box[0]);
        else if (likely(boundsFunc2)) boundsFunc2(boundsFuncUserPtr,intersectors.ptr,i,(RTCBounds*)box);
        else                          boundsFunc (intersectors.ptr,i,(RTCBounds&)box[0]);
        return box[0];
      }

      /*! calculates the linear bounds of the i'th item at the itime'th time segment */
      __forceinline LBBox3fa linearBounds(size_t i, size_t itime) const
      {
        BBox3fa box[2]; 
        assert(i < size());
        if (likely(boundsFunc3)) {
          boundsFunc3(boundsFuncUserPtr,intersectors.ptr,i,itime+0,(RTCBounds&)box[0]);
          boundsFunc3(boundsFuncUserPtr,intersectors.ptr,i,itime+1,(RTCBounds&)box[1]);
        }
        else if (likely(boundsFunc2))
          boundsFunc2(boundsFuncUserPtr,intersectors.ptr,i,(RTCBounds*)box);
        else                  
          boundsFunc(intersectors.ptr,i,(RTCBounds&)box[0]);
        return LBBox3fa(box[0],box[1]);
      }

      /*! calculates the linear bounds of the i'th item at the itimeGlobal'th time segment */
      __forceinline LBBox3fa linearBounds(size_t i, size_t itimeGlobal, size_t numTimeStepsGlobal) const
      {
        return Geometry::linearBounds([&] (size_t itime) { return bounds(i, itime); },
                                      itimeGlobal, numTimeStepsGlobal, numTimeSteps);
      }

      /*! calculates the build bounds of the i'th item, if it's valid */
      __forceinline bool buildBounds(size_t i, BBox3fa* bbox = nullptr) const
      {
        const BBox3fa b = bounds(i);
        if (bbox) *bbox = b;
        return isvalid(b);
      }

      /*! calculates the build bounds of the i'th item at the itime'th time segment, if it's valid */
      __forceinline bool buildBounds(size_t i, size_t itime, BBox3fa& bbox) const
      {
        const BBox3fa bounds0 = bounds(i,itime+0);
        const BBox3fa bounds1 = bounds(i,itime+1);
        bbox = bounds0; // use bounding box of first timestep to build BVH
        return isvalid(bounds0) && isvalid(bounds1);
      }

      /*! calculates the build bounds of the i'th item at the itimeGlobal'th time segment, if it's valid */
      __forceinline bool buildBounds(size_t i, size_t itimeGlobal, size_t numTimeStepsGlobal, BBox3fa& bbox) const
      {
        return Geometry::buildBounds([&] (size_t itime, BBox3fa& bbox) -> bool
                                     {
                                       bbox = bounds(i, itime);
                                       return isvalid(bbox);
                                     },
                                     itimeGlobal, numTimeStepsGlobal, numTimeSteps, bbox);
      }
      
      void enabling ();
      void disabling();

  public:

      /*! Intersects a single ray with the scene. */
      __forceinline void intersect (Ray& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (likely(intersectors.intersector1.intersect)) { // old code for compatibility
          intersectors.intersector1.intersect(intersectors.ptr,(RTCRay&)ray,item);
        } else if (likely(intersectors.intersector1M.intersect)) {
          RTCRay* pray = (RTCRay*)&ray;
          intersectors.intersector1M.intersect(intersectors.ptr,context->user,&pray,1,item);
        } else {
          int mask = -1;
          assert(intersectors.intersectorN.intersect);
          intersectors.intersectorN.intersect((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,1,item);
        }
      }
   
      /*! Intersects a packet of 4 rays with the scene. */
#if defined(__SSE__)   
      __forceinline void intersect (const vbool4& valid, Ray4& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (likely(intersectors.intersector4.intersect)) { // old code for compatibility
          if (intersectors.intersector4.ispc) {
            ((ISPCIntersectFunc4)intersectors.intersector4.intersect)(intersectors.ptr,(RTCRay4&)ray,item,valid.mask32());
          } else {
            vint4 mask = valid.mask32();
            ((IntersectFunc4)intersectors.intersector4.intersect)(&mask,intersectors.ptr,(RTCRay4&)ray,item);
          }
        } else {
          vint4 mask = valid.mask32();
          assert(intersectors.intersectorN.intersect);          
          intersectors.intersectorN.intersect((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,4,item);
        }
      }
#endif
      
#if defined(__AVX__)
      /*! Intersects a packet of 8 rays with the scene. */
      __forceinline void intersect (const vbool8& valid, Ray8& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (likely(intersectors.intersector8.intersect)) { // old code for compatibility
          if (intersectors.intersector8.ispc) {
            ((ISPCIntersectFunc8)intersectors.intersector8.intersect)(intersectors.ptr,(RTCRay8&)ray,item,valid.mask32());
          } else {
            vint8 mask = valid.mask32();
            ((IntersectFunc8)intersectors.intersector8.intersect)(&mask,intersectors.ptr,(RTCRay8&)ray,item);
          }
        } else {
          vint8 mask = valid.mask32();
          assert(intersectors.intersectorN.intersect);
          intersectors.intersectorN.intersect((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,8,item);
        }
      }
#endif

      /*! Intersects a packet of 16 rays with the scene. */
#if defined(__AVX512F__)
      __forceinline void intersect (const vbool16& valid, Ray16& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (likely(intersectors.intersector16.intersect)) { // old code for compatibility
          if (intersectors.intersector16.ispc) {
            ((ISPCIntersectFunc16)intersectors.intersector16.intersect)(intersectors.ptr,(RTCRay16&)ray,item,valid.mask8());
          } else {
            vint16 mask = valid.mask32();
            ((IntersectFunc16)intersectors.intersector16.intersect)(&mask,intersectors.ptr,(RTCRay16&)ray,item);
          }
        } else {
          vint16 mask = valid.mask32();
          assert(intersectors.intersectorN.intersect);
          intersectors.intersectorN.intersect((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,16,item);
        }
      }
#endif

      /*! Intersects a stream of rays with the scene. */
      __forceinline void intersect1M (Ray** rays, size_t N, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (intersectors.intersector1M.intersect) { // Intersect1N callback is optional
          intersectors.intersector1M.intersect(intersectors.ptr,context->user,(RTCRay**)rays,N,item);
        }
        else if (N == 1) {
          int mask = -1;
          assert(intersectors.intersectorN.intersect);
          intersectors.intersectorN.intersect((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)rays[0],1,item);
        } 
        else 
        {
          int mask[MAX_INTERNAL_STREAM_SIZE];
          StackRayPacket<MAX_INTERNAL_STREAM_SIZE> packet(N);
          for (size_t i=0; i<N; i++) packet.writeRay(i,mask,*rays[i]);
          assert(intersectors.intersectorN.intersect);
          intersectors.intersectorN.intersect(mask,intersectors.ptr,context->user,(RTCRayN*)packet.data,N,item);
          for (size_t i=0; i<N; i++) packet.readHit(i,*rays[i]);
        }
      }
      
      /*! Tests if single ray is occluded by the scene. */
      __forceinline void occluded (Ray& ray, size_t item, IntersectContext* context) 
      {
        if (likely(intersectors.intersector1.occluded)) { // old code for compatibility
          intersectors.intersector1.occluded(intersectors.ptr,(RTCRay&)ray,item);
        } else if (likely(intersectors.intersector1M.occluded)) {
          RTCRay* pray = (RTCRay*)&ray;
          intersectors.intersector1M.occluded(intersectors.ptr,context->user,&pray,1,item);
        } else {
          int mask = -1;
          assert(intersectors.intersectorN.occluded);          
          intersectors.intersectorN.occluded((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,1,item);
        }
      }
      
      /*! Tests if a packet of 4 rays is occluded by the scene. */
#if defined(__SSE__)
      __forceinline void occluded (const vbool4& valid, Ray4& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
	if (likely(intersectors.intersector4.occluded)) { // old code for compatibility
          if (intersectors.intersector4.ispc) {
            ((ISPCOccludedFunc4)intersectors.intersector4.occluded)(intersectors.ptr,(RTCRay4&)ray,item,valid.mask32());
          } else {
            vint4 mask = valid.mask32();
            ((OccludedFunc4)intersectors.intersector4.occluded)(&mask,intersectors.ptr,(RTCRay4&)ray,item);
          }
        } else {
          vint4 mask = valid.mask32();
          assert(intersectors.intersectorN.occluded);          
          intersectors.intersectorN.occluded((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,4,item);
        }
      }
#endif
      
      /*! Tests if a packet of 8 rays is occluded by the scene. */
#if defined(__AVX__)
      __forceinline void occluded (const vbool8& valid, Ray8& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
	if (likely(intersectors.intersector8.occluded)) { // old code for compatibility
          if (intersectors.intersector8.ispc) {
            ((ISPCOccludedFunc8)intersectors.intersector8.occluded)(intersectors.ptr,(RTCRay8&)ray,item,valid.mask32());
          } else {
            vint8 mask = valid.mask32();
            ((OccludedFunc8)intersectors.intersector8.occluded)(&mask,intersectors.ptr,(RTCRay8&)ray,item);
          }
        } else {
          vint8 mask = valid.mask32();
          assert(intersectors.intersectorN.occluded);          
          intersectors.intersectorN.occluded((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,8,item);
        }
      }
#endif
      
      /*! Tests if a packet of 16 rays is occluded by the scene. */
#if defined(__AVX512F__)
      __forceinline void occluded (const vbool16& valid, Ray16& ray, size_t item, IntersectContext* context) 
      {
        assert(item < size());
        if (likely(intersectors.intersector16.occluded)) { // old code for compatibility
          if (intersectors.intersector16.ispc) {
            ((ISPCOccludedFunc16)intersectors.intersector16.occluded)(intersectors.ptr,(RTCRay16&)ray,item,valid.mask8());
          }
          else {
            vint16 mask = valid.mask32();
            ((OccludedFunc16)intersectors.intersector16.occluded)(&mask,intersectors.ptr,(RTCRay16&)ray,item);
          }
        } else {
          vint16 mask = valid.mask32();
          assert(intersectors.intersectorN.occluded);          
          intersectors.intersectorN.occluded((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)&ray,16,item);
        }
      }
#endif

      /*! Tests if a stream of rays is occluded by the scene. */
      __forceinline void occluded1M (Ray** rays, size_t N, size_t item, IntersectContext* context) 
      {
        if (likely(intersectors.intersector1M.occluded)) { // Occluded1N callback is optional
          intersectors.intersector1M.occluded(intersectors.ptr,context->user,(RTCRay**)rays,N,item);
        }
        else if (N == 1) {
          int mask = -1;
          assert(intersectors.intersectorN.occluded);
          intersectors.intersectorN.occluded((int*)&mask,intersectors.ptr,context->user,(RTCRayN*)rays[0],1,item);
        } 
        else 
        {
          int mask[MAX_INTERNAL_STREAM_SIZE];
          StackRayPacket<MAX_INTERNAL_STREAM_SIZE> packet(N);
          for (size_t i=0; i<N; i++) packet.writeRay(i,mask,*rays[i]);
          assert(intersectors.intersectorN.occluded);
          intersectors.intersectorN.occluded(mask,intersectors.ptr,context->user,(RTCRayN*)packet.data,N,item);
          for (size_t i=0; i<N; i++) packet.readOcclusion(i,*rays[i]);
        }
      }

    public:
      RTCBoundsFunc  boundsFunc;
      RTCBoundsFunc2 boundsFunc2;
      RTCBoundsFunc3 boundsFunc3;
      void* boundsFuncUserPtr;

      struct Intersectors 
      {
        Intersectors() : ptr(nullptr) {}
      public:
        void* ptr;
        Intersector1 intersector1;
        Intersector4 intersector4;
        Intersector8 intersector8;
        Intersector16 intersector16;
        Intersector1M intersector1M;
        IntersectorN intersectorN;
      } intersectors;
  };
  
#define DEFINE_SET_INTERSECTOR1(symbol,intersector)                     \
  AccelSet::Intersector1 symbol() {                                     \
    return AccelSet::Intersector1((AccelSet::IntersectFunc)intersector::intersect, \
                                  (AccelSet::OccludedFunc )intersector::occluded, \
                                  TOSTRING(isa) "::" TOSTRING(symbol)); \
  }
  
#define DEFINE_SET_INTERSECTOR4(symbol,intersector)                     \
  AccelSet::Intersector4 symbol() {                                     \
    return AccelSet::Intersector4((void*)intersector::intersect,        \
                                  (void*)intersector::occluded,         \
                                  TOSTRING(isa) "::" TOSTRING(symbol),	\
                                  false);                               \
  }
  
#define DEFINE_SET_INTERSECTOR8(symbol,intersector)                     \
  AccelSet::Intersector8 symbol() {                                     \
    return AccelSet::Intersector8((void*)intersector::intersect,        \
                                  (void*)intersector::occluded,         \
                                  TOSTRING(isa) "::" TOSTRING(symbol),	\
                                  false);                               \
  }
  
#define DEFINE_SET_INTERSECTOR16(symbol,intersector)                    \
  AccelSet::Intersector16 symbol() {                                    \
    return AccelSet::Intersector16((void*)intersector::intersect,       \
                                   (void*)intersector::occluded,        \
                                   TOSTRING(isa) "::" TOSTRING(symbol), \
                                   false);                              \
  }
  
#define DEFINE_SET_INTERSECTOR1M(symbol,intersector)                    \
  AccelSet::Intersector1M symbol() {                                    \
    return AccelSet::Intersector1M((AccelSet::IntersectFunc1M)intersector::intersect, \
                                   (AccelSet::OccludedFunc1M )intersector::occluded, \
                                   TOSTRING(isa) "::" TOSTRING(symbol)); \
  }
  
#define DEFINE_SET_INTERSECTORN(symbol,intersector)                     \
  AccelSet::IntersectorN symbol() {                                     \
    return AccelSet::IntersectorN((AccelSet::IntersectFuncN)intersector::intersect, \
                                  (AccelSet::OccludedFuncN)intersector::occluded, \
                                  TOSTRING(isa) "::" TOSTRING(symbol)); \
  }
}
