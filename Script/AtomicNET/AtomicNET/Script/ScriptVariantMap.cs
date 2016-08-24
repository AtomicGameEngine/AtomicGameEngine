

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

        public T GetPtr<T>(string key) where T : RefCounted
        {
            var value = GetPtr(key);

            if (value == null)
                return null;

            // TODO: allow derived classes, throw error
            if (value.GetType().Name != typeof(T).Name)
                return null;

            return (T) value;            
        }

        public IntPtr GetVoidPtr(string key)
        {
            return csi_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(nativeInstance, key);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(IntPtr self, string key);

    }
}

