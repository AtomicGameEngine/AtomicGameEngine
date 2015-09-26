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
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector4
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Vector2
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Quaternion
	{
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
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct BoundingBox
	{
	}

	[StructLayout (LayoutKind.Sequential, CharSet = CharSet.Ansi)]
	public struct Rect
	{
	}}

