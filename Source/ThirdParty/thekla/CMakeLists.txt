
# beware nVidia vendor stuff does not compile with c++11 due to sizeof(bool) != 4 and potentially other issues
set(CMAKE_CXX_FLAGS ${ATOMIC_CXX_FLAGS_DEFAULT})
set(CMAKE_C_FLAGS ${ATOMIC_C_FLAGS_DEFAULT})

add_subdirectory(vendor)

if (APPLE)
    add_definitions(-DNV_OS_DARWIN=1)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Define target name
set (TARGET_NAME thekla)

# Define source files
define_source_files ()

set (INCLUDE_DIRS vendor vendor/poshlib ../STB)

# Setup target
setup_library ()
