macro(GroupSources curdir)

    if (MSVC)

    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)

    foreach(child ${children})

        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})

            if ("${curdir}" STREQUAL "")
                GroupSources(${child})
            else()
                GroupSources(${curdir}/${child})
            endif()
        else()

            string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})

        endif()

    endforeach()

    endif()

endmacro()

if(NOT CMAKE_CROSSCOMPILING AND ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(LINUX TRUE CACHE BOOL "Indicates if host is Linux.")
endif()
