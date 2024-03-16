@echo OFF

call %0\..\SetupVSEnvironment.bat %1

if not defined RENGINE_CMAKE_GENERATOR (
  @echo Problem setting up %1 compilation environment
  exit
)

@echo OFF

set RENGINE_CMAKE=cmake
set RENGINE_ROOT=%~2
set RENGINE_SOLUTION_PATH=%~3
set RENGINE_CMAKE_FLAGS="%4"

%RENGINE_CMAKE% -E make_directory "%RENGINE_SOLUTION_PATH%"
%RENGINE_CMAKE% -E chdir "%RENGINE_SOLUTION_PATH%" %RENGINE_CMAKE% "%RENGINE_ROOT%" %RENGINE_CMAKE_FLAGS% -G %RENGINE_CMAKE_GENERATOR%
