
# Define target name
set (TARGET_NAME MojoShader)

# Define source files
file (GLOB SOURCE_FILES *.c)

if (MSVC)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS=1 -TP)
endif (MSVC)

add_library(MojoShader ${SOURCE_FILES})
