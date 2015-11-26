include_directories ( ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D
                      ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/Editor/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE})

set (SOURCE_FILES ${SOURCE_FILES} ${CSHARP_BINDINGS_SOURCE} ${CSHARPATOMICPLAYER_BINDINGS_SOURCE} ${CSHARPATOMICNET_BINDINGS_SOURCE})

if (APPLE)

    file (GLOB_RECURSE OBJC_FILES *.mm *.h)
    set (SOURCE_FILES ${SOURCE_FILES} ${OBJC_FILES})

    set (EXE_TYPE MACOSX_BUNDLE)

    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE Atomic.icns)
    set(ATOMIC_EDITOR_ICON ${CMAKE_SOURCE_DIR}/Build/CMake/Modules/Atomic.icns)
    set_source_files_properties(${ATOMIC_EDITOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

elseif(LINUX)

else()
    include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/libcurl/include)
    add_definitions(-DCURL_STATICLIB)

    # We want the console for now
    #set (EXE_TYPE WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)

    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/Build/CMake/Modules/Atomic.rc)
endif(APPLE)

add_executable(AtomicEditor ${EXE_TYPE} ${SOURCE_FILES} ${ATOMIC_EDITOR_ICON})

target_link_libraries(AtomicEditor ToolCore AtomicJS AtomicPlayerJS AtomicNETJS ToolCoreJS Poco nativefiledialog ${ATOMIC_LINK_LIBRARIES})

if (APPLE)
    set (TARGET_PROPERTIES MACOSX_BUNDLE_INFO_PLIST MacOSXBundleInfo.plist.template)

    target_link_libraries(AtomicEditor NETCore NETScript curl)

elseif(LINUX)
    target_link_libraries(AtomicEditor NETCore NETScript curl nativefiledialog ${GTK3_LIBRARIES})

else()
    target_link_libraries(AtomicEditor NETCore NETScript libcurl Iphlpapi Wldap32)

    # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
    add_custom_command (TARGET AtomicEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:AtomicEditor>/D3DCompiler_47.dll\")

endif()

if (TARGET_PROPERTIES)
    set_target_properties (AtomicEditor PROPERTIES ${TARGET_PROPERTIES})
endif ()

GroupSources("Application")
GroupSources("EditorMode")
GroupSources("Editors")
GroupSources("Javascript")
GroupSources("PlayerMode")
GroupSources("Utils")
GroupSources("Components")
