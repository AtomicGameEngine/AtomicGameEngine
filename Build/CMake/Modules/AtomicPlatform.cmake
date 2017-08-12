
set(ATOMIC_DYNAMIC_RUNTIME OFF CACHE BOOL "Build engine as shared library and link dynamically to system runtime.")

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set(ATOMIC_NODE_JAKE Build/Mac/node/node Build/node_modules/jake/bin/cli.js -f Build/Scripts/Bootstrap.js)
endif()
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    set(ATOMIC_NODE_JAKE Build/Linux/node/node Build/node_modules/jake/bin/cli.js -f Build/Scripts/Bootstrap.js)
endif()
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(ATOMIC_NODE_JAKE Build/Windows/node/node.exe Build/node_modules/jake/bin/cli.js -f Build\\Scripts\\Bootstrap.js)
endif()


if (WIN32)
    include(AtomicWindows)
elseif (APPLE)
    if (IOS)
        include(AtomicIOS)
    else ()
        include(AtomicMac)
    endif ()
elseif (LINUX)
    include(AtomicLinux)
elseif (ANDROID)
    include(AtomicAndroid)
elseif (WEB)
    include(AtomicWeb)
endif ()

if (NOT WIN32)
    set(ATOMIC_OPENGL 1)
endif ()

message(STATUS "Atomic platform: ${JAVASCRIPT_BINDINGS_PLATFORM}")

set(JAVASCRIPT_BINDINGS_PLATFORM_ROOT "${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated")

if (NOT EXISTS "${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/Javascript")
    execute_process(COMMAND ${ATOMIC_NODE_JAKE};build:precreateScriptBindings[${JAVASCRIPT_BINDINGS_PLATFORM}]
        WORKING_DIRECTORY "${ATOMIC_SOURCE_DIR}")
endif ()

file(GLOB_RECURSE JAVASCRIPT_BINDINGS_NATIVE_FILENAMES ${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/*.cpp ${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/*.h)
