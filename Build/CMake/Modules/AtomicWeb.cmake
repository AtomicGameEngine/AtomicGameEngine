
include(AtomicCommon)

set (JAVASCRIPT_BINDINGS_PLATFORM "WEB")

add_definitions(-DATOMIC_PLATFORM_WEB)
add_definitions(-DATOMIC_OPENGL -Wno-warn-absolute-paths -DATOMIC_TBUI -DNO_POPEN)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths -Wno-unknown-warning-option")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x -Wno-warn-absolute-paths -Wno-unknown-warning-option")

set (CMAKE_EXECUTABLE_SUFFIX_C .html)
set (CMAKE_EXECUTABLE_SUFFIX_CXX .html)

# Linker flags
set (MEMORY_LINKER_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
#set (MEMORY_LINKER_FLAGS "-s TOTAL_MEMORY=${EMSCRIPTEN_TOTAL_MEMORY}")

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MEMORY_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -O3 -s AGGRESSIVE_VARIABLE_ELIMINATION=1")     # Remove variables to make the -O3 regalloc easier
set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g4")     # Preserve LLVM debug information, show line number debug comments, and generate source maps



set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES})
