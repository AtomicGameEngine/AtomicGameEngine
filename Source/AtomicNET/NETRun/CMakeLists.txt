include_directories (${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty )

file (GLOB SOURCE_FILES *.cpp *.h)

add_executable(NETRun ${SOURCE_FILES})

target_link_libraries(NETRun NETCore NETScript Poco ${ATOMIC_LINK_LIBRARIES})
