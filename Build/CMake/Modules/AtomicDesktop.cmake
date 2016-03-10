
include(AtomicCommon)

include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_NAVIGATION -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS -DPOCO_STATIC )

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId SQLite)

# Check whether the CEF submodule is available
if (EXISTS ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    #Check if CEF got pulled
    file(GLOB RESULT ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    list(LENGTH ${RESULT} RES_LEN)
    if(RES_LEN EQUAL 0)
     # DIR is empty, so init the submodule and checkout master
     execute_process(COMMAND git submodule update --init --remote)
    endif()

    set(ATOMIC_WEBVIEW TRUE)
    add_definitions( -DATOMIC_WEBVIEW )
    include("AtomicWebView")
endif()

include(AtomicNET)
