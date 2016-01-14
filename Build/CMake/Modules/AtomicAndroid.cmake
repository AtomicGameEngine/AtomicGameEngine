
include(AtomicCommon)

set (JAVASCRIPT_BINDINGS_PLATFORM "ANDROID")

add_definitions(-DATOMIC_PLATFORM_ANDROID)
add_definitions(-DATOMIC_OPENGL -DKNET_UNIX -DATOMIC_TBUI)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x -fstack-protector")

list (APPEND ATOMIC_LINK_LIBRARIES dl log android GLESv1_CM GLESv2)
