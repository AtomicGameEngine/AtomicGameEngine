
set (SOURCE_FILES nfd_common.c nfd.h)

if (APPLE)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_cocoa.mm )
endif( APPLE)

if (WIN32)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_win.cpp )
endif( WIN32)

if (LINUX)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_gtk.c )    
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})

endif(LINUX)


add_library(nativefiledialog ${SOURCE_FILES})
target_include_directories(nativefiledialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
