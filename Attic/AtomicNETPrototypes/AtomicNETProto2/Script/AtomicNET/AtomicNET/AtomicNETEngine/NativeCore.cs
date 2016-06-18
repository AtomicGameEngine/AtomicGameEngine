using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace AtomicEngine
{

  static class NativeCore
  {
    [DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern void csb_AtomicEngine_ReleaseRef (IntPtr refCounted);

    // given an existing instance classID, construct the managed instance, with downcast support (ask for Component, get StaticModel for example)
    public static Dictionary<IntPtr, Func<IntPtr, RefCounted>> nativeClassIDToManagedConstructor = new Dictionary<IntPtr, Func<IntPtr, RefCounted>>();

    // weak references here, hold a ref native side
    public static Dictionary<IntPtr, WeakReference> nativeLookup = new Dictionary<IntPtr, WeakReference> ();

    // native engine types, instances of these types can be trivially recreated managed side
    private static Dictionary<Type, Type> nativeTypes = new Dictionary<Type, Type> ();

    public static bool GetNativeType (Type type)
    {
      return nativeTypes.ContainsKey (type);
    }

    public static void RegisterNativeType (Type type)
    {
      nativeTypes.Add (type, type);
    }

    public static void ReleaseExpiredNativeReferences()
    {
      List<IntPtr> released = null;

      foreach(KeyValuePair<IntPtr, WeakReference> entry in nativeLookup)
      {

        if (entry.Value.Target == null || !entry.Value.IsAlive)
        {
          if (released == null)
            released = new List<IntPtr> ();

          released.Add (entry.Key);
        }

      }

      if (released == null)
        return;

      foreach (IntPtr native in released) {
        nativeLookup.Remove (native);
        csb_AtomicEngine_ReleaseRef(native);
        //Console.WriteLine("Released: " + test);
      }

    }

    static IntPtr test = IntPtr.Zero;

    // called by native code
    public static void NETUpdate (float timeStep)
    {

      GC.Collect();
      GC.WaitForPendingFinalizers();
      GC.Collect();
      ReleaseExpiredNativeReferences();

      ComponentCore.Update(timeStep);

    }

    // called by native code for every refcounted deleted
    public static void RefCountedDeleted (IntPtr native)
    {
      // native side deleted, immediate remove, if we have a script reference still this is an error

      WeakReference w;

      if (nativeLookup.TryGetValue (native, out w)) {

          if ( w != null && w.IsAlive) {
            throw new System.InvalidOperationException("Native Refcounted was deleted with live managed instance");
          }
      }

      nativeLookup.Remove(native);
    }

    // register a newly created native
    public static IntPtr RegisterNative (IntPtr native, RefCounted r)
    {
      r.nativeInstance = native;

      var w = new WeakReference (r);
      NativeCore.nativeLookup [native] = w;
      // keep native side alive
      r.AddRef();

      return native;
    }

    // wraps an existing native instance, with downcast support
    public static T WrapNative<T> (IntPtr native) where T:RefCounted
    {
      if (native == IntPtr.Zero)
        return null;

      WeakReference w;

      // first see if we're already available
      if (nativeLookup.TryGetValue (native, out w)) {

        if (w.IsAlive) {

          // we're alive!
          return (T)w.Target;

        } else {

          // we were seen before, but have since been GC'd, remove!
          nativeLookup.Remove (native);
          csb_AtomicEngine_ReleaseRef(native);
        }
      }

      IntPtr classID = RefCounted.csb_Atomic_RefCounted_GetClassID (native);

      // and store, with downcast support for instance Component -> StaticModel
      // we never want to hit this path for script inherited natives

      RefCounted r = nativeClassIDToManagedConstructor[classID](native);
      w = new WeakReference (r);
      NativeCore.nativeLookup [native] = w;

      // store a ref, so native side will not be released while we still have a reference in managed code
      r.AddRef();

      return (T) r;

    }

  }

}
