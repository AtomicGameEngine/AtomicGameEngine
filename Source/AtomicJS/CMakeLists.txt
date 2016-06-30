
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D)

file (GLOB JAVASCRIPT_SOURCE Javascript/*.cpp Javascript/*.h)

GroupSources("Javascript")

if (NOT MSVC)
    # for kNet
    add_definitions (-DUNIX)
endif()

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Packages/Atomic/*.cpp)

set (SOURCE_FILES ${JAVASCRIPT_SOURCE} ${JAVASCRIPT_BINDINGS_SOURCE})

add_library(AtomicJS ${SOURCE_FILES})

add_dependencies(AtomicJS AtomicToolCheckScripts)
