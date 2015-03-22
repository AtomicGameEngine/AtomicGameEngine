
add_subdirectory(ThirdParty)
add_subdirectory(Atomic)
add_subdirectory(AtomicJS)
add_subdirectory(AtomicPlayer)

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)    
	add_subdirectory(AtomicToolbox)
	add_subdirectory(AtomicCLI)
    add_subdirectory(AtomicEditor)
    add_subdirectory(Tools)
endif()


