
project (Atomic)

cmake_minimum_required (VERSION 2.8.6)

include (CMakeDependentOption)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

cmake_dependent_option (ATOMIC_STATIC_RUNTIME "Use static C/C++ runtime libraries and eliminate the need for runtime DLLs installation (VS only)" TRUE "MSVC" FALSE)

add_definitions(-DATOMIC_STATIC_DEFINE -DATOMIC_PHYSICS -DATOMIC_NETWORK -DATOMIC_ATOMIC2D -DATOMIC_TBUI)

set (ATOMIC_LINK_LIBRARIES Atomic Box2D Bullet Civetweb Detour Duktape FreeType JO kNet
                           LibCpuId LZ4 PugiXml Recast SDL SQLite StanHull STB TurboBadger)

if (MSVC)

    set (CMAKE_DEBUG_POSTFIX _d)

    if (ATOMIC_STATIC_RUNTIME)
        set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
        )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()
    endif ()

    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} /fp:fast /Zi /GS-")
    set (CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /fp:fast /Zi /GS- /D _SECURE_SCL=0")
    set (CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    # SSE flag is redundant if already compiling as 64bit
    if (ATOMIC_SSE AND NOT ATOMIC_64BIT)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE")
    endif ()
    set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /DEBUG")
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")

    list (APPEND ATOMIC_LINK_LIBRARIES MojoShader user32 gdi32 winmm imm32 ole32 oleaut32 version uuid d3d9 d3dcompiler)

else()

    add_definitions(-DATOMIC_OPENGL)
    list (APPEND ATOMIC_LINK_LIBRARIES GLEW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

endif()

if (APPLE)

    include (BundleUtilities)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices")

endif()

add_subdirectory(Source)

#if (IS_DIRECTORY ./AtomicEditor)

add_subdirectory(AtomicEditor)

#endif()

