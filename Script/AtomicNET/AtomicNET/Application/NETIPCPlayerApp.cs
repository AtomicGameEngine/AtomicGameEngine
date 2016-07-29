using System;
using System.Reflection;


namespace AtomicEngine
{
    public partial class PlayerApp : AppBase
    {
        protected static void ExecuteAtomicMain(string[] args)
        {
            var file = AtomicNET.Cache.GetFile("AtomicProject.dll");

            if (file == null)
                return;

            Assembly assembly = Assembly.LoadFrom(file.FullPath);

            if (assembly == null)
                return;

            Type atomicMainType = null;
            Type[] assemblyTypes = assembly.GetTypes();

            for (int j = 0; j < assemblyTypes.Length; j++)
            {
                if (assemblyTypes[j].Name == "AtomicMain")
                {
                    atomicMainType = assemblyTypes[j];
                    break;
                }
            }

            if (atomicMainType == null)
                return;

            Type[] mainParms = new Type[1] { typeof(string[]) };

            var method = atomicMainType.GetMethod("Main", BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic, Type.DefaultBinder, mainParms, null);

            if (method == null)
                return;

            Object[] parms = new Object[1] { args };
            method.Invoke(null, parms);

        }

    }

    public partial class NETIPCPlayerApp : IPCPlayerApp
    {

        public static NETIPCPlayerApp Create(string[] args, bool headless = false)
        {
            // Initialize AtomicNET
            AtomicNET.Initialize();

            var app = CreateInternal();

            app.Initialize();

            // TODO: Refactor these registrations
            AtomicNET.RegisterSubsystem("FileSystem");

            AtomicNET.RegisterSubsystem("Graphics");
            AtomicNET.RegisterSubsystem("Player");

            AtomicNET.RegisterSubsystem("Input");
            AtomicNET.RegisterSubsystem("Renderer");

            AtomicNET.RegisterSubsystem("ResourceCache");
            AtomicNET.Cache = AtomicNET.GetSubsystem<ResourceCache>();

            ExecuteAtomicMain(args);

            return app;
        }


    }

}
