
/usr/local/bin/mcs /out:../../../Artifacts/AtomicNET/TPA/AtomicNETBootstrap.dll /nostdlib /noconfig /t:library \
/lib:../../../Submodules/CoreCLR/MacOSX/Debug/x64/ \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Collections.dll /r:System.Collections.Concurrent.dll /r:System.Linq.dll \
AtomicNETBootstrap/*.cs

/usr/local/bin/mcs /out:../../../Artifacts/AtomicNET/AtomicNETEngine.dll /nostdlib /noconfig /t:library /w:0 \
/lib:../../../Submodules/CoreCLR/MacOSX/Debug/x64/ \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Linq.dll \
../../../Build/Source/Generated/MACOSX/CSharp/Packages/Atomic/Managed/*.cs \
../../../Build/Source/Generated/MACOSX/CSharp/Packages/AtomicNET/Managed/*.cs \
../../../Build/Source/Generated/MACOSX/CSharp/Packages/AtomicPlayer/Managed/*.cs \
AtomicNETEngine/*.cs

/usr/local/bin/mcs /unsafe /out:../../../Artifacts/AtomicNET/AtomicNETTools.dll /nostdlib /noconfig /t:library /w:0 \
/lib:../../../Submodules/CoreCLR/MacOSX/Debug/x64/ \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Linq.dll /r:System.Reflection.Primitives.dll /r:System.Reflection.Metadata.dll \
/r:System.Collections.dll /r:System.Collections.Immutable.dll \
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll \
AtomicNETTools/*.cs

/usr/local/bin/mcs /out:../../../Artifacts/AtomicNET/AtomicNETTestCompile.dll /nostdlib /noconfig /t:library /w:0 \
/lib:../../../Submodules/CoreCLR/MacOSX/Debug/x64/ \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:../../../Artifacts/AtomicNET/AtomicNETEngine.dll \
../AtomicNETTest/MyClass.cs
