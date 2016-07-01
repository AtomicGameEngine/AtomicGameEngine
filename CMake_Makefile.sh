#!/usr/bin/env bash

PLATFORM=""
if [ "$(uname)" = "Darwin" ]; then
    PLATFORM="MacOS"
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    PLATFORM="Linux"
elif [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]; then
    PLATFORM="MSYS"
else
    exit "Unknown platform"
fi

SOURCE=$(cd ${0%/*}; pwd)
FOLDERNAME=${PWD##*/}
if [ -d "$SOURCE/Artifacts/Build/Source/Generated/" ]; then rm -Rf "$SOURCE/Artifacts/Build/Source/Generated/"; fi
cmake -E make_directory ../$FOLDERNAME-Build-$PLATFORM && cmake -E chdir ../$FOLDERNAME-Build-$PLATFORM cmake -DCMAKE_BUILD_TYPE=Debug "$SOURCE"
echo "makefile written to ../$FOLDERNAME-Build-$PLATFORM"
