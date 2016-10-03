using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Controls
    {
        // If null, we own the unmanaged value
        // This is not-null when this is created from a pointer to another object, so we use this to keep the other object alive.
        Connection connection;
        internal IntPtr handle;

        internal Controls(Connection connection, IntPtr handle)
        {
            this.connection = connection;
            this.handle = handle;
        }

        public Controls()
        {
            handle = csi_Atomic_Controls_Create();
            connection = null;
        }

        static void ReleaseControl(IntPtr h)
        {
            csi_Atomic_Controls_Destroy(h);
        }

        ~Controls()
        {
            if (connection == null)
            {
                ReleaseControl(handle);
                handle = IntPtr.Zero;
            }
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static void csi_Atomic_Controls_Destroy(IntPtr handle);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static IntPtr csi_Atomic_Controls_Create();

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static uint csi_Atomic_Controls_GetButtons(IntPtr handle);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static void csi_Atomic_Controls_SetButtons(IntPtr handle, uint value);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static float csi_Atomic_Controls_GetYaw(IntPtr handle);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static void csi_Atomic_Controls_SetYaw(IntPtr handle, float yaw);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static float csi_Atomic_Controls_GetPitch(IntPtr handle);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        extern static void csi_Atomic_Controls_SetPitch(IntPtr handle, float yaw);

        public uint Buttons
        {
            get { return csi_Atomic_Controls_GetButtons(handle); }
            set { csi_Atomic_Controls_SetButtons(handle, value); }
        }

        public float Yaw
        {
            get { return csi_Atomic_Controls_GetYaw(handle); }
            set { csi_Atomic_Controls_SetYaw(handle, value); }
        }

        public float Pitch
        {
            get { return csi_Atomic_Controls_GetPitch(handle); }
            set { csi_Atomic_Controls_SetPitch(handle, value); }
        }

        public bool IsPressed(uint button, ref Controls previousControls)
        {
            return ((Buttons & button) != 0) && ((previousControls.Buttons & button) != 0);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Controls_Reset(IntPtr handle);

        public void Reset()
        {
            csi_Atomic_Controls_Reset(handle);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Controls_Set(IntPtr handle, uint buttons, bool down);

        public void Set(uint buttons, bool down)
        {
            csi_Atomic_Controls_Set(handle, buttons, down);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool csi_Atomic_Controls_IsDown(IntPtr handle, uint button);

        public bool IsDown(uint button)
        {
            return csi_Atomic_Controls_IsDown(handle, button);
        }
    }
}
