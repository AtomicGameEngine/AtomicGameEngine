#
# Listing and grouping of all the source files.
# 1) Set the file lists for each component
# 2) Create a Source Group for each component, for IDE project orginization
# 3) Add libassimp using the file lists (eliminates duplication of file names between
#    source groups and library command)
#

# Modified by Lasse Oorni and Yao Wei Tjong for Urho3D

project(Assimp)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

if (MINGW)
    # The IFCReaderGen.cpp.obj has too many sections in DEBUG configuration build
    # Since GCC does not support /bigobj compiler flags as in MSVC, we use optimization flags to reduce the object file size
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O1")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
endif ()

INCLUDE_DIRECTORIES( code )
INCLUDE_DIRECTORIES( include )
INCLUDE_DIRECTORIES( code/BoostWorkaround )
INCLUDE_DIRECTORIES( contrib/unzip )
ADD_DEFINITIONS( -DASSIMP_BUILD_BOOST_WORKAROUND -DASSIMP_BUILD_NO_C4D_IMPORTER -DASSIMP_BUILD_NO_OPENGEX_IMPORTER -DASSIMP_BUILD_NO_ASSBIN_IMPORTER)

file (GLOB_RECURSE CODE_FILES code/*.cpp code/*.h)

SET( IrrXML_SRCS
	code/irrXMLWrapper.h
	contrib/irrXML/CXMLReaderImpl.h
	contrib/irrXML/heapsort.h
	contrib/irrXML/irrArray.h
	contrib/irrXML/irrString.h
	contrib/irrXML/irrTypes.h
	contrib/irrXML/irrXML.cpp
	contrib/irrXML/irrXML.h
)
SOURCE_GROUP( IrrXML FILES ${IrrXML_SRCS})

SET( ConvertUTF_SRCS
	contrib/ConvertUTF/ConvertUTF.h
	contrib/ConvertUTF/ConvertUTF.c
)
SOURCE_GROUP( ConvertUTF FILES ${ConvertUTF_SRCS})

SET( Clipper_SRCS
	contrib/clipper/clipper.hpp
	contrib/clipper/clipper.cpp
)
SOURCE_GROUP( Clipper FILES ${Clipper_SRCS})

SET( Poly2Tri_SRCS
	contrib/poly2tri/poly2tri/common/shapes.cc
	contrib/poly2tri/poly2tri/common/shapes.h
	contrib/poly2tri/poly2tri/common/utils.h
	contrib/poly2tri/poly2tri/sweep/advancing_front.h
	contrib/poly2tri/poly2tri/sweep/advancing_front.cc
	contrib/poly2tri/poly2tri/sweep/cdt.cc
	contrib/poly2tri/poly2tri/sweep/cdt.h
	contrib/poly2tri/poly2tri/sweep/sweep.cc
	contrib/poly2tri/poly2tri/sweep/sweep.h
	contrib/poly2tri/poly2tri/sweep/sweep_context.cc
	contrib/poly2tri/poly2tri/sweep/sweep_context.h
)
SOURCE_GROUP( Poly2Tri FILES ${Poly2Tri_SRCS})

SET( unzip_SRCS
	contrib/unzip/crypt.h
	contrib/unzip/ioapi.c
	contrib/unzip/ioapi.h
	contrib/unzip/unzip.c
	contrib/unzip/unzip.h
)
SOURCE_GROUP( unzip FILES ${unzip_SRCS})

SET( zlib_SRCS
	contrib/zlib/adler32.c
	contrib/zlib/compress.c
	contrib/zlib/crc32.c
	contrib/zlib/crc32.h
	contrib/zlib/deflate.c
	contrib/zlib/deflate.h
#	contrib/zlib/gzclose.c
#	contrib/zlib/gzguts.h
#	contrib/zlib/gzlib.c
#	contrib/zlib/gzread.c
#	contrib/zlib/gzwrite.c
	contrib/zlib/infback.c
	contrib/zlib/inffast.c
	contrib/zlib/inffast.h
	contrib/zlib/inffixed.h
	contrib/zlib/inflate.c
	contrib/zlib/inflate.h
	contrib/zlib/inftrees.c
	contrib/zlib/inftrees.h
	contrib/zlib/trees.c
	contrib/zlib/trees.h
	contrib/zlib/zconf.h
	contrib/zlib/zlib.h
	contrib/zlib/zutil.c
	contrib/zlib/zutil.h
)
SOURCE_GROUP( zlib FILES ${zlib_SRCS})

if ( MSVC80 OR MSVC90 OR MSVC10 )
	ADD_DEFINITIONS( -D_SCL_SECURE_NO_WARNINGS )
	ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )
endif ( MSVC80 OR MSVC90 OR MSVC10 )

SET( SOURCE_FILES
	# Assimp Files
        ${CODE_FILES}

)

SET( CONTRIB_FILES
	# Third-party libraries
	${IrrXML_SRCS}
	${ConvertUTF_SRCS}
	${unzip_SRCS}
#	${zlib_SRCS}
	${Poly2Tri_SRCS}
	${Clipper_SRCS}
	# Necessary to show the headers in the project when using the VC++ generator:
	${Boost_SRCS}
)

find_package(ZLIB)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
else ()
    message(STATUS "Using contrib version of zlib.")
    list(APPEND CONTRIB_FILES ${zlib_SRCS})
endif ()


set (TARGET_NAME Assimp)
list (APPEND SOURCE_FILES ${CONTRIB_FILES} ${PUBLIC_HEADERS} ${COMPILER_HEADERS})

#set_source_files_properties (contrib/clipper/clipper.cpp contrib/irrXML/irrXML.cpp PROPERTIES NO_PCH TRUE)
#enable_pch (code/AssimpPCH.h)

add_library(Assimp ${SOURCE_FILES})
target_link_libraries(Assimp zlib)
