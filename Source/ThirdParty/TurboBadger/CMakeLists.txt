
include_directories("${CMAKE_CURRENT_SOURCE_DIR}" ../FreeType/include ../SDL/include)

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -Wall -Wextra -Wno-unused-parameter")
  add_definitions(-DMACOSX)
endif(APPLE)

add_definitions(-DTB_FONT_RENDERER_FREETYPE)
#add_definitions(-DTB_FONT_RENDERER_STB)

if (ATOMIC_OPENGL)
    add_definitions(-DATOMIC_OPENGL=1)
elseif (ATOMIC_D3D11)
    add_definitions(-DATOMIC_D3D11=1)
endif ()

# Define source files
file (GLOB C_FILES *.cpp image/*.cpp animation/*.cpp parser/*.cpp renderers/*.cpp utf8/*.cpp)
file (GLOB H_FILES *.h image/*.h animation/*.h parser/*.h renderers/*.h utf8/*.h)
set (SOURCE_FILES ${C_FILES} ${H_FILES})

add_library(TurboBadger ${SOURCE_FILES})
target_include_directories(TurboBadger SYSTEM BEFORE PUBLIC .)
