include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/nativefiledialog
                     ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Poco/Foundation/include)

file (GLOB_RECURSE SOURCE_FILES Source/*.cpp Source/*.h)

add_definitions(-DPOCO_NO_AUTOMATIC_LIBS)

set (SOURCE_FILES ${SOURCE_FILES} Vendor/nativefiledialog/nfd_common.c)

if (APPLE)
    set (EXE_TYPE MACOSX_BUNDLE)
    set (SOURCE_FILES ${SOURCE_FILES} Source/Build/BuildIOSUtils.mm Vendor/nativefiledialog/nfd_cocoa.mm Vendor/nativefiledialog/nfd.h)

    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE Atomic.icns)
    set(ATOMIC_EDITOR_ICON ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.icns)
    set_source_files_properties(${ATOMIC_EDITOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")    
else()
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/Vendor/libcurl/include)
    add_definitions(-DCURL_STATICLIB)
    set (EXE_TYPE WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} Vendor/nativefiledialog/nfd_win.cpp Vendor/nativefiledialog/nfd.h)
    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.rc)
endif(APPLE)

add_subdirectory(Vendor)

add_executable(AtomicEditor ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})

target_link_libraries(AtomicEditor ${ATOMIC_LINK_LIBRARIES} AtomicJS Poco)

if (APPLE)    
    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)
    target_link_libraries(AtomicEditor curl)
else()
    target_link_libraries(AtomicEditor libcurl Iphlpapi Wldap32)
endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicEditor PROPERTIES ${TARGET_PROPERTIES})
endif ()


file (GLOB_RECURSE EDITORDATA_FILES ${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData/*)
file (GLOB_RECURSE COREDATA_FILES ${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData/*)

add_custom_command (OUTPUT "${CMAKE_SOURCE_DIR}/Bin/EditorData.pak"
                    COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${CMAKE_SOURCE_DIR}/Bin/EditorData.pak" -c
                    DEPENDS PackageTool ${EDITORDATA_FILES}
                    COMMENT "Packaging Editor Data")

add_custom_command (OUTPUT "${CMAKE_SOURCE_DIR}/Bin/CoreData.pak"
                    COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${CMAKE_SOURCE_DIR}/Bin/CoreData.pak" -c
                    DEPENDS PackageTool ${COREDATA_FILES}
                    COMMENT "Packaging Core Data")

add_custom_target(BuildEditorDataPak DEPENDS "${CMAKE_SOURCE_DIR}/Bin/EditorData.pak")
add_custom_target(BuildCoreDataPak DEPENDS "${CMAKE_SOURCE_DIR}/Bin/CoreData.pak")

if (APPLE)

    add_custom_target(CopyPakFiles ALL
                      COMMAND
                      rsync -u "${CMAKE_SOURCE_DIR}/Bin/EditorData.pak" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/EditorData.pak"
                      COMMAND
                      rsync -u "${CMAKE_SOURCE_DIR}/Bin/CoreData.pak" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/CoreData.pak"
                      COMMENT "Copying Editor Resources")


else()

    add_custom_target(CopyPakFiles ALL
                      COMMAND xcopy \"${CMAKE_SOURCE_DIR}\\Bin\\EditorData.pak\" \"$<TARGET_FILE_DIR:AtomicEditor>\" /y
                      COMMAND xcopy \"${CMAKE_SOURCE_DIR}\\Bin\\CoreData.pak\" \"$<TARGET_FILE_DIR:AtomicEditor>\" /y
                      COMMENT "Copying Editor Resources")

endif()

add_dependencies( AtomicEditor BuildCoreDataPak BuildEditorDataPak)










