
set (JAVASCRIPT_BINDINGS_PLATFORM "MACOSX")
set (ATOMIC_NODE_JAKE Build/Mac/node/node Build/node_modules/jake/bin/cli.js  -f  Build/Scripts/Bootstrap.js)

include (BundleUtilities)
include(AtomicDesktop)

# only have 32 bit mono installed, fix this
# set (CMAKE_OSX_ARCHITECTURES i386)

set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET  "10.9")

add_definitions(-DATOMIC_PLATFORM_OSX -DATOMIC_OPENGL -DKNET_UNIX)

if (CMAKE_GENERATOR STREQUAL "Xcode")
    add_definitions(-DATOMIC_XCODE)
endif()

list (APPEND ATOMIC_LINK_LIBRARIES GLEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++11")

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")
