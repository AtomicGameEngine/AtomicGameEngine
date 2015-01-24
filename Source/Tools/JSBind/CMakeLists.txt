# Define target name
set (TARGET_NAME JSBind)

add_definitions(-DCPLUSPLUS_WITHOUT_QT)

# Define source files
define_source_files (GLOB_CPP_PATTERNS *.cpp *.c cplusplus/*.cpp GLOB_H_PATTERNS *.h cplusplus/*.h PARENT_SCOPE)

# Setup target
if (APPLE)
    setup_macosx_linker_flags (CMAKE_EXE_LINKER_FLAGS)
endif ()
setup_executable ()
