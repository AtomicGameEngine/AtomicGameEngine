
# If submodules aren't initialized, look for git and initialize
if (NOT EXISTS ${ATOMIC_SOURCE_DIR}/Submodules/CEF/.git OR NOT EXISTS ${ATOMIC_SOURCE_DIR}/Submodules/AtomicExamples/.git)
    find_package(Git)
    if (GIT_FOUND)
        message("\n\nUpdating submodules, please wait...\n\n")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init
            WORKING_DIRECTORY ${ATOMIC_SOURCE_DIR}
            RESULT_VARIABLE ATOMIC_GIT_STATUS)
        # check return status
        if (NOT ATOMIC_GIT_STATUS EQUAL 0)
            message(FATAL_ERROR "BUILD ERROR:\n\nError initializing submodules: git submodule update --init returned ${ATOMIC_GIT_STATUS}\n")
        endif ()
    else ()
        # Git not found
        message(FATAL_ERROR "BUILD ERROR:\n\Submodules not initialized, please run: git submodule update --init\n")
    endif ()
endif ()
