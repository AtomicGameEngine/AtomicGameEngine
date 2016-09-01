using System;
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

        public static implicit operator IntPtr(RefCounted refCounted)
        {
            if (refCounted == null)
                return IntPtr.Zero;
                
            return refCounted.nativeInstance;
        }

        public IntPtr nativeInstance = IntPtr.Zero;

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr csi_Atomic_RefCounted_GetClassID(IntPtr self);

    }


}
