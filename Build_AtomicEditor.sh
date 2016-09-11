#!/usr/bin/env sh

if [ "$(uname)" = "Darwin" ]; then
    ./Build/Mac/node/node ./Build/Scripts/Bootstrap.js buildeditor "$@"
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    ./Build/Linux/node/node ./Build/Scripts/Bootstrap.js buildeditor "$@"
elif [ "$(expr substr $(uname -s) 1 7)" = "MSYS_NT" ]; then
    ./Build/Windows/node/node.exe ./Build/Scripts/Bootstrap.js buildeditor "$@"
fi
