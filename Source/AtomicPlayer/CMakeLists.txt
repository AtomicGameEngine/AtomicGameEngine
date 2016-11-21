include_directories (${ATOMIC_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                     ${ATOMIC_SOURCE_DIR}/Source/ThirdParty )


file (GLOB SOURCE_FILES *.cpp *.h)

add_library(AtomicPlayerLib ${SOURCE_FILES})

add_subdirectory(Application)
