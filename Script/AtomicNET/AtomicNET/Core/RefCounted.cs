using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [ComVisible(true)]
    public partial class RefCounted
    {

        public RefCounted()
        {
        }

        protected RefCounted(IntPtr native)
        {
            nativeInstance = native;
        }

        // This method may be called multiple times, called on instance after it is either registered as a new native created in C# (InstantiationType == InstantiationType.INSTANTIATION_NET)
        // or a native which has been wrapped ((InstantiationType != InstantiationType.INSTANTIATION_NET)
        // Note that RefCounted that get GC'd from script, can still live in native code, and get rewrapped 
        internal virtual void PostNativeUpdate()
        {

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
