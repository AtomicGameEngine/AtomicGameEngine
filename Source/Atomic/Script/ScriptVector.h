
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

    unsigned GetSize() const
    {
        return refVector_.Size();
    }

    template <class T>
    bool AdaptToPODVector(PODVector<T> vectorOut)
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

    }

    template <class T>
    bool AdaptFromPODVector(PODVector<T> vectorIn)
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

    }


private:

    Vector<SharedPtr<RefCounted>> refVector_;

};

}
