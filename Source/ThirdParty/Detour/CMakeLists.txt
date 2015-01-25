

include_directories(include)

file (GLOB SOURCE_FILES source/*.cpp include/*.h)

add_library(Detour ${SOURCE_FILES})


