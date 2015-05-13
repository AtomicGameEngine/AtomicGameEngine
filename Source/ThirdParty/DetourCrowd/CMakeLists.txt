
include_directories(include ${CMAKE_SOURCE_DIR}/Source/ThirdParty ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Detour/include )


file (GLOB SOURCE_FILES source/*.cpp include/*.h)

add_library(DetourCrowd ${SOURCE_FILES})
