

add_executable(PackageTool PackageTool.cpp)

target_link_libraries(PackageTool Atomic)


file (GLOB_RECURSE EDITORDATA_FILES ${ATOMIC_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData/*)
file (GLOB_RECURSE COREDATA_FILES ${ATOMIC_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData/*)

if (ATOMIC_BUILD_PAK_FILES)

set (EDITORDATA_PAK "${ATOMIC_SOURCE_DIR}/Bin/EditorData.pak")
set (COREDATA_PAK "${ATOMIC_SOURCE_DIR}/Bin/CoreData.pak")

if (WIN32)

    add_custom_command(OUTPUT "${EDITORDATA_PAK}"
                       COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${EDITORDATA_PAK}" -c
                       COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy \"${EDITORDATA_PAK}\" \"$<TARGET_FILE_DIR:AtomicEditor>/EditorData.pak\"
                       DEPENDS PackageTool ${EDITORDATA_FILES})

    add_custom_command(OUTPUT "${COREDATA_PAK}"
                      COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${COREDATA_PAK}" -c
                      COMMAND ${CMAKE_COMMAND}
                      ARGS -E copy \"${COREDATA_PAK}\" \"$<TARGET_FILE_DIR:AtomicEditor>/CoreData.pak\"
                      DEPENDS PackageTool ${COREDATA_FILES})

elseif (APPLE)

    add_custom_command(OUTPUT "${EDITORDATA_PAK}"
                       COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${EDITORDATA_PAK}" -c
                       COMMAND rsync -u "${EDITORDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/EditorData.pak"
                       DEPENDS PackageTool ${EDITORDATA_FILES})

    add_custom_command(OUTPUT "${COREDATA_PAK}"
                      COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${COREDATA_PAK}" -c
                      COMMAND rsync -u "${COREDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/CoreData.pak"
                      DEPENDS PackageTool ${COREDATA_FILES})

elseif (LINUX)

    add_custom_command(OUTPUT "${EDITORDATA_PAK}"
                       COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "${EDITORDATA_PAK}" -c
                       COMMAND rsync -u "${EDITORDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/EditorData.pak"
                       DEPENDS PackageTool ${EDITORDATA_FILES})

    add_custom_command(OUTPUT "${COREDATA_PAK}"
                      COMMAND $<TARGET_FILE:PackageTool> "${ATOMIC_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "${COREDATA_PAK}" -c
                      COMMAND rsync -u "${COREDATA_PAK}" "$<TARGET_FILE_DIR:AtomicEditor>/CoreData.pak"
                      DEPENDS PackageTool ${COREDATA_FILES})

endif()

add_custom_target(BuildEditorFiles ALL DEPENDS AtomicEditor "${EDITORDATA_PAK}" "${COREDATA_PAK}")

endif()
