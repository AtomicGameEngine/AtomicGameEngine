
if (WIN32)
    include(LibCurlWindows.cmake)
else()
    include(LibCurl.cmake)
endif()
