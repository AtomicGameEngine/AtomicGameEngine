set (ATOMIC_CLI_SOURCES CLI.cpp CLI.h)

add_executable(atomic-cli ${ATOMIC_CLI_SOURCES})

target_link_libraries(atomic-cli ${ATOMIC_LINK_LIBRARIES} ToolCore Poco curl)


