call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
msbuild /m Atomic.sln /t:AtomicEditor /t:AtomicPlayer /p:Configuration=%1 /p:Platform=x64
