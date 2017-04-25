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

#ifdef _WIN32
#  define RTCORE_API extern "C" __declspec(dllexport)
#else
#  define RTCORE_API extern "C" __attribute__ ((visibility ("default")))
#endif

#include "default.h"
#include "device.h"
#include "scene.h"
#include "context.h"
#include "../../include/embree2/rtcore_ray.h"

namespace embree
{  
  /* mutex to make API thread safe */
  static MutexSys g_mutex;

  RTCORE_API RTCDevice rtcNewDevice(const char* cfg)
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewDevice);
    Lock<MutexSys> lock(g_mutex);
    return (RTCDevice) new Device(cfg,false);
    RTCORE_CATCH_END(nullptr);
    return (RTCDevice) nullptr;
  }

  RTCORE_API void rtcDeleteDevice(RTCDevice device) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeleteDevice);
    RTCORE_VERIFY_HANDLE(device);
    Lock<MutexSys> lock(g_mutex);
    delete (Device*) device;
    RTCORE_CATCH_END(nullptr);
  }

  /* global device for compatibility with old rtcInit / rtcExit scheme */
  static Device* g_device = nullptr;

  RTCORE_API void rtcInit(const char* cfg) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcInit);
    Lock<MutexSys> lock(g_mutex);
    if (g_device) throw_RTCError(RTC_INVALID_OPERATION,"already initialized");
    g_device = new Device(cfg,true);
    RTCORE_CATCH_END(g_device);
  }
  
  RTCORE_API void rtcExit() 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcExit);
    Lock<MutexSys> lock(g_mutex);
    if (!g_device) throw_RTCError(RTC_INVALID_OPERATION,"rtcInit has to get called before rtcExit");
    delete g_device; g_device = nullptr;
    RTCORE_CATCH_END(g_device);
  }

  RTCORE_API void rtcSetParameter1i(const RTCParameter parm, ssize_t val)
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetParameter1i);
    Lock<MutexSys> lock(g_mutex);
    if (g_device) g_device->setParameter1i(parm,val);
    RTCORE_CATCH_END(g_device);
  }

  RTCORE_API ssize_t rtcGetParameter1i(const RTCParameter parm)
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcGetParameter1i);
    assert(g_device);
    Lock<MutexSys> lock(g_mutex);
    return g_device->getParameter1i(parm);
    RTCORE_CATCH_END(g_device);
    return 0;
  }

  RTCORE_API void rtcDeviceSetParameter1i(RTCDevice hdevice, const RTCParameter parm, ssize_t val)
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceSetParameter1i);
    const bool internal_parm = (size_t)parm >= 1000000 && (size_t)parm < 1000004;
    if (!internal_parm) RTCORE_VERIFY_HANDLE(hdevice); // allow NULL device for special internal settings
    Lock<MutexSys> lock(g_mutex);
    device->setParameter1i(parm,val);
    RTCORE_CATCH_END(device);
  }

  RTCORE_API ssize_t rtcDeviceGetParameter1i(RTCDevice hdevice, const RTCParameter parm)
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceGetParameter1i);
    RTCORE_VERIFY_HANDLE(hdevice);
    Lock<MutexSys> lock(g_mutex);
    return device->getParameter1i(parm);
    RTCORE_CATCH_END(device);
    return 0;
  }

  RTCORE_API RTCError rtcGetError()
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcGetError);
    if (g_device == nullptr) return Device::getThreadErrorCode();
    else                     return g_device->getDeviceErrorCode();
    RTCORE_CATCH_END(g_device);
    return RTC_UNKNOWN_ERROR;
  }

  RTCORE_API RTCError rtcDeviceGetError(RTCDevice hdevice)
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceGetError);
    if (device == nullptr) return Device::getThreadErrorCode();
    else                   return device->getDeviceErrorCode();
    RTCORE_CATCH_END(device);
    return RTC_UNKNOWN_ERROR;
  }

  RTCORE_API void rtcSetErrorFunction(RTCErrorFunc func) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetErrorFunction);
    assert(g_device);
    if (g_device) g_device->setErrorFunction(func);
    RTCORE_CATCH_END(g_device);
  }

  RTCORE_API void rtcDeviceSetErrorFunction(RTCDevice hdevice, RTCErrorFunc func) 
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceSetErrorFunction);
    RTCORE_VERIFY_HANDLE(hdevice);
    device->setErrorFunction(func);
    RTCORE_CATCH_END(device);
  }

  RTCORE_API void rtcDeviceSetErrorFunction2(RTCDevice hdevice, RTCErrorFunc2 func, void* userPtr) 
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceSetErrorFunction2);
    RTCORE_VERIFY_HANDLE(hdevice);
    device->setErrorFunction(func,userPtr);
    RTCORE_CATCH_END(device);
  }

  RTCORE_API void rtcSetMemoryMonitorFunction(RTCMemoryMonitorFunc func) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetMemoryMonitorFunction);
    assert(g_device);
    if (g_device) g_device->setMemoryMonitorFunction(func);
    RTCORE_CATCH_END(g_device);
  }

  RTCORE_API void rtcDeviceSetMemoryMonitorFunction(RTCDevice hdevice, RTCMemoryMonitorFunc func) 
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceSetMemoryMonitorFunction);
    device->setMemoryMonitorFunction(func);
    RTCORE_CATCH_END(device);
  }

  RTCORE_API void rtcDeviceSetMemoryMonitorFunction2(RTCDevice hdevice, RTCMemoryMonitorFunc2 func, void* userPtr) 
  {
    Device* device = (Device*) hdevice;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceSetMemoryMonitorFunction2);
    device->setMemoryMonitorFunction(func,userPtr);
    RTCORE_CATCH_END(device);
  }

  RTCORE_API void rtcDebug() 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDebug);

