using System.Runtime.InteropServices;

namespace AtomicEngine
{
    /// Custom geometry vertex.
    [StructLayout(LayoutKind.Sequential)]
    public struct CustomGeometryVertex
    {
        /// Position.
        public Vector3 position_;
        /// Normal.
        public Vector3 normal_;
        /// Color.
        public uint color_;
        /// Texture coordinates.
        public Vector2 texCoord_;
        /// Tangent.
        public Vector4 tangent_;
    };

}

