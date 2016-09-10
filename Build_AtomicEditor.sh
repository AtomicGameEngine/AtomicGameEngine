#!/usr/bin/env sh

buildcmd="build:atomiceditor"

for var in "$@"
do
    if [ $var == "--with-android" ]; then
        if [ "$ANDROID_NDK" == "" ]; then
            echo "\n\nANDROID_NDK environment variable not set, exiting\n\n"
            exit 1
        fi
        echo "\n\nBuilding Atomic Editor with Android support\n\n"
        buildcmd+="[android]"
    fi
done

if [ "$(uname)" = "Darwin" ]; then
    ./Build/Mac/node/node ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js $buildcmd
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    ./Build/Linux/node/node ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js $buildcmd
elif [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]; then
    ./Build/Windows/node/node.exe ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js $buildcmd
fi
