include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty)

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

# Create the JSBind files ahead of time, so they are picked up with glob
set (JSFILES JSPackageAtomicPlayer.cpp;JSModulePlayer.cpp)

foreach(JSFILE ${JSFILES})

  set (JSFILEPATH "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/AtomicPlayer/${JSFILE}")

  if (NOT EXISTS ${JSFILEPATH})
    file(WRITE "${JSFILEPATH}" "// will be created by JSBind")
  endif()

endforeach()

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/AtomicPlayer/*.cpp)

set (SOURCE_FILES ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE} )

add_library(AtomicPlayerJS ${SOURCE_FILES})

target_link_libraries(AtomicPlayerJS AtomicPlayerLib)

add_dependencies(AtomicPlayerJS AtomicTool)
