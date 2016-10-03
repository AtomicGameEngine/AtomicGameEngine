
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential)]
    public struct Matrix3x4
    {
        public float m00;
        public float m01;
        public float m02;
        public float m03;
        public float m10;
        public float m11;
        public float m12;
        public float m13;
        public float m20;
        public float m21;
        public float m22;
        public float m23;

        public Matrix3x4 Inverse()
        {
            float det = m00 * m11 * m22 +
                m10 * m21 * m02 +
                m20 * m01 * m12 -
                m20 * m11 * m02 -
                m10 * m01 * m22 -
                m00 * m21 * m12;

            float invDet = 1.0f / det;
            Matrix3x4 ret;

            ret.m00 = (m11 * m22 - m21 * m12) * invDet;
            ret.m01 = -(m01 * m22 - m21 * m02) * invDet;
            ret.m02 = (m01 * m12 - m11 * m02) * invDet;
            ret.m03 = -(m03 * ret.m00 + m13 * ret.m01 + m23 * ret.m02);
            ret.m10 = -(m10 * m22 - m20 * m12) * invDet;
            ret.m11 = (m00 * m22 - m20 * m02) * invDet;
            ret.m12 = -(m00 * m12 - m10 * m02) * invDet;
            ret.m13 = -(m03 * ret.m10 + m13 * ret.m11 + m23 * ret.m12);
            ret.m20 = (m10 * m21 - m20 * m11) * invDet;
            ret.m21 = -(m00 * m21 - m20 * m01) * invDet;
            ret.m22 = (m00 * m11 - m10 * m01) * invDet;
            ret.m23 = -(m03 * ret.m20 + m13 * ret.m21 + m23 * ret.m22);

            return ret;
        }

        public static Matrix4 operator *(Matrix4 left, Matrix3x4 rhs)
        {
            return new Matrix4(
                left.M11 * rhs.m00 + left.M12 * rhs.m10 + left.M13 * rhs.m20,
                left.M11 * rhs.m01 + left.M12 * rhs.m11 + left.M13 * rhs.m21,
                left.M11 * rhs.m02 + left.M12 * rhs.m12 + left.M13 * rhs.m22,
                left.M11 * rhs.m03 + left.M12 * rhs.m13 + left.M13 * rhs.m23 + left.M14,
                left.M21 * rhs.m00 + left.M22 * rhs.m10 + left.M23 * rhs.m20,
                left.M21 * rhs.m01 + left.M22 * rhs.m11 + left.M23 * rhs.m21,
                left.M21 * rhs.m02 + left.M22 * rhs.m12 + left.M23 * rhs.m22,
                left.M21 * rhs.m03 + left.M22 * rhs.m13 + left.M23 * rhs.m23 + left.M24,
                left.M31 * rhs.m00 + left.M32 * rhs.m10 + left.M33 * rhs.m20,
                left.M31 * rhs.m01 + left.M32 * rhs.m11 + left.M33 * rhs.m21,
                left.M31 * rhs.m02 + left.M32 * rhs.m12 + left.M33 * rhs.m22,
                left.M31 * rhs.m03 + left.M32 * rhs.m13 + left.M33 * rhs.m23 + left.M34,
                left.M41 * rhs.m00 + left.M42 * rhs.m10 + left.M43 * rhs.m20,
                left.M41 * rhs.m01 + left.M42 * rhs.m11 + left.M43 * rhs.m21,
                left.M41 * rhs.m02 + left.M42 * rhs.m12 + left.M43 * rhs.m22,
                left.M41 * rhs.m03 + left.M42 * rhs.m13 + left.M43 * rhs.m23 + left.M44
            );
        }
    }


}