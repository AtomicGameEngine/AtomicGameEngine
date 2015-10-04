
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

// This must be in TPA list
public class AtomicLoadContext : AssemblyLoadContext
{

    public static void Startup()
    {
      AssemblyLoadContext.InitializeDefaultContext(new AtomicLoadContext());
      Console.WriteLine("Bootstrap Startup");
    }


    [DllImport("dl")]
    protected static extern IntPtr dlopen(string filename, int flags);

    protected override IntPtr LoadUnmanagedDll(String unmanagedDllName)
    {
      if (unmanagedDllName == "__Internal")
      {
        return dlopen(null, 1 /*RTLD_LAZY*/);
      }

      // do we need to walk paths here?
      return dlopen(unmanagedDllName, 1 /*RTLD_LAZY*/);
    }

    protected override Assembly Load(AssemblyName assemblyName)
    {

      Console.WriteLine(assemblyName.Name);
      Assembly assembly = null;
      try {
        assembly = LoadFromAssemblyPath("/Users/josh/Desktop/" + assemblyName.Name + ".dll");
      } catch (Exception e)
      {
        Console.WriteLine(e.Message);
      }
      Console.WriteLine("Assembly: " + assembly);
      return assembly;

    }

}
