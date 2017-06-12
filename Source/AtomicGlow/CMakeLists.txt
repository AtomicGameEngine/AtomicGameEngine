include_directories ( ${CMAKE_SOURCE_DIR}/Source/ThirdParty )
include_directories ( ${CMAKE_SOURCE_DIR}/Source/ThirdParty/embree/include )

add_definitions(-DEMBREE_STATIC_LIB=1)

file (GLOB GLOW_COMMON_SOURCE Common/*.cpp Common/*.h)
file (GLOB GLOW_KERNEL_SOURCE Kernel/*.cpp Kernel/*.h)
file (GLOB GLOW_ATLAS_SOURCE Atlas/*.cpp Atlas/*.h)
file (GLOB GLOW_SERVICE_SOURCE GlowService/*.cpp GlowService/*.h)

add_library(AtomicGlowLib ${GLOW_COMMON_SOURCE} ${GLOW_KERNEL_SOURCE} ${GLOW_ATLAS_SOURCE} ${GLOW_SERVICE_SOURCE} )

target_link_libraries(AtomicGlowLib ToolCore Poco embree)

target_compile_definitions(AtomicGlowLib PUBLIC -DATOMIC_GLOW=1)

add_dependencies(AtomicApp AtomicToolCheckScripts)

if (WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif(WIN32)

file (GLOB GLOW_APPLICATION_SOURCE GlowApplication/*.cpp GlowApplication/*.h)

add_executable(AtomicGlow ${GLOW_APPLICATION_SOURCE})

target_link_libraries(AtomicGlow AtomicApp AtomicJS AtomicNETScript AtomicGlowLib)

if (WIN32)
    target_link_libraries(AtomicGlow Iphlpapi Wldap32)
endif()


# Copy AtomicGlow to Artifacts
add_custom_command( TARGET AtomicGlow POST_BUILD
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicGlow\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E copy_if_different \"$<TARGET_FILE:AtomicGlow>\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Build/AtomicGlow/\"
                    COMMENT "Copying AtomicGlow to Build Artifacts" )

