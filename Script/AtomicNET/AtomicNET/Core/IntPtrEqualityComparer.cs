using System;
using System.Collections.Generic;

namespace AtomicEngine
{
    internal class IntPtrEqualityComparer : IEqualityComparer<IntPtr>
    {
        public static readonly IEqualityComparer<IntPtr> Instance = new IntPtrEqualityComparer();

        public bool Equals(IntPtr x, IntPtr y)
        {
            return x == y;
        }

        public int GetHashCode(IntPtr obj)
        {
            return obj.GetHashCode();
        }
    }
}
