
project (Atomic)

cmake_minimum_required (VERSION 3.0.2)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

add_definitions( -DATOMIC_API= -DATOMIC_STATIC_DEFINE -DATOMIC_ATOMIC2D )

# this is here as QtCreator is having trouble picking up #include <Atomic/*> without it
include_directories(${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/AtomicEditor/Source)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Duktape FreeType JO LZ4 PugiXml  STB)

if (NOT ATOMIC_BUILD_2D)
    add_definitions( -DATOMIC_PHYSICS -DATOMIC_3D)
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} Bullet StanHull)
endif()

if (NOT EMSCRIPTEN)
    add_definitions( -DATOMIC_NETWORK -DATOMIC_LOGGING)
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} SDL Civetweb Recast Detour kNet )
endif()

if (MSVC)

    include(AtomicWindows)
    
elseif(APPLE)

    if (IOS)
        include(AtomicIOS)
    else()
        include(AtomicMac)
    endif()

elseif(ANDROID)

    include(AtomicAndroid)

elseif(EMSCRIPTEN)    

	include(AtomicWeb)

endif()

add_subdirectory(Source)
