
if (APPLE)
  include(LibCurl.cmake)
else()
  include(LibCurlWindows.cmake)
endif()
