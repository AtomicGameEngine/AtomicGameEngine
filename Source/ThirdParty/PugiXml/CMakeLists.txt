set (TARGET_NAME PugiXml)

file (GLOB SOURCE_FILES src/*.cpp include/*.hpp)

# Setup target
add_library(PugiXml ${SOURCE_FILES})

