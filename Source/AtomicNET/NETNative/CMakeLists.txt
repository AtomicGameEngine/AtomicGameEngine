include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/rapidjson/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/kNet/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/FreeType/include
                    ${CMAKE_SOURCE_DIR}/Source/ThirdParty/Box2D)

set (CSATOMICDIR "${CMAKE_SOURCE_DIR}/Artifacts/Build/Source/Generated/${JAVASCRIPT_BINDINGS_PLATFORM}/CSharp/Packages/")

file (GLOB CSHARP_BINDINGS_SOURCE ${CSATOMICDIR}/Atomic/Native/*.cpp ${CSATOMICDIR}/Atomic/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETNative/Native/*.cpp ${CSATOMICDIR}/AtomicNETNative/Native/*.h
                                  ${CSATOMICDIR}/AtomicNETScript/Native/*.cpp ${CSATOMICDIR}/AtomicNETScript/Native/*.h
                                  ${CSATOMICDIR}/AtomicApp/Native/*.cpp ${CSATOMICDIR}/AtomicApp/Native/*.h
                                  ${CSATOMICDIR}/AtomicPlayer/Native/*.cpp ${CSATOMICDIR}/AtomicPlayer/Native/*.h )

file (GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(AtomicNETNative SHARED ${SOURCE_FILES}  ${CSHARP_BINDINGS_SOURCE})

add_dependencies(AtomicNETNative AtomicToolCheckScripts)

target_link_libraries(AtomicNETNative AtomicNETScriptBindings AtomicNETScript AtomicJS AtomicPlayerLib AtomicPlayerJS AtomicApp ${ATOMIC_LINK_LIBRARIES})

if (APPLE)

    if (NOT IOS)

        target_link_libraries( AtomicNETNative "-stdlib=libc++ -framework AudioUnit -framework Carbon -framework Cocoa -framework CoreAudio -framework CoreVideo -framework ForceFeedback -framework IOKit -framework OpenGL -framework CoreServices -framework Security")

    endif()

endif()

add_custom_command( TARGET AtomicNETNative POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" ARGS -E make_directory \"${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>\"
                    COMMAND "${CMAKE_COMMAND}" ARGS -E copy_if_different \"$<TARGET_FILE:AtomicNETNative>\" \"${CMAKE_SOURCE_DIR}/Artifacts/AtomicNET/$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>\" )
