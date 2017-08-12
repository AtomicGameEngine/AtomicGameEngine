set(JAVASCRIPT_BINDINGS_PLATFORM "LINUX")

include(AtomicDesktop)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -fPIC")

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-warn-absolute-paths")
endif()

find_package(PkgConfig REQUIRED)
# for native file dialog
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

