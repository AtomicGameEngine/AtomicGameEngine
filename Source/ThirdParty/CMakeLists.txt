#
# Portions Copyright (c) 2017 the Atomic project.
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

# When porting build scripts of third party libs please be aware:
# * Atomic removed all instances of `install_header_files()` macro calls. Do not bring them back.
# * Atomic added extra `set (INCLUDE_DIRS ...)`. Do not remove them.
# * Atomic added extra `target_link_libraries(...)`. Do not remove them.
# * Atomic added extra `target_compile_definitions(...)`. Do not remove them.
# * Atomic added extra `target_include_directories(...)`. Do not remove them.
# * If file contains a license with copyright attributed to Atomic project - this file is written and maintained by
#   Atomic team and should not be synced with upstream.
# Changes should be marked with "ATOMIC BEGIN|END" or "ATOMIC FIX" comments.

add_subdirectory(Box2D)
add_subdirectory(Duktape)
add_subdirectory(FreeType)
add_subdirectory(JO)
add_subdirectory(LZ4)
add_subdirectory(PugiXml)
add_subdirectory(STB)
add_subdirectory(TurboBadger)
add_subdirectory(zlib)

if (NOT ATOMIC_BUILD_2D)
    add_subdirectory(Bullet)
    add_subdirectory(StanHull)
endif()

add_subdirectory(SDL)

add_subdirectory(Recast)
add_subdirectory(Detour)
add_subdirectory(DetourCrowd)
add_subdirectory(DetourTileCache)

if (NOT WEB)
    add_subdirectory(Civetweb)
    add_subdirectory(kNet)
    if (NOT WIN32)
        add_subdirectory(openssl)
    endif()
    set (CURL_STATICLIB 1)
    add_subdirectory(libcurl)
endif()

if (NOT IOS AND NOT ANDROID AND NOT WEB)
    add_subdirectory(Assimp)
    if (NOT LINUX)
        add_subdirectory(LibCpuId)
    endif()
    add_subdirectory(Poco)
    add_subdirectory(nativefiledialog)
    add_subdirectory(libsquish)
endif ()

if (ATOMIC_DATABASE_SQLITE)
    add_subdirectory(SQLite)
endif ()
if (ATOMIC_DATABASE_ODBC)
    add_subdirectory(nanodbc)
endif ()

if (LINUX OR APPLE OR ATOMIC_OPENGL AND NOT IOS)
    add_subdirectory(GLEW)
endif()

if (WIN32)
    add_subdirectory(MojoShader)
endif()

add_subdirectory(ASIO)
add_subdirectory(rapidjson)
add_subdirectory(WebSocketPP)
