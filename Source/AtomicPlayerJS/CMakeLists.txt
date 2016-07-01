include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/AtomicPlayer/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(AtomicPlayerJS ${SOURCE_FILES})

target_link_libraries(AtomicPlayerJS AtomicPlayerLib)

add_dependencies(AtomicPlayerJS AtomicToolCheckScripts)
