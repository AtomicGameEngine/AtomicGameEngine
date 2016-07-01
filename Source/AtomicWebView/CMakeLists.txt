
include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/WebView/*.cpp)

if (APPLE)
    set (PLATFORM_SOURCE WebBrowserHost.mm)
endif()

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE} ${PLATFORM_SOURCE} )

add_library(AtomicWebView ${SOURCE_FILES})

add_dependencies(AtomicWebView AtomicToolCheckScripts)
