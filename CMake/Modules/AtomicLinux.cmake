include(AtomicDesktop)

set (JAVASCRIPT_BINDINGS_PLATFORM "LINUX")

add_definitions(-DATOMIC_PLATFORM_LINUX -DATOMIC_OPENGL -DKNET_UNIX -DHAVE_INT64_T)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

find_package(PkgConfig REQUIRED)

#for native file dialog
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

list (APPEND ATOMIC_LINK_LIBRARIES pthread GLEW GL dl)


add_definitions(-DATOMIC_PLATFORM_LINUX)
add_definitions(-DATOMIC_OPENGL -Wno-warn-absolute-paths -DATOMIC_TBUI)
