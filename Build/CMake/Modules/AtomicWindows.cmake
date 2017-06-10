
set(JAVASCRIPT_BINDINGS_PLATFORM "WINDOWS")

include(AtomicDesktop)

#set (CMAKE_DEBUG_POSTFIX _d)

set(ATOMIC_NODE_JAKE Build/Windows/node/node.exe Build/node_modules/jake/bin/cli.js -f Build\\Scripts\\Bootstrap.js)

set(D3DCOMPILER_47_DLL ${ATOMIC_SOURCE_DIR}/Build/Windows/Binaries/${ATOMIC_PROJECT_ARCH_SHORT}/D3DCompiler_47.dll)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# compile with static runtime
if (MSVC)
    if (ATOMIC_DYNAMIC_RUNTIME)
        set(ATOMIC_MSVC_RUNTIME /MD)
    else ()
        set(ATOMIC_MSVC_RUNTIME /MT)
    endif ()
    msvc_set_runtime(${ATOMIC_MSVC_RUNTIME})
endif ()
