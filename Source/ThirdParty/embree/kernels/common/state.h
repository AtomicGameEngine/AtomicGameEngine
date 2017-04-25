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

namespace embree
{
  struct State
  {
  public:
    /*! state construction */
    State (bool singledevice);

    /*! state destruction */
    ~State();

    /*! verifies that state is correct */
    void verify();

    /*! parses state from a configuration file */
    bool parseFile(const FileName& fileName);

    /*! parses the state from a string */
    void parseString(const char* cfg);

    /*! parses the state from a stream */
    void parse(Ref<TokenStream> cin);

    /*! prints the state */
    void print();

    /*! checks if verbosity level is at least N */
    bool verbosity(size_t N);

    /*! checks if some particular ISA is enabled */
    bool hasISA(const int isa);

  public:
    std::string tri_accel;                 //!< acceleration structure to use for triangles
    std::string tri_builder;               //!< builder to use for triangles
    std::string tri_traverser;             //!< traverser to use for triangles
    
  public:
    std::string tri_accel_mb;              //!< acceleration structure to use for motion blur triangles
    std::string tri_builder_mb;            //!< builder to use for motion blur triangles
    std::string tri_traverser_mb;          //!< traverser to use for triangles

  public:
    std::string quad_accel;                 //!< acceleration structure to use for quads
    std::string quad_builder;               //!< builder to use for quads
    std::string quad_traverser;             //!< traverser to use for quads

  public:
    std::string quad_accel_mb;             //!< acceleration structure to use for motion blur quads
    std::string quad_builder_mb;           //!< builder to use for motion blur quads
    std::string quad_traverser_mb;         //!< traverser to use for motion blur quads

  public:
    std::string line_accel;                 //!< acceleration structure to use for line segments
    std::string line_builder;               //!< builder to use for line segments
    std::string line_traverser;             //!< traverser to use for line segments

  public:
    std::string line_accel_mb;             //!< acceleration structure to use for motion blur line segments
    std::string line_builder_mb;           //!< builder to use for motion blur line segments
    std::string line_traverser_mb;         //!< traverser to use for motion blur line segments

  public:
    std::string hair_accel;                //!< hair acceleration structure to use
    std::string hair_builder;              //!< builder to use for hair
    std::string hair_traverser;            //!< traverser to use for hair

  public:
    std::string hair_accel_mb;             //!< acceleration structure to use for motion blur hair
    std::string hair_builder_mb;           //!< builder to use for motion blur hair
    std::string hair_traverser_mb;         //!< traverser to use for motion blur hair

  public:
    std::string object_accel;               //!< acceleration structure for user geometries
    std::string object_builder;             //!< builder for user geometries
    int object_accel_min_leaf_size;         //!< minimal leaf size for object acceleration structure
    int object_accel_max_leaf_size;         //!< maximal leaf size for object acceleration structure

  public:
    int object_accel_mb_min_leaf_size;         //!< minimal leaf size for mblur object acceleration structure
    int object_accel_mb_max_leaf_size;         //!< maximal leaf size for mblur object acceleration structure

  public:
    std::string subdiv_accel;              //!< acceleration structure to use for subdivision surfaces
    std::string subdiv_accel_mb;           //!< acceleration structure to use for subdivision surfaces

  public:
    float max_spatial_split_replications;  //!< maximally replications*N many primitives in accel for spatial splits
    size_t tessellation_cache_size;        //!< size of the shared tessellation cache 

  public:
    size_t instancing_open_min;            //!< instancing opens tree to minimally that number of subtrees
    size_t instancing_block_size;          //!< instancing opens tree up to average block size of primitives
    float  instancing_open_factor;         //!< instancing opens tree up to x times the number of instances
    size_t instancing_open_max_depth;      //!< maximal open depth for geometries
    size_t instancing_open_max;            //!< instancing opens tree to maximally that number of subtrees

  public:
    bool ignore_config_files;              //!< if true no more config files get parse
    bool float_exceptions;                 //!< enable floating point exceptions
    int scene_flags;                       //!< scene flags to use
    size_t verbose;                        //!< verbosity of output
    size_t benchmark;                      //!< true
    
  public:
    size_t numThreads;                     //!< number of threads to use in builders
    bool set_affinity;                     //!< sets affinity for worker threads
    bool start_threads;                    //!< true when threads should be started at device creation time
    int enabled_cpu_features;              //!< CPU ISA features to use
    int enabled_builder_cpu_features;      //!< CPU ISA features to use for builders only

  public:
    struct ErrorHandler
    {
    public:
      ErrorHandler();
      ~ErrorHandler();
      RTCError* error();

    public:
      tls_t thread_error;
      std::vector<RTCError*> thread_errors;
      MutexSys errors_mutex;
    };
    ErrorHandler errorHandler;
    static ErrorHandler g_errorHandler;

  public:
    void setErrorFunction(RTCErrorFunc fptr) 
    {
      error_function = fptr;
      error_function2 = nullptr;
      error_function_userptr = nullptr;
    }
    
    void setErrorFunction(RTCErrorFunc2 fptr, void* uptr) 
    {
      error_function = nullptr;
      error_function2 = fptr;
      error_function_userptr = uptr;
    }

    RTCErrorFunc error_function;
    RTCErrorFunc2 error_function2;
    void* error_function_userptr;

  public:
    void setMemoryMonitorFunction(RTCMemoryMonitorFunc fptr) 
    {
      memory_monitor_function = fptr;
      memory_monitor_function2 = nullptr;
      memory_monitor_userptr = nullptr;
    }
    
    void setMemoryMonitorFunction(RTCMemoryMonitorFunc2 fptr, void* uptr) 
    {
      memory_monitor_function = nullptr;
      memory_monitor_function2 = fptr;
      memory_monitor_userptr = uptr;
    }
      
    RTCMemoryMonitorFunc memory_monitor_function;
    RTCMemoryMonitorFunc2 memory_monitor_function2;
    void* memory_monitor_userptr;
  };
}
