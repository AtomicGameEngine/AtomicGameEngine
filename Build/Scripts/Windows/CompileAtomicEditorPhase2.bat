@echo OFF

call %0\..\SetupVSEnvironment.bat %2

if not defined ATOMIC_CMAKE_GENERATOR (
  @echo Problem setting up %2 compilation environment
  exit /b 1
)

msbuild /m Atomic.sln /t:AtomicEditor /t:AtomicPlayer /p:Configuration=%1 /p:Platform=x64
