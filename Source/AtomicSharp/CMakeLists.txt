
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D)

file (GLOB CSHARP_SOURCES *.cpp *.h)

if (NOT MSVC)
    # for kNet
    add_definitions (-DUNIX)
endif()

# Create the JSBind files ahead of time, so they are picked up with glob
set (CSFILES CSModuleAtomic2D.cpp;CSModuleAtomic3D.cpp;CSModuleAudio.cpp;CSModuleContainer.cpp;CSModuleCore.cpp;
             CSModuleEngine.cpp;CSModuleEnvironment.cpp;CSModuleGraphics.cpp;CSModuleInput.cpp;
             CSModuleIO.cpp;CSModuleJavascript.cpp;CSModuleMath.cpp;CSModuleNavigation.cpp;
             CSModuleNetwork.cpp;CSModulePhysics.cpp;CSModuleResource.cpp;CSPackageAtomic.cpp;
             CSModuleScene.cpp;CSModuleUI.cpp)

foreach(CSFILE ${CSFILES})

  set (CSFILEPATH "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/Atomic/Native/${JSFILE}")

  if (NOT EXISTS ${CSFILEPATH})
    file(WRITE "${CSFILEPATH}" "// will be created by JSBind")
  endif()

endforeach()


file (GLOB CSHARP_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/Atomic/Native/*.cpp)

set (SOURCE_FILES ${CSHARP_SOURCES} ${CSHARP_BINDINGS_SOURCE})

add_library(AtomicSharp SHARED ${SOURCE_FILES})

target_link_libraries(AtomicSharp AtomicJS AtomicPlayerLib AtomicPlayerJS ${ATOMIC_LINK_LIBRARIES})

if (APPLE)

target_link_libraries(AtomicSharp "-framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")

endif()

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_dependencies(AtomicSharp AtomicTool)
endif()

