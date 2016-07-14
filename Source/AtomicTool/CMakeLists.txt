set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif(MSVC)

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore AtomicNETScript Poco ${ATOMIC_LINK_LIBRARIES})

if (MSVC)
    target_link_libraries(AtomicTool libcurl Iphlpapi Wldap32)
else()
    target_link_libraries(AtomicTool libcurl)
endif()

# Custom target which is always considered out of date, checks for zero size/missing script bindings and generates
add_custom_target( AtomicToolCheckScripts
                   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                   COMMAND "${CMAKE_COMMAND}" -E make_directory \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different \"$<TARGET_FILE:AtomicTool>\" \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool/\"
                   COMMAND ${ATOMIC_NODE_JAKE};build:genscripts[${JAVASCRIPT_BINDINGS_PLATFORM},false] )

add_dependencies (AtomicToolCheckScripts AtomicTool)

# Copy AtomicTool to Artifacts
add_custom_command( TARGET AtomicTool POST_BUILD
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E copy_if_different \"$<TARGET_FILE:AtomicTool>\" \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool/\"
                    COMMENT "Copying AtomicTool to Build Artifacts" )

if (MSVC)

# Copy the D3D shader compiler (for pre-Windows 8)
add_custom_command( TARGET AtomicTool POST_BUILD
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicTool>/D3DCompiler_47.dll\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool\"
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool/D3DCompiler_47.dll\"
                    COMMENT "Copying D3DShader compiler to Build Artifacts" )
endif(MSVC)

add_custom_target ( GenerateScriptBindings
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                    COMMAND ${ATOMIC_NODE_JAKE};build:genscripts[${JAVASCRIPT_BINDINGS_PLATFORM},true];--trace
                    DEPENDS AtomicTool )

add_custom_target ( GenerateAtomicNET
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                    COMMAND ${ATOMIC_NODE_JAKE};build:genAtomicNET[${JAVASCRIPT_BINDINGS_PLATFORM},$<$<CONFIG:debug>:"Debug">$<$<CONFIG:release>:"Release">];--trace
                    DEPENDS AtomicTool )
