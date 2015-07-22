set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif()

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore Poco ${ATOMIC_LINK_LIBRARIES})

# add_dependencies(AtomicTool AtomicEditor)

if (MSVC)
    target_link_libraries(AtomicTool libcurl Iphlpapi Wldap32)

    # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
    # TODO: Remove this dependency
    add_custom_command (TARGET AtomicTool POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicTool>/D3DCompiler_47.dll\")

else()
    target_link_libraries(AtomicTool curl)
endif()


add_custom_command(TARGET AtomicTool POST_BUILD
                   COMMAND $<TARGET_FILE:AtomicTool> bind "\"${CMAKE_SOURCE_DIR}\"" "Script/Packages/Atomic/" "${JAVASCRIPT_BINDINGS_PLATFORM}"
                   COMMAND $<TARGET_FILE:AtomicTool> bind "\"${CMAKE_SOURCE_DIR}\"" "Script/Packages/AtomicPlayer/" "${JAVASCRIPT_BINDINGS_PLATFORM}"
                   COMMAND $<TARGET_FILE:AtomicTool> bind "\"${CMAKE_SOURCE_DIR}\"" "Script/Packages/ToolCore/" "${JAVASCRIPT_BINDINGS_PLATFORM}"
                   COMMAND $<TARGET_FILE:AtomicTool> bind "\"${CMAKE_SOURCE_DIR}\"" "Script/Packages/Editor/" "${JAVASCRIPT_BINDINGS_PLATFORM}"
                   COMMENT "Generating Javascript Bindings")
