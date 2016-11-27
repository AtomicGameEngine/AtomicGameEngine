@ECHO OFF
Build\Windows\node\node.exe ./Build/Scripts/Bootstrap.js --task=build:genvs2015 %*
