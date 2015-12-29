# Spicific to Atomic Game Engine
# Assimp includes zlib

set(ZLIB_FOUND 1 CACHE INTERNAL "ZLib Found")
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" CACHE INTERNAL "ZLib include")
set(ZLIB_LIBRARIES zlib CACHE INTERNAL "ZLib target")

mark_as_advanced(ZLIB_FOUND ZLIB_INCLUDE_DIR ZLIB_LIBRARIES)