#if defined(EMBREE_STAT_COUNTERS)
    Stat::print(std::cout);
    Stat::clear();
#endif

    RTCORE_CATCH_END(g_device);
  }

  RTCORE_API RTCScene rtcNewScene (RTCSceneFlags flags, RTCAlgorithmFlags aflags) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewScene);
    assert(g_device);
    if (!isCoherent(flags) && !isIncoherent(flags)) flags = RTCSceneFlags(flags | RTC_SCENE_INCOHERENT);
    return (RTCScene) new Scene(g_device,flags,aflags);
    RTCORE_CATCH_END(g_device);
    return nullptr;
  }

  RTCORE_API RTCScene rtcDeviceNewScene (RTCDevice device, RTCSceneFlags flags, RTCAlgorithmFlags aflags) 
  {
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeviceNewScene);
    RTCORE_VERIFY_HANDLE(device);
    if (!isCoherent(flags) && !isIncoherent(flags)) flags = RTCSceneFlags(flags | RTC_SCENE_INCOHERENT);
    return (RTCScene) new Scene((Device*)device,flags,aflags);
    RTCORE_CATCH_END((Device*)device);
    return nullptr;
  }

  RTCORE_API void rtcSetProgressMonitorFunction(RTCScene hscene, RTCProgressMonitorFunc func, void* ptr) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetProgressMonitorFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    scene->setProgressMonitorFunction(func,ptr);
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcCommit (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcCommit);
    RTCORE_VERIFY_HANDLE(hscene);
    scene->commit(0,0,true);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcCommitJoin (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcCommitJoin);
    RTCORE_VERIFY_HANDLE(hscene);
    scene->commit(0,0,false);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcCommitThread(RTCScene hscene, unsigned int threadID, unsigned int numThreads) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcCommitThread);
    RTCORE_VERIFY_HANDLE(hscene);

    if (unlikely(numThreads == 0)) 
      throw_RTCError(RTC_INVALID_OPERATION,"invalid number of threads specified");

    /* for best performance set FTZ and DAZ flags in the MXCSR control and status register */
    unsigned int mxcsr = _mm_getcsr();
    _mm_setcsr(mxcsr | /* FTZ */ (1<<15) | /* DAZ */ (1<<6));
    
    /* perform scene build */
    scene->commit(threadID,numThreads,false);

    /* reset MXCSR register again */
    _mm_setcsr(mxcsr);
    
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcGetBounds(RTCScene hscene, RTCBounds& bounds_o)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcGetBounds);
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    BBox3fa bounds = scene->bounds.bounds();
    bounds_o.lower_x = bounds.lower.x;
    bounds_o.lower_y = bounds.lower.y;
    bounds_o.lower_z = bounds.lower.z;
    bounds_o.align0  = 0;
    bounds_o.upper_x = bounds.upper.x;
    bounds_o.upper_y = bounds.upper.y;
    bounds_o.upper_z = bounds.upper.z;
    bounds_o.align1  = 0;
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcGetLinearBounds(RTCScene hscene, RTCBounds* bounds_o)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcGetBounds);
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    bounds_o[0].lower_x = scene->bounds.bounds0.lower.x;
    bounds_o[0].lower_y = scene->bounds.bounds0.lower.y;
    bounds_o[0].lower_z = scene->bounds.bounds0.lower.z;
    bounds_o[0].align0  = 0;
    bounds_o[0].upper_x = scene->bounds.bounds0.upper.x;
    bounds_o[0].upper_y = scene->bounds.bounds0.upper.y;
    bounds_o[0].upper_z = scene->bounds.bounds0.upper.z;
    bounds_o[0].align1  = 0;
    bounds_o[1].lower_x = scene->bounds.bounds1.lower.x;
    bounds_o[1].lower_y = scene->bounds.bounds1.lower.y;
    bounds_o[1].lower_z = scene->bounds.bounds1.lower.z;
    bounds_o[1].align0  = 0;
    bounds_o[1].upper_x = scene->bounds.bounds1.upper.x;
    bounds_o[1].upper_y = scene->bounds.bounds1.upper.y;
    bounds_o[1].upper_z = scene->bounds.bounds1.upper.z;
    bounds_o[1].align1  = 0;
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcIntersect (RTCScene hscene, RTCRay& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect);
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)&ray) & 0x0F        ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT3(normal.travs,1,1,1);
    IntersectContext context(scene,nullptr);
    scene->intersect(ray,&context);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect1Ex (RTCScene hscene, const RTCIntersectContext* user_context, RTCRay& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect1Ex);
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)&ray) & 0x0F        ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT3(normal.travs,1,1,1);
    IntersectContext context(scene,user_context);
    scene->intersect(ray,&context);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect4 (const void* valid, RTCScene hscene, RTCRay4& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect4);

