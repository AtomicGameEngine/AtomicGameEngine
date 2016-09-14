

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Graphics : AObject
    {

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr csi_Atomic_Graphics_GetSDLWindow();

        /// <summary>
        /// Pointer to SDL window
        /// </summary>
        public IntPtr SDLWindow => csi_Atomic_Graphics_GetSDLWindow();

        // Shader Parameters

        public void SetShaderParameter(string param, Matrix3x4 matrix)
        {
            csi_Atomic_Graphics_SetShaderParameter_Matrix3x4(nativeInstance, param, ref matrix);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_Graphics_SetShaderParameter_Matrix3x4(IntPtr self, string param, ref Matrix3x4 matrix);


    };

}

