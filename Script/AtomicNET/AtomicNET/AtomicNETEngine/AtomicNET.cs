using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace AtomicEngine
{

public static class Atomic
{

  static public void Initialize()
  {
    ContainerModule.Initialize ();
    CoreModule.Initialize ();
    MathModule.Initialize ();
    EngineModule.Initialize ();
    InputModule.Initialize ();
    IOModule.Initialize ();
    ResourceModule.Initialize ();
    AudioModule.Initialize ();
    GraphicsModule.Initialize ();
    SceneModule.Initialize ();
    Atomic2DModule.Initialize ();
    Atomic3DModule.Initialize ();
    NavigationModule.Initialize ();
    NetworkModule.Initialize ();
    PhysicsModule.Initialize ();
    EnvironmentModule.Initialize ();
    UIModule.Initialize ();
    NETCoreModule.Initialize();
    NETScriptModule.Initialize();
    AtomicPlayer.PlayerModule.Initialize ();
  }

  static public void Start()
  {
    initSubsystems();
  }

  static public void ExecMainAssembly()
  {
      Assembly assembly = null;

      try
      {
        assembly = Assembly.Load("Main");
      }
      catch (Exception e)
      {
          Console.WriteLine("Failed to exec main assembly {0}", e.Message );
          return;
      }

      if (assembly == null)
        return;

      // a project assembly may define an AtomicMain which will be run
      foreach (var type in assembly.GetTypes())
      {
        MethodInfo main = type.GetMethod("Main", BindingFlags.Public | BindingFlags.Static);

        if (main == null || main.GetParameters().Length != 0)
          continue;

        main.Invoke(null, null);

      }
  }

  static Dictionary<Type, RefCounted> subSystems = new Dictionary<Type, RefCounted>();

  static private void registerSubsystem (RefCounted subsystem)
  {
    subSystems[subsystem.GetType()] = subsystem;
  }

  static public T GetSubsystem<T>() where T : RefCounted
  {
    return (T) subSystems [typeof(T)];
  }

  static private void initSubsystems()
  {
    registerSubsystem (NativeCore.WrapNative<Graphics> (csb_AtomicEngine_GetSubsystem("Graphics")));
    registerSubsystem (NativeCore.WrapNative<Renderer> (csb_AtomicEngine_GetSubsystem("Renderer")));
    registerSubsystem (NativeCore.WrapNative<ResourceCache> (csb_AtomicEngine_GetSubsystem("ResourceCache")));
  }

  [DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
  private static extern IntPtr csb_AtomicEngine_GetSubsystem(string name);

}
				

}
