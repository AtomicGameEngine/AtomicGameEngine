using System;
using System.Collections;
using System.Collections.Generic;

namespace AtomicEngine
{

    public partial class ResourceNameIterator : IEnumerator<string>, IEnumerable<string>
    {
        object IEnumerator.Current
        {
            get
            {
                return Current;
            }
        }

        public IEnumerator<string> GetEnumerator()
        {
            return this;
        }

        void IDisposable.Dispose() { }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return this;
        }
    }


}