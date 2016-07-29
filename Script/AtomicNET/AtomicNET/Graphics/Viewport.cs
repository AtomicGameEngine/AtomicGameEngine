

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Viewport : AObject
    {

        public void SetRenderPath(RenderPath renderPath)
        {
            csb_Atomic_Viewport_SetRenderPath_RenderPath(nativeInstance, renderPath != null ? renderPath.nativeInstance : IntPtr.Zero);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_Viewport_SetRenderPath_RenderPath(IntPtr self, IntPtr renderPath);

    };

}

