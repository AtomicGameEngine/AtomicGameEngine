## ======================================================================== ##
## Copyright 2009-2017 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

SET(EMBREE_VERSION_MAJOR 2)
SET(EMBREE_VERSION_MINOR 15)
SET(EMBREE_VERSION_PATCH 1)
SET(EMBREE_VERSION_NOTE "")
SET(EMBREE_VERSION ${EMBREE_VERSION_MAJOR}.${EMBREE_VERSION_MINOR}.${EMBREE_VERSION_PATCH})
MATH(EXPR EMBREE_VERSION_NUMBER "10000*${EMBREE_VERSION_MAJOR} + 100*${EMBREE_VERSION_MINOR} + ${EMBREE_VERSION_PATCH}")
SET(CPACK_RPM_PACKAGE_RELEASE 1)

PROJECT(embree${EMBREE_VERSION_MAJOR})

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)


# find git version
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "EMBREE_HASH"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  ELSE()
    SET(EMBREE_HASH 0)
  ENDIF()
ENDIF()

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/kernels/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)
FILE(WRITE "${PROJECT_BINARY_DIR}/hash" "${EMBREE_HASH}")

IF(COMMAND cmake_policy)
  if (POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
  endif()
  if (POLICY CMP0042)
    cmake_policy(SET CMP0042 OLD)
  endif()
ENDIF(COMMAND cmake_policy)

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)

MARK_AS_ADVANCED(CMAKE_OSX_ARCHITECTURES)
MARK_AS_ADVANCED(CMAKE_OSX_DEPLOYMENT_TARGET)
MARK_AS_ADVANCED(CMAKE_OSX_SYSROOT)
MARK_AS_ADVANCED(CLEAR CMAKE_CXX_COMPILER)
MARK_AS_ADVANCED(GLUT_cocoa_LIBRARY)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common/cmake ${CMAKE_MODULE_PATH})

INCLUDE(test)

##############################################################
# Embree configuration
##############################################################

# ATOMIC BEGIN
OPTION(EMBREE_STATIC_LIB "Build Embree as a static library." ON)
# ATOMIC END
IF (EMBREE_STATIC_LIB)
  SET(EMBREE_LIB_TYPE STATIC)
  ADD_DEFINITIONS(-DEMBREE_STATIC_LIB)
ELSE()
  SET(EMBREE_LIB_TYPE SHARED)
ENDIF()

# ATOMIC BEGIN
OPTION(EMBREE_ISPC_SUPPORT "Build Embree with support for ISPC applications." OFF)
OPTION(EMBREE_TUTORIALS    "Enable to build Embree tutorials" OFF)
# ATOMIC END

##############################################################################
# Configurations (add configurations also to common/cmake/embree-config-default.cmake)

OPTION(EMBREE_STAT_COUNTERS "Enables statistic counters.")

OPTION(EMBREE_RAY_MASK "Enables ray mask support.")
OPTION(EMBREE_BACKFACE_CULLING "Enables backface culling.")
OPTION(EMBREE_INTERSECTION_FILTER "Enables intersection filter callback." ON)
OPTION(EMBREE_INTERSECTION_FILTER_RESTORE "Restores previous hit when hit is filtered out." ON)
OPTION(EMBREE_IGNORE_INVALID_RAYS "Ignores invalid rays." OFF) # FIXME: enable by default?

OPTION(EMBREE_GEOMETRY_TRIANGLES "Enables support for triangle geometries." ON)
OPTION(EMBREE_GEOMETRY_QUADS "Enables support for quad geometries." ON)
OPTION(EMBREE_GEOMETRY_LINES "Enables support for line geometries." ON)
# ATOMIC BEGIN
OPTION(EMBREE_GEOMETRY_HAIR "Enables support for hair geometries." OFF)
OPTION(EMBREE_GEOMETRY_SUBDIV "Enables support for subdiv geometries." OFF)
# ATOMIC END
OPTION(EMBREE_GEOMETRY_USER "Enables support for user geometries." ON)
OPTION(EMBREE_RAY_PACKETS "Enabled support for ray packets." ON)

SET(EMBREE_NATIVE_SPLINE_BASIS BEZIER CACHE STRING "Sets the basis for curves which Embree uses internally. Other types are converted and need more memory.")
SET_PROPERTY(CACHE EMBREE_NATIVE_SPLINE_BASIS PROPERTY STRINGS BSPLINE BEZIER)
IF (EMBREE_NATIVE_SPLINE_BASIS STREQUAL "BSPLINE")
  SET(EMBREE_NATIVE_CURVE_BSPLINE ON)
