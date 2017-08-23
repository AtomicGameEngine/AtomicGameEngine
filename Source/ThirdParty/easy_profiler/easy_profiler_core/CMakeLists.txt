message(STATUS "")
message(STATUS "EASY_PROFILER.Core version = ${EASY_PRODUCT_VERSION_STRING}")
message(STATUS "")

set(EASY_DEFAULT_PORT                  28077  CACHE STRING "Default listening port")
set(EASY_OPTION_LISTEN                 OFF    CACHE BOOL   "Enable automatic startListen on startup")
set(EASY_OPTION_PROFILE_SELF           OFF    CACHE BOOL   "Enable self profiling (measure time for internal storage expand)")
set(EASY_OPTION_PROFILE_SELF_BLOCKS_ON OFF    CACHE BOOL   "Storage expand default status (profiler::ON or profiler::OFF)")
set(EASY_OPTION_LOG                    OFF    CACHE BOOL   "Print errors to stderr")
set(EASY_OPTION_PREDEFINED_COLORS      ON     CACHE BOOL   "Use predefined set of colors (see profiler_colors.h). If you want to use your own colors palette you can turn this option OFF")
set(EASY_OPTION_LIB_STATIC             OFF    CACHE BOOL   "Build easy_profiler as static library.")
if (WIN32)
    set(EASY_OPTION_EVENT_TRACING      ON     CACHE BOOL   "Enable event tracing by default")
    set(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING ON CACHE BOOL "Set low priority for event tracing thread")
endif (WIN32)
set(BUILD_WITH_CHRONO_STEADY_CLOCK     OFF    CACHE BOOL   "Use std::chrono::steady_clock as a timer" )
set(BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK OFF CACHE BOOL "Use std::chrono::high_resolution_clock as a timer")

message(STATUS "-------- EASY_PROFILER OPTIONS: --------")
if (BUILD_WITH_CHRONO_STEADY_CLOCK)
    message(STATUS "  Use std::chrono::steady_clock as a timer")
elseif (BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK)
    message(STATUS "  Use std::chrono::high_resolution_clock as a timer")
else ()
    if (WIN32)
        message(STATUS "  Use QueryPerformanceCounter as a timer")
    else ()
        message(STATUS "  Use rtdsc as a timer")
    endif ()
endif ()

