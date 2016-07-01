call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
cmake %1 -DATOMIC_DEV_BUILD=1 -G "Visual Studio 14 2015 Win64"

