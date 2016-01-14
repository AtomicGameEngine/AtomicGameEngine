
include(AtomicCommon)

set (JAVASCRIPT_BINDINGS_PLATFORM "WEB")

add_definitions(-DATOMIC_PLATFORM_WEB)
add_definitions(-DATOMIC_OPENGL -Wno-warn-absolute-paths -DATOMIC_TBUI)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES})
