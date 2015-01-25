
include_directories(${CMAKE_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty)

# Define source files
file (GLOB SOURCE_FILES *.cpp *.h )

if (MSVC)

set (EXE_TYPE WIN32)

endif()

add_executable(AtomicPlayer ${EXE_TYPE} ${SOURCE_FILES})

target_link_libraries(AtomicPlayer ${ATOMIC_LINK_LIBRARIES})
