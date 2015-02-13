

add_definitions(-DUSE_WINDOWS_SSPI -DUSE_SCHANNEL)

include_directories (lib)

file (GLOB_RECURSE SOURCE_FILES lib/*.c )

add_library(libcurl ${SOURCE_FILES})

