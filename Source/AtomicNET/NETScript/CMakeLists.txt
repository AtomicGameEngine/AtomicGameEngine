
include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty )

file (GLOB SOURCE_FILES *.cpp *.h)

add_library(NETScript ${SOURCE_FILES})
