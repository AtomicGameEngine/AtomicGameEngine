
include(AtomicCommon)

include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_NAVIGATION -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS -DPOCO_STATIC )

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId SQLite)

include(AtomicNET)
