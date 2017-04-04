file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicApp ${SOURCE_FILES})
target_link_libraries(AtomicApp Atomic)
if (ATOMIC_DESKTOP)
    add_dependencies(AtomicApp AtomicToolCheckScripts)
endif ()
GroupSources("Player")
