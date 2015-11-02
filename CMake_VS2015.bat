@ECHO OFF
@echo:
@echo:
ECHO Generating Visual Studio Solution, this process will take a few minutes
@echo:
@echo:
PAUSE
Build\Windows\node\node.exe Build\node_modules\jake\bin\cli.js -f ./Build/Scripts/Bootstrap.js build:genvs2015
@echo:
@echo:
PAUSE
