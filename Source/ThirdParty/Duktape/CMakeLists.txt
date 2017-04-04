
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
file (GLOB CPP_FILES *.c)
file (GLOB H_FILES *.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})
set (INCLUDE_DIRS .)
add_library(Duktape ${SOURCE_FILES})
