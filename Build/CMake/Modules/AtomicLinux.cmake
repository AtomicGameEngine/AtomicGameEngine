set (JAVASCRIPT_BINDINGS_PLATFORM "LINUX")
set (ATOMIC_NODE_JAKE Build/Linux/node/node Build/node_modules/jake/bin/cli.js  -f  Build/Scripts/Bootstrap.js)

include(AtomicDesktop)

add_definitions(-DATOMIC_PLATFORM_LINUX -DATOMIC_OPENGL -DKNET_UNIX -DHAVE_INT64_T)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x -fPIC")


find_package(PkgConfig REQUIRED)

#for native file dialog
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

list (APPEND ATOMIC_LINK_LIBRARIES pthread GLEW GL dl)


add_definitions(-DATOMIC_PLATFORM_LINUX)
add_definitions(-DATOMIC_OPENGL -Wno-warn-absolute-paths -DATOMIC_TBUI)
