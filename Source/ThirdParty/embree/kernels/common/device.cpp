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

#include "device.h"
#include "version.h"
#include "scene_triangle_mesh.h"
#include "scene_user_geometry.h"
#include "scene_instance.h"
#include "scene_bezier_curves.h"
#include "scene_subdiv_mesh.h"

#include "../subdiv/tessellation_cache.h"

#include "acceln.h"
#include "geometry.h"

#include "../geometry/cylinder.h"

#include "../bvh/bvh4_factory.h"
#include "../bvh/bvh8_factory.h"

#include "../common/tasking/taskscheduler.h"

namespace embree
{
  /*! some global variables that can be set via rtcSetParameter1i for debugging purposes */
  ssize_t Device::debug_int0 = 0;
  ssize_t Device::debug_int1 = 0;
  ssize_t Device::debug_int2 = 0;
  ssize_t Device::debug_int3 = 0;

  DECLARE_SYMBOL2(RayStreamFilterFuncs,rayStreamFilterFuncs);

  static MutexSys g_mutex;
  static std::map<Device*,size_t> g_cache_size_map;
  static std::map<Device*,size_t> g_num_threads_map;

  Device::Device (const char* cfg, bool singledevice)
    : State(singledevice)
  {
    /* initialize global state */
    State::parseString(cfg);
    if (!ignore_config_files && FileName::executableFolder() != FileName(""))
      State::parseFile(FileName::executableFolder()+FileName(".embree" TOSTRING(__EMBREE_VERSION_MAJOR__)));
    if (!ignore_config_files && FileName::homeFolder() != FileName(""))
      State::parseFile(FileName::homeFolder()+FileName(".embree" TOSTRING(__EMBREE_VERSION_MAJOR__)));
    State::verify();

    /*! do some internal tests */
    assert(isa::Cylinder::verify());
    
    /*! set tessellation cache size */
    setCacheSize( State::tessellation_cache_size );

    /*! enable some floating point exceptions to catch bugs */
    if (State::float_exceptions)
    {
      int exceptions = _MM_MASK_MASK;
      //exceptions &= ~_MM_MASK_INVALID;
      exceptions &= ~_MM_MASK_DENORM;
      exceptions &= ~_MM_MASK_DIV_ZERO;
      //exceptions &= ~_MM_MASK_OVERFLOW;
      //exceptions &= ~_MM_MASK_UNDERFLOW;
      //exceptions &= ~_MM_MASK_INEXACT;
      _MM_SET_EXCEPTION_MASK(exceptions);
    }

    /* print info header */
    if (State::verbosity(1))
      print();
    if (State::verbosity(2)) 
      State::print();

    /* register all algorithms */
    instance_factory = make_unique(new InstanceFactory(enabled_cpu_features));

    bvh4_factory = make_unique(new BVH4Factory(enabled_builder_cpu_features, enabled_cpu_features));

#if defined(__TARGET_AVX__)
    bvh8_factory = make_unique(new BVH8Factory(enabled_builder_cpu_features, enabled_cpu_features));
#endif

    /* setup tasking system */
    initTaskingSystem(numThreads);

    /* ray stream SOA to AOS conversion */
#if defined(EMBREE_RAY_PACKETS)
    RayStreamFilterFuncsType rayStreamFilterFuncs;
    SELECT_SYMBOL_DEFAULT_SSE42_AVX_AVX2_AVX512KNL_AVX512SKX(enabled_cpu_features,rayStreamFilterFuncs);
    rayStreamFilters = rayStreamFilterFuncs();
#endif
  }

  Device::~Device ()
  {
    setCacheSize(0);
    exitTaskingSystem();
  }

