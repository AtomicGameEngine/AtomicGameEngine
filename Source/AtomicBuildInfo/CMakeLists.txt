
set (ATOMIC_GIT_SHA "Unversioned Build")

if (DEFINED ENV{ATOMIC_BUILD_SHA})
    set (ATOMIC_GIT_SHA $ENV{ATOMIC_BUILD_SHA})
endif ()

set (WRITE_REVISION "")

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/_GeneratedRevision.h OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/_GeneratedRevision.cache)
    set (WRITE_REVISION "TRUE")
else()

    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/_GeneratedRevision.cache ATOMIC_CACHE_SHA)

    if (NOT ATOMIC_GIT_SHA STREQUAL ATOMIC_CACHE_SHA)
        set (WRITE_REVISION "TRUE")
    endif()

endif()

if (${WRITE_REVISION})
    #message ("Writing Git Revision: ${WRITE_REVISION} : ATOMIC_GIT_SHA:${ATOMIC_GIT_SHA} ATOMIC_CACHE_SHA:${ATOMIC_CACHE_SHA}")
    file (WRITE ${CMAKE_CURRENT_SOURCE_DIR}/_GeneratedRevision.h "const char* atomicBuildRevision=\"${ATOMIC_GIT_SHA}\";\n")
    file (WRITE ${CMAKE_CURRENT_SOURCE_DIR}/_GeneratedRevision.cache "${ATOMIC_GIT_SHA}")
else()
    #message ("Using Cached Git Revision: ${WRITE_REVISION} : ATOMIC_GIT_SHA:${ATOMIC_GIT_SHA} ATOMIC_CACHE_SHA:${ATOMIC_CACHE_SHA}")
endif()


file (GLOB SOURCE_FILES *.cpp *.h)

add_library(AtomicBuildInfo ${SOURCE_FILES})
