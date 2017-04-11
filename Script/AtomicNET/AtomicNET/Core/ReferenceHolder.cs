using System;

namespace AtomicEngine
{
    internal class ReferenceHolder<T> where T : class
    {
        public ReferenceHolder(T obj, bool weak)
        {
            if (weak)
                WeakRef = new WeakReference<T>(obj);
            else
                StrongRef = obj;
        }

        public T StrongRef { get; private set; }
        public WeakReference<T> WeakRef { get; private set; }
        public bool IsWeak => WeakRef != null;

        public T Reference
        {
            get
            {
                if (StrongRef != null)
                    return StrongRef;

                T wr;
                WeakRef.TryGetTarget(out wr);
                return wr;
            }
        }

        /// <summary>
        /// Change Weak to Strong
        /// </summary>
        public bool MakeStrong()
        {
            if (StrongRef != null)
                return true;
            T strong = null;
            WeakRef?.TryGetTarget(out strong);

            StrongRef = strong;
            WeakRef = null;
            return StrongRef != null;
        }

        /// <summary>
        /// Change Strong to Weak
        /// </summary>
        public bool MakeWeak()
        {
            if (StrongRef != null)
            {
                WeakRef = new WeakReference<T>(StrongRef);
                StrongRef = null;
                return true;
            }
            return false;
        }
    }
}
