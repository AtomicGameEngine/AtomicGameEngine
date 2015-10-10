
mcs /out:/Users/josh/Desktop/OSX.x64.Debug/AtomicNETBootstrap.dll /nostdlib /noconfig /t:library /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Collections.Concurrent.dll \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETBootstrap/*.cs

mcs /out:/Users/josh/Desktop/AtomicNETEngine.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/Source/Generated/MACOSX/CSharp/Packages/Atomic/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/Source/Generated/MACOSX/CSharp/Packages/AtomicNET/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/Source/Generated/MACOSX/CSharp/Packages/AtomicPlayer/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETEngine/*.cs

mcs /unsafe /out:/Users/josh/Desktop/AtomicNETTools.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Linq.dll /r:System.Reflection.Primitives.dll /r:System.Reflection.Metadata.dll \
/r:System.Collections.dll /r:System.Collections.Immutable.dll /r:/Users/josh/Desktop/AtomicNETEngine.dll \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETTools/*.cs

mcs /out:/Users/josh/Dev/atomic/AtomicExamples/NET/Basic3D/Resources/Assemblies/AtomicNETTest.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:/Users/josh/Desktop/AtomicNETEngine.dll \
/Users/josh/Dev/atomic/AtomicGameEngine/Build/AtomicNETTest/MyClass.cs
