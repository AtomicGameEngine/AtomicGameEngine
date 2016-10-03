//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "ScriptSystem.h"

#pragma once

namespace Atomic
{

class ScriptVector : public RefCounted
{
    ATOMIC_REFCOUNTED(ScriptVector)

public:

    ScriptVector() : RefCounted()
    {

    }

    virtual ~ScriptVector()
    {

    }

    RefCounted* At(unsigned index) const
    {
        if (index >= refVector_.Size())
            return 0;

        return refVector_[index];
    }

    void Push(RefCounted* refCounted)
    {
        // TODO: check null?

        refVector_.Push(SharedPtr<RefCounted>(refCounted));
    }

    void SetAt(unsigned index, RefCounted* refCounted)
    {
        refVector_[index] = refCounted;
    }

    void Insert(unsigned index, RefCounted* refCounted)
    {
        refVector_.Insert(index, SharedPtr<RefCounted>(refCounted));
    }

    void Erase(unsigned index)
    {
        refVector_.Erase(index);
    }

    bool Remove(RefCounted* refCounted)
    {
        return refVector_.Remove(SharedPtr<RefCounted>(refCounted));
    }

    void Clear()
    {
        refVector_.Clear();
    }

    unsigned GetSize() const
    {
        return refVector_.Size();
    }

    template <class T>
    bool AdaptFromVector(PODVector<T> vectorIn)
    {

        Vector<SharedPtr<RefCounted>> keepAlive = refVector_;

        refVector_.Clear();

        for (unsigned i = 0; i < vectorIn.Size(); i++)
        {
            refVector_.Push(SharedPtr<RefCounted>(vectorIn[i]));
        }

        return true;
    }

    template <class T>
    bool AdaptFromVector(Vector<T> vectorIn)
    {
        Vector<SharedPtr<RefCounted>> keepAlive = refVector_;

        refVector_.Clear();

        for (unsigned i = 0; i < vectorIn.Size(); i++)
        {
            refVector_.Push((T) vectorIn[i]);
        }

        return true;

    }

    template <class T>
    bool AdaptFromVector(Vector<WeakPtr<T>> vectorIn)
    {
        Vector<SharedPtr<RefCounted>> keepAlive = refVector_;

        refVector_.Clear();

        for (unsigned i = 0; i < vectorIn.Size(); i++)
        {
            refVector_.Push(SharedPtr<RefCounted>((RefCounted*) vectorIn[i]));
        }

        return true;

    }

    template <class T>
    bool AdaptFromVector(Vector<SharedPtr<T>> vectorIn)
    {
        Vector<SharedPtr<RefCounted>> keepAlive = refVector_;

        refVector_.Clear();

        for (unsigned i = 0; i < vectorIn.Size(); i++)
        {
            refVector_.Push(SharedPtr<RefCounted>((RefCounted*) vectorIn[i]));
        }

        return true;

    }

    template <class T>
    bool AdaptToVector(PODVector<T> vectorOut)
    {
        vectorOut.Clear();

        for (unsigned i = 0; i < refVector_.Size(); i++)
        {
            vectorOut.Push(static_cast<T>(refVector_[i].Get()));
        }

        return true;

    }

    template <class T>
    bool AdaptToVector(Vector<T> vectorOut)
    {
        vectorOut.Clear();

        for (unsigned i = 0; i < refVector_.Size(); i++)
        {
            vectorOut.Push((T) refVector_[i]);
        }

        return true;

    }

    template <class T>
    bool AdaptToVector(Vector<SharedPtr<T>> vectorOut)
    {
        vectorOut.Clear();

        for (unsigned i = 0; i < refVector_.Size(); i++)
        {
            vectorOut.Push(SharedPtr<T>((T) refVector_[i]));
        }

        return true;

    }

    template <class T>
    bool AdaptToVector(Vector<WeakPtr<T>> vectorOut)
    {
        vectorOut.Clear();

        for (unsigned i = 0; i < refVector_.Size(); i++)
        {
            vectorOut.Push(WeakPtr<T>((T) refVector_[i]));
        }

        return true;
    }


private:

    Vector<SharedPtr<RefCounted>> refVector_;

};

}
