

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class ScriptVariantMap : RefCounted
    {
        public string this[string key]
        {
            get
            {
                return GetString(key);
            }
            set
            {
                SetString(key, value);
            }
        }

        public IntPtr GetVoidPtr(string key)
        {
            return csb_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(nativeInstance, key);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(IntPtr self, string key);

    }
}