ELSE()
  SET(EMBREE_NATIVE_CURVE_BSPLINE OFF)
ENDIF()

# ATOMIC BEGIN
# defaulting from TBB to INTERNAL, TODO: look into TBB
SET(EMBREE_TASKING_SYSTEM "INTERNAL" CACHE STRING "Selects tasking system")
# ATOMIC END

IF (WIN32)
  SET_PROPERTY(CACHE EMBREE_TASKING_SYSTEM PROPERTY STRINGS TBB INTERNAL PPL)
ELSE()
  SET_PROPERTY(CACHE EMBREE_TASKING_SYSTEM PROPERTY STRINGS TBB INTERNAL)
ENDIF()

IF (EMBREE_TASKING_SYSTEM STREQUAL "TBB")
  SET(TASKING_TBB      ON )
  SET(TASKING_INTERNAL OFF)
  SET(TASKING_PPL      OFF )
  ADD_DEFINITIONS(-DTASKING_TBB)
ELSEIF (EMBREE_TASKING_SYSTEM STREQUAL "PPL")
  SET(TASKING_PPL      ON )
  SET(TASKING_TBB      OFF )
  SET(TASKING_INTERNAL OFF)
  ADD_DEFINITIONS(-DTASKING_PPL)
ELSE()
  SET(TASKING_INTERNAL ON )
  SET(TASKING_TBB      OFF)
  SET(TASKING_PPL      OFF )
  ADD_DEFINITIONS(-DTASKING_INTERNAL)
ENDIF()

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/kernels/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

##############################################################
# Compiler
##############################################################

SET(CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")

IF (WIN32)
  IF (NOT DEFAULT_CMAKE_CONFIGURATION_TYPES_SET)
    SET(CMAKE_CONFIGURATION_TYPES "${CONFIGURATION_TYPES}" CACHE STRING "List of generated configurations." FORCE)
    SET(DEFAULT_CMAKE_CONFIGURATION_TYPES_SET ON CACHE INTERNAL "Default CMake configuration types set.")
  ENDIF()

  # ATOMIC BEGIN
  # SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
  # ATOMIC END

  IF (${CMAKE_GENERATOR_TOOLSET} MATCHES "^LLVM" )
    INCLUDE(clang)
  ELSE()
    INCLUDE (msvc)
  ENDIF()

ELSE (WIN32)
  IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    INCLUDE (gcc)
  ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    INCLUDE (clang)
  ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    INCLUDE (icc)
  ELSE ()
    MESSAGE(FATAL_ERROR "Unsupported compiler: " ${CMAKE_CXX_COMPILER_ID})
  ENDIF ()

  IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Specifies the build type." FORCE)
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CONFIGURATION_TYPES})
  ENDIF(NOT CMAKE_BUILD_TYPE)

  OPTION(CMAKE_VERBOSE_MAKEFILE "Enables verbose mode.")
  MARK_AS_ADVANCED(CLEAR CMAKE_VERBOSE_MAKEFILE)

ENDIF (WIN32)

##############################################################
# ISA configuration
##############################################################

# ATOMIC BEGIN
# Setting to NONE as don't support ISPC
# macOS is selecting SSE2, look into this on Windows
SET(EMBREE_MAX_ISA "NONE" CACHE STRING "Selects highest ISA to support.")
# ATOMIC END
SET_PROPERTY(CACHE EMBREE_MAX_ISA PROPERTY STRINGS NONE SSE2 SSE4.2 AVX AVX2 AVX512KNL AVX512SKX)

IF (EMBREE_MAX_ISA STREQUAL "NONE")
  SET(EMBREE_ISA_SSE2  ON)
  SET(EMBREE_ISA_SSE42 "OFF" CACHE BOOL "Enables SSE4.2 ISA.")
  SET(EMBREE_ISA_AVX "OFF" CACHE BOOL "Enables AVX ISA.")
  SET(EMBREE_ISA_AVX2 "OFF" CACHE BOOL "Enables AVX2 ISA.")
  SET(EMBREE_ISA_AVX512KNL "OFF" CACHE BOOL "Enables AVX512 ISA for Knights Landing.")
  SET(EMBREE_ISA_AVX512SKX "OFF" CACHE BOOL "Enables AVX512 ISA for Skylake.")
