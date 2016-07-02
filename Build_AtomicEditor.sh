#!/usr/bin/env sh

if [ "$(uname)" = "Darwin" ]; then
    ./Build/Mac/node/node ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js build:atomiceditor
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    ./Build/Linux/node/node ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js build:atomiceditor
elif [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]; then
    ./Build/Windows/node/node.exe ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js build:atomiceditor
fi
