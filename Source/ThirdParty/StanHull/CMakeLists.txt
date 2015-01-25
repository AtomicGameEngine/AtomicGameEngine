

include_directories(include)

file (GLOB SOURCE_FILES *.cpp *.h)

add_library(StanHull ${SOURCE_FILES})



