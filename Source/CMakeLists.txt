
include_directories_exported(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ThirdParty)
add_subdirectory(Atomic)
add_subdirectory(AtomicJS)


if (ATOMIC_WEBVIEW)
    add_subdirectory(AtomicWebView)
endif()

add_subdirectory(AtomicPlayer)
add_subdirectory(AtomicPlayerJS)

if (NOT EMSCRIPTEN)
    add_subdirectory(AtomicApp)
    add_subdirectory(AtomicNET)
endif()

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_subdirectory(ToolCore)
    add_subdirectory(ToolCoreJS)
    add_subdirectory(AtomicEditor)
    add_subdirectory(AtomicTool)
    add_subdirectory(Tools)
endif()


include(AtomicDocList)
