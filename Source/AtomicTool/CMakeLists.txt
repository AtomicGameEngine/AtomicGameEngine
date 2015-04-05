set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif()

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ToolCore Poco ${ATOMIC_LINK_LIBRARIES})

add_dependencies(AtomicTool AtomicEditor)

if (MSVC)
    target_link_libraries(AtomicTool libcurl Iphlpapi Wldap32)
else()
    target_link_libraries(AtomicTool curl)
endif()
