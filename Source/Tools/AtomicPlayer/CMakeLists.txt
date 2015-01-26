
include_directories(${CMAKE_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty)

# Define source files
file (GLOB SOURCE_FILES *.cpp *.h )

if (MSVC)
    set (EXE_TYPE WIN32)
else()
    set (EXE_TYPE MACOSX_BUNDLE)
endif()

add_executable(AtomicPlayer ${EXE_TYPE} ${SOURCE_FILES})

target_link_libraries(AtomicPlayer ${ATOMIC_LINK_LIBRARIES})

if (APPLE)
    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)
endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicPlayer PROPERTIES ${TARGET_PROPERTIES})
endif ()

