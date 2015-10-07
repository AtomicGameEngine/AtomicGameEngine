
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Atomic.Bootstrap
{

// This must be in TPA list
public class AtomicLoadContext : LoadContext
{
    public static void Startup()
    {
      LoadContext.InitializeDefaultContext(new AtomicLoadContext());
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
      Console.WriteLine("LoadUnmanagedDll: " + unmanagedDllName);
      return dlopen(unmanagedDllName, 1 /*RTLD_LAZY*/);
    }

    public override Assembly LoadAssembly(AssemblyName assemblyName)
    {

      Console.WriteLine(assemblyName.Name);
      Assembly assembly = null;
      try {
            assembly = LoadFromAssemblyPath("/Users/josh/Desktop/" + assemblyName.Name + ".dll");
      } catch (Exception e)
      {
        Console.WriteLine(e.Message);
      }

      if (assembly == null)
        assembly = LoadFromAssemblyPath("/Users/josh/Desktop/OSX.x64.Debug/" + assemblyName.Name + ".dll");

      Console.WriteLine("Assembly: " + assembly);
      return assembly;

    }


}

}
