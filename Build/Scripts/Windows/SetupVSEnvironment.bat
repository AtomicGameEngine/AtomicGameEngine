@echo OFF

:: locate VS2017, this is done first as inside the conditional was having problems
FOR /F "usebackq tokens=2,* skip=2" %%L IN (
    `reg query ""HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7"" /v 15.0 2^> nul`
) DO (SET ATOMIC_VS2017PATH=%%M)

if "%1"=="VS2017" (

  if defined ATOMIC_VS2017PATH (

    set ATOMIC_CMAKE="%ATOMIC_VS2017PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    set ATOMIC_CMAKE_GENERATOR="Visual Studio 15 2017 Win64"

    call "%ATOMIC_VS2017PATH%\Common7\Tools\VsDevCmd.bat"

  )

) else (

  if defined VS140COMNTOOLS (

    set ATOMIC_CMAKE=cmake
    set ATOMIC_CMAKE_GENERATOR="Visual Studio 14 2015 Win64"

    call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"

  )

)
