
project (Atomic)

cmake_minimum_required (VERSION 2.8.6)

add_definitions(-DATOMIC_PHYSICS -DATOMIC_NETWORK -DATOMIC_ATOMIC2D)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Civetweb Detour Duktape FreeType JO kNet
                           LibCpuId LZ4 PugiXml Recast SDL SQLite StanHull STB TurboBadger)

if (MSVC)

set (CMAKE_DEBUG_POSTFIX _d)

else()

add_definitions(-DATOMIC_OPENGL)
list (APPEND ATOMIC_LINK_LIBRARIES GLEW)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

endif()

if (APPLE)

    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices")

endif()


add_subdirectory(Source)

