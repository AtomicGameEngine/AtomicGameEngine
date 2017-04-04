
set(JAVASCRIPT_BINDINGS_PLATFORM "WINDOWS")

include(AtomicDesktop)

#set (CMAKE_DEBUG_POSTFIX _d)

set(ATOMIC_NODE_JAKE Build/Windows/node/node.exe Build/node_modules/jake/bin/cli.js -f Build\\Scripts\\Bootstrap.js)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(D3DCOMPILER_47_DLL ${ATOMIC_SOURCE_DIR}/Build/Windows/Binaries/x64/D3DCompiler_47.dll)
else ()
    set(D3DCOMPILER_47_DLL ${ATOMIC_SOURCE_DIR}/Build/Windows/Binaries/x86/D3DCompiler_47.dll)
endif ()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)


# compile with static runtime
set(CompilerFlags
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_MINSIZEREL)

foreach (CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach ()
