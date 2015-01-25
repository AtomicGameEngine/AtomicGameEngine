

include_directories(src)

file (GLOB SOURCE_FILES src/*.c src/*.h)

add_library(LibCpuId ${SOURCE_FILES})
