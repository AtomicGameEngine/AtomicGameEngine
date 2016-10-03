
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Camera : Component
    {
       
        /// Return ray corresponding to normalized screen coordinates (0 - 1), with origin on the near clip plane.
        public Ray GetScreenRay(float x, float y)
        {
            csi_Atomic_Camera_GetScreenRay(nativeInstance, x, y, ref GetScreenRayReturnValue);
            return GetScreenRayReturnValue;
        }

        public Plane ClipPlane
        {
            get
            {
                return GetClipPlane();
            }
            set
            {
                SetClipPlane(value);
            }
        }

        public Plane ReflectionPlane
        {
            get
            {
                return GetReflectionPlane();
            }
            set
            {
                SetReflectionPlane(value);
            }
        }

        public Plane GetClipPlane()
        {
            Plane plane = new Plane();
            csi_Atomic_Camera_GetClipPlane(nativeInstance, ref plane);
            return plane;
        }

        public Plane GetReflectionPlane()
        {
            Plane plane = new Plane();
            csi_Atomic_Camera_GetReflectionPlane(nativeInstance, ref plane);
            return plane;
        }

        public void SetClipPlane(Plane plane)
        {
            csi_Atomic_Camera_SetClipPlane(nativeInstance, ref plane);
        }

        public void SetReflectionPlane(Plane plane)
        {
            csi_Atomic_Camera_SetReflectionPlane(nativeInstance, ref plane);
        }

        private Ray GetScreenRayReturnValue = new Ray();
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Camera_GetScreenRay(IntPtr self, float x, float y, ref Ray retValue);

        
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Camera_GetClipPlane(IntPtr self, ref Plane retValue);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Camera_GetReflectionPlane(IntPtr self, ref Plane retValue);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Camera_SetClipPlane(IntPtr self, ref Plane retValue);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Camera_SetReflectionPlane(IntPtr self, ref Plane retValue);


    };

}

