call "%VS140COMNTOOLS%..\..\VC\bin\vcvars32.bat"
cmake ..\\..\\  -DATOMIC_DEV_BUILD=1 -G "Visual Studio 14 2015"
msbuild /m Atomic.sln /t:AtomicTool /p:Configuration=Release /p:Platform=Win32
