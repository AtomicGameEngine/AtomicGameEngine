
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BiasParameters
    {
        public float ConstantBias;
        public float SlopeScaleBias;
        public float NormalOffset;

        public BiasParameters(float constantBias, float slopeScaleBias, float normalOffset = 0.0f)
        {
            ConstantBias = constantBias;
            SlopeScaleBias = slopeScaleBias;
            NormalOffset = normalOffset;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CascadeParameters
    {
        public float Split1, Split2, Split3, Split4;
        public float FadeStart;
        public float BiasAutoAdjust;

        public CascadeParameters(float split1, float split2, float split3, float split4, float fadeStart, float biasAutoAdjust = 1f)
        {
            Split1 = split1;
            Split2 = split2;
            Split3 = split3;
            Split4 = split4;
            FadeStart = fadeStart;
            BiasAutoAdjust = biasAutoAdjust;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FocusParameters
    {
        public byte Focus;
        public byte NonUniform;
        public byte AutoSize;
        public float Quantize;
        public float MinView;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Ray
    {
        public Vector3 Origin;
        public Vector3 Direction;

        public Ray(Vector3 origin, Vector3 direction)
        {
            Origin = origin;
            Direction = Vector3.Normalize(direction);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Ray))
                return false;

            return (this == (Ray)obj);
        }

        public static bool operator ==(Ray left, Ray right)
        {
            return ((left.Origin == right.Origin) && (left.Direction == right.Direction));
        }

        public static bool operator !=(Ray left, Ray right)
        {
            return ((left.Origin != right.Origin) || (left.Direction != right.Direction));
        }

        public Vector3 Project(Vector3 point)
        {
            var offset = point - Origin;
            return Origin + Vector3.Dot(offset, Direction) * Direction;
        }

        public override int GetHashCode()
        {
            return Origin.GetHashCode() + Direction.GetHashCode();
        }

        public float Distance(Vector3 point)
        {
            var projected = Project(point);
            return (point - projected).Length;
        }

        public Vector3 ClosestPoint(Ray otherRay)
        {
            var p13 = Origin - otherRay.Origin;
            var p43 = otherRay.Direction;
            Vector3 p21 = Direction;

            float d1343 = Vector3.Dot(p13, p43);
            float d4321 = Vector3.Dot(p43, p21);
            float d1321 = Vector3.Dot(p13, p21);
            float d4343 = Vector3.Dot(p43, p43);
            float d2121 = Vector3.Dot(p21, p21);

            float d = d2121 * d4343 - d4321 * d4321;
            if (Math.Abs(d) < float.Epsilon)
                return Origin;
            float n = d1343 * d4321 - d1321 * d4343;
            float a = n / d;

            return Origin + a * Direction;
        }

        public float HitDistance(Plane plane)
        {
            float d = Vector3.Dot(plane.Normal, Direction);
            if (Math.Abs(d) >= float.Epsilon)
            {
                float t = -(Vector3.Dot(plane.Normal, Origin) + plane.D) / d;
                if (t >= 0.0f)
                    return t;
                else
                    return float.PositiveInfinity;
            }
            else
                return float.PositiveInfinity;
        }

    }

    [StructLayout(LayoutKind.Sequential)]
    public struct WeakPtr
    {
        IntPtr ptr;
        IntPtr refCountPtr;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct AtomicString
    {
        public uint Length;
        public uint Capacity;
        public IntPtr Buffer;
    }

}
