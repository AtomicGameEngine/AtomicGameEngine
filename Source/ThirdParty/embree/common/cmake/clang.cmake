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

SET(FLAGS_SSE2  "-msse2")
SET(FLAGS_SSE3  "-msse3")
SET(FLAGS_SSSE3 "-mssse3")
SET(FLAGS_SSE41 "-msse4.1")
SET(FLAGS_SSE42 "-msse4.2")
SET(FLAGS_AVX   "-mavx")
SET(FLAGS_AVX2  "-mf16c -mavx2 -mfma -mlzcnt -mbmi -mbmi2")
SET(FLAGS_AVX512KNL "-march=knl")
SET(FLAGS_AVX512SKX "-march=skx")

IF (WIN32)

  SET(COMMON_CXX_FLAGS "/EHsc /GR -Xclang -fcxx-exceptions")
  SET(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_CXX_FLAGS}")
  SET(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        ${COMMON_CXX_FLAGS} /Ox /Oi /Gy")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COMMON_CXX_FLAGS} /Ox /Oi /Gy")

  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")

  # remove define NDEBUG and instead set define DEBUG for config RelWithDebInfo
  STRING(REPLACE "NDEBUG" "DEBUG" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

ELSE()

  OPTION(EMBREE_IGNORE_CMAKE_CXX_FLAGS "When enabled Embree ignores default CMAKE_CXX_FLAGS." ON)
  IF (EMBREE_IGNORE_CMAKE_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "")
  ENDIF()

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -std=c++11 -fvisibility-inlines-hidden -fvisibility=hidden -fno-strict-aliasing")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-DDEBUG  -DTBB_USE_DEBUG -g -O0")
  SET(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG                    -O3")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DDEBUG  -DTBB_USE_DEBUG -g -O3")

  IF (APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 ") # we only use MacOSX 10.7 features
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++"            ) # link against C++11 stdlib
  ELSE(APPLE)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  ENDIF(APPLE)

ENDIF()
