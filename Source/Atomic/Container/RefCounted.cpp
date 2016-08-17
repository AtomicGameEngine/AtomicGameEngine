//
// Copyright (c) 2008-2016 the Urho3D project.
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

#include "../Precompiled.h"

#include "../Container/RefCounted.h"

#include "../DebugNew.h"

namespace Atomic
{

RefCounted::RefCounted() :
    refCount_(new RefCount()),
// ATOMIC BEGIN
    instantiationType_(INSTANTIATION_NATIVE),
    jsHeapPtr_(0)
// ATOMIC END
{
    // Hold a weak ref to self to avoid possible double delete of the refcount
    (refCount_->weakRefs_)++;
}

RefCounted::~RefCounted()
{
    assert(refCount_);
    assert(refCount_->refs_ == 0);
    assert(refCount_->weakRefs_ > 0);

    // Mark object as expired, release the self weak ref and delete the refcount if no other weak refs exist
    refCount_->refs_ = -1;
    (refCount_->weakRefs_)--;
    if (!refCount_->weakRefs_)
        delete refCount_;

    refCount_ = 0;
}

void RefCounted::AddRef()
{
    assert(refCount_->refs_ >= 0);
    (refCount_->refs_)++;

    if (jsHeapPtr_ && refCount_->refs_ == 2)
    {
        for (unsigned i = 0; i < refCountChangedFunctions_.Size(); i++)
        {
            refCountChangedFunctions_[i](this, 2);
        }
    }
}

void RefCounted::ReleaseRef()
{
    assert(refCount_->refs_ > 0);
    (refCount_->refs_)--;


    if (jsHeapPtr_ && refCount_->refs_ == 1)
    {
        for (unsigned i = 0; i < refCountChangedFunctions_.Size(); i++)
        {
            (refCount_->refs_)++;
            refCountChangedFunctions_[i](this, 1);
            if (refCount_->refs_ == 1)
            {
                refCount_->refs_ = 0;
                delete this;
                return;
            }
            (refCount_->refs_)--;
        }
    }

    if (!refCount_->refs_)
        delete this;
}

int RefCounted::Refs() const
{
    return refCount_->refs_;
}

int RefCounted::WeakRefs() const
{
    // Subtract one to not return the internally held reference
    return refCount_->weakRefs_ - 1;
}

// ATOMIC BEGIN

PODVector<RefCountChangedFunction> RefCounted::refCountChangedFunctions_;

void RefCounted::AddRefSilent()
{
    assert(refCount_->refs_ >= 0);
    (refCount_->refs_)++;
}

void RefCounted::AddRefCountChangedFunction(RefCountChangedFunction function)
{
    refCountChangedFunctions_.Push(function);
}

void RefCounted::RemoveRefCountChangedFunction(RefCountChangedFunction function)
{
    refCountChangedFunctions_.Remove(function);
}

// ATOMIC END

}