  std::string getEnabledTargets()
  {
    std::string v = std::string(ISA_STR) + " ";
#if defined(__TARGET_SSE41__)
    v += "SSE4.1 ";
#endif
#if defined(__TARGET_SSE42__)
    v += "SSE4.2 ";
#endif
#if defined(__TARGET_AVX__)
    v += "AVX ";
#endif
#if defined(__TARGET_AVX2__)
    v += "AVX2 ";
#endif
#if defined(__TARGET_AVX512KNL__)
    v += "AVX512KNL ";
#endif
#if defined(__TARGET_AVX512SKX__)
    v += "AVX512SKX ";
#endif
    return v;
  }

  std::string getEmbreeFeatures()
  {
    std::string v;
#if defined(EMBREE_RAY_MASK)
    v += "raymasks ";
#endif
#if defined (EMBREE_BACKFACE_CULLING)
    v += "backfaceculling ";
#endif
#if defined(EMBREE_INTERSECTION_FILTER)
    v += "intersection_filter ";
#endif
    return v;
  }

  void Device::print()
  {
    const int cpu_features = getCPUFeatures();
    std::cout << "Embree Ray Tracing Kernels " << __EMBREE_VERSION__ << " (" << __EMBREE_HASH__ << ")" << std::endl;
    std::cout << "  Compiler  : " << getCompilerName() << std::endl;
    std::cout << "  Build     : ";
#if defined(DEBUG)
    std::cout << "Debug " << std::endl;
#else
    std::cout << "Release " << std::endl;
#endif
    std::cout << "  Platform  : " << getPlatformName() << std::endl;
    std::cout << "  CPU       : " << stringOfCPUModel(getCPUModel()) << " (" << getCPUVendor() << ")" << std::endl;
    std::cout << "   Threads  : " << getNumberOfLogicalThreads() << std::endl;
    std::cout << "   ISA      : " << stringOfCPUFeatures(cpu_features) << std::endl;
    std::cout << "   Targets  : " << supportedTargetList(cpu_features) << std::endl;
    const bool hasFTZ = _mm_getcsr() & _MM_FLUSH_ZERO_ON;
    const bool hasDAZ = _mm_getcsr() & _MM_DENORMALS_ZERO_ON;
    std::cout << "   MXCSR    : " << "FTZ=" << hasFTZ << ", DAZ=" << hasDAZ << std::endl;
    std::cout << "  Config" << std::endl;
    std::cout << "    Threads : " << (numThreads ? toString(numThreads) : std::string("default")) << std::endl;
    std::cout << "    ISA     : " << stringOfCPUFeatures(enabled_cpu_features) << std::endl;
    std::cout << "    Targets : " << supportedTargetList(enabled_cpu_features) << " (supported)" << std::endl;
    std::cout << "              " << getEnabledTargets() << " (compile time enabled)" << std::endl;
    std::cout << "    Features: " << getEmbreeFeatures() << std::endl;
    std::cout << "    Tasking : ";
#if defined(TASKING_TBB)
    std::cout << "TBB" << TBB_VERSION_MAJOR << "." << TBB_VERSION_MINOR << " ";
    std::cout << "TBB_header_interface_" << TBB_INTERFACE_VERSION << " TBB_lib_interface_" << tbb::TBB_runtime_interface_version() << " ";
#endif
#if defined(TASKING_INTERNAL)
    std::cout << "internal_tasking_system ";
#endif
#if defined(TASKING_PPL)
	std::cout << "PPL ";
#endif
    std::cout << std::endl;

    /* check of FTZ and DAZ flags are set in CSR */
    if (!hasFTZ || !hasDAZ) 
    {
#if !defined(_DEBUG)
      if (State::verbosity(1)) 
#endif
      {
        std::cout << std::endl;
        std::cout << "================================================================================" << std::endl;
        std::cout << "  WARNING: \"Flush to Zero\" or \"Denormals are Zero\" mode not enabled "         << std::endl 
                  << "           in the MXCSR control and status register. This can have a severe "     << std::endl
                  << "           performance impact. Please enable these modes for each application "   << std::endl
                  << "           thread the following way:" << std::endl
                  << std::endl 
                  << "           #include \"xmmintrin.h\"" << std::endl 
                  << "           #include \"pmmintrin.h\"" << std::endl 
                  << std::endl 
                  << "           _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);" << std::endl 
                  << "           _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);" << std::endl;
        std::cout << "================================================================================" << std::endl;
        std::cout << std::endl;
      }
    }
    std::cout << std::endl;
  }

