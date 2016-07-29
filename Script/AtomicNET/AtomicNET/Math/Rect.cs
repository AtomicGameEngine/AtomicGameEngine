using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct Rect
    {
        public Rect(float left, float top, float right, float bottom)
        {
            Min.X = left;
            Min.Y = top;
            Max.X = right;
            Max.Y = bottom;

            // for native
            Defined = true;
        }

        public float Left
        {
            get
            {
                return Min.X;
            }
            set
            {
                Min.X = value;
            }
        }

        public float Right
        {
            get
            {
                return Max.X;
            }
            set
            {
                Max.X = value;
            }
        }

        public float Top
        {
            get
            {
                return Min.Y;
            }
            set
            {
                Min.Y = value;
            }
        }

        public float Bottom
        {
            get
            {
                return Max.Y;
            }
            set
            {
                Max.Y = value;
            }
        }

        public float Width { get { return Max.X - Min.X; } }
        public float Height {  get { return Max.Y - Min.Y; } }

        /// Minimum vector.
        public Vector2 Min;
        
        /// Maximum vector.
         public Vector2 Max;

        /// Defined flag.
        bool Defined;

    }

}