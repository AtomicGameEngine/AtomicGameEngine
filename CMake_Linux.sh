#!/usr/bin/env bash
SOURCE=$(cd ${0%/*}; pwd)
cmake -E make_directory ../AtomicGameEngine-Linux && cmake -E chdir ../AtomicGameEngine-Linux cmake "$SOURCE"
echo "makefile written to ../AtomicGameEngine-Linux"
