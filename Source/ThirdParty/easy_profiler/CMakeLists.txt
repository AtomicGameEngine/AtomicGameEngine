cmake_minimum_required(VERSION 2.8.12.1)
project(easy_profiler CXX)

set(EASY_PROGRAM_VERSION_MAJOR 1)
set(EASY_PROGRAM_VERSION_MINOR 3)
set(EASY_PROGRAM_VERSION_PATCH 0)
set(EASY_PRODUCT_VERSION_STRING "${EASY_PROGRAM_VERSION_MAJOR}.${EASY_PROGRAM_VERSION_MINOR}.${EASY_PROGRAM_VERSION_PATCH}")

# ATOMIC BEGIN

if (ATOMIC_DESKTOP)
    find_package(Qt5Widgets)
endif ()

set(EASY_OPTION_LIB_STATIC ON CACHE BOOL "" FORCE)
set(EASY_OPTION_PREDEFINED_COLORS ON CACHE BOOL "" FORCE)
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "" FORCE)
set(BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK OFF CACHE BOOL "" FORCE)

# force std::chrono::high_resolution_clock usage on iOS
if (IOS)
    set(BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK ON FORCE)
endif()

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${easy_profiler_SOURCE_DIR}")
    # Do not set these paths if easy_profiler is added as dependency to other projects.
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
    # set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/sdk)
endif ()
# ATOMIC END

macro(easy_define_target_option TARGET SOURCE_OPTION TARGET_DEFINITION)
    if (${SOURCE_OPTION})
        set(_VALUE 1)
    else ()
        set(_VALUE 0)
    endif ()
    target_compile_options(${TARGET} PUBLIC -D${TARGET_DEFINITION}=${_VALUE})
endmacro()

add_subdirectory(easy_profiler_core)

# ATOMIC BEGIN
# Only include the Qt client if on we're building desktop platform and Qt5 was found on system
if (Qt5Widgets_FOUND)
    add_subdirectory(profiler_gui)
endif()
#ATOMIC END

if (NOT EASY_PROFILER_NO_SAMPLES)
    add_subdirectory(sample)
    add_subdirectory(reader)
endif ()
