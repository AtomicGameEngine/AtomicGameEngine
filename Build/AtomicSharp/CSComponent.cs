using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
	public class CSComponent : Component
	{
		public uint ManagedID;

		public CSComponent ()
		{	
			// component being created native side
			if (ComponentCore.CurrentCSComponentNativeInstance != IntPtr.Zero) {

				nativeInstance = ComponentCore.CurrentCSComponentNativeInstance;
				NativeCore.RegisterNative (nativeInstance, this);
				ComponentCore.CurrentCSComponentNativeInstance = IntPtr.Zero;
				
			} else {
				
				nativeInstance = NativeCore.RegisterNative (csb_Atomic_CSComponent_Constructor(), this);	
				ComponentCore.RegisterCSComponent (this);
			
			}

		}

		virtual public void Update(float timeStep)
		{
			
		}

		public void SetManagedID(uint id)
		{
			csb_Atomic_CSComponent_SetManagedID (nativeInstance, id);
		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern IntPtr csb_Atomic_CSComponent_Constructor();

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern void csb_Atomic_CSComponent_SetManagedID(IntPtr self, uint managedID);


	}
}