  void Device::setDeviceErrorCode(RTCError error)
  {
    RTCError* stored_error = errorHandler.error();
    if (*stored_error == RTC_NO_ERROR)
      *stored_error = error;
  }

  RTCError Device::getDeviceErrorCode()
  {
    RTCError* stored_error = errorHandler.error();
    RTCError error = *stored_error;
    *stored_error = RTC_NO_ERROR;
    return error;
  }

  void Device::setThreadErrorCode(RTCError error)
  {
    RTCError* stored_error = g_errorHandler.error();
    if (*stored_error == RTC_NO_ERROR)
      *stored_error = error;
  }

  RTCError Device::getThreadErrorCode()
  {
    RTCError* stored_error = g_errorHandler.error();
    RTCError error = *stored_error;
    *stored_error = RTC_NO_ERROR;
    return error;
  }

  void Device::process_error(Device* device, RTCError error, const char* str)
  { 
    /* store global error code when device construction failed */
    if (!device)
      return setThreadErrorCode(error);

    /* print error when in verbose mode */
    if (device->verbosity(1)) 
    {
      switch (error) {
      case RTC_NO_ERROR         : std::cerr << "Embree: No error"; break;
      case RTC_UNKNOWN_ERROR    : std::cerr << "Embree: Unknown error"; break;
      case RTC_INVALID_ARGUMENT : std::cerr << "Embree: Invalid argument"; break;
      case RTC_INVALID_OPERATION: std::cerr << "Embree: Invalid operation"; break;
      case RTC_OUT_OF_MEMORY    : std::cerr << "Embree: Out of memory"; break;
      case RTC_UNSUPPORTED_CPU  : std::cerr << "Embree: Unsupported CPU"; break;
      default                   : std::cerr << "Embree: Invalid error code"; break;                   
      };
      if (str) std::cerr << ", (" << str << ")";
      std::cerr << std::endl;
    }

    /* call user specified error callback */
    if (device->error_function) 
      device->error_function(error,str); 
    if (device->error_function2) 
      device->error_function2(device->error_function_userptr,error,str); 

    /* record error code */
    device->setDeviceErrorCode(error);
  }

  void Device::memoryMonitor(ssize_t bytes, bool post)
  {
    if (State::memory_monitor_function && bytes != 0) {
      if (!State::memory_monitor_function(bytes,post)) {
        if (bytes > 0) { // only throw exception when we allocate memory to never throw inside a destructor
          throw_RTCError(RTC_OUT_OF_MEMORY,"memory monitor forced termination");
        }
      }
    }

    if (State::memory_monitor_function2 && bytes != 0) {
      if (!State::memory_monitor_function2(State::memory_monitor_userptr,bytes,post)) {
        if (bytes > 0) { // only throw exception when we allocate memory to never throw inside a destructor
          throw_RTCError(RTC_OUT_OF_MEMORY,"memory monitor forced termination");
        }
      }
    }
  }

  size_t getMaxNumThreads()
  {
    size_t maxNumThreads = 0;
    for (std::map<Device*,size_t>::iterator i=g_num_threads_map.begin(); i != g_num_threads_map.end(); i++)
      maxNumThreads = max(maxNumThreads, (*i).second);
    if (maxNumThreads == 0)
      maxNumThreads = std::numeric_limits<size_t>::max();
    return maxNumThreads;
  }

  size_t getMaxCacheSize()
  {
    size_t maxCacheSize = 0;
    for (std::map<Device*,size_t>::iterator i=g_cache_size_map.begin(); i!= g_cache_size_map.end(); i++)
      maxCacheSize = max(maxCacheSize, (*i).second);
    return maxCacheSize;
  }
 
