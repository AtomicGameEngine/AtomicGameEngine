using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
	public class CSComponent : Component
	{
		public uint ManagedID;

		public CSComponent ()
		{	
			nativeInstance = NativeCore.RegisterNative (csb_Atomic_CSComponent_Constructor(), this);	
			ComponentCore.RegisterCSComponent (this);
		}

		virtual public void Start()
		{

		}

		virtual public void Update(float timeStep)
		{
			
		}
			
		public void SendEvent(string eventType, Dictionary<string, object> eventData = null)
		{

		}

		// function would be a C# method, and all classes other than component and uiwidget are sealed
		// so basically, not having inheritance means no local state for native classes
		// allowing inheritance, neans need to GCHandle the object as can have local state
		// and can't regenerate it, comes down to components and UI?
		public void SubscribeToEvent(AObject sender, string eventType, object function)
		{

		}

		public void SubscribeToEvent(string eventType, object function)
		{

		}

		void handleEvent(string eventType, Dictionary<uint, object> eventData)
		{

		}
			
		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern IntPtr csb_Atomic_CSComponent_Constructor();

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern void csb_Atomic_CSComponent_SetManagedID(IntPtr self, uint managedID);


	}
}

