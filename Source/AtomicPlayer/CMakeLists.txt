file (GLOB SOURCE_FILES *.cpp *.h)

add_library(AtomicPlayerLib ${SOURCE_FILES})
target_link_libraries(AtomicPlayerLib Atomic)
add_subdirectory(Application)
