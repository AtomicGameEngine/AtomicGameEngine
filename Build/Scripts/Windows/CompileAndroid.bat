SET PATH=%ANDROID_NDK%\prebuilt\windows\bin\;%PATH%
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../../../Build/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../../
%ANDROID_NDK%\prebuilt\windows\bin\make.exe -j4
