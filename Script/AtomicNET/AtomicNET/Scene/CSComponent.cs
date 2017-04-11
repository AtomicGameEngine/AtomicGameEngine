using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    public partial class CSComponent : ScriptComponent
    {

        public override string GetTypeName()
        {
            return GetType().Name;
        }

        public bool Started
        {
            get { return started; }
        }

        internal bool started = false;

        // CSComponents can be instantiated from a serialized scene or from within managed code
        protected CSComponent()
        {
            if (nativeLoadOverrideValidate != IntPtr.Zero)
            {
                // When loading CSComponents from a scene, many issues circumvented by not allowing additional components
                // to be created (on the Node) during serialization, so this is an error state
                throw new InvalidOperationException($"CSComponent() - Recursive CSComponent instantiation in default constructor during load type: { GetType().Name} ");
            }

            // detect instantiation type
            InstantiationType itype = InstantiationType.INSTANTIATION_NATIVE;

            IntPtr ninstance = IntPtr.Zero;

            if (nativeLoadOverride == IntPtr.Zero)
            {
                // We are being "new'd" in managed code
                ninstance = csi_Atomic_CSComponent_Constructor();
                itype = InstantiationType.INSTANTIATION_NET;
            }
            else
            {
                // We are loading from a serialized CSComponent
                ninstance = nativeInstance = nativeLoadOverride;

                // validation bookkeeping
                nativeLoadOverrideValidate = nativeLoadOverride;
                nativeLoadOverride = IntPtr.Zero;
            }

            NativeCore.RegisterNative(ninstance, this, itype);


        }

        // Create an instance of a CSComponent derived class, given an existing native CSComponent
        // This happens when loading components from serializes scene/prefab assets
        internal static CSComponent LoadCreateInstance(Type type, IntPtr nativeCSComponent)
        {
            // initialize instantiation validation
            nativeLoadOverride = nativeCSComponent;
            nativeLoadOverrideValidate = IntPtr.Zero;

            var component = (CSComponent)Activator.CreateInstance(type);

            if (component == null)
            {
                throw new InvalidOperationException("CSComponent.CreateInstance - Unable to instantiate instance of type: " + type.Name);
            }

            // clear validation
            nativeLoadOverride = nativeLoadOverrideValidate = IntPtr.Zero;

            return component;
        }

        // When instantiating from a serialized scene, the native CSComponent instance will already exist
        // nativeLoadOverride stores the overriden native instance to use in the default CSComponent constructor
        internal static IntPtr nativeLoadOverride = IntPtr.Zero;
        // Check for whether additional CSComponents were created in default constructor during serialization, this is an error condition and will throw an exception
        internal static IntPtr nativeLoadOverrideValidate = IntPtr.Zero;

        // native constructor for CSComponent
        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_CSComponent_Constructor();


    }

}
