
using System;
using System.Linq;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Atomic.Bootstrap
{

// This must be in TPA list
public class AtomicLoadContext : LoadContext
{

    static List<string> assemblyLoadPaths = new List<string>();

    public static void Startup(string _assemblyLoadPaths)
    {
      LoadContext.InitializeDefaultContext(new AtomicLoadContext());
      assemblyLoadPaths = _assemblyLoadPaths.Split(';').ToList();
      //Console.WriteLine("Bootstrap Startup paths: {0}", _assemblyLoadPaths);
    }

    public static void AddAssemblyLoadPath(string path)
    {
      assemblyLoadPaths.Add(path);
    }

    [DllImport("kernel32.dll")]
    public static extern IntPtr GetModuleHandle(string lpModuleName);

    [DllImport("dl")]
    protected static extern IntPtr dlopen(string filename, int flags);

    protected override IntPtr LoadUnmanagedDll(String unmanagedDllName)
    {

      /*
      if (unmanagedDllName == "__Internal")
      {
        IntPtr result = GetModuleHandle(null);
        return result;
      }

      // do we need to walk paths here?
      Console.WriteLine("LoadUnmanagedDll: " + unmanagedDllName);
      return GetModuleHandle(unmanagedDllName); //RTLD_LAZY
      */


      if (unmanagedDllName == "__Internal")
      {
        IntPtr result = dlopen(null, 1); //RTLD_LAZY
        return result;
      }

      // do we need to walk paths here?
      Console.WriteLine("LoadUnmanagedDll: " + unmanagedDllName);
      return dlopen(unmanagedDllName, 1 ); //RTLD_LAZY

    }

    public override Assembly LoadAssembly(AssemblyName assemblyName)
    {

      Assembly assembly = null;

      foreach (var path in assemblyLoadPaths)
      {
        try
        {
            //Console.WriteLine("Assembly Load Attempt: {0}", path + assemblyName.Name + ".dll");
            assembly = LoadFromAssemblyPath(path + assemblyName.Name + ".dll");
            break;
        }
        catch //(Exception e)
        {
          //Console.WriteLine(e.Message);
        }

      }

      return assembly;

    }


}

}
