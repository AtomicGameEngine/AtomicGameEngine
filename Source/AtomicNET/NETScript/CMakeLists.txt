include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty)

set (CSATOMICNETSCRIPTNATIVEDIR "${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/AtomicNETScript/Native")
set (JSATOMICNETSCRIPTDIR "${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/AtomicNETScript")

file ( GLOB ATOMICNETSCRIPT_BINDINGS_SOURCE ${CSATOMICNETSCRIPTNATIVEDIR}/*.cpp ${CSATOMICNETSCRIPTNATIVEDIR}/*.h
                                            ${JSATOMICNETSCRIPTDIR}/*.cpp ${JSATOMICNETSCRIPTDIR}/*.h )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicNETScript ${SOURCE_FILES})

add_library(AtomicNETScriptBindings ${ATOMICNETSCRIPT_BINDINGS_SOURCE})

add_dependencies(AtomicNETScriptBindings AtomicToolCheckScripts)
