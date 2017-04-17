include(BundleUtilities)

set(JAVASCRIPT_BINDINGS_PLATFORM "IOS")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=gnu++0x")

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_XCODE_EFFECTIVE_PLATFORMS -iphoneos -iphonesimulator)
set(CMAKE_OSX_SYSROOT iphoneos)    # Set Base SDK to "Latest iOS"
set(CMAKE_OSX_DEPLOYMENT_TARGET "")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

set(CMAKE_OSX_ARCHITECTURES arm64)

# This is a CMake hack in order to make standard CMake check modules that use try_compile() internally work on iOS platform
# The injected "flags" are not compiler flags, they are actually CMake variables meant for another CMake subprocess that builds the source file being passed in the try_compile() command
# CAVEAT: these injected "flags" must always be kept at the end of the string variable, i.e. when adding more compiler flags later on then those new flags must be prepended in front of these flags instead
set(CMAKE_REQUIRED_FLAGS ";-DSmileyHack=byYaoWT;-DCMAKE_MACOSX_BUNDLE=1;-DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED=0")

# Obtain iOS sysroot path
execute_process(COMMAND xcodebuild -version -sdk ${CMAKE_OSX_SYSROOT} Path OUTPUT_VARIABLE IOS_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

message(${IOS_SYSROOT})

set(CMAKE_FIND_ROOT_PATH ${IOS_SYSROOT})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioToolbox -framework AVFoundation -framework CoreAudio -framework CoreGraphics -framework Foundation -framework OpenGLES -framework QuartzCore -framework UIKit -framework CoreMotion -framework GameController")
