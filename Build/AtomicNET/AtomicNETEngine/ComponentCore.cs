using System;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.Loader;
using System.Runtime.InteropServices;

namespace AtomicEngine
{


public partial class CSComponent : ScriptComponent
{

  public virtual void Update(float timeStep)
  {

  }

}

public static class ComponentCore
{

  // holds a reference
  static Dictionary<IntPtr, CSComponent> liveComponents = new Dictionary<IntPtr, CSComponent>();

  public static void Update (float timeStep)
  {
      foreach (var c in liveComponents.Values)
      {
            c.Update(timeStep);
      }
  }

  public static IntPtr CSComponentCreate(string assemblyName, string classTypeName)
  {

    Assembly assembly = AssemblyLoadContext.Default.LoadFromAssemblyName(new AssemblyName(assemblyName));

    Type type = assembly.GetType("AtomicNETTest."  + classTypeName);

    // TODO: check type is derived from CSComponent

    var component = (CSComponent) Activator.CreateInstance(type);

    liveComponents[component.nativeInstance] = component;

    return component.nativeInstance;

  }

}

}
