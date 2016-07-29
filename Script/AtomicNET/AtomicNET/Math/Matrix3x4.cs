
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct Matrix3x4
    {
        static public Matrix3x4 IDENTITY = CreateIdentity();

        static Matrix3x4 CreateIdentity()
        {
            var m = new Matrix3x4();
            m.m00_ = 1.0f;
            m.m01_ = 0.0f;
            m.m02_ = 0.0f;
            m.m03_ = 0.0f;
            m.m10_ = 0.0f;
            m.m11_ = 1.0f;
            m.m12_ = 0.0f;
            m.m13_ = 0.0f;
            m.m20_ = 0.0f;
            m.m21_ = 0.0f;
            m.m22_ = 1.0f;
            m.m23_ = 0.0f;
            return m;

        }

        float m00_;
        float m01_;
        float m02_;
        float m03_;
        float m10_;
        float m11_;
        float m12_;
        float m13_;
        float m20_;
        float m21_;
        float m22_;
        float m23_;

    }


}