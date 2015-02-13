
include_directories(${CMAKE_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include)

file (GLOB_RECURSE SOURCE_FILES Source/*.cpp Source/*.h)

include_directories (${CMAKE_SOURCE_DIR}/AtomicEditor/Source)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Vendor/nativefiledialog)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Poco/Foundation/include)

add_definitions(-DPOCO_NO_AUTOMATIC_LIBS)

set (SOURCE_FILES ${SOURCE_FILES} Vendor/nativefiledialog/nfd_common.c)

if (APPLE)
    set (EXE_TYPE MACOSX_BUNDLE)
    set (SOURCE_FILES ${SOURCE_FILES} Vendor/nativefiledialog/nfd_cocoa.mm Vendor/nativefiledialog/nfd.h)
else()
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Vendor/libcurl/include)
    add_definitions(-DCURL_STATICLIB)
    set (EXE_TYPE WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} Vendor/nativefiledialog/nfd_win.cpp Vendor/nativefiledialog/nfd.h)
endif(APPLE)

add_subdirectory(Vendor)

add_executable(AtomicEditor ${EXE_TYPE} ${SOURCE_FILES})

target_link_libraries(AtomicEditor ${ATOMIC_LINK_LIBRARIES} Poco)

if (APPLE)
    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)
    target_link_libraries(AtomicEditor curl)
else()

    target_link_libraries(AtomicEditor libcurl Iphlpapi Wldap32)

endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicEditor PROPERTIES ${TARGET_PROPERTIES})
endif ()
