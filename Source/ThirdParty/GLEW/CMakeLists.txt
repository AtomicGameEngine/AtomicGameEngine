

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file (GLOB SOURCE_FILES *.c)

add_library(GLEW ${SOURCE_FILES})