
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public struct NativeBone
    {
        public AtomicString Name;

        public int NameHash;
        public uint ParentIndex;
        public Vector3 InitialPosition;
        public Quaternion InitialRotation;
        public Vector3 InitialScale;
        public Matrix3x4 OffsetMatrix;

        byte animated; //bool is not blittable.

        public byte CollisionMask;
        public float Radius;
        public BoundingBox BoundingBox;

        WeakPtr Node;

        // Accessors
        public bool Animated { get { return animated != 0; } set { animated = (byte)(value ? 1 : 0); } }
    }

    public unsafe class Bone
    {
        // Keeps object which holds bone alive
        readonly object objRefHolder;
        readonly NativeBone* b;

        public Bone(object objHolder, NativeBone* bone)
        {
            this.objRefHolder = objHolder;
            this.b = bone;
        }

        public AtomicString Name { get { return b->Name; } set { b->Name = value; } }

        public int NameHash { get { return b->NameHash; } set { b->NameHash = value; } }
        public uint ParentIndex { get { return b->ParentIndex; } set { b->ParentIndex = value; } }
        public Vector3 InitialPosition { get { return b->InitialPosition; } set { b->InitialPosition = value; } }
        public Quaternion InitialRotation { get { return b->InitialRotation; } set { b->InitialRotation = value; } }
        public Vector3 InitialScale { get { return b->InitialScale; } set { b->InitialScale = value; } }
        public Matrix3x4 OffsetMatrix { get { return b->OffsetMatrix; } set { b->OffsetMatrix = value; } }
        public bool Animated { get { return b->Animated; } set { b->Animated = value; } }
        public byte CollisionMask { get { return b->CollisionMask; } set { b->CollisionMask = value; } }
        public float Radius { get { return b->Radius; } set { b->Radius = value; } }
        public BoundingBox BoundingBox { get { return b->BoundingBox; } set { b->BoundingBox = value; } }
    }

    public unsafe class Skeleton
    {
        public Skeleton(RefCounted owner, IntPtr nativeSkeleton)
        {
            this.owner = owner;
            this.nativeSkeleton = nativeSkeleton;
        }

        /// Define from another skeleton.
        public void Define(Skeleton src)
        {
            csi_Atomic_Skeleton_Define(nativeSkeleton, src.nativeSkeleton);
        }
        /// Set root bone's index.
        public void SetRootBoneIndex(uint index)
        {
            csi_Atomic_Skeleton_SetRootBoneIndex(nativeSkeleton, index);
        }

        /// Clear bones.
        public void ClearBones()
        {
            csi_Atomic_Skeleton_ClearBones(nativeSkeleton);
        }

        /// Reset all animating bones to initial positions.
        public void Reset()
        {
            csi_Atomic_Skeleton_Reset(nativeSkeleton);
        }

        /// Return number of bones.
        public uint GetNumBones()
        {
            return csi_Atomic_Skeleton_GetNumBones(nativeSkeleton);
        }

        /// Return root bone.
        public Bone GetRootBone()
        {
            NativeBone* nativeBone = csi_Atomic_Skeleton_GetRootBone(nativeSkeleton);
            return new Bone(this, nativeBone);
        }

        /// Return bone by index.
        public Bone GetBone(uint index)
        {
            NativeBone* nativeBone = csi_Atomic_Skeleton_GetBone_ByIndex(nativeSkeleton, index);
            return new Bone(this, nativeBone);
        }

        /// Return bone by name.
        public Bone GetBone(string boneName)
        {
            NativeBone* nativeBone = csi_Atomic_Skeleton_GetBone_ByName(nativeSkeleton, boneName);
            return new Bone(this, nativeBone);
        }

        /// Reset all animating bones to initial positions without marking the nodes dirty. Requires the node dirtying to be performed later.
        public void ResetSilent()
        {
            csi_Atomic_Skeleton_ResetSilent(nativeSkeleton);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Skeleton_Define(IntPtr self, IntPtr other);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Skeleton_SetRootBoneIndex(IntPtr self, uint index);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Skeleton_Reset(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Skeleton_ResetSilent(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_Skeleton_ClearBones(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint csi_Atomic_Skeleton_GetNumBones(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern NativeBone* csi_Atomic_Skeleton_GetRootBone(IntPtr self);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern NativeBone* csi_Atomic_Skeleton_GetBone_ByIndex(IntPtr self, uint index);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern NativeBone* csi_Atomic_Skeleton_GetBone_ByName(IntPtr self, string name);

        RefCounted owner;
        IntPtr nativeSkeleton;

    }

}