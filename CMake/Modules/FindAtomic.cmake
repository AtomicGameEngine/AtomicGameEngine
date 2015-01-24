#
# Copyright (c) 2008-2014 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Find Atomic include directories and library in source & build tree or installed Atomic SDK.
#
#  ATOMIC_FOUND
#  ATOMIC_INCLUDE_DIRS
#  ATOMIC_LIBRARIES
#  ATOMIC_LIBRARIES_REL (WIN32 only)
#  ATOMIC_LIBRARIES_DBG (WIN32 Only)
#
#
# For internal Atomic project, the Atomic "build tree" path is already known.
#
# For external project that attempts to use the Atomic build tree or installed Atomic SDK,
# use ATOMIC_HOME environment variable or build option to specify the path (not needed when the path is a system-wide default location).
# When setting ATOMIC_HOME variable, it should be set to a parent directory containing both the "include" or "lib" subdirectories.
# For example: set ATOMIC_HOME=/home/john/usr/local, if the SDK is installed using DESTDIR=/home/john and CMAKE_INSTALL_PREFIX=/usr/local
#

if (ATOMIC_FOUND)
    # All the subprojects should use the same Atomic library, so only need to search on the first (sub)project that requires Atomic library
    return ()
endif ()

# If the ATOMIC_LIB_TYPE build option changes then invalidate the found library cache
if (NOT ATOMIC_LIB_TYPE STREQUAL ATOMIC_FOUND_LIB_TYPE)
    unset (ATOMIC_LIBRARIES CACHE)
    set (ATOMIC_FOUND_LIB_TYPE ${ATOMIC_LIB_TYPE} CACHE INTERNAL "Lib type when Atomic library was last found")

    # Atomic prefers static library type by default while CMake prefers shared one, so we need to change CMake preference to agree with Atomic
    if (NOT ATOMIC_LIB_TYPE STREQUAL SHARED)
        list (REVERSE CMAKE_FIND_LIBRARY_SUFFIXES)
    endif ()
endif ()

set (PATH_SUFFIX Atomic)
if (CMAKE_PROJECT_NAME STREQUAL Atomic AND TARGET Atomic)
    # Library location is already known to be in the build tree
    set (ATOMIC_HOME ${CMAKE_BINARY_DIR})
    set (ATOMIC_INCLUDE_DIRS ${ATOMIC_HOME}/include ${ATOMIC_HOME}/include/${PATH_SUFFIX}/ThirdParty)
    if (ATOMIC_PHYSICS)
        # Bullet library depends on its own include dir to be added in the header search path
        # This is more practical than patching its header files in many places to make them work with relative path
        list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_HOME}/include/${PATH_SUFFIX}/ThirdParty/Bullet)
    endif ()
    if (ATOMIC_LUA)
        # ditto for Lua/LuaJIT
        list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_HOME}/include/${PATH_SUFFIX}/ThirdParty/Lua${JIT})
    endif ()
    set (ATOMIC_LIBRARIES Atomic)
    set (FOUND_MESSAGE "Found Atomic: as CMake target")
