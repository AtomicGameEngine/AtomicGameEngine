using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
	internal static class AtomicInterop
	{
		[UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		delegate void CSComponentCreateDelegate (string name, IntPtr nativeCSComponent);

		static void CSComponentCreate(string name, IntPtr nativeCSComponent)
		{
			ComponentCore.CreateCSComponent (name, nativeCSComponent);
		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern void csb_AtomicEngine_AtomicInterop_Set_CSComponentCreate(CSComponentCreateDelegate method);


		// ----

		[UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		delegate void CSComponentCallMethodDelegate (uint componentID, CSComponentMethod method, float value);

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern void csb_AtomicEngine_AtomicInterop_Set_CSComponentCallMethod(CSComponentCallMethodDelegate method);	

		static void CSComponentCallMethod(uint componentID, CSComponentMethod method, float value)
		{
			ComponentCore.CallComponentMethod (componentID, method, value);
		}

		public static void Initialize()
		{
			csb_AtomicEngine_AtomicInterop_Set_CSComponentCreate (CSComponentCreate);	
			csb_AtomicEngine_AtomicInterop_Set_CSComponentCallMethod (CSComponentCallMethod);
		}

	}
}

