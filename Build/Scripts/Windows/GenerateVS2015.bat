call "%VS140COMNTOOLS%..\..\VC\bin\amd64\vcvars64.bat"
cmake %1 -DATOMIC_DEV_BUILD=%2 -DATOMIC_OPENGL=%3 -DATOMIC_D3D11=%4 -G "Visual Studio 14 2015 Win64"

