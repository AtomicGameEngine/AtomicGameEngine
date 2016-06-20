using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class RefCounted
    {

        public RefCounted()
        {
        }

        protected RefCounted(IntPtr native)
        {
            nativeInstance = native;
        }

        public IntPtr nativeInstance = IntPtr.Zero;

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr csb_Atomic_RefCounted_GetClassID(IntPtr self);

    }


}