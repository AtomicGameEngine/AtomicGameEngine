using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public interface Deserializer
    {
        IntPtr NativeInstance { get; }
    }


}