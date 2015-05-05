
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/Poco/Foundation/include)

add_definitions( -DATOMIC_TBUI -DATOMIC_FILEWATCHER -DPOCO_NO_AUTOMATIC_LIBS)

set (ATOMIC_LINK_LIBRARIES ${ATOMIC_LINK_LIBRARIES} LibCpuId SQLite)
