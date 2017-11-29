using System;
using System.Collections;
using System.Collections.Generic;
namespace AtomicEngine
{

    public partial class FileNameIterator : IEnumerable<string>
    {
        public IEnumerator<string> GetEnumerator()
        {
            Reset();
            while (MoveNext())
            {
                yield return GetCurrent();
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            Reset();
            while (MoveNext())
            {
                yield return GetCurrent();
            }
        }
    }


}