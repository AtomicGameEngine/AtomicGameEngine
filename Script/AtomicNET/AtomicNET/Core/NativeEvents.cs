using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public struct CoreDelegates
    {
        [MarshalAs(UnmanagedType.FunctionPtr)]
        public EventDispatchDelegate eventDispatch;
    }


    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void EventDispatchDelegate(uint eventType, IntPtr eventData);

    public delegate void EventDelegate(uint eventType, ScriptVariantMap eventData);

    public partial class ScriptVariantMap
    {
        public void CopyVariantMap(IntPtr vm)
        {
            csb_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(nativeInstance, vm);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(IntPtr svm, IntPtr vm);

    }

}