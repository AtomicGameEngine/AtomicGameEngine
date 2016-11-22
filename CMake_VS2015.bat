@ECHO OFF
@echo:
@echo:
SET ARG=%1
@echo:
@echo:
ECHO Generating Visual Studio Solution (64 bit)
@echo:
SET BUILDMODE="d3d9"
IF DEFINED ARG (
    IF "%ARG%"=="--opengl" (
    	SET BUILDMODE="opengl"
		ECHO OpenGL configuration selected.
	)
	IF "%ARG%"=="/opengl" (
    	SET BUILDMODE="opengl"
		ECHO OpenGL configuration selected.
	)
    IF "%ARG%"=="--d3d11" (
    	SET BUILDMODE="d3d11"
		ECHO Direct3D 11 configuration selected.
	)
    IF "%ARG%"=="/d3d11" (
    	SET BUILDMODE="d3d11"
		ECHO Direct3D 11 configuration selected.
	)
)
IF %BUILDMODE%=="d3d9" (
  ECHO Direct3D 9 configuration selected by default (Use /opengl or /d3d11 switches to specify OpenGL or DirectX 11)

)
@echo: 
call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
:: remove any generated sources
if exist "%~p0Artifacts\Build\Source\Generated\" rd /q /s "%~p0Artifacts\Build\Source\Generated\"
:: get current foldername
set "cdir=%~dp0"
:: for loop requires removing trailing backslash from %~dp0 output
set "cdir=%cdir:~0,-1%"
for %%i IN ("%cdir%") do set "foldername=%%~nxi"
:: run cmake
IF %BUILDMODE%=="d3d9" (
 cmake -E make_directory "..\%foldername%-VS2015" && cmake -E chdir "..\%foldername%-VS2015" cmake %~dp0 -G "Visual Studio 14 2015 Win64"
 @echo:
 ECHO Solution created in ..\%foldername%-VS2015
)
IF %BUILDMODE%=="d3d11" (
 cmake -E make_directory "..\%foldername%-VS2015-d3d11" && cmake -E chdir "..\%foldername%-VS2015-d3d11" cmake %~dp0 -G "Visual Studio 14 2015 Win64" -DATOMIC_D3D11=ON
 @echo:
 ECHO Solution created in ..\%foldername%-VS2015-d3d11
)
IF %BUILDMODE%=="opengl" (
 cmake -E make_directory "..\%foldername%-VS2015-opengl" && cmake -E chdir "..\%foldername%-VS2015-opengl" cmake %~dp0 -G "Visual Studio 14 2015 Win64" -DATOMIC_OPENGL=ON
 @echo:
 ECHO Solution created in ..\%foldername%-VS2015-opengl
)
@echo: