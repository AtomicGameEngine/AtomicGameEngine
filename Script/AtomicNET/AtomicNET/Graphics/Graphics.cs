

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Graphics : AObject
    {

        // Shader Parameters

        public void SetShaderParameter(string param, Matrix3x4 matrix)
        {
            csb_Atomic_Graphics_SetShaderParameter_Matrix3x4(nativeInstance, param, ref matrix);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_Graphics_SetShaderParameter_Matrix3x4(IntPtr self, string param, ref Matrix3x4 matrix);

        public void SetShaderParameter(string param, Color color)
        {
            csb_Atomic_Graphics_SetShaderParameter_Color(nativeInstance, param, ref color);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_Graphics_SetShaderParameter_Color(IntPtr self, string param, ref Color color);


    };

}

