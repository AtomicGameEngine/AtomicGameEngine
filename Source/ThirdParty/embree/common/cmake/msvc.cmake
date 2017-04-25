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

# the /QxXXX flags are meant for the Intel Compiler, MSVC ignores them
SET(FLAGS_SSE2  "/QxSSE2")
SET(FLAGS_SSE3  "/QxSSE3")
SET(FLAGS_SSSE3 "/QxSSSE3")
SET(FLAGS_SSE41 "/DCONFIG_SSE41 /QxSSE4.1")
SET(FLAGS_SSE42 "/DCONFIG_SSE42 /QxSSE4.2")
SET(FLAGS_AVX   "/arch:AVX /DCONFIG_AVX")

# Intel Compiler 15, Update 1 unfortunately cannot handle /arch:AVX2
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  SET(FLAGS_AVX2  "/DCONFIG_AVX2 /QxCORE-AVX2")
  SET(FLAGS_AVX512KNL "/DCONFIG_AVX512KNL /QxMIC-AVX512")
  SET(FLAGS_AVX512SKX "/DCONFIG_AVX512SKX /QxCORE-AVX512")
ELSE()
  SET(FLAGS_AVX2  "/arch:AVX2 /DCONFIG_AVX2 /QxCORE-AVX2")
  SET(FLAGS_AVX512KNL "")
  SET(FLAGS_AVX512SKX "")
ENDIF()

SET(COMMON_CXX_FLAGS "/EHsc /MP /GR /GS-")
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  SET(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} /Qdiag-disable:11074 ")  # remark #11074: Inlining inhibited by limit max-size
  SET(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} /Qdiag-disable:11075 ")  # remark #11075: To get full report use -Qopt-report:4 -Qopt-report-phase ipo
ENDIF()

SET(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        ${COMMON_CXX_FLAGS} /Ox /Oi /Gy /Qinline-max-total-size- /Qinline-factor=200 ")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COMMON_CXX_FLAGS} /Ox /Oi /Gy /Qinline-max-total-size- /Qinline-factor=200 ")

SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /Qdiag-disable:11082 ")  # warning #11082: locally defined symbol XXX imported
ENDIF()
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /IGNORE:4217")  # locally defined symbol XXX imported in function YYY (happens as the ISPC API layer uses exported library functions)
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /IGNORE:4049")  # warning LNK4049: locally defined symbol _rtcOccluded1M imported
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")

# optionally use static runtime library
OPTION(EMBREE_STATIC_RUNTIME "Use the static version of the C/C++ runtime library." ON)
IF (EMBREE_STATIC_RUNTIME)
  STRING(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  STRING(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
  STRING(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
ENDIF()

# remove define NDEBUG and instead set define DEBUG for config RelWithDebInfo
STRING(REPLACE "NDEBUG" "DEBUG" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
