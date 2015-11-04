
include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty )

file (GLOB SOURCE_FILES *.cpp *.h)

if (APPLE)
    file (GLOB_RECURSE PLATFORM_SOURCE_FILES Platforms/Unix/*.cpp Platforms/Unix/*.h)
elseif(MSVC)
    file (GLOB_RECURSE PLATFORM_SOURCE_FILES Platforms/Windows/*.cpp Platforms/Windows/*.h)
elseif(LINUX)
    file (GLOB_RECURSE PLATFORM_SOURCE_FILES Platforms/Unix/*.cpp Platforms/Unix/*.h)
endif()

add_library(NETCore ${SOURCE_FILES} ${PLATFORM_SOURCE_FILES})
