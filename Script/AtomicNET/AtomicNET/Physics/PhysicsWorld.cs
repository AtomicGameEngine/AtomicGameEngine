
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsRaycastResult
    {
        public Vector3 Position;
        public Vector3 Normal;
        public float Distance;
        public float HitFraction;

        IntPtr bodyPtr;
        public RigidBody Body => NativeCore.WrapNative<RigidBody>(bodyPtr);
    }

    public partial class PhysicsWorld : Component
    {
        public void RaycastSingle(ref PhysicsRaycastResult result, Ray ray, float maxDistance, uint collisionMask = uint.MaxValue)
        {
            csi_Atomic_PhysicsWorld_RaycastSingle(nativeInstance, ref ray, maxDistance, collisionMask, out result);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_PhysicsWorld_RaycastSingle(IntPtr self, ref Ray ray, float maxDistance, uint collisionMask, out PhysicsRaycastResult result);

    }

}
