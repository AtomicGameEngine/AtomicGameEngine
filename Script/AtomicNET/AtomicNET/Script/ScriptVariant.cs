

using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class ScriptVariant : RefCounted
    {
        public ScriptVariant(Vector2 value) :this()
        {
            SetVector2(value);
        }

        public ScriptVariant(Vector3 value) : this()
        {
            SetVector3(value);
        }

        public ScriptVariant(Color value) : this()
        {
            SetColor(value);
        }

        public static implicit operator ScriptVariant(Vector2 value)  // Vector2
        {
            return new ScriptVariant(value);
        }

        public static implicit operator ScriptVariant(Vector3 value)  // Vector2
        {
            return new ScriptVariant(value);
        }

        public static implicit operator ScriptVariant(Color value)  // Vector2
        {
            return new ScriptVariant(value);
        }
    }

}

