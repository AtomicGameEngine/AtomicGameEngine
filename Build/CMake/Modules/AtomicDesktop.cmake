
include(AtomicCommon)

include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_NAVIGATION -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS -DPOCO_STATIC )

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId SQLite)

# Check whether the CEF submodule is available
if (EXISTS ${CMAKE_SOURCE_DIR}/Submodules/CEF)
    set(ATOMIC_WEBVIEW TRUE)
    add_definitions( -DATOMIC_WEBVIEW )
    include("AtomicWebView")
endif()

include(AtomicNET)
