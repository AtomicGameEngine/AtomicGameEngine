
add_subdirectory(ThirdParty)
add_subdirectory(Atomic)
add_subdirectory(AtomicPlayer)

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
add_subdirectory(Tools)
add_subdirectory(AtomicEditor)
endif()


