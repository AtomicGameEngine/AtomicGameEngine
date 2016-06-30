
if (MSVC)

    include(AtomicWindows)

elseif(APPLE)

    if (IOS)
        include(AtomicIOS)
    else()
        include(AtomicMac)
    endif()

elseif(LINUX)

  include(AtomicLinux)

elseif(ANDROID)

    include(AtomicAndroid)

elseif(EMSCRIPTEN)

	include(AtomicWeb)

endif()

set (JAVASCRIPT_BINDINGS_PLATFORM_ROOT "${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}")

if(NOT EXISTS "${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/Javascript")

execute_process ( COMMAND ${ATOMIC_NODE_JAKE};build:precreateScriptBindings[${JAVASCRIPT_BINDINGS_PLATFORM}]
                  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" )

endif()

file (GLOB_RECURSE JAVASCRIPT_BINDINGS_NATIVE_FILENAMES ${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/*.cpp ${JAVASCRIPT_BINDINGS_PLATFORM_ROOT}/*.h)