  void Device::setCacheSize(size_t bytes) 
  {
#if defined(EMBREE_GEOMETRY_SUBDIV)
    Lock<MutexSys> lock(g_mutex);
    if (bytes == 0) g_cache_size_map.erase(this);
    else            g_cache_size_map[this] = bytes;
    
    size_t maxCacheSize = getMaxCacheSize();
    resizeTessellationCache(maxCacheSize);
#endif
  }

  void Device::initTaskingSystem(size_t numThreads) 
  {
    Lock<MutexSys> lock(g_mutex);
    if (numThreads == 0) 
      g_num_threads_map[this] = std::numeric_limits<size_t>::max();
    else 
      g_num_threads_map[this] = numThreads;

    /* create task scheduler */
    size_t maxNumThreads = getMaxNumThreads();
    TaskScheduler::create(maxNumThreads,State::set_affinity,State::start_threads);
#if USE_TASK_ARENA
    arena = make_unique(new tbb::task_arena((int)min(maxNumThreads,TaskScheduler::threadCount())));
#endif
  }

  void Device::exitTaskingSystem() 
  {
    Lock<MutexSys> lock(g_mutex);
    g_num_threads_map.erase(this);

    /* terminate tasking system */
    if (g_num_threads_map.size() == 0) {
      TaskScheduler::destroy();
    } 
    /* or configure new number of threads */
    else {
      size_t maxNumThreads = getMaxNumThreads();
      TaskScheduler::create(maxNumThreads,State::set_affinity,State::start_threads);
    }
#if USE_TASK_ARENA
    arena.reset();
#endif
  }

  void Device::setParameter1i(const RTCParameter parm, ssize_t val)
  {
    /* hidden internal parameters */
    switch ((size_t)parm)
    {
    case 1000000: debug_int0 = val; return;
    case 1000001: debug_int1 = val; return;
    case 1000002: debug_int2 = val; return;
    case 1000003: debug_int3 = val; return;
    }

    switch (parm) {
    case RTC_SOFTWARE_CACHE_SIZE: setCacheSize(val); break;
    default: throw_RTCError(RTC_INVALID_ARGUMENT, "unknown writable parameter"); break;
    };
  }

