using System.Runtime.InteropServices;

namespace AtomicEngine
{


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
}