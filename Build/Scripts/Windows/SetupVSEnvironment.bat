@echo OFF

echo Setup VS Environment
set RENGINE_VSWHERE=%~dp0..\..\Windows\vswhere.exe


if "%1"=="VS2022" (
  goto setup_vs_2022
)

if "%1"=="VS2019" (
  goto setup_vs_2019
)

if "%1"=="VS2017" (
  goto setup_vs_2017
) 

goto setup_unsupported

:setup_vs_2022
  for /f "tokens=*" %%i in ('%RENGINE_VSWHERE% -version 17.0 -property installationPath') do set "RENGINE_VS_PATH=%%i"
  set RENGINE_CMAKE_GENERATOR="Visual Studio 17 2022"
  goto setup_vs

:setup_vs_2019
  for /f "tokens=*" %%i in ('%RENGINE_VSWHERE% -version 16.0 -property installationPath') do set "RENGINE_VS_PATH=%%i"
  set RENGINE_CMAKE_GENERATOR="Visual Studio 16 2019"
  goto setup_vs

:setup_vs_2017
  for /f "tokens=*" %%i in ('%RENGINE_VSWHERE% -version 15.0 -property installationPath') do set "RENGINE_VS_PATH=%%i"
  set RENGINE_CMAKE_GENERATOR="Visual Studio 15 2017 Win64"
  goto setup_vs

:setup_vs
  call "%RENGINE_VS_PATH%\Common7\Tools\VsDevCmd.bat"
  goto end

:setup_unsupported
  echo Unsupported Visual Studio Version. Please Install the Latest Version of Visual Studio!
  goto end

:end
  echo Finish VS Setup Environment