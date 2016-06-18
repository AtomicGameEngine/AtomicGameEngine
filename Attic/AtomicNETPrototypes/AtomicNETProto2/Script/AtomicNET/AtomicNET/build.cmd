
csc /out:../../../Artifacts/AtomicNET/TPA/AtomicNETBootstrap.dll /nostdlib+ /noconfig /t:library ^
/lib:../../../Submodules/CoreCLR/Windows/Release/x64/ ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Collections.Concurrent.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Collections.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/mscorlib.ni.dll ^
AtomicNETBootstrap/*.cs

csc /out:../../../Artifacts/AtomicNET/AtomicNETEngine.dll /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/Release/x64/ ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.IO.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.IO.FileSystem.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/mscorlib.ni.dll ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/Atomic/Managed/*.cs ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicNET/Managed/*.cs ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicPlayer/Managed/*.cs ^
AtomicNETEngine/*.cs

csc /unsafe /out:../../../Artifacts/AtomicNET/AtomicNETTools.dll /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/Release/x64/ ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.IO.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Reflection.Primitives.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Reflection.Metadata.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Collections.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Collections.Immutable.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/mscorlib.ni.dll ^
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll ^
AtomicNETTools/*.cs

csc /out:../../../Artifacts/AtomicNET/AtomicNETTestCompile.dll ^
/debug+ /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/Release/x64/ ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.IO.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/System.IO.FileSystem.dll ^
/r:../../../Submodules/CoreCLR/Windows/Release/x64/mscorlib.ni.dll ^
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll ^
../AtomicNETTest/MyClass.cs
