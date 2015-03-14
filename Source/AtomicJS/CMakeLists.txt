
if (MSVC)
    set (JAVASCRIPT_BINDINGS_PLATFORM "WINDOWS")
elseif(APPLE)
    if (NOT IOS)
      set (JAVASCRIPT_BINDINGS_PLATFORM "MACOSX")
    else()
      set (JAVASCRIPT_BINDINGS_PLATFORM "IOS")
    endif()
elseif(ANDROID)
    set (JAVASCRIPT_BINDINGS_PLATFORM "ANDROID")
elseif (EMSCRIPTEN)
  set (JAVASCRIPT_BINDINGS_PLATFORM "WEB")
endif()

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_subdirectory(JSBind)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D)

file (GLOB JAVASCRIPT_SOURCE Javascript/*.cpp Javascript/*.h)

if (NOT MSVC)
    # for kNet
    add_definitions (-DUNIX)
endif()


file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Modules/*.cpp)

set (SOURCE_FILES ${JAVASCRIPT_SOURCE} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(AtomicJS ${SOURCE_FILES})

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_dependencies(AtomicJS JSBind BuildJSBindings)
endif()
