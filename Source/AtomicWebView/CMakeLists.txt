include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/CEF )


file (GLOB SOURCE_FILES *.cpp *.h)

add_library(AtomicWebView ${SOURCE_FILES})

