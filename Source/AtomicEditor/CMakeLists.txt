include_directories ( ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

# Remove the web helper sources
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/WebView/WebProcessHelperMac.cpp)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/Editor/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE})

#set (SOURCE_FILES ${SOURCE_FILES} ${CSHARP_BINDINGS_SOURCE} ${CSHARPATOMICPLAYER_BINDINGS_SOURCE} ${CSHARPATOMICNET_BINDINGS_SOURCE})

if (APPLE)

    file (GLOB_RECURSE OBJC_FILES *.mm *.h)
    set (SOURCE_FILES ${SOURCE_FILES} ${OBJC_FILES})

    set (EXE_TYPE MACOSX_BUNDLE)

    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE AtomicEditor.icns)
    set(ATOMIC_EDITOR_ICON ${CMAKE_SOURCE_DIR}/Build/CMake/Modules/AtomicEditor.icns)
    set_source_files_properties(${ATOMIC_EDITOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

else()

    include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/libcurl/include)
    add_definitions(-DCURL_STATICLIB)

    # We want a console for development builds
    if (${ATOMIC_DEV_BUILD} EQUAL 1)
      add_definitions(-DATOMIC_WIN32_CONSOLE)
    else()
      set (EXE_TYPE WIN32)
    endif()

    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/Build/CMake/Modules/Atomic.rc)

endif(APPLE)

if (ATOMIC_WEBVIEW)

# Target executable names.
set(CEF_TARGET "AtomicEditor")
if(OS_MACOSX)
  set(CEF_HELPER_TARGET "AtomicEditor Helper")
endif()

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()


#
# Windows configuration.
#

if(OS_WINDOWS)
  # Executable target.
  add_executable(${CEF_TARGET} ${EXE_TYPE} ${SOURCE_FILES})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Add the custom manifest files to the executable.
  ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}/Windows" "${CEF_TARGET}")

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
endif()

#
# Mac OS X configuration.
#

if(APPLE)
  # Output paths for the app bundles.
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")
  set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_TARGET}.app")

  # Variable referenced from Info.plist files.
  set(PRODUCT_NAME "${CEF_TARGET}")

  # Helper executable target.
  add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE "${CMAKE_CURRENT_SOURCE_DIR}/WebView/WebProcessHelperMac.cpp")
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
  add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} Atomic AtomicWebView ${ATOMIC_LINK_LIBRARIES})

  set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Build/CMake/Modules/AtomicEditor-HelperInfo.plist.template"
    )

  # Fix the framework link in the helper executable.
  FIX_MACOSX_HELPER_FRAMEWORK_LINK(${CEF_HELPER_TARGET} ${CEF_HELPER_APP})

  # Main executable target.
  add_executable(${CEF_TARGET} ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})

  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper "${CEF_HELPER_TARGET}")
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_TARGET} PROPERTIES
    RESOURCE "${CEFSIMPLE_RESOURCES_SRCS}"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
    )

  # Copy files into the main app bundle.
  add_custom_command(
    TARGET ${CEF_TARGET}
    POST_BUILD
    # Copy the helper app bundle into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_HELPER_APP}"
            "${CEF_APP}/Contents/Frameworks/${CEF_HELPER_TARGET}.app"
    # Copy the CEF framework into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
    )

  # Fix the framework link in the main executable.
  FIX_MACOSX_MAIN_FRAMEWORK_LINK(${CEF_TARGET} ${CEF_APP})

  if(NOT ${CMAKE_GENERATOR} STREQUAL "Xcode")
    # Manually process and copy over resource files.
    # The Xcode generator handles this via the set_target_properties RESOURCE directive.
    set(PREFIXES "mac/")  # Remove these prefixes from input file paths.
    COPY_MACOSX_RESOURCES("${CEFSIMPLE_RESOURCES_SRCS}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
  endif()
endif(APPLE)

if(OS_LINUX)
  # Executable target.
  add_executable(${CEF_TARGET} ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

  # Set rpath so that libraries can be placed next to the executable.
  set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()


endif(ATOMIC_WEBVIEW)

target_link_libraries(${CEF_TARGET} ToolCore AtomicApp AtomicWebView AtomicJS AtomicPlayerJS ToolCoreJS AtomicNETScriptBindings AtomicNETScript Poco nativefiledialog ${ATOMIC_LINK_LIBRARIES})


if (APPLE)

    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Build/CMake/Modules/AtomicEditorInfo.plist.template")
    set (MACOSX_BUNDLE_GUI_IDENTIFIER "com.thunderbeastgames.atomiceditor")
    set (MACOSX_BUNDLE_BUNDLE_NAME "AtomicEditor")

    target_link_libraries(AtomicEditor libcurl)

elseif(LINUX)
    target_link_libraries(AtomicEditor libcurl nativefiledialog ${GTK3_LIBRARIES})
    target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

else()
    target_link_libraries(AtomicEditor libcurl Iphlpapi Wldap32)

    # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
    add_custom_command (TARGET AtomicEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicEditor>/D3DCompiler_47.dll\")

endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicEditor PROPERTIES ${TARGET_PROPERTIES})
endif ()

add_dependencies(${CEF_TARGET} AtomicToolCheckScripts)

GroupSources("Application")
GroupSources("EditorMode")
GroupSources("Editors")
GroupSources("Javascript")
GroupSources("PlayerMode")
GroupSources("Utils")
GroupSources("Components")

include(AtomicDoc)
