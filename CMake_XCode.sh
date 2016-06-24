#!/usr/bin/env bash
SOURCE=$(cd ${0%/*}; pwd)
cmake -E make_directory ../AtomicGameEngine-XCode && cmake -E chdir ../AtomicGameEngine-XCode cmake "$SOURCE" -G Xcode
echo "XCode project written to ../AtomicGameEngine-XCode"
