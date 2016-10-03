
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class RenderPath : RefCounted
    {

        public void SetShaderParameter(string name, ScriptVariant variant)
        {
            csi_Atomic_RenderPath_SetShaderParameter(nativeInstance, name, variant.nativeInstance);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_RenderPath_SetShaderParameter(IntPtr self, string name, IntPtr variant);

        public ScriptVariant GetShaderParameter(string name)
        {
            ScriptVariant variant = new ScriptVariant();
            csi_Atomic_RenderPath_GetShaderParameter(nativeInstance, name, variant.nativeInstance);
            return variant;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_RenderPath_GetShaderParameter(IntPtr self, string name, IntPtr variant);


    };

}

