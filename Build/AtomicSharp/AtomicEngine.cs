using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using AtomicPlayer;

namespace AtomicEngine
{

	public static class Atomic
	{
		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
		private static extern int atomicsharp_initialize ();

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
		private static extern bool atomicsharp_runframe ();

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern IntPtr csb_AtomicEngine_GetSubsystem(string name);
			

		static Atomic()
		{
            try { 
			    Initialize ();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
                throw;
            }
        }

		public static void Run()
		{
			
			while (Atomic.RunFrame ()) {


			}

		}

		public static bool RunFrame()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
			GC.Collect();				
			NativeCore.ReleaseExpiredNativeReferences ();

			return atomicsharp_runframe ();
		}

		public static void Initialize()
		{
			ContainerModule.Initialize ();
			CoreModule.Initialize ();
			IOModule.Initialize ();
			ResourceModule.Initialize ();
			GraphicsModule.Initialize ();
			SceneModule.Initialize ();		

			AtomicPlayer.PlayerModule.Initialize ();

			atomicsharp_initialize ();

			initSubsystems ();
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
			registerSubsystem (NativeCore.WrapNative<Player> (csb_AtomicEngine_GetSubsystem("Player")));	
		}

	}

	public static partial class Constants
	{
		public const string LIBNAME = "/Users/josh/Dev/atomic/AtomicGameEngineSharp-build/Source/AtomicSharp/AtomicSharp";
	}

	public partial class RefCounted
	{
		protected RefCounted (IntPtr native)
		{
			nativeInstance = native;
		}
			
		public IntPtr nativeInstance;

		static public void _AddRef(IntPtr native)
		{
			csb_Atomic_RefCounted_AddRef(native);
		}
			
		static public void _ReleaseRef(IntPtr native)
		{
			csb_Atomic_RefCounted_ReleaseRef(native);
		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		public static extern IntPtr csb_RefCounted_GetClassID (IntPtr self);	

	}

	public partial class Node : Animatable
	{
		public T GetComponent<T> (bool recursive  = false) where T:Component
		{
			return (T) GetComponent (typeof(T).Name, recursive);
		}
		
	}

	static class NativeCore
	{
		// given an existing instance classID, construct the managed instance, with downcast support (ask for Component, get StaticModel for example)
		public static Dictionary<IntPtr, Func<IntPtr, RefCounted>> nativeClassIDToManagedConstructor = new Dictionary<IntPtr, Func<IntPtr, RefCounted>>();

		public static Dictionary<IntPtr, WeakReference> nativeLookup = new Dictionary<IntPtr, WeakReference> ();

		public static IntPtr RegisterNative (IntPtr native, RefCounted r)
		{
			var w = new WeakReference (r);
			NativeCore.nativeLookup [native] = w;
			RefCounted._AddRef (native);
			return native;
		}

		public static void ReleaseExpiredNativeReferences()
		{
			List<IntPtr> released = new List<IntPtr> ();

			foreach(KeyValuePair<IntPtr, WeakReference> entry in nativeLookup)
			{

				if (entry.Value.Target == null || !entry.Value.IsAlive) {										
					released.Add (entry.Key);

				} else {
					
				}

			}

			foreach (IntPtr native in released) {
				RefCounted._ReleaseRef(native);
				nativeLookup.Remove (native);
			}

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
				}
			}

			IntPtr classID = RefCounted.csb_RefCounted_GetClassID (native);

			// and store, with downcast support for instance Component -> StaticModel
			w = new WeakReference (nativeClassIDToManagedConstructor[classID](native));
			NativeCore.nativeLookup [native] = w;

			// store a ref, so native side will not be released while we still have a reference in managed code
			RefCounted._AddRef (native);

			return (T) w.Target;

		}
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector3
	{
		public Vector3 (float x, float y, float z)
		{
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public override string ToString()
		{
			return x + ", " + y + ", " + z;
		}

		public float x;
		public float y;
		public float z;
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector4
	{
	}
		
	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector2
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Quaternion
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Color
	{
		public Color (float r, float g, float b, float a = 1.0f)
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = a;
		}

		public float r;
		public float g;
		public float b;
		public float a;

	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct IntRect
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct IntVector2
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct BoundingBox
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Rect
	{
	}


}
