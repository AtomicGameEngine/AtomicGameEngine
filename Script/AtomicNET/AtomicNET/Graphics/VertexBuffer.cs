using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class VertexBuffer: AObject
    {

        public IntPtr Lock(uint start, uint count, bool discard = false)
        {
            return csb_Atomic_VertexBuffer_Lock(this.nativeInstance, start, count, discard);
        }


        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_VertexBuffer_Lock(IntPtr self, uint start, uint count, bool discard);

    };

}

