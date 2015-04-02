
add_subdirectory(ThirdParty)
add_subdirectory(Atomic)
add_subdirectory(AtomicJS)
add_subdirectory(AtomicPlayer)

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)    
    add_subdirectory(ToolCore)
    add_subdirectory(AtomicEditor)
    add_subdirectory(AtomicTool)
    add_subdirectory(Tools)
endif()


