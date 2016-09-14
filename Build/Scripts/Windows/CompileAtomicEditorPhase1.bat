call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
cmake ..\\..\\..\\ -DATOMIC_DEV_BUILD=0 -G "Visual Studio 14 2015 Win64"
:: Note, we're building LibCpuId as it uses masm as getting XamlFactory load errors if delayed
msbuild /m Atomic.sln /t:LibCpuId /t:AtomicNETNative /p:Configuration=%1 /p:Platform=x64
