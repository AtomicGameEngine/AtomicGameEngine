

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class ValueAnimation : Resource
    {
        public bool SetKeyFrame(float time, ScriptVariant variant)
        {
            return csi_Atomic_ValueAnimation_SetKeyFrame(nativeInstance, time, variant.nativeInstance);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool csi_Atomic_ValueAnimation_SetKeyFrame(IntPtr self, float time, IntPtr variant);

    }

}

