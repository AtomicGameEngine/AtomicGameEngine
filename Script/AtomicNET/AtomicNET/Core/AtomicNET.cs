using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using AtomicPlayer;

namespace AtomicEngine
{

    public static class AtomicNET
    {
        public static Context Context => context;

        public static string GetCacheStatus() => NativeCore.GetCacheStatus();

        public static T GetSubsystem<T>() where T : AObject
        {
            AObject subsystem = null;
            var type = typeof(T);

            // See if already registered (or a managed subsystem which will only be in the dictionary)
            if (subSystems.TryGetValue(type, out subsystem))
            {
                return (T) subsystem;
            }

            // If we're a managed type, throw error
            if (!NativeCore.IsNativeType(type))
            {
                throw new System.InvalidOperationException($"AtomicNET.GetSubsystem<T> - Attempting to get null subsystem: {type.Name}");
            }

            // Look up possible native subsystem
            subsystem = AtomicNET.Context.GetSubsystem(type.Name);

            // If we didn't find one, this is an error
            if (subsystem == null)
            {
                throw new System.InvalidOperationException($"AtomicNET.GetSubsystem<T> - Attempting to get null subsystem: {type.Name}");
            }

            // register the subsystem
            RegisterSubsystem(subsystem);

            return (T)subsystem;
        }

        public static void RegisterSubsystem(AObject instance)
        {
            var type = instance.GetType();
            if (subSystems.ContainsKey(type))
            {
                Log.Error($"AtomicNET.RegisterSubsystem - Attempting to reregister subsystem: {type.Name}");
                return;
            }
            subSystems[instance.GetType()] = instance;
        }

        public static uint StringToStringHash(string value)
        {
            return csi_Atomic_AtomicNET_StringToStringHash(value);
        }

        /// <summary>
        ///  Runs a GC collection and waits for any finalizers
        /// </summary>
        public static void RunGC()
        {
            NativeCore.RunGC();
        }

        /// <summary>
        ///  Returns true if called on main engine thread, false if on another thread
        /// </summary>
        public static bool IsMainThread()
        {
            return csi_AtomicEngine_IsMainThread();
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern uint csi_Atomic_AtomicNET_StringToStringHash(string name);

        // static members so they don't get GC'd
        private static EventDispatchDelegate eventDispatchDelegate = NativeCore.EventDispatch;
        private static UpdateDispatchDelegate updateDispatchDelegate = NativeCore.UpdateDispatch;
        private static RefCountedDeletedDelegate refCountedDeletedDelegate = NativeCore.RefCountedDeleted;
        private static ThrowManagedExceptionDelegate throwManagedExceptionDelegate = NativeCore.ThrowManagedException;

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
            MetricsModule.Initialize();

            AtomicNETScriptModule.Initialize();
            AtomicNETNativeModule.Initialize();

            PlayerModule.Initialize();

            IntPtr coreptr = csi_Atomic_NETCore_Initialize(eventDispatchDelegate, updateDispatchDelegate, refCountedDeletedDelegate, throwManagedExceptionDelegate);

            NETCore core = (coreptr == IntPtr.Zero ? null : NativeCore.WrapNative<NETCore>(coreptr));

            if (core != null)
                AtomicNET.RegisterSubsystem(core);

            context = NETCore.Context;

            NativeCore.Initialize();
            CSComponentCore.Initialize();            

#if ATOMIC_DESKTOP
            string[] arguments = Environment.GetCommandLineArgs();
            foreach (string arg in arguments)
                AppBase.AddArgument(arg);
#endif

        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr csi_Atomic_NETCore_Initialize(EventDispatchDelegate eventDispatch, UpdateDispatchDelegate updateDispatch, RefCountedDeletedDelegate refCountedDeleted, ThrowManagedExceptionDelegate throwManagedException);

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool csi_AtomicEngine_IsMainThread();

        private static Context context;
        private static Dictionary<Type, AObject> subSystems = new Dictionary<Type, AObject>();

    }

}
