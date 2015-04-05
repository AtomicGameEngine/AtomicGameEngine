
set (SOURCE_FILES nfd_common.c nfd.h)

if (APPLE)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_cocoa.mm )
endif( APPLE)

if (MSVC)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_win.cpp )
endif( MSVC)

if (LINUX)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_gtk.c )    
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})

endif(LINUX)


add_library(nativefiledialog ${SOURCE_FILES})
