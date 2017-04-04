file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicApp ${SOURCE_FILES})
target_link_libraries(AtomicApp Atomic)
add_dependencies(AtomicApp AtomicToolCheckScripts)

GroupSources("Player")
