

using System;
using System.Reflection;
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

            // TODO: warn on mismatch?
            if (value.GetType() == typeof(T) || value.GetType().GetTypeInfo().IsSubclassOf(typeof(T)))
                return (T) value;

            return null;            
        }

        public IntPtr GetVoidPtr(string key)
        {
            return csi_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(nativeInstance, key);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(IntPtr self, string key);

    }

}

