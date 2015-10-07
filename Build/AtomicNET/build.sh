
mcs /out:/Users/josh/Desktop/OSX.x64.Debug/AtomicNETBootstrap.dll /nostdlib /noconfig /t:library /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Collections.Concurrent.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNET/AtomicNETBootstrap/*.cs

mcs /out:/Users/josh/Desktop/AtomicNETRuntime.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/Source/Generated/MACOSX/CSharp/Packages/Atomic/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/Source/Generated/MACOSX/CSharp/Packages/AtomicPlayer/Managed/*.cs \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNET/AtomicNETRuntime/*.cs

#mcs /out:/Users/josh/Desktop/AtomicEditor.dll /t:library \
#/r:/Users/josh/Desktop/AtomicNETRuntime.dll \
#/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNETTest/AtomicEditor/AtomicEditor.cs

mcs /unsafe /out:/Users/josh/Desktop/AtomicEditor.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:System.Linq.dll /r:System.Reflection.Primitives.dll /r:System.Reflection.Metadata.dll /r:System.Collections.dll /r:System.Collections.Immutable.dll /r:/Users/josh/Desktop/AtomicNETRuntime.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNETTest/AtomicEditor/*.cs

mcs /out:/Users/josh/Desktop/AtomicNETTest.dll /nostdlib /noconfig /t:library /w:0 /lib:/Users/josh/Desktop/OSX.x64.Debug \
/r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll \
/r:/Users/josh/Desktop/AtomicNETRuntime.dll \
/Users/josh/Dev/atomic/AtomicGameEngineSharp/Build/AtomicNETTest/MyClass.cs
