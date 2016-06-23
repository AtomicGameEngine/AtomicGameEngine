@ECHO OFF
@echo:
@echo:
ECHO Generating Visual Studio Solution (64 bit)
@echo:
@echo:
call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
cmake -E make_directory "..\AtomicGameEngine-VS2015" && cmake -E chdir "..\AtomicGameEngine-VS2015" cmake ../AtomicGameEngine -G "Visual Studio 14 2015 Win64"
@echo:
ECHO Solution created in ..\AtomicGameEngine-VS2015
@echo:
