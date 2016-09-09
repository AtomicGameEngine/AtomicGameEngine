@echo OFF
setlocal enabledelayedexpansion

set ATOMICEDITOR_BUILD_CMD=Build\Windows\node\node.exe Build\node_modules\jake\bin\cli.js -f ./Build/Scripts/Bootstrap.js build:atomiceditor

for %%a in (%*) do (
  if /I "%%a"=="--with-android" (set ATOMICEDITOR_ANDROID=YES)
)

:: If we're building in android support, make sure ANDROID_NDK is defined
if "%ATOMICEDITOR_ANDROID%" == "YES" (

    if "%ANDROID_NDK%" == "" (
        @echo:
        echo ANDROID_NDK not set, exiting
        @echo:
        exit /B
    )

    @echo:
    echo Building Atomic Editor with Android support
    @echo:
    set ATOMICEDITOR_BUILD_CMD=!ATOMICEDITOR_BUILD_CMD!["android"]
)

@echo:
@echo:
ECHO Building Atomic Editor, this process will take a few minutes
@echo:
@echo:
PAUSE
%ATOMICEDITOR_BUILD_CMD%
@echo:
@echo:
PAUSE

endlocal
