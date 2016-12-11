

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Material : Resource
    {

        public void SetShaderParameter(string name, ScriptVariant variant)
        {
            csi_Atomic_Material_SetShaderParameter(nativeInstance, name, variant.nativeInstance);
        }
      
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Material_SetShaderParameter(IntPtr self, string name, IntPtr variant);

    };

}




