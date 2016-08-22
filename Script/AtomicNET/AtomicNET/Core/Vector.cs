

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
	// Type safe wrapper around ScriptVector
    public class Vector<T> where T : RefCounted
    {

		public uint Size
		{
			get
			{
				return scriptVector.GetSize();
			}
		}

		public T At(uint index)
		{
			return (T) scriptVector.At(index);	
		}


		public static implicit operator IntPtr(Vector<T> vector)
		{
			if (vector == null)
				return IntPtr.Zero;

			return vector.scriptVector.nativeInstance;
		}


		ScriptVector scriptVector = new ScriptVector();
    }
}

