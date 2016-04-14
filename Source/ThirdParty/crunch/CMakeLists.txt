include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
file (GLOB CRNLIB_CPP_FILES crnlib/*.cpp)
file (GLOB CRNLIB_H_FILES crnlib/*.h)
file (GLOB INC_H_FILES inc/*.h)

# Remove windows only files
if (NOT OS_WINDOWS)

    list(REMOVE_ITEM CRNLIB_CPP_FILES "crnlib/lzma_LzFindMt.cpp")
    list(REMOVE_ITEM CRNLIB_CPP_FILES "crnlib/lzma_Threads.cpp")
    list(REMOVE_ITEM CRNLIB_H_FILES "crnlib/lzma_LzFindMt.h")
    list(REMOVE_ITEM CRNLIB_H_FILES "crnlib/lzma_Threads.h")

endif ()

set (SOURCE_FILES ${CRNLIB_CPP_FILES} ${CRNLIB_H_FILES} ${INC_H_FILES})

# These are used to create visual studio folders.
source_group(crnlib FILES ${CRNLIB_CPP_FILES} ${CRNLIB_H_FILES})
source_group(inc FILES ${INC_H_FILES})


add_library(crunch ${SOURCE_FILES})

