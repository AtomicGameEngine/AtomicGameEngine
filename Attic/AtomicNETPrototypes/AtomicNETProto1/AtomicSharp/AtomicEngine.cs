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

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern uint csb_Atomic_StringToStringHash(string name);
					
		public static uint StringToStringHash(string value)
		{
			return csb_Atomic_StringToStringHash (value);
		}

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
			registerSubsystem (NativeCore.WrapNative<Graphics> (csb_AtomicEngine_GetSubsystem("Graphics")));	
			registerSubsystem (NativeCore.WrapNative<Renderer> (csb_AtomicEngine_GetSubsystem("Renderer")));	
			registerSubsystem (NativeCore.WrapNative<ResourceCache> (csb_AtomicEngine_GetSubsystem("ResourceCache")));	
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

		}

		public void AllocGCHandle()
		{
			// if we're not a native type, we need to not be kept alive
			// as we need to save managed state, native types don't 
			// save managed state and can be trivially regenerated
			// by creating a GCHandle this managed instance will
			// be used whenever we get a native ptr back from engine

			if (!handle.IsAllocated)
			{
				handle = GCHandle.Alloc (this);
			}
		}


		public void FreeGCHandle()
		{
			if (handle.IsAllocated)
			{
				handle.Free();
			}
		}

		protected RefCounted (IntPtr native)
		{
			nativeInstance = native;
		}
			
		public IntPtr nativeInstance;


		public static void SafeAddRef(uint id)
		{
			csb_Atomic_RefCounted_SafeAddRef (id);
		}

		public static void SafeReleaseRef(uint id)
		{
			csb_Atomic_RefCounted_SafeReleaseRef (id);
		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		public static extern void csb_Atomic_RefCounted_SafeAddRef (uint id);

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		public static extern void csb_Atomic_RefCounted_SafeReleaseRef (uint id);

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		public static extern IntPtr csb_Atomic_RefCounted_GetClassID (IntPtr self);

		GCHandle handle;

	}


	public partial class Component : Animatable
	{
		public void Destroy()
		{
			if (Node != null)
				Node.RemoveComponent (this);

			// if we're a CSComponent notify ComponentCore
			ComponentCore.DestroyComponent (this);

		}
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

	public class InspectorAttribute : Attribute
	{
		public InspectorAttribute(string defaultValue = "")
		{
			DefaultValue = defaultValue;
		}

		public string DefaultValue;
	}		
}
