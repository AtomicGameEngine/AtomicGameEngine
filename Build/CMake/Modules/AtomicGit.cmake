
macro (initialize_submodule target_dir result_variable)
    file(GLOB SUBMODULE_FILES ${target_dir}/*)
    list(LENGTH SUBMODULE_FILES SUBMODULE_FILES_LEN)

    if (SUBMODULE_FILES_LEN LESS 2)
        message(STATUS "Initialising ${target_dir}")
        find_package(Git REQUIRED)
        if (GIT_FOUND)
            execute_process(
                COMMAND git submodule update --init --remote "${target_dir}"
                WORKING_DIRECTORY ${ATOMIC_SOURCE_DIR}
                RESULT_VARIABLE SUBMODULE_RESULT
            )
            if (SUBMODULE_RESULT EQUAL 0)
                set (${result_variable} ON)
                message(INFO "Initialized ${target_dir}")
            else ()
                set (${result_variable} OFF)
                message(WARNING "Running git returned an error.")
            endif ()
        else ()
            message(STATUS "Could not find git in your Path. Please install git.")
        endif (GIT_FOUND)
    endif ()
endmacro ()
