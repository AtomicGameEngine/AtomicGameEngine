
project (Atomic)

cmake_minimum_required (VERSION 3.0.0)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

add_definitions( -DATOMIC_STATIC_DEFINE -DATOMIC_LOGGING -DATOMIC_PHYSICS -DATOMIC_ATOMIC2D )


# this is here as QtCreator is having trouble picking up #include <Atomic/*> without it
include_directories(${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/AtomicEditor/Source)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Duktape FreeType JO LZ4 PugiXml StanHull STB )

if (NOT EMSCRIPTEN)
    add_definitions( -DATOMIC_NETWORK )
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} SDL Civetweb Recast Detour kNet )
endif()

if (NOT MSVC)
    add_definitions(-DATOMIC_API=)
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
