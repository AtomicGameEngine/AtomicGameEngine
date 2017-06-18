using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsRaycastResult2D
    {
        public Vector2 Position;
        public Vector2 Normal;
        public float Distance;

        IntPtr bodyPtr;
        public RigidBody2D Body => bodyPtr == IntPtr.Zero ? null : NativeCore.WrapNative<RigidBody2D>(bodyPtr);
    }

    public partial class PhysicsWorld2D : Component
    {
        public void RaycastSingle(ref PhysicsRaycastResult2D result, Vector2 startPoint, Vector2 endPoint, uint collisionMask = uint.MaxValue)
        {
            csi_Atomic_PhysicsWorld2D_RaycastSingle(nativeInstance, ref startPoint, ref endPoint, collisionMask, out result);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_PhysicsWorld2D_RaycastSingle(IntPtr self, ref Vector2 origin, ref Vector2 direction, uint collisionMask, out PhysicsRaycastResult2D result);
    }
}
