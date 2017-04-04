add_definitions(-DCPLUSPLUS_WITHOUT_QT)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

if (APPLE)
    file (GLOB_RECURSE OBJC_FILES *.mm )
    set (SOURCE_FILES ${SOURCE_FILES} ${OBJC_FILES})
endif()

add_library(ToolCore ${SOURCE_FILES})

target_link_libraries(ToolCore Assimp Poco Atomic)

if (ATOMIC_XCODE)
    target_compile_definitions(ToolCore PRIVATE -DATOMIC_XCODE)
endif ()

GroupSources("Assets")
GroupSources("Build")
GroupSources("Command")
GroupSources("Import")
GroupSources("JSBind")
GroupSources("License")
GroupSources("Net")
GroupSources("Platform")
GroupSources("Project")
GroupSources("Subprocess")
GroupSources("NETTools")
