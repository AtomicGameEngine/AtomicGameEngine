using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial interface AbstractFile : Deserializer, Serializer
    {
        IntPtr NativeInstance { get; }
    }

}
