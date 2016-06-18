set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif(MSVC)

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore Poco ${ATOMIC_LINK_LIBRARIES})

if (MSVC)
    target_link_libraries(AtomicTool libcurl Iphlpapi Wldap32)

    # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
    # TODO: Remove this dependency
    add_custom_command (TARGET AtomicTool POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicTool>/D3DCompiler_47.dll\")

else()
    target_link_libraries(AtomicTool libcurl)
endif()

add_custom_command( TARGET AtomicTool POST_BUILD
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E make_directory ${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool
                   COMMAND ${CMAKE_COMMAND}
                   ARGS -E copy_if_different $<TARGET_FILE:AtomicTool> \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool/\" )

if (MSVC)

# Copy the D3D shader compiler
add_custom_command( TARGET AtomicTool POST_BUILD
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicTool/D3DCompiler_47.dll\"
                    COMMENT "Copying AtomicTool to Build Artifacts" )
endif(MSVC)

add_custom_target ( GenerateScriptBindings
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                    COMMAND ${ATOMIC_NODE_JAKE};build:genscripts[${JAVASCRIPT_BINDINGS_PLATFORM}]
                    DEPENDS AtomicTool )
