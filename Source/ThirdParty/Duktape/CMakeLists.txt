
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
file (GLOB CPP_FILES *.c)
file (GLOB H_FILES *.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

add_library(Duktape ${SOURCE_FILES})

if (MSVC)
    # technically, we could reuse the Duktape lib, but we'll compile it directly
    # for instance, if we want to change settings slightly for native plugins
	add_library(AtomicPlugin ${SOURCE_FILES})
	add_custom_command (TARGET AtomicPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${CMAKE_CURRENT_SOURCE_DIR}/duktape.h\" \"${CMAKE_SOURCE_DIR}/Bin/NativePluginSDK/Windows/AtomicPlugin.h\"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy \"$<TARGET_LINKER_FILE:AtomicPlugin>\" \"${CMAKE_SOURCE_DIR}/Bin/NativePluginSDK/Windows/x64/$<TARGET_LINKER_FILE_NAME:AtomicPlugin>\")
endif()
