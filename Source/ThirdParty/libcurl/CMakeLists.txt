
if (MSVC)
    include(LibCurlWindows.cmake)
else()
    include(LibCurl.cmake)
endif()
