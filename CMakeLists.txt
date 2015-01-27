
project (Atomic)

cmake_minimum_required (VERSION 3.0.0)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

add_definitions(-DATOMIC_STATIC_DEFINE -DATOMIC_PHYSICS -DATOMIC_NETWORK -DATOMIC_ATOMIC2D)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Civetweb Detour Duktape FreeType JO kNet LZ4 
						   PugiXml Recast SDL  StanHull STB )

if (MSVC)

    include(AtomicWindows)

elseif(APPLE)

    include(AtomicApple)

elseif(ANDROID)

    include(AtomicAndroid)

endif()

add_subdirectory(Source)

#Build Editor if it is available
if (NOT ANDROID)

    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/AtomicEditor)

        add_subdirectory(AtomicEditor)

    endif()

endif()

