
csc /out:../../../Artifacts/AtomicNET/TPA/AtomicNETBootstrap.dll /nostdlib+ /noconfig /t:library ^
/lib:../../../Submodules/CoreCLR/Windows/x64/;../../../Submodules/CoreCLR/AnyCPU/TPA/ ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Collections.Concurrent.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Collections.dll ^
/r:../../../Submodules/CoreCLR/Windows/x64/mscorlib.dll ^
AtomicNETBootstrap/*.cs

csc /out:../../../Artifacts/AtomicNET/AtomicNETEngine.dll /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/x64/;../../../Submodules/CoreCLR/AnyCPU/TPA/ ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.IO.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.IO.FileSystem.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/Windows/x64/mscorlib.dll ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/Atomic/Managed/*.cs ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicNET/Managed/*.cs ^
../../../Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicPlayer/Managed/*.cs ^
AtomicNETEngine/*.cs

csc /unsafe /out:../../../Artifacts/AtomicNET/AtomicNETTools.dll /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/x64/;../../../Submodules/CoreCLR/AnyCPU/TPA/ ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.IO.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Linq.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Reflection.Primitives.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Reflection.Metadata.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Collections.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Collections.Immutable.dll ^
/r:../../../Submodules/CoreCLR/Windows/x64/mscorlib.dll ^
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll ^
AtomicNETTools/*.cs

csc /out:C:/Dev/atomic/AtomicExamples/AtomicNET/Basic3D/Resources/Assemblies/AtomicNETTest.dll ^
/debug+ /nostdlib+ /noconfig /t:library /w:0 ^
/lib:../../../Submodules/CoreCLR/Windows/x64/;../../../Submodules/CoreCLR/AnyCPU/TPA/ ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.Runtime.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.IO.dll ^
/r:../../../Submodules/CoreCLR/AnyCPU/TPA/System.IO.FileSystem.dll ^
/r:../../../Submodules/CoreCLR/Windows/x64/mscorlib.dll ^
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll ^
../AtomicNETTest/MyClass.cs
