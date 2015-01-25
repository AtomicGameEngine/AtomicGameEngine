
project (Atomic)

cmake_minimum_required (VERSION 2.8.6)

add_definitions(-DATOMIC_PHYSICS -DATOMIC_NETWORK -DATOMIC_ATOMIC2D)

if (MSVC)

set (CMAKE_DEBUG_POSTFIX _d)

else()

add_definitions(-DATOMIC_OPENGL)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

endif()


add_subdirectory(Source)

