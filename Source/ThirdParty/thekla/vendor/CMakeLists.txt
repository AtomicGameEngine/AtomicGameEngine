include_directories( ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/nvcore
                     ${CMAKE_CURRENT_SOURCE_DIR}/poshlib )

add_subdirectory(nvcore)
add_subdirectory(nvimage)
add_subdirectory(nvmath)
add_subdirectory(nvmesh)
add_subdirectory(poshlib)
