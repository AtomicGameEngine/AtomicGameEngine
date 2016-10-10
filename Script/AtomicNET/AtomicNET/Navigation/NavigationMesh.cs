
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class NavigationMesh : Component
    {

        public bool FindPath(List<Vector3> results, Vector3 start, Vector3 end)
        {
            return FindPath(results, start, end, Vector3.One);

        }

        public bool FindPath(List<Vector3> results, Vector3 start, Vector3 end, Vector3 extents)
        {

            results.Clear();

            int count;
            IntPtr resultVector;

            var ptr = csi_Atomic_NavigationMesh_FindPath(nativeInstance, ref start, ref end, ref extents, out resultVector, out count);

            if (ptr == IntPtr.Zero)
            {
                return false;
            }

            int structSize = Marshal.SizeOf(typeof(Vector3));

            for (int i = 0; i < count; i++)
            {
                IntPtr data = new IntPtr(ptr.ToInt64() + structSize * i);
                Vector3 v3 = (Vector3)Marshal.PtrToStructure(data, typeof(Vector3));
                results.Add(v3);
            }

            csi_Atomic_NavigationMesh_FindPath_FreeResult(resultVector);

            return true;

        }

        // Any resultVector returned here, must be freed with csi_Atomic_Octree_Raycast_FreeResult
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_NavigationMesh_FindPath(IntPtr self, ref Vector3 start, ref Vector3 end, ref Vector3 extents, out IntPtr resultVector, out int count);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_NavigationMesh_FindPath_FreeResult(IntPtr resultVector);


    };

}

