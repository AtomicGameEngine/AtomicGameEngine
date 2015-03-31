set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

if (MSVC)
    set (EXE_TYPE WIN32)
endif()

add_executable(AtomicTool ${EXE_TYPE} ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ${ATOMIC_LINK_LIBRARIES} ToolCore Poco)

add_dependencies(AtomicTool AtomicEditor)

if (MSVC)
    target_link_libraries(AtomicTool libcurl Iphlpapi Wldap32)
else()
    target_link_libraries(AtomicTool curl)
endif()
