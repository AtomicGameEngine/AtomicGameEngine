using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using AtomicPlayer;

namespace AtomicEngine
{

    public static class AtomicNET
    {

        public static Context Context => context;
        public static ResourceCache Cache;

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
            return csi_Atomic_AtomicNET_StringToStringHash(value);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern uint csi_Atomic_AtomicNET_StringToStringHash(string name);

        // static members so they don't get GC'd
        private static EventDispatchDelegate eventDispatchDelegate = NativeCore.EventDispatch;
        private static UpdateDispatchDelegate updateDispatchDelegate = NativeCore.UpdateDispatch;
        private static RefCountedDeletedDelegate refCountedDeletedDelegate = NativeCore.RefCountedDeleted;

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
            NavigationModule.Initialize();
            NetworkModule.Initialize();
            PhysicsModule.Initialize();
            EnvironmentModule.Initialize();
            UIModule.Initialize();

#if ATOMIC_DESKTOP
            IPCModule.Initialize();
#endif

            AtomicAppModule.Initialize();
            ScriptModule.Initialize();

            AtomicNETScriptModule.Initialize();
            AtomicNETNativeModule.Initialize();

            PlayerModule.Initialize();

            IntPtr coreptr = csi_Atomic_NETCore_Initialize(eventDispatchDelegate, updateDispatchDelegate, refCountedDeletedDelegate);

            NETCore core = (coreptr == IntPtr.Zero ? null : NativeCore.WrapNative<NETCore>(coreptr));

            if (core != null)
                AtomicNET.RegisterSubsystem("NETCore", core);

            context = core.Context;

            NativeCore.Initialize();
            CSComponentCore.Initialize();

#if ATOMIC_DESKTOP
            string[] arguments = Environment.GetCommandLineArgs();
            foreach (string arg in arguments)
                AppBase.AddArgument(arg);
#endif

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_NETCore_Initialize(EventDispatchDelegate eventDispatch, UpdateDispatchDelegate updateDispatch, RefCountedDeletedDelegate refCountedDeleted);

        private static Context context;
        private static Dictionary<Type, AObject> subSystems = new Dictionary<Type, AObject>();

    }

}
