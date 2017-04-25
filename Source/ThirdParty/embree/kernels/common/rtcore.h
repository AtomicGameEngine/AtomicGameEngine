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

#include "../../include/embree2/rtcore.h"

namespace embree
{
  /*! decoding of geometry flags */
  __forceinline bool isStatic    (RTCSceneFlags flags) { return (flags & 1) == RTC_SCENE_STATIC; }
  __forceinline bool isDynamic   (RTCSceneFlags flags) { return (flags & 1) == RTC_SCENE_DYNAMIC; }

  __forceinline bool isCompact   (RTCSceneFlags flags) { return (flags & RTC_SCENE_COMPACT) != 0; }
  __forceinline bool isRobust    (RTCSceneFlags flags) { return (flags & RTC_SCENE_ROBUST) != 0; }
  __forceinline bool isCoherent  (RTCSceneFlags flags) { return (flags & RTC_SCENE_COHERENT) != 0; }
  __forceinline bool isIncoherent(RTCSceneFlags flags) { return (flags & RTC_SCENE_INCOHERENT) != 0; }
  __forceinline bool isHighQuality(RTCSceneFlags flags) { return (flags & RTC_SCENE_HIGH_QUALITY) != 0; }

  /*! decoding of algorithm flags */
  __forceinline bool isInterpolatable(RTCAlgorithmFlags flags) { return (flags & RTC_INTERPOLATE) != 0; }
  __forceinline bool isStreamMode(RTCAlgorithmFlags flags) { return (flags & RTC_INTERSECT_STREAM) != 0; }
  __forceinline bool isIntersect1Mode(RTCAlgorithmFlags flags) { return (flags & RTC_INTERSECT1) != 0; }
  __forceinline bool isIntersect4Mode(RTCAlgorithmFlags flags) { return (flags & RTC_INTERSECT4) != 0; }
  __forceinline bool isIntersect8Mode(RTCAlgorithmFlags flags) { return (flags & RTC_INTERSECT8) != 0; }
  __forceinline bool isIntersect16Mode(RTCAlgorithmFlags flags) { return (flags & RTC_INTERSECT16) != 0; }

   /*! decoding of intersection flags */
  __forceinline bool isCoherent  (RTCIntersectFlags flags) { return (flags & RTC_INTERSECT_INCOHERENT) == 0; }
  __forceinline bool isIncoherent(RTCIntersectFlags flags) { return (flags & RTC_INTERSECT_INCOHERENT) != 0; }

#if defined(TASKING_TBB) && (TBB_INTERFACE_VERSION_MAJOR >= 8)
#  define USE_TASK_ARENA 1
#else
#  define USE_TASK_ARENA 0
#endif

/*! Makros used in the rtcore API implementation */
#define RTCORE_CATCH_BEGIN try {
#define RTCORE_CATCH_END(device)                                        \
  } catch (std::bad_alloc&) {                                            \
    Device::process_error(device,RTC_OUT_OF_MEMORY,"out of memory");    \
  } catch (rtcore_error& e) {                                           \
    Device::process_error(device,e.error,e.what());                     \
  } catch (std::exception& e) {                                         \
    Device::process_error(device,RTC_UNKNOWN_ERROR,e.what()); \
  } catch (...) {                                                       \
    Device::process_error(device,RTC_UNKNOWN_ERROR,"unknown exception caught"); \
  }

#define RTCORE_VERIFY_HANDLE(handle)                                    \
  if (handle == nullptr) {                                              \
    throw_RTCError(RTC_INVALID_ARGUMENT,"invalid argument");            \
  }

#define RTCORE_VERIFY_GEOMID(id)                                  \
  if (id == RTC_INVALID_GEOMETRY_ID) {                             \
    throw_RTCError(RTC_INVALID_ARGUMENT,"invalid argument");       \
  }

#if 0 // enable to debug print all API calls
#define RTCORE_TRACE(x) std::cout << #x << std::endl;
#else
#define RTCORE_TRACE(x) 
#endif

  /*! used to throw embree API errors */
  struct rtcore_error : public std::exception
  {
    __forceinline rtcore_error(RTCError error, const std::string& str)
      : error(error), str(str) {}
    
    ~rtcore_error() throw() {}
    
    const char* what () const throw () {
      return str.c_str();
    }
    
    RTCError error;
    std::string str;
  };

#if defined(DEBUG) // only report file and line in debug mode
  #define throw_RTCError(error,str)                                      \
    throw rtcore_error(error,std::string(__FILE__) + " (" + toString(__LINE__) + "): " + std::string(str));
#else
  #define throw_RTCError(error,str)                                      \
    throw rtcore_error(error,str);
#endif

#define RTC_BUILD_SETTINGS_HAS(settings,member)                 \
  (settings.size > (offsetof(RTCBuildSettings,member)+sizeof(settings.member))) 
}