ELSE()
  UNSET(EMBREE_ISA_SSE2 CACHE)
  UNSET(EMBREE_ISA_SSE42 CACHE)
  UNSET(EMBREE_ISA_AVX CACHE)
  UNSET(EMBREE_ISA_AVX2 CACHE)
  UNSET(EMBREE_ISA_AVX512KNL CACHE)
  UNSET(EMBREE_ISA_AVX512SKX CACHE)

  IF(EMBREE_MAX_ISA STREQUAL "SSE2")
    SET(ISA  1)
  ELSEIF(EMBREE_MAX_ISA STREQUAL "SSE4.2")
    SET(ISA  2)
  ELSEIF(EMBREE_MAX_ISA STREQUAL "AVX")
    SET(ISA  3)
  ELSEIF(EMBREE_MAX_ISA STREQUAL "AVX2")
    SET(ISA  4)
  ELSEIF(EMBREE_MAX_ISA STREQUAL "AVX512KNL")
    SET(ISA  5)
  ELSEIF(EMBREE_MAX_ISA STREQUAL "AVX512SKX")
    SET(ISA 6)
  ELSE()
    MESSAGE(FATAL_ERROR "Unsupported ISA specified: " ${EMBREE_MAX_ISA})
  ENDIF()

  SET(EMBREE_ISA_SSE2 OFF)
  SET(EMBREE_ISA_SSE42 OFF)
  SET(EMBREE_ISA_AVX OFF)
  SET(EMBREE_ISA_AVX2 OFF)
  SET(EMBREE_ISA_AVX512KNL OFF)
  SET(EMBREE_ISA_AVX512SKX OFF)

  IF (ISA GREATER 0)
    SET(EMBREE_ISA_SSE2  ON)
  ENDIF ()
  IF (ISA GREATER 1)
    SET(EMBREE_ISA_SSE42  ON)
  ENDIF ()
  IF (ISA GREATER 2)
    SET(EMBREE_ISA_AVX  ON)
  ENDIF ()
  IF (ISA GREATER 3)
    SET(EMBREE_ISA_AVX2  ON)
  ENDIF ()
  IF (ISA GREATER 4)
    SET(EMBREE_ISA_AVX512KNL  ON)
  ENDIF ()
  IF (ISA GREATER 5)
    SET(EMBREE_ISA_AVX512SKX  ON)
  ENDIF ()
ENDIF()

IF (EMBREE_ISA_SSE2)
  ADD_DEFINITIONS(-D__TARGET_SSE2__)
  LIST(APPEND ISPC_TARGETS "sse2")
ENDIF()

IF (EMBREE_ISA_SSE42)
  ADD_DEFINITIONS(-D__TARGET_SSE42__)
  LIST(APPEND ISPC_TARGETS "sse4")
ENDIF ()

IF (EMBREE_ISA_AVX)
  ADD_DEFINITIONS(-D__TARGET_AVX__)
  LIST(APPEND ISPC_TARGETS "avx")
ENDIF ()

IF (EMBREE_ISA_AVX2)
  ADD_DEFINITIONS(-D__TARGET_AVX2__)
  LIST(APPEND ISPC_TARGETS "avx2")
ENDIF ()

IF (EMBREE_ISA_AVX512KNL)
  ADD_DEFINITIONS(-D__TARGET_AVX512KNL__)
  LIST(APPEND ISPC_TARGETS "avx512knl-i32x16")
ENDIF ()

IF (EMBREE_ISA_AVX512SKX)
  ADD_DEFINITIONS(-D__TARGET_AVX512SKX__)
  LIST(APPEND ISPC_TARGETS "avx512skx-i32x16")
ENDIF ()

INCLUDE (ispc)

##############################################################
# Create Binary Packages (uses above config options)
##############################################################
# ATOMIC BEGIN
# include(package)
# ATOMIC END

##############################################################
# Search paths
##############################################################
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")
INCLUDE_DIRECTORIES_ISPC("${PROJECT_BINARY_DIR}")
IF (TASKING_TBB)
  FIND_PACKAGE(TBB REQUIRED)
  INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIRS})
ENDIF()

##############################################################
# Output paths
##############################################################
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

##############################################################
# Directories to compile
##############################################################

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(kernels)

IF (EMBREE_TUTORIALS)
  ADD_SUBDIRECTORY(tutorials)
ENDIF()

##############################################################
# Uninstall
##############################################################

# ATOMIC BEGIN
#configure_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake/uninstall.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
#    IMMEDIATE @ONLY)

#add_custom_target(uninstall
#    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

#SET_PROPERTY(TARGET uninstall PROPERTY FOLDER CMakePredefinedTargets)

# ATOMIC END

##############################################################
# Has to be last
##############################################################
INCLUDE(CPack)
