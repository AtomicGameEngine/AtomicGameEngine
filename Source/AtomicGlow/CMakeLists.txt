include_directories ( ${CMAKE_SOURCE_DIR}/Source/ThirdParty )

#FIX ME FOR EMBREE SUBMODULE

include_directories ( /usr/local/Cellar/tbb/4.4-20160128/include /usr/local/Cellar/embree/2.7.0.x86_64.macosx/include )
link_directories ( /usr/local/Cellar/tbb/4.4-20160128/lib /usr/local/Cellar/embree/2.7.0.x86_64.macosx/lib )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_executable(AtomicGlow ${SOURCE_FILES})

target_link_libraries(AtomicGlow ToolCore AtomicApp AtomicJS AtomicNETScript Poco ${ATOMIC_LINK_LIBRARIES} embree tbb)
