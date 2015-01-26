
project (Atomic)

cmake_minimum_required (VERSION 3.0.0)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

add_definitions(-DATOMIC_STATIC_DEFINE -DATOMIC_PHYSICS -DATOMIC_NETWORK -DATOMIC_ATOMIC2D -DATOMIC_TBUI)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Civetweb Detour Duktape FreeType JO kNet
                           LibCpuId LZ4 PugiXml Recast SDL SQLite StanHull STB TurboBadger)

if (MSVC)

    include(AtomicWindows)

elseif(APPLE)

    include(AtomicApple)

endif()

add_subdirectory(Source)

if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/AtomicEditor)

    add_subdirectory(AtomicEditor)

endif()

