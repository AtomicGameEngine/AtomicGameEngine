
csc /out:C:/Dev/coreclr/x64/AtomicNETBootstrap.dll /nostdlib+ /noconfig /t:library /lib:C:/Dev/coreclr/x64/ ^
/r:C:/Dev/coreclr/x64/System.Collections.Concurrent.dll /r:C:/Dev/coreclr/x64/System.Runtime.dll ^
/r:C:/Dev/coreclr/x64/mscorlib.dll ^
C:/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETBootstrap/*.cs

csc /out:C:/Users/Josh/Desktop/AtomicNETEngine.dll /nostdlib+ /noconfig /t:library /w:0 /lib:C:/Dev/coreclr/x64/ ^
/r:C:/Dev/coreclr/x64/System.Runtime.dll /r:C:/Dev/coreclr/x64/System.IO.dll /r:C:/Dev/coreclr/x64/System.IO.FileSystem.dll ^
/r:C:/Dev/coreclr/x64/mscorlib.dll ^
/r:C:/Dev/coreclr/x64/System.Linq.dll ^
C:/Dev/atomic/AtomicGameEngine/Build/Source/Generated/WINDOWS/CSharp/Packages/Atomic/Managed/*.cs ^
C:/Dev/atomic/AtomicGameEngine/Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicNET/Managed/*.cs ^
C:/Dev/atomic/AtomicGameEngine/Build/Source/Generated/WINDOWS/CSharp/Packages/AtomicPlayer/Managed/*.cs ^
C:/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETEngine/*.cs

csc /unsafe /out:C:/Users/Josh/Desktop/AtomicNETTools.dll /nostdlib+ /noconfig /t:library /w:0 /lib:C:/Dev/coreclr/x64/ ^
/r:C:/Dev/coreclr/x64/System.Runtime.dll /r:C:/Dev/coreclr/x64/System.IO.dll ^
/r:C:/Dev/coreclr/x64/mscorlib.dll ^
/r:C:/Dev/coreclr/x64/System.Linq.dll /r:C:/Dev/coreclr/x64/System.Reflection.Primitives.dll /r:C:/Dev/coreclr/x64/System.Reflection.Metadata.dll ^
/r:C:/Dev/coreclr/x64/System.Collections.dll /r:C:/Dev/coreclr/x64/System.Collections.Immutable.dll /r:C:/Dev/coreclr/x64/AtomicNETEngine.dll ^
C:/Dev/atomic/AtomicGameEngine/Build/AtomicNET/AtomicNETTools/*.cs

csc /out:C:/Users/Josh/Desktop/AtomicNETTest.dll /nostdlib+ /noconfig /t:library /w:0 /lib:C:/Dev/coreclr/x64/ ^
/r:C:/Dev/coreclr/x64/System.Runtime.dll /r:C:/Dev/coreclr/x64/System.IO.dll /r:C:/Dev/coreclr/x64/System.IO.FileSystem.dll ^
/r:C:/Dev/coreclr/x64/mscorlib.dll ^
/r:C:/Users/Josh/Desktop/AtomicNETEngine.dll ^
C:/Dev/atomic/AtomicGameEngine/Build/AtomicNETTest/MyClass.cs
