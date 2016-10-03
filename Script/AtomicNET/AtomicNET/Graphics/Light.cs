
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Light : Drawable
    {

        public BiasParameters ShadowBias
        {
            get
            {
                return GetShadowBias();
            }
            set
            {
                SetShadowBias(value);
            }
        }

        public CascadeParameters ShadowCascade
        {
            get
            {
                return GetShadowCascade();
            }
            set
            {
                SetShadowCascade(value);
            }
        }

        public FocusParameters ShadowFocus
        {
            get
            {
                return GetShadowFocus();
            }
            set
            {
                SetShadowFocus(value);
            }
        }

        /// Set shadow depth bias parameters.
        public void SetShadowBias(BiasParameters parameters)
        {
            csi_Atomic_Light_SetShadowBias(nativeInstance, ref parameters);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_SetShadowBias(IntPtr self, ref BiasParameters parameters);


        /// Set directional light cascaded shadow parameters.
        void SetShadowCascade(CascadeParameters parameters)
        {
            csi_Atomic_Light_SetShadowCascade(nativeInstance, ref parameters);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_SetShadowCascade(IntPtr self, ref CascadeParameters parameters);


        /// Set shadow map focusing parameters.
        void SetShadowFocus(FocusParameters parameters)
        {
            csi_Atomic_Light_SetShadowFocus(nativeInstance, ref parameters);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_SetShadowFocus(IntPtr self, ref FocusParameters parameters);


        /// Set shadow depth bias parameters.

        BiasParameters GetShadowBias()
        {
            BiasParameters biasParams = new BiasParameters();
            csi_Atomic_Light_GetShadowBias(nativeInstance, ref biasParams);
            return biasParams;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_GetShadowBias(IntPtr self, ref BiasParameters retValue);

        /// Set directional light cascaded shadow parameters.
        CascadeParameters GetShadowCascade()
        {
            CascadeParameters cascadeParams = new CascadeParameters();
            csi_Atomic_Light_GetShadowCascade(nativeInstance, ref cascadeParams);
            return cascadeParams;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_GetShadowCascade(IntPtr self, ref CascadeParameters retValue);

        /// Set shadow map focusing parameters.
        FocusParameters GetShadowFocus()
        {
            FocusParameters focusParams = new FocusParameters();
            csi_Atomic_Light_GetShadowFocus(nativeInstance, ref focusParams);
            return focusParams;

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void csi_Atomic_Light_GetShadowFocus(IntPtr self, ref FocusParameters retValue);

    };

}

