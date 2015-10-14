
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

    [DllImport("kernel32.dll")]
    public static extern IntPtr GetModuleHandle(string lpModuleName);

    [DllImport("dl")]
    protected static extern IntPtr dlopen(string filename, int flags);

    protected override IntPtr LoadUnmanagedDll(String unmanagedDllName)
    {

      if (unmanagedDllName == "__Internal")
      {
        IntPtr result = GetModuleHandle(null);
        return result;
      }

      // do we need to walk paths here?
      Console.WriteLine("LoadUnmanagedDll: " + unmanagedDllName);
      return GetModuleHandle(unmanagedDllName); //RTLD_LAZY

      /*
      if (unmanagedDllName == "__Internal")
      {
        return dlopen(null, 1); //RTLD_LAZY
      }

      // do we need to walk paths here?
      Console.WriteLine("LoadUnmanagedDll: " + unmanagedDllName);
      return dlopen(unmanagedDllName, 1 ); //RTLD_LAZY
      */
    }

    public override Assembly LoadAssembly(AssemblyName assemblyName)
    {

      Console.WriteLine(assemblyName.Name);
      Assembly assembly = null;
      try {
            assembly = LoadFromAssemblyPath("C:\\Users\\Josh/Desktop\\" + assemblyName.Name + ".dll");
      } catch (Exception e)
      {
        Console.WriteLine(e.Message);
      }

      if (assembly == null)
        assembly = LoadFromAssemblyPath("C:\\Dev\\coreclr\\x64\\" + assemblyName.Name + ".dll");

      Console.WriteLine("Assembly: " + assembly);
      return assembly;

    }


}

}
