using System;
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
    AtomicNETModule.Initialize();
    AtomicPlayer.PlayerModule.Initialize ();
    //initSubsystems();
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

public static partial class Constants
{
    public const string LIBNAME = "__Internal";
}

public partial class RefCounted
{

  public RefCounted()
  {
  }

  protected RefCounted (IntPtr native)
  {
    nativeInstance = native;
  }

  public IntPtr nativeInstance = IntPtr.Zero;

  [DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
  public static extern IntPtr csb_Atomic_RefCounted_GetClassID (IntPtr self);

}

public class InspectorAttribute : Attribute
{
  public InspectorAttribute(string DefaultValue = "")
  {
  }

  public string DefaultValue;
}


}
