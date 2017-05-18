# TODO: Look into the application-extension flag for iOS
#if (IOS)
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fapplication-extension")
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fapplication-extension")
#endif()

set (CSATOMICDIR "${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/CSharp/Packages/")

file (GLOB CSHARP_BINDINGS_SOURCE ${CSATOMICDIR}/Atomic/Native/*.cpp ${CSATOMICDIR}/Atomic/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETNative/Native/*.cpp ${CSATOMICDIR}/AtomicNETNative/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETScript/Native/*.cpp ${CSATOMICDIR}/AtomicNETScript/Native/*.h
                                  ${CSATOMICDIR}/AtomicApp/Native/*.cpp ${CSATOMICDIR}/AtomicApp/Native/*.h
                                  ${CSATOMICDIR}/AtomicPlayer/Native/*.cpp ${CSATOMICDIR}/AtomicPlayer/Native/*.h )

file (GLOB SOURCE_FILES *.cpp *.h)

if (NOT IOS AND NOT ANDROID AND NOT WEB)
    file (GLOB DESKTOP_SOURCE_FILES Desktop/*.cpp Desktop/*.h)
    set (SOURCE_FILES ${SOURCE_FILES} ${DESKTOP_SOURCE_FILES})
endif()

if (ANDROID)
	set (SOURCE_FILES ${SOURCE_FILES} ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/SDL/src/main/android/SDL_android_main.c)
endif()

add_library(AtomicNETNative SHARED ${SOURCE_FILES} ${CSHARP_BINDINGS_SOURCE})
if (ATOMIC_DESKTOP)
    add_dependencies(AtomicNETNative AtomicToolCheckScripts)
endif ()
target_link_libraries(AtomicNETNative AtomicApp AtomicNETScriptBindings AtomicNETScript AtomicJS AtomicPlayerLib AtomicPlayerJS Atomic)
target_include_directories(AtomicNETNative PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (APPLE)
    if (NOT IOS)
        target_link_libraries( AtomicNETNative "-stdlib=libc++ -framework AudioToolbox -framework Carbon -framework Cocoa -framework CoreAudio -framework CoreVideo -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")
    else()
        set_target_properties(AtomicNETNative PROPERTIES
            FRAMEWORK TRUE
            MACOSX_FRAMEWORK_IDENTIFIER com.atomicgameengine.atomicnetframework
            MACOSX_FRAMEWORK_INFO_PLIST ${ATOMIC_SOURCE_DIR}/Build/CMake/IOS/AtomicNET.framework.plist
            # PUBLIC_HEADER dynamicFramework.h
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        )
        target_link_libraries( AtomicNETNative "-framework AudioToolbox -framework AVFoundation -framework CoreAudio -framework CoreGraphics -framework Foundation -framework OpenGLES -framework QuartzCore -framework UIKit -framework CoreMotion -framework GameController")
    endif()
endif()

if (WIN32)
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

set (ATOMICNET_NATIVE_DIR "${ATOMIC_SOURCE_DIR}/Artifacts/AtomicNET/$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/Native/${ATOMICNET_NATIVE_PLATFORM}")

if (NOT IOS)
    add_custom_command( TARGET AtomicNETNative POST_BUILD
                        COMMAND "${CMAKE_COMMAND}" ARGS -E make_directory "\"${ATOMICNET_NATIVE_DIR}\""
                        COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different \"$<TARGET_FILE:AtomicNETNative>\" "\"${ATOMICNET_NATIVE_DIR}\"" )
else()
    # See BuildIOS.js for handling of the AtomicNETNative framework, including install_name_tool modification, etc
    # Handling it here was causing issues with certain builds of XCode and where it thought the TARGET_FILE was...
endif()

if (WIN32)
    # Copy the D3D shader compiler (for pre-Windows 8)
    add_custom_command( TARGET AtomicNETNative POST_BUILD
                        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${ATOMICNET_NATIVE_DIR}/D3DCompiler_47.dll\" )
endif(WIN32)
