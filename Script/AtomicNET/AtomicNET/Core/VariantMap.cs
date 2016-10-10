using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    internal static class VariantMap 
    {

        public static void SetBool(IntPtr vmap, string key, bool value)
        {
            csi_Atomic_VariantMap_SetBool(vmap, key, value);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_VariantMap_SetBool(IntPtr vmap, string key, bool value);

    }

}
