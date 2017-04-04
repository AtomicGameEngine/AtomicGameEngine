# Define source files
file (GLOB SOURCE_FILES *.cpp *.h )

# set (SOURCE_FILES ${SOURCE_FILES}) ${CSHARP_BINDINGS_SOURCE} ${CSHARPATOMICPLAYER_BINDINGS_SOURCE}  ${CSHARPATOMICNET_BINDINGS_SOURCE})

if (WIN32)
    set (EXE_TYPE WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} ${ATOMIC_SOURCE_DIR}/Build/CMake/Modules/Atomic.rc)
elseif(APPLE)
    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE Atomic.icns)
if (NOT IOS)
    set(ATOMIC_PLAYER_ICON ${ATOMIC_SOURCE_DIR}/Build/CMake/Modules/Atomic.icns)
endif()
    set_source_files_properties(${ATOMIC_PLAYER_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set (EXE_TYPE MACOSX_BUNDLE)
endif()

if (ANDROID)
	add_library(AtomicPlayer SHARED ${SOURCE_FILES} ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/SDL/src/main/android/SDL_android_main.c)
else()

# iOS AtomicPlayer currently disabled
if (NOT IOS)
	add_executable(AtomicPlayer ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_PLAYER_ICON})
endif()
endif()

if (NOT IOS)
    target_link_libraries(AtomicPlayer AtomicJS AtomicPlayerLib AtomicPlayerJS Atomic)
endif()

if (WIN32)
  # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
  add_custom_command (TARGET AtomicPlayer POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicPlayer>/D3DCompiler_47.dll\")

endif(WIN32)


if (APPLE)

  if (NOT IOS)

    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)

  else()

#    set_target_properties(AtomicPlayer PROPERTIES
#      MACOSX_BUNDLE_GUI_IDENTIFIER "com.atomicgameengine.atomicplayer"
#      XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
#      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
#      XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
#      XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 9.3
#      XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" # iPhone/iPad
#      XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO
#    )

#    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${ATOMIC_SOURCE_DIR}/Build/CMake/Modules/iOSBundleInfo.plist.template)

  endif()

endif(APPLE)


if (TARGET_PROPERTIES)
if (NOT IOS)
    set_target_properties (AtomicPlayer PROPERTIES ${TARGET_PROPERTIES})
endif()
endif ()
