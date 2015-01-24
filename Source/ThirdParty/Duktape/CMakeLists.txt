# Define target name
set (TARGET_NAME Duktape)

# Define source files
file (GLOB CPP_FILES *.c)
file (GLOB H_FILES *.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

set (INCLUDE_DIRS .)

# Setup target
setup_library ()

# Install headers for building the Urho3D library
install_header_files (DIRECTORY ./ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/Duktape FILES_MATCHING PATTERN *.h BUILD_TREE_ONLY)  # Note: the trailing slash is significant
