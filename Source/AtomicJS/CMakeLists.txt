
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

# Create the JSBind files ahead of time, so they are picked up with glob
set (JSFILES JSModuleAtomic2D.cpp;JSModuleAtomic2D.cpp;JSModuleAudio.cpp;JSModuleContainer.cpp;JSModuleCore.cpp;
             JSModuleEngine.cpp;JSModuleEnvironment.cpp;JSModuleGraphics.cpp;JSModuleInput.cpp;
             JSModuleIO.cpp;JSModuleJavascript.cpp;JSModuleMath.cpp;JSModuleNavigation.cpp;
             JSModuleNetwork.cpp;JSModulePhysics.cpp;JSModuleResource.cpp;JSModules.cpp;
             JSModuleScene.cpp;JSModuleUI.cpp)

foreach(JSFILE ${JSFILES})

  set (JSFILEPATH "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Modules/${JSFILE}")

  if (NOT EXISTS ${JSFILEPATH})
    file(WRITE "${JSFILEPATH}" "// will be created by JSBind")
  endif()

endforeach()


file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Modules/*.cpp)

set (SOURCE_FILES ${JAVASCRIPT_SOURCE} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(AtomicJS ${SOURCE_FILES})

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_dependencies(AtomicJS JSBind)
endif()
