call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
cmake ..\\..\\..\\ -DATOMIC_DEV_BUILD=0 -G "Visual Studio 14 2015 Win64"
msbuild /m /p:Configuration=Release /p:Platform=x64 Source\AtomicTool\GenerateScriptBindings.vcxproj
msbuild /m Atomic.sln /t:AtomicEditor /t:AtomicPlayer /p:Configuration=Release /p:Platform=x64
