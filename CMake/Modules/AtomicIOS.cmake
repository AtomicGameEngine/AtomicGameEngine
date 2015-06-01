include (BundleUtilities)

set (JAVASCRIPT_BINDINGS_PLATFORM "IOS")

add_definitions (-DIOS -DATOMIC_PLATFORM_IOS -DATOMIC_OPENGL -DKNET_UNIX)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

set (CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_INCLUDING_64_BIT))
set (CMAKE_XCODE_EFFECTIVE_PLATFORMS -iphoneos -iphonesimulator)

# Set Base SDK to "Latest iOS"
set (CMAKE_OSX_SYSROOT iphoneos)

# Obtain iOS sysroot path
execute_process (COMMAND xcodebuild -version -sdk ${CMAKE_OSX_SYSROOT} Path OUTPUT_VARIABLE IOS_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

set (CMAKE_FIND_ROOT_PATH ${IOS_SYSROOT})

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioToolbox -framework CoreAudio -framework CoreGraphics -framework Foundation -framework OpenGLES -framework QuartzCore -framework UIKit")
