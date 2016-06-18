using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace AtomicEngine
{
	// Want to be able to inherit from CSComponent, ScriptObject, and the various UI classes
	// This means that the managed side needs to be kept alive as will have object state
	// and we might get these back from native code in a variety of ways

	// a inherited instance must have a GCHandle to keep it alive
	// it must also at some point along constructor chain create the backing native

	// can we detect a native class?  

	// we shouldn't go off IntPtr here, as a pointer could be reused native before recycles

	static class NativeCore
	{
		// given an existing instance classID, construct the managed instance, with downcast support (ask for Component, get StaticModel for example)
		public static Dictionary<IntPtr, Func<IntPtr, RefCounted>> nativeClassIDToManagedConstructor = new Dictionary<IntPtr, Func<IntPtr, RefCounted>>();

		// native engine types, instances of these types can be trivially recreated managed side
		private static Dictionary<Type, Type> nativeTypes = new Dictionary<Type, Type> ();

		// RefID -> WeakReference of managed instance
		public static Dictionary<uint, WeakReference> nativeLookup = new Dictionary<uint, WeakReference> ();


		public static bool GetNativeType (Type type)
		{
			return nativeTypes.ContainsKey (type);
		}
			
		public static void RegisterNativeType (Type type)
		{
			nativeTypes.Add (type, type);		
		}

		public static IntPtr RegisterNative (IntPtr native, RefCounted r)
		{
			r.nativeInstance = native;

			var w = new WeakReference (r);
			NativeCore.nativeLookup [r.RefID] = w;
			r.AddRef();

			return native;
		}

		public static void ReleaseExpiredNativeReferences()
		{
			List<uint> released = new List<uint> ();

			foreach(KeyValuePair<uint, WeakReference> entry in nativeLookup)
			{

				if (entry.Value.Target == null || !entry.Value.IsAlive) {										
					released.Add (entry.Key);

				} else {

				}

			}

			foreach (uint id in released) {

				// use safe release
				RefCounted.SafeReleaseRef (id);
				nativeLookup.Remove (id);
			}

		}

		// wraps an existing native instance, with downcast support
		public static T WrapNative<T> (IntPtr native) where T:RefCounted
		{
			if (native == IntPtr.Zero)
				return null;

			// instance id
			uint id = csb_Atomic_RefCounted_GetRefID (native);

			WeakReference w;

			// first see if we're already available
			if (nativeLookup.TryGetValue (id, out w)) {

				if (w.IsAlive) {

					// we're alive!
					return (T)w.Target; 

				} else {

					// we were seen before, but have since been GC'd, remove!
					nativeLookup.Remove (id);
				}
			}

			IntPtr classID = RefCounted.csb_Atomic_RefCounted_GetClassID (native);

			// and store, with downcast support for instance Component -> StaticModel
			// we never want to hit this path for script inherited natives

			RefCounted r = nativeClassIDToManagedConstructor[classID](native);
			w = new WeakReference (r);
			NativeCore.nativeLookup [id] = w;

			// store a ref, so native side will not be released while we still have a reference in managed code
			r.AddRef();

			return (T) r;

		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern uint csb_Atomic_RefCounted_GetRefID(IntPtr self);

	}

}

