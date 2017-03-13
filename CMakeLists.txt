
project (Atomic)

cmake_minimum_required(VERSION 2.8.12.1)

set (ATOMIC_SOURCE_DIR ${Atomic_SOURCE_DIR})
set (CMAKE_MODULE_PATH ${ATOMIC_SOURCE_DIR}/Build/CMake/Modules)

include(AtomicGit)
include(AtomicUtils)
include(AtomicCommon)

add_definitions(-DATOMIC_ROOT_SOURCE_DIR="${ATOMIC_SOURCE_DIR}" -DATOMIC_ROOT_BUILD_DIR="${CMAKE_BINARY_DIR}")

add_definitions( -DATOMIC_API= -DATOMIC_STATIC_DEFINE -DATOMIC_ATOMIC2D -DATOMIC_LOGGING -DATOMIC_PROFILING)

if (NOT DEFINED ATOMIC_DEV_BUILD)
    set(ATOMIC_DEV_BUILD 1)
ENDIF()

if (ATOMIC_DEV_BUILD)
    add_definitions("-DATOMIC_DEV_BUILD=1")
endif()

add_link_libraries_exported (Atomic Box2D Duktape TurboBadger FreeType JO LZ4 PugiXml STB SDL)

if (MSVC)
option(ATOMIC_D3D11 "This force D3D11 on for AGE as the default rendering backend, we don't plan to support D3D9 but if you need it just opt out" ON)   
endif()

if (NOT ATOMIC_BUILD_2D)
    add_definitions_exported (-DATOMIC_PHYSICS -DATOMIC_3D)
    add_link_libraries_exported (Bullet StanHull)
endif()

if (NOT EMSCRIPTEN)
    add_definitions_exported (-DATOMIC_THREADING -DATOMIC_NETWORK)
    add_link_libraries_exported (Civetweb Recast Detour DetourCrowd DetourTileCache kNet libcurl)
endif()

add_definitions_exported (-DATOMIC_WEB)

include (AtomicPlatform)

find_program(CLDOC cldoc)
if(CLDOC)
   add_custom_target(docs DEPENDS AtomicEngineDocs)
endif()

if (ATOMIC_WEBVIEW)

  if(APPLE)
    if(POLICY CMP0037)
      # new cmake doesn't like creating framework whose name has spaces
      # which CEF3 scripts (including shell) currently require on OSX
      cmake_policy(SET CMP0037 OLD)
    endif()
    set(CEF_ROOT "${ATOMIC_SOURCE_DIR}/Submodules/CEF/MacOSX")
  elseif(MSVC)
    if (ATOMIC_PROJECT_ARCH STREQUAL "x86")
    else()
      set(CEF_ROOT "${ATOMIC_SOURCE_DIR}/Submodules/CEF/Windows/64bit")
    endif()
  else()
      set(CEF_ROOT "${ATOMIC_SOURCE_DIR}/Submodules/CEF/Linux")
  endif()

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
  find_package(CEF REQUIRED)
  include_directories(${CEF_ROOT})

  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
endif()

add_definitions(${ATOMIC_DEFINITIONS})
add_subdirectory(Source)

if (ATOMIC_DESKTOP AND ATOMIC_DEV_BUILD)
    if (NOT DEFINED ATOMIC_CPLUSPLUS_EXAMPLES)
        set (ATOMIC_CPLUSPLUS_EXAMPLES 1)
    endif ()
    if (ATOMIC_CPLUSPLUS_EXAMPLES)
        add_subdirectory(Submodules/AtomicExamples/FeatureExamples/CPlusPlus)
    endif()
endif()
