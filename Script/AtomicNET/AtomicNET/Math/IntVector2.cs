#region --- License ---
/*
Copyright (c) 2006 - 2008 The Open Toolkit library.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#endregion

using System;
using System.Runtime.InteropServices;
namespace AtomicEngine
{
	/// <summary>Represents a 2D vector using two single-precision inting-point numbers.</summary>
	/// <remarks>
	/// The IntVector2 structure is suitable for interoperation with unmanaged code requiring two consecutive ints.
	/// </remarks>
	[StructLayout(LayoutKind.Sequential)]
	public struct IntVector2 : IEquatable<IntVector2>
	{
		#region Fields

		/// <summary>
		/// The X component of the IntVector2.
		/// </summary>
		public int X;

		/// <summary>
		/// The Y component of the IntVector2.
		/// </summary>
		public int Y;

		#endregion

		#region Constructors

		/// <summary>
		/// Constructs a new IntVector2.
		/// </summary>
		/// <param name="x">The x coordinate of the net IntVector2.</param>
		/// <param name="y">The y coordinate of the net IntVector2.</param>
		public IntVector2(int x, int y)
		{
			X = x;
			Y = y;
		}

		/// <summary>
		/// Constructs a new IntVector2 from the given IntVector2.
		/// </summary>
		/// <param name="v">The IntVector2 to copy components from.</param>
		[Obsolete]
		public IntVector2(IntVector2 v)
		{
			X = v.X;
			Y = v.Y;
		}

		/// <summary>
		/// Constructs a new IntVector2 from the given Vector2.
		/// </summary>
		/// <param name="v">The IntVector2 to copy components from.</param>
		[Obsolete]
		public IntVector2(Vector2 v)
		{
			X = (int) v.X;
			Y = (int) v.Y;
		}

		#endregion

		#region Public Members

		#region Instance

		#region public void Add()

		/// <summary>Add the Vector passed as parameter to this instance.</summary>
		/// <param name="right">Right operand. This parameter is only read from.</param>
		[Obsolete("Use static Add() method instead.")]
		public void Add(IntVector2 right)
		{
			this.X += right.X;
			this.Y += right.Y;
		}

		/// <summary>Add the Vector passed as parameter to this instance.</summary>
		/// <param name="right">Right operand. This parameter is only read from.</param>
		[CLSCompliant(false)]
		[Obsolete("Use static Add() method instead.")]
		public void Add(ref IntVector2 right)
		{
			this.X += right.X;
			this.Y += right.Y;
		}

		#endregion public void Add()

		#region public void Sub()

		/// <summary>Subtract the Vector passed as parameter from this instance.</summary>
		/// <param name="right">Right operand. This parameter is only read from.</param>
		[Obsolete("Use static Subtract() method instead.")]
		public void Sub(IntVector2 right)
		{
			this.X -= right.X;
			this.Y -= right.Y;
		}

		/// <summary>Subtract the Vector passed as parameter from this instance.</summary>
		/// <param name="right">Right operand. This parameter is only read from.</param>
		[CLSCompliant(false)]
		[Obsolete("Use static Subtract() method instead.")]
		public void Sub(ref IntVector2 right)
		{
			this.X -= right.X;
			this.Y -= right.Y;
		}

		#endregion public void Sub()

		#region public void Mult()

		/// <summary>Multiply this instance by a scalar.</summary>
		/// <param name="f">Scalar operand.</param>
		[Obsolete("Use static Multiply() method instead.")]
		public void Mult(int f)
		{
			this.X *= f;
			this.Y *= f;
		}

		#endregion public void Mult()

		#region public void Div()

		/// <summary>Divide this instance by a scalar.</summary>
		/// <param name="f">Scalar operand.</param>
		[Obsolete("Use static Divide() method instead.")]
		public void Div(int f)
		{
			this.X =  X / f;
			this.Y = Y / f;
		}

		#endregion public void Div()

		#region public int Length

		/// <summary>
		/// Gets the length (magnitude) of the vector.
		/// </summary>
		/// <see cref="LengthFast"/>
		/// <seealso cref="LengthSquared"/>
		public int Length
		{
			get
			{
				return (int)System.Math.Sqrt(X * X + Y * Y);
			}
		}

		#endregion

		#region public int LengthFast

		/// <summary>
		/// Gets an approximation of the vector length (magnitude).
		/// </summary>
		/// <remarks>
		/// This property uses an approximation of the square root function to calculate vector magnitude, with
		/// an upper error bound of 0.001.
		/// </remarks>
		/// <see cref="Length"/>
		/// <seealso cref="LengthSquared"/>
		public int LengthFast
		{
			get
			{
				return (int) (1.0f / MathHelper.InverseSqrtFast(X * X + Y * Y));
			}
		}

		#endregion

		#region public int LengthSquared

		/// <summary>
		/// Gets the square of the vector length (magnitude).
		/// </summary>
		/// <remarks>
		/// This property avoids the costly square root operation required by the Length property. This makes it more suitable
		/// for comparisons.
		/// </remarks>
		/// <see cref="Length"/>
		/// <seealso cref="LengthFast"/>
		public int LengthSquared
		{
			get
			{
				return X * X + Y * Y;
			}
		}

		#endregion

		#region public IntVector2 PerpendicularRight

		/// <summary>
		/// Gets the perpendicular vector on the right side of this vector.
		/// </summary>
		public IntVector2 PerpendicularRight
		{
			get
			{
				return new IntVector2(Y, -X);
			}
		}

		#endregion

		#region public IntVector2 PerpendicularLeft

		/// <summary>
		/// Gets the perpendicular vector on the left side of this vector.
		/// </summary>
		public IntVector2 PerpendicularLeft
		{
			get
			{
				return new IntVector2(-Y, X);
			}
		}

		#endregion

		#region public void Normalize()

		/// <summary>
		/// Scales the IntVector2 to unit length.
		/// </summary>
		public void Normalize()
		{
			X =  X / this.Length;
			Y *= Y / this.Length;
		}

		#endregion

		#region public void NormalizeFast()

		/// <summary>
		/// Scales the IntVector2 to approximately unit length.
		/// </summary>
		public void NormalizeFast()
		{
			int scale = (int) MathHelper.InverseSqrtFast(X * X + Y * Y);
			X *= scale;
			Y *= scale;
		}

		#endregion

		#region public void Scale()

		/// <summary>
		/// Scales the current IntVector2 by the given amounts.
		/// </summary>
		/// <param name="sx">The scale of the X component.</param>
		/// <param name="sy">The scale of the Y component.</param>
		[Obsolete("Use static Multiply() method instead.")]
		public void Scale(int sx, int sy)
		{
			this.X = X * sx;
			this.Y = Y * sy;
		}

		/// <summary>Scales this instance by the given parameter.</summary>
		/// <param name="scale">The scaling of the individual components.</param>
		[Obsolete("Use static Multiply() method instead.")]
		public void Scale(IntVector2 scale)
		{
			this.X *= scale.X;
			this.Y *= scale.Y;
		}

		/// <summary>Scales this instance by the given parameter.</summary>
		/// <param name="scale">The scaling of the individual components.</param>
		[CLSCompliant(false)]
		[Obsolete("Use static Multiply() method instead.")]
		public void Scale(ref IntVector2 scale)
		{
			this.X *= scale.X;
			this.Y *= scale.Y;
		}

		#endregion public void Scale()

		#endregion

		#region Static

		#region Fields

		/// <summary>
		/// Defines a unit-length IntVector2 that points towards the X-axis.
		/// </summary>
		public static readonly IntVector2 UnitX = new IntVector2(1, 0);

		/// <summary>
		/// Defines a unit-length IntVector2 that points towards the Y-axis.
		/// </summary>
		public static readonly IntVector2 UnitY = new IntVector2(0, 1);

		/// <summary>
		/// Defines a zero-length IntVector2.
		/// </summary>
		public static readonly IntVector2 Zero = new IntVector2(0, 0);

		/// <summary>
		/// Defines an instance with all components set to 1.
		/// </summary>
		public static readonly IntVector2 One = new IntVector2(1, 1);

		/// <summary>
		/// Defines the size of the IntVector2 struct in bytes.
		/// </summary>
		public static readonly int SizeInBytes = Marshal.SizeOf(new IntVector2());

		#endregion

		#region Add

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="a">Left operand.</param>
		/// <param name="b">Right operand.</param>
		/// <returns>Result of operation.</returns>
		public static IntVector2 Add(IntVector2 a, IntVector2 b)
		{
			Add(ref a, ref b, out a);
			return a;
		}

		/// <summary>
		/// Adds two vectors.
		/// </summary>
		/// <param name="a">Left operand.</param>
		/// <param name="b">Right operand.</param>
		/// <param name="result">Result of operation.</param>
		public static void Add(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result = new IntVector2(a.X + b.X, a.Y + b.Y);
		}

		#endregion

		#region Subtract

		/// <summary>
		/// Subtract one Vector from another
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>Result of subtraction</returns>
		public static IntVector2 Subtract(IntVector2 a, IntVector2 b)
		{
			Subtract(ref a, ref b, out a);
			return a;
		}

		/// <summary>
		/// Subtract one Vector from another
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">Result of subtraction</param>
		public static void Subtract(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result = new IntVector2(a.X - b.X, a.Y - b.Y);
		}

		#endregion

		#region Multiply

		/// <summary>
		/// Multiplies a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Multiply(IntVector2 vector, int scale)
		{
			Multiply(ref vector, scale, out vector);
			return vector;
		}

		/// <summary>
		/// Multiplies a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Multiply(ref IntVector2 vector, int scale, out IntVector2 result)
		{
			result = new IntVector2(vector.X * scale, vector.Y * scale);
		}

		/// <summary>
		/// Multiplies a vector by the components a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Multiply(IntVector2 vector, IntVector2 scale)
		{
			Multiply(ref vector, ref scale, out vector);
			return vector;
		}

		/// <summary>
		/// Multiplies a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Multiply(ref IntVector2 vector, ref IntVector2 scale, out IntVector2 result)
		{
			result = new IntVector2(vector.X * scale.X, vector.Y * scale.Y);
		}

		#endregion

		#region Divide

		/// <summary>
		/// Divides a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Divide(IntVector2 vector, int scale)
		{
			Divide(ref vector, scale, out vector);
			return vector;
		}

		/// <summary>
		/// Divides a vector by a scalar.
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Divide(ref IntVector2 vector, int scale, out IntVector2 result)
		{
			Multiply(ref vector, 1 / scale, out result);
		}

		/// <summary>
		/// Divides a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of the operation.</returns>
		public static IntVector2 Divide(IntVector2 vector, IntVector2 scale)
		{
			Divide(ref vector, ref scale, out vector);
			return vector;
		}

		/// <summary>
		/// Divide a vector by the components of a vector (scale).
		/// </summary>
		/// <param name="vector">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <param name="result">Result of the operation.</param>
		public static void Divide(ref IntVector2 vector, ref IntVector2 scale, out IntVector2 result)
		{
			result = new IntVector2(vector.X / scale.X, vector.Y / scale.Y);
		}

		#endregion

		#region ComponentMin

		/// <summary>
		/// Calculate the component-wise minimum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>The component-wise minimum</returns>
		public static IntVector2 ComponentMin(IntVector2 a, IntVector2 b)
		{
			a.X = a.X < b.X ? a.X : b.X;
			a.Y = a.Y < b.Y ? a.Y : b.Y;
			return a;
		}

		/// <summary>
		/// Calculate the component-wise minimum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">The component-wise minimum</param>
		public static void ComponentMin(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result.X = a.X < b.X ? a.X : b.X;
			result.Y = a.Y < b.Y ? a.Y : b.Y;
		}

		#endregion

		#region ComponentMax

		/// <summary>
		/// Calculate the component-wise maximum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <returns>The component-wise maximum</returns>
		public static IntVector2 ComponentMax(IntVector2 a, IntVector2 b)
		{
			a.X = a.X > b.X ? a.X : b.X;
			a.Y = a.Y > b.Y ? a.Y : b.Y;
			return a;
		}

		/// <summary>
		/// Calculate the component-wise maximum of two vectors
		/// </summary>
		/// <param name="a">First operand</param>
		/// <param name="b">Second operand</param>
		/// <param name="result">The component-wise maximum</param>
		public static void ComponentMax(ref IntVector2 a, ref IntVector2 b, out IntVector2 result)
		{
			result.X = a.X > b.X ? a.X : b.X;
			result.Y = a.Y > b.Y ? a.Y : b.Y;
		}

		#endregion

		#region Min

		/// <summary>
		/// Returns the Vector3 with the minimum magnitude
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>The minimum Vector3</returns>
		public static IntVector2 Min(IntVector2 left, IntVector2 right)
		{
			return left.LengthSquared < right.LengthSquared ? left : right;
		}

		#endregion

		#region Max

		/// <summary>
		/// Returns the Vector3 with the minimum magnitude
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>The minimum Vector3</returns>
		public static IntVector2 Max(IntVector2 left, IntVector2 right)
		{
			return left.LengthSquared >= right.LengthSquared ? left : right;
		}

		#endregion

		#region Clamp

		/// <summary>
		/// Clamp a vector to the given minimum and maximum vectors
		/// </summary>
		/// <param name="vec">Input vector</param>
		/// <param name="min">Minimum vector</param>
		/// <param name="max">Maximum vector</param>
		/// <returns>The clamped vector</returns>
		public static IntVector2 Clamp(IntVector2 vec, IntVector2 min, IntVector2 max)
		{
			vec.X = vec.X < min.X ? min.X : vec.X > max.X ? max.X : vec.X;
			vec.Y = vec.Y < min.Y ? min.Y : vec.Y > max.Y ? max.Y : vec.Y;
			return vec;
		}

		/// <summary>
		/// Clamp a vector to the given minimum and maximum vectors
		/// </summary>
		/// <param name="vec">Input vector</param>
		/// <param name="min">Minimum vector</param>
		/// <param name="max">Maximum vector</param>
		/// <param name="result">The clamped vector</param>
		public static void Clamp(ref IntVector2 vec, ref IntVector2 min, ref IntVector2 max, out IntVector2 result)
		{
			result.X = vec.X < min.X ? min.X : vec.X > max.X ? max.X : vec.X;
			result.Y = vec.Y < min.Y ? min.Y : vec.Y > max.Y ? max.Y : vec.Y;
		}

		#endregion

		#region Normalize

		/// <summary>
		/// Scale a vector to unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <returns>The normalized vector</returns>
		public static IntVector2 Normalize(IntVector2 vec)
		{
			vec.X = vec.X / vec.Length;
			vec.Y = vec.Y / vec.Length;
			return vec;
		}

		/// <summary>
		/// Scale a vector to unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <param name="result">The normalized vector</param>
		public static void Normalize(ref IntVector2 vec, out IntVector2 result)
		{
			result.X = vec.X / vec.Length;
			result.Y = vec.Y / vec.Length;
		}

		#endregion

		#region NormalizeFast

		/// <summary>
		/// Scale a vector to approximately unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <returns>The normalized vector</returns>
		public static IntVector2 NormalizeFast(IntVector2 vec)
		{
			int scale = (int) MathHelper.InverseSqrtFast(vec.X * vec.X + vec.Y * vec.Y);
			vec.X *= scale;
			vec.Y *= scale;
			return vec;
		}

		/// <summary>
		/// Scale a vector to approximately unit length
		/// </summary>
		/// <param name="vec">The input vector</param>
		/// <param name="result">The normalized vector</param>
		public static void NormalizeFast(ref IntVector2 vec, out IntVector2 result)
		{
			int scale = (int) MathHelper.InverseSqrtFast(vec.X * vec.X + vec.Y * vec.Y);
			result.X = vec.X * scale;
			result.Y = vec.Y * scale;
		}

		#endregion

		#region Dot

		/// <summary>
		/// Calculate the dot (scalar) product of two vectors
		/// </summary>
		/// <param name="left">First operand</param>
		/// <param name="right">Second operand</param>
		/// <returns>The dot product of the two inputs</returns>
		public static int Dot(IntVector2 left, IntVector2 right)
		{
			return left.X * right.X + left.Y * right.Y;
		}

		/// <summary>
		/// Calculate the dot (scalar) product of two vectors
		/// </summary>
		/// <param name="left">First operand</param>
		/// <param name="right">Second operand</param>
		/// <param name="result">The dot product of the two inputs</param>
		public static void Dot(ref IntVector2 left, ref IntVector2 right, out int result)
		{
			result = left.X * right.X + left.Y * right.Y;
		}

		#endregion

		#region Lerp

		/// <summary>
		/// Returns a new Vector that is the linear blend of the 2 given Vectors
		/// </summary>
		/// <param name="a">First input vector</param>
		/// <param name="b">Second input vector</param>
		/// <param name="blend">The blend factor. a when blend=0, b when blend=1.</param>
		/// <returns>a when blend=0, b when blend=1, and a linear combination otherwise</returns>
		public static IntVector2 Lerp(IntVector2 a, IntVector2 b, int blend)
		{
			a.X = blend * (b.X - a.X) + a.X;
			a.Y = blend * (b.Y - a.Y) + a.Y;
			return a;
		}

		/// <summary>
		/// Returns a new Vector that is the linear blend of the 2 given Vectors
		/// </summary>
		/// <param name="a">First input vector</param>
		/// <param name="b">Second input vector</param>
		/// <param name="blend">The blend factor. a when blend=0, b when blend=1.</param>
		/// <param name="result">a when blend=0, b when blend=1, and a linear combination otherwise</param>
		public static void Lerp(ref IntVector2 a, ref IntVector2 b, int blend, out IntVector2 result)
		{
			result.X = blend * (b.X - a.X) + a.X;
			result.Y = blend * (b.Y - a.Y) + a.Y;
		}

		#endregion

		#region Barycentric

		/// <summary>
		/// Interpolate 3 Vectors using Barycentric coordinates
		/// </summary>
		/// <param name="a">First input Vector</param>
		/// <param name="b">Second input Vector</param>
		/// <param name="c">Third input Vector</param>
		/// <param name="u">First Barycentric Coordinate</param>
		/// <param name="v">Second Barycentric Coordinate</param>
		/// <returns>a when u=v=0, b when u=1,v=0, c when u=0,v=1, and a linear combination of a,b,c otherwise</returns>
		public static IntVector2 BaryCentric(IntVector2 a, IntVector2 b, IntVector2 c, int u, int v)
		{
			return a + u * (b - a) + v * (c - a);
		}

		/// <summary>Interpolate 3 Vectors using Barycentric coordinates</summary>
		/// <param name="a">First input Vector.</param>
		/// <param name="b">Second input Vector.</param>
		/// <param name="c">Third input Vector.</param>
		/// <param name="u">First Barycentric Coordinate.</param>
		/// <param name="v">Second Barycentric Coordinate.</param>
		/// <param name="result">Output Vector. a when u=v=0, b when u=1,v=0, c when u=0,v=1, and a linear combination of a,b,c otherwise</param>
		public static void BaryCentric(ref IntVector2 a, ref IntVector2 b, ref IntVector2 c, int u, int v, out IntVector2 result)
		{
			result = a; // copy

			IntVector2 temp = b; // copy
			Subtract(ref temp, ref a, out temp);
			Multiply(ref temp, u, out temp);
			Add(ref result, ref temp, out result);

			temp = c; // copy
			Subtract(ref temp, ref a, out temp);
			Multiply(ref temp, v, out temp);
			Add(ref result, ref temp, out result);
		}

		#endregion

		#endregion

		#region Operators

		/// <summary>
		/// Adds the specified instances.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>Result of addition.</returns>
		public static IntVector2 operator +(IntVector2 left, IntVector2 right)
		{
			left.X += right.X;
			left.Y += right.Y;
			return left;
		}

		/// <summary>
		/// Subtracts the specified instances.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>Result of subtraction.</returns>
		public static IntVector2 operator -(IntVector2 left, IntVector2 right)
		{
			left.X -= right.X;
			left.Y -= right.Y;
			return left;
		}

		/// <summary>
		/// Negates the specified instance.
		/// </summary>
		/// <param name="vec">Operand.</param>
		/// <returns>Result of negation.</returns>
		public static IntVector2 operator -(IntVector2 vec)
		{
			vec.X = -vec.X;
			vec.Y = -vec.Y;
			return vec;
		}

		/// <summary>
		/// Multiplies the specified instance by a scalar.
		/// </summary>
		/// <param name="vec">Left operand.</param>
		/// <param name="scale">Right operand.</param>
		/// <returns>Result of multiplication.</returns>
		public static IntVector2 operator *(IntVector2 vec, int scale)
		{
			vec.X *= scale;
			vec.Y *= scale;
			return vec;
		}

		/// <summary>
		/// Multiplies the specified instance by a scalar.
		/// </summary>
		/// <param name="scale">Left operand.</param>
		/// <param name="vec">Right operand.</param>
		/// <returns>Result of multiplication.</returns>
		public static IntVector2 operator *(int scale, IntVector2 vec)
		{
			vec.X *= scale;
			vec.Y *= scale;
			return vec;
		}

		/// <summary>
		/// Divides the specified instance by a scalar.
		/// </summary>
		/// <param name="vec">Left operand</param>
		/// <param name="scale">Right operand</param>
		/// <returns>Result of the division.</returns>
		public static IntVector2 operator /(IntVector2 vec, int scale)
		{
			vec.X = vec.X / scale;
			vec.Y = vec.Y / scale;
			return vec;
		}

		/// <summary>
		/// Compares the specified instances for equality.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>True if both instances are equal; false otherwise.</returns>
		public static bool operator ==(IntVector2 left, IntVector2 right)
		{
			return left.Equals(right);
		}

		/// <summary>
		/// Compares the specified instances for inequality.
		/// </summary>
		/// <param name="left">Left operand.</param>
		/// <param name="right">Right operand.</param>
		/// <returns>True if both instances are not equal; false otherwise.</returns>
		public static bool operator !=(IntVector2 left, IntVector2 right)
		{
			return !left.Equals(right);
		}

		#endregion

		#region Overrides

		#region public override string ToString()

		/// <summary>
		/// Returns a System.String that represents the current IntVector2.
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			return String.Format("({0}, {1})", X, Y);
		}

		#endregion

		#region public override int GetHashCode()

		/// <summary>
		/// Returns the hashcode for this instance.
		/// </summary>
		/// <returns>A System.Int32 containing the unique hashcode for this instance.</returns>
		public override int GetHashCode()
		{
			return X.GetHashCode() ^ Y.GetHashCode();
		}

		#endregion

		#region public override bool Equals(object obj)

		/// <summary>
		/// Indicates whether this instance and a specified object are equal.
		/// </summary>
		/// <param name="obj">The object to compare to.</param>
		/// <returns>True if the instances are equal; false otherwise.</returns>
		public override bool Equals(object obj)
		{
			if (!(obj is IntVector2))
				return false;

			return this.Equals((IntVector2)obj);
		}

		#endregion

		#endregion

		#endregion

		#region IEquatable<IntVector2> Members

		/// <summary>Indicates whether the current vector is equal to another vector.</summary>
		/// <param name="other">A vector to compare with this vector.</param>
		/// <returns>true if the current vector is equal to the vector parameter; otherwise, false.</returns>
		public bool Equals(IntVector2 other)
		{
			return
				X == other.X &&
				Y == other.Y;
		}

		#endregion

		public bool IsEmpty => X == 0 && Y == 0;
	}
}
