
# beware nVidia vendor stuff does not compile with c++11 due to sizeof(bool) != 4 and potentially other issues
set(CMAKE_CXX_FLAGS ${ATOMIC_CXX_FLAGS_DEFAULT})
set(CMAKE_C_FLAGS ${ATOMIC_C_FLAGS_DEFAULT})

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/vendor 
                     ${CMAKE_CURRENT_SOURCE_DIR}/vendor/poshlib
                     ${CMAKE_SOURCE_DIR}/Source/ThirdParty/STB )


add_subdirectory(vendor)

if (APPLE)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

endif()

add_library( thekla_atlas thekla_atlas.cpp )

target_link_libraries( thekla_atlas nvmesh )