message(STATUS "  Default listening port = ${EASY_DEFAULT_PORT}")
message(STATUS "  Auto-start listening = ${EASY_OPTION_LISTEN}")
message(STATUS "  Profile self = ${EASY_OPTION_PROFILE_SELF}")
message(STATUS "  Profile self blocks initial status = ${EASY_OPTION_PROFILE_SELF_BLOCKS_ON}")
if (WIN32)
    message(STATUS "  Event tracing = ${EASY_OPTION_EVENT_TRACING}")
    if (EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
        message(STATUS "  Event tracing has low priority = Yes")
    else ()
        message(STATUS "  Event tracing has low priority = No")
    endif (EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
endif (WIN32)
message(STATUS "  Log messages = ${EASY_OPTION_LOG}")
message(STATUS "  Use EasyProfiler colors palette = ${EASY_OPTION_PREDEFINED_COLORS}")
if (EASY_OPTION_LIB_STATIC)
    set(EASY_OPTION_LIB_TYPE STATIC)
else ()
    set(EASY_OPTION_LIB_TYPE SHARED)
endif ()
message(STATUS "  Library type: ${EASY_OPTION_LIB_TYPE}")
message(STATUS "------ END EASY_PROFILER OPTIONS -------")
message(STATUS "")

set(CPP_FILES
    block.cpp
    profile_manager.cpp
    reader.cpp
    event_trace_win.cpp
    easy_socket.cpp
)

set(H_FILES
    profile_manager.h
    spin_lock.h
    event_trace_win.h
    current_time.h
)

set(INCLUDE_FILES
    include/easy/profiler.h
    include/easy/reader.h
    include/easy/easy_net.h
    include/easy/easy_socket.h
    include/easy/easy_compiler_support.h
    include/easy/profiler_aux.h
    include/easy/profiler_colors.h
    include/easy/reader.h
    include/easy/serialized_block.h
)

source_group(include FILES ${INCLUDE_FILES})

set(SOURCES
    ${CPP_FILES}
    ${H_FILES}
    ${INCLUDE_FILES}
)

add_library(easy_profiler ${EASY_OPTION_LIB_TYPE} ${SOURCES} resources.rc)
target_include_directories(easy_profiler PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_compile_definitions(easy_profiler PRIVATE
    -D_BUILD_PROFILER=1
    #-DEASY_PROFILER_API_DISABLED # uncomment this to disable profiler api only (you will have to rebuild only easy_profiler)
)
if (EASY_OPTION_LIB_STATIC)
    target_compile_definitions(easy_profiler PUBLIC -DEASY_PROFILER_STATIC=1)
endif ()
target_compile_definitions(easy_profiler PUBLIC
    -DEASY_PROFILER_VERSION_MAJOR=${EASY_PROGRAM_VERSION_MAJOR}
    -DEASY_PROFILER_VERSION_MINOR=${EASY_PROGRAM_VERSION_MINOR}
    -DEASY_PROFILER_VERSION_PATCH=${EASY_PROGRAM_VERSION_PATCH}
    -DEASY_DEFAULT_PORT=${EASY_DEFAULT_PORT}
    -DBUILD_WITH_EASY_PROFILER=1
)

easy_define_target_option(easy_profiler BUILD_WITH_CHRONO_STEADY_CLOCK EASY_CHRONO_STEADY_CLOCK)
easy_define_target_option(easy_profiler BUILD_WITH_CHRONO_HIGH_RESOLUTION_CLOCK EASY_CHRONO_HIGHRES_CLOCK)
easy_define_target_option(easy_profiler EASY_OPTION_LISTEN EASY_OPTION_START_LISTEN_ON_STARTUP)
easy_define_target_option(easy_profiler EASY_OPTION_PROFILE_SELF EASY_OPTION_MEASURE_STORAGE_EXPAND)
easy_define_target_option(easy_profiler EASY_OPTION_PROFILE_SELF_BLOCKS_ON EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON)
if (WIN32)
    easy_define_target_option(easy_profiler EASY_OPTION_EVENT_TRACING EASY_OPTION_EVENT_TRACING_ENABLED)
    easy_define_target_option(easy_profiler EASY_OPTION_LOW_PRIORITY_EVENT_TRACING EASY_OPTION_LOW_PRIORITY_EVENT_TRACING)
endif ()
easy_define_target_option(easy_profiler EASY_OPTION_LOG EASY_OPTION_LOG_ENABLED)
easy_define_target_option(easy_profiler EASY_OPTION_PREDEFINED_COLORS EASY_OPTION_BUILTIN_COLORS)

if (UNIX)
    target_compile_options(easy_profiler PRIVATE -Wall -Wno-long-long -Wno-reorder -Wno-braced-scalar-init -pedantic -O3)
    # ATOMIC BEGIN
    if (NOT ANDROID)
        target_link_libraries(easy_profiler pthread)
    endif ()
    # ATOMIC END
elseif (WIN32)
    target_compile_definitions(easy_profiler PRIVATE -D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
    target_link_libraries(easy_profiler ws2_32 psapi)
endif ()

if (MINGW)
    target_compile_definitions(easy_profiler PRIVATE -DSTRSAFE_NO_DEPRECATE)
endif ()

if (APPLE)
    target_compile_options(easy_profiler PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11>)
else ()
    

    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (NOT MSVC)
            target_compile_options(easy_profiler PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11>)
        endif ()
    else()
        if (NOT MSVC)
            target_compile_options(easy_profiler PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++11>)
        endif ()
        set_target_properties(easy_profiler PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
    endif ()
endif ()

# ATOMIC BEGIN
if (0)
####
# Installation 
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")

include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)

write_basic_package_version_file(
    "${version_config}"
    VERSION
    ${EASY_PRODUCT_VERSION_STRING}
    COMPATIBILITY
    SameMajorVersion
)

configure_package_config_file(
    "cmake/config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    FILES
    ${INCLUDE_FILES}
    DESTINATION
    include/easy
)

install(
    FILES
    LICENSE.MIT
    LICENSE.APACHE
    DESTINATION
    .
)

install(
    TARGETS
    easy_profiler
    EXPORT
    ${targets_export_name}
    DESTINATION
    bin
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    EXPORT "${targets_export_name}"
    DESTINATION "${config_install_dir}"
)
endif ()

# If we're on MSVC, we need a /MD version of the easy profiler library to link with Qt
if (MSVC AND Qt5Widgets_FOUND)
    add_library(easy_profiler_md ${EASY_OPTION_LIB_TYPE} ${SOURCES} resources.rc)
    foreach(prop COMPILE_DEFINITIONS COMPILE_OPTIONS CXX_STANDARD CXX_STANDARD_REQUIRED INCLUDE_DIRECTORIES INTERFACE_COMPILE_DEFINITIONS INTERFACE_COMPILE_OPTIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_LINK_LIBRARIES)
        get_target_property(val easy_profiler ${prop})
        set_target_properties(easy_profiler_md PROPERTIES ${prop} "${val}")
    endforeach()
    target_compile_options(easy_profiler_md PUBLIC $<$<CONFIG:Debug>:/MDd> $<$<NOT:$<CONFIG:Debug>>:/MD>)
endif ()
# ATOMIC END
