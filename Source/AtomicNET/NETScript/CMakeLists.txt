include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty)

set (CSATOMICNETSCRIPTNATIVEDIR "${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/CSharp/Packages/AtomicNETScript/Native")
set (JSATOMICNETSCRIPTDIR "${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/AtomicNETScript")

file ( GLOB ATOMICNETSCRIPT_BINDINGS_SOURCE ${CSATOMICNETSCRIPTNATIVEDIR}/*.cpp ${CSATOMICNETSCRIPTNATIVEDIR}/*.h
                                            ${JSATOMICNETSCRIPTDIR}/*.cpp ${JSATOMICNETSCRIPTDIR}/*.h )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicNETScript ${SOURCE_FILES})

add_library(AtomicNETScriptBindings ${ATOMICNETSCRIPT_BINDINGS_SOURCE})

add_dependencies(AtomicNETScriptBindings AtomicToolCheckScripts)
