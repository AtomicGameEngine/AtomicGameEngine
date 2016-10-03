
using System.Runtime.InteropServices;

namespace AtomicEngine
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct BoundingBox
    {
        public Vector3 Min;
        public float DummyMin;
        public Vector3 Max;
        public float DummyMax;

        public BoundingBox(float min, float max)
        {
            DummyMax = 0;
            DummyMin = 0;
            Min = new Vector3(min, min, min);
            Max = new Vector3(max, max, max);
        }

        public BoundingBox(Vector3 min, Vector3 max)
        {
            DummyMax = 0;
            DummyMin = 0;
            Min = min;
            Max = max;
        }

        public void Merge(Vector3 point)
        {
            if (point.X < Min.X)
                Min.X = point.X;
            if (point.Y < Min.Y)
                Min.Y = point.Y;
            if (point.Z < Min.Z)
                Min.Z = point.Z;
            if (point.X > Max.X)
                Max.X = point.X;
            if (point.Y > Max.Y)
                Max.Y = point.Y;
            if (point.Z > Max.Z)
                Max.Z = point.Z;
        }

        public void Merge(BoundingBox box)
        {
            if (box.Min.X < Min.X)
                Min.X = box.Min.X;
            if (box.Min.Y < Min.Y)
                Min.Y = box.Min.Y;
            if (box.Min.Z < Min.Z)
                Min.Z = box.Min.Z;
            if (box.Max.X > Max.X)
                Max.X = box.Max.X;
            if (box.Max.Y > Max.Y)
                Max.Y = box.Max.Y;
            if (box.Max.Z > Max.Z)
                Max.Z = box.Max.Z;
        }

        public bool Defined()
        {
            return Min.X != float.PositiveInfinity;
        }

        public Vector3 Center
        {
            get
            {
                return (Max + Min) * 0.5f;
            }
        }

        public Vector3 Size
        {
            get
            {
                return Max - Min;
            }
        }
        public Vector3 HalfSize
        {
            get
            {
                return (Max - Min) * 0.5f;
            }
        }

        public Intersection IsInside(Vector3 point)
        {
            if (point.X < Min.X || point.X > Max.X ||
                point.Y < Min.Y || point.Y > Max.Y ||
                point.Z < Min.Z || point.Z > Max.Z)
                return Intersection.OUTSIDE;
            return Intersection.INSIDE;
        }

        public Intersection IsInside(BoundingBox box)
        {
            if (box.Max.X < Min.X || box.Min.X > Max.X ||
                box.Max.Y < Min.Y || box.Min.Y > Max.Y ||
                box.Max.Z < Min.Z || box.Min.Z > Max.Z)
                return Intersection.OUTSIDE;
            else if (box.Min.X < Min.X || box.Max.X > Max.X ||
                     box.Min.Y < Min.Y || box.Max.Y > Max.Y ||
                     box.Min.Z < Min.Z || box.Max.Z > Max.Z)
                return Intersection.INTERSECTS;
            else
                return Intersection.INSIDE;
        }

        public Intersection IsInsideFast(BoundingBox box)
        {
            if (box.Max.X < Min.X || box.Min.X > Max.X ||
                box.Max.Y < Min.Y || box.Min.Y > Max.Y ||
                box.Max.Z < Min.Z || box.Min.Z > Max.Z)
                return Intersection.OUTSIDE;
            else
                return Intersection.INSIDE;
        }
    }

}