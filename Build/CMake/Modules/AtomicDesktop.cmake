set(ATOMIC_DESKTOP TRUE)

# Check whether the CEF submodule is available
if (NOT DEFINED ATOMIC_WEBVIEW AND EXISTS ${ATOMIC_SOURCE_DIR}/Submodules/CEF)
    #Check if CEF got pulled by looking if the foldes is empty
    file(GLOB CEF_FILES ${ATOMIC_SOURCE_DIR}/Submodules/CEF/*)

    list(LENGTH CEF_FILES CEF_FILES_LEN)
    if (CEF_FILES_LEN EQUAL 0)
        message(STATUS "Initialising CEF submodule")

        find_package(Git REQUIRED)
        if (GIT_FOUND)
            execute_process(COMMAND git submodule update --init --remote)
        else ()
            message(STATUS "Could not find git in your Path. Please install git")
        endif (GIT_FOUND)
    endif ()
    set(ATOMIC_WEBVIEW TRUE)
    add_definitions(-DATOMIC_WEBVIEW)
endif ()

