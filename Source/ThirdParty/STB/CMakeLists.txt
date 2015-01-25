# Define source files
file (GLOB C_FILES *.c)
file (GLOB H_FILES *.h)

set (SOURCE_FILES ${C_FILES} ${H_FILES})

add_library(STB ${SOURCE_FILES})

