include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog)


file (GLOB_RECURSE SOURCE_FILES Source/*.cpp Source/*.h)

add_definitions(-DPOCO_NO_AUTOMATIC_LIBS)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(ToolCore ${SOURCE_FILES})
