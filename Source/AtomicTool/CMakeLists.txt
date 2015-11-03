set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif(MSVC)

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore NETCore NETScript Poco ${ATOMIC_LINK_LIBRARIES})

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

if (ATOMIC_DOTNET_DEV)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/TPA)
 file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/TPA)
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/Build)
 file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/Build)
endif()

if (MSVC)

add_custom_command(TARGET AtomicTool POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/Build/
                   COMMAND $<TARGET_FILE:AtomicTool> net-projectgen "\"${CMAKE_SOURCE_DIR}/Script/AtomicNET/AtomicNETProjects.json\"" WINDOWS
                   COMMAND msbuild
                   COMMENT "Compiling AtomicNET Assemblies")
elseif(APPLE)

if (NOT IOS)

# if we have xbuild available

if(EXISTS "/usr/local/bin/xbuild")

add_custom_command(TARGET AtomicTool POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/Build/
                   COMMAND $<TARGET_FILE:AtomicTool> net-projectgen "\"${CMAKE_SOURCE_DIR}/Script/AtomicNET/AtomicNETProjects.json\"" MACOSX
                   COMMAND /usr/local/bin/xbuild
                   COMMENT "Compiling AtomicNET Assemblies")
endif(xbuild)
endif(NOT IOS)
endif()

endif(ATOMIC_DOTNET_DEV)
