
include(AtomicCommon)

include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_PLATFORM_DESKTOP -DATOMIC_NAVIGATION -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS -DPOCO_STATIC )

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} libsquish)

if (NOT LINUX)

    set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId)

endif()

# Check whether the CEF submodule is available
if (EXISTS ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    #Check if CEF got pulled by looking if the foldes is empty
    file(GLOB CEF_FILES ${CMAKE_SOURCE_DIR}/Submodules/CEF/*)

    list(LENGTH CEF_FILES CEF_FILES_LEN)
    if(CEF_FILES_LEN EQUAL 0)
     message(STATUS "Initialising CEF submodule")

     find_package(Git REQUIRED)
     if(GIT_FOUND)
      execute_process(COMMAND git submodule update --init --remote)
     else ()
      message(STATUS "Could not find git in your Path. Please install git")
     endif(GIT_FOUND)
    endif()
    set(ATOMIC_WEBVIEW TRUE)
    add_definitions( -DATOMIC_WEBVIEW )
endif()

