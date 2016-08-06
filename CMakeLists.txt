
project (Atomic)

cmake_minimum_required(VERSION 2.8.12.1)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Build/CMake/Modules)

include(AtomicUtils)

add_definitions(-DATOMIC_ROOT_SOURCE_DIR="${CMAKE_SOURCE_DIR}" -DATOMIC_ROOT_BUILD_DIR="${CMAKE_BINARY_DIR}")

add_definitions( -DATOMIC_API= -DATOMIC_STATIC_DEFINE -DATOMIC_ATOMIC2D -DATOMIC_LOGGING -DATOMIC_PROFILING)

if (NOT DEFINED ATOMIC_DEV_BUILD)
    set(ATOMIC_DEV_BUILD 1)
ENDIF()

if (ATOMIC_DEV_BUILD)
    add_definitions("-DATOMIC_DEV_BUILD=1")
endif()

# Disable until https://github.com/AtomicGameEngine/AtomicGameEngine/issues/554 is addressed
# add_definitions("-DATOMIC_SOURCE_BUILD=1")

# this is here as QtCreator is having trouble picking up #include <Atomic/*> without it
include_directories(${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/AtomicEditor/Source)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Duktape TurboBadger FreeType JO LZ4 PugiXml STB SDL)

if (NOT ATOMIC_BUILD_2D)
    add_definitions( -DATOMIC_PHYSICS -DATOMIC_3D)
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} Bullet StanHull)
endif()

if (NOT EMSCRIPTEN)
    add_definitions( -DATOMIC_THREADING -DATOMIC_NETWORK)
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} Civetweb Recast Detour DetourCrowd DetourTileCache kNet libcurl )
endif()

add_definitions( -DATOMIC_WEB )

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
    set(CEF_ROOT "${CMAKE_SOURCE_DIR}/Submodules/CEF/MacOSX")
  elseif(MSVC)
    if (ATOMIC_PROJECT_ARCH STREQUAL "x86")
    else()
      set(CEF_ROOT "${CMAKE_SOURCE_DIR}/Submodules/CEF/Windows/64bit")
    endif()
  else()
      set(CEF_ROOT "${CMAKE_SOURCE_DIR}/Submodules/CEF/Linux")
  endif()

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
  find_package(CEF REQUIRED)
  include_directories(${CEF_ROOT})

  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

endif()

add_subdirectory(Source)
