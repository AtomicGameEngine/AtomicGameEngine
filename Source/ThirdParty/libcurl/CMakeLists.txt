
add_definitions(-DCURL_STATICLIB -DUSE_WINDOWS_SSPI -DUSE_SCHANNEL)

include_directories (include lib)

file (GLOB_RECURSE SOURCE_FILES lib/*.c )

add_library(libcurl ${SOURCE_FILES})

