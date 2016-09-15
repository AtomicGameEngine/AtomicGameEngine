
include(GetGitRevisionDescription)

get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/AtomicGitSHA.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/AtomicGitSHA.cpp" @ONLY)

set (SOURCE_FILES AtomicBuildInfo.cpp AtomicBuildInfo.h AtomicGitSHA.cpp AtomicGitSHA.h)

add_library(AtomicBuildInfo ${SOURCE_FILES})
