# create target to generate docs with clang based doc tool
# NB: pretty brute force ATM
if(EXISTS "/usr/local/bin/cldoc")
   add_custom_target(AtomicEngineDocs 
    DEPENDS AtomicEditorDocs AtomicDocs
    )
endif()