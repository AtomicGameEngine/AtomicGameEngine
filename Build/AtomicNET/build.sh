
mcs /out:/Users/josh/Desktop/OSX.x64.Debug/AtomicNETBootstrap.dll /nostdlib /noconfig /t:library /lib:/Users/josh/Desktop/OSX.x64.Debug /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNET/AtomicNETBootstrap/*.cs

mcs /out:/Users/josh/Desktop/AtomicNETRuntime.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/Source/Generated/MACOSX/CSharp/Packages/Atomic/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/Source/Generated/MACOSX/CSharp/Packages/AtomicPlayer/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNET/AtomicNETRuntime/*.cs
