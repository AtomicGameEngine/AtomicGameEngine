include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog)


file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

# Create the JSBind files ahead of time, so they are picked up with glob
set (JSFILES JSPackageEditor.cpp;JSModuleEditor.cpp)

foreach(JSFILE ${JSFILES})

  set (JSFILEPATH "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/Editor/${JSFILE}")

  if (NOT EXISTS ${JSFILEPATH})
    file(WRITE "${JSFILEPATH}" "// will be created by JSBind")
  endif()

endforeach()

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/Editor/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE})

if (APPLE)
    set (EXE_TYPE MACOSX_BUNDLE)

    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE Atomic.icns)
    set(ATOMIC_EDITOR_ICON ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.icns)
    set_source_files_properties(${ATOMIC_EDITOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

elseif(LINUX)

else()
    include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/libcurl/include)
    add_definitions(-DCURL_STATICLIB)

    # We want the console for now
    #set (EXE_TYPE WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)

    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/CMake/Modules/Atomic.rc)
endif(APPLE)

add_executable(AtomicEditorWork ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})

target_link_libraries(AtomicEditorWork ToolCore AtomicJS ToolCoreJS Poco nativefiledialog ${ATOMIC_LINK_LIBRARIES})

if (APPLE)
    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)
    target_link_libraries(AtomicEditorWork curl)
elseif(LINUX)
    target_link_libraries(AtomicEditorWork curl nativefiledialog ${GTK3_LIBRARIES})
else()
    target_link_libraries(AtomicEditorWork libcurl Iphlpapi Wldap32)

    # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
    add_custom_command (TARGET AtomicEditorWork POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicEditor>/D3DCompiler_47.dll\")

endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicEditorWork PROPERTIES ${TARGET_PROPERTIES})
endif ()
