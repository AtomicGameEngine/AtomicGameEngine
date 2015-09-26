
#include "CSComponent.h"
#include "AtomicSharp.h"


namespace Atomic
{

/// Register AtomicSharp library objects.
void ATOMIC_API RegisterAtomicSharpLibrary(Context* context);

WeakPtr<Context> AtomicSharp::csContext_;
WeakPtr<AtomicSharp> AtomicSharp::instance_;

AtomicSharp::AtomicSharp(Context* context) :
    Object(context)
{
    RegisterAtomicSharpLibrary(context_);

    assert(!instance_);
    instance_ = this;
    csContext_ = context;
}

AtomicSharp::~AtomicSharp()
{
    instance_ = NULL;
}


void RegisterAtomicSharpLibrary(Context* context)
{
    CSComponent::RegisterObject(context);
}


}

