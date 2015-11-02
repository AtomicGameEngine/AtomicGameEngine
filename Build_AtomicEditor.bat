@ECHO OFF
@echo:
@echo:
ECHO Building Atomic Editor, this process will take a few minutes
@echo:
@echo:
PAUSE
Build\Windows\node\node.exe Build\node_modules\jake\bin\cli.js -f ./Build/Scripts/Bootstrap.js build:atomiceditor
@echo:
@echo:
PAUSE
