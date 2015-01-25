
include_directories(${CMAKE_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty)

# Define source files
file (GLOB SOURCE_FILES *.cpp *.h )

add_executable(AtomicPlayer ${SOURCE_FILES})

target_link_libraries(AtomicPlayer ${ATOMIC_LINK_LIBRARIES})
