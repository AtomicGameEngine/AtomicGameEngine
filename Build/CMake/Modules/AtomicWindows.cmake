
set (JAVASCRIPT_BINDINGS_PLATFORM "WINDOWS")

include(AtomicDesktop)

#set (CMAKE_DEBUG_POSTFIX _d)

set (ATOMIC_NODE_JAKE Build/Windows/node/node.exe Build/node_modules/jake/bin/cli.js  -f  Build\\Scripts\\Bootstrap.js)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set (D3DCOMPILER_47_DLL ${CMAKE_SOURCE_DIR}/Build/Windows/Binaries/x64/D3DCompiler_47.dll)
else()
    set (D3DCOMPILER_47_DLL ${CMAKE_SOURCE_DIR}/Build/Windows/Binaries/x86/D3DCompiler_47.dll)
endif()

add_definitions(-DATOMIC_PLATFORM_WINDOWS -D_CRT_SECURE_NO_WARNINGS -DATOMIC_TBUI )

list (APPEND ATOMIC_LINK_LIBRARIES MojoShader user32 gdi32 winmm imm32 ole32 oleaut32 version uuid Ws2_32)

if (ATOMIC_D3D11)

    add_definitions(-DATOMIC_D3D11)

    list (APPEND ATOMIC_LINK_LIBRARIES d3d11 d3dcompiler dxguid)

else()

    list (APPEND ATOMIC_LINK_LIBRARIES  d3d9 d3dcompiler)

endif()

# removes dependency on D3DCompiler dll for Atomic Direct3D9 builds which don't require it
# (binaries that never initialize the Direct3D9 graphics subsystem)
if (ATOMIC_D3D9SHADERCOMPILER_DISABLE)
  add_definitions(-DATOMIC_D3D9SHADERCOMPILER_DISABLE)
endif()


# compile with static runtime
set( CompilerFlags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO )

foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()
