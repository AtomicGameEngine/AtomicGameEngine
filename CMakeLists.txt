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

# Set project name
project (Atomic)

# Set minimum version
cmake_minimum_required (VERSION 2.8.6)

if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Atomic Cmake common module
include (Atomic-CMake-common)

# Setup SDK install destinations
if (WIN32)
    set (SCRIPT_EXT .bat)
else ()
    set (SCRIPT_EXT .sh)
endif ()
set (PATH_SUFFIX Atomic)
if (ANDROID)
    # For Android platform, install to a path similar to ANDROID_LIBRARY_OUTPUT_PATH variable, e.g. libs/armeabi-v7a
    set (LIB_SUFFIX s/${ANDROID_NDK_ABI_NAME})
elseif (ATOMIC_64BIT)
    if (EXISTS /usr/lib64)
        set (HAS_LIB64 TRUE)
    endif ()
    # Install to 'lib64' when one of these conditions is true
    if (WIN32 OR ATOMIC_USE_LIB64_RPM OR (HAS_LIB64 AND NOT ATOMIC_USE_LIB_DEB))
        set (LIB_SUFFIX 64)
    endif ()
endif ()
set (DEST_INCLUDE_DIR include/${PATH_SUFFIX})
set (DEST_SHARE_DIR share/${PATH_SUFFIX})
set (DEST_RUNTIME_DIR bin)
set (DEST_BUNDLE_DIR ${DEST_SHARE_DIR}/Applications)
# Note to package maintainer: ${PATH_SUFFIX} for library could be removed if the extra path is not desired, but if so then the RPATH setting in Source's CMakeLists.txt needs to be adjusted accordingly
set (DEST_LIBRARY_DIR lib${LIB_SUFFIX}/${PATH_SUFFIX})
# TODO: Investigate the implication of using CMAKE_SYSROOT variable (available since 3.0.0) instead of our own SYSROOT variable
set (SYSROOT ${ANDROID_TOOLCHAIN_ROOT} ${RPI_SYSROOT} ${MINGW_SYSROOT} ${IOS_SYSROOT} CACHE INTERNAL "Path to system root of the cross-compiling target")  # SYSROOT is empty for native build
set (DEST_PKGCONFIG_DIR lib${LIB_SUFFIX}/pkgconfig)
set (DEST_PERMISSIONS FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
# Install application launcher scripts
file (GLOB APP_SCRIPTS ${CMAKE_SOURCE_DIR}/bin/*${SCRIPT_EXT})
install (FILES ${APP_SCRIPTS} DESTINATION ${DEST_RUNTIME_DIR} PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)   # 755
# Install resource directories required by applications built with Atomic library
install (DIRECTORY ${CMAKE_SOURCE_DIR}/bin/CoreData ${CMAKE_SOURCE_DIR}/bin/Data DESTINATION ${DEST_SHARE_DIR}/Resources ${DEST_PERMISSIONS})
# Install CMake modules and toolchains provided by and for Atomic
install (DIRECTORY ${CMAKE_SOURCE_DIR}/CMake/ DESTINATION ${DEST_SHARE_DIR}/CMake ${DEST_PERMISSIONS})    # Note: the trailing slash is significant
# Install CMake build scripts
file (GLOB CMAKE_SCRIPTS ${CMAKE_SOURCE_DIR}/*${SCRIPT_EXT})
install (FILES ${CMAKE_SCRIPTS} DESTINATION ${DEST_SHARE_DIR}/Scripts PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Setup package variables
set (ATOMIC_DESCRIPTION "Atomic is a free lightweight, cross-platform 2D and 3D game engine implemented in C++ and released under the MIT license. Greatly inspired by OGRE (http://www.ogre3d.org) and Horde3D (http://www.horde3d.org).")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${ATOMIC_DESCRIPTION})
set (ATOMIC_URL "https://github.com/urho3d/Atomic")
set (CPACK_PACKAGE_VENDOR ${ATOMIC_URL})
set (CPACK_PACKAGE_CONTACT ${ATOMIC_URL})
execute_process (COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/CMake/Modules/GetAtomicRevision.cmake WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE ATOMIC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
set (CPACK_PACKAGE_VERSION ${ATOMIC_VERSION})
string (REGEX MATCH "([^.]+)\\.([^.]+)\\.(.+)" MATCHED ${ATOMIC_VERSION})
if (MATCHED)
    set (CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
    set (CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})
    set (CPACK_PACKAGE_VERSION_PATCH ${CMAKE_MATCH_3})
endif ()
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/License.txt)
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set (CPACK_GENERATOR TGZ)
if (ANDROID)
    set (CPACK_SYSTEM_NAME Android)
elseif (IOS)
    set (CPACK_SYSTEM_NAME iOS)
elseif (APPLE)
    set (CPACK_SYSTEM_NAME OSX)
elseif (WIN32)
    set (CPACK_GENERATOR ZIP)
elseif (CPACK_SYSTEM_NAME STREQUAL Linux)
    if (RPI)
        set (CPACK_SYSTEM_NAME Raspberry-Pi)
    endif ()
    if (NOT ATOMIC_64BIT OR HAS_LIB64)
        list (APPEND CPACK_GENERATOR RPM)
    endif ()
    if (NOT ATOMIC_64BIT OR NOT HAS_LIB64)
        list (APPEND CPACK_GENERATOR DEB)
    endif ()
    if (ATOMIC_64BIT)
        if (ATOMIC_USE_LIB64_RPM AND NOT HAS_LIB64)
            set (CPACK_GENERATOR RPM)
        elseif (ATOMIC_USE_LIB_DEB AND HAS_LIB64)
            set (CPACK_GENERATOR DEB)
        endif ()
    endif ()
    # Note to package maintainer: comment out below line to revert the prefix from '/usr/local' (default for CMAKE_INSTALL_PREFIX) to '/usr' (default for CPACK_PACKAGING_INSTALL_PREFIX)
    set (CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif ()
if (ATOMIC_64BIT)
    set (CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}-64bit)
endif ()
set (CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}-${ATOMIC_LIB_TYPE})
if (WIN32 AND NOT ATOMIC_OPENGL)
    set (CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}-D3D)    # Stands for Direct 3D
elseif (ANDROID AND X86)    # Take advantage of Android toolchain setting X86 variable to true for both 'x86' and 'x86_64' ABIs
    set (CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}-IA)     # Stands for Intel Architecture
endif ()
if (NOT DEFINED ENV{RELEASE_TAG})
    set (CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}-snapshot)
endif ()
include (CPack)

# Setup MacOSX and iOS bundle variables
if (IOS OR ATOMIC_MACOSX_BUNDLE)
    if (NOT MACOSX_BUNDLE_ICON_FILE)
        set (MACOSX_BUNDLE_ICON_FILE UrhoIcon)
    endif ()
    if (NOT MACOSX_BUNDLE_BUNDLE_VERSION)
        set (MACOSX_BUNDLE_BUNDLE_VERSION ${ATOMIC_VERSION})
    endif ()
    if (NOT MACOSX_BUNDLE_LONG_VERSION_STRING)
        set (MACOSX_BUNDLE_LONG_VERSION_STRING ${ATOMIC_VERSION})
    endif ()
    if (NOT MACOSX_BUNDLE_SHORT_VERSION_STRING)
        set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${ATOMIC_VERSION})
    endif ()
    if (NOT MACOSX_BUNDLE_COPYRIGHT)
        set (MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2008-2014 the Urho3D project.")
    endif ()
endif ()

# Setup SDK-like include dir in the build tree for building the Atomic library
file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DEST_INCLUDE_DIR}/ThirdParty)

# Atomic source
add_subdirectory (Source)