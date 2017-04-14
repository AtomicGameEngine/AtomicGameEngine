
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class AnimatedModel : StaticModel
    {

        
        new public Model Model
        {
            get
            {
                return GetModel();
            }
            set
            {
                // AnimatedModel overloads StaticModel::SetModel(Model* model) with AnimatedModel::SetModel(Model*, bool createBones = false)
                // need to make sure we pick right overload, otherwise will call wrong method native side
                SetModel(value, true);
            }
        }

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

