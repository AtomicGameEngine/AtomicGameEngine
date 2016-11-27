@echo OFF

call %0\..\SetupVSEnvironment.bat %1

if not defined ATOMIC_CMAKE_GENERATOR (
  @echo Problem setting up %1 compilation environment
  exit
)

@echo OFF

set ATOMIC_ROOT=%~2
set ATOMIC_SOLUTION_PATH=%~3
set ATOMIC_CMAKE_FLAGS="%4"

%ATOMIC_CMAKE% -E make_directory "%ATOMIC_SOLUTION_PATH%"
%ATOMIC_CMAKE% -E chdir "%ATOMIC_SOLUTION_PATH%" %ATOMIC_CMAKE% "%ATOMIC_ROOT%" %ATOMIC_CMAKE_FLAGS% -G %ATOMIC_CMAKE_GENERATOR%
