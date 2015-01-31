
project (Atomic)

cmake_minimum_required (VERSION 3.0.0)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

add_definitions( -DATOMIC_STATIC_DEFINE -DATOMIC_LOGGING -DATOMIC_PHYSICS -DATOMIC_ATOMIC2D )

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Duktape FreeType JO LZ4 PugiXml StanHull STB )

if (NOT EMSCRIPTEN)
    add_definitions( -DATOMIC_NETWORK )
    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} SDL Civetweb Recast Detour kNet )
endif()

if (MSVC)

    include(AtomicWindows)

elseif(APPLE)

    include(AtomicApple)

elseif(ANDROID)

    include(AtomicAndroid)

elseif(EMSCRIPTEN)    
	
	include(AtomicWeb)

endif()

add_subdirectory(Source)

#Build Editor if it is available
if (NOT ANDROID AND NOT EMSCRIPTEN)

    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/AtomicEditor)

        add_subdirectory(AtomicEditor)

    endif()

endif()

