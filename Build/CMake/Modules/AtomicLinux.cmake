set(JAVASCRIPT_BINDINGS_PLATFORM "LINUX")
set(ATOMIC_NODE_JAKE Build/Linux/node/node Build/node_modules/jake/bin/cli.js -f Build/Scripts/Bootstrap.js)

include(AtomicDesktop)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-warn-absolute-paths -Wno-invalid-offsetof -std=gnu++0x -fPIC")

find_package(PkgConfig REQUIRED)
# for native file dialog
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

