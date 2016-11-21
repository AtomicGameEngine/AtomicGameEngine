
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Box2D)

file (GLOB JAVASCRIPT_SOURCE Javascript/*.cpp Javascript/*.h)

GroupSources("Javascript")

if (NOT MSVC)
    # for kNet
    add_definitions (-DUNIX)
endif()

file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/Atomic/*.cpp)

set (SOURCE_FILES ${JAVASCRIPT_SOURCE} ${JAVASCRIPT_BINDINGS_SOURCE})

add_library(AtomicJS ${SOURCE_FILES})

add_dependencies(AtomicJS AtomicToolCheckScripts)
