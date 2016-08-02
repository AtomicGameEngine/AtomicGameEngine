using System;
using System.IO;
using System.Reflection;

namespace AtomicEngine
{


    public partial class PlayerApp : AppBase
    {
        public static bool DeployedApp = false;

        static protected void RegisterSubsystems()
        {
            // TODO: Refactor these registrations
            AtomicNET.RegisterSubsystem("FileSystem");

            AtomicNET.RegisterSubsystem("Graphics");
            AtomicNET.RegisterSubsystem("Player");

            AtomicNET.RegisterSubsystem("Input");
            AtomicNET.RegisterSubsystem("Renderer");

            AtomicNET.RegisterSubsystem("ResourceCache");
            AtomicNET.Cache = AtomicNET.GetSubsystem<ResourceCache>();
        }

        protected static void ExecuteAtomicMain(string[] args)
        {
            String projectAssemblyPath;

            if (!DeployedApp)
            {
                var file = AtomicNET.Cache.GetFile("AtomicProject.dll");
                projectAssemblyPath = file.FullPath;
            }
            else
            {
                projectAssemblyPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + "/AtomicProject.dll";
            }
               
            Assembly assembly = Assembly.LoadFrom(projectAssemblyPath);

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


}