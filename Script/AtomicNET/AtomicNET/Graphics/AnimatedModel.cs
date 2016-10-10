
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class AnimatedModel : StaticModel
    {

        public Skeleton Skeleton
        {
            get
            {
                return GetSkeleton();
            }
        }

        Skeleton GetSkeletonReturnValue;
        public Skeleton GetSkeleton()
        {
            if (GetSkeletonReturnValue == null)
                GetSkeletonReturnValue = new Skeleton(this, csi_Atomic_AnimatedModel_GetSkeleton(nativeInstance));

            return GetSkeletonReturnValue;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr csi_Atomic_AnimatedModel_GetSkeleton(IntPtr self);

    };

}

