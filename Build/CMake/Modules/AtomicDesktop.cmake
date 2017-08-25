set(ATOMIC_DESKTOP TRUE)

# Check whether the CEF submodule is available
if (NOT MINGW)
    if (NOT DEFINED ATOMIC_WEBVIEW)
        set (ATOMIC_WEBVIEW ON)
    endif ()
    if (ATOMIC_WEBVIEW)
        initialize_submodule(Submodules/CEF ATOMIC_WEBVIEW)
        if (ATOMIC_WEBVIEW)
            add_definitions(-DATOMIC_WEBVIEW)
        endif ()
    endif ()
endif ()
