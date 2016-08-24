

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

		public void Push(T refCounted)
		{
			scriptVector.Push(refCounted);
		}

		public T At(uint index)
		{
			return (T)scriptVector.At(index);
		}


		public T this[int key]
		{
			get
			{
				return At((uint)key);
			}
		}

		public T this[uint key]
		{
			get
			{
				return At(key);
			}
		}



		public static implicit operator IntPtr(Vector<T> vector)
		{
			if (vector == null)
				return IntPtr.Zero;

			return vector.scriptVector.nativeInstance;
		}

		public ScriptVector GetScriptVector() { return scriptVector; }


		ScriptVector scriptVector = new ScriptVector();
    }
}

