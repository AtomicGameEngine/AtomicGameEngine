

include_directories(include)

file (GLOB SOURCE_FILES src/*.c include/*.h)

add_library(Civetweb ${SOURCE_FILES})



