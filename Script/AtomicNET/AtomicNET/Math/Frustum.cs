using System;
using System.Runtime.InteropServices;
using System.Text;

namespace AtomicEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct Frustum
    {
        /// <summary>
        /// Define with projection parameters and a transform.
        /// </summary>
        public void Define(float fov, float aspectRatio, float zoom, float nearZ, float farZ, Vector3 position, Quaternion rotation)
        {
            csi_Atomic_Frustum_Define(ref this, fov, aspectRatio, zoom, nearZ, farZ, ref position, ref rotation);
        }

        /// <summary>
        /// Define with projection parameters
        /// </summary>
        public void Define(float fov, float aspectRatio, float zoom, float nearZ, float farZ)
        {
            var zero = Vector3.Zero;
            var identity = Quaternion.Identity;

            csi_Atomic_Frustum_Define(ref this, fov, aspectRatio, zoom, nearZ, farZ, ref zero, ref identity);
        }

        /// <summary>
        /// Define with orthographic projection parameters and a transform.
        /// </summary>
        public void DefineOrtho(float orthoSize, float aspectRatio, float zoom, float nearZ, float farZ, Vector3 position, Quaternion rotation)
        {
            csi_Atomic_Frustum_DefineOrtho(ref this, orthoSize, aspectRatio, zoom, nearZ, farZ, ref position, ref rotation);
        }

        /// <summary>
        /// Define with orthographic projection parameters.
        /// </summary>
        public void DefineOrtho(float orthoSize, float aspectRatio, float zoom, float nearZ, float farZ)
        {
            var zero = Vector3.Zero;
            var identity = Quaternion.Identity;

            csi_Atomic_Frustum_DefineOrtho(ref this, orthoSize, aspectRatio, zoom, nearZ, farZ, ref zero, ref identity);
        }

        /// <summary>
        /// Test if a point is inside or outside.
        /// </summary>
        public Intersection IsInside(Vector3 point)
        {
            for (uint i = 0; i < Constants.NUM_FRUSTUM_PLANES; ++i)
            {
                if (Planes[i].Distance(point) < 0.0f)
                    return Intersection.OUTSIDE;
            }

            return Intersection.INSIDE;
        }

        /// <summary>
        /// Test if a bounding box is inside, outside or intersects.
        /// </summary>
        public Intersection IsInside(BoundingBox box)
        {
            return csi_Atomic_Frustum_IsInside_BoundingBox(ref this, ref box);
        }

        /// <summary>
        /// Test if a bounding box is (partially) inside or outside.
        /// </summary>
        Intersection IsInsideFast(BoundingBox box)
        {
            return csi_Atomic_Frustum_IsInsideFast_BoundingBox(ref this, ref box);
        }

        /// <summary>
        /// Return distance of a point to the frustum, or 0 if inside.
        /// </summary>
        float Distance(Vector3 point)
        {
            return csi_Atomic_Frustum_Distance(ref this, ref point);
        }

        public Plane[] Planes
        {
            get
            {
                if (planes == null)
                {
                    planes = new Plane[Constants.NUM_FRUSTUM_PLANES];

                    for (uint i = 0; i < Constants.NUM_FRUSTUM_PLANES; i++)
                    {
                        planes[i] = new Plane();
                    }
                }

                return planes;
            }

        }

        public Vector3[] Vertices
        {
            get
            {
                if (vertices == null)
                {
                    vertices = new Vector3[Constants.NUM_FRUSTUM_VERTICES];

                    for (uint i = 0; i < Constants.NUM_FRUSTUM_VERTICES; i++)
                    {
                        vertices[i] = new Vector3();
                    }
                }

                return vertices;
            }

        }


        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();

            // ensure planes is initialized
            var oplanes = Planes;

            builder.Append("Frustum Planes:\n");

            for (uint i = 0; i < Constants.NUM_FRUSTUM_PLANES; i++)
            {
                builder.Append(String.Format("{0} : {1} - ", oplanes[i].Normal.ToString(), oplanes[i].D));
            }

            return builder.ToString();

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Frustum_Define(ref Frustum self, float fov, float aspectRatio, float zoom, float nearZ, float farZ, ref Vector3 position, ref Quaternion rotation);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Frustum_DefineOrtho(ref Frustum self, float orthoSize, float aspectRatio, float zoom, float nearZ, float farZ, ref Vector3 position, ref Quaternion rotation);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern Intersection csi_Atomic_Frustum_IsInside_Vector3(ref Frustum self, ref Vector3 point);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern Intersection csi_Atomic_Frustum_IsInside_BoundingBox(ref Frustum self, ref BoundingBox bbox);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern Intersection csi_Atomic_Frustum_IsInsideFast_BoundingBox(ref Frustum self, ref BoundingBox box);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern float csi_Atomic_Frustum_Distance(ref Frustum self, ref Vector3 point);

        // marking planes/vertices as private causes mono error loading type?
        [MarshalAs(UnmanagedType.ByValArray, ArraySubType = UnmanagedType.Struct, SizeConst = sizeOfPlaneArray)]
        internal Plane[] planes;

        [MarshalAs(UnmanagedType.ByValArray, ArraySubType = UnmanagedType.Struct, SizeConst = sizeOfVerticesArray)]
        internal Vector3[] vertices;

        private const int sizeOfPlaneArray = (int)(28 * Constants.NUM_FRUSTUM_PLANES);
        private const int sizeOfVerticesArray = (int)(12 * Constants.NUM_FRUSTUM_VERTICES);

    }
}