  ssize_t Device::getParameter1i(const RTCParameter parm)
  {
    size_t iparm = (size_t)parm;

    /* get name of internal regression test */
    if (iparm >= 2000000 && iparm < 3000000)
    {
      RegressionTest* test = getRegressionTest(iparm-2000000);
      if (test) return (ssize_t) test->name.c_str();
      else      return 0;
    }

    /* run internal regression test */
    if (iparm >= 3000000 && iparm < 4000000)
    {
      RegressionTest* test = getRegressionTest(iparm-3000000);
      if (test) return test->run();
      else      return 0;
    }

    /* documented parameters */
    switch (parm) 
    {
    case RTC_CONFIG_VERSION_MAJOR: return __EMBREE_VERSION_MAJOR__;
    case RTC_CONFIG_VERSION_MINOR: return __EMBREE_VERSION_MINOR__;
    case RTC_CONFIG_VERSION_PATCH: return __EMBREE_VERSION_PATCH__;
    case RTC_CONFIG_VERSION      : return __EMBREE_VERSION_NUMBER__;

    case RTC_CONFIG_INTERSECT1: return 1;

#if defined(__TARGET_SIMD4__) && defined(EMBREE_RAY_PACKETS)
    case RTC_CONFIG_INTERSECT4:  return hasISA(SSE2);
#else
    case RTC_CONFIG_INTERSECT4:  return 0;
#endif

#if defined(__TARGET_SIMD8__) && defined(EMBREE_RAY_PACKETS)
    case RTC_CONFIG_INTERSECT8:  return hasISA(AVX);
#else
    case RTC_CONFIG_INTERSECT8:  return 0;
#endif

#if defined(__TARGET_SIMD16__) && defined(EMBREE_RAY_PACKETS)
    case RTC_CONFIG_INTERSECT16: return hasISA(AVX512KNL) | hasISA(AVX512SKX);
#else
    case RTC_CONFIG_INTERSECT16: return 0;
#endif

#if defined(EMBREE_RAY_PACKETS)
    case RTC_CONFIG_INTERSECT_STREAM:  return 1;
#else
    case RTC_CONFIG_INTERSECT_STREAM:  return 0;
#endif
    
#if defined(EMBREE_RAY_MASK)
    case RTC_CONFIG_RAY_MASK: return 1;
#else
    case RTC_CONFIG_RAY_MASK: return 0;
#endif

#if defined(EMBREE_BACKFACE_CULLING)
    case RTC_CONFIG_BACKFACE_CULLING: return 1;
#else
    case RTC_CONFIG_BACKFACE_CULLING: return 0;
#endif

#if defined(EMBREE_INTERSECTION_FILTER)
    case RTC_CONFIG_INTERSECTION_FILTER: return 1;
#else
    case RTC_CONFIG_INTERSECTION_FILTER: return 0;
#endif

#if defined(EMBREE_INTERSECTION_FILTER_RESTORE)
    case RTC_CONFIG_INTERSECTION_FILTER_RESTORE: return 1;
#else
    case RTC_CONFIG_INTERSECTION_FILTER_RESTORE: return 0;
#endif

#if defined(EMBREE_IGNORE_INVALID_RAYS)
    case RTC_CONFIG_IGNORE_INVALID_RAYS: return 1;
#else
    case RTC_CONFIG_IGNORE_INVALID_RAYS: return 0;
#endif

#if defined(TASKING_INTERNAL)
    case RTC_CONFIG_TASKING_SYSTEM: return 0;
#endif

#if defined(TASKING_TBB)
    case RTC_CONFIG_TASKING_SYSTEM: return 1;
#endif

#if defined(EMBREE_GEOMETRY_TRIANGLES)
    case RTC_CONFIG_TRIANGLE_GEOMETRY: return 1;
#else
    case RTC_CONFIG_TRIANGLE_GEOMETRY: return 0;
#endif
        
#if defined(EMBREE_GEOMETRY_QUADS)
    case RTC_CONFIG_QUAD_GEOMETRY: return 1;
#else
    case RTC_CONFIG_QUAD_GEOMETRY: return 0;
#endif

#if defined(EMBREE_GEOMETRY_LINES)
    case RTC_CONFIG_LINE_GEOMETRY: return 1;
#else
    case RTC_CONFIG_LINE_GEOMETRY: return 0;
#endif

#if defined(EMBREE_GEOMETRY_HAIR)
    case RTC_CONFIG_HAIR_GEOMETRY: return 1;
#else
    case RTC_CONFIG_HAIR_GEOMETRY: return 0;
#endif

#if defined(EMBREE_GEOMETRY_SUBDIV)
    case RTC_CONFIG_SUBDIV_GEOMETRY: return 1;
#else
    case RTC_CONFIG_SUBDIV_GEOMETRY: return 0;
#endif

#if defined(EMBREE_GEOMETRY_USER)
    case RTC_CONFIG_USER_GEOMETRY: return 1;
#else
    case RTC_CONFIG_USER_GEOMETRY: return 0;
#endif

#if defined(TASKING_TBB) && (TBB_INTERFACE_VERSION_MAJOR < 8)
    case RTC_CONFIG_COMMIT_JOIN: return 0;
    case RTC_CONFIG_COMMIT_THREAD: return 0;
#else
    case RTC_CONFIG_COMMIT_JOIN: return 1;
    case RTC_CONFIG_COMMIT_THREAD: return 1;
#endif

    default: throw_RTCError(RTC_INVALID_ARGUMENT, "unknown readable parameter"); break;
    };
  }
}
