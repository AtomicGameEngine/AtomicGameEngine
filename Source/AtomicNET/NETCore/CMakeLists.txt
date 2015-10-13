
include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Assimp/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog)

file (GLOB SOURCE_FILES *.cpp *.h)

if (APPLE)
    file (GLOB_RECURSE PLATFORM_SOURCE_FILES Platforms/Unix/*.cpp Platforms/Unix/*.h)
elseif(MSVC)
    file (GLOB_RECURSE PLATFORM_SOURCE_FILES Platforms/Windows/*.cpp Platforms/Windows/*.h)
endif()

add_library(NETCore ${SOURCE_FILES} ${PLATFORM_SOURCE_FILES})
