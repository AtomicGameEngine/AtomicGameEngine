

add_executable(PackageTool PackageTool.cpp)

target_link_libraries(PackageTool ${ATOMIC_LINK_LIBRARIES})


file (GLOB_RECURSE EDITORDATA_FILES ${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData/*)
file (GLOB_RECURSE COREDATA_FILES ${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData/*)

if (MSVC)

    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/EditorData.pak"
                       COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${CMAKE_CURRENT_BINARY_DIR}/EditorData.pak" -c
                       DEPENDS PackageTool ${EDITORDATA_FILES})

    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CoreData.pak"
                      COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${CMAKE_CURRENT_BINARY_DIR}/CoreData.pak" -c
                      DEPENDS PackageTool ${COREDATA_FILES})

    add_custom_target(BuildPakFiles ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/EditorData.pak" "${CMAKE_CURRENT_BINARY_DIR}/CoreData.pak")

elseif (APPLE)

    set (EDITORDATA_PAK "${CMAKE_SOURCE_DIR}/Bin/EditorData.pak")
    set (COREDATA_PAK "${CMAKE_SOURCE_DIR}/Bin/CoreData.pak")

    add_custom_command(OUTPUT "${EDITORDATA_PAK}"
                       COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${EDITORDATA_PAK}" -c
                       COMMAND rsync -u "${EDITORDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/EditorData.pak"
                       DEPENDS PackageTool ${EDITORDATA_FILES})

    add_custom_command(OUTPUT "${COREDATA_PAK}"
                      COMMAND $<TARGET_FILE:PackageTool> "${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${COREDATA_PAK}" -c
                      COMMAND rsync -u "${COREDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/CoreData.pak"
                      DEPENDS PackageTool ${COREDATA_FILES})

    add_custom_target(BuildEditorFiles ALL DEPENDS AtomicEditor "${EDITORDATA_PAK}" "${COREDATA_PAK}")

endif()

