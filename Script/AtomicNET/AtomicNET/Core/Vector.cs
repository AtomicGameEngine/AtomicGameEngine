using System;
using System.Collections;
using System.Collections.Generic;

namespace AtomicEngine
{
    // Type safe wrapper around ScriptVector
    public class Vector<T> : IList<T> where T : RefCounted
    {

        public uint Size
        {
            get
            {
                return scriptVector.GetSize();
            }
        }

        public void Push(T refCounted)
        {
            scriptVector.Push(refCounted);
        }

        public T At(uint index)
        {
            return (T)scriptVector.At(index);
        }


        public T this[int key]
        {
            get
            {
                return At((uint)key);
            }
            set
            {
                scriptVector.SetAt((uint)key, value);
            }
        }

        public T this[uint key]
        {
            get
            {
                return At(key);
            }
            set
            {
                scriptVector.SetAt(key, value);
            }
        }

        public static implicit operator IntPtr(Vector<T> vector)
        {
            if (vector == null)
                return IntPtr.Zero;

            return vector.scriptVector.nativeInstance;
        }

        public ScriptVector GetScriptVector() { return scriptVector; }

        ScriptVector scriptVector = new ScriptVector();

        #region IList implementation
        public int Count
        {
            get
            {
                return (int)scriptVector.GetSize();
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return false;
            }
        }

        public int IndexOf(T item)
        {
            uint size = scriptVector.Size;
            for (uint i = 0; i < size; ++i)
            {
                if (scriptVector.At(i) == item)
                    return (int)i;
            }
            return -1;
        }

        public void Insert(int index, T item)
        {
            scriptVector.Insert((uint)index, item);
        }

        public void RemoveAt(int index)
        {
            scriptVector.Erase((uint)index);
        }

        public void Add(T item)
        {
            scriptVector.Push(item);
        }

        public void Clear()
        {
            scriptVector.Clear();
        }

        public bool Contains(T item)
        {
            return IndexOf(item) != -1;
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            uint size = scriptVector.Size;
            for (uint i = 0; i < size; ++i)
            {
                array[arrayIndex] = (T)scriptVector.At(i);
                ++arrayIndex;
            }
        }

        public bool Remove(T item)
        {
            return scriptVector.Remove(item);
        }

        public IEnumerator<T> GetEnumerator()
        {
            uint size = scriptVector.Size;
            for (uint i = 0; i < size; ++i)
            {
                yield return (T)scriptVector.At(i);
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            uint size = scriptVector.Size;
            for (uint i = 0; i < size; ++i)
            {
                yield return scriptVector.At(i);
            }
        }
        #endregion
    }
}

