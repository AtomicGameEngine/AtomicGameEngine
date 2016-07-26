using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using AtomicPlayer;

namespace AtomicEngine
{

    public static class AtomicNET
    {

        public static Context Context => context;

        public static T GetSubsystem<T>() where T : AObject
        {
            AObject subSystem = null;
            subSystems.TryGetValue(typeof(T), out subSystem);
            return (T)subSystem;
        }

        public static void RegisterSubsystem(String name, AObject instance = null)
        {
            if (instance != null)
            {
                subSystems[instance.GetType()] = instance;
                return;
            }

            var subsystem = AtomicNET.Context.GetSubsystem(name);

            if (subsystem == null)
            {
                throw new System.InvalidOperationException("AtomicNET.RegisterSubsystem - Attempting to register null subsystem");
            }

            subSystems[subsystem.GetType()] = subsystem;
        }

        public static uint StringToStringHash(string value)
        {
            return csb_Atomic_AtomicNET_StringToStringHash(value);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern uint csb_Atomic_AtomicNET_StringToStringHash(string name);

        public static void Initialize()
        {
            // Atomic Modules
            CoreModule.Initialize();
            MathModule.Initialize();
            EngineModule.Initialize();
            InputModule.Initialize();
            IOModule.Initialize();
            ResourceModule.Initialize();
            AudioModule.Initialize();
            GraphicsModule.Initialize();
            SceneModule.Initialize();
            Atomic2DModule.Initialize();
            Atomic3DModule.Initialize();
            NavigationModule.Initialize();
            NetworkModule.Initialize();
            PhysicsModule.Initialize();
            EnvironmentModule.Initialize();
            UIModule.Initialize();
            IPCModule.Initialize();
            AtomicAppModule.Initialize();

            AtomicNETScriptModule.Initialize();
            AtomicNETNativeModule.Initialize();

            PlayerModule.Initialize();            

            CoreDelegates delegates = new CoreDelegates();
            delegates.eventDispatch = NativeCore.EventDispatch;

            IntPtr coreptr = csb_Atomic_NETCore_Initialize(ref delegates);

            NETCore core = (coreptr == IntPtr.Zero ? null : NativeCore.WrapNative<NETCore>(coreptr));

            if (core != null)
                AtomicNET.RegisterSubsystem("NETCore", core);

            context = core.Context;

            NativeCore.Initialize();

            CSComponentCore.Initialize();

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csb_Atomic_NETCore_Initialize(ref CoreDelegates delegates);

        private static Context context;
        private static Dictionary<Type, AObject> subSystems = new Dictionary<Type, AObject>();

    }

}