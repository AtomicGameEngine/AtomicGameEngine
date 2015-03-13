
add_custom_command(TARGET AtomicEditor POST_BUILD
                   COMMAND ln -sfh "${CMAKE_SOURCE_DIR}/Data/AtomicEditor/Resources/EditorData" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/EditorData"
                   COMMAND ln -sfh "${CMAKE_SOURCE_DIR}/Data/AtomicPlayer/Resources/CoreData" "$<TARGET_FILE_DIR:AtomicEditor>/../Resources/CoreData" )


