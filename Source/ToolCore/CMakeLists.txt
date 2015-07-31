include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Assimp/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/nativefiledialog)

add_definitions(-DCPLUSPLUS_WITHOUT_QT)

if (MSVC)
    include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/libcurl/include)
    add_definitions(-DCURL_STATICLIB)
endif()

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(ToolCore ${SOURCE_FILES})

target_link_libraries(ToolCore Assimp Poco)

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
