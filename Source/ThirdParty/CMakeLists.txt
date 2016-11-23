
add_subdirectory(Box2D)
add_subdirectory(Duktape)
add_subdirectory(FreeType)
add_subdirectory(JO)
add_subdirectory(LZ4)
add_subdirectory(PugiXml)
add_subdirectory(STB)
add_subdirectory(TurboBadger)
add_subdirectory(zlib)

include_directories_exported(
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/WebSocketPP/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/libcurl/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/kNet/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/FreeType/include
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/Box2D
    ${ATOMIC_SOURCE_DIR}/Source/ThirdParty/ASIO/include)

if (NOT ATOMIC_BUILD_2D)
    add_subdirectory(Bullet)
    add_subdirectory(StanHull)
endif()

add_subdirectory(SDL)

if (NOT EMSCRIPTEN)
    add_subdirectory(Recast)
    add_subdirectory(Civetweb)
    add_subdirectory(Detour)
    add_subdirectory(DetourCrowd)
    add_subdirectory(DetourTileCache)
    add_subdirectory(kNet)
if (NOT MSVC)
    add_subdirectory(openssl)
endif()
    add_subdirectory(libcurl)
endif()

if (NOT IOS AND NOT ANDROID AND NOT EMSCRIPTEN)
    add_subdirectory(Assimp)
    if (NOT LINUX)
        add_subdirectory(LibCpuId)
    endif()
    #add_subdirectory(SQLite)
    add_subdirectory(Poco)
    add_subdirectory(nativefiledialog)
    add_subdirectory(libsquish)
endif ()

if (LINUX OR APPLE AND NOT IOS)
    add_subdirectory(GLEW)
endif()

if (MSVC)
    add_subdirectory(MojoShader)
endif()
