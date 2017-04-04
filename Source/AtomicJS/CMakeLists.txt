
file (GLOB JAVASCRIPT_SOURCE Javascript/*.cpp Javascript/*.h)

GroupSources("Javascript")

if (NOT WIN32)
    # for kNet
    add_definitions (-DUNIX)
endif()

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/Atomic/*.cpp)

set (SOURCE_FILES ${JAVASCRIPT_SOURCE} ${JAVASCRIPT_BINDINGS_SOURCE})

add_library(AtomicJS ${SOURCE_FILES})
target_link_libraries(AtomicJS Atomic)
if (ATOMIC_DESKTOP)
    add_dependencies(AtomicJS AtomicToolCheckScripts)
endif ()
