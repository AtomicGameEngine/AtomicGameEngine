
set (JAVASCRIPT_BINDINGS_PLATFORM "WINDOWS")

include(AtomicDesktop)

#set (CMAKE_DEBUG_POSTFIX _d)

set (ATOMIC_NODE_JAKE Build/Windows/node/node.exe Build/node_modules/jake/bin/cli.js  -f  Build\\Scripts\\Bootstrap.js)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set (D3DCOMPILER_47_DLL ${ATOMIC_SOURCE_DIR}/Build/Windows/Binaries/x64/D3DCompiler_47.dll)
else()
    set (D3DCOMPILER_47_DLL ${ATOMIC_SOURCE_DIR}/Build/Windows/Binaries/x86/D3DCompiler_47.dll)
endif()

add_definitions_exported(-DATOMIC_PLATFORM_WINDOWS -DATOMIC_TBUI )
add_definitions(-D_CRT_SECURE_NO_WARNINGS )

add_link_libraries_exported(MojoShader user32 gdi32 winmm imm32 ole32 oleaut32 version uuid Ws2_32)

if (ATOMIC_D3D11)
    add_definitions_exported(-DATOMIC_D3D11)
    add_link_libraries_exported(d3d11 d3dcompiler dxguid)
else()
    add_link_libraries_exported(d3d9 d3dcompiler)
endif()

# removes dependency on D3DCompiler dll for Atomic Direct3D9 builds which don't require it
# (binaries that never initialize the Direct3D9 graphics subsystem)
if (ATOMIC_D3D9SHADERCOMPILER_DISABLE)
  add_definitions_exported(-DATOMIC_D3D9SHADERCOMPILER_DISABLE)
endif()


# compile with static runtime
set( CompilerFlags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO )

foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()
