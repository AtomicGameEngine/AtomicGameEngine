using System.Runtime.InteropServices;

namespace AtomicEngine
{


    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct Color
    {
        public Color(float r, float g, float b, float a = 1.0f)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public uint ToUInt()
        {
            uint r = (uint)(R * 255.0f);
            uint g = (uint)(G * 255.0f);
            uint b = (uint)(B * 255.0f);
            uint a = (uint)(A * 255.0f);
            return (a << 24) | (b << 16) | (g << 8) | r;
        }



        public static readonly Color White = new Color(1, 1, 1, 1);
        public static readonly Color LightBlue = new Color(0.50f, 0.88f, 0.81f);
        public static readonly Color Transparent = new Color(0, 0, 0, 0);

        public static Color operator *(Color value, float scale)
        {
            return new Color((float)(value.R * scale), (float)(value.G * scale), (float)(value.B * scale), (float)(value.A * scale));
        }

        public static Color Lerp(Color value1, Color value2, float amount)
        {
            amount = MathHelper.Clamp(amount, 0, 1);
            return new Color(
                MathHelper.Lerp(value1.R, value2.R, amount),
                MathHelper.Lerp(value1.G, value2.G, amount),
                MathHelper.Lerp(value1.B, value2.B, amount),
                MathHelper.Lerp(value1.A, value2.A, amount));
        }


        public float R;
        public float G;
        public float B;
        public float A;

    }
}