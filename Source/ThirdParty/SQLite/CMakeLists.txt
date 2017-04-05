#
# Copyright (c) 2008-2016 the Urho3D project.
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

# Define target name
set (TARGET_NAME sqlite)

# Define source files
set (SOURCE_FILES src/sqlite3.c)

# ATOMIC BEGIN
set (INCLUDE_DIRS src)
# ATOMIC END

# Setup target
setup_library ()

# ATOMIC BEGIN
# Define preprocessor macros
target_compile_definitions (${TARGET_NAME} PUBLIC -DSQLITE_USE_URI=1 -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_SOUNDEX)   # https://www.sqlite.org/compile.html
if (WEB)
    # Do not use pthread and dl libraries for Web platform
    target_compile_definitions (${TARGET_NAME} PUBLIC -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION)
elseif (MINGW)
    # We only support MinGW with "_mingw.h" header file, note the leading underscore
    target_compile_definitions (${TARGET_NAME} PRIVATE -D_HAVE__MINGW_H)
endif ()
foreach (VAR HAVE_STDINT_H HAVE_INTTYPES_H HAVE_MALLOC_H HAVE_MALLOC_USABLE_SIZE)
    if (${VAR})
        target_compile_definitions (${TARGET_NAME} PRIVATE -D${VAR})
    endif ()
endforeach ()
# ATOMIC END

# Setup additional SQLite CLI standalone target (this target can be transfered and executed on an embedded device, such as Raspberry Pi and Android)
if (NOT IOS AND NOT WEB AND NOT ANDROID)    # ATOMIC FIX
    # Define target name for SQLite shell
    set (TARGET_NAME isql)

    # Define source files
    set (SOURCE_FILES src/shell.c src/sqlite3.c src/sqlite3.h)

    # Define dependency libs
    if (NOT WIN32)
        set (LIBS dl pthread)    # ATOMIC FIX
        if (READLINE_FOUND)
            add_definitions (-DHAVE_READLINE)
            list (APPEND INCLUDE_DIRS ${READLINE_INCLUDE_DIRS})
            list (APPEND LIBS ${READLINE_LIBRARIES})
        endif ()
    endif ()

    # Setup target
    setup_executable (NODEPS)
endif ()

# SQLite will not work correctly with the -ffast-math option of GCC
# ATOMIC: Add check for empty CMAKE_C_FLAGS
if (CMAKE_C_FLAGS)
    string (REPLACE -ffast-math "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
endif()
