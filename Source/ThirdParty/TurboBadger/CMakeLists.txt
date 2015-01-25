# Define target name
set (TARGET_NAME TurboBadger)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" ../FreeType/include ../SDL/include)

if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -Wall -Wextra -Wno-unused-parameter")
    add_definitions(-DMACOSX)
endif(APPLE)

add_definitions(-DTB_FONT_RENDERER_FREETYPE)

# Define source files
file (GLOB C_FILES *.cpp image/*.cpp animation/*.cpp parser/*.cpp renderers/*.cpp utf8/*.cpp)
file (GLOB H_FILES *.h image/*.h animation/*.h parser/*.h renderers/*.h utf8/*.h)
set (SOURCE_FILES ${C_FILES} ${H_FILES})

# Setup target
setup_library ()

# Install headers for building the Urho3D library
install_header_files (DIRECTORY ./ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/TurboBadger FILES_MATCHING PATTERN *.h BUILD_TREE_ONLY)  # Note: the trailing slash is significant


