
include_directories(${CMAKE_SOURCE_DIR}/Source)
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty)

# Define source files
file (GLOB SOURCE_FILES *.cpp *.h )

if (MSVC)
    set (EXE_TYPE WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.rc)
elseif(APPLE)
    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE Atomic.icns)
    set(ATOMIC_EDITOR_ICON ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.icns)
    set_source_files_properties(${ATOMIC_EDITOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set (EXE_TYPE MACOSX_BUNDLE)    
endif()

if (ANDROID)
	include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/SDL/include)
	add_library(AtomicPlayer SHARED ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/Source/ThirdParty/SDL/src/main/android/SDL_android_main.c)
else()
	add_executable(AtomicPlayer ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})
endif()

target_link_libraries(AtomicPlayer ${ATOMIC_LINK_LIBRARIES})

if (APPLE)

if (NOT IOS)

    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)

else()

    set_target_properties(AtomicPlayer PROPERTIES
      MACOSX_BUNDLE_GUI_IDENTIFIER "com.atomicgameengine.atomicplayer"
      XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
      XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
      XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 7.1
      XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" # iPhone/iPad
      XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO
    )

	set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/CMake/Modules/iOSBundleInfo.plist.template)

endif()
endif(APPLE)

if (TARGET_PROPERTIES)
    set_target_properties (AtomicPlayer PROPERTIES ${TARGET_PROPERTIES})
endif ()

