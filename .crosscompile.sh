#!/usr/bin/env bash

cd /tmp/AtomicGameEngine-build
mkdir -p $CROSS_TRIPLE
cd $CROSS_TRIPLE

if [[ "$CROSS_TRIPLE" == "x86_64-apple-darwin14" ]];
then
    cmake /workdir -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/workdir/Build/CMake/Toolchains/CrossMacOS.cmake
elif [[ "$CROSS_TRIPLE" == "x86_64-linux-gnu" ]];
then
    cmake /workdir -DCMAKE_BUILD_TYPE=Release -DATOMIC_WEBVIEW=1
fi

cmake --build . -- -j$(nproc --all)
