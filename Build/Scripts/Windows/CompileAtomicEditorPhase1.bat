@echo OFF

call %0\..\SetupVSEnvironment.bat %2

if not defined ATOMIC_CMAKE_GENERATOR (
  @echo Problem setting up %2 compilation environment
  exit /b 1
)

set ATOMIC_CMAKE_FLAGS=%3
set ATOMIC_CMAKE_FLAGS=%ATOMIC_CMAKE_FLAGS:\=%
set ATOMIC_CMAKE_FLAGS=%ATOMIC_CMAKE_FLAGS:"=%

%ATOMIC_CMAKE% %0\..\..\..\..\ %ATOMIC_CMAKE_FLAGS% -G %ATOMIC_CMAKE_GENERATOR%

:: Note, we're building LibCpuId as it uses masm as getting XamlFactory load errors if delayed
msbuild /m Atomic.sln /t:LibCpuId /t:AtomicNETNative /p:Configuration=%1 /p:Platform=x64
