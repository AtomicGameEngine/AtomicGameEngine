file (GLOB CONTAINER_SOURCE Container/*.cpp Container/*.h)
file (GLOB CORE_SOURCE Core/*.cpp Core/*.h)
file (GLOB MATH_SOURCE Math/*.cpp Math/*.h)
file (GLOB IO_SOURCE IO/*.cpp IO/*.h)
file (GLOB ENGINE_SOURCE Engine/*.cpp Engine/*.h)
file (GLOB INPUT_SOURCE Input/*.cpp Input/*.h)
file (GLOB RESOURCE_SOURCE Resource/*.cpp Resource/*.h)
file (GLOB AUDIO_SOURCE Audio/*.cpp Audio/*.h)
file (GLOB NETWORK_SOURCE Network/*.cpp Network/*.h)
file (GLOB WEB_SOURCE Web/*.cpp Web/*.h)
file (GLOB SCRIPT_SOURCE Script/*.cpp Script/*.h)
file (GLOB METRICS_SOURCE Metrics/*.cpp Metrics/*.h)

if (NOT EMSCRIPTEN AND NOT IOS AND NOT ANDROID)
  file (GLOB IPC_SOURCE IPC/*.cpp IPC/*.h)
endif()

file (GLOB ATOMIC2D_SOURCE Atomic2D/*.cpp Atomic2D/*.h)
file (GLOB SCENE_SOURCE Scene/*.cpp Scene/*.h)
file (GLOB UI_SOURCE UI/*.cpp UI/*.h)
file (GLOB SYSTEM_UI_SOURCE UI/SystemUI/*.cpp UI/SystemUI/*.h)

if (NOT ATOMIC_BUILD_2D)
  file (GLOB PHYSICS_SOURCE Physics/*.cpp Physics/*.h)
  file (GLOB NAVIGATION_SOURCE Navigation/*.cpp Navigation/*.h)
  file (GLOB ENVIRONMENT_SOURCE Environment/*.cpp Environment/*.h)
endif()

file (GLOB GRAPHICS_SOURCE Graphics/*.cpp Graphics/*.h)

if (WIN32)
    if (ATOMIC_D3D11) # DirectX 11
        file (GLOB GRAPHICS_IMPL_SOURCE Graphics/Direct3D11/*.cpp Graphics/Direct3D11/*.h)
    elseif (ATOMIC_OPENGL) # OpenGL
	    file (GLOB GRAPHICS_IMPL_SOURCE Graphics/OpenGL/*.cpp Graphics/OpenGL/*.h)
	else() # DirectX 9
        file (GLOB GRAPHICS_IMPL_SOURCE Graphics/Direct3D9/*.cpp Graphics/Direct3D9/*.h)
    endif()
else()
    file (GLOB GRAPHICS_IMPL_SOURCE Graphics/OpenGL/*.cpp Graphics/OpenGL/*.h)
endif()

if (APPLE)
    if (NOT IOS)
        set (PLATFORM_SOURCE IO/MacFileWatcher.mm UI/UIDragDropMac.mm)
    else()
    endif()
endif()

set (SOURCE_FILES ${CONTAINER_SOURCE} ${CORE_SOURCE} ${ENGINE_SOURCE} ${INPUT_SOURCE}
                  ${AUDIO_SOURCE} ${IO_SOURCE} ${MATH_SOURCE}
                  ${RESOURCE_SOURCE} ${PHYSICS_SOURCE} ${IPC_SOURCE}
                  ${GRAPHICS_SOURCE} ${GRAPHICS_IMPL_SOURCE}
                  ${ATOMIC3D_SOURCE}
                  ${ATOMIC2D_SOURCE} ${ENVIRONMENT_SOURCE}
                  ${SCENE_SOURCE} ${UI_SOURCE} ${SYSTEM_UI_SOURCE}
                  ${WEB_SOURCE} ${SCRIPT_SOURCE} ${METRICS_SOURCE}
                  ${PLATFORM_SOURCE})

if (NOT EMSCRIPTEN)
  set (SOURCE_FILES ${SOURCE_FILES} ${NETWORK_SOURCE} ${NAVIGATION_SOURCE})
endif()

GroupSources("Atomic2D")
GroupSources("Atomic3D")
GroupSources("Audio")
GroupSources("Container")
GroupSources("Core")
GroupSources("Engine")
GroupSources("Environment")
GroupSources("Graphics")
GroupSources("Input")
GroupSources("IO")
GroupSources("IPC")
GroupSources("Math")
GroupSources("Navigation")
GroupSources("Network")
GroupSources("Physics")
GroupSources("Resource")
GroupSources("Scene")
GroupSources("UI")
GroupSources("Web")
GroupSources("Script")
GroupSources("Metrics")
GroupSources("BuildInfo")

# Handle Git Revision
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo/AtomicGitSHA.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo/AtomicGitSHA.cpp" @ONLY)

set (SOURCE_FILES ${SOURCE_FILES} BuildInfo/AtomicBuildInfo.cpp BuildInfo/AtomicBuildInfo.h BuildInfo/AtomicGitSHA.cpp BuildInfo/AtomicGitSHA.h)

add_library(Atomic ${SOURCE_FILES})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Atomic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries (Atomic Box2D Duktape TurboBadger FreeType JO LZ4 PugiXml STB SDL)
target_compile_definitions (Atomic PUBLIC -DATOMIC_CXX11=1)
if (ATOMIC_64BIT)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_64BIT=1)
endif ()
if (ANDROID)
    target_link_libraries (Atomic dl log android GLESv1_CM GLESv2)
    target_compile_definitions(Atomic PUBLIC -DATOMIC_PLATFORM_ANDROID=1 -DGLEW_NO_GLU=1)
elseif (APPLE)
    if (IOS)
        target_compile_definitions(Atomic PUBLIC -DATOMIC_PLATFORM_IOS=1 -DIOS=1)
    else () # MacOS
        target_link_libraries (Atomic GLEW)
        target_compile_definitions(Atomic PUBLIC -DATOMIC_PLATFORM_OSX=1)
    endif ()
elseif (LINUX)
    target_link_libraries (Atomic pthread GLEW GL dl)
    target_compile_definitions(Atomic PUBLIC -DATOMIC_PLATFORM_LINUX=1)
elseif (WIN32)
    option (ATOMIC_D3D11 "Use DirectX 11" ON)
    option (ATOMIC_OPENGL "Use OpenGL" OFF)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_PLATFORM_WINDOWS=1)
    target_link_libraries (Atomic user32 gdi32 winmm imm32 ole32 oleaut32 version uuid Ws2_32)
    if (ATOMIC_D3D11) # DirectX 11
        target_compile_definitions (Atomic PUBLIC -DATOMIC_D3D11=1)
        target_link_libraries (Atomic d3d11 d3dcompiler dxguid)
    elseif(ATOMIC_OPENGL) # OpenGL
        find_package(OpenGL REQUIRED)
        include_directories (${OpenGL_INCLUDE_DIRS})
        link_directories (${OpenGL_LIBRARY_DIRS})
        target_compile_definitions (Atomic PRIVATE ${OpenGL_DEFINITIONS})
        target_compile_definitions (Atomic PUBLIC -DGLEW_STATIC=1)
        target_link_libraries (Atomic GLEW opengl32 glu32)
    else() # DirectX 9
        target_link_libraries (Atomic d3d9 d3dcompiler)
        target_compile_definitions (Atomic PUBLIC -DATOMIC_D3D9=1)
        set (ATOMIC_D3D9 1)
    endif()
    target_link_libraries(Atomic MojoShader user32 gdi32 winmm imm32 ole32 oleaut32 version uuid Ws2_32)

    # removes dependency on D3DCompiler dll for Atomic Direct3D9 builds which don't require it
    # (binaries that never initialize the Direct3D9 graphics subsystem)
    option (ATOMIC_D3D9SHADERCOMPILER_DISABLE "Remove D3DCompiler.dll dependency. Use for binaries that never initialize the Direct3D9 graphics subsystem." OFF)
    if (ATOMIC_D3D9SHADERCOMPILER_DISABLE)
        target_compile_definitions (Atomic PUBLIC -DATOMIC_D3D9SHADERCOMPILER_DISABLE=1)
    endif()
elseif (WEB)
    target_compile_definitions(Atomic PUBLIC -DATOMIC_PLATFORM_WEB=1 -DNO_POPEN=1)
endif ()

if (ATOMIC_OPENGL)
    target_compile_definitions(Atomic PUBLIC -DATOMIC_OPENGL=1)
endif ()

if (ATOMIC_DESKTOP)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_PLATFORM_DESKTOP=1 -DATOMIC_NAVIGATION=1 -DATOMIC_FILEWATCHER=1
        -DPOCO_NO_AUTOMATIC_LIBS=1 -DPOCO_STATIC=1)
    target_link_libraries (Atomic libsquish)
    if (NOT LINUX)
        target_link_libraries (Atomic LibCpuId)
    endif()
endif ()

option (ATOMIC_BUILD_2D "Enable 2D support" OFF)
if (ATOMIC_BUILD_2D)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_ATOMIC2D=1)
else ()
    target_compile_definitions (Atomic PUBLIC -DATOMIC_PHYSICS=1 -DATOMIC_3D=1)
    target_link_libraries (Atomic Bullet StanHull)
endif()

if (NOT EMSCRIPTEN)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_THREADING=1 -DATOMIC_NETWORK=1)
    target_link_libraries (Atomic Civetweb Recast Detour DetourCrowd DetourTileCache kNet libcurl)
endif()

option (ATOMIC_PROFILING "Enable profiler" ON)
if (ATOMIC_PROFILING)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_PROFILING=1)
endif ()

option (ATOMIC_LOGGING "Enable logging" ON)
if (ATOMIC_PROFILING)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_LOGGING=1)
endif ()

option (ATOMIC_WEB "Enable web subsystem" ON)
if (ATOMIC_PROFILING)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_WEB=1)
endif ()

# TODO: enable shared library builds.
target_compile_definitions (Atomic PUBLIC -DATOMIC_STATIC_DEFINE=1 -DATOMIC_API=)
if (NOT APPLE OR IOS OR EMSCRIPTEN)
    target_compile_definitions (Atomic PUBLIC -DATOMIC_TBUI=1)
endif ()


target_include_directories (Atomic PUBLIC
    ${ATOMIC_SOURCE_DIR}/Source
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/ASIO/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Assimp/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Box2D
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Bullet/src
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Civetweb/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Detour/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/DetourCrowd/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/DetourTileCache/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Duktape
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/FreeType/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/GLEW
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/JO
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/kNet/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/LibCpuId/src
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/libcurl/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/libsquish
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/LZ4
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/MojoShader
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Mustache
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/nanodbc/src
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/nativefiledialog
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/PugiXml/src
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Recast/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/SDL/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/SQLite/src
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/StanHull
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/STB
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/TurboBadger
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/WebSocketPP/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/zlib
)

include(AtomicDoc)
