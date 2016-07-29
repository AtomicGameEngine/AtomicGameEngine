using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct IntRect
    {

        public IntRect(int left, int top, int right, int bottom)
        {
            this.Left = left;
            this.Top = top;
            this.Right = right;
            this.Bottom = bottom;
        }

        public int Width { get { return Right - Left; } }
        public int Height { get { return Bottom - Top; } }

        public void Inflate(int horizontalAmount, int verticalAmount)
        {
            Left -= horizontalAmount;
            Right += horizontalAmount;

            Top -= verticalAmount;
            Bottom += verticalAmount;

        }

        public bool Contains(Vector2 vector)
        {
            int x = (int)vector.X;
            int y = (int)vector.Y;

            if (x < Left || y < Top || x >= Right || y >= Bottom)
                return false;

            return true;
        }

        /// Left coordinate.
        public int Left;
        /// Top coordinate.
        public int Top;
        /// Right coordinate.
        public int Right;
        /// Bottom coordinate.
        public int Bottom;

    }

}