project (JSBind)

include_directories(${CMAKE_SOURCE_DIR}/Source)

add_definitions(-DCPLUSPLUS_WITHOUT_QT)

# Define source files
file (GLOB SOURCE_FILES *.cpp cplusplus/*.cpp *.h cplusplus/*.h )

add_executable(JSBind ${SOURCE_FILES})

target_link_libraries(JSBind ${ATOMIC_LINK_LIBRARIES})

#TODO: finer grained dependencies
add_custom_command(OUTPUT "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Modules/JSModules.cpp"
                   COMMAND $<TARGET_FILE:JSBind> "\"${CMAKE_SOURCE_DIR}\"" "${JAVASCRIPT_BINDINGS_PLATFORM}"
                   DEPENDS JSBind
                   COMMENT "Generating Javascript Bindings")

add_custom_target(BuildJSBindings DEPENDS "${CMAKE_SOURCE_DIR}/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/Javascript/Modules/JSModules.cpp")
