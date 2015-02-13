

include_directories(src)

file (GLOB SOURCE_FILES src/*.c src/*.h)

if (MSVC AND CMAKE_CL_64)
    enable_language (ASM_MASM)
    add_custom_command (OUTPUT masm-x64.obj COMMAND ${CMAKE_ASM_MASM_COMPILER} -Fo ${CMAKE_CURRENT_BINARY_DIR}/masm-x64.obj -c ${CMAKE_CURRENT_SOURCE_DIR}/src/masm-x64.asm DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/masm-x64.asm) 
    set (ASM_FILES ${CMAKE_CURRENT_BINARY_DIR}/masm-x64.obj ${CMAKE_CURRENT_SOURCE_DIR}/src/masm-x64.asm)
	list (APPEND SOURCE_FILES ${ASM_FILES})	
endif ()

add_library(LibCpuId ${SOURCE_FILES})

