set (CSATOMICNATIVEDIR "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/Atomic/Native")
set (CSATOMICPLAYERNATIVEDIR "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/AtomicPlayer/Native")
set (CSATOMICNETNATIVEDIR "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/AtomicNET/Native")

# Create the JSBind files ahead of time, so they are picked up with glob

set (CSFILES CSModuleAtomic2D.cpp;CSModuleAtomic3D.cpp;CSModuleAudio.cpp;CSModuleContainer.cpp;CSModuleCore.cpp;
             CSModuleEngine.cpp;CSModuleEnvironment.cpp;CSModuleGraphics.cpp;CSModuleInput.cpp;
             CSModuleIO.cpp;CSModuleJavascript.cpp;CSModuleMath.cpp;CSModuleNavigation.cpp;
             CSModuleNetwork.cpp;CSModulePhysics.cpp;CSModuleResource.cpp;CSPackageAtomic.cpp;
             CSModuleScene.cpp;CSModuleUI.cpp;CSPackageAtomic.h;CSModuleAtomicNET.cpp)

set (CSATOMICPLAYERFILES CSModulePlayer.cpp;CSPackageAtomicPlayer.cpp;CSPackageAtomicPlayer.h)

set (CSATOMICNETFILES CSModuleNETCore.cpp;CSModuleNETScript.cpp;CSPackageAtomicNET.cpp;CSPackageAtomicNET.h)

foreach(CSFILE ${CSFILES})

  set (CSFILEPATH "${CSATOMICNATIVEDIR}/${CSFILE}")

  if (NOT EXISTS ${CSFILEPATH})
    file(WRITE "${CSFILEPATH}" "// will be created by AtomicTool")
  endif()

endforeach()

foreach(CSFILE ${CSATOMICPLAYERFILES})

  set (CSFILEPATH "${CSATOMICPLAYERNATIVEDIR}/${CSFILE}")

  if (NOT EXISTS ${CSFILEPATH})
    file(WRITE "${CSFILEPATH}" "// will be created by AtomicTool")
  endif()

endforeach()

foreach(CSFILE ${CSATOMICNETFILES})

  set (CSFILEPATH "${CSATOMICNETNATIVEDIR}/${CSFILE}")

  if (NOT EXISTS ${CSFILEPATH})
    file(WRITE "${CSFILEPATH}" "// will be created by AtomicTool")
  endif()

endforeach()


file (GLOB CSHARP_BINDINGS_SOURCE ${CSATOMICNATIVEDIR}/*.cpp ${CSATOMICNATIVEDIR}/*.h)
file (GLOB CSHARPATOMICPLAYER_BINDINGS_SOURCE ${CSATOMICPLAYERNATIVEDIR}/*.cpp ${CSATOMICPLAYERNATIVEDIR}/*.h)
file (GLOB CSHARPATOMICNET_BINDINGS_SOURCE ${CSATOMICNETNATIVEDIR}/*.cpp ${CSATOMICNETNATIVEDIR}/*.h)
