SET PATH=%ANDROID_NDK%\prebuilt\windows-x86_64\bin\;%ANDROID_NDK%\prebuilt\windows\bin\;%PATH%
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../../../Build/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=%1 ../../../
make.exe -j4
