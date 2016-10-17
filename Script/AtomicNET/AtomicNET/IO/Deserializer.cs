using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial interface Deserializer
    {
        IntPtr NativeInstance { get; }
    }


}