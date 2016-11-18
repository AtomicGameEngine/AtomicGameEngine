include_directories (${ATOMIC_SOURCE_DIR}/Source/ThirdParty ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Assimp/include)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/ToolCore/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(ToolCoreJS ${SOURCE_FILES})

target_link_libraries(ToolCoreJS ToolCore)

add_dependencies(ToolCoreJS AtomicToolCheckScripts)
