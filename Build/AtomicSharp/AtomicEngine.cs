using System;
using System.Collections.Generic;
using System.Reflection;
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

		public static void RegisterAssemblyComponents(Assembly assembly)
		{
			ComponentCore.RegisterAssemblyComponents (assembly);
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

			AtomicInterop.Initialize ();

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
		public RefCounted()
		{
			// if we're not a native type, we need to not be kept alive
			// as we need to save managed state, native types don't 
			// save local state
			if (!NativeCore.GetNativeType (this.GetType ())) {

				handle = GCHandle.Alloc (this);
			}
		
		}

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

		GCHandle handle;

	}

	public partial class AObject
	{
	}
		
	public partial class Node : Animatable
	{
		public T GetComponent<T> (bool recursive  = false) where T:Component
		{
			return (T) GetComponent (typeof(T).Name, recursive);
		}

		public T AddComponent<T> () where T:Component, new()
		{
			T component = new T ();
			AddComponent( component, 0, CreateMode.REPLICATED);
			return component;
		}

		
	}
		
}
