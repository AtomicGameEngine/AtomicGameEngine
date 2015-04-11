


file (GLOB HEADER_FILES *.h)

if (MSVC)
  set (SOURCE_FILES pipe_win.cpp)
else()
  set (SOURCE_FILES pipe_unix.cpp)
endif()

add_library(SimpleIPC ${SOURCE_FILES} ${HEADER_FILES})
