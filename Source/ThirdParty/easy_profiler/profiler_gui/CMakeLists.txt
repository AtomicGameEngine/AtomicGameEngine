#set(CMAKE_PREFIX_PATH f:/qt/5.5/5.6/msvc2013_64/lib/cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ATOMIC BEGIN
# moved to easy profiler root CMake, to avoid duplicate checks
# find_package(Qt5Widgets)
# ATOMIC END

if (Qt5Widgets_FOUND)
    if (NOT("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") AND WIN32)
        set(APPLICATION_PLATFORM WIN32)
    endif ()
    add_executable(profiler_gui ${APPLICATION_PLATFORM}
        main.cpp
        blocks_graphics_view.h
        blocks_graphics_view.cpp
        blocks_tree_widget.h
        blocks_tree_widget.cpp
        descriptors_tree_widget.h
        descriptors_tree_widget.cpp
        easy_chronometer_item.h
        easy_chronometer_item.cpp
        easy_frame_rate_viewer.h
        easy_frame_rate_viewer.cpp
        easy_graphics_item.h
        easy_graphics_item.cpp
        easy_graphics_scrollbar.h
        easy_graphics_scrollbar.cpp
        easy_qtimer.h
        easy_qtimer.cpp
        globals.h
        globals.cpp
        globals_qobjects.cpp
        main_window.h
        main_window.cpp
        tree_widget_item.h
        tree_widget_item.cpp
        tree_widget_loader.h
        tree_widget_loader.cpp
        resources.qrc
        resources.rc
    )
    # ATOMIC BEGIN
    target_link_libraries(profiler_gui Qt5::Widgets)
    set_target_properties(profiler_gui PROPERTIES OUTPUT_NAME Profiler)
    set_property(TARGET profiler_gui PROPERTY RUNTIME_OUTPUT_DIRECTORY ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Profiler)
    if (TARGET easy_profiler_md)
        target_link_libraries(profiler_gui easy_profiler_md)
    else ()
        target_link_libraries(profiler_gui easy_profiler)
    endif ()
    deploy_qt_dlls (profiler_gui)
    # ATOMIC END
    if (WIN32)
        target_compile_definitions(profiler_gui PRIVATE -D_WIN32_WINNT=0x0600)
    endif ()
    if (MINGW)
        target_compile_definitions(profiler_gui PRIVATE -DSTRSAFE_NO_DEPRECATE)
    endif ()
else ()
    # ATOMIC BEGIN
    message(WARNING
        "Qt5 not found! Profiler GUI application will not be built!\n"
        "You may have to specify path to Qt SDK in CMAKE_PREFIX_PATH variable."
    )
    # ATOMIC END
endif ()