else ()
    # Library location would be searched (based on ATOMIC_HOME variable if provided and in system-wide default location)
    if (NOT ATOMIC_HOME AND DEFINED ENV{ATOMIC_HOME})
        file (TO_CMAKE_PATH "$ENV{ATOMIC_HOME}" ATOMIC_HOME)
    endif ()
    # ATOMIC_HOME variable should be an absolute path, so use a non-rooted search even when we are cross-compiling
    if (ATOMIC_HOME)
        list (APPEND CMAKE_PREFIX_PATH ${ATOMIC_HOME})
        set (SEARCH_OPT NO_CMAKE_FIND_ROOT_PATH)
    endif ()
    # For Android platform, search in path similar to ANDROID_LIBRARY_OUTPUT_PATH variable
    if (ANDROID)
        if (ATOMIC_HOME)
            set (ATOMIC_LIB_SEARCH_HINT HINTS ${ATOMIC_HOME}/libs/${ANDROID_NDK_ABI_NAME})
        else ()
            set (ATOMIC_LIB_SEARCH_HINT HINTS /usr/local/libs/${ANDROID_NDK_ABI_NAME})
        endif ()
    endif ()
    if (NOT ATOMIC_64BIT)
        # For 32-bit, force to search in 'lib' path even when the host system defaulted to use 'lib64'
        set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS FALSE)
    elseif (WIN32)
        # For 64-bit, force to search in 'lib64' path even when the Windows platform is not defaulted to use it
        set_property (GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)
    endif ()
    find_path (ATOMIC_INCLUDE_DIRS Atomic.h PATH_SUFFIXES ${PATH_SUFFIX} ${SEARCH_OPT})
    find_library (ATOMIC_LIBRARIES NAMES Atomic ${ATOMIC_LIB_SEARCH_HINT} PATH_SUFFIXES ${PATH_SUFFIX} ${SEARCH_OPT})
    if (WIN32)
        # For Windows platform, give a second chance to search for a debug version of the library
        find_library (ATOMIC_LIBRARIES_DBG NAMES Urho3D_d ${ATOMIC_LIB_SEARCH_HINT} PATH_SUFFIXES ${PATH_SUFFIX} ${SEARCH_OPT})
        # If both the non-debug and debug version of the libraries are found then use them both
        set (ATOMIC_LIBRARIES_REL ${ATOMIC_LIBRARIES})
        # Otherwise, ATOMIC_LIBRARIES variable should have the path to either one of the version
        if (ATOMIC_LIBRARIES)
            if (ATOMIC_LIBRARIES_DBG)
                list (APPEND ATOMIC_LIBRARIES ${ATOMIC_LIBRARIES_DBG})
            endif ()
        else ()
            set (ATOMIC_LIBRARIES ${ATOMIC_LIBRARIES_DBG})
        endif ()
    endif ()
    if (ATOMIC_INCLUDE_DIRS)
        get_filename_component (PATH ${ATOMIC_INCLUDE_DIRS} PATH)
        set (ATOMIC_INCLUDE_DIRS ${PATH} ${ATOMIC_INCLUDE_DIRS}/ThirdParty)
        if (ATOMIC_PHYSICS)
            list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_INCLUDE_DIRS}/ThirdParty/Bullet)
        endif ()
        if (ATOMIC_LUA)
            list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_INCLUDE_DIRS}/ThirdParty/Lua${JIT})
        endif ()
        if (NOT ATOMIC_HOME)
            # ATOMIC_HOME is not set when using SDK installed on system-wide default location, so set it now
            get_filename_component (PATH ${PATH} PATH)
            set (ATOMIC_HOME ${PATH})
        endif ()
    endif ()
endif ()

if (ATOMIC_INCLUDE_DIRS AND ATOMIC_LIBRARIES)
    set (ATOMIC_FOUND 1)
    if (NOT FOUND_MESSAGE)
        set (FOUND_MESSAGE "Found Atomic: ${ATOMIC_LIBRARIES}")
    endif ()
    include (FindPackageMessage)
    FIND_PACKAGE_MESSAGE (Atomic ${FOUND_MESSAGE} "[${ATOMIC_LIBRARIES}][${ATOMIC_INCLUDE_DIRS}]")
    set (ATOMIC_HOME ${ATOMIC_HOME} CACHE PATH "Path to Atomic build tree or SDK installation location (external project only)" FORCE)
elseif (Urho3D_FIND_REQUIRED)
    if (ANDROID)
        set (NOT_FOUND_MESSAGE "For Android platform, double check if you have specified to use the same ANDROID_ABI as the Atomic library in the build tree or SDK.")
    endif ()
    message (FATAL_ERROR
        "Could not find Atomic library in Atomic build tree or SDK installation. "
        "Use ATOMIC_HOME environment variable or build option to specify the location of the build tree or SDK installation. ${NOT_FOUND_MESSAGE}")
endif ()

mark_as_advanced (ATOMIC_INCLUDE_DIRS ATOMIC_LIBRARIES ATOMIC_LIBRARIES_REL ATOMIC_LIBRARIES_DBG ATOMIC_HOME)
