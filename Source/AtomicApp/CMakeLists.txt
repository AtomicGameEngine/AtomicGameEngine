
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicApp ${SOURCE_FILES})

add_dependencies(AtomicApp AtomicToolCheckScripts)

GroupSources("Player")
