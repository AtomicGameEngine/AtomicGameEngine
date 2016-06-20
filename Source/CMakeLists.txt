
add_subdirectory(ThirdParty)
add_subdirectory(AtomicBuildInfo)
add_subdirectory(Atomic)
add_subdirectory(AtomicJS)


if (ATOMIC_WEBVIEW)
    add_subdirectory(AtomicWebView)
endif()

add_subdirectory(AtomicPlayer)
add_subdirectory(AtomicPlayerJS)

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_subdirectory(AtomicApp)
    add_subdirectory(ToolCore)
    add_subdirectory(ToolCoreJS)
    add_subdirectory(AtomicEditor)
    add_subdirectory(AtomicTool)
    add_subdirectory(Tools)
    add_subdirectory(AtomicNET)
endif()

include(AtomicDocList)
