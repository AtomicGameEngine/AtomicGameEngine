
include (BundleUtilities)
include(AtomicDesktop)

set (JAVASCRIPT_BINDINGS_PLATFORM "MACOSX")

add_definitions(-DATOMIC_PLATFORM_OSX -DATOMIC_OPENGL -DKNET_UNIX)

if (CMAKE_GENERATOR STREQUAL "Xcode")

add_definitions(-DATOMIC_XCODE)

endif()

list (APPEND ATOMIC_LINK_LIBRARIES GLEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")
