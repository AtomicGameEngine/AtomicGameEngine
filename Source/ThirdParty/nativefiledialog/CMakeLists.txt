
set (SOURCE_FILES nfd_common.c nfd.h)

if (APPLE)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_cocoa.mm )
endif( APPLE)

if (MSVC)
    set (SOURCE_FILES ${SOURCE_FILES} nfd_win.cpp )
endif( MSVC)


add_library(nativefiledialog ${SOURCE_FILES})
