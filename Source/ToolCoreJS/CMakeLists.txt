file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/ToolCore/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(ToolCoreJS ${SOURCE_FILES})

target_link_libraries(ToolCoreJS ToolCore Assimp)

add_dependencies(ToolCoreJS AtomicToolCheckScripts)
