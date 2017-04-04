set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif(WIN32)

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore AtomicNETScript Poco Atomic)

if (WIN32)
    target_link_libraries(AtomicTool Iphlpapi Wldap32)
endif()
target_link_libraries(AtomicTool libcurl)

# Custom target which is always considered out of date, checks for zero size/missing script bindings and generates
add_custom_target( AtomicToolCheckScripts
                   WORKING_DIRECTORY "${ATOMIC_SOURCE_DIR}"
                   COMMAND "${CMAKE_COMMAND}" -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different \"$<TARGET_FILE:AtomicTool>\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool/\"
                   COMMAND ${ATOMIC_NODE_JAKE};build:genscripts[false] )

add_dependencies (AtomicToolCheckScripts AtomicTool)

# Copy AtomicTool to Artifacts
add_custom_command( TARGET AtomicTool POST_BUILD
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E copy_if_different \"$<TARGET_FILE:AtomicTool>\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool/\"
                    COMMENT "Copying AtomicTool to Build Artifacts" )

if (WIN32)

# Copy the D3D shader compiler (for pre-Windows 8)
add_custom_command( TARGET AtomicTool POST_BUILD
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicTool>/D3DCompiler_47.dll\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicTool/D3DCompiler_47.dll\"
                    COMMENT "Copying D3DShader compiler to Build Artifacts" )
endif(WIN32)

add_custom_target ( GenerateScriptBindings
                    WORKING_DIRECTORY "${ATOMIC_SOURCE_DIR}"
                    COMMAND ${ATOMIC_NODE_JAKE};build:genscripts[true];--trace
                    DEPENDS AtomicTool )

add_custom_target ( GenerateAtomicNET
                    WORKING_DIRECTORY "${ATOMIC_SOURCE_DIR}"
                    COMMAND ${ATOMIC_NODE_JAKE};build:genAtomicNET[desktop,$<$<CONFIG:debug>:"Debug">$<$<CONFIG:release>:"Release">];--trace
                    DEPENDS AtomicTool )
