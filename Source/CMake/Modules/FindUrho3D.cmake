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

# Find Atomic library and include directories in the project root tree or installed location
# For project root tree detection to work, Atomic library must be already been built
#
#  ATOMIC_FOUND
#  ATOMIC_INCLUDE_DIRS
#  ATOMIC_LIBRARIES
#  ATOMIC_LIBRARIES_REL
#  ATOMIC_LIBRARIES_DBG
#

if (ATOMIC_FOUND)
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

set (ATOMIC_LIB_NAMES Atomic)
if (WIN32)
    set (ATOMIC_LIB_NAMES_DBG Urho3D_d)
endif ()

if (CMAKE_PROJECT_NAME STREQUAL Atomic AND PROJECT_ROOT_DIR)
    set (ATOMIC_HOME ${PROJECT_ROOT_DIR} CACHE PATH "Path to Atomic project root tree" FORCE)
    set (IS_INTERNAL 1)
elseif (NOT ATOMIC_HOME AND DEFINED ENV{ATOMIC_HOME})
    file (TO_CMAKE_PATH "$ENV{ATOMIC_HOME}" ATOMIC_HOME)
    set (ATOMIC_HOME ${ATOMIC_HOME} CACHE PATH "Path to Atomic project root tree")
endif ()
if (ATOMIC_HOME)
    # Construct source tree paths from ATOMIC_HOME environment variable
    find_file (ATOMIC_SOURCE_TREE Atomic.h.in ${ATOMIC_HOME}/Source/Engine DOC "Path to Atomic project source tree" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    if (ATOMIC_SOURCE_TREE)
        get_filename_component (ATOMIC_SOURCE_TREE ${ATOMIC_SOURCE_TREE} PATH)
        set (ATOMIC_INCLUDE_DIRS ${ATOMIC_SOURCE_TREE})
        foreach (DIR Audio Container Core Engine Graphics Input IO LuaScript Math Navigation Network Physics Resource Scene Script UI Atomic2D Javascript Environment)
            list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_SOURCE_TREE}/${DIR})     # Note: variable change to list context after this
        endforeach ()
        set (DIRS SDL/include)
        if (ATOMIC_ANGELSCRIPT)
            list (APPEND DIRS AngelScript/include)
        endif ()
        if (ATOMIC_NETWORK)
            list (APPEND DIRS kNet/include)
        endif ()
        if (ATOMIC_PHYSICS)
            list (APPEND DIRS Bullet/src)
        endif ()
        if (ATOMIC_ATOMIC2D)
            list (APPEND DIRS Box2D)
        endif ()
        if (ATOMIC_JAVASCRIPT)
            list (APPEND DIRS Duktape)
        endif ()        
        if (ATOMIC_TBUI)
            list (APPEND DIRS TurboBadger)
        endif ()                                
        foreach (DIR ${DIRS})
            list (APPEND ATOMIC_INCLUDE_DIRS ${ATOMIC_HOME}/Source/ThirdParty/${DIR})
        endforeach ()

        # For non Atomic project using Atomic as external library, Atomic project itself must be built using predefined build directory as per specified in the provided build scripts.
        if (IS_INTERNAL)
            set (BINARY_DIR ${CMAKE_BINARY_DIR})
        elseif (ANDROID AND CMAKE_HOST_WIN32 AND NOT ATOMIC_MKLINK)
            set (BINARY_DIR ${ATOMIC_HOME}/Source/Android)
        else ()
            set (BINARY_DIR ${ATOMIC_HOME}/${PLATFORM_PREFIX}Build)
        endif () 
        list (APPEND ATOMIC_INCLUDE_DIRS ${BINARY_DIR}/Engine)
        if (ANDROID)
            if (IS_INTERNAL)
                set (ATOMIC_LIB_SEARCH_PATH ${ANDROID_LIBRARY_OUTPUT_PATH})
            else ()
                set (ATOMIC_LIB_SEARCH_PATH ${BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME})
            endif ()
        else ()
            set (ATOMIC_LIB_SEARCH_PATH ${ATOMIC_HOME}/${PLATFORM_PREFIX}Lib)
        endif ()
        if (TARGET Atomic)
            set (ATOMIC_LIBRARIES Atomic)
            set (FOUND_MESSAGE "Found Atomic: as CMake target")
        else ()
            find_library (ATOMIC_LIBRARIES NAMES ${ATOMIC_LIB_NAMES} PATHS ${ATOMIC_LIB_SEARCH_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
            if (WIN32)
                find_library (ATOMIC_LIBRARIES_DBG NAMES ${ATOMIC_LIB_NAMES_DBG} PATHS ${ATOMIC_LIB_SEARCH_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
            endif ()
        endif ()
    endif ()
else ()
    # If Atomic SDK is not being installed in the default system location, use the CMAKE_PREFIX_PATH environment variable to specify the prefix path to that location
    # Note that the prefix path should not contain the "/include" or "/lib"
    # For example on Windows platform: CMAKE_PREFIX_PATH=C:/Users/john/Atomic if the SDK is installed using CMAKE_INSTALL_PREFIX=C:/Users/john/Atomic
    # For example on Linux platform: CMAKE_PREFIX_PATH=/home/john/usr/local if the SDK is installed using DESTDIR=/home/john and CMAKE_INSTALL_PREFIX=/usr/local
    if (CMAKE_HOST_WIN32)
        set (ATOMIC_INC_SEARCH_PATH include)
        set (ATOMIC_LIB_SEARCH_PATH lib)
    else ()
        set (PATH_SUFFIX Atomic)
        if (IOS)
            set (CMAKE_LIBRARY_ARCHITECTURE ios)
        endif ()
    endif ()
    # Additional search paths are added by CMake by default which should already include most common platform specific paths
    find_path (ATOMIC_INCLUDE_DIRS Atomic.h PATHS ${ATOMIC_INC_SEARCH_PATH} PATH_SUFFIXES ${PATH_SUFFIX})
    find_library (ATOMIC_LIBRARIES NAMES ${ATOMIC_LIB_NAMES} PATHS ${ATOMIC_LIB_SEARCH_PATH} PATH_SUFFIXES ${PATH_SUFFIX})
    if (WIN32)
        find_library (ATOMIC_LIBRARIES_DBG NAMES ${ATOMIC_LIB_NAMES_DBG} PATHS ${ATOMIC_LIB_SEARCH_PATH} PATH_SUFFIXES ${PATH_SUFFIX})
    endif ()

    if (ATOMIC_INCLUDE_DIRS)
        set (BASE_DIR ${ATOMIC_INCLUDE_DIRS})
        set (DIRS SDL)
        if (ATOMIC_ANGELSCRIPT)
            list (APPEND DIRS AngelScript)
        endif ()
        if (ATOMIC_NETWORK)
            list (APPEND DIRS kNet)
        endif ()
        if (ATOMIC_PHYSICS)
            list (APPEND DIRS Bullet)
        endif ()
        if (ATOMIC_ATOMIC2D)
            list (APPEND DIRS Box2D)
        endif ()
        if (ATOMIC_JAVASCRIPT)
            list (APPEND DIRS Duktape)
        endif ()                
        if (ATOMIC_TBUI)
            list (APPEND DIRS TurboBadger)
        endif ()                        
        foreach (DIR ${DIRS})
            list (APPEND ATOMIC_INCLUDE_DIRS ${BASE_DIR}/${DIR})     # Note: variable change to list context after this, so we need BASE_DIR to remain the same
        endforeach ()
    endif ()
endif ()

if (WIN32)
    set (ATOMIC_LIBRARIES_REL ${ATOMIC_LIBRARIES})
    if (ATOMIC_LIBRARIES)
        if (ATOMIC_LIBRARIES_DBG)
            list (APPEND ATOMIC_LIBRARIES ${ATOMIC_LIBRARIES_DBG})
        endif ()
    else ()
        set (ATOMIC_LIBRARIES ${ATOMIC_LIBRARIES_DBG})
    endif ()
endif ()

if (ATOMIC_INCLUDE_DIRS AND ATOMIC_LIBRARIES)
    set (ATOMIC_FOUND 1)
    if (NOT FOUND_MESSAGE)
        set (FOUND_MESSAGE "Found Atomic: ${ATOMIC_LIBRARIES}")
    endif ()
endif ()

if (ATOMIC_FOUND)
    include (FindPackageMessage)
    FIND_PACKAGE_MESSAGE (Atomic ${FOUND_MESSAGE} "[${ATOMIC_LIBRARIES}][${ATOMIC_INCLUDE_DIRS}]")
else ()
    if (Urho3D_FIND_REQUIRED)
        message (FATAL_ERROR
            "Could not find Atomic library in default SDK installation location or Atomic project root tree. "
            "For searching in a non-default Atomic SDK installation, use 'CMAKE_PREFIX_PATH' environment variable to specify the prefix path of the installation location. "
            "For searching in a build tree of Atomic project, use 'ATOMIC_HOME' environment variable to specify the Urho3D project root directory. The Atomic library itself must already be built successfully.")
    endif ()
endif ()

mark_as_advanced (ATOMIC_INCLUDE_DIRS ATOMIC_LIBRARIES ATOMIC_SOURCE_TREE ATOMIC_HOME)
