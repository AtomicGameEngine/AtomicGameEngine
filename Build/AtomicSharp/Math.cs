using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
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


		/// Zero vector.
		static public readonly Vector3 Zero = new Vector3(0, 0, 0);
		/// (-1,0,0) vector.
		static public readonly Vector3 Left = new Vector3(-1, 0, 0);
		/// (1,0,0) vector.
		static public readonly Vector3 Right = new Vector3(1, 0, 0);
		/// (0,1,0) vector.
		static public readonly Vector3 Up = new Vector3(0, 1, 0);
		/// (0,-1,0) vector.
		static public readonly Vector3 Down = new Vector3(0, -1, 0);
		/// (0,0,1) vector.
		static public readonly Vector3 Forward = new Vector3(0, 0, 1);
		/// (0,0,-1) vector.
		static public readonly Vector3 Back = new Vector3(0, 0, -1);
		/// (1,1,1) vector.
		static public readonly Vector3 One = new Vector3(1, 1, 1);

	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector4
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector2
	{
		public Vector2 (float x, float y)
		{
			this.x = x;
			this.y = y;
		}
			
		public float x;
		public float y;

	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Quaternion
	{
		public Quaternion (float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			this.w = w;
			this.x = x;
			this.y = y;
			this.z = z;
		}

		public override string ToString()
		{
			return x + ", " + y + ", " + z;
		}

		public float w;
		public float x;
		public float y;
		public float z;
			
		static public readonly Quaternion Identity = new Quaternion();

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
		public int x;
		public int y;

		public IntVector2 (int x, int y)
		{
			this.x = x;
			this.y = y;
		}


		static public readonly IntVector2 Zero = new IntVector2(0, 0);

	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct BoundingBox
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Rect
	{
	}}

