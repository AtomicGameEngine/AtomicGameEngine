# create docs
find_program(CLDOC cldoc)
if(CLDOC)

    get_property(include_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    get_directory_property( DEFS DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
    
    FOREACH(infileName ${include_dirs})
        LIST(APPEND DOC_INCLUDES "-I${infileName}")
    ENDFOREACH(infileName)

    FOREACH(DefName ${DEFS})
        LIST(APPEND DOC_DEFINES "-D${DefName}")
    ENDFOREACH(DefName)

    string(REGEX MATCHALL "([^\ ]+)" CXX_LIST "${CMAKE_CXX_FLAGS}")
    string(REGEX MATCHALL "([^\ ]+)" C_LIST "${CMAKE_C_FLAGS}")

    get_filename_component(COMPNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
    SET(TARGETNAME ${COMPNAME}Docs)

    MESSAGE(STATUS "Enabling documentation for: " ${COMPNAME})

    SET(doc_args generate ${CXX_LIST} ${C_LIST} -std=c++11 -DATOMIC_DEV_BUILD=1 ${DOC_DEFINES} ${DOC_INCLUDES} -- --type html --language c++ --output ${CMAKE_SOURCE_DIR}/Artifacts/Build/AtomicDocs )

    LIST( APPEND doc_args ${SOURCE_FILES} )

    add_custom_target(${TARGETNAME} COMMAND ${CLDOC} ${doc_args})
  
    set_target_properties(${TARGETNAME} PROPERTIES
        EXCLUDE_FROM_ALL 1
        EXCLUDE_FROM_DEFAULT_BUILD 1
    )

endif()