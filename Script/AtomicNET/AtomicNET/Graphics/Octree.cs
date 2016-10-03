
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public struct RayQueryResult
    {
        public Vector3 Position;
        public Vector3 Normal;
        public Vector2 TextureUV;
        public float Distance;

        IntPtr drawablePtr;
        IntPtr nodePtr;

        // Optimize
        public Drawable Drawable => NativeCore.WrapNative<Drawable>(drawablePtr);
        public Node Node => NativeCore.WrapNative<Node>(nodePtr);

        public uint SubObject;

    }

    public class RayOctreeQuery
    {

        /// Construct with ray and query parameters.
        public RayOctreeQuery(Ray ray, RayQueryLevel level = RayQueryLevel.RAY_TRIANGLE, float maxDistance = Single.PositiveInfinity, uint drawableFlags = Constants.DRAWABLE_ANY, uint viewMask = Constants.DEFAULT_VIEWMASK)
        {
            this.Ray = ray;
            this.Level = level;
            this.MaxDistance = maxDistance;
            this.DrawableFlags = drawableFlags;
            this.ViewMask = viewMask;
        }

        public List<RayQueryResult> Results = new List<RayQueryResult>();

        /// Ray.
        public Ray Ray;

        /// Drawable flags to include.
        public uint DrawableFlags;
        /// Drawable layers to include.
        public uint ViewMask;

        /// Maximum ray distance.
        public float MaxDistance;

        /// Raycast detail level.
        public RayQueryLevel Level;

    }

    public partial class Octree : Component
    {

        /// Return the closest drawable object by a ray query.
        public void RaycastSingle(RayOctreeQuery query)
        {
            Raycast(query, true);
        }

        public void Raycast(RayOctreeQuery query, bool single = false)
        {

            query.Results.Clear();

            if (query.Ray == null)
                return;

            int count;
            IntPtr resultVector;

            var ptr = csi_Atomic_Octree_Raycast(nativeInstance, ref query.Ray, ref query.Level, query.MaxDistance, query.DrawableFlags, query.ViewMask, single, out resultVector, out count);

            if (ptr == IntPtr.Zero)
            {
                return;
            }
                
            int structSize = Marshal.SizeOf(typeof(RayQueryResult));

            for (int i = 0; i < count; i++)
            {
                IntPtr data = new IntPtr(ptr.ToInt64() + structSize * i);
                RayQueryResult item = (RayQueryResult)Marshal.PtrToStructure(data, typeof(RayQueryResult));
                query.Results.Add(item);
            }

            csi_Atomic_Octree_Raycast_FreeResult(resultVector);

        }

        // Any resultVector returned here, must be freed with csi_Atomic_Octree_Raycast_FreeResult
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_Octree_Raycast(IntPtr self, ref Ray ray, ref RayQueryLevel level, float maxDistance, uint drawableFlags, uint viewMask, bool single, out IntPtr resultVector, out int count);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_Octree_Raycast_FreeResult(IntPtr resultVector);

    };

}