#if defined(__TARGET_SIMD4__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)&ray ) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,4);
    IntersectContext context(scene,nullptr);
    scene->intersect4(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect4 not supported");  
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect4Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay4& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect4Ex);

#if defined(__TARGET_SIMD4__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)&ray ) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,4);
    IntersectContext context(scene,user_context);
    scene->intersect4(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect4Ex not supported");  
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcIntersect8 (const void* valid, RTCScene hscene, RTCRay8& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect8);

#if defined(__TARGET_SIMD8__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)&ray ) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,8);
    IntersectContext context(scene,nullptr);
    scene->intersect8(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect8 not supported");      
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect8Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay8& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect8Ex);

#if defined(__TARGET_SIMD8__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)&ray ) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,8);
    IntersectContext context(scene,user_context);
    scene->intersect8(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect8Ex not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcIntersect16 (const void* valid, RTCScene hscene, RTCRay16& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect16);

#if defined(__TARGET_SIMD16__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)&ray ) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,16);
    IntersectContext context(scene,nullptr);
    scene->intersect16(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect16 not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect16Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay16& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect16Ex);

#if defined(__TARGET_SIMD16__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)&ray ) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(normal.travs,1,cnt,16);
    IntersectContext context(scene,user_context);
    scene->intersect16(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect16Ex not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect1M (RTCScene hscene, const RTCIntersectContext* user_context, RTCRay* rays, const size_t M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect1M);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for single rays */
    if (likely(M == 1)) {
      if (likely(rays->tnear <= rays->tfar)) 
        scene->intersect(*rays,&context);
    } 

    /* codepath for streams */
    else {
      scene->device->rayStreamFilters.filterAOS(scene,rays,M,stride,&context,true);   
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect1M not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersect1Mp (RTCScene hscene, const RTCIntersectContext* user_context, RTCRay** rays, const size_t M) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersect1Mp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for single rays */
    if (likely(M == 1)) {
      if (likely(rays[0]->tnear <= rays[0]->tfar)) 
        scene->intersect(*rays[0],&context);
    } 

    /* codepath for streams */
    else {
      scene->device->rayStreamFilters.filterAOP(scene,rays,M,&context,true);   
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersect1Mp not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersectNM (RTCScene hscene, const RTCIntersectContext* user_context, struct RTCRayN* rays, const size_t N, const size_t M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersectNM);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,N*M,N*M,N*M);
    IntersectContext context(scene,user_context);

    /* code path for single ray streams */
    if (likely(N == 1))
    {
      /* fast code path for streams of size 1 */
      if (likely(M == 1)) {
        if (likely(((RTCRay*)rays)->tnear <= ((RTCRay*)rays)->tfar))
          scene->intersect(*(RTCRay*)rays,&context);
      } 
      /* normal codepath for single ray streams */
      else {
        scene->device->rayStreamFilters.filterAOS(scene,(RTCRay*)rays,M,stride,&context,true);
      }
    }
    /* code path for ray packet streams */
    else {
      scene->device->rayStreamFilters.filterSOA(scene,(char*)rays,N,M,stride,&context,true);
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersectNM not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcIntersectNp (RTCScene hscene, const RTCIntersectContext* user_context, const RTCRayNp& rays, const size_t N) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcIntersectNp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays.orgx   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgx not aligned to 4 bytes");   
    if (((size_t)rays.orgy   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgy not aligned to 4 bytes");   
    if (((size_t)rays.orgz   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgz not aligned to 4 bytes");   
    if (((size_t)rays.dirx   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirx not aligned to 4 bytes");   
    if (((size_t)rays.diry   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.diry not aligned to 4 bytes");   
    if (((size_t)rays.dirz   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirz not aligned to 4 bytes");   
    if (((size_t)rays.tnear  ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirx not aligned to 4 bytes");   
    if (((size_t)rays.tfar   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.tnear not aligned to 4 bytes");   
    if (((size_t)rays.time   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.time not aligned to 4 bytes");   
    if (((size_t)rays.mask   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.mask not aligned to 4 bytes");   
    if (((size_t)rays.Ngx    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngx not aligned to 4 bytes");   
    if (((size_t)rays.Ngy    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngy not aligned to 4 bytes");   
    if (((size_t)rays.Ngz    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngz not aligned to 4 bytes");   
    if (((size_t)rays.u      ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.u not aligned to 4 bytes");   
    if (((size_t)rays.v      ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.v not aligned to 4 bytes");   
    if (((size_t)rays.geomID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.geomID not aligned to 4 bytes");   
    if (((size_t)rays.primID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.primID not aligned to 4 bytes");   
    if (((size_t)rays.instID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.instID not aligned to 4 bytes");   
#endif
    STAT3(normal.travs,N,N,N);
    IntersectContext context(scene,user_context);
    scene->device->rayStreamFilters.filterSOP(scene,rays,N,&context,true);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcIntersectNp not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcOccluded (RTCScene hscene, RTCRay& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded);
    STAT3(shadow.travs,1,1,1);
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)&ray) & 0x0F        ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    IntersectContext context(scene,nullptr);
    scene->occluded(ray,&context);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccluded1Ex (RTCScene hscene, const RTCIntersectContext* user_context, RTCRay& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded1Ex);
    STAT3(shadow.travs,1,1,1);
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)&ray) & 0x0F        ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    IntersectContext context(scene,user_context);
    scene->occluded(ray,&context);
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcOccluded4 (const void* valid, RTCScene hscene, RTCRay4& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded4);

#if defined(__TARGET_SIMD4__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)&ray ) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,4);
    IntersectContext context(scene,nullptr);
    scene->occluded4(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded4 not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccluded4Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay4& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded4Ex);

#if defined(__TARGET_SIMD4__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 16 bytes");   
    if (((size_t)&ray ) & 0x0F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 16 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<4; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,4);
    IntersectContext context(scene,user_context);
    scene->occluded4(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded4Ex not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
 
  RTCORE_API void rtcOccluded8 (const void* valid, RTCScene hscene, RTCRay8& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded8);

#if defined(__TARGET_SIMD8__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)&ray ) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,8);
    IntersectContext context(scene,nullptr);
    scene->occluded8(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded8 not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccluded8Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay8& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded8Ex);

#if defined(__TARGET_SIMD8__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 32 bytes");   
    if (((size_t)&ray ) & 0x1F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 32 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<8; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,8);
    IntersectContext context(scene,user_context);
    scene->occluded8(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded8Ex not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcOccluded16 (const void* valid, RTCScene hscene, RTCRay16& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded16);

#if defined(__TARGET_SIMD16__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)&ray ) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,16);
    IntersectContext context(scene,nullptr);
    scene->occluded16(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded16 not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccluded16Ex (const void* valid, RTCScene hscene, const RTCIntersectContext* user_context, RTCRay16& ray) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded16Ex);

#if defined(__TARGET_SIMD16__) && defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)valid) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "mask not aligned to 64 bytes");   
    if (((size_t)&ray ) & 0x3F       ) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 64 bytes");   
#endif
    STAT(size_t cnt=0; for (size_t i=0; i<16; i++) cnt += ((int*)valid)[i] == -1;);
    STAT3(shadow.travs,1,cnt,16);
    IntersectContext context(scene,user_context);
    scene->occluded16(valid,ray,&context);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded16Ex not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcOccluded1M(RTCScene hscene, const RTCIntersectContext* user_context, RTCRay* rays, const size_t M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded1M);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for streams of size 1 */
    if (likely(M == 1)) {
      if (likely(rays->tnear <= rays->tfar)) 
        scene->occluded (*rays,&context);
    } 
    /* codepath for normal streams */
    else {
      scene->device->rayStreamFilters.filterAOS(scene,rays,M,stride,&context,false);
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded1M not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccluded1Mp(RTCScene hscene, const RTCIntersectContext* user_context, RTCRay** rays, const size_t M) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccluded1Mp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,M,M,M);
    IntersectContext context(scene,user_context);

    /* fast codepath for streams of size 1 */
    if (likely(M == 1)) {
      if (likely(rays[0]->tnear <= rays[0]->tfar)) 
        scene->occluded (*rays[0],&context);
    } 
    /* codepath for normal streams */
    else {
      scene->device->rayStreamFilters.filterAOP(scene,rays,M,&context,false);
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccluded1Mp not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccludedNM(RTCScene hscene, const RTCIntersectContext* user_context, RTCRayN* rays, const size_t N, const size_t M, const size_t stride) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccludedNM);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (stride < sizeof(RTCRay)) throw_RTCError(RTC_INVALID_OPERATION,"stride too small");
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays ) & 0x03) throw_RTCError(RTC_INVALID_ARGUMENT, "ray not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,N*M,N*N,N*N);
    IntersectContext context(scene,user_context);

    /* codepath for single rays */
    if (likely(N == 1))
    {
      /* fast path for streams of size 1 */
      if (likely(M == 1)) {
        if (likely(((RTCRay*)rays)->tnear <= ((RTCRay*)rays)->tfar))
          scene->occluded (*(RTCRay*)rays,&context);
      } 
      /* codepath for normal ray streams */
      else {
        scene->device->rayStreamFilters.filterAOS(scene,(RTCRay*)rays,M,stride,&context,false);
      }
    }
    /* code path for ray packet streams */
    else {
      scene->device->rayStreamFilters.filterSOA(scene,(char*)rays,N,M,stride,&context,false);
    }
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccludedNM not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcOccludedNp(RTCScene hscene, const RTCIntersectContext* user_context, const RTCRayNp& rays, const size_t N) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcOccludedNp);

#if defined (EMBREE_RAY_PACKETS)
#if defined(DEBUG)
    RTCORE_VERIFY_HANDLE(hscene);
    if (scene->isModified()) throw_RTCError(RTC_INVALID_OPERATION,"scene got not committed");
    if (((size_t)rays.orgx   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgx not aligned to 4 bytes");   
    if (((size_t)rays.orgy   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgy not aligned to 4 bytes");   
    if (((size_t)rays.orgz   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.orgz not aligned to 4 bytes");   
    if (((size_t)rays.dirx   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirx not aligned to 4 bytes");   
    if (((size_t)rays.diry   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.diry not aligned to 4 bytes");   
    if (((size_t)rays.dirz   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirz not aligned to 4 bytes");   
    if (((size_t)rays.tnear  ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.dirx not aligned to 4 bytes");   
    if (((size_t)rays.tfar   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.tnear not aligned to 4 bytes");   
    if (((size_t)rays.time   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.time not aligned to 4 bytes");   
    if (((size_t)rays.mask   ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.mask not aligned to 4 bytes");   
    if (((size_t)rays.Ngx    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngx not aligned to 4 bytes");   
    if (((size_t)rays.Ngy    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngy not aligned to 4 bytes");   
    if (((size_t)rays.Ngz    ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.Ngz not aligned to 4 bytes");   
    if (((size_t)rays.u      ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.u not aligned to 4 bytes");   
    if (((size_t)rays.v      ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.v not aligned to 4 bytes");   
    if (((size_t)rays.geomID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.geomID not aligned to 4 bytes");   
    if (((size_t)rays.primID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.primID not aligned to 4 bytes");   
    if (((size_t)rays.instID ) & 0x03 ) throw_RTCError(RTC_INVALID_ARGUMENT, "rays.instID not aligned to 4 bytes");   
#endif
    STAT3(shadow.travs,N,N,N);
    IntersectContext context(scene,user_context);
    scene->device->rayStreamFilters.filterSOP(scene,rays,N,&context,false);
#else
    throw_RTCError(RTC_INVALID_OPERATION,"rtcOccludedNp not supported");
#endif
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcDeleteScene (RTCScene hscene) 
  {
    Scene* scene = (Scene*) hscene;
    Device* device = scene ? scene->device : nullptr;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeleteScene);
    RTCORE_VERIFY_HANDLE(hscene);
    delete scene;
    RTCORE_CATCH_END(device);
  }

  RTCORE_API unsigned rtcNewInstance (RTCScene htarget, RTCScene hsource) 
  {
    Scene* target = (Scene*) htarget;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewInstance);
    RTCORE_VERIFY_HANDLE(htarget);
    RTCORE_VERIFY_HANDLE(hsource);
#if defined(EMBREE_GEOMETRY_USER)
    Scene* source = (Scene*) hsource;
    if (target->device != source->device) throw_RTCError(RTC_INVALID_OPERATION,"scenes do not belong to the same device");
    return target->newInstance(source,1);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewInstance is not supported");
#endif
    RTCORE_CATCH_END(target->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewInstance2 (RTCScene htarget, RTCScene hsource, size_t numTimeSteps) 
  {
    Scene* target = (Scene*) htarget;
    Scene* source = (Scene*) hsource;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewInstance2);
    RTCORE_VERIFY_HANDLE(htarget);
    RTCORE_VERIFY_HANDLE(hsource);
#if defined(EMBREE_GEOMETRY_USER)
    if (target->device != source->device) throw_RTCError(RTC_INVALID_OPERATION,"scenes do not belong to the same device");
    return target->newInstance(source,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewInstance2 is not supported");
#endif
    RTCORE_CATCH_END(target->device);
    return -1;
  }

  AffineSpace3fa convertTransform(RTCMatrixType layout, const float* xfm)
  {
    AffineSpace3fa transform = one;
    switch (layout) 
    {
    case RTC_MATRIX_ROW_MAJOR:
      transform = AffineSpace3fa(Vec3fa(xfm[ 0],xfm[ 4],xfm[ 8]),
                                 Vec3fa(xfm[ 1],xfm[ 5],xfm[ 9]),
                                 Vec3fa(xfm[ 2],xfm[ 6],xfm[10]),
                                 Vec3fa(xfm[ 3],xfm[ 7],xfm[11]));
      break;

    case RTC_MATRIX_COLUMN_MAJOR:
      transform = AffineSpace3fa(Vec3fa(xfm[ 0],xfm[ 1],xfm[ 2]),
                                 Vec3fa(xfm[ 3],xfm[ 4],xfm[ 5]),
                                 Vec3fa(xfm[ 6],xfm[ 7],xfm[ 8]),
                                 Vec3fa(xfm[ 9],xfm[10],xfm[11]));
      break;

    case RTC_MATRIX_COLUMN_MAJOR_ALIGNED16:
      transform = AffineSpace3fa(Vec3fa(xfm[ 0],xfm[ 1],xfm[ 2]),
                                 Vec3fa(xfm[ 4],xfm[ 5],xfm[ 6]),
                                 Vec3fa(xfm[ 8],xfm[ 9],xfm[10]),
                                 Vec3fa(xfm[12],xfm[13],xfm[14]));
      break;

    default: 
      throw_RTCError(RTC_INVALID_OPERATION,"Unknown matrix type");
      break;
    }
    return transform;
  }

  RTCORE_API void rtcSetTransform (RTCScene hscene, unsigned geomID, RTCMatrixType layout, const float* xfm) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetTransform);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    RTCORE_VERIFY_HANDLE(xfm);
    const AffineSpace3fa transform = convertTransform(layout,xfm);
    ((Scene*) scene)->get_locked(geomID)->setTransform(transform,0);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetTransform2 (RTCScene hscene, unsigned geomID, RTCMatrixType layout, const float* xfm, size_t timeStep) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetTransform);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    RTCORE_VERIFY_HANDLE(xfm);
    const AffineSpace3fa transform = convertTransform(layout,xfm);
    ((Scene*) scene)->get_locked(geomID)->setTransform(transform,timeStep);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API unsigned rtcNewUserGeometry (RTCScene hscene, size_t numItems) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewUserGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_USER)
    return scene->newUserGeometry(RTC_GEOMETRY_STATIC,numItems,1);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewUserGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewUserGeometry2 (RTCScene hscene, size_t numItems, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewUserGeometry2);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_USER)
    return scene->newUserGeometry(RTC_GEOMETRY_STATIC,numItems,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewUserGeometry2 is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewUserGeometry3 (RTCScene hscene, RTCGeometryFlags gflags, size_t numItems, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewUserGeometry2);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_USER)
    return scene->newUserGeometry(gflags,numItems,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewUserGeometry3 is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewTriangleMesh (RTCScene hscene, RTCGeometryFlags flags, size_t numTriangles, size_t numVertices, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewTriangleMesh);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_TRIANGLES)
    return scene->newTriangleMesh(flags,numTriangles,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewTriangleMesh is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewQuadMesh (RTCScene hscene, RTCGeometryFlags flags, size_t numQuads, size_t numVertices, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewQuadMesh);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_QUADS)
    return scene->newQuadMesh(flags,numQuads,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewQuadMesh is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewHairGeometry (RTCScene hscene, RTCGeometryFlags flags, size_t numCurves, size_t numVertices, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewHairGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::HAIR,NativeCurves::BEZIER,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewHairGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewBezierHairGeometry (RTCScene hscene, RTCGeometryFlags flags, unsigned int numCurves, unsigned int numVertices, unsigned int numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewBezierHairGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::HAIR,NativeCurves::BEZIER,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewBezierHairGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }
  
  RTCORE_API unsigned rtcNewBSplineHairGeometry (RTCScene hscene, RTCGeometryFlags flags, unsigned int numCurves, unsigned int numVertices, unsigned int numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewBSplineHairGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::HAIR,NativeCurves::BSPLINE,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewBSplineHairGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewCurveGeometry (RTCScene hscene, RTCGeometryFlags flags, size_t numCurves, size_t numVertices, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewCurveGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::SURFACE,NativeCurves::BEZIER,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewCurveGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewBezierCurveGeometry (RTCScene hscene, RTCGeometryFlags flags, unsigned int numCurves, unsigned int numVertices, unsigned int numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewBezierCurveGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::SURFACE,NativeCurves::BEZIER,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewBezierCurveGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewBSplineCurveGeometry (RTCScene hscene, RTCGeometryFlags flags, unsigned int numCurves, unsigned int numVertices, unsigned int numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewBSplineCurveGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_HAIR)
    return scene->newCurves(NativeCurves::SURFACE,NativeCurves::BSPLINE,flags,numCurves,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewBSplineCurveGeometry is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewLineSegments (RTCScene hscene, RTCGeometryFlags flags, size_t numSegments, size_t numVertices, size_t numTimeSteps)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewLineSegments);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_LINES)
    return scene->newLineSegments(flags,numSegments,numVertices,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewLineSegments is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API unsigned rtcNewSubdivisionMesh (RTCScene hscene, RTCGeometryFlags flags, size_t numFaces, size_t numEdges, size_t numVertices, 
                                             size_t numEdgeCreases, size_t numVertexCreases, size_t numHoles, size_t numTimeSteps) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcNewSubdivisionMesh);
    RTCORE_VERIFY_HANDLE(hscene);
#if defined(EMBREE_GEOMETRY_SUBDIV)
    return scene->newSubdivisionMesh(flags,numFaces,numEdges,numVertices,numEdgeCreases,numVertexCreases,numHoles,numTimeSteps);
#else
    throw_RTCError(RTC_UNKNOWN_ERROR,"rtcNewSubdivisionMesh is not supported");
#endif
    RTCORE_CATCH_END(scene->device);
    return -1;
  }

  RTCORE_API void rtcSetMask (RTCScene hscene, unsigned geomID, int mask) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetMask);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setMask(mask);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetBoundaryMode (RTCScene hscene, unsigned geomID, RTCBoundaryMode mode) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBoundaryMode);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setSubdivisionMode(0,(RTCSubdivisionMode)mode);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetSubdivisionMode (RTCScene hscene, unsigned geomID, unsigned topologyID, RTCSubdivisionMode mode) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetSubdivisionMode);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setSubdivisionMode(topologyID,mode);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIndexBuffer (RTCScene hscene, unsigned geomID, RTCBufferType vertexBuffer, RTCBufferType indexBuffer) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIndexBuffer);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIndexBuffer(vertexBuffer,indexBuffer);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void* rtcMapBuffer(RTCScene hscene, unsigned geomID, RTCBufferType type) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcMapBuffer);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    return scene->get_locked(geomID)->map(type);
    RTCORE_CATCH_END(scene->device);
    return nullptr;
  }

  RTCORE_API void rtcUnmapBuffer(RTCScene hscene, unsigned geomID, RTCBufferType type) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcUnmapBuffer);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->unmap(type);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetBuffer(RTCScene hscene, unsigned geomID, RTCBufferType type, const void* ptr, size_t offset, size_t stride)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBuffer);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setBuffer(type,(void*)ptr,offset,stride,-1);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetBuffer2(RTCScene hscene, unsigned geomID, RTCBufferType type, const void* ptr, size_t offset, size_t stride, size_t size)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBuffer2);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setBuffer(type,(void*)ptr,offset,stride,size);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcEnable (RTCScene hscene, unsigned geomID) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcEnable);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->enable();
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcUpdate (RTCScene hscene, unsigned geomID) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcUpdate);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->update();
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcUpdateBuffer (RTCScene hscene, unsigned geomID, RTCBufferType type) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcUpdateBuffer);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->updateBuffer(type);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcDisable (RTCScene hscene, unsigned geomID) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDisable);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->disable();
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcDeleteGeometry (RTCScene hscene, unsigned geomID) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcDeleteGeometry);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->deleteGeometry(geomID);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetTessellationRate (RTCScene hscene, unsigned geomID, float tessellationRate)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetTessellationRate);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setTessellationRate(tessellationRate);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetUserData (RTCScene hscene, unsigned geomID, void* ptr) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetUserData);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setUserData(ptr);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void* rtcGetUserData (RTCScene hscene, unsigned geomID)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcGetUserData);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    return scene->get(geomID)->getUserData(); // this call is on purpose not thread safe
    RTCORE_CATCH_END(scene->device);
    return nullptr;
  }

  RTCORE_API void rtcSetBoundsFunction (RTCScene hscene, unsigned geomID, RTCBoundsFunc bounds)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBoundsFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setBoundsFunction(bounds);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetBoundsFunction2 (RTCScene hscene, unsigned geomID, RTCBoundsFunc2 bounds, void* userPtr)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBoundsFunction2);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setBoundsFunction2(bounds,userPtr);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetBoundsFunction3 (RTCScene hscene, unsigned geomID, RTCBoundsFunc3 bounds, void* userPtr)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetBoundsFunction3);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setBoundsFunction3(bounds,userPtr);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetDisplacementFunction (RTCScene hscene, unsigned geomID, RTCDisplacementFunc func, RTCBounds* bounds)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetDisplacementFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setDisplacementFunction(func,bounds);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetDisplacementFunction2 (RTCScene hscene, unsigned geomID, RTCDisplacementFunc2 func, RTCBounds* bounds)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetDisplacementFunction2);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setDisplacementFunction2(func,bounds);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectFunction (RTCScene hscene, unsigned geomID, RTCIntersectFunc intersect) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunction(intersect);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectFunction4 (RTCScene hscene, unsigned geomID, RTCIntersectFunc4 intersect4) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunction4);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunction4(intersect4);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectFunction8 (RTCScene hscene, unsigned geomID, RTCIntersectFunc8 intersect8) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunction8);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunction8(intersect8);
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcSetIntersectFunction16 (RTCScene hscene, unsigned geomID, RTCIntersectFunc16 intersect16) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunction16);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunction16(intersect16);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectFunction1Mp (RTCScene hscene, unsigned geomID, RTCIntersectFunc1Mp intersect) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunction1Mp);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunction1Mp(intersect);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectFunctionN (RTCScene hscene, unsigned geomID, RTCIntersectFuncN intersect) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectFunctionN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectFunctionN(intersect);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunction (RTCScene hscene, unsigned geomID, RTCOccludedFunc occluded) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunction(occluded);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunction4 (RTCScene hscene, unsigned geomID, RTCOccludedFunc4 occluded4) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunction4);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunction4(occluded4);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunction8 (RTCScene hscene, unsigned geomID, RTCOccludedFunc8 occluded8) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunction8);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunction8(occluded8);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunction16 (RTCScene hscene, unsigned geomID, RTCOccludedFunc16 occluded16) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunction16);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunction16(occluded16);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunction1Mp (RTCScene hscene, unsigned geomID, RTCOccludedFunc1Mp occluded) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunction1Mp);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunction1Mp(occluded);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOccludedFunctionN (RTCScene hscene, unsigned geomID, RTCOccludedFuncN occluded) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOccludedFunctionN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOccludedFunctionN(occluded);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectionFilterFunction (RTCScene hscene, unsigned geomID, RTCFilterFunc intersect) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectionFilterFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectionFilterFunction(intersect);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectionFilterFunction4 (RTCScene hscene, unsigned geomID, RTCFilterFunc4 filter4) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectionFilterFunction4);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectionFilterFunction4(filter4);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectionFilterFunction8 (RTCScene hscene, unsigned geomID, RTCFilterFunc8 filter8) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectionFilterFunction8);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectionFilterFunction8(filter8);
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcSetIntersectionFilterFunction16 (RTCScene hscene, unsigned geomID, RTCFilterFunc16 filter16) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectionFilterFunction16);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectionFilterFunction16(filter16);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetIntersectionFilterFunctionN (RTCScene hscene, unsigned geomID, RTCFilterFuncN filterN) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetIntersectionFilterFunctionN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setIntersectionFilterFunctionN(filterN);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOcclusionFilterFunction (RTCScene hscene, unsigned geomID, RTCFilterFunc intersect) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOcclusionFilterFunction);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOcclusionFilterFunction(intersect);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOcclusionFilterFunction4 (RTCScene hscene, unsigned geomID, RTCFilterFunc4 filter4) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOcclusionFilterFunction4);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOcclusionFilterFunction4(filter4);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOcclusionFilterFunction8 (RTCScene hscene, unsigned geomID, RTCFilterFunc8 filter8) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOcclusionFilterFunction8);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOcclusionFilterFunction8(filter8);
    RTCORE_CATCH_END(scene->device);
  }
  
  RTCORE_API void rtcSetOcclusionFilterFunction16 (RTCScene hscene, unsigned geomID, RTCFilterFunc16 filter16) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOcclusionFilterFunction16);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOcclusionFilterFunction16(filter16);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcSetOcclusionFilterFunctionN (RTCScene hscene, unsigned geomID, RTCFilterFuncN filterN) 
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcSetOcclusionFilterFunctionN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get_locked(geomID)->setOcclusionFilterFunctionN(filterN);
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcInterpolate(RTCScene hscene, unsigned geomID, unsigned primID, float u, float v, 
                                 RTCBufferType buffer,
                                 float* P, float* dPdu, float* dPdv, size_t numFloats)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcInterpolate);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get(geomID)->interpolate(primID,u,v,buffer,P,dPdu,dPdv,nullptr,nullptr,nullptr,numFloats); // this call is on purpose not thread safe
    RTCORE_CATCH_END(scene->device);
  }

  RTCORE_API void rtcInterpolate2(RTCScene hscene, unsigned geomID, unsigned primID, float u, float v, 
                                  RTCBufferType buffer,
                                  float* P, float* dPdu, float* dPdv, 
                                  float* ddPdudu, float* ddPdvdv, float* ddPdudv, 
                                  size_t numFloats)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcInterpolate);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get(geomID)->interpolate(primID,u,v,buffer,P,dPdu,dPdv,ddPdudu,ddPdvdv,ddPdudv,numFloats); // this call is on purpose not thread safe
    RTCORE_CATCH_END(scene->device);
  }


#if defined (EMBREE_RAY_PACKETS)
  RTCORE_API void rtcInterpolateN(RTCScene hscene, unsigned geomID, 
                                  const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                                  RTCBufferType buffer,
                                  float* P, float* dPdu, float* dPdv, size_t numFloats)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcInterpolateN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get(geomID)->interpolateN(valid_i,primIDs,u,v,numUVs,buffer,P,dPdu,dPdv,nullptr,nullptr,nullptr,numFloats); // this call is on purpose not thread safe
    RTCORE_CATCH_END(scene->device);
  }
#endif

#if defined (EMBREE_RAY_PACKETS)
  RTCORE_API void rtcInterpolateN2(RTCScene hscene, unsigned geomID, 
                                   const void* valid_i, const unsigned* primIDs, const float* u, const float* v, size_t numUVs, 
                                   RTCBufferType buffer,
                                   float* P, float* dPdu, float* dPdv, 
                                   float* ddPdudu, float* ddPdvdv, float* ddPdudv, 
                                   size_t numFloats)
  {
    Scene* scene = (Scene*) hscene;
    RTCORE_CATCH_BEGIN;
    RTCORE_TRACE(rtcInterpolateN);
    RTCORE_VERIFY_HANDLE(hscene);
    RTCORE_VERIFY_GEOMID(geomID);
    scene->get(geomID)->interpolateN(valid_i,primIDs,u,v,numUVs,buffer,P,dPdu,dPdv,ddPdudu,ddPdvdv,ddPdudv,numFloats); // this call is on purpose not thread safe
    RTCORE_CATCH_END(scene->device);
  }
#endif
}
