using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{


	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct IntRect
	{
		/// Left coordinate.
    int left;
    /// Top coordinate.
    int top;
    /// Right coordinate.
    int right;
    /// Bottom coordinate.
    int bottom;

	}

}