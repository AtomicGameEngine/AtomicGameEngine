using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public delegate void EventDelegate(uint eventType, ScriptVariantMap eventData);

    public delegate void SenderEventDelegate(AObject sender, uint eventType, ScriptVariantMap eventData);

    public partial class ScriptVariantMap
    {
        public void CopyVariantMap(IntPtr vm)
        {
            csi_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(nativeInstance, vm);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(IntPtr svm, IntPtr vm);

    }

}