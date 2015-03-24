set (ATOMIC_TOOL_SOURCES AtomicTool.cpp AtomicTool.h)

add_executable(AtomicTool ${ATOMIC_TOOL_SOURCES})

target_link_libraries(AtomicTool ${ATOMIC_LINK_LIBRARIES} ToolCore Poco curl)


