include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D)

set (CSATOMICDIR "${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/CSharp/Packages/")

file (GLOB CSHARP_BINDINGS_SOURCE ${CSATOMICDIR}/Atomic/Native/*.cpp ${CSATOMICDIR}/Atomic/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETNative/Native/*.cpp ${CSATOMICDIR}/AtomicNETNative/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETScript/Native/*.cpp ${CSATOMICDIR}/AtomicNETScript/Native/*.h
                                  ${CSATOMICDIR}/AtomicApp/Native/*.cpp ${CSATOMICDIR}/AtomicApp/Native/*.h
                                  ${CSATOMICDIR}/AtomicPlayer/Native/*.cpp ${CSATOMICDIR}/AtomicPlayer/Native/*.h )

file (GLOB SOURCE_FILES *.cpp *.h)

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    file (GLOB DESKTOP_SOURCE_FILES Desktop/*.cpp Desktop/*.h)
    set (SOURCE_FILES ${SOURCE_FILES} ${DESKTOP_SOURCE_FILES})
endif()

if (ANDROID)
	include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/SDL/include)
	set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/Source/ThirdParty/SDL/src/main/android/SDL_android_main.c)
endif()

add_library(AtomicNETNative SHARED ${SOURCE_FILES} ${CSHARP_BINDINGS_SOURCE})

add_dependencies(AtomicNETNative AtomicToolCheckScripts)

target_link_libraries(AtomicNETNative AtomicApp AtomicNETScriptBindings AtomicNETScript AtomicJS AtomicPlayerLib AtomicPlayerJS ${ATOMIC_LINK_LIBRARIES})

if (LINUX)

#https://github.com/nothings/stb/issues/280
target_link_libraries(AtomicNETNative gcc_s gcc)

endif()

if (APPLE)

    if (NOT IOS)

        target_link_libraries( AtomicNETNative "-stdlib=libc++ -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework CoreVideo -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")

    endif()

endif()

if (MSVC)
    set (ATOMICNET_NATIVE_PLATFORM Windows)
elseif(APPLE)
    if (IOS)
        set (ATOMICNET_NATIVE_PLATFORM iOS)
    else()
        set (ATOMICNET_NATIVE_PLATFORM Mac)
    endif()
elseif(LINUX)
  set (ATOMICNET_NATIVE_PLATFORM Linux)
elseif(ANDROID)
    set (ATOMICNET_NATIVE_PLATFORM Android)
endif()

set (ATOMICNET_NATIVE_DIR "${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/Native/${ATOMICNET_NATIVE_PLATFORM}")

add_custom_command( TARGET AtomicNETNative POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" ARGS -E make_directory "\"${ATOMICNET_NATIVE_DIR}\""
                    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different \"$<TARGET_FILE:AtomicNETNative>\" "\"${ATOMICNET_NATIVE_DIR}\"" )

if (MSVC)

    # Copy the D3D shader compiler (for pre-Windows 8)
    add_custom_command( TARGET AtomicNETNative POST_BUILD
                        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${ATOMICNET_NATIVE_DIR}/D3DCompiler_47.dll\" )
endif(MSVC)
